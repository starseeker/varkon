/*!******************************************************************/
/*  File: ge135.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE135() Extrapolate a rational cubic segment                    */
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

      DBstatus GE135(
      DBSeg   *prin,
      DBfloat uextr[],
      DBSeg   *prout)

/*    The function extrapolates a rational cubic segment to given
 *    parameter values. Note that the direction of a curve segment
 *    can be changed with this function.
 *
 *    In: prin  = Original segment
 *        uextr = [0] New start parameter value
 *                [1] New end parameter value
 *
 *    Out: *prout = Reparametrized segment
 *
 *    (C)microform ab 1992-01-16 G.Liden
 *
 *    1993-03-05 Typ av segment, J. Kjellander
 *    1994-11-23 UV_CUB_SEGM's G. Liden
 *    1999-05-21 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat u0;   /* Start parameter value = uextr[0] */
   DBfloat u1;   /* End   parameter value = uextr[1] */
   DBfloat u02;  /* u0 * u0                          */
   DBfloat u03;  /* u0 * u0 * u0                     */
   DBfloat u10;  /* u1 - u0                          */
   DBfloat u102; /* (u1 - u0)*(u1 -u0)               */
   DBfloat u103; /* (u1 - u0)*(u1 -u0)*(u1 - u0)     */

/*
***Check that parameter interval !u1-u0! > TOL4 (too small ?)
*/
   u0 = uextr[0]; 
   u1 = uextr[1];
   if ( ABS(u1-u0) < TOL4) return(erpush("GE1253","GE135"));
/*
***Compute some repeatedly used values.
*/
   u02  = u0*u0;
   u03  = u02*u0;
   u10  = u1 - u0;
   u102 = u10*u10;
   u103 = u102*u10;
/*
***Linear reparameterization
*/
   prout->c0x = prin->c0x     + 
                prin->c1x*u0  + 
                prin->c2x*u02 +
                prin->c3x*u03;

   prout->c0y = prin->c0y     + 
                prin->c1y*u0  + 
                prin->c2y*u02 +
                prin->c3y*u03;

   prout->c0z = prin->c0z     + 
                prin->c1z*u0  + 
                prin->c2z*u02 +
                prin->c3z*u03;

   prout->c0  = prin->c0      + 
                prin->c1 *u0  + 
                prin->c2 *u02 +
                prin->c3 *u03;

   prout->c1x = prin->c1x*        u10 + 
                prin->c2x*2.0*u0* u10 +
                prin->c3x*3.0*u02*u10;

   prout->c1y = prin->c1y*        u10 + 
                prin->c2y*2.0*u0* u10 +
                prin->c3y*3.0*u02*u10;

   prout->c1z = prin->c1z*        u10 + 
                prin->c2z*2.0*u0* u10 +
                prin->c3z*3.0*u02*u10;

   prout->c1  = prin->c1 *        u10 + 
                prin->c2 *2.0*u0* u10 +
                prin->c3 *3.0*u02*u10;

   prout->c2x = prin->c2x*       u102 + 
                prin->c3x*3.0*u0*u102; 

   prout->c2y = prin->c2y*       u102 + 
                prin->c3y*3.0*u0*u102; 

   prout->c2z = prin->c2z*       u102 + 
                prin->c3z*3.0*u0*u102; 

   prout->c2  = prin->c2 *       u102 + 
                prin->c3 *3.0*u0*u102; 

   prout->c3x = prin->c3x*u103; 

   prout->c3y = prin->c3y*u103; 

   prout->c3z = prin->c3z*u103; 

   prout->c3  = prin->c3 *u103; 
/*
***Output offset is equal to input offset
*/
   prout->ofs = prin->ofs;
/*
***Output type is equal to input type
*/
   prout->typ = prin->typ;
/*
***Output surface data equal to input surface data
*/
   if ( prin->typ == UV_CUB_SEG )
     {
     prout->subtyp   = prin->subtyp;
     prout->spek_gm  = prin->spek_gm;
     prout->spek2_gm = prin->spek2_gm;
     }

   return(0);
 }

/********************************************************************/
