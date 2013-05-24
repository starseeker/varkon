/**********************************************************************
*
*    WPwait.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPintr();   Interrupt routine
*    WPlset();   Sets alarm signal
*    WPlinc();   Catches alarm signal
*    WPwait();   Handles Wait...
*    WPwton();   Returns wait status On/Off
*    WPposw();   Positions windows
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
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define WP_INTTIME 2       /* Antal sekunder mellan alarm */

static int     n_larm = 0;
static int     n_secs = 0;
static bool    wt_on  = FALSE;
static WPGWIN *wt_win = NULL;

/* n_larm s�tt till noll av WPwait(xxx,TRUE) och
   r�knas upp av WPlinc() samt ner av WPintr(). 
   n_secs s�tt ocks� till noll men r�knas inte ner.
   wt_on �r TRUE om v�nta �r p�slaget av WPwait() annars FALSE.
   wt_win �r en pekare till det f�nster d�r drwait()
   skall visa v�ntameddelandet. */

extern MNUALT smbind[];

/*!*******************************************************/

       bool WPintr()

/*     wp:s interrupt-rutin. Anropas med j�mna mellanrum
 *     av alla rutiner som kan ta l�ng tid p� sig. Sk�ter
 *     uppdatering av v�nta-meddelandet om v�nta �r p� samt
 *     pollar event-k�n efter <CTRL>c.
 *
 *     In: Inget.
 *
 *     Ut: Inget.
 *
 *     FV: TRUE  = V3 skall avbrytas.
 *         FALSE = Forts�tt exekveringen.
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
***Om n_larm > 0 �r v�nta-hanteringen p�slagen och det har
***g�tt tillr�ckligt med tid sen vi sist uppdaterade v�nta-
***meddelandet. Allts� uppdaterar vi igen. Om n_larm == 0
***har det antingen inte g�tt tillr�ckligt l�ng tid eller
***ocks� �r inte v�nta-hanteringen p�slagen �verhuvudtaget.
*/
   if ( n_larm > 0 )
     {
     drwait(TRUE);
   --n_larm;
/*
***Samtidigt som vi uppdaterar v�nta-meddelandet passar vi
***p� att polla eventk�n efter <CTRL>c.
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

       short WPlset(
       bool set)

/*     Best�ller en alarmsignal om en liten stund eller
 *     d�dar en redan best�lld.
 *
 *     In: set => TRUE  = Best�ll signal.
 *                FALSE = D�da best�lld.
 *
 *     Ut: Inget.
 *
 *     FV: 0
 *
 *     (C)microform ab 17/1/95 J. Kjellander
 *
 *******************************************************!*/

 {


#ifdef DEBUG
return(0);
#endif

/*
***Om det �r f�rsta g�ngen v�ntar vi lite l�ngre sen
***k�r vi p� som vanligt.
*/
   if ( set )
     {
     signal(SIGALRM,WPlinc);
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

       void WPlinc()

/*     F�ngar en alarmsignal best�lld av WPlset()
 *     samt best�ller en ny.
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
***R�kna upp larm- och sekund-r�knarna.
*/
   ++n_larm;
   n_secs += WP_INTTIME;
/*
***Best�ll ett nytt larm om en stund igen.
*/
   WPlset(TRUE);

   return;
 }

/*********************************************************/
/*!*******************************************************/

       bool WPwton()

/*     Returnerar v�rdet av wt_on. Anv�nds av rutiner
 *     som tillf�lligt vill st�nga av interruptgenereringen.
 *     Tex. exos(). Om WPwton() == TRUE g�r man d� f�rst
 *     WPlset(FALSE) och senare wplset(TRUE) igen.
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

       short WPwait(
       DBint win_id,
       bool  wait)

/*     Sl�r p� eller av v�nta-hanteringen.
 *
 *     In: win_id => F�nster-ID eller GWIN_ALL.
 *         wait   => TRUE = sl� p�, FALSE = st�ng av.
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
***Om det �r GWIN_ALL som g�ller handlar det om att v�nta
***p� n�got som inte h�nder i ett s�rskilt f�nster utan
***i alla f�nster eller inte i n�got f�nster alls. Is�fall
***visar vi v�nta... i huvudf�nstret.
*/
   if ( wait )
     {
     if ( win_id == GWIN_ALL )
       {
       winptr = WPwgwp((wpw_id)GWIN_MAIN);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Om det handlar om att v�nta i ett visst f�nster fixar vi
***en C-pekare till just det f�nstret.
*/
     else
       {
       winptr = WPwgwp((wpw_id)win_id);
       wt_win = (WPGWIN *)winptr->ptr;
       }
/*
***Eftersom v�nta skall sl�s p� nollst�ller vi lite
***och startar best�llning av alarmsignaler.
*/
     n_larm = n_secs = 0;
     WPlset(TRUE);
     }
/*
***Om v�nta skall sl�s av st�nger vi av best�llningen av
***alarmsignaler och nollst�ller larmr�knaren igen s� att
***WPintr() inte servar interrupt i on�dan om alla inte
***har hunnit servas medan v�nta var p�.
*/
   else
     {
     drwait(FALSE);
     n_larm = 0;
     WPlset(FALSE);
     }
/*
***Kom ih�g om vi �r p� eller av. Anv�nds av WPwton().
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

/*     Ritar v�nta....
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
***Om buttpt == NULL �r det f�rsta g�ngen och d� m�ste vi skapa
***sj�lva v�nta-f�nstret.
*/
   if ( draw  &&  buttpt == NULL )
     {
     dx = WPstrl("V�nta...XXXXX") + 15;
     dy = (int)(1.6*(double)WPstrh());
     x  = (int)(wt_win->vy.scrwin.xmax - dx - 10);
     y  = (int)(wt_win->geo.dy - wt_win->vy.scrwin.ymin - dy - 10);

     WPwcbu(wt_win->id.x_id,
           (short)x,(short)y,(short)dx,(short)dy,
           (short)1,"V{nta...","V{nta...","",
           (short)WP_BGND2,(short)WP_FGND,&buttpt);

     XMapRaised(xdisp,buttpt->id.x_id);
     }
/*
***Skall vi rita eller sudda ?
*/
   if ( draw )
     {
     strcpy(wt_str,IGgtts(164));
     sprintf(timbuf,"%d",n_secs);
     strcat(wt_str,timbuf);

     strcpy(buttpt->stron,wt_str);
     strcpy(buttpt->stroff,wt_str);
     WPxpbu(buttpt);
     XFlush(xdisp);
     }
/*
***Om sudda beg�rts d�dar vi f�nstret.
*/
   else if ( buttpt != NULL )
     {
     XDestroyWindow(xdisp,buttpt->id.x_id);
     WPdlbu(buttpt);
     buttpt = NULL;
     }

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

       short WPposw(
       int  rx,
       int  ry,
       int  dx,
       int  dy,
       int *px,
       int *py)

/*     Placerar ett f�nster s� n�ra en �nskad punkt som
 *     m�jligt utan att det kommer ut utanf�r sk�rmen.
 *
 *     In: rx,ry = �nskad position relativt root-f�nstret.
 *         dx,dy = F�nstrets storlek.
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
***I h�jdled.
*/
   y = ry;
   if ( y + dy > height ) y = height - dy;
   if ( y < 0 ) y = 0;

  *px = x;
  *py = y;

   return(0);
 }

/*********************************************************/
