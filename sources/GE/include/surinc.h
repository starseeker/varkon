/**********************************************************************
*
*    surinc.h
*    ========
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
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
*    (C)Microform AB 1984-1999, Gunnar Liden     gunnar@microform.se
*    2000-09-26   Argument changed in sur289 metod->s_case Gunnar Liden
*    2000-11-01   varkon_sur_curves added                  Gunnar Liden
*
***********************************************************************/

/*
***Temporary definitions of long names
*/

#define varkon_erpush          erpush
#define varkon_errmes          errmes 
#define varkon_erinit          erinit

/*
***Surface interrogation calculation definitions
*/

#include "surint.h"
#include "homop.h"

/*
***Function prototypes for surface interrogation functions
*/

/* sur100.c   varkon_sur_silhouette Silhouette curve on a surface     */
   DBstatus   varkon_sur_silhouette ( 
              DBSurf  *p_sur, DBPatch  *p_pat, DBptr    p_gm,
              DBfloat  eyedata[9], DBint    rtype, 
              DBint    acc, DBint    cur_no, DBint   *p_nobranch, 
              DBCurve *p_cur, DBSeg **pp_seg);

/* sur102.c   varkon_sur_interplane Intersection surface/plane        */
   DBstatus   varkon_sur_interplane (
              DBSurf *p_sur, DBPatch *p_pat, DBptr    p_gm,
              DBTmat *p_isys, DBint acc, DBint cur_no,
              DBint *p_nobranch, DBCurve *p_cur, DBSeg **pp_seg );

/* sur106.c   varkon_sur_geodesic   Geodesic curve on a surface       */
   DBstatus   varkon_sur_geodesic (
              DBSurf  *p_sur, DBPatch *p_pat, DBptr    pgm_sur,
              DBVector *p_s_uv, DBVector *p_s_tuv, DBint    gtype,
              DBint    acc, DBCurve *p_cur, DBSeg  **pp_seg );

/* sur107.c   varkon_sur_interfacet Intersection facet surfaces       */
   DBstatus   varkon_sur_interfacet (
              DBSurf  *p_sur1, DBPatch *p_pat1,
              DBSurf  *p_sur2, DBPatch *p_pat2,
              DBint a_size, DBVector s_pts[], DBVector e_pts[],
              DBint *p_nlines );

/* sur108.c   varkon_sur_interaplan Intersection APLANE surfaces      */
   DBstatus   varkon_sur_interaplan (
              APLANE  *p_asur1, DBint    nasur1,
              APLANE  *p_asur2, DBint    nasur2,
              APOINT  *p_apts, DBint   *p_napts );

/* sur104.c   varkon_sur_intersurf  Intersection surface/surface      */
   DBstatus   varkon_sur_intersurf (
              DBSurf  *p_sur1, DBPatch *p_pat1, DBptr    p_gm1,
              DBSurf  *p_sur2, DBPatch *p_pat2, DBptr    p_gm2,
              DBint    acc, DBint    cur_no, DBint   *p_nobranch,
              DBCurve *p_cur, DBSeg  **pp_seg );

/* sur105.c   varkon_sur_planplan   Intersection B-plane/B-plane      */
  DBstatus    varkon_sur_planplan (
              DBBplane *p_bpl1, DBBplane *p_bpl2, DBint lin_no,
              DBLine *p_lin, DBint *p_nlin );



/* sur150.c   varkon_sur_isoparam   Isoparametric curve               */
   DBstatus   varkon_sur_isoparam (
              DBSurf  *p_sur, DBPatch *p_pat, char    *iso_c,
              DBfloat iso_v, DBfloat start_v_in, DBfloat end_v_in,
              DBptr la, DBCurve *p_cur, DBSeg  **pp_seg );

/* sur160.c   varkon_sur_closestpt  Closest point on a surface        */
   DBstatus   varkon_sur_closestpt (
              DBSurf *p_sur, DBPatch *p_pat, DBVector *p_extpt,
              DBVector *p_s_uvpt, DBint ocase, DBint acc,
              DBint sol_no, DBint *p_no_points, DBVector *p_cpt,
              DBVector *p_cuvpt );

/* sur161.c   varkon_sur_intlinept  Intersect line and surface        */
   DBstatus   varkon_sur_intlinept (
              DBSurf  *p_sur, DBPatch *p_pat, DBVector *p_extpt,
              DBVector *p_v_proj, DBVector *p_s_uvpt, DBint ocase,
              DBint scase, DBint acc, DBint sol_no, DBint *p_no_points,
              DBVector *p_cpt, DBVector *p_cuvpt, DBVector uv_out[],
              DBVector r3_out[] );

/* sur163.c   varkon_sur_int2linpt  Intersect line and surface        */
   DBstatus   varkon_sur_int2linpt (
              DBSurf *p_sur, DBPatch *p_pat, DBVector *p_extpt,
              DBVector *p_v_proj, DBVector *p_s_uvpt, DBint scase,
              DBint acc, DBint *p_no_points, EVALS xy_out[] );

/* sur164.c   varkon_sur_curint     Intersect curve and surface       */
   DBstatus   varkon_sur_curint (
              DBSurf  *p_sur, DBPatch  *p_pat, 
              DBCurve *p_cur, DBSeg    *p_seg,
              DBVector *p_s_uvpt, DBint ocase, DBint acc, DBint sol_no,
              DBint *p_no_points, DBVector *p_cpt, DBVector *p_cuvpt,
              DBfloat *p_uvalue, DBfloat u_out[], DBVector uv_out[],
              DBVector r3_out[] );

/* sur120.c   varkon_sur_fanga      FANGA surface analysis curves     */
   DBstatus   varkon_sur_fanga (
              DBSurf *p_sur, DBPatch *p_pat, DBptr pgm_sur,
              DBVector *p_sdir1, DBVector *p_sdir2,
              DBfloat ustart, DBfloat uend, DBint ivstart, DBint ivend,
              DBint f_case, DBCurve *p_cur, DBSeg *p_fseg[6] );

/* sur122.c   varkon_cur_2dchord    Fanga chord and angle (2 silh's)  */
   DBstatus   varkon_cur_2dchord (
              DBCurve *p_cur1, DBSeg   *p_seg1,
              DBCurve *p_cur2, DBSeg   *p_seg2,
              DBCurve *p_spine, DBSeg   *p_spiseg,
              DBVector angdir, DBCurve *p_chocur, DBSeg  **p_choseg,
              DBCurve *p_angcur, DBSeg  **p_angseg );


/*
***Surface creation functions
*/

/* sur201.c   varkon_nmgbicubic     From NMG bicubic to CUB_SUR       */
   DBstatus   varkon_nmgbicubic (
              char *infile, DBSurf *p_sur, DBPatch **pp_pat );

/* sur202.c   varkon_nmglftsurf     From NMG Consurf to LFT_SUR       */
   DBstatus   varkon_nmglftsurf (
              char *infile, DBSurf *p_sur, DBPatch **pp_pat );

/* sur203.c   varkon_sur_ratimport  Import a rational surface         */
   DBstatus   varkon_sur_ratimport (
              char    *infile, DBSurf  *p_sur, DBPatch **pp_pat );

/* sur204.c   varkon_nmgconsurf     From NMG Consurf to CON_SUR       */
   DBstatus   varkon_nmgconsurf (
              char *infile, DBSurf *p_sur, DBPatch **pp_pat );

/* sur205.c   varkon_sur_curves     Create surface SUR_CURVES         */
   DBstatus   varkon_sur_curves (
              DBint nu_cur, DBint nv_cur,
              DBCurve vcur[STRIP], DBSeg *p_seg[STRIP], char *metod,
              DBSurf  *p_surout,DBPatch **pp_patout); 

/* sur200.c   varkon_sur_importtype Determine type of import surface  */
   DBstatus   varkon_sur_importtype (
              char *infile, DBint *p_type, DBint *p_nu, DBint *p_nv );


/* sur800.c   varkon_sur_creloft    Create a LFT_SUR surface          */
   DBstatus   varkon_sur_creloft (
              DBCurve *p_spine, DBSeg *p_spineseg, DBint no_strips,
              DBCurve  lim[STRIP],  DBSeg   *p_lseg[STRIP],
              DBCurve  tan[STRIP],  DBSeg   *p_tseg[STRIP],
              DBCurve  mid[STRIP],  DBSeg   *p_mseg[STRIP],
              DBint mflag[STRIP], DBSurf *p_sur, DBPatch **pp_pat );

/* sur850.c   varkon_sur_rot        Create a rotational surface       */
   DBstatus   varkon_sur_rot (
              DBCurve  *p_cur, DBSeg    *p_seg, DBint     r_case,
              DBVector *p_p1, DBVector *p_p2, DBfloat v0, DBfloat v1,
              DBint sur_type, DBSurf *p_surout, DBPatch **pp_patout );

/* sur851.c   varkon_sur_rotloft    Create a LFT_SUR rotation surface */
   DBstatus   varkon_sur_rotloft (
              DBCurve *p_spine, DBSeg *p_spineseg, DBint no_strips,
              DBCurve  lim[STRIP], DBSeg   *p_lseg[STRIP],
              DBCurve  tan[STRIP], DBSeg   *p_tseg[STRIP],
              DBCurve  mid[STRIP], DBSeg   *p_mseg[STRIP],
              DBint    mflag[STRIP], DBSurf *p_sur, DBPatch **pp_pat );

/* sur860.c   varkon_sur_ruled      Create a ruled surface SUR_RULED  */
   DBstatus   varkon_sur_ruled (
              DBCurve *p_cur1, DBSeg   *p_seg1,
              DBCurve *p_cur2, DBSeg   *p_seg2, DBint    r_case,
              DBCurve *p_spine, DBSeg   *p_spineseg, DBint s_case,
              DBfloat idpoint_in, DBfloat idangle_in,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur861.c   varkon_sur_ruled_e    SUR_RULED for equal no. of segm.  */
   DBstatus   varkon_sur_ruled_e (
              DBCurve *p_cur1, DBSeg   *p_seg1, 
              DBCurve *p_cur2, DBSeg   *p_seg2, DBint    r_e_case,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur863.c   varkon_sur_ruled_l    SUR_RULED as arclength LFT_SUR    */
   DBstatus   varkon_sur_ruled_l (
              DBCurve *p_cur1, DBSeg   *p_seg1,
              DBCurve *p_cur2, DBSeg   *p_seg2, DBint    r_l_case,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur865.c   varkon_sur_ruled_d    SUR_RULED developable LFT_SUR     */
   DBstatus   varkon_sur_ruled_d (
              DBCurve *p_cur1, DBSeg   *p_seg1,
              DBCurve *p_cur2, DBSeg   *p_seg2, DBint    r_d_case,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur868.c   varkon_sur_sweep      Create swept surface SUR_SWEEP    */
   DBstatus   varkon_sur_sweep (
              DBCurve *p_spine, DBSeg *p_spineseg, DBCurve *p_cur,
              DBSeg *p_seg, DBVector *p_dir, DBint s_case,
              DBint reverse, DBSurf *p_surout, DBPatch **pp_patout );

/* sur869.c   varkon_sur_cylind     Create cylinder surface SUR_CYL   */
   DBstatus   varkon_sur_cylind (
              DBCurve *p_cur1, DBSeg *p_seg1, DBfloat dist,
              DBVector *p_dir, DBint s_case, DBSurf *p_surout,
              DBPatch **pp_patout );

/* sur801.c   varkon_sur_sele01     Create surface element SELE01     */
   DBstatus   varkon_sur_sele01 (
              DBCurve *p_spine, DBSeg  *p_spineseg, DBCurve *p_lim_s,
              DBSeg  *p_limseg, DBfloat l_s, DBfloat l_e,
              DBCurve*p_gener1, DBSeg *p_gener1seg, 
              DBCurve *p_gener2, DBSeg *p_gener2seg, DBint e_case,
              DBSurf *p_sur, DBPatch **pp_pat );

/* sur905.c   varkon_cur_develop    SUR_RULED developable ruler       */
   DBstatus   varkon_cur_develop  (
              DBCurve *p_cur1, DBSeg   *p_seg1, DBfloat  t_in1[2],
              DBCurve *p_cur2, DBSeg   *p_seg2, DBfloat  t_in2[2],
              IRUNON  *p_comp, DBint   *p_npts, EVALC    xyz_a[SMAX],
              DBint    d_code[SMAX]);

/* sur180.c   varkon_sur_offset     Create an offset surface          */
   DBstatus   varkon_sur_offset (
              DBSurf  *p_surin, DBPatch *p_patin, DBfloat  offset, 
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur170.c   varkon_sur_facapprox  Approximate a surface with facets */
   DBstatus   varkon_sur_facapprox (
              DBSurf *p_sur, DBPatch *p_pat, DBfloat offset,
              DBfloat thick, DBfloat s_lim[2][2], DBint acase,
              DBint np_u, DBint np_v, DBint acc, 
              DBfloat ctol, DBfloat ntol, DBfloat rtol,
              DBSurf *p_surout, DBPatch **pp_patout );

/* sur172.c   varkon_sur_bicapprox  Approximate to CUB_SUR surface    */
   DBstatus   varkon_sur_bicapprox (
              DBSurf *p_sur, DBPatch *p_pat, DBfloat s_lim[2][2],
              DBint acase, DBint np_u, DBint np_v, DBint tcase,
              DBfloat ctol, DBfloat ntol, DBfloat rtol,
              DBSurf *p_surout, DBPatch **pp_patout );

/* sur174.c   varkon_sur_tobspline  Approximate to BSPL_SUR surface   */
   DBstatus   varkon_sur_tobspline (
              DBSurf *p_sur, DBPatch *p_pat, DBfloat s_lim[2][2],
              DBint acase, DBint np_u, DBint np_v, DBint tcase,
              DBfloat ctol, DBfloat ntol, DBfloat rtol,
              DBSurf *p_sur_nurb, DBPatch **pp_pat_nurb );

/* sur177.c   varkon_sur_boxapprox  Approximate surface with BBOX'es  */
   DBstatus   varkon_sur_boxapprox (
              DBSurf *p_sur, DBPatch *p_pat, DBBplane *p_bpltable,
              DBint *p_nbpl );

/* sur179.c   varkon_sur_minmaxbox  Create a SUR_BOX surface.         */
   DBstatus   varkon_sur_minmaxbox (
              DBSurf  *p_sur,    DBPatch  *p_pat,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur181.c   varkon_sur_trim       Trim a surface                    */
   DBstatus   varkon_sur_trim (
              DBSurf *p_sur, DBPatch *p_patin, DBfloat s_lim[2][2],
              DBint tcase, DBSurf *p_surout, DBPatch **pp_patout );

/* sur182.c   varkon_sur_postrim    Trim a surface without reverse    */
   DBstatus   varkon_sur_postrim (
              DBSurf  *p_sur, DBPatch *p_patin, DBfloat s_lim[2][2],
              DBint tcase, DBSurf *p_surout, DBPatch **pp_patout );

/* sur184.c   varkon_sur_reverse    Reverse surface parameter direct. */
   DBstatus   varkon_sur_reverse (
              DBSurf  *p_sur, DBPatch *p_patin, DBint    rcase,
              DBSurf  *p_surout, DBPatch **pp_patout );


/* sur186.c   varkon_sur_interch    Interchange surface param direct. */
   DBstatus   varkon_sur_interch (
              DBSurf *p_sur, DBPatch *p_patin, DBint rcase,
              DBSurf *p_surout, DBPatch **pp_patout );

/* sur190.c   varkon_sur_aface      Create an approximate face        */
   DBstatus   varkon_sur_aface (
              DBCurve *p_cur, DBSeg *p_seg, DBint s_typ, DBint method,
              DBint a_case, DBint acc, DBint n_ulin, DBint n_vlin,
              DBfloat c_crit, DBfloat ctol, DBfloat ntol, DBfloat rtol,
              DBSurf *p_surout, DBPatch **pp_patout );

/* sur191.c   varkon_sur_aface_l    Line intersects for SUR_AFACE     */
   DBstatus   varkon_sur_aface_l (
              EVALS *p_xyz_s_in, DBint n_sur, DBSurf *p_all_sur[],
              DBPatch *p_all_pat[], DBint *p_n_int,
              EVALS *p_xyz_s_out );

/* sur294.c   varkon_sur_splarr     Create CUB_SUR (MBS SUR_SPLARR)   */
   DBstatus   varkon_sur_splarr (
              char *metod, DBint np_u, DBint np_v, DBVector *p_poi,
              DBVector *p_utan, DBVector *p_vtan, DBVector *p_twist,
              DBSurf *p_surout, DBPatch **pp_patout );

/* sur295.c   varkon_sur_poicre     Create CUB_SUR from points        */
   DBstatus   varkon_sur_poicre (
              char *infile, DBint c_case, DBint iu_s, DBint iv_s,
              DBint iu_d, DBint iv_d, DBint iu_e, DBint iv_e,
              DBint f_p, DBSurf *p_surout, DBPatch **pp_patout );

/* sur296.c   varkon_sur_readpts    Read points from a file           */
   DBstatus   varkon_sur_readpts (
              char *infile, SURPOI **pp_ptsout,
              gmint *p_nu, gmint *p_nv );

/* sur293.c   varkon_sur_readarr    Retrieve pts/deriv. from arrays   */
   DBstatus   varkon_sur_readarr (
              char *metod, DBint np_u, DBint np_v, DBVector *p_poi,
              DBVector *p_utan, DBVector *p_vtan, DBVector *p_twist,
              DBint *p_f_utan, DBint *p_f_vtan, DBint *p_f_twist,
              SURPOI **pp_ptsout );

/* sur288.c   varkon_sur_poiprint   Printout of table SURPOI          */
   DBstatus   varkon_sur_poiprint (
              DBint nu_poi, DBint nv_poi, SURPOI *p_surpoi,
              DBint s_record, DBint e_record );

/* sur289.c   varkon_sur_poispline  Spline tangents for SUR_SPLARR    */
   DBstatus   varkon_sur_poispline (
              DBint s_case, DBint iu_s, DBint iv_s, DBint iu_d,
              DBint iv_d, DBint iu_e, DBint iv_e, DBint f_p,
              DBint nu_poi, DBint nv_poi, DBint *p_f_utan,
              DBint *p_f_vtan, DBint *p_f_twist, SURPOI *p_surpoi,
              DBint *p_dflag, DBint *p_iuu, DBint *p_ivu );

/* sur297.c   varkon_sur_poiadr     Record number for pt in POISUR    */
   DBstatus   varkon_sur_poiadr (
              DBint iuc, DBint ivc, SURPOI *p_surpoi, DBint nu_poi,
              DBint nv_poi, DBint iu_s, DBint iv_s, DBint iu_e,
              DBint iv_e, DBint f_p, DBint *p_recno, DBint *p_dflag );

/* sur298.c   varkon_sur_poitanl    Corner tangent lengths for surf.  */
   DBstatus   varkon_sur_poitanl (
              DBint c_case, DBint iu_s, DBint iv_s, DBint iu_d,
              DBint iv_d, DBint iu_e, DBint iv_e, DBint f_p,
              DBint nu_poi, DBint nv_poi, SURPOI *p_surpoi,
              DBint *p_dflag, DBint *p_iuu, DBint *p_ivu );

/* sur299.c   varkon_sur_poitwist   Twist vector calculation          */
   DBstatus   varkon_sur_poitwist (
              DBint c_case, DBint iu_s, DBint iv_s, DBint iu_d,
              DBint iv_d, DBint iu_e, DBint iv_e, DBint f_p,
              DBint nu_poi, DBint nv_poi, SURPOI *p_surpoi,
              DBint *p_dflag, DBint *p_iuu, DBint *p_ivu );

/* sur400.c   varkon_sur_comp       Composite surface (SUR_COMP)      */
   DBstatus   varkon_sur_comp (
              DBSurf   sur_all[], DBPatch *p_pat_all[], DBint no_surf,
              DBint c_case, DBSurf *p_surout, DBPatch **pp_patout );

/* sur410.c   varkon_sur_comptwo    Combine two surfaces              */
   DBstatus   varkon_sur_comptwo (
              DBSurf  *p_sur_1, DBPatch *p_pat_1,
              DBSurf  *p_sur_2, DBPatch *p_pat_2, SURCOMP *p_scomp,
              DBSurf  *p_surout, DBPatch **pp_patout );

/* sur412.c   varkon_sur_compana    Analyses surface composite case   */
   DBstatus   varkon_sur_compana (
              DBSurf  *p_sur_1, DBPatch *p_pat_1,
              DBSurf  *p_sur_2, DBPatch *p_pat_2, SURCOMP *p_scomp );

/* sur414.c   varkon_sur_compide    Identical boundaries? (SUR_COMP)  */
   DBstatus   varkon_sur_compide (
              DBSurf *p_sur_1, DBPatch *p_pat_1,
              DBSurf *p_sur_2, DBPatch *p_pat_2, SURCOMP *p_scomp,
              DBint uv_type1, DBfloat par_val1, DBint uv_type2,
              DBfloat  par_val2 );

/* sur416.c   varkon_sur_compwho    Combine two whole surfaces        */
   DBstatus   varkon_sur_compwho (
              DBSurf  *p_sur_1, DBPatch *p_pat_1,
              DBSurf  *p_sur_2, DBPatch *p_pat_2, SURCOMP *p_scomp,
              DBSurf  *p_surout, DBPatch *p_patout );

/*
***NURBS surfaces
*/

/* sur890.c   varkon_sur_nurbsarr   Create NURB_SUR with SUR_NURBSARR */
   DBstatus   varkon_sur_nurbsarr (
              DBint order_u, DBint order_v, DBint nk_u, DBint nk_v,
              DBVector *p_cpts, DBfloat *p_uknots, DBfloat *p_vknots,
              DBfloat *p_w, DBSurf *p_surout, DBPatch **pp_patout );


/* sur891.c   varkon_sur_nurbsacp   Is NURB_SUR closed/periodic ?     */
   DBstatus   varkon_sur_nurbsacp (
              DBint f_surcur, DBint order_u, DBint order_v,
              DBint nk_u, DBint nk_v, DBVector *p_cpts, 
              DBfloat *p_uknots, DBfloat *p_vknots, DBfloat  *p_w,
              DBint *p_form_u, DBint *p_form_v );

/* sur892.c   varkon_sur_nurbsnon   Calculate number of NURBS nodes   */
   DBstatus   varkon_sur_nurbsnon (
              DBint order_u, DBint order_v, DBint nk_u, DBint nk_v,
              DBint *p_np_u, DBint *p_np_v,
              DBint *p_degree_u, DBint *p_degree_v );

/* sur893.c   varkon_sur_nurbsche   Check NURBS surface/curve data    */
   DBstatus   varkon_sur_nurbsche (
              DBint f_surcur, DBint order_u, DBint order_v,
              DBint nk_u, DBint nk_v,
              DBVector *p_cpts, DBfloat *p_uknots, DBfloat *p_vknots,
              DBfloat *p_w, DBint *p_error_code);

/* sur894.c   varkon_sur_nurbswrite Write NURBS data to a file        */
   DBstatus   varkon_sur_nurbswrite (
              DBSurf *p_sur, DBPatch   *p_pat,
              char * f_name_in, DBint print_flag);


/*
***Patch creation functions
*/


/* sur254.c   varkon_pat_biccre1    Bicubic patch from 4 corner pts   */
   DBstatus   varkon_pat_biccre1 (
              EVALS *p_r00, EVALS *p_r10, EVALS *p_r01, EVALS *p_r11,
              GMPATC *p_patc );

/* sur256.c   varkon_pat_biccre2    Create a ruled cubic patch        */
   DBstatus   varkon_pat_biccre2 (
              DBSeg  *p_u0, DBSeg  *p_u1, GMPATC *p_patc );

/* sur260.c   varkon_sur_hardptcre1 Create parabola hardpoint segment */
   DBstatus   varkon_sur_hardptcre1 (
              DBSeg  *p_u0, DBSeg  *p_um, DBSeg  *p_u1, DBSeg  *p_hp );

/* sur262.c   varkon_pat_hardptcre2 Epsilon hardpoint curve segment   */
   DBstatus   varkon_pat_hardptcre2 (
              DBSeg *p_l1, DBSeg *p_t1, DBSeg *p_l2, DBSeg *p_t2,
              DBfloat  u_s, DBfloat  u_e, DBSeg   *p_hp );

/* sur252.c   varkon_pat_ratcre2    Create a ruled rational patch     */
   DBstatus   varkon_pat_ratcre2 (
              DBSeg  *p_u0, DBSeg  *p_u1, GMPATR *p_patr );

/* sur280.c   varkon_pat_bicbez     Bicubic to Bezier patch           */
   DBstatus   varkon_pat_bicbez (
              GMPATC *p_i, GMPATBR3 *p_o );

/* sur630.c   varkon_pat_bicrep     Reparameterization bicubic patch  */
   DBstatus   varkon_pat_bicrep (
              GMPATC *p_i, DBfloat s_u, DBfloat s_v,
              DBfloat e_u, DBfloat e_v, GMPATC *p_o );

/* sur635.c   varkon_pat_loftrev    Reverse a conic lofting patch     */
   DBstatus   varkon_pat_loftrev (
              GMPATL *p_ipatl, DBint rcase, GMPATL *p_opatl );

/*
***Import of VDAFS surfaces 
*/

/* sur550.c   varkon_vda_rsurm      From VDAFS surface to POL_SUR     */
   DBstatus   varkon_vda_rsurm (
              char *infile, DBint sur_no,
              DBSurf *p_sur, DBPatch **pp_pat, char sur_name[] );

/* sur549.c   varkon_vda_nosur      No. of surfaces in a VDA-FS file  */
   DBstatus   varkon_vda_nosur (
              char    *infile, DBint   *p_no_sur );

/* sur551.c   varkon_vda_rsur1      No. of patches in VDA-FS surface  */
   DBstatus   varkon_vda_rsur1 (
              FILE *f_vda, DBint sur_no, DBint *p_nu, DBint *p_nv,
              char sur_name[], char rest[] );

/* sur552.c   varkon_vda_rsur2      Parameter values VDA-FS surface   */
   DBstatus   varkon_vda_rsur2 (
              FILE *f_vda, char rest[], DBint nu, DBint nv, 
              DBPatch *p_pat );

/* sur553.c   varkon_vda_rsur3      Retrieve floats from VDA-FS line  */
   DBstatus   varkon_vda_rsur3 (
              char rad[], DBint *p_n_flt, DBfloat all_flt[] );

/* sur554.c   varkon_vda_rsur4      Coefficients for VDA-FS surface   */
   DBstatus   varkon_vda_rsur4 (
              FILE  *f_vda, DBint nu, DBint nv,
              DBPatch *p_pat, DBint *p_sur_type );

/* sur555.c   varkon_vda_rp3        Read P3_PAT  VDA-FS coefficients  */
   DBstatus   varkon_vda_rp3 (
              FILE *f_vda, DBint n_degu, DBint n_degv, DBPatch *p_t );

/* sur556.c   varkon_vda_rp5        Read P5_PAT  VDA-FS coefficients  */
   DBstatus   varkon_vda_rp5 (
              FILE *f_vda, DBint n_degu, DBint n_degv, DBPatch *p_t );

/* sur557.c   varkon_vda_rp7        Read P7_PAT  VDA-FS coefficients  */
   DBstatus   varkon_vda_rp7 (
              FILE *f_vda, DBint n_degu, DBint n_degv, DBPatch *p_t );

/* sur558.c   varkon_vda_rp9        Read P9_PAT  VDA-FS coefficients  */
   DBstatus   varkon_vda_rp9 ( 
              FILE *f_vda, DBint n_degu, DBint n_degv, DBPatch *p_t );

/* sur559.c   varkon_vda_rp21       Read P21_PAT VDA-FS coefficients  */
   DBstatus   varkon_vda_rp21 (
              FILE *f_vda, DBint n_degu, DBint n_degv, DBPatch *p_t );

/*
***Ferguson surfaces
*/

/* sur290.c   varkon_sur_wferguson  Write CUB_SUR as Ferguson         */
   DBstatus   varkon_sur_wferguson (
              DBSurf  *p_sur, DBPatch *p_pat, char  *outfile );

/* sur291.c   varkon_sur_rferguson  Patch corner points to CUB_SUR    */
   DBstatus   varkon_sur_rferguson (
              char *infile, DBSurf *p_surout, DBPatch **pp_patout );

/*
***Saab export function to Catia
*/

/* sur292.c   varkon_sur_wnmgcatia  CUB_SUR to NMG->Catia format      */
   DBstatus   varkon_sur_wnmgcatia (
              DBSurf *p_sur, DBPatch *p_pat, char *outfile,
              char *s_model );

/*
***Patch print functions
*/

/* sur233.c   varkon_pat_pribic     Printout of bicubic patch data    */
   DBstatus   varkon_pat_pribic (
              GMPATC  *p_p );

/* sur234.c   varkon_pat_prirat     Printout of rational patch data   */
   DBstatus   varkon_pat_prirat (
              GMPATR  *p_p );


/*
***Function prototypes for surface evaluation functions
*/

/* sur210.c   varkon_sur_eval       Surface evaluation routine        */
   DBstatus   varkon_sur_eval (
              DBSurf *p_sur, DBPatch *p_pat, DBint    icase,
              DBfloat  uglob, DBfloat  vglob, EVALS   *p_xyz ) ;

/* sur213.c   varkon_pat_evalbox    BOX_SUR patch evaluation routine  */
   DBstatus   varkon_pat_evalbox (
              DBSurf *p_sur, DBPatch *p_pat, DBint icase,
              DBfloat u_pat, DBfloat v_pat, EVALS *p_xyz );

/* sur220.c   varkon_pat_eval       Surface patch evaluation fctn     */
   DBstatus   varkon_pat_eval (
              DBSurf *p_sur, DBPatch *p_pat, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur214.c   varkon_sur_uvsegeval  Coord. and derivatives for UV pt  */
   DBstatus   varkon_sur_uvsegeval (
              DBCurve *p_cur, DBSeg *p_seg, EVALC *p_xyz_c );

/* sur786.c   varkon_seg_uveval     U,V curve segm. evaluation fctn   */
   DBstatus   varkon_seg_uveval (
              DBCurve *p_cur, DBSeg *p_seg, EVALC *p_xyz );

/* sur217.c   varkon_seg_offset     Offset coordinates & derivatives  */
   DBstatus   varkon_seg_offset (
              DBVector  r, DBVector  drdt, DBVector  d2rdt2,
              DBfloat   offset, DBint     evltyp, DBVector  n_p,
              DBVector *p_r_o, DBVector *p_drdt_o,
              DBVector *p_d2rdt2_o );

/* sur952.c   varkon_sur_normkappa  Normal curvature                  */
   DBstatus   varkon_sur_normkappa (
              DBfloat  dudt, DBfloat  dvdt, EVALS   *p_xyz,
              DBfloat   *p_nkappa );

/* sur221.c   varkon_pat_biceval    Bicubic patch evaluation fctn     */
   DBstatus   varkon_pat_biceval (
              GMPATC *p_patc, DBint icase, DBfloat u, DBfloat v,
              EVALS  *p_xyz  );

/* sur224.c   varkon_pat_rateval    Rational patch evaluation fctn    */
   DBstatus   varkon_pat_rateval (
              GMPATR  *p_patr, DBint icase, 
              DBfloat u_pat, DBfloat v_pat, EVALS   *p_xyz );

/* sur222.c   varkon_pat_conloft    Conic lofting evaluation fctn     */
   DBstatus   varkon_pat_conloft (
              GMPATL *p_patl, DBint  icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur227.c   varkon_pat_rotloft    Rotation lofting evaluation fctn  */
   DBstatus   varkon_pat_rotloft (
              GMPATL *p_patl, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur228.c   varkon_pat_rulloft    Ruled    lofting evaluation fctn  */
   DBstatus   varkon_pat_rulloft (
              GMPATL *p_patl, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur229.c   varkon_pat_sweeploft  Sweep lofting evaluation fctn     */
   DBstatus   varkon_pat_sweeploft (
              GMPATL *p_patl, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur226.c   varkon_pat_coneval    Consurf patch evaluation fctn     */
   DBstatus   varkon_pat_coneval (
              GMPATN *p_patn, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS  *p_xyz );

/* sur500.c   varkon_pat_p3eval     Evaluation of GMPATP3             */
   DBstatus   varkon_pat_p3eval (
              GMPATP3  *p_pat3, DBint    icase,
              DBfloat  u_l, DBfloat  v_l, EVALS   *p_xyz );

/* sur501.c   varkon_pat_p5eval     Evaluation of GMPATP5             */
   DBstatus   varkon_pat_p5eval (
              GMPATP5  *p_pat5, DBint    icase,
              DBfloat  u_l, DBfloat  v_l, EVALS   *p_xyz );

/* sur502.c   varkon_pat_p7eval     Evaluation of GMPATP7             */
   DBstatus   varkon_pat_p7eval (
              GMPATP7  *p_pat7, DBint    icase,
              DBfloat  u_l, DBfloat  v_l, EVALS   *p_xyz );

/* sur503.c   varkon_pat_p9eval     Evaluation of GMPATP9             */
   DBstatus   varkon_pat_p9eval (
              GMPATP9  *p_pat9, DBint    icase,
              DBfloat  u_l, DBfloat  v_l, EVALS   *p_xyz );

/* sur504.c   varkon_pat_p21eval    Evaluation of GMPATP21            */
   DBstatus   varkon_pat_p21eval (
              GMPATP21 *p_pat21, DBint    icase,
              DBfloat  u_l, DBfloat  v_l, EVALS   *p_xyz );

/* sur242.c   varkon_pat_nurbs      NURBS evaluation function         */
   DBstatus   varkon_pat_nurbs (
              GMPATNU *p_patnu, DBint   icase, 
              DBfloat u_glob, DBfloat v_glob, EVALS   *p_xyz );


/* sur243.c   varkon_poi_nurbs      Set pointer to a NURBS node       */
   DBstatus   varkon_poi_nurbs   (
              GMPATNU *p_patnu, DBint rwcase,
              DBint i_up, DBint i_vp, DBHvector *p_pp );

/* sur245.c   varkon_sur_coonseval  Coons surface evaluation fctn     */
   DBstatus   varkon_sur_coonseval (
              GMPATB  *p_patb, DBint    icase,
              DBfloat  s_sur, DBfloat  t_sur, EVALS   *p_xyz );

/* sur250.c   varkon_pat_ratcre1    Create rational patch of 6 crv's  */
   DBstatus   varkon_pat_ratcre1 (
              DBSeg  *p_u0, DBSeg  *p_u1, DBSeg  *p_v0, DBSeg  *p_v1,
              DBSeg  *p_um, DBSeg  *p_vm, GMPATR *p_patr );

/* sur212.c   varkon_sur_princurv   Principal curvature               */
   DBstatus   varkon_sur_princurv (
              EVALS   *p_xyz );

/* sur240.c   varkon_pat_norm       Surface normal with derivatives   */
   DBstatus   varkon_pat_norm (
              DBint  icase, EVALS  *p_xyz );

/* sur219.c   varkon_sur_normderiv  Surface normal and derivatives    */
   DBstatus   varkon_sur_normderiv (
              EVALS  *p_xyz );

/* sur232.c   varkon_pat_pritop     Print topological patch data      */
   DBstatus   varkon_pat_pritop (
              DBSurf *p_sur, DBPatch *p_pat,
              DBint    aiu, DBint    aiv );

/* sur211.c   varkon_sur_patadr     Patch adress for given UV point   */
   DBstatus   varkon_sur_patadr (
              DBfloat  uglob, DBfloat  vglob, DBint nu, DBint nv,
              DBint *p_iu,DBint *p_iv,DBfloat *p_u, DBfloat *p_v );

/* sur230.c   varkon_sur_nopatch    Retrieve number of patches        */
   DBstatus   varkon_sur_nopatch (
              DBSurf *p_sur, DBint  *p_nu, DBint  *p_nv,
              DBint  *p_surtype);

/* sur235.c   varkon_pat_loftcur    Conic lofting curves evaluation   */
   DBstatus   varkon_pat_loftcur (
              GMPATL *p_patl, DBfloat u_pat, DBfloat v_pat,
              DBVector *p_s_r, DBVector *p_s_drdt, DBVector *p_s_d2rdt2,
              DBVector *p_ls_r, DBVector *p_ls_drdt, 
              DBVector *p_ls_d2rdt2,
              DBVector *p_ts_r, DBVector *p_ts_drdt, 
              DBVector *p_ts_d2rdt2,
              DBVector *p_m_r, DBVector *p_m_drdt, DBVector *p_m_d2rdt2,
              DBint *p_m_f, DBVector *p_le_r, DBVector *p_le_drdt,
              DBVector *p_le_d2rdt2,
              DBVector *p_te_r, DBVector *p_te_drdt,
              DBVector *p_te_d2rdt2,
              DBVector *p_g_r, DBVector *p_g_drdt, 
              DBVector *p_g_d2rdt2 );

/* sur236.c   varkon_sur_secteval   Evaluate LFT_SUR input curve      */
   DBstatus   varkon_sur_secteval (
              DBSurf *p_sur, DBPatch *p_pat, 
              DBfloat uglob, DBfloat vglob,
              char     *ecase, DBVector *p_outvec );

/* sur209.c   varkon_sur_eval_gm    Surface (DB) evaluation routine   */
   DBstatus   varkon_sur_eval_gm (
              DBSurf *p_sur, DBint icase, DBfloat u_mbs, DBfloat v_mbs,
              EVALS *p_xyz );

/* sur215.c   varkon_sur_mbseval    Surface MBS evaluation routine    */
   DBstatus   varkon_sur_mbseval (
              DBSurf   *p_sur, DBPatch  *p_pat, char     *ecase,
              DBfloat   uglob, DBfloat   vglob, DBVector *p_outvec );

/* sur216.c   varkon_cur_mbseval    Curve   MBS evaluation routine    */
   DBstatus   varkon_cur_mbseval (
              DBCurve *p_cur, DBSeg *p_seg, char *ecase, DBfloat tglob,
              DBVector *p_outvec );

/* sur709.c   varkon_bpl_on         Point on B-plane for U,V          */
   DBstatus   varkon_bpl_on (
              DBBplane *p_bplin, DBfloat u_b, DBfloat v_b,
              DBVector *p_point );


/*
***Curve conversion and approximation functions
*/

/* sur715.c   varkon_cur_fromarc    Convert a circle to a curve       */
   DBstatus   varkon_cur_fromarc (
              DBArc *p_arc, DBCurve *p_cur, DBSeg *p_seg );

/* sur716.c   varkon_cur_fromlin    Convert a line to a curve         */
   DBstatus   varkon_cur_fromlin (
              DBLine *p_lin, DBCurve *p_cur, DBSeg *p_seg );

/* sur722.c   varkon_cur_approx     Approximate to a given curve type */
   DBstatus   varkon_cur_approx (
              DBCurve *p_cur, DBSeg *p_seg, DBint c_type, DBint n_req,
              DBint t_incur, DBint a_case,
              DBfloat in_ctol, DBfloat in_ntol, DBfloat in_rtol,
              DBfloat d_che, DBCurve *p_ocur, DBSeg **pp_oseg );

/* sur723.c   varkon_cur_approxseg  Approximate, given no of segments */
   DBstatus   varkon_cur_approxseg (
              DBCurve *p_cur, DBSeg   *p_seg, APPCUR  *p_appdat,
              DBCurve *p_ocur, DBSeg   *p_oseg );

/* sur724.c   varkon_cur_approxrdr  Approxim., reduce with rationals  */
   DBstatus   varkon_cur_approxrdr (
              DBCurve *p_cur, DBSeg   *p_seg, APPCUR  *p_appdat,
              DBCurve *p_ocur, DBSeg   *p_oseg );

/* sur725.c   varkon_cur_approxadd  Approximate, add segments         */
   DBstatus   varkon_cur_approxadd (
              DBCurve *p_cur, DBSeg   *p_seg, APPCUR  *p_appdat,
              DBCurve *p_ocur, DBSeg   *p_oseg );

/* sur726.c   varkon_cur_approxrci  Approximate to circles + lines    */
   DBstatus   varkon_cur_approxrci (
              DBCurve *p_cur, DBSeg   *p_seg, APPCUR  *p_appdat,
              DBCurve *p_ocur, DBSeg   *p_oseg );

/* sur960.c   varkon_sur_scur_gra   Approx. UV curve for graphics     */
   DBstatus   varkon_sur_scur_gra (
              DBCurve *p_cur, DBSeg   *p_seg, DBSeg  **pp_graseg );

/* sur962.c   varkon_sur_graphic    Surface graphical representation  */
   DBstatus   varkon_sur_graphic (
              DBSurf  *p_sur, DBPatch *p_pat, DBptr pgm_sur,
              DBint g_case, DBSeg *p_gseg[6] );

/* sur963.c   varkon_sur_granurbs   Surface graphical NURBS represen. */
   DBstatus   varkon_sur_granurbs (
              DBSurf *p_sur, DBPatch *p_pat, DBptr pgm_sur,
              gmint g_case, GLfloat **pp_kvu, GLfloat **pp_kvv,
              GLfloat **pp_cpts );

/* sur970.c   varkon_sur_scur_uv    Scaled UV curve in the UV plane   */
   DBstatus   varkon_sur_scur_uv (
              DBCurve *p_cur, DBSeg *p_seg, DBSeg **pp_graseg );

/* sur951.c   varkon_sur_scur_mbs   MBS code for curve as debug       */
   DBstatus   varkon_sur_scur(
              DBCurve *pcur, DBSeg *pseg );

/*
***Curve evaluation functions
*/

/* sur950.c   varkon_sur_cureval    Coord. and derivatives for UV pt  */
   DBstatus   varkon_sur_cureval (
              DBfloat uv_in[], EVALS *p_xyz, short rcode, 
              DBfloat out[] );

/* sur720.c   varkon_cur_intplan    Curve/plane intersect             */
   DBstatus   varkon_cur_intplan (
              DBCurve *p_cur, DBSeg   *p_seg, DBfloat  plane[4],
              DBint   *p_nint, DBfloat  u_out[INTMAX]);

/* sur721.c   varkon_cur_intbplan   Curve/B-plane intersect           */
   DBstatus   varkon_cur_intbplan (
              DBCurve  *p_cur, DBSeg    *p_seg, DBBplane *p_bpl,
              DBint *p_nint, DBfloat u_out[INTMAX], 
              DBVector r_out[INTMAX] );


/* sur780.c   varkon_cur_ratevalh   Homogen. coord. and derivatives   */
   DBstatus   varkon_cur_ratevalh(
              DBSeg   *pseg, DBfloat  u_s, EVALCH  *pout );



/* sur782.c   varkon_seg_eval       Curve segment evaluation fctn     */
   DBstatus   varkon_seg_eval (
              DBCurve *p_cur, DBSeg   *p_seg, DBint    icase,
              DBfloat  u_seg, EVALC   *p_xyz );


/* sur784.c   varkon_cur_eval       Curve   evaluation routine        */
   DBstatus   varkon_cur_eval (
              DBCurve *p_cur, DBSeg   *p_seg, DBint    icase,
              DBfloat  u_global, EVALC   *p_xyz );

/* sur785.c   varkon_cur_segadr     Segment adress for given T value  */
   DBstatus   varkon_cur_segadr (
              DBfloat tglob, DBint nseg, DBint *p_iseg, DBfloat *p_t );


/* sur799.c   varkon_cur_print      Debug printout of curve data      */
   DBstatus   varkon_cur_print (
              DBCurve *p_cur, DBSeg   *p_seg, DBint    p_case );

/*
***Solvers
*/


/* sur900.c   varkon_sur_num1       Surface one param. calculation    */
   DBstatus   varkon_sur_num1 (
              DBSurf *p_sur, DBPatch *p_pat, DBfloat t_in[2][2],
              IRUNON  *p_comp, DBint *p_npts, EVALS xyz_a[SMAX],
              DBint  *p_b_case );

/* sur901.c   varkon_cur_num1       Curve   one param. calculation    */
   DBstatus   varkon_cur_num1(
              DBCurve *p_cur, DBSeg  *p_seg, DBfloat t_in[2],
              IRUNON  *p_comp, DBint *p_npts, EVALC  xyz_a[SMAX] );

/* sur902.c   varkon_sur_num2       Surface two param. calculation    */
   DBstatus   varkon_sur_num2 (
              DBSurf *p_sur, DBPatch *p_pat, IRUNON *p_comp,
              SPOINT *p_spt, DBint *p_npts, EVALS xyz_a[SMAX] );

/* sur903.c   varkon_sur_num3       Differential equation solution    */
   DBstatus   varkon_sur_num3 (
              DBSurf  *p_sur, DBPatch *p_pat, DBptr pgm_sur,
              IRUNON *p_comp, PBOUND *p_pbtable, DBint cur_pat,
              DBCurve *p_cur, DBSeg *p_seg, DBint n_in,
              DBint *p_n_out );

/* sur910.c   varkon_sur_defpatb    Define calcul. patch boundaries   */
   DBstatus   varkon_sur_defpatb (
              DBSurf *p_sur, DBPatch *p_pat, DBint s_type,
              DBint nu, DBint nv, DBfloat s_lim[2][2],
              DBint ctype, DBint acc, DBint np_u, DBint np_v,
              DBint   pb_alloc, PBOUND *p_pbtable, DBint  *p_nr );

/* sur914.c   varkon_pat_chebound   Check intersect. box and plane,.  */
   DBstatus   varkon_pat_chebound (
              DBSurf *p_sur, DBPatch *p_patr, IRUNON  *p_comp,
              DBfloat u_s, DBfloat v_s, DBfloat u_e, DBfloat v_e,
              BBOX    *p_pbox, BCONE *p_pcone, DBint *p_sflag );

/* sur915.c   varkon_cur_bound      Boundary box and cone for curve   */
   DBstatus   varkon_cur_bound (
              DBCurve *p_cur, DBSeg *p_seg, BBOX *p_cbox );

/* sur916.c   varkon_pat_trabound   Transformate boundary box & cone  */
   DBstatus   varkon_pat_trabound (
              BBOX    *p_inpbox, BCONE   *p_inpcone, DBTmat  *p_c,
              BBOX    *p_outpbox, BCONE   *p_outpcone );

/* sur920.c   varkon_sur_epts       Entry/exit points in patches      */
   DBstatus   varkon_sur_epts (
              DBSurf  *p_sur, DBPatch *p_pat, IRUNON  *p_comp,
              PBOUND  *p_pbtable, DBint  pbn, EPOINT *p_etable,
              DBint   *p_en );

/* sur921.c   varkon_sur_epts_mbs   MBS code (lines) of table EPOINT  */
   DBstatus   varkon_sur_epts_mbs (
              DBSurf  *p_sur, DBPatch *p_pat, EPOINT  *p_etable,
              DBint    no_ep );

/* sur922.c   varkon_sur_epts_ord   Order & find branches in EPOINT   */
   DBstatus   varkon_sur_epts_ord (
              EPOINT  *p_etable, DBint    no_ep, DBint   *p_no_br,
              DBint    cbsr[] );

/* sur924.c   varkon_sur_ppts       Calculate surface SPOINT points   */
   DBstatus   varkon_sur_ppts (
              DBSurf  *psura[ISMAX], DBPatch *ppata[ISMAX],
              DBVector s_uvpt_a[], DBint    n_isur, IRUNON  *p_comp,
              SPOINT  *p_stable, DBint   *p_sn );

/* sur925.c   varkon_sur_d3toduv    UV tangent for given R*3 tangent  */
   DBstatus   varkon_sur_d3toduv (
              EVALS *p_xyz, DBVector *p_r3tang, DBVector *p_uvtang );

/* sur928.c   varkon_sur_epts_ele   Eliminate equal EPOINT records    */
   DBstatus   varkon_sur_epts_ele (
              EPOINT  *p_etable, DBint    no_ep );

/* sur926.c   varkon_sur_uvtang     Calc. UV "interrogation" tangent  */
   DBstatus   varkon_sur_uvtang (
              DBint n_points, EVALS xyzpt[9], EVALC curpt[9],
              IRUNON *p_comp, short  b_flag );

/* sur930.c   varkon_sur_curbr      Create one crv branch in EPOINT   */
   DBstatus   varkon_sur_curbr (
              DBSurf *p_sur, DBPatch *p_pat, IRUNON *p_comp,
              EPOINT *p_etable, DBint no_ep, DBint no_br,
              DBint cbsr[CBMAX], DBint cur_no, DBCurve *p_cur,
              DBSeg  *p_seg );

/* sur940.c   varkon_pat_uvcur      Create a UV curve in a patch      */
   DBstatus   varkon_pat_uvcur (
              DBSurf *p_sur, DBPatch *p_pat, IRUNON *p_comp,
              DBfloat uls, DBfloat vls,
              DBfloat ule, DBfloat vle,
              DBfloat ustart, DBfloat vstart,
              DBfloat uend  , DBfloat vend,
              DBfloat us_t, DBfloat vs_t,
              DBfloat ue_t, DBfloat ve_t,
              DBint   start_seg, DBint   max_seg,
              DBCurve *p_cur, DBSeg *p_seg, DBint  *p_nseg );

/* sur980.c   varkon_sur_order      Order surface (solution) points   */
   DBstatus   varkon_sur_order  (
              DBVector *p_refpt, EVALS xyz_in[], DBint  n_inpts,
              DBint ocase_in, EVALS xyz_out[], DBint *p_n_outpts);


/* sur982.c   varkon_cur_order      Order curve   (solution) points   */
   DBstatus   varkon_cur_order (
              DBVector *p_refpt, EVALC xyz_in[], DBint  n_inpts,
              DBint ocase, EVALC xyz_out[], DBint* p_n_outpts );


/* sur983.c   varkon_cur_order_2    Order curve   (solution) points   */
   DBstatus   varkon_cur_order_2 (
              DBVector *p_refpt, EVALC   xyz_in[], DBint   n_inpts,
              DBint   ocase, EVALC   xyz_out[], DBint   ord_out[],
              DBint  *p_n_outpts );

/* sur984.c   varkon_sur_selsol     Select surface (solution) points  */
   DBstatus   varkon_sur_selsol (
              DBVector *p_refpt, DBVector *p_refvec, EVALS xyz_in[],
              DBint n_inpts, DBint scase, EVALS xyz_out[],
              DBint *p_n_outpts );

/* sur178.c   varkon_sur_aplane     Approximate a surface for APLANE  */
   DBstatus   varkon_sur_aplane (
              DBSurf  *p_sur, DBPatch *p_pat, PBOUND  *p_pbtable,
              DBint    pbn, DBint    req_sur_no, APLANE  *p_pltable,
              DBint   *p_pl );

/* sur923.c   varkon_sur_epts2      Entry/exit patch pts, 2 surfaces  */
   DBstatus   varkon_sur_epts2 (
              DBSurf  *p_sur1, DBPatch *p_pat1, DBptr    p_gm1,
              DBSurf  *p_sur2, DBPatch *p_pat2, DBptr    p_gm2,
              IRUNON  *p_comp, PBOUND  *p_pbtable, DBint    pbn,
              APOINT  *p_apts, DBint    napts,
              EPOINT  *p_etable1, DBint   *p_en1,
              EPOINT  *p_etable2, DBint   *p_en2 );

/* sur311.c   varkon_pat_uvloc      Map UV pt to the patch UV area    */
   DBstatus   varkon_pat_uvloc (
              DBSurf *p_sur, DBPatch *p_pat, DBint i_u, DBint i_v,
              DBfloat u_loc, DBfloat v_loc,
              DBfloat *p_u_map, DBfloat *p_v_map,
              DBfloat *p_u_s, DBfloat *p_u_e,
              DBfloat *p_v_s, DBfloat *p_v_e );

/* sur360.c   varkon_sur_uvmap      Map MBS UV pt to surpac UV point  */
   DBstatus   varkon_sur_uvmap (
              DBSurf *p_sur,DBPatch *p_pat,DBfloat u_mbs,DBfloat v_mbs,
              DBfloat *p_u_map, DBfloat *p_v_map );

/* sur361.c   varkon_pat_uvmap      Map MBS UV pt to patch  UV point  */
   DBstatus   varkon_pat_uvmap (
              DBSurf *p_sur, DBPatch *p_pat, 
              DBfloat u_loc, DBfloat v_loc,
              DBfloat *p_u_map, DBfloat *p_v_map );

/* sur362.c   varkon_sur_uvmapr     Map NURBS UV pt to MBS UV point   */
   DBstatus   varkon_sur_uvmapr (
              DBSurf  *p_sur, DBPatch *p_pat, 
              DBfloat  u_nurb, DBfloat  v_nurb,
              DBfloat *p_u_map, DBfloat *p_v_map );

/* sur363.c   varkon_cur_uvmap      Map a UV (sur150) curve           */
   DBstatus   varkon_cur_uvmap (
              DBSurf  *p_sur, DBPatch *p_pat, 
              DBCurve *p_cur, DBSeg   *p_seg );


/* sur365.c   varkon_sur_uvnmap     Map a UV net (for NURBS surface)  */
   DBstatus   varkon_sur_uvnmap (
              DBSurf *p_sur, DBPatch *p_pat, PBOUND  *p_pbtable,
              DBint    no_rec );

/* sur918.c   varkon_sur_selpati    Select intersecting patches       */
   DBstatus   varkon_sur_selpati (
              DBSurf  *p_sur1, DBPatch *p_pat1, DBint   s_type1,
              DBint   nu1, DBint   nv1, DBfloat s_lim1[2][2],
              DBSurf  *p_sur2, DBPatch *p_pat2, DBint   s_type2,
              DBint   nu2, DBint   nv2, DBfloat s_lim2[2][2],
              IRUNON *p_comp, PBOUND *p_pbtable, DBint  *p_nr );

/* sur919.c   varkon_sur_selpatseg  Select segment intersect patches  */
   DBstatus   varkon_sur_selpatseg (
              DBSurf   *p_sur, DBPatch  *p_pat, DBint    s_type,
              DBint n_u, DBint n_v, DBfloat  s_lim[2][2],
              DBCurve *p_cur, DBSeg   *p_seg, IRUNON  *p_comp,
              PBOUND **pp_pbtable, DBint   *p_nr );




/* sur904.c   varkon_pat_segint     Curve segment/patch intersect     */
   DBstatus   varkon_pat_segint (
              DBSurf  *p_sur, DBPatch *p_pat,
              DBCurve *p_cur, DBSeg   *p_seg, IRUNON  *p_comp,
              DBint *p_npts, EVALS xyz_a[SMAX], EVALC xyz_b[SMAX] );

/*
***Vector functions
*/

/* sur670.c   varkon_vec_projpla    Project vector onto a plane       */
   DBstatus   varkon_vec_projpla (
              DBVector v_in, DBVector plane,
              DBint    ocase, DBVector *p_v_out );


/* sur690.c   varkon_tra_intlinsur  Matrix for line/surface intersect */
   DBstatus   varkon_tra_intlinsur (
              DBVector *p_extpt,DBVector *p_v_proj,DBTmat *p_pat_tra );

/*
***Coons patch functions
*/

/* sur270.c   varkon_pat_coonscre1  Coons patch from 4 curve segments */
   DBstatus   varkon_pat_coonscre1 (
              DBSeg *p_u0, DBfloat s_u0, DBfloat e_u0, DBSeg *p_u1,
              DBfloat s_u1, DBfloat e_u1, DBSeg *p_v0, DBfloat s_v0,
              DBfloat e_v0, DBSeg *p_v1, DBfloat s_v1, DBfloat e_v1,
              GMPATB *p_patb );

/* sur271.c   varkon_pat_coonscre2  Coons patch from 2 curve segments */
   DBstatus   varkon_pat_coonscre2 (
               DBSeg *p_u0, DBfloat s_u0, DBfloat e_u0, DBSeg *p_u1,
               DBfloat s_u1, DBfloat e_u1, DBSeg *p_v0, DBSeg *p_v1,
               GMPATB *p_patb );

/* sur272.c   varkon_pat_coonscre3  Coons patch from 3 curve segments */
   DBstatus   varkon_pat_coonscre3 (
              DBSeg  *p_u0, DBfloat s_u0, DBfloat e_u0,
              DBSeg  *p_u1, DBfloat s_u1, DBfloat e_u1,
              DBSeg  *p_um, DBfloat s_um, DBfloat e_um,
              DBSeg  *p_v0, DBSeg  *p_v1, GMPATB *p_patb );

/* sur273.c   varkon_pat_coonscre4  Coons patch from 1 boundary crv   */
   DBstatus   varkon_pat_coonscre4 (
              DBCurve *p_cur, DBSeg   *p_seg, DBfloat  c_crit,
              GMPATB *p_patb );

/* sur225.c   varkon_pat_coonseval  Coons patch evaluation fctn       */
   DBstatus   varkon_pat_coonseval (
              GMPATB  *p_patb, DBint   icase,
              DBfloat u_pat, DBfloat v_pat, EVALS   *p_xyz );

/*
***Curve segment functions
*/


/* sur700.c   varkon_seg_parabola   Parabola interpolation of 3 pts   */
   DBstatus   varkon_seg_parabola (
              DBVector pt_a, DBVector pt_b, DBVector pt_c,
              DBint    u_case, DBSeg   *pseg );

/* sur701.c   varkon_seg_parab_b    Parabola from 2 pts and 1 tangent */
   DBstatus   varkon_seg_parab_b (
              DBVector pt_a, DBVector tan_a, DBVector pt_c,
              DBint u_case, DBSeg *pseg );

/* sur702.c   varkon_seg_parab_c    Parabolangent+2nd deriv.  */
   DBstatus   varkon_seg_parab_c (
              DBVector pt_a, DBVector tan_a, DBVector der2_a,
              DBSeg *pseg );

/* sur787.c   varkon_cur_extidpoint    Extend idpoint param. values   */
   DBstatus   varkon_cur_extidpoint (
              DBCurve *p_cur, DBSeg   *p_seg, DBint    c_case, 
              DBfloat *p_tstart, DBfloat *p_tend );


/* sur790.c   varkon_cur_reparam    Reparameterise a rational cubic   */
   DBstatus   varkon_cur_reparam (
              DBSeg *prin, DBfloat us_in, DBfloat ue_in,
              DBfloat us_out, DBfloat ue_out, DBSeg *prout );

/* sur792.c   varkon_cur_analysis   Position, tangent, curvat. steps  */
   DBstatus   varkon_cur_analysis (
              DBCurve *p_cur, DBSeg  *p_seg, DBint istart, DBint iend, 
              DBint    c_case, DBfloat  pcrit, 
              DBint *p_np, DBint piseg[SUANA], DBfloat    dcrit,
              DBint   *p_nd, DBint    diseg[SUANA], DBfloat    ccrit,
              DBint   *p_nc, DBint    ciseg[SUANA], DBint   *p_ns,
              DBint    siseg[SUANA] );

/* sur367.c   varkon_cur_usrche     Check CUR_USRDEF on a surface     */
   DBstatus   varkon_cur_usrche (
              DBSurf *p_sur, DBPatch *p_pat, DBCurve *p_cur,
              DBSeg *p_seg );

/* sur705.c   varkon_cir_twoseg     Create two 2D circle segments     */
   DBstatus   varkon_cir_twoseg (
              DBfloat poi1_x, DBfloat poi1_y, 
              DBfloat poi2_x, DBfloat poi2_y,
              DBfloat tang1_x_i, DBfloat tang1_y_i,
              DBfloat tang2_x_i, DBfloat tang2_y_i,
              DBfloat *p_centre1_x, DBfloat *p_centre1_y,
              DBfloat *p_centre2_x, DBfloat *p_centre2_y,
              DBfloat *p_circle1_r, DBfloat *p_circle2_r,
              DBfloat *p_angle_1, DBfloat *p_angle_2,
              DBfloat *p_poiout_x, DBfloat *p_poiout_y,
              DBfloat *p_tangout_x, DBfloat *p_tangout_y );

/* sur460.c   varkon_cur_spline     Spline with CUR_SPLARR(..chord..) */
   DBstatus   varkon_cur_spline (
              DBVector *p_pts, DBVector *p_tan, DBint *p_pcase,
              DBint n_poi, DBint s_case, DBfloat c_tol,
              DBCurve *p_cur, DBSeg *p_seg, DBfloat *p_c_act );

/* sur461.c   varkon_cur_saabspl    Create a Saab spline              */
   DBstatus   varkon_cur_saabspl (
              int n_poi, int i_start, int i_end,
              double x_coord[], double y_coord[], double z_coord[],
              double t_start[], double t_end[], int type,
              double idpoint, int reparam, double l_chord[],
              double x_coeff[][4], double y_coeff[][4],
              double z_coeff[][4] );

/*
***Planes and lines          
*/

/* sur674.c   varkon_lin_2plane     Two B-plane intersect line        */
   DBstatus   varkon_lin_2plane (
              DBBplane *p_bpl1, DBBplane *p_bpl2, DBint in_lin_no,
              DBVector *p_ps_out, DBVector *p_pe_out, DBint *p_nlin );


/* sur672.c   varkon_poi_3plane     Three plane intersect point       */
   DBstatus   varkon_poi_3plane (
              DBVector u_1, DBfloat d_1, DBVector u_2, DBfloat d_2,
              DBVector u_3, DBfloat d_3, DBVector *p_pint );


/* sur676.c   varkon_bpl_barycen    Barycentric coord.'s for B-plane  */
   DBstatus   varkon_bpl_barycen (
              DBBplane *p_bpl, DBVector *p_poi, DBfloat  *p_alpha,
              DBfloat  *p_beta, DBfloat  *p_gamma );

/* sur680.c   varkon_cur_averplan   Curve average plane               */
   DBstatus   varkon_cur_averplan (
              DBCurve *p_cur, DBSeg *p_seg, 
              DBfloat pcrit, DBfloat dcrit, DBint n_add,
              DBVector *p_cog, DBfloat *p_maxdev, DBfloat  a_plane[] );

/* sur710.c   varkon_lin_linlind    Shortest dist. between two lines  */
   DBstatus   varkon_lin_linlind (
              DBVector p0, DBVector u, DBVector p1, DBVector v,
              DBVector *p_pu, DBVector *p_pv, DBfloat *p_s,
              DBfloat *p_t, DBfloat *p_dist );

/* sur711.c   varkon_pla_distpt     Distance point to 3 point plane   */
   DBstatus   varkon_pla_distpt (
              DBVector *p_p1, DBVector *p_p2, DBVector *p_p3,
              DBVector *p_ext, DBVector *p_ref, DBfloat  *p_dist );


/* sur712.c   varkon_lin_ptclose    Closest point to a straight line  */
   DBstatus   varkon_lin_ptclose (
              DBVector *p_p0, DBVector *p_u, DBVector *p_pe,
              DBVector *p_pu, DBfloat  *p_s, DBfloat  *p_dist );

/* sur717.c   varkon_bpl_extend3    Extend a 3-point B-plane polygon  */
   DBstatus   varkon_bpl_extend3 (
              DBVector *p_p1, DBVector *p_p2, DBVector *p_p4,
              DBfloat   dist,
              DBVector *p_p1e, DBVector *p_p2e, DBVector *p_p4e );

/* sur718.c   varkon_bpl_analyse    Analyse (classify) a B-plane      */
   DBstatus   varkon_bpl_analyse (
              DBVector *p_p1, DBVector *p_p2,
              DBVector *p_p3, DBVector *p_p4, 
              DBint   *p_btype, DBint   *p_pnr );

/* sur719.c   varkon_bpl_extend     Extend a B-plane                  */
   DBstatus   varkon_bpl_extend (
              DBBplane *p_bplin, DBfloat dist, DBBplane *p_bplout );
/*
***Debug printout functions 
*/

/* sur223.c   varkon_pat_priconloft Printout of conic lofting patch   */
   DBstatus   varkon_pat_priconloft ( GMPATL *p_patl );


/*
***Transformation functions 
*/

/* sur640.c   varkon_evals_transf   Transformation of EVALS           */
   DBstatus    varkon_evals_transf  (
               EVALS *p_xyz_in, DBTmat *p_transf, EVALS *p_xyz_out);


/*
***Bounding boxes and cones
*/

/* sur911.c   varkon_sur_bound      Surface boundary boxes and cones  */
   DBstatus   varkon_sur_bound (
              DBSurf *p_sur, DBPatch *p_pat, DBint acc );

/* sur912.c   varkon_pat_bound      Boundary box and cone for patch   */
   DBstatus   varkon_pat_bound (
              DBSurf *p_sur, DBPatch *p_pat, 
              DBfloat u_s, DBfloat v_s, DBfloat u_e, DBfloat v_e,
              DBint acc, BBOX    *p_pbox, BCONE   *p_pcone );

/* sur913.c   varkon_sur_mboxcone   Calculate surface BBOX and BCONE  */
   DBstatus   varkon_sur_mboxcone (
              DBSurf *p_sur, DBPatch *p_pat, BBOX *p_bbox,
              BCONE *p_bcone, DBfloat *p_angmax, DBint *p_nbadb,
              DBint *p_nbadc );


/*
***Transformation functions 
*/

/* sur610.c   varkon_sur_transf     Transform a surface               */
   DBstatus   varkon_sur_transf (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );


/* sur600.c   varkon_sur_bictra     Transform bicubic surface         */
   DBstatus   varkon_sur_bictra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );

/* sur602.c   varkon_sur_rbictra    Transform rat. bicubic surface    */
   DBstatus   varkon_sur_rbictra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *pc );

/* sur604.c   varkon_sur_lofttra    Transform conic lofting surface   */
   DBstatus   varkon_sur_lofttra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );

/* sur606.c   varkon_sur_contra     Transform a Consurf surface       */
   DBstatus   varkon_sur_contra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );

/* sur608.c   varkon_sur_factra     Transform a facet surface         */
   DBstatus   varkon_sur_factra (
              GMSUR   *p_sur, GMPAT   *p_pat, DBTmat  *p_c );

/* sur609.c   varkon_sur_nurbtra    Transform a NURBS surface         */
   DBstatus   varkon_sur_nurbtra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );

/* sur520.c   varkon_sur_poltra     Transform a polynomial surface    */
   DBstatus   varkon_sur_poltra (
              DBSurf  *p_sur, DBPatch *p_pat, DBTmat  *p_c );

/* sur620.c   varkon_pat_bictra     Transform bicubic patch           */
   DBstatus   varkon_pat_bictra (
              GMPATC *ppi, DBTmat *pc, GMPATC *ppo );

/* sur622.c   varkon_pat_rbictra    Transform rational bicubic patch  */
   DBstatus   varkon_pat_rbictra (
              GMPATR *ppi, DBTmat *pc, GMPATR *ppo );

/* sur624.c   varkon_pat_lofttra    Transform a conic lofting patch   */
   DBstatus   varkon_pat_lofttra (
              GMPATL *p_ipatl, DBTmat *p_c, GMPATL *p_opatl );

/* sur626.c   varkon_pat_contra     Transform Consurf patch           */
   DBstatus   varkon_pat_contra (
              GMPATN *p_patin, DBTmat *p_c, GMPATN *p_patout );

/* sur628.c   varkon_pat_factra     Transform a facet patch           */
   DBstatus   varkon_pat_factra (
              GMPATF *p_patin, DBTmat *p_c, GMPATF *p_patout );

/* sur629.c   varkon_pat_nurbtra    Transform a NURBS patch           */
   DBstatus   varkon_pat_nurbtra (
              GMPATNU *p_patin, DBTmat  *p_c, GMPATNU *p_patout );


/* sur510.c   varkon_pat_trap3      Transformation of GMPATP3         */
   DBstatus   varkon_pat_trap3 (
              GMPATP3   *p_i, DBTmat *p_c, GMPATP3   *p_o );

/* sur511.c   varkon_pat_trap5      Transformation of GMPATP5         */
   DBstatus   varkon_pat_trap5 (
              GMPATP5   *p_i, DBTmat *p_c, GMPATP5   *p_o );

/* sur512.c   varkon_pat_trap7      Transformation of GMPATP7         */
   DBstatus   varkon_pat_trap7 (
              GMPATP7   *p_i, DBTmat  *p_c, GMPATP7   *p_o );

/* sur513.c   varkon_pat_trap9      Transformation of GMPATP9         */
   DBstatus   varkon_pat_trap9 (
              GMPATP9   *p_i, DBTmat  *p_c, GMPATP9   *p_o );

/* sur514.c   varkon_pat_trap21     Transformation of GMPATP21        */
   DBstatus   varkon_pat_trap21 (
              GMPATP21  *p_i, DBTmat  *p_c, GMPATP21  *p_o );


/* sur650.c   varkon_tensmult_l     Tensor T(4X4X4)=G(4X4)*A(4X4X4X)  */
   DBstatus   varkon_tensmult_l (
              GMPATR *ppi, DBTmat *pc, GMPATR *ppo );

/* sur651.c   varkon_tensmult_r     Tensor T(4X4X4)=A(4X4X4X)*G(4X4)  */
   DBstatus   varkon_tensmult_r (
              GMPATR *ppi, DBTmat *pc, GMPATR *ppo );

/* sur660.c   varkon_invmat         Invertation of a 4X4 matrix       */
   DBstatus   varkon_invmat ( 
              DBTmat  *pm1, DBTmat  *pm2, DBfloat *p_deter );

/* sur661.c   varkon_detmat         Determinant of a 4X4 matrix       */
   DBstatus   varkon_detmat (
              DBTmat   *pm, DBfloat  *p_deter );



/* Tolerances */

   DBfloat varkon_ntol();
   DBfloat varkon_ctol();
   DBfloat varkon_idpoint();
   DBfloat varkon_idangle();
   DBfloat varkon_comptol();
   DBfloat varkon_infinitetol();
   DBfloat varkon_geodesictol();
   DBfloat varkon_idradius();
   DBfloat varkon_idknot();
   DBfloat varkon_cgraph();
   DBfloat varkon_linecrit();

/* Initialization functions */

/* sur770.c   varkon_ini_evals      Initialize variable EVALS         */
   DBstatus   varkon_ini_evals  ( EVALS   *p_xyz );
/* sur776.c   varkon_ini_evalc      Initialize variable EVALC         */
   DBstatus   varkon_ini_evalc  ( EVALC   *p_xyz );
/* sur775.c   varkon_ini_epoint     Initialize variable EPOINT        */
   DBstatus   varkon_ini_epoint ( EPOINT  *p_ep  );
/* sur772.c   varkon_ini_bbox       Initialize variable BBOX          */
   DBstatus   varkon_ini_bbox   ( BBOX    *p_box ); 
/* sur773.c   varkon_ini_bcone      Initialize variable BCONE         */
   DBstatus   varkon_ini_bcone  ( BCONE   *p_con );
/* sur774.c   varkon_ini_pbound     Initialize variable PBOUND        */
   DBstatus   varkon_ini_pbound ( PBOUND  *p_pb  );
/* sur778.c   varkon_ini_gmcur      Initialize variable DBCurve      */
   DBstatus   varkon_ini_gmcur  ( DBCurve *p_cur );
/* sur779.c   varkon_ini_gmseg      Initialize variable DBSeg         */
   DBstatus   varkon_ini_gmseg  ( DBSeg   *p_seg );
/* sur763.c   varkon_ini_gmpatb     Initialize variable GMPATB        */
   DBstatus   varkon_ini_gmpatb ( GMPATB  *p_pat );
/* sur757.c   varkon_ini_gmpatnu    Initialize variable GMPATNU       */
   DBstatus   varkon_ini_gmpatnu ( GMPATNU  *p_pat );
/* sur758.c   varkon_ini_gmpatbr3   Initialize variable GMPATBR3      */
   DBstatus   varkon_ini_gmpatbr3 ( GMPATBR3 *p_pat );
/* sur759.c   varkon_ini_apoint     Initialize variable APOINT        */
   DBstatus   varkon_ini_apoint ( APOINT *p_pt   );
/* sur760.c   varkon_ini_aplane     Initialize variable APLANE        */
   DBstatus   varkon_ini_aplane ( APLANE *p_pl   );
/* sur761.c   varkon_ini_gmpatn     Initiate variable GMPATN          */
   DBstatus   varkon_ini_gmpatn ( GMPATN *p_pat );
/* sur762.c   varkon_ini_appcur     Initiate variable APPCUR          */
   DBstatus   varkon_ini_appcur ( APPCUR *p_app );
/* sur764.c   varkon_ini_gmlin      Initialize variable DBLine        */
   DBstatus   varkon_ini_gmlin  ( DBLine *p_lin );
/* sur765.c   varkon_ini_gmpatl     Initialize variable GMPATL        */
   DBstatus   varkon_ini_gmpatl ( GMPATL *p_pat );
/* sur766.c   varkon_ini_gmpatr     Initiate variable GMPATR          */
   DBstatus   varkon_ini_gmpatr ( GMPATR *p_pat );
/* sur767.c   varkon_ini_gmpatc     Initialize variable GMPATC        */
   DBstatus   varkon_ini_gmpatc ( GMPATC *p_pat );
/* sur768.c   varkon_ini_gmpat      Initialize variable DBPatch       */
   DBstatus   varkon_ini_gmpat  ( DBPatch *p_pat );
/* sur769.c   varkon_ini_gmsur      Initialize variable DBSurf        */
   DBstatus   varkon_ini_gmsur  ( DBSurf  *p_sur );
/* sur771.c   varkon_ini_surpoi     Initiate variable SURPOI          */
   DBstatus   varkon_ini_surpoi ( SURPOI *p_sp );
/* sur777.c   varkon_ini_spoint     Initialize variable SPOINT        */
   DBstatus   varkon_ini_spoint ( SPOINT *p_sp );


/* sur490.c   varkon_sur_warning    Surface quality warnings/errors   */
   DBstatus   varkon_sur_warning (
              DBSurf *p_sur, DBPatch *p_pat, DBint n_w,
              SURWARN  p_s_w[] );

/*
***Surface analysis functions
*/

/* sur300.c   varkon_sur_sarea      Surface area c.o.g. m.o. inertia  */
   DBstatus   varkon_sur_sarea (
              DBSurf *p_surin, DBPatch *p_patin, DBTmat *p_csys,
              DBint acc, DBfloat a_crit_in, DBint c_case,
              DBfloat *p_sarea, DBVector *p_cog, DBVector *p_moi,
              DBVector *p_axis1, DBVector *p_axis2, DBVector *p_axis3);

/* sur301.c   varkon_pat_sarea      Patch area c.o.g. m.o. inertia    */
   DBstatus   varkon_pat_sarea (
              DBSurf *p_surin, DBPatch *p_patin, DBTmat *p_csys,
              DBint c_case, DBfloat u_s, DBfloat v_s,
              DBfloat u_e, DBfloat v_e, DBfloat  *p_patarea,
              DBVector *p_patcog, DBVector *p_patmoi );

/* sur302.c   varkon_cur_secta      Section area c.o.g. m.o. inertia  */
   DBstatus   varkon_cur_secta (
              DBCurve *p_cur, DBSeg   *p_seg, DBTmat  *p_csys,
              DBfloat  delta, DBfloat  a_crit, DBint    c_case,
              DBfloat  *p_maxdev, DBfloat  *p_sarea,
              DBVector *p_cog, DBVector *p_moi,
              DBVector *p_axis1, DBVector *p_axis2, DBVector *p_axis3 );

/*
***Vector functions (TODO replace with faster functions)
*/

/*  suvecm    varkon_normv The normalisation of a vector             */
    DBstatus  varkon_normv ( 
              DBfloat v[] );
/*  suvecm The length of a vector                                    */
    DBstatus  varkon_lengv ( 
              DBfloat v[], DBfloat *pleng );
/*  suvecm Scalar product                                            */
    DBstatus  varkon_scalar ( 
              DBfloat u[], DBfloat v[], DBfloat *pdot );
/*  suvecm Vector product                                            */
    DBstatus  varkon_xprod ( 
              DBfloat u[], DBfloat v[], DBfloat cross[] );
/*  suvecm Vector product, normalised                                */
    DBstatus  varkon_nxprod ( 
              DBfloat u[], DBfloat v[], DBfloat cross[] );
/*  suvecm The triple scalar product                                 */
    DBstatus varkon_triscl( 
             DBfloat v1[], DBfloat v2[], DBfloat v3[], DBfloat *res );
/*  suvecm The angle between two vectors in radians                  */
    DBstatus  varkon_angr ( 
              DBfloat u[],DBfloat v[], DBfloat *pang );
/*  suvecm The angle between two vectors in degrees                  */
    DBstatus  varkon_angd ( 
              DBfloat u[], DBfloat v[], DBfloat *pang );


/*  GE807() Create Ferguson spline                                  */
/*  GE808() Create Chordlength spline                               */
/*  GE809() Create Variable stiffness spline                        */
        DBstatus GE807(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt);

        DBstatus GE808(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt);

        DBstatus GE809(
        DBVector *pv,
        DBVector *tv,
        DBint     npoi,
        DBCurve  *curpek,
        DBSeg    *segmnt);

DBstatus suinit();
DBstatus suexit();
