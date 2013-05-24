/**********************************************************************
*
*    EX.h
*    ====
*
*    This file is part of the VARKON EXecute Library.
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
***Function prototypes for the public EX API.
*/

/*
***exdxfin.c
*/
int EXimport_dxf(char *file,char *name, double x, double y, double angle,
                 double scale, char *logfile);

/*
***exdxfout.c
*/
int EXall_to_dxf(char *file, double x, double y);

/*
***ex1.c
*/
short EXstrt(DBId *idpek, DBVector *vecptr);
short EXend(DBId *idpek, DBVector *vecptr);
short EXon(DBId *idpek, gmflt u, gmflt v, DBVector *vecptr);
short EXion(DBId *idpek, DBVector *vecptr, DBshort tnr, gmflt *tptr);
short EXtang(DBId *idpek, gmflt t, DBTmat *crdptr, DBVector *vecptr);
short EXitan(DBId *idpek, DBVector *vecptr, DBshort tnr, gmflt *tptr);
short EXcurv(DBId *idpek, gmflt t, gmflt *fltptr);
short EXicur(DBId *idpek, gmflt kappa, DBshort tnr, gmflt *tptr);
short EXcen(DBId *idpek, gmflt t, DBTmat *crdptr, DBVector *vecptr);
short EXnorm(DBId *idpek, gmflt u, gmflt v, DBVector *vecptr);
short EXarcl(DBId *idpek, gmflt *length);
short EXiarc(DBId *idpek, gmflt l, gmflt *tptr);
short EXsuar(DBId *idpek, gmflt tol, gmflt *p_area);
short EXsear(DBId *idpek, gmflt tol, gmflt *p_area);
short EXsecg(DBId *idpek, gmflt tol, DBVector *p_cgrav);
short EXtxtl(char *str, gmflt *l);
short EXsect(DBId *idp1, DBId *idp2, DBint inr, DBint alt, DBVector *vecptr);
short EXnsec(DBId *idp1, DBId *idp2, DBshort inr,
             DBTmat *crdptr, DBshort *numint);
short EXidnt(DBetype *typmsk, DBId ident[], bool  *end, bool  *right);
short EXpos(double *px, double *py, char *pc);
short EXscr(DBshort *pix, DBshort *piy, DBint *win_id);
short EXos(char oscmd[], DBshort mode);
short EXarea(DBId *ridvek,DBshort nref, gmflt dist, gmflt *area, DBVector *tp);
short EXusec(DBId *idp1, DBId *idp2, DBshort inr,
             DBTmat *crdptr, DBVector *vecptr);

/*
***ex2.c
*/
short EXecsy(DBId *id, GMCSY *crdpek, DBTmat *pmat, V2NAPA *pnp);
short EXcs3p(DBId *id, char *str, DBVector *p1, DBVector *p2,
             DBVector *p3, V2NAPA *pnp);
short EXcs1p(DBId *id, char *str, DBVector *p, gmflt v1,
             gmflt v2, gmflt v3, V2NAPA *pnp);
short EXetxt(DBId *id, GMTXT *txtpek, char strpek[], V2NAPA *pnp);
short EXtext(DBId *id, DBVector *pos, gmflt vridn, char *strpek, V2NAPA *pnp);
short EXgrp(DBId *id, DBId *ridvek, GMGRP *grppek);

/*
***expoint.c
*/
short EXepoi(DBId *id, GMPOI *poipek, V2NAPA *pnp);
short EXpofr(DBId *id, DBVector *pv, V2NAPA *pnp);
short EXpopr(DBId *id, DBVector *pv, V2NAPA *pnp);

/*
***ex4.c
*/
short EXelin(DBId *id, GMLIN *linpek, V2NAPA *pnp);
short EXlifr(DBId *id, DBVector *p1, DBVector *p2, V2NAPA *pnp);
short EXlipr(DBId *id, DBVector *p1, DBVector *p2, V2NAPA *pnp);
short EXliof(DBId *id, DBId *refid, gmflt offset, V2NAPA *pnp);
short EXlipv(DBId *id, DBVector *pos, gmflt ang, gmflt length, V2NAPA *pnp);
short EXlipt(DBId *id, DBVector *pos, DBId *refid, DBshort alt, V2NAPA *pnp);
short EXli2t(DBId *id, DBId *refid1, DBId *refid2, DBshort alt, V2NAPA *pnp);
short EXlipe(DBId *id, DBVector *pos, DBId *refid, gmflt length, V2NAPA *pnp);

/*
***ex5.c
*/
short EXearc(DBId *id, GMARC *arcpek, GMSEG segmnt[], V2NAPA *pnp);
short EXarfr(DBId *id, DBVector *pos, gmflt r, gmflt v1, gmflt v2, V2NAPA *pnp);
short EXar2p(DBId *id, DBVector *p1, DBVector *p2, gmflt r, V2NAPA *pnp);
short EXar3p(DBId *id, DBVector *p1, DBVector *p2, DBVector *p3, V2NAPA *pnp);
short EXarof(DBId *id, DBId *refid, gmflt dist, V2NAPA *pnp);
short EXarfl(DBId *id, DBId *refid1, DBId *refid2, gmflt r,
             DBshort alt, V2NAPA *pnp);

/*
***ex6.c
*/
short EXeldm(DBId *id, GMLDM *ldmpek, V2NAPA *pnp);
short EXldim(DBId *id, DBVector *p1, DBVector *p2, DBVector *p3,
             DBshort alt, V2NAPA *pnp);
short EXecdm(DBId *id, GMCDM *cdmpek, V2NAPA *pnp);
short EXcdim(DBId *id, DBId *refid, DBVector *pos, DBshort alt, V2NAPA *pnp);
short EXerdm(DBId *id, GMRDM *rdmpek, V2NAPA *pnp);
short EXrdim(DBId *id, DBId *refid, DBVector *p1, DBVector *p2, V2NAPA *pnp);
short EXeadm(DBId *id, GMADM *admpek, V2NAPA *pnp);
short EXadim(DBId *id, DBId *refid1, DBId *refid2, DBVector *pos,
             DBshort alt, V2NAPA *pnp);
short EXexht(DBId *id, GMXHT *xhtpek, gmflt crdvek[], V2NAPA *pnp);
short EXxht(DBId *id, DBId *ridvek, DBshort nref, gmflt dist,
            gmflt ang, V2NAPA *pnp);

/*
***ex7.c
*/
short EXsymb(DBId *id, char *nam, DBVector *pos, gmflt size,
             gmflt ang, V2NAPA *pnp);
short EXebpl(DBId *id, GMBPL *bplpek, V2NAPA *pnp);
short EXbpln(DBId *id, DBVector *p1, DBVector *p2, DBVector *p3,
             DBVector *p4, V2NAPA *pnp);

/*
***ex8.c
*/
short EXtrim(DBId *idp1, DBId *idp2, DBshort end, DBshort inr);
short EXblk(DBId *idvek);
short EXblla(DBptr la, DBetype typ);
short EXublk(DBId *idvek);
short EXdraw(DBId *idvek, bool draw);
short EXdren(DBptr la, DBetype typ, bool draw, DBint win_id);
short EXdral(DBint win_id);

/*
***ex9.c
*/
short EXdel(DBId *idpek);
short EXdlla(DBptr la, DBetype typ);
short EXclgm();
short EXldgm(char *filnam);
short EXsvgm(char *filnam);
short EXsvjb(char *filnam);
short EXldjb(char *filnam, DBshort kod);
short EXbllv(DBint levnum, DBint win_id);
short EXublv(DBint levnum,DBint win_id);
short EXgtlv(DBshort levnum, bool *blank, char *name);
short EXcrvp(char name[], VYVEC *bpos);
short EXcrvc(char name[], DBId *idvek);
short EXacvi(char name[], DBint win_id);
short EXscvi(char name[], double scl);
short EXcevi(char name[], double x, double y);
short EXhdvi(char name[], bool flag1, bool flag2, FILE *pfil, DBVector *origo);
short EXprvi(char name[], double dist);
short EXervi(DBint win_id);
short EXrpvi(DBint autoz, DBint win_id);
short EXgvnl(char *namptr[], DBint *numptr);
short EXcavi(double newcn);
short EXplvi(char *vynam, char *filnam);
short EXplwi(DBVector *p1, DBVector *p2, char *filnam, DBVector *p0);
short vyindx(char vynamn[]);

/*
***ex10.c
*/
short EXgtid(DBId *id, char *code);
short EXgthd(DBId *id, GMRECH *header);
short EXgwdt(DBId *id, gmflt *width);
short EXpdat(char *key, DBint typ, DBint size, DBint count, char *datpek);
short EXgdat(char *key, DBint *typ, DBint *size, DBint *count, char **datpek);
short EXddat(char *key);
short EXgtpo(DBId *id, GMPOI *punkt);
short EXgtli(DBId *id, GMLIN *linje);
short EXgtar(DBId *id, GMARC *cirkel, GMSEG segment[]);
short EXgtcu(DBId *id, GMCUR *kurva, GMSEG **graseg, GMSEG **geoseg);
short EXgtop(DBId *id, DBshort iu, DBshort iv, GMPAT *toppat);
short EXgcub(DBId *id, DBshort iu, DBshort iv, GMPATC *cubpat);
short EXgfac(DBId *id, DBshort iu, DBshort iv, GMPATF *facpat);
short EXgttx(DBId *id, GMTXT *text, char str[]);
short EXgtxh(DBId *id, GMXHT *snitt, gmflt lindat[]);
short EXgtld(DBId *id, GMLDM *ldim);
short EXgtcd(DBId *id, GMCDM *cdim);
short EXgtrd(DBId *id, GMRDM *rdim);
short EXgtad(DBId *id, GMADM *adim);
short EXgtgp(DBId *id, GMGRP *grupp, DBptr lavek[]);
short EXgtcs(DBId *id, GMCSY *csys);
short EXgttf(DBId *id, GMTRF *ptrf);
short EXgtbp(DBId *id, GMBPL *bplan);

/*
***ex11.c
*/
short EXgtpt(DBId *id, GMPRT *part);
short EXgtyp(DBId *id, DBshort pnum, V2LITVA *typ);
short EXgint(DBId *id, DBshort pnum, V2LITVA *intval);
short EXgflt(DBId *id, DBshort pnum, V2LITVA *fltval);
short EXgstr(DBId *id, DBshort pnum, V2LITVA *strval);
short EXgvec(DBId *id, DBshort pnum, V2LITVA *vecval);
short EXgref(DBId *id, DBshort pnum, V2LITVA *refval);
short EXtime(DBshort *y, DBshort *mo, DBshort *d, DBshort *h,
             DBshort *mi, DBshort *s);
short EXuphd(DBId *id, DBshort blank, DBshort level, DBshort pen);
short EXinpt(char *pmt, char *dstr, DBshort ntkn, char *istr);
short EXinfn(char *pmt, char *dstr, char *path, char *pattern, char *name);
short EXdirl(char *inpath, char *pattern, DBint maxant, DBint maxsiz,
             char *pekarr[], char *strarr, DBint *nf);

/*
***ex12.c
*/
short EXrstr(DBId *id, DBshort form, char *idstr);
short EXrval(char  *idstr, DBId *id);
short EXlsin(char *rubr);
short EXlsex();
short EXlsln(char *utstr);
short EXglor(DBId *inref, DBshort lev, DBId *utref);
short EXfins(char str1[], char str2[], DBshort *strpos);

/*
***ex13.c
*/
short EXpspt(char *pstr);
short EXpopt();
short EXcrts(DBshort tsnr, char *tstr);
short EXgtts(DBshort tsnr, V2LITVA *tstr);
short EXcrmy(PMPARVA *pv, DBshort npar);
short EXpsmy(DBshort mnr);
short EXpomy();
short EXgtal(char *prstr, V2LITVA *akod);
short EXgtmy(DBshort mnr, char *rubr);

/*
***ex14.c
*/
short EXdbop(char *namn, DBint *status);
short EXdbbg(char *mode, DBint *status);
short EXdbsl(char *tab, char *kol1, V2LITVA *val1, char *kol2,
             V2LITVA *val2, PMPARVA *pv, DBshort nkol, DBint *status);
short EXdbfi(DBshort pnum, DBint *ival, DBint *status);
short EXdbff(DBshort pnum, double *fval, DBint *status);
short EXdbfs(DBshort pnum, char *sval, DBint *status);
short EXdbnx(DBint *status);
short EXdbup(char *tab, char *kol1, V2LITVA *val1, char *kol2,
             V2LITVA *val2, PMPARVA *pv, DBshort nkol, DBint *status);
short EXdbin(char *tab, PMPARVA *pv, DBshort nkol, DBint *status);
short EXdbdl(char *tab, char *kol1, V2LITVA *val1, char *kol2,
             V2LITVA *val2, DBint *status);
short EXdben(DBint *status);
short EXdbrl(DBint *status);
short EXdbcl(DBint *status);

/*
***ex15.c
*/
short EXmove(DBId idmat[][MXINIV], DBshort idant, DBVector *p1, DBVector *p2);
short EXmvla(DBptr la, DBetype typ, DBTmat *ptr);
short EXcopy(DBId idmat[][MXINIV], DBshort idant, DBVector *p1, DBVector *p2,
             DBshort antal, bool orgniv);
short EXcpla(DBId *id, DBptr la, DBetype typ, DBTmat *ptr, bool orgniv);
short EXmirr(DBId idmat[][MXINIV], DBshort idant, DBVector *p1,
             DBVector *p2, bool orgniv);
short EXmrla(DBId *id, DBptr la, DBetype typ, DBVector *p1,
             DBVector *p2, bool orgniv);
short EXrot(DBId idmat[][MXINIV], DBshort idant, DBVector *p, gmflt v);
short EXrola(DBptr la, DBetype typ, DBTmat *ptr);

/*
***ex16.c
*/
short EXgatt(DBId *id, V2NAPA *pnp, DBptr *pcsypk);
short EXgala(DBptr la, DBetype typ, V2NAPA *pnp, DBptr *pcsypk);
short EXrist(DBId *id);

/*
***ex17.c
*/
short EXesur(DBId *id, GMSUR *surpek, GMPAT *patpek, V2NAPA *pnp);
short EXssar(DBId *id, char *metod, DBint nu, DBint nv, DBVector *p,
             DBVector *u_tan, DBVector *v_tan, DBVector *twist, V2NAPA *pnp);
short EXscar(DBId *id, char *param_case, DBId *spine, DBint ncur, DBId *lim,
             DBId *tan, DBint *metod, DBId *mid, V2NAPA *pnp);
short EXsnar(DBId *id, DBint order_u, DBint order_v, DBint nk_u, DBint nk_v,
             DBVector *cpts, gmflt *uknots, gmflt *vknots, gmflt *w,
             V2NAPA *pnp);

/*
***ex18.c
*/
short EXetrf(DBId *id, GMTRF *trfpek, V2NAPA *pnp);
short EXtrmv(DBId *id, DBVector *p1, DBVector *p2, gmflt sx, gmflt sy,
             gmflt sz, V2NAPA *pnp);
short EXtrro(DBId *id, DBVector *p1, DBVector *p2, gmflt v, gmflt sx,
             gmflt sy, gmflt sz, V2NAPA *pnp);
short EXtrmi(DBId *id, DBVector *p1, DBVector *p2, DBVector *p3, V2NAPA *pnp);
short EXtrco(DBId *id, DBVector *p1, DBVector *p2, DBVector *p3, DBVector *p4,
             DBVector *p5, DBVector *p6, V2NAPA *pnp);
short EXtrus(DBId *id, DBTmat *ptr, V2NAPA *pnp);
short EXtcop(DBId *id, DBId *ref, DBId *trid, DBshort mode, V2NAPA *pnp);
short EXtcla(DBId *id, DBptr la, DBetype typ, DBTmat *ptr,
             DBshort mode, V2NAPA *pnp);

/*
***ex19.c
*/
short EXions(DBId *rid, DBVector *r3pos, DBint kod, DBint sol_no,
             DBVector *uv_start, DBVector *ut_pos);
short EXeval(DBId *rid, char *fall, DBVector *in_val, DBVector *ut_val);
short EXncui(DBId *rid1, DBId *rid2, DBint *pnsect);
short EXails(DBId *rid, DBVector *punkt, DBVector *riktn, DBint sort,
             DBVector *start, DBint *nint, DBVector uv[], DBVector xyz[]);
short EXailm(V2LITVA ref[], DBint nref, DBVector *punkt, DBVector *riktn,
             DBint sort, DBint *nint, DBVector uv[], DBVector xyz [],
             DBint **snum);
short EXaios(DBId *rid, DBVector *punkt, DBint sort, DBint *nsol,
             DBVector uv[], DBVector xyz[]);

/*
***ex20.c
*/
short EXsusr(V2REFVA *sur_id, V2REFVA *geo_id, char *hea_name,
             char *top_name, char *cub_name, V2NAPA *pnp);
short EXsusrh(V2REFVA *geo_id, char hea_name[20], DBint *p_n_u,
              DBint *p_n_v, DBint *p_sur_type );
short EXsusrt(GMPAT *p_pat, V2REFVA *geo_id, char top_name[20],
              DBint n_u, DBint n_v, DBint sur_type);
short EXsusrg(GMPAT *p_pat, V2REFVA *geo_id, char cub_name[20],
              DBint n_u, DBint n_v, DBint sur_type);

/*
***ex21.c
*/
short EXsexd(DBId *id, char *str1, char *str2, char *str3, gmflt trim[4],
             DBint new, V2NAPA *pnp);
short EXsexdv(DBId *sur_id, char *str1, char *str2, char *str3, gmflt trim[4],
              DBint sur_no_in, V2NAPA *pnp);

/*
***ex22.c
*/
short EXcsil(V2REFVA *id, V2REFVA *rid, DBVector *pos, DBshort fall,
             DBshort gren, V2NAPA *pnp);
short EXcint(V2REFVA *id, V2REFVA *yta, V2REFVA *plan,DBshort en, V2NAPA *pnp);
short EXciso(V2REFVA *id, V2REFVA *rid, char *fall, gmflt value,
             gmflt start, gmflt slut, V2NAPA *pnp);
short EXcfan(V2REFVA *id, V2REFVA *rid, DBVector *rikt_1, DBVector *rikt_2,
             DBint sstart, DBint sslut, gmflt ustart, gmflt uslut, V2NAPA *pnp);
short EXcged(V2REFVA *id, V2REFVA *rid, char *metod, DBVector *p1,
             DBVector *p2, V2NAPA *pnp);
short EXcapp(V2REFVA *id, V2REFVA *rid, char *segtyp, DBint metod,
             gmflt tol1, gmflt tol2, V2NAPA *pnp);

/*
***ex23.c
*/
short EXsswp(DBId *sur_id, DBId *cur_id, DBId *spine_id, DBint reverse,
             DBVector *p_yaxis, V2NAPA *pnp);
short EXscyl(DBId *sur_id, DBId *cur_id, gmflt dist, DBint reverse,
             DBVector *p_dir, V2NAPA *pnp);
short EXsrot(DBId *id, DBId *rid, DBVector *p1, DBVector *p2, gmflt v1,
             gmflt v2, DBint dir_in, DBint reverse, V2NAPA *pnp);
short EXsrul(DBId *id, DBId *ref1, DBId *ref2, DBint reverse,
             DBint par, V2NAPA *pnp);

/*
***ex24.c
*/
short EXscon(DBId *id, DBId *spine, DBint nstrips, DBId *limarr[],
             DBId *tanarr[], DBint typarr[], DBId *midarr[], V2NAPA *pnp);
short EXsoff(DBId *id, DBId *rid, gmflt offset, V2NAPA *pnp);
short EXsapp(DBId *id, DBId *rid, char *yttyp, DBint metod, gmflt tol_1,
             gmflt tol_2, V2NAPA *pnp);
short EXscom(DBId *id, DBId *ridvek, DBint nref, V2NAPA *pnp);
short EXstrm(DBId *id, DBId *rid, gmflt u1, gmflt u2, gmflt v1, gmflt v2,
             DBint fall, V2NAPA *pnp);

/*
***ex25.c
*/
short EXodco(char *source, char *user, char *passw, DBint *pstat);
short EXoded(char *sqlstr, DBint *pstat);
short EXodfe(DBint *pstat);
short EXodgs(char *bufptr, DBint *pstat);
short EXoddc(DBint *pstat);
short EXoder(char *buf);

/*
***ex26.c
*/
short EXoppt(DBId *id, DBId *refid, GMPRT *prtpek, GMPDAT *datpek,
             V2PATLOG typarr[], char valarr[]);
short EXclpt();
void  EXmsini();
short EXmoba();
short EXmogl();
short EXmolo(DBId *idvek);
short EXmlla(DBptr la);

/*
***ex27.c
*/
   short EXscur( DBId  *sur_id, DBint nu_cur, DBint nv_cur, 
                 DBId  *pVlines, char *twistmethod,
                 V2NAPA  *pnp);  
/*
***excurve.c
*/
short EXecur(DBId *id, GMCUR *curpek, GMSEG *grsegp,
             GMSEG *gesegp, V2NAPA *pnp);
short EXcufr(DBId *id, DBVector *pv, DBVector *tv, DBshort npoi, V2NAPA *pnp);
short EXcupr(DBId *id, DBVector *pv, DBVector *tv, DBshort npoi, V2NAPA *pnp);
short EXcurs(DBId *id, DBint type, DBVector *pv, DBVector *tv,
             DBint npoi, V2NAPA *pnp);
short EXcunu(DBId *id, DBVector *cpts, DBfloat *w, DBint ncpts,
             DBfloat *knots, DBint nknots,DBint degree, V2NAPA *pnp);
short EXcurc(DBId *id, bool proj, DBVector *pv, DBVector *tv,
             DBVector *iv, gmflt fv[], DBshort npoi, V2NAPA *pnp);
short EXcuro(DBId *id, DBId *rid, gmflt offs, V2NAPA *pnp);
short EXcomp(DBId *id, DBId *ridvek, DBshort nref, V2NAPA *pnp);
short EXcurt(DBId *id, DBId *rid, gmflt t1, gmflt t2, V2NAPA *pnp);
short EXcuru(DBId *id, DBshort nseg, GMSEG geoseg[] , DBId *surid, V2NAPA *pnp);

/*
***excugl.c
*/
short EXcugl(DBId *id, DBId *rid, char *str, V2NAPA *pnp);

/*
***exsugl.c
*/
short EXsugl(DBId *id, DBId *rid, char *str, V2NAPA *pnp);

/*
***exmesh.c
*/
short EXemsh(DBId *id, DBMesh *mshptr, V2NAPA *pnp);
short EXmsar(DBId *id, DBMesh *mshptr, V2NAPA *pnp);
short EXgmsh(DBId *id, DBint *nv, DBint *nh, DBint *nf, DBVector *pmin, DBVector *pmax, DBint *font);
short EXgvrt(DBId *id, DBint n, DBVector *p, DBint *eh);
short EXghdg(DBId *id, DBint n, DBint *ev, DBint *oh, DBint *nh, DBint *pf);
short EXgface(DBId *id, DBint n, DBint *h);
short EXgxflt(DBId *id, DBint n, DBfloat *h);
short EXsort_mesh(DBId *id, char mode, DBint *facelist, DBfloat *min, DBfloat *max);

/*
***extcp.c
*/
short  EXtcpco(char *server, DBint port, DBint localport, DBint *sock, DBint *pstat);
short  EXtcpli(DBint   port, DBfloat timeout, DBint *sock, DBint *pstat);
short  EXtcpcl(DBint sock, DBint *pstat);
short  EXtcpoutint(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBint data, DBint *pstat);
short  EXtcpinint(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBint *data, DBint *pstat);
short  EXtcpoutfloat(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBfloat data, DBint *pstat);
short  EXtcpinfloat(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBfloat *data, DBint *pstat);

short  EXtcpoutvec(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBfloat datax, DBfloat datay, DBfloat dataz, DBint *pstat);
short  EXtcpinvec(DBint sock,DBint bytecount, DBint byteorder, DBfloat timeout, DBfloat *datax, DBfloat *datay,DBfloat *dataz, DBint *pstat);

short  EXtcpoutchar(DBint sock, DBfloat timeout, char *data, DBint *pstat);
short  EXtcpinchar(DBint sock, DBfloat timeout, char *data, DBint *pstat);
short  EXtcpgetlocal(DBint sock, char *name, DBint *port, DBint *pstat);
short  EXtcpgetremote(DBint sock, char *name, DBint *port, DBint *pstat);

/*
***exrpc.c
*/
short EXrpcclntcr(char *server,DBint program,DBint version,DBint *Pclient, DBint *pstat);
short EXrpcclntde(DBint Pclient,DBint *pstat);
short EXrapoutfloat(DBint Pclient, char *name,  DBfloat  data, DBint *pstat);
short EXrapinfloat(DBint Pclient, char *name,  DBfloat *data, DBint *pstat);
short EXrapoutint (DBint Pclient, char *name,  DBint    data, DBint *pstat);
short EXrapinint  (DBint Pclient, char *name,  DBint   *data, DBint *pstat);

short EXrapinrobtarget(DBint Pclient,char *name, DBVector *trans, 
                       DBfloat *rot, DBint *conf, DBfloat *extax, DBint  *pstat);

short EXrapoutrobtarget(DBint Pclient, char *name, DBVector *trans,
                          DBfloat *rot, DBint *conf, DBfloat *extax, DBint *pstat);

short EXrapoutspeed(DBint Pclient, char *name, DBfloat speeddata[],DBint *pstat);

short EXrapinspeed(DBint Pclient, char *name,  DBfloat *speeddata,DBint *pstat);

short EXrapoutrobtarr(DBint Pclient, char *name, DBint first, DBint last,
             DBVector *trans, DBfloat *rot, DBint *conf, DBfloat *extax, DBint *pstat);

short EXrapinrobtarr(DBint Pclient, char *name, DBint first, DBint last,
             DBVector *trans, DBfloat *rot, DBint *conf, DBfloat *extax, DBint *pstat);

short EXrapoutfloatarr(DBint Pclient, char *name, DBint first, DBint last,
              DBfloat *data, DBint *pstat);

short EXrapinfloatarr(DBint Pclient, char *name, DBint first, DBint last,
              DBfloat *data, DBint *pstat);



             

/*
***exoru.c
*/
short EXorurstimediff(DBint Pclient, DBint Ssocket,  DBfloat *diff,DBfloat *robtime, DBint *pstat);
