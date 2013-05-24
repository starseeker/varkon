/*!******************************************************************/
/*  File: geinvon.c                                                 */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE703() Find the closest point on any entity                    */
/*  GE709() Find the closest point on a line                        */
/*  GE721() Find the closest point on a 2D arc                      */
/*  GE713() Find all closest points on a 3D Arc/Curve               */
/*  GE143() Find all closest points on a segment                    */
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

      DBstatus GE703(
      DBAny    *pstr,
      DBSeg    *pseg,
      DBVector *pexpt,
      short     intnr,
      DBfloat  *pu)

/*    The function calculates the closest point on a 
 *    curve, 2D or 3D arc or a line. Closest point means
 *    that it is the shortest, perpendicular distance to an external
 *    point.
 *
 *    In: pstr  = The entity
 *        pseg  = Optional segments
 *        pexpt = The external position
 *        intnr = The requested solution
 *
 *    Out: *pu = The parametric value, < 0 = No solution
 *
 *    (C)microform ab 1992-02-14 G.Liden
 *
 *    1992-12-05 Error 733 changed to 740, Gunnar Liden
 *    1999-05-25 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;      /* Function value from called function    */
   DBetype  type;        /* The input curve type                   */
   short    noint;       /* The number of closest points           */
   DBfloat  uout[INTMAX];/* Array with ordered solutions (u values)*/
   DBfloat  uglob;       /* Global u parameter for the curve       */

/*
***Check of input data.
*/
   if ( intnr < 1 ) return(erpush("GE7033","GE703"));

   if ( intnr > INTMAX ) return(erpush("GE7033","GE703"));
/*
***Determine the curve type
*/
   type = pstr->poi_un.hed_p.type;
/*
***Line.
*/
   if ( type == LINTYP ) return(GE709(&pstr->lin_un,pexpt,pu));  
/*
***2D or 3D arc.
*/
   else if ( type == ARCTYP ) 
     {
     if ( pstr->arc_un.ns_a == 0 ) return(GE721(&pstr->arc_un,pexpt,pu)); 
     else
       {
       status = GE713(pstr,pseg,pexpt,&noint,uout);
       if ( status < 0 ) return(erpush("GE7323","GE703"));
       }
     }
/*
***Curve.
*/
   else if ( type == CURTYP )
     {
     status = GE713(pstr,pseg,pexpt,&noint,uout);
     if ( status < 0 ) return(erpush("GE7323","GE703"));
     }
/*
***Unknown entity type.
*/
   else return(erpush("GE7993","GE703"));
/*
***Select solution
*/
   if ( noint < intnr )
     {
    *pu = -1.0;
     return(erpush("GE7403","GE703"));
     }

  *pu = uout[intnr-1] - 1.0;
/*
***Calculate relative arclength for a 3D circle
*/
   if ( type == ARCTYP  &&  pstr->arc_un.ns_a > 0 )
     {
     uglob = uout[intnr-1];
     status = GE311(&pstr->arc_un,pseg,uglob,pu);
     if ( status < 0 ) return(erpush("GE7993","GE703"));
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

        DBstatus GE709(
        DBLine   *linpek,
        DBVector *pos,
        DBfloat  *tptr)

/*      Computes the closest position on a line.
 *
 *      In: linpek = The line.
 *          pos    = The external position.
 *
 *      Out: *tptr  = Parametric value for closest pos.
 *
 *      (C)microform ab 10/2/92 J. Kjellander
 *
 ********************************************************************/

  {
   DBfloat v1[3],v2[3],sprod,sroot;

/*
***Start-to-end.
*/
   v1[0] = linpek->crd2_l.x_gm - linpek->crd1_l.x_gm;
   v1[1] = linpek->crd2_l.y_gm - linpek->crd1_l.y_gm;
   v1[2] = linpek->crd2_l.z_gm - linpek->crd1_l.z_gm;
/*
***Start-to-external position.
*/
   v2[0] = pos->x_gm - linpek->crd1_l.x_gm;
   v2[1] = pos->y_gm - linpek->crd1_l.y_gm;
   v2[2] = pos->z_gm - linpek->crd1_l.z_gm;
/*
***Project v2 on v1.
*/
   sroot = SQRT(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
   sprod = (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]) / sroot;
/*
***Make 0 -> 1 parametrization.
*/
   sprod /= sroot;
/*
***If outside 0 -> 1 select closest end.
*/
   if      ( sprod > 1.0 ) sprod = 1.0;
   else if ( sprod < 0.0 ) sprod = 0.0;
/*
***End.
*/
   *tptr = sprod;

   return(0);
 }

/********************************************************************/
/********************************************************************/

        DBstatus GE721(
        DBArc    *arcpek,
        DBVector *pos,
        DBfloat  *tptr)

/*      Computes the closest position on a 2D arc.
 *
 *      In: arcpek = The 2D arc.
 *          pos    = The external position.
 *
 *      Out: *tptr  = Parametric value for closest pos.
 *
 *      (C)microform ab 10/2/92 J. Kjellander
 *
 *******************************************************************!*/

  {
    short  status;
    gmflt  v1,v2,r;
    GMARC  arc;
    GMSEG  seg[4];
    DBVector  origo;
/*
***Create 3D-arc.
*/
    arc.hed_a.type = ARCTYP;
    origo.x_gm = arcpek->x_a;
    origo.y_gm = arcpek->y_a;
    origo.z_gm = 0.0;
    v1 = arcpek->v1_a;
    v2 = arcpek->v2_a;
    r  = arcpek->r_a;

    if ( (status=GE300(&origo,r,v1,v2,NULL,&arc,seg,3)) < 0 ) 
      return(status);
/*
***Call GE703().
*/
    return(GE703((DBAny *)&arc,seg,pos,(short)1,tptr));
 }

/********************************************************************/
/********************************************************************/

        DBstatus GE713(
        DBAny    *pstr,
        DBSeg    *pseg,
        DBVector *pexpt,
        short    *pnoint,
        DBfloat   uout[])

/*      The function calculates the closest points on a multi-segment
 *      cubic rational curve. Closest points means that it is the
 *      shortest, perpendicular distance to an external point.
 *
 *      In: pstr   = The curve
 *          pseg   = Its segments
 *          pexpt  = The external position
 *
 *      Out: *pnoint = The number of solutions
 *            uout[] = The solutions
 *
 *      (C)microform ab 1991-11-29 G.Liden
 *
 *      1999-05-25 Rewritten, J.Kjellander
 *
 ********************************************************************/

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
***Initializations for the curve segment loop
***Retrieve the number of segments from the arc/curve
*/
   if ( type == ARCTYP ) 
     {
     noseg = pstr->arc_un.ns_a;
     if ( noseg == 0 ) return(erpush("GE7323","GE713"));
     }
/*
***Curve.
*/
   else if ( type == CURTYP ) noseg = pstr->cur_un.ns_cu;
/*
***Unknown type of entity.
*/
   else return(erpush("GE7993","GE713"));
          
/*
***Start value noall=0 (the total number of closest points)
*/
   noall = 0;
/*
***The number of restarts nstart= RESTRT+1  (RESTRT+2 for arc)
*/
   nstart = RESTRT+1;     

   if ( type == ARCTYP ) nstart += 1;
/*
***Find closest points for all the segments in the curve
***Loop for all segments k= 1,2,3...,noseg
*/
   for ( k=1; k <= noseg; k++ )
     {
/*
***Adress to the current segment
*/
     pcseg = (pseg + k - 1);
     status = GE143(pstr,pcseg,pexpt,nstart,&noinse,useg);
     if ( status < 0 ) return(erpush("GE7313","GE713"));
/*
***Add u values to the global u vector
*/
     for ( i=noall; i <= noall + noinse - 1; i++ )
       uglob[i] = useg[i-noall] + (DBfloat)k;
/*
***Increase the number of intersects
*/
     noall += noinse;
     }
/*
***Output u values for the closest points
*/
  *pnoint = noall;
/*
***Return from function if there are no closest points.
***If noall <= 1  return with no error
*/
   if ( noall == 0 ) return(0);

   if ( noall == 1 ) uout[0] = uglob[0];
/*
***Ordering of the u array and deletion of double points.
*/
   else if ( noall > 1 )
     {
     if ( GEsort_1(uglob,noall,pnoint,uout ) < 0 )
       return(erpush("GE7313","GE713"));
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE143(
      DBAny    *pstr,
      DBSeg    *pseg,
      DBVector *pexpt,
      short     nstart,
      short    *pnoint,
      DBfloat   uout[])

/*    The function calculates the closest points on a rational
 *    cubic segment. Closest points means that it is the shortest,
 *    perpendicular distance to an external point.
 *
 *    The objective function is the scalar product of the tangent
 *    and the vector from the point on the curve to the external
 *    point. The scalar product will be zero when the angle
 *    between the vectors is 90 degrees, i.e. the closest point
 *    to the curve. The scalar product will also become zero when
 *    the point is on the curve, i.e for the case when the tangent
 *    and the vector, between the input point and the curve point,
 *    are parallell. The scalar product will be zero when vector
 *    is zero, i.e when the input point and the curve point are
 *    equal.
 *
 *    The end criterion is the coordinate tolerance ctol/5. This
 *    value is compared with the scalar product (the objective
 *    function) divided with the length of the tangent. This
 *    value is a "true" (projected) distance.
 *
 *    All solutions will be compared with the first solution. For
 *    most of the cases will all equal solutions (due to the
 *    restarts) be eliminated. But there is no guarantee for this !
 *
 *    In: pstr   = The arc/curve
 *        pseg   = Its segments
 *        pexpt  = The external position
 *        nstart = The number of restarts
 *
 *    Out: *pnoint = The number of solutions
 *          uout[] = The solutions including double points
 *                   created by restarts
 *
 *      (C)microform ab 1992-01-25 G. Liden
 *
 *     1992-12-04 End criterion 0 changed to 2*TOL4 Gunnar Liden
 *     1996-05-24 ctol, documentation and debug     Gunnar Liden
 *     1996-06-09 Debug                             Gunnar Liden
 *     1996-10-22 Debug                             Gunnar Liden
 *     1996-11-16 Debug                             Gunnar Liden
 *     1999-05-24 Rewritten, J.Kjellander
 *     1999-12-18 sur751->varkon_ctol               Gunnar Liden
 *
 *****************************************************************!*/

 {
#define JMAX  20            /* The maximum number of iterations     */

   DBfloat  ctol;           /* Coordinate tolerance                 */
   DBfloat  f_zero;         /* Objective function value             */
   DBfloat  v_leng;         /* Tangent length                       */
   DBfloat  ex,ey,ez;       /* The input external point             */
   DBfloat  xu,yu,zu;       /* Coordinates for u                    */
   DBfloat  dxdu,dydu,dzdu; /* Derivatives for u                    */
   DBfloat  d2xdu2;         /* Second derivative with respect to u  */
   DBfloat  d2ydu2;         /* Second derivative with respect to u  */
   DBfloat  d2zdu2;         /* Second derivative with respect to u  */
   DBfloat  u;              /* Parameter value u                    */
   DBfloat  f;              /* Function value                       */
   DBfloat  dfdu;           /* Derivative of f with respect to u    */
   DBfloat  deltau;         /* The u step for the restarts          */
   DBfloat  h;              /* The Newton Rhapson step              */
   DBint    i,j;            /* Loop index i=restarts j=Newton R     */
   DBint    numint;         /* The number of intersects (=*pnoint)  */
   DBfloat  u_first;        /* Parameter for first (best) solution  */
   DBfloat  f_first;        /* Value     for first (best) solution  */
   DBVector p_first;        /* Point     for first (best) solution  */
   DBfloat  dist;           /* Distance to first solution point     */
   EVALC    evldat;         /* Fot evaluation in GE110()           */

/*
***Initializations
*/
   u_first = F_UNDEF;       
   f_first = F_UNDEF;      
   f_zero  = F_UNDEF;      
/*
***Initializations for Newton Rhapson
***External point to local variables
*/
   ex = pexpt->x_gm;
   ey = pexpt->y_gm;
   ez = pexpt->z_gm;
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
***Get coordinate tolerance (end criterion for calculation)
*/
   ctol      = varkon_ctol();
   ctol      = ctol/1000.0;
/*
***Evaluation needed by GE110().
*/
   evldat.evltyp = EVC_R + EVC_DR + EVC_D2R;
/*
***The Newton Rhapson loop
*/
   for ( i=1; i <= nstart; i++ )
     {
     u = deltau*(i-1);

     for ( j=1; j <= JMAX; j++ )
        {
/*
***Compute function f and dfdu
*/
        evldat.t_local = u;
        GE110(pstr,pseg,&evldat);

        xu = evldat.r.x_gm;
        yu = evldat.r.y_gm;
        zu = evldat.r.z_gm;

        dxdu = evldat.drdt.x_gm;
        dydu = evldat.drdt.y_gm;
        dzdu = evldat.drdt.z_gm;

        d2xdu2 = evldat.d2rdt2.x_gm;
        d2ydu2 = evldat.d2rdt2.y_gm;
        d2zdu2 = evldat.d2rdt2.z_gm;

        f    =  dxdu*(ex-xu) + dydu*(ey-yu) + dzdu*(ez-zu); 

        dfdu =   d2xdu2*(ex-xu) - dxdu*dxdu + 
                 d2ydu2*(ey-yu) - dydu*dydu +
                 d2zdu2*(ez-zu) - dzdu*dzdu ;

         f_zero = ABS(f);
         v_leng = SQRT(dxdu*dxdu + dydu*dydu + dzdu*dzdu);
         if ( v_leng > 0.00000001 ) f_zero = f_zero/v_leng;
/*
***Minimum point found ?
*/
         if ( f_zero  <= ctol ) 
           {
/*
***Minimum point is found
***Increase the number of solution points if not
***equal to the first solution.
***Solution only if the u value is between 0 and 1
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress
*/
         if ( u >= 0.0  && u < 1.0 )
           {
           numint = numint + 1;
           if ( numint == 1 )
             {
             u_first        = u;
             f_first        = f_zero;
             p_first.x_gm   = xu;
             p_first.y_gm   = yu;
             p_first.z_gm   = zu;
             uout[numint-1] = u;
             } /* End  numint == 1 */
           else
             { /* Start numint != 1 */
             dist = SQRT((p_first.x_gm-xu)* (p_first.x_gm-xu)+
                         (p_first.y_gm-yu)* (p_first.y_gm-yu)+
                         (p_first.z_gm-zu)* (p_first.z_gm-zu));
             if ( dist <  ctol )
               {
               numint = numint - 1; /* No new solution */ 
               if ( f_zero <  f_first ) /* Best solution */
                 {
                 u_first        = u;
                 f_first        = f_zero;
                 p_first.x_gm   = xu;
                 p_first.y_gm   = yu;
                 p_first.z_gm   = zu;
                 uout[0]        = u;
                 }
               } /* End dist < ctol <==> solution exists */
             else uout[numint-1] = u;
             } /* End   numint != 1 */
           } /* End   u >= 0.0  and u < 1.0 */
/*
***Note that u = 0 and 1-TOL4 will be tested
***The minimum number of restarts is 2
*/
         j = JMAX;
         } /* End, there is a solution */
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
       if ( ABS(u - 1.0) < TOL4  &&  h > 2*TOL4 ) j = JMAX;
       if ( ABS(h) > 2.0 ) j = JMAX;
/*
***Compute the next parameter value
*/
       u = u + h;
       if ( u < -TOL4 ) u = 0.1*TOL4;
       if ( u > 1.0 + TOL4 ) u = 1.0 - 0.1*TOL4;
       } /* end loop j : Newton Rhapson */
     } /* end loop i : Restarts with deltau */

  *pnoint = (short)numint;

   return(0);
 }

/*******************************************************************/
