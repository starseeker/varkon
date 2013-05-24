/*!******************************************************************/
/*  File: ge354.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE354() Computes offset directions for a fillet                 */
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

      DBstatus GE354(
      DBfloat  radius,
      short    nalt,
      DBfloat *poffs1,
      DBfloat *poffs2)

/*    Select which of the 4 corners to use for a fillet. Set
 *    the sign of the offset accordingly.
 *
 *      In: radius => Fillet radius
 *          nalt   => Defines the corner
 *
 *      Out: *poffs1 => The offset for line 1
 *           *poffs2 => The offset for line 2
 *
 *      (C)microform ab 1985-08-11 G. Liden
 *
 *      1999-04-17 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {

/*
***Which alternative ?
*/
   switch ( nalt )
     {
     case 1:
       {
      *poffs1 = radius;
      *poffs2 = radius;
       break;
       }
     case 2:
       {
      *poffs1 = - radius;
      *poffs2 =   radius;
       break;
       }
     case 3:
       {
      *poffs1 =   radius;
      *poffs2 = - radius;
       break;
       }
     case 4:
       {
      *poffs1 = - radius;
      *poffs2 = - radius;
       break;
       }
     }

   return(0);
 }

/********************************************************************/
