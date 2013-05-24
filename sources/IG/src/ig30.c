/*!******************************************************************/
/*  File: ig30.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   igedit();   Edit entity/part                                   */
/*   iguppt();   Uppdate Part                                       */
/*   igcptp();   Edit part, VT100 version                           */
/*   igcptw();   Edit part, window verion                           */
/*   igcpen();   Edit pen                                           */
/*   igcniv();   Edit level                                         */
/*   igcwdt();   Edit linewidth                                     */
/*   iggnps();   Build part call                                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../AN/include/AN.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include "../include/screen.h"
#include <string.h>

extern pm_ptr  actmod;
extern DBptr   lsysla;
extern short   actfun,igtrty,v3mode,modtyp,posmod;
extern char    actpnm[];
extern V2NAPA  defnap;
extern struct  ANSYREC sy;

/*!******************************************************/

       short igedit()

/*     Varkonfunktion för att ändra storhet/part. f170.
 *
 *
 *      (C)microform ab 1998-09-17 J. Kjellander
 *
 ******************************************************!*/

  {
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short iguppt()

/*      Varkonfunktion för att uppdatera en part.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5222  => Fel vid interpr. av PART-sats
 *               IG5332  => Kan ej hitta PART-sats i PM
 *
 *      (C)microform ab 27/10/88 R. Svedin
 *
 *      1/12/88  Ritpaketet, J. Kjellander
 *      3/5/89   Bug REJECT, J. Kjellander
 *      30/3/92  Ny re-interpretering, J. Kjellander
 *      20/5/94  Bug DBread_part_attributes()-1.10, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype    typ;
    bool     end,right,dstflg;
    short    status;
    pm_ptr   retla,stlla,dummy;
    PMREFVA  idvek[MXINIV];
    GMPRT    part;
    GMPDAT   pdat;
    PMMONO  *np;
    V2NAPA   oldnap;

/*
***Initiering.
*/
    dstflg = FALSE;
/*
***Ta reda på parten:s ID.
*/
    igptma(269,IG_MESS);
    typ = PRTTYP;
    status=getidt(idvek,&typ,&end,&right,(short)0);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Om ritpaket, skapa partanrop.
*/
    if ( v3mode == RIT_MOD )
      {
      if ( (status=iggnps(idvek)) < 0 ) goto error;
      else dstflg = TRUE;
      }
/*
*** Hämta PM-pekare till partsatsen.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0 );
    stlla = np->pstl_;

    if ( pmlges(idvek, &stlla, &retla) < 0 || retla == (pm_ptr)NULL)
      {
      erpush("IG5332","");
      goto error;
      }

    pmglin(retla, &dummy, &retla);
/*
***Kör om anropet.
*/
    V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
    EXgtpt(idvek,&part);
    DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
/*
***Om det handlar om en mycket gammal part, 1.10 etc. kanske den
***inte har några attribut lagrade !!!
*/
    if ( part.dts_pt == sizeof(GMPDAT)  &&  pdat.attp_pd > DBNULL )
      DBread_part_attributes((char *)&defnap,(int)pdat.asiz_pd,pdat.attp_pd);

    EXdraw(idvek,FALSE);
    gmmtm((DBseqnum)idvek->seq_val);
    status = inssta(retla); 
    V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

    if ( status < 0 ) 
      {
      erpush("IG5222","PART");
      errmes();
      EXdraw(idvek,FALSE);
      gmumtm();
      EXdraw(idvek,TRUE);
      }
/*
***Avslutning.
*/
exit:
    if ( dstflg ) pmdlst();
    gphgal(0);
    return(status);
/*
***Felutgång.
*/
error:
    errmes();
    goto exit;
  }

/********************************************************/
/*!******************************************************/

        short igcptp()

/*      Varkonfunktion för att ändra en parameter i
 *      ett part-anrop.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5272  => Kan ej ladda modulen %s
 *               IG2192  => Parametern %s finns ej
 *               IG5312  => Parten är refererad
 *               IG5302  => Parametern %s av otillåten typ
 *               IG5222  => Fel vid interpr. av PART-sats
 *               IG5322  => Kan ej ändra PART-sats
 *
 *      (C)microform ab 10/3/86 J. Kjellander
 *
 *      23/3/86  genpos(pnr, B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      24/3/86  Bug, J. Kjellander
 *      26/5/86  Listning av parametrar, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *      6/10/86  Bug strängländer, J. Kjellander
 *      7/10/86  REF, J. Kjellander
 *      10/10/86 igrsma(), J. Kjellander
 *      11/11/86 Refererad part, J. Kjellander
 *      30/9/87  Listning av z, J. Kjellander
 *      17/3/88  Penna och nivå, J. Kjellander
 *      18/3/88  Listning av parametrar, J. Kjellander
 *      11/10/88 Bug, listning J. Kjellander
 *      1/12/88  Ritpaketet, J. Kjellander
 *      19/9/89  dstflg, J. Kjelander
 *      3/10/91  Strippat promt, J. Kjellander
 *      17/2/92  Framåt-referenser, J. Kjellander
 *      17/2/92  REF_parametrar, J. Kjellander
 *      17/2/92  referensräknaren, J. Kjellander
 *      26/9/93  Formatkod flyttal, J. Kjellander
 *      14/11/93 EXIT med felmeddelande, J. Kjellander
 *      20/5/94  Bug DBread_part_attributes()-1.10, J. Kjellander
 *
 ******************************************************!*/

  {
    char    linbuf[81];          /* line output buffer */
    char    typbuf[10];          /* sprintf() type buffer */
    char    valbuf[V3STRLEN];    /* sprintf() value buffer */
    char    pmtbuf[V3STRLEN];    /* sprintf() prompt buffer */
    char    scrbuf1[V3STRLEN];   /* sprintf() scratch buffer */
    char    scrbuf2[V3STRLEN];   /* sprintf() scratch buffer */
    char    errbuf[81];
    DBetype   typ;
    bool    end,right,edit,ref,dstflg;
    pm_ptr  oldmod,newmod,panola,exnpt,retla;
    char    parnam[V3STRLEN+1];
    char    tmpnam[V3STRLEN+1];
    char    prompt[V3STRLEN+1];
    char    dstr[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    short   pnum,status,mnum,oldpmd,posalt,pant;
    pm_ptr  pla[V2MPMX];
    PMREFVA idvek[MXINIV];
    GMPRT   part;
    GMPDAT  pdat;
    PMLITVA defval;
    V2NAPA  oldnap;

/*
***Initiering.
*/
    edit = dstflg = FALSE;
/*
***Ta reda på parten:s ID.
*/
    igptma(269,IG_MESS);
    typ = PRTTYP;
    status=getidt(idvek,&typ,&end,&right,(short)0);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Om ritpaket, skapa partanrop.
*/
    if ( v3mode == RIT_MOD )
      {
      if ( (status=iggnps(idvek)) < 0 ) goto error1;
      else dstflg = TRUE;
      }
/*
***Notera nuvarande parametrars PM-adresser.
*/
    pmgpla(idvek,&pant,pla);
/*
***Läs part-posten och ladda in den modul som skapat parten.
*/
    EXgtpt(idvek,&part);
    if ( v3mode & BAS_MOD  &&  pmamir(idvek) ) ref = TRUE;
    else  ref = FALSE;

    oldmod = pmgbla();
    if ( pmgeba(part.name_pt,&newmod) != 0 )
      {
      erpush("IG5272",part.name_pt);
      goto error1;
      }
/*
***Lista parametrar.
*/
    if ( igialt(120,67,68,FALSE) )
      {
      pmsbla(newmod);
      pmrpap((pm_ptr)0);
      iginla(iggtts(TS_PLHEAD));

      for ( pnum=1;; ++pnum)
        {
        pmgpad(&panola);
        if ( panola == (pm_ptr)NULL )
          {
          pmsbla(oldmod);
          if ( (status=igexla()) < 0 ) goto exit;
          else goto loop;
          }
/*
***Läs parameterns namn och prompt.
*/
       pmrpar(panola,parnam,prompt,&defval);

       switch(defval.lit_type)
         {
         case C_INT_VA:
         sprintf(typbuf,"%-9s",iggtts(TS_INT));
         EXgint(idvek,pnum,&defval);
         sprintf(valbuf,"%-25d",defval.lit.int_va);
         break;
 
         case C_FLO_VA:
         sprintf(typbuf,"%-9s",iggtts(TS_FLOAT));
         EXgflt(idvek,pnum,&defval);
         sprintf(valbuf,"%-25.15g",defval.lit.float_va);
         break;
  
         case C_STR_VA:
         sprintf(typbuf,"%-9s",iggtts(TS_STR));
         EXgstr(idvek,pnum,&defval);
         sprintf(valbuf,"%-25s",defval.lit.str_va);
         break;

         case C_VEC_VA:
         sprintf(typbuf,"%-9s",iggtts(TS_VEC));
         EXgvec(idvek,pnum,&defval);
         sprintf(scrbuf1,"(%g",defval.lit.vec_va.x_val);
         sprintf(scrbuf2,", %g",defval.lit.vec_va.y_val);
         strcat(scrbuf1,scrbuf2);
         if ( modtyp == 3 )
           {
           sprintf(scrbuf2,", %g",defval.lit.vec_va.z_val);
           strcat(scrbuf1,scrbuf2);
           }
         strcat(scrbuf1,")");
         sprintf(valbuf,"%-25s",scrbuf1);
         break;

         case C_REF_VA:
         sprintf(typbuf,"%-9s",iggtts(48));
         EXgref(idvek,pnum,&defval);
         igidst(&defval.lit.ref_va[0],scrbuf1);
         sprintf(valbuf,"%-25s",scrbuf1);
         }
/*
***Kopiera ihop strängarna till en enda sträng.
*/
       sprintf(linbuf,"%-15s",parnam);
       linbuf[15] = '\0';
       typbuf[9] = '\0';
       strcat(linbuf,typbuf);
       valbuf[25] = '\0';
       strcat(linbuf,valbuf);
       sprintf(pmtbuf,"%-30s",prompt);
       pmtbuf[30] = '\0';
       strcat(linbuf,pmtbuf);
/*
***Skriv ut.
*/
       if ( (status=igalla(linbuf,2)) == GOMAIN )
         {
         pmsbla(oldmod);
         goto exit;
         }
       }
     }
/*
***Ändra en parameter;
*/
loop:
/*
***Läs in parameternamn.
*/
    igptma(228,IG_INP);
    status=igssip(iggtts(267),parnam,"",80);
    igrsma();
    if ( status < 0 ) goto endpar;
/*
***Sätt basadress = anropad modul och leta upp parametern.
*/
    pmsbla(newmod);

    pmrpap((pm_ptr)0);
    for ( pnum=1;; ++pnum)
      {
      pmgpad(&panola);
/*
***Parametern finns ej.
*/
      if (panola == (pm_ptr)NULL)
        {
        igrsma();
        pmsbla(oldmod);
        erpush("IG2192",parnam);
        errmes();
        goto loop;
        }
      pmrpar(panola,tmpnam,prompt,&defval);
      if(sticmp(tmpnam,parnam) == 0) break;
      }

    pmsbla(oldmod);
/*
***Parametern har hittats, strippa promten på krull-
***alfa-varianter och läs in nytt värde.
*/
    igoptp(prompt);
    igdefp(prompt,&defval);

    switch ( defval.lit_type )
      {
      case C_INT_VA:
      igplma(prompt,IG_INP);
      EXgint(idvek,pnum,&defval);
      sprintf(dstr,"%d",defval.lit.int_va);
      status=genint(0,dstr,istr,&exnpt);
      break;

      case C_FLO_VA: 
      igplma(prompt,IG_INP);
      EXgflt(idvek,pnum,&defval);
      sprintf(dstr,"%.15g",defval.lit.float_va);
      status=genflt(0,dstr,istr,&exnpt);
      break;

      case C_STR_VA:
      if ( igmenp(prompt,&mnum) )
        {
        igplma(prompt,IG_MESS);
        status = genstm(mnum,&exnpt);
        }
      else
        {
        igplma(prompt,IG_INP);
        EXgstr(idvek,pnum,&defval);
        status=genstr(0,defval.lit.str_va,istr,&exnpt);
        }
      break;

      case C_VEC_VA:
      oldpmd = posmod;
      if ( igposp(prompt,&posalt) ) posmod = posalt;
      igplma(prompt,IG_MESS);
      status=genpos(0,&exnpt);
      posmod = oldpmd;
      break;

      case C_REF_VA:
      if ( !igtypp(prompt,&typ) ) typ = ALLTYP;
      igplma(prompt,IG_MESS);
      status=genref(0,&typ,&exnpt,&end,&right);
      break;

      default:
      erpush("IG5302",parnam);
      igrsma();
      goto error1;
      }
    igrsma();
    if ( status == GOMAIN ) return(GOMAIN);
    else if ( status < 0 ) goto loop;
/*
***Kolla om det nya parametervärdet innebär framåt-referenser.
*/
    if ( v3mode & BAS_MOD  &&  pmarex(idvek,exnpt) == TRUE )
      {
      pprexs(exnpt,modtyp,errbuf,80);
      erpush("IG3882",errbuf);
      errmes();
      goto loop;
      }
/*
***Byt parametern i part-anropet och fråga efter nästa parameter.
*/
    if ( pmchpa(idvek,pnum,exnpt,&retla) < 0 || retla == (pm_ptr)NULL)
      {
      erpush("IG5322","");
      goto error1;
      }
    edit = TRUE;
    goto loop;
/*
***Om refererad, fråga om hela modulen skall köras. Annars
***kör bara om part-anropet.
*/
endpar:
    if ( edit )
      {
      if ( ref && igialt(373,67,68,FALSE) ) igramo();
/*
***Reinterpretera inkrementellt.
*/
      else
        {
        V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
        DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
/*
   Ändrat villkoret 19/5 pga. problem hos Kungshus !!!
        if ( pdat.asiz_pd > 0  &&  pdat.attp_pd > NULL )
*/
        if ( part.dts_pt == sizeof(GMPDAT)  &&  pdat.attp_pd > DBNULL )
          DBread_part_attributes((char *)&defnap,(int)pdat.asiz_pd,pdat.attp_pd);
        EXdraw(idvek,FALSE);
        gmmtm((DBseqnum)idvek->seq_val);
        status = inssta(retla); 
        V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

        if ( status < 0  ||  status == 3 ) 
          {
          if ( status < 0 )
            {
            erpush("IG5222","PART");
            errmes();
            }
          EXdraw(idvek,FALSE);
          gmumtm();
          EXdraw(idvek,TRUE);
          if ( v3mode & BAS_MOD ) for ( pnum=0; pnum<pant; ++pnum)
            pmchpa(idvek,(short)(pnum+1),pla[pnum],&retla);
          }
        }
      }
/*
***Avslutning.
*/
exit:
    if ( dstflg ) pmdlst();
    gphgal(0);

    return(status);
/*
***Felutgångar.
*/
error1:
    errmes();
    goto exit;
  }

/********************************************************/
/*!******************************************************/

        short igcptw()

/*      Fönstervariant av 'Ändra part' gemensam för X11
 *      och Windows.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5272  => Kan ej ladda modulen %s
 *               IG5222  => Fel vid interpr. av PART-sats
 *               IG5322  => Kan ej ändra PART-sats
 *               IG3982  => Fel vid analysering av parameter
 *               IG4072  => Parten har inga parametrar
 *
 *      (C)microform ab 2/9/92 J. Kjellander
 *
 *      20/5/94 Bug DBread_part_attributes()-1.10, J. Kjellander
 *      13/2/95 ALLTYP, J. Kjellander
 *      1996-04-30 Noll parametrar, J. Kjellander
 *      1996-12-12 Flyttad till igepac, J.Kjellander
 *      1998-09-17 t/@ i promtsträngar, J.Kjellander
 *      1998-09-25 actfun, J.Kjellander
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *
 ******************************************************!*/

  {
    int     typarr[V2MPMX];                 /* Parametertyper */
    char    oldval[V2MPMX][V3STRLEN];       /* Gamla värden */
    char   *osparr[V2MPMX];         
    char    newval[V2MPMX][V3STRLEN];       /* Nya värden */
    char   *nsparr[V2MPMX];         
    char    pmtarr[V2MPMX][V3STRLEN+1];     /* Promptar */
    char   *pmparr[V2MPMX];         
    short   maxtkn[V2MPMX];                 /* Stränglängder */
    short   pant;                           /* Antal parametrar */
    short   i,status;
    char    parnam[V3STRLEN+1];
    char    rubrik[V3STRLEN],errbuf[V3STRLEN];
    pm_ptr  pla[V2MPMX];                    /* Parametrars PM-adress */
    pm_ptr  oldmod,newmod,panola,exnpt,retla;
    DBetype   typ;
    bool    end,right,edit,ref,dstflg;
    short   oldafn;
    ANFSET  set;
    ANATTR  attr;
    GMPRT   part;
    PMREFVA idvek[MXINIV];
    GMPDAT  pdat;
    PMLITVA defval;
    V2NAPA  oldnap;

/*
***Denna funktion får bara anropas med X11 eller Windows
***som terminaltyp.
*/
    if ( igtrty != X11  &&  igtrty != MSWIN ) return(0);
/*
***Initiering.
*/
    edit = dstflg = FALSE;
    pmmark();
/*
***Ta reda på parten:s ID.
*/
    igptma(269,IG_MESS);
    typ = PRTTYP;
    status=getidt(idvek,&typ,&end,&right,(short)0);
    igrsma();

    if ( status < 0 )
      goto exit;
    else
      {
      idvek[0].ord_val = 1;
      idvek[0].p_nextre = NULL;
      }
/*
***Läs part-posten.
*/
    if ( EXgtpt(idvek,&part) < 0 ) goto error1;
/*
***Om ritpaket, skapa partanrop.
*/
    if ( v3mode == RIT_MOD )
      {
      if ( (status=iggnps(idvek)) < 0 ) goto error1;
      else dstflg = TRUE;
      }
/*
***Notera nuvarande parametrars antal och PM-adresser.
*/
    pmgpla(idvek,&pant,pla);
/*
***Om parametrar saknas slutar vi här.
*/
   if ( pant == 0 )
     {
     erpush("IG4072",part.name_pt);
     goto error1;
     }
/*
***Dekompilera de aktuella parametrarnas värde till strängar
***så att lämpliga värden kan presenteras i inmatningsfälten.
*/
    for ( i=0; i<pant; ++i )
      {
      maxtkn[i] = V3STRLEN;
      pprexs(pla[i],modtyp,oldval[i],maxtkn[i]);
      }
/*
***Är parten refererad ?
*/
    if ( v3mode & BAS_MOD  &&  pmamir(idvek) ) ref = TRUE;
    else  ref = FALSE;
/*
***Ladda in modulen.
*/
    oldmod = pmgbla();
    if ( pmgeba(part.name_pt,&newmod) != 0 )
      {
      erpush("IG5272",part.name_pt);
      goto error1;
      }
/*
***Gör den anropade modulen aktiv och plocka fram promtar
***och typer.
*/
    pmsbla(newmod);
    pmrpap((pm_ptr)0);

    for ( i=0; i<pant; ++i )
      {
      pmgpad(&panola);
/*
***Initiera pekare till strängar för senare indata till wpmsip().
*/
      osparr[i] = oldval[i];
      nsparr[i] = newval[i];
      pmparr[i] = pmtarr[i];
/*
***Läs parameterns namn, prompt och defaultvärde.
*/
      pmrpar(panola,parnam,pmtarr[i],&defval);
/*
***Mappa ev. t-sträng till klartext och strippa
***ev. inledande "@ " eller om det är 2 @ strippa
***det 1:a.
*/
      if ( strlen(pmtarr[i]) > 0 )
        {
        igtstp(pmtarr[i]);
        igoptp(pmtarr[i]);
        }
/*
***Parameter utan promt är gömd.
*/
     else strcpy(pmtarr[i],iggtts(1627));
/*
***Vilken typ har den ?
*/
      typarr[i] = defval.lit_type;
/*
***Lägg till typ och namn efter promten.
*/
      switch(defval.lit_type)
        {
        case C_INT_VA: strcat(pmtarr[i],"  (INT "); break;
        case C_FLO_VA: strcat(pmtarr[i],"  (FLOAT "); break;
        case C_STR_VA: strcat(pmtarr[i],"  (STRING "); break;
        case C_VEC_VA: strcat(pmtarr[i],"  (VECTOR "); break;
        case C_REF_VA: strcat(pmtarr[i],"  (REF "); break;
        }
      strcat(pmtarr[i],parnam);
      strcat(pmtarr[i],")");
      }

    pmsbla(oldmod);
/*
***Låt hjälpsystemet få veta vad vi gör.
*/
    oldafn = actfun;
    actfun = -2;
    strcpy(actpnm,part.name_pt);
/*
***Anropa wpmsip. t1599 = "Parametrar för parten : "
*/
    strcpy(rubrik,iggtts(1599));
    strcat(rubrik,part.name_pt);
#ifdef V3_X11
    status = wpmsip(rubrik,pmparr,osparr,nsparr,maxtkn,typarr,pant);
#endif
#ifdef WIN32
    status = (short)msmsip(rubrik,pmparr,osparr,nsparr,maxtkn,typarr,pant);
#endif

    actfun = oldafn;

    if ( status < 0 ) goto exit;
/*
***Gå igenom de nya strängarna och byt ut alla parametrar i
***anropet som har ändrats.
*/
    for ( i=0; i<pant; ++i )
      {
      if ( strcmp(nsparr[i],osparr[i]) != 0 )
        {
/*
***Parametern "i" har ändrats ! Analysera den nya MBS-strängen.
*/
        anlogi();
        if ( (status=asinit(nsparr[i],ANRDSTR)) < 0 ) goto exit;
        ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
        anascan(&sy);
        anarex(&exnpt,&attr,&set);
        if ( (status=asexit()) < 0 ) goto exit;
/*
***Hur gick det.
*/ 
        if ( anyerr() )
          {
          sprintf(errbuf,"%d%%%s",i+1,nsparr[i]);
          erpush("IG3982",errbuf);
          goto error1;
          }
/*
***Kolla om det nya parametervärdet innebär framåt-referenser.
*/
        if ( v3mode & BAS_MOD  &&  pmarex(idvek,exnpt) == TRUE )
          {
          erpush("IG3882",nsparr[i]);
          goto error1;
          }
/*
***Byt parametern i part-anropet.
*/
        if ( pmchpa(idvek,(short)(i+1),exnpt,&retla) < 0 || retla == (pm_ptr)NULL )
          {
          erpush("IG5322","");
          goto error1;
          }
        edit = TRUE;
        }
      }
/*
***Om refererad, fråga om hela modulen skall köras. Annars
***kör bara om part-anropet.
*/
    if ( edit )
      {
      if ( ref && igialt(373,67,68,FALSE) ) igramo();
/*
***Reinterpretera inkrementellt.
*/
      else
        {
        V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
        DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
        if ( part.dts_pt == sizeof(GMPDAT)  &&  pdat.attp_pd > DBNULL )
          DBread_part_attributes((char *)&defnap,(int)pdat.asiz_pd,pdat.attp_pd);
        EXdraw(idvek,FALSE);
        gmmtm((DBseqnum)idvek->seq_val);
#ifdef V3_X11
        wpwait(GWIN_MAIN,TRUE);
#endif
        status = inssta(retla); 
#ifdef V3_X11
        wpwait(GWIN_MAIN,FALSE);
#endif
        V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

        if ( status < 0  ||  status == 3 ) 
          {
          if ( status < 0 )
            {
            erpush("IG5222","PART");
            errmes();
            }
          EXdraw(idvek,FALSE);
          gmumtm();
          EXdraw(idvek,TRUE);
          if ( v3mode & BAS_MOD ) for ( i=0; i<pant; ++i )
            pmchpa(idvek,(short)(i+1),pla[i],&retla);
          }
        }
      }
/*
***Avslutning.
*/
exit:
    if ( dstflg ) pmdlst();
    if ( v3mode == RIT_MOD ) pmrele();
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error1:
    errmes();
    goto exit;
  }

/********************************************************/
/*!******************************************************/

        short igcpen()

/*      Varkonfunktion för att ändra en storhets penn-
 *      nummer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3562 = Storheten ingår i en part
 *              IG3702 = Otillåtet pennummer
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingår i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 getmid(), J. Kjellander
 *
 ******************************************************!*/

  {
    short   status,i,nid;
    DBetype   typ,typvek[IGMAXID];
    DBptr   la;
    pm_ptr  exnpt,retla,valtyp;
    char    istr[V3STRLEN+1];
    PMREFVA idmat[IGMAXID][MXINIV];
    GMRECH  hed;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storheternas:s ID.
*/
loop:
    igptma(150,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          gphgal(0);
          goto loop;
         }
      }
/*
***Fråga efter nytt pennummer.
*/
    pmmark();
    if ( (status=genint(16,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Kolla att värdet är tillåtet.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > 32767 )
      {
      erpush("IG3702","");
      errmes();
      gphgal(0);
      goto loop;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra pennummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMPEN,exnpt,&retla);
/*
***Under alla omständigheter, ändra pennummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.pen != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.pen = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        }
      }
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcniv()

/*      Varkonfunktion för att ändra en storhets nivå.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3212 = Otillåtet nivånummer
 *              IG3562 = Någon av storh. ingår ipart
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingår i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 getmid(), J. Kjellander
 *      1998-01-12 Aktivt csys, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,i,nid;
    DBetype   typ,typvek[IGMAXID];
    DBptr   la;
    char    istr[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idmat[IGMAXID][MXINIV];
    GMRECH  hed;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Ta reda på storhetens:s ID.
*/
loop:
    igptma(151,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          gphgal(0);
          goto loop;
         }
      }
/*
***Fråga efter nytt nivånummer.
*/
    pmmark();
    if ( (status=genint(227,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Kolla att värdet är tillåtet.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > NT1SIZ-1 )
      {
      erpush("IG3212","");
      errmes();
      gphgal(0);
      goto loop;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra nivånummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMLEVEL,exnpt,&retla);
/*
***Ändra nivånummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.level != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.level = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        if ( la == lsysla ) igupcs(lsysla,V3_CS_ACTIVE);
        }
      }
/*
***Om igen.
*/
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcwdt()

/*      Varkonfunktion för att ändra en storhets 
 *      linjebredd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3562 = Storheten ingår i en part
 *              IG3702 = Otillåtet pennummer
 *
 *      (C)microform ab 1998-11-25 J. Kjellander
 *
 *      1999-03-09, Bugfix pen, J.Kjellander
 *      2004-07-27 B_plane, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    short    status,i,nid;
    DBetype  typ,typvek[IGMAXID];
    DBptr    la;
    gmflt    width;
    pm_ptr   exnpt,retla,valtyp;
    char     istr[V3STRLEN+1];
    GMPOI    poi;
    GMLIN    lin;
    GMARC    arc;
    GMCUR    cur;
    GMTXT    txt;
    DBBplane bpl;
    PMREFVA  idmat[IGMAXID][MXINIV];
    GMRECH   hed;
    PMLITVA  val;

    static char dstr[V3STRLEN+1] = "0";

/*
***Ta reda på storheternas:s ID.
*/
loop:
    igptma(1633,IG_MESS);
    typvek[0] = POITYP+LINTYP+ARCTYP+CURTYP+TXTTYP+BPLTYP;
    nid = IGMAXID;
    if ( (status=getmid(idmat,typvek,&nid)) < 0 ) goto exit;
    igrsma();
/*
***Om basmodul, kolla att ingen av storheterna ingår i en part.
*/
    if ( v3mode & BAS_MOD )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          gphgal(0);
          goto loop;
         }
      }
/*
***Fråga efter ny linjebredd.
*/
askwdt:
    pmmark();
    if ( (status=genflt(1632,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
    igrsma();
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) width = (gmflt)val.lit.int_va;
    else width = val.lit.float_va;
/*
***Kolla att värdet är tillåtet.
*/
    if ( width < 0.0 )
      {
      pmrele();
      erpush("IG3872","");
      errmes();
      goto askwdt;
      }
/*
***Ändra alla storheterna.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***Om basmodul, ändra pennummer i PM.
*/
      if ( v3mode & BAS_MOD ) pmchnp(idmat[i],PMWIDTH,exnpt,&retla);
/*
***Under alla omständigheter, ändra pennummer i GM och GP.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      EXdren(la,typ,FALSE,GWIN_ALL);

      if ( typ == POITYP )
        {
        DBread_point(&poi,la);
        poi.wdt_p = width;
        DBupdate_point(&poi,la);
        }
      else if ( typ == LINTYP )
        {
        DBread_line(&lin,la);
        lin.wdt_l = width;
        DBupdate_line(&lin,la);
        }
      else if ( typ == ARCTYP )
        {
        DBread_arc(&arc,NULL,la);
        arc.wdt_a = width;
        DBupdate_arc(&arc,NULL,la);
        }
      else if ( typ == CURTYP )
        {
        DBread_curve(&cur,NULL,NULL,la);
        cur.wdt_cu = width;
        DBupdate_curve(&cur,NULL,la);
        }
      else if ( typ == TXTTYP )
        {
        DBread_text(&txt,NULL,la);
        txt.wdt_tx = width;
        DBupdate_text(&txt,NULL,la);
        }
      else if ( typ == BPLTYP )
        {
        DBread_bplane(&bpl,la);
        bpl.wdt_bp = width;
        DBupdate_bplane(&bpl,la);
        }


      EXdren(la,typ,TRUE,GWIN_ALL);
      }
/*
***Avslutning.
*/
exit:
    gphgal(0);
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short iggnps(PMREFVA *id)

/*    Genererar en part-anropssats med utseende enl.
 *    part-post i GM.
 *
 *      In: id  => Partens ID.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5023 = Kan ej skapa PART sats 
 *              IG5342 = Någon parameter är call by reference
 *
 *      (C)microform ab 29/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short    prtid;               /* Partens sekvensnummer */
    pm_ptr   dummy;               /* Som det låter */
    pm_ptr   oblpar;              /* Obligatoriska parametrar */
    pm_ptr   parlst;              /* Parameterlista */
    pm_ptr   exnpt;               /* Pekare till expr. node */
    pm_ptr   retla;               /* Pekare till den skapade satsen */
    short    status;              /* Anropade rutiners status */
    short    i;                   /* Loop-variabel */
    PMLITVA  litval;              /* Litterärt värde */
    GMPRT    part;                /* Part-posten */
    GMPDAT   data;                /* Part-data */
    PMPATLOG typarr[V2MPMX];      /* Parametertyper */

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
    DBread_part_parameters(&data,typarr,NULL,part.dtp_pt,part.dts_pt);
/*
***Ta reda på modulens attribut LOCAL/GLOBAL, om LOCAL,
***skapa referens till lokalt koordinatsystem.
*/
    if ( data.matt_pd  == LOCAL )
        {
        litval.lit_type = C_REF_VA;
        DBget_id(data.csp_pd,&litval.lit.ref_va[0]);
        pmclie(&litval,&exnpt);
        pmtcon(exnpt,(pm_ptr)NULL,&oblpar,&dummy);
        }
     else oblpar = (pm_ptr)NULL;
/*
***Skapa parameterlistan.
*/
    parlst = (pm_ptr)NULL;

    for( i=0; i<data.npar_pd; ++i)
      {
      switch ( typarr[i].log_id )
        {
        case C_INT_VA:
        litval.lit_type = C_INT_VA;
        if ( (status=EXgint(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_FLO_VA: 
        litval.lit_type = C_FLO_VA;
        if ( (status=EXgflt(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_STR_VA:
        litval.lit_type = C_STR_VA;
        if ( (status=EXgstr(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_VEC_VA:
        litval.lit_type = C_VEC_VA;
        if ( (status=EXgvec(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_REF_VA:
        litval.lit_type = C_REF_VA;
        if ( (status=EXgref(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        default:
        return(erpush("IG5342",""));
        }

      pmclie(&litval,&exnpt);
      pmtcon(exnpt,parlst,&parlst,&dummy);
      }
/*
***Skapa partsats.
*/
    prtid = id[0].seq_val;

    if ( pmcpas(prtid,part.name_pt,(pm_ptr)NULL,parlst,
                oblpar,(pm_ptr)NULL,&retla) != 0 )
      return(erpush("IG5023","PART"));
    else
      pmlmst(actmod,retla);

    return(0);

  }

/********************************************************/
