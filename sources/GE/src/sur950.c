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
/*  Function: varkon_sur_cureval                   File: sur950.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the coordinates and derivatives         */
/*  for a point on a UV (surface) curve.                            */
/*                                                                  */
/*  Input is coordinates and derivatives for the UV curve and       */
/*  coordinates and derivatives for the surface.                    */
/*                                                                  */
/*  This routine will be replaced by varkon_sur_uvsegeval (sur214). */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1996-05-28   errbuf deleted                                     */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_cureval    Coord. and derivatives for UV pt */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*  Reference: Faux & Pratt p 110-111 and p 274                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_normkappa    * Normal curvature                       */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_cureval  (sur950).   */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_cureval (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  uv_in[],     /* Coordinates and derivatives for UV crv  */
                         /* (Dimension 16 for rcode= 3)             */
   EVALS   *p_xyz,       /* Coordin./derivat. for surface pt  (ptr) */
   short    rcode,       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
                         /*        and curvature           out[ 15 ]*/

/* Out:                                                             */
   DBfloat  out[] )      /* Array with coordinates and derivatives  */
                         /* (Dimension 16 for rcode= 3)             */

/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  dsdt;        /* Length of tangent (dr/dt)               */
   DBfloat  tan[3];      /* Normalized tangent= (dr/dt)/dsdt        */
   DBfloat  nkappa;      /* Normal   curvature                      */
   DBfloat  geodesic;    /* Geodesic curvature                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950 Enter*varkon_sur_cureval******** rcode %d\n", rcode);
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950 uv_in(3-4) %f %f  (6-7) %f %f\n",
          uv_in[3], uv_in[4] ,uv_in[6],uv_in[7]);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950 p_xyz= %f %f %f \n",
          p_xyz->r_x , p_xyz->r_y  ,p_xyz->r_z );
  }
#endif

/*!                                                                 */
/* Coordinates from p_xyz. Goto nomore if rcode= 0.                   */
/*                                                                 !*/

   out[0]= p_xyz->r_x;
   out[1]= p_xyz->r_y;
   out[2]= p_xyz->r_z;

   if (rcode == 0 ) goto nomore;

/*!                                                                 */
/* 2. Calculate derivatives for the surface curve                   */
/* _____________________________________________                    */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* First  derivatives to output array out[3-5].                     */
/* Goto nomore if rcode= 1.                                         */
/*                                                                 !*/

   out[3]= p_xyz->u_x*uv_in[3] + p_xyz->v_x*uv_in[4];
   out[4]= p_xyz->u_y*uv_in[3] + p_xyz->v_y*uv_in[4];
   out[5]= p_xyz->u_z*uv_in[3] + p_xyz->v_z*uv_in[4];

   if (rcode == 1 ) goto nomore;

/*!                                                                 */
/* Second derivatives to output array out[6-8]                      */
/* Goto nomore if rcode= 2.                                         */
/*                                                                 !*/

   out[6]=     p_xyz->u2_x*uv_in[3]*uv_in[3] 
         + 2.0*p_xyz->uv_x*uv_in[3]*uv_in[4] 
         +     p_xyz->v2_x*uv_in[4]*uv_in[4]   
         +     p_xyz->u_x *uv_in[6]
         +     p_xyz->v_x *uv_in[7];
   out[7]=     p_xyz->u2_y*uv_in[3]*uv_in[3] 
         + 2.0*p_xyz->uv_y*uv_in[3]*uv_in[4] 
         +     p_xyz->v2_y*uv_in[4]*uv_in[4]   
         +     p_xyz->u_y *uv_in[6]
         +     p_xyz->v_y *uv_in[7];
   out[8]=     p_xyz->u2_z*uv_in[3]*uv_in[3] 
         + 2.0*p_xyz->uv_z*uv_in[3]*uv_in[4] 
         +     p_xyz->v2_z*uv_in[4]*uv_in[4]   
         +     p_xyz->u_z *uv_in[6]
         +     p_xyz->v_z *uv_in[7];

   if (rcode == 2 ) goto nomore;

/*!                                                                 */
/* 3. Calculate kappa and binormal                                  */
/* _______________________________                                  */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* The length dsdt of the tangent (dr/dt)                           */
/*                                                                 !*/

   dsdt= SQRT(out[3]*out[3]+out[4]*out[4]+out[5]*out[5]);
   if ( dsdt < 1e-10 ) dsdt= 1e-10;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950 out(3-5) %10.7f %10.7f %10.7f dsdt %f\n",
          out[3],out[4],out[5],dsdt);
  }
#endif

/*!                                                                 */
/* First kappa*binormal                                             */
/*                                                                 !*/

   out[12]= (out[4]*out[8]-out[5]*out[7])/dsdt/dsdt/dsdt;
   out[13]= (out[5]*out[6]-out[3]*out[8])/dsdt/dsdt/dsdt;
   out[14]= (out[3]*out[7]-out[4]*out[6])/dsdt/dsdt/dsdt;

/*!                                                                 */
/* Kappa is the length of kappa*binormal                            */
/*                                                                 !*/

   out[15]= SQRT(out[12]*out[12]+out[13]*out[13]+out[14]*out[14]);


/*!                                                                 */
/* The binormal (divide with kappa)                                 */
/*                                                                 !*/


   if ( out[15] > 1e-10 ) 
       {
       out[12]= out[12]/out[15];
       out[13]= out[13]/out[15];
       out[14]= out[14]/out[15];
       }

/*!                                                                 */
/* 4. Calculate the principal normal                                */
/* _________________________________                                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* The normalized tangent                                           */
/*                                                                 !*/

   tan[0] = out[3]/dsdt;
   tan[1] = out[4]/dsdt;
   tan[2] = out[5]/dsdt;


/*!                                                                 */
/* The principal normal= binormal X tangent                         */
/*                                                                 !*/

   out[ 9]= out[13]*tan[2]-out[14]*tan[1];
   out[10]= out[14]*tan[0]-out[12]*tan[2];
   out[11]= out[12]*tan[1]-out[13]*tan[0];

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950 binormal         %f %f %f\n",
   out[12],out[13],out[14] );
  fprintf(dbgfil(SURPAC),
  "sur950 principal normal %f %f %f\n",
   out[ 9],out[10],out[11] );
  }
#endif

/*!                                                                 */
/* 5. Calculate the normal curvature                                */
/* _________________________________                                */
/*                                                                  */
/* Calculate the normal curvature                                   */
/* Call of varkon_sur_normkappa (sur952).                           */
/*                                                                 !*/

   varkon_sur_normkappa (uv_in[3],uv_in[4],p_xyz,&nkappa);
/* No errors from this function */

/*!                                                                 */
/* 6. Geodesic curvature                                            */
/* _____________________                                            */
/*                                                                  */
/* Calculate the geodesic (tangential) curvature                    */
/*                                                                 !*/

   geodesic=((out[4]*out[8]-out[5]*out[7])*p_xyz->n_x +   
             (out[5]*out[6]-out[3]*out[8])*p_xyz->n_y +   
             (out[3]*out[7]-out[4]*out[6])*p_xyz->n_z )
                      /dsdt/dsdt/dsdt;

nomore: /*! Label nomore: End of calculation (rcode=0, 1 or 2)     !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur950*Exit*varkon_sur_cureval ****************\n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

