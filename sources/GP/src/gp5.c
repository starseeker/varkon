/**********************************************************************
*
*    gp5.c
*    =====
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpdrar();    Draw arc
*    gpdlar();    Erase arc
*    gpplar();    Make polyline
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
#include "../include/GP.h"
#include <math.h>

extern VY     actvy;
extern tbool  nivtb1[];
extern double ritskl;
extern double x[],y[],z[],curnog;
extern short  actpen;
extern int    ncrdxy;
extern char   a[];

/*!******************************************************/

        short gpdrar(
        GMARC *arcpek,
        GMSEG *segmnt,
        DBptr  la,
        short  drmod)

/*      Ritar en cirkelbåge.
 *
 *      IN:  arcpek => Pekare till arc-structure.
 *           segmnt => Array med max 4 segment.
 *           la     => Logisk adress till arc i GM.
 *           drmod  => Ritmode.
 *
 *      UT: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Displayfilen full.
 *
 *      (C)microform ab 31/12/84 J. Kjellander
 *
 *      REVIDERAD
 *
 *      3/9-85   Div, Ulf Johansson
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/86 drmod, J. Kjellander
 *      27/12/86 hit, J. KJellander
 *      1997-12-29, Breda linjer, J.Kjellander
 *
 ******************************************************!*/

  {
    short err;
    int   k;

/*
*** Kanske arc:en ligger på en släckt nivå eller är blankad ? 
*/
    if ( nivtb1[arcpek->hed_a.level] ||
          arcpek->hed_a.blank) return(0);
/*
***Ev. skalning av strecklängd.
*/
    if ( arcpek->fnt_a > 0 ) arcpek->lgt_a /= ritskl;
/*
***Generera vektorer.
*/
    k = -1;
    if ( (err=gpplar(arcpek,segmnt,&k,x,y,a)) < 0 ) return(err);
/*
***Ev. klippning.
*/
    if ( drmod > GEN )
      {
      ncrdxy = 0;
      if (klpply(-1,&k,x,y,a))
        {
        ncrdxy = k+1;
/*
***Ritning och lagring i df.
*/
        if ( drmod == DRAW )
          {
          if ( arcpek->hed_a.pen != actpen ) gpspen(arcpek->hed_a.pen);
          if ( arcpek->wdt_a != 0.0 ) gpswdt(arcpek->wdt_a);
          if ( arcpek->hed_a.hit )
            {
            if ( stoply(k,x,y,a,la,ARCTYP) ) drwobj(TRUE);
            else return(erpush("GP0012",""));
            }
          else
            {
            drwply(k,x,y,a,TRUE);
            }
          if ( arcpek->wdt_a != 0.0 ) gpswdt(0.0);
          }
        }
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpdlar(
        GMARC *arcpek,
        GMSEG *segmnt,
        DBptr la)

/*      Stryker en arc med adress la ur display-
 *      filen och från skärmen.
 *
 *      In: la  => Arc:ens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 6/12-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9-85   Div, Ulf Johansson
 *      14/10/86 ritskl, J. Kjellander
 *      27/12/86 hit, J. KJellander
 *      23/3/87  Släckt nivå, J. Kjellander
 *      1997-12-29, Breda linjer, J.Kjellander
 *
 ******************************************************!*/

  {
    int     k;
    DBetype typ;

/*
***Stryk ur DF.
*/
    if (fndobj(la,ARCTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);
      remobj();
      }
/*
***Och från skärmen.
*/
    if ( nivtb1[arcpek->hed_a.level] || arcpek->hed_a.blank)
      {
      return(0);
      }
    else
      {
      if ( arcpek->fnt_a > 0 ) arcpek->lgt_a /= ritskl;
      k = -1;
      gpplar(arcpek,segmnt,&k,x,y,a);
      if (klpply(-1,&k,x,y,a))
        {
        if ( arcpek->wdt_a != 0.0 ) gpswdt(arcpek->wdt_a);
        drwply(k,x,y,a,FALSE);
        if ( arcpek->wdt_a != 0.0 ) gpswdt(0.0);
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplar(
        GMARC *arcpek,
        GMSEG *segmnt,
        int   *n,
        double x[],
        double y[],
        char a[])

/*      Bygger en cirkelbåge i form av en polylinje.
 *
 *      Först projiceras segmentmatriserna på det aktuella vy-
 *      planet. Därefter beräknas ett lämpligt antal punkter på
 *      den resulterande 2-dimensionella cirkelbågen.
 *
 *      IN:
 *         arcpek:      Pekare till arc-struktur.
 *         segmnt:      Array med max 4 segment.
 *
 *      UT:
 *         n:           Offset till sista vektorn i polylinjen.
 *         a,y,a:       x-,y-koordinater och status hos vektorerna 
 *                      i polylinjen
 *
 *      FV:
 *         0 =>         Ok.
 *         GP0023 =>    Viktterm < 0
 *
 *      (C)microform ab 31/12/84 J. Kjellander
 *
 *      REVIDERAD
 *
 *      25/8/85  2D-cirkel, Ulf Johansson
 *      17/10/85 dsdu = 0.0, J. Kjellander
 *      14/11/86 nvec = PLYMXV/4, J. Kjellander
 *      3/8/87   Bättre vektorgen. J. Kjellander
 *      9/2/89   hide, J. Kjellander
 *      9/12/91  gpplcu() i 3D, J. Kjellander
 *      1996/11/06 nsgr_cu, J. Kjellander
 *
 ******************************************************!*/

  {
    int    k=0,i;
    double cx,cy,cr,v1,v2,fi,fi1,fi2,fi3,dfi,
           sindfi,cosdfi,lgt,fis,fim;
    short  state;
    GMCUR  cur;


/*
***2D-cirkel.
*/
    if ( arcpek->ns_a == 0 )
      {
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
      dfi = 0.1*sqrt((actvy.vywin[ 2 ] - actvy.vywin[ 0 ])/cr)/curnog;
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

      for (i = *n+1; i <= (*n + k)/2; i++) {
           x[ i ] += cx;
           y[ i ] += cy;
      }
      for (i = (*n + k)/2 + 1; i <= k; i++) {
           x[ i ] += cx;
           y[ i ] += cy;
           a[ i ] |= ENDSIDE;
      }
  *n = k;
 }


/***************************************************************/

/*
***3-D cirkel, använd samma rutin som kurvor dvs. gpplcu i gp8.c.
***Låtsas att det är en kurva genom att skapa en temporär GMCUR-
***post och skicka med cirkelns segment. 
*/
   else
     {
     cur.hed_cu.type = CURTYP;
     cur.ns_cu = cur.nsgr_cu = arcpek->ns_a;
     cur.fnt_cu = arcpek->fnt_a;
     cur.lgt_cu = arcpek->lgt_a;
     cur.al_cu = arcpek->al_a;
     cur.plank_cu = FALSE;
     gpplcu(&cur,segmnt,n,x,y,z,a);
     }
/*
***Sätt ncrdxy, används tex. av autozoom !
*/
   ncrdxy = *n+1;

   return(0);
}

/********************************************************/
