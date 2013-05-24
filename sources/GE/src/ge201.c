/*!******************************************************************/
/*  File: ge201.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE201() Create line with two points. Project the line to        */
/*          the input coordinate system XY-plane.                   */
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
/*  Revision history:                                               */
/*                                                                  */
/*                                                                  */
/*  Gunnar Liden 1984-12-26                                         */
/*                                                                  */
/*  Revised 1985-01-21                                              */
/*  Revised 1985-01-29 No check of pointers                         */
/*  Revised 1985-05-04                                              */
/*          1985-07-08 Error system  B. Doverud                     */
/*          1986-05-07 extern, B. Doverud                           */
/*          1999-03-04 Rewritten, J.Kjellander                      */
/*                                                                  */
/********************************************************************/

/********************************************************************/
/*   Error messages and warnings                                    */
/*   -------------------------------------------------------------  */
/*                                                                  */
/*    GE2032 = No distance between points after projection          */
/*                                                                  */
/********************************************************************/

      DBstatus GE201(
      DBVector *pp1,     /* Pointer to the start point of the line  */
      DBVector *pp2,     /* Pointer to the end   point of the line  */
      DBTmat   *pc,      /* Pointer to the coordinate system        */
      DBLine   *pli)     /* Pointer to the output line              */

 { 
      DBVector proj1;    /* The projected  start point of the line  */
      DBVector proj2;    /* The projected  end   point of the line  */
      DBLine   projli;   /* The projected line in local coordinates */

      /* Project the input points                          */
      /* ( Transform to the local system and let z=0 )     */

      if ( pc == NULL )
        {
        proj1.x_gm = (*pp1).x_gm;
        proj1.y_gm = (*pp1).y_gm;
        proj2.x_gm = (*pp2).x_gm;
        proj2.y_gm = (*pp2).y_gm;
        }
      else
        {
        GEtfpos_to_local(pp1,pc,&proj1);
        GEtfpos_to_local(pp2,pc,&proj2);
        }

      proj1.z_gm = 0.0;
      proj2.z_gm = 0.0;

      /* Create the projected line in local coordinates        */
      /* and check the distance between the projected points   */

      if ( GE200(&proj1,&proj2,&projli) < 0 )
        return(erpush("GE2032","GE201"));

      /* Transform the line back to basic coordinates          */

      GEtfpos_to_basic(&projli.crd1_l,pc,&pli->crd1_l);
      GEtfpos_to_basic(&projli.crd2_l,pc,&pli->crd2_l);

      return(0);
 }

/********************************************************************/
