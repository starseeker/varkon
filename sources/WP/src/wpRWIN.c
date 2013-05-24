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
*    WPrenw();   WP:s rendering window
*    WPwcrw();   Creates a WPRWIN
*    WPbtrw();   Button routine for WPRWIN
*    WPxprw();   Expose routine for WPRWIN
*    WPcrrw();   Crossing routine for WPRWIN 
*    WPcorw();   Configure routine for WPRWIN 
*    WPcmrw();   Client message for WPRWIN
*    WPdlrw();   Deletes a WPRWIN
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

#ifdef V3_OPENGL
#include "../include/v3icon.h"

#define RWIN_MARGIN 20

extern char         jobnam[];

/*Gunnar_added_1998_11_21*/
extern VY           actvy;    /* Active view */
extern VY           vytab[];  /* View table */

static short setup_ogl(WPRWIN *rwinpt);
static short get_visinfo(WPRWIN *rwinpt);
static void  init_colors(WPRWIN *rwinpt);
static short create_bt(WPRWIN *rwinpt);

/*!******************************************************/

        short WPrenw()

/*      Huvudfunktion för VARKON:s interaktiva
 *      renderings-fönster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 1997-12-21 J. Kjellander
 *
 *      1998-10-30 X-Resurser, J.Kjellander
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
***Hårdprogrammerad storlek och placering.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);

   x  = 0.3*width - 12;
   y  = 50;
   dx = 0.6*width - 15;
   dy = 0.7*height;
/*
***Värden från resursdatabasen.
***Kolla att resultatet hamnar på skärmen.
*/
   if ( XrmGetResource(xresDB,"varkon.renwin.geometry",
                              "Varkon.Renwin.Geometry",
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
***Fönstertitel.
*/
   if ( !WPgrst("varkon.renwin.title",title) )
                  strcpy(title,"VARKON Dynamic Rendering");
/*
***Skapa fönstret.
*/
   status = WPwcrw(x,y,dx,dy,title,&id);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short WPwcrw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        DBint  *id)

/*      Skapar WPRWIN-fönster.
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
 *              WP1612 = Kan ej initiera OpenGL.
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *
 *      1998-03-24 Omarbetad, J.Kjellander
 *      1998-11-21 actview    G. Liden      
 *
 ******************************************************!*/

  {
   short                status;
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   XSizeHints           xhint;
   Window               xwin_id;
   XEvent               event;
   XWMHints             wmhint;
   Pixmap               IconPixmap,SavePixmap;
   int                  i,width,height;
   double               mdx,mdy,mdz;
   unsigned int         depth;
   XGCValues            values;
   DBVector                por,pu1,pu2;
   DBTmat               vymat;
   GC                   Win_gc,Rub_gc;
   WPWIN               *winptr;
   WPRWIN              *rwinpt;
   WPGWIN              *mainpt;

/*
***Allokera minne för ett WPRWIN.
*/
   if ( (rwinpt=(WPRWIN *)v3mall(sizeof(WPRWIN),"WPwcrw")) == NULL )
     return(erpush("WP1062",label));
/*
***Välj visual.
*/
   if ( (status=get_visinfo(rwinpt)) < 0 )
     {
     v3free((char *)rwinpt,"WPwcrw");
     return(erpush("WP1612",""));
     }
/*
***Skapa rendering context.
*/
   if ( (rwinpt->rc=glXCreateContext(xdisp,rwinpt->pvinfo,NULL,TRUE)) == NULL )
     {
     v3free((char *)rwinpt,"WPwcrw");
     erpush("WP1643","");
     return(erpush("WP1612",""));
     }
/*
***Skapa ett ledigt fönster-ID.
*/
   if ( (*id=WPwffi()) < 0 )
     {
     v3free((char *)rwinpt,"WPwcrw");
     return(erpush("WP1052",label));
     }
/*
***Skapa colormap.
*/
   rwinpt->colmap = XCreateColormap(xdisp,
                                    RootWindow(xdisp,rwinpt->pvinfo->screen),
                                    rwinpt->pvinfo->visual,AllocNone);
/*
***Skapa färger för tryckknappar som går att använda
***i den valda visualen. Vi kan inte använda WPgcol()
***eftersom den inte säkert använder den visual som
***OpenGL-fönstret har.
*/
   init_colors(rwinpt);
/*
***Fönsterattribut.
*/
   xwina.colormap          = rwinpt->colmap; 
   xwina.background_pixel  = rwinpt->bgnd;
   xwina.border_pixel      = 0;
   xwina.override_redirect = False;
   xwina.save_under        = False;
/*
***Mask.
*/
   xwinm = ( CWBackPixel        | CWBorderPixel |
             CWOverrideRedirect | CWSaveUnder   | CWColormap );
/*
***Skapa ett fönster med samma visual som OpenGL:s RC.
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
***Skicka hints till fönsterhanteraren.
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
***Fönstertitel och ikon-text.
*/
   WPmaps(label); 
   XStoreName(xdisp,xwin_id,label);   
   XSetIconName(xdisp,xwin_id,jobnam);
/*
***Input.
*/
   XSelectInput(xdisp,xwin_id,RWEM_NORM);
/*
***Skapa ikon.
*/
  IconPixmap = XCreateBitmapFromData(xdisp,xwin_id,(char *)v3icon_bits,
                                      v3icon_width,v3icon_height);
  wmhint.icon_pixmap = IconPixmap;
  wmhint.flags       = IconPixmapHint;
  XSetWMHints(xdisp,xwin_id,&wmhint);
/*
***V3 får ej dödas av en WINDOW-Manager som tex. Motif.
*/
   WPsdpr(xwin_id);
/*
***Skapa ett eget GC för det grafiska fönstret och
***använd GWIN_MAIN:S för gummiband.
*/
   Win_gc = XCreateGC(xdisp,xwin_id,0,&values);
   values.graphics_exposures = False;
   XChangeGC(xdisp,Win_gc,GCGraphicsExposures,&values);

   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   mainpt = (WPGWIN *)winptr->ptr;
   Rub_gc = mainpt->rub_gc;
/*
***Skapa pixmap för 'save under'.
*/
   SavePixmap = XCreatePixmap(xdisp,xwin_id,width,height,depth);
/*
***Nollställ den. Samtidigt sätter vi bak- och för-grund i
***fönstrets GC.
*/
   XSetBackground(xdisp,Win_gc,rwinpt->bgnd);
   XSetForeground(xdisp,Win_gc,rwinpt->bgnd);
   XFillRectangle(xdisp,SavePixmap,Win_gc,0,0,width,height);
   XSetForeground(xdisp,Win_gc,rwinpt->fgnd);
/*
***Fyll i data om WPRWIN.
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
***Default aktiv vy. Motsvarar vy:n "xy" med skala = 1.0.
***och origo i nedre vänstra hörnet.
*/
   rwinpt->vy.vynamn[0] = '\0';
/*
***Skärmfönstrets storlek och marginaler.
*/
   rwinpt->vy.scrwin.xmin = 0;
   rwinpt->vy.scrwin.ymin = 0;
   rwinpt->vy.scrwin.xmax = dx;
   rwinpt->vy.scrwin.ymax = dy - RWIN_MARGIN;
/*
***Modellfönstret gör vi lika med det grafiska fönstret i skala = 1.0.
*/
   rwinpt->vy.modwin.xmin = 0;
   rwinpt->vy.modwin.ymin = 0;
   rwinpt->vy.modwin.xmax =
             (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
              DisplayWidthMM(xdisp,xscr) / DisplayWidth(xdisp,xscr);
   rwinpt->vy.modwin.ymax =
             (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin) *
              DisplayHeightMM(xdisp,xscr) / DisplayHeight(xdisp,xscr);

   rwinpt->vy.vy_3D = TRUE;
   rwinpt->vy.vydist = 0.0;
/*
***Temp fix.
*/
   rwinpt->vy.vymat.k11 = 1.0;
   rwinpt->vy.vymat.k12 = 0.0;
   rwinpt->vy.vymat.k13 = 0.0;
   rwinpt->vy.vymat.k21 = 0.0;
   rwinpt->vy.vymat.k22 = 1.0;
   rwinpt->vy.vymat.k23 = 0.0;
   rwinpt->vy.vymat.k31 = 0.0;
   rwinpt->vy.vymat.k32 = 0.0;
   rwinpt->vy.vymat.k33 = 1.0;
/*
***Vy definierad av matris.
*
   if ( actvy.vytypp == FALSE )
      {
      rwinpt->vy.vymat.k11 = actvy.vymatr.v11;
      rwinpt->vy.vymat.k12 = actvy.vymatr.v21;
      rwinpt->vy.vymat.k13 = actvy.vymatr.v31;
      rwinpt->vy.vymat.k21 = actvy.vymatr.v12;
      rwinpt->vy.vymat.k22 = actvy.vymatr.v22;
      rwinpt->vy.vymat.k23 = actvy.vymatr.v32;
      rwinpt->vy.vymat.k31 = actvy.vymatr.v13;
      rwinpt->vy.vymat.k32 = actvy.vymatr.v23;
      rwinpt->vy.vymat.k33 = actvy.vymatr.v33;
      }
/*
***Vy definierad av betraktelseposition.
*
   else
      {
      pu1.y_gm = 0.0;
      if ( (actvy.vyrikt.x_vy*actvy.vyrikt.x_vy +
            actvy.vyrikt.z_vy*actvy.vyrikt.z_vy) < 0.001 )
        {
        pu1.x_gm = 0.0; pu1.z_gm = 1.0; pu2.x_gm = 1.0;
        pu2.y_gm = 0.0; pu2.z_gm = 0.0;
        }
      else
        {
        pu1.x_gm = actvy.vyrikt.z_vy;
        pu1.z_gm = 0.0;
        if ( actvy.vyrikt.x_vy != 0.0 ) pu1.z_gm = -actvy.vyrikt.x_vy;
        if ( (actvy.vyrikt.y_vy*actvy.vyrikt.y_vy) < 0.001 )
          {
          pu2.x_gm = 0.0; pu2.y_gm = 1.0; pu2.z_gm = 0.0;
          }
        else
          {
          pu2.x_gm = actvy.vyrikt.z_vy*pu1.y_gm +
                     actvy.vyrikt.y_vy*pu1.z_gm;
          pu2.y_gm = actvy.vyrikt.z_vy*pu1.x_gm -
                     actvy.vyrikt.x_vy*pu1.z_gm;
          pu2.z_gm = actvy.vyrikt.x_vy*pu1.y_gm -
                     actvy.vyrikt.y_vy*pu1.x_gm;
          }
      por.x_gm = 0.0; por.y_gm = 0.0; por.z_gm = 0.0;
      GEmktf_3p(&por,&pu1,&pu2,&vymat);

      rwinpt->vy.vymat.k11 = vymat.g11;
      rwinpt->vy.vymat.k12 = vymat.g21;
      rwinpt->vy.vymat.k13 = vymat.g31;
      rwinpt->vy.vymat.k21 = vymat.g12;
      rwinpt->vy.vymat.k22 = vymat.g22;
      rwinpt->vy.vymat.k23 = vymat.g32;
      rwinpt->vy.vymat.k31 = vymat.g13;
      rwinpt->vy.vymat.k32 = vymat.g23;
      rwinpt->vy.vymat.k33 = vymat.g33;
      }
    }
/*
***Denna vy är fullständig och kan användas, valid = TRUE men
***den har inget namn.
*/
   rwinpt->vy.valid = TRUE;
/*
***Lite defaultvärden.
*/
   rwinpt->musmod  = 2;
   rwinpt->movx    = rwinpt->movy = 0.0;
   rwinpt->rotx    = rwinpt->roty = rwinpt->rotz = 0.0;
   rwinpt->scale   = 1.0;
   rwinpt->light   = 50.0;
   rwinpt->pfactor = 0.0;
   rwinpt->zclip   = FALSE;
   rwinpt->zfactor = 50.0;
/*
***Fönstret har ännu inga subfönster.
*/
   for ( i=0; i<WP_RWSMAX; ++i) rwinpt->wintab[i].ptr = NULL;
/*
***Lagra fönstret i fönstertabellen.
*/
   wpwtab[*id].typ = TYP_RWIN;
   wpwtab[*id].ptr = (char *)rwinpt;
/*
***Skapa tryckknappar.
*/
   create_bt(rwinpt);
/*
***Visa resultatet.
*/
   WPwshw(*id);
/*
***Vänta på MapNotify så allt säkert är klart
***innan vi börjar. Utan detta försvinner i bland
***första bilden.
*/
   XWindowEvent(xdisp,rwinpt->id.x_id,SubstructureNotifyMask,&event);
/*
***Ställ in OpenGL-parametrar.
*/
   setup_ogl(rwinpt);
/*
***Hur stor är modellen ?
*/
   WPmmod(rwinpt);
/*
***Sätt upp en ortogonal proj först så att
***NURBS-tesselatorn fixar vettigt stora
***trianglar.
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
   mdz = rwinpt->zmax - rwinpt->zmin;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-mdx/2.0,+mdx/2.0,-mdy/2.0,+mdy/2.0,mdz,mdz+2*mdz);
/*
***Skapa OpenGL displaylist 1. Måste göras
***före glFrustum() !!!
*/
   WPmodl(rwinpt);
/*
***Visa bilden.
*/
   WPsodl(rwinpt,(GLuint)1);
/*
***Kopiera fönstrets innehåll till "save-under"-
***bitmappen.
*/
   XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
             0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool  WPbtrw(
        WPRWIN       *rwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin för WPRWIN med vidhängande sub-fönster.
 *      Kollar om muspekning skett i något av WPRWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: rwinptr = C-pekare till WPRWIN.
 *          butev   = X-but event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 1998-01-09 J. Kjellander
 *
 *      1998-11-17 Musrotation, J.Kjellander
 *      1998-11-21 actview      G. Liden
 *
 ******************************************************!*/

  {
    short         i,acttyp,actnum=0;
    int           ix1,iy1,ix2,iy2,xrk,yrk,xck,yck;
    bool          hit;
    Window        root,child;
    unsigned int  xbuts;
    char         *subptr;
    double        pf,intensity;
    WPBUTT       *butptr=NULL;
    XEvent        event;
    GLfloat       ambient[4];
    GLfloat       diffuse[4];
    GLfloat       specular[4];

   /* GLfloat active_view_matrix[16]; * Current view matrix 15->16 JK */
/*
***Om musknappeventet är riktat till WPRWIN-fönstret
***har en musknapp tryckts ner i fönstret.
*/
   if ( butev->window == rwinpt->id.x_id )
     {
     if ( butev->button == 1 )
       {
       ix1 = butev->x;
       iy1 = butev->y;

evloop:
       XNextEvent(xdisp,&event);
       if ( event.xany.window == rwinpt->id.x_id )
         {
         switch ( event.type)
           {
/*
***Knappen har som sagt tryckts ner. Töm eventstacken på
***vilande händelser och beställ en ny mousemove så vi
***säkert får en som hör hit.
*/
           case ButtonPress:
           ix1 = event.xmotion.x;
           iy1 = event.xmotion.y;
           while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
           XQueryPointer(xdisp,rwinpt->id.x_id,&root,&child,
                        &xrk,&yrk,&xck,&yck,&xbuts);
           goto evloop;
/*
***Knappen släpps. Här avslutar vi. Spar bilden för expose.
*/
           case ButtonRelease:
           while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
           XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                                       0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
           return(TRUE);
/*
***Musen har rört sig. Töm event-stacken på icke mouse-move.
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
***Gör nåt.
*/
           switch ( rwinpt->musmod )
             {
/*
***Skalning.
*/
             case 0:
             if ( abs(iy1-iy2) > 1 )
               {
               rwinpt->scale *= 1.0+(double)(iy2-iy1)/rwinpt->geo.dy;
               if ( rwinpt-> scale < 1E-10 ) rwinpt->scale = 1E-10;
               if ( rwinpt-> scale > 1E+10 ) rwinpt->scale = 1E+10;
               ix1 = ix2;
               iy1 = iy2;
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
/*
***Panorering.
*/
             case 1:
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
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
/*
***Rotation.
*/
             case 2:
             if ( abs(ix1-ix2) > 1  ||  abs(iy1-iy2) > 1 )
               {
               rwinpt->rotx = (iy2-iy1)/4.0;
               rwinpt->roty = (ix2-ix1)/4.0;
               ix1 = ix2;
               iy1 = iy2;
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
/*
***Perspektiv.
*/
             case 3:
             pf = rwinpt->pfactor;
             if ( abs(iy1-iy2) > 1 )
               {
               pf += 150*((double)iy1-(double)iy2)/rwinpt->geo.dy;
               if (      pf > 100 ) pf = 100.0;
               else if ( pf < 0 )   pf = 0.0;
               rwinpt->pfactor = pf;
               ix1 = ix2;
               iy1 = iy2;
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
/*
***Ljusstyrka.
*/
             case 4:
             intensity = rwinpt->light;
             if ( abs(iy1-iy2) > 1 )
               {
               intensity += 100*((double)iy1-(double)iy2)/rwinpt->geo.dy;
               if (      intensity > 100 ) intensity = 100;
               else if ( intensity < 0 )   intensity = 0;
               rwinpt->light = intensity;
               ambient[0] = intensity/100.0 * rwinpt->ambient[0];
               ambient[1] = intensity/100.0 * rwinpt->ambient[1];
               ambient[2] = intensity/100.0 * rwinpt->ambient[2];
               ambient[3] = 1.0;
               glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);

               diffuse[0] = intensity/100.0 * rwinpt->diffuse[0];
               diffuse[1] = intensity/100.0 * rwinpt->diffuse[1];
               diffuse[2] = intensity/100.0 * rwinpt->diffuse[2];
               diffuse[3] = 1.0;
               glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);

               specular[0] = intensity/100.0 * rwinpt->specular[0];
               specular[1] = intensity/100.0 * rwinpt->specular[1];
               specular[2] = intensity/100.0 * rwinpt->specular[2];
               specular[3] = 1.0;
               glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

               ix1 = ix2;
               iy1 = iy2;
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
/*
***Klippning.
*/
             case 5:
             intensity = rwinpt->zfactor;
             if ( abs(iy1-iy2) > 1 )
               {
               intensity += 100*((double)iy1-(double)iy2)/rwinpt->geo.dy;
               if (      intensity > 100 ) intensity = 100;
               else if ( intensity < 0 )   intensity = 0;
               rwinpt->zfactor = intensity;
               ix1 = ix2;
               iy1 = iy2;
               WPsodl(rwinpt,(GLuint)1);
               }
             break;
             }
/*
***Betäll nytt mousemove-event.
*/
           while ( XPending(xdisp) ) XNextEvent(xdisp,&event);
           XQueryPointer(xdisp,rwinpt->id.x_id,&root,&child,
                        &xrk,&yrk,&xck,&yck,&xbuts);
           goto evloop;
           }
         }
/*
***Musen har kommit ut utanför fönstret utan att knappen
***släppts upp igen. Då bryter vi.
*/
       else
         {
         XPutBackEvent(xdisp,&event);
         return(TRUE);
         }
       }
/*
***Rätt fönster men fel musknapp.
*/
     else
       {
       XBell(xdisp,100);
       return(TRUE);
       }
     }
/*
***Musknapptryckning kan också ha skett i nån av knapparna.
*/
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
            hit     = TRUE;
            acttyp  = butptr->acttyp;
            actnum  = butptr->actnum;
           *serv_id = butptr->id.w_id;
            }
          break;
          }
/*
***Träff kanske ?
*/
        if ( hit )
          {
          switch ( actnum )
            {
/*
***Scale, togglas.
*/
            case 0:
            butptr->status = TRUE;
            rwinpt->musmod = 0;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[1].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[2].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[3].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[4].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[5].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            if ( rwinpt->zclip )
              {
              glDisable(GL_CLIP_PLANE0);
              rwinpt->zclip = FALSE;
              WPsodl(rwinpt,(GLuint)1);
              }
            break;
/*
***Pan, togglas.
*/
            case 1:
            butptr->status = TRUE;
            rwinpt->musmod = 1;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[0].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[2].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[3].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[4].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[5].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            if ( rwinpt->zclip )
              {
              glDisable(GL_CLIP_PLANE0);
              rwinpt->zclip = FALSE;
              WPsodl(rwinpt,(GLuint)1);
              }
            break;
/*
***Rot, togglas.
*/
            case 2:
            butptr->status = TRUE;
            rwinpt->musmod = 2;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[0].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[1].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[3].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[4].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[5].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            if ( rwinpt->zclip )
              {
              glDisable(GL_CLIP_PLANE0);
              rwinpt->zclip = FALSE;
              WPsodl(rwinpt,(GLuint)1);
              }
            break;
/*
***Persp, togglas.
*/
            case 3:
            butptr->status = TRUE;
            rwinpt->musmod = 3;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[0].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[1].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[2].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[4].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[5].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            if ( rwinpt->zclip )
              {
              glDisable(GL_CLIP_PLANE0);
              rwinpt->zclip = FALSE;
              WPsodl(rwinpt,(GLuint)1);
              }
            break;
/*
***Light, togglas.
*/
            case 4:
            butptr->status = TRUE;
            rwinpt->musmod = 4;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[0].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[1].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[2].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[3].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[5].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            if ( rwinpt->zclip )
              {
              glDisable(GL_CLIP_PLANE0);
              rwinpt->zclip = FALSE;
              WPsodl(rwinpt,(GLuint)1);
              }
            break;
/*
***Klipp, togglas.
*/
            case 5:
            glEnable(GL_CLIP_PLANE0);
            rwinpt->zclip = TRUE;
            rwinpt->zfactor = 50.0;
            butptr->status = TRUE;
            rwinpt->musmod = 5;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);
            WPsodl(rwinpt,(GLuint)1);

            butptr = (WPBUTT *)rwinpt->wintab[0].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[1].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[2].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[3].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);

            butptr = (WPBUTT *)rwinpt->wintab[4].ptr;
            butptr->status = FALSE;
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);
            break;
/*
***Line/Fill.
*/
            case 6:
            if ( butptr->status == 0 )
              {
              butptr->status = TRUE;
              glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
              }
            else
              {
              butptr->status = FALSE;
              glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
              }
            XClearWindow(xdisp,butptr->id.x_id);
            WPxpbu(butptr);
            WPsodl(rwinpt,(GLuint)1);
            break;
/*
***Stäng fönstret och uppdatera aktiv vy TODO Egen knapp.
*/
            case 7:
            /* TODO Move to WPuact ..                             */
            /* Strange error if glGetFloatv is called in WPuact ??*/
            /* glGetFloatv(GL_MODELVIEW_MATRIX,active_view_matrix); */
            /* WPuact(rwinpt,active_view_matrix); */
            WPwdel((DBint)rwinpt->id.w_id);
/*
{
#include <time.h>
clock_t start,stopp;
start=clock();
               glEnable(GL_POLYGON_SMOOTH);
               glEnable(GL_LINE_SMOOTH);
               glEnable(GL_BLEND);
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
WPsodl(rwinpt,(GLuint)1);
stopp=clock();
printf("Tid=%g\n",(double)(stopp-start)/CLOCKS_PER_SEC);
fflush(stdout);
}
*/

            break;
            }
          return(TRUE);
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPxprw(
        WPRWIN       *rwinpt,
        XExposeEvent *expev)

/*      Expose-rutin för WPRWIN.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1988-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    char   *subptr;

/*
***Först expose på alla sub-fönster.
*/
    for ( i=0; i<WP_RWSMAX; ++i )
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
***Kopiera från 'save under'-pixmappen.
*/
    XCopyArea(xdisp,rwinpt->savmap,rwinpt->id.x_id,rwinpt->win_gc,
                expev->x,expev->y,expev->width,expev->height,
                expev->x,expev->y);
/*
***Slut.
*/

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcrrw(
        WPRWIN         *rwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPRWIN med vidhängande sub-fönster.
 *      Kollar om Leave/Enter skett i något av WPRWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
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
***Gå igenom alla sub-fönster.
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

        bool WPcorw(
        WPRWIN *rwinpt,
        XConfigureEvent *conev)

/*      Configure-rutin för WPRWIN.
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
 *
 ******************************************************!*/

  {
   double mdx,mdy,gadx,gady,mprop,gprop;
   XEvent event;
   
/*
***To prevent multiple updates of the window during resize
***by the user, remove pending StructureNotify events.
*/
   while ( XCheckMaskEvent(xdisp, StructureNotifyMask, &event)  &&
           event.type == ConfigureNotify)
     V3MOME(&event.xconfigure,conev,sizeof(XConfigureEvent));
/*
***Lagra den nya fönstergeometrin i WRGWIN-posten.
*/
   rwinpt->geo.dx = conev->width;
   rwinpt->geo.dy = conev->height;
/*
***Ny vy.
*/
   rwinpt->vy.scrwin.xmin = 0;
   rwinpt->vy.scrwin.ymin = 0;
   rwinpt->vy.scrwin.xmax = conev->width;
   rwinpt->vy.scrwin.ymax = conev->height - RWIN_MARGIN;
/*
***Normalisera. Se WPnrrw(). Här lägger vi inte
***på nåt extra dock.
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
***Sudda hela fönstret så att det inte blir skräp till höger om knapparna.
*/
    glDisable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
/*
***Rita om.
*/
    WPsodl(rwinpt,(GLuint)1);
    XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                    0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcmrw(
        WPRWIN               *rwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen för WPRWIN.
 *
 *      In: rwinpt  = C-pekare till WPRWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om det är WM_DELETE_WINDOW servar vi genom att döda
***fönstret ifråga såvida det inte är huvudfönstret
***förstås. Detta skall dock inte kunna ta emot ett client-
***message = WM_DELETE_WINDOW.
*/
   if ( clmev->message_type  ==  XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]  ==  XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPwdel((DBint)rwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short WPdlrw(
        WPRWIN *rwinpt)

/*      Dödar ett WPRWIN.
 *
 *      In: rwinptr = C-pekare till WPRWIN.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Stäng OpenGL.
*/
   glDeleteLists((GLuint)1,(GLsizei)1);
   glXMakeCurrent(xdisp,None,NULL);
   glXDestroyContext(xdisp,rwinpt->rc);
/*
***Döda eventuella subfönster.
*/
    for ( i=0; i<WP_RWSMAX; ++i )
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
***Lämna tillbaks grafiska fönstrets "Save under"-pixmapp.
*/
    XFreePixmap(xdisp,rwinpt->savmap);
/*
***Lämna tillbaks GC:t.
*/
    XFreeGC(xdisp,rwinpt->win_gc);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)rwinpt,"WPdlrw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short setup_ogl(
       WPRWIN *rwinpt)

/*      Ställer in OpenGL-parametrar för skuggning.
 *
 *      In: rwinpt = Pekare till renderingsfönster.
 *
 *      Ut: Inget.
 *
 *      Felkoder: Inga.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-12-10 Färgresurser, J.Kjellander
 *
 ******************************************************!*/

  {
   int     ival;
   char    buf[V3STRLEN];
   GLfloat ambient[4],diffuse[4],specular[4];

/*
***Aktivera Rendering Contextet.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Aktivera djuptest.
*/
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glClearDepth((GLclampd)1.0);
/*
***Viewport. Den del av det grafiska fönstret som får användas.
***Eventuella marginaler för knappar skall ej skrivas över.
*/
   glScissor((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
             (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
             (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
   glEnable(GL_SCISSOR_TEST);

   glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
/*
***Låt positiv normal definieras av moturs riktning precis
***som i Varkon.
*/
   glFrontFace(GL_CCW);
/*
***Slå på ljus.
*/
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
/*
***Default ljusstyrka.
***Först Ambient.
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

/* Diffuse*/

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

/* Specular */

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
***Sudda färgbuffert och djupbuffert.
*/ 
   glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short get_visinfo(
        WPRWIN *rwinpt)

/*      Väljer X-visual.
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

   static XVisualInfo *pvisinfo; /* Behöver kanske inte vara static */

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
***Prova att välja en matchande visual.
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

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short create_bt(
        WPRWIN *rwinpt)

/*      Skapar renderingsfönstrets snabbvalsknappar.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 1998-01-09 J. Kjellander
 *
 ******************************************************!*/

  {
    char      str1[V3STRLEN],str2[V3STRLEN];
    int       x,y,dx,dy;
    short     status;
    WPBUTT   *buttpt;

/*
***Scale.
*/
   x = 0;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.scale",str1) ) strcpy(str1,"Scale");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[0].typ = TYP_BUTTON;
     rwinpt->wintab[0].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 0;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 0;
     buttpt->status  = FALSE;
     }
/*
***Pan.
*/
   x = 47;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.pan",str1) ) strcpy(str1,"Pan");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[1].typ = TYP_BUTTON;
     rwinpt->wintab[1].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 1;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 1;
     buttpt->status  = FALSE;
     }
/*
***Rot.
*/
   x = 94;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.rotate",str1) ) strcpy(str1,"Rot");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[2].typ = TYP_BUTTON;
     rwinpt->wintab[2].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 2;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 2;
     buttpt->status  = TRUE;
     }
/*
***Perspective.
*/
   x = 141;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.perspective",str1) ) strcpy(str1,"Persp");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[3].typ = TYP_BUTTON;
     rwinpt->wintab[3].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 3;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 3;
     buttpt->status  = FALSE;
     }
/*
***Light.
*/
   x = 188;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.light",str1) ) strcpy(str1,"Light");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[4].typ = TYP_BUTTON;
     rwinpt->wintab[4].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 4;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 4;
     buttpt->status  = FALSE;
     }
/*
***Z-klipp.
*/
   x = 235;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.clip",str1) ) strcpy(str1,"Z-clip");
   strcpy(str2,"*");
   strcat(str2,str1);

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[5].typ = TYP_BUTTON;
     rwinpt->wintab[5].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 5;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 5;
     buttpt->status  = FALSE;
     }
/*
***Polygon/Shade.
*/
   x = 282;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.poly",str1) ) strcpy(str1,"Poly");
   if ( !WPgrst("varkon.renwin.fill",str2) ) strcpy(str2,"Shade");

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str2,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[6].typ = TYP_BUTTON;
     rwinpt->wintab[6].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 6;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 6;
     }
/*
***Close.
*/
   x = 329;
   y = 0;
   dx = 45;
   dy = 20;
   if ( !WPgrst("varkon.renwin.close",str1) ) strcpy(str1,"Close");

   status = WPwcbu(rwinpt->id.x_id,
                  (short)x,(short)y,(short)dx,(short)dy,(short)1,
                   str1,str1,"",WP_BGND,WP_FGND,&buttpt);

   if ( status == 0 )
     {
     rwinpt->wintab[7].typ = TYP_BUTTON;
     rwinpt->wintab[7].ptr = (char *)buttpt;
     buttpt->acttyp = FUNC;
     buttpt->actnum = 7;
     buttpt->id.p_id = rwinpt->id.w_id;
     buttpt->id.w_id = 7;
     }

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static void    init_colors(
        WPRWIN *rwinpt)

/*      Skapar X-färger för tryckknappar.
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
***Skapa de 5 systemfärgerna.
*/
    if ( !WPgrst("varkon.background",colnam) )
      strcpy(colnam,"Gray");
    XLookupColor(xdisp,colmap,colnam,&rgb,&hardw);
    if ( XAllocColor(xdisp,colmap,&hardw) ) rwinpt->bgnd = hardw.pixel;

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

/*!******************************************************/

        short WPuact(
        WPRWIN *rwinpt,
        GLfloat active_view_matrix[]) /* Current view matrix */

/*      Uppdatera aktiv vy med WPRWIN.
 *
 *      In: rwinptr = C-pekare till WPRWIN.
 *          
 *
 *      (C)microform ab 1998-11-21 G. Liden
 *
 ******************************************************!*/

  {
   int  i_view; /* Index to active view in view table */
   double scale; /* Scaling factor for view */
   short status; /* Error code from a called function */
   double mdx, mdy, scale_in; /* For calculation of scale */
   int     i_window;     /* Loop index window */
   WPWIN  *winptr;   
   WPGWIN *gwinpt;       /* Pointer to the active view window */

/*
***Get index in view table
*/

   i_view =vyindx(actvy.vynamn);
   if ( -1 == i_view )
     {
     return(-1);
     }

/*
***View now defined by a matrix
*/

   actvy.vytypp = FALSE;

   vytab[i_view].vytypp = FALSE;

/*
***Get matrix from OpenGL. Modify with the scale
***TODO Verify if necessary to modify actview and vytab[i_view]
*/

   if ( rwinpt->scale > 0.000000000001 ) scale = rwinpt->scale;
   else scale = 1.0;

   actvy.vymatr.v11 = active_view_matrix[ 0]/scale;
   actvy.vymatr.v21 = active_view_matrix[ 4]/scale;
   actvy.vymatr.v31 = active_view_matrix[ 8]/scale;
   actvy.vymatr.v12 = active_view_matrix[ 1]/scale;
   actvy.vymatr.v22 = active_view_matrix[ 5]/scale;
   actvy.vymatr.v32 = active_view_matrix[ 9]/scale;
   actvy.vymatr.v13 = active_view_matrix[ 2]/scale;
   actvy.vymatr.v23 = active_view_matrix[ 6]/scale;
   actvy.vymatr.v33 = active_view_matrix[10]/scale;

   vytab[i_view].vymatr.v11 = active_view_matrix[ 0]/scale;
   vytab[i_view].vymatr.v21 = active_view_matrix[ 4]/scale;
   vytab[i_view].vymatr.v31 = active_view_matrix[ 8]/scale;
   vytab[i_view].vymatr.v12 = active_view_matrix[ 1]/scale;
   vytab[i_view].vymatr.v22 = active_view_matrix[ 5]/scale;
   vytab[i_view].vymatr.v32 = active_view_matrix[ 9]/scale;
   vytab[i_view].vymatr.v13 = active_view_matrix[ 2]/scale;
   vytab[i_view].vymatr.v23 = active_view_matrix[ 6]/scale;
   vytab[i_view].vymatr.v33 = active_view_matrix[10]/scale;


   actvy.vymatr.v11 = active_view_matrix[ 0]/scale;
   actvy.vymatr.v21 = active_view_matrix[ 1]/scale;
   actvy.vymatr.v31 = active_view_matrix[ 2]/scale;
   actvy.vymatr.v12 = active_view_matrix[ 4]/scale;
   actvy.vymatr.v22 = active_view_matrix[ 5]/scale;
   actvy.vymatr.v32 = active_view_matrix[ 6]/scale;
   actvy.vymatr.v13 = active_view_matrix[ 8]/scale;
   actvy.vymatr.v23 = active_view_matrix[ 9]/scale;
   actvy.vymatr.v33 = active_view_matrix[10]/scale;


   vytab[i_view].vymatr.v11 = active_view_matrix[ 0]/scale;
   vytab[i_view].vymatr.v21 = active_view_matrix[ 1]/scale;
   vytab[i_view].vymatr.v31 = active_view_matrix[ 2]/scale;
   vytab[i_view].vymatr.v12 = active_view_matrix[ 4]/scale;
   vytab[i_view].vymatr.v22 = active_view_matrix[ 5]/scale;
   vytab[i_view].vymatr.v32 = active_view_matrix[ 6]/scale;
   vytab[i_view].vymatr.v13 = active_view_matrix[ 8]/scale;
   vytab[i_view].vymatr.v23 = active_view_matrix[ 9]/scale;
   vytab[i_view].vymatr.v33 = active_view_matrix[10]/scale;

/*
***Move  
*/

   actvy.vywin[0] =   actvy.vywin[0] -  rwinpt->movx;
   actvy.vywin[1] =   actvy.vywin[1] -  rwinpt->movy;
   actvy.vywin[2] =   actvy.vywin[2] -  rwinpt->movx;
   actvy.vywin[3] =   actvy.vywin[3] -  rwinpt->movy;

   vytab[i_view].vywin[0] =   vytab[i_view].vywin[0] -  rwinpt->movx;
   vytab[i_view].vywin[1] =   vytab[i_view].vywin[1] -  rwinpt->movy;
   vytab[i_view].vywin[2] =   vytab[i_view].vywin[2] -  rwinpt->movx;
   vytab[i_view].vywin[3] =   vytab[i_view].vywin[3] -  rwinpt->movy;

/*
***Get WPGWIN pointer to the active view
*/
   gwinpt = NULL;
   for ( i_window=0; i_window<WTABSIZ; ++i_window )
     {
     if ( (winptr=WPwgwp((wpw_id)i_window)) != NULL  &&
           winptr->typ == TYP_GWIN )
       {
/*     Graphic window is found */
       gwinpt = (WPGWIN *)winptr->ptr;
/*     The right view ?                             */
       if ( strcmp(gwinpt->vy.vynamn,actvy.vynamn ) == 0 )
         {
/*       Yes, we have got the pointer. End of loop  */
         break;
         }
       }
     }
     if ( NULL == gwinpt ) 
       {
       return(-3);
       }


/*
***Scale view (= the vywin[] array). Note that the user defined
***scale for the rendering window is 1.0. The active view 
***window scale shall be changed to the original scale
***multiplied with the user defined new scale (scale_in*scale)
*/

   mdx   =  gwinpt->geo.psiz_x *
           (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   mdy   =  gwinpt->geo.psiz_y *
           (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   scale_in = mdx/(gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);

/* TODO Verify that EXscvi can and shall be used */
   status =EXscvi(actvy.vynamn, scale_in*scale);
   if ( status < 0 )
     {
     return(-2);
     }

   actvy.vywin[0] =   vytab[i_view].vywin[0];
   actvy.vywin[1] =   vytab[i_view].vywin[1];
   actvy.vywin[2] =   vytab[i_view].vywin[2];
   actvy.vywin[3] =   vytab[i_view].vywin[3];

/*
***Repaint active view window 
***TODO Fungerar ej. Måste byta till samma, aktiva vy
*/

    WPrepa((wpw_id)gwinpt->id.w_id);

    return(0);
  }

/********************************************************/

/*
***Dummy för system utan OpenGL 
*/

#else
short WPrenw()                                                     {return(0);}
bool  WPbtrw(WPRWIN *rwinpt, XButtonEvent *butev, wpw_id *serv_id) {return(0);}
bool  WPxprw(WPRWIN *rwinpt, XExposeEvent *expev)                  {return(0);}
bool  WPcrrw(WPRWIN *rwinpt, XCrossingEvent *croev)                {return(0);}
bool  WPcmrw(WPRWIN *rwinpt, XClientMessageEvent *clmev)           {return(0);}
short WPdlrw(WPRWIN *rwinpt)                                       {return(0);}
short WPuact(WPRWIN *rwinpt, GLfloat active_view_matrix[])         {return(0);}
bool  WPcorw(WPRWIN *rwinpt, XConfigureEvent *conev)               {return(0);}
#endif

