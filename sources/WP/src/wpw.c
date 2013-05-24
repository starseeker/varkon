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
*    WPbell();   Rings a bell
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

/* wpwtab �r en tabell med typ och pekare till f�nster.
   Typ �r en kod som anger vilken typ av f�nster det r�r
   sig om tex. TYP_IWIN f�r ett input-f�nster fr�n MBS.
   Pekaren �r en C-pekare som pekar p� en structure av
   den aktuella typen tex. WPIWIN f�r ett input-f�nster.

   Alla element i wpwtab initieras av WPwini() till NULL.
   N�r ett nytt f�nster skapas f�r det som ID l�gsta lediga
   plats i wpwtab och n�r det deletas nollst�lls platsen
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

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPwexp(XExposeEvent *expev)

/*      Expose-rutinen f�r wpw-f�nstren. Letar upp 
 *      r�tt f�nster och anropar dess expose-rutin.
 *
 *      In: expev = Pekare till Expose-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got wpw-f�nster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-04, WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    bool     status;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPLWIN  *lwinpt;
    WPGWIN  *gwinpt;
    WPRWIN  *rwinpt;

/*
***S�k igenom wpwtab och kolla om n�got av f�nstren
***har r�tt x_id. Expose-events kan bara intr�ffa p�
***hela huvudf�nster, ej enskilda sub-f�nster s� vi
***kan redan h�r avg�ra vilket f�nster det r�r sig om.
***F�r ett Button-event tex. m�ste vi g�ra den testen
***p� varje sub-f�nster individuellt !
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
***Flush efter expose g�rs bara h�r.
*/
    if ( status == TRUE ) XFlush(xdisp);

    return(status);
  }

/********************************************************/
/*!******************************************************/

        bool           WPwbut(
        XButtonEvent  *butev,
        wpw_id        *serv_id)

/*      Button-rutinen f�r wpw-f�nstren. Kollar
 *      vilken typ av f�nster det �r och anropar
 *      r�tt rutin f�r jobbet.
 *
 *      In: butev    = Pekare till Button-event.
 *          serv_id  = Pekare till utdata.
 *
 *      Ut: *serv_id = ID f�r subf�nster som servat eventet.
 *
 *      FV. TRUE  = Eventet har servats.
 *          FALSE = Eventet g�llde inga av dessa f�nster.
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
***S�k igenom wpwtab och anropa alla f�nstrens
***respektive butt-hanterare. Den som vill k�nnas vid
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

/*      Crossing-rutinen f�r wpw-f�nstren.
 *
 *      In: croev = Pekare till Crossing-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got av dessa f�nster.
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
***S�k igenom wpwtab och anropa alla f�nstrens
***respektive cro-hanterare. Den som vill k�nnas vid
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

        bool       WPwkey(
        XKeyEvent *keyev,
        int        slevel,
        wpw_id    *serv_id)

/*      Key handler for wpw windows.
 *
 *      In: keyev   = Pekare till Key-event.
 *          slevel  = �nskad service-niv�.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID f�r f�nster som servat eventet.
 *
 *      FV: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got av dessa f�nster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    WPWIN  *winptr;
    WPIWIN *iwinpt;

/*
***Till att b�rja med skall vi avg�ra om det �r en funktions-
***tangent, dvs. ett snabbval eller om det bara �r en vanlig
***tangenttryckning.
*/
   status = WPkepf(keyev);
   if ( status == SMBESCAPE ) return(FALSE);
/*
***S�k igenom wpwtab och leta upp alla WPIWIN-f�nster.
***Om key-eventet har uppst�tt i n�got av dessa, anropa
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

/*      ClientMessage-rutinen f�r wpw-f�nstren. Letar upp 
 *      r�tt f�nster och anropar dess clm-rutin.
 *
 *      In: expev = Pekare till ClientMessage-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got wpw-f�nster.
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
***S�k igenom wpwtab och kolla om n�got av f�nstren
***har r�tt x_id. ClientMessage-event kan bara upptr�da
***p� huvudf�nster, ej subf�nster.
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

/*      Configure Notify-rutinen f�r wpw-f�nstren. Letar upp 
 *      r�tt f�nster och anropar dess configure-rutin.
 *
 *      In: conev = Pekare till configure-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got wpw-f�nster.
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
***S�k igenom wpwtab och kolla om n�got av f�nstren
***har r�tt x_id. Configure-events kan bara intr�ffa p�
***WPGWIN-f�nster.
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

/*      FocusIn-rutinen f�r wpw-f�nster. 
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
***Focus-events kan bara genereras av WPGWIN-f�nster.
***Detta sker tex. om V3 varit t�ckt av andra applikationer
***och man klickar i ett grafiskt f�nster tillh�rande V3.
***V3 skall d� ha keybord focus och bli aktivt. F�r att 
***s�kerst�lla att �ven menyf�nstret blir aktivt g�r vi
***d� raise p� det h�r. Detta inneb�r att det grafiska
***man clickat i samt menyf�nstret kommer upp till toppen.
***�vriga grafiska f�nster kommer inte upp.
*/
    WPfocus_menu();

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPwrep(
        XReparentEvent *repev)

/*      ReparentNotify-rutinen f�r wpw-f�nstren. Letar upp 
 *      r�tt f�nster och anropar dess reparent-rutin.
 *
 *      In: repev = Pekare till configure-event.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i n�got wpw-f�nster.
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
***S�k igenom wpwtab och kolla om n�got av f�nstren
***har r�tt x_id. Reparent-events kan bara intr�ffa p�
***WPGWIN-f�nster.
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

/*      Visar ett f�nster.
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
***Fixa en C-pekare till f�nstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL ) return(-2);
/*
***Vilken typ av f�nster �r det ?
*/
    switch ( winptr->typ )
      {
/*
***WPIWIN-f�nster. Mappa f�nster och subf�nster. Om
***f�nstret inneh�ller WPEDIT:s s�tter vi input-focus
***p� det f�rsta. S�tt mapped = TRUE s� att subf�nster
***som skapas fr�n och med nu mappas direkt n�r dom skapas.
*/
      case TYP_IWIN:
      iwinpt = (WPIWIN *)winptr->ptr;
      xwin_id = iwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapRaised(xdisp,xwin_id);
      edtptr = WPffoc(iwinpt,FIRST_EDIT);
      if ( edtptr != NULL ) WPfoed(edtptr,TRUE);
      iwinpt->mapped = TRUE;
      break;
/*
***WPLWIN-f�nster. Mappa f�nster och subf�nster.
*/
      case TYP_LWIN:
      lwinpt = (WPLWIN *)winptr->ptr;
      xwin_id = lwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      break;
/*
***WPGWIN-f�nster. Mappa f�nster och subf�nster.
*/
      case TYP_GWIN:
      gwinpt = (WPGWIN *)winptr->ptr;
      xwin_id = gwinpt->id.x_id;
      XMapSubwindows(xdisp,xwin_id);
      XMapWindow(xdisp,xwin_id);
      break;
/*
***WPRWIN-f�nster. Mappa f�nster och subf�nster.
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
***Flusha s� att de s�kert syns p� sk�rmen.
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

/*   Event-loop f�r MBS-rutinen WAIT_WIN. L�gger sig
 *   och v�ntar p� events i det WPIWIN-f�nster som angetts.
 *
 *   Denna rutin anv�nds dels av MBS (WAIT_WIN) och dessutom
 *   av WPialt() samt WPmsip(). K�nnetecknande �r att den i
 *   princip bara servar events som kan h�nf�ras till det WPIWIN-
 *   f�nster som angetts som indata. Undantaget �r att den ocks�
 *   servar expose-events p� andra f�nster.
 *
 *   In: iwin_id = ID f�r huvudf�nstret.
 *       slevel  = Service-niv� f�r key-event.
 *       subw_id = Pekare till utdata.
 *
 *   Ut: *subw_id = ID f�r det subf�nster d�r ett event intr�ffat.
 *
 *   Return:       0 = Ok.
 *           SMBPOSM = Pos-button pressed.
 *           WP1202  = iwin_id %s is not a window.
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
    MNUALT              *altptr;

/*
***Kolla att f�nstret finns.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1202",errbuf));
      }
/*
***Om events finns, serva dom. Om inga events finns
***l�gger vi oss och v�ntar.
*/
evloop:
    XNextEvent(xdisp,&event);

    switch ( event.type )
      {
/*
***Expose �r till�tet i alla f�nster.
*/
      case Expose:
      WPwexp((XExposeEvent *)&event);
      goto evloop;
      break;
/*
***KeyPress-events uppst�r i WPIWIN-f�nstret sj�lvt
***men l�nkas vidare till det WPEDIT-f�nster som har
***fokus. WPwkey() returnerar det WPEDIT-f�nster
***som servat eventet om FV=TRUE. Vissa events kan (beroende 
***p� slevel) ibland servas lokalt och WPwkey returnerar d� FALSE.
*/
      case KeyPress:
      par_id = WPwfpx(keyev->window);
      if ( par_id >= 0  &&  WPwkey(keyev,slevel,&serv_id) == TRUE )
        {
       *subw_id = (DBint)serv_id;
        return(0);
        }
      else goto evloop;
/*
***Leave/Enter uppst�r bara i WPBUTT-f�nster. Kolla att
***det g�ller v�rt WPIWIN eller ett WPLWIN. Andra f�nster
***�r inte aktiva nu.
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
***ButtonPress is always associated with a WPRWIN.
***Use WPwfpx() to get the WP-id of the window that
***created the event. WPwfpx() returns the parent
***of the window if it has one or the window itself
***if it has no parent, ie. WPRWIN.
*/
      case ButtonPress:
      par_id = WPwfpx(butev->window);
      if ( par_id < 0 ) goto evloop;
      if ( wpwtab[par_id].typ == TYP_RWIN )
        WPbtrw((WPRWIN *)wpwtab[par_id].ptr,butev,&serv_id);
      goto evloop;
/*
***If it is a ButtonRelease, it may come from a WPLWIN...
*/
      case ButtonRelease:
      par_id = WPwfpx(butev->window);
      if ( par_id < 0 ) goto evloop;

      if ( par_id != iwin_id )
        {
        if ( wpwtab[par_id].typ == TYP_LWIN )
          {
          WPwbut(butev,&serv_id);
          goto evloop;
          }
/*
***...or from a pos-button in the menu window...
*/
        else if ( WPmenu_button(&event,&altptr) && altptr == NULL ) return(SMBPOSM);
/*
***...or some other window where it should not be...
*/
        else
          {
          WPbell();
          goto evloop;
          }
        }
/*
***...but may also come from the WPIWIN we are waiting
***for (or one of it's children). In that case, let WPwbut() take
***care of it. If WPwbut() returns TRUE the event should break
***the loop, otherwise the event was "local" and we go on
***waiting.
*/
      switch ( butev->button )
        {
        case 1:
        if ( WPwbut(butev,&serv_id) == TRUE )
          {
         *subw_id = (DBint)serv_id;
          return(0);
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
***Ok�nd typ av event.
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

/*      D�dar ett huvudf�nster med alla subf�nster.
 *
 *      In: w_id   = Huvudf�nstrets entry i wpwtab.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudf�nstret finns ej.
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
***Fixa en C-pekare till huvud-f�nstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av f�nster �r det ?
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
***D�da f�nstret ur X.
*/
    XDestroyWindow(xdisp,xwin_id);
/*
***Stryk f�nstret ur f�nstertabellen.
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

/*      D�dar ett subf�nster. Klara fn. bara subf�nster
 *      i huvudf�nster av typen WPIWIN och WPGWIN.
 *
 *      In: w_id   = Huvudf�nstrets entry i wpwtab.
 *          sub_id = Subf�nstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudf�nstret finns ej.
 *              WP1232 = Subf�nstret finns ej.
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
***Fixa en C-pekare till huvud-f�nstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av f�nster �r det ?
*/
    switch ( winptr->typ )
      {
/*
***WPIWIN, kolla att subf�nstret finns.
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
***D�da f�nstret ur wpw och ta reda p� X-id.
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
***D�da f�nstret ur X.
*/
      XDestroyWindow(xdisp,xwin_id);
/*
***L�nka bort subf�nstret fr�n WPIWIN-f�nstret.
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
***D�da f�nstret ur wpw och ta reda p� X-id.
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
***D�da f�nstret ur X.
*/
      XDestroyWindow(xdisp,xwin_id);
/*
***L�nka bort subf�nstret fr�n WPIWIN-f�nstret.
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
***D�da alla f�nster i f�nster-tabellen.
*/
   for ( i=0; i<WTABSIZ; ++i)
     if ( wpwtab[i].ptr != NULL ) WPwdel((DBint)i);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        wpw_id WPwffi()

/*      Letar upp l�gsta lediga entry i wpwtab.
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
***Leta upp ledig plats i f�nstertabellen. L�mna ID = 0
***ledigt eftersom detta ID �r reserverat f�r V3:s grafiska
***huvudf�nster.
*/
    i = 1;

    while ( i < WTABSIZ  &&  wpwtab[i].ptr != NULL ) ++i;
/*
***Finns det n�gon ?
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

/*      Letar upp id f�r f�r�ldern till ett sub-
 *      f�nster med visst X-id. Om f�nstret med
 *      det angivna X-id:t �r en f�r�lder returneras
 *      ID f�r f�nstret (f�r�ldern) sj�lvt.
 *
 *      Denna rutin anv�nds av WPwwtw() f�r att avg�ra
 *      om ett X-event har skett i det f�nster som vi
 *      v�ntar p�.
 *
 *      In: x_id  = Subwindow X-id.
 *
 *      Return: Window wpw_id or -1.
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
***S�k igenom hela wpwtab efter f�nster.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
/*
***Vilken typ av f�nster �r det ?
*/
        switch ( wpwtab[i].typ )
          {
/*
***WPIWIN-f�nster. Kolla f�nstret sj�lvt och 
***s�k igenom alla sub-f�nster.
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
***WPLWIN-f�nster. Kolla f�nstret sj�lvt och 
***s�k igenom alla sub-f�nster.
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
***Grafiskt f�nster.
*/
          case TYP_GWIN:
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
          if ( x_id == gwinpt->id.x_id ) return((wpw_id)i);
          else if ( x_id == gwinpt->mcw_ptr->messcom_xid ) return((wpw_id)i);
          else if ( x_id == gwinpt->mcw_ptr->resize_xid ) return((wpw_id)i);
          break;
/*
***OpenGL f�nster.
*/
          case TYP_RWIN:
          rwinpt = (WPRWIN *)wpwtab[i].ptr;
          if ( rwinpt->id.x_id == x_id ) return((wpw_id)i);
          break;
          }
        }
     }
/*
***Ingen tr�ff.
*/
    return((wpw_id)-1);
  }

/********************************************************/
/*!******************************************************/

        WPWIN *WPwgwp(
        wpw_id id)

/*      �vers�tter id till C-pekare f�r motsvarande entry
 *      i wpwtab.
 *
 *      In: id = F�nstrets entry i wpwtab.
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
***�r det ett giltigt ID ?
*/
    if ( id < 0  ||  id >= WTABSIZ ) return(NULL);
/*
***Ja, returnera pekare om det finns n�gon.
*/
    else
      {
      if ( wpwtab[id].ptr != NULL ) return(&wpwtab[id]);
      else return(NULL);
      }
  }

/********************************************************/
/*!******************************************************/

        void WPbell()

/*      Rings a bell.
 *
 *      (C)2007-01-06 J. Kjellander
 *
 ******************************************************!*/

  {
#ifdef UNIX
    XBell(xdisp,100);
#endif
  }

/********************************************************/
