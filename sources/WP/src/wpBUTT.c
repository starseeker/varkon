/**********************************************************************
*
*    wpBUTT.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPcrfb();      Create WPBUTT, CRE_FBUTTON in MBS
*    WPmcbu();      Create WPBUTT, CRE_BUTTON in MBS
*    WPwcbu();      Create WPBUTT, wpw-version
*    WPxpbu();      Expose routine for WPBUTT
*    WPbtbu();      Button routine for WPBUTT
*    WPcrbu();      Crossing routine for WPBUT
*    WPgtbu();      Get routine for WPBUTT, GET_BUTTON in MBS
*    WPdlbu();      Kills WPBUTT
*    WPgcbu();      Returns button color
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

/*!******************************************************/

        short WPcrfb(
        int    pid,
        short  x,
        short  y,
        short  dx,
        short  dy,
        char  *butstr,
        char  *akod,
        short  anum,
        DBint *bid)

/*      Create button in graphical window.
 *
 *      In: pid    = ID för grafiskt fönster.
 *          x,y    = Placering.
 *          dx,dy  = Storlek.
 *          butstr = Knapptext.
 *          akod   = Aktionskod.
 *          anum   = Aktionsnummer.
 *          bid    = Pekare till resultat.
 *
 *      Ut: *bid = Button ID.
 *
 *      Felkod: 
 *              WP1512 = %s är en otillåten aktionskod.
 *              WP1482 = Fönstret %s finns ej
 *              WP1492 = Fönstret %s är av fel typ
 *              WP1502 = Fönster %s är fullt
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
    WPBUTT  *butptr;

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
  
      default: return(erpush("WP1512",akod));
      break;
      }
/*
***Fixa C-pekare till det grafiska fönstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1482",errbuf));
      }
/*
***Kolla att det är ett WPGWIN och fixa en pekare till
***förälder-fönstret självt.
*/
    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1492",errbuf));
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
      return(erpush("WP1502",errbuf));
      }
    else *bid = i;
/*
***Prova att skapa en tryckknapp.
*/
    status = WPwcbu(gwinpt->id.x_id,x,y,dx,dy,(short)1,
                        butstr,butstr,"",WP_BGND,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***Länka in den i WPGWIN-fönstret.
*/
    gwinpt->wintab[*bid].typ = TYP_BUTTON;
    gwinpt->wintab[*bid].ptr = (char *)butptr;

    butptr->id.w_id = *bid;
    butptr->id.p_id =  pid;

    XMapWindow(xdisp,butptr->id.x_id);
/*
***Aktion.
*/
    butptr->acttyp = action;
    butptr->actnum = anum;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPmcbu(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str1,
        char   *str2,
        char   *fstr,
        short   cb,
        short   cf,
        DBint  *bid)

/*      Skapar WPBUTT-fönster och länkar in i ett WPIWIN.
 *      CRE_BUTTON i MBS.
 *
 *      In: pid   = Förälder.
 *          x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          bw    = Border-width.
 *          str1  = Text i läge off/FALSE.
 *          str2  = Text i läge on/TRUE.
 *          fstr  = Fontnamn eller "" (default).
 *          cb    = Bakgrundsfärg.
 *          cf    = Förgrundsfärg.
 *          bid   = Pekare till utdata.
 *
 *      Ut: *bid = Giltigt entry i förälderns wintab.
 *
 *      Felkod: WP1072 = Föräldern %s finns ej.
 *              WP1082 = Föräldern %s är ej ett WPIWIN.
 *              WP1092 = För många subfönster i %s.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 errbuf[80];
    short                i,status;
    WPWIN               *winptr;
    WPIWIN              *iwinptr;
    WPBUTT              *butptr;

/*
***Fixa C-pekare till förälderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1072",errbuf));
      }
/*
***Kolla att det är ett WPIWIN och fixa en pekare till
***förälder-fönstret självt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1082",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID för den nya knappen, dvs fixa
***en ledig plats i förälderns fönstertabell.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1092",errbuf));
      }
    else *bid = i;
/*
***Skapa knappen.
*/
    if ( (status=WPwcbu(iwinptr->id.x_id,x,y,dx,dy,bw,
                        str1,str2,fstr,cb,cf,&butptr)) < 0 ) return(status);
/*
***Länka in den i WPIWIN-fönstret.
*/
    iwinptr->wintab[*bid].typ = TYP_BUTTON;
    iwinptr->wintab[*bid].ptr = (char *)butptr;

    butptr->id.w_id = *bid;
    butptr->id.p_id =  pid;
/*
***Om WPIWIN-fönstret redan är mappat skall knappen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,butptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPwcbu(
        Window   px_id,
        short    x,
        short    y,
        short    dx,
        short    dy,
        short    bw,
        char    *str1,
        char    *str2,
        char    *fstr,
        short    cb,
        short    cf,
        WPBUTT **outptr)

/*      Skapar WPBUTT-fönster.
 *
 *      In: px_id  = Föräldra fönstrets X-id.
 *          x      = Läge i X-led.
 *          y      = Läge i Y-led.   
 *          dx     = Storlek i X-led.
 *          dy     = Storlek i Y-led.
 *          bw     = Border-width.
 *          str1   = Text i läge off/FALSE.
 *          str2   = Text i läge on/TRUE.
 *          fstr   = Fontnamn eller "".
 *          cb     = Bakgrundsfärg.
 *          cf     = Förgrundsfärg.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPBUTT.
 *
 *      Felkod: WP1102 = Fonten %s finns ej.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2006-12-11 Added ButtonReleaseMask, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPBUTT              *butptr;

/*
***Skapa fönstret i X.
*/
    xwina.background_pixel  = WPgcbu(WPwfpx(px_id),cb);
    xwina.border_pixel      = WPgcbu(WPwfpx(px_id),WP_BGND);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,CopyFromParent,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Om knappen har ram skall den också kunna clickas i och
***highligtas.
***Utan ram är den bara en "label".
*/
    if ( bw > 0 ) XSelectInput(xdisp,xwin_id,ButtonPressMask | ButtonReleaseMask |
                                             EnterWindowMask | LeaveWindowMask);
/*
***Skapa en WPBUTT.
*/
    if ( (butptr=(WPBUTT *)v3mall(sizeof(WPBUTT),"WPwcbu")) == NULL )
       return(erpush("WP1112",str1));

    butptr->id.w_id = (wpw_id)NULL;
    butptr->id.p_id = (wpw_id)NULL;
    butptr->id.x_id = xwin_id;

    butptr->geo.x =  x;
    butptr->geo.y =  y;
    butptr->geo.dx =  dx;
    butptr->geo.dy =  dy;
    butptr->geo.bw =  bw;

    butptr->color.bckgnd = cb;
    butptr->color.forgnd = cf;

    if ( strlen(str1) > 80 ) str1[80] = '\0';
    strcpy(butptr->stroff,str1);
    if ( strlen(str2) > 80 ) str2[80] = '\0';
    strcpy(butptr->stron,str2);

    butptr->status = FALSE;
    butptr->hlight = FALSE;

    if ( fstr[0] == '\0' ) butptr->font = 0;
    else if ( (butptr->font=WPgfnr(fstr)) < 0 )
                         return(erpush("WP1102",fstr));

   *outptr = butptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPxpbu(
        WPBUTT *butptr)

/*      Expose-rutin för WPBUTT.
 *
 *      In: buttptr = C-pekare till WPBUTT.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1996-12-12 Vänsterjust. lablar, J.Kjellander
 *      1998-03-27 OpenGL för AIX, J.Kjellander
 *
 ******************************************************!*/

  {
    int      x,y;
    char     text[81];
    GC       but_gc;
    WPRWIN  *rwinpt;
    XFontStruct *xfs;         /*********ny********/

/*
***Vilken text skall fönstret innehålla ?
*/
    if ( butptr->status ) strcpy(text,butptr->stron);
    else                  strcpy(text,butptr->stroff);
/*
***Mappa till 8-bitar ASCII.
*/
    WPmaps(text);
/*
***Om det är en knapp med ram, beräkna textens läge så
***att den hamnar mitt i fönstret.
*/
    x = (butptr->geo.dx - WPstrl(text))/2;
    y =  WPftpy(butptr->geo.dy);
/*
***Vilket GC skall användas ? Om knappen sitter i
***ett WPRWIN måste vi använda  dess GC eftersom
***knappen isåfall delar dess visual. Knappar i
***övriga typer av fönster kan använda xgc.
*/
    switch ( wpwtab[butptr->id.p_id].typ )
      {
      case TYP_RWIN:
      rwinpt = (WPRWIN *)wpwtab[butptr->id.p_id].ptr;
      but_gc = rwinpt->win_gc;
      xfs = WPgfnt(0);       /*****ny****/
      XSetFont(xdisp,but_gc,xfs->fid);    /*******ny*******/
      break;

      default:
      but_gc = xgc;
      WPsfnt(butptr->font);       /*****ny****/
      break;
      }
/*
***Sätt färger.
*/
    if ( butptr->color.bckgnd != WP_BGND )
      XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
    if ( butptr->color.forgnd != WP_FGND )
      XSetForeground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.forgnd));
/*
***Skriv ut.
*/
    XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,text,strlen(text));
/*
***Tills vidare återställer vi aktiv font och
***färger till default igen.
*/
    WPsfnt(0);

    if ( butptr->color.bckgnd != WP_BGND )
      XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,WP_BGND));
    if ( butptr->color.forgnd != WP_FGND )
      XSetForeground(xdisp,but_gc,WPgcbu(butptr->id.p_id,WP_FGND));
/*
***Test av 3D-ram.
*/
    if ( butptr->geo.bw > 0 )
      WPd3db((char *)butptr,TYP_BUTTON);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPbtbu(
        WPBUTT *butptr)

/*      Button-rutin för WPBUTT.
 *
 *      In: buttptr = C-pekare till WPBUTT.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Muspekning i button-fönster. Status togglas
***till motsatta värdet.
*/
    if ( butptr->status == 0 ) butptr->status = TRUE;
    else                       butptr->status = FALSE;
/*
***Sudda fönstret och gör expose.
*/
    XClearWindow(xdisp,butptr->id.x_id);
    WPxpbu(butptr);
 
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool WPcrbu(
        WPBUTT *butptr,
        bool    enter)

/*      Crossing-rutin för WPBUTT.
 *
 *      In: iwinpt = C-pekare till WPBUTT.
 *          enter  = TRUE  => Enter
 *                   FALSE => Leave
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Enter => Highligt, dvs. blå ram.
*/
    if ( enter == TRUE )
      {
      XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_NOTI));
      butptr->hlight = TRUE;
      }
/*
***Leave => Ram med samma färg som bakgrunden igen.
*/
    else                            
      {
      XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_BGND));
      butptr->hlight = FALSE;
      }

    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short WPgtbu(
        DBint  iwin_id,
        DBint  butt_id,
        DBint *status)

/*      Get-rutin för WPBUTT.
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
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinptr;
    WPBUTT *buttptr;

/*
***Fixa C-pekare till förälderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1122",errbuf));
      }
/*
***Kolla att det är ett WPIWIN.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1132",errbuf));
      }
/*
***Fixa en C-pekare till WPIWIN.
*/
    iwinptr = (WPIWIN *)winptr->ptr;
/*
***Kolla om subfönstret med angivet id finns och är
***av rätt typ.
*/
    if ( iwinptr->wintab[(wpw_id)butt_id].ptr == NULL )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1142",errbuf));
      }

    if ( iwinptr->wintab[(wpw_id)butt_id].typ != TYP_BUTTON )
      {
      sprintf(errbuf,"%d",(int)butt_id);
      return(erpush("WP1152",errbuf));
      }
/*
***Fixa en C-pekare till WPBUTT.
*/
    buttptr = (WPBUTT *)iwinptr->wintab[(wpw_id)butt_id].ptr;
/*
***Returnera status.
*/
    *status = buttptr->status;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdlbu(
        WPBUTT *butptr)

/*      Dödar en WPBUTT.
 *
 *      In: buttptr = C-pekare till WPBUTT.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)butptr,"WPdlbu");
    return(0);
  }

/********************************************************/
/*!******************************************************/

 unsigned long WPgcbu(
        wpw_id p_id,
        int    colnum)

/*      Returnerar färg för WPBUTT. Om föräldern är
 *      en WPRWIN görs särskild hantering.
 *
 *      In: p_id   = ID för förälder.
 *          colnum = VARKON färgnummer.
 *
 *      FV: Pixelvärde.
 *
 *      (C)microform ab 1998-03-27 J. Kjellander
 *
 ******************************************************!*/

  {
   WPRWIN *rwinpt;

/*
***Om knappen sitter i ett WPRWIN måste vi returnera
***ett pixelvärde som är kompatibelt med det fönstrets
***visual. Övriga fönster använder default visual.
*/
   switch ( wpwtab[p_id].typ )
     {
     case TYP_RWIN:
     rwinpt   = (WPRWIN *)wpwtab[p_id].ptr;
     switch ( colnum )
       {
       case WP_BGND: return(rwinpt->bgnd);
       case WP_FGND: return(rwinpt->fgnd);
       case WP_TOPS: return(rwinpt->tops);
       case WP_BOTS: return(rwinpt->bots);
       case WP_NOTI: return(rwinpt->noti);
       default:      return(0);
       }
     break;

     default:
     return(WPgcol(colnum));
     }
  }

/********************************************************/
