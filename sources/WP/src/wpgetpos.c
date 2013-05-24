/**********************************************************************
*
*    wpgetpos.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPgtmp();   Returns current mouse position
*    WPgmc2();   Returns 2D-model coordinate
*    WPgmc3();   Returns 3D-model coordinate
*    WPgtsc();   Returns screen coordinate in pixels
*    WPgtsw();   Returns rubberband rectangle
*    WPneww();   Interactive creation of new window
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
#include <math.h>
#include <string.h>
#include <GL/glu.h>

extern MNUALT   smbind[];
extern char     jobnam[];
extern DBTmat  *lsyspk;

/*
***Prototypes for internal functions.
*/
static void setgwm(long eventmask);
static void drwbox(Drawable win, GC gc, int ix1, int iy1,
                   int ix2, int iy2, int mode);

/*!******************************************************/

        short WPgtmp(
        int *px,
        int *py)

/*      Returnerar (pollar) aktuell musposition relativt sk�rmens
 *      �vre v�nstra h�rn.
 *
 *      In: px     = Pekare till utdata.
 *          py     = Pekare till utdata.
 *
 *      Ut: *px    = X-koordinat i 2D modellkoordinater.
 *          *py    = Y-koordinat i 2D modellkoordinater.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 ******************************************************!*/

 {
   Window          root,parent,child;
   int             xrk,yrk,xck,yck;
   unsigned int    xbuts;

/*
***Var �r musen.
*/
   parent = DefaultRootWindow(xdisp);

   XQueryPointer(xdisp,parent,&root,&child,&xrk,&yrk,&xck,&yck,&xbuts);

  *px = xrk;
  *py = yrk;
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPgmc2(
        bool    mark,
        char   *pektkn,
        double *px,
        double *py,
        wpw_id *grw_id)

/*      Returns pointer character and 2D model coordinates
 *      in the BASIC coordinate system. Z is always = 0.
 *      3D views or rotated coordinate systems are not
 *      allowed.
 *
 *      In: mark   => Pointer marker TRUE/FALSE
 *
 *      Out: *pektkn => Pointer character.
 *           *px     => X coordinate.
 *           *py     => Y coordinate.
 *           *grw_id => ID of graphical window used.
 *
 *      Return: SMBCHAR = Normal exit with ptr char + coordinates
 *              SMBPOSM = Pos-button pressed
 *
 *      (C)microform ab 8/1-95 J. Kjellander
 *
 *      2007-03-06 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   short    ix,iy;
   DBint    win_id;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;
   GLint    viewport[4];
   GLdouble projMatrix[16];
   GLdouble modelMatrix[16];
   GLdouble winx,winy,winz,objx,objy,objz;

/*
***Get screen (viewport) coordinates. Return window ID.
*/
   if ( WPgtsc(mark,pektkn,&ix,&iy,&win_id) == SMBPOSM ) return(SMBPOSM);

  *grw_id = win_id;
/*
***What kind of graphical window ?
*/
   winptr = WPwgwp((wpw_id)win_id);
/*
***WPGWIN.
*/
   if ( winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;

    *px = gwinpt->vy.modwin.xmin + (ix - gwinpt->vy.scrwin.xmin)*
                     (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                     (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
    *py = gwinpt->vy.modwin.ymin + (iy - gwinpt->vy.scrwin.ymin)*
                     (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                     (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     }
/*
***WPRWIN. This only works for 2D views without rotation. See
***WPgmc3() for 3D code.
*/
   else if ( winptr->typ == TYP_RWIN )
     {
     glGetIntegerv(GL_VIEWPORT, viewport);
     glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
     glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

     winx = ix;
     winy = iy;
     winz = 0.0;
     gluUnProject(winx,winy,winz,modelMatrix,projMatrix,viewport,&objx,&objy,&objz);
    *px = objx;
    *py = objy;
     }
/*
***The end.
*/
   return(SMBCHAR);
 }

/********************************************************/
/*!******************************************************/

        short     WPgmc3(
        bool      mark,
        char     *pektkn,
        DBVector *pout,
        wpw_id   *grw_id)

/*      Returns pointer character and 3D model coordinates
 *      on the XY-plane of the active coordinate system,
 *      ie. Z = 0 in local coordinates.
 *
 *      In: mark   => Pointer marker TRUE/FALSE
 *
 *      Out: *pektkn => Pointer character.
 *           *pout   => 3D position in BASIC coordinates.
 *           *grw_id => ID of WPGWIN/WPRWIN.
 *
 *      Return: SMBCHAR = Normal exit with ptr char + coordinates
 *              SMBPOSM = Pos-button pressed
 *
 *      (C)microform ab 1998-03-31 J. Kjellander
 *
 *      2007-04-03 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   short    ix,iy;
   DBint    win_id;
   DBfloat  tt;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;
   WPRWIN  *rwinpt;
   DBVector p,t;
   DBTmat   vm;
   VYTRF   *vymatpt;
   GLint    viewport[4];
   GLdouble projmat[16];
   GLdouble modmat[16];
   GLdouble winx,winy,winz,objx,objy,objz;

/*
***Get screen (viewport) coordinates. Return window ID.
*/
   if ( WPgtsc(mark,pektkn,&ix,&iy,&win_id) == SMBPOSM ) return(SMBPOSM);

  *grw_id = win_id;
/*
***What kind of window ?
*/
   winptr = WPwgwp((wpw_id)win_id);
/*
***WPGWIN, transform to 2D model coordinate.
*/
   if ( winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;

     p.x_gm = gwinpt->vy.modwin.xmin + (ix - gwinpt->vy.scrwin.xmin)*
                     (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                     (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     p.y_gm = gwinpt->vy.modwin.ymin + (iy - gwinpt->vy.scrwin.ymin)*
                     (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                     (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     p.z_gm = 0.0;

     vymatpt = &gwinpt->vy.matrix;
     }
/*
***WPRWIN, transform to 2D model coordinate. Remove rotation from
***the MODELVIEW matrix so we can use the same code as for WPGWIN
***below. Leave translation though. Scaling is in the PROJECTION
***matrix.
*/
   else if ( winptr->typ == TYP_RWIN )
     {
     rwinpt = (WPRWIN *)winptr->ptr;

     glGetIntegerv(GL_VIEWPORT, viewport);
     glGetDoublev(GL_PROJECTION_MATRIX,projmat);
     glGetDoublev(GL_MODELVIEW_MATRIX,modmat);
     
     modmat[ 0] = 1.0;
     modmat[ 1] = 0.0;
     modmat[ 2] = 0.0;
     modmat[ 3] = 0.0;

     modmat[ 4] = 0.0;
     modmat[ 5] = 1.0;
     modmat[ 6] = 0.0;
     modmat[ 7] = 0.0;

     modmat[ 8] = 0.0;
     modmat[ 9] = 0.0;
     modmat[10] = 1.0;
     modmat[11] = 0.0;

     winx = ix;
     winy = iy;
     winz = 0.0;
     gluUnProject(winx,winy,winz,modmat,projmat,viewport,&objx,&objy,&objz);
     p.x_gm = objx;
     p.y_gm = objy;
     p.z_gm = 0.0;

     vymatpt = &rwinpt->vy.matrix;
     }
/*
***Make 4X4 transformation matrix from 3X3 window view matrix.
*/
   vm.g11 = vymatpt->k11; vm.g12 = vymatpt->k12;
   vm.g13 = vymatpt->k13; vm.g14 = 0.0;
   vm.g21 = vymatpt->k21; vm.g22 = vymatpt->k22;
   vm.g23 = vymatpt->k23; vm.g24 = 0.0;
   vm.g31 = vymatpt->k31; vm.g32 = vymatpt->k32;
   vm.g33 = vymatpt->k33; vm.g34 = 0.0;
   vm.g41 = vm.g42 = vm.g43 = 0.0; vm.g44 = 1.0;
/*
***Transform p to 3D model coordinate in BASIC.
*/
   GEtfpos_to_basic(&p,&vm,&p);
/*
***Shooting direction, ie. view Z-axis in BASIC.
*/
   t.x_gm = vymatpt->k31;
   t.y_gm = vymatpt->k32;
   t.z_gm = vymatpt->k33;
/*
***Transforme p and t to LOCAL.
*/
   if ( lsyspk != NULL )
     {
     GEtfpos_to_local(&p,lsyspk,&p);
     GEtfvec_to_local(&t,lsyspk,&t);
     }
/*
***Local Z-component must be <> 0 or the XY-plane
***is parallell with the shooting direction.
*/
   if ( ABS(t.z_gm) < 1E-10 ) return(-1);
/*
***Calculate the intersect between the shooting line and the XY-plane.
*/
   tt = -p.z_gm/t.z_gm;

   pout->x_gm = p.x_gm + tt*t.x_gm;
   pout->y_gm = p.y_gm + tt*t.y_gm;
   pout->z_gm = 0.0;
/*
***The end.
*/
   return(SMBCHAR);
 }

/********************************************************/
/*!******************************************************/

        short   WPgtsc(
        bool    mark,
        char   *pektkn,
        short  *pix,
        short  *piy,
        wpw_id *win_id)

/*      Returns window-ID, pointer character and 2D pixel 
 *      coordinate relative to the lower left of the window.
 *      Pointing can be done with a mouse button or by pressing
 *      a key on the keyboard.
 *
 *      In:  mark    = Pointer mark TRUE/FALSE.
 *
 *      Out: *pektkn = Pointer character.
 *           *pix    = X coordinate.
 *           *piy    = Y coordinate.
 *           *win_id = ID of window used for pointing.
 *
 *      Return: SMBCHAR = Character in *pektkn + coordinates and win_id
 *              SMBPOSM = Pos button pressed, no coordinates or window
 *
 *      (C)microform ab 13/12/94 J. Kjellander
 *
 *       8/1-95     Multif�nster, J. Kjellander
 *       2006-12-13 ButtonRelease, J.Kjellander
 *       2007-03-06 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
    Window       root,parent,child;
    XEvent       xev;           
    int          xmax = 64;
    char         xkeybuf[65];
    int          xrk,yrk,xck,yck,win_dy;
    unsigned int xbuts; 
    short        n,status,retsym;
    WPGWIN      *gwinpt;
    WPRWIN      *rwinpt;
    MNUALT      *altptr;

/*
***Initializations.
*/
   retsym = SMBCHAR;
/*
***X11, byt utseende p� cursorn och l�t anv�ndaren
***peka med mus eller tangent. En egen event-loop h�r
***f�rhindrar tex. notify p� icke aktiva pek-rutor.
*/
   WPscur(GWIN_ALL,TRUE,xgcur1);
start:
   while ( 1 )
     {
     XMaskEvent(xdisp,ButtonPressMask | ButtonReleaseMask |
                      KeyPressMask    | ExposureMask,&xev);
/*
***Something happend !
*/
     switch (xev.type)
       {
/*
***Button press in WPGWIN or WPRWIN ? Theoretically the user
***might try to click in the menu winow, an input window or
***some other window.
*/
       case ButtonPress:
       switch(xev.xbutton.button)
         {
         case 1: *pektkn = ' ';  break; /* Left  = OK */
         case 2: *pektkn = '\t'; break; /* Mid   = GOMAIN */
         case 3: *pektkn = '\n'; break; /* Right = REJECT */
         }
       xrk = xev.xkey.x;
       yrk = xev.xkey.y;
/*
***Mouse down in WPGWIN.
*/
       if ( (gwinpt=WPggwp(xev.xbutton.window)) != NULL )
         {
        *pix    = (short)(xev.xkey.x);
        *piy    = (short)(gwinpt->geo.dy - xev.xkey.y);
        *win_id = gwinpt->id.w_id;
         }
/*
***Mouse down in WPRWIN.
*/
       else if ( (rwinpt=WPgrwp(xev.xbutton.window)) != NULL )
         {
        *pix    = (short)(xev.xkey.x);
        *piy    = (short)(rwinpt->geo.dy - xev.xkey.y);
        *win_id = rwinpt->id.w_id;
         }
       break;
/*
***Button release must happen in WPGWIN or WPRWIN or Menu.
***If Menu it may have been a pos-button.
*/
       case ButtonRelease:
       if ( (gwinpt=WPggwp(xev.xbutton.window)) == NULL  &&
            (rwinpt=WPgrwp(xev.xbutton.window)) == NULL )
         {
         if ( WPmenu_button(&xev,&altptr) && altptr == NULL )
           {
          *pektkn = ' ';
           retsym = SMBPOSM;
           goto exit;
           }
         else
           {
           WPbell();
           goto start;
           }
         }
       else
         {
         if ( gwinpt != NULL ) *win_id = gwinpt->id.w_id;
         else                  *win_id = rwinpt->id.w_id;
         goto exit;
         }
/*
***Keyboard used. F�r att XQyeryPointer skall kunna
***anropas med r�tt f�nster som indata m�ste vi ta reda p� det
***f�rst. Eftersom vi inte vet vilket av flera m�jliga grafiska
***f�nster det r�r sig om anv�nder vi f�rst XQueryPointer() f�r
***att ta reda p� Root-f�nstrets child. Om detta inte �r ett
***WPGWIN eller WPRWIN provar vi om detta har n�n child som �r
***det osv. tills vi hittar v�rt f�nster.
*/
       case KeyPress:
       parent = DefaultRootWindow(xdisp);

       do
         {
         XQueryPointer(xdisp,parent,&root,&child,&xrk,&yrk,&xck,&yck,&xbuts);
         if ( (gwinpt=WPggwp(child)) != NULL )
           {
          *win_id = gwinpt->id.w_id;
           win_dy = gwinpt->geo.dy;
           break;
           }
         else if ( (rwinpt=WPgrwp(child)) != NULL )
           {
          *win_id = rwinpt->id.w_id;
           win_dy = rwinpt->geo.dy;
           break;
           }
         parent = child;
         } while ( child != 0 );

       if ( child == 0 ) break;
       else              XQueryPointer(xdisp,child,&root,&child,
                                       &xrk,&yrk,&xck,&yck,&xbuts);

       n = WPlups((XKeyEvent *)&xev,xkeybuf,xmax);
       xrk    = xck;
       yrk    = yck;
/*
***Ett tecken blir det vid normal h�rkorspekning fr�n tangentbordet.
*/
       if ( n == 1 )
         {
        *pektkn = xkeybuf[0];           
         if ( *pektkn == '\015' ) *pektkn = '\n';
        *pix    = (short)xck;
        *piy    = (short)(win_dy - yck);
         goto exit;
         }
/*
***Noll tecken blir det vid funktionstangent eller tex. SHIFT.
***Tv� tecken eller fler kan vara en piltangent etc.
***Om snabbval avbryts med TAB dvs. igdofu() returnerar SMBMAIN
***ser vi till att detta propageras bak�t genom att avsluta
***�ven h�r med TAB dvs. *smbind�7�.str.
*/
       else
         {
         status = WPkepf((XKeyEvent *)&xev);
         if ( status != SMBNONE )
           {
           WPscur(GWIN_ALL,TRUE,xgcur1);
           if ( status == SMBMAIN )
             {
            *pektkn = *smbind[7].str;
            *pix    = (short)(xck);
            *piy    = (short)(win_dy - yck);
             goto exit;
             }
           }
         }
       break;
/*
***Handle expose-events here.
*/
       case Expose:
       WPwexp((XExposeEvent *)&xev);
       break;
       } 
     }        *win_id = rwinpt->id.w_id;
/*
***The end.
*/
exit:
   WPscur(GWIN_ALL,FALSE,(Cursor)0);
/*
***Optionally, create pointer mark.
*/
   if ( mark ) WPcpmk((wpw_id)*win_id,xrk,yrk);

   return(retsym);
 }

/********************************************************/
/*!******************************************************/

        short    WPgtsw(
        wpw_id  *grw_id,
        int     *pix1,
        int     *piy1,
        int     *pix2,
        int     *piy2,
        int      mode,
        bool     prompt)

/*      Interactive input of 2 positions with XOR-outline.
 *
 *      In:  mode  = WP_RUB_RECT = Rectangle outline
 *                    WP_RUB_LINE = Line outline
 *                    WP_RUB_ARROW = Arrow outline
 *                    WP_RUB_NONE = No outline outline
 *           prompt = Autoprompt yes/no.
 *
 *      Out: *grw_id  = ID of WPGWIN or WPRWIN used
 *           *pix1    = Upper left X window coordinate
 *           *piy1    = Upper left Y window coordiante
 *           *pix2    = Lower right X window coordinate
 *           *piy2    = Lower right Y window coordinate
 *
 *      Return:  0 = OK.
 *              -1 = pos1=pos2
 *          REJECT = Operation rejected
 *          GOMAIN = Back to main menu.
 *
 *      (C)microform ab 12/12/94 J. Kjellander
 *
 *      23/2/95 PointerMotionHintMask, J. Kjellander
 *      1/3/95  drwbox(), J. Kjellander
 *      1998-03-19 Returnera f�nster, J.Kjellander
 *      1998-03-24 mode, J.Kjellander
 *      2007-07-31 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      x1=0,y1=0,lastx2=0,lasty2=0;
    int      curx2,cury2,minh,minw,xmin,ymin,xmax,ymax,win_dy;
    Window   root,child,xwin_id;
    GC       rub_gc;
    int      root_x,root_y,win_x,win_y,wintype;
    unsigned int mask;
    XEvent   xev;
    WPGWIN  *gwinpt;
    WPRWIN  *rwinpt;

/*
***Here we need a special eventmask and cursor. We don't
***know yet which window the user will use so we set it on
***all graphical windows (WPGWIN's and WPRWIN's).
*/
   if ( prompt ) WPaddmess_mcwin(IGgtts(322),WP_PROMPT);
   setgwm(GWEM_RUB);
   WPscur(GWIN_ALL,TRUE,xgcur1);
/*
***Ensure no old events pending. First event here
***should be Button Press.
*/
   while ( XPending(xdisp) ) XNextEvent(xdisp,&xev);
/*
***Eventloop.
*/
   while ( 1 )
     {
     XMaskEvent(xdisp,ButtonPressMask |
                      ButtonReleaseMask |
                      PointerMotionMask,&xev);

     switch (xev.type)
       {
/*
***This is the first time that the mouse button is pressed.
***Check which window type it is and get it's ID, geometry and rubberband GC.
*/
       case ButtonPress:
       WPscur(GWIN_ALL,TRUE,xgcur2);

       gwinpt = WPggwp(xev.xany.window);
       if ( gwinpt == NULL )
         {
         rwinpt = WPgrwp(xev.xany.window);
         if ( rwinpt == NULL )
           {
           status = REJECT;
           goto exit;
           }
/*
***The window is a WPRWIN.
*/
         else
           {
           wintype = TYP_RWIN;
           xwin_id = rwinpt->id.x_id;
          *grw_id  = rwinpt->id.w_id;
           rub_gc  = rwinpt->rub_gc;
           xmin    = rwinpt->vy.scrwin.xmin;
           xmax    = rwinpt->vy.scrwin.xmax;
           ymin    = rwinpt->geo.dy - rwinpt->vy.scrwin.ymin;
           ymax    = rwinpt->geo.dy - rwinpt->vy.scrwin.ymax;
           win_dy  = rwinpt->geo.dy;
           }
         }
/*
***The window is a WPGWIN.
*/
       else
         {
         wintype = TYP_GWIN;
         xwin_id = gwinpt->id.x_id;
        *grw_id  = gwinpt->id.w_id;
         rub_gc  = gwinpt->rub_gc;
         xmin    = gwinpt->vy.scrwin.xmin;
         xmax    = gwinpt->vy.scrwin.xmax;
         ymin    = gwinpt->geo.dy - gwinpt->vy.scrwin.ymin;
         ymax    = gwinpt->geo.dy - gwinpt->vy.scrwin.ymax;
         win_dy  = gwinpt->geo.dy;
         }
/*
***Button 2 and 3 means exit.
*/
       if ( xev.xbutton.button == 2 )
         {
         status = GOMAIN;
         goto exit;
         }
       if ( xev.xbutton.button == 3 )
         {
         status = REJECT;
         goto exit;
         }

       x1 = curx2 = lastx2 = xev.xkey.x;
       y1 = cury2 = lasty2 = xev.xkey.y;
       if ( prompt) WPaddmess_mcwin(IGgtts(323),WP_PROMPT);
/*
***Arrow needs special initialization.
*/
       if ( mode == WP_RUB_ARROW )
         {
         XDrawArc(xdisp,xwin_id,rub_gc,
                                 x1-10,y1-10,20,20,0,360*64);
         drwbox(xwin_id,rub_gc,x1,y1,x1,y1,mode);
         }
       break;
/*
***The mouse is moving.
*/
       case MotionNotify:
/*
***The mouse is not allowed to move outside the window.
*/
       if ( xev.xmotion.x < xmin ) xev.xmotion.x = xmin;
       if ( xev.xmotion.x > xmax ) xev.xmotion.x = xmax;
       if ( xev.xmotion.y < ymax ) xev.xmotion.y = ymax;
       if ( xev.xmotion.y > ymin ) xev.xmotion.y = ymin;
/*
***We are still inside the window, redraw the outline.
*/
       curx2 = xev.xmotion.x;
       cury2 = xev.xmotion.y;

       if ( curx2 != lastx2  || cury2 != lasty2 )
         {
         drwbox(xwin_id,rub_gc,x1,y1,lastx2,lasty2,mode);
         if ( curx2 != x1  || cury2 != y1 )
           drwbox(xwin_id,rub_gc,x1,y1,curx2,cury2,mode);
         lastx2 = curx2;
         lasty2 = cury2;
         }
/*
***To get a new motion event we need to call XQueryPointer().
*/
       XQueryPointer(xdisp,xwin_id,&root,&child,&root_x,&root_y,
                     &win_x,&win_y,&mask);
       break;
/*
***The button is released !
*/
       case ButtonRelease:
       if ( xev.xkey.x < xmin ) xev.xkey.x = xmin;
       if ( xev.xkey.x > xmax ) xev.xkey.x = xmax;
       if ( xev.xkey.y < ymax ) xev.xkey.y = ymax;
       if ( xev.xkey.y > ymin ) xev.xkey.y = ymin;

       if ( lastx2 != x1  || lasty2 != y1 )
         drwbox(xwin_id,rub_gc,x1,y1,lastx2,lasty2,mode);

       curx2 = xev.xkey.x;
       cury2 = xev.xkey.y;
/*
***Arrow needs special final.
*/
       if ( mode == WP_RUB_ARROW )
         XDrawArc(xdisp,xwin_id,rub_gc,
                                 x1-10,y1-10,20,20,0,360*64);
/*
***Reverse the Y axis from X11 to Varkon.
*/
       y1    = win_dy - y1;
       cury2 = win_dy - cury2;
/*
***Save coordinates.
*/
        *pix1 = x1;  *pix2 = curx2;
        *piy1 = y1;  *piy2 = cury2;
/*
***Smallest window allowed = 1% of the screen.
*/
       minh = (int)(0.01*(double)DisplayHeight(xdisp,xscr));
       minw = (int)(0.01*(double)DisplayWidth(xdisp,xscr));

       if ( abs(*pix2 - *pix1) < minw  ||
            abs(*piy2 - *piy1) < minh ) status = -1;
       else                             status =  0;
       goto exit;
       }
     }
/*
***Reset eventmask and cursor. Clear message window.
*/
exit:
   if ( wintype == TYP_GWIN ) setgwm(GWEM_NORM);
   else                       setgwm(RWEM_NORM);
   WPscur(GWIN_ALL,FALSE,(Cursor)0);
   WPclear_mcwin();
/*
***The end.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short WPneww()

/*      Interaktiv funktion f�r att skapa nytt WPGWIN.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 1/1/95 J. Kjellander
 *
 *      23/2/95 PointerMotionHintMask, J. Kjellander
 *      1/3/95  drwbox(), J. Kjellander
 *
 ******************************************************!*/

  {
   int                  x1=0,y1=0,curx2,cury2,lastx2=0,lasty2=0,
                        minh,minw,ix1=0,iy1=0,ix2=0,iy2=0,
                        root_x,root_y,win_x,win_y;
   unsigned int         mask;
   XEvent               xev;           
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   Window               xwin_id,root,child;
   short                status,ix,iy,dx,dy;
   DBint                id;
   WPWIN               *winptr;
   WPGWIN              *mainpt;
   GC                   rub_gc;

/*
***Skapa ett (osynligt) InputOnly-f�nster lika stort som hela sk�rmen.
*/
    xwina.override_redirect = True;
    xwinm                   = CWOverrideRedirect;  

    xwin_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),1,1,
                            DisplayWidth(xdisp,xscr),
                            DisplayHeight(xdisp,xscr),0,0,
                            InputOnly,CopyFromParent,xwinm,&xwina);

    XMapRaised(xdisp,xwin_id);
/*
***Modifiera gummibands-GC:et i V3:s huvudf�nster lite.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   mainpt = (WPGWIN *)winptr->ptr;
   rub_gc = mainpt->rub_gc;
   XSetSubwindowMode(xdisp,rub_gc,IncludeInferiors);
/*
***En variant av WPgtsw() f�r rubberband-rektangel.
*/
   WPaddmess_mcwin(IGgtts(322),WP_PROMPT);
   XSelectInput(xdisp,xwin_id,GWEM_RUB);
   XDefineCursor(xdisp,xwin_id,xgcur1);
/*
***N�r vi b�rjar har musknappen �nnu inte tryckts ned.
*/
   status = 0;

   while ( 1 )
     {
     XMaskEvent(xdisp,ButtonPressMask |
                      ButtonReleaseMask |
                      PointerMotionMask,&xev);

     switch (xev.type)
       {
/*
***Detta �r f�rsta g�ngen som musknappen trycks ned.
***Kolla att det �r i r�tt f�nster.
*/
       case ButtonPress:
       XDefineCursor(xdisp,xwin_id,xgcur2);

       if ( xev.xany.window != xwin_id )
         {
         status = REJECT;
         goto exit;
         }
/*
***Knapp 2 och 3 betyder avbrott.
*/
       if ( xev.xbutton.button == 2 )
         {
         status = GOMAIN;
         goto exit;
         }
       if ( xev.xbutton.button == 3 )
         {
         status = REJECT;
         goto exit;
         }

       x1 = curx2 = lastx2 = xev.xkey.x;
       y1 = cury2 = lasty2 = xev.xkey.y;
       WPaddmess_mcwin(IGgtts(323),WP_PROMPT);
       break;
/*
***Nu flyttar sig musen.
*/
       case MotionNotify:
       curx2 = xev.xmotion.x;
       cury2 = xev.xmotion.y;
       if ( curx2 != lastx2  || cury2 != lasty2 )
         {
         drwbox(DefaultRootWindow(xdisp),rub_gc,
                             x1,y1,lastx2,lasty2,WP_RUB_RECT);

         if ( curx2 != x1  || cury2 != y1 )
           drwbox(DefaultRootWindow(xdisp),rub_gc,
                             x1,y1,curx2,cury2,WP_RUB_RECT);
         lastx2 = curx2;
         lasty2 = cury2;
         }
/*
***F�r att f� ett nytt Motion-event m�ste vi anropa XQueryPointer().
*/
       XQueryPointer(xdisp,DefaultRootWindow(xdisp),&root,&child,
                                    &root_x,&root_y,&win_x,&win_y,&mask);
       break;
/*
***N�r knappen sl�pps igen �r det slut.
*/
       case ButtonRelease:
       if ( lastx2 != x1  || lasty2 != y1 )
         drwbox(DefaultRootWindow(xdisp),rub_gc,
                         x1,y1,lastx2,lasty2,WP_RUB_RECT);
       curx2 = xev.xkey.x;
       cury2 = xev.xkey.y;
/*
***Sortera koordinaterna.
*/
       if ( x1 < curx2 ) { ix1 = x1;    ix2 = curx2; }
       else              { ix1 = curx2; ix2 = x1; }
       if ( y1 < cury2 ) { iy1 = y1;    iy2 = cury2; }
       else              { iy1 = cury2; iy2 = y1; }
/*
***Minsta f�nster f�r att betraktas som ok = 1% av sk�rmen.
*/
       minh = (int)(0.01*(double)DisplayHeight(xdisp,xscr));
       minw = (int)(0.01*(double)DisplayWidth(xdisp,xscr));

       if ( abs(ix2 - ix1) < minw  ||
            abs(iy2 - iy1) < minh ) status = REJECT;
       else                             status = 0;
       goto exit;
       }
     }
/*
***Utg�ng. �terst�ll GC och d�da sp�k-f�nstret.
*/
exit:
   WPclear_mcwin();
   XSetSubwindowMode(xdisp,rub_gc,ClipByChildren);
   XDestroyWindow(xdisp,xwin_id);
/*
***Om allt har g�tt bra forts�tter vi annars �r det slut.
*/
   if ( status < 0 ) return(status);
/*
***Skapa nytt WPGWIN. Kompensera lite f�r Motif-ram.
*/
   ix = (short)(ix1 - 6);
   iy = (short)(iy1 - 23);
   dx = (short)(ix2 - ix1);
   dy = (short)(iy2 - iy1);

   if ( WPwcgw(ix,iy,dx,dy,"",FALSE,&id) < 0 )
     {
     errmes();
     return(0);
     }
/*
***Rita.
*/
   WPrepaint_GWIN(id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

  static void     drwbox(
         Drawable win,
         GC       gc,
         int      ix1,
         int      iy1,
         int      ix2,
         int      iy2,
         int      mode)

/*      Draw/erase rubberband rectangle etc.
 *
 *      In:  win    = X11-id for window.
 *           gc     = X11 GC to use.
 *           ix1    = Pos 1 X-coordinate.
 *           iy1    = Pos 1 Y-coordinate.
 *           ix2    = Pos 2 X-coordinate.
 *           iy2    = Pos 2 Y-coordinate.
 *           mode   = 1 => Rectangle
 *                    2 => Line
 *
 *      (C)microform ab 12/12/94 J. Kjellander
 *
 *      1/3-95  Bug, dx/dy < 0, J. Kjellander
 *      1998-03-23 mode, J.Kjellander
 *
 ******************************************************!*/

  {
   int          x1,y1,x2,y2,dx,dy;
   unsigned int abs_dx,abs_dy;
   double       alfa,cosalf,sinalf;

/*
***Vilken sorts figur ?
*/
   switch ( mode )
     {
     case WP_RUB_NONE:
     return;
/*
***Rektangel, hur stor ?
*/
     case WP_RUB_RECT:
     dx = ix2 - ix1;
     dy = iy2 - iy1;
/*
***Om dx eller dy �r negativ m�ste vi fixa lite eftersom
***alla X-servrar inte klarar detta.
*/
     if ( dx < 0 ) x1 = ix2, x2 = ix1;
     else          x1 = ix1, x2 = ix2;

     if ( dy < 0 ) y1 = iy2, y2 = iy1;
     else          y1 = iy1, y2 = iy2;

     abs_dx = abs(dx);
     abs_dy = abs(dy);
     XDrawRectangle(xdisp,win,gc,x1,y1,abs_dx,abs_dy);
     break;
/*
***Linje �r enkelt.
*/
     case WP_RUB_LINE:
     XDrawLine(xdisp,win,gc,ix1,iy1,ix2,iy2);
     break;
/*
***Pil, ber�kna vinkeln mellan gummibandslinjen och positiva
***X-axeln.
*/
     case WP_RUB_ARROW:
     dx = ix2 - ix1;
     dy = iy2 - iy1;

     if ( dx == 0  &&  dy > 0 ) alfa = PI05;
     else if ( dx == 0 ) alfa = -PI05;
     else
       {
       if ( dx > 0 )
         {
         if ( dy >= 0 ) alfa =  ATAN((double)dy/(double)dx);
         else           alfa = -ATAN((double)-dy/(double)dx);
         }
       else
         {
         if ( dy >= 0 ) alfa = PI - ATAN((double)dy/(double)-dx);
         else           alfa = ATAN((double)-dy/(double)-dx) - PI;
         }
       }
/*
***Rita sj�lva pilen.
*/
     XDrawLine(xdisp,win,gc,ix1,iy1,ix2,iy2);
/*
***Pilspetsen
*/
     cosalf = cos(alfa);
     sinalf = sin(alfa);

     dx = (int)(-25.0*cosalf + 10.0*sinalf);
     dy = (int)(-10.0*cosalf - 25.0*sinalf);
     XDrawLine(xdisp,win,gc,ix2,iy2,ix2+dx,iy2+dy);

     dx = (int)(-25.0*cosalf - 10.0*sinalf);
     dy = (int)( 10.0*cosalf - 25.0*sinalf);
     XDrawLine(xdisp,win,gc,ix2,iy2,ix2+dx,iy2+dy);
     break;
     }

   XFlush(xdisp);
  }

/********************************************************/
/*!******************************************************/

  static void setgwm(
         long eventmask)

/*      Sets the eventmask of all graphical windows.
 *
 *      In:  eventmask = GWEM_NORM or GWEM_RUB
 *
 *      (C)2007-07-31 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Find all WPGWIN/WPRWIN and set their eventmask.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
         XSelectInput(xdisp,gwinpt->id.x_id,eventmask);
         }
       else if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinpt = (WPRWIN *)wpwtab[i].ptr;
         XSelectInput(xdisp,rwinpt->id.x_id,eventmask);
         }
       }
     }
  }

/********************************************************/
