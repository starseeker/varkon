/**********************************************************************
*
*    pmac3.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    bool  pmargs();    Hitta framåt-referenser i GEO/PART-sats
*    bool  pmarex();    Hitta framåt-referenser i ett uttryck
*    bool  pmamir();    Är jag refererad ?
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

static short pmcsnt();
static short csndsl(pm_ptr stlist);
static short csndst(pm_ptr statla);
static bool  arelst(pm_ptr exlist, int snrtpt);
static bool  arexpr(pm_ptr exprla, int snrtpt);
static bool  fndrsl(PMREFVA *id, pm_ptr stlist);
static bool  fndrst(PMREFVA *id, pm_ptr statla);
static bool  fndrel(PMREFVA *id, pm_ptr exlist);
static bool  fndrex(PMREFVA *id, pm_ptr exprla);
static bool  fndrid(PMREFVA *id, pm_ptr symla);

/*
***snrtab är en tabell som innehåller sekvensnummer för alla
***storheter som finns i modulen och i den ordning dom kommer.
***Minne till snrtab allokeras av pmcsnt() som också fyller i data.
***snrant är antal identiteter i snrtab. snrsiz talar om hur 
***mycket minne (i antal DBseqnum) som allokerats för snrtab.
***SNRBSZ anger hur många DBseqnum i taget som allokeras.
*/
static DBseqnum *snrtab = NULL;
static int       snrant = 0;
static int       snrsiz = 0;
#define SNRBSZ 50

/***************************************************************/
/*!*************************************************************/

 static short pmcsnt()

/*      Skapa snrtab.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      Felkoder: PM2502 = Kan ej allokera minne
 *
 *      (C)microform ab 23/1/92 J. Kjellander
 *
 *************************************************************!*/

 {
   short   status;
   PMMONO *np;

/*
***Om snrsiz > 0 har vi anropats vid ett tidigare tillfälle.
***Isåfall finns minne allokerat som vi kan frigöra.
*/
    if ( snrsiz > 0 )
      {
      v3free(snrtab,"pmcsnt");
      snrsiz = 0;
      }
/*
***Allokera nytt minne för snrtab.
*/
   if ( (snrtab=(DBseqnum *)v3mall(SNRBSZ*sizeof(DBseqnum),"pmcsnt")) == NULL )
     return(erpush("PM2502","malloc"));

   snrsiz = SNRBSZ;
   snrant = 0;
/*
***Hämta C-pekare till aktiv modul.
*/
   if ( (status=pmgmod((pm_ptr)0,&np)) != 0 ) return(status);

   if ( np->monocl != MODULE ) return(erpush("PM2542",""));
/*
***Gå igenom modulens satslista.
*/
   status = csndsl(np->pstl_);

   return(status);
 }

/***************************************************************/
/*!*************************************************************/

 static short csndsl(pm_ptr stlist)

/*      Går igenom en sats-lista.
 *
 *      In: stlist = PM-pekare till sats-lista.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 23/1/92 J. Kjellander
 *
 *************************************************************!*/

 {
   short   status;
   pm_ptr  listla;  /* PM-pekare till list node */ 
   pm_ptr  nextla;  /* PM-pekare till nästa list-node */
   pm_ptr  statla;  /* PM-pekare till sats-node */

/*
***Tom satslista ?
*/
   if ( stlist == (pm_ptr)NULL ) return( 0 );
/*
***Hämta första list-noden.
*/
   if ( (status=pmgfli(stlist,&listla)) != 0 ) return(status);
/*
***Följ listan tills den är slut. För varje listnod, kolla
***motsvarande sats.
*/
   while ( listla != (pm_ptr)NULL )
     { 
     if ( (status=pmglin(listla,&nextla,&statla)) != 0 ) return(status);
     if ( (status=csndst(statla)) != 0 ) return( status );
     listla = nextla;
     }

   return(0);
 }

/***************************************************************/
/*!*************************************************************/

 static short csndst(pm_ptr statla)

/*      Kollar en sats.
 *
 *      In: statla => Satsens PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 23/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
   short     status;
   pm_ptr    colist,colila,conxla,condla;
   PMSTNO   *np;
   PMCONO   *condp;

/*
***Är det en tom sats ?
*/
   if ( statla == (pm_ptr)NULL ) return(0);
/*
***Hämta C-pekare till satsen och kolla att det är en sats.
*/
   pmgsta(statla,&np);
   if ( np->noclst != STAT ) return(erpush("PM2512",""));
/*
***Vilken typ av sats är det ? Vi intresserar oss bara för
***Geometri-satser och satser med egna sats-listor.
*/
   switch ( np->suclst )
     {
/*
***IF har en eller flera egna satslistor. Först en condition-list som
***kan bestå av en eller flera conditions, som i sin tur innehåller
***var sin egen satslista och sist en ytterligare sats-lista om
**det finns en else-gren.
*/
     case IF_ST:
     colist = np->stsubc.if_st.ifcond;
     if ( colist != (pm_ptr)NULL )
       {
       pmgfli(colist,&colila);
       while ( colila != (pm_ptr)NULL )
         {
         pmglin(colila,&conxla,&condla);
         pmgcon(condla,&condp);
         if ( (status=csndsl(condp->p_stl_co)) < 0 ) return(status);
         colila = conxla;
         }
       }
     if ( (status=csndsl(np->stsubc.if_st.ifstat)) < 0 ) return(status);
     break;
/*
***FOR har också en egen satslista.
*/
     case FOR_ST:
     if ( (status=csndsl(np->stsubc.for_st.fordo)) < 0 ) return(status);
     break;
/*
***Part- eller Geometri-procedur.
*/
     case GEO_ST:
     case PART_ST:
     if ( snrant == snrsiz )
       {
       if ( (snrtab=(DBseqnum *)v3rall((char *)snrtab,
                       (snrsiz+SNRBSZ)*sizeof(DBseqnum),"csndst")) == NULL )
         return(erpush("PM2502","realloc"));
       else snrsiz += SNRBSZ;
       }
     snrtab[snrant++] = np->stsubc.geo_st.geident;
     break;
/*
***Lablad sats har en egen sats.
*/
     case LAB_ST:
     if ( (status=csndst(np->stsubc.labest.lastat)) < 0 ) return(status);
     break;
     }

  return(0);
  }

/***********************************************************/
/*!*********************************************************/

        bool pmargs(pm_ptr statla)

/*      Analyserar om en geometri- eller part-sats innebär
 *      en framåt-referens.
 *
 *      In: statla => PM-pekare till generisk nod.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Framåt-referens finns.
 *          FALSE = Inga framåt-referenser.
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 *********************************************************!*/

 {
   int     snrtpt; /* Satsens plats i snrtab */
   short   status;
   PMSTNO *np;     /* c-pointer to statement node */
   PMGEST *geop;   /* c-pointr to geo_stat node */
   PMPAST *partp;  /* c-pointer to part-stat subnode */

/*
***Tom sats ?
*/
   if ( statla == (pm_ptr)NULL ) return(0); 
/*
***Skapa sekvensnummer-tabell.
*/
   status = pmcsnt();
   if ( status < 0 ) return(status);
/*
***Nähä, det var ju kul. Då får vi väl ta och titta på den då.
*/
   if ( (status=pmgsta(statla,&np)) != 0 ) return(status);

   if ( np->noclst != STAT ) return(erpush("PM2512",""));
/*
***Vilken typ av sats är det ?
*/
   switch ( np->suclst )
     {
/*
***Part. Här finns dels en möjlighet till referens i själva
***part-procedurens parameterlista och dessutom oändliga
***möjligheter i parameterlistan till den part som anropas.
*/
     case PART_ST:
     partp = &(np->stsubc.partst);
/*
***Leta upp part-satsen i snrtab.
*/
     snrtpt = 0;
     while ( snrtpt < snrant )
       if ( snrtab[snrtpt++] == np->stsubc.partst.geident ) break;
/*
***Kolla båda parameterlistorna efter framåt-referenser.
*/
     if ( arelst(partp->modpara,snrtpt) == TRUE ) return(TRUE);
     if ( partp->partpara != (pm_ptr)NULL  &&
          arelst(partp->partpara,snrtpt) == TRUE ) return(TRUE);
     break;
/*
***Geometri-sats.
*/
     case GEO_ST:
     geop = &(np->stsubc.geo_st);
/*
***Leta upp storheten själv i snrtab.
*/
     snrtpt = 0;
     while ( snrtpt < snrant )
       if ( snrtab[snrtpt++] == np->stsubc.geo_st.geident ) break;
/*
***Gå igenom satsen och kolla för varje referens om den finns
***i resten av snrtab, dvs. utgör en framåt-referens.
*/
     if ( geop->geacva != (pm_ptr)NULL  &&
          arelst(geop->geacva,snrtpt) == TRUE ) return(TRUE);
     break;

     default:
     return(erpush("PM2512",""));
     break;
     }

   return(FALSE);
 } 

/***********************************************************/
/*!*********************************************************/

        bool pmarex(
        PMREFVA *idvek,
        pm_ptr   exprla)

/*      Analyserar om ett uttryck innebär en framåt-referens.
 *
 *      In: idvek  => Pekare till storhets identitet.
 *          exprla => PM-pekare till uttryck, tex. parameter i 
 *                    part-anrop.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Framåt-referens finns.
 *          FALSE = Inga framåt-referenser.
 *
 *      (C)microform ab 17/2/92 J. Kjellander
 *
 *      1996-06-05 abs(id->seq_val), J.Kjellander
 *
 *********************************************************!*/

 {
   int     snrtpt; /* Satsens plats i snrtab */
   short   status;

/*
***Skapa sekvensnummer-tabell.
*/
   status = pmcsnt();
   if ( status < 0 ) return(TRUE);
/*
***Leta upp satsen i snrtab.
*/
     snrtpt = 0;
     while ( snrtpt < snrant )
       if ( snrtab[snrtpt++] == abs(idvek[0].seq_val) ) break;
/*
***Kolla uttrycket efter framåt-referenser.
*/
     return(arexpr(exprla,snrtpt));
 } 

/***********************************************************/
/*!*********************************************************/

 static bool arelst(
        pm_ptr exlist,
        int    snrtpt)

/*      Analyserar om en uttrycks-lista innebär en framåt-referens.
 *
 *      In: exlist => Listans PM-adress.
 *          snrtpt => Plats i snrtab att kolla mot.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Framåt-referens finns.
 *          FALSE = Inga framåt-referenser.
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 *********************************************************!*/

 {
   pm_ptr listla,nextla,exprla;

   if ( exlist == (pm_ptr)NULL ) return(FALSE);

   pmgfli(exlist,&listla);

   while ( listla != (pm_ptr)NULL )
     { 
     pmglin(listla,&nextla,&exprla);
     if ( arexpr(exprla,snrtpt) == TRUE ) return(TRUE);
     listla = nextla;
     }

   return(FALSE);
 }

/***********************************************************/
/*!*********************************************************/

 static bool arexpr(
        pm_ptr exprla,
        int    snrtpt)

/*      Analyserar om ett uttryck innebär en framåt-referens.
 *
 *      In: exprla => Uttryckets PM-adress.
 *          snrtpt => Plats i snrtab att kolla emot.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Framåtreferens finns.
 *           FALSE = Inga framåtreferenser.
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 *********************************************************!*/

  {
   short    status;
   int      i;
   DBseqnum sekvnr;
   PMEXNO  *np;
   PMLITEX *litp;
   PMREFEX *refp;

/*
***NULL-uttryck.
*/
   if ( exprla == (pm_ptr)NULL ) return(FALSE);
/*
***Hämta C-pekare till uttrycket.
*/
   if ( (status=pmgexp(exprla,&np)) != 0 ) return(status);

   if ( np->noclex != EXPR ) return(erpush("PM2522",""));
/*
***Vilken typ av uttryck är det ?
*/
   switch ( np->suclex )
     {
/*
***Unary expression, dvs. unärt plus eller minus. Analysera
***det uttryck som hör till.
*/
     case C_UN_EX:
     if ( arexpr(np->ex_subcl.unop_ex.p_unex,snrtpt) == TRUE ) return(TRUE);
     break;
/*
***Binary expression, dvs. binärt uttryck typ a+b. Här
***blir det två uttryck att analysera.
*/
     case C_BIN_EX:
     if ( arexpr(np->ex_subcl.binop_ex.p_bin_l,snrtpt) == TRUE ) return(TRUE);
     if ( arexpr(np->ex_subcl.binop_ex.p_bin_r,snrtpt) == TRUE ) return(TRUE);
     break;
/*
***Litteral. Om det är en REF-literal kan det vara frågan
***om en framåt-referens.
*/
     case C_LIT_EX:
     litp = &(np->ex_subcl.lit_ex);

     if ( litp->lit_type == C_REF_VA )
       {
       refp = &litp->litex.ref_li;
       sekvnr = refp->seq_lit;
       i = snrtpt - 1;
       while ( i < snrant )
         if ( snrtab[i++] == sekvnr ) return(TRUE);
       }
     break;
/*
***Enkel variabel. Vad skall göras med denna ?
*/
     case C_ID_EX:
     break;
/*
***Indexerad variabel. Vad skall göras med denna ?
*/
     case C_IND_EX:
     break;
/*
***Kompound, tex. p.x i en VECTOR. Vad skall göras med denna ?
*/
     case C_COM_EX:
     break;
/*
***Funktion. Noll eller flera parametrar i form av en expr.-list.
*/
     case C_FUN_EX:
     if ( arelst(np->ex_subcl.func_ex.p_funcar,snrtpt) == TRUE ) return(TRUE);
     break;

     default:
     erpush("PM2522","");
     break;
     }

   return(FALSE);
 } 

/***********************************************************/
/*!*************************************************************/

        bool pmamir(PMREFVA *id)

/*      Kollar om satsen id är refererad. En referens i en
 *      icke exekverad else-gren räknas ändå som en referens !
 *
 *      In: id => Satsens id.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Jag är refererad.
 *           FALSE = Jag är inte refererad.
 *
 *      (C)microform ab 19/2/92 J. Kjellander
 *
 *************************************************************!*/

 {
   PMMONO *np;

/*
***Hämta C-pekare till aktiv modul.
*/
   pmgmod((pm_ptr)0,&np);
/*
***Gå igenom modulens satslista.
*/
   return(fndrsl(id,np->pstl_));

   return(0);
 }

/***************************************************************/
/*!*************************************************************/

 static bool fndrsl(
        PMREFVA *id,
        pm_ptr   stlist)

/*      Letar efter viss referens i en sats-lista.
 *
 *      In: id     = Referens att leta efter.
 *          stlist = PM-pekare till sats-lista.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Jag är refererad.
 *           FALSE = Jag är inte refererad.
 *
 *      (C)microform ab 19/2/92 J. Kjellander
 *
 *************************************************************!*/

 {
   pm_ptr  listla;  /* PM-pekare till list node */ 
   pm_ptr  nextla;  /* PM-pekare till nästa list-node */
   pm_ptr  statla;  /* PM-pekare till sats-node */

/*
***Tom satslista ?
*/
   if ( stlist == (pm_ptr)NULL ) return(FALSE);
/*
***Hämta första list-noden.
*/
   pmgfli(stlist,&listla);
/*
***Följ listan tills den är slut. För varje listnod, kolla
***motsvarande sats.
*/
   while ( listla != (pm_ptr)NULL )
     { 
     pmglin(listla,&nextla,&statla);
     if ( fndrst(id,statla) == TRUE ) return(TRUE);
     listla = nextla;
     }

   return(FALSE);
 }

/***************************************************************/
/*!*************************************************************/

 static bool fndrst(
        PMREFVA *id,
        pm_ptr   statla)

/*      Letar efter viss referens i en sats.
 *
 *      In: id     => Referens att leta efter.
 *          statla => Satsens PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Jag är refererad.
 *           FALSE = Jag är inte refererad.
 *
 *      (C)microform ab 19/2/92 J. Kjellander
 *
 ******************************************************!*/

  {
   stidcl  idclass;
   char   *str;
   pm_ptr  colist,colila,conxla,condla;
   PMSTNO *np;
   PMCONO *condp;
   PMGEST *geop;
   PMPAST *partp;
   PMPRST *procp;

/*
***Är det en tom sats ?
*/
   if ( statla == (pm_ptr)NULL ) return(FALSE);
/*
***Hämta C-pekare till satsen och kolla att det är en sats.
*/
   pmgsta(statla,&np);
/*
***Vilken typ av sats är det ? Vi intresserar oss bara för
***Geometri-satser och satser med egna sats-listor.
*/
   switch ( np->suclst )
     {
/*
***IF har en eller flera egna satslistor. Först en condition-list som
***kan bestå av en eller flera conditions, som i sin tur innehåller
***var sin egen satslista och sist en ytterligare sats-lista om
**det finns en else-gren.
*/
     case IF_ST:
     colist = np->stsubc.if_st.ifcond;
     if ( colist != (pm_ptr)NULL )
       {
       pmgfli(colist,&colila);
       while ( colila != (pm_ptr)NULL )
         {
         pmglin(colila,&conxla,&condla);
         pmgcon(condla,&condp);
         if ( fndrsl(id,condp->p_stl_co) == TRUE ) return(TRUE);
         colila = conxla;
         }
       }
     if ( fndrsl(id,np->stsubc.if_st.ifstat) == TRUE ) return(TRUE);
     break;
/*
***FOR har också en egen satslista.
*/
     case FOR_ST:
     if ( fndrsl(id,np->stsubc.for_st.fordo) == TRUE ) return(TRUE);
     break;
/*
***Part. Här finns dels en möjlighet till referens i själva
***part-procedurens parameterlista och dessutom oändliga
***möjligheter i parameterlistan till den part som anropas.
*/
     case PART_ST:
     partp = &(np->stsubc.partst);
     if ( fndrel(id,partp->modpara) == TRUE ) return(TRUE);
     if ( partp->partpara != (pm_ptr)NULL  &&
          fndrel(id,partp->partpara) == TRUE ) return(TRUE);
     break;
/*
***Geometri-sats.
*/
     case GEO_ST:
     geop = &(np->stsubc.geo_st);
     if ( fndrel(id,geop->geacva) == TRUE ) return(TRUE);
     break;
/*
***Proceduranrops-sats.
*/
     case PRO_ST:
     procp = &(np->stsubc.procst);
     stratt(procp->prname,&idclass,&str);
     if ( strcmp(str,"SET") == 0  ||  strcmp(str,"SET_BASIC") == 0 )
      return(FALSE);
     else if ( fndrel(id,procp->pracva) == TRUE ) return(TRUE);
     break;
/*
***Lablad sats har en egen sats.
*/
     case LAB_ST:
     return(fndrst(id,np->stsubc.labest.lastat));
     break;
     }

  return(FALSE);
  }

/***********************************************************/
/*!*********************************************************/

 static bool fndrel(
        PMREFVA *id,
        pm_ptr   exlist)

/*      Analyserar om en uttrycks-lista innebär en framåt-referens.
 *
 *      In: id     => Referens att söka efter.
 *          exlist => Listans PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referensen finns.
 *          FALSE = Referensen finns ej.
 *
 *      (C)microform ab 18/2/92 J. Kjellander
 *
 *********************************************************!*/

 {
   pm_ptr listla,nextla,exprla;

   if ( exlist == (pm_ptr)NULL ) return(FALSE);

   pmgfli(exlist,&listla);

   while ( listla != (pm_ptr)NULL )
     { 
     pmglin(listla,&nextla,&exprla);
     if ( fndrex(id,exprla) == TRUE ) return(TRUE);
     listla = nextla;
     }

   return(FALSE);
 }

/***********************************************************/
/*!*********************************************************/

 static bool fndrex(
        PMREFVA *id,
        pm_ptr   exprla)

/*      Analyserar om ett uttryck innebär en framåt-referens.
 *
 *      In: id     => Referens att söka efter.
 *          exlist => Listans PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referensen finns.
 *          FALSE = Referensen finns ej.
 *
 *      (C)microform ab 18/2/92 J. Kjellander
 *
 *      1996-06-05 abs(id->seq_val), J.Kjellander
 *
 *********************************************************!*/

  {
   PMEXNO  *np;
   PMLITEX *litp;
   PMREFEX *refp;

/*
***NULL-uttryck.
*/
   if ( exprla == (pm_ptr)NULL ) return(FALSE);
/*
***Hämta C-pekare till uttrycket.
*/
   pmgexp(exprla,&np);
/*
***Vilken typ av uttryck är det ?
*/
   switch ( np->suclex )
     {
/*
***Unary expression, dvs. unärt plus eller minus. Analysera
***det uttryck som hör till.
*/
     case C_UN_EX:
     if ( fndrex(id,np->ex_subcl.unop_ex.p_unex) == TRUE ) return(TRUE);
     break;
/*
***Binary expression, dvs. binärt uttryck typ a+b. Här
***blir det två uttryck att analysera.
*/
     case C_BIN_EX:
     if ( fndrex(id,np->ex_subcl.binop_ex.p_bin_l) == TRUE ) return(TRUE);
     if ( fndrex(id,np->ex_subcl.binop_ex.p_bin_r) == TRUE ) return(TRUE);
     break;
/*
***Litteral. Om det är en REF-literal kan det vara frågan
***om den sökta referensen.
*/
     case C_LIT_EX:
     litp = &(np->ex_subcl.lit_ex);

     if ( litp->lit_type == C_REF_VA )
       {
       refp = &litp->litex.ref_li;
       if ( abs(id->seq_val) == refp->seq_lit ) return(TRUE);
       }
     break;
/*
***Identifierare. Kan vara en CONSTANT REF.
*/
     case C_ID_EX:
     if ( fndrid(id,np->ex_subcl.id_ex.p_id) == TRUE ) return(TRUE);
     break;
/*
***Indexerad variabel. Vad skall göras med denna ?
*/
     case C_IND_EX:
     break;
/*
***Kompound, tex. p.x i en VECTOR. Vad skall göras med denna ?
*/
     case C_COM_EX:
     break;
/*
***Funktion. Noll eller flera parametrar i form av en expr.-list.
*/
     case C_FUN_EX:
     if ( fndrel(id,np->ex_subcl.func_ex.p_funcar) == TRUE ) return(TRUE);
     break;
     }

   return(FALSE);
 } 

/***********************************************************/
/*!*********************************************************/

 static bool fndrid(
        PMREFVA *id,
        pm_ptr   symla)

/*      Analyserar om en identifierare innebär en framåt-referens.
 *
 *      In: id     => Referens att söka efter.
 *          symla  => Identifierarens symboltabellpekare.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referensen finns.
 *          FALSE = Referensen finns ej.
 *
 *      (C)microform ab 30/4/92 J. Kjellander
 *
 *      1996-06-05 abs(id->seq_val), J.Kjellander
 *      1996-06-06 Bug inget returvärde, J.Kjellander
 *
 *********************************************************!*/

 {
   stidcl  idclass;    /* identifier class */
   string  symname;    /* string for symbol name */
   STCONST konst;      /* st structure for a constant */
   STTYTBL typ;        /* MBS-type definition structure */

/*
***Läs attribut från symboltabell.
*/
   stratt(symla,&idclass,&symname);
/*
***Vilken typa av symbol är det ?
*/
   switch ( idclass )
     {
/*
***Konstant. Här returnerades inget väldefinierat värde TRUE
***eller FALSE om det inte var en REF-konstant. Slumpen gjorde
***att det för det mesta returnerades ett värde <> 0 dvs. TRUE
***men i samband med att abs(seq_val) infördes råkade 0 returneras
***vilket gjorde att tex. en INT-konstant fick rutinen att påstå
***att en referens till den angivna storheten förelåg.
*/
     case ST_CONST:
     strcon(symla,&konst);
     strtyp(konst.type_co,&typ);

     if ( typ.kind_ty == ST_REF  &&
          abs(id->seq_val) == konst.valu_co.lit.ref_va[0].seq_val )
       return(TRUE);
     else
       return(FALSE);
/*
***Övriga.
*/
     default:
     return(FALSE);
     }
 } 

/***********************************************************/
