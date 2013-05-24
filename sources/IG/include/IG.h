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
#define PNLGTH    132      /* Max antal tecken i vï¿½gbeskr. till plotprogram */
#define GMMXCO    50       /* Max antal referenser i en "composite" */
#define GMMXGP    100      /* Max antal referenser i en grupp */
#define GMMXXH    50       /* Max antal referenser i ett snitt */
#define MXLSTP    25       /* Max antal strippar i en LFT_SUR */
#define DSGANT    25       /* Allokeringsstorlek fï¿½r dynamiska DBSeg */
#define V3SRCMAX  300      /* Max antal filer att kompilera */

#define MODEXT ".MBO"      /* Extension for modul files */
#define MBSEXT ".MBS"      /* Extension for MBS source files */
#define RESEXT ".RES"      /* Extension for resultfiles */
#define JOBEXT ".JOB"      /* Extension for job files */
#define PIDEXT ".PID"      /* Extension for PID files */
#define PLTEXT ".PLT"      /* Extension for GKS files */
#define DXFEXT ".DXF"      /* Extension for DXF files */
#define SYMEXT ".PLT"      /* Extension for symbol files */
#define LSTEXT ".LST"      /* Extension for list files */
#define MDFEXT ".MDF"      /* Extension for menu definition files */
#define ERMEXT ".ERM"      /* Extension for error message files */

#define AVBRYT (short)5    /* <CTRL>c */
#define WINDOW (short)-994 /* Window */
#define EREXIT -996        /* Error exit */
#define EXIT   -997        /* Normal exit */
#define GOMAIN -998        /* Back to main menu */
#define REJECT -999        /* Operation cancelled */
/*
***Koder för meddelanderutinerna.
*/
#define IG_INP     1       /* Promt fï¿½r inmatning */
#define IG_MESS    2       /* Meddelande */

/*
***System/function modes (sysmode).
*/
#define TOP_MOD    1        /* System top level */
#define EXPLICIT   2        /* Explicit mode */
#define GENERIC    4        /* Generic mode */

/*
***Constants related to MDF-files.
*/
#define CHRMAX 50000  /* max number of chars in all t-strings */
#define TXTMAX 2500   /* max number of t-strings */
#define SMBMAX 200    /* max number of s-strings */
#define MNUMAX 400    /* highest menu number */
#define MALMAX 2000   /* max number of menu alternatives */

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
#define DGTORD   0.017453292519943294  /* Frï¿½n grader till radianer */
#define RDTODG  57.295779513082322     /* Frï¿½n radianer till grader */

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
#define TSTR 't'      /* t-strï¿½ng */
#define MENU 'm'      /* Meny */
#define MAIN_MENU 'M' /* Huvudmeny */
#define SYMBOL 's'    /* Symbol */
#define ALT  'a'      /* Alternativ */
#define PART 'p'      /* Part */
#define RUN  'r'      /* Kï¿½r namngiven modul */
#define CFUNC 'f'     /* C-funktion */
#define OLDMF 'F'     /* Gammal kod fï¿½r det som nu ï¿½r Macro */
#define MFUNC 'M'     /* MBS-Macro */
#define STRING '"'    /* strï¿½ngidentifierare */
#define EOR ';'       /* end of record identifierare */
#define SCOM '!'      /* start of comment identifierare */
#define ECOM '\n'     /* end of comment identifierare */
#define NUMASCII '%'  /* inleder ett num angivet ascii-tecken */
#define INCL '#'      /* Include av MDF-fil */
#define RECLIM 1024   /* stï¿½rsta tillï¿½tna filrecord */
#define MENLEV 25     /* max antal menynivï¿½er */

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
short IGcheck_jobname(char jobnam[]);

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

/*
***igview.c
*/
short IGset_cacc();
short IGcreate_gwin();
short IGcreate_rwin();

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
short IGrun_named();
short IGmacro();
short IGcall_part(char *filnam, short atyp);
short IGcall_macro(char *filnam);
short IGuppt();
short IGcptp();
short IGcptw();
short IGedit_MBS();
short IGcompile_all();
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
short IGcsy_3p();
short IGcsy_1p();
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
short IGandb();
short IGanpm();
short IGlitb();
short IGrgmp();
short IGlsyd();

/*
***igdelete.c
*/
short IGdelete_entity();
short IGdelete_group();
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
short IGexport_GKSM();
short IGexport_DXF2D();
short IGshell();
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
short IGabout_help();

/*
***igmodule.c
*/
short IGevlp();
short IGrun_active();
short IGream();
short IGedit_active();
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
short IGssip(char *prompt, char *ps, char *is, char *ds, short ml);
short IGsfip(char *ps, DBfloat *fval);
short IGsiip(char *ps, int *ival) ;
short IGmsip(char *prompt, char *ps[], char *is[], char *ds[], short ml[], short as);
bool  IGialt(short psnum, short tsnum, short fsnum, bool  pip);
bool  IGials(char *ps, char *ts, char *fs, bool  pip);

/*
***igjob.c
*/
short IGgeneric();
short IGexplicit();
short IGdljb();
short IGload();
short IGldmo();
short IGsjpg();
short IGsave_all_as();
short IGsave_MBS_as();
short IGsave_MBO_as();
short IGsave_RES_as();
short IGsave_JOB_as();
short IGcatt();
short IGexit();
short IGexit_sn();
short IGexit_sa();
short IGexsd();
short IGnjsd();
short IGnjsa();
short IGsave_all();
short IGnjsn();
short IGselect_job(char *newjob);
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

/*
***igattedit.c
*/
short IGset_actatts();
short IGcpen();
short IGclevel();
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
short IGdir(char *inpath, char *typ, int maxant, int maxsiz, char *pekarr[],
            char *strarr, int *nf);

/*
***igdebug.c
*/
/* See debug.h */

/********************************************************************/
