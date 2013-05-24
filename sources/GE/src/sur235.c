/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
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
/*  Function: varkon_pat_loftcur                   File: sur235.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates limit, tangent, mid curve and           */
/*  spine curve coordinates and derivatives for a given             */
/*  parameter ( u,v ) point on a conic lofting patch.               */
/*                                                                  */
/*  Note that the P value function is a (scalar) function of        */
/*  the spine length and the derivative dP/ds can be used           */
/*  in for instance CONSURF. But the arcclength (X for the          */
/*  the scalar function) is for the spine of the patch. Not         */
/*  for the spine of the whole surface.                             */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-01-28   Originally written                                 */
/*  1996-05-28   Declaration of normv                               */
/*  1997-07-08   Error for rotation surface: Not yet implemented    */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_loftcur    Conic lofting curves evaluation  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short spipla();        /* Spine plane for input u value      */
static short intspi();        /* Intersects longitudinal curves     */
static short genpoi();        /* Calculate generatrix point         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  plane[4];         /* Spine plane for 3D curves      */
static DBfloat  pval_plane[4];    /* Spine plane for 2D functions   */
static DBfloat  outlims[16];      /* Coord. & derivat. for lims     */
static DBfloat  outlime[16];      /* Coord. & derivat. for lime     */
static DBfloat  outtans[16];      /* Coord. & derivat. for tans     */
static DBfloat  outtane[16];      /* Coord. & derivat. for tane     */
static DBfloat  outmidc[16];      /* Coord. & derivat. for midc     */
static DBfloat  outpval[16];      /* Coord. & derivat. for P fctn   */
static DBfloat  outgener[16];     /* Coord. & derivat. for generatr.*/
static DBfloat  outspine[16];     /* Coord. & derivat. for spine    */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE136                  * Rational cubic interm. pt               */
/* GE107                  * Curve segment evaluation function       */
/* GE101                  * Curve segment/plane intersect           */
/* varkon_normv           * Vector normalization                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_loftcur   */
/* SU2882 = sur235 No spline plane intersects with .. is .. (not 1) */
/* SU2993 = Severe program error in varkon_pat_loftcur (sur235).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus  varkon_pat_loftcur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL   *p_patl,     /* Pointer to the conic lofting patch      */
   DBfloat   u_pat,      /* Patch (local) U parameter value         */
   DBfloat   v_pat,      /* Patch (local) V parameter value         */
   DBVector *p_s_r,      /* Spine   start point               (ptr) */
   DBVector *p_s_drdt,   /* Spine   start tangent             (ptr) */
   DBVector *p_s_d2rdt2, /* Spine   start second derivative   (ptr) */
   DBVector *p_ls_r,     /* Limit   start point               (ptr) */
   DBVector *p_ls_drdt,  /* Limit   start tangent             (ptr) */
   DBVector *p_ls_d2rdt2,/* Limit   start second derivative   (ptr) */
   DBVector *p_ts_r,     /* Tangent start point               (ptr) */
   DBVector *p_ts_drdt,  /* Tangent start tangent             (ptr) */
   DBVector *p_ts_d2rdt2,/* Tangent start second derivative   (ptr) */
   DBVector *p_m_r,      /* Mid     start point               (ptr) */
   DBVector *p_m_drdt,   /* Mid     start tangent             (ptr) */
   DBVector *p_m_d2rdt2, /* Mid     start second derivative   (ptr) */
   DBint    *p_m_f,      /* Mid flag  1: P value  2: Mid crv  (ptr) */
   DBVector *p_le_r,     /* Limit   end   point               (ptr) */
   DBVector *p_le_drdt,  /* Limit   end   tangent             (ptr) */
   DBVector *p_le_d2rdt2,/* Limit   end   second derivative   (ptr) */
   DBVector *p_te_r,     /* Tangent end   point               (ptr) */
   DBVector *p_te_drdt,  /* Tangent end   tangent             (ptr) */
   DBVector *p_te_d2rdt2,/* Tangent end   second derivative   (ptr) */
   DBVector *p_g_r,      /* Gener.  start point               (ptr) */
   DBVector *p_g_drdt,   /* Gener.  start tangent             (ptr) */
   DBVector *p_g_d2rdt2 )/* Gener.  start second derivative   (ptr) */

/* Out:                                                             */
/*       Data to ......                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur235 Enter *** varkon_pat_loftcur p_patl %d u %f v %f \n",
                p_patl,u_pat,v_pat);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

/*!                                                                 */
/* Rotation LFT_SUR not yet implemented                             */
/*                                                                 !*/

   *p_m_f = I_UNDEF;
   if      ( p_patl->p_flag == 3 ) 
     {
     *p_m_f = 3;
     sprintf(errbuf," %%sur235");
     return(varkon_erpush("SU8073",errbuf));
     }

/*!                                                                 */
/* 2. Spine plane                                                   */

/* Calculate the spine plane for the input u_pat value              */
/* Call of spipla.                       Error SU2973 for failure.  */
/*                                                                 !*/
   status=spipla (p_patl,u_pat); 
   if (status<0) 
   {
   sprintf(errbuf,"spipla%%varkon_pat_loftcur (sur235)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 3. Longitudinal intersection points                              */

/* Make intersection with start limit curve, end limit curve,       */
/* start tangent curve, end tangent curve and P value curve         */
/* Call of intspi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   status=intspi (p_patl); 
   if (status<0) 
   {
   sprintf(errbuf,"intspi%%varkon_pat_loftcur (sur235)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 3. Generatrix point                                              */

/* Calculate the generatrix point for the input v_pat value         */
/* Call of genpoi.                       Error SU2973 for failure.  */
/*                                                                 !*/

   status=genpoi (p_patl,v_pat); 
   if (status<0) 
   {
   sprintf(errbuf,"genpoi%%varkon_pat_loftcur (sur235)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 4. Data to output variables                                      */
/*                                                                 !*/

   p_s_r->x_gm      = outspine[0];
   p_s_r->y_gm      = outspine[1];
   p_s_r->z_gm      = outspine[2];
   p_s_drdt->x_gm   = outspine[3];
   p_s_drdt->y_gm   = outspine[4];
   p_s_drdt->z_gm   = outspine[5];
   p_s_d2rdt2->x_gm = outspine[6];
   p_s_d2rdt2->y_gm = outspine[7];
   p_s_d2rdt2->z_gm = outspine[8];

   p_ls_r->x_gm      = outlims[0];
   p_ls_r->y_gm      = outlims[1];
   p_ls_r->z_gm      = outlims[2];
   p_ls_drdt->x_gm   = outlims[3];
   p_ls_drdt->y_gm   = outlims[4];
   p_ls_drdt->z_gm   = outlims[5];
   p_ls_d2rdt2->x_gm = outlims[6];
   p_ls_d2rdt2->y_gm = outlims[7];
   p_ls_d2rdt2->z_gm = outlims[8];

   if      ( p_patl->p_flag == 2 ) 
     {
     p_m_r->x_gm      = outmidc[0];
     p_m_r->y_gm      = outmidc[1];
     p_m_r->z_gm      = outmidc[2];
     p_m_drdt->x_gm   = outmidc[3];
     p_m_drdt->y_gm   = outmidc[4];
     p_m_drdt->z_gm   = outmidc[5];
     p_m_d2rdt2->x_gm = outmidc[6];
     p_m_d2rdt2->y_gm = outmidc[7];
     p_m_d2rdt2->z_gm = outmidc[8];
     }
   else
     {
     p_m_r->x_gm      = outpval[0];
     p_m_r->y_gm      = outpval[1];
     p_m_r->z_gm      = outpval[2];
     p_m_drdt->x_gm   = outpval[3];
     p_m_drdt->y_gm   = outpval[4];
     p_m_drdt->z_gm   = outpval[5];
     p_m_d2rdt2->x_gm = outpval[6];
     p_m_d2rdt2->y_gm = outpval[7];
     p_m_d2rdt2->z_gm = outpval[8];
     }

   *p_m_f = p_patl->p_flag;
   p_ts_r->x_gm      = outtans[0];
   p_ts_r->y_gm      = outtans[1];
   p_ts_r->z_gm      = outtans[2];
   p_ts_drdt->x_gm   = outtans[3];
   p_ts_drdt->y_gm   = outtans[4];
   p_ts_drdt->z_gm   = outtans[5];
   p_ts_d2rdt2->x_gm = outtans[6];
   p_ts_d2rdt2->y_gm = outtans[7];
   p_ts_d2rdt2->z_gm = outtans[8];

   p_le_r->x_gm      = outlime[0];
   p_le_r->y_gm      = outlime[1];
   p_le_r->z_gm      = outlime[2];
   p_le_drdt->x_gm   = outlime[3];
   p_le_drdt->y_gm   = outlime[4];
   p_le_drdt->z_gm   = outlime[5];
   p_le_d2rdt2->x_gm = outlime[6];
   p_le_d2rdt2->y_gm = outlime[7];
   p_le_d2rdt2->z_gm = outlime[8];

   p_te_r->x_gm      = outtane[0];
   p_te_r->y_gm      = outtane[1];
   p_te_r->z_gm      = outtane[2];
   p_te_drdt->x_gm   = outtane[3];
   p_te_drdt->y_gm   = outtane[4];
   p_te_drdt->z_gm   = outtane[5];
   p_te_d2rdt2->x_gm = outtane[6];
   p_te_d2rdt2->y_gm = outtane[7];
   p_te_d2rdt2->z_gm = outtane[8];

   p_g_r->x_gm      = outgener[0];
   p_g_r->y_gm      = outgener[1];
   p_g_r->z_gm      = outgener[2];
   p_g_drdt->x_gm   = outgener[3];
   p_g_drdt->y_gm   = outgener[4];
   p_g_drdt->z_gm   = outgener[5];
   p_g_d2rdt2->x_gm = outgener[6];
   p_g_d2rdt2->y_gm = outgener[7];
   p_g_d2rdt2->z_gm = outgener[8];

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur235 ls_r       %f %f %f\n",p_ls_r->x_gm,p_ls_r->y_gm,p_ls_r->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 ts_r       %f %f %f\n",p_ts_r->x_gm,p_ts_r->y_gm,p_ts_r->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  m_r       %f %f %f\n", p_m_r->x_gm, p_m_r->y_gm, p_m_r->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 le_r       %f %f %f\n",p_le_r->x_gm,p_le_r->y_gm,p_le_r->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 te_r       %f %f %f\n",p_te_r->x_gm,p_te_r->y_gm,p_te_r->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  g_r       %f %f %f\n", p_g_r->x_gm, p_g_r->y_gm, p_g_r->z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235 ls_drdt    %f %f %f\n"
     ,p_ls_drdt->x_gm,p_ls_drdt->y_gm,p_ls_drdt->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 ts_drdt    %f %f %f\n"
     ,p_ts_drdt->x_gm,p_ts_drdt->y_gm,p_ts_drdt->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  m_drdt    %f %f %f\n"
     , p_m_drdt->x_gm, p_m_drdt->y_gm, p_m_drdt->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 le_drdt    %f %f %f\n"
     ,p_le_drdt->x_gm,p_le_drdt->y_gm,p_le_drdt->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 te_drdt    %f %f %f\n"
     ,p_te_drdt->x_gm,p_te_drdt->y_gm,p_te_drdt->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  g_drdt    %f %f %f\n"
     , p_g_drdt->x_gm, p_g_drdt->y_gm, p_g_drdt->z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235 ls_d2rdt2  %f %f %f\n"
     ,p_ls_d2rdt2->x_gm,p_ls_d2rdt2->y_gm,p_ls_d2rdt2->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 ts_d2rdt2  %f %f %f\n"
     ,p_ts_d2rdt2->x_gm,p_ts_d2rdt2->y_gm,p_ts_d2rdt2->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  m_d2rdt2  %f %f %f\n"
     , p_m_d2rdt2->x_gm, p_m_d2rdt2->y_gm, p_m_d2rdt2->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 le_d2rdt2  %f %f %f\n"
     ,p_le_d2rdt2->x_gm,p_le_d2rdt2->y_gm,p_le_d2rdt2->z_gm);
fprintf(dbgfil(SURPAC),
"sur235 te_d2rdt2  %f %f %f\n"
     ,p_te_d2rdt2->x_gm,p_te_d2rdt2->y_gm,p_te_d2rdt2->z_gm);
fprintf(dbgfil(SURPAC),
"sur235  g_d2rdt2  %f %f %f\n"
     , p_g_d2rdt2->x_gm, p_g_d2rdt2->y_gm, p_g_d2rdt2->z_gm);
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
/* Calculate the spine plane                                        */

   static short spipla (p_patl,u_pat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL  *p_patl;      /* Pointer to the conic lofting patch      */
   DBfloat  u_pat;       /* Patch (local) U parameter value         */

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
   DBint  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235*spipla Parameter U= %f Spine addresse &p_patl->spine= %d\n"
     ,u_pat, &p_patl->spine);
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
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
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
        sprintf(errbuf,"GE107%%varkon_pat_loftcur (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235*spipla Spine point   %f %f %f \n",
        outspine[0], outspine[1], outspine[2]  );
fprintf(dbgfil(SURPAC),
"sur235*spipla Spine tangent %f %f %f \n",
        outspine[3], outspine[4], outspine[5]  );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Spine plane for 3D curves                                     */

/* Let the spine plane normal be the normalized tangent             */
/* Call of varkon_normv.                                            */
/* Let the plane coefficient d= scalar product of the spine         */
/* plane normal and (the vector origin to) the spine point          */
/*                                                                 !*/

   plane[0] = outspine[3];
   plane[1] = outspine[4];
   plane[2] = outspine[5];

   status=varkon_normv(plane);
   if (status<0) 
        {
        sprintf(errbuf,"normv%%varkon_pat_loftcur (spipla)");
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
"sur235*spipla Spine plane 3D nx %f nx %f nx %f d %f\n",
        plane[0], plane[1], plane[2], plane[3] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur235*spipla Spine plane 2D nx %f nx %f nx %f d %f\n",
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

   static short intspi (p_patl)
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
   short    nstart;      /* The number of restarts (=RESTRT except  */
                         /* for arcs where nstart=RESTRT+1 )        */
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
   DBint  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235*intspi Intersect calculation for the input u_pat plane \n");
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235*intspi addresses lims %d lime %d tans %d tane %d   \n"
     ,&p_patl->lims,&p_patl->lime,&p_patl->tans,&p_patl->tane );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */

/* Initializations for GE101 and GE107.                             */
   rcode = 3;
   scur.hed_cu.type    = CURTYP;
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
   nstart = 2;
/*                                                                 !*/


/*!                                                                 */
/* 2. Coordinates and derivatives for the start limit curve (lims)  */

/* Intersection.     Call of GE101.    Error SU2943  for failure.  */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.  */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.  */
/*                                                                 !*/
   status=GE101((DBAny *)&scur,&p_patl->lims,plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for lims Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (lims)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for lims Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (lims)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of lims intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"lims%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }

   status=GE107((GMUNON *)&scur,&p_patl->lims ,t_l,rcode,outlims);

/* 3. Coordinates and derivatives for the end   limit curve (lime)  */
/* Intersection.     Call of GE101.    Error SU2943  for failure.   */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.   */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.   */
/*                                                                 !*/
   status=GE101((DBAny *)&scur,&p_patl->lime,plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for lime Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (lime)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for lime Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (lime)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ???   if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of lime intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"lime%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }
       status=GE107((GMUNON *)&scur,&p_patl->lime ,t_l,rcode,outlime);

/*!                                                                 */
/* 4. Coordinates and derivatives for the start tangent curve (tans)*/
/* Intersection.     Call of GE101.    Error SU2943  for failure.   */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.   */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.   */
/*                                                                 !*/
   status=GE101((DBAny *)&scur,&p_patl->tans,plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for tans Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (tans)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for tans Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (tans)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ?? if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of tans intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"tans%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }

   status=GE107((GMUNON *)&scur,&p_patl->tans ,t_l,rcode,outtans);

/*!                                                                 */
/* 5. Coordinates and derivatives for the end tangent curve (tane)  */

/* Intersection.     Call of GE101.    Error SU2943  for failure.   */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.   */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.   */
/*                                                                 !*/
   status=GE101((DBAny *)&scur,&p_patl->tane,plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for tane Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (tane)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for tane Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (tane)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ?? if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of tane intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"tane%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }

   status=GE107((GMUNON *)&scur,&p_patl->tane ,t_l,rcode,outtane);
   if (status<0) 
      {
      sprintf(errbuf,"GE107 (tane)%%varkon_pat_loftcur");
      return(varkon_erpush("SU2943",errbuf));
      }


/*!                                                                 */
/* 6. Coordinates and derivatives for the mid curve (midc)          */

/* For a P value function and not a mid curve: Goto pval.           */
/* Intersection.     Call of GE101.    Error SU2943  for failure.   */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.   */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.   */
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
     sprintf(errbuf,"Invalid p_flag%%varkon_pat_loftcur (sur235)");
     return(varkon_erpush("SU2993",errbuf));
     }
   
   
   status=GE101((DBAny *)&scur,&p_patl->midc,plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for midc Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GE101 (midc)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for midc Plane d= %f\n",plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (midc)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ?? if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of midc intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"midc%%%d" ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }


   status=GE107((GMUNON *)&scur,&p_patl->midc ,t_l,rcode,outmidc);
   if (status<0) 
      {
      sprintf(errbuf,"GE107 (midc)%%varkon_pat_loftcur");
      return(varkon_erpush("SU2943",errbuf));
      }


   if ( p_patl->p_flag == 2 ) goto epval;

/*!                                                                 */
/* 7. Coordinates and derivatives for the P value function  (pval)  */

pval:   /* Label: P value function                                  */

/* Intersection.     Call of GE101.    Error SU2943  for failure.   */
/*                   A 2D function and pval_plane is used.          */
/* Delete and order. Call of GEsort_1. Error SU2943  for failure.   */
/* Evaluation.       Call of GE107.    Error SU2943  for failure.   */
/*                                                                 !*/

   status = GE101
    ((DBAny *)&scur,&p_patl->pval,pval_plane,nstart,&noall,useg);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi GE101 failed for pval Plane d= %f\n",pval_plane[3] );
#endif
        sprintf(errbuf,"GE101 (pval)%%varkon_pat_loftcur");
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
"sur235*intspi GEsort_1 failed for pval Plane d= %f\n",pval_plane[3] );
#endif
        sprintf(errbuf,"GEsort_1 (pval)%%varkon_pat_loftcur");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else 
     {
     noinse = noall;
     }

/* Wrong tolerance ?? if ( noinse == 1 )  */
   if ( noinse >= 1 )
     {
     t_l = useg[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur235*intspi No of pval intersects is %d (not 1) Plane d= %f\n"
               ,noinse,plane[3] );
#endif
     sprintf(errbuf,"pval%%%d"
            ,noinse);
     return(varkon_erpush("SU2882",errbuf));
     }


   status=GE107((GMUNON *)&scur,&p_patl->pval ,t_l,rcode,outpval);
   if (status<0) 
      {
      sprintf(errbuf,"GE107 (pval)%%varkon_pat_loftcur");
      return(varkon_erpush("SU2943",errbuf));
      }

epval:   /* Label: End P value function                             */


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

   static short genpoi (p_patl,v_pat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL  *p_patl;      /* Pointer to the conic lofting patch      */
   DBfloat  v_pat;       /* Patch (local) V parameter value         */

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
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   DBfloat  p_value;     /* P value for the generatrix              */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */

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

/*!                                                                 */
/*  Call of GE133 if p_patl->p_flag = 1                             */
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
"sur235*genpoi GE133 failed p_value %f\n",
        p_value  );
}
#endif
         sprintf(errbuf,"GE133%%varkon_pat_loftcur (sur235)");
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
"sur235*genpoi GE136 failed ipt %f %f %f\n",
        ipt.x_gm, ipt.y_gm, ipt.z_gm  );
}
#endif
         sprintf(errbuf,"%8.2f %8.2f %8.2f%%varkon_pat_loftcur",
          ipt.x_gm, ipt.y_gm, ipt.z_gm  );
         return(varkon_erpush("SU2823",errbuf));
         }
       }
    else
       {
       sprintf(errbuf,"Unknown p_flag%%varkon_pat_loftcur (sur235)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* 3. Generatrix point                                              */

/* Calculate point on generatrix for the given v_pat value          */
/* Call of GE107.                                                  */
/*                                                                 !*/

t_l = v_pat;
  status=GE107((GMUNON *)&scur,&gener,t_l,rcode,outgener);
  if (status<0) 
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur235*genpoi GE107 failed t_l %f \n",
        t_l                  );
}
#endif
    sprintf(errbuf,"GE107%%varkon_pat_loftcur (genpoi)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur235 gen. %8.2f %8.2f %8.2f %10.4f %10.4f %10.4f \n",
        outgener[0], outgener[1], outgener[2],
        outgener[3], outgener[4], outgener[5]  );
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/


