/**********************************************************************
*
*    MS.h
*    ====
*
*    This file is part of the VARKON WindowPac library
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
*    (C)Johan Kjellander 1984-1999, Microform AB
*    (C)Johan Kjellander 2000-2004, Örebro university
*
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <windows.h>

#ifdef V3_OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#else
#define GLfloat float
#endif

#include "msdef.h"
#include "../src/resource.h"

/*
***Globala variable commonly used
*/
extern HINSTANCE ms_inst;
extern int       ms_wmod;
extern HWND      ms_main;
extern WPMES32   ms_lstmes;
extern HDC       ms_dc;
extern HDC       ms_bmdc;
extern HFONT     ms_fonth;

/*
***Function prototypes.
*
***ms1.c
*/
int     msinit(char *inifil);
int     msexit();
int     mscdgw(bool create);
int     msclrg();
short   mssvjb(FILE *f);
short   msldjb(FILE *f, V3MDAT *psyd);
int     msmbox(char *rubrik, char *text, int typ);
int     msmwsx();
int     msmwsy();
int     msggeo(HWND win32_id, int *px,int *py,int *pxp,int *pyp, double *pxm,double *pym);
int     msngws();
int     msprtf(char *rubrik, char *filnam);
int     msgtwi(v2int id, int *px,int *py,int *pdx,int *pdy,int *ptyp, char *rubrik);
char   *msmest(UINT);

/*
***ms2.c
*/
int     msinla(char *hs);
int     msalla(char *s, short rs);
int     msexla(bool show);
int     mssvlw(WPLWIN *lwinpt);
int     msdllw(WPLWIN *lwinpt);
WPLWIN *msglwp(HWND win32_id);

/*
***ms3.c
*/
short   msdl01(short *typ, short *att);
int     msdl02(double *skala, char *vynamn);
int     msdl03(char *dp, char *ip[], char *ds[], char *is[], short ml[], int ns);
int     msdl07(int *level, int *count, int *state);
int     msdl08(double *cnog);
bool    msialt(char *ps, char *ts, char *fs, bool pip);
short   msview();

/*
***ms4.c
*/
int     msfini();
int     msgfnr(char *fntsiz);
HFONT   msgfnt(int fntnum);
int     msfexi();
int     mscini();
int     mscexi();
int     msccol(int pen,int red,int green,int blue);
HPEN    msgcol(int colnum);
HBRUSH  msgbrush(int colnum);
int     msgrgb(int colnum, COLORREF *pcolor);
int     msstrl(char *str);
int     msgtsl(char *str, char *font, int *plen);
int     msstrh();
int     msgtsh(char *font, int *phgt);
int     msftpy(int dy);
int     mswstr(HDC dc, int x, int y, char *s);
int     msdivs(char text[], int maxpix, int *tdx, int *tdy, char str1[], char str2[]);

/*
***ms5.c
*/
int     mswlma(char *s);
bool    msrpma(HWND win32_id);
int     msupwb(WPGWIN *gwinpt);

/*
***ms6.c
*/
char    msgtch(MNUALT **altptr, short *alttyp, bool flag);
int     mssacd(HWND win32_id);

/*
***ms7.c
*/
int     msmini();
int     msmexi();
int     mspamu(MNUDAT *meny);
int     mswcmw(int x, int y, int dx, int dy, char *label, v2int *id);
int     msrcmw();
bool    msifae(HWND win32_id, MNUALT **altptr);
int     mscrmu();
int     mssmmu(int mmain);
int     msshmu(int meny);
int     mshdmu();

/*
***ms8.c
*/
int     msmsip(char *dps, char *ips[], char *ds[], char *is[], short maxtkn[],
               int typarr[], short nstr);

/*
***ms9.c
*/
int     msilst(int x, int y, char *rubrik, char *strlst[], int actalt, int nstr, int *palt);
int     msilse(int x, int y, char *rubrik, char *defstr, char *strlst[], int actalt, int nstr, char *altstr);

/*
***ms10.c
*/
short   msrenw();
int     mswcrw(int x, int y, int dx, int dy, char *label, v2int *id);
bool    msrprw(WPRWIN *rwinpt);
int     msdlrw(WPRWIN *rwinpt);
bool    msbtrw(WPRWIN *rwinpt, WPMES32 *butmes, wpw_id *serv_id);
bool    msrsrw(WPRWIN *rwinpt, int newdx, int newdy);
WPRWIN *msgrwp(HWND win32_id);

/*
***ms11.c
*/
int     mswini();
int     mswexi();
bool    msregc();
bool    mswrep(HWND win32_id);
bool    mswres(HWND win32_id);
bool    mswbut(WPMES32 *butmes, wpw_id *serv_id);
void    msrpmw();
int     mswshw(v2int w_id);
int     mswwtw(v2int iwin_id, v2int slevel, v2int *subw_id);
int     mswdel(v2int w_id);
int     mswdls(v2int w_id, v2int sub_id);
wpw_id  mswffi();
wpw_id  mswfpw(HWND win32_id);
WPWIN  *mswgwp(wpw_id id);

/*
***ms12.c
*/
int     mswciw(int x, int y, int dx, int dy, char *label, v2int *id);
bool    msrpiw(WPIWIN *iwinpt);
bool    msbtiw(WPIWIN *iwinptr, WPMES32 *butmes, wpw_id *serv_id);
int     msdliw(WPIWIN *iwinptr);
int     mscrfb(wpw_id pid, int x, int y, int dx, int dy, char *butstr,
               char *akod, int anum, v2int *bid);
int     mscrdb(wpw_id pid, int x, int y, int dx, int dy, int bw,
               char *str, char *fstr, int cb, int cf, v2int *bid);
int     msmcbu(wpw_id pid, int x, int y, int dx, int dy, int bw,
               char *str1, char *str2, char *fstr, int cb, int cf, v2int *bid);
int     mswcbu(HWND ms_pid, int x, int y, int dx, int dy, int bw, char *str1,
               char *str2, char *fstr, int cb, int cf, int mode, int bid, WPBUTT **outptr);
bool    msrpbu(WPBUTT *butptr);
bool    msbtbu(WPBUTT *butptr);
int     msgtbu(v2int iwin_id, v2int butt_id, v2int *status);
int     msdlbu(WPBUTT *butptr);
WPBUTT *msdefb(wpw_id iwin_id);
WPBUTT *msgbup(HWND win32_id);

/*
***ms13.c
*/
int     msmced(wpw_id  pid, int x, int y, int dx, int dy, int bw, char *str,
               int ntkn, v2int *eid);
int     mswced(HWND ms_pid, int x, int y, int dx, int dy, int bw, char *str,
               int ntkn, WPEDIT **outptr);
int     msgted(v2int iwin_id, v2int edit_id, char *str);
int     msdled(WPEDIT *edtptr);
int     msfoeb(WPIWIN *iwinpt, wpw_id edbuid, bool mode);
wpw_id  msffoc(WPIWIN *iwinptr, int code);
WPEDIT *msgedp(HWND win32_id);

/*
***ms14.c
*/

int     msmcic(wpw_id pid, int x, int y, int bw, char *fnam, int cb, int cf, v2int *iid);
int     mscrfi(wpw_id pid, int x, int y, char *filnam, char *akod, int anum, v2int *iid);
int     mswcic(HWND ms_pid, int x, int y, int bw, char *fnam, int cb, int cf, WPICON **outptr);
bool    msrpic(WPICON *icoptr);
bool    msbtic(WPICON *icoptr);
int     msdlic(WPICON *icoptr);

/*
***ms15.c
*/
int     mswcgw(int x, int y, int dx, int dy, char *label, bool main, v2int *id);
int     msnrgw(WPGWIN *gwinpt);
bool    msrpgw(WPGWIN *gwinpt);
int     msrepa(v2int win_id);
bool    msbtgw(WPGWIN *gwinpt, WPMES32 *butmes, wpw_id *serv_id);
bool    msrsgw(WPGWIN *gwinpt, int newdx, int newdy);
int     msergw(v2int win_id);
int     msdlgw(WPGWIN *gwinpt);
WPGWIN *msggwp(HWND win32_id);

/*
***ms16.c
*/
int     msgtmp(int *px, int *py);
int     msgtmc(char *pektkn, double *px, double *py, bool mark);
int     msgmc3(char *pektkn, DBVector *pout, bool mark);
int     msgtsc(bool mark, char *pektkn, short *pix, short *piy, wpw_id *win_id);
int     msiaut(WPGWIN *gwinpt);
int     mschvi(WPGWIN *gwinpt, int x, int y);
int     msgtsw(WPGWIN **gwinptp, int *pix1, int *piy1, int *pix2, int *piy2,
               int rubmod, bool prompt);
int     mszoom();
short   msneww();
int     msmaut(v2int win_id);
int     msgtvi(v2int win_id, char *vynamn, double *skala, double *xmin, double *ymin,
               double *xmax, double *ymax, DBTmat *vymat, double *persp);

/*
***ms17.c
*/
int     mslstv(WPGWIN *gwinpt);
int     msperp(WPGWIN *gwinpt, int x, int y);
int     mscent(WPGWIN *gwinpt);
int     msscle(WPGWIN *gwinpt, int x, int y);
int     msnivs(WPGWIN *gwinpt, int x, int y);
int     msacvi(char name[], v2int win_id);
int     msupvi(char *name, VY *newwin);
int     msuppr(char *name, double dist);

/*
***ms18.c
*/
int     msupgp(WPGWIN *gwinpt);
int     msfixg(WPGWIN *gwinpt);
int     msspen(short pen);
int     msclin(WPGWIN *gwinpt, WPGRPT *pt1, WPGRPT *pt2);
bool    mscply(WPGWIN *gwinpt, short kmin, short *kmax, double x[], double y[], char a[]);
int     mscpmk(v2int win_id, int ix, int iy);
int     msepmk(v2int win_id);
int     msdpmk(WPGWIN *gwinpt, int ix, int iy, bool draw);

/*
***ms30.c
*/
short   msamod();
short   msomod();
short   mscoal();
WPEWIN *msgewp(HWND   win32_id);
int     msdlew(WPEWIN *ewinpt);

/*
***ms31.c
*/
short   mshlp1();
short   mshlp2();
short   mshlp3();
/*
***ms32.c
*/
short   mspl01();
int     msplot(VY *plotvy);

/*
***ms33.c
*/
int     msmrdb(char *filnam, bool create);
bool    msgrst(char *name, char *valpek);

/*
***ms34.c
*/
int     msargv(char *args, int *pargc, char *argv[]);
/*
***ms35.c
*/
int     msinsh(int wid, double zmin, double zmax, bool smooth);
int     msexsh();

/*
***ms36.c
*/
int     msdl10(WPRWIN *rwinpt);
int     msdl11(WPRWIN *rwinpt);
int     msdl12(WPRWIN *rwinpt);

/*
***ms37.c
*/
void    msprWC(WPRWIN *rwinpt, UINT wParam);

/*
***ms38.c
*/
void  mscbmm(WPRWIN *rwinpt, POINTS *ptold, POINTS *ptnew);
void  msvsline();
void  msvsfill();
void  msvslight(WPRWIN *rwinpt);