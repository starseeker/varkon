/*!******************************************************************/
/*  File: getfents.c                                                */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*     GEtfPoint_to_local  Transform point from basic to local      */
/*     GEtfLine_to_local   Transform line from basic to local       */
/*     GEtfLine_to_basic   Transform line from local to basic       */
/*     GEtfArc_to_local    Transform arc from basic to local        */
/*     GEtfCurve_to_local  Transform curve from basic to local      */
/*     GEtfCsys_to_local   Transform csys from basic to local       */
/*     GEtfText_to_local   Transform text from basic to local       */
/*     GEtfLdim_to_local   Transform linear dimension to local      */
/*     GEtfCdim_to_local   Transform diameter dimension to local    */
/*     GEtfRdim_to_local   Transform radius dimension to local      */
/*     GEtfAdim_to_local   Transform angular dimension to local     */
/*     GEtfHatch_to_local  Transform hatch from basic to local      */
/*     GEtfSurf_to_local   Transform surface from basic to local    */
/*     GEtfBplane_to_local Transform B_plane from basic to local    */
/*     GEtfMesh_to_local   Transform mesh from basic to local       */
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


/* Compute 2D-determinant and X-scale */
static short calds2(DBTmat *ptr, DBfloat *pdet, DBfloat *psx);

/*!******************************************************/

        DBstatus GEtfPoint_to_local(
        DBPoint *opoi,
        DBTmat  *pt,
        DBPoint *npoi)

/*      Transforms the point with t.
 *      (from basic to local)
 *
 *      In: opoi  = Original point
 *          ptr   = Transformation
 *
 *      Out: *npoi = Transformed point
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *      1999-12-18 surxxx->varkon_sur_.. Gunnar Liden
 *
 ******************************************************!*/

  {
/*
***Transform the coordinates.
*/
   GEtfpos_to_local(&opoi->crd_p,pt,&npoi->crd_p);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfLine_to_local(
        DBLine   *plin,
        DBTmat   *pt,
        DBLine   *plout)

/*      Transforms the input line with t.
 *      (from basic to local)
 *
 *      In: plin = Original line
 *          pt   = Transformation
 *
 *      Out: *plout = Transformed line
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***No transformation if pt = NULL.
*/
   if ( pt == NULL )
     {
     if ( plin != plout )
       {
       plout->crd1_l.x_gm =  plin->crd1_l.x_gm;
       plout->crd1_l.y_gm =  plin->crd1_l.y_gm;
       plout->crd1_l.z_gm =  plin->crd1_l.z_gm;
       plout->crd2_l.x_gm =  plin->crd2_l.x_gm;
       plout->crd2_l.y_gm =  plin->crd2_l.y_gm;
       plout->crd2_l.z_gm =  plin->crd2_l.z_gm;
       }
     return(0);
     }
/*
***Transform the end points to the local system
*/
   GEtfpos_to_local(&plin->crd1_l,pt,&plout->crd1_l);
   GEtfpos_to_local(&plin->crd2_l,pt,&plout->crd2_l);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfLine_to_basic(
        DBLine   *plin,
        DBTmat   *pt,
        DBLine   *plout)

/*      Transforms the input line with t inverted.
 *      (from local to basic)
 *
 *      In: plin = Original line
 *          pt   = Transformation
 *
 *      Out: *plout = Transformed line
 *
 *      (C)microform ab 1984-12-26 G.Liden
 *
 *      1999-03-04 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***No transformation if pt = NULL.
*/
   if ( pt == NULL )
     {
     if ( plin != plout )
       {
       plout->crd1_l.x_gm =  plin->crd1_l.x_gm;
       plout->crd1_l.y_gm =  plin->crd1_l.y_gm;
       plout->crd1_l.z_gm =  plin->crd1_l.z_gm;
       plout->crd2_l.x_gm =  plin->crd2_l.x_gm;
       plout->crd2_l.y_gm =  plin->crd2_l.y_gm;
       plout->crd2_l.z_gm =  plin->crd2_l.z_gm;
       }
     return(0);
     }
/*
***Transform the end points to BASIC.
*/
   GEtfpos_to_basic(&plin->crd1_l,pt,&plout->crd1_l);
   GEtfpos_to_basic(&plin->crd2_l,pt,&plout->crd2_l);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfArc_to_local(
        DBArc  *oarc,
        DBSeg   oseg[],
        DBTmat *pt,
        DBArc  *narc,
        DBSeg   nseg[])

/*      Transforms the input arc and optional segments with t.
 *      (from basic to local)
 *
 *      In: oarc  = Original arc
 *          oseg  = Original segments
 *          pt    = Transformation
 *
 *      Out: *narc = Transformed arc
 *           *nseg = Transformed segments
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      24/2/93 Spegling 2D, J. Kjellander
 *      1999-04-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
      int      i;
      DBfloat  det,sx,vs;
      DBVector oldorg,neworg;
      DBTmat   tmp;

/*
***2D. Transform the origin.
*/
     if ( oarc->ns_a == 0 )
        {
        oldorg.x_gm = oarc->x_a;
        oldorg.y_gm = oarc->y_a;
        oldorg.z_gm = 0.0;
        GEtfpos_to_local(&oldorg,pt,&neworg);
        narc->x_a = neworg.x_gm;
        narc->y_a = neworg.y_gm;
/*
***Transform angles.
*/
        GEtform_inv(pt,&tmp);
        GEtfang_to_basic(oarc->v1_a,&tmp,&narc->v1_a);
        GEtfang_to_basic(oarc->v2_a,&tmp,&narc->v2_a);
/*
***Compute transformation determinant and scalefactor.
*/
        calds2(pt,&det,&sx);
/*
***If det < 0 the transformation includes mirroring. In that
***case angles need to be adjusted.
*/
        if ( det < 0.0 )
          {
          vs = narc->v1_a - oarc->v1_a;
          narc->v1_a = vs - oarc->v2_a;
          narc->v2_a = vs - oarc->v1_a;
          }
/*
***Normalize angles.
*/
        GE312(&narc->v1_a,&narc->v2_a);
/*
***Scale the radius.
*/
        if ( ABS(sx - 1.0) > 1E-14 ) narc->r_a *= sx;
        }
/*
***3D, transform segments. 2D-data is not currently handled ????
*/
     else
        {
        for ( i=0; i<oarc->ns_a; ++i ) GEtfseg_to_local(&oseg[i],pt,&nseg[i]);
        }

      return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  GEtfCurve_to_local(
        DBCurve *ocur,
        DBSeg    osge[],
        DBSeg    osgr[],
        DBTmat  *pt,
        DBCurve *ncur,
        DBSeg    nsge[],
        DBSeg    nsgr[])

/*      Transforms the input curve and optional segments with t.
 *      (from basic to local)
 *
 *      In: ocur  = Original curve
 *          osge  = Original geometric segments
 *          osgr  = Original graphical segments
 *          pt    = Transformation
 *
 *      Out: *ncur = Transformed curve
 *           *nsge = Transformed geometric segments
 *           *nsgr = Transformed graphical segments
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      5/12/94  GE6282, J. Kjellander
 *      7/03/94  Plan kurva, G. Liden
 *      17/3/95  Plan kurva, J. Kjellander
 *      1999-04-27 Rewritten, J.Kjellander
 *      2006-11-08 Transform NURBS, Sören Larsson
 *
 ******************************************************!*/

  {
    DBTmat invmat;
    char   errbuf[81];
    int    i;
    DBptr      cpts_la,knots_la;
    DBVector   ocpt,ncpt;
    DBfloat    w;
    DBint      ncpts,nknots;
   
/*
***Transform curve plane.
*/
   if ( ocur->plank_cu == TRUE )
      {
      GEtform_inv(pt,&invmat);
      GEtform_mult(&ocur->csy_cu,&invmat,&ncur->csy_cu);
      }
    if ( osge != NULL )
      {
      if ( osge[0].typ == NURB_SEG  )
        {
        nknots=osge->nknots;
        ncpts=nknots-osge->nurbs_degree-1;
        for ( i=0; i<ncpts; ++i )
          {
          w=osge->cpts_c[i].w_gm;
          ocpt.x_gm=osge->cpts_c[i].x_gm/w;
          ocpt.y_gm=osge->cpts_c[i].y_gm/w;
          ocpt.z_gm=osge->cpts_c[i].z_gm/w;
          GEtfpos_to_local(&ocpt,pt,&ncpt);
          nsge->cpts_c[i].x_gm=ncpt.x_gm*w;
          nsge->cpts_c[i].y_gm=ncpt.y_gm*w;
          nsge->cpts_c[i].z_gm=ncpt.z_gm*w;
          }
/*
***Store transformed controlpoints and knotvector in DB.
*/
        DBwrite_nurbs(nsge->cpts_c,ncpts,nsge->knots_c,nknots,&cpts_la,&knots_la);
        /*if ( status < 0 ) return(erpush("GExxx2",""));   TODO */
/*
***Update DBSeg's. only logical adresses to cpts and knots
***need update here.
*/
        for ( i=0; i<ocur->ns_cu; ++i )
          {
          (nsge+i)->cpts_db  = cpts_la;
          (nsge+i)->knots_db = knots_la;
          }
        }

      else /* not NURB_SEG */
        {
        for ( i=0; i<ocur->ns_cu; ++i )
          {
          if ( osge[i].typ == CUB_SEG )
            GEtfseg_to_local(&osge[i],pt,&nsge[i]);
          else
            {
            sprintf(errbuf,"%d",i+1);
            return(erpush("GE6282",errbuf)); /* this segment is an UV_CUB_SEG */
            }
          }
        }
      }
/*
***Transform graphical segments if they are different
***from the geometric segments.
*/
    if ( osgr != osge  &&  osgr != NULL )
      {
      for ( i=0; i<ocur->nsgr_cu; ++i )
        GEtfseg_to_local(&osgr[i],pt,&nsgr[i]);
      }
/*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfCsys_to_local(
        DBCsys *ocsy,
        DBTmat *opmat,
        DBTmat *pt,
        DBCsys *ncsy,
        DBTmat *npmat)

/*      Transforms the input coordinate system with t.
 *      (from basic to local)
 *
 *      In: ocsy  = Original coordinate system
 *          opmat = Original matrix
 *          pt    = Transformation
 *
 *      Out: *npmat = Transformed matrix
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    DBTmat tmp;

/*
***Invert and myltiply.
*/
    GEtform_inv(pt,&tmp);
    GEtform_mult(opmat,&tmp,npmat);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfText_to_local(
        DBText *otxt,
        DBTmat *pt,
        DBText *ntxt)

/*      Transforms a text with t.
 *      (from basic to local)
 *
 *      In: otxt  = Original text
 *          pt    = Transformation
 *
 *      Out: *ntxt = Transformed text
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat det,sx,vs;
    DBTmat  tmp;

/*
***Transform origin.
*/
    GEtfpos_to_local(&otxt->crd_tx,pt,&ntxt->crd_tx);
/*
***And angle.
*/
    GEtform_inv(pt,&tmp);
    GEtfang_to_basic(otxt->v_tx,&tmp,&ntxt->v_tx);
/*
***Special treatment for miroring.
*/
    calds2(pt,&det,&sx);

    if ( det < 0.0 )
      {
      vs = ntxt->v_tx - otxt->v_tx;
      ntxt->v_tx = vs - otxt->v_tx;
      }
/*
***Normalize angle.
*/
    if ( ntxt->v_tx >  360.0 ) ntxt->v_tx -= 360.0;
    if ( ntxt->v_tx < -360.0 ) ntxt->v_tx += 360.0;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfLdim_to_local(
        DBLdim *oldm,
        DBTmat *pt,
        DBLdim *nldm)

/*      Transforms a linear dimension with t.
 *      (from basic to local)
 *
 *      In: oldm  = Original dimension
 *          pt    = Transformation
 *
 *      Out: *nldm = Transformed dimension
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    GEtfpos_to_local(&oldm->p1_ld,pt,&nldm->p1_ld);
    GEtfpos_to_local(&oldm->p2_ld,pt,&nldm->p2_ld);
    GEtfpos_to_local(&oldm->p3_ld,pt,&nldm->p3_ld);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfCdim_to_local(
        DBCdim *ocdm,
        DBTmat *pt,
        DBCdim *ncdm)

/*      Transforms a diameter dimension with t.
 *      (from basic to local)
 *
 *      In: ocdm  = Original dimension
 *          pt    = Transformation
 *
 *      Out: *ncdm = Transformed dimension
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    GEtfpos_to_local(&ocdm->p1_cd,pt,&ncdm->p1_cd);
    GEtfpos_to_local(&ocdm->p2_cd,pt,&ncdm->p2_cd);
    GEtfpos_to_local(&ocdm->p3_cd,pt,&ncdm->p3_cd);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfRdim_to_local(
        DBRdim *ordm,
        DBTmat *pt,
        DBRdim *nrdm)

/*      Transforms a radius dimension with t.
 *      (from basic to local)
 *
 *      In: ordm  = Original dimension
 *          pt    = Transformation
 *
 *      Out: *nrdm = Transformed dimension
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    GEtfpos_to_local(&ordm->p1_rd,pt,&nrdm->p1_rd);
    GEtfpos_to_local(&ordm->p2_rd,pt,&nrdm->p2_rd);
    GEtfpos_to_local(&ordm->p3_rd,pt,&nrdm->p3_rd);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfAdim_to_local(
        DBAdim *oadm,
        DBTmat *pt,
        DBAdim *nadm)

/*      Transforms an angular dimension with t.
 *      (from basic to local)
 *
 *      In: oadm  = Original dimension
 *          pt    = Transformation
 *
 *      Out: *nadm = Transformed dimension
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    GEtfpos_to_local(&oadm->pos_ad,pt,&nadm->pos_ad);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfHatch_to_local(
        DBHatch *xhtp,
        DBfloat  crdvek[],
        DBTmat  *pt)

/*      Transforms a hatch with t.
 *      (from basic to local)
 *
 *      In: xhtp   = Original hatching
 *          crdvec = Hatchline coordinates
 *          pt     = Transformation
 *
 *      Out: *crdvek = Transformed coordinates
 *
 *      (C)microform ab 8/12/92 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   int   i;
   DBVector op,np;

   for ( i=0; i<4*xhtp->nlin_xh; i+=2 )
     {
     op.x_gm = crdvek[i];
     op.y_gm = crdvek[i+1];
     op.z_gm = 0.0;
     GEtfpos_to_local(&op,pt,&np);
     crdvek[i]   = np.x_gm;
     crdvek[i+1] = np.y_gm;
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfSurf_to_local(
        DBSurf  *sur,
        DBPatch *patpek,
        DBTmat  *pt)

/*      Transforms a surface and its patches with t.
 *      (from basic to local)
 *
 *      In: sur    = Original surface
 *          patpek = Topological patches
 *          pt     = Transformation
 *
 *      Out: *patpek = Transformed patches
 *
 *      (C)microform ab 1/3/93 J. Kjellander
 *
 *       940418  RAT_SUR och LFT_SUR, J. Kjellander
 *       960224  CON_SUR              G. Liden       
 *       970419  POL_SUR P3_SUR ....  G. Liden       
 *       971204  FAC_SUR, .. sur610   G. Liden       
 *       1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***All surface transformations are handled by dedicated 
***routines.
*/
    switch ( sur->typ_su )
      {
      case CUB_SUR:
      status = varkon_sur_bictra(sur,patpek,pt);
      break;

      case RAT_SUR:
      status = varkon_sur_rbictra(sur,patpek,pt);
      break;

      case LFT_SUR:
      status = varkon_sur_lofttra(sur,patpek,pt);
      break;

      case CON_SUR:
      status = varkon_sur_contra(sur,patpek,pt);
      break;

      case POL_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  P3_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  P5_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  P7_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  P9_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  P21_SUR:
      status = varkon_sur_poltra(sur,patpek,pt);
      break;

      case  NURB_SUR:
      status = varkon_sur_nurbtra(sur,patpek,pt);
      break;

      default:
      status = varkon_sur_transf(sur,patpek,pt);
      break;
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfBplane_to_local(
        DBBplane *obpl,
        DBTmat   *pt,
        DBBplane *nbpl)

/*      Transforms a B_plane with t.
 *      (from basic to local)
 *
 *      In: obpl  = Original B_plane
 *          pt    = Transformation
 *
 *      Out: *nbpl = Transformed B_plane
 *
 *      (C)microform ab 1996-06-25 J.Kjellander
 *
 *       1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Transform the 4 corners.
*/
   GEtfpos_to_local(&obpl->crd1_bp,pt,&nbpl->crd1_bp);
   GEtfpos_to_local(&obpl->crd2_bp,pt,&nbpl->crd2_bp);
   GEtfpos_to_local(&obpl->crd3_bp,pt,&nbpl->crd3_bp);
   GEtfpos_to_local(&obpl->crd4_bp,pt,&nbpl->crd4_bp);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEtfMesh_to_local(
        DBMesh  *mshp,
        DBTmat  *pt)

/*      Transforms a mesh with t.
 *      (from basic to local)
 *
 *      In: mshp   = Original mesh
 *          pt     = Transformation
 *
 *      Out: *crdvek = Transformed coordinates
 *
 *      (C)18/7/04 J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   int   i;

   for ( i=0; i<mshp->nv_m; ++i )
     {
     GEtfpos_to_local(&(mshp->pv_m[i].p),pt,&(mshp->pv_m[i].p));
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short calds2(
        DBTmat  *pt,
        DBfloat *pdet,
        DBfloat *psx)

/*      Computes 2D-determinant and X-scale.
 *
 *      In: pt   = Transformation
 *
 *      Ut: *pdet = 2D-determinant
 *          *psx  = 2D-scale
 *
 *      (C)microform ab 28/2/93 J. Kjellander
 *
 *      1999-04-28 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Compute determinant for transformation upper left
***2X2-submatrix.
*/
  *pdet = pt->g11*pt->g22 - pt->g12*pt->g21;
/*
***Scalefactor.
*/
  *psx = SQRT(pt->g11*pt->g11 + pt->g21*pt->g21);

   return(0);
  }

/********************************************************/




