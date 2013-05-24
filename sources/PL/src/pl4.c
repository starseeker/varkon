/*!******************************************************************/
/*  File: pl4.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plbpoi();    Build point                                       */
/*   plblin();    Build line                                        */
/*   plbarc();    Build arc                                         */
/*   plbtxt();    Build text                                        */
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
#include <string.h>

extern double skala,curnog;

/*
***Koder för linjetyp.
*/

#define SOLIDLN  0           /* Heldragen */ 
#define DASHLN   1           /* Streckad */ 
#define DADOLN   2           /* Punkstreckad */ 

/*
***Koder för cirkeltyp
*/

#define SOLIDARC 0           /* Heldragen */ 
#define DASHARC  1           /* Streckad */ 
#define DADOARC  2           /* Punkstreckad */ 

#define DASHRATIO 0.25       /* Mellanrummens del av strecklängden i % */ 
#define STOL      0.0        /* Minsta strecklängd */
#define DOTSIZ    1.0        /* Punkts storlek */

/*
***Koder för vektorstatus (vec.a) i displayfilen.
*/

#define VISIBLE  1           /* Tänd förflyttning */
#define ENDSIDE  2           /* Slut-vektor */

/*
***Vektortabell för teckengenerering.
*/
#include "../../GP/include/font0.h"

/*!******************************************************/

        short plbpoi(
        GMPOI *poipek,
        short *n,
        double x[],
        double y[],
        char a[])

/*      Bygger en punkt i form av en polylinje..
 *
 *      IN:
 *         poipek:     Pekare till punkt-struktur
 *         n+1:        Offset till punktens startposition
 *         xÄ n+1 Å,
 *         yÄ n+1 Å:   Punktens koordinater.
 *
 *      UT:
 *         n:          Offset till polylinjens slutpunkt
 *         x,y,a:      Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 4/2/91 J. Kjellander
 *
 ******************************************************!*/

 {
     register short k;
     double xp,yp,dpx,dpy;


     k = *n;

     dpx = DOTSIZ/skala;
     dpy = DOTSIZ/skala;

     xp = x[ k+1 ];
     yp = y[ k+1 ];

/*
***Övre högra hörnet.
*/
     x[++k] = xp + dpx; y[k] = yp + dpy; a[k] = 0;
/*
***Nedre vänstra.
*/
     x[++k] = xp - dpx; y[k] = yp - dpy; a[k] = VISIBLE;
/*
***Övre vänstra hörnet.
*/
     x[++k] = xp - dpx; y[k] = yp + dpy; a[k] = 0;
/*
***Nedre högra.
*/
     x[++k] = xp + dpx; y[k] = yp - dpy; a[k] = VISIBLE;

     *n = k;

     return(0);
 }

/********************************************************/
/*!******************************************************/

        short plblin(
        GMLIN  *linpek,
        short  *n,
        double x[],
        double y[],
        char   a[])


/*      En heldragen linje expanderas till en streckad eller
 *      punktstreckad polylinje.
 *      
 *
 *      IN:
 *         linpek:    Adress till linje-struktur.
 *         n+1:       Offset till polylinjens startpunkt.
 *         xÄ n+1 Å,yÄ n+1 Å,aÄ n+1 Å: Startpunkt med status.
 *         yÄ n+2 Å,yÄ n+2 Å,aÄ n+2 Å: Slutpunkt med status.
 *
 *      Ut:
 *         n:         Offset till polylinjens slutpunkt.
 *         x,y,a:     X-,y-koordinater och status hos polylinjen.
 *
 *      FV: 0
 *
 *      (c)microform ab 4/2/91 J. Kjellander
 *
 ******************************************************!*/

 {
    register short i,k=0;
    char aa='\0',a1='\0',a2='\0';
    double vec[4];
    double len,dlen,dx,dy;
    double c1,c2,dxc1,dyc1,dxc2,dyc2;

    
    switch (linpek->fnt_l)
      {
/*
***Heldragen.
*/
       case SOLIDLN:
       *n += 2;
       a[0] = 0;
       a[1] = VISIBLE;
       break;
/*
***Streckad.
*/
       case DASHLN:
       i = 0;
       do
         {
         switch (i)
           {
/*
***Start.
*/
           case 0:
           k = *n;
           vec[ 0 ] = x[ k+1 ];    /* Startpunkt */
           vec[ 1 ] = y[ k+1 ];
           a1 = a[ k+1 ];
           vec[ 2 ] = x[ k+2 ];    /* Slutpunkt */
           vec[ 3 ] = y[ k+2 ];
           a2 = a[ k+2 ];

           dx = vec[2] - vec[0];
           dy = vec[3] - vec[1];
           len = sqrt(dx*dx + dy*dy);
           c1 = linpek->lgt_l;
           if (c1 <= STOL) c1 = len; 
           c2 = c1*DASHRATIO;

           dxc1 = c1*dx/len;
           dyc1 = c1*dy/len;
           dxc2 = c2*dx/len;
           dyc2 = c2*dy/len;
           dlen = 0.0;
           k++;
           aa = 0;
           i = 1;
           break;
/*
***Ett streck.
*/
           case 1:
           dlen += c1;
           if (dlen < len)
             {
             vec[0] += dxc1;
             vec[1] += dyc1;
             i = 2;
             }
           else i = 3;

           aa = VISIBLE;
           k++;
           break;
/*
***Ett mellanrum.
*/
           case 2:
           dlen += c2;
           if (dlen < len)
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             k++;
             i = 1;
             }
           else i = 3;

           aa = 0;
           break;
/*
***Stopp.
*/ 
           case 3:
           vec[0] = vec[2];
           vec[1] = vec[3];
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Lagra i x, y och a.
*/
         if (k < PLYMXV)
           {
           x[k] = vec[0];
           y[k] = vec[1];
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while (i >= 0);
/*
***Endside-information.
*/
       if ((a1 & ENDSIDE) == ENDSIDE)
         for (i = *n + 1;i <= (k + *n)/2; ++i) a[ i ] |= ENDSIDE;

       if ((a2 & ENDSIDE) == ENDSIDE)
         for (i = (k + *n)/2 + 1; i <= k; ++i) a[ i ] |= ENDSIDE;

       *n = k;
       break;
/*
***Streckprickad.
*/
       case DADOLN:
       i = 0;
       do
         {
         switch (i)
           {
/*
***Start.
*/
           case 0:
           k = *n;
           vec[ 0 ] = x[ k+1 ];    /* Startpunkt */
           vec[ 1 ] = y[ k+1 ];
           a1 = a[ k+1 ];
           vec[ 2 ] = x[ k+2 ];    /* Slutpunkt */
           vec[ 3 ] = y[ k+2 ];
           a2 = a[ k+2 ];

           dx = vec[2] - vec[0];
           dy = vec[3] - vec[1];
           len = sqrt(dx*dx + dy*dy);

           c1 = linpek->lgt_l;
           if (c1 <= STOL) c1 = len;
           c2 = c1*DASHRATIO;
           dxc1 = c1*dx/len;
           dyc1 = c1*dy/len;
           dxc2 = c2*dx/len;
           dyc2 = c2*dy/len;
           dlen = 0.0;
           k++;
           aa = 0;
           i = 1;
           break;
/*
***Ett långt streck.
*/
           case 1:
           dlen += c1;
           if (dlen < len)
             {
             vec[0] += dxc1;
             vec[1] += dyc1;
             i = 2;
             }
           else i = 5;

           aa = VISIBLE;
           k++;
           break;
/*
***Mellanrum.
*/
           case 2:
           dlen += c2;
           if (dlen < len)
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             k++;
             i = 3;
             }
           else i = 5;

           aa = 0;
           break;
/*
***Kort sterck.
*/
           case 3:
           dlen += c2;
           if (dlen < len)
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             i = 4;
             }
           else i = 5;

           aa = VISIBLE;
           k++;
           break;
/*
***Mellanrum.
*/
           case 4:
           dlen += c2;
           if (dlen < len)
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             k++;
             i = 1;
             }
           else i = 5;

           aa = 0;
           break;
/*
***Stopp.
*/
           case 5:
           vec[0] = vec[2];
           vec[1] = vec[3];
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Lagra i x, y och a.
*/
         if (k < PLYMXV)
           {
           x[k] = vec[0];
           y[k] = vec[1];
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while (i >= 0);
/*
***Endside-information.
*/
       if ((a1 & ENDSIDE) == ENDSIDE)
           for (i = *n + 1;i <= (k + *n)/2; ++i) a[ i ] |= ENDSIDE;

       if ((a2 & ENDSIDE) == ENDSIDE)
           for (i = (k + *n)/2 + 1; i <= k; ++i) a[ i ] |= ENDSIDE;
       *n = k;
       break;
       } 
/*
***Slut.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short plbarc(
        GMARC *arcpek,
        short *n,
        double x[],
        double y[],
        char a[])

/*      Bygger en 2D-cirkelbåge i form av en polylinje.
 *
 *      IN:
 *         arcpek:      Pekare till arc-struktur.
 *
 *      UT:
 *         n:           Offset till sista vektorn i polylinjen.
 *         a,y,a:       x-,y-koordinater och status hos vektorerna 
 *                      i polylinjen
 *
 *
 *      (C)microform ab 31/1/91 J. Kjellander
 *
 ******************************************************!*/

  {
    register short k=0,i;
    double cx,cy,cr,v1,v2,fi,fi1,fi2,fi3,dfi,sindfi,cosdfi,lgt,fis,fim;
    short  state;

/*
***Initiering.
*/
    cx = arcpek->x_a;
    cy = arcpek->y_a;
    cr = arcpek->r_a;
    lgt = arcpek->lgt_a;
    v1 = arcpek->v1_a*DGTORD;
    v2 = arcpek->v2_a*DGTORD;
/*
***Beräkna vinkelincrement.
*/
    dfi = 1.0/sqrt(cr*skala)/curnog;
/*
***Välj fall beroende på font.
*/
    switch (arcpek->fnt_a) {

         case SOLIDARC: {                    /* Heldragen */
              state = 0;
              do {
                   switch (state) {
                        case 0: {            /* start */
                             k = *n+1;
                             fi1 = v1;
                             fi2 = v2;
                             sindfi = SIN(dfi);
                             cosdfi = COS(dfi);
                             x[ k ] = cr*COS(fi1); /* startpunkt */
                             y[ k ] = cr*SIN(fi1);
                             a[ k ] = 0;           /* invisible */
                             state = 1;
                             break;
                        }
                        case 1: {            /* streck */
                             state = -1;
                             i = k++;
                             fi = fi1 + dfi;
                             while (fi < fi2) {
                                  if (k >= (PLYMXV-2)) { /* overflow */
                                       dfi *= 4.0;
                                       state = 0;
                                       break;
                                  }
                                  x[ k ] = x[ i ]*cosdfi - y[ i ]*sindfi;
                                  y[ k ] = y[ i ]*cosdfi + x[ i ]*sindfi;
                                  a[ k ] = VISIBLE;
                                  i = k++;
                                  fi += dfi;
                             }
                             x[ k ] = cr*COS(fi2);  /* slutpunkt */
                             y[ k ] = cr*SIN(fi2);
                             a[ k ] = VISIBLE;
                             break;
                        }
                   }
              } while (state >= 0);
              break;
         }
         case DADOARC: {                     /* punktstreckad */
              state = 0;

              do {
                   switch (state) {
                        case 1: {            /* streck */
                             fi2 = fi1 + fis; 
                             if (fi2 >= fi3) {
                                  fi2 = fi3;
                                  state = -1;
                             } else
                                  state = 2;
                             i = k++;
                             fi = fi1 + dfi;
                             while (fi < fi2) {
                                  if (k >= (PLYMXV-2)) {  /* overflow */
                                       state = 6;
                                       break;
                                  }
                                  x[ k ] = x[ i ]*cosdfi - y[ i ]*sindfi;
                                  y[ k ] = y[ i ]*cosdfi + x[ i ]*sindfi;
                                  a[ k ] = VISIBLE;
                                  i = k++;
                                  fi += dfi;
                             }
                             x[ k ] = cr*COS(fi2);    /* slutpunkt */
                             y[ k ] = cr*SIN(fi2);
                             a[ k ] = VISIBLE;
                             fi1 = fi2;
                             break;
                        } 
                        case 2: {           /* mellanrum */ 
                             state = 3;
                             fi2 = fi1 + fim; 
                             if (fi2 < fi3) {
                                  if (k >= (PLYMXV-2)) {
                                       state = 6;
                                       break;
                                  }
                                  x[ ++k ] = cr*COS(fi2); 
                                  y[ k ] = cr*SIN(fi2);
                                  a[ k ] = 0;        /* invisible */
                                  fi1 = fi2;
                             }
                             break;
                        }
                        case 3: {            /* punkt */
                             fi2 = fi1 + fim; 
                             if (fi2 >= fi3) {
                                  fi2 = fi3;
                                  state = -1;
                             } else
                                  state = 4;
                             i = k++;
                             fi = fi1 + dfi;
                             while (fi < fi2) {
                                  if (k >= (PLYMXV-2)) {
                                       state = 6;
                                       break;
                                  }
                                  x[ k ] = x[ i ]*cosdfi - y[ i ]*sindfi;
                                  y[ k ] = y[ i ]*cosdfi + x[ i ]*sindfi;
                                  a[ k ] = VISIBLE;
                                  i = k++;
                                  fi += dfi;
                             }
                             x[ k ] = cr*COS(fi2);    /* slutpunkt */
                             y[ k ] = cr*SIN(fi2);
                             a[ k ] = VISIBLE;
                             fi1 = fi2;
                             break;
                        } 
                        case 4: {           /* mellanrum */ 
                             state = 1;
                             fi2 = fi1 + fim; 
                             if (fi2 < fi3) {
                                  if (k >= (PLYMXV-2)) { /* overflow */
                                       state = 6;
                                       break;
                                  }
                                  x[ ++k ] = cr*COS(fi2); 
                                  y[ k ] = cr*SIN(fi2);
                                  a[ k ] = 0;        /* invisible */
                                  fi1 = fi2;
                             }
                             break;
                        }
                        case 0: {            /* start */
                             k = *n+1;
                             fi1 = v1;
                             fi2 = v2;
                             sindfi = SIN(dfi);
                             cosdfi = COS(dfi);
                             fi3 = fi2;
                             if (lgt <= STOL)
                                  fis = fi2;
                             else 
                                  fis = lgt/cr;
                             fim = fis*DASHRATIO;
                             x[ k ] = cr*COS(fi1);  /* startpunkt */
                             y[ k ] = cr*SIN(fi1);
                             a[ k ] = 0;            /* invisible */
                             state = 1;
                             break;
                        } 
                        case 6: {           /* overflow */ 
                             if (dfi < 0.5) {
                                  dfi *= 4.0;
                                  state = 0;

                             } else
                                  state = -1;

                             break;
                        }
                   }
              } while (state >= 0);

              break;
         }
         case DASHARC: {                         /* streckad */
              state = 0;
              do {
                   switch (state) {
                        case 1: {            /* streck */
                             fi2 = fi1 + fis; 
                             if (fi2 >= fi3) {
                                  fi2 = fi3;
                                  state = -1;
                             } else
                                  state = 2;
                             i = k++;
                             fi = fi1 + dfi;
                             while (fi < fi2) {
                                  if (k >= (PLYMXV-2)) { /* overflow */
                                       state = 6;
                                       break;
                                  }
                                  x[ k ] = x[ i ]*cosdfi - y[ i ]*sindfi;
                                  y[ k ] = y[ i ]*cosdfi + x[ i ]*sindfi;
                                  a[ k ] = VISIBLE;
                                  i = k++;
                                  fi += dfi;
                             }
                             x[ k ] = cr*COS(fi2);    /* slutpunkt */
                             y[ k ] = cr*SIN(fi2);
                             a[ k ] = VISIBLE;
                             fi1 = fi2;
                             break;
                        } 
                        case 2: {           /* mellanrum */ 
                             state = 1;
                             fi2 = fi1 + fim; 
                             if (fi2 < fi3) {
                                  if (k >= (PLYMXV-2)) { /* overflow */
                                       state = 6;
                                       break;
                                  }
                                  x[ ++k ] = cr*COS(fi2); 
                                  y[ k ] = cr*SIN(fi2);
                                  a[ k ] = 0;        /* invisible */
                                  fi1 = fi2;
                             }
                             break;
                        }
                        case 0: {            /* start */
                             k = *n+1;
                             fi1 = v1;
                             fi2 = v2;
                             sindfi = SIN(dfi);
                             cosdfi = COS(dfi);
                             fi3 = fi2;
                             if (lgt <= STOL)
                                  fis = fi2;
                             else 
                                  fis = lgt/cr;
                             fim = fis*DASHRATIO;
                             x[ k ] = cr*COS(fi1);  /* startpunkt */
                             y[ k ] = cr*SIN(fi1);
                             a[ k ] = 0;            /* invisible */
                             state = 1;
                             break;
                        } 
                        case 6: {           /* overflow */ 
                             if (dfi < 0.5) {
                                  dfi *= 4.0;
                                  state = 0;
                             } else
                                  state = -1;
                             break;
                        }
                   }
              } while (state >= 0);

              break;
         }
    }    
/*
***Translatera till cirkelns verkliga position.
*/
    for (i = *n+1; i <= (*n + k)/2; i++)
      {
      x[ i ] += cx;
      y[ i ] += cy;
      }

    for (i = (*n + k)/2 + 1; i <= k; i++) 
      {
      x[ i ] += cx;
      y[ i ] += cy;
      a[ i ] |= ENDSIDE;
      }

    *n = k;

  return(0);
}

/********************************************************/
/*!******************************************************/

        short plbtxt(
        GMTXT *txtpek,
        char  *strpek,
        short *k,
        double x[],
        double y[],
        char   a[])

/*      Bygger upp en text i form av en polylinje.
 *
 *      IN:
 *         txtpek:    Pekare till textstruktur
 *         strpek:    Pekare till textsträng
 *         k+1:       Offset till textstart
 *         xÄ k+1 Å,yÄ k+1 Å,aÄ k+1Å: Startposition med status
 * 
 *      UT:
 *         k:         Offset till textslut
 *         x,y,a:     X-,y-koordinater och status hos texten
 *
 *      FV: 0
 *
 *      (C)microform ab 4/2/91 J. Kjellander
 *
 ******************************************************!*/

  {
   register short i,j,n;
   short ntkn,veclim;
   long ix,iy;
   gmflt dx,dy,tposx,tposy,dtpos,dtposx,dtposy,cosvri,sinvri;
   gmflt kh,kb,khl;

/*
***Div. Initiering.
*/
   n = *k;
   tposx = x[n+1];
   tposy = y[n+1];
/*
***Konstanter för vridning och omvandling till radianer.
*/
   cosvri = cos(txtpek->v_tx*DGTORD);
   sinvri = sin(txtpek->v_tx*DGTORD);
/*
***Konstanter för teckenpositionering.
*/
   dtpos = txtpek->b_tx*txtpek->h_tx*0.0166666667;
   dtposx = dtpos*cosvri;
   dtposy = dtpos*sinvri;
/*
***Antal tecken.
*/
   ntkn = (short)strlen(strpek);
/*
***Konstanter för vektorgenerering.
*/
   kh = txtpek->h_tx*0.0001;
   kb = txtpek->b_tx*txtpek->h_tx*0.0000016666667;
   khl = txtpek->h_tx*txtpek->l_tx*0.000001;
/*
   kh = txtpek->h_tx*0.0131578;
   kb = txtpek->b_tx*txtpek->h_tx*0.0001179;
   khl = txtpek->h_tx*txtpek->l_tx*0.000131578;
*/

   for ( i=0; i < ntkn; i++ )
     {
     a[++n] = 0; x[n] = tposx; y[n] = tposy;
     j = pektab[(int)strpek[i]];
     veclim = j + 2*anttab[(int)strpek[i]];

     while ( j < veclim )
       {
       ix = vektab[j++];
       iy = vektab[j++] - 5000;

       if ( ix > 32767 ) 
         {
         ix -= 32768;
         if ( a[n] > 0 ) a[++n] = 0;
         }
       else
         {
         a[++n] = VISIBLE;
         }

       dx = ix*kb + iy*khl;
       dy = iy*kh;
       x[n] = tposx + dx*cosvri - dy*sinvri;
       y[n] = tposy + dy*cosvri + dx*sinvri;

       if ( n >= (PLYMXV-2) ) break;          
       }           
       
     if ( a[n] == 0 ) --n;
 
     tposx += dtposx;
     tposy += dtposy;

     if ( n >= (PLYMXV-3) ) break;          
     }

   *k = n;

   return(0); 
  }

/********************************************************/
