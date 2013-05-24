/*!******************************************************************/
/*  File: ig20.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*               -- statusfield driver --                           */
/*     iggnsa();         Generate status area                       */
/*     igupsf();         Update statusfield                         */
/*     igupsa();         Update all statusfields                    */
/*                                                                  */
/*               -- menu driver --                                  */
/*     igaamu();         Add VT100-meny to menu area                */
/*     igsamu();         Sub VT100-meny from menu area              */
/*     iggalt();         Read choice from active menu               */
/*     igupmu();         Update menu area                           */
/*     igpamu();         Display menu                               */
/*     igerpl();         Erase arrow                                */
/*     igsmmu();         Select main menu                           */
/*     iggmmu();         Returns number of main menu                */
/*                                                                  */
/*               -- list driver --                                  */
/*     iginla();         Initiate list driver                       */
/*     igalla();         Add line to list area                      */
/*     igrsla();         Reset display after listing                */
/*     igexla();         Exit from list driver                      */
/*                                                                  */
/*               -- message driver --                               */
/*     igplma();         Push line in message area                  */
/*     igptma();         Push t-string in message area              */
/*     igwlma();         Write line in message area                 */
/*     igwtma();         Write t-string in message area             */
/*     igrsma();         Pop  line in message area                  */
/*    *igqema();         Return current t-string                    */
/*                                                                  */
/*     igmlv1();         Display full menus                         */
/*     igmlv2();         Only display headings                      */
/*     igmlv3();         Don't display menus                        */
/*     igslv1();         Display  status area                       */
/*     igslv2();         Don't display status area                  */
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
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include "../include/screen.h"
#include <string.h>

short menlev;

/* menlev är ett heltal 1, 2 eller 3 som anger hur
   stor del av menyer som skall visas på skärmen.
      1 => Meny och rubrik
      2 => Endast rubrik
      3 => Inget alls */


short stalev;

/* stalev är ett heltal 1 eller två som anger
   om statusarean skall visas på skärmen eller ej.
     1 => Visa statusarean
     2 => Visa ej statusarean */


char actmes[MAXTXT+1];

/* actmes är den sträng som finns i meddelanderaden */


short astack[MENLEV+1];

/* astack är en stack med heltal som håller reda på
   vilket alternativ som aktiverats i en viss meny */


char hstack[MENLEV+1][40];

/* hstack är en stack med strängar som håller reda på
   hur man kommit till en viss meny. */

#define MXMLEV 5
char  pstack[MXMLEV+1][81];
short pant = 0;

/* pstack är en stack med strängar som skrivits ut
   på meddelanderaden.
   pant är antal strängar i stacken */


short mstack[MENLEV+1];
short mant = 0;

/* mstack är en stack med heltal som håller reda på
   vilka menyer som aktiverats.
   mant är är antalet menyer i stacken. */


short linpnt;
bool  listop;
short bmhndl;
char  acthds[V3STRLEN+1];

/* Pekare till nästa rad i listarean samt flagga för list-stopp.
   acthds är aktuell headersträng för listarea.
   bmhndl är bitmap-handle för CGI. */

static short mmain;

/* mmain är aktuell huvudmeny */



extern bool    tmpref;
extern char    pidnam[],jobnam[],jobdir[],amodir[],asydir[],hlpdir[];
extern char    mdffil[],actcnm[];
extern V2NAPA  defnap;
extern char    txtmem[];  /* alla t-strängar i en "ragged" array */
extern char   *txtind[];  /* pekare till alla textsträngar */
extern MNUDAT  mnutab[];  /* alla menyer 910227/JK */
extern MNUALT  smbind[];
extern double  gpgszx;
extern short   sarx,sadx,sarfw,sadfw,say,shgt;
extern short   modtyp,modatt,menurx,menury,mhgt,mwdt;
extern short   ialy,ialx,lafcol,lafw,laly,lahedy,laflin;
extern short   lallin,igtrty,maly,malx,rmarg,bmarg;
extern short   v3mode,gptrty;

static void igupsf(char *s, short fnum);

/*!****************************************************************/

        void iggnsa()
 
/*      Skriver ut hela statusarean, både rubriker och värden.
 *
 *      14/5/86  Test av IP32 som terminal, R. Svedin
 *      29/10/86 Rubrik även för IP32, R. Svedin
 *      23/3/88  Ritpaketet, J. Kjellander
 *
 ***************************************************************!*/

  {
/*
***I BATCH-mode skall inget göras här.
*/
   if ( igtrty == BATCH ) return;
/*
***Likaså i X11-mode.
*/
#ifdef V3_X11
    if ( igtrty == X11 ) return;
#endif
/*
***Skall status överhuvud taget skrivas ut ?
*/
    if ( stalev == 1 )
      {
      gpspen(1);
/*
***Toppnivåns rubriker.
*/
     if ( v3mode == TOP_MOD )
       {
       igdfld(sarx,sarfw,say+1);
       igfstr(iggtts(63),JURIGHT,UNDLIN);  /* projekt */
       igpstr(":",REVERS);

       igdfld(sarx,sarfw,say+2);
       igfstr(iggtts(64),JURIGHT,UNDLIN);  /* jobkatalog */
       igpstr(":",REVERS);

       igdfld(sarx,sarfw,say+3);
       igfstr(iggtts(65),JURIGHT,UNDLIN);  /* partkatalog */
       igpstr(":",REVERS);

       igdfld(sarx,sarfw,say+4);
       igfstr(iggtts(66),JURIGHT,UNDLIN);  /* symkatalog */
       igpstr(":",REVERS);

       igdfld(sarx,sarfw,say+5);
       igfstr(iggtts(53),JURIGHT,UNDLIN);  /* hjälpkatalog */
       igpstr(":",REVERS);

       igdfld(sarx,sarfw,say+6);
       igfstr(iggtts(52),JURIGHT,NORMAL);  /* partkatalog */
       igpstr(":",REVERS);
       }
/*
***Ritmodulens rubriker.
*/
      else if ( v3mode == RIT_MOD )
        {
        igdfld(sarx,sarfw,say+1);
        igfstr(iggtts(62),JURIGHT,UNDLIN);  /* job */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+2);
        igfstr(iggtts(56),JURIGHT,UNDLIN);  /* ksy */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+3);
        igfstr(iggtts(58),JURIGHT,UNDLIN);  /* vy */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+4);
        igfstr(iggtts(59),JURIGHT,UNDLIN);  /* pen */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+5);
        igfstr(iggtts(60),JURIGHT,UNDLIN);  /* skl */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+6);
        igfstr(iggtts(61),JURIGHT,NORMAL);  /* niv */
        igpstr(":",REVERS);
        }
/*
***Basmodulens rubriker.
*/
      else
        {
        igdfld(sarx,sarfw,say+1);
        igfstr(iggtts(62),JURIGHT,UNDLIN);  /* job */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+2);
        igfstr(iggtts(54),JURIGHT,UNDLIN);  /* typ */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+3);
        igfstr(iggtts(55),JURIGHT,UNDLIN);  /* atr */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+4);
        igfstr(iggtts(56),JURIGHT,UNDLIN);  /* ksy */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+5);
        igfstr(iggtts(57),JURIGHT,UNDLIN);  /* ref */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+6);
        igfstr(iggtts(58),JURIGHT,UNDLIN);  /* vy */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+7);
        igfstr(iggtts(59),JURIGHT,UNDLIN);  /* pen */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+8);
        igfstr(iggtts(60),JURIGHT,UNDLIN);  /* skl */
        igpstr(":",REVERS);

        igdfld(sarx,sarfw,say+9);
        igfstr(iggtts(61),JURIGHT,NORMAL);  /* niv */
        igpstr(":",REVERS);
        }
/*
***Uppdatera statusvärden.
*/
      igupsa();
      }
  }

/******************************************************************/
/*!****************************************************************/

static  void igupsf(
        char *s,
        short fnum)

/*      Uppdaterar ett visst statusfälts värde.
 *
 *      In: *s   => Värde-sträng.
 *          fnum => Fältnummer.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/3/88 J. Kjellander
 *
 ***************************************************************!*/

  {

/*
***I BATCH-mode skall inget göras här.
*/
   if ( igtrty == BATCH ) return;
/*
***Likaså i X11-mode.
*/
#ifdef V3_X11
    if ( igtrty == X11 ) return;
#endif
/*
***Är statusarean påslagen ?
*/
    if ( stalev == 1 )
      {
      igdfld(sadx,sadfw,say+1+fnum);
      igfstr(s,JULEFT,NORMAL);
      igflsh();
    }
  }

/*****************************************************************/
/*!***************************************************************/

        void igupsa()

/*      Uppdaterar samtliga statusareans värde-fält.
 *
 *      (C)microform ab 23/6/85 J. Kjellander
 *
 *      13/6/86  Variabler för skärmstorlek.  R. Svedin
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      18/10/86 gpgwin(), J. Kjellander
 *      23/3/88  Ritpaketet, J. Kjellander
 *      1999-04-22 BATCH, J.Kjellander
 *
 ***************************************************************!*/

 {
   char   strbuf[20];
   double skala;
   VY     actwin;

/*
***I BATCH-mode skall inget göras här.
*/
   if ( igtrty == BATCH ) return;
/*
***Likaså i X11-mode.
*/
#ifdef V3_X11
    if ( igtrty == X11 ) return;
#endif
/*
***Nokia-terminalen suddar inte när en text skriver över
***en annan.
*/
      if ( igtrty == N220G  &&  stalev == 1 )
        {
        igmvac(sadx,say+1);
        igerar(sadfw,9);
        }
/*
***V3:s toppnivå.
*/
    if ( v3mode == TOP_MOD )
      {
      igupsf(pidnam,0);
      igupsf(jobdir,1);
      igupsf(amodir,2);
      igupsf(asydir,3);
      igupsf(hlpdir,4);
      igupsf(mdffil,5);
      }
/*
***Ritmodulen.
*/
    else if ( v3mode == RIT_MOD )
      {
      igupsf(jobnam,0);
      igupsf(actcnm,1);
      gpgwin(&actwin);
      igupsf(actwin.vynamn,2);
      sprintf(strbuf,"%d",defnap.pen);
      igupsf(strbuf,3);
      skala = gpgszx / (actwin.vywin[2]-actwin.vywin[0]);
      sprintf(strbuf,"%.4g",skala);
      igupsf(strbuf,4);
      sprintf(strbuf,"%d",defnap.level);
      igupsf(strbuf,5);
      }
/*
***Basmodulen.
*/
    else
      {
      igupsf(jobnam,0);
      if ( modtyp == 3 ) igupsf("GEOMETRY",1);
      else igupsf("DRAWING",1);
      if ( modatt == LOCAL ) igupsf("LOCAL",2);
      else if ( modatt == GLOBAL ) igupsf("GLOBAL",2);
      else igupsf("BASIC",2);
      igupsf(actcnm,3);
      if ( tmpref ) igupsf(iggtts(23),4);
      else igupsf(iggtts(22),4);
      gpgwin(&actwin);
      igupsf(actwin.vynamn,5);
      sprintf(strbuf,"%d",defnap.pen);
      igupsf(strbuf,6);
      skala = gpgszx / (actwin.vywin[2]-actwin.vywin[0]);
      sprintf(strbuf,"%.4g",skala);
      igupsf(strbuf,7);
      sprintf(strbuf,"%d",defnap.level);
      igupsf(strbuf,8);
      }
 }

/******************************************************************/
/*!****************************************************************/

         bool igaamu(short mnum)

/*       Add new vt100 menu to menu area and make it active.
 *
 *       IN:
 *       mnum:          menynummer
 *
 *       Ut: Inget.
 *
 *       FV: Inget.
 *
 *       REVIDERAD:
 *
 *       21/10-85 Menystack, Ulf Johansson
 *       9/10/86  Menypil, J. Kjellander
 *
 ****************************************************************!*/

  {

/*
***Kolla att menystacken inte är full.
*/
    if (mant < MENLEV)
      {
/*
***Uppdatera mstack, astack och skriv ut den nya menyn.
*/
      mstack[ mant++ ] = mnum;
      astack[mant] = -1;
      igupmu();
      return(TRUE);
      }
      else return(FALSE);
  }

/******************************************************************/
/*!****************************************************************/

        bool igsamu()

/*      Sub vt100 menu from menu area and make
 *      previous menu active.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      REVIDERAD:
 *
 *      21/10-85 Ulf Johansson
 *      9/10/86  Menypil, J. Kjellander
 *
 ****************************************************************!*/

  {
    if (mant > 0)
      {
/*
***Stryk menyn.
*/
      mant--;
      igupmu();
      return(TRUE);
      }
      else return(FALSE);
  }

/******************************************************************/
/*!****************************************************************/

        short iggalt(
        MNUALT **paltp,
        short   *ptyp)

/*      Returnerar alternativ ur aktiv meny.
 *
 *      In: paltp = Pekare till alternativpekare
 *          ptyp  = Pekare till typ av alternativ
 *
 *      Ut: *paltp = Pekare till alternativ eller NULL
 *          *ptyp  = Typ av alternativ om paltp=NULL
 *
 *      FV: Inget.
 *
 *      REVIDERAD:
 *
 *      22/10-85 Symboler, Ulf Johansson
 *      6/11/85  Meny noll, J. Kjellander
 *      24/2/86  Input läge i X-led R. Svedin
 *      2/10/86  Direkt till huvudmeny, J. Kjellander
 *      12/10/86 Historik, J. Kjellander
 *      28/10/86 Flyttat menyarean, R. Svedin
 *      8/11/88  Snabbval, J. Kjellander
 *      7/8/91   Pil uppåt, J. Kjellander
 *      23/8/92  X11, J. Kjellander
 *
 ****************************************************************!*/

  {
    MNUDAT *menu;
    MNUALT *alt;
    short i,nalt,wdth,cnum,nhits,plen,mnum,altnum,xpos;
    char cbuf[81];
    char c;
    bool exit;

/*
***Div. initiering.
*/
    if ( (mnum=mstack[mant-1]) == 0 ) mnum = iggmmu();
    menu = &mnutab[mnum];
    nalt = menu->nalt;
    plen = strlen(iggtts(MSELPR));      /* promtlängd */
    wdth = 1;
    cnum = 0;                           /* antal inlästa tecken */
    cbuf[0] = '\0';
/*
***Skriv ut prompt och definiera inmatningsfält.
*/
    if ( igtrty != X11 )
      {
      igmvac(ialx,ialy);
      igerar(rmarg,1);
      igmvac(ialx,ialy);
      igpstr(iggtts(MSELPR),REVERS);
      }

    xpos = ialx + plen + 2;

    if ( igtrty != X11 )
      {
      igdfld(xpos,wdth,ialy);
      igmvac(xpos,ialy);
      igflsh();
      }
/*
***Läs in svar.
*/
    exit = FALSE;

    while ( !exit )
      {
      switch ( iggtsm(&c,&alt) )
        {
/*
***Ett tecken, kolla om något alternativ ännu valts ut.
*/
        case SMBCHAR:
        cbuf[cnum++] = c;
        cbuf[cnum] = '\0';

        i = nhits = altnum = 0;
        alt = menu->alt;
        while ( i++ < nalt )
          {
          if (strncmp(alt->str,cbuf,cnum) == 0)
            {
            nhits++;
            *paltp = alt;
            altnum = i;
            }
          alt++;
          }

        if (nhits <= 0)
          {
          igbell();
          cbuf[ --cnum ] = '\0';
          }
        else if (nhits > 1)
          {
          xpos++;
          wdth++;
          }
        else
          {
/*
***Om meny spara nuvarande rubrik i hstack.
*/
          if ( (*paltp)->acttyp == MENU && mant == 1 )
            hstack[mant][0] = '\0';
          if ( (*paltp)->acttyp == MENU && mant > 1 )
            strcpy(hstack[mant],menu->rubr);
/*
***Om inte meny, spara alternativet i hstack och astack
***samt markera i menyn.
*/
          if ( (*paltp)->acttyp != MENU )
            {
            astack[mant] = altnum;
            strcpy(hstack[mant],(*paltp)->str);
            if ( menlev == 1 && igtrty != X11 )
              {
              igdfld(menurx+1,menu->wdth,menury+1+astack[mant]);
              igfstr((*paltp)->str,JULEFT,REVERS);
              }
            }
          exit = TRUE;
          }
        break;
/*
***Return och pil uppåt.
*/
        case SMBRETURN:
        case SMBUP:
        *paltp = NULL;
        *ptyp = SMBRETURN;
        exit = TRUE;
        break;
/*
***Backspace.
*/
        case SMBBACKSP:
        if (cnum == 0) igbell();
        else 
          {
          cbuf[ --cnum ] = '\0';
          xpos--;
          wdth--;
          }
        break;
/*
***Hjälp.
*/
        case SMBHELP:
        if ( ighelp() == GOMAIN ) goto gomain;
/*
***Snabbval. Under inmatningen kan ett MBS-program ha anropats
***och gjort PSH_MEN()!!!.
*/
        case SMBESCAPE:
        if ( (mnum=mstack[mant-1]) == 0 ) mnum = iggmmu();
        menu = &mnutab[mnum];
        nalt = menu->nalt;

        if ( igtrty != X11 )
          {
          igmvac(ialx,ialy);
          igerar(rmarg,1);
          igmvac(ialx,ialy);
          igpstr(iggtts(MSELPR),REVERS);
          igdfld(ialx+plen+2,menu->wdth,ialy);
          }
        break;
/*
***Huvudmenyn.
*/
        case SMBMAIN:
        goto gomain;
        break;
/*
***Meny-alternativ.
*/
        case SMBALT:
       *paltp = alt;
        return(0);
        break;
/*
***Fel.
*/
        default:
        igbell();
        break;
        }
/*
***Skriv ut inmatade tecken och läs in nästa.
*/
      xpos = ialx + plen + 2;

      if ( igtrty != X11 )
        {
        igdfld(xpos,wdth,ialy);
        igfstr(cbuf,JULEFT,NORMAL);
        igmvac(xpos,ialy);
        igflsh();
        }
      }
/*
***Avslutning.
*/
    if ( igtrty != X11 )
      {
      igmvac(ialx,ialy);
      igerar(rmarg,1);
      igflsh();
      }

    return(0);
/*
***GOMAIN.
*/
gomain:
   *paltp = NULL;
   *ptyp = SMBMAIN;
    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igupmu()

/*      Uppdatera aktiv meny.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      REVIDERAD:
 *
 *      22/10-85 Scroll, Ulf Johansson
 *      6/11/85  Meny noll, J. Kjellander
 *      19/2/86  Menystack endast för LVT100 R. Svedin
 *      14/5/86  Test av IP3215 R. Svedin
 *      2/10/86  menlev, J. Kjellander
 *      9/10/86  Tagit bort stacken, J. Kjellander
 *      28/10/86 Flyttat menyarean, R. Svedin
 *
 ****************************************************************!*/

  {
    short m;

/*
***Om det är en NOKIA måste vi sudda menyarean
***eftersom en ny text inte skriver över en gammal.
*/
    if ( igtrty == N220G  &&  menlev < 3)
      {
      igmvac(menurx,menury);
      if ( menlev == 1 ) igerar(mwdt+2,mhgt);
      else igerar(mwdt+2,2);
      }
/*
***Skriv ut aktiv meny om det finns nån.
*/
    if ( mant > 0 )
      {
      if ( (m=mstack[mant-1]) == 0 ) m = iggmmu();
      igpamu(menurx,menury,m);
      }

    igflsh();

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

       short igpamu(
       short x,
       short y,             
       short mnum)            

/*     Skriver ut meny.
 *
 *     In: x,y  => Menyns övre vänstra hörn, vt100-koordinat
 *         mnum => Menynummer
 *
 *     Ut: Inget
 *
 *     FV:  0   => Ok
 *         -1   => No such menu defined
 *
 *     (C)microform ab 1985 Ulf Johansson
 *
 *     2/10/86  menlev, J. Kjellander
 *     8/10/86  historik, J. Kjellander
 *     11/10/86 aktivt alt, J. Kjellander
 *     28/2/91  Optimerat suddning, J. Kjellander
 *
 ****************************************************************!*/

  {
    short   nnalt;        /* Nytt antal alternativ */
    short   nalen;        /* Ny alternativ-maxlängd */
    short   nrlen;        /* Ny rubriklängd */
    short   onalt;        /* Old antal alternativ */
    short   oalen;        /* Old alternativ-maxlängd */

    short   hlen,diff,ax,ay,nrows;    
    char    rbuf[V3STRLEN+1];

    static short omnum = 0;  /* Föregående meny */
    static short orlen = 0;  /* Föregående rubriklängd */
    MNUALT *naltp;

/* 
***Kolla att menyn finns.
*/
    if ( mnutab[mnum].rubr == NULL ) return(-1);
/*
***X11.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      wppamu(&mnutab[mnum]);
      return(0);
      }
#endif
/*
***WIN32.
*/
#ifdef WIN32
    return(0);
#endif
/*
***Den gamla och den nya menyns storlek.
*/
    naltp = mnutab[mnum].alt;
    nnalt = mnutab[mnum].nalt;
    nalen = mnutab[mnum].wdth;

    if ( omnum > 0 )
      {
      onalt = mnutab[omnum].nalt;
      oalen = mnutab[omnum].wdth;
      }
    else
      { onalt = 0; oalen = 0; }
/*
***Beräkna rubrikfältets längd.
*/
    if ( menlev < 3 && mant > 1 ) hlen=strlen(hstack[mant-1]);
    else hlen = 0;

    if ( hlen > nalen ) nrlen = hlen;
    else nrlen = nalen;
    if ( igtrty == CGI ) nrlen = nrlen + 1;
    else nrlen = nrlen + 2;
/*
***Skriv ut alternativ och rubrik.
*/
    if ( menlev < 3 )
      {
/*
***Först alternativen, ev. aktivt alternativ i REVERS.
***Om de nya alternativen är färre än de gamla, sudda
***skillnaden. Likaså om nya rader är kortare än gamla.
*/
      if ( menlev == 1 )
        {
        gpspen(1);
        ax = x+1; ay = y+2;
        nrows = nnalt;
        if ( (diff = oalen - nalen) < 0 ) diff = 0;
        rbuf[diff] = '\0';
        while ( diff > 0 ) rbuf[--diff] = ' ';

        while ( nrows-- )
          {
          igdfld(ax,nalen,ay++); 
          if ( astack[mant] == nnalt-nrows )
               igfstr((naltp++)->str,JULEFT,REVERS);
          else igfstr((naltp++)->str,JULEFT,UNDLIN);
          igwstr(rbuf);
          }
        if ( onalt > nnalt )
          { igmvac(x+1,y+2+nnalt); igerar(oalen,onalt-nnalt); }
        }
     }
/*
***Skriv ut rubriken.
*/
    igdfld(x,nrlen,y+1);
    strcpy(rbuf," ");
    strcat(rbuf,mnutab[mnum].rubr);
    if ( menlev < 3 )
      {
      igfstr(rbuf,JULEFT,REVERS);
/*
***Om nya rubriken är kortare än gamla behövs lite
***extra suddning.
*/
      if ( (diff = orlen - nrlen) < 0 ) diff = 0;
      rbuf[diff] = '\0';
      while ( diff > 0 ) rbuf[--diff] = ' ';
      igwstr(rbuf);
/*
***Skriv ut historik eller tom rad.
*/
      igdfld(x,nrlen,y);
      strcpy(rbuf," ");
      if ( mant > 1 ) strcat(rbuf,hstack[mant-1]);
      igfstr(rbuf,JULEFT,REVERS);

      if ( (diff = orlen - nrlen) < 0 ) diff = 0;
      rbuf[diff] = '\0';
      while ( diff > 0 ) rbuf[--diff] = ' ';
      igwstr(rbuf);
      }
/*
***Uppdatera statiska variabler.
*/
    omnum = mnum;
    orlen = nrlen;

    return(0);

  }

/******************************************************************/
/*!****************************************************************/

        short igerpl()

/*      Suddar eventuell pil i aktiv meny.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform 1986 J. Kjellander
 *
 *      28/10/86 Flyttat menyarean, R. Svedin
 *      3/11/88  N220G, R. Svedin
 *      23/8/92 X11, J. Kjellander
 *
 ****************************************************************!*/

  {
    MNUDAT *menu;
    MNUALT *alt;
    short   m;
/*
***Terminaler med menyer i Tek-alfa mode utan färg och X11.
*/
    if ( igtrty == N220G  ||  igtrty == MSMONO  ||
         igtrty == X11 ) return(0);
/*
***Sudda ev pil på nuvarande meny.
*/
   if ( menlev == 1 && mant > 0 && astack[mant] != -1 )
     {
     gpspen(1);
     if ( (m=mstack[mant-1]) == 0 ) m = iggmmu();
     menu = &mnutab[m];
     alt  = menu->alt + astack[mant] - 1;
     igdfld(menurx+1,menu->wdth,menury+1+astack[mant]);
     igfstr(alt->str,JULEFT,UNDLIN);
     }

   return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igsmmu(short m)

/*      Sätter aktuell huvudmeny.
 *
 *      In: m = Ny huvudmeny.
 *
 *      (C)microform 1996-02-26  J.Kjellander
 *
 ****************************************************************!*/

  {
    mmain = m;
    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short iggmmu()

/*      Returnerar aktuell huvudmeny.
 *
 *      In: Inget.
 *
 *      FV: menynummer.
 *
 *      (C)microform 1996-02-26  J.Kjellander
 *
 ****************************************************************!*/

  {

/*
***Om mmain = 0 har ingen huvudmeny angetts i menyfilen
***utan systemets default huvudmeny skall användas.
*/
   if ( mmain == 0 )
     {
     switch ( v3mode )
       {
       case BAS2_MOD: return(2);
       case BAS3_MOD: return(3);
       case RIT_MOD:  return(4);
       default:       return(-1);
       }
     }
/*
***Om mmain <> 0 har denna huvudmeny angetts som huvudmeny
***i den aktuella menyfilen.  main_menu = nnn.
*/
   else return(mmain);
  }

/******************************************************************/
/*!****************************************************************/

        short iginla(char *hs)

/*      Initiera listarean.
 *
 *      In: *hs => headlinesträng
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      14/5/86  Villkorlig kompilering IP_32 R. Svedin
 *      18/4/87  listop, J. Kjellander
 *      31/10/88 CGI, J. KJellander
 *      3/11/88  N220G, R. Svedin
 *      21/7/92  X11, J. Kjellander
 *      1998-05-27 BATCH, J.Kjellander
 *
 ****************************************************************!*/

  {
    char  tmpbuf[80];

/*
***Stäng av liststoppet.
*/
    listop = FALSE;
/*
***WIN32.
*/
#ifdef WIN32
    msinla(hs);
    return(0);
#endif
/*
***Vilken typ av output ?.
*/
    if ( igtrty == BATCH ) return(0);
/*
***X11.
*/
#ifdef V3_X11
    else if ( igtrty == X11 )
      {
      wpinla(hs);
      return(0);
      }
#endif
/*
***Terminaler med ett bildminne. Spara undan headersträngen
***så att den kan skrivas ut igen vid ny sida.
*/
    else if ( igtrty == N220G  ||  igtrty == MSCOLOUR  ||
              igtrty == MSMONO )
      {
      gperal();
      strcpy(acthds,hs);
      }
/*
***VT100.
*/
    else
      {
      gpansi();
      sprintf(tmpbuf,"\033[%d;%dr",laflin,lallin);
      igpstr(tmpbuf,NORMAL);
      igmvac(1,1); 
      igerar(rmarg,laly);
      }
/*
***Skriv ut rubrik.
*/
    igdfld(lafcol,lafw,lahedy);
    igfstr(hs,JULEFT,REVERS);
    igmvac(1,laflin);
    linpnt = laflin;
  
    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igalla(
        char *ls,
        short lf)

/*      Skriv ut nästa rad listarean.
 *
 *      In: *ls  => Pekare till sträng
 *          lf   => Antal radsprång före utskrift
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      3/2/86   Bytt getchar mot iggtch R. Svedin
 *      6/10/86  GOMAIN, J. Kjellander
 *      21/10/86 Bug, J. Kjellander
 *      18/4/87  listop, J. Kjellander
 *      21/7/92  X11, J. Kjellander
 *      1998-05-27 BATCH, J.Kjellander
 *
 ****************************************************************!*/

  {
    char  c;

/*
***Vilken typ av output ?
*/
    if ( igtrty == BATCH ) return(0);
/*
***X11.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      wpalla(ls,lf);
      return(0);
      }
#endif
/*
***WIN32.
*/
#ifdef WIN32
    msalla(ls,lf);
    return(0);
#endif
/*
***Om listop görs ingenting.
*/
    if ( listop ) return(0);
/*
***Om listarean är full, fråga efter rad eller sida.
*/
    while ( linpnt+lf > lallin )
      {
      igmvac(1,ialy);
      igerar(rmarg,1);
      igmvac(1,ialy);
      igpstr(iggtts(LANXT),REVERS);
      igflsh();
/*
***Läs in svar.
*/
      if ( (c=iggtch()) == ' ')
        {
        if ( igtrty != N220G  &&  igtrty !=  MSCOLOUR  &&
             igtrty != MSMONO )
          {
          igmvac(1,lallin);
          igpstr("\n",NORMAL);
          linpnt--;
          igflsh();
          }
        else igbell();
        }
      else if(c == '\n')
        {
        if ( igtrty == N220G  || igtrty == MSCOLOUR  ||  igtrty == MSMONO )
          {
          gpersc();
          igdfld(lafcol,lafw,lahedy);
          igfstr(acthds,JULEFT,REVERS);
          igmvac(1,laflin);
          linpnt = laflin;
          }
        else
          {
          igmvac(1,lallin);
          while (linpnt > laflin)
             {
             igpstr("\n",NORMAL);
             linpnt--;
             }
           }
        igflsh();
        }
      else if ( c == '\033' )
        {
        listop = TRUE;
        igrsla();
        return(REJECT);
        }
      else if ( c == *smbind[7].str )
        {
        listop = TRUE;
        igrsla();
        return(GOMAIN);
        }
      else
        {
        igbell();
        }
      }
/*
***Listarean är inte full, skriv ut raden.
*/
    linpnt += lf;
    igdfld(lafcol,lafw,linpnt);
    igfstr(ls,JULEFT,NORMAL);
    igflsh();

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igrsla()

/*      Återställer skärmen efter listning i listarean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1986 J. Kjellander
 *
 *      17/1/87  IP_32 V1.3, J. Kjellander
 *      31/10/88 CGI, J. Kjellander
 *      3/11/88  N220G, R. Svedin
 *      1998-05-27 BATCH, J.Kjellander
 *
 ****************************************************************!*/

  {
    char  tmpbuf[80];

/*
***Vilken typ av output ?
*/
    if ( igtrty == BATCH ) return(0);
/*
***Terminaler med ett bildminne.
*/
    else if ( igtrty == N220G  ||  igtrty == MSCOLOUR  ||
              igtrty == MSMONO )
      {
      if ( igialt(165,67,68,FALSE) ) repagm();
      else
        {
        gpersc(); gpdram();
        igupmu(); iggnsa();
        }
      }
/*
***VT100, Återställ menyarea, statusarea och inputrad.
*/
    else
      {
      sprintf(tmpbuf,"\033[%d;%dr",1,bmarg);
      igpstr(tmpbuf,NORMAL);
      igmvac(1,1);
      igerar(rmarg,laly);
      iggnsa();
      igupmu();
      igmvac(1,ialy);
      igerar(rmarg,1);
      }

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igexla()

/*      Avslutar listning i listarean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      14/5/86 Villkorlig kompilering IP_32 R. Svedin
 *      18/4/87  listop, J. Kjellander
 *      21/7/92  X11, J. Kjellander
 *      1998-05-27 BATCH, J.Kjellander
 *
 ****************************************************************!*/

  {
    char  c;
    short status;
/*
***Vilken typ av output ?
*/
    if ( igtrty == BATCH ) return(0);
/*
***X11.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      wpexla(TRUE);
      return(0);
      }
#endif
/*
***WIN32.
*/
#ifdef WIN32
    msexla(TRUE);
    return(0);
#endif
/*
***Om listop är avslutning redan klar.
*/
    if ( !listop )
      {
      switch ( igtrty )
        {
/*
***Om terminal med bara ett bildminne återställer vi
***skärmen utan extra fråga. Fråga kommer ju då i
***igrsla() om hurvida skärmen skall ritas om.
*/
        case N220G:
        case MSCOLOUR:
        case MSMONO:
        status = 0;
        break;
/*
***Om två bildminnen behöver inte skärmen ritas om och följd-
***aktligen kommer ingen fråga om detta i igrsla(). Ställ
***istället en fråga här.
*/
        default:
loop:
        igmvac(1,ialy);
        igerar(rmarg,1);
        igmvac(1,ialy);
        igpstr(iggtts(LARDY),REVERS);
        igflsh();

        if ( (c=iggtch()) == '\n') status = 0;
        else if ( c == *smbind[7].str ) status = GOMAIN;
        else
          {
          igbell();
          goto loop;
          }
        break;
        }
/*
***Avslutning.
*/
      igrsla();
      return(status);
      }

    else return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igplma(
        char *s,
        int   mode)

/*      Skriv ut och stacka text på meddelanderaden.
 *
 *      In:  *s  => Pekare till sträng.
 *          mode => IG_INP  = Prompt före inmatning
 *                  IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/1/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    if ( pant < MXMLEV )
      {
      strncpy(pstack[pant],s,80);
      pstack[pant][80] = '\0';
      ++pant;
      }

    return(igwlma(s,mode));
  }

/******************************************************************/
/*!****************************************************************/

        short igptma(
        int tsnum,
        int mode)

/*      Skriver ut (pushar) t-sträng på meddelanderaden eller
 *      motsvarande.
 *
 *      In: tsnum => Numret på t-strängen.
 *          mode  => IG_INP  = Prompt före inmatning
 *                   IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/1/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    return(igplma(iggtts(tsnum),mode));
  }

/******************************************************************/
/*!****************************************************************/

        short igwlma(
        char *s,
        int   mode)

/*      Skriv ut text på meddelanderaden.
 *
 *      In:  *s  => Pekare till sträng.
 *          mode => IG_INP  = Prompt före inmatning
 *                  IG_MESS = Meddelande
 *
 *      Ut: Inget.
 *
 *     (C)microform ab 15/11/88 J. Kjellander
 *
 ****************************************************************!*/

  {
    static short pl = -1;   /* Föregående promts längd */

/*
***X11/WIN32. Om mode = IG_INP kommer promten ut i inmatningsfönstret
***utan att vi behöver göra något ytterligare här. Om mode = IG_MESS
***använder vi winpac:s meddelandefönster.
*/
#ifdef V3_X11
    if ( gptrty == X11 )
      {
      if ( mode == IG_MESS ) wpwlma(s);
      return(0);
      }
#endif

#ifdef WIN32
    if ( mode == IG_MESS ) mswlma(s);
    return(0);
#endif
/*
***Övriga terminaler.
***Första gången man kommer in här finns ingen föregående
***promt, sätt den till max.
*/
    if ( pl == -1 ) pl = rmarg;
/*
***Sudda promtarean.
*/
    igmvac(malx,maly);
    igerar(pl,1);
    pl = strlen(s);
/*
***Skriv ut.
*/
    igmvac(malx,maly);
    igpstr(s,NORMAL);
    if ( igtrty == LVT100 ) gpdram();
    igflsh();

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short igwtma(short tsnum)

/*
 *      Skriver ut t-sträng på meddelanderaden utan att
 *      meddelandet pushas på meddelandestacken.
 *
 *      In: tsnum => Numret på t-strängen.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 2/2/95 J. Kjellander
 *
 ****************************************************************!*/

  {
    return(igwlma(iggtts(tsnum),IG_MESS));
  }

/******************************************************************/
/*!****************************************************************/

        short igrsma()

/*      Suddar meddelanderaden, dvs. gör pull på pstack.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 ****************************************************************!*/

  {

    if ( pant > 0 ) --pant;

    if ( pant > 0 ) igwlma(pstack[pant-1],IG_MESS);
    else            igwlma("",IG_MESS);

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        char *igqema()

/*      Svarar på frågan: Vilken var den sista t-sträng som skrevs
 *      i meddelande-arean. 
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Pekare till sträng.
 *
 *
 *     (C)microform ab 28/10-85 Ulf Johansson
 *
 *     10/10/86 pstack, J. Kjellander
 *
 ****************************************************************!*/

  {
    if ( pant > 0 )
      {
      if ( pant <= MXMLEV ) return(pstack[pant-1]);
      else return(pstack[MXMLEV-1]);
      }
    else return("");
  }

/******************************************************************/
/*!******************************************************/

        short igmlv1()

/*      Aktiverar menyer och rubriker.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: Ingen.
 *
 *      (C)microform ab 2/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
    menlev = 1;
    igupmu();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igmlv2()

/*      Aktiverar bara rubriker.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: Ingen.
 *
 *      (C)microform ab 2/10/86 J. Kjellander
 *
 *      28/10/86 Flyttat menyarean, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Sätt menlev.
*/
    /*igerpl();*/
    menlev = 2;
/*
***Sudda menyarean.
*/
    igmvac(menurx,menury);
    igerar(mwdt+2,mhgt);
    igupmu();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igmlv3()

/*      Stänger av menyer och rubriker.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: Ingen.
 *
 *      (C)microform ab 2/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sätt menlev.
*/
    /*igerpl();*/
    menlev = 3;
/*
***Sudda menyarean.
*/
    igmvac(menurx,menury);
    igerar(mwdt+2,mhgt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igslv1()

/*      Aktiverar statusarean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: Ingen.
 *
 *      (C)microform ab 2/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sätt stalev.
*/
    stalev = 1;
/*
***Uppdatera status-arean.
*/
    iggnsa();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igslv2()

/*      Stänger av statusarean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: Ingen.
 *
 *      (C)microform ab 2/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sätt stalev.
*/
    stalev = 2;
/*
***Sudda status-arean.
*/
    igmvac(sarx-2,say-1);
    igerar(sarfw+sadfw+4,shgt+3);

    return(0);
  }

/********************************************************/
