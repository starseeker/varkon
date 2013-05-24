/**********************************************************************
*
*    ms5.c
*    =====
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msintr();   Handles interrupt
*     mslset();   Sets alarm signal
*     mslinc();   Catch alarm signal
*     mswait();   Handles Wait....
*
*     mswlma();   Write line in message area
*     msrpma();   Repaint message area
*     msupwb();   Update window borders
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
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"


static int drwait(); /* Visar vänta... */

static char messtr[V3STRLEN+1];
static HWND meswin = NULL;

/* meswin är meddelandefönstret och messtr är aktivt meddelande.
   Fönstret skapas av mswlma() och "repaintas" av msrpma() */

#define WP_INTTIME 2000       /* Antal sekunder mellan alarm */

static int     n_larm;
static int     n_secs;
static WPGWIN *wt_win;


/* n_larm sätt till noll av wpwait(xxx,TRUE) och
   räknas upp av wplinc() samt ner av wpintr(). 
   n_secs sätt också till noll men räknas inte ner.
   wt_win är en pekare till det fönster där drwait()
   skall visa väntameddelandet. */


extern char        pidnam[],jobnam[];
extern V3MDAT      sydata;


#ifdef senare
/*!*******************************************************/

       bool msintr()

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
#ifdef SENARE
     while ( XCheckMaskEvent(xdisp,KeyPressMask,keyev) )
       {
       if ( keyev->state & ControlMask  &&
            XLookupString(keyev,tknbuf,10,&keysym,&costat) == 1   &&
            tknbuf[0] == *smbind[11].str )
         {
         XBell(xdisp,100);
         return(TRUE);
         }
       }
#endif
     }

   return(FALSE);
 }

/*********************************************************/
/*!*******************************************************/

       int  mslset(set)
       bool set;

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
     if ( n_secs == 0 )
       {
       n_secs = WP_INTTIME;
       SetTimer(ms_main,NULL,2*WP_INTTIME,mslinc);
       }
     else SetTimer(ms_main,NULL,WP_INTTIME,mslinc);
     }

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

 VOID CALLBACK mslinc()

/*     Fångar en alarmsignal beställd av mslset()
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

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       int   mswait(win_id,wait)
       v2int win_id;
       bool  wait;

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
 *******************************************************!*/

 {
   WPWIN  *winptr;

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
       winptr = mswgwp((wpw_id)GWIN_MAIN);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Om det handlar om att vänta i ett visst fönster fixar vi
***en C-pekare till just det fönstret.
*/
     else
       {
       winptr = mswgwp((wpw_id)win_id);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Eftersom vänta skall slås på nollställer vi lite
***och startar beställning av alarmsignaler.
*/
     n_larm = n_secs = 0;
     mslset(TRUE);
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
     mslset(FALSE);
     }

   return(0);
 }

/*********************************************************/

/*!*******************************************************/

static int   drwait(draw)
       bool  draw;

/*     Ritar vänta....
 *
 *     In: draw => TRUE = Sudda+rita. FALSE = Bara sudda.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 25/11/95 J. Kjellander
 *
 *******************************************************!*/

 {
   int     x,y,dx,dy;
   char    wt_str[V3STRLEN+1];
   char    timbuf[V3STRLEN+1];

static  WPBUTT *buttpt = NULL;

/*
***Om buttpt == NULL är det första gången och då måste vi skapa
***själva vänta-fönstret.
*
   if ( draw  &&  buttpt == NULL )
     {
     dx = msstrl("Vänta...XXXXX") + 15;
     dy = (int)(1.6*(double)msstrh());
     x  = (int)(wt_win->vy.scrwin.xmax - dx - 10);
     y  = (int)(wt_win->geo.dy - wt_win->vy.scrwin.ymin - dy - 10);

     mswcbu(wt_win->id.ms_id,x,y,dx,dy,1,"Vänta...","Vänta...","",
                                            WP_BGND,WP_FGND,&buttpt);
     }
/*
***Skall vi rita eller sudda ?
*/
   if ( draw )
     {
     strcpy(wt_str,"Vänta...");
     sprintf(timbuf,"%d",n_secs);
     strcat(wt_str,timbuf);

     strcpy(buttpt->stron,wt_str);
     strcpy(buttpt->stroff,wt_str);
     }
/*
***Om sudda begärts dödar vi fönstret.
*/
   else if ( buttpt != NULL )
     {
     msdlbu(buttpt);
     buttpt = NULL;
     }

   return(0);
 }

/*********************************************************/
#endif
/*!*******************************************************/

       int     msupwb(gwinpt)
       WPGWIN *gwinpt;

/*     Uppdaterar texten i ett WPGWIN-fönsters ram.
 *
 *     In: gwinpt => C-pekare till grafiskt fönster
 *                   eller NULL för huvudfönstret.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 7/11/95 J. Kjellander
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
     if ( (winptr=mswgwp((wpw_id)GWIN_MAIN)) != NULL  &&
         winptr->typ == TYP_GWIN ) grawin = (WPGWIN *)winptr->ptr;
     else return(0);
     }
   else grawin = gwinpt;
/*
***Applikationsfönstrets titel. Finns resurs, använd den annars
***default titel med versionsnummer mm.
*/
   if ( !msgrst("APPWIN.TITLE",title) )
     {
     if ( sydata.opmode == BAS_MOD )
       sprintf(title,"VARKON %d.%d%c",sydata.vernr,sydata.revnr,
                                           sydata.level);
     else
       sprintf(title,"VARKON-R %d.%d%c",sydata.vernr,sydata.revnr,
                                           sydata.level);
     }
/*
***Skall projektnamn skrivas ut ?
*/
   if ( msgrst("APPWIN.TITLE.PROJECT",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     strcat(title," - ");
     strcat(title,pidnam);
     }
/*
***Samma med jobnamn.
*/
   if ( msgrst("APPWIN.TITLE.JOBNAME",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     strcat(title," - ");
     strcat(title,jobnam);
     }

   SetWindowText(ms_main,title);
/*
***WPGWIN-fönstrets titel.
*/
   if ( grawin->vy.vynamn[0] == '\0' ) strcpy(title,"*****");
   else                                strcpy(title,grawin->vy.vynamn);

   SetWindowText(grawin->id.ms_id,title);

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

     int   mswlma(s)
     char *s;

/*   Skriver ut meddelande i meddelandefönster.
 *
 *   In: s  = Sträng
 *
 *   Ut: Inget.
 *
 *   FV: 0
 *
 *   (C)microform ab 7/11/95 J. Kjellander
 *
 *   1996-12-15 Lite uppfräschning, J.Kjellander
 *
 *******************************************************!*/

 {
   int    x,y,dx,dy;
   RECT   cr;

/*
***Om meswin != NULL finns gammalt meddelandefönster att döda.
*/
   if ( meswin != NULL )
     {
     DestroyWindow(meswin);
     meswin = NULL;
     }
/*
***Spara texten för repaint.
*/
   strcpy(messtr,s);
/*
***Om tom sträng går det fort. Detta är detsamma som sudda.
*/
   if ( strlen(s) == 0 ) return(0);
/*
***Skapa fönster för det nya meddelandet. Placering i Y-led = 15 + 1
***pixels upp från underkanten. Höjd = 15, Skugga = 1.
***Hur stort är fönstret ? Vid uppstart finns ingen meny !!!!
*/
   GetClientRect(ms_main,&cr);

   x  = 5;
   y  = cr.bottom - 16;

   dx = msstrl(s);
   dy = 15;

   meswin = CreateWindow(VARKON_IWIN_CLASS,"",WS_CHILD,
		      				 x,y,dx,dy,ms_main,NULL,ms_inst,NULL);
   ShowWindow(meswin,SW_SHOWNORMAL);
   UpdateWindow(meswin);
/*
***Slut.
*/
   return(0);
 }

/*********************************************************/
/*!*******************************************************/

     bool msrpma(win32_id)
     HWND win32_id;

/*   Repaintrutin för meddelandefönster.
 *
 *   In: win32_id = Fönster id.
 *
 *   Ut: Inget.
 *
 *   FV: TRUE  = win32_id är detta fönster.
 *       FALSE = Det gällde inte meddelandefönstret.
 *
 *   (C)microform ab 7/11/95 J. Kjellander
 *
 *   1996-12-15 Lite uppfräschning, J.Kjellander
 *
 *******************************************************!*/

 {
   int         x,y;
   HDC         dc;
   PAINTSTRUCT ps;
   TEXTMETRIC  tm;

/*
***Gäller det detta fönster ?
*/
   if ( win32_id != meswin ) return(FALSE);
/*
***Fönstrets DC.
*/
   dc = BeginPaint(meswin,&ps);
/*
***Beräkna textens läge. Tills vidare använder vi font 0.
*/
   SelectObject(dc,msgfnt(0));
   GetTextMetrics(dc,&tm);

   x = 0;  
   y = 15 - tm.tmDescent - 2;
/*
***Färg 7 är ok.
*/
   SetBkColor(dc,PALETTEINDEX(7));             /* 7 = Grå */
   SetTextColor(dc,PALETTEINDEX(0));           /* 1 = Svart */
/*
***Skriv ut.
*/
   mswstr(dc,x,y,messtr);
/*
***Släpp DC't.
*/
   EndPaint(meswin,&ps);
/*
***Slut.
*/
   return(TRUE);
 }

/*********************************************************/
