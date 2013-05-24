/*!******************************************************************/
/*  File: ge204.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE204() Create line with angle to current X-axis                */
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
/*                                                                  */
/*  Gunnar Liden 1985-07-23                                         */
/*                                                                  */
/*  Revised 1999-03-04 Rewritten, J.Kjellander                      */
/*                                                                  */
/********************************************************************/

      DBstatus GE204(
      DBVector *pos,     /* Pointer to start position               */
      DBTmat   *pc,      /* Pointer to coordinatesystem             */
      DBfloat   ang,     /* Angle to X-axis                         */
      DBfloat   len,     /* Line length                             */
      DBLine   *plout)   /* Pointer to the output line              */

 { 
      DBVector proj1,proj2;
      DBfloat  angrad;
      DBLine   projli;

      /* Project the input point onto the z-plane */

      if ( pc == NULL )
        {
        proj1.x_gm = pos->x_gm;
        proj1.y_gm = pos->y_gm;
        }
       else GEtfpos_to_local(pos,pc,&proj1);

      proj1.z_gm = 0.0;

      /* Compute the endpoint */

      angrad = ang*DGTORD;
      proj2.x_gm = proj1.x_gm + len*COS(angrad);
      proj2.y_gm = proj1.y_gm + len*SIN(angrad);
      proj2.z_gm = 0.0;

      /* Create the projected line in local coordinates        */
      /* and check the input length (= distance between the    */
      /* points )                                              */

      if ( GE200(&proj1,&proj2,&projli) < 0 )
        return(erpush("GE2142","GE204"));

      /* Transform the line back to basic coordinates          */
      /* and file data. No error possible here                 */

      GEtfLine_to_basic(&projli,pc,plout);

      return(0);
 }

/********************************************************************/
