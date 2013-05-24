/*!******************************************************************/
/*  File: IG.h                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/********************************************************************/

#include <stdlib.h>
#include <string.h>

#ifdef UNIX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif


#define  V3EXOK 0

/*
***Microsofts 32-bitars Visual C++ har symbolen "WIN32" definierad.
***så det behöver inte göras här.
*/
#ifdef WIN32
#undef  NULL
#define NULL    (long)0
#endif

/*
***Environment.
*/
#include "env.h"

/*
***PM.
*/
#include "../../PM/include/PM.h"

/*
***Macro för att flytta data i primärminne.
*/
#ifdef UNIX
#define V3MOME(from,to,size) memcpy(to,from,size)
#endif

#ifdef WIN32
#define V3MOME(from,to,size) memcpy(to,from,size)
#endif

/*
***Allmänt förekommande definitioner.
*/
#define PNLGTH    80       /* Max antal tecken i vägbeskr. till plotprogram */
#define GMMXCO    50       /* Max antal referenser i en "composite" */
#define GMMXGP    100      /* Max antal referenser i en grupp */
#define GMMXXH    50       /* Max antal referenser i ett snitt */
#define MXLSTP    25       /* Max antal strippar i en LFT_SUR */
#define DSGANT    25       /* Allokeringsstorlek för dynamiska DBSeg */
#define GPMAXV    20       /* Max antal vyer */
#define GPVNLN    15       /* Max tecken i vynamn */
#define NT1SIZ    2000     /* Max antal nivåer */
#define NT2SIZ    100      /* Max antal namngivna nivåer */
#define NIVNLN    10       /* Max antal tecken i nivå-namn */
#define V3SRCMAX  300      /* Max antal filer att kompilera */

#define MODEXT ".MBO"      /* Extension för modul-fil */
#define MBSEXT ".MBS"      /* Extension för MBS-fil */
#define RESEXT ".RES"      /* Extension för resultatfil */
#define RITEXT ".RIT"      /* Extension för ritningsfil */
#define JOBEXT ".JOB"      /* Extension för jobb-fil */
#define PIDEXT ".PID"      /* Extension för PID-fil */
#define PLTEXT ".PLT"      /* Extension för GKS-fil */
#define DXFEXT ".DXF"      /* Extension för DXF-fil */
#define SYMEXT ".PLT"      /* Extension för symbol-fil */
#define LSTEXT ".LST"      /* Extension för list-filer */
#define MDFEXT ".MDF"      /* Extension för meny-filer */
#define ERMEXT ".ERM"      /* Extension för ERM-filer */

#define AVBRYT (short)5    /* Kod för <CTRL>c */
#define WINDOW (short)-994 /* Kod för Window */
#define POSMEN (short)-995 /* Kod för Pos-menyn */
#define EREXIT -996        /* Kod för felslut */
#define EXIT   -997        /* Kod för ok-slut */
#define GOMAIN -998        /* Kod för direkt till huvudmenyn */
#define REJECT -999        /* Kod för avbruten operation */
#define IGUNDEF -1         /* igmtyp/igmatt "not defined" på kom.rad */
/*
***Koder för meddelanderutinerna.
*/
#define IG_INP     1       /* Promt för inmatning */
#define IG_MESS    2       /* Meddelande */

/*
***Koder för v3mode.
*/
#define NONE_MOD   0        /* Funktionen ej implementerad */
#define TOP_MOD    1        /* V3:s Topp-nivå */
#define RIT_MOD    2        /* Ritmodulen */
#define BAS2_MOD   4        /* Basmodulen i 2D-mode */
#define BAS3_MOD   8        /* Basmodulen i 3D-mode */
#define BAS_MOD    12       /* Basmodulen allmänt */
#define X11_MOD    16       /* Funktion bara för X11 */
#define NO_X11_MOD 32       /* Funktion ej för X11 */

/*
***MDF konstanter.
*/
#define CHRMAX 25000  /* maximalt antal tecken i samtliga t-strängar */
#define TXTMAX 2000   /* maximalt antal t-strängar */
#define SMBMAX 200    /* maximalt antal s-strängar */
#define MNUMAX 400    /* maximalt antal meny-nummer */
#define MALMAX 2000   /* maximalt antal alternativ (alla menyer) */

/* MNUALT lagrar uppgifter om ett alternativ i en meny */
typedef struct mualt
    {
    char *str;        /* pekare till alt.-strängen */
    char acttyp;      /* typ av aktion - ny meny/funktion */
    short actnum;     /* meny/funktions/partsträng-nummer */
    } MNUALT;

/* MNUDAT lagrar uppgifter om en meny, 910227/JK */
typedef struct mnudat
    {
    char *rubr;       /* Rubrik */
    MNUALT *alt;      /* Alternativ */
    short wdth;       /* Maxbredd */
    short nalt;       /* Antal alternativ */
    } MNUDAT;

/*
***Koder för terminaler.
*/
#define LVT100      1        /* ABC1600 */
#define V550        2        /* Visual 550 */
#define IP3215      3        /* InterPro-32 15 tum */
#define MG400       4        /* Pericom 15 tum */
#define MG420       5        /* Pericom 20 tum */
#define FT4600      6        /* Facit Twist */
#define ALFA        7        /* Ej grafisk */
#define BATCH       7        /* Ingen terminal */
#define MG700       8        /* Pericom 17 tum */
#define T4207       9        /* Tektronix */
#define MO2000     10        /* Modgraf */
#define VT100      11        /* 24 raders VT100 */
#define N220G      12        /* Nokia VDU 220G */
#define CGI        13        /* SCO-XENIX CGI-Interface */
#define MX7250     14        /* Pericom 20 tum med färg */
#define MSCOLOUR   15        /* MSKERMIT på PC med färg */
#define MSMONO     16        /* MSKERMIT på monokrom PC */
#define X11        17        /* X-WINDOWS */
#define MSWIN      18        /* Microsoft WINDOWS-32 */

/*
***Följande matematiska funktioner skall användas.
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
***Konstanter för PI och delar därav. I Linux är PI och PI2
***definierade annorlunda.
*/
#ifdef PI
#undef PI
#endif

#ifdef PI2
#undef PI2
#endif

#define PI05     1.5707963267948966    /* PI/2   = 90  grader */
#define PI       3.1415926535897932    /* PI     = 180 grader */
#define PI15     4.7123889803846896    /* 3*PI/2 = 270 grader */
#define PI2      6.2831853071795862    /* 2*PI   = 360 grader */
#define DGTORD   0.017453292519943294  /* Från grader till radianer */
#define RDTODG  57.295779513082322     /* Från radianer till grader */

/*
***Koder för rit-mode.
*/
#define GEN      1           /* Generera vektorer */
#define CLIP     2           /* Klipp vektorerna */
#define DRAW     3           /* Rita till skärm och df */

/*
***Struktur för vyriktning.
*/
typedef struct vyvec
{
   double x_vy;
   double y_vy;
   double z_vy;
} VYVEC;

/*
***Struktur för vytransformation.
*/
typedef struct vymat
{
   double v11,v12,v13;
   double v21,v22,v23;
   double v31,v32,v33;
} VYMAT;

/*
***Struktur för vy fr.o.m. V1.11C.
*/
typedef struct vy
{
   char    vynamn [GPVNLN+1];/* Antal tecken +1 för '/0' */
   tbool   vy3d;             /* TRUE => 3D-vy */
   tbool   vytypp;           /* TRUE/FALSE => Position/Matris */
   VYVEC   vyrikt;           /* Vy-riktning */
   VYMAT   vymatr;           /* Vy-matris */
   double  vydist;           /* Betraktelseavstånd */
   double  vywin[4];         /* Modellfönster */
} VY;

/*
***Struktur för vy fr.o.m. V1.7E.
*/
typedef struct ovy111
{
   char    vynamn [GPVNLN+1];/* Antal tecken +1 för '/0' */
   tbool   vy3d;             /* TRUE => 3D-vy */
   VYVEC   vyrikt;           /* Vy-riktning */
   double  vydist;           /* Betraktelseavstånd */
   double  vywin[4];         /* Fönster */
} OVY111;

/*
***Gammal struktur för vy.
*/
typedef struct ovy17
{
   char    vynamn [GPVNLN+1];/* Antal tecken +1 för '/0' */
   VYVEC   vyrikt;           /* Vy-riktning */
   double  vywin[4];         /* Fönster */
} OVY17;

/*
*** Gammal def. av nivåer behålls för kompatibilitet
*/
#define NIVANT  100         /* Max antal nivåer */

typedef struct niv          /* Structure för en nivå */
{
   tbool vis;               /* TRUE om synlig */
   tbool def;               /* TRUE om definierad */
   char  nivnam[16];        /* Namn */
} NIV;

/*
*** Ny def av nivåer from. V1.2I 29/9/86.
*/
typedef struct nivnam        /* Structure för en nivå */
{
   short num;                /* Nivå-nr */
   char  nam[NIVNLN+1];      /* Nivå-namn */
} NIVNAM;

/*
***Diverse datorberoende maxstorlekar.
*/
#ifdef V3_DEMO

#define PMSIZE   50000 
#define RTSSIZE  10000
#define GMSMXV   1000
#define PLYMXV   6000
#define IGMAXID  1000

#else

#ifdef WIN32
#define PMSIZE   400000 
#define RTSSIZE  200000
#define GMSMXV   1000
#define PLYMXV   30000
#define IGMAXID  5000
#endif

#ifdef WRK_STAT
#define PMSIZE   400000 
#define RTSSIZE  200000
#define GMSMXV   1000
#define PLYMXV   30000
#define IGMAXID  5000
#endif

#endif

/*
***MDF symboler.
*/
#define TSTR 't'      /* t-sträng */
#define MENU 'm'      /* Meny */
#define MAIN_MENU 'M' /* Huvudmeny */
#define SYMBOL 's'    /* Symbol */
#define ALT  'a'      /* Alternativ */
#define PART 'p'      /* Part */
#define RUN  'r'      /* Kör namngiven modul */
#define FUNC 'f'      /* C-funktion */
#define OLDMF 'F'     /* Gammal kod för det som nu är Macro */
#define MFUNC 'M'     /* MBS-Macro */
#define STRING '"'    /* strängidentifierare */
#define EOR ';'       /* end of record identifierare */
#define SCOM '!'      /* start of comment identifierare */
#define ECOM '\n'     /* end of comment identifierare */
#define NUMASCII '%'  /* inleder ett num angivet ascii-tecken */
#define INCL '#'      /* Include av MDF-fil */
#define RECLIM 1024   /* största tillåtna filrecord */
#define MENLEV 25     /* max antal menynivåer */

/*
***Define för input-rutiner.
*/
#define MAXTXT      132         /* max ant tecken i en string */
#define MAXDTXT      23         /* max ant tecken i en double */
#define MAXLITXT     10         /* max ant tecken i en int */

/*
***Defs för iggtsm() som läser symboler/tecken från standard input.
*/ 
#define SMBNONE     -1          /* varken symbol eller tecken */
#define SMBCHAR      0          /* ingen symbol men ett tecken */
#define SMBRETURN    1          /* symbolen return */
#define SMBBACKSP    2          /* backning */
#define SMBDELETE    3          /* reserverad */
#define SMBLEFT      4          /* vänster */
#define SMBRIGHT     5          /* höger */
#define SMBUP        6          /* uppåt */
#define SMBDOWN      7          /* neråt */
#define SMBESCAPE    8          /* escape return */
#define SMBMAIN      9          /* direkt till huvudmenyn */
#define SMBHELP     10          /* hjälp */
#define SMBPOSM     11          /* positionsmenyn */
#define SMBALT      12          /* meny-alternativ */


/*
***Function prototypes for the IG API.
*/

/*
***varkon.c
*/
short igckjn(char jobnam[]);

/*
***iginit.c
*/
short iginit(char *fnam);
short iglmdf(char *fnam);
short igexit();
short igexfu(short mnum, short *palt);
short igdofu(short atyp, short anum);
short notimpl();
short wpunik();
short editcopy(char *p1, char *p2);
short igstmu(short mnum, char *rubr, short nalt, char altstr[][V3STRLEN+1],
             char alttyp[], short altnum[]);
short igsini();

/*
***ig2.c
*/
char *iggtts(short tnr);
void igflsh();
void igbell();
short iggtsm(char *cp, MNUALT **altptr);
short igglin(char *pmt, char *dstr, short *ntkn, char *istr);

/*
***igview.c
*/
short igcrvp();
short igcrvc();
short dlview();

short igcnog();

short igcror();
short igcrdx();
short igcrdy();
short igtndr();
short igslkr();

short igshd0();
short igshd1();
short igrenw();

/*
***igpoint.c
*/
short pofrpm();
short poprpm();

/*
***igline.c
*/
short lifrpm();
short liprpm();
short liofpm();
short lipvpm();
short liptpm();
short li2tpm();
short lipepm();

/*
***igarc.c
*/
short ar1ppm();
short ar2ppm();
short ar3ppm();
short arofpm();
short arflpm();
short igcarr();
short igcar1();
short igcar2();

/*
***igcurve.c
*/
short cuispm();
short curapm();
short curtpm();
short cusipm();
short cuftpm();
short cuctpm();
short cuvtpm();
short cufnpm();
short cucnpm();
short cuvnpm();
short cucfpm();
short cucppm();
short comppm();
short curopm();
short curipm();

/*
***igsurf.c
*/
short suropm();
short suofpm();
short sucypm();
short suswpm();
short surupm();
short surtpm();
short surapm();
short sucopm();
short suexpm();
short sulopm();

/*
***igpart.c
*/
short partpm();
short igcpts(char *filnam, short atyp);
bool  igoptp(char *prompt);
bool  igmenp(char *prompt, short *mnum);
bool  igposp(char *prompt, short *posalt);
bool  igtypp(char *prompt, DBetype *typmsk);
short igdefp(char *prompt, PMLITVA *defval);
short igtstp(char *prompt);
short iguppt();
short igcptp();
short igcptw();
short iggnps(PMREFVA *id);

/*
***igtext.c
*/
short textpm();
short igctxv();
short igctxs();

/*
***igcsy.c
*/
short cs3ppm();
short cs1ppm();
short modbpm();
short modgpm();
short modlpm();
short igupcs(DBptr la, int mode);

/*
***igtform.c
*/
short tfmopm();
short tfropm();
short tfmipm();
short tcpypm();

/*
***igbplane.c
*/
short bplnpm();

/*
***igdim.c
*/
short ld0pm();
short ld1pm();
short ld2pm();
short cd0pm();
short cd1pm();
short cd2pm();
short rdimpm();
short adimpm();
short xhtpm();

/*
***igsysdat.c
*/
short igangm();
short iganpm();
short lstitb();
short rdgmpk();
short lstsyd();

/*
***igdelete.c
*/
short igdlen();
short igdlls();
short igdlgp();
short trimpm();
short igblnk();
short igubal();
short ightal();

/*
***igexpr.c
*/
short genint(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short geninv(short pnr, char *istr, char *dstr, DBint  *ival);
short genflt(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short genflv(short pnr, char *istr, char *dstr, double *fval);
short genstr(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short genstv(short pnr, char *istr, char *dstr, char   *strval);
short genstm(short mnum, pm_ptr *pexnpt);
short genref(short pnr, DBetype *ptyp, pm_ptr *pexnpt, bool *pe, bool *pr);
short genpos(short pnr, pm_ptr *pexnpt);
short genpov(DBVector *pos);
short igpmon();
short igpmof();
short getidt(DBId *idvek, DBetype *typ, bool *end, bool *right, short utstat);
short getmid(DBId idmat[][MXINIV], DBetype typv[], short *idant);
short gtpcrh(DBVector *pos);
short gtpend(DBVector *vecptr);
short gtpon(DBVector *vecptr);
short gtpint(DBVector *vecptr);
short gtpcen(DBVector *vecptr);
short gtpdig(DBVector *vecptr);

/*
***igstatem.c
*/
DBseqnum iggnid();
short igcges(char *typ, pm_ptr pplist);
short igcprs(char *typ, pm_ptr pplist);
short igedst();
short iganrf();

/*
***igplot.c
*/
short igplot();
short igcgkm();
short igcdxf();
short igshll();
short ighid1();
short ighid2();
short ighid3();

/*
***igID.c
*/
short igstid(char *idstr, DBId *idvek);
short igidst(DBId *idvek, char *idstr);
bool  igcsid(DBId *pid1, DBId *pid2);
bool  igcmid(DBId *idpek, DBId idvek[][MXINIV], short vn);
short igcpre(DBId *frompk, DBId *topk);

/*
***ig16.c
*/
short grppm();
short chgrgm();
short symbpm();

/*
***ighelp.c
*/
short ighelp();

/*
***igmodule.c
*/
short crepar();
short chapar();
short lstpar();
short igevpl();
short igramo();
short igream();
short igrnmo();
short igmfun();
short prtmod();
short lstmod();

/*
***ig20.c
*/
void iggnsa();
void igupsa();
bool igaamu(short mnum);
bool igsamu();
short iggalt(MNUALT **paltp, short *ptyp);
short igupmu();
short igpamu(short x, short y, short mnum);
short igsmmu(short m);
short iggmmu();
short igrsla();
short igplma(char *s, int mode);
short igptma(int tsnum, int mode);
short igwlma(char *s, int mode);
short igwtma(short tsnum);
short igrsma();
char *igqema();

/*
***ig21.c
*/
short igssip(char *ps, char *is, char *ds, short ml);
short igsfip(char *ps, DBfloat *fval);
short igsiip(char *ps, int *ival) ;
short igmsip(char *ps[], char *is[], char *ds[], short ml[], short as);
bool  igialt(short psnum, short tsnum, short fsnum, bool  pip);
bool  igials(char *ps, char *ts, char *fs, bool  pip);

/*
***igjob.c
*/
short igmain();
short irmain();
short iglsjb();
short igdljb();
short igmvrr();
short igload();
short igldmo();
short igsjpg();
short igsaln();
short igspmn();
short igsgmn();
short igsjbn();
short igcatt();
short igcmpc();
short v3exit();
short igexsn();
short igexsa();
short igexsd();
short ignjsd();
short ignjsa();
short igsjsa();
short ignjsn();
short igselj(char *newjob);
short igchjn(char *newnam);
bool  iggrst(char *resurs, char *pvalue);

/*
***igset.c
*/
short igslvl();
short igspen();
short igslfs();
short igslfd();
short igslfc();
short igsafs();
short igsafd();
short igsafc();
short igscfs();
short igscfd();
short igscfc();
short igsldl();
short igsadl();
short igscdl();
short igstsz();
short igstwd();
short igstsl();
short igstfn();
short igstpm();
short igsdts();
short igsdas();
short igsdnd();
short igsda0();
short igsda1();
short igsxfs();
short igsxfd();
short igsxfc();
short igsxdl();
short igswdt();

/*
***ig25.c
*/
short mvengm();
short mv1gm();
short cpengm();
short mrengm();
short roengm();

/*
***ig27.c
*/
short igcmos();

/*
***igfile.c
*/
short v3fmov(char *from, char *to);
short v3fcpy(char *from, char *to);
short v3fapp(char *from, char *to);
short v3fdel(char *fil);
bool  v3facc(char *fil, char mode);
bool  v3ftst(char *fil);
short v3mkdr(char *dirnam);
short igcmos(char oscmd[]);
bool  v3cmpw(char *wc_str, char *tststr);

/*
***ig29.c
*/
int   v3fopr(char *path, char *fil, char *ext);
void  v3trfp(char *path1, char *path2);
short igckhw();
short igckdl(int yl, int ml, int dl);
short v3dksn(unsigned long crypt);
char *v3genv(int envkod);
char *gtenv3(char *envstr);

/*
***igattedit.c
*/
short igcpen();
short igcniv();
short igcwdt();
short igcdal();
short igcfs();
short igcfd();
short igcfc();
short igcff();
short igctsz();
short igctwd();
short igctsl();
short igctfn();
short igctpm();
short igcdts();
short igcdas();
short igcdnd();
short igcda0();
short igcda1();

/*
***igerror.c
*/
short erinit();
short erpush(char *code, char *str);
short errmes();
short erlerr();

/*
***igmalloc.c
*/
void  v3mini();
void *v3mall(unsigned size, char *name);
void *v3rall(void *ptr, unsigned size, char *name);
short v3free(void *ptr, char *name);
void  v3msta();

/*
***igPID.c
*/
short igchpr();
short igcnpr(char *newpid);
short igselp(char *projekt);
short iglspr();
short igdlpr();
short igldpf(char *filnam);
short igdir(char *inpath, char *typ, int maxant, int maxsiz, char *pekarr[],
            char *strarr, int *nf);
short igckpr();

/*
***igdebug.c
*/
/* See debug.h */

/********************************************************************/
