/**********************************************************************
*
*    wpMCWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_mcwin();  Create WPMCWIN
*    WPaddmess_mcwin(); Add a message to WPMCWIN
*    WPclear_mcwin();   Clears the message line
*    WPexpose_mcwin();  Expose handler for WPMCWIN
*    WPbutton_mcwin();  Button handler (resize) for WPMCWIN
*    WPkey_mcwin();     Key handler for WPMCWIN command edits.
*    WPunfocus_mcwin(); Removes focus from WPMCWIN command edits.
*    WPdelete_mcwin();  Delete WPMCWIN
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
#include <X11/xpm.h>

extern char jobnam[];

/*
***The message buffer.
*/

#define WP_MCWBUFSIZ  50

static char message_buffer[WP_MCWBUFSIZ][V3STRLEN+1];   /* Current message text */
static int  message_types[WP_MCWBUFSIZ];                /* Current message types */
static int  oldest_message;                              /* Offset to first message */
static int  message_count = 0;                           /* Message count */

/*
***The message icons.
*/
static Pixmap message_icon = 0;             /* The icon used for messages */
static Pixmap prompt_icon = 0;              /* The icon used for prompts */
static Pixmap error_icon = 0;               /* The icon used for error mesages */
static int    icon_dx = 10;                 /* Icon with */
static int    icon_dy = 10;                 /* Icon heihght */
static GC     icon_gc = 0;                  /* X GC with clip mask for transparency */
/*
***Prototypes for internal functions.
*/

/********************************************************/

        short   WPcreate_mcwin(
        wpw_id  grw_id,
        int     size)

/*      Create WPMCWIN window.
 *
 *      In: grw_id = ID of parent WPGWIN/WPRWIN
 *          size   = Size in Y direction
 *
 *      (C)2007-05-30 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 fnam[V3PTHLEN];
    XSetWindowAttributes xwina;
    XpmAttributes        attributes;
    XGCValues            values;
    unsigned long        xwinm;
    int                  mcw_x,mcw_y,mcw_dx,mcw_dy,resize_x,resize_y,
                         resize_dx,resize_dy,grw_dx,grw_dy,edit_x,edit_y,
                         edit_dx,edit_dy,status;
    Window               messcom_xid,parent_xid,resize_xid;
    Pixmap               icon_mask;
    WPWIN               *winptr;
    WPMCWIN             *mcwptr;
    WPGWIN              *gwinpt;
    WPRWIN              *rwinpt;
    WPEDIT              *editpt;

/*
***Get X-ID and size of the parent WPGWIN/WPRWIN.
*/
   winptr = WPwgwp((wpw_id)grw_id);

   if      ( winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     parent_xid = gwinpt->id.x_id;
     grw_dx = gwinpt->geo.dx;
     grw_dy = gwinpt->geo.dy;
     }
   else if ( winptr->typ == TYP_RWIN )
     {
     rwinpt = (WPRWIN *)winptr->ptr;
     parent_xid = rwinpt->id.x_id;
     grw_dx = rwinpt->geo.dx;
     grw_dy = rwinpt->geo.dy;
     }
   else return(-3);
/*
***Position of WPMCWIN relative to graphical window.
*/
   mcw_x = 0;
   mcw_y = grw_dy - size;
   mcw_dx = grw_dx;
   mcw_dy = size;
/*
***Set colors etc.
*/
    xwina.background_pixel  = WPgcol(WP_BGND2);
    xwina.border_pixel      = BlackPixel(xdisp,xscr);
    xwina.override_redirect = False;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  
/*
***Create the Message and Command main window.
*/
    messcom_xid = XCreateWindow(xdisp,parent_xid,mcw_x,mcw_y,mcw_dx,mcw_dy,0,
                            CopyFromParent,InputOutput,
                            CopyFromParent,xwinm,&xwina);
/*
***Allow (right-)button press in the message window.
*/
    XSelectInput(xdisp,messcom_xid,ButtonPressMask);
/*
***Create the command edit.
*/
    edit_dx = 20*WPstrl("A");
    edit_x  = mcw_dx - edit_dx - 5;
    edit_dy = WPstrh() + 5;
    edit_y  = mcw_dy - edit_dy - 5;

    WPwced(messcom_xid,edit_x,edit_y,edit_dx,edit_dy,1,"",20,&editpt);
    XMapWindow(xdisp,editpt->id.x_id);
/*
***Create the 5 pixel resize window with the vertical resize cursor.
*/
    xwina.background_pixel = WPgcol(0);
    xwina.cursor            = xgcur3;
    xwinm |= CWCursor;

    resize_x   = 0;
    resize_y   = mcw_y - 5;
    resize_dx  = mcw_dx;
    resize_dy  = 5;
    resize_xid = XCreateWindow(xdisp,parent_xid,resize_x,resize_y,resize_dx,resize_dy,0,
                                CopyFromParent,InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Set the input mask for the resize window.
*/
    XSelectInput(xdisp,resize_xid,ButtonPressMask | ButtonReleaseMask |
                                   ButtonMotionMask);
/*
***Create the WPMCWIN window.
*/
    if ( (mcwptr=(WPMCWIN *)v3mall(sizeof(WPMCWIN),"WPwcmcw")) == NULL )
      return(erpush("WP1062","WPMCWIN"));

    mcwptr->parent_id   = grw_id;
    mcwptr->messcom_xid = messcom_xid;
    mcwptr->resize_xid  = resize_xid;
    mcwptr->cmdptr      = editpt;

    mcwptr->geo.x  = mcw_x;
    mcwptr->geo.y  = mcw_y;
    mcwptr->geo.dx = mcw_dx;
    mcwptr->geo.dy = mcw_dy;
/*
***Update the graphical window.
*/
   if   ( winptr->typ == TYP_GWIN ) gwinpt->mcw_ptr = mcwptr;
   else                             rwinpt->mcw_ptr = mcwptr;
/*
***Load message icon pixmaps. The code below should force
***this to happen for the first WPMCWIN and then reuse
***pixmaps for additional WPMCWIN's. Note that all three
***icon's must be of same size and use the same shapemask
***(or none).
*/
    attributes.valuemask = XpmSize;

    if ( message_icon == 0 )
      {
      strcpy(fnam,IGgenv(VARKON_ICO));
      strcat(fnam,"Varkon_message.xpm");
      status = XpmReadFileToPixmap(xdisp,messcom_xid,fnam,&message_icon,
                                   &icon_mask,&attributes);
      if ( status != XpmSuccess ) message_icon = 0;
      icon_dx = attributes.width;
      icon_dy = attributes.height;
      }

    if ( prompt_icon == 0 )
      {
      strcpy(fnam,IGgenv(VARKON_ICO));
      strcat(fnam,"Varkon_prompt.xpm");
      status = XpmReadFileToPixmap(xdisp,messcom_xid,fnam,&prompt_icon,
                                   &icon_mask,&attributes);
      if ( status != XpmSuccess ) prompt_icon = 0;
      }

    if ( error_icon == 0 )
      {
      strcpy(fnam,IGgenv(VARKON_ICO));
      strcat(fnam,"Varkon_error.xpm");
      status = XpmReadFileToPixmap(xdisp,messcom_xid,fnam,&error_icon,
                                   &icon_mask,&attributes);
      if ( status != XpmSuccess ) error_icon = 0;
      }
/*
***Create a private GC for WPMCWIN icon's. If Icons have shapemasks
***set the GC Clipmask accordingly.
*/
    if ( message_icon != 0  &&  icon_gc == 0 )
      {
      icon_gc = XCreateGC(xdisp,messcom_xid,0,&values);
      if ( icon_mask != 0 ) XSetClipMask(xdisp,icon_gc,icon_mask);
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        short  WPaddmess_mcwin(
        char  *message,
        int    type)

/*      Add a message to the message window.
 *
 *      In: message = C ptr to message string.
 *          type    = Type of message, WP_MESSAGE
 *                                     WP_ERROR
 *                                     WP_PROMPT
 *                                     WP_CLEARED
 *
 *      (C)2007-06-18 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i,current;
   WPGWIN  *gwinptr;
   WPRWIN  *rwinptr;
   WPMCWIN *mcwptr;

/*
***If the current message is WP_CLEARED, reuse that position.
*/
   if ( message_count > 0 )
     {
     current = oldest_message + message_count - 1;
     if ( current > WP_MCWBUFSIZ-1 ) current -= WP_MCWBUFSIZ;

     if ( message_types[current] == WP_CLEARED )
       {
       strcpy(message_buffer[current],message);
       message_types[current] = type;
       goto expose;
       }
     }
/*
***Else, add the message to the message buffer.
***message_buffer is a circular list with
***current size = message_count, and the
***oldest message at index oldest_message.
*/
   if ( message_count < WP_MCWBUFSIZ )
     {
     strcpy(message_buffer[message_count],message);
     message_types[message_count] = type;
     oldest_message = 0;
   ++message_count;
     }
   else if ( oldest_message == WP_MCWBUFSIZ-1 )
     {
     strcpy(message_buffer[oldest_message],message);
     message_types[oldest_message] = type;
     oldest_message = 0;
     }
   else
     {
     strcpy(message_buffer[oldest_message],message);
     message_types[oldest_message] = type;
   ++oldest_message;
     }
/*
***Expose all WPMCWIN's.
*/
expose:
  for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinptr = (WPGWIN *)wpwtab[i].ptr;
         mcwptr = gwinptr->mcw_ptr;
         XClearWindow(xdisp,mcwptr->messcom_xid);
         WPexpose_mcwin(mcwptr);
         }
       else if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinptr = (WPRWIN *)wpwtab[i].ptr;
         mcwptr = rwinptr->mcw_ptr;
         XClearWindow(xdisp,mcwptr->messcom_xid);
         WPexpose_mcwin(mcwptr);
         }
       }
     }

   return(0);
  }

/********************************************************/
/********************************************************/

        void WPclear_mcwin()

/*      Move all current messages one step up in the
 *      history and clear the message line.
 *
 *      (C)2007-04-30 J. Kjellander
 *
 ******************************************************!*/

  {
   WPaddmess_mcwin("",WP_CLEARED);

   return;
  }

/********************************************************/
/********************************************************/

        bool     WPexpose_mcwin(
        WPMCWIN *mcwptr)

/*      Expose handler for WPMCWIN.
 *
 *      In: mcwptr = C ptr to WPMCWIN.
 *
 *      (C)2007-05-30 J. Kjellander
 *
 *      2007-09-22 Flush added, J.Kjellander
 *
 ******************************************************!*/

  {
   char *mptr;
   int   i,nlines,x_text,y_text,x_icon,y_icon,start,stop,mindex,type;

/*
***A fat 3D line in the resize window.
*/
   XSetForeground(xdisp,xgc,WPgcol(WP_BGND2));
   XDrawLine(xdisp,mcwptr->resize_xid,xgc, 0,1,mcwptr->geo.dx,1);

   XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
   XDrawLine(xdisp,mcwptr->resize_xid,xgc, 0,2,mcwptr->geo.dx,2);

   XSetForeground(xdisp,xgc,WPgcol(WP_BOTS));
   XDrawLine(xdisp,mcwptr->resize_xid,xgc, 0,3,mcwptr->geo.dx,3);

   XSetForeground(xdisp,xgc,WPgcol(0));
   XDrawLine(xdisp,mcwptr->resize_xid,xgc, 0,4,mcwptr->geo.dx,4);
/*
***If the window is minimized we merge with the border of the
***graphical window.
*/
   if ( mcwptr->geo.dy == 1 )
     {
     XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
     XDrawLine(xdisp,mcwptr->resize_xid,xgc, 0,4,mcwptr->geo.dx,4);
     return(TRUE);
     }
/*
***Set the forground color for writing.
*/
   XSetBackground(xdisp,xgc,WPgcol(WP_BGND2));
   XSetForeground(xdisp,xgc,WPgcol(WP_FGND));
/*
***How many lines will fit in the window ?
*/
   nlines = mcwptr->geo.dy/WPstrh();
/*
***Where do we start and end.
*/
   start = oldest_message + message_count - 1;
   if ( start > WP_MCWBUFSIZ-1 ) start -= WP_MCWBUFSIZ;

   stop = message_count;
   if ( stop > nlines ) stop = nlines;
/*
***Write.
*/
   x_icon = 2;
   x_text = x_icon + icon_dx + 5;

   for ( i=0; i<stop; ++i )
     {
     y_text = mcwptr->geo.dy - i*WPstrh() - 4;
     y_icon = y_text - WPstrh() + 5;

     if ( start-i < 0 ) mindex = start - i + WP_MCWBUFSIZ;
     else               mindex = start - i;

     mptr = message_buffer[mindex];
     type = message_types[mindex];

     if ( strlen(mptr) > 0 )
       {
/*
***Display leading icon.
*/
       XSetClipOrigin(xdisp,icon_gc,x_icon,y_icon);

       switch ( type )
         {
         case WP_ERROR:
         if ( error_icon != 0 )
           XCopyArea(xdisp,error_icon,mcwptr->messcom_xid,icon_gc,
                     0,0,icon_dx,icon_dy,x_icon,y_icon);
         break;

         case WP_PROMPT:
         if ( prompt_icon != 0 ) 
           XCopyArea(xdisp,prompt_icon,mcwptr->messcom_xid,icon_gc,
                     0,0,icon_dx,icon_dy,x_icon,y_icon);
         break;

         case WP_MESSAGE:
         if ( message_icon != 0 ) 
           XCopyArea(xdisp,message_icon,mcwptr->messcom_xid,icon_gc,
                     0,0,icon_dx,icon_dy,x_icon,y_icon);
         break;
         }
       }
/*
***Display the message.
*/
     XDrawImageString(xdisp,mcwptr->messcom_xid,xgc,x_text,y_text,mptr,strlen(mptr));
     XSetForeground(xdisp,xgc,WPgcol(WP_BGND3));
     }
/*
***The Command prompt.
*/
   XSetForeground(xdisp,xgc,WPgcol(WP_FGND));
   XDrawImageString(xdisp,mcwptr->messcom_xid,xgc,mcwptr->cmdptr->geo.x,
                    mcwptr->geo.dy - mcwptr->cmdptr->geo.dy - 10,
                    "Command:",strlen("Command:"));
/*
***Expose the command WPEDIT window.
*/
   WPxped(mcwptr->cmdptr);
/*
***Flush.
*/
   XFlush(xdisp);
/*
***The end.
*/
   return(TRUE);
  }

/********************************************************/
/********************************************************/

        bool          WPbutton_mcwin(
        WPMCWIN      *mcwptr,
        XButtonEvent *butev)

/*      Button handler for WPMCWIN. Handles right click
 *      and resize.
 *
 *      In: mcwptr = C ptr to WPMCWIN.
 *
 *      (C)2007-05-29 J. Kjellander
 *
 *      2008-03-29 Right click, J.Kjellander
 *
 ******************************************************!*/

  {
   int     i,mindex,mcw_org_y,mcw_org_dy,mouse_y,
           grw_dy;
   char    message[V3STRLEN+10];
   XEvent  event;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Right click.
*/
   if ( butev->window == mcwptr->messcom_xid )
     {
     if ( butev->type == ButtonPress  &&  butev->button == 3 )
       {
       if ( WPialt("Show the full list ?","Yes","No",FALSE) )
         {
         strncpy(message,"Message list for: ",19);
         strncat(message,jobnam,JNLGTH+1);
         WPinla(message);

         for ( i=0; i<message_count; ++i )
           {
           if ( i < WP_MCWBUFSIZ ) mindex = oldest_message + i;
           else                    mindex = oldest_message + i - WP_MCWBUFSIZ;

           switch ( message_types[mindex] )
             {
             case WP_ERROR:   strncpy(message,"ERROR: ",8);    break;
             case WP_PROMPT:  strncpy(message,"PROMPT: ",9);   break;
             case WP_MESSAGE: strncpy(message,"MESSAGE: ",10); break;
             default:         strncpy(message,"",1);           break;
             }
           strncat(message,message_buffer[mindex],V3STRLEN+10);
           WPalla(message,1);
           }
         WPexla(TRUE);
         }
       }
     return(TRUE);
     }
/*
***Resize.
***Remember original size and assume first mouse position = 0.
*/
   mcw_org_y  = mcwptr->geo.y;
   mcw_org_dy = mcwptr->geo.dy;
   mouse_y    = 0;
/*
***What is the size of the graphical parent window ?
***This constrains the size of the WPMCWIN.
*/
   winptr = WPwgwp((wpw_id)mcwptr->parent_id);

   if      ( winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     grw_dy = gwinpt->geo.dy;
     }
   else if ( winptr->typ == TYP_RWIN )
     {
     rwinpt = (WPRWIN *)winptr->ptr;
     grw_dy = rwinpt->geo.dy;
     }
/*
***Start of event loop.
*/
loop:
   XNextEvent(xdisp,&event);

   switch ( event.type )
     {
/*
***Mouse move.
*/
     case MotionNotify:
     mouse_y  = event.xmotion.y;
     while ( XPending(xdisp) )
       {
       XNextEvent(xdisp,&event);
       if ( event.type == MotionNotify )
         {
         mouse_y  = event.xmotion.y;
         }
       }
     mcwptr->geo.y  += mouse_y ;
     if ( mcwptr->geo.y < 5 ) mcwptr->geo.y = 5;
     if ( mcwptr->geo.y > mcw_org_y + mcw_org_dy )
       {
       mcwptr->geo.y  = grw_dy - 1;
       mcwptr->geo.dy = 1;
       }
     else mcwptr->geo.dy = grw_dy - mcwptr->geo.y;
/*
***Move the resize window and move and resize the Message and Command window.
*/
     if ( mcwptr->geo.dy > 0 )
       {
       XMoveWindow      (xdisp,mcwptr->resize_xid,mcwptr->geo.x,
                                                   mcwptr->geo.y - 5);
       XMoveResizeWindow(xdisp,mcwptr->messcom_xid,mcwptr->geo.x,
                                                    mcwptr->geo.y,
                                                    mcwptr->geo.dx,
                                                    mcwptr->geo.dy);
       XMoveResizeWindow(xdisp,mcwptr->cmdptr->id.x_id,mcwptr->cmdptr->geo.x,
                                                        mcwptr->geo.dy - mcwptr->cmdptr->geo.dy - 5,
                                                        mcwptr->cmdptr->geo.dx,
                                                        mcwptr->cmdptr->geo.dy);
       }
     goto loop;
/*
***Button is released. Empty the event que, make a final expose
***on the WPMCWIN and reconfigure and repaint the parent graphics window.
***TODO reconfigure WPRWIN.
*/
     case ButtonRelease:
     while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
     WPexpose_mcwin(mcwptr);
     if      ( winptr->typ == TYP_GWIN )
       {
       gwinpt->vy.scrwin.ymin = mcwptr->geo.dy;
       WPnrgw(gwinpt);
       WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);
       }
     else if ( winptr->typ == TYP_RWIN )
       {
       WPrepaint_RWIN((wpw_id)rwinpt->id.w_id,FALSE);
       }
     return(TRUE);
/*
***Expose.
*/
     case Expose:
     WPexpose_mcwin(mcwptr);
     goto loop;
/*
***Let's just skip all other events.
*/
     default:
     goto loop;
     }
  }

/********************************************************/
/*!******************************************************/

        bool WPkey_mcwin(XKeyEvent *keyev)

/*      Key handler for WPMCWIN.
 *
 *      (C)2007-05-09 J. Kjellander
 *
 ******************************************************!*/

  {
   char     c,macro_name[JNLGTH+1],mesbuf[V3STRLEN];
   short    symbol;
   int      i;
   bool     hit;
   WPGWIN  *gwinptr;
   WPRWIN  *rwinptr;
   WPMCWIN *mcwptr;

/*
***Loop through all WPMCWIN's.
*/
  for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinptr = (WPGWIN *)wpwtab[i].ptr;
         mcwptr = gwinptr->mcw_ptr;
         hit = TRUE;
         }
       else if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinptr = (WPRWIN *)wpwtab[i].ptr;
         mcwptr = rwinptr->mcw_ptr;
         hit = TRUE;
         }
       else hit = FALSE;
/*
***Window found. Check what symbol the key maps to. Editing symbols
***are passed through to WPkeed(). Return calls a macro.
*/
       if ( hit )
         {
         if ( mcwptr->cmdptr->focus )
           {
           WPmap_key(keyev,&symbol,&c);
           switch ( symbol )
             {
             case SMBCHAR:
             case SMBBACKSP:
             case SMBDELETE:
             case SMBLEFT:
             case SMBBEGL:
             case SMBRIGHT:
             case SMBENDL:
             WPkeed(mcwptr->cmdptr,keyev,SLEVEL_NONE);
             return(TRUE);
/*
***MACRO call. Clear command line and add a message.
*/
             case SMBRETURN:
             strncpy(macro_name,mcwptr->cmdptr->str,JNLGTH);
             macro_name[JNLGTH] = '\0';
             WPunfocus_mcwin();
             mcwptr->cmdptr->str[0] = '\0';
             mcwptr->cmdptr->curpos = 0;
             XClearWindow(xdisp,mcwptr->cmdptr->id.x_id);
             WPxped(mcwptr->cmdptr);
             sprintf(mesbuf,"Command:%s",macro_name);
             WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***Execute macro.
*/
             if ( IGcall_macro(macro_name) < 0 ) errmes();

             return(TRUE);
             }
           }
         }
       }
     }

   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        void WPunfocus_mcwin()

/*      Removes focus from any WPMCWIN command edit
 *      that might have focus.
 *
 *      (C)2007-06-18 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   WPGWIN  *gwinptr;
   WPRWIN  *rwinptr;
   WPMCWIN *mcwptr;

/*
***Loop through all WPMCWIN's.
*/
  for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinptr = (WPGWIN *)wpwtab[i].ptr;
         mcwptr = gwinptr->mcw_ptr;
         if ( mcwptr->cmdptr->focus )
           {
           WPfoed(mcwptr->cmdptr,FALSE);
           }
         }
       else if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinptr = (WPRWIN *)wpwtab[i].ptr;
         mcwptr = rwinptr->mcw_ptr;
         if ( mcwptr->cmdptr->focus )
           {
           WPfoed(mcwptr->cmdptr,FALSE);
           }
         }
       }
     }

   return;
  }

/********************************************************/
/*!******************************************************/

        short    WPdelete_mcwin(
        WPMCWIN *mcwptr)

/*      Deletes a WPMCWIN.
 *
 *      In: mcwptr = C ptr to WPMCWIN.
 *
 *      (C)2007-04-30 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Release allocated C memory.
*/
   v3free((char *)mcwptr,"WPdlmcw");
   return(0);
  }

/********************************************************/
