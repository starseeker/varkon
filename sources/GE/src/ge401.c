/*!******************************************************************/
/*  File: ge401.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE401() Create point projected to XY-plane of coordinate system */
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
/*  (C)Microform AB 1984-1999, J.Kjellander, johan@microform.se     */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/

      DBstatus GE401(
      DBVector *ppin,
      DBTmat   *pt,
      DBPoint  *ppout)

/*    Create a point projected to the XY-plane of a
 *    local coordinate system.
 *
 *      In: ppin    = Pointer to coordinates.
 *          pt      = Coordinate system
 *
 *      Out: *ppout = Projected point.
 *
 *
 *      (C)microform ab 1999-04-14 J. Kjellander
 *
 *****************************************************************!*/

 {
   DBVector  localp;

/*
***Project the input position.
*/
   if ( pt == NULL )
     {
     ppout->crd_p.x_gm = ppin->x_gm;
     ppout->crd_p.y_gm = ppin->y_gm;
     ppout->crd_p.z_gm = 0.0;
     }
   else
     {
     GEtfpos_to_local(ppin,pt,&localp);
     localp.z_gm = 0.0;
     GEtfpos_to_basic(&localp,pt,&ppout->crd_p);
     }

   return(0);
 }

/********************************************************************/
