/*!******************************************************************/
/*  File: ge308.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE308() Creation of a fillet circle between two lines           */
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

      DBstatus GE308(
      DBLine *plin1,
      DBLine *plin2,
      DBfloat radius,
      short   nalt,
      DBTmat *pc,
      DBArc  *pa,
      DBSeg  *paseg,
      short   modtyp)

/*    Line/line fillet.
 *
 *      In: plin1  => Pointer to line 1
 *          plin2  => Pointer to line 2
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
   DBstatus status;     /* Status of called routines               */
   DBfloat  offs1;      /* The offset to the first line            */
   DBfloat  offs2;      /* The offset to the second line           */
   DBLine   lioff1;     /* Offset line 1                           */
   DBLine   lioff2;     /* Offset line 2                           */
   DBfloat  u1;         /* The parameter value for the intersect   */
                        /* point for line 1                        */
   DBfloat  u2;         /* The parameter value for the intersect   */
                        /* point for line 2                        */
   DBVector origin;     /* The origin of the circle (intersect pt) */
   DBVector start;      /* The start point of the circle           */
   DBVector end;        /* The end   point of the circle           */


/*
***Choice of fillet
*/
   status = GE354(radius,nalt,&offs1,&offs2);
   if ( status < 0 ) return(erpush("GE3333","GE308(GE354)"));
/*
***The intersect point between the two offsets lines where
***the offset is equal to the radius
*/
   status = GE730(plin1,offs1,plin2,offs2,pc,
                   &lioff1,&lioff2,&u1,&u2,&origin);
   if ( status < 0 ) return(erpush("GE3333","GE308(GE730)"));
/*
***Start and end points of the circle
*/
   start.x_gm = plin1->crd1_l.x_gm +
         u1*(plin1->crd2_l.x_gm - plin1->crd1_l.x_gm);
   start.y_gm = plin1->crd1_l.y_gm +
         u1*(plin1->crd2_l.y_gm - plin1->crd1_l.y_gm);
   start.z_gm = plin1->crd1_l.z_gm +
         u1*(plin1->crd2_l.z_gm - plin1->crd1_l.z_gm);

   end.x_gm = plin2->crd1_l.x_gm +
         u2*(plin2->crd2_l.x_gm - plin2->crd1_l.x_gm);
   end.y_gm = plin2->crd1_l.y_gm +
         u2*(plin2->crd2_l.y_gm - plin2->crd1_l.y_gm);
   end.z_gm = plin2->crd1_l.z_gm +
         u2*(plin2->crd2_l.z_gm - plin2->crd1_l.z_gm);
/*
***Create the circle
*/
   if ( GE353(&origin,&start,&end,pc,pa,paseg,modtyp) < 0 )
      return(erpush("GE3333","GE308(GE353)"));

   return(0);
 }

/********************************************************************/
