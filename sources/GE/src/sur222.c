/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_pat_conloft                   File: sur222.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a conic lofting patch.       */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-11   Originally written                                 */
/*  1994-10-26           sur225 sur272                              */
/*  1995-03-07   nstart_add added, offset check                     */
/*  1995-06-23   Bug: Length of d2rdu2                              */
/*  1996-05-28   Deleted unused variables                           */
/*  1996-11-06   Debug                                              */
/*  1997-01-31   Johan's boat bug: Parameter for Coon's             */
/*               Unused parts deleted. Backup to bibliotek          */
/*  1997-02-09   Elimination of compilation warning (p_value= F_UNDE*/
/*  1997-03-05   Rotation surface. Call sur227 for p_flag= 3        */
/*  1997-04-20   Ruled    surface. Call sur228 for p_flag= 4        */
/*  1997-05-02   Sweep    surface. Call sur229 for p_flag= 5        */
/*  1998-01-09   Debug                                              */
/*  1999-11-24   Free source code modifications                     */
/*  2001-04-06   Check that patch id deined (p_flag != I_UNDEF)     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_conloft    Conic lofting evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short spipla();        /* Spine plane for input u value      */
static short intspi();        /* Intersects longitudinal curves     */
static short genpoi();        /* Calculate generatrix point         */
static short isov01();        /* Isoparametric V=0 or =1 and icase=1*/
/*                                                                  */
#ifdef DEBUG
static short pripat();        /* Printout of patch data             */
#endif
#ifdef DEBUGTILLF
static short initial();       /* Initialization of variables        */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat plane[4];          /* Spine plane for 3D curves      */
static DBfloat pval_plane[4];     /* Spine plane for 2D functions   */
static DBfloat outlimsparam[2];   /* Trimming parameter values lims */
static DBfloat outlimeparam[2];   /* Trimming parameter values lime */
static DBfloat outtansparam[2];   /* Trimming parameter values tans */
static DBfloat outtaneparam[2];   /* Trimming parameter values tane */
static DBfloat outlims[16];       /* Coord. & derivat. for lims     */
static DBfloat outlime[16];       /* Coord. & derivat. for lime     */
static DBfloat outtans[16];       /* Coord. & derivat. for tans     */
static DBfloat outtane[16];       /* Coord. & derivat. for tane     */
static DBfloat outmidc[16];       /* Coord. & derivat. for midc     */
static DBfloat outpval[16];       /* Coord. & derivat. for P fctn   */
static DBfloat outgener[16];      /* Coord. & derivat. for generatr.*/
static DBfloat outspine[16];      /* Coord. & derivat. for spine    */
static DBfloat epsilon;           /* Defines the epsilon plane      */
static DBfloat epsilon_p;         /* Epsilon in positive V direction*/
static DBfloat epsilon_n;         /* Epsilon in negative V direction*/
static DBfloat u_pat_e;           /* Parameter for epsilon Coons    */
static DBfloat v_pat_e;           /* Parameter for epsilon Coons    */
static DBfloat e_outlims[16];     /* Data for the epsilon plane     */
static DBfloat e_outlime[16];     /* Data for the epsilon plane     */
static DBfloat e_outtans[16];     /* Data for the epsilon plane     */
static DBfloat e_outtane[16];     /* Data for the epsilon plane     */
static DBfloat e_outmidc[16];     /* Data for the epsilon plane     */
static DBfloat e_outpval[16];     /* Data for the epsilon plane     */
static DBfloat r_x,r_y,r_z;       /* Coordinates          r(u)      */
static DBfloat u_x,u_y,u_z;       /* Tangent             dr/du      */
static DBfloat v_x,v_y,v_z;       /* Tangent             dr/dv      */
static DBfloat u2_x,u2_y,u2_z;    /* Second derivative  d2r/du2     */
static DBfloat v2_x,v2_y,v2_z;    /* Second derivative  d2r/dv2     */
static DBfloat uv_x,uv_y,uv_z;    /* Twist vector       d2r/dudv    */
static DBfloat m_outlims[16];     /* Data for the mid     plane     */
static DBfloat m_outlime[16];     /* Data for the mid     plane     */
static DBfloat m_outtans[16];     /* Data for the mid     plane     */
static DBfloat m_outtane[16];     /* Data for the mid     plane     */
static DBfloat m_outmidc[16];     /* Data for the mid     plane     */
static DBfloat m_outpval[16];     /* Data for the mid     plane     */
static DBSeg   g_u0;              /* Boundary segment U= 0.0        */
static DBSeg   g_um;              /* Middle   segment U= 0.5        */
static DBSeg   g_u1;              /* Boundary segment U= 1.0        */
static EVALS   xyz_coon;          /* Temporary for test             */
static DBSeg   rul_v0;            /* Ruling for Coons patch         */
static DBSeg   rul_v1;            /* Ruling for Coons patch         */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE107                  * Curve segment evaluation function       */
/* GE136                  * Rational cubic with intermediate point  */
/* GE133                  * Rational cubic with P Value             */
/* varkon_cur_coonscre3   * Create coons from 3 segments            */
/* varkon_pat_coonseval   * Evaluate Coons patch                    */
/* varkon_cur_reparam     * Reparam. a rational segment             */
/* varkon_pat_rotloft     * Rotation lofting surface                */
/* varkon_pat_rulloft     * Ruled    lofting surface                */
/* varkon_pat_sweeploft   * Sweep    lofting surface                */
/* varkon_normv           * Vector normalization                    */
/* varkon_erinit          * Initial. of error messages              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2413 = Offset curve segments not yet implemented               */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_conloft   */
/* SU2962 = sur222 Surface normal is a zero vector in u= , v=       */
/* SU2823 = sur222 Conic creation failed for mid pt ....            */
/* SU2882 = sur222 No spline plane intersects with .. is .. (not 1) */
/* SU2993 = Severe program error in varkon_pat_conloft (sur222).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_pat_conloft (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl,       /* Pointer to the conic lofting patch      */
   DBint  icase,         /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat u_pat,        /* Patch (local) U parameter value         */
   DBfloat v_pat,        /* Patch (local) V parameter value         */

   EVALS  *p_xyz )       /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   GMPATB patb;          /* Coons (epsilon) patch                   */
   short  e_case;        /* =1: u_pat =2: +epsilon =3: +epsilon/2   */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 Enter**varkon_pat_conloft p_patl %d u %f v %f icase %d\n",
                (int)p_patl,u_pat,v_pat,(short)icase);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 0. Call varkon_pat_rotloft (sur227) for p_flag= 3                */
/*    Call varkon_pat_rulloft (sur228) for p_flag= 4                */
/*                                                                 !*/

   if      ( p_patl->p_flag == 3 ) 
     {
     status= varkon_pat_rotloft
    (p_patl, icase, u_pat, v_pat, p_xyz);
     return(status);
     }

   if      ( p_patl->p_flag == 4 ) 
     {
     status= varkon_pat_rulloft
    (p_patl, icase, u_pat, v_pat, p_xyz);
     return(status);
     }

   if      ( p_patl->p_flag == 5 ) 
     {
     status= varkon_pat_sweeploft
    (p_patl, icase, u_pat, v_pat, p_xyz);
     return(status);
     }

   if      ( p_patl->p_flag == I_UNDEF ) 
     {
      sprintf(errbuf,"p_flag%%varkon_pat_conloft (sur222)");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial.                                                */
/*  Printout of conic lofting patch data        for DEBUG on.       */
/*  Call of pripat.                                                 */
/*  Let epsilon= 0.01 for the approximate derivative calculation    */
/*                                                                 !*/

    g_u0.c0x = 0.0;
    g_u0.c1x = 0.0;
    g_u0.c2x = 0.0;
    g_u0.c3x = 0.0;
    g_u0.c0y = 0.0;
    g_u0.c1y = 0.0;
    g_u0.c2y = 0.0;
    g_u0.c3y = 0.0;
    g_u0.c0z = 0.0;
    g_u0.c1z = 0.0;
    g_u0.c2z = 0.0;
    g_u0.c3z = 0.0;
    g_u0.c0  = 1.0;
    g_u0.c1  = 0.0;
    g_u0.c2  = 0.0;
    g_u0.c3  = 0.0;

    g_um     = g_u0;
    g_u1     = g_u0;

#ifdef DEBUGTILLF
   status=initial(p_patl,u_pat,v_pat,p_xyz); 
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }
   status=pripat (p_patl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* Let epsilon= +0.01 if input U < 0.99 else let epsilon= -0.01     */
/*                                                                 !*/

   if ( u_pat < 0.99 )
     {
     epsilon= 0.01;
     }
   else
     {
     epsilon=  -0.01;
     }

/*!                                                                 */
/* 2. Spine plane                                                   */
/*                                                                  */
/* Calculate the spine plane for the input u_pat value              */
/* Call of spipla.                       Error SU2973 for failure.  */
/* Spine coordinates and derivatives to output EVALS                */
/*                                                                 !*/
   status=spipla (p_patl,u_pat); 
   if (status<0) 
   {
#ifdef DEBUG     
   status=pripat (p_patl); 
#endif
   sprintf(errbuf,"spipla%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

   p_xyz->sp_x  = outspine[0];    /* Coordinates       for spine    */
   p_xyz->sp_y  = outspine[1];
   p_xyz->sp_z  = outspine[2]; 
   p_xyz->spt_x = outspine[3];    /* Tangent           for spine    */
   p_xyz->spt_y = outspine[4];
   p_xyz->spt_z = outspine[5];
   p_xyz->spt2_x= outspine[5];    /* Second derivative for spine    */
   p_xyz->spt2_y= outspine[7];
   p_xyz->spt2_z= outspine[8];

/*!                                                                 */
/* 3. Longitudinal intersection points                              */
/*                                                                  */
/* Make intersection with start limit curve, end limit curve,       */
/* start tangent curve, end tangent curve and P value curve         */
/* Call of intspi.                       Error SU2973 for failure.  */
/* (Call of isov01 for rcode= 2. If V=0 or =1 goto lcase  )         */
/*                                                                 !*/

   if  ( icase == 1)  
     {
     status=isov01 ( p_patl , v_pat ); 
     if (status == 0) 
        {
        goto  lcase;
        }
     }

   e_case  = 1;
   status=intspi (p_patl, e_case); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 intspi failed (e_case=1) u_pat %f\n",
                u_pat);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
#ifdef DEBUG     
   status=pripat (p_patl); 
#endif
   sprintf(errbuf,"intspi%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 3. Generatrix point                                              */
/*                                                                  */
/* Calculate the generatrix point for the input v_pat value         */
/* Call of genpoi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   e_case  = 1;
   status=genpoi (p_patl,v_pat,e_case); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 genpoi failed (e_case=1) u_pat %f v_pat %f\n",
                u_pat,v_pat);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
#ifdef DEBUG     
   status=pripat (p_patl); 
#endif
   sprintf(errbuf,"genpoi%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

   if  ( icase == 0 || icase == 2)  
     {
     r_x  = outgener[0];
     r_y  = outgener[1];
     r_z  = outgener[2];
     v_x  = outgener[3];
     v_y  = outgener[4];
     v_z  = outgener[5];
     v2_x = outgener[6];
     v2_y = outgener[7];
     v2_z = outgener[8];
     goto  lcase;
     }

/*!                                                                 */
/* 4. Spine plane for the epsilon plane                             */
/*                                                                  */
/* Calculate the spine plane for U= u_pat + epsilon                 */
/* Call of spipla.                       Error SU2973 for failure.  */
/*                                                                 !*/
   status=spipla (p_patl,u_pat+epsilon); 
   if (status<0) 
   {
#ifdef DEBUG     
   status=pripat (p_patl); 
#endif
   sprintf(errbuf,"spipla(epsilon)%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 5. Longitudinal intersection points for the epsilon plane        */
/*                                                                  */
/* Make intersection with start limit curve, end limit curve,       */
/* start tangent curve, end tangent curve and P value curve         */
/* Call of intspi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   e_case  = 2;
   status=intspi (p_patl, e_case); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 intspi failed (e_case=2) u_pat+epsilon %f\n",
                u_pat+epsilon);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
#ifdef DEBUG     
   status=pripat (p_patl); 
#endif
   sprintf(errbuf,"intspi(epsilon)%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 6. Generatrix point for the epsilon plane          for Debug On  */
/*                                                                  */
/* Calculate the generatrix point for the input v_pat value         */
/* Call of genpoi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   e_case  = 2;
   status=genpoi (p_patl,v_pat,e_case); 
   if (status<0) 
   {
   sprintf(errbuf,"genpoi%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 7. Spine plane for the mid     plane                             */
/*                                                                  */
/* Calculate the spine plane for U= u_pat+epsilon/2                 */
/* Call of spipla.                       Error SU2973 for failure.  */
/*                                                                 !*/

   status=spipla (p_patl,u_pat+epsilon/2.0); 
   if (status<0) 
   {
   sprintf(errbuf,"spipla(epsilon)%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 8. Longitudinal mid plane intersection points                    */
/*                                                                  */
/* Make intersection with start limit curve, end limit curve,       */
/* start tangent curve, end tangent curve and P value curve         */
/* Call of intspi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   e_case  = 3;
   status=intspi (p_patl, e_case); 
   if (status<0) 
   {
   sprintf(errbuf,"intspi(epsilon)%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 9. Generatrix point for the mid     plane          for Debug On  */
/*                                                                  */
/* Calculate the generatrix point for the input v_pat value         */
/* Call of genpoi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   e_case  = 3;
   status=genpoi (p_patl,v_pat,e_case); 
   if (status<0) 
   {
   sprintf(errbuf,"genpoi%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 12. Surface derivatives                                          */
/*                                                                  */
/* Create a Coons (epsilon) patch with curves g_u0, g_u1, g_um.     */
/* Call of varkon_pat_coonscre3 (sur272).                           */
/*                                                                 !*/

   if ( v_pat < 0.01 )
      {
      epsilon_n = 0.00;
      epsilon_p = 0.02;
      v_pat_e   = 0.0;
      }
   else if ( v_pat > 0.99 )
      {
      epsilon_n = 0.02;
      epsilon_p = 0.00;
      v_pat_e   = 1.0;
      }
   else
      {
      epsilon_n = 0.01;
      epsilon_p = 0.01;
      v_pat_e   = 0.5;
      }

   if  ( epsilon  > 0.0 )u_pat_e = 0.0;
   else                  u_pat_e = 1.0;

   status= varkon_pat_coonscre3
   ( &g_u0, v_pat-epsilon_n, v_pat+epsilon_p, 
     &g_u1, v_pat-epsilon_n, v_pat+epsilon_p, 
     &g_um, v_pat-epsilon_n, v_pat+epsilon_p,
       &rul_v0,&rul_v1, &patb);
   if (status<0) 
   {
#ifdef DEBUG     
   status=pripat (p_patl,u_pat, v_pat, p_xyz); 
#endif
   sprintf(errbuf,"sur272%%varkon_mbs_conloft (sur222)");
   return(varkon_erpush("SU2943",errbuf));
   }

/* Evaluate coordinates and derivatives in Coons patch              */
/* Call of varkon_pat_coonseval (sur225).                           */
/*                                                                 !*/

   status= varkon_pat_coonseval
   ( &patb , icase, u_pat_e , v_pat_e, &xyz_coon ); 
   if (status<0) 
   {
   sprintf(errbuf,"sur225%%varkon_mbs_conloft (sur222)");
   return(varkon_erpush("SU2943",errbuf));
   }

/* No division with epsilon_p+epsilon_n since the sections */
/* not are reparameterized                                 */
/* Reference: Faux & Pratt page 111                        */


    r_x= xyz_coon.r_x;
    r_y= xyz_coon.r_y;
    r_z= xyz_coon.r_z;

    u_x= xyz_coon.u_x/fabs(epsilon);
    u_y= xyz_coon.u_y/fabs(epsilon);
    u_z= xyz_coon.u_z/fabs(epsilon);

    v_x= xyz_coon.v_x;
    v_y= xyz_coon.v_y;
    v_z= xyz_coon.v_z;

    u2_x= xyz_coon.u2_x/fabs(epsilon)/fabs(epsilon);
    u2_y= xyz_coon.u2_y/fabs(epsilon)/fabs(epsilon);
    u2_z= xyz_coon.u2_z/fabs(epsilon)/fabs(epsilon);

    v2_x= xyz_coon.v2_x;
    v2_y= xyz_coon.v2_y;
    v2_z= xyz_coon.v2_z;

    uv_x= xyz_coon.uv_x/fabs(epsilon);
    uv_y= xyz_coon.uv_y/fabs(epsilon);
    uv_z= xyz_coon.uv_z/fabs(epsilon);

#ifdef DEBUG
/* Check that Coon's patch evaluation is OK */
   r_x = outgener[0];
   r_y = outgener[1];
   r_z = outgener[2];
   v_x = outgener[3];
   v_y = outgener[4];
   v_z = outgener[5];
   v2_x= outgener[6];
   v2_y= outgener[7];
   v2_z= outgener[8];
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 r_x %f r_y %f r_z %f g_u0\n",
   r_x,r_y,r_z);
  fprintf(dbgfil(SURPAC),
  "sur222 r_x %f r_y %f r_z %f Coons\n",
   xyz_coon.r_x,xyz_coon.r_y,xyz_coon.r_z);
  fprintf(dbgfil(SURPAC),
  "sur222 r(u,v) distance %25.10f \n",
   SQRT((xyz_coon.r_x-r_x)*(xyz_coon.r_x-r_x)+
        (xyz_coon.r_y-r_y)*(xyz_coon.r_y-r_y)+
        (xyz_coon.r_z-r_z)*(xyz_coon.r_z-r_z)));
  fprintf(dbgfil(SURPAC),
  "sur222 u_pat %f v_pat %f u_pat_e %f, v_pat_e %f\n",
         u_pat ,v_pat , u_pat_e , v_pat_e );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 v_x %f v_y %f v_z %f g_u0\n",
   v_x,v_y,v_z);
  fprintf(dbgfil(SURPAC),
  "sur222 v_x %f v_y %f v_z %f Coons\n",
   xyz_coon.v_x,xyz_coon.v_y,xyz_coon.v_z);
  fprintf(dbgfil(SURPAC),
  "sur222 drdv  deviation %25.10f\n",
   SQRT((xyz_coon.v_x-v_x)*(xyz_coon.v_x-v_x)+
        (xyz_coon.v_y-v_y)*(xyz_coon.v_y-v_y)+
        (xyz_coon.v_z-v_z)*(xyz_coon.v_z-v_z)));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 v2_x %f v2_y %f v2_z %f g_u0\n",
   v2_x,v2_y,v2_z);
  fprintf(dbgfil(SURPAC),
  "sur222 v2_x %f v2_y %f v2_z %f Coons\n",
   xyz_coon.v2_x,xyz_coon.v2_y,xyz_coon.v2_z);
  fprintf(dbgfil(SURPAC),
  "sur222 d2rdv2  deviation %25.10f\n",
   SQRT((xyz_coon.v2_x-v2_x)*(xyz_coon.v2_x-v2_x)+
        (xyz_coon.v2_y-v2_y)*(xyz_coon.v2_y-v2_y)+
        (xyz_coon.v2_z-v2_z)*(xyz_coon.v2_z-v2_z)));
  }
#endif

/*!                                                                 */
/* 11. Calculated data to output variables                          */
/*                                                                  */
lcase:  /* Label for icase                                          */
/*                                                                  */
/* Coordinates, derivatives and curvature to p_xyz                  */
/*                                                                 !*/

    p_xyz->r_x= r_x;
    p_xyz->r_y= r_y;
    p_xyz->r_z= r_z;

    p_xyz->u_x= u_x;
    p_xyz->u_y= u_y;
    p_xyz->u_z= u_z;

    p_xyz->v_x= v_x;
    p_xyz->v_y= v_y;
    p_xyz->v_z= v_z;

    p_xyz->u2_x= u2_x;
    p_xyz->u2_y= u2_y;
    p_xyz->u2_z= u2_z;

    p_xyz->v2_x= v2_x;
    p_xyz->v2_y= v2_y;
    p_xyz->v2_z= v2_z;

    p_xyz->uv_x= uv_x;
    p_xyz->uv_y= uv_y;
    p_xyz->uv_z= uv_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur222 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur222 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fprintf(dbgfil(SURPAC),
  "sur222 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur222 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur222 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 Exit *** varkon_pat_conloft x= %8.2f y= %8.2f z= %8.2f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef DEBUGTILLF 
/*!********* Internal ** function **Defined*Only*For*Debug*On********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF.                            */
/* Printout also of the conic lofting patch data.                   */

   static short initial(p_patl,u_pat,v_pat,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur222 *** initial: p_xyz= %d\n", (int)p_xyz);
  }

/*!                                                                 */
/* 1. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   

    r_x= F_UNDEF;   
    r_y= F_UNDEF;   
    r_z= F_UNDEF;   

/*  Tangent             dr/du                                       */
    p_xyz->u_x=  -F_UNDEF;   
    p_xyz->u_y=   F_UNDEF;   
    p_xyz->u_z=   F_UNDEF;   

    u_x=  -F_UNDEF;   
    u_y=   F_UNDEF;   
    u_z=   F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

    v_x= F_UNDEF;   
    v_y= F_UNDEF;   
    v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

    u2_x= F_UNDEF;   
    u2_y= F_UNDEF;   
    u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

    v2_x= F_UNDEF;   
    v2_y= F_UNDEF;   
    v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

    uv_x= F_UNDEF;   
    uv_y= F_UNDEF;   
    uv_z= F_UNDEF;   

/*  Surface normal       r(u)                                       */
    p_xyz->n_x= F_UNDEF;   
    p_xyz->n_y= F_UNDEF;   
    p_xyz->n_z= F_UNDEF;   

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= F_UNDEF;   
    p_xyz->nu_y= F_UNDEF;   
    p_xyz->nu_z= F_UNDEF;   

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= F_UNDEF;   
    p_xyz->nv_y= F_UNDEF;   
    p_xyz->nv_z= F_UNDEF;   


/*  Maximum and minimum principal curvature                          */
    p_xyz->kmax= F_UNDEF;   
    p_xyz->kmin= F_UNDEF;   

/*  Direction in R*3 for kmax                                        */
    p_xyz->kmax_x= F_UNDEF;   
    p_xyz->kmax_y= F_UNDEF;   
    p_xyz->kmax_z= F_UNDEF;   

/*  Direction in R*3 for kmin                                        */
    p_xyz->kmin_x= F_UNDEF;   
    p_xyz->kmin_y= F_UNDEF;   
    p_xyz->kmin_z= F_UNDEF;   

/*  Direction in u,v for kmax                                       */
    p_xyz->kmax_u= F_UNDEF;   
    p_xyz->kmax_v= F_UNDEF;   

/*  Direction in u,v for kmin                                       */
    p_xyz->kmin_u= F_UNDEF;   
    p_xyz->kmin_v= F_UNDEF;   

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif


#ifdef DEBUG
/*!********* Internal ** function **Defined*Only*For*Debug*On********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of the conic lofting patch data.                        */

   static short pripat (p_patl)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
if ( dbglev(SURPAC) == 1 )    /* Note: if statement for whole fctn  */
  {
/* Algorithm                                                        */
/* =========                                                        */

/* 2. Initiations                                                   */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve (scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Patch address                                                 */
/*                                                                 !*/
fprintf(dbgfil(SURPAC),
"sur222 *** pripat*Printout of patch data for p_patl= %d\n", (int)p_patl);

/*!                                                                 */
/* 3. Spine data                                                    */
/*                                                                 !*/

fprintf(dbgfil(SURPAC),
"sur222 Spine addresses &p_patl->spine= %d &p_patl->spine.c0y= %d\n"
     ,(int)&p_patl->spine,  (int)&p_patl->spine.c0y);

fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->spine.c0x,p_patl->spine.c1x,
        p_patl->spine.c2x,p_patl->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->spine.c0y,p_patl->spine.c1y,
        p_patl->spine.c2y,p_patl->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->spine.c0z,p_patl->spine.c1z,
        p_patl->spine.c2z,p_patl->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->spine.c0 ,p_patl->spine.c1 ,
        p_patl->spine.c2 ,p_patl->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->spine.ofs,p_patl->spine.typ,p_patl->spine.sl );
fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lims.c0x,p_patl->lims.c1x,
        p_patl->lims.c2x,p_patl->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lims.c0y,p_patl->lims.c1y,
        p_patl->lims.c2y,p_patl->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lims.c0z,p_patl->lims.c1z,
        p_patl->lims.c2z,p_patl->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lims.c0 ,p_patl->lims.c1 ,
        p_patl->lims.c2 ,p_patl->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->lims.ofs,p_patl->lims.typ,p_patl->lims.sl );
fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->lime.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lime.c0x,p_patl->lime.c1x,
        p_patl->lime.c2x,p_patl->lime.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lime.c0y,p_patl->lime.c1y,
        p_patl->lime.c2y,p_patl->lime.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lime.c0z,p_patl->lime.c1z,
        p_patl->lime.c2z,p_patl->lime.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lime.c0 ,p_patl->lime.c1 ,
        p_patl->lime.c2 ,p_patl->lime.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->lime.ofs,p_patl->lime.typ,p_patl->lime.sl );
fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->tans.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tans.c0x,p_patl->tans.c1x,
        p_patl->tans.c2x,p_patl->tans.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tans.c0y,p_patl->tans.c1y,
        p_patl->tans.c2y,p_patl->tans.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tans.c0z,p_patl->tans.c1z,
        p_patl->tans.c2z,p_patl->tans.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tans.c0 ,p_patl->tans.c1 ,
        p_patl->tans.c2 ,p_patl->tans.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->tans.ofs,p_patl->tans.typ,p_patl->tans.sl );
fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->tane.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tane.c0x,p_patl->tane.c1x,
        p_patl->tane.c2x,p_patl->tane.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tane.c0y,p_patl->tane.c1y,
        p_patl->tane.c2y,p_patl->tane.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tane.c0z,p_patl->tane.c1z,
        p_patl->tane.c2z,p_patl->tane.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tane.c0 ,p_patl->tane.c1 ,
        p_patl->tane.c2 ,p_patl->tane.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->tane.ofs,p_patl->tane.typ,p_patl->tane.sl );
fprintf(dbgfil(SURPAC),
"sur222 Coefficients: p_patl->pval.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur222 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->pval.c0x,p_patl->pval.c1x,
        p_patl->pval.c2x,p_patl->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur222 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->pval.c0y,p_patl->pval.c1y,
        p_patl->pval.c2y,p_patl->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur222 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->pval.c0z,p_patl->pval.c1z,
        p_patl->pval.c2z,p_patl->pval.c3z );
fprintf(dbgfil(SURPAC),
"sur222 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->pval.c0 ,p_patl->pval.c1 ,
        p_patl->pval.c2 ,p_patl->pval.c3  );
fprintf(dbgfil(SURPAC),
"sur222 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->pval.ofs,p_patl->pval.typ,p_patl->pval.sl );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */

t_l = 0.0;
   status=GE107((GMUNON *)&scur,&p_patl->spine,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_conloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }
fprintf(dbgfil(SURPAC),
"sur222 Spine start point   %f %f %f \n",
        out[0], out[1], out[2]  );
fprintf(dbgfil(SURPAC),
"sur222 Spine start tangent %f %f %f \n",
        out[3], out[4], out[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */

t_l = 1.0;
   status=GE107((GMUNON *)&scur,&p_patl->spine,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_conloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }

fprintf(dbgfil(SURPAC),
"sur222 Spine end   point   %f %f %f GE107 status= %d\n",
        out[0], out[1], out[2],status  );
fprintf(dbgfil(SURPAC),
"sur222 Spine end   tangent %f %f %f \n",
        out[3], out[4], out[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */



t_l = 0.0;
   status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_conloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }
fprintf(dbgfil(SURPAC),
"sur222 Limit start point   %f %f %f \n",
        out[0], out[1], out[2]  );
fprintf(dbgfil(SURPAC),
"sur222 Limit start tangent %f %f %f \n",
        out[3], out[4], out[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */

t_l = 1.0;
   status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_conloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }
fprintf(dbgfil(SURPAC),
"sur222 Limit end   point   %f %f %f \n",
        out[0], out[1], out[2]  );
fprintf(dbgfil(SURPAC),
"sur222 Limit end   tangent %f %f %f \n",
        out[3], out[4], out[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */



}                            /* End   if statement for whole fctn  */
    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate the spine plane                                        */

   static short spipla (p_patl,u_pat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat u_pat;        /* Patch (local) U parameter value         */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222*spipla Parameter U= %f Spine addresse &p_patl->spine= %d\n"
     ,u_pat, (int)&p_patl->spine);
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve (scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Spine point                                                   */

/* Calculate point on spline for the given u_pat value              */
/* Call of GE107.                                                  */
/*                                                                 !*/

t_l = u_pat;
   status=GE107
   ((GMUNON *)&scur,&p_patl->spine,t_l,rcode,outspine);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_conloft (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222*spipla Spine point   %f %f %f \n",
        outspine[0], outspine[1], outspine[2]  );
fprintf(dbgfil(SURPAC),
"sur222*spipla Spine tangent %f %f %f \n",
        outspine[3], outspine[4], outspine[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 3. Spine plane for 3D curves                                     */

/* Let the spine plane normal be the normalized tangent             */
/* Call of normv.                                                   */
/* Let the plane coefficient d= scalar product of the spine         */
/* plane normal and (the vector origin to) the spine point          */
/*                                                                 !*/

   plane[0] = outspine[3];
   plane[1] = outspine[4];
   plane[2] = outspine[5];

   status=varkon_normv(plane);
   if (status<0) 
        {
        sprintf(errbuf,"normv%%varkon_pat_conloft (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }

   plane[3] = plane[0]*outspine[0]+
              plane[1]*outspine[1]+
              plane[2]*outspine[2];

/*!                                                                 */
/* 4. Spine plane for 2D functions                                  */

/* Let the spine plane normal (for pval_plane) be (1,0,0) and the   */
/* plane coefficient d= current U value * spine segment arclength   */
/*                                                                 !*/

   pval_plane[0] = 1.0;   
   pval_plane[1] = 0.0;   
   pval_plane[2] = 0.0;   
   pval_plane[3] = u_pat*p_patl->spine.sl;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222*spipla Spine plane 3D nx %f nx %f nx %f d %f\n",
        plane[0], plane[1], plane[2], plane[3] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222*spipla Spine plane 2D nx %f nx %f nx %f d %f\n",
 pval_plane[0], pval_plane[1], pval_plane[2], pval_plane[3] );
}
#endif

    return(SUCCED);

} /* End of function                                                */


/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate intersection points between the spine plane and all    */
/* the longitudinal 3D curves and 2D functions in the patch.        */

   static short intspi (p_patl, e_case)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   short   e_case;       /* =1: u_pat =2: +epsilon =3: +epsilon/2   */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   short    nstart;      /* The number of restarts (=RESTRT except  */
                         /* for arcs where nstart=RESTRT+1 )        */
   short    nstart_add;  /* Additional restarts if first intersect  */
                         /* attempt fails                           */
   short    noall;       /* All intersects from one segment         */
   short    noinse;      /* Number of intersects from one segment   */
   DBfloat  useg[INTMAX];/* The unordered array of u solutions from */
                         /* one segment                             */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
if ( e_case == 1)
{
fprintf(dbgfil(SURPAC),
"sur222*intspi Intersect calculation for the input u_pat plane \n");
}
if ( e_case == 2)
{
fprintf(dbgfil(SURPAC),
"sur222*intspi Intersect calculation for the epsilon     plane \n");
}
if ( e_case == 3)
{
fprintf(dbgfil(SURPAC),
"sur222*intspi Intersect calculation for the mid         plane \n");
}
if ( e_case == 4)
{
fprintf(dbgfil(SURPAC),
"sur222*intspi Intersect calculation only for lims             \n");
}
if ( e_case == 5)
{
fprintf(dbgfil(SURPAC),
"sur222*intspi Intersect calculation only for lime             \n");
}
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222*intspi addresses lims %d lime %d tans %d tane %d   \n"
     ,(int)&p_patl->lims,(int)&p_patl->lime,
      (int)&p_patl->tans,(int)&p_patl->tane );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */

/* Initiations for GE101 and GE107.                               */
   rcode = 3;
   scur.hed_cu.type    = CURTYP;
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE; /* F|r offset kurvor ska data finnas i patch !! */
   nstart     = 2;
   nstart_add = 6;

   if ( fabs(p_patl->lims.ofs) > 0.00000001  ||
        fabs(p_patl->lime.ofs) > 0.00000001  ||
        fabs(p_patl->tans.ofs) > 0.00000001  ||
        fabs(p_patl->tane.ofs) > 0.00000001     )
      {
      sprintf(errbuf,"%%sur222");
      varkon_erinit();
      return(varkon_erpush("SU2413",errbuf));
      }

/* Goto lablime for e_case= 5.                                      */
/*                                                                 !*/

   if ( e_case == 5 ) goto lablime;

/*!                                                                 */
/* 2. Coordinates and derivatives for the start limit curve (lims)  */

/* Intersection.     Call of GE101.    Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1.    Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.  */
/*                                                                 !*/
   status = GE101((DBAny *)&scur,&p_patl->lims,plane,nstart    ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->lims,plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for lims Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (lims)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for lims Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (lims)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of lims intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"lims%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1  ||  e_case == 4 )
       {
       outlimsparam[0]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,outlims);
       }
     else if ( e_case == 2)
       {
       outlimsparam[1]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,e_outlims);
       }
     else if ( e_case == 3)
       {
       status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,m_outlims);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (lims)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }

   if ( e_case == 4 ) goto labback;

/*!                                                                 */
lablime:

/* 3. Coordinates and derivatives for the end   limit curve (lime)  */
/* Intersection.     Call of GE101.    Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1.    Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.  */
/*                                                                 !*/
   status = GE101((DBAny *)&scur,&p_patl->lime,plane,nstart ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->lime,plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for lime Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (lime)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for lime Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (lime)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of lime intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"lime%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1 ||  e_case == 5 )
       {
       outlimeparam[0]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->lime ,t_l,rcode,outlime);
       }
     else if ( e_case == 2)
       {
       outlimeparam[1]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->lime ,t_l,rcode,e_outlime);
       }
     else if ( e_case == 3)
       {
       status=GE107((GMUNON *)&scur,&p_patl->lime ,t_l,rcode,m_outlime);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (lime)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }

   if ( e_case == 5 ) goto labback;

/*!                                                                 */
/* 4. Coordinates and derivatives for the start tangent curve (tans)*/

/* Intersection.     Call of GE101.     Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1.  Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.     Error SU2943  for failure.  */
/*                                                                 !*/
   status = GE101((DBAny *)&scur,&p_patl->tans,plane,nstart    ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->tans,plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for tans Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (tans)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for tans Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (tans)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of tans intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"tans%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1)
       {
       outtansparam[0]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->tans ,t_l,rcode,outtans);
       }
     else if ( e_case == 2 )
       {
       outtansparam[1]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->tans ,t_l,rcode,e_outtans);
       }
     else if ( e_case == 3 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->tans ,t_l,rcode,m_outtans);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (tans)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }


/*!                                                                 */
/* 5. Coordinates and derivatives for the end tangent curve (tane)  */

/* Intersection.     Call of GE101.    Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1.    Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.  */
/*                                                                 !*/
   status = GE101((DBAny *)&scur,&p_patl->tane,plane,nstart    ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->tane,plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for tane Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (tane)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for tane Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (tane)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of tane intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"tane%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1)
       {
       outtaneparam[0]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->tane ,t_l,rcode,outtane);
       }
     else if ( e_case == 2 )
       {
       outtaneparam[1]=t_l; /* Parameter value for sur790 (calder) */
       status=GE107((GMUNON *)&scur,&p_patl->tane ,t_l,rcode,e_outtane);
       }
     else if ( e_case == 3 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->tane ,t_l,rcode,m_outtane);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (tane)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }


/*!                                                                 */
/* 6. Coordinates and derivatives for the mid curve (midc)          */

/* For a P value function and not a mid curve: Goto pval.           */
/* Intersection.     Call of GE101.    Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1.    Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.  */
/* Goto epval.                                                      */
/*                                                                 !*/

   if      ( p_patl->p_flag == 1 ) 
     {
     goto pval;
     }
   else if ( p_patl->p_flag == 2 ) 
     {
     ;
     }
   else
     {
     sprintf(errbuf,"Invalid p_flag%%varkon_pat_conloft (sur222)");
     return(varkon_erpush("SU2993",errbuf));
     }
   
   
   status = GE101((DBAny *)&scur,&p_patl->midc,plane,nstart    ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->midc,plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for midc Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (midc)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for midc Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (midc)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of midc intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"midc%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1)
       {
       status=GE107((GMUNON *)&scur,&p_patl->midc ,t_l,rcode,outmidc);
       }
     else if ( e_case == 2 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->midc ,t_l,rcode,e_outmidc);
       }
     else if ( e_case == 3 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->midc ,t_l,rcode,m_outmidc);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (midc)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }


   if ( p_patl->p_flag == 2 ) goto epval;

/*!                                                                 */
/* 7. Coordinates and derivatives for the P value function  (pval)  */

pval:   /* Label: P value function                                  */

/* Intersection.     Call of GE101.     Error SU2943  for failure.  */
/*                   A 2D function and pval_plane is used.          */
/* Delete and order. Call of GEsort_1.  Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.     Error SU2943  for failure.  */
/*                                                                 !*/

   status = GE101((DBAny *)&scur,&p_patl->pval,pval_plane,nstart    ,&noall,useg);
   if ( noall == 0 )
   status = GE101((DBAny *)&scur,&p_patl->pval,pval_plane,nstart_add,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GE101 failed for pval Plane d= %f\n",pval_plane[3] );
#endif
        sprintf(errbuf,"GE101 (pval)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
   if ( noall > 1 )
     {
     status=GEsort_1(useg,noall,&noinse,useg);
     if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi GEsort_1 failed for pval Plane d= %f\n",pval_plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (pval)%%varkon_pat_conloft");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Fel tolerans ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur222*intspi No of pval intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"pval%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
     if ( e_case == 1)
       {
       status=GE107((GMUNON *)&scur,&p_patl->pval ,t_l,rcode,outpval);
       }
     else if ( e_case == 2 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->pval ,t_l,rcode,e_outpval);
       }
     else if ( e_case == 3 )
       {
       status=GE107((GMUNON *)&scur,&p_patl->pval ,t_l,rcode,m_outpval);
       }
       if (status<0) 
          {
          sprintf(errbuf,"GE107 (pval)%%varkon_pat_conloft");
          return(varkon_erpush("SU2943",errbuf));
          }

epval:   /* Label: End P value function                             */
labback: /* Label: Return for e_case=4 or =5                        */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if ( e_case == 1)
{
fprintf(dbgfil(SURPAC),
"sur222 lims %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outlims[0], outlims[1], outlims[2],
        outlims[3], outlims[4], outlims[5]  );
fprintf(dbgfil(SURPAC),
"sur222 lime %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outlime[0], outlime[1], outlime[2],
        outlime[3], outlime[4], outlime[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tans %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outtans[0], outtans[1], outtans[2],
        outtans[3], outtans[4], outtans[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tane %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outtane[0], outtane[1], outtane[2],
        outtane[3], outtane[4], outtane[5]  );
if (  p_patl->p_flag == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 pval %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outpval[0], outpval[1], outpval[2],
        outpval[3], outpval[4], outpval[5]  );
}
if (  p_patl->p_flag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222 midc %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outmidc[0], outmidc[1], outmidc[2],
        outmidc[3], outmidc[4], outmidc[5]  );
}
}

if ( e_case == 2)
{
fprintf(dbgfil(SURPAC),
"sur222 lims %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outlims[0], e_outlims[1], e_outlims[2],
        e_outlims[3], e_outlims[4], e_outlims[5]  );
fprintf(dbgfil(SURPAC),
"sur222 lime %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outlime[0], e_outlime[1], e_outlime[2],
        e_outlime[3], e_outlime[4], e_outlime[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tans %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outtans[0], e_outtans[1], e_outtans[2],
        e_outtans[3], e_outtans[4], e_outtans[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tane %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outtane[0], e_outtane[1], e_outtane[2],
        e_outtane[3], e_outtane[4], e_outtane[5]  );
if (  p_patl->p_flag == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 pval %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outpval[0], e_outpval[1], e_outpval[2],
        e_outpval[3], e_outpval[4], e_outpval[5]  );
}
if (  p_patl->p_flag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222 midc %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outmidc[0], e_outmidc[1], e_outmidc[2],
        e_outmidc[3], e_outmidc[4], e_outmidc[5]  );
}
}

/* #ifdef DEBUGTILLF */
if ( e_case == 3)
{
fprintf(dbgfil(SURPAC),
"sur222 lims %18.8f %18.8f %18.8f %10.4f %10.4f %10.4f \n",
        m_outlims[0], m_outlims[1], m_outlims[2],
        m_outlims[3], m_outlims[4], m_outlims[5]  );
fprintf(dbgfil(SURPAC),
"sur222 lime %18.8f %18.8f %18.8f %10.4f %10.4f %10.4f \n",
        m_outlime[0], m_outlime[1], m_outlime[2],
        m_outlime[3], m_outlime[4], m_outlime[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tans %18.8f %18.8f %18.8f %10.4f %10.4f %10.4f \n",
        m_outtans[0], m_outtans[1], m_outtans[2],
        m_outtans[3], m_outtans[4], m_outtans[5]  );
fprintf(dbgfil(SURPAC),
"sur222 tane %18.8f %18.8f %18.8f %10.4f %10.4f %10.4f \n",
        m_outtane[0], m_outtane[1], m_outtane[2],
        m_outtane[3], m_outtane[4], m_outtane[5]  );
if (  p_patl->p_flag == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 pval %18.8f %18.8f %18.8f %10.4f %10.4f %10.4f \n",
        m_outpval[0], m_outpval[1], m_outpval[2],
        m_outpval[3], m_outpval[4], m_outpval[5]  );
}
if (  p_patl->p_flag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222 midc %8.8f %8.8f %8.8f %10.4f %10.4f %10.4f \n",
        m_outmidc[0], m_outmidc[1], m_outmidc[2],
        m_outmidc[3], m_outmidc[4], m_outmidc[5]  );
}
}
/* #endif DEBUGTILLF */

fflush(dbgfil(SURPAC));

}
#endif


    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create the generatrix curve and calculate coordinates and        */
/* derivatives for given input parameter value v_pat                */

   static short genpoi (p_patl,v_pat,e_case)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   short   e_case;       /* =1: u_pat =2: +epsilon =3: +epsilon/2   */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   DBVector ipt;         /* Intermediate point for generatrix       */
   DBSeg    gener;       /* Generatrix curve                        */
   DBfloat   t_l;        /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   DBfloat p_value;      /* P value for the generatrix              */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initializations                                               */

   p_value = F_UNDEF;

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Create curve segment                                          */

/*    Segment data to array points()                                */
/*                                                                 !*/

   if ( e_case == 1 )
     {
     points[0].x_gm=outlims[0];                   /* Start point    */
     points[0].y_gm=outlims[1];                   /*                */
     points[0].z_gm=outlims[2];                   /*                */
     points[1].x_gm=outtans[0];                   /* Start tangent  */
     points[1].y_gm=outtans[1];                   /*                */
     points[1].z_gm=outtans[2];                   /*                */
     points[3].x_gm=outlime[0];                   /* End   point    */
     points[3].y_gm=outlime[1];                   /*                */
     points[3].z_gm=outlime[2];                   /*                */
     points[2].x_gm=outtane[0];                   /* End   tangent  */
     points[2].y_gm=outtane[1];                   /*                */
     points[2].z_gm=outtane[2];                   /*                */
     p_value       =outpval[1];                   /* P value        */
     ipt.x_gm      =outmidc[0];                   /* Mid point      */
     ipt.y_gm      =outmidc[1];                   /*                */
     ipt.z_gm      =outmidc[2];                   /*                */
    }
   else if ( e_case == 2 )
     {
     points[0].x_gm=e_outlims[0];                 /* Start point    */
     points[0].y_gm=e_outlims[1];                 /*                */
     points[0].z_gm=e_outlims[2];                 /*                */
     points[1].x_gm=e_outtans[0];                 /* Start tangent  */
     points[1].y_gm=e_outtans[1];                 /*                */
     points[1].z_gm=e_outtans[2];                 /*                */
     points[3].x_gm=e_outlime[0];                 /* End   point    */
     points[3].y_gm=e_outlime[1];                 /*                */
     points[3].z_gm=e_outlime[2];                 /*                */
     points[2].x_gm=e_outtane[0];                 /* End   tangent  */
     points[2].y_gm=e_outtane[1];                 /*                */
     points[2].z_gm=e_outtane[2];                 /*                */
     p_value       =e_outpval[1];                 /* P value        */
     ipt.x_gm      =e_outmidc[0];                 /* Mid point      */
     ipt.y_gm      =e_outmidc[1];                 /*                */
     ipt.z_gm      =e_outmidc[2];                 /*                */
    }
   else if ( e_case == 3 )
     {
     points[0].x_gm=m_outlims[0];                 /* Start point    */
     points[0].y_gm=m_outlims[1];                 /*                */
     points[0].z_gm=m_outlims[2];                 /*                */
     points[1].x_gm=m_outtans[0];                 /* Start tangent  */
     points[1].y_gm=m_outtans[1];                 /*                */
     points[1].z_gm=m_outtans[2];                 /*                */
     points[3].x_gm=m_outlime[0];                 /* End   point    */
     points[3].y_gm=m_outlime[1];                 /*                */
     points[3].z_gm=m_outlime[2];                 /*                */
     points[2].x_gm=m_outtane[0];                 /* End   tangent  */
     points[2].y_gm=m_outtane[1];                 /*                */
     points[2].z_gm=m_outtane[2];                 /*                */
     p_value       =m_outpval[1];                 /* P value        */
     ipt.x_gm      =m_outmidc[0];                 /* Mid point      */
     ipt.y_gm      =m_outmidc[1];                 /*                */
     ipt.z_gm      =m_outmidc[2];                 /*                */
    }

/*!                                                                 */
/*  Call of GE133 if p_patl->p_flag = 1                            */
/*                                                                 !*/

    if ( p_patl->p_flag == 1 )
      {
      status=GE133(points,p_value ,&gener); 
      if (status<0) 
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222*genpoi GE133 failed p_value %f\n",
        p_value  );
}
#endif
         sprintf(errbuf,"GE133%%varkon_pat_conloft (sur222)");
         return(varkon_erpush("SU2943",errbuf));
         }
       }
    else if ( p_patl->p_flag == 2 )
      {
      status=GE136(points,&ipt,&gener);
      if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222*genpoi GE136 failed ipt %f %f %f\n",
        ipt.x_gm, ipt.y_gm, ipt.z_gm  );
}
#endif
         sprintf(errbuf,"%8.2f %8.2f %8.2f%%varkon_pat_conloft",
          ipt.x_gm, ipt.y_gm, ipt.z_gm  );
         return(varkon_erpush("SU2823",errbuf));
         }
       }
    else
       {
       sprintf(errbuf,"Unknown p_flag%%varkon_pat_conloft (sur222)");
       return(varkon_erpush("SU2993",errbuf));
       }

    if      ( e_case == 1 && epsilon > 0.0 )
      {
      g_u0 = gener; 
      }
    else if ( e_case == 1 && epsilon < 0.0 )
      {
      g_u1 = gener; 
      }
    else if ( e_case == 2 && epsilon > 0.0 )
      {
      g_u1 = gener; 
      }
    else if ( e_case == 3 )
      {
      g_um = gener; 
      }
    else if ( e_case == 2 && epsilon < 0.0 )
      {
      g_u0 = gener; 
      }

/*!                                                                 */
/* 3. Generatrix point                                              */

/* Calculate point on generatrix for the given v_pat value          */
/* Call of GE107.                                                  */
/*                                                                 !*/

t_l = v_pat;
if ( e_case == 1 )
  {
  status=GE107((GMUNON *)&scur,&gener,t_l,rcode,outgener);
  }
#ifdef DEBUGTILLF
else if ( e_case == 2 )
  {
  status=GE107((GMUNON *)&scur,&gener,t_l,rcode,e_outgener);
  }
else if ( e_case == 3 )
  {
  status=GE107((GMUNON *)&scur,&gener,t_l,rcode,m_outgener);
  }
#endif
  if (status<0) 
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222*genpoi GE107 failed t_l %f \n",
        t_l                  );
}
#endif
    sprintf(errbuf,"GE107%%varkon_pat_conloft (genpoi)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
if ( e_case == 1 )
{
fprintf(dbgfil(SURPAC),
"sur222 gen. %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outgener[0], outgener[1], outgener[2],
        outgener[3], outgener[4], outgener[5]  );
}
#ifdef DEBUGTILLF
else if ( e_case == 3 )
{
fprintf(dbgfil(SURPAC),
"sur222 gen. %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        m_outgener[0], m_outgener[1], m_outgener[2],
        m_outgener[3], m_outgener[4], m_outgener[5]  );
}
else if ( e_case == 2 )
{
fprintf(dbgfil(SURPAC),
"sur222 gen. %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        e_outgener[0], e_outgener[1], e_outgener[2],
        e_outgener[3], e_outgener[4], e_outgener[5]  );
}
#endif
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/


/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the derivatives in the U direction       */
/* (icase=1) when V=0 or V=1                                        */

   static short isov01 ( p_patl ,  v_pat )
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat v_pat;        /* Patch (local) V parameter value         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   short  e_case;        /* =4: Intersect lims   =5: Intersect lime */
/*-----------------------------------------------------------------!*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Determine if V=0 or V=1                                       */
/*                                                                 !*/

   if      ( fabs(v_pat) < TOL1 )
      {
      e_case = 4;
      }
   else if ( fabs(v_pat-1.0) < TOL1 )
      {
      e_case = 5;
      }
   else
      {
      return(-1);
      }

/*!                                                                 */
/* 2. Make the intersection                                         */
/*    Call of intspi.                    Error SU2973 for failure.  */
/*                                                                 !*/

   status=intspi (p_patl, e_case); 
   if (status<0) 
   {
   sprintf(errbuf,"intspi%%varkon_pat_conloft (sur222)");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 3. Derivatives to output variable                                */
/*                                                                 !*/

   if  ( e_case == 4 )  
     {
     r_x  = outlims[0];
     r_y  = outlims[1];
     r_z  = outlims[2];
     u_x  = outlims[3];
     u_y  = outlims[4];
     u_z  = outlims[5];
     u2_x = outlims[6];
     u2_y = outlims[7];
     u2_z = outlims[8];
     }
   else                 
     {
     r_x  = outlime[0];
     r_y  = outlime[1];
     r_z  = outlime[2];
     u_x  = outlime[3];
     u_y  = outlime[4];
     u_z  = outlime[5];
     u2_x = outlime[6];
     u2_y = outlime[7];
     u2_z = outlime[8];
     }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


