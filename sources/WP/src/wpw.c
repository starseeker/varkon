/**********************************************************************
*
*    wpw.c
*    =====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPwini();   Init WP internals (wpw)
*   
*    WPwexp();   Expose routine for wpw-window
*    WPwbut();   Button routine for wpw-window
*    WPwcro();   Crossing routine for wpw-window
*    WPwkey();   Key routine for wpw-window
*    WPwclm();   ClientMessage routine for wpw-window
*    WPwrep();   Reparent routine for wpw-window
*    WPwcon();   Configure routine for wpw-window
*    WPwfoc();   FocusIn routine for wpw-window
*   
*    WPwshw();   Maps window, SHOW_WIN in MBS
*    WPwwtw();   Event-loop, WAIT_WIN in MBS
*    WPwdel();   Kill main window, DEL_WIN in MBS
*    WPwdls();   Kill subwindow, DEL_WIN in MBS
*    WPwexi();   Exit WP
*   
*    WPwffi();   Returns free index in wpwtab
*    WPwfpx();   Returns WP-ID for parent window by child X-ID
*   *WPwgwp();   Returnc C-pointer to index in wpwtab
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

WPWIN wpwtab[WTABSIZ];

/* wpwtab är en tabell med typ och pekare till fönster.
   Typ är en kod som anger vilken typ av fönster det rör
   sig om tex. TYP_IWIN för ett input-fönster från MBS.
   Pekaren är en C-pekare som pekar på en structure av
   den aktuella typen tex. WPIWIN för ett input-fönster.

   Alla element i wpwtab initieras av WPwini() till NULL.
   När ett nytt fönster skapas får det som ID lägsta lediga
   plats i wpwtab och när det deletas nollställs platsen
   igen.
*/

/*!******************************************************/

        short WPwini()

/*      Init WP.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***Init window table.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     wpwtab[i].typ = TYP_UNDEF;
     wpwtab[i].ptr = NULL;
     }
/*
***Create text-cursor.
*/
   xtcur = XCreateFontCursor(xdisp,152);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPwexp(
        XExposeEvent *expev)

/*      Expose-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess expose-rutin.
 *
 *      In: expev = Pekare till Expose-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något wpw-fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-04, WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    bool    status;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPLWIN *lwinpt;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och kolla om något av fönstren
***har rätt x_id. Expose-events kan bara inträffa på
***hela huvudfönster, ej enskilda sub-fönster så vi
***kan redan här avgöra vilket fönster det rör sig om.
***För ett Button-event tex. måste vi göra den testen
***på varje sub-fönster individuellt !
*/
    status = FALSE;

    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( iwinpt->id.x_id == expev->window  &&  expev->count == 0 )
            {
            WPxpiw(iwinpt);
            status = TRUE;
            }
          break;

          case TYP_LWIN:
          lwinpt = (WPLWIN *)winptr->ptr;
          if ( lwinpt->id.x_id == expev->window  &&  expev->count == 0 )
            {
            WPxplw(lwinpt);
            status = TRUE;
            }
          break;

          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( gwinpt->id.x_id == expev->window )
            {
            WPxpgw(gwinpt,expev);
            status = TRUE;
            }
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( rwinpt->id.x_id == expev->window )
            {
            WPxprw(rwinpt,expev);
            status = TRUE;
            }
          break;
          }
        }
      }
/*
***Flush efter expose görs bara här.
*/
    if ( status == TRUE ) XFlush(xdisp);

    return(status);
  }

/********************************************************/
/*!******************************************************/

        bool WPwbut(
        XButtonEvent  *butev,
        wpw_id        *serv_id)

/*      Button-rutinen för wpw-fönstren. Kollar
 *      vilken typ av fönster det är och anropar
 *      rätt rutin för jobbet.
 *
 *      In: butev    = Pekare till Button-event.
 *          serv_id  = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      FV. TRUE  = Eventet har servats.
 *          FALSE = Eventet gällde inga av dessa fönster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-09 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPLWIN *lwinpt;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och anropa alla fönstrens
***respektive butt-hanterare. Den som vill kännas vid
***eventet tar hand om det.
*/
    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( WPbtiw(iwinpt,butev,serv_id) ) return(TRUE);
          break;

          case TYP_LWIN:
          lwinpt = (WPLWIN *)winptr->ptr;
          if ( WPbtlw(lwinpt,butev,serv_id) ) return(TRUE);
          break;

          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( WPbtgw(gwinpt,butev,serv_id) ) return(TRUE);
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( WPbtrw(rwinpt,butev,serv_id) ) return(TRUE);
          break;
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwcro(
        XCrossingEvent *croev)

/*      Crossing-rutinen för wpw-fönstren.
 *
 *      In: croev = Pekare till Crossing-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något av dessa fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-09 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPLWIN *lwinpt;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och anropa alla fönstrens
***respektive cro-hanterare. Den som vill kännas vid
***eventet tar hand om det.
*/
    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( WPcriw(iwinpt,croev) ) return(TRUE);
          break;

          case TYP_LWIN:
          lwinpt = (WPLWIN *)winptr->ptr;
          if ( WPcrlw(lwinpt,croev) ) return(TRUE);
          break;

          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( WPcrgw(gwinpt,croev) ) return(TRUE);
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( WPcrrw(rwinpt,croev) ) return(TRUE);
          break;
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwkey(
        XKeyEvent *keyev,
        int        slevel,
        wpw_id    *serv_id)

/*      Key handler for wpw windows.
 *
 *      In: keyev   = Pekare till Key-event.
 *          slevel  = Önskad service-nivå.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för fönster som servat eventet.
 *
 *      FV: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något av dessa fönster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    WPWIN  *winptr;
    WPIWIN *iwinpt;

/*
***Till att börja med skall vi avgöra om det är en funktions-
***tangent, dvs. ett snabbval eller om det bara är en vanlig
***tangenttryckning.
*/
   status = WPkepf(keyev);
   if ( status == SMBESCAPE ) return(FALSE);
/*
***Sök igenom wpwtab och leta upp alla WPIWIN-fönster.
***Om key-eventet har uppstått i något av dessa, anropa
***dess key-hanterare.
*/
    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( keyev->window == iwinpt->id.x_id )
            {
            if ( WPkeiw(iwinpt,keyev,slevel,serv_id) ) return(TRUE);
            }
          break;
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwclm(
        XClientMessageEvent *clmev)

/*      ClientMessage-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess clm-rutin.
 *
 *      In: expev = Pekare till ClientMessage-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något wpw-fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 4/1/94 J. Kjellander
 *
 *      1998-01-04 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPLWIN *lwinpt;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och kolla om något av fönstren
***har rätt x_id. ClientMessage-event kan bara uppträda
***på huvudfönster, ej subfönster.
*/
    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( iwinpt->id.x_id == clmev->window )
            {
            WPcmiw(iwinpt,clmev);
            return(TRUE);
            }
          break;

          case TYP_LWIN:
          lwinpt = (WPLWIN *)winptr->ptr;
          if ( lwinpt->id.x_id == clmev->window )
            {
            WPcmlw(lwinpt,clmev);
            return(TRUE);
            }
          break;

          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( gwinpt->id.x_id == clmev->window )
            {
            WPcmgw(gwinpt,clmev);
            return(TRUE);
            }
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( rwinpt->id.x_id == clmev->window )
            {
            WPcmrw(rwinpt,clmev);
            return(TRUE);
            }
          break;
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwcon(
        XConfigureEvent *conev)

/*      Configure Notify-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess configure-rutin.
 *
 *      In: conev = Pekare till configure-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något wpw-fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 8/2/94 J. Kjellander
 *
 *      1998-10-29 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    bool    status;
    WPWIN  *winptr;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och kolla om något av fönstren
***har rätt x_id. Configure-events kan bara inträffa på
***WPGWIN-fönster.
*/
    status = FALSE;

    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( gwinpt->id.x_id == conev->window )
            {
            WPcogw(gwinpt,conev);
            status = TRUE;
            }
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( rwinpt->id.x_id == conev->window )
            {
            WPcorw(rwinpt,conev);
            status = TRUE;
            }
          break;
          }
        }
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        bool WPwfoc(
        XFocusInEvent *focev)

/*      FocusIn-rutinen för wpw-fönster. 
 *
 *      In: focev = Pekare till FocusIn-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *
 *      (C)microform ab 1996-02-12 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Focus-events kan bara genereras av WPGWIN-fönster.
***Detta sker tex. om V3 varit täckt av andra applikationer
***och man klickar i ett grafiskt fönster tillhörande V3.
***V3 skall då ha keybord focus och bli aktivt. För att 
***säkerställa att även menyfönstret blir aktivt gör vi
***då raise på det här. Detta innebär att det grafiska
***man clickat i samt menyfönstret kommer upp till toppen.
***Övriga grafiska fönster kommer inte upp.
*/
    WPfomw();

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwrep(
        XReparentEvent *repev)

/*      ReparentNotify-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess reparent-rutin.
 *
 *      In: repev = Pekare till configure-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något wpw-fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 31/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    bool    status;
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Sök igenom wpwtab och kolla om något av fönstren
***har rätt x_id. Reparent-events kan bara inträffa på
***WPGWIN-fönster.
*/
    status = FALSE;

    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( gwinpt->id.x_id == repev->window )
            {
            WPrpgw(gwinpt,repev);
            status = TRUE;
            }
          break;
          }
        }
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short WPwshw(
        DBint w_id)

/*      Visar ett fönster.
 *
 *      In: w_id  = Entry i wpwtab.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-04 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    Window   xwin_id;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPLWIN  *lwinpt;
    WPEDIT  *edtptr;
    WPGWIN  *gwinpt;
    WPRWIN  *rwinpt;

/*
***Fixa en C-pekare till fönstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL ) return(-2);
/*
***Vilken typ av fönster är det ?
*/
    switch ( winptr->typ )
      {
/*
***WPIWIN-fönster. Mappa fönster och subfönster. Om
***fönstret innehåller WPEDIT:s sätter vi input-focus
***på det första. Sätt mapped = TRUE så att subfönster
***som skapas från och med nu mappas direkt när dom skapas.
*/
      case TYP_IWIN:
      iwinpt = (WPIWIN *)winptr->ptr;
      xwin_id = iwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      edtptr = WPffoc(iwinpt,FIRST_EDIT);
      if ( edtptr != NULL ) WPfoed(edtptr,TRUE);
      iwinpt->mapped = TRUE;
      break;
/*
***WPLWIN-fönster. Mappa fönster och subfönster.
*/
      case TYP_LWIN:
      lwinpt = (WPLWIN *)winptr->ptr;
      xwin_id = lwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      break;
/*
***WPGWIN-fönster. Mappa fönster och subfönster.
*/
      case TYP_GWIN:
      gwinpt = (WPGWIN *)winptr->ptr;
      xwin_id = gwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      break;
/*
***WPRWIN-fönster. Mappa fönster och subfönster.
*/
      case TYP_RWIN:
      rwinpt = (WPRWIN *)winptr->ptr;
      xwin_id = rwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      break;

      default:
      return(-2);
      }
/*
***Flusha så att de säkert syns på skärmen.
*/
    XFlush(xdisp);

    return(0);
  }

/********************************************************/
/*!*******************************************************/

     short WPwwtw(
     DBint  iwin_id,
     DBint  slevel,
     DBint *subw_id)

/*   Event-loop för MBS-rutinen WAIT_WIN. Lägger sig
 *   och väntar på events i det WPIWIN-fönster som angetts.
 *
 *   Denna rutin används dels av MBS (WAIT_WIN) och dessutom
 *   av WPialt() samt WPmsip(). Kännetecknande är att den i
 *   princip bara servar events som kan hänföras till det WPIWIN-
 *   fönster som angetts som indata. Undantaget är att den också
 *   servar expose-events på andra fönster.
 *
 *   In: iwin_id = ID för huvudfönstret.
 *       slevel  = Service-nivå för key-event.
 *       subw_id = Pekare till utdata.
 *
 *   Ut: *subw_id = ID för det subfönster där ett event inträffat.
 *
 *   Felkoder : WP1202 = iwin_id %s är ej ett fönster
 *
 *   (C)microform ab 8/12/93 J. Kjellander
 *
 *   2006-12-11 ButtonPress->Release, J.Kjellander
 *
 *******************************************************!*/

 {
    char                 errbuf[80];
    wpw_id               par_id,serv_id;
    XEvent               event;
    XButtonEvent        *butev = (XButtonEvent *) &event;
    XCrossingEvent      *croev = (XCrossingEvent *) &event;
    XKeyEvent           *keyev = (XKeyEvent *) &event;
    XClientMessageEvent *clmev = (XClientMessageEvent *) &event;
    WPWIN               *winptr;

/*
***Kolla att fönstret finns.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1202",errbuf));
      }
/*
***Om events finns, serva dom. Om inga events finns
***lägger vi oss och väntar.
*/
evloop:
    XNextEvent(xdisp,&event);

    switch ( event.type )
      {
/*
***Expose är tillåtet i alla fönster.
*/
      case Expose:
      WPwexp((XExposeEvent *)&event);
      goto evloop;
      break;
/*
***KeyPress-events uppstår i WPIWIN-fönstret självt
***men länkas vidare till det WPEDIT-fönster som har
***fokus. WPwkey() returnerar det WPEDIT-fönster
***som servat eventet om FV=TRUE. Vissa events kan (beroende 
***på slevel) ibland servas lokalt och WPwkey returnerar då FALSE.
*/
      case KeyPress:
      par_id = WPwfpx(keyev->window);
      if ( par_id >= 0  &&  WPwkey(keyev,slevel,&serv_id) == TRUE )
        {
       *subw_id = (DBint)serv_id;
        return(TRUE);
        }
      else goto evloop;
/*
***Leave/Enter uppstår bara i WPBUTT-fönster. Kolla att
***det gäller vårt WPIWIN eller ett WPLWIN. Andra fönster
***är inte aktiva nu.
*/
      case EnterNotify:
      case LeaveNotify:
      par_id = WPwfpx(croev->window);
      if ( par_id >= 0 )
        {
        if      ( par_id == iwin_id ) WPwcro(croev);
        else if ( wpwtab[par_id].typ == TYP_LWIN ) WPwcro(croev);
        }
      goto evloop;
/*
***Musknapp-events uppstår i WPBUTT-, WPEDIT-,WPICON- eller WPLWIN.
***Vi skall dock bara serva sådana vars förälder är vårt
***WPIWIN-fönster. Detta kollas genom att jämföra click-fönstrets
***föräldra-ID med WPIWIN-fönstrets. Om de är olika beror det
***antingen på att vi har flera WPIWIN-fönster på skärmen
***samtidigt eller att clickningen skedde i ett fönster som
***inte är WPIWIN. Sådana clickningar är i princip förbjudna
***bortsett från clickningar i WPLWIN-fönster.
*/
      case ButtonPress:
      goto evloop;
      
      case ButtonRelease:
      par_id = WPwfpx(butev->window);
      if ( par_id < 0 ) goto evloop;

      if ( par_id != iwin_id )
        {
        if ( wpwtab[par_id].typ == TYP_LWIN ) WPwbut(butev,&serv_id);
        goto evloop;
        }
/*
***Clickningen har skett i vårt WPIWIN-fönster eller något
***av dess subfönster. Om WPwbut() returnerar TRUE är detta
***en händelse som skall bryta event-loopen och föras tillbaks
***till anropande rutin (MBS-program). Om WPwbut() returnerar 
***FALSE har eventet servats men ansetts som "lokalt", tex.
***cursor-positionering i WPEDIT-fönster.
*/
      switch ( butev->button )
        {
        case 1:
        if ( WPwbut(butev,&serv_id) == TRUE )
          {
         *subw_id = (DBint)serv_id;
          return(TRUE);
          }
        else goto evloop;

        case 2:
        case 3:
        XBell(xdisp,100);
        goto evloop;
        }
/*
***ClientMessage-event.
*/
      case ClientMessage:
      WPwclm(clmev);
      goto evloop;
/*
***Okänd typ av event.
*/
      default:
      goto evloop;
      }
/*
***Slut.
*/
   return(0);
 }

/*********************************************************/
/*!******************************************************/

        short WPwdel(
        DBint w_id)

/*      Dödar ett huvudfönster med alla subfönster.
 *
 *      In: w_id   = Huvudfönstrets entry i wpwtab.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudfönstret finns ej.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-04 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    char     errbuf[80];
    Window   xwin_id=0;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPLWIN  *lwinpt;
    WPGWIN  *gwinpt;
    WPRWIN  *rwinpt;

/*
***Fixa en C-pekare till huvud-fönstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av fönster är det ?
*/
    switch ( winptr->typ )
      {
      case TYP_IWIN:
      iwinpt = (WPIWIN *)winptr->ptr;
      xwin_id = iwinpt->id.x_id;
      WPdliw(iwinpt);
      break;

      case TYP_LWIN:
      lwinpt = (WPLWIN *)winptr->ptr;
      xwin_id = lwinpt->id.x_id;
      WPdllw(lwinpt);
      break;

      case TYP_GWIN:
      gwinpt = (WPGWIN *)winptr->ptr;
      xwin_id = gwinpt->id.x_id;
      WPdlgw(gwinpt);
      break;

      case TYP_RWIN:
      rwinpt = (WPRWIN *)winptr->ptr;
      xwin_id = rwinpt->id.x_id;
      WPdlrw(rwinpt);
      break;
      }
/*
***Döda fönstret ur X.
*/
    XDestroyWindow(xdisp,xwin_id);
/*
***Stryk fönstret ur fönstertabellen.
*/
/* Följande rad skall inte finnas !!! BUG. Borttagen 23/11/95 JK 
   Det minne som winptr->ptr pekar på är visserligen dynamiskt
   allokerat men deallokeras av respektive destruktor-rutin med
   v3free() !!!  free() får inte användas överhuvudtaget.
    free(winptr->ptr);
*/
    winptr->typ = TYP_UNDEF;
    winptr->ptr = NULL;
   
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPwdls(
        DBint w_id,
        DBint sub_id)

/*      Dödar ett subfönster. Klara fn. bara subfönster
 *      i huvudfönster av typen WPIWIN och WPGWIN.
 *
 *      In: w_id   = Huvudfönstrets entry i wpwtab.
 *          sub_id = Subfönstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudfönstret finns ej.
 *              WP1232 = Subfönstret finns ej.
 *
 *      (C)microform ab 17/1/94 J. Kjellander
 *
 *      1996-05-20 WPGWIN, J. Kjellander
 *
 ******************************************************!*/

  {
    char     errbuf[80];
    char    *subptr;
    Window   xwin_id=0;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPGWIN  *gwinpt;
    WPBUTT  *butptr;
    WPEDIT  *edtptr;
    WPICON  *icoptr;

/*
***Fixa en C-pekare till huvud-fönstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av fönster är det ?
*/
    switch ( winptr->typ )
      {
/*
***WPIWIN, kolla att subfönstret finns.
*/
      case TYP_IWIN:
      if ( sub_id < 0  ||  sub_id > WP_IWSMAX-1 )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
      iwinpt = (WPIWIN *)winptr->ptr;
      subptr = iwinpt->wintab[(wpw_id)sub_id].ptr;
      if ( subptr == NULL )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
/*
***Döda fönstret ur wpw och ta reda på X-id.
*/
      switch ( iwinpt->wintab[(wpw_id)sub_id].typ )
        {
        case TYP_BUTTON:
        butptr = (WPBUTT *)subptr;
        xwin_id = butptr->id.x_id;
        WPdlbu(butptr);
        break;

        case TYP_EDIT:
        edtptr = (WPEDIT *)subptr;
        xwin_id = edtptr->id.x_id;
        WPdled(edtptr);
        break;

        case TYP_ICON:
        icoptr = (WPICON *)subptr;
        xwin_id = icoptr->id.x_id;
        WPdlic(icoptr);
        break;
        }
/*
***Döda fönstret ur X.
*/
      XDestroyWindow(xdisp,xwin_id);
/*
***Länka bort subfönstret från WPIWIN-fönstret.
*/
      iwinpt->wintab[(wpw_id)sub_id].ptr = NULL;
      iwinpt->wintab[(wpw_id)sub_id].typ = TYP_UNDEF;
      break;
/*
***WPGWIN.
*/
      case TYP_GWIN:
      if ( sub_id < 0  ||  sub_id > WP_GWSMAX-1 )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
      gwinpt = (WPGWIN *)winptr->ptr;
      subptr = gwinpt->wintab[(wpw_id)sub_id].ptr;
      if ( subptr == NULL )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
/*
***Döda fönstret ur wpw och ta reda på X-id.
*/
      switch ( gwinpt->wintab[(wpw_id)sub_id].typ )
        {
        case TYP_BUTTON:
        butptr = (WPBUTT *)subptr;
        xwin_id = butptr->id.x_id;
        WPdlbu(butptr);
        break;

        case TYP_EDIT:
        edtptr = (WPEDIT *)subptr;
        xwin_id = edtptr->id.x_id;
        WPdled(edtptr);
        break;

        case TYP_ICON:
        icoptr = (WPICON *)subptr;
        xwin_id = icoptr->id.x_id;
        WPdlic(icoptr);
        break;
        }
/*
***Döda fönstret ur X.
*/
      XDestroyWindow(xdisp,xwin_id);
/*
***Länka bort subfönstret från WPIWIN-fönstret.
*/
      gwinpt->wintab[(wpw_id)sub_id].ptr = NULL;
      gwinpt->wintab[(wpw_id)sub_id].typ = TYP_UNDEF;
      break;
      }
   
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPwexi()

/*      Avslutar wpw-paketet.
 *
 *      In: Inget.   
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short i;

/*
***Döda alla fönster i fönster-tabellen.
*/
   for ( i=0; i<WTABSIZ; ++i)
     if ( wpwtab[i].ptr != NULL ) WPwdel((DBint)i);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        wpw_id WPwffi()

/*      Letar upp lägsta lediga entry i wpwtab.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Giltigt ID eller erpush().
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    int i;
/*
***Leta upp ledig plats i fönstertabellen. Lämna ID = 0
***ledigt eftersom detta ID är reserverat för V3:s grafiska
***huvudfönster.
*/
    i = 1;

    while ( i < WTABSIZ  &&  wpwtab[i].ptr != NULL ) ++i;
/*
***Finns det någon ?
*/
    if ( i == WTABSIZ ) return(-2);
/*
***Ja, returnera ID.
*/
    else return((wpw_id)i);
  }

/********************************************************/
/*!******************************************************/

        wpw_id WPwfpx(
        Window   x_id)

/*      Letar upp id för föräldern till ett sub-
 *      fönster med visst X-id. Om fönstret med
 *      det angivna X-id:t är en förälder returneras
 *      ID för fönstret (föräldern) självt.
 *
 *      Denna rutin används av WPwwtw() för att avgöra
 *      om ett X-event har skett i det fönster som vi
 *      väntar på.
 *
 *      In: x_id  = Subfönstrets X-id.
 *
 *      Ut: Inget.   
 *
 *      FV: id för fönster eller -1.
 *
 *      (C)microform ab 15/12/93 J. Kjellander
 *
 *      1998-03-27 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    short    i,j;
    WPIWIN  *iwinpt;
    WPLWIN  *lwinpt;
    WPGWIN  *gwinpt;
    WPRWIN  *rwinpt;
    WPBUTT  *buttpt;
    WPEDIT  *edtptr;
    WPICON  *icoptr;

/*
***Sök igenom hela wpwtab efter fönster.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
/*
***Vilken typ av fönster är det ?
*/
        switch ( wpwtab[i].typ )
          {
/*
***WPIWIN-fönster. Kolla fönstret självt och 
***sök igenom alla sub-fönster.
*/
          case TYP_IWIN:
          iwinpt = (WPIWIN *)wpwtab[i].ptr;
          if ( iwinpt->id.x_id == x_id ) return((wpw_id)i);

          for ( j=0; j<WP_IWSMAX; ++j )
            {
            if ( iwinpt->wintab[j].ptr != NULL )
              {
              switch ( iwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)iwinpt->wintab[j].ptr;
                if ( buttpt->id.x_id == x_id ) return((wpw_id)i);
                break;

                case TYP_EDIT:
                edtptr = (WPEDIT *)iwinpt->wintab[j].ptr;
                if ( edtptr->id.x_id == x_id ) return((wpw_id)i);
                break;

                case TYP_ICON:
                icoptr = (WPICON *)iwinpt->wintab[j].ptr;
                if ( icoptr->id.x_id == x_id ) return((wpw_id)i);
                break;
                }
              }
            }
          break;
/*
***WPLWIN-fönster. Kolla fönstret självt och 
***sök igenom alla sub-fönster.
*/
          case TYP_LWIN:
          lwinpt = (WPLWIN *)wpwtab[i].ptr;
          if ( lwinpt->id.x_id == x_id ) return((wpw_id)i);

          for ( j=0; j<WP_LWSMAX; ++j )
            {
            if ( lwinpt->wintab[j].ptr != NULL )
              {
              switch ( lwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)lwinpt->wintab[j].ptr;
                if ( buttpt->id.x_id == x_id ) return((wpw_id)i);
                break;
                }
              }
            }
          break;
/*
***Grafiskt fönster.
*/
          case TYP_GWIN:
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
          if ( gwinpt->id.x_id == x_id ) return((wpw_id)i);
          break;
/*
***OpenGL fönster.
*/
          case TYP_RWIN:
          rwinpt = (WPRWIN *)wpwtab[i].ptr;
          if ( rwinpt->id.x_id == x_id ) return((wpw_id)i);
          break;
          }
        }
     }
/*
***Ingen träff.
*/
    return((wpw_id)-1);
  }

/********************************************************/
/*!******************************************************/

        WPWIN *WPwgwp(
        wpw_id id)

/*      Översätter id till C-pekare för motsvarande entry
 *      i wpwtab.
 *
 *      In: id = Fönstrets entry i wpwtab.
 *
 *      Ut: Inget.
 *
 *      FV: Giltig C-pekare eller NULL.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Är det ett giltigt ID ?
*/
    if ( id < 0  ||  id >= WTABSIZ ) return(NULL);
/*
***Ja, returnera pekare om det finns någon.
*/
    else
      {
      if ( wpwtab[id].ptr != NULL ) return(&wpwtab[id]);
      else return(NULL);
      }
  }

/********************************************************/
