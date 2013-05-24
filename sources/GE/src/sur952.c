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
/*  Function: varkon_sur_normkappa                 File: sur952.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the normal curvature for a point        */
/*  on a surface in a given direction                               */
/*                                                                  */
/*  Input is a direction in the U,V plane (the tangent of a         */
/*  surface curve) and derivatives for the surface.                 */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-23   Originally written                                 */
/*  1996-05-28   errbuf deleted                                     */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_normkappa  Normal curvature                 */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*  Reference: Faux & Pratt p 111                                   */
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
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_normkappa (sur952).  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_sur_normkappa (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  dudt,        /* Direction: U component                  */
   DBfloat  dvdt,        /* Direction: V component                  */
   EVALS   *p_xyz,       /* Coordin./derivat. for surface pt  (ptr) */

/* Out:                                                             */
   DBfloat   *p_nkappa )   /* Normal curvature                  (ptr) */

/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  g11,g12;     /* First  fundamental matrix               */
   DBfloat  g21,g22;     /*                                         */
   DBfloat  d11,d12;     /* Second fundamental matrix               */
   DBfloat  d21,d22;     /*                                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBfloat  denom;       /* Denominator for normal curvature        */

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
  "sur952 Enter*varkon_sur_normkappa**** dudt %f dvdt %f\n",dudt,dvdt);
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
  "sur952 p_xyz= %f %f %f \n",
          p_xyz->r_x , p_xyz->r_y  ,p_xyz->r_z );
  }
#endif


/*!                                                                 */
/* 2. Fundamental matrices D and G                                  */
/* _______________________________                                  */
/*                                                                  */
/* Second fundamental matrix D= d11, d12, d21, d22                  */
/*                                                                 !*/

      d11 = p_xyz->n_x*p_xyz->u2_x +
            p_xyz->n_y*p_xyz->u2_y +
            p_xyz->n_z*p_xyz->u2_z;
      d12 = p_xyz->n_x*p_xyz->uv_x +
            p_xyz->n_y*p_xyz->uv_y +
            p_xyz->n_z*p_xyz->uv_z;
      d21 = d12;
      d22 = p_xyz->n_x*p_xyz->v2_x +
            p_xyz->n_y*p_xyz->v2_y +
            p_xyz->n_z*p_xyz->v2_z;


/*!                                                                 */
/* First fundamental matrix G= g11, g12, g21, g22                   */
/*                                                                 !*/

      g11 = p_xyz->u_x*p_xyz->u_x+
            p_xyz->u_y*p_xyz->u_y+ 
            p_xyz->u_z*p_xyz->u_z;
      g12 = p_xyz->u_x*p_xyz->v_x+
            p_xyz->u_y*p_xyz->v_y+
            p_xyz->u_z*p_xyz->v_z;
      g21 = g12;
      g22 = p_xyz->v_x*p_xyz->v_x+
            p_xyz->v_y*p_xyz->v_y+
            p_xyz->v_z*p_xyz->v_z;


/*!                                                                 */
/* 3. Calculate normal curvature                                    */
/* _____________________________                                    */
/*                                                                  */
/*                                                                 !*/

  *p_nkappa= dudt*dudt*d11 + 2.0*dudt*dvdt*d21 + dvdt*dvdt*d22;
  denom = dudt*dudt*g11 + 2.0*dudt*dvdt*g21 + dvdt*dvdt*g22;

  if (  fabs(denom) >  1e-12  )
  *p_nkappa= *p_nkappa/denom;
  else
  *p_nkappa= 50000.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur952*Exit*varkon_sur_normkappa***** *p_nkappa %20.16f\n",
        *p_nkappa );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

