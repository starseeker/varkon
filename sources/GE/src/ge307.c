/*!******************************************************************/
/*  File: ge307.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE307() Creation of a fillet circle between two curves          */
/*          The equivalent for arc_fil in MBS                       */
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

      DBstatus GE307(
      DBAny  *pstr1,
      DBSeg  *pseg1,
      DBAny  *pstr2,
      DBSeg  *pseg2,
      DBfloat radius,
      short   nalt,
      DBTmat *pc,
      DBArc  *pa,
      DBSeg  *paseg,
      short   modtyp)

/*    Main entry for fillet arcs.
 *
 *      In: pstr1  => Pointer to entity 1
 *          pseg1  => Pointer to segments or NULL
 *          pstr2  => Pointer to entity 2
 *          pseg2  => Pointer to segments or NULL
 *          radius => Fillet radius
 *          nalt   => Requested corner 1-4
 *          pc     => Pointer to active coordinate system
 *          modtyp => 2 or 3 for 2D or 3D
 *
 *      Out: *pa    => The output arc fillet
 *           *paseg => Segments or NULL
 *
 *
 *      (C)microform ab 1985-08-11 G.Liden
 *
 *      1999-04-16 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBstatus status;
   DBetype  type1,type2;

   type1 = pstr1->hed_un.type;
   type2 = pstr2->hed_un.type;

/*
***Line/line fillet.
*/
   if( type1 == LINTYP && type2 == LINTYP) 
     {
     status = GE308(&pstr1->lin_un,&pstr2->lin_un,
                     radius,nalt,pc,pa,paseg,modtyp);
     if(status < 0 ) return(erpush("GE3333","GE307(GE308)"));
     }
/*
***Line/arc_curve fillet.
*/
   else if ( type1 == LINTYP  ||  type2 == LINTYP )
     {
     status = GE309(pstr1,pseg1,pstr2,pseg2,radius,nalt,pc,pa,paseg,modtyp);
     if(status < 0 ) return(erpush("GE3333","GE307(GE309)"));
     }
/*
***Arc_curve/arc_curve fillet.
*/
   else
     {
     status = GE352(pstr1,pseg1,pstr2,pseg2,radius,nalt,pc,pa,paseg,modtyp);
     if(status < 0 ) return(erpush("GE3333","GE307(GE352)"));
     }

   return(0);
 }

/********************************************************************/
