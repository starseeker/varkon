/**********************************************************************
*
*    wpEDIT.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPmced();   Create WPEDIT, MBS-version
*    WPwced();   Create WPEDIT, wpw-version
*    WPxped();   Expose routine for WPEDIT
*    WPbted();   Button routine for WPEDIT
*    WPcred();   Crossing routine for WPEDIT
*    WPkeed();   Key routine for WPEDIT
*    WPgted();   Get routine for WPEDIT, GET_EDIT in MBS
*    WPuped();   Replace text in WPEDIT
*    WPdled();   Kill WPEDIT
*
*   *WPffoc();   Which WPEDIT has focus ?
*    WPfoed();   Focus On/Off for WPEDIT
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

#define XK_MISCELLANY
#define XK_LATIN1
#ifdef UNIX
#include <X11/keysymdef.h>
#endif

extern MNUALT       smbind[];

static short edmapk(XKeyEvent *keyev, short *sym, char *t);

/*!******************************************************/

        short WPmced(
        wpw_id  pid,
        short   x,
        short   y,
        short   dx,
        short   dy,
        short   bw,
        char   *str,
        short   ntkn,
        DBint  *eid)

/*      Create WPEDIT window.
 *
 *      In: pid   = Förälder.
 *          x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          bw    = Border-width.
 *          str   = Text.
 *          ntkn  = Max antal tecken.
 *          eid   = Pekare till utdata.
 *
 *      Ut: *eid = Giltigt entry i förälderns wintab.
 *
 *      Felkod: WP1302 = Föräldern %s finns ej.
 *              WP1312 = Föräldern %s är ej ett WPIWIN.
 *              WP1322 = För många subfönster i %s.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char                 errbuf[80];
    short                i;
    WPWIN               *winptr;
    WPIWIN              *iwinptr;
    WPEDIT              *edtptr;

/*
***Fixa C-pekare till förälderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp(pid)) == NULL )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1302",errbuf));
      }
/*
***Kolla att det är ett WPIWIN och fixa en pekare till
***förälder-fönstret självt.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1312",errbuf));
      }
    else iwinptr = (WPIWIN *)winptr->ptr;
/*
***Skapa ID för  ny edit, dvs fixa
***en ledig plats i förälderns fönstertabell.
*/
    i = 0;
    while ( i < WP_IWSMAX  &&  iwinptr->wintab[i].ptr != NULL ) ++i;

    if ( i == WP_IWSMAX )
      {
      sprintf(errbuf,"%d",(int)pid);
      return(erpush("WP1322",errbuf));
      }

   *eid = i;
/*
***Skapa edit.
*/
    WPwced(iwinptr->id.x_id,x,y,dx,dy,bw,str,ntkn,&edtptr);
/*
***Länka in den i WPIWIN-fönstret.
*/
    iwinptr->wintab[*eid].typ = TYP_EDIT;
    iwinptr->wintab[*eid].ptr = (char *)edtptr;

    edtptr->id.w_id = *eid;
    edtptr->id.p_id =  pid;
/*
***Om WPIWIN-fönstret redan är mappat skall editen mappas nu.
*/
    if ( iwinptr->mapped ) XMapWindow(xdisp,edtptr->id.x_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPwced(
        Window   px_id,
        short    x,
        short    y,
        short    dx,
        short    dy,
        short    bw,
        char    *str,
        short    ntkn,
        WPEDIT **outptr)

/*      Skapar WPEDIT-fönster.
 *
 *      In: px_id  = Föräldra fönstrets X-id.
 *          x      = Läge i X-led.
 *          y      = Läge i Y-led.   
 *          dx     = Storlek i X-led.
 *          dy     = Storlek i Y-led.
 *          bw     = Border-width.
 *          str    = Text.
 *          ntkn   = Max antal tecken.
 *          outptr = Pekare till utdata.
 *
 *      Ut: *outptr = Pekare till WPEDIT.
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      12/4/95  ntkn > V3STRLEN, J. Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    Window               xwin_id;
    WPEDIT              *edtptr;

/*
***Skapa fönstret i X. Ramen samma färg som bakgrunden.
*/
    xwina.border_pixel      = WPgcol(WP_BGND);
    xwina.background_pixel  = WPgcol(WP_BGND);
    xwina.override_redirect = True;
    xwina.save_under        = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  

    xwin_id = XCreateWindow(xdisp,px_id,x,y,dx,dy,bw,
                            DefaultDepth(xdisp,xscr),
                            InputOutput,CopyFromParent,xwinm,&xwina);
/*
***Edit-fönster skall det kunna clickas i. 
*/
    XSelectInput(xdisp,xwin_id,ButtonPressMask);
/*
***Skapa en WPEDIT.
*/
    if ( (edtptr=(WPEDIT *)v3mall(sizeof(WPEDIT),"WPwced"))
                                                  == NULL ) return(-2);

    edtptr->id.w_id = (wpw_id)NULL;
    edtptr->id.p_id = (wpw_id)NULL;
    edtptr->id.x_id = xwin_id;

    edtptr->geo.x =  x;
    edtptr->geo.y =  y;
    edtptr->geo.dx =  dx;
    edtptr->geo.dy =  dy;
    edtptr->geo.bw =  bw;

    if ( ntkn < 0 ) ntkn = 0;
    if ( ntkn > V3STRLEN ) ntkn = V3STRLEN;
    if ( strlen(str) > (unsigned int)ntkn ) str[ntkn] = '\0';
    strcpy(edtptr->str,str);
    strcpy(edtptr->dstr,str);
    edtptr->tknmax = ntkn;

    edtptr->scroll = 0;
    edtptr->curpos = 0;
    edtptr->fuse   = TRUE;
    edtptr->focus  = FALSE;

   *outptr = edtptr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPxped(
        WPEDIT *edtptr)

/*      Expose-rutin för WPEDIT.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *
 *      Ut: Inget.   
 *
 *      Fv: Alltid TRUE.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    int  x,y,tknf;
    char str[V3STRLEN+1];

/*
***Beräkna textens läge.
*/
    x =  WPstrl(" ");
    y =  WPftpy(edtptr->geo.dy);
/*
***Skriv ut den del av texten som syns i fönstret, dvs.
***inte det som är utscrollat. Antal tillgängliga tecken-
***positioner är i princip så många som får plats men vi
***börjar alltid med en tom för sysn skull och reserverar
***dessutom en tom på slutet för cursorn.
*/
    strcpy(str,&edtptr->str[edtptr->scroll]);

    tknf = (edtptr->geo.dx/WPstrl(" ")) - 2;
    if ( tknf > 0 ) str[tknf] = '\0';

    WPwstr(edtptr->id.x_id,x,y,str);
/*
***Om fönstret har focus, även en cursor. Tecken 152 i font 1.
***X-koordinat beräknas med font 0 aktiv men Y-koordinat med
***font 1.
*/
    if ( edtptr->focus  &&  edtptr->curpos >= 0 )
      {
      x = x + (edtptr->curpos - edtptr->scroll)*WPstrl(" ") -1;
      WPsfnt(1);
      y = WPftpy(edtptr->geo.dy);
      str[0] = 152; str[1] = '\0';
      XDrawString(xdisp,edtptr->id.x_id,xgc,x,y,str,1);
      WPsfnt(0);
      }
/*
***Test av 3D-look.
*/
    if ( edtptr->geo.bw > 0 )
      WPd3db((char *)edtptr,TYP_EDIT);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool WPbted(
        WPEDIT       *edtptr,
        XButtonEvent *butev)

/*      Button-rutin för WPEDIT.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          butev  = X-event.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Eventet gäller ej detta fönster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    int      x,tknpos,nvis;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPEDIT  *actptr;

/*
***Om detta fönster inte har input-focus skall det 
***ha det nu ! Det fönster som ev. hade det tidigare
***skal då inte ha focus längre. För att säkert bli av
***med cursorn suddar vi då hela fönstret och ritar om det.
***Eftersom fokus-byte kan returnera TRUE sätts gamla
***fönstrets symbol till SMBNONE så att ingen skall tro
***att det är en tangenttryckning som genererat fokus-bytet.
*/
    if ( edtptr->focus == FALSE )
      {
      winptr = WPwgwp(edtptr->id.p_id);
      iwinpt = (WPIWIN *)winptr->ptr;
      actptr = WPffoc(iwinpt,FOCUS_EDIT);
      if ( actptr != NULL )
        {
        XClearWindow(xdisp,actptr->id.x_id);
        WPfoed(actptr,FALSE);
        actptr->symbol = SMBNONE;
        }
      WPfoed(edtptr,TRUE);
      return(TRUE);
      }
/*
***Om fönstret redan har input-focus handlar det om att
***placera cursorn.
*/
    else
      {
      x = butev->x;
      tknpos = (int)((double)x/(double)WPstrl(" "));
      nvis = strlen(edtptr->str) - edtptr->scroll;
      if ( tknpos > nvis ) tknpos = nvis+1;
      if ( tknpos < 1 ) tknpos = 1;
      edtptr->curpos = tknpos - 1 + edtptr->scroll;
      XClearWindow(xdisp,edtptr->id.x_id);
      WPxped(edtptr);

      return(FALSE);
      }
  }

/********************************************************/
/*!******************************************************/

        bool WPcred(
        WPEDIT         *edtptr,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPEDIT. Anropande rutin har
 *      konstaterat att eventet gäller detta fönster men 
 *      skickar med croev för att vi skall kunna avgöra
 *      om det är enter eller leave.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          croev  = X-crossing event.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Eventet gäller ej detta fönster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***För tillfället utnyttjas inte CrossingEvents i WPEDIT:s.
*/
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool WPkeed(
        WPEDIT    *edtptr,
        XKeyEvent *keyev,
        int        slevel)

/*      Key-rutin för WPEDIT. Servar eventet och returnerar
 *      status TRUE/FALSE beroende på vilken tangent som 
 *      tryckts ned och vilken service-nivå som angetts.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          keyev  = X-key event.
 *          slevel = Service-nivå.
 *
 *          SLEVEL_ALL    => Inga servas lokalt.
 *          SLEVEL_V3_INP => SMBCHAR + SMBBACKSP +
 *                           SMBLEFT + SMBRIGHT  servas lokalt.
 *          SLEVEL_MBS    => Dessutom SMBMAIN, SMBPOSM och SMBHELP
 *          SLEVEL_NONE   => Alla event servas lokalt.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Eventet gäller ej detta fönster.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short    symbol;
    int      tknant,tknwin,i;
    char     tkn;
    bool     servat;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPEDIT  *nxtptr;

/*
***Initiering.
*/
    tknant = strlen(edtptr->str);
    tknwin = edtptr->geo.dx/WPstrl(" ") - 2;
    servat = FALSE;
/*
***Mappa tangenttryckningen till V3-symbol.
***Logga symbolen i editen.
*/
    edmapk(keyev,&symbol,&tkn);
    edtptr->symbol = symbol;
/*
***Vidtag åtgärd..
*/
    switch ( symbol )
      {
      case SMBCHAR:
/*
***Insert, finns det plats för fler tecken ?
***Det finns det alltid om fuse = TRUE !
*/
      if ( !edtptr->fuse && (tknant >= edtptr->tknmax) ) XBell(xdisp,100);
/*
***Ja, kolla om det är första gången ett tecken kommer till detta
***fönster, dvs om fuse = TRUE. Isåfall blir det inte vanlig insert
***utan först suddning av gammal text och sedan insert. Om cursorn
***inte står i pos 0 gäller dock detta inte.
*/
      else
        {          
        if ( edtptr->fuse  &&  edtptr->curpos == 0 )
          {
          *edtptr->str = '\0';
          tknant = 0;
          }
        for ( i=tknant++; i>=edtptr->curpos; i--)
          edtptr->str[i+1] = edtptr->str[i];
        edtptr->str[edtptr->curpos++] = tkn;
        if ( tknant > tknwin ) ++edtptr->scroll;
        if ( edtptr->fuse ) edtptr->fuse = FALSE;
        }
      break;
/*
***Backspace = sudda.
*/
      case SMBBACKSP:
      if ( edtptr->fuse ) edtptr->fuse = FALSE;

      if ( edtptr->curpos > 0 )
        {
        for ( i= --edtptr->curpos; i<tknant; i++)
          edtptr->str[i] = edtptr->str[i+1];
        tknant--;
        }
      else if ( tknant > 0 )
        {
        for ( i=edtptr->curpos; i<tknant; i++)
          edtptr->str[i] = edtptr->str[i+1];
        tknant--;
        }
      else
        {
        strcpy(edtptr->str,edtptr->dstr);
        tknant = strlen(edtptr->str);
        if ( tknant == 0 ) XBell(xdisp,100);
        else edtptr->fuse = TRUE;
        }
      if ( edtptr->curpos < edtptr->scroll )
        edtptr->scroll = edtptr->curpos;
      break;
/*
***Pil till vänster.
*/
      case SMBLEFT:
      if ( edtptr->curpos > 0 )
        {
        --edtptr->curpos;
        if ( edtptr->curpos < edtptr->scroll ) --edtptr->scroll;
        }
      else XBell(xdisp,100);
      break;
/*
***Pil till höger.
*/
      case SMBRIGHT:
      if ( edtptr->curpos < tknant )
        {
        ++edtptr->curpos;
        if ( edtptr->curpos > edtptr->scroll + tknwin ) ++edtptr->scroll;
        }
      else XBell(xdisp,100);
      break;
/*
***Pil upp.
*/
      case SMBUP:
      winptr = WPwgwp(edtptr->id.p_id);
      iwinpt = (WPIWIN *)winptr->ptr;
      nxtptr = WPffoc(iwinpt,PREV_EDIT);
      WPfoed(edtptr,FALSE);
      WPfoed(nxtptr,TRUE);
      if ( slevel == SLEVEL_MBS  ||
           slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
/*
***Pil ner.
*/
      case SMBDOWN:
      case SMBRETURN:
      winptr = WPwgwp(edtptr->id.p_id);
      iwinpt = (WPIWIN *)winptr->ptr;
      nxtptr = WPffoc(iwinpt,NEXT_EDIT);
      WPfoed(edtptr,FALSE);
      WPfoed(nxtptr,TRUE);
      if ( slevel == SLEVEL_MBS  ||
           slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
/*
***Huvudmenyn <TAB>, Help <CTRL>a och Pos-menyn <CTRL>b.
*/
      case SMBMAIN:
      case SMBPOSM:
      case SMBHELP:
      if ( slevel == SLEVEL_V3_INP ) servat = TRUE;
      break;
      }
/*
***Edit-fönstrets text-sträng är nu editerad.
***Sudda fönstret och gör expose.
*/
    XClearWindow(xdisp,edtptr->id.x_id);
    WPxped(edtptr);
/*
***Om begärd service-nivå är att alla key-events skall returneras
***servade (inga servas lokalt) sätter vi servat = TRUE oavsett
***vilken tangent det handlade om.
*/
    if ( slevel == SLEVEL_ALL ) servat = TRUE;
/*
***Slut.
*/
    return(servat);
  }

/********************************************************/
/*!******************************************************/

        short WPgted(
        DBint  iwin_id,
        DBint  edit_id,
        char  *str)

/*      Get-rutin för WPEDIT.
 *
 *      In: iwin_id = Huvudfönstrets id.
 *          edit_id = Edit-fönstrets id.
 *
 *      Ut: str = Aktuell text.
 *
 *      Felkod: WP1162 = Föräldern %s finns ej.
 *              WP1172 = Föräldern %s ej WPIWIN.
 *              WP1182 = Knappen %s finns ej.
 *              WP1192 = %s är ej en knapp.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinptr;
    WPEDIT *editptr;

/*
***Fixa C-pekare till förälderns entry i wpwtab.
*/
    if ( (winptr=WPwgwp((wpw_id)iwin_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1162",errbuf));
      }
/*
***Kolla att det är ett WPIWIN.
*/
    if ( winptr->typ != TYP_IWIN )
      {
      sprintf(errbuf,"%d",(int)iwin_id);
      return(erpush("WP1172",errbuf));
      }
/*
***Fixa en C-pekare till WPIWIN.
*/
    iwinptr = (WPIWIN *)winptr->ptr;
/*
***Kolla om subfönstret med angivet id finns och är
***av rätt typ.
*/
    if ( iwinptr->wintab[(wpw_id)edit_id].ptr == NULL )
      {
      sprintf(errbuf,"%d",(int)edit_id);
      return(erpush("WP1182",errbuf));
      }

    if ( iwinptr->wintab[(wpw_id)edit_id].typ != TYP_EDIT )
      {
      sprintf(errbuf,"%d",(int)edit_id);
      return(erpush("WP1192",errbuf));
      }
/*
***Fixa en C-pekare till WPEDIT.
*/
    editptr = (WPEDIT *)iwinptr->wintab[(wpw_id)edit_id].ptr;
/*
***Returnera sträng.
*/
    strcpy(str,editptr->str);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPuped(
        WPEDIT *edtptr,
        char   *newstr)

/*      Byter ut texten i ett WPEDIT-fönster.
 *
 *      In: edtptr = Pekare till WPEDIT
 *          newstr = Ny text
 *
 *      Ut: Inget.
 *
 *      Felkod: .
 *
 *      (C)microform ab 1996-12-12 J. Kjellander
 *
 ******************************************************!*/

  {
   int ntkn;

/*
***Kolla stränglängden.
*/
    ntkn = strlen(newstr);
    if ( ntkn < 0 ) ntkn = 0;
    if ( ntkn > V3STRLEN ) ntkn = V3STRLEN;
    if ( ntkn > edtptr->tknmax ) ntkn = edtptr->tknmax;
    newstr[ntkn] = '\0';
/*
***Lagra den nya texten i editen.
*/
    strcpy(edtptr->str,newstr);

    edtptr->scroll = 0;
    edtptr->curpos = 0;
    edtptr->fuse   = TRUE;
/*
***Uppdatera skärmen.
*/
    WPxped(edtptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPdled(
        WPEDIT *edtptr)

/*      Dödar en WPEDIT.
 *
 *      In: edttptr = C-pekare till WPEDIT.
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
    v3free((char *)edtptr,"WPdled");
    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPEDIT *WPffoc(
        WPIWIN *iwinptr,
        int     code)

/*      Letar efter WPEDIT:s i ett WPIWIN och returnerar
 *      en pekare till det som efterfrågats eller NULL om
 *      det inte finns.
 *
 *      In: iwinptr = C-pekare till WPIWIN.
 *          code    = FIRST_EDIT  => Första i wintab oavsett fokus.
 *                  = NEXT_EDIT   => 1:a efter den som har fokus
 *                  = PREV_EDIT   => 1:a före den som har fokus
 *                  = FOCUS_EDIT  => Den som har fokus.
 *
 *      Ut: Inget.   
 *
 *      FV: Pekare till WPEDIT eller NULL.
 *
 *      (C)microform ab 14/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    char   *subptr;
    wpw_id  foc_id;
    WPEDIT *edtptr;

/*
***Leta igenom alla sub-fönster efter WPEDIT.
*/
    switch ( code )
      {
/*
***Första.
*/
      case FIRST_EDIT:
      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          return((WPEDIT *)subptr);
        }
      return(NULL);
/*
***Sista.
*/
      case LAST_EDIT:
      for ( i=WP_IWSMAX-1; i>=0; --i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          return((WPEDIT *)subptr);
        }
      return(NULL);
/*
***Nästa.
*/
      case NEXT_EDIT:
      edtptr = WPffoc(iwinptr,FOCUS_EDIT);
      if ( edtptr == NULL ) return(NULL);
      else foc_id = edtptr->id.w_id;

      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->id.w_id > foc_id ) return(edtptr);
          }
        }
      edtptr = WPffoc(iwinptr,FIRST_EDIT);
      return(edtptr);
/*
***Föregående.
*/
      case PREV_EDIT:
      edtptr = WPffoc(iwinptr,FOCUS_EDIT);
      if ( edtptr == NULL ) return(NULL);
      else foc_id = edtptr->id.w_id;

      for ( i=WP_IWSMAX-1; i>=0; --i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->id.w_id < foc_id ) return(edtptr);
          }
        }
      edtptr = WPffoc(iwinptr,LAST_EDIT);
      return(edtptr);
/*
***Aktivt.
*/
      case FOCUS_EDIT:
      for ( i=0; i<WP_IWSMAX; ++i )
        {
        subptr = iwinptr->wintab[i].ptr;
        if ( subptr != NULL  &&  iwinptr->wintab[i].typ == TYP_EDIT )
          {
          edtptr = (WPEDIT *)subptr;
          if ( edtptr->focus == TRUE ) return(edtptr);
          }
        }
      return(NULL);
      }
   return(NULL);
  }

/********************************************************/
/*!******************************************************/

        short WPfoed(
        WPEDIT *edtptr,
        bool    mode)

/*      Fokus-rutin för WPEDIT.
 *
 *      In: edtptr = C-pekare till WPEDIT.
 *          mode   = TRUE  => Fokus på.
 *                   FALSE => Fokus av.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 14/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Fokus PÅ !
*/
    if ( mode == TRUE )
      {
      XSetWindowBorder(xdisp,edtptr->id.x_id,WPgcol(WP_NOTI));
      edtptr->focus = TRUE;
      }
/*
***Fokus AV !
*/
    else                            
      {
      XSetWindowBorder(xdisp,edtptr->id.x_id,WPgcol(WP_BGND));
      edtptr->focus = FALSE;
      }
/*
***För att resultatet skall synas (cursorn) krävs nu ett expose.
*/
    WPxped(edtptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short edmapk(
        XKeyEvent *keyev, 
        short     *sym,
        char      *t)

/*      Översätter keycode och state i ett key-event
 *      till en V3-symbol samt eventuellt ASCII-tecken.
 *
 *      In: keyev  = Pekare till key-event.
 *          sym    = Pekare till utdata.
 *          t      = Pekare till utdata.
 *
 *      Ut: *sym    = Motsvarande V3-symbol, tex. SMBUP.
 *          *t      = ASCII-kod om symbol = SMBCHAR.
 * 
 *      Fv:  0.
 *
 *      (C)microform ab 10/12/93 J. Kjellander
 *
 *      1998-04-21 8-Bitars ASCII, J.Kjellander
 *      1998-04-21 8-Bitars ASCII, R.Svedin
 *
 ******************************************************!*/

  {
    char            tknbuf[11],tkn='\0';
    short           symbol;
    bool            numlock = FALSE;
    bool            shift   = FALSE;
    KeySym          keysym;
    XComposeStatus  costat;
    int             ntkn = 0;

/*
***Numlock = Mod2Mask har det visat
***sig under ODT, detta är inte nödvändigtvis standard.
*/
    if ( ((keyev->state & Mod2Mask)  > 0) ) numlock = TRUE;
/*
***Var shift-tangenten nere ?
*/
    if ( ((keyev->state & ShiftMask)  > 0) ) shift = TRUE;
/*
***Vi börjar med att använda LookupString för att ta reda på
***vilken keysym det var. Vi kunde använda LookupKeysym() men
***LookupString() tar hänsyn till shift, numlock etc. åt oss
***på ett bättre sätt (hårdvaruoberoende).
*/
    ntkn = XLookupString(keyev,tknbuf,10,&keysym,&costat);
/*
***Vissa symboler skall mappas på ett för V3 speciellt sätt.
*/
    symbol = SMBNONE;

    switch ( keysym )
      {
/*
***å, ä och ö.
*/
      case XK_aring:      symbol = SMBCHAR; tkn = 'å' ; break;
      case XK_Aring:      symbol = SMBCHAR; tkn = 'Å' ; break;
      case XK_adiaeresis: symbol = SMBCHAR; tkn = 'ä' ; break;
      case XK_Adiaeresis: symbol = SMBCHAR; tkn = 'Ä' ; break;
      case XK_odiaeresis: symbol = SMBCHAR; tkn = 'ö' ; break;
      case XK_Odiaeresis: symbol = SMBCHAR; tkn = 'Ö' ; break;
/*
***Backspace och return (samt enter).
*/
      case XK_BackSpace:  symbol = SMBBACKSP; break;
      case XK_Return:     symbol = SMBRETURN; break;
/*
***Delete-tangenten skall i shiftad version mappas
***till '.' Med normal shift mappas den till KP_Separator
***och med numlock inte alls.
*/
      case XK_Delete:
      if ( numlock == TRUE )
        {
        symbol = SMBCHAR;
        tkn = '.';
        }
      else symbol = SMBBACKSP; 
      break;
/*
***Keypad-separator skall mappas till '.'
*/
      case XK_KP_Separator: symbol = SMBCHAR; tkn = '.'; break;
/*
***Piltangenter.
*/
      case XK_Up:     symbol = SMBUP;    break;
      case XK_Down:   symbol = SMBDOWN;  break;
      case XK_Left:   symbol = SMBLEFT;  break;
      case XK_Right:  symbol = SMBRIGHT; break;
/*
***Övriga tangenter använder vi LookupString():s mappning.
***Vissa ASCII-koder under 32 är tillåtna.
*/
      default:
      if ( ntkn == 1 )
        {
        switch ( tkn = tknbuf[0] )
          {
          case 13:
          case 10:
          symbol = SMBRETURN;
          break;

          case 8:
          symbol = SMBBACKSP;
          break;

          default:
          if ( tkn > 31 )                   symbol = SMBCHAR;
          else if ( tkn == *smbind[7].str ) symbol = SMBMAIN;
          else if ( tkn == *smbind[8].str ) symbol = SMBHELP;
          else if ( tkn == *smbind[9].str ) symbol = SMBPOSM;
          break;
          }
        }
      }
/*
***Slut.
*/
    *sym = symbol;
    *t   = tkn;

    return(0);
  }

/********************************************************/
