/**********************************************************************
*
*    gevector.c
*    ==========
*
*    This file includes the following public functions:
*
*    GEvector_length2D()      The lenght of a 2D vector
*    GEvector_length3D()      The lenght of a 3D vector
*
*    GEnormalise_vector2D()   Normalizes a 2D vector
*    GEnormalise_vector3D()   Normalizes a 3D vector
*
*    GEscalar_product2D()     2D scalar product
*    GEscalar_product3D()     3D scalar product
*
*    GEvector_product()       3D vector product
*
*    GEtriscl_product()       3D triple scalarproduct
*
*    GEvector_angler2D()      Rad angle between 2D vectors
*    GEvector_angler3D()      Rad angle between 3D vectors
*    GEvector_angled2D()      Deg angle between 2D vectors
*    GEvector_angled3D()      Deg angle between 3D vectors
*    GExy_angled()            True deg angle of (X,Y)
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

        DBfloat  GEvector_length2D(DBVector *pv)

/*      Returns the 2D length of a vector.
 *
 *      In: pv = Pointer to vector.
 *
 *      Return: The 2D lenght.
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   return(SQRT(pv->x_gm*pv->x_gm +
               pv->y_gm*pv->y_gm));
  }

/********************************************************/
/*!******************************************************/

        DBfloat GEvector_length3D(DBVector *pv)

/*      Returns the 3D length of a vector.
 *
 *      In: pv = Pointer to vector.
 *
 *      Return: The 3D lenght.
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   return(SQRT(pv->x_gm*pv->x_gm +
               pv->y_gm*pv->y_gm +
               pv->z_gm*pv->z_gm));
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEnormalise_vector2D(
        DBVector *pvin,
        DBVector *pvout)

/*      Creates a normalised 2D vector. If pvin
 *      is "infinitely" short negative status is
 *      returned and *pvout is set = *pvin.
 *
 *      In: pvin = Pointer to vector.
 *
 *      Out: *pvout = *pvin normalised
 *
 *      Return:  0 = OK.
 *              -1 = *pvin has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
    DBfloat d2,dist;

    d2 = pvin->x_gm*pvin->x_gm + pvin->y_gm*pvin->y_gm;

    if ( d2 < COMPTOL )
      {
      pvout->x_gm = pvin->x_gm;
      pvout->y_gm = pvin->y_gm;
      return(-1);
      }

    dist = SQRT(d2);
    pvout->x_gm = pvin->x_gm/dist;
    pvout->y_gm = pvin->y_gm/dist;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEnormalise_vector3D(
        DBVector *pvin,
        DBVector *pvout)

/*      Creates a normalised 3D vector. If pvin
 *      is "infinitely" short negative status is
 *      returned and *pvout is set = *pvin.
 *
 *      In: pvin = Pointer to vector.
 *
 *      Out: *pvout = *pvin normalised
 *
 *      Return:  0 = OK.
 *              -1 = *pvin has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
    DBfloat d2,dist;

    d2 = pvin->x_gm*pvin->x_gm +
         pvin->y_gm*pvin->y_gm +
         pvin->z_gm*pvin->z_gm;

    if ( d2 < COMPTOL )
      {
      pvout->x_gm = pvin->x_gm;
      pvout->y_gm = pvin->y_gm;
      pvout->z_gm = pvin->z_gm;
      return(-1);
      }

    dist = SQRT(d2);
    pvout->x_gm = pvin->x_gm/dist;
    pvout->y_gm = pvin->y_gm/dist;
    pvout->z_gm = pvin->z_gm/dist;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBfloat  GEscalar_product2D(
        DBVector *pv1,
        DBVector *pv2)

/*      Calculates the 2D scalar product of two vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Return:  The 2D scalar product.
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dot;

   dot = pv1->x_gm*pv2->x_gm +
         pv1->y_gm*pv2->y_gm;

   if ( ABS(dot) < COMPTOL ) dot = 0.0;

   return(dot);
  }

/********************************************************/
/*!******************************************************/

        DBfloat  GEscalar_product3D(
        DBVector *pv1,
        DBVector *pv2)

/*      Calculates the 3D scalar product of two vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Return:  The 3D scalar product.
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dot;

   dot = pv1->x_gm*pv2->x_gm +
         pv1->y_gm*pv2->y_gm +
         pv1->z_gm*pv2->z_gm;

   if ( ABS(dot) < COMPTOL ) dot = 0.0;

   return(dot);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEvector_product(
        DBVector *pv1,
        DBVector *pv2,
        DBVector *pvout)

/*      Calculates the 3D vector product of two vectors.
 *      Also known as the cross product.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Out *pvout = The 3D vector product.
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   pvout->x_gm = pv1->y_gm*pv2->z_gm - pv1->z_gm*pv2->y_gm;
   pvout->y_gm = pv1->z_gm*pv2->x_gm - pv1->x_gm*pv2->z_gm;
   pvout->z_gm = pv1->x_gm*pv2->y_gm - pv1->y_gm*pv2->x_gm;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtriscl_product(
        DBVector *pv1,
        DBVector *pv2,
        DBVector *pv3,
        DBfloat  *pout)

/*      Calculates the 3D triple scalar product of three
 *      vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *          pv3 = Pointer to vector 3.
 *
 *      Out: *pout = The triple product.
 *
 *      Return:  0 = OK.
 *              -1 = *pv1 or *pv2 or *pv3 has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Check that none of the input vectors have 
***zero length.
*/
  if ( pv1->x_gm*pv1->x_gm +
       pv1->y_gm*pv1->y_gm +
       pv1->z_gm*pv1->z_gm < COMPTOL ) return(-1);

  if ( pv2->x_gm*pv2->x_gm +
       pv2->y_gm*pv2->y_gm +
       pv2->z_gm*pv2->z_gm < COMPTOL ) return(-1);

  if ( pv3->x_gm*pv3->x_gm +
       pv3->y_gm*pv3->y_gm +
       pv3->z_gm*pv3->z_gm < COMPTOL ) return(-1);
/*
***Calculate triple scalar product.
*/
   *pout  = pv1->x_gm * (pv2->y_gm*pv3->z_gm - pv2->z_gm*pv3->y_gm) +
            pv1->y_gm * (pv2->z_gm*pv3->x_gm - pv2->x_gm*pv3->z_gm) +
            pv1->z_gm * (pv2->x_gm*pv3->y_gm - pv2->y_gm*pv3->x_gm);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEvector_angler2D(
        DBVector *pv1,
        DBVector *pv2,
        DBfloat  *pout)

/*      Calculates the angle (in radians) between 2
 *      2D vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Out: *pout = The angle.
 *
 *      Return:  0 = OK.
 *              -1 = *pv1 or *pv2 has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBfloat d2,dist,dot;

/*
***Normalise the input vectors.
*/
   d2 = pv1->x_gm*pv1->x_gm +
        pv1->y_gm*pv1->y_gm;

   if ( d2 < COMPTOL ) return(-1);

   dist = SQRT(d2);
   pv1->x_gm /= dist;
   pv1->y_gm /= dist;

   d2 = pv2->x_gm*pv2->x_gm +
        pv2->y_gm*pv2->y_gm;

   if ( d2 < COMPTOL ) return(-1);

   dist = SQRT(d2);
   pv2->x_gm /= dist;
   pv2->y_gm /= dist;
/*
***Calculate the scalar product.
*/
   dot = pv1->x_gm*pv2->x_gm +
         pv1->y_gm*pv2->y_gm;
/*
***Determine the angle.
*/
  if      ( dot >=  1.0 ) *pout = 0;
  else if ( dot <= -1.0 ) *pout = PI;
  else                    *pout = DACOS(dot);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEvector_angler3D(
        DBVector *pv1,
        DBVector *pv2,
        DBfloat  *pout)

/*      Calculates the angle (in radians) between 2
 *      3D vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Out: *pout = The angle.
 *
 *      Return:  0 = OK.
 *              -1 = *pv1 or *pv2 has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBfloat d2,dist,dot;

/*
***Normalise the input vectors.
*/
   d2 = pv1->x_gm*pv1->x_gm +
        pv1->y_gm*pv1->y_gm +
        pv1->z_gm*pv1->z_gm;

   if ( d2 < COMPTOL ) return(-1);

   dist = SQRT(d2);
   pv1->x_gm /= dist;
   pv1->y_gm /= dist;
   pv1->z_gm /= dist;

   d2 = pv2->x_gm*pv2->x_gm +
        pv2->y_gm*pv2->y_gm +
        pv2->z_gm*pv2->z_gm;

   if ( d2 < COMPTOL ) return(-1);

   dist = SQRT(d2);
   pv2->x_gm /= dist;
   pv2->y_gm /= dist;
   pv2->z_gm /= dist;
/*
***Calculate the scalar product.
*/
   dot = pv1->x_gm*pv2->x_gm +
         pv1->y_gm*pv2->y_gm +
         pv1->z_gm*pv2->z_gm;
/*
***Determine the angle.
*/
  if      ( dot >=  1.0 ) *pout = 0;
  else if ( dot <= -1.0 ) *pout = PI;
  else                    *pout = DACOS(dot);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEvector_angled2D(
        DBVector *pv1,
        DBVector *pv2,
        DBfloat  *pout)

/*      Calculates the angle (in degrees) between 2
 *      2D vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Out: *pout = The angle.
 *
 *      Return:  0 = OK.
 *              -1 = *pv1 or *pv2 has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBstatus status;

   status = GEvector_angler2D(pv1,pv2,pout);

   if ( status == 0 ) *pout *= RDTODG;

   return(status);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEvector_angled3D(
        DBVector *pv1,
        DBVector *pv2,
        DBfloat  *pout)

/*      Calculates the angle (in degrees) between 2
 *      3D vectors.
 *
 *      In: pv1 = Pointer to vector 1.
 *          pv2 = Pointer to vector 2.
 *
 *      Out: *pout = The angle.
 *
 *      Return:  0 = OK.
 *              -1 = *pv1 or *pv2 has zero length
 *
 *      (C)microform ab 1999-03-04, J. Kjellander
 *
 ******************************************************!*/

  {
   DBstatus status;

   status = GEvector_angler3D(pv1,pv2,pout);

   if ( status == 0 ) *pout *= RDTODG;

   return(status);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GExy_angled(
        DBfloat  xpos,
        DBfloat  ypos,
        DBfloat *angle)

/*      Calculates true angle (0-360 degrees) between
 *      the positive X-axis and a line from (0,0) to (X,Y).
 *
 *      In:  xpos  = X-position.
 *           ypos  = Y-position.
 *
 *      Out: *angle = True angle.
 *
 *      (C)microform ab 1999-03-04, J.Kjellander
 *
 ******************************************************!*/

  {

    if ( xpos == 0.0 && ypos > 0.0 ) *angle = 90.0;
    else if ( xpos == 0.0 ) *angle = -90.0;
    else
      {
      if ( xpos > 0.0 )
        {
        if ( ypos >= 0.0 )
          {
          *angle = RDTODG * ATAN(ypos/xpos);
          }
        else
          {
          *angle = RDTODG * (-ATAN(-ypos/xpos));
          }
        }
      else
        {
        if ( ypos >= 0.0 )
          {
          *angle = RDTODG*(PI - ATAN(ypos / -xpos));
          }
        else
          {
          *angle = RDTODG*(ATAN(-ypos / -xpos) - PI);
          }
        }
      }

    return(0);
  }

/*********************************************************/
