/**********************************************************************
*
*    wpIWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
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

/*!******************************************************/

        short WPwciw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        DBint  *id)

/*      Create WPIWIN window.
 *
 *      In: x     = L�ge i X-led.
 *          y     = L�ge i Y-led.
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = F�nstertitel.
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel fr�n malloc().
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *       2006-12-10 XSetWMNormalHints(), J.Kjellander
 *       2007-10-28 Slidebars, J.Kjellander
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
***Skapa ett ledigt f�nster-ID.
*/
    if ( (*id=WPwffi()) < 0 ) return(erpush("WP1052",label));
/*
***S�tt f�rg mm.
*/
    xwina.background_pixel  = WPgcol(WP_BGND1);
    xwina.border_pixel      = BlackPixel(xdisp,xscr);
    xwina.override_redirect = False;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );
/*
***Skapa ett popup-f�nster med 1 pixels ram.
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

    XStoreName(xdisp,xwin_id,label);
    /* XSetTransientForHint(xdisp,xwin_id,xgwin); */
    XSelectInput(xdisp,xwin_id,ExposureMask | KeyPressMask);
/*
***WPIWIN f�r ej d�das av en WINDOW-Manager som tex. Motif.
*/
    WPsdpr(xwin_id);
/*
***Skapa ett WPIWIN.
*/
    if ( (iwinptr=(WPIWIN *)v3mall(sizeof(WPIWIN),"WPwciw")) == NULL )
      return(erpush("WP1062",label));

    iwinptr->id.w_id = *id;
    iwinptr->id.p_id = (wpw_id)NULL;
    iwinptr->id.x_id = xwin_id;

    iwinptr->geo.x   = x;
    iwinptr->geo.y   = y;
    iwinptr->geo.dx  = dx;
    iwinptr->geo.dy  = dy;

    iwinptr->mapped  = FALSE;

    for ( i=0; i<WP_IWSMAX; ++i) iwinptr->wintab[i].ptr = NULL;
/*
***Lagra f�nstret i f�nstertabellen.
*/
    wpwtab[*id].typ = TYP_IWIN;
    wpwtab[*id].ptr = (char *)iwinptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    WPxpiw(
        WPIWIN *iwinptr)

/*      Expose handler for WPIWIN (and children).
 *
 *      In: iwinptr = C ptr to WPIWIN.
 *
 *      Return: Always TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-10-28 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    short i;
    char *subptr;

/*
***Clear window.
*/
   XClearWindow(xdisp,iwinptr->id.x_id);
/*
***Expose all children.
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

         case TYP_SBAR:
         WPexpose_slidebar((WPSBAR *)subptr);
         break;

         case TYP_DECRN:
         WPexpose_decoration((WPDECRN *)subptr);
         break;
         }
       }
     }
/*
***The end.
*/
   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool          WPbtiw(
        WPIWIN       *iwinptr,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button handler for WPIWIN (with children).
 *
 *      In:  iwinptr = C-ptr to WPIWIN.
 *           butev   = X-but event.
 *
 *      Out: *serv_id = ID for child window.
 *
 *      Return: TRUE  = Event served.
 *              FALSE = Event not served..
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-03-07 Menu window, J.Kjellander
 *      2007-10-28 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    WPBUTT *butptr;
    WPEDIT *edtptr,*focptr;
    WPICON *icoptr;
    WPSBAR *sbptr;

/*
***A WPIWIN is normally insensitive to button clicks.
***The menu window however is a WPIWIN but has a special
***SelectInputMask so that it can generate ButtonRelease.
***This is to make it possible to use the right and middle
***mouse button also in the menu window. So, if the button
***event is not left button the event should not propagate
***down into the children.
*/
   if ( butev->button != 1 ) return(FALSE);
/*
***Lopp through all WPIWIN cild windows.
*/
    for ( i=0; i<WP_IWSMAX; ++i )
      {
      subptr = iwinptr->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( iwinptr->wintab[i].typ )
          {
/*
***Button. Only ButtonRelease is reconized.
*/
          case TYP_BUTTON:
          butptr = (WPBUTT *)subptr;
          if ( butev->window == butptr->id.x_id  &&
               butev->type   == ButtonRelease )
            {
            WPbtbu(butptr);
           *serv_id = butptr->id.w_id;
            return(TRUE);
            }
          break;
/*
***A button click in a WPEDIT means that it will get focus.
***The edit that has focus now will loose focus and this
***is treated as a serving event. Only ButtonRelease is
***reconized.
*/
          case TYP_EDIT:
          edtptr = (WPEDIT *)subptr;
          if ( butev->window == edtptr->id.x_id  &&
               butev->type   == ButtonRelease )
            {
            focptr = WPffoc(iwinptr,FOCUS_EDIT);
           *serv_id = focptr->id.w_id;
            return(WPbted(edtptr,butev));
            }
          break;
/*
***Icon. Only ButtonRelease is reconized.
*/
          case TYP_ICON:
          icoptr = (WPICON *)subptr;
          if ( butev->window == icoptr->id.x_id  &&
               butev->type   == ButtonRelease )
            {
            WPbtic(icoptr);
           *serv_id = icoptr->id.w_id;
            return(TRUE);
            }
          break;
/*
***Slidebar. A slidebar recognizes ButtonPress, ButtonRelease
***and PointerMotion.
*/
          case TYP_SBAR:
          sbptr = (WPSBAR *)subptr;
          if ( butev->window == sbptr->id.x_id )
            {
            WPbutton_slidebar(sbptr,butev);
           *serv_id = sbptr->id.w_id;
            return(TRUE);
            }
          break;
          }
        }
      }
/*
***The end.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool            WPcriw(
        WPIWIN         *iwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin f�r WPIWIN med vidh�ngande sub-f�nster.
 *      Kollar om Leave/Enter skett i n�got av WPIWIN-f�nstrets
 *      subf�nster och servar is�fall eventet.
 *
 *      In: iwinpt = C-pekare till WPIWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
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
***G� igenom alla sub-f�nster.
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
            else                              WPcrbu(butptr,FALSE);
            return(TRUE);
            }
          else if ( croev->type == EnterNotify  &&  butptr->hlight == TRUE )
            WPcrbu(butptr,FALSE);
          break;

          case TYP_EDIT:
          edtptr = (WPEDIT *) subptr;
          if ( croev->window == edtptr->id.x_id )
            {
            if ( croev->type == EnterNotify ) WPcred(edtptr,TRUE);
            else WPcred(edtptr,FALSE);
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

/*      Key-rutin f�r WPIWIN med vidh�ngande sub-f�nster.
 *      Om WPIWIN-f�nstret har ett subf�nster (WPEDIT) med
 *      input-focus = TRUE servas eventet.
 *
 *      In: iwinpt  = C-pekare till WPIWIN.
 *          keyev   = X-Key event.
 *          slevel  = Service-niv�.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID f�r WPEDIT som servat eventet.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    WPEDIT *edtptr;

/*
***Finns sub-f�nster med focus ?.
*/
    edtptr = WPffoc(iwinpt,FOCUS_EDIT);
/*
***Om ja, prova att serva eventet. Vissa tangenttryckningar
***servas "lokal" utan att WPkeed returnerar TRUE. Exempel
***p� detta �r n�r man flyttar cursorn med piltangenterna till
***h�ger eller v�nster. 
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
***Kommer vi hit beror det antingen p� att inget WPEDIT-f�nster
***har fokus just nu eller att det som har fokus inte anser att
***key-eventet �r av den typ som skall signaleras upp�t och f�
***WPwwtw() att g�ra return. Tex. piltangenter i sidled.
*/
   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcmiw(
        WPIWIN               *iwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen f�r WPIWIN.
 *
 *      In: iwinpt  = C-pekare till WPIWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***IWIN-f�nster kan normalt inte ta hand om ClientMessage.
***Om det �r WM_DELETE_WINDOW servar vi genom att pipa lite.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPbell();
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short   WPdliw(
        WPIWIN *iwinptr)

/*      Deletes a WPIWIN with children.
 *
 *      In: iwinptr = C ptr to WPIWIN.
 *
 *      (C)microform ab 6/12/93 J.Kjellander
 *
 *      2007-10-28 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;

/*
***Delete all children.
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

          case TYP_DECRN:
          WPdelete_decoration((WPDECRN *)subptr);
          break;

          case TYP_SBAR:
          WPdelete_slidebar((WPSBAR *)subptr);
          break;
          }
        }
      }
/*
***Deallocate memory for WPIWIN itself.
*/
    v3free((char *)iwinptr,"WPdliw");
/*
***The end.
*/
    return(0);
  }

/********************************************************/
