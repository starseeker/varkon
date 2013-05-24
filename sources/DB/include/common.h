/**********************************************************************
*
*    common.h
*    ========
*
*    This file is part of the VARKON Database Library.
*    URL:  http://www.varkon.com
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
***This file includes definitions of things that are also
***used in other parts of the VARKON system. Many of these
***are used by DB and stored in DB so any changes might
***destroy DB file compatibility if not properly handled
***in also in DB.
*/
#define V3STRLEN  132           /* Max number of characters in MBS string */
#define JNLGTH    31            /* Max length of a jobname */
#define V3PTHLEN  V3STRLEN      /* Max length of a file path */
#define MODNIV    10            /* Max level of nested parts */
#define MXINIV    MODNIV        /*        - "" -             */
#define PARTNIMX  MODNIV        /*        - "" -             */
#define V2SYNLEN  31            /* Max number of characters in MBS symbol */
#define V2PARMAX  105           /* Max number of parameters to MBS routine */
#define V2MPMX    50            /* Max number of parameters to part */
#define V2NPAMAX  V2PARMAX+15   /* Max number of nested parameters */
#define V2NFUMAX  15            /* Max level of nested function calls */
#define TRUE      1             /* Boolean TRUE value */
#define FALSE     0             /* Boolean FALSE value */

typedef DBint   gmint;          /* Varkon integer */
typedef DBfloat gmflt;          /* Varkon float */
typedef DBint   v2int;          /* MBS Integer */
typedef DBfloat v2float;        /* MBS Float */
typedef DBint   v2addr;         /* MBS internal RTS address value */
typedef DBint   v2bool;         /* MBS internal boolean value */
typedef char    v2vaty;         /* MBS internal type code */
typedef DBint   pm_ptr;         /* MBS internal PM pointer */

typedef struct                  /* MBS 3D vector */
{
DBfloat x_val;                  /* X-coordinate  */
DBfloat y_val;                  /* Y-   - " -    */
DBfloat z_val;                  /* Z-   - " -    */
 } V2VECVA;

typedef struct v2refva          /* Entity ID used by MBS and GM */
{                               /* Compatible with PMREFVA & DBId */
DBseqnum seq_val;               /* Sequence number */
DBordnum ord_val;               /* Order number */
struct v2refva  *p_nextre;      /* Pointer to next ID in chain */
} V2REFVA;

/*
***The following definition needs FILE to be defined so
***lets include stdio.h here.
*/

#include <stdio.h>

typedef struct                  /* MBS file control block */
{
FILE *fp;                       /* C file pointer */
char mode[3];                   /* I/O type (R,W,U..) */
int  open;                      /* Open/closed TRUE/FALSE */
int  iostat;                    /* Error status */
}  V2FILE;

typedef struct                  /* Parameter descriptor (PMPATLOG in MBS) */
{
v2vaty log_id;                  /* Parameter type */
DBint log_offs;                 /* Parameter value offset */
} V2PATLOG;

typedef struct                  /* Parameter value (PMLITVA in MBS) */
{
v2vaty  lit_type;               /* Value type  */
union
  {
  v2int    int_va;              /* Value type  :   integer */
  v2float  float_va;            /*     - " -       float   */
  char     str_va[ V3STRLEN+1]; /*     - " -       string  */
  V2VECVA  vec_va;              /*     - " -       vector  */
  V2REFVA  ref_va[MXINIV];      /*     - " -       reference */
  v2addr   adr_va;              /*     - " -       RTS offset */
  v2bool   bool_va;             /*     - " -       boolean */
  V2FILE  *fil_va;              /*     - " -       file control block */
  } lit;
} V2LITVA;

typedef struct                  /* Graphical attribute block */
 {
 v2int   pen;                   /* Pen/color number */
 v2int   level;                 /* Level/layer number */
 v2int   lfont;                 /* Line font */
 v2int   afont;                 /* Arc font */
 v2float ldashl;                /* Line dash length */
 v2float adashl;                /* Arc dash length */
 v2float tsize;                 /* Text size */
 v2float twidth;                /* Text width */
 v2float tslant;                /* Text slant */
 v2float dtsize;                /* Dimension text size */
 v2float dasize;                /* Dimension arrow size */
 v2int   dndig;                 /* Dimension decimal places */
 v2int   dauto;                 /* Dimension auto flag */
 v2int   xfont;                 /* Xhatch font */
 v2float xdashl;                /* Xhatch dash length */
 v2int   blank;                 /* Visible/unvisible flag */
 v2int   hit;                   /* Selectable/unselectable flag */
 v2int   save;                  /* Persistent/temporary flag */
 v2int   cfont;                 /* Curve font */
 v2float cdashl;                /* Curve dashlength */
 v2int   tfont;                 /* Text font */
 v2int   sfont;                 /* Surface font */
 v2float sdashl;                /* Surface dash length */
 v2int   nulines;               /* Surface U-mesh density */
 v2int   nvlines;               /* Surface V-mesh density */
 v2float width;                 /* Linewidth */
 v2int   tpmode;                /* Text position mode */
 v2int   mfont;                 /* Mesh font */
 v2int   pfont;                 /* Point font */
 v2float psize;                 /* Point size */
 } V2NAPA;

typedef struct                  /* System memory sizes */
{
v2int pm;                       /* Size of PM */
v2int df;                       /* Default size of display file */
v2int gm;                       /* Size of GM RAM cache */
v2int rts;                      /* Size of RTS */
} V3MSIZ;

typedef struct                  /* System metadata */
{
short sernr;                    /* Serial number */
short vernr;                    /* Version number */
short revnr;                    /* Revision number */
char  level;                    /* Development level */
short year_c;                   /* Year initially created */
short mon_c;                    /* Month initially created */
short day_c;                    /* Day initially created */
short hour_c;                   /* Hour initially created */
short min_c;                    /* Minute initially created */
short year_u;                   /* Year updated */
short mon_u;                    /* Month updated */
short day_u;                    /* Day updated */
short hour_u;                   /* Hour updated */
short min_u;                    /* Minute updated */
char  sysname[9];               /* OS or host name */
char  dummy[5];                 /* Not used */
short opmode;                   /* BAS_MOD/RIT_MOD */
short mpcode;                   /* Module protection code */
char  release[9];               /* OS release */
char  version[9];               /* OS version */
unsigned int ser_crypt;         /* Encrypted serialnumber */
} V3MDAT;

#define C_NO_VA   0             /* Constant for no value */
#define C_INT_VA  1             /*    - " -     INT_VAL */
#define C_FLO_VA  2             /*    - " -     FLO_VAL */
#define C_STR_VA  3             /*    - " -     STR_VAL */
#define C_VEC_VA  4             /*    - " -     VEC_VAL */
#define C_REF_VA  5             /*    - " -     REF_VAL */
#define C_ADR_VA  6             /*    - " -     address value */
#define C_BOL_VA  7             /*    - " -     boolean value */
#define C_FIL_VA  8             /*    - " -     FILE */

#define  v2intsz      sizeof(v2int)            /* Size of MBS INT */
#define  v2flosz      sizeof(v2float)          /* Size of MBS FLOAT */
#define  v2chsz       1                        /* Size of MBS STRING *1 */
#define  v2vecsz      sizeof(V2VECVA)          /* Size of VECTOR */
#define  v2refsz      MXINIV*sizeof(V2REFVA)   /* Size of REF */
#define  v2addrsz     sizeof(v2addr)           /* Size of internal address */
#define  v2boolsz     sizeof(v2bool)           /* Size of internal boolean */
#define  v2filesz     sizeof(V2FILE)           /* Size of FILE */
