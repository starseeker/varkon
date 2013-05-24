/**********************************************************************
*
*    anpar52.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    MBS statement parser and generator of the internal form.
*
*    This file includes the following routines:
*
*    anlgex();            Analyse log_expression
*    anlter();            Analyse logical term
*    anlpri();            Analyse logical primary
*    anlrel();            Analyse relation
*    anifst();            Analyse IF statement
*    anfost();            Analyse FOR statement
*    angost();            Analyse GOTO statement
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

extern struct ANSYREC sy;          /* scanner interface variable */

extern pm_ptr stintp;              /* PM-pointer to integer type descr. */

/********************************************************/

        void anlgex(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse logical expression
 *
 *      log_expression ::= Ä'NOT'Å log_term ä 'OR' log_term å
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to log. expression.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lterm;                   /* left term */
   pm_ptr rterm;                   /* right term */
   ANFSET locfol;                  /* local follow set */
   bool negated=FALSE;             /* TRUE if negated */
/* 
***Create local follow set
*/
   ancset(&locfol,follow,1,ANSYOR,0,0,0,0,0,0,0);
/*
***Check for leading NOT
*/
   if (sy.sytype == ANSYNOT)
     {
     negated=TRUE;
     anascan(&sy);
     }
/*
**Analyse leftmost term
*/
   anlter(&lterm,&locfol);
/*
***If negated
*/
   if (negated)
/*
***Create PM unary expr.
*/
     pmcune(PM_NOT,lterm,&lterm);

next:
/* 
***Check for OR
*/

   if (sy.sytype != ANSYOR)
/*
***Return from function
*/
     {
     *rptr=lterm;
     return;
     }
/*
***Skip operator
*/
   anascan(&sy);
/*
***Analyse next term
*/
   anlter(&rterm,&locfol);
/*
***Create PM binary expression
*/
   pmcbie(PM_OR,lterm,rterm,&lterm);

   goto next;
  }

/********************************************************/
/*!******************************************************/

        void anlter(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse logical term
 *
 *      log_term ::= log_primary ä 'AND' log_primary å
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to log. term.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lprim;                   /* left primary */
   pm_ptr rprim;                   /* right primary */
   ANFSET locfol;                  /* local follow set */
/*
**Create local follow set
*/
   ancset(&locfol,follow,1,ANSYAND,0,0,0,0,0,0,0);
/*
***Analyse leftmost primary
*/
   anlpri(&lprim,&locfol);

next:
/*
***Check for AND
*/
   if (sy.sytype != ANSYAND)
/*
*****Return from function
*/
     {
     *rptr=lprim;
     return;
     }
/*
***Skip operator
*/
   anascan(&sy);
/*
***Analyse next primary
*/
   anlpri(&rprim,&locfol);
/*
***Create PM binary expression
*/
   pmcbie(PM_AND,lprim,rprim,&lprim);

   goto next;
  }

/********************************************************/
/*!******************************************************/

        void anlpri(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse logical primary
 *
 *      log_primary ::= '(' log_expression ')' ö relation
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to log. primary.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   ANFSET locfol;                   /* local follow set */
/* 
***Create local follow set
*/
   ancset(&locfol,follow,1,ANSYRPAR,0,0,0,0,0,0,0);
/*
***Check for "(" 
*/
   if (sy.sytype != ANSYLPAR)
/*
***Ralation
*/
     anlrel(rptr,follow);           /* analyse (logical) relation */
   else
     {
/*
***( log_expression )
*/
     anascan(&sy);
     anlgex(rptr,&locfol);
     if (sy.sytype == ANSYRPAR)
       anascan(&sy);
     else
       anperr("AN9172",")",follow,sy.sypos.srclin,sy.sypos.srccol);
     }
   return;
  }

/********************************************************/
/*!******************************************************/

        void anlrel(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse relation
 *
 *      relation ::= arith_expression relop arith_expression
 *      relop ::= '<' ö '>' ö '<>' ö '<=' ö '>=' ö '='
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to log. relation.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lexp;                     /* left expr. */
   ANATTR lattr;                    /* left expr. attributes */
   pm_ptr rexp;                     /* right expr. */
   ANATTR rattr;                    /* right expr. attributes */
   ANATTR dattr;                    /* dummy attributes */
   ANSYTYP relop;                   /* relation operator */
   short roplin,ropcol;             /* relation operator location */
   pmopty pmrop;                    /* relation operator - PM-code */
   ANFSET locfol;                   /* local follow set */
/*
***Create local follow set
*/
   ancset(&locfol,follow,6,ANSYLT,ANSYGT,ANSYNEQ,ANSYLE,ANSYGE,ANSYEQ,0,0);
/*
***Analyse left expr
*/
   anarex(&lexp,&lattr,&locfol);
/*
***Store relation operand location
*/
   roplin=sy.sypos.srclin;
   ropcol=sy.sypos.srccol;
/*
***Eat realtion operator
*/
   relop=sy.sytype;
   if ( (relop == ANSYLT)  || (relop == ANSYGT) ||
        (relop == ANSYNEQ) || (relop == ANSYLE) ||
        (relop == ANSYGE)  || (relop == ANSYEQ) )
     anascan(&sy);
   else
     {
     anperr("AN9262","",NULL,roplin,ropcol);
     relop = ANSYLE+1;
     }
/* 
***Convert scanner operator code to PM code
*/
   switch(relop)
     {
     case ANSYLT:
       pmrop=PM_LT;
       break;
     case ANSYGT:
       pmrop=PM_GT;
       break;
     case ANSYNEQ:
       pmrop=PM_NE;
       break;
     case ANSYLE:
       pmrop=PM_LE;
       break;
     case ANSYGE:
       pmrop=PM_GE;
       break;
     case ANSYEQ:
       pmrop=PM_EQ;
       break;
     default:
       pmrop=PM_NE;                  /* dummy, to be able to continue */
       break;
     }
/*
***Analyse right expr
*/
   anarex(&rexp,&rattr,follow);
/*
***Check binary combination
*/
   anbcom(roplin,ropcol,relop,&lattr,&rattr,&dattr);
/*
***Create PM binary expression
*/
   pmcbie(pmrop,lexp,rexp,rptr);

   return;
  }

/********************************************************/
/*!******************************************************/

        void anifst(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyses a IF statement
 *
 *      if_statement ::= 'IF' log_expression 'THEN' statements ä 'ELIF'
 *                            log_expression 'THEN' statements å Ä 'ELSE'
 *                            statements Å 'ENDIF'
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to IF statement.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lexp;                    /* PM-pointer to log_expr */
   pm_ptr statl;                   /* PM-pointer to statement list */
   pm_ptr cond;                    /* PM-pointer to condition node */
   pm_ptr condl=(pm_ptr)NULL;      /* PM-pointer to condition list */
   pm_ptr dumptr;                  /* dummy */
   ANFSET locfol1;                 /* local follow set */
   ANFSET locfol2;                 /* local follow set */
   ANFSET locfol3;                 /* local follow set */
/*
***Create local follow set
*/
   ancset(&locfol1,follow,1,ANSYTHEN,0,0,0,0,0,0,0);
   ancset(&locfol2,follow,3,ANSYELIF,ANSYELSE,ANSYEIF,0,0,0,0,0);
   ancset(&locfol3,follow,1,ANSYEIF,0,0,0,0,0,0,0);

   do
     {
     anascan(&sy);
/*
***Analyse log_expr.
*/
     anlgex(&lexp,&locfol1);
/*
***Check for THEN
*/
     if (sy.sytype == ANSYTHEN)
       anascan(&sy);
     else
       anperr("AN9272","THEN",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Analyse statement list
*/
     ansmts(&statl,&locfol2);
/*
***Create PM condition node
*/
     pmccon(lexp,statl,&cond);
/*
***Update PM condition list
*/
     pmtcon(cond,condl,&condl,&dumptr);
     }
     while(sy.sytype == ANSYELIF);
/*
***Check for ELSE
*/
   statl=(pm_ptr)NULL;

   if (sy.sytype == ANSYELSE)
     {
     anascan(&sy);
/*
***Analyse statement list
*/
     ansmts(&statl,&locfol3);
     }
/*
***Check for ENDIF
*/
   if (sy.sytype == ANSYEIF)
     anascan(&sy);
   else
     anperr("AN9272","ENDIF",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Create PM if statement
*/
   pmcifs(condl, statl, rptr);
  }

/********************************************************/
/*!******************************************************/

        void anfost(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse FOR statement
 *
 *      for_statement ::= 'FOR' identifier ':=' arith_expression 'TO'
 *                        arith_expression Ä 'STEP' arith_expression Å
 *                        'DO' statements ' ENDFOR'
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to FOR statement.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STVAR var;                     /* ST variable access struct. */
   pm_ptr fromex;                 /* PM-pointer to "from value"-expression */
   pm_ptr toex;                   /* PM-pointer to "to to value"-expression*/
   pm_ptr stepex=(pm_ptr)NULL;    /* PM-pointer to "step value"-expression */
   pm_ptr statl;                  /* PM-pointer to statement list */
   ANATTR attr;                   /* attributes */
   char eristr[32];               /* error insert strings */
   char tstr1[32],tstr2[32];
   short lin,col;                 /* source location */
   stidcl st_typ;                 /* variable type class */
   pm_ptr st_ent;                 /* variable ST entry */
   ANFSET locfol;                 /* local follow set */
/*
***Skip FOR
*/
   anascan(&sy);
/*
***Save variable source position and create local follow set
*/
   lin=sy.sypos.srclin;
   col=sy.sypos.srccol;
   ancset(&locfol,follow,1,ANSYASGN,0,0,0,0,0,0,0);
/*
***Read variable
*/
   if ( sy.sytype == ANSYID )
     {
     stlook(sy.syval.name, &st_typ, &st_ent);

     if ( st_typ == ST_VAR )
       {
       strvar(st_ent,&var);
       if ( !aneqty(var.type_va,stintp) )
         anperr("AN9282","",&locfol,lin,col);
       else
         anascan(&sy);
       }
     else
       anperr("AN9282","",&locfol,lin,col);
     }
   else
     anperr("AN9282","",&locfol,lin,col);
/*
***Eat :=
*/
   if (sy.sytype == ANSYASGN)
     anascan(&sy);
   else
     anperr("AN9172",":=",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Save from-expr. source position and create local follow set
*/
   lin=sy.sypos.srclin;
   col=sy.sypos.srccol;
   ancset(&locfol,follow,1,ANSYTO,0,0,0,0,0,0,0);
/*
***Analyse "from" arith_expr. 
*/
   anarex(&fromex,&attr,&locfol);
/*
***Check integer expr.
*/
   if ( !aneqty(attr.type, stintp) )
     {
     angtps(stintp,tstr1);
     angtps(attr.type,tstr2);
     sprintf(eristr,"%s\004%s", tstr1, tstr2);
     anperr("AN9292",eristr,NULL,lin,col);
     }
/*
***Eat TO
*/
   if (sy.sytype == ANSYTO)
     anascan(&sy);
   else
     anperr("AN9272","TO",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Save to-expr. source position and create local follow set
*/
   lin=sy.sypos.srclin;
   col=sy.sypos.srccol;
   ancset(&locfol,follow,2,ANSYDO,ANSYSTEP,0,0,0,0,0,0);
/*
***Analyse "to" arith_expr
*/
   anarex(&toex,&attr,&locfol);
/*
***Check integer expr.
*/
   if ( !aneqty(attr.type, stintp) )
     {
     angtps(stintp,tstr1);
     angtps(attr.type,tstr2);
     sprintf(eristr,"%s\004%s", tstr1, tstr2);
     anperr("AN9292",eristr,NULL,lin,col);
     }
/*
***STEP ? 
*/
   if (sy.sytype == ANSYSTEP)
     {
     anascan(&sy);
/*
***Save step-expr. source position and create local follow set
*/
     lin=sy.sypos.srclin;
     col=sy.sypos.srccol;
     ancset(&locfol,follow,1,ANSYDO,0,0,0,0,0,0,0);
/*
***Analyse "step" arith_expr
*/
     anarex(&stepex,&attr,&locfol);
/*
***Check integer expr.
*/
     if ( !aneqty(attr.type, stintp) )
       {
       angtps(stintp,tstr1);
       angtps(attr.type,tstr2);
       sprintf( eristr, "%s\004%s", tstr1, tstr2 );
       anperr("AN9292",eristr,NULL,lin,col);
       }
     }
/*
***Eat DO
*/
   if (sy.sytype == ANSYDO)
     anascan(&sy);
   else
     anperr("AN9272","DO",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Analyse statement list
*/
   ancset(&locfol,follow,1,ANSYEFOR,0,0,0,0,0,0,0);
   ansmts(&statl,&locfol);
/*
***Check for ENDFOR
*/
   if (sy.sytype == ANSYEFOR)
     anascan(&sy);
   else
     anperr("AN9272","ENDFOR",follow,sy.sypos.srclin,sy.sypos.srccol);
/*
***Create PM FOR-statement
*/
   pmcfos(st_ent,fromex,toex,stepex,statl,rptr);
   return;
  }

/********************************************************/
/*!******************************************************/

        void angost(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse GOTO statement
 *
 *      goto_statement ::= 'GOTO' identifier
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to GOTO statement.
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr labptr;                   /* PM-pointer to label entry in ST */
   stidcl st_typ;                   /* identifier type */
   STLABEL labattr;                 /* ST label attributes */
/*
***Skip GOTO
*/
   anascan(&sy);
/*
***Check for identifier
*/
   if (sy.sytype != ANSYID)
     {
     anperr("AN9302","",follow,sy.sypos.srclin,sy.sypos.srccol);
     rptr=NULL;
     return;
     }
   else
     {
/*
***If new name:  create "unreferensed" label
***If old name: check for type "STLABEL"
*/
     stlook(sy.syval.name,&st_typ,&labptr); /* consult symbol table */
     if (labptr == (pm_ptr)NULL)
/*
***New name, create new ST entry
*/
       stclab(sy.syval.name,&labptr);
     else if (st_typ != ST_LABEL)
/*
***Old name, but not a label
*/
       {
       anperr("AN9312","",NULL,sy.sypos.srclin,sy.sypos.srccol);
       anascan(&sy);
       rptr=NULL;
       return;
       }
     }
/*
***Create PM GOTO statement
*/
   pmcgos(labptr,rptr);
/*
***Update label info. (referenced)
*/
   strlab(labptr, &labattr);
   labattr.ref_la = TRUE;
   stulab(labptr, &labattr);
/*
***Skip label identifier
*/
   anascan(&sy);
   return;
  }

/*********************************************************************/
