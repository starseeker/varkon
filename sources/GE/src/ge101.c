/*!******************************************************************/
/*  File: ge101.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE101() Intersect between a rational cubic segment and a plane  */
/*  GE102() Intersect between two rational cubic segments           */
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

#define JMAX  20             /* The maximum number of iterations    */

static short fix_tol2(DBAny *pstr, DBSeg *pseg, DBfloat *ptol2);

/********************************************************************/

      DBstatus GE101(
      DBAny  *pstr,
      DBSeg  *pseg,
      DBfloat plane[],
      short   nstart,
      short  *pnoint,
      DBfloat uout[])

/*    Intersect between a rational cubic segment and a plane.
 *
 *      In: pstr   = Pointer to arc/curve
 *          pseg   = Pointer to segment
 *          plane  = Nx, Ny, Nz for the plane
 *          nstart = Requested number of restarts
 *
 *      Out: *pnoint = Number of intersects incl. double pts.
 *           *uout   = Solution parametric values
 *
 *      (C)microform ab 1985-01-26 G.Liden
 *
 *      1999-04-19 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat u;              /* Parameter value u                   */
   DBfloat f;              /* Function value                      */
   DBfloat dfdu;           /* Derivative of f with respect to u   */
   DBfloat deltau;         /* The u step for the restarts         */
   DBfloat h;              /* The Newton Rhapson step             */
   short   i,j;            /* Loop index i=restarts j=Newton R    */
   short   numint;         /* The number of intersects (=*pnoint) */
   short   status;         /* Return value  gei101()              */
   DBfloat tol2;           /* Modified tolerance TOL2             */
   EVALC evldat;           /* Fot evaluation in GE110()           */

/*
***Modification of the coordinate tolerance TOL2
*/
   status = fix_tol2(pstr,pseg,&tol2);
   if (status < 0 )return(erpush("GE1993","GE101"));
/*
***The step for the restarts
***Min should be 2 and note that last u will be < 1.0 )
*/
   deltau = (1.0 - TOL4)/(DBfloat)(nstart - 1);
/*
***Start numint = 0
*/
   numint = 0;
/*
***Evaluation needed by GE110().
*/
   evldat.evltyp = EVC_R + EVC_DR;
/*
***Start restarts
*/
   for ( i=1; i <= nstart; ++i )
     {
     u = deltau*(i-1);
/*
***Start Newton Raphson iterations
***Compute function f and dfdu
*/
     for ( j=1; j <= JMAX; j++ )
       {
        evldat.t_local = u;
        GE110(pstr,pseg,&evldat);

        f    = plane[0]*evldat.r.x_gm +
               plane[1]*evldat.r.y_gm + 
               plane[2]*evldat.r.z_gm - plane[3];

        dfdu = plane[0]*evldat.drdt.x_gm + 
               plane[1]*evldat.drdt.y_gm + 
               plane[2]*evldat.drdt.z_gm;
/*
***Intersect point found ?
*/
        if ( ABS(f) <= tol2 )
          {
/*
***Intersect point is found
***Increase the number of intersects, also if it is
***the same point, and add u point to uout vector
***Solution only if the u value is between 0 and 1
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress.
*/
          if ( u >= 0.0  )
            {
            if ( u < 1.0  )
              {
              numint = numint + 1;
              uout[numint-1] = u;
               }
             }
/*
***Note that u = 0 and 1-TOL4 will be tested
***The minimum number of restarts is 2
*/
          j = JMAX;
          }
/*
***Derivative dfdu to small (only nearest point)  ?
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
        if ( ABS(u) < TOL4 )
          {
/*
***1994-10-17     if ( h < 0 ) j = JMAX;
***Finns mer att fundera på .....
*/
          if ( h < 0 && u + h < 0.0 ) j = JMAX;
          }

        if ( ABS(u-1.0) < TOL4 )
          {
          if ( h > 0 && u + h > 1.0 ) j = JMAX;
          }

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

/********************************************************************/
/********************************************************************/

      DBstatus GE102(
      DBAny  *pstr1,
      DBSeg  *pseg1,
      DBAny  *pstr2,
      DBSeg  *pseg2,
      DBTmat *pc,
      short   nstart,
      short  *pnoint,
      DBfloat uout1[],
      DBfloat uout2[])

/*    Intersect between two rational cubic segments.
 *
 *      In: pstr1  = Pointer to first arc/curve
 *          pseg1  = Pointer to the segment
 *          pstr2  = Pointer to second arc/curve
 *          pseg2  = Pointer to the segment
 *          pc     = Active coordinate system
 *          nstart = Requested number of restarts
 *
 *      Out: *pnoint = Number of intersects incl. double pts.
 *           *uout1  = Solution parametric values related to pseg1
 *           *uout2  = Solution parametric values related to pseg2
 *
 *      (C)microform ab 1985-02-22 G.Liden
 *
 *     1985-05-02 The hessian matrix is computed earlier
 *     1985-05-26 No scaling, the hessian vector length is used
 *     1985-07-05 Error system  B. Doverud
 *     1985-09-26 Maximum hstep when Hessian is computed Gunnar
 *     1986-05-11 extern short, B. Doverud
 *     1991-12-12 Debug to file fp added     Gunnar Liden
 *     1991-12-14 GE107(), Johan Kjellander
 *     1996-09-30 Intialization m2,m3. Transformation of
 *                segments replaced by transformation of pts
 *                and vectors. Problems with UV_CUB_SEG and curve
 *                planes otherwise.
 *     1999-04-19 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
#define KMAX  20            /* The maximum number of iterations     */
#define HESSD 100000.0      /* Defines for which distance the       */
                            /* objective function can be approxi-   */
                            /* mated with a second degree polynom.  */
                            /* Distance = HESSD*TOL2                */
   DBfloat  xu,yu;          /* Coordinates for u. Curve 1           */
   DBfloat  dxdu,dydu;      /* Derivatives for u. Curve 1           */
   DBfloat  d2xdu2,d2ydu2;  /* Derivatives for u. Curve 1           */
   DBfloat  u;              /* Parameter value u. Curve 1           */
   DBfloat  xv,yv;          /* Coordinates for v. Curve 2           */
   DBfloat  dxdv,dydv;      /* Derivatives for v. Curve 2           */
   DBfloat  d2xdv2,d2ydv2;  /* Derivatives for v. Curve 2           */
   DBfloat  v;              /* Parameter value v. Curve 2           */
   DBfloat  delta;          /* The step for the restarts            */
   short    i;              /* Loop index <=> Restarts curve 1      */
   short    j;              /* Loop index <=> Restarts curve 2      */
   short    k;              /* Loop index <=> Gradient iterations   */
   DBfloat  m;              /* The objective function               */
   DBfloat  dmdu,dmdv;      /* The gradient                         */
   DBfloat  gradl;          /* The length of the gradient vector    */
   short    numint;         /* The number of intersects (=*pnoint)  */
   DBfloat  p[16];          /* The output point in function GE107   */
   DBfloat  deltam;         /* deltam = m2 - m                      */
   DBfloat  dum1,dum2,dum3; /* Dummy variables                      */
   DBfloat  vdum[3];        /* The adjusted, normalised, gradient   */
   DBfloat  d2mdu2,d2mdv2;  /* Elements in the Hessian matrix       */
   DBfloat  d2mdudv;        /*                                      */
   DBfloat  deterh;         /* The determinant of H                 */
   short    nopt;           /* Number of optimal step steps         */
   DBfloat  a0,a1,a2;       /* Parabola coefficients                */
   DBfloat  hmin;           /* Minimum for X=hmin                   */
   DBfloat  minm;           /* Minimum minm = a1*hmin+a2*hmin**2    */
   DBfloat  m1,m2,m3;       /* Three points for the parabola interp */
   DBfloat  hstep;          /* The step along the gradient          */
   short    ka,nopta,nbacka;/* Total sum of steps for one restart   */
   short    nhess;          /* Total times the gradient has been    */
                            /* adjusted                             */
   DBVector poi_in;         /* Point  from p_xyz_in                 */
   DBVector poi_tra;        /* Transformated point                  */
   DBVector vec_in;         /* Vector from p_xyz_in                 */
   DBVector vec_tra;        /* Transformated vector                 */
   DBint    status;         /* Error code from a called function    */
   char     errbuf[80];     /* String for error message fctn erpush */

/*
***The step for the restarts
***Min should be 2 and note that last u will be < 1.0 )
*/
   dum1 = (gmflt)nstart;
   delta = (1.0 - TOL4)/(dum1 - 1.0);
/*
***Start numint = 0
*/
   numint = 0;
/*
***Hessian adjustment of the gradient
*/
   for ( i=1; i <= nstart; i++ ) 
     { /* Loop restarts on curve 1 */
     for ( j=1; j <= nstart; j++ )
       { /* Loop restarts on curve 2 */
       dum1 = (gmflt)i;
       u = 0.0 + delta*(dum1-1.0);
       dum1 = (gmflt)j;
       v = 0.0 + delta*(dum1-1.0);
/*
***Start values
*/
       m    =  TOL6;
       m2   =  TOL6;  /* 1996-09-30 */
       m3   =  TOL6;  /* 1996-09-30 */
/*
***nopt < 0  <==> Flag for k=1, the first iteration
*/
       nopt = - 10;
       ka = 0;    /* Uses non-initialized values below */
       nopta = 0;
       nbacka = 0;
       nhess  = 0;
       for ( k=1; k <= KMAX; k++ )
         { /* Loop Gradient                 */
/*
***Number of steps in optimal ...
*/
         if ( k != 1 ) nopt = 0;

_010:   /* Restart point for m2 < m */

/*
***Evaluation curve 1 segment
*/
         status = GE107(pstr1,pseg1 ,u,(short)2,p);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GE107 cur1%%ge102");
           return(erpush("SU2943",errbuf));
           }
/*
***Transform coordinates.
*/
         poi_in.x_gm = p[0];          
         poi_in.y_gm = p[1];          
         poi_in.z_gm = p[2];          

         status = GEtfpos_to_local (&poi_in, pc, &poi_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfpos_to_local%%ge102");
           return(erpush("SU2943",errbuf));
           }

         xu = poi_tra.x_gm;
         yu = poi_tra.y_gm;
/*
***First derivative
*/
         vec_in.x_gm = p[3];
         vec_in.y_gm = p[4];
         vec_in.z_gm = p[5];

         status = GEtfvec_to_local (&vec_in, pc, &vec_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfvec_to_local drdu%%geo102");
           return(erpush("SU2943",errbuf));
           }

         dxdu = vec_tra.x_gm;
         dydu = vec_tra.y_gm;
/*
***Second derivative
*/
         vec_in.x_gm = p[6];
         vec_in.y_gm = p[7];
         vec_in.z_gm = p[8];

         status = GEtfvec_to_local (&vec_in, pc, &vec_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfvec_to_local d2rdu2%%geo102");
           return(erpush("SU2943",errbuf));
           }

         d2xdu2 = vec_tra.x_gm;
         d2ydu2 = vec_tra.y_gm;
/*
***Evaluation curve 2 segment
*/
         GE107(pstr2,pseg2 ,v,(short)2,p);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GE107 cur1%%ge102");
           return(erpush("SU2943",errbuf));
           }
/*
***Transform coordinates.
*/
         poi_in.x_gm = p[0];          
         poi_in.y_gm = p[1];          
         poi_in.z_gm = p[2];          

         status = GEtfpos_to_local (&poi_in, pc, &poi_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfpos_to_local cur2%%ge102");
           return(erpush("SU2943",errbuf));
           }

         xv = poi_tra.x_gm;
         yv = poi_tra.y_gm;
/*
***First derivative
*/
         vec_in.x_gm = p[3];
         vec_in.y_gm = p[4];
         vec_in.z_gm = p[5];

         status = GEtfvec_to_local (&vec_in, pc, &vec_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfvec_to_local drdv%%geo102");
           return(erpush("SU2943",errbuf));
           }

         dxdv = vec_tra.x_gm;
         dydv = vec_tra.y_gm;
/*
***Second derivative
*/
         vec_in.x_gm = p[6];
         vec_in.y_gm = p[7];
         vec_in.z_gm = p[8];

         status = GEtfvec_to_local (&vec_in, pc, &vec_tra);
         if ( status < 0 ) 
           {
           sprintf(errbuf,"GEtfvec_to_local d2rdv2%%geo102");
           return(erpush("SU2943",errbuf));
           }

         d2xdv2 = vec_tra.x_gm;
         d2ydv2 = vec_tra.y_gm;
/*
***The objective function
*/
         m = (xu-xv)*(xu-xv) + (yu-yv)*(yu-yv);

         if ( nopt <  0 )
           {
/*
***The gradient and the length of the gradient
*/
           dmdu  =  2.0*(xu-xv)*dxdu + 2.0*(yu-yv)*dydu;
           dmdv  = -2.0*(xu-xv)*dxdv - 2.0*(yu-yv)*dydv;
           gradl = dmdu*dmdu + dmdv*dmdv;
           gradl = SQRT(gradl);
           }
/*
***Gradient to small (only nearest point)  ?
*/
         if ( gradl <= TOL1 && m >= TOL1 ) 
           { /* Start gradient is to small  */
           ka = k;
           k = KMAX;
           goto _030;
           } /* End gradient is to small    */
/*
***Adjustment of the search direction
*/
         dum1 = SQRT(m) - HESSD*TOL2;

         if ( dum1 < 0.0 && nopt <  0 )
           { /* Start adjustment  */
           nhess = nhess + 1;
           d2mdu2 =   2.0*dxdu*dxdu + 2.0*(xu-xv)*d2xdu2
                  +   2.0*dydu*dydu + 2.0*(yu-yv)*d2ydu2;
           d2mdv2 =   2.0*dxdv*dxdv - 2.0*(xu-xv)*d2xdv2
                  +   2.0*dydv*dydv - 2.0*(yu-yv)*d2ydv2;
           d2mdudv= - 2.0*dxdu*dxdv - 2.0*dydu*dydv;
           deterh = d2mdu2*d2mdv2 - d2mdudv*d2mdudv;
/*
***Multiplication with the inverse (with deterh ! )
*/
           vdum[0] =   d2mdv2*dmdu  - d2mdudv*dmdv;
           vdum[1] = - d2mdudv*dmdu + d2mdu2*dmdv;
           if( ABS(deterh) > TOL1 )
             {
             vdum[0] =  vdum[0]/deterh;
             vdum[1] =  vdum[1]/deterh;
             }

           vdum[2] = 0.0;
/*
***For the analysis of an elliptic point.
*/
           dum3 = d2mdu2*u*u + d2mdv2*v*v + 2.0*d2mdudv*u*v;

           if ( dum3 > 0.0 && deterh  > TOL1 ) 
             { /* Start elliptic point */
             dmdu = vdum[0];
             dmdv = vdum[1];
/*
***Note that gradl now is the length of the Hessian vector
***and not the gradient
*/
             gradl = vdum[0]*vdum[0] + vdum[1]*vdum[1];
             gradl = SQRT(gradl);
             hstep = -0.8;
             } /* End elliptic point */

           dum1 = SQRT(m) - HESSD*TOL2;

           if ( k > 1  &&
                deterh < -TOL1 &&
                gradl < 200000.0*TOL2 &&
                dum1 < 0.0 )  
             { /* Start saddle point */
/*
***Make an extra restart if i and/or j <= nstart
*/
             dum1 = (gmflt)i;
             dum2 = (gmflt)k;
             dum3 = (gmflt)KMAX;
             if ( i <= nstart-1 ) u = delta*(dum1-1.0) + dum2/dum3*delta;
             dum1 = (gmflt)j;
             if ( j <= nstart-1 ) v = delta*(dum1-1.0) + dum2/dum3*delta;
/*
***Keep the length of the step  hstep
***Not to many attempts
*/
             k += 5;

             if ( k >= KMAX ) goto _030;

             nopt = -10;
             goto _010;
             } /* End saddle point */
           } /* End adjustment  */
/*
***Start step value along the gradient and
***the first piint m1 for the parabola inter-
***polation.
*/ 
           if ( k == 1 ) 
             {
             hstep = -TOL6;
             }

           if ( nopt < 0 )
             { /* Start adjustment of the step length hstep */
/*
***At least three steps within limits 0 <= u,v <= 1
***hstep = -0.8 or estimated for parabola minimum.
*/
             if ( dmdu == 0.0 )                 dum1  = TOL6;
             if ( dmdu >  0.0 && u >  0.1*TOL4 )dum1  = u/3.0/gradl;

             if ( dmdu < 0.0 && u <  1.0-0.1*TOL4 )dum1 = (1.0-u)/3.0/gradl;

             if ( dmdv == 0.0 )                 dum2  = TOL6;
             if ( dmdv >  0.0 && v >  0.1*TOL4 )dum2  = v/3.0/gradl;

             if ( dmdv < 0.0 && v <  1.0-0.1*TOL4 )dum2 = (1.0-v)/3.0/gradl;
/*
***The minimum hstep (=dum3) within 0 <= u,v <= 1
*/
             if ( dum1 > dum2 ) dum3 = dum2;
             else               dum3 = dum1;
/*
***Compare with the input hstep and choose the minimum value
*/
             dum1 = -hstep;
             if ( dum3 < dum1 ) hstep = -dum3;
/*
***Note that it is checked below if the minimum point is
***outside 0 <= u,v <= 1
*/
          } /* End adjustment of the step length hstep */

       if ( nopt == 0 )
         {
/*
***The first point
*/
         m1 = m;
/*
***u,l and v for the next point
*/
         u = u + hstep*dmdu;
         v = v + hstep*dmdv;
         nopt = 1;
         goto _010;
         }
       if ( nopt == 1 ) 
         { /* Start nopt == 1 */
         if ( m >= m1 )
           { /* Start m>=m1 */
           u = u - 0.5*hstep*dmdu;
           v = v - 0.5*hstep*dmdv;
           hstep = hstep/2.0;

           if ( nbacka > KMAX )
             {
             ka = k;
             k = KMAX;
             goto _030;
             }
           nopt = 1;
           nbacka = nbacka + 1;
           goto _010;
           } /* End m>=m1 */
/*
***Point 2
*/
         m2 = m;
/*
***u,l and v for the next point
*/
         u = u + hstep*dmdu;
         v = v + hstep*dmdv;
         nopt = 2;
         goto _010;
         } /* End nopt == 1 */

       deltam = m2 - m ;
       if ( deltam < 0 && nopt > 0 )
/*
***No more steps. Interpolate with parabola and
***find minimum point. Estimation of step length
***for the next step. 
*/
         { /* Start m2 < m */
/*
***Point 3
*/
         m3 = m;
/*
***Parabola interpolation of the three points
***Y = a0 + a1*X + a2*X**2
*/ 
         a0 = m1;
         a2 = (m1/2.0 - m2 + m3/2.0)/hstep/hstep;
         a1 = (m2 - m1)/hstep - a2*hstep;
/*
***Minimum point
*/
         if ( ABS(a2) > TOL1 )
           {
           hmin = - a1/2.0/a2;
           minm = a0 + a1*hmin + a2*hmin*hmin;
           }
         else 
           {
           hmin = hstep;
           minm = m2;
           }

         if ( minm <= m1 && minm <= m2 && minm <= m3 )
           {
/*
***Go (back) to hmin
*/
            u = u - (2.0*hstep - hmin)*dmdu;
            v = v - (2.0*hstep - hmin)*dmdv;
            }
          else  return(erpush("GE1062","geo102"));

          if ( nopt > 2 )
            {
            dum2  = - delta/4.0/gradl;
            dum1 = (gmflt)nopt; 
            dum1 = hstep*SQRT(dum1);
            if ( dum1 > dum2 ) hstep = dum1;
            else               hstep = dum2;
            }
          else
            {
            hstep  = - delta/2.0/gradl;
            }
          nopt = -10;
          goto _010;
          } /* End m2 < m */

        if ( deltam > 0  &&  nopt > 0 )
/*
***Continue along the gradient with step hstep
*/
          { /* Start m2  > m */
/*
***Point 3
*/
          m3 = m;
          m1 = m2;
          m2 = m3;
          nopt = nopt + 1;
          nopta = nopta + 1;
/*
***u and v for the next point
*/
          u = u + hstep*dmdu;
          v = v + hstep*dmdv;
/*
***Check that the values are within limits
***If not, nopt < 0
*/
          if ( u < -TOL4 ) 
            {
            u = 0.1*TOL4;
            nopt = -10;
            }
          if ( u > 1.0 + TOL4 )
            {
            u = 1.0 - 0.1*TOL4;
            nopt = -10;
            }
          if ( v < -TOL4 ) 
            {
            v = 0.1*TOL4;
            nopt = -10;
            }
          if ( v > 1.0 + TOL4 ) 
            {
            v = 1.0 - 0.1*TOL4;
            nopt = -10;
            }
/*
***Max number of steps along the gradient
*/
          if ( nopt > KMAX )
            {
             k = KMAX;
             goto _030;
             }
           if ( nopt > 10 ) nopt = - 10;
           goto _010;                   
           } /* End m2 > m */
/*
***Intersect point found ?
*/
          dum1 = ABS(m);
          dum1 = SQRT(dum1);

          if ( dum1 <= TOL2 ) 
            { /* Start intersect point found  */
/*
***Intersect point is found
***Increase the number of intersects, also if it is
***the same point, and add u point to uout vector
***Solution only if the u value is between 0 and 1
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress.
*/
            if ( u >= 0.0  )
              {
              if ( u < 1.0  )
                {
                if ( v >= 0.0 )
                  {
                  if ( v < 1.0 )
                    {
                    numint = numint + 1;
                    uout1[numint-1] = u;
                    uout2[numint-1] = v;
                    }
                  }
                }
              }
/*
***Note that u = 0 and 1-TOL4 will be tested
***The minimum number of restarts is 2
*/
            ka = k;
            k = KMAX;
            goto _030;
            } /* End intersect point found */
/*
***Stop searching although minimum not is reached
***Searching for min distance for curves far away ?
*/

/*!!!!!!!!!!    if ( k - 5 > 0 && m - 200.0 > 0.0 )
                  ä
                  ka = k;
                  k = KMAX;
                  goto _030;
                  å
!!!!!!!!!!*/

/*
***Solution outside 0 < u < 1 ?
*/
          if ( ABS(u) < TOL4 )
            {
            if ( dmdu >  0 )
              {
              ka = k;
              k = KMAX;
              goto _030;
              }
            }
          dum1 = u - 1.0;
          if ( ABS(dum1) < TOL4 )
            {
            if ( dmdu <  0 ) 
              {
              ka = k;
              k = KMAX;
              goto _030;
              }
            }
/*
***Solution outside 0 < v < 1 ?
*/
          if ( ABS(v) < TOL4 )
            {
            if ( dmdv >  0 ) 
              {
              ka = k;
              k = KMAX;
              goto _030;
              }
            }
          dum1 = v - 1.0;
          if ( ABS(dum1) < TOL4 )
            {
            if ( dmdv <  0 ) 
              {
              ka = k;
              k = KMAX;
              goto _030;
              }
            }
_030:;
          } /* end loop k : Gradient iterations */
        } /* end loop j : Restarts curve 2 with delta */
      } /* end loop i : Restarts curve 1 with delta */

  *pnoint = numint;

   return(0);
 }

/*******************************************************************/
/********************************************************************/

static short fix_tol2(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBfloat *ptol2)

/*    The function calculates the appropriate coordinate
 *    tolerance (TOL2), which will be scaled as a function
 *    of the segment length.
 *
 *      In: pstr = Pointer to arc/curve
 *          pseg = Pointer to  a segment
 *
 *      Out: *ptol2 = The modified tolerance
 *
 *      (C)microform ab 1991-12-11 G.Liden
 *
 *      1998-01-09 TOL2*0.1->TOL2 for long segments Gunnar Liden
 *      1999-04-19 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short   status;      /* Function value from called function    */
   DBetype type;        /* The input curve type                   */
   short   noseg;       /* Number of segments in the curve        */
   DBfloat seg_leng;    /* Segment arclength                      */
   DBfloat interv[2];   /* Local u value for GE120                */

/*
***Determine the curve type and retrieve/calculate length
*/
   type = pstr->poi_un.hed_p.type;
/*
***Line.
*/
   if ( type == LINTYP ) return(erpush("GE7373","fix_tol2"));
/*
***2D arc.
*/
   else if ( type == ARCTYP ) 
     {
     noseg = pstr->arc_un.ns_a;
     if ( noseg == 0 ) return(erpush("GE7373","fix_tol2"));
/*
***3D arc.
*/
     interv[0] = 0.0;
     interv[1] = 1.0;
     status = GE120(pstr,pseg,interv,&seg_leng);
     if ( status < 0 ) return(erpush("GE1273","fix_tol2"));
     }
/*
***Curve.
*/
   else if ( type == CURTYP )
     {
     seg_leng = pseg->sl;
     if ( ABS(seg_leng) < TOL1 )
       {
       interv[0] = 0.0;
       interv[1] = 1.0;
       status = GE120(pstr,pseg,interv,&seg_leng);
       if ( status < 0 ) return(erpush("GE1273","fix_tol2"));
       }
     }

/*
***Illegal entity type.
*/
   else return(erpush("GE9983","fix_tol2"));
/*
***Output tolerance.
*/
   if ( seg_leng > 1000.0 ) *ptol2 = TOL2;

   else if ( seg_leng <= 1000.0   &&  seg_leng > 100.0 )   *ptol2 = TOL2;

   else if ( seg_leng <= 100.0    &&  seg_leng > 10.0 )    *ptol2 = TOL2*0.1;

   else if ( seg_leng <= 10.0     &&  seg_leng > 1.0 )     *ptol2 = TOL2*0.01;

   else if ( seg_leng <= 1.0      &&  seg_leng > 10*TOL2 ) *ptol2 = TOL2*0.001;

   else return(erpush("GE9983","fix_tol2"));

   return(0);
 }

/********************************************************************/
