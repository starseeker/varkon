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

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_pat_segint                    File: sur904.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the intersection between a curve        */
/*  segment and a surface patch.                                    */
/*                                                                  */
/*  Input to the function is a surface (not only one patch) and     */
/*  a curve (not only one segment) but the calculation is only      */
/*  made on one patch and one segment. Which segment is defined     */
/*  in a computation (structure) variable. This variable also       */
/*  defines number restarts, etc.                                   */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-10   Originally written                                 */
/*  1996-06-14   Trimming ...                                       */
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_segint     Curve segment/patch intersect    */
/*                                                              */
/*------------------------------------------------------------- */

/*!New-Page--------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short retrieve();      /* Retrieve input to local variables  */
static short newstart();      /* Restart with new start point       */
#ifdef  TILLSVIDARE
static short addsol();        /* Add a non-existent solution.       */
#endif
#ifdef  DEBUG
static short initial();       /* Check and initiations For Debug On */
static short analyze();       /* Determine calculation case         */
static short curfail();       /* Printout curve/surface  failure    */
static short clofail();       /* Printout closest point failure     */
static short linfail();       /* Printout line/surface  failure     */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ___________________                               */
/*               !                   !                              */
/*               ! varkon_pat_segint !                              */
/*               !    (sur904)       !  Felaktigt !!!!!!!           */
/*               !___________________!                              */
/*    ____________________!__________________                       */
/*  _!_   _!_     _!_     _!_   _!_   _!_   _!_                     */
/* !   ! !   !   !   !   !   ! !   ! !   ! !   !                    */
/* ! 1 ! ! 2 ! L ! 3 ! L ! 4 ! ! 5 ! ! 6 ! ! 7 !                    */
/* !   ! !   ! a !   ! a !   ! !   ! !   ! !   !                    */
/* ! i ! ! r ! b ! n ! b ! O ! ! a ! ! s ! ! S !                    */
/* ! n ! ! e ! e ! e ! e ! b ! ! d ! ! d ! ! t !                    */
/* ! i ! ! t ! l ! w ! l ! j ! ! d ! ! i ! ! e !                    */
/* ! t ! ! r !   ! s !   ! e ! ! s ! ! r ! ! p !                    */
/* ! i ! ! i ! n ! t ! n ! c ! ! o ! ! e ! !   !                    */
/* ! a ! ! e ! e ! a ! o ! t ! ! l ! ! c ! !___!                    */
/* ! l ! ! v ! w ! r ! p ! i ! !___! ! t !   !                      */
/* !___! ! e ! s ! t ! t ! v !       !___!   !                      */
/*       !___! t !___!   ! e !               !                      */
/*                       !___!               !                      */
/*                                                                  */
/*------------------------------------------------------------------*/
/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  objf;             /* Objective fctn (distance)      */
static DBfloat  objf_crit;        /* Objective fctn end criterion   */
static DBint    ctype;            /* Case of computation            */
static DBint    method;           /* Method which shall be used     */
static DBint    nstart;           /* The number of restarts         */
static DBint    i_start;          /* Current no of restarts         */
static DBint    maxiter;          /* Maximum number of iterations   */
static DBVector s_uvpt;           /* Start UV point surface         */
static DBVector s_upt;            /* Start U  point curve           */
static DBint    uvstart;          /* Eq. 1: s_uvpt is defined       */
static DBint    ustart;           /* Eq. 1: s_upt  is defined       */
static DBfloat  us,vs,ue,ve;      /* The search area (a patch)      */
static DBfloat  usc,uec;          /* The search area (a segment)    */
static PROJECT  prodat;           /* Closest pt   computation data  */
static PROJECT  lindat;           /* Surface/line computation data  */
static SPOINT   sprec;            /* Surface point   record         */
static IPLANE   pladat;           /* Planar intersect comput. data  */
static DBfloat  t_in[2];          /* Parameter curve segment limits */
static DBfloat  t_in_e1[2];       /* Extra segment 1                */
static DBfloat  t_in_e2[2];       /* Extra segment 2                */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
static DBfloat  ctol;             /* Coordinate tolerance           */
static DBfloat  ntol;             /* Angle      tolerance           */
static DBfloat  idpoint;          /* Identical point criterion      */
static DBfloat  idangle;          /* Identical angle criterion      */
static DBint    numsol;           /* Number of solutions (=*p_npts) */
static DBint    icase;            /* Case for varkon_sur_eval       */
/*static EVALS    xyz; */         /* Surface point and derivatives  */
static EVALC    xyz_c;            /* Curve   point and derivatives  */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109                   * Curve   evaluation routine             */
/* varkon_tra_intlinsur    * Matrix for surface/line                */
/* varkon_sur_num2         * Patch two parameter calculat           */
/* varkon_cur_num1         * Curve one parameter calculat           */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_sur_order        * Order surface (solut.) pts             */
/* varkon_cur_order        * Ordering of curve points               */
/* varkon_ini_spoint       * Initialize SPOINT                      */
/* varkon_angd             * Angle in degrees                       */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_pat_segint      */
/* SU2983 = sur904 Illegal computation case=  for varkon_pat_segint */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_segint    */
/* SU2993 = Severe program error in varkon_pat_segint (sur904).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_pat_segint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Curve segment data                (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
/* Out:                                                             */
   DBint   *p_npts,      /* Number of output points           (ptr) */
   EVALS   xyz_a[SMAX],  /* Surface solution points  (R*3 and UV)   */
   EVALC   xyz_b[SMAX] ) /* Curve   solution points  (R*3 and UV)   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_par;       /* Current curve   parameter value         */
   DBVector uv_pt;       /* Current surface parameter point         */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint  no_iter;       /* Number of iterations                    */
   DBint  try_lin;       /* Flag for restart surface/line           */
   DBTmat pat_tra;       /* Transformation matrix for patches       */
   EVALS  xyz_clo_a[SMAX];/* All varkon_sur_num2 solutions          */
   EVALS  xyz_clo[SMAX]; /* Ordered solutions                       */
   DBint  clo_npts_a;    /* Number of xyz_clo_a solutions           */
   DBint  clo_npts;      /* Number of xyz_clo   solutions           */
   DBint  pla_npts;      /* Number of curve /plane solutions        */
   DBint  pla_npts_a;    /* Number of ordered solutions             */
   EVALC  xyz_p_a[SMAX]; /* Array with all curve/plane solutions    */
   EVALC  xyz_p[SMAX];   /* Ordered solutions                       */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

  DBint   ocase_s;       /* Point ordering case for sur980          */
  DBint   ocase_c;       /* Point ordering case for sur982          */

#ifdef  DEBUG
   DBint  calc_type;     /* Type of calculation                     */
#endif
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 ** Enter varkon_pat_segint ** \n"  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 p_sur %d p_pat %d p_cur %d p_seg %d p_seg->spek_gm %d\n",
              p_sur, p_pat, p_cur, p_seg , p_seg->spek_gm);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives For DEBUG On.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

#ifdef DEBUG
    status=initial(p_comp,p_npts);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_pat_segint (sur904)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif


/*!                                                                 */
/* 2. Retrieve the input data                                       */
/* __________________________                                       */
/*                                                                  */
/*  Retrieve input data to local static variables.                  */
/*  Call of retrieve.                                               */
/*                                                                 !*/

    status=retrieve (p_cur,p_comp);
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"retrieve%%varkon_pat_segint (sur904)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* 3. Restart (or start) of calculation                             */
/* ____________________________________                             */
/*                                                                 !*/

    objf_crit = ctol;                       
    no_iter   = 0;
    objf      = 50000.0;
    try_lin   = 0;
    i_start   = 0;
    ocase_s   = 10;
    ocase_c   =  1;

    u_par = s_upt.x_gm;           /* Start curve   parameter value */
    uv_pt = s_uvpt;               /* Start surface parameter value */

nxt_pt:; /* Label: Next iteration                                   */

/*!                                                                 */
/*  Calculate point on the curve                                    */
/*  Call of varkon_GE109 (GE109).                                 */
/*                                                                 !*/

    no_iter   = no_iter + 1;

    xyz_c.evltyp   = EVC_DR;
    xyz_c.t_global = u_par;        /* Current curve parameter value */

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_c);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error GE109 failed for U= %10.6f \n", xyz_c.t_global );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
        sprintf(errbuf,"GE109%%varkon_sur_patsegint (sur904)");
        return(erpush("SU2943",errbuf));
        }

    sprec.extpt = xyz_c.r;              /* Current external (curve) point */
    sprec.proj  = xyz_c.drdt;           /* Current shooting direction     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Current external (curve) point sprec.expt %10.2f %10.2f %10.2f \n",
  sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm   );
fflush(dbgfil(SURPAC)); 
}
#endif


    sprec.startuvpt = uv_pt;     /* Current surface parameter point */

/*!                                                                 */
/*  Calculate the surface point with u,v as variables in a          */
/*  given patch.                                                    */
/*  This is formulated as a non-linear optimization problem         */
/*  which is solved in varkon_sur_num2 (sur902).                    */
/*                                                                 !*/

r_start:;

    if  ( objf <  100.0*idpoint || try_lin == 1  )
      {
status=varkon_tra_intlinsur
      ( &xyz_c.r, &xyz_c.drdt, &pat_tra );
      if(status<0)
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error sur690 failed for \n");
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"varkon_tra_intlinsur%%sur904");
        return(varkon_erpush("SU2943",errbuf));
        }

   lindat.pat_tra  = pat_tra;  /* Current transformation matrix   */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Calculate line/surface intersect (Call sur902) \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

status=varkon_sur_num2
    (p_sur,p_pat,(IRUNON*)&lindat,&sprec,&clo_npts_a,xyz_clo_a);
    if(status<0)
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 Line/surface failed for sprec.expt %10.2f %10.2f %10.2f \n",
  sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
    sprintf(errbuf,"sur902 line/surface%%sur904");
    return(varkon_erpush("SU2943",errbuf));
      }
    status = varkon_sur_order
      (&sprec.extpt, xyz_clo_a, clo_npts_a, ocase_s, xyz_clo, &clo_npts);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_order%%sur904");
        return(varkon_erpush("SU2943",errbuf));
        }
      try_lin = 1;
      if ( clo_npts > 0 ) goto surlin;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 There is no line/surface intersect point ! (try_lin %d)\n",
  try_lin  );
fflush(dbgfil(SURPAC)); 
}
   linfail ( xyz_clo, clo_npts);
#endif

      }    /* End objf < 100*idpoint Line/surface intersect */





#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Calculate closest point          (Call sur902) \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

status=varkon_sur_num2
    (p_sur,p_pat,(IRUNON*)&prodat,&sprec,&clo_npts_a,xyz_clo_a);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 Closest pt failure sprec.expt %10.2f %10.2f %10.2f \n",
  sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
    sprintf(errbuf,"sur902 Close pt%%sur904");
    return(varkon_erpush("SU2943",errbuf));
    }

    status = varkon_sur_order
      (&sprec.extpt, xyz_clo_a, clo_npts_a, ocase_s, xyz_clo, &clo_npts);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_order 2%%sur904");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && clo_npts == 0 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 There is no closest pt to current curve pt (try_lin %d)\n",
  try_lin  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
   clofail ( xyz_clo, clo_npts);
#endif

/*!                                                                 */
/*  If closest point calculation failed and no surface/line         */
/*  intersection is made: Goto r_start                              */
/*                                                                 !*/

    if ( clo_npts == 0 && try_lin  == 0 ) 
      {
      try_lin = 1;
      goto r_start;
    }

/*!                                                                 */
/*  Restart with new start point if there are no solution points    */
/*  Call of newstart and goto nxt_pt.                               */
/*  End of calculation if there are no solution points and number   */
/*  of restarts (nstart) is exceeded.                               */
/*                                                                 !*/

    if   ( clo_npts == 0 )
      {
       newstart(p_cur,&u_par);
       if ( u_par > 900.0 )
         {
#ifdef DEBUG

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 No intersect for pt %10.2f %10.2f %10.2f\n",
  sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm   );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
/*      No solution  */
        *p_npts = 0;
        goto no_sol; 
        }

       else 
         {
         goto nxt_pt; /* Borde start punkt yta anv{ndas igen ? */
         objf      = 50000.0;
         try_lin   = 0;
         uv_pt = s_uvpt;   /* Start surface parameter value */
         }
      }  /* End clo_npts == 0  */


surlin:;  /* There is a surface line intersect */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 Number of solutions from varkon_sur_num2 clo_npts= %d \n",
     clo_npts ); 
  }
if ( dbglev(SURPAC) == 2 && clo_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 Normal xyz_clo %f %f %f \n",
     xyz_clo[0].n_x, xyz_clo[0].n_y, xyz_clo[0].n_z  ); 
  }
fflush(dbgfil(SURPAC));
#endif

    pladat.in_x    = xyz_clo[0].n_x; 
    pladat.in_y    = xyz_clo[0].n_y; 
    pladat.in_z    = xyz_clo[0].n_z; 
    pladat.in_d    = xyz_clo[0].r_x*xyz_clo[0].n_x+
                     xyz_clo[0].r_y*xyz_clo[0].n_y+
                     xyz_clo[0].r_z*xyz_clo[0].n_z; 

/*!                                                                 */
/*  Calculate intersect points in segment i_seg.                    */
/*  Call of varkon_cur_num1 (sur901).                               */
/*                                                                 !*/

status=varkon_cur_num1
    (p_cur,p_seg,t_in,(IRUNON*)&pladat,&pla_npts_a,xyz_p_a);
     if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error sur901 failed for t_in \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"sur901%%varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2943",errbuf));
       }


/*  Added 1996-06-13 Not nicely programmed ....... */
  if  ( pla_npts_a == 0 && t_in_e1[0] > 0.0 )
    {
status=varkon_cur_num1
    (p_cur,p_seg,t_in_e1,(IRUNON*)&pladat,&pla_npts_a,xyz_p_a);
     if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error sur901 failed for t_in_e1 \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"sur901 t_in_e1%%sur904");
       return(varkon_erpush("SU2943",errbuf));
       }
    }      /*  End  pla_npts_a == 0 && t_in_e1[0] > 0.0 */


  if  ( pla_npts_a == 0 && t_in_e2[0] > 0.0 )
    {
status=varkon_cur_num1
    (p_cur,p_seg,t_in_e2,(IRUNON*)&pladat,&pla_npts_a,xyz_p_a);
     if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error sur901 failed for t_in_e2 \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"sur901 t_in_e2%%sur904");
       return(varkon_erpush("SU2943",errbuf));
       }
    }      /*  End  pla_npts_a == 0 && t_in_e2[0] > 0.0 */

status=varkon_cur_order
      (&sprec.extpt, xyz_p_a, pla_npts_a, ocase_c, xyz_p, &pla_npts);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_cur_order%%sur904");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 Number of solutions from varkon_cur_num1 pla_npts= %d \n",
     pla_npts ); 
  }
if ( dbglev(SURPAC) == 2 && pla_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 Curve/plane intersect xyz_p %10.2f %10.2f %10.2f \n",
     xyz_p[0].r.x_gm, xyz_p[0].r.y_gm, xyz_p[0].r.z_gm  ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/*  Analyze the calculation  for Debug On                           */
/*                                                                 !*/

#ifdef  DEBUG
   analyze (xyz_clo, clo_npts, pla_npts, xyz_p, &calc_type);
#endif



/*!                                                                 */
/*  Restart with new start point if there are no plane/curve        */
/*  intersect points. Call of newstart and goto nxt_pt.             */
/*  End of calculation if there are no solution points and number   */
/*  of restarts (nstart) is exceeded.                               */
/*                                                                 !*/

    if   ( pla_npts == 0 )
      {
       newstart(p_cur,&u_par);
       if ( u_par > 900.0 )
         {
#ifdef DEBUG

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 sur902 No intersect for pt %10.2f %10.2f %10.2f (2)\n",
  sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm   );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
/*      No solution  */
        *p_npts = 0;
        goto no_sol; 
        }

       else 
         {
         goto nxt_pt; /* Borde start punkt yta anv{ndas igen ? */
         objf      = 50000.0;
         try_lin   = 0;
         uv_pt = s_uvpt;   /* Start surface parameter value */
         }
      }  /* End pla_npts == 0  */



      objf =(xyz_clo[0].r_x - xyz_p[0].r.x_gm)*(xyz_clo[0].r_x - xyz_p[0].r.x_gm)+
            (xyz_clo[0].r_y - xyz_p[0].r.y_gm)*(xyz_clo[0].r_y - xyz_p[0].r.y_gm)+
            (xyz_clo[0].r_z - xyz_p[0].r.z_gm)*(xyz_clo[0].r_z - xyz_p[0].r.z_gm);

      objf = SQRT(objf);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 Distance curve and surface point objf= %15.10f no_iter= %d\n", 
          objf, no_iter  ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

  if ( objf > objf_crit && no_iter < maxiter )
    {
    uv_pt.x_gm = xyz_clo[0].u;
    uv_pt.y_gm = xyz_clo[0].v;
    u_par      = xyz_p[0].t_global;
    goto nxt_pt;
    }

/*!                                                                 */
/* 4. Exit                                                          */
/* ________                                                         */
/*                                                                 !*/

    if ( no_iter > maxiter )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error no_iter > maxiter = %d      \n", maxiter        );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"maxiter exceeded%%varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2993",errbuf));
         }

#ifdef  DEBUG
   if ( pla_npts > 1 || clo_npts > 1  )
     {
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur904 There are multiple solutions pla_npts= %d clo_npts %d \n", 
          pla_npts, clo_npts  ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#ifdef BORDEINTETASBORT
     sprintf(errbuf,"pla_npts>1/clo_npts>1%%sur904");
     return(varkon_erpush("SU2993",errbuf));
#endif  /* slut  #ifdef BORDEINTETASBORT   */
     }
#endif

/* !!!!!!!  Not yet programmed for multiple solutions !!!!!!! */

   *p_npts = 1;            /* Number of output points         (ptr) */
   xyz_a[0] = xyz_clo[0];  /* Array with all solutions R*3 and UV   */
   xyz_b[0] = xyz_p[0];    /* Array with all solutions R*3 and UV   */

no_sol:;   /* There is no solution (intersect)                      */

#ifdef DEBUG

if ( dbglev(SURPAC) == 1 && *p_npts > 0 )
  {
fprintf(dbgfil(SURPAC),
"sur904 Exit*varkon_pat_segint Intersect pt %10.2f %10.2f %10.2f\n",
 xyz_b[0].r.x_gm,  xyz_b[0].r.y_gm,  xyz_b[0].r.z_gm );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
if ( dbglev(SURPAC) == 1 && *p_npts == 0 )
  {
fprintf(dbgfil(SURPAC),
"sur904 Exit*varkon_pat_segint No intersect between patch and curve segment\n");
fflush(dbgfil(SURPAC)); 
  }

#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

#ifdef DEBUG
/*!********* Internal ** function ** initial **For*Debug*On**********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data and it initializes the        */
/* output variables and the static (common) variables to the        */
/* values 1.23456789 and 123456789.                                 */

   static short initial(p_comp,p_npts)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBint   *p_npts;      /* Number of output points           (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/* ==============================                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"Enter ***** varkon_pat_segint (sur904) ** initial ******\n");
}

/* For new types of calculation. Add check here and check that      */
/* current retrieved data (maxiter, ctype, nu, .. ) also are        */
/* defined in the new structure variable. If not add IF-THEN        */
/* statements for the different types of calculations               */

  if      ( p_comp->icu_un.ctype == SURCUR  )
       {
       ;
       }
   else
       {
       sprintf(errbuf,"(ctype)%% varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2993",errbuf));
       }

if ( p_comp->icu_un.ctype == SURCUR  )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*initial Case SURCUR: \n"); 
}
}


if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*initial Comp. case %d  Method %d  Restrts %d Max iter %d\n",
      p_comp->icu_un.ctype,
      p_comp->icu_un.method, 
      p_comp->icu_un.nstart, 
      p_comp->icu_un.maxiter);

fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*initial  Patch   us  %8.4f vs  %8.4f ue  %8.4f ve  %8.4f\n",
    p_comp->icu_un.us ,  p_comp->icu_un.vs ,  
    p_comp->icu_un.ue ,  p_comp->icu_un.ve );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*initial  Segment usc %8.4f uec %8.4f\n",
    p_comp->icu_un.usc,  p_comp->icu_un.uec); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

/*!                                                                 */
/* 2. Check of input data                                           */
/* ======================                                           */
/*                                                                  */
/* Check the UV area. Mininum rectangle sides are 10*TOL4.          */
/*                                                                 !*/

   if      ( fabs(p_comp->icu_un.ue - p_comp->icu_un.us) < 10.0*TOL4 ) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error ue - us < 10*TOL4  \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"(ue-us<10*TOL4)%% varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2993",errbuf));
       }
   else if ( fabs(p_comp->icu_un.ve - p_comp->icu_un.vs) < 10.0*TOL4 ) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904 Error ve - vs < 10*TOL4  \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"(ve-vs<10*TOL4)%% varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/* The number of maxiter  must be between 10 and 100.               */
/* The number of restarts must be between 2 and maxiter.            */
/*                                                                 !*/

    if (p_comp->icu_un.maxiter>=  10     || 
        p_comp->icu_un.maxiter<= 100   ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(maxiter)%% varkon_pat_segint (sur904)");
        return(varkon_erpush("SU2993",errbuf));
        }

    if (p_comp->icu_un.nstart > 1 || 
        p_comp->icu_un.nstart <= p_comp->icu_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%% varkon_pat_segint (sur904)");
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 3. Initialize output variables and static (internal) variables   */
/* ==============================================================   */
/*                                                                 !*/

    *p_npts     = I_UNDEF;    

    us          = F_UNDEF;      
    ue          = F_UNDEF;      
    vs          = F_UNDEF;      
    ve          = F_UNDEF;      

    ctype       = I_UNDEF;    
    method      = I_UNDEF;    
    nstart      = I_UNDEF;    
    maxiter     = I_UNDEF;    

/*!                                                                 */
/* Initialize SPOINT                                                */
/* Call of varkon_ini_spoint (sur777).                              */
/*                                                                 !*/

    varkon_ini_spoint (&sprec);

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

/*!New-Page--------------------------------------------------------!*/


#ifdef  TILLSVIDARE
/*!************** Internal ** function ** addsol ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Add a solution if it is a new solution (not already existing).  */
/*                                                                  */
/*  Note that the identical points criterion (ctol) is used for     */
/*  all types of interrogation (c_zero is not used).                */
/*                                                                  */
               static  short   addsol(p_sur,p_pat,xyz_a)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   EVALS    xyz_a[SMAX]; /* Array with all solutions (R*3 and UV)   */

/*     Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*      .....                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_sol;         /* Loop index for previous solutions       */
   DBint  icase_l;       /* Calculation case for varkon_sur_eval    */
 
/*-----------------------------------------------------------------!*/

   DBfloat dist;         /* Distance between solution points (R*3)  */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*addsol ctol %f numsol %d \n"
   ,   ctol , numsol );
  }
#endif

/*!                                                                 */
/* 1. Check if solution already exists                              */
/* __________________________________                               */

/*  If numsol = 0 add solution (goto adds).                         */
/*                                                                 !*/

    if ( numsol == 0 ) goto  adds;

/*!                                                                 */
/*  Start loop previous solutions i_sol= 1,2, .. ,numsol.           */
/*                                                                 !*/

   for (i_sol = 1; i_sol <= numsol ; i_sol = i_sol+1)
     {

/*!                                                                 */
/*    Solution exists if the R*3 distance between xyz and xyz_a     */
/*    is less than ctol.  Goto sex if this is the case.             */
/*                                                                 !*/

     dist = SQRT (
       (xyz.r_x-xyz_a[i_sol-1].r_x)*(xyz.r_x-xyz_a[i_sol-1].r_x) +
       (xyz.r_y-xyz_a[i_sol-1].r_y)*(xyz.r_y-xyz_a[i_sol-1].r_y) +
       (xyz.r_z-xyz_a[i_sol-1].r_z)*(xyz.r_z-xyz_a[i_sol-1].r_z) );

#ifdef DEBUG
/* 1995-08-27 if ( dbglev(SURPAC) == 1 &&  dist > 10*ctol )  */
if ( dbglev(SURPAC) == 1  )
 {
 fprintf(dbgfil(SURPAC),
 "sur904*addsol u %10.5f v %10.5f dist to prev. solut. pt %25.10f\n",
  xyz_a[i_sol-1].u , xyz_a[i_sol-1].v , dist );
 }
#endif

     if ( dist <= 10.0*ctol ) goto sex;


     }  /* End loop i_sol= 1,2,..., numsol                          */



/*!                                                                 */
/* 2. Add solution                                                  */
/* _______________                                                  */
/*                                                                  */
adds:  /* Label: Add solution                                       */
/*                                                                  */
/*  Increase number of solutions.                                   */
    numsol = numsol + 1;
/*  Check that numsol is less or equal SMAX                         */
/*                                                                 !*/
    if ( numsol > SMAX )
       {
       sprintf(errbuf,"(SMAX)%% varkon_pat_segint (sur904)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/*  Recompute xyz with icase_l= 3 for  xxx???                       */
/*                                                                 !*/

    if ( ctype  == SURPLAN )
       {
       icase_l = 3;  
       status=varkon_sur_eval
       (p_sur,p_pat,icase_l,xyz.u,xyz.v,&xyz);
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_pat_segint (addsol)");
        return(varkon_erpush("SU2943",errbuf));
        }
       }

/*!                                                                 */
/*  Points and coordinates to xyz_a.                                */
/*                                                                 !*/

    xyz_a[numsol-1]=xyz;     /* The R*3 solution point    */



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*addsol numsol %d u %8.5f v %8.5f \n",
   numsol , xyz_a[numsol-1].u , xyz_a[numsol-1].v );
fflush(dbgfil(SURPAC)); 
  }
#endif





sex:  /*! Label sex: Solution already exists                        !*/

    return(SUCCED);


} /* End of function */

/*!****************************************************************!*/
#endif  /*  TILLSVIDARE  */










/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** retrieve ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function retrieves input data to local static variables.     */

   static short retrieve(p_cur,p_comp)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur;       /* Curve                             (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Computation data.                                             */
/* ====================                                             */
/*                                                                  */
/* Case of computation, method and number of restarts, etc          */
/* Retrieve ctype, method, nstart and maxiter from p_comp.          */
/*                                                                 !*/

   ctype   = p_comp->icu_un.ctype;
   method  = p_comp->icu_un.method;
   nstart  = p_comp->icu_un.nstart;
   maxiter = p_comp->icu_un.maxiter;
   comptol = p_comp->icu_un.comptol;
   ctol    = p_comp->icu_un.ctol;
   ntol    = p_comp->icu_un.ntol;
   idpoint = p_comp->icu_un.idpoint;
   idangle = p_comp->icu_un.idangle;

/*!                                                                 */
/* Let numsol= 0       (start value for number of solutions)        */
/*                                                                 !*/

   numsol    = 0;

/*!                                                                 */
/* 2. Point data.                                                   */
/* ==============                                                   */

/* Retrieve       us, vs, ue, ve, ....  from p_comp                 */
/*                                                                 !*/

/*!                                                                 */
/* If the start UV point is defined:                                */
/*    i. Retrieve s_uvpt from ISURCUR record p_comp                 */
/*   ii. Let flag uvstart=  1                                       */
/* If the start UV point not is defined:                            */
/*    i. Let flag uvstart= -1                                       */
/*                                                                 !*/

   if      ( ctype  == SURCUR  )
       {
       icase = 3;
       us          = p_comp->icu_un.us;   
       vs          = p_comp->icu_un.vs;   
       ue          = p_comp->icu_un.ue;   
       ve          = p_comp->icu_un.ve;   
       usc         = p_comp->icu_un.usc;     
       uec         = p_comp->icu_un.uec; 
       if ( p_comp->icu_un.startuvpt.z_gm >= 0.0 ) 
          {
          s_uvpt  = p_comp->icu_un.startuvpt;   
          uvstart = 1;
          }
       else
          {
          uvstart =  -1;
          }
       if ( p_comp->icu_un.startupt.z_gm >= 0.0 ) 
          {
          s_upt  = p_comp->icu_un.startupt;   
          ustart = 1;
          }
       else
          {
          ustart =  -1;
          }
       }           /*  End SURCUR  */


/*!                                                                 */
/* Interrogation (computation) data to prodat (PROJECT):            */
/* Interrogation case is closest (projected) point CLOSEPT          */
/* One-dimensional interpolation method is three pt parabola.       */
/* Number of restarts in the patch nstart =   4                     */
/* Maximum number of iterations    maxiter=  50                     */
/* First steps are closest point calculations                       */
/*                                                                 !*/

   prodat.ctype    = CLOSEPT;
   prodat.method   = I_UNDEF;
   prodat.nstart   =  2; 
   prodat.maxiter  = 50;   
   prodat.ocase    = 1;      /* Order in R*3 distance */
   prodat.acc      =   1;    /* Search only in one patch Not used ..*/
   prodat.ctol     = idpoint;  /* Not necessary to have tight       */
   prodat.ntol     = 10.0*idangle;  /* tol.'s for the closest pt calc.   */
   prodat.scase    = 1;       
   prodat.pat_tra.g11 = 0.0;
   prodat.pat_tra.g12 = 0.0;
   prodat.pat_tra.g13 = 0.0;
   prodat.pat_tra.g14 = 0.0;
   prodat.pat_tra.g21 = 0.0;
   prodat.pat_tra.g22 = 0.0;
   prodat.pat_tra.g23 = 0.0;
   prodat.pat_tra.g24 = 0.0;
   prodat.pat_tra.g31 = 0.0;
   prodat.pat_tra.g32 = 0.0;
   prodat.pat_tra.g33 = 0.0;
   prodat.pat_tra.g34 = 0.0;
   prodat.pat_tra.g41 = 0.0;
   prodat.pat_tra.g42 = 0.0;
   prodat.pat_tra.g43 = 0.0;
   prodat.pat_tra.g44 = 0.0;
/* prodat.objfunc  = varkon_int_nearpt;    ska l{ggas till */



/*!                                                                 */
/* Interrogation (computation) data to lindat (PROJECT):            */
/* Interrogation case is line surface intersect LINTSUR             */
/* Last steps will be line/surface intersects                       */
/*                                                                 !*/

   lindat.ctype    = LINTSUR;
   lindat.method   = I_UNDEF;
   lindat.nstart   =  0;       /* Only the current start point      */
   lindat.maxiter  = 50;   
   lindat.ocase    = 1;        /* R*3 distance ordering */
   lindat.acc      = 1;        
   lindat.ctol     = ctol;
   lindat.ntol     = ntol;
   lindat.scase    = 1;        /* All intersect points              */
   lindat.pat_tra  = prodat.pat_tra;  /* Initialization of data     */
/* lindat.objfunc  = varkon_int_linsurpt;    ska l{ggas till */


/*!                                                                 */
/* Data to the surface point SPOINT:                                */
/* External (input) pt and UV start point                           */
/*                                                                 !*/

   sprec.extpt.x_gm     = F_UNDEF;         
   sprec.extpt.y_gm     = F_UNDEF;         
   sprec.extpt.z_gm     = F_UNDEF;         
   sprec.startuvpt.x_gm = s_uvpt.x_gm;   /* First iteration value   */
   sprec.startuvpt.y_gm = s_uvpt.y_gm;   
   sprec.startuvpt.z_gm = s_uvpt.z_gm;   
   sprec.us             = us;            /* For all iterations and  */
   sprec.ue             = ue;            /* closest and intesersct  */
   sprec.vs             = vs;      
   sprec.ve             = ve;      
   sprec.proj.x_gm      = 0.0;           /* Shall be tangent from   */
   sprec.proj.y_gm      = 0.0;           /* the curve for surface   */
   sprec.proj.z_gm      = 0.0;           /* line intersection calc. */
   sprec.extptnorm.x_gm = 0.0;     
   sprec.extptnorm.y_gm = 0.0;     
   sprec.extptnorm.z_gm = 0.0;     
   sprec.extuvpt.x_gm   = 0.0;     
   sprec.extuvpt.y_gm   = 0.0;     
   sprec.extuvpt.z_gm   = 0.0;     


/*!                                                                 */
/* Computation case rtype and plane to structure variable pladat.   */
/* Selected method (NEWTON), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable pladat.               */
/* Let the criterion for identical points be pladat.ctol= ctol      */
/*                                                                 !*/


pladat.ctype   = CURPLAN;
pladat.method  = NEWTON;
pladat.nstart  = 3;      
pladat.maxiter = 20;      

pladat.in_x    = F_UNDEF; 
pladat.in_y    = F_UNDEF; 
pladat.in_z    = F_UNDEF; 
pladat.in_d    = F_UNDEF; 

pladat.ctol    = ctol;


t_in[0]        = usc;           /* Parameter curve segment limits */
t_in[1]        = uec;           /* Parameter curve segment limits */

if  ( usc >= 2.0 )
  {
  t_in_e1[0]    = usc - 1.0;
  t_in_e1[1]    = uec - 1.0;
  }
else
  {
  t_in_e1[0]    = F_UNDEF;    
  t_in_e1[1]    = F_UNDEF;    
  }

if  ( uec <= p_cur->ns_cu - 1 )
  {
  t_in_e2[0]    = usc + 1.0;
  t_in_e2[1]    = uec + 1.0;
  }
else
  {
  t_in_e2[0]    = F_UNDEF;    
  t_in_e2[1]    = F_UNDEF;    
  }

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*retrieve ctype %d method %d nstart %d maxiter %d\n",
        ctype,    method,    nstart,    maxiter );
fprintf(dbgfil(SURPAC),
"sur904*retrieve nstart for Closest pt %d Line/surface intersect %d\n",
        prodat.nstart, lindat.nstart );
fprintf(dbgfil(SURPAC),
"sur904*retrieve ctol %10.7f ntol %10.7f idpoint %10.7f idangle %10.7f\n", 
               ctol, ntol, idpoint, idangle);
fprintf(dbgfil(SURPAC),
"sur904*retrieve Tolerances for closest pt ctol %10.7f ntol %10.7f \n", 
        prodat.ctol, prodat.ntol  );

if ( ctype == SURCUR  )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*retrieve Case SURCUR: \n"); 
  }

fprintf(dbgfil(SURPAC),
"sur904*retrieve s_uvpt %8.4f %8.4f %8.4f uvstart %d\n",
    s_uvpt.x_gm , s_uvpt.y_gm , s_uvpt.z_gm , uvstart );

fprintf(dbgfil(SURPAC),
"sur904*retrieve s_upt  %8.4f %8.4f %8.4f ustart  %d\n",
    s_upt.x_gm , s_upt.y_gm , s_upt.z_gm , ustart );

fprintf(dbgfil(SURPAC),
"sur904*retrieve Patch   us  %8.4f vs  %8.4f ue  %8.4f ve  %8.4f\n",
    us ,  vs ,  ue ,  ve );
fprintf(dbgfil(SURPAC),
"sur904*retrieve Segment usc %8.4f uec %8.4f\n", usc, uec );

fflush(dbgfil(SURPAC)); /* To file from buffer      */
}  /* End dbglev */

#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/







/*!************** Internal ** function ** newstart ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate a new start point i_start= 1, 2, ... nstart           */
/*                                                                  */
          static  short   newstart (p_cur,p_u_par)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur;       /* Curve                             (ptr) */
   DBfloat *p_u_par;     /* Current curve   parameter value   (ptr) */

/*     Global (static) variables:                                   */
/*     i_start, nstart, usc, uec                                    */
/*      .....                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_delta;     /* Delta U for restarts                    */
   DBfloat  usc_e;       /* Start with extra segment                */
   DBfloat  uec_e;       /* End   with extra segment                */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*newstart Input i_start %d nstart %d \n",i_start,nstart);
  }
#endif

/*!                                                                 */
/* 1. Check if all restarts are made                                */
/* __________________________________                               */
/*                                                                 !*/

    if ( i_start == nstart ) 
      {
      i_start  = 10000;
      *p_u_par = 10000.0;
      goto nomore;
      }

if  ( usc >= 2.0 )
  {
  usc_e         = usc - 1.0;
  }
else
  {
  usc_e         = usc;
  }

if  ( uec <= p_cur->ns_cu - 1 )
  {
  uec_e         = uec + 1.0;
  }
else
  {
  uec_e         = uec;
  }

/*!                                                                 */
/* Calculate delta                                                  */
/*                                                                 !*/

   u_delta     = (uec_e-usc_e)/(DBfloat)(nstart+1);
       
/*!                                                                 */
/* Calculate new curve start point                                  */
/*                                                                 !*/

   i_start = i_start + 1;

   *p_u_par = usc_e + (DBfloat)(i_start-1)*u_delta;


nomore:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*newstart i_start %d nstart %d *p_u_par %7.4f usc_e %7.4f  uec_e %7.4f \n",
 i_start,nstart, *p_u_par,  usc_e,  uec_e );
}
#endif

/*!                                                                 */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/







#ifdef  DEBUG
/*!************** Internal ** function ** analyze  ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Analyze the calculation, solution or non-solution, ....         */
/*                                                                  */
   static short analyze (xyz_clo, clo_npts, pla_npts, xyz_p, p_type)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  xyz_clo[SMAX]; /* All varkon_sur_num2 solutions           */
   DBint  clo_npts;      /* Number of xyz_a solutions               */
   DBint  pla_npts;      /* Number of curve /plane solutions        */
   EVALC  xyz_p[SMAX];   /* Array with all curve/plane solutions    */
   DBint *p_type;        /* Type of calculation situation     (ptr) */
                         /* Eq. 1: Surface normal perpendicular     */
                         /* Eq.-1: No classification                */

/*     Global (static) variables:                                   */
/*                                                                  */
/*      xyz_c                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat sur_norm[3];  /* Surface normal                          */
   DBfloat cur_tang[3];  /* Curve tangent                           */
   DBfloat angle;        /* Angle between tangent and surface norm. */
   DBfloat dist_clopla;  /* Distance closest/curve-plane intersect  */
   DBfloat dist_clocur;  /* Distance closest/current curve point    */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 && pla_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  No of curve/plane intersect pts pla_npts %d\n",
       pla_npts );
  }
if ( dbglev(SURPAC) == 1 && pla_npts == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  No of curve/plane intersect pts pla_npts %d (zero)\n",
       pla_npts );
  }
if ( dbglev(SURPAC) == 1 && clo_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  No of closest points            clo_npts %d\n",
       clo_npts );
  }
if ( dbglev(SURPAC) == 1 && clo_npts == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  No of closest points            clo_npts %d (zero)\n",
       clo_npts );
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Surface point  %8.5f %8.5f %8.5f\n",
       xyz_clo[0].r_x , xyz_clo[0].r_y , xyz_clo[0].r_z  );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Curve   point   %12.5f %12.5f %12.5f\n",
       xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm  );
  }

  dist_clopla =
  (xyz_clo[0].r_x - xyz_p[0].r.x_gm)*(xyz_clo[0].r_x - xyz_p[0].r.x_gm)+
  (xyz_clo[0].r_y - xyz_p[0].r.y_gm)*(xyz_clo[0].r_y - xyz_p[0].r.y_gm)+
  (xyz_clo[0].r_z - xyz_p[0].r.z_gm)*(xyz_clo[0].r_z - xyz_p[0].r.z_gm);

  dist_clocur =
  (xyz_clo[0].r_x - xyz_c.r.x_gm)*(xyz_clo[0].r_x - xyz_c.r.x_gm)+
  (xyz_clo[0].r_y - xyz_c.r.y_gm)*(xyz_clo[0].r_y - xyz_c.r.y_gm)+
  (xyz_clo[0].r_z - xyz_c.r.z_gm)*(xyz_clo[0].r_z - xyz_c.r.z_gm);

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Surface UV pt  closest point %8.5f %8.5f \n",
       xyz_clo[0].u   , xyz_clo[0].v    );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Surface normal closest point %8.5f %8.5f %8.5f\n",
       xyz_clo[0].n_x , xyz_clo[0].n_y , xyz_clo[0].n_z  );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Curve   tangent %12.5f %12.5f %12.5f\n",
       xyz_c.drdt.x_gm, xyz_c.drdt.y_gm, xyz_c.drdt.z_gm  );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*analyze  Curve   U value %12.5f \n",
       xyz_c.t_global  );
  }

/*!                                                                 */
/* Initializations                                                  */
/*                                                                 !*/

   *p_type = -1;

/*!                                                                 */
/* Calculate angle                                                  */
/*                                                                 !*/

sur_norm[0] = xyz_clo[0].n_x;
sur_norm[1] = xyz_clo[0].n_y;
sur_norm[2] = xyz_clo[0].n_z;
cur_tang[0] = xyz_c.drdt.x_gm; 
cur_tang[1] = xyz_c.drdt.y_gm; 
cur_tang[2] = xyz_c.drdt.z_gm; 
varkon_angd(sur_norm,cur_tang,&angle);

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*analyze  Angle %8.2f objf %15.8f\n",
     angle, objf  );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*analyze  Distance Closest to plane/curve intersect %15.8f \n",
     dist_clopla  );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*analyze  Distance Closest to external curve point  %15.8f \n",
     dist_clocur  );
}

       
    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif



#ifdef  DEBUG
/*!************** Internal ** function ** curfail  ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of data for curve/plane calculation failure             */
/*                                                                  */
   static short curfail (pla_npts )
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  pla_npts;      /* Number of curve /plane solutions        */

/*     Global (static) variables:                                   */
/*                                                                  */
/*      xyz_c                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*TABORT DBfloat  cur_tang[3]; */  /* Curve tangent                 */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 && pla_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*curfail  No of curve/plane intersect pts pla_npts %d\n",
       pla_npts );
  }
if ( dbglev(SURPAC) == 1 && pla_npts == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*curfail  No of curve/plane intersect pts pla_npts %d (zero)\n",
       pla_npts );
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*curfail  Curve   point   %12.5f %12.5f %12.5f\n",
       xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm  );
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*curfail  Curve   tangent %12.5f %12.5f %12.5f\n",
       xyz_c.drdt.x_gm, xyz_c.drdt.y_gm, xyz_c.drdt.z_gm  );
  }


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif




#ifdef  DEBUG
/*!************** Internal ** function ** clofail  ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of data for closest point calculation failure           */
/*                                                                  */
   static short clofail (xyz_clo, clo_npts)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  xyz_clo[SMAX]; /* All varkon_sur_num2 solutions           */
   DBint  clo_npts;      /* Number of xyz_a solutions               */

/*     Global (static) variables:                                   */
/*                                                                  */
/*      xyz_c                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*TABORT DBfloat  sur_norm[3]; *//* Surface normal                  */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 && clo_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail  No of closest points            clo_npts %d\n",
       clo_npts );
  }
if ( dbglev(SURPAC) == 1 && clo_npts == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail  No of closest points            clo_npts %d (zero)\n",
       clo_npts );
  }


if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail External point sprec.extpt   %12.5f %12.5f %12.5f\n",
       sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm  );
  }


if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail Start point sprec.startuvpt  %12.8f %12.8f %12.8f\n",
   sprec.startuvpt.x_gm, sprec.startuvpt.y_gm, sprec.startuvpt.z_gm  );
  }



if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail  Curve   point   %12.5f %12.5f %12.5f\n",
       xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm  );
  }


if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail  Surface point  %8.5f %8.5f %8.5f\n",
       xyz_clo[0].r_x , xyz_clo[0].r_y , xyz_clo[0].r_z  );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*clofail  Surface normal %8.5f %8.5f %8.5f\n",
       xyz_clo[0].n_x , xyz_clo[0].n_y , xyz_clo[0].n_z  );
  }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif



#ifdef  DEBUG
/*!************** Internal ** function ** linfail  ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of data for line/surface  calculation failure           */
/*                                                                  */
   static short linfail (xyz_clo, clo_npts)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  xyz_clo[SMAX]; /* All varkon_sur_num2 solutions           */
   DBint  clo_npts;      /* Number of xyz_a solutions               */

/*     Global (static) variables:                                   */
/*                                                                  */
/*      xyz_c                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/* TABORT ?? DBfloat  sur_norm[3]; */  /* Surface normal            */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 && clo_npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail  No of line/surface intersect pts clo_npts %d\n",
       clo_npts );
  }
if ( dbglev(SURPAC) == 1 && clo_npts == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail  No of line/surface intersect pts clo_npts %d (zero)\n",
       clo_npts );
  }


if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail External point     sprec.extpt   %12.5f %12.5f %12.5f\n",
       sprec.extpt.x_gm, sprec.extpt.y_gm, sprec.extpt.z_gm  );
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail Shooting direction sprec.proj    %12.5f %12.5f %12.5f\n",
       sprec.proj.x_gm, sprec.proj.y_gm, sprec.proj.z_gm  );
  }


if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail Start point     sprec.startuvpt  %12.8f %12.8f %12.8f\n",
   sprec.startuvpt.x_gm, sprec.startuvpt.y_gm, sprec.startuvpt.z_gm  );
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur904*linfail Matrix lindat.pat_tra %10.5f %10.5f %10.5f (X axis)\n",
  lindat.pat_tra.g11,lindat.pat_tra.g12, lindat.pat_tra.g13 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail  Curve   point   %12.5f %12.5f %12.5f\n",
       xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm  );
  }


if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail  Surface point  %8.5f %8.5f %8.5f\n",
       xyz_clo[0].r_x , xyz_clo[0].r_y , xyz_clo[0].r_z  );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur904*linfail  Surface normal %8.5f %8.5f %8.5f\n",
       xyz_clo[0].n_x , xyz_clo[0].n_y , xyz_clo[0].n_z  );
  }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif






