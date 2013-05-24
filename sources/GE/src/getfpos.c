/**********************************************************************
*
*    getfpos.c
*    =========
*
*    This file includes the following public functions:
*
*    GEtfpos_to_local()  Transform position from basic to local
*    GEtfpos_to_basic()  Transform position from local to basic
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

        DBstatus  GEtfpos_to_local(
        DBVector *pin,
        DBTmat   *pt,
        DBVector *pout)

/*      Transforms the input position with t.
 *      (from basic to local)
 *
 *      In: pin = Pointer to position.
 *          pt  = Pointer to transformation.
 *
 *      Out: *pout = Transformed position.
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat x,y,z;

   if ( pt == NULL )
     {
     if ( pin != pout )
      {
      pout->x_gm = pin->x_gm;
      pout->y_gm = pin->y_gm;
      pout->z_gm = pin->z_gm;
      }
    return(0);
    }

   x = (pt->g11 * pin->x_gm + pt->g12 * pin->y_gm +
        pt->g13 * pin->z_gm + pt->g14) / pt->g44;

   y = (pt->g21 * pin->x_gm + pt->g22 * pin->y_gm +
        pt->g23 * pin->z_gm + pt->g24) / pt->g44;

   z = (pt->g31 * pin->x_gm + pt->g32 * pin->y_gm +
        pt->g33 * pin->z_gm + pt->g34) / pt->g44;

   pout->x_gm = x;
   pout->y_gm = y;
   pout->z_gm = z;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfpos_to_basic(
        DBVector *pin,
        DBTmat   *pt,
        DBVector *pout)

/*      Transforms the input position with t inverted.
 *      (from local to basic)
 *
 *      In: pin = Pointer to position.
 *          pt  = Pointer to transformation.
 *
 *      Out: *pout = Transformed position..
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat x,y,z;
   DBTmat  ti;

   if ( pt == NULL )
     {
     if ( pin != pout )
      {
      pout->x_gm = pin->x_gm;
      pout->y_gm = pin->y_gm;
      pout->z_gm = pin->z_gm;
      }
    return(0);
    }

   GEtform_inv(pt,&ti);

   x = (ti.g11 * pin->x_gm + ti.g12 * pin->y_gm +
        ti.g13 * pin->z_gm + ti.g14) / ti.g44;

   y = (ti.g21 * pin->x_gm + ti.g22 * pin->y_gm +
        ti.g23 * pin->z_gm + ti.g24) / ti.g44;

   z = (ti.g31 * pin->x_gm + ti.g32 * pin->y_gm +
        ti.g33 * pin->z_gm + ti.g34) / ti.g44;

   pout->x_gm = x;
   pout->y_gm = y;
   pout->z_gm = z;

   return(0);
  }

/********************************************************/
