/**********************************************************************
*
*    WPgrid.c
*    ========
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    WPdrrs();    Display grid
*    WPdlrs();    Undisplay grid
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
#include "../../WP/include/WP.h"
#include <math.h>

#ifdef V3_X11
extern Display *xdisp;
extern Window   xgwin; 
extern GC       gpxgc;
extern Pixmap   xgmap;
#endif

static void drwrs(WPGWIN *gwinpt, double ox, double oy, double dx, double dy);

/*!******************************************************/

        short WPdrrs(
        double ox,
        double oy,
        double dx,
        double dy)

/*      Display grid.
 *
 *      In: ox,oy => Origin in model coordinates
 *          dx,dy => Spacing
 *
 *      Return: 0
 *
 *      (C)2006-12-30 J.Kjellander
 *
 ******************************************************!*/

{
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Use pen 1.
*/
    WPspen(1);
/*
***Don't draw if the grid is too dense ie, if the number
***of empty pixels between 4 dots < 15.
*/
    if ( dx*gwinpt->vy.k2x*dy*gwinpt->vy.k2y > 15 )
      {
      drwrs(gwinpt,ox,oy,dx,dy);
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short WPdlrs(
        double ox,
        double oy,
        double dx,
        double dy)

/*      Undisplay grid.
 *
 *      In: ox,oy => Origin in model coordinates
 *          dx,dy => Spacing
 *
 *      Return: 0
 *
 *      (C)2006-12-30 J.Kjellander
 *  
 ******************************************************!*/

{
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Undisplay = display with Pen 0.
*/
    WPspen(0);
/*
***Don't draw if too dense.
*/
    if ( dx*gwinpt->vy.k2x*dy*gwinpt->vy.k2y > 15 )
      {
      drwrs(gwinpt,ox,oy,dx,dy);
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

 static void    drwrs(
        WPGWIN *gwinpt,
        double  ox,
        double  oy,
        double  dx,
        double  dy)

/*      Draw/erase a grid in a window.
 *
 *      In: gwinpt => Ptr to window
 *          ox,oy  => Origin in model coordinates
 *          dx,dy  => Spacing
 *
 *      Return: 0
 *
 *      (C)2006-12-30 J.Kjellander
 *  
 ******************************************************!*/

{
     short  i,j,imax,jmax;
     short  x[PLYMXV],y[PLYMXV];
     double tt,xmax,ymax;

/*
***Check that dx and dy are valid numbers.
*/
     dx = ABS(dx);
     dy = ABS(dy);
     if ((dx == 0.0) || (dy == 0.0)) return;
/*
***Move the origin into the window.
*/     
     tt = (ox - gwinpt->vy.modwin.xmin)/dx;
     if ((tt = DEC(tt)) >= 0)
          ox = gwinpt->vy.modwin.xmin + tt*dx;
     else
          ox = gwinpt->vy.modwin.xmin + (1.0 - tt)*dx;

     tt = (oy - gwinpt->vy.modwin.ymin)/dy;
     if ((tt = DEC(tt)) >= 0)
          oy = gwinpt->vy.modwin.ymin + tt*dy;
     else
          oy = gwinpt->vy.modwin.ymin + (1.0 - tt)*dy;
/*
***Transformera till skärmkoordinater.
*/
     ox = gwinpt->vy.k1x + gwinpt->vy.k2x*ox; dx = gwinpt->vy.k2x*dx;
     oy = gwinpt->vy.k1y + gwinpt->vy.k2y*oy; dy = gwinpt->vy.k2y*dy;
/*
***Beräkna var vi skall sluta. -1 för att säkerställa att inte
***heltalsavrundning nedan ger en extra rad utanför skärmen.
***Ny metod 920304 JK.
*/
     xmax = gwinpt->vy.k1x + gwinpt->vy.k2x*gwinpt->vy.modwin.xmax - 1;
     ymax = gwinpt->vy.k1y + gwinpt->vy.k2y*gwinpt->vy.modwin.ymax - 1;
/*
***Generera rasterkoordinater.
*/
     tt = oy;
     do
       {  
       i = 0;
       while ((ox < xmax) && (i < PLYMXV))
         {
         x[ i++ ] = (short)ox;
         ox += dx; 
         }
       imax = i;

       oy = tt;
       do
         {
         j = 0;
         while ((oy < ymax) && (j < PLYMXV))
           {
           y[ j++ ] = (short)oy;
           oy += dy; 
           }
         jmax = j;

         for (i = 0; i < imax; i++)
           {
           for (j = 0; j < jmax; j++)
             {
#ifdef V3_X11
             XDrawPoint(xdisp,xgwin,gpxgc,(int)x[i],(int)(gwinpt->geo.dy-y[j]));
             XDrawPoint(xdisp,xgmap,gpxgc,(int)x[i],(int)(gwinpt->geo.dy-y[j]));
#endif
             }
           }
         } while (oy < ymax);
       } while (ox < xmax);
/*
***The end.
*/
     return;
}

/********************************************************/
