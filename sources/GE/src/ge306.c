/*!******************************************************************/
/*  File: ge306.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE306() Creation of an offset circle ARC_OFFS in MBS            */
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

      DBstatus GE306(
      DBArc  *polarc,
      DBSeg  *polseg,
      DBfloat dist,
      DBTmat *pc,
      DBArc  *pa,
      DBSeg  *paseg)

/*    Create 3D arc/circle in the active XY-plane, offset to
 *    another circle.
 *
 *      In:  polarc => Pointer to original arc/circle
 *           polseg => Original segments
 *           dist   => Offset
 *           pc     => Active coordinate system matrix
 *
 *      Out: *pa    => The new circle
 *           *paseg => The new circle segments
 *
 *      (C)microform ab 1985-07-27 Gunnar Liden
 *
 *      1999-04-15 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat  radius;     /* The radius of the circle                */
   DBVector origin;     /* For GEmktf_3p                           */
   DBVector xaxis;      /* For GEmktf_3p                           */
   DBVector yaxis;      /* For GEmktf_3p                           */
   DBTmat   corcir;     /* Local definition coordinate system for  */
                        /* the circle (should be canonical data)   */
   DBTmat   transm;     /* For the translation of the circle to    */
                        /* the x/y-plane of the active coordinate  */
                        /* system (pc)                             */
   DBfloat  scale;      /* Scaling factor corresponding to the new */
                        /* offset radius for the circle            */
   int      i;          /* Loop index (all segments)               */
   GMSEG   *pcseg;      /* Pointer to the current segment          */
   GMSEG    traseg;     /* A segment transformed to corcir         */
   GMSEG    scaseg;     /* A segment scaled to the new offset      */
   DBVector origpc;     /* Origin in pc coordinates                */
   DBfloat  ztrans;     /* Translation <=> projection onto the     */
                        /* z plane of pc                           */
   DBTmat   scalem;     /* Scaling matrix                          */
   EVALC    evldat;     /* Fot evaluation in GE110()           */


/* Find the origin and the radius of the circle and  */
/* the definition z plane for the circle             */
/* Note that this can be replaced by the canonical   */
/* data if the coordinate system is added to the     */
/* canonical data                                    */

/*
***Check if 3D circle.
*/
   if ( polarc->ns_a != 0 )
     {
/*
***3D. Calculate 3D canonical data and create the
***arc plane.
*/
     evldat.evltyp = EVC_R + EVC_DR + EVC_PN + EVC_KAP;
     evldat.t_local = 0.0;
     GE110((GMUNON *)polarc,polseg,&evldat);

     radius = 1.0/evldat.kappa;

     origin.x_gm = evldat.r.x_gm + radius*evldat.p_norm.x_gm;
     origin.y_gm = evldat.r.y_gm + radius*evldat.p_norm.y_gm;
     origin.z_gm = evldat.r.z_gm + radius*evldat.p_norm.z_gm;

     xaxis.x_gm = evldat.r.x_gm - origin.x_gm;
     xaxis.y_gm = evldat.r.y_gm - origin.y_gm;
     xaxis.z_gm = evldat.r.z_gm - origin.z_gm;
     yaxis.x_gm = evldat.drdt.x_gm;
     yaxis.y_gm = evldat.drdt.y_gm;
     yaxis.z_gm = evldat.drdt.z_gm;

     if ( GEmktf_3p(&origin,&xaxis,&yaxis,&corcir) < 0 ) 
       return(erpush("GE3333","GE306(GEmktf_3p)"));
/*
***Find the scaling factor to get the new radius
*/
     if ( ABS(radius) > TOL1) scale = (radius + dist)/radius;
/*
***Find the translation which will project the new
***offset circle onto the z plane of pc
*/
     if ( GEtfpos_to_local(&origin,pc,&origpc) < 0 ) 
       return(erpush("GE3333","GE306(GEtfpos_to_local)"));
     ztrans =  - origpc.z_gm;
/*
***Create the translation matrix
*/
     transm.g11 = 1.0;
     transm.g12 = 0.0;
     transm.g13 = 0.0;
     transm.g14 = 0.0;

     transm.g21 = 0.0;
     transm.g22 = 1.0;
     transm.g23 = 0.0;
     transm.g24 = 0.0;

     transm.g31 = 0.0;
     transm.g32 = 0.0;
     transm.g33 = 1.0;
     transm.g34 = ztrans;

     transm.g41 = 0.0;
     transm.g42 = 0.0;
     transm.g43 = 0.0;
     transm.g44 = 1.0;
/*
***Create the scaling matrix
*/
     scalem.g11 = scale;
     scalem.g12 = 0.0;
     scalem.g13 = 0.0;
     scalem.g14 = 0.0;

     scalem.g21 = 0.0;
     scalem.g22 = scale;
     scalem.g23 = 0.0;
     scalem.g24 = 0.0;

     scalem.g31 = 0.0;
     scalem.g32 = 0.0;
     scalem.g33 = scale;
     scalem.g34 = 0.0;

     scalem.g41 = 0.0;
     scalem.g42 = 0.0;
     scalem.g43 = 0.0;
     scalem.g44 = 1.0;
/*
***Transform all segments to the local coordinate system,
***scale the segment and transform it back to the basic system
*/
     for ( i=0; i < polarc->ns_a; ++i )
       {
/*
***Pointer to the current segment in the input circle
*/
        pcseg = polseg + i;
/*
***Translate the current segment to corcir
*/
        if ( GEtfseg_to_local(pcseg,&corcir,&traseg) < 0 )
          return(erpush("GE3333","GE306(GEtfseg_to_local)"));
/*
***Scale the current segment
*/
        if ( GEtfseg_to_local(&traseg,&scalem,&scaseg) < 0 )
           return(erpush("GE3333","GE306(GEtfseg_to_local)"));
/*
***Project the scaled segment to the x/y plane of pc
*/
        if ( GEtfseg_to_local(&scaseg,&transm,&traseg) < 0 )
          return(erpush("GE3333","GE306(GEtfseg_to_local)"));
/*
***Pointer to the current segment in the output circle
*/
        pcseg = paseg + i;
/*
***Transform the current segment to basic coordinates
*/
        if ( GEtfseg_to_basic(&traseg,&corcir,pcseg) < 0 )
           return(erpush("GE3333","GE306(GEtfseg_to_basic)"));
/*
***Segment offset = 0. 1991-12-12 JK.
*/
        pcseg->ofs = 0.0;
        }
     }
/*
***2D-data.
*/
   if ( polarc->r_a+dist <= TOL1 ) return(erpush("GE3202","geo306"));

   pa->ns_a = polarc->ns_a;
   pa->x_a  = polarc->x_a;
   pa->y_a  = polarc->y_a;
   pa->r_a  = polarc->r_a + dist;
   pa->v1_a = polarc->v1_a;
   pa->v2_a = polarc->v2_a;

   return(0);
 } 

/********************************************************************/
