/*!******************************************************************/
/*  File: ge320.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE320() Circle from 2 points and radius                         */
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

      DBstatus GE320(
      DBVector *ppoi1,
      DBVector *ppoi2,
      DBfloat   rad,
      DBTmat   *pc,
      DBArc    *arcpek,
      DBSeg    *segmnt,
      short     alt)

/*    This routine defines a circle arc out of two points, in basic
 *    coordinate system, and a radius. Start point, end point, radius
 *    gives two possible circles, they are distinguished by the sign
 *    of rad. If rad > 0 the angle circumferenced should be <= 180, if
 *    rad < 0 the angle is larger than 180.
 *
 *      In: ppoi1  => Start position
 *          ppoi2  => End position
 *          rad    => Requested radius with sign
 *          pc     => Coordinate system
 *          alt    => 2 or 3 for 2D and 3D
 *
 *      Out: *arcpek => Arc
 *           *segmnt => Arc segments
 *
 *
 *      (C)microform ab 1984-12-19 Thomas Schierwagen
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  sign;        /* rad with sign                            */
   DBfloat  dum1,dum2;   /* help variables                           */
   DBfloat  u1,v1,q1;    /* help to solve sys of eq. to find centre  */
   DBfloat  d1,d2,c1,c2; /*      --------- = --------------------    */
   DBfloat  xa0,ya0;     /* x,y coord for one of the circle centres  */
   DBfloat  xb0,yb0;     /* x,y coord for the other centre           */
   DBfloat  tax1,tay1;   /* cos, sin for start angle, "a" origin     */
   DBfloat  tax2,tay2;   /* cos, sin for end angle, "a" origin       */
   DBfloat  tbx1,tby1;   /* cos, sin for start angle "b" origin      */
   DBfloat  tbx2,tby2;   /* cos, sin for end angle "b" origin        */
   DBfloat  anga1,anga2; /* start, end angle "a" origin              */
   DBfloat  angb1,angb2; /* start, end angle "b" origin              */
   DBVector pos;         /* coordinates for desired origin           */
   DBVector or;          /* coordinates for centere point in basic   */
   DBfloat  ang1,ang2;   /* desired start and end angles             */
   DBVector proj1;       /* coordinates in ref. plane for poi1       */
   DBVector proj2;       /* coordinates in ref. plane for poi2       */

/*
***Project points to local system
*/
   if ( GEtfpos_to_local(ppoi1,pc,&proj1) < 0 )
      return(erpush("GE3032","GE320"));
   if ( GEtfpos_to_local(ppoi2,pc,&proj2) < 0 )
      return(erpush("GE3032","GE320"));

   proj1.z_gm = proj2.z_gm = 0.0;
/*
***Check if points equal
*/
   dum1 = ABS(proj1.x_gm - proj2.x_gm);
   dum2 = ABS(proj1.y_gm - proj2.y_gm);
   if ( dum1 < TOL1 && dum2 < TOL1 ) return(erpush("GE3192","GE320"));
/*
***Check radius
*/
   if ( rad < TOL1 && rad > -TOL1 ) return(erpush("GE3202","GE320"));
/*
***Save sign of rad for later determination of desired arc
*/
   sign = rad;
   rad  = ABS(rad);
/*
***Solve system of eq. , to get circle centres
*/
   u1 = proj2.x_gm - proj1.x_gm;
   v1 = proj2.y_gm - proj1.y_gm;
   if ( ABS(u1) < TOL1 ) u1 = 0.00001;

   q1 = v1 / u1;
   dum1 = -u1 * u1 / 4.0 + rad * rad /( 1.0 + q1 * q1);
   if ( dum1 < 0.0 ) return(erpush("GE3212","GE320"));
   dum1 = SQRT(dum1);

   d1 = -v1 / 2.0 + dum1;
   d2 = -v1 / 2.0 - dum1;

   c1 = -(u1 * u1 + v1 * v1 + 2.0 * d1 * v1) / 2.0 / u1;
   c2 = -(u1 * u1 + v1 * v1 + 2.0 * d2 * v1) / 2.0 / u1;
/*
***Circle centres are then given by
*/
   xa0 = proj1.x_gm - c1;
   ya0 = proj1.y_gm - d1;

   xb0 = proj1.x_gm - c2;
   yb0 = proj1.y_gm - d2;
/*
***Calculate the possible start and end angles
*/
   tax1 = (proj1.x_gm - xa0) / rad;
   tay1 = (proj1.y_gm - ya0) / rad;
   tax2 = (proj2.x_gm - xa0) / rad;
   tay2 = (proj2.y_gm - ya0) / rad;

   tbx1 = (proj1.x_gm - xb0) / rad;
   tby1 = (proj1.y_gm - yb0) / rad;
   tbx2 = (proj2.x_gm - xb0) / rad;
   tby2 = (proj2.y_gm - yb0) / rad;

   if ( tax1 >= 1.0 )       anga1 = 0.0;
   else if ( tax1 <= -1.0 ) anga1 = 180.0;
   else                     anga1 = DACOS(tax1)*RDTODG;

   if ( tay1 < 0.0 ) anga1 = -anga1;

   if ( tax2 >= 1.0 )       anga2 = 0.0;
   else if ( tax2 <= -1.0 ) anga2 = 180.0;
   else                     anga2 = DACOS(tax2)*RDTODG;

   if ( tay2 < 0.0 ) anga2 = -anga2;

   if ( tbx1 >= 1.0 )       angb1 = 0.0;
   else if ( tbx1 <= -1.0 ) angb1 = 180.0;
   else                     angb1 = DACOS(tbx1)*RDTODG;

   if ( tby1 < 0.0 ) angb1 = -angb1;
   
   if ( tbx2 >= 1.0 )       angb2 = 0.0;
   else if ( tbx2 <= -1.0 ) angb2 = 180.0;
   else                     angb2 = DACOS(tbx2)*RDTODG;

   if ( tby2 < 0.0 ) angb2 = -angb2;
/*
***Make all angles positive
*/
   if ( anga1 < 0.0 ) anga1 += 360.0;
   if ( anga2 < 0.0 ) anga2 += 360.0;
   if ( angb1 < 0.0 ) angb1 += 360.0;
   if ( angb2 < 0.0 ) angb2 += 360.0;

   if ( anga2 - anga1 <= 0.0 ) anga1 -= 360.0;
   if ( angb2 - angb1 <= 0.0 ) angb1 -= 360.0;
/*
***Test which arc to use
*/
   if ( sign > 0.0 )     /* circle arc <= 180 */
     {
     if ( anga2 - anga1 <= 180.0 )  /* the "a" centre is desired */
       {
       pos.x_gm = xa0;
       pos.y_gm = ya0;
       pos.z_gm = proj1.z_gm;
       ang1 = anga1;
       ang2 = anga2;
       }
     else if ( angb2 - angb2 <= 180.0 )   /* the "b" centre should be used */
       {
       pos.x_gm = xb0;
       pos.y_gm = yb0;
       pos.z_gm = proj1.z_gm;
       ang1 = angb1;
       ang2 = angb2;
       }
     else return(erpush("GE3224","GE320"));
     }

   else              /* circle arc > 180 */
     {
     if ( anga2 - anga1 > 180.0 )  /* the "a" centre */
       {
       pos.x_gm = xa0;
       pos.y_gm = ya0;
       pos.z_gm = proj1.z_gm;
       ang1 = anga1;
       ang2 = anga2;
       }
     else if ( angb2 - angb1 > 180.0 )   /* "b" centre */
       {
       pos.x_gm = xb0;
       pos.y_gm = yb0;
       pos.z_gm = proj1.z_gm;
       ang1 = angb1;
       ang2 = angb2;
       }
     else return(erpush("GE3224","GE320"));
   }
/*
***Transform circle centre to basic coordinates
*/
   if ( GEtfpos_to_basic(&pos,pc,&or) < 0 ) return(erpush("GE3152","GE320"));
/*
***Create arc
*/
   return(GE300(&or,rad,ang1,ang2,pc,arcpek,segmnt,alt));
 }

/********************************************************************/
