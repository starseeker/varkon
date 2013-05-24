/**********************************************************************
*
*    anpar12.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    Parser access functions for the MBS analyser in Varkon.
*    Function convention : on entry, next token in global sy
*                          on exit,  next token in global sy
*
*    This file includes the following routines:
*
*    anvar();           Analyse variable.
*    anfunc();          Analyse function_call.
*    anrefc();          Analyse reference (constant).
*    anidtc();          Analyse identifier constant.
*    anlitc();          Analyse literal constant
*    anparl();          Analyse parameter list.
*    anexpl();          Analyse expression list.
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/AN.h"

extern struct  ANSYREC sy;             /* scanner interface structure */
extern pm_ptr  stintp, stflop,         /* type descr. adresses */
               stvecp, strefp,
               ststrp, stfilp;
extern char    *typstr[];              /* typecode to string conversion */

/*!******************************************************/

        void anvar(
        pm_ptr *rptr,     
        ANATTR *attr,  
        ANFSET *set) 

/*      Analyse variable
 *
 *      variable ::= identifier Ä index Å Ä '.' component Å
 *      index ::= '(' expression_list ')'
 *      component ::= 'x' ö 'y' ö 'z'
 *
 *
 *      In:  *set    =>  Follow set
 *
 *      Out:  *rptr  =>  Pointer to expr. node
 *            *attr  =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-20 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char varnam[ANSYNLEN+1];        /* local var. name copy */
   ANATTR attrs[ANEXPMAX];         /* index attributes */
   short count = 0;                /* parameter count */
   ANFSET lset;                    /* local ANFSET */
   stidcl st_kind;
   pm_ptr varptr;                  /* pointer to var. ST-location */
   short varlin,varcol;            /* variable source location */
   pm_ptr idxptr = (pm_ptr)NULL;   /* if indexed - point to expr. list */
   pmfieldt fldnam='\0';           /* if fieldspec. - field name */
   bool idxflg = FALSE;            /* true if index */
   bool fldflg = FALSE;            /* true if field */
   STVAR vattr;                    /* variable attr. */
   STTYTBL typst;                  /* type interface */
   STARRTY arrst;                  /* array interface */
   pm_ptr tdpnt;                   /* PM-pointer to type descriptor */
   char eristr[32];                /* error insert string */
   char tstr[32];                  /* error type string */
   short i = 0;                    /* loop */
/*
***Save variable name and source location
*/
   strcpy(varnam,sy.syval.name);
   varlin = sy.sypos.srclin;
   varcol = sy.sypos.srccol;
/*
***Handle variable index
*/
   anascan(&sy);
   if ( sy.sytype == ANSYLPAR )
     {
     idxflg = TRUE;
     ancset(&lset,set,1,ANSYRPAR,0,0,0,0,0,0,0);
     anascan(&sy);
     anexpl(&idxptr,attrs,&count,&lset);
     if ( sy.sytype == ANSYRPAR )
       anascan(&sy);
     else
       anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
     }
/*
***Handle variable field
*/
   if ( sy.sytype == ANSYDOT )
     {
     anascan(&sy);
     if ( sy.sytype == ANSYID )
       {
       if ( sticmp(sy.syval.name,"x") == 0 )
         fldnam = PM_X;
       else if ( sticmp(sy.syval.name,"y") == 0 )
         fldnam = PM_Y;
       else if ( sticmp(sy.syval.name,"z") == 0 )
         fldnam = PM_Z;
       else
         anperr("AN9082",sy.syval.name,NULL,
         sy.sypos.srclin,sy.sypos.srccol);
         anascan(&sy);
         fldflg = TRUE;
       }
     else
       anperr("AN9082","",set,sy.sypos.srclin,sy.sypos.srccol);
     }
/*
***Check the index list
*/
   stlook(varnam,&st_kind,&varptr);               /* find variable in ST */
/*
***Read attributes
*/
   strvar(varptr,&vattr);
   tdpnt = vattr.type_va;
/*
***Get index count
*/
   strtyp(tdpnt,&typst);
   while( (typst.kind_ty == ST_ARR) && i < count )
     {
     i++;
     strarr(typst.arr_ty,&arrst);
     tdpnt = arrst.base_arr;
     strtyp(tdpnt,&typst);
     if ( !aneqty(attrs[i-1].type,stintp) )       /* index type = integer ? */
       {
       angtps(attrs[i-1].type,tstr);
       sprintf(eristr,"%d\004%s",i,tstr);
       anperr("AN9132",eristr,NULL,varlin,varcol);
       }
     }
/*
***Check for "too many indexes" 
*/
   if ( count > i )
     {
     sprintf(eristr,"%d\004%d",i,count);
     anperr("AN9142",eristr,NULL,varlin,varcol);
     attr->type = (pm_ptr)NULL;
     }
   else
     attr->type = tdpnt;
/*
***Create the PM-tree
*/
   if ( idxflg )
/*
***Create indexed expression
*/
     pmcine(varptr,idxptr,rptr);
   else 
/*
Create identifier expression
*/
     pmcide(varptr,rptr);

   if ( fldflg )
     {
     if ( !aneqty(attr->type,stvecp ) )
       {
/*
***Error : component must operate on type vector
*/
       angtps(attr->type,tstr);
       anperr("AN9152",tstr,NULL, varlin,varcol);
       attr->type = (pm_ptr)NULL;
       }
     else
       {
/*
Create identifier expression
*/
       pmccoe(*rptr,fldnam,rptr);
       attr->type = stflop;
       }
     }
   attr->rvclass = ST_VARCL;
   return;
  }

/********************************************************/
/*!******************************************************/

        void anfunc(
        pm_ptr *rptr, 
        ANATTR *attr, 
        ANFSET *set) 

/*      Analyse function_call.
 *
 *      Function_call ::= identifier '(' parameter list ')'
 *
 *
 *      In:  *set    =>  Follow set
 *
 *      Out:  *rptr  =>  Pointer to expr. node
 *            *attr  =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char funam[ANSYNLEN+1];         /* local func. name copy */
   ANATTR a_attrs[ANPARMAX];       /* parameter actual attributes */
   short acount;                   /* actual parameter count */
   pm_ptr elptr = (pm_ptr)NULL;    /* pointer to expr. list. */
   ANFSET lset;                    /* local ANFSET */
   stidcl st_kind;
   pm_ptr fuptr;                   /* pointer to ST-location */
   short fulin,fucol;              /* function source location */
/*
***Save function name and source location
*/
   strcpy(funam,sy.syval.name);
   ancset(&lset,set,1,ANSYRPAR,0,0,0,0,0,0,0);
   fulin = sy.sypos.srclin;
   fucol = sy.sypos.srccol;
/*
***Look for "("
*/
   anascan(&sy);
   if ( sy.sytype == ANSYLPAR )
     anascan(&sy);
   else
     anperr("AN9172","(",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Analyse the parameter list
*/
   anparl(&elptr,a_attrs,&acount,&lset);
/*
***Eat ")"
*/
   if ( sy.sytype == ANSYRPAR )
     anascan(&sy);
   else
     anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Check the parameter list
*/
   stlook(funam,&st_kind,&fuptr);         /* get function ST-adress */
/*
***Ceck the parameter list
*/
   ancpar(a_attrs,acount,fuptr,fulin,fucol,0,attr);
/*
***Create the PM tree
*/
   pmcfue(fuptr,elptr,rptr);
   return;
  }

/********************************************************/
/*!******************************************************/

        void anrefc(
        pm_ptr *rptr,  
        ANATTR *attr,  
        ANFSET *set) 

/*      Analyse reference (constant).
 *
 *      In:  *set   =>  Follow set
 *
 *      Out: *rptr  =>  Pointer to expr. node
 *           *attr  =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short refind = 0;               /* referens index */
   PMLITVA litval;                 /* literal access struct. */
   short seqlin,seqcol;            /* source position */
/*
***Save source position
*/
   seqlin = sy.sypos.srclin;
   seqcol = sy.sypos.srccol;

   do
     {
/*
***Bara 1:a sekvensnumret får vara negativt (global referens).
***Detta kollas här. 3/11-94 JK
*/
     if ( refind > 0  &&  sy.syval.rval.seq_val < 0 )
       {
       anperr("AN1112",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
       }
/*
***Store sequenze number
*/
     litval.lit.ref_va[refind].seq_val = sy.syval.rval.seq_val;
/*
***Store order number
*/
     if ( sy.syval.rval.ord_val == 0 ) 
       litval.lit.ref_va[refind].ord_val = 1;
     else
       litval.lit.ref_va[refind].ord_val = sy.syval.rval.ord_val; 
/*
***Link to next PMREFVA
*/
     litval.lit.ref_va[refind].p_nextre = &litval.lit.ref_va[refind+1];
     refind++;
     anascan(&sy);
     } 

   while(sy.sytype == ANSYREFL);

   litval.lit.ref_va[refind-1].p_nextre = NULL;      /* store default link */
/*
***Check the reference
*/
#ifdef ANALYZER
   if ( litval.lit.ref_va[0].seq_val >= 1 )
     ancsnr(seqlin,seqcol,litval.lit.ref_va[0].seq_val);
#endif
/*
***Create PM literal
*/
   litval.lit_type = C_REF_VA;
   pmclie(&litval,rptr);

   attr->type = strefp;
   attr->rvclass = ST_CONCL;
   return;
  }

/********************************************************/
/*!******************************************************/

        void anidtc(
        pm_ptr *rptr,  
        ANATTR *attr,  
        ANFSET *set) 

/*      Analyse identifier constant
 *
 *      In:  *set   =>  Follow set
 *
 *      Out: *rptr  =>  Pointer to expr. node
 *           *attr  =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl st_kind;
   pm_ptr conptr;                  /* pointer to var. ST-location */
   STCONST st_con;                 /* interface struct. */
/*
***Get constant ST-adress from ST
*/
   stlook(sy.syval.name,&st_kind,&conptr);

/*
***Get const. info
*/
   strcon(conptr,&st_con);
/*
***Get attributes
*/
   attr->rvclass = ST_CONCL;
   attr->type = st_con.type_co;
/*
Create PM expr.
*/
   pmcide(conptr,rptr);            /* create PM ident. expr. */
   anascan(&sy);                   /* next token */
   return;
  }

/********************************************************/
/*!******************************************************/

        void anlitc(
        pm_ptr *rptr,  
        ANATTR *attr,  
        ANFSET *set) 

/*      Analyse literal constant.
 *
 *      In:  *set   =>  Follow set
 *
 *      Out: *rptr  =>  Pointer to expr. node
 *           *attr  =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMLITVA litval;                 /* literal value struct. */

   switch(sy.sytype)
     {
     case ANSYSTRL:
/*
***String literal
*/
       strcpy(litval.lit.str_va,sy.syval.sval);
       litval.lit_type=C_STR_VA;
       attr->type = ststrp;
       break;
     case ANSYINTL:
/*
***Integer literal
*/
       litval.lit.int_va=sy.syval.ival;
       litval.lit_type=C_INT_VA;
       attr->type = stintp;
       break;
     case ANSYDECL:
/*
***String literal
*/
       litval.lit.float_va=sy.syval.fval;
       litval.lit_type=C_FLO_VA;
       attr->type = stflop;
       break;
     default:
       printf("anlitc - internal error : unknown constant type\n");
       break;
     }
   attr->rvclass = ST_CONCL;
/*
***Create PM literal expr.
*/
   pmclie(&litval,rptr);

   anascan(&sy);
   return;
  }

/********************************************************/
/*!******************************************************/

        void anparl(
        pm_ptr *rptr, 
        ANATTR  attrs[], 
        short  *count, 
        ANFSET *set) 

/*      Analyse parameter list.
 *
 *      Parameter_list ::= Ä parameter ä ',' parameter å Å
 *
 *      In:  *set      =>  Follow set
 *
 *      Out: *rptr     =>  Pointer to expr. list.
 *           *attrs[]  =>  Attribute vector.
 *           *count    =>  Parameter/attribute count. 
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lrptr;
   pm_ptr dumptr;                  /* dummy */
   ANFSET lset;                    /* local follow set */
/*
***Initialize
*/
   *count=0;                       /* reset parameter count */
   *rptr=(pm_ptr)NULL;             /* init rptr */

/*
***Check for empty list
*/
   if ( aniset(sy.sytype, set) )
     return;
/*
***Create local follow set
*/
   ancset(&lset,set,1,ANSYCOM,0,0,0,0,0,0,0);         /* create local set */
/*
***Analyse first expression
*/
   anarex(&lrptr,&attrs[*count],&lset);
   (*count)++;
/*
***Link expression to expression list
*/
   pmtcon(lrptr,*rptr,rptr,&dumptr);
/*
***Analyse the following expressions
*/
   while ( sy.sytype == ANSYCOM )
     {
     anascan(&sy);
     if ( sy.sytype == ANSYCOM )
/*
***Default value
*/
       {
       lrptr=(pm_ptr)NULL;
       attrs[*count].type = (pm_ptr)NULL;
       attrs[*count].rvclass = ST_DEFCL;
       }
     else
/*
***Expression
*/
       anarex(&lrptr,&attrs[*count],&lset);

     (*count)++;
/*
***Link expression to expression list
*/
     pmtcon(lrptr,*rptr,rptr,&dumptr);
/*
***Check for too many parameters
*/
     if ( *count == ANPARMAX )
       (*count)--;
     }

    return;
  }

/********************************************************/
/*!******************************************************/

        void anexpl(
        pm_ptr *rptr, 
        ANATTR  attrs[],    
        short  *count,  
        ANFSET *set) 

/*      Analyse expression list.
 *
 *      Expression_list ::= arith_expression ä ',' arith_expression å
 *
 *      In:  *set      =>  Follow set
 *
 *      Out: *rptr     =>  Pointer to expr. list.
 *           *attrs[]  =>  Attribute vector.
 *           *count    =>  Parameter/attribute count. 
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lrptr;
   pm_ptr dumptr;                  /* dummy */
   ANFSET lset;                    /* local follow set */
   *count = 0;                     /* reset parameter count */
   *rptr=(pm_ptr)NULL;             /* init rptr */
/*
***Create local follow set
*/
   ancset(&lset,set,1,ANSYCOM,0,0,0,0,0,0,0);/* create local set */
/*
***Analyse first expression
*/
   anarex(&lrptr,&attrs[*count],&lset);
   (*count)++;
/*
***Link expression to expression list
*/
   pmtcon(lrptr,*rptr,rptr,&dumptr);

   while ( sy.sytype == ANSYCOM )
     {
     anascan(&sy);
/*
***Analyse next expression
*/
     anarex(&lrptr,&attrs[*count],&lset);
     (*count)++;
/*
***Link expression to expression list
*/
     pmtcon(lrptr,*rptr,rptr,&dumptr);
     } 
   return;
  }

/**********************************************************************/
