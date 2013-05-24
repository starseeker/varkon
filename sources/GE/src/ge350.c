/*!******************************************************************/
/*  File: ge350.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE350() Creation of a circle as two rational cubic segments     */
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

      DBstatus GE350(
      DBVector *por,
      DBfloat   radius,
      DBTmat   *pc,
      DBArc    *pa,
      DBSeg    *paseg)

/*    Creation of a circle as two rational cubic segments.
 *
 *      In: por    => Circle origin
 *          radius => Circle radius
 *          pc     => Coordinate system
 *
 *      Out: *pa    => Arc
 *           *paseg => Arc segments
 *
 *      (C)microform ab 1985-01-05 G.Liden
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;     /* Status of gei350()                      */
   DBVector proj1;      /* The projected  origin point             */
   DBSeg    uarc;       /* The upper half of the circle            */
                        /* in local coordinates                    */
   DBSeg    larc;       /* The lower half of the circle            */
                        /* in local coordinates                    */
   DBTmat   tran;       /* Translation matrix                      */

/*
***Project the input origin point
*/
   if ( GEtfpos_to_local(por,pc,&proj1) < 0 )
     return(erpush("GE3262","GE350"));
   proj1.z_gm = 0.0;
/*
***Create the upper half circle in local coordinates
***First with origin = (0,0,0)
*/
   status = GE351(radius,&uarc);
   if ( status < 0 ) return(erpush("GE3272","GE350"));
/*
***Create the lower part of the circle
***Rotation of uarc with 180 degrees
*/
   larc.c0x = -radius;
   larc.c0y = uarc.c0y;
   larc.c0z = uarc.c0z;
   larc.c0  = uarc.c0;

   larc.c1x =  2.0*radius;
   larc.c1y = -2.0*radius;
   larc.c1z = uarc.c1z;
   larc.c1  = uarc.c1;

   larc.c2x = uarc.c2x;
   larc.c2y =  2.0*radius;
   larc.c2z = uarc.c2z;
   larc.c2  = uarc.c2;

   larc.c3x = uarc.c3x;
   larc.c3y = uarc.c3y;
   larc.c3z = uarc.c3z;
   larc.c3  = uarc.c3;
/*
***Translation matrix  (the circles to the origin )
*/
   tran.g11 = 1.0;
   tran.g12 = 0.0;
   tran.g13 = 0.0;
   tran.g14 = proj1.x_gm;

   tran.g21 = 0.0;
   tran.g22 = 1.0;
   tran.g23 = 0.0;
   tran.g24 = proj1.y_gm;

   tran.g31 = 0.0;
   tran.g32 = 0.0;
   tran.g33 = 1.0;
   tran.g34 = proj1.z_gm;

   tran.g41 = 0.0;
   tran.g42 = 0.0;
   tran.g43 = 0.0;
   tran.g44 = 1.0;
/*
***Translate the upper half circle to the origin
*/
   if ( GEtfseg_to_local(&uarc,&tran,&uarc) < 0 )
     return(erpush("GE3292","GE350"));
/*
***Translate the lower part of the circle to the origin
*/
   if ( GEtfseg_to_local(&larc,&tran,&larc) < 0 )
     return(erpush("GE3302","GE350"));
/*
***Transform the upper circle to basic coordinates
*/
   if ( GEtfseg_to_basic(&uarc,pc,paseg) < 0 )
     return(erpush("GE3282","GE350"));
/*
***Transform the lower circle to basic coordinates
*/
   if ( GEtfseg_to_basic(&larc,pc,(paseg+1)) < 0 )
     return(erpush("GE3282","GE350"));
/*
***The number of segments to the arc structure
*/
   pa->ns_a = 2;
/*
***Offset = 0. 1991-12-12 JK.
*/
   paseg->ofs = (paseg+1)->ofs = 0.0;
/*
***Segment type = CUB_SEG. 1994-12-4 JK.
***Segment length = 0. Bugfix (paseg+1) 18/1-95 JK.
*/
   paseg->typ = (paseg+1)->typ = CUB_SEG;
   paseg->sl  = (paseg+1)->sl  = 0.0;

   return(0);
 }

/********************************************************************/
