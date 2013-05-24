/*!******************************************************************/
/*  File: ge815.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE815() Create free conic curve                                 */
/*  GE816() Create projected conic curve                            */
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

      DBstatus GE815(
      DBVector *pv,
      DBVector *tv,
      DBVector *iv,
      DBfloat   pvalue[],
      short     npoi,
      DBCurve  *curpek,
      DBSeg    *segmnt)

/*    The function creates a rational cubic multi-segment curve
 *    from arrays of points, tangent definition points, intermediate
 *    points or p-values. The output rational segments will be Ball's
 *    generalised conics, which will be "true" conics if input
 *    points are in a plane and if tangents and the chord is a
 *    triangle.
 *
 *      In: pv     = Positions
 *          tv     = Tangents
 *          iv     = Intermediate positions
 *          pvalue = P-values. 0=Formela F1, -1=F2
 *          npoi   = Number of positions
 *
 *      Out: curpek = The curve created
 *           segmnt = Its segments
 *
 *      (C)microform ab 1991-12-10 G.Liden
 *
 *      5/3/93     CUB_SEG, J. Kjellander
 *      1996-03-19 pvalue= 0 Straight line, Debug Gunnar Liden
 *      1999-05-20 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    iseg;       /* Loop index segment in curve             */
   DBVector segpts[4];  /* Points defining one rational segment    */
                        /* 0: Segment start         point          */
                        /* 1: Segment start tangent point          */
                        /* 2: Segment end   tangent point          */
                        /* 3: Segment end           point          */
   short status;        /* Error code from a called function       */

/*
***Check number of points
*/
   if ( npoi < 2 ) return(erpush("GE8102","GE815"));
/*
***Number of segments in the whole curve
*/
   curpek->ns_cu = npoi - 1;
/*
***No planar curve
*/
   curpek->plank_cu = FALSE;
/*
***Data (coefficients) for all segments of the curve
***Loop for all segments in the curve, iseg=0,2,..,npoi-1
*/
   for ( iseg=0; iseg < npoi-1; iseg++ )
     {
/*
***Let segpts = points from input arrays
*/
     segpts[0].x_gm = (pv+iseg)->x_gm;
     segpts[0].y_gm = (pv+iseg)->y_gm;
     segpts[0].z_gm = (pv+iseg)->z_gm;

     segpts[1].x_gm = (tv+iseg)->x_gm;
     segpts[1].y_gm = (tv+iseg)->y_gm;
     segpts[1].z_gm = (tv+iseg)->z_gm;

     segpts[2].x_gm = (tv+iseg+1)->x_gm;
     segpts[2].y_gm = (tv+iseg+1)->y_gm;
     segpts[2].z_gm = (tv+iseg+1)->z_gm;

     segpts[3].x_gm = (pv+iseg+1)->x_gm;
     segpts[3].y_gm = (pv+iseg+1)->y_gm;
     segpts[3].z_gm = (pv+iseg+1)->z_gm;
/*
***Compute coefficients.
***For a p-value segment (0 < pvalue < 1). Call of GE133
*/
     if ( pvalue[iseg] > 0.000001 &&  pvalue[iseg] < 1.0 )
       {
       status = GE133(segpts,pvalue[iseg],&segmnt[iseg]);  
       if ( status < 0 ) return(status);  
       }
/*
***For a interm. point segment ( pvalue < 0 ). Call of GE136
*/
     else if (  pvalue[iseg] < -0.000001 )
       {
       status = GE136(segpts,&iv[iseg],&segmnt[iseg]);  
       if ( status < 0 ) return(status);  
       }
/*
***For a straight line pvalue= 0. Input tangent points are
***not used. There will be kinks in curve! Call of GE133
*/
     else if ( ABS(pvalue[iseg]) <=  0.000001 )
       {
       segpts[1].x_gm = segpts[3].x_gm;
       segpts[1].y_gm = segpts[3].y_gm;
       segpts[1].z_gm = segpts[3].z_gm;
       segpts[2].x_gm = segpts[0].x_gm;
       segpts[2].y_gm = segpts[0].y_gm;
       segpts[2].z_gm = segpts[0].z_gm;
       status = GE133(segpts, 0.5 ,&segmnt[iseg]);  
       if ( status < 0 ) return(status);  
       }
     else return(erpush("GE8993","GE815"));
     }

   return(0);

 }

/********************************************************************/
/********************************************************************/

      DBstatus GE816(
      DBVector *pv,
      DBVector *tv,
      DBVector *iv,
      DBfloat   pvalue[],
      short     npoi,
      DBTmat   *pcrd,
      DBCurve  *curpek,
      DBSeg    *segmnt)

/*    Basically the same as GE815() but positions, tangents
 *    and intermediate positions are first projected to the
 *    XY-plane of the coordinate system.
 *
 *      In: pv     = Positions
 *          tv     = Tangents
 *          iv     = Intermediate positions
 *          pvalue = P-values. 0=Formela F1, -1=F2
 *          npoi   = Number of positions
 *          pcrd   = Coordinate system
 *
 *      Out: curpek = The curve created
 *           segmnt = Its segments
 *
 *      (C)microform ab 1991-12-03 G.Liden
 *
 *      1999-05-20 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short ipoi;       /* Loop index point for curve              */
   short status;     /* Error code from a called function       */

/*
***Check number of points
*/
   if ( npoi < 2 ) return(erpush("GE8102","GE816"));
/*
***Projection of all points onto the XY plane of pcrd
***Loop for all points defining the curve, ipoi=1,2,..,npoi
*/
   for ( ipoi=0; ipoi < npoi; ipoi++ )
     {
/*
***Transform point pv, tv and iv to local system pcrd.
***Calls of function GEtfpos_to_local.
*/
     status = GEtfpos_to_local( &pv[ipoi], pcrd , &pv[ipoi] );
     if ( status < 0 ) return(status);  

     status = GEtfpos_to_local( &tv[ipoi], pcrd , &tv[ipoi] );
     if ( status < 0 ) return(status);  

     status = GEtfpos_to_local( &iv[ipoi], pcrd , &iv[ipoi] );
     if ( status < 0 ) return(status);  
/*
***Let the z coordinate be zero for the projected points
*/
     (pv+ipoi)->z_gm = (tv+ipoi)->z_gm = (iv+ipoi)->z_gm = 0.0;
/*
***Transform projected pts pv and tv back to basic system 
***Calls of function GEtfpos_to_basic.
*/
     status = GEtfpos_to_basic( &pv[ipoi], pcrd , &pv[ipoi] );
     if ( status < 0 ) return(status);  

     status = GEtfpos_to_basic( &tv[ipoi], pcrd , &tv[ipoi] );
     if ( status < 0 ) return(status);  

     status = GEtfpos_to_basic( &iv[ipoi], pcrd , &iv[ipoi] );
     if ( status < 0 ) return(status);  
     }
/*
***Create the projected output curve.
***Call of GE815. The output curve will be a multi-conic
***segment curve (if tangents and chord is a triangle) since
***all the points are in a plane.
*/
   status = GE815(pv,tv,iv,pvalue,npoi,curpek,segmnt);       
   if ( status < 0 ) return(status);  
/*
***Planar curve and transformation matrix to CUR..
*/
   curpek->plank_cu = TRUE;

   GE814(curpek,pcrd);

   return(0);

 }

/********************************************************************/
