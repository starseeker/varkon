/*!******************************************************************/
/*  File: geevale.c     (evaluate entity)                           */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*    GEposition      Evaluate position, "ON" in MBS                */
/*    GEtangent       Evaluate tangent, "TANG" in MBS               */
/*    GEcentre        Evaluate centre, "CENTRE" in MBS              */
/*    GEarclength     Evaluate arclength, "ARCL" in MBS             */
/*    GEcurvature     Evaluate curvature, "CURV" in MBS             */
/*    GEnormal        Evaluate normal, "NORM" in MBS                */
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
/*  (C)Microform AB 1984-1999, J.Kjellander, johan@microform.se     */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

        DBstatus GEposition(
        DBAny    *gmstrp,
        char     *datpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Computes the coordinates of a position on an entity.
 *
 *      In: gmstrp = Entity
 *          datpek = Additional data (segments, patches...)
 *          u      = Parametric value
 *          v      = Second value for surfaces
 *
 *      Out: *vecptr = Coordinates
 *
 *      (C)microform ab 11/1/85 J. Kjellander
 *
 *      13/8/85  2D cirkel J. Kjellander
 *      14/9/85  Resten av typerna J. Kjellander
 *      26/9/85  Parametrisering för 3D-cirklar J. Kjellander
 *      18/11/85 Tolerenser, J. Kjellander
 *      29/12/85 Symbol, J. Kjellander
 *      20/11/91 Nytt format för kurvor, J. Kjellander
 *      16/12/91 GE107(), J. Kjellander
 *      22/2/93  Ytor, J. Kjellander
 *      2/11/94  GE109(), J. Kjellander
 *      1996-07-09 9->0 i anrop av sur210, G.Liden
 *      1997-12-07 sur360 Gunnar Liden
 *      1998-09-24 B_plan, J,Kjellander
 *      1999-04-29 Rewritten, J.Kjellander
 *      1999-12-18 sur360->varkon_sur_uvmap 
 *                 sur210->varkon_sur_eval 
 *                 sur709->varkon_bpl_on    Gunnar Liden
 *
 ******************************************************!*/

  {
    DBfloat dx,dy,dz,fi;
    short   status;
    DBTmat  invcrd;
    EVALC   eval_c;
    EVALS   eval_s;
    DBfloat u_map;     /* Mapped U parameter value */
    DBfloat v_map;     /* Mapped V parameter value */

/*
***What kind of entity ?
*/
    switch ( gmstrp->hed_un.type )
      {
/*
***Point.
*/
      case POITYP:
      vecptr->x_gm = gmstrp->poi_un.crd_p.x_gm;
      vecptr->y_gm = gmstrp->poi_un.crd_p.y_gm;
      vecptr->z_gm = gmstrp->poi_un.crd_p.z_gm;
      return(0);
/*
***Line.
*/
      case LINTYP:
      dx = gmstrp->lin_un.crd2_l.x_gm - gmstrp->lin_un.crd1_l.x_gm;
      dy = gmstrp->lin_un.crd2_l.y_gm - gmstrp->lin_un.crd1_l.y_gm;
      dz = gmstrp->lin_un.crd2_l.z_gm - gmstrp->lin_un.crd1_l.z_gm;

      vecptr->x_gm = gmstrp->lin_un.crd1_l.x_gm + u*dx;
      vecptr->y_gm = gmstrp->lin_un.crd1_l.y_gm + u*dy;
      vecptr->z_gm = gmstrp->lin_un.crd1_l.z_gm + u*dz;
      return(0);
/*
***Arc 2D.
*/
      case ARCTYP:
      if ( gmstrp->arc_un.ns_a == 0 )
         {
         fi = DGTORD * (gmstrp->arc_un.v1_a + u * 
              (gmstrp->arc_un.v2_a - gmstrp->arc_un.v1_a));
         vecptr->x_gm = gmstrp->arc_un.x_a + gmstrp->arc_un.r_a * COS(fi);
         vecptr->y_gm = gmstrp->arc_un.y_a + gmstrp->arc_un.r_a * SIN(fi);
         vecptr->z_gm = 0.0;
         return(0);
         }
/*
***Arc 3D. Use arc length parametrisation.
*/
      else
        {
        status = GE717(gmstrp,(GMSEG *)datpek,NULL,u,&eval_c.t_global);
        if( status < 0 ) return(status);

        eval_c.evltyp = EVC_R;
        status = GE109(gmstrp,(GMSEG *)datpek,&eval_c);
        if ( status < 0 ) return(status);

        vecptr->x_gm = eval_c.r.x_gm;
        vecptr->y_gm = eval_c.r.y_gm;
        vecptr->z_gm = eval_c.r.z_gm;
        return(0);
        }
/*
***Curve. Use native parametrisation.
*/
      case CURTYP:
      eval_c.evltyp   = EVC_R;
      eval_c.t_global = u + 1.0;
      status = GE109(gmstrp,(GMSEG *)datpek,&eval_c);
      if ( status < 0 ) return(status);

      vecptr->x_gm = eval_c.r.x_gm;
      vecptr->y_gm = eval_c.r.y_gm;
      vecptr->z_gm = eval_c.r.z_gm;
      return(0);
/*
***Surface.
*/
      case SURTYP:
      status = varkon_sur_uvmap(
          (GMSUR *)&gmstrp->sur_un,(DBPatch*)datpek,u,v,&u_map,&v_map);
      if( status < 0 ) return(status);
      status = varkon_sur_eval((GMSUR *)&gmstrp->sur_un,
               (DBPatch*)datpek, (gmint)0,u_map,v_map,&eval_s);
      if( status < 0 ) return(status);
      vecptr->x_gm = eval_s.r_x;
      vecptr->y_gm = eval_s.r_y;
      vecptr->z_gm = eval_s.r_z;
      return(0);
/*
***B_plane.
*/
      case BPLTYP:
      status = varkon_bpl_on((GMBPL *)&gmstrp->bpl_un,u,v,vecptr);
      if( status < 0 ) return(status);
      return(0);
/*
***Coordinate system.
*/
      case CSYTYP:
      GEtform_inv(&gmstrp->csy_un.mat_pl,&invcrd);
      vecptr->x_gm = invcrd.g14;
      vecptr->y_gm = invcrd.g24;
      vecptr->z_gm = invcrd.g34;
      return(0);
/*
***Text.
*/
      case TXTTYP:
      vecptr->x_gm = gmstrp->txt_un.crd_tx.x_gm;
      vecptr->y_gm = gmstrp->txt_un.crd_tx.y_gm;
      vecptr->z_gm = gmstrp->txt_un.crd_tx.z_gm;
      return(0);
/*
***Linear dimension.
*/
      case LDMTYP:
      vecptr->x_gm = gmstrp->ldm_un.p3_ld.x_gm;
      vecptr->y_gm = gmstrp->ldm_un.p3_ld.y_gm;
      vecptr->z_gm = gmstrp->ldm_un.p3_ld.z_gm;
      return(0);
/*
***Diameter dimension.
*/
      case CDMTYP:
      vecptr->x_gm = gmstrp->cdm_un.p3_cd.x_gm;
      vecptr->y_gm = gmstrp->cdm_un.p3_cd.y_gm;
      vecptr->z_gm = gmstrp->cdm_un.p3_cd.z_gm;
      return(0);
/*
***Radius dimension.
*/
      case RDMTYP:
      vecptr->x_gm = gmstrp->rdm_un.p3_rd.x_gm;
      vecptr->y_gm = gmstrp->rdm_un.p3_rd.y_gm;
      vecptr->z_gm = gmstrp->rdm_un.p3_rd.z_gm;
      return(0);
/*
***Angular dimension.
*/
      case ADMTYP:
      vecptr->x_gm = gmstrp->adm_un.pos_ad.x_gm +
              gmstrp->adm_un.r_ad*COS(gmstrp->adm_un.tv_ad*DGTORD);
      vecptr->y_gm = gmstrp->adm_un.pos_ad.y_gm +
              gmstrp->adm_un.r_ad*SIN(gmstrp->adm_un.tv_ad*DGTORD);
      vecptr->z_gm = gmstrp->adm_un.pos_ad.z_gm;
      return(0);
/*
***Unknown entity type.
*/
      default:
      return(erpush("GE8024",""));
      }
   }

/********************************************************/
/*!******************************************************/

        DBstatus GEtangent(
        DBAny    *gmstrp,
        DBSeg    *segmnt,
        DBfloat   t,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Computes the normalised tangent of an entity.
 *
 *      In: gmstrp = Entity
 *          segmnt = Optional segments
 *          t      = Parametric value
 *
 *      Out: *vecptr = Normaliserad tangent.
 *
 *      (C)microform ab 3/1/86  J. Kjellander
 *
 *      20/11/91 Nytt format för kurvor, J. Kjellander
 *      16/12/91 GE107(), J. Kjellander
 *      20/12/91 Bågl-par. 3D-cirklar, J. Kjellander
 *      12/11/94 GE109 för kurva, G. Liden         
 *      2/11/94  GE109(), J. Kjellander
 *      1999-04-29 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBfloat fi;
    EVALC   eval_c;

/*
***What kind of entity ?
*/
    switch (gmstrp->hed_un.type)
      {
/*
***Line.
*/
      case LINTYP:
      vecptr->x_gm = gmstrp->lin_un.crd2_l.x_gm - gmstrp->lin_un.crd1_l.x_gm;
      vecptr->y_gm = gmstrp->lin_un.crd2_l.y_gm - gmstrp->lin_un.crd1_l.y_gm;
      vecptr->z_gm = gmstrp->lin_un.crd2_l.z_gm - gmstrp->lin_un.crd1_l.z_gm;
      GEnormalise_vector3D(vecptr,vecptr);
      return(0);
/*
***Arc 2D.
*/
      case ARCTYP:
      if ( gmstrp->arc_un.ns_a == 0 )
         {
         fi = DGTORD * (gmstrp->arc_un.v1_a + t * 
              (gmstrp->arc_un.v2_a - gmstrp->arc_un.v1_a));
         vecptr->x_gm = -SIN(fi);
         vecptr->y_gm =  COS(fi);
         vecptr->z_gm =  0.0;
         return(0);
         }
/*
***Arc 3D.
*/
      else
        {
        status = GE717(gmstrp,segmnt,NULL,t,&eval_c.t_global);
        if( status < 0 ) return(status);

        eval_c.evltyp = EVC_DR;
        status = GE109(gmstrp,segmnt,&eval_c);
        if ( status < 0 ) return(status);

        GEnormalise_vector3D(&eval_c.drdt,vecptr);
        return(0);
        }
/*
***Curve.
*/
      case CURTYP:
      eval_c.evltyp   = EVC_DR;
      eval_c.t_global = t + 1.0;
      if ( (status=GE109(gmstrp,segmnt,&eval_c)) < 0 ) return(status);

      GEnormalise_vector3D(&eval_c.drdt,vecptr);
      return(0);
/*
***Coordinate system.
*/
      case CSYTYP:
      vecptr->x_gm = crdptr->g11;
      vecptr->y_gm = crdptr->g12;
      vecptr->z_gm = crdptr->g13;
      return(0);
/*
***Unknown entity type.
*/
      default:
      return(erpush("GE8024",""));
      }
   }

/********************************************************/
/*!******************************************************/

        DBstatus GEcentre(
        DBAny    *gmstrp,
        DBSeg    *segmnt,
        DBfloat   t,
        DBVector *vecptr)

/*      Computes the centre of curvature of an entity.
 *
 *      In: gmstrp = Entity
 *          segmnt = Optional segments
 *          t      = Parametric value
 *
 *      Out: *vecptr = Position of centre of curvature
 *
 *      (C)microform ab 6/1/86  J. Kjellander
 *
 *      11/5/87  kappa=0, J. Kjellander
 *      20/11/91 Nytt format för kurvor, J. Kjellander
 *      15/12/91 GE107(), J. Kjellander
 *      2/12/94  GE109(), J. Kjellander
 *      1999-04-29 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;
    EVALC eval_c;

/*
***What kind of entity ?
*/
    switch (gmstrp->hed_un.type)
      {
/*
***Arc 2D.
*/
      case ARCTYP:
      if ( gmstrp->arc_un.ns_a == 0 )
         {
         vecptr->x_gm = gmstrp->arc_un.x_a;
         vecptr->y_gm = gmstrp->arc_un.y_a;
         vecptr->z_gm =  0.0;
         return(0);
         }
/*
***Arc 3D.
*/
      else
        {
        eval_c.evltyp   = EVC_R + EVC_PN + EVC_KAP;
        eval_c.t_global = 1.0;
        if ( (status=GE109(gmstrp,segmnt,&eval_c)) < 0 ) return(status);

        vecptr->x_gm = eval_c.r.x_gm + eval_c.p_norm.x_gm/eval_c.kappa;
        vecptr->y_gm = eval_c.r.y_gm + eval_c.p_norm.y_gm/eval_c.kappa;
        vecptr->z_gm = eval_c.r.z_gm + eval_c.p_norm.z_gm/eval_c.kappa;
        return(0);
        }
/*
***Curve.
*/
      case CURTYP:
      eval_c.evltyp   = EVC_R + EVC_PN + EVC_KAP;
      eval_c.t_global = t + 1.0;
      if ( (status=GE109(gmstrp,segmnt,&eval_c)) < 0 ) return(status);

      if ( eval_c.kappa > 1E-15 )
        {
        vecptr->x_gm = eval_c.r.x_gm + eval_c.p_norm.x_gm/eval_c.kappa;
        vecptr->y_gm = eval_c.r.y_gm + eval_c.p_norm.y_gm/eval_c.kappa;
        vecptr->z_gm = eval_c.r.z_gm + eval_c.p_norm.z_gm/eval_c.kappa;
        }
      else
        {
        vecptr->x_gm = 0.0;
        vecptr->y_gm = 0.0;
        vecptr->z_gm = 0.0;
        }

      return(0);
/*
***Unknown entity type.
*/
      default:
      return(erpush("GE8024",""));
      }
   }

/********************************************************/
/*!******************************************************/

        DBstatus GEarclength(
        DBAny   *gmstrp,
        DBSeg   *segmnt,
        DBfloat *length)

/*      Computes the length of an entity.
 *
 *      In: gmstrp = Entity
 *          segmnt = Optional segments
 *
 *      Out: *length        = Total lengt
 *           *segmnt->sl    = The length of each arc segment
 *
 *      (C)microform ab 8/1/86  J. Kjellander
 *
 *      20/11/91 Nytt format för kurvor, J. Kjellander
 *      12/12/91 Kurvlängd, J. Kjellander
 *      21/11/94 sur753(), G Liden
 *      1999-04-29 Rewritten, J.Kjellander
 *      1999-12-18 sur753->varkon_comptol G. Liden
 ******************************************************!*/

  {
    int      i,noseg=0;
    DBfloat  dl,interv[2];
    DBVector dp;

/*
***What kind of entity ?
*/
    switch (gmstrp->hed_un.type)
      {
/*
***Line.
*/
      case LINTYP:
      dp.x_gm = gmstrp->lin_un.crd2_l.x_gm - gmstrp->lin_un.crd1_l.x_gm;
      dp.y_gm = gmstrp->lin_un.crd2_l.y_gm - gmstrp->lin_un.crd1_l.y_gm;
      dp.z_gm = gmstrp->lin_un.crd2_l.z_gm - gmstrp->lin_un.crd1_l.z_gm;
     *length = GEvector_length3D(&dp);
      return(0); 
/*
***Arc 2D.
*/
      case ARCTYP:
      if ( gmstrp->arc_un.ns_a == 0 )
        {
        *length = (gmstrp->arc_un.v2_a - gmstrp->arc_un.v1_a)/360.0*
                   PI2*gmstrp->arc_un.r_a;
        gmstrp->arc_un.al_a = *length;
        return(SUCCED);
        }
/*
***Arc 3D.
*/
      else noseg = gmstrp->arc_un.ns_a;
      break;
/*
***Curve.
*/
      case CURTYP:
      noseg = gmstrp->cur_un.ns_cu;
      break;
      }
/*
***Common method for 3D-arc, and curve.
*/
    *length   = 0.0;
    interv[0] = 0.0 + varkon_comptol();
    interv[1] = 1.0 - varkon_comptol();
/*
***Compute segment lengths with GE120() and add.
***Save each segments length in DBSeg as well.
*/
    for ( i=0; i<noseg ; ++i )
       {
       if ( GE120(gmstrp,segmnt+i,interv,&dl) < 0 )
         return(erpush("GE1213",""));
       (segmnt+i)->sl = dl;
       *length += dl;
       } 
/*
***Save total length.
*/
     if ( gmstrp->hed_un.type == CURTYP )
       gmstrp->cur_un.al_cu = *length;
     else if ( gmstrp->hed_un.type == ARCTYP )
       gmstrp->arc_un.al_a = *length;

     return(0);
   }

/********************************************************/
/*!******************************************************/

        DBstatus GEcurvature(
        DBAny   *gmstrp,
        DBSeg   *segmnt,
        DBfloat  t,
        DBfloat *pkappa)

/*      Computes the curvature of an entity.
 *
 *      In: gmstrp = Entity
 *          segmnt = Optional segments
 *          t      = Parametric value
 *
 *      Out: *pkappa = Curvature
 *
 *      (C)microform ab 12/12/91  J. Kjellander
 *
 *      2/12/94  GE109(), J. Kjellander
 *      1999-04-29 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    short status; 
    EVALC eval_c;

/*
***What kind of entity ?
*/
    switch (gmstrp->hed_un.type)
      {
/*
***Line, kappa = 0.
*/
      case LINTYP:
     *pkappa = 0.0;
      return(0);
/*
***Arc.
*/
      case ARCTYP:
     *pkappa = 1.0/gmstrp->arc_un.r_a;
      return(0);
/*
***Curve.
*/
      case CURTYP:
      eval_c.evltyp   = EVC_KAP;
      eval_c.t_global = t + 1.0;
      if ( (status=GE109(gmstrp,segmnt,&eval_c)) < 0 ) return(status);
     *pkappa = eval_c.kappa;
      return(0);
/*
***Unknown entity type.
*/
      default:
      return(erpush("GE8024",""));
      }
   }

/********************************************************/
/*!******************************************************/

        DBstatus GEnormal(
        DBAny    *gmstrp,
        char     *datpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Computes the normal direction of an entity.
 *
 *      In: gmstrp = Entity
 *          datpek = Additional data (segments, patches...)
 *          u      = Parametric value
 *          v      = Second value for surfaces
 *
 *      Out: *vecptr = Normaliserad normal.
 *
 *      (C)microform ab 3/12/92  J. Kjellander
 *
 *      22/2/93  Ytor, J. Kjellander
 *      2/12/94  GE109(), J. Kjellander
 *      2/12/95  icase= 3 för normal i sur210 Gunnar Liden
 *      1997-12-07 sur360 Gunnar Liden
 *      1998-04-27 b_plan, J.Kjellander
 *      1999-04-29 Rewritten, J.Kjellander
 *      1999-12-18 sur360->varkon_sur_uvmap Gunnar Liden
 *
 ******************************************************!*/

  {
    DBfloat  fi,d;
    short    status;
    DBVector p21,p41,prod;
    EVALS    xyz;
    EVALC    eval_c;
    DBTmat  *crdptr;
    DBfloat  u_map;     /* Mapped U parameter value                */
    DBfloat  v_map;     /* Mapped V parameter value                */

/*
***What kind of entity ?
*/
    switch (gmstrp->hed_un.type)
      {
/*
***Arc 2D.
*/
      case ARCTYP:
      if ( gmstrp->arc_un.ns_a == 0 )
         {
         fi = DGTORD * (gmstrp->arc_un.v1_a + u * 
              (gmstrp->arc_un.v2_a - gmstrp->arc_un.v1_a));
         vecptr->x_gm =  -COS(fi);
         vecptr->y_gm =  -SIN(fi);
         vecptr->z_gm =  0.0;
         return(0);
         }
/*
***Arc 3D.
*/
      else
        {
        status = GE717(gmstrp,(GMSEG *)datpek,NULL,u,&eval_c.t_global);
        if( status < 0 ) return(status);

        eval_c.evltyp = EVC_PN;
        status = GE109(gmstrp,(GMSEG *)datpek,&eval_c);
        if ( status < 0 ) return(status);

        vecptr->x_gm = eval_c.p_norm.x_gm;
        vecptr->y_gm = eval_c.p_norm.y_gm;
        vecptr->z_gm = eval_c.p_norm.z_gm;
        return(0);
        }
/*
***Curve.
*/
      case CURTYP:
      eval_c.evltyp   = EVC_PN;
      eval_c.t_global = u + 1.0;
      status = GE109(gmstrp,(GMSEG *)datpek,&eval_c);
      if ( status < 0 ) return(status);

      vecptr->x_gm = eval_c.p_norm.x_gm;
      vecptr->y_gm = eval_c.p_norm.y_gm;
      vecptr->z_gm = eval_c.p_norm.z_gm;
      return(0);
/*
***Coordinate system.
*/
      case CSYTYP:
      crdptr = (DBTmat *)datpek;
      vecptr->x_gm = crdptr->g31;
      vecptr->y_gm = crdptr->g32;
      vecptr->z_gm = crdptr->g33;
      return(0);
/*
***B_plane.
*/
      case BPLTYP:
      p21.x_gm = gmstrp->bpl_un.crd2_bp.x_gm - gmstrp->bpl_un.crd1_bp.x_gm;
      p21.y_gm = gmstrp->bpl_un.crd2_bp.y_gm - gmstrp->bpl_un.crd1_bp.y_gm;
      p21.z_gm = gmstrp->bpl_un.crd2_bp.z_gm - gmstrp->bpl_un.crd1_bp.z_gm;

      p41.x_gm = gmstrp->bpl_un.crd4_bp.x_gm - gmstrp->bpl_un.crd1_bp.x_gm;
      p41.y_gm = gmstrp->bpl_un.crd4_bp.y_gm - gmstrp->bpl_un.crd1_bp.y_gm;
      p41.z_gm = gmstrp->bpl_un.crd4_bp.z_gm - gmstrp->bpl_un.crd1_bp.z_gm;

      prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
      prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
      prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;

      d = 1.0/SQRT(prod.x_gm*prod.x_gm + 
                   prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm);

      if ( d < 1E-10 ) return(erpush("GE8302",""));
   
      vecptr->x_gm = prod.x_gm * d;
      vecptr->y_gm = prod.y_gm * d;
      vecptr->z_gm = prod.z_gm * d;

      return(0);
/*
***Surface.
*/
      case SURTYP:
      status = varkon_sur_uvmap(
           (GMSUR *)&gmstrp->sur_un,(DBPatch*)datpek,u,v,&u_map,&v_map);
      if( status < 0 ) return(status);
      status = varkon_sur_eval(
              (GMSUR *)&gmstrp->sur_un,(DBPatch*)datpek,
               (gmint)3,u,v,&xyz);
      if( status < 0 ) return(status);
      vecptr->x_gm = xyz.n_x;
      vecptr->y_gm = xyz.n_y;
      vecptr->z_gm = xyz.n_z;
      return(0);
/*
***Unknown entity type.
*/
      default:
      return(erpush("GE8024",""));
      }
   }

/********************************************************/
