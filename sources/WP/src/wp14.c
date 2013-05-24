/**********************************************************************
*
*    wp14.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpmcic();   Create WPICON, CRE_ICON in MBS
*    wpcrfi();   CreateWPICON, CRE_FICON in MBS
*    wpwcic();   Create WPICON, wpw-version
*    wpxpic();   Expose routine for WPICON
*    wpbtic();   Button routine for WPICON
*    wpcric();   Crossing routine for WPICON
*    wpgtic();   Get routine for WPICON, GET_ICON in MBS
*    wpdlic();   Kills WPICON
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
#include "../include/WP.h"

/*!******************************************************/

        short wpmcic(
        wpw_id  pid,
        short   x,
        short   y,
        short   bw,
        char   *fnam,
        short   cb,
        short   cf,
        v2int  *iid)

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
    if ( (winptr=wpwgwp(pid)) == NULL )
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
    if ( (status=wpwcic(iwinptr->id.x_id,x,y,bw,fnam,cb,cf,
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

        short wpcrfi(
        int    pid,
        short  x,
        short  y,
        char  *filnam,
        char  *akod,
        short  anum,
        v2int *iid)

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
      case 'f': action = FUNC;  break;
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
    if ( (winptr=wpwgwp(pid)) == NULL )
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
    status = wpwcic(gwinpt->id.x_id,x,y,(short)1,
                        filnam,WP_BGND,WP_FGND,&icoptr);

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

        short wpwcic(
        Window   px_id,
        short    x,
        short    y,
        short    bw,
        char    *fnam,
        short    cb,
        short    cf,
        WPICON **outptr)

/*      Skapar WPICON-fönster.
 *
 *      In: px_id  = Föräldra fönstrets X-id.
 *          x      = Läge i X-led.
 *          y      = Läge i Y-led.   
 *          fnam   = Bitmap-fil.
 *          cb     = Bakgrundsfärg.
 *          cf     = Förgrundsfärg.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPICON.
 *
 *      Felkod: WP1212 = Kan ej läsa bitmapfilen %s
 *              WP1292 = Fel från malloc()
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    unsigned int         dx,dy;
    int                  status,x_hot,y_hot;
    Window               xwin_id;
    Pixmap               bitmap;
    WPICON              *icoptr;

/*
***Prova att läsa bitmap från fil.
*/
    status = XReadBitmapFile(xdisp,px_id,fnam,&dx,&dy,
                                       &bitmap,&x_hot,&y_hot);

    if ( status != BitmapSuccess ) return(erpush("WP1212",fnam));
/*
***Skapa fönstret i X.
*/
    xwina.background_pixel  = wpgcol(cb);
    xwina.border_pixel      = wpgcol(WP_BGND);
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
    if ( bw > 0 ) XSelectInput(xdisp,xwin_id,ButtonPressMask |
                                             EnterWindowMask |
                                             LeaveWindowMask);
/*
***Skapa en WPICON.
*/
    if ( (icoptr=(WPICON *)v3mall(sizeof(WPICON),"wpwcic")) == NULL )
       return(erpush("WP1292",fnam));

    icoptr->id.w_id = (wpw_id)NULL;
    icoptr->id.p_id = (wpw_id)NULL;
    icoptr->id.x_id = xwin_id;

    icoptr->geo.x =  x;
    icoptr->geo.y =  y;
    icoptr->geo.dx =  (short)dx;
    icoptr->geo.dy =  (short)dy;
    icoptr->geo.bw =  bw;

    icoptr->color.bckgnd = cb;
    icoptr->color.forgnd = cf;

    icoptr->bitmap = bitmap;

   *outptr = icoptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool wpxpic(
        WPICON *icoptr)

/*      Expose-rutin för WPICON.
 *
 *      In: icoptr = C-pekare till WPICON.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 13/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    int dst;

/*
***Färger.
*/
    if ( icoptr->color.bckgnd != WP_BGND )
      XSetBackground(xdisp,xgc,wpgcol(icoptr->color.bckgnd));
    if ( icoptr->color.forgnd != WP_FGND )
      XSetForeground(xdisp,xgc,wpgcol(icoptr->color.forgnd));
/*
***Kopiera bitmappen till fönstret. Om fönstret har ram
***kompenserar vi positionen för detta.
*/
    if ( icoptr->geo.bw > 0 )
      dst = icoptr->geo.bw + 1;
    else
      dst = 0;

    XCopyPlane(xdisp,icoptr->bitmap,icoptr->id.x_id,xgc,0,0,
                          icoptr->geo.dx,icoptr->geo.dy,
                          dst,dst,1);
/*
***Tills vidare återställer vi färger till default igen.
*/
    if ( icoptr->color.bckgnd != WP_BGND )
      XSetBackground(xdisp,xgc,wpgcol(WP_BGND));
    if ( icoptr->color.forgnd != WP_FGND )
      XSetForeground(xdisp,xgc,wpgcol(WP_FGND));
/*
***Ev. 3D-ram.
*/
    if ( icoptr->geo.bw > 0 )
      wpd3db((char *)icoptr,TYP_ICON);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool wpbtic(
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
/*!******************************************************/

        bool wpcric(
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
 ******************************************************!*/

  {

/*
***Enter => Highligt, dvs. tjockare ram i annan färg.
*/
    if ( croev->type == EnterNotify )
      {
      XSetWindowBorder(xdisp,croev->window,wpgcol(WP_NOTI));
      }
/*
***Leave => Normal ram igen.
*/
    else                            
      {
      XSetWindowBorder(xdisp,croev->window,wpgcol(WP_BGND));
      }

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short wpgtic(
        v2int  iwin_id,
        v2int  butt_id,
        v2int *status)

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
/*!******************************************************/

        short wpdlic(
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
    XFreePixmap(xdisp,icoptr->bitmap);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)icoptr,"wpdlic");

    return(0);
  }

/********************************************************/
