/*****************************************************************************/
/*                                                                           */
/*  File: genurbs.c                                                          */
/*                                                                           */
/*  This file includes geometric routines for NURBS                          */
/*  Evaluation routines for NURBS can be found in: geevalnnc.c, sur890.c     */
/*                                                                           */
/*  GEsegcpts() - calculates control pts from cubic segment coefficients.    */
/*                                                                           */
/*                                                                           */
/*  This file is part of the VARKON Geometry Library.                        */
/*  URL:                                                                     */
/*                                                                           */
/*  This library is free software; you can redistribute it and/or            */
/*  modify it under the terms of the GNU Library General Public              */
/*  License as published by the Free Software Foundation; either             */
/*  version 2 of the License, or (at your option) any later                  */
/*  version.                                                                 */
/*                                                                           */
/*  This library is distributed in the hope that it will be                  */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied               */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                  */
/*  PURPOSE.  See the GNU Library General Public License for more            */
/*  details.                                                                 */
/*                                                                           */
/*  You should have received a copy of the GNU Library General               */
/*  Public License along with this library; if not, write to the             */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,                 */
/*  MA 02139, USA.                                                           */
/*                                                                           */
/*  (C) 2006-10-10 Sören Larsson, Örebro University                          */
/*                                                                           */
/*****************************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"



/*****************************************************************************/
/*                                                                           */
    DBstatus GEsegcpts(
    bool        skiplast,
    DBSeg       *p_seg,
    DBHvector   *p_cpts)

/*
 *  The function calculates homogeneus control points for 3:rd degree
 *  bezier/NURBS segment from cubic segment coefficients.
 *
 *
 *            skiplast   => If TRUE, do not calculate last pt
 *      In:  *p_seg      => Ptr to curve segment
 *
 *      Out: *p_cpts     => 3 or 4 control points
 *
 *
 *  (C) 2006-10-10 Sören Larsson, Örebro University
 *
 *****************************************************************************/

  {

/*
****  |p0x p1x p2x p3x|     |0   0   0   1|     |c3x  c2x  c1x  c0x|
****  |               |     |             |     |                  |
****  |p0y p1y p2y p3y|     |0   0  1/3  1|     |c3y  c2y  c1y  c0y|
****  |               |  =  |             |  X  |                  |
****  |p0z p1z p2z p3z|     |0  1/3 2/3  1|     |c3z  c2z  c1z  c0z|
****  |               |     |             |     |                  |
****  |p0w p1w p2w p3w|     |1   1   1   1|     |c3   c2   c1   c0 |
*/

  p_cpts[0].x_gm =                                              p_seg->c0x;
  p_cpts[0].y_gm =                                              p_seg->c0y;
  p_cpts[0].z_gm =                                              p_seg->c0z;
  p_cpts[0].w_gm =                                              p_seg->c0;

  p_cpts[1].x_gm =                             p_seg->c1x/3   + p_seg->c0x;
  p_cpts[1].y_gm =                             p_seg->c1y/3   + p_seg->c0y;
  p_cpts[1].z_gm =                             p_seg->c1z/3   + p_seg->c0z;
  p_cpts[1].w_gm =                             p_seg->c1/3    + p_seg->c0;

  p_cpts[2].x_gm =              p_seg->c2x/3 + p_seg->c1x*2/3 + p_seg->c0x;
  p_cpts[2].y_gm =              p_seg->c2y/3 + p_seg->c1y*2/3 + p_seg->c0y;
  p_cpts[2].z_gm =              p_seg->c2z/3 + p_seg->c1z*2/3 + p_seg->c0z;
  p_cpts[2].w_gm =              p_seg->c2/3  + p_seg->c1*2/3  + p_seg->c0;

  /*if (skiplast!=TRUE)
    {*/


    p_cpts[3].x_gm = p_seg->c3x + p_seg->c2x   + p_seg->c1x     + p_seg->c0x;
    p_cpts[3].y_gm = p_seg->c3y + p_seg->c2y   + p_seg->c1y     + p_seg->c0y;
    p_cpts[3].z_gm = p_seg->c3z + p_seg->c2z   + p_seg->c1z     + p_seg->c0z;
    p_cpts[3].w_gm = p_seg->c3  + p_seg->c2    + p_seg->c1      + p_seg->c0;
 /*  }*/

  return(0);
  }


