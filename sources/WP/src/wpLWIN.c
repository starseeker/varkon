/**********************************************************************
*
*    wpLWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPinla();     Init list window (WPLWIN)
*    WPalla();     Add lines to WPLWIN
*    WPexla();     Exit list window
*
*    WPxplw();     Expose routine for WPLWIN
*    WPbtlw();     Button routine for WPLWIN
*    WPcrlw();     Crossing routine for WPLWIN
*    WPcmlw();     ClientMessage routine for WPLWIN
*    WPdllw();     Delete WPLWIN
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
#include <string.h>

extern char  jobdir[],jobnam[];
extern char *mktemp();

static WPLWIN  *actlwin = NULL;

/* actlwin �r en C-pekare till ett WPLWIN som skapats (WPinla) men
   �nnu ej blivit f�rdig (WPexla). Om actlwin = NULL finns inget
   s�dant f�nster p� g�ng. */


static char title [81];        /* varkon.list.title  */
static char row   [81];        /* varkon.list.row    */
static char page  [81];        /* varkon.list.page   */
static char save  [81];        /* varkon.list.save   */
static char kill  [81];        /* varkon.list.kill   */

/*
***Diverse resurser.
*/
static short crlwin(int,int,int,int,char *wtitle);
static short scrrup(WPLWIN *lwinpt);
static short scrpup(WPLWIN *lwinpt);
static short scrrdn(WPLWIN *lwinpt);
static short scrpdn(WPLWIN *lwinpt);
static short savelw(WPLWIN *lwinpt);

/*!******************************************************/

        short WPinla(
        char *hs)


/*      Skapar nytt list-f�nster, wp:s WPinla(). MBS-
 *      LST_INI(rubrik);
 *
 *      In: hs = Rubriktext.  
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/7/92 U. Andersson
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      8/11/94  Resurser f�r texterna, J. Kjellander
 *      1996-04-25 6 st. "X", J. Kjellander
 *      1997-01-15 IGgenv(), J.Kjellander
 *
 ******************************************************!*/

  {
    char     templ[V3PTHLEN+1],tmpfil[V3PTHLEN+1],
             tmpbuf[V3STRLEN+1];
    wpw_id   id;
    int      dx;
    WPLWIN  *lwinptr;
    FILE    *fp;

/*
***Kolla att inte ett listf�nster redan �r p� g�ng.
***Is�fall, st�ng det och returnera felkod. D�rmed
***borde det vara OK f�r errmes() att skapa ny lista
***med felmeddelanden.
*/
    if ( actlwin != NULL )
      { 
      WPexla(TRUE);
      return(-2);
      }
/*
***Allra f�rst ser vi till att �ppna en ny tempor�r-fil.
***Skulle det misslyckas �r ju ingen skada skedd och vi 
***slipper jobbig felhantering med malloc() och s�nt.
*/  
    strcpy(templ,IGgenv(VARKON_TMP));
    strcat(templ,jobnam);
    strcat(templ,".XXXXXX");
    mktemp(templ);
    strcpy(tmpfil,templ);

    if ( (fp=fopen(tmpfil,"w+")) == NULL ) return(-2);
/*
***Skapa f�nstertitel (den text som skrivs ut i ramen).
***H�rdprogrammerat default �r " ". Tom str�ng "" tolkas
***av Motif som ingen titel och ers�tts med stj�rnor. Det
***vill vi inte ha.
*/
    if ( !WPgrst("varkon.list.title",title) ) strcpy(title," ");
    if (  WPgrst("varkon.list.title.jobnam",tmpbuf)  &&
          strcmp(tmpbuf,"True") == 0 ) strcat(title,jobnam);
/*
***Initiera knapptexter.
*/
    if ( !WPgrst("varkon.list.row",row) ) strcpy(row,"Rad");
    if ( !WPgrst("varkon.list.page",page) ) strcpy(page,"Sida");
    if ( !WPgrst("varkon.list.save",save) ) strcpy(save,"Spara");
    if ( !WPgrst("varkon.list.kill",kill) ) strcpy(kill,"St{ng");
/*
***Skapa ett ledigt f�nster-ID.
*/
    if ( (id=WPwffi()) < 0 ) return(-2);
/*
***Skapa ett interimistiskt WPLWIN.
***Alla data finns �nnu inte. X-f�nstret �r tex. inte skapat.
***Detta g�rs f�rst i WPexla() som d� f�r fylla i resten.
*/
    if ( (lwinptr=(WPLWIN *)v3mall(sizeof(WPLWIN),"WPinla"))
                                                   == NULL ) return(-2);

    lwinptr->id.w_id = id;
    lwinptr->id.p_id = 0;
    lwinptr->id.x_id = 0;

    lwinptr->geo.x =  0;
    lwinptr->geo.y =  0;
    lwinptr->geo.dx =  0;
    lwinptr->geo.dy =  0;

    lwinptr->wintab[0].ptr = NULL;
    lwinptr->wintab[1].ptr = NULL;
    lwinptr->wintab[2].ptr = NULL;

    lwinptr->filpek = fp;
    strcpy(lwinptr->filnam,tmpfil);

    strcpy(lwinptr->rubrik,hs);
/*
***F�nstrets m�ste minst vara s� brett att alla 
***knappar samt rubrik och titel syns.
*/
    dx = WPstrl(page);
    if ( WPstrl(row) > dx ) dx = WPstrl(row);
    if ( WPstrl(save) > dx ) dx = WPstrl(save);
    if ( WPstrl(kill) > dx ) dx = WPstrl(kill);

    dx = (int)(3*1.4*dx + 4*WPstrh());

    if ( WPstrl(hs) > dx ) dx = WPstrl(hs);
    if ( WPstrl(title) > dx ) dx = WPstrl(title);

    lwinptr->maxrln = dx;

    lwinptr->rstart = 0;
    lwinptr->trant  = 1;
    lwinptr->frant  = 0;
    lwinptr->sscrol = TRUE;
/*
***Lagra f�nstret i f�nstertabellen.
***F�nstret �r �nnu ej komplett med alla data tex. X-id eller
***geometri men detta skall inte vara n�got problem.
*/
    wpwtab[id].typ = TYP_LWIN;
    wpwtab[id].ptr = (char *)lwinptr;
/*
***S�tt pekaren till aktivt listf�nster.
*/
    actlwin = lwinptr;
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPalla(
        char *s, 
        short rs)

/*      Skriver in en rad text i listfilen.
 *
 *      In: s  = textstr�ng. 
 *          rs = radspr�ng.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *      931207 Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *
 ******************************************************!*/

  {
    int lt,i,rest,nstkn;
    char mell[V3STRLEN+1],rad[V3STRLEN+1];

/*
***Kolla att aktivt listf�nster verkligen finns.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Kolla att raden inte �r l�ngre �n V3STRLEN tecken.
*/
    nstkn = strlen(s); 
    if ( nstkn > V3STRLEN )
      {
      s[V3STRLEN] = '\0';
      nstkn = V3STRLEN;
      }
/*
***Ber�kna textradens l�ngd i pixels.
*/  
    lt = WPstrl(s);
    if ( lt > actlwin->maxrln ) actlwin->maxrln = lt;
/*
***Om raden �r kortare �n V3STRLEN tecken, padda med space.
***Alla rader i list-filen skall vara V3STRLEN tecken + \n.
*/
    rest  = V3STRLEN - nstkn; 
    for ( i=0; i<rest; i++ ) mell[i] = ' ';
    mell[rest] = '\0';
    strcpy(rad,s);
    strcat(rad,mell);
    fprintf(actlwin->filpek,"%s\n",rad);
/*
***R�kna upp antalet textrader.
*/  
    ++actlwin->trant;
/*
***Skapa en rad med V3STRLEN mellanslag.
*/
      for (i=0;i<V3STRLEN;i++) mell[i] = ' ';
      mell[V3STRLEN] = '\0';
/*
***Ber�kna antalet radspr�ng.
***Skriv till listfilen.
*/
      for ( i=0; i<rs - 1; i++ )
        {
        ++actlwin->trant;
        fprintf(actlwin->filpek,"%s\n",mell);
        }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPexla(
        bool  show)

/*      Avslutar en listning till ett listf�nster.
 *      St�nger tempor�r-filen, skapar f�nstret i X.
 *
 *      In: show = TRUE  => Visa.
 *                 FALSE => St�ng utan att visa.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *       8/12/93    Omarbetad, J. Kjellander
 *       7/11/94    Resurser f�r placering, J. Kjellander
 *       1996-02-05 show, J. Kjellander
 *
 ******************************************************!*/

  {
    int      x,y,mar,ar,a,b,px,py,dx,dy;
    unsigned int dum1,dum2;
    double   c,d;
    char    *type[20];
    XrmValue value;
    WPWIN   *winptr;
    WPBUTT  *butptr;

/*
***Kolla att ett listf�nster verkligen �r p� g�ng.
*/
    if ( actlwin == NULL ) return(-2);
/*
***St�ng tempor�rfilen.
*/
    fclose(actlwin->filpek);
/*
***Om inget skall visas �r det enkelt. Ta bort tempor�rfilen och
***det interimistiska f�nster som skapats i WPinla();
*/
   if ( !show)
     {
     IGfdel(actlwin->filnam);
     if ( (winptr=WPwgwp(actlwin->id.w_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       v3free((char *)actlwin,"WPexla");
       }
     actlwin = NULL;
     return(0);
     }
/*
***Ber�kning av av max h�jd p� listf�nstret.
*/
    mar = (int)(0.6*DisplayHeight(xdisp,xscr)/WPstrh());
    ar = actlwin->trant;

    if ( ar > mar ) ar = mar; 

    dy = 4*WPstrh() + ar*WPstrh() + WPstrh();
/*
***Ber�kning av av min bredd p� listf�nstret.
*/
    dx = WPstrh() + actlwin->maxrln + WPstrh();
/*
***Listf�nstrets l�ge.
*/
    x  = 90;
    y  = DisplayHeight(xdisp,xscr) - dy - 50; 

    if ( XrmGetResource(xresDB,"varkon.list.geometry","Varkon.List.Geometry",
         type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Skapa listf�nstret.
*/
    crlwin(x,y,dx,dy,title);
/*
***Fr�ga hur stort listf�nstret blev.
*/
    WPgwsz(actlwin->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***S�tt antal rader i f�nstret.
*/
    actlwin->frant = py/WPstrh() - 6;
/*
***Ber�kna knapparnas storlek.
*/
    dx = WPstrl(page);
    if ( WPstrl(row) > dx ) dx = WPstrl(row);
    if ( WPstrl(save) > dx ) dx = WPstrl(save);
    if ( WPstrl(kill) > dx ) dx = WPstrl(kill);
/*
***Skapa sida/rad-subf�nstret.
*/
    x  = WPstrh();
    y  = (int)(0.25*x);
    dx = (int)(1.4*dx);
    dy = (int)(2*x);

    WPwcbu(actlwin->id.x_id,x,y,dx,dy,2,page,row,"",
                                     WP_BGND2,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 0;

    actlwin->wintab[0].typ = TYP_BUTTON;
    actlwin->wintab[0].ptr = (char *)butptr;
/*
***Skapa spara-subf�nstret.
*/
    x = x + dx + x;

    WPwcbu(actlwin->id.x_id,x,y,dx,dy,2,save,save,"",
                                     WP_BGND2,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 1;

    actlwin->wintab[1].typ = TYP_BUTTON;
    actlwin->wintab[1].ptr = (char *)butptr;
/*
***Skapa st�ng-subf�nstret.
*/
    x = x + dx + WPstrh();

    WPwcbu(actlwin->id.x_id,x,y,dx,dy,2,kill,kill,"",
                                     WP_BGND2,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 2;

    actlwin->wintab[2].typ = TYP_BUTTON;
    actlwin->wintab[2].ptr = (char *)butptr;
/*
***Visa listf�nstret.
*/
    WPwshw(actlwin->id.w_id);
/*
***Aktivt listf�nster finns ej nu l�ngre.
*/
    actlwin = NULL;
/*
***Slut.
*/
    return(0);
  }
/********************************************************/
/*!******************************************************/

 static short crlwin(
        int   x,
        int   y,
        int   dx,
        int   dy,
        char *wtitle)

/*      Skapar listf�nster.   
 *
 *      In: x,y    = startkoordinater listf�nstret.
 *          dx,dy  = bredd resp h�jd p� listf�nster.
 *          wtitel = pekare till listf�nstertitel. 
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 19/7/92 U. Andersson
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2006-12-19 ButtonReleaseMAsk, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    XSizeHints           xhint;
    char                 titel[V3STRLEN];

/*
***Tilldelning av v�rden. 
*/
    strcpy(titel,"l-");
    strcat(titel,jobnam); 
/*
***S�tt f�rg mm.
*/
    xwina.background_pixel = WPgcol(WP_BGND1);
    xwina.border_pixel = BlackPixel( xdisp, xscr );
    xwina.override_redirect = False;
    xwina.save_under = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  
/*
***Skapa ett listf�nster med 3 pixels ram.
*/
    actlwin->id.x_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),
                                     x,y,dx,dy,3,
                                     DefaultDepth(xdisp,xscr),
                                     InputOutput,
                                     CopyFromParent,xwinm,&xwina);
   
    xhint.flags  = USPosition | USSize | PMinSize | PMaxSize;   
    xhint.x = x;
    xhint.y = y;
    xhint.width  = dx;
    xhint.height = dy; 
    xhint.min_height = 4*WPstrh();  
    xhint.min_width  = WPstrl(wtitle);
    xhint.max_width  = V3STRLEN*WPstrl("w")+2*WPstrh();
    xhint.max_height = (int)(0.8*DisplayHeight(xdisp,xscr));

    XSetNormalHints(xdisp,actlwin->id.x_id,&xhint);
 
    XStoreName(xdisp,actlwin->id.x_id,wtitle);   
    XSetIconName(xdisp,actlwin->id.x_id,titel);
/*
***S�tt upp delete-protokollet mot f�nsterhanteraren f�r
***listf�nstret. 
*/   
    WPsdpr(actlwin->id.x_id);
/*
***Events som vi �r intresserade av. 
*/   
    XSelectInput(xdisp,actlwin->id.x_id,ExposureMask    |
                                        ButtonPressMask |
                                        ButtonReleaseMask);
/*
***Slut. 
*/   
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    WPxplw(
        WPLWIN *lwinpt)

/*      Expose-rutin f�r WPLWIN.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      Ut: Inget. 
 *
 *      FV: 0.
 *
 *      (C)microform ab 11/7/92 U. Andersson.
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *
 ******************************************************!*/

  {
    char    rad[V3STRLEN+3];
    int     j,n,tx,a,b,px,py;
    double  c,d;
    WPBUTT *butptr;

/*
***F�rst expose p� subf�nster.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    WPxpbu(butptr);

    butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
    WPxpbu(butptr);

    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    WPxpbu(butptr);
/*
***Ta reda p� list-f�nstrets storlek.
*/  
    WPgwsz(lwinpt->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***Aktivera r�tt f�rg.
*/
    XSetBackground(xdisp,xgc,WPgcol(WP_BGND1));
/*
***Antal rader i f�nstret.
*/   
    lwinpt->frant = py/WPstrh() - 6;
/*
***Tilldelning av v�rden.
*/
    n = WPstrh();
    tx = n;
/*
***Skriv ut rubriken.
*/
    n = n + 3*WPstrh();
    WPwstr(lwinpt->id.x_id,tx,n,lwinpt->rubrik);
/*
***En tomrad efter rubriken.
*/
    n = n + WPstrh();
/*
***�ppna listfil f�r l�sning.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Vilken rad ska vi b�rja l�sningen ifr�n.
*/
    fseek(lwinpt->filpek,lwinpt->rstart*(V3STRLEN+1),SEEK_SET);
/*
***L�s rad och skriv ut i listf�nstret. I filen avslutas
***raderna med '\n' men h�r skall de avslutas med '\0'.
*/
    for ( j=0; j<lwinpt->frant; ++j )
      {  
      if ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL )
        {
        n +=  WPstrh();
        rad[V3STRLEN] = '\0';
        WPwstr(lwinpt->id.x_id,tx,n,rad);
        }
      else break; 
      }
/*
***St�ng listfil och �terst�ll f�rg.
*/
    fclose(lwinpt->filpek); 

    return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool          WPbtlw(
        WPLWIN       *lwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin f�r WPLWIN med vidh�ngande sub-f�nster.
 *
 *      In: iwinpt  = C-pekare till WPLWIN.
 *          butev   = Pekare till X-event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID f�r subf�nster som servat eventet.
 *
 *      FV: TRUE =  Eventet har servats.
 *          FALSE = Eventet g�llde n�t annat f�nster. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *
 ******************************************************!*/

  {
    WPBUTT *butptr;

/*
***Till att b�rja med kollar vi om mus-tryckningen
***skett i sj�lva list-f�nstret.
*/
    if ( butev->window == lwinpt->id.x_id )
      {
     *serv_id = lwinpt->id.w_id;
      switch ( butev->button )
        {
/*
***Knapp 1 = scrolla texten upp�t.
*/
        case 1:
        butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
        if ( butptr->status == TRUE ) scrrup(lwinpt);
        else                          scrpup(lwinpt);
        break;
/*
***Knapp nr. 2 (mittenknappen p� en 3-knappars mus) i ett
***list-f�nster anv�nds f�r att d�da det.
*/
        case 2:
        WPwdel(lwinpt->id.w_id);
        break;
/*
***Knapp 3 = scrolla texten ner�t.
*/
        case 3:
        butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
        if ( butptr->status == TRUE ) scrrdn(lwinpt);
        else                          scrpdn(lwinpt);
        break;
/*
***�r det n�n annan knapp bryr vi oss inte om den.
***Observera att vi d� m�ste g�ra return() h�r.
*/
        default:
        return(FALSE);
        }
      return(TRUE);
      }
/*
***Rad/sida-knappen. P� samma s�tt som i crossing-
***rutinen kan man komma hit utan att knapparna
***finns �nnu.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    if ( butptr != NULL  &&  butev->window == butptr->id.x_id )
      {
      WPbtbu(butptr);
     *serv_id = butptr->id.w_id;
      return(TRUE);
      }
/*
***Spara-knappen.
*/
    butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
    if ( butptr != NULL  &&  butev->window == butptr->id.x_id )
      {
      savelw(lwinpt);
     *serv_id = butptr->id.w_id;
      return(TRUE);
      }
/*
***St�ng-knappen.
*/
    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    if ( butptr != NULL  &&  butev->window == butptr->id.x_id )
      {
      WPwdel(lwinpt->id.w_id);
      return(TRUE);
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool            WPcrlw(
        WPLWIN         *lwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin f�r WPLWIN med vidh�ngande sub-f�nster.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  => Eventet servat.
 *          FALSE => Detta f�nster inte inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *
 ******************************************************!*/

  {
    WPBUTT *butptr;

/*
***Rad/sida-knappen. Om inte WPexla() anropats �nnu, dvs
***listan h�ller p� att skapas �r inga knappar �nnu skapade
***men ett WPLWIN finns och butptr = NULL.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
      {
      if ( croev->type == EnterNotify ) return(WPcrbu(butptr,TRUE));
      else                              return(WPcrbu(butptr,FALSE));
      }
/*
***Spara-knappen.
*/
    else
      {
      butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
      if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
        {
        if ( croev->type == EnterNotify ) return(WPcrbu(butptr,TRUE));
        else                              return(WPcrbu(butptr,FALSE));
        }
      else
        {
        butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
        if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
          {
          if ( croev->type == EnterNotify ) return(WPcrbu(butptr,TRUE));
          else                              return(WPcrbu(butptr,FALSE));
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool                  WPcmlw(
        WPLWIN               *lwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage-rutinen f�r WPLWIN.
 *
 *      In: iwinpt  = C-pekare till WPLWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta f�nster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om det �r WM_DELETE_WINDOW servar vi genom att d�da
***f�nstret ifr�ga.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPwdel((DBint)lwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short   WPdllw(
        WPLWIN *lwinpt)

/*      D�dar ett WPLWIN-f�nster.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/7/92 U. Andersson 
 *
 *      7/12/93 Omarbetad, J. Kjellander
 *
 ******************************************************!*/

   {
    WPBUTT *butptr;
/*
***Ta bort r�tt tempor�rfilen.
*/
    IGfdel(lwinpt->filnam);
/*
***L�mna tillbaks allokerat minne f�r subf�nstren.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    v3free((char *)butptr,"WPdllw");

    butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
    v3free((char *)butptr,"WPdllw");

    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    v3free((char *)butptr,"WPdllw");
/*
***L�mna tillbaks allokerat minne f�r sj�lva listf�nstret.
*/
    v3free((char *)lwinpt,"WPdllw");
/*
***Slut.
*/
    return(0);
   }
/********************************************************/
/*!******************************************************/

 static short scrrup(
        WPLWIN *lwinpt)

/*      Scrollar listan en rad upp�t.
 *
 *      In: lwinpt = C-pekare till list-f�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/7/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *
 ******************************************************!*/

   {
    int nyradb;
/*
***Scrollar listan en rad upp�t.
*/  
    nyradb = lwinpt->rstart + 1;

    if ( lwinpt->trant - nyradb < lwinpt->frant ) 
      nyradb = lwinpt->trant - lwinpt->frant;
    else if ( nyradb >= lwinpt->trant - lwinpt->frant )
      nyradb = lwinpt->rstart; 

    lwinpt->rstart = nyradb;
         
    WPxplw(lwinpt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrrdn(
        WPLWIN *lwinpt)

/*      Scrollar listan en rad ned�t.
 *
 *      In: lwinpt = C-pekare till list-f�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/7/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Scrollar listan en rad ned�t.
*/  
    if ( lwinpt->rstart > 0 && lwinpt->rstart < lwinpt->trant )
      {
      --lwinpt->rstart;
      WPxplw(lwinpt);
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrpup(
        WPLWIN *lwinpt)

/*      Scrollar listan en hel sida upp�t.
 *
 *      In: lwinptr = C-pekare till listf�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 31/7/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *
 ******************************************************!*/

  {
    int nyradb;

/*
***Scrolla listan en hel sida upp�t.
*/    
    nyradb = lwinpt->rstart + lwinpt->frant - 1;

    if ( lwinpt->trant - nyradb < lwinpt->frant )
      nyradb = lwinpt->trant - lwinpt->frant - 1;

    lwinpt->rstart = nyradb;

    WPxplw(lwinpt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrpdn(
        WPLWIN *lwinpt)

/*      Scrollar listan en hel sida ned�t.
 *
 *      In: lwinpt = C-pekare till listf�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 31/7/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Scrolla listan en hel sida ned�t.
*/  
    if ( lwinpt->rstart - lwinpt->frant + 2 >  0  &&
                             lwinpt->rstart <= lwinpt->trant )
      lwinpt->rstart = lwinpt->rstart - lwinpt->frant + 1;
    else
      lwinpt->rstart = 0;

    WPxplw(lwinpt);

    return(0);

  }

/********************************************************/
/*!******************************************************/

 static short savelw(
        WPLWIN *lwinpt)

/*      Sparar det angivna listf�nstret eller
 *      hela listfilen p� fil eller utskrift till valfri skrivare.
 *
 *      In: lwinpt = C-pekare till list-f�nster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 7/8/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1997-01-25 printer, J.Kjellander
 *      1998-03-11 L�ngre rader, J.Kjellander
 *      2004-09-03 English texts, Johan Kjellander, �rebro university
 *
 ******************************************************!*/

  {
   int   radb,r = 0;
   char *slt = "Entire list or window";
   char *est = "e";
   char *wst = "w";  
   char *spt = "  File or printer  ";  
   char *fst = "f";
   char *pst = "p";  
   char  rad[V3STRLEN+2];
   bool  hela,fil;
   short status;
   char  oscmd[V3PTHLEN+25],fnam[V3PTHLEN],printer[V3STRLEN];
   FILE *tempfil;

   static char dstr[V3PTHLEN] = "";

/*
***Anrop till alternativ funktionen WPialt
***ska vi spara hela listan eller bara f�nstret?. 
*/
    hela = WPialt(slt,est,wst,FALSE);
/*
***Spara aktiv radb�rjan.
*/
    radb = lwinpt->rstart;
/*
***Ska vi spara hela listan?. 
*/
    if ( hela == TRUE )  lwinpt->rstart = 0;
/*
***Anrop till alternativ funktionen WPialt
***spara p� fil eller skrivare. 
*/
    fil = WPialt(spt,fst,pst,FALSE);
/*
***Vad skall filen heta ?
*/
    if ( dstr[0] == '\0' )
      {
      strcpy(dstr,jobdir);
      strcat(dstr,jobnam);
      strcat(dstr,LSTEXT);
      }

    if ( fil == TRUE )
      {
      status = IGssip("Enter filename :",fnam,dstr,V3PTHLEN);
      if ( status < 0 ) goto end;
      strcpy(dstr,fnam);
      }
    else
      {
      strcpy(fnam,dstr);
      }
/*
***�ppna listfil f�r l�sning.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***�ppna  ny fil f�r skrivning.
*/
    tempfil = fopen(fnam,"w+");
/*
***Skriv in �verskrift och en tomrad f�rst i tmpfil.
*/
    fprintf(tempfil,"%s\n\n",lwinpt->rubrik);
/*
***Spara listfilen p� en valfri fil.
***Vilken rad ska vi b�rja l�sningen ifr�n.
*/
     fseek(lwinpt->filpek,lwinpt->rstart*81,SEEK_SET);

     if ( hela == TRUE ) 
       {
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL) 
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
         }
       }
     else 
       {
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL &&
               r < lwinpt->frant ) 
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
         ++r;
         }
       }
/*
***St�ng filerna.
*/
    fclose(lwinpt->filpek);
    fclose(tempfil);
/*
***Tilldela radb�rjan sitt ursprungliga v�rde.
*/
    lwinpt->rstart = radb;
/*
***Ev. utskrift p� skrivare.
*/
    if ( !WPgrst("varkon.list.printer",printer) ) strcpy(printer,"lp");

    if ( fil == FALSE )
      {
      strcpy(oscmd,"cat ");
      strcat(oscmd,fnam);
      strcat(oscmd," | ");
      strcat(oscmd,printer);
      EXos(oscmd,2);
      }
/*
***Slut.
*/
end:
    return(0);
  }
/********************************************************/
