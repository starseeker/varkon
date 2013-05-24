/*!******************************************************************/
/*  File: ge141.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE141() From coefficients to the Consurf polygon                */
/*  GE142() Determine General Conic Plane for a rational cubic      */
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

      DBstatus GE141(
      DBSeg  *pr,
      DBfloat conp[])

/*    The function calculates the Consurf polygon points for
 *    given rational cubic segment coefficients.
 *
 *    Mer att fundera på när det gäller halvcirklar !!!
 *    Lösningen nedan duger bra för att bestämma Consurf plan
 *    Men för annan användning måste man tänka till !!!
 *
 *    In:  pr = The segment
 *
 *    Out: *conp = The Consurf polygon
 *                 conp( 1- 4)  = Start point
 *                 conp( 5- 8)  = Start tangent point
 *                 conp( 9-12)  = End   tangent point
 *                 conp(13-16)  = End   point
 *
 *   (C)microform ab 1991-11-29 G. Liden
 *
 *   1999-05-23 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  uextr[2];   /* Extrapolation parameter values          */
                        /* uextrÄ0Å = u0   Start                   */ 
                        /* uextrÄ1Å = u1   End                     */
   DBSeg    arcseg;     /* Coefficients for a half circle          */
   short    status;     /* Function value from a called function   */

/*
***Special treatment of 2-seg arcs.
*/
   if ( ABS(pr->c0 - 1.0) < TOL1 &&
        ABS(pr->c1 + 2.0) < TOL1 &&
        ABS(pr->c2 - 2.0) < TOL1     )
     {
/*
***Coefficients to the Consurf polygon for an 180 degree arc
*/
     uextr[0] = 0.01;
     uextr[1] = 0.99;

     status= GE135(pr,uextr,&arcseg);
     if ( status < 0 ) return(erpush("GE1253","GE135"));
/*
***P1 = C0
*/
     conp[0] = arcseg.c0x;
     conp[1] = arcseg.c0y;
     conp[2] = arcseg.c0z;
     conp[3] = arcseg.c0 ;
/*
***P2 = (C1+2*C0)/2
*/
     conp[4] = (arcseg.c1x + 2.0*arcseg.c0x)/2.0;
     conp[5] = (arcseg.c1y + 2.0*arcseg.c0y)/2.0;
     conp[6] = (arcseg.c1z + 2.0*arcseg.c0z)/2.0;
     conp[7] = (arcseg.c1  + 2.0*arcseg.c0 )/2.0;
/*
***P3 = (-C3 + C1 + 2*C0 )/2
*/
     conp[ 8] = (-arcseg.c3x + arcseg.c1x + 2.0*arcseg.c0x)/2.0 ;
     conp[ 9] = (-arcseg.c3y + arcseg.c1y + 2.0*arcseg.c0y)/2.0 ;
     conp[10] = (-arcseg.c3z + arcseg.c1z + 2.0*arcseg.c0z)/2.0 ;
     conp[11] = (-arcseg.c3  + arcseg.c1  + 2.0*arcseg.c0 )/2.0 ;
/*
***P4 = C3 + C2 + C1 + C0
*/
     conp[12] = arcseg.c3x + arcseg.c2x + arcseg.c1x + arcseg.c0x ;
     conp[13] = arcseg.c3y + arcseg.c2y + arcseg.c1y + arcseg.c0y ;
     conp[14] = arcseg.c3z + arcseg.c2z + arcseg.c1z + arcseg.c0z ;
     conp[15] = arcseg.c3  + arcseg.c2  + arcseg.c1  + arcseg.c0  ;
     }
/*
***"Normal" segment.
***Coefficients to the Consurf polygon
***P1 = C0
*/
   else
     {
     conp[0] = pr->c0x;
     conp[1] = pr->c0y;
     conp[2] = pr->c0z;
     conp[3] = pr->c0 ;
/*
***P2 = (C1+2*C0)/2
*/
     conp[4] = (pr->c1x + 2.0*pr->c0x)/2.0;
     conp[5] = (pr->c1y + 2.0*pr->c0y)/2.0;
     conp[6] = (pr->c1z + 2.0*pr->c0z)/2.0;
     conp[7] = (pr->c1  + 2.0*pr->c0 )/2.0;
/*
***P3 = (-C3 + C1 + 2*C0 )/2
*/
     conp[ 8] = (-pr->c3x + pr->c1x + 2.0*pr->c0x)/2.0 ;
     conp[ 9] = (-pr->c3y + pr->c1y + 2.0*pr->c0y)/2.0 ;
     conp[10] = (-pr->c3z + pr->c1z + 2.0*pr->c0z)/2.0 ;
     conp[11] = (-pr->c3  + pr->c1  + 2.0*pr->c0 )/2.0 ;
/*
***P4 = C3 + C2 + C1 + C0
*/
     conp[12] = pr->c3x + pr->c2x + pr->c1x + pr->c0x ;
     conp[13] = pr->c3y + pr->c2y + pr->c1y + pr->c0y ;
     conp[14] = pr->c3z + pr->c2z + pr->c1z + pr->c0z ;
     conp[15] = pr->c3  + pr->c2  + pr->c1  + pr->c0  ;
     }

   return(0);
 }
/********************************************************************/
/********************************************************************/

      DBstatus GE142(
      DBSeg    *pr,
      DBVector *ppt3,
      DBTmat   *pc)

/*    The function determines the Consurf General Conic Plane.
 *    Viewing Ball's Generalized Conic in the plane normal direction
 *    will show a "true" conic.
 *
 *    In:  pr = The segment
 *
 *    Out: *ppt = The third plane point (hard point)
 *         *pc  = Transformation matrix for the local
                  coordinate system, where the xy plane
                  is the Generalized Conic plane. The
                  xaxis is the chord of the conic.
 *
 *   (C)microform ab 1991-11-18 G. Liden
 *
 *   1999-05-23 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;    /* Function value from a called function  */
   DBfloat  conp[16];  /* The Consurf polygon                    */
   DBfloat  p1[3];     /* Point in the Consurf polygon           */
   DBfloat  p2[3];     /* Point in the Consurf polygon           */
   DBfloat  p3[3];     /* Point in the Consurf polygon           */
   DBfloat  p4[3];     /* Point in the Consurf polygon           */
   DBVector origin;    /* Pointer to the point , which shall be  */
                       /* the origin in the new coord. system    */
   DBVector vecx;      /* Pointer to the vector, which shall be  */
                       /* the xaxis                              */
   DBVector vecy;      /* Pointer to the vector, which shall be  */
                       /* close to the yaxis                     */

/*
***Calculate the three points which define the plane
***Convert the coefficients to a Consurf polygon
*/
   status = GE141(pr,conp);
   if ( status < 0 ) return(erpush("GE1223","GE142"));
/*
***The homogenous points to cartesian points
*/
   if ( ABS(conp[3]) > TOL1 )
     {
     p1[0] = conp[0]/conp[3];
     p1[1] = conp[1]/conp[3];
     p1[2] = conp[2]/conp[3];
     }
   else return(erpush("GE1233","GE142"));

   if ( ABS(conp[7]) > TOL1 )
     {
     p2[0] = conp[4]/conp[7];
     p2[1] = conp[5]/conp[7];
     p2[2] = conp[6]/conp[7];
     }
   else return(erpush("GE1233","GE142"));
 
   if ( ABS(conp[11]) > TOL1 )
     {
     p3[0] = conp[ 8]/conp[11];
     p3[1] = conp[ 9]/conp[11];
     p3[2] = conp[10]/conp[11];
     }
   else return(erpush("GE1233","GE142"));
 
   if ( ABS(conp[15]) > TOL1 )
     {
     p4[0] = conp[12]/conp[15];
     p4[1] = conp[13]/conp[15];
     p4[2] = conp[14]/conp[15];
     }
   else return(erpush("GE1233","GE142"));
/*
***Origin is the start point p1 of the segment
*/
   origin.x_gm = p1[0];
   origin.y_gm = p1[1];
   origin.z_gm = p1[2];
/*
***Vectors vecx=p4-p1 and vecy=(p2+0.5*(p3-p2)) - p1
*/
   vecx.x_gm = p4[0] - p1[0];
   vecx.y_gm = p4[1] - p1[1];
   vecx.z_gm = p4[2] - p1[2];

   vecy.x_gm = (p2[0]+0.5*(p3[0]-p2[0])) - p1[0];
   vecy.y_gm = (p2[1]+0.5*(p3[1]-p2[1])) - p1[1];
   vecy.z_gm = (p2[2]+0.5*(p3[2]-p2[2])) - p1[2];
/*
***The third point p2+0.5*(p3-p2)
*/
   ppt3->x_gm = p2[0]+0.5*(p3[0]-p2[0]);
   ppt3->y_gm = p2[1]+0.5*(p3[1]-p2[1]);
   ppt3->z_gm = p2[2]+0.5*(p3[2]-p2[2]);
/*
***Output coordinate system (xy plane)
***Calculation of matrix. Call of GEmktf_3p
*/
    status = GEmktf_3p(&origin,&vecx,&vecy,pc);
    if ( status < 0 )
      {
/*
***Can be a rational cubic with vecx // vecy
***Try vecy = p2-p1
*/
      vecy.x_gm = p2[0] - p1[0];
      vecy.y_gm = p2[1] - p1[1];
      vecy.z_gm = p2[2] - p1[2];
      ppt3->x_gm = p2[0];
      ppt3->y_gm = p2[1];
      ppt3->z_gm = p2[2];

      status = GEmktf_3p(&origin,&vecx,&vecy,pc);
      if ( status < 0 )
        {
/*
***Try vecy = p3-p4
*/
        vecy.x_gm = p3[0] - p4[0];
        vecy.y_gm = p3[1] - p4[1];
        vecy.z_gm = p3[2] - p4[2];
        ppt3->x_gm = p3[0];
        ppt3->y_gm = p3[1];
        ppt3->z_gm = p3[2];

        status = GEmktf_3p(&origin,&vecx,&vecy,pc);
        if ( status < 0 ) return(erpush("GE9183","GE142"));
        }
      }

   return(0);
 }

/********************************************************************/
