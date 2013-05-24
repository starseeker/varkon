/*!******************************************************************/
/*  File: ge717.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE717() Calculates global u for a given relative arclength      */
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

/* Static variables (common for GE717 and parab)                   */

static DBfloat ulocal_pre; /* Parameter for the segment. Previous pt */
static DBfloat f_pre;      /* Function value f= f(u).    Previous pt */
static DBfloat ulocal;     /* Parameter for the segment              */
static DBfloat f;          /* Function value f= f(u)                 */
static DBfloat dfdu;       /* Function f(u) derative value w.r.t u   */
static short   no_iter;    /* Number of iterations                   */
static DBfloat comptol;    /* Computer tolerance (accuracy)          */
static DBfloat om_comptol; /* 1.0 - comptol (for speed)              */
static DBfloat ctol;       /* Coordinate end calculation criterion   */

static void parab();

/********************************************************************/

      DBstatus GE717(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBTmat  *pc,
      DBfloat  rel_leng,
      DBfloat *pu)

/*    The function calculates the global parameter value for a
 *    given relative arclength on a curve or a 3D circle.
 *
 *      In: pstr     = The entity
 *          pseg     = Optional segments
 *          pc       = The active coordinate system
 *          rel_leng = Relative length
 *
 *      Out: *pu = The parametric value
 *
 *      (C)microform ab 1992-01-26 G.Liden
 *
 *      1994-11-20 comptol added (for surface curves) G Liden
 *      1996-11-17 Bug: Negative arqument to SQR
 *                 Error in parabola creation
 *                 TOL2->ctol TOL1->100*comptol  G Liden
 *      1999-05-25 Rewritten, J.Kjellander
 *      1999-12-18 sur753->varkon_comptol sur751->.._ctol G Liden
 *      2007-01-22 Added restart with linear method, Sören L
 *
 *****************************************************************!*/

 {
   short   status;       /* Function value from called function    */
   DBetype  type;        /* The input curve type                   */
   short    noseg;       /* Number of segments in the curve        */
   DBfloat  tot_leng;    /* Total arclength                        */
   DBfloat  abs_leng;    /* Absolute arclength (tot_leng*rel_leng) */
   DBfloat  interv[2];   /* Local u value for GE120                */
   DBfloat  sum_leng;    /* Sum of segment arclengths              */
   DBfloat  dl;          /* Arclength for one segment              */
   short    iseg;        /* Loop index segment number              */
   DBfloat  delta_leng;  /* Delta length= abs_leng-sum_leng        */
   EVALC    evldat;      /* For evaluation in GE110()              */
   short    restart;     /* 1 will trig restart with linear method */

/*
***Surface computer accuracy and end calulation criterion
*/
   comptol    = varkon_comptol();
   om_comptol = 1.0 - comptol;
   ctol       = varkon_ctol();

   if ( rel_leng < -comptol ) return(erpush("GE7353","GE717"));

   if ( rel_leng > 1.0 + comptol ) return(erpush("GE7363","GE717"));
/*
***Determine the curve type and retrieve noseg.
*/
   type = pstr->hed_un.type;
/*
***Line.
*/
   if ( type == LINTYP ) return(erpush("GE7373","GE717"));
/*
***2D arc.
*/
   else if ( type == ARCTYP ) 
     {
     noseg = pstr->arc_un.ns_a;
     if ( noseg == 0 ) return(erpush("GE7373","GE717"));
/*
***3D arc.
*/
      tot_leng = pstr->arc_un.al_a;
      if ( tot_leng < comptol )
        {
        status = GEarclength(pstr,pseg,&tot_leng);
        if(status<0)return(erpush("GE8243","GE717"));
        }
      }
/*
***Curve.
*/
   else if ( type == CURTYP )
     {
     noseg    = pstr->cur_un.ns_cu;
     tot_leng = pstr->cur_un.al_cu;

     if ( tot_leng < comptol )
       {
       status = GEarclength(pstr,pseg,&tot_leng);
       if(status<0)return(erpush("GE8243","GE717"));
        }
     }
/*
***Illegal entity type.
*/
   else return(erpush("GE7993","GE717,wrong type"));
/*
***If rel_leng is zero (<comptol) or one (>1-comptol)
***we can make it really simple.
*/
   if ( rel_leng < comptol )
     {
    *pu = 1.0;
     return(0);
     }

   if ( rel_leng > om_comptol )
     {
    *pu = (DBfloat)(noseg + 1);
     return(0);
     }
/*
***The absolute length abs_leng = tot_leng*rel_leng.
*/
   abs_leng = tot_leng*rel_leng;
/*
***Evaluation needed by GE110().
*/
   evldat.evltyp = EVC_DR;
/*
***Retrieve segment arclengths until sum exceeds abs_leng
*/
   sum_leng  = 0.0;
   interv[0] = 0.0;
   interv[1] = 1.0;

   for ( iseg=0; iseg<noseg; iseg++ )
      {
      dl = (pseg+iseg)->sl;
      sum_leng += dl;
      if (sum_leng > abs_leng ) break;
      }
/*
***Normaly the function will not restart, but if it fails
***it will try once also with linear method, not using parab()
*/
restart=-1;
restart:
restart++;

/*
***The relative arclength is in segment iseg
***Delta arclength in the segment delta_leng= dl-(sum_leng-abs_leng)
*/
   delta_leng = dl - (sum_leng - abs_leng);
/*
***Start value ulocal = delta_leng/dl for the numerical solution
*/
   if ( ABS(dl) >= comptol ) ulocal = delta_leng/dl;
   else return(erpush("GE7993","GE717, ABS(dl)>=comptol"));
/*
***Numerical solution for find X for F(X)=0
***Initialisation of loop variables.
*/
   no_iter = interv[0] = 0.0;

   if ( ulocal < 0.5 ) 
     {
     ulocal_pre =  comptol;
     f_pre      = -delta_leng;
     }
   else
     {
     ulocal_pre = om_comptol;
     f_pre      = dl - delta_leng;
     }
/*
***Next iteration.
*/
loop: ++no_iter;

   if ( no_iter > 10 )
     {
     if (restart==0) goto restart;
     else if ( no_iter > 20 ) return(erpush("GE7993","GE717 (no_iter)"));
     }
/*
***Calculation of function value f and derivative dfdu
*/
   if ( ulocal < -4.0 ) return(erpush("GE7993","GE717 (u<-4.0)"));
   if ( ulocal >  4.0 ) return(erpush("GE7993","GE717 (u>4.0)"));

   interv[1] = ulocal;

   status = GE120(pstr,pseg+iseg,interv,&dl);
   if ( status < 0 ) return(erpush("GE1273","GE717 (loop)"));

   evldat.t_local = ulocal;
   GE110(pstr,pseg+iseg,&evldat);

   f    = dl - delta_leng;
   dfdu = SQRT(evldat.drdt.x_gm*evldat.drdt.x_gm +
               evldat.drdt.y_gm*evldat.drdt.y_gm +
               evldat.drdt.z_gm*evldat.drdt.z_gm);
/*
***Optimal point if function value f < ctol.
*/
   if ( ABS(f) < ctol )
     {
    *pu = (DBfloat)(iseg + 1) + ulocal;
     goto  end;
     }

   if ( ABS(dfdu) < comptol ) return(erpush("GE7993","GE717"));
/*
***Next ulocal = ulocal-f/dfdu and goto loop
***A Newton-Rhapson (linear interpolation) solution would be
***     ulocal_pre = ulocal;
***     f_pre      = f;
***     ulocal= ulocal-f/dfdu;
***Normally (geo102) a linear method is faster in the beginning
***but in this case (hyperbola p=0.95) will Newton-Rhapson fail
***Parabola (second degree) interpolation.
*/
   if (restart) /* use linear method */
     {
     ulocal_pre = ulocal;
     f_pre      = f;
     ulocal= ulocal-f/dfdu;
     }
   else parab();
   goto loop;
/*
***Label end: Optimal point
*/
end:

   return(0);
 }

/********************************************************************/
/********************************************************************/

  static void parab()

/*    The function interpolates a start point, an end point and
 *    and an end derivative with a parabola and calculates X
 *    for F(X)=0, corresponding to the requested arclength.
 *
 *  Theory
 *  ______
 *
 *  Local system u,v where (ulocal_pre,f_pre) is origin:
 *
 *  v(u) = b1*u +   b2*u**2          (1)
 *  dvdu = b1   + 2*b2*u             (2)
 *
 *  v1   = b1*u1 +   b2*u1**2        (1)
 *  dfdu = b1    + 2*b2*u1           (2)
 *
 *  b1   = v1/u1 -   b2*u1           (1)
 *  b2   = (dfdu - b1)/(u1*2)        (2)
 *
 *  b1   = v1/u1 -   (dfdu-b1)/2     (1)
 *  b2   = (dfdu - b1)/(u1*2)        (2)
 *
 *  b1   = v1/u1 -  dfdu/2+b1/2      (1)
 *  b2   = (dfdu - b1)/(u1*2)        (2)
 *
 *  b1   = 2*v1/u1 -  dfdu           (1)
 *  b2   = (dfdu - b1)/(u1*2)        (2)
 *
 *  b1   = 2*v1/u1 -  dfdu           (1)
 *  b2   = (dfdu-2*v1/u1+dfdu)/(u1*2)(2)
 *
 *  b1   = 2*v1/u1 -  dfdu           (1)
 *  b2   = dfdu/u1 - v1/u1**2        (2)
 *
 *  Test
 *  ____
 *
 *  h_l= b1*u1 +   b2*u1**2          (1)
 *  h_l= b1    + 2*b2*u1             (2)
 *
 *  h_l= (2*v1/u1-dfdu)*u1 +  (dfdu/u1-v1/u1**2)*u1**2   (1)
 *  h_l= 2*v1/u1 -dfdu    + 2*(dfdu/u1-v1/u1**2)*u1      (2)
 *
 *  h_l= 2*v1-dfdu*u1 +   dfdu*u1-v1                     (1)
 *  h_l= 2*v1/u1 -dfdu    + 2*dfdu-2*v1/u1               (2)
 *
 *  h_l=   v1                                            (1)
 *  h_l=   dfdu                                          (2)
 *
 *      (C)microform ab 1992-01-26 G.Liden
 *
 *      1999-05-25 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat u1;          /* Local coordinate for ulocal_pre        */
   DBfloat v1;          /* Local coordinate for f_pre             */
   DBfloat b1,b2,k;     /* Coefficients for parabola              */
   DBfloat v;           /* Local coordinate corresp. to parab=0   */
   DBfloat u_r1,u_r2;   /* Two solutions (roots)                  */
   DBfloat temp;        /* Dummy gmflt                            */

/*
***Create parabola
***Input points to local coordinates
*/
   u1 = ulocal - ulocal_pre;
   v1 = f - f_pre;
/*
***Coefficients for the parabola
*/
   if ( u1*u1 > 10.0*comptol)
     {
     b1 = 2.0*v1/u1 - dfdu;
     b2 = dfdu/u1 - v1/u1/u1;
     k  = b1/(2.0*b2);
     }
   else
     {
     u_r1 = u1;
     u_r2 = u_r1;
     goto sel_root;
     }
/*
***Calculate new ulocal for parabola= 0
*/
   v =  -f_pre;

   if ( ABS(b2) > 100.0*comptol )
     {
     if  ( k*k + v/b2 < 0.0 )
       {
       u_r1 = u_r2 = u1;
       goto sel_root;
       }
     else
       {
       temp = SQRT(k*k + v/b2);
       u_r1 = -k + temp;
       u_r2 = -k - temp;
       }
     }
   else
     {
     if ( ABS(b1) > 100.0*comptol ) u_r1 = u_r2 = v*u1/b1;
     else                           u_r1 = u_r2 = u1;
     }
/*
***Choose root nearest current u value (ulocal) in the loop
*/
sel_root:

   if ( ABS(u_r1 - u1) < ABS(u_r2-u1) )
     {
     temp       = ulocal;
     ulocal     = u_r1 + ulocal_pre;
     ulocal_pre = temp;
     f_pre      = f;
     }
   else
     {
     temp       = ulocal;
     ulocal     = u_r2 + ulocal_pre;
     ulocal_pre = temp;
     f_pre      = f;
     }

   if      ( ulocal < comptol    ) ulocal = comptol;
   else if ( ulocal > om_comptol ) ulocal = om_comptol;

   return;
 }
/********************************************************************/
