/**********************************************************************
*
*    gp26.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpcl2d();  2D classification
*    gpsvp2();  2D clip
*    gpsvp3();  3D intersect
*    gpspl1();  2D split with contact
*    gpspl2();  2D split
*    gpspl3();  3D intersect
*    gpsvp4();  Intersect plane/plane
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
#include "../../GE/include/GE.h"
#include "../include/GP.h"
#include <math.h>

/*
***Skär vektor med plan
*/
static void cutpl(GPBPL *pl, int *n, double u[]);

extern int      nsplit,edge,nsk,edsk[],plsk[];
extern double  *px1,*px2,*py1,*py2,*pz1,*pz2,
               *spx,*spy,*spz,inutol,x[],y[],z[];
extern DBVector psk[];
extern GPBPL  **ppek;

/*!******************************************************/

        short gpcl2d(
        double *xv,
        double *yv,
        GPBPL  *pl)

/*      Klassar en vektor mot ett plan, 2D.
 *
 *      In: xv,yv = Adresser till vektorns startpos.
 *          pl    = Planets adress.
 *
 *      FV: INUTI/UTANFR
 *
 *      (C)microform ab 6/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
  double tt;

  tt = (pl->dy1 * (*xv - pl->p1.x_gm) -
        pl->dx1 * (*yv - pl->p1.y_gm)) * pl->l1 + pl->k1;
  if ( tt < inutol )
    {
    tt = (pl->dy2 * (*xv - pl->p2.x_gm) -
          pl->dx2 * (*yv - pl->p2.y_gm)) * pl->l2 + pl->k2;
    if ( tt < inutol )
      {
      tt = (pl->dy3 * (*xv - pl->p3.x_gm) -
            pl->dx3 * (*yv - pl->p3.y_gm)) * pl->l3 + pl->k3;
      if ( tt < inutol )
        {
        tt = (pl->dy4 * (*xv - pl->p4.x_gm) -
             pl->dx4 * (*yv - pl->p4.y_gm)) * pl->l4 + pl->k4;
        if ( tt < inutol ) return(INUTI);
        }
      }
    }

   return(UTANFR);
 }

/********************************************************/
/*!******************************************************/

        void gpsvp2(
        GPBPL  *pl,
        int     end)

/*      Klipper aktiv vektor mot ett plan 2D.
 *      Vektorns ena ändpunkt ligger säkert inuti planet och
 *      den andra ligger säkert utanför.
 *
 *      In: pl   = Planets adress.
 *          end  = 1/2 Vilken av ändarna som ligger inuti.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double dx12,dy12,dz12,uv,up,q,upmin,upmax;

/*
***Initiering.
*/
   dx12 = *px1 - *px2; dy12 = *py1 - *py2; dz12 = *pz1 - *pz2;
   upmin = 0.0; upmax = 1.0;
/*
***Planets 1:a sida.
*/
start:
   q = dx12 * pl->dy1 - dy12 * pl->dx1;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p1.x_gm)*pl->dy1 - (*py1-pl->p1.y_gm)*pl->dx1)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p1.x_gm-*px1)*dy12 - (pl->p1.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 2:a sida.
*/
   q = dx12 * pl->dy2 - dy12 * pl->dx2;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p2.x_gm)*pl->dy2 - (*py1-pl->p2.y_gm)*pl->dx2)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p2.x_gm-*px1)*dy12 - (pl->p2.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 3:e sida.
*/
   q = dx12 * pl->dy3 - dy12 * pl->dx3;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p3.x_gm)*pl->dy3 - (*py1-pl->p3.y_gm)*pl->dx3)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p3.x_gm-*px1)*dy12 - (pl->p3.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 4:e sida.
*/
   q = dx12 * pl->dy4 - dy12 * pl->dx4;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p4.x_gm)*pl->dy4 - (*py1-pl->p4.y_gm)*pl->dx4)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p4.x_gm-*px1)*dy12 - (pl->p4.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Kommer vi hit ner har inte vektorn befunnits skära någon
***av planets sidor trots att ena änden säkert ligger innanför
***och andra änden utanför. Förklaringen måste då vara att den
***slunkit ut genom ett hörn. Justera upmin och upmax och prova igen.
*/
   upmin -=  0.001; upmax += 0.001; goto start;

 }

/********************************************************/
/*!******************************************************/

        void gpspl1(
        GPBPL  *pl,
        int     end)

/*      Splittar aktiv vektor mot ett plan 2D.
 *      Vektorns ena ände har kontakt med planet och
 *      den andra ligger säkert utanför.
 *
 *      In: pl   = Planets adress.
 *          end  = 1/2 Vilken ände som har kontakt.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   register int  i;
   int    ns;
   double u[4],dx12,dy12,dz12,umin,umax;

/*
***Beräkna skärningar.
*/
   cutpl(pl,&ns,u);
/*
***Har vi 0 skärningar är allt ok! Vektorn kan omöjligt
***skära planet.
*/
   if ( ns == 0 ) return;
/*
***Har vi 1 skärning går det att klippa vektorn.
*/
   else if ( ns == 1 )
     {
     gpsvp2(pl,end);
     return;
     }
/*
***Har vi två skärningar kan de vara identiska eller också är
***den ena kontakten med planet och den andra en sann skärning.
***Om vi har mer än 2 skärningar måste några vara identiska.
***Leta upp största och minsta.
*/
   else
     {
     umin = 2.0; umax = -1.0;
     for ( i=0; i<ns; ++i )
       {
       if ( u[i] < umin ) umin = u[i];
       if ( u[i] > umax ) umax = u[i];
       }
     u[0] = umin;
     u[1] = umax;
     }
/*
***Nu har vi exakt 2 skärningar. Om de är identiska är det
***fråga om en vektor som har kontakt med ett hörn.
*/
   if ( ABS(u[0] - u[1]) < 1e-5 ) return;
/*
***Om skärningarna inte är identiska skall vi klippa mot
***den som ligger längst bort från kontaktänden.
*/
   dx12 = *px1 - *px2;
   dy12 = *py1 - *py2;
   dz12 = *pz1 - *pz2;

   if ( end == 1 )
     {
     if ( u[0] > u[1] )
       { *px1 = *px1 - u[0]*dx12; *py1 = *py1 - u[0]*dy12;
       *pz1 = *pz1 - u[0]*dz12; }
     else
       { *px1 = *px1 - u[1]*dx12; *py1 = *py1 - u[1]*dy12;
       *pz1 = *pz1 - u[1]*dz12; }
     }
   else
     {
     if ( u[0] < u[1] )
       { *px2 = *px1 - u[0]*dx12; *py2 = *py1 - u[0]*dy12;
       *pz2 = *pz1 - u[0]*dz12; }
     else
       { *px2 = *px1 - u[1]*dx12; *py2 = *py1 - u[1]*dy12;
       *pz2 = *pz1 - u[1]*dz12; }
     }

 }

/********************************************************/
/*!******************************************************/

        short gpspl2(
        GPBPL  *pl)

/*      Splittar aktiv vektor mot ett plan 2D.
 *      Vektorns båda ändpunkter ligger säkert
 *      utanför planet.
 *
 *      In: pl   = Planets adress.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   register int i;
   int    ns;
   double u[4],dx12,dy12,dz12,umin,umax;

/*
***Beräkna skärningar.
*/
   cutpl(pl,&ns,u);
/*
***Har vi nu 0 skärningar är allt ok! Vektorn kan omöjligt
***skära planet. Har vi 1 skärning är det lite skumt. Troligen
***passerar vektorn förbi planet på dess utsida väldigt nära
***ett hörn.
*/
   if ( ns < 2 ) return(SYNLIG);
/*
***Om ns = 2 eller mera finns ett största u och ett minsta.
***Om vi har mer än 2 skärningar måste några vara identiska.
***Leta upp största och minsta.
*/
   else
     {
     umin = 2.0; umax = -1.0;
     for ( i=0; i<ns; ++i )
       {
       if ( u[i] < umin ) umin = u[i];
       if ( u[i] > umax ) umax = u[i];
       }
     u[0] = umin;
     u[1] = umax;
     }
/*
***Nu har vi exakt 2 skärningar. Om de är identiska är det
***fråga om en vektor som passerar planet på utsidan nära ett
***hörn.
*/
   if ( ABS(u[0] - u[1]) < 1e-5 ) return(SYNLIG);
/*
***Om skärningarna inte är identiska är det dags för split.
***Börja med att skapa en split-del.
*/
   if ( nsplit < SPLMAX ) 
     {
     dx12 = *px1 - *px2;
     dy12 = *py1 - *py2;
     dz12 = *pz1 - *pz2;
     *(spx+2*nsplit) = *px1 - u[1]*dx12;
     *(spy+2*nsplit) = *py1 - u[1]*dy12;
     *(spz+2*nsplit) = *pz1 - u[1]*dz12;
     *(spx+2*nsplit+1) = *px2;
     *(spy+2*nsplit+1) = *py2;
     *(spz+2*nsplit+1) = *pz2;
/*
***Beräkna sedan ny slutpunkt för den ursprungliga
***nu synliga vektorn.
*/
     *px2 = *px1 - u[0]*dx12;
     *py2 = *py1 - u[0]*dy12;
     *pz2 = *pz1 - u[0]*dz12;
     return(SPLIT2);
     }
/*
***Om nsplit = SPLMAX....
*/
   return(OSYNLIG);

 }

/********************************************************/
/*!******************************************************/

        static void cutpl(
        GPBPL  *pl,
        int    *n,
        double  u[])

/*      Skär aktiv vektor mot ett tätt plan.
 *
 *      In: pl = Planets adress.
 *
 *      Ut: n  = Antal skärningar 0 - 4.
 *          u  = Skärningarnas u-värden.
 *
 *      FV: void
 *
 *      (C)microform ab 15/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double dx12,dy12,uv,up,q,upmin,upmax;
   int    ns;

/*
***Initiering. Tätt plan.
*/
   dx12 = *px1 - *px2; dy12 = *py1 - *py2;
   upmin = -0.001; upmax = 1.001; ns = 0;
/*
***Planets 1:a sida.
*/
   q = dx12 * pl->dy1 - dy12 * pl->dx1;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p1.x_gm)*pl->dy1 - (*py1-pl->p1.y_gm)*pl->dx1)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p1.x_gm-*px1)*dy12 - (pl->p1.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 2:a sida.
*/
   q = dx12 * pl->dy2 - dy12 * pl->dx2;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p2.x_gm)*pl->dy2 - (*py1-pl->p2.y_gm)*pl->dx2)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p2.x_gm-*px1)*dy12 - (pl->p2.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 3:e sida.
*/
   q = dx12 * pl->dy3 - dy12 * pl->dx3;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p3.x_gm)*pl->dy3 - (*py1-pl->p3.y_gm)*pl->dx3)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p3.x_gm-*px1)*dy12 - (pl->p3.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 4:e sida.
*/
   q = dx12 * pl->dy4 - dy12 * pl->dx4;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p4.x_gm)*pl->dy4 - (*py1-pl->p4.y_gm)*pl->dx4)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p4.x_gm-*px1)*dy12 - (pl->p4.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Antal skärningar.
*/
   *n = ns;

 }

/********************************************************/
/*!******************************************************/

        void gpsvp3(
        GPBPL  *pl,
        DBVector  *ps)

/*      Beräknar skärning mellan aktiv vektor och ett
 *      oändligt plan 3D.
 *
 *      In: pl   = Planets adress.
 *
 *      Ut: ps   = Skärningspunkt.
 *
 *      FV: Void.
 *
 *      (C)microform ab 17/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
#define NOLLTOL 1e-12

/*
***Denna tolerens används för att testa hurvida ett plan
***är parallellt med något av de tre huvudplanen. Den måste
***vara så liten så att en vektor vars ena ändpunkt ligger
***i planet ( tex. en splitdel ) klassas säkert som liggande
***helt på ena eller andra sidan.
*/

   double t,l,n1x,n1y,n1z,n2x,n2y,n2z,dx,dy,dz,k1,k2,k3,k4,
          k7,ka,kb;

/*
***Beräkna normalvektorn (n1x,n1y,n1z) längs planets första sida.
*/
   n1x = pl->dx1; n1y = pl->dy1; n1z = pl->p2.z_gm - pl->p1.z_gm;
   l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
   n1x /= l; n1y /= l; n1z /= l;
/*
***Beräkna normalvektorn n2 längs planets sista sida.
*/
   n2x = -pl->dx4; n2y = -pl->dy4; n2z = pl->p4.z_gm - pl->p1.z_gm;
   l = SQRT(n2x*n2x + n2y*n2y + n2z*n2z);
   n2x /= l; n2y /= l; n2z /= l;
/*
***Beräkna linjens normalvektor (dx,dy,dz).
*/
   dx = *px2 - *px1; dy = *py2 - *py1; dz = *pz2 - *pz1;
   l = SQRT(dx*dx + dy*dy + dz*dz);
   dx /= l; dy /= l; dz /= l;
/*
***Om n1x = 0, prova att vrida sida 1 mot sida 4.
***Om n1x fortfarande är 0 ligger planet i ett XY-plan.
***Eftersom vektorn tidigare konstaterats inte vara parallell
***med planet kan skärning enkelt beräknas.
***Om n1x efter vridning inte = 0 kan inte snabb metod användas,
***fortsätt då istället och testa n1y och n1z på samma sätt.
*/
   if ( ABS(n1x) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1x) < NOLLTOL )
       { t = (pl->p1.x_gm - *px1)/dx; goto end; }
     }
/*
***Samma för y.
*/
   if ( ABS(n1y) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1y) < NOLLTOL )
       { t = (pl->p1.y_gm - *py1)/dy; goto end; }
     }
/*
***Samma för z.
*/
   if ( ABS(n1z) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1z) < NOLLTOL )
       { t = (pl->p1.z_gm - *pz1)/dz; goto end; }
     }
/*
***Planet är inte parallellt med något av XY, ZY eller ZX-
***planen och ingen av n1:s komponenter = 0. Beräkna då
***skärning med JK:s metod.
*/
   k1 = n2x/n1x;
   k2 = -(dx/n1x);
   k3 = (*px1 - pl->p1.x_gm)/n1x;
   k4 = n2y/n1y;
   k7 = n2z/n1z;
   ka = (k4 - k1)*((*pz1 - pl->p1.z_gm)/n1z - k3) +
        (k1 - k7)*((*py1 - pl->p1.y_gm)/n1y - k3);
   kb = (k1 - k4)*(dz/n1z + k2) + (k7 - k1)*(dy/n1y + k2);
   t = ka/kb;
/*
***t har nu beräknats och punkten ps kan därmed beräknas.
*/
end:
   ps->x_gm = *px1 + t*dx;
   ps->y_gm = *py1 + t*dy;
   ps->z_gm = *pz1 + t*dz;

 }

/********************************************************/
/*!******************************************************/

        void gpspl3(
        DBVector  *ps,
        int     sida1)

/*      Delar aktiv vektor i två delar vid ps.
 *      Änden som är hitom planet lagras i (px,py,pz)
 *      och andra änden lagras som en split-del.
 *
 *      In: ps    = Delningspunkt.
 *          sida1 = HITOM/BAKOM för vektorns startpunkt.
 *
 *      FV: void.
 *
 *      (C)microform ab 17/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( sida1 == HITOM )
     {
     *(spx+2*nsplit) = ps->x_gm;
     *(spy+2*nsplit) = ps->y_gm;
     *(spz+2*nsplit) = ps->z_gm;
     *(spx+2*nsplit+1) = *px2;
     *(spy+2*nsplit+1) = *py2;
     *(spz+2*nsplit+1) = *pz2;
     *px2 = ps->x_gm;
     *py2 = ps->y_gm;
     *pz2 = ps->z_gm;
     }
   else
     {
     *(spx+2*nsplit) = ps->x_gm;
     *(spy+2*nsplit) = ps->y_gm;
     *(spz+2*nsplit) = ps->z_gm;
     *(spx+2*nsplit+1) = *px1;
     *(spy+2*nsplit+1) = *py1;
     *(spz+2*nsplit+1) = *pz1;
     *px1 = *px2;
     *py1 = *py2;
     *pz1 = *pz2;
     *px2 = ps->x_gm;
     *py2 = ps->y_gm;
     *pz2 = ps->z_gm;
     }
 }

/********************************************************/
/*!******************************************************/

        void gpsvp4(
        GPBPL *plr,
        GPBPL *plt,
        DBVector *ps)

/*      Beräknar skärning plan/plan.
 *
 *      FV: Void.
 *
 *      (C)microform ab 20/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double n,k1,k2,k3;
   DBVector  p2a,p2b,p3a,p3b,p3d,u3,u12,u23,u31,v1,v2;

/*
***Bilda ett tredje plan vinkelrätt mot test-planet
***och innehållande dess 1:a rand.
*/
   p2a.x_gm = plt->p1.x_gm;
   p2a.y_gm = plt->p1.y_gm;
   p2a.z_gm = plt->p1.z_gm;
   p2b.x_gm = plt->p2.x_gm;
   p2b.y_gm = plt->p2.y_gm;
   p2b.z_gm = plt->p2.z_gm;

start:
   p3a.x_gm = p2a.x_gm;
   p3a.y_gm = p2a.y_gm;
   p3a.z_gm = p2a.z_gm;

   p3b.x_gm = p2b.x_gm;
   p3b.y_gm = p2b.y_gm;
   p3b.z_gm = p2b.z_gm;

   p3d.x_gm = p2a.x_gm + plt->nv.x_gm;
   p3d.y_gm = p2a.y_gm + plt->nv.y_gm;
   p3d.z_gm = p2a.z_gm + plt->nv.z_gm;
/*
***Beräkna det tredje planets normal.
*/
   v1.x_gm = p3b.x_gm - p3a.x_gm; v1.y_gm = p3b.y_gm - p3a.y_gm;
   v1.z_gm = p3b.z_gm - p3a.z_gm;
   v2.x_gm = p3d.x_gm - p3a.x_gm; v2.y_gm = p3d.y_gm - p3a.y_gm;
   v2.z_gm = p3d.z_gm - p3a.z_gm;
   GEvector_product(&v1,&v2,&u3);
   GEnormalise_vector3D(&u3,&u3);
/*
***Beräkna de tre kryssprodukterna i skärningen.
*/
   GEvector_product(&(plt->nv),&u3,&u23);
   GEvector_product(&u3,&(plr->nv),&u31);
   GEvector_product(&(plr->nv),&(plt->nv),&u12);
/*
***Kolla om skärning finns. Om inte, prova med sidan p4-p1
***istället.
*/
   n = plr->nv.x_gm*u23.x_gm + plr->nv.y_gm*u23.y_gm + plr->nv.z_gm*u23.z_gm;
   if ( ABS(n) < 1e-10 ) 
     {
     p2a.x_gm = plt->p4.x_gm; p2a.y_gm = plt->p4.y_gm;
     p2a.z_gm = plt->p4.z_gm; p2b.x_gm = plt->p1.x_gm;
     p2b.y_gm = plt->p1.y_gm; p2b.z_gm = plt->p1.z_gm;
     goto start;
     }
/*
***Beräkna skärningspunkten plr-plt-p3.
*/
   k1 = plr->p1.x_gm*plr->nv.x_gm + plr->p1.y_gm*plr->nv.y_gm +
        plr->p1.z_gm*plr->nv.z_gm;
   k2 = p2a.x_gm*plt->nv.x_gm + p2a.y_gm*plt->nv.y_gm +
        p2a.z_gm*plt->nv.z_gm;
   k3 = p3a.x_gm*u3.x_gm + p3a.y_gm*u3.y_gm + p3a.z_gm*u3.z_gm;

   u23.x_gm *= k1; u23.y_gm *= k1; u23.z_gm *= k1;
   u31.x_gm *= k2; u31.y_gm *= k2; u31.z_gm *= k2;
   u12.x_gm *= k3; u12.y_gm *= k3; u12.z_gm *= k3;

   ps->x_gm = (u23.x_gm + u31.x_gm + u12.x_gm )/n;
   ps->y_gm = (u23.y_gm + u31.y_gm + u12.y_gm )/n;
   ps->z_gm = (u23.z_gm + u31.z_gm + u12.z_gm )/n;
 }

/********************************************************/
