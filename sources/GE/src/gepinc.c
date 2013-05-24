/**********************************************************************
*
*    gepinc.c
*    =========
*
*    This file includes the following public functions:
*
*    GEpos_in_cone()  Check if positions are inside a cone
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
*    (C)Johan Kjellander, Örebro university
*
*
***********************************************************************/

/*
***Include files.
*/
#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

       short GEpos_in_cone(
       DBVector *p1,
       DBVector *p2,
       DBfloat   r1,
       DBfloat   r2,
       DBint     npos,
       DBVector *pos,
       DBint    *pst,
       bool     *any)

/*      Checks if one or more positions lie inside
 *      the body of a cone (or not).
 *
 *
 *      In: p1     => Ptr to cone start
 *          p2     => Ptr to cone end
 *          r1     => Start radius.
 *          r2     => End radius
 *          npos   => Number of positions
 *          pos    => Ptr to position coordinates
 *          pst    => Ptr to result or NULL
 *          any    => Ptr to result
 *
 *      Out: *pst  => Status for all positions
 *           *any  => TRUE if one or more inside
 *
 *      Return: 0      => Ok.
 *              GE8322 => P1 = P2
 *
 *      (C)2005-08-04 J. Kjellander, Örebro university
 *         2006-01-29 Bug fix Sören Larsson
 *                    (all points did not returne a value) 
 *         2007-11-02 Removed compiler warnings, J.Kjellander
 *
 ******************************************************!*/

  {
   DBstatus status;
   DBint    i;
   DBfloat  x,y,z,cl,pos_r2,cone_r2;
   DBVector cx;
   DBTmat   pmat;

/*
***Start with assuming that no position is inside.
*/
   *any = FALSE;
/*
***Calculate cone length (cl).
*/
   cx.x_gm = p2->x_gm - p1->x_gm;
   cx.y_gm = p2->y_gm - p1->y_gm;
   cx.z_gm = p2->z_gm - p1->z_gm;

   if ( (cl=GEvector_length3D(&cx)) < COMPTOL ) return(erpush("GE8322",""));
/*
***Create local coordinate system with p1 as origin and
***p2 as X-axis.
*/
   if ( (status=GEmktf_2p(p1,p2,NULL,&pmat)) < 0 ) return(status);
/*
***For each position, start with X-check. If inside X-limits,
***check for radius inside.
*/

   for ( i=0; i < npos; ++i )
     {
     x = (pmat.g11 * (pos+i)->x_gm + 
          pmat.g12 * (pos+i)->y_gm +
          pmat.g13 * (pos+i)->z_gm +
          pmat.g14)/pmat.g44;

     if ( x >= 0  &&  x <= cl )
       {
       y = (pmat.g21 * (pos+i)->x_gm + 
            pmat.g22 * (pos+i)->y_gm +
            pmat.g23 * (pos+i)->z_gm +
            pmat.g24)/pmat.g44;

       z = (pmat.g31 * (pos+i)->x_gm + 
            pmat.g32 * (pos+i)->y_gm +
            pmat.g33 * (pos+i)->z_gm +
            pmat.g34)/pmat.g44;

       pos_r2  = y*y + z*z;
       cone_r2 = r1 + x/cl*(r2-r1);
       cone_r2 *= cone_r2;

       if ( pos_r2 <= cone_r2 )
         {
        *any = TRUE;
         if ( pst == NULL )  return(0);
         else               *(pst+i) = 1;
         }
       else if ( pst != NULL ) *(pst+i) = 0;
       }
     else if ( pst != NULL ) *(pst+i) = 0; /* line added 2006-01-30 sl */
     }
    return(0);
  }

/********************************************************/
