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
/*  Function: varkon_pat_rateval                   File: sur224.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a given a rational patch.    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-06   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_rateval    Rational patch evaluation fctn   */
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
/*           varkon_pat_norm();        * Normal with derivatives    */
/*           varkon_erpush();          * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_pat_rateval     */
/* SU2962 = sur224 Surface normal is a zero vector in u= , v=       */
/* SU2993 = Severe program error in varkon_pat_rateval sur224.      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_pat_rateval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATR  *p_patr,      /* Current rational patch            (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat  u_pat,       /* Patch (local) U parameter value         */
   DBfloat  v_pat,       /* Patch (local) V parameter value         */
   EVALS   *p_xyz )      /* Coordinates and derivatives       (ptr) */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat hr_x,hr_y,hr_z;   /* Coordinates          R(u)           */
   DBfloat hr_w;             /*       (homogenous)                  */
   DBfloat hu_x,hu_y,hu_z;   /* First  derivative   dR/du           */
   DBfloat hu_w;             /*       (homogenous)                  */
   DBfloat hv_x,hv_y,hv_z;   /* First  derivative   dR/dv           */
   DBfloat hv_w;             /*       (homogenous)                  */
   DBfloat hu2_x,hu2_y,hu2_z;/* Second derivative  d2R/du2          */
   DBfloat hu2_w;            /*       (homogenous)                  */
   DBfloat hv2_x,hv2_y,hv2_z;/* Second derivative  d2R/dv2          */
   DBfloat hv2_w;            /*       (homogenous)                  */
   DBfloat r_x,r_y,r_z;      /* Coordinates          r(u)           */
   DBfloat u_x,u_y,u_z;      /* Tangent             dr/du           */
   DBfloat v_x,v_y,v_z;      /* Tangent             dr/dv           */
   DBfloat u2_x,u2_y,u2_z;   /* Second derivative  d2r/du2          */
   DBfloat v2_x,v2_y,v2_z;   /* Second derivative  d2r/dv2          */
   DBfloat uv_x,uv_y,uv_z;   /* Twist vector       d2r/dudv         */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat f0,f1,f2,f3;    /* For the matrix multiplication         */
   DBfloat u,v;            /* Parameters identical to u_pat,v_pat   */
   char    errbuf[80];     /* String for error message fctn erpush  */
   DBint   status;         /* Error code from a called function     */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  No checks implemented .......                                   */
/*                                                                 !*/

/*!                                                                 */
/* 2. Calculate homogenous coordinates and derivatives              */
/* _____________________________________________________            */
/*                                                                 !*/

u  = u_pat;
v  = v_pat;

/*!                                                                 */
/* Homogenous coordinates hr_x, hr_y, hr_z and hr_w                 */
/*                                                                 !*/

f0= p_patr->a00x + u*(p_patr->a10x + u*(p_patr->a20x + u*p_patr->a30x));
f1= p_patr->a01x + u*(p_patr->a11x + u*(p_patr->a21x + u*p_patr->a31x));
f2= p_patr->a02x + u*(p_patr->a12x + u*(p_patr->a22x + u*p_patr->a32x));
f3= p_patr->a03x + u*(p_patr->a13x + u*(p_patr->a23x + u*p_patr->a33x));
hr_x=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0= p_patr->a00y + u*(p_patr->a10y + u*(p_patr->a20y + u*p_patr->a30y));
f1= p_patr->a01y + u*(p_patr->a11y + u*(p_patr->a21y + u*p_patr->a31y));
f2= p_patr->a02y + u*(p_patr->a12y + u*(p_patr->a22y + u*p_patr->a32y));
f3= p_patr->a03y + u*(p_patr->a13y + u*(p_patr->a23y + u*p_patr->a33y));
hr_y=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0= p_patr->a00z + u*(p_patr->a10z + u*(p_patr->a20z + u*p_patr->a30z));
f1= p_patr->a01z + u*(p_patr->a11z + u*(p_patr->a21z + u*p_patr->a31z));
f2= p_patr->a02z + u*(p_patr->a12z + u*(p_patr->a22z + u*p_patr->a32z));
f3= p_patr->a03z + u*(p_patr->a13z + u*(p_patr->a23z + u*p_patr->a33z));
hr_z=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0= p_patr->a00  + u*(p_patr->a10  + u*(p_patr->a20  + u*p_patr->a30 ));
f1= p_patr->a01  + u*(p_patr->a11  + u*(p_patr->a21  + u*p_patr->a31 ));
f2= p_patr->a02  + u*(p_patr->a12  + u*(p_patr->a22  + u*p_patr->a32 ));
f3= p_patr->a03  + u*(p_patr->a13  + u*(p_patr->a23  + u*p_patr->a33 ));
hr_w=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

/*!                                                                 */
/* Homogenous first derivative hu_x, hu_y, hu_z, hu_w               */
/*                                                                 !*/

f0=        p_patr->a10x + u*(2.0*p_patr->a20x + u*3.0*p_patr->a30x) ;
f1=        p_patr->a11x + u*(2.0*p_patr->a21x + u*3.0*p_patr->a31x) ;
f2=        p_patr->a12x + u*(2.0*p_patr->a22x + u*3.0*p_patr->a32x) ;
f3=        p_patr->a13x + u*(2.0*p_patr->a23x + u*3.0*p_patr->a33x) ;
hu_x=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=        p_patr->a10y + u*(2.0*p_patr->a20y + u*3.0*p_patr->a30y) ;
f1=        p_patr->a11y + u*(2.0*p_patr->a21y + u*3.0*p_patr->a31y) ;
f2=        p_patr->a12y + u*(2.0*p_patr->a22y + u*3.0*p_patr->a32y) ;
f3=        p_patr->a13y + u*(2.0*p_patr->a23y + u*3.0*p_patr->a33y) ;
hu_y=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=        p_patr->a10z + u*(2.0*p_patr->a20z + u*3.0*p_patr->a30z) ;
f1=        p_patr->a11z + u*(2.0*p_patr->a21z + u*3.0*p_patr->a31z) ;
f2=        p_patr->a12z + u*(2.0*p_patr->a22z + u*3.0*p_patr->a32z) ;
f3=        p_patr->a13z + u*(2.0*p_patr->a23z + u*3.0*p_patr->a33z) ;
hu_z=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=        p_patr->a10  + u*(2.0*p_patr->a20  + u*3.0*p_patr->a30 ) ;
f1=        p_patr->a11  + u*(2.0*p_patr->a21  + u*3.0*p_patr->a31 ) ;
f2=        p_patr->a12  + u*(2.0*p_patr->a22  + u*3.0*p_patr->a32 ) ;
f3=        p_patr->a13  + u*(2.0*p_patr->a23  + u*3.0*p_patr->a33 ) ;
hu_w=     f0      + v*(     f1     + v*(     f2     + v*     f3    ));

/*!                                                                 */
/* Homogenous first derivative hv_x, hv_y, hv_z, hv_w               */
/*                                                                 !*/

f0= p_patr->a00x + u*(p_patr->a10x + u*(p_patr->a20x + u*p_patr->a30x));
f1= p_patr->a01x + u*(p_patr->a11x + u*(p_patr->a21x + u*p_patr->a31x));
f2= p_patr->a02x + u*(p_patr->a12x + u*(p_patr->a22x + u*p_patr->a32x));
f3= p_patr->a03x + u*(p_patr->a13x + u*(p_patr->a23x + u*p_patr->a33x));
hv_x=                       f1     + v*( 2.0*f2     + v* 3.0*f3    ) ;

f0= p_patr->a00y + u*(p_patr->a10y + u*(p_patr->a20y + u*p_patr->a30y));
f1= p_patr->a01y + u*(p_patr->a11y + u*(p_patr->a21y + u*p_patr->a31y));
f2= p_patr->a02y + u*(p_patr->a12y + u*(p_patr->a22y + u*p_patr->a32y));
f3= p_patr->a03y + u*(p_patr->a13y + u*(p_patr->a23y + u*p_patr->a33y));
hv_y=                       f1     + v*( 2.0*f2     + v* 3.0*f3    ) ;

f0= p_patr->a00z + u*(p_patr->a10z + u*(p_patr->a20z + u*p_patr->a30z));
f1= p_patr->a01z + u*(p_patr->a11z + u*(p_patr->a21z + u*p_patr->a31z));
f2= p_patr->a02z + u*(p_patr->a12z + u*(p_patr->a22z + u*p_patr->a32z));
f3= p_patr->a03z + u*(p_patr->a13z + u*(p_patr->a23z + u*p_patr->a33z));
hv_z=                       f1     + v*( 2.0*f2     + v* 3.0*f3    ) ;

f0= p_patr->a00  + u*(p_patr->a10  + u*(p_patr->a20  + u*p_patr->a30 ));
f1= p_patr->a01  + u*(p_patr->a11  + u*(p_patr->a21  + u*p_patr->a31 ));
f2= p_patr->a02  + u*(p_patr->a12  + u*(p_patr->a22  + u*p_patr->a32 ));
f3= p_patr->a03  + u*(p_patr->a13  + u*(p_patr->a23  + u*p_patr->a33 ));
hv_w=                       f1     + v*( 2.0*f2     + v* 3.0*f3    ) ;

/*!                                                                 */
/* Homogenous second derivative hu2_x, hu2_y, hu2_z, hu2_w          */
/*                                                                 !*/

f0=                         2.0*p_patr->a20x + u*6.0*p_patr->a30x  ;
f1=                         2.0*p_patr->a21x + u*6.0*p_patr->a31x  ;
f2=                         2.0*p_patr->a22x + u*6.0*p_patr->a32x  ;
f3=                         2.0*p_patr->a23x + u*6.0*p_patr->a33x  ;
hu2_x=    f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=                         2.0*p_patr->a20y + u*6.0*p_patr->a30y  ;
f1=                         2.0*p_patr->a21y + u*6.0*p_patr->a31y  ;
f2=                         2.0*p_patr->a22y + u*6.0*p_patr->a32y  ;
f3=                         2.0*p_patr->a23y + u*6.0*p_patr->a33y  ;
hu2_y=    f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=                         2.0*p_patr->a20z + u*6.0*p_patr->a30z  ;
f1=                         2.0*p_patr->a21z + u*6.0*p_patr->a31z  ;
f2=                         2.0*p_patr->a22z + u*6.0*p_patr->a32z  ;
f3=                         2.0*p_patr->a23z + u*6.0*p_patr->a33z  ;
hu2_z=    f0      + v*(     f1     + v*(     f2     + v*     f3    ));

f0=                         2.0*p_patr->a20  + u*6.0*p_patr->a30   ;
f1=                         2.0*p_patr->a21  + u*6.0*p_patr->a31   ;
f2=                         2.0*p_patr->a22  + u*6.0*p_patr->a32   ;
f3=                         2.0*p_patr->a23  + u*6.0*p_patr->a33   ;
hu2_w=    f0      + v*(     f1     + v*(     f2     + v*     f3    ));

/*!                                                                 */
/* Homogenous second derivative hv2_x, hv2_y, hv2_z, hv2_w          */
/*                                                                 !*/

f0= p_patr->a00x + u*(p_patr->a10x + u*(p_patr->a20x + u*p_patr->a30x));
f1= p_patr->a01x + u*(p_patr->a11x + u*(p_patr->a21x + u*p_patr->a31x));
f2= p_patr->a02x + u*(p_patr->a12x + u*(p_patr->a22x + u*p_patr->a32x));
f3= p_patr->a03x + u*(p_patr->a13x + u*(p_patr->a23x + u*p_patr->a33x));
hv2_x=                                   2.0*f2     + v* 6.0*f3      ;

f0= p_patr->a00y + u*(p_patr->a10y + u*(p_patr->a20y + u*p_patr->a30y));
f1= p_patr->a01y + u*(p_patr->a11y + u*(p_patr->a21y + u*p_patr->a31y));
f2= p_patr->a02y + u*(p_patr->a12y + u*(p_patr->a22y + u*p_patr->a32y));
f3= p_patr->a03y + u*(p_patr->a13y + u*(p_patr->a23y + u*p_patr->a33y));
hv2_y=                                   2.0*f2     + v* 6.0*f3      ;

f0= p_patr->a00z + u*(p_patr->a10z + u*(p_patr->a20z + u*p_patr->a30z));
f1= p_patr->a01z + u*(p_patr->a11z + u*(p_patr->a21z + u*p_patr->a31z));
f2= p_patr->a02z + u*(p_patr->a12z + u*(p_patr->a22z + u*p_patr->a32z));
f3= p_patr->a03z + u*(p_patr->a13z + u*(p_patr->a23z + u*p_patr->a33z));
hv2_z=                                   2.0*f2     + v* 6.0*f3      ;

f0= p_patr->a00  + u*(p_patr->a10  + u*(p_patr->a20  + u*p_patr->a30 ));
f1= p_patr->a01  + u*(p_patr->a11  + u*(p_patr->a21  + u*p_patr->a31 ));
f2= p_patr->a02  + u*(p_patr->a12  + u*(p_patr->a22  + u*p_patr->a32 ));
f3= p_patr->a03  + u*(p_patr->a13  + u*(p_patr->a23  + u*p_patr->a33 ));
hv2_w=                                   2.0*f2     + v* 6.0*f3      ;

/*!                                                                 */
/* 3. Calculate cartesian  coordinates and derivatives              */
/* _____________________________________________________            */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that the denominator is greater than zero   for DEBUG on   */
/*                                                                 !*/

#ifdef DEBUG
if ( hr_w < 0.0001 )
  {
  sprintf(errbuf,"Denominator < 0%%varkon_pat_rateval (sur224)");
  return(varkon_erpush("SU2993",errbuf));
  }
#endif



/*!                                                                 */
/* Cartesian  coordinates  r_x,  r_y,  r_z                          */
/*                                                                 !*/

  r_x = hr_x/hr_w;
  r_y = hr_y/hr_w;
  r_z = hr_z/hr_w;

/*!                                                                 */
/* Cartesian  first derivative  u_x,  u_y,  u_z                     */
/*                                                                 !*/

  u_x = (hr_w*hu_x - hu_w*hr_x)/hr_w/hr_w;
  u_y = (hr_w*hu_y - hu_w*hr_y)/hr_w/hr_w;
  u_z = (hr_w*hu_z - hu_w*hr_z)/hr_w/hr_w;

/*!                                                                 */
/* Cartesian  first derivative  v_x,  v_y,  v_z                     */
/*                                                                 !*/

  v_x = (hr_w*hv_x - hv_w*hr_x)/hr_w/hr_w;
  v_y = (hr_w*hv_y - hv_w*hr_y)/hr_w/hr_w;
  v_z = (hr_w*hv_z - hv_w*hr_z)/hr_w/hr_w;

/*!                                                                 */
/* Cartesian second derivative  u2_x,  u2_y,  u2_z                  */
/*                                                                 !*/

  u2_x = (hr_w*hu2_x - hu_w*hu_x)/hr_w/hr_w  -
    ((hu2_w*hr_x + hu_w*hu_x)*hr_w*hr_w - 2.0*hr_w*hr_x*hu_w*hu_w)
           /hr_w/hr_w/hr_w/hr_w;
  u2_y = (hr_w*hu2_y - hu_w*hu_y)/hr_w/hr_w  -
    ((hu2_w*hr_y + hu_w*hu_y)*hr_w*hr_w - 2.0*hr_w*hr_y*hu_w*hu_w)
           /hr_w/hr_w/hr_w/hr_w;
  u2_z = (hr_w*hu2_z - hu_w*hu_z)/hr_w/hr_w  -
    ((hu2_w*hr_z + hu_w*hu_z)*hr_w*hr_w - 2.0*hr_w*hr_z*hu_w*hu_w)
           /hr_w/hr_w/hr_w/hr_w;



/*!                                                                 */
/* Cartesian second derivative  v2_x,  v2_y,  v2_z                  */
/*                                                                 !*/

  v2_x = (hr_w*hv2_x - hv_w*hv_x)/hr_w/hr_w  -
    ((hv2_w*hr_x + hv_w*hv_x)*hr_w*hr_w - 2.0*hr_w*hr_x*hv_w*hv_w)
           /hr_w/hr_w/hr_w/hr_w;
  v2_y = (hr_w*hv2_y - hv_w*hv_y)/hr_w/hr_w  -
    ((hv2_w*hr_y + hv_w*hv_y)*hr_w*hr_w - 2.0*hr_w*hr_y*hv_w*hv_w)
           /hr_w/hr_w/hr_w/hr_w;
  v2_z = (hr_w*hv2_z - hv_w*hv_z)/hr_w/hr_w  -
    ((hv2_w*hr_z + hv_w*hv_z)*hr_w*hr_w - 2.0*hr_w*hr_z*hv_w*hv_w)
           /hr_w/hr_w/hr_w/hr_w;

/*! !! Derivative d2r/dudv= uv_x,uv_y,uv_z !!!! Error !!!!!        !*/

  uv_x= F_UNDEF;     
  uv_y= F_UNDEF;     
  uv_z= F_UNDEF;     

/*!                                                                 */
/* 3. Coordinates and derivatives to output variable p_xy           */
/* ______________________________________________________           */
/*                                                                  */
/* Coordinates, derivatives to p_xyz                                */
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

/*!                                                                 */
/* 4. Surface normal and surface normal derivatives                 */
/* ________________________________________________                 */
/*                                                                  */
/* Calculate surface normal and derivatives w.r.t u and v.          */
/* Error SU2963 for a zero length surface normal.                   */
/* Call of varkon_pat_norm (sur240).                                */
/*                                                                 !*/


   status=varkon_pat_norm (icase,p_xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }



#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur224 hr_w %f hu_w %f hu2_w %f \n",
   hr_w ,hu_w, hu2_w               );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur224 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur224 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur224 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur224 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur224 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur224 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  fprintf(dbgfil(SURPAC),
  "sur224 Exit *** varkon_pat_rateval ******* \n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
