/**********************************************************************
*
*    WPtooltip.c
*    ===========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPorder_tooltip();   Ask for a tooltip
*    WPclear_tooltip();   Delete a tooltip
*    WPshow_tooltip();    Display a tooltip
*    WPexpose_tooltip();  Expose tooltip
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
#include <unistd.h>
#include <signal.h>
#include <string.h>

static int    x,y;              /* Tooltip window position */
static char   tipstr[81];       /* Tooltip text */
static Window tooltip_id = 0;   /* Tooltip window X-id */

/*!*******************************************************/

       void  WPorder_tooltip(
       int   ix,
       int   iy,
       char *tip)

/*     Sets WPshow_tooltip() as signal handler and
 *     orders a signal within 1 second. This means 
 *     that the tooltip will be displayed with 1 
 *     second delay after the button crossing event.
 *
 *     In:  ix,iy = Position
 *          tip   = Tooltip text
 *
 *     (C)2007-03-08 J. Kjellander
 *
 *******************************************************!*/

 {

/*
***Save ix,iy and tip in static variables to be used
***by WPshow_tooltip() when the signal comes.
*/
   x = ix;
   y = iy;
   strcpy(tipstr,tip);
/*
***Set up WPshow_tooltip() as signal handler for SIGALRM.
*/
   signal(SIGALRM,WPshow_tooltip);
/*
***Order an alarm a short time from now.
*/
   alarm(1);
 }

/*********************************************************/
/*!*******************************************************/

       void WPclear_tooltip()

/*     Clears an active tooltip.
 *
 *     (C)2007-03-08 J. Kjellander
 *
 *******************************************************!*/

 {
/*
***Kill any pending order.
*/
   alarm(0);
/*
***Kill active tooltip.
*/
   if ( tooltip_id > 0 )
     {
     XDestroyWindow(xdisp,tooltip_id);
     tooltip_id = 0;
     XFlush(xdisp);
     }
 }

/*********************************************************/
/*!******************************************************/

        void WPshow_tooltip()

/*      Displays a tooltip and kill it after 5 seconds.
 *
 *      (C)2007-03-08 J. Kjellander
 *
 ******************************************************!*/

 {
   int                  dx,dy;
   XSetWindowAttributes xwina;
   unsigned long        xwinm;

/*
***Clear any pending tooltip.
*/
   WPclear_tooltip();
/*
***Size of tooltip window.
*/
   dx  = WPstrl(tipstr) + 4;
   dy  = WPstrh() + 2;
/*
***Create the tool tip window.
*/
   xwina.background_pixel  = WPgcol(WP_TTIP);
   xwina.border_pixel      = WPgcol(WP_FGND);
   xwina.override_redirect = True;
   xwina.save_under        = False;

   xwinm = ( CWBackPixel        | CWBorderPixel |
             CWOverrideRedirect | CWSaveUnder );

   tooltip_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),x,y,dx,dy,1,CopyFromParent,
                              InputOutput,CopyFromParent,xwinm,&xwina);

   XMapWindow(xdisp,tooltip_id);

   WPexpose_tooltip();
/*
***Order a kill after 5 seconds.
*/
   signal(SIGALRM,WPclear_tooltip);
   alarm(5);
 }

/********************************************************/
/*!******************************************************/

        void WPexpose_tooltip()

/*      Expose handler for tooltip window.
 *
 *      (C)2007-05-28 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( tooltip_id > 0 )
     {
     XSetBackground(xdisp,xgc,WPgcol(WP_TTIP));
     XSetForeground(xdisp,xgc,WPgcol(WP_FGND));
     XDrawImageString(xdisp,tooltip_id,xgc,2,WPstrh()-1,tipstr,strlen(tipstr));
     XSetBackground(xdisp,xgc,WPgcol(WP_BGND2));
     XFlush(xdisp);
     }
 }

/********************************************************/
