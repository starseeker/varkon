/*!******************************************************************/
/*  File: ge200.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE200() Create line with two points                             */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/
/*                                                                  */
/*  Gunnar Liden 1984-12-26                                         */
/*                                                                  */
/*  Revised 1985-07-08 Error system  B. Doverud                     */
/*          1986-05-11 extern short, B. Doverud                     */
/*          1999-01-05 Function prototypes, gei200 deleted G. Liden */
/*          1999-03-04 Rewritten, J.Kjellander                      */
/*                                                                  */
/********************************************************************/


/********************************************************************/
/*   Error messages and warnings                                    */
/*   -------------------------------------------------------------  */
/*                                                                  */
/*    GE2012 = No distance between input points ( < TOL2 )          */
/*                                                                  */
/********************************************************************/

      DBstatus GE200(
      DBVector *pp1,     /* Pointer to the start point of the line  */
      DBVector *pp2,     /* Pointer to the end   point of the line  */
      DBLine   *pli)     /* Pointer to the output line              */

 { 
      DBfloat  dist;        /* The distance between the input points*/
      DBVector dp;          /* The vector from p1 tp p2             */


      /* Check the distance between the input points      */

      dp.x_gm = pp1->x_gm - pp2->x_gm;
      dp.y_gm = pp1->y_gm - pp2->y_gm;
      dp.z_gm = pp1->z_gm - pp2->z_gm;

      dist = GEvector_length3D(&dp);

     /* Error if the distance is less than identical points        */

      if (ABS(dist) < TOL2 ) return(erpush("GE2012","GE200"));
 
      pli->crd1_l.x_gm = pp1->x_gm;
      pli->crd1_l.y_gm = pp1->y_gm;
      pli->crd1_l.z_gm = pp1->z_gm;

      pli->crd2_l.x_gm = pp2->x_gm;
      pli->crd2_l.y_gm = pp2->y_gm;
      pli->crd2_l.z_gm = pp2->z_gm;

      return(0);
 }

/********************************************************************/

