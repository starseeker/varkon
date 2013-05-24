/**********************************************************************
*
*    wp24.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrsu();    Draw surface
*    wpdlsu();    Delete surface
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

static short drawsu(WPGWIN *gwinpt, GMSUR *surpek, GMSEG *sptarr[],
                    DBptr la, bool draw);

/*!******************************************************/

        short wpdrsu(
        GMSUR  *surpek,
        GMSEG  *sptarr[],
        DBptr   la,
        v2int   win_id)

/*      Ritar en yta.
 *
 *      In: surpek => Pekare till sur-post.
 *          sptarr => Pekare till grafisk representation.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 22/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om ytan är blankad är det enkelt.
*/
   if ( surpek->hed_su.blank) return(0);
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
***Ja, ligger ytan på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,surpek->hed_su.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( surpek->hed_su.pen != actpen ) wpspen(surpek->hed_su.pen);
/*
***Sen är det bara att rita.
*/
           drawsu(gwinpt,surpek,sptarr,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlsu(
        GMSUR  *surpek,
        GMSEG  *sptarr[],
        DBptr   la,
        v2int   win_id)

/*      Suddar en yta.
 *
 *      In: surpek => Pekare till sur-post.
 *          sptarr => Pekare till grafisk representation.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 21/1/95 J. Kjellander
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
***Ja. Om den finns i DF kan vi sudda snabbt.
*/
         if ( wpfobj(gwinpt,la,SURTYP,&typ) )
           {
           wpdobj(gwinpt,FALSE);
           wprobj(gwinpt);
           }
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !wpnivt(gwinpt,surpek->hed_su.level)  ||
                               surpek->hed_su.blank) return(0);
           drawsu(gwinpt,surpek,sptarr,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawsu(
        WPGWIN *gwinpt,
        GMSUR  *surpek,
        GMSEG  *sptarr[],
        DBptr   la,
        bool    draw)

/*      Ritar/suddar en yta i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          surpek => Pekare till sur-post.
 *          sptarr => Pekare till GMSEG-arrayer.
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
   int  k;

/*
***Ställ om grapac:s vy så att detta fönster gäller.
*/
   wpfixg(gwinpt);
/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   gpplsu(surpek,sptarr,&k,x,y,z,a);
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
     if ( draw  &&  surpek->hed_su.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,SURTYP) ) wpdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else wpdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
