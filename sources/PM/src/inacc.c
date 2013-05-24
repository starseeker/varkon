/**********************************************************************
*
*    inacc.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    short inmod()     interpret module
*    short inssta()    interpret signle statement 
*    short ininit()    initialize the interpreter
*
*    short inevmo()    evaluate  module
*    short inevpa()    evaluate  module parameter
*    short inevpl()    evaluate  module parameter list
*    short inevst()    evaluate  statement 
*    short inevsl()    evaluate  statement list 
*    short inevel()    evaluate  expression list
*    short inevnp()    evaluate  named parameter 
*    short inevco()    evaluate  condition 
*    short inevcl()    evaluate  condition list
*    short ineqty()    equal types ?
*    short incoty()    convert type
*
*    short inmiss();   Error report for removed MBS procedures
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
#include <math.h>

extern pm_ptr ststrp;      /* PM-pointer to literal string type in sym. tab. */
extern DBint  stgldz;      /* global data area size */

extern pm_ptr stintp;
extern pm_ptr stflop;
extern pm_ptr stvecp;
extern pm_ptr strefp;
extern pm_ptr ststrp;
extern pm_ptr stfilp;
extern pm_ptr stbolp;

#ifndef ANALYZER            /* don't compile if analyser */

/*
***label for an actual GOTO-statement
*/
static STLABEL actlabel;

/********************************************************/
/*!******************************************************/

        short inmod(
        pm_ptr   mola)

/*      Interpret activ module.
 *
 *      In:   
 *
 *      Out:  
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(inevmo(mola));
  }  

/********************************************************/
/*!******************************************************/

        short inssta(
        pm_ptr   statla)

/*      Interpret signle statement.
 *
 *      In:   
 *
 *      Out:  
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(inevst(statla));
  }

/********************************************************/

#endif

/*!******************************************************/

        short ininit( )

/*      Initiate the interpreter.
 *
 *      In:   
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {

#ifndef ANALYZER      /* Don't compile if analyser */

   stidcl kind;
   pm_ptr symbla;
   STVAR var;
   PMLITVA litva;
   short status;
/*
***Initiera RTS. Allokera minne för RTS.
***Detta är enda stället i V3 där inirts() anropas.
*/
   if ( (status=inirts()) < -1 ) return(status);
/*
***Create and initiate global data in RTS.
***Put global data frame.
*/
   inpdfr(stgldz);
/*
***Initialize INPUT.
*/
   stlook("INPUT",&kind,&symbla);
   strvar(symbla,&var);
   ingval(var.addr_va,stfilp,FALSE,&litva);
   litva.lit.fil_va->fp        = stdin;
   litva.lit.fil_va->mode[ 0 ] = 'r';
   litva.lit.fil_va->mode[ 1 ] = '\0';
   litva.lit.fil_va->open      = TRUE;
   litva.lit.fil_va->iostat    = 0;
/*
***Initialize OUTPUT.
*/
   stlook("OUTPUT",&kind,&symbla);
   strvar(symbla,&var);
   ingval(var.addr_va,stfilp,FALSE,&litva);
   litva.lit.fil_va->fp        = stdout;
   litva.lit.fil_va->mode[ 0 ] = 'w';
   litva.lit.fil_va->mode[ 1 ] = '\0';
   litva.lit.fil_va->open      = TRUE;
   litva.lit.fil_va->iostat    = 0;
/*
***Initialize PI.
*/
   stlook("PI",&kind,&symbla);
   strvar(symbla,&var);
   litva.lit.float_va = PI;
   inpval(var.addr_va,stflop,FALSE,&litva);
/*
***Set RTS base for local data area.
*/
   inslrb();
/*
***End of initiate global data area.
***Reset default named parameter values.
*/
   inrdnp();

#endif

   return( 0 );

  }

/********************************************************/

#ifndef ANALYZER           /* don't compile if analyser */

/*!******************************************************/

        short inevst(
        pm_ptr  statla)

/*      Evaluate statement.
 *
 *      In:   statla  =>  PM-pointer to statement 
 *
 *      Out:  
 *
 *      FV:   return - status  == 1 : GOTO statement
 *                             == 3 : EXIT-call med felmeddelande
 *                             == 4 : EXIT-call utan felmeddelande
 *                             == AVBRYT : <CTRL>c 
 *                             < 0 : error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMSTNO  *np;         /* c-pointer to statement node */
   PMASST  *assp;       /* c-pointer to ass_stat node */
   PMIFST  *ifp;        /* c_pointer to if_stat node */
   PMFOST  *forp;       /* c-pointer to for_stat node */
   PMGEST  *geop;       /* c-pointr to geo_stat node */
   PMPAST  *partp;      /* c-pointer to part_stat node */
   PMPRST  *procp;      /* c-pointer to proc_stat node */
   V2REFVA  ident;      /* geometric id */
   V2NAPA   npblock;    /* name parameter block */

/*
***Variabler för felhantering.
*/
   char  snrstr[10];
   short status;

/*
***variables for assignment
*/
   PMLITVA   lval;  
   PMLITVA   rval;
   pm_ptr    ltyla;
   pm_ptr    rtyla;

/*
***variables for FOR-statement
*/
   PMLITVA  val;
   pm_ptr   tyla;
   STVAR    var;
   int      to;
   int      step;
   int      i;

/*
***Tom sats.
*/
   if ( statla == (pm_ptr)NULL ) return( 0 );
/*
***PM-pekare till C-adress.
*/
   np = (PMSTNO *)pmgadr(statla);
/*
***Är det en satsnod ?
*/
   if ( np->noclst != STAT ) return(erpush("IN2512",""));
/*
***Vilken sats ?
*/
   switch ( np->suclst )
     {
/*
***Tilldelning.
*/
     case ASS_ST:
     assp = &( np->stsubc.ass_st );
/*
***Evaluera l- och r-värden.
*/      
     if ( (status=inevex(assp->asvar,&lval,&ltyla)) != 0 ) return(status);
     if ( (status=inevev(assp->asexpr,&rval,&rtyla)) != 0 ) return(status);
/*
***Typkontroll och ev. konvertering.
*/
     if ( !ineqty( ltyla, rtyla ) )
       if ( ( status = incoty( &rval, &rtyla, ltyla ) ) < -1 )
         return( erpush( "IN2082", "") ); 
/*
***l-värdets adress.
*/
     if ( lval.lit_type != C_ADR_VA ) return(erpush("IN2072",""));
/*
***Skriv in r-värde i RTS.
*/
     if ( (status=inpval(lval.lit.adr_va,ltyla,FALSE,&rval)) < -1 )
       return( status );
     break;
/*
***IF-sats.
*/
  case IF_ST:

      ifp = &( np->stsubc.if_st );

      /* evaluate condition list and check status */
      status = inevcl( ifp->ifcond );
      if ( status != 0 )
          {
          if ( status == 2 )
              status = 0;        /* condition true but no GOTO */
          return( status );      /* return == 1 indicates GOTO */
          }
      else
          {  /* evaluate MBS-else statement list */
          return(inevsl(ifp->ifstat));
          }

      break;

  case FOR_ST:

      /* TO and STEP expression are avaluated only ones before the */
      /* FOR-loop it self are evaluated */

      forp = &( np->stsubc.for_st );

      /* evaluate start expression */
      if ( ( status = inevev( forp->fostar, &rval, &rtyla ) ) != 0 )
          return( status );

      /* read loop variable info. */
      strvar( forp->fovar, &var );
      ltyla = var.type_va;

      /* evaluate STEP-expression */
      if ( forp->fostep != (pm_ptr)NULL )
          {
          if ( ( status = inevev( forp->fostep, &val, &tyla ) ) != 0 )
              return( status );
          step = val.lit.int_va;
          }
      else /* use default STEP-value */
          step = 1;

      /* evaluate TO-expression */
      if ( ( status = inevev( forp->foto, &val, &tyla ) ) != 0 )
          return( status );
      to = val.lit.int_va;
/*
***Evaluera FOR-loopen olika för positivt och negativt steg.
*/
    if ( step > 0 )
      {
      for ( i=rval.lit.int_va; i<=to; i += step )
        {
        rval.lit.int_va = i;
        inpval( var.addr_va, ltyla, FALSE, &rval );
        np = (PMSTNO *)pmgadr(statla);
        if ( (status=inevsl(np->stsubc.for_st.fordo)) != 0 ) return(status);
        }
      }
    else
      {
      for ( i=rval.lit.int_va; i>=to; i += step )
        {
        rval.lit.int_va = i;
        inpval( var.addr_va, ltyla, FALSE, &rval );
        np = (PMSTNO *)pmgadr(statla);
        if ( (status=inevsl(np->stsubc.for_st.fordo)) != 0 ) return(status);
        }
      }
      break;

  case GO_ST:

      /* read label info from symbol table and put in global data area */
      /* for actual goto label */
      strlab( np->stsubc.gotost.golabe, &actlabel );

      return( 1 );          /* return goto status */

      break;

  case PRO_ST:

      procp = &( np->stsubc.procst );
      if ( (status = inevpr(procp->prname,procp->pracva)) != 0 )
        return(status);
      break;

  case PART_ST:

      partp = &(np->stsubc.partst);

      ident.seq_val = partp->geident;
      ident.ord_val = 1;
      ident.p_nextre = NULL;

      if ( ( status = evpart( &ident, partp ) ) != 0 )
         return( status );
      break;

  case GEO_ST:

      geop = &(np->stsubc.geo_st);

      ident.seq_val = geop->geident;
      ident.ord_val = 1;
      ident.p_nextre = NULL;

/*
***evaluate named parameter
*/
      if ( ( status = inevnl( geop->geacna, &npblock ) ) < -1 )
          return( status );

/*
***evaluate geometric procedure
*/
      if ((status = inevgp( &ident,geop->gename,geop->geacva,&npblock )) != 0 )
        {
        sprintf(snrstr,"#%d",ident.seq_val);
        erpush("IN2252",snrstr);
        return( status );
        }
      break;

  case LAB_ST:

/*
***evaluate labeled statement
*/
      if ( (status=inevst(np->stsubc.labest.lastat)) != 0 )
          return(status);

      break;

  default:
      return( erpush( "IN2512", "" ) );
      break;

  }
   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inevsl(
        pm_ptr  stlist)

/*      Evaluate statement list.
 *
 *      In:   statla  =>  PM-pointer to statement list (tconc-node) 
 *
 *      Out:  
 *
 *      FV:   return - status  == 1 : GOTO statement
 *                              < 0 : error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;       /* PM-pointer to list node */ 
   pm_ptr nextla;       /* PM-pointer to next list node */
   pm_ptr statla;       /* PM-pointer to statment node */
   short status;

   if ( stlist == (pm_ptr)NULL ) return(0);
/*
***Första satsen i satslistan.
*/
   if ( (status=pmgfli(stlist,&listla)) != 0 ) return(status);
/*
***Gå igenom satslistan.
*/
   while ( listla != (pm_ptr)NULL )
     { 
/*
***Läs listnod.
*/
     if ( (status=pmglin(listla,&nextla,&statla)) != 0 ) return(status);
/*
***Evaluera satsen. Om status = 0, fortsätt med nästa sats.
*/
     if ( (status=inevst(statla)) == 0 ) listla = nextla;
/*
***Om status = 1 => GOTO. Kolla om hoppmålet
***finns i den aktuella satslistan och hoppa i
***så fall dit.
*/
     else if ( status == 1 )
       {
       if ( actlabel.list_la == stlist ) listla = actlabel.node_la;
       else return(1);
       }
/*
***Status = 3 eller 4 => EXIT.
*/
     else if ( status > 2 ) return(status);
/*
***Status < 0  => Fel.
*/
     else if ( status < 0 ) return(status);
     }
/*
***Slut.
*/
   return(0);

  }

/********************************************************/
/*!******************************************************/

        short inevel(
        pm_ptr  exlist)

/*      Evaluate expression list.
 *      O.B.S.!!!! endast exempel på list hantering  !!!!
 *
 *      In:   statla  =>  PM-pointer to expression list 
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr exprla;     /* PM-pointer to statment node */
   short status;

   if ( exlist == (pm_ptr)NULL )
     {
     return( 0 );
     }

/*
***get first list node in list
*/
   if ( ( status = pmgfli( exlist, &listla ) ) != 0 )
     {  
     return( status );                   /* Error */
     }

   while ( listla != (pm_ptr)NULL )
     { 
/*
***get contents of list node
*/
     if ( ( status = pmglin( listla, &nextla, &exprla ) ) != 0 )
        return( status );

/*
***print expression
*/
     /*if ( ( status = inevex( exprla ) ) != 0 )
        return( status );
     */

     listla = nextla;

     }
   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inevpa(
        pm_ptr   pala, 
        PMLITVA *valp, 
        STTYTBL *in_tp,  
        DBint    cfsize) 


/*      Evaluate module parameter.
 *
 *      In:   statla  =>  PM-pointer to module parameter node 
 *            *valp   =>  Pointer to parameter value
 *                                NULL pointer indicates use of default value
 *            *in_tp  =>  Parameter value type info
 *             cfsize =>  Callers RTS frame size, used to calculate new offset address
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMPANO *np;          /* c-pointer to module parameter node */
   char *chp;           /* temporary char pointers */
   char *chpc;
   STVAR var;           /* interface struct for a parameter */
   DBint rtsofs;        /* offset in RTS */
   DBint objsize;       /* size of object to be put in RTS */
   PMLITVA val;         /* value structure */
   pm_ptr  tyla;        /* value type pointer */
   PMLITVA *actvp;      /* pointer to actual value */
   STTYTBL *acttp;      /* pointer to it's type info */
   STTYTBL acttype;     /* actual type info struct */

   pm_ptr formty;       /* formal parameter type pointer */
   STTYTBL ftype;       /* formal parameter type info. struct */
   bool putflg = TRUE;  /* boolean to indicate if it excist a value to be put on RTS */
   bool rpavaflg;       /* Flag to indicate "call by reference" parameter */
   short status;

   np = (PMPANO *)pmgadr(pala);

   if ( np->noclpa != PARAM )
       {  /* Error */
       return( erpush( "IN2532", "" ) );       /* Not an module parameter node */
       }

/*
***read variable/parameter info.
*/
   if ( ( status = strvar( np->fopa_, &var ) ) != 0 )
       return( status );

/*
***actual parameter value
*/
   if ( valp == NULL )
       {             
/*
***use default value
***evaluate default value
***check if default value excist
*/
       if ( var.def_va != (pm_ptr)NULL )
           {
        
           if ( var.kind_va == ST_RPAVA )                /* check if call by reference */
               {  
               return( erpush( "IN3233", "" ) );         /* error */
               }   
/*
***call by reference parameter can not have default value
*/
           if ( ( status = inevex( var.def_va, &val, &tyla ) ) != 0 )
               return( status );

/*
***read type info
*/
           strtyp( tyla, &acttype );

           }
       else
/*
***zero "val" and "type"
*/
           {
           chp = (char *)&val;
           for ( chpc = chp; chpc < chp + sizeof( val ); chpc ++ )
               *chpc = '\0';
           acttype.kind_ty = ST_UNDEF;                   /* undef type */
           putflg = FALSE;    
           }

       actvp = &val;
       acttp = &acttype;
      }
   else
       {
/*
***use actual value
*/
       actvp = valp;
       acttp = in_tp;
       }

/*
***Read type for formall parameter and check actual parameter type.
***Do type conversion if necessary.
*/
   formty = var.type_va;

/*
***read formal type info.
*/
   strtyp( formty, &ftype );

/*
***Rune har upptäckt att om A anropar B med en indexerad variabel
***och b har den VAR-deklarerad men med annat antal dimensioner får
***man mycket märkliga fel. Detta skulle kunna kontrolleras här !!!
***14/2/94 JK.
*/
/*
***check parameter type VPAVA or RPAVA
*/
   if ( var.kind_va == ST_RPAVA )
       {
/*
***check if types are exactly equal
*/
       if ( acttp->kind_ty != ftype.kind_ty )
           {                /* Not equal types for "call by reference" parameter */
           return( erpush( "IN3293", "" ) );
           }
/*
***check if act-value is address
*/
       if ( actvp->lit_type != C_ADR_VA )
           {                                   
           return( erpush( "IN2062", "" ) );      /* error */
           }                /* ilegal address for call by reference parameter, tex om
                               konstantuttryck används i anropet, JK */

/*
***calculate address offset from new frame
*/
       actvp->lit.adr_va = actvp->lit.adr_va - cfsize;
       objsize = sizeof( actvp->lit.adr_va );
       rpavaflg = TRUE;
       }
   else
       {

/*
***if not equal types do type conversion
*/
       if ( acttp->kind_ty != ftype.kind_ty )
           {
           if ( ( ftype.kind_ty == ST_FLOAT ) && ( acttp->kind_ty == ST_INT ) )
/*
***convert value and type
*/
               {
               actvp->lit.float_va = actvp->lit.int_va;
               actvp->lit_type = C_FLO_VA;
               }
           else
               return( erpush( "IN3203", "") );   /* Incompatible types for Part param. */
           }

       objsize = var.size_va;
       rpavaflg = FALSE;
       putflg = TRUE;                     /* if no value the zeroed PMLITVA will be put */
       }

/*
***allocate a block in RTS and put the value
*/
   if ( ( status = inallo( objsize, &chp, &rtsofs ) ) != 0 )
       return( status );
   if ( putflg )
       if ( ( status = inpval( rtsofs, formty, rpavaflg, actvp ) ) != 0 )
           return( status );

/*
***update symbol table
*/
   var.addr_va = rtsofs;
   if ( ( status = stuvar( np->fopa_, &var ) ) != 0 )
       return( status );

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inevpl(
        pm_ptr   palist)
 
/*      Evaluate module parameter list.
 *
 *      In:   palist  =>  PM-pointer to module parameter list 
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr parala;     /* PM-pointer to module parameter node */
   short status;

/*
***check if no parameters
*/
   if ( palist == (pm_ptr)NULL )
       {
       return( 0 );
       }

/*
***get first list node in list
*/
   if ( ( status = pmgfli( palist, &listla ) ) != 0 )
       {
       return( status );                  /* Error */
       }

   while ( listla != (pm_ptr)NULL )
       { 
/*
***get contents of list node
*/
       if ( ( status = pmglin( listla, &nextla, &parala ) ) != 0 )
           return( status );

/*
***evaluate parameter
*/
       if ( ( status = inevpa( parala, NULL, NULL, 0 ) ) != 0 )
           return( status );

       listla = nextla;

       }
   return( 0 );

  } 

/********************************************************/
/*!******************************************************/

        short inevmo(
        pm_ptr   mola)

/*      Evaluate module.
 *
 *      In:   mola  =>  PM-pointer to base for module header 
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;             /* c-pointer to module header */
   PMMODULE module;
   short status;

/*
***clear the parameter pool for general parameter handling
*/
/*
***Flyttat till igramo(), 8/10/92 JK
***nclpp();
*/

/*
***Reset Run Time Stack administration
*/
/*
***Flyttat till igramo(), 10/6/92 JK
*/

/*
***Put data frame on RTS
*/
   pmrmod( &module );
   if ( ( status = inpdfr( module.ldsize ) ) != 0 )
       {  
       return( status );                   /* Error */
       }

   np = (PMMONO *)pmgadr((pm_ptr)0);

   if ( np->monocl != MODULE )
       { 
       return( erpush( "IN2542", "" ) );   /* Error Not a statement node */
       }

   if ( ( status = inevpl( np->ppali_ ) ) != 0 ) 
/*
***evaluate parameter list
*/
       {
       return( status );
       }

if ( ( status = inevsl( np->pstl_ ) ) != 0 ) 
/*
***evaluate statement list
*/
    {
    return( status );
    }

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inevcl(
        pm_ptr  colist)

/*      Evaluate condition list.
 *
 *      In:   mola  =>  PM-pointer to condition list 
 *
 *      Out:  
 *
 *      FV:   return  = 2 when one condition evaluates true and
 *                               the corresponding statment list are evaluated
 *                    = 1 as 2 and an GOTO statement has been evaluated
 *                    = 0 no condition has evaluates to true
 *                    < 0 error severity code
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr condla;     /* PM-pointer to condition node */
   short status;

   if ( colist == (pm_ptr)NULL )
       {
       return( 0 );
       }

/*
***get first list node in list
*/
   if ( ( status = pmgfli( colist, &listla ) ) != 0 )
       { 
       return( status );            /* Error */
       }

   while ( listla != (pm_ptr)NULL )
       { 
/*
***get contents of list node
*/
       if ( ( status = pmglin( listla, &nextla, &condla ) ) != 0 )
           return( status );

/*
***evaluate condition
*/
       status = inevco( condla );
       if ( status >= 1 )
           {
           return( status );
           }
       else if ( status < 0 )
           return( status );

       listla = nextla;
       }

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inevco(
        pm_ptr  condla)

/*      Evaluate condition.
 *
 *      In:   mola  =>  PM-pointer to condition 
 *
 *      Out:  
 *
 *      FV:   return  = 2 when one condition evaluates true and
 *                               the corresponding statment list are evaluated
 *                    = 1 as 2 and an GOTO statement has been evaluated
 *                    = 0 no condition has evaluates to true
 *                    = 3,4 EXIT-call
 *                    < 0 error severity code
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMCONO *np;
   PMLITVA val;
   pm_ptr tyla;
   short status; 

/*
***get a c-pointer to the condition node
*/
   np = (PMCONO *)pmgadr(condla);

/*
***Evaluate condition
*/
   if ( ( status = inevex( np->p_cond, &val, &tyla ) ) < -1 )
       return( status );
   if ( ! ineqty( tyla, stbolp ) )
       return( erpush( "IN3273","" ) );

   if ( val.lit.bool_va )
       {  
/*
***evaluate statement list
*/
       if ( (status=inevsl(np->p_stl_co)) < -1 )
           return( status );
       else if ( status == 1 )
           return(1);               /* indicates GOTO statement */
       else if ( status > 2 ) 
           return(status);          /* indicates EXIT-call. */
       else
           return( 2 );
       }
   else
       return( 0 );

  }
 
/********************************************************/
/*!******************************************************/

        short inev(
        pm_ptr  a)

/*      Evaluate.
 *
 *      In:   mola  =>  PM-pointer to 
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return( 0 );
  } 

/********************************************************/

#endif     /* KE851023 */

/*!******************************************************/

        short ineqty(
        pm_ptr  tyla1,
        pm_ptr  tyla2)

/*      Equal types ?.
 *
 *      In:   tyla1  =>  Type pointer 
 *            tyla2  =>  Type pointer 
 *
 *      Out:  
 *
 *      FV:   return 1 if equal and 0 if not equal
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STARRTY  arrty1;     /* array type info. */
   STARRTY  arrty2;     /* array type info. */
   STTYTBL  type1;      /* array component type info */
   STTYTBL  type2;      /* array component type info */

/*
***check if equql type pointers
*/
   if ( tyla1 == tyla2 )
       return( TRUE );

/*
***check for NULL types
*/
   if ( ( tyla1 == (pm_ptr)NULL ) || ( tyla2 == (pm_ptr)NULL ) )
        return( TRUE );

/*
***read type info
*/
   strtyp( tyla1, &type1 );
   strtyp( tyla2, &type2 );

/*
***check for string types
*/
   if ( ( type1.kind_ty == ST_STR ) && ( type2.kind_ty == ST_STR ) )
       return( TRUE );

/*
***check for array types
*/
   if ( ( type1.kind_ty == ST_ARR ) && ( type2.kind_ty  == ST_ARR ) )
       {
/*
***read arry type info.
*/
       strarr( type1.arr_ty, &arrty1 );
       strarr( type2.arr_ty, &arrty2 );
/*
***check range
*/
       if ( ( arrty1.up_arr - arrty1.low_arr ) == 
            ( arrty2.up_arr - arrty2.low_arr ) )
           {
           return( ineqty( arrty1.base_arr, arrty2.base_arr ) );
           }
/*
***range not ok, check if conformant
*/
       else if ( (( arrty1.up_arr - arrty1.low_arr ) < 0 ) || 
                 (( arrty2.up_arr - arrty2.low_arr ) < 0 ) )
                {
                return( ineqty( arrty1.base_arr, arrty2.base_arr ) );
                }

       else
           {  
           return( FALSE );         /* types not equal */
           }
       }
   else
       {  
       return( FALSE );             /* types not equal */
       }

  }

/********************************************************/
/*!******************************************************/

        short incoty(
        PMLITVA *avalp,
        pm_ptr  *atylap,
        pm_ptr   ctyla)

/*      Convert type for value.
 *
 *      In:   *avalp   =>  Actual value to be converted 
 *            *atylap  =>  Actual type pointer 
 *             ctyla   =>  Conversion type pointer 
 *
 *      Out:  *avalp   =>  Actual value to be converted
 *
 *      FV:   return - error code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( avalp->lit_type != C_ADR_VA )
       {  
/*
***if not address do conversion
*/
       if ( ineqty( ctyla, stflop ) )
           {

           if ( ineqty( *atylap, stintp ) )
                 {
                 avalp->lit.float_va = avalp->lit.int_va;
                 avalp->lit_type = C_FLO_VA;
                 *atylap = stflop;
                 }
             else 
                 return( -3 );    /* Error , incompatiable types */
            }
       else
           return( -3 );
       }

   return ( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inmiss()

/*      This function is used to replace old functions that
 *      are removed from functab[] in evfuncs.h. Old modules
 *      compiled with an old MBS compiler may include calls
 *      to such procedures. The user will get an error message
 *      with instructions to recompile the MBS code.
 *
 *      (C)2007-03-31 J.Kjellander
 *
 ******************************************************!*/

  {
   return(erpush("IN5802",""));
  }

/********************************************************/
