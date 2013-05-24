/*!******************************************************************/
/*  File: ge304.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE304() Calculate p value for an intermediate point             */
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

/********************************************************************/

      DBstatus GE304(
      DBVector  *pa,
      DBVector  *pb,
      DBVector  *pc,
      DBVector  *pd,
      DBfloat   *ppval)   

/*    Calculate p value for an intermediate point.
 *    The intermediate point must be in the "Conic Triangle" 
 *
 *      In:  pa => Pointer to start position
 *           pb => Pointer to hardpoint
 *           pc => Pointer to end position
 *           pd => Pointer to intermediate point
 *
 *      Out: *ppval => P-value.
 *
 *      (C)microform ab 1991-11-19 G.Liden
 *
 *      1999-04-15 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat chd;       /* The length     between points a and c  */
   DBfloat dum;       /* Dummy variable                         */
   DBfloat area1;     /* Part area 1 of the "Conic Triangle"    */
   DBfloat area2;     /* Part area 2 of the "Conic Triangle"    */
   DBfloat area3;     /* Part area 3 of the "Conic Triangle"    */
   DBfloat res;       /* The triangle ratio                     */
   DBVector ad;       /* The difference between points a and d  */
   DBVector bd;       /* The difference between points b and d  */
   DBVector cd;       /* The difference between points c and d  */
   DBVector pn;       /* The plane normal                       */

/*
***Length and vectors in input "Conic Triangle"
***The length of the chord chd
*/
    dum = (pa->x_gm - pc->x_gm) * (pa->x_gm - pc->x_gm) +
          (pa->y_gm - pc->y_gm) * (pa->y_gm - pc->y_gm) +
          (pa->z_gm - pc->z_gm) * (pa->z_gm - pc->z_gm) ;

    chd = SQRT(dum);

    if ( chd < TOL1 ) return(erpush("GE3132","GE304"));
/*
***The difference vectors
*/
    ad.x_gm = pa->x_gm - pd->x_gm;
    ad.y_gm = pa->y_gm - pd->y_gm;
    ad.z_gm = pa->z_gm - pd->z_gm;

    bd.x_gm = pb->x_gm - pd->x_gm;
    bd.y_gm = pb->y_gm - pd->y_gm;
    bd.z_gm = pb->z_gm - pd->z_gm;

    cd.x_gm = pc->x_gm - pd->x_gm;
    cd.y_gm = pc->y_gm - pd->y_gm;
    cd.z_gm = pc->z_gm - pd->z_gm;
/*
***Plane normal and the part areas of the "Conic Triangle"
***Plane normal pn.
*/
    GEvector_product(&cd,&ad,&pn);
/*
***Part areas area1, area2, area3.
*/
    GEtriscl_product(&ad,&bd,&pn,&area1);
    GEtriscl_product(&bd,&cd,&pn,&area2);
    GEtriscl_product(&cd,&ad,&pn,&area3);
/*
***Check that areas area1, area2, area3 are greater than TOL1
*/
    if ( area1 < TOL1 ) return(erpush("GE3162","GE304"));
    if ( area2 < TOL1 ) return(erpush("GE3162","GE304"));
    if ( area2 < TOL1 ) return(erpush("GE3162","GE304"));
/*
***Calculate the p value
***The ratio res for the areas
*/
    res = SQRT(area1*area2/(area3*area3));
/*
***The p value to ppval
*/
   *ppval = 1.0 / (1.0 + 2.0*res);
/*
***Check the p value
*/
    if ( *ppval < 0.0 ) return(erpush("GE3172","GE304"));
    if ( *ppval > 1.0 ) return(erpush("GE3172","GE304"));

    return(0);
 }

/********************************************************************/
