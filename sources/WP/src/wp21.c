/**********************************************************************
*
*    wp21.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrli();    Draw line
*    wpdlli();    Delete line
*    wptrli();    Transform line to view
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

extern short  actpen;


static short drawli(WPGWIN *gwinpt, GMLIN *linpek, WPGRPT *pt1,
                    WPGRPT *pt2, bool draw);

/*!******************************************************/

        short wpdrli(
        GMLIN  *linpek,
        DBptr   la,
        v2int   win_id)

/*      Ritar en linje.
 *
 *      In: linpek => Pekare till linje-post.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   double  x[2],y[2];
   char    a[2];
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPGRPT  pt1,pt2;

/*
***Om linjen är blankad behöver vi inte göra nånting.
*/
   if ( linpek->hed_l.blank) return(0);
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
***Ja, är den nivå som punkten ligger på tänd i detta fönster.
*/
         if ( wpnivt(gwinpt,linpek->hed_l.level) )
           {
/*
***Ja, då transformerar vi linjen (GMLIN) till fönstrets vyplan.
***Resultatet blir 2 WPGRPT.
***Därefter klipper vi och om linjen till någon del nu är
***synlig på skärmen genererar vi vektorer och ritar.
*/
           if ( wptrli(gwinpt,linpek,&pt1,&pt2)  &&
                wpclin(gwinpt,&pt1,&pt2) >= 0 )
             {
/*
***Rita linjen på skärmen.
*/
             if ( linpek->hed_l.pen != actpen ) wpspen(linpek->hed_l.pen);
             if ( linpek->wdt_l != 0.0 ) wpswdt(gwinpt->id.w_id,linpek->wdt_l);
             drawli(gwinpt,linpek,&pt1,&pt2,TRUE);
             if ( linpek->wdt_l != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
/*
***Om linjen skall vara pekbar lagrar vi den i DF,
*/
             if ( linpek->hed_l.hit )
               {
               x[0] = pt1.x; y[0] = pt1.y;
               x[1] = pt2.x; y[1] = pt2.y;
               a[0] = 0;     a[1] = VISIBLE | ENDSIDE;
               if ( !wpsply(gwinpt,(short)1,x,y,a,la,LINTYP) )
                 return(erpush("GP0012",""));
               }
             }
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlli(
        GMLIN  *linpek,
        DBptr   la,
        v2int   win_id)

/*      Suddar en linje.
 *
 *      In: linpek => Pekare till linje-post.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPGRPT  pt1,pt2;

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
***Ja. Stryk den ur DF om den finns där.
*/
         if ( wpfobj(gwinpt,la,LINTYP,&typ) ) wprobj(gwinpt);
/*
***Om den ligger på en släckt nivå eller är blankad gör vi inget mer.
*/
         if ( !wpnivt(gwinpt,linpek->hed_l.level) ||
                             linpek->hed_l.blank) return(0);
/*
***Annars suddar vi från skärmen också.
*/
         if ( wptrli(gwinpt,linpek,&pt1,&pt2)  &&
              wpclin(gwinpt,&pt1,&pt2) >= 0 )
            {
            if ( linpek->wdt_l != 0.0 ) wpswdt(gwinpt->id.w_id,linpek->wdt_l);
            drawli(gwinpt,linpek,&pt1,&pt2,FALSE);
            if ( linpek->wdt_l != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
            }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        bool wptrli(
        WPGWIN *gwinpt,
        GMLIN  *linpek,
        WPGRPT *pt1,
        WPGRPT *pt2)

/*      Transformerar en linje till ett fönsters vyplan.
 *
 *      In: gwinpt  => Pekare till fönster.
 *          linpek  => Pekare till linje-post.
 *          pt1,pt2 => Pekare till utdata.
 *
 *      Ut: *pt1,*pt2 = Ändpunkter transformerade.
 *
 *      FV: TRUE  => Ok.
 *          FALSE => Z-klipp.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( wptrpo(gwinpt,&linpek->crd1_l,pt1)  &&
        wptrpo(gwinpt,&linpek->crd2_l,pt2) )
     return(TRUE);
   else
     return(FALSE);
 }

/********************************************************/
/*!******************************************************/

        static short drawli(
        WPGWIN *gwinpt,
        GMLIN  *linpek,
        WPGRPT *pt1,
        WPGRPT *pt2,
        bool    draw)

/*      Ritar en grafisk linje i ett visst fönster.
 *
 *      In:  gwinpt => Pekare till fönster.
 *           linpek => Pekare till linje-post.
 *           pt1    => Pekare till klippt startpunkt.
 *           pt2    => Pekare till klippt slutpunkt.
 *           draw   => TRUE = Rita, FALSE = Sudda
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
***Fixa till "activ vy" i grapac.
*/
   wpfixg(gwinpt);
/*
***Skapa grafisk representation.
*/
   k    = -1;
   x[0] = pt1->x; y[0] = pt1->y; a[0] = 0;
   x[1] = pt2->x; y[1] = pt2->y; a[1] = 1;
   gpplli(linpek,&k,x,y,a);
/*
***Återställ grapac.
*/
   wpfixg(NULL);
/*
***Rita/sudda (poly-)linjen.
*/
   wpdply(gwinpt,k,x,y,a,draw);

   return(0);
 }

/***********************************************************/
