/*!******************************************************************/
/*  File: ge352.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE352() Creation of a fillet circle between two arcs or curves  */
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

      DBstatus GE352(
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

/*    Arc_curve/arc_curve fillet.
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
 *      (C)microform ab 1985-08-25 G.Liden
 *
 *      1999-04-16 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  offs1;    /* The offset for the first  curve         */
   DBfloat  offs2;    /* The offset for the second curve         */
   DBVector origin;   /* The origin of the circle                */
   DBVector start;    /* Start point for the fillet circle       */
   DBVector end;      /* End   point for the fillet circle       */
   DBfloat  uc1;      /* The parameter value for the intersect   */
                      /* point for the line                      */
   DBfloat  uc2;      /* The parameter value for the intersect   */
                      /* point for the curve                     */

/*
***Choice of fillet
*/
   if ( GE354(radius,nalt,&offs1,&offs2) < 0 )
     return(erpush("GE3333","GE352(GE354)"));
/*
***Compute intersect in offset.
*/
   if ( GE732(pstr1,pseg1,offs1,pstr2,pseg2,offs2,pc,
             &uc1,&uc2,&origin,&start,&end) < 0 )
     return(erpush("GE3333","GE352(GE732)"));
/*
***Create the fillet radius
*/
   if ( GE353(&origin,&start,&end,pc,pa,paseg,modtyp) < 0 )
       return(erpush("GE3333","GE352(GE353)"));

   return(SUCCED);
 }

/********************************************************************/
