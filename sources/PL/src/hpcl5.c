/*!******************************************************************/
/*  File: hpcl5.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes the sources to the hpcl5 plotter             */
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

#define ppixsz   0.025  /* Plotter pixel x-size i mm */
#define ppiysz   0.025  /* Plotter pixel y-size i mm */

bool   arccon;          /* Måla cirklars rand */
bool   clip;            /* Klippflagga */
double clipw[4];        /* Klippfönster */
long   lastx;           /* Sista pos X */
long   lasty;           /* Sista pos Y */
short  lastk;           /* Sista kommado
                           0 = Övriga
                           1 = Rita
                           2 = Flytta  */
double actwdt;          /* Aktuell linjebredd */

extern double ptabp[];

/*!******************************************************/

        int main(int argc, char *argv[])

/*      Huvudprogram för Hewlett Packard Laserjet 3 med CPL-HPGL/2
 *
 *      (C)microform ab 19/2/85 J. Kjellander
 *      
 *      16/12/85 Vridning mm.   J. Kjellander
 *      15/1/91 HP-laser format. R. Svedin
 *      1998-09-21 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
/*
***Programnamn.
*/
   strcpy(prognam,argv[0]);
/*
***Defaultvärde för hastighet.
*/
   hast = 0.6;
/*
***Processa kommandoraden.
*/
   plppar(argc,argv);
/*
***Hur bred är pennan ?
*/
   pllpfp("hpcl5.PEN");
/*
***Om penbreddsfil angetts på kommandoraden, ladda denna
***samt plotterns dito.
*/
   arccon = FALSE;

   if ( pfrnam[0] != '\0' ) pllpfr(pfrnam); 
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
 *      Plotter typ HP Laserjet 3 med CPL.HP-GL/2
 *
 *      (C)microform 18/12/85  R. Svedin
 *
 *      15/1/91 HP-laser format. R. Svedin
 *      7/8/91  Div. justeringar, J. Kjellander
 *
 ********************************************************/

{

/*
***EscE = Reset.
***Esc*c#Y = Set picture frame vertical size (Y)
***Esc%0B = Change emulation to HPGL
***IN = init HPGL
*/
   printf("\033E\033*c7910Y\033%%0BIN"); 
/*
***Aktivera penna 1.
*/
   printf("SP1;");
/*
***Gör ett move till (0,0).
*/
   printf("PU0,0");

   lastx = lasty = 0;
   lastk = 0;

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plexpl()

/*      Avslutning av plotter 
 *
 *      Plotter typ HP Laserjet 3 med CPL-HPGL/2
 *
 *      Avslutning = "ESC"%0A "ESC"E 
 *
 *      Växla till PCL, gör reset och mata ut pappret.
 *
 *      (C)microform 18/12/85 R. Svedin
 *      15/1/91 HP-laser format. R. Svedin
 *
 ********************************************************/

{

   printf("PU0,0;");     /* Gör ett "MOVE" till 0,0 */
   printf("\033%%0A\033E");
   lastk = 0;

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(double x, double y)

/*      Förflyttning av penna,  penna upp först.
 *
 *      Plotter typ HP Laserjet 3 med CPL-HPGL/2
 *
 *      PU x , y
 *      Penna upp x1,y1  Absoluta eller relativa koord.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 18/12/85 R. Svedin
 *
 *      15/1/91 HP-laser format. R. Svedin
 *
 ********************************************************/

{
    long  ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      if ( lastk != 2 )
        printf("PU%ld,%ld",ix,iy);
      else
        printf(",%ld,%ld",ix,iy);

      lastx = ix;
      lasty = iy;
      lastk = 2;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short pldraw(double x, double y)

/*      Rita med penna, penna ned först.
 *
 *      Plotter typ HP Laserjet 3 med CPL-HPGL/2
 *
 *      PD x , y
 *      Penna ner x2,y2  Absoluta eller relativa koord.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 18/12/85 R. Svedin
 *
 *      15/1/91 HP-laser format. R. Svedin
 *
 ********************************************************/

{
    long   ix,iy;

    ix = x/ppixsz;
    iy = y/ppiysz;

    if ( ix != lastx || iy != lasty )
      {
      if ( lastk != 1 ) 
        printf("PD%ld,%ld",ix,iy);
      else
        printf(",%ld,%ld",ix,iy);

      lastx = ix;
      lasty = iy;
      lastk = 1;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchpn(short pn)

/*      Byt penna.
 *
 *      SP n PW n
 *
 *      Byt penna Pennummer. Ändra bredd Bredd
 * 
 *      (C)microform 18/12/85 R. Svedin
 *
 *      15/1/91 HP-laser format. R. Svedin
 *      7/8/91  Slutputs, J. Kjellander
 *      1998-09-21 WIDTH, J.Kjellander
 *
 ********************************************************/

{
    double pw;

   if ( pfrnam[0] != '\0' )
     {
     if      ( pn == 1 )  pw = 0.1;
     else if ( pn == 2 )  pw = 0.2;
     else if ( pn == 3 )  pw = 0.3;
     else if ( pn == 4 )  pw = 0.4;
     else if ( pn == 5 )  pw = 0.5;
     else if ( pn == 6 )  pw = 0.6;
     else if ( pn == 7 )  pw = 0.7;
     else if ( pn == 8 )  pw = 0.8;
     else if ( pn == 9 )  pw = 0.9;
     else if ( pn == 10 ) pw = 1.0;
     else if ( pn == 11 ) pw = 1.1;
     else if ( pn == 12 ) pw = 1.2;
     else if ( pn == 13 ) pw = 1.3;
     else if ( pn == 14 ) pw = 1.4;
     else if ( pn == 15 ) pw = 1.5;
     else if ( pn == 16 ) pw = 1.6;
     else if ( pn == 17 ) pw = 1.7;
     else if ( pn == 18 ) pw = 1.8;
     else if ( pn == 19 ) pw = 1.9;
     else if ( pn == 20 ) pw = 2.0;
     else                 pw = 0.0;

     printf("PW%g",pw);
     lastk = 0;
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
    double lw;

/*
***Om pennfil angetts gör vi ingenting.
*/
   if ( pfrnam[0] != '\0' ) return(0);
/*
***Ställ in den önskade linjebredden.
*/
    if ( width != actwdt )
      {
      lw = (float)(skala*width/ptabp[0]);

      printf("PW%g",lw);

      actwdt = width;
      lastk = 1;
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
    return(0);
}

/********************************************************/
