/**********************************************************************
*
*    pmac6.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    short pmwrme();   Vem/vilka refererar till mig ?
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

/*
***reftab är en tabell som innehåller data om alla referenser
***till storheten id och i den ordning dom kommer.
***Minne till reftab allokeras av pmwrme() som också fyller i data.
***refant är antal referenser i reftab. refsiz talar om hur 
***mycket minne (i antal element) som allokerats för reftab.
***REFBSZ anger hur många element i taget som allokeras.
*/

static PMREFL *reftab = NULL;
static int     refant = 0;
static int     rtabsz = 0;
#define REFBSZ 100

static short wrussl(pm_ptr stlist);
static short wrusst(pm_ptr statla);
static bool  wrusel(pm_ptr exlist);
static bool  wrusex(pm_ptr exprla);
static bool  wrusid(pm_ptr symla);

/***************************************************************/
/*!*************************************************************/

        short pmwrme(
        PMREFVA  *id,
        PMREFL  **prtabp,
        int      *rant)

/*      Vem/vilka refererar till mig ? Går igenom aktiv modul
 *      och noterar alla direkta eller indirekta referenser till
 *      storheten id i reftab.
 *
 *      För varje referens noteras den refererande satsens:
 *        - Sekvensnummer om det är en Part/Geo-sats.
 *        - PM-adress för ev. reinterpretering.
 *        - Typ dvs. Part/Geo/If/Proc osv.
 *
 *      Antal referenser ges av variablen refant-1.
 *
 *      OBS! 1:a elementet i reftab är den refererade storheten
 *           själv.
 *
 *      In: id     = Storhet att analysera.
 *          prtabp = Pekare till utdata.
 *          rant   = Pekare till utdata.
 *
 *      Ut: refant(-1) element i reftab. (1:a elementet = id själv)
 *          *prtabp = Adressen till reftab.
 *          *rant   = Antal referenser.
 *
 *      FV:  0  => Ok.
 *
 *      Felkoder: PM2502 = Kan ej allokera minne
 *
 *      (C)microform ab 30/4/92 J. Kjellander
 *
 *************************************************************!*/

 {
   short   status;
   PMMONO *np;

/*
***Om rtabsz > 0 har vi anropats vid ett tidigare tillfälle.
***Isåfall finns minne allokerat som vi kan frigöra.
*/
    if ( rtabsz > 0 )
      {
      v3free(reftab,"pmwrme");
      rtabsz = 0;
      }
/*
***Allokera nytt minne för reftab.
*/
   if ( (reftab=(PMREFL *)v3mall(REFBSZ*sizeof(PMREFL),"pmwrme")) == NULL )
     return(erpush("PM2502","malloc"));

   rtabsz = REFBSZ;
   refant = 0;
/*
***Skriv in storheten id först i reftab.
*/
   reftab[refant].snr    = id->seq_val;
   reftab[refant].statla = (pm_ptr)NULL;
   reftab[refant].typ    = (pm_ptr)NULL;
   ++refant;
/*
***Hämta C-pekare till aktiv modul.
*/
   if ( (status=pmgmod((pm_ptr)0,&np)) != 0 ) return(status);

   if ( np->monocl != MODULE ) return(erpush("PM2542",""));
/*
***Gå igenom modulens satslista. Fel kan uppstå om minne inte
***kan allokeras.
*/
   if ( wrussl(np->pstl_) < 0 ) return(erpush("PM2502","realloc"));
/*
***Om allt gick bra returnera adressen till reftab och antal referenser.
*/
   else
     {
     *prtabp = reftab;
     *rant = refant;
     return(0);
     }
 }

/***************************************************************/
/*!*************************************************************/

 static short wrussl(pm_ptr stlist)

/*      Letar efter referenser till reftab i en sats-lista.
 *
 *      In: stlist = PM-pekare till sats-lista.
 *
 *      Ut: Fyller på refererande storheters snr i reftab.
 *
 *      FV:  0 = OK.
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 *************************************************************!*/

 {
   pm_ptr  listla;  /* PM-pekare till list node */ 
   pm_ptr  nextla;  /* PM-pekare till nästa list-node */
   pm_ptr  statla;  /* PM-pekare till sats-node */

/*
***Tom satslista ?
*/
   if ( stlist == (pm_ptr)NULL ) return(0);
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
     if ( wrusst(statla) < 0 ) return(-1);
     listla = nextla;
     }

   return(0);
 }

/***************************************************************/
/*!*************************************************************/

 static short wrusst(pm_ptr statla)

/*      Letar efter referenser till reftab i en sats.
 *
 *      In: statla => Satsens PM-adress.
 *
 *      Ut: Fyller på reftab.
 *
 *      FV:  0 = OK.
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 ******************************************************!*/

  {
   PMSTNO *np;
   PMGEST *geop;
   PMPAST *partp;

/*
***Är det en tom sats ?
*/
   if ( statla == (pm_ptr)NULL ) return(0);
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
/*
     case IF_ST:
     colist = np->stsubc.if_st.ifcond;
     if ( colist != NULL )
       ä
       pmgfli(colist,&colila);
       while ( colila != NULL )
         ä
         pmglin(colila,&conxla,&condla);
         pmgcon(condla,&condp);
         if ( wrussl(condp->p_stl_co) < 0 ) return(-1);
         colila = conxla;
         å
       å
     if ( wrussl(np->stsubc.if_st.ifstat) < 0 ) return(-1);
     break;
*/
/*
***FOR har också en egen satslista.
*/
/*
     case FOR_ST:
     if ( wrussl(np->stsubc.for_st.fordo) < 0 ) return(-1);
     break;
*/
/*
***Part. Här finns dels en möjlighet till referens i själva
***part-procedurens parameterlista och dessutom oändliga
***möjligheter i parameterlistan till den part som anropas.
*/
     case PART_ST:
     partp = &(np->stsubc.partst);
     if ( wrusel(partp->modpara) == TRUE  ||
         (partp->partpara != (pm_ptr)NULL && wrusel(partp->partpara) == TRUE) )
       {
       if ( refant == rtabsz )
         {
         if ( (reftab=(PMREFL *)v3rall((char *)reftab,
                        (rtabsz+REFBSZ)*sizeof(PMREFL),"wrusst")) == NULL )
           return(-1);
         else
           rtabsz += REFBSZ;
         }
       reftab[refant].snr    = partp->geident;
       reftab[refant].statla = statla;
       reftab[refant].typ    = PART_ST;
       ++refant;
       }
     break;
/*
***Geometri-sats.
*/
     case GEO_ST:
     geop = &(np->stsubc.geo_st);
     if ( wrusel(geop->geacva) == TRUE )
       {
       if ( refant == rtabsz )
         {
         if ( (reftab=(PMREFL *)v3rall((char *)reftab,
                      (rtabsz+REFBSZ)*sizeof(PMREFL),"wrusst")) == NULL )
           return(-1);
         else
           rtabsz += REFBSZ;
         }
       reftab[refant].snr    = geop->geident;
       reftab[refant].statla = statla;
       reftab[refant].typ    = GEO_ST;
       ++refant;
       }
     break;
/*
***Proceduranrops-sats.
*/
/*
     case PRO_ST:
     procp = &(np->stsubc.procst);
     stratt(procp->prname,&idclass,&str);
     if ( strcmp(str,"SET") == 0  öö  strcmp(str,"SET_BASIC") == 0 )
      return(FALSE);
     else if ( wrusel(id,procp->pracva) == TRUE ) return(TRUE);
     break;
*/
/*
***Lablad sats har en egen sats.
*/
/*
     case LAB_ST:
     return(wrusst(id,np->stsubc.labest.lastat));
     break;
*/
     }

  return(FALSE);
  }

/***********************************************************/
/*!*********************************************************/

 static bool wrusel(pm_ptr exlist)

/*      Analyserar om en uttrycks-lista innebär en referens
 *      till reftab.
 *
 *      In: exlist => Listans PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referens finns.
 *          FALSE = Referens finns ej.
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 *********************************************************!*/

 {
   pm_ptr listla,nextla,exprla;

   if ( exlist == (pm_ptr)NULL ) return(FALSE);

   pmgfli(exlist,&listla);

   while ( listla != (pm_ptr)NULL )
     { 
     pmglin(listla,&nextla,&exprla);
     if ( wrusex(exprla) == TRUE ) return(TRUE);
     listla = nextla;
     }

   return(FALSE);
 }

/***********************************************************/
/*!*********************************************************/

 static bool wrusex(pm_ptr exprla)

/*      Analyserar om ett uttryck innebär en referens
 *      till reftab.
 *
 *      In: exlist => Listans PM-adress.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referens finns.
 *          FALSE = Referens finns ej.
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 *********************************************************!*/

  {
   int      i;
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
     if ( wrusex(np->ex_subcl.unop_ex.p_unex) == TRUE ) return(TRUE);
     break;
/*
***Binary expression, dvs. binärt uttryck typ a+b. Här
***blir det två uttryck att analysera.
*/
     case C_BIN_EX:
     if ( wrusex(np->ex_subcl.binop_ex.p_bin_l) == TRUE ) return(TRUE);
     if ( wrusex(np->ex_subcl.binop_ex.p_bin_r) == TRUE ) return(TRUE);
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
       for ( i=0; i<refant; ++i )
         if ( refp->seq_lit == reftab[i].snr ) return(TRUE);
       }
     break;
/*
***Enkel variabel. Vad skall göras med denna ?
*/
     case C_ID_EX:
     if ( wrusid(np->ex_subcl.id_ex.p_id) == TRUE ) return(TRUE);
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
     if ( wrusel(np->ex_subcl.func_ex.p_funcar) == TRUE ) return(TRUE);
     break;
     }

   return(FALSE);
 } 

/***********************************************************/
/*!*********************************************************/

 static bool wrusid(pm_ptr symla)

/*      Analyserar om en identifierare innebär en referens
 *      till reftab.
 *
 *      In: symla  => Identifierarens symboltabellpekare.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Referens.
 *          FALSE = Ej referens.
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 *********************************************************!*/

 {
   int     i;          /* Loop-räknare */
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
***Konstant.
*/
     case ST_CONST:
     strcon(symla,&konst);
     strtyp(konst.type_co,&typ);

     if ( typ.kind_ty == ST_REF )
       {
       for ( i=0; i<refant; ++i )
         {
         if ( konst.valu_co.lit.ref_va[0].seq_val == reftab[i].snr )
           return(TRUE);
         }
       return(FALSE);
       }
     break;
/*
***Övriga.
*/
     default:
     return(FALSE);
     }
   return(FALSE);
 } 

/***********************************************************/
