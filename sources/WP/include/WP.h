/**********************************************************************
*
*    WP.h
*    ====
*
*    This file is part of the VARKON WindowPac Library.
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

/*
***UNIX, X-Windows and OpenGL include.
*/
#ifdef UNIX
#include <X11/Xresource.h>
#include <GL/gl.h>
#include "wpdef.h"
#endif

/*
***Microsoft Windows WIN32.
*/
#ifdef WIN32
#include "../../MS/include/MS.h"
#endif

/*
***Line and Arc fonts.
*/
#define SOLIDLN  0           /* LFONT=0, Solid */ 
#define DASHLN   1           /* LFONT=1, Dashed */ 
#define DADOLN   2           /* LFONT=2, Phantom */ 
#define DOTLN    3           /* LFONT=3, Dash dotted */
#define SOLIDARC 0           /* AFONT=0, Solid */ 
#define DASHARC  1           /* AFONT=1, Dashed */ 
#define DADOARC  2           /* AFONT=2, Phantom */ 

#define DASHRATIO 0.25       /* Space/dash */ 
#define DOTRATIO  5.0        /* Space/dot */ 
#define STOL      0.0        /* Minimum dash length */
#define DTOL      1.0E-10    /* A very small distance */

/*
***Common globals for X.
*/
#ifdef UNIX
extern Display     *xdisp;
extern GC           xgc;
extern int          xscr;
extern Cursor       xgcur1,xgcur2,xgcur3,xwcur;
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
#define WPwgwp(id) mswgwp(id)
extern WPWIN    *mswgwp(wpw_id);
#endif

/*
***GKS metafile definitions.
*/

#define MAXMETA  1000        /* Max ant tecken i en metafilpost */

#define TYPLEN   4           /* Typf�ltets l�ngd */
#define DATLEN   6           /* Dataf�ltets l�ngd */
#define INTLEN   6           /* Ett heltalsf�lts l�ngd */
#define FLTLEN   10          /* Ett flyttalsf�lts l�ngd */

#define ENDITEM      0       /* End of metafile */
#define CLEARITEM    1       /* Clear workstation */
#define VAR_WINDOW 175       /* Varkon window */
#define LINWDTITEM 174       /* Varkon linewidth */
#define PENNRITEM  173       /* Varkon pen number */
#define POLYLNITEM  11       /* Polyline */
#define FILLITEM    14       /* Fill area */

typedef struct metadef
{
    int pennr;               /* Aktuell penna */   
    int datlen;              /* Dataf�ltets l�ngd */
    int typlen;              /* Typf�ltets l�ngd */
    int fltlen;              /* Ett flyttalsf�lts l�ngd */
    int intlen;              /* Ett heltalsf�lts l�ngd */
    char formtyp[ 16 ];      /* Formatstr�ng f�r typf�lt */
    char formdat[ 16 ];      /* Formatstr�ng f�r dataf�lt */
    char formint[ 16 ];      /* Formatstr�ng f�r heltalsf�lt */
    char formflt[ 16 ];      /* Formatstr�ng f�r flyttalsf�lt */
} METADEF;

/*
***Function prototypes for the public WP API. Most
***of these are only used in the X version of Varkon
***but some are used also in the WIN32 version. A few
***of routines are only used with OpenGL.
*/
#ifdef UNIX
/*
***wpinit.c
*/
short WPinit(char *if1, char *if2);
short WPexit();
short WPclrg();
int   WPngws();
short WPcgws();
short WPmrdb(int *ac, char *av[]);
short WPgtwi(DBint id, int *x, int *y, int *dx, int *dy, int *typ, char *r);
short WPgtwp(Window w, int *x, int *y);
short WPgwsz(Window w, int *px, int *py, int *dx, int *dy,
             double *xm, double *ym);
short WPsdpr(Window w);
bool  WPgrst(char *resnam, char *resval);

/*
***wpjob.c
*/
short WPsave_job(FILE *jf);
short WPload_WPGWIN(FILE *jf);
short WPload_WPRWIN(FILE *jf);

/*
***wpLWIN.c
*/
short WPinla(char *hs);
short WPalla(char *s, short rs);
short WPexla(bool show);
bool  WPxplw(WPLWIN *lwinpt);
bool  WPbtlw(WPLWIN *lwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool  WPcrlw(WPLWIN *lwinpt, XCrossingEvent *croev);
bool  WPcmlw(WPLWIN *lwinpt, XClientMessageEvent *clmev);
short WPdllw(WPLWIN *lwinpt);

/*
***wpfont.c
*/
short         WPfini();
short         WPgfnr(char *fntnam);
XFontStruct  *WPgfnt(short fntnum);
short         WPsfnt(short fntnum);
int           WPstrl(char *fstring);
short         WPgtsl(char *str, char *font, int *plen);
int           WPstrh();
short         WPgtsh(char *font, int *phgt);
int           WPftpy(int dy);
short         WPwstr(Window wid, int x, int y, char *s);
short         WPdivs(char text[], int  maxpix, int *tdx, int *tdy,
                     char str1[], char str2[]);
short         WPd3db(char *winptr, int wintyp);

/*
***wpwait.c
*/
bool  WPintr();
short WPlset(bool set);
void  WPlinc();
bool  WPwton();
short WPwait(DBint win_id, bool wait);
short drwait(bool draw);
short WPupwb(WPGWIN *gwinpt);
short WPposw(int  rx, int  ry, int  dx, int  dy, int *px, int *py);
short WPwlma(char *s);

/*
***wpgetkey.c
*/
char  WPgtch( MNUALT **altptr, short *alttyp, bool flag);
short WPlups(XKeyEvent *keyev, char *s, int tknmax);
short WPkepf(XKeyEvent *keyev);

/*
***wpmenu.c
*/
short WPinit_menu();
short WPactivate_menu(MNUDAT *meny);
short WPupdate_menu();
bool  WPmenu_button(XEvent *event, MNUALT **altptr);
void  WPfocus_menu();

/*
***wpgetstr.c
*/
short WPmsip(char *dps, char *ips[], char *ds[], char *is[], short maxtkn[],
             int  typarr[], short nstr);

/*
***wpgetalt.c
*/
bool  WPialt(char *ps, char *ts, char *fs, bool pip);
short WPilse(char *rubrik, char *defstr, char *strlst[],
             int actalt, int nstr, char *altstr);
short WPselect_partname(int source, char *namlst[], int nstr, char *name);

/*
***wpgetpos.c
*/
short WPgtmp(int *px, int *py);
short WPgmc2(bool mark, char *pektkn, double *px, double *py, wpw_id *grw_id);
short WPgmc3(bool mark, char *pektkn, DBVector *pout, wpw_id *grw_id);
short WPgtsc(bool mark, char *pektkn, short *pix, short *piy,
             wpw_id *win_id);
short WPgtsw(wpw_id  *grw_id, int *pix1, int *piy1, int *pix2,
             int *piy2, int mode, bool prompt);
short WPneww();

/*
***wpw.c
*/
short   WPwini();
bool    WPwexp(XExposeEvent *expev);
bool    WPwbut(XButtonEvent *butev, wpw_id *serv_id);
bool    WPwcro(XCrossingEvent *croev);
bool    WPwkey(XKeyEvent *keyev, int slevel, wpw_id *serv_id);
bool    WPwclm(XClientMessageEvent *clmev);
bool    WPwcon(XConfigureEvent *conev);
bool    WPwfoc(XFocusInEvent *focev);
bool    WPwrep(XReparentEvent *repev);
short   WPwshw(DBint w_id);
short   WPwwtw(DBint iwin_id, DBint slevel, DBint *subw_id);
short   WPwdel(DBint w_id);
short   WPwdls(DBint w_id, DBint sub_id);
short   WPwexi();
wpw_id  WPwffi();
wpw_id  WPwfpx(Window x_id);
WPWIN  *WPwgwp(wpw_id id);
void    WPbell();

/*
***wpIWIN.c
*/
short WPwciw(short x, short y, short dx, short dy, char *label, DBint *id);
bool  WPxpiw(WPIWIN *iwinptr);
bool  WPbtiw(WPIWIN *iwinptr, XButtonEvent *butev, wpw_id *serv_id);
bool  WPcriw(WPIWIN *iwinpt, XCrossingEvent *croev);
bool  WPkeiw(WPIWIN *iwinpt, XKeyEvent *keyev, int slevel, wpw_id *serv_id);
bool  WPcmiw(WPIWIN *iwinpt, XClientMessageEvent *clmev);
short WPdliw(WPIWIN *iwinptr);

/*
***wpBUTT.
*/
short WPcrfb(int pid, short x, short y, short dx, short dy, char *butstr,
             char *akod, short anum, DBint *bid);
short WPmcbu(wpw_id pid, short x, short y, short dx, short dy, short bw,
             char *str1, char *str2, char *fstr, short cb, short cf,
             DBint *bid);
short WPwcbu(Window px_id, short x, short y, short dx, short dy, short bw,
             char *str1, char *str2, char *fstr, short cb, short cf,
             WPBUTT **outptr);
bool  WPxpbu(WPBUTT *butptr);
void  WPscbu(WPBUTT *butptr, int color);
bool  WPbtbu(WPBUTT *butptr);
bool  WPcrbu(WPBUTT *butptr, bool enter);
short WPgtbu(DBint iwin_id, DBint butt_id, DBint *status);
short WPdlbu(WPBUTT *butptr);

unsigned long WPgcbu(wpw_id p_id, int colnum);

/*
***wpEDIT.c
*/
short   WPmced(wpw_id pid, short x, short y, short dx, short dy,
               short bw, char *str, short ntkn, DBint *eid);
short   WPwced(Window px_id, short x, short y, short dx, short dy,
               short bw, char *str, short ntkn, WPEDIT **outptr);
bool    WPxped(WPEDIT *edtptr);
bool    WPbted(WPEDIT *edtptr, XButtonEvent *butev);
bool    WPcred(WPEDIT *edtptr, bool enter);
bool    WPkeed(WPEDIT *edtptr, XKeyEvent *keyev, int slevel);
short   WPgted(DBint iwin_id, DBint edit_id, char *str);
short   WPuped(DBint iwin_id, DBint edit_id, char *newstr);
short   WPdled(WPEDIT *edtptr);
WPEDIT *WPffoc(WPIWIN *iwinptr, int code);
short   WPfoed(WPEDIT *edtptr, bool mode);
void    WPmap_key(XKeyEvent *keyev,  short *sym, char *t);

/*
***wpICON.c
*/
short WPmcic(wpw_id pid, short x, short y, short bw, char *fnam,
             short cb, short cf, DBint *iid);
short WPcrfi(int pid, short x, short y, char *filnam, char *akod,
             short anum, DBint *iid);
short WPwcic(Window px_id, short x, short y, short bw, char *fnam,
             short cb, short cf, WPICON **outptr);
bool  WPxpic(WPICON *icoptr);
bool  WPbtic(WPICON *icoptr);
bool  WPcric(WPICON *icoptr, XCrossingEvent *croev);
short WPgtic(DBint iwin_id, DBint butt_id, DBint *status);
short WPdlic(WPICON *icoptr);

/*
***wpGWIN.c
*/
short   WPwcgw(short x, short y, short dx, short dy, char *label,
               bool main, DBint *id);
short   WPnrgw(WPGWIN *gwinpt);
bool    WPxpgw(WPGWIN *gwinpt, XExposeEvent *expev);
bool    WPcrgw(WPGWIN *gwinpt, XCrossingEvent *croev);
bool    WPbtgw(WPGWIN *gwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool    WPrpgw(WPGWIN *gwinpt, XReparentEvent *repev);
bool    WPcogw(WPGWIN *gwinpt, XConfigureEvent *conev);
bool    WPcmgw(WPGWIN *gwinpt, XClientMessageEvent *clmev);
short   WPergw(DBint win_id);
short   WPrepaint_GWIN(DBint win_id);
short   WPdlgw(WPGWIN *gwinpt);
WPGWIN *WPggwp(Window x_id);

/*
***wpRWIN.c
*/
short   WPrenw();
short   WPwcrw(int x, int y, int dx, int dy, char *label, DBint *id);
bool    WPbtrw(WPRWIN *rwinpt, XButtonEvent *butev, wpw_id *serv_id);
bool    WPxprw(WPRWIN *rwinpt, XExposeEvent *expev);
bool    WPcrrw(WPRWIN *rwinpt, XCrossingEvent *croev);
bool    WPcorw(WPRWIN *rwinpt, XConfigureEvent *conev);
bool    WPcmrw(WPRWIN *rwinpt, XClientMessageEvent *clmev);
void    WPrepaint_RWIN(DBint win_id, bool resize);
short   WPerrw(DBint win_id);
short   WPdlrw(WPRWIN *rwinpt);
WPRWIN *WPgrwp(Window x_id);

/*
***wpviews.c
*/
short WPinit_views();
short WPview_dialogue(DBint grw_id);
short WPcreate_view(WPVIEW *view, int status);
short WPcampos_to_matrix(DBVector *campos, VYTRF *matrix);
short WPreload_view(DBint win_id);
short WPactivate_view(char name[], WPGWIN *gwinpt, WPRWIN *rwinpt, int wintype);
short WPdelete_view(char name[]);
short WPgtvi(DBint win_id, char *vynamn, double *skala, double *xmin,
             double *ymin, double *xmax, double *ymax, DBTmat *vymat,
             double *persp);

/*
***wplevels.c
*/
short WPlevels_dialogue(DBint grw_id);
short WPmtsl(DBint win_id, int first, int last, int mode);
bool  WPnivt(unsigned char *lvltab, int level);

/*
***wpzoom.c
*/
short WPzoom();
short WPiaut(WPGWIN *gwinpt, WPRWIN *rwinpt, int wintype);
short WPmaut(DBint win_id);
short WPauto_modwin(WPGWIN *gwinpt, VYWIN *minwin);
short WPlstv(WPGWIN *gwinpt);
short WPperp(WPGWIN *gwinpt, int x, int y);
short WPcent(WPGWIN *gwinpt);
short WPscle(WPGWIN *gwinpt, int x, int y);
short WPupvi(char *name, WPVIEW *newwin);
short WPuppr(char *name, double dist);

#endif

/*
***wpplylin.c
*/
short WPswdt(wpw_id win_id, double width);
short WPmsiz(WPGWIN *gwinpt, WPVIEW *pminvy);
short WPclin(VYWIN *clipwin, WPGRPT *pt1, WPGRPT *pt2);
bool  WPcply(VYWIN *clipwin, int kmin, int *kmax, double x[],
             double y[], char a[]);
short WPpply(WPGWIN *gwinpt, int k, double x[], double y[], double z[]);
short WPdply(WPGWIN *gwinpt, int k, double x[], double y[],
             char a[], bool s);
short WPfply(WPGWIN *gwinpt, int k, double x[], double y[], bool s);
short WPcpmk(DBint win_id, int ix, int iy);
short WPepmk(DBint win_id);
short WPdpmk(WPGWIN *gwinpt, int ix, int iy, bool draw);
bool  WPppos(WPGWIN *gwinpt, DBVector *po, WPGRPT *pt);
bool  WPcpos(WPGWIN *gwinpt, WPGRPT *pt);
void  WPset_cacc(double newcacc);
void  WPget_cacc(double *caccpt);

#ifdef UNIX
short WPscur(int win_id, bool set, Cursor cursor);
#endif

/*
***wpDF.c
*/
DBptr WPgtlarw(WPRWIN *rwinpt, DBetype typmsk, int ix, int iy);
DBptr WPgtla(DBint win_id, DBetype typmsk, short ix, short iy, DBetype *typ,
             bool *ends, bool *right);
short WPgmla(DBint win_id, short ix1, short iy1, short ix2, short iy2,
             short mode, bool hl, short *nla, DBetype typvek[], DBptr lavek[]);
short WPgmlw(DBptr lavek[], int *idant, DBetype typvek[], short hitmod);
bool  WPsply(WPGWIN *gwinpt, int k, double x[], double y[], char a[],
            DBptr la, DBetype typ);
bool  WPfobj(WPGWIN *gwinpt, DBptr la, DBetype typmsk, DBetype *typ);
bool  WPdobj(WPGWIN *gwinpt, bool s);
bool  WProbj(WPGWIN *gwinpt);
void  WPhgen(DBint win_id, DBptr la, bool draw);
void  WPerhg();
short WPrwms(WPRWIN *rwinpt, DBptr lavek[], int *idant, DBetype typvek[],
             int xc, int yc, int xwidth, int yheight);

/*
***wppoint.c
*/
short WPdrpo(DBPoint *poipek, DBptr la, DBint win_id);
short WPdlpo(DBPoint *poipek, DBptr la, DBint win_id);
short WPplpt(DBPoint *pointp, double  size, int *n, double x[],
             double  y[], double  z[], char a[]);

/*
***wpline.c
*/
short WPdrli(DBLine *linpek, DBptr la, DBint win_id);
short WPdlli(DBLine *linpek, DBptr la, DBint win_id);
short WPplli(DBLine *linpek, int *n, double x[], double y[], double z[], char a[]);

/*
***wparc.c
*/
short WPdrar(DBArc *arcpek, DBSeg *segpek, DBptr la, DBint win_id);
short WPdlar(DBArc *arcpek, DBSeg *segpek, DBptr la, DBint win_id);
short WPplar(DBArc *arcpek, DBSeg *segmnt, double wsm, int   *n,
             double x[], double y[], double z[], char   a[]);

/*
***wpcurve.c
*/
short WPdrcu(DBCurve *curpek, DBSeg *segpek, DBptr la, DBint win_id);
short WPdlcu(DBCurve *curpek, DBSeg *segpek, DBptr la, DBint win_id);
short WPplcu(DBCurve *gmpost, DBSeg *segmnt, double scale, int *npts, double x[],
             double y[], double z[], char a[]);

/*
***wpsurf.c
*/
short WPdrsu(DBSurf *surpek, DBSegarr *pborder, DBSegarr *piso, DBptr la, DBint win_id);
short WPdlsu(DBSurf *surpek, DBSegarr *pborder, DBSegarr *piso, DBptr la, DBint win_id);
short WPplsu(DBSurf *surpek, DBSegarr *pborder, DBSegarr *piso, double scale, int *n,
             double x[], double y[], double z[], char a[]);
short WPuvstepsu(DBSurf *surpek, DBPatch *patpek, DBfloat uscale, DBfloat vscale);

/*
***wptext.c
*/
short WPdrtx(DBText *txtpek, char *strpek, DBptr la, DBint win_id);
short WPdltx(DBText *txtpek, char *strpek, DBptr la, DBint win_id);
short WPpltx(DBText *txtpek, unsigned char *strpek, int *k, double x[],
             double y[], double z[], char a[]);
short WPprtx(WPGWIN *gwinpt, DBText *txtpek, int npts,
             double x[], double y[], double z[]);
short WPinfn();
short WPldfn(int font, char *filnam);
short WPexfn();

/*
***wpxhatch.c
*/
short WPdrxh(DBHatch *xhtpek, DBfloat crdvek[], DBptr la, DBint win_id);
short WPdlxh(DBHatch *xhtpek, DBfloat crdvek[], DBptr la, DBint win_id);
short WPplxh(DBHatch *xhtptr, DBfloat  crdvek[], int *n,
        double x[], double y[], double z[], char a[]);

/*
***wpdim.c
*/
short WPdrdm(DBAny *dimpek, DBptr la, DBint win_id);
short WPdldm(DBAny *dimpek, DBptr la, DBint win_id);
short WPplld(DBLdim *dimpek, int *n, double x[], double y[], double z[], char a[]);
short WPplcd(DBCdim *dimpek, int *n, double x[], double y[], double z[], char a[]);
short WPplrd(DBRdim *dimpek, int *n, double x[], double y[], double z[], char a[]);
short WPplad(DBAdim *dimpek, double scale, int *n, double x[], double y[], double z[], char a[]);

/*
***wpcsy.c
*/
short WPdrcs(DBCsys *csypek, DBptr la, DBint win_id);
short WPdlcs(DBCsys *csypek, DBptr la, DBint win_id);
short WPupcs(DBCsys *csypek, DBptr la, int mode, DBint win_id);
short WPplcs(DBCsys *csypek, double size, int mode, int *n,
             double x[], double y[], double z[], char a[]);

/*
***wpbplane.c
*/
short WPdrbp(DBBplane *bplpek, DBptr la, DBint win_id);
short WPdlbp(DBBplane *bplpek, DBptr la, DBint win_id);
short WPplbp(DBBplane *bplpek, int *n, double x[], double y[],
             double z[], char a[]);
/*
***wpmesh.c
*/
short WPdrms(DBMesh *mshptr, DBptr la, DBint win_id);
short WPdlms(DBMesh *mshptr, DBptr la, DBint win_id);
short WPplms(DBMesh *mshptr, double size, int *n, double x[], double y[],
             double z[], char a[]);

/*
***wpedmbs.c
*/
#ifdef UNIX

short WPamod();
short WPomod();
short WPcoal();

/*
***wppen.c
*/
short         WPcini();
short         WPccol(int pen, int red, int green, int blue);
unsigned long WPgcol(int colnum);
short         WPspen(int pen);
short         WPgpen(int pen, int *red, int *green, int *blue);
short         WPspenGL(WPRWIN *rwinpt, int pen);
short         WPcmat(int mtnum, DBfloat ar, gmflt ag, gmflt ab, gmflt dr, gmflt dg,
                     DBfloat db, gmflt sr, gmflt sg, gmflt sb, gmflt er, gmflt eg,
                     DBfloat eb, gmflt sh);
/*
***wplight.c
*/
short WPltvi(DBint ltnum, DBVector *pos1, DBVector *pos2,
             DBfloat ang, DBfloat focus);
short WPlton(int ltnum, DBfloat intensity, bool state);
short WPconl(WPGWIN *gwinpt);
#endif

/*
***wpGLlist.c
*/
short WPmmod(WPRWIN *rwinpt);
short WPmodl_all(WPRWIN *rwinpt);
short WPmodl_highlight(WPRWIN *rwinpt, int n_ents, DBptr * la_arr);
void  WPdodl_highlight(WPRWIN *rwinpt);
short WPsodl_all(WPRWIN *rwinpt);
short WPeodls(WPRWIN *rwinpt, int ix, int iy, int dix, int diy);
short WPnrrw(WPRWIN *rwinpt);

/*
***wpplot.c
*/
short WPmkpf(WPGWIN *gwinpt, FILE *filpek, VYWIN *plotwin, DBVector *origo);
short WPgksm_header(METADEF *mdp, FILE *filpek, char metarec[]);
short WPgksm_window(METADEF *mdp, FILE *filpek, char metarec[], VYWIN *plotwin, DBVector *origo);
short WPgksm_polyline(METADEF *mdp, FILE *filpek, int k, double x[], double y[], char a[],
                      VYWIN *plotwin, char metarec[]);
short WPgksm_fill(METADEF *mdp, FILE *fp, int k, double x[], double y[], VYWIN *plotwin);
short WPgksm_clear(METADEF *mdp, FILE *filpek, char metarec[]);
short WPgksm_pen(METADEF *mdp, FILE *filpek, short pen, char metarec[]);
short WPgksm_width(METADEF *mdp, FILE *filpek, double wdt, char metarec[]);
short WPgksm_end(METADEF *mdp, FILE *filpek, char metarec[]);

/*
***wphide.c
*/
short WPhide(WPGWIN *gwinpt, bool flag1, bool flag2, FILE *pfil, DBVector *origo);

/*
***wpdxf.c
*/
short WPdxf_out(WPGWIN *gwinpt, FILE *filpek, WPVIEW *plotvy, DBVector *origo);

/*
***wpgrid.c
*/
short WPgrid_dialogue(DBint grw_id);
short WPdraw_grid(wpw_id grw_id);
short WPdelete_grid(wpw_id grw_id);
short WPget_grid(double *ox,double *oy,double *dx, double *dy,bool *on);
short WPset_grid(double ox,double oy,double dx, double dy);
void  WPgrid_on(wpw_id grw_id);
void  WPgrid_off(wpw_id grw_id);

/*
***wptooltip.c
*/
void WPorder_tooltip(int ix, int iy, char *tip);
void WPclear_tooltip();
void WPshow_tooltip();
void WPexpose_tooltip();

/*
***wpattributes.c
*/
short WPatdi();

/*
***wpprint.c
*/
short WPprint_dialogue(DBint grw_id);

/*
***wpMCWIN.c
*/
short WPcreate_mcwin(wpw_id grw_id, int dy);
short WPaddmess_mcwin(char *str,int type);
void  WPclear_mcwin();
bool  WPexpose_mcwin(WPMCWIN *mcwptr);
bool  WPbutton_mcwin(WPMCWIN *mcwptr, XButtonEvent *butev);
bool  WPkey_mcwin(XKeyEvent *keyev);
void  WPunfocus_mcwin();
short WPdelete_mcwin(WPMCWIN *mcwptr);
