/**********************************************************************
*
*    wpIWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPwciw();      Create WPIWIN, CRE_WIN in MBS
*    WPxpiw();      Expose routine for WPIWIN
*    WPbtiw();      Button routine for WPIWIN
*    WPcriw();      Crossing routine for WPIWIN 
*    WPkeiw();      Key routine for WPIWIN
*    WPcmiw();      ClientMessage routine for WPIWIN
*    WPdliw();      Kills WPIWIN 
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

extern Window xgwin;

/*!******************************************************/

        short WPwciw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        DBint  *id)

/*      Skapar WPIWIN-fönster.
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
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel från malloc().
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *       2006-12-10 XSetWMNormalHints(), J.Kjellander
 *
 ******************************************************!*/

  {
    short                i;
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    XSizeHints          *xhints;
    Window               xwin_id;
    WPIWIN              *iwinptr;

/*
***Skapa ett ledigt fönster-ID.
*/
    if ( (*id=WPwffi()) < 0 ) return(erpush("WP1052",label));
/*
***Sätt färg mm.
*/
    xwina.background_pixel  = WPgcol(WP_BGND);
    xwina.border_pixel      = BlackPixel(xdisp,xscr);
    xwina.override_redirect = False;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  
/*
***Skapa ett popup-fönster med 1 pixels ram.
*/
    xwin_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),x,y,dx,dy,1,
                            CopyFromParent,InputOutput,
                            CopyFromParent,xwinm,&xwina);

    xhints = XAllocSizeHints();
    xhints->flags      = USPosition | USSize | PMinSize | PMaxSize; 
    xhints->x          = x;
    xhints->y          = y;
    xhints->min_width  = xhints->max_width  = xhints->width  = dx;
    xhints->min_height = xhints->max_height = xhints->height = dy; 
    XSetWMNormalHints(xdisp,xwin_id,xhints);
    XFree(xhints);

    WPmaps(label); 
    XStoreName(xdisp,xwin_id,label);   
    XSetTransientForHint(xdisp,xwin_id,xgwin);
    XSelectInput(xdisp,xwin_id,ExposureMask | KeyPressMask);
/*
***WPIWIN får ej dödas av en WINDOW-Manager som tex. Motif.
*/
    WPsdpr(xwin_id);
/*
***Skapa ett WPIWIN.
*/
    if ( (iwinptr=(WPIWIN *)v3mall(sizeof(WPIWIN),"WPwciw")) == NULL )
      return(erpush("WP1062",label));

    iwinptr->id.w_id = *id;
    iwinptr->id.p_id =  (wpw_id)NULL;
    iwinptr->id.x_id =  xwin_id;

    iwinptr->geo.x =  x;
    iwinptr->geo.y =  y;
    iwinptr->geo.dx =  dx;
    iwinptr->geo.dy =  dy;

    iwinptr->mapped =  FALSE;

    for ( i=0; i<WP_IWSMAX; ++i) iwinptr->wintab[i].ptr = NULL;
/*
***Lagra fönstret i fönstertabellen.
*/
    wpwtab[*id].typ = TYP_IWIN;
    wpwtab[*id].ptr = (char *)iwinptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPxpiw(
        WPIWIN *iwinptr)

/*      Expose-rutin för WPIWIN med vidhängande sub-fönster.
 *      Denna event-rutin servar alltid eventet. Anropande
 *      rutin har redan avgjort att Expose skall göras just
 *      på detta fönster.
 *
 *      In: iwinptr = C-pekare till WPIWIN.
 *
 *      Ut: Alltid TRUE.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;

/*
***Först expose på alla sub-fönster.
*/
    for ( i=0; i<WP_IWSMAX; ++i )
      {
      subptr = iwinptr->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( iwinptr->wintab[i].typ )
          {
          case TYP_EDIT:
          WPxped((WPEDIT *)subptr);
          break;

          case TYP_BUTTON:
          WPxpbu((WPBUTT *)subptr);
          break;

          case TYP_ICON:
          WPxpic((WPICON *)subptr);
          break;
          }
        }
      }
/*
***WPIWIN-fönstret självt har inga texter etc.
***att göra expose på !
*/

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool  WPbtiw(
        WPIWIN       *iwinptr,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin för WPIWIN med vidhängande sub-fönster.
 *      Kollar om muspekning skett i något av WPIWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: iwinptr = C-pekare till WPIWIN.
 *          butev   = X-but event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    WPBUTT *butptr;
    WPEDIT *edtptr,*focptr;
    WPICON *icoptr;

/*
***WPIWIN självt kan inte generera ButtonEvent:s,
***bara sub-fönstren.
*/
    for ( i=0; i<WP_IWSMAX; ++i )
      {
      subptr = iwinptr->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( iwinptr->wintab[i].typ )
          {
          case TYP_BUTTON:
          butptr = (WPBUTT *)subptr;
          if ( butev->window == butptr->id.x_id )
            {
            WPbtbu(butptr);
           *serv_id = butptr->id.w_id;
            return(TRUE);
            }
          break;

          case TYP_EDIT:
          edtptr = (WPEDIT *)subptr;
          if ( butev->window == edtptr->id.x_id )
            {
            focptr = WPffoc(iwinptr,FOCUS_EDIT);
           *serv_id = focptr->id.w_id;
            return(WPbted(edtptr,butev));
            }
          break;

          case TYP_ICON:
          icoptr = (WPICON *)subptr;
          if ( butev->window == icoptr->id.x_id )
            {
            WPbtic(icoptr);
           *serv_id = icoptr->id.w_id;
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

        bool WPcriw(
        WPIWIN         *iwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPIWIN med vidhängande sub-fönster.
 *      Kollar om Leave/Enter skett i något av WPIWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: iwinpt = C-pekare till WPIWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    WPBUTT *butptr;
    WPEDIT *edtptr;
    WPICON *icoptr;

/*
***Gå igenom alla sub-fönster.
*/
    for ( i=0; i<WP_IWSMAX; ++i )
      {
      subptr = iwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( iwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          butptr = (WPBUTT *) subptr;
          if ( croev->window == butptr->id.x_id )
            {
            if ( croev->type == EnterNotify ) WPcrbu(butptr,TRUE);
            else WPcrbu(butptr,FALSE);
            return(TRUE);
            }
          else if ( croev->type == EnterNotify  &&  butptr->hlight == TRUE )
            WPcrbu(butptr,FALSE);
          break;

          case TYP_EDIT:
          edtptr = (WPEDIT *) subptr;
          if ( croev->window == edtptr->id.x_id )
            {
            WPcred(edtptr,croev);
            return(TRUE);
            }
          break;

          case TYP_ICON:
          icoptr = (WPICON *) subptr;
          if ( croev->window == icoptr->id.x_id )
            {
            WPcric(icoptr,croev);
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

        bool WPkeiw(
        WPIWIN     *iwinpt,
        XKeyEvent  *keyev,
        int         slevel,
        wpw_id     *serv_id)

/*      Key-rutin för WPIWIN med vidhängande sub-fönster.
 *      Om WPIWIN-fönstret har ett subfönster (WPEDIT) med
 *      input-focus = TRUE servas eventet.
 *
 *      In: iwinpt  = C-pekare till WPIWIN.
 *          keyev   = X-Key event.
 *          slevel  = Service-nivå.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för WPEDIT som servat eventet.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    WPEDIT *edtptr;

/*
***Finns sub-fönster med focus ?.
*/
    edtptr = WPffoc(iwinpt,FOCUS_EDIT);
/*
***Om ja, prova att serva eventet. Vissa tangenttryckningar
***servas "lokal" utan att WPkeed returnerar TRUE. Exempel
***på detta är när man flyttar cursorn med piltangenterna till
***höger eller vänster. 
*/
    if ( edtptr != NULL )
      {
      if ( WPkeed(edtptr,keyev,slevel) )
        {
       *serv_id = edtptr->id.w_id;
        return(TRUE);
        }
      }
/*
***Kommer vi hit beror det antingen på att inget WPEDIT-fönster
***har fokus just nu eller att det som har fokus inte anser att
***key-eventet är av den typ som skall signaleras uppåt och få
***WPwwtw() att göra return. Tex. piltangenter i sidled.
*/
   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcmiw(
        WPIWIN               *iwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen för WPIWIN.
 *
 *      In: iwinpt  = C-pekare till WPIWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***IWIN-fönster kan normalt inte ta hand om ClientMessage.
***Om det är WM_DELETE_WINDOW servar vi genom att pipa lite.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     XBell(xdisp,100);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short WPdliw(
        WPIWIN *iwinptr)

/*      Dödar en WPIWIN med vidhängande sub-fönster.
 *
 *      In: iwinptr = C-pekare till WPIWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;

/*
***Döda alla sub-fönster.
*/
    for ( i=0; i<WP_IWSMAX; ++i )
      {
      subptr = iwinptr->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( iwinptr->wintab[i].typ )
          {
          case TYP_EDIT:
          WPdled((WPEDIT *)subptr);
          break;

          case TYP_BUTTON:
          WPdlbu((WPBUTT *)subptr);
          break;

          case TYP_ICON:
          WPdlic((WPICON *)subptr);
          break;
          }
        }
      }
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)iwinptr,"WPdliw");
 
    return(0);
  }

/********************************************************/
