/*!******************************************************************/
/*  File: gecurint.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE707() 2D arc - 2D arc intersect                               */
/*  GE708() 2D arc - arc/curve intersect                            */
/*  GE720() Arc/curve - arc/curve intersect                         */
/*  GE732() Arc/curve - arc/curve intersect in offset               */
/*  GE724() Curve - surface intersect                               */
/*  GE715() Line/arc/curve - plane intersect                        */
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

/*!******************************************************************/

        DBstatus GE707(
        DBArc  *arcp1,
        DBArc  *arcp2,
        short  *noint,
        DBfloat u1[],
        DBfloat u2[])

/*      Arc/arc intersect in 2D.
 *
 *      In: arcp1  => First arc
 *          arcp2  => Second arc
 *          noint  => Requested solution +/-
 *
 *      Out: *noint => Number of intersects
 *           *u1    => Parametric values related to arc 1
 *           *u2    => Parametric values related to arc 2
 *
 *      (C)microform ab  6/8/85 R. Svedin
 *
 *      13/8/85  Div, J. Kjellander
 *      29/5/86  Div. med 0 om arc1 = arc2, J. Kjellander
 *      26/11/89 Neg. intnr, J. Kjellander
 *      1999-05-10 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

  {
   gmflt d,f,a;
   gmflt x1,y1,x2,y2,dx,dy;
   gmflt tmp;

/*
***Distance between centers.
*/
   d = sqrt((arcp1->x_a - arcp2->x_a) * (arcp1->x_a - arcp2->x_a) +
       (arcp1->y_a - arcp2->y_a) * (arcp1->y_a - arcp2->y_a));

   if ( d == 0.0 )
      {
      *noint = 0;
      return(0);
      }
/*
***Does a solution exist ?
*/
   if ( arcp1->r_a + arcp2->r_a < d + 0.0001)
      {
      *noint = 0;
      return(0);           /* Too far away */
      }

   if ( arcp2->r_a > arcp1->r_a && arcp2->r_a >= arcp1->r_a +
        d + 0.0001 )
      {
      *noint = 0;
      return(0);           /* 1 is inside 2 */
      }

   if ( arcp1->r_a >= arcp2->r_a && arcp1->r_a >= arcp2->r_a +
        d + 0.0001 )
      {
      *noint = 0;
      return(0);           /* 2 is inside 1 */
      }
/*
***Compute the two intersects.
*/
   a = acos((-(arcp2->r_a * arcp2->r_a) + arcp1->r_a * arcp1->r_a +
       d * d) / (2.0 * arcp1->r_a * d));

   f = acos((arcp2->x_a - arcp1->x_a) / d);
     
   if ( arcp2->y_a - arcp1->y_a < 0.0 ) f = -f;

   x1 = arcp1->x_a + arcp1->r_a * cos(a + f);
   y1 = arcp1->y_a + arcp1->r_a * sin(a + f);
   x2 = arcp1->x_a + arcp1->r_a * cos(-a + f);
   y2 = arcp1->y_a + arcp1->r_a * sin(-a + f);
/*
***Corresponding parametric values.
*/
   dx = x1 - arcp1->x_a;
   dy = y1 - arcp1->y_a;
   GE315(arcp1,dx,dy,&u1[0]);

   dx = x2 - arcp1->x_a;
   dy = y2 - arcp1->y_a;
   GE315(arcp1,dx,dy,&u1[1]);

   dx = x1 - arcp2->x_a;
   dy = y1 - arcp2->y_a;
   GE315(arcp2,dx,dy,&u2[0]);

   dx = x2 - arcp2->x_a;
   dy = y2 - arcp2->y_a;
   GE315(arcp2,dx,dy,&u2[1]);
/*
***If *noint > 0, only solutions actually on are valid.
*/
   if ( *noint > 0 )
     {
     *noint = 2;
     if ( u1[0] < 1.0 || u1[0] > 2.0 || u2[0] < 1.0 || u2[0] > 2.0 )
        {
        u1[0] = u1[1];
        u2[0] = u2[1];
        *noint = *noint - 1;
        }

     if ( u1[1] < 1.0 || u1[1] > 2.0 || u2[1] < 1.0 || u2[1] > 2.0 )
        {
        *noint = *noint - 1;
        }
     }
   else *noint = 2;
/*
***Sort in increasing order with respect to arc 1.
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

      DBstatus GE708(
      DBAny   *pstr1,
      DBSeg   *pseg1,
      DBAny   *pstr2,
      DBSeg   *pseg2,
      DBTmat  *pc,
      short   *pnoint,
      DBfloat  uout1[],
      DBfloat  uout2[])

/*    Intersect between 2D arc and 2D/3D arc or curve.
 *
 *      In:  pstr1  = First entity
 *           pseg1  = Segments
 *           pstr2  = Second entity
 *           pseg2  = Segments
 *           pc     = Active coordinate system
 *           pnoint = Requested solution +/-
 *
 *      Out: *noint = Number of intersects
 *           *uout1 = Parametric values related to first entity
 *           *uout2 = Parametric values related to second entity
 *
 *
 *      (C)microform ab 1985-08-13 J. Kjellander
 *
 *       1999-05-10 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 { 
   DBetype  typ1;        /* The curve 1 type                       */
   DBetype  typ2;        /* The curve 2 type                       */
   DBArc    arc;         /* Temporary 3D-arc                       */
   DBSeg    seg[4];      /* Temporary segments for 3D-arc          */
   DBVector pos;         /* Temporary position for 3D-arc          */
   short    i;           /* Loop-variabel                          */
   DBfloat  v1,v2;       /* Start and end angles                   */
   short    intnr;

/*
***Determine the curve types.
*/
   typ1 = pstr1->poi_un.hed_p.type;
   typ2 = pstr2->poi_un.hed_p.type;
/*
***Perhaps both curves are 2D-circles.
*/
   if ( typ1 == ARCTYP && pstr1->arc_un.ns_a == 0 )
     {
     if ( typ2 == ARCTYP && pstr2->arc_un.ns_a == 0 )
       {
       if ( GE707((GMARC *)pstr1,(GMARC *)pstr2,pnoint,
            uout1,uout2) < 0 ) return(erpush("GE7213","geo707"));
       else return(0);
       }
/*
***First entity is the 2D arc. Second is 3D arc or curve.
***Convert the 2D arc to 3D and use GE720().
*/
    else
      {
      arc.hed_a.type = ARCTYP;
      pos.x_gm = pstr1->arc_un.x_a;
      pos.y_gm = pstr1->arc_un.y_a;
      pos.z_gm = 0.0;

      intnr = *pnoint;

      if ( intnr < 0 )
        { v1 = 0.0; v2 = 360.0; }
      else
        { v1 = pstr1->arc_un.v1_a; v2 = pstr1->arc_un.v2_a; }

      if ( GE300(&pos,pstr1->arc_un.r_a,v1,v2,NULL,&arc,seg,3) < 0 ) 
           return(erpush("GE7213","GE300"));

      if ( GE720((DBAny *)&arc,seg,pstr2,pseg2,pc,pnoint,uout1,uout2) < 0 )
           return(erpush("GE7213","GE720"));
/*
***If intnr < 0 uout1 holds parametric values related to a full
***circle. Adjust if needed.
*/
      for ( i=0; i < *pnoint; ++i )
        {
        if ( intnr < 0 )
          uout1[i] = (uout1[i] - 1.0 - pstr1->arc_un.v1_a/360.0) *
                      360.0 / (pstr1->arc_un.v2_a - pstr1->arc_un.v1_a) + 1.0;
        }
      }
    }
/*
***The second entity is the 2D arc. In this case we don't need
***to adjust parametrisation since parameters from the second
***entity are not used.
*/
   else
     {
     arc.hed_a.type = ARCTYP;
     pos.x_gm = pstr2->arc_un.x_a;
     pos.y_gm = pstr2->arc_un.y_a;
     pos.z_gm = 0.0;

     intnr = *pnoint;

     if ( intnr < 0 )
       { v1 = 0.0; v2 = 360.0; }
     else
       { v1 = pstr2->arc_un.v1_a; v2 = pstr2->arc_un.v2_a; }

     if ( GE300(&pos,pstr2->arc_un.r_a,v1,v2,NULL,&arc,seg,3) < 0 ) 
          return(erpush("GE7213","GE300"));

     if ( GE720(pstr1,pseg1,(DBAny *)&arc,seg,pc,pnoint,uout1,uout2) < 0 )
          return(erpush("GE7213","GE720"));
     }

     return(0);
  }

/********************************************************************/
/********************************************************************/

      DBstatus GE720(
      DBAny  *pstr1,
      DBSeg  *pseg1,
      DBAny  *pstr2,
      DBSeg  *pseg2,
      DBTmat *pc,
      short  *pnoint,
      DBfloat uout1[],
      DBfloat uout2[])
  
/*    Arc/curve intersect with arc/curve.
 *
 *      In:  pstr1 = Curve 1
 *           pseg1 = Its segments
 *           pstr2 = Curve 2
 *           pseg2 = Its segments
 *           pc    = Active coordinate system
 *
 *      Out: *pnoint  = The number of intersects
 *            uout1[] = Parametric values relative to curve 1
 *            uout2[] = Parametric values relative to curve 2
 *
 *      (C)microform ab 1985-02-17 G. Liden
 *
 *      1985-02-27 k changed to j for segm address. + short
 *      1985-05-19 nstart+1 for ARCS
 *      1985-07-10 Error system  B. Doverud
 *      1985-08-14 2D-circle  J. Kjellander
 *      1986-05-11 extern short, B. Doverud
 *      1991-11-20 Nytt format för kurvor, J. Kjellander
 *      1991-12-12 nstart increased with one for arcs G Liden
 *      1999-05-14 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   short    status;        /* Return status                          */
   DBetype  typ1;          /* The curve 1 type                       */
   DBetype  typ2;          /* The curve 2 type                       */
   DBArc   *parc1;         /* If curve 1 or 2 is an ARC type curve   */
   DBCurve *pcur1;         /* If curve 1 or 2 is a  CUR type curve   */
   short    noseg1=0;      /* Number of segments in curve 1          */
   short    noseg2=0;      /* Number of segments in curve 2          */
   DBSeg   *pcseg1;        /* Pointer to the current rational segment*/
                           /* in curve 1                             */
   DBSeg   *pcseg2;        /* Pointer to the current rational segment*/
                           /* in curve 2                             */
   short    nstart;        /* The number of restarts (=RESTRT except */
                           /* for arcs where nstart=RESTRT+1 )       */
   short    noinse;        /* Number of intersects from GE102        */
   DBfloat  us1[3*INTMAX]; /* The unordered array of u solutions from*/
                           /* GE102 ( the curve 1 segment )          */
   DBfloat  us2[3*INTMAX]; /* The unordered array of u solutions from*/
                           /* GE102 ( the curve 2 segment )          */
   short    j;             /* Loop index ( segments in curve 1)      */
   short    k;             /* Loop index ( segments in curve 2)      */
   short    noall;         /* The total number of intersects incl.   */
                           /* possible double points ( and before    */
                           /* ordering of the u values )             */
   short    i;             /* Loop index                             */

   DBfloat  uglob1[GMMXSG*INTMAX];/* The global u before ordering (curve 1)*/
   DBfloat  uglob2[GMMXSG*INTMAX];/* The global u before ordering (curve 2)*/

/*
***Determine the curve types.
*/
   typ1 = pstr1->hed_un.type;
   typ2 = pstr2->hed_un.type;
/*
***Retrieve the number of segments from the curves.
*/
   if ( typ1 == ARCTYP )
      {
      parc1  = (GMARC *)pstr1;
      noseg1 = parc1->ns_a;
      if ( noseg1 == 0 )
         {
         if ( GE708(pstr1,pseg1,pstr2,pseg2,pc,pnoint,
              uout1,uout2) < 0 ) return(erpush("GE7273","GE708"));
         else return(0);
         }
      }
   else if ( typ1 == CURTYP )
       {
       pcur1  = (GMCUR *)pstr1;
       noseg1 = pcur1->ns_cu;
       }

   if ( typ2 == ARCTYP ) 
       {
       parc1  = (GMARC *)pstr2;
       noseg2 = parc1->ns_a;
       if ( noseg2 == 0 )
          {
          if ( GE708(pstr1,pseg1,pstr2,pseg2,pc,pnoint,
               uout1,uout2) < 0 ) return(erpush("GE7273","GE708"));
          else return(SUCCED);
          }
       }
   else if ( typ2 == CURTYP )
       {
       pcur1  = (GMCUR *)pstr2;
       noseg2 = pcur1->ns_cu;
       }
/*
***The total number of intersects before ordering and deletion
***of equal points . Start value = 0.
*/
   noall = 0;
/*
***The number of restarts.
*/
   nstart = RESTRT + 1;
/*
***Some arc/arc cases need an extra restart.
This statement should not be needed any more ?
*/
   if ( typ1 == ARCTYP  &&   typ2  == ARCTYP  &&
        noseg1 == 2  &&  noseg2 == 2 ) nstart = nstart + 1;


/*
Ruvens exempel funkar bättre om man ökar nstart. Med radie = 1001
funkar det dock fortfarande inte ävenb om  nstart=8;
GE102 bör studeras noggrannare. Fallet måste först renodlas bättre
så att det inte blir skärning 4 seg med 4 seg.
*/



/*
***Loop for all the segments in curve 1
*/
   for ( j=1; j <= noseg1; j++ )
     {
/*
***Loop for all the segments in curve 2
*/
     for ( k=1; k <= noseg2; k++ )
       {
/*
***Adress to the current segments
*/
       pcseg1 = (pseg1 + j-1);
       pcseg2 = (pseg2 + k-1);
/*
***Compute the segment/segment intersects.
*/
       status = GE102(pstr1,pcseg1,pstr2,pcseg2,pc,nstart,&noinse,us1,us2);
       if ( status < 0 ) return(erpush("GE7273","geo720"));
/*
***Add u values to the global u vector
*/
       for ( i=noall; i <= noall + noinse-1; i++ )
         {
         uglob1[i] = us1[i-noall] + (gmflt)j;
         uglob2[i] = us2[i-noall] + (gmflt)k;
         }
/*
***Increase the number of intersects
*/
       noall = noall + noinse;
       }
     }
/*
***pnoint = noall if noall <= 1
*/
  *pnoint = noall;

   if ( noall == 0 ) return(0);
/*
***Return arc length parameter values for arcs.
*/
   if ( typ1 == ARCTYP )
     for ( i=0; i<noall; ++i )
       {
       GE311(&pstr1->arc_un,pseg1,uglob1[i],&uglob1[i]);
       uglob1[i] += 1.0;
       }

   if ( typ2 == ARCTYP )
     for ( i=0; i<noall; ++i )
       {
       GE311(&pstr2->arc_un,pseg2,uglob2[i],&uglob2[i]);
       uglob2[i] += 1.0;
       }
/*
***Ordering of the u arrays with respect to the first curve.
*/
   if ( noall > 1 )
     {
     status = GEsort_2(uglob1,uglob2,noall,pnoint,uout1,uout2);
     if (status < 0 )return(erpush("GE7273","geo720"));
     }
   else
    {
/*
***noall = 1
*/
     uout1[0] = uglob1[0];
     uout2[0] = uglob2[0];
     }

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GE732(
      DBAny    *pstr1,
      DBSeg    *pseg1,
      DBfloat   offs1,
      DBAny    *pstr2,
      DBSeg    *pseg2,
      DBfloat   offs2,
      DBTmat   *pc,
      DBfloat  *puc1,
      DBfloat  *puc2,
      DBVector *ppout,
      DBVector *ppcur1,
      DBVector *ppcur2)

/*    Compute arc/curve - arc/curve intersect in offset.
 *
 *    In: pstr1 = Entity 1
 *        pseg1 = Its segments
 *        offs1 = Offset 1
 *        pstr2 = Entity 2
 *        pseg2 = Its segments
 *        offs2 = Offset 2
 *        pc    = Active coordinate system
 *
 *    Out: puc1  = Parametric value for curve 1
 *         puc2  = Parametric value for curve 2
 *         ppout = The point in R3
 *         ppcur1 = The point on the projected curve 1 with offset = 0
 *         ppcur2 = The point on the projected curve 2 with offset = 0
 *
 *     Gunnar Liden 1985-09-22
 *
 *     1986-05-11 extern short, B. Doverud
 *     1993-06-16 Dynamiska segment, J. Kjellander
 *     1994-04-26 Nytt anrop geo813, J. Kjellander
 *     1999-05-18 Rewritten, J.Kjellander
 *
 ********************************************************************/

 {
   short   noint;         /* The number of intersects                */
   short   status;        /* Return status                           */
   DBfloat uout1[INTMAX]; /* Array with ordered solutions (u values) */
                          /* for curve 1                             */
   DBfloat uout2[INTMAX]; /* Array with ordered solutions (u values) */
                          /* for curve 2                             */
   DBfloat  uc1;          /* u value for the curve 1                 */
   DBfloat  uc2;          /* u value for the curve 2                 */
   DBAny    curof1;       /* Offset curve 1                          */
   DBSeg   *segof1;       /* Offset segments curve 1                 */
   DBAny    curof2;       /* Offset curve 2                          */
   DBSeg   *segof2;       /* Offset segments curve 2                 */

/*
***Init segmentptrs.
*/
   segof1 = segof2 = NULL;
/*
***Create the offset curves.
*/
   if ( pstr1->hed_un.type == ARCTYP )
     {
     curof1.arc_un.hed_a.type = ARCTYP;
     if ( pstr1->arc_un.ns_a > 0 )
       segof1 = DBcreate_segments(pstr1->arc_un.ns_a);
     else
       segof1 = NULL;

     if ( GE306((DBArc *)pstr1,pseg1,offs1,pc,(DBArc *)&curof1,segof1) < 0 )
       return(erpush("GE7133","GE732(GE306)"));
     }
   else
     {
     segof1 = DBcreate_segments(pstr1->cur_un.ns_cu);
     if ( GE813(&pstr1->cur_un,pseg1,pseg1,pc,offs1,
                              &curof1.cur_un,segof1,segof1) < 0 )
     return(erpush("GE7133","GE732(geo813)"));
     }

   if ( pstr2->hed_un.type == ARCTYP )
     {
     curof2.arc_un.hed_a.type = ARCTYP;
     if ( pstr2->arc_un.ns_a > 0 )
       segof2 = DBcreate_segments(pstr2->arc_un.ns_a);
     else
       segof2 = NULL;

     if ( GE306((DBArc *)pstr2,pseg2,offs2,pc,(DBArc *)&curof2,segof2) < 0 )
       {
       status = erpush("GE7133","GE732(GE306)");
       goto error;
       }
     }
   else
     {
     segof2 = DBcreate_segments(pstr2->cur_un.ns_cu);
     if ( GE813(&pstr2->cur_un,pseg2,pseg2,pc,offs2,
                                   &curof2.cur_un,segof2,segof2) < 0 )
       {
       status = erpush("GE7133","GE732(geo813)");
       goto error;
       }
     }
/*
***Computation of the intersect (u-values).
*/
   noint = 1;
   if ( GE720(&curof1,segof1,&curof2,segof2,pc,&noint,uout1,uout2) < 0 )
     {
     status = erpush("GE7133","GE732(GE720)");
     goto error;
     }

   if ( noint < 1 )
     {
     status = erpush("GE7133","jk:noint<1");
     goto error;
     }
/*
***The first intersect point is taken.
*/
   uc1 = uout1[0] - 1.0;
   uc2 = uout2[0] - 1.0;
/*
***The intersect point.
*/
   if ( GEposition((DBAny *)&curof1,(char *)segof1,uc1,(gmflt)0.0,ppout) < 0 )
     {
     status = erpush("GE7133","GE732(GEposition)");
     goto error;
     }
/*
***The point on the input curve 1.
*/
   if ( GEposition(pstr1,(char *)pseg1,uc1,(gmflt)0.0,ppcur1) < 0 )
     {
     status = erpush("GE7133","GE732(GEposition)");
     goto error;
     }
/*
***The point on the input curve 2.
*/
   if ( GEposition(pstr2,(char *)pseg2,uc2,(gmflt)0.0,ppcur2) < 0 )
     {
     status = erpush("GE7133","GE732(GEposition)");
     goto error;
     }
/*
***Output parameter values
*/
   *puc1 = uc1;
   *puc2 = uc2;

   status = 0;
/*
***Error exit.
*/
error:
   if ( segof1 != NULL ) DBfree_segments(segof1);
   if ( segof2 != NULL ) DBfree_segments(segof2);

   return(status);
}

/********************************************************************/
/*!******************************************************************/

        DBstatus GE724(
        DBCurve *cur,
        DBSeg   *seg,
        DBSurf  *sur,
        DBPatch *pat,
        short   *pnoint,
        DBfloat  uout1[],
        DBfloat  uout2[])

/*      Computes line/surface intersect with varkon_sur_curint (sur164)
 *      Solutions are returned as coordinates in the surface
 *      UV-plane.
 *
 *      In: 
 *          cur     = The curve
 *          seg     = Its segments
 *          sur     = The surface
 *          pat     = Its patches
 *          pnoint  = +/- Requested intersect
 *
 *      Out:
 *         *uout1   => u-coordinates
 *         *uout2   => v-coordinates
 *         *pnoint  => Number of intersects
 *
 *      1996-01-13 Gunnar Liden
 *
 *      1999-05-18 Rewritten, J.Kjellander
 *      1999-12-18 sur164->varkon_sur_curint G. Liden
 *
 ******************************************************************!*/

{
   short    status;
   DBint    i,ocase,acc,sol,nsol;
   DBVector start,r3_pt,uv_pt,all_uv[SMAX],all_xyz[SMAX];
   DBfloat  all_u[SMAX];
   DBfloat  uvalue;

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
   acc   = 1;
   sol   = (DBint)*pnoint; if ( sol < 0 ) sol = -sol;
   nsol  = 0;
/*
***Compute the intersects.
*/
   status = varkon_sur_curint(sur,pat,cur,seg,&start,ocase,acc,sol,
         &nsol,&r3_pt,&uv_pt,&uvalue,all_u,all_uv,all_xyz);
   if ( status < 0 ) return(status);
/*
***Copy solutions to uout1 och uout2.
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
/*!******************************************************************/

        DBstatus GE715(
        DBAny  *curve,
        DBSeg  *seg,
        DBAny  *plan,
        short  *pnoint,
        DBfloat uout[])

/*      Intersect line, arc or curve with b_plane or XY-plane
 *      of coordinate system.
 *
 *      In: curve  = Pekare till GM-post med linje/arc/kurva.
 *          seg    = Pekare till segment.
 *          plan   = Pekare till GM-post med csys/b_plane.
 *          pnoint = Begärd skärnings ordningsnummer med tecken
 *          uout   = Pekare till skärningar
 *
 *      Ut: *pnoint = Antal skärningar.
 *          *uout   = u-värden sorterade.
 *
 *      (C)microform ab 26/11/91 J. Kjellander efter GE710()
 *
 *      20/12/91 *pnoint < 0, J. Kjellander
 *      17/10/94 Debug tillagt G.Liden        
 *      1999-05-13 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

  {
    short    status,i,k,nstart,nseg,noinse,noall;
    gmflt    plane[4],useg[INTMAX];
    DBVector vx,vy;
    DBTmat   crdmat,invcrd;
    DBTmat  *pc;

/*
***Oavsett om det gäller skärning med koordinatsystem eller
***B_plan använder vi oss av planets ekvation nx*x + ny*y + nz*z = D.
***(nx,ny,nz)  är planets normal och D fås genom att sätta i en känd
***punkt i ekvationen. Om det är ett B-plan skapar vi först ett
***koordinatsystem med GEmktf_3p() och sedan gör vi på samma sätt
***som om det vore ett koordinatsystem det gällde.
*/
   if ( plan->hed_un.type == BPLTYP )
     {
     vx.x_gm = plan->bpl_un.crd2_bp.x_gm - plan->bpl_un.crd1_bp.x_gm;
     vx.y_gm = plan->bpl_un.crd2_bp.y_gm - plan->bpl_un.crd1_bp.y_gm;
     vx.z_gm = plan->bpl_un.crd2_bp.z_gm - plan->bpl_un.crd1_bp.z_gm;
     vy.x_gm = plan->bpl_un.crd4_bp.x_gm - plan->bpl_un.crd1_bp.x_gm;
     vy.y_gm = plan->bpl_un.crd4_bp.y_gm - plan->bpl_un.crd1_bp.y_gm;
     vy.z_gm = plan->bpl_un.crd4_bp.z_gm - plan->bpl_un.crd1_bp.z_gm;
     status = GEmktf_3p(&plan->bpl_un.crd1_bp,&vx,&vy,&crdmat);
     if ( status < 0 )
       {
       *pnoint = 0; 
       return(status);
       }
     pc = &crdmat;
     }
   else pc = &plan->csy_un.mat_pl;
/*
***pc pekar nu på ett koordinatsystem. (nx,ny,nz) är nu detsamma
***som koordinatsystemets Z-axel, och en punkt (0,0,0) fås som
***koordinatsystemets origo.
*/
   plane[0] = pc->g31;
   plane[1] = pc->g32;
   plane[2] = pc->g33;
   GEtform_inv(pc,&invcrd);
   plane[3] = plane[0]*invcrd.g14 +
              plane[1]*invcrd.g24 + plane[2]*invcrd.g34;
/*
***Om det gäller skärning linje/plan använder vi GE716().
***Om den begärda skärningens ordningsnummer är < 0 är alla
***skärningar godkända, annars bara skärningar som verkligen
***ligger på linjen.
*/
   if ( curve->hed_un.type == LINTYP )
     {
     status = GE716((DBLine *)curve,plane,uout);
     if ( status < 0 )
       {
       *pnoint = 0; return(status);
       }
     else
       {
       if ( *pnoint < 0  ||  ( uout[0] > 1.0-TOL4 && uout[0] < 2.0+TOL4 ) )
         *pnoint = 1;
       else
         *pnoint = 0;
       return(0);
       }
     }
/*
***Är det en kurva eller arc använder vi GE101() på liknande
***sätt som GE710(). Antal skärningar före sortering och
***borttagning av identiska punkter är:
*/
    noall = 0;
/*
***I det här läget är det inte tillåtet med negativa ordningsnummer.
*/
    if ( *pnoint < 0 ) return(erpush("GE9992",""));
/*
***Antalet omstarter. Om en cirkel är inblandad skall det
***vara en omstart extra.
*/
   if ( curve->hed_un.type == ARCTYP )
     {
     nstart = RESTRT+2;
     nseg = curve->arc_un.ns_a;
     }
   else
     {
     nstart = RESTRT +1;
     nseg = curve->cur_un.ns_cu;
     }
/*
***Här börjar loopen för alla segment.
*/
    for ( k=0; k<nseg; k++ )
      {
      status = GE101(curve,seg+k,plane,nstart,&noinse,useg);
        if ( status < 0 ) return(status);
/*
***GE101() returnerar ett för segmentet lokalt u-värde.
***Beräkna globalt genom att lägga till segmentets ordnings-
***nummer. 1:a segmentet = 1.
*/
      for ( i=0; i<noinse; i++ ) uout[noall+i] = useg[i] + (gmflt)k + 1.0;
      noall += noinse;
      }
/*
***Här slutar loopen för alla segment. Om det blev mer än en
***skärning, sortera dom.
*/
    *pnoint = noall;
    if ( noall == 0 ) return(SUCCED);

    if ( noall > 1 )
      if ( GEsort_1(uout,noall,pnoint,uout ) < 0 )
        return(erpush("GE7213","GE715"));
/*
***Om kurvan är en 3D-cirkel, returnera båglängd som parameter!
*/
   if ( curve->hed_un.type == ARCTYP )
     for ( i=0; i < *pnoint; ++i )
       {
       GE311(&curve->arc_un,seg,uout[i],&uout[i]);
       uout[i] += 1.0;
       }

    return(0);
}

/********************************************************************/
