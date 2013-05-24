/**********************************************************************
*
*    stacc.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    ST - Symbol Table access routines
*    The library contains the following entry routines 
*
*    Initialization routines:
*     stgini()       Inits the systems symbol table
*     stlini()       Inits a local symbol table
*
*    Common symbol table routines:
*     sticmp()       Compares two identifiers
*     stlook()       Search for a name in the table
*     stratt()       Read common attributes
*     stgnid()       Get next identifier
*
*    Type manipulation routines:
*     stdtyp()       Define a primitive type (Not compound)
*     stdarr()       Define an array type
*     strtyp()       Read type attributes
*     strarr()       Read array attributes
*
*    Variable access routines:
*     stcvar()       Creates a variable
*     strvar()       Read variable attributes
*     stuvar()       Update variable attributes
*
*    Named constants routines:
*     stccon()       Create a named constant
*     strcon()       Read constant attributes
*     stgcon()       get constant declaration attributes
*     stucon()       Update constant attributes
*
*     Label access routines:
*     stclab()       Create a label
*     strlab()       Read label attributes
*     stulab()       Update label attributes
*
*    Procedure/Function access routines
*     strrou()       Read procedure/function attributes 
*
*    Local routines
*
*     static short  setbase();
*     static void   resbase();
*     static short  sysrou();
*     static short  insert();
*     static pm_ptr find();
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
*    (C) Johan Kjellander, Örebro university
*
**********************************************************************/

/**********************************************************************
*  The symbol table is implemented by one binary tree for each module
*  level. The static nesting depth is two - the systems level and the
*  active module. Pointers constituting the binary tree and pointers to
*  type descriptors etc. are "offsets" from a base address in PM.
*  Note! The systems symbols have negative offsets to distinguish them
*  from other symbols without storing the level number.
**********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../IG/include/isch.h"
#include "../include/mbsrout.h"

#define  STLEVMAX     2             /* Number of nested blocks (modules)      */
#define  STCALLMX     16            /* Number of nested calls (read routines) */

/*
***Implementation of the Symbol table internal representation
*/
typedef struct stidtbl
   {
     pm_ptr      name_id;           /* Name string              */
     pm_ptr      llink_id;          /* Left hand side subtree   */
     pm_ptr      rlink_id;          /* Right hand side subtree  */
     pm_ptr      next_id;           /* Next identifier in chain */
     stidcl      kind_id;           /* Identifier classification  */
     union
       {
        struct
           {                        /* Variable */
             pm_ptr      vtyp_id;   /* Variable type           */
             DBint       vadr_id;   /*          address        */
             stvarcl     vkind_id;  /*          classification */
             pm_ptr      vdef_id;   /*          default value  */
           } var_id;

        struct
           {                        /* Constant */
            pm_ptr       ctyp_id;   /* Named constant type     */
            pm_ptr       cval_id;   /*                value    */
           } const_id;

        struct
           {                        /* Label */
             pm_ptr      llst_id;   /* statement list address    */
             pm_ptr      lnod_id;   /* statement list node addr. */
             bool        ldef_id;   /* TRUE -> defined           */
             bool        lref_id;   /* TRUE -> referenced        */
           } label_id;

        struct
           {                        /* Procedure/Function  */
             pmroutn     fkind_id;  /* Procedure/Function class. */
             bool        fcnst_id;  /* TRUE if ok in const expr. */
             stprcl      fpcl_id;   /* Procedure classification  */
           } rout_id;
        } class_id;
   }  STIDTBL;

typedef  struct symtab
            {
               pm_ptr     first;    /* Pointer to first name in tbl. */
               pm_ptr     bla;      /* Base address                  */
            }  SYMTAB; 

DBint   stgldz;                      /* Global data size                   */
pm_ptr  stintp;                      /* Integer type pointer               */
pm_ptr  stflop;                      /* Float type pointer                 */
pm_ptr  stvecp;                      /* Vector type pointer                */
pm_ptr  strefp;                      /* Reference type pointer             */
pm_ptr  stfilp;                      /* File type pointer                  */
pm_ptr  ststrp;                      /* Dummy string pointer               */
pm_ptr  stbolp;                      /* Boolean pointer                    */

pm_ptr st_j_p;                       /* 4*4 array of float                 */
pm_ptr st_k_p;                       /* conformant array of st_a_p         */
pm_ptr st_l_p;                       /* conformant array of stflop         */
pm_ptr st_m_p;                       /* conformant array of strefp         */
pm_ptr st_n_p;                       /* conformant array of stvecp         */
pm_ptr st_o_p;                       /* conformant array of ststrp         */
pm_ptr st_p_p;                       /* conformant array of stintp         */

static  SYMTAB  mlevel[STLEVMAX];    /* Symbol table nesting               */
static  short   currlev;             /* Current module level               */
static  pm_ptr  prev_bas[STCALLMX];  /* Previous base address. Pushed by   */
                                     /* setbase() and popped by resbase()  */
static  short   baseptr = 0;         /* Base address stack pointer         */
static  pm_ptr  lastid = (pm_ptr)NULL; /* Last inserted identifier         */

static  struct syspf                 /* System names                       */
           {
               string    name;
               stidcl    class;
               bool      cnst;
               stprcl    proc_cl;
               pmroutn   pfid;
           }
/*
***The Varkon symbol table starts here.
***Symbols marked with stars are dummy replacements for
***symbols that are removed and no longer valid. 
*/
#ifdef VARKON
            systab[ ] =
              {
               {"POI_FREE",    ST_PROC,    FALSE,   ST_GEO,   VPOI},
               {"POI_PROJ",    ST_PROC,    FALSE,   ST_GEO,   VPOIP},
               {"LIN_FREE",    ST_PROC,    FALSE,   ST_GEO,   VLINF},
               {"LIN_PROJ",    ST_PROC,    FALSE,   ST_GEO,   VLINP},
               {"LIN_ANG",     ST_PROC,    FALSE,   ST_GEO,   VLINA},
               {"LIN_OFFS",    ST_PROC,    FALSE,   ST_GEO,   VLINO},
               {"LIN_TAN1",    ST_PROC,    FALSE,   ST_GEO,   VLINT1},
               {"LIN_TAN2",    ST_PROC,    FALSE,   ST_GEO,   VLINT2},
               {"ARC_1POS",    ST_PROC,    FALSE,   ST_GEO,   VARCF},
               {"ARC_2POS",    ST_PROC,    FALSE,   ST_GEO,   VARC2P},
               {"ARC_3POS",    ST_PROC,    FALSE,   ST_GEO,   VARC3P},
               {"ARC_OFFS",    ST_PROC,    FALSE,   ST_GEO,   VARCO},
               {"ARC_FIL",     ST_PROC,    FALSE,   ST_GEO,   VARCFI},
               {"********",    ST_PROC,    FALSE,   ST_GEO,   VCURF},
               {"********",    ST_PROC,    FALSE,   ST_GEO,   VCURP},
               {"COMP",        ST_PROC,    FALSE,   ST_GEO,   VCOMP},
               {"XHATCH",      ST_PROC,    FALSE,   ST_GEO,   VXHT},
               {"PART",        ST_PROC,    FALSE,   ST_GEO,   VPART},
               {"TEXT",        ST_PROC,    FALSE,   ST_GEO,   VTEXT},
               {"CSYS_3P",     ST_PROC,    FALSE,   ST_GEO,   VCSYS3P},
               {"GROUP",       ST_PROC,    FALSE,   ST_GEO,   VGROUP},
               {"LDIM",        ST_PROC,    FALSE,   ST_GEO,   VLDIM},
               {"CDIM",        ST_PROC,    FALSE,   ST_GEO,   VCDIM},
               {"RDIM",        ST_PROC,    FALSE,   ST_GEO,   VRDIM},
               {"ADIM",        ST_PROC,    FALSE,   ST_GEO,   VADIM},
               {"ARCCOS",      ST_FUNC,    TRUE,    ST_ORD,   VACOS},
               {"ARCSIN",      ST_FUNC,    TRUE,    ST_ORD,   VASIN},
               {"ARCTAN",      ST_FUNC,    TRUE,    ST_ORD,   VATAN},
               {"COS",         ST_FUNC,    TRUE,    ST_ORD,   VCOS},
               {"SIN",         ST_FUNC,    TRUE,    ST_ORD,   VSIN},
               {"TAN",         ST_FUNC,    TRUE,    ST_ORD,   VTAN},
               {"LN",          ST_FUNC,    TRUE,    ST_ORD,   VLN},
               {"LOG",         ST_FUNC,    TRUE,    ST_ORD,   VLOG},
               {"SQRT",        ST_FUNC,    TRUE,    ST_ORD,   VSQRT},
               {"ABS",         ST_FUNC,    TRUE,    ST_ORD,   VABS},
               {"FRAC",        ST_FUNC,    TRUE,    ST_ORD,   VFRAC},
               {"ROUND",       ST_FUNC,    TRUE,    ST_ORD,   VROUND},
               {"TRUNC",       ST_FUNC,    TRUE,    ST_ORD,   VTRUNC},
               {"ARCL",        ST_FUNC,    FALSE,   ST_GEO,   VARCL},
               {"INTERSECT",   ST_FUNC,    FALSE,   ST_GEO,   VINTERS},
               {"ON",          ST_FUNC,    FALSE,   ST_GEO,   VON},
               {"VEC",         ST_FUNC,    TRUE,    ST_GEO,   VVEC},
               {"VECP",        ST_FUNC,    TRUE,    ST_GEO,   VVECP},
               {"IDENT",       ST_FUNC,    FALSE,   ST_GEO,   VIDENT},
               {"POS",         ST_FUNC,    FALSE,   ST_GEO,   VPOS},
               {"SCREEN",      ST_FUNC,    FALSE,   ST_GEO,   VSCREEN},
               {"MODE_LOCAL",  ST_PROC,    FALSE,   ST_ORD,   VMODLO},
               {"MODE_GLOBAL", ST_PROC,    FALSE,   ST_ORD,   VMODGL},
               {"SET",         ST_PROC,    FALSE,   ST_ORD,   VSET},
               {"ASCII",       ST_FUNC,    TRUE,    ST_ORD,   VASCII},
               {"STR",         ST_FUNC,    TRUE,    ST_ORD,   VSTR},
               {"CHR",         ST_FUNC,    TRUE,    ST_ORD,   VCHR},
               {"***",         ST_UNDEF,   FALSE,   ST_UNDEF, VVAL},
               {"LENGTH",      ST_FUNC,    TRUE,    ST_ORD,   VLENGTH},
               {"SUBSTR",      ST_FUNC,    TRUE,    ST_ORD,   VSUBSTR},
               {"OPEN",        ST_PROC,    FALSE,   ST_ORD,   VOPEN},
               {"SEEK",        ST_PROC,    FALSE,   ST_ORD,   VSEEK},
               {"OUTINT",      ST_PROC,    FALSE,   ST_ORD,   VOUTINT},
               {"OUTFLT",      ST_PROC,    FALSE,   ST_ORD,   VOUTFLT},
               {"OUTSTR",      ST_PROC,    FALSE,   ST_ORD,   VOUTSTR},
               {"OUTLIN",      ST_PROC,    FALSE,   ST_ORD,   VOUTLIN},
               {"OUTBIN",      ST_PROC,    FALSE,   ST_ORD,   VOUTBIN},
               {"ININT",       ST_FUNC,    FALSE,   ST_ORD,   VININT},
               {"INFLT",       ST_FUNC,    FALSE,   ST_ORD,   VINFLT},
               {"INSTR",       ST_FUNC,    FALSE,   ST_ORD,   VINSTR},
               {"INLIN",       ST_FUNC,    FALSE,   ST_ORD,   VINLIN},
               {"INBIN",       ST_PROC,    FALSE,   ST_ORD,   VINBIN},
               {"FPOS",        ST_FUNC,    FALSE,   ST_ORD,   VFPOS},
               {"CLOSE",       ST_PROC,    FALSE,   ST_ORD,   VCLOSE},
               {"IOSTAT",      ST_FUNC,    FALSE,   ST_ORD,   VIOSTAT},
               {"OS",          ST_PROC,    FALSE,   ST_ORD,   VOS},
               {"",            ST_UNDEF,   FALSE,   ST_ORD,   ST_UNDEF}
              };

static  struct syspf addtab1[ ] =       /* System names  - add on table 1 */
              {
               {"TANG",        ST_FUNC,    FALSE,   ST_GEO,   VTANG},
               {"CENTRE",      ST_FUNC,    FALSE,   ST_GEO,   VCENTRE},
               {"",            ST_UNDEF,   FALSE,   ST_ORD,   ST_UNDEF}
              };

static  struct syspf addtab2[ ] =       /* System names  - add on table 2 */
              {
               {"*****",             ST_FUNC,     FALSE,  ST_ORD,    VGTID},
               {"GETHDR",            ST_PROC,     FALSE,  ST_ORD,    VGTHDR},
               {"GETPOI",            ST_PROC,     FALSE,  ST_ORD,    VGTPOI},
               {"GETLIN",            ST_PROC,     FALSE,  ST_ORD,    VGTLIN},
               {"GETARC",            ST_PROC,     FALSE,  ST_ORD,    VGTARC},
               {"******",            ST_PROC,     FALSE,  ST_ORD,    VGTCUR},
               {"GETTRF",            ST_PROC,     FALSE,  ST_ORD,    VGTTRF},
               {"GETTXT",            ST_PROC,     FALSE,  ST_ORD,    VGTTXT},
               {"GETXHT",            ST_PROC,     FALSE,  ST_ORD,    VGTXHT},
               {"GETLDM",            ST_PROC,     FALSE,  ST_ORD,    VGTLDM},
               {"GETCDM",            ST_PROC,     FALSE,  ST_ORD,    VGTCDM},
               {"GETRDM",            ST_PROC,     FALSE,  ST_ORD,    VGTRDM},
               {"GETADM",            ST_PROC,     FALSE,  ST_ORD,    VGTADM},
               {"GETGRP",            ST_PROC,     FALSE,  ST_ORD,    VGTGRP},
               {"GETCSY",            ST_PROC,     FALSE,  ST_ORD,    VGTCSY},
               {"GETSYM",            ST_PROC,     FALSE,  ST_ORD,    VGTSYM},
               {"GETPRT",            ST_PROC,     FALSE,  ST_ORD,    VGTPRT},
               {"GETTYP",            ST_FUNC,     FALSE,  ST_ORD,    VGTTYP},
               {"GETINT",            ST_FUNC,     FALSE,  ST_ORD,    VGTINT},
               {"GETFLT",            ST_FUNC,     FALSE,  ST_ORD,    VGTFLT},
               {"GETSTR",            ST_FUNC,     FALSE,  ST_ORD,    VGTSTR},
               {"GETVEC",            ST_FUNC,     FALSE,  ST_ORD,    VGTVEC},
               {"SYMB",              ST_PROC,     FALSE,  ST_GEO,    VSYMB},
               {"TRIM",              ST_PROC,     FALSE,  ST_ORD,    VTRIM},
               {"DEL",               ST_PROC,     FALSE,  ST_ORD,    VDEL},
               {"CLEAR_GM",          ST_PROC,     FALSE,  ST_ORD,    VCLGM},
               {"BLANK",             ST_PROC,     FALSE,  ST_ORD,    VBLNK},
               {"UNBLANK",           ST_PROC,     FALSE,  ST_ORD,    VUBLNK},
               {"GETREF",            ST_FUNC,     FALSE,  ST_ORD,    VGTREF},
               {"LOAD_GM",           ST_PROC,     FALSE,  ST_ORD,    VLDGM},
               {"SAVE_GM",           ST_PROC,     FALSE,  ST_ORD,    VSVGM},
               {"*********",         ST_PROC,     FALSE,  ST_ORD,    VBLEV},
               {"***********",       ST_PROC,     FALSE,  ST_ORD,    VUBLEV},
               {"*******",           ST_PROC,     FALSE,  ST_ORD,    VGTLEV},
               {"*******",           ST_FUNC,     FALSE,  ST_ORD,    VACLEV},
               {"********",          ST_UNDEF,    FALSE,  ST_UNDEF,  VPOI},
               {"********",          ST_UNDEF,    FALSE,  ST_UNDEF,  VPOI},
               {"CRE_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VCREVI},
               {"ACT_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VACTVI},
               {"SCL_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VSCLVI},
               {"CEN_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VCENVI},
               {"ERASE_VIEW",        ST_PROC,     FALSE,  ST_ORD,    VERAVI},
               {"REP_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VREPVI},
               {"CACC_VIEW",         ST_PROC,     FALSE,  ST_ORD,    VCACVI},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VPLTVI},
               {"TIME",              ST_PROC,     FALSE,  ST_ORD,    VTIM},
               {"RSTR",              ST_FUNC,     FALSE,  ST_ORD,    VRSTR},
               {"RVAL",              ST_FUNC,     FALSE,  ST_ORD,    VRVAL},
               {"REFC",              ST_FUNC,     FALSE,  ST_ORD,    VREFC},
               {"LST_INI",           ST_PROC,     FALSE,  ST_ORD,    VLSIN},
               {"LST_EXI",           ST_PROC,     FALSE,  ST_ORD,    VLSEX},
               {"LST_LIN",           ST_PROC,     FALSE,  ST_ORD,    VLSLN},
               {"ACT_BLANK",         ST_FUNC,     FALSE,  ST_ORD,    VACBLK},
               {"ACT_VNAM",          ST_FUNC,     FALSE,  ST_ORD,    VACVIN},
               {"ACT_PEN",           ST_FUNC,     FALSE,  ST_ORD,    VACPEN},
               {"ACT_SCL",           ST_FUNC,     FALSE,  ST_ORD,    VACSCL},
               {"********",          ST_FUNC,     FALSE,  ST_ORD,    VACDSCL},
               {"ACT_CACC",          ST_FUNC,     FALSE,  ST_ORD,    VACCAC},
               {"ACT_GRIDX",         ST_FUNC,     FALSE,  ST_ORD,    VACGRX},
               {"ACT_GRIDY",         ST_FUNC,     FALSE,  ST_ORD,    VACGRY},
               {"ACT_GRIDDX",        ST_FUNC,     FALSE,  ST_ORD,    VACGRDX},
               {"ACT_GRIDDY",        ST_FUNC,     FALSE,  ST_ORD,    VACGRDY},
               {"ACT_GRID",          ST_FUNC,     FALSE,  ST_ORD,    VACGRID},
               {"ACT_JOBNAM",        ST_FUNC,     FALSE,  ST_ORD,    VACJOBN},
               {"*********",         ST_FUNC,     FALSE,  ST_ORD,    VACMTYP},
               {"ACT_MATTR",         ST_FUNC,     FALSE,  ST_ORD,    VACMATR},
               {"ACT_JOBDIR",        ST_FUNC,     FALSE,  ST_ORD,    VACJOBD},
               {"ACT_LFONT",         ST_FUNC,     FALSE,  ST_ORD,    VACLFNT},
               {"ACT_AFONT",         ST_FUNC,     FALSE,  ST_ORD,    VACAFNT},
               {"ACT_XFONT",         ST_FUNC,     FALSE,  ST_ORD,    VACXFNT},
               {"ACT_LDASHL",        ST_FUNC,     FALSE,  ST_ORD,    VACLDSL},
               {"ACT_ADASHL",        ST_FUNC,     FALSE,  ST_ORD,    VACADSL},
               {"ACT_XDASHL",        ST_FUNC,     FALSE,  ST_ORD,    VACXDSL},
               {"ACT_TSIZE",         ST_FUNC,     FALSE,  ST_ORD,    VACTSIZ},
               {"ACT_TWIDTH",        ST_FUNC,     FALSE,  ST_ORD,    VACTWID},
               {"ACT_TSLANT",        ST_FUNC,     FALSE,  ST_ORD,    VACTSLT},
               {"ACT_DTSIZ",         ST_FUNC,     FALSE,  ST_ORD,    VACDTSZ},
               {"ACT_DASIZ",         ST_FUNC,     FALSE,  ST_ORD,    VACDASZ},
               {"ACT_DNDIG",         ST_FUNC,     FALSE,  ST_ORD,    VACDNDG},
               {"ACT_DAUTO",         ST_FUNC,     FALSE,  ST_ORD,    VACDAUT},
               {"CUR_COMP",          ST_PROC,     FALSE,  ST_GEO,    VCOMP},
               {"GLOBAL_REF",        ST_FUNC,     FALSE,  ST_ORD,    VGLOREF},
               {"FINDS",             ST_FUNC,     FALSE,  ST_ORD,    VFINDS},
               {"VECL",              ST_FUNC,     FALSE,  ST_ORD,    VVECL},
               {"VECN",              ST_FUNC,     FALSE,  ST_ORD,    VVECN},
               {"ANGLE",             ST_FUNC,     FALSE,  ST_ORD,    VANGLE},
               {"PSH_PMT",           ST_PROC,     FALSE,  ST_ORD,    VPSPMT},
               {"POP_PMT",           ST_PROC,     FALSE,  ST_ORD,    VPOPMT},
               {"CRE_TSTR",          ST_PROC,     FALSE,  ST_ORD,    VCRTSTR},
               {"GET_TSTR",          ST_FUNC,     FALSE,  ST_ORD,    VGTTSTR},
               {"CRE_MEN",           ST_PROC,     FALSE,  ST_ORD,    VCRMEN},
               {"PSH_MEN",           ST_PROC,     FALSE,  ST_ORD,    VPSMEN},
               {"POP_MEN",           ST_PROC,     FALSE,  ST_ORD,    VPOMEN},
               {"GET_ALT",           ST_FUNC,     FALSE,  ST_ORD,    VGTALT},
               {"GET_MEN",           ST_PROC,     FALSE,  ST_ORD,    VGTMEN},
               {"VPROD",             ST_FUNC,     FALSE,  ST_ORD,    VVPROD},
               {"LIN_PERP",          ST_PROC,     FALSE,  ST_GEO,    VLINPER},
               {"*******",           ST_UNDEF,    FALSE,  ST_UNDEF,  VDBOP},
               {"********",          ST_UNDEF,    FALSE,  ST_UNDEF,  VDBBG},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBSL},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBUP},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBIN},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBDL},
               {"******",            ST_UNDEF,    FALSE,  ST_UNDEF,  VDBEN},
               {"***********",       ST_UNDEF,    FALSE,  ST_UNDEF,  VDBRL},
               {"********",          ST_UNDEF,    FALSE,  ST_UNDEF,  VDBCL},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBFI},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBFF},
               {"*********",         ST_UNDEF,    FALSE,  ST_UNDEF,  VDBFS},
               {"*******",           ST_UNDEF,    FALSE,  ST_UNDEF,  VDBNX},
               {"B_PLANE",           ST_PROC,     FALSE,  ST_GEO,    VBPLAN},
               {"GETBPL",            ST_PROC,     FALSE,  ST_ORD,    VGTBPL},
               {"CSYS_1P",           ST_PROC,     FALSE,  ST_GEO,    VCSYS1P},
               {"PLOT_WIN",          ST_PROC,     FALSE,  ST_ORD,    VPLTWI},
               {"UPDHDR",            ST_PROC,     FALSE,  ST_ORD,    VUPHDR},
               {"LOAD_JOB",          ST_PROC,     FALSE,  ST_ORD,    VLDJOB},
               {"SAVE_JOB",          ST_PROC,     FALSE,  ST_ORD,    VSVJOB},
               {"INPMT",             ST_FUNC,     FALSE,  ST_ORD,    VINPMT},
               {"HIDE_VIEW",         ST_PROC,     FALSE,  ST_ORD,    VHIDVI},
               {"PERP_VIEW",         ST_PROC,     FALSE,  ST_ORD,    VPRPVI},
               {"TEXTL",             ST_FUNC,     FALSE,  ST_ORD,    VTEXTL},
               {"EXIT",              ST_PROC,     FALSE,  ST_ORD,    VEXIT}, 
               {"EXESTAT",           ST_FUNC,     FALSE,  ST_ORD,    VEXEST},
               {"AREA",              ST_FUNC,     FALSE,  ST_ORD,    VAREA},
               {"CGRAV",             ST_FUNC,     FALSE,  ST_ORD,    VCGRAV},
               {"SET_BASIC",         ST_PROC,     FALSE,  ST_ORD,    VSETB},
               {"CUR_CONIC",         ST_PROC,     FALSE,  ST_GEO,    VCURC},
               {"INV_TANG",          ST_FUNC,     FALSE,  ST_ORD,    VITANG},
               {"CUR_SPLINE",        ST_PROC,     FALSE,  ST_GEO,    VCURS},
               {"CUR_OFFS",          ST_PROC,     FALSE,  ST_GEO,    VCURO},
               {"INV_ON",            ST_FUNC,     FALSE,  ST_ORD,    VION},
               {"CUR_TRIM",          ST_PROC,     FALSE,  ST_GEO,    VCURT},
               {"CUR_USRDEF",        ST_PROC,     FALSE,  ST_GEO,    VCURU},
               {"CURV",              ST_FUNC,     FALSE,  ST_ORD,    VCURV},
               {"INV_CURV",          ST_FUNC,     FALSE,  ST_ORD,    VICURV},
               {"INV_ARCL",          ST_FUNC,     FALSE,  ST_ORD,    VIARCL},
               {"ACT_CFONT",         ST_FUNC,     FALSE,  ST_ORD,    VACCFNT},
               {"ACT_CDASHL",        ST_FUNC,     FALSE,  ST_ORD,    VACCDSL},
               {"ADD_MBS",           ST_FUNC,     FALSE,  ST_ORD,    VADDMBS},
               {"N_INTERSECT",       ST_FUNC,     FALSE,  ST_ORD,    VNINTER},
               {"GETP_MBS",          ST_FUNC,     FALSE,  ST_ORD,    VGPMBS},
               {"UPDP_MBS",          ST_PROC,     FALSE,  ST_ORD,    VUPMBS},
               {"SUR_EXDEF",         ST_PROC,     FALSE,  ST_GEO,    VSUREX},
               {"CUR_SIL",           ST_PROC,     FALSE,  ST_GEO,    VCURSIL},
               {"CUR_INT",           ST_PROC,     FALSE,  ST_GEO,    VCURINT},
               {"CUR_ISO",           ST_PROC,     FALSE,  ST_GEO,    VCURISO},
               {"NORM",              ST_FUNC,     FALSE,  ST_ORD,    VNORM},
               {"TFORM_MOVE",        ST_PROC,     FALSE,  ST_GEO,    VTFMOV},
               {"TFORM_ROTP",        ST_PROC,     FALSE,  ST_GEO,    VTFROTP},
               {"TFORM_ROTL",        ST_PROC,     FALSE,  ST_GEO,    VTFROTL},
               {"TFORM_MIRR",        ST_PROC,     FALSE,  ST_GEO,    VTFMIRR},
               {"TFORM_COMP",        ST_PROC,     FALSE,  ST_GEO,    VTFCOMP},
               {"TFORM_USRDEF",      ST_PROC,     FALSE,  ST_GEO,    VTFU},
               {"TCOPY",             ST_PROC,     FALSE,  ST_GEO,    VTCOPY},
               {"CUR_GL",            ST_PROC,     FALSE,  ST_GEO,    VCURGL},
               {"MSIZE_VIEW",        ST_PROC,     FALSE,  ST_ORD,    VMSIZE},
               {"CRE_WIN",           ST_FUNC,     FALSE,  ST_ORD,    VCRWIN},
               {"CRE_EDIT",          ST_FUNC,     FALSE,  ST_ORD,    VCREDI},
               {"CRE_ICON",          ST_FUNC,     FALSE,  ST_ORD,    VCRICO},
               {"CRE_BUTTON",        ST_FUNC,     FALSE,  ST_ORD,    VCRBUT},
               {"SHOW_WIN",          ST_PROC,     FALSE,  ST_ORD,    VSHWIN},
               {"WAIT_WIN",          ST_FUNC,     FALSE,  ST_ORD,    VWTWIN},
               {"GET_EDIT",          ST_FUNC,     FALSE,  ST_ORD,    VGTEDI},
               {"UPD_EDIT",          ST_PROC,     FALSE,  ST_ORD,    VUPEDI},
               {"GET_BUTTON",        ST_FUNC,     FALSE,  ST_ORD,    VGTBUT},
               {"DEL_WIN",           ST_PROC,     FALSE,  ST_ORD,    VDLWIN},
               {"GETCURH",           ST_PROC,     FALSE,  ST_ORD,    VGTCUH},
               {"GETSEG",            ST_PROC,     FALSE,  ST_ORD,    VGTSEG},
               {"GETSURH",           ST_PROC,     FALSE,  ST_ORD,    VGTSUH},
               {"UPDPP_MBS",         ST_PROC,     FALSE,  ST_ORD,    VUPPMBS},
               {"SUR_CONIC",         ST_PROC,     FALSE,  ST_GEO,    VSURCON},
               {"SUR_OFFS",          ST_PROC,     FALSE,  ST_GEO,    VSUROF},
               {"INV_ON_SUR",        ST_FUNC,     FALSE,  ST_ORD,    VIONSUR},
               {"N_CUR_INT",         ST_FUNC,     FALSE,  ST_ORD,    VNCURI},
               {"EVAL",              ST_FUNC,     FALSE,  ST_ORD,    VEVAL},
               {"GET_VIEW",          ST_PROC,     FALSE,  ST_ORD,    VGTVIEW},
               {"DEL_MBS",           ST_PROC,     FALSE,  ST_ORD,    VDELMBS},
               {"ACT_CSY",           ST_FUNC,     FALSE,  ST_ORD,    VACTCSY},
               {"MODE_BASIC",        ST_PROC,     FALSE,  ST_ORD,    VMODBA},
               {"SUR_SPLARR",        ST_PROC,     FALSE,  ST_GEO,    VSURSA},
               {"SUR_CONARR",        ST_PROC,     FALSE,  ST_GEO,    VSURCA},
               {"SUR_APPROX",        ST_PROC,     FALSE,  ST_GEO,    VSURAP},
               {"SUR_COMP",          ST_PROC,     FALSE,  ST_GEO,    VSURCO},
               {"SUR_TRIM",          ST_PROC,     FALSE,  ST_GEO,    VSURT},
               {"GETTOPP",           ST_PROC,     FALSE,  ST_ORD,    VGTTOP},
               {"GETCUBP",           ST_PROC,     FALSE,  ST_ORD,    VGTCUP},
               {"GETRATP",           ST_PROC,     FALSE,  ST_ORD,    VGTRAP},
               {"CUR_SPLARR",        ST_PROC,     FALSE,  ST_GEO,    VCURSA},
               {"CUR_CONARR",        ST_PROC,     FALSE,  ST_GEO,    VCURCA},
               {"CUR_FANGA",         ST_PROC,     FALSE,  ST_GEO,    VCURFA},
               {"CUR_GEODESIC",      ST_PROC,     FALSE,  ST_GEO,    VCURGD},
               {"CUR_APPROX",        ST_PROC,     FALSE,  ST_GEO,    VCURAP},
               {"RUN_MBS",           ST_PROC,     FALSE,  ST_ORD,    VRUNMBS},
               {"*******",           ST_FUNC,     FALSE,  ST_ORD,    VAPID},
               {"ACT_VARKON_VERS",   ST_FUNC,     FALSE,  ST_ORD,    VAVVERS},
               {"*****************", ST_FUNC,     FALSE,  ST_ORD,    VAVSER},
               {"ACT_OSTYPE",        ST_FUNC,     FALSE,  ST_ORD,    VAOSTYP},
               {"ACT_HOST",          ST_FUNC,     FALSE,  ST_ORD,    VAHOST},
               {"UNIQUE_FILENAME",   ST_FUNC,     FALSE,  ST_ORD,    VUFNAM},
               {"RANDOM",            ST_FUNC,     FALSE,  ST_ORD,    VRAND},
               {"ALL_INT_LIN_SUR",   ST_PROC,     FALSE,  ST_ORD,    VALLILS},
               {"ALL_INV_ON_SUR",    ST_PROC,     FALSE,  ST_ORD,    VALLIOS},
               {"APPEND_FILE",       ST_PROC,     FALSE,  ST_ORD,    VAPPFIL},
               {"DELETE_FILE",       ST_PROC,     FALSE,  ST_ORD,    VDELFIL},
               {"MOVE_FILE",         ST_PROC,     FALSE,  ST_ORD,    VMOVFIL},
               {"COPY_FILE",         ST_PROC,     FALSE,  ST_ORD,    VCOPFIL},
               {"TEST_FILE",         ST_FUNC,     FALSE,  ST_ORD,    VTSTFIL},
               {"GET_ENVIRONMENT",   ST_FUNC,     FALSE,  ST_ORD,    VGETENV},
               {"GRID_VIEW",         ST_PROC,     FALSE,  ST_ORD,    VGRID},
               {"GRIDX_VIEW",        ST_PROC,     FALSE,  ST_ORD,    VGRIDX},
               {"GRIDY_VIEW",        ST_PROC,     FALSE,  ST_ORD,    VGRIDY},
               {"GRIDDX_VIEW",       ST_PROC,     FALSE,  ST_ORD,    VGRIDDX},
               {"GRIDDY_VIEW",       ST_PROC,     FALSE,  ST_ORD,    VGRIDDY},
               {"SET_ROOT_GM",       ST_PROC,     FALSE,  ST_ORD,    VSRTGM},
               {"GET_NEXT_GM",       ST_FUNC,     FALSE,  ST_ORD,    VGNXGM},
               {"CRE_FBUTTON",       ST_FUNC,     FALSE,  ST_ORD,    VCRFBUT},
               {"CRE_FICON",         ST_FUNC,     FALSE,  ST_ORD,    VCRFICO},
               {"GET_WIN",           ST_PROC,     FALSE,  ST_ORD,    VGETWIN},
               {"TEXTL_WIN",         ST_FUNC,     FALSE,  ST_ORD,    VTXTLW},
               {"TEXTH_WIN",         ST_FUNC,     FALSE,  ST_ORD,    VTXTHW},
               {"LOAD_MDF",          ST_PROC,     FALSE,  ST_ORD,    VLDMDF},
               {"BOX_1P",            ST_PROC,     FALSE,  ST_GEO,    VBOX1P},
               {"BOX_ENCLOSE",       ST_PROC,     FALSE,  ST_GEO,    VBOXEN},
               {"SUR_ROT",           ST_PROC,     FALSE,  ST_GEO,    VSURROT},
               {"CRE_DBUTTON",       ST_FUNC,     FALSE,  ST_ORD,    VCRDBUT},
               {"ENDP",              ST_FUNC,     FALSE,  ST_ORD,    VEND},
               {"STARTP",            ST_FUNC,     FALSE,  ST_ORD,    VSTART},
               {"LOAD_DLL",          ST_FUNC,     FALSE,  ST_ORD,    VLDDLL},
               {"CALL_DLL_FUNCTION", ST_PROC,     FALSE,  ST_ORD,    VCLDLL},
               {"UNLOAD_DLL",        ST_PROC,     FALSE,  ST_ORD,    VULDLL},
               {"",                  ST_UNDEF,    FALSE,  ST_ORD,    ST_UNDEF}
              };
#else
   ;
#endif

static  struct syspf addtab3[ ] =         /* System names  - add on table 3 */
               {
#include "../include/newrout.h"
               };

/*
***l o c a l    r o u t i n e s 
*/
static short  setbase(pm_ptr ref);
static void   resbase();
static short  sysrou(char *name, stidcl class, bool cnst, stprcl pr_cl,
                     pmroutn id);
static short  insert(pm_ptr ref);
static pm_ptr find(char *name, short lev);


/********************************************************/
/*!******************************************************/

        short stgini() 

/*      Initiates the global symbol table.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985 Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *      2001-04-20 #ifdef VARKON, J.Kjellander
 *
 ******************************************************!*/

  {
   STTYTBL     *tp;           /* Pointer to type table entry     */
   pm_ptr      pmtp;          /* PM pointer to type table entry  */
   pmroutn     r;             /* For loop index                  */
   short       status;        /* Error severity code             */

   currlev = 0;
   mlevel[currlev].bla = pmgbla();
   mlevel[currlev].first = (pm_ptr)NULL;

/*
***For VARKON, insert the first part of the system routines.
*/
#ifdef VARKON
   for (r = 0; systab[r].pfid != ST_UNDEF;  r++)
       if ((status = sysrou(systab[r].name, 
                            systab[r].class, 
                            systab[r].cnst,
                            systab[r].proc_cl,
                            systab[r].pfid)) != 0)
             return(status);
#endif
/*
***For all systems, create the system types.
*/
   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0) /* Integer */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty  = v2intsz;
   tp-> kind_ty  = ST_INT;
   tp-> arr_ty   = (pm_ptr)NULL;
   stintp = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0) /* Float */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty  = v2flosz;
   tp-> kind_ty  = ST_FLOAT;
   tp-> arr_ty   = (pm_ptr)NULL;
   stflop = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0) /* Vector */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty  = v2vecsz;
   tp-> kind_ty  = ST_VEC;
   tp-> arr_ty   = (pm_ptr)NULL;
   stvecp = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0) /* Reference */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty  = v2refsz;
   tp-> kind_ty  = ST_REF;
   tp-> arr_ty   = (pm_ptr)NULL;
   strefp = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0)  /* File */   
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty  = v2filesz;
   tp-> kind_ty  = ST_FILE;
   tp-> arr_ty   = (pm_ptr)NULL;
   stfilp = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0) /* Dummy string */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty = 0;
   tp-> kind_ty = ST_STR;
   tp-> arr_ty = (pm_ptr)NULL;
   ststrp = -pmtp;

   if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0)  /* Boolean  */
         return(status);
   tp = (STTYTBL *) pmgadr(pmtp);
   tp-> size_ty = v2boolsz;
   tp-> kind_ty = ST_BOOL;
   tp-> arr_ty = (pm_ptr)NULL;
   stbolp = -pmtp;
/*
***For all systems, insert the global variables.
*/
   stgldz = 0;

   if((status = stcvar("INPUT", stfilp, ST_GLOVA, stgldz,
                       (DBint)NULL, &pmtp)) != 0) return(status);

   stgldz += ((v2filesz % MIN_BLKS) != 0) ?
             ((v2filesz + MIN_BLKS) / MIN_BLKS) * MIN_BLKS :
               v2filesz ;

   if((status = stcvar("OUTPUT", stfilp, ST_GLOVA, stgldz,
                       (DBint)NULL, &pmtp)) != 0) return(status);

   stgldz += ((v2filesz % MIN_BLKS) != 0) ?
             ((v2filesz + MIN_BLKS) / MIN_BLKS)*MIN_BLKS :
               v2filesz ;
/*
***For Varkon, insert more functions/procedures.
*/
#ifdef VARKON
   for (r = 0; addtab1[r].pfid != ST_UNDEF;  r++)
       if ((status = sysrou(addtab1[r].name, 
                            addtab1[r].class, 
                            addtab1[r].cnst,
                            addtab1[r].proc_cl,
                            addtab1[r].pfid)) != 0)
             return(status);
#endif
/*
***For all systems, insert array types.
*/
   stdarr(4, 1, stflop, &pmtp);        /* 4*4 matrix */
   stdarr(4, 1, -pmtp, &pmtp);
   st_j_p = -pmtp;

   stdarr(0, 10, st_j_p, &pmtp);       /* conformant 4*4 matrix array */
   st_k_p = -pmtp;

   stdarr(0, 10, stflop, &pmtp);       /* conformant float array */
   st_l_p = -pmtp;

   stdarr(0, 10, strefp, &pmtp);       /* conformant ref array  */
   st_m_p = -pmtp;

/*
***For Varkon,insert more functions/procedures.
*/
#ifdef VARKON
   for (r = 0; addtab2[r].pfid != ST_UNDEF;  r++)
       if ((status = sysrou(addtab2[r].name, 
                            addtab2[r].class, 
                            addtab2[r].cnst,
                            addtab2[r].proc_cl,
                            addtab2[r].pfid)) != 0)
             return(status);
#endif
/*
***For all systems, insert some more array types. 950227 JK.
*/
   stdarr(0, 10, stvecp, &pmtp);       /* conformant vector array  */
   st_n_p = -pmtp;

   stdarr(0, 10, ststrp, &pmtp);       /* conformant string array  */
   st_o_p = -pmtp;

   stdarr(0, 10, stintp, &pmtp);       /* conformant int array 040714 JK  */
   st_p_p = -pmtp;
/*
***For all systems, insert global variable PI.
*/
   if ( (status=stcvar("PI",stflop,ST_GLOVA,stgldz,(DBint)NULL,&pmtp)) != 0 )
       return(status);

   stgldz += ((v2flosz % MIN_BLKS) != 0) ?
             ((v2flosz + MIN_BLKS) / MIN_BLKS)*MIN_BLKS :
               v2flosz ;
/*
***For all systems, insert final table of functions/procedures.
*/
   for (r = 0; addtab3[r].pfid != ST_UNDEF;  r++)
       if ((status = sysrou(addtab3[r].name, 
                            addtab3[r].class, 
                            addtab3[r].cnst,
                            addtab3[r].proc_cl,
                            addtab3[r].pfid)) != 0)
             return(status);
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short stlini(
        pm_ptr   pmidp,
        char    *name,
        pm_ptr  *newp)

/*      Initiates a local symbol table.
 *
 *      In:    pmidp   =>  PM pointer to the first symbol.
 *            *name    =>  Pointer to the module name.
 *
 *      Out:  *newp    =>  PM pointer to the first symbol in table.
 *                         If "idp" != NULL "newp" = * idp".
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL   *mod_p;           /* C-pointer to the module name entry  */
   STIDTBL   *id_p;            /* C-pointer identifier entries        */
   pm_ptr    pmnp;             /* PM pointer to the creted name       */
   short     status;           /* Severity code                       */
   stidcl    idclass;          /* Identifier classification           */
   string    np;               /* C pointer to module name            */


   baseptr = 0;               /* Nested read calls  */
   currlev = 1;               /* User module level  */
   mlevel[currlev].bla = pmgbla();
                                 
   if (pmidp == (pm_ptr)NULL)  /* No symbol table exists. Create a module entry */
        {
           if ((status = pmallo((DBint)sizeof(*mod_p), &pmidp)) != 0)
                return(status);
           if ((status = pmcstr("%module", &pmnp)) != 0)
                return(status);

           mod_p = (STIDTBL *) pmgadr(pmidp);     /* Get C - address  */
           mod_p->name_id  = pmnp;
           mod_p->kind_id  = ST_MOD;
           mod_p->llink_id = (pm_ptr)NULL;
           mod_p->rlink_id = (pm_ptr)NULL;
           mod_p->next_id  = (pm_ptr)NULL;
        }
   else
        {
           if ((status = stratt(pmidp, &idclass, &np)) != 0)
                 return(status);
/*
***I samband med kompilering för V1.8 på DS90 upptäcktes det ensamma
***likhetstecknet nedan. Eftersom testen ändå inte resulterar i något
***vettigt resultat kunde man lika gärna ta bort alltihop.
*/
/*           if (idclass = ST_MOD) JK890412 */
           if (idclass == ST_MOD)
                {
                   if (sticmp(name, np) != 0)     /* Inconsistent names */
/* NOTE! */        ;    /* return(erpush("ST0013", np)); */ 
                }
           else return(erpush("ST0024", ""));      /* Not a module */
        }

   mlevel[currlev].first = pmidp;
   *newp = pmidp;

        /* Get last identifier in the table */

   while (pmidp != (pm_ptr)NULL)
      {
       lastid = pmidp;
       id_p = (STIDTBL *) pmgadr(pmidp);
       pmidp = id_p->next_id;
      }

   return(0);
}

/********************************************************/
/*!******************************************************/

        short  sticmp(
        char  *s1,
        char  *s2)


/*      Compares two identifiers. Upper & Lower case is not significant.
 *
 *      In:   *s1      =>  String I.
 *            *s2      =>  String II.
 *
 *      FV:    Return  =>  0 = equal, <0 = s1 < s2, >0 s1 > s2
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char c1,c2;

   for ( ; (c1 = (isilower(*s1) ? toiupper(*s1) : *s1)) ==
           (c2 = (isilower(*s2) ? toiupper(*s2) : *s2)) ;  s1++, s2++ )
         {
         if ( c1 == '\0' ) return(0);
         }
   return(c1 - c2);
  }

/********************************************************/
/*!******************************************************/

        short stlook(
        char    *name,
        stidcl  *kind,
        pm_ptr  *ref)

/*      Search the symbol table for a given name.
 *
 *      In:   *name    =>  Pointer to the name string.
 *
 *      In:   *kind    =>  Name type. ST_UNDEF if not found.
 *            *ref     =>  Entry point. NULL if not found.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL     *id_p;     /* C-pointer to the name entry */
   short       lev;       /* Current block level         */

   for (lev = currlev; lev >= 0; lev--)
      if ((*ref = find(name, lev)) != (pm_ptr)NULL)
         {
           setbase(*ref);
           id_p = (STIDTBL *) pmgadr(abs(*ref));     /* Get address */
           *kind = id_p->kind_id;
           resbase();
           return(0);
         }
   *kind = ST_UNDEF;
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short stratt(
        pm_ptr    ref,
        stidcl   *kind,
        char    **name)

/*      Read common name attributes.
 *
 *      In:   *ref     =>  Symbol table name reference (STIDTBL).
 *
 *      In:   *kind    =>  Name type.
 *           **name    =>  Pointer to the name string.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL    *id_p;     /* C pointer to structure        */

   if (ref != 0)
        {
           setbase(ref);
           if ( ref < 0 ) id_p = (STIDTBL *)pmgadr(-ref);
           else           id_p = (STIDTBL *)pmgadr(ref);
          *kind = id_p->kind_id;
           if ( id_p->name_id < 0 ) *name = pmgadr(-id_p->name_id);
           else                     *name = pmgadr(id_p->name_id);
           resbase();
           return(0);
        }
   else  return(erpush("ST0074", ""));   /* NULL pointer reference */
}

/********************************************************/
/*!******************************************************/

        short stgnid(
        pm_ptr   prev,
        stidcl  *kind,
        char   **name,
        pm_ptr  *ref)

/*      Get next identifier.
 *
 *      In:   *prev    =>  Previous id. reference. NULL if first 
 *                         identifier is wanted. 
 *
 *      Out:  *kind    =>  "Next" name type.
 *           **name    =>  "Next" name pointer.
 *            *ref     =>  Symbol table entry ptr. for "next".
 *                             NULL if previous was the last.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL    *id_p;     /* C pointer to structure        */
   short      status;    /* Error severity code           */


   if (prev == 0)
        *ref = mlevel[currlev].first;
   else 
/*
***Not the first symbol in the table
*/
        {
         setbase(prev);
         id_p = (STIDTBL *) pmgadr(abs(prev));
         *ref = id_p->next_id;
         resbase();
        }
/*
***Get attributes
*/
   if (*ref != 0)
        {
         setbase(*ref);
         id_p = (STIDTBL *) pmgadr(abs(*ref));
         *kind = id_p-> kind_id;
         if ((status = pmgstr(abs(id_p->name_id), name)) != 0)
               return(status);
         resbase();
        }
   else  *kind = ST_UNDEF;
  
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short stdtyp(
        sttycl   typ,
        short    strlen,
        pm_ptr  *ref)

/*      Define a primitive type (not compound).
 *
 *      In:    typ     =>  Type classification
 *             strlen  =>  String length. 0 if not applicable.
 *
 *      Out:  *ref     =>  Reference to the type entry in PM
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL     *tp;       /* C pointer to type entry   */
   pm_ptr      pmtp;      /* PM pointer to type entry  */
   short       status;    /* Error severity code       */


   switch (typ) 
      {
      case ST_INT:        /* Integer type */
         *ref = stintp;
         break;

      case ST_FLOAT:      /* Float type  */
         *ref = stflop;
         break;

      case ST_STR:        /* Allocate the string type */
         if (strlen != 0)
            {
            if ((status = pmallo((DBint)sizeof(*tp), &pmtp)) != 0)
               return(status);
            tp = (STTYTBL *) pmgadr(pmtp);
            tp-> size_ty = (strlen+1)*v2chsz;
            tp-> kind_ty = ST_STR;
            tp-> arr_ty  = (pm_ptr)NULL;
            }
         else pmtp = ststrp;

         *ref = pmtp;
         break;

      case ST_VEC:        /* Vector type */
         *ref = stvecp;
         break;

      case ST_REF:        /* Reference (identification) type */
         *ref = strefp;
         break;

      case ST_FILE:       /* File type   */
         *ref = stfilp;
         break;

      case ST_BOOL:       /* Boolean type */
         *ref = stbolp;
         break;

      case ST_ARR:        /* Array type is illegal */
         return(erpush("ST0034", ""));

      default:            /* Unknown constant type  */
         return(erpush("ST0044", ""));
      }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short stdarr(
        int     up,
        int     low,
        pm_ptr  base,
        pm_ptr *ref)

/*      Define an array type
 *
 *      In:    up      =>  Upper bound
 *             low     =>  Lower bound.
 *             base    =>  Reference to the component type.
 *
 *      Out:  *ref     =>  Reference to the type entry in PM
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *      2004-07-12 up&low->int, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   pm_ptr       pmarrp;    /* PM pointer to the array descriptor    */
   STTYTBL      *tp;       /* C pointer to type entry               */
   STARRTY      *arrp;     /* C pointer to array entry              */
   int          bassz;     /* Size of the base type in bytes        */
   STTYTBL      bastyp;    /* Access data for base type             */
   short        status;    /* Error severity code                   */

/*
***Create the array descriptor
*/
   if ((status = pmallo((DBint)sizeof(*arrp), &pmarrp)) != 0)
        return(status);
   arrp = (STARRTY *) pmgadr(pmarrp);
   arrp-> low_arr = low;
   arrp-> up_arr = up;
   arrp-> base_arr = base;

/*
***Create the array type
*/
   if ((status = pmallo((DBint)sizeof(*tp), ref)) != 0)
        return(status);
   tp = (STTYTBL *) pmgadr(*ref);

   if (base == 0)            /* Get the size of the base type */
        bassz = 0;
   else
        {
          if ((status = strtyp(base, &bastyp)) != 0)
               return(status);
          bassz = bastyp.size_ty;
        }

   tp-> size_ty = (up-low+1) * bassz;
   tp-> kind_ty = ST_ARR;
   tp-> arr_ty  = (currlev == 0) ? -pmarrp : pmarrp;
   
   return (0);
  }

/********************************************************/
/*!******************************************************/

        short strtyp(
        pm_ptr    ref,
        STTYTBL *type)

/*      Read type attributes.
 *
 *      In:    ref     =>  Reference to the type object
 *
 *      Out:  *type    =>  Type attributes
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL     *tp;      /* C -pointer to the type descriptor  */

   if (ref != 0)
        {
           setbase(ref);
           if ( ref < 0 ) tp = (STTYTBL *)pmgadr(-ref);
           else           tp = (STTYTBL *)pmgadr(ref);
           type-> size_ty  = tp-> size_ty;
           type-> kind_ty  = tp-> kind_ty;
           type-> arr_ty   = tp-> arr_ty;
           resbase();
           return(0);
        }
   else return(erpush("ST0074", ""));   /* NULL pointer reference */
  }

/********************************************************/
/*!******************************************************/

        short strarr(
        pm_ptr    ref,
        STARRTY  *arr)

/*      Read array type attributes.
 *
 *      In:    ref     =>  Reference to the array descriptor
 *
 *      Out:  *arr     =>  Array attributes
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STARRTY     *arrp;    /* C -pointer to the array descriptor  */

   if (ref != 0)
        {
           setbase(ref);
           if ( ref < 0 ) arrp = (STARRTY *)pmgadr(-ref);
           else           arrp = (STARRTY *)pmgadr(ref);
           arr-> low_arr  = arrp-> low_arr;
           arr-> up_arr   = arrp-> up_arr;
           arr-> base_arr = arrp-> base_arr;
           resbase();
           return(0);
        }
   else  return(erpush("ST0074", ""));  /* NULL pointer reference */
  }

/********************************************************/
/*!******************************************************/

        short stcvar(
        char    *name,
        pm_ptr   type,
        stvarcl  kind,
        DBint    addr,
        pm_ptr   defval,
        pm_ptr  *ref)

/*      Create a symbol table variable entry.
 *
 *      In:    name    =>  Pointer to the name string
 *             type    =>  Pointer to the type defin. (STTYTBL)
 *             kind    =>  Variable classification.
 *             addr    =>  Run time address.
 *             defval  =>  Default value for parameters.
 *
 *      Out:  *ref     =>  Created entry. 0 if not found
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr     pmnp;       /* PM pointer to name string      */
   stidcl     idclass;    /* Identifier type                */
   STIDTBL    *var_p;     /* C-pointer to a variable        */
   short      status;     /* Error severity code            */

   if ((*ref = find(name, currlev)) == (pm_ptr)NULL)       /* Symbol not found */
        {
           if ((status = pmallo((DBint)sizeof(*var_p), ref)) != 0)
                return(status);
           if ((status = pmcstr(name, &pmnp)) != 0)
                return(status);
                                        
           var_p = (STIDTBL *) pmgadr(*ref);
           var_p-> name_id = pmnp;
           var_p-> kind_id = ST_VAR;
           var_p-> class_id.var_id.vtyp_id = type;
           var_p-> class_id.var_id.vadr_id = addr;
           var_p-> class_id.var_id.vkind_id = kind;
           var_p-> class_id.var_id.vdef_id = defval;
           var_p-> llink_id = (pm_ptr)NULL;
           var_p-> rlink_id = (pm_ptr)NULL;
           var_p-> next_id = (pm_ptr)NULL;

           if(kind == ST_GLOVA)
               *ref = -(*ref);
               
           return(insert(*ref));
        }
   else
/*
***Name is already there
*/
        { 
           if ((status = stratt(*ref, &idclass, &name)) != 0)
                return(status);
           *ref = (pm_ptr)NULL;
           if (idclass == ST_VAR)
                return(erpush("ST0052", name));  /* Variable already declared */
           else 
/*
***Ambiguous definitions
*/
                return(erpush("ST0062", name));
        }
  }

/********************************************************/
/*!******************************************************/

        short strvar(
        pm_ptr  ref,
        STVAR  *var)

/*      Read variable attributes.
 *
 *      In:    name    =>  Variable reference in PM
 *
 *      Out:  *ref     =>  Variable record
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL     *var_p;   /* C pointer to the variable in sym. table. */
   STTYTBL     vartyp;   /* Variable type record                     */
   int         varsz;    /* Variable size. Fetched from the type     */
   short       status;   /* Error severity code                      */


   if (ref != (pm_ptr)NULL)
        {
           setbase(ref);
           var_p = (STIDTBL *) pmgadr(abs(ref));
           if (var_p-> kind_id == ST_VAR)
                {
/*
***Determine the variable size
*/
                   if (var_p-> class_id.var_id.vtyp_id != 0)
                        {
                          if ((status = strtyp(var_p->class_id.var_id.vtyp_id, 
                                               &vartyp)) != 0)
                                return(status);
                          varsz = vartyp.size_ty;
                        }
                   else
                        varsz = 0;

                   var-> type_va  = var_p-> class_id.var_id.vtyp_id;
                   var-> addr_va  = var_p-> class_id.var_id.vadr_id;
                   var-> kind_va  = var_p-> class_id.var_id.vkind_id;
                   var-> def_va   = var_p-> class_id.var_id.vdef_id; 
                   var-> size_va  = varsz;
                }
           else return(erpush("ST0084", ""));  /* Not a variable */
           resbase();
        }
   else  return(erpush("ST0074", ""));  /* NULL pointer reference */
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short stuvar(
        pm_ptr  ref,
        STVAR  *var)

/*      Update variable attributes.
 *
 *      In:    name    =>  Variable reference in PM
 *
 *      Out:  *ref     =>  Variable record
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL     *var_p;   /* C pointer to the variable in sym. table. */

   if (ref != (pm_ptr)NULL)
        {
           setbase(ref);
           var_p = (STIDTBL *) pmgadr(abs(ref));
           if (var_p-> kind_id == ST_VAR)
                {
                   var_p-> class_id.var_id.vtyp_id = var->type_va;
                   var_p-> class_id.var_id.vadr_id = var-> addr_va;
                   var_p-> class_id.var_id.vkind_id = var-> kind_va;
                   var_p-> class_id.var_id.vdef_id = var-> def_va;
                }
           else  return(erpush("ST0084", "")); /* Not a variable */
           resbase();
        } 
   else  return(erpush("ST0074", ""));   /* NULL pointer reference */

   return(0);
}

/********************************************************/
/*!******************************************************/

        short stccon(
        char    *name,
        PMLITVA *value,
        pm_ptr  *ref)

/*      Create a symbol table entry for a named constant.
 *
 *      In:   *name    =>  Pointer to the name string
 *            *value   =>  Ref. to constant value. NULL otherwise
 *
 *      Out:  *ref     =>  Created entry. NULL if not created.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl    idclass;        /* Identifier type               */
   sttycl    typ;            /* Type class                    */
   short     status;         /* Error severity code           */
   pm_ptr    tp;             /* PM pointer to the type def    */
   pm_ptr    lit_p;          /* Pm pointer to literal expr.   */
   pm_ptr    pmnp;           /* PM ptr to the name string     */
   STIDTBL   *cnst_p;        /* C - pointer to constant       */


   if ((*ref = find(name, currlev)) == (pm_ptr)NULL)     /* Name is not there */
       {
       if ((status = pmallo((DBint)sizeof(*cnst_p), ref)) != 0)
            return(status);
       if ((status = pmcstr(name, &pmnp)) != 0)
            return(status);

       if (value != NULL)
          {
          switch (value->lit_type)
             {
             case C_INT_VA:     /* Integer literal */
                typ = ST_INT;
                break; 
         
             case C_FLO_VA:     /* Floating point literal */
                typ = ST_FLOAT;
                break;

             case C_STR_VA:    /* String literal */
                typ = ST_STR;
                break;

             case C_VEC_VA:    /* Vector literal */
                typ = ST_VEC;
                break;

             case C_REF_VA:    /* Reference literal */
                typ = ST_REF;
                break;

             default: /* Unknown constant type */
                return(erpush("ST0114", ""));  
             }
               
          if ((status = stdtyp(typ, 0, &tp)) != 0)
             return(status);
/*
***Create the literal expression
*/
          if ((status = pmclie(value, &lit_p)) != 0)
               return(status);
          }
       else
          {
          tp = (pm_ptr)NULL;
          lit_p = (pm_ptr)NULL;
          }
/*
***Fill in the values in the internal form
*/
       cnst_p = (STIDTBL*) pmgadr(*ref);
       cnst_p ->name_id = pmnp;
       cnst_p ->kind_id = ST_CONST;
       cnst_p ->class_id.const_id.ctyp_id = tp;
       cnst_p ->class_id.const_id.cval_id = lit_p;
       cnst_p ->llink_id = (pm_ptr)NULL;
       cnst_p ->rlink_id = (pm_ptr)NULL;
       cnst_p ->next_id = (pm_ptr)NULL;
   
       return(insert(*ref));
       }
   else
/*
***Name is already in the symbol table
*/
     {
     if ((status = stratt(*ref, &idclass, &name)) != 0)
            return(status);
        *ref = (pm_ptr)NULL;
        if (idclass == ST_CONST)
             return(erpush("ST0122", name));  /* Constant already declared */
        else /* Ambiguous definition */
             return(erpush("ST0062", name));
     }

  }

/********************************************************/

#ifndef DECOMP    /* don't compile if Decompiler */

/*!******************************************************/

        short strcon(
        pm_ptr    ref,
        STCONST  *konst)

/*      Read constant attributes.
 *
 *      In:    ref     =>  Pointer to the named constant in PM
 *
 *      Out:  *konst   =>  Named constant attributes.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL   *cnst_p;         /* C ptr. to the constant symbol tab. entry  */
   short     status;          /* Current error severity code               */
   int       size;            /* Constant size                             */
   STTYTBL   ctyp;            /* Constant type descriptor                  */
   pm_ptr    type;            /* 1999-03-26 JK                             */

   size = 0;
   if (ref != (pm_ptr)NULL) 
        {
         setbase(ref);
         cnst_p = (STIDTBL *) pmgadr(abs(ref));
         if (cnst_p-> kind_id == ST_CONST)
             {   
/*
***Pick up the attributes
*/
              if (cnst_p-> class_id.const_id.ctyp_id != (pm_ptr)NULL)
                   {  
/*
***Get the constant size
*/
                    if ((status = strtyp(cnst_p-> class_id.const_id.ctyp_id,
                                         &ctyp)) != 0)
                          return(status);
                    size = ctyp.size_ty;
                   }

              konst-> type_co = cnst_p-> class_id.const_id.ctyp_id;
              konst-> size_co = size;
              inevex(cnst_p-> class_id.const_id.cval_id, &konst-> valu_co,
                     &type);
             }
         else return(erpush("ST0154", ""));   /* Not a constant */
         resbase();
        }
   else return(erpush("ST074", ""));        /* Null pointer */

   return(0);
  }

/********************************************************/

#endif

/*!******************************************************/

        short stgcon(
        pm_ptr    ref,
        STCONST  *konst,
        pm_ptr   *valdef)

/*      Get constant declaration attributes.
 *      The routine is used by the Decompiler.
 *
 *      In:    ref     =>  Pointer to the named constant in PM
 *
 *      Out:  *konst   =>  Named constant attributes.
 *            *valdef  =>  Declared definition.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL   *cnst_p;         /* C ptr. to the constant symbol tab. entry  */
   short     status;          /* Current error severity code               */
   int       size;            /* Constant size                             */
   STTYTBL   ctyp;            /* Constant type descriptor                  */

   size = 0;
   if (ref != (pm_ptr)NULL) 
        {
         setbase(ref);
         cnst_p = (STIDTBL *) pmgadr(abs(ref));
         if (cnst_p-> kind_id == ST_CONST)
             {   
/*
***Pick up the attributes
*/
              if (cnst_p-> class_id.const_id.ctyp_id != (pm_ptr)NULL)
                   {  
/*
***Get the constant size
*/
                    if ((status = strtyp(cnst_p-> class_id.const_id.ctyp_id,
                                         &ctyp)) != 0)
                          return(status);
                    size = ctyp.size_ty;
                   }

              konst-> type_co = cnst_p-> class_id.const_id.ctyp_id;
              konst-> size_co = size;

              *valdef = cnst_p-> class_id.const_id.cval_id;
             }
         else return(erpush("ST0154", ""));   /* Not a constant */
         resbase();
        }
   else return(erpush("ST074", ""));          /* Null pointer */

   return(0);
}

/********************************************************/
/*!******************************************************/

        short stucon(
        pm_ptr    ref,
        STCONST  *konst)

/*      Update constant attributes.
 *
 *      In:    ref     =>  Pointer to the named constant in PM
 *            *konst   =>  Named constant attributes.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   sttycl    typ;            /* Type class                    */
   short     status;         /* Error severity code           */
   pm_ptr    tp;             /* PM pointer to the type def    */
   pm_ptr    lit_p;          /* Pm pointer to literal expr.   */
   STIDTBL   *cnst_p;        /* C - pointer to constant       */

   if (ref != (pm_ptr)NULL)
      {
      setbase(ref);
      cnst_p = (STIDTBL *) pmgadr(abs(ref));
      if (cnst_p-> kind_id == ST_CONST)
         {
         switch (konst-> valu_co.lit_type)
            {
            case C_INT_VA:     /* Integer literal */
               typ = ST_INT;
               break; 
         
            case C_FLO_VA:     /* Floating point literal */
               typ = ST_FLOAT;
               break;

            case C_STR_VA:    /* String literal */
               typ = ST_STR;
               break;

            case C_VEC_VA:    /* Vector literal */
               typ = ST_VEC;
               break;

            case C_REF_VA:    /* Reference literal */
               typ = ST_REF;
               break;

            default: /* Unknown constant type */
               return(erpush("ST0114", ""));  
            }
               
         if ((status = stdtyp(typ, 0, &tp)) != 0)
            return(status);
/*
***Create the literal expression
*/
        if ((status = pmclie(&(konst-> valu_co), &lit_p)) != 0)
           return(status);
    
/*
***Fill in the values in the internal form
*/
        cnst_p ->class_id.const_id.ctyp_id = tp;
        cnst_p ->class_id.const_id.cval_id = lit_p;
      }
      else return(erpush("ST0154", ""));       /* It is not a constant */
      resbase();
     }
   else return(erpush("ST0074", ""));          /* Null pointer */            
   
   return(0);
}

/********************************************************/
/*!******************************************************/

        short stclab(
        char   *name,
        pm_ptr *ref)

/*      Create a MBS label.
 *
 *      In:   *name    =>  Label name string
 *
 *      Out:  *ref     =>  Reference to the label entry.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl    idclass;        /* Identifier type               */
   short     status;         /* Error severity code           */
   pm_ptr    pmnp;           /* PM ptr to the name string     */
   STIDTBL   *lab_p;         /* C - pointer to label          */

   if ((*ref = find(name, currlev)) == (pm_ptr)NULL)     /* Name is not there */
       {
        if ((status = pmallo((DBint)sizeof(*lab_p), ref)) != 0)
             return(status);
        if ((status = pmcstr(name, &pmnp)) != 0)
             return(status);
/*
***Fill in the values in the internal form
*/
        lab_p = (STIDTBL*) pmgadr(*ref);
        lab_p ->name_id = pmnp;
        lab_p ->kind_id = ST_LABEL;
        lab_p ->class_id.label_id.ldef_id = FALSE;  /* not defined */
        lab_p ->class_id.label_id.lref_id = FALSE;  /* not referenced */
        lab_p ->llink_id = (pm_ptr)NULL;
        lab_p ->rlink_id = (pm_ptr)NULL;
        lab_p ->next_id = (pm_ptr)NULL;
   
        return(insert(*ref));
       }
   else  
/*
***Name is already in the symbol table
*/
       {
        if ((status = stratt(*ref, &idclass, &name)) != 0)
             return(status);
        *ref = (pm_ptr)NULL;
        if (idclass == ST_LABEL)
             return(erpush("ST0132", name));   /* Label already declared */
        else 
             return(erpush("ST0062", name));   /* Ambiguous definition */
       }
  }

/********************************************************/
/*!******************************************************/

        short strlab(
        pm_ptr     ref,
        STLABEL    *label)

/*      Read label attributes.
 *
 *      In:    ref     =>  Reference to the label entry.
 *
 *      Out:  *label   =>  Label attributes.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL     *lab_p;   /* C pointer to the label in sym. table. */

   if (ref != (pm_ptr)NULL)
        {
        setbase(ref);
        lab_p = (STIDTBL *) pmgadr(abs(ref));
        if (lab_p-> kind_id == ST_LABEL)
           {
           label-> list_la = lab_p-> class_id.label_id.llst_id;
           label-> node_la = lab_p-> class_id.label_id.lnod_id;
           label-> def_la  = lab_p-> class_id.label_id.ldef_id;
           label-> ref_la  = lab_p-> class_id.label_id.lref_id;
           }
        else return(erpush("ST0144", ""));  /* Not a label */
        resbase();
        }
   else  return(erpush("ST0074", ""));      /* NULL pointer reference */
   
   return(0);
}

/********************************************************/
/*!******************************************************/

        short stulab(
        pm_ptr    ref,
        STLABEL  *label)

/*      Update label attributes.
 *
 *      In:    ref     =>  Reference to the label entry.
 *
 *      Out:  *label   =>  New Label attributes.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STIDTBL     *lab_p;   /* C pointer to the label in sym. table. */

   if (ref != (pm_ptr)NULL)
      {
      setbase(ref);
      lab_p = (STIDTBL *) pmgadr(abs(ref));
      if (lab_p-> kind_id == ST_LABEL)
         {
         lab_p-> class_id.label_id.llst_id = label-> list_la;
         lab_p-> class_id.label_id.lnod_id = label-> node_la;
         lab_p-> class_id.label_id.ldef_id = label-> def_la;
         lab_p-> class_id.label_id.lref_id = label-> ref_la;
         }
       else  return(erpush("ST0144", "")); /* It is not a label */
         resbase();
      }
   else  return(erpush("ST0074", ""));        /* NULL pointer reference */
   
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short strrou(
        pm_ptr  ref,
        STPROC *rout)

/*      Read routine attributes.
 *
 *      In:    ref     =>  Reference to the label entry.
 *
 *      Out:  *rout    =>  Routine attributes.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short      status;      /* Error severity code      */
   STIDTBL    *rout_p;     /* C pointer to name table  */

   if (ref != (pm_ptr)NULL)
      {
      setbase(ref);
      rout_p = (STIDTBL *) pmgadr(abs(ref));
      if ((rout_p-> kind_id == ST_PROC) || 
         (rout_p-> kind_id == ST_FUNC))
         { 
         rout->kind_pr = rout_p->class_id.rout_id.fkind_id;
         rout->cnst_pr = rout_p->class_id.rout_id.fcnst_id;
         rout->class_pr = rout_p->class_id.rout_id.fpcl_id;
         }
      else
/*
***Not a procedure/function
*/
         status = erpush("ST0104", "");
         resbase();
      }
   else  return(erpush("ST0074", ""));    /* NULL pointer */

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short setbase(
        pm_ptr   ref)

/*      Set current base address.
 *
 *      In:    ref     =>  PM pointer to the symbol table.
 *                         If negative the systems table is meant.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if (baseptr <= STCALLMX)
      {
      prev_bas[baseptr] = pmgbla();
      baseptr++;
      if (ref < 0) 
         pmsbla(mlevel[0].bla);
      return( 0 );
      }
   else  return(erpush("ST0094", ""));   /* Stack overflow */
  }

/********************************************************/
/*!******************************************************/

        static void resbase()

/*      Restores the previously active base address.
 *
 *      FV:    Return  =>  None                               
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   baseptr--;
   pmsbla(prev_bas[baseptr]);
  }

/********************************************************/
/*!******************************************************/

        static short sysrou(
        string   name,
        stidcl   class,
        bool     cnst,
        stprcl   pr_cl,
        pmroutn  id)

/*      Create the system functions and procedures.
 *
 *      In:    name    =>  Routine name.
 *             class   =>  Procedure or Function.
 *             cnst    =>  TRUE if allowed in constant expressions.
 *             pr_cl   =>  Procedure class (Geometry/Ordinary).
 *             id      =>  Internal routine number.
 *
 *      FV:    Return  =>  Error severity code
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr     pmnp;        /* PM name pointer           */
   STIDTBL    *pf_p;       /* C - pointer to entry      */
   pm_ptr     pmpf;        /* PM pointer to entry       */
   short      status;      /* Error severity code       */


   if ((status = pmallo((DBint)sizeof(*pf_p), &pmpf)) != 0)
        return(status);
   if ((status = pmcstr(name, &pmnp)) != 0)
        return(status);
   pf_p = (STIDTBL *) pmgadr(pmpf);

   pf_p-> name_id  = -pmnp;
   pf_p-> kind_id  = class;
   pf_p-> class_id.rout_id.fcnst_id = cnst;
   pf_p-> class_id.rout_id.fpcl_id  = pr_cl;
   pf_p-> class_id.rout_id.fkind_id = id;
   pf_p-> llink_id = (pm_ptr)NULL;
   pf_p-> rlink_id = (pm_ptr)NULL;
   pf_p-> next_id = (pm_ptr)NULL;

   return(insert(-pmpf));
}

/********************************************************/
/*!******************************************************/

        static pm_ptr find(
        char  *name,
        short  lev)

/*      Search one level in the symbol table  for a name.
 *
 *      In:   *name    =>  Name string.
 *             lev     =>  Block search level.
 *
 *      FV:    Return  =>  Symbol table entry. NULL if not found
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr     pmidp;           /* PM pointer to the name table entry  */
   STIDTBL    *id_p;           /* C-pointer to the name table         */
   short      compa;           /* Result from string compare          */
   pm_ptr     oldbase;         /* Old base address                    */
   string     np;              /* C-pointer to a name string          */
   short      status;          /* Error severity code                 */

   oldbase = pmgbla();         /* Get current base address */
   pmsbla(mlevel[lev].bla);    /* Set base address             */
   pmidp = mlevel[lev].first;  /* Get first symbol in table    */

   while (pmidp != (pm_ptr)NULL)
      {
      id_p = (STIDTBL *) pmgadr(abs(pmidp));
      if ((status = pmgstr(abs(id_p->name_id), &np)) != 0)
         return(status);
      compa = sticmp(name, np);
      if (compa == 0) 
         {
         pmsbla(oldbase);
         return(pmidp);
         }
      else if (compa < 0) 
               pmidp = id_p -> llink_id;
      else pmidp = id_p -> rlink_id;
      }
   pmsbla(oldbase);
   return((pm_ptr)NULL);
  }

/********************************************************/
/*!******************************************************/

        static short insert(
        pm_ptr  ref)

/*      Insert a name entry in the symbol table.
 *
 *      In:   *name    =>  PM pointer to the new entry.
 *
 *      (C)microform ab 1985----- Kenth Ericson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr    pmidp;       /* PM pointer to the name table       */
   pm_ptr    prev;        /* PM pointer to the name table       */
   string    np;          /* Name string (pointer). To insert.  */
   string    np1;         /* Name string (pointer)              */
   STIDTBL   *id_p;       /* C-pointer to the name table        */
   short     compa;       /* Result after string comparision    */
   short     status;      /* Error severity codes               */
   bool      left;        /* Left or right side in insertion    */

   id_p = (STIDTBL *) pmgadr(abs(ref));
   if ((status = pmgstr(abs(id_p->name_id), &np)) != 0)
      return(status);

   pmidp = abs(mlevel[currlev].first);
   if (pmidp == (pm_ptr)NULL) 
      {
      mlevel[currlev].first = ref;
      lastid = ref;
      return( 0 );
     };

   do
     {
     prev = pmidp;
     id_p = (STIDTBL *) pmgadr(pmidp);
     if ((status = pmgstr(abs(id_p->name_id), &np1)) != 0)
        return(status);

     compa = sticmp(np, np1);
      
     if (compa < 0)
        {
        pmidp = abs(id_p->llink_id);
        left = TRUE;
        }
     else
        {
        pmidp = abs(id_p->rlink_id);
        left = FALSE;
        }
     }
   while (pmidp != (pm_ptr)NULL);
/*
***Insert the new name entry
*/
   id_p = (STIDTBL *) pmgadr(prev);
   if (left)
        id_p->llink_id = ref;
   else id_p->rlink_id = ref;

   id_p = (STIDTBL *) pmgadr(abs(lastid));
   id_p-> next_id = ref;
   lastid = ref;

   return(0);
  }

/********************************************************/
