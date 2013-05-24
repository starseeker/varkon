/**********************************************************************
*
*    gp19.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpdrad();      Draw angular dimension
*    gpdlad();      Erase angular dimension
*    gpplad();      Make polyline
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

        short gpdrad(
        GMADM  *dimpek,
        DBptr   la,
        short   drmod)

/*      Ritar vinkelmått
 *
 *      IN: dimpek      => Pekare på vinkelmåttstruktur
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
 *      29/9/86  Ny nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/86 drmod, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    int   k;
    GMTXT txtrec;
    double x1,y1;
    char a1;

/*
***Släckt nivå eller är blankad ? 
*/
    if ( nivtb1[dimpek->hed_ad.level] ||
          dimpek->hed_ad.blank ) return(0);
/*
***Ritskalning.
*/
    dimpek->tsiz_ad /= ritskl;
    dimpek->asiz_ad /= ritskl;
/*
***Generera vektorer.
*/
    k = -1;
    gpplad(dimpek,&txtrec,&k,x,y,a);
    ncrdxy = k+1;
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
***Ev. ritning på skärm.
*/
        if ( drmod == DRAW )
          {
          if ( dimpek->hed_ad.pen != actpen ) gpspen(dimpek->hed_ad.pen);
          drwply(k,x,y,a,TRUE);
          if ( dimpek->hed_ad.hit )
            {
            x1 = txtrec.crd_tx.x_gm;
            y1 = txtrec.crd_tx.y_gm;
            a1 = 0;

            if (klpdot(-1,&x1,&y1))
                 if (!stoply(0,&x1,&y1,&a1,la,ADMTYP))
                      return(erpush("GP0012",""));
            }
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlad(
        GMADM  *dimpek,
        DBptr   la)

/*      Suddar vinkelmått
 *
 *      IN: dimpek      => Pekare på vinkelmåttstruktur
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
 *      3/9-85   Div, Ulf Johansson
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
    if (fndobj(la,ADMTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);          /* Sudda highlight */
      remobj();                          /* ta bort ur DF */
      }
/*
***Släckt nivå eller är blankad ? 
*/
    if ( nivtb1[dimpek->hed_ad.level] ||
          dimpek->hed_ad.blank ) return(0);
/*
***Sudda från skärm.
*/
    dimpek->tsiz_ad /= ritskl;
    dimpek->asiz_ad /= ritskl;

    k = -1;
    gpplad(dimpek,&txtrec,&k,x,y,a);
    if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplad(
        GMADM *dimpek,
        GMTXT *txtpek,
        int   *n,
        double x[],
        double y[],
        char typ[])

/*      Bygger vinkelmått i form av en polylinje
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
 *      5/9-85 Ulf Johansson
 *
 *      2/11/92 Bug texttransformering, J. Kjellander
 *
 ******************************************************!*/

  {
    int  i,k,ndig;
    char txt[ MAXTXT+1 ];
    char txtformat[ 16 ];
    double x3,y3,x5,y5,v1,v2,tv,r,r1,r2;
    double sinfi,cosfi,dfi;
    double d,d5,tt;
    double len,radk;
    bool flag;
    GMARC arcrec;
    GMSEG seg;

    

    k = *n;    

    radk = PI/180.0;

    x3 = dimpek->pos_ad.x_gm;                      /* cirkelcentrum */
    y3 = dimpek->pos_ad.y_gm;


    txtpek->fnt_tx = 0;
    txtpek->h_tx = dimpek->tsiz_ad;            /* Textstorlek */
    txtpek->b_tx = 60.0;
    txtpek->l_tx = 15.0;



    ndig = dimpek->ndig_ad;
    v1 = dimpek->v1_ad;                      /* Startvinkel */
    v2 = dimpek->v2_ad;                      /* Slutvinkel */  
    tv = dimpek->tv_ad;                      /* Textvinkel */
    r =  dimpek->r_ad;                       /* Radien */
    r1 = dimpek->r1_ad;                      /* startradien startv. */
    r2 = dimpek->r2_ad;                      /* Startradie slutv */

                           /* normalisera till 0.0 <= v < 360.0 */
    while (tv >= 360.0)
         tv -= 360.0;
    while (tv < 0.0)
         tv += 360.0;
    flag = (v1 != v2);
    while (v1 >= 360.0)
         v1 -= 360.0;
    while (v1 < 0.0)
         v1 += 360.0;
    while (v2 >= 360.0)
         v2 -= 360.0;
    while (v2 < 0.0)
         v2 += 360.0;
                      /* slutvinkeln ska vara större än startvinkeln */
                      /* textvinkelns relation till v1 och v2 ska bevaras */
    if (v2 <= v1)
         if (flag) {
              v2 += 360.0;
              tv += 360.0;
         }

                      /* förläng cirkelbågen på sidan närmast tv */

    if (tv > ((v1 + v2)/2.0 + 180.0)) 
         tv -= 360.0;
    else if (tv < ((v1 + v2)/2.0 - 180.0)) 
         tv += 360.0;


    d = dimpek->asiz_ad;                      /* Pilstorlek */
    d5 = d/5;

    if (dimpek->auto_ad) {             /* ska det vara måttext? */
         sprintf(txtformat,"%%0.%df\015",ndig);    /* Textformat */
         sprintf(txt,txtformat,(v2-v1));            /* Måttsträng */
         if ((len = strlen(txt)*(txtpek->b_tx*txtpek->h_tx/60.0) - 1.0) < 0.0)
              len = 0.0;
    } else
         len = 0.0;


    arcrec.fnt_a = SOLIDARC;           /* Heldragen */
    arcrec.x_a = x3;
    arcrec.y_a = y3;
    arcrec.r_a = r;
    arcrec.ns_a = 0;                    /* 2D */
    arcrec.v1_a = v1;
    arcrec.v2_a = v2;

    k = -1;
    gpplar(&arcrec,&seg,&k,x,y,typ);    /* bygg cirkelbåge */

    if (k > PLYMXV-15) {                /* Out of space */
         *n = k;    
         return(0);
    }


                                        /* dra ut cirkelbågen */

    dfi = 1.0/(r*radk);
    if (tv >= v2) {
         arcrec.v1_a = v1 - (d + d)*dfi;
         arcrec.v2_a = v1;
         gpplar(&arcrec,&seg,&k,x,y,typ);
         arcrec.v1_a = v2;
         if (tv <= 180.0)
              arcrec.v2_a = tv;
         else
              arcrec.v2_a = tv + len*dfi;
         gpplar(&arcrec,&seg,&k,x,y,typ);
    } else if (tv <= v1) {
         arcrec.v1_a = v2;
         arcrec.v2_a = v2 + (d + d)*dfi;
         gpplar(&arcrec,&seg,&k,x,y,typ);
         arcrec.v2_a = v1;
         if (tv <= 180.0)
              arcrec.v1_a = tv - len*dfi;
         else
              arcrec.v1_a = tv;
         gpplar(&arcrec,&seg,&k,x,y,typ);
    }


    if (k > PLYMXV-15) {                /* Out of space */
         *n = k;    
         return(0);
    }




    
    if ((flag = ((tv <= v1) || (tv >= v2))))
         d = -d;


    /* Bygg begr. linje 1 */

         i = k;
         x[ ++k ] = r1;     y[ k ] = 0.0;    typ[ k ] = 0;
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;
         x[ ++k ] = r + d5; y[ k ] = d;      typ[ k ] = VISIBLE; 
         x[ ++k ] = r - d5; y[ k ] = d;      typ[ k ] = VISIBLE; 
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
         x[ ++k ] = r + d5; y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

         /* Transformera till globala koordinater */
     
         sinfi = SIN(radk*v1);
         cosfi = COS(radk*v1);
         while (++i <= k) {
              tt = x[ i ];
              x[ i ] = x3 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y3 + tt*sinfi + y[ i ]*cosfi;
         }


    /* Bygg begr. linje 2 */

         i = k;
         x[ ++k ] = r2;     y[ k ] = 0.0;    typ[ k ] = 0;
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;
         x[ ++k ] = r + d5; y[ k ] = -d;     typ[ k ] = VISIBLE; 
         x[ ++k ] = r - d5; y[ k ] = -d;     typ[ k ] = VISIBLE; 
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
         x[ ++k ] = r + d5; y[ k ] = 0.0;    typ[ k ] = VISIBLE; 

         /* Transformera till globala koordinater */
     
         sinfi = SIN(radk*v2);
         cosfi = COS(radk*v2);
         while (++i <= k) {
              tt = x[ i ];
              x[ i ] = x3 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y3 + tt*sinfi + y[ i ]*cosfi;
         }

    if (flag)
         d = -d;





    if (dimpek->auto_ad) {             /* ska det vara måttext? */


         if (r != 0.0)
              dfi = ATAN(0.5*len/r)/radk;
         else
              dfi = 0.0;


         if (tv >= 360.0)
            tv -= 360.0;
         if (tv <= 180.0) {
                                     /* Rita texten rakt ned */
              txtpek->v_tx = 270.0;
              x5 = r + d5;
              y5 = 0.5*len;
              dfi = -dfi;
         } else {
                                     /* Rita texten uppåt */
              txtpek->v_tx = 90.0;
              tt = r*r - len*len*0.25;
              if (tt < 0.0)
                   x5 = -SQRT(-tt) - d5;
              else
                   x5 = SQRT(tt) - d5;
              y5 = -0.5*len;

         }
         txtpek->crd_tx.x_gm = x5;
         txtpek->crd_tx.y_gm = y5;
         txtpek->pmod_tx     = 0;
         x[ ++k ] = x5;
         y[ k-- ] = y5;
/*
***Följander rad är en bug-rättning som upptäcktes i samband
***med optimering av text-genereringen. 1:a vektorn i texten
***transformerades inte till globala koordinater. Detta hade
***ingen betydelse tidigare då texten normalt började med två
***släckta vektorer.
*/
         i = k;
/*
***Slut bugrättning 2/11/92 JK.
*/
         gppltx(txtpek,(unsigned char *)txt,&k,x,y,z,typ);

/* Transformera till globala koordinater */
  
         sinfi = SIN(radk*(tv + dfi));
         cosfi = COS(radk*(tv + dfi));

         while (++i <= k)
           {
           tt = x[ i ];
           x[ i ] = x3 + tt*cosfi - y[ i ]*sinfi;
           y[ i ] = y3 + tt*sinfi + y[ i ]*cosfi;
           }

         txtpek->crd_tx.x_gm = x3 + x5*cosfi - y5*sinfi;
         txtpek->crd_tx.y_gm = y3 + x5*sinfi + y5*cosfi;



    } else {
         txtpek->crd_tx.x_gm = x3;
         txtpek->crd_tx.y_gm = y3;
    }

    *n = k;    

    return(0);
  }

/********************************************************/
