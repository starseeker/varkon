/**********************************************************************
*
*    getfseg.c
*    =========
*
*    This file includes the following public functions:
*
*    GEtfseg_to_local()  Transform curve segment from basic to local
*    GEtfseg_to_basic()  Transform curve segment from local to basic
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

/*
***Include files.
*/
#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

        DBstatus GEtfseg_to_local(
        DBSeg  *ps1,
        DBTmat *pt,
        DBSeg  *ps2)

/*      Transforms the input segment with t.
 *      (from basic to local)
 *
 *      In: ps1 = Pointer to segment.
 *          pt  = Pointer to transformation.
 *
 *      Out: *ps2 = Transformed segment.
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBTmat  m1,m2;

/*
***No transformation if pt = NULL
*/
   if ( pt == NULL )
     {
     if ( ps1 != ps2 ) V3MOME(ps1,ps2,sizeof(DBSeg));
     return(0);
     }
/*
***Multiply old segment coefficients with t
*/
   m1.g11 = ps1->c0x;
   m1.g12 = ps1->c1x;
   m1.g13 = ps1->c2x;
   m1.g14 = ps1->c3x;

   m1.g21 = ps1->c0y;
   m1.g22 = ps1->c1y;
   m1.g23 = ps1->c2y;
   m1.g24 = ps1->c3y;

   m1.g31 = ps1->c0z;
   m1.g32 = ps1->c1z;
   m1.g33 = ps1->c2z;
   m1.g34 = ps1->c3z;

   m1.g41 = ps1->c0;
   m1.g42 = ps1->c1;
   m1.g43 = ps1->c2;
   m1.g44 = ps1->c3;

   GEtform_mult(pt,&m1,&m2);
/*
***Copy the result to the new segment
*/
   ps2->c0x = m2.g11;
   ps2->c1x = m2.g12;
   ps2->c2x = m2.g13;
   ps2->c3x = m2.g14;

   ps2->c0y = m2.g21;
   ps2->c1y = m2.g22;
   ps2->c2y = m2.g23;
   ps2->c3y = m2.g24;

   ps2->c0z = m2.g31;
   ps2->c1z = m2.g32;
   ps2->c2z = m2.g33;
   ps2->c3z = m2.g34;

   ps2->c0  = m2.g41;
   ps2->c1  = m2.g42;
   ps2->c2  = m2.g43;
   ps2->c3  = m2.g44;
/*
***Copy old segment specific data the new segment.
*/
   ps2->ofs = ps1->ofs;
   ps2->sl  = ps1->sl;
   ps2->typ = ps1->typ;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfseg_to_basic(
        DBSeg  *ps1,
        DBTmat *pt,
        DBSeg  *ps2)

/*      Transforms the input segment with t inverted.
 *      (from local to basic)
 *
 *      In: ps1 = Pointer to segment.
 *          pt  = Pointer to transformation.
 *
 *      Out: *ps2 = Transformed segment.
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBTmat ti;

/*
***No transformation if pt = NULL
*/
   if ( pt == NULL )
     {
     if ( ps1 != ps2 ) V3MOME(ps1,ps2,sizeof(DBSeg));
     return(0);
     }
/*
***Compute the inverse of t.
*/
   GEtform_inv(pt,&ti);
/*
***Transform.
*/
   return(GEtfseg_to_local(ps1,&ti,ps2));
  }

/********************************************************/
