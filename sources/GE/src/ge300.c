/*!******************************************************************/
/*  File: ge300.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE()300 Create circle from origin, radius and angles            */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/

      DBstatus GE300(
      DBVector *por,
      DBfloat  rad,
      DBfloat  ang1,
      DBfloat  ang2,
      DBTmat  *pt,
      GMARC   *arcpek,
      GMSEG   *basseg,
      short    alt)

/*   Creates a 2D or 3D circle.
 *
 *   In: por  = Pointer to center position
 *       rad  = Radius
 *       ang1 = Start angle
 *       ang2 = End angle
 *       pt   = Pointer to active coordinate system
 *       alt  = 2=2D, 3=3D
 *
 *   Out: *arcpek = Arc
 *        *basseg = Arc segments
 *
 *   (C)Microform AB 1984-11-18 Thomas Schierwagen
 *
 *   1999-04-14 Rewritten, J.Kjellander
 *
 *******************************************************************!*/

 {
   short    stat=0;              /* status from called func. */
   short    no;                  /* number of segments       */
   DBfloat  ang3,ang4,ang5;      /* angles to limit segments */
   DBfloat  ang1t,ang2t;         /* transformed angles       */
   DBVector segdat[4];           /* result from GE301        */
   DBfloat  pval;                /* pvalue for segment       */
   GMSEG    locseg;              /* segment with local coeff */
   DBVector pos;                 /* projected origin position*/

/*
***Input data check.
*/
   if ( rad <= TOL1 ) return(erpush("GE3202","GE300"));
/*
***Transform angles to active coordinate system.
*/
   GEtfang_to_basic(ang1,pt,&ang1t);
   GEtfang_to_basic(ang2,pt,&ang2t);
/*
***Normalize angles.
*/
   if ( GE312(&ang1t,&ang2t) < 0 ) return(erpush("GE3372",""));
/*
***Filing of canonical data.
*/
   arcpek->x_a  = por->x_gm;
   arcpek->y_a  = por->y_gm;
   arcpek->r_a  = rad;
   arcpek->v1_a = ang1t;
   arcpek->v2_a = ang2t;
/*
***If alt == 2 create 2D-circle.
*/
   if ( alt == 2 )
     {
     arcpek->ns_a = 0;
     return(0);
     }
/*
***Alt = 3, thus create 3D circle. Transform origin to active system.
*/
   GEtfpos_to_local(por,pt,&pos);
   pos.z_gm = 0.0;
/*
***Check angles.
*/
   if ( ang1 > 360.0 + TOL1 || ang2 > 360.0 + TOL1) 
     return(erpush("GE3012","GE300" ));

   if ( ang2 - ang1 > 360.0 + TOL1) return(erpush("GE3022","GE300" ));

   if ( ang2 - ang1 <= 0.0 ) ang1 = ang1 - 360.0;
/*
***How many segments do we need ?
*/
   if      ( ang2 - ang1 <= 90.0 + 2.0 * TOL1 )  no=1;
   else if ( ang2 - ang1 <= 180.0 + 2.0 * TOL1 ) no=2;
   else if ( ang2 - ang1 <= 270.0 + 2.0 * TOL1 ) no=3;
   else if ( ang2 - ang1 <= 360.0 - 2.0 * TOL1 ) no=4;
   else if ( ang2 - ang1 > 360.0 - 2.0 * TOL1 
          && ang2 - ang1 < 360.0 + 2.0 * TOL1)   no=5;
   else return(erpush("GE3022","GE300" ));

   arcpek->ns_a = no;
/*
***One segment.
*/
   switch ( no )
     {
     case 1:
     stat = GE301(&pos,rad,ang1,ang2,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,basseg);
     break;
/*
***Two segments.
*/
     case 2:
     ang3 = ang1 + 90.0;
     stat = GE301(&pos,rad,ang1,ang3,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,basseg);
  
     stat = GE301(&pos,rad,ang3,ang2,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[1]);
     break;
/*
***Three segments.
*/
     case 3:
     ang3 = ang1 + 90.0;
     stat = GE301(&pos,rad,ang1,ang3,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,basseg);

     ang4 = ang1 + 180.0;
     stat = GE301(&pos,rad,ang3,ang4,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[1]);

     stat = GE301(&pos,rad,ang4,ang2,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[2]);
     break;
/*
***Four segments.
*/
     case 4:
     ang3 = ang1 + 90.0;
     stat = GE301(&pos,rad,ang1,ang3,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,basseg);

     ang4 = ang1 + 180.0;
     stat = GE301(&pos,rad,ang3,ang4,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[1]);
       
     ang5 = ang1 + 270.0;
     stat = GE301(&pos,rad,ang4,ang5,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[2]);

     stat = GE301(&pos,rad,ang5,ang2,segdat,&pval);
     stat = GE133(segdat,pval,&locseg);
     GEtfseg_to_basic(&locseg,pt,&basseg[3]);
     break;
/*
***5=full circle, special case with only two segments.
*/
     case 5:
     arcpek->ns_a = 2;
     stat = GE350(por,rad,pt,arcpek,basseg);
     break;
   }

   return(stat);
 }

/********************************************************************/
