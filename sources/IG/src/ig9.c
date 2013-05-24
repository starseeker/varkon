/*!******************************************************************/
/*  File: ig9.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igdlen();    Delete entity                                      */
/*  igdlls();    Delete last entity                                 */
/*  igdlgp();    Delete named group                                 */
/*  trimpm();    Generate TRIM statement                            */
/*  igblnk();    Blank entity                                       */
/*  igubal();    Unblank all entities                               */
/*  ightal();    Make all entities hitable                          */
/*  igupcs();    Redisplay currently active coordinate system       */
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
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern short v3mode,gptrty;

static short igdlgs(DBId *idvek);
static char *mk_dpsimple(V2REFVA *id);
static char *mk_dppart(V2REFVA *id);

/*!******************************************************/

        short igdlen()

/*      Varkonfunktion för att ta bort en geometrisats
 *      via hårkors.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 12/3/86 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *      18/8/88  getmid(), J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,nid,i;
    DBId     idmat[IGMAXID][MXINIV];
    DBetype    typv[IGMAXID];

/*
***Hämta id för någon av storhetens instanser.
*/
    if ( v3mode == RIT_MOD )
      {
      nid = IGMAXID;
      igptma(146,IG_MESS);
      }
    else
      {
      nid = 1;
      igptma(145,IG_MESS);
      }

    typv[0] = ALLTYP;
    status = getmid(idmat,typv,&nid);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Om RITPAK, fråga om OK innan allt försvinner. och
***ta sedan bort allt utan vidare kontroller.
*/
    if ( v3mode == RIT_MOD )
      {
      if ( nid > 0  &&  igialt(1630,67,68,FALSE) == FALSE ) goto exit;
      for ( i=0; i<nid; ++i) igdlgs(&idmat[i][0]);
      }
/*
***I basmodulen tar vi bara bort en storhet i taget.
*/
    igdlgs((DBId *)idmat);
/*
***Slut.
*/
exit:
    gphgal(0);

    return(status);
  }

/********************************************************/
/*!******************************************************/

 static short igdlgs(DBId *idvek)

/*      Tar bort en geometrisats. Om den exekverats
 *      flera gånger tex. i en loop och det finns
 *      flera instanser i GM genereras fel. Det är
 *      dock inte möjligt att kolla om flera instanser
 *      funnits men alla utom första tagits bort med
 *      DEL-anrop. Om så är fallet och den kvarvarande
 *      instansen ej är refererad tas den bort ändå.
 *
 *      In: idvek = ID vars sekvensnummer identifierar
 *                  en viss sats. ID skall vara Lokal!
 *      Ut: Inget.
 *
 *      Felkoder: IG3502 = Storheten ingår i en part
 *                IG3512 = Storheten har flera instanser.
 *
 *      (C)microform ab 12/3/86 J. Kjellander
 *
 *      20/3/86    Anrop till pmtcon pmclie, B. Doverud
 *      31/3/86    DEL, J. Kjellander
 *      9/11/86    BLANK, J. Kjellander
 *      23/12/86   Globala ref, J. Kjellander
 *      2/3/88     Ritpaketet, J. Kjellander
 *      18/2/92    pmamir(), J. Kjellander
 *      1998-09-17 Ny promptning, J.Kjellander
 *
 ******************************************************!*/

  {
    bool     prtflg;
    DBptr    la;
    pm_ptr   exnpt,valparam,dummy,retla;
    GMRECH   hed;
    PMLITVA  litval;

/*
***Om RITPAK, ta bort storheten utan några kontroller.
*/
    if ( v3mode == RIT_MOD ) return(EXdel(idvek));
/*
***Kolla om storheten ingår i en part. Om så är fallet
***tar vi antingen bort hela parten eller blankar
***storheten.
*/
    if ( idvek[0].p_nextre != NULL )
      {
      prtflg = TRUE;
      if ( igials(mk_dppart(idvek),iggtts(67),iggtts(68),TRUE) )
        idvek[0].p_nextre = NULL;
      else if( igialt(249,67,68,FALSE) ) goto blank;
      else return(0);
      }
    else prtflg = FALSE;
/*
***Kolla om den består av flera instanser.
*/
    la = gmrdid(DBNULL,idvek[0].seq_val);
    DBread_header(&hed,la);
    if ( hed.n_ptr != DBNULL || hed.ordnr > 1 )
      {
      if( igialt(372,67,68,TRUE) ) goto blank;
      else return(0);
      }
/*
***Kolla om parten/storheten är refererad.
*/
    if ( pmamir(idvek) == TRUE )
      {
      if ( ( prtflg && igialt(174,67,68,TRUE))  ||
           (!prtflg && igialt(354,67,68,TRUE)) )
        {
        litval.lit_type = C_INT_VA;
        litval.lit.int_va = 1;
        pmclie(&litval,&exnpt);
        if ( pmchnp(idvek,PMBLANK,exnpt,&retla) < 0 ||
             retla == (pm_ptr)NULL ) goto syserr;
        EXblk(idvek);
        return(0);
        }
      else return(0);
      }
/*
***Storheten/parten är inte refererad, ta bort
***ur GM och PM. Om det är en enskild storhet har vi ännu
***inte pratat något, gör det.
*/
    if ( !prtflg  &&  igials(mk_dpsimple(idvek),iggtts(67),
                             iggtts(68),FALSE) == FALSE ) return(0);
    EXdel(idvek);
    if ( pmdges(idvek) < 0 ) goto syserr;

    return(0);
/*
***Blankning.
*/
blank:
    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val = idvek[0].seq_val;
    litval.lit.ref_va[0].ord_val = idvek[0].ord_val;
    litval.lit.ref_va[0].p_nextre = idvek[0].p_nextre;
    pmclie(&litval,&exnpt);
 
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
    if ( igcprs("BLANK",valparam) < 0 ) goto syserr;

    return(0);
/*
***Felutgång.
*/
syserr:
    erpush("IG3523","");
    errmes();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igdlls()

/*      Tar bort sista satsen i PM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      6/9/85  Felhantering, B. Doverud
 *
 ******************************************************!*/

  {

/*
***Stryk sista satsen ur PM.
*/
    if ( pmdlst() < 0 )
      {
      erpush("IG3523","");
      errmes();
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igdlgp()

/*      Varkonfunktion för att ta bort grupp via namn.
 *      Om flera grupper med samma namn fins i GM tas
 *      bara den första bort.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkoder: IG3542 = Gruppen finns ej
 *
 *      (C)microform ab 5/9/85 J. Kjellander
 *
 *      5/11/85 Felhantering, J. Kjellander
 *      1/2/86  EXdel(), J. Kjellander
 *      13/3/86 igdlgs(), J. Kjellander
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    DBId    id[MXINIV];
    GMGRP   grupp;
    DBptr   lavek[GMMXGP];
    DBptr   la;
    DBetype   typ;
    char    grpnam[JNLGTH+1];
    short   ndel = 0,status;

/*
***Läs in gruppnamn.
*/
    igptma(318,IG_INP);
    if ( (status=igssip(iggtts(267),grpnam,"",JNLGTH)) < 0 ) goto exit;
/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',id,&la,&typ);
/*
***Leta rätt på la för första gruppen med angivet namn
   i den aktiva parten och ta bort den.
*/
    while ( DBget_pointer('N',id,&la,&typ) == 0 )
      {
      if ( typ == GRPTYP )
         {
         DBread_group(&grupp,lavek,la);
         if ( strcmp(grpnam,grupp.name_gp) == 0 )
            {
            DBget_id(la,id);
            id[0].seq_val = abs(id[0].seq_val);
            igdlgs(id);
            ++ndel;
            break;
            }
         }
       }
/*
***Om ingen grupp med angivet namn fanns, felmeddelande.
*/
      if (ndel == 0)
         {
         erpush("IG3542","");
         errmes();
         }
/*
***Slut.
*/
exit:
      igrsma();
      return(status);
  }

/********************************************************/
/*!******************************************************/

        short trimpm()

/*      Varkonfunktion för skapa TRIM-sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkoder: IG5023 = Kan ej skapa TRIM-sats
 *
 *      (C)microform ab 15/3/86 J. Kjellander
 *
 *      20/3/86 Anrop till pmtcon pmclie, B. Doverud
 *      24/3/86 Felutgångar, B. Doverud
 *      31/3/86 UNBLANK, J. Kjellander
 *      5/10/86  GOMAIN, B. Doverud
 *      20/11/89 Neg. intnr, J. Kjellander
 *      
 ******************************************************!*/

  {
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype   typ1,typ2;
    bool    end,right;
    short   status;
    PMLITVA litval;
    DBId    id[MXINIV];
    char    istr[V3STRLEN+1];

/*
***Skapa referens till storhet som skall trimmas.
*/
start:
    igptma(352,IG_MESS);
    typ1 = LINTYP+ARCTYP;
    if ( (status=getidt(id,&typ1,&end,&right,(short)0)) < 0 ) goto exit;
    igrsma();

    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val = id[0].seq_val;
    litval.lit.ref_va[0].ord_val = id[0].ord_val;
    litval.lit.ref_va[0].p_nextre = id[0].p_nextre;
    pmclie(&litval,&exnpt1);
/*
***Ände.
*/
    litval.lit_type = C_INT_VA;
    if ( end ) litval.lit.int_va = 1;
    else       litval.lit.int_va = 0;
    pmclie(&litval,&exnpt2);
/*
***Skapa referens till storhet mot vilken trimmning skall ske.
*/
    typ2 = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref (353,&typ2,&exnpt3,&end,&right)) < 0 ) goto exit;
    igrsma();
/*
***Om skärning linje/linje, alt = -1.
*/
    if ( typ1 == LINTYP && typ2 == LINTYP )
      {
      litval.lit_type = C_INT_VA;
      litval.lit.int_va = -1;
      pmclie( &litval, &exnpt4);
      }
/*
***Annars läs in alternativ.
*/
    else
      {
      if ( (status=genint(327,"1",istr,&exnpt4)) < 0 ) goto exit;
      }
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("TRIM",valparam) < 0 ) 
      {
      erpush("IG5023","TRIM");
      goto errend;
      }
/*
***Om storheten inte ingår i en part, skapa
***UNBLANK-anrop, exnpt1 = storhetens id.
*/
    if ( id[0].p_nextre == NULL )
      {
      pmtcon(exnpt1,(pm_ptr)NULL,&valparam,&dummy);
      igcprs("UNBLANK",valparam);
/*
***och blanka storheten när den skapas.
*/
      litval.lit_type = C_INT_VA;
      litval.lit.int_va = 1;
      pmclie(&litval,&exnpt1);
      pmchnp(id,PMBLANK,exnpt1,&retla);
      }

    gphgal(0);
    goto start;
/*
***Slut.
*/
exit:
    igrsma();
    gphgal(0);
    return(status);
/*
***Felutgång.
*/
errend:
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

        short igblnk()

/*      Varkonfunktion för att släcka en storhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *          REJECT => Avsluta
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 2/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status,nid,i;
    DBetype   typvek[IGMAXID];
    DBId    idmat[IGMAXID][MXINIV];

/*
***Hämta identitet.
*/
    igptma(149,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    status=getmid(idmat,typvek,&nid);
    igrsma();
    if (status < 0 ) return(status);
/*
***Blanka storheten.
*/
    for ( i=0; i<nid; ++i ) EXblk(idmat[i]);
    gpepmk();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igubal()

/*      Varkonfunktion för att tända alla storheter.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/3/88 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    bool    gpupfl;
    DBId    dummy;
    GMRECH  hed;


    gpupfl = FALSE;
/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av GM.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
      switch(typ)
        {
        case PRTTYP:
        case GRPTYP:
        break;

        default:
        DBread_header(&hed,la);
        if ( hed.blank )
          {
          hed.blank = FALSE;
          DBupdate_header(&hed,la);
          gpupfl = TRUE;
          }
        break;
        }

/*
***Uppdatera GP.
*/
      if ( gpupfl ) 
        {
        EXdren(la,typ,TRUE,GWIN_ALL);
        gpupfl = FALSE;
        }

      }
    return(0);
    }

/********************************************************/
/*!******************************************************/

        short ightal()

/*      Varkonfunktion för att göra alla storheter pekbara.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 27/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    GMRECH  hed;
    DBId    dummy;

/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av GM.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
      DBread_header(&hed,la);
      hed.hit = TRUE;
      DBupdate_header(&hed,la);
      }
/*
***Rita om skärmen.
*/
    repagm();

    return(0);
    }

/********************************************************/
/*!******************************************************/

        short igupcs(
        DBptr la,
        int   mode)

/*      Uppdaterar grafiska fönster m.a.p.  ev.
 *      aktivt koordinatsystem.
 *
 *      In: la   = GM-pekare till koordinatsystem eller DBNULL.
 *                 Vanligen detsamma som lsysla.
 *          mode = V3_CS_NORMAL eller V3_CS_ACTIVE
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-03-11 J. Kjellander
 *
 ******************************************************!*/

  {
   GMCSY  csy;
   DBTmat mat;

   if ( la != DBNULL )
     {
     DBread_csys(&csy,&mat,la);
     gpupcs(&csy,&mat,la,mode);
#ifdef V3_X11
     if ( gptrty == X11 ) wpupcs(&csy,&mat,la,mode,GWIN_ALL);
#endif
#ifdef WIN32
     wpupcs(&csy,&mat,la,mode,GWIN_ALL);
#endif
     }

   return(0);
   }

/********************************************************/
/*!******************************************************/

  static char *mk_dpsimple(V2REFVA *id)

/*      Skapar delsträng för enkla storheter.
 *
 *      In: id = Storhetens ID.
 *
 *      FV: Pekare till sträng.
 *
 *      (C)microform ab 1998-09-17 J. Kjellander
 *
 *      10/7/2004 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   short tnum;
   int   n;
   DBptr la;
   DBetype typ;
   char  idbuf[V3STRLEN];

   static char strbuf[V3STRLEN] = "";

/*
***Ta reda på typ av storhet.
*/
   DBget_pointer('I',id,&la,&typ);
/*
***"Vill du ta bort"
*/
   strcpy(strbuf,iggtts(409));
   strcat(strbuf," ");
/*
***Storhetens typ.
*/
   switch ( typ )
     {
     case TRFTYP: tnum = 40;  break;
     case POITYP: tnum = 41;  break;
     case LINTYP: tnum = 42;  break;
     case ARCTYP: tnum = 127; break;
     case CURTYP: tnum = 139; break;
     case SURTYP: tnum = 231; break;
     case TXTTYP: tnum = 232; break;
     case XHTTYP: tnum = 233; break;
     case LDMTYP: tnum = 234; break;
     case CDMTYP: tnum = 235; break;
     case RDMTYP: tnum = 236; break;
     case ADMTYP: tnum = 237; break;
     case GRPTYP: tnum = 239; break;
     case CSYTYP: tnum = 240; break;
     case PRTTYP: tnum = 241; break;
     case BPLTYP: tnum = 242; break;
     case MSHTYP: tnum = 558; break;
     default:     tnum = 0;   break;
     }
   strcat(strbuf,iggtts(tnum));
/*
***Lägg till ID.
*/
   igidst(id,idbuf);
   n = strlen(idbuf);
   if ( idbuf[n-1] == '1'  &&  idbuf[n-2] == '.' ) idbuf[n-2] = '\0';
   strcat(strbuf," ");
   strcat(strbuf,idbuf);
/*
***Lägg till "j/n ?".
*/
   strcat(strbuf," ");
   strcat(strbuf,iggtts(67));
   strcat(strbuf,"/");
   strcat(strbuf,iggtts(68));
   strcat(strbuf," ?");

   return(strbuf);
   }

/********************************************************/
/*!******************************************************/

  static char *mk_dppart(V2REFVA *id)

/*      Skapa delsträng för part.
 *
 *      In: id = Storhetens ID.
 *
 *      FV: Pekare till sträng.
 *
 *      (C)microform ab 1998-09-17 J. Kjellander
 *
 *      10/7/2004 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   short    tnum;
   int      n;
   char     idbuf[V3STRLEN];
   DBetype    typ;
   DBptr    la;
   GMPRT    part;
   V2REFVA *tmpptr;

   static char strbuf[V3STRLEN] = "";

/*
***"Denna ".
*/
   strcpy(strbuf,iggtts(1629));
   strcat(strbuf," ");
/*
***Storhetens typ.
*/
   DBget_pointer('I',id,&la,&typ);

   switch ( typ )
     {
     case TRFTYP: tnum = 40;  break;
     case POITYP: tnum = 41;  break;
     case LINTYP: tnum = 42;  break;
     case ARCTYP: tnum = 127; break;
     case CURTYP: tnum = 139; break;
     case SURTYP: tnum = 231; break;
     case TXTTYP: tnum = 232; break;
     case XHTTYP: tnum = 233; break;
     case LDMTYP: tnum = 234; break;
     case CDMTYP: tnum = 235; break;
     case RDMTYP: tnum = 236; break;
     case ADMTYP: tnum = 237; break;
     case GRPTYP: tnum = 239; break;
     case CSYTYP: tnum = 240; break;
     case PRTTYP: tnum = 241; break;
     case BPLTYP: tnum = 242; break;
     case MSHTYP: tnum = 558; break;
     default:     tnum = 0;   break;
     }
   strcat(strbuf,iggtts(tnum));
/*
***Ta fram partens ID.
*/
   tmpptr = id[0].p_nextre;
   id[0].p_nextre = NULL;
/*
***" ingår i "
*/
   strcat(strbuf,iggtts(371));
   strcat(strbuf," '");
/*
***Lägg till ID.
*/
   igidst(id,idbuf);
   n = strlen(idbuf);
   if ( idbuf[n-1] == '1'  &&  idbuf[n-2] == '.' ) idbuf[n-2] = '\0';
   strcat(strbuf,idbuf);
/*
***Partens namn.
*/
   EXgtpt(id,&part);
   strcat(strbuf," ");
   strcat(strbuf,part.name_pt);
/*
***Lägg till ", Vill du ta bort hela parten ?".
*/
   strcat(strbuf,"' ");
   strcat(strbuf,iggtts(1628));
/*
***Återskapa storhetens id.
*/
   id[0].p_nextre = tmpptr;

   return(strbuf);
   }

/********************************************************/
