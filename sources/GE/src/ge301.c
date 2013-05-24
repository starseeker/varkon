/*!******************************************************************/
/*  File: ge301.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE301() Calculate circle segment data                           */
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

      DBstatus GE301(
      DBVector *ppos,
      DBfloat   rad,
      DBfloat   ang1,
      DBfloat   ang2,
      DBVector *segdat,
      DBfloat  *ppval)
      
/*    This routine is invoked by the GE300 function, and will get
 *    coordinate information according to current coord. system
 *    The routine calculates start and end coordinates for a circle
 *    segment and administrates hardpoint and pvalue calculations
 *
 *      In:  ppos  => Pointer to origin in local coordinates
 *           rad   => Radius
 *           ang1  => Start angle
 *           ang2  => End angle
 *
 *      Out: segdat[0]   X,Y,Z for start point on segment
 *           segdat[1]   X,Y,Z for hardpoint of segment
 *           segdat[2]   X,Y,Z for hardpoint of segment
 *           segdat[3]   X,Y,Z for end point on segment
 *          *ppval       Pvalue for segment
 *
 *      (C)microform ab 1994-11-18 Thomas Scheirwagen
 *
 *      1999-04-15 rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    stat=0;                 /* status from called func. */
   DBVector ta;                     /* start tangent */
   DBVector tc;                     /* end tangent */
   DBfloat  theta;                  /* half the included angle */

/*
***Calculate start and end coordinates on circle segment
*/
   ang1 *= DGTORD;
   ang2 *= DGTORD;

   segdat->x_gm = ppos->x_gm + rad * COS(ang1);
   segdat->y_gm = ppos->y_gm + rad * SIN(ang1);
   segdat->z_gm = ppos->z_gm;

   (segdat+3)->x_gm = ppos->x_gm + rad * COS(ang2);
   (segdat+3)->y_gm = ppos->y_gm + rad * SIN(ang2);
   (segdat+3)->z_gm = ppos->z_gm;
/*
***Calculate tangents in above points
*/
   stat = GE302(ppos,&segdat[0],&ta);
   if ( stat < 0 ) return(stat);
   stat = GE302(ppos,&segdat[3],&tc);
   if ( stat < 0 ) return(stat);
/*
***The hardpoint(s) can now be calculated
*/
   stat = GE303(&segdat[0],&segdat[3],&ta,&tc,&segdat[1]);
   if ( stat < 0 ) return(stat);

   (segdat+2)->x_gm = (segdat+1)->x_gm;
   (segdat+2)->y_gm = (segdat+1)->y_gm;
   (segdat+2)->z_gm = (segdat+1)->z_gm;
/*
***Calculate the pvalue
*/
   theta = ( ang2 - ang1 ) / 2.0;
   *ppval = 1.0 / (1.0 + 1.0 / COS(theta));

   return(0);
 }

/********************************************************************/
