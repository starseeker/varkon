/*!******************************************************************/
/*  File: ig11.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  modbpm();    Genererate mode_basic.... statement                */
/*  modgpm();    Genererate mode_global.... statement               */
/*  modlpm();    Genererate mode_local.... statement                */
/*  igcges();    Generic create geometrical statement               */
/*  igcprs();    Generic create procedure call statement            */
/*  igcpts();    Create part statement                              */
/*  iggnid();    Generate new ID sequencenumber                     */
/*                                                                  */
/*  igoptp();    Check for optional parameter                       */
/*  igmenp();    Check for menu parameter                           */
/*  igposp();    Check for pos parameter                            */
/*  igtypp();    Check for type parameter                           */
/*  igdefp();    Check for default parameter                        */
/*  igtstp();    Maps t-string to prompt                            */
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
#include "../../GP/include/GP.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern pm_ptr   actmod;
extern DBptr    lsysla;
extern DBseqnum snrmax;
extern short    actfun,v3mode,posmod,gptrty;
extern char     jobdir[],jobnam[],actpnm[];
extern bool     tmpref,iggflg;
extern DBTmat  *lsyspk;
extern V2NAPA   defnap;

/*!******************************************************/

       short modbpm()

/*      Genererar mode_basic sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5023 = Kan ej skapa mode_global sats
 *
 *      (C)microform ab 1997-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr old_la;

/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_BASIC",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_BASIC");
      goto errend;
      }
/*
***Uppdatera skärmen.
*/
    igupsa();
    igupcs(old_la,V3_CS_NORMAL);

    return(0);
/*
***Felutgångar.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short modgpm()

/*      Genererar mode_global sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5023 = Kan ej skapa mode_global sats
 *              IG2262 = Globala systemet är redan aktivt
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      12/11/85 Koll av lsyspk, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr old_la;

/*
***Om lsyspk = NULL är det globala redan aktivt.
*/
    if ( lsyspk == NULL )
      {
      erpush("IG2262","");
      goto errend;
      }
/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_GLOBAL",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_GLOBAL");
      goto errend;
      }
/*
***Uppdatera skärmen.
*/
    igupsa();
    igupcs(old_la,V3_CS_NORMAL);

    return(0);
/*
***Felutgångar.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short modlpm()

/*      Genererar mode_local sats.

 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: IG5023 = Kan ej skapa MODE_LOCAL sats
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      4/7/84   Felhantering, B. Doverud
 *      9/9/85   Higlight, R. Svedin
 *      30/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ;
    DBptr  old_la;
    bool   right,end;
    pm_ptr valparam,exnpt,dummy;
    short  status;

/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa referens till planet.
*/
    typ = CSYTYP;
    if ( (status=genref(271,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_LOCAL",valparam) < 0 ) goto error;
/*
***Uppdatera skärmen.
*/
    igupsa();
    igupcs(old_la,V3_CS_NORMAL);
    igupcs(lsysla,V3_CS_ACTIVE);

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","MODE_LOCAL");
    errmes();
    gphgal(0);
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igcges(
       char  *typ,
       pm_ptr pplist)

/*      Creates, executes and adds a geometric statement
 *      to end of active module.
 *
 *      In: typ    => Typ av geometrisats, tex. "POI-FREE"
 *          pplist => Pekare till parameterlista.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5213 = Fel från pmcges i igcges
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5233 = Fel från pmlmst i igcges
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *
 ******************************************************!*/

  {
    pmseqn geid;
    pm_ptr retla,ref;
    stidcl kind;
/*
***Create a new unused ID.
*/
    geid = iggnid();
/*
***Mark current PM-stack pointer.
*/
    pmmark();
/*
***Create the statement.
*/
    stlook( typ, &kind, &ref);
    if ( pmcges( ref, geid, pplist, (pm_ptr)NULL, &retla) < 0)
         return(erpush("IG5213",typ));
/*
***Try to execute.
*/
    if ( inssta(retla) < 0) 
      {
      pmrele();
      return(erpush("IG5222",typ));
      }
/*
***Everything ok, add statement to end of module. In RIT-mode,
***reset PM stack pointer.
*/
    if ( v3mode & BAS_MOD )
      {
      if ( pmlmst(actmod, retla) < 0 ) return(erpush("IG5233",typ));
      }
    else
      {
      pmrele();
      }

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short igcprs(
       char  *typ,
       pm_ptr pplist)

/*      Creates, executes and adds a procedure statement
 *      to end of active module.
 *
 *      In: typ    => Typ av procedursats, tex. "MODE_LOCAL"
 *          pplist => Pekare till parameterlista.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5253 = Fel från pmcges i igcprs
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5263 = Fel från pmlmst i igcprs
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      15/3/88    Ritpaketet, J. Kjellander
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr retla,ref;
    stidcl kind;

/*
***Mark current PM-stack pointer.
*/
    pmmark();
/*
***Create the statement.
*/
    stlook( typ, &kind, &ref);
    if ( pmcprs(ref,pplist,&retla) < 0 ) return(erpush("IG5253",typ));
/*
***Try to execute.
*/
    if ( inssta(retla) < 0)
       {
       pmrele();
       return(erpush("IG5222",typ));
       }
/*
***Everything ok, add procedure call to end of module. In RIT-mode,
***reset PM stack pointer.
*/
    if ( v3mode & BAS_MOD )
      {
      if ( pmlmst(actmod, retla) < 0 ) return(erpush("IG5263",typ));
      }
    else
      {
      pmrele();
      }

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short igcpts(
       char *filnam,
       short atyp)

/*      Skapar part-sats.
 *
 *      In: filnam => Pekare till modulfilnamn.
 *          atyp   => PART/RUN/MFUNC
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *
 *      Felkod: IG5272   => Kan ej ladda modulen %s
 *              IG5283   => Kan ej läsa modulens parametrar
 *              IG5023   => Kan ej skapa PART sats
 *              IG5222   => Fel vid interpretering av PART-sats
 *              IG5043   => Fel vid länkning
 *              IG5352   => MACRO anropas som PART
 *
 *      (C)microform ab 9/9/85 J. Kjellander efter Mats "partpm"
 *
 *      30/10/85 Ände och sida, J. Kjellander
 *      23/2/86  Link, J. Kjellander
 *      6/3/86   Defaultvärden, J. Kjellander
 *      23/3/86  genpos(pnr,  B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      14/4/86  pmmark(), J. Kjellander
 *      26/6/86  Nytt anrop till pmrpap(), J. Kjellander
 *      26/6/86  Stöd för ref, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      13/10/86 Help, J. Kjellander
 *      20/10/86 tmpref, J. Kjellander
 *      25/4/87  MFUNC, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      10/11/88 Optionella parametrar, J. Kjellander
 *      11/11/88 Meny-parametrar, J. Kjellander
 *       1/12/91 Default-parametrar, J. Kjellander
 *      13/2/92  Macro, J. Kjellander
 *      15/8/93  Nytt anrop till pmcpas(), J. Kjellander
 *      16/8/93  MACRO får inte anropas som PART, J. Kjellander
 *      9/11/94  GLOBAL_REF, J. Kjellander
 *      1996-05-30 t-sträng i promt, J.Kjellander
 *      1998-09-08 Vänta.. även för MACRO:n, J.Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  oblparam;            /* obligatoriska parametrar */
    pm_ptr  parlst;              /* soft parameter list */
    pm_ptr  exnpt;               /* pekare till expr. node */
    pm_ptr  retla;     
    pm_ptr  oldmod;              /* base adress of caller */
    pm_ptr  newmod;              /* base adress of called module */
    pm_ptr  dummy;
    short   status,mnum,posalt,oldpmd;
    DBetype   typmsk;
    bool    end,right;
    pm_ptr  panola;              /* PM-pointer to param. node */
    char    name[80];            /* parameter name string buffer */
    char    prompt[80];          /* parameter prompt string buffer */
    PMLITVA defval;              /* literal value structure */
    PMMONO *mnpnt;               /* pointer to module node */
    char    dstr[V3STRLEN+1];    /* Defaultsträng för param.värde */
    char    istr[V3STRLEN+1];    /* Inputsträng för param.värde */
    short   tmpafu;              /* Aktiv funktion */
    bool    oldtrf;              /* Aktiv tmpref */
    short   tmphit;              /* Temporär hit */
    short   oldpen,oldlev;       /* Aktiv penna och nivå */
    short   prtid;               /* Partens sekvensnummer */
    bool    optflg,optpar;       /* Optionella parametrar */
    bool    menpar=FALSE;        /* Meny-parameter */
    bool    pospar=FALSE;        /* Pos-parameter */
    PMREFVA prtref;              /* Partens identitet */
    pm_ptr  ref,arglst;          /* För GLOABL_REF */
    stidcl  kind;                /* För GLOABL_REF */

/*
***Om "Kör namngiven modul" eller MFUNC aktivera temporär 
***referens och hit = FALSE.
*/
     optflg = FALSE;
     oldtrf = tmpref;
     tmphit = (short)defnap.hit;
     oldpen = (short)defnap.pen;
     oldlev = (short)defnap.level;

     if ( atyp != PART )
       {
       tmpref = TRUE;
       defnap.hit = 0;
       }
/*
***Om MFUNC, stäng av save.
*/
     if ( atyp == MFUNC ) defnap.save = 0;
/*
***Lagra undan aktiv funktion, sätt den = -2 och lagra
***filnamnet i actpnm för hjälpsystemet.
*/
    tmpafu = actfun;
    actfun = -2;
    strcpy(actpnm,filnam);
/*
***Sätt aktuell pm-pekare.
*/
    pmmark();
/*
***Ladda in modulen.
*/
    oldmod = pmgbla();
    if ( pmgeba(filnam,&newmod) != 0 ) 
      {
      status = erpush("IG5272",filnam);
      goto exit;
      }
/*
***Läs modulens huvud.
*/
    pmsbla(newmod);                 /* set new base adress */
    pmgmod((pm_ptr)0,&mnpnt);       /* get c-pointer to module node */
    pmsbla(oldmod);                 /* reset base adress */
/*
***MACRO får inte anropas som en part.
*/
    if ( (mnpnt->moat_ == MACRO)  && (atyp != MFUNC) )
      {
      status = erpush("IG5352",filnam);
      goto exit;
      }
/*
***Ta reda på modulens attribut LOCAL/GLOBAL.
***Skapa listan med obligatoriska parametrar, ref.
*/
    oblparam = (pm_ptr)NULL;

    if ( mnpnt->moat_ == LOCAL )
        {
        typmsk = CSYTYP;
        if ( (status=genref(271,&typmsk,&exnpt,&end,&right)) < 0 ) goto rject1;
        pmtcon(exnpt,(pm_ptr)NULL,&oblparam,&dummy);
        }
/*
***Skapa listan med modulens parametrar.
*/
    parlst = (pm_ptr)NULL;
    pmsbla(newmod);                          /* set base adress */
    if ( pmrpap((pm_ptr)0) != 0 ) goto error1;

    for(;;)
        {
        if ( pmgpad(&panola) != 0 ) goto error1;
        if ( panola == (pm_ptr)NULL )      /* no more params. ? */
            {
            pmsbla(oldmod);
            break;
            }
        if ( pmrpar(panola,name,prompt,&defval) != 0 ) goto error1;
        pmsbla(oldmod);            /* reset base adress */
/*
***Är det en gömd parameter vars värde skall hämtas från fil ?
*/
        igdefp(prompt,&defval);
/*
***Parameter med promptsträng.
*/
        if ( strlen(prompt) > 0 )
          {
          optpar = igoptp(prompt);
          if ( defval.lit_type == C_STR_VA )
            menpar = igmenp(prompt,&mnum);
          if ( defval.lit_type == C_VEC_VA )
            pospar = igposp(prompt,&posalt);
          if ( defval.lit_type == C_REF_VA &&
            !igtypp(prompt,&typmsk) ) typmsk = ALLTYP;

          igdefp(prompt,&defval);

          if ( optflg && optpar )
            {
            pmclie(&defval,&exnpt);
            }
          else
            {
            optflg = FALSE;
/*
***Mappa ev. t-sträng i prompten till klartext.
*/    
            igtstp(prompt);
/*
***Läs in parametervärde.
*/ 
            switch(defval.lit_type)
              {
              case C_INT_VA:
              igplma(prompt,IG_INP);
              if ( optpar ) status = genint(0,"",istr,&exnpt);
              else
                {
                sprintf(dstr,"%d",defval.lit.int_va);
                status = genint(0,dstr,istr,&exnpt);
                }
              break;

              case C_FLO_VA: 
              igplma(prompt,IG_INP);
              if ( optpar ) status = genflt(0,"",istr,&exnpt);
              else
                {
                sprintf(dstr,"%g",defval.lit.float_va);
                status = genflt(0,dstr,istr,&exnpt);
                }
              break;

              case C_STR_VA:
              if ( optpar )
                {
                if ( menpar )
                  {
                  igplma(prompt,IG_MESS);
                  status = genstm(mnum,&exnpt);
                  }
                else
                  {
                  igplma(prompt,IG_INP);
                  status = genstr(0,"",istr,&exnpt);
                  }
                }
              else
                {
                if ( menpar )
                  {
                  igplma(prompt,IG_MESS);
                  status = genstm(mnum,&exnpt);
                  }
                else
                  {
                  igplma(prompt,IG_INP);
                  status = genstr(0,defval.lit.str_va,istr,&exnpt);
                  }
                }
              break;

              case C_VEC_VA:
              igplma(prompt,IG_MESS);
              if ( pospar )
                {
                oldpmd = posmod; posmod = posalt;
                status = genpos(0,&exnpt); posmod = oldpmd;
                }
              else { status = genpos(0,&exnpt); }
              break;

              case C_REF_VA:
              igplma(prompt,IG_MESS);
              status = genref(0,&typmsk,&exnpt,&end,&right);
              if ( iggflg )
                {
                pmtcon(exnpt,(pm_ptr)NULL,&arglst,&dummy);
                stlook("GLOBAL_REF",&kind,&ref);
                pmcfue(ref,arglst,&exnpt);
                }
              break;

              default:
              gphgal(0);    
              status = erpush("IG5302",name);
              goto exit;
              }

            if ( optpar && (status == REJECT) )
              {
              optflg = TRUE;
              pmclie(&defval,&exnpt);
              }
            else if ( status < 0 ) goto rject2;

            igrsma();
            }
          }
/*
***Parameter utan promptsträng.
*/
        else pmclie(&defval,&exnpt);
/*
***Länka in parametern i i parameterlistan.
*/
        pmtcon(exnpt,parlst,&parlst,&dummy);
        pmsbla(newmod);                  /* set new base adress */
        }
/*
***Skapa satsen, spara sekvensnummer i prtid.
***Nytt anrop till pmcpas 15/8/93 JK.
*/
    prtid = iggnid();

    if ( pmcpas(prtid,filnam,(pm_ptr)NULL,parlst,oblparam,
                (pm_ptr)NULL,&retla) != 0 )
      {
      status = erpush("IG5023","PART");
      goto exit;
      }
/*
***Om det är en MACRO-modul kan den komma att skapa nya satser i PM.
***Därför städar vi bort det nyss genererade part-anropet innan vi
***interpreterar. Under alla omständigheter skall anropet städas
***bort om det är något annat än en part eller om ritsystemet är
***aktivt.
*/
    if ( atyp != PART ||  v3mode == RIT_MOD ) pmrele();
/*
***Prova att interpretera.
***Om exekveringen inte gick bra kan skräp i GM behöva
***städas bort. Dessutom skall anropet i PM strykas.
***Med X kör vi med vänt-hanteringen påslagen.
*/
#ifdef V3_X11
    if ( gptrty == X11 ) wpwait(GWIN_ALL,TRUE);
#endif

    status = inssta(retla);

#ifdef V3_X11
    if ( gptrty == X11 ) wpwait(GWIN_ALL,FALSE);
#endif

    if ( status < 0 )
      {
      if ( defnap.save == 1 )
        {
        prtref.seq_val = prtid; prtref.ord_val = 1;
        prtref.p_nextre = NULL; EXdel(&prtref);
        }
      if ( atyp == PART ) pmrele();
      if ( atyp == MFUNC ) status = erpush("IG5222","MACRO");
      else                 status = erpush("IG5222","PART");
      goto exit;
      }
/*
***EXIT med felmeddelande.
*/
    else if ( status == 3 )
      {
      if ( defnap.save == 1 )
        {
        prtref.seq_val = prtid; prtref.ord_val = 1;
        prtref.p_nextre = NULL; EXdel(&prtref);
        }
      status = 0;
      goto exit;
      }
/*
***EXIT utan felmeddelande.
*/
    else if ( status == 4 ) status = 0;
/*
***Interpreteringen gick bra. Om basmodulen aktiv, och det
***är ett PART-anrop, länka in satsen i satslistan. 
*/
    if ( atyp == PART )
      {
      if ( v3mode & BAS_MOD  &&  pmlmst(actmod, retla) < 0 )
        {
        status = erpush("IG5043","");
        goto exit;
        }
      }
/*
***Avslutning.
*/
    status = 0;
exit:
    tmpref = oldtrf;
/*
***Om ej part, stryk satsen ur PM och återställ statusarea,
***hit och save. Om det är en part kan ändå statusarean behöva
***uppdateras.
*/
    if ( atyp != PART )
      {
      igupsa();
      defnap.hit = tmphit;
      }
    else if ( defnap.pen != oldpen  ||  defnap.level != oldlev ) igupsa();
     
    if ( atyp == MFUNC ) defnap.save = 1;

    actfun = tmpafu;
    gphgal(0);    

    return(status);
/*
***Felutgångar.
*/
rject2:
    igrsma();
rject1:
    pmrele();
    goto exit;

error1:
    pmsbla(oldmod);
    status = erpush("IG5283","");
    goto exit;

  }

/********************************************************/
/*!******************************************************/

       DBseqnum iggnid()

/*      Genererar nytt sekvensnummer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Nytt sekvensnummer.
 *
 *      (C)microform ab 7/11/85 J. Kjellander
 *
 *      1/2/86  anrop till gmrdid() ändrat, J. Kjellander
 *      13/3/86 Slopat ERASED, J. Kjellander
 *      15/3/88 Ritpaketet, J. Kjellander
 *      1998-04-02 Kolla även GM, J.Kjellander
 *
 ******************************************************!*/

  {
    DBseqnum idmax;

/*
***Om ritmodulen aktiv, returnera GM:s idmax + 1.
*/
    if ( v3mode == RIT_MOD )
      {
      DBget_highest_id(&idmax);
      if ( idmax == -1 ) idmax = 0;
      return(idmax+1);
      }
/*
***Om basmodulen aktiv, returnera snrmax + 1.
***Om detta id redan är använt (eller suddat) i GM bör vi
***inte använda det. Så kan det tex. bli om man laddar ett
***GM från fil och sen börjar jobba vidare.
*/
    else
      {
    ++snrmax;
      DBget_highest_id(&idmax);
      while ( snrmax <= idmax  && 
              gmrdid(DBNULL,(DBseqnum)snrmax) != (DBptr)NOTUSD ) ++snrmax;
      return(snrmax);
      }
  }
  
/********************************************************/
/*!******************************************************/

       bool igoptp(
       char *prompt)

/*      Kollar om parameter är optionell, dvs. om dess
 *      promptsträng börjar på $+mellanslag eller $$.
 *
 *      In: Parameterns promptsträng.
 *
 *      Ut: Promtsträngen strippad på inledande $.
 *
 *      FV: TRUE  = Optionell parameter.
 *          FALSE = Ej optionell.
 *
 *      (C)microform ab 19/2/88 J. Kjellander
 *
 *      11/11/88 Meny-parametrar, J. Kjellander
 *
 ******************************************************!*/

  {
  char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == ' ' )
      {
      strcpy(tmp,prompt);
      strcpy(prompt,tmp+2);
      return(TRUE);
      }
    else if ( *prompt == '@'  &&  *(prompt+1) == '@' )
      {
      strcpy(tmp,prompt);
      strcpy(prompt,tmp+1);
      return(TRUE);
      }
    else return(FALSE);

  }
  
/********************************************************/
/*!******************************************************/

       bool igmenp(
       char  *prompt,
       short *mnum)

/*      Kollar om parameter skall tilldelas värde genom
 *      val i meny, dvs. om dess promptsträng börjar på
 *      $+m+heltal.
 *
 *      In: prompt = Parameterns promptsträng.
 *          mnum   = Pekare till ev. resultat.
 *
 *      Ut: Promtsträngen strippad på inledande kod.
 *          *mnum = Ev. meny-nummer.
 *
 *      FV: TRUE  = Meny-parameter.
 *          FALSE = Ej meny-parameter.
 *
 *      (C)microform ab 11/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short n,i,pl;
    char tmp[V3STRLEN+1];

/*
***Står det @m....
*/
    if ( *prompt == '@'  &&  *(prompt+1) == 'm' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%hd",mnum);
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
/*
***Ingen träff.
*/
    return(FALSE);
  }
  
/********************************************************/
/*!******************************************************/

       bool igposp(
       char  *prompt,
       short *posalt)

/*      Kollar om VECTOR-parameter skall tilldelas värde utan
 *      val i pos-meny, dvs. om dess promptsträng börjar på
 *      $+a+heltal.
 *
 *      In: prompt = Parameterns promptsträng.
 *          posalt = Pekare till ev. resultat.
 *
 *      Ut: Promtsträngen strippad på inledande kod.
 *          *posalt = Ev. pos-metod.
 *
 *      FV: TRUE  = Pos-parameter.
 *          FALSE = Ej pos-parameter.
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short n,i,pl;
    char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == 'a' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%hd",posalt);
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
    return(FALSE);
  }
  
/********************************************************/
/*!******************************************************/

       bool igtypp(
       char  *prompt,
       DBetype *typmsk)

/*      Kollar om REF-parameter skall tillåtas referera till
 *      vad som helst, dvs. om dess promptsträng börjar på
 *      $+t+heltal.
 *
 *      In: prompt = Parameterns promptsträng.
 *          typmsk = Pekare till ev. resultat.
 *
 *      Ut: Promtsträngen strippad på inledande kod.
 *          *typmsk = Ev. typmask.
 *
 *      FV: TRUE  = Typ-parameter.
 *          FALSE = Ej typ-parameter.
 *
 *      (C)microform ab 25/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short n,i,pl;
    int   tmpint;
    char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == 't' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%d",&tmpint);
       *typmsk = tmpint;
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
    return(FALSE);
  }
  
/********************************************************/
/*!******************************************************/

       short igdefp(
       char    *prompt,
       PMLITVA *defval)

/*      Kollar om parameter skall använda default
 *      default-värde eller om det skall hämtas från fil.
 *      @f+vägbeskrivning.
 *
 *      In: prompt = Parameterns promptsträng.
 *          defval = Pekare till defaultvärde.
 *
 *      Ut: Promtsträngen strippad på inledande kod.
 *          *defval = Ev. nytt defaultvärde.
 *
 *      FV: 0.
 *
 *      (C)microform ab 11/8/90 J. Kjellander
 *
 *      1/10/91  Strippat Ön, J. Kjellander.
 *      10/11/91 Gömd parameter, J. Kjellander
 *      12/2/92  Bug "iggtts(119)", J. Kjellander
 *
 ******************************************************!*/

  {
    short  n,i,pl,rn;
    int    ival;
    double fval;
    char   tmp[V3STRLEN+1];
    char   defstr[V3STRLEN+1];
    char   path[80];
    FILE   *f;

/*
***Är det 'Krullalfa + f' ?
*/
    if ( *prompt == '@'  &&  *(prompt+1) == 'f' )
      {
/*
***Packa upp filnamnet.
*/
      pl = strlen(prompt);
      for ( n=2; n<pl; ++n)
        if ( prompt[n] == ' '  ||  prompt[n] == '(' ) break;

      strncpy(path,&prompt[2],n-2);
      path[n-2] = '\0';
/*
***Om det är "act_job", ersätt med act_jobdir()+actjobnam().
*/
      if ( strncmp(iggtts(119),path,7) == 0 )
        {
        strcpy(tmp,jobdir);
        strcat(tmp,jobnam);
        strcat(tmp,&path[7]);
        strcpy(path,tmp);
        }
/*
***Packa upp ev. radnr.
*/
      if ( prompt[n] == '(' )
        {
        for ( i=n; i<pl; ++i) if ( prompt[i] == ')' ) break;
        strncpy(tmp,&prompt[n+1],i-n-1);
        tmp[i-n-1] = '\0';
        sscanf(tmp,"%hd",&rn);
        }
      else
        {
        rn = 1;
        i = n - 1;
        }
/*
***Läs värde från filen.
*/
      if ( (f=fopen(path,"r")) != 0 )
        {
        for ( n=0; n<rn; ++n )
          fgets(defstr,V3STRLEN,f);
        n = strlen(defstr);
        if ( defstr[n-1] == '\n' ) defstr[n-1] = '\0';
        fclose(f);
/*
***Lagra defaultvärdet i PMLITVA:n.
*/
        switch ( defval->lit_type )
          {
          case C_STR_VA:
          strcpy(defval->lit.str_va,defstr);
          break;

          case C_INT_VA:
          if ( sscanf(defstr,"%d",&ival) == 1 )
            defval->lit.int_va = ival;
          break;

          case C_FLO_VA:
          if ( sscanf(defstr,"%lf",&fval) == 1 )
            defval->lit.float_va = fval;
          break;
          }
        }
/*
***Strippa promten från vägbeskrivn etc.
*/
      if ( (int)strlen(prompt) > i+1 )
        {
        strcpy(tmp,prompt+i+2);
        strcpy(prompt,tmp);
        }
      else prompt[0] = '\0';
      }

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short igtstp(char *prompt)

/*      Kollar om promten börjar med t samt ett nummer
 *      och översätter isåfall till motsvarande t-sträng.
 *
 *      In: Parameterns promptsträng.
 *
 *      Ut: Ursprunglig eller ny promtsträng.
 *
 *      FV: 0
 *
 *      (C)microform ab 1996-05-30 J. Kjellander
 *
 ******************************************************!*/

  {
   int  tnum;

   if ( *prompt == 't'  &&  (sscanf(prompt+1,"%d",&tnum) == 1) )
     strcpy(prompt,iggtts((short)tnum));

   return(0);
  }
  
/********************************************************/
