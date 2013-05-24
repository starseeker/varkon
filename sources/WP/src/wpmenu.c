/**********************************************************************
*
*    wpmenu.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPmini();   Init menu window handler
*    WPpamu();   Push active menu
*    WPwcmw();   Create menu window
*    WPrcmw();   Replace menu alternatives
*    WPifae();   Checks if alternative is selected
*    WPfomw();   FocusIn
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

static wpw_id   men_id;

/* men_id är ID för meny-fönstret. */

extern MNUDAT      *actmeny;

static Window   altid[25];
static int      fh,fl,ly; 
 
/*!******************************************************/

        short WPmini()

/*      Initierar menyfönsterhanteringen. Skapar meny-
 *      fönstret.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 * 
 *      Felkoder: WP1252 = Kan ej skapa menyfönster på %s
 *
 *      (C)microform ab 30/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int       x,y,dx,dy;
   unsigned int dum1,dum2; 
   char      title[V3STRLEN+1];
   short     status;
   DBint     w_id;
   char     *type[20];
   XrmValue  value;

/*
***Placering och storlek.
*/
   x  = y  = 5;
   dx = dy = 30;

   if ( XrmGetResource(xresDB,"varkon.menu.geometry","Varkon.Menu.Geometry",
        type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Fönstertitel.
*/
    if ( !WPgrst("varkon.menu.title",title) ) strcpy(title,"MENY");

   status = WPwcmw(x,y,dx,dy,title,&w_id);
   if ( status < 0 ) return(erpush("WP1252",XDisplayName(NULL)));
/*
***Inga menyer ännu aktiva.
*/
   actmeny = NULL;
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!*******************************************************/

     short WPpamu(
     MNUDAT *meny)  

/*   Gör meny aktiv.
 *
 *   In: *meny = pekare till en viss meny.
 *
 *   Ut: Inget.
 *
 *   FV: 0. 
 *
 *   (C)microform ab 24/1/94 J. Kjellander
 *
 *******************************************************!*/

 {

/*
***Aktiv meny.
*/
    actmeny = meny;   
/*
***Skapa nya alternativfönster.
*/
    WPrcmw();
/*
***Slut.
*/
    return(0);
 }

/*********************************************************/
/*!******************************************************/

        short WPwcmw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        DBint  *id)

/*      Skapar ett WPIWIN-fönster för en meny.
 *
 *      In: x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = Fönstertitel.
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      (C)microform ab 24/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    WPIWIN *iwinpt;

/*
***Skapa ett WPIWIN-fönster. Spara ID i static-variabeln 
***men_id. Denna används av WPrcmw().
*/
    status = WPwciw(x,y,dx,dy,label,id);
    if ( status < 0 ) return(status);
    men_id = *id;
/*
***Vad blev resultatet.
*/
    iwinpt = (WPIWIN *)wpwtab[(wpw_id)*id].ptr;
/*
***Mappa.
*/
    WPwshw(*id);
/*
***För att varje sub-fönster vid meny-byte varje gång inte
***skall mappas individuellt av WPmcbu() sätter vi meny-
***fönstrets map-status till false och mappar alla subfönster
***vid ett enda tillfälle i WPrcmw().
*/
    iwinpt->mapped = FALSE;

    return(0);
  }

/********************************************************/
/*!*******************************************************/

     short WPrcmw()

/*   Skapar nya alternativ-fönster i meny-fönstret.
 *
 *   In: Inget.
 *
 *   Ut: Inget.
 *
 *   (C)microform ab 8/7/92 U. Andersson.
 *
 *    24/1/94  Omarbetad, J. Kjellander
 *
 *******************************************************!*/

 {
    int         mendx,mendy;
    char       *ftitel = "MENY";
    int         bw,lm,tl,mrl,i,tlmax,ftl; 
    DBint       rubr_id,alt_id;
    MNUALT     *altptr;
    WPIWIN     *mwinpt;
    WPBUTT     *butptr;
    XSizeHints *size_hints;

/*
***Lite initiering.
*/
    altptr = actmeny->alt;
    tlmax = 0;
    mrl = WPstrl(actmeny->rubr);
    ftl = WPstrl(ftitel);
/*
***Beräkning av menyhöjd.
*/
    ly = (int)(0.5*WPstrh());
    lm = (int)(0.5*WPstrh());
    fh = (int)(1.5*WPstrh());
    mendy = lm + fh + (actmeny->nalt - 1)*lm + actmeny->nalt*fh + ly;
/*
***Beräkning av maximal textlängd.
*/
    for (i=0; i<actmeny->nalt; i++)
       {
       tl = WPstrl(altptr->str);
       ++altptr;
       if ( tl > tlmax) tlmax = tl;
       } 
/*
***Kolla även meny-rubrikens och fönstertitelns längd.
*/
       if ( mrl > tlmax) tlmax = mrl;
       if ( ftl > tlmax) tlmax = ftl;
/*
***Beräkning av menyns bredd.
*/    
    fl = (int)(1.2*tlmax);
    mendx = ly + fl + ly;
/*
***Döda nuvarande rubrik och alternativ-fönster.
*/
    mwinpt = (WPIWIN *)wpwtab[men_id].ptr;

    for ( i=0; i<WP_IWSMAX; ++i )
      {
      butptr = (WPBUTT *)mwinpt->wintab[i].ptr;
      if ( butptr != NULL )
        {
        WPdlbu(butptr);
        mwinpt->wintab[i].ptr = NULL;
        mwinpt->wintab[i].typ = TYP_UNDEF;
        }
      else break;
      }

    XDestroySubwindows(xdisp,mwinpt->id.x_id);
/*
***Låt meny-fönstret ändra storlek.
*/
    mwinpt->geo.dx = (short)mendx;
    mwinpt->geo.dy = (short)mendy;
    XResizeWindow(xdisp,mwinpt->id.x_id,mendx,mendy);

    size_hints = XAllocSizeHints();
    size_hints->flags      = USSize | PMinSize | PMaxSize; 
    size_hints->min_width  = size_hints->max_width  = size_hints->width  = mendx;
    size_hints->min_height = size_hints->max_height = size_hints->height = mendy; 
    XSetWMNormalHints(xdisp,mwinpt->id.x_id,size_hints);
    XFree(size_hints);
/*
***Skapa menyrubrik-fönster.
*/
    WPmcbu(men_id,ly,lm-2,fl,fh,0,actmeny->rubr,actmeny->rubr,
                                     "",WP_BGND,WP_FGND,&rubr_id);
/*
***Skapa alternativ-fönster.
*/
    altptr = actmeny->alt;
    bw = 1;

    for ( i=0; i<actmeny->nalt; i++ )
      {
      WPmcbu(men_id,ly-bw,lm + fh + i*(fh + lm),fl,fh,bw,
                   altptr->str,altptr->str,"",WP_BGND,WP_FGND,&alt_id);
      butptr = (WPBUTT *)mwinpt->wintab[alt_id].ptr;
      altid[i] = butptr->id.x_id;
      ++altptr;
      } 
/*
***Mappa alla sub-fönster nu. Detta görs inte av WPmcbu() eftersom
***vi har satt mwinpt->mapped = FALSE när vi skapade meny-fönstret.
*/
    XMapSubwindows(xdisp,mwinpt->id.x_id);
/*
***Slut.
*/
    return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool WPifae(
        XEvent   *event,
        MNUALT  **altptr)

/*      Testar om ett event skett i något av de
 *      alternativ-fönster som ingår i aktiv meny.
 *
 *      In: event  = Pekare till event.
 *          altptr = Pekare till utdata.
 *
 *      Ut: *altptr = Pekare till utpekat meny-alternativ.
 *
 *      FV: TRUE  => Ja.
 *          FALSE => Nej.
 *
 *      (C)microform ab 17/7/92 J. Kjellander
 *
 ******************************************************!*/

  {
    int  i = 0;
    bool hit = FALSE;

/*
***Om ingen meny finns, returnera FALSE.
*/
    if ( actmeny == NULL ) hit = FALSE;
/*
***Om altptr == NULL är det inte tillåtet att göra
***menyval.
*/
    else if ( altptr == NULL ) hit = FALSE;
/*
***Leta igenom menyns fönsterlista efter träff.
*/
   else
     {
     *altptr = actmeny->alt;
  
      while ( hit == FALSE  &&  i < actmeny->nalt )
        {
        if ( event->xany.window == altid[i++] ) hit = TRUE;
        else *altptr = *altptr + 1;
        }
      }

    return(hit);
  }

/********************************************************/
/*!******************************************************/

        void WPfomw()

/*      Lyfter upp menyfönstret till toppen av fönster-
 *      stacken som ett svar på ett FocusIn-event på ett
 *      grafiskt fönster.
 *
 *      (C)microform ab 1996-02-12 J. Kjellander
 *
 ******************************************************!*/

  {
   WPIWIN *mwinpt;

/*
***Fixa en C-pekare till menyfönstret.
*/
    mwinpt = (WPIWIN *)wpwtab[men_id].ptr;
/*
***Lyft upp.
*/
   XRaiseWindow(xdisp,mwinpt->id.x_id);
  }

/********************************************************/
