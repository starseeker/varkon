/**********************************************************************
*
*    pmac1.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    pminit()       Initierar PM.
*    pmcmod()       Skapar ett modulhuvud och den lokala symbol-
*                     tabellen. 
*    pmload()       Laddar in en module från fil och initiera den som aktiv.
*    pmsave()       save active module an a file
*    pmlmst()       Länkar in en sats i modulens satslista.
*    pmlmpa()       Länkar in en parameter i modulens parameterlista.
*    pmumod()       Uppdaterar information i modul huvudet.
*    pmrmod()       Läser information från modul huvudet.
*    pmcpar()       Skapar en parameternod.
*    pmcass()         - " -   tilldelningssats
*    pmcifs()         - " -   if-stats-nod
*    pmcfos()         - " -   for-sats-nod
*    pmcgos()         - " -   goto-sats-nod
*    pmclas()         - " -   labeled-sats-nod
*    pmcprs()         - " -   procedur-sats-nod.
*    pmcpas()         - " -   part-sats-nod.
*    pmcges()         - " -   geometri-sats-nod.
*    pmcune()         - " -   unäroperator-uttrycks-nod.
*    pmcbie()         - " -   binäroperator-uttrycks-nod.
*    pmclie()         - " -   literal-uttrycks-nod.
*    pmcide()         - " -   identifierar-uttrycks-nod.
*    pmcine()         - " -   index-uttrycks-nod.
*    pmccoe()         - " -   komponent-uttrycks-nod.
*    pmcfue()         - " -   funktions-uttrycks-nod.
*    pmcnpa()       Skapar en namnparameter-nod.
*    pmccon()       Skapar en villkors-nod.
*    pmtcon()       Skapar en list-nod och länkar in den i en 
*                     lista. Om det är första elementet i listan 
*                     skapas också en TCONC_NODE .
*    pmcstr()       Skapar och lägger in en textsträng i PM.
*    pmcref()            - " -           ett referensvärde i PM.
*    pmstpr()       Rutin för att rätta bug vid partanrop.9/12/86 RS
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"

/********************************************************/
/*!******************************************************/

        short pminit()

/*      Initialize PM.
 *      Initierar PM och dess olika delar så som administration 
 *      och symboltabell
 *
 *      FV:  return - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr bla;
   short  status;

/*
***Allocate memory for PM and init pma and pmb.
*/
   if ( (status=pmibuf()) < 0 ) return(status);
/*
***Set PM baseadress to 0.
*/
   pmsbla((pm_ptr)0);
/*
***Init symbol table.
*/
   if ( (status=stgini()) != 0 ) return(status);
/*
***Determine the point to start module area in PM by allocate a block and
***set base for PM and active module.
*/
   if ( (status=pmsbla((pm_ptr)0) ) != 0 ) return(status);

   if ( (status=pmallo((DBint)2,&bla)) != 0 ) return(status);

   if ( (status=pmsbla(bla)) != 0 ) return(status);

   if ( (status=pmsaba(bla)) != 0 ) return(status);
/*
***Init the interpretor.
*/
   if ( (status=ininit()) != 0 ) return(status);

   return( 0 );
  } 

/********************************************************/
/*!******************************************************/

        short pmcmod(
        char     *name,
        PMMODULE *module,
        pm_ptr   *retla)

/*      Create module header.
 *      Skapar ett modulhuvud och den locala symboltabellen. Modulhuvudet initiali-
 *      seras och en pekare till huvudet returneras. Kollar också om modulen redan 
 *      existerar. Data i modulhuvudet kan sättas genom interface strukturen 
 *      "module". Om det inte finns några pekare till "parlist" och "stlist" skall 
 *      dessa sättas till NULL. Detta gäller endast för pmcmod().
 *
 *      In:   *name    =>  Module name, pointer to character string
 *            *module  =>  interface structure for module info 
 *
 *      Out:  *retla   =>  logical address to the module header
 *                         base address for the active module 
 *
 *      FV:    return - status 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMMONO *np;         /* c-pointer to module node */
   pm_ptr symbla;      /* symbol table pointer */
   pm_ptr pmla;        /* temp PM-pointer */

   pmclea();                       /* reset PM's stack  */

   status = pmsbla((pm_ptr)0 );    /* set PM-base address to 0 */

/*
***Allocate a block for the module node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );           /* Error  No room in PM */ 
      }

   status = pmsbla( *retla );     /* set PM-base to start from module header */
   status = pmsaba( *retla );     /* set base for active module */

/*
***Create a string in PM for the module name string
***Allocate a block for the string in PM
*/
   if ( ( status = pmallo( (DBint)(V2SYNLEN + 1), &pmla ) ) != 0 )
      {  
      return( status );           /* Error */
      }

/*
***Fill in data in the module node
*/
   np = (PMMONO *)pmgadr((pm_ptr) 0 );   /* Fix a c-pointer to module node in PM */
   np->monocl = MODULE;                  /* Node class */
   np->mona_ = pmla;                     /* insert the PM-pointer to the name string */

/*
***copy the module name string into MODULE interface structure
*/
   strcpy( module->mname, name );
 
   pmumod( module );

/*
***Initialize local symbol table and fill in pointers to symbol tables
*/
   if ( ( status = stlini( (pm_ptr)NULL, name, &symbla ) ) != 0 )
      {  
      return( status );                   /* Error */
      }
   np->plosb_ = symbla;

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short pmload(
        char  *modname,
        pm_ptr *retla)


/*      Load module.
 *      Laddar in en module från fil som skall bli aktiv.
 *
 *      In:   *modname  =>  Module name
 *
 *      Out:  *retla    =>  logical address to the module header
 *
 *      FV:    return   - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;
   pm_ptr symbla;     /* local symbol table PM-pointer */
   PMMONO *np;        /* c-pointer to module node */

/*
***load the module to stack
*/
   if ( ( status = pmlsta( modname, retla ) ) != 0 )
      {        
      return( status );         /* Error */
      }

/*
***set PM-base to start from module header
*/
   status = pmsbla( *retla );   
   np = (PMMONO *)pmgadr((pm_ptr)0 );

/*
***initialize local symbol table
*/
   if ( ( status = stlini( np->plosb_, modname, &symbla ) ) != 0 )
      {  
      return( status );         /* Error */
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmsave(
        pm_ptr   modptr)

/*      Save active module an a file.
 *
 *      In:   
 *
 *      Out: 
 *
 *      FV:    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;

   if ( modptr == (pm_ptr)NULL ) 
      return( erpush( "PM1011", "" ) );   /* no module at base 0, module not saved */
                                                   
/*
***save active module on file
*/
   if ( ( status = pmssta() ) != 0 )
      return( status );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmlmst(
        pm_ptr  modptr,
        pm_ptr  statptr)

/*      Link module statement.
 *      Länkar in en sats i den aktiva modulens satslista.
 *
 *      In:   modptr   =>  PM-pointer to an module header.
 *            statptr  =>  PM-pointer to an statement node.
 *
 *      Out: 
 *
 *      FV:    return   - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;           /* c-pointer to module node */
   pm_ptr stlla;         /* statement list PM-pointer */
   pm_ptr lnodla;        /* list node pointer, dummy for pmtcon() */
   short status;

   if ( modptr != pmgaba() )
      {  
      return( erpush( "PM3053", "" ));  /* Error  Not module header */
       }

   if ( modptr != pmgbla() )
      status = pmsbla( modptr );

/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;

/*
***Link the statement in the list
*/
   if ( ( status = pmtcon( statptr, stlla, &np->pstl_, &lnodla ) ) != 0 )
      {  
      return( status );                  /* Error */
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmlmpa(
        pm_ptr   modptr,
        pm_ptr   parptr)

/*      Link module parameter.
 *      Länkar in en modulparameter i den aktiva modulens parameterlista.
 *
 *      In:   modptr   =>  PM-pointer to an module header.
 *            parptr   =>  PM-pointer to an module parameter node.
 *
 *      Out: 
 *
 *      FV:    return   - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;        /* c-pointer to module node */
   pm_ptr palla;      /* statement list PM-pointer */
   pm_ptr lnodla;     /* list node pointer, dummy for pmtcon() */
   short status;

   if ( modptr != pmgaba() )
      {  
      return( erpush( "PM3053", "" ));  /*Error  Not module header */
      }

   if ( modptr != pmgbla() )
      status = pmsbla( modptr );

/*
***Get PM-pointer to module parameter list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   palla = np->ppali_;

/*
***Link the statement in the list
*/
   if ( ( status = pmtcon( parptr, palla, &np->ppali_, &lnodla ) ) != 0 )
      {  /* Error */
      return( status );
      }

   return( 0 );
  } 

/********************************************************/
/*!******************************************************/

        short pmumod(
        PMMODULE *module)

/*      Update module header.
 *      Updaterar modulhuvudet för den aktiva modulen.
 *
 *      In:   modptr   =>  interface structure for module info.
 *
 *      Out: 
 *
 *      FV:    return   - status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
  PMMONO *np;      /* c-pointer to module node */
  string str;      /* c-pointer to module name string area in PM */


/*
***Fill in data in the module node
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );    /* Fix a c-pointer to module node in PM */

/*
***get c-address to module name in PM and copy from interface struct
*/
   str = pmgadr( np->mona_ );
   strcpy( str, module->mname );

   np->moty_ = module->mtype;
   np->moat_ = module->mattri;
   np->ppali_ = module->parlist;
   np->pstl_ = module->stlist;
   np->geidlev = module->idmax;
   np->datasize = module->ldsize;
   np->sysdat.sernr = module->system.sernr;
   np->sysdat.vernr = module->system.vernr;
   np->sysdat.revnr = module->system.revnr;
   np->sysdat.level = module->system.level;
   np->sysdat.year_c = module->system.year_c;
   np->sysdat.mon_c = module->system.mon_c;
   np->sysdat.day_c = module->system.day_c;
   np->sysdat.hour_c = module->system.hour_c;
   np->sysdat.min_c = module->system.min_c;
   np->sysdat.year_u = module->system.year_u;
   np->sysdat.mon_u = module->system.mon_u;
   np->sysdat.day_u = module->system.day_u;
   np->sysdat.hour_u = module->system.hour_u;
   np->sysdat.min_u = module->system.min_u;
   strcpy( np->sysdat.sysname, module->system.sysname );
   np->sysdat.mpcode = module->system.mpcode ;              /* MN860610 */
   strcpy( np->sysdat.release, module->system.release );
   strcpy( np->sysdat.version, module->system.version );

/*
***Krypterat serienummer, 1996-01-26, J. Kjellander
*/
   np->sysdat.ser_crypt = module->system.ser_crypt;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmrmod(
        PMMODULE *module)

/*      Read module header.
 *      Läser data från  modulhuvudet för den aktiva modulen.
 *
 *      In:   modptr   =>  interface structure for module info.
 *
 *      Out: 
 *
 *      FV:    return   - status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;      /* c-pointer to module node */
   string str;      /* temporary string pointer to module name */


/*
***Fill in data in the module node
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );    /* Fix a c-pointer to module node in PM */

/* 
***get module name string from PM and copy it to interface struct
*/
   pmgstr( np->mona_, &str );
   strcpy( module->mname, str );
   module->mtype = np->moty_;
   module->mattri = np->moat_;
   module->parlist = np->ppali_;
   module->stlist = np->pstl_;
   module->idmax = np->geidlev;
   module->ldsize = np->datasize;
   module->system.sernr = np->sysdat.sernr;
   module->system.vernr = np->sysdat.vernr;
   module->system.revnr = np->sysdat.revnr;
   module->system.level = np->sysdat.level;
   module->system.year_c = np->sysdat.year_c;
   module->system.mon_c = np->sysdat.mon_c;
   module->system.day_c = np->sysdat.day_c;
   module->system.hour_c = np->sysdat.hour_c;
   module->system.min_c = np->sysdat.min_c;
   module->system.year_u = np->sysdat.year_u;
   module->system.mon_u = np->sysdat.mon_u;
   module->system.day_u = np->sysdat.day_u;
   module->system.hour_u = np->sysdat.hour_u;
   module->system.min_u = np->sysdat.min_u;
   strcpy( module->system.sysname, np->sysdat.sysname );
   module->system.mpcode = np->sysdat.mpcode ;            /* MN860610 */
   strcpy( module->system.release, np->sysdat.release );
   strcpy( module->system.version, np->sysdat.version );

/*
***Krypterat serienummer, 1996-01-26, J. Kjellander
*/

module->system.ser_crypt = np->sysdat.ser_crypt;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcpar(
        pm_ptr  symbp,
        char   *prompt,
        pm_ptr *retla)

/*      Create module parameter.
 *      Skapar en modulparameternod, för senare inlänkning i en parameterlista.
 *
 *      In:    symbp   =>  PM-pointer to the parameter in the symboltable, local part
 *            *prompt  =>  Prompt string
 *
 *      Out:  *retla   =>  Logical address to the parameter node
 *
 *      FV:    return  -   status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMPANO *np;            /* c-pointer to the parameter node */
   pm_ptr pmla;           /* temp PM-pointer */

/*
***Allocate a block for the parameter node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );    /* Error  No room in PM */ 
      }

/*
***Create a string in PM and copy the prompt string to it
*/
   if ( ( status = pmcstr( prompt , &pmla ) ) != 0 )
      {  
      return( status );    /* Error */ 
      }
 
/*
***Fill in data in the parameter node
*/
   np = (PMPANO *)pmgadr( *retla );   /*Fix a c-pointer to the parameter node in PM*/
   np->noclpa = PARAM;                /* Node class */
   np->fopa_ = symbp;
   np->ppro_ = pmla;                  /* insert the PM-pointer to the prompt string in PM */

   return( 0 );
  } 

/********************************************************/
/*!******************************************************/

        short pmcass(
        pm_ptr  lvalue,
        pm_ptr  rvalue,
        pm_ptr *retla)

/*      Create assignment statement.
 *
 *      In:    lvalue  =>  PM-pointer to variable expression
 *             rvalue  =>  PM-pointer to left expression
 *
 *      Out:  *retla   =>  Logical address to the statment node
 *
 *      FV:    return  -   error severity code  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;      /* c-pointer to the statement node */
   PMASST *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                /*Error No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np =(PMSTNO *)pmgadr( *retla );     /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                  /* Node class */
   np->suclst = ASS_ST;                /* Node Subclass */
   sp = (PMASST *)&np->stsubc.procst;  /* Fix a c-pointer to the procedure 
                                                  statement subclass  union */
/*
***Fill in the subclass union
*/
  sp->asvar = lvalue;    
  sp->asexpr = rvalue;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcifs(
        pm_ptr  condl,
        pm_ptr  statl,
        pm_ptr *retla)

/*      Create if statement.
 *
 *      In:    lvalue  =>  PM-pointer to condition list
 *             rvalue  =>  PM-pointer to statement list
 *
 *      Out:  *retla   =>  Logical address to the statment node
 *
 *      FV:    return  -   error severity code  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;      /* c-pointer to the statement node */
   PMIFST *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );                 /*Error No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np =(PMSTNO *)pmgadr( *retla );      /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                   /* Node class */
   np->suclst = IF_ST;                  /* Node Subclass */
   sp = (PMIFST *)&np->stsubc.procst;   /*Fix a c-pointer to the subclass  union */
/*
***Fill in the subclass union
*/
   sp->ifcond = condl;    
   sp->ifstat = statl;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcfos(
        pm_ptr  loopvar,
        pm_ptr  startex,
        pm_ptr  toex,
        pm_ptr  stepex,
        pm_ptr  statl,
        pm_ptr *retla)

/*      Create for statement.
 *
 *      In:    loopvar =>  Loop variable, pointer to symbol table
 *             startex =>  Start expression
 *             toex    =>  Stop expression
 *             stepex  =>  Step expression
 *             statl   =>  PM-pointer to statement list
 *
 *      Out:  *retla   =>  Logical address to the statment node
 *
 *      FV:    return  -   error severity code  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;      /* c-pointer to the statement node */
   PMFOST *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );                 /* Error  No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np =(PMSTNO *)pmgadr( *retla );      /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                   /* Node class */
   np->suclst = FOR_ST;                 /* Node Subclass */
   sp = (PMFOST *)&np->stsubc.procst;   /*Fix a c-pointer to the subclass  union */
/*
***Fill in the subclass union
*/
   sp->fovar = loopvar;
   sp->fostar = startex;
   sp->foto = toex;
   sp->fostep = stepex;    
   sp->fordo = statl;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcgos(
        pm_ptr  label,
        pm_ptr *retla)

/*      Create goto statement.
 *
 *      In:    label   =>  PM-pointer to symbol table
 *
 *      Out:  *retla   =>  Logical address to the statment node
 *
 *      FV:    return  -   error severity code  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;      /* c-pointer to the statement node */
   PMGOST *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );                 /*Error  No room in PM */ 
      }

/*
***Fill in data in the statement node 
*/
   np =(PMSTNO *)pmgadr( *retla );      /*Fix a c-ptr to statement node in PM*/
   np->noclst = STAT;                   /* Node class */
   np->suclst = GO_ST;                  /* Node Subclass */
   sp = (PMGOST *)&np->stsubc.procst;   /*Fix a c-ptr to subclass  union */
/*
***Fill in the subclass union
*/
   sp->golabe = label;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmclas(
        pm_ptr label,
        pm_ptr stat,
        pm_ptr *retla)

/*      Create labeled statement.
 *
 *      In:    label   =>  PM-pointer to symbol table
 *             stat    =>  PM-pointer to statement 
 *
 *      Out:  *retla   =>  Logical address to the statment node
 *
 *      FV:    return  -   error severity code  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;   /* c-pointer to the statement node */
   PMLAST *sp;   /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                 /*Error  No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np =(PMSTNO *)pmgadr( *retla );      /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                   /* Node class */   
   np->suclst = LAB_ST;                 /* Node Subclass */
   sp = (PMLAST *)&np->stsubc.procst;   /*Fix a c-pointer to the subclass  union */
/*
***Fill in the subclass union
*/
   sp->lalabe = label;
   sp->lastat = stat;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcprs(
        pm_ptr  nameid,
        pm_ptr  valparam,
        pm_ptr *retla)

/*      Create procedure statement.
 *      Skapar en proceduranrops-sats, med en pekare till 
 *      symboltabellen som indikerar procedurens namn och 
 *      en värdeparameterlista som parametrar. 
 *
 *      In:    nameid   =>  PM-pointer to symbol table, for procedure name
 *             valparam =>  Actual value parameter list, expression list 
 *
 *      Out:  *retla    =>  Logical address to the statment node
 *
 *      FV:    return   -   status   
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;      /* c-pointer to the statement node */
   PMPRST *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                 /* Error  No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np = (PMSTNO *)pmgadr( *retla );     /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                   /* Node class */
   np->suclst = PRO_ST;                 /* Node Subclass */
   sp = (PMPRST *)&np->stsubc.procst;   /* Fix a c-pointer to the procedure 
                                                  statement subclass  union */
/*
***Fill in the subclass union
*/
   sp->prname = nameid;                 /* Geometric procedure name id pointer */ 
   sp->pracva = valparam;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcpas(
        pmseqn  geid,
        char   *mname,
        pm_ptr  mnexpr,
        pm_ptr  mpara,
        pm_ptr  ppara,
        pm_ptr  nampara,
        pm_ptr *retla)

/*      Create part statement.
 *
 *      In:    geid     =>  Geometric identity
 *            *mname    =>  Module name as string or NULL 
 *             mnexpr   =>  Module name as expression or NULL 
 *             mpara    =>  Module parameters 
 *             ppara    =>  The part call parameters 
 *             nampara  =>  The part call named parameters 
 *
 *      Out:  *retla    =>  Logical address to the statment node
 *
 *      FV:    return   -   status   
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1993-08-15 Ny representation av namnet på den anropade modulen J. Kjellander
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;         /* c-pointer to the parameter node */
   PMPAST *sp;         /* c-pointer to the subclass union */
   pm_ptr pmla;        /* temp PM-pointer */
   PMMODULE module;    /* interface structure for module information */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                  /*Error  No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np = (PMSTNO *)pmgadr( *retla );      /*Fix a c-pointer to the statement node in PM*/
   np->noclst = STAT;                    /* Node class */
   np->suclst = PART_ST;                 /* Node Subclass */
   sp = (PMPAST *)&np->stsubc.partst;    /* Fix a c-pointer to the part 
                                              statement subclass  union */
/*
***Fill in the subclass union
*/
   sp->geident = geid;

/*
***Om namnet på den anropade parten är en string lagrar
***vi det i PM som en PM-stäng. Detta gäller alla versioner
***av V3 t.o.m. version 1.11.
*/
   if ( mname != NULL )
     {
     if ( ( status = pmcstr( mname, &pmla ) ) < -1 ) return( status );
     sp->modname = pmla;     
     }
/*
***Om namnet är angivet som ett uttryck lagrar vi istället
***en pekare till uttrycket. För att skilja på de två varianterna
***inför vi här (och endast här) konventionen att uttryckspekaren
***föregås av ett minustecken och strängpekaren är positiv. Detta
***gäller för version 1.12 och senare.
*/
   else
     {
     sp->modname = -mnexpr;     
     }

   sp->modpara = mpara;
   sp->partpara = ppara;
   sp->partacna = nampara;

/*
***update "idmax" in module header
*/
   pmrmod( &module );
   if ( geid > module.idmax )
      {
      module.idmax = geid;
      pmumod( &module );
      }

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcges(
        pm_ptr nameid,
        pmseqn geid,
        pm_ptr valparam,
        pm_ptr namparam,
        pm_ptr *retla)

/*      Create geometric statement.
 *      Skapar en geometridefinitionsrutinanrops-sats, med ett tal som indikerar 
 *      geometrirutinens namn, en värdeparameterlista och en namnparameterlista 
 *      som parametrar.
 *
 *      In:    nameid   =>  PM-pointer to symbol table, for procedure name
 *             geid     =>  Geometric identity 
 *             valparam =>  Actual value parameter list, expression list 
 *             namparam =>  Actual named parameter list, Name_param list 
 *
 *      Out:  *retla    =>  Logical address to the statment node
 *
 *      FV:    return   -   status   
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMSTNO *np;         /* c-pointer to the statement node */
   PMGEST *sp;         /* c-pointer to the subclass union */
   PMMODULE module;    /* interface structure for module information */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                 /*Error  No room in PM */ 
      }

/*
***Fill in data in the statement node
*/
   np = (PMSTNO *)pmgadr( *retla );     /*Fix a c-pointer to the statement node in PM */
   np->noclst = STAT;                   /* Node class */
   np->suclst = GEO_ST;                 /* Node Subclass */
   sp = (PMGEST *)&np->stsubc.geo_st;   /* Fix a c-pointer to the geometric
                                                  statement subclass  union */
/*
***Fill in the subclass union
*/
   sp->geident = geid;
   sp->gename = nameid;                 /* Geometric procedure name id pointer */ 
   sp->geacva = valparam;
   sp->geacna = namparam;

/*
***update "idmax" in module header
*/
   pmrmod( &module );
   if ( geid > module.idmax )
      {
      module.idmax = geid;
      pmumod( &module );
      }

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcune(
        pmopty opid,
        pm_ptr expr,
        pm_ptr *retla)

/*      Create unary expression.
 *      Skapar en unäroperatornod, operator id och uttryck ges som parametrar. 
 *
 *      In:    opid   =>  operator id
 *             expr   =>  PM-pointer to expression 
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   errror severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

{
   DBint bsize;
   short status;
   PMEXNO *np;      /* c-pointer to the expression node */
   PMUNEX *sp;      /* c-pointer to the subclass union */

/*
***Allocate a block for the expression node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                    /*Error  No room in PM */ 
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr(*retla);          /*Fix a c-pointer to the expression node in PM*/
   np->noclex = EXPR;                      /* Node class */
   np->suclex = C_UN_EX;                   /* Node Subclass */
   sp = (PMUNEX *)&np->ex_subcl.unop_ex;   /* Fix a c-pointer to the unary 
                                                expression subclass  union */
/*
***Fill in the subclass union
*/
   sp->unop = opid;
   sp->p_unex = expr;  

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcbie(
        pmopty  opid,
        pm_ptr  lexpr,
        pm_ptr  rexpr,
        pm_ptr *retla)

/*      Create binary expression.
 *      Skapar en binäroperatornod, operator id , vänster och höger uttryck ges 
 *      som parametrar. 
 *
 *      In:    opid   =>  Operator id
 *             lexpr  =>  PM-pointer to left expression 
 *             rexpr  =>  PM-pointer to rigth expression 
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   errror severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMEXNO *np;      /* c-pointer to the expression node */
   PMBINEX *sp;     /* c-pointer to the subclass union */

/*
***Allocate a block for the expression node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );                      /*Error  No room in PM */ 
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr(*retla);            /*Fix a c-pointer to the expression node in PM*/
   np->noclex = EXPR;                        /* Node class */
   np->suclex = C_BIN_EX;                    /* Node Subclass */
   sp = (PMBINEX *)&np->ex_subcl.binop_ex;   /* Fix a c-pointer to the binary 
                                                   expression subclass  union */
/*
***Fill in the subclass union
*/
   sp->binop = opid;
   sp->p_bin_l = lexpr;  
   sp->p_bin_r = rexpr;  

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmclie(
        PMLITVA *lit_p,
        pm_ptr  *retla)

/*      Create literal expression.
 *      Skapar en uttrycksnod som lagrar en literal. Parametern utgörs av en 
 *      c-pekare till en "struct" som lagrar literalens värde och en variabel som 
 *      indikerar literalens typ. Funktionen kopierar in värdet i PM STRING och
 *      REF konverteras till PM-pekare.
 *
 *      In:   *lit_p  =>  Pointer to an literal value struct defined in
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   status
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   DBint bsize;
   short status;
   PMEXNO *np;         /* c-pointer to the expression node */
   PMLITEX *sp;        /* c-pointer to the subclass union */
   PMVECVA *vecp;      /* c-pointer to vector value struct */
   PMREFVA *refp;      /* c-pointer to an ref-value */
   PMREFEX *tp;        /* c-pointer to an ref-value in PM */
   pm_ptr pmla;        /* temp PM-pointer */

/*
***Allocate a block for the expression node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                    /* Error */   
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr( *retla );        /*Fix a c-pointer to the expression in PM*/
   np->noclex = EXPR;                      /* Node class */
   np->suclex = C_LIT_EX;                  /* Node Subclass */

/*
***Fill in the subclass union
*/
   sp = (PMLITEX *)&np->ex_subcl.lit_ex;   /* Fix a c-pointer to the literal 
                                                  expression subclass  union */
   sp->lit_type = lit_p->lit_type;
   switch ( lit_p->lit_type ) {

     case C_INT_VA:
        sp->litex.int_li = lit_p->lit.int_va;
        break;

     case C_FLO_VA:
        sp->litex.float_li = lit_p->lit.float_va;
        break;

     case C_STR_VA:
/*
***Allocate space for a string in PM and copy the string to PM
*/
        if ( ( status = pmcstr( lit_p->lit.str_va , &pmla ) ) != 0 )
           {  
           return( status );                  /* Error */    
           }
        sp->litex.str_li = pmla;              /* PM-pointer to the string in PM */
        break;

     case C_VEC_VA:
/*
***Allocate a block for the vector value
*/
        bsize = sizeof( *vecp );
        if ( ( status = pmallo( bsize, &pmla ) ) != 0 )
           {  
           return( erpush( "PM0013", "" ) );    /*Error  No room in PM */ 
           }

/*
***Fill in data in the vector value structure in PM
*/
        vecp = (PMVECVA *)pmgadr( pmla );       /* Fix a c-pointer to the vector
                                                              value struct in PM */
        vecp->x_val = lit_p->lit.vec_va.x_val;
        vecp->y_val = lit_p->lit.vec_va.y_val;
        vecp->z_val = lit_p->lit.vec_va.z_val;

        sp->litex.vec_li = pmla;                /* Put a link to the vector struct in the
                                                                  literal expression node */
        break;

     case C_REF_VA:
        refp = &( lit_p->lit.ref_va[ 0 ] );
        tp = &( sp->litex.ref_li );
  
        tp->seq_lit = refp->seq_val;            /* fill in first ref in chain */
        tp->ord_lit = refp->ord_val; 

        while ( refp->p_nextre != NULL )
           {
           if (( status = pmallo((DBint)sizeof( *tp ), &pmla ) ) != 0 )
              {  
              return( status );                 /* Error */
              }

           tp->nextref = pmla;

/*
***step to next ref
*/
           refp = refp->p_nextre;
           tp = (PMREFEX *)pmgadr( pmla );

           tp->seq_lit = refp->seq_val;         /* fill in next ref in chain */
           tp->ord_lit = refp->ord_val; 
           }

        tp->nextref = (pm_ptr)NULL;

        break;

     default:
        return( erpush( "PM0124", "" ) );       /* Not a literal, unrecognised id */
        break;
     }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcide(
        pm_ptr  id_p,
        pm_ptr *retla)

/*      Create identifier expression.
 *      Skapar en uttrycksnod för en variabel, med en pekare till symboltabellen 
 *      som parameter. 
 *
 *      In:   *lit_p  =>  PM-pointer to an identifier in symbol table
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   status
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   DBint bsize;
   short status;
   PMEXNO *np;       /* c-pointer to the expression node */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );               /* Error */
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr( *retla );   /*Fix a c-pointer to the expression in PM */
   np->noclex = EXPR;                 /* Node class */
   np->suclex = C_ID_EX;              /* Node Subclass */

/*
***Fill in the subclass union
*/
   np->ex_subcl.id_ex.p_id = id_p;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcine(
        pm_ptr  indid_p,
        pm_ptr  experl,
        pm_ptr *retla)

/*      Create index expression.
 *
 *      In:   *lit_p  =>  PM-pointer to an identifier in symbol table
 *             experl =>  Index list ( expression list ) 
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   status
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   DBint bsize;
   short status;
   PMEXNO *np;       /* c-pointer to the expression node */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                /* Error */
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr( *retla );    /*Fix a c-pointer to the expression in PM */
   np->noclex = EXPR;                  /* Node class */
   np->suclex = C_IND_EX;              /* Node Subclass */

/*
***Fill in the subclass union
*/
   np->ex_subcl.ind_ex.p_indid = indid_p;
   np->ex_subcl.ind_ex.p_indli = experl;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmccoe(
        pm_ptr   comvar,
        pmfieldt field,
        pm_ptr  *retla)

/*      Create component expression.
 *
 *      In:    comvar =>  PM-pointer to an exper_node
 *             field  =>  Indicates component X, Y or Z 
 *
 *      Out:  *retla  =>  Logical address to the expression node
 *
 *      FV:    return -   status
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   DBint bsize;
   short status;
   PMEXNO *np;           /* c-pointer to the expression node */

/*
***Allocate a block for the statement node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );               /* Error */ 
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr( *retla );   /*Fix a c-pointer to the expression in PM */
   np->noclex = EXPR;                 /* Node class */
   np->suclex = C_COM_EX;             /* Node Subclass */

/*
***Fill in the subclass union
*/
   np->ex_subcl.comp_ex.p_comvar = comvar;
   np->ex_subcl.comp_ex.p_cfield = field;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcfue(
        pm_ptr  fid,
        pm_ptr  arglist,
        pm_ptr *retla)

/*      Create function expression.
 *      Skapar ett funktionsanropsuttryck, med identitet till funktionsnamn 
 *      och en pekare till en argumentlista (uttryckslista) som parametrar.
 *
 *      In:    fid     =>  PM-pointer to symbol table, routine name
 *             arglist =>  PM-pointer to expression list (argument list) 
 *
 *      Out:  *retla   =>  PM-pointer to the expression node
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMEXNO *np;         /* c-pointer to the expression node */

/*
***Allocate a block for the expression node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );               /* Error */ 
      }

/*
***Fill in data in the expression node
*/
   np = (PMEXNO *)pmgadr( *retla );   /*Fix a c-pointer to the expression in PM */
   np->noclex = EXPR;                 /* Node class */
   np->suclex = C_FUN_EX;             /* Node Subclass */

/*
***Fill in the subclass union
*/
   np->ex_subcl.func_ex.funcid = fid;
   np->ex_subcl.func_ex.p_funcar = arglist;

   return ( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcnpa(
        pmmdid paname,
        pm_ptr valuep,
        pm_ptr *retla)

/*      Create named parameter.
 *      Skapar en namnparamenternod, med namnet och värdesuttrycket som parameter 
 *
 *      In:    paname  =>  Idetifier for modifier name
 *             valuep  =>  PM-pointer to an expression
 *
 *      Out:  *retla   =>  PM-pointer to the named paremeter node
 *
 *      FV:    return  -   status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMNPNO *np;        /* c-pointer to the named parameter node */

/*
***Allocate a block for the named parameter node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      {  
      return( status );                /* Error */
      }

/*
***Fill in data in the named parameter node
*/
   np = (PMNPNO *)pmgadr( *retla );    /* Fix a c-pointer to the named parameter node */
   np->noclnp = NAMEPA;                /* Node class */
   np ->par_np = paname;               /* parameter id-value */
   np->p_val_np = valuep;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmccon(
        pm_ptr  cond,
        pm_ptr  statl,
        pm_ptr *retla)

/*      Create condition.
 *
 *      In:    cond    =>  PM-pointer to an expression
 *             statl   =>  Statement list
 *
 *      Out:  *retla   =>  PM-pointer to the condition node
 *
 *      FV:    return  -   status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMCONO *np;        /* c-pointer to the condition node */

/*
***Allocate a block for the condition node in PM
*/
   bsize = sizeof( *np );
   if ( ( status = pmallo( bsize, retla ) ) != 0 )
      { 
      return( status );                  /* Error */
      }

/*
***Fill in data in the condition node
*/
   np = (PMCONO *)pmgadr( *retla );      /*Fix a c-pointer to the condition node*/
   np->noclco = COND;                    /* Node class */
   np->p_cond = cond;
   np->p_stl_co = statl;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmtcon(
        pm_ptr  gennop,
        pm_ptr  listp,
        pm_ptr *retla, 
        pm_ptr *lnodep)

/*      Tail concatenete and create an list node.
 *      Skapar en listnod och lägger in en länk till en generisk nod, därefter 
 *      länkas listnoden in i en lista. Pekare till generisk nod och lista ges
 *      som parametrar. Om det är första elementet i listan skall NULL ges som 
 *      argument till "pmtcon". En lista består alltid av en TCONC_NODE som 
 *      håller pekare till början och slutet av listan med list noder.
 *
 *      In:    gennop  =>  PM-pointer to an generic node (stat, expr ...)
 *             listp   =>  PM-pointer to an tconc node, if NULL the tconc
 *                         node will be created. 
 *
 *      Out:  *retla   =>  PM-pointer to the list (tconc node)
 *            *lnodep  =>  PM-pointer to the created list node
 *
 *      FV:    return  -   status  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMTCNO *tnp;       /* c-pointer to the tconc node */
   PMLINO *lnp;       /* c-pointer to the list node */
   PMLINO *newlnp;    /* c-pointer to the created list node */
   pm_ptr pmla;       /* temp PM-pointer */

/*
***Allocate a block for the list node in PM
*/
   bsize = sizeof( *newlnp );
   if ( ( status = pmallo( bsize, &pmla ) ) != 0 )
      {  
      return( status );                        /*Error  No room in PM */ 
      }

/*
***set pointer to list node for out parameter
*/
   *lnodep = pmla;

/*
***Fill in the list node
*/
   newlnp = (PMLINO *)pmgadr( pmla );
   newlnp->noclli = LIST;                      /* Node class */
   newlnp->p_no_li = gennop;                   /* put a link to the generic node */ 
   newlnp->p_nex_li = (pm_ptr)NULL;

/*
***Check if it is the first element in the list
*/
   if ( listp == (pm_ptr)NULL )
      {  
/*
***Create an tconc node and link the list node
***Allocate a block for the tconc node in PM
*/
      bsize = sizeof( *tnp );
      if ( ( status = pmallo( bsize, retla ) ) != 0 )
          {  
          return( status );                    /*Error  No room in PM */ 
          }
    
      tnp = (PMTCNO *)pmgadr( *retla );        /* convert and fix an c-pointer to
                                                             the tconc node in PM */
      tnp->nocltc = TCONC;
      tnp->p_head = pmla;
      tnp->p_tail = pmla;
      }
   else
      {  
/*
***Concatenate the new list node to the tail of the list
*/
      tnp = (PMTCNO *)pmgadr( listp );         /* c-pointer to the tconc node */

      lnp = (PMLINO *)pmgadr( tnp->p_tail );   /* c-pointer to the tail list node */
      lnp->p_nex_li = pmla;                    /* link the new list node to the tail */
      tnp->p_tail = pmla;                      /* and put the link in the tconc node */

      *retla = listp;                          /* set the return variable to the tconc node */
      }

   return( 0 ); 
  }

/********************************************************/
/*!******************************************************/

        short pmcstr(
        char   *str,
        pm_ptr *retla)

/*      Create string .
 *      Skapar och lägger in en textsträng i PM. 
 *
 *      In:    str     =>  Char pointer to the string
 *
 *      Out:  *retla   =>  PM-pointer to the string in PM
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char *chp;           /* character pointer */
   short status;

/*
***Allocate a block for the string in PM
*/

   if ( ( status = pmallo((DBint)(strlen( str ) + 1), retla ) ) != 0 )
      {  
      return( status );        /* Error */
      }
/*
***copy the string into PM
*/
   chp = pmgadr( *retla );     /* Fix a c-pointer to the string in PM */
   strcpy( chp, str );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmcref(
        PMREFVA *refp,
        pm_ptr  *retla)

/*      Create referens value
 *      Skapar plats och lägger in referensvärden i PM från en länkad list med 
 *      c-länkar. Rutinen anropar sig själv recursivt för att bygga upp listan 
 *      med referens värden i PM 
 *
 *      In:    str     =>  C-pointer to an referens chain
 *
 *      Out:  *retla   =>  PM-pointer to one referens in PM
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint bsize;
   short status;
   PMREFEX *tp;        /* c-pointer to an ref-value in PM */
   pm_ptr pmla;        /* temp PM-pointer */

   if ( refp != NULL )
      {
/*
***more ref. in chain
*
***Allocate a block for the ref. value in PM
*/
      bsize = sizeof( *tp );
      if ( ( status = pmallo( bsize, &pmla ) ) != 0 )
         {  
         return( status );              /* Error */
         }
/*
***Fill in values in ref. struct in PM
*/
      tp = (PMREFEX *)pmgadr( pmla );   /* Fix a c-pointer to struct in PM */
      tp->seq_lit = refp->seq_val;
      tp->ord_lit = refp->ord_val;

      refp = refp->p_nextre;            /* let refp point to next ref. in chain */
/*
***Make a recursive call to get the PM-address for next ref.
***in the chain in PM
*/
      if ( ( status = pmcref( refp, &pmla ) ) != 0 )
         {  
         *retla = (pm_ptr)NULL;
         return( status );              /* Error */
         }
      tp->nextref = pmla;
      }
   else 
      {  
      *retla = (pm_ptr)NULL;            /* End of ref chain */
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        void pmstpr(
        pm_ptr   mbase)

/*      Module Symbol Table Parameter RTS address calculation.
 *      beräknar RTS adress samt uppdaterar symboltabellen med denna adress. 
 *
 *      In:    mbase =>  PM base pointer to module
 *
 *      Out:  
 *
 *      FV:    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-17 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   DBint    size;       /* parameter size */
   DBint    rtsofs;     /* current base pointer */
   pm_ptr  parali;      /* PM pointer to parameter list */
   pm_ptr  parala;      /* PM pointer to parameter node */
   pm_ptr  currb;       /* current PM-base */
   PMPANO *paranp;      /* c-pointer to parameter node */
   PMMONO *np;          /* c-pointer to module node */
   STVAR   var;         /* interface struct for symbol table */

   currb = pmgbla();    /* store current PM base */
   pmsbla( mbase );     /* set PM base to target module */

/*
***Get module node
*/
   pmgmod( (pm_ptr)0, &np );

/*
***Get first list node
*/
   pmgfli( np->ppali_, &parali );

/*
***First address after data frame
*/
   rtsofs = np->datasize;

   while ( parali != (pm_ptr)NULL )
      {
/*
***Get contents of list node
*/
      pmglin(parali, &parali, &parala);
      pmgpar(parala, &paranp);

/*
***Get symbol table info
*/
      strvar(paranp->fopa_, &var);

      if ( var.kind_va == ST_RPAVA )
         size = v2addrsz;
      else
         size = var.size_va;

/*
***Calculate allignment using the same algorithm as in inallo()
*/
      if ((size % MIN_BLKS) != 0 )
         size = ((size + MIN_BLKS)/MIN_BLKS)*MIN_BLKS;

/*
***Update symbol table with RTS address
*/
      var.addr_va = rtsofs;
      stuvar(paranp->fopa_, &var);

      rtsofs = rtsofs + size;
      }

   pmsbla(currb);
  }

/********************************************************/
