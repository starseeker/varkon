/**********************************************************************
*
*    wpSBAR.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_slidebar(); Create WPSBAR
*    WPexpose slidebar(); Expose WPSBAR
*    WPbutton_slidebar(); Button handler (scroll) for WPSBAR
*    WPdelete_slidebar(); Delete WPSBAR
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

#define CBTHOLD 3   /* Threshold (pixels moved) for callback */

/*!******************************************************/

        short    WPcreate_slidebar(
        int      pid,
        int      sx,
        int      sy,
        int      sdx,
        int      sdy,
        int      bstart,
        int      bend,
        int      dir,
        WPSBAR **outptr)

/*      Creates a WPSBAR.
 *
 *      In: pid    = Parent window ID.
 *          sx,sy  = Position (relative to parent).
 *          sdx,dy = Size.
 *          bstart = Button start position.
 *          bend   = Button end position.
 *          dir    = WP_SBARH or WP_SBARV.
 *
 *      Out: *outptr = Pointer to WPSBAR.
 *
 *      Return:  0 = Ok.
 *              -3 = Programming error.
 *
 *      (C)2007-10-28 J. Kjellander
 *
 ******************************************************!*/

  {
   WPWIN               *winptr;
   WPLWIN              *lwinpt;
   WPIWIN              *iwinpt;
   Window               pxid,sbxid;
   int                  i,sb_id,wintype;
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   WPSBAR              *sbptr;

/*
***Get a C pointer to the parent's entry in wpwtab.
*/
   if ( (winptr=WPwgwp(pid)) == NULL ) return(-3);
/*
***Parent X ID.
*/
   wintype = winptr->typ;

   switch ( wintype )
     {
     case TYP_LWIN:
     lwinpt = (WPLWIN *)winptr->ptr;
     pxid   = lwinpt->id.x_id;
     break;

     case TYP_IWIN:
     iwinpt = (WPIWIN *)winptr->ptr;
     pxid   = iwinpt->id.x_id;
     break;

     default:
     return(-3);
     }
/*
***Create the X window.
*/
   xwina.background_pixel  = WPgcbu(WPwfpx(pxid),0);
   xwina.override_redirect = True;
   xwina.save_under        = False;

   xwinm = ( CWBackPixel | CWOverrideRedirect | CWSaveUnder );

   sbxid = XCreateWindow(xdisp,pxid,sx,sy,sdx,sdy,0,CopyFromParent,
                           InputOutput,CopyFromParent,xwinm,&xwina);
/*
***A slidebar window needs mouse input.
*/
   XSelectInput(xdisp,sbxid,ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
/*
***Crete the WPSBAR.
*/
   if ( (sbptr=(WPSBAR *)v3mall(sizeof(WPSBAR),"WPcreate_slidebar")) == NULL )
      return(-3);

   sbptr->id.w_id  = (wpw_id)NULL;
   sbptr->id.p_id  = pid;
   sbptr->id.x_id  = sbxid;

   sbptr->geo.x    = sx;
   sbptr->geo.y    = sy;
   sbptr->geo.dx   = sdx;
   sbptr->geo.dy   = sdy;
   sbptr->geo.bw   = 0;

   sbptr->butstart = bstart;
   sbptr->butend   = bend;
   sbptr->dir      = dir;
   sbptr->cback    = NULL;
/*
***Add the slidebar to the parent window.
*/
   switch ( wintype )
     {
     case TYP_LWIN:
     if ( dir == WP_SBARV ) sb_id = 0;
     else                   sb_id = 1;
     lwinpt->wintab[sb_id].typ = TYP_SBAR;
     lwinpt->wintab[sb_id].ptr = (char *)sbptr;
     sbptr->id.w_id = sb_id;
     break;

     case TYP_IWIN:
     i = 0;
     while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;
     if ( i == WP_IWSMAX ) return(-3);
     else sb_id = i;

     iwinpt->wintab[sb_id].typ = TYP_SBAR;
     iwinpt->wintab[sb_id].ptr = (char *)sbptr;
     sbptr->id.w_id = sb_id;
     break;
     }
/*
***Return ptr to slidebar.
*/
  *outptr = sbptr;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        void    WPexpose_slidebar(
        WPSBAR *sbptr)

/*      Expose handler for WPSBAR.
 *
 *      In: buttptr = C ptr to WPSBAR.
 *
 *      (C)2007-10-28 J.Kjellander
 *
 ******************************************************!*/

  {
   int     x1,y1,x2,y2;

/*
***Clear window.
*/
   XClearWindow(xdisp,sbptr->id.x_id);
/*
***Horizontal or vertical ?
*/
    switch ( sbptr->dir )
      {
/*
***Vertical.
*/
      case WP_SBARV:
      x1 = x2 = 0;
      y1 = 0;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 2;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->butend - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = sbptr->geo.dy - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = 2;
      x2 = sbptr->geo.dx;
      y1 = sbptr->butstart + 2;
      y2 = sbptr->butend   - 1;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND2));
      XFillRectangle(xdisp,sbptr->id.x_id,xgc,x1,y1,x2-x1,y2-y1);

      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      x1 = 3;
      x2 = sbptr->geo.dx - 3;
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 - (x2 - x1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      y1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 + (x2 - x1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y1);
      break;
/*
***Horizontal.
*/
      case WP_SBARH:
      x1 = 0;
      x2 = sbptr->geo.dx;
      y1 = y2 = 0;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = x2 = sbptr->butend - 1;
      y1 = 2;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = x2 = sbptr->geo.dx - 1;
      y1 = 0;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND1));
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x2,y2);

      x1 = sbptr->butstart + 2;
      x2 = sbptr->butend   - 1;
      y1 = 2;
      y2 = sbptr->geo.dy;
      XSetForeground(xdisp,xgc,WPgcol(WP_BGND2));
      XFillRectangle(xdisp,sbptr->id.x_id,xgc,x1,y1,x2-x1,y2-y1);

      XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
      y1 = 3;
      y2 = sbptr->geo.dy - 3;
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 - (y2 - y1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      x1 = sbptr->butstart + (sbptr->butend - sbptr->butstart)/2 + (y2 - y1)/2;
      XDrawLine(xdisp,sbptr->id.x_id,xgc,x1,y1,x1,y2);
      break;
      }
/*
***The end.
*/
    return;
  }

/********************************************************/
/********************************************************/

        bool          WPbutton_slidebar(
        WPSBAR       *sbptr,
        XButtonEvent *butev)

/*      Button handler for WPSBAR. Handles scroll.
 *
 *      In: sbptr = C ptr to WPSBAR.
 *          butev = X11 Button event.
 *
 *      (C)2007-10-28 J. Kjellander
 *
 ******************************************************!*/

  {
   int     butsize,mouse_pos,cb_pos,mouse_offset,
           sblen,wintype,diff;
   XEvent  event;
   WPWIN  *winptr;
   WPLWIN *lwinpt;
   WPIWIN *iwinpt;

/*
***Check that it was a ButtonPress.
*/
   if ( butev->type != ButtonPress ) return(FALSE);
/*
***Check that left mouse button was used.
*/
   if ( butev->button != 1 ) return(FALSE);
/*
***Check that the mouse was inside the button.
*/
   if ( sbptr->dir == WP_SBARV ) mouse_pos = butev->y;
   else                          mouse_pos = butev->x;

   if ( mouse_pos  < sbptr->butstart  ||
        mouse_pos  > sbptr->butend ) return(FALSE);
/*
***Grab the pointer.
*/
   XGrabPointer(xdisp,sbptr->id.x_id,FALSE,ButtonReleaseMask |
                ButtonMotionMask,GrabModeAsync,
                GrabModeAsync,None,None,CurrentTime);
/*
***Slidebar length.
*/
   if ( sbptr->dir == WP_SBARV ) sblen = sbptr->geo.dy;
   else                          sblen = sbptr->geo.dx;
/*
***Button size.
*/
   butsize = sbptr->butend - sbptr->butstart;
/*
***Offset between the mouse position
***and the start of the button (integer > 0).
*/
   mouse_offset = mouse_pos - sbptr->butstart;
/*
***Callback will be called when mouse has moved
***more than CBTHOLD. Initially move = 0.
*/
   cb_pos = mouse_pos;
/*
***Ptr to parent window.
*/
   winptr = WPwgwp(sbptr->id.p_id);
   wintype = winptr->typ;

   switch ( wintype )
     {
     case TYP_LWIN:
     lwinpt = (WPLWIN *)winptr->ptr;
     break;

     case TYP_IWIN:
     iwinpt = (WPIWIN *)winptr->ptr;
     break;

     default:
     return(FALSE);
     }
/*
***Start of event loop.
*/
loop:
   XNextEvent(xdisp,&event);

   switch ( event.type )
     {
/*
***Mouse move. Check that mouse is moving within slidebar
***limits.
*/
     case MotionNotify:
     if ( sbptr->dir == WP_SBARV ) mouse_pos = event.xmotion.y;
     else                          mouse_pos = event.xmotion.x;

/*
***Remove pending motion events.
*/
     while ( XCheckMaskEvent(xdisp,PointerMotionMask,&event) )
       {
       if ( sbptr->dir == WP_SBARV ) mouse_pos = event.xmotion.y;
       else                          mouse_pos = event.xmotion.x;
       }
/*
***Update button position.
*/
     if ( mouse_pos < mouse_offset )
       mouse_pos = mouse_offset;
     else if ( mouse_pos > sblen - butsize + mouse_offset )
       mouse_pos = sblen - butsize + mouse_offset;

     sbptr->butstart = mouse_pos - mouse_offset;
     sbptr->butend   = sbptr->butstart + butsize;
/*
***If pointer has moved more than CBTHOLD since last
***execution of a callback, update parent window by
***execute callback again.
*/
     diff = abs(cb_pos - mouse_pos);

     switch ( wintype )
       {
       case TYP_LWIN:
       if ( sbptr->cback != NULL )
         {
         if ( diff > CBTHOLD )
           {
           sbptr->cback(lwinpt);
           cb_pos = mouse_pos;
           }
         }
       break;

       case TYP_IWIN:
       if ( sbptr->cback != NULL )
         {
         if ( diff > CBTHOLD )
           {
           sbptr->cback(iwinpt);
           cb_pos = mouse_pos;
           }
         }
       break;
       }

     goto loop;
/*
***Button is released. Update parent window and exit.
*/
     case ButtonRelease:
     XUngrabPointer(xdisp,CurrentTime);
     while ( XPending(xdisp) ) XNextEvent(xdisp,&event);

     switch ( wintype )
       {
       case TYP_LWIN:
       if ( sbptr->cback != NULL ) sbptr->cback(lwinpt);
       break;

       case TYP_IWIN:
       if ( sbptr->cback != NULL ) sbptr->cback(iwinpt);
       break;
       }

     return(TRUE);
/*
***Let's just skip all other events.
*/
     default:
     goto loop;
     }
  }

/********************************************************/
/*!******************************************************/

        short   WPdelete_slidebar(
        WPSBAR *sbptr)

/*      Delete a WPSBAR.
 *
 *      In: sbptr = C ptr to WPSBAR.
 *
 *      (C)2007-10-28, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Release allocated C memory.
*/
   v3free((char *)sbptr,"WPdelete_slidebar");
/*
***The end.
*/
   return(0);
  }

/********************************************************/
