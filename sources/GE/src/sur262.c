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

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_pat_hardptcre2                File: sur262.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a hard point curve segment as a            */
/*  rational cubic curve defined by three points and two            */
/*  tangents. The output curve segment should only be               */
/*  used in the parameter interval u_s to u_e (input to             */
/*  the function).                                                  */
/*                                                                  */
/*  The four input rational cubic segments will be approximated     */
/*  by four non-rational cubic segments in the parameter            */
/*  interval u_s to u_e. Two ruled surfaces will be created         */
/*  from these curves.                                              */
/*                                                                  */
/*  The output curve segment is an approximation of the             */
/*  intersect curve between the two ruled surfaces.                 */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-26   Originally written                                 */
/*  1994-10-22   Ruled bicubic surface in interval u_s              */
/*               to u_e, debug, comments, ....                      */
/*  1998-02-28   _cur_linlind -> _lin_linlind, declaration angd     */
/*  1999-12-18   Free source code modifications GMPATR->GMPATC      */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_hardptcre2 Epsilon hardpoint curve segment  */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short cubappr();       /* Approximate to cubic segment       */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_pat_biccre2       * Create ruled bicubic patch            */
/* varkon_pat_biceval       * Evaluation of bicubic patch           */
/* varkon_pat_norm          * Normal with derivatives               */
/* varkon_lin_linlind       * Distance between two lines            */
/* varkon_lengv             * Length of a vector                    */
/* varkon_xprod             * Cross vector product.                 */
/* GE136                    * Rational cubic interm. pt             */
/* GE133                    * Rational cubic with P-value           */
/* varkon_cur_reparam       * Reparam. a rational segment           */
/* varkon_seg_parabola      * Parabola interp. of 3 pts             */
/* varkon_angd              * Calculate angle For Debug On          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Function xxxxxx failed in varkon_pat_hardptcre2 (sur262)*/
/* SU2993 = Severe program error in varkon_pat_hardptcre2 (sur262). */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus  varkon_pat_hardptcre2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg   *p_l1,        /* Limit segment for ruled surface 1 (ptr) */
   DBSeg   *p_t1,        /* Tang. segment for ruled surface 1 (ptr) */
   DBSeg   *p_l2,        /* Limit segment for ruled surface 2 (ptr) */
   DBSeg   *p_t2,        /* Tang. segment for ruled surface 2 (ptr) */
   DBfloat  u_s,         /* Parameter value for start section       */
   DBfloat  u_e,         /* Parameter value for end   section       */
   DBSeg   *p_hp )       /* Hard point segment for u_e to u_s (ptr) */

/* Out:                                                             */
/*       Data to p_hp                                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBSeg  l1_a;          /* Limit approx. segment for surface 1     */
   DBSeg  t1_a;          /* Tang. approx. segment for surface 1     */
   DBSeg  l2_a;          /* Limit approx. segment for surface 2     */
   DBSeg  t2_a;          /* Tang. approx. segment for surface 2     */
   GMPATC rul1;          /* Tangent, ruled surface 1                */
   GMPATC rul2;          /* Tangent, ruled surface 2                */
   DBfloat  u_pat;       /* Patch (local) U parameter value         */
   DBfloat  v_pat;       /* Patch (local) V parameter value         */
   EVALS  xyz;           /* Coordinates and derivatives for a       */
                         /* point on a surface patch                */
   DBVector p0_rul1;     /* Start point      for ruling u0 in rul1  */
   DBVector u0_rul1;     /* Direction vector for ruling u0 in rul1  */
   DBVector p0_rul2;     /* Start point      for ruling u0 in rul2  */
   DBVector u0_rul2;     /* Direction vector for ruling u0 in rul2  */
   DBVector pm_rul1;     /* Start point      for ruling um in rul1  */
   DBVector um_rul1;     /* Direction vector for ruling um in rul1  */
   DBVector pm_rul2;     /* Start point      for ruling um in rul2  */
   DBVector um_rul2;     /* Direction vector for ruling um in rul2  */
   DBVector p1_rul1;     /* Start point      for ruling u1 in rul1  */
   DBVector u1_rul1;     /* Direction vector for ruling u1 in rul1  */
   DBVector p1_rul2;     /* Start point      for ruling u1 in rul2  */
   DBVector u1_rul2;     /* Direction vector for ruling u1 in rul2  */
   DBfloat  n_rul1[3];   /* Surface normal for hardpoint in rul1    */
   DBfloat  n_rul2[3];   /* Surface normal for hardpoint in rul2    */
   DBVector u0hp;        /* The hard point for the start U section  */
   DBVector umhp;        /* The hard point for the mid   U section  */
   DBVector u1hp;        /* The hard point for the end   U section  */
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   DBVector ipt;         /* Intermediate point                      */
   short  p_flag;        /* p_flag=1: Non-parallell   =2: Parallell */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   gmint   icase;        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBVector pn_rul1;     /* Nearest point    on  ruling    in rul1  */
   DBVector pn_rul2;     /* Nearest point    on  ruling    in rul2  */
   DBfloat l1;           /* Parameter for unit vector (rul1 direc.) */
   DBfloat l2;           /* Parameter for unit vector (rul2 direc.) */
   DBfloat u0_ruling1[3];/* Ruling direction vector with real length*/
   DBfloat u0_ruling2[3];/* Ruling direction vector with real length*/
   DBfloat u1_ruling1[3];/* Ruling direction vector with real length*/
   DBfloat u1_ruling2[3];/* Ruling direction vector with real length*/
   DBfloat rul_length;   /* Length of ruling                        */
   DBfloat rul_dist;     /* Shortest distance between rulings       */
   DBfloat cross_v[3];   /* Cross vector product                    */
   DBfloat us_in;        /* Parameter value for input  start point  */
   DBfloat ue_in;        /* Parameter value for input  end   point  */
   DBfloat us_out;       /* Parameter value for output start point  */
   DBfloat ue_out;       /* Parameter value for output end   point  */
   gmint  u_case;        /* Parameterisation case:                  */
                         /* Eq. 1: Not yet programmed ...           */
                         /* Eq. 2: Mid point for u= 0.5             */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

#ifdef DEBUG
   DBfloat  check_v[3];  /* Check vector                            */
   DBfloat  che_length;  /* Check vector length                     */
   DBSeg  hp;            /* Hard point segment defined as parabola  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat    out[16];   /* Coordinates and derivatives for crv pt  */
   DBfloat    t_l;       /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
   DBfloat  start_tr[3]; /* Check start vector for rational segm.   */
   DBfloat  start_tp[3]; /* Check start vector for 3 pt parabola.   */
   DBfloat  che_angle ;  /* Check angle between tangents            */
#endif

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur262 Enter *** varkon_pat_hardptcre2: Ruled surfaces intersect\n");
  fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur262 Surfaces defined by p_l1 %d p_t1 %d p_l2 %d p_t2 %d\n",
        (int)p_l1, (int)p_t1, (int)p_l2 , (int)p_t2);
}
#endif

/*!                                                                 */
/* 1. Initializations                                               */
/* __________________                                               */
/*                                                                  */
/* Flag for parallell lines. Start value not parallell:             */
   p_flag = 1;
/*                                                                 !*/

   icase = 9;                /* For varkon_sur_eval (sur210)        */

/* Initiate variables for parallell plane case (p_case=2)           */
   u0hp.x_gm = F_UNDEF;     /* Hard point for the start U section   */
   u0hp.y_gm = F_UNDEF;   
   u0hp.z_gm = F_UNDEF;   
   umhp.x_gm = F_UNDEF;     /* Hard point for the mid   U section   */
   umhp.y_gm = F_UNDEF;   
   umhp.z_gm = F_UNDEF;   
   u1hp.x_gm = F_UNDEF;     /* Hard point for the end   U section   */
   u1hp.y_gm = F_UNDEF;   
   u1hp.z_gm = F_UNDEF;   

/*!                                                                 */
/* 2. Create the ruled surfaces                                     */
/* ____________________________                                     */
/*                                                                  */
/* Approximate the input rational curve segments with non-          */
/* rational cubic segments in the interval u_s to u_e.              */
/* Calls of internal function cubappr.                              */
/*                                                                 !*/

   status=cubappr (p_l1,u_s,u_e,&l1_a); 
   if (status<0) 
   {
   sprintf(errbuf,"cubappr p_l1%%varkon_pat_hardptcre2 (sur262)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=cubappr (p_t1,u_s,u_e,&t1_a); 
   if (status<0) 
   {
   sprintf(errbuf,"cubappr p_t1%%varkon_pat_hardptcre2 (sur262)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=cubappr (p_l2,u_s,u_e,&l2_a); 
   if (status<0) 
   {
   sprintf(errbuf,"cubappr p_l2%%varkon_pat_hardptcre2 (sur262)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=cubappr (p_t2,u_s,u_e,&t2_a); 
   if (status<0) 
   {
   sprintf(errbuf,"cubappr p_t2%%varkon_pat_hardptcre2 (sur262)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* Create ruled tangent surfaces 1 and 2 between the whole          */
/* input curves (from parameter 0 to 1 for the curves).             */
/* Calls of varkon_pat_biccre2 (sur256).                            */
/*                                                                 !*/


status=varkon_pat_biccre2 (&l1_a, &t1_a, &rul1 );
if (status<0) 
    {
    sprintf(errbuf,"sur256(rul1)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

status=varkon_pat_biccre2 (&l2_a, &t2_a, &rul2 );
if (status<0) 
    {
    sprintf(errbuf,"sur256(rul2)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* 2. Hard points and derivatives                                   */
/* ______________________________                                   */
/*                                                                  */
/* For the start U section:                                         */
/* Calculate start and end points for rulings in the                */
/* bicubic (epsilon) surfaces rul1 and rul2.                        */
/* Calls of varkon_pat_biceval (sur221) for parameter               */
/* values:                                                          */
/* (u,v ) = (0.0 , 0.0) for line:  p0_rul1 + t*u0_rul1              */
/*      and             for line:  p0_rul2 + t*u0_rul2              */
/* (u,v ) = (0.5 , 0.0) for line:  pm_rul1 + t*um_rul1              */
/*      and             for line:  pm_rul2 + t*um_rul2              */
/* (u,v ) = (1.0 , 0.0) for line:  p1_rul1 + t*u1_rul1              */
/*      and             for line:  p1_rul2 + t*u1_rul2              */
/*                                                                 !*/


   u_pat = 0.0;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul1, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(1)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   p0_rul1.x_gm = xyz.r_x;
   p0_rul1.y_gm = xyz.r_y;
   p0_rul1.z_gm = xyz.r_z;

   u0_rul1.x_gm = xyz.v_x;
   u0_rul1.y_gm = xyz.v_y;
   u0_rul1.z_gm = xyz.v_z;

   u0_ruling1[0]   = xyz.v_x;
   u0_ruling1[1]   = xyz.v_y;
   u0_ruling1[2]   = xyz.v_z;

   u_pat = 0.0;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul2, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(2)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   p0_rul2.x_gm = xyz.r_x;
   p0_rul2.y_gm = xyz.r_y;
   p0_rul2.z_gm = xyz.r_z;

   u0_rul2.x_gm = xyz.v_x;
   u0_rul2.y_gm = xyz.v_y;
   u0_rul2.z_gm = xyz.v_z;


   u0_ruling2[0]   = xyz.v_x;
   u0_ruling2[1]   = xyz.v_y;
   u0_ruling2[2]   = xyz.v_z;


   u_pat = 0.5;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul1, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(1)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   pm_rul1.x_gm = xyz.r_x;
   pm_rul1.y_gm = xyz.r_y;
   pm_rul1.z_gm = xyz.r_z;

   um_rul1.x_gm = xyz.v_x;
   um_rul1.y_gm = xyz.v_y;
   um_rul1.z_gm = xyz.v_z;

   u_pat = 0.5;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul2, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(2)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   pm_rul2.x_gm = xyz.r_x;
   pm_rul2.y_gm = xyz.r_y;
   pm_rul2.z_gm = xyz.r_z;

   um_rul2.x_gm = xyz.v_x;
   um_rul2.y_gm = xyz.v_y;
   um_rul2.z_gm = xyz.v_z;


   u_pat = 1.0;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul1, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(1)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   p1_rul1.x_gm = xyz.r_x;
   p1_rul1.y_gm = xyz.r_y;
   p1_rul1.z_gm = xyz.r_z;

   u1_rul1.x_gm = xyz.v_x;
   u1_rul1.y_gm = xyz.v_y;
   u1_rul1.z_gm = xyz.v_z;

   u1_ruling1[0]   = xyz.v_x;
   u1_ruling1[1]   = xyz.v_y;
   u1_ruling1[2]   = xyz.v_z;

   u_pat = 1.0;
   v_pat = 0.0; 

status=varkon_pat_biceval (&rul2, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(2)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

   p1_rul2.x_gm = xyz.r_x;
   p1_rul2.y_gm = xyz.r_y;
   p1_rul2.z_gm = xyz.r_z;

   u1_rul2.x_gm = xyz.v_x;
   u1_rul2.y_gm = xyz.v_y;
   u1_rul2.z_gm = xyz.v_z;

   u1_ruling2[0]   = xyz.v_x;
   u1_ruling2[1]   = xyz.v_y;
   u1_ruling2[2]   = xyz.v_z;




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262 p0_rul1 %f %f %f \n",
  p0_rul1.x_gm , p0_rul1.y_gm , p0_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 u0_rul1 %f %f %f \n",
  u0_rul1.x_gm , u0_rul1.y_gm , u0_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 p0_rul2 %f %f %f \n",
  p0_rul2.x_gm , p0_rul2.y_gm , p0_rul2.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 u0_rul2 %f %f %f \n",
  u0_rul2.x_gm , u0_rul2.y_gm , u0_rul2.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 pm_rul1 %f %f %f \n",
  pm_rul1.x_gm , pm_rul1.y_gm , pm_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 um_rul1 %f %f %f \n",
  um_rul1.x_gm , um_rul1.y_gm , um_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 pm_rul2 %f %f %f \n",
  pm_rul2.x_gm , pm_rul2.y_gm , pm_rul2.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 um_rul2 %f %f %f \n",
  um_rul2.x_gm , um_rul2.y_gm , um_rul2.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 p1_rul1 %f %f %f \n",
  p1_rul1.x_gm , p1_rul1.y_gm , p1_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 u1_rul1 %f %f %f \n",
  u1_rul1.x_gm , u1_rul1.y_gm , u1_rul1.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 p1_rul2 %f %f %f \n",
  p1_rul2.x_gm , p1_rul2.y_gm , p1_rul2.z_gm );
fprintf(dbgfil(SURPAC),
"sur262 u1_rul2 %f %f %f \n",
  u1_rul2.x_gm , u1_rul2.y_gm , u1_rul2.z_gm );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Calculate intersect point between line:                          */
/* p0_rul1 + l1*u0_rul1  and  p0_rul2 + l2*u0_rul2                  */
/* Call of varkon_lin_linlind (sur710).                             */
/* Let p_flag= 2 (parallell lines) if calculation fails and         */
/* goto l_u1. Check distance for Debug On (tolerance 5*TOL2).       */
/* Calculate v value (surface rul1) for intersect point.            */
/* Call of varkon_lengv (lengv).                                    */
/* Surface evaluation for calculated v value and u= u_s.            */
/* Call of varkon_pat_biceval (sur221) and varkon_pat_norm (sur240) */
/* Check that evaluated and intersect pt are equal for Debug On.    */
/*                                                                 !*/

status= varkon_lin_linlind
(p0_rul1,u0_rul1,p0_rul2,u0_rul2,&pn_rul1,&pn_rul2,&l1,&l2,&rul_dist);
if (status<0) 
    {
    p_flag = 2;
    }

if ( p_flag == 2 ) goto l_u1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262 rul_dist= %f l1= %f l2= %f for u=u_s \n",
 rul_dist , l1 , l2 );
fprintf(dbgfil(SURPAC),
"sur262 n_rul1  %f %f %f \n",
  pn_rul1.x_gm, pn_rul1.y_gm, pn_rul1.z_gm);
fprintf(dbgfil(SURPAC),
"sur262 n_rul2  %f %f %f \n",
  pn_rul2.x_gm, pn_rul2.y_gm, pn_rul2.z_gm);
}
#endif

#ifdef DEBUG
/* Distance shall be zero for u= 0  */
if (rul_dist > 5.0*TOL2 )
  {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262 rul_dist= %f > 5.0*TOL2= %f for u=u_s \n",
 rul_dist , 5.0*TOL2 );
fprintf(dbgfil(SURPAC),
"sur262 n_rul1  %f %f %f \n",
  pn_rul1.x_gm, pn_rul1.y_gm, pn_rul1.z_gm);
}
  sprintf(errbuf,"rul_dist u=0%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

   points[0].x_gm = pn_rul1.x_gm;
   points[0].y_gm = pn_rul1.y_gm;
   points[0].z_gm = pn_rul1.z_gm;

/* Surface normal for hardpoint in surface rul1 */

   u_pat = 0.0;
   
   status=varkon_lengv (u0_ruling1,&rul_length);
   if ( rul_length > TOL2 )
      {
      v_pat = l1/rul_length;
      }
    else 
      {
      sprintf(errbuf,"l1/rul_length%%varkon_pat_hardptcre2 (sur262)");
      return(varkon_erpush("SU2993",errbuf));
      }


status=varkon_pat_biceval (&rul1, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(3)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
/* Check the calculation of v_pat for the intersect point */
check_v[0] = pn_rul1.x_gm-xyz.r_x;
check_v[1] = pn_rul1.y_gm-xyz.r_y;
check_v[2] = pn_rul1.z_gm-xyz.r_z;
status=varkon_lengv (check_v,&che_length);
if ( che_length > TOL2 )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 che_length= %f > TOL2= %f (1) \n",
    che_length , TOL2 );
    fprintf(dbgfil(SURPAC),
    "sur262 u_pat= %f u_pat= %f (1) \n",
     u_pat , v_pat );
    fprintf(dbgfil(SURPAC),
    "sur262 pn_rul1= %f %f %f (1) \n",
     pn_rul1.x_gm,pn_rul1.y_gm,pn_rul1.z_gm );
    fprintf(dbgfil(SURPAC),
    "sur262    xyz = %f %f %f (1) \n",
         xyz.r_x ,    xyz.r_y ,    xyz.r_z  );
    fprintf(dbgfil(SURPAC),
    "sur262   drdv = %f %f %f (1) \n",
         xyz.v_x ,    xyz.v_y ,    xyz.v_z  );
    fprintf(dbgfil(SURPAC),
    "sur262   d2rdv2 = %f %f %f (1) \n",
         xyz.v2_x ,    xyz.v2_y ,    xyz.v2_z  );
    }
  sprintf(errbuf,"v_pat pn_rul1%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

/* Calculate surface normal and derivatives w.r.t u and v.          */

   status=varkon_pat_norm (icase, &xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

   n_rul1[0]   = xyz.n_x;
   n_rul1[1]   = xyz.n_y;
   n_rul1[2]   = xyz.n_z;

/* Surface normal for hardpoint in surface rul2 */

   u_pat = 0.0;

   status=varkon_lengv (u0_ruling2,&rul_length);
   if ( rul_length > TOL2 )
      {
      v_pat = l2/rul_length;
      }
    else 
      {
      sprintf(errbuf,"l2/rul_length%%varkon_pat_hardptcre2 (sur262)");
      return(varkon_erpush("SU2993",errbuf));
      }


status=varkon_pat_biceval (&rul2, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(3)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
/* Check the calculation of v_pat for the intersect point */
check_v[0] = pn_rul2.x_gm-xyz.r_x;
check_v[1] = pn_rul2.y_gm-xyz.r_y;
check_v[2] = pn_rul2.z_gm-xyz.r_z;
status=varkon_lengv (check_v,&che_length);
if ( che_length > TOL2 )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 che_length= %f > TOL2= %f (2) \n",
    che_length , TOL2 );
    }
  sprintf(errbuf,"v_pat pn_rul2%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

   status=varkon_pat_norm (icase, &xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

   n_rul2[0]   = xyz.n_x;
   n_rul2[1]   = xyz.n_y;
   n_rul2[2]   = xyz.n_z;

   status=varkon_xprod (n_rul1,n_rul2,cross_v);

#ifdef DEBUG
if (status<0) 
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 xprod (1) failed n_rul1 %f %f %f\n",
    n_rul1[0],n_rul1[1],n_rul1[2]);
    fprintf(dbgfil(SURPAC),
    "sur262                  n_rul2 %f %f %f\n",
    n_rul2[0],n_rul2[1],n_rul2[2]);
    }
  sprintf(errbuf,"xprod(n1Xn2)%%varkon_pat_hardptcre2(sur262)");
  sprintf(errbuf,"xprod(n1Xn2)%%varkon_pat_hardptcre2(sur262)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif


   points[1].x_gm = points[0].x_gm + cross_v[0];
   points[1].y_gm = points[0].y_gm + cross_v[1];
   points[1].z_gm = points[0].z_gm + cross_v[2];


l_u1:   /*! Label l_u1: p_flag = 2                                !*/

/*!                                                                 */
/* Calculate intersect point between line:                          */
/* p1_rul1 + l1*u1_rul1  and  p1_rul2 + l2*u1_rul2                  */
/* Call of varkon_lin_linlind (sur710).                             */
/* Let p_flag= 2 (parallell lines) if calculation                   */
/* fails and goto l_um.                                             */
/*                                                                 !*/

status= varkon_lin_linlind
(p1_rul1,u1_rul1,p1_rul2,u1_rul2,&pn_rul1,&pn_rul2,&l1,&l2,&rul_dist);
if (status<0) 
    {
    p_flag=2;
    }

if ( p_flag == 2 ) goto l_um;

#ifdef DEBUG
/* Distance shall be zero for u= 1  */
if (rul_dist > 5.0*TOL2 )
  {
  sprintf(errbuf,"rul_dist u=1%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

   points[3].x_gm = pn_rul1.x_gm;
   points[3].y_gm = pn_rul1.y_gm;
   points[3].z_gm = pn_rul1.z_gm;


/* Surface normal for hardpoint in surface rul1 */

   u_pat = 1.0;

   status=varkon_lengv (u1_ruling1,&rul_length);
   if ( rul_length > TOL2 )
      {
      v_pat = l1/rul_length;
      }
    else 
      {
      sprintf(errbuf,"l1/rul_length%%varkon_pat_hardptcre2 (sur262)");
      return(varkon_erpush("SU2993",errbuf));
      }


status=varkon_pat_biceval (&rul1, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(4)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
/* Check the calculation of v_pat for the intersect point */
check_v[0] = pn_rul1.x_gm-xyz.r_x;
check_v[1] = pn_rul1.y_gm-xyz.r_y;
check_v[2] = pn_rul1.z_gm-xyz.r_z;
status=varkon_lengv (check_v,&che_length);
if ( che_length > TOL2 )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 che_length= %f > TOL2= %f (3) \n",
    che_length , TOL2 );
    }
  sprintf(errbuf,"v_pat p1_rul1%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

   status=varkon_pat_norm (icase, &xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

   n_rul1[0]   = xyz.n_x;
   n_rul1[1]   = xyz.n_y;
   n_rul1[2]   = xyz.n_z;

/* Surface normal for hardpoint in surface rul2 */

   u_pat = 1.0;

   status=varkon_lengv (u1_ruling2,&rul_length);
   if ( rul_length > TOL2 )
      {
      v_pat = l2/rul_length;
      }
    else 
      {
      sprintf(errbuf,"l2/rul_length%%varkon_pat_hardptcre2 (sur262)");
      return(varkon_erpush("SU2993",errbuf));
      }


status=varkon_pat_biceval (&rul2, icase, u_pat,v_pat, &xyz);
if (status<0) 
    {
    sprintf(errbuf,"sur221(5)%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
/* Check the calculation of v_pat for the intersect point */
check_v[0] = pn_rul2.x_gm-xyz.r_x;
check_v[1] = pn_rul2.y_gm-xyz.r_y;
check_v[2] = pn_rul2.z_gm-xyz.r_z;
status=varkon_lengv (check_v,&che_length);
if ( che_length > TOL2 )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 che_length= %f > TOL2= %f (4) \n",
    che_length , TOL2 );
    }
  sprintf(errbuf,"v_pat p1_rul2%%varkon_pat_hardptcre2 (sur262)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif

   status=varkon_pat_norm (icase, &xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

   n_rul2[0]   = xyz.n_x;
   n_rul2[1]   = xyz.n_y;
   n_rul2[2]   = xyz.n_z;

   status=varkon_xprod (n_rul1,n_rul2,cross_v);

#ifdef DEBUG
if (status<0) 
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur262 xprod (2) failed n_rul1 %f %f %f\n",
    n_rul1[0],n_rul1[1],n_rul1[2]);
    fprintf(dbgfil(SURPAC),
    "sur262                  n_rul2 %f %f %f\n",
    n_rul2[0],n_rul2[1],n_rul2[2]);
    }
  sprintf(errbuf,"xprod(n1Xn2)%%varkon_pat_hardptcre2(sur262)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

   points[2].x_gm = points[3].x_gm + cross_v[0];
   points[2].y_gm = points[3].y_gm + cross_v[1];
   points[2].z_gm = points[3].z_gm + cross_v[2];


l_um:   /*! Label l_um: p_flag = 2                                !*/

/*!                                                                 */
/* Calculate intersect point between line:                          */
/* pm_rul1 + l1*um_rul1  and  pm_rul2 + l2*um_rul2                  */
/* Call of varkon_lin_linlind (sur710).                             */
/* Let p_flag= 2 (parallell lines) if calculation                   */
/* fails and goto l_cre.                                            */
/*                                                                 !*/

status= varkon_lin_linlind
(pm_rul1,um_rul1,pm_rul2,um_rul2,&pn_rul1,&pn_rul2,&l1,&l2,&rul_dist);
if (status<0) 
    {
    p_flag = 2;
    }

if ( p_flag == 2 ) goto l_cre;

   ipt = pn_rul1;  /* Actually mid point   !!!!  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur262 n_rul1  %f %f %f \n",
  n_rul1[0], n_rul1[1], n_rul1[2]);
fprintf(dbgfil(SURPAC),
"sur262 n_rul2  %f %f %f \n",
  n_rul2[0], n_rul2[1], n_rul2[2]);
fprintf(dbgfil(SURPAC),
"sur262 cross_v %f %f %f \n",
 cross_v[0],cross_v[1],cross_v[2]);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262 Start     pt  %f %f %f \n",
  points[0].x_gm,points[0].y_gm,points[0].z_gm );
fprintf(dbgfil(SURPAC),
"sur262 Start tan pt  %f %f %f \n",
  points[1].x_gm,points[1].y_gm,points[1].z_gm );
fprintf(dbgfil(SURPAC),
"sur262 End   tan pt  %f %f %f \n",
  points[2].x_gm,points[2].y_gm,points[2].z_gm );
fprintf(dbgfil(SURPAC),
"sur262 End       pt  %f %f %f \n",
  points[3].x_gm,points[3].y_gm,points[3].z_gm );
fprintf(dbgfil(SURPAC),
"sur262 Intermed. pt  %f %f %f \n",
  ipt.x_gm,ipt.y_gm,ipt.z_gm    );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 3. Create the hard point curve segment                           */
/* ______________________________________                           */
/*                                                                  */
/* Create the hardpoint curve defined by points() and ipt.          */
/* Call of GE136 or GE133 if GE136 fails.                        */
/*                                                                 !*/

 status=GE136 (points,&ipt,p_hp);    
 if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262 GE136 (intermed. pt) failed. GE133 creates parabola\n");
}
#endif
     status=GE133 (points,0.5,p_hp);    
     if (status<0) 
        {
        sprintf(errbuf,"GE133%%varkon_pat_hardptcre2 (sur262)");
        return(varkon_erpush("SU2943",errbuf));
        }
     }


/*!                                                                 */
/* Interpolate points u0hp, umhp and u1hp with a parabola.          */
/* Call of varkon_seg_parabola (sur700)                             */
/*                                                                 !*/

l_cre:  /*! Label l_cre: p_flag = 2                               !*/

   if ( p_flag == 2 )
     {
     u0hp.x_gm = p0_rul1.x_gm + 0.5*(p0_rul2.x_gm-p0_rul1.x_gm);        
     u0hp.y_gm = p0_rul1.y_gm + 0.5*(p0_rul2.y_gm-p0_rul1.y_gm);        
     u0hp.z_gm = p0_rul1.z_gm + 0.5*(p0_rul2.z_gm-p0_rul1.z_gm);        
     umhp.x_gm = pm_rul1.x_gm + 0.5*(pm_rul2.x_gm-pm_rul1.x_gm);        
     umhp.y_gm = pm_rul1.y_gm + 0.5*(pm_rul2.y_gm-pm_rul1.y_gm);        
     umhp.z_gm = pm_rul1.z_gm + 0.5*(pm_rul2.z_gm-pm_rul1.z_gm);        
     u1hp.x_gm = p1_rul1.x_gm + 0.5*(p1_rul2.x_gm-p1_rul1.x_gm);        
     u1hp.y_gm = p1_rul1.y_gm + 0.5*(p1_rul2.y_gm-p1_rul1.y_gm);        
     u1hp.z_gm = p1_rul1.z_gm + 0.5*(p1_rul2.z_gm-p1_rul1.z_gm);        

     u_case = 1;

status=varkon_seg_parabola ( u0hp,umhp,u1hp , u_case , p_hp);
if (status<0) 
    {
    sprintf(errbuf,"sur700%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }
  }



/*!                                                                 */
/* 4. Reparameterise                                                */
/* _________________                                                */
/*                                                                  */
/*    Reparameterise the hard point curve.                          */
/*    Call of varkon_cur_reparam (sur790).                          */
/*                                                                 !*/

us_out = u_s;
ue_out = u_e;


us_in  = 0.0;
ue_in  = 1.0;

status=varkon_cur_reparam (p_hp,us_in,ue_in,us_out,ue_out,p_hp);
if (status<0) 
   {
   sprintf(errbuf,"sur790%%varkon_pat_hardptcre2 (sur262)");
   return(varkon_erpush("SU2943",errbuf));
   }

#ifdef  DEBUG
/* Compare 3 pt parabola with "intersect" parabola          */
if ( p_flag == 1 )
  {
  u_case = 1;

status=varkon_seg_parabola (points[0],ipt,points[3],u_case,&hp);
if (status<0) 
    {
    sprintf(errbuf,"sur700%%varkon_pat_hardptcre2 (sur262)");
    return(varkon_erpush("SU2943",errbuf));
    }

    rcode = 3;
    scur.hed_cu.type    = CURTYP;
    scur.ns_cu     = 1;      
    scur.plank_cu  = FALSE;      
    t_l = u_s;
   status=GE107 ((DBAny  *)&scur,p_hp,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_hardptcre2 (sur262)");
        return(varkon_erpush("SU2943",errbuf));
        }

    start_tr[0] = out[3];
    start_tr[1] = out[4];
    start_tr[2] = out[5];

    t_l = 0.0;
   status=GE107 ((DBAny  *)&scur,&hp,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_hardptcre2 (sur262)");
        return(varkon_erpush("SU2943",errbuf));
        }

    start_tp[0] = out[3];
    start_tp[1] = out[4];
    start_tp[2] = out[5];

    angd(start_tr,start_tp,&che_angle);

fprintf(dbgfil(SURPAC),
"sur262 Rational start tangent %f %f %f \n",
        start_tr[0], start_tr[1], start_tr[2]  );
fprintf(dbgfil(SURPAC),
"sur262 Parabola start tangent %f %f %f \n",
        start_tp[0], start_tp[1], start_tp[2]  );
fprintf(dbgfil(SURPAC),
"sur262 Angle between start tangents %f \n",
        che_angle );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!********* Internal ** function ** cubappr ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Approximate a rational cubic curve in the interval u_s to u_e    */
/* with a non-rational cubic segment.                               */

   static short  cubappr (p_in, u_s, u_e, p_out)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_in;         /* Rational cubic segment            (ptr) */
   DBfloat   u_s;          /* Start approximation parameter value     */
   DBfloat   u_e;          /* End   approximation parameter value     */
   DBSeg  *p_out;        /* Non-rational cubic segment        (ptr) */

/* Out:  Coefficients to p_out. Definition interval U=0 to U=1      */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat    out[16];     /* Coordinates and derivatives for crv pt  */
   DBCurve    scur;        /* Curve header for segment (dummy) curve  */
   DBfloat    t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   char   errbuf[80];    /* String for error message fctn erpush    */
   gmint  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur262*cubappr p_in %d u_s %f u_e %f p_out %d \n"
     ,(int)p_in, u_s, u_e, (int)p_out );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initializations                                               */
/*                                                                 !*/

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Definition polygon points for the approximative cubic         */
/*                                                                  */
/* Coordinates and tangents for U= u_s and U= u_e                   */
/* Calls of varkon_GE107 (GE107).                                 */
/*                                                                 !*/

t_l = u_s;       
   status=GE107 ((DBAny  *)&scur,p_in,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_hardptcre2 (cubappr)");
        return(varkon_erpush("SU2943",errbuf));
        }

   points[0].x_gm = out[0];       
   points[0].y_gm = out[1];       
   points[0].z_gm = out[2];       
   points[1].x_gm = points[0].x_gm + out[3];    
   points[1].y_gm = points[0].y_gm + out[4];    
   points[1].z_gm = points[0].z_gm + out[5];    

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur262*cubappr start point      %f %f %f \n",
        out[0], out[1], out[2] );
fprintf(dbgfil(SURPAC),
"sur262_cubappr start tangent    %f %f %f \n",
        out[3], out[4], out[5] );
fprintf(dbgfil(SURPAC),
"sur262_cubappr start tangent pt %f %f %f \n",
        points[1].x_gm, points[1].y_gm, points[1].z_gm    );
  fflush(dbgfil(SURPAC)); 
}
#endif

t_l = u_e;       
   status=GE107 ((DBAny  *)&scur,p_in,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 u_e%% sur762 (cubappr)");
        return(varkon_erpush("SU2943",errbuf));
        }
   points[3].x_gm = out[0];      
   points[3].y_gm = out[1];      
   points[3].z_gm = out[2];      
   points[2].x_gm = points[3].x_gm + out[3];    
   points[2].y_gm = points[3].y_gm + out[4];    
   points[2].z_gm = points[3].z_gm + out[5];    

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur262*cubappr end   point      %f %f %f \n",
        out[0], out[1], out[2] );
fprintf(dbgfil(SURPAC),
"sur262_cubappr end   tangent    %f %f %f \n",
        out[3], out[4], out[5] );
fprintf(dbgfil(SURPAC),
"sur262_cubappr end   tangent pt %f %f %f \n",
        points[2].x_gm, points[2].y_gm, points[2].z_gm    );
  fflush(dbgfil(SURPAC)); 
}
#endif

     status=GE133 (points,0.5,p_out);    
     if (status<0) 
        {
        sprintf(errbuf,"GE133%%varkon_pat_hardptcre2 (cubappr)");
        return(varkon_erpush("SU2943",errbuf));
        }



    return(SUCCED);

} /* End of function                                                */


/*!****************************************************************!*/



