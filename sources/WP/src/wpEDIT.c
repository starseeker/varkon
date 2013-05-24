/**********************************************************************
*
*    wpEDIT.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPmced();    Create WPEDIT, MBS-version
*    WPwced();    Create WPEDIT, wpw-version
*    WPxped();    Expose routine for WPEDIT
*    WPbted();    Button routine for WPEDIT
*    WPcred();    Crossing routine for WPEDIT
*    WPkeed();    Key routine for WPEDIT
*    WPgted();    Get routine for WPEDIT, GET_EDIT in MBS
*    WPuped();    Replace text in WPEDIT
*    WPdled();    Kill WPEDIT
*   *WPffoc();    Which WPEDIT has focus ?
*    WPfoed();    Focus On/Off for WPEDIT
*    WPmap_key(); Maps X11 keycode/state to Varkon symbol
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

#define XK_MISCELLANY
#define XK_LATIN1
#ifdef UNIX
#include <X11/keysymdef.h>
#endif

extern MNUALT smbind[];

/*!******************************************************/

        short WPmced(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str,
        short   ntkn,
        DBint  *eid)

/*      Create WPEDIT window.
 *
 *      In: pid   = Fï¿½rï¿½lder.
 *          x     = Lï¿½ge i X-led.
 *          y     = Lï¿½ge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          bw    = Border-width.
 *          str   = Text.
 *          ntkn  = Max antal tecken.
 *          eid   = Pekare till utdata.
 *
 *      Ut: *eid = Giltigt entry i fï¿½rï¿½lderns wintab.
 *
 *      Felkod: WP1302 = Fï¿½rï¿½ldern %s finns ej.
 *              WP1312 = Fï¿½rï¿½ldern %s ï¿½r ej ett WPIWIN.
 *              WP1322 = Fï¿½r mï¿½nga subfï¿½nster i %s.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 errbuf[80];
    short                i;
    WPWIN               *winptr;
    WPIWIN              *iwinptr;
    WPEDIT              *edtptr;

/*
***Fixa C-pekare till fï¿½rï¿½lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1302",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPIWIN och fixa en pekare till
***fï¿½rï¿½lder-fï¿½nstret sjï¿½lvt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1312",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID fï¿½r  ny edit, dvs fixa
***en ledig plats i fï¿½rï¿½lderns fï¿½nstertabell.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1322",errbuf));
      }

   *eid = i;
/*
***Skapa edit.
*/
    WPwced(iwinptr->id.x_id,x,y,dx,dy,bw,str,ntkn,&edtptr);
/*
***Lï¿½nka in den i WPIWIN-fï¿½nstret.
*/
    iwinptr->wintab[*eid].typ = TYP_EDIT;
    iwinptr->wintab[*eid].ptr = (char *)edtptr;

    edtptr->id.w_id = *eid;
    edtptr->id.p_id =  pid;
/*
***Om WPIWIN-fï¿½nstret redan ï¿½r mappat skall editen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,edtptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPwced(
        Window   px_id,
        short    x,
        short    y,
        short    dx,
        short    dy,
        short    bw,
        char    *str,
        short    ntkn,
        WPEDIT **outptr)

/*      Skapar WPEDIT-fï¿½nster.
 *
 *      In: px_id  = Fï¿½rï¿½ldra fï¿½nstrets X-id.
 *          x      = Lï¿½ge i X-led.
 *          y      = Lï¿½ge i Y-led.   
 *          dx     = Storlek i X-led.
 *          dy     = Storlek i Y-led.
 *          bw     = Border-width.
 *          str    = Text.
 *          ntkn   = Max antal tecken.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPEDIT.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      12/4/95  ntkn > V3STRLEN, J. Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPEDIT              *edtptr;

/*
***Skapa fï¿½nstret i X. Ramen samma fï¿½rg som bakgrunden.
*/
    xwina.border_pixel      = WPgcol(WP_BGND1);
    xwina.background_pixel  = WPgcol(WP_TOPS);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,
                            DefaultDepth(xdisp,xscr),
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Edit-fï¿½nster skall det kunna klickas i. Enter/Leave for tooltips.
*/
    XSelectInput(xdisp,xwin_id,ButtonPressMask | ButtonReleaseMask |
                               EnterWindowMask | LeaveWindowMask);
/*
***Skapa en WPEDIT.
*/
    if ( (edtptr=(WPEDIT *)v3mall(sizeof(WPEDIT),"WPwced"))
                                                  == NULL ) return(-2);

    edtptr->id.w_id = (wpw_id)NULL;
    edtptr->id.p_id = (wpw_id)NULL;
    edtptr->id.x_id = xwin_id;

    edtptr->geo.x =  x;
    edtptr->geo.y =  y;
    edtptr->geo.dx =  dx;
    edtptr->geo.dy =  dy;
    edtptr->geo.bw =  bw;

    if ( ntkn < 0 ) ntkn = 0;
    if ( ntkn > V3STRLEN ) ntkn = V3STRLEN;
    if ( strlen(str) > (unsigned int)ntkn ) str[ntkn] = '\0';
    strcpy(edtptr->str,str);
    strcpy(edtptr->dstr,str);
    edtptr->tknmax = ntkn;

    edtptr->scroll = 0;
    edtptr->curpos = 0;
    edtptr->fuse   = TRUE;
    edtptr->focus  = FALSE;
    edtptr->tt_str[0] = '\0';

   *outptr = edtptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPxped(
        WPEDIT *edtptr)

/*      Expose handler for WPEDIT.
 *
 *      In: edtptr = C ptr to WPEDIT.
 *
 *      Return: Always TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-06-03 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    int  x,y,tknf;
    char str[V3STRLEN+1];

/*
***Text position.
*/
    x =  WPstrl(" ");
    y =  WPftpy(edtptr->geo.dy);
/*
***Skriv ut den del av texten som syns i fï¿½nstret, dvs.
***inte det som ï¿½r utscrollat. Antal tillgï¿½ngliga tecken-
***positioner ï¿½r i princip sï¿½ mï¿½nga som fï¿½r plats men vi
***bï¿½rjar alltid med en tom fï¿½r sysn skull och reserverar
***dessutom en tom pï¿½ slutet fï¿½r cursorn.
*/
    strcpy(str,&edtptr->str[edtptr->scroll]);

    tknf = (edtptr->geo.dx/WPstrl(" ")) - 2;
    if ( tknf > 0 ) str[tknf] = '\0';

    XSetForeground(xdisp,xgc,WPgcol(WP_FGND));
    XSetBackground(xdisp,xgc,WPgcol(0));
    WPwstr(edtptr->id.x_id,x,y,str);
/*
***If the WPEDIT has focus, display a cursor by drawing
***a vertical line 1 pixel wide.
*/
    if ( edtptr->focus  &&  edtptr->curpos >= 0 )
      {
      x = x + (edtptr->curpos - edtptr->scroll)*WPstrl(" ") - 1;
      y = 3;
      XDrawLine(xdisp,edtptr->id.x_id,xgc,x,y,x,y+edtptr->geo.dy-3);
      }
/*
***Optional 3D frame.
*/
    if ( edtptr->geo.bw > 0 )
      WPd3db((char *)edtptr,TYP_EDIT);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool          WPbted(
        WPEDIT       *edtptr,
        XButtonEvent *butev)

/*      Button-rutin fï¿½r WPEDIT.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          butev  = X-event.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Eventet gï¿½ller ej detta fï¿½nster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    int      x,tknpos,nvis;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPEDIT  *actptr;

/*
***Om detta fï¿½nster inte har input-focus skall det 
***ha det nu ! Det fï¿½nster som ev. hade det tidigare
***skall dï¿½ inte ha focus lï¿½ngre. Fï¿½r att sï¿½kert bli av
***med cursorn suddar vi dï¿½ hela fï¿½nstret och ritar om det.
***Eftersom fokus-byte kan returnera TRUE sï¿½tts gamla
***fï¿½nstrets symbol till SMBNONE sï¿½ att ingen skall tro
***att det ï¿½r en tangenttryckning som genererat fokus-bytet.
***
***The edit used in the WPMCWIN has no parent ID.
*/
    if ( edtptr->focus == FALSE )
      {
      if ( edtptr->id.p_id != (wpw_id)NULL )
        {
        winptr = WPwgwp(edtptr->id.p_id);
        iwinpt = (WPIWIN *)winptr->ptr;
        actptr = WPffoc(iwinpt,FOCUS_EDIT);
        if ( actptr != NULL )
          {
          XClearWindow(xdisp,actptr->id.x_id);
          WPfoed(actptr,FALSE);
          actptr->symbol = SMBNONE;
          }
        }
      WPfoed(edtptr,TRUE);
      return(TRUE);
      }
/*
***Om fï¿½nstret redan har input-focus handlar det om att
***placera cursorn.
*/
    else
      {
      x = butev->x;
      tknpos = (int)((double)x/(double)WPstrl(" "));
      nvis = strlen(edtptr->str) - edtptr->scroll;
      if ( tknpos > nvis ) tknpos = nvis+1;
      if ( tknpos < 1 ) tknpos = 1;
      edtptr->curpos = tknpos - 1 + edtptr->scroll;
      XClearWindow(xdisp,edtptr->id.x_id);
      WPxped(edtptr);

      return(FALSE);
      }
  }

/********************************************************/
/*!******************************************************/

        bool    WPcred(
        WPEDIT *edtptr,
        bool    enter)

/*      Crossing-rutin fï¿½r WPEDIT. Anropande rutin har
 *      konstaterat att eventet gï¿½ller detta fï¿½nster men 
 *      skickar med croev fï¿½r att vi skall kunna avgï¿½ra
 *      om det ï¿½r enter eller leave.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          croev  = X-crossing event.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  = Eventet servat.
 *
 *      (C)2007-03-24, J. Kjellander
 *
 ******************************************************!*/

  {
   int x,y;

/*
***Enter => Order a tooltip in a few seconds if
***there is one to display.
*/
   if ( enter == TRUE )
     {
     if ( edtptr->tt_str[0] != '\0' )
       {
       WPgtmp(&x,&y);
       WPorder_tooltip(x+5,y+10,edtptr->tt_str);
       }
     }
/*
***Leave => Remove ordered or active tooltip.
*/
   else
     {
     WPclear_tooltip();
     }
/*
***The end.
*/
   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool       WPkeed(
        WPEDIT    *edtptr,
        XKeyEvent *keyev,
        int        slevel)

/*      Keypress function for WPEDIT. Serves the event and
 *      returns status TRUE/FALSE depending on which key was
 *      pressed and what service level that was asked for.
 *
 *      In: edtptr = C cptr to WPEDIT.
 *          keyev  = X-key event.
 *          slevel = Service level.
 *
 *          SLEVEL_ALL    => Nothing served locally.
 *          SLEVEL_V3_INP => SMBCHAR + SMBBACKSP +
 *                           SMBLEFT + SMBRIGHT  served locally.
 *          SLEVEL_MBS    => Also SMBMAIN, SMBPOSM and SMBHELP
 *          SLEVEL_NONE   => All events served locally.
 *
 *      FV: TRUE  = Event served.
 *          FALSE = Event not in this window.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *       2007-01-19 SMBENDL/SMBBEGL, J.Kjellander
 *
 ******************************************************!*/

  {
    short    symbol;
    int      tknant,tknwin,i;
    char     tkn;
    bool     servat;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPEDIT  *nxtptr;

/*
***Init.
*/
    tknant = strlen(edtptr->str);
    tknwin = edtptr->geo.dx/WPstrl(" ") - 2;
    servat = FALSE;
/*
***Map keypress to Varkon internal symbol.
***Save the symbol in the edit.
*/
    WPmap_key(keyev,&symbol,&tkn);
    edtptr->symbol = symbol;
/*
***Process...
*/
    switch ( symbol )
      {
      case SMBCHAR:
/*
***Insert, is there room for more characters ?
***Yes, always if fuse = TRUE !
*/
      if ( !edtptr->fuse && (tknant >= edtptr->tknmax) ) XBell(xdisp,100);
/*
***Yes, check if this is the first time a character is recieved by this
***WPEDIT, ie. if fuse = TRUE. In that case we first erase the old text
***and then insert the new. This requires that the cursor is in pos 0.
*/
      else
        {          
        if ( edtptr->fuse  &&  edtptr->curpos == 0 )
          {
          *edtptr->str = '\0';
          tknant = 0;
          }
        for ( i=tknant++; i>=edtptr->curpos; i--)
          edtptr->str[i+1] = edtptr->str[i];
        edtptr->str[edtptr->curpos++] = tkn;
        if ( tknant > tknwin ) ++edtptr->scroll;
        if ( edtptr->fuse ) edtptr->fuse = FALSE;
        }
      break;
/*
***Backspace = erase character to the left of the cursor.
*/
      case SMBBACKSP:
      if ( edtptr->fuse ) edtptr->fuse = FALSE;

      if ( edtptr->curpos > 0 )
        {
        for ( i= --edtptr->curpos; i<tknant; i++)
          edtptr->str[i] = edtptr->str[i+1];
        tknant--;
        }
      else if ( tknant > 0 )
        {
        for ( i=edtptr->curpos; i<tknant; i++)
          edtptr->str[i] = edtptr->str[i+1];
        tknant--;
        }
      else
        {
        strcpy(edtptr->str,edtptr->dstr);
        tknant = strlen(edtptr->str);
        if ( tknant == 0 ) XBell(xdisp,100);
        else edtptr->fuse = TRUE;
        }
      if ( edtptr->curpos < edtptr->scroll )
        edtptr->scroll = edtptr->curpos;
      break;
/*
***Delete = erase character to the right of the cursor.
*/
      case SMBDELETE:
      if ( edtptr->fuse ) edtptr->fuse = FALSE;

      if ( tknant > 0  &&  edtptr->curpos < tknant )
        {
        for ( i=edtptr->curpos; i<tknant; i++)
          edtptr->str[i] = edtptr->str[i+1];
        tknant--;
        }
      else XBell(xdisp,100);
      break;
/*
***Move cursor one step to the left.
*/
      case SMBLEFT:
      if ( edtptr->curpos > 0 )
        {
        --edtptr->curpos;
        if ( edtptr->curpos < edtptr->scroll ) --edtptr->scroll;
        }
      else XBell(xdisp,100);
      break;
/*
***Move cursor to the beginning of the line.
*/
      case SMBBEGL:
      if ( edtptr->curpos > 0 )
        {
        edtptr->curpos = 0;
        edtptr->scroll = 0;
        }
      else XBell(xdisp,100);
      break;
/*
***Move cursor one step to the right.
*/
      case SMBRIGHT:
      if ( edtptr->curpos < tknant )
        {
        ++edtptr->curpos;
        if ( edtptr->curpos > edtptr->scroll + tknwin ) ++edtptr->scroll;
        }
      else XBell(xdisp,100);
      break;
/*
***Move cursor to the end of the line.
*/
      case SMBENDL:
      if ( edtptr->curpos < tknant )
        {
        edtptr->curpos = tknant;
        if ( tknant - tknwin > 0 ) edtptr->scroll = tknant - tknwin;
        else                       edtptr->scroll = 0;
        }
      else XBell(xdisp,100);
      break;
/*
***Move focus to previous edit. UpArrow.
*/
      case SMBUP:
      winptr = WPwgwp(edtptr->id.p_id);
      iwinpt = (WPIWIN *)winptr->ptr;
      nxtptr = WPffoc(iwinpt,PREV_EDIT);
      WPfoed(edtptr,FALSE);
      WPfoed(nxtptr,TRUE);
      if ( slevel == SLEVEL_MBS  ||
           slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
/*
***Move focus to next edit. TAB and DownArrow.
*/
      case SMBDOWN:
      case SMBMAIN:
      winptr = WPwgwp(edtptr->id.p_id);
      iwinpt = (WPIWIN *)winptr->ptr;
      nxtptr = WPffoc(iwinpt,NEXT_EDIT);
      WPfoed(edtptr,FALSE);
      WPfoed(nxtptr,TRUE);
      if ( slevel == SLEVEL_MBS  ||
           slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
/*
***Return.
*/
      case SMBRETURN:
      if ( slevel == SLEVEL_MBS  ||
           slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
/*
***Help <CTRL>a and Pos menu <CTRL>b.
*/
      case SMBPOSM:
      case SMBHELP:
      if ( slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
      }
/*
***The WPEDIT window string is now edited.
***Erase window and expose.
*/
    XClearWindow(xdisp,edtptr->id.x_id);
    WPxped(edtptr);
/*
***If the service level asked for is SLEVEL_ALL set
***the result = TRUE irrespective of what happened.
*/
    if ( slevel == SLEVEL_ALL ) servat = TRUE;
/*
***The end.
*/
    return(servat);
  }

/********************************************************/
/*!******************************************************/

        short WPgted(
        DBint  iwin_id,
        DBint  edit_id,
        char  *str)

/*      Get-rutin fï¿½r WPEDIT.
 *
 *      In: iwin_id = Huvudfï¿½nstrets id.
 *          edit_id = Edit-fï¿½nstrets id.
 *
 *      Ut: str = Aktuell text.
 *
 *      Felkod: WP1162 = Fï¿½rï¿½ldern %s finns ej.
 *              WP1172 = Fï¿½rï¿½ldern %s ej WPIWIN.
 *              WP1182 = Knappen %s finns ej.
 *              WP1192 = %s ï¿½r ej en knapp.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinptr;
    WPEDIT *editptr;

/*
***Fixa C-pekare till fï¿½rï¿½lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1162",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPIWIN.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1172",errbuf));
      }
/*
***Fixa en C-pekare till WPIWIN.
*/
    iwinptr = (WPIWIN *)winptr->ptr;
/*
***Kolla om subfï¿½nstret med angivet id finns och ï¿½r
***av rï¿½tt typ.
*/
    if ( iwinptr->wintab[(wpw_id)edit_id].ptr == NULL )
      {
      sprintf(errbuf,"%d",(int)edit_id);
      return(erpush("WP1182",errbuf));
      }

    if ( iwinptr->wintab[(wpw_id)edit_id].typ != TYP_EDIT )
      {
      sprintf(errbuf,"%d",(int)edit_id);
      return(erpush("WP1192",errbuf));
      }
/*
***Fixa en C-pekare till WPEDIT.
*/
    editptr = (WPEDIT *)iwinptr->wintab[(wpw_id)edit_id].ptr;
/*
***Returnera strï¿½ng.
*/
    strcpy(str,editptr->str);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPuped(
        DBint  iwin_id,
        DBint  edit_id,
        char  *newstr)

/*      Replaces the text in a WPEDIT, UPD_EDIT() in MBS.
 *
 *      In: iwin_id = Parent (WPIWIN) window
 *          edit_id = Child (WPEDIT) window
 *          newstr  = New string
 *
 *      Return: WP1162 = Parent %s does not exist.
 *              WP1172 = Parent %s is not a WPIWIN.
 *              WP1182 = Edit %s does not exist.
 *              WP1192 = %s is not an edit.
 *
 *      (C)2007-02-02 J.Kjellander
 *
 *      2007-11-23 Added XClearWindow(), J.Kjellander
 *
 ******************************************************!*/

  {
   int     ntkn;
   char    errbuf[80];
   WPWIN  *winptr;
   WPIWIN *iwinptr;
   WPEDIT *edtptr;

/*
***Get a C-ptr to the parent entry in wpwtab[].
*/
   if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
     {
     sprintf(errbuf,"%d",(int)iwin_id);
     return(erpush("WP1162",errbuf));
     }
/*
***Is it a WPIWIN.
*/
   if ( winptr->typ != TYP_IWIN )
     {
     sprintf(errbuf,"%d",(int)iwin_id);
     return(erpush("WP1172",errbuf));
     }
/*
***Get a C-ptr to the WPIWIN.
*/
   iwinptr = (WPIWIN *)winptr->ptr;
/*
***Check for the existence of the WPEDIT.
*/
   if ( iwinptr->wintab[(wpw_id)edit_id].ptr == NULL )
     {
     sprintf(errbuf,"%d",(int)edit_id);
     return(erpush("WP1182",errbuf));
     }

   if ( iwinptr->wintab[(wpw_id)edit_id].typ != TYP_EDIT )
     {
     sprintf(errbuf,"%d",(int)edit_id);
     return(erpush("WP1192",errbuf));
     }
/*
***Get a C-ptr to the WPEDIT.
*/
   edtptr = (WPEDIT *)iwinptr->wintab[(wpw_id)edit_id].ptr;
/*
***Check string length.
*/
    ntkn = strlen(newstr);
    if ( ntkn < 0 ) ntkn = 0;
    if ( ntkn > V3STRLEN ) ntkn = V3STRLEN;
    if ( ntkn > edtptr->tknmax ) ntkn = edtptr->tknmax;
    newstr[ntkn] = '\0';
/*
***Update the WPEDIT.
*/
    strcpy(edtptr->str,newstr);

    edtptr->scroll = 0;
    edtptr->curpos = 0;
    edtptr->fuse   = TRUE;
    XClearWindow(xdisp,edtptr->id.x_id);
    WPxped(edtptr);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdled(
        WPEDIT *edtptr)

/*      Dï¿½dar en WPEDIT.
 *
 *      In: edttptr = C-pekare till WPEDIT.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Lï¿½mna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)edtptr,"WPdled");
    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPEDIT *WPffoc(
        WPIWIN *iwinptr,
        int     code)

/*      Letar efter WPEDIT:s i ett WPIWIN och returnerar
 *      en pekare till det som efterfrï¿½gats eller NULL om
 *      det inte finns.
 *
 *      In: iwinptr = C-pekare till WPIWIN.
 *          code    = FIRST_EDIT  => Fï¿½rsta i wintab oavsett fokus.
 *                  = NEXT_EDIT   => 1:a efter den som har fokus
 *                  = PREV_EDIT   => 1:a fï¿½re den som har fokus
 *                  = FOCUS_EDIT  => Den som har fokus.
 *
 *      Ut: Inget.   
 *
 *      FV: Pekare till WPEDIT eller NULL.
 *
 *      (C)microform ab 14/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    wpw_id  foc_id;
    WPEDIT *edtptr;

/*
***Leta igenom alla sub-fï¿½nster efter WPEDIT.
*/
    switch ( code )
      {
/*
***Fï¿½rsta.
*/
      case FIRST_EDIT:
      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          return((WPEDIT *)subptr);
        }
      return(NULL);
/*
***Sista.
*/
      case LAST_EDIT:
      for ( i=WP_IWSMAX-1; i>=0; --i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          return((WPEDIT *)subptr);
        }
      return(NULL);
/*
***Nï¿½sta.
*/
      case NEXT_EDIT:
      edtptr = WPffoc(iwinptr,FOCUS_EDIT);
      if ( edtptr == NULL ) return(NULL);
      else foc_id = edtptr->id.w_id;

      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->id.w_id > foc_id ) return(edtptr);
          }
        }
      edtptr = WPffoc(iwinptr,FIRST_EDIT);
      return(edtptr);
/*
***Fï¿½regï¿½ende.
*/
      case PREV_EDIT:
      edtptr = WPffoc(iwinptr,FOCUS_EDIT);
      if ( edtptr == NULL ) return(NULL);
      else foc_id = edtptr->id.w_id;

      for ( i=WP_IWSMAX-1; i>=0; --i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->id.w_id < foc_id ) return(edtptr);
          }
        }
      edtptr = WPffoc(iwinptr,LAST_EDIT);
      return(edtptr);
/*
***Aktivt.
*/
      case FOCUS_EDIT:
      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->focus == TRUE ) return(edtptr);
          }
        }
      return(NULL);
      }
   return(NULL);
  }

/********************************************************/
/*!******************************************************/

        short WPfoed(
        WPEDIT *edtptr,
        bool    mode)

/*      Fokus-rutin fï¿½r WPEDIT.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          mode   = TRUE  => Fokus pï¿½.
 *                   FALSE => Fokus av.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 14/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Fokus Pï¿½ !
*/
    if ( mode == TRUE )
      {
      XSetWindowBorder(xdisp,edtptr->id.x_id,WPgcol(WP_NOTI));
      edtptr->focus = TRUE;
      }
/*
***Fokus AV !
*/
    else
      {
      XSetWindowBorder(xdisp,edtptr->id.x_id,WPgcol(WP_BGND1));
      edtptr->focus = FALSE;
      }
/*
***Fï¿½r att resultatet skall synas (cursorn) krï¿½vs nu ett expose.
*/
    WPxped(edtptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        void       WPmap_key(
        XKeyEvent *keyev, 
        short     *sym,
        char      *t)

/*      ï¿½versï¿½tter keycode och state i ett key-event
 *      till en V3-symbol samt eventuellt ASCII-tecken.
 *
 *      In: keyev  = Pekare till key-event.
 *          sym    = Pekare till utdata.
 *          t      = Pekare till utdata.
 *
 *      Ut: *sym    = Motsvarande V3-symbol, tex. SMBUP.
 *          *t      = ASCII-kod om symbol = SMBCHAR.
 *
 *      (C)microform ab 10/12/93 J. Kjellander
 *
 *      1998-04-21 8-Bitars ASCII, J.Kjellander
 *      2007-02-03 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    char            tknbuf[11],tkn='\0';
    short           symbol;
    bool            numlock = FALSE;
    bool            shift   = FALSE;
    KeySym          keysym;
    XComposeStatus  costat;
    int             ntkn = 0;

/*
***Numlock = Mod2Mask har det visat
***sig under ODT, detta ï¿½r inte nï¿½dvï¿½ndigtvis standard.
*/
    if ( ((keyev->state & Mod2Mask)  > 0) ) numlock = TRUE;
/*
***Var shift-tangenten nere ?
*/
    if ( ((keyev->state & ShiftMask)  > 0) ) shift = TRUE;
/*
***Vi bï¿½rjar med att anvï¿½nda LookupString fï¿½r att ta reda pï¿½
***vilken keysym det var. Vi kunde anvï¿½nda LookupKeysym() men
***LookupString() tar hï¿½nsyn till shift, numlock etc. ï¿½t oss
***pï¿½ ett bï¿½ttre sï¿½tt (hï¿½rdvaruoberoende).
*/
    ntkn = XLookupString(keyev,tknbuf,10,&keysym,&costat);
/*
***Vissa symboler skall mappas pï¿½ ett fï¿½r V3 speciellt sï¿½tt.
*/
    symbol = SMBNONE;

    switch ( keysym )
      {
/*
***ï¿½, ï¿½ och ï¿½.
*/
      case XK_aring:      symbol = SMBCHAR; tkn = 'å' ; break;
      case XK_Aring:      symbol = SMBCHAR; tkn = 'Å' ; break;
      case XK_adiaeresis: symbol = SMBCHAR; tkn = 'ä' ; break;
      case XK_Adiaeresis: symbol = SMBCHAR; tkn = 'Ä' ; break;
      case XK_odiaeresis: symbol = SMBCHAR; tkn = 'ö' ; break;
      case XK_Odiaeresis: symbol = SMBCHAR; tkn = 'Ö' ; break;
/*
***Backspace, delete och return (samt enter).
*/
      case XK_BackSpace:  symbol = SMBBACKSP; break;
      case XK_Return:     symbol = SMBRETURN; break;
      case XK_Delete:     symbol = SMBDELETE; break;
/*
***Keypad-separator skall mappas till '.'
*/
      case XK_KP_Separator: symbol = SMBCHAR; tkn = '.'; break;
/*
***Piltangenter.
*/
      case XK_Up:     symbol = SMBUP;    break;
      case XK_Down:   symbol = SMBDOWN;  break;
      case XK_Left:   symbol = SMBLEFT;  break;
      case XK_Right:  symbol = SMBRIGHT; break;
/*
***End of line and beginning of line.
*/
      case XK_End:    symbol = SMBENDL;  break;
      case XK_Begin:  symbol = SMBBEGL;  break;
      case XK_Home:   symbol = SMBBEGL;  break;
/*
***ï¿½vriga tangenter anvï¿½nder vi LookupString():s mappning.
***Vissa ASCII-koder under 32 ï¿½r tillï¿½tna.
*/
      default:
      if ( ntkn == 1 )
        {
        switch ( tkn = tknbuf[0] )
          {
          case 13:
          case 10:
          symbol = SMBRETURN;
          break;

          case 8:
          symbol = SMBBACKSP;
          break;

          default:
          if ( tkn > 31 )                   symbol = SMBCHAR;
          else if ( tkn == *smbind[7].str ) symbol = SMBMAIN;
          else if ( tkn == *smbind[8].str ) symbol = SMBHELP;
          else if ( tkn == *smbind[9].str ) symbol = SMBPOSM;
          break;
          }
        }
      }
/*
***Slut.
*/
    *sym = symbol;
    *t   = tkn;

    return;
  }

/********************************************************/
