/*!******************************************************************/
/*  File: ge310.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE310() Create arc from 3 positions                             */
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

      DBstatus GE310(
      DBVector *ppoi1,
      DBVector *ppoi2,
      DBVector *ppoi3,
      DBTmat   *pc,
      DBArc    *arcpek,
      DBSeg    *segmnt,
      short     alt)

/*    Create arc from start, intermediate and end position.
 *
 *      In: ppoi1 => Pointer to start position
 *          ppoi2 => Pointer to mid position
 *          ppoi3 => Pointer to end position
 *          pc    => Pointer to coordinate system
 *          alt   => 2 or 3 for 2D or 3D
 *
 *      Out: *arcpek => Arc
 *           *segmnt => Arc segments
 *
 *      (C)microform ab 1984-12-08 Thomas Schierwagen
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  dum1,dum2,dum3;    /* help variables                          */
   DBfloat  dum21,dum22,dum23; /* help variables                          */
   DBfloat  dum11;             /* guess what                              */
   DBVector poi;               /* center point for circle                 */
   DBVector or;                /* center point, origin, for circle in basic */
   DBfloat  rad;               /* radius for circle                         */
   DBfloat  ang1;              /* start angle for circle, count. from x-axis*/
   DBfloat  ang2;              /* end angle for circle, count from x-axis   */
   DBfloat  angm;              /* angle, from x-axis, to poi2               */
   DBfloat  deg1[2];           /* used for angle to poi1 calc.              */
   DBfloat  deg2[2];           /* used for angle to poi2 calc.              */
   DBfloat  deg3[2];           /* used for angle to poi3 calc.              */
   DBVector proj1;             /* coordinates for poi1 in local system      */
   DBVector proj2;             /* coordinates for poi2 in local system      */
   DBVector proj3;             /* coordinates for poi3 in local system      */

/*
***Project to local XY-plane.
*/
   if ( GEtfpos_to_local(ppoi1,pc,&proj1) < 0 )
     return(erpush("GE3032","GE310"));
   if ( GEtfpos_to_local(ppoi2,pc,&proj2) < 0 )
     return(erpush("GE3032","GE310"));
   if ( GEtfpos_to_local(ppoi3,pc,&proj3) < 0 )
     return(erpush("GE3032","GE310"));

   proj1.z_gm = proj2.z_gm = proj3.z_gm = 0.0;
/*
***Check for equal points.
*/
   dum1 = ABS(proj1.x_gm - proj2.x_gm);
   dum2 = ABS(proj1.y_gm - proj2.y_gm);
   if ( dum1 < TOL1 && dum2 < TOL1 ) return(erpush("GE3162","GE310"));

   dum1 = ABS(proj1.x_gm - proj3.x_gm);
   dum2 = ABS(proj1.y_gm - proj3.y_gm);
   if ( dum1 < TOL1 && dum2 < TOL1 ) return(erpush("GE3162","GE310"));

   dum1 = ABS(proj2.x_gm - proj3.x_gm);
   dum2 = ABS(proj2.y_gm - proj3.y_gm);
   if ( dum1 < TOL1 && dum2 < TOL1 ) return(erpush("GE3162","GE310"));
/*
***Check for colinearity.
*/
   dum1 = (proj2.x_gm - proj1.x_gm) * (proj3.x_gm - proj1.x_gm) +
          (proj2.y_gm - proj1.y_gm) * (proj3.y_gm - proj1.y_gm);
   dum1 = ABS(dum1);

   dum2 = SQRT((proj2.x_gm - proj1.x_gm) * (proj2.x_gm - proj1.x_gm) +
               (proj2.y_gm - proj1.y_gm) * (proj2.y_gm - proj1.y_gm));
   dum3 = SQRT((proj3.x_gm - proj1.x_gm) * (proj3.x_gm - proj1.x_gm) +
               (proj3.y_gm - proj1.y_gm) * (proj3.y_gm - proj1.y_gm));
   dum1 = dum1/dum2/dum3;
   dum1 = ABS(dum1 - 1.0);

   if ( dum1 < TOL1 ) return(erpush("GE3172","GE310"));
/*
***Solve system and compute origin.
*/
   dum1 = 2 * ((proj2.y_gm - proj1.y_gm) * (proj2.x_gm - proj3.x_gm) -
               (proj2.y_gm - proj3.y_gm) * (proj2.x_gm - proj1.x_gm));
   dum11 = ABS(dum1);
   if ( dum11 < TOL1 ) return(erpush("GE3182","GE310"));
   dum2 = proj2.x_gm - proj3.x_gm;
   dum22 = proj2.x_gm * proj2.x_gm - proj1.x_gm * proj1.x_gm +
           proj2.y_gm * proj2.y_gm - proj1.y_gm * proj1.y_gm;
   dum2 = dum2 * dum22;
   dum21 = proj2.x_gm - proj1.x_gm;
   dum23 = proj2.x_gm * proj2.x_gm - proj3.x_gm * proj3.x_gm +
           proj2.y_gm * proj2.y_gm - proj3.y_gm * proj3.y_gm;
   dum21 = dum21 * dum23;
   dum2 = dum2 - dum21;
/*
***Y-coordinate.
*/
   poi.y_gm = dum2 / dum1;
/*
***X-coordinate.
*/
   dum21 = ABS(proj2.x_gm - proj3.x_gm);
   if ( dum21 < TOL1 )
     {
     dum1 = (proj2.x_gm * proj2.x_gm - proj1.x_gm * proj1.x_gm+proj2.y_gm
     * proj2.y_gm - proj1.y_gm * proj1.y_gm)/2/(proj2.x_gm - proj1.x_gm);
     dum2 = (proj2.y_gm - proj1.y_gm) / (proj2.x_gm - proj1.x_gm) * poi.y_gm;
     poi.x_gm = dum1 - dum2;
     }
   else
     {
     dum1 = (proj2.x_gm * proj2.x_gm - proj3.x_gm * proj3.x_gm+proj2.y_gm
     * proj2.y_gm - proj3.y_gm * proj3.y_gm)/2/(proj2.x_gm - proj3.x_gm);
     dum2 = (proj2.y_gm - proj3.y_gm) / (proj2.x_gm - proj3.x_gm) * poi.y_gm;
     poi.x_gm = dum1 - dum2;
     }
/*
***Radius.
*/
   rad = SQRT((poi.x_gm - proj1.x_gm) * (poi.x_gm - proj1.x_gm) +
         (poi.y_gm - proj1.y_gm) * (poi.y_gm - proj1.y_gm));
/*
***Angles.
*/
   deg1[0] = (proj1.x_gm - poi.x_gm) / rad;
   deg1[1] = (proj1.y_gm - poi.y_gm) / rad;

   deg2[0] = (proj2.x_gm - poi.x_gm) / rad;
   deg2[1] = (proj2.y_gm - poi.y_gm) / rad;

   deg3[0] = (proj3.x_gm - poi.x_gm) / rad;
   deg3[1] = (proj3.y_gm - poi.y_gm) / rad;

   if ( deg1[0] >= 0.99999 ) ang1 = 0.0;
   else if ( deg1[0] <= -0.99999 ) ang1 = 180.0;
   else if ( deg1[0] >= -0.00001 && deg1[0] <= 0.00001 ) ang1 = 90.0;
   else
     {
     ang1 = DACOS(deg1[0]);
     ang1 = ang1 * 180.0 / PI;
     }
   if ( deg1[1] < 0.0 ) ang1 = -ang1;

   if ( deg3[0] >= 0.99999 ) ang2 = 0.0;
   else if ( deg3[0] <= -0.99999 ) ang2 = 180.0;
   else if ( deg3[0] >= -0.00001 && deg3[0] <= 0.00001 ) ang2 = 90.0;
   else
     {
     ang2 = DACOS(deg3[0]) * 180.0 / PI;
     }
   if ( deg3[1] < 0.0 ) ang2 = -ang2;

   if ( deg2[0] >= 0.99999 ) angm = 0.0;
   else if ( deg2[0] <= -0.99999 ) angm = 180.0;
   else if ( deg2[0] >= -0.00001 && deg2[0] <= 0.00001 ) angm = 90.0;
   else
     {
     angm = DACOS(deg2[0]) * 180.0 / PI;
     }
   if ( deg2[1] < 0.0 ) angm = -angm;
/*
***Make angles positive.
*/
   if ( ang1 < 0.0 ) ang1 = 360.0 + ang1;
   if ( ang2 < 0.0 ) ang2 = 360.0 + ang2;
   if ( angm < 0.0 ) angm = 360.0 + angm;
/*
***Make ang2 > ang1.
*/
   if ( ang2 < ang1 )
     {
     dum1 = ang1;
     dum2 = ang2;
     ang2 = dum1;
     ang1 = dum2;
     }
/*
***Mid angle between 1 and 2.
*/
   if ( angm < ang1 || angm > ang2 )
     {
     dum1 = ang1;
     dum2 = ang2;
     ang1 = dum2;
     ang2 = dum1;
     }
/*
***Z-coordinate.
*/
   poi.z_gm = proj1.z_gm;
/*
***Transform back to basic.
*/
   if ( GEtfpos_to_basic(&poi,pc,&or) < 0 ) return(erpush("GE3152","GE310"));
/*
***Create circle.
*/
    return(GE300(&or,rad,ang1,ang2,pc,arcpek,segmnt,alt));
 }

/********************************************************************/
