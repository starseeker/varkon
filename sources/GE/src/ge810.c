/*!******************************************************************/
/*  File: ge810.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE810() Create Vs_spline free                                   */
/*  GE811() Create Vs_spline projected                              */
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

static short solve(DBVector *pv, DBVector *ts, DBVector *te,
                   short npoi, DBSeg *segmnt);

/*!******************************************************/

        DBstatus GE810(
        DBVector *pv,
        DBVector *tv,
        short     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt)

/*      Create variable stiffness spline. FREE version.
 *
 *      In: pv     = Positions to interpolate
 *          tv     = Optional tangents
 *          npoi   = Number of positions
 *
 *      Out: *curpek = Updated
 *           *segmnt = Segment koefficients
 *
 *      (C)microform ab 2/2/85 J. Kjellander
 *
 *      17/11/85 solve(), J. Kjellander
 *      20/11/91 Nytt format för segment, J. Kjellander
 *      5/32/93  CUB_SEG, J. Kjellander
 *      1999-05-19 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    DBVector ts,te;
    short    i,is,status;

/*
***Number of segments. This is not in general a plane curve.
*/
    curpek->ns_cu    = npoi-1;
    curpek->plank_cu = FALSE;
/*
***Start constraint.
*/
    if ( (tv)->x_gm == 0.0 && (tv)->y_gm == 0.0 && (tv)->z_gm == 0.0 )
      {
      ts.x_gm = (pv+1)->x_gm - (pv)->x_gm;
      ts.y_gm = (pv+1)->y_gm - (pv)->y_gm;
      ts.z_gm = (pv+1)->z_gm - (pv)->z_gm;
      }
    else
      {
      ts.x_gm = (tv)->x_gm;
      ts.y_gm = (tv)->y_gm;
      ts.z_gm = (tv)->z_gm;
      }
/*
***Any optional tangents ?
*/
    is = 0;
    for ( i=1; i<npoi-1; ++i )
       {
       if ( (tv+i)->x_gm != 0.0 || (tv+i)->y_gm != 0.0 ||
            (tv+i)->z_gm != 0.0 )
         {
         te.x_gm = (tv+i)->x_gm;
         te.y_gm = (tv+i)->y_gm;
         te.z_gm = (tv+i)->z_gm;
         if ((status=solve(&pv[is],&ts,&te,i-is+1,&segmnt[is])) < 0 )
            return(status);
         ts.x_gm = te.x_gm;
         ts.y_gm = te.y_gm;
         ts.z_gm = te.z_gm;
         is = i;
         }
       }
/*
***End constraint.
*/
    if ( (tv+npoi-1)->x_gm == 0.0 && (tv+npoi-1)->y_gm == 0.0 && 
         (tv+npoi-1)->z_gm == 0.0 )
      {
      te.x_gm = (pv+npoi-1)->x_gm - (pv+npoi-2)->x_gm;
      te.y_gm = (pv+npoi-1)->y_gm - (pv+npoi-2)->y_gm;
      te.z_gm = (pv+npoi-1)->z_gm - (pv+npoi-2)->z_gm;
      }
    else
      {
      te.x_gm = (tv+npoi-1)->x_gm;
      te.y_gm = (tv+npoi-1)->y_gm;
      te.z_gm = (tv+npoi-1)->z_gm;
      }
/*
***Solve the equation system.
*/
    return(solve(&pv[is],&ts,&te,npoi-is,&segmnt[is]));
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE811(
        DBVector *pv,
        DBVector *tv,
        short     npoi,
        DBTmat   *crdpek,
        DBCurve  *curpek,
        DBSeg    *segmnt)

/*      Create variable stiffness spline. PROJ version.
 *      Positions and tangents are projected to the XY-
 *      plane of the active coordinate system and then
 *      a vs_spline is created by calling GE810().
 *
 *      In: pv     = Positions to interpolate
 *          tv     = Optional tangents
 *          npoi   = Number of positions
 *          crdpek = Active coordinate system
 *
 *      Out: *curpek = Updated
 *           *segmnt = Segment koefficients
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      12/1/86  Tangenter, J. Kjellander
 *      26/11/91 Kurvplan, J. Kjellander
 *      1999-05-19 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    short i,status;

/*
***Project positions and tangents.
*/
    for ( i=0; i<npoi; ++i)
      {
      GEtfpos_to_local( &pv[i], crdpek, &pv[i] );
      GEtfvec_to_local( &tv[i], crdpek, &tv[i] );
      (pv+i)->z_gm = 0.0;
      (tv+i)->z_gm = 0.0;
      GEtfpos_to_basic( &pv[i], crdpek, &pv[i] );
      GEtfvec_to_basic( &tv[i], crdpek, &tv[i] );
      }
/*
***Create the spline.
*/
    if ( (status=GE810(pv,tv,npoi,curpek,segmnt)) < 0 ) return(status);
/*
***The result is a plane curve.
*/
    curpek->plank_cu = TRUE;
    GE814(curpek,crdpek);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus solve(
        DBVector *pv,
        DBVector *ts,
        DBVector *te,
        short     npoi,
        DBSeg    *segmnt)

/*      Compute segment coefficients using the Variable
 *      stiffness spline algorithm. See "Computers in
 *      Mechanical Engineering" November 1983.
 *
 *      The system of equations is solve using Crout:s
 *      method. See Hildebrand, Methods of applied
 *      mathematics.
 *
 *      In: pv     => Pekare till punkt-array
 *          ts     => Pekare till start-tangent
 *          te     => Pekare till slut-tangent
 *          npoi   => Antal punkter
 *          segmnt => Pekare till segment-array
 *
 *      Ut: *segmnt => Segmentkoefficienter
 *
 *      FV:       0 => Ok.
 *           GE8032 => Kan ej mallokera
 *           GE8102 => Otillåtet antal punkter.
 *           GE8112 => Kordalängd = 0
 *
 *      (C)microform ab 17/11/85 J. Kjellander
 *
 *      12/5/86  DBVector tv B. Doverud
 *      20/11/91 Nytt format för segment, J. Kjellander
 *      5/3/93   CUB_SEG, J. Kjellander
 *      1/6/93   Bug max punkter(tvbd), J. Kjellander
 *      15/6/93  malloc, J. Kjellander
 *      16/3/95  v3mall(), J. Kjellander
 *
 ******************************************************!*/

  {

    short     i;
    DBfloat  *datptr;          /* Scratch memory */
    DBfloat  *l;               /* Chord lengths */
    DBfloat  *c;               /* Stiffnesses */
    DBfloat  *rhx;             /* Right-X */
    DBfloat  *rhy;             /* Right-Y */
    DBfloat  *rhz;             /* Right-Z */
    DBfloat  *sm1;             /* System matrix col. 1 */
    DBfloat  *sm2;             /* System matrix col. 2 */
    DBfloat  *sm3;             /* System matrix col. 3 */
    DBfloat   dx,dy,dz;
    DBfloat   c1,c2,c12,c3;
    DBfloat   t0,t1;
    DBVector *tv;

/*
***Check input.
*/
    if ( npoi<2 ) return(erpush("GE8102","solve"));
/*
***Allocate memory.
*/
    if ( (datptr=(DBfloat *)v3mall(11*npoi*sizeof(gmflt),"solve")) == NULL )
      return(erpush("GE8032",""));
/*
***Init pointers.
*/
    l   = datptr; datptr += npoi;
    c   = datptr; datptr += npoi;
    rhx = datptr; datptr += npoi;
    rhy = datptr; datptr += npoi;
    rhz = datptr; datptr += npoi;
    sm1 = datptr; datptr += npoi;
    sm2 = datptr; datptr += npoi;
    sm3 = datptr; datptr += npoi;
    tv  = (DBVector *)datptr;
/*
***Compute chord lengths and stiffnesses.
*/
    for ( i=0; i<npoi-1; ++i)
      {
      dx = (pv+i)->x_gm - (pv+i+1)->x_gm;
      dy = (pv+i)->y_gm - (pv+i+1)->y_gm;
      dz = (pv+i)->z_gm - (pv+i+1)->z_gm;
      if ( (l[i]=SQRT(dx*dx + dy*dy + dz*dz)) - TOL1 < 0.0 )
         return(erpush("GE8112","solve"));
      c[i] = l[i]*l[i]*l[i];
      }
/*
***Configure the right side of the system.
*/
    rhx[0] = ts->x_gm/l[0];
    rhy[0] = ts->y_gm/l[0];
    rhz[0] = ts->z_gm/l[0];

    for ( i=1; i<npoi-1; ++i)
      {
      c1 = c[i-1] * l[i] * l[i];
      c2 = c[i] * l[i-1] * l[i-1];
      c12 = c1-c2;
      c3 = 3.0 / (l[i-1] * l[i]);
      rhx[i] = c3*(c2*(pv+i+1)->x_gm+c12*(pv+i)->x_gm-c1*(pv+i-1)->x_gm);
      rhy[i] = c3*(c2*(pv+i+1)->y_gm+c12*(pv+i)->y_gm-c1*(pv+i-1)->y_gm);
      rhz[i] = c3*(c2*(pv+i+1)->z_gm+c12*(pv+i)->z_gm-c1*(pv+i-1)->z_gm);
      }

    rhx[i] = te->x_gm/l[i-1];
    rhy[i] = te->y_gm/l[i-1];
    rhz[i] = te->z_gm/l[i-1];
/*
***Create the system matrix.
*/
    sm2[0] = 1.0;
    sm3[0] = 0.0;
    for ( i=1; i<npoi-1; ++i)
      {
      sm1[i] = c[i-1]*l[i];
      sm3[i] = c[i]*l[i-1];
      sm2[i] = 2.0*(sm1[i]+sm3[i]);
      }
    sm1[i] = 0.0;
    sm2[i] = 1.0;

    sm3[0] = sm3[0]/sm2[0];
    for ( i=1; i<npoi-1; ++i)
      {
      sm2[i] = sm2[i]-sm1[i]*sm3[i-1];
      sm3[i] = sm3[i]/sm2[i];
      }
    sm2[i] = sm2[i]-sm1[i]*sm3[i-1];
/*
***Solve for X.
*/
   (tv)->x_gm = rhx[0]/sm2[0];

   for ( i=1; i<npoi; ++i)
      (tv+i)->x_gm = (rhx[i]-sm1[i]*(tv+i-1)->x_gm)/sm2[i];

   for ( i=npoi-2; i>=0; --i )
      (tv+i)->x_gm = (tv+i)->x_gm-sm3[i]*(tv+i+1)->x_gm;
/*
***Solve for Y.
*/
   (tv)->y_gm = rhy[0]/sm2[0];

   for ( i=1; i<npoi; ++i)
      (tv+i)->y_gm = (rhy[i]-sm1[i]*(tv+i-1)->y_gm)/sm2[i];

   for ( i=npoi-2; i>=0; --i )
      (tv+i)->y_gm = (tv+i)->y_gm-sm3[i]*(tv+i+1)->y_gm;
/*
***Solve for Z.
*/
   (tv)->z_gm = rhz[0]/sm2[0];

   for ( i=1; i<npoi; ++i)
      (tv+i)->z_gm = (rhz[i]-sm1[i]*(tv+i-1)->z_gm)/sm2[i];

   for ( i=npoi-2; i>=0; --i )
      (tv+i)->z_gm = (tv+i)->z_gm-sm3[i]*(tv+i+1)->z_gm;
/*
***Create the segments.
*/
   for ( i=0; i<npoi-1; ++i)
     {
     t0 = (tv+i)->x_gm*l[i];
     t1 = (tv+i+1)->x_gm*l[i];
     (segmnt+i)->c0x = (pv+i)->x_gm;
     (segmnt+i)->c1x = t0;
     (segmnt+i)->c2x =
        3.0*((pv+i+1)->x_gm-(pv+i)->x_gm)-2.0*t0-t1;
     (segmnt+i)->c3x =
        2.0*((pv+i)->x_gm-(pv+i+1)->x_gm)+t0+t1;

     t0 = (tv+i)->y_gm*l[i];
     t1 = (tv+i+1)->y_gm*l[i];
     (segmnt+i)->c0y = (pv+i)->y_gm;
     (segmnt+i)->c1y = t0;
     (segmnt+i)->c2y =
        3.0*((pv+i+1)->y_gm-(pv+i)->y_gm)-2.0*t0-t1;
     (segmnt+i)->c3y =
        2.0*((pv+i)->y_gm-(pv+i+1)->y_gm)+t0+t1;

     t0 = (tv+i)->z_gm*l[i];
     t1 = (tv+i+1)->z_gm*l[i];
     (segmnt+i)->c0z = (pv+i)->z_gm;
     (segmnt+i)->c1z = t0;
     (segmnt+i)->c2z =
        3.0*((pv+i+1)->z_gm-(pv+i)->z_gm)-2.0*t0-t1;
     (segmnt+i)->c3z =
        2.0*((pv+i)->z_gm-(pv+i+1)->z_gm)+t0+t1;

     (segmnt+i)->c0=1.0;
     (segmnt+i)->c1=0.0;
     (segmnt+i)->c2=0.0;
     (segmnt+i)->c3=0.0;

     (segmnt+i)->typ = CUB_SEG;
     (segmnt+i)->ofs = 0.0;
     }
/*
***Free used memory.
*/
    return(v3free(l,"solve"));
  }

/********************************************************/
