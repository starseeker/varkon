/**********************************************************************
*
*    wp22.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrar();     Draw arc
*    wpdlar();     Delete arc
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

static short drawar(WPGWIN *gwinpt, GMARC *arcpek, GMSEG *segpek,
                    DBptr la, bool draw);

/*!******************************************************/

        short wpdrar(
        GMARC  *arcpek,
        GMSEG  *segpek,
        DBptr   la,
        v2int   win_id)

/*      Ritar en arc.
 *
 *      In: arcpek => Pekare till arc-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-27 Breda linjer,J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om cirkeln är blankad gör vi inget alls !
*/
   if ( arcpek->hed_a.blank) return(0);
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
***Ja, ligger cirkel på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,arcpek->hed_a.level) )
           {
/*
***Ja. Kolla att rätt färg och bredd är inställd.
*/
           if ( arcpek->hed_a.pen != actpen ) wpspen(arcpek->hed_a.pen);
           if ( arcpek->wdt_a != 0.0 ) wpswdt(gwinpt->id.w_id,arcpek->wdt_a);
/*
***Sen är det bara att rita.
*/
           drawar(gwinpt,arcpek,segpek,la,TRUE);
           if ( arcpek->wdt_a != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlar(
        GMARC  *arcpek,
        GMSEG  *segpek,
        DBptr   la,
        v2int   win_id)

/*      Suddar en arc.
 *
 *      In: arcpek => Pekare till arc-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-27 Breda linjer,J.Kjellander
 *
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
***Ja. Stryk ur DF.
*/
         if ( wpfobj(gwinpt,la,ARCTYP,&typ) ) wprobj(gwinpt);
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda från skärm.
*/
         if ( !wpnivt(gwinpt,arcpek->hed_a.level)  ||
                             arcpek->hed_a.blank) return(0);
           
         if ( arcpek->wdt_a != 0.0 ) wpswdt(gwinpt->id.w_id,arcpek->wdt_a);
         drawar(gwinpt,arcpek,segpek,la,FALSE);
         if ( arcpek->wdt_a != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawar(
        WPGWIN *gwinpt,
        GMARC  *arcpek,
        GMSEG  *segpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar en grafisk cirkel i ett visst fönster.
 *      Vid ritning lagras samtidigt objektet i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          arcpek => Pekare till arc-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 19/2/94 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV];
   char   a[PLYMXV];

   int    k;

/*
***Sätt upp grapac.
*/
   wpfixg(gwinpt);
/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   gpplar(arcpek,segpek,&k,x,y,a);
/*
***Återställ grapac.
*/
   wpfixg(NULL);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( wpcply(gwinpt,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  arcpek->hed_a.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,ARCTYP) ) wpdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else wpdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
