/*!******************************************************************/
/*  File: ge807.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE807() Create Ferguson spline                                  */
/*  GE808() Create Chordlength spline                               */
/*  GE809() Create Variable stiffness spline                        */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

static DBstatus solve_ferguson (DBVector *pv, DBVector *ts, DBVector *te,
                                DBint npoi, DBSeg *segmnt);
static DBstatus solve_chord    (DBVector *pv, DBVector *ts, DBVector *te,
                                DBint npoi, DBSeg *segmnt);
static DBstatus solve_stiffness(DBVector *pv, DBVector *ts, DBVector *te,
                                DBint npoi, DBSeg *segmnt);

/*!******************************************************/

        DBstatus GE807(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt)

/*      Create Ferguson spline with free ends or
 *      optional tangents.
 *
 *      In: pv     = Positions to interpolate
 *          tv     = Optional tangents
 *          npoi   = Number of positions
 *
 *      Out: *curpek = Updated
 *           *segmnt = Segment coefficients
 *
 *      (C)microform ab 1999-10-25 J.Kjellander
 *
 ******************************************************!*/

  {
    DBVector ts,te;
    DBstatus status;
    DBint    i,is;

/*
***Number of segments. This is not in general a plane curve.
*/
    curpek->ns_cu    = npoi-1;
    curpek->plank_cu = FALSE;
/*
***Start constraint.
*/
    if ( tv->x_gm == 0.0 && tv->y_gm == 0.0 && tv->z_gm == 0.0 )
      {
      ts.x_gm = ts.y_gm = ts.z_gm = 0.0;
      }
    else
      {
      ts.x_gm = tv->x_gm;
      ts.y_gm = tv->y_gm;
      ts.z_gm = tv->z_gm;
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
         if ((status=solve_ferguson(&pv[is],&ts,&te,i-is+1,&segmnt[is])) < 0 )
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
      te.x_gm = te.y_gm = te.z_gm = 0.0;
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
    return(solve_ferguson(&pv[is],&ts,&te,npoi-is,&segmnt[is]));
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE808(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt)

/*      Create Chord length spline with free ends or
 *      optional tangents.
 *
 *      In: pv     = Positions to interpolate
 *          tv     = Optional tangents
 *          npoi   = Number of positions
 *
 *      Out: *curpek = Updated
 *           *segmnt = Segment coefficients
 *
 *      (C)microform ab 1999-10-25 J.Kjellander
 *
 ******************************************************!*/

  {
    DBVector ts,te;
    DBstatus status;
    DBint    i,is;

/*
***Number of segments. This is not in general a plane curve.
*/
    curpek->ns_cu    = npoi-1;
    curpek->plank_cu = FALSE;
/*
***Start constraint.
*/
    if ( tv->x_gm == 0.0 && tv->y_gm == 0.0 && tv->z_gm == 0.0 )
      {
      ts.x_gm = ts.y_gm = ts.z_gm = 0.0;
      }
    else
      {
      ts.x_gm = tv->x_gm;
      ts.y_gm = tv->y_gm;
      ts.z_gm = tv->z_gm;
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
         if ((status=solve_chord(&pv[is],&ts,&te,i-is+1,&segmnt[is])) < 0 )
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
      te.x_gm = te.y_gm = te.z_gm = 0.0;
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
    return(solve_chord(&pv[is],&ts,&te,npoi-is,&segmnt[is]));
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE809(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt)

/*      Create variable stiffness spline with free ends.
 *
 *      In: pv     = Positions to interpolate
 *          tv     = Optional tangents
 *          npoi   = Number of positions
 *
 *      Out: *curpek = Updated
 *           *segmnt = Segment coefficients
 *
 *      (C)microform ab 1999-10-25 J. Kjellander
 *
 ******************************************************!*/

  {
    DBVector ts,te;
    DBstatus status;
    DBint    i,is;

/*
***Number of segments. This is not in general a plane curve.
*/
    curpek->ns_cu    = npoi-1;
    curpek->plank_cu = FALSE;
/*
***Start constraint.
*/
    if ( tv->x_gm == 0.0 && tv->y_gm == 0.0 && tv->z_gm == 0.0 )
      {
      ts.x_gm = ts.y_gm = ts.z_gm = 0.0;
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
         if ((status=solve_stiffness(&pv[is],&ts,&te,i-is+1,&segmnt[is])) < 0 )
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
      te.x_gm = te.y_gm = te.z_gm = 0.0;
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
    return(solve_stiffness(&pv[is],&ts,&te,npoi-is,&segmnt[is]));
  }

/********************************************************/
/*!******************************************************/

 static DBstatus solve_ferguson(
        DBVector *pv,
        DBVector *ts,
        DBVector *te,
        DBint     npoi,
        DBSeg    *segmnt)

/*      Compute segment coefficients using the Ferguson
 *      spline algorithm. See "Computational Geometry
 *      for Design and Manufacturing" by Faux and Pratt
 *      page 171. If ts and/or te = (0,0,0) this end is
 *      assumed to be free ie. with curvature = 0.
 *
 *      The system of equations is solve using Crout:s
 *      method. See Hildebrand, Methods of applied
 *      mathematics.
 *
 *      In: pv     => Pointer to array of positions
 *          ts     => Pointer to  start tangent
 *          te     => Pointer to  end tangent
 *          npoi   => Number of points
 *          segmnt => Pointer to array of segments
 *
 *      Out: *segmnt => Segment coefficients
 *
 *      FV:       0 => Ok.
 *           GE8032 => Can't malloc
 *           GE8102 => Too few or too many positions
 *
 *      (C)microform ab 1999-10-25 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i;
    DBfloat  *datptr;          /* Scratch memory */
    DBfloat  *rhx;             /* Right-X */
    DBfloat  *rhy;             /* Right-Y */
    DBfloat  *rhz;             /* Right-Z */
    DBfloat  *sm1;             /* System matrix col. 1 */
    DBfloat  *sm2;             /* System matrix col. 2 */
    DBfloat  *sm3;             /* System matrix col. 3 */
    DBfloat   t0,t1;
    DBVector *tv;

/*
***Check input.
*/
    if ( npoi<2 ) return(erpush("GE8102","solve_ferguson"));
/*
***Allocate memory.
*/
    if ( (datptr=(DBfloat *)v3mall(9*npoi*sizeof(DBfloat),
                  "solve_ferguson")) == NULL ) return(erpush("GE8032",""));
/*
***Init pointers.
*/
    rhx = datptr; datptr += npoi;
    rhy = datptr; datptr += npoi;
    rhz = datptr; datptr += npoi;
    sm1 = datptr; datptr += npoi;
    sm2 = datptr; datptr += npoi;
    sm3 = datptr; datptr += npoi;
    tv  = (DBVector *)datptr;
/*
***Configure the right side of the system for a Ferguson spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      rhx[0] = 3.0*((pv+1)->x_gm - pv->x_gm);
      rhy[0] = 3.0*((pv+1)->y_gm - pv->y_gm);
      rhz[0] = 3.0*((pv+1)->z_gm - pv->z_gm);
      }
    else
      {
      rhx[0] = ts->x_gm;
      rhy[0] = ts->y_gm;
      rhz[0] = ts->z_gm;
      }

    for ( i=1; i<npoi-1; ++i)
      {
      rhx[i] = 3.0*((pv+i+1)->x_gm - (pv+i-1)->x_gm);
      rhy[i] = 3.0*((pv+i+1)->y_gm - (pv+i-1)->y_gm);
      rhz[i] = 3.0*((pv+i+1)->z_gm - (pv+i-1)->z_gm);
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      rhx[i] = 3.0*((pv+npoi-1)->x_gm - (pv+npoi-2)->x_gm);
      rhy[i] = 3.0*((pv+npoi-1)->y_gm - (pv+npoi-2)->y_gm);
      rhz[i] = 3.0*((pv+npoi-1)->z_gm - (pv+npoi-2)->z_gm);
      }
    else
      {
      rhx[i] = te->x_gm;
      rhy[i] = te->y_gm;
      rhz[i] = te->z_gm;
      }
/*
***Create the system matrix for a Ferguson spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      sm2[0] = 2.0;
      sm3[0] = 1.0;
      }
    else
      {
      sm2[0] = 1.0;
      sm3[0] = 0.0;
      }

    for ( i=1; i<npoi-1; ++i)
      {
      sm1[i] = 1.0;
      sm2[i] = 4.0;
      sm3[i] = 1.0;
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      sm1[i] = 1.0;
      sm2[i] = 2.0;
      }
    else
      {
      sm1[i] = 0.0;
      sm2[i] = 1.0;
      }

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
***Create the segments for a Ferguson spline.
*/
   for ( i=0; i<npoi-1; ++i)
     {
     t0 = (tv+i)->x_gm;
     t1 = (tv+i+1)->x_gm;
     (segmnt+i)->c0x = (pv+i)->x_gm;
     (segmnt+i)->c1x = t0;
     (segmnt+i)->c2x = 3.0*((pv+i+1)->x_gm - (pv+i)->x_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3x = 2.0*((pv+i)->x_gm - (pv+i+1)->x_gm) + t0 + t1;

     t0 = (tv+i)->y_gm;
     t1 = (tv+i+1)->y_gm;
     (segmnt+i)->c0y = (pv+i)->y_gm;
     (segmnt+i)->c1y = t0;
     (segmnt+i)->c2y = 3.0*((pv+i+1)->y_gm - (pv+i)->y_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3y = 2.0*((pv+i)->y_gm - (pv+i+1)->y_gm) + t0 + t1;

     t0 = (tv+i)->z_gm;
     t1 = (tv+i+1)->z_gm;
     (segmnt+i)->c0z = (pv+i)->z_gm;
     (segmnt+i)->c1z = t0;
     (segmnt+i)->c2z = 3.0*((pv+i+1)->z_gm - (pv+i)->z_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3z = 2.0*((pv+i)->z_gm - (pv+i+1)->z_gm) + t0 + t1;

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
    return(v3free(rhx,"solve_ferguson"));
  }

/********************************************************/
/*!******************************************************/

 static DBstatus solve_chord(
        DBVector *pv,
        DBVector *ts,
        DBVector *te,
        DBint     npoi,
        DBSeg    *segmnt)

/*      Compute segment coefficients using the Chord-
 *      length spline algorithm.  See "Computational Geometry
 *      for Design and Manufacturing" by Faux and Pratt
 *      chapt. 6.2.2. If ts and/or te = (0,0,0) this end is
 *      assumed to be free ie. with curvature = 0.
 *
 *      The system of equations is solve using Crout:s
 *      method. See Hildebrand, Methods of applied
 *      mathematics.
 *
 *      In: pv     => Pointer to array of positions
 *          ts     => Pointer to  start tangent
 *          te     => Pointer to  end tangent
 *          npoi   => Number of points
 *          segmnt => Pointer to array of segments
 *
 *      Out: *segmnt => Segment coefficients
 *
 *      FV:       0 => Ok.
 *           GE8032 => Can't malloc
 *           GE8102 => Too few or too many positions
 *           GE8112 => Chordlength = 0
 *
 *      (C)microform ab 1999-10-25 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i;
    DBfloat  *datptr;          /* Scratch memory */
    DBfloat  *l;               /* Chord lengths */
    DBfloat  *rhx;             /* Right-X */
    DBfloat  *rhy;             /* Right-Y */
    DBfloat  *rhz;             /* Right-Z */
    DBfloat  *sm1;             /* System matrix col. 1 */
    DBfloat  *sm2;             /* System matrix col. 2 */
    DBfloat  *sm3;             /* System matrix col. 3 */
    DBfloat   dx,dy,dz;
    DBfloat   c1,c2;
    DBfloat   t0,t1;
    DBVector *tv;

/*
***Check input.
*/
    if ( npoi<2 ) return(erpush("GE8102","solve_chord"));
/*
***Allocate memory.
*/
    if ( (datptr=(DBfloat *)v3mall(10*npoi*sizeof(DBfloat),
                 "solve_chord")) == NULL ) return(erpush("GE8032",""));
/*
***Init pointers.
*/
    l   = datptr; datptr += npoi;
    rhx = datptr; datptr += npoi;
    rhy = datptr; datptr += npoi;
    rhz = datptr; datptr += npoi;
    sm1 = datptr; datptr += npoi;
    sm2 = datptr; datptr += npoi;
    sm3 = datptr; datptr += npoi;
    tv  = (DBVector *)datptr;
/*
***Compute chord lengths.
*/
    for ( i=0; i<npoi-1; ++i)
      {
      dx = (pv+i)->x_gm - (pv+i+1)->x_gm;
      dy = (pv+i)->y_gm - (pv+i+1)->y_gm;
      dz = (pv+i)->z_gm - (pv+i+1)->z_gm;
      if ( (l[i]=SQRT(dx*dx + dy*dy + dz*dz)) - TOL1 < 0.0 )
         return(erpush("GE8112","solve_chord"));
      }
/*
***Configure the right side of the system for a chord length spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      rhx[0] = 3.0*((pv+1)->x_gm - pv->x_gm)/l[0];
      rhy[0] = 3.0*((pv+1)->y_gm - pv->y_gm)/l[0];
      rhz[0] = 3.0*((pv+1)->z_gm - pv->z_gm)/l[0];
      }
    else
      {
      rhx[0] = ts->x_gm/l[0];
      rhy[0] = ts->y_gm/l[0];
      rhz[0] = ts->z_gm/l[0];
      }

    for ( i=1; i<npoi-1; ++i)
      {
      c1 = 3.0*l[i]/l[i-1];
      c2 = 3.0*l[i-1]/l[i];
      rhx[i] = c1*((pv+i)->x_gm - (pv+i-1)->x_gm) +
               c2*((pv+i+1)->x_gm - (pv+i)->x_gm);
      rhy[i] = c1*((pv+i)->y_gm - (pv+i-1)->y_gm) +
               c2*((pv+i+1)->y_gm - (pv+i)->y_gm);
      rhz[i] = c1*((pv+i)->z_gm - (pv+i-1)->z_gm) +
               c2*((pv+i+1)->z_gm - (pv+i)->z_gm);
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      rhx[i] = 3.0*((pv+npoi-1)->x_gm - (pv+npoi-2)->x_gm)/l[i-1];
      rhy[i] = 3.0*((pv+npoi-1)->y_gm - (pv+npoi-2)->y_gm)/l[i-1];
      rhz[i] = 3.0*((pv+npoi-1)->z_gm - (pv+npoi-2)->z_gm)/l[i-1];
      }
    else
      {
      rhx[i] = te->x_gm/l[i-1];
      rhy[i] = te->y_gm/l[i-1];
      rhz[i] = te->z_gm/l[i-1];
      }
/*
***Create the system matrix for a chord length spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      sm2[0] = 2.0;
      sm3[0] = 1.0;
      }
    else
      {
      sm2[0] = 1.0;
      sm3[0] = 0.0;
      }

    for ( i=1; i<npoi-1; ++i)
      {
      sm1[i] = l[i];
      sm2[i] = 2.0*(l[i] + l[i-1]);
      sm3[i] = l[i-1];
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      sm1[i] = 1.0;
      sm2[i] = 2.0;
      }
    else
      {
      sm1[i] = 0.0;
      sm2[i] = 1.0;
      }

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
***Create the segments for a chord length spline.
*/
   for ( i=0; i<npoi-1; ++i)
     {
     t0 = (tv+i)->x_gm*l[i];
     t1 = (tv+i+1)->x_gm*l[i];
     (segmnt+i)->c0x = (pv+i)->x_gm;
     (segmnt+i)->c1x = t0;
     (segmnt+i)->c2x = 3.0*((pv+i+1)->x_gm - (pv+i)->x_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3x = 2.0*((pv+i)->x_gm - (pv+i+1)->x_gm) + t0 + t1;

     t0 = (tv+i)->y_gm*l[i];
     t1 = (tv+i+1)->y_gm*l[i];
     (segmnt+i)->c0y = (pv+i)->y_gm;
     (segmnt+i)->c1y = t0;
     (segmnt+i)->c2y = 3.0*((pv+i+1)->y_gm - (pv+i)->y_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3y = 2.0*((pv+i)->y_gm - (pv+i+1)->y_gm) + t0 + t1;

     t0 = (tv+i)->z_gm*l[i];
     t1 = (tv+i+1)->z_gm*l[i];
     (segmnt+i)->c0z = (pv+i)->z_gm;
     (segmnt+i)->c1z = t0;
     (segmnt+i)->c2z = 3.0*((pv+i+1)->z_gm - (pv+i)->z_gm) - 2.0*t0 - t1;
     (segmnt+i)->c3z = 2.0*((pv+i)->z_gm - (pv+i+1)->z_gm) + t0 + t1;

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
    return(v3free(l,"solve_chord"));
  }

/********************************************************/
/*!******************************************************/

 static DBstatus solve_stiffness(
        DBVector *pv,
        DBVector *ts,
        DBVector *te,
        DBint     npoi,
        DBSeg    *segmnt)

/*      Compute segment coefficients using the Variable
 *      stiffness spline algorithm. See "Computers in
 *      Mechanical Engineering" November 1983. If ts 
 *      and/or te = (0,0,0) this end is assumed to be
 *      free ie. with curvature = 0.
 *
 *      The system of equations is solve using Crout:s
 *      method. See Hildebrand, Methods of applied
 *      mathematics.
 *
 *      In: pv     => Pointer to array of positions
 *          ts     => Pointer to  start tangent
 *          te     => Pointer to  end tangent
 *          npoi   => Number of points
 *          segmnt => Pointer to array of segments
 *
 *      Out: *segmnt => Segment coefficients
 *
 *      FV:       0 => Ok.
 *           GE8032 => Can't malloc
 *           GE8102 => Too few or too many positions
 *           GE8112 => Chordlength = 0
 *
 *      (C)microform ab 1999-10-25 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i;
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
    if ( npoi<2 ) return(erpush("GE8102","solve_stiffness"));
/*
***Allocate memory.
*/
    if ( (datptr=(DBfloat *)v3mall(11*npoi*sizeof(DBfloat),
                 "solve_stiffness")) == NULL ) return(erpush("GE8032",""));
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
         return(erpush("GE8112","solve_stiffness"));
      c[i] = l[i]*l[i]*l[i];
      }
/*
***Configure the right side of the system for a stiffness spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      rhx[0] = 3.0*((pv+1)->x_gm - pv->x_gm)/l[0];
      rhy[0] = 3.0*((pv+1)->y_gm - pv->y_gm)/l[0];
      rhz[0] = 3.0*((pv+1)->z_gm - pv->z_gm)/l[0];
      }
    else
      {
      rhx[0] = ts->x_gm/l[0];
      rhy[0] = ts->y_gm/l[0];
      rhz[0] = ts->z_gm/l[0];
      }

    for ( i=1; i<npoi-1; ++i)
      {
      c1 = c[i-1]*l[i]*l[i];
      c2 = c[i]*l[i-1]*l[i-1];
      c12 = c1 - c2;
      c3 = 3.0/(l[i-1]*l[i]);
      rhx[i] = c3*(c2*(pv+i+1)->x_gm+c12*(pv+i)->x_gm-c1*(pv+i-1)->x_gm);
      rhy[i] = c3*(c2*(pv+i+1)->y_gm+c12*(pv+i)->y_gm-c1*(pv+i-1)->y_gm);
      rhz[i] = c3*(c2*(pv+i+1)->z_gm+c12*(pv+i)->z_gm-c1*(pv+i-1)->z_gm);
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      rhx[i] = 3.0*((pv+npoi-1)->x_gm - (pv+npoi-2)->x_gm)/l[i-1];
      rhy[i] = 3.0*((pv+npoi-1)->y_gm - (pv+npoi-2)->y_gm)/l[i-1];
      rhz[i] = 3.0*((pv+npoi-1)->z_gm - (pv+npoi-2)->z_gm)/l[i-1];
      }
    else
      {
      rhx[i] = te->x_gm/l[i-1];
      rhy[i] = te->y_gm/l[i-1];
      rhz[i] = te->z_gm/l[i-1];
      }
/*
***Create the system matrix for a stiffness spline.
*/
    if ( ts->x_gm == 0.0  &&  ts->y_gm == 0.0  &&  ts->z_gm == 0.0 )
      {
      sm2[0] = 2.0;
      sm3[0] = 1.0;
      }
    else
      {
      sm2[0] = 1.0;
      sm3[0] = 0.0;
      }

    for ( i=1; i<npoi-1; ++i)
      {
      sm1[i] = c[i-1]*l[i];
      sm3[i] = c[i]*l[i-1];
      sm2[i] = 2.0*(sm1[i]+sm3[i]);
      }

    if ( te->x_gm == 0.0  &&  te->y_gm == 0.0  &&  te->z_gm == 0.0 )
      {
      sm1[i] = 1.0;
      sm2[i] = 2.0;
      }
    else
      {
      sm1[i] = 0.0;
      sm2[i] = 1.0;
      }

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
***Create the segments for a stiffness spline.
*/
   for ( i=0; i<npoi-1; ++i)
     {
     t0 = (tv+i)->x_gm*l[i];
     t1 = (tv+i+1)->x_gm*l[i];
     (segmnt+i)->c0x = (pv+i)->x_gm;
     (segmnt+i)->c1x = t0;
     (segmnt+i)->c2x = 3.0*((pv+i+1)->x_gm-(pv+i)->x_gm)-2.0*t0-t1;
     (segmnt+i)->c3x = 2.0*((pv+i)->x_gm-(pv+i+1)->x_gm)+t0+t1;

     t0 = (tv+i)->y_gm*l[i];
     t1 = (tv+i+1)->y_gm*l[i];
     (segmnt+i)->c0y = (pv+i)->y_gm;
     (segmnt+i)->c1y = t0;
     (segmnt+i)->c2y = 3.0*((pv+i+1)->y_gm-(pv+i)->y_gm)-2.0*t0-t1;
     (segmnt+i)->c3y = 2.0*((pv+i)->y_gm-(pv+i+1)->y_gm)+t0+t1;

     t0 = (tv+i)->z_gm*l[i];
     t1 = (tv+i+1)->z_gm*l[i];
     (segmnt+i)->c0z = (pv+i)->z_gm;
     (segmnt+i)->c1z = t0;
     (segmnt+i)->c2z = 3.0*((pv+i+1)->z_gm-(pv+i)->z_gm)-2.0*t0-t1;
     (segmnt+i)->c3z = 2.0*((pv+i)->z_gm-(pv+i+1)->z_gm)+t0+t1;

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
    return(v3free(l,"solve_stiffness"));
  }

/********************************************************/
