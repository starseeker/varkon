/**********************************************************************
*
*    anpar40.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    Module parser and generator of the internal form
*
*    This file includes the following routines:
*
*    anpmod();            Analysis the module header and body.
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
***********************************************************************/

#ifdef ANALYZER

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/AN.h"
#include <string.h>

extern struct ANSYREC sy;          /* Scanner interface variable          */
extern V3MDAT sydata;              /* System specification                */
extern pm_ptr ststrp;              /* String type pointer                 */
extern pm_ptr stintp;              /* Integer type pointer                */
extern pm_ptr stflop;              /* Float type pointer                  */
extern pm_ptr stvecp;              /* Vector type pointer                 */
extern pm_ptr strefp;              /* Reference type pointer              */
extern short  modtyp;              /* Module type 2 = _2D, 3 = _3D        */
extern short  modatt;              /* Modul-attribut, GLOBAL, LOCAL etc...*/

void  anpmod();                    /* Analysis of the module              */

DBint anoffs;                      /* Run time stack offsets for variables*/

static void form_param(pm_ptr *parm_p, ANFSET *fsys);
static void var_parm_decl(pm_ptr *parm_list, ANFSET *fsys);
static void val_parm_decl(pm_ptr *parm_list, ANFSET *fsys);
static void declaration(ANFSET *fsys);
static void const_decl(ANFSET *fsys);
static void var_decl(ANFSET *fsys);
static void get_base(pm_ptr *type);
static void var_def(pm_ptr base_typ, pm_ptr *var_pm,
                    pm_ptr *type_pm, ANFSET *fsys);
static void index_def(pm_ptr base_typ, pm_ptr *type_pm, ANFSET *fsys);
static void string_def(pm_ptr *type_pm, ANFSET *fsys);

/********************************************************/
/*!******************************************************/

        void anpmod(
        pm_ptr *modul_p)

/*      Analyze a module definition
 *
 *      module_description =  
 *       Ämod_attrÅ Ämod_typeÅ 'MODULE' id  formal_param_list  ';' body
 *      mod_attr = 'LOCAL' ! 'GLOBAL' ! 'BASIC' ! 'MACRO'
 *      mod_type = 'GEOMETRY' ! 'DRAWING'
 *      body = declarations 'BEGINMODULE' statement_list 'ENDMODULE'
 *
 *      In:   *modul_p  =>  PM pointer to the module header .
 *
 *      Out:  
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *      2007-11-20 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
   ANSYNAME modnm;           /* Module name                       */
   PMMODULE modulatt;        /* Module attributes                 */
   ANFSET   loc_fol;         /* Current follower set              */
   ANFSET   decl_sys;        /* Symbols in FIRST(declaration)     */
   ANFSET   body_sys;        /* Symbols in FIRST(body)            */
   pm_ptr   p_list;          /* Parameter list                    */
   pm_ptr   st_list;         /* Statement list                    */
   char     buf[V3STRLEN+1]; /* Temporär buffert för modulnamn    */
   bool     varning,hit;     /* Flaggor för felhant. av modulnamn */

   anoffs = 0;               /* 861014JK */

   ancset(&decl_sys, NULL, 7, ANSYINT, ANSYFLT, ANSYSTR, ANSYVECT, 
                              ANSYREF, ANSYFILE, ANSYCONS,0);
   ancset(&body_sys, &decl_sys, 1, ANSYBMOD,0,0,0,0,0,0,0);
/*
***Set up the module header parameter
*/
   modulatt.mtype   = _3D;
   modulatt.mattri  = LOCAL;
   modulatt.parlist = (pm_ptr)NULL;
   modulatt.stlist  = (pm_ptr)NULL;
   modulatt.idmax   = 0;
   modulatt.ldsize  = 0;
   V3MOME(&sydata, &(modulatt.system), sizeof(sydata));
/*
***Attribut, 13/2/92 J. Kjellander.
*/
   if ( (sy.sytype == ANSYLOCL) || (sy.sytype == ANSYGLOB) || 
        (sy.sytype == ANSYBASI) || (sy.sytype == ANSYMAC) )
     {
     if (sy.sytype == ANSYGLOB)
       modulatt.mattri = GLOBAL;
     else if (sy.sytype == ANSYBASI)
       modulatt.mattri = BASIC;
     else if (sy.sytype == ANSYMAC)
       modulatt.mattri = MACRO;

     anascan(&sy);
     }
   modatt = (short) modulatt.mattri;
/*
***Type, always _3D. Issue warning if type keyword is used.
*/
   if ( sy.sytype == ANSYDRAW )
     {
     anperr("AN9471", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
     anascan(&sy);
     }
   else if ( sy.sytype == ANSYGEO )
     {
     anperr("AN9481", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
     anascan(&sy);
     }

   modtyp = (short) modulatt.mtype;

   if ( sy.sytype != ANSYMOD )
     anperr("AN2012", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
/*
***Modulnamnet, inledande siffror tillåtna 3/5/88 JK.
*/
   strcpy(modnm,"");
   varning = hit = FALSE;
loop:
   anascan(&sy);
   if ( sy.sytype == ANSYID )
     {
     strcat(modnm, sy.syval.name);
     hit = TRUE;
     goto loop;
     }
   else if ( sy.sytype == ANSYINTL )
     {
     sprintf(buf,"%d",sy.syval.ival);
     strcat(modnm,buf);
     hit = TRUE;
     varning = TRUE;
     goto loop;
     }
   else if ( sy.sytype == ANSYUND )
     {
     strcat(modnm,"_");
     if ( !hit ) varning = TRUE;
     hit = TRUE;
     goto loop;
     }

   if ( varning )
     anperr("AN2291", modnm, NULL, sy.sypos.srclin, sy.sypos.srccol);
   else if ( !hit )
     anperr("AN2022", "Unknown", NULL, sy.sypos.srclin, sy.sypos.srccol);
/*
***Kolla att modulnamnet inte är längre än 10 tkn.
*/
   if ( strlen(modnm) > JNLGTH )
     anperr("AN2302", modnm, NULL, sy.sypos.srclin, sy.sypos.srccol);
/* 
***Create the module header -> creates the local symbol table
*/
   pmcmod(modnm, &modulatt, modul_p);
/*
***Pararameter section
*/
   ancset(&loc_fol, &body_sys, 1,  ANSYSEMI,0,0,0,0,0,0,0);
   form_param(&p_list, &loc_fol);
   if (sy.sytype != ANSYSEMI)
        anperr("AN2032", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
   else anascan(&sy);
/*
***Module declarations
*/
   do
     {
     declaration(&body_sys);
     if ( !aniset(sy.sytype, &body_sys) )
       anperr("AN2043", "", &body_sys, sy.sypos.srclin, sy.sypos.srccol);
     }

   while (aniset(sy.sytype, &decl_sys));

   if (sy.sytype == ANSYBMOD)
     anascan(&sy);
/*
***Module statement part
*/
   ancset(&loc_fol, NULL, 1, ANSYEMOD,0,0,0,0,0,0,0);
   ansmts(&st_list, &loc_fol);
/*
***Update module header
*/
   pmrmod(&modulatt);                      /* Read module header */

   modulatt.ldsize  = anoffs;              /* Data area size */
   modulatt.stlist = st_list;
   modulatt.parlist = p_list;

   pmumod(&modulatt);                      /* Update module header */

   ancset(&loc_fol, NULL, 1, ANSYEOF,0,0,0,0,0,0,0);
   if (sy.sytype == ANSYEMOD)
     {
     anascan(&sy);
     if (sy.sytype != ANSYEOF) 
       anperr("AN2052", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
     }
   else 
     anperr("AN2062", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
  }

/********************************************************/
/*!******************************************************/

        static void form_param(
        pm_ptr    *parm_p,
        ANFSET    *fsys)

/*      Recognises the formal parameter list.
 *
 *      formal_param_def = '(' Äparameter_listÅ ')'
 *      parameter_list = parm_decl ä';' parm_declå
 *      parm_decl = 'VAR' var_parm_decl ! value_parm_decl
 *
 *      In:  *fsys    =>  Follower set.
 *
 *      Out: *parm_p  =>  List of parameters.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET    parm_sys;      /* Symbols in FIRST(parm_decl)     */
   ANFSET    var_sys;       /* Symbols in FIRST(var_parm_decl) */
   ANFSET    ssys;          /* Recovery symbols                */
   ANFSET    loc_fol;       /* Local follower set              */

   ancset(&var_sys, NULL, 6, ANSYINT, ANSYFLT, ANSYVECT, ANSYREF,
                             ANSYSTR, ANSYFILE,0,0);
   ancset(&parm_sys, NULL, 6, ANSYINT, ANSYFLT, ANSYSTR, ANSYVECT, 
                              ANSYREF, ANSYVAR,0,0);
   ancset(&ssys, fsys, 7, ANSYINT, ANSYFLT, ANSYSTR, ANSYVECT, ANSYREF,
                          ANSYVAR, ANSYRPAR,0);
   *parm_p = (pm_ptr)NULL;
   if (sy.sytype == ANSYLPAR)
     anascan(&sy);
   else 
     anperr("AN2072", "", NULL, sy.sypos.srclin, sy.sypos.srccol);

   ancset(&loc_fol, fsys, 2, ANSYRPAR, ANSYSEMI,0,0,0,0,0,0);
/*
***Repeat for all parameter declarations
*/
   do    
     {
     if (aniset(sy.sytype, &parm_sys))
       if (sy.sytype == ANSYVAR)
         {
         anascan(&sy);
         if (aniset(sy.sytype, &var_sys))
           var_parm_decl(parm_p, &loc_fol);
         else 
           anperr("AN2213", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
         }

       else
         val_parm_decl(parm_p, &loc_fol);
     else if (sy.sytype != ANSYRPAR)
/*
***Missing parameter definition
*/
       anperr("AN2263", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);

     if (sy.sytype == ANSYSEMI)
       {
       anascan(&sy);
       if ( !aniset(sy.sytype, &parm_sys) )
         anperr("AN2263", "", &ssys, sy.sypos.srclin, sy.sypos.srccol);
       }
     else if (sy.sytype != ANSYRPAR)
        anperr("AN2272", "", &ssys, sy.sypos.srclin, sy.sypos.srccol);
     } 

   while (aniset(sy.sytype, &parm_sys));

   if (sy.sytype == ANSYRPAR)
     anascan(&sy);
  }

/********************************************************/
/*!******************************************************/

        static void var_parm_decl(
        pm_ptr  *parm_list,
        ANFSET  *fsys) 

/*      Analysis of a formal reference parameter list
 *
 *      var_parm_decl =  var_parm_type var_def ä',' var_defå
 *      var_parm_type = 'INT' ! 'FLOAT' ! 'VECTOR' ! 'REF' !
 *                      'STRING' ! 'FILE'
 *
 *      In:  *fsys       =>  Follower set.
 *
 *      Out: *parm_list  =>  Parameter list internal form.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET   loc_fol;      /* Local follower set              */
   pm_ptr   base_typ;     /* Basic parameter type            */
   pm_ptr   par_pm;       /* PM pointer to param. definition */
   pm_ptr   typ_pm;       /* PM pointer to parameter type    */
   STVAR    par_descr;    /* Parameter attributes            */
   pm_ptr   parm;         /* PM pointer to parameter node    */
   pm_ptr   dummy;        /* Dummy PM pointer                */

   get_base(&base_typ);
   anascan(&sy);

   do
     {
     if (sy.sytype == ANSYID)
       {
       ancset(&loc_fol, fsys, 1, ANSYCOM,0,0,0,0,0,0,0);
       var_def(base_typ, &par_pm, &typ_pm, &loc_fol);
       if (par_pm != (pm_ptr)NULL)
/*
***Fill in the missing symbol table values
*/ 
         {  
         par_descr.type_va = typ_pm;
         par_descr.addr_va = 0 ;
         par_descr.kind_va = ST_RPAVA;
         par_descr.def_va = (pm_ptr)NULL;
         stuvar(par_pm, &par_descr);
/*
***Create the parameter node and link
*/
         pmcpar(par_pm, "", &parm);
         pmtcon(parm, *parm_list, parm_list, &dummy);
         }
       }
     else
/*
***Not an identifier
*/
       {
       ancset(&loc_fol, fsys, 1, ANSYCOM,0,0,0,0,0,0,0);
       anperr("AN2163", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
       }

     if (sy.sytype == ANSYCOM)
       {
       anascan(&sy);
       if (sy.sytype != ANSYID)
         {
         ancset(&loc_fol, fsys, 1, ANSYID,0,0,0,0,0,0,0);
         anperr("AN2163", "", &loc_fol, sy.sypos.srclin,
                                           sy.sypos.srccol);
         }
       }
     else if ( !aniset(sy.sytype, fsys) )
       {
       ancset(&loc_fol, fsys, 1, ANSYID,0,0,0,0,0,0,0);
       anperr("AN2282", "", &loc_fol, sy.sypos.srclin,
                                         sy.sypos.srccol);
       }     
     }
   while (sy.sytype == ANSYID);
  }

/********************************************************/
/*!******************************************************/

        static void val_parm_decl(
        pm_ptr *parm_list,
        ANFSET *fsys)

/*      Analysis of a formal value parameter list
 *
 *      value_parm_decl = value_parm_type  value_parm_def ä',' value_parm_defå
 *      value_parm_type = 'INT' ! 'FLOAT' ! 'VECTOR' ! 'REF' ! 'STRING'
 *      value_parm_decl = var_def Ä':=' init_valueÅ Ä'>' prompt_stringÅ
 *      init_value = constant_expression
 *      prompt_string = character_string
 *
 *      In:  *fsys       =>  Follower set.
 *
 *      Out: *parm_list  =>  Parameter list internal form.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-03-26 *fsys)  IN  - Follower set, J. Kjellander
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET   loc_fol;       /* Local follower set                */
   pm_ptr   base_typ;      /* Basic parameter type              */
   pm_ptr   par_pm;        /* PM pointer to param. definition   */
   pm_ptr   typ_pm;        /* PM pointer to parameter type      */
   STVAR    par_descr;     /* Parameter attributes              */
   STTYTBL  type_descr;    /* Type attributes                   */
   pm_ptr   init_expr;     /* Initial value - expression PM ptr */
   ANATTR   init_attr;     /*       -"-     - expression attr.  */
   PMLITVA  init_valu;     /*       -"-     - expression value  */
   string   prompt;        /* Prompt string                     */
   pm_ptr   parm;          /* PM pointer to parameter node      */
   short    p_line;        /* Parameter position - line         */
   short    p_col;         /* Parameter position - column       */
   pm_ptr   dummy;         /* Dummy PM pointer                  */

   get_base(&base_typ);    /* Legal type is checked by caller   */
   anascan(&sy);

   do
     {
     if (sy.sytype == ANSYID)
       {
       p_line = sy.sypos.srclin;
       p_col  = sy.sypos.srccol;

       ancset(&loc_fol, fsys, 3, ANSYCOM, ANSYASGN, ANSYGT,0,0,0,0,0);
       var_def(base_typ, &par_pm, &typ_pm, &loc_fol);
/*
***The value parameter(s) must not be dimensioned
*/     
       if (typ_pm != (pm_ptr)NULL)
         {
         strtyp(typ_pm, &type_descr);
         if (type_descr.kind_ty == ST_ARR) 
           {
           anperr("AN2223", "", NULL, p_line, p_col);
           typ_pm = (pm_ptr)NULL;
           }
         }
/*
***Init. the optional initial value struct.
*/
       if (base_typ == stintp)
         {
         init_valu.lit.int_va = 0;
         init_valu.lit_type = C_INT_VA;
         }
       else if(base_typ == stflop)
         {
         init_valu.lit.float_va = 0.0;
         init_valu.lit_type = C_FLO_VA;
         }
       else if(base_typ == stvecp)
         {
         init_valu.lit.vec_va.x_val = 0.0;
         init_valu.lit.vec_va.y_val = 0.0;
         init_valu.lit.vec_va.z_val = 0.0;
         init_valu.lit_type = C_VEC_VA;
         }
       else if(base_typ == ststrp)
         {
         strcpy(init_valu.lit.str_va,"");
         init_valu.lit_type = C_STR_VA;
         }
       else if(base_typ == strefp)
         {
         init_valu.lit.ref_va[0].seq_val = 1;
         init_valu.lit.ref_va[0].ord_val = 1;
         init_valu.lit.ref_va[0].p_nextre = NULL;
         init_valu.lit_type = C_REF_VA;
         }
/*
***Pick up the optional initial value
*/
       if (sy.sytype == ANSYASGN)
         {
         anascan(&sy);
         p_line = sy.sypos.srclin;
         p_col  = sy.sypos.srccol;
         ancset(&loc_fol, fsys, 2, ANSYCOM, ANSYGT,0,0,0,0,0,0);
         ancoex(&init_expr, &init_attr, &init_valu, &loc_fol);
/*
***Create a PM pointer to the literal (expr)
*/
         if (anacty(base_typ, init_attr.type))       /* Compat. types */
           {
           if (init_attr.type != (pm_ptr)NULL)
             if (aneqty(stflop, base_typ) && aneqty(stintp,
                                              init_attr.type))
               {
               init_valu.lit.float_va = init_valu.lit.int_va;
               init_valu.lit_type = C_FLO_VA;
               }
           }
         else 
/*
***Types not compatible
*/
           {
           anperr("AN2123", "", NULL, p_line, p_col);
           }
         }

       pmclie(&init_valu, &init_expr); 

/*
***Pick up the optional prompt string
*/          
       prompt = "";
       if (sy.sytype == ANSYGT)
         {
         anascan(&sy);
         if (sy.sytype == ANSYSTRL)
           {
           prompt = sy.syval.sval;
           anascan(&sy);
           }
         else 
           {
           ancset(&loc_fol, fsys, 1, ANSYCOM,0,0,0,0,0,0,0);
           anperr("AN2233", "", &loc_fol, sy.sypos.srclin,
                                             sy.sypos.srccol);
           }
         }
                   
       if (par_pm != (pm_ptr)NULL)
/*
***Fill in the missing symbol table values
*/ 
         {
         par_descr.type_va = typ_pm;
         par_descr.addr_va = 0 ;
         par_descr.kind_va = ST_VPAVA;
         par_descr.def_va = init_expr;
         stuvar(par_pm, &par_descr);
/*
***Create the parameter node and link
*/
         pmcpar(par_pm, prompt, &parm);
         pmtcon(parm, *parm_list, parm_list, &dummy);
         }
       }
     else
/*
***Not an identifier
*/
       {
       ancset(&loc_fol, fsys, 1, ANSYCOM,0,0,0,0,0,0,0);
       anperr("AN2163", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
       }

     if (sy.sytype == ANSYCOM)
       {
       anascan(&sy);
       if (sy.sytype != ANSYID)
         {
         ancset(&loc_fol, fsys, 1, ANSYID,0,0,0,0,0,0,0);
         anperr("AN2163", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
         }
       }
     else if (!aniset(sy.sytype, fsys))
       {
       ancset(&loc_fol, fsys, 1, ANSYID,0,0,0,0,0,0,0);
       anperr("AN2282", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
       }     
     }
   while (sy.sytype == ANSYID);
  }

/********************************************************/
/*!******************************************************/

        static void declaration(
        ANFSET   *fsys)


/*      Analyse the declaration part of the MBS language
 *
 *      declaration = CONSTANT constant_declaration !
 *      var_declaration !
 *
 *      In:  *fsys  =>  Follower set.
 *
 *      Out:
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET   decl_sys;       /* Symbols in FIRST(declaration)         */
   ANFSET   cons_sys;       /* Symbols in FIRST(constant_declaration)*/

   ancset(&cons_sys, NULL, 5, ANSYINT, ANSYFLT, ANSYSTR, ANSYVECT,
                              ANSYREF,0,0,0);
   ancset(&decl_sys, &cons_sys, 2, ANSYFILE, ANSYCONS,0,0,0,0,0,0);

   if (aniset(sy.sytype, &decl_sys))
     {
     if (sy.sytype == ANSYCONS)             /* Constant declaration? */
       {
       anascan(&sy);
       if (aniset(sy.sytype, &cons_sys)) const_decl(fsys);
       else anperr("AN2093", "", fsys, sy.sypos.srclin, sy.sypos.srccol);
       }
     else var_decl(fsys);
     }
  }

/********************************************************/
/*!******************************************************/

        static void const_decl(
        ANFSET *fsys)

/*      Analyse a constant declaration
 *
 *      constant_declaration = const_type  const_def ä',' const_defå ';'
 *      const_type = 'INT' ! 'FLOAT' ! 'REF' ! 'VECTOR' ! 'STRING'
 *      const_def = identifier '=' constant_expression
 *
 *      In:  *fsys  =>  Follower set.
 *
 *      Out: 
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET    loc_fol;      /* The local follower set             */
   pm_ptr    base_typ;     /* The basic constant type PM pointer */
   pm_ptr    cid_pm;       /* PM pointer to constant entry       */
   pm_ptr    expr_p;       /* PM pointer to created expression   */
   short     e_line;       /* Expression line position           */
   short     e_col;        /* Expression column position         */
   ANATTR    attr;         /* Expression attributes              */
   PMLITVA   valu;         /* Expression value                   */
   STCONST   c_descr;      /* Constant attributes                */


   get_base(&base_typ);    /* Legal type is already checked      */
   anascan(&sy);

/*
***Repeat for all constant definitions 
*/
   do 
     {
     if (sy.sytype == ANSYID)
/*
***Create the symbol table entry 
*/
       {   
       if (stccon(sy.syval.name, NULL, &cid_pm) != 0)
/*
***Constant already declared or ambiquous
*/
         anperr("AN2102", "", NULL, sy.sypos.srclin,sy.sypos.srccol);
       anascan(&sy);
             
       if (sy.sytype == ANSYEQ)
         anascan(&sy);
       else
         anperr("AN2112", "", NULL, sy.sypos.srclin, sy.sypos.srccol);

       e_line = sy.sypos.srclin;
       e_col  = sy.sypos.srccol;               
       ancset(&loc_fol, fsys, 1 , ANSYSEMI,0,0,0,0,0,0,0);
       ancoex(&expr_p, &attr, &valu, &loc_fol);
              
       if (anacty(base_typ, attr.type))
/*
***Compatible types 
*/
         {
         if (attr.type != (pm_ptr)NULL)
           {
           if (aneqty(stflop, base_typ) && aneqty(stintp,attr.type))
             {
             valu.lit.float_va = valu.lit.int_va;
             valu.lit_type = C_FLO_VA;
             }
           c_descr.type_co = attr.type;
           V3MOME(&valu,&c_descr.valu_co, sizeof(valu));
           if (cid_pm != (pm_ptr)NULL)
             stucon(cid_pm, &c_descr);
           }
         }
       else
         anperr("AN2123", "", NULL, e_line, e_col);        
                   
       }              
     else
/*
***Not an identifier
*/
       { 
       ancset(&loc_fol, fsys, 2, ANSYCOM, ANSYSEMI,0,0,0,0,0,0);
       anperr("AN2133", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
       }

     if (sy.sytype == ANSYCOM)
       {
       anascan(&sy);
       if (sy.sytype != ANSYID)
         {
         ancset(&loc_fol, fsys, 2, ANSYID, ANSYSEMI,0,0,0,0,0,0);
         anperr("AN2133", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
         }
       }
     else if (sy.sytype != ANSYSEMI)
       { 
       ancset(&loc_fol, fsys, 2, ANSYID, ANSYSEMI,0,0,0,0,0,0);
       anperr("AN2142", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
       } 
     }
   while (sy.sytype == ANSYID);

   if (sy.sytype == ANSYSEMI)
     anascan(&sy);
  }

/********************************************************/
/*!******************************************************/

        static void var_decl(
        ANFSET  *fsys)

/*      Analysis of the variable declaration.
 *
 *      var_declaration = var_type var_def ä',' var_defå ';'
 *      var_type = 'INT' ! 'FLOAT' ! 'VECTOR' ! 'REF' ! 'STRING' ! 'FILE'
 *
 *      In:  *fsys  =>  Follower set.
 *
 *      Out: 
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET   loc_fol;       /* Local follower set                */
   pm_ptr   base_typ;      /* Basic variable type               */
   pm_ptr   var_pm;        /* PM pointer to variable definition */
   pm_ptr   typ_pm;        /* PM pointer to variable type       */
   STVAR    var_descr;     /* Variable attributes               */
   STTYTBL  type_descr;    /* Type attributes                   */
   int      addsize;       /* add size to PM, JK 2004-07-22     */

   get_base(&base_typ);
   anascan(&sy);
 
   do
     {
     if (sy.sytype == ANSYID)
       {
       ancset(&loc_fol, fsys, 2, ANSYCOM, ANSYSEMI,0,0,0,0,0,0);
       var_def(base_typ, &var_pm, &typ_pm, &loc_fol);
       if (var_pm != (pm_ptr)NULL)
         {
         var_descr.type_va = typ_pm;
         var_descr.addr_va = anoffs;
         var_descr.kind_va = ST_LOCVA;
         var_descr.def_va = (pm_ptr)NULL;
         stuvar(var_pm, &var_descr);
         if (typ_pm != (pm_ptr)NULL)
           {
           strtyp(typ_pm, &type_descr);
/*
***Update PM offset
 */
           addsize = ((type_descr.size_ty % MIN_BLKS)!=0)?
           ((type_descr.size_ty+MIN_BLKS)/MIN_BLKS)*MIN_BLKS:
           type_descr.size_ty;

           anoffs += addsize;
           }
         }
       }
     else
/*
***Not an identifier
*/
       {
       ancset(&loc_fol, fsys, 2, ANSYCOM, ANSYSEMI,0,0,0,0,0,0);
       anperr("AN2163", "", &loc_fol, sy.sypos.srclin, sy.sypos.srccol);
       }

     if (sy.sytype == ANSYCOM)
       {
       anascan(&sy);
       if (sy.sytype != ANSYID)
         {
         ancset(&loc_fol, fsys, 2, ANSYID, ANSYSEMI,0,0,0,0,0,0);
         anperr("AN2163", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
         }
       }
     else if (sy.sytype != ANSYSEMI)
       {
       ancset(&loc_fol, fsys, 2, ANSYID, ANSYSEMI,0,0,0,0,0,0);
       anperr("AN2142", "", &loc_fol, sy.sypos.srclin,sy.sypos.srccol);
       }
     }
   while (sy.sytype == ANSYID);

   if (sy.sytype == ANSYSEMI) 
     anascan(&sy);
  }

/********************************************************/
/*!******************************************************/

        static void get_base(
        pm_ptr *type)

/*      Inspects whether current token is a symbol in FIRST
 *      (var_declaration) and in such cases returns corresponding 
 *      type pointer.
 *
 *      In:  
 *
 *      Out: *type  =>  PM pointer to the type descriptor.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   sttycl    class;       /* Type classification  */

   switch (sy.sytype)
     {
     case ANSYSTR:
       class = ST_STR;
       break;
     case ANSYFILE:
       class = ST_FILE;
       break;
     case ANSYINT:
       class = ST_INT;
       break;
     case ANSYFLT:
       class = ST_FLOAT;
       break;
     case ANSYVECT:
       class = ST_VEC;
       break;
     case ANSYREF:
       class = ST_REF;
       break;
     default:
       class = ST_UNDEF;
       break;
     }
   
   if (class != ST_UNDEF)
      stdtyp(class,(short)0, (pm_ptr *)type);
   else 
     type = NULL;
  }

/********************************************************/
/*!******************************************************/

        static void var_def(
        pm_ptr     base_typ,
        pm_ptr     *var_pm, 
        pm_ptr     *type_pm,
        ANFSET     *fsys)

/*      Recognises a variable definition. Inserts the variable and 
 *      its type into the symbol table.
 *
 *      variable_def = identifier Äindex_defÅ Ästring_defÅ
 *
 *      In:  base_typ  =>  PM pointer to the basic type. 
 *           fsys      =>  Follower set.                   
 *
 *      Out: *var_pm   =>  PM pointer to created variable.
 *           *type_pm  =>  PM pointer to variable type
 *                           Equal to "base_typ" if simple. 
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if (sy.sytype == ANSYID)        
/*
***Identifier. Create entry
*/
     {
     if ( stcvar(sy.syval.name, (pm_ptr)NULL, ST_UNDEF,
                                (DBint)0, (pm_ptr)NULL, var_pm) != 0 )
        anperr("AN2102", "", NULL, sy.sypos.srclin, sy.sypos.srccol);

     anascan(&sy);
     if (sy.sytype == ANSYLPAR)
       index_def(base_typ, type_pm, fsys);
     else if ((base_typ != (pm_ptr)NULL) && aneqty(ststrp, base_typ))
/*
***String
*/
       string_def(type_pm, fsys);
     else
       *type_pm = base_typ;
     }
   else
/*
***An identifier expected
*/
     {
     anperr("AN2163", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
     *var_pm = (pm_ptr)NULL;
     *type_pm = (pm_ptr)NULL;
     }
  }

/********************************************************/
/*!******************************************************/

        static void index_def(
        pm_ptr   base_typ,
        pm_ptr   *type_pm, 
        ANFSET   *fsys)

/*      Analysis of a variables index definition. 
 *
 *      index_def = '(' dimension ä',' dimension å ')'
 *      dimension = Älow ':'Å up
 *      low = constant_expression
 *      up  = constant_expression
 *
 *      In:  base_typ  =>  The basic type. 
 *           fsys      =>  Follower set.                   
 *
 *      Out: *var_pm   =>  PM pointer to the resulting type.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *      2004-07-12 low&high->int, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   int     low;           /* Lower index bound        */
   int     high;          /* Upper index bound        */
   pm_ptr  expr_p;        /* PM - constant expression */
   ANATTR  attr;          /* Expression attributes    */
   PMLITVA valu;          /* Expression value         */
   ANFSET  loc_fol;       /* Current follower set     */
   pm_ptr  arr_type;      /* Current type pointer     */

   anascan(&sy);          /* Skip ')' or ','          */

   low = 1;  high = 1;
   ancset(&loc_fol, fsys, 3, ANSYRPAR, ANSYCOM, ANSYCOL,0,0,0,0,0);
   ancoex(&expr_p, &attr, &valu, &loc_fol);
   if (attr.type != (pm_ptr)NULL)
     {
     if (aneqty(stintp, attr.type)) high = valu.lit.int_va;
     else anperr("AN2173", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
     }

   if (sy.sytype == ANSYCOL)
     {
     anascan(&sy);
     low = high;
     ancset(&loc_fol, fsys, 2, ANSYRPAR, ANSYCOL,0,0,0,0,0,0);
     ancoex(&expr_p, &attr, &valu, &loc_fol);
     if (attr.type != (pm_ptr)NULL)
       {
       if (aneqty(stintp, attr.type)) high = valu.lit.int_va;
       else anperr("AN2173", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
       }
     }

   if (low > high) anperr("AN2183", "", NULL, sy.sypos.srclin, sy.sypos.srccol);

   if ( sy.sytype == ANSYCOM )
     {
     index_def(base_typ, &arr_type, fsys);
     if (arr_type != (pm_ptr)NULL) stdarr(high, low, arr_type, type_pm);
     }
   else
/*
***No more dimensions. Find the tail of the definition
*/
     {
     if (sy.sytype == ANSYRPAR) anascan(&sy);
     else anperr("AN2082", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
/*
***Call string defintion if base type is STRING
*/
     if ((base_typ != (pm_ptr)NULL) && aneqty(base_typ, ststrp))
       string_def(&arr_type, fsys);
     else
       arr_type = base_typ;

     stdarr(high, low, arr_type, type_pm);
     }
  }

/********************************************************/
/*!******************************************************/

        static void string_def(
        pm_ptr *type_pm,
        ANFSET *fsys)

/*      Creates the internal form of a string definition.
 *
 *      string_def = '*' size
 *      size = constant_expression
 *
 *      In:  fsys      =>  Follower set. 
 *
 *      Out: *type_pm  =>  Resulting string type pointer.
 *
 *      (C)microform ab 1985-11-05 Kenth Ericson
 *
 *      1999-04-23 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr    expr_p;       /* Expression internal form */
   ANATTR    attr;         /* Expression attributes    */
   PMLITVA   valu;         /* Expression value         */

   *type_pm = (pm_ptr)NULL;

   if (sy.sytype == ANSYMUL)
     {
     anascan(&sy);
     ancoex(&expr_p, &attr, &valu, fsys);
     if (attr.type != (pm_ptr)NULL)
       {
       if (aneqty(stintp, attr.type))
         {
         if (valu.lit.int_va < 0)
/*
***Negative string size
*/
           {
           anperr("AN2243", "", NULL, sy.sypos.srclin,sy.sypos.srccol);
           valu.lit.int_va = 1;
           }
         if (valu.lit.int_va > ANSTRLEN)
/*
***String size too large
*/
           {
           anperr("AN2253", "", NULL, sy.sypos.srclin,sy.sypos.srccol);
           valu.lit.int_va = ANSTRLEN;
           }
         stdtyp(ST_STR,(short)valu.lit.int_va, type_pm);
         }
       }
     else
/*
***Integer type expected
*/
       anperr("AN2193", "", NULL, sy.sypos.srclin, sy.sypos.srccol);
     }
   else
     anperr("AN2202", "", fsys, sy.sypos.srclin, sy.sypos.srccol);
  }

#endif

/**********************************************************************/
