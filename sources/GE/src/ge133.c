/*!******************************************************************/
/*  File: ge133.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE133() Creates a rational cubic segment from four              */
/*          points and a p-value.                                   */
/*  GE136() Create a Generalized Conic with intermediate point      */
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

#define KUBTOL 1E-9    /* Tolerance for CUBSEG classification */

static void mk_seg(DBfloat conp[], DBSeg *pr);

/********************************************************************/

      DBstatus GE133(
      DBVector *points,
      DBfloat   p,
      DBSeg    *pr)

/*    Cretaes a rational cubic (generalised) conic segment from
 *    points, tangents and P-value.
 *
 *      In:  *points = [0] = Start point
 *                     [1] = Start tangent point
 *                     [2] = End tangent point
 *                     [3] = End point
 *            p      = P-value
 *
 *      Out: *pr => The segment created
 *
 *      (C)microform ab 1984-12-29 G.Liden
 *
 *      1999-04-22 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  a_cos;        /* The angle cosine                        */
   DBfloat  w1,w2,w3,w4;  /* Weights for the points                  */
   DBfloat  cl;           /* Chord length                            */
   DBfloat  lambda,mu;    /* Distance along the tangent vectors      */
   DBVector t1,t2;        /* Normalised tangent vectors              */
   DBfloat  alpha;        /* Angle between chord and t1()            */
   DBfloat  beta;         /* Angle between chord and t2()            */
   DBfloat  conp[16];     /* The consurf polygon points for mk_seg() */
   DBVector chord;        /* A dummy vector                          */

/*
***Compute the normalised start tangent
*/
   t1.x_gm = (points+0)->x_gm - (points+1)->x_gm;
   t1.y_gm = (points+0)->y_gm - (points+1)->y_gm;
   t1.z_gm = (points+0)->z_gm - (points+1)->z_gm;
   GEnormalise_vector3D(&t1,&t1);
/*
***Compute the normalised end   tangent
*/
   t2.x_gm = (points+3)->x_gm - (points+2)->x_gm;
   t2.y_gm = (points+3)->y_gm - (points+2)->y_gm;
   t2.z_gm = (points+3)->z_gm - (points+2)->z_gm;
   GEnormalise_vector3D(&t2,&t2);
/*
***Angle between t1 and the chord. Note that chord = P1-P4
***Compute the chord length
*/
   chord.x_gm = points->x_gm - (points+3)->x_gm;
   chord.y_gm = points->y_gm - (points+3)->y_gm;
   chord.z_gm = points->z_gm - (points+3)->z_gm;

   cl = GEvector_length3D(&chord);

   GEnormalise_vector3D(&chord,&chord);

   a_cos = -GEscalar_product3D(&t1,&chord);

   if ( a_cos >  1.0 ) a_cos =  1.0;  /* 1994-11-13 G Liden */
   if ( a_cos < -1.0 ) a_cos = -1.0;

   if ( a_cos < 0.0 )
     {
     alpha = ACOS(-a_cos);
     alpha = PI - alpha;
     }
   if ( a_cos >= 0.0 )  alpha = ACOS(a_cos);
/*
***The direction of t1 must be changed
*/
   if ( alpha < -PI05  ||  alpha > PI05 )
     {
     t1.x_gm = -t1.x_gm;
     t1.y_gm = -t1.y_gm;
     t1.z_gm = -t1.z_gm;

     a_cos = -GEscalar_product3D(&t1,&chord);

     if ( a_cos >  1.0 ) a_cos =  1.0;  /* 1994-11-13 G Liden */
     if ( a_cos < -1.0 ) a_cos = -1.0;
     if ( a_cos < 0.0 )
       {
       alpha = ACOS(-a_cos);
       alpha = PI - alpha;
       }
     if ( a_cos >= 0.0 )  alpha = ACOS(a_cos);
     }
/*
***Angle between t2 and the chord. Note that chord = P1-P4
*/
   a_cos = GEscalar_product3D(&t2,&chord);

   if ( a_cos >  1.0 ) a_cos =  1.0;  /* 1994-11-13 G Liden */
   if ( a_cos < -1.0 ) a_cos = -1.0;
   if ( a_cos < 0.0 )
     {
     beta = ACOS(-a_cos);
     beta = PI - beta;
     }
   if ( a_cos >= 0.0 )  beta = ACOS(a_cos);
/*
***The direction of t2 must be changed
*/
   if ( beta < -PI05  ||  beta > PI05 )
     {
     t2.x_gm = -t2.x_gm;
     t2.y_gm = -t2.y_gm;
     t2.z_gm = -t2.z_gm;

     a_cos = GEscalar_product3D(&t2,&chord);

     if ( a_cos >  1.0 ) a_cos =  1.0;  /* 1994-11-13 G Liden */
     if ( a_cos < -1.0 ) a_cos = -1.0;
     if ( a_cos < 0.0 )
       {
       beta = ACOS(-a_cos);
       beta = PI - beta;
       }
     if ( a_cos >= 0.0 )  beta = ACOS(a_cos);
     }
/*
***alpha and beta = 90 degrees
*/
   if ( ABS(alpha+beta-PI) <= TOL3 ) return(erpush("GE1182","GE133"));
/*
***The weights for the points
*/
   w1 = 1.0/p - 1.0;
   w2 = 1.0;
   w3 = 1.0;
   w4 = 1.0/p - 1.0;
/*
***The Consurf polygon points
*/
   if( ABS(alpha+beta) < TOL3 )
     {
     /* A straight line                         */
     w1 = 1.0;
     w2 = 1.0;
     w3 = 1.0;
     w4 = 1.0;
     lambda = 0.5*cl;
     mu     = 0.5*cl;
     }
   else if( ABS(alpha)*RDTODG < TOL7*500.0 ||
            ABS(beta) *RDTODG < TOL7*500.0 )
     {
     lambda = 0.5*cl;
     mu     = 0.5*cl;
     }
   else
     {
     lambda = SIN(beta)*cl/SIN(alpha+beta);
     mu     = SIN(alpha)*cl/SIN(alpha+beta);
     }

   conp[0] = points->x_gm*w1;
   conp[1] = points->y_gm*w1;
   conp[2] = points->z_gm*w1;
   conp[3] = w1;

   conp[4] = (points->x_gm + t1.x_gm*lambda)*w2;
   conp[5] = (points->y_gm + t1.y_gm*lambda)*w2;
   conp[6] = (points->z_gm + t1.z_gm*lambda)*w2;
   conp[7] = w2;

   conp[8] =  ((points+3)->x_gm + t2.x_gm*mu)*w3;
   conp[9] =  ((points+3)->y_gm + t2.y_gm*mu)*w3;
   conp[10] = ((points+3)->z_gm + t2.z_gm*mu)*w3;
   conp[11] = w3;

   conp[12] = (points+3)->x_gm*w4;
   conp[13] = (points+3)->y_gm*w4;
   conp[14] = (points+3)->z_gm*w4;
   conp[15] = w4;
/*
***From the consurf polygon to coefficients
*/
   mk_seg(conp,pr);
/*
***Om 3:e-gradstermerna är tillräckligt små skall dom
***sättas till 0.0 så att andra rutiner kan använda detta
***för klassificering.
*/
   if ( ABS(pr->c3x) < KUBTOL ) pr->c3x = 0.0;
   if ( ABS(pr->c3y) < KUBTOL ) pr->c3y = 0.0;
   if ( ABS(pr->c3z) < KUBTOL ) pr->c3z = 0.0;
   if ( ABS(pr->c3)  < KUBTOL ) pr->c3  = 0.0;

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE136(
      DBVector *points,
      DBVector *pipt,
      DBSeg    *pr)

/*    The function creates a Generalized Conic segment with an
 *    intermediate point, which determines the p value. The inter-
 *    mediate point will be projected onto the Generalized Conic
 *    plane. The projected point determines the p-value, and the
 *    projected output curve segment will interpolate the projected
 *    point.
 *
 *    In: points  = Array of: [0] = Start point
 *                            [1] = Start tangent point
 *                            [2] = End tangent point
 *                            [3] = End point
 *        pipt    = Intermediate point
 *
 *    Out: *pr = The segment created
 *
 *    (C)microform ab 1991-11-19 G.Liden
 *
 *    1991-12-05 Handling of (nearly) straight lines, G.Liden
 *    1991-12-05 Error for P value > 0.85, Gunnar Liden
 *    1999-05-20 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from a called function  */
   DBTmat   gcsys;       /* Generalized Conic coordinate system    */
   DBfloat  pval;        /* The p value     0 < p < 1              */
   DBSeg    tseg;        /* Temporary rational segment for GE142   */
   DBVector projipt;     /* Projected intermediate point           */
   DBVector pt3;         /* Hard point in Gener. Conic Plane       */
   DBLine   chord;       /* Chord line when segment is a line      */
   DBfloat  u_line;      /* Parameter value for the line           */
   DBVector v_p;         /* Vector pipt to closest point on line   */

/*
***Projection plane for the intermediate point
***Create a rational segment (with p value equal to 0.5)
***Call of GE133
*/
   pval = 0.5;
   status = GE133(points,pval,&tseg);
   if (status<0) return(erpush("GE1263","GE136"));
/*
***Determine the Generalized Conic Plane coordinate system
***Call of GE142
*/
   status = GE142(&tseg,&pt3,&gcsys);
   if ( status < 0 ) 
     {
/*
***Straight line case is assumed
*/
      status = GE200(&points[0],&points[3],&chord);
      if ( status < 0 ) return(erpush("GE1263","GE136"));

      GE709(&chord,pipt,&u_line);  

      v_p.x_gm = points->x_gm + u_line*((points+3)->x_gm -
                 points->x_gm) - pipt->x_gm;
      v_p.y_gm = points->y_gm + u_line*((points+3)->y_gm -
                 points->y_gm) - pipt->y_gm;
      v_p.z_gm = points->z_gm + u_line*((points+3)->z_gm -
                 points->z_gm) - pipt->z_gm;

      if ( GEvector_length3D(&v_p) > TOL2) return(erpush("GE3383","GE136"));

      pval = 0.5;
      goto line;
      }
/*
***Project the interm. point onto the Generalized Conic plane
***Transform the intermediate point to local system gcsys
***Call of GEtfpos_to_local
*/
   status = GEtfpos_to_local(pipt,&gcsys,&projipt);
   if ( status < 0 ) return(erpush("GE3032","GE136"));
/*
***Project. Let z=0 for and transform back to basic.
***Call of GEtfpos_to_basic
*/
   projipt.z_gm = 0.0;

   status = GEtfpos_to_basic(&projipt,&gcsys,&projipt);
   if ( status < 0 ) return(erpush("GE3152","GE136"));
/*
***The output rational segment
***The p-value. Call of GE304.
*/
   status = GE304(&points[0],&pt3,&points[3],&projipt,&pval);
/*
***P value point is outside Conic Triangle
*/
   if ( status < 0 ) return(erpush("GE3383","GE136"));
/*
***Max. P value is 0.85
*/
   if ( pval > 0.85) return(erpush("GE3393","GE136"));
/*
***Straight line case.
*/
line:
/*
***Create a rational segment.
***Call of GE133().
*/
   status = GE133(points,pval,pr);
   if ( status < 0 ) return(erpush("GE1263","GE136"));

   return(0);
 }

/********************************************************************/
/*******************************************************************/

static void mk_seg(
       DBfloat  conp[],
       DBSeg   *pr)

/*    Compute the coefficients for a rational cubic segment for
 *    a given Consurf polygon.
 *
 *      In:  conp = [0-3]   = Start point
 *                  [4-7]   = Start tangent point
 *                  [8-11]  = End tangent point
 *                  [12-15] = End point
 *
 *      Out: *pr => The segment created
 *
 *      (C)microform ab 1984-12-29 G.Liden
 *
 *      1999-04-22 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {

/*
***C0 = P0
*/
   pr->c0x = conp[0];
   pr->c0y = conp[1];
   pr->c0z = conp[2];
   pr->c0  = conp[3];
/*
***C1 = -2P1 + 2P2
*/
   pr->c1x = 2.0*(conp[4] - conp[0]);
   pr->c1y = 2.0*(conp[5] - conp[1]);
   pr->c1z = 2.0*(conp[6] - conp[2]);
   pr->c1  = 2.0*(conp[7] - conp[3]);
/*
***C2 = P1 -4P2 + 2P3 + P4
*/
   pr->c2x = conp[0] - 4.0*conp[4] + 2.0*conp[8]  + conp[12];
   pr->c2y = conp[1] - 4.0*conp[5] + 2.0*conp[9]  + conp[13];
   pr->c2z = conp[2] - 4.0*conp[6] + 2.0*conp[10] + conp[14];
   pr->c2  = conp[3] - 4.0*conp[7] + 2.0*conp[11] + conp[15];
/*
***C3 = 2P2 - 2P3
*/
   pr->c3x = 2.0*(conp[4] - conp[8]);
   pr->c3y = 2.0*(conp[5] - conp[9]);
   pr->c3z = 2.0*(conp[6] - conp[10]);
   pr->c3  = 2.0*(conp[7] - conp[11]);
/*
***Offset = 0 JK 27/11/91
*/
   pr->ofs = 0.0;
/*
***Type = CUB_SEG JK 5/3/93
*/
   pr->typ = CUB_SEG;
/*
***Segment length is undefined JK 6/9/93
*/
   pr->sl = 0.0;

return;

 } 

/********************************************************************/
