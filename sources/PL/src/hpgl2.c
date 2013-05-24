/*!******************************************************************/
/*  File: hpgl2.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes the sources to the hpgl2 plotter             */
/*  driver/filter for Varkon.                                       */
/*                                                                  */
/*  main();          Main                                           */
/*  plinpl();        Init plotter                                   */
/*  plexpl();        Exit plotter                                   */
/*  plmove();        Upp/Move                                       */
/*  pldraw();        Down/Draw                                      */
/*  plchpn();        New pen                                        */
/*  plchwd();        New width                                      */
/*  plfill();        Fill area                                      */
/*                                                                  */
/*  This file is part of the VARKON Plotter Library.                */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-2000, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include "../include/params.h"
#include <string.h>

#define ppixsz   0.025 /* Plotter resolution in mm:s */
#define ppiysz   0.025 /* Plotter resolution in mm:s */

/* Some global variables */

bool   arccon;
bool   clip;
double clipw[4];
long   lastx;
long   lasty;

/*!******************************************************/

        int main(int argc, char *argv[])

/*      Main entrypoint for the Hewlett Packard HPGL-2
 *      plotter filter.
 *
 *      (C)microform ab 2000-06-18 J. Kjellander
 *      
 ******************************************************!*/

 {

/*
***Remember the name of the filter for error messages.
*/
   strcpy(prognam,argv[0]);
/*
***Process commandline for options.
*/
   plppar(argc,argv);
/*
***Software clipping is not needed.
*/
   clip  = FALSE;
/*
***Init.
*/
    plinpl();
/*
***Processa.
*/
    plprpf();
/*
***Exit.
*/
    plexpl();
/*
***End.
*/
    exit(0);
  }

/********************************************************/
/********************************************************/

        short plinpl()

/*      Init before processing.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{

/*
***Initialize the printer.
*/
   printf("\033E");
/*
***Set size of picture frame.
*/
   printf("\033*c1000X");
   printf("\033*c5000Y");
/*
***Enter HPGL/2 mode.
*/
   printf("\033%%0B");
/*
***Goto zero.
*/
   printf("PU0,0;");

   lastx = lasty = 0;

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plexpl()

/*      Exit after processing.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{

/*
***Return to (0,0).
*/
   printf("PU0,0;");
/*
***If the -FF option was given, eject the paper.
*/
   if ( formf ) printf("PG;");

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(double x, double y)

/*      Pen up + move to in absolute coordinates.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{
    int ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      printf("PU%d,%d;",ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short pldraw(double x, double y)

/*      Pen down + move to in absolute coordinates.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{
    int ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      printf("PD%d,%d;",ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchpn(short pn)

/*      Change pen.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{
    printf("SP%d;",pn);

    return(0);
}

/********************************************************/
/********************************************************/

        short plchwd(double width)

/*      Set linewidth.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{

   return(0);

 }

/********************************************************/
/********************************************************/

        short plfill(
        short  n,
        double x[],
        double y[],
        char   a[])

/*      Fill area.
 *
 *      (C)microform 2000-06-18 J.Kjellander
 *
 ********************************************************/

{
    return(0);
}

/********************************************************/
