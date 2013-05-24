/**********************************************************************
*
*    wp15.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpwcgw();   Create WPGWIN
*    wpnrgw();   Normalize modelwindow
*    wpxpgw();   Expose routine for WPGWIN 
*    wpcrgw();   Crossing routine for WPGWIN
*    wpbtgw();   Button routine for WPGWIN
*    wprpgw();   Reparent routine for WPGWIN
*    wpcogw();   Configure routine for WPGWIN
*    wpcmgw();   Client message routine for WPGWIN
*    wpergw();   Kill one or more WPGWIN
*    wprepa();   Repaint WPGWIN
*    wpdlgw();   Kill one WPGWIN
*   *wpggwp();   Map X-id to WPGWIN-C-pointer
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include "../../GP/include/GP.h"
#include "../include/WP.h"
#include "../include/v3icon.h"
#include <string.h>

extern char         jobnam[];
extern short        actpen,actfun,v3mode;
extern gmflt        rstrox,rstroy,rstrdx,rstrdy;
extern bool         rstron;
extern DBptr        lsysla;

static short creqbt(WPGWIN *gwinpt);

/*!******************************************************/

        short wpwcgw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        bool    main,
        v2int  *id)

/*      Skapar WPGWIN-f�nster. V3:s grafiska huvudf�nster
 *      f�r ock skall ovillkorligen ha ID = 0 = GWIN_MAIN.
 *
 *      In: x     = L�ge i X-led.
 *          y     = L�ge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = F�nstertitel.
 *          main  = TRUE  = Huvudf�nstret
 *                  FALSE = Ytterligare f�nster
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel fr�n malloc().
 *
 *      (C)microform ab 24/1/94 J. Kjellander
 *
 *      13/12/94 Rub_GC, J. Kjellander
 *      27/12/94 Multif�nster, J. Kjellander
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
***Skapa ett ledigt f�nster-ID. Om det �r huvudf�nstret
***som skall skapas tvingar vi ID till GWIN_MAIN = 0.
*/
    if ( main ) *id = GWIN_MAIN;
    else if ( (*id=wpwffi()) < 0 ) return(erpush("WP1052",label));
/*
***S�tt f�rg mm.
*/
    xwina.background_pixel  = wpgcol(0);
    xwina.border_pixel      = wpgcol(1);
    xwina.override_redirect = False;
    xwina.save_under        = False;
    xwina.colormap          = DefaultColormap(xdisp,xscr);

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder   | CWColormap );
/*
***Skapa ett f�nster.
*/
    depth  = DefaultDepth(xdisp,xscr);

    xwin_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),x,y,dx,dy,3,depth,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Skicka hints till f�nsterhanteraren.
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
***F�nstertitel och ikon-text.
*/
    wpmaps(label); 
    XStoreName(xdisp,xwin_id,label);   
    XSetIconName(xdisp,xwin_id,jobnam);
/*
***Input.
*/
    XSelectInput(xdisp,xwin_id,GWEM_NORM);
/*
***Skapa ikon.
*/
   IconPixmap = XCreateBitmapFromData(xdisp,xwin_id,(char *)v3icon_bits,
                                      v3icon_width,v3icon_height);
   wmhint.icon_pixmap = IconPixmap;
   wmhint.flags       = IconPixmapHint;
   XSetWMHints(xdisp,xwin_id,&wmhint);
/*
***V3 f�r ej d�das av en WINDOW-Manager som tex. Motif.
*/
    wpsdpr(xwin_id);
/*
***Skapa ett GC f�r det grafiska f�nstret.
***S�tt graphics_exposures till false s� vi slipper s�dana
***event i on�dan.
*/
    Win_GC = XCreateGC(xdisp,xwin_id,0,&values);
    values.graphics_exposures = False;
    XChangeGC(xdisp,Win_GC,GCGraphicsExposures,&values);
/*
***Skapa ett GC till f�r gummibandsritning.
***F�rg nummer 3 ger bl�tt gummiband n�r bakgrunden �r
***vit men �r egentligen gr�n. Detta beror p� XOR. F�rger
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
      XSetBackground(xdisp,Rub_GC,wpgcol(0));
      XSetForeground(xdisp,Rub_GC,wpgcol(3));
      }
/*
***Om det inte �r huvudf�nstret det �r fr�gan om utan ytterligare
***grafiska f�nster skall dessa dela Rub_GC med huvudf�nstret.
*/
    else
      {
      winptr = wpwgwp((wpw_id)GWIN_MAIN);
      mainpt = (WPGWIN *)winptr->ptr;
      Rub_GC = mainpt->rub_gc;
      }
/*
***Skapa pixmap f�r 'save under' p� V3:s grafiska f�nster.
***F�r att pixmappen s�kert skall klara ett stort grafiskt
***f�nster g�r vi den lika stor som hela sk�rmen.
*/
    SavePixmap = XCreatePixmap(xdisp,xwin_id,width,height,depth);
/*
***Nollst�ll den. Samtidigt s�tter vi bak- och f�r-grund i
***f�nstrets GC.
*/
    XSetBackground(xdisp,Win_GC,wpgcol(0));
    XSetForeground(xdisp,Win_GC,wpgcol(0));
    XFillRectangle(xdisp,SavePixmap,Win_GC,0,0,width,height);
    XSetForeground(xdisp,Win_GC,wpgcol(1));
/*
***Skapa ett WPGWIN.
*/
    if ( (gwinpt=(WPGWIN *)v3mall(sizeof(WPGWIN),"wpwcgw")) == NULL )
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
***och origo i nedre v�nstra h�rnet.
*/
    gwinpt->vy.vynamn[0] = '\0';
/*
***Hur stort skall sk�rmf�nstret vara ? Skall det ha marginaler
***f�r tryckknappar ?
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
***Modellf�nstret g�r vi lika med det grafiska f�nstret i skala = 1.0.
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
***Denna vy �r fullst�ndig och kan anv�ndas, valid = TRUE men
***den har inget namn. F�reg�ende vy finns �nnu ej.
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
***T�nd alla niv�er.
*/
    for ( i=0; i<WP_NTSIZ; ++i ) gwinpt->nivtab[i] = 255;
/*
***F�nstret har �nnu inga subf�nster.
*/
    for ( i=0; i<WP_GWSMAX; ++i) gwinpt->wintab[i].ptr = NULL;
/*
***Eller pekm�rken.
*/
    gwinpt->pmkant = 0;
/*
***Lagra f�nstret i f�nstertabellen.
*/
    wpwtab[*id].typ = TYP_GWIN;
    wpwtab[*id].ptr = (char *)gwinpt;
/*
***Skapa ev. tryckknappar f�r snabbval.
*/
    creqbt(gwinpt);
/*
***Visa resultatet.
*/
    wpwshw(*id);
/*
***Normalisera, dvs. fixa till modellf�nstrets proportioner
***och ber�kna 2D transformationskonstanter.
*/
    wpnrgw(gwinpt);
/*
***Om det �r huvudf�nstret det g�ller meddelar vi �ven
***grapac.
*/
    if ( main ) wpupgp(gwinpt);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpnrgw(
        WPGWIN *gwinpt)

/*      Normaliserar proportionerna f�r GWIN-f�nstrets
 *      modellf�nster och ber�knar nya 2D-transformations-
 *      konstanter.
 *
 *      In: gwinpt => Pekare till f�nster.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 29/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   double mdx,mdy,gadx,gady,mprop,gprop;

/*
***Hur stor �r f�nstrets grafiska area.
*/
   gadx = gwinpt->geo.psiz_x *
        (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   gady = gwinpt->geo.psiz_y *
        (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Hur stort �r modellf�nstret i millimeter.
*/
   mdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
   mdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin;
/*
***F�rh�llandet mellan grafiska areans h�jd och bredd �r gady/gadx.
***Se till att modellf�nstret f�r samma f�rh�llande s� att cirklar
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
***Ber�kna nya transformationskonstanter.
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

        bool wpxpgw(
        WPGWIN       *gwinpt,
        XExposeEvent *expev)

/*      Expose-rutin f�r WPGWIN.
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
***F�rst expose p� alla sub-f�nster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          wpxpbu((WPBUTT *)subptr);
          break;

          case TYP_ICON:
          wpxpic((WPICON *)subptr);
          break;
          }
        }
      }
/*
***�ven "meddelanderaden" �r ett subf�nster.
*/
   if ( gwinpt->mesbpt != NULL ) wpxpbu(gwinpt->mesbpt);
/*
***Kopiera fr�n 'save under'-pixmappen.
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

        bool wpcrgw(
        WPGWIN         *gwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin f�r WPGWIN med vidh�ngande sub-f�nster.
 *      Kollar om Leave/Enter skett i n�got av WPGWIN-f�nstrets
 *      subf�nster och servar is�fall eventet.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *          croev  = X-cro event.
 *
 *      Ut: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
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
***G� igenom alla sub-f�nster.
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
            if ( croev->type == EnterNotify ) wpcrbu(butptr,TRUE);
            else wpcrbu(butptr,FALSE);
            return(TRUE);
            }
          else if ( croev->type == EnterNotify  &&  butptr->hlight == TRUE )
            wpcrbu(butptr,FALSE);
          break;

          case TYP_ICON:
          icoptr = (WPICON *) subptr;
          if ( croev->window == icoptr->id.x_id )
            {
            wpcric(icoptr,croev);
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

        bool  wpbtgw(
        WPGWIN       *gwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin f�r WPGWIN med vidh�ngande sub-f�nster.
 *      Kollar om muspekning skett i n�got av WPGWIN-f�nstrets
 *      subf�nster och servar is�fall eventet.
 *
 *      In: iwinptr = C-pekare till WPGWIN.
 *          butev   = X-but event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID f�r subf�nster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
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
***WPGWIN sj�lvt kan inte generera ButtonEvent:s,
***bara sub-f�nstren.
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
***Tr�ff kanske ?
*/
        if ( hit )
          {
/*
***Om s� tar vi s�rskilt hand om vissa snabbval h�r.
*/
          if ( acttyp == FUNC )
            {
            oldfun = actfun;
            actfun = actnum;

            switch ( actnum )
              {
              case 188:
              wpscle(gwinpt,x,y);
              break;

              case 189:
              wpcent(gwinpt);
              break;

              case 190:
              if ( v3mode == BAS3_MOD ) wpperp(gwinpt,x,y);
              else XBell(xdisp,100);
              break;

              case 191:
              wplstv(gwinpt);
              break;

              case 193:
              wpzoom(gwinpt);
              break;

              case 194:
              wpiaut(gwinpt);
              break;

              case 195:
              wpchvi(gwinpt,x,y);
              break;

              case 197:
              wpnivs(gwinpt,x,y);
              break;

              case 198:
              wpshad((int)gwinpt->id.w_id,FALSE);
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

        bool wprpgw(
        WPGWIN *gwinpt,
        XReparentEvent *repev)

/*      Reparent-rutin f�r WPGWIN.
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
***S�tt reparent-flagga.
*/
   gwinpt->reprnt = TRUE;

   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool wpcogw(
        WPGWIN *gwinpt,
        XConfigureEvent *conev)

/*      Configure-rutin f�r WPGWIN.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *          conev  = C-pekare till ConfigureEvent.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE.
 *
 *      (C)microform ab 8/2/94 J. Kjellander
 *
 *      8/1-94   Multif�nster, J. Kjellander
 *
 ******************************************************!*/

  {
    int    oldx,oldy,olddx,olddy,newx,newy,newdx,newdy,
           dx,dy,ddx,ddy;
    double oldmdx,oldmdy,sizx,sizy;
    bool   right,left,up,down;
    Window child;

/*
***F�nstrets gamla placering och storlek.
*/
    oldx  = gwinpt->geo.x;  oldy  = gwinpt->geo.y;
    olddx = gwinpt->geo.dx; olddy = gwinpt->geo.dy;
/*
***Var befinner sig f�nstret nu ? Enda s�ttet att s�kert avg�ra
***var ett f�nster befinner sig �r med hj�lp av XTranslate...
*/ 
    XTranslateCoordinates(xdisp,gwinpt->id.x_id,
                      DefaultRootWindow(xdisp),0,0,&newx,&newy,&child);
/*
***Hur stort �r det nu ?
*/
    newdx = conev->width;
    newdy = conev->height;
/*
***Hur mycket har det �ndrats ?
*/
    dx  = newx -  oldx;  dy  = newy -  oldy;
    ddx = newdx - olddx; ddy = newdy - olddy;
/*
***F�nstret kan: 1 - Ha flyttats.
***              2 - Ha �ndrat storlek utan att flyttas
***              3 - B�da om man tar �vre och/eller v�nstra kanten.
***Vilket �r det som g�ller ?
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
***Om f�nstret �nnu inte har "reparentiserats" av WM
***kan en flyttning av f�nstret inte bero p� anv�ndaren
***av systemet utan m�ste bero p� WM:s garnering av
***f�nstret med egna ramar etc. Is�fall sparar vi p�
***oss denna f�rflyttning s� att vi vet hur stor den �r.
*/
    if ( !gwinpt->reprnt )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      }
/*
***Om ovanst�ende garnering �nnu inte skett, dvs. (wmandx,wmandy) = 0
***men f�nstret �nd� "reparentiserats" har WM valt att g�ra saker i
***en annan ordning �n normalt. Is�fall tolkar vi denna f�rsta f�rflyttning
***som garnering i alla fall.
*/
    else if ( gwinpt->wmandx == 0  &&  gwinpt->wmandy == 0 )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      }
/*
***Lagra den nya f�nstergeometrin i WPGWIN-posten.
*/
    gwinpt->geo.x  = newx;
    gwinpt->geo.y  = newy;
    gwinpt->geo.dx = newdx;
    gwinpt->geo.dy = newdy;
    gwinpt->geo.bw = conev->border_width;
/*
***Vad �r det som har h�nt ?
***Om f�nstret har �ndrat storlek ber�knar vi nytt modell-
***f�nster, viewport mm. s� att bilden efter automatisk repaint
***ligger kvar p� samma st�lle som f�rut �ven om f�nstrets origo
***pga. �ndringen har flyttats.
*/
    if ( ( ddx != 0.0 ) || ( ddy != 0.0 ) )
      {
/*
***Under all omst�ndigheter skall grafiska arean �ndra storlek.
***Detta g�r vi genom att flytta origo till det nya nedre v�nstra
***h�rnet och justera xmax och ymax d�refter oavsett om det �r
***h�gra eller v�nstra alt. �vre eller nedre kanten som har �ndrats.
*/
      gwinpt->vy.scrwin.xmax += ddx;
      gwinpt->vy.scrwin.ymax += ddy;
/*
***Hur blir det med modellf�nstret ? H�r justerar vi den kant som
***verkligen har �ndrats s� att bilden ligger still p� sk�rmen.
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
***Om det �r huvudf�nstret det g�ller meddelar vi �ven grapac.
*/
      if ( gwinpt->id.w_id == GWIN_MAIN )
        {
        wpupgp(gwinpt);
        sizx = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin)/
               (double)DisplayWidth(xdisp,xscr)*XDisplayWidthMM(xdisp,xscr);

        sizy = (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin)/
               (double)DisplayHeight(xdisp,xscr)*XDisplayHeightMM(xdisp,xscr);
        gpsgsz(sizx,sizy);
        }
/*
***Rita om f�nstret.
*/
      wprepa((wpw_id)gwinpt->id.w_id);
/*
***F�reg�ende vy skall nu kunna visas i det nya f�nstret, allts�
***m�ste �ven dennas modellf�nster uppdateras.
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
***Om f�nstret inte har �ndrat storlek beh�ver vi inte g�ra s� mycket.
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

        bool wpcmgw(
        WPGWIN               *gwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen f�r WPGWIN.
 *
 *      In: gwinpt  = C-pekare till WPGWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
 *
 *      (C)microform ab 28/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om det �r WM_DELETE_WINDOW servar vi genom att d�da
***f�nstret ifr�ga s�vida det inte �r huvudf�nstret
***f�rst�s. Detta skall dock inte kunna ta emot ett client-
***message = WM_DELETE_WINDOW.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) &&
        gwinpt->id.w_id > GWIN_MAIN )
     {
     wpwdel((v2int)gwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short wpergw(
        v2int win_id)

/*      Suddar ett grafiskt f�nster med tillh�rande
 *      pixmap f�r save_under.
 *
 *      In: win_id = ID f�r ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = F�nstret %s finns ej.
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
***�nskats.
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
***Ja, sudda sj�lva f�nstret.
*/
           hit = TRUE;
           XClearWindow(xdisp,gwinpt->id.x_id);
/*
***Sudda save_under-pixmappen.
*/
           oldpen = actpen;
           wpspen(0);
           XFillRectangle(xdisp,gwinpt->savmap,gwinpt->win_gc,0,0,
           DisplayWidth(xdisp,xscr),DisplayHeight(xdisp,xscr));
           wpspen(oldpen);
/*
***Om det �r huvudf�nstret skall grapac:s DF suddas.
*/
           if ( gwinpt->id.w_id == GWIN_MAIN ) gperdf();
/*
***�vriga f�nster suddar vi DF:en p� h�r.
*/
           else gwinpt->df_ptr = gwinpt->df_cur = -1;
/*
***Gammal aktiv linjebredd g�ller nu inte l�ngre.
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

        short wprepa(
        v2int win_id)

/*      Riatr om ett grafiskt f�nster med tillh�rande
 *      pixmap f�r save_under.
 *
 *      In: win_id = ID f�r ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = F�nstret %s finns ej.
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
    wpergw(win_id);
/*
***Sl� p� v�ntan.
*/
    wpwait(win_id,TRUE);
/*
***Rita om GM.
*/
    EXdral(win_id);
/*
***Och om det �r huvudf�nstret �ven eventuellt raster.
*/
    if ( win_id == GWIN_MAIN  &&  rstron )
      gpdrrs(rstrox,rstroy,rstrdx,rstrdy);
/*
***Uppdatera aktivt koordinatsustem.
*/
    igupcs(lsysla,V3_CS_ACTIVE);
/*
***Sl� av v�ntan.
*/
    wpwait(win_id,FALSE);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpdlgw(
        WPGWIN *gwinpt)

/*      D�dar ett WPGWIN.
 *
 *      In: gwinptr = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 *      15/3/95  �ven subf�nster, J. Kjellander
 *      1998-01-04 Individuella GC:n, J.Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***D�da eventuella subf�nster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          wpdlbu((WPBUTT *)subptr);
          break;

          case TYP_ICON:
          wpdlic((WPICON *)subptr);
          break;
          }
        }
      }
/*
***�ven "meddelanderaden" �r ett subf�nster.
*/
   if ( gwinpt->mesbpt != NULL ) wpdlbu(gwinpt->mesbpt);
/*
***L�mna tillbaks grafiska f�nstrets "Save under"-pixmapp.
*/
    XFreePixmap(xdisp,gwinpt->savmap);
/*
***L�mna tillbaks f�nstrets GC.
*/
    XFreeGC(xdisp,gwinpt->win_gc);
/*
***Om det �r GWIN_MAIN det g�ller l�mnar vi �ven tillbaks
***�ven gummibands-GC:t som ju delas av alla grafiska f�nster.
*/
    if ( gwinpt->id.w_id == GWIN_MAIN )
      {
      XFreeGC(xdisp,gwinpt->rub_gc);
      }
/*
***H�r borde eventuella grafiska cursors l�mnas tillbaks
***med XFreeCursor(id); Eller kanske i wp1 eftersom flera
***grafiska f�nster kan anv�nda samma cursor.
*/
/*
***Displayfil.
*/
    if ( gwinpt->df_adr != NULL) v3free((char *)gwinpt->df_adr,"wpdlgw");
/*
***L�mna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)gwinpt,"wpdlgw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPGWIN *wpggwp(
        Window   x_id)

/*      Letar upp det grafiska f�nstret med angiven
 *      X-id och returnerar en C-pekare till f�nstret.
 *
 *      In: x_id  = F�nstrets X-id.
 *
 *      Ut: Inget.   
 *
 *      FV: C-adress eller NULL om f�nster saknas.
 *
 *      (C)microform ab 13/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPGWIN  *gwinpt;

/*
***Leta upp f�nstret.
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
***Ingen tr�ff.
*/
    return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static short creqbt(
        WPGWIN *gwinpt)

/*      Skapar grafiska f�nstrets snabbvalsknappar.
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
***Antal subf�nster �r till att b�rja med = 0.
*/
    nsub    = 0;
/*
***Vi b�rjar med textknapparna. varkon.button_n.geometry
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
***Prova att h�mta .geometry-resursen.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry finns. D� provar vi med texten.
***Om inte text finns tar vi knappens nummer ist�llet.
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
***D� skapar vi en tryckknapp.
*/
        status = wpwcbu(gwinpt->id.x_id,
                       (short)x,(short)y,(short)dx,(short)dy,(short)1,
                        butstr,butstr,"",WP_BGND,WP_FGND,&buttpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_BUTTON;
          gwinpt->wintab[nsub].ptr = (char *)buttpt;
/*
***Nu fattas bara aktionskoden. Om s�dan saknas v�ljer vi "f0".
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
***Slutligen r�knar vi upp nsub.
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
***Prova att h�mta .geometry-resursen.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry finns. D� provar vi med filnamnet.
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
***D� skapar vi en ikon.
*/
        status = wpwcic(gwinpt->id.x_id,
                       (short)x,(short)y,(short)1,iconam,
                       WP_BGND,WP_FGND,&iconpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_ICON;
          gwinpt->wintab[nsub].ptr = (char *)iconpt;
/*
***Nu fattas bara aktionskoden. Om s�dan saknas v�ljer vi "f0".
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
***Slutligen r�knar vi upp nsub.
*/
          if ( ++nsub == WP_GWSMAX ) return(0);
          }
       }
/*
***N�sta knapp eller ikon.
*/
    }

    return(0);
  }

/********************************************************/