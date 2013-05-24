/**********************************************************************
*
*    getfang.c
*    =========
*
*    This file includes the following public functions:
*
*    GEtfang_to_basic()  Transform an angle from local to basic
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

/********************************************************************/

      DBstatus GEtfang_to_basic(
      DBfloat  vin,
      DBTmat  *pt,
      DBfloat *pvout)

/*      Transforms the input angle with t.
 *      (from local to basic)
 *
 *      In: vin = Angle.
 *          pt  = Pointer to transformation.
 *
 *      Out: *pvout = Transformed angle.
 *
 *      (C)microform ab 1985-08-22 J.Kjellander
 *
 *      1999-04-07 Rewritten, J.Kjellander
 *
 *******************************************************************!*/


{
   DBfloat fi;

/*
***No transformation if pc = NULL.
*/
   if( pt == NULL )
     {
     *pvout = vin;
     return(0);
     }
/*
***Compute angle between local X-axis and basic X-axis.
*/
   if ( pt->g11 == 0.0  &&  pt->g12 > 0.0 )
     {
     fi = 90.0;
     }
   else if ( pt->g11 == 0.0  &&  pt->g12 < 0.0 )
     {
     fi = 270.0;
     }
   else if ( pt->g11 == 0.0 ) fi = 0.0;
   else
     {
     fi = ATAN(pt->g12/pt->g11) * RDTODG;
     if ( pt->g11 < 0.0 ) fi = fi + 180.0;
     if ( fi < 0.0 ) fi = fi + 360.0;
     }
/*
***Transform input angle.
*/
   *pvout = vin + fi;
/*
***End.
*/
    return(0);
} 

/********************************************************************/
