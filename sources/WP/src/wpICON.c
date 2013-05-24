/**********************************************************************
*
*    wpICON.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPmcic();   Create WPICON, CRE_ICON in MBS
*    WPcrfi();   CreateWPICON, CRE_FICON in MBS
*    WPwcic();   Create WPICON, wpw-version
*    WPxpic();   Expose routine for WPICON
*    WPbtic();   Button routine for WPICON
*    WPcric();   Crossing routine for WPICON
*    WPgtic();   Get routine for WPICON, GET_ICON in MBS
*    WPdlic();   Kills WPICON
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

/*!******************************************************/

        short WPmcic(
        wpw_id  pid,
        short   x,
        short   y,
        short   bw,
        char   *fnam,
        short   cb,
        short   cf,
        DBint  *iid)

/*      Skapar WPICON-fönster och länkar in i ett WPIWIN.
 *
 *      In: pid   = Förälder.
 *          x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          bw    = Ramens bredd.
 *          fnam  = Ikon-fil.
 *          cb    = Bakgrundsfärg.
 *          cf    = Förgrundsfärg.
 *          iid   = Pekare till utdata.
 *
 *      Ut: *iid = Giltigt entry i förälderns wintab.
 *
 *      Felkod: WP1262 = Föräldern %s finns ej.
 *              WP1272 = Föräldern %s är ej ett WPIWIN.
 *              WP1282 = För många subfönster i %s.
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 errbuf[80];
    short                i,status;
    WPWIN               *winptr;
    WPIWIN              *iwinptr;
    WPICON              *icoptr;

/*
***Fixa C-pekare till förälderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1262",errbuf));
      }
/*
***Kolla att det är ett WPIWIN och fixa en pekare till
***förälder-fönstret självt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1272",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID för den nya ikonen, dvs. fixa
***en ledig plats i förälderns fönstertabell.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1282",errbuf));
      }
    else *iid = i;
/*
***Skapa ikonen.
*/
    if ( (status=WPwcic(iwinptr->id.x_id,x,y,bw,fnam,cb,cf,
                                        &icoptr)) < 0 ) return(status);
/*
***Länka in den i WPIWIN-fönstret.
*/
    iwinptr->wintab[*iid].typ = TYP_ICON;
    iwinptr->wintab[*iid].ptr = (char *)icoptr;

    icoptr->id.w_id = *iid;
    icoptr->id.p_id =  pid;
/*
***Om WPIWIN-fönstret redan är mappat skall ikonen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,icoptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPcrfi(
        int    pid,
        short  x,
        short  y,
        char  *filnam,
        char  *akod,
        short  anum,
        DBint *iid)

/*      Skapar snabbvalsikon (CRE_FICON) i grafiskt fönster.
 *
 *      In: pid    = ID för grafiskt fönster.
 *          x,y    = Placering.
 *          filnam = Vägbeskrivning till ikonfil
 *          akod   = Aktionskod.
 *          anum   = Aktionsnummer.
 *          iid    = Pekare till resultat.
 *
 *      Ut: *iid = Ikonens ID.
 *
 *      Felkod: 
 *              WP1552 = %s är en otillåten aktionskod.
 *              WP1522 = Fönstret %s finns ej
 *              WP1532 = Fönstret %s är av fel typ
 *              WP1542 = Fönster %s är fullt
 *
 *      (C)microform ab 1996-05-20 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,action;
    int      i;
    char     errbuf[80];
    WPWIN   *winptr;
    WPGWIN  *gwinpt;
    WPICON  *icoptr;

/*
***Vilken aktionskod ?
*/
    if ( akod[1] != '\0' ) return(erpush("WP1512",akod));

    switch ( akod[0] )
      {
      case 'f': action = CFUNC; break;
      case 'm': action = MENU;  break;
      case 'p': action = PART;  break;
      case 'r': action = RUN;   break;
      case 'M': action = MFUNC; break;
  
      default: return(erpush("WP1552",akod));
      break;
      }
/*
***Fixa C-pekare till det grafiska fönstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1522",errbuf));
      }
/*
***Kolla att det är ett WPGWIN och fixa en pekare till
***förälder-fönstret självt.
*/
    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1532",errbuf));
      }
    else gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skapa ID för den nya knappen, dvs fixa
***en ledig plats i förälderns fönstertabell.
*/
    i = 0;
    while ( i < WP_GWSMAX  &&  gwinpt->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_GWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1542",errbuf));
      }
    else *iid = i;
/*
***Prova att skapa en ikon.
*/
    status = WPwcic(gwinpt->id.x_id,x,y,(short)1,
                        filnam,WP_BGND2,WP_FGND,&icoptr);

    if ( status < 0 ) return(status);
/*
***Länka in den i WPGWIN-fönstret.
*/
    gwinpt->wintab[*iid].typ = TYP_ICON;
    gwinpt->wintab[*iid].ptr = (char *)icoptr;

    icoptr->id.w_id = *iid;
    icoptr->id.p_id =  pid;

    XMapWindow(xdisp,icoptr->id.x_id);
/*
***Aktion.
*/
    icoptr->acttyp = action;
    icoptr->actnum = anum;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    WPwcic(
        Window   px_id,
        short    x,
        short    y,
        short    bw,
        char    *fnam,
        short    cb,
        short    cf,
        WPICON **outptr)

/*      Create WPICON.
 *
 *      In: px_id  = Parent window X-id.
 *          x      = X-position.
 *          y      = Y-position.   
 *          fnam   = Pixmap file.
 *          cb     = Not used.
 *          cf     = Not used.
 *
 *      Ut: *outptr = Ptr to a WPICON.
 *
 *      Felkod: WP1212 = Can't load pixmap file %s
 *              WP1292 = Can't malloc()
 *              WP1712 = XPM error code = %s
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 *      2007-06-17 Xpm, J.Kjellander
 *
 ********************************************************/

  {
    char                 errbuf[V3STRLEN];
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    unsigned int         dx,dy;
    int                  status;
    Window               xwin_id;
    Pixmap               icon_pixmap,icon_mask;
    XGCValues            values;
    XpmAttributes        attributes;
    WPICON              *icoptr;

/*
***Read the xpm-file and create a pixmap. Set attributes to XpmSize so
***that the size of the icon is returned.
***XpmColorError  =  1
***XpmSuccess     =  0
***XpmOpenFailed  = -1
***XpmFileInvalid = -2
***XpmNoMemory    = -3
***XpmColorFailed = -4
*/
    attributes.valuemask = XpmSize;

    status = XpmReadFileToPixmap(xdisp,px_id,fnam,&icon_pixmap,&icon_mask,&attributes);

    if ( status != XpmSuccess )
      {
      sprintf(errbuf,"%d",status);
      erpush("WP1712",errbuf);
      return(erpush("WP1212",fnam));
      }

    dx = attributes.width;
    dy = attributes.height;
/*
***Create the X window.
*/
    xwina.background_pixel  = WPgcol(cb);
    xwina.border_pixel      = WPgcol(WP_BGND2);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    if ( bw > 0 )
      {
      dx += 2*bw + 2;
      dy += 2*bw + 2;
      }

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,
                            DefaultDepth(xdisp,xscr),
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Input events.
*/
    if ( bw > 0 ) XSelectInput(xdisp,xwin_id,ButtonPressMask   |
                                             ButtonReleaseMask |
                                             EnterWindowMask   |
                                             LeaveWindowMask);
/*
***Create a WPICON.
*/
    if ( (icoptr=(WPICON *)v3mall(sizeof(WPICON),"WPwcic")) == NULL )
       return(erpush("WP1292",fnam));

    icoptr->id.w_id = (wpw_id)NULL;
    icoptr->id.p_id = (wpw_id)NULL;
    icoptr->id.x_id = xwin_id;

    icoptr->geo.x =  x;
    icoptr->geo.y =  y;
    icoptr->geo.dx =  (short)dx;
    icoptr->geo.dy =  (short)dy;
    icoptr->geo.bw =  bw;
    icoptr->pixmap = icon_pixmap;
    icoptr->mask   = icon_mask;
    icoptr->tt_str[0] = '\0';
/*
***Set up a private GC for this icon.
*/
    icoptr->gc = XCreateGC(xdisp,icoptr->id.x_id,0,&values);
    XSetClipMask(xdisp,icoptr->gc,icoptr->mask);

   *outptr = icoptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        bool WPxpic(
        WPICON *icoptr)

/*      Expose handler for WPICON.
 *
 *      In: icoptr = C-ptr to WPICON
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 *      2007-05-19 Xpm, J.Kjellander
 *
 ******************************************************!*/

  {
    int dst;

/*
***Compensate position for optional frame.
*/
    if ( icoptr->geo.bw > 0 )
      dst = icoptr->geo.bw + 1;
    else
      dst = 0;
/*
***Copy pixmap to window.
*/
    XSetClipOrigin(xdisp,icoptr->gc,dst,dst);
    XCopyArea(xdisp,icoptr->pixmap,icoptr->id.x_id,icoptr->gc,0,0,icoptr->geo.dx,icoptr->geo.dy,dst,dst);
/*
***Optional 3D-frame.
*/
    if ( icoptr->geo.bw > 0 )
      WPd3db((char *)icoptr,TYP_ICON);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/********************************************************/

        bool WPbtic(
        WPICON *icoptr)

/*      Button-rutin för WPICON.
 *
 *      In: icoptr = C-pekare till WPICON.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 14/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    return(TRUE);
  }

/********************************************************/
/********************************************************/

        bool WPcric(
        WPICON         *icoptr,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPICON.
 *
 *      In: icoptr = C-pekare till WPICON.
 *          croev  = X-crossing event.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 17/1/94 J. Kjellander
 *
 *      2007-05-28 Tooltips, J.Kjellander
 *
 ******************************************************!*/

  {
    int x,y;

/*
***Enter => Highligt border.
*/
    if ( croev->type == EnterNotify )
      {
      XSetWindowBorder(xdisp,croev->window,WPgcol(WP_NOTI));
      if ( icoptr->tt_str[0] != '\0' )
        {
        WPgtmp(&x,&y);
        WPorder_tooltip(x+5,y+10,icoptr->tt_str);
        }
      }
/*
***Leave => Normal border again.
*/
    else                            
      {
      XSetWindowBorder(xdisp,croev->window,WPgcol(WP_BGND2));
      WPclear_tooltip();
      }

    return(TRUE);
  }

/********************************************************/
/********************************************************/

        short WPgtic(
        DBint  iwin_id,
        DBint  butt_id,
        DBint *status)

/*      Get-rutin för WPICON. Används ej för närvarande.
 *
 *      In: iwin_id = Huvudfönstrets id.
 *          butt_id = Button-fönstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1122 = Föräldern %s finns ej.
 *              WP1132 = Föräldern %s ej WPIWIN.
 *              WP1142 = Knappen %s finns ej.
 *              WP1152 = %s är ej en knapp.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    return(0);
  }

/********************************************************/
/********************************************************/

        short WPdlic(
        WPICON *icoptr)

/*      Dödar en WPICON.
 *
 *      In: icotptr = C-pekare till WPICON.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lämna tillbaks pixmappen.
*/
    XFreePixmap(xdisp,icoptr->pixmap);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)icoptr,"WPdlic");

    return(0);
  }

/********************************************************/
