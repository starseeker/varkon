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
/*  Function: varkon_sur_normderiv                 File: sur219.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates surface normal and derivatives          */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-06   Originally written                                 */
/*  1998-05-04   Comments changed                                   */
/*  1999-11-22   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_normderiv  Surface normal and derivatives   */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2962 = sur219 Surface normal is a zero vector in u= , v=       */
/* SU2872 = sur219 Poor surface (derivatives)      in u= , v=       */
/* SU2993 = Severe program error in varkon_sur_normderiv            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_sur_normderiv (

/*!New-Page--------------------------------------------------------!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_xyz )       /* Coordinates and derivatives       (ptr) */
/* Out:                                                             */
/*       Surface normal and its derivatives to p_xyz                */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  n_x,n_y,n_z; /* Surface normal                          */
   DBfloat  n_leng;      /* Surface normal length                   */
   DBfloat  cuv_x;       /* x component of cross vector product cuv */
   DBfloat  cuv_y;       /* y component of cross vector product cuv */
   DBfloat  cuv_z;       /* z component of cross vector product cuv */
   DBfloat  cuu_x;       /* x component of cross vector product cuu */
   DBfloat  cuu_y;       /* y component of cross vector product cuu */
   DBfloat  cuu_z;       /* z component of cross vector product cuu */
   DBfloat  cvu_x;       /* x component of cross vector product cvu */
   DBfloat  cvu_y;       /* y component of cross vector product cvu */
   DBfloat  cvu_z;       /* z component of cross vector product cvu */
   DBfloat  cvv_x;       /* x component of cross vector product cvv */
   DBfloat  cvv_y;       /* y component of cross vector product cvv */
   DBfloat  cvv_z;       /* z component of cross vector product cvv */
   DBfloat  f1,f2;       /* Scalar products                         */
   DBfloat  nu_x,nu_y,nu_z;/* Derivative with respect to U          */
   DBfloat  nv_x,nv_y,nv_z;/* Derivative with respect to U          */

/*-----------------------------------------------------------------!*/

   DBfloat   u_pat,v_pat;/* U,V value                               */
   char      errbuf[80]; /* String for error message fctn erpush    */

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

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

/*!                                                                 */
/* 2. Surface normal                                                */
/*                                                                 !*/

/* The cross product vector (u_x,u_y,u_z) X (v_x,v_y,v_z) is        */
/* perpendicular to the surface.                                    */
/* Calculate the length of the cross vector product and             */
/* let n_x, n_y, n_z be the normalised vector.                      */
/* Error SU2963 if length is zero                                   */

    n_x = p_xyz->u_y*p_xyz->v_z - p_xyz->u_z*p_xyz->v_y;
    n_y = p_xyz->u_z*p_xyz->v_x - p_xyz->u_x*p_xyz->v_z;
    n_z = p_xyz->u_x*p_xyz->v_y - p_xyz->u_y*p_xyz->v_x;

    n_leng = SQRT( n_x*n_x  + n_y*n_y  + n_z*n_z );
    if ( n_leng > 1e-8  ) 
        {
        n_x = n_x/n_leng;
        n_y = n_y/n_leng;
        n_z = n_z/n_leng;
        }
    else
        {
        u_pat = F_UNDEF;
        v_pat = F_UNDEF;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur219 Failure surface normal length= %f \n", 
   n_leng );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur219 u_x %f u_y %f u_z %f\n",p_xyz->u_x,p_xyz->u_y,p_xyz->u_z); 
  fprintf(dbgfil(SURPAC),
  "sur219 v_x %f v_y %f v_z %f\n",p_xyz->v_x,p_xyz->v_y,p_xyz->v_z); 
  }
#endif
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

    p_xyz->n_x= n_x;
    p_xyz->n_y= n_y;
    p_xyz->n_z= n_z;

/*!                                                                 */
/* 3. Normalised surface normal derivatives                         */
/*                                                                  */
/*                                                                 !*/

/* Formulae                                                         */
/* ........                                                         */
/*                                                                  */
/*         dr/du X dr/dv                                            */
/* n(u) = ---------------                                           */
/*        !dr/du X dr/dv!                                           */
/*                                                                  */
/*         d2r/du2 X dr/dv + dr/du X d2r/dudv                       */
/* dn/du = ----------------------------------  -                    */
/*                !dr/du X dr/dv!                                   */
/*                                                                  */
/*         n(u)*((d2r/du2 X dr/dv + dr/du X d2r/dudv )!n(u))        */
/*         -------------------------------------------------        */
/*                           !dr/du X dr/dv!                        */
/*                                                                  */
/*                                                                  */
/* dn/dv = ........                                                 */
/*                                                                  */
/*                                                                 !*/

/*    Cross vector products                                         */
/*                                                                  */

/* Let cuv= dr/du X d2r/dudv= (u_x,u_y,u_z)X(uv_x,uv_y,uv_z)        */
/*                                                                 !*/

    cuv_x = p_xyz->u_y*p_xyz->uv_z - p_xyz->u_z*p_xyz->uv_y;
    cuv_y = p_xyz->u_z*p_xyz->uv_x - p_xyz->u_x*p_xyz->uv_z;
    cuv_z = p_xyz->u_x*p_xyz->uv_y - p_xyz->u_y*p_xyz->uv_x;

/*!                                                                 */
/* Let cuu= d2r/du2 X dr/dv= (u2_x,u2_y,u2_z)X(v_x,v_y,v_z)         */
/*                                                                 !*/

    cuu_x = p_xyz->u2_y*p_xyz->v_z - p_xyz->u2_z*p_xyz->v_y;
    cuu_y = p_xyz->u2_z*p_xyz->v_x - p_xyz->u2_x*p_xyz->v_z;
    cuu_z = p_xyz->u2_x*p_xyz->v_y - p_xyz->u2_y*p_xyz->v_x;

/*!                                                                 */
/* Let cvu= d2r/dudv X dr/dv= (uv_x,uv_y,uv_z)X(v_x,v_y,v_z)        */
/*                                                                 !*/

    cvu_x = p_xyz->uv_y*p_xyz->v_z - p_xyz->uv_z*p_xyz->v_y;
    cvu_y = p_xyz->uv_z*p_xyz->v_x - p_xyz->uv_x*p_xyz->v_z;
    cvu_z = p_xyz->uv_x*p_xyz->v_y - p_xyz->uv_y*p_xyz->v_x;

/*!                                                                 */
/* Let cvv= dr/du X d2r/dv2= (u_x,u_y,u_z)X(v2_x,v2_y,v2_z)         */
/*                                                                 !*/

    cvv_x = p_xyz->u_y*p_xyz->v2_z - p_xyz->u_z*p_xyz->v2_y;
    cvv_y = p_xyz->u_z*p_xyz->v2_x - p_xyz->u_x*p_xyz->v2_z;
    cvv_z = p_xyz->u_x*p_xyz->v2_y - p_xyz->u_y*p_xyz->v2_x;

/*!                                                                 */
/* 2. Output surface normal derivatives                             */
/*                                                                  */

/* Calculate (d2r/du2 + d2r/dudv)/n_leng                            */
/*                                                                 !*/

    nu_x  = ( cuu_x + cuv_x )/n_leng;
    nu_y  = ( cuu_y + cuv_y )/n_leng;
    nu_z  = ( cuu_z + cuv_z )/n_leng;

/*!                                                                 */
/* Calculate (d2r/dudv + d2r/du2)/n_leng                            */
/*                                                                 !*/

    nv_x  = ( cvu_x + cvv_x )/n_leng;
    nv_y  = ( cvu_y + cvv_y )/n_leng;
    nv_z  = ( cvu_z + cvv_z )/n_leng;

/*!                                                                 */
/* Calculate dot product f1= n(u) * (d2r/du2 + d2r/dudv)/n_leng     */
/*                                                                 !*/

    f1    = nu_x*n_x + nu_y*n_y + nu_z*n_z;

/*!                                                                 */
/* Calculate dot product f2= n(u) * (d2r/dudv + d2r/du2)/n_leng     */
/*                                                                 !*/

    f2    = nv_x*n_x + nv_y*n_y + nv_z*n_z;

/*!                                                                 */
/* Let dn/du= (d2r/du2 + d2r/dudv)/n_leng - n(u)*f1                 */
/*                                                                 !*/

    nu_x  = nu_x - n_x*f1;
    nu_y  = nu_y - n_y*f1;
    nu_z  = nu_z - n_z*f1;

/*!                                                                 */
/* Let dn/dv= (d2r/dudv + d2r/du2)/n_leng - n(u)*f2                 */
/*                                                                 !*/

    nv_x  = nv_x - n_x*f2;
    nv_y  = nv_y - n_y*f2;
    nv_z  = nv_z - n_z*f2;

    p_xyz->nu_x= nu_x;
    p_xyz->nu_y= nu_y;
    p_xyz->nu_z= nu_z;

    p_xyz->nv_x= nv_x;
    p_xyz->nv_y= nv_y;
    p_xyz->nv_z= nv_z;


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
