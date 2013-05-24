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
/*  Function: varkon_lin_linlind                   File: sur710.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the minimum distance between            */
/*  two straight lines and the corresponding nearest                */
/*  points on the lines.                                            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1993-11-11   Originally written                                 */
/*  1998-02-28   _cur_linlind -> _lin_linlind                       */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_lin_linlind    Shortest dist. between two lines */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* Shortest distance between two lines                              */
/* -----------------------------------                              */
/*                                                                  */
/* Reference: Faux & Pratt  pages 64-65                             */
/*                                                                  */
/*                                                                  */
/* Find the shortest distance dist between two straight lines       */
/* and the corresponding points p_u and p_v.                        */
/*                                                                  */
/* Notations:                                                       */
/*                                                                  */
/* Line 1: r(s)= p0 + s*u_n where !u_n! = 1                         */
/* Line 2: r(t)= p1 + t*v_n where !v_n! = 1                         */
/*                                                                  */
/* The vector from p_u to p_v must be perpendicular to both         */
/* direction vectors u and v for the lines.                         */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* The scalar products (1) and (2) must be zero:                    */
/*                                                                  */
/* (u_n ! (p1+t*v_n - p0-s*u_n)) = 0                (1)             */
/* (v_n ! (p1+t*v_n - p0-s*u_n)) = 0                (2)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* (u_n ! (p1-p0))+ (u_n ! (t*v_n-s*u_n)) = 0       (1)             */
/* (v_n ! (p1-p0))+ (v_n ! (t*v_n-s*u_n)) = 0       (2)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* (u_n ! (p1-p0))+ t*(u_n!v_n)-s*(u_n!u_n) = 0     (1)             */
/* (v_n ! (p1-p0))+ t*(v_n!v_n)-s*(v_n!u_n) = 0     (2)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* (u_n ! (p1-p0))+ t*(u_n!v_n-s)         = 0       (1)             */
/* (v_n ! (p1-p0))+ t        -s*(v_n!u_n) = 0       (2)    (A)      */
/*                                                                  */
/*                 <==> (multiply (2) with -(u_n!v_n) )             */
/*                                                                  */
/* (u_n ! (p1-p0))+ t*(u_n!v_n)-s                     = 0   (1)     */
/*-(u_n!v_n)*(v_n!(p1-p0))-(u_n!v_n)*t+s*(v_n!u_n)**2 = 0   (2)     */
/*                                                                  */
/*                 <==> (let (2) = (1) + (2)        )               */
/*                                                                  */
/* (u_n ! (p1-p0))+ t*(u_n!v_n)-s                     = 0      (1)  */
/* (u_n!(p1-p0))-(u_n!v_n)*(v_n!(p1-p0))+s*((v_n!u_n)**2-1)=0  (2)  */
/*                                                                  */
/*                 <==>  ( s from (2) and t from (A) )              */
/*                                                                  */
/*          (u_n!v_n)*(v_n!(p1-p0)) - (u_n!(p1-p0))                 */
/*      s = ---------------------------------------                 */
/*                  (v_n!u_n)**2-1                                  */
/*                                                                  */
/*      t =  s*(v_n!u_n) - (v_n!(p1-p0))                            */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_normv           * Normalisation of a vector               */
/* varkon_lengv           * Length of a vector                      */
/* varkon_scalar          * Vector dot product.                     */
/* varkon_xprod           * Cross vector product.                   */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2511 = Parallel lines in varkon_lin_linlind                    */
/* SU2943 = Called function xxxxxx failed in varkon_lin_linlind     */
/* SU2993 = Program error ( ) in varkon_lin_linlind         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_lin_linlind (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector  p0,          /* Start point      for line 1             */
  DBVector  u,           /* Direction vector for line 1             */
  DBVector  p1,          /* Start point      for line 2             */
  DBVector  v,           /* Direction vector for line 2             */
  DBVector *p_pu,        /* Nearest point    on  line 1 (pointer)   */
  DBVector *p_pv,        /* Nearest point    on  line 2 (pointer)   */
  DBfloat  *p_s,         /* Parameter for line 1 (unit vector)      */
  DBfloat  *p_t,         /* Parameter for line 1 (unit vector)      */
  DBfloat  *p_dist )     /* Distance                    (pointer)   */

/* Out:                                                             */
/*        Nearest points and distance                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  u_n[3];       /* Normalised u vector for line 1          */
  DBfloat  v_n[3];       /* Normalised v vector for line 2          */
  DBfloat  s_l;          /* Parameter value for line 1              */
  DBfloat  t_l;          /* Parameter value for line 2              */
  DBfloat  p1_p0[3];     /* Vector p1-p0                            */
  DBfloat  dot_up1p0;    /* Dot product u_n*(p1-p0)                 */
  DBfloat  dot_vp1p0;    /* Dot product v_n*(p1-p0)                 */
  DBfloat  dot_uv;       /* Dot product u_n*v_n                     */
  DBfloat  cross_vu[3];  /* Vector cross product v_n X u_n          */
  DBfloat  cross_vu_l;   /* Vector cross product v_n X u_n length.  */
  DBfloat  dot_p1p0_vu;  /* Dot product (p1-p0)*cross_vu            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  length;       /* Length of a vector (p1-p0)              */
  short    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */
#ifdef DEBUG
  DBfloat  check_v[3];   /* Check vector p_pv-p_pu                  */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur710 Enter *** varkon_lin_linlind  **Lin/lin closest pt ****\n");
}
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur710 p0   %f %f %f\n",p0.x_gm, p0.y_gm, p0.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur710 u    %f %f %f\n",u.x_gm, u.y_gm, u.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur710 p1   %f %f %f\n",p1.x_gm, p1.y_gm, p1.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur710 v    %f %f %f\n",v.x_gm, v.y_gm, v.z_gm );
}
#endif
 

/*!                                                                 */
/* 1. Initializations and check of input data                       */
/* __________________________________________                       */

/* Normalise u and v (calculate u_n and v_n). Calls of normv.       */
/* (Lengths of u and v are checked in normv).                       */
/*                                                                 !*/

   u_n[0] = u.x_gm;
   u_n[1] = u.y_gm;
   u_n[2] = u.z_gm;
   v_n[0] = v.x_gm;
   v_n[1] = v.y_gm;
   v_n[2] = v.z_gm;

   status=varkon_normv(u_n);
   if (status<0) 
     {
     sprintf(errbuf,"normv(u_n)%%varkon_lin_linlind(sur710)");
     return(varkon_erpush("SU2943",errbuf));
     }
   status=varkon_normv(v_n);
   if (status<0) 
     {
     sprintf(errbuf,"normv(v_n)%%varkon_lin_linlind(sur710)");
     return(varkon_erpush("SU2943",errbuf));
     }
 
/*!                                                                 */
/* 2. Vectors and dot products                                      */
/* ___________________________                                      */

/* Let vector p1_p0= p1 - p0.                                       */
/* Calculate length of p1_p0. Call of lengv.                        */
/* Let nearest points be p1 and p0 and return                       */
/* if the length is zero ( < TOL2 ).                                */
/*                                                                 !*/

   p1_p0[0] = p1.x_gm-p0.x_gm;
   p1_p0[1] = p1.y_gm-p0.y_gm;
   p1_p0[2] = p1.z_gm-p0.z_gm;


   status=varkon_lengv(p1_p0,&length);

   if ( length < TOL2  )  
   {
   *p_pu   = p0;
   *p_pv   = p1;
   *p_dist = length;
   goto dp1p0;
   }

/*!                                                                 */
/* Let dot_up1p0  = u_n*(p1-p0).     Call of scalar.                */
/* Let dot_vp1p0  = v_n*(p1-p0).     Call of scalar.                */
/* Let dot_uv     = u_n*v_n.         Call of scalar.                */
/* Let cross_vu   = v_nXu_n.         Call of xprod.                 */
/* Let cross_vu_l = !v_nXu_n!.       Call of length.                */
/* Return if cross_vu_l is less than TOL1 (parallell lines)         */
/* Let dot_p1p0_vu= (p1-p0)*cross_vu Call of scalar.                */
/*                                                                 !*/

   status=varkon_scalar(u_n,p1_p0,&dot_up1p0);

#ifdef DEBUG
/* Negative status for zero vectors. This is already is checked     */
if (status<0) 
  {
  sprintf(errbuf,"scalar(u_n)%%varkon_lin_linlind(sur710)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

   status=varkon_scalar(v_n,p1_p0,&dot_vp1p0);

#ifdef DEBUG
/* Negative status for zero vectors. This is already is checked     */
if (status<0) 
  {
  sprintf(errbuf,"scalar(v_n)%%varkon_lin_linlind(sur710)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

   status=varkon_scalar(u_n,v_n,&dot_uv);

#ifdef DEBUG
/* Negative status for zero vectors. This is already is checked     */
if (status<0) 
  {
  sprintf(errbuf,"scalar(u_n*v_n)%%varkon_lin_linlind(sur710)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

   status=varkon_xprod(v_n,u_n,cross_vu);

#ifdef DEBUG
/* Negative status for zero vectors. This is already is checked     */
if (status<0) 
  {
  sprintf(errbuf,"xprod(u_nXv_n)%%varkon_lin_linlind(sur710)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

   status=varkon_lengv(cross_vu,&cross_vu_l);

   if ( cross_vu_l < TOL1  )  
   {
   (*p_pu).x_gm  = 1.23456789;
   (*p_pu).y_gm  = 1.23456789;
   (*p_pu).z_gm  = 1.23456789;
   (*p_pv).x_gm  = 1.23456789;
   (*p_pv).y_gm  = 1.23456789;
   (*p_pv).z_gm  = 1.23456789;
   /* Add calculation of distance between the parallell lines !! */
   *p_dist       =   -1.23456789;
   sprintf(errbuf," %%varkon_lin_linlind(sur710)");
   return(varkon_erpush("SU2511",errbuf));
   }


   status=varkon_scalar(p1_p0,cross_vu,&dot_p1p0_vu);

#ifdef DEBUG
/* Negative status for zero vectors. This is already is checked     */
if (status<0) 
  {
  sprintf(errbuf,"scalar(cross_vu)%%varkon_lin_linlind(sur710)");
  return(varkon_erpush("SU2943",errbuf));
  }
#endif

/*!                                                                 */
/* 3. Shortest distance and closest points                          */
/* _______________________________________                          */

/* Let output distance *p_dist= dot_p1p0_vu/cross_vu_l.             */
/* Let s_l= (dot_uv*dot_vp1p0 - dot_up1p0) / (dot_uv*dot_uv-1)      */
/* Let t_l=  s_l*dot_uv - dot_vp1p0                                 */
/* Let output point p_pu = p0 + s_l*u_n                             */
/* Let output point p_pv = p1 + t_l*v_n                             */
/*                                                                 !*/



   *p_dist= fabs(dot_p1p0_vu)/cross_vu_l; 

   if ( fabs(dot_uv*dot_uv-1.0) > TOL1 )
     {
     s_l= (dot_uv*dot_vp1p0 - dot_up1p0) / (dot_uv*dot_uv-1.0); 
     }
   else
     {
     sprintf(errbuf,"s_l calc.%%varkon_lin_linlind(sur710)");
     return(varkon_erpush("SU2993",errbuf));
     }

   t_l=  s_l*dot_uv - dot_vp1p0;  


   (*p_pu).x_gm  = p0.x_gm + s_l*u_n[0];
   (*p_pu).y_gm  = p0.y_gm + s_l*u_n[1];
   (*p_pu).z_gm  = p0.z_gm + s_l*u_n[2];
   (*p_pv).x_gm  = p1.x_gm + t_l*v_n[0];
   (*p_pv).y_gm  = p1.y_gm + t_l*v_n[1];
   (*p_pv).z_gm  = p1.z_gm + t_l*v_n[2];

   *p_s = s_l;
   *p_t = t_l;

/*!                                                                 */
/* For Debug On:                                                    */
/* Calculate distance between p_pu and p_pv and                     */
/* compare (TOL2) with p_dist. Call of lengv.                       */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur710 p_pu %f %f %f\n",(*p_pu).x_gm, (*p_pu).y_gm, (*p_pu).z_gm );
fprintf(dbgfil(SURPAC),
"sur710 p_pv %f %f %f\n",(*p_pv).x_gm, (*p_pv).y_gm, (*p_pv).z_gm );
fprintf(dbgfil(SURPAC),
"sur710 s_l %f t_l %f *p_dist %f\n",s_l,t_l,*p_dist );
}
  check_v[0] = (*p_pv).x_gm - (*p_pu).x_gm;
  check_v[1] = (*p_pv).y_gm - (*p_pu).y_gm;
  check_v[2] = (*p_pv).z_gm - (*p_pu).z_gm;
  status=varkon_lengv(check_v,&length);
  if ( fabs(*p_dist-length) > TOL2  )  
   {
   if ( dbglev(SURPAC) == 1 )
     {
     fprintf(dbgfil(SURPAC),
     "sur710 p0   %f %f %f\n",p0.x_gm, p0.y_gm, p0.z_gm );
     fprintf(dbgfil(SURPAC),
     "sur710 u    %f %f %f\n",u.x_gm, u.y_gm, u.z_gm );
     fprintf(dbgfil(SURPAC),
     "sur710 p1   %f %f %f\n",p1.x_gm, p1.y_gm, p1.z_gm );
     fprintf(dbgfil(SURPAC),
     "sur710 v    %f %f %f\n",v.x_gm, v.y_gm, v.z_gm );
     fprintf(dbgfil(SURPAC),
     "sur710 length %f - *p_dist %f = %f\n",
      length,*p_dist, length - *p_dist );
     }
   sprintf(errbuf,"check_v%%varkon_lin_linlind(sur710)");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif



dp1p0:   /* Label: Length of p1_p0 is zero                    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur710 Exit*varkon_lin_linlind Distance= %f\n",*p_dist);
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
