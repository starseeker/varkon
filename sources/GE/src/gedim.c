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
/*  URL:  http://varkon.sourceforge.net                             */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************/

        DBstatus  GE821(
        DBArc    *parc,
        DBSeg    *seg,
        DBVector *pos,
        short     type,
        DBTmat   *dimsys,
        DBCdim   *cdmpek)

/*      Uses pos and DBArc data to calculate the three
 *      LOCAL positions that define a CDIM.
 *
 *      Varkon pre SVN#28 saved CDIM DB-geometry in BASIC.
 *      From SVN#28 CDIM data is saved in LOCAL coordinates.
 *
 *      In: parc   = C ptr to arc
 *          seg    = C ptr to segments
 *          pos    = C ptr to LOCAL text position
 *          type   = Horizontal, vertical or parallell
 *          dimsys = C ptr to CDIM coordinate system
 *
 *      Out: *cdmpek = DBCdim with positions calculated
 *
 *      (C)microform ab 19/8/85 J. Kjellander
 *
 *      1999-05-27 Rewritten, J.Kjellander
 *      2007-09-17 3D arcs, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat  dx,dy,fi;
   DBVector arc_centre;

/*
***Calculate CDIM data (3 pos). Note that DBArc
***centre pos x_a and y_a are BASIC coordinates
***and can not be used in 3D since Z is missing.
*/
   if ( parc->ns_a == 0 )
     {
     arc_centre.x_gm = parc->x_a;
     arc_centre.y_gm = parc->y_a;
     arc_centre.z_gm = 0.0;
     }
/*
***3D arc. Calculate arc centre. GEcentre() returns
***coordinates in BASIC. Transform to LOCAL and project
***to XY-plane of dimsys.
*/
   else
     {
     GEcentre((DBAny *)parc,seg,0.0,&arc_centre);
     if ( dimsys != NULL ) GEtfpos_to_local(&arc_centre,dimsys,&arc_centre);
     arc_centre.z_gm = 0.0;
     }
/*
***Orientation.
*/
   switch ( type )
     {
/*
***Horizontal.
*/
     case 0:
     cdmpek->p1_cd.x_gm = arc_centre.x_gm + parc->r_a;
     cdmpek->p1_cd.y_gm = arc_centre.y_gm;
     cdmpek->p1_cd.z_gm = 0.0;
     cdmpek->p2_cd.x_gm = arc_centre.x_gm - parc->r_a;
     cdmpek->p2_cd.y_gm = arc_centre.y_gm;
     cdmpek->p2_cd.z_gm = 0.0;
     break;
/*
***Vertical.
*/
     case 1:
     cdmpek->p1_cd.x_gm = arc_centre.x_gm;
     cdmpek->p1_cd.y_gm = arc_centre.y_gm + parc->r_a;
     cdmpek->p1_cd.z_gm = 0.0;
     cdmpek->p2_cd.x_gm = arc_centre.x_gm;
     cdmpek->p2_cd.y_gm = arc_centre.y_gm - parc->r_a;
     cdmpek->p2_cd.z_gm = 0.0;
     break;
/*
***Parallell.
*/
     case 2:
     dx = pos->x_gm - arc_centre.x_gm;
     dy = pos->y_gm - arc_centre.y_gm;
     if       ( dx == 0.0 && dy > 0.0 ) fi = PI05;
     else if ( dx == 0.0 && dy < 0.0 ) fi = PI15;
     else
       {
       fi = ATAN(dy/dx);
       if ( dx < 0.0 ) fi = fi + PI;
       if ( fi < 0.0 ) fi = fi + PI2;
       }
     fi = fi - PI05;
     cdmpek->p1_cd.x_gm = arc_centre.x_gm + parc->r_a*COS(fi);
     cdmpek->p1_cd.y_gm = arc_centre.y_gm + parc->r_a*SIN(fi);
     cdmpek->p1_cd.z_gm = 0.0;
     cdmpek->p2_cd.x_gm = arc_centre.x_gm - parc->r_a*COS(fi);
     cdmpek->p2_cd.y_gm = arc_centre.y_gm - parc->r_a*SIN(fi);
     cdmpek->p2_cd.z_gm = 0.0;
     break;
     }
/*
***Third position is always = pos.
*/
   cdmpek->p3_cd.x_gm = pos->x_gm;
   cdmpek->p3_cd.y_gm = pos->y_gm;
   cdmpek->p3_cd.z_gm = 0.0;
/*
***Add type.
*/
   cdmpek->dtyp_cd = type;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE822(
        DBArc    *parc,
        DBSeg    *seg,
        DBVector *p1,
        DBVector *p2,
        DBTmat   *dimsys,
        DBRdim   *rdmpek)

/*      Uses p1, p2 and DBArc data to calculate the three
 *      LOCAL positions that define a RDIM.
 *
 *      In: parc   = C ptr to arc.
 *          seg    = C ptr to segments.
 *          p1     = C ptr to break position.
 *          p2     = C ptr to end position.
 *          dimsys = C ptr to RDIM coordinate system.
 *
 *      Out: *rdmpek = DBRdim with positions calculated.
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      28/3/89    Bug P1=Cirkelcentrum, J, Kjellander
 *      1999-05-27 Rewritten, J.Kjellander
 *      2007-09-22 3D, J.Kjellander
 *
 ******************************************************!*/

  {
   DBVector p0,arc_centre;;
   DBLine   lin;
   DBfloat  dx,dy,dist;
   DBfloat  u1[2],u2[2],u;
   short    noint;

/*
***Calculate arc centre.
*/
   if ( parc->ns_a == 0 )
     {
     arc_centre.x_gm = parc->x_a;
     arc_centre.y_gm = parc->y_a;
     arc_centre.z_gm = 0.0;
     }
/*
***3D arc. Calculate arc centre. GEcentre() returns
***coordinates in BASIC. Transform to LOCAL and project
***to XY-plane of dimsys.
*/
   else
     {
     GEcentre((DBAny *)parc,seg,0.0,&arc_centre);
     if ( dimsys != NULL ) GEtfpos_to_local(&arc_centre,dimsys,&arc_centre);
     arc_centre.z_gm = 0.0;
     }
/*
***Calculate 2D distance between P1 and arc_centre.
*/
   dx = p1->x_gm - arc_centre.x_gm;
   dy = p1->y_gm - arc_centre.y_gm;

   dist = SQRT(dx*dx + dy*dy);
/*
***If dist > 0, create a line through arc centre 
***in the direction of P1 and calculate intersection.
*/
   if ( dist > 1e-10 )
     {
     lin.hed_l.type = LINTYP;
     lin.crd1_l.x_gm = arc_centre.x_gm + 2.0*(parc->r_a/dist)*dx;
     lin.crd1_l.y_gm = arc_centre.y_gm + 2.0*(parc->r_a/dist)*dy;
     lin.crd1_l.z_gm = 0.0;
     lin.crd2_l.x_gm = arc_centre.x_gm - 2.0*(parc->r_a/dist)*dx;
     lin.crd2_l.y_gm = arc_centre.y_gm - 2.0*(parc->r_a/dist)*dy;
     lin.crd2_l.z_gm = 0.0;
/*
     GE706((DBAny *)&lin,(DBAny *)parc,&noint,u1,u2);
*/
     GE710((DBAny *)&lin,NULL,(DBAny *)parc,seg,dimsys,&noint,u1,u2);
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
***Calculate the coordinates of P0.
*/
   GEposition((DBAny *)parc,(char *)seg,u,(DBfloat)0.0,&p0);
   if ( dimsys != NULL ) GEtfpos_to_local(&p0,dimsys,&p0);
/*
***Store positions in RDIM.
*/
   rdmpek->p1_rd.x_gm = p0.x_gm;
   rdmpek->p1_rd.y_gm = p0.y_gm;
   rdmpek->p1_rd.z_gm = 0.0;

   rdmpek->p2_rd.x_gm = p1->x_gm;
   rdmpek->p2_rd.y_gm = p1->y_gm;
   rdmpek->p2_rd.z_gm = 0.0;

   rdmpek->p3_rd.x_gm = p2->x_gm;
   rdmpek->p3_rd.y_gm = p2->y_gm;
   rdmpek->p3_rd.z_gm = 0.0;

   rdmpek->r_rd = parc->r_a;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GE823(
        DBLine   *l1,
        DBLine   *l2,
        DBVector *pos,
        short     alt,
        DBTmat   *dimsys,
        DBAdim   *admpek)

/*      Uses 2 lines, a text position and an
 *      alternative to calculate DBAdim data.
 *
 *      In: l1     = C ptr to first line.
 *          l2     = C ptr to second line.
 *          pos    = C ptr to text position.
 *          alt    = Alternative.
 *
 *      Out: *admpek = DBAdim data calculated.
 *
 *      Error: GE8132 = Illegal alt value.
 *             GE8122 = Lines parallell.
 *
 *      (C)microform ab  4/9/85 J. Kjellander
 *
 *       10/9/85    Nya felnummer, R. Svedin
 *       23/10/86   Bug, J. Kjellander
 *       1999-05-27 Rewritten, J.Kjellander
 *       2007-09-24 3D, J.Kjellander
 *
 ******************************************************!*/

  {
   DBfloat dx1,dy1,dx2,dy2;
   DBfloat dx,dy,fi1,fi2;
   DBfloat q,t;
   short   rikt1;

/*
***Line delta values.
*/
   if ( dimsys != NULL )
     {
     GEtfpos_to_local(&l1->crd1_l,dimsys,&l1->crd1_l);
     GEtfpos_to_local(&l1->crd2_l,dimsys,&l1->crd2_l);
     GEtfpos_to_local(&l2->crd1_l,dimsys,&l2->crd1_l);
     GEtfpos_to_local(&l2->crd2_l,dimsys,&l2->crd2_l);
     }

   dx1 = l1->crd2_l.x_gm - l1->crd1_l.x_gm;
   dy1 = l1->crd2_l.y_gm - l1->crd1_l.y_gm;

   dx2 = l2->crd2_l.x_gm - l2->crd1_l.x_gm;
   dy2 = l2->crd2_l.y_gm - l2->crd1_l.y_gm;
/*
***Calculate line intersect (local coordinates).
*/
   q = dy1*dx2 - dx1*dy2;
   if ( ABS(q) < TOL1 ) return(erpush("GE8122",""));

   t = ((l1->crd1_l.x_gm - l2->crd1_l.x_gm) * dy2 -
        (l1->crd1_l.y_gm - l2->crd1_l.y_gm) * dx2) / q;

   admpek->pos_ad.x_gm = l1->crd1_l.x_gm + t*dx1;
   admpek->pos_ad.y_gm = l1->crd1_l.y_gm + t*dy1;
   admpek->pos_ad.z_gm = 0.0;
/*
***Calculate radius.
*/
   dx = pos->x_gm - admpek->pos_ad.x_gm;
   dy = pos->y_gm - admpek->pos_ad.y_gm;
   admpek->r_ad = SQRT(dx*dx + dy*dy);
/*
***Calculate text angle.
*/
   if       ( dx == 0.0 && dy > 0.0 ) admpek->tv_ad = 90.0;
   else if ( dx == 0.0 && dy < 0.0 ) admpek->tv_ad = 270.0;
   else
     {
     admpek->tv_ad = ATAN(dy/dx) * RDTODG;
     if ( dx < 0.0 ) admpek->tv_ad += 180.0;
     }
/*
***Calculate line angles to X-axis between 0 and 180.
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
***Determine 1:st line direction through X-axis.
*/
   rikt1 = 1;

   if       ( dy1 == 0.0 && dx1 < 0 ) rikt1 = -1;
   else if ( dy1 < 0.0 ) rikt1 = -1;
/*
***Calculate ADIM start- and end angle.
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
***Normalize angles.
*/
   if ( fi2 > 360.0 )
     {
     fi1 -= 360;
     fi2 -= 360;
     }

   admpek->v1_ad = fi1;
   admpek->v2_ad = fi2;
/*
***Set r1 and r2 to zero. (Not used ?)
*/
   admpek->r1_ad = 0.0;
   admpek->r2_ad = 0.0;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
