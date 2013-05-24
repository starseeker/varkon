/*!******************************************************************/
/*  File: geinvtan.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE702() Find the specified pos on a curve with a given tangent  */
/*  GE712() Find all positions on a curve with a given tangent      */
/*  GE140() Find all positions on a segment with a given tangent    */
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

#define JMAX  20            /* The maximum number of iterations     */

/********************************************************************/

      DBstatus GE702(
      DBAny    *pstr,
      DBSeg    *pseg,
      DBVector *ptang,
      short     intnr,
      DBfloat  *pu)

/*    The function calculates the points on a multi-segment cubic
 *    rational curve or a 3D arc, which has a tangent direction equal
 *    to an input vector. The 2D circle is not yet implemented.
 *
 *    In: pstr  = The curve (or arc)
 *        pseg  = Its segments
 *        ptang = The requested tangent
 *        intnr = Te requested solution
 *
 *    Out: *pu = The parametric value, < 0 = No solution
 *
 *    (C)microform ab 1992-01-25 G.Liden
 *
 *    1999-05-21 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from called function    */
   DBetype  type;        /* The input curve type                   */
   short    noint;       /* The number of intersects               */
   DBfloat  uout[INTMAX];/* Array with ordered solutions (u values)*/
   DBfloat  uglob;       /* Global u parameter for the curve       */

/*
***Check of input data.
*/
   if ( intnr < 1 ) return(erpush("GE7033","GE702"));

   if ( intnr > INTMAX ) return(erpush("GE7033","GE702"));

   if ( GEvector_length3D(ptang) < TOL2 ) return(erpush("GE7283","GE702"));
/*
***Determine the curve type
*/
   type = pstr->poi_un.hed_p.type;
/*
***Switch to the right function
***Straight line: Error
***2D arc:        Not yet implemented
***3D arc:        Call of GE712
***Curve:         Call of GE712
*/
   if ( type == LINTYP ) return(erpush("GE7293","GE702"));
/*
***Arc.
*/
   else if ( type == ARCTYP ) 
     {
     if ( pstr->arc_un.ns_a == 0 ) return(erpush("GE7323","GE702"));
/*
***3D-Circle defined by rational quadratics
*/
     status = GE712(pstr,pseg,ptang,&noint,uout);
     if(status<0)return(erpush("GE7343","GE702"));
     }
/*
***Curve.
*/
   else if ( type == CURTYP )
     {
     status = GE712(pstr,pseg,ptang,&noint,uout);
     if(status<0)return(erpush("GE7343","GE702"));
     }
/*
***Unknown entity type.
*/
   else return(erpush("GE7993","GE702"));
/*
***Select solution
*/
   if ( noint < intnr )
     {
    *pu = -1.0;
     return(erpush("GE7333","GE702"));
     }

    *pu = uout[intnr-1] - 1.0;
/*
***Calculate relative arclength for a 3D circle
*/
   if ( type == ARCTYP ) 
     {
     if ( pstr->arc_un.ns_a > 0 )
       {
       uglob = uout[intnr-1];
       status= GE311(&pstr->arc_un,pseg,uglob,pu);
       if ( status < 0 ) return(erpush("GE7993","GE702"));
       }
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE712(
      DBAny    *pstr,
      DBSeg    *pseg,
      DBVector *ptang,
      short    *pnoint,
      DBfloat   uout[])

/*    The function calculates the points on a multi-segment cubic
 *    rational curve, which has a tangent direction equal to an
 *    input vector.
 *
 *    In: pstr   = The curve (or arc)
 *        pseg   = Its segments
 *        ptang  = The requested tangent
 *
 *    Out: *pnoint = The number of solutions (including double points)
 *         *uout   = The solutions (including double points)
 *
 *    (C)microform ab 1991-11-29 G.Liden
 *
 *    1999-05-21 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;       /* Function value from a called function  */
   DBetype  type;         /* The input curve type                   */
   short    noseg;        /* Number of segments in the curve        */
   DBSeg   *pcseg;        /* Pointer to the current rational segment*/
   short    nstart;       /* The number of restarts (=RESTRT except */
                          /* for arcs where nstart=RESTRT+1 )       */
   short    noinse;       /* Number of tangent pts from one segment */
   DBfloat  useg[INTMAX]; /* The unordered array of u solutions     */
                          /* from one segment                       */
   short    k;            /* Loop index corresp. to the curve segm. */
   short    noall;        /* The total number of intersects incl.   */
                          /* possible double points ( and before    */
                          /* ordering of the u values )             */
   DBfloat  uglob[INTMAX];/* The global u before ordering           */
   short    i;            /* Loop index  for the u values           */

/*
***Determine the curve type.
*/
   type = pstr->poi_un.hed_p.type;
/*
***Initializations for the curve segment loop
***Retrieve the number of segments from the curve
*/
   if ( type == LINTYP ) return(erpush("GE7293","GE712"));
/*
***Arc.
*/
   else if ( type == ARCTYP ) 
     {
     noseg = pstr->arc_un.ns_a;
     if ( noseg == 0 ) return(erpush("GE7323","GE712"));
     }
/*
***Curve.
*/
   else if ( type == CURTYP ) noseg = pstr->cur_un.ns_cu;
/*
***Unknown entity type.
*/
    else return(erpush("GE7993","GE712"));
/*
***Start value noall=0 (the total number of tangent points)
*/
    noall = 0;
/*
***The number of restarts nstart= RESTRT+1  (RESTRT+2 for arc)
*/
    nstart = RESTRT+1;     

    if ( type == ARCTYP ) nstart = nstart + 1;
/*
***Find tangent points for all the segments in the curve
***Loop for all segments k= 1,2,3...,noseg
*/
    for ( k=1; k <= noseg; k++ )
      { 
/*
***Adress to the current segment
*/
      pcseg = (pseg + k - 1);
      status = GE140(pstr,pcseg,ptang,nstart,&noinse,useg);
      if ( status < 0 ) return(erpush("GE7313","GE712"));
/*
***Add u values to the global u vector
*/
      for ( i=noall; i <= noall + noinse - 1; i++ )
        {
        uglob[i] = useg[i-noall] + (DBfloat)k;
        }
/*
***Increase the number of tangents.
*/
      noall += noinse;
      }
/*
***End loop all segments.
***Output u values for the tangent points.
*/
  *pnoint = noall;
/*
***Return from function if there are no tangent points
***If noall <= 1  return with no error.
*/
   if ( noall == 0 ) return(0);
/*
***If noall = 1  let uout=uglob.
*/
   else if ( noall == 1 ) uout[0] = uglob[0];
/*
***Ordering of the u array and deletion of double points.
*/
   else if ( noall > 1 )
     {
     if ( GEsort_1(uglob,noall,pnoint,uout ) < 0 )
       return(erpush("GE7313","GE712"));
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE140(
      DBAny    *pstr,
      DBSeg    *pseg,
      DBVector *ptang,
      short     nstart,
      short    *pnoint,
      DBfloat   uout[])

/*    The function computes the points with a given vector (tangent)
 *    on a rational cubic segment. The segment may be in offset.
 *    Note that the output normaly contains double points due to
 *    the restarts.
 *
 *    In: pstr   = The curve (or arc)
 *        pseg   = Its segments
 *        ptang  = The requested tangent
 *        nstart = Te reqiored number of restarts
 *
 *    Out: *pnoint = The number of solutions (including double points)
 *         *uout   = The solutions (including double points)
 *
 *    (C)microform ab 1992-01-25 G.Liden
 *
 *    1999-05-21 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  tx,ty,tz;       /* The input tangent vector             */
   DBfloat  nx,ny,nz;       /* General conic plane for a rational   */
   DBfloat  dxdu,dydu,dzdu; /* Derivatives for u                    */
   DBfloat  d2xdu2;         /* Second derivative with respect to u  */
   DBfloat  d2ydu2;         /* Second derivative with respect to u  */
   DBfloat  d2zdu2;         /* Second derivative with respect to u  */
   DBfloat  u;              /* Parameter value u                    */
   DBfloat  f;              /* Function value                       */
   DBfloat  dfdu;           /* Derivative of f with respect to u    */
   DBfloat  deltau;         /* The u step for the restarts          */
   DBfloat  h;              /* The Newton Rhapson step              */
   short    i,j;            /* Loop index i=restarts j=Newton R     */
   short    numint;         /* The number of intersects (=*pnoint)  */
   short    status;         /* Return value  gei140()               */
   DBVector pt3;            /* Third (hard) point for Gen. Conic Pl.*/
   DBTmat   csys;           /* Coordinate system where the xy plane */
                            /* is the General Conic Plane           */
   EVALC    evldat;         /* Fot evaluation in GE110()            */

/*
***Initializations for Newton Rhapson
***Determine the general conic plane
***Call of GE142
*/
   status = GE142(pseg,&pt3,&csys);
   if ( status < 0 )
     {
/*
***Assume that the segment is a straight line
***Let numint=10 if line equal to vector
*/
     numint = 0;
     goto straight_line;
     }

   nx = csys.g31;
   ny = csys.g32;
   nz = csys.g33;
/*
***Tangent to local variables
*/
   tx = ptang->x_gm;
   ty = ptang->y_gm;
   tz = ptang->z_gm;
/*
***The step for the restarts
***(Min should be 2 and note that last u will be < 1.0 )
*/
   deltau = (1.0 - TOL4)/((DBfloat)nstart - 1.0);
/*
***Start number of points with given vector numint = 0
*/
   numint = 0;
/*
***Evaluation needed by GE110().
*/
   evldat.evltyp = EVC_DR + EVC_D2R;
/*
***The Newton Rhapson loop
*/
   for ( i=1; i <= nstart; i++ )
     {
     u = deltau*((DBfloat)i - 1.0);
     for ( j=1; j <= JMAX; j++ )
       {
/*
***Compute function f and dfdu
*/
       evldat.t_local = u;
       GE110(pstr,pseg,&evldat);

       dxdu = evldat.drdt.x_gm;
       dydu = evldat.drdt.y_gm;
       dzdu = evldat.drdt.z_gm;

       d2xdu2 = evldat.d2rdt2.x_gm;
       d2ydu2 = evldat.d2rdt2.y_gm;
       d2zdu2 = evldat.d2rdt2.z_gm;

       f    =   nx*ty*dzdu + nz*tx*dydu + ny*tz*dxdu 
               -nz*ty*dxdu - ny*tx*dzdu - nx*tz*dydu;

       dfdu =   nx*ty*d2zdu2 + nz*tx*d2ydu2 + ny*tz*d2xdu2 
               -nz*ty*d2xdu2 - ny*tx*d2zdu2 - nx*tz*d2ydu2;
/*
***Minimum point found ?
*/
       if ( ABS(f) <= TOL2 ) 
         {
/*
***Minimum point is found
***Increase the number of minimum points, also if
***it is the same point
***Solution only if the u value is between 0 and 1
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress
***Only tangent with the same direction as the given
***vector is a solution (dot product > 0 )
*/
         if ( u >= 0.0  )
           {
           if ( u < 1.0  )
             {
             if ( tx*dxdu+ty*dydu+tz*dzdu > 0.0 )
               {
               numint = numint + 1;
               uout[numint-1] = u;
               }
             }
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
       }   /* end loop j : Newton Rhapson        */
     }       /* end loop i : Restarts with deltau  */
/*
***The segment is a straight line
*/
straight_line:

  *pnoint = numint;

   return(0);
 }

/*******************************************************************/
