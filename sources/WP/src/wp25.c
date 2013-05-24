/**********************************************************************
*
*    wp25.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrtx();    Draw text
*    wpdltx();    Delete text
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
#include "../../GP/include/GP.h"
#include "../include/WP.h"

extern short    actpen;

static short drawtx(WPGWIN *gwinpt, GMTXT *txtpek, char *strpek,
                    DBptr la, bool draw);

/*!******************************************************/

        short wpdrtx(
        GMTXT  *txtpek,
        char   *strpek,
        DBptr   la,
        v2int   win_id)

/*      Ritar en text.
 *
 *      In: txtpek => Pekare till text-post.
 *          strpek => Pekare till sträng.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 22/1/95 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander

 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om texten är blankad är det enkelt.
*/
   if ( txtpek->hed_tx.blank) return(0);
/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi rita i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, ligger texten på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,txtpek->hed_tx.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( txtpek->hed_tx.pen != actpen ) wpspen(txtpek->hed_tx.pen);
           if ( txtpek->wdt_tx != 0.0 ) wpswdt(gwinpt->id.w_id,txtpek->wdt_tx);
/*
***Sen är det bara att rita.
*/
           drawtx(gwinpt,txtpek,strpek,la,TRUE);
           if ( txtpek->wdt_tx != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdltx(
        GMTXT  *txtpek,
        char   *strpek,
        DBptr   la,
        v2int   win_id)

/*      Suddar en text.
 *
 *      In: txtpek => Pekare till text-post.
 *          strpek => Pekare till sträng.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 21/1/95 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander

 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi sudda i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, sudda ur DF.
*/
         if ( wpfobj(gwinpt,la,TXTTYP,&typ) ) wprobj(gwinpt);
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !wpnivt(gwinpt,txtpek->hed_tx.level)  ||
                               txtpek->hed_tx.blank) return(0);
           else
             {
             if ( txtpek->wdt_tx != 0.0 ) wpswdt(gwinpt->id.w_id,txtpek->wdt_tx);
             drawtx(gwinpt,txtpek,strpek,la,FALSE);
             if ( txtpek->wdt_tx != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
             }
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawtx(
        WPGWIN *gwinpt,
        GMTXT  *txtpek,
        char   *strpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar en text i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          txtpek => Pekare till text-post.
 *          strpek => Pekare till sträng.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    k;

/*
***Ställ om grapac:s vy så att detta fönster gäller.
*/
   wpfixg(gwinpt);
/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   gppltx(txtpek,(unsigned char *)strpek,&k,x,y,z,a);
   gpprtx(txtpek,x,y,z,k+1);
/*
***Återställ grapac:s ursprungliga vy igen.
*/
   wpfixg(NULL);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( wpcply(gwinpt,-1,&k,x,y,a) )
     {
     if ( draw  &&  txtpek->hed_tx.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,TXTTYP) ) wpdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else wpdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
