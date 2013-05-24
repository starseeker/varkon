/*!******************************************************************/
/*  File: geinvcrv.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE704() Find point on arc/curve with given curvature            */
/*  GE714() Find all points on a curve with given kappa             */
/*  GE144() Find all points on a segment with given kappa           */
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

      DBstatus GE704(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBfloat *pkappa,
      short    intnr,
      DBfloat *pu)

/*    The function finds the points with given curvature on a
 *    3D arc or curve.
 *
 *      In:  pstr   = The arc or curve
 *           pseg   = Its segments
 *           pkappa = The requested curvature
 *           intnr  = The requested solution
 *
 *      Out: *pu = The solution
 *
 *      (C)microform ab 1992-01-25 G.Liden
 *
 *      1999-05-25 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from called function    */
   short    noint;       /* The number of curvature points         */
   DBfloat  uout[INTMAX];/* Array with ordered solutions (u values)*/

/*
***Check of input data and initializations
*/
    if ( intnr <1 ) return(erpush("GE7033","GE704"));

    if ( intnr > INTMAX ) return(erpush("GE7033","GE704"));
/*
***Check that the entity really is a curve and call GE714().
*/
   if ( pstr->poi_un.hed_p.type == CURTYP )
     {
     status = GE714(pstr,pseg,pkappa,&noint,uout);
     if ( status < 0 ) return(erpush("GE7323","GE704"));
     }
/*
***Unknown type of entity.
*/
    else return(erpush("GE7993","GE704"));
/*
***Select solution.
*/
   if ( noint < intnr )
     {
    *pu = -1.0;
     return(erpush("GE7393","GE704"));
     }

  *pu = uout[intnr-1] - 1.0;

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE714(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBfloat *pkappa,
      short   *pnoint,
      DBfloat  uout[])

/*    The function finds the points with given curvature on a
 *    curve.
 *
 *      In:  pstr   = The curve
 *           pseg   = Its segments
 *           pkappa = The requested curvature
 *
 *      Out: *pnoint = The number of solutions
 *            uout[] = The solutions (incl. doulble pts)
 *
 *      (C)microform ab 1991-11-29 G.Liden
 *
 *      1999-05-25 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from a called function  */
   DBetype  type;        /* The input curve type                   */
   short    noseg;       /* Number of segments in the curve        */
   DBSeg   *pcseg;       /* Pointer to the current rational segment*/
   short    nstart;      /* The number of restarts (=RESTRT except */
                         /* for arcs where nstart=RESTRT+1 )       */
   short    noinse;      /* Number of closest pts from one segment */
   DBfloat  useg[INTMAX];/* The unordered array of u solutions     */
                         /* from one segment                       */
   short    k;           /* Loop index corresp. to the curve segm. */
   short    noall;       /* The total number of solution pts incl. */
                         /* possible double points ( and before    */
                         /* ordering of the u values )             */
   DBfloat uglob[INTMAX];/* The global u before ordering           */
   short   i;            /* Loop index  for the u values           */


/*
***Determine the curve type
*/
   type = pstr->poi_un.hed_p.type;
/*
***Retrieve the number of segments from the curve
*/
   if ( type == CURTYP ) noseg = pstr->cur_un.ns_cu;
/*
***Illegal entity type.
*/
    else return(erpush("GE7993","GE714"));
/*
***Start value noall=0 (the total number of kappa points)
*/
   noall = 0;
/*
***The number of restarts nstart= RESTRT+1
*/
   nstart = RESTRT+1;     
/*
***Find kappa points for all the segments in the curve
***Loop for all segments k= 1,2,3...,noseg.
*/
   for ( k=1; k <= noseg; k++ )
     { 
/*
***Adress to the current segment
*/
      pcseg = (pseg + k - 1);
      status = GE144(pstr,pcseg,pkappa,nstart,&noinse,useg);
      if ( status < 0 ) return(erpush("GE7313","GE714"));
/*
***Add u values to the global u vector.
*/
      for ( i=noall; i <= noall + noinse - 1; i++ )
            uglob[i] = useg[i-noall] + (DBfloat)k;
/*
***Increase the number of kappa points.
*/
      noall += noinse;
      }
/*
***Output u values for the kappa points
*/
  *pnoint = noall;

   if ( noall == 0 ) return(0);

   if ( noall == 1 ) uout[0] = uglob[0];
/*
***Ordering of the u array and deletion of double points.
*/
   else if ( noall > 1 )
     {
     if ( GEsort_1(uglob,noall,pnoint,uout ) < 0 )
       return(erpush("GE7313","GE714"));
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE144(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBfloat *pkappa,
      short    nstart,
      short   *pnoint,
      DBfloat  uout[])
      
/*    The function finds the points with given curvature on a
 *    curve.
 *
 *      In:  pstr   = The curve
 *           pseg   = Its segments
 *           pkappa = The requested curvature
 *           nstart = The number of restarts
 *
 *      Out: *pnoint = The number of solutions
 *            uout[] = The solutions (incl. doulble pts)
 *
 *      (C)microform ab 1992-01-25 G.Liden
 *
 *      1999-05-25 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
#define JMAX  20           /* The maximum number of iterations     */

   DBfloat kappain;        /* The input curvature value            */
   DBfloat p[17];          /* The output point in function GE107   */
   DBfloat kappau;         /* Curvature for u                      */
   DBfloat dkappadu;       /* Derivative of kappa for u            */
   DBfloat u;              /* Parameter value u                    */
   DBfloat f;              /* Function value                       */
   DBfloat dfdu;           /* Derivative of f with respect to u    */
   DBfloat deltau;         /* The u step for the restarts          */
   DBfloat h;              /* The Newton Rhapson step              */
   short   i,j;            /* Loop index i=restarts j=Newton R     */
   short   numint;         /* The number of intersects (=*pnoint)  */

/*
***Initializations for Newton Rhapson
***Input kappa to local variable
*/
   kappain = *pkappa;
/*
***The step for the restarts
***(Min should be 2 and note that last u will be < 1.0 )
*/
   deltau = (1.0 - TOL4)/(DBfloat)(nstart - 1);
/*
***Start number of points with given vector numint = 0
*/
   numint = 0;
/*
***The Newton Rhapson loop
*/
   for ( i=1; i <= nstart; i++ )
     {
     u = deltau*(i - 1);

     for ( j=1; j <= JMAX; j++ )
       {
/*
***Compute function f and dfdu
*/
       if ( GE107(pstr,pseg,u,(short)4,p) != 0 )
         return(erpush("GE1003","GEinvcrv"));

       kappau   = p[15];
       dkappadu = p[16];

       f     = kappau - kappain ;
       dfdu  = dkappadu ; 
/*
***Minimum point found ?
*/
       if ( ABS(f) <= TOL2*0.1 ) 
         {
/*
***Minimum point is found
***Increase the number of minimum points, also if
***it is the same point
***Solution only if the u value is between 0 and 1
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress.
*/
         if ( u >= 0.0  &&  u < 1.0  )
           {
         ++numint;
           uout[numint-1] = u;
           }
/*
***Note that u = 0 and 1-TOL4 will be tested
***The minimum number of restarts is 2
*/
         j = JMAX;
         }
/*
***Derivative dfdu to small
*/
       if ( ABS(dfdu) <= TOL3 ) 
         {
/*
***The step h to big
*/
         j = JMAX;
         h = TOL6;
         }
       else h = -f/dfdu;
/*
***Solution outside 0 < u < 1 ?
*/
       if ( ABS(u) < TOL4  &&  h < 0 ) j = JMAX;
       if ( ABS(u - 1.0) < TOL4  &&  h > 0 ) j = JMAX;
       if ( ABS(h) > 2.0 ) j = JMAX;
/*
***Compute the next parameter value
*/
       u = u + h;
       if ( u < -TOL4 ) u = 0.1*TOL4;
       if ( u > 1.0 + TOL4 ) u = 1.0 - 0.1*TOL4;
       } /* end loop j : Newton Rhapson */
     } /* end loop i : Restarts with deltau */

  *pnoint = numint;

   return(0);
 } 

/*******************************************************************/
