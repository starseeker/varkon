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
*    WPscbu();      Set button color
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
 *      In: pid    = ID f�r grafiskt f�nster.
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
 *              WP1512 = %s �r en otill�ten aktionskod.
 *              WP1482 = F�nstret %s finns ej
 *              WP1492 = F�nstret %s �r av fel typ
 *              WP1502 = F�nster %s �r fullt
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
      case 'f': action = CFUNC; break;
      case 'm': action = MENU;  break;
      case 'p': action = PART;  break;
      case 'r': action = RUN;   break;
      case 'M': action = MFUNC; break;
  
      default: return(erpush("WP1512",akod));
      break;
      }
/*
***Fixa C-pekare till det grafiska f�nstrets entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1482",errbuf));
      }
/*
***Kolla att det �r ett WPGWIN och fixa en pekare till
***f�r�lder-f�nstret sj�lvt.
*/
    if ( winptr->typ != TYP_GWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1492",errbuf));
      }
    else gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skapa ID f�r den nya knappen, dvs fixa
***en ledig plats i f�r�lderns f�nstertabell.
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
                        butstr,butstr,"",WP_BGND2,WP_FGND,&butptr);

    if ( status < 0 ) return(status);
/*
***L�nka in den i WPGWIN-f�nstret.
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

/*      Skapar WPBUTT-f�nster och l�nkar in i ett WPIWIN.
 *      CRE_BUTTON i MBS.
 *
 *      In: pid   = F�r�lder.
 *          x     = L�ge i X-led.
 *          y     = L�ge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          bw    = Border-width.
 *          str1  = Text i l�ge off/FALSE.
 *          str2  = Text i l�ge on/TRUE.
 *          fstr  = Fontnamn eller "" (default).
 *          cb    = Bakgrundsf�rg.
 *          cf    = F�rgrundsf�rg.
 *          bid   = Pekare till utdata.
 *
 *      Ut: *bid = Giltigt entry i f�r�lderns wintab.
 *
 *      Felkod: WP1072 = F�r�ldern %s finns ej.
 *              WP1082 = F�r�ldern %s �r ej ett WPIWIN.
 *              WP1092 = F�r m�nga subf�nster i %s.
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
***Fixa C-pekare till f�r�lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1072",errbuf));
      }
/*
***Kolla att det �r ett WPIWIN och fixa en pekare till
***f�r�lder-f�nstret sj�lvt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1082",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID f�r den nya knappen, dvs fixa
***en ledig plats i f�r�lderns f�nstertabell.
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
***L�nka in den i WPIWIN-f�nstret.
*/
    iwinptr->wintab[*bid].typ = TYP_BUTTON;
    iwinptr->wintab[*bid].ptr = (char *)butptr;

    butptr->id.w_id = *bid;
    butptr->id.p_id =  pid;
/*
***Om WPIWIN-f�nstret redan �r mappat skall knappen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,butptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short    WPwcbu(
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

/*      Skapar WPBUTT-f�nster.
 *
 *      In: px_id  = F�r�ldra f�nstrets X-id.
 *          x      = L�ge i X-led.
 *          y      = L�ge i Y-led.   
 *          dx     = Storlek i X-led.
 *          dy     = Storlek i Y-led.
 *          bw     = Border-width.
 *          str1   = Text i l�ge off/FALSE.
 *          str2   = Text i l�ge on/TRUE.
 *          fstr   = Fontnamn eller "".
 *          cb     = Bakgrundsf�rg.
 *          cf     = F�rgrundsf�rg.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPBUTT.
 *
 *      Felkod: WP1102 = Fonten %s finns ej.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2006-12-11 Added ButtonReleaseMask, J.Kjellander
 *      2007-03-08 Tooltips, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPBUTT              *butptr;

/*
***Skapa f�nstret i X.
*/
    xwina.background_pixel  = WPgcbu(WPwfpx(px_id),cb);
    xwina.border_pixel      = WPgcbu(WPwfpx(px_id),WP_BGND1);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,CopyFromParent,
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Om knappen har ram skall den ocks� kunna clickas i och
***highligtas.
***Utan ram �r den bara en "label".
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
/*
***Init tooltip text.
*/
    butptr->tt_str[0] = '\0';
/*
***Return ptr to button.
*/
   *outptr = butptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    WPxpbu(
        WPBUTT *butptr)

/*      Expose handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1996-12-12 V�nsterjust. lablar, J.Kjellander
 *      1998-03-27 OpenGL f�r AIX, J.Kjellander
 *
 ******************************************************!*/

  {
    int          x,y;
    char         text[81];
    GC           but_gc;
    WPRWIN      *rwinpt;
    XFontStruct *xfs;

/*
***Vilken text skall f�nstret inneh�lla ?
*/
    if ( butptr->status ) strcpy(text,butptr->stron);
    else                  strcpy(text,butptr->stroff);
/*
***Om det �r en knapp med ram, ber�kna textens l�ge s�
***att den hamnar mitt i f�nstret.
*/
    x = (butptr->geo.dx - WPstrl(text))/2;
    y =  WPftpy(butptr->geo.dy);
/*
***Vilket GC skall anv�ndas ? Om knappen sitter i
***ett WPRWIN m�ste vi anv�nda  dess GC eftersom
***knappen is�fall delar dess visual. Knappar i
***�vriga typer av f�nster kan anv�nda xgc.
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
***Set the button backgrund color and the background for writing.
*/
    XSetWindowBackground(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
    XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.bckgnd));
/*
***Set the forground color for writing and write.
*/
    XSetForeground(xdisp,but_gc,WPgcbu(butptr->id.p_id,butptr->color.forgnd));
    XDrawImageString(xdisp,butptr->id.x_id,but_gc,x,y,text,strlen(text));
/*
***Tills vidare �terst�ller vi aktiv font och
***f�rger till default igen.
*/
    WPsfnt(0);

    if ( butptr->color.bckgnd != WP_BGND2 )
      XSetBackground(xdisp,but_gc,WPgcbu(butptr->id.p_id,WP_BGND2));
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

        void    WPscbu(
        WPBUTT *butptr,
        int     color)

/*      Sets the background color of a button.
 *
 *      In: buttptr = C-ptr to WPBUTT.
 *          color   = Color number.
 *
 *      (C)2007-03-24 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Set the button backgrund color and force an expose
***to make the new color visible.
*/
   butptr->color.bckgnd = color;

   XUnmapWindow(xdisp,butptr->id.x_id);

   XSetWindowBackground(xdisp,butptr->id.x_id,
                        WPgcbu(butptr->id.p_id,butptr->color.bckgnd));

   XMapWindow(xdisp,butptr->id.x_id);
  }

/********************************************************/
/*!******************************************************/

        bool WPbtbu(
        WPBUTT *butptr)

/*      Button handler for WPBUTT.
 *
 *      In: buttptr = C ptr to WPBUTT.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Toggle the button status.
*/
    if ( butptr->status == FALSE )
      {
      butptr->status = TRUE;
/*      WPscbu(butptr,WP_BGND3); */
      }
    else
      {
      butptr->status = FALSE;
/*      WPscbu(butptr,WP_BGND2); */
      }
/*
***Erase window and expose again.
*/
    XClearWindow(xdisp,butptr->id.x_id);
    WPxpbu(butptr);
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        bool    WPcrbu(
        WPBUTT *butptr,
        bool    enter)

/*      Crossing handler for WPBUTT.
 *
 *      In: butptr = C ptr to WPBUTT.
 *          enter  = TRUE  => Enter.
 *                   FALSE => Leave.
 *
 *      Return: Always = TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      2007-03-08 Tooltips, J.Kjellander
 *
 ******************************************************!*/

 {
   int x,y;

/*
***Enter => Change color of window border to WP_NOTI.
*/
   if ( enter == TRUE )
     {
     XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_NOTI));
     butptr->hlight = TRUE;
/*
***Order a tooltip in a few seconds if there is one to display.
*/
     if ( butptr->tt_str[0] != '\0' )
       {
       WPgtmp(&x,&y);
       WPorder_tooltip(x+5,y+10,butptr->tt_str);
       }
     }
/*
***Leave => Reset window border color to WP_BGND.
*/
   else                            
     {
     XSetWindowBorder(xdisp,butptr->id.x_id,WPgcbu(butptr->id.p_id,WP_BGND1));
     butptr->hlight = FALSE;
/*
***Remove ordered or active tooltip.
*/
     WPclear_tooltip();
     }

   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

        short WPgtbu(
        DBint  iwin_id,
        DBint  butt_id,
        DBint *status)

/*      Get-rutin f�r WPBUTT.
 *
 *      In: iwin_id = Huvudf�nstrets id.
 *          butt_id = Button-f�nstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1122 = F�r�ldern %s finns ej.
 *              WP1132 = F�r�ldern %s ej WPIWIN.
 *              WP1142 = Knappen %s finns ej.
 *              WP1152 = %s �r ej en knapp.
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
***Fixa C-pekare till f�r�lderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1122",errbuf));
      }
/*
***Kolla att det �r ett WPIWIN.
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
***Kolla om subf�nstret med angivet id finns och �r
***av r�tt typ.
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

/*      D�dar en WPBUTT.
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
***Check for active tooltip and delete if nessecary.
*/
   WPclear_tooltip();
/*
***Release allocated C memory.
*/
   v3free((char *)butptr,"WPdlbu");
   return(0);
  }

/********************************************************/
/*!******************************************************/

 unsigned long WPgcbu(
        wpw_id p_id,
        int    colnum)

/*      Returnerar f�rg f�r WPBUTT. Om f�r�ldern �r
 *      en WPRWIN g�rs s�rskild hantering.
 *
 *      In: p_id   = ID f�r f�r�lder.
 *          colnum = VARKON f�rgnummer.
 *
 *      FV: Pixelv�rde.
 *
 *      (C)microform ab 1998-03-27 J. Kjellander
 *
 *      2007-04-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   WPRWIN *rwinpt;

/*
***Om knappen sitter i ett WPRWIN m�ste vi returnera
***ett pixelv�rde som �r kompatibelt med det f�nstrets
***visual. �vriga f�nster anv�nder default visual.
*/
   switch ( wpwtab[p_id].typ )
     {
     case TYP_RWIN:
     rwinpt   = (WPRWIN *)wpwtab[p_id].ptr;
     switch ( colnum )
       {
       case WP_BGND1: return(rwinpt->bgnd1);
       case WP_BGND2: return(rwinpt->bgnd2);
       case WP_BGND3: return(rwinpt->bgnd3);
       case WP_FGND:  return(rwinpt->fgnd);
       case WP_TOPS:  return(rwinpt->tops);
       case WP_BOTS:  return(rwinpt->bots);
       case WP_NOTI:  return(rwinpt->noti);
       default:       return(0);
       }
     break;

     default:
     return(WPgcol(colnum));
     }
  }

/********************************************************/
