/*!******************************************************************/
/*  File: ge302.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE()302 Calculate normalized tangent on circle                  */
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

      DBstatus GE302(
      DBVector *pcen,
      DBVector *ppoi,
      DBVector *ptan)

/*    Calculate normalized tangent on circle.
 *
 *      In:  pcen  => Pointer to circle center
 *           ppoi  => Pointer to point
 *
 *      Out: *ptan => Output tangent
 *
 *      (C)microform ab 1984-11-24 Thomas Schierwagen
 *
 *      1999-04-15 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat sum;

/*
***Check Z-coordinates.
*/
   if ( ABS(ppoi->z_gm - pcen->z_gm) > TOL1 ) return(erpush("GE3052","GE302"));
/*
***Calculate circle tangent.
*/
   ptan->x_gm = pcen->y_gm - ppoi->y_gm;
   ptan->y_gm = ppoi->x_gm - pcen->x_gm;
   ptan->z_gm = ppoi->z_gm;
/*
***Normalize.
*/
   sum = SQRT(ptan->x_gm*ptan->x_gm + ptan->y_gm*ptan->y_gm +
              ptan->z_gm*ptan->z_gm);

   if ( sum < TOL2 ) return(erpush("GE3042","GE302"));

   ptan->x_gm /= sum;
   ptan->y_gm /= sum;
   ptan->z_gm /= sum;

   return(0);
 }

/********************************************************************/
