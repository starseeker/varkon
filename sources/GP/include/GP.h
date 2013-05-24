/**********************************************************************
*
*    GP.h
*    ====
*
*    This file is part of the VARKON Graphics Library.
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

#ifdef UNIX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#define SUDDAD  -1           /* Suddad post i displayfil */
#define MAXTXT   132         /* Max textlängd */
#define STOL   0.0           /* Min strecklängd hos linje och cirkelbåge */
#define DOTSIZ 1.0           /* Storleken hos en punkt i mm */

/*
***Koder för markeringar.
*/

#define PMKMAX     50        /* Max antal pek-märken */
#define NOMARK    -1         /* Inget */
#define KRYSS      1         /* Punkt */
#define DIAMANT    2         /* Highlight */
#define PLUS       3         /* Pekmärke */
#define DOT        4         /* Grid-punkt */

/*
***Koder för måttsättningstyp.
*/

#define LDHORIZON  0         /* Längdmått horisontell */ 
#define LDVERTIC   1         /* Längdmått vertikal */ 
#define LDPARALL   2         /* Längdmått parallel */ 
#define CDHORIZON  0         /* Diametermått horisontell */ 
#define CDVERTIC   1         /* Diametermått vertikal */ 
#define CDPARALL   2         /* Diametermått parallel */ 

/*
***Koder för linjetyp.
*/

#define SOLIDLN  0           /* Heldragen */ 
#define DASHLN   1           /* Streckad */ 
#define DADOLN   2           /* Punkstreckad */ 
#define DOTLN    3           /* Punktad */ 

/*
***Koder för cirkeltyp
*/

#define SOLIDARC 0           /* Heldragen */ 
#define DASHARC  1           /* Streckad */ 
#define DADOARC  2           /* Punkstreckad */ 
#define DASHRATIO  0.25      /* Mellanrum/strecklängd */ 
#define DOTRATIO   5.0       /* Mellanrum/strecklängd */ 

/*
***Koder för "hihlight" markeringar (hdr.hili).
*/

#define HILIINVISIBLE    0      /* Inget */
#define HILIDIAMOND      1      /* Diamant */

/*
***GKS metafil definitioner.
*/

#define MAXMETA  1000        /* Max ant tecken i en metafilpost */

#define TYPLEN   4           /* Typfältets längd */
#define DATLEN   6           /* Datafältets längd */
#define INTLEN   6           /* Ett heltalsfälts längd */
#define FLTLEN   10          /* Ett flyttalsfälts längd */

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
    int datlen;              /* Datafältets längd */
    int typlen;              /* Typfältets längd */
    int fltlen;              /* Ett flyttalsfälts längd */
    int intlen;              /* Ett heltalsfälts längd */
    char formtyp[ 16 ];      /* Formatsträng för typfält */
    char formdat[ 16 ];      /* Formatsträng för datafält */
    char formint[ 16 ];      /* Formatsträng för heltalsfält */
    char formflt[ 16 ];      /* Formatsträng för flyttalsfält */
} METADEF;

/*
***Definitioner för terminal-egenskaper.
*/

typedef struct gptatt
{
  char   to4010[10];         /* Växla till tek-mode */
  char   toansi[10];         /* Växla till VT100-mode */
  short  scorgx;             /* Origo x */
  short  scorgy;             /* Origo y */
  double scsizx;             /* Skärmstorlek i x-led */
  double scsizy;             /* Skärmstorlek i y-led */
  short  npixx;              /* Antal pxels i x-led */
  short  npixy;              /* Antal pxels i y-led */
  char   pen0[10];           /* Växla till penna 0 */
  char   pen1[10];           /* Växla till penna 1 */
} GPTATT;

/*
***Defintioner för hide och shade.
*/

#define MAXPBL  1000               /* Max antal minnesblock för Plan-data */
#define PBLSIZ  100*sizeof(GPBPL)  /* Blockens storlek i bytes < 32K */
#define MAXPLN 100000              /* Max antal plan = PBLSIZ/sizeof(GPBPL) */
                                   /* gånger max antal block */

#define DTOL   1E-10               /* Ett litet avstånd */
#define SPLMAX   100               /* Max antal split-delar */

#define SYNLIG     0               /* Kod för synlig vektor */
#define OSYNLIG    1               /* Kod för osynlig vektor */
#define SPLIT2     2               /* Kod för 2D-delad vektor */
#define SPLIT3     3               /* Kod för 3D-delad vektor */
#define KLIPPT     4               /* Kod för klippt vektor */
#define UTANFR     0               /* Pos 2D-utanför plan */
#define INUTI      1               /* Pos 2D-inuti plan */
#define HITOM      1               /* Kod för 3D-hitom */
#define BAKOM      0               /* Kod för 3D-bakom */
#define NOEDGE     0               /* Ingen rand */
#define EDGE1      1               /* Rand 1 */
#define EDGE2      2               /* Rand 2 */
#define EDGE3      3               /* Rand 3 */
#define EDGE4      4               /* Rand 4 */

typedef struct gpbpl
{
DBVector  p1,p2,p3,p4,nv;
double dx1,dx2,dx3,dx4;
double dy1,dy2,dy3,dy4;
double l1,l2,l3,l4;
double k1,k2,k3,k4;
double xmin,xmax,ymin,ymax,zmax;
short  pen;
tbool blank;
tbool triangle;
} GPBPL;

/*
***Function prototypes for the public GP API.
*/

/*
***gp1.c
*/
short gpinit(char *term);
short gpexit();
short gpstvi(VY *pnewvy);
short gpsvpt(double vpt[]);
short gpsgsz(double dxmm, double dymm);
short gpgvpt(double vpt[]);
short gpswin(VY *pwin);
short gpgwin(VY *pwin);
short gpfwin(VY *pwin);
short gpgovy(VY *pvy);
short gpstcn(double newcn);
short gpgtcn(double *cnpek);
short gpgtpn();
short gpsrsk(double skala);
short gpgrsk(double *pskala);

/*
***gp2.c
*/
short gpdrpo(GMPOI *poipek, DBptr la, short drmod);
short gpdlpo(GMPOI *poipek, DBptr la);
short plydot(GMPOI *poipek, int *n, double x[], double y[], char a[]);

/*
***gp3.c
*/
short gp4010();
short gpansi();
short gpflsh();
short gpersc();
short gperal();
short gperdf();
short gpdram();
short gpmvsc(short ix, short iy);
short gpdwsc(short ix, short iy);
short gpgtmc(char *pektkn, double *px, double *py, bool mark);
short gpgtsc(char *pektkn, short *pix, short *piy, bool mark);
short gpdpmk(short ix, short iy);
short gpepmk();
short gpdmrk(short ix, short iy, short typ, bool draw);
short gpspen(short pen);
short gpsbrush(short pen);
short gpswdt(double width);

/*
***gp4.c
*/
short gppltr(DBVector *p, double *x, double *y, double *z);
short gppstr(double x[], double y[], double z[]);
short gptrpv(DBVector *p);

/*
***gp5.c
*/
short gpdrar(GMARC *arcpek, GMSEG *segmnt, DBptr la, short drmod);
short gpdlar(GMARC *arcpek, GMSEG *segmnt, DBptr la);
short gpplar(GMARC *arcpek, GMSEG *segmnt, int *n, double x[], double y[],
             char a[]);

/*
***gp6.c
*/
short gpdrtx(GMTXT *txtpek, char *strpek, DBptr la, short drmod);
short gpdltx(GMTXT *txtpek, char *strpek, DBptr la);
short gppltx(GMTXT *txtpek, unsigned char *strpek, int *k, double x[],
             double y[], double z[], char a[]);
short gpprtx(GMTXT *txtpek, double x[], double y[], double z[], int npts);
short gpinfn();
short gpldfn(int font, char *filnam);
short gpexfn();

/*
***gp8.c
*/
short gpdrcu(GMCUR *curpek, GMSEG *segmnt, DBptr la, short drmod);
short gpdlcu(GMCUR *curpek, GMSEG *segmnt, DBptr la);
short gpplcu(GMCUR *curpek, GMSEG *segmnt, int *n, double x[], double y[],
             double z[], char a[]);

/*
***gp9.c
*/
short gpdrld(GMLDM *dimpek, DBptr la, short drmod);
short gpdlld(GMLDM *dimpek, DBptr la);
short gpplld(GMLDM *dimpek, int *n, double x[], double y[], char typ[]);

/*
***gp10.c
*/
short gpdrli(GMLIN *linpek, DBptr la, short drmod);
short gpdlli(GMLIN *linpek, DBptr la);
short gpprli(GMLIN *linpek);
short gpplli(GMLIN *linpek, int *n, double x[], double y[], char a[]);

/*
***gp11.c
*/
bool klpply(int kmin, int *kmax, double x[], double y[], char a[]);
bool klplin(int k, double x[], double y[], char a[]);
bool klpdot(int k, double x[], double y[]);
short klp(double *v, double *w);
short klptst(double *v, double *w, double *t1, double *t2);
short klpplq(int kmin, int *kmax, double x[], double y[]);

/*
***gp12.c
*/
short gpdrcs(GMCSY *csypek, DBTmat *pmat, DBptr la, short drmod);
short gpdlcs(GMCSY *csypek, DBTmat *pmat, DBptr la);
short gpupcs(GMCSY *csypek, DBTmat *pmat, DBptr la, int mode);
short gpplcs(GMCSY *csypek, DBTmat *pmat, int *n, double x[], double y[],
             char a[]);

/*
***gp13.c
*/
bool fndobj(DBptr la, DBetype typmsk, DBetype *typ);
bool stoply(int k, double x[], double y[], char a[], DBptr la, DBetype tp);
bool remobj();
bool drwobj(bool s);
DBptr gpgtla(DBetype typmsk, short ix, short iy, DBetype *typ, bool *ends,
             bool *right);
short gpgmla(short ix1, short iy1, short ix2, short iy2, short mode,
             bool hl, short *nla, DBetype typvek[], DBptr lavek[]);
bool drwply(int k, double x[], double y[], char a[], bool s);
bool fillply(int k, double x[], double y[], short pen);
short gphgen(DBptr la, DBint mrktyp);
short gphgal(DBint mrktyp);
short gperhg();

/*
***gp14.c
*/
short gpmeta(FILE *filpek, VY *plotvy, DBVector *origo);
short methdr(METADEF *mdp, FILE *filpek, char metarec[]);
short metwin(METADEF *mdp, FILE *filpek, char metarec[], VY *plotvy,
             DBVector *origo);
short metply(METADEF *mdp, FILE *filpek, int k, double x[], double y[],
             char a[], VY *plotvy, char metarec[]);
short metfill(METADEF *mdp, FILE *filpek, int k, double x[], double y[],
              VY *plotvy);
short metclr(METADEF *mdp, FILE *filpek, char metarec[]);
short metpen(METADEF *mdp, FILE *filpek, short pen, char metarec[]);
short metwdt(METADEF *mdp, FILE *filpek, double wdt, char metarec[]);
short metend(METADEF *mdp, FILE *filpek, char metarec[]);

/*
***gp15.c
*/
short gpdrcd(GMCDM *dimpek, DBptr la, short drmod);
short gpdlcd(GMCDM *dimpek, DBptr la);
short gpplcd(GMCDM *dimpek, int *n, double x[], double y[], char typ[]);

/*
***gp16.c
*/
short gpdrsu(GMSUR *surpek, GMSEG *sptarr[], DBptr la, short drmod);
short gpdlsu(GMSUR *surpek, GMSEG *sptarr[], DBptr la);
short gpplsu(GMSUR *surpek, GMSEG *sptarr[], int *n, double x[],
             double y[], double z[], char a[]);

/*
***gp17.c
*/
short gpdrxh(GMXHT *xhtpek, gmflt xyvek[], DBptr la, short drmod);
short gpdlxh(GMXHT *xhtpek, gmflt xyvek[], DBptr la);

/*
***gp18.c
*/
short gpdrrd(GMRDM *dimpek, DBptr la, short drmod);
short gpdlrd(GMRDM *dimpek, DBptr la);
short gpplrd(GMRDM *dimpek, GMTXT *txtpek, int *n, double x[],
             double y[], char typ[]);

/*
***gp19.c
*/
short gpdrad(GMADM *dimpek, DBptr la, short drmod);
short gpdlad(GMADM *dimpek, DBptr la);
short gpplad(GMADM *dimpek, GMTXT *txtpek, int *n, double x[], double y[],
             char typ[]);

/*
***gp20.c
*/
short gpdrrs(gmflt ox, gmflt oy, gmflt dx, gmflt dy);
short gpdlrs(gmflt ox, gmflt oy, gmflt dx, gmflt dy);
short drwrs( gmflt ox, gmflt oy, gmflt dx, gmflt dy);

/*
***gp22.c
*/
char  *gpitoa(char *buf, short n);
double gpaton(char *pos);

/*
***gp23.c
*/
short gpdrbp(GMBPL *bplpek, DBptr la, short drmod);
short gpdlbp(GMBPL *bplpek, DBptr la);
short gpplbp(GMBPL *bplpek, int *n);

/*
***gp24.c
*/
short gphide(bool flag1, bool flag2, FILE *pfil, DBVector *origo);
short gpdrhd();
void  hidply();

/*
***gp25.c
*/
short gpgnpd();

/*
***gp26.c
*/
short gpcl2d(double *xv, double *yv, GPBPL *pl);
void  gpsvp2(GPBPL *pl, int end);
void  gpspl1(GPBPL *pl, int end);
short gpspl2(GPBPL *pl);
void  gpsvp3(GPBPL *pl, DBVector *ps);
void  gpspl3(DBVector *ps, int sida1);
void  gpsvp4(GPBPL *plr, GPBPL *plt, DBVector *ps);

/*
***gp27.c
*/
short dxfout(FILE *filpek, VY *plotvy, DBVector *origo);
short dxfopo(FILE *filpek, GMPOI *poipek, DBVector *origo);
short dxfoli(FILE *filpek, GMLIN *linpek, DBVector *origo);
short dxfoar(FILE *filpek, GMARC *arcpek, GMSEG *seg, DBVector *origo);
short dxfocu(FILE *filpek, GMCUR *curpek, GMSEG *seg, DBVector *origo);
short dxfocs(FILE *filpek, GMCSY *csypek, DBTmat *pmat, DBVector *origo);
short dxfobp(FILE *filpek, GMBPL *bplpek, DBVector *origo);
short dxfoms(FILE *filpek, DBMesh *mshpek, DBVector *origo);
short dxfotx(FILE *filpek, GMTXT *txtpek, char str[], DBVector *origo);
short dxfold(FILE *filpek, GMLDM *ldmpek, DBVector *origo);
short dxfocd(FILE *filpek, GMCDM *cdmpek, DBVector *origo);
short dxford(FILE *filpek, GMRDM *rdmpek, DBVector *origo);
short dxfoad(FILE *filpek, GMADM *admpek, DBVector *origo);
short dxfoxh(FILE *filpek, GMXHT *xhtpek, gmflt crd[], DBVector *origo);
short dxfopl(FILE *filpek, GMRECH *hedpek, int n, double x[], double y[],
             char a[], DBVector *origo);
short dxfniv(FILE *filpek, short niv);
short dxfpen(FILE *filpek, short pen);
short dxfwdt(FILE *filpek, double wdt);
short dxffnt(FILE *filpek, short typ, gmflt lgt);

/*
***gp28.c
*/
short plycud(GMCUR *gmpost, GMSEG *segmnt, int *n, double x[], double y[],
             double z[],char a[]);

/*
***gp29.c
*/
short gpdrms(DBMesh *mshptr, DBptr la, short drmod);
short gpdlms(DBMesh *mshptr, DBptr la);
short gpplms(DBMesh *mshptr, double *x, double *y, double *z, char *a, int *n);

