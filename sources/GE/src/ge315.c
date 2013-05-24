/**********************************************************************
*
*    ge315.c
*    =======
*
*    This file includes:
*
*    GE315() Calculate parametric value for coordinate on 2D circle
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

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

        DBstatus GE315(
        DBArc   *arcp,
        DBfloat  dx,
        DBfloat  dy,
        DBfloat *u)

/*      Beräknar för visst dx,dy motsvarande parametervärde
 *      på en 2D-cirkel.
 *
 *      In: arcp   => Pekare till cirkel-structure.
 *          dx     => Delta x
 *          dy     => Delta y
 *          u      => Pekare till parametervärde
 *
 *      Ut: *u     => Parametervärde
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab  14/9/85 J. Kjellander
 *
 *      19/4/86  fi < 0, J. Kjellander
 *
 ******************************************************!*/

  {
     DBfloat fi;

/*
***Calculate angle fi, given by dx,dy.
*/
     if ( dx == 0.0 )
       {
       if ( dy >= 0.0 )
         {
         fi = 90.0;
         }
       else
         {
         if ( arcp->v1_a >= 0.0 ) fi = 270.0;
         else fi = -90;
         }
       }
     else
       {
       fi = ATAN(dy/dx) * RDTODG;          /* Grader ! */
       if ( dx < 0.0 ) fi = fi + 180.0;
       if ( fi < 0.0 && arcp->v1_a >= 0.0) fi = fi + 360.0;
       }
/*
***Normalize fi.
*/
     if ( fi+360.0 < arcp->v2_a ) fi += 360.0;
     if ( fi-360.0 > arcp->v1_a ) fi -= 360.0;
/*
***Calculate corresponding u-value.
*/
     *u = 1.0 + (fi - arcp->v1_a) / (arcp->v2_a - arcp->v1_a);

     return(0);
  }

/********************************************************/
