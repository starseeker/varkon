/*!******************************************************************/
/*  File: IG.h                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
***Macro for moving data in C memory.
*/
#ifdef UNIX
#define V3MOME(from,to,size) memcpy(to,from,size)
#endif

#ifdef WIN32
#define V3MOME(from,to,size) memcpy(to,from,size)
#endif

/*
***Some other common def's.
*/
#define PNLGTH    132      /* Max antal tecken i v�gbeskr. till plotprogram */
#define GMMXCO    50       /* Max antal referenser i en "composite" */
#define GMMXGP    100      /* Max antal referenser i en grupp */
#define GMMXXH    50       /* Max antal referenser i ett snitt */
#define MXLSTP    25       /* Max antal strippar i en LFT_SUR */
#define DSGANT    25       /* Allokeringsstorlek f�r dynamiska DBSeg */
#define V3SRCMAX  300      /* Max antal filer att kompilera */

#define MODEXT ".MBO"      /* Extension f�r modul-fil */
#define MBSEXT ".MBS"      /* Extension f�r MBS-fil */
#define RESEXT ".RES"      /* Extension f�r resultatfil */
#define RITEXT ".RIT"      /* Extension f�r ritningsfil */
#define JOBEXT ".JOB"      /* Extension f�r jobb-fil */
#define PIDEXT ".PID"      /* Extension f�r PID-fil */
#define PLTEXT ".PLT"      /* Extension f�r GKS-fil */
#define DXFEXT ".DXF"      /* Extension f�r DXF-fil */
#define SYMEXT ".PLT"      /* Extension f�r symbol-fil */
#define LSTEXT ".LST"      /* Extension f�r list-filer */
#define MDFEXT ".MDF"      /* Extension f�r meny-filer */
#define ERMEXT ".ERM"      /* Extension f�r ERM-filer */

#define AVBRYT (short)5    /* Kod f�r <CTRL>c */
#define WINDOW (short)-994 /* Kod f�r Window */
#define EREXIT -996        /* Kod f�r felslut */
#define EXIT   -997        /* Kod f�r ok-slut */
#define GOMAIN -998        /* Kod f�r direkt till huvudmenyn */
#define REJECT -999        /* Kod f�r avbruten operation */
#define IGUNDEF -1         /* igmtyp/igmatt "not defined" p� kom.rad */
/*
***Koder f�r meddelanderutinerna.
*/
#define IG_INP     1       /* Promt f�r inmatning */
#define IG_MESS    2       /* Meddelande */

/*
***Koder f�r v3mode.
*/
#define NONE_MOD   0        /* Funktionen ej implementerad */
#define TOP_MOD    1        /* V3:s Topp-niv� */
#define RIT_MOD    2        /* Ritmodulen */
#define BAS2_MOD   4        /* Basmodulen i 2D-mode */
#define BAS3_MOD   8        /* Basmodulen i 3D-mode */
#define BAS_MOD    12       /* Basmodulen allm�nt */
#define X11_MOD    16       /* Funktion bara f�r X11 */
#define NO_X11_MOD 32       /* Funktion ej f�r X11 */

/*
***MDF konstanter.
*/
#define CHRMAX 50000  /* maximalt antal tecken i samtliga t-str�ngar */
#define TXTMAX 2500   /* maximalt antal t-str�ngar */
#define SMBMAX 200    /* maximalt antal s-str�ngar */
#define MNUMAX 400    /* maximalt antal meny-nummer */
#define MALMAX 2000   /* maximalt antal alternativ (alla menyer) */

/*
***A menu alternative.
*/
typedef struct
{
char *str;         /* Alternative string */
char  acttyp;      /* Type of action */
int   actnum;      /* Action number */
} MNUALT;

/*
***A menu.
*/
typedef struct
{
char   *rubr;      /* Menu title */
MNUALT *alt;       /* Ptr to array of nalt alternatives */
int     nalt;      /* Number of alternatives */
} MNUDAT;

/*
***Macro's for math library.
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
***PI and it's fractions.
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
#define DGTORD   0.017453292519943294  /* Fr�n grader till radianer */
#define RDTODG  57.295779513082322     /* Fr�n radianer till grader */

/*
***Size of some system data areas.
*/
#define PMSIZE   400000   /* Default max size of module 400Kb */
#define RTSSIZE 1000000   /* Default max size of MBS variables 1Mb */
#define PLYMXV    50000   /* Max number of lines in a graphical polyline */
#define IGMAXID    5000   /* Max number of ID's in multiple selections */

/*
***MDF symboler.
*/
#define TSTR 't'      /* t-str�ng */
#define MENU 'm'      /* Meny */
#define MAIN_MENU 'M' /* Huvudmeny */
#define SYMBOL 's'    /* Symbol */
#define ALT  'a'      /* Alternativ */
#define PART 'p'      /* Part */
#define RUN  'r'      /* K�r namngiven modul */
#define CFUNC 'f'     /* C-funktion */
#define OLDMF 'F'     /* Gammal kod f�r det som nu �r Macro */
#define MFUNC 'M'     /* MBS-Macro */
#define STRING '"'    /* str�ngidentifierare */
#define EOR ';'       /* end of record identifierare */
#define SCOM '!'      /* start of comment identifierare */
#define ECOM '\n'     /* end of comment identifierare */
#define NUMASCII '%'  /* inleder ett num angivet ascii-tecken */
#define INCL '#'      /* Include av MDF-fil */
#define RECLIM 1024   /* st�rsta till�tna filrecord */
#define MENLEV 25     /* max antal menyniv�er */

/*
***Defines for input.
*/
#define MAXTXT      132         /* max ant tecken i en string */
#define MAXDTXT      23         /* max ant tecken i en double */
#define MAXLITXT     10         /* max ant tecken i en int */


/*
***Function prototypes for the IG API.
*/

/*
***varkon.c
*/
short igckjn(char jobnam[]);

/*
***IGinit.c
*/
short IGinit(char *fnam);
short IGlmdf(char *fnam);
short IGexfu(short mnum, short *palt);
short IGdofu(short atyp, short anum);
short IGatoc(char *p1, char *p2);
short IGstmu(short mnum, char *rubr, short nalt, char altstr[][V3STRLEN+1],
             char alttyp[], short altnum[]);
short IGsini();
char *IGgtts(int tnr);
short IGckhw();

short notimpl();
short wpunik();

/*
***igview.c
*/
short IGcnog();
short IGrenw();

/*
***igpoint.c
*/
short IGpofr();
short IGpopr();

/*
***igline.c
*/
short IGlifr();
short IGlipr();
short IGliof();
short IGlipv();
short IGlipt();
short IGli2t();
short IGlipe();

/*
***igarc.c
*/
short IGar1p();
short IGar2p();
short IGar3p();
short IGarof();
short IGarfl();
short IGcarr();
short IGcar1();
short IGcar2();

/*
***igcurve.c
*/
short IGcuis();
short IGcura();
short IGcurt();
short IGcusi();
short IGcuft();
short IGcuct();
short IGcuvt();
short IGcufn();
short IGcucn();
short IGcuvn();
short IGcucf();
short IGcucp();
short IGcomp();
short IGcuro();
short IGcuri();

/*
***igsurf.c
*/
short IGsuro();
short IGsuof();
short IGsucy();
short IGsusw();
short IGsuru();
short IGsurt();
short IGsura();
short IGsuco();
short IGsuex();
short IGsulo();

/*
***igpart.c
*/
short IGpart();
short IGrnmo();
short IGmfun();
short IGcall_part(char *filnam, short atyp);
short IGcall_macro(char *filnam);
short IGuppt();
short IGcptp();
short IGcptw();
short IGgnps(PMREFVA *id);

/*
***igtext.c
*/
short IGtext();
short IGctxv();
short IGctxs();

/*
***igcsy.c
*/
short IGcs3p();
short IGcs1p();
short IGmodb();
short IGmodg();
short IGmodl();
short IGupcs(DBptr la, int mode);

/*
***igtform.c
*/
short IGtfmo();
short IGtfro();
short IGtfmi();
short IGtcpy();
short IGmven();
short IGmv1();
short IGcpy1();
short IGmir1();
short IGrot1();

/*
***igbplane.c
*/
short IGbpln();

/*
***igdim.c
*/
short IGld0();
short IGld1();
short IGld2();
short IGcd0();
short IGcd1();
short IGcd2();
short IGrdim();
short IGadim();
short IGxht();

/*
***iggroup.c
*/
short IGgrp();
short IGchgr();

/*
***igsymb.c
*/
short IGsymb();

/*
***igsysdat.c
*/
short IGangm();
short IGanpm();
short IGlitb();
short IGrgmp();
short IGlsyd();

/*
***igdelete.c
*/
short IGdlen();
short IGdlls();
short IGdlgp();
short IGtrim();
short IGblnk();
short IGubal();
short IGhtal();

/*
***igexpr.c
*/
short IGcint(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short IGcinv(short pnr, char *istr, char *dstr, DBint  *ival);
short IGcflt(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short IGcflv(short pnr, char *istr, char *dstr, double *fval);
short IGcstr(short pnr, char *dstr, char *istr, pm_ptr *pexnpt);
short IGcstv(short pnr, char *istr, char *dstr, char   *strval);
short IGcstm(short mnum, pm_ptr *pexnpt);
short IGcref(short pnr, DBetype *ptyp, pm_ptr *pexnpt, bool *pe, bool *pr);
short IGcpos(short pnr, pm_ptr *pexnpt);
short IGcpov(DBVector *pos);
short IGgsid(DBId *idvek, DBetype *typ, bool *end, bool *right, short utstat);
short IGgmid(DBId idmat[][MXINIV], DBetype typv[], int *idant);

/*
***igstatem.c
*/
DBseqnum IGgnid();
short IGcges(char *typ, pm_ptr pplist);
short IGcprs(char *typ, pm_ptr pplist);
short IGedst();
short IGanrf();

/*
***igplot.c
*/
short IGcgkm();
short IGcdxf();
short IGshll();
short IGhid1();
short IGhid2();
short IGhid3();

/*
***igID.c
*/
short IGstid(char *idstr, DBId *idvek);
short IGidst(DBId *idvek, char *idstr);
bool  IGcsid(DBId *pid1, DBId *pid2);
bool  IGcmid(DBId *idpek, DBId idvek[][MXINIV], short vn);
short IGcpre(DBId *frompk, DBId *topk);

/*
***ighelp.c
*/
short IGhelp();

/*
***igmodule.c
*/
short IGevlp();
short IGramo();
short IGream();
short IGprtm();

/*
***igmenu.c
*/
bool  IGaamu(int mnum);
bool  IGsamu();
short IGgalt(MNUALT **paltp, short *ptyp);
short IGsmmu(int m);
int   IGgmmu();

/*
***igprompt.c
*/
short IGplma(char *s, int mode);
short IGptma(int tsnum, int mode);
short IGwlma(char *s, int mode);
short IGwtma(short tsnum);
short IGrsma();
char *IGqema();

/*
***iginput.c
*/
short IGssip(char *ps, char *is, char *ds, short ml);
short IGsfip(char *ps, DBfloat *fval);
short IGsiip(char *ps, int *ival) ;
short IGmsip(char *ps[], char *is[], char *ds[], short ml[], short as);
bool  IGialt(short psnum, short tsnum, short fsnum, bool  pip);
bool  IGials(char *ps, char *ts, char *fs, bool  pip);

/*
***igjob.c
*/
short IGgene();
short IGexpl();
short IGlsjb();
short IGdljb();
short IGmvrr();
short IGload();
short IGldmo();
short IGsjpg();
short IGsaln();
short IGspmn();
short IGsgmn();
short IGsjbn();
short IGcatt();
short IGexit();
short IGexsn();
short IGexsa();
short IGexsd();
short IGnjsd();
short IGnjsa();
short IGsjsa();
short IGnjsn();
short IGselj(char *newjob);
short IGchjn(char *newnam);
bool  IGgrst(char *resurs, char *pvalue);

/*
***igset.c
*/
short IGset_active_attribute(int attribute, char *value);
short IGslvl();
short IGspen();
short IGswdt();

/*
***igfile.c
*/
short IGfmov(char *from, char *to);
short IGfcpy(char *from, char *to);
short IGfapp(char *from, char *to);
short IGfdel(char *fil);
bool  IGfacc(char *fil, char mode);
bool  IGftst(char *fil);
short IGmkdr(char *dirnam);
bool  IGcmpw(char *wc_str, char *tststr);
short IGcmos(char oscmd[]);

/*
***igenvpath.c
*/
int   IGfopr(char *path, char *fil, char *ext);
void  IGtrfp(char *path1, char *path2);
char *IGgenv(int envkod);
char *IGenv3(char *envstr);

/*
***igattedit.c
*/
short IGcpen();
short IGcniv();
short IGcwdt();
short IGcdal();
short IGcfs();
short IGcfd();
short IGcfc();
short IGcff();
short IGctsz();
short IGctwd();
short IGctsl();
short IGctfn();
short IGctpm();
short IGcdts();
short IGcdas();
short IGcdnd();
short IGcda0();
short IGcda1();

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
short IGchpr();
short IGcnpr(char *newpid);
short IGselp(char *projekt);
short IGlspr();
short IGdlpr();
short IGldpf(char *filnam);
short IGdir(char *inpath, char *typ, int maxant, int maxsiz, char *pekarr[],
            char *strarr, int *nf);
short IGckpr();

/*
***igdebug.c
*/
/* See debug.h */

/********************************************************************/
