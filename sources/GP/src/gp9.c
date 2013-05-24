/**********************************************************************
*
*    gp9.c
*    =====
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpdrld();      Draw linear dimension
*    gpdlld();      Erase linear dimension
*    gpplld();      Make polyline
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

extern tbool  nivtb1[];

extern double x[],y[],z[],ritskl;
extern short  actpen;
extern int    ncrdxy;
extern char   a[];

/*!******************************************************/

        short gpdrld(
        GMLDM  *dimpek,
        DBptr   la,
        short   drmod)

/*      Ritar längdmått
 *
 *      IN: dimpek      => Pekare på längdmåttstruktur
 *          la          => Logisk adress i GM.
 *          drmod       => Ritmode.
 *
 *      UT: Inget.
 *
 *      FV: 0           => Ok.
 *          GP0012      => Displayfilen full.
 *
 *      (C)microform ab 10/7-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      3/9-85   Div, Ulf johansson
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/68 drmod, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    int k;

/*
***Släckt nivå eller är blankad ?
*/
    if ( nivtb1[dimpek->hed_ld.level] ||
          dimpek->hed_ld.blank ) return(0);
/*
***Ritskalning av text och pilar.
*/
    dimpek->tsiz_ld /= ritskl;
    dimpek->asiz_ld /= ritskl;
/*
***Generera vektorer.
*/
    x[ 0 ] = dimpek->p3_ld.x_gm;
    y[ 0 ] = dimpek->p3_ld.y_gm;
    a[ 0 ] = 0;

    k = -1;
    gpplld(dimpek,&k,x,y,a);
    ncrdxy = k+1;
/*
***Ev. klippning av vektorer.
*/
    if ( drmod > GEN )
      {
      ncrdxy = 0;
      if (klpply(-1,&k,x,y,a))
        {
        ncrdxy = k+1;
/*
***Ev. ritning på skärm.
*/
        if ( drmod == DRAW )
          {
          if ( dimpek->hed_ld.pen != actpen ) gpspen(dimpek->hed_ld.pen);
          drwply(k,x,y,a,TRUE);

          if ( dimpek->hed_ld.hit )
            {
            x[ 0 ] = dimpek->p3_ld.x_gm;
            y[ 0 ] = dimpek->p3_ld.y_gm;
            a[ 0 ] = 0;

            if (klpdot(-1,x,y))
                if (!stoply(0,x,y,a,la,LDMTYP))
                     return(erpush("GP0012",""));
            }
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlld(
        GMLDM  *dimpek,
        DBptr   la)

/*      Suddar längdmått
 *
 *      IN: dimpek      => Pekare på längdmåttstruktur
 *          la          => Logisk adress i GM.
 *
 *      UT: Inget.
 *
 *      FV: 0           => Ok.

 *
 *      (C)microform ab 10/7-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      3/9-85   Div, Ulf johansson
 *      14/10/86 ritskl, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      24/3/87  Släckt nivå, J. Kjellander
 *
 ******************************************************!*/

  {
    int     k;
    DBetype typ;
    
/*
***Kolla om måttet finns i df.
*/
    if (fndobj(la,LDMTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);
      remobj();
      }
/*
***Släckt nivå eller är blankad ?
*/
    if ( nivtb1[dimpek->hed_ld.level] ||
          dimpek->hed_ld.blank ) return(0);
/*
***Ritskalning av text och pilar.
*/
    dimpek->tsiz_ld /= ritskl;
    dimpek->asiz_ld /= ritskl;
/*
***Måttets läge.
*/
    x[ 0 ] = dimpek->p3_ld.x_gm;
    y[ 0 ] = dimpek->p3_ld.y_gm;
    a[ 0 ] = 0;
/*
***Generera vektorer och sudda från skärm.
*/
    k = -1;
    gpplld(dimpek,&k,x,y,a);
    if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplld(
        GMLDM  *dimpek,
        int    *n,
        double  x[],
        double  y[],
        char    typ[])

/*      Bygger längdmått i form av en polylinje
 *
 *      IN:
 *         dimpek:     Pekare till måttsättningsstruktur
 *         n+1:        Offset till måttstart (textens position)
 *         xÄ k+1 Å,
 *         yÄ k+1 Å,
 *         typÄ k+1 Å: Startposition med status
 *
 *      UT:
 *         n:          Offset till måttslut
 *         x,y,typ:    X-,y-koordinater och status hos måttet
 *
 *      FV: 
 *          
 *
 *      (c) Microform AB 10/7-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      4/9-85  Ulf Johansson
 *      23/3/93 Nyuppstånden bug på DIAB, J. Kjellander
 *
 ******************************************************!*/

  {
    int   i,k;
    char  txt[ MAXTXT+1 ];
    char  txtformat[ 16 ];

    double x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
    double fi,sinfi,cosfi;
    double a,b,d,e,d5,tt;
    double xp2,xp3,xp6,xp7,yp1,yp4;
    double dy,dx,pa,radk;
    short  dimtyp,ndig;

    GMTXT txtrec;

    radk = PI/180.0;

    k = *n;    
    x5 = x[ k+1 ];                          /* Textens placering */
    y5 = y[ k+1 ];


    dimtyp = dimpek->dtyp_ld;                 /* Måtttyp */

    ndig = dimpek->ndig_ld;
    x1 = dimpek->p1_ld.x_gm;                 /* Start */
    y1 = dimpek->p1_ld.y_gm;       
    x4 = dimpek->p2_ld.x_gm;                 /* Slut */
    y4 = dimpek->p2_ld.y_gm;
    if ((x1 == x4) && (y1 == y4))
         return(0);

    d = dimpek->asiz_ld;                      /* Pilstorlek */
    d5 = d/5;


    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimpek->tsiz_ld;            /* Textstorlek */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;


    /* Beräkna vinkeln och x2,y2,x3,y3 */ 

    switch (dimtyp) {

         case LDHORIZON: {                     /* Horisontellt mått */
              if ((y5 > y1) && (y5 > y4)) { 
                   fi = 180.0;
                   if (x4 > x1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 0.0;
                   if (x4 < x1) {  
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }
              x2 = x1;
              x3 = x4;
              y2 = y3 = y5;
              break;
         }

         case LDVERTIC: {                      /* Vertikalt mått */
              if ((x5 < x1) && (x5 < x4)) { 
                   fi = 270.0;
                   if (y4 > y1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 90.0;
                   if (y4 < y1) {  
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }    
              x2 = x3 = x5;
              y2 = y1;
              y3 = y4;
              break;
         }

         case LDPARALL: {                      /* Parallelt mått */
              dy = x1 - x4; dx = y4 - y1;
              tt = SQRT(dx*dx + dy*dy);
              dx /= tt; dy /= tt;
              pa = dx*(x5 -x1) + dy*(y5 -y1);
              if (pa != 0.0) {
                   if (pa < 0.0) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                        dx = -dx; dy = -dy;
                        pa = -pa;
                   }
                   x2 = x1 + pa*dx; y2 = y1 + pa*dy;
                   x3 = x4 + x2 - x1; y3 = y4 + y2 - y1;
              } else {
                   x2 = x3 = y2 = y3 = 0;
              }

              dx = x3 - x2; dy = y3 - y2;
              if (dx > 0.0) 
                   if (dy > 0.0) 
                        fi = ATAN(dy/dx)/radk;
                   else if (dy < 0.0)
                        fi = 360.0 - ATAN(-(dy/dx))/radk;
                   else
                        fi = 0.0;
              else if (dx < 0.0)
                   if (dy > 0.0)
                        fi = 180.0 - ATAN(-(dy/dx))/radk;
                   else if (dy < 0.0)
                        fi = 180.0 + ATAN(dy/dx)/radk;
                   else
                        fi = 180.0;
              else
                   if (dy > 0.0) 
                        fi = 90.0;
                   else if (dy < 0.0) 
                        fi = 270.0;
                   else
                        fi = 0.0;             /* undefined */

              break;
         }
    }


    a = SQRT((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
    b = SQRT((x5 - x2)*(x5 - x2) + (y5 - y2)*(y5 - y2));
    e = SQRT((x5 - x3)*(x5 - x3) + (y5 - y3)*(y5 - y3));
    yp1 = SQRT((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));    
    yp4 = SQRT((x4 - x3)*(x4 - x3) + (y4 - y3)*(y4 - y3));    

    switch (dimtyp) {

         case LDHORIZON: {                     /* Horisontellt mått */
              if (fi == 0.0) {
                   if (y1 < y5)
                        yp1 = -yp1;
                   if (y4 < y5)
                        yp4 = -yp4;
              }
              break;
         }

         case LDVERTIC: {                      /* Vertikalt mått */
              if (fi == 90.0) {
                   if (x1 > x5)
                        yp1 = -yp1;
                   if (x4 > x5)
                        yp4 = -yp4;
              }
              break;
         }

         case LDPARALL: {                      /* Parallelt mått */
              break;
         }
    }


    sprintf(txtformat,"%%0.%df",ndig);
    sprintf(txt,txtformat,a);                   /* Bygg måtttext */

/*
***Av någon anledning har -strlen(txt)*txtrec.... slutat
***fungera i och med 1.11 på DIAB-maskinerna. Ett par extra
***parenteser löser problemet, fråga inte mig varför.
***DIAB!!!!
*/

    if ((e > a) && (b < e)) {               /* Vänster */

         xp2 = b;
         xp3 = e;
         xp7 = e + 2*d;      
         if ((fi > 90.0) && (fi <= 270.0))
              xp6 = -(strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0);
         else
              xp6 = 0.0;  
    } else if ((b > a) && (e < b)) {
                xp6 = -(b + 2*d);            /* Höger */
                xp2 = -b; 
                xp3 = -e;
                if ((fi <= 90.0) || (fi > 270.0))
                     xp7 = strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0;
                else
                     xp7 = 0.0;  
           } else {
                xp3 = xp7 = e;
                xp2 = xp6 = -b;          /* Mellan */
           }



    /* Bygg måttet */ 

    x[ ++k ] = xp2;      y[ k ] = yp1;    typ[ k ] = 0;
    x[ ++k ] = xp2;      y[ k ] = -d5;    typ[ k ] = VISIBLE; 
    x[ ++k ] = xp3;      y[ k ] = yp4;    typ[ k ] = 0;
    x[ ++k ] = xp3;      y[ k ] = -d5;    typ[ k ] = VISIBLE; 
    x[ ++k ] = xp6;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp7;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

    if (xp2*xp3 > 0.0)
         d = -d;

    x[ ++k ] = xp2;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp2 + d;  y[ k ] = d5;     typ[ k ] = VISIBLE; 
    x[ ++k ] = xp2 + d;  y[ k ] = -d5;    typ[ k ] = VISIBLE; 
    x[ ++k ] = xp2;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

    x[ ++k ] = xp3;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp3 - d;  y[ k ] = d5;     typ[ k ] = VISIBLE; 
    x[ ++k ] = xp3 - d;  y[ k ] = -d5;    typ[ k ] = VISIBLE; 
    x[ ++k ] = xp3;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

    x[ ++k ] = 0;                         typ[ k ] = 0;

    if ((fi > 90.0) && (fi <= 270.0))
         y[ k ] = -2*d5;
    else
         y[ k ] = 2*d5;





    /* Transformera till lokalt koordinatsystem */


    if (fi == 0.0)                        /* cos=1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] += x5;
              y[ i ] += y5;
         }
    else if (fi == 90.0)                  /* cos=0 sin=1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 - y[ i ];
              y[ i ] = y5 + tt;
         }
    else if (fi == 180.0)                 /* cos=-1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] = x5 - x[ i ];
              y[ i ] = y5 - y[ i ];
         }
    else if (fi == 270.0)                 /* cos=0 sin=-1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + y[ i ];
              y[ i ] = y5 - tt;
         }
    else {
         sinfi = SIN(radk*fi);
         cosfi = COS(radk*fi);
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y5 + tt*sinfi + y[ i ]*cosfi;
         }
    }


                                             /* Måttsträngens vinkel */
    if ((fi > 90.0) && (fi <= 270.0)) {
         if (fi <= 180.0)
              txtrec.v_tx = fi + 180.0;
         else
              txtrec.v_tx = fi - 180.0;
    } else
         txtrec.v_tx = fi;

    txtrec.crd_tx.x_gm = x[ k ];
    txtrec.crd_tx.y_gm = y[ k-- ];
    txtrec.pmod_tx     = 0;

    if (dimpek->auto_ld)
         gppltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);

    *n = k;    

    return(0);
  }

/********************************************************/
