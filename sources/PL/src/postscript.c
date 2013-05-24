/*!******************************************************************/
/*  File: postscript.c                                              */
/*  ==================                                              */
/*                                                                  */
/*  This file includes the sources to the postscript plotter        */
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

#define ppixsz   0.35277778 /* Plotter pixel x-size i mm  (25.4/72.0) */
#define ppiysz   0.35277778 /* Plotter pixel y-size i mm */
#define pplwsz   0.35277778 /* Plotter pixel line width i mm */
#define MAXRAD   200        /* Max antal rader i ett block */

/*
***Global variables.
*/
bool   arccon;          /* Måla cirklars rand */
bool   clip;            /* Klippflagga */
double clipw[4];        /* Klippfönster */
float  lastx;           /* Sista pos X */
float  lasty;           /* Sista pos Y */
short  rant;            /* Räknare för skickade rader */
double actwdt;          /* Akturll linjebredd */
short  lastc;           /* Senaste kommando;
                           0=odef; -1=moveto; -2=lineto; 1=pen */
extern int   actpen;

/*!******************************************************/

        int main(
        int   argc,
        char *argv[])

/*      Huvudprogram för PostScript.
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 *      1998-03-25 Färg, breda linjer mm. J.Kjellander
 *
 ******************************************************!*/

 {

/*
***Aktuellt programnamn.
*/
   strcpy(prognam,argv[0]);
/*
***Processa kommandoraden.
*/
   plppar(argc,argv);
/*
***Ladda eventuell penbreddsfil.
*/
   if ( pfrnam[0] != '\0' )
     {
     pllpfp("postscript.PEN");
     pllpfr(pfrnam);
     }
/*
***Cirklars målning.
*/
   arccon = FALSE;
/*
***Klipp-fönster.
*/
   clip  = FALSE;
/*
***Initiera plotter
*/
   plinpl();
/*
***Processa plotfil.
*/
   plprpf();
/*
***Avsluta plotter
*/
   plexpl();
/*
***Slut.
*/
   exit(V3EXOK);
  }

/********************************************************/
/********************************************************/

        short plinpl()

/*      Init plotter.
 *
 *      Plotter type = PostScript.
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 *      18/2-94  Bugfix, flyttat plchpn() till sist, J. Kjellander
 *      28/10-94 Kommentar och joinstyle i initieringen enl. Åke Å
 *      1997-11-27 Färg, J.Kjellander
 *
 ********************************************************/

{

/*
***Initieringens 1:a rad bör vara en kommentar för att
***vissa skrivare skall tolka det hela som PostScript.
*/
   printf("%%!VARKON-3D 1.17\n");
   printf("/m {stroke newpath moveto} def\n");
   printf("/l {lineto} def\n");
   printf("/t {stroke newpath setlinewidth} def\n");
   printf("2 setlinejoin\n");
   printf("%3.1f %3.1f m\n",0.0,0.0);
/*
***Ska vi köra  färg eller svartvitt ?
*/
   if ( !bw )
     {
     printf("/c {stroke setcolor} def\n");
     printf("/DeviceRGB setcolorspace\n");
     }
/*
***Globala variabler.
*/
   rant   = 0;
   actpen = -1;
   actwdt = -1.0;
   lastc  = -1;
   lastx  = lasty = -1.0;
/*
***Default penna = penna 1.
*/
   plchpn(1);
/*
***Default linjebredd = 0.0, dvs. 1 pixel.
*/
   plchwd(0.0);

   return(0);
}

/********************************************************/
/********************************************************/

        short plexpl()

/*      Avslutning av plotter
 *
 *      Plotter typ = PostScript
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 ********************************************************/

{
   printf("stroke\n");
   printf("showpage\n");

   return(0);
}

/********************************************************/
/********************************************************/

        short plmove(double x, double y)

/*      Förflyttning.
 *
 *      In: x och y modellkoordinater i mm.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 ********************************************************/

{
    float  ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      printf("%3.1f %3.1f m\n",ix,iy);

      lastx = ix;
      lasty = iy;
      rant  = 0;
      lastc = -1;
      }

    return(0);
}

/********************************************************/
/********************************************************/

        short pldraw(double x, double y)

/*      Rita linje.
 *
 *      In: x och y modellkoordinater i mm.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 ********************************************************/

{
    float  ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

/*  Rita endast om en verklig förflyttning skall utföras */

    if ( ix != lastx || iy != lasty )
      {

/*    Föregående kommando initierade 'newpath'. Definiera aktuell punkt. */

      if (lastc > 0 )
        {
        printf("%3.1f %3.1f m\n",lastx,lasty);
        rant  = 0;
        lastc = -1;
        }

      if ( rant > MAXRAD  )
        {
        printf("%3.1f %3.1f l\n",ix,iy);
        printf("stroke newpath\n");
        printf("%3.1f %3.1f m\n",ix,iy);
        rant  = 0;
        lastc = -1;

        }
      else
        {

        printf("%3.1f %3.1f l\n",ix,iy);
        rant++;
        lastc = -2;
        }

      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/
/********************************************************/

        short plchpn(short pn)

/*      Byt penna.
 *
 *      (C)microform ab 25/9/92  J. Kjellander
 *
 *      1/2/93 20 standard-bredder, JK
 *      1997-11-27 Färg, J.Kjellander
 *      1998-09-21 Pennfil, J.Kjellander
 *
 ********************************************************/

{
    float  w,lw,r,g,b;

/*
***Om pennfil angetts skall vi mappa pennummer till
***linjebredd.
*/
   if ( pfrnam[0] != '\0' )
     {
     if      ( pn == 1 ) w = 0.1;
     else if ( pn == 2 ) w = 0.2;
     else if ( pn == 3 ) w = 0.3;
     else if ( pn == 4 ) w = 0.4;
     else if ( pn == 5 ) w = 0.5;
     else if ( pn == 6 ) w = 0.6;
     else if ( pn == 7 ) w = 0.7;
     else if ( pn == 8 ) w = 0.8;
     else if ( pn == 9 ) w = 0.9;

     else if ( pn == 10 ) w = 1.0;
     else if ( pn == 11 ) w = 1.1;
     else if ( pn == 12 ) w = 1.2;
     else if ( pn == 13 ) w = 1.3;
     else if ( pn == 14 ) w = 1.4;
     else if ( pn == 15 ) w = 1.5;
     else if ( pn == 16 ) w = 1.6;
     else if ( pn == 17 ) w = 1.7;
     else if ( pn == 18 ) w = 1.8;
     else if ( pn == 19 ) w = 1.9;
     else if ( pn == 20 ) w = 2.0;
     else                 w = 0;

     lw = (float)(skala*w/pplwsz);
     printf("%8.6f t\n",lw);

     actpen = pn;
     lastc  = 1;
     return(0);
     }
/*
***Om ingen pennfil getts skall vi mappa penn-nummer till RGB-värde.
*/
   else if ( pn != actpen )
     {
     if      ( pn == 1 )  {r=0;   g=0;   b=0;}
     else if ( pn == 2 )  {r=1;   g=0;   b=0;}
     else if ( pn == 3 )  {r=0;   g=1;   b=0;}
     else if ( pn == 4 )  {r=0;   g=0;   b=1;}
     else if ( pn == 5 )  {r=0.5; g=0;   b=0;}
     else if ( pn == 6 )  {r=0;   g=0.5; b=0;}
     else if ( pn == 7 )  {r=0;   g=0;   b=0.5;}
     else if ( pn == 8 )  {r=1;   g=0.5; b=1;}
     else if ( pn == 9 )  {r=1;   g=1;   b=0;}

     else if ( pn == 10 ) {r=0;   g=0;   b=0;}
     else if ( pn == 11 ) {r=0;   g=0;   b=0;}
     else if ( pn == 12 ) {r=0;   g=0;   b=0;}
     else if ( pn == 13 ) {r=0;   g=0;   b=0;}
     else if ( pn == 14 ) {r=0;   g=0;   b=0;}
     else if ( pn == 15 ) {r=0;   g=0;   b=0;}
     else if ( pn == 16 ) {r=0;   g=0;   b=0;}
     else if ( pn == 17 ) {r=0;   g=0;   b=0;}
     else if ( pn == 18 ) {r=0;   g=0;   b=0;}
     else if ( pn == 19 ) {r=0;   g=0;   b=0;}
     else if ( pn == 20 ) {r=0;   g=0;   b=0;}
     else                 {r=0;   g=0;   b=0;}

     if ( !bw ) printf("%5.3f %5.3f %5.3f c\n",r,g,b);

     actpen = pn;
     lastc  = 1;
     }

    return(0);
}

/********************************************************/
/********************************************************/

        short plchwd(double width)

/*      Byt linjebredd.
 *
 *      In: width = Önskad linjebredd.
 *
 *      (C)microform ab 1997-12-28 J. Kjellander
 *
 *      1998-09-21 Pennfil, J.Kjellander
 *
 ********************************************************/

{
    float  lw;

/*
***Om pennfil angetts gör vi ingenting.
*/
   if ( pfrnam[0] != '\0' ) return(0);
/*
***Ställ in den önskade linjebredden.
*/
    if ( width != actwdt )
      {
      lw = (float)(width/pplwsz);

      printf("%8.6f t\n",lw);

      actwdt = width;
      lastc = 1;
      }

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
 *      (C)microform ab 1999-12-15 J. Kjellander
 *
 ********************************************************/

{
    plpoly(n,x,y,a);
    printf("%s\n%s\n","closepath","fill");

    return(0);
}

/********************************************************/
