/**********************************************************************
*
*    GE.h
*    ====
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

/*
***Include files.
*/
#include "geotol.h"
#include <math.h>

/*
***Temporary decl. of erpush(). To be moved somwhere else later.
*/
extern short erpush();

/*
***Some define constants.
*/
#define  SNAME        /* Used in surxxx-routines */
#define  REDERM       /* Reduce error messages */
#define  SUCCED  0    /* Returncode for successful completion of function */
#define  RESTRT  2    /* Number of restarts for curve intersects */
#define  INTMAX  50   /* Max antal skärningar på en kurva */
#define  GMMXSG  25   /* Temporärt */

/*
***Macro's for math functions.
*/
#define SIN       sin
#define COS       cos
#define SQRT      sqrt
#define TAN       tan
#define ASIN      asin
#define ACOS      acos
#define DACOS(x)  (((x) < 0) ? 4.0*ATAN(1.0)-ACOS(-x) : ACOS(x))
#define ATAN      atan
#define ABS       fabs
#define HEL       floor
#define DEC(x)    (x-floor(x))

/*
***Macro for block memory move/copy.
*/
#define V3MOME(from,to,size) memcpy(to,from,size)

/*
***Constants for PI and it's fractions.
*/
#ifdef PI
#undef PI
#endif

#ifdef PI2
#undef PI2
#endif

#define PI05     1.5707963267948966    /* PI/2   = 90  degrees */
#define PI       3.1415926535897932    /* PI     = 180 degrees */
#define PI15     4.7123889803846896    /* 3*PI/2 = 270 degrees */
#define PI2      6.2831853071795862    /* 2*PI   = 360 degrees */
#define DGTORD   0.017453292519943294  /* From degrees to radians */
#define RDTODG  57.295779513082322     /* From radians to degrees */

/*
***Function prototypes for the public GE API.
*/

/* Basic vector math routines */

DBfloat  GEvector_length2D(DBVector *pv);
DBfloat  GEvector_length3D(DBVector *pv);
DBstatus GEnormalise_vector2D(DBVector *pvin, DBVector *pvout);
DBstatus GEnormalise_vector3D(DBVector *pvin, DBVector *pvout);
DBfloat  GEscalar_product2D(DBVector *pv1, DBVector *pv2);
DBfloat  GEscalar_product3D(DBVector *pv1, DBVector *pv2);
DBstatus GEvector_product(DBVector *pv1, DBVector *pv2, DBVector *pvout);
DBstatus GEtriscl_product(DBVector *pv1, DBVector *pv2, DBVector *pv3, DBfloat *pout);
DBstatus GEvector_angler2D(DBVector *pv1, DBVector *pv2, DBfloat *pout);
DBstatus GEvector_angler3D(DBVector *pv1, DBVector *pv2, DBfloat *pout);
DBstatus GEvector_angled2D(DBVector *pv1, DBVector *pv2, DBfloat *pout);
DBstatus GEvector_angled3D(DBVector *pv1, DBVector *pv2, DBfloat *pout);
DBstatus GExy_angled(DBfloat x, DBfloat y, DBfloat *angle);

/* Transformations */

DBstatus GEtform_mult(DBTmat *pt1, DBTmat *pt2, DBTmat *pt3);
DBstatus GEtform_det(DBTmat *pt1, DBfloat *det);
DBstatus GEtform_inv(DBTmat *pt1, DBTmat *pt2);
DBstatus GEtfpos_to_local(DBVector *pin, DBTmat *pt, DBVector *pout);
DBstatus GEtfpos_to_basic(DBVector *pin, DBTmat *pt, DBVector *pout);
DBstatus GEtfvec_to_local(DBVector *pin, DBTmat *pt, DBVector *pout);
DBstatus GEtfvec_to_basic(DBVector *pin, DBTmat *pt, DBVector *pout);
DBstatus GEtfseg_to_local(DBSeg *pin, DBTmat *pt, DBSeg *pout);
DBstatus GEtfseg_to_basic(DBSeg *pin, DBTmat *pt, DBSeg *pout);
DBstatus GEtfang_to_basic(DBfloat vin, DBTmat *pt, DBfloat *pvout);

DBstatus GEtfPoint_to_local(DBPoint *opoi, DBTmat *pt, DBPoint *npoi);
DBstatus GEtfLine_to_local(DBLine *pin, DBTmat *pt, DBLine *pout);
DBstatus GEtfLine_to_basic(DBLine *pin, DBTmat *pt, DBLine *pout);
DBstatus GEtfArc_to_local(DBArc *oarc, DBSeg oseg[], DBTmat *pt,
                          DBArc *narc, DBSeg nseg[]);
DBstatus GEtfCurve_to_local(DBCurve *ocur, DBSeg osge[], DBSeg osgr[],
                            DBTmat *pt, DBCurve *ncur,
                            DBSeg nsge[], DBSeg nsgr[]);
DBstatus GEtfSurf_to_local(DBSurf *sur, DBPatch *patpek, DBTmat *ptr);
DBstatus GEtfBplane_to_local(DBBplane *obpl, DBTmat *pt, DBBplane *nbpl);
DBstatus GEtfMesh_to_local(DBMesh *mesh, DBTmat *pt);
DBstatus GEtfCsys_to_local(DBCsys *ocsy, DBTmat *opmat, DBTmat *pt,
                           DBCsys *ncsy, DBTmat *npmat);
DBstatus GEtfText_to_local(DBText *otxt, DBTmat *pt, DBText *ntxt);
DBstatus GEtfLdim_to_local(DBLdim *oldm, DBTmat *pt, DBLdim *nldm);
DBstatus GEtfCdim_to_local(DBCdim *ocdm, DBTmat *pt, DBCdim *ncdm);
DBstatus GEtfRdim_to_local(DBRdim *ordm, DBTmat *pt, DBRdim *nrdm);
DBstatus GEtfAdim_to_local(DBAdim *oadm, DBTmat *pt, DBAdim *nadm);
DBstatus GEtfHatch_to_local(DBHatch *xhtp, DBfloat crdvek[], DBTmat *pt);

DBstatus GEmktf_3p(DBVector *por, DBVector *pv1, DBVector *pv2, DBTmat *pc);
DBstatus GEmktf_2p(DBVector *p1, DBVector *p2, DBVector *p3, DBTmat *csymat);
DBstatus GEmktf_1p(DBVector *p, DBfloat v1, DBfloat v2, DBfloat v3,
                   DBTmat *pc, DBTmat *pmat);
DBstatus GEmktf_ts(DBVector *p1, DBVector *p2, DBfloat sx, DBfloat sy,
                   DBfloat sz, DBTmat *ptm);
DBstatus GEmktf_rs(DBVector *p1, DBVector *p2, DBfloat v, DBfloat sx,
                   DBfloat sy, DBfloat sz, DBTmat *ptm);
DBstatus GEmktf_mirr(DBVector *p1, DBVector *p2, DBVector *p3, DBTmat *ptm);
DBstatus GEmktf_6p(DBVector *p1, DBVector *p2, DBVector *p3, DBVector *p4,
                   DBVector *p5, DBVector *p6, DBTmat *ptm);

/* Generic entity evaluation */
DBstatus GEposition(DBAny *gmstrp, char *datpek, DBfloat u,
                    DBfloat v, DBVector *vecptr);
DBstatus GEtangent(DBAny *gmstrp, DBSeg *segmnt, DBfloat t,
                   DBTmat *crdptr, DBVector *vecptr);
DBstatus GEcentre(DBAny *gmstrp, DBSeg *segmnt, DBfloat t, DBVector *vecptr);
DBstatus GEarclength(DBAny *gmstrp, DBSeg *segmnt, DBfloat *length);
DBstatus GEcurvature(DBAny *gmstrp, DBSeg *segmnt, DBfloat t, DBfloat *pkappa);
DBstatus GEnormal(DBAny *gmstrp, char *datpek, DBfloat u,
                  DBfloat v, DBVector *vecptr);

/* NURBS curves */
DBstatus GEevalnc(DBCurve *curptr,DBSeg *segptr,EVALC *evlptr);

/* ge100-routines */

DBstatus GE101(DBAny *pstr, DBSeg *pseg, DBfloat plane[], short nstart,
               short *pnoint, DBfloat uout[]);
DBstatus GE102(DBAny *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBTmat *pc, short nstart, short *pnoint,
               DBfloat uout1[], DBfloat uout2[]);
DBstatus GE107(DBAny *gmpost, DBSeg *sp, DBfloat u,
               short rcode, DBfloat out[]);
DBstatus GE109(DBAny *gmpost, DBSeg *segpek, EVALC *evlpek);
DBstatus GE110(DBAny *gmpost, DBSeg *segpek, EVALC *evlpek);
DBstatus GE120(DBAny *pstr, DBSeg *pseg, DBfloat interv[], DBfloat *pdelta);
DBstatus GE133(DBVector *points, DBfloat p, DBSeg *pr);
DBstatus GE135(DBSeg *prin, DBfloat uextr[], DBSeg *prout);
DBstatus GE136(DBVector *points, DBVector *pipt, DBSeg *pr);
DBstatus GE140(DBAny *pstr, DBSeg *pseg, DBVector *ptang, short nstart,
               short *pnoint, DBfloat uout[]);
DBstatus GE141(DBSeg *pr, DBfloat conp[]);
DBstatus GE142(DBSeg *pr, DBVector *ppt3, DBTmat *pc);
DBstatus GE143(DBAny *pstr,  DBSeg *pseg,  DBVector *pexpt, short nstart,
               short *pnoint, DBfloat uout[]);
DBstatus GE144(DBAny *pstr, DBSeg *pseg, DBfloat *pkappa, short nstart,
               short *pnoint, DBfloat uout[]);

/* Line routines */

DBstatus GE200(DBVector *p1, DBVector *p2, DBLine *lin);
DBstatus GE201(DBVector *p1, DBVector *p2, DBTmat *pc, DBLine *lin);
DBstatus GE202(DBLine *lin, DBfloat offset, DBTmat *pc, DBLine *lout);
DBstatus GE203(DBVector *p, DBLine *lin, DBfloat len, DBTmat *pc, DBLine *lout);
DBstatus GE204(DBVector *p, DBTmat *pc, DBfloat ang, DBfloat len, DBLine *lout);
DBstatus GE205(DBAny *str, DBSeg *seg, DBVector *p,
               DBTmat *pc, DBshort alt, DBLine *lout);
DBstatus GE206(DBArc *arc1, DBSeg *seg1, DBTmat *pc1, DBArc *arc2,
               DBSeg *seg2, DBTmat *pc2, DBshort alt, DBTmat *pc, DBLine *lout);DBstatus GE207(DBArc *arcp, DBVector *pos, DBTmat *pc, DBshort nalt,
               DBfloat *puout, DBVector *ptanp);
DBstatus GE208(DBfloat c1x, DBfloat c1y, DBfloat r1, DBfloat v11, DBfloat v12,
               DBfloat c2x, DBfloat c2y, DBfloat r2, DBfloat v21, DBfloat v22,
               DBshort alt, DBVector *p1, DBVector *p2, DBint *nsol);

/* Arc/Circle routines */

DBstatus GE300(DBVector *por, DBfloat rad, DBfloat ang1, DBfloat ang2,
               DBTmat *pt, DBArc *arcpek, DBSeg *basseg, short alt);
DBstatus GE301(DBVector *ppos, DBfloat rad, DBfloat ang1, DBfloat ang2,
               DBVector *segdat, DBfloat *ppval);
DBstatus GE302(DBVector *pcen, DBVector *ppoi, DBVector *ptan);
DBstatus GE303(DBVector *pa, DBVector *pc, DBVector *pta,
               DBVector *ptc, DBVector *pb);
DBstatus GE304(DBVector *pa, DBVector *pb, DBVector *pc,
               DBVector *pd, DBfloat *ppval);
DBstatus GE306(DBArc *polarc, DBSeg *polseg, DBfloat dist, DBTmat *pc,
               DBArc *pa, DBSeg *paseg);
DBstatus GE307(DBAny  *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBfloat radius, short nalt, DBTmat *pc, DBArc *pa,
               DBSeg *paseg, short modtyp);
DBstatus GE308(DBLine *plin1, DBLine *plin2, DBfloat radius, short nalt,
               DBTmat *pc, DBArc *pa, DBSeg *paseg, short modtyp);
DBstatus GE309(DBAny  *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBfloat radius, short nalt, DBTmat *pc, DBArc *pa,
               DBSeg *paseg, short modtyp);
DBstatus GE310(DBVector *ppoi1, DBVector *ppoi2, DBVector *ppoi3,
               DBTmat *pc, DBArc *arcpek, DBSeg *segmnt, short alt);
DBstatus GE311(DBArc *parc, DBSeg *pseg, DBfloat uglob, DBfloat *ps);
DBstatus GE312(DBfloat *v1, DBfloat *v2);
DBstatus GE315(DBArc *arcp, DBfloat dx, DBfloat dy, DBfloat *u);
DBstatus GE320(DBVector *ppoi1, DBVector *ppoi2, DBfloat rad, DBTmat *pc,
               DBArc *arcpek, DBSeg *segmnt, short alt);
DBstatus GE350(DBVector *por, DBfloat radius, DBTmat *pc,
               DBArc *pa, DBSeg *paseg);
DBstatus GE351(DBfloat radius, DBSeg *pr);
DBstatus GE352(DBAny  *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBfloat radius, short nalt, DBTmat *pc, DBArc *pa,
               DBSeg *paseg, short modtyp);
DBstatus GE353(DBVector *porig, DBVector *pstart, DBVector *pend,
               DBTmat *pc, DBArc *pa, DBSeg *paseg, short modtyp);
DBstatus GE354(DBfloat radius, short nalt, DBfloat *poffs1, DBfloat *poffs2);

/* 400-routines */

DBstatus GE400(DBVector *ppin, DBPoint *ppout);
DBstatus GE401(DBVector *ppin, DBTmat *pc, DBPoint *ppout);
DBstatus GE402(DBAny *pstr, DBSeg *pseg, DBVector *pexpt, DBTmat *pc,
               short nalter, DBfloat *puout, DBVector *ptanpt);
DBstatus GE403(DBAny *pstr, DBSeg *pseg, DBVector *pexpt, DBTmat *pc,
                short nstart, short *pnoint, DBfloat uout[]);

/* 700-routines */

DBstatus GEintersect_pos(DBAny *pstr1, char *pdat1, DBAny *pstr2, char *pdat2,
                          DBTmat *pc, short intnr, DBVector *pvec);
DBstatus GEintersect_pv(DBAny *pstr1, char *pdat1, DBAny *pstr2, char *pdat2,
                        DBTmat *pc, short intnr, DBfloat *pu1, DBfloat *pu2);
DBstatus GEintersect_npos(DBAny *pstr1, char *pdat1, DBAny *pstr2, char *pdat2,
                          DBTmat *pc, short extend, short *numint);
DBstatus GE702(DBAny *pstr, DBSeg *pseg, DBVector *ptang, short intnr,
               DBfloat *pu);
DBstatus GE703(DBAny *pstr, DBSeg *pseg, DBVector *pexpt, short intnr,
               DBfloat *pu);
DBstatus GE704(DBAny *pstr, DBSeg *pseg, DBfloat *pkappa, short intnr,
               DBfloat *pu);
DBstatus GE705(DBAny *pstr1, DBAny *pstr2, DBTmat *pc, short *pnoint,
               DBfloat u1[], DBfloat u2[]);
DBstatus GE706(DBAny *pstr1, DBAny *pstr2, short *noint,
               DBfloat u1[], DBfloat u2[]);
DBstatus GE707(DBArc *arcp1, DBArc *arcp2, short *noint,
               DBfloat u1[], DBfloat u2[]);
DBstatus GE708(DBAny *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBTmat *pc, short *pnoint, DBfloat uout1[], DBfloat uout2[]);
DBstatus GE709(DBLine *linpek, DBVector *pos, DBfloat *tptr);
DBstatus GE710(DBAny *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBTmat *pc, short *pnoint, DBfloat uout1[], DBfloat uout2[]);
DBstatus GE711(DBLine *plin, DBAny *pstr2, DBSeg *pseg2, DBTmat *pc,
               short  noin, DBfloat uinc[], short *pnoout,
               DBfloat uoutc[], DBfloat soutl[]);
DBstatus GE712(DBAny *pstr, DBSeg *pseg, DBVector *ptang,
               short *pnoint, DBfloat uout[]);
DBstatus GE713(DBAny *pstr, DBSeg *pseg, DBVector *pexpt,
               short *pnoint, DBfloat uout[]);
DBstatus GE714(DBAny *pstr, DBSeg *pseg, DBfloat *pkappa,
               short *pnoint, DBfloat uout[]);
DBstatus GE715(DBAny *curve, DBSeg *seg, DBAny *plan, short *pnoint,
               DBfloat uout[]);
DBstatus GE716(DBLine *lin, DBfloat plane[], DBfloat uout[]);
DBstatus GE717(DBAny *pstr, DBSeg *pseg, DBTmat *pc, DBfloat rel_leng,
               DBfloat *pu);
DBstatus GE718(DBAny *pstr, DBSeg *pseg, DBTmat *pc, DBfloat uglobs,
               DBfloat uglobe, DBfloat *prel_leng);
DBstatus GE720(DBAny *pstr1, DBSeg *pseg1, DBAny *pstr2, DBSeg *pseg2,
               DBTmat *pc, short *pnoint, DBfloat uout1[], DBfloat uout2[]);
DBstatus GE721(DBArc *arcpek, DBVector *pos, DBfloat *tptr);
DBstatus GE723(DBLine *lin, DBSurf *sur, DBPatch *pat, short *pnoint,
               DBfloat uout1[], DBfloat uout2[]);
DBstatus GE724(DBCurve *cur, DBSeg *seg, DBSurf *sur, DBPatch *pat,
               short *pnoint, DBfloat uout1[], DBfloat uout2[]);
DBstatus GE730(DBLine *plin1, DBfloat offs1, DBLine *plin2, DBfloat offs2,
               DBTmat *pc, DBLine *ploff1, DBLine *ploff2, DBfloat *pu1,
               DBfloat *pu2, DBVector *ppout);
DBstatus GE732(DBAny *pstr1, DBSeg *pseg1, DBfloat offs1,
               DBAny *pstr2, DBSeg *pseg2, DBfloat offs2,
               DBTmat *pc, DBfloat *puc1, DBfloat *puc2,
               DBVector *ppout, DBVector *ppcur1, DBVector *ppcur2);

/* 800-routines */

DBstatus GE810(DBVector *pv, DBVector *tv, short npoi,
               DBCurve *curpek, DBSeg *segmnt);
DBstatus GE811(DBVector *pv, DBVector *tv, short npoi, DBTmat *crdpek,
               DBCurve *curpek, DBSeg *segmnt);
DBstatus GE813(DBCurve *ocur, DBSeg *oldgra, DBSeg *oldgeo, DBTmat *pc,
               DBfloat offset, DBCurve *ncur, DBSeg *newgra, DBSeg *newgeo);
DBstatus GE814(DBCurve *cur, DBTmat *pc);
DBstatus GE815(DBVector *pv, DBVector *tv, DBVector *iv, DBfloat pvalue[],
               short npoi, DBCurve *curpek, DBSeg *segmnt);
DBstatus GE816(DBVector *pv, DBVector *tv, DBVector *iv, DBfloat pvalue[],
               short npoi, DBTmat *pcrd, DBCurve *curpek, DBSeg *segmnt);
DBstatus GE817(DBAny *pstr, DBSeg *pseg, DBCurve *pcur_out, DBSeg *pseg_out,
               DBfloat glob_s, DBfloat glob_e);
DBstatus GE821(DBArc *parc, DBVector *pos, short alt, DBCdim *cdmpek);
DBstatus GE822(DBArc *parc, DBVector *p1, DBVector *p2, DBRdim *rdmpek);
DBstatus GE823(DBLine *l1, DBLine *l2, DBVector *p, short alt, DBAdim *admpek);

/* Hatching/area/cgrav. */

DBstatus GEhatch2D(DBLine *lpvek[], short nlin, DBArc *apvek[], short narc,
                   DBCurve *cpvek[], DBSeg *spvek[], short ncur,
                   DBHatch *xhtpek, DBfloat crdvek[]);
DBstatus GEarea2D(DBLine *lpvek[], short nlin, DBArc *apvek[], short narc,
                  DBCurve *cpvek[], DBSeg *spvek[], short ncur, DBfloat dist,
                  DBfloat *area, DBVector *tp);
DBstatus GEint_2Dlb(DBLine *lpvek[], short nlin, DBArc *apvek[], short narc,
                    DBCurve *cpvek[], DBSeg *spvek[], short ncur, DBfloat px,
                    DBfloat py, DBfloat k, DBfloat t[], short *ts);

/* Sorting routines. */
DBstatus GEsort_1(DBfloat *uin, short nin, short *nut, DBfloat *uut);
DBstatus GEsort_2(DBfloat uin1[], DBfloat uin2[], short noin,
                  short *pnout, DBfloat uout1[], DBfloat uout2[]);
DBstatus GEsort_mesh(DBMesh *mesh, char mode, DBint *facelist, DBfloat *min, DBfloat *max);

/* Tolerances */

DBfloat sur751();
DBfloat sur753();

/**********************************************************************/


/* Gunnar added lines */

extern short erinit();
extern short errmes();
extern void *v3mall();
extern short v3free();

#include "surcre.h"
#include "surinc.h"
