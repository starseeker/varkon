/**********************************************************************
*
*    wp5.c
*    ====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpintr();   Interrupt routine
*    wplset();   Sets alarm signal
*    wplinc();   Catches alarm signal
*    wpwait();   Handles Wait...
*    wpwton();   Returns wait status On/Off
*    wpupwb();   Updates WPGWIN title text
*    wpposw();   Positions windows
*    wpwlma();   Dsiplays message text
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
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define WP_INTTIME 2       /* Antal sekunder mellan alarm */

static int     n_larm = 0;
static int     n_secs = 0;
static bool    wt_on  = FALSE;
static WPGWIN *wt_win = NULL;

/* n_larm sätt till noll av wpwait(xxx,TRUE) och
   räknas upp av wplinc() samt ner av wpintr(). 
   n_secs sätt också till noll men räknas inte ner.
   wt_on är TRUE om vänta är påslaget av wpwait() annars FALSE.
   wt_win är en pekare till det fönster där drwait()
   skall visa väntameddelandet. */


extern char        pidnam[],jobnam[];
extern short       gptrty;
extern MNUALT      smbind[];
extern V3MDAT      sydata;

/*!*******************************************************/

       bool wpintr()

/*     wp:s interrupt-rutin. Anropas med jämna mellanrum
 *     av alla rutiner som kan ta lång tid på sig. Sköter
 *     uppdatering av vänta-meddelandet om vänta är på samt
 *     pollar event-kön efter <CTRL>c.
 *
 *     In: Inget.
 *
 *     Ut: Inget.
 *
 *     FV: TRUE  = V3 skall avbrytas.
 *         FALSE = Fortsätt exekveringen.
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *     1998-02-26 event, J.Kjellander
 *
 *******************************************************!*/

 {
   XEvent          event;
   XKeyEvent      *keyev = (XKeyEvent *) &event;
   char            tknbuf[11];
   KeySym          keysym;
   XComposeStatus  costat;

/*
***Om n_larm > 0 är vänta-hanteringen påslagen och det har
***gått tillräckligt med tid sen vi sist uppdaterade vänta-
***meddelandet. Alltså uppdaterar vi igen. Om n_larm == 0
***har det antingen inte gått tillräckligt lång tid eller
***också är inte vänta-hanteringen påslagen överhuvudtaget.
*/
   if ( n_larm > 0 )
     {
     drwait(TRUE);
   --n_larm;
/*
***Samtidigt som vi uppdaterar vänta-meddelandet passar vi
***på att polla eventkön efter <CTRL>c.
*/
     while ( XCheckMaskEvent(xdisp,KeyPressMask,&event) )
       {
       if ( keyev->state & ControlMask  &&
            XLookupString(keyev,tknbuf,10,&keysym,&costat) == 1   &&
            tknbuf[0] == *smbind[11].str )
         {
         XBell(xdisp,100);
         XFlush(xdisp);
         return(TRUE);
         }
       }
     }

   return(FALSE);
 }

/*********************************************************/
/*!*******************************************************/

       short wplset(
       bool set)

/*     Beställer en alarmsignal om en liten stund eller
 *     dödar en redan beställd.
 *
 *     In: set => TRUE  = Beställ signal.
 *                FALSE = Döda beställd.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *******************************************************!*/

 {
/*
***Om det är första gången väntar vi lite längre sen
***kör vi på som vanligt.
*/
   if ( set )
     {
     signal(SIGALRM,wplinc);
     if ( n_secs == 0 )
       {
       n_secs = WP_INTTIME;
       alarm(2*WP_INTTIME);
       }
     else alarm(WP_INTTIME);
     }
   else
     {
     alarm(0);
     signal(SIGALRM,SIG_DFL);
     }

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       void wplinc()

/*     Fångar en alarmsignal beställd av wplset()
 *     samt beställer en ny.
 *
 *     In: Inget.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *     1998-02-26 void, J.Kjellander
 *
 *******************************************************!*/

 {

/*
***Räkna upp larm- och sekund-räknarna.
*/
   ++n_larm;
   n_secs += WP_INTTIME;
/*
***Beställ ett nytt larm om en stund igen.
*/
   wplset(TRUE);

   return;
 }

/*********************************************************/
/*!*******************************************************/

       bool wpwton()

/*     Returnerar värdet av wt_on. Används av rutiner
 *     som tillfälligt vill stänga av interruptgenereringen.
 *     Tex. exos(). Om wpwton() == TRUE gör man då först
 *     wplset(FALSE) och senare wplset(TRUE) igen.
 *
 *     FV: wt_on.
 *
 *     (C)microform ab 2/1/96 J. Kjellander
 *
 *******************************************************!*/

 {
   return(wt_on);
 }

/*********************************************************/
/*!*******************************************************/

       short wpwait(
       v2int win_id,
       bool  wait)

/*     Slår på eller av vänta-hanteringen.
 *
 *     In: win_id => Fönster-ID eller GWIN_ALL.
 *         wait   => TRUE = slå på, FALSE = stäng av.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *     1998-05-13 Batch, J.Kjellander
 *
 *******************************************************!*/

 {
   WPWIN  *winptr;

/*
***Om inte X11 är initierat kan vi inget göra.
*/
   if ( gptrty != X11 ) return(0);
/*
***Om det är GWIN_ALL som gäller handlar det om att vänta
***på något som inte händer i ett särskilt fönster utan
***i alla fönster eller inte i något fönster alls. Isåfall
***visar vi vänta... i huvudfönstret.
*/
   if ( wait )
     {
     if ( win_id == GWIN_ALL )
       {
       winptr = wpwgwp((wpw_id)GWIN_MAIN);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Om det handlar om att vänta i ett visst fönster fixar vi
***en C-pekare till just det fönstret.
*/
     else
       {
       winptr = wpwgwp((wpw_id)win_id);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Eftersom vänta skall slås på nollställer vi lite
***och startar beställning av alarmsignaler.
*/
     n_larm = n_secs = 0;
     wplset(TRUE);
     }
/*
***Om vänta skall slås av stänger vi av beställningen av
***alarmsignaler och nollställer larmräknaren igen så att
***wpintr() inte servar interrupt i onödan om alla inte
***har hunnit servas medan vänta var på.
*/
   else
     {
     drwait(FALSE);
     n_larm = 0;
     wplset(FALSE);
     }
/*
***Kom ihåg om vi är på eller av. Används av wpwton().
*/
   wt_on = wait;
/*
***Slut.
*/
   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       short drwait(
       bool  draw)

/*     Ritar vänta....
 *
 *     In: draw => TRUE = Sudda+rita. FALSE = Bara sudda.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *     1998-09-08 iggtts(), J.Kjellander
 *
 *******************************************************!*/

 {
   int     x,y,dx,dy;
   char    wt_str[V3STRLEN+1];
   char    timbuf[V3STRLEN+1];

static  WPBUTT *buttpt = 0;

/*
***Om buttpt == NULL är det första gången och då måste vi skapa
***själva vänta-fönstret.
*/
   if ( draw  &&  buttpt == NULL )
     {
     dx = wpstrl("Vänta...XXXXX") + 15;
     dy = (int)(1.6*(double)wpstrh());
     x  = (int)(wt_win->vy.scrwin.xmax - dx - 10);
     y  = (int)(wt_win->geo.dy - wt_win->vy.scrwin.ymin - dy - 10);

     wpwcbu(wt_win->id.x_id,
           (short)x,(short)y,(short)dx,(short)dy,
           (short)1,"V{nta...","V{nta...","",
           (short)WP_BGND,(short)WP_FGND,&buttpt);

     XMapRaised(xdisp,buttpt->id.x_id);
     }
/*
***Skall vi rita eller sudda ?
*/
   if ( draw )
     {
     strcpy(wt_str,iggtts(164));
     sprintf(timbuf,"%d",n_secs);
     strcat(wt_str,timbuf);

     wpmaps(wt_str);
     strcpy(buttpt->stron,wt_str);
     strcpy(buttpt->stroff,wt_str);
     wpxpbu(buttpt);
     XFlush(xdisp);
     }
/*
***Om sudda begärts dödar vi fönstret.
*/
   else if ( buttpt != NULL )
     {
     XDestroyWindow(xdisp,buttpt->id.x_id);
     wpdlbu(buttpt);
     buttpt = NULL;
     }

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       short wpupwb(
       WPGWIN *gwinpt)

/*     Uppdaterar texten i ett WPGWIN-fönsters ram.
 *
 *     In: gwinpt => C-pekare till grafiskt fönster
 *                   eller NULL för huvudfönstret.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 28/1/95 J. Kjellander
 *
 *     2/9/95  varkon.title.project, J. Kjellander
 *     28/9/95 NULL = GWIN_MAIN, J. Kjellander
 *
 *******************************************************!*/

 {
   char      title[V3STRLEN+1],tmpbuf[V3STRLEN+1];
   WPGWIN   *grawin;
   WPWIN    *winptr;
/*
***Initiering.
*/
   title[0] = '\0';
/*
***Fixa fram fönstrets C-pekare.
*/
   if ( gwinpt == NULL )
     {
     if ( (winptr=wpwgwp((wpw_id)GWIN_MAIN)) != NULL  &&
         winptr->typ == TYP_GWIN ) grawin = (WPGWIN *)winptr->ptr;
     else return(0);
     }
   else grawin = gwinpt;
/*
***Om det är huvudfönstret inleder vi med en särskild titel-resurs.
*/
   if ( grawin->id.w_id == GWIN_MAIN )
     {
     if ( !wpgrst("varkon.title",title) )
       {
       if ( sydata.opmode == BAS_MOD )
         sprintf(title,"VARKON-3D/B %d.%d%c",sydata.vernr,sydata.revnr,
                                             sydata.level);
       else
         sprintf(title,"VARKON-3D/R %d.%d%c",sydata.vernr,sydata.revnr,
                                             sydata.level);
       }
/*
***Om varkon.title.project är satt skriver vi även ut projektnamn i 
***huvudförnstret.
*/
     if ( wpgrst("varkon.title.project",tmpbuf) &&
          strcmp(tmpbuf,"True") == 0 )
         {
         strcat(title," - ");
         strcat(title,pidnam);
         }
     }
/*
***Alla fönster kan ha jobnamn och/eller vynamn.
*/  
   if ( wpgrst("varkon.title.jobname",tmpbuf) &&
        strcmp(tmpbuf,"True") == 0 )
       {
       if ( grawin->id.w_id == GWIN_MAIN )
         {
         strcat(title," - ");
         strcat(title,jobnam);
         }
         else strcpy(title,jobnam);
       }

   if ( wpgrst("varkon.title.viewname",tmpbuf) &&
        strcmp(tmpbuf,"True") == 0 )
       {
       if ( title[0] != '\0' )             strcat(title," - ");
       if ( grawin->vy.vynamn[0] == '\0' ) strcat(title,"*****");
       else                                strcat(title,grawin->vy.vynamn);
       }
/*
***Mappa till svenska tecken.
*/
   wpmaps(title);
/*
***Uppdatera fönsterramen.
*/
   XStoreName(xdisp,grawin->id.x_id,title);

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       short wpposw(
       int  rx,
       int  ry,
       int  dx,
       int  dy,
       int *px,
       int *py)

/*     Placerar ett fönster så nära en önskad punkt som
 *     möjligt utan att det kommer ut utanför skärmen.
 *
 *     In: rx,ry = Önskad position relativt root-fönstret.
 *         dx,dy = Fönstrets storlek.
 *         px,py = Pekare till utdata.
 *
 *     Ut: *px,*py = Ny position.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *******************************************************!*/

 {
   int x,y,width,height;

   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);
/*
***I sidled.
*/
   x = rx;
   if ( x + dx > width ) x = width - dx;
   if ( x < 0 ) x = 0;
/*
***I höjdled.
*/
   y = ry;
   if ( y + dy > height ) y = height - dy;
   if ( y < 0 ) y = 0;

  *px = x;
  *py = y;

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

     short wpwlma(
     char *s)

/*   Skriver ut meddelande i meddelandefönster.
 *
 *   In: s  = Sträng
 *
 *   Ut: Inget.
 *
 *   FV: 0
 *
 *   (C)microform ab 31/1/95 J. Kjellander
 *
 *******************************************************!*/

 {
   int     x,y,dx,dy;
   WPWIN  *winptr;
   WPGWIN *mainpt;

/*
***Om det inte finns något grafiskt fönster gör vi intet.
***Annars en C-pekare till V3:s huvudfönster.
*/
   if ( (winptr=wpwgwp((wpw_id)GWIN_MAIN)) != NULL  &&
         winptr->typ == TYP_GWIN ) mainpt = (WPGWIN *)winptr->ptr;
   else return(0);
/*
***Mappa till ISO-8859.
*/
   wpmaps(s);
/*
***Om mesbpt != NULL finns gammalt meddelande att döda.
*/
   if ( mainpt->mesbpt != NULL )
     {
     XDestroyWindow(xdisp,(mainpt->mesbpt)->id.x_id);
     wpdlbu(mainpt->mesbpt);
     mainpt->mesbpt = NULL;
     }
/*
***Om tom sträng går det fort. Detta är detsamma som sudda.
*/
   if ( strlen(s) == 0 ) return(0);
/*
***Skapa knapp för nya meddelandet.
*/
   dx = wpstrl(s) + 15;
   dy = (int)(1.6*(double)wpstrh());
   x  = (int)(mainpt->vy.scrwin.xmin + 10);
   y  = (int)(mainpt->geo.dy - mainpt->vy.scrwin.ymin - dy - 10);

   wpwcbu(mainpt->id.x_id,
         (short)x,(short)y,(short)dx,(short)dy,
         (short)1,s,s,"",
         (short)WP_BGND,(short)WP_FGND,&mainpt->mesbpt);

   XMapRaised(xdisp,(mainpt->mesbpt)->id.x_id);
   wpxpbu(mainpt->mesbpt);
   XFlush(xdisp);
/*
***Slut.
*/
   return(0);
 }

/*********************************************************/
