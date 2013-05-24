/*!******************************************************************/
/*  File: ge205.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE205() Create line from point tangent to curve                 */
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
/*  Gunnar Liden 1985-07-23                                         */
/*                                                                  */
/*  Revised 1999-03-04 Rewritten, J.Kjellander                      */
/*                                                                  */
/********************************************************************/

      DBstatus GE205(
      DBAny     *pstr,   /* Pointer to arc/curve                    */
      DBSeg     *pseg,   /* Pointer to arc/curve segments           */
      DBVector  *pos,    /* Pointer to start position               */
      DBTmat    *pc,     /* Pointer current coordinatesystem        */
      DBshort    alt,    /* Which solution ?                        */
      DBLine    *pli)    /* Pointer to the output line              */

 { 
      DBVector proj1,proj2;
      DBfloat  uout;


      /* Project the input point                           */
      /* ( Transform to the local system and let z=0 )     */

      GEtfpos_to_local(pos,pc,&proj1);
      proj1.z_gm = 0.0;
      GEtfpos_to_basic(&proj1,pc,&proj1);

      /* Computation of the tangent point                      */

      if ( GE402(pstr,pseg,pos,pc,alt,&uout,&proj2) < 0 )
        return(erpush("GE2172","GE205"));

      /* Create the projected line in basic coordinates        */

      if ( GE200(&proj1,&proj2,pli) < 0 ) return(erpush("GE2162","GE205"));

      return(0);
 }

/********************************************************************/
