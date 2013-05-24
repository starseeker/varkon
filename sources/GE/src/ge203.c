/*!******************************************************************/
/*  File: ge203.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE203() Create line perpendicular to another line               */
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
/*                                                                  */
/*  Johan Kjellander 1987-04-24                                     */
/*                                                                  */
/*  Revised 1999-03-04 Rewritten, J.Kjellander                      */
/*                                                                  */
/********************************************************************/

      DBstatus GE203(
      DBVector *pos,     /* Pointer to start position               */
      DBLine   *plin,    /* Pointer to the original line            */
      DBfloat   len,     /* Length of new line                      */
      DBTmat   *pc,      /* Pointer to coordinatesystem             */
      DBLine   *plout)   /* Pointer to the output line              */

 { 
      DBLine   projlin;
      DBVector tang,zaxis,offdir,pend;

      /* Project the input line onto the z-plane */
      /* The points will be projected onto the basic */
      /* z -plane if pc=0. The reason to call GE201 is */
      /* (for pc=0) to check the distance between the points */
      /* Note that the resulting line is in basic coordinates */

      if ( GE201(&plin->crd1_l,&plin->crd2_l,pc,&projlin) < 0 )
        return(erpush("GE2092","GE202"));

     /* Direction vector for the projected line */

      tang.x_gm = projlin.crd2_l.x_gm - projlin.crd1_l.x_gm;
      tang.y_gm = projlin.crd2_l.y_gm - projlin.crd1_l.y_gm;
      tang.z_gm = projlin.crd2_l.z_gm - projlin.crd1_l.z_gm;

      /* The zaxis in the local system pc */

      if ( pc == NULL )
        {
        zaxis.x_gm = 0.0;
        zaxis.y_gm = 0.0;
        zaxis.z_gm = 1.0;
        }
      else
        {
        zaxis.x_gm = pc->g31;
        zaxis.y_gm = pc->g32;
        zaxis.z_gm = pc->g33;
        }

      /* The normalised direction vector of the offset */

      GEvector_product(&tang,&zaxis,&offdir);
      GEnormalise_vector3D(&offdir,&offdir);

      /* Compute the end point of the new line */

      pend.x_gm = pos->x_gm + len*offdir.x_gm;
      pend.y_gm = pos->y_gm + len*offdir.y_gm;
      pend.z_gm = pos->z_gm + len*offdir.z_gm;

      /* Create the new  line */

      if ( GE200(pos,&pend,plout) < 0 ) return(erpush("GE2083","GE202"));

      return(0);
 }

/********************************************************************/
