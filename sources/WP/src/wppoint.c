/**********************************************************************
*
*    wppoint.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPdrpo();    Display point
*    WPdlpo();    Erase point
*    WPplpt();    Create 3D polyline
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

extern short  actpen;

static short drawpt(WPGWIN *gwinpt, DBPoint *pointp, DBptr la, bool draw);

/*!******************************************************/

        short    WPdrpo(
        DBPoint *poipek,
        DBptr    la,
        DBint    win_id)

/*      Display a DBPoint in one or all WPGWIN.
 *
 *      In: poipek => C-ptr to DBPoint.
 *          la     => The point's DBptr.
 *          win_id => Window-ID or GWIN_ALL.
 *
 *      Return:  0 => Ok.
 *
 *      (C)microform ab 19/12/94 J. Kjellander
 *
 *      1998-04-03 WIDTH, J.Kjellander
 *      2006-12-08 Removed gpdrpo(), J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***If the point is BLANK we don't need to do anything.
*/
   if ( poipek->hed_p.blank ) return(0);
/*
***Loop through all WPGWIN-windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Is this a window that matches ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Yes, check if the corresponding LEVEL is unblanked.
*/
         if ( WPnivt(gwinpt,poipek->hed_p.level) )
           {
/*
***Set color and width.
*/
           if ( poipek->hed_p.pen != actpen ) WPspen(poipek->hed_p.pen);
           if ( poipek->wdt_p != 0.0 ) WPswdt(gwinpt->id.w_id,poipek->wdt_p);
/*
***Display.
*/
           drawpt(gwinpt,poipek,la,TRUE);
           if ( poipek->wdt_p != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPdlpo(
        DBPoint *poipek,
        DBptr    la,
        DBint    win_id)

/*      Erases a DBPoint from one or all WPGWIN.
 *
 *      In: poipek => C-ptr to DBPoint.
 *          la     => The point's DBptr.
 *          win_id => Window-ID or GWIN_ALL.
 *
 *      Return:  0 => Ok.
 *
 *      (C)microform ab 19/12/94 J. Kjellander
 *
 *      1998-04-03 WIDTH, J.Kjellander
 *      2006-12-08 Removed gpdlpo(), J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPGRPT  pt;

/*
***Loop through all WPGWIN-windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Is this a window that matches ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Yes. Without knowing if the point is visible or not we
***always try to remove it from DF for the sake of security.
*/
         if ( WPfobj(gwinpt,la,POITYP,&typ) ) WProbj(gwinpt);
/*
***If the point is BLANK or on a blanked LEVEL there is
***nothing more to do.
*/
         if ( !WPnivt(gwinpt,poipek->hed_p.level)  ||
                            poipek->hed_p.blank ) return(0);
/*
***Erase it from the display.
*/
         if ( poipek->wdt_p != 0.0 ) WPswdt(gwinpt->id.w_id,poipek->wdt_p);
         drawpt(gwinpt,poipek,la,FALSE);
         if ( poipek->wdt_p != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

    return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawpt(
        WPGWIN  *gwinpt,
        DBPoint *pointp,
        DBptr    la,
        bool     draw)

/*      Display or erase a graphical point in a WPGWIN.
 *
 *      In: gwinpt => C-ptr to WPGWIN.
 *          pointp => C-ptr to DBPoint.
 *          la     => Point adress in DB
 *          draw   => TRUE = Display, FALSE = Erase
 *
 *      Return: 0
 *
 *      (C)2006-12-28 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   double size;
   int    k;

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
***Point size = 1.5/scale.
*/
   size = 1.5/((gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
                gwinpt->geo.psiz_x /
               (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin));
/*
***Create graphical polyline representation.
*/
   k = -1;
   WPplpt(pointp,size,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Clip the polyline to the window borders.
***Display or erase visible parts.
*/
   if ( WPcply(gwinpt,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  pointp->hed_p.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,POITYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }
/*
***End.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPplpt(
        DBPoint *pointp,
        double   size,
        int     *n,
        double   x[],
        double   y[],
        double   z[],
        char     a[])

/*      Creates the graphical 3D polyline representation 
 *      for a point.
 *      
 *      In:  pointp  = C-ptr to DBPoint.
 *           size    = Size of point in model units.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-28 J.Kjellander
 *
 ******************************************************!*/

 {
    int    k;
    double d;
/*
***Initializations.
*/
    k = *n;
    d =  size/2.0;
/*
***Make a 3D cross.
*/
  ++k;
    x[k] = pointp->crd_p.x_gm - d;
    y[k] = pointp->crd_p.y_gm - d;
    z[k] = pointp->crd_p.z_gm;
    a[k] = 0;

  ++k;
    x[k] = pointp->crd_p.x_gm + d;
    y[k] = pointp->crd_p.y_gm + d;
    z[k] = pointp->crd_p.z_gm;
    a[k] = 1;

  ++k;
    x[k] = pointp->crd_p.x_gm + d;
    y[k] = pointp->crd_p.y_gm - d;
    z[k] = pointp->crd_p.z_gm;
    a[k] = 0;

  ++k;
    x[k] = pointp->crd_p.x_gm - d;
    y[k] = pointp->crd_p.y_gm + d;
    z[k] = pointp->crd_p.z_gm;
    a[k] = 1;

  ++k;
    x[k] = pointp->crd_p.x_gm;
    y[k] = pointp->crd_p.y_gm;
    z[k] = pointp->crd_p.z_gm + d;
    a[k] = 0;

  ++k;
    x[k] = pointp->crd_p.x_gm;
    y[k] = pointp->crd_p.y_gm;
    z[k] = pointp->crd_p.z_gm - d;
    a[k] = 1;
/*
***End.
*/
   *n = k;

    return(0);
 }

/********************************************************/
