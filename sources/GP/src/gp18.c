/**********************************************************************
*
*    gp18.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpdrrd();      Draw radius dimension
*    gpdlrd();      Erase radius dimension
*    gpplrd();      Make polyline
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

        short gpdrrd(
        GMRDM  *dimpek,
        DBptr   la,
        short   drmod)

/*      Ritar radiemått
 *
 *      IN: dimpek      => Pekare på radiemåttstruktur
 *          la          => Logisk adress i GM.
 *          drmod       => Ritmode.
 *
 *      UT: Inget.
 *
 *      FV: 0           => Ok.
 *          GP0012      => Displayfilen full.
 *
 *      (C)microform ab 27/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      4/9-85   Div, Ulf Johansson
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny Nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/86 drmod, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    int    k;
    double x1,y1;
    char   a1;
    GMTXT  txtrec;

/*
***Släckt nivå eller är blankad ? 
*/
    if ( nivtb1[dimpek->hed_rd.level] ||
          dimpek->hed_rd.blank ) return(0);
/*
***Ritskalning.
*/
    dimpek->tsiz_rd /= ritskl;
    dimpek->asiz_rd /= ritskl;
/*
***Generera vektorer.
*/
    k = -1;
    gpplrd(dimpek,&txtrec,&k,x,y,a);
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
          if ( dimpek->hed_rd.pen != actpen ) gpspen(dimpek->hed_rd.pen);
          drwply(k,x,y,a,TRUE);
          if ( dimpek->hed_rd.hit )
            {
            x1 = txtrec.crd_tx.x_gm;
            y1 = txtrec.crd_tx.y_gm;
            a1 = 0;

            if (klpdot(-1,&x1,&y1))
                 if (!stoply(0,&x1,&y1,&a1,la,RDMTYP))
                    return(erpush("GP0012",""));
            }
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlrd(
        GMRDM  *dimpek,
        DBptr   la)

/*      Suddar radiemått
 *
 *      IN: dimpek      => Pekare på radiemåttstruktur
 *          la          => Logisk adress i GM.
 *
 *      UT: Inget.
 *
 *      FV: 0           => Ok.
 *
 *      (C)microform ab 25/8-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      4/9-85   Div, Ulf Johansson
 *      14/10/86 ritskl, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      24/3/87  Släckt nivå, J. Kjellander
 *
 ******************************************************!*/

  {
    int     k;
    DBetype typ;
    GMTXT   txtrec;    

/*
***Sudda ur DF.
*/
    if (fndobj(la,RDMTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);          /* Sudda highlight */
      remobj();                          /* Avlägsna ur DF */ 
      }
/*
***Släckt nivå eller är blankad ? 
*/
    if ( nivtb1[dimpek->hed_rd.level] ||
          dimpek->hed_rd.blank ) return(0);
/*
***Sudda från skärm.
*/
    dimpek->tsiz_rd /= ritskl;
    dimpek->asiz_rd /= ritskl;

    k = -1;
    gpplrd(dimpek,&txtrec,&k,x,y,a);
    if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplrd(
        GMRDM  *dimpek,
        GMTXT  *txtpek,
        int   *n,
        double x[],
        double y[],
        char typ[])

/*      Bygger radiemått i form av en polylinje
 *
 *      IN:
 *         dimpek:     Pekare till måttsättningsstruktur
 *         txtpek:     Pekare till textstruktur
 *         n+1:        Offset till måttstart
 *
 *      UT:
 *         txtpek:     Pekare till ifylld textstruktur
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
 *      4/9-85 Ulf Johansson
 *      1998-10-01 TPMODE, J.Kjellander
 *
 ******************************************************!*/

  {
    int  i,k;
    char txt[ MAXTXT+1 ];
    char txtformat[ 16 ];

    double x1,y1,x2,y2,x3,y3;
    double fi,sinfi,cosfi,tt;
    double x5,y5,dy,dx;
    double d,d5,c,radk;
    short ndig;
    bool undef;



    radk = PI/180.0;

    k = *n;    

    ndig = dimpek->ndig_rd;
    x1 = dimpek->p1_rd.x_gm;  
    y1 = dimpek->p1_rd.y_gm;       
    x2 = dimpek->p2_rd.x_gm;  
    y2 = dimpek->p2_rd.y_gm;
    x3 = dimpek->p3_rd.x_gm;  
    y3 = dimpek->p3_rd.y_gm;

    d = dimpek->asiz_rd;                      /* Pilstorlek */
    d5 = d/5;

    txtpek->fnt_tx = 0;
    txtpek->h_tx = dimpek->tsiz_rd;            /* Textstorlek */
    txtpek->b_tx = 60.0;
    txtpek->l_tx = 15.0;


    /* Beräkna vinkeln fi för hänvisningspilen */ 

              dx = x1 - x2; dy = y1 - y2;
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
                        fi = 0.0;             /* 0/0 undefined */


    undef = ((c = SQRT(dx*dx + dy*dy)) <= 0.0);        /* Pilens längd */

    /* Bygg pil i lokalt koordinatsystem med x2,y2 i origo */


    if (!undef) {
         i = k;
         x[ ++k ] = 0.0;    y[ k ] = 0.0;    typ[ k ] = 0;
         x[ ++k ] = c;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;
         x[ ++k ] = c - d;  y[ k ] = d5;     typ[ k ] = VISIBLE; 
         x[ ++k ] = c - d;  y[ k ] = -d5;    typ[ k ] = VISIBLE; 
         x[ ++k ] = c;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

         /* Transformera till globala koordinater */
     
         sinfi = SIN(radk*fi);
         cosfi = COS(radk*fi);
         while (++i <= k) {
              tt = x[ i ];
              x[ i ] = x2 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y2 + tt*sinfi + y[ i ]*cosfi;
         }
    } 




    /* Beräkna vinkeln fi för måttlinjen */ 

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
                        fi = 0.0;             /* 0/0 undefined */


    c = SQRT(dx*dx + dy*dy);        /* Måttlinjens längd */

    /* Bygg måttlinje i lokalt koordinatsystem med x2,y2 i origo */

    i = k;
    x[ ++k ] = 0.0;    y[ k ] = 0.0;    typ[ k ] = 0;

      /* Om pilens riktning var odef. bygg på den här i stället */

    if (undef) { 
         x[ ++k ] = d;        y[ k ] = d5;     typ[ k ] = VISIBLE; 
         x[ ++k ] = d;        y[ k ] = -d5;    typ[ k ] = VISIBLE; 
         x[ ++k ] = 0.0;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
    }

    x[ ++k ] = c;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;

    sinfi = SIN(radk*fi);
    cosfi = COS(radk*fi);

    if (dimpek->auto_rd) {             /* ska det vara måttext? */

         sprintf(txtformat,"R%%0.%df",ndig);       /* Textformat */
         sprintf(txt,txtformat,dimpek->r_rd);       /* Måttsträng */

         if ((fi > 90.0) && (fi < 270.0)) {
                                     /* Rita upponer på undersidan */
              txtpek->v_tx = 180.0;
              x5 = c;  
              y5 = -d5;
         } else {
                            /* Rita förskjutet åt vänster på ovansidan */
              txtpek->v_tx = 0.0;
              x5 = c - (strlen(txt)*(txtpek->b_tx*txtpek->h_tx/60.0));
              y5 = d5;
         }
         txtpek->crd_tx.x_gm = x5;
         txtpek->crd_tx.y_gm = y5;
         txtpek->pmod_tx     = 0;
         x[ ++k ] = x5;
         y[ k-- ] = y5;
         gppltx(txtpek,(unsigned char *)txt,&k,x,y,z,typ);

         txtpek->crd_tx.x_gm = x2 + x5*cosfi - y5*sinfi; /* textstart */
         txtpek->crd_tx.y_gm = y2 + x5*sinfi + y5*cosfi;


    } else {
         txtpek->crd_tx.x_gm = x2;                 /* Brytpunkten */
         txtpek->crd_tx.y_gm = y2;
    }


    /* Transformera till globala koordinater */

    while (++i <= k) {
         tt = x[ i ];
         x[ i ] = x2 + tt*cosfi - y[ i ]*sinfi;
         y[ i ] = y2 + tt*sinfi + y[ i ]*cosfi;
    }


    *n = k;    

    return(0);
  }

/********************************************************/
