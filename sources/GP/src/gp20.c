/**********************************************************************
*
*    gp20.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrrs();    Display grid
*    gpdlrs();    Undisplay grid
*    drwrs();     Draw grid
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
#include "../../WP/include/WP.h"
#include "../include/GP.h"
#include <math.h>

extern VY     actvy;
extern double k1x,k1y,k2x,k2y;
extern short  gptrty,gpsnpy;

#ifdef V3_X11
extern Display *xdisp;
extern Window   xgwin; 
extern GC       gpxgc;
extern Pixmap   xgmap;

#endif

/*!******************************************************/

        short gpdrrs(
        gmflt ox,
        gmflt oy,
        gmflt dx,
        gmflt dy)

/*      Tänder ett raster.
 *
 *      IN:
 *         ox,oy: Origo
 *         dx,dy: Indelning
 *
 *      UT: Inget.
 *
 *      FV: 0
 *
 *      (C) microform ab 11/2/86 J. Kjellander
 *
 *      1/11/88  CGI, J. Kjellander
 *      30/3/89  Koll av täthet, J. Kjellander
 *
 ******************************************************!*/

{

/*
***Raster ritas med penna 1 på monokroma skärmar.
*/
    gpspen(1);
/*
***Rita rastret, men bara under förutsätning
***att det inte blir så tätt att det är orimligt.
***För att man skall uppleva att rasterprickarna
***skiljer sig åt bör det vara minst 2 tomma pixels
***mellan två rasterprickar, dvs. delningen måste vara
***minst 3 pixels. Et rimligt villkor är tex. att produkten
***av rasterprickar i X- och Y-led är 15 pixels. På så
***sätt tar man hänsyn till delningen i båda riktningarna.
*/
    if ( dx*k2x*dy*k2y > 15 ) drwrs(ox,oy,dx,dy);

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpdlrs(
        gmflt ox,
        gmflt oy,
        gmflt dx,
        gmflt dy)

/*      Släcker ett raster.
 *
 *      IN:
 *         ox,oy: Origo
 *         dx,dy: Indelning
 *
 *      UT: Inget.
 *
 *      FV: 0
 *
 *      (C) microform ab 11/2/86 J. Kjellander
 *
 *      30/3/89  Koll av täthet, J. Kjellander
 *  
 ******************************************************!*/

{

/*
***Raster släcks genom att ritas med penna 0.
*/
    gpspen(0);
    if ( dx*k2x*dy*k2y > 15 ) drwrs(ox,oy,dx,dy);

    return(0);
}

/********************************************************/
/*!******************************************************/

        short drwrs(
        gmflt ox,
        gmflt oy,
        gmflt dx,
        gmflt dy)

/*      Ritar ett raster.
 *
 *      IN: ox,oy: Origo
 *          dx,dy: Indelning
 *
 *      UT: Inget.
 *
 *      FV: 0
 *
 *      (C) microform ab 28/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *  
 *     24/9-85  Bug, Ulf Johansson
 *     11/2/86  drwmrk, J. Kjellander
 *     26/2/86  Avsluta i ANSI-mode, J. Kjellander
 *     30/10/86 DEC för IBM, J. Kjellander
 *     9/1/87   Ny term. FT4600, R. Svedin
 *     7/12/87  Ny term. B. Doverud
 *     1/11/88  CGI, J. Kjellander
 *     4/3/92   Nytt stopp-kriterium, J. Kjellander
 *  
 ******************************************************!*/

{
     register short i,j;
     short imax,jmax;
     short x[ PLYMXV ];
     short y[ PLYMXV ];
     double tt,xmax,ymax;

/*
***Gör delningen positiv.
*/
     dx = ABS(dx);
     dy = ABS(dy);
     if ((dx == 0.0) || (dy == 0.0)) return(0);
/*
***Flytta in origo i fönstret.
*/     
     tt = (ox - actvy.vywin[ 0 ])/dx;
     if ((tt = DEC(tt)) >= 0)
          ox = actvy.vywin[ 0 ] + tt*dx;
     else
          ox = actvy.vywin[ 0 ] + (1.0 - tt)*dx;

     tt = (oy - actvy.vywin[ 1 ])/dy;
     if ((tt = DEC(tt)) >= 0)
          oy = actvy.vywin[ 1 ] + tt*dy;
     else
          oy = actvy.vywin[ 1 ] + (1.0 - tt)*dy;
/*
***Transformera till skärmkoordinater.
*/
     ox = k1x + k2x*ox; dx = k2x*dx;
     oy = k1y + k2y*oy; dy = k2y*dy;
/*
***Beräkna var vi skall sluta. -1 för att säkerställa att inte
***heltalsavrundning nedan ger en extra rad utanför skärmen.
***Ny metod 920304 JK.
*/
     xmax = k1x + k2x*actvy.vywin[2] - 1;
     ymax = k1y + k2y*actvy.vywin[3] - 1;
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
#ifdef V3_X11
           if ( gptrty == X11  &&  wpintr() ) return(0);
#endif
           for (j = 0; j < jmax; j++)
             {
#ifdef V3_X11
             if ( gptrty == X11 )
               {
               XDrawPoint(xdisp,xgwin,gpxgc,(int)x[i],(int)(gpsnpy-y[j]));
               XDrawPoint(xdisp,xgmap,gpxgc,(int)x[i],(int)(gpsnpy-y[j]));
               }
             else gpdmrk(x[i],y[j],DOT,TRUE);
#else
             gpdmrk(x[i],y[j],DOT,TRUE);
#endif
             }
           }
         } while (oy < ymax);
       } while (ox < xmax);
/*
***Special för MG400, MG420 och FT4600.
*/
   switch ( gptrty )
     {
     case MG400:
     case MG420:
     case FT4600:
     case MG700:
     putchar(13);
     break; 
     }
/*
***Slut.
*/
     return(0);
}

/********************************************************/
