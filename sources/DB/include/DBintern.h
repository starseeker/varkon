/**********************************************************************
*
*    DB_intern.h
*    ===========
*
*    This file is part of the VARKON Database Library.
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
***********************************************************************/

/*
***Include old versions of GM structures for backward
***compatibility.
*/

#include "gmolds.h"

/*
***Some global data used by the internals.
*/

extern GMMDAT   metdat;
extern V3MDAT   gmsdat_org;
extern V3MDAT   gmsdat_db;
extern V3MSIZ   gmssiz;
extern FILE    *gmpfpk;
extern DBint    pfsiz;
extern DBshort  cirpek;
extern DBshort  cirdef;
extern DBptr    lstprt;
extern DBptr    lstidt;
extern DBptr    nxtins;
extern DBseqnum lstid;
extern DBseqnum lstidm;
extern DBpagnum fystal;
extern DBpagnum fystsz;
extern char    *gmbuf;
extern DBptr    actprt;
extern DBptr    actidt;
extern DBptr    huvprt;
extern DBseqnum huvidm;
extern DBseqnum actidm;
extern DBpagnum ipgnum;
extern DBpagnum dpgnum;
extern DBshort  datofs;
extern DBptr    templa;
extern DBseqnum tempsn;

/*
***A logical page.
*/

typedef struct
{
DBint ptr;                     /* Page adress */
tbool all;                     /* Allocated/Not allocated */
} LTBSTR;

extern LTBSTR *logtab;

/*
***A physical page.
*/

typedef struct
{
DBpagnum pagnum;               /* Logical page number */
DBint    pfpadr;               /* Adress in page file */
tbool    wrflg;                /* Modified/Not modified */
tbool    reflg;                /* Referenced/Not referenced */
} FTBSTR;

extern FTBSTR *fystab;

/*
***The keytable.
*/

typedef struct
{
char key[V3STRLEN+1];          /* Key value */
DBint type;                    /* Basic type, C_INT_VA... */
DBint size;                    /* Basic size in bytes */
DBint count;                   /* Number of array elements */
DBptr datla;                   /* Data address */
} KEYDATA;

extern DBint    nkeys;
extern KEYDATA *keytab;

/*
***Some internal constants.
*/

#define LTSDEF   (DBint)256    /* Default logical size */
#define IDTSIZ   (DBint)128    /* Default size of ID table */
#define PAGSIZ   (DBint)1024   /* Number of bytes/page */
#define FYSSIZ   (DBint)262144 /* Default physical cache size */
#define FYSTSZ   FYSSIZ/PAGSIZ /* Number of physical pages available */
#define PGFPNA   -999999       /* Pagefile page not allocated */
#define GMBPNA   -9999         /* Physical page not allocated */
#define EMPTY    -127          /* Empty byte */
#define NOTUSD   -1            /* Not used ID table entry */
#define ERASED   -2            /* Deleted ID table entry */

/*
***Entity version numbers. (GMRECH->vers).
*/

#define GMPOSTV0   0           /* Everything before 1.10N */
#define GMPOSTV1  -1           /* GMCUR changed */
#define GMPOSTV2  -2           /* Textfont and UV-curves added */
#define GMPOSTV3  -3           /* Linewidth added */
#define GMPOSTV4  -4           /* 3D text added */

#ifdef WIN32
#define GMVERS(p) (DBint)p->vers /* Fast macro for version */
#else
#define GMVERS(p) gmvers(p)      /* Portable macro for version */
#endif

/* Macro for block memory move */

#define V3MOME(from,to,size) memcpy(to,from,size)

/*
***Prototypes for internal functions.
*/

DBstatus DBreset();
DBstatus gmsvpf();
DBstatus gmsvkt();
DBstatus gmldpf();
DBstatus gmldkt();
DBstatus gmclr(DBint logmax);
DBstatus gmgrow();
DBstatus gmcrpf(char *filnam);
DBstatus gmclpf();
DBstatus gmidst(DBId *id, char *idstr);
DBint    gmvers(GMRECH *hdrptr);
DBstatus gmunlk(DBptr la);
DBstatus gmglor(DBId *idin, DBshort level, DBId *idout);
DBstatus gmmtm(DBseqnum id);
DBstatus gmumtm();
DBstatus gmrltm();
DBptr    pfault(DBpagnum pagnum);
DBptr    alldat(DBint size);
DBptr    allidt(DBseqnum size);
DBstatus clrpag(DBpagnum pagnum);
DBstatus wrdat1(char *datptr, DBptr *laptr, DBint size);
DBstatus wrdat2(char *datptr, DBptr *laptr, DBint size);
DBstatus rddat1(char *datptr, DBptr la, DBint size);
DBstatus rddat2(char *datptr, DBptr la, DBint size);
DBstatus updata(char *datptr, DBptr la, DBint size);
DBstatus rldat1(DBptr la, DBint size);
DBstatus rldat2(DBptr la, DBint size);
DBstatus inpost(GMUNON *entity, DBId *id, DBptr *laptr, DBint size);
char    *gmgadr(DBptr la);
DBstatus wrid(DBseqnum id, DBptr idla);
DBptr    gmrdid(DBptr idtab, DBseqnum id);
DBptr    gtlain(DBptr la, DBordnum ordnum, DBetype *type);

/* The following error handler must be provided by the user */

DBstatus erpush(char *errcode, char *insert);

/* The following memory allocation must be provided by the user */

void     *v3mall(unsigned size, char *user);
void     *v3rall(void *memptr, unsigned size, char *user);
DBstatus  v3free(void *memptr, char *user);
