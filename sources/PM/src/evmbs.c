/**********************************************************************
*
*    evmbs.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evaddm();      Evaluerar ADD_MBS 
*    evdelm();      Evaluerar DEL_MBS 
*    evrunm();      Evaluerar RUN_MBS 
*    evgpm();       Evaluerar GETP_MBS 
*    evupm();       Evaluerar UPDP_MBS 
*    evuppm();      Evaluerar UPDPP_MBS 
*    evclpm();      Evaluerar CLEAR_PM 
*    evposm();      Evaluerar POS_MBS 
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
#include "../../AN/include/AN.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include <string.h>

extern char     jobnam[];
extern short    sysmode,modtyp,tmpref;
extern pm_ptr   actmod;
extern struct   ANSYREC sy;
extern V2NAPA   defnap;
extern DBseqnum snrmax;

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evaddm()

/*      Evaluerar ADD_MBS.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 12/2/92 J. Kjellander
 *
 *      20/5/92    modtyp, J. Kjellander
 *      7/2/94     Bytt till funktion, J. Kjellander
 *      1996-04-23 Bug oldmod, J. Kjellander
 *      1998-03-11 Part utan parametrar, J.Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2004-02-21 Bugfix PM grows in RIT-mode, J.Kjellander
 *
 ******************************************************!*/

  {
    short    i,status,oldhit,oldsav,oldmty;
    bool     prtflg;
    int      snr;
    char     mbsstr[V2PARMAX*V3STRLEN];
    char    *oldrtb;
    char     curmna[V2SYNLEN+1];
    pm_ptr   retla,oldmod;
    ANFSET   set;
    PMMODULE modhed;
    PMMONO  *np;

/*
***Bygg ihop en MBS-sats av parametrarna i pv.
***1:a parametern är namnet på rutinen.
***Därefter lägger vi till nästa lediga sekvensnummer.
*/
    snr = (int)IGgnid();
    sprintf(mbsstr,"%s(#%d",func_pv[1].par_va.lit.str_va,snr);
/*
***Funktionsvärdet = den nya storhetens id.
*/
    func_vp->lit.ref_va[0].seq_val = snr;
    func_vp->lit.ref_va[0].ord_val = 1;
    func_vp->lit.ref_va[0].p_nextre = NULL;
/*
***Är det frågan om en part-sats ?
*/
    if ( sticmp("PART",func_pv[1].par_va.lit.str_va) == 0 ) prtflg = TRUE;
    else prtflg = FALSE;
/*
***Om det var en part är nästa parameter part-namnet.
*/
    if ( prtflg )
      {
      strcat(mbsstr,",");
      strcat(mbsstr,func_pv[2].par_va.lit.str_va);
      strcat(mbsstr,"(");
      for ( i=3; i<=func_pc; ++i )
        {
        strcat(mbsstr,func_pv[i].par_va.lit.str_va);
        strcat(mbsstr,",");
        }
      if ( func_pc > 2 ) mbsstr[strlen(mbsstr)-1] = '\0';
      strcat(mbsstr,"));");
      }
/*
***Ej part, alltså en vanlig geometri-procedur.
*/
    else
      {
      for ( i=2; i<=func_pc; ++i )
        {
        if ( func_pv[i].par_va.lit.str_va[0] != ':' ) strcat(mbsstr,",");
        strcat(mbsstr,func_pv[i].par_va.lit.str_va);
        }
      strcat(mbsstr,");");
      }
/*
***Vad heter nuvarande modul. Måste vi veta för att
***kunna anropa pmgeba() på slutet.
*/
    pmgmod((pm_ptr)0,&np);
    strcpy(curmna,pmgadr(np->mona_));
/*
***Sätt PM:s och RTS:s bas-pekare till aktiv modul.
***Observera att pmgbla() returnerar MACRO-modulens
***nuvarande basadress men om add_mbs() görs på part
***kan denna bli utswappad och då finns den inte i PM
***när parten exekverat. oldmod är alltså egentligen
***ointressant i det här läget.
*/
    oldmod = pmgbla();
    pmsbla(actmod);

    oldrtb = ingrtb();
    insrtb(inglrb());
/*
***Notera aktuellt läge i PM och initiera scannern.
***Skapa tomt set.
***Hämta första token. anascan() returnerar ingen status.
***Analysera. anunst() är en void.
***Stäng scannern.
*/
    pmmark(); anlogi();
    if ( (status=asinit(mbsstr,ANRDSTR)) < 0 ) goto exit;
    ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
    anascan(&sy);
    anunst(&retla,&set);
    if ( (status=asexit()) < 0 ) goto exit;
/*
***Blev det några fel ?
*/
    if ( anyerr() )
      {
      pmrele();
      erpush("IG3902",mbsstr);
      status = erpush("IG3892","");
      goto exit;
      }
/*
***Interpretera. Först måste dock HIT och SAVE fixas med lite.
***Den MACRO-modul som ADD_MBS(....-satsen ingår i har ju an-
***ropats interaktivt med funktionskod M, dvs. med HIT och SAVE=0.
***Den sats som skall skapas av ADD_MBS skall dock lagras i PM och
***resultatet i GM. Även modtyp måste grejas lite med. Under inter-
***preteringen av den nya satsen är det viktigt att modtyp har samma
***värde som aktiv modul. Detta är inte nödvändigtvis samma som 
***den MACRO-modul som innehåller ADD_MBS(...
*/
    oldhit = defnap.hit; oldsav = defnap.save; oldmty = modtyp;

    defnap.hit = defnap.save = 1;
    pmrmod(&modhed); modtyp = modhed.mtype;

    status = inssta(retla);

    defnap.hit = oldhit; defnap.save = oldsav; modtyp = oldmty;

    if ( status < 0 )
      {
      erpush("IG3902",mbsstr);
      status = erpush("IG3912","");
      goto exit;
      }
/*
***Länka in satsen i modulen.
***retla = Den nya satsen som den kommer från anunst().
*/
    if ( sysmode & GENERIC )
      {
      status = pmlmst(actmod,retla);
      if ( status < 0 ) goto exit;
      }
        else pmrele();    /* 040221 JK Don't let PM grow in RIT-mode */
/*
***Slut.
*/
exit:
/*
***Återställ PM:s och RTS:s baspekare. MACRO-modulen kan
***ha blivit utswappad så vi gör pmgeba()för att säkerställa
***att den finns i PM och på vilken adress.
*/
    pmgeba(curmna,&oldmod);
    pmsbla(oldmod);
    insrtb(oldrtb);

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evdelm()

/*      Evaluerar DEL_MBS.
 *
 *      In: extern proc_pv   = Pekare till parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG4052 = Storheten %s finns ej.
 *
 *      (C)microform ab 24/1-95 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    char    errbuf[81];
    short   status;
    pm_ptr  oldmod,slstla,retla;
    PMMONO *np;

/*
***Sätt PM:s bas-pekare till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Kolla att satsen finns i PM.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = np->pstl_;

    status = pmlges(&proc_pv[1].par_va.lit.ref_va[0],&slstla,&retla);

    if ( status < 0  ||  retla == (pm_ptr)NULL )
      {
      IGidst(&proc_pv[1].par_va.lit.ref_va[0],errbuf);
      status = erpush("IG4052",errbuf);
      goto exit;
      }
/*
***Stryk satsen ur PM.
*/
    pmdges(&proc_pv[1].par_va.lit.ref_va[0]);
/*
***Slut.
*/
    pmsbla(oldmod);
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evrunm()

/*      Evaluerar RUN_MBS.
 *
 *      In: extern proc_pv   = Pekare till parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG4062 = Fel vid exekvering med RUN_MBS.
 *
 *      (C)microform ab 9/3-95 J. Kjellander
 *
 *      1996-04-23 Bug oldmod, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
    short    status,oldmty;
    pm_ptr   oldmod;
    char    *oldrtb,*oldrsp;
    char     curmna[V2SYNLEN+1];
    PMMODULE modhed;
    PMMONO  *np;

/*
***I ritmodulen finns ingen aktiv modul att köra !
*/
    if ( sysmode == EXPLICIT ) return(0);
/*
***Vad heter nuvarande modul. Måste vi veta för att
***kunna anropa pmgeba() på slutet.
*/
    pmgmod((pm_ptr)0,&np);
    strcpy(curmna,pmgadr(np->mona_));
/*
***Spara nuvarande MACRO-moduls PM-baspekare och sätt 
***om den till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Spara nuvarande MACRO-moduls RTS-bas och stack-pekare. 
*/
    oldrtb = ingrtb();
    oldrsp = ingrsp();
/*
***Spara nuvarande MACRO-moduls modultyp och sätt om till
***den som aktiv modul har.
*/
   oldmty = modtyp;
   pmrmod(&modhed);
   modtyp = modhed.mtype;
/*
***Kör om.
*/
   status = IGream();

   if ( status < 0 ) erpush("IG4062","");
/*
***Gör rätt modul aktiv igen.
*/
   modtyp = oldmty;
   insrtb(oldrtb);
   insrsp(oldrsp);
   pmgeba(curmna,&oldmod);
   pmsbla(oldmod);
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgpm()

/*      Evaluerar GETP_MBS. Parameter 0 är procedur-namnet,
 *      tex. POI_FREE eller CUR_SPLINE. Parameter 1 är 1:a
 *      parametern.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:   IG3922 = Parameter %s finns ej.
 *            IG3932 = Storheten är en part.
 *            IG3962 = Storheten finns ej.
 *
 *      (C)microform ab 12/3/92 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short    i,pnum,status;
    pm_ptr   oldmod,slstla,listla,nextla,statla,exlist,
             exprla;
    char    *namn,errbuf[V3STRLEN+1];
    stidcl   idcl;
    PMMONO  *mnp;
    PMSTNO  *snp;
    PMGEST  *gnp;

/*
***Sätt PM:s bas-pekare till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Var i PM ligger satsen ? Först en C-pekare till aktiv modul = np.
***Sen en PM-pekare till aktiv moduls satslista = slstla.
***Sen en PM-pekare till list-noden för satsen = listla.
***Sen en PM-pekare till själva satsen = statla.
*/
    mnp = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = mnp->pstl_;
    pmlges(&func_pv[1].par_va.lit.ref_va[0],&slstla,&listla);
    if ( listla == (pm_ptr)NULL )
      {
      status = erpush("IG3962","GETP_MBS");
      goto exit;
      }
    if ( (status=pmglin(listla,&nextla,&statla)) < 0 ) goto exit;
/*
***Kolla att det är en geometri-procedur.
*/
    pmgsta(statla,&snp);

    switch ( snp->suclst )
      {
      case GEO_ST:
/*
***En C-pekare till geometri-sats och en PM-pekare
***till dess parameterlista.
*/
      gnp = &(snp->stsubc.geo_st);
      exlist = gnp->geacva;
/*
***Leta upp parametern och dekompilera.
*/
      pnum = (short)func_pv[2].par_va.lit.int_va;

      if ( pnum == 0 )
        {
        stratt(gnp->gename,&idcl,&namn);
        strcpy(func_vp->lit.str_va,namn);
        }
      else
        {
        if ( pnum < 0 )
          {
          sprintf(errbuf,"GETP_MBS%%%d",pnum);
          status = erpush("IG3922",errbuf);
          goto exit;
          }
        else
          {
          pmgfli(exlist,&listla);
  
          for ( i=0; i<pnum; ++i )
            {
            if ( listla == (pm_ptr)NULL )
              {
              sprintf(errbuf,"GETP_MBS%%%d",pnum);
              status = erpush("IG3922",errbuf);
              goto exit;
              }
            pmglin(listla,&nextla,&exprla);
            listla = nextla;
            }
          status = pprexs(exprla,mnp->moty_,func_vp->lit.str_va,V3STRLEN-1);
          }
        }
      break;
/*
***Ej geometri-sats.
*/
      default:
      status = erpush("IG3932","GETP_MBS");
      goto exit;
      }
/*
***Slut.
*/
exit:
    pmsbla(oldmod);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evupm()

/*      Evaluerar UPDP_MBS.
 *
 *      In: extern proc_pv   = Pekare till parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: 
 *                IG3932 = Storheten är en part
 *                IG3962 = Storheten finns ej
 *                IF3922 = Parametern %s finns ej, fel i %s
 *                IG3902 = MBS-sträng = %s
 *                IG3942 = Fel vid kompilering med UPDP_MBS(...
 *                IG3952 = Typfel
 *                IG3882 = %s utgör en framåtreferens
 *                IG3972 = Fel vid exekvering
 *
 *      (C)microform ab 12/3/92 J. Kjellander
 *
 *      3/2/94  remode, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short    i,pnum,status,oldmty,remode;
    pm_ptr   oldmod,slstla,listla,nextla,statla,exlist,
             plstla=0,oldexp,newexp,valt;
    char     errbuf[V3STRLEN+1];
    char    *oldrtb,*oldrsp;
    PMMONO  *mnp;
    PMSTNO  *snp;
    PMGEST  *gnp;
    PMLINO  *lnp;
    PMLITVA  valp;
    STPROC   rout;
    STPARDAT par;
    STTYTBL  ftype,atype;
    ANFSET   set;
    ANATTR   attr;
    V2REFVA *id;
    PMMODULE modhed;

/*
***Sätt PM:s bas-pekare till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Spara aktiv moduls RTS-bas och stack-pekare i oldrtb och oldrtp. 
*/
    oldrtb = ingrtb();
    oldrsp = ingrsp();
/*
***Var i PM ligger satsen ? Först en C-pekare till aktiv modul = np.
***Sen en PM-pekare till aktiv moduls satslista = slstla.
***Sen en PM-pekare till list-noden för satsen = listla.
***Sen en PM-pekare till själva satsen = statla.
*/
    mnp = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = mnp->pstl_;
    id = &proc_pv[1].par_va.lit.ref_va[0];
    pmlges(id,&slstla,&listla);
    if ( listla == (pm_ptr)NULL )
     {
     status = erpush("IG3962","UPDP_MBS");
     goto exit;
     }
    if ( (status=pmglin(listla,&nextla,&statla)) < 0 ) goto exit;
/*
***Kolla om det är en geometri-procedur.
*/
    pmgsta(statla,&snp);

    switch ( snp->suclst )
      {
      case GEO_ST:
/*
***En C-pekare till geometri-sats och en PM-pekare
***till dess parameterlista.
*/
      gnp = &(snp->stsubc.geo_st);
      exlist = gnp->geacva;
/*
***Leta upp den aktuella parametern.
*/
      pnum = (short)proc_pv[2].par_va.lit.int_va;

      if ( pnum < 1 )
        {
        sprintf(errbuf,"UPDP_MBS%%%d",pnum);
        status = erpush("IG3922",errbuf);
        goto exit;
        }
      else
        {
        pmgfli(exlist,&listla);

        for ( i=0; i<pnum; ++i )
          {
          if ( listla == (pm_ptr)NULL )
            {
            sprintf(errbuf,"UPDP_MBS%%%d",pnum);
            status = erpush("IG3922",errbuf);
            goto exit;
            }
          pmglin(listla,&nextla,&oldexp);
          plstla = listla;
          listla = nextla;
          }
        }
/*
***oldexp är nu en PM-pekare till den aktuella parametern.
***Ta reda på formell typ = par.type, en pm_ptr till STTYTBL.
***par.type = NULL => anytype, tex. i CUR_CONIC P-värde eller
***mellanliggande punkt.
*/
      strrou(gnp->gename,&rout);
      stsrou(rout.kind_pr);
      stgpar(pnum,&par);
/*
***Analysera det nya uttrycket.
*/
      anlogi();
      if ( (status=asinit(proc_pv[3].par_va.lit.str_va,ANRDSTR)) < 0 ) goto exit;
      ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
      anascan(&sy);
      anarex(&newexp,&attr,&set);
      if ( (status=asexit()) < 0 ) goto exit;
/*
***Blev det några fel ?
*/
      if ( anyerr() )
        {
        erpush("IG3902",proc_pv[3].par_va.lit.str_va);
        status = erpush("IG3942","");
        goto exit;
        }
/*
***Prova att interpretera uttrycket.
*/
      if ( inevex(newexp,&valp,&valt) < 0 )
        {
        erpush("IG3902",proc_pv[3].par_va.lit.str_va);
        status = erpush("IG3942","");
        goto exit;
        }
/*
***Jämför typerna. atype = aktuell typ. ftype = formell typ.
***Om par.type == NULL är parametern deklarerad som "any type".
*/
      if ( par.type != (pm_ptr)NULL )
        {
        strtyp(valt,&atype);
        strtyp(par.type,&ftype);

        if ( atype.kind_ty != ftype.kind_ty )
          {
          if ( !(ftype.kind_ty == ST_FLOAT  &&  atype.kind_ty == ST_INT) )
            {
            status = erpush("IG3952","");
            goto exit;
            }
          }
        }
/*
***Framåt-referens ?
*/
      if ( pmarex(id,newexp) )
        {
        status = erpush("IG3882",proc_pv[3].par_va.lit.str_va);
        goto exit;
        }
/*
***Inga fel, byt ut uttrycket. plstla är gamla uttryckets list-nod.
*/
      lnp = (PMLINO *)pmgadr(plstla);
      lnp->p_no_li = newexp;
/*
***Hur mycket reevaluering som skall göras beror på mode.
***remode = 0 => Fullständig, inkl. kör aktiv om refererad. Default.
***remode = 1 => Bara inkrementell reinterpretering.
***remode = 2 => Ingen reinterpretering.
*/
      remode = proc_pv[4].par_va.lit.int_va;

      if ( remode < 0  ||  remode > 2 ) remode = 0;
/*
***Oavsett om storheten är refererad elle inte provar vi med
***inkrementell interpretering.
*/
      if ( remode < 2 )
        {
        insrtb(inglrb());
        oldmty = modtyp; pmrmod(&modhed); modtyp = modhed.mtype;
        status = EXrist(id); 
        modtyp = oldmty;
        insrtb(oldrtb);
/*
***Om det inte gick bra måste vi återställa efter oss även
***i PM. EXrist() återställer automatiskt i GM och GP !
*/
        if ( status < 0 ) 
          {
          lnp->p_no_li = oldexp;
          status = erpush("IG3972","");
          goto exit;
          }
/*
***Om storheten är refererad kanske vi ska köra hela modulen ?
***Isåfall kan den globala variabeln modtyp behöva sättas.


***OBSOBSOBS varför inte test på ritmodul som i updpp??????


*/
        if ( remode == 0  &&  pmamir(id) && IGialt(175,67,68,FALSE) )
          {
          insrtb(oldrsp);
          oldmty = modtyp; pmrmod(&modhed); modtyp = modhed.mtype;
          status = IGream();
          modtyp = oldmty;
          insrtb(oldrtb);
          insrsp(oldrsp);
          }
        }
      break;
/*
***Ej geometri-sats.
*/
      default:
      status = erpush("IG3932","UPDP_MBS");
      goto exit;
      }
/*
***Slut.
*/
exit:
    pmsbla(oldmod);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evuppm()

/*      Evaluerar UPDPP_MBS.
 *
 *      In: extern proc_pv   = Pekare till parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: 
 *                IG3882 = %s utgör en framåtreferens
 *                IG3902 = MBS-sträng = %s
 *                IF3922 = Parametern %s finns ej, fel i %s
 *                IG3962 = Storheten finns ej
 *                IG3992 = ID har mer än 1 nivå.
 *                IG4002 = Storheten är ej en part
 *                IG4012 = Hittar ej modulen %s
 *                IG4022 = Fel vid kompilering med UPDPP_MBS(...
 *                IG4032 = Typfel
 *                IG4042 = Fel vid exekvering
 *
 *      (C)microform ab 2/2/94 J. Kjellander
 *
 *      1996-04-23 Bug oldmod, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short    i,pnum,status,oldmty,remode;
    pm_ptr   oldmod,slstla,listla,nextla,statla,exlist,
             plstla=0,oldexp,newexp,valt,newmod,panola;
    char     errbuf[V3STRLEN+1],parnam[V3STRLEN+1],pmt[V3STRLEN+1];
    char    *oldrtb,*oldrsp;
    char     curmna[V2SYNLEN+1];
    bool     dstflg;
    DBPart    part;
    PMMONO  *mnp,*np;
    PMSTNO  *snp;
    PMPAST  *pnp;
    PMLINO  *lnp;
    PMLITVA  valp,defval;
    STTYTBL  ftype,atype;
    ANFSET   set;
    ANATTR   attr;
    V2REFVA *id;
    PMMODULE modhed;

/*
***Vad heter nuvarande modul. Måste vi veta för att
***kunna anropa pmgeba() på slutet.
*/
    pmgmod((pm_ptr)0,&np);
    strcpy(curmna,pmgadr(np->mona_));
/*
***Sätt PM:s bas-pekare till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Spara aktiv moduls RTS-bas och stack-pekare i oldrtb och oldrtp. 
*/
    oldrtb = ingrtb();
    oldrsp = ingrsp();
/*
***Till att börja med sätter vi dstflg = FALSE. Skulle det dock
***visa sig att det handlar om ritmodulen och att ett temporärt
***part-anrop skapas sätter vi dstflg = TRUE så att detta säkert
***stryks ur PM när vi avslutar.
*/
    dstflg = FALSE;
/*
***Kolla att storheten ingår i aktiv modul.
*/
    id = &proc_pv[1].par_va.lit.ref_va[0];
    if ( id->p_nextre != NULL )
      {
      IGidst(id,errbuf);
      status = erpush("IG3992",errbuf);
      goto exit;
      }
/*
***I ritmodulen måste vi först återskapa ett part-anrop i PM.
*/
    if ( sysmode == EXPLICIT )
      {
      if ( (status=IGgnps(id)) < 0 ) goto exit;
      else dstflg = TRUE;
      }
/*
***Var i PM ligger satsen ? Först en C-pekare till aktiv modul = np.
***Sen en PM-pekare till aktiv moduls satslista = slstla.
***Sen en PM-pekare till list-noden för satsen = listla.
***Sen en PM-pekare till själva satsen = statla.
*/
    mnp = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = mnp->pstl_;
    pmlges(id,&slstla,&listla);
    if ( listla == (pm_ptr)NULL )
     {
     status = erpush("IG3962","UPDPP_MBS");
     goto exit;
     }
    if ( (status=pmglin(listla,&nextla,&statla)) < 0 ) goto exit;
/*
***Kolla att det är en partsats.
*/
    pmgsta(statla,&snp);

    switch ( snp->suclst )
      {
      case PART_ST:
/*
***En C-pekare till part-sats och en PM-pekare
***till den anropade partens parameterlista.
*/
      pnp = &(snp->stsubc.partst);
      exlist = pnp->modpara;
/*
***Leta upp den aktuella parametern i part-anropet.
*/
      pnum = (short)proc_pv[2].par_va.lit.int_va;

      if ( pnum < 1 )
        {
        sprintf(errbuf,"UPDPP_MBS%%%d",pnum);
        status = erpush("IG3922",errbuf);
        goto exit;
        }
      else
        {
        pmgfli(exlist,&listla);

        for ( i=0; i<pnum; ++i )
          {
          if ( listla == (pm_ptr)NULL )
            {
            sprintf(errbuf,"UPDPP_MBS%%%d",pnum);
            status = erpush("IG3922",errbuf);
            goto exit;
            }
          pmglin(listla,&nextla,&oldexp);
          plstla = listla;
          listla = nextla;
          }
        }
/*
***oldexp är nu en PM-pekare till den aktuella parametern.
***Ta reda på parameterns formella typ. Detta gör vi genom
***att titta i den anropade modulens parameterlista.
***Läs part-posten och ladda in den modul som skapat parten.
*/
    EXgtpt(id,&part);

    if ( pmgeba(part.name_pt,&newmod) != 0 )
      {
      status = erpush("IG4012",part.name_pt);
      goto exit;
      }
/*
***Gör den anropade modulen aktiv och leta upp den aktuella
***parametern.
*/
    pmsbla(newmod);
    pmrpap((pm_ptr)0);

    for ( i=0; i<pnum; ++i ) pmgpad(&panola);
/*
***Läs lite data om parametern och kolla vilken typ den har.
***ftype = formell typ.
*/
    pmrpar(panola,parnam,pmt,&defval);
    ftype.kind_ty = defval.lit_type;
/*
***Gör aktiv modul aktiv igen
*/
    pmsbla(actmod);
/*
***Analysera det nya uttrycket.
*/
      anlogi();
      if ( (status=asinit(proc_pv[3].par_va.lit.str_va,ANRDSTR)) < 0 ) goto exit;
      ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
      anascan(&sy);
      anarex(&newexp,&attr,&set);
      if ( (status=asexit()) < 0 ) goto exit;
/*
***Blev det några fel ?
*/
      if ( anyerr() )
        {
        erpush("IG3902",proc_pv[3].par_va.lit.str_va);
        status = erpush("IG4022","");
        goto exit;
        }
/*
***Prova att interpretera uttrycket.
*/
      if ( inevex(newexp,&valp,&valt) < 0 )
        {
        erpush("IG3902",proc_pv[3].par_va.lit.str_va);
        status = erpush("IG4022","");
        goto exit;
        }
/*
***Jämför typerna. atype = aktuell typ. ftype = formell typ.
*/
      strtyp(valt,&atype);

      if ( atype.kind_ty != ftype.kind_ty )
        {
        if ( !(ftype.kind_ty == ST_FLOAT  &&  atype.kind_ty == ST_INT) )
          {
          status = erpush("IG4032","");
          goto exit;
          }
        }
/*
***Framåt-referens ?
*/
      if ( sysmode & GENERIC  &&  pmarex(id,newexp) )
        {
        status = erpush("IG3882",proc_pv[3].par_va.lit.str_va);
        goto exit;
        }
/*
***Inga fel, byt ut uttrycket. plstla är gamla uttryckets list-nod.
*/
      lnp = (PMLINO *)pmgadr(plstla);
      lnp->p_no_li = newexp;
/*
***Hur mycket reevaluering som skall göras beror på mode.
***remode = 0 => Fullständig, inkl. kör aktiv om refererad. Default.
***remode = 1 => Bara inkrementell reinterpretering.
***remode = 2 => Ingen reinterpretering.
*/
      remode = proc_pv[4].par_va.lit.int_va;

      if ( remode < 0  ||  remode > 2 ) remode = 0;

/*
***I ritmodulen ändrar vi parametervärdet direkt i GM
***när remode = 2. Då måste vi också hantera typ-
***konvertering här.
*/
      if ( sysmode == EXPLICIT  &&  remode == 2 )
        {
        if ( ftype.kind_ty == ST_FLOAT  &&  atype.kind_ty == ST_INT )
          valp.lit.float_va = (v2float)valp.lit.int_va;
        DBupdate_part_parameter(&part,pnum,&valp);
        }
/*
***Oavsett om storheten är refererad elle inte provar vi med
***inkrementell interpretering.
*/
      if ( remode < 2 )
        {
        insrtb(inglrb());
        oldmty = modtyp; pmrmod(&modhed); modtyp = modhed.mtype;
        status = EXrist(id); 
        modtyp = oldmty;
        insrtb(oldrtb);
/*
***Om det inte gick bra måste vi återställa efter oss även
***i PM. EXrist() återställer automatiskt i GM och GP !
*/
        if ( status < 0 ) 
          {
          lnp->p_no_li = oldexp;
          status = erpush("IG4042","");
          goto exit;
          }
/*
***Om storheten är refererad kanske vi ska köra hela modulen ?
***Isåfall kan den globala variabeln modtyp behöva sättas.
*/
        if ( remode == 0  &&  sysmode & GENERIC  &&  pmamir(id)  &&
                                               IGialt(175,67,68,FALSE) )
          {
          insrtb(oldrsp);
          oldmty = modtyp; pmrmod(&modhed); modtyp = modhed.mtype;
          status = IGream();
          modtyp = oldmty;
          insrtb(oldrtb);
          insrsp(oldrsp);
          }
        }
      break;
/*
***Ej part-sats.
*/
      default:
      IGidst(id,errbuf);
      status = erpush("IG4002",errbuf);
      goto exit;
      }
/*
***Slut. Ta bort ev. skräp i PM och gör rätt modul 
***aktiv igen.
*/
exit:
    if ( dstflg ) pmdlst();
    pmgeba(curmna,&oldmod);
    pmsbla(oldmod);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evclpm()

/*      Evaluerar CLEAR_PM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-03-26 J. Kjellander
 *
 ******************************************************!*/

  {
   char     curmna[V2SYNLEN+1];
   pm_ptr   base;
   PMMONO  *np;
/*
***Vad heter nuvarande modul.
*/
   pmgmod((pm_ptr)0,&np);
   strcpy(curmna,pmgadr(np->mona_));
/*
***Töm heapen.
*/
   clheap();
/*
***Om den modul som nu exekverar är anropad
***måste den in i PM igen. Den hamnar då med
***största sannolikhet på en annan adress och
***PM:s basdress måste därför uppdateras.
*/
   if ( pmgbla() != pmgaba() )
     {
     pmgeba(curmna,&base);
     pmsbla(base);
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evposm()

/*      Evaluerar POS_MBS. Parameter promt-stängen.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:   
 *
 *      (C)microform ab 1997-04-21 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short    status,oldmty,oldtmp;
    pm_ptr   oldmod,exprla;
    PMMONO  *np;

/*
***Ställ om systemet.
*/
    oldmod = pmgbla();
    pmsbla(actmod);

    pmgmod((pm_ptr)0,&np);
    oldmty = modtyp;
    modtyp = np->moty_;

    pmmark();

    oldtmp = tmpref;
    tmpref = FALSE;
/*
***Generera positionsuttryck.
*/
    WPaddmess_mcwin(func_pv[1].par_va.lit.str_va,WP_PROMPT);
    status = IGcpos(0,&exprla);
    WPclear_mcwin();
    if ( status < 0 )
      {
      func_vp->lit.str_va[0] = '\0';
      status = 0;
      goto exit;
      }
/*
***Dekompilera resultatet.
*/
    status = pprexs(exprla,np->moty_,func_vp->lit.str_va,V3STRLEN-1);
/*
***Slut.
*/
exit:
    modtyp = oldmty;
    pmrele();
    tmpref = oldtmp;
    pmsbla(oldmod);
    return(status);
  }

/********************************************************/
