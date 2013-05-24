/*!******************************************************************/
/*  File: pl6.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plpoly();    Plott polyline                                    */
/*   plcwdt();    Selects new paint pattern                         */
/*   plcnpt();    Calculates number of paint strokes                */
/*   plclip();    Clip vector                                       */
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

extern double xmin,ymin,skala,vinkel,curnog,clipw[];
extern double ptabr[],ptabp[];
extern char pfrnam[];
extern bool clip;

#define VISIBLE  1           /* Tänd förflyttning */

/*
***Globala variabler för målning och klippning.
*/

int    npaint;
double actwdt,doffs,penwdt;
double xt[PLYMXV],yt[PLYMXV];
char   at[PLYMXV];

/*!******************************************************/

        short plpoly(
        short  n,
        double x[],
        double y[],
        char   a[])

/*      Transformerar och plottar en polyline.
 *
 *      IN:
 *         n:           Offset till sista vektorn i polylinjen.
 *         x,y,a:       x-,y-koordinater och status hos vektorerna 
 *                      i polylinjen
 *
 *      (C)microform ab 31/1/91 J. Kjellander
 *
 ******************************************************!*/

 {
   short  i;
   double cosv,sinv,xs,ys,vec[4];

/*
***Skapa transformerad polylinje xt,yt.
*/
   cosv = COS(vinkel*DGTORD);
   sinv = SIN(vinkel*DGTORD);

   for ( i=0; i<=n; ++i )
     {
     xs = skala * x[i];
     ys = skala * y[i];
     xt[i] = (xs*cosv - ys*sinv) - xmin;
     yt[i] = (xs*sinv + ys*cosv) - ymin;
     }
/*
***Plotta med klippning.
*/
     if ( clip ) 
       {
/*
***Klipp alla vektorer och plotta dom individuellt.
*/
       for ( i=0; i<n; ++i )
         { 
         vec[0] = xt[i]; vec[1] = yt[i];
         vec[2] = xt[i+1]; vec[3] = yt[i+1];
         if ( plclip(vec,clipw) >= 0  &&  (a[i+1]&VISIBLE ) == VISIBLE )
           {
           plmove(vec[0],vec[1]);
           pldraw(vec[2],vec[3]);
           }
         }
       }
/*
***Plotta utan klippning.
*/
     else
       {
       for ( i=0; i<=n; ++i )
         {
         if ( (a[i] & VISIBLE) == VISIBLE ) pldraw(xt[i],yt[i]);
         else                               plmove(xt[i],yt[i]);
         }
       }

   return(0);

 }

/******************************************************/
/*!******************************************************/

        short plcwdt(int pen)

/*      Huvudrutin för pennbyte. Om breda linjer är aktivt,
 *      dvs. om pennbreddsfil angetts på kommandoraden,
 *      beräknas nytt målningsmönster, annars anropas plchpn()
 *      för normalt pennbyte.
 *
 *      In: pen = Nytt pennummer, dvs linjebredd.
 *
 *      (C)microform ab 6/2/91 J. Kjellander
 *
 *      18/2/91 5% överlapp, J. Kjellander
 *      1998-09-18 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   double width;

/*
***Om ingen pennbreddsfil getts byter vi bara penna
***som vanligt. Denna metod används då man har en
***plotter med flera pennor och vill styra vilken
***penna som skall användas via VARKON:s PEN-attribut.
***Målning kommer då inte att ske.
*/
   if ( pfrnam[0] == '\0' ) plchpn(pen);
/*
***Breda linjer är aktivt. Leta upp den begärda linjebredden
***i ritningens pennbreddstabell. Om pennummer > WPNMAX,
***prova att låta plottern ta den pennan. Isåfall sker
***heller ingen målning. Om pennummer < WPNMAX kan den
***ha ett vettigt entry i ritningens pennbreddsfil eller
***om inte så är motsvarande bredd i ptabr = 0.0 I vilket
***fall som helst sätter vi actwdt = ptabr[pen].
*/
   else
     {
     if ( pen < WPNMAX ) width = ptabr[pen];
     else
       {
       plchpn(pen);
       return(0);
       }
/*
***Om actwdt nu är större än noll skall nytt målnings-
***mönster väljas, annars tar vi den begärda pennan.
*/
     if ( width == 0.0 )
       {
       plchpn(pen);
       npaint = 0;
       }
/*
***Börja med att prova om det finns en penna som passar
***exakt till den begärda linjebredden.
*/
     else plcnpt(width);
     }

   return(0);
 }

/*!******************************************************/
/*!******************************************************/

        short plcnpt(double width)

/*      Beräknar nytt målningsmönster. Sätter actwdt,
 *      byter penna och sätter npaint.
 *
 *      In: width = Önskad linjebredd.
 *
 *      (C)microform ab 1998-09-18 J. Kjellander
 *
 ******************************************************!*/

 {
   double minwdt;
   int    i,minpen=0;

/*
***Sätt actwdt.
*/
   actwdt = width;
/*
***Beräkna npaint.
*/
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] == actwdt )
       {
       plchpn(i);
       npaint = 0;
       return(0);
       }
/*
***Så var inte fallet. Leta istället upp den smalaste pennan.
***Om denna är för bred, ta den ialla fall.
*/
   minwdt = 1E10;
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] > 0.0  &&  ptabp[i] < minwdt )
       {
       minwdt = ptabp[i];
       minpen = i;
       }
   if ( minwdt > actwdt )
     {
     plchpn(minpen);
     npaint = 0;
     return(0);
     }
/*
***Vi har nu konstaterat att minst en penna smalare än actwdt
***finns att tillgå. Börja med att prova om det finns någon
***penna som är bred nog för bara 2 målningar. Om inte prova 
***med tre osv... Ett överlapp på 5% är önskvärt för att säker-
***ställa att små hål inte uppstår tex. i cirklar.
*/
   npaint = 2;
nploop:
   minwdt = actwdt;
   for ( i=0; i<WPNMAX; ++i )
     if ( ptabp[i] >= 1.05*actwdt/npaint  &&  ptabp[i] < minwdt ) 
       {
       minwdt = ptabp[i];
       minpen = i;
       }
   if ( minwdt < actwdt )
     {
     plchpn(minpen);
     penwdt = minwdt;
     doffs = (actwdt - penwdt)/(npaint-1);
     return(0);
     }
   else
     {
     ++npaint;
     goto nploop;
     }
/*
***Slut.
*/
   return(0);
 }

/******************************************************/
/*!******************************************************/

        short plclip(
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
*       (c)microform ab  J. Kjellander efter grapac's klp().
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
