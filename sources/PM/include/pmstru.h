/*!******************************************************************/
/*  File: pmstru.h                                                  */
/*  ==============                                                  */
/*                                                                  */
/* Definition of MBS's internal form.                               */
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
/*  (C)Microform AB 1984-1999, J. Kjellander johan@microform.se     */
/*                                                                  */
/********************************************************************/
/* The internal form is described and defined with C-structures       */
/* Names on structures and elements are constructed from the          */
/* following abbreviations.                                           */
/*              AT = ATTRIBUTE                                        */
/*              CO = CONDITION                                        */
/*              EX = EXPRESSION                                       */
/*              GE = GEOMETRIC                                        */
/*              LI = LIST                                             */
/*              LO = Local                                            */
/*              MO = MODULE                                           */
/*              NO = NODE                                             */
/*              NP = NAMED PARAMETER                                  */
/*              PA = PARAMETER                                        */
/*              PM = Program Memory                                   */
/*              SB = SYMBOL TABLE                                     */
/*              ST = STATEMENT                                        */
/*              SY = System                                           */
/*              TC = Tail Concatenate                                 */
/*              TY = TYPE                                             */


/*  The internal form of MBS are built-up with the following node     */
/*  classes.                                                          */
/*              PMMONO - MODULE_NODE                                  */
/*              PMPANO - PARAM_NODE ( Module parameter node)          */
/*              PMSTNO - STAT_NODE (Statement node)                   */
/*              PMEXNO - EXPR_NODE (Expression node)                  */
/*              PMNPNO - NAMED_PARAM_NODE (Named parameter node)      */
/*              PMCONO - COND_NODE (Condition node)                   */
/*              PMLINO - LIST_NODE                                    */
/*              PMTCNO - TCONC_NODE (Tail Concatenate node            */

/**********************************************************************/
/*  Common 'typedef' and '#define' declarations for the Node classes. */

typedef char pmnocl;    /* Type for node class                               */
#define MODULE  1       /*  constant for MODULE_NODE                         */
#define PARAM   2       /*      - " -    PARAM_NODE                          */
#define STAT    3       /*      - " -    STAT_NODE                           */
#define EXPR    4       /*      - " -    EXPR_NODE                           */
#define NAMEPA  5       /*      - " -    NAME_PARAM_NODE                     */
#define LIST    6       /*      - " -    LIST_NODE                           */
#define COND    7       /*      - " -    COND_NODE                           */
#define TCONC   8       /*      - " -    TCONC_NODE                          */

typedef char pmsubcl;   /* Type for subclasses with values defined by        */
                        /* constatns for each node class.                    */

/*=========================== MODULE_NODE ===================================*/
/*      Node for a MODULE.                                                   */
/*---------------------------------------------------------------------------*/
/*    PMMONO  -  definition                                                  */

typedef struct pmmono   /* The Module node                                   */
    {
    pmnocl  monocl;     /* Node class                                        */
    pm_ptr  mona_;      /* Module name : pointer to a character string in PM */
    pmmoty  moty_;      /* Module type : always 3D                           */
    pmmoat  moat_;      /* Module attribute : LOCAL,GLOBAL.. etc.            */
    pm_ptr  psysb_;     /* Pointer to Symbol table, system part              */
    pm_ptr  ppali_;     /* Pointer to List Node which point to  PARAM_NODE   */
    pm_ptr  plosb_;     /* Pointer to Symbol table, local part               */
    pm_ptr  pstl_;      /* Pointer to List Node which point to  STAT_NODE    */
    pmseqn  geidlev;    /* geometric id level, the highest number used for   */
                        /* an identity in a module */
    DBint   datasize;   /* size of local data size */
    V3MDAT  sysdat;     /* system data structure */
    } PMMONO ;
/*---------------------------------------------------------------------------*/
/*      End of definition for MODULE_NODE.                                   */
/*===========================================================================*/

/*=========================== PARAM_NODE ====================================*/
/*         Node for a formal parameter to a MODULE.                          */
/*---------------------------------------------------------------------------*/
/*    PMPANO  -  definition                                                  */

typedef struct pmpano   /* The Module Parameter node                         */
    {
    pmnocl noclpa;      /* Node class */
    pm_ptr  fopa_;      /* Pointer to Symbol table, local part               */
    pm_ptr  defval;     /* PM-pointer to an literal expression for def. value*/
    pm_ptr  ppro_;      /* Pointer to Prompt character string                */
    }  PMPANO ;

/*---------------------------------------------------------------------------*/
/*     End of definition for PARAM_NODE.                                     */
/*===========================================================================*/

/*=========================== STAT_NODE =====================================*/
/*      Node for a statement.                                                */
/*---------------------------------------------------------------------------*/
/*    The statement node have a number of subclasses that represent the      */
/*    different MBS statements. The type for the following node subclasses   */
/*    are defined before the statement node it self.                         */
/*              PMASST  -  ASS_STAT (Assignment statement)                   */
/*              PMIFST  -  IF_STAT  (IF-statement)                           */
/*              PMFOST  -  FOR_STAT (FOR-statement)                          */
/*              PMGOST  -  GOTO_STAT (GOTO-statement)                        */
/*              PMPRST  -  PROC_STAT (Procedure call)                        */
/*              PMPAST  -  PART_STAT ( Part call statement )                 */
/*              PMGEST  -  GEO_STAT  (Geometric definition routine call)     */
/*              PMEMST  -  EMPTY_STAT (empty statement)                      */
/*              PMLAST  -  LABELED_STAT (Labeled statement)                  */
/*---------------------------------------------------------------------------*/
#define ASS_ST  1       /* constant for subclass  ASS_STAT                   */
#define IF_ST   2       /*        - " -           IF_STAT                    */
#define FOR_ST  3       /*        - " -           FOR_STAT                   */
#define GO_ST   4       /*        - " -           GOTO_STAT                  */
#define PRO_ST  5       /*        - " -           PROC_STAT                  */
#define PART_ST 6       /*        - " -           PART_STAT                  */
#define GEO_ST  7       /*        - " -           GEO_STAT                   */
#define EMP_ST  8       /*        - " -           EMPTY_STAT                 */
#define LAB_ST  9       /*        - " -           LABELED_STAT               */
/*---------------------------------------------------------------------------*/

/*      PMASST  -  definition   (ASS_STAT)                                   */

typedef struct pmasst   /* Assignment subnode                                */
    {
    pm_ptr  asvar;      /* Pointer to an Expression Node (left expr.)       */
    pm_ptr  asexpr;     /* Pointer to an Expression Node (right expr.)      */
    }  PMASST;

/*---------------------------------------------------------------------------*/
/*      PMIFST  -  definition   (IF_STAT)                                    */

typedef struct pmifst   /* IF-statement subnode                              */
    {
    pm_ptr  ifcond;     /* Pointer to an Condition list                      */
                        /*       ( TCONC_NODE -> LIST_NODE -> COND_NODE )    */
    pm_ptr  ifstat;     /* Pointer to an Statement list                      */
                        /*       ( TCONC_NODE -> LIST_NODE -> STAT_NODE )    */
    }  PMIFST;

/*---------------------------------------------------------------------------*/
/*      PMFOST  -  definition    (FOR_STAT)                                  */

typedef struct pmfost   /* For statement subnode                             */
    {
    pm_ptr  fovar;      /* For-variabel, pointer to Symbol tabel (local part)*/
    pm_ptr  fostar;     /* Start-expression, pointer to PMEXNO (EXPR_NODE)   */
    pm_ptr  foto;       /* To-expression, pointer to PMEXNO (EXPR_NODE)      */
    pm_ptr  fostep;     /* Step-expression,      - " -                       */
    pm_ptr  fordo;      /* For-Do statement, Statement list                  */
    }  PMFOST;

/*---------------------------------------------------------------------------*/
/*      PMGOST  -  definition      (GOTO_STAT)                               */

typedef struct pmgost   /* Goto statement subnode                            */
    {
    pm_ptr  golabe;     /* Pointer to Symbol table, local part               */
    }  PMGOST;

/*---------------------------------------------------------------------------*/
/*      PMPRST  -  definition       (PROC_STAT)                              */

typedef struct pmprst   /* Procedure call statement subnode                  */
    {
    pm_ptr prname;      /* Procedure name, PM-pointer to symbol table        */
    pm_ptr  pracva;     /* Actual value parameters, expression list          */
   }  PMPRST;

/*---------------------------------------------------------------------------*/
/*      PMPAST  -  definition       (PART_STAT)                              */

typedef struct pmpast  /* Part call statement subnode*/
    {
    pmseqn  geident;    /* Geometric identity, the sequence number           */
    pm_ptr  modname;    /* module name (parted module)                       */
    pm_ptr  modpara;    /* module parameters (actual param list)             */
    pm_ptr  partpara;   /* the part statement parameters                     */
    pm_ptr  partacna;   /* the part statrement named parameters              */
   }  PMPAST;

/*---------------------------------------------------------------------------*/
/*      PMGEST  -  definition       (GEO_STAT)                               */

typedef struct pmgest  /* Geometric definition routine call statement subnode*/
    {
    pmseqn  geident;    /* Geometric identity, the sequence number           */
    pm_ptr  gename;     /* Procedure name PM-pointer to symbol table         */
    pm_ptr  geacva;     /* Actual value parameters, expression list          */
    pm_ptr  geacna;     /* Actual named parameters, NAME_PARAM_NODE list     */
   }  PMGEST;

/*---------------------------------------------------------------------------*/
/*      PMLAST  -  definition      (LABELED_STAT)                            */

typedef struct pmlast   /* Labeled statement subnode                         */
    {
    pm_ptr  lalabe;     /* Label, pointer to Symbol table, local part        */
    pm_ptr  lastat;     /* PM-pointer to a statement                         */
    }  PMLAST;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*    PMSTNO  -  definition      (STAT_NODE)                                 */

typedef struct pmstno   /* The Statement node                                */
    {
    pmnocl  noclst;     /* Node class                                        */
    pmsubcl suclst;     /* Node subclass                                     */
    union st_tag 
        {
        PMASST  ass_st; /* Node subclass : Assignment statement              */ 
        PMIFST  if_st;  /* Node subclass : IF - statement                    */
        PMFOST  for_st; /*    - " -        FOR - statement                   */
        PMGOST  gotost; /*    - " -        GOTO - statement                  */
        PMPRST  procst; /*    - " -        PROC - statement                  */
        PMPAST  partst; /*    - " -        PART - statement                  */
        PMGEST  geo_st; /*    - " -        GEO - statement                   */
        PMLAST  labest; /*    - " -        LABELED - statement               */
       } stsubc; 
    }  PMSTNO;

/*---------------------------------------------------------------------------*/
/*      End of Statement Node definition                                     */
/*===========================================================================*/

/*=========================== EXPR_NODE =====================================*/
/*      Node for an expression.                                              */
/*---------------------------------------------------------------------------*/
/*    The expression node have a number of subclasses that represent the     */
/*    different expression types. The type for the following node subclasses */
/*    are defined before the expression node it self.                        */
/*              PMUNEX   -  UNARY_OP  unary operator                         */
/*              PMBINEX  -  BIN_OP    binary operators                       */
/*              PMLITEX  -  LITERAL   literals- int, float, str, vec, ref    */
/*              PMIDEX   -  ID_EXPR   identifier                             */
/*              PMINDEX  -  IND_EXPR  identifier(expr,....)                  */
/*              PMCOMPEX -  COMP_EXPR identifier.identifier                  */
/*              PMFUNCEX -  FUNC_EXPR identifier(expr,....)                  */
/*---------------------------------------------------------------------------*/
/*      Constants for expression node subclasses.                            */

#define C_BIN_EX  1     /* constant for subclass BIN_OP                      */
#define C_UN_EX   2     /*          - " -        UN_EX                       */
#define C_LIT_EX  3     /*          - " -        LITERAL                     */
#define C_ID_EX   4     /*          - " -        ID_EXPR                     */
#define C_IND_EX  5     /*          - " -        IND_EXPR                    */
#define C_COM_EX  6     /*          - " -        COMP_EXPR                   */
#define C_FUN_EX  7     /*          - " -        FUNC_EXPR                   */

/*---------------------------------------------------------------------------*/
/*      PMBINEX  - definition      (BIN_OP)                                  */

typedef struct pmbinex  /* binary operator expression subclass               */
    {
    pm_ptr  p_bin_l;    /* pointer to left expression  EXPR_NODE             */
    pm_ptr  p_bin_r;    /*     - " -  right       - " -                      */
    pmopty  binop;      /* operator (minus, plus, etc..)                     */
    } PMBINEX;

/*---------------------------------------------------------------------------*/
/*      PMUNEX  - definition      (UNARY_OP)                                 */

typedef struct pmunex    /* unary operator expression subclass               */
    {
    pm_ptr  p_unex;     /* pointer to expression  EXPR_NODE                  */
    pmopty  unop;      /* operator (minus, NOT)                              */
    } PMUNEX;

/*---------------------------------------------------------------------------*/
/*      PMLITEX  -  definition        (LITERAL)                              */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*    The literal expression subclass have a number of value structures that */
/*    represent the  different literal types. The type for the following     */
/*    value structures are defined in pmdef.h .                              */ 
/*              pmint    -  INT_VAL    integer value                         */
/*              pmfloat  -  FLOAT_VAL  float value                           */
/*          and the following are defined below:                             */
/*              PMVECEX  -  VEC_VAL    vector value                          */
/*              PMREFEX  -  REF_VAL    reference value                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      PMVECEX  -  definition           (VEC_VAL)                           */

typedef pm_ptr pmvecex;   /* PM_pointer to an PMVECVA, vector value.         */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      PMREFEX  -  definition            (REF_VAL)                          */

typedef struct pmrefex   /* reference value structure                        */
    {
    DBseqnum  seq_lit;   /* sequence number value in 16 bits                 */
    DBordnum  ord_lit;   /* order-number value in 16 bits                    */
    pm_ptr nextref;      /* PM_pointer to next ref. in chain           */
    } PMREFEX;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      PMLITEX  -  definition           (LIT_EXPR)                          */

typedef struct pmlitex   /* literal expression subclass                     */  
    {
    pmvaty  lit_type;    /* literal value type (int, float, ref, .....)      */
    union
        {
        pmint  int_li;       /* Value type  :   integer                      */
        pmfloat  float_li;   /*     - " -       float                        */
        pm_ptr  str_li;      /*     - " -       string                       */
        pmvecex  vec_li;     /*     - " -       vector                       */
        PMREFEX  ref_li;     /*     - " -       reference                    */
        } litex;
    } PMLITEX;


/*---------------------------------------------------------------------------*/
/*      PMIDEX  -  definition         (ID_EXPR)                              */

typedef struct pmidex   /* variable expression subclass                      */
    {
    pm_ptr  p_id;       /* pointer to Symbol table, local part               */
    } PMIDEX;

/*---------------------------------------------------------------------------*/
/*      PMINDEX  -  definition         (IND_EXPR)                            */

typedef struct pmindex   /* indexed variabel expression subclass             */
    {
    pm_ptr  p_indid;     /* pointer to Symbol table, local part              */
    pm_ptr  p_indli;     /* pointer to expression list                       */
    } PMINDEX;

/*---------------------------------------------------------------------------*/
/*      PMCOMPEX  -  definition           (COMP_EXPR)                        */

typedef struct pmcompex  /* component variable expression subclass           */
    {
    pm_ptr  p_comvar;    /* pointer to EXPR_NODE                             */
    pmfieldt p_cfield;    /* indicate field  X, Y or Z     (1, 2, 3)         */
    } PMCOMPEX;

/*---------------------------------------------------------------------------*/
/*      PMFUNCEX  -  definition            (FUNC_EXPR)                       */

typedef struct pmfuncex   /* function call expression subclass               */
    {
    pm_ptr  funcid;       /* pointer to Symbol table, system part            */
    pm_ptr  p_funcar;     /* pointer to expression list (arguments)          */
    } PMFUNCEX;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*    PMEXNO  -  definition      (EXPR_NODE)                                 */

typedef struct pmexno   /* The Expression node                               */
    {
    pmnocl  noclex;     /* Node class                                        */
    pmsubcl suclex;     /* Node subclass                                     */
    union ex_tag 
        {
        PMBINEX  binop_ex;   /* Node subclass : Binary operator expression   */
        PMUNEX   unop_ex;    /*    - "  -       Unary operator      - " -    */
        PMLITEX  lit_ex;     /*    - " -        Literal             - " -    */
        PMIDEX  id_ex;       /*    - " -        Identifier          - " -    */
        PMINDEX  ind_ex;     /*    - " -        Index variable      - " -    */
        PMCOMPEX  comp_ex;   /*    - " -        Vector component    - " -    */
        PMFUNCEX  func_ex;   /*    - " -        Function call       - " -    */
      } ex_subcl;
    } PMEXNO;

/*---------------------------------------------------------------------------*/
/*      End of Expression Node definition.                                   */
/*===========================================================================*/

/*=========================== NAME_PARAM_NODE ===============================*/
/*         Node for an named actual parameter to an geometric routine.       */
/*---------------------------------------------------------------------------*/
/*    PMNPNO  -  definition                                                  */

typedef struct pmnpno   /* The Named Parameter node */        /* POA 850515 */
    {
    pmnocl noclnp;      /* Node class */
    pmmdid par_np;      /* Id value for parameter name                    */
    pm_ptr  p_val_np;   /* Pointer to an expression, named param value.   */
    } PMNPNO;


/*---------------------------------------------------------------------------*/
/*     End of NAME_PARAM_NODE definition.                                    */
/*===========================================================================*/

/*=========================== COND_NODE =====================================*/
/*      Node for an if statement Condition.                                  */
/*---------------------------------------------------------------------------*/
/*      PMCONO  -  definition                                                */

typedef struct pmcono   /* The if-condition node                             */
    {
    pmnocl  noclco;     /* Node class */
    pm_ptr  p_cond;     /* Pointer to an EXPR_NODE                           */
    pm_ptr  p_stl_co;   /* Pointer to an statement list                      */
    } PMCONO;

/*---------------------------------------------------------------------------*/
/*      End of COND_NODE definition.                                         */
/*===========================================================================*/

/*=========================== LIST_NODE =====================================*/
/*      Node for an generic List node. This node is used for all list        */
/*      structures with other Node classes. ie. an statement list contains   */
/*      of an LIST_NODE which point to an STAT_NODE and to an other          */
/*      LIST_NODE or NIL (null pointer) if end of list. A list also always   */
/*      contain one TCONC_NODE which is used to store the list in right      */
/*      order.                                                               */ 
/*---------------------------------------------------------------------------*/
/*      PMLINO  -  definition           (LIST_NODE)                          */

typedef struct pmlino   /* The generic List node                             */
    {
    pmnocl  noclli;     /* Node class */
    pm_ptr  p_no_li;    /* Pointer to an generic PM-structure                */
    pm_ptr  p_nex_li;   /* Pointer to next LIST_NODE                         */
    } PMLINO;

/*---------------------------------------------------------------------------*/
/*      End of LIST_NODE definition.                                         */
/*===========================================================================*/

/*=========================== TCONC_NODE ====================================*/
/*      This node holdes the head and tail of an list. All list contains one */
/*      TCONC_NODE to holde the righ (forward) order of an list.             */
/*                                                                           */
/*                LIST_NODE (head) --------> LIST_NODE (tail)                */
/*                ^                          ^                               */
/*                !------- TCONC_NODE -------!                               */
/*---------------------------------------------------------------------------*/
/*      PMTCNO  -  definition           (TCONC_NODE)                         */

typedef struct pmtcno     /* The tail concatenete node                       */
    {
    pmnocl nocltc;        /* Node class */
    pm_ptr p_head;        /* PM-pointer to first LIST_NODE in list           */
    pm_ptr p_tail;        /* PM-pointer to tail (last) LIST_NODE in list     */
    } PMTCNO;

/*---------------------------------------------------------------------------*/
/*      End of TCONC_NODE definition.                                        */
/*===========================================================================*/

