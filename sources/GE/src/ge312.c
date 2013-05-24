/*!******************************************************************/
/*  File: ge312.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE312() Normalize circle angles                                 */
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

/*!******************************************************/

        DBstatus GE312(
        DBfloat *v1,
        DBfloat *v2)

/*      Normalizes circle angles and checks that they are
 *      not equal.
 *
 *      In: v1 = Pointer to start angle
 *          v2 = Pointer to end angle
 *
 *      Ut: *v1 = Normalized start angle
 *          *v2 = Normalized end angle
 *
 *      (C)microform ab 7/12/89 J. Kjellander
 *
 *      1999-05-06 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Make end angle larger than start angle.
*/
   while ( *v1 > *v2 ) *v2 += 360.0;
/*
***Make difference <= 360 degrees.
*/
   while ( *v2-*v1 > 360.0 + TOL7 ) *v1 += 360.0;
/*
***Make end angle <= 360.0
*/
   while ( *v2 > 360.0 )
     {
     *v1 -= 360.0;
     *v2 -= 360.0;
     }
/*
***Make start angle positive.
*/
   while ( *v2 < 0.0 )
     {
     *v1 += 360.0;
     *v2 += 360.0;
     }
/*
***Check difference.
*/
   if ( ABS(*v2 - *v1)  <  TOL7 ) return(-1);
   else return(0);
  }

/********************************************************/
