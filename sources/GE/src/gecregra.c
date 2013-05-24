/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://www.oru.se/tech/cad/varkon                         */
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

/********************************************************************/

   DBstatus   GEcur_cre_gra(
   DBCurve   *p_cur,
   DBSeg     *p_seg,
   DBSeg    **pp_graseg)

/*
 *  Create graphical representation for a curve.
 *  Segments are added until the midpoints of the created
 *  graphical segments are within varkon_cgraph() from the 
 *  corresponding point on the original curve. We always start 
 *  by trying to use 2 gra seg for each geom seg.
 *  The approximate curve will be a non rational cubic curve.
 *
 *  Memory area is allocated for the graphical curve segment 
 *  coefficients. This area must be deallocated by the calling
 *  function!
 *
 *  In:
 *
 *    DBCurve *p_cur,      Curve
 *    DBSeg   *p_seg,      Alloc. area for curve geo segments 
 *    DBSeg  **pp_graseg   ptr to memory for graphic output segments 
 *
 *  Out:
 *    Cubic curve (coefficients) in allocated area pp_graseg 
 *
 *
 *  (C)Sören Larsson, Örebro University 2006-11-10  
 *
 *  2007-01-28 Tuning, J.Kjellander
 *
 ********************************************************************/

{ 
   DBint    ns_old;      /* No of segments in input curve              */
   DBint    ns_new;      /* No of new seg's for actual input segment   */
   DBint    no_seg_out;  /* No of segments in output  curve            */
   DBint    is_old;      /* Loop index: Segment in input curve         */
   EVALC    xyz_s;       /* Start point for old segment                */
   EVALC    xyz_e;       /* End point for old segment                  */
   EVALC    xyz_m;       /* Mid point for old segment                  */
   EVALC    xyz_m_new;   /* Mid point for new segment                  */
   DBint    is_new;      /* Loop index segment record                  */
   DBfloat  t_start;     /* Start parameter value for segment          */
   DBfloat  t_end;       /* End   parameter value for segment          */
   DBfloat  t_delta;     /* Step length                                */
   DBfloat  t0,t1;       /* Scaled tangents                            */
   DBfloat  graphtol;    /* Minimum error distance                     */
   DBfloat  comptol;     /* Tolerance                                  */
   DBint    status;      /* Error code from a called function          */
   DBfloat  dist;        /* Error distance                             */
   DBint    nalloc;      /* No of new segments allocated               */

/*
*** Number of geometric segments in the old curve:
*/
   ns_old = p_cur->ns_cu;
/*
*** Allocation of memory for minimum number of segments = ns_old.
*/
   if ( (*pp_graseg=DBcreate_segments(ns_old) ) == NULL )
     {
     return(varkon_erpush("GE6313",""));
     }

   nalloc = ns_old;

   no_seg_out = 0;
   graphtol   = varkon_cgraph();
   comptol    = varkon_comptol();
/*
***Loop through all geom. segments in the curve
*/
   xyz_s.evltyp     = EVC_DR + EVC_R; 
   xyz_e.evltyp     = EVC_DR + EVC_R;
   xyz_m.evltyp     = EVC_R;
   xyz_m_new.evltyp = EVC_R;

   for ( is_old=0; is_old<ns_old; ++is_old )
     {
/*
***Always start by trying 1 new segment for each old segment. This
***will work if the old segment is cubic non rational. If the input
***segment is rational, offset, NURBS or UV-segment more segments
***might be needed. We will never create more than 256 segments though.
*/
     ns_new = 1;
restart:
     t_delta = 1.0/ns_new;
/*
***If more space is needed, increase by ns_new + 50 seg
*/
     if ( nalloc < no_seg_out + ns_new )
       {
      *pp_graseg = DBadd_segments(*pp_graseg,no_seg_out + ns_new + 50);
       if ( *pp_graseg == NULL) return(varkon_erpush("GE6313",""));
       nalloc  = no_seg_out + ns_new + 50;
       }
/*
***Loop for the approximation of one original segment with ns_new segments.
*/
     for ( is_new=0; is_new < ns_new; ++is_new )
       {
       t_start  = is_new*t_delta ;
       t_end    = t_start + t_delta - comptol; 
       t_start  = t_start           + comptol;

       xyz_s.iseg        = xyz_e.iseg = xyz_m.iseg = is_old + 1;
       xyz_s.t_local     = t_start;
       xyz_e.t_local     = t_end; 
       xyz_m.t_local     = (t_start + t_end)*0.5; 

       xyz_m_new.iseg    = no_seg_out + 1;
       xyz_m_new.t_local = 0.5; 
/*
***Calculate r and dr/dt at both ends of the interval t_start -> t_end.
*/
       status = GE110((DBAny *)p_cur,p_seg+is_old,&xyz_s);
       if ( status < 0 )
         {
         return(status);
         }

       status = GE110((DBAny *)p_cur,p_seg+is_old,&xyz_e);
       if ( status < 0 )
         {
         return(status);
         }
/*
***Calculate segment coefficients.
***1:st derivatives are multiplied with t_delta to give correct tangent
***length for the new segment (scaling according to new parameterisation).
*/
       t0 = xyz_s.drdt.x_gm * t_delta; 
       t1 = xyz_e.drdt.x_gm * t_delta;
       (*pp_graseg + no_seg_out)->c0x = xyz_s.r.x_gm;
       (*pp_graseg + no_seg_out)->c1x = t0;
       (*pp_graseg + no_seg_out)->c2x = 3.0*(xyz_e.r.x_gm - xyz_s.r.x_gm) - 2.0*t0 - t1;
       (*pp_graseg + no_seg_out)->c3x = 2.0*(xyz_s.r.x_gm - xyz_e.r.x_gm) + t0 + t1;

       t0 = xyz_s.drdt.y_gm * t_delta;
       t1 = xyz_e.drdt.y_gm * t_delta;
       (*pp_graseg + no_seg_out)->c0y = xyz_s.r.y_gm;
       (*pp_graseg + no_seg_out)->c1y = t0;
       (*pp_graseg + no_seg_out)->c2y = 3.0*(xyz_e.r.y_gm - xyz_s.r.y_gm) - 2.0*t0 - t1;
       (*pp_graseg + no_seg_out)->c3y = 2.0*(xyz_s.r.y_gm - xyz_e.r.y_gm) + t0 + t1;

       t0 = xyz_s.drdt.z_gm * t_delta;
       t1 = xyz_e.drdt.z_gm * t_delta;
       (*pp_graseg + no_seg_out)->c0z = xyz_s.r.z_gm;
       (*pp_graseg + no_seg_out)->c1z = t0;
       (*pp_graseg + no_seg_out)->c2z = 3.0*(xyz_e.r.z_gm - xyz_s.r.z_gm) - 2.0*t0 - t1;
       (*pp_graseg + no_seg_out)->c3z = 2.0*(xyz_s.r.z_gm - xyz_e.r.z_gm) + t0 + t1;;

       (*pp_graseg + no_seg_out)->c0=1.0;
       (*pp_graseg + no_seg_out)->c1=0.0;
       (*pp_graseg + no_seg_out)->c2=0.0;
       (*pp_graseg + no_seg_out)->c3=0.0;

       (*pp_graseg + no_seg_out)->typ = CUB_SEG;
       (*pp_graseg + no_seg_out)->ofs = 0.0;
/*
***Calculate midpoint of new and original segment.
*/
       status = GE110((DBAny *)p_cur, *pp_graseg+no_seg_out, &xyz_m_new);
       if ( status < 0 )
         {
         return(status);
         }

       status = GE110((DBAny *)p_cur, p_seg+is_old, &xyz_m);
       if ( status < 0 )
         {
         return(status);
         }
/*
***Compare midpoints. If the error is too big, try with more segments
***up to a maximum of 64. 1:st try = 4, 2:nd = 16, 3:rd = 64, last = 256.
*/
       dist = SQRT((xyz_m.r.x_gm - xyz_m_new.r.x_gm) * (xyz_m.r.x_gm - xyz_m_new.r.x_gm) +
                   (xyz_m.r.y_gm - xyz_m_new.r.y_gm) * (xyz_m.r.y_gm - xyz_m_new.r.y_gm) +
                   (xyz_m.r.z_gm - xyz_m_new.r.z_gm) * (xyz_m.r.z_gm - xyz_m_new.r.z_gm));

       if ( dist > graphtol  &&  ns_new < 256 )
         {
         ns_new = ns_new*4;
         no_seg_out = no_seg_out - is_new;
         goto restart;
         }
       else
         { 
         no_seg_out = no_seg_out + 1;
         }
       }
     }
/*
*** Number of graphical segments in the new curve:
*/
   p_cur->nsgr_cu = (short)no_seg_out;

   return(SUCCED);
} 
/********************************************************************/
