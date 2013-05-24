/*!******************************************************************/
/*  File: stdef.h                                                   */
/*  =============                                                   */
/*                                                                  */
/*  Interface definitions for Symbol Table access routines.         */
/*                                                                  */
/*  This file includes the definition of the last part of the       */
/*  system global symbol table.                                     */
/*                                                                  */
/*  This file is part of the VARKON Pmpac Library.                  */
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
/*  (C)Microform AB 1984-2000, J. Kjellander johan@microform.se     */
/*                                                                  */
/********************************************************************/


#define  ST_UNDEF     0                   /* Common NULL value              */

/* ========================== T Y P E S ====================================*/
/*                                                                          */


/* --------------------------- sttycl --------------------------------------*/
/*                                                                          */
/* sttycl    - Classification of a type                                     */

typedef  char  sttycl;

#define  ST_INT       1               /* Integer type                       */
#define  ST_FLOAT     2               /* Float type                         */
#define  ST_STR       3               /* String type                        */
#define  ST_VEC       4               /* Vector type                        */
#define  ST_REF       5               /* Identification (reference) type    */
#define  ST_FILE      6               /* File type                          */
#define  ST_BOOL      7               /* Bool type (for internal use)       */
#define  ST_ARR       8               /* Array type                         */


/* --------------------------- STARRTY -------------------------------------*/
/*                                                                          */
/* STARRTY  - Definition of the array type                                  */

typedef struct starrty                 /* short->int 2004-07-12 JK */
   {
    int     low_arr;           /* Lower bound                              */
    int     up_arr;            /* Upper bound                              */
    pm_ptr  base_arr;          /* Component type                           */
   } STARRTY;


/* --------------------------- STTYTBL -------------------------------------*/
/*                                                                          */
/* STTYTBL  -  Type definition structure                                    */

typedef struct sttytbl                 /* Not for update*/
   {
    int      size_ty;          /* Size of object in bytes.                 */
    sttycl   kind_ty;          /* Type classification.                     */
    pm_ptr   arr_ty;           /* Array structure ref. NULL if no array    */
   }  STTYTBL;


/* ========================== N A M E S ====================================*/
/*                                                                          */


/* -------------------------- stidcl ---------------------------------------*/
/*                                                                          */
/* stidcl    - Classification of an identifier                              */

typedef   char  stidcl;

#define   ST_VAR      1               /* Variable                           */
#define   ST_CONST    2               /* Named constant                     */
#define   ST_LABEL    3               /* Label                              */
#define   ST_FUNC     4               /* Function                           */
#define   ST_PROC     5               /* Procedure                          */
#define   ST_MOD      6               /* Module                             */

/* -------------------------- stvarcl --------------------------------------*/
/*                                                                          */
/* stvarcl   - Classification of a variable                                 */

typedef  char stvarcl;

#define  ST_LOCVA       1               /* Local variable                   */
#define  ST_VPAVA       2               /* Value parameter                  */
#define  ST_RPAVA       3               /* Reference parameter              */
#define  ST_GLOVA       4               /* Global variable                  */

/* --------------------------- STVAR ---------------------------------------*/
/*                                                                          */
/* STVAR  -  Interface structure for a variable                             */

typedef struct stvar              
   {
    pm_ptr      type_va;        /* Pointer to the type descriptor (STTYTBL) */
    int         addr_va;        /* Address (offset) in data area            */
    stvarcl     kind_va;        /* Variable classification                  */
    int         size_va;        /* Size of the variable in bytes. No update.*/
    pm_ptr      def_va;         /* Default value for parameters.            */
   }  STVAR;

/* --------------------------- STCONST -------------------------------------*/
/*                                                                          */ 
/* STCONST  - Interface structure for a named constant                      */

typedef struct stconst
   {
    pm_ptr       type_co;       /* Pointer to the type descriptor (STTYTBL) */
    int          size_co;       /* Size of the constant in bytes. No update.*/
    PMLITVA      valu_co;       /* Value definition                         */
   }  STCONST;

/* --------------------------- STLABEL -------------------------------------*/
/*                                                                          */
/* STLABEL  - Interface structure for a label                               */

typedef struct stlabel
   {
    pm_ptr       list_la;       /* Reference to statement list              */
    pm_ptr       node_la;       /* Reference to statement list node         */
    bool         def_la;        /* TRUE -> defined                          */
    bool         ref_la;        /* TRUE -> referenced                       */
   }  STLABEL;

/* ------------------------- stprcl --------------------------------------- */
/*                                                                          */
/* stprcl    - Classification of procedures                                 */

typedef  char  stprcl;

#define  ST_GEO     1                       /* Geometric procedure          */
#define  ST_ORD     2                       /* Ordinary procedure           */

/* ------------------------- STPROC ----------------------------------------*/
/*                                                                          */
/* STPROC  - Interface structure for a procedure or a function              */

typedef struct stproc                /* No update    */
   {
    pmroutn    kind_pr;         /* Routine classification                   */
    bool       cnst_pr;         /* Can be included in constant expressions? */
    stprcl     class_pr;        /* procedure classification                 */
   }  STPROC;

/* --------------------------- stvalc --------------------------------------*/
/*                                                                          */
/* stvalc - Formal parameter class                                          */

typedef char stvalc;                 /* value class               */

#define ST_VARCL   0                 /* value class = variable    */
#define ST_CONCL   1                 /* value class = constant    */
#define ST_EXPCL   2                 /* value class = expression  */
#define ST_DEFCL   3                 /* value class = default     */

/* --------------------------- stvldc --------------------------------------*/
/*                                                                          */
/* stvldc - Formal parameter validity                                       */

typedef char stvldc;                 /* validity  code */

#define ST_OPT    'o'                /* validity code = optional  */
#define ST_FOR    'f'                /* validity code = forced    */
#define ST_ILL    'i'                /* validity code = illegal   */

/* -------------------------- STPARDAT -------------------------------------*/
/*                                                                          */
/* STPARDAT - Interface structure for a formal parameter                    */

typedef struct stpardat              /* parameter info. access struct */
    {
    stvldc valid;                    /* validity code                 */
    pm_ptr type;                     /* result value type. (STTYTBL)  */
    stvalc rvclass;                  /* result value class            */
    PMLITVA defval;                  /* default value - if any        */
    } STPARDAT;

/*
***Function prototypes.
*/

/*
***stacc.c
*/
short stgini();
short stlini(pm_ptr pmidp, char *name, pm_ptr *newp);
short sticmp(char *s1, char *s2);
short stlook(char *name, stidcl *kind, pm_ptr *ref);
short stratt(pm_ptr ref, stidcl *kind, char **name);
short stgnid(pm_ptr prev, stidcl *kind, char **name, pm_ptr *ref);
short stdtyp(sttycl typ, short strlen, pm_ptr *ref);
short stdarr(int up, int low, pm_ptr base, pm_ptr *ref);
short strtyp(pm_ptr ref, STTYTBL *type);
short strarr(pm_ptr ref, STARRTY *arr);
short stcvar(char *name, pm_ptr type, stvarcl kind, int addr,
             pm_ptr defval, pm_ptr *ref);
short strvar(pm_ptr ref, STVAR *var);
short stuvar(pm_ptr ref, STVAR *var);
short stccon(char *name, PMLITVA *value, pm_ptr *ref);
short strcon(pm_ptr ref, STCONST *konst);
short stgcon(pm_ptr ref, STCONST *konst, pm_ptr *valdef);
short stucon(pm_ptr ref, STCONST *konst);
short stclab(char *name, pm_ptr *ref);
short strlab(pm_ptr ref, STLABEL *label);
short stulab(pm_ptr ref, STLABEL *label);
short strrou(pm_ptr ref, STPROC *rout);

/*
***stac10.c
*/
void stgret(pm_ptr *rtype, pmvaty *ltype, short *geol);
void stgpar(short index, STPARDAT *par);
void stsrou(pmroutn rout);
