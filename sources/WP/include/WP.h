/**********************************************************************
*
*    WP.h
*    ====
*
*    This file is part of the VARKON WindowPac Library.
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
***UNIX, X-Windows and OpenGL include.
*/
#ifdef UNIX
#include <X11/Xresource.h>
#ifdef V3_OPENGL
#include <GL/gl.h>
#endif
#include "wpdef.h"
#endif

/*
***Microsoft Windows WIN32.
*/
#ifdef WIN32
#include "../../MS/include/MS.h"
#endif

/*
***Common globals for X.
*/
#ifdef V3_X11
extern Display     *xdisp;
extern GC           xgc;
extern int          xscr;
extern Cursor       xgcur1,xgcur2,xwcur,xtcur;
extern XrmDatabase  xresDB;
#endif

/*
***Globals for X and WIN32.
*/
extern WPWIN        wpwtab[];

/*
***WIN32-macros.
*/
#ifdef WIN32
#define wpwgwp(id) mswgwp(id)
extern WPWIN    *mswgwp(wpw_id);
#endif

/*
***Function prototypes for the public WP API. Most
***of these are only used in the X version of Varkon
***but some are used also in the WIN32 version. A few
***of routines are only used with OpenGL.
*/
#ifdef V3_X11
/*
***wp1.c
*/
short wpinit(char *if1, char *if2);
short wpexit();
short wpclrg();
short wpsvjb(FILE *jf);
short wpldjb(FILE *jf);
int   wpngws();
short wpcgws(bool create);
short wpmrdb(int *ac, char *av[]);
short wpgtwi(DBint id, int *x, int *y, int *dx, int *dy, int *typ, char *r);
short wpgtwp(Window w, int *x, int *y);
short wpgwsz(Window w, int *px, int *py, int *dx, int *dy,
             double *xm, double *ym);
short wpsdpr(Window w);

/*
***wp2.c
*/
short wpinla(char *hs);
short wpalla(char *s, short rs);
short wpexla(bool show);
bool  wpxplw(WPLWIN *lwinpt);
bool  wpbtlw(WPLWIN *lwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool  wpcrlw(WPLWIN *lwinpt, XCrossingEvent *croev);
bool  wpcmlw(WPLWIN *lwinpt, XClientMessageEvent *clmev);
short wpdllw(WPLWIN *lwinpt);

/*
***wp3.c
*/
bool wpgrst(char *resnam, char *resval);

/*
***wp4.c
*/
short         wpfini();
short         wpgfnr(char *fntnam);
XFontStruct  *wpgfnt(short fntnum);
short         wpsfnt(short fntnum);
short         wpcini();
short         wpccol(int pen, int red, int green, int blue);
unsigned long wpgcol(short colnum);
short         wpgrgb(short colnum, XColor *rgb);
int           wpstrl(char *fstring);
short         wpgtsl(char *str, char *font, int *plen);
int           wpstrh();
short         wpgtsh(char *font, int *phgt);
int           wpftpy(int dy);
short         wpwstr(Window wid, int x, int y, char *s);
short         wpmaps(char *s);
short         wpdivs(char text[], int  maxpix, int *tdx, int *tdy,
                     char str1[], char str2[]);
short         wpd3db(char *winptr, int wintyp);

/*
***wp5.c
*/
bool  wpintr();
short wplset(bool set);
void  wplinc();
bool  wpwton();
short wpwait(DBint win_id, bool wait);
short drwait(bool draw);
short wpupwb(WPGWIN *gwinpt);
short wpposw(int  rx, int  ry, int  dx, int  dy, int *px, int *py);
short wpwlma(char *s);

/*
***wp6.c
*/
char  wpgtch( MNUALT **altptr, short *alttyp, bool flag);
short wplups(XKeyEvent *keyev, char *s, int tknmax);
short wpkepf(XKeyEvent *keyev);

/*
***wp7.c
*/
short wpmini();
short wppamu(MNUDAT *meny);
short wpwcmw(short x, short y, short dx, short dy, char *label, DBint *id);
short wprcmw();
bool  wpifae(XEvent *event, MNUALT **altptr);
void  wpfomw();

/*
***wp8.c
*/
short wpmsip(char *dps, char *ips[], char *ds[], char *is[], short maxtkn[],
             int  typarr[], short nstr);

/*
***wp9.c
*/
bool  wpialt(char *ps, char *ts, char *fs, bool pip);
short wpilst(int x, int y, char *rubrik, char *strlst[], int actalt,
             int nstr, int *palt);
short wpilse(int x, int y, char *rubrik, char *defstr, char *strlst[],
             int actalt, int nstr, char *altstr);

/*
***wp10.c
*/
short wprenw();
short wpwcrw(short x, short y, short dx, short dy, char *label, DBint *id);
bool  wpbtrw(WPRWIN *rwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool  wpxprw(WPRWIN *rwinpt, XExposeEvent *expev);
bool  wpcrrw(WPRWIN *rwinpt, XCrossingEvent *croev);
bool  wpcorw(WPRWIN *rwinpt, XConfigureEvent *conev);
bool  wpcmrw(WPRWIN *rwinpt, XClientMessageEvent *clmev);
short wpdlrw(WPRWIN *rwinpt);
short wpuact(WPRWIN *rwinpt, GLfloat active_view_matrix[]);

/*
***wp11.c
*/
short   wpwini();
bool    wpwexp(XExposeEvent *expev);
bool    wpwbut(XButtonEvent *butev, wpw_id *serv_id);
bool    wpwcro(XCrossingEvent *croev);
bool    wpwkey(XKeyEvent *keyev, int slevel, wpw_id *serv_id);
bool    wpwclm(XClientMessageEvent *clmev);
bool    wpwcon(XConfigureEvent *conev);
bool    wpwfoc(XFocusInEvent *focev);
bool    wpwrep(XReparentEvent *repev);
short   wpwshw(DBint w_id);
short   wpwwtw(DBint iwin_id, DBint slevel, DBint *subw_id);
short   wpwdel(DBint w_id);
short   wpwdls(DBint w_id, DBint sub_id);
short   wpwexi();
wpw_id  wpwffi();
wpw_id  wpwfpx(Window x_id);
WPWIN  *wpwgwp(wpw_id id);

/*
***wp12.c
*/
short wpwciw(short x, short y, short dx, short dy, char *label, DBint *id);
bool  wpxpiw(WPIWIN *iwinptr);
bool  wpbtiw(WPIWIN *iwinptr, XButtonEvent *butev, wpw_id *serv_id);
bool  wpcriw(WPIWIN *iwinpt, XCrossingEvent *croev);
bool  wpkeiw(WPIWIN *iwinpt, XKeyEvent *keyev, int slevel, wpw_id *serv_id);
bool  wpcmiw(WPIWIN *iwinpt, XClientMessageEvent *clmev);
short wpdliw(WPIWIN *iwinptr);
short wpcrfb(int pid, short x, short y, short dx, short dy, char *butstr,
             char *akod, short anum, DBint *bid);
short wpmcbu(wpw_id pid, short x, short y, short dx, short dy, short bw,
             char *str1, char *str2, char *fstr, short cb, short cf,
             DBint *bid);
short wpwcbu(Window px_id, short x, short y, short dx, short dy, short bw,
             char *str1, char *str2, char *fstr, short cb, short cf,
             WPBUTT **outptr);
bool  wpxpbu(WPBUTT *butptr);
bool  wpbtbu(WPBUTT *butptr);
bool  wpcrbu(WPBUTT *butptr, bool enter);
short wpgtbu(DBint iwin_id, DBint butt_id, DBint *status);
short wpdlbu(WPBUTT *butptr);

unsigned long wpgcbu(wpw_id p_id, int colnum);

/*
***wp13.c
*/
short wpmced(wpw_id pid, short x, short y, short dx, short dy,
             short bw, char *str, short ntkn, DBint *eid);
short wpwced(Window px_id, short x, short y, short dx, short dy,
             short bw, char *str, short ntkn, WPEDIT **outptr);
bool  wpxped(WPEDIT *edtptr);
bool  wpbted(WPEDIT *edtptr, XButtonEvent *butev);
bool  wpcred(WPEDIT *edtptr, XCrossingEvent *croev);
bool  wpkeed(WPEDIT *edtptr, XKeyEvent *keyev, int slevel);
short wpgted(DBint iwin_id, DBint edit_id, char *str);
short wpuped(WPEDIT *edtptr, char *newstr);
short wpdled(WPEDIT *edtptr);
WPEDIT *wpffoc(WPIWIN *iwinptr, int code);
short wpfoed(WPEDIT *edtptr, bool mode);

/*
***wp14.c
*/
short wpmcic(wpw_id pid, short x, short y, short bw, char *fnam,
             short cb, short cf, DBint *iid);
short wpcrfi(int pid, short x, short y, char *filnam, char *akod,
             short anum, DBint *iid);
short wpwcic(Window px_id, short x, short y, short bw, char *fnam,
             short cb, short cf, WPICON **outptr);
bool  wpxpic(WPICON *icoptr);
bool  wpbtic(WPICON *icoptr);
bool  wpcric(WPICON *icoptr, XCrossingEvent *croev);
short wpgtic(DBint iwin_id, DBint butt_id, DBint *status);
short wpdlic(WPICON *icoptr);

/*
***wp15.c
*/
short wpwcgw(short x, short y, short dx, short dy, char *label,
             bool main, DBint *id);
short wpnrgw(WPGWIN *gwinpt);
bool  wpxpgw(WPGWIN *gwinpt, XExposeEvent *expev);
bool  wpcrgw(WPGWIN *gwinpt, XCrossingEvent *croev);
bool  wpbtgw(WPGWIN *gwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool  wprpgw(WPGWIN *gwinpt, XReparentEvent *repev);
bool  wpcogw(WPGWIN *gwinpt, XConfigureEvent *conev);
bool  wpcmgw(WPGWIN *gwinpt, XClientMessageEvent *clmev);
short wpergw(DBint win_id);
short wprepa(DBint win_id);
short wpdlgw(WPGWIN *gwinpt);
WPGWIN *wpggwp(Window x_id);

/*
***wp16.c
*/
short wpgtmp(int *px, int *py);
short wpgtmc(char *pektkn, double *px, double *py, bool mark);
short wpgmc3(char *pektkn, DBVector *pout, bool mark);
short wpgtsc(bool mark, char *pektkn, short *pix, short *piy,
             wpw_id *win_id);
short wpgtsw(WPGWIN **gwinptp, int *pix1, int *piy1, int *pix2,
             int *piy2, int mode, bool prompt);
short wpneww();
short wpzoom(WPGWIN *gwinpt1);
short wpiaut(WPGWIN *gwinpt);
short wpmaut(DBint win_id);
short wpchvi(WPGWIN *gwinpt, int x, int y);
short wpnivs(WPGWIN *gwinpt, int x, int y);
short wpitsl(WPGWIN *gwinpt, int x, int y, int mode);
short wpmtsl(DBint win_id, int first, int last, int mode);
short wpgtvi(DBint win_id, char *vynamn, double *skala, double *xmin,
             double *ymin, double *xmax, double *ymax, DBTmat *vymat,
             double *persp);

/*
***wp17.c
*/
short wplstv(WPGWIN *gwinpt);
short wpperp(WPGWIN *gwinpt, int x, int y);
short wpcent(WPGWIN *gwinpt);
short wpscle(WPGWIN *gwinpt, int x, int y);
short wpacvi(char name[], DBint win_id);
short wpupvi(char *name, VY *newwin);
short wpuppr(char *name, double dist);

#endif

/*
***wp18.c
*/
bool  wpnivt(WPGWIN *gwinpt, short level);
short wpupgp(WPGWIN *gwinpt);
short wpfixg(WPGWIN *gwinpt);
short wpspen(short pen);
short wpswdt(wpw_id win_id, double width);
short wpmsiz(WPGWIN *gwinpt, VY *pminvy);
short wpclin(WPGWIN *gwinpt, WPGRPT *pt1, WPGRPT *pt2);
bool  wpcply(WPGWIN  *gwinpt, int kmin, int *kmax, double x[],
             double y[], char a[]);
short wpdply(WPGWIN *gwinpt, int k, double x[], double y[],
             char a[], bool s);
short wpfply(WPGWIN *gwinpt, int k, double x[], double y[], bool s);
short wpcpmk(DBint win_id, int ix, int iy);
short wpepmk(DBint win_id);
short wpdpmk(WPGWIN *gwinpt, int ix, int iy, bool draw);

#ifdef V3_X11
short wpscur(int win_id, bool set, Cursor cursor);
#endif

/*
***wp19.c
*/
DBptr wpgtla(DBint win_id, DBetype typmsk, short ix, short iy, DBetype *typ,
             bool *ends, bool *right);
short wpgmla(DBint win_id, short ix1, short iy1, short ix2, short iy2,
             short mode, bool hl, short *nla, DBetype typvek[], DBptr lavek[]);
short wpgmlw(DBptr lavek[], short *idant, DBetype typvek[], short hitmod);
bool  wpsply(WPGWIN *gwinpt, int k, double x[], double y[], char a[],
            DBptr la, DBetype typ);
bool  wpfobj(WPGWIN *gwinpt, DBptr la, DBetype typmsk, DBetype *typ);
bool  wpdobj(WPGWIN *gwinpt, bool s);
bool  wprobj(WPGWIN *gwinpt);
short wphgen(DBint win_id, DBptr la, bool draw);
short wperhg();

/*
***wp20.c
*/
short wpdrpo(GMPOI *poipek, DBptr la, DBint win_id);
short wpdlpo(GMPOI *poipek, DBptr la, DBint win_id);
bool  wptrpo( WPGWIN *gwinpt, DBVector *po, WPGRPT *pt);
bool  wpclpt(WPGWIN *gwinpt, WPGRPT *pt);

/*
***wp21.c
*/
short wpdrli(GMLIN *linpek, DBptr la, DBint win_id);
short wpdlli(GMLIN *linpek, DBptr la, DBint win_id);
bool  wptrli(WPGWIN *gwinpt, GMLIN  *linpek, WPGRPT *pt1, WPGRPT *pt2);

/*
***wp22.c
*/
short wpdrar(GMARC *arcpek, GMSEG *segpek, DBptr la, DBint win_id);
short wpdlar(GMARC *arcpek, GMSEG *segpek, DBptr la, DBint win_id);

/*
***wp23.c
*/
short wpdrcu(GMCUR *curpek, GMSEG *segpek, DBptr la, DBint win_id);
short wpdlcu(GMCUR *curpek, GMSEG *segpek, DBptr la, DBint win_id);
short wpmpcu(GMCUR *gmpost, GMSEG *segmnt, double scale, int *npts, double x[],
             double y[], double z[], char a[]);

/*
***wp24.c
*/
short wpdrsu(GMSUR *surpek, GMSEG *sptarr[], DBptr la, DBint win_id);
short wpdlsu(GMSUR *surpek, GMSEG *sptarr[], DBptr la, DBint win_id);

/*
***wp25.c
*/
short wpdrtx(GMTXT *txtpek, char *strpek, DBptr la, DBint win_id);
short wpdltx(GMTXT *txtpek, char *strpek, DBptr la, DBint win_id);

/*
***wp26.c
*/
short wpdrxh(GMXHT *xhtpek, gmflt crdvek[], DBptr la, DBint win_id);
short wpdlxh(GMXHT *xhtpek, gmflt crdvek[], DBptr la, DBint win_id);

/*
***wp27.c
*/
short wpdrdm(GMUNON *dimpek, DBptr la, DBint win_id);
short wpdldm(GMUNON *dimpek, DBptr la, DBint win_id);

/*
***wp28.c
*/
short wpdrcs(GMCSY *csypek, DBTmat *csymat, DBptr la, DBint win_id);
short wpdlcs(GMCSY *csypek, DBTmat *csymat, DBptr la, DBint win_id);
short wpupcs(GMCSY *csypek, DBTmat *pmat, DBptr la, int mode, DBint win_id);
short wpplcs(WPGWIN *gwinpt, GMCSY *csypek, DBTmat *csymat, int *n,
             double x[], double y[], double z[], char a[]);
short wpmpcs(GMCSY *csypek, double size, int font, int *npts, double x[],
             double y[], double z[], char a[]);

/*
***wp29.c
*/
short wpdrbp(GMBPL *bplpek, DBptr la, DBint win_id);
short wpdlbp(GMBPL *bplpek, DBptr la, DBint win_id);
short wpplbp(WPGWIN *gwinpt, GMBPL *bplpek, int *n, double x[], double y[],
             double z[], char a[]);

/*
***wp30.c
*/
#ifdef V3_X11

short wpamod();
short wpomod();
short wpcoal();

/*
***wp31.c
*/
short wpinsh(int wid, double zmin, double zmax, bool smooth);
short wpexsh();

#endif

/*
***wp32.c
*/
short wpshad(int wid, bool smooth);
short wpdrsh(WPGWIN *gwinpt);
short wpscsh(int pen);
short wpltvi(DBint ltnum, DBVector *pos1, DBVector *pos2,
             DBfloat ang, DBfloat focus);
short wpmtvi(int mtnum, gmflt ar, gmflt ag, gmflt ab, gmflt dr, gmflt dg,
             gmflt db, gmflt sr, gmflt sg, gmflt sb, gmflt er, gmflt eg,
             gmflt eb, gmflt sh);
short wplton(int ltnum, gmflt intensity, bool state);
short wpconl(WPGWIN *gwinpt);

/*
***wp33.c
*/
short wpmmod(WPRWIN *rwinpt);
short wpmodl(WPRWIN *rwinpt);
#ifdef V3_OPENGL
short wpsodl(WPRWIN *rwinpt,GLuint list);
#else
short wpsodl(WPRWIN *rwinpt,int list);
#endif
short wpscog(WPRWIN *rwinpt, short pen);
short wpnrrw(WPRWIN *rwinpt);

/*
***wp34.c
*/
short wpdrms(DBMesh *mshptr, DBptr la, DBint win_id);
short wpdlms(DBMesh *mshptr, DBptr la, DBint win_id);
short wpplms(WPGWIN *gwinpt, DBMesh *mshptr, int *n, double x[], double y[],
             double z[], char a[]);
