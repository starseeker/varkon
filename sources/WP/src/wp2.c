/**********************************************************************
*
*    wp2.c
*    ====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpinla();     Init list window (WPLWIN)
*    wpalla();     Add lines to WPLWIN
*    wpexla();     Exit list window
*
*    wpxplw();     Expose routine for WPLWIN
*    wpbtlw();     Button routine for WPLWIN
*    wpcrlw();     Crossing routine for WPLWIN
*    wpcmlw();     ClientMessage routine for WPLWIN
*    wpdllw();     Delete WPLWIN
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
#include "../include/WP.h"
#include <string.h>

extern char  jobdir[],jobnam[];
extern char *mktemp();

static WPLWIN  *actlwin = NULL;

/* actlwin är en C-pekare till ett WPLWIN som skapats (wpinla) men
   ännu ej blivit färdig (wpexla). Om actlwin = NULL finns inget
   sådant fönster på gång. */


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

        short wpinla(
        char *hs)


/*      Skapar nytt list-fönster, wp:s iginla(). MBS-
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
 *      8/11/94  Resurser för texterna, J. Kjellander
 *      1996-04-25 6 st. "X", J. Kjellander
 *      1997-01-15 v3genv(), J.Kjellander
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
***Kolla att inte ett listfönster redan är på gång.
***Isåfall, stäng det och returnera felkod. Därmed
***borde det vara OK för errmes() att skapa ny lista
***med felmeddelanden.
*/
    if ( actlwin != NULL )
      { 
      wpexla(TRUE);
      return(-2);
      }
/*
***Allra först ser vi till att öppna en ny temporär-fil.
***Skulle det misslyckas är ju ingen skada skedd och vi 
***slipper jobbig felhantering med malloc() och sånt.
*/  
    strcpy(templ,v3genv(VARKON_TMP));
    strcat(templ,jobnam);
    strcat(templ,".XXXXXX");
    mktemp(templ);
    strcpy(tmpfil,templ);

    if ( (fp=fopen(tmpfil,"w+")) == NULL ) return(-2);
/*
***Skapa fönstertitel (den text som skrivs ut i ramen).
***Hårdprogrammerat default är " ". Tom sträng "" tolkas
***av Motif som ingen titel och ersätts med stjärnor. Det
***vill vi inte ha.
*/
    if ( !wpgrst("varkon.list.title",title) ) strcpy(title," ");
    if (  wpgrst("varkon.list.title.jobnam",tmpbuf)  &&
          strcmp(tmpbuf,"True") == 0 ) strcat(title,jobnam);

    wpmaps(title);
/*
***Initiera knapptexter.
*/
    if ( !wpgrst("varkon.list.row",row) ) strcpy(row,"Rad");
    if ( !wpgrst("varkon.list.page",page) ) strcpy(page,"Sida");
    if ( !wpgrst("varkon.list.save",save) ) strcpy(save,"Spara");
    if ( !wpgrst("varkon.list.kill",kill) ) strcpy(kill,"St{ng");
/*
***Skapa ett ledigt fönster-ID.
*/
    if ( (id=wpwffi()) < 0 ) return(-2);
/*
***Skapa ett interimistiskt WPLWIN.
***Alla data finns ännu inte. X-fönstret är tex. inte skapat.
***Detta görs först i wpexla() som då får fylla i resten.
*/
    if ( (lwinptr=(WPLWIN *)v3mall(sizeof(WPLWIN),"wpinla"))
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
***Fönstrets måste minst vara så brett att alla 
***knappar samt rubrik och titel syns.
*/
    dx = wpstrl(page);
    if ( wpstrl(row) > dx ) dx = wpstrl(row);
    if ( wpstrl(save) > dx ) dx = wpstrl(save);
    if ( wpstrl(kill) > dx ) dx = wpstrl(kill);

    dx = (int)(3*1.4*dx + 4*wpstrh());

    if ( wpstrl(hs) > dx ) dx = wpstrl(hs);
    if ( wpstrl(title) > dx ) dx = wpstrl(title);

    lwinptr->maxrln = dx;

    lwinptr->rstart = 0;
    lwinptr->trant  = 1;
    lwinptr->frant  = 0;
    lwinptr->sscrol = TRUE;
/*
***Lagra fönstret i fönstertabellen.
***Fönstret är ännu ej komplett med alla data tex. X-id eller
***geometri men detta skall inte vara något problem.
*/
    wpwtab[id].typ = TYP_LWIN;
    wpwtab[id].ptr = (char *)lwinptr;
/*
***Sätt pekaren till aktivt listfönster.
*/
    actlwin = lwinptr;
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpalla(
        char *s, 
        short rs)

/*      Skriver in en rad text i listfilen.
 *
 *      In: s  = textsträng. 
 *          rs = radsprång.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *      931207 Omarbetad, J. Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *
 ******************************************************!*/

  {
    int lt,i,rest,nstkn;
    char mell[V3STRLEN+1],rad[V3STRLEN+1];

/*
***Kolla att aktivt listfönster verkligen finns.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Kolla att raden inte är längre än V3STRLEN tecken.
*/
    nstkn = strlen(s); 
    if ( nstkn > V3STRLEN )
      {
      s[V3STRLEN] = '\0';
      nstkn = V3STRLEN;
      }
/*
***Beräkna textradens längd i pixels.
*/  
    lt = wpstrl(s);
    if ( lt > actlwin->maxrln ) actlwin->maxrln = lt;
/*
***Om raden är kortare än V3STRLEN tecken, padda med space.
***Alla rader i list-filen skall vara V3STRLEN tecken + \n.
*/
    rest  = V3STRLEN - nstkn; 
    for ( i=0; i<rest; i++ ) mell[i] = ' ';
    mell[rest] = '\0';
    strcpy(rad,s);
    strcat(rad,mell);
    fprintf(actlwin->filpek,"%s\n",rad);
/*
***Räkna upp antalet textrader.
*/  
    ++actlwin->trant;
/*
***Skapa en rad med V3STRLEN mellanslag.
*/
      for (i=0;i<V3STRLEN;i++) mell[i] = ' ';
      mell[V3STRLEN] = '\0';
/*
***Beräkna antalet radsprång.
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

        short wpexla(
        bool  show)

/*      Avslutar en listning till ett listfönster.
 *      Stänger temporär-filen, skapar fönstret i X.
 *
 *      In: show = TRUE  => Visa.
 *                 FALSE => Stäng utan att visa.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *       8/12/93    Omarbetad, J. Kjellander
 *       7/11/94    Resurser för placering, J. Kjellander
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
***Kolla att ett listfönster verkligen är på gång.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Stäng temporärfilen.
*/
    fclose(actlwin->filpek);
/*
***Om inget skall visas är det enkelt. Ta bort temporärfilen och
***det interimistiska fönster som skapats i wpinla();
*/
   if ( !show)
     {
     v3fdel(actlwin->filnam);
     if ( (winptr=wpwgwp(actlwin->id.w_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       v3free((char *)actlwin,"wpexla");
       }
     actlwin = NULL;
     return(0);
     }
/*
***Beräkning av av max höjd på listfönstret.
*/
    mar = (int)(0.6*DisplayHeight(xdisp,xscr)/wpstrh());
    ar = actlwin->trant;

    if ( ar > mar ) ar = mar; 

    dy = 4*wpstrh() + ar*wpstrh() + wpstrh();
/*
***Beräkning av av min bredd på listfönstret.
*/
    dx = wpstrh() + actlwin->maxrln + wpstrh();
/*
***Listfönstrets läge.
*/
    x  = 90;
    y  = DisplayHeight(xdisp,xscr) - dy - 50; 

    if ( XrmGetResource(xresDB,"varkon.list.geometry","Varkon.List.Geometry",
         type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Skapa listfönstret.
*/
    crlwin(x,y,dx,dy,title);
/*
***Fråga hur stort listfönstret blev.
*/
    wpgwsz(actlwin->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***Sätt antal rader i fönstret.
*/
    actlwin->frant = py/wpstrh() - 6;
/*
***Beräkna knapparnas storlek.
*/
    dx = wpstrl(page);
    if ( wpstrl(row) > dx ) dx = wpstrl(row);
    if ( wpstrl(save) > dx ) dx = wpstrl(save);
    if ( wpstrl(kill) > dx ) dx = wpstrl(kill);
/*
***Skapa sida/rad-subfönstret.
*/
    x  = wpstrh();
    y  = (int)(0.25*x);
    dx = (int)(1.4*dx);
    dy = (int)(2*x);

    wpwcbu(actlwin->id.x_id,x,y,dx,dy,2,page,row,"",
                                     WP_BGND,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 0;

    actlwin->wintab[0].typ = TYP_BUTTON;
    actlwin->wintab[0].ptr = (char *)butptr;
/*
***Skapa spara-subfönstret.
*/
    x = x + dx + x;

    wpwcbu(actlwin->id.x_id,x,y,dx,dy,2,save,save,"",
                                     WP_BGND,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 1;

    actlwin->wintab[1].typ = TYP_BUTTON;
    actlwin->wintab[1].ptr = (char *)butptr;
/*
***Skapa stäng-subfönstret.
*/
    x = x + dx + wpstrh();

    wpwcbu(actlwin->id.x_id,x,y,dx,dy,2,kill,kill,"",
                                     WP_BGND,WP_FGND,&butptr);

    butptr->id.p_id = actlwin->id.w_id;
    butptr->id.w_id = 2;

    actlwin->wintab[2].typ = TYP_BUTTON;
    actlwin->wintab[2].ptr = (char *)butptr;
/*
***Visa listfönstret.
*/
    wpwshw(actlwin->id.w_id);
/*
***Aktivt listfönster finns ej nu längre.
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
        int     x,
        int     y,
        int     dx,
        int     dy,
        char   *wtitle)

/*      Skapar listfönster.   
 *
 *      In: x,y    = startkoordinater listfönstret.
 *          dx,dy  = bredd resp höjd på listfönster.
 *          wtitel = pekare till listfönstertitel. 
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 19/7/92 U. Andersson
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    XSizeHints           xhint;
    char                 titel[V3STRLEN];

/*
***Tilldelning av värden. 
*/
    strcpy(titel,"l-");
    strcat(titel,jobnam); 
/*
***Sätt färg mm.
*/
    xwina.background_pixel = wpgcol(WP_BGND);
    xwina.border_pixel = BlackPixel( xdisp, xscr );
    xwina.override_redirect = False;
    xwina.save_under = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );  
/*
***Skapa ett listfönster med 3 pixels ram.
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
    xhint.min_height = 4*wpstrh();  
    xhint.min_width  = wpstrl(wtitle);
    xhint.max_width  = V3STRLEN*wpstrl("w")+2*wpstrh();
    xhint.max_height = (int)(0.8*DisplayHeight(xdisp,xscr));

    XSetNormalHints(xdisp,actlwin->id.x_id,&xhint);
 
    XStoreName(xdisp,actlwin->id.x_id,wtitle);   
    XSetIconName(xdisp,actlwin->id.x_id,titel);
/*
***Sätt upp delete-protokollet mot fönsterhanteraren för
***listfönstret. 
*/   
    wpsdpr(actlwin->id.x_id);
/*
***Events som vi är intresserade av. 
*/   
    XSelectInput(xdisp,actlwin->id.x_id,ExposureMask |
                                        ButtonPressMask);
/*
***Slut. 
*/   
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool  wpxplw(
        WPLWIN *lwinpt)

/*      Expose-rutin för WPLWIN.
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
 *      1998-03-11 Längre rader, J.Kjellander
 *
 ******************************************************!*/

  {
    char    rad[V3STRLEN+3];
    int     j,n,tx,a,b,px,py;
    double  c,d;
    WPBUTT *butptr;

/*
***Först expose på subfönster.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    wpxpbu(butptr);

    butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
    wpxpbu(butptr);

    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    wpxpbu(butptr);
/*
***Ta reda på list-fönstrets storlek.
*/  
    wpgwsz(lwinpt->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***Antal rader i fönstret.
*/   
    lwinpt->frant = py/wpstrh() - 6;
/*
***Tilldelning av värden.
*/
    n = wpstrh();
    tx = n;
/*
***Skriv ut rubriken.
*/
    n = n + 3*wpstrh();
    wpwstr(lwinpt->id.x_id,tx,n,lwinpt->rubrik);
/*
***En tomrad efter rubriken.
*/
    n = n + wpstrh();
/*
***Öppna listfil för läsning.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Vilken rad ska vi börja läsningen ifrån.
*/
    fseek(lwinpt->filpek,lwinpt->rstart*(V3STRLEN+1),SEEK_SET);
/*
***Läs rad och skriv ut i listfönstret. I filen avslutas
***raderna med '\n' men här skall de avslutas med '\0'.
*/
    for ( j=0; j<lwinpt->frant; ++j )
      {  
      if ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL )
        {
        n +=  wpstrh();
        rad[V3STRLEN] = '\0';
        wpwstr(lwinpt->id.x_id,tx,n,rad);
        }
      else break; 
      }
/*
***Stäng listfil och återställ färg.
*/
    fclose(lwinpt->filpek); 

    return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool  wpbtlw(
        WPLWIN       *lwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button-rutin för WPLWIN med vidhängande sub-fönster.
 *
 *      In: iwinpt  = C-pekare till WPLWIN.
 *          butev   = Pekare till X-event.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      FV: TRUE =  Eventet har servats.
 *          FALSE = Eventet gällde nåt annat fönster. 
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *
 ******************************************************!*/

  {
    WPBUTT *butptr;

/*
***Till att börja med kollar vi om mus-tryckningen
***skett i själva list-fönstret.
*/
    if ( butev->window == lwinpt->id.x_id )
      {
     *serv_id = lwinpt->id.w_id;
      switch ( butev->button )
        {
/*
***Knapp 1 = scrolla texten uppåt.
*/
        case 1:
        butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
        if ( butptr->status == TRUE ) scrrup(lwinpt);
        else                          scrpup(lwinpt);
        break;
/*
***Knapp nr. 2 (mittenknappen på en 3-knappars mus) i ett
***list-fönster används för att döda det.
*/
        case 2:
        wpwdel(lwinpt->id.w_id);
        break;
/*
***Knapp 3 = scrolla texten neråt.
*/
        case 3:
        butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
        if ( butptr->status == TRUE ) scrrdn(lwinpt);
        else                          scrpdn(lwinpt);
        break;
/*
***Är det nån annan knapp bryr vi oss inte om den.
***Observera att vi då måste göra return() här.
*/
        default:
        return(FALSE);
        }
      return(TRUE);
      }
/*
***Rad/sida-knappen. På samma sätt som i crossing-
***rutinen kan man komma hit utan att knapparna
***finns ännu.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    if ( butptr != NULL  &&  butev->window == butptr->id.x_id )
      {
      wpbtbu(butptr);
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
***Stäng-knappen.
*/
    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    if ( butptr != NULL  &&  butev->window == butptr->id.x_id )
      {
      wpwdel(lwinpt->id.w_id);
      return(TRUE);
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool  wpcrlw(
        WPLWIN         *lwinpt,
        XCrossingEvent *croev)

/*      Crossing-rutin för WPLWIN med vidhängande sub-fönster.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE  => Eventet servat.
 *          FALSE => Detta fönster inte inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *
 ******************************************************!*/

  {
    WPBUTT *butptr;

/*
***Rad/sida-knappen. Om inte wpexla() anropats ännu, dvs
***listan håller på att skapas är inga knappar ännu skapade
***men ett WPLWIN finns och butptr = NULL.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
      {
      if ( croev->type == EnterNotify ) return(wpcrbu(butptr,TRUE));
      else                              return(wpcrbu(butptr,FALSE));
      }
/*
***Spara-knappen.
*/
    else
      {
      butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
      if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
        {
        if ( croev->type == EnterNotify ) return(wpcrbu(butptr,TRUE));
        else                              return(wpcrbu(butptr,FALSE));
        }
      else
        {
        butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
        if ( butptr != NULL  &&  croev->window == butptr->id.x_id )
          {
          if ( croev->type == EnterNotify ) return(wpcrbu(butptr,TRUE));
          else                              return(wpcrbu(butptr,FALSE));
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool wpcmlw(lwinpt,clmev)
        WPLWIN               *lwinpt;
        XClientMessageEvent  *clmev;

/*      ClientMessage-rutinen för WPLWIN.
 *
 *      In: iwinpt  = C-pekare till WPLWIN.
 *          clmev   = X-event.
 *
 *      FV: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Om det är WM_DELETE_WINDOW servar vi genom att döda
***fönstret ifråga.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    == 
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     wpwdel((v2int)lwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        short wpdllw(
        WPLWIN *lwinpt)

/*      Dödar ett WPLWIN-fönster.
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
***Ta bort rätt temporärfilen.
*/
    v3fdel(lwinpt->filnam);
/*
***Lämna tillbaks allokerat minne för subfönstren.
*/
    butptr = (WPBUTT *) lwinpt->wintab[0].ptr;
    v3free((char *)butptr,"wpdllw");

    butptr = (WPBUTT *) lwinpt->wintab[1].ptr;
    v3free((char *)butptr,"wpdllw");

    butptr = (WPBUTT *) lwinpt->wintab[2].ptr;
    v3free((char *)butptr,"wpdllw");
/*
***Lämna tillbaks allokerat minne för själva listfönstret.
*/
    v3free((char *)lwinpt,"wpdllw");
/*
***Slut.
*/
    return(0);
   }
/********************************************************/
/*!******************************************************/

 static short scrrup(
        WPLWIN *lwinpt)

/*      Scrollar listan en rad uppåt.
 *
 *      In: lwinpt = C-pekare till list-fönster.
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
***Scrollar listan en rad uppåt.
*/  
    nyradb = lwinpt->rstart + 1;

    if ( lwinpt->trant - nyradb < lwinpt->frant ) 
      nyradb = lwinpt->trant - lwinpt->frant;
    else if ( nyradb >= lwinpt->trant - lwinpt->frant )
      nyradb = lwinpt->rstart; 

    lwinpt->rstart = nyradb;
         
    wpxplw(lwinpt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrrdn(
        WPLWIN *lwinpt)

/*      Scrollar listan en rad nedåt.
 *
 *      In: lwinpt = C-pekare till list-fönster.
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
***Scrollar listan en rad nedåt.
*/  
    if ( lwinpt->rstart > 0 && lwinpt->rstart < lwinpt->trant )
      {
      --lwinpt->rstart;
      wpxplw(lwinpt);
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrpup(
        WPLWIN *lwinpt)

/*      Scrollar listan en hel sida uppåt.
 *
 *      In: lwinptr = C-pekare till listfönster.
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
***Scrolla listan en hel sida uppåt.
*/    
    nyradb = lwinpt->rstart + lwinpt->frant - 1;

    if ( lwinpt->trant - nyradb < lwinpt->frant )
      nyradb = lwinpt->trant - lwinpt->frant - 1;

    lwinpt->rstart = nyradb;

    wpxplw(lwinpt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short scrpdn(
        WPLWIN *lwinpt)

/*      Scrollar listan en hel sida nedåt.
 *
 *      In: lwinpt = C-pekare till listfönster.
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
***Scrolla listan en hel sida nedåt.
*/  
    if ( lwinpt->rstart - lwinpt->frant + 2 >  0  &&
                             lwinpt->rstart <= lwinpt->trant )
      lwinpt->rstart = lwinpt->rstart - lwinpt->frant + 1;
    else
      lwinpt->rstart = 0;

    wpxplw(lwinpt);

    return(0);

  }

/********************************************************/
/*!******************************************************/

 static short savelw(
        WPLWIN *lwinpt)

/*      Sparar det angivna listfönstret eller
 *      hela listfilen på fil eller utskrift till valfri skrivare.
 *
 *      In: lwinpt = C-pekare till list-fönster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 7/8/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1997-01-25 printer, J.Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *      2004-09-03 English texts, Johan Kjellander, Örebro university
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
***Anrop till alternativ funktionen wpialt
***ska vi spara hela listan eller bara fönstret?. 
*/
    hela = wpialt(slt,est,wst,FALSE);
/*
***Spara aktiv radbörjan.
*/
    radb = lwinpt->rstart;
/*
***Ska vi spara hela listan?. 
*/
    if ( hela == TRUE )  lwinpt->rstart = 0;
/*
***Anrop till alternativ funktionen wpialt
***spara på fil eller skrivare. 
*/
    fil = wpialt(spt,fst,pst,FALSE);
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
      status = igssip("Enter filename :",fnam,dstr,V3PTHLEN);
      if ( status < 0 ) goto end;
      strcpy(dstr,fnam);
      }
    else
      {
      strcpy(fnam,dstr);
      }
/*
***Öppna listfil för läsning.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Öppna  ny fil för skrivning.
*/
    tempfil = fopen(fnam,"w+");
/*
***Skriv in överskrift och en tomrad först i tmpfil.
*/
    fprintf(tempfil,"%s\n\n",lwinpt->rubrik);
/*
***Spara listfilen på en valfri fil.
***Vilken rad ska vi börja läsningen ifrån.
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
***Stäng filerna.
*/
    fclose(lwinpt->filpek);
    fclose(tempfil);
/*
***Tilldela radbörjan sitt ursprungliga värde.
*/
    lwinpt->rstart = radb;
/*
***Ev. utskrift på skrivare.
*/
    if ( !wpgrst("varkon.list.printer",printer) ) strcpy(printer,"lp");

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
