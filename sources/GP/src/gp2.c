/**********************************************************************
*
*    gp2.c
*    =====
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrpo();    Draw point
*    gpdlpo();    Erase point
*    plydot();    Make polyline
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

extern VY     actvy;
extern tbool  nivtb1[];
extern short  actpen;
extern int    ncrdxy;
extern double k1x,k2x,k1y,k2y;
extern double x[],y[],z[];
extern char   a[];

/*!******************************************************/

        short gpdrpo(
        GMPOI *poipek,
        DBptr  la,
        short  drmod)

/*      Ritar en punkt.
 *
 *      Lagrar i displayfilen (DF) om den matematiska punkten
 *      ligger innanför bilskärmen.
 *      Ritar på bildskärmen om den grafiska punkten ligger
 *      innanför bildskärmen.
 *
 *      In: poipek => Pekare till punkt-structure
 *          la     => Logisk adress till punkt i GM.
 *          drmod  => Generera/Rita/Lagra
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Display-filen full.
 *
 *      (C)microform ab 27/12/84 J. Kjellander
 *
 *      REVIDERAD:
 *  
 *      4/9-85   Ulf Johansson
 *      26/1/86  Penna, J. Kjellander
 *      29/9/86  Ny nivåhant. R. Svedin
 *      15/10/86 drmod, J. Kjellander
 *      20/10/86 projvy, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      27/2/89  hide, J. Kjellander
 *      27/12/94 wpdrpo(), J. Kjellander
 *      1998-04-03 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

    {
    int k;
    
/*
***Kanske punkten ligger på en släckt nivå eller är blankad ?
*/
    if ( nivtb1[poipek->hed_p.level] ||
         poipek->hed_p.blank) return(0);
/*
***Projicera punkten på vy-planet.
*/
   gppltr(&(poipek->crd_p),x,y,z);
   ncrdxy = 1;
   if ( actvy.vydist != 0.0 ) gppstr(x,y,z);
/*
***Om CLIP, klipp punkten och generera vektorer.
*/
    if ( drmod == CLIP )
      {
      ncrdxy = 0;
      if (klpdot(-1,x,y))
        {
        k = -1;
        plydot(poipek,&k,x,y,a);
        ncrdxy = k+1;
        return(0);
        }
      }
/*
***Om DRAW, rita på skärmen.
*/
    else if ( drmod == DRAW )
      {
      if (klpdot((short)-1,x,y))
        {
        if ( poipek->hed_p.pen != actpen ) gpspen(poipek->hed_p.pen);
        if ( poipek->wdt_p != 0.0 ) gpswdt(poipek->wdt_p);
        gpdmrk((short)(k1x + k2x*x[0]),(short)(k1y + k2y*y[0]),KRYSS,TRUE);
        if ( poipek->wdt_p != 0.0 ) gpswdt(0.0);
/*
***Om hit lagra i df.
*/
        if ( poipek->hed_p.hit )
          {
          if ( !stoply(0,x,y,a,la,POITYP) )
                                return(erpush("GP0012",""));
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlpo(
        GMPOI *poipek,
        DBptr la)

/*      Stryker en punkt med adress la ur display-
 *      filen och från skärmen.
 *
 *      In: poipek = Pekare till punktpost.
*           la     => Punktens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 6/12-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      4/9/85   Bug, Ulf Johansson
 *      27/12/86 hit, J. Kjellander
 *      23/3/87  Släckt nivå, J. Kjellander
 *      1998-04-03 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype typ;
    
/*
***Sudda punkten ur df om den finns där.
*/
    if (fndobj(la,POITYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);
      remobj();
      }
/*
***Kanske punkten ligger på en släckt nivå eller är blankad ?
*/
    if ( nivtb1[poipek->hed_p.level] ||
         poipek->hed_p.blank) return(0);
/*
***Projicera punkten på vy-planet.
*/
   gppltr(&(poipek->crd_p),x,y,z);
   ncrdxy = 1;
   if ( actvy.vydist != 0.0 ) gppstr(x,y,z);
/*
***Sudda från skärm.
*/
    if ( poipek->wdt_p != 0.0 ) gpswdt(poipek->wdt_p);
    gpdmrk((short)(k1x + k2x*x[0]),(short)(k1y + k2y*y[0]),KRYSS,FALSE);
    if ( poipek->wdt_p != 0.0 ) gpswdt(0.0);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short plydot(
        GMPOI *poipek,
        int   *n,
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
 *      (C)microform ab 27/12/84 J. Kjellander
 *
 *      REVIDERAD:
 *  
 *         13/9-85 Ulf Johansson
 *
 ******************************************************!*/

 {
     int    k;
     double xp,yp,zp,dpx,dpy;


     k = *n;

     dpx = DOTSIZ*2.0/k2x;
     dpy = DOTSIZ*2.0/k2y;

     xp = x[ k+1 ];
     yp = y[ k+1 ];
     zp = z[ k+1 ];


    /* Bygg ett kryss */

                                            /* Övre högra hörnet */

     x[++k] = xp + dpx; y[k] = yp + dpy; z[k] = zp; a[k] = 0;

                                            /* Nedre vänstra */

     x[++k] = xp - dpx; y[k] = yp - dpy; z[k] = zp; a[k] = VISIBLE;

                                            /* Övre vänstra hörnet  */

     x[++k] = xp - dpx; y[k] = yp + dpy; z[k] = zp; a[k] = 0;

                                            /* Nedre högra */

     x[++k] = xp + dpx; y[k] = yp - dpy; z[k] = zp; a[k] = VISIBLE;

     *n = k;

     return(0);
 }

/********************************************************/
