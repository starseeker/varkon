/**********************************************************************
*
*    wpGWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPwcgw();   Create WPGWIN
*    WPnrgw();   Normalize modelwindow
*    WPxpgw();   Expose routine for WPGWIN 
*    WPcrgw();   Crossing routine for WPGWIN
*    WPbtgw();   Button routine for WPGWIN
*    WPrpgw();   Reparent routine for WPGWIN
*    WPcogw();   Configure routine for WPGWIN
*    WPcmgw();   Client message routine for WPGWIN
*    WPergw();   Erase one or more WPGWIN
*    WPrepa();   Repaint WPGWIN
*    WPdlgw();   Kill one WPGWIN
*   *WPggwp();   Map X-id to WPGWIN-C-pointer
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
#include "../include/WP.h"
#include "../include/v3icon.h"
#include <string.h>

extern char         jobnam[];
extern short        actpen,actfun,v3mode;
extern DBfloat        rstrox,rstroy,rstrdx,rstrdy;
extern bool         rstron;
extern DBptr        lsysla;

static short creqbt(WPGWIN *gwinpt);

/*!******************************************************/

        short WPwcgw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        bool    main,
        DBint  *id)

/*      Skapar WPGWIN-fönster. V3:s grafiska huvudfönster
 *      får ock skall ovillkorligen ha ID = 0 = GWIN_MAIN.
 *
 *      In: x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = Fönstertitel.
 *          main  = TRUE  = Huvudfönstret
 *                  FALSE = Ytterligare fönster
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel från malloc().
 *
 *      (C)microform ab 24/1/94 J. Kjellander
 *
 *      13/12/94 Rub_GC, J. Kjellander
 *      27/12/94 Multifönster, J. Kjellander
 *      1997-06-11 Colormap, J.Kjellander
 *      1997-12-27 Individuella GC:n, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    unsigned char        dlist[2];
    XSizeHints           xhint;
    Window               xwin_id;
    XWMHints             wmhint;
    Pixmap               IconPixmap,SavePixmap;
    GC                   Win_GC,Rub_GC;
    XGCValues            values;
    WPWIN               *winptr;
    WPGWIN              *gwinpt,*mainpt;
    int                  i,depth,width,height,margin;
    char                *type[20];
    XrmValue             value;


/*
***Skapa ett ledigt fönster-ID. Om det är huvudfönstret
***som skall skapas tvingar vi ID till GWIN_MAIN = 0.
*/
    if ( main ) *id = GWIN_MAIN;
    else if ( (*id=WPwffi()) < 0 ) return(erpush("WP1052",label));
/*
***Sätt färg mm.
*/
    xwina.background_pixel  = WPgcol(0);
    xwina.border_pixel      = WPgcol(1);
    xwina.override_redirect = False;
    xwina.save_under        = False;
    xwina.colormap          = DefaultColormap(xdisp,xscr);

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder   | CWColormap );
/*
***Skapa ett fönster.
*/
    depth  = DefaultDepth(xdisp,xscr);

    xwin_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),x,y,dx,dy,3,depth,
                            InputOutput,CopyFromParent,xwinm,&xwina);
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
    XSelectInput(xdisp,xwin_id,GWEM_NORM );
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
***Skapa ett GC för det grafiska fönstret.
***Sätt graphics_exposures till false så vi slipper sådana
***event i onödan.
*/
    Win_GC = XCreateGC(xdisp,xwin_id,0,&values);
    values.graphics_exposures = False;
    XChangeGC(xdisp,Win_GC,GCGraphicsExposures,&values);
/*
***Skapa ett GC till för gummibandsritning.
***Färg nummer 3 ger blått gummiband när bakgrunden är
***vit men är egentligen grön. Detta beror på XOR. Färger
***och XOR funkar tydligen bara med svart bakgrund.
*/
    if ( main )
      {
      Rub_GC = XCreateGC(xdisp,xwin_id,0,&values);
      XSetLineAttributes(xdisp,Rub_GC,2,LineOnOffDash,CapButt,JoinBevel);
      dlist[0] = (unsigned char)((double)width/40.0);
      dlist[1] = (unsigned char)((double)dlist[0]/2.5);
      XSetDashes(xdisp,Rub_GC,(unsigned int)0,(char *)dlist,2);
      XSetFunction(xdisp,Rub_GC,GXxor);
      XSetBackground(xdisp,Rub_GC,WPgcol(0));
      XSetForeground(xdisp,Rub_GC,WPgcol(3));
      }
/*
***Om det inte är huvudfönstret det är frågan om utan ytterligare
***grafiska fönster skall dessa dela Rub_GC med huvudfönstret.
*/
    else
      {
      winptr = WPwgwp((wpw_id)GWIN_MAIN);
      mainpt = (WPGWIN *)winptr->ptr;
      Rub_GC = mainpt->rub_gc;
      }
/*
***Skapa pixmap för 'save under' på V3:s grafiska fönster.
***För att pixmappen säkert skall klara ett stort grafiskt
***fönster gör vi den lika stor som hela skärmen.
*/
    SavePixmap = XCreatePixmap(xdisp,xwin_id,width,height,depth);
/*
***Nollställ den. Samtidigt sätter vi bak- och för-grund i
***fönstrets GC.
*/
    XSetBackground(xdisp,Win_GC,WPgcol(0));
    XSetForeground(xdisp,Win_GC,WPgcol(0));
    XFillRectangle(xdisp,SavePixmap,Win_GC,0,0,width,height);
    XSetForeground(xdisp,Win_GC,WPgcol(1));
/*
***Skapa ett WPGWIN.
*/
    if ( (gwinpt=(WPGWIN *)v3mall(sizeof(WPGWIN),"WPwcgw")) == NULL )
      return(erpush("WP1062",label));

    gwinpt->id.w_id = *id;
    gwinpt->id.p_id =  0;
    gwinpt->id.x_id =  xwin_id;

    gwinpt->geo.x =  x;
    gwinpt->geo.y =  y;
    gwinpt->geo.dx =  dx;
    gwinpt->geo.dy =  dy;
    gwinpt->geo.psiz_x = XDisplayWidthMM(xdisp,xscr) /
                         (double)DisplayWidth(xdisp,xscr);
    gwinpt->geo.psiz_y = XDisplayHeightMM(xdisp,xscr) /
                         (double)DisplayHeight(xdisp,xscr);

    gwinpt->savmap = SavePixmap;
    gwinpt->win_gc = Win_GC;
    gwinpt->rub_gc = Rub_GC;
    gwinpt->linwdt = 0.0;

    gwinpt->reprnt = FALSE;
    gwinpt->wmandx = 0;
    gwinpt->wmandy = 0;

    gwinpt->mesbpt = NULL;
/*
***Default aktiv vy. Motsvarar vy:n "xy" med skala = 1.0.
***och origo i nedre vänstra hörnet.
*/
    gwinpt->vy.vynamn[0] = '\0';
/*
***Hur stort skall skärmfönstret vara ? Skall det ha marginaler
***för tryckknappar ?
*/
    gwinpt->vy.scrwin.xmin = 0;
    gwinpt->vy.scrwin.ymin = 0;
    gwinpt->vy.scrwin.xmax = dx;
    gwinpt->vy.scrwin.ymax = dy;

    if ( XrmGetResource(xresDB,"varkon.margin_up",
                        "Varkon.Margin_up",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.ymax -= margin;

    if ( XrmGetResource(xresDB,"varkon.margin_down",
                        "Varkon.Margin_down",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.ymin += margin;

    if ( XrmGetResource(xresDB,"varkon.margin_left",
                        "Varkon.Margin_left",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.xmin += margin;

    if ( XrmGetResource(xresDB,"varkon.margin_right",
                        "Varkon.Margin_right",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.xmax -= margin;
/*
***Modellfönstret gör vi lika med det grafiska fönstret i skala = 1.0.
*/
    gwinpt->vy.modwin.xmin = 0;
    gwinpt->vy.modwin.ymin = 0;
    gwinpt->vy.modwin.xmax =
              (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
               DisplayWidthMM(xdisp,xscr) / DisplayWidth(xdisp,xscr);
    gwinpt->vy.modwin.ymax =
              (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) *
               DisplayHeightMM(xdisp,xscr) / DisplayHeight(xdisp,xscr);

    gwinpt->vy.vy_3D = FALSE;
    gwinpt->vy.vydist = 0.0;

    gwinpt->vy.vymat.k11 = 1.0;
    gwinpt->vy.vymat.k12 = 0.0;
    gwinpt->vy.vymat.k13 = 0.0;
    gwinpt->vy.vymat.k21 = 0.0;
    gwinpt->vy.vymat.k22 = 1.0;
    gwinpt->vy.vymat.k23 = 0.0;
    gwinpt->vy.vymat.k31 = 0.0;
    gwinpt->vy.vymat.k32 = 0.0;
    gwinpt->vy.vymat.k33 = 1.0;
/*
***Denna vy är fullständig och kan användas, valid = TRUE men
***den har inget namn. Föregående vy finns ännu ej.
*/
    gwinpt->vy.valid     = TRUE;
    gwinpt->old_vy.valid = FALSE;
/*
***Displayfil.
*/
    gwinpt->df_adr =  NULL;
    gwinpt->df_all =  0;
    gwinpt->df_ptr = -1;
    gwinpt->df_cur = -1;
/*
***Tänd alla nivåer.
*/
    for ( i=0; i<WP_NTSIZ; ++i ) gwinpt->nivtab[i] = 255;
/*
***Fönstret har ännu inga subfönster.
*/
    for ( i=0; i<WP_GWSMAX; ++i) gwinpt->wintab[i].ptr = NULL;
/*
***Eller pekmärken.
*/
    gwinpt->pmkant = 0;
/*
***Lagra fönstret i fönstertabellen.
*/
    wpwtab[*id].typ = TYP_GWIN;
    wpwtab[*id].ptr = (char *)gwinpt;
/*
***Skapa ev. tryckknappar för snabbval.
*/
    creqbt(gwinpt);
/*
***Visa resultatet.
*/
    WPwshw(*id);
/*
***Normalisera, dvs. fixa till modellfönstrets proportioner
***och beräkna 2D transformationskonstanter.
*/
    WPnrgw(gwinpt);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPnrgw(
        WPGWIN *gwinpt)

/*      Normaliserar proportionerna för GWIN-fönstrets
 *      modellfönster och beräknar nya 2D-transformations-
 *      konstanter.
 *
 *      In: gwinpt => Pekare till fönster.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 29/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   double mdx,mdy,gadx,gady,mprop,gprop;

/*
***Hur stor är fönstrets grafiska area.
*/
   gadx = gwinpt->geo.psiz_x *
        (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   gady = gwinpt->geo.psiz_y *
        (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Hur stort är modellfönstret i millimeter.
*/
   mdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
   mdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin;
/*
***Förhållandet mellan grafiska areans höjd och bredd är gady/gadx.
***Se till att modellfönstret får samma förhållande så att cirklar
***blir "runda" tex.
*/
   gprop = gady/gadx;
   mprop = mdy/mdx;

   if ( mprop > gprop )
     {
     gwinpt->vy.modwin.xmin -= (mdy/gprop - mdx)/2.0;
     gwinpt->vy.modwin.xmax += (mdy/gprop - mdx)/2.0;
     }
   else if ( mprop < gprop )
     {
     gwinpt->vy.modwin.ymin -= (gprop*mdx - mdy)/2.0;
     gwinpt->vy.modwin.ymax += (gprop*mdx - mdy)/2.0;
     }
/*
***Beräkna nya transformationskonstanter.
*/
   gwinpt->vy.k2x = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) /
                    (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
   gwinpt->vy.k2y = (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) /
                    (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);

   gwinpt->vy.k1x = gwinpt->vy.scrwin.xmin -
                    gwinpt->vy.modwin.xmin * gwinpt->vy.k2x;
   gwinpt->vy.k1y = gwinpt->vy.scrwin.ymin -
                    gwinpt->vy.modwin.ymin * gwinpt->vy.k2y;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPxpgw(
        WPGWIN       *gwinpt,
        XExposeEvent *expev)

/*      Expose-rutin för WPGWIN.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 23/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    char   *subptr;

/*
***Först expose på alla sub-fönster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
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
***Även "meddelanderaden" är ett subfönster.
*/
   if ( gwinpt->mesbpt != NULL ) WPxpbu(gwinpt->mesbpt);
/*
***Kopiera från 'save under'-pixmappen.
*/
    XCopyArea(xdisp,gwinpt->savmap,gwinpt->id.x_id,gwinpt->win_gc,
                expev->x,expev->y,expev->width,expev->height,
                expev->x,expev->y);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcrgw(
        WPGWIN         *gwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPGWIN med vidhängande sub-fönster.
 *      Kollar om Leave/Enter skett i något av WPGWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      Felkod: .
 *
 *      (C)microform ab 16/12/94 J. Kjellander
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
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
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

        bool          WPbtgw(
        WPGWIN       *gwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin för WPGWIN med vidhängande sub-fönster.
 *      Kollar om muspekning skett i något av WPGWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: iwinptr = C-pekare till WPGWIN.
 *          butev   = X-but event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 16/12/94 J. Kjellander
 *
 *      1997-02-18 f189, J.Kjellander
 *      1998-09-14 EXIT, J.Kjellander
 *
 ******************************************************!*/

  {
    int     x=0,y=0;
    short   i,acttyp=0,actnum=0,oldfun;
    bool    hit;
    char   *subptr;
    WPBUTT *butptr;
    WPICON *icoptr;

/*
***WPGWIN självt kan inte generera ButtonEvent:s,
***bara sub-fönstren.
*/
    hit = FALSE;

    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          butptr = (WPBUTT *)subptr;
          if ( butev->window == butptr->id.x_id )
            {
            x = gwinpt->geo.x + butptr->geo.x;
            y = gwinpt->geo.y + butptr->geo.y;
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
            x = gwinpt->geo.x + icoptr->geo.x;
            y = gwinpt->geo.y + icoptr->geo.y;
            hit     = TRUE;
            acttyp  = icoptr->acttyp;
            actnum  = icoptr->actnum;
           *serv_id = icoptr->id.w_id;
            }
          break;
          }
/*
***Träff kanske ?
*/
        if ( hit )
          {
/*
***Om så tar vi särskilt hand om vissa snabbval här.
*/
          if ( acttyp == FUNC )
            {
            oldfun = actfun;
            actfun = actnum;

            switch ( actnum )
              {
              case 188:
              WPscle(gwinpt,x,y);
              break;

              case 189:
              WPcent(gwinpt);
              break;

              case 190:
              if ( v3mode == BAS3_MOD ) WPperp(gwinpt,x,y);
              else XBell(xdisp,100);
              break;

              case 191:
              WPlstv(gwinpt);
              break;

              case 193:
              WPzoom(gwinpt);
              break;

              case 194:
              WPiaut(gwinpt);
              break;

              case 195:
              WPchvi(gwinpt,x,y);
              break;

              case 197:
              WPnivs(gwinpt,x,y);
              break;

              case 198:
              WPshad((int)gwinpt->id.w_id,FALSE);
              break;

              default:
              actfun = oldfun;
              if ( igdofu(acttyp,actnum) == EXIT ) v3exit(); 
              break;
              }
            actfun = oldfun;
            return(TRUE);
            }
          else
            {
            if ( igdofu(acttyp,actnum) == EXIT ) v3exit(); 
            else return(TRUE);
            }
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPrpgw(
        WPGWIN *gwinpt,
        XReparentEvent *repev)

/*      Reparent-rutin för WPGWIN.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *          repev  = C-pekare till ReparentEvent.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE.
 *
 *      (C)microform ab 31/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sätt reparent-flagga.
*/
   gwinpt->reprnt = TRUE;

   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcogw(
        WPGWIN *gwinpt,
        XConfigureEvent *conev)

/*      Configure-ruutine for WPGWIN.
 *
 *      In: gwinpt = C-ptr to WPGWIN.
 *          conev  = C-ptr to ConfigureEvent.
 *
 *      Out: Nothing.   
 *
 *      Return: TRUE.
 *
 *      (C)microform ab 8/2/94 J. Kjellander
 *
 *      1994-01-08 Multifönster, J. Kjellander
 *      2006-11-27 Pending events, Johan Kjellander
 *
 ******************************************************!*/

  {
    int    oldx,oldy,olddx,olddy,newx,newy,newdx,newdy,
           dx,dy,ddx,ddy;
    double oldmdx,oldmdy,sizx,sizy;
    bool   right,left,up,down;
    Window child;
    XEvent event;
   
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
    oldx  = gwinpt->geo.x;  oldy  = gwinpt->geo.y;
    olddx = gwinpt->geo.dx; olddy = gwinpt->geo.dy;
/*
***Where is the window now ? Only way to be
***sure is with XTranslate...
*/ 
    XTranslateCoordinates(xdisp,gwinpt->id.x_id,
                      DefaultRootWindow(xdisp),0,0,&newx,&newy,&child);
/*
***What is the new size ?
*/
    newdx = conev->width;
    newdy = conev->height;
/*
***How big is the change in size and position ?
*/
    dx  = newx -  oldx;  dy  = newy -  oldy;
    ddx = newdx - olddx; ddy = newdy - olddy;
/*
***The window may have: 1 - Be moved.
***                     2 - Changed size without move.
***                     3 - Moved and changed size (moving upper/left border).
***Which one is it ?
*/
    right = left = up = down = FALSE;

    if ( ddx != 0 )
      {
      if ( dx == 0 ) right = TRUE;
      else           left  = TRUE;
      }

    if ( ddy != 0 )
      {
      if ( dy == 0 ) down = TRUE;
      else           up   = TRUE;
      }
/*
***Om fönstret ännu inte har "reparentiserats" av WM
***kan en flyttning av fönstret inte bero på användaren
***av systemet utan måste bero på WM:s garnering av
***fönstret med egna ramar etc. Isåfall sparar vi på
***oss denna förflyttning så att vi vet hur stor den är.
*/
    if ( !gwinpt->reprnt )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      }
/*
***Om ovanstående garnering ännu inte skett, dvs. (wmandx,wmandy) = 0
***men fönstret ändå "reparentiserats" har WM valt att göra saker i
***en annan ordning än normalt. Isåfall tolkar vi denna första förflyttning
***som garnering i alla fall.
*/
    else if ( gwinpt->wmandx == 0  &&  gwinpt->wmandy == 0 )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      }
/*
***Lagra den nya fönstergeometrin i WPGWIN-posten.
*/
    gwinpt->geo.x  = newx;
    gwinpt->geo.y  = newy;
    gwinpt->geo.dx = newdx;
    gwinpt->geo.dy = newdy;
    gwinpt->geo.bw = conev->border_width;
/*
***Vad är det som har hänt ?
***Om fönstret har ändrat storlek beräknar vi nytt modell-
***fönster, viewport mm. så att bilden efter automatisk repaint
***ligger kvar på samma ställe som förut även om fönstrets origo
***pga. ändringen har flyttats.
*/
    if ( ( ddx != 0.0 ) || ( ddy != 0.0 ) )
      {
/*
***Under all omständigheter skall grafiska arean ändra storlek.
***Detta gör vi genom att flytta origo till det nya nedre vänstra
***hörnet och justera xmax och ymax därefter oavsett om det är
***högra eller vänstra alt. övre eller nedre kanten som har ändrats.
*/
      gwinpt->vy.scrwin.xmax += ddx;
      gwinpt->vy.scrwin.ymax += ddy;
/*
***Hur blir det med modellfönstret ? Här justerar vi den kant som
***verkligen har ändrats så att bilden ligger still på skärmen.
*/
    oldmdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
    oldmdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin;

      if ( right )
        gwinpt->vy.modwin.xmax += (double)ddx/(double)olddx*oldmdx;

      if ( left )
        gwinpt->vy.modwin.xmin -= (double)ddx/(double)olddx*oldmdx;

      if ( up )
        gwinpt->vy.modwin.ymax += (double)ddy/(double)olddy*oldmdy;

      if ( down )
        gwinpt->vy.modwin.ymin -= (double)ddy/(double)olddy*oldmdy;
/*
***Nya 2D transformationskonstanter.
*/
      gwinpt->vy.k2x =
        (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) /
        (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
      gwinpt->vy.k2y =
        (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) /
        (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);
      gwinpt->vy.k1x =
        gwinpt->vy.scrwin.xmin - gwinpt->vy.modwin.xmin * gwinpt->vy.k2x;
      gwinpt->vy.k1y =
        gwinpt->vy.scrwin.ymin - gwinpt->vy.modwin.ymin * gwinpt->vy.k2y;
/*
***Rita om fönstret.
*/
      WPrepa((wpw_id)gwinpt->id.w_id);
/*
***Föregående vy skall nu kunna visas i det nya fönstret, alltså
***måste även dennas modellfönster uppdateras.
*/
      if ( gwinpt->old_vy.valid )
        {
        gwinpt->old_vy.scrwin.xmax += ddx;
        gwinpt->old_vy.scrwin.ymax += ddy;
        oldmdx = gwinpt->old_vy.modwin.xmax - gwinpt->old_vy.modwin.xmin;
        oldmdy = gwinpt->old_vy.modwin.ymax - gwinpt->old_vy.modwin.ymin;

        if ( right )
          gwinpt->old_vy.modwin.xmax += (double)ddx/(double)olddx*oldmdx;

        if ( left )
          gwinpt->old_vy.modwin.xmin -= (double)ddx/(double)olddx*oldmdx;

        if ( up )
          gwinpt->old_vy.modwin.ymax += (double)ddy/(double)olddy*oldmdy;

        if ( down )
          gwinpt->old_vy.modwin.ymin -= (double)ddy/(double)olddy*oldmdy;

        gwinpt->old_vy.k2x =
          (gwinpt->old_vy.scrwin.xmax - gwinpt->old_vy.scrwin.xmin) /
          (gwinpt->old_vy.modwin.xmax - gwinpt->old_vy.modwin.xmin);
        gwinpt->old_vy.k2y =
          (gwinpt->old_vy.scrwin.ymax - gwinpt->old_vy.scrwin.ymin) /
          (gwinpt->old_vy.modwin.ymax - gwinpt->old_vy.modwin.ymin);
        gwinpt->old_vy.k1x = gwinpt->old_vy.scrwin.xmin -
                        gwinpt->old_vy.modwin.xmin * gwinpt->old_vy.k2x;
        gwinpt->old_vy.k1y = gwinpt->old_vy.scrwin.ymin -
                        gwinpt->old_vy.modwin.ymin * gwinpt->old_vy.k2y;
        }
      }
/*
***Om fönstret inte har ändrat storlek behöver vi inte göra så mycket.
*/
    else
      {
      ;
      }
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcmgw(
        WPGWIN               *gwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen för WPGWIN.
 *
 *      In: gwinpt  = C-pekare till WPGWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 28/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om det är WM_DELETE_WINDOW servar vi genom att döda
***fönstret ifråga såvida det inte är huvudfönstret
***förstås. Detta skall dock inte kunna ta emot ett client-
***message = WM_DELETE_WINDOW.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) &&
        gwinpt->id.w_id > GWIN_MAIN )
     {
     WPwdel((DBint)gwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short WPergw(
        DBint win_id)

/*      Suddar ett grafiskt fönster med tillhörande
 *      pixmap för save_under och DF mm.
 *
 *      In: win_id = ID för ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = Fönstret %s finns ej.
 *
 *      (C)microform ab 27/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   bool    hit;
   int     i;
   char    errbuf[81];
   short   oldpen;
   WPGWIN *gwinpt;

/*
***Initiering.
*/
   hit = FALSE;
/*
***Loopa igenom alla WPGWIN och sudda det eller de som
***önskats.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Ett WPGWIN har hittats. Skall det suddas ?
*/
         if ( win_id == gwinpt->id.w_id  ||  win_id == GWIN_ALL )
           {
/*
***Ja, sudda själva fönstret.
*/
           hit = TRUE;
           XClearWindow(xdisp,gwinpt->id.x_id);
/*
***Sudda save_under-pixmappen.
*/
           oldpen = actpen;
           WPspen(0);
           XFillRectangle(xdisp,gwinpt->savmap,gwinpt->win_gc,0,0,
           DisplayWidth(xdisp,xscr),DisplayHeight(xdisp,xscr));
           WPspen(oldpen);
/*
***Sudda fönstrets DF.
*/
           gwinpt->df_ptr = gwinpt->df_cur = -1;
/*
***Nollställ pekmärken.
*/
           gwinpt->pmkant = 0;
/*
***Gammal aktiv linjebredd gäller nu inte längre.
*/
           gwinpt->linwdt = 0.0;
           }
         }
       }
     }
/*
***Gick det bra ?
*/
   if ( !hit )
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1362",errbuf));
     }
   else return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPrepa(
        DBint win_id)

/*      Riatr om ett grafiskt fönster med tillhörande
 *      pixmap för save_under.
 *
 *      In: win_id = ID för ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = Fönstret %s finns ej.
 *
 *      (C)microform ab 1996-02-13 J. Kjellander
 *
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Sudda.
*/
    WPergw(win_id);
/*
***Slå på väntan.
*/
    WPwait(win_id,TRUE);
/*
***Rita om GM.
*/
    EXdral(win_id);
/*
***Uppdatera aktivt koordinatsustem.
*/
    igupcs(lsysla,V3_CS_ACTIVE);
/*
***Slå av väntan.
*/
    WPwait(win_id,FALSE);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdlgw(
        WPGWIN *gwinpt)

/*      Dödar ett WPGWIN.
 *
 *      In: gwinptr = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 *      15/3/95  Även subfönster, J. Kjellander
 *      1998-01-04 Individuella GC:n, J.Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Döda eventuella subfönster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
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
***Även "meddelanderaden" är ett subfönster.
*/
   if ( gwinpt->mesbpt != NULL ) WPdlbu(gwinpt->mesbpt);
/*
***Lämna tillbaks grafiska fönstrets "Save under"-pixmapp.
*/
    XFreePixmap(xdisp,gwinpt->savmap);
/*
***Lämna tillbaks fönstrets GC.
*/
    XFreeGC(xdisp,gwinpt->win_gc);
/*
***Om det är GWIN_MAIN det gäller lämnar vi även tillbaks
***även gummibands-GC:t som ju delas av alla grafiska fönster.
*/
    if ( gwinpt->id.w_id == GWIN_MAIN )
      {
      XFreeGC(xdisp,gwinpt->rub_gc);
      }
/*
***Här borde eventuella grafiska cursors lämnas tillbaks
***med XFreeCursor(id); Eller kanske i wp1 eftersom flera
***grafiska fönster kan använda samma cursor.
*/
/*
***Displayfil.
*/
    if ( gwinpt->df_adr != NULL) v3free((char *)gwinpt->df_adr,"WPdlgw");
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)gwinpt,"WPdlgw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPGWIN *WPggwp(
        Window   x_id)

/*      Letar upp det grafiska fönstret med angiven
 *      X-id och returnerar en C-pekare till fönstret.
 *
 *      In: x_id  = Fönstrets X-id.
 *
 *      Ut: Inget.   
 *
 *      FV: C-adress eller NULL om fönster saknas.
 *
 *      (C)microform ab 13/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPGWIN  *gwinpt;

/*
***Leta upp fönstret.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
        if ( wpwtab[i].typ == TYP_GWIN )
          {
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
          if ( gwinpt->id.x_id == x_id ) return(gwinpt);
          }
        }
      }
/*
***Ingen träff.
*/
    return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static short creqbt(
        WPGWIN *gwinpt)

/*      Skapar grafiska fönstrets snabbvalsknappar.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 16/12/94 J. Kjellander
 *
 *      1997-01-15 v3genv(), J.Kjellander
 *
 ******************************************************!*/

  {
    char      xrmstr1[40],xrmstr2[40],numstr[10],butstr[V3STRLEN+1],
              actstr[20],iconam[V3PTHLEN+1];
    char     *type[20];
    long      flags;
    XrmValue  value;
    int       i,x,y,nsub;
    unsigned int dx,dy;
    short     status;
    WPBUTT   *buttpt;
    WPICON   *iconpt;

/*
***Antal subfönster är till att börja med = 0.
*/
    nsub    = 0;
/*
***Vi börjar med textknapparna. varkon.button_n.geometry
***                             varkon.button_n.text
***                             varkon.button_n.action
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      strcpy(xrmstr1,"varkon.button_");
      strcpy(xrmstr2,"Varkon.Button_");
      sprintf(numstr,"%d",i);
      strcat(xrmstr1,numstr);
      strcat(xrmstr2,numstr);
      strcat(xrmstr1,".geometry");
      strcat(xrmstr2,".Geometry");
/*
***Prova att hämta .geometry-resursen.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry finns. Då provar vi med texten.
***Om inte text finns tar vi knappens nummer istället.
*/
        strcpy(xrmstr1,"varkon.button_");
        strcpy(xrmstr2,"Varkon.Button_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".text");
        strcat(xrmstr2,".Text");

        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          strcpy(butstr,value.addr);
        else
          strcpy(butstr,numstr);
/*
***Då skapar vi en tryckknapp.
*/
        status = WPwcbu(gwinpt->id.x_id,
                       (short)x,(short)y,(short)dx,(short)dy,(short)1,
                        butstr,butstr,"",WP_BGND,WP_FGND,&buttpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_BUTTON;
          gwinpt->wintab[nsub].ptr = (char *)buttpt;
/*
***Nu fattas bara aktionskoden. Om sådan saknas väljer vi "f0".
*/
          strcpy(xrmstr1,"varkon.button_");
          strcpy(xrmstr2,"Varkon.Button_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".action");
          strcat(xrmstr2,".Action");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            strcpy(actstr,value.addr);
          else
            strcpy(actstr,"f0");

          if ( sscanf(&actstr[1],"%hd",&buttpt->actnum) != 1 )
            buttpt->actnum = 0;
  
          switch ( actstr[0] )
            {
            case 'f': buttpt->acttyp = FUNC; break;
            case 'm': buttpt->acttyp = MENU; break;
            case 'p': buttpt->acttyp = PART; break;
            case 'M': buttpt->acttyp = MFUNC; break;
            case 'r': buttpt->acttyp = RUN; break;
  
            default:
            buttpt->acttyp = FUNC;
            buttpt->actnum = 0;
            break;
            }
/*
***Slutligen räknar vi upp nsub.
*/
          if ( ++nsub == WP_GWSMAX ) return(0);
          }
        }
/*
***Kanske finns det en ikon med samma nummer.
*/
      strcpy(xrmstr1,"varkon.icon_");
      strcpy(xrmstr2,"Varkon.Icon_");
      sprintf(numstr,"%d",i);
      strcat(xrmstr1,numstr);
      strcat(xrmstr2,numstr);
      strcat(xrmstr1,".geometry");
      strcat(xrmstr2,".Geometry");
/*
***Prova att hämta .geometry-resursen.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry finns. Då provar vi med filnamnet.
*/
        strcpy(xrmstr1,"varkon.icon_");
        strcpy(xrmstr2,"Varkon.Icon_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".name");
        strcat(xrmstr2,".Name");
/*
***Filnamn.
*/
        strcpy(iconam,v3genv(VARKON_ICO));
        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          strcat(iconam,value.addr);
/*
***Då skapar vi en ikon.
*/
        status = WPwcic(gwinpt->id.x_id,
                       (short)x,(short)y,(short)1,iconam,
                       WP_BGND,WP_FGND,&iconpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_ICON;
          gwinpt->wintab[nsub].ptr = (char *)iconpt;
/*
***Nu fattas bara aktionskoden. Om sådan saknas väljer vi "f0".
*/
          strcpy(xrmstr1,"varkon.icon_");
          strcpy(xrmstr2,"Varkon.Icon_");
          sprintf(numstr,"%d",i);
          strcat(xrmstr1,numstr);
          strcat(xrmstr2,numstr);
          strcat(xrmstr1,".action");
          strcat(xrmstr2,".Action");

          if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
            strcpy(actstr,value.addr);
          else
            strcpy(actstr,"f0");

          if ( sscanf(&actstr[1],"%hd",&iconpt->actnum) != 1 )
            iconpt->actnum = 0;

          switch ( actstr[0] )
            {
            case 'f': iconpt->acttyp = FUNC; break;
            case 'm': iconpt->acttyp = MENU; break;
            case 'p': iconpt->acttyp = PART; break;
            case 'M': iconpt->acttyp = MFUNC; break;
            case 'r': iconpt->acttyp = RUN; break;
    
            default:
            iconpt->acttyp = FUNC;
            iconpt->actnum = 0;
            break;
            }
/*
***Slutligen räknar vi upp nsub.
*/
          if ( ++nsub == WP_GWSMAX ) return(0);
          }
       }
/*
***Nästa knapp eller ikon.
*/
    }

    return(0);
  }

/********************************************************/
