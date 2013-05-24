/**********************************************************************
*
*    anpar51.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://www.varkon.com
*
*    MBS statement parser and generator of the internal form.
*
*    This file includes the following routines:
*
*    ansmts()            Analyse statements
*    ansmte()            Analyse statement
*    anlast()            Analyse single labeled statement
*    anunst()            Analyse single unlabeled statement
*    anoprc()            Analyse ordinary ( and set ) procedure call
*    angprc()            Analyse geometric( and part ) procedure call
*    anasst()            Analyse assignment statement
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../PM/include/mbsrout.h"
#include "../include/AN.h"
#include <string.h>

extern struct ANSYREC sy;       /* Scanner interface variable  */
extern short  anstyp;           /* Source-type ANRDFIL/ANRDSTR */

extern pm_ptr ststrp,stvecp,    /* typedescr. (STTYTBL)        */
              stflop,stintp,
              strefp,stfilp;

/********************************************************/
/*!******************************************************/

        void ansmts(
        pm_ptr *rptr,
        ANFSET *follow)

/*      Analyse MBS statements.
 *
 *      statements ::= statement ä statement å
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to statement list.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lrptr;          /* local PM-pointer to statement */
   pm_ptr nodla;          /* pointer to statement list node */
   pm_ptr labptr;         /* ST label entry */
   STLABEL labattr;       /* ST label attributes */
   ANFSET locfol;         /* local followset */
   ANFSET sync_set;
   ANFSET beg_set;
/*
***Create local followset
*/
   ancset(&locfol,follow,1,ANSYSEMI,0,0,0,0,0,0,0);
/*
***Create sync_set
*/
   ancset(&sync_set,follow,5,ANSYSEMI,ANSYIF,ANSYFOR,ANSYGOTO,ANSYID,0,0,0);
/*
***Create beg_set
*/
   ancset(&beg_set,NULL,5,ANSYSEMI,ANSYIF,ANSYFOR,ANSYGOTO,ANSYID,0,0,0);
/*
***Create empty statement list
*/
   *rptr = (pm_ptr)NULL;            /* init. statement list = empty */
/*
***Loop and eat statements
*/
   do
     {
     if ( sy.sytype == ANSYSEMI )
/*
***Empty statement
*/
       {
       lrptr = (pm_ptr)NULL;
       labptr = (pm_ptr)NULL;
       }
     else
       {
/*
***Analyse statement (not empty)
*/
       ansmte(&lrptr,&locfol,&labptr);
       }
/*
***Link statement to the statement list
*/
     pmtcon(lrptr,*rptr,rptr,&nodla);
/*
***If labeled statement : update ST entry 
*/
     if ( labptr != (pm_ptr)NULL )
       {
       strlab(labptr,&labattr);
       labattr.list_la = *rptr;
       labattr.node_la = nodla;
       labattr.def_la = TRUE;
       stulab(labptr,&labattr);
       }
/*
***Eat ';'
*/

     if ( sy.sytype == ANSYSEMI )
       anascan(&sy);
     else
       anperr("AN9172",";",&sync_set,sy.sypos.srclin,sy.sypos.srccol);

     if ( !aniset(sy.sytype,&sync_set) )
        anperr("AN9202","",&sync_set,sy.sypos.srclin,sy.sypos.srccol);
            
     }
   while( aniset(sy.sytype, &beg_set) && (sy.sytype != ANSYEOF));

   return;
  }

/********************************************************/
/*!******************************************************/

        void ansmte(
        pm_ptr   *rptr,
        ANFSET   *follow,
        pm_ptr   *labptr)

/*      Analyse a statement.
 *
 *      statement ::= labeled_statement ! unlabeled_statement
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to statement node.
 *            *labptr  =>  ST label entry - if labeled.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   struct ANSYREC psy;             /* local token structure */
/*
***Labeled or unlabeled ?
*/
   if ( sy.sytype == ANSYID )
     {
     anapeek(&psy);                /* take a look at next token */
     if ( psy.sytype == ANSYCOL )
       {
/*
***Labeled
*/
       anlast(rptr,follow,labptr); /* analyses labeled statement */
       return;
       }
     }
/*
***Unlabeled
*/
   anunst(rptr,follow);            /* analyses unlabeled statement */
   *labptr = (pm_ptr)NULL;
   return;
  }

/********************************************************/
/*!******************************************************/

        void anlast(
        pm_ptr   *rptr,
        ANFSET   *follow,
        pm_ptr   *labptr) 

/*      Analyse a labeled statement.
 *
 *      labeled_statement ::= identifier ':' unlabeled_statement
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to labeled statement.
 *            *labptr  =>  ST label entry.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/
 
  {
   char labnam[ANSYNLEN];          /* local copy of label name */
   pm_ptr lrptr;                   /* PM-pointer to unlabeled statement */
   stidcl st_typ;                  /* identifier type */
   STLABEL labattr;                /* ST label attributes */
   short lablin,labcol;            /* label source position */
/*
***Save label name
*/
   strcpy(labnam,sy.syval.name);
/*
***Save label source position for error messages
*/
   lablin = sy.sypos.srclin;
   labcol = sy.sypos.srccol;
/*
***Analyse the (unlabeled) statement
*/
   anascan(&sy);                    /* consume identifier and ':' */
   anascan(&sy);
   anunst(&lrptr,follow);
/*
***If new name:  create "undefined","unreferensed" label
***if old name: check for type "STLABEL" and attr.: "undefined"
*/
   stlook(labnam,&st_typ,labptr); /* consult symbol table */
   if ( *labptr == (pm_ptr)NULL )
/*
***New name, create new ST entry
*/
      stclab(labnam,labptr);
   else if ( st_typ == ST_LABEL )
/*
***Old label, check undefined
*/
     {
     strlab(*labptr,&labattr);
     if ( labattr.def_la == TRUE )
/*
***Error - level multiply defined
*/
       {
       anperr("AN9212",labnam,NULL,lablin,labcol);
       *labptr=(pm_ptr)NULL;
       }
     }
   else
/*
***Error - identifier multiply defined
*/
     {
     anperr("AN9212",labnam,NULL,lablin,labcol);
     *labptr = (pm_ptr)NULL;
     }
/*
***Create PM labeled statement
*/
   pmclas(*labptr,lrptr,rptr);
/*
***Update label info.
*/
/*
***This is made bye ansmts() via parameter labptr
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        void anunst(
        pm_ptr   *rptr,
        ANFSET   *follow)

/*      Analyse an unlabeled statement
 *
 *      unlabeled_statement ::= if_statement   ! for_statement   ! 
 *                              goto_statement ! empty_statement ! 
 *                              ord_proc_call  ! geo_proc_call   !
 *                              assignment_statement
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to unlabeled statement.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl st_typ;                  /* identifier type */
   pm_ptr st_ent;                  /* ST entry point */
   STPROC pattr;                   /* procedure attributes */
   struct ANSYREC psy;             /* local token structure */
/*
***Select function from statement syntax and semantics
*/
   switch(sy.sytype)
     {
/*
***Empty statement
*/
     case ANSYSEMI:
       *rptr = (pm_ptr)NULL;
       break;
/*
****IF statement
*/
     case ANSYIF:
       anifst(rptr,follow);
       break;
/*
***FOR statement
*/
     case ANSYFOR:
       anfost(rptr,follow);
       break;
/*
***GOTO statement
*/
     case ANSYGOTO:
       angost(rptr,follow);
       break;
/*
***Identifier
*/
     case ANSYID:
/*
***Take a look in ST
*/
       stlook(sy.syval.name,&st_typ,&st_ent); /* consult symbol table */
/*
***Peek next token
*/
       anapeek(&psy);

/*
***Next token ":=" ?
***Next token "." ?
*/
       if ( (psy.sytype == ANSYASGN) || (psy.sytype == ANSYDOT) )
         {
/*
***Syntax -> assignment statement
*/
         if ( st_typ == ST_VAR )
           anasst(rptr,follow); /* analyse assignment statement */
         else
           {
/*
***Identifier not a variable
*/
           anperr("AN9242",sy.syval.name,follow,
                           sy.sypos.srclin,sy.sypos.srccol);
           return;
           }
         }
/*
***Next token "(" ?
*/
       else if ( psy.sytype == ANSYLPAR )
         {
/*
***Syntax -> assignment or procedure statement
*/
         if ( st_typ == ST_VAR )
/*
***Assignment statement !
*/
           anasst(rptr,follow); /* analyse assignment statement */
         else if ( st_typ == ST_PROC )
/*
***Procedure statement !
*/
           {
           strrou(st_ent,&pattr); /* get procedure attributes */
           if ( pattr.class_pr == ST_ORD )
             {
/*
***Analyse ordinary procedure call
*/
             anoprc(rptr,follow);
             }
           else                     /* pattr.class_pr == ST_GEO */
             {
/*
***Analyse geometric procedure call
*/
             angprc(rptr,follow);
             }
           }
         else
           {
/*
***Unrecognised statement, report error and skip
*/
           anperr("AN9202","",follow,sy.sypos.srclin,sy.sypos.srccol);
           }
         }
       else
         {
/*
***Unrecognised statement, report error and skip
*/
         anperr("AN9202","",follow,sy.sypos.srclin,sy.sypos.srccol);
         }
       break;
/*
***Unrecognised statement, report error and skip
*/
     default:
       anperr("AN9202","",follow,sy.sypos.srclin,sy.sypos.srccol);
       break;
     }
   return;
  }

/********************************************************/
/*!******************************************************/

        void anoprc(
        pm_ptr  *rptr,
        ANFSET  *follow)

/*      Analyse ordinary procedure call.
 *
 *      ord_proc_call ::= identifier '(' parameter list ')'
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to procedure call.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char pronam[ANSYNLEN+1];        /* local proc. name copy */
   ANATTR a_attrs[ANPARMAX];       /* parameter actual attributes */
   ANATTR dumatt;                  /* dummy attribute */
   short acount;                   /* parameter actual count */
   pm_ptr lrptr;                   /* PM-pointer to parameter list */
   short prolin,procol;            /* procedure source position */
   ANFSET locfol1;                 /* local follow set */
   stidcl st_typ;                  /* identifier type */
   pm_ptr proptr;                  /* ST procedure entry point */
   STPROC pattr;                   /* procedure attributes */
/*
***Save procedure name
*/
   strcpy(pronam,sy.syval.name);
/*
***Create local followset
*/
   ancset(&locfol1,follow,1,ANSYRPAR,0,0,0,0,0,0,0);
/*
***Save procedure source position for error messages
*/
   prolin = sy.sypos.srclin;
   procol = sy.sypos.srccol;
/*
***Get ST-info on procedure
*/
   stlook(sy.syval.name,&st_typ,&proptr);
   strrou(proptr,&pattr);
/*
***Eat "(" 
*/
   anascan(&sy);
   if ( sy.sytype == ANSYLPAR )
     anascan(&sy);
   else
     anperr("AN9172","(",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Analyse the parameter list
*/
#ifdef VARKON
   if ( pattr.kind_pr == VSET  ||  pattr.kind_pr == VSETB )
/*
***Special treatement for the set-procedure
***Set_basic, 2/6/91 JK
*/
     {
     annaml(&lrptr,&locfol1,pattr.kind_pr);
     acount = 0;
     }
   else
#endif
/*
***Normal treatement for other ordinary procedure call
*/
anparl(&lrptr,a_attrs,&acount,&locfol1);
/*
***Eat ")"
*/
   if ( sy.sytype == ANSYRPAR )
     anascan(&sy);
   else
     anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Check parameter list
*/
   ancpar(a_attrs,acount,proptr,prolin,procol,0,&dumatt);
/*
***Create the PM tree
*/
   pmcprs(proptr,lrptr,rptr);
   return;
  }

/********************************************************/
/*!******************************************************/

        void angprc(
        pm_ptr  *rptr,
        ANFSET  *follow)

/*      Analyse geometric procedure call.
 *
 *      geo_proc_call ::= identifier '(' parameter list Ä namned_params Å ')'
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to procedure call.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char pronam[ANSYNLEN+1];        /* local proc. name copy */
   ANATTR a_attrs[ANPARMAX];       /* parameter actual attributes */
   ANATTR dumatt;                  /* dummy attribute */
   ANATTR pnatt;                   /* Part-namn attribut */
   short acount = 0;               /* parameter actual count */
   char prtnam[ANSYNLEN+1];        /* module name if part procedure */
   char *pnamptr=NULL;             /* ptr to part nam if VPART */
   pmseqn  seqnum=0;               /* sequence number */
   pm_ptr mlptr = (pm_ptr)NULL;    /* PM_pointer to module params. if part */
   pm_ptr lrptr = (pm_ptr)NULL;    /* PM-pointer to actual parameter list */
   pm_ptr nlptr = (pm_ptr)NULL;    /* PM-pointer to named parameter list */
   short prolin,procol;            /* procedure source position */
   short namlin,namcol;            /* part name source position */
   ANFSET locfol1;                 /* local follow set */
   ANFSET locfol2;                 /* local follow set */
   ANFSET pnfoll;                  /* Follow-set för part-namn */
   stidcl st_typ,id_typ;           /* identifier type */
   pm_ptr proptr,idptr;            /* ST procedure entry point */
   pm_ptr funtyp;                  /* PM-pekare till funktionstyp */
   pmvaty valtyp;                  /* Typ av funktion */
   short  dummy;                   /* Används ej */
   STPROC pattr,func;              /* ST-data om rutin proc/func */
   STVAR  var;                     /* ST-data om en variabel */
   STCONST konst;                  /* ST-data om en konstant */
   STTYTBL type;                   /* Typ-info */
   pm_ptr pnexpr;                  /* Pekare till part-namnsuttryck */
   char eristr[32];                /* error insert string */
   char tstr1[32],tstr2[32];       /* insert-strängar */
   short i;                        /* loop */
   short pcount = 1;               /* number of parameters outside list */
   bool  exprfl = FALSE;           /* Part-namn har getts som expression */
/*
***Save procedure name
*/
    strcpy(pronam,sy.syval.name);
/*
***Create local followset
*/
   ancset(&locfol1,follow,1,ANSYRPAR,0,0,0,0,0,0,0);
   ancset(&locfol2,follow,3,ANSYRPAR,ANSYCOL,ANSYCOM,0,0,0,0,0);
/*
***Set ST-info on procedure
*/
   stlook(sy.syval.name,&st_typ,&proptr);
   strrou(proptr,&pattr);
/*
***Save procedure source position for error messages
*/
   prolin=sy.sypos.srclin;
   procol=sy.sypos.srccol;
/*
***Eat "("
*/
   anascan(&sy);
   if ( sy.sytype == ANSYLPAR )
     anascan(&sy);
   else
     anperr("AN9172","(",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Eat reference
*/
   if ( sy.sytype == ANSYREFL )
     {
     seqnum = sy.syval.rval.seq_val;
/*
***Global ref  or #0 not allowed here. 3/11-94 JK
*/
     if ( sy.syval.rval.seq_val < 0 )
        anperr("AN9452","",NULL,sy.sypos.srclin,sy.sypos.srccol);
     if ( sy.syval.rval.seq_val == 0 )
        anperr("AN9462","",NULL,sy.sypos.srclin,sy.sypos.srccol);
     if ( sy.syval.rval.ord_val != 0 )
       anperr("AN9392","",NULL,sy.sypos.srclin,sy.sypos.srccol);
     if ( seqnum >= 1  &&  anstyp == ANRDFIL )
       ancsnd(sy.sypos.srclin,sy.sypos.srccol,(short)seqnum);
     anascan(&sy);
     }
   else
     anperr("AN9122","",&locfol2,sy.sypos.srclin,sy.sypos.srccol);
/*
***Kolla att nästa token är ett komma. Scanna inte igen dock eftersom
***detta skall göras olika för part och geometriprocedur.
*/
   if (sy.sytype != ANSYCOM )
     anperr("AN9172",",",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Speciell hantering av PART. Titta på nästa token.
*/
   if ( pattr.kind_pr == VPART )
     {
     anascan(&sy);
/*
***Nu gäller det att avgöra vilken av de två tillåtna formerna
***av part-anrop det handlar om. Om part-namnet inte kan anses
***vara en identifierare provar vi att tolka det som ett uttryck.
*/
     if ( sy.sytype != ANSYID ) exprfl = TRUE;
/*
***Om det är en identifierare måste vi kolla vilken typ av identi-
***fierare det ät frågan om. Variabel, konstant och funktion av typen
***STRING implicerar att part-namnet skall parsas som ett uttryck.
*/
     else
       {
       stlook(sy.syval.name,&id_typ,&idptr);
       if ( idptr != (pm_ptr)NULL )
         {
         switch ( id_typ )
           {
           case ST_VAR:
           strvar(idptr,&var);
           strtyp(var.type_va,&type);
           if ( type.kind_ty == ST_STR ) exprfl = TRUE;
           break;

           case ST_CONST:
           strcon(idptr,&konst);
           strtyp(konst.type_co,&type);
           if ( type.kind_ty == ST_STR ) exprfl = TRUE;
           break;

           case ST_FUNC:
           strrou(idptr,&func);
           stsrou(func.kind_pr);
           stgret(&funtyp,&valtyp,&dummy);
           if ( valtyp == C_STR_VA ) exprfl = TRUE;
           break;
           }
         }
       }
/*
***Om exprfl är TRUE skall vi tolka partnamnet som ett uttryck.
***anarex börjar då med sist scannade token och slutar med 1:a
***icke godkända. Detta skall vara ett kommatecken.
*/
     if ( exprfl )
       {
       namlin = sy.sypos.srclin;
       namcol = sy.sypos.srccol;
       ancset(&pnfoll,follow,1,ANSYCOM,0,0,0,0,0,0,0);
       anarex(&pnexpr,&pnatt,&pnfoll);
       if ( sy.sytype != ANSYCOM )
         anperr("AN2282","",NULL,sy.sypos.srclin,sy.sypos.srccol);
       pnamptr = NULL;
/*
***Kolla uttryckets typ.
*/
       if ( !aneqty(pnatt.type,ststrp) )
         {
         angtps(pnatt.type,tstr1);
         angtps(ststrp,tstr2);
         sprintf(eristr,"%s\004%s",tstr2,tstr1);
         anperr("AN9292",eristr,NULL,namlin,namcol);
         }
       }
/*
***Annars tolkar vi det som vi alltid gjorde före version 1.12.
*/
     else
       {
       strcpy(prtnam,sy.syval.name);
       pnamptr = prtnam;
       pnexpr = (pm_ptr)NULL;
       }
/*
***Parameterlistans vänster-parentes.
*/
       anascan(&sy);
       if ( sy.sytype == ANSYLPAR )
           anascan(&sy);
       else
           anperr("AN9172","(",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Den anropade partens parameterlista.
*/
       anparl(&mlptr,a_attrs,&i,&locfol1);
/*
***Parameterlistans högerparentes.
*/ 
       if ( sy.sytype == ANSYRPAR )
         anascan(&sy);
       else
         anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Kanske finns ett kommatecken och en REF till lokalt koordinatsystem.
*/
       if ( sy.sytype == ANSYCOM)
/*
*** "," found, continue with the parameter list
*/
         anascan(&sy);
       else
/*
*** no "," found, assume : no more parameters
*/
         goto lab1;
       }
/*
***Slut part. Om det är en vanlig geometri-procedur scannar vi 
***vidare som vanligt.
*/
     else anascan(&sy);
/*
***Analyse the parameter list
*/
     anparl(&lrptr,a_attrs,&acount,&locfol2);
/*
***Analyse named parameters (if any)
*/
lab1:
    if ( sy.sytype == ANSYCOL )
      {
      anascan(&sy);
      annaml(&nlptr,&locfol1,pattr.kind_pr);
      }
/*
***Eat ")"
*/
   if ( sy.sytype == ANSYRPAR )
     anascan(&sy);
   else
     anperr("AN9172",")",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Check parameter list
*/
   ancpar(a_attrs,acount,proptr,prolin,procol,pcount,&dumatt);
/*
***Skapa PM-träd. Om det är ett part-anrop skickar vi fr.o.m.
***version 1.12 med både pekare till namn-sträng och pekare till
***namnuttryck, pnamptr och pnexpr.
*/ 
   if ( pattr.kind_pr == VPART )
     pmcpas(seqnum,pnamptr,pnexpr,mlptr,lrptr,nlptr,rptr);
/*
***Om det är en vanlig geometriprocedur gör vi som vanligt.
*/
   else pmcges(proptr,seqnum,lrptr,nlptr,rptr);
/*
***Slut.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        void anasst(
        pm_ptr  *rptr,
        ANFSET  *follow)

/*      Analyse assignment statement.
 *
 *      assignment_statement ::= variable ':=' arith_expression
 *
 *      In:   *follow  =>  Follower set.
 *
 *      Out:  *rptr    =>  Pointer to assignment statement.
 *
 *      (C)microform ab 1985-09-23 Mats Nelson
 *
 *      1999-04-26 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr lrptr;                   /* PM-pointer to expr. - varaible */
   pm_ptr rrptr;                   /* PM-pointer to expression */
   ANATTR lattr;                   /* left expression attributes */
   ANATTR rattr;                   /* right expression attributes */
   ANFSET locfol;                  /* local follow set */
   short valin,vacol;              /* variable source location */
   char eristr[32];                /* error insert strings */
   char tstr1[32],tstr2[32];
/*
***Create local follow set
*/
   ancset(&locfol,follow,1,ANSYASGN,0,0,0,0,0,0,0);
/*
***Save variable source location
*/
   valin=sy.sypos.srclin;
   vacol=sy.sypos.srccol;
/*
***Analyse variable
*/
   anvar(&lrptr,&lattr,follow);
/*
***Eat ":="
*/
   if ( sy.sytype == ANSYASGN )
     anascan(&sy);
   else
     anperr("AN9172",":=",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
**Analyse arith_expression
*/
   anarex(&rrptr,&rattr,follow);
/*
***Check compatible types
*/
   if ( aneqty(lattr.type, rattr.type ) || 
      ( (lattr.type == stflop) && (rattr.type == stintp)) )
     ;
   else
     {
     angtps(lattr.type,tstr1);
     angtps(rattr.type,tstr2);
     sprintf(eristr,"%s\004%s",tstr1,tstr2);
     anperr("AN9252",eristr,NULL,valin,vacol);
     }
/*
***Create PM statement
*/
   pmcass(lrptr,rrptr,rptr);
   return;
  }

/*********************************************************************/
