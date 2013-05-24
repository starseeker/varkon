/********************************************************************/
/*                                                                  */
/*  File: sumkgrw.c                                                 */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  SUmk_grwire();  Create graphical wireframe surface rep.         */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL: http://varkon.sourceforge.net                              */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*
***Prototypes for internal funcctions.
*/
static DBstatus split_isocurve(DBint ncur,DBSegarr *pgetrimcvs,
                               DBfloat us[],DBfloat ue[],DBfloat vs[],
                               DBfloat ve[],DBint *nn);

/********************************************************************/
/*                                                                  */
  DBstatus   SUmk_grwire(
  DBSurf    *p_sur,      /* Surface                        (C  ptr) */
  DBPatch   *p_pat,      /* Allocated area for patch data  (C  ptr) */
  DBSegarr  *pgetrimcvs, /* Alloc area for geom trim curves(C  ptr) */
  DBptr      pgm_sur,    /* Surface                        (DB ptr) */
  DBint      g_case,     /* Graphical case (not yet used)           */
  DBSegarr **ppborder,   /* ptr to border curves address            */
  DBSegarr **ppiso)      /* ptr to iso curves address               */

/*  The function creates isoparametric curves for the               */
/*  graphical representation of a surface.                          */
/*                                                                  */
/*                                                                  */
/*  Remark:                                                         */
/*  Note that the calling function must deallocate memory           */
/*  for the created graphical curves (for curves with               */
/*  pointers not equal to NULL).                                    */
/*                                                                  */
/*  Out:   DBSeggar's with Segment data for border and iso curves   */
/*         (arclengths are not calculated for the segments)         */
/*                                                                  */
/*  Error: SU2993 = Unknown type of surface                         */
/*                                                                  */
/*  Author: Johan Kjellander & Gunnar Liden                         */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-03-27   Originally written                                 */
/*  1994-10-29   pgm_sur input to sur150                            */
/*  1996-02-23   Deallocate sur150 memory also when sur150 fails    */
/*               CON_SUR added, unused variables                    */
/*  1997-03-07   Temporary fix for LFT_SUR rotation surface         */
/*  1997-03-12   Check of chord lengths replaces temp. fix          */
/*               Check of surface type                              */
/*  1997-11-28   NURB_SUR added                                     */
/*  1997-12-13   Comments                                           */
/*  1999-12-18   Free source code modifications                     */
/*  2006-11-20   varkon_sur_scur_gra => GEcur_cre_gra, sören L      */
/*  2007-01-08   pborder,piso   Sören L                             */
/*  2007-01-10   rewritten for trimmed surf, Sören L, J.Kjellander  */
/*                                                                  */
/********************************************************************/

 {
   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR  ...     */
   DBfloat start_v;      /* Start parametric value for iso-param.   */
   DBfloat end_v;        /* End   parametric value for iso-param.   */
   DBfloat start_u;      /* Start parametric value for iso-param.   */
   DBfloat end_u;        /* End   parametric value for iso-param.   */
   DBCurve cur;          /* Curve                                   */
   DBSeg  *p_graseg;     /* Graphical (R*3) curve (isoparametric)   */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   i;            /* loop variable                           */
   DBfloat us[MAXISOCVS+MAXISOSPLIT];/* iso curve u start points    */
   DBfloat ue[MAXISOCVS+MAXISOSPLIT];/* iso curve u end points      */
   DBfloat vs[MAXISOCVS+MAXISOSPLIT];/* iso curve v start points    */
   DBfloat ve[MAXISOCVS+MAXISOSPLIT];/* iso curve v end points      */
   DBint   n,nn;         /* counters, number of iso curves          */
   DBint   ntrim;        /* number of trim curves                   */
   DBint   nulines;
   DBint   nvlines;
   DBfloat ratio;
   DBSeg   seg;          /* local tmp seg used for calculations     */
   DBint   nopat;        /* total number of patches in surface      */
   DBint   nc;           /* number of successfully created curves   */
/*
***What type of surface ?
*/
   status = varkon_sur_nopatch (p_sur,&nu,&nv,&surtype);

   if ( surtype     ==  CUB_SUR ||
        surtype     ==  RAT_SUR ||           
        surtype     ==  CON_SUR ||           
        surtype     ==  MIX_SUR ||           
        surtype     ==  POL_SUR ||           
        surtype     ==   P3_SUR ||           
        surtype     ==   P5_SUR ||           
        surtype     ==   P7_SUR ||           
        surtype     ==   P9_SUR ||           
        surtype     ==  P21_SUR ||           
        surtype     == NURB_SUR ||           
        surtype     ==  LFT_SUR )
     {
     ;
     }
   else if ( surtype == FAC_SUR )
     {
     sprintf(errbuf, "(FAC_SUR)%%SUmk_grwire()");
     return(varkon_erpush("SU2993",errbuf));
     }
   else if ( surtype == BOX_SUR )
     {
     sprintf(errbuf, "(BOX_SUR)%%SUmk_grwire()");
     return(varkon_erpush("SU2993",errbuf));
     }
   else
     {
     sprintf(errbuf, "(type)%%SUmk_grwire()");
     return(varkon_erpush("SU2993",errbuf));
     }
/*
*** Initiate
*/
   ntrim   = p_sur->ntrim_su;
   nulines = p_sur->ngu_su;
   nvlines = p_sur->ngv_su;
/*
*** For a nurbs surface the surface uv-span (defined by the knot-
*** vectors) can be retrieved from the start and end topological 
*** patches.
*/
   if ( p_sur->typ_su == NURB_SUR )
     {
     nopat = p_sur->nu_su * p_sur->nv_su;
     start_u = p_pat->us_pat;
     start_v = p_pat->vs_pat;
     end_u =   (p_pat + nopat-1)->ue_pat;
     end_v =   (p_pat + nopat-1)->ve_pat;
     }
   else
     {
     start_v = 1.0;
     end_v   = (DBfloat)nv + 1.0;
     start_u = 1.0;
     end_u   = (DBfloat)nu + 1.0;
     }

   cur.hed_cu.type = CURTYP;
   cur.plank_cu    = FALSE;
   seg.typ         = UV_CUB_SEG;
   seg.spek_gm     = pgm_sur;
   seg.ofs         = 0.0;
/*
***Border curves. For trimmed surfaces we can probably reuse an existing
***graph rep, added by the surface creation routine, otherwise fix it here.
*/
   if ( ntrim  > 0 )
     {
     if (p_sur->ngrwborder_su == 0)
       {
      *ppborder = DBcreate_segarrs(p_sur->ntrim_su);
       nc = 0;
       for (i=0; i<p_sur->ntrim_su; ++i)
         {
         cur.ns_cu = pgetrimcvs[i].nseg;
         cur.al_cu = 0.0;
         status = GEcur_cre_gra (&cur,pgetrimcvs[i].segptr_c,&p_graseg);
         if ( status == 0  &&  cur.nsgr_cu > 0 )
           {
           (*ppborder)[nc].segptr_c = p_graseg;
           (*ppborder)[nc].nseg     = cur.nsgr_cu;
           nc++;
           }
         }
         p_sur->ngrwborder_su = nc;
       }
     }
/*
*** This is an untrimmed surface
*** Create border curves
*/
   else
     {
     us[0] = start_u;
     ue[0] = end_u;
     vs[0] = ve[0] = start_v;

     vs[1] = start_v;
     ve[1] = end_v;
     us[1] = ue[1] = end_u;

     us[2] = end_u;
     ue[2] = start_u;
     vs[2] = ve[2] = end_v;

     vs[3] = end_v;
     ve[3] = start_v;
     us[3] = ue[3] = start_u;

     n=4;
    *ppborder = DBcreate_segarrs(n);
     cur.ns_cu = 1;
     nc = 0;
     for ( i=0; i<n; i++ )
       {
       cur.al_cu = 0.0;
       seg.sl  = 0.0;
       seg.c0x = us[i];
       seg.c0y = vs[i];
       seg.c1x = ue[i] - us[i];
       seg.c1y = ve[i] - vs[i];

       seg.c2x = seg.c3x = 0.0;
       seg.c2y = seg.c3y = 0.0;
       seg.c0z = seg.c1z = seg.c2z = seg.c3z = 0.0;
       seg.c0  = 1.0;
       seg.c1  = seg.c2  = seg.c3  = 0.0;

       cur.al_cu = 0.0;
       status = GEcur_cre_gra (&cur,&seg,&p_graseg);
       if ( status == 0  &&  cur.nsgr_cu > 0 )
         {
         (*ppborder)[nc].nseg = cur.nsgr_cu;
         (*ppborder)[nc].segptr_c = p_graseg;
         nc++;
         }
       }
       p_sur->ngrwborder_su = nc;
     }
/*
***Iso-curves. Calculate iso u and v-lines start / endpoints
*/
   n=0;
   for ( i=0; i<nulines; i++ )
     {
     us[n] = start_u;
     ue[n] = end_u;
     ratio = (DBfloat)(i+1)/(nulines+1);
     vs[n] = ve[n]= start_v * ratio + end_v * (1-ratio);
     if ( ntrim  > 0 ) 
       {
       split_isocurve (ntrim,pgetrimcvs,&(us[n]),&(ue[n]),&(vs[n]),&(ve[n]),&nn);
       n += nn;
       }
     else n++;
     if ( n >= MAXISOCVS ) goto create;
     }

   for ( i=0; i<nvlines; i++ )
     {
     vs[n] = start_v;
     ve[n] = end_v;
     ratio = (DBfloat)(i+1)/(nvlines+1);
     us[n] = ue[n]= start_u * ratio + end_u * (1-ratio);
     if ( ntrim  > 0 ) 
       {
       split_isocurve (ntrim,pgetrimcvs,&(us[n]),&(ue[n]),&(vs[n]),&(ve[n]),&nn);
       n += nn;
       }
     else n++;
     if ( n >= MAXISOCVS ) goto create;
     }
/*
*** Create all iso curves
*/
create:
  *ppiso = DBcreate_segarrs(n);
   cur.ns_cu = 1;
   nc = 0;
   for ( i=0; i<n; i++ )
     {
     cur.al_cu = 0.0;
     seg.sl = 0.0;
     seg.c0x = us[i];
     seg.c0y = vs[i];
     seg.c1x = ue[i] - us[i];
     seg.c1y = ve[i] - vs[i];

     seg.c2x = seg.c3x = 0.0;
     seg.c2y = seg.c3y = 0.0;
     seg.c0z = seg.c1z = seg.c2z = seg.c3z = 0.0;
     seg.c0  = 1.0;
     seg.c1  = seg.c2  = seg.c3  = 0.0;

     cur.al_cu = 0.0;
     status = GEcur_cre_gra (&cur,&seg,&p_graseg);
     if ( status == 0  &&  cur.nsgr_cu > 0 )
       {
       (*ppiso)[nc].nseg = cur.nsgr_cu;
       (*ppiso)[nc].segptr_c = p_graseg;
       nc++;
       }
     }
   p_sur->ngrwiso_su = nc;
/*
***The end.
*/
   return(SUCCED);
   }

/*********************************************************************/
/*********************************************************************/

  static DBstatus split_isocurve (
  DBint     ncur,            /*number of trim curves                 */
  DBSegarr *pgetrimcvs,      /* pointer to trim curves               */
  DBfloat   us[MAXISOSPLIT], /* iso curve u start pts (in and out)   */
  DBfloat   ue[MAXISOSPLIT], /* iso curve u end pts (in and out)     */
  DBfloat   vs[MAXISOSPLIT], /* iso curve v start pts (in and out)   */
  DBfloat   ve[MAXISOSPLIT], /* iso curve v end pts (in and out)     */
  DBint    *nn)              /* number of iso curves after split     */

/*  splits an iso-curve at crossing of trim curves
 *
 *  (C) 2007-01-17 Sören L. , Örebro University
 *
 *********************************************************************/

  {
  bool     is_uline;
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
  DBint    nrestart;      /* number of restarts to move the line     */
  DBSegarr *pthiscurve;   /* Pointer to the processed trimcurve      */
  DBCurve  cur;           /* Temporary Curve                         */
  short    noint;         /* number of intersects                    */
  EVALC    evldat;        /* For evaluation in GE110()               */
  DBfloat  move;          /* moved line to avoid odd no of intersect */
  DBfloat  usin,vsin;     /* Unsplitted line start point             */
  DBfloat  uein,vein;     /* Unsplitted line end point               */
  DBfloat  idknot;        /* tol for identical pts in parameter space*/

/*
***Inittiate and retrieve idknot tolerance.
*/
  idknot=varkon_idknot();
  move=0;
  nrestart=0;

  usin=us[0];
  uein=ue[0];
  vsin=vs[0];
  vein=ve[0];
/*
*** U or V line ?
*/
  if (ABS(vsin - vein) < idknot) is_uline = TRUE;
  else                           is_uline = FALSE;
/*
*** Create a plane normal
*/
  if (is_uline) /* constant v */
    {
    plane[0] = 0;
    plane[1] = 1;
    plane[2] = 0;
    }
  else          /* constant u */
    {
    plane[0] = 1;
    plane[1] = 0;
    plane[2] = 0;
    }

restart:
  noall = 0; /* no off intersects */
/*
*** Create a plane D term, move if requested after restart
*/
  if (is_uline) plane[3] = vsin + move;
  else          plane[3] = usin + move;
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
      cseg = *(pthiscurve->segptr_c + k-1);
      if (cseg.typ == UV_CUB_SEG) cseg.typ = CUB_SEG;
      if (cseg.typ == UV_NURB_SEG)  cseg.typ = NURB_SEG;
/*
*** we must forget the R3 length as it influence the tolerances
*** adjusted by GE101(). Setting it to 0.0 would yield a computation
*** of segment length. This is not needed.
*** cseg.sl = 0.5 will always give us the same tolerance suited for uv.
*/
      cseg.sl = 0.5;
      status = GE101((DBAny *)&cur,&cseg,plane,RESTRT+1,&noinse,useg);
      if ( status < 0 ) return(erpush("GE7213","GE710"));
/*
*** Evaluate the points to get parameter values on the surface
*/
      evldat.evltyp = EVC_R;
      for ( i = 0; i <= noinse-1; i++ )
        {
        evldat.t_local = useg[i];
        GE110((DBAny *)&cur,&cseg,&evldat);
        if(is_uline) useg[i]=evldat.r.x_gm;
        else         useg[i]=evldat.r.y_gm;
        }
/*
***Add values to the global useg vector.
*/
      for (i=noall; i <= noall+noinse-1; i++) uglob[i] = useg[i-noall];
      noall += noinse;
      }
    }
/*
***End loop all segments.
*/
  noint = noall;
  if ( noall == 0 )
    {
    *nn=0;
    goto end;
    }
/*
***(Sort solutions) and remove identical points.
*/
  if ( noall > 1 )
     if ( GEsort_1(uglob,noall,&noint,uglob ) < 0 ) /* 2Xuglob ! */
       return(erpush("GE7213","GE710"));
/*
***Check that the number of intersects is even and return intersect points
*/
  if (noint % 2 == 0)
    {
    if (noint > MAXISOSPLIT*2) noint=MAXISOSPLIT*2;
    *nn=0;
    for (j= 0; 2*j < noint; j++)
      {
      if(is_uline) /* constant v */
        {
        us[j]=uglob[2*j+0];
        ue[j]=uglob[2*j+1];
        vs[j]=ve[j]=ve[0];
        }
      else         /* constant u */
        {
        vs[j]=uglob[2*j+0];
        ve[j]=uglob[2*j+1];
        us[j]=ue[j]=ue[0];
        }
      *nn+=1;
      }
    }
  else /* move the iso line to avoid problems */
    {
    move+=idknot*10;
    nrestart++;
    *nn=0;
    if (nrestart < 50) goto restart;
    }

end:
  return(0);
  }

/**********************************************************************/
