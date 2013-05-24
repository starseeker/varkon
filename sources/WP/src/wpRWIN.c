/**********************************************************************
*
*    wpRWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPrenw();          WP:s rendering window
*    WPwcrw();          Creates a WPRWIN
*    WPbtrw();          Button routine for a WPRWIN
*    WPxprw();          Expose routine for a WPRWIN
*    WPcrrw();          Crossing routine for a WPRWIN 
*    WPcorw();          Configure routine for a WPRWIN 
*    WPcmrw();          Client message for a WPRWIN
*    WPrepaint_RWIN();  Update the contents of a WPRWIN
*    WPerrw();          Erase one or more WPRWIN
*    WPdlrw();          Deletes a WPRWIN
*   *WPgrwp();          Map X-id to WPRWIN-C-pointer
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
#include "../../EX/include/EX.h"
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <string.h>
#include "../include/v3icon.h"

#define MCWIN_DY   45         /* Default size of Message_and_command window */
#define ROT         0         /* Dynamic rotation */
#define SCALE       1         /* Dynamic scaling */
#define PAN         2         /* Dynamic pan */

extern char jobnam[];

static short setup_ogl(WPRWIN *rwinpt);
static short get_visinfo(WPRWIN *rwinpt);
static void  init_colors(WPRWIN *rwinpt);
static void  create_toolbar(WPRWIN *rwinpt);
static void  scale_on(WPRWIN *rwinpt);
static void  pan_on(WPRWIN *rwinpt);
static void  rot_on(WPRWIN *rwinpt);
static void  persp_on(WPRWIN *rwinpt);
static void  light_on(WPRWIN *rwinpt);
static void  clip_on(WPRWIN *rwinpt);
static void  line_fill(WPRWIN *rwinpt);

/*!******************************************************/

        short WPrenw()

/*      Creates a default size WPRWIN window.
 *
 *      Felkod:
 *
 *      (C)microform ab 1997-12-21 J. Kjellander
 *
 *      1998-10-30 X-Resurser, J.Kjellander
 *      2007-01-12 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   short        status;
   int          x,y;
   long         flags;
   char         title[V3STRLEN+1];
   char        *type[20];
   unsigned int dx,dy,height,width;
   DBint        id;
   XrmValue     value;

/*
***H�rdprogrammerad storlek och placering.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);

   x  = 0.3*width - 12;
   y  = 50;
   dx = 0.6*width - 15;
   dy = 0.7*height;
/*
***V�rden fr�n resursdatabasen.
***Kolla att resultatet hamnar p� sk�rmen.
*/
   if ( XrmGetResource(xresDB,"varkon.rwin.geometry",
                              "Varkon.rwin.geometry",
                               type,&value) )
     {
     flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
     if ( XValue & flags )
       if ( XNegative & flags ) x = width + x - dx;
     if ( YValue & flags )
       if ( YNegative & flags ) y = height + y - dy;
     }

   WPposw(x,y,dx+10,dy+25,&x,&y);
/*
***F�nstertitel.
*/
   if ( !WPgrst("varkon.rwin.title",title) )
                  strcpy(title,"VARKON Dynamic Rendering");
/*
***Create the window.
*/
   status = WPwcrw(x,y,dx,dy,title,&id);
/*
***Update the contents from DB.
*/
   WPrepaint_RWIN(id,TRUE);

   return(status);
 }

/********************************************************/
/********************************************************/

        short  WPwcrw(
        int    x,
        int    y,
        int    dx,
        int    dy,
        char  *label,
        DBint *id)

/*      Create a WPRWIN.
 *
 *      In: x     = Position in X.
 *          y     = Position in Y.   
 *          dx    = Size in X-dir.
 *          dy    = Size in Y-dir.
 *          label = Window title.
 *
 *      Out: *id = Valid entry in wpwtab.
 *
 *      Error: WP1052 = wpwtab full.
 *             WP1062 = Error from malloc().
 *             WP1612 = Can't init OpenGL.
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *
 *      1998-03-24 Omarbetad, J.Kjellander
 *      1998-11-21 actview    G. Liden
 *      2007-06-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   short                status;
   unsigned char        dlist[2];
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   XSizeHints           xhint;
   Window               xwin_id;
   XEvent               event;
   XWMHints             wmhint;
   Pixmap               IconPixmap,SavePixmap;
   int                  i,width,height,margin;
   unsigned int         depth;
   XGCValues            values;
   GC                   Win_gc,Rub_gc;
   WPWIN               *winptr;
   WPRWIN              *rwinpt;
   WPGWIN              *mainpt;
   char                *type[20];
   XrmValue             value;

/*
***Allocate C memory for a WPRWIN.
*/
   if ( (rwinpt=(WPRWIN *)v3mall(sizeof(WPRWIN),"WPwcrw")) == NULL )
     return(erpush("WP1062",label));
/*
***Select a visual.
*/
   if ( (status=get_visinfo(rwinpt)) < 0 )
     {
     v3free((char *)rwinpt,"WPwcrw");
     return(erpush("WP1612",""));
     }
/*
***Create rendering context.
*/
   if ( (rwinpt->rc=glXCreateContext(xdisp,rwinpt->pvinfo,NULL,TRUE)) == NULL )
     {
     v3free((char *)rwinpt,"WPwcrw");
     erpush("WP1643","");
     return(erpush("WP1612",""));
     }
/*
***Get a free window ID.
*/
   if ( (*id=WPwffi()) < 0 )
     {
     v3free((char *)rwinpt,"WPwcrw");
     return(erpush("WP1052",label));
     }
/*
***Create colormap.
*/
   rwinpt->colmap = XCreateColormap(xdisp,
                                    RootWindow(xdisp,rwinpt->pvinfo->screen),
                                    rwinpt->pvinfo->visual,AllocNone);
/*
***Skapa f�rger f�r tryckknappar som g�r att anv�nda
***i den valda visualen. Vi kan inte anv�nda WPgcol()
***eftersom den inte s�kert anv�nder den visual som
***OpenGL-f�nstret har.
*/
   init_colors(rwinpt);
/*
***Window attributes.
*/
   xwina.colormap          = rwinpt->colmap; 
   xwina.background_pixel  = rwinpt->bgnd2;
   xwina.border_pixel      = 0;
   xwina.override_redirect = False;
   xwina.save_under        = False;
/*
***Mask.
*/
   xwinm = ( CWBackPixel        | CWBorderPixel |
             CWOverrideRedirect | CWSaveUnder   | CWColormap );
/*
***Create a window with the same visual as OpenGL:s RC.
*/
   depth  = rwinpt->pvinfo->depth;

   xwin_id = XCreateWindow(xdisp,
                           RootWindow(xdisp,rwinpt->pvinfo->screen),
                           x,y,dx,dy,
                           0,
                           depth,
                           InputOutput,
                           rwinpt->pvinfo->visual,
                           xwinm,&xwina);
/*
***Send hints to the Window manager.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);

   xhint.flags  = USPosition | USSize | PMinSize | PMaxSize;   
   xhint.x      = x;
   xhint.y      = y;
   xhint.width  = dx;
   xhint.height = dy;  
   xhint.min_width  = 5;
   xhint.min_height = 5;  
   xhint.max_width  = width - 5;
   xhint.max_height = height - 5;
   XSetNormalHints(xdisp,xwin_id,&xhint);
/*
***Window title and icon text.
*/
   XStoreName(xdisp,xwin_id,label);
   XSetIconName(xdisp,xwin_id,jobnam);
/*
***Input.
*/
   XSelectInput(xdisp,xwin_id,RWEM_NORM);
/*
***Create icon.
*/
  IconPixmap = XCreateBitmapFromData(xdisp,xwin_id,(char *)v3icon_bits,
                                      v3icon_width,v3icon_height);
  wmhint.icon_pixmap = IconPixmap;
  wmhint.flags       = IconPixmapHint;
  XSetWMHints(xdisp,xwin_id,&wmhint);
/*
***Don't let Varkon be killed by the Window Manager.
*/
   WPsdpr(xwin_id);
/*
***Create one normal X11 GC and one for rubber banding.
*/
   Win_gc = XCreateGC(xdisp,xwin_id,0,&values);
   values.graphics_exposures = False;
   XChangeGC(xdisp,Win_gc,GCGraphicsExposures,&values);

   Rub_gc = XCreateGC(xdisp,xwin_id,0,&values);
   XSetLineAttributes(xdisp,Rub_gc,2,LineOnOffDash,CapButt,JoinBevel);
   dlist[0] = (unsigned char)((double)width/40.0);
   dlist[1] = (unsigned char)((double)dlist[0]/2.5);
   XSetDashes(xdisp,Rub_gc,(unsigned int)0,(char *)dlist,2);
   XSetFunction(xdisp,Rub_gc,GXxor);
   XSetBackground(xdisp,Rub_gc,WPgcol(0));
   XSetForeground(xdisp,Rub_gc,WPgcol(3));
/*
***Create pixmap for 'save under'.
*/
   SavePixmap = XCreatePixmap(xdisp,xwin_id,width,height,depth);
/*
***Set Background/forground colors and clear the window.
*/
   XSetBackground(xdisp,Win_gc,rwinpt->bgnd2);
   XSetForeground(xdisp,Win_gc,rwinpt->bgnd2);
   XFillRectangle(xdisp,SavePixmap,Win_gc,0,0,width,height);
   XSetForeground(xdisp,Win_gc,rwinpt->fgnd);
/*
***Register WPRWIN data.
*/
   rwinpt->id.w_id = *id;
   rwinpt->id.p_id =  0;
   rwinpt->id.x_id =  xwin_id;

   rwinpt->geo.x  =  x;
   rwinpt->geo.y  =  y;
   rwinpt->geo.dx =  dx;
   rwinpt->geo.dy =  dy;
   rwinpt->geo.psiz_x = XDisplayWidthMM(xdisp,xscr) /
                        (double)DisplayWidth(xdisp,xscr);
   rwinpt->geo.psiz_y = XDisplayHeightMM(xdisp,xscr) /
                        (double)DisplayHeight(xdisp,xscr);

   rwinpt->savmap = SavePixmap;
   rwinpt->win_gc = Win_gc;
   rwinpt->rub_gc = Rub_gc;
/*
***WM trix data.
*/
   rwinpt->reprnt = FALSE;
   rwinpt->wmandx = 0;
   rwinpt->wmandy = 0;
/*
***Init view name.
*/
   rwinpt->vy.name[0] = '\0';
/*
***Screen window/viewport size and margins.
*/
   rwinpt->vy.scrwin.xmin = 0;
   rwinpt->vy.scrwin.ymin = MCWIN_DY;
   rwinpt->vy.scrwin.xmax = dx;
   rwinpt->vy.scrwin.ymax = dy;

   if ( XrmGetResource(xresDB,"varkon.rwin.margin_up",
                       "Varkon.rwin.margin_up",type,&value) &&
        sscanf(value.addr,"%d",&margin) == 1 )
     rwinpt->vy.scrwin.ymax -= margin;

   if ( XrmGetResource(xresDB,"varkon.rwin.margin_down",
                       "Varkon.rwin.margin_down",type,&value) &&
        sscanf(value.addr,"%d",&margin) == 1 )
     rwinpt->vy.scrwin.ymin += margin;

   if ( XrmGetResource(xresDB,"varkon.rwin.margin_left",
                       "Varkon.rwin.margin_left",type,&value) &&
        sscanf(value.addr,"%d",&margin) == 1 )
     rwinpt->vy.scrwin.xmin += margin;

   if ( XrmGetResource(xresDB,"varkon.rwin.margin_right",
                       "Varkon.rwin.margin_right",type,&value) &&
        sscanf(value.addr,"%d",&margin) == 1 )
     rwinpt->vy.scrwin.xmax -= margin;
/*
***Modellf�nstret g�r vi lika med det grafiska f�nstret i skala = 1.0.
************************Bort !!!!!!!!!!!
*/
   rwinpt->vy.modwin.xmin = 0;
   rwinpt->vy.modwin.ymin = 0;
   rwinpt->vy.modwin.xmax =
             (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
              DisplayWidthMM(xdisp,xscr) / DisplayWidth(xdisp,xscr);
   rwinpt->vy.modwin.ymax =
             (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin) *
              DisplayHeightMM(xdisp,xscr) / DisplayHeight(xdisp,xscr);

   rwinpt->vy.pdist  = 0.0;
/*
***Default view.
*/
   rwinpt->vy.matrix.k11 = 1.0;
   rwinpt->vy.matrix.k12 = 0.0;
   rwinpt->vy.matrix.k13 = 0.0;
   rwinpt->vy.matrix.k21 = 0.0;
   rwinpt->vy.matrix.k22 = 1.0;
   rwinpt->vy.matrix.k23 = 0.0;
   rwinpt->vy.matrix.k31 = 0.0;
   rwinpt->vy.matrix.k32 = 0.0;
   rwinpt->vy.matrix.k33 = 1.0;
/*
***View status = 3D.
*/
   rwinpt->vy.status = VIEW_3D_ONLY;
/*
***Default values.
*/
   rwinpt->musmod  = 2;
   rwinpt->movx    = rwinpt->movy = 0.0;
   rwinpt->rotx    = rwinpt->roty = rwinpt->rotz = 0.0;
   rwinpt->scale   = 1.0;
   rwinpt->light   = 50.0;
   rwinpt->pfactor = 0.0;
   rwinpt->zclip   = FALSE;
   rwinpt->zfactor = 50.0;
   rwinpt->fill    = TRUE;
/*
***Init levels by copying GWIN_MAIN.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   mainpt = (WPGWIN *)winptr->ptr;
   V3MOME(mainpt->nivtab,rwinpt->nivtab,WP_NTSIZ);
/*
***At this stage there are no sub windows.
*/
   for ( i=0; i<WP_GWSMAX; ++i) rwinpt->wintab[i].ptr = NULL;
/*
***Register the window in the global window table.
*/
   wpwtab[*id].typ = TYP_RWIN;
   wpwtab[*id].ptr = (char *)rwinpt;
/*
***Create the Message and command window.
*/
   WPcreate_mcwin(*id,MCWIN_DY);
/*
***Create toolbar(s).
*/
   create_toolbar(rwinpt);
/*
***Show window.
*/
   WPwshw(*id);
/*
***Wait for MapNotify so we are sure the first frame will be visible.
*/
   XWindowEvent(xdisp,rwinpt->id.x_id,SubstructureNotifyMask,&event);
/*
***Configure OpenGL settings.
*/
   setup_ogl(rwinpt);
/*
***Activate view XY.
*/
   WPactivate_view("xy",NULL,rwinpt,TYP_RWIN);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool          WPbtrw(
        WPRWIN       *rwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button handler for WPRWIN. Tries to serve the
 *      event supplied and returns TRUE if successful.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *          butev   = X button event.
 *
 *      Out: *serv_id = ID for subwindow that served the event.
 *
 *      Return: TRUE  = Event served.
 *              FALSE = Event not served.
 *
 *      (C)microform ab 1998-01-09 J. Kjellander
 *
 *      1998-11-17 Musrotation, J.Kjellander
 *      1998-11-21 actview, G. Liden
 *      2007-07-09 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short         i,acttyp,actnum=0;
    int           butx,buty,ix1,iy1,ix2,iy2,xrk,yrk,xck,yck,mode;
    bool          hit;
    Window        root,child;
    unsigned int  xbuts;
    char         *subptr;
    WPBUTT       *butptr;
    WPICON       *icoptr;
    XEvent        event;


/*
***It could also be a resize of the Message and Command window...
*/
   if ( butev->window == rwinpt->mcw_ptr->resize_xid )
     {
     return(WPbutton_mcwin(rwinpt->mcw_ptr,butev));
     }
/*
***...or a mouse click in the command edit.
*/
   else if ( butev->window == rwinpt->mcw_ptr->cmdptr->id.x_id )
     {
     return(WPbted(rwinpt->mcw_ptr->cmdptr,butev));
     }
/*
***...or a dynamic zoom/rot/pan operation (button 2)
*/
   else if ( butev->window == rwinpt->id.x_id  &&  butev->button == 2 )
     {
     glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
     ix1 = butev->x;
     iy1 = butev->y;
evloop:
     XNextEvent(xdisp,&event);
     if ( event.xany.window == rwinpt->id.x_id )
       {
       switch ( event.type)
         {
/*
***Mouse button pressed. Clear event stack from pending
***events and order a new event. When a new event comes,
***poll the mouse position.
*/
         case ButtonPress:
         ix1 = event.xmotion.x;
         iy1 = event.xmotion.y;
         while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
         XQueryPointer(xdisp,rwinpt->id.x_id,&root,&child,
                      &xrk,&yrk,&xck,&yck,&xbuts);
         goto evloop;
/*
***Mouse button is released, time to exit. Save result for expose.
*/
         case ButtonRelease:
         while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
         XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                                     0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
         return(TRUE);
/*
***Mouse is moving. Clear event stack from pending events and save
***last motion event and do something.
*/
         case MotionNotify:
         ix2 = event.xmotion.x;
         iy2 = event.xmotion.y;
         while ( XPending(xdisp) )
           {
           XNextEvent(xdisp,&event);
           if ( event.type == MotionNotify )
             {
             ix2 = event.xmotion.x;
             iy2 = event.xmotion.y;
             }
           }
/*
***Mouse button 2 = ROT
***<CTRL>  + mouse button 2 = SCALE
***<SHIFT> + mouse button 2 = PAN
*/
        if      ( event.xmotion.state & ControlMask ) mode = SCALE;
        else if ( event.xmotion.state & ShiftMask )   mode = PAN;
        else                                           mode = ROT;

         switch ( mode )
           {
/*
***Scaling.
*/
           case SCALE:
           if ( abs(iy1-iy2) > 1 )
             {
             rwinpt->scale *= 1.0+(double)(iy2-iy1)/rwinpt->geo.dy;
             if ( rwinpt-> scale < 1E-10 ) rwinpt->scale = 1E-10;
             if ( rwinpt-> scale > 1E+10 ) rwinpt->scale = 1E+10;
             ix1 = ix2;
             iy1 = iy2;
             WPsodl_all(rwinpt);
             }
           break;
/*
***Pan.
*/
           case PAN:
           if ( abs(ix1-ix2) > 1  ||  abs(iy1-iy2) > 1 )
             {
             rwinpt->movx += (double)(ix1-ix2)/rwinpt->geo.dx*
                             (rwinpt->xmax - rwinpt->xmin)*
                             rwinpt->scale;
             rwinpt->movy += (double)(iy2-iy1)/rwinpt->geo.dy*
                             (rwinpt->ymax - rwinpt->ymin)*
                             rwinpt->scale;
             ix1 = ix2;
             iy1 = iy2;
             WPsodl_all(rwinpt);
             }
           break;
/*
***Rotation.
*/
           case ROT:
           if ( abs(ix1-ix2) > 1  ||  abs(iy1-iy2) > 1 )
             {
             rwinpt->rotx = (iy2-iy1)/4.0;
             rwinpt->roty = (ix2-ix1)/4.0;
             ix1 = ix2;
             iy1 = iy2;
             WPsodl_all(rwinpt);
             }
           break;
           }
/*
***Wait for new mousemove event.
*/
         while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
         XQueryPointer(xdisp,rwinpt->id.x_id,&root,&child,
                      &xrk,&yrk,&xck,&yck,&xbuts);
         goto evloop;
         }
       }
/*
***The mouse left the window without being released.
*/
     else
       {
       XPutBackEvent(xdisp,&event);
       return(TRUE);
       }
     }
/*
***This is the end of Button 2 handling. Button 1 events
***only occur in child windows.
***Check if a button or icon in a toolbar is involved.
*/
   else if ( butev->button == 1 )
     {
     hit = FALSE;

     for ( i=0; i<WP_GWSMAX; ++i )
       {
       subptr = rwinpt->wintab[i].ptr;
       if ( subptr != NULL )
         {
         switch ( rwinpt->wintab[i].typ )
           {
           case TYP_BUTTON:
           butptr = (WPBUTT *)subptr;
           if ( butev->window == butptr->id.x_id )
             {
             butx = rwinpt->geo.x + butptr->geo.x;
             buty = rwinpt->geo.y + butptr->geo.y;
             hit     = TRUE;
             acttyp  = butptr->acttyp;
             actnum  = butptr->actnum;
            *serv_id = butptr->id.w_id;
             }
           break;

           case TYP_ICON:
           icoptr = (WPICON *)subptr;
           if ( butev->window == icoptr->id.x_id )
             {
             butx = rwinpt->geo.x + icoptr->geo.x;
             buty = rwinpt->geo.y + icoptr->geo.y;
             hit     = TRUE;
             acttyp  = icoptr->acttyp;
             actnum  = icoptr->actnum;
            *serv_id = icoptr->id.w_id;
             }
           break;
           }
/*
***Button hit ?
*/
         if ( hit )
           {
           if ( acttyp == CFUNC )
             {
             switch ( actnum )
               {
/*
***Zoom.
*/
               case 193:
               WPzoom();
               break;
/*
***AutoZOOM.
*/
               case 194:
               WPiaut(NULL,rwinpt,TYP_RWIN);
               break;
/*
***Wireframe mode, f100.
*/
               case 100:
               rwinpt->fill = FALSE;
               WPrepaint_RWIN(rwinpt->id.w_id,FALSE);
               break;
/*
***Shaded mode, f101.
*/
               case 101:
               rwinpt->fill = TRUE;
               WPrepaint_RWIN(rwinpt->id.w_id,FALSE);
               break;
/*
***The view dialogue, f102.
*/
               case 102:
               WPview_dialogue(rwinpt->id.w_id);
               break;
/*
***The grid dialogue, f103.
*/
               case 103:
               WPgrid_dialogue(rwinpt->id.w_id);
               break;
/*
***The levels dialogue, f197.
*/
               case 197:
               WPlevels_dialogue(rwinpt->id.w_id);
               break;
/*
***Some other function.
*/
               default:
               if ( IGdofu(acttyp,actnum) == EXIT ) IGexit(); 
               break;
               }
             }
/*
***Another action code than FUNC.
*/
           else
             {
             if ( IGdofu(acttyp,actnum) == EXIT ) IGexit(); 
             else return(TRUE);
             }
          return(TRUE);
          }
        }
      }
    }
/*
***No hit.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool          WPxprw(
        WPRWIN       *rwinpt,
        XExposeEvent *expev)

/*      Expose-rutin f�r WPRWIN.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1988-01-04 J. Kjellander
 *
 *      2007-06-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    char   *subptr;

/*
***Expose all subwindows.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = rwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( rwinpt->wintab[i].typ )
          {
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
***Then the Message and Command window.
*/
   WPexpose_mcwin(rwinpt->mcw_ptr);
/*
***Copy from 'save under'-pixmappen.
*/
    XCopyArea(xdisp,rwinpt->savmap,rwinpt->id.x_id,rwinpt->win_gc,
                expev->x,expev->y,expev->width,expev->height,
                expev->x,expev->y);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool            WPcrrw(
        WPRWIN         *rwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin f�r WPRWIN med vidh�ngande sub-f�nster.
 *      Kollar om Leave/Enter skett i n�got av WPRWIN-f�nstrets
 *      subf�nster och servar is�fall eventet.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
 *
 *      (C)microform ab 1998-01-09 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    WPBUTT *butptr;
    WPICON *icoptr;

/*
***G� igenom alla sub-f�nster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = rwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( rwinpt->wintab[i].typ )
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

        bool             WPcorw(
        WPRWIN          *rwinpt,
        XConfigureEvent *conev)

/*      Configure handler for WPRWIN.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *          conev  = C-pekare till ConfigureEvent.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE.
 *
 *      (C)microform ab 1998-10-29 J. Kjellander
 *
 *      2006-11-27 Pending events, Johan Kjellander
 *      2007-04-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   int    oldx,oldy,olddx,olddy,newx,newy,newdx,newdy,
          dx,dy,ddx,ddy;
   double mdx,mdy,gadx,gady,mprop,gprop;
   Window child;
   XEvent event;

/*
***Activate this GL window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***To prevent multiple updates of the window during resize
***by the user, remove pending StructureNotify events.
*/
   while ( XCheckMaskEvent(xdisp, StructureNotifyMask, &event)  &&
           event.type == ConfigureNotify)
     V3MOME(&event.xconfigure,conev,sizeof(XConfigureEvent));
/*
***Old position and size.
*/
    oldx  = rwinpt->geo.x;  oldy  = rwinpt->geo.y;
    olddx = rwinpt->geo.dx; olddy = rwinpt->geo.dy;
/*
***What is the new position ?
*/
    XTranslateCoordinates(xdisp,rwinpt->id.x_id,
                     DefaultRootWindow(xdisp),0,0,&newx,&newy,&child);
/*
***What is the new size ?
*/
    newdx = conev->width;
    newdy = conev->height;
/*
***How big is the change in position and size ?
*/
    dx  = newx -  oldx;  dy  = newy -  oldy;
    ddx = newdx - olddx; ddy = newdy - olddy;
/*
***Save new size in WPGWIN.
*/
   rwinpt->geo.dx = conev->width;
   rwinpt->geo.dy = conev->height;
/*
***Update view data.
*/
   rwinpt->vy.scrwin.xmax += ddx;
   rwinpt->vy.scrwin.ymax += ddy;
/*
***The following lines are copied from WPGWIN to
***establish how the WM moves the window after it
***is created.
*/ 
   if ( !rwinpt->reprnt )
     {
     rwinpt->wmandx += dx;
     rwinpt->wmandy += dy;
     rwinpt->reprnt = TRUE;
     }
   else if ( rwinpt->wmandx == 0  &&  rwinpt->wmandy == 0 )
     {
     rwinpt->wmandx += dx;
     rwinpt->wmandy += dy;
     }
/*
***Update the position of the Message and Command window.
*/
      rwinpt->mcw_ptr->geo.y  = rwinpt->geo.dy - rwinpt->mcw_ptr->geo.dy;
      rwinpt->mcw_ptr->geo.dx = rwinpt->geo.dx;
      XMoveResizeWindow(xdisp,rwinpt->mcw_ptr->messcom_xid,rwinpt->mcw_ptr->geo.x,
                                                            rwinpt->mcw_ptr->geo.y,
                                                            rwinpt->mcw_ptr->geo.dx,
                                                            rwinpt->mcw_ptr->geo.dy);
      XMoveResizeWindow(xdisp,rwinpt->mcw_ptr->resize_xid,rwinpt->mcw_ptr->geo.x,
                                                           rwinpt->mcw_ptr->geo.y-5,
                                                           rwinpt->mcw_ptr->geo.dx,
                                                           5);
      rwinpt->mcw_ptr->cmdptr->geo.x = rwinpt->mcw_ptr->geo.dx - rwinpt->mcw_ptr->cmdptr->geo.dx - 5;
      rwinpt->mcw_ptr->cmdptr->geo.y = rwinpt->mcw_ptr->geo.dy - rwinpt->mcw_ptr->cmdptr->geo.dy - 5;
      XMoveResizeWindow(xdisp,rwinpt->mcw_ptr->cmdptr->id.x_id,rwinpt->mcw_ptr->cmdptr->geo.x,
                                                                rwinpt->mcw_ptr->cmdptr->geo.y,
                                                                rwinpt->mcw_ptr->cmdptr->geo.dx,
                                                                rwinpt->mcw_ptr->cmdptr->geo.dy);
/*
***Normalize. See WPnrrw().
*/
   gadx = rwinpt->geo.psiz_x *
         (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin);
   gady = rwinpt->geo.psiz_y *
         (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin);

   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;

   gprop = gady/gadx;
   mprop = mdy/mdx;

   if ( mprop > gprop )
     {
     rwinpt->xmin -= (mdy/gprop - mdx)/2.0;
     rwinpt->xmax += (mdy/gprop - mdx)/2.0;
     }
   else if ( mprop < gprop )
     {
     rwinpt->ymin -= (gprop*mdx - mdy)/2.0;
     rwinpt->ymax += (gprop*mdx - mdy)/2.0;
     }
/*
***Viewport.
*/
    glScissor((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
    glEnable(GL_SCISSOR_TEST);

    glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
               (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
               (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
/*
***Erase everything, even the margins.
*/
    glDisable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
/*
***Repaint.
*/
    WPsodl_all(rwinpt);
    XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                    0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool                 WPcmrw(
        WPRWIN              *rwinpt,
        XClientMessageEvent *clmev)

/*      ClientMessage handler for WPRWIN.
 *
 *      In: rwinpt  = C ptr to WPRWIN.
 *          clmev   = C ptr to X event.
 *
 *      FV: TRUE  = Event served.
 *          FALSE = Not this window.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Only client message we serve is WM_DELETE_WINDOW.
*/
   if ( clmev->message_type  ==  XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]     ==  XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPwdel((DBint)rwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        void WPrepaint_RWIN(
        DBint rwin_id,
        bool  resize)

/*      Updates the contents of one WPRWIN or all. The
 *      current OpenGL list is deleted and a new list is
 *      created from scratch.
 *
 *      In: rwin_id = ID of WPRWIN window or RWIN_ALL.
 *          resize  = True if size of model should be
 *                    calculated. False = Use current size.
 *
 *      (C)2007-01-12, J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPRWIN *rwinpt;

/*
***Erase the window(s). This deletes the current GL list.
*/
   WPerrw(rwin_id);
/*
***Loop through the global window table.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinpt = (WPRWIN *)wpwtab[i].ptr;
/*
***Found a WPRWIN.
*/
         if ( rwin_id == rwinpt->id.w_id  ||  rwin_id == RWIN_ALL )
           {
/*
***What size is the model ?
*/
           if ( resize ) WPmmod(rwinpt);
/*
***Activate the RC of this window.
*/
           glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Create the GL display list for this window.
*/
           WPmodl_all(rwinpt);
/*
***Delete possible highlight list(s).
*/
           WPdodl_highlight(rwinpt);
/*
***Execute the list.
*/
           WPsodl_all(rwinpt);
/*
***Copy window contents to the "save-under" bitmap.
*/
           XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                     0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
           }
         }
       }
     }
 }

/********************************************************/
/*!******************************************************/

        short WPerrw(
        DBint win_id)

/*      Erase one or more WPRWIN.
 *
 *      In: win_id = WPRWIN ID or GWIN_ALL
 *
 *      Error: WP1362 = Window %s does not exist.
 *
 *      (C)2007-01-12, J. Kjellander
 *
 ******************************************************!*/

  {
   bool    hit;
   int     i;
/*   char    errbuf[81];*/
   WPRWIN *rwinpt;

/*
***Initiering.
*/
   hit = FALSE;
/*
***Loop through all windows.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinpt = (WPRWIN *)wpwtab[i].ptr;
/*
***Found a WPRWIN.
*/
         if ( win_id == rwinpt->id.w_id  ||  win_id == RWIN_ALL )
           {
           hit = TRUE;
           glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Clear the window.
*/
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
           if ( rwinpt->double_buffer ) glXSwapBuffers(xdisp,rwinpt->id.x_id);
           else                         glFlush();
/*
***And the save_under pixmap.
*/
           XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                     0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
/*
***Delete the list.
*/
           if ( glIsList((GLuint)1) ) glDeleteLists((GLuint)1,(GLsizei)1);
           }
         }
       }
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdlrw(
        WPRWIN *rwinpt)

/*      Delete a WPRWIN.
 *
 *      In: rwinptr = C ptr to a WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Delete this OpenGL rendering context.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
   glDeleteLists((GLuint)1,(GLsizei)1);
   glXMakeCurrent(xdisp,None,NULL);
   glXDestroyContext(xdisp,rwinpt->rc);
/*
***Kill subwindows.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = rwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( rwinpt->wintab[i].typ )
          {
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
***Kill the Message and Command window.
*/
   WPdelete_mcwin(rwinpt->mcw_ptr);
/*
***Free the "Save under"-pixmap.
*/
    XFreePixmap(xdisp,rwinpt->savmap);
/*
***Gree the X GC.
*/
    XFreeGC(xdisp,rwinpt->win_gc);
/*
***Free Varkon memory.
*/
    v3free((char *)rwinpt,"WPdlrw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPRWIN  *WPgrwp(
        Window   x_id)

/*      Returns a C-ptr to the WPRWIN by X id.
 *
 *      In: x_id  = Window X-id.
 *
 *      Return: C-ptr or NULL if window does not exist.
 *
 *      (C)2007-02-13 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPRWIN  *rwinpt;

/*
***Look up the window in wpwtab[].
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
        if ( wpwtab[i].typ == TYP_RWIN )
          {
          rwinpt = (WPRWIN *)wpwtab[i].ptr;
          if ( rwinpt->id.x_id == x_id ) return(rwinpt);
          }
        }
      }
/*
***No hit.
*/
    return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static void    scale_on(
        WPRWIN *rwinpt)

/*      Turns "Scale" on for a WPRWIN, f7.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Scale" mode.
*/
   rwinpt->musmod = 0;
  }

/********************************************************/
/*!******************************************************/

 static void    pan_on(
        WPRWIN *rwinpt)

/*      Turns "Pan" on for a WPRWIN, f91.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Pan" mode.
*/
   rwinpt->musmod = 1;
  }

/********************************************************/
/*!******************************************************/

 static void    rot_on(
        WPRWIN *rwinpt)

/*      Turns "Rot" on for a WPRWIN, f93.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Rot" mode.
*/
   rwinpt->musmod = 2;
  }

/********************************************************/
/*!******************************************************/

 static void    persp_on(
        WPRWIN *rwinpt)

/*      Turns "Persp" on for a WPRWIN, f94.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Persp" mode.
*/
   rwinpt->musmod = 3;
  }

/********************************************************/
/*!******************************************************/

 static void    light_on(
        WPRWIN *rwinpt)

/*      Turns "Light" on for a WPRWIN, f95.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Light" mode.
*/
   rwinpt->musmod = 4;
  }

/********************************************************/
/*!******************************************************/

 static void    clip_on(
        WPRWIN *rwinpt)

/*      Turns "Clip" on for a WPRWIN, f97.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn on "Clip mode".
*/
   glEnable(GL_CLIP_PLANE0);
   rwinpt->zclip = TRUE;
   rwinpt->zfactor = 50.0;
   WPsodl_all(rwinpt);
/*
***Toggle "Clip" mode.
*/
   rwinpt->musmod = 5;
  }

/********************************************************/
/*!******************************************************/

 static void    line_fill(
        WPRWIN *rwinpt)

/*      Toggles "Line/Fill" for a WPRWIN, f100.
 *
 *      In: rwinptr = C ptr to WPRWIN.
 *
 *      (C)2007-01-12 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Turn of "Clip mode".
*/
   if ( rwinpt->zclip )
     {
     glDisable(GL_CLIP_PLANE0);
     rwinpt->zclip = FALSE;
     WPsodl_all(rwinpt);
     }
/*
***Toggle "Line/Fill" mode.
*/
   if ( rwinpt->fill ) rwinpt->fill = FALSE;
   else                rwinpt->fill = TRUE;

   WPrepaint_RWIN(rwinpt->id.w_id,FALSE);
  }

/********************************************************/
/*!******************************************************/

static short setup_ogl(
       WPRWIN *rwinpt)

/*      Set up OpenGL default parameters.
 *
 *      In: rwinpt = C ptr to WPRWIN.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-12-10 F�rgresurser, J.Kjellander
 *
 ******************************************************!*/

  {
   int     ival;
   char    buf[V3STRLEN];
   GLfloat ambient[4],diffuse[4],specular[4];

/*
***Activate the Rendering Contextet.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Activate depth test.
*/
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glClearDepth((GLclampd)1.0);
/*
***Specify a viewport. Don't use parts of the window
***that the user has allocated for margins.
*/
   glScissor((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
             (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
             (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
   glEnable(GL_SCISSOR_TEST);

   glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
/*
***Positive normal direction same as in Varkon.
*/
   glFrontFace(GL_CCW);
/*
***Turn on light.
*/
   glEnable(GL_LIGHTING);
/*
***Create a default lightsource, GL_LIGHT0.
***Ambient.
*/
   if ( WPgrst("varkon.shade.ambient.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) ambient[0] = ival/100.0;
   else ambient[0] = 0.4;

   if ( WPgrst("varkon.shade.ambient.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) ambient[1] = ival/100.0;
   else ambient[1] = 0.4;

   if ( WPgrst("varkon.shade.ambient.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) ambient[2] = ival/100.0;
   else ambient[2] = 0.4;

   ambient[3] = 1.0;

   rwinpt->ambient[0] = ambient[0];
   rwinpt->ambient[1] = ambient[1];
   rwinpt->ambient[2] = ambient[2];

   ambient[0] *= rwinpt->light/100.0;
   ambient[1] *= rwinpt->light/100.0;
   ambient[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
/*
***Diffuse
*/
   if ( WPgrst("varkon.shade.diffuse.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) diffuse[0] = ival/100.0;
   else diffuse[0] = 1.0;

   if ( WPgrst("varkon.shade.diffuse.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) diffuse[1] = ival/100.0;
   else diffuse[1] = 1.0;

   if ( WPgrst("varkon.shade.diffuse.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) diffuse[2] = ival/100.0;
   else diffuse[2] = 1.0;

   diffuse[3] = 1.0;

   rwinpt->diffuse[0] = diffuse[0];
   rwinpt->diffuse[1] = diffuse[1];
   rwinpt->diffuse[2] = diffuse[2];

   diffuse[0] *= rwinpt->light/100.0;
   diffuse[1] *= rwinpt->light/100.0;
   diffuse[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);

/*
***Specular
*/
   if ( WPgrst("varkon.shade.specular.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) specular[0] = ival/100.0;
   else specular[0] = 0.8;

   if ( WPgrst("varkon.shade.specular.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) specular[1] = ival/100.0;
   else specular[1] = 0.8;

   if ( WPgrst("varkon.shade.specular.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) specular[2] = ival/100.0;
   else specular[2] = 0.8;

   specular[3] = 1.0;

   rwinpt->specular[0] = specular[0];
   rwinpt->specular[1] = specular[1];
   rwinpt->specular[2] = specular[2];

   specular[0] *= rwinpt->light/100.0;
   specular[1] *= rwinpt->light/100.0;
   specular[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
/*
***Turn on the default lightsource.
*/
   ambient[0] = ambient[1] = ambient[2] = 1.0; ambient[3] = 1.0;
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
/*
***Clear color and depth buffers.
*/ 
   glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short get_visinfo(
        WPRWIN *rwinpt)

/*      V�ljer X-visual.
 *
 *      In:  rwinpt    = Pekare till blivande WPRWIN.
 *
 *      Ut: *rwinpt = Fyller i vald visinfo.
 *
 *      Felkoder: WP1623 = GLX-saknas.
 *                WP1633 = Ingen visual
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 ******************************************************!*/

  {
   char   tmpbuf[V3STRLEN];
   int    i,dummy,redbits,greenbits,bluebits,zbits;
   int    visatt[50];

   static XVisualInfo *pvisinfo; /* Beh�ver kanske inte vara static */

/*
***Kolla att OpenGL supportas av X-servern.
*/
   if ( !glXQueryExtension(xdisp,&dummy,&dummy) )
     return(erpush("WP1623",""));
/*
***Vilka egenskaper skall visualen ha ?
*/
   i = 0;

   if ( WPgrst("varkon.shade.rgba",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     visatt[i++] = GLX_RGBA;
     rwinpt->rgb_mode = TRUE;

     visatt[i++] = GLX_RED_SIZE;
     if ( !(WPgrst("varkon.shade.redbits",tmpbuf)  &&
         sscanf(tmpbuf,"%d",&redbits) == 1) ) redbits = 1;
     visatt[i++] = redbits;

     visatt[i++] = GLX_GREEN_SIZE;
     if ( !(WPgrst("varkon.shade.greenbits",tmpbuf)  &&
         sscanf(tmpbuf,"%d",&greenbits) == 1) ) greenbits = 1;
     visatt[i++] = greenbits;

     visatt[i++] = GLX_BLUE_SIZE;
     if ( !(WPgrst("varkon.shade.bluebits",tmpbuf)  &&
         sscanf(tmpbuf,"%d",&bluebits) == 1) ) bluebits = 1;
     visatt[i++] = bluebits;
     }
   else rwinpt->rgb_mode = FALSE;

   visatt[i++] = GLX_DEPTH_SIZE;
   if ( !(WPgrst("varkon.shade.zbits",tmpbuf)  &&
       sscanf(tmpbuf,"%d",&zbits) == 1) ) zbits = 16;
   visatt[i++] = zbits;
/*
***Enkel eller dubbelbuffrat ?
*/
   if ( WPgrst("varkon.shade.doublebuffer",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     visatt[i++] = GLX_DOUBLEBUFFER;
     rwinpt->double_buffer = TRUE;
     }
     else rwinpt->double_buffer = FALSE;
/*
***Level 0 is the normal frame buffer used for geometric primitives.
*/
     visatt[i++] = GLX_LEVEL;
     visatt[i++] = 0;
/*
***Request a matching visual.
*/
   visatt[i]   = None;

   pvisinfo = glXChooseVisual(xdisp,xscr,visatt);
/*
***Hur gick det ?
*/
   if ( !pvisinfo )
     {
     if ( rwinpt->rgb_mode ) return(erpush("WP1633","RGB Mode"));
     else                    return(erpush("WP1633","Color Index Mode"));
     }
/*
***Bra !
*/
   rwinpt->pvinfo = pvisinfo;

/*
***Testa att be om en overlay.
*/
   i = 0;
   visatt[i++] = GLX_BUFFER_SIZE;
   visatt[i++] = 4;
   visatt[i++] = GLX_LEVEL;
   visatt[i++] = 1;
   visatt[i]   = None;
   pvisinfo    = glXChooseVisual(xdisp,xscr,visatt);

/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static void    create_toolbar(
        WPRWIN *rwinpt)

/*      Creates the default toolbar(s) from X resource
 *      values as follows:
 *
 *      varkon.rwin.button_n.geometry:     xsizexysize+x+y
 *      varkon.rwin.button_n.text:         Button text
 *      varkon.rwin.button_n.action:       code+num
 *      varkon.rwin.button_n.tooltip:      Tooltip text
 *
 *      or
 *
 *      varkon.rwin.icon_n.geometry:       xsizexysize+x+y
 *      varkon.rwin.icon_n.name:           XPM file name
 *      varkon.rwin.icon_n.action:         code+num
 *      varkon.rwin.icon_n.tooltip:        Tooltip text
 *
 *      In: rwinpt = C ptr to WPRWIN
 *
 *      (C)2007-03-08 J. Kjellander
 *
 ******************************************************!*/

  {
    char         xrmstr1[40],xrmstr2[40],numstr[10],butstr[V3STRLEN+1],
                 actstr[20],iconam[V3PTHLEN+1];
    char        *type[20];
    long         flags;
    XrmValue     value;
    int          i,x,y,nsub;
    unsigned int dx,dy;
    short        status;
    WPBUTT      *buttpt;
    WPICON      *iconpt;

/*
***Number of subwindows is initially = 0.
*/
    nsub = 0;
/*
***Text buttons (WPBUTTON). varkon.rwin.button_n.geometry
***                         varkon.rwin.button_n.text
***                         varkon.rwin.button_n.action
***                         varkon.rwin.button_n.tooltip
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      strcpy(xrmstr1,"varkon.rwin.button_");
      strcpy(xrmstr2,"Varkon.rwin.button_");
      sprintf(numstr,"%d",i);
      strcat(xrmstr1,numstr);
      strcat(xrmstr2,numstr);
      strcat(xrmstr1,".geometry");
      strcat(xrmstr2,".geometry");
/*
***Get the .geometry resource value.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = rwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = rwinpt->geo.dy + y - dy;
/*
***Get the .text resource value. If not available,
***use button number instead.
*/
        strcpy(xrmstr1,"varkon.rwin.button_");
        strcpy(xrmstr2,"Varkon.rwin.button_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".text");
        strcat(xrmstr2,".text");

        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          strcpy(butstr,value.addr);
        else
          strcpy(butstr,numstr);
/*
***Create the button.
*/
        status = WPwcbu(rwinpt->id.x_id,
                       (short)x,(short)y,(short)dx,(short)dy,(short)1,
                        butstr,butstr,"",WP_BGND2,WP_FGND,&buttpt);

        if ( status == 0 )
          {
          rwinpt->wintab[nsub].typ = TYP_BUTTON;
          rwinpt->wintab[nsub].ptr = (char *)buttpt;
/*
***Get the .action resource value. If missing, use "f0".
*/
          strcpy(xrmstr1,"varkon.rwin.button_");
          strcpy(xrmstr2,"Varkon.rwin.button_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".action");
          strcat(xrmstr2,".action");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            strcpy(actstr,value.addr);
          else
            strcpy(actstr,"f0");

          if ( sscanf(&actstr[1],"%hd",&buttpt->actnum) != 1 )
            buttpt->actnum = 0;

          switch ( actstr[0] )
            {
            case 'f': buttpt->acttyp = CFUNC; break;
            case 'm': buttpt->acttyp = MENU;  break;
            case 'p': buttpt->acttyp = PART;  break;
            case 'M': buttpt->acttyp = MFUNC; break;
            case 'r': buttpt->acttyp = RUN;   break;

            default:
            buttpt->acttyp = CFUNC;
            buttpt->actnum = 0;
            break;
            }
/*
***Finally, check for optional tooltip.
*/
          strcpy(xrmstr1,"varkon.rwin.button_");
          strcpy(xrmstr2,"Varkon.rwin.button_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".tooltip");
          strcat(xrmstr2,".tooltip");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            {
            strncpy(buttpt->tt_str,value.addr,80);
            buttpt->tt_str[80] = '\0';
            }
          else
            buttpt->tt_str[0] = '\0';
/*
***Increment nsub.
*/
          if ( ++nsub == WP_RWSMAX ) return;
          }
        }
/*
***Check for an Icon definition with the same number.
*/
      strcpy(xrmstr1,"varkon.rwin.icon_");
      strcpy(xrmstr2,"Varkon.rwin.icon_");
      sprintf(numstr,"%d",i);
      strcat(xrmstr1,numstr);
      strcat(xrmstr2,numstr);
      strcat(xrmstr1,".geometry");
      strcat(xrmstr2,".geometry");
/*
***Get the .geometry resource value.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = rwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = rwinpt->geo.dy + y - dy;
/*
***Get the .filename resource value.
*/
        strcpy(xrmstr1,"varkon.rwin.icon_");
        strcpy(xrmstr2,"Varkon.rwin.icon_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".name");
        strcat(xrmstr2,".name");

        strcpy(iconam,IGgenv(VARKON_ICO));
        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          strcat(iconam,value.addr);
        else
          {
          erpush("WP1723",xrmstr1);
          errmes();
          return;
          }
/*
***Create the icon.
*/
        status = WPwcic(rwinpt->id.x_id,
                       (short)x,(short)y,(short)1,iconam,
                       WP_BGND2,WP_BGND2,&iconpt);

        if ( status == 0 )
          {
          rwinpt->wintab[nsub].typ = TYP_ICON;
          rwinpt->wintab[nsub].ptr = (char *)iconpt;
/*
******Get the .action resource value. If missing, use "f0".
*/
          strcpy(xrmstr1,"varkon.rwin.icon_");
          strcpy(xrmstr2,"Varkon.rwin.icon_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".action");
          strcat(xrmstr2,".action");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            strcpy(actstr,value.addr);
          else
            strcpy(actstr,"f0");

          if ( sscanf(&actstr[1],"%hd",&iconpt->actnum) != 1 )
            iconpt->actnum = 0;

          switch ( actstr[0] )
            {
            case 'f': iconpt->acttyp = CFUNC; break;
            case 'm': iconpt->acttyp = MENU;  break;
            case 'p': iconpt->acttyp = PART;  break;
            case 'M': iconpt->acttyp = MFUNC; break;
            case 'r': iconpt->acttyp = RUN;   break;
    
            default:
            iconpt->acttyp = CFUNC;
            iconpt->actnum = 0;
            break;
            }
/*
***Finally, check for optional tooltip.
*/
          strcpy(xrmstr1,"varkon.rwin.icon_");
          strcpy(xrmstr2,"Varkon.rwin.icon_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".tooltip");
          strcat(xrmstr2,".tooltip");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            {
            strncpy(iconpt->tt_str,value.addr,80);
            iconpt->tt_str[80] = '\0';
            }
          else
            iconpt->tt_str[0] = '\0';
/*
***Increment nsub..
*/
          if ( ++nsub == WP_RWSMAX ) return;
          }
       else errmes();
       }
/*
***Next button or icon.
*/
    }
/*
***The end.
*/
    return;
  }

/********************************************************/
/*!******************************************************/

 static void    init_colors(
        WPRWIN *rwinpt)

/*      Skapar X-f�rger f�r tryckknappar.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 1988-03-27 J. Kjellander
 *
 *      1999-03-24 Bug vid WPgrst,  R. Svedin
 *
 ******************************************************!*/

  {
    char      colnam[80];
    XColor    rgb,hardw;
    Colormap  colmap; 

/*
***Fixa colormap.
*/
    colmap = rwinpt->colmap;
/*
***Skapa de 5 systemf�rgerna.
*/
    if ( !WPgrst("varkon.background",colnam) )
      strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->bgnd1 = hardw.pixel;

    if ( !WPgrst("varkon.buttonColor",colnam) )
      strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->bgnd2 = hardw.pixel;

    if ( !WPgrst("varkon.selectedButtonColor",colnam) )
      strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->bgnd3 = hardw.pixel;

    if ( !WPgrst("varkon.foreground",colnam) )
      strcpy(colnam,"Black");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->fgnd = hardw.pixel;

    if ( !WPgrst("varkon.topShadowColor",colnam) )
      strcpy(colnam,"White");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->tops = hardw.pixel;

    if ( !WPgrst("varkon.bottomShadowColor",colnam) )
      strcpy(colnam,"Black");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->bots = hardw.pixel;

    if ( !WPgrst("varkon.highlightColor",colnam) )
      strcpy(colnam,"Blue");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->noti = hardw.pixel;
/*
***Slut.
*/
    return;
  }

/********************************************************/
