/*!******************************************************************/
/*  File: ge351.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE351() Convert the upper half of a circle to a segment         */
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

      DBstatus GE351(
      DBfloat radius,
      DBSeg  *pr)      

/*    Convert the upper half of a circle to a segment.
 *
 *      In: radius => The circle radius
 *
 *      Out: *pr => The segment
 *
 *      (C)microform ab 1985-01-05 G.Liden
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {

/*
***A circle is a rational quadratic. All c3 coeffients = 0
*/
   pr->c3x = pr->c3y = pr->c3z = pr->c3 = 0.0;
/*
***The circle is in the z=0.0 plane
*/
   pr->c0z = pr->c1z = pr->c2z = 0.0;
/*
**The coefficients for the upper half circle with origin = 0,0
*/
   pr->c0x =  radius;
   pr->c1x = -2.0*radius;
   pr->c2x =  0.0;

   pr->c0y =  0.0;
   pr->c1y =  2.0*radius;
   pr->c2y = -2.0*radius;
    
   pr->c0  =  1.0;
   pr->c1  = -2.0;
   pr->c2  =  2.0;

   return(0);
 }

/********************************************************************/
