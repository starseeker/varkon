/*!******************************************************************/
/*  File: ge353.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE353() Creation of a circle defined by the origin, the         */
/*          start point and the end point                           */
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

      DBstatus GE353(
      DBVector *porig,
      DBVector *pstart,
      DBVector *pend,
      DBTmat   *pc,
      DBArc    *pa,
      DBSeg    *paseg,
      short     modtyp)

/*    Create a circle/arc defined by origin and end points.
 *
 *      In: porig  => Pointer to center position
 *          pstart => Pointer to start position
 *          pend   => Pointer to end position
 *          pc     => Pointer to coordinate system
 *          modtyp => 2 or 3 for 2D or 3D
 *
 *      Out: *pa    => Output arc fillet
 *           *paseg => Output segments
 *
 *      (C)microform ab 1985-09-01 G. Liden
 *
 *      1999-04-16 Rewritten J. Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  dum1,dum2;  /* Dummy variables                       */
   DBPoint  projor;     /* The projected origin point            */
   DBPoint  projst;     /* The projected start  point            */
   DBPoint  projen;     /* The projected end    point            */
   DBVector vstart;     /* Vector from origin to start point     */
   DBVector vend;       /* Vector from origin to end   point     */
   DBVector xaxis;      /* The xaxis of pc                       */
   DBVector yaxis;      /* The yaxis of pc                       */
   DBfloat  radius;     /* The radius of the circle              */
   DBfloat  ang1;       /* The start angle of the circle         */
   DBfloat  ang2;       /* The end   angle of the circle         */

/*
***Projection of the input points
*/
   if ( GE401(porig,pc,&projor) < 0 )
     return(erpush("GE3333","GE353(GE401)"));

   if ( GE401(pstart,pc,&projst) < 0 )
     return(erpush("GE3333","GE353(GE401)"));

   if ( GE401(pend,pc,&projen) < 0 )
     return(erpush("GE3333","GE353(GE401)"));
/*
***The vectors from origin to the start and end points
*/
   vstart.x_gm = projst.crd_p.x_gm - projor.crd_p.x_gm;
   vstart.y_gm = projst.crd_p.y_gm - projor.crd_p.y_gm;
   vstart.z_gm = projst.crd_p.z_gm - projor.crd_p.z_gm;

   vend.x_gm = projen.crd_p.x_gm - projor.crd_p.x_gm;
   vend.y_gm = projen.crd_p.y_gm - projor.crd_p.y_gm;
   vend.z_gm = projen.crd_p.z_gm - projor.crd_p.z_gm;
/*
***The radius (and a check that both vectors have the same length)
*/
   radius = GEvector_length3D(&vstart);
   if ( radius < 0 ) return(erpush("GE3333","GE353(radius)"));

   if ( ABS(radius - GEvector_length3D(&vend)) > TOL2 )
     return(erpush("GE3343","GE353(diff)"));
/*
***The xaxis of the coordinate system pc
*/
   if ( pc == NULL )
     {
     xaxis.x_gm = 1.0;
     xaxis.y_gm = 0.0;
     xaxis.z_gm = 0.0;
     }
   else
     {
     xaxis.x_gm = pc->g11;
     xaxis.y_gm = pc->g12;
     xaxis.z_gm = pc->g13;
     }
/*
***The yaxis of the coordinate system pc
*/
   if ( pc == NULL )
     {
     yaxis.x_gm = 0.0;
     yaxis.y_gm = 1.0;
     yaxis.z_gm = 0.0;
     }
   else
     {
     yaxis.x_gm = pc->g21;
     yaxis.y_gm = pc->g22;
     yaxis.z_gm = pc->g23;
     }
/*
***The angles between the start (end) vectors to the xaxis in degrees
*/
   if ( GEvector_angled3D(&vstart,&xaxis,&ang1) < 0 )
     return(erpush("GE3333","GE353(ang1)"));

   if ( GEvector_angled3D(&vend,&xaxis,&ang2) < 0 )
     return(erpush("GE3333","GE353(ang2)"));
/*
***Determine sign of angle
*/
   dum1 = GEscalar_product3D(&vstart,&yaxis);
   dum2 = GEscalar_product3D(&vend,&yaxis);

   if ( ang1 > 0.0 && dum1 < 0.0 ) ang1 = -ang1;
   if ( ang2 > 0.0 && dum2 < 0.0 ) ang2 = -ang2;
/*
***Case  1
*/
   if ( ang1 >= 0.0  && ang2 >= 0.0 )
     {
     if ( ang2 < ang1 )
       {
       dum1 = ang1;
       ang1 = ang2;
       ang2 = dum1;
       }
     goto _010;
     }
/*
***Case  2
*/
   if ( ang1 >= 0.0  && ang2 < 0.0 )
     {
     if ( ang2 + 360.0 -ang1 <= 180.0 )
       {
       ang1 = ang1;
       ang2 = ang2 + 360.0;
       goto _010;
       }
     if ( ang2 + 360.0 -ang1 > 180.0 )
       {
       dum1 = ang1;
       ang1 = ang2;
       ang2 = dum1;
       goto _010;
       }
     }
/*
***Case  3
*/
   if ( ang1 < 0.0  && ang2 >= 0.0 )
     {
     if ( ang1 + 360.0 - ang2 >= 180.0 )
       {
       ang1 = ang1;
       ang2 = ang2;
       goto _010;
       }
     if ( ang1 + 360.0 - ang2 < 180.0 )
       {
       dum1 = ang1;
       ang1 = ang2;
       ang2 = dum1 + 360.0;
       goto _010;
       }
     }
/*
***Case  4
*/
   if ( ang1 < 0.0  && ang2 < 0.0 )
     {
     if ( ang2 + 360.0 >= ang1 + 360.0 )
       {
       ang1 = ang1 + 360.0;
       ang2 = ang2 + 360.0;
       goto _010;
       }
     if ( ang2 + 360.0 < ang1 + 360.0 )
       {
       dum1 = ang1;
       ang1 = ang2 + 360.0;
       ang2 = dum1 + 360.0;
       goto _010;
       }
     }
_010:
/*
***Create the circle
*/
   if (GE300(&projor.crd_p,radius,ang1,ang2,pc,pa,paseg,modtyp) < 0 )
       return(erpush("GE3333","geo353(GE300)"));

   return(0);
 }

/********************************************************************/
