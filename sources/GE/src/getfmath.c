/**********************************************************************
*
*    getfmath.c
*    ==========
*
*    This file includes the following public functions:
*
*    GEtform_mult()      Multiplication of two DBTmat's
*    GEtform_det()       Determinant of a DBTmat
*    GEtform_inv()       Inverse of a DBTmat
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://varkon.sourceforge.net
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

        DBstatus GEtform_mult(
        DBTmat   *pt1,
        DBTmat   *pt2,
        DBTmat   *pt3)

/*      Multiplies t1 with t2.
 *
 *      In: pt1 = Pointer to t1.
 *          pt2 = Pointer to t2.
 *
 *      Out: *pt3 = t1 X t2.
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {

   pt3->g11 = pt1->g11 * pt2->g11 + pt1->g12 * pt2->g21 +
              pt1->g13 * pt2->g31 + pt1->g14 * pt2->g41;

   pt3->g12 = pt1->g11 * pt2->g12 + pt1->g12 * pt2->g22 +
              pt1->g13 * pt2->g32 + pt1->g14 * pt2->g42;

   pt3->g13 = pt1->g11 * pt2->g13 + pt1->g12 * pt2->g23 +
              pt1->g13 * pt2->g33 + pt1->g14 * pt2->g43;

   pt3->g14 = pt1->g11 * pt2->g14 + pt1->g12 * pt2->g24 +
              pt1->g13 * pt2->g34 + pt1->g14 * pt2->g44;

   pt3->g21 = pt1->g21 * pt2->g11 + pt1->g22 * pt2->g21 +
              pt1->g23 * pt2->g31 + pt1->g24 * pt2->g41;

   pt3->g22 = pt1->g21 * pt2->g12 + pt1->g22 * pt2->g22 +
              pt1->g23 * pt2->g32 + pt1->g24 * pt2->g42;

   pt3->g23 = pt1->g21 * pt2->g13 + pt1->g22 * pt2->g23 +
              pt1->g23 * pt2->g33 + pt1->g24 * pt2->g43;

   pt3->g24 = pt1->g21 * pt2->g14 + pt1->g22 * pt2->g24 +
              pt1->g23 * pt2->g34 + pt1->g24 * pt2->g44;

   pt3->g31 = pt1->g31 * pt2->g11 + pt1->g32 * pt2->g21 +
              pt1->g33 * pt2->g31 + pt1->g34 * pt2->g41;

   pt3->g32 = pt1->g31 * pt2->g12 + pt1->g32 * pt2->g22 +
              pt1->g33 * pt2->g32 + pt1->g34 * pt2->g42;

   pt3->g33 = pt1->g31 * pt2->g13 + pt1->g32 * pt2->g23 +
              pt1->g33 * pt2->g33 + pt1->g34 * pt2->g43;

   pt3->g34 = pt1->g31 * pt2->g14 + pt1->g32 * pt2->g24 +
              pt1->g33 * pt2->g34 + pt1->g34 * pt2->g44;

   pt3->g41 = pt1->g44 * pt2->g41;
   pt3->g42 = pt1->g44 * pt2->g42;
   pt3->g43 = pt1->g44 * pt2->g43;
   pt3->g44 = pt1->g44 * pt2->g44;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtform_det(
        DBTmat   *pt,
        DBfloat  *pdet)

/*      Calculates the determinant of t.
 *
 *      In: pt = Pointer to t.
 *
 *      Out: *pdet = Determinant.
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dum1,dum2,dum3,dum4;


/* Determinant for element g11 */

   dum1 = pt->g22 * pt->g33 * pt->g44 +
          pt->g23 * pt->g34 * pt->g42 +
          pt->g24 * pt->g32 * pt->g43 -
          pt->g22 * pt->g34 * pt->g43 -
          pt->g23 * pt->g32 * pt->g44 -
          pt->g24 * pt->g33 * pt->g42;

   dum1 = pt->g11 * dum1;


/* Determinant for element g12 */

   dum2 = pt->g21 * pt->g33 * pt->g44 +
          pt->g23 * pt->g34 * pt->g41 +
          pt->g24 * pt->g31 * pt->g43 -
          pt->g21 * pt->g34 * pt->g43 -
          pt->g23 * pt->g31 * pt->g44 -
          pt->g24 * pt->g33 * pt->g41;

   dum2 = -pt->g12 * dum2;


/* Determinant for element g13 */

   dum3 = pt->g21 * pt->g32 * pt->g44 +
          pt->g22 * pt->g34 * pt->g41 +
          pt->g24 * pt->g31 * pt->g42 -
          pt->g21 * pt->g34 * pt->g42 -
          pt->g22 * pt->g31 * pt->g44 -
          pt->g24 * pt->g32 * pt->g41;

   dum3 = pt->g13 * dum3;


/* Determinant for element g14 */

   dum4 = pt->g22 * pt->g33 * pt->g41 +
          pt->g23 * pt->g31 * pt->g42 +
          pt->g21 * pt->g32 * pt->g43 -
          pt->g22 * pt->g31 * pt->g43 -
          pt->g23 * pt->g32 * pt->g41 -
          pt->g21 * pt->g33 * pt->g42;

   dum4 = -pt->g14 * dum4;

  *pdet  = dum1 + dum2 + dum3 + dum4;


   if ( ABS(*pdet) < TOL1 ) return(erpush("GE6144","GEtform_det"));

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtform_inv(
        DBTmat   *pt1,
        DBTmat   *pt2)

/*      Calculates the inverse of t.
 *
 *      In: pt1 = Pointer to t.
 *
 *      Out: *pt2 = t inverted.
 *
 *      (C)microform ab 1985-02-26, G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {

/* Rows and columns change place in the upper left 3x3 matrix */

   pt2->g11 = pt1->g11;
   pt2->g12 = pt1->g21;
   pt2->g13 = pt1->g31;

   pt2->g21 = pt1->g12;
   pt2->g22 = pt1->g22;
   pt2->g23 = pt1->g32;

   pt2->g31 = pt1->g13;
   pt2->g32 = pt1->g23;
   pt2->g33 = pt1->g33;

/* The bottom row 0 0 0 1  equal to the original matrix  */

   pt2->g41 = pt1->g41;
   pt2->g42 = pt1->g42;
   pt2->g43 = pt1->g43;
   pt2->g44 = pt1->g44;

/* Translation coefficients for the inverse with multiplication */
/* of the inverse (transposed) matrix and the original trans-   */
/* slation coefficients                                         */

   pt2->g14 = -(pt2->g11*pt1->g14 + 
                pt2->g12*pt1->g24 +
                pt2->g13*pt1->g34);

   pt2->g24 = -(pt2->g21*pt1->g14 + 
                pt2->g22*pt1->g24 +
                pt2->g23*pt1->g34);

   pt2->g34 = -(pt2->g31*pt1->g14 + 
                pt2->g32*pt1->g24 +
                pt2->g33*pt1->g34);

   return(0);
  }

/********************************************************/
