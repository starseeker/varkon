/**********************************************************************
*
*    wpGWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPwcgw();           Create WPGWIN
*    WPnrgw();           Normalize modelwindow
*    WPxpgw();           Expose routine for WPGWIN
*    WPtitle_GWIN();     Update WPGWIN window title
*    WPcrgw();           Crossing routine for WPGWIN
*    WPbtgw();           Button routine for WPGWIN
*    WPrpgw();           Reparent routine for WPGWIN
*    WPcogw();           Configure routine for WPGWIN
*    WPcmgw();           Client message routine for WPGWIN
*    WPergw();           Erase one or more WPGWIN
*    WPrepaint_GWIN();   Repaint WPGWIN
*    WPdlgw();           Kill one WPGWIN
*   *WPggwp();           Map X-id to WPGWIN-C-pointer
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

extern char   jobnam[],jobdir[];
extern int    actpen,actfunc,sysmode;
extern bool   rstron;
extern DBptr  lsysla;
extern V3MDAT sydata;

#define MCWIN_DY 45

/*
***Prototypes for internal functions.
*/
static void cre_toolbar(WPGWIN *gwinpt);

/*!******************************************************/

        short WPwcgw(
        short   x,
        short   y,
        short   dx,
        short   dy,
        char   *label,
        bool    main,
        DBint  *id)

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
 *      2007-05-05 WPMCWIN, J.Kjellander
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
    else if ( (*id=WPwffi()) < 0 ) return(erpush("WP1052",label));
/*
***S�tt f�rg mm.
*/
    xwina.background_pixel  = WPgcol(0);
    xwina.border_pixel      = WPgcol(1);
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
***V3 f�r ej d�das av en WINDOW-Manager som tex. Motif.
*/
    WPsdpr(xwin_id);
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
      XSetBackground(xdisp,Rub_GC,WPgcol(0));
      XSetForeground(xdisp,Rub_GC,WPgcol(3));
      }
/*
***Om det inte �r huvudf�nstret det �r fr�gan om utan ytterligare
***grafiska f�nster skall dessa dela Rub_GC med huvudf�nstret.
*/
    else
      {
      winptr = WPwgwp((wpw_id)GWIN_MAIN);
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

    gwinpt->geo.x  =  x;
    gwinpt->geo.y  =  y;
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
/*
***Create a default full 2D view with name "xy".
*/
    strcpy(gwinpt->vy.name,"xy");
/*
***Hur stort skall sk�rmf�nstret vara ? Skall det ha marginaler
***f�r tryckknappar ?
*/
    gwinpt->vy.scrwin.xmin = 0;
    gwinpt->vy.scrwin.ymin = MCWIN_DY;
    gwinpt->vy.scrwin.xmax = dx;
    gwinpt->vy.scrwin.ymax = dy;

    if ( XrmGetResource(xresDB,"varkon.margin_up",
                        "Varkon.Margin_up",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.ymax -= margin;
/*
    if ( XrmGetResource(xresDB,"varkon.margin_down",
                        "Varkon.Margin_down",type,&value) &&
         sscanf(value.addr,"%d",&margin) == 1 )
      gwinpt->vy.scrwin.ymin += margin;
*/
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

    gwinpt->vy.pdist = 0.0;

    gwinpt->vy.matrix.k11 = 1.0;
    gwinpt->vy.matrix.k12 = 0.0;
    gwinpt->vy.matrix.k13 = 0.0;
    gwinpt->vy.matrix.k21 = 0.0;
    gwinpt->vy.matrix.k22 = 1.0;
    gwinpt->vy.matrix.k23 = 0.0;
    gwinpt->vy.matrix.k31 = 0.0;
    gwinpt->vy.matrix.k32 = 0.0;
    gwinpt->vy.matrix.k33 = 1.0;
/*
***Denna vy �r fullst�ndig och kan anv�ndas, valid = TRUE men
***den har inget namn. F�reg�ende vy finns �nnu ej.
*/
    gwinpt->vy.status     = VIEW_3D_AND_2D;
    gwinpt->old_vy.status = VIEW_NOT_USED;
/*
***Init Display File. Memory will be allocated later.
*/
    gwinpt->df_adr =  NULL;
    gwinpt->df_all =  0;
    gwinpt->df_ptr = -1;
    gwinpt->df_cur = -1;
/*
***Init levels. All unblanked.
*/
    for ( i=0; i<WP_NTSIZ; ++i ) gwinpt->nivtab[i] = 255;
/*
***Init the table of child windows. No child windows yet.
*/
    for ( i=0; i<WP_GWSMAX; ++i) gwinpt->wintab[i].ptr = NULL;
/*
***Or pointer marks.
*/
    gwinpt->pmkant = 0;
/*
***Default grid.
*/
    gwinpt->grid_on = FALSE;
    gwinpt->grid_x  = 0.0;
    gwinpt->grid_y  = 0.0;
    gwinpt->grid_dx = 10.0;
    gwinpt->grid_dy = 10.0;
/*
***Enter the WPGWIN into the global window table.
*/
    wpwtab[*id].typ = TYP_GWIN;
    wpwtab[*id].ptr = (char *)gwinpt;
/*
***Create the Message and Command window before the toolbar.
***Important.
*/
    if ( main ) WPcreate_mcwin(*id,MCWIN_DY);
    else        WPcreate_mcwin(*id,1);
/*
***Create the toolbar. Any errors are reported by cre_toolbar().
*/
    cre_toolbar(gwinpt);
/*
***Show (map) the result.
*/
    WPwshw(*id);
/*
***Normalize, ie. fix model window proportions
***and calculate 2D transformation constants.
*/
    WPnrgw(gwinpt);
/*
***Update window border.
*/
    WPtitle_GWIN(gwinpt);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPnrgw(
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

        bool          WPxpgw(
        WPGWIN       *gwinpt,
        XExposeEvent *expev)

/*      Expose handler for WPGWIN.
 *
 *      In: gwinpt = C-ptr to WPGWIN.
 *          expev  = C-ptr to Expose event
 *
 *      (C)microform ab 23/1/94 J. Kjellander
 *
 *     2007-05-02 WPMCWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    char   *subptr;

/*
***First expose on all buttons/icons in the toolbar.
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
***Then the Message and Command window.
*/
   WPexpose_mcwin(gwinpt->mcw_ptr);
/*
***Finally the graphics, copy from 'save under'-pixmap.
*/
    XCopyArea(xdisp,gwinpt->savmap,gwinpt->id.x_id,gwinpt->win_gc,
                expev->x,expev->y,expev->width,expev->height,
                expev->x,expev->y);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*********************************************************/

       short   WPtitle_GWIN(
       WPGWIN *gwinpt)

/*     Update the title text of a WPGWIN window border.
 *
 *     In: gwinpt => C ptr to WPGWIN
 *                   or NULL for GWIN_MAIN.
 *
 *     (C)2008-02-10 J.Kjellander
 *
 *******************************************************!*/

 {
   char    title[V3STRLEN+1],tmpbuf[V3STRLEN+1];
   WPGWIN *grawin;
   WPWIN  *winptr;
/*
***Init.
*/
   title[0] = '\0';
/*
***If NULL was passed, get a C ptre to GWIN_MAIN.
*/
   if ( gwinpt == NULL )
     {
     if ( (winptr=WPwgwp((wpw_id)GWIN_MAIN)) != NULL  &&
         winptr->typ == TYP_GWIN ) grawin = (WPGWIN *)winptr->ptr;
     else return(0);
     }
   else grawin = gwinpt;
/*
***All windows either have a custom title or the defult title.
*/
   if ( !WPgrst("varkon.title",title) )
     {
     sprintf(title,"VARKON %d.%d%c",sydata.vernr,sydata.revnr,
                                    sydata.level);
     }
/*
***They can also have have jobdir and/or jobname.
*/
   strcat(title, " ");

   if ( WPgrst("varkon.title.jobdir",tmpbuf) && strcmp(tmpbuf,"True") == 0 )
     {
     strcat(title,jobdir);
     }

   if ( WPgrst("varkon.title.jobname",tmpbuf) && strcmp(tmpbuf,"True") == 0 )
     {
     strcat(title,jobnam);
     }
/*
***And finally also view name.
*/
   if ( WPgrst("varkon.title.viewname",tmpbuf) &&
        strcmp(tmpbuf,"True") == 0 )
       {
       if ( grawin->vy.name[0] != '\0' )
         {
         strcat(title," - ");
         strcat(title,grawin->vy.name);
         }
       }
/*
***Update the window border.
*/
   XStoreName(xdisp,grawin->id.x_id,title);
/*
***The end.
*/
   return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool            WPcrgw(
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
***Check the toolbar.
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
/*
***No hit, return false.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool          WPbtgw(
        WPGWIN       *gwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button handler for WPGWIN with children.
 *
 *      In: iwinptr = C ptr to WPGWIN.
 *          butev   = C ptr to X-button event.
 *
 *      Out: *serv_id = ID of child that served the event.
 *
 *      Return: TRUE  = Event served.
 *              FALSE = Event not served.
 *
 *      (C)2008 J. Kjellander
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
***Check all icon/button subwindows for hit.
*/
    hit = FALSE;

    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
/*
***A button.
*/
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
/*
***An icon.
*/
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
***Hit ?
*/
        if ( hit )
          {
/*
***Yes, some shortcuts are handled here. Clear possible
***tooltip first.
*/
          WPclear_tooltip();

          if ( acttyp == CFUNC )
            {
            oldfun  = actfunc;
            actfunc = actnum;

            switch ( actnum )
              {
              case 102:
              WPview_dialog(gwinpt->id.w_id);
              break;

              case 103:
              WPgrid_dialog(gwinpt->id.w_id);
              break;

              case 150:
              WPprint_dialogue(gwinpt->id.w_id);
              break;

              case 188:
              WPscle(gwinpt,x,y);
              break;

              case 189:
              WPcent(gwinpt);
              break;

              case 190:
              WPperp(gwinpt,x,y);
              break;

              case 191:
              WPlstv(gwinpt);
              break;

              case 193:
              WPzoom();
              break;

              case 194:
              WPiaut(gwinpt,NULL,TYP_GWIN);
              break;

              case 197:
              WPlevel_dialog(gwinpt->id.w_id);
              break;

              default:
              actfunc = oldfun;
              if ( IGdofu(acttyp,actnum) == EXIT ) IGexit(); 
              break;
              }
            actfunc = oldfun;
            return(TRUE);
            }
          else
            {
            if ( IGdofu(acttyp,actnum) == EXIT ) IGexit(); 
            else return(TRUE);
            }
          }
        }
      }
/*
***It could also be a rightclick in the Message and Command window...
*/
   if ( butev->window == gwinpt->mcw_ptr->messcom_xid )
     return(WPbutton_mcwin(gwinpt->mcw_ptr,butev));
/*
***...or a resize of the Message and Command window...
*/
   if ( butev->window == gwinpt->mcw_ptr->resize_xid )
     return(WPbutton_mcwin(gwinpt->mcw_ptr,butev));
/*
***...or a mouse click in the command edit.
*/
   if ( butev->window == gwinpt->mcw_ptr->cmdptr->id.x_id )
     return(WPbted(gwinpt->mcw_ptr->cmdptr,butev));
/*
***The end and no hit.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPrpgw(
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

        bool WPcogw(
        WPGWIN *gwinpt,
        XConfigureEvent *conev)

/*      Configure-routine for WPGWIN.
 *
 *      In: gwinpt = C-ptr to WPGWIN.
 *          conev  = C-ptr to ConfigureEvent.
 *
 *      Return: TRUE.
 *
 *      (C)microform ab 8/2/94 J. Kjellander
 *
 *      1994-01-08 Multif�nster, J. Kjellander
 *      2006-11-27 Pending events, Johan Kjellander
 *      2007-05-06 WPMCWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int    oldx,oldy,olddx,olddy,newx,newy,newdx,newdy,
           dx,dy,ddx,ddy;
    double oldmdx,oldmdy;
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
***What is the new size and position ?
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
***If the window is not yet "reparentisized" by the WM
***the event is probably not caused by the user but rather
***by the fact that the WM has dressed the window with it's
***own borders etc.
***If that is the case, we save the move so we know
***how big it is and set the reprnt flag = TRUE. Some WM's
***may not reparentisize at all, KDE for example.
*/
    if ( !gwinpt->reprnt )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      gwinpt->reprnt = TRUE;
      }
/*
***If dressing has not yet been done (wmandx,wmandy) = 0 but the
***window has been reparented the WM is doing things in another order.
***In that case we treat the first move as dressing anyway.
*/
    else if ( gwinpt->wmandx == 0  &&  gwinpt->wmandy == 0 )
      {
      gwinpt->wmandx += dx;
      gwinpt->wmandy += dy;
      }
/*
***Save new window geometry with WPGWIN.
*/
    gwinpt->geo.x  = newx;
    gwinpt->geo.y  = newy;
    gwinpt->geo.dx = newdx;
    gwinpt->geo.dy = newdy;
    gwinpt->geo.bw = conev->border_width;
/*
***If the window changed size set up a new model window and viewport
***so that the image after repaint stays in the same position with
***the same scale.
*/
    if ( ( ddx != 0.0 ) || ( ddy != 0.0 ) )
      {
/*
***Under all circomstances the graphical area changed size.
*/
      gwinpt->vy.scrwin.xmax += ddx;
      gwinpt->vy.scrwin.ymax += ddy;
/*
***Update the position of the Message and Command window.
*/
      gwinpt->mcw_ptr->geo.y  = gwinpt->geo.dy - gwinpt->mcw_ptr->geo.dy;
      gwinpt->mcw_ptr->geo.dx = gwinpt->geo.dx;

      XMoveResizeWindow(xdisp,gwinpt->mcw_ptr->messcom_xid,gwinpt->mcw_ptr->geo.x,
                                                            gwinpt->mcw_ptr->geo.y,
                                                            gwinpt->mcw_ptr->geo.dx,
                                                            gwinpt->mcw_ptr->geo.dy);

      XMoveResizeWindow(xdisp,gwinpt->mcw_ptr->resize_xid,gwinpt->mcw_ptr->geo.x,
                                                           gwinpt->mcw_ptr->geo.y-5,
                                                           gwinpt->mcw_ptr->geo.dx,
                                                           5);

      gwinpt->mcw_ptr->cmdptr->geo.x = gwinpt->mcw_ptr->geo.dx -
                                       gwinpt->mcw_ptr->cmdptr->geo.dx - 5;
      gwinpt->mcw_ptr->cmdptr->geo.y = gwinpt->mcw_ptr->geo.dy -
                                       gwinpt->mcw_ptr->cmdptr->geo.dy - 5;

      XMoveResizeWindow(xdisp,gwinpt->mcw_ptr->cmdptr->id.x_id,
                              gwinpt->mcw_ptr->cmdptr->geo.x,
                              gwinpt->mcw_ptr->cmdptr->geo.y,
                              gwinpt->mcw_ptr->cmdptr->geo.dx,
                              gwinpt->mcw_ptr->cmdptr->geo.dy);
/*
***Update the model window so that the image stays in the same position.
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
***New 2D transformation constants.
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
***Repaint.
*/
      WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);
/*
***Update previous view.
*/
      if ( gwinpt->old_vy.status != VIEW_NOT_USED )
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
***Om the window didn't change size we do nothing.
*/
    else
      {
      ;
      }
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcmgw(
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
     WPwdel((DBint)gwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short WPergw(
        DBint win_id)

/*      Suddar ett grafiskt f�nster med tillh�rande
 *      pixmap f�r save_under och DF mm.
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
           WPspen(0);
           XFillRectangle(xdisp,gwinpt->savmap,gwinpt->win_gc,0,0,
           DisplayWidth(xdisp,xscr),DisplayHeight(xdisp,xscr));
           WPspen(oldpen);
/*
***Sudda f�nstrets DF.
*/
           gwinpt->df_ptr = gwinpt->df_cur = -1;
/*
***Nollst�ll pekm�rken.
*/
           gwinpt->pmkant = 0;
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

        short WPrepaint_GWIN(
        DBint win_id)

/*      Repaint one or all WPGWIN's.
 *
 *      In: win_id = WPGWIN ID or GWIN_ALL
 *
 *      (C)2008-03-15 J. Kjellander
 *
 ******************************************************!*/

  {
   DBCsys csy;

/*
***Erase.
*/
   WPergw(win_id);
/*
***Display grid if on. Do this before displaying
***the model so that the model surley becomes visible
***and not overwritten by the grid.
*/
   WPdraw_grid(win_id);
/*
***Display model.
*/
   EXdral(win_id);
/*
***Display possible active coordinate system.
*/
   if ( lsysla != DBNULL )
     {
     DBread_csys(&csy,NULL,lsysla);
     WPupcs(&csy,lsysla,V3_CS_ACTIVE,GWIN_ALL);
     }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPdlgw(
        WPGWIN *gwinpt)

/*      Kill a WPGWIN. Releases all memory resources
 *      allocated by the WPGWIN.
 *
 *      In: gwinptr = C-ptr to WPGWIN.
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 *      15/3/95  �ven subf�nster, J. Kjellander
 *      1998-01-04 Individuella GC:n, J.Kjellander
 *      2007-05-02 WPMCWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Kill the toolbar buttons/icons.
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
***Kill the Message and Command window.
*/
   WPdelete_mcwin(gwinpt->mcw_ptr);
/*
***Free the "Save under"-pixmapp.
*/
    XFreePixmap(xdisp,gwinpt->savmap);
/*
***Free the GC. 
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
***Display file.
*/
    if ( gwinpt->df_adr != NULL) v3free((char *)gwinpt->df_adr,"WPdlgw");
/*
***Free the window itself.
*/
    v3free((char *)gwinpt,"WPdlgw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPGWIN *WPggwp(
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
***Look up the window in wpwtab[].
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
***No hit.
*/
    return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static void    cre_toolbar(
        WPGWIN *gwinpt)

/*      Creates WPGWIN toolbar.
 *
 *      In: gwinpt = C ptr to WPGWIN.
 *
 *      Error: WP1723 = Can't find icon file %s
 *
 *      (C)microform ab 16/12/94 J. Kjellander
 *
 *      1997-01-15 IGgenv(), J.Kjellander
 *      2007-08-10 1.19, J.Kjellander
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
***Init sub window count.
*/
    nsub    = 0;
/*
***Start with buttons. varkon.button_n.geometry
***                    varkon.button_n.text
***                    varkon.button_n.action
***                    varkon.button_n.tooltip
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
***Try .geometry.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry exists. Try the text. If no text use count instead.
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
***Create the button.
*/
        status = WPwcbu(gwinpt->id.x_id,
                       (short)x,(short)y,(short)dx,(short)dy,(short)1,
                        butstr,butstr,"",WP_BGND2,WP_FGND,&buttpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_BUTTON;
          gwinpt->wintab[nsub].ptr = (char *)buttpt;
          buttpt->type = FUNCBUTTON;
/*
***Action code, default = "f0".
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
        strcpy(xrmstr1,"varkon.button_");
        strcpy(xrmstr2,"Varkon.Button_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".tooltip");
        strcat(xrmstr2,".Tooltip");

        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          {
          strncpy(buttpt->tt_str,value.addr,80);
          buttpt->tt_str[80] = '\0';
          }
        else
          buttpt->tt_str[0] = '\0';
/*
***Increment subwindow count.
*/
          if ( ++nsub == WP_GWSMAX ) return;
          }
        else errmes();
        }
/*
***Now check for icons.
*/
      strcpy(xrmstr1,"varkon.icon_");
      strcpy(xrmstr2,"Varkon.Icon_");
      sprintf(numstr,"%d",i);
      strcat(xrmstr1,numstr);
      strcat(xrmstr2,numstr);
      strcat(xrmstr1,".geometry");
      strcat(xrmstr2,".Geometry");
/*
***Try to get the .geometry resource.
*/
      if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
        {
        flags = XParseGeometry((char *)value.addr,&x,&y,&dx,&dy);
        if ( XValue & flags )
          if ( XNegative & flags ) x = gwinpt->geo.dx + x - dx;
        if ( YValue & flags )
          if ( YNegative & flags ) y = gwinpt->geo.dy + y - dy;
/*
***Geometry exists, try the filename.
*/
        strcpy(xrmstr1,"varkon.icon_");
        strcpy(xrmstr2,"Varkon.Icon_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".name");
        strcat(xrmstr2,".Name");
/*
***Filname.
*/
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
        status = WPwcic(gwinpt->id.x_id,
                       (short)x,(short)y,(short)1,iconam,
                       WP_BGND2,WP_FGND,&iconpt);

        if ( status == 0 )
          {
          gwinpt->wintab[nsub].typ = TYP_ICON;
          gwinpt->wintab[nsub].ptr = (char *)iconpt;
/*
***Actioncode, default = "f0".
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
***Optional tooltip.
*/
        strcpy(xrmstr1,"varkon.icon_");
        strcpy(xrmstr2,"Varkon.Icon_");
        sprintf(numstr,"%d",i);
        strcat(xrmstr1,numstr);
        strcat(xrmstr2,numstr);
        strcat(xrmstr1,".tooltip");
        strcat(xrmstr2,".Tooltip");

        if ( XrmGetResource(xresDB,xrmstr1,xrmstr2,type,&value) )
          {
          strncpy(iconpt->tt_str,value.addr,80);
          iconpt->tt_str[80] = '\0';
          }
        else
          iconpt->tt_str[0] = '\0';
/*
***Increment subwindow count.
*/
          if ( ++nsub == WP_GWSMAX ) return;
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
