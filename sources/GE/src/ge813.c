/*!******************************************************************/
/*  File: ge813.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE813() Create offset curve                                     */
/*  GE814() Store curve plane with curve                            */
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
#include <memory.h> /* for memset */

#define NTEST 5      /* Number of curvature tests/segment */

/*!******************************************************************/

        DBstatus GE813(
        DBCurve *ocur,
        DBSeg   *oldgra,
        DBSeg   *oldgeo,
        DBTmat  *pc,
        DBfloat  offset,
        DBCurve *ncur,
        DBSeg   *newgra,
        DBSeg   *newgeo)

/*      Create analytical offset curve.
 *
 *      In: ocur   = Original curve
 *          oldgra = Original graphic segments
 *          oldgeo = Original geometric segments
 *          pc     = Active coordinate system
 *          offset = The offset value
 *
 *      Out:
 *          *ncur   = New curve
 *          *newgra = New graphical segments
 *          *newgeo = New geometrical segments
 *
 *      (C)microform ab 26/11/91 J. Kjellander
 *
 *      5/3/93     Typ av segment, J. Kjellander
 *      21/4/94    UV-segment, J. Kjellander
 *      1996-11-04 Antal nya grafiska segment, J. Kjellander
 *      1999-05-20 Rewritten, J.Kjellander
 *      2004-09-06 NURBS curves, Sören Larsson, Örebro University
 *
 ******************************************************************!*/

  {
   int        i,k;
   char       errbuf[80];
   bool       uvflag;
   bool       nurbsflag;
   DBfloat    angle,sprod,u,radius;
   DBVector   pn;
   DBSeg      traseg;
   EVALC      p1,p2;
   DBHvector *new_cpts_c,*old_cpts_c;
   DBfloat   *new_knots_c,*old_knots_c;
   DBVector   ocpt,ncpt,tcpt;
   DBfloat    w,inv_w;
   DBint      nknots,ncpts;
   
/*
***Check that the original curve is C1 continous
***across segment boundaries.
*/
   if ( ocur->ns_cu > 1 )
     {
     p1.evltyp = p2.evltyp = EVC_DR;
     p1.t_local = 1.0;
     p2.t_local = 0.0;
     for ( i=0; i<ocur->ns_cu-1; i++ )
       {
       GE110((DBAny *)ocur,oldgeo+i,&p1);
       GE110((DBAny *)ocur,oldgeo+i+1,&p2);
       GEvector_angled3D(&p1.drdt,&p2.drdt,&angle);

       if ( angle > TOL7 )
         {
         sprintf(errbuf,"%d%%%d",i+1,i+2);
         return(erpush("GE8262",errbuf));
         }
       }
     }

/*
***Check for the existence of NURBS-segments.
*/
   nurbsflag = FALSE;
   for ( i=0; i<ocur->ns_cu; ++i )
     {
     if ( (oldgeo+i)->typ == NURB_SEG )
       {
       nurbsflag  = TRUE;
       ncpts      = (oldgeo+i)->ncpts;
       nknots     = (oldgeo+i)->nknots;
       
       new_cpts_c   = (newgeo+i)->cpts_c; 
       new_knots_c  = (newgeo+i)->knots_c;
       old_cpts_c   = (oldgeo+i)->cpts_c; 
       old_knots_c  = (oldgeo+i)->knots_c;
        
       break;
       }
     }

/*
***Check for the existence of UV-segments.
*/
   uvflag = FALSE;
   for ( i=0; i<ocur->ns_cu; ++i )
     {
     if ( (oldgeo+i)->typ == UV_SEG )
       {
       uvflag = TRUE;
       break;
       }
     }
    
/*
***In that case the curve need to be be planar.
*/
   if ( uvflag  &&  !ocur->plank_cu ) return(erpush("GE8282",""));
/*
***Create new segments by copying old segments. If
***the old curve is non planar, the segments are
***projected to the active plane.
*/
   for ( i=0; i<ocur->ns_cu; ++i )
     { 
     if (nurbsflag )
       {
       V3MOME(oldgeo+i,newgeo+i,sizeof(GMSEG));
       (newgeo+i)->cpts_c   =  old_cpts_c ; /* theese where overwritten and  */
       (newgeo+i)->knots_c  =  old_knots_c; /* need to be put back           */
       }
     else if (ocur->plank_cu)
       {
       V3MOME(oldgeo+i,newgeo+i,sizeof(GMSEG));
       }       
     else
       {
       GEtfseg_to_local(oldgeo+i,pc,&traseg);
       traseg.c0z = traseg.c1z = traseg.c2z = traseg.c3z = 0.0;
       GEtfseg_to_basic(&traseg,pc,newgeo+i);
       }
     }
   if (nurbsflag )
     {
     V3MOME(old_knots_c,new_knots_c,nknots*sizeof(DBfloat));
     if (ocur->plank_cu)
       {
       V3MOME(old_cpts_c,new_cpts_c,ncpts*sizeof(DBHvector));
       }
     else
       {     
       for ( i=0; i<ncpts; ++i )
         {
         w = old_cpts_c[i].w_gm;
         inv_w = 1/w;
         ocpt.x_gm = old_cpts_c[i].x_gm * inv_w;
         ocpt.y_gm = old_cpts_c[i].y_gm * inv_w;
         ocpt.z_gm = old_cpts_c[i].z_gm * inv_w;
                 
         GEtfpos_to_local(&ocpt,pc,&tcpt);
         tcpt.z_gm=0;
         GEtfpos_to_basic(&tcpt,pc,&ncpt);
          
         new_cpts_c[i].x_gm = ncpt.x_gm * w;
         new_cpts_c[i].y_gm = ncpt.y_gm * w;
         new_cpts_c[i].z_gm = ncpt.z_gm * w;
         new_cpts_c[i].w_gm = w;
         }
       }
     }
  
/*
***Check that the new curve is free from loops.
***To do this we need a curve plane normal.
*/
   if ( ocur->plank_cu )
      {
      pn.x_gm = ocur->csy_cu.g31;
      pn.y_gm = ocur->csy_cu.g32;
      pn.z_gm = ocur->csy_cu.g33;
      }
   else
      {
      if ( pc == NULL )
         {
         pn.x_gm = 0.0; pn.y_gm = 0.0; pn.z_gm = 1.0;
         }
      else
         {
         pn.x_gm = pc->g31; pn.y_gm = pc->g32; pn.z_gm = pc->g33;
         }
      }
/*
***Check the curvature of the original but projected segment
***in some points. The scalar product between the plane normal
***and the curve binormal is positive if the curve turns to the
***left and negative if it turns to right. Opposite of the
***convention for positive and negative sides thus !
***If they have different signs, a loop exists !
*/
   p1.evltyp = EVC_BN + EVC_KAP;

   for ( k=0; k<ocur->ns_cu; ++k )
     { 
     for ( i=0; i<NTEST; ++i )
       {
       p1.t_local = u = (DBfloat)i/(DBfloat)(NTEST-1);
       GE110((DBAny *)ocur,newgeo+k,&p1);

       if ( p1.kappa > TOL2 )
         {
         radius = 1.0/p1.kappa;
         sprod = GEscalar_product3D(&pn,&p1.b_norm);
         if ( sprod*offset < 0.0  &&  ABS(offset) > radius )
           {
           sprintf(errbuf,"%d",k+1);
           return(erpush("GE8252",errbuf));
           }
         }
       }
/*
***If everything seems ok, save the new offset.
*/
     (newgeo+k)->ofs = (oldgeo+k)->ofs + offset;
     }
/*
***Save type, plane, segcount etc. in the curve struct.
***This function is used in cases where this is needed.
***(temporary curves)
*/
   ncur->hed_cu.type = CURTYP;
   ncur->ns_cu       = ocur->ns_cu;
   ncur->plank_cu    = TRUE;

   if ( ocur->plank_cu ) GE814(ncur,&ocur->csy_cu);
   else                  GE814(ncur,pc);


/************* GANMMAL KOD (TA BORT OM NY OK)*********

***If oldgra != oldgeo fix the graphical rep. as well.

   if ( oldgra != oldgeo)
     {
     for ( i=0; i<ocur->nsgr_cu; ++i )
       {
       V3MOME(oldgra+i,newgra+i,sizeof(GMSEG));
       (newgra+i)->ofs = (oldgra+i)->ofs + offset;
       }
     }
*/
   
/************* NY KOD *****************************   

***If oldgra != oldgeo fix the graphical rep. as well.
*/
   
   if ( oldgra != oldgeo)
     {
     for ( i=0; i<ocur->nsgr_cu; ++i )
       {
       if (ocur->plank_cu)
         {
         V3MOME(oldgra+i,newgra+i,sizeof(GMSEG));
         (newgra+i)->ofs = (oldgra+i)->ofs + offset;
         }       
       else
         {
         GEtfseg_to_local(oldgra+i,pc,&traseg);
         traseg.c0z = traseg.c1z = traseg.c2z = traseg.c3z = 0.0;
         GEtfseg_to_basic(&traseg,pc,newgra+i);
         (newgra+i)->ofs = (oldgra+i)->ofs + offset;
         }
       }
     }

/*
***Number of graphical segments.
*/
   ncur->nsgr_cu = ocur->nsgr_cu;   
   
   return(SUCCED);   
  
} 

/********************************************************************/
/*!******************************************************************/

       DBstatus GE814(
       DBCurve *cur,
       DBTmat  *pc)

/*     Saves the curve plane in a curve struct.
 *     Ifx  pc = NULL store BASIC.
 *
 *     In: cur = The curve
 *         pc  = Active coordinate system
 *
 *     Out: *cur.csy_cu.
 *
 *     (C)microform ab 25/11/91 J. Kjellander
 *
 *     1999-05-20 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

  {

   if ( pc == NULL )
     {
     cur->csy_cu.g11 = 1.0;
     cur->csy_cu.g12 = cur->csy_cu.g13 = cur->csy_cu.g14 = 0.0;

     cur->csy_cu.g22 = 1.0;
     cur->csy_cu.g21 = cur->csy_cu.g23 = cur->csy_cu.g24 = 0.0;

     cur->csy_cu.g33 = 1.0;
     cur->csy_cu.g31 = cur->csy_cu.g32 = cur->csy_cu.g34 = 0.0;

     cur->csy_cu.g44 = 1.0;
     cur->csy_cu.g41 = cur->csy_cu.g42 = cur->csy_cu.g43 = 0.0;
     }
   else V3MOME(pc,&cur->csy_cu,sizeof(DBTmat));

   return(0);
  }

/********************************************************************/
