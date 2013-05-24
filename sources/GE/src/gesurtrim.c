/*******************************************************************
*
*    gesurtrim.c
*    ===========
*
*    GEsur_trim_usrdef()     Creates a trimmed surface
*    GEmk_grsurftrim()       Creates trimcurves for graph.surf.
*                                      (openGL)rep
*    GEis_inside_trim()      Inside/outside test
*    GEtrim_loop_direction() Test of trim loop directions
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"
#include <GL/glu.h>

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif



/*****************************************************************************/

    DBstatus GEsur_trim_usrdef(

    DBint      ncur,             /* Number of trimming curves                 */
    DBCurve    cur[NTRIMMAX],    /* Pointer to trimming curves                */
    DBSeg     *pseg[NTRIMMAX],   /* Pointer to trimming curve segments        */
    DBSeg     *pgraseg[NTRIMMAX],/* Pointer to trimming curve graph segments  */
    DBSurf    *psur,             /* C- poniter to surface (in and out)        */
    DBptr      db_sur,           /* DB ptr to input surface (used for check)  */
    DBPatch   *ppat,             /* Pointer to surface patches                */
    DBSegarr **ppborder,         /* Ptr to optional return graph wire rep cvs */
    DBSegarr **ppgetrimcvs)      /* Ptr to return geom trim cvs               */


/*  Creates a trimmed surface
 *
 *
 *  (C) 2007-01-10 Sören L. , Örebro University
 *
 ****************************************************************************!*/

{
  DBint     i,j,n,q;     /* loop variables                                     */
  bool      reusegraphic;  /* if TRUE graphics from input surf can be reused   */
  DBSeg     *segp;         /* pointer to current seg                           */
  EVALC     evldat;        /* For evaluation in GE110()                        */
  DBfloat   idknot;        /* tol for identical pts in parameter space         */
  DBfloat   idknot_sqr;    /* idknot*idknot                                    */
  DBfloat   u1,v1,u2,v2;   /* tmp variables for u/v coordinates                */
  DBSeg     cseg,cseg2;    /* temporary segment for evaluation                 */
  short     noinse;        /* Number of intersects from GE102                  */
  DBfloat   us1[3*INTMAX]; /* The unordered array of u solutions from          */
                           /* GE102 ( the curve 1 segment )                    */
  DBfloat   us2[3*INTMAX]; /* The unordered array of u solutions from          */
                           /* GE102 ( the curve 1 segment )                    */
  DBstatus  status;        /* status from called functions                     */
  DBSegarr  outer_loop;    /* outer loop                                       */
  DBfloat   comptol;
  char      errbuf[133];   /* Error message string                             */


/*
*** Get tolerances
*/
  comptol=varkon_comptol();
  idknot=varkon_idknot();
  idknot_sqr=idknot*idknot;
/*
***Init graphical rep to none.
*/
  psur->ngrwborder_su = 0;
  psur->ngrwiso_su    = 0;
  psur->nku_su        = 0;
  psur->nkv_su        = 0;
/*
*** Test that all segs are UV-segs and that they refer to given surface
*/
  for (i=0; i<ncur; ++i)
    {
    segp = pseg[i];
    for (j=0; j<cur[i].ns_cu; ++j)
      {
      if(segp->typ==CUB_SEG||segp->typ==NURB_SEG)
        {
        sprintf(errbuf, "%d",i+1);
        return(varkon_erpush("SU8502",errbuf));
        }
      if(segp->spek_gm!=db_sur)
        {
        sprintf(errbuf, "%d",i+1);
        return(varkon_erpush("SU8512",errbuf));
        }
      segp++;
      }
    }
/*
*** Test that all curves are loops
*/
  evldat.evltyp = EVC_R;
  for (i=0; i<ncur; ++i)
    {
    cseg = *(pseg[i]); /*first seg*/
    if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
    if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
    evldat.t_local =0.0;
    GE110((DBAny *)&(cur[i]),&cseg,&evldat);
    u1=evldat.r.x_gm; v1=evldat.r.y_gm;

    cseg = *(pseg[i] + cur[i].ns_cu -1); /*last seg*/
    if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
    if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
    evldat.t_local =1.0;
    GE110((DBAny *)&(cur[i]),&cseg,&evldat);
    u2=evldat.r.x_gm; v2=evldat.r.y_gm;

    if ( ((u1-u2)*(u1-u2)+(v1-v2)*(v1-v2)) >idknot_sqr)
      {
      sprintf(errbuf, "%d",i+1);
      return(varkon_erpush("SU8522",errbuf));
      }
    }
/*
*** Test if any loop is self intersecting
*** GE102 can be used if result are further tested to remove detected
*** intersections at segment connection points
*** self intersecting segment will not be detected
***
*** NOT IMPLEMENTED!
*/

/*
*** Test that loops do not intersect each other
*** GE102() will use toleranses suited for R3 curves, but this
*** is ok since it will be a higher demand of uv curve distance
*** wich may be good because openGL use only 32 bit floats and
*** according to documentation requires non intersecting (or touching) 
*** trim loops.
*/
  for (i=0; i<ncur-1; ++i) /* first test curve index */
    {
    for (j=i+1; j<ncur; ++j) /* second curve index */
      {
      for (n=0; n<cur[i].ns_cu; ++n) /* segments in first curve */
        {
        cseg = *(pseg[i] + n);
        if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
        if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
        cseg.sl=0.0; /* not needed now, but if someone change GE102 ... */
        for (q=0; q<cur[j].ns_cu; ++q) /* segments in 2:nd curve */
          {
          cseg2 = *(pseg[j] + q);
          if (cseg2.typ == UV_CUB_SEG) cseg2.typ = CUB_SEG;
          if (cseg2.typ == UV_NURB_SEG)  cseg2.typ = NURB_SEG;
          cseg2.sl=0.0; /* not needed now, but if someone change GE102 ... */
          noinse=0;
          status = GE102((DBAny *)&(cur[i]),&cseg,(DBAny *)&(cur[j]),&cseg2,
                          NULL,RESTRT + 1,&noinse,us1,us2);
          if ( status < 0 ) return(erpush("GE7273","gesur_trim_usrdef")); 
          if (noinse > 0)
            {
            sprintf(errbuf, "%d%%%d",i+1,j+1);
            return(varkon_erpush("SU8532",errbuf));
            }
          }
        }
      }
    }

/*
*** Test that lopp 2 and higher are inside loop 1.
*** Only one pt on inner loops to test (in/out test against
*** outer loop). Because we already know that they do not intersect
*/
  outer_loop.nseg      = cur[0].ns_cu;
  outer_loop.segptr_c  = pseg[0];
  for (i=1; i<ncur; ++i) /* loop 2 and higher */
    {
    cseg = *(pseg[i]);  /* 1:st segment */
    if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
    if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
    evldat.t_local =0.5; /* inside  segment */
    evldat.evltyp = EVC_R;
    GE110((DBAny *)&(cur[i]),&cseg,&evldat);
    u1=evldat.r.x_gm; v1=evldat.r.y_gm;
    status=GEis_inside_trim(evldat.r.x_gm,evldat.r.y_gm,1,&outer_loop);
    if (status == 0)
      {
      sprintf(errbuf, "%d",i+1);
      return(varkon_erpush("SU8542",errbuf));
      }
    else if (status < 0) return(status);
    }

/*
*** Create (allocate) segarr's for geom trim curves,
*** asign existing segment pointers to them and
*** update surface header with number of trim curves.
*/
  *ppgetrimcvs = DBcreate_segarrs(ncur);
  for (i=0; i<ncur; ++i)
    {
    (*ppgetrimcvs)[i].nseg      = cur[i].ns_cu;
    (*ppgetrimcvs)[i].segptr_c  = pseg[i];
    (*ppgetrimcvs)[i].segptr_db = DBNULL;
    }
  psur->ntrim_su = ncur;

/*
*** Test that loops has correct direction 
*/
  status=GEtrim_loop_direction(ncur,*ppgetrimcvs);
  if (status>0) /* wrong direction for curve number: status */
    {
    sprintf(errbuf, "%d",status);
    return(varkon_erpush("SU8552",errbuf));
    }
  else if (status < 0) /* error from GEtrim_loop_direction */
    {
    return(varkon_erpush("SU8563",errbuf));
    }

/*
*** If all input curves contain graph repr. that can be reused,
*** Create (allocate) segarr's for graph, wire rep border curves
*** and asign existing segment pointers to them.
*/
  reusegraphic = TRUE;
  for (i=0; i<ncur; ++i)
    {
    if (cur[i].nsgr_cu == 0) 
      {
      reusegraphic=FALSE;
      break;
      }
    }
  if (reusegraphic)
    {
    *ppborder = DBcreate_segarrs(ncur);
    for (i=0; i<ncur; ++i)
      {
      (*ppborder)[i].nseg      = cur[i].nsgr_cu;
      (*ppborder)[i].segptr_c  = pgraseg[i];
      (*ppborder)[i].segptr_db = DBNULL;
      }
    psur->ngrwborder_su = ncur;
    }
  else /* graphics will not be reused and must be deallocated */
    {
    for (i=0; i<ncur; ++i) 
      {
      if (cur[i].nsgr_cu > 0) DBfree_segments(pgraseg[i]);
      }
    }

  return(0);
}

/*****************************************************************************/



/*****************************************************************************/

    DBstatus GEmk_grsurftrim(
    DBSurf            *psur,         /* Pointer to surface (in and out)      */
    DBSegarr          *pgetrimcvs,   /* Ptr to geom trim cvs                 */
    DBGrstrimcurve   **ppgrstrimcvs) /* Ptr to returned graph. trim curves   */


/*  Creates trim curves for graphical surface (openGL) representation
 *  Memory allocated need to be deallocated by the calling function
 *
 *  (C) 2007-01-15 Sören L. , Örebro University
 *
 ******************************************************************************/

   {
   DBint i,j,s,size,nseg;

   DBint ntrim_su;             /* Number of trim curves (loops)               */
   DBint ncpts;                /* Number of control points                    */
   DBint nknots;               /* Number of knots                             */
   DBint order;                /* NURBS order                                 */
   DBHvector *cpts;            /* Input curve 4D controlpoints if UV_NURB_SEG */
   DBfloat   *knots;           /* Input curve knots if UV_NURB_SEG            */
   DBSeg     *p_seg;           /* Pointer to one curve segment                */
   GLfloat   *knots_gl;        /* openGL knots                                */
   GLfloat   *cpts_gl;         /* openGL cpts                                 */
   bool      ratflag;          /* TRUE=rational curve                         */
   DBfloat   invw;             /* 1/w                                         */


/*
*** Allocate for the surface rep. trimcurves
*/
   ntrim_su=psur->ntrim_su;
   size = ntrim_su * sizeof(DBGrstrimcurve);
   *ppgrstrimcvs = v3mall(size,"SUmk_grsurftrim");

/*
*** Loop through the trim curves
*/
   for (i=0; i < ntrim_su; ++i)
     {
     if (pgetrimcvs[i].segptr_c[0].typ==UV_NURB_SEG)
       {
       cpts    = pgetrimcvs[i].segptr_c[0].cpts_c;
       knots   = pgetrimcvs[i].segptr_c[0].knots_c;
       ncpts   = pgetrimcvs[i].segptr_c[0].ncpts;
       order   = pgetrimcvs[i].segptr_c[0].nurbs_degree+1;
       ratflag = FALSE;
       for (j=1; j < ncpts; ++j)
         {
         if (ABS(cpts[j].w_gm - cpts[0].w_gm) > COMPTOL)
           {
           ratflag=TRUE;
           break;
           }
         }
       }
     else /* This is a UV_CUB_SEG */
       {
       nseg = pgetrimcvs[i].nseg;
       ncpts   = nseg * 3 + 1;
       order   = 4;
       ratflag = FALSE;
       for (s=0; s < nseg; ++s)
         {
         p_seg = &pgetrimcvs[i].segptr_c[s];
         if ( ABS(p_seg->c0 - 1.0) > COMPTOL || ABS(p_seg->c1) > COMPTOL  ||
           ABS(p_seg->c1) > COMPTOL       || ABS(p_seg->c1) > COMPTOL)
           {
           ratflag=TRUE;
           break;
           }
         }
       }
/*
*** Allocate for GL cpts and knots
*/
     if (ratflag) size = ncpts * 3 * sizeof(GLfloat);
     else size = ncpts * 2 * sizeof(GLfloat);
     if( (cpts_gl = v3mall(size,"GEmk_grsurftrim"))==NULL)
        {
        return(erpush("SU8573",""));
        }
     nknots  = ncpts+order;
     size = nknots * sizeof(GLfloat);
     if( (knots_gl = v3mall(size,"GEmk_grsurftrim"))==NULL)
        {
        v3free(cpts_gl);
        return(erpush("SU8573",""));
        }

/*
*** If input is a NURBS curve, transfer values for knots and cpts
*/
     if (pgetrimcvs[i].segptr_c[0].typ==UV_NURB_SEG)
       {
       for (j=0; j < nknots; ++j)  knots_gl[j] = (GLfloat) knots[j];
       if (ratflag)
         {
         for (j=0; j < ncpts; ++j)  
           {
           cpts_gl[j*3  ] = (GLfloat) cpts[j].x_gm;
           cpts_gl[j*3+1] = (GLfloat) cpts[j].y_gm;
           cpts_gl[j*3+2] = (GLfloat) cpts[j].w_gm;
           }
         }
       else /* not rational */
         invw = 1 / cpts[0].w_gm; /* same for all cpts */
         {
         for (j=0; j < ncpts; ++j)  
           {
           cpts_gl[j*2  ] = (GLfloat) (cpts[j].x_gm * invw );
           cpts_gl[j*2+1] = (GLfloat) (cpts[j].y_gm * invw );
           }
         }
       }
/*
*** this is a UV_CUB_SEG, calculations needed for cpts and knots
*/
     else
       {
/*
*** Build knotvector
*/
       j=0;
       knots_gl[0]=0;
       for ( s=0; s <= nseg; s++ )
         {
         knots_gl[j+1] = (GLfloat)s;
         knots_gl[j+2] = (GLfloat)s;
         knots_gl[j+3] = (GLfloat)s;
         j=j+3;
         }
       knots_gl[nknots-1] = knots_gl[nknots-2];
/*
*** Build cpts, see GEsegcpts() for theory, code below asumes the curve is
*** a loop, and also skips z.
*/
       if (ratflag)
         {
         for ( s=0; s < nseg; s++ )
           {
           p_seg = &(pgetrimcvs[i].segptr_c[s]);
           cpts_gl[s*9+0] =                                  p_seg->c0x;
           cpts_gl[s*9+1] =                                  p_seg->c0y;
           cpts_gl[s*9+2] =                                  p_seg->c0;
           cpts_gl[s*9+3] =                 p_seg->c1x/3   + p_seg->c0x;
           cpts_gl[s*9+4] =                 p_seg->c1y/3   + p_seg->c0y;
           cpts_gl[s*9+5] =                 p_seg->c1/3    + p_seg->c0;
           cpts_gl[s*9+6] =  p_seg->c2x/3 + p_seg->c1x*2/3 + p_seg->c0x;
           cpts_gl[s*9+7] =  p_seg->c2y/3 + p_seg->c1y*2/3 + p_seg->c0y;
           cpts_gl[s*9+8] =  p_seg->c2/3  + p_seg->c1*2/3  + p_seg->c0;
           }
         /* last cpt = first cpt */
         cpts_gl[3*(ncpts-1)   ] = cpts_gl[0];
         cpts_gl[3*(ncpts-1) +1] = cpts_gl[1];
         cpts_gl[3*(ncpts-1) +2] = cpts_gl[2];
         }
       else /* not rational */
         {
         for ( s=0; s < nseg; s++ )
           {
           p_seg = &(pgetrimcvs[i].segptr_c[s]);
           cpts_gl[s*6+0] =                                  p_seg->c0x;
           cpts_gl[s*6+1] =                                  p_seg->c0y;
           cpts_gl[s*6+2] =                 p_seg->c1x/3   + p_seg->c0x;
           cpts_gl[s*6+3] =                 p_seg->c1y/3   + p_seg->c0y;
           cpts_gl[s*6+4] =  p_seg->c2x/3 + p_seg->c1x*2/3 + p_seg->c0x;
           cpts_gl[s*6+5] =  p_seg->c2y/3 + p_seg->c1y*2/3 + p_seg->c0y;
           }
         /* last cpt = first cpt */
         cpts_gl[2*(ncpts-1)   ] = cpts_gl[0];
         cpts_gl[2*(ncpts-1) +1] = cpts_gl[1];
         }
       }
/*
*** Asign values to DBGrstrimcurve structure
*/
     (*ppgrstrimcvs+i)->order = order;
     (*ppgrstrimcvs+i)->nknots = nknots;
     (*ppgrstrimcvs+i)->cpts_c = cpts_gl;
     (*ppgrstrimcvs+i)->knots_c = knots_gl;
     if (ratflag) (*ppgrstrimcvs+i)->vertextype = GLU_MAP1_TRIM_3; 
     else         (*ppgrstrimcvs+i)->vertextype = GLU_MAP1_TRIM_2;
     }
   return(0);
   }



/******************************************************************************/

  DBshort GEis_inside_trim (

  DBfloat   u,             /* uv coordinates to test                          */
  DBfloat   v,             /* uv coordinates to test                          */
  DBint     ncur,          /* number of trim curves                           */
  DBSegarr *pgetrimcvs)    /* pointer to trim curves                          */

/*  Determine if a given u/v coordinate is insde or outside the defined
 *  area. Counting of crossings is used.
 *  The algorithm do not care about loop directions
 *
 *  return from function:
 *
 *     0 => outside
 *     1 => on a trim curve
 *     2 => inside
 *
 *    -1 => error
 *
 *
 *  (C) 2007-01-17 Sören L. , Örebro University
 *
 ******************************************************************************/

  {
  short    status;        /* Status from called functions            */
  DBfloat  plane[4];      /* The intersection plane nx*X+ny*Y+nz*Z=D */
  DBfloat  uglob[INTMAX]; /* The global u before ordering            */
  short    noall;         /* The total number of intersects incl.    */
                          /* possible double points ( and before     */
  short    i,j,k;         /* Loop index                              */
  DBSeg    cseg;          /* The current rational segment            */
  short    noinse;        /* Number of intersects from one segment   */
  DBfloat  useg[INTMAX];  /* The unordered array of u solutions from */
                          /* one segment                             */
  DBint    nrestart;      /* number of restarts to turn the plane    */
  DBSegarr *pthiscurve;   /* Pointer to the processed trimcurve      */
  DBCurve  cur;           /* Temporary Curve                         */
  short    noint;         /* number of intersects                    */
  EVALC    evldat;        /* For evaluation in GE110()               */
  DBfloat  move;          /* turn plane by moving v tangent component*/
  DBfloat  idknot;        /* tol for identical pts in parameter space*/
  DBfloat  idknot_sqr;    /* idknot*idknot                           */
  DBfloat  ru,rv;         /* result from evaluation on crossing seg  */
  DBfloat  du,dv;         /* intersection line tangent               */
  DBint    nlower;        /* count of intersect lower than u         */
  DBVector dr;            /* drdt from evaluation on crossing seg    */
/*
***Inittiate and retrieve idknot tolerance.
*/
  idknot=varkon_idknot();
  idknot_sqr=idknot*idknot;
  move=-1;
  nrestart=0;

/*
*** START
***
*** Stepsize for iterations to find a good itersecting plane is choosen to 0.01,
*** i.e 200 iterations will move plane[0] from -1 to 1.
*** Max tilt of the plane will be [1,1] => 45 degrees from the u direction in the 
*** parameter space. This is background to sorting along u should always 
*** be OK to do below.
*/

restart:
  nrestart++;
  noall = 0;

/*
*** Create the plane. The D term is defined with the input u/v coordinates.
*** plane[3] = plane[0]*x +  plane[1]*y +  plane[2]*z
*** in our case plane[0]=move, plane[1]=1, plane[2]=0
*** ... so we can simplify to the code below:
*/
  plane[0] = move; /* =-1 in first iteration */
  plane[1] = 1;
  plane[2] = 0;
  plane[3] = move*u + v;

  du= plane[1];
  dv=-move;

/*
***Start of loop for all curves and all segments.
*/
  cur.hed_cu.type = CURTYP;
  cur.plank_cu    = TRUE;
  for (j=0; j<ncur; ++j)
    {
    pthiscurve      = &(pgetrimcvs[j]);
    cur.al_cu       = 0.0;
    cur.ns_cu       = pthiscurve->nseg;
    for ( k=1 ; k <= pthiscurve->nseg ; k++ )
      {
/*
*** Copy seg data to a temporary seg, as we will otherwise destroy some data
*/
      cseg = *(pthiscurve->segptr_c + k-1);
      if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
      if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
/*
*** we must forget the R3 length as it influence the tolerances
*** adjusted by GE101. Setting it to 0.0 would yield a computation
*** of segment length in uv space. This is not needed, instead we set
*** cseg.sl = 0.5 which will give us a tolerance suited for uv-space.
*/
      cseg.sl = 0.5;
      status = GE101((DBAny *)&cur,&cseg,plane,RESTRT+1,&noinse,useg);
      if ( status < 0 ) return(erpush("GE7213","GEis_inside_trim()"));
/*
*** Evaluate the intersection points to get parameter values in u/v space
*/
      evldat.evltyp = EVC_R + EVC_DR;
      for ( i = 0; i <= noinse-1; i++ )
        {
        evldat.t_local = useg[i];
        GE110((DBAny *)&cur,&cseg,&evldat);
        if (GEnormalise_vector2D(&(evldat.drdt),&dr) != 0) /* normalise failed */
          {
          move+=0.01;
          if (nrestart < 200) goto restart;
          else return(erpush("SU8593",""));
          }
        if (ABS(( du * dr.y_gm - dv * dr.x_gm)) < 0.01 ) /* near paralell */
          {
          move+=0.01;
          if (nrestart < 200) goto restart;
          else return(erpush("SU8593",""));
          }
        ru=evldat.r.x_gm; rv=evldat.r.y_gm;
        if ((u-ru)*(u-ru) + (v-rv)*(v-rv) < idknot_sqr * 2) return(2); /* on curve */
        useg[i]=ru;
        }
/*
*** Add values to the global useg vector.
*/
      for (i=noall; i <= noall+noinse-1; i++) uglob[i] = useg[i-noall];
      noall += noinse;
      }
    }
/*
*** Sort solutions and remove identical points or return(0) if no hits
*/
  if ( noall > 1 ) GEsort_1(uglob,noall,&noint,uglob);  /* 2Xuglob ! */
  else return(0);
/*
*** Check that the number of intersects is even, else restart with turned plane
*** Calculate number of crossings with lower u than our point. 
*** Even or odd determine inside (return 1) or outside (return 0).
*/
  if (noint % 2 == 0)
    {
    nlower=0;
    for (j= 0; j < noint; j++) if (uglob[j]<u)nlower++;
    if (nlower % 2 == 0) return(0); /* outside */
    else return(1);
    }
  else
    {
    move+=0.01;
    if (nrestart < 200) goto restart;
    }
/*
***End, we should never come to here
*/
  return(erpush("SU8583",""));
  }





/******************************************************************************/

  DBshort GEtrim_loop_direction (

  DBint     ncur,          /* number of trim curves to test                   */
  DBSegarr *pgetrimcvs)    /* pointer to trim curves to test                  */

/*  Determine if a set of trim loops has ok directions.
 *  That is: Outer loop is anti-clockwise, and following
 *  nested loops have alternating directions, so that 'material' always
 *  can be found to the left of each loop.
 *
 *  Before this function is called, loops must be tested to be sure:
 *     - all input curves are loops (ie connected start/end)
 *     - no trim loops intersect or touch each other
 *     - First loop is outside loop 2 and hihger
 *  Otherwise the result may be unpredictable
 *
 *  The function can be used to determine the direction of one loop
 *  if ncur==1
 *
 *  return from function:
 *
 *     0         => all direction s is ok
 *               => anti clockwise if ncur==1
 *
 *     1 to ncur => curve number for first found curve with wrong direction
 *               => clockwise if ncur==1
 *
 *     < 0       => error, via varkon_erpush()
 *
 *
 *  (C) 2007-01-19 Sören L. , Örebro University
 *
 ******************************************************************************/

  {
  DBint     i;             /* loop variables                                  */
  short     sr,si;         /* restart counter , seg index                     */
  DBSegarr *pthiscurve;    /* Pointer to the processed trimcurve              */
  DBSeg     cseg;          /* temporary segment for evaluation                */
  EVALC     evldat;        /* For evaluation in GE110()                       */
  DBCurve   cur;           /* temporary curve for evaluation                  */
  DBfloat   u1,v1,u2,v2,ln;/* tmp variables for u/v coordinates               */
  DBfloat   comptol;       /* varkon_comptol()                                */
  char      errbuf[133];   /* Error message string                            */
  DBfloat   idknot;        /* tol for identical pts in parameter space        */
  DBstatus  status;        /* status from called functions                    */

/*
*** Get tolerances, initiate temporary curve structure
*/
  comptol=varkon_comptol();
  idknot=varkon_idknot();
  cur.hed_cu.type = CURTYP;
  cur.plank_cu = TRUE;
/*
*** Test that loops has correct direction by
*** evaluating a point on it, move to outside by use
*** of tangent direction, and make inside/outside test.
*** If problems occur, test on another point.
*** The inside/otside test do not care about loop directons and
*** can thus be used for this.
*/
  for (i=0; i<ncur; ++i) 
    {
    pthiscurve = &(pgetrimcvs[i]);
    cur.ns_cu = pthiscurve->nseg;
    cur.al_cu = 0.0;
    sr=si=0;
/*
*** Restart here with new point if the solution has problems
*/
restart_sr:
    cseg = *(pthiscurve->segptr_c + si);
    if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
    if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
/*
*** Pick a point inside the segment, new param value each
*** time the same segment is revisited
*/
    evldat.t_local = (sr+1) * 0.37 - floor((sr+1) * 0.37);
    evldat.evltyp = EVC_R + EVC_DR;
    GE110((DBAny *)&cur,&cseg,&evldat); /* evaluate */
    u1=evldat.r.x_gm;
    v1=evldat.r.y_gm; 
    u2=evldat.drdt.x_gm;
    v2=evldat.drdt.y_gm;
    ln=SQRT(u2*u2+v2*v2);
/*
*** If length of drdt to small => not good for test => restart
*/
    if (ln < comptol)
      {
      sr++;
      si++; if (si >= cur.ns_cu) si=0; /* first segment again */
      if (sr > 50)  return(varkon_erpush("SU8613","")); /* max iterations */
      goto restart_sr;
      }
/*
*** Normalize derivative and move u/v to empty side
*/
    u2=u2/ln; v2=v2/ln;
    u1+=  v2 * idknot*2;
    v1+= -u2 * idknot*2; 
    status=GEis_inside_trim(u1,v1,ncur,pgetrimcvs);
/*
*** status == 1 => pt inside => curve has wrong dir. => return curve no
*/
    if (status == 1) 
      {
      return(i+1);
      }
/*
*** status == 2 => pt on a trimcurve, not good for test => restart
*/
    if (status == 2) 
      {
      sr++;
      si++; if (si >= cur.ns_cu) si=0;
      if (sr > 50)  return(varkon_erpush("SU8613","")); /* max iterations */
      goto restart_sr;
      }
    else if (status < 0) /* error from GEis_inside_trim */
      {
      return(varkon_erpush("SU8623",errbuf));
      }
    }
/*
*** Directions OK, return 0
*/
  return(0);
  }

