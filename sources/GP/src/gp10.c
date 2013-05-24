/**********************************************************************
*
*    gp10.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrli();    Draw line
*    gpdlli();    Erase line
*    gpprli();    Project line to active view
*    gpplli();    Make polyline
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

extern DBTmat actvym;
extern VY     actvy;
extern tbool  nivtb1[];
extern double x[],y[],z[],ritskl,k2x;
extern short  actpen;
extern int    ncrdxy;
extern char   a[];
extern bool   gpgenz,pltflg;

/*!******************************************************/

        short gpdrli(
        GMLIN  *linpek,
        DBptr   la,
        short   drmod)

/*      Ritar en linje.
 *
 *      IN: linpek => Adress till linje-structure.
 *          la     => Logisk adress i GM.
 *          drmod  => Ritmode
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Displayfilen full.
 *
 *      (C) microform ab 12/11-84 J. Kjellander
 *
 *      REVIDERAD:
 *  
 *      3/9-85   Div, Ulf Johansson
 *      27/1/86  penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      14/10/86 ritskl, J. Kjellander
 *      15/10/86 rdmod, J. Kjellander
 *      19/10/86 projvy, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      18/2/87  plotbug, J. Kjellander
 *      3/2/89   hide, J. Kjellander
 *      1997-12-29 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

  {
    int k;

/*
***Släckt nivå eller är blankad ? 
*/
    if ( nivtb1[linpek->hed_l.level] ||
          linpek->hed_l.blank ) return(0);
/*
***Projicering.
*/
    gpprli(linpek);
/*
***Vektorer för streckade eller streckprickade linjer
***genereras bara om klippning beställts. Av prestanda-
***skäl är det lämpligt att klippa innan streckad polylinje
***genereras. Rutiner som behöver streckad linjes polylinje
***måste alltså acceptera att den först klipps. Detta duger
***dock inte i samband med hide eftersom klippet bara är 2D.
***hide anropar istället gpplli() själv och klipper sedan med
***egen 3D-klipprutin.
*/
    if ( drmod > GEN )
      {
      a[ 0 ] = 0;
      a[ 1 ] = (VISIBLE | ENDSIDE);
      ncrdxy = 0;
      if (klplin(-1,x,y,a))
        {
/*
***Ev. lagring i df.
*/
        if ( drmod == DRAW && linpek->hed_l.hit )
          {
          if (!stoply(1,x,y,a,la,LINTYP))
                          return(erpush("GP0012",""));
          }
/*
***Generera vektorer.
*/
        k = -1;
        gpplli(linpek,&k,x,y,a);
        ncrdxy = k+1;
/*
***Ev. ritning på skärm.
*/
        if ( drmod == DRAW )
          {
          if ( linpek->hed_l.pen != actpen ) gpspen(linpek->hed_l.pen);
          if ( linpek->wdt_l != 0.0 ) gpswdt(linpek->wdt_l);
          drwply(k,x,y,a,TRUE);
          if ( linpek->wdt_l != 0.0 ) gpswdt(0.0);
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlli(
        GMLIN  *linpek,
        DBptr la)

/*      Stryker en linje med adress la ur display-
 *      filen och från skärmen.
 *
 *      In: la  => Linjens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *          GP0032 => Finns ej i DF
 *
 *      (C)microform ab 6/12-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9-85   Genomgång, Ulf Johansson
 *      11/2/86  Penna, J. Kjellander
 *      14/10/86 ritskl, J. Kjellander
 *      23/3/87  Släckt nivå, J. Kjellander
 *      3/2/89   hide, J. Kjellander
 *      1997-12-29 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

{
    int     k;
    DBetype typ;

/*
***Sudda linjen ur df om den finns där.
*/
    if (fndobj(la,LINTYP,&typ))
      {
      gphgen(la,HILIINVISIBLE);
      remobj();
      }
/*
***Släckt nivå eller blankad ? 
*/
    if ( nivtb1[linpek->hed_l.level] ||
          linpek->hed_l.blank ) return(0);
/*
***Projicering.
*/
    gpprli(linpek);
/*
***Sudda linjen från skärmen.
*/
    a[ 0 ] = 0;
    a[ 1 ] = (VISIBLE | ENDSIDE);

    if (klplin(-1,x,y,a))
      {
      k = -1;
      gpplli(linpek,&k,x,y,a);        /* Generera vektorer */
      if ( linpek->wdt_l != 0.0 ) gpswdt(linpek->wdt_l);
      drwply(k,x,y,a,FALSE);
      if ( linpek->wdt_l != 0.0 ) gpswdt(0.0);
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpprli(
        GMLIN  *linpek)

/*      Projicerar en linje till XY-planet i aktiv vy.
 *
 *      IN: linpek => Adress till linje-structure.
 *
 *      Ut: Lagrar projicerade ändkoordinater i de
 *          globala vektorerna x, y och ev. z samt
 *          lagrar antal koordinater, dvs. 2 i ncrdxy.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/2-89 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Ritskalning av strecklängd.
*/
      if ( linpek->fnt_l > 0 ) linpek->lgt_l /= ritskl;
/*
***Om det är en 3D-vy måste linjens X- och Y-
***koordinater plan-projiceras till aktiv vy.
*/
    if ( actvy.vy3d )
      {
      x[0] = actvym.g11*linpek->crd1_l.x_gm +
             actvym.g12*linpek->crd1_l.y_gm +
             actvym.g13*linpek->crd1_l.z_gm;
      y[0] = actvym.g21*linpek->crd1_l.x_gm +
             actvym.g22*linpek->crd1_l.y_gm +
             actvym.g23*linpek->crd1_l.z_gm;
      x[1] = actvym.g11*linpek->crd2_l.x_gm +
             actvym.g12*linpek->crd2_l.y_gm +
             actvym.g13*linpek->crd2_l.z_gm;
      y[1] = actvym.g21*linpek->crd2_l.x_gm +
             actvym.g22*linpek->crd2_l.y_gm +
             actvym.g23*linpek->crd2_l.z_gm;
/*
***Om Z-koordinater har beställts, måste även dessa
***plan-projiceras till aktiv vy.
*/
      if ( gpgenz )
        {
        z[0] = actvym.g31*linpek->crd1_l.x_gm +
               actvym.g32*linpek->crd1_l.y_gm +
               actvym.g33*linpek->crd1_l.z_gm;
        z[1] = actvym.g31*linpek->crd2_l.x_gm +
               actvym.g32*linpek->crd2_l.y_gm +
               actvym.g33*linpek->crd2_l.z_gm;
        }
      }
/*
***Om inte 3D-vy, ta ändkoordinater som dom är.
*/
    else
      {
      x[0] = linpek->crd1_l.x_gm;
      y[0] = linpek->crd1_l.y_gm;
      x[1] = linpek->crd2_l.x_gm;
      y[1] = linpek->crd2_l.y_gm;
      if ( gpgenz )
        {
        z[0] = linpek->crd1_l.z_gm;
        z[1] = linpek->crd2_l.z_gm;
        }
      }

   ncrdxy = 2;
/*
***Ev. perspektiv-transformation.
*/
   if ( actvy.vydist != 0.0 ) gppstr(x,y,z);

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short gpplli(
        GMLIN  *linpek,
        int    *n,
        double  x[],
        double  y[],
        char    a[])

/*      En heldragen linje expanderas till en streckad eller
 *      punktstreckad polylinje.
 *      
 *      In:
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
 *      (C)microform ab 15/7-85 Ulf Johansson
 *
 *      REVIDERAD:
 *
 *      15/10/86 ncrdxy, J. Kjellander
 *      10/2/92  3D-streck, J. Kjellander
 *
 ******************************************************!*/

 {
    int    i,k=0;
    char   aa=0,a1=0,a2=0;
    double vec[4],len,dlen,dx,dy,dz,c1,c2,dxc1,dyc1,dzc1,
           dxc2,dyc2,dzc2,vecz1,vecz2;

    
    switch (linpek->fnt_l)
      {
/*
***Heldragen.
*/
       case SOLIDLN:
       *n += 2;
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
           if ( gpgenz )
             {
             vecz1 = z[ k+1 ];
             vecz2 = z[ k+2 ];
             dz = vecz2 - vecz1;
             }

           len = SQRT(dx*dx + dy*dy);
           c1 = linpek->lgt_l;
           c2 = c1*DASHRATIO;
/*
***Om (den ev. projicerade) linjens längd är mindre (tex. 0.0)
***än att det räcker till 2 streck skall den ritas heldragen.
***Detta åstadkommer vi genom att sätta c1 > len.
*/
           if ( len < 1.5*c1 + c2 ) c1 = len + 1.0;
           else
             {
             dxc1 = c1*dx/len;
             dyc1 = c1*dy/len;
             dxc2 = c2*dx/len;
             dyc2 = c2*dy/len;
             if ( gpgenz ) { dzc1 = c1*dz/len; dzc2 = c2*dz/len; }
             }
/*
***Om strecklängden i förhållande till skärmskalan är så
***liten att mellanrummen (c2) mellan strecken inte skulle
***synas ritar vi en heldragen linje.
*/
           if ( c2*k2x < 1.0  &&  !pltflg ) c1 = len + 1.0;

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
           if ( dlen < len )
             {
             vec[0] += dxc1;
             vec[1] += dyc1;
             if ( gpgenz ) vecz1 += dzc1;
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
           if ( dlen < len )
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             if ( gpgenz ) vecz1 += dzc2;
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
           if ( gpgenz ) vecz1 = vecz2;
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Lagra i x, y och a.
*/
         if ( k < PLYMXV )
           {
           x[k] = vec[0];
           y[k] = vec[1];
           if ( gpgenz ) z[k] = vecz1;
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while ( i >= 0 );
/*
***Endside-information.
*/
       if ( (a1 & ENDSIDE) == ENDSIDE )
         for (i = *n + 1;i <= (k + *n)/2; ++i) a[ i ] |= ENDSIDE;

       if ( (a2 & ENDSIDE) == ENDSIDE )
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
/*
***Prickad.
*/
       case DOTLN:
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
           if ( gpgenz )
             {
             vecz1 = z[ k+1 ];
             vecz2 = z[ k+2 ];
             dz = vecz2 - vecz1;
             }

           len = SQRT(dx*dx + dy*dy);
           c1 = linpek->lgt_l;
           c2 = c1*DOTRATIO;
/*
***Om (den ev. projicerade) linjens längd är mindre (tex. 0.0)
***än att det räcker till 2 streck skall den ritas heldragen.
***Detta åstadkommer vi genom att sätta c1 > len.
*/
           if ( len < 1.5*c1 + c2 ) c1 = len + 1.0;
           else
             {
             dxc1 = c1*dx/len;
             dyc1 = c1*dy/len;
             dxc2 = c2*dx/len;
             dyc2 = c2*dy/len;
             if ( gpgenz ) { dzc1 = c1*dz/len; dzc2 = c2*dz/len; }
             }
/*
***Om strecklängden i förhållande till skärmskalan är så
***liten att mellanrummen (c2) mellan strecken inte skulle
***synas ritar vi en heldragen linje.
*/
           if ( c2*k2x < 1.0  &&  !pltflg ) c1 = len + 1.0;

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
           if ( dlen < len )
             {
             vec[0] += dxc1;
             vec[1] += dyc1;
             if ( gpgenz ) vecz1 += dzc1;
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
           if ( dlen < len )
             {
             vec[0] += dxc2;
             vec[1] += dyc2;
             if ( gpgenz ) vecz1 += dzc2;
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
           if ( gpgenz ) vecz1 = vecz2;
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Lagra i x, y och a.
*/
         if ( k < PLYMXV )
           {
           x[k] = vec[0];
           y[k] = vec[1];
           if ( gpgenz ) z[k] = vecz1;
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while ( i >= 0 );
/*
***Endside-information.
*/
       if ( (a1 & ENDSIDE) == ENDSIDE )
         for (i = *n + 1;i <= (k + *n)/2; ++i) a[ i ] |= ENDSIDE;

       if ( (a2 & ENDSIDE) == ENDSIDE )
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
