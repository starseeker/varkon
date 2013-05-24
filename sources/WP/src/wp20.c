/**********************************************************************
*
*    wp20.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrpo();    Draw point
*    wpdlpo();    Delete point
*    wptrpo();    Transform point to view
*    wpclpt();    Clip point to window
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

extern short  actpen;

static short drawpt(WPGWIN *gwinpt, WPGRPT *pt, bool draw);

/*!******************************************************/

        short wpdrpo(
        GMPOI *poipek,
        DBptr  la,
        v2int  win_id)

/*      Ritar en punkt i ett eller flera fönster.
 *
 *      In: poipek => Pekare till punkt-post.
 *          la     => GM-adress.
 *          win_id => Fönster-ID eller alla.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 19/12/94 J. Kjellander
 *
 *      1998-04-03 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   double  x[1],y[1];
   char    a[1];
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPGRPT  pt;

/*
***Om punkten är blankad behöver vi inte göra någonting.
*/
   if ( poipek->hed_p.blank ) return(0);
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
***Ja, är den nivå som punkten ligger på tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,poipek->hed_p.level) )
           {
/*
***Nu transformerar vi punkten (DBVector) till fönstrets vy.
***Resultatet dvs. en WPGRPT hamnar i pt. Sen provar vi att
***klippa och om punkten syns, ritar/suddar vi.
*/
           if ( wptrpo(gwinpt,&poipek->crd_p,&pt) && wpclpt(gwinpt,&pt) )
             {
/*
***Rätt penna och bredd.
*/
             if ( poipek->hed_p.pen != actpen ) wpspen(poipek->hed_p.pen);
             if ( poipek->wdt_p != 0.0 ) wpswdt(gwinpt->id.w_id,poipek->wdt_p);
             drawpt(gwinpt,&pt,TRUE);
             if ( poipek->wdt_p != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
/*
***Om pekbar, lagra i DF.
*/
             if ( poipek->hed_p.hit )
               {
               x[0] = pt.x;
               y[0] = pt.y;
               a[0] = 0;
               if ( !wpsply(gwinpt,(short)0,x,y,a,la,POITYP) )
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

        short wpdlpo(
        GMPOI *poipek,
        DBptr  la,
        v2int  win_id)

/*      Suddar en punkt ur ett eller flera fönster.
 *
 *      In: poipek => Pekare till punkt-post.
 *          la     => GM-adress.
 *          win_id => Fönster-ID eller alla.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 19/12/94 J. Kjellander
 *
 *      1998-04-03 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPGRPT  pt;

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
***Ja. Oavsett om punkten är synlig eller ej provar vi alltid
***att stryka den ur DF för säkerhets skull.
*/
         if ( wpfobj(gwinpt,la,POITYP,&typ) ) wprobj(gwinpt);
/*
***Om punkten ligger på en släckt nivå eller är blankad behöver
***vi inte göra något mer.
*/
         if ( !wpnivt(gwinpt,poipek->hed_p.level)  ||
                            poipek->hed_p.blank ) return(0);
/*
***Är den synlig suddar vi den även från skärmen.
*/
         if ( wptrpo(gwinpt,&poipek->crd_p,&pt) && wpclpt(gwinpt,&pt) )
           {
           if ( poipek->wdt_p != 0.0 ) wpswdt(gwinpt->id.w_id,poipek->wdt_p);
           drawpt(gwinpt,&pt,FALSE);
           if ( poipek->wdt_p != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

    return(0);
 }

/********************************************************/
/*!******************************************************/

        bool wptrpo(
        WPGWIN    *gwinpt,
        DBVector  *po,
        WPGRPT *pt)

/*      Transformerar en position till ett fönsters vyplan.
 *
 *      In:  gwinpt => Pekare till fönster.
 *           po     => Pekare till 3D position.
 *
 *      Ut: *pt     => Positionen transformerad.
 *
 *      FV: TRUE  => OK.
 *          FALSE => Z-klipp vid perspektivtransformation.
 *
 *      (C)microform ab 19/2/94 J. Kjellander
 *
 ******************************************************!*/

 {
   double d,dz;

/*
***Om det är en 3D-vy måste åtminstone X och Y-koordinaten
***transformeras.
*/
   if ( gwinpt->vy.vy_3D )
     {
     pt->x = gwinpt->vy.vymat.k11 * po->x_gm +
             gwinpt->vy.vymat.k12 * po->y_gm +
             gwinpt->vy.vymat.k13 * po->z_gm;

     pt->y = gwinpt->vy.vymat.k21 * po->x_gm +
             gwinpt->vy.vymat.k22 * po->y_gm +
             gwinpt->vy.vymat.k23 * po->z_gm;
/*
***Om det är en perspektivvy måste även Z-koordinater
***transformeras.
*/
     if ( gwinpt->vy.vydist > 0 )
       {
       pt->z = gwinpt->vy.vymat.k31 * po->x_gm +
               gwinpt->vy.vymat.k32 * po->y_gm +
               gwinpt->vy.vymat.k33 * po->z_gm;

       d  = gwinpt->vy.vydist;
       dz = d - pt->z;

       if ( dz > 0.0 )
         {
         pt->x /= dz; pt->x *= d;
         pt->y /= dz; pt->y *= d;
         }
       else return(FALSE);
       }
     }
/*
***Om det inte är en 3D-vy behövs ingen transformation.
*/
   else
     {
     pt->x = po->x_gm;
     pt->y = po->y_gm;
     }

   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

        bool wpclpt(
        WPGWIN *gwinpt,
        WPGRPT *pt)

/*      Klipper en grafisk punkt mot ett visst fönster.
 *
 *      In:  gwinpt => Pekare till fönster.
 *           pt     => Pekare till punkt.
 *
 *      Ut:  Inget.
 *
 *      FV: TRUE  => Punkten ligger i fönstret.
 *          FALSE => Punkten ligger på kanten eller utaför.
 *
 *      (C)microform ab 19/2/94 J. Kjellander
 *
 ******************************************************!*/

 {
     return ((pt->x > gwinpt->vy.modwin.xmin ) &&
             (pt->x < gwinpt->vy.modwin.xmax ) &&
             (pt->y > gwinpt->vy.modwin.ymin ) &&
             (pt->y < gwinpt->vy.modwin.ymax ));
 }

/********************************************************/
/*!******************************************************/

        static short drawpt(
        WPGWIN *gwinpt,
        WPGRPT *pt,
        bool    draw)

/*      Ritar en grafisk punkt i ett visst fönster.
 *
 *      In: gwinpt => Pekare till fönster.
 *          pt     => Pekare till punkt.
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
   int ix,iy;

   if ( !draw  &&  actpen != 0 ) wpspen(0);

   ix = (int)(gwinpt->vy.k1x + gwinpt->vy.k2x*pt->x);
   iy = (int)(gwinpt->geo.dy - (int)(gwinpt->vy.k1y + gwinpt->vy.k2y*pt->y));

#ifdef V3_X11
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix-2,iy+2,ix+2,iy-2);
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix+2,iy+2,ix-2,iy-2);

   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix-2,iy+2,ix+2,iy-2);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix+2,iy+2,ix-2,iy-2);
#endif

#ifdef WIN32
   MoveToEx(gwinpt->dc,ix-2,iy+2,NULL);
   LineTo(gwinpt->dc,ix+2,iy-2);
   MoveToEx(gwinpt->dc,ix+2,iy+2,NULL);
   LineTo(gwinpt->dc,ix-2,iy-2);

   MoveToEx(gwinpt->bmdc,ix-2,iy+2,NULL);
   LineTo(gwinpt->bmdc,ix+2,iy-2);
   MoveToEx(gwinpt->bmdc,ix+2,iy+2,NULL);
   LineTo(gwinpt->bmdc,ix-2,iy-2);
#endif

   return(0);
 }

/********************************************************/
