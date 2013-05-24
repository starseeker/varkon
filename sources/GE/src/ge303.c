/*!******************************************************************/
/*  File: ge303.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE303() Calculate the hardpoint of a planar curve segment       */
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

      DBstatus GE303(
      DBVector *pa,
      DBVector *pc,
      DBVector *pta,
      DBVector *ptc,
      DBVector *pb)

/*    Calculates the hardpoint of a planar curve by using
 *    the law of sine for triangles.
 *
 *      In: pa  => Pointer to start coordinates
 *          pc  => Pointer to end coordinates
 *          pta => Pointer to start tangent
 *          ptc => Pointer to end tangent
 *
 *      Out: *pb => Hardpoint coordinates
 *
 *      (C)microform ab 1984-11-24 Thomas Schierwagen
 *
 *      1999-04-15 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  c1,c2,c3;
   DBVector ac,pn;

/*
***Check Z-coordinates.
*/
   if ( ABS(pa->z_gm - pc->z_gm) > TOL1 ) return(erpush("GE3062","GE303"));
   if ( pta->z_gm > TOL1 ) return(erpush("GE3062","GE303"));
   if ( ptc->z_gm > TOL1 ) return(erpush("GE3062","GE303"));
/*
***Calculate plane normal
*/
   pta->z_gm = ptc->z_gm = 0.0;
   GEvector_product(pta,ptc,&pn);
/*
***Calculate hardpoint.
*/
   ac.x_gm = pa->x_gm - pc->x_gm;
   ac.y_gm = pa->y_gm - pc->y_gm;
   ac.z_gm = 0.0;

   GEtriscl_product(pta,ptc,&pn,&c1);
   GEtriscl_product(ptc,&ac,&pn,&c2);

   c2 /= c1;

   GEtriscl_product(&ac,pta,&pn,&c3);

   c3 /= c1;

   pb->x_gm = ((pa->x_gm + pc->x_gm) + c2 * pta->x_gm - c3 * ptc->x_gm)/2.0;
   pb->y_gm = ((pa->y_gm + pc->y_gm) + c2 * pta->y_gm - c3 * ptc->y_gm)/2.0;
   pb->z_gm = pa->z_gm;

   return(0);
 }

/********************************************************************/
