/*!******************************************************************/
/*  File: ge400.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE400() Create point                                            */
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

      DBstatus GE400(
      DBVector *ppin,
      DBPoint  *ppout)

/*    Create a point.
 *
 *      In: ppin    => Pointer to coordinates.
 *
 *      Out: *ppout => Point.
 *
 *
 *      (C)microform ab 1999-04-14 J. Kjellander
 *
 *****************************************************************!*/

 {
   ppout->crd_p.x_gm = ppin->x_gm;
   ppout->crd_p.y_gm = ppin->y_gm;
   ppout->crd_p.z_gm = ppin->z_gm;

   return(0);
 }

/********************************************************************/
