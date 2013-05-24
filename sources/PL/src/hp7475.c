/*!******************************************************************/
/*  File: hp7475.c                                                   */
/*  ==============                                                   */
/*                                                                  */
/*  This file includes the sources to the hp7475 plotter            */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include "../include/params.h"
#include <string.h>

#define ppixsz   0.024874  /* Plotter resolution in mm:s */
#define ppiysz   0.024874  /* Plotter resolution in mm:s */

/* Some global variables */

bool   arccon;
bool   clip;
double clipw[4];
long   lastx;
long   lasty;

/*!******************************************************/

        int main(int argc, char *argv[])

/*      Main entrypoint for the Hewlett Packard HP7475
 *      plotter filter.
 *
 *      (C)microform ab 19/2/85 J. Kjellander
 *      
 *      16/12/85 Vridning mm.   J. Kjellander
 *      18/12/85 HP7475 format. R. Svedin
 *      1998-09-21 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {

/*
***Remember the name of the filter for error messages.
*/
   strcpy(prognam,argv[0]);
/*
***Default value for pen speed.
*/
   hast = 0.6;
/*
***Process commandline for options.
*/
   plppar(argc,argv);
/*
***If the penfile option was included, load it and
***also load the penfile for the printer.
*/
   pllpfp("hp7475.PEN");
   arccon = FALSE;

   if ( pfrnam[0] != '\0' ) pllpfr(pfrnam); 
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
 *      Plotter type = hp7475
 *
 *      Init command = "ESC".I81; ; 17 : "ESC".N; 19 : DF; VS hast;
 *      
 *      (C)microform 18/12/85  R. Svedin
 *
 ********************************************************/

{

/*
***Set up pen speed.
*/
   if      ( hast <= 0.2 ) printf("\033.I81;;17:\033.N;19:DF;VS3;");
   else if ( hast <= 0.4 ) printf("\033.I81;;17:\033.N;19:DF;VS5;");
   else if ( hast <= 0.6 ) printf("\033.I81;;17:\033.N;19:DF;VS10;");
   else if ( hast <= 0.8 ) printf("\033.I81;;17:\033.N;19:DF;VS20;");
   else if ( hast <= 1.0 ) printf("\033.I81;;17:\033.N;19:DF;VS40;");
/*
***Make a first move to zero.
*/
   printf("PU0,0;");     /* "MOVE" to 0,0 */

   lastx = lasty = 0;

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plexpl()

/*      Exit after processing.
 *
 *      Plotter type = hp7475
 *
 *      Command = SP0;
 *
 *      (C)microform 18/12/85 R. Svedin
 *
 *      17/9/91 FF, J. Kjellander
 *
 ********************************************************/

{

/*
***Load Pen 0.
*/
   printf("SP0;");
/*
***If the -FF option was given, eject the paper.
*/
   if ( formf ) putchar('\014');

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(double x, double y)

/*      Pen up + move to in absolute coordinates.
 *
 *      hp7475 = PU x , y ;
 *
 *
 *      (C)microform 18/12/85 R. Svedin
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
 *      hp7475 = PD x , y ;
 *
 *      (C)microform 18/12/85 R. Svedin
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
 *      hp7475 = SP n ;
 *
 *      (C)microform 18/12/85 R. Svedin
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
 *      (C)microform ab 1998-09-21 J. Kjellander
 *
 ********************************************************/

{

/*
***If penfile option was given use the internal
***painting system which will emulate any width.
***If not, we can't do anything because hp7475
***has no support for variable linewidth.
*/
   if ( pfrnam[0] == '\0' ) return(plcnpt(width));
   else                     return(0);

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
 *      (C)microform ab 1999-12-15 J. Kjellander
 *
 ********************************************************/

{
    return(0);
}

/********************************************************/
