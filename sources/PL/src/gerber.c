/*!******************************************************************/
/*  File: gerber.c                                                   */
/*  ==============                                                   */
/*                                                                  */
/*  This file includes the sources to the gerber plotter            */
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
#include <time.h>
#include <malloc.h>

#define ppixsz   0.0254 /* Plotter pixel x-size i mm */
#define ppiysz   0.0254 /* Plotter pixel y-size i mm */

bool   arccon;          /* Måla cirklars rand */
bool   clip;            /* Klippflagga */
double clipw[4];        /* Klippfönster */
long   lastx;           /* Sista pos X */
long   lasty;           /* Sista pos Y */

/*!******************************************************/

        int main(int argc, char *argv[])

/*      Huvudprogram för fotoplottrar med Gerber-emulator.
 *
 *      (C)microform 2/1/91  R. Svedin
 *      
 ******************************************************!*/

 {

/*
***Defaultvärde för hastighet.
*/
     hast = 0.6;
/*
***Processa kommandoraden.
*/
     plppar(argc,argv);
/*
***Om penbreddsfil angetts på kommandoraden, ladda denna
***samt plotterns dito.
*/
   if ( pfrnam[0] != '\0' )
     {
     pllpfr(pfrnam); 
     pllpfp("gerber.PEN");
     arccon = FALSE;
     }
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

/*      Initiering av plotter 
 *
 *      Plotter typ = Gerber
 *
 *      Initiering = G54*G01*
 *
 *      G54  Välj appertur.
 *      G01  Linjär interpolation.
 *      
 *      (C)microform 2/1/91  R. Svedin
 *
 ********************************************************/

{
/*
   printf("G54*G01*");
*/
   printf("Z*G90*");

   lastx = lasty = 0;

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plexpl()

/*      Avslutning av plotter 
 *
 *      Plotter typ = Gerber
 *
 *      Avslutning = M02*
 *
 *      Avsluta och återvänd till hem positionen.
 *
 *      (C)microform 2/1/91  R. Svedin
 *
 ********************************************************/

{

   printf("M02*");

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(double x, double y)

/*      Flytta. Förflyttning av fotohuvud med släckt lampa.
 *
 *      Gerber = D02*   Släck lampan.
 *               Xxxx   X + x-koord.
 *               Yyyy*  Y + y-koord.
 *
 *      Släck lampan x2,y2  Absoluta koord.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      Ut:*
 *
 *      (C)microform 2/1/91  R. Svedin
 *
 ********************************************************/

{
    long  ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      printf("D02*X%ldY%ld*",ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short pldraw(double x, double y)

/*      Rita. Förflyttning av fotohuvud med tänd lampa.
 *
 *      Gerber = D01*   Tänd lampan.
 *               Xxxx   X + x-koord.
 *               Yyyy*  Y + y-koord.
 *
 *      Tänd lampa x2,y2  Absoluta koord.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      Ut: 
 *
 *      (C)microform 2/1/91  R. Svedin
 *
 ********************************************************/

{
    long   ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      printf("D01*X%ldY%ld*",ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchpn(short pn)

/*      Byt appertur.
 *
 *      Gerber = Dn*
 *      n = apperturnummer.
 *
 *      In: pn = pennummer.
 *
 *      Ut: 
 *
 *      (C)microform 2/1/91  R. Svedin
 *
 ********************************************************/

{

    short  an;
   
    an = 10;

    if      ( pn == 1 ) an = 10;
    else if ( pn == 2 ) an = 11;
    else if ( pn == 3 ) an = 12;
    else if ( pn == 4 ) an = 13;
    else if ( pn == 5 ) an = 14;
    else if ( pn == 6 ) an = 15;
    else if ( pn == 7 ) an = 16;
    else if ( pn == 8 ) an = 17;
    else if ( pn == 9 ) an = 18;
    else if ( pn == 10 ) an = 19;

    else if ( pn == 11 ) an = 70;
    else if ( pn == 12 ) an = 71;

    else if ( pn == 13 ) an = 20;
    else if ( pn == 14 ) an = 21;
    else if ( pn == 15 ) an = 22;
    else if ( pn == 16 ) an = 23;
    else if ( pn == 17 ) an = 24;
    else if ( pn == 18 ) an = 25;
    else if ( pn == 19 ) an = 26;
    else if ( pn == 20 ) an = 27;
    else if ( pn == 21 ) an = 28;
    else if ( pn == 22 ) an = 29;

    else if ( pn == 23 ) an = 72;
    else if ( pn == 24 ) an = 73;

    printf("D%d*",an);

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchwd(double width)

/*      Byt linjebredd.
 *
 *      In: width = Önskad linjebredd.
 *
 *      (C)microform ab 1999-03-01 J. Kjellander
 *
 ********************************************************/

{

/*
***Om penfil (-pf) saknas sätter vi den
***begärda bredden, annars gör vi ingenting. I
***så fall skall PEN mappas till linjebredd och
***det sköts av plcwdt().
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
