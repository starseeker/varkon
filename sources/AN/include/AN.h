/*!******************************************************************/
/*  File: AN.h                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  Definitions for the MBS analyser.                               */
/*                                                                  */
/*  This file is part of the VARKON Analyzer Library.               */
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
/*  (C)Microform AB 1984-2000, J.Kjellander,  johan@microform.se    */
/*                                                                  */
/********************************************************************/

/*
 * Constants
 */

#define ANSYNLEN 31          /* Maximum length of identifiers        */
#define ANSTRLEN V3STRLEN    /* Maximum length of string literals    */
#define ANMAXDIG 17          /* Maximum number of significant digits */
#define ANMAXEXP 36          /* Maximum exponent value               */
#define ANMAXINT 2147483647  /* Maximal value of an integer          */
#define ANMAXREF 2147483647  /* Maximum value of a reference literal */

/*
 * Symbols. Max 64 different without changing ANFSET.
 */

typedef short ANSYTYP;  /* Symbol type */
#define ANSYEOF  1      /* End Of File */
#define ANSYID   2      /* Identifier  */
#define ANSYSTRL 3      /* String Literal */
#define ANSYINTL 4      /* Integer Literal */
#define ANSYDECL 5      /* Decimal Literal */
#define ANSYREFL 6	/* Reference Literal (#int Ä.intÅ) */
#define ANSYDOT  7      /* '.' */
#define ANSYCOM  8      /* ',' */
#define ANSYSEMI 9      /* ';' */
#define ANSYCOL  10     /* ':' */
#define ANSYLPAR 11     /* '(' */
#define ANSYRPAR 12     /* ')' */
#define ANSYMUL  15     /* '*' */
#define ANSYDIV  16     /* '/' */
#define ANSYPLUS 17     /* '+' */
#define ANSYMINS 18     /* '-' */
#define ANSYEXP  19     /* '**' */
#define ANSYASGN 20     /* ':=' */
#define ANSYEQ   21     /* '=' */
#define ANSYNEQ  22     /* '<>' */
#define ANSYLT   23     /* '<' */
#define ANSYGT   24     /* '>' */
#define ANSYLE   25     /* '<=' */
#define ANSYGE   26     /* '>=' */
#define ANSYMOD  27     /* 'MODULE' */
#define ANSYGEO  28     /* 'GEOMETRY' */
#define ANSYDRAW 29     /* 'DRAWING' */
#define ANSYLOCL 30     /* 'LOCAL' */
#define ANSYGLOB 31     /* 'GLOBAL' */
#define ANSYBMOD 32     /* 'BEGINMODULE' */
#define ANSYEMOD 33     /* 'ENDMODULE' */
#define ANSYVAR  34     /* 'VAR' */
#define ANSYSTR  35     /* 'STRING' */
#define ANSYFILE 36     /* 'FILE' */
#define ANSYINT  37     /* 'INT' */
#define ANSYFLT  38     /* 'FLOAT' */
#define ANSYVECT 39     /* 'VECTOR' */
#define ANSYREF  40     /* 'REF' */
#define ANSYCONS 41     /* 'CONSTANT' */
#define ANSYNOT  42     /* 'NOT' */
#define ANSYOR   43     /* 'OR' */
#define ANSYAND  44     /* 'AND' */
#define ANSYIF   45     /* 'IF' */
#define ANSYTHEN 46     /* 'THEN' */
#define ANSYELIF 47     /* 'ELIF' */
#define ANSYELSE 48     /* 'ELSE' */
#define ANSYEIF  49     /* 'ENDIF' */
#define ANSYFOR  50     /* 'FOR' */
#define ANSYTO   51     /* 'TO' */
#define ANSYSTEP 52     /* 'STEP' */
#define ANSYEFOR 53     /* 'ENDFOR' */
#define ANSYGOTO 54     /* 'GOTO' */
#define ANSYDO   55     /* 'DO' */
#define ANSYBASI 56     /* 'BASIC' */
#define ANSYUND  57     /* '_' */
#define ANSYMAC  58     /* 'MACRO' */

/*
***Analyser structures.
*/
typedef char ANSYNAME [ANSYNLEN + 1];   /* Name of identifier */

typedef char ANSTR [ANSTRLEN + 1];      /* String Literal value */

struct ANSRCP {                 /* Source position for symbol */
        short srclin;           /* Source line number, short 861028JK */
        short srccol;           /* Source column, short 861028JK */
};

struct ANSYREC {
        ANSYTYP sytype;                 /* Symbol type */
        struct ANSRCP sypos;            /* Source position for this symbol */
        union {
                ANSYNAME name;          /* Name of identifier or ... */
                v2int ival;             /* integer literal value or ... */
                v2float fval;           /* decimal literal value or ... */
                ANSTR sval;             /* string literal value or ... */
                V2REFVA rval;           /* reference literal value */
        } syval;
};

/* 64-bit followset structure */
typedef struct anfset
    {
    char byte[8];
    } ANFSET;

/* expression attributes */
typedef struct anattr
    {
    pm_ptr type;                      /* result value type. (STTYTBL) */
    stvalc rvclass;                   /* result value class */
    } ANATTR;

/* sizes */
#define ANREFMAX MXINIV               /* ref. chain max. length */
#define ANPARMAX V2PARMAX             /* parameter list max. length */
#define ANEXPMAX 10                   /* expression list max. length */

/* asinit() source type symbols */
#define ANRDFIL  0                    /* source on file */
#define ANRDSTR  1                    /* source in string */

#ifdef ANALYZER
/* ancseq() max number of sequence numbers */
#define ANMAXSEQ  100000
/* ancseq() max number of forward references */
#define ANMAXFR  10000
#else
#define ANMAXSEQ  1
#define ANMAXFR  1
#endif


/*
***Function prototypes.
*/

/*
***anaexp.c
*/
short anaexp(char *expr, bool tmpref, pm_ptr *rptr, sttycl *typ) ;
/*
***anascan.c
*/
short asinit(char src[], short alt);
int   anascan(struct ANSYREC *sy);
int   anapeek(struct ANSYREC *sy);
short asexit();

/*
***anlog.c
*/
#ifdef ANALYZER
short anlogi(char  *logfnm, int lipag, int licol, bool list);
#else
short anlogi();
#endif
short analog(short line, short col, char *errcode, char *insert);
short anlogx();
void  anlist(char filename[]);
short anlmsg(short msgnum, short *line_num, short *col_num, char *errtext);
bool  anyerr();

/*
***annaml.c
*/
void annaml(pm_ptr *rptr, ANFSET *follow, pmroutn  proc);

/*
***anpar11.c
*/
void ancoex(pm_ptr *rptr, ANATTR *attr, PMLITVA *value, ANFSET *set);
void anarex(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anterm(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anfact(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anprim(pm_ptr *rptr, ANATTR *attr, ANFSET *set);

/*
***anpar12.c
*/
void anvar(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anfunc(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anrefc(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anidtc(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anlitc(pm_ptr *rptr, ANATTR *attr, ANFSET *set);
void anparl(pm_ptr *rptr, ANATTR attrs[], short *count, ANFSET *set);
void anexpl(pm_ptr *rptr, ANATTR attrs[], short *count, ANFSET *set);

/*
***anpar31.c
*/
void   anperr(char *mess, char *insert, ANFSET *follow, short line, short col);
bool   aneqty(pm_ptr type1, pm_ptr type2);
bool   anacty(pm_ptr ltype, pm_ptr rtype);
void   angtps(pm_ptr type, char *str);
sttycl angtcl(pm_ptr type);
void   anbcom(short oplin, short opcol, ANSYTYP op, ANATTR *lattr,
              ANATTR *rattr, ANATTR *cattr);
void   anucom(short oplin, short opcol, ANSYTYP op,
              ANATTR *attr, ANATTR *cattr);
void   ancpar(ANATTR actpar[], short count, pm_ptr rouptr, short lin,
              short col, short offset, ANATTR *retattr);
void   ancsnd(short seqlin, short seqcol, DBseqnum seqn);
void   ancsnr(short seqlin, short seqcol, DBseqnum seqn);
void   ancseq();
void   anclab();
void   ancset(ANFSET *dset, ANFSET *sset, int ntmls, int tml1, int tml2,
              int tml3, int tml4, int tml5, int tml6, int tml7, int tml8);
bool   aneset(ANFSET *set);
bool   aniset(short memb, ANFSET *set);
void   prtset(ANFSET *set);

/*
***anpar40.c
*/
void anpmod(pm_ptr *modul_p);

/*
***anpar51.c
*/
void ansmts(pm_ptr *rptr, ANFSET *follow);
void ansmte(pm_ptr *rptr, ANFSET *follow, pm_ptr *labptr);
void anlast(pm_ptr *rptr, ANFSET *follow, pm_ptr *labptr);
void anunst(pm_ptr *rptr, ANFSET *follow);
void anoprc(pm_ptr *rptr, ANFSET *follow);
void angprc(pm_ptr *rptr, ANFSET *follow);
void anasst(pm_ptr *rptr, ANFSET *follow);

/*
***anpar52.c
*/
void anlgex(pm_ptr *rptr, ANFSET *follow);
void anlter(pm_ptr *rptr, ANFSET *follow);
void anlpri(pm_ptr *rptr, ANFSET *follow);
void anlrel(pm_ptr *rptr, ANFSET *follow);
void anifst(pm_ptr *rptr, ANFSET *follow);
void anfost(pm_ptr *rptr, ANFSET *follow);
void angost(pm_ptr *rptr, ANFSET *follow);
