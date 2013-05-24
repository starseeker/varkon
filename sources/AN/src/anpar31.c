/**********************************************************************
*
*    anpar31.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    Parser internal functions for MBS analyser in Varkon 3
*
*    This file includes the following routines:
*
*    anperr();    Calls the error logging routine.
*    aneqty();    Compare types and return TRUE if the types are equal.
*    anacty();    Compare types and return TRUE if ltype and rtype are
*                 assignment.
*    angtps();    Return a string description from a type pointer.
*    angtcl();    Get type class from type descriptor.
*    anbcom();    Check binary combination and get result value attributes.
*    anucom();    Check unary combination and get result value attributes.
*    ancpar();    Check actual parameter list with formal parameter.
*    ancsnd();    Check if the sequencenumber are uniqe ( not defined ).
*    ancsnr();    Check if the sequencenumber are known ( defined ).
*    ancseq();    Check all the sequencenumbers in the module.
*    anclab();    Check all labels in a module.
*    ancset();    Copy exsisting followset and update the copy.
*    aneset();    Predicate returning TRUE if the set is empty.
*    aniset();    Predicate returning TRUE if the member is in the set.
*    prtset();
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
#include "../../PM/include/mbsrout.h"
#include "../include/AN.h"
#include <string.h>

extern struct ANSYREC sy;                  /* scanner interface structure */
extern pm_ptr stintp,stflop,stvecp,strefp,ststrp,stbolp;
extern short  modtyp;                      /* geometry type 2D/3D */
extern short  modatt;                      /* Modulattribut, tex. MACRO */
/*
***Ragged arrays used to compose type error messages:
*/
char *typstr[] ={         "<anytype>",     /* ST_UNDEF */
                          "<integer>",     /* ST_INT */
                          "<float>",       /* ST_FLOAT */
                          "<string>",      /* ST_STR */
                          "<vector>",      /* ST_VEC */
                          "<reference>",   /* ST_REF */
                          "<file>",        /* ST_FIL */
                          "<bool>",        /* ST_BOOL */
                          "<array>",       /* ST_ARR */
                          "<default>"};    /* ST_DEF */

char *clsstr[] ={         "<variable>",    /* ST_VARCL */
                          "<constant>",    /* ST_CONCL */
                          "<expression>",  /* ST_EXPCL */
                          "<default>"};    /* ST_DEFCL*/

static char *opstr[] =  { "*",             /* ANSYMUL */
                          "/",             /* ANSYDIV */
                          "+",             /* ANSYPLUS */
                          "-",             /* ANSYMINS */
                          "**",            /* ANSYEXP */
                          "",              /* ANSYASGN - not applicable */
                          "=",             /* ANSYEQ */
                          "<>",            /* ANSYNEQ */
                          "<",             /* ANSYLT */
                          ">",             /* ANSYGT */
                          "<=",            /* ANSYLE */
                          "<anyop>"};      /* any operator */
/*
***Number to bit number conversion table
*/
static unsigned char bct[8] = {1,2,4,8,16,32,64,128};
/*
**Defined sequence number array
*/
static DBseqnum ansvec[ ANMAXSEQ ];
/*
***Forward reference sequence number array
*/
static struct anfdat
  {
  DBseqnum seqn;                          /* sequence number */
  short lin;                              /* source position */
  short col;
  } anfvec[ ANMAXFR ];

/*
***Current number of sequence numbers in ansvecÄÅ 
*/
static DBseqnum anscnt = 0;
/*
Current number of undefined sequence numbers in anfvecÄÅ
*/
static DBseqnum anfcnt = 0;

/********************************************************/
/*!******************************************************/

        void anperr(
        char    *mess,           
        char    *insert,         
        ANFSET  *follow,        
        short    line,           
        short    col)

/*      Calls the error logging routine.
 *                                                             
 *      Effectuates the parser error recovery strategy - Panic mode.
 *      Skip input symbols until one is found which is a member in the 
 *      follower set (or until EOF)                    
 *      follow = NULL --> empty set  
 *
 *
 *      In:   *mess    =>  Error message code.
 *            *insert  =>  Insert string.
 *            *follow  =>  Follower set.
 *             line    =>  Source line number.
 *             col     =>  Source column counter.
 *
 *      Out:  
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   bool   skip = FALSE;            /*  Symbols have been discarded?   */

   analog(line, col, mess, insert);
   if (!aneset(follow))            /* Empty set? */
     {
     while (!aniset(sy.sytype, follow) && (sy.sytype != ANSYEOF))
       {
       anascan(&sy);
       skip = TRUE;
       }
     }
   if (skip)
     analog((short)sy.sypos.srclin,(short)sy.sypos.srccol, "AN9991", "");
  }

/********************************************************/
/*!******************************************************/

        bool aneqty(
        pm_ptr type1,
        pm_ptr type2)

/*      Compare types and return TRUE if the types are equal, else FALSE.
 *
 *      type1 and type2 are a PM-pointer to a type desriptor records
 *      (STTYTBL) in the symbol table (ST). 
 *
 *      In:   type1  => PM-pointer to type descriptors.
 *            type2  => PM-pointer to type descriptors.
 *
 *      Out:  
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL typas1;                 /* type access */
   STTYTBL typas2;                 /* type access */
   STARRTY arras1;                 /* array access */
   STARRTY arras2;                 /* array access */
/*
***Check for equal type pointers
*/
   if ( type1 == type2 )
     return(TRUE);
/*
***Check for NULL-type
*/
   if ( (type1 == (pm_ptr)NULL) || (type2 == (pm_ptr)NULL) )
     return(TRUE);                 /* NULL matches any type !!*/

/*
***Check for string types
*/
   if ( (angtcl(type1) == ST_STR) && (angtcl(type2) == ST_STR) )
        return(TRUE);
/*
***Check for array types
*/
   if ( (angtcl(type1) == ST_ARR) && (angtcl(type2) == ST_ARR) )
     {
/*
***Read type info
*/
     strtyp(type1,&typas1);
     strtyp(type2,&typas2);
/*
***Read array info
*/
     strarr(typas1.arr_ty,&arras1);
     strarr(typas2.arr_ty,&arras2);
/*
***Range check
*/
     if ( (arras1.up_arr - arras1.low_arr) ==
          (arras2.up_arr - arras2.low_arr))
        return(aneqty(arras1.base_arr, arras2.base_arr));
/*
***Range not okey - check if conformant
*/
     else if( ((arras1.up_arr - arras1.low_arr) < 0) ||
              ((arras2.up_arr - arras2.low_arr) < 0) )
       return(aneqty(arras1.base_arr, arras2.base_arr));
     else
       return(FALSE);
     }
/*
***Not equal types return FALSE
*/
   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool anacty(
        pm_ptr ltype,
        pm_ptr rtype)

/*      Compare types and return TRUE if ltype and rtype are assignment
 *      compatible ; lvalue(ltype) := rvalue(rtype), else FALSE.
 *
 *      type1 and type2 are a PM-pointer to a type desriptor records
 *      (STTYTBL) in the symbol table (ST).
 *
 *      In:   ltype  => PM-pointer to type descriptors.
 *            rtype  => PM-pointer to type descriptors.
 *
 *      Out:  
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
 /*
***Equal types ? 
*/
   if ( aneqty(ltype, rtype) )
      return(TRUE);
/*
***Ltype -> float and rtype -> int ?
*/
   if ( (angtcl(ltype) == ST_FLOAT) && (angtcl(rtype) == ST_INT) )
     return(TRUE);
/*
***Not assignment compatible types return FALSE
*/
   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        void angtps(
        pm_ptr type,
        char *str)

/*      Return a string description from a type pointer
 *
 *      type is a PM-pointer to a type descriptor (STTYTBL) in the symbol
 *      table (ST).
 *
 *      In:  type  =>  PM-pointer to type descriptor.
 *           str   =>  Text type descr.
 *
 *      Out: str   =>  Text type descr.
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL typas;                  /* type access */
   STARRTY arras;                  /* array access */
   char tstr[40];
   char astr[40];
   int  i;

   strcpy(astr,"(");
   while( angtcl(type) == ST_ARR )
     {
     if ( strlen(astr) != 1 )
       strcat(astr,"*");
/*
***Read type info
*/
     strtyp(type,&typas);
/*
***Read array info
*/
     strarr(typas.arr_ty,&arras);
/*
***Range
*/
     i = arras.up_arr - arras.low_arr + 1;
     if (i > 0)
       sprintf(tstr,"%d",i);
     else
       strcpy(tstr,"?");
     strcat(astr,tstr);
     type = arras.base_arr;
     }

   strcat(astr,")");
   strcpy(str,typstr[(int)angtcl(type)]);
   if ( strlen(astr) > 2 )
     strcat(str,astr);
   return;
  }

/********************************************************/
/*!******************************************************/

        sttycl angtcl(
        pm_ptr type)

/*      Get type class from type descriptor.
 *
 *      type are a PM-pointer to a type desriptor record (STTYTBL) 
 *      in the symbol table (ST). angtcl() returns the type class.
 *
 *      In:  type  =>  PM-pointer to type descriptor.
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL tcl;                    /* local type descriptor copy */

   if ( type == (pm_ptr)NULL )     /* Undefined type ? */
     return(ST_UNDEF);
   strtyp(type,&tcl);              /* No, get local type descr. copy . */
   return(tcl.kind_ty);            /* . and return type class */
  }

/********************************************************/
/*!******************************************************/

        void anbcom(
        short oplin,
        short opcol, 
        ANSYTYP op, 
        ANATTR *lattr, 
        ANATTR *rattr,  
        ANATTR *cattr) 

/*      Check binary combination and get result value attributes.
 *
 *      In:   oplin   =>  
 *            opcol   =>  operator location.
 *            op      =>  binary operator.
 *           *lattr   =>  left operand attributes.
 *           *rattr   =>  right operand attributes.
 *           *cattr   =>  combined attributes.
 *
 *      Out: *cattr   =>  combined attributes.
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1997-12-01 string <> string infört, J.Kjellander
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   sttycl ltc;                     /* left operand type class */
   sttycl rtc;                     /* right operand type class */
   pm_ptr ltype;                   /* local copy of left type */
   char eristr[32];                /* error insert strings */
   char tstr1[32],tstr2[32];
/*
***Combine classes
*/
   if ( (lattr->rvclass == ST_CONCL) && (rattr->rvclass == ST_CONCL) )
     cattr->rvclass = ST_CONCL;
   else
     cattr->rvclass = ST_EXPCL;
/*
***Combine types
*/
   ltc = angtcl(ltype = lattr->type);      /* get type class */
   rtc = angtcl(rattr->type);
   switch(ltc)
     {
     case ST_INT:
     case ST_FLOAT:
       switch(rtc)
         {
         case ST_INT:
         case ST_FLOAT:
           if ( (op >= ANSYEQ) && (op <= ANSYGE) )
             cattr->type = stbolp;
           else if ( (ltc == ST_FLOAT) || (rtc == ST_FLOAT) )
             cattr->type = stflop;
           else
             {
             if ( (op == ANSYDIV) || (op == ANSYEXP) )
               cattr->type = stflop;
             else
               cattr->type = stintp;
             }
           return;
           break;
         case ST_VEC:
           if ( op == ANSYMUL )
             {
             cattr->type = stvecp;
             return;
             }
           break;
         case ST_UNDEF:
           cattr->type = (pm_ptr)NULL;
           return;
           break;
         }
         break;

     case ST_STR:
       switch(rtc)
         {
         case ST_STR:
           if ( (op >= ANSYEQ) && (op <= ANSYGE) )
/*
***1997-12-01 JK      if((op == ANSYEQ) || (op == ANSYNEQ))
*/
             {
             cattr->type = stbolp;
             return;
             }
           else if ( op == ANSYPLUS )
             {
             cattr->type = ststrp;
             return;
             }
           break;
         case ST_UNDEF:
           if ( (op == ANSYEQ) || (op == ANSYNEQ) || (op == ANSYPLUS) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         }
       break;

     case ST_VEC:
       switch(rtc)
         {
         case ST_INT:
         case ST_FLOAT:
           if ( op == ANSYMUL )
             {
             cattr->type = stvecp;
             return;
             }
           break;
         case ST_VEC:
           if ( (op == ANSYPLUS) || (op == ANSYMINS) )
             {
             cattr->type = stvecp;
             return;
             }
           else if ( op == ANSYMUL )
             {
             cattr->type = stflop;
             return;
             }
           else if ( (op == ANSYEQ) || (op == ANSYNEQ) )
             {
             cattr->type = stbolp;
             return;
             }
           break;
         case ST_UNDEF:
           if ( (op == ANSYPLUS) || (op == ANSYMINS) || (op == ANSYMUL) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         }
       break;

     case ST_REF:
       switch(rtc)
         {
         case ST_REF:
           if ( (op == ANSYEQ) || (op == ANSYNEQ) )
             {
             cattr->type = stbolp;
             return;
             }
           break;
         case ST_UNDEF:
           if ( (op == ANSYEQ) || (op == ANSYNEQ) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         }
       break;

     case ST_UNDEF:
       switch(rtc)
         {
         case ST_INT:
         case ST_FLOAT:
         case ST_UNDEF:
           cattr->type = (pm_ptr)NULL;
           return;
         break;
         case ST_STR:
           if ( (op == ANSYPLUS) || (op == ANSYEQ) || (op == ANSYNEQ) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         case ST_VEC:
           if ( (op == ANSYPLUS) || (op == ANSYMINS) || (op == ANSYMUL) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         case ST_REF:
           if ( (op == ANSYEQ) || (op == ANSYNEQ) )
             {
             cattr->type = (pm_ptr)NULL;
             return;
             }
           break;
         }
       break;
       }

   cattr->type = (pm_ptr)NULL;            /* illegal binary combination */
   angtps(rattr->type,tstr2);
   angtps(ltype,tstr1);
   sprintf(eristr,"%s\004%s\004%s",tstr1,opstr[op-ANSYMUL],tstr2);
   anperr("AN9012",eristr,NULL,oplin,opcol);               /* log error */
   return;
  }

/********************************************************/
/*!******************************************************/

        void anucom(
        short oplin,
        short opcol, 
        ANSYTYP op, 
        ANATTR *attr,  
        ANATTR *cattr)


/*      Check unary combination and get result value attributes.
 *
 *      In:   oplin   =>  
 *            opcol   =>  operator location.
 *            op      =>  unary operator.
 *           *attr    =>  operand attributes.
 *
 *      Out: *cattr   =>  result attributes.
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   sttycl tc;                      /* operand type class */
   char eristr[32];                /* error insert strings */
   char tstr[32];
   pm_ptr type;                    /* local copy of type */
/*
***New class
*/
   if ( attr->rvclass == ST_CONCL )
     cattr->rvclass = ST_CONCL;
   else
     cattr->rvclass = ST_EXPCL;
/*
***New type
*/
   tc=angtcl(type = attr->type);      /* get type class */
   switch(tc)
     {
     case ST_INT:
     case ST_FLOAT:
     case ST_VEC:
     case ST_UNDEF:
       if ( (op == ANSYPLUS) || (op == ANSYMINS) )
         {
         cattr->type = attr->type;
         return;
         }
       break;
     }

   cattr->type = (pm_ptr)NULL;               /* illegal unary combination */
   angtps(type,tstr);
   sprintf(eristr,"%s\004%s",opstr[op-ANSYMUL],tstr);
   anperr("AN9002",eristr,NULL,oplin,opcol);                 /* log error */
   return;
  }

/********************************************************/
/*!******************************************************/

        void ancpar(
        ANATTR  actpar[],
        short   count, 
        pm_ptr  rouptr,
        short   lin,
        short   col,
        short   offset, 
        ANATTR *retattr)

/*      Check actual parameter list with formal parameter and print 
 *      error/warning messages
 *
 *      In:   actpar[]  =>  Actual attribute array.
 *            count     =>  actuel attribute count.
 *            rouptr    =>  routine ST-adress.
 *            lin       =>  
 *            col       =>  routine source position.
 *            offset    =>  parameter index offset.
 *
 *      Out: *retattr   =>  MBS-function return attr.
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short i;                        /* loop */
   char eristr[32];                /* error insert strings */
   char tstr1[32], tstr2[32];
   short fortyp;                   /* function formal type */
   STPROC roudat;                  /* routine attributes */
   STPARDAT pardat;                /* parameter access struct. */
   pmvaty dum;                     /* dummy */
/*
***Get routine attributes
*/
   strrou(rouptr,&roudat);
/*
***Select routine
*/
   stsrou(roudat.kind_pr);
/*
***Get function data, and geometry type - case function
*/
   stgret(&(retattr->type),&dum,&fortyp);
/*
***Check the function geometry type match
*/
   if ( (fortyp == 2) && (modtyp == 3) )
     anperr("AN9422","GEOMETRY",NULL,lin,col);
   else if ( (fortyp == 3) && (modtyp == 2) )
     anperr("AN9422","DRAWING",NULL,lin,col);
/*
***ADD_MBS, GETP_MBS och UPDP_MBS får inte förekomma i andra
***moduler än MACRO.
*/
   if ( modatt != MACRO )
     {
     switch ( roudat.kind_pr )
       {
#ifdef VARKON
       case VADDMBS:
       case VGPMBS:
       case VUPMBS:
       case VUPPMBS:
       anperr("AN9432","MACRO",NULL,lin,col);
       break;
#endif
       default:
       break;
      }
     }
/*
***Init function returned value class 
*/
   retattr->rvclass = (roudat.cnst_pr == TRUE) ? ST_CONCL : ST_EXPCL;
/*
***For all parameters
*/
   for ( i = 0; i < count; ++i )
      {
/*
***Get formal parameter
*/
      stgpar((short)(i+1),&pardat);
/*
***Check if legal parameter
*/
      if ( pardat.valid == ST_ILL )
        {
        anperr("AN9102","",NULL,lin,col);   /* too many parames */
        break;
        }
/*
***Check actual parameter type
*/
      if ( !anacty(pardat.type,actpar[i].type) )
        {
        angtps(pardat.type,tstr1);
        angtps(actpar[i].type,tstr2);
        sprintf(eristr,"%d\004%s\004%s",offset+i+1, tstr1,tstr2);
        anperr("AN9112",eristr,NULL,lin,col);  /* log error */
        }
/*
***Check actual parameter class
*/
      if ( actpar[i].rvclass > pardat.rvclass )
        {
        sprintf(eristr,"%d\004%s\004%s",offset+i+1,
                                        clsstr[(int)pardat.rvclass],
                                        clsstr[(int)actpar[i].rvclass]);
        anperr("AN9342",eristr,NULL,lin,col);                 /* log error */
        }
/*
***Update function returned value class
*/
      if ( (actpar[i].rvclass != ST_CONCL) || (retattr->rvclass != ST_CONCL) )
        retattr->rvclass = ST_EXPCL;
      }
/*
***Get next formal parameter
*/
   stgpar((short)(i+1),&pardat);
/*
***Check if forced
*/
   if ( pardat.valid == ST_FOR )
     {
     anperr("AN9072","",NULL,lin,col);    /* too few parames */
     }
/*
***SPECIAL TREATMENT
*/
#ifdef VARKON
   switch(roudat.kind_pr)
     {
     case VABS :                          /* ABS() : */
       if ( (count >= 1) && (actpar[0].type == stintp) )
         retattr->type = stintp; 
       break;
     }
#endif

   return;
  }

/********************************************************/
/*!******************************************************/

        void ancsnd(
        short seqlin,
        short seqcol,
        DBseqnum seqn) 

/*      Check if the sequencenumber are uniqe ( not defined ) and
 *      report error if not.
 *
 *      In:   seqlin  =>  
 *            seqcol  =>  sequense number source position.
 *            seqn    =>  sequense number.
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBseqnum i;                         /* loop */
/*
***Check for mult. def. seq. number
*/
   for ( i = 0; i < anscnt; ++i )
     {
     if ( seqn == ansvec[ i ] )
       {
       anperr( "AN9352", "", NULL, seqlin, seqcol );
       return;
       }
     }
/*
***Store the number in ansvecÄÅ 
*/
   if ( anscnt <  ANMAXSEQ )
     ansvec[ anscnt++ ] = seqn;

   return;
  }

/********************************************************/
/*!******************************************************/

        void ancsnr(
        short    seqlin,
        short    seqcol,
        DBseqnum seqn) 

/*      Check if the sequencenumber are known ( defined ).
 *      if not - store the number for later check by ancseq()
 *
 *      In:   seqlin  =>  
 *            seqcol  =>  sequense number source position.
 *            seqn    =>  sequense number.
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBseqnum i;                        /* loop */
/*
***Check for seq. number
*/
   for ( i = 0; i < anscnt; ++i )
     {
     if ( seqn == ansvec[ i ] )
       {
/*
***Found, return
*/
       return;
       }
     }
/*
***Not found, store it in anfvecÄÅ
*/
   if ( anfcnt <  ANMAXFR )
     {
     anfvec[ anfcnt ].seqn = seqn;
     anfvec[ anfcnt ].lin = seqlin;
     anfvec[ anfcnt++ ].col = seqcol;
     }
  }

/********************************************************/
/*!******************************************************/

        void ancseq()

/*      Check all the sequencenumbers in the module.
 *
 *      In:     =>  
 *
 *      Out:    =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBseqnum i,j;                   /* loop */
   bool defined;                   /* flag */
   char eristr[32];                /* error insert string */
/*
***Check forward reference seq. numbers
*/
   for ( i = 0; i < anfcnt; ++i )
     {
     defined = FALSE;
     for ( j = 0; j < anscnt; ++j )
       {
        if ( anfvec[i].seqn == ansvec[j] )
          {
          defined = TRUE;
          break;
          }
       }
     sprintf(eristr,"%d",anfvec[i].seqn);
     if ( defined )
       anperr( "AN9161", eristr, NULL, anfvec[i].lin, anfvec[i].col );
     else
       anperr( "AN9381", eristr, NULL, anfvec[i].lin, anfvec[i].col );
     }

   anscnt = 0;                      /* too be able too restart analyser */
   anfcnt = 0;
   return;
  }

/********************************************************/
/*!******************************************************/

        void anclab()

/*      Check all labels in a module.
 *
 *      In:     =>  
 *
 *      Out:    =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Local variables
*/
   pm_ptr prev = (pm_ptr)NULL;
   stidcl idcl;
   char *name;
   STLABEL labattr;

   do
     {
     stgnid( prev, &idcl, &name, &prev );
     if ( prev != (pm_ptr)NULL )
       {
       if ( idcl == ST_LABEL )
         {
         strlab(prev, &labattr);
         if ( ! labattr.def_la )
/*
**Undefined label
*/
           anperr("AN9362",name,NULL,0,0);
         if ( ! labattr.ref_la )
/*
***Unreferenced label
*/
           anperr("AN9371",name,NULL,0,0);
         }
       }
     } while( prev != (pm_ptr)NULL );
  return;
  }

/********************************************************/
/*!******************************************************/

        void ancset(
        ANFSET *dset,
        ANFSET *sset, 
        int     ntmls,
        int     tml1, 
        int     tml2,
        int     tml3, 
        int     tml4,
        int     tml5,
        int     tml6,
        int     tml7,
        int     tml8)

/*      Copy exsisting followset and update the copy.
 *
 *      sset = NULL --> empty set                                                
 *      
 *      In:  *dset   =>  pointer to destination ANFSET.
 *           *sset   =>  pointer to source ANFSET.
 *            ntmls  =>  number of terminals.
 *            tml1   =>  terminal 1 (optional).
 *            tml2   =>  terminal 2 (optional).
 *            tml3   =>  terminal 3 (optional).
 *            tml4   =>  terminal 4 (optional).
 *            tml5   =>  terminal 5 (optional).
 *            tml6   =>  terminal 6 (optional).
 *            tml7   =>  terminal 7 (optional).
 *            tml8   =>  terminal 8 (optional).
 *
 *      Out:         =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Denna rutin fungerade inte vid 1:a portningen till AIX 18/1-96
***Genom att byta typ på parameter 3-11 från short till int visade
***det sig gå bättre. Varför är inte utrett i detalj. Det funkar
***ju på alla andra maskiner.
*/
   int    i;
/*
***Borttaget, se nedan.
   int   *tmlptr;
   int    tmldif;
   int index1,index2;

   tmlptr= &tml1;
   if ( ntmls > 1 ) tmldif = &tml2 - &tml1;
*/

   if ( sset != NULL )
     V3MOME(sset,dset,sizeof(ANFSET));    /* copy source, or .. */
   else
    {                                     /* .. clear desination */
    for ( i=0;i<8;++i )
      dset->byte[i]=0;
    }
/*
***Följande loop misstänks orsaka problem på vissa datorer.
*
   for(i=1;i<=ntmls;++i) 
     {
     index1 = (*tmlptr)/8;
     index2 = (*tmlptr)%8;
     dset->byte[index1] |= bct[index2]; 
     tmlptr += tmldif;
     }
*
***Vi provar det här istället som inte bygger på antaganden
***om adresser till data på stacken.
*/
   if ( ntmls > 0 ) dset->byte[tml1/8] |= bct[tml1%8];
   if ( ntmls > 1 ) dset->byte[tml2/8] |= bct[tml2%8];
   if ( ntmls > 2 ) dset->byte[tml3/8] |= bct[tml3%8];
   if ( ntmls > 3 ) dset->byte[tml4/8] |= bct[tml4%8];
   if ( ntmls > 4 ) dset->byte[tml5/8] |= bct[tml5%8];
   if ( ntmls > 5 ) dset->byte[tml6/8] |= bct[tml6%8];
   if ( ntmls > 6 ) dset->byte[tml7/8] |= bct[tml7%8];
   if ( ntmls > 7 ) dset->byte[tml8/8] |= bct[tml8%8];

   return;
  }

/********************************************************/
/*!******************************************************/

        bool  aneset(
        ANFSET   *set)

/*      Predicate returning TRUE if the set is empty.
 *
 *      set = NULL --> empty set                                                
 *      
 *      In:  *set  =>  Pointer to set.
 *                     RET - TRUE if set is empty FALSE otherwise.
 *           
 *      Out:         =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short  i;                       /* For loop index  */

   if ( set == NULL )
     return(TRUE);
   for ( i = 0; i<8; ++i )
     if ( set->byte[i] != 0 )
       return (FALSE);
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool  aniset(
        short    memb,
        ANFSET  *set)

/*      Predicate returning TRUE if the member is in the set.
 *
 *      set = NULL --> empty set                                                
 *      
 *      In:   memb  =>  Member to test for inclusion.
 *           *set   =>  Pointer to set.
 *                      RET - TRUE if set is empty FALSE otherwise.
 *           
 *      Out:        =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( set->byte[memb / 8] & bct[sy.sytype % 8] )
     return(TRUE);

   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        void prtset(
        ANFSET  *set)

/*      
 *
 *      
 *      In:  *set  =>  Pointer to set.
 *           
 *      Out:         =>
 *
 *      (C)microform ab 1985-09-03 M. Nelson
 * 
 *      1999-04-21 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short i,j;
   for ( i=0;i<8;++i )
     {
     for ( j=0;j<8;++j )
       {
       if ( set->byte[i] & bct[j] )
         printf("1");
       else
         printf("0");
       }
     }
   printf("\n");
   return;
  }

/*********************************************************************/
