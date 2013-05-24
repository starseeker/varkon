/*!******************************************************************/
/*  File: ge309.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE309() Creation of a fillet circle between a line and a curve  */
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

      DBstatus GE309(
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

/*    Line/arc_curve fillet.
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
 *      16/12/92   DS90-fix, J. Kjellander
 *      22/2/93    Nytt anrop GEposition(), J. Kjellander
 *      16/6/93    Dynamiska segment, J. Kjellander
 *      25/4/94    Nytt anropt ill GE813(), J. Kjellander
 *      1999-04-16 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status,noint;
   DBfloat  offs1,offs2,offsl,offscu;
   DBLine  *plin,linofs;
   DBAny   *pcur,curofs;
   DBSeg   *pseg,*segofs;
   DBfloat  uout1[INTMAX],uout2[INTMAX],ul,uc;
   DBVector origin,start,end;

/*
***Select signs for the offsets.
*/
   if ( GE354(radius,nalt,&offs1,&offs2) < 0 )
     return(erpush("GE3333","GE309(GE354)"));
/*
***Who is who ?
*/
   if ( pstr1->hed_un.type == LINTYP )
     { plin  = &pstr1->lin_un; pcur = pstr2; pseg = pseg2;
       offsl = offs1; offscu = offs2; }
   else
     { plin  = &pstr2->lin_un; pcur = pstr1; pseg = pseg1;
       offsl = offs2; offscu = offs1; }
/*
***Create temporary offset copies of original entities.
*/
    linofs.hed_l.type = LINTYP;
    if ( GE202(plin,offsl,pc,&linofs) < 0 )
      return(erpush("GE7133","GE309(GE202)"));

    if ( pcur->hed_un.type == ARCTYP )
      {
      curofs.arc_un.hed_a.type = ARCTYP;
      segofs = DBcreate_segments((int)4);
      if ( GE306((DBArc *)pcur,pseg,offscu,pc,(DBArc *)&curofs,segofs) < 0 )
        {
        status = erpush("GE7133","GE309(GE306)");
        goto error;
        }
      }
    else
      {
      segofs = DBcreate_segments(pcur->cur_un.ns_cu);
      if ( GE813(&pcur->cur_un,pseg,pseg,pc,offscu,
                              &curofs.cur_un,segofs,segofs) < 0 )
        {
        status = erpush("GE7133","GE309(GE813)");
        goto error;
        }
      }
/*
***Compute the intersect.
*/
   noint = 1;
   if ( GE710((DBAny *)&linofs,pseg,&curofs,segofs,pc,&noint,uout1,uout2) < 0 )
     {
     status = erpush("GE7133","GE309(GE710)");
     goto error;
     }
   if ( noint < 1 )
     {
     status = erpush("GE7133","GE309:noint<1");
     goto error;
     }
/*
***Select solution no: 1.
*/
   ul = uout1[0] - 1.0;
   uc = uout2[0] - 1.0;
/*
***Calculate coordinates.
*/
   if ( GEposition((DBAny *)&linofs,(char *)pseg,ul,(gmflt)0.0,&origin) < 0 )
     {
     status = erpush("GE7133","GE309(GEposition)");
     goto error;
     }

   if ( GEposition((DBAny *)plin,(char *)pseg,ul,(gmflt)0.0,&start) < 0 )
     {
     status = erpush("GE7133","GE309(GEposition)");
     goto error;
     }

   if ( GEposition((DBAny *)pcur,(char *)pseg,uc,(gmflt)0.0,&end) < 0 )
     {
     status = erpush("GE7133","GE309(GEposition)");
     goto error;
     }
/*
***Create the fillet arc.
*/
   if ( GE353(&origin,&start,&end,pc,pa,paseg,modtyp) < 0 )
     {
     status = erpush("GE3333","GE309(GE353)");
     goto error;
     }
   else status = 0;
/*
***Felutgång.
*/
error:
   DBfree_segments(segofs);

   return(status);
 }

/********************************************************************/
