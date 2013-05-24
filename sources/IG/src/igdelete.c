/*!******************************************************************/
/*  igdelete.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGdelete_entity();  Delete entity                               */
/*  IGdelete_group();   Delete named group (explicit mode only)     */
/*  IGtrim();           Generate TRIM statement                     */
/*  IGblnk();           Blank entity                                */
/*  IGubal();           Unblank all entities                        */
/*  IGhtal();           Make all entities hitable                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern short sysmode;

static short igdlgs(DBId *idvek);
static char *mk_dpsimple(V2REFVA *id);
static char *mk_dppart(V2REFVA *id);

/*!******************************************************/

        short IGdelete_entity()

/*      Varkonfunktion f�r att ta bort en geometrisats
 *      via h�rkors.
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
 *      18/8/88  IGgmid(), J. Kjellander
 *      2007-04-10 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i,nid;
    DBId     idmat[IGMAXID][MXINIV];
    DBetype    typv[IGMAXID];

/*
***H�mta id f�r n�gon av storhetens instanser.
*/
    if ( sysmode == EXPLICIT )
      {
      nid = IGMAXID;
      WPaddmess_mcwin(IGgtts(146),WP_PROMPT);
      }
    else
      {
      nid = 1;
      WPaddmess_mcwin(IGgtts(145),WP_PROMPT);
      }

    typv[0] = ALLTYP;
    status = IGgmid(idmat,typv,&nid);
    WPclear_mcwin();
    if ( status < 0 ) goto exit;
/*
***Om RITPAK, fr�ga om OK innan allt f�rsvinner. och
***ta sedan bort allt utan vidare kontroller.
*/
    if ( sysmode == EXPLICIT )
      {
      if ( nid > 0  &&  IGialt(1630,67,68,FALSE) == FALSE ) goto exit;
      for ( i=0; i<nid; ++i) igdlgs(&idmat[i][0]);
      }
/*
***I basmodulen tar vi bara bort en storhet i taget.
*/
    igdlgs((DBId *)idmat);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***The end.
*/
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

 static short igdlgs(DBId *idvek)

/*      Tar bort en geometrisats. Om den exekverats
 *      flera g�nger tex. i en loop och det finns
 *      flera instanser i GM genereras fel. Det �r
 *      dock inte m�jligt att kolla om flera instanser
 *      funnits men alla utom f�rsta tagits bort med
 *      DEL-anrop. Om s� �r fallet och den kvarvarande
 *      instansen ej �r refererad tas den bort �nd�.
 *
 *      In: idvek = ID vars sekvensnummer identifierar
 *                  en viss sats. ID skall vara Lokal!
 *      Ut: Inget.
 *
 *      Felkoder: IG3502 = Storheten ing�r i en part
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
    char     mesbuf[2*V3STRLEN],idstr[V3STRLEN];
    bool     prtflg;
    DBptr    la;
    pm_ptr   exnpt,valparam,dummy,retla;
    DBHeader hed;
    PMLITVA  litval;

/*
***Om RITPAK, ta bort storheten utan n�gra kontroller.
*/
    if ( sysmode == EXPLICIT ) return(EXdel(idvek));
/*
***Kolla om storheten ing�r i en part. Om s� �r fallet
***tar vi antingen bort hela parten eller blankar
***storheten.
*/
    if ( idvek[0].p_nextre != NULL )
      {
      prtflg = TRUE;
      if ( IGials(mk_dppart(idvek),IGgtts(67),IGgtts(68),TRUE) )
        idvek[0].p_nextre = NULL;
      else if( IGialt(249,67,68,FALSE) ) goto blank;
      else return(0);
      }
    else prtflg = FALSE;
/*
***Kolla om den best�r av flera instanser.
*/
    la = gmrdid(DBNULL,idvek[0].seq_val);
    DBread_header(&hed,la);
    if ( hed.n_ptr != DBNULL || hed.ordnr > 1 )
      {
      if( IGialt(372,67,68,TRUE) ) goto blank;
      else return(0);
      }
/*
***Kolla om parten/storheten �r refererad.
*/
    if ( pmamir(idvek) == TRUE )
      {
      if ( ( prtflg && IGialt(174,67,68,TRUE))  ||
           (!prtflg && IGialt(354,67,68,TRUE)) )
        {
        litval.lit_type = C_INT_VA;
        litval.lit.int_va = 1;
        pmclie(&litval,&exnpt);
        if ( pmchnp(idvek,PMBLANK,exnpt,&retla) < 0 ||
             retla == (pm_ptr)NULL ) goto syserr;
        EXblk(idvek);
        strcpy(mesbuf,IGgtts(60));
        IGidst(idvek,idstr);
        strcat(mesbuf,idstr);
        WPaddmess_mcwin(mesbuf,WP_MESSAGE);
        return(0);
        }
      else return(0);
      }
/*
***Storheten/parten �r inte refererad, ta bort
***ur GM och PM. Om det �r en enskild storhet har vi �nnu
***inte pratat n�got, g�r det.
*/
    if ( !prtflg  &&  IGials(mk_dpsimple(idvek),IGgtts(67),
                             IGgtts(68),FALSE) == FALSE ) return(0);
    EXdel(idvek);
    if ( pmdges(idvek) < 0 ) goto syserr;
/*
***Confirmational message.
*/
    strcpy(mesbuf,IGgtts(59));
    IGidst(idvek,idstr);
    strcat(mesbuf,idstr);
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
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
    if ( IGcprs("BLANK",valparam) < 0 ) goto syserr;

    return(0);
/*
***Felutg�ng.
*/
syserr:
    erpush("IG3523","");
    errmes();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGdelete_group()

/*      Varkonfunktion f�r att ta bort grupp via namn.
 *      Om flera grupper med samma namn fins i GM tas
 *      bara den f�rsta bort.
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
    DBGroup   grupp;
    DBptr   lavek[GMMXGP];
    DBptr   la;
    DBetype   typ;
    char    grpnam[JNLGTH+1];
    short   ndel = 0,status;

/*
***L�s in gruppnamn.
*/
    if ( (status=IGssip(IGgtts(318),IGgtts(267),grpnam,"",JNLGTH)) < 0 ) goto exit;
/*
***H�mta LA och typ f�r huvud-parten.
*/
    DBget_pointer('F',id,&la,&typ);
/*
***Leta r�tt p� la f�r f�rsta gruppen med angivet namn
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
      IGrsma();
      return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGtrim()

/*      Varkonfunktion f�r skapa TRIM-sats.
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
 *      24/3/86 Felutg�ngar, B. Doverud
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
***ID of entity to be trimmed.
*/
start:
    WPaddmess_mcwin(IGgtts(352),WP_MESSAGE);
    typ1 = LINTYP+ARCTYP;
    if ( (status=IGgsid(id,&typ1,&end,&right,(short)0)) < 0 ) goto exit;

    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val = id[0].seq_val;
    litval.lit.ref_va[0].ord_val = id[0].ord_val;
    litval.lit.ref_va[0].p_nextre = id[0].p_nextre;
    pmclie(&litval,&exnpt1);
/*
***End.
*/
    litval.lit_type = C_INT_VA;
    if ( end ) litval.lit.int_va = 1;
    else       litval.lit.int_va = 0;
    pmclie(&litval,&exnpt2);
/*
***Skapa referens till storhet mot vilken trimmning skall ske.
*/
    typ2 = LINTYP+ARCTYP+CURTYP;
    if ( (status=IGcref (353,&typ2,&exnpt3,&end,&right)) < 0 ) goto exit;
/*
***Om sk�rning linje/linje, alt = -1.
*/
    if ( typ1 == LINTYP && typ2 == LINTYP )
      {
      litval.lit_type = C_INT_VA;
      litval.lit.int_va = -1;
      pmclie( &litval, &exnpt4);
      }
/*
***Annars l�s in alternativ.
*/
    else
      {
      if ( (status=IGcint(327,"1",istr,&exnpt4)) < 0 ) goto exit;
      }
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcprs("TRIM",valparam) < 0 ) 
      {
      erpush("IG5023","TRIM");
      goto errend;
      }
/*
***Om storheten inte ing�r i en part, skapa
***UNBLANK-anrop, exnpt1 = storhetens id.
*/
    if ( id[0].p_nextre == NULL )
      {
      pmtcon(exnpt1,(pm_ptr)NULL,&valparam,&dummy);
      IGcprs("UNBLANK",valparam);
/*
***och blanka storheten n�r den skapas.
*/
      litval.lit_type = C_INT_VA;
      litval.lit.int_va = 1;
      pmclie(&litval,&exnpt1);
      pmchnp(id,PMBLANK,exnpt1,&retla);
      }

    WPerhg();
    goto start;
/*
***Slut.
*/
exit:
    WPerhg();
    return(status);
/*
***Felutg�ng.
*/
errend:
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

        short IGblnk()

/*      Varkonfunktion f�r att sl�cka en storhet.
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
    short   status;
    int     nid,i;
    DBetype typvek[IGMAXID];
    DBId    idmat[IGMAXID][MXINIV];

/*
***H�mta identitet.
*/
    IGptma(149,IG_MESS);
    typvek[0] = ALLTYP;
    nid = IGMAXID;
    status=IGgmid(idmat,typvek,&nid);
    IGrsma();
    if (status < 0 ) return(status);
/*
***Blanka storheten.
*/
    for ( i=0; i<nid; ++i ) EXblk(idmat[i]);
    WPepmk(GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGubal()

/*      Varkonfunktion f�r att t�nda alla storheter.
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
    DBHeader  hed;


    gpupfl = FALSE;
/*
***H�mta LA och typ f�r huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och h�mta LA och typ f�r resten av GM.
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

        short IGhtal()

/*      Varkonfunktion f�r att g�ra alla storheter pekbara.
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
    DBHeader  hed;
    DBId    dummy;

/*
***H�mta LA och typ f�r huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och h�mta LA och typ f�r resten av GM.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
      DBread_header(&hed,la);
      hed.hit = TRUE;
      DBupdate_header(&hed,la);
      }
/*
***Uppdatera.
*/
    WPrepaint_GWIN(GWIN_MAIN);

    return(0);
    }

/********************************************************/
/*!******************************************************/

  static char *mk_dpsimple(V2REFVA *id)

/*      Skapar delstr�ng f�r enkla storheter.
 *
 *      In: id = Storhetens ID.
 *
 *      FV: Pekare till str�ng.
 *
 *      (C)microform ab 1998-09-17 J. Kjellander
 *
 *      10/7/2004 Mesh, J.Kjellander, �rebro university
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
***Ta reda p� typ av storhet.
*/
   DBget_pointer('I',id,&la,&typ);
/*
***"Vill du ta bort"
*/
   strcpy(strbuf,IGgtts(409));
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
   strcat(strbuf,IGgtts(tnum));
/*
***L�gg till ID.
*/
   IGidst(id,idbuf);
   n = strlen(idbuf);
   if ( idbuf[n-1] == '1'  &&  idbuf[n-2] == '.' ) idbuf[n-2] = '\0';
   strcat(strbuf," ");
   strcat(strbuf,idbuf);
/*
***L�gg till "j/n ?".
*/
   strcat(strbuf," ");
   strcat(strbuf,IGgtts(67));
   strcat(strbuf,"/");
   strcat(strbuf,IGgtts(68));
   strcat(strbuf," ?");

   return(strbuf);
   }

/********************************************************/
/*!******************************************************/

  static char *mk_dppart(V2REFVA *id)

/*      Skapa delstr�ng f�r part.
 *
 *      In: id = Storhetens ID.
 *
 *      FV: Pekare till str�ng.
 *
 *      (C)microform ab 1998-09-17 J. Kjellander
 *
 *      10/7/2004 Mesh, J.Kjellander, �rebro university
 *
 ******************************************************!*/

  {
   short    tnum;
   int      n;
   char     idbuf[V3STRLEN];
   DBetype    typ;
   DBptr    la;
   DBPart    part;
   V2REFVA *tmpptr;

   static char strbuf[V3STRLEN] = "";

/*
***"Denna ".
*/
   strcpy(strbuf,IGgtts(1629));
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
   strcat(strbuf,IGgtts(tnum));
/*
***Ta fram partens ID.
*/
   tmpptr = id[0].p_nextre;
   id[0].p_nextre = NULL;
/*
***" ing�r i "
*/
   strcat(strbuf,IGgtts(371));
   strcat(strbuf," '");
/*
***L�gg till ID.
*/
   IGidst(id,idbuf);
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
***L�gg till ", Vill du ta bort hela parten ?".
*/
   strcat(strbuf,"' ");
   strcat(strbuf,IGgtts(1628));
/*
***�terskapa storhetens id.
*/
   id[0].p_nextre = tmpptr;

   return(strbuf);
   }

/********************************************************/
