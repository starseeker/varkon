/*!******************************************************************/
/*  File: epson.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes the sources to the epson plotter             */
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

/*
*******************************************************!*/

bool   arccon;          /* Måla cirklars rand */
bool   clip;            /* Klippflagga */
double clipw[4];        /* Klippfönster */
short  lastx;           /* Sista pos X */
short  lasty;           /* Sista pos Y */

#define MAXBMB  100     /* Max antal bytes i bitmap = 3.200.000 */
char  *bmbpek[MAXBMB];  /* Pekare till maximalt antal bitmap-block */
short  nbmaps;          /* Antal block i bruk */
short  bitmsx;          /* Bitmappens storlek i X-led */
short  bitmsy;          /* Bitmappens storlek i Y-led */
double ppixsz;          /* Pixelstorlek i X-led */
double ppiysz;          /* Pixelstorlek i Y-led */
short  kod;             /* Kod för vald upplösning */

/*!******************************************************/

        int main(int argc, char *argv[])

/*      Huvudprogram för Epson matrisskrivare.
 *
 *      (C)microform ab 23/2/89 J. Kjellander
 *
 *      1998-09-21 WIDE, J.Kjellander
 *
 ******************************************************!*/

 {
   short i;
   long time1=0,time2=0,time3=0,time4=0;
   char timstr[27];

/*
***Programnamn.
*/
   strcpy(prognam,argv[0]);
/*
***Defaultvärde för upplösning. Epson-skrivare kan plotta
***med olika upplösningar beroende på om man kör den
***i 8- eller 24-pinnars mode. 
***
***I 8-pinnars mode är den vertikala upplösningen 72/tum och i
***24-pinnars mode är den 180/tum.
***
***Aktiv plott-area för en A4:a är :
***      I X-led  8 tum dvs. 203.2 mm.
***      I Y-led 102 rader dvs. 287.9 mm. i  8-pinnars mode.
***      I Y-led  85 rader dvs. 289.9 mm. i 24-pinnars mode.
***
***Detta är något mindre än det verkliga A-formatet som är:
***      210 * 296 mm. för en A4:a
*/
   hast = 1.0; npins = 0; nrows = 0; formw = 203.2;
/*
***Processa kommandoraden.
*/
   plppar(argc,argv);
/*
***Om penbreddsfil angetts på kommandoraden, ladda denna.
***Plotterns pennfil laddas under alla förutsättningar
***eftersom den behövs för att kunna måla rätt.
*/
   arccon = FALSE;
   pllpfp("epson.PEN");

   if ( pfrnam[0] != '\0' ) pllpfr(pfrnam); 
/*
***Klipp-fönster.
*/
   clip  = FALSE;
/*
***Bitmappens storlek i antal pixel och i millimeter.
***24-pinnars mode.
*/
   if ( npins == 24 )
     {
     if ( nrows == 0 ) nrows = 85;
     bitmsy = nrows * 24; ppiysz = 25.4/180.0; 

     if ( hast < 0.25 )      { ppixsz = 25.4/360; kod = 40; }
     else if ( hast < 0.5 )  { ppixsz = 25.4/180; kod = 39; }
     else if ( hast < 0.75 ) { ppixsz = 25.4/120; kod = 33; }
     else                    { ppixsz = 25.4/60;  kod = 32; }
     }
/*
***8-pinnars mode.
*/
   else
     {
     if ( npins == 8 )
       {
       if ( nrows == 0 ) nrows = 85;
       bitmsy = nrows * 8; ppiysz = 3*25.4/180.0;
       }
     else
       {
       if ( nrows == 0 ) nrows = 102;
       bitmsy = nrows * 8; ppiysz = 25.4/72.0;
       }
       
     if ( hast < 0.4 )      { ppixsz = 25.4/240.0; kod = 3; }
     else if ( hast < 0.8 ) { ppixsz = 25.4/120.0; kod = 1; }
     else                   { ppixsz = 25.4/60.0;  kod = 0; }
     }

   if ( (formw/ppixsz) > 6000 )
     {
     printf("epson: Bitmap too big in horizontal direction,\n");
     printf("       try lower resolution or smaller value for -w !\n");
     exit(V3EXOK);
     }
   bitmsx = formw/ppixsz;
/*
***Allokera minne för bitmap. 
*/
   nbmaps = (short)(((long)bitmsx*(long)bitmsy)/BMBSIZ/8 + 1);
   if ( nbmaps > MAXBMB )
     {
     printf("epson: Too large bitmap, try lower resolution !\n");
     exit(V3EXOK);
     }
   for ( i=0; i<nbmaps; ++i )
     if ( (bmbpek[i]=malloc((unsigned)BMBSIZ)) == NULL )
       {
       for ( ; i>0; --i ) free(bmbpek[i-1]);
       printf("epson: Can't malloc bitmap, try lower resolution !\n");
       exit(V3EXOK);
       }
/*
***Initiera plotter
*/
   if ( debug ) time1 = time((long *)0);
   plinpl();
/*
***Processa plotfil.
*/
   if ( debug ) time2 = time((long *)0);
   plprpf();
/*
***Avsluta plotter. Om debug, rita ramen.
*/
   if ( debug )
     {
     time3 = time((long *)0);
     plrast((short)0,(short)0,bitmsx-1,(short)0);
     plrast(bitmsx-1,(short)0,bitmsx-1,bitmsy-1);
     plrast(bitmsx-1,bitmsy-1,(short)0,bitmsy-1);
     plrast((short)0,bitmsy-1,(short)0,(short)0);
     }

   plexpl();
/*
***Slut. time1 = start, time2-time1 är tid för nollställning
***av bitmap, time3-time2 är tid för rasterkonvertering,
***time4 är slut och time4-time3 är tid för dumpning av bitmap.
*/
   if ( debug )
     {
     time4 = time((long *)0);
     time4 = time4 - time3;
     strcpy(timstr,ctime(&time4)); timstr[19] = '\0';
     printf("\nDumpning av bitmap : %s\n",&timstr[10]);
     time3 = time3 - time2;
     strcpy(timstr,ctime(&time3)); timstr[19] = '\0';
     printf("Rasterkonvertering : %s\n",&timstr[10]);
     time2 = time2 - time1;
     strcpy(timstr,ctime(&time2)); timstr[19] = '\0';
     printf("Nollst. av bitmap  : %s\n",&timstr[10]);
     printf("Antal bmblock      : %d\n",nbmaps);
     printf("Antal rader grafik : %d\n",nrows);
     printf("Antal pixels i X   : %d\n",bitmsx);
     printf("Antal pixels i Y   : %d\n",bitmsy);
     printf("Antal mm. i X      : %g\n",bitmsx*ppixsz);
     printf("Antal mm. i Y      : %g\n",bitmsy*ppiysz);
     }

   exit(V3EXOK);

  }

/********************************************************/
/********************************************************/

        short plinpl()

/*      Initiering av plotter 
 *
 *      Plotter typ = Epson.
 *
 *      (C)microform 23/1/89  J. Kjellander
 *
 ********************************************************/

 {
   register unsigned short i;
   short j;
   char *pek;

/*
***Nollställ bitmapp.
*/
   for ( j=0; j<nbmaps; ++j )
     {
     pek = bmbpek[j];
     for ( i=0; i<BMBSIZ; ++i ) *(pek+i) = 0;
     }
/*
***Initiera last-variabler.
*/
   lastx = lasty = -1;

   return(0);
 }

/********************************************************/ 
/********************************************************/

        short plexpl()

/*      Dumpar bitmap till skrivare.
 *
 *      Plotter typ = Epson.
 *
 *      (C)microform 23/2/89 J. Kjellander
 *
 *      900109   FF och tknbufÄÅ, J. Kjellander
 *
 ********************************************************/

{
   short ix,iy,tknrad,n1,n2,block,i;
   int   ntkn;
   char  tknbuf[3*6000]; /* 3 * Max upplösning i X-led */
   long  bytofs;

/*
***Börja med ett <CR>.
*/
   putchar('\015');
/*
***24-pinnars dumpning.
*/
   if ( npins == 24 )
     {
     for ( tknrad=0; tknrad<nrows; ++tknrad)
       {
       ntkn = 0;
       iy = 3*nrows - 3*tknrad - 1;
       for ( ix=0; ix<bitmsx; ++ix)
         {
         bytofs = (long)iy*(long)bitmsx + ix;
         block = (short)(bytofs/BMBSIZ);
         bytofs -= (long)block*(long)BMBSIZ;
         tknbuf[ntkn++] = *(bmbpek[block]+bytofs);

         bytofs = (long)(iy-1)*(long)bitmsx + ix;
         block = (short)(bytofs/BMBSIZ);
         bytofs -= (long)block*(long)BMBSIZ;
         tknbuf[ntkn++] = *(bmbpek[block]+bytofs);

         bytofs = (long)(iy-2)*(long)bitmsx + ix;
         block = (short)(bytofs/BMBSIZ);
         bytofs -= (long)block*(long)BMBSIZ;
         tknbuf[ntkn++] = *(bmbpek[block]+bytofs);
         }
       while ( ntkn>2 )
         {
         if ( tknbuf[ntkn-1] == '\0' &&
              tknbuf[ntkn-2] == '\0' &&
              tknbuf[ntkn-3] == '\0' ) ntkn -= 3;
         else break;
         }

       if ( ntkn > 0 )
         {
         ntkn /= 3;
         n2 = ntkn>>8; n1 = ntkn - n2*256;
         printf("\033*%c%c%c",(char)kod,(char)n1,(char)n2);
         fwrite(tknbuf,1,3*ntkn,stdout);
         }
       printf("\033J\030\015");
       }
     }
/*
***8-pinnars dumpning.
*/
   else
     {
     for ( tknrad=0; tknrad<nrows; ++tknrad)
       {
       ntkn = 0; iy = nrows - tknrad - 1;
       for ( ix=0; ix<bitmsx; ++ix)
         {
         bytofs = (long)iy*(long)bitmsx + ix;
         block = (short)(bytofs/BMBSIZ);
         bytofs -= (long)block*(long)BMBSIZ;
         tknbuf[ntkn++] = *(bmbpek[block]+bytofs);
         }
       for ( ; ntkn>0; --ntkn ) if ( tknbuf[ntkn-1] != '\0' ) break;
       if ( ntkn > 0 )
         {
         n2 = ntkn>>8; n1 = ntkn - n2*256;
         printf("\033*%c%c%c",(char)kod,(char)n1,(char)n2);
         fwrite(tknbuf,1,ntkn,stdout);
         }
       printf("\033J\030\015");
       }
     }
/*
***<CR> + ev. avslutande formfeed.
*/
   putchar('\015');
   if ( formf ) putchar('\014');
/*
***Återlämna allokerat minne.
*/
   for ( i=0; i<nbmaps; ++i ) free(bmbpek[i]);

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(double x, double y)

/*      Förflyttning av penna. Ställer om last-variablerna.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 23/1/89 J. Kjellander
 *
 ********************************************************/

{
    short ix,iy;

    ix = (short)(x/ppixsz);
    iy = (short)(y/ppiysz);

    if ( ix != lastx || iy != lasty )
      {
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short pldraw(double x, double y)

/*      Ritar vektor.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 23/1/89 J. Kjellander
 *
 ********************************************************/

{
    short   ix,iy;

    ix = (short)(x/ppixsz);
    iy = (short)(y/ppiysz);

    if ( ix != lastx || iy != lasty )
      {
      plrast(lastx,lasty,ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchpn(short pn)

/*      Byt penna. Dummy i fallet Epson.
 *
 *      (C)microform 23/1/89 J. Kjellander
 *
 ********************************************************/

{ return(0); }

/********************************************************/ 
/********************************************************/

        short plchwd(double width)

/*      Byt linjebredd.
 *
 *      (C)microform 1998-03-25 J. Kjellander
 *
 ********************************************************/

{

/*
***Om penfil (-pf) saknas utgår sätter vi den
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
