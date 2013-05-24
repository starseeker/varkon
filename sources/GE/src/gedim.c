/*!******************************************************************/
/*  File: gedim.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE821() Create circular dimension                               */
/*  GE822() Create radius dimension                                 */
/*  GE823() Create angular dimension                                */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

        DBstatus GE821(
        DBArc    *parc,
        DBVector *pos,
        short     alt,
        DBCdim   *cdmpek)

/*      Beräknar med hjälp av cirkeln och pos de tre
 *      positioner som definierar måttet.
 *
 *      In: parc   = Pekare till cirkel
 *          pos    = Pekare till måttextens läge
 *          alt    = Horisontellt, vertikalt eller parall.
 *          cdmpek = Pekare till diametermått-struktur
 *
 *      Out: *cdmpek = Fyller i de tre positionerna
 *
 *      (C)microform ab 19/8/85 J. Kjellander
 *
 *       1999-05-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dx,dy,fi;

/*
***Testa alt och beräkna positionerna därefter.
*/
   switch ( alt )
     {
/*
***Horisontellt.
*/
     case 0:
     cdmpek->p1_cd.x_gm = parc->x_a + parc->r_a;
     cdmpek->p1_cd.y_gm = parc->y_a;
     cdmpek->p2_cd.x_gm = parc->x_a - parc->r_a;
     cdmpek->p2_cd.y_gm = parc->y_a;
     break;
/*
***Vertikalt.
*/
     case 1:
     cdmpek->p1_cd.x_gm = parc->x_a;
     cdmpek->p1_cd.y_gm = parc->y_a + parc->r_a;
     cdmpek->p2_cd.x_gm = parc->x_a;
     cdmpek->p2_cd.y_gm = parc->y_a - parc->r_a;
     break;
/*
***Parallellt.
*/
     case 2:
     dx = pos->x_gm - parc->x_a;
     dy = pos->y_gm - parc->y_a;
     if ( dx == 0 && dy > 0 ) fi = PI05;
     else if ( dx == 0 && dy < 0 ) fi = PI15;
     else
        {
        fi = ATAN(dy/dx);
        if ( dx < 0 ) fi = fi + PI;
        if ( fi < 0.0 ) fi = fi + PI2;
        }
     fi = fi - PI05;
     cdmpek->p1_cd.x_gm = parc->x_a + parc->r_a*COS(fi);
     cdmpek->p1_cd.y_gm = parc->y_a + parc->r_a*SIN(fi);
     cdmpek->p2_cd.x_gm = parc->x_a - parc->r_a*COS(fi);
     cdmpek->p2_cd.y_gm = parc->y_a - parc->r_a*SIN(fi);
     break;
     }
/*
***Oavsett typ är alltid 3:e positionen = pos.
*/
   cdmpek->p3_cd.x_gm = pos->x_gm;
   cdmpek->p3_cd.y_gm = pos->y_gm;
   cdmpek->dtyp_cd = alt;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE822(
        DBArc    *parc,
        DBVector *p1,
        DBVector *p2,
        DBRdim   *rdmpek)

/*      Beräknar med hjälp av cirkeln och p1, p2 de tre
 *      positioner som definierar måttet.
 *
 *      In: parc   = Pekare till cirkel
 *          p1     = Pekare till brytposition
 *          p2     = Pekare till slutposition
 *          cdmpek = Pekare till diametermått-struktur
 *
 *      Out: *rdmpek = Fyller i de tre positionerna
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      28/3/89    Bug P1=Cirkelcentrum, J, Kjellander
 *      1999-05-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBVector p0;
   DBLine   lin;
   DBfloat  dx,dy,dist;
   DBfloat  u1[2],u2[2],u;
   short    noint;

/*
***Låtsas att det är en 2D-cirkel.
*/
   parc->ns_a = 0;
/*
***Beräkna längden cirkelcentrum - P1.
*/
   dx = p1->x_gm - parc->x_a;
   dy = p1->y_gm - parc->y_a;

   dist = SQRT(dx*dx + dy*dy);
/*
***Om > 0, skapa lagom lång linje igenom cirkelcentrum riktad
***mot P1 och beräkna skärningen mellan linjen och cirkeln.
*/
   if ( dist > 1e-10 )
     {
     lin.hed_l.type = LINTYP;
     lin.crd1_l.x_gm = parc->x_a + 2.0 * (parc->r_a/dist) * dx;
     lin.crd1_l.y_gm = parc->y_a + 2.0 * (parc->r_a/dist) * dy;
     lin.crd2_l.x_gm = parc->x_a - 2.0 * (parc->r_a/dist) * dx;
     lin.crd2_l.y_gm = parc->y_a - 2.0 * (parc->r_a/dist) * dy;
     GE706((DBAny *)&lin,(DBAny *)parc,&noint,u1,u2);
     }
/*
***Om inte, dvs. P1 = Cirkelcentrum, välj u=0.5.
*/
   else noint = 0;
/*
***Välj fall beroende på antal skärningar.
*/
   switch ( noint )
     {
/*
***Ingen skärning. Välj P0 = bågens mittpunkt.
*/
     case 0:
     u = 0.5;
     break;
/*
***En skärning. Välj P0 = skärningspunkten.
*/
     case 1:
     u = u2[0] - 1.0;
     break;
/*
***Två skärningar. Välj P0 = den närmast P1, dvs nr. 2.
*/
     case 2:
     u = u2[0] - 1.0;
     break;
     }
/*
***Beräkna P0:s koordinater.
*/
   GEposition((DBAny *)parc,NULL,u,(gmflt)0.0,&p0);
/*
***Lagra resultatet i mått-posten.
*/
   rdmpek->p1_rd.x_gm = p0.x_gm;
   rdmpek->p1_rd.y_gm = p0.y_gm;

   rdmpek->p2_rd.x_gm = p1->x_gm;
   rdmpek->p2_rd.y_gm = p1->y_gm;

   rdmpek->p3_rd.x_gm = p2->x_gm;
   rdmpek->p3_rd.y_gm = p2->y_gm;

   rdmpek->r_rd = parc->r_a;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE823(
        DBLine   *l1,
        DBLine   *l2,
        DBVector *pos,
        short     alt,
        DBAdim   *admpek)

/*      Geo-rutin för ADIM.
 *
 *      In: l1     = Pekare till 1:a linjen.
 *          l2     = Pekare till 2:a linjen.
 *          pos    = Pekare till textens position.
 *          alt    = Alternativ.
 *          admpek = Pekare till vinkelmått-post.
 *
 *      Out: *admpek = Fyller i mått-posten.
 *
 *      (C)microform ab  4/9/85 J. Kjellander
 *
 *       10/9/85    Nya felnummer, R. Svedin
 *       23/10/86   Bug, J. Kjellander
 *       1999-05-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dx1,dy1,dx2,dy2;
   DBfloat dx,dy,fi1,fi2;
   DBfloat q,t;
   short   rikt1;

/*
***Beräkna delta-värden.
*/
   dx1 = l1->crd2_l.x_gm - l1->crd1_l.x_gm;
   dy1 = l1->crd2_l.y_gm - l1->crd1_l.y_gm;

   dx2 = l2->crd2_l.x_gm - l2->crd1_l.x_gm;
   dy2 = l2->crd2_l.y_gm - l2->crd1_l.y_gm;
/*
***Beräkna skärningspunkt.
*/
   q = dy1*dx2 - dx1*dy2;
   if ( ABS(q) < TOL1 ) return(erpush("GE8122",""));

   t = ((l1->crd1_l.x_gm - l2->crd1_l.x_gm) * dy2 -
        (l1->crd1_l.y_gm - l2->crd1_l.y_gm) * dx2) / q;

   admpek->pos_ad.x_gm = l1->crd1_l.x_gm + t*dx1;
   admpek->pos_ad.y_gm = l1->crd1_l.y_gm + t*dy1;
   admpek->pos_ad.z_gm = 0.0;
/*
***Beräkna radie.
*/
   dx = pos->x_gm - admpek->pos_ad.x_gm;
   dy = pos->y_gm - admpek->pos_ad.y_gm;
   admpek->r_ad = SQRT(dx*dx + dy*dy);
/*
***Beräkna vinkel för placering av text.
*/
   if ( dx == 0.0 && dy > 0.0 ) admpek->tv_ad = 90.0;
   else if ( dx == 0.0 && dy < 0.0 ) admpek->tv_ad = 270.0;
   else
     {
     admpek->tv_ad = ATAN(dy/dx) * RDTODG;
     if ( dx < 0.0 ) admpek->tv_ad += 180.0;
     }
/*
***Beräkna linjernas vinkel mot X-axeln. Se till
***att vinkeln ligger mellan 0 och 180 grader.
*/
   if ( dx1 == 0.0 )
     {
     fi1 = 90.0;
     }
   else
     {
     fi1 = ATAN(dy1/dx1) * RDTODG;
     if ( fi1 < 0.0 ) fi1 += 180.0;
     }

   if ( dx2 == 0.0 )
     {
     fi2 = 90.0;
     }
   else
     {
     fi2 = ATAN(dy2/dx2) * RDTODG;
     if ( fi2 < 0.0 ) fi2 += 180.0;
     }
/*
***Beräkna 1:a linjens riktning genom X-axeln.
*/
   rikt1 = 1;
   if ( dy1 == 0.0 && dx1 < 0 ) rikt1 = -1;
   else if ( dy1 < 0.0 ) rikt1 = -1;
/*
***Bestäm måttets start och slutvinkel.
*/
   switch ( alt )
     {
     case 1:
     if ( rikt1 > 0 )
       {
       fi1 += 180;
       fi2 += 180;
       }
     if ( fi2 < fi1 ) fi2 += 180;
     break;

     case 2:
     if ( rikt1 > 0 )
       {
       fi1 += 180;
       fi2 += 180;
       }
     if ( fi2 < fi1 ) fi2 += 180;
     fi2 += 180;
     break;

     case -1:
     if ( rikt1 < 0 )
       {
       fi1 += 180;
       fi2 += 180;
       }
     if ( fi2 < fi1 ) fi2 += 180;
     break;

     case -2:
     if ( rikt1 < 0 )
       {
       fi1 += 180;
       fi2 += 180;
       }
     if ( fi2 < fi1 ) fi2 += 180;
     fi2 += 180;
     break;

     default:
     return(erpush("GE8132",""));
     break;
     }
/*
***Gör vinklarna säkert mindre än 360 grader.
*/
   if ( fi2 > 360.0 )
     {
     fi1 -= 360;
     fi2 -= 360;
     }

   admpek->v1_ad = fi1;
   admpek->v2_ad = fi2;
/*
***Lagra r1 och r2.
*/
   admpek->r1_ad = 0.0;
   admpek->r2_ad = 0.0;

   return(0);
  }

/********************************************************/
