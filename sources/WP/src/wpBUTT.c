/**********************************************************************
*
*    wpBUTT.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcrlb();      Create a LABELBUTTON
*    WPcrtb();      Create a TEXTBUTTON
*    WPcrpb();      Create a PUSHBUTTON
*    WPcrsb();      Create a STATEBUTTON
*    WPcrfb();      Create a FUNCBUTTON
*
*    WPwcbu();      Create WPBUTT
*    WPxpbu();      Expose routine for WPBUTT
*    WPscbu();      Set button color
*    WPbtbu();      Button routine for WPBUTT
*    WPcrbu();      Crossing routine for WPBUT
*    WPgtbu();      Get routine for WPBUTT, GET_BUTTON in MBS
*    WPdlbu();      Kills WPBUTT
*    WPgcbu();      Returns button color
*    WPupbu();      Replaces button text
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

        short  WPcrlb(
        int    pid,
        short  x,
        short  y,
        short  dx,
        short  dy,
        char  *butstr,
        DBint *bid)

/*      Create a LABELBUTTON in a WPIWIN window.
 *
 *      In: pid    = Parent window ID.
 *          x,y    = Position.
 *          dx,dy  = Size.
 *          butstr = Text.
 *
 *      Out: *bid = Button ID.
 *
 *      (C)2007-10-22 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPBUTT  *butptr;

/*
***Get a C ptr to the WPIWIN.
*/
    winptr = WPwgwp(pid);
    iwinpt = (WPIWIN *)winptr->ptr;
/*
***Allocate an ID for the new button.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX ) return(-3);
    else *bid = i;
/*
***Create the button.
*/
    status = WPwcbu(iwinpt->id.x_id,x,y,dx,dy,(short)0,
                        butstr,butstr,"",WP_BGND1,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***A label can not recieve any events.
*/
    XSelectInput(xdisp,butptr->id.x_id,0);
/*
***Add the button to the WPIWIN.
*/
   iwinpt->wintab[*bid].typ = TYP_BUTTON;
   iwinpt->wintab[*bid].ptr = (char *)butptr;

   butptr->id.w_id = *bid;
   butptr->id.p_id =  pid;

   XMapWindow(xdisp,butptr->id.x_id);
/*
***Type.
*/
   butptr->type = LABELBUTTON;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPcrtb(
        int    pid,
        short  x,
        short  y,
        char  *butstr,
        DBint *bid)

/*      Create a TEXTBUTTON in a WPIWIN window.
 *
 *      In: pid    = Parent window ID.
 *          x,y    = Position.
 *          butstr = Text.
 *
 *      Out: *bid = Button ID.
 *
 *      (C)2007-10-24 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,dx,dy;
    int      i;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPBUTT  *butptr;

/*
***Get a C ptr to the WPIWIN.
*/
    winptr = WPwgwp(pid);
    iwinpt = (WPIWIN *)winptr->ptr;
/*
***Allocate an ID for the new button.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX ) return(-3);
    else *bid = i;
/*
***Button size.
*/
    dx = WPstrl(butstr);
    dy = WPstrh();
/*
***Create the button.
*/
    status = WPwcbu(iwinpt->id.x_id,x,y,dx,dy,(short)0,
                        butstr,butstr,"",WP_BGND1,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***Set the input mask.
*/
   XSelectInput(xdisp,butptr->id.x_id,ButtonPressMask | ButtonReleaseMask |
                                      EnterWindowMask | LeaveWindowMask);
/*
***Add the button to the WPIWIN.
*/
   iwinpt->wintab[*bid].typ = TYP_BUTTON;
   iwinpt->wintab[*bid].ptr = (char *)butptr;

   butptr->id.w_id = *bid;
   butptr->id.p_id =  pid;

   XMapWindow(xdisp,butptr->id.x_id);
/*
***Type.
*/
   butptr->type = TEXTBUTTON;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPcrpb(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str1,
        char   *str2,
        char   *fstr,
        short   cb,
        short   cf,
        DBint  *bid)

/*      Creates a PUSHBUTTON and links it to a WPIWIN.
 *
 *      In: pid   = Parent window ID.
 *          x     = X position.
 *          y     = Y position.
 *          dx    = X size.
 *          dy    = Y size.
 *          bw    = Border-width.
 *          str1  = Text for off/FALSE.
 *          str2  = Text for on/TRUE.
 *          fstr  = Fontname or "" (default).
 *          cb    = Background color.
 *          cf    = Forground color.
 *
 *      Out: *bid = Button ID (relative to parent).
 *
 *      Error: WP1072 = Parent does not exist.
 *             WP1082 = Parent is not a WPIWIN.
 *             WP1092 = Parent window table full.
 *
 *      (C)2007-10-22 J.Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[80];
   short   i,status;
   WPWIN  *winptr;
   WPIWIN *iwinptr;
   WPBUTT *butptr;

/*
***Get a C pointer to the parent's entry in wpwtab.
*/
   if ( (winptr=WPwgwp(pid)) == NULL )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1072",errbuf));
     }
/*
***Check that it is a WPIWIN and get a C pointer to the parent
***itself.
*/
   if ( winptr->typ != TYP_IWIN )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1082",errbuf));
     }
   else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Allocate a free ID for the new button.
*/
   i = 0;
   while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

   if ( i == WP_IWSMAX )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1092",errbuf));
     }
   else *bid = i;
/*
***Create the button.
*/
   if ( (status=WPwcbu(iwinptr->id.x_id,x,y,dx,dy,bw,
                       str1,str1,"",cb,cf,&butptr)) < 0 ) return(status);
/*
***Set the input mask.
*/
   XSelectInput(xdisp,butptr->id.x_id,ButtonPressMask | ButtonReleaseMask |
                                      EnterWindowMask | LeaveWindowMask);
/*
***Link it to the WPIWIN window.
*/
   iwinptr->wintab[*bid].typ = TYP_BUTTON;
   iwinptr->wintab[*bid].ptr = (char *)butptr;

   butptr->id.w_id = *bid;
   butptr->id.p_id =  pid;
/*
***If the parent WPIWIN is mapped, map the buton now.
*/
   if ( iwinptr->mapped ) XMapWindow(xdisp,butptr->id.x_id);
/*
***Button type.
*/
   butptr->type = PUSHBUTTON;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPcrsb(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str1,
        char   *str2,
        char   *fstr,
        short   cb,
        short   cf,
        DBint  *bid)

/*      Creates a STATEBUTTON and links it to a WPIWIN.
 *      CRE_BUTTON in MBS.
 *
 *      In: pid   = Parent window ID.
 *          x     = X position.
 *          y     = Y position.
 *          dx    = X size.
 *          dy    = Y size.
 *          bw    = Border-width.
 *          str1  = Text for off/FALSE.
 *          str2  = Text for on/TRUE.
 *          fstr  = Fontname or "" (default).
 *          cb    = Background color.
 *          cf    = Forground color.
 *
 *      Out: *bid = Button ID (relative to parent).
 *
 *      Error: WP1072 = Parent does not exist.
 *             WP1082 = Parent is not a WPIWIN.
 *             WP1092 = Parent window table full.
 *
 *      (C)2007-10-22 J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[80];
   short   i,status;
   WPWIN  *winptr;
   WPIWIN *iwinptr;
   WPBUTT *butptr;

/*
***Get a C pointer to the parent's entry in wpwtab.
*/
   if ( (winptr=WPwgwp(pid)) == NULL )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1072",errbuf));
     }
/*
***Check that it is a WPIWIN and get a C pointer to the parent
***itself.
*/
   if ( winptr->typ != TYP_IWIN )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1082",errbuf));
     }
   else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Allocate a free ID for the new button.
*/
   i = 0;
   while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

   if ( i == WP_IWSMAX )
     {
     sprintf(errbuf,"%d",(int)pid);
     return(erpush("WP1092",errbuf));
     }
   else *bid = i;
/*
***Create the button.
*/
   if ( (status=WPwcbu(iwinptr->id.x_id,x,y,dx,dy,bw,
                       str1,str2,fstr,cb,cf,&butptr)) < 0 ) return(status);
/*
***Set the input mask.
*/
   XSelectInput(xdisp,butptr->id.x_id,ButtonPressMask | ButtonReleaseMask |
                                      EnterWindowMask | LeaveWindowMask);
/*
***Link it to the WPIWIN window.
*/
   iwinptr->wintab[*bid].typ = TYP_BUTTON;
   iwinptr->wintab[*bid].ptr = (char *)butptr;

   butptr->id.w_id = *bid;
   butptr->id.p_id =  pid;
/*
***If the parent WPIWIN is mapped, map the buton now.
*/
   if ( iwinptr->mapped ) XMapWindow(xdisp,butptr->id.x_id);
/*
***Button type.
*/
   butptr->type = STATEBUTTON;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPcrfb(
        int    pid,
        short  x,
        short  y,
        short  dx,
        short  dy,
        char  *butstr,
        char  *akod,
        short  anum,
        DBint *bid)

/*      Create FBUTTON in a WPGWIN window.
 *      CRE_FBUTTON in MBS.
 *
 *      In: pid    = WPGWIN window ID.
 *          x,y    = Position.
 *          dx,dy  = Size.
 *          butstr = Text.
 *          akod   = Action code.
 *          anum   = Action number.
 *
 *      Out: *bid = Button ID.
 *
 *      Error:  WP1512 = %s is an illegal action code.
 *              WP1482 = Window %s does not exist.
 *              WP1492 = Window %s has illegal type.
 *              WP1502 = Window %s is full.
 *
 *      (C)2007-10-22 J.Kjellander
 *
 ******************************************************!*/

  {
    short    status,action;
    int      i;
    char     errbuf[80];
    WPWIN   *winptr;
    WPGWIN  *gwinpt;
    WPBUTT  *butptr;

/*
***Check action code ?
*/
    if ( akod[1] != '\0' ) return(erpush("WP1512",akod));

    switch ( akod[0] )
      {
      case 'f': action = CFUNC; break;
      case 'm': action = MENU;  break;
      case 'p': action = PART;  break;
      case 'r': action = RUN;   break;
      case 'M': action = MFUNC; break;

      default: return(erpush("WP1512",akod));
      break;
      }
/*
***Get a C ptr to the WPGWIN.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1482",errbuf));
      }

    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1492",errbuf));
      }
    else gwinpt = (WPGWIN *)winptr->ptr;
/*
***Allocate an ID for the new button.
*/
    i = 0;
    while ( i < WP_GWSMAX  &&  gwinpt->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_GWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1502",errbuf));
      }
    else *bid = i;
/*
***Create the button.
*/
    status = WPwcbu(gwinpt->id.x_id,x,y,dx,dy,(short)1,
                        butstr,butstr,"",WP_BGND2,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***Set the input mask.
*/
   XSelectInput(xdisp,butptr->id.x_id,ButtonPressMask | ButtonReleaseMask |
                                      EnterWindowMask | LeaveWindowMask);
/*
***Add the button to the WPGWIN window.
*/
   gwinpt->wintab[*bid].typ = TYP_BUTTON;
   gwinpt->wintab[*bid].ptr = (char *)butptr;

   butptr->id.w_id = *bid;
   butptr->id.p_id =  pid;

   XMapWindow(xdisp,butptr->id.x_id);
/*
***Action.
*/
   butptr->acttyp = action;
   butptr->actnum = anum;
/*
***Button type.
*/
   butptr->type = FUNCBUTTON;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short    WPwcbu(
        Window   px_id,
        short    x,
        short    y,
        short    dx,
        short    dy,
        short    bw,
        char    *str1,
        char    *str2,
        char    *fstr,
        short    cb,
        short    cf,
        WPBUTT **outptr)

/*      Create a WPBUTT.
 *
 *      In: px_id    = Parent window ID.
 *          x        = X position.
 *          y        = Y position.
 *          dx       = X size.
 *          dy       = Y size.
 *          bw       = Border width.
 *          str1     = Text for off/FALSE.
 *          str2     = Text for on/TRUE.
 *          fstr     = Fontname or "".
 *          cb       = Background color.
 *          cf       = Foreground color.
 *
 *      Out: *outptr = Pointer to WPBUTT.
 *
 *      Error: WP1102 = Can't activate font %s.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2006-12-11 Added ButtonReleaseMask, J.Kjellander
 *      2007-03-08 Tooltips, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPBUTT              *butptr;

/*
***Create the X window.
*/
    xwina.background_pixel  = WPgcbu(WPwfpx(px_id),cb);
    xwina.border_pixel      = WPgcbu(WPwfpx(px_id),WP_BGND1);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,CopyFromParent,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Crete the WPBUTT.
*/
    if ( (butptr=(WPBUTT *)v3mall(sizeof(WPBUTT),"WPwcbu")) == NULL )
       return(erpush("WP1112",str1));

    butptr->id.w_id = (wpw_id)NULL;
    butptr->id.p_id = (wpw_id)NULL;
    butptr->id.x_id = xwin_id;

    butptr->geo.x  = x;
    butptr->geo.y  = y;
    butptr->geo.dx = dx;
    butptr->geo.dy = dy;
    butptr->geo.bw = bw;

    butptr->color.bckgnd = cb;
    butptr->color.forgnd = cf;

    if ( strlen(str1) > 80 ) str1[80] = '\0';
    strcpy(butptr->stroff,str1);
    if ( strlen(str2) > 80 ) str2[80] = '\0';
    strcpy(butptr->stron,str2);

    butptr->status = FALSE;
    butptr->hlight = FALSE;

    if ( fstr[0] == '\0' ) butptr->font = WP_FNTNORMAL;
    else if ( (butptr->font=WPgfnr(fstr)) < 0 )
                         return(erpush("WP1102",fstr));
/*
***Init tooltip text.
*/
    butptr->tt_str[0] = '\0';
/*
***Return ptr to button.
*/
   *outptr = butptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    WPxpbu(
        WPBUTT *butptr)

/*      Expose handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1996-12-12 Vänsterjust. lablar, J.Kjellander
 *      1998-03-27 OpenGL för AIX, J.Kjellander
 *      2007-10-23 Button types, J.Kjellander
 *
 ******************************************************!*/

  {
   int          x,y;
   char         text[81];
   GC           but_gc;
   WPRWIN      *rwinpt;
   XFontStruct *xfs;

/*
***A button in an OpenGL window must use the same
***visual as the OpenGL window. Other windows can
***use xgc.
*/

/* NOTE wrong font used here */

   switch ( wpwtab[butptr->id.p_id].typ )
     {
     case TYP_RWIN:
     rwinpt = (WPRWIN *)wpwtab[butptr->id.p_id].ptr;
     but_gc = rwinpt->win_gc;
     xfs    = WPgfnt(WP_FNTNORMAL);
     XSetFont(xdisp,but_gc,xfs->fid);
     break;

     default:
     but_gc = xgc;
     WPsfnt(butptr->font);
     break;
     }
/*
***What type of button ?
*/
   switch ( butptr->type )
     {
     case LABELBUTTON:
     x = (butptr->geo.dx - WPstrl(butptr->stron))/2;
     y =  WPftpy(butptr->geo.dy);
     XSetBackground(xdisp,but_gc,WPgcol(butptr->color.bckgnd));
     XSetForeground(xdisp,but_gc,WPgcol(butptr->color.forgnd));
     XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,
                      butptr->stron,strlen(butptr->stron));
     return(TRUE);

     case TEXTBUTTON:
     x = 0;
     y =  WPftpy(butptr->geo.dy);
     XSetBackground(xdisp,but_gc,WPgcol(butptr->color.bckgnd));
     XSetForeground(xdisp,but_gc,WPgcol(butptr->color.forgnd));
     XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,
                      butptr->stron,strlen(butptr->stron));
     return(TRUE);

     case PUSHBUTTON:
     x = (butptr->geo.dx - WPstrl(butptr->stron))/2;
     y =  WPftpy(butptr->geo.dy);
     XSetBackground(xdisp,but_gc,WPgcol(butptr->color.bckgnd));
     XSetForeground(xdisp,but_gc,WPgcol(butptr->color.forgnd));
     XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,
                      butptr->stron,strlen(butptr->stron));
     if ( butptr->geo.bw > 0 ) WPd3db((char *)butptr,TYP_BUTTON);
     return(TRUE);

     case STATEBUTTON:
     if ( butptr->status ) strcpy(text,butptr->stron);
     else                  strcpy(text,butptr->stroff);
     x = (butptr->geo.dx - WPstrl(text))/2;
     y =  WPftpy(butptr->geo.dy);
     XSetBackground(xdisp,but_gc,WPgcol(butptr->color.bckgnd));
     XSetForeground(xdisp,but_gc,WPgcol(butptr->color.forgnd));
     XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,text,strlen(text));
     if ( butptr->geo.bw > 0 ) WPd3db((char *)butptr,TYP_BUTTON);
     return(TRUE);

     case FUNCBUTTON:
     x = (butptr->geo.dx - WPstrl(butptr->stron))/2;
     y =  WPftpy(butptr->geo.dy);
     XSetWindowBackground(xdisp,butptr->id.x_id,
                          WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
     XSetBackground(xdisp,but_gc,WPgcol(butptr->color.bckgnd));
     XSetForeground(xdisp,but_gc,WPgcol(butptr->color.forgnd));
     XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,
                      butptr->stron,strlen(butptr->stron));
     if ( butptr->geo.bw > 0 ) WPd3db((char *)butptr,TYP_BUTTON);
     return(TRUE);
     }
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        void    WPscbu(
        WPBUTT *butptr,
        int     color)

/*      Sets the background color of a button.
 *
 *      In: buttptr = C-ptr to WPBUTT.
 *          color   = Color number.
 *
 *      (C)2007-03-24 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Set the button backgrund color and force an expose
***to make the new color visible.
*/
   butptr->color.bckgnd = color;

   XUnmapWindow(xdisp,butptr->id.x_id);

   XSetWindowBackground(xdisp,butptr->id.x_id,
                        WPgcbu(butptr->id.p_id,butptr->color.bckgnd));

   XMapWindow(xdisp,butptr->id.x_id);
  }

/********************************************************/
/*!******************************************************/

        bool    WPbtbu(
        WPBUTT *butptr)

/*      Button handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J.Kjellander
 *
 *      2007-10-23 Button types, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***The only button type that needs to react on
***a button event is the STATEBUTTON.
*/
   if ( butptr->type == STATEBUTTON )
     {
     if ( butptr->status == FALSE )
       {
       butptr->status = TRUE;
       }
     else
       {
       butptr->status = FALSE;
       }

     XClearWindow(xdisp,butptr->id.x_id);
     WPxpbu(butptr);
     }
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool    WPcrbu(
        WPBUTT *butptr,
        bool    enter)

/*      Crossing handler for WPBUTT.
 *
 *      In: butptr = C ptr to WPBUTT.
 *          enter  = TRUE  => Enter.
 *                   FALSE => Leave.
 *
 *      Return: Always = TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-03-08 Tooltips, J.Kjellander
 *      2007-10-22 Button types, J.Kjellander
 *
 ******************************************************!*/

 {
   int    x,y,orgcol;

/*
***LABELBUTTON's dont react on crossing events.
*/
   if ( butptr->type == LABELBUTTON ) return(TRUE);
/*
***All others do.
***Enter => Change color of window border to WP_NOTI.
*/
   if ( enter == TRUE  &&  !butptr->hlight )
     {
     switch ( butptr->type )
       {
       case TEXTBUTTON:
       orgcol = butptr->color.bckgnd;
       butptr->color.bckgnd = WP_NOTI;
       XSetWindowBackground(xdisp,butptr->id.x_id,WPgcol(WP_NOTI));
       WPxpbu(butptr);
       XFlush(xdisp);
       butptr->color.bckgnd = orgcol;
       break;

       case PUSHBUTTON:
       case STATEBUTTON:
       case FUNCBUTTON:
       XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_NOTI));
       break;
       }
/*
***Button is now highlighted.
*/
     butptr->hlight = TRUE;
/*
***Order a tooltip in a few seconds if there is one to display.
*/
     if ( butptr->tt_str[0] != '\0' )
       {
       WPgtmp(&x,&y);
       WPorder_tooltip(x+5,y+10,butptr->tt_str);
       }
     }
/*
***Leave => Reset window border color.
*/
   else if ( butptr->hlight )
     {
     switch ( butptr->type )
       {
       case TEXTBUTTON:
       XSetWindowBackground(xdisp,butptr->id.x_id,WPgcol(butptr->color.bckgnd));
       WPxpbu(butptr);
       XFlush(xdisp);
       break;

       case PUSHBUTTON:
       case STATEBUTTON:
       case FUNCBUTTON:
       XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_BGND1));
       break;
       }
/*
***Button is no longer highlighted.
*/
     butptr->hlight = FALSE;
/*
***Remove ordered or active tooltip.
*/
     WPclear_tooltip();
     }

   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

        short  WPgtbu(
        DBint  iwin_id,
        DBint  butt_id,
        DBint *status)

/*      Get routine for WPBUTT.
 *
 *      In: iwin_id = Parent WPIWIN window id.
 *          butt_id = Button id.
 *
 *      Error: WP1122 = Parent %s does not exist.
 *             WP1132 = Parent %s is not a WPIWIN.
 *             WP1142 = Button %s does not exist.
 *             WP1152 = %s is not a button.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinptr;
    WPBUTT *buttptr;

/*
***Fixa C-pekare till fï¿½rï¿½lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1122",errbuf));
      }
/*
***Kolla att det ï¿½r ett WPIWIN.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1132",errbuf));
      }
/*
***Fixa en C-pekare till WPIWIN.
*/
    iwinptr = (WPIWIN *)winptr->ptr;
/*
***Kolla om subfï¿½nstret med angivet id finns och ï¿½r
***av rï¿½tt typ.
*/
    if ( iwinptr->wintab[(wpw_id)butt_id].ptr == NULL )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1142",errbuf));
      }

    if ( iwinptr->wintab[(wpw_id)butt_id].typ != TYP_BUTTON )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1152",errbuf));
      }
/*
***Fixa en C-pekare till WPBUTT.
*/
    buttptr = (WPBUTT *)iwinptr->wintab[(wpw_id)butt_id].ptr;
/*
***Returnera status.
*/
    *status = buttptr->status;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPdlbu(
        WPBUTT *butptr)

/*      Delete a WPBUTT.
 *
 *      In: buttptr = C-pekare till WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Check for active tooltip and delete if nessecary.
*/
   WPclear_tooltip();
/*
***Release allocated C memory.
*/
   v3free((char *)butptr,"WPdlbu");
   return(0);
  }

/********************************************************/
/*!******************************************************/

 unsigned long WPgcbu(
        wpw_id p_id,
        int    colnum)

/*      Returnerar fï¿½rg fï¿½r WPBUTT. Om fï¿½rï¿½ldern ï¿½r
 *      en WPRWIN gï¿½rs sï¿½rskild hantering.
 *
 *      In: p_id   = ID fï¿½r fï¿½rï¿½lder.
 *          colnum = VARKON fï¿½rgnummer.
 *
 *      FV: Pixelvï¿½rde.
 *
 *      (C)microform ab 1998-03-27 J. Kjellander
 *
 *      2007-04-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   WPRWIN *rwinpt;

/*
***Om knappen sitter i ett WPRWIN mï¿½ste vi returnera
***ett pixelvï¿½rde som ï¿½r kompatibelt med det fï¿½nstrets
***visual. ï¿½vriga fï¿½nster anvï¿½nder default visual.
*/
   switch ( wpwtab[p_id].typ )
     {
     case TYP_RWIN:
     rwinpt   = (WPRWIN *)wpwtab[p_id].ptr;
     switch ( colnum )
       {
       case WP_BGND1: return(rwinpt->bgnd1);
       case WP_BGND2: return(rwinpt->bgnd2);
       case WP_BGND3: return(rwinpt->bgnd3);
       case WP_FGND:  return(rwinpt->fgnd);
       case WP_TOPS:  return(rwinpt->tops);
       case WP_BOTS:  return(rwinpt->bots);
       case WP_NOTI:  return(rwinpt->noti);
       default:       return(0);
       }
     break;

     default:
     return(WPgcol(colnum));
     }
  }

/********************************************************/
/********************************************************/

        short  WPupbu(
        DBint  iwin_id,
        DBint  but_id,
        char  *new_str,
        short  new_dx)

/*      Replaces the text and changes the length of a WPBUTT.
 *      This function is not yet available from MBS but
 *      could easily be added since error processing etc.
 *      is there.
 *
 *      In: iwin_id = Parent (WPIWIN) window
 *          but_id  = Child (WPBUTT) window
 *          new_str = New string
 *          new_dx  = New length
 *
 *      Return: WP1782 = Parent %s does not exist.
 *              WP1792 = Parent %s is not a WPIWIN.
 *              WP1802 = Button %s does not exist.
 *              WP1812 = %s is not a button.
 *
 *      (C)2008-02-03 J.Kjellander
 *
 ******************************************************!*/

  {
   int     ntkn;
   char    errbuf[80];
   WPWIN  *winptr;
   WPIWIN *iwinptr;
   WPBUTT *butptr;

/*
***Get a C-ptr to the parent entry in wpwtab[].
*/
   if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
     {
     sprintf(errbuf,"%d",(int)iwin_id);
     return(erpush("WP1782",errbuf));
     }
/*
***Is it a WPIWIN.
*/
   if ( winptr->typ != TYP_IWIN )
     {
     sprintf(errbuf,"%d",(int)iwin_id);
     return(erpush("WP1792",errbuf));
     }
/*
***Get a C-ptr to the WPIWIN.
*/
   iwinptr = (WPIWIN *)winptr->ptr;
/*
***Check for the existence of the WPBUTT.
*/
   if ( iwinptr->wintab[(wpw_id)but_id].ptr == NULL )
     {
     sprintf(errbuf,"%d",(int)but_id);
     return(erpush("WP1802",errbuf));
     }

   if ( iwinptr->wintab[(wpw_id)but_id].typ != TYP_BUTTON )
     {
     sprintf(errbuf,"%d",(int)but_id);
     return(erpush("WP1812",errbuf));
     }
/*
***Get a C-ptr to the WPBUTT.
*/
   butptr = (WPBUTT *)iwinptr->wintab[(wpw_id)but_id].ptr;
/*
***Check string length.
*/
    ntkn = strlen(new_str);
    if ( ntkn < 0 ) ntkn = 0;
    if ( ntkn > V3STRLEN ) ntkn = V3STRLEN;
    new_str[ntkn] = '\0';
/*
***Update the WPBUTT.
*/
    strncpy(butptr->stron,new_str,V3STRLEN);

    if ( new_dx > 0 )
      {
      butptr->geo.dx = new_dx;
      XResizeWindow(xdisp,butptr->id.x_id,butptr->geo.dx,butptr->geo.dy);
      }

    XClearWindow(xdisp,butptr->id.x_id);
    WPxpbu(butptr);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
