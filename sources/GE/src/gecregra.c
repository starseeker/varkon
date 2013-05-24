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
/*  (C)Sören Larsson, Örebro university 2006-11-10                  */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"




/********************************************************************/

   DBstatus   GEcur_cre_gra (
   DBCurve *p_cur,      
   DBSeg   *p_seg,      
   DBSeg  **pp_graseg ) 

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
 *    DBSeg  **pp_graseg   ptr to Alloc. area for graphic segments 
 *         
 *  Out:             
 *    Rational cubic curve (coefficients) in allocated area pp_graseg 
 *
 *          
 *  (C) Sören Larsson, Örebro University 2006-11-10  
 *      
 *  Revisions: 
 *  2006-11-10   Originally written
 *                                    
 ********************************************************************/

{ 
   DBint    no_seg;      /* No of segments in input            curve   */
   DBint    no_seg_out;  /* No of segments in (R3)  output  curve      */
   DBint    ir;          /* Loop index: Segment in input curve         */
   EVALC    xyz_s;       /* Start point for output segment             */
   EVALC    xyz_e;       /* End   point for output segment             */
   EVALC    xyz_m;       /* Mid point target for output segment        */
   EVALC    xyz_m_new;   /* Mid point for output segment               */
   DBint    is;          /* Loop index segment record                  */
   DBfloat  t_start;     /* Start parameter value for segment          */
   DBfloat  t_end;       /* End   parameter value for segment          */
   DBfloat  t_delta; 
   DBfloat  t0,t1; 
   DBfloat  graphtol_sq; /* graphtol * graphtol                        */
   DBfloat  comptol;
   DBint    status;      /* Error code from a called function          */
   DBint    nnew;        /* no off new seg's for actual input segment  */
   DBfloat  dist_sq;
   DBint    nalloc;

/*
***  TODO select case segtype:
***
***    - case NURB_SEG and degree 1 to 3 --> calculate exact cubic seg
***    - case CUB_SEG  --> copy geo segdata
***    - else  --> use code below
***    
***    For the time beeing, we always use the code below.
*/


/*
*** Number of geometric segments in the curve:
*/
   no_seg= (p_cur)->ns_cu;
/*
*** Allocation of memory for some segments (4*no_seg).
*/
/*   if ( *pp_graseg  == NULL ) 
     {*/
     if((*pp_graseg = DBcreate_segments(no_seg * 4))==NULL)
       {                                    
       return(varkon_erpush("GE6313",""));
       } 
       nalloc=no_seg * 2;                                   
/*     }  */

   
     
   no_seg_out= 0;
   graphtol_sq=varkon_cgraph();  
   graphtol_sq=graphtol_sq*graphtol_sq;
   comptol=varkon_comptol();
/*
*** Loop through all geom. segments in the curve
*/
   xyz_s.evltyp     = EVC_DR + EVC_R; 
   xyz_e.evltyp     = EVC_DR + EVC_R;
   xyz_m.evltyp     = EVC_R;
   xyz_m_new.evltyp = EVC_R;
   
   for ( ir=0; ir < no_seg; ++ir )
     {
/*
*** Always start by trying 2 new graph. segments for each geometric seg.
*/     
     nnew=2;
restart:
     t_delta = 1.0/nnew;
     
     if ( nalloc < no_seg_out + nnew )
       {
/*
*** increase by 50 seg
*/       
       *pp_graseg  = DBadd_segments(*pp_graseg,no_seg_out + nnew + 50);
       if (*pp_graseg==NULL) return(varkon_erpush("GE6313",""));
       nalloc  = no_seg_out + nnew + 50;
       }
     
     for ( is=0; is < nnew; ++is )
       {
       t_start  = ((DBfloat)is)*t_delta ;
       t_end    = t_start + t_delta - comptol; 
       t_start  = t_start           + comptol;

       xyz_s.iseg        = xyz_e.iseg = xyz_m.iseg = ir+1;  
       xyz_s.t_local     = t_start;
       xyz_e.t_local     = t_end; 
       xyz_m.t_local     = (t_start+t_end)*0.5; 
       xyz_m_new.t_local = 0.5; 
       xyz_m_new.iseg    = no_seg_out+1;           

       if(GE110 ((DBAny *)p_cur,p_seg+ir,&xyz_s)<0) return(status);
       if(GE110 ((DBAny *)p_cur,p_seg+ir,&xyz_e)<0) return(status);
/*
*** Calculate segment coefficients.
*** 1:st derivatives are multiplicated with t_delta to give correct tangent
*** for the new segment (scaling according to new parameterisation).
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
       
       if(GE110 ((DBAny *)p_cur,p_seg+ir,&xyz_m)<0) return(status);       
       if(GE110 ((DBAny *)p_cur,*pp_graseg + no_seg_out,&xyz_m_new)<0) return(status);
/*
*** compare and restart this input segment if needed
*/     
       dist_sq = (xyz_m.r.x_gm - xyz_m_new.r.x_gm) * (xyz_m.r.x_gm - xyz_m_new.r.x_gm)+
                 (xyz_m.r.y_gm - xyz_m_new.r.y_gm) * (xyz_m.r.y_gm - xyz_m_new.r.y_gm)+
                 (xyz_m.r.z_gm - xyz_m_new.r.z_gm) * (xyz_m.r.z_gm - xyz_m_new.r.z_gm);
       if (dist_sq > graphtol_sq)
         {
         nnew = nnew*2;
         no_seg_out = no_seg_out - is;
         goto restart;
         }
        no_seg_out= no_seg_out + 1;
       }
     }
/*
*** Number of graphical segments in the curve:
*/
   p_cur->nsgr_cu = (short)no_seg_out;
   return(SUCCED);
} 
/********************************************************************/
