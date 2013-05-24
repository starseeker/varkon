/*!******************************************************************/
/*  File: gelinint.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE705() Line - line intersect                                   */
/*  GE730() Line - line intersect in offset                         */
/*  GE706() Line - arc intersect in 2D                              */
/*  GE710() Line - arc/curve intersect                              */
/*  GE716() Line - plane intersect                                  */
/*  GE723() Line - surface intersect                                */
/*  GE711() Compute points on a line from a curve/plane intersect   */
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

        DBstatus GE705(
        DBAny  *pstr1,
        DBAny  *pstr2,
        DBTmat *pc,
        short  *pnoint,
        DBfloat u1[],
        DBfloat u2[])

/*      Line/line intersect.
 *
 *      In: pstr1  = First line
 *          pstr2  = Second line
 *          pc     = Active coordinate system
 *          pnoint = Requested solution
 *
 *      Out: *pnoint = Number of intersects
 *           *u1     = Parametric vale first line
 *           *u2     = Parametric value second line
 *
 *      (C)microform ab 18/11/85 J. Kjellander
 *
 *      89/11/20 neg. intnr, J. Kjellander
 *      1999-05-08 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

 {
   DBLine  proj1,proj2;
   DBfloat q1,q2,x1,y1,x2,y2,x3,y3,x4,y4;

/*
***Transform lines to active XY-plane. True projection
***is not needed since line/line intersect is a 2D
***operation.
*/
   GEtfLine_to_local((DBLine *)pstr1,pc,&proj1);
   GEtfLine_to_local((DBLine *)pstr2,pc,&proj2);
   x1 = proj1.crd1_l.x_gm;
   y1 = proj1.crd1_l.y_gm;
   x2 = proj1.crd2_l.x_gm;
   y2 = proj1.crd2_l.y_gm;
   x3 = proj2.crd1_l.x_gm;
   y3 = proj2.crd1_l.y_gm;
   x4 = proj2.crd2_l.x_gm;
   y4 = proj2.crd2_l.y_gm;
/*
***Compute q1 and q2.
*/
    q1 = (x1-x2)*(y4-y3)-(y1-y2)*(x4-x3);
    q2 = (x3-x4)*(y2-y1)-(y3-y4)*(x2-x1);
/*
***Are the lines parallell ?
*/
    if ( ABS(q1) < TOL1 ) return(erpush("GE7152",""));
/*
***Compute the intersect.
*/
    u1[0] = ((x1-x3)*(y4-y3)-(y1-y3)*(x4-x3))/q1;
    u2[0] = ((x3-x1)*(y2-y1)-(y3-y1)*(x2-x1))/q2;
/*
***If *pnoint > 0 line must intersect within ther finite
***length.
*/
   if ( *pnoint > 0 )
     {
     if ( u1[0] < -TOL4 || u1[0] > 1.0+TOL4 ||
          u2[0] < -TOL4 || u2[0] > 1.0+TOL4 )
       {
       *pnoint = 0;
       return(0);
       }
     }
/*
***Otherwise only one solution is possible.
*/
   *pnoint = 1;

   return(0);
  }

/********************************************************************/
/********************************************************************/

      DBstatus GE730(
      DBLine   *plin1,
      DBfloat   offs1,
      DBLine   *plin2,
      DBfloat   offs2,
      DBTmat   *pc,
      DBLine   *ploff1,
      DBLine   *ploff2,
      DBfloat  *pu1,
      DBfloat  *pu2,
      DBVector *ppout)

/*    Line/line intersect in offset.
 *
 *    In: plin1 = Line 1
 *        offs1 = Offset for line 1
 *        plin2 = Line 2
 *        offs2 = Offset for line 2
 *        pc    = The active coordinate system
 *
 *    Out: *ploff1 = Offset line 1
 *         *ploff2 = Offset line 2
 *         *pu1    = Parametric value for ploff1
 *         *pu2    = Parametric value for ploff2
 *         *ppout  = The intersect position in R3
 *
 *    Gunnar Liden 1985-08-11
 *
 *    1986-05-11 extern short, B. Doverud
 *    1999-05-18 Rewritten, J.Kjellander
 *
 ********************************************************************/

 {
   short   noint;         /* The number of intersects (=1)           */
   DBfloat uout1[INTMAX]; /* Array with ordered solutions (u values) */
                          /* for curve 1                             */
                          /* For the line/line intersect only one u  */
   DBfloat uout2[INTMAX]; /* Array with ordered solutions (u values) */
                          /* for curve 2                             */
                          /* For the line/line intersect only one u  */

/*
***Create the offset lines.
*/
   if ( GE202(plin1,offs1,pc,ploff1) < 0 )
     return(erpush("GE7133","GE730(GE202)"));

   if ( GE202(plin2,offs2,pc,ploff2) < 0 )
     return(erpush("GE7133","GE730(GE202)"));
/*
***Computation of the intersect (u-values) for the offset lines.
*/
   noint = -1;

   if ( GE705((DBAny *)ploff1,(DBAny *)ploff2,pc,&noint,uout1,uout2) < 0 )
     return(erpush("GE7133","GE730(GE705)"));      

   ppout->x_gm = ploff1->crd1_l.x_gm + 
                 uout1[0]*(ploff1->crd2_l.x_gm - ploff1->crd1_l.x_gm);
   ppout->y_gm = ploff1->crd1_l.y_gm + 
                 uout1[0]*(ploff1->crd2_l.y_gm - ploff1->crd1_l.y_gm);
   ppout->z_gm = ploff1->crd1_l.z_gm + 
                 uout1[0]*(ploff1->crd2_l.z_gm - ploff1->crd1_l.z_gm);

  *pu1 = uout1[0];
  *pu2 = uout2[0];

   return(0);
 }

/********************************************************************/
/*!******************************************************************/

        DBstatus GE706(
        DBAny  *pstr1,
        DBAny  *pstr2,
        short  *noint,
        DBfloat u1[],
        DBfloat u2[])

/*      Compute intersect between line and arc/circle in 2D.
 *
 *      In: pstr1  => First entity
 *          pstr2  => Second entity
 *          noint  => Requested solution +/-
 *
 *      Out: *noint => Number of solutions
 *           *u1    => Parametric values related to the line
 *           *u2    => Parameteric values related to the arc
 *
 *      (C)microform ab  5/8/85 R. Svedin efter IVAR
 *
 *       12/8/85  Anpassning till v3, J.Kjellander
 *       3/1/86   Tangering, J. Kjellander
 *       15/1/86  Bug lodrät linje, J. Kjellander
 *       13/4/86  Bytt TOL1 mot TOL2, J. Kjellander
 *       19/4/86  Horisontell linje, J. Kjellander
 *       26/11/89 Neg intnr, J. Kjellander
 *       1999-05-10 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

  {
   DBLine *linpek;
   DBArc  *arcpek;
   DBfloat k,k2,p,q,dx,dy;
   DBfloat x1,y1,x2,y2,tt,tmp;
   DBfloat t1[2],t2[2];
   short   i;

/*
***Which is which ?
*/
   if ( pstr1->hed_un.type == LINTYP )
      {
      linpek = (GMLIN *)pstr1;
      arcpek = (GMARC *)pstr2;
      }
   else
      {
      linpek = (GMLIN *)pstr2;
      arcpek = (GMARC *)pstr1;
      }
/*
***Some init.
*/
   dx = linpek->crd2_l.x_gm - linpek->crd1_l.x_gm;
   dy = linpek->crd2_l.y_gm - linpek->crd1_l.y_gm;
/*
***Vertical, horisontal or sloping line ?
*
***Horisontal.
*/
   if ( ABS(dx) > 1000.0*ABS(dy) )
      {
      if ( ABS(linpek->crd1_l.y_gm-arcpek->y_a)-arcpek->r_a  > TOL2 )
         {
         *noint = 0;
         return(0);
         }

      y1 = y2 = linpek->crd1_l.y_gm;

      if ( arcpek->r_a > ABS(linpek->crd1_l.y_gm-arcpek->y_a) )
        {
        x1 = arcpek->x_a + arcpek->r_a*COS(ASIN((linpek->
             crd1_l.y_gm-arcpek->y_a)/arcpek->r_a));
        x2 = 2.0*arcpek->x_a - x1;
        }
      else
        {
        x1 = x2 = arcpek->x_a;
        }

      t1[0] = 1.0 + (x1 - linpek->crd1_l.x_gm) / dx;
      t1[1] = 1.0 + (x2 - linpek->crd1_l.x_gm) / dx;
      }
/*
***Vertical.
*/
   else if ( ABS(dy) > 1000.0*ABS(dx) )
      {
      if ( ABS(linpek->crd1_l.x_gm-arcpek->x_a)-arcpek->r_a  > TOL2 )
         {
         *noint = 0;
         return(0);
         }

      x1 = x2 = linpek->crd1_l.x_gm;

      if ( arcpek->r_a > ABS(linpek->crd1_l.x_gm-arcpek->x_a) )
        {
        y1 = arcpek->y_a + arcpek->r_a*SIN(ACOS((linpek->
             crd1_l.x_gm-arcpek->x_a)/arcpek->r_a));
        y2 = 2.0*arcpek->y_a - y1;
        }
      else
        {
        y1 = y2 = arcpek->y_a;
        }

      t1[0] = 1.0 + (y1 - linpek->crd1_l.y_gm) / dy;
      t1[1] = 1.0 + (y2 - linpek->crd1_l.y_gm) / dy;
      }
/*
***Sloping.
*/
   else
      {
      k  = dy/dx;
      k2 = k*k;

      p = (arcpek->x_a + k2 * linpek->crd1_l.x_gm - k *
          (linpek->crd1_l.y_gm - arcpek->y_a)) / (1.0 + k2);

      q = (arcpek->x_a * arcpek->x_a + k2 * linpek->crd1_l.x_gm *
          linpek->crd1_l.x_gm + 2.0 * k * linpek->crd1_l.x_gm * 
          (arcpek->y_a - linpek->crd1_l.y_gm) + (linpek->crd1_l.y_gm - 
          arcpek->y_a) * (linpek->crd1_l.y_gm - arcpek->y_a) - 
          arcpek->r_a * arcpek->r_a) / (1.0 + k2);
/*
***Do they intersect ?
*/
      if ( (tt=p*p-q) < 0.0 && tt > -TOL2 ) tt = 0.0;
      if ( tt >= 0.0 )
        {
/*
***Yes, analytical solution.
*/
        x1 = p+SQRT(tt);
        y1 = k*(x1-linpek->crd1_l.x_gm) + linpek->crd1_l.y_gm;
        x2 = p-SQRT(tt);     
        y2 = k*(x2-linpek->crd1_l.x_gm) + linpek->crd1_l.y_gm;
        }
/*
***No intersect.
*/
      else
        {
        *noint = 0;
        return(0);
        }
/*
***Compute line parametric values.
*/
      t1[0] = 1.0 + (x1 - linpek->crd1_l.x_gm) / dx;
      t1[1] = 1.0 + (x2 - linpek->crd1_l.x_gm) / dx;
      }
/*
***Compute arc parametric values.
*/
   dx = x1 - arcpek->x_a;
   dy = y1 - arcpek->y_a;
   GE315(arcpek,dx,dy,&t2[0]);

   dx = x2 - arcpek->x_a;
   dy = y2 - arcpek->y_a;
   GE315(arcpek,dx,dy,&t2[1]);
/*
***If *noint > 0, remove intersects outside actual length of entity.
*/
   if ( *noint > 0 )
     {
     *noint = 2;
     if ( t1[0] < 1.0-TOL4 || t1[0] > 2.0+TOL4 ||
          t2[0] < 1.0-TOL4 || t2[0] > 2.0+TOL4 )
       {
       t1[0] = t1[1];
       t2[0] = t2[1];
       *noint = 1;
       }

     if ( t1[1] < 1.0-TOL4 || t1[1] > 2.0+TOL4 ||
          t2[1] < 1.0-TOL4 || t2[1] > 2.0+TOL4 )
       {
       *noint = *noint - 1;
       }
     }
   else *noint = 2;
/*
***Copy remaining solutions to u1 and u2.
*/
   for ( i=0; i<*noint; ++i )
      {
      if ( pstr1->hed_un.type == LINTYP ) 
         {
         u1[i] = t1[i];
         u2[i] = t2[i];
         }
      else
         {
         u1[i] = t2[i];
         u2[i] = t1[i];
         }
      }
/*
***If more than one solution left sort in increasing order
***with respect of the first entity.
*/
   if ( *noint > 1 && u1[0] > u1[1] )
      {
      tmp = u1[0];
      u1[0] = u1[1];
      u1[1] = tmp;

      tmp = u2[0];
      u2[0] = u2[1];
      u2[1] = tmp;
      }

   return(0);
  }

/********************************************************************/
/********************************************************************/

      DBstatus GE710(
      DBAny  *pstr1,
      DBSeg  *pseg1,
      DBAny  *pstr2,
      DBSeg  *pseg2,
      DBTmat *pc,
      short  *pnoint,
      DBfloat uout1[],
      DBfloat uout2[])

/*    Computes the intersects between a line and a arc or curve.
 *
 *      In: pstr1  = First entity
 *          pseg1  = Optional segments
 *          pstr2  = Second entity
 *          pseg2  = Optional segments
 *          pc     = Active coordinate system
 *          pnoint = Requested solution
 *
 *      Out: *pnoint = The number of intersects
 *           uout1[] = Parametric values related to first entity
 *           uout2[] = Parametric values related to second entity
 *
 *      (C)microform ab 1985-02-05 G. Liden
 *
 *      1985-03-03 Decl of GE710 added
 *      1985-07-10 Error system  B. Doverud
 *      1985-08-12 2D-circle J. Kjellander
 *      1985-11-20 Ökat antal omstarter, J. Kjellander
 *      1986-05-11 extern short, B. Doverud
 *      1991-11-20 Nytt format för kurvor, J. Kjellander
 *      1999-05-12 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;       /* Status from called functions           */
   DBetype  typ1;         /* The curve 1 type                       */
   DBetype  typ2;         /* The curve 2 type                       */
   DBLine  *plin;         /* Pointer to the line (= pstr1 or pstr2) */
   DBVector v1,v2,v3;     /* Vectors for the definition of the      */
                          /* intersection plane                     */
   DBfloat  plane[4];     /* The intersection plane nx*X+ny*Y+nz*Z=D*/
   short    noseg;        /* Number of segments in the curve        */
   DBSeg   *pcseg;        /* Pointer to the current rational segment*/
   short    nstart;       /* The number of restarts (=RESTRT except */
                          /* for arcs where nstart=RESTRT+1 )       */
   short    noinse;       /* Number of intersects from one segment  */
   DBfloat  useg[INTMAX]; /* The unordered array of u solutions from*/
                          /* one segment                            */
   short    k;            /* Loop index                             */
   short    noall;        /* The total number of intersects incl.   */
                          /* possible double points ( and before    */
                          /* ordering of the u values )             */
   DBfloat  uglob[INTMAX];/* The global u before ordering           */
   short    i;            /* Loop index                             */
   short    intnr;        /* Önskad skärnings ordningsnr = *pnoint  */

/*
***Determine the curve types
*/
   typ1 = pstr1->poi_un.hed_p.type;
   typ2 = pstr2->poi_un.hed_p.type;
/*
***Save the requested solution number.
*/
   intnr = *pnoint;
/*
***Which of the curves is the line ? "Create" plin and retrive
***the number of segments from the curve.
*/
   if ( typ1 == LINTYP )
     {
     plin = (DBLine *)pstr1;
     if ( typ2 == ARCTYP ) 
       {
       noseg = pstr2->arc_un.ns_a;
       if ( noseg == 0 )                /* 2D-circle */
         {
         if ( GE706(pstr1,pstr2,pnoint,uout1,uout2) < 0 )
           return(erpush("GE7213","GE710"));
         else return(0);
         }
       }
     else noseg = pstr2->cur_un.ns_cu;
     }
   else
     {
     plin = (DBLine *)pstr2;
     if ( typ1 == ARCTYP )
       {
       noseg = pstr1->arc_un.ns_a;
       if ( noseg == 0 )                /* 2D-circle */
         {
         if ( GE706(pstr1,pstr2,pnoint,uout1,uout2) < 0 )
           return(erpush("GE7213","GE710"));
         else return(0);
         }
       }
     else noseg = pstr1->cur_un.ns_cu;
     }
/*
***Create a plane. The line tangent is one vector v1 in the plane.
*/
     v1.x_gm = plin->crd1_l.x_gm - plin->crd2_l.x_gm;
     v1.y_gm = plin->crd1_l.y_gm - plin->crd2_l.y_gm;
     v1.z_gm = plin->crd1_l.z_gm - plin->crd2_l.z_gm;
/*
***The zaxis of the local coordinate system is the other
***vector v2 which defines the plane.
*/
    if ( pc == NULL )
      { v2.x_gm = v2.y_gm = 0.0; v2.z_gm = 1.0; }
    else
      { v2.x_gm = pc->g31; v2.y_gm = pc->g32; v2.z_gm = pc->g33; }
/*
***The cross product of v1 and v2 is the plane normal. If zero,
***the line is perpendicular to the active XY-plane.
*/
    GEvector_product(&v1,&v2,&v3);
    if ( GEnormalise_vector3D(&v3,&v3) < 0 ) return(erpush("GE7222","GE710"));
/*
***Create the plane. The D term is defined with the start point of the line.
*/
    plane[0] = v3.x_gm;
    plane[1] = v3.y_gm;
    plane[2] = v3.z_gm;
    plane[3] = plane[0]*plin->crd1_l.x_gm +
               plane[1]*plin->crd1_l.y_gm +
               plane[2]*plin->crd1_l.z_gm;
/*
***Total number of intersects before sort and removal of 
***multiple points so far is:
*/
    noall = 0;
/*
***Number of restarts.
*/
    nstart = RESTRT+1;     /* 851120JK */

    if      ( typ1 == LINTYP  &&  typ2 == ARCTYP ) ++nstart;
    else if ( typ2 == LINTYP  &&  typ1 == ARCTYP ) ++nstart;
/*
***Start of loop for all segments.
*/
    for ( k=1 ; k <= noseg ; k++ )
      {
      if ( typ1 == LINTYP ) 
        {
        pcseg = (pseg2 + k-1);
        status = GE101(pstr2,pcseg,plane,nstart,&noinse,useg);
        if ( status < 0 ) return(erpush("GE7213","GE710"));
        }
      else
        {
        pcseg = (pseg1 + k-1);
        status = GE101(pstr1,pcseg,plane,nstart,&noinse,useg);
        if ( status < 0 ) return(erpush("GE7213","GE710"));
        }
/*
***Add u values to the global u vector.
*/
      for ( i = noall; i <= noall+noinse-1; i++ )
        uglob[i] = useg[i-noall] + (gmflt)k;
/*
***Increase the number of intersects.
*/
      noall += noinse;
      }
/*
***End loop all segments.
*/
   *pnoint = noall;
   if ( noall == 0 ) return(SUCCED);
/*
***Sort solutions and remove identical points.
*/
   if ( noall > 1 )
     if ( GEsort_1(uglob,noall,pnoint,uglob ) < 0 ) /* 2Xuglob ! */
               return(erpush("GE7213","GE710"));
/*
***uglob now holds parametric values related to the curve.
***Compute the corresponding values for the line. If intnr
***is positive, also remove solutions outside the actual
***line, otherwise use the infinite line.
*/
   if ( typ1 == LINTYP) status = GE711(plin,pstr2,pseg2,pc,*pnoint,uglob,
                                        &intnr,uout2,uout1);
   else                 status = GE711(plin,pstr1,pseg1,pc,*pnoint,uglob,
                                        &intnr,uout1,uout2);
   *pnoint = intnr;

   if (status < 0 )return(erpush("GE7213","GE710"));
/*
***If the other entity was a 3D circle, adjust parameter
***values for true arc length parametrisation. In this case
***this must be done even if the arc was the second entity.
***Both values are used by GE309() for example.
*/
   if ( typ1 == ARCTYP )
     for ( i=0; i< *pnoint; ++i )
       {
       GE311(&pstr1->arc_un,pseg1,uout1[i],&uout1[i]);
       uout1[i] += 1.0;
       }
   else if ( typ2 == ARCTYP )
     for ( i=0; i< *pnoint; ++i )
       {
       GE311(&pstr2->arc_un,pseg2,uout2[i],&uout2[i]);
       uout2[i] += 1.0;
       }
/*
***If the first entity was the arc/curve we are now finished.
*/
   if ( typ2 == LINTYP ) return(0);
/*
***If not, sort the solutions with respect to the line.
*/
   if ( *pnoint > 1 )
     {
     status = GEsort_2(uout1,uout2,*pnoint,pnoint,uout1,uout2);
     if (status < 0 )return(erpush("GE7213","GE710"));
     }

   return(0);
}

/********************************************************************/
/********************************************************************/

      DBstatus GE711(
      DBLine  *plin,
      DBAny   *pstr2,
      DBSeg   *pseg2,
      DBTmat  *pc,
      short   noin,
      DBfloat uinc[],
      short  *pnoout,
      DBfloat uoutc[],
      DBfloat soutl[])

/*    Compute corresponding points on a line from an intersect
 *    with a curve or plane. The line is in the intersecting plane.
 *
 *      In: plin  = The line
 *          pstr2 = The arc/curve
 *          pseg2 = It's segments
 *          pc    = The active coordinate system
 *          noin  = The number of input values
 *          uinc  = The array of input values
 *
 *      Out: *pnoout = The number of output values
 *            uoutc  = The output values for the curve
 *            soutl  = The output values for the line
 *
 *      (C)microform ab 1985-02-06 G. Liden
 *
 *      1985-07-10 Error system  B. Doverud
 *      1986-05-11 extern short, B. Doverud
 *      1990-07-25 Oändl. linjer, J. Kjellander
 *      1991-12-20 Oändl. linjer bara om inr < 0
 *      1996-03-23 Bug segment number calculation Gunnar Liden
 *      1997-01-27 Bug antal segment, J.Kjellander
 *      1999-05-13 Rewritten, J.Kjellander
 *      1999-12-18 sur751->varkon_ctol & surint.h erased  G Liden
 *
 *****************************************************************!*/

 {
   short    status;    /* Function value                         */
   DBTmat   *pcl;      /* Pointer to a transformation matrix      */
                       /* ( = the local ,local coordinate system )*/
   DBTmat   basic;     /* Coordinate system when pc = 0           */
   short    noout;     /* Number of output values (=*pnoout)     */
   short    k;         /* Loop index (all input values )         */
   short    nseg;      /* The number of segments in the arc/cur  */
   short    ipart;     /* The integer part of the input u value  */
                       /* = the adress to the element in the     */
                       /* curve array                            */
   DBfloat  u;         /* Current u value                        */
   DBSeg   *pcseg;     /* The current segment in the curve       */
   DBfloat  pout[16];  /* Output point from GE107               */
   DBfloat  denom1;    /* Denominator for s                      */
   DBfloat  denom2;    /* Denominator for s                      */
   DBfloat  denom3;    /* Denominator for s                      */
   DBfloat  s;         /* Parameter for the line                 */
   DBfloat  dum1,dum2; /* Dummy variables                        */
   DBfloat  dum3;      /* Dummy variable                         */

/*
***We need a coordinate system.
*/
   if ( pc == NULL )
     {
     pcl = &basic;
     pcl->g31 = 0.0;
     pcl->g32 = 0.0;
     pcl->g33 = 1.0;
     }
   else pcl = pc;
/*
***Start number of output points = 0
*/
   noout = 0;

/*
***How many segments in arc/cur ?  1997-01-27 JK
*/
   switch ( pstr2->poi_un.hed_p.type )
     {
     case ARCTYP:
     nseg = pstr2->arc_un.ns_a;
     break;

     case CURTYP:
     nseg = pstr2->cur_un.ns_cu;
     break;

     default:
     return(erpush("GE7993","Unknown type"));
     }
/*
***Loop for all points in the array
*/
   for ( k=1; k <= noin; k++ )
     {
/*
***Adress to the current segment
*/
     ipart = (short)uinc[k-1];
/*
***Current u value
*/
     u = uinc[k-1] - (gmflt)ipart;
/*
***Point on extrapolated curve (end of curve)
*/
     if ( ipart == nseg + 1 )
       {
       pcseg = (pseg2 + ipart - 2);
       status = GE107(pstr2,pcseg,1.0,(short)1,pout);
       if ( status < 0 ) return(erpush("GE7073","GE711(ns_cu)"));
       if ( u*SQRT(pout[3]*pout[3]+pout[4]*pout[4]+pout[5]*pout[5]) 
                 < varkon_ctol() )
         {
         ipart = ipart - (short)1;
         u     = u + 1.0;
         }
       }

     if ( ipart < 1  || ipart > nseg )
       return(erpush("GE7993","GE711(1-ns_cu)"));

     pcseg = (pseg2 + ipart - 1);
/*
***Evaluate the point for the given u on the curve
*/
     status = GE107(pstr2,pcseg,u,(short)0,pout);
     if ( status < 0 ) return(erpush("GE7073","GE711"));
/*
***Denominators (all alternatives) for s
***X1 - X0
*/
     dum1= (*plin).crd2_l.x_gm - (*plin).crd1_l.x_gm;
/*
***Y1 - Z0
*/
     dum2= (*plin).crd2_l.y_gm - (*plin).crd1_l.y_gm;
/*
***Z1 - Z0
*/
     dum3= (*plin).crd2_l.z_gm - (*plin).crd1_l.z_gm;
     denom1 = dum1*(*pcl).g32 - dum2*(*pcl).g31;
     denom2 = dum1*(*pcl).g33 - dum3*(*pcl).g31;
     denom3 = dum2*(*pcl).g33 - dum3*(*pcl).g32;
     s = TOL6; /* For the check of program error */
     if ( ABS(denom1) > ABS(denom2) )
       { /* if1 */
       if ( ABS(denom1) >= ABS(denom3) )
         { /* if2 */
/*
***X(u) - X0
*/
         dum1= pout[0] - (*plin).crd1_l.x_gm;
/*
***Y(u) - Y0
*/
         dum2= pout[1] - (*plin).crd1_l.y_gm;
         s = dum1*(*pcl).g32 - dum2*(*pcl).g31;
         s = s/denom1;
         } /* if2 */
       }    /* if1 */
     if ( ABS(denom2) > ABS(denom1) )
       { /* if3 */
       if ( ABS(denom2) >= ABS(denom3) )
         { /* if4 */
/*
***X(u) - X0
*/
         dum1= pout[0] - (*plin).crd1_l.x_gm;
/*
***Z(u) - Z0
*/
         dum2= pout[2] - (*plin).crd1_l.z_gm;
         s = dum1*(*pcl).g33 - dum2*(*pcl).g31;
         s = s/denom2;
         } /* if4 */
       }    /* if3 */

     if ( ABS(denom3) > ABS(denom1) )
       { /* if5 */
       if ( ABS(denom3) >= ABS(denom2) )
         { /* if6 */
/*
***Y(u) - Y0
*/
         dum1= pout[1] - (*plin).crd1_l.y_gm;
/*
***Z(u) - Z0
*/
         dum2= pout[2] - (*plin).crd1_l.z_gm;
         s = dum1*(*pcl).g33 - dum2*(*pcl).g32;
         s = s/denom3;
         } /* if6 */
       }    /* if5 */

     if ( s == TOL6 ) return(erpush("GE7233","GE711"));
/*
***Kravet att 0<s<1 borttaget 25/7/90 JK.
***Ändrat till villkorligt krav, 20/12/91 JK.
*/
     uoutc[noout] = uinc[k-1];
     soutl[noout] = s + 1.0;
     if ( *pnoout < 0  ||  ( s > 0.0-TOL4  &&  s < 1.0+TOL4 ) ) ++noout;
     } /* End   loop all u values */

  *pnoout = noout;

   return(0);
 }

/********************************************************************/
/*!******************************************************************/

        DBstatus GE716(
        DBLine *lin,
        DBfloat plane[],
        DBfloat uout[])

/*      Compute the intersect between an infinite line and an
 *      infinite plane in 3D.
 *
 *      In: lin   = The line
 *          plane = The plane (canonical form)
 *          uout  = Pekare till utdata.
 *
 *      Out: *uout = Parametric value related to line.
 *
 *      (C)microform ab 27/11/91 J. Kjellander
 *
 *      1999-05-13 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

 {
   DBfloat k1,k2;

   k1 = plane[0] * (lin->crd2_l.x_gm - lin->crd1_l.x_gm) +
        plane[1] * (lin->crd2_l.y_gm - lin->crd1_l.y_gm) +
        plane[2] * (lin->crd2_l.z_gm - lin->crd1_l.z_gm);

   if ( ABS(k1) < 1E-12 ) return(-1);

   k2 = plane[3] - plane[0]*lin->crd1_l.x_gm - 
                   plane[1]*lin->crd1_l.y_gm - 
                   plane[2]*lin->crd1_l.z_gm; 

   uout[0] = 1.0 + (k2/k1);

   return(0);
 }

/********************************************************************/
/*!******************************************************************/

        DBstatus GE723(
        DBLine  *lin,
        DBSurf  *sur,
        DBPatch *pat,
        short   *pnoint,
        DBfloat  uout1[],
        DBfloat  uout2[])

/*      Computes line/surface intersect with varkon_sur_intlinept.
 *      Solutions are returned as coordinates in the surface
 *      UV-plane.
 *
 *      In: 
 *          lin    = The line
 *          sur    = The surface
 *          pat    = Patchdata.
 *          pnoint = +/- requested solution
 *
 *      Out: 
 *         *uout1  = u-coordinates
 *         *uout2  = v-coordinates
 *         *pnoint = Number of intersects
 *
 *      (C)microform ab 7/9/95 J. Kjellander
 *
 *      1996-07-09 Bug, solvec.x, G.Liden
 *      1999-05-17 Rewritten, J.Kjellander
 *      1999-12-18 sur161->varkon_sur_intlinept G. Liden
 *
 ******************************************************************!*/

{
   short    status;
   DBint    i,ocase,scase,acc,sol,nsol;
   DBfloat  linlgt,sollgt;
   DBVector punkt,riktn,start,r3_pt,uv_pt,
            all_uv[SMAX],all_xyz[SMAX],solvec;

/*
***Input to varkon_sur_intlinept (sur161) is surface, 
***point and direction.
*/
   punkt.x_gm = lin->crd1_l.x_gm;
   punkt.y_gm = lin->crd1_l.y_gm;
   punkt.z_gm = lin->crd1_l.z_gm;

   riktn.x_gm = lin->crd2_l.x_gm - lin->crd1_l.x_gm;
   riktn.y_gm = lin->crd2_l.y_gm - lin->crd1_l.y_gm;
   riktn.z_gm = lin->crd2_l.z_gm - lin->crd1_l.z_gm;
/*
***No startpoint.
*/
   start.x_gm =  0.0;
   start.y_gm =  0.0;
   start.z_gm = -1.0;
/*
***Sort (ocase) relative line start.
*/
   ocase = 1;
   scase = 1; if ( *pnoint > 0 ) scase = 2;
   acc   = 1;
   sol   = (DBint)*pnoint; if ( sol < 0 ) sol = -sol;
   nsol  = 0;
/*
***Compute intersects.
*/
   status = varkon_sur_intlinept(sur,pat,&punkt,&riktn,&start,
             ocase,scase,acc,sol,
             &nsol,&r3_pt,&uv_pt,all_uv,all_xyz);
   if ( status < 0 ) return(status);
/*
***If pnoint is > 0 skip solutions outside actual line limits.
***varkon_sur_intlinept only returnes solutions in the line forward dir
*/
   if ( *pnoint > 0 )
     {
     linlgt = GEvector_length3D(&riktn);
     for ( i=nsol-1; i>=0; --i )
       {
       solvec.x_gm = all_xyz[i].x_gm - lin->crd1_l.x_gm;
       solvec.y_gm = all_xyz[i].y_gm - lin->crd1_l.y_gm;
       solvec.z_gm = all_xyz[i].z_gm - lin->crd1_l.z_gm;
       sollgt = GEvector_length3D(&solvec);
       if ( sollgt > linlgt ) --nsol;
       }
     }
/*
***Copy the rest of the solutions to uout1 och uout2.
*/
   for ( i=0; i<nsol; ++i )
     {
     uout1[i] = all_uv[i].x_gm;
     uout2[i] = all_uv[i].y_gm;
     }
/*
***Number of solutions.
*/
  *pnoint = (short)nsol;

   return(0);
}

/********************************************************************/
