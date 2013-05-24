/*!******************************************************************/
/*  File: ig20.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*               -- menu driver --                                  */
/*     igaamu();         Add VT100-meny to menu area                */
/*     igsamu();         Sub VT100-meny from menu area              */
/*     iggalt();         Read choice from active menu               */
/*     igsmmu();         Select main menu                           */
/*     iggmmu();         Returns number of main menu                */
/*                                                                  */
/*                                                                  */
/*               -- message driver --                               */
/*     igplma();         Push line in message area                  */
/*     igptma();         Push t-string in message area              */
/*     igwlma();         Write line in message area                 */
/*     igwtma();         Write t-string in message area             */
/*     igrsma();         Pop  line in message area                  */
/*    *igqema();         Return current t-string                    */
/*                                                                  */
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

extern short   modtyp,modatt,igtrty;
extern short   v3mode;

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
   short m;

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
/*
***Skriv ut aktiv meny om det finns nån.
*/
    if ( mant > 0 )
      {
      if ( (m=mstack[mant-1]) == 0 ) m = iggmmu();
      igpamu(0,0,m);
      }

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
    short m;


    if (mant > 0)
      {
/*
***Stryk menyn.
*/
      mant--;


      /*igupmu(); */

/*
***Skriv ut aktiv meny om det finns nån.
*/
    if ( mant > 0 )
      {
      if ( (m=mstack[mant-1]) == 0 ) m = iggmmu();
      igpamu(0,0,m);
      }

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
    short i,nalt,wdth,cnum,nhits,mnum,altnum,xpos;
    char cbuf[81];
    char c;
    bool exit;

/*
***Div. initiering.
*/
    if ( (mnum=mstack[mant-1]) == 0 ) mnum = iggmmu();
    menu = &mnutab[mnum];
    nalt = menu->nalt;
    wdth = 1;
    cnum = 0;                           /* antal inlästa tecken */
    cbuf[0] = '\0';
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
      }
/*
***Avslutning.
*/
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

/*
***X11/WIN32. Om mode = IG_INP kommer promten ut i inmatningsfönstret
***utan att vi behöver göra något ytterligare här. Om mode = IG_MESS
***använder vi winpac:s meddelandefönster.
*/
#ifdef V3_X11
    if ( mode == IG_MESS ) WPwlma(s);
    return(0);
#endif

#ifdef WIN32
    if ( mode == IG_MESS ) mswlma(s);
    return(0);
#endif

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

/* 
***Kolla att menyn finns.
*/
    if ( mnutab[mnum].rubr == NULL ) return(-1);
/*
***X11.
*/
#ifdef UNIX
    WPpamu(&mnutab[mnum]);
    return(0);
#endif

  }

/******************************************************************/
