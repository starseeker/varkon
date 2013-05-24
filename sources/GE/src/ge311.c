/*!******************************************************************/
/*  File: ge311.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE311() Curve global parameter to arclength for a 3D circle     */
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

      DBstatus GE311(
      DBArc   *parc,
      DBSeg   *pseg,
      DBfloat  uglob,
      DBfloat *ps)

/*    The function calculates the arclength parameter value for a
 *    given global curve parameter value on a 3D circle.
 *    The arclength parameter is a relative arclength (0 <= s <= 1).
 *    The global curve parameter is the sum of the curve segment
 *    number and the local parameter value uglob = iseg+u (0 <=u<= 1)
 *    The 2D circle is not handled in this function.
 *
 *      In: parc  => Pointer to arc
 *          pseg  => Pointer to arc segments
 *          uglob => Parameter value
 *
 *      Out: *ps => Relative arclength (0-1)
 *
 *      (C)microform ab 1992-02-07 Gunnar Liden 
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from called function    */
   short    noseg;       /* Number of segments in the curve        */
   DBfloat  v_total;     /* The total angle for the circle         */
   DBfloat  v_delta;     /* The delta angle for the input uglob    */
   short    it;          /* Integer part of uglob                  */
   DBfloat  u;           /* The decimal part og uglob              */
   DBfloat  out[16];     /* Point and derivatives from GE107       */

   DBfloat  v_delta1;    /* The delta angle for first segment      */
   DBfloat  v_delta2;    /* The delta angle for second segment     */
   DBfloat  v_delta3;    /* The delta angle for third segment      */

   DBVector vec1;        /* Tangent 1 vector                       */
   DBVector vec2;        /* Tangent 2 vector                       */

/*
***Retrieve number of segments noseg
***Error if input curve is 2D circle
*/
    noseg = parc->ns_a;

    if ( noseg == 0 ) return(erpush("GE7323","GE311"));
/*
***Total angle difference v_total from start angle to end angle
***from arc header parc (arclength = radius*v_total*pi/180 )
*/
    v_total = ABS(parc->v2_a - parc->v1_a);
/*
***Integer part it and local parameter u for input uglob
*/
    it = HEL(uglob);
    u  = uglob-it;
/*
***The maximum number of segments in an arc is four and the
***maximum angle difference in one segment is 180 degrees.
***Calculate v_delta as the sum of the angle differences in the
***segments 1,2,.. it-1 and add angle difference in segment it.
***Calls of GE107 and GEvector_angled3D.
***Start tangent to vec1
*/
    status= GE107((GMUNON *)parc,pseg,0.0,(short)1,out);
    if ( status < 0 ) return(status);
    vec1.x_gm = out[3];
    vec1.y_gm = out[4];
    vec1.z_gm = out[5];
/*
***How many segments are involved ? (1-4)
*/
    if ( it == 1 )
        {
        status= GE107((GMUNON *)parc,pseg,u,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta);
        if ( status < 0 ) return(status);
        }

    else if ( it == 2 )
        {
        status= GE107((GMUNON *)parc,pseg,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta1);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg+1,u,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta);
        if ( status < 0 ) return(status);
        v_delta = v_delta + v_delta1;
        }

    else if ( it == 3 )
        {
        status= GE107((GMUNON *)parc,pseg,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta1);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta2);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg+2,u,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta);
        if ( status < 0 ) return(status);
        v_delta = v_delta + v_delta1 + v_delta2;
        }

    else if ( it == 4 )
        {
        status= GE107((GMUNON *)parc,pseg,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta1);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg+1,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta2);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg+2,1.0,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta3);
        if ( status < 0 ) return(status);
        vec1.x_gm = vec2.x_gm;
        vec1.y_gm = vec2.y_gm;
        vec1.z_gm = vec2.z_gm;
        status= GE107((GMUNON *)parc,pseg+3,u,(short)1,out);
        if ( status < 0 ) return(status);
        vec2.x_gm = out[3];
        vec2.y_gm = out[4];
        vec2.z_gm = out[5];
        status= GEvector_angled3D(&vec1,&vec2,&v_delta);
        if ( status < 0 ) return(status);
        v_delta = v_delta + v_delta1 + v_delta2 + v_delta3;
        }
    else return(erpush("GE3343","GE311"));


    if ( ABS(v_total) > TOL1 ) *ps = v_delta/v_total;
    else                        return(erpush("GE3343","GE311"));

   return(0);
 }

/********************************************************************/
