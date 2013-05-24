/**********************************************************************
*
*    gp11.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    klpply();    Clip polyline
*    klplin();    Clip line
*    klpdot();    Clip point
*
*    klp();       Clip vector
*    klptst();    Clip vector with additional output
*    klpplq();    Clip test polyline quickly
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

/*!******************************************************/

   bool klpply(
        int    kmin,
        int   *kmax,
        double x[],
        double y[],
        char a[])

/* 
*        Klipper en polylinje
*
*       IN: (Oklippt)
*          kmin+1:      Offset till polylinjestart.
*          kmax:        Offset till polylinjeslut.
*          xÄ kmin+1 Å,yÄ kmin+1 Å,aÄ kmin+1 Å: Startpunkt med status
*          ....
*          xÄ kmax Å,yÄ kmax Å,aÄ kmax Å:       Slutpunkt med status
* 
*       UT: (Klippt)
*          kmax:        Offset till polylinjeslut.
*          a: Status (Bit(ENDSIDE) modifierad) om mer än 50%
*             av polylinjen klippts bort.
* 
*       FV:
*          klpply: TRUE = Någon del av polylinjen är synlig.
*
*
*       (C)microform ab 15/7-85 Ulf Johansson
*
*        6/4/89  Avlusad, J. Kjellander
*
********************************************************!*/

{
   int    i,j,l;
   double vec[4],sav0,sav1;

/*
***Initiering.
*/
   i = j = kmin + 1;
/*
***Leta upp 1:a synliga vektor. Om ingen synlig, returnera FALSE.
*/
   do
     { 
     if ( i >= *kmax )
       {
       *kmax = kmin;
       return(FALSE);
       } 
     vec[0] = x[i];
     vec[1] = y[i];
     vec[2] = sav0 = x[++i];
     vec[3] = sav1 = y[i];
     } while ( klp(vec,actvy.vywin) == -1 );
/*
***I vec ligger nu en vektor som efter klippning syns, helt
***eller delvis! Börja med en släckt förflyttning till
***startpunkten och en normal förflyttning till nästa punkt.
*/
   x[j] = vec[0];
   y[j] = vec[1];
   a[j] &= a[i-1] & ~VISIBLE; /* Varför & framför = ???? */

   x[++j] = vec[2];
   y[j] = vec[3];
   a[j] = a[i];
/*
***Loopa igenom resten av polylinjen.
*/
   while ( ++i <= *kmax )
     {
     vec[0] = sav0;
     vec[1] = sav1;
     vec[2] = sav0 = x[i];
     vec[3] = sav1 = y[i];
/*
***Klipp aktuell vektor. 0 => Vektorn helt innanför.
***                      2 => Ände 2 utanför.
*/
     switch ( klp(vec,actvy.vywin) )
       {
       case 0:
       case 2:
       x[++j] = vec[2];
       y[j] = vec[3];
       a[j] = a[i];            /* Detta är fel !!! */
       break;
/*
*** 1 => Ände 1 utanför.
*** 3 => Båda ändarna utanför. Vektorn klippt på två ställen.
*/
       case 1: 
       case 3:
       if ( ++j == i )
         { /* En koordinat blir två */    
         if ( ++i >= PLYMXV )
           { /* Om det finns plats, */
           *kmax = j;
           return(TRUE);
           }
         for ( l = ++(*kmax); l >= j; l-- )
           { /* fixa utrymme */
           x[l] = x[l-1];
           y[l] = y[l-1];
           a[l] = a[l-1];
           }
         }
       x[j] = vec[0];
       y[j] = vec[1];
       a[j] = a[i-1] & ~VISIBLE;    /* Osynlig */
       x[++j] = vec[2];
       y[j] = vec[3];
       a[j] = a[i];
       break;
       }
     }
/*
***Uppdatera polylinjeslut.
*/
     *kmax = j;

     return(TRUE);
}

/********************************************************/
/*!******************************************************/

   bool klplin(
        int    k,
        double x[],
        double y[],
        char   a[])


/* 
*       Klipper en linje.
*
*       IN: (Oklippt)
*          k+1:     Offset till linjestart
*          k+2:     Offset till linjeslut
*          xÄ k+1 Å,yÄ k+1 Å,aÄ k+1 Å: Startpunkt med status
*          xÄ k+2 Å,yÄ k+2 Å,aÄ k+2 Å: Slutpunkt med status
*
*       UT: (Klippt)
*          a: Status (Bit(ENDSIDE) modifierad) om mer än 50%
*             av linjen klippts bort.
*
*       FV:
*          klplin: TRUE = Någon del av linjen är synlig.
*
*
*       (c) Microform AB 15/7-85 Ulf Johansson
*
*******************************************************!*/
{
     int    i,j,l;
     double vec[ 4 ];
     double t1,t2;

     i = j = k + 1;

     vec[ 0 ] = x[ i ];                 /* P1 */
     vec[ 1 ] = y[ i ];
     vec[ 2 ] = x[ ++j ];               /* P2 */
     vec[ 3 ] = y[ j ];

     l = klptst(vec,actvy.vywin,&t1,&t2);

     if (l != 0)
          if (l > 0)
               if (l == 1) {
                                                 /* P1 klippt */
                    x[ i ] = vec[ 0 ];
                    y[ i ] = vec[ 1 ];
                    if (t1 > 0.5)
                         if ((a[ j ] & ENDSIDE) == ENDSIDE)
                              a[ i ] |= ENDSIDE;  /* Enable */
                         else
                              a[ i ] &= ~ENDSIDE; /* Disable */

               } else if (l == 2) {
                                                 /* P2 klippt */
                    x[ j ] = vec[ 2 ];
                    y[ j ] = vec[ 3 ];
                    if (t2 > 0.5)
                         if ((a[ i ] & ENDSIDE) == ENDSIDE)
                              a[ j ] |= ENDSIDE;  /* Enable */
                         else
                              a[ j ] &= ~ENDSIDE; /* Disable */

               } else {
                                                 /* Klippt i båda ändar */
                    x[ i ] = vec[ 0 ];
                    y[ i ] = vec[ 1 ];
                    x[ j ] = vec[ 2 ];
                    y[ j ] = vec[ 3 ];
                    if (t1 > 0.5)
                         if ((a[ j ] & ENDSIDE) == ENDSIDE)
                              a[ i ] |= ENDSIDE;  /* Enable */
                         else
                              a[ i ] &= ~ENDSIDE; /* Disable */
                    else if (t2 > 0.5)
                         if ((a[ i ] & ENDSIDE) == ENDSIDE)
                              a[ j ] |= ENDSIDE;  /* Enable */
                         else
                              a[ j ] &= ~ENDSIDE; /* Disable */

               }               
          else
               return(FALSE);

     return(TRUE);
}
/********************************************************/
/*!******************************************************/

   bool klpdot(
        int    k,
        double x[],
        double y[])


/* 
*       Klipper en punkt
*
*       IN: (Oklippt)
*          k+1:     Offset till punkt
*          xÄ k+1 Å,yÄ k+1 Å,aÄ k+1 Å: Punkt med status
*
*       UT: (Klippt)
*
*       FV:
*          klpdot: TRUE = Punkten ligger i fönstret.
*
*
*       (c) Microform AB 15/7-85 Ulf Johansson
*
********************************************************!*/
{
     int i;

     i = k + 1;

     return ((x[ i ] > actvy.vywin[ 0 ]) &&
             (x[ i ] < actvy.vywin[ 2 ]) &&
             (y[ i ] > actvy.vywin[ 1 ]) &&
             (y[ i ] < actvy.vywin[ 3 ]));
}
/********************************************************/
/*!******************************************************/

   short klp( 
        double *v,
        double *w)

/*      Klipper en vektor mot ett fönster.
*
*       IN:
*            v: Vektor.
*            w: Fönster.
*
*       UT:
*            v: Vektor enl FV.
*         
*       FV:
*            -1: Vektorn är oklippt och ligger utanför fönstret
*             0: Vektorn är oklippt och ligger innanför fönstret.
*             1: Vektorn är klippt i punkt 1 (start).
*             2: Vektorn är klippt i punkt 2 (slut).
*             3: Vektorn är klippt i både punkt 1 och punkt 2.
*
*
*       (c) Microform AB 1984 M. Nelson
*
*
*       REVIDERAD:
*
*       15/7-85 Ulf Johansson
*
********************************************************!*/

  {
    register double *p1,*p2,*win;
    short sts1,sts2;

        sts1 = sts2 = 0;
        p1 = v;
        p2 = p1 + 2;
        win = w;

        /* Om punkt 1 ligger utanför fönstret, klipp till fönsterkanten */

        if (*p1 < *win) {

            if (*p2 < *win)
                return(-1);          /* Hela vektorn vänster om fönstret */

            *(p1+1) += (*(p2+1) - *(p1+1))*(*win - *p1)/(*p2 - *p1);
            *p1 = *win;
            sts1 = 1;
        } else if (*p1 > *(win+2)) {

            if (*p2 > *(win+2))
                return(-1);               /* Hela vektorn höger om fönstret */

            *(p1+1) += (*(p2+1) - *(p1+1))*(*(win+2) - *p1)/(*p2 - *p1);
            *p1 = *(win+2);
            sts1 = 1; 
        }

        if (*(p1+1) < *(win+1)) {

            if (*(p2+1) < *(win+1))
                return(-1);               /* Hela vektorn nedanför fönstret */

            *p1 += (*p2 - *p1)*(*(win+1) - *(p1+1))/(*(p2+1) - *(p1+1));
            *(p1+1) = *(win+1);
            sts1 = 1;
        } else if (*(p1+1) > *(win+3)) {

            if (*(p2+1) > *(win+3))
                return(-1);               /* Hela vektor ovanför fönstret */

            *p1 += (*p2 - *p1)*(*(win+3) - *(p1+1))/(*(p2+1) - *(p1+1));
            *(p1+1) = *(win+3);
            sts1 = 1;
        }

        if (sts1 == 1)                    /* Punkt 1 klippt */
            if ((*p1 < *win) ||
                (*p1 > *(win+2)) ||
                (*(p1+1) < *(win+1)) ||
                (*(p1+1) > *(win+3)))
                 return(-1);              /* Hela vektorn utanför fönstret */


       /* Punkt 1 ligger innanför fönstret, klipp punkt 2 om utanför. */ 

        if (*p2 < *win) {

            *(p2+1) += (*(p2+1) - *(p1+1))*(*win - *p2)/(*p2 - *p1);
            *p2 = *win;
            sts2 = 2;
        } else if (*p2 > *(win+2)) {

            *(p2+1) += (*(p2+1) - *(p1+1))*(*(win+2) - *p2)/(*p2 - *p1);
            *p2 = *(win+2);
            sts2 = 2;
        }

        if (*(p2+1) < *(win+1)) {

            *p2 += (*p2 - *p1)*(*(win+1) - *(p2+1))/(*(p2+1) - *(p1+1));
            *(p2+1) = *(win+1);
            sts2 = 2;
        } else if (*(p2+1) > *(win+3)) {

            *p2 += (*p2 - *p1)*(*(win+3) - *(p2+1))/(*(p2+1) - *(p1+1));
            *(p2+1) = *(win+3);
            sts2 = 2;
        }

        return(sts1 + sts2);
  }

/********************************************************/
/*!******************************************************/

   short klptst( 
        double *v,
        double *w,
        double *t1,
        double *t2)

/*      Klipper en vektor mot ett fönster samt ger ett mått på hur
*       mycket som klippts bort.
*
*       IN:
*            v: Vektor.
*            w: Fönster.
*
*
*       UT:
*            v: Vektor enl FV.
*            t1: Anger hur mycket som klippts bort vid punkt 1
*                0.0 < t1 < 1.0.
*            t2: Anger hur mycket som klippts bort vid punkt 2
*                0.0 < t2 < 1.0.
*         
*       FV:
*            -1: Vektorn är oklippt och ligger utanför fönstret
*             0: Vektorn är oklippt och ligger innanför fönstret.
*             1: Vektorn är klippt i punkt 1.
*             2: Vektorn är klippt i punkt 2.
*             3: Vektorn är klippt i både punkt 1 och punkt 2.
*
*
*       (c) Microform AB 1984 M. Nelson
*
*
*       REVIDERAD:
*
*       15/7/85  Bug, Ulf Johansson
*       29/10/86 *p1 == *p2, J. Kjellander
*
********************************************************!*/

  {
    register double *p1,*p2,*win;
    short sts1,sts2;
    double d1,d2;

        sts1 = sts2 = 0;
        p1 = v;
        p2 = p1 + 2;
        win = w;

        /* Om punkt 1 ligger utanför fönstret, klipp till fönsterkanten */

        if (*p1 < *win) {

            if (*p2 < *win)
                return(-1);          /* Hela vektorn vänster om fönstret */
            d1 = (*win - *p1)/(*p2 - *p1);
            *(p1+1) += (*(p2+1) - *(p1+1))*d1;
            *p1 = *win;
            sts1 = 1;
        } else if (*p1 > *(win+2)) {

            if (*p2 > *(win+2))
                return(-1);               /* Hela vektorn höger om fönstret */
            d1 = (*(win+2) - *p1)/(*p2 - *p1);
            *(p1+1) += (*(p2+1) - *(p1+1))*d1;
            *p1 = *(win+2);
            sts1 = 1; 
        }

        if (*(p1+1) < *(win+1)) {

            if (*(p2+1) < *(win+1))
                return(-1);               /* Hela vektorn nedanför fönstret */
            d2 = (*(win+1) - *(p1+1))/(*(p2+1) - *(p1+1));
            *p1 += (*p2 - *p1)*d2;
            *(p1+1) = *(win+1);
            sts1 += 2;
        } else if (*(p1+1) > *(win+3)) {

            if (*(p2+1) > *(win+3))
                return(-1);               /* Hela vektor ovanför fönstret */
            d2 = (*(win+3) - *(p1+1))/(*(p2+1) - *(p1+1));
            *p1 += (*p2 - *p1)*d2;
            *(p1+1) = *(win+3);
            sts1 += 2;
        }

        if (sts1 != 0) {                   /* Punkt 1 klippt */
            if ((*p1 < *win) ||
                (*p1 > *(win+2)) ||
                (*(p1+1) < *(win+1)) ||
                (*(p1+1) > *(win+3)))
                 return(-1);              /* Hela vektorn utanför fönstret */
            if ( *p1 == *p2 && *(p1+1) == *(p2+1) ) return(-1); /*861029JK */
        
            if (sts1 == 1)
                 *t1 = d1;
            else if (sts1 == 2)
                 *t1 = d2;
            else
                 *t1 = d1 + d2 - d1*d2;  /* Kompensation för tvåstegsklipp */

            sts1 = 1;
        }


       /* Punkt 1 ligger innanför fönstret, klipp punkt 2 om utanför. */ 

        if (*p2 < *win) {
            d1 = (*win - *p2)/(*p1 - *p2);
            *(p2+1) -= (*(p2+1) - *(p1+1))*d1;
            *p2 = *win;
            sts2 = 1;
        } else if (*p2 > *(win+2)) {
            d1 = (*(win+2) - *p2)/(*p1 - *p2);
            *(p2+1) -= (*(p2+1) - *(p1+1))*d1;
            *p2 = *(win+2);
            sts2 = 1;
        }

        if (*(p2+1) < *(win+1)) {
            d2 = (*(win+1) - *(p2+1))/(*(p1+1) - *(p2+1));
            *p2 -= (*p2 - *p1)*d2;
            *(p2+1) = *(win+1);
            sts2 += 2;
        } else if (*(p2+1) > *(win+3)) {
            d2 = (*(win+3) - *(p2+1))/(*(p1+1) - *(p2+1));
            *p2 -= (*p2 - *p1)*d2;
            *(p2+1) = *(win+3);
            sts2 += 2;
        }

        if (sts2 != 0) {                   /* Punkt 2 klippt */
        
            if ( *p1 == *p2 && *(p1+1) == *(p2+1) ) return(-1); /*861029JK */
            if (sts2 == 1)
                 *t2 = d1;
            else if (sts2 == 2)
                 *t2 = d2;
            else
                 *t2 = d1 + d2 - d1*d2;  /* Kompensation för tvåstegsklipp */

            sts2 = 2;

            if (sts1 != 0)
                 *t2 *= (1.0 - *t1);     /* kompensation för punkt 1-klipp */
        }

        return(sts1 + sts2);
  }

/********************************************************/
/*!******************************************************/

  short klpplq(
        int    kmin,
        int   *kmax,
        double x[],
        double y[])

/* 
*        Klipptestar en polylinje
*
*      In: kmin+1:      Offset till polylinjestart.
*          kmax:        Offset till polylinjeslut.
*          xÄ kmin+1 Å,yÄ kmin+1 Å Startpunkt.
*          ....
*          xÄ kmax Å,yÄ kmax Å     Slutpunkt.
* 
*       Ut: Inget.
* 
*       FV: -1 = Helt osynlig.
*            0 = Helt synlig.
*            1 = Delvis synlig.
*
*       (C)microform ab 2/4/91 J. Kjellander
*
********************************************************!*/

{
   int    i;
   double vec[4];

/*
***Initiering.
*/
   i = kmin + 1;
/*
***Leta upp första synliga vektor. Om alla helt osynliga,
***returnera -1 dvs. hela polylinjen osynlig.
*/
   do
     { 
     if ( i >= *kmax ) return(-1);
     vec[0] = x[i];
     vec[1] = y[i];
     vec[2] = x[++i];
     vec[3] = y[i];
     } while ( klp(vec,actvy.vywin) == -1 );
/*
***Om inte alla men minst en är helt osynlig är hela polylinjen
***delvis synlig, returnera +1.
*/
   if ( i > kmin+2 ) return(1);
/*
***Om inte ens första vektorn är helt osynlig, kolla
***istället hur många som är helt synliga.
*/
   i = kmin + 1;

   while ( i < *kmax )
     { 
     vec[0] = x[i];
     vec[1] = y[i];
     vec[2] = x[++i];
     vec[3] = y[i];
     if ( klp(vec,actvy.vywin) != 0 ) return(1);
     }
/*
***Alla vektorer helt synliga.
*/
   return(0);

  }

/********************************************************/
