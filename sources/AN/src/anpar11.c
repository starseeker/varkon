/**********************************************************************
*
*    anpar11.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    Parser access functions for MBS analyser in Varkon
*
*    This file includes the following routines:
*
*    ancoex();         Analyse constant expression.
*    anarex();         Analyse arithmetic expression.
*    anterm();         Analyse term.
*    anfact();         Analyse factor.
*    anprim();         Analyse primary.
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


extern struct ANSYREC sy;          /* scanner interface structure */

extern pm_ptr stintp, stflop,      /* type descr. adresses */
              stvecp, strefp,
              ststrp;

/*!******************************************************/

        void ancoex(
        pm_ptr  *rptr,  
        ANATTR  *attr,  
        PMLITVA *value, 
        ANFSET  *set)   

/*      Analyse constant expression.
 *
 *      constant_expression ::= arith_expression with semantic restrictions
 *      restrictions : value class = constant, therefor only functions
 *      that can return constant value may be used in such expression
 *
 *
 *      In:   *value  =>  Value access structure
 *            *set    =>  Follow set
 *
 *      Out:  *rptr   =>  Pointer to expr. node
 *            *attr   =>  Attributes
 *
 *      (C)microform ab 1985-03-09 M. Nelson
 * 
 *      1999-04-20 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*1999-04-26JK    
   STTYTBL type;                      constant value type   */

   pm_ptr type;                    /* constant value type   */
   short explin,expcol;            /* expr. source location */
/*
***Save expression source location
*/
   explin = sy.sypos.srclin;
   expcol = sy.sypos.srccol;
/*
***Analyse arith_expr.
*/
   anarex(rptr,attr,set);
   if ( attr->rvclass != ST_CONCL )
     {
/*
***Not constant expression
*/
     anperr("AN9322","",NULL,explin,expcol);
     attr->type = (pm_ptr)NULL;
     }
   else
     {
/*
***Constant okey, evaluate it 
*/
     if ( inevex(*rptr,value,&type) != 0 )
       {
/*
***Evaluation error
*/
       anperr("AN9332","",NULL,explin,expcol);
       attr->type = (pm_ptr)NULL;
       }
     }
   return;
  }

/********************************************************/
/*!******************************************************/

        void anarex(
        pm_ptr *rptr,  
        ANATTR *attr, 
        ANFSET *set)

/*      Analyse arithmetic expression
 *
 *      arith_expression ::= Ä adop Å term ä adop term å
 *
 *
 *      In:   *set   =>  Follow set
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
   pm_ptr lterm;                /* left term */
   ANATTR lattr;                /* left term attr. */
   pm_ptr rterm;                /* right term */
   ANATTR rattr;                /* right term attr. */
   ANSYTYP unop=0;              /* unary operator type ANSYPLUS or ANSYMINS */
   bool signd=FALSE;            /* TRUE if the leading term i signed */
   ANSYTYP adop;                /* binary operator type ANSYPLUS or ANSYMINS */
   short oplin,opcol;           /* binary operator location */
   short uoplin=0,uopcol=0;     /* unary operator location */

/*
***Deal with leading operator "+" or "-"
*/
   if ( sy.sytype == ANSYPLUS )
     anascan(&sy);
   else if ( sy.sytype == ANSYMINS )
     {
     signd = TRUE;
     unop = sy.sytype;
     uoplin = sy.sypos.srclin;
     uopcol = sy.sypos.srccol;
     anascan(&sy);
     }
/*
***Analyse left term
*/
   anterm(&lterm,&lattr,set);
/*
***If signed, create PM unary erpression
*/
   if ( signd )
     {
     if ( unop == ANSYPLUS )
       pmcune(PM_PLUS,lterm,&lterm);
     else
       pmcune(PM_MINUS,lterm,&lterm);

     anucom(uoplin,uopcol,unop,&lattr,&lattr);
     }

next:
/*
***Look for adop
*/
   adop=sy.sytype;
   if ( (adop != ANSYMINS) && (adop != ANSYPLUS) )
     {
/*
***No adop found, exit
*/
     V3MOME(&lattr,attr,sizeof(ANATTR));
     *rptr=lterm;
     return;
     }
/*
***Adop found, store it's source position
*/
   oplin = sy.sypos.srclin;
   opcol = sy.sypos.srccol;
/*
***Analyse next term
*/
   anascan(&sy);
   anterm(&rterm,&rattr,set);
/*
***Create PM binary expression
*/
   anbcom(oplin,opcol,adop,&lattr,&rattr,&lattr);
   if ( adop == ANSYPLUS )
     pmcbie(PM_PLUS,lterm,rterm,&lterm);
   else
     pmcbie(PM_MINUS,lterm,rterm,&lterm);

    goto next;
  }

/********************************************************/
/*!******************************************************/

        void anterm(
        pm_ptr *rptr, 
        ANATTR *attr,  
        ANFSET *set)

/*      Analyse term.
 *
 *      term ::= factor ä mulop factor å
 *
 *
 *      In:   *set   =>  Follow set
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
   pm_ptr lfact;                   /* left factor */
   ANATTR lattr;                   /* left factor attr. */
   pm_ptr rfact;                   /* right factor */
   ANATTR rattr;                   /* right factor attr. */
   ANSYTYP mulop;                  /* operator type ANSYMUL or ANSYDIV */
   short oplin,opcol;              /* operator location */

/*
***Analyse left factor
*/
   anfact(&lfact,&lattr,set);

next:
/*
***Look for mulop
*/
   mulop=sy.sytype;
   if ( (mulop != ANSYMUL) && (mulop != ANSYDIV) )
     {
/*
***No mulop found, exit
*/
     V3MOME(&lattr,attr,sizeof(ANATTR));
     *rptr = lfact;
     return;
     }
/*
***Mulop found, store it's source position
*/
   oplin = sy.sypos.srclin;
   opcol = sy.sypos.srccol;
/*
***Analyse next factor
*/
   anascan(&sy);
   anfact(&rfact,&rattr,set);
/*
***Create PM binary expression
*/
   anbcom(oplin,opcol,mulop,&lattr,&rattr,&lattr);
   if ( mulop == ANSYMUL )
     pmcbie(PM_MUL,lfact,rfact,&lfact);
   else
     pmcbie(PM_DIV,lfact,rfact,&lfact);

    goto next;
  }

/********************************************************/
/*!******************************************************/

        void anfact(
        pm_ptr *rptr, 
        ANATTR *attr, 
        ANFSET *set)

/*      Analyse factor.
 *
 *      Factor ::= primary ä '**' primary å
 *
 *
 *      In:   *set   =>  Follow set
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
   pm_ptr lprim;                   /* left primary */
   ANATTR lattr;                   /* left primary attr. */
   pm_ptr rprim;                   /* right primary */
   ANATTR rattr;                   /* right primary attr. */
   ANSYTYP expop;                  /* operator attr ANSYEXP */
   short oplin,opcol;              /* operator location */
/*
***Analyse left primary
*/
   anprim(&lprim,&lattr,set);

next:
/*
***Look for '**' 
*/
   expop = sy.sytype;
   if ( expop != ANSYEXP )
     {
/*
***No '**' found, exit
*/
     V3MOME(&lattr,attr,sizeof(ANATTR));
     *rptr = lprim;
     return;
     }
/*
*** '**' found, store it's source position
*/
   oplin = sy.sypos.srclin;
   opcol = sy.sypos.srccol;
/*
Analyse next primary
*/
   anascan(&sy);
   anprim(&rprim,&rattr,set);
/*
***Create PM binary expression
*/
   anbcom(oplin,opcol,expop,&lattr,&rattr,&lattr);/* check binary comb. */
   pmcbie(PM_EXP,lprim,rprim,&lprim);

   goto next;
  }

/********************************************************/
/*!******************************************************/

        void anprim(
        pm_ptr *rptr,  
        ANATTR *attr, 
        ANFSET *set)   

/*      Analyse primary.
 *
 *      primary ::= '(' arith_expression ')' ö constant ö variable ö 
 *                                                     function_call
 *
 *      In:   *set   =>  Follow set
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
   ANFSET lset;                    /* local ANFSET */
   pm_ptr st_ent;                  /* ST entry point */
   stidcl st_typ;                  /* ST type */
/*
***Parenthesis expression :
*/
   if ( sy.sytype == ANSYLPAR )
     {
     anascan(&sy);
     ancset(&lset,set,1,ANSYRPAR,0,0,0,0,0,0,0);
     anarex(rptr,attr,&lset);
     if ( sy.sytype == ANSYRPAR )             /* check for ")" */
       anascan(&sy);
     else
       anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
     }
/*
***Literal : 
*/
   else if ((sy.sytype == ANSYSTRL )||        /* constant string literal ?  */
            (sy.sytype == ANSYINTL)||         /* constant integer literal ? */
            (sy.sytype == ANSYDECL) )         /* constant decimal literal ? */
     anlitc(rptr,attr,set);
/*
***Reference literal : 
*/
   else if ( sy.sytype == ANSYREFL)           /* reference */
     anrefc(rptr,attr,set);
/*
Identifier : 
*/
   else if ( sy.sytype == ANSYID)
     {
     stlook(sy.syval.name,&st_typ,&st_ent);   /* consult symbol table */

     switch(st_typ)
       {
/*
***Variable
*/
       case ST_VAR:
         anvar(rptr,attr,set);
         break;
/*
***Constant
*/
       case ST_CONST:
         anidtc(rptr,attr,set);
         break;
/*
***Function
*/
       case ST_FUNC:
         anfunc(rptr,attr,set);
         break;
/*
*** .. undefined
*/
       case ST_UNDEF:
         anperr("AN9022",sy.syval.name,NULL,
                 sy.sypos.srclin,sy.sypos.srccol);
         anascan(&sy);
         attr->type = (pm_ptr)NULL;
         attr->rvclass = ST_VARCL;
         *rptr = (pm_ptr)NULL;
         break;
/*
*** .. defined but illegal 
*/
       default:                               /* ST_LABEL, ST_MOD, ST_PROC : */
         anperr("AN9092",sy.syval.name,NULL,
                 sy.sypos.srclin,sy.sypos.srccol);
         anascan(&sy);
         attr->type = (pm_ptr)NULL;
         attr->rvclass = ST_VARCL;
         *rptr = (pm_ptr)NULL;
         break;
       }
     }

/*
***Illegal / unknown primary 
*/
   else
     {
     anperr("AN9052","",set,sy.sypos.srclin,sy.sypos.srccol);
     attr->type = (pm_ptr)NULL;
     attr->rvclass = ST_VARCL;
     *rptr = (pm_ptr)NULL;
     }

   return;
  }

/*********************************************************************/
