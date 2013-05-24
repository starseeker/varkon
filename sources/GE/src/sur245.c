/********************************************************************/
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
/*!                                                                 */
/*  Function: varkon_sur_coonseval                 File: sur245.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( s,t ) point on a given Coons patch          */
/*  defined by multi-segment curves.                                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-31   Originally written                                 */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_coonseval  Coons surface evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals          * Initialize EVALS                     */
/* varkon_GE717              * Function INV_ARCL                    */
/* varkon_GE109              * Curve evaluation function            */
/* varkon_pat_norm           * Normal with derivatives              */
/* varkon_erpush             * Error message to terminal            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_coonseval   */
/* SU2962 = sur245 Surface normal is a zero vector in u= , v=       */
/* SU2993 = Severe program error in varkon_sur_coonseval sur245.    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_sur_coonseval (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATB  *p_patb,      /* Coons surface                     (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat  s_sur,       /* Relative arclength in S (U) direction   */
   DBfloat  t_sur,       /* Relative arclength in T (V) direction   */
   EVALS   *p_xyz )      /* Coordinates and derivatives       (ptr) */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  cur_u0;        /* Curve U= 0                            */
   DBSeg   *p_u0;          /* Limit segment U= 0              (ptr) */
   DBfloat  s_u0;          /* Start local parameter value           */
   DBfloat  e_u0;          /* End   local parameter value           */
   DBCurve  cur_u1;        /* Curve U= 1                            */
   DBSeg   *p_u1;          /* Limit segment U= 1              (ptr) */
   DBfloat  s_u1;          /* Start local parameter value           */
   DBfloat  e_u1;          /* End   local parameter value           */
   DBCurve  cur_v0;        /* Curve V= 0                            */
   DBSeg   *p_v0;          /* Limit segment V= 0              (ptr) */
   DBfloat  s_v0;          /* Start local parameter value           */
   DBfloat  e_v0;          /* End   local parameter value           */
   DBCurve  cur_v1;        /* Curve V= 1                            */
   DBSeg   *p_v1;          /* Limit segment V= 1              (ptr) */
   DBfloat  s_v1;          /* Start local parameter value           */
   DBfloat  e_v1;          /* End   local parameter value           */

   DBint    n_u0;          /* Number of segments in cur_u0          */
   DBint    n_u1;          /* Number of segments in cur_u1          */
   DBint    n_v0;          /* Number of segments in cur_v0          */
   DBint    n_v1;          /* Number of segments in cur_v1          */

   DBVector r00;           /* Corner point s= 0  t= 0               */
   DBVector r01;           /* Corner point s= 0  t= 1               */
   DBVector r10;           /* Corner point s= 1  t= 0               */
   DBVector r11;           /* Corner point s= 1  t= 1               */

   DBTmat  *p_csys;        /* Coordinate system               (ptr) */

   DBfloat  rel_u0;        /* Relative arclength for curve cur_u0   */
   DBfloat  rel_u1;        /* Relative arclength for curve cur_u1   */
   DBfloat  rel_v0;        /* Relative arclength for curve cur_v0   */
   DBfloat  rel_v1;        /* Relative arclength for curve cur_v1   */

   DBfloat  tot_u0;        /* Total arclength of curve cur_u0       */
   DBfloat  tot_u1;        /* Total arclength of curve cur_u1       */
   DBfloat  tot_v0;        /* Total arclength of curve cur_v0       */
   DBfloat  tot_v1;        /* Total arclength of curve cur_v1       */

   DBfloat  param_u0;      /* Curve p_u0 parameter for s_sur        */
   DBfloat  param_u1;      /* Curve p_u1 parameter for s_sur        */
   DBfloat  param_v0;      /* Curve p_v0 parameter for t_sur        */
   DBfloat  param_v1;      /* Curve p_v1 parameter for t_sur        */

   EVALC    xyz_c;         /* Point and derivatives  GE109         */

   DBfloat  x0t,y0t,z0t;   /* r(0,t) = r0t = (x0t,y0t,z0t)          */
   DBfloat  x1t,y1t,z1t;   /* r(1,t) = r1t = (x1t,y1t,z1t)          */
   DBfloat  xs0,ys0,zs0;   /* r(s,0) = rs0 = (xs0,ys0,zs0)          */
   DBfloat  xs1,ys1,zs1;   /* r(s,1) = rs1 = (xs1,ys1,zs1)          */

   DBfloat  dx0tdt;        /* dr(0,t)/dt= dr0t/dt= (dx0t/dt, ...)   */
   DBfloat  dy0tdt;        /*                                       */
   DBfloat  dz0tdt;        /*                                       */
   DBfloat  dx1tdt;        /* dr(1,t)/dt= dr1t/dt= (dx1t/dt, ...)   */
   DBfloat  dy1tdt;        /*                                       */
   DBfloat  dz1tdt;        /*                                       */
   DBfloat  dxs0ds;        /* dr(s,0)/ds= drs0/ds= (dxs0/ds, ...)   */
   DBfloat  dys0ds;        /*                                       */
   DBfloat  dzs0ds;        /*                                       */
   DBfloat  dxs1ds;        /* dr(s,1)/ds= drs1/ds= (dxs1/ds, ...)   */
   DBfloat  dys1ds;        /*                                       */
   DBfloat  dzs1ds;        /*                                       */
   
   DBfloat  v_leng;        /* Length of a vector                    */

/*                                                                  */
/*----------------------------------------------------------------- */

   char     errbuf[80];  /* String for error message fctn erpush    */

   DBint    status;      /* Error code from a called function       */

/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* Refer to the definition of Coons patch (GMPATB in surdef.h)      */
/*! Coons patch with linear blending functions                     !*/
/*! ------------------------------------------                     !*/
/*!                                                                !*/
/*! Refererence: Faux & Pratt p 199 , Figure 7.1 and p 200  (7.4)  !*/
/*!                                                                !*/
/*! The surface patch is defined by four boundary curves which     !*/
/*! are blended together with blending functions which are         !*/
/*! linear.                                                        !*/
/*!                                                                !*/
/*! This surface patch may have many segments in the boundary      !*/
/*! curves. The parameter of the curves may be the input           !*/
/*! curve parameters (u,v) but the relative arclength for          !*/
/*! curves may also be the parameter (s,t).                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!          p_r01                               p_r11             !*/
/*!         s=0,t=1                             s=1,t=1            !*/
/*!           _______________________________________              !*/
/*!          /          r(s,1)= rs1 --->             !             !*/
/*!         /   /                               !    !             !*/
/*!        / r(0,t)=                         r(1,t)= !             !*/
/*!       /  = r0t                           = r1t   !             !*/
/*!      /            r(s,0)= rs0 --->               !             !*/
/*!     /____________________________________________!             !*/
/*!    s=0,t=0                                   s=1,t=0           !*/
/*!     p_r00                                     p_r10            !*/
/*!                                                                !*/
/*!                       _      _                        _   _    !*/
/*!           _       _  ! _      !    _              _  !     !   !*/
/*! _        !         ! ! r(0,t) !   ! _       _      ! ! 1-t !   !*/
/*! r(s,t) = ! (1-s) s !*! _      ! + ! r(s,0)  r(s,1) !*!     ! - !*/
/*!          !_       _! ! r(1,t) !   !_              _! !  t  !   !*/
/*!                      !_      _!                      !_   _!   !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!                        ! _       _      ! !     !              !*/
/*!                        ! r(0,0)  r(0,1) ! ! 1-t !              !*/
/*!          - ! (1-s) s !*! _       _      !*!     !              !*/
/*!                        ! r(1,0)  r(1,1) ! !     !              !*/
/*!                        !_              _! !  t  !              !*/
/*!                                           !_   _!              !*/
/*!                                                                !*/
/*!                       _   _                  _   _             !*/
/*!           _       _  !     !    _        _  !     !            !*/
/*! _        !         ! ! r0t !   !          ! ! 1-t !            !*/
/*! r(s,t) = ! (1-s) s !*!     ! + ! rs0  rs1 !*!     !  -         !*/
/*!          !_       _! ! r1t !   !_        _! !  t  !            !*/
/*!                      !_   _!                !_   _!            !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!                        !                ! !     !              !*/
/*!                        ! p_r00   p_r01  ! ! 1-t !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!                        ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  t  !              !*/
/*!                                           !_   _!              !*/
/*!                                                                !*/
/*! x(s,t)= (1-s)*  x0t + s*x1t + xs0*(1-t) + xs1*t -              !*/
/*!                  - ((1-s)*p_x00 + s*p_x10)*(1-t) -             !*/
/*!                  - ((1-s)*p_x01 + s*p_x11)*t                   !*/
/*! y(s,t)= (1-s)*  y0t + s*y1t + ys0*(1-t) + ys1*t -              !*/
/*!                  - ((1-s)*p_y00 + s*p_y10)*(1-t)  -            !*/
/*!                  - ((1-s)*p_y01 + s*p_y11)*t                   !*/
/*! z(s,t)= (1-s)*  z0t + s*z1t + zs0*(1-t) + zs1*t -              !*/
/*!                  - ((1-s)*p_z00 + s*p_z10)*(1-t)  -            !*/
/*!                  - ((1-s)*p_z01 + s*p_z11)*t                   !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  r(0,t) = r0t = (x0t,y0t,z0t)                                  !*/
/*!  r(1,t) = r1t = (x1t,y1t,z1t)                                  !*/
/*!  r(s,0) = rs0 = (xs0,ys0,zs0)                                  !*/
/*!  r(s,1) = rs1 = (xs1,ys1,zs1)                                  !*/
/*!  p_r00  = (p_x00,p_y00,p_z00)                                  !*/
/*!  p_r01  = (p_x01,p_y01,p_z01)                                  !*/
/*!  p_r10  = (p_x10,p_y10,p_z10)                                  !*/
/*!  p_r11  = (p_x11,p_y11,p_z11)                                  !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of r(s,t) with respect to s:                 !*/
/*!                                                                !*/
/*!                    _   _                        _   _          !*/
/*!          _     _  !     !    _              _  !     !         !*/
/*!   _     !       ! ! r0t !   !                ! ! 1-t !         !*/
/*!  dr/ds= ! -1  1 !*!     ! + ! drs0/ds drs1/ds!*!     !  -      !*/
/*!         !_     _! ! r1t !   !_              _! !  t  !         !*/
/*!                   !_   _!                      !_   _!         !*/
/*!                                                                !*/
/*!                       _             _   _    _                 !*/
/*!             _     _  !               ! !     !                 !*/
/*!            !       ! ! p_r00  p_r01  ! ! 1-t !                 !*/
/*!          - ! -1  1 !*!               !*!     !                 !*/
/*!            !_     _! ! p_r10  p_r11  ! !     !                 !*/
/*!                      !_             _! !  t  !                 !*/
/*!                                        !_   _!                 !*/
/*!                                                                !*/
/*!  dr/ds= -r0t +r1t + drs0/ds*(1-t)+ drs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_r00+p_r10)*(1-t) - (-p_r01+p_r11)*t            !*/
/*!                                                                !*/
/*!                                                                !*/
/*!  dx/ds= -x0t +x1t + dxs0/ds*(1-t)+ dxs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_x00+p_x10)*(1-t) - (-p_x01+p_x11)*t            !*/
/*!                                                                !*/
/*!  dy/ds= -y0t +y1t + dys0/ds*(1-t)+ dys1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_y00+p_y10)*(1-t) - (-p_y01+p_y11)*t            !*/
/*!                                                                !*/
/*!  dz/ds= -z0t +z1t + dzs0/ds*(1-t)+ dzs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_z00+p_z10)*(1-t) - (-p_z01+p_z11)*t            !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  dr(s,0)/ds = drs0/ds = (dxs0/ds,dys0/ds,dzs0/ds)              !*/
/*!  dr(s,1)/ds = drs1/ds = (dxs1/ds,dys1/ds,dzs1/ds)              !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of r(s,t) with respect to t:                 !*/
/*!                                                                !*/
/*!                     _      _                  _   _            !*/
/*!         _       _  !        !    _        _  !     !           !*/
/*!   _    !         ! ! dr0t/dt!   !          ! ! -1  !           !*/
/*!  dr/dt=! (1-s) s !*!        ! + ! rs0  rs1 !*!     !  -        !*/
/*!        !_       _! ! dr1t/dt!   !_        _! !  1  !           !*/
/*!                    !_      _!                !_   _!           !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! ! -1  !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  1  !              !*/
/*!                                           !_   _!              !*/
/*!   _                                                            !*/
/*!  dr/dt=(1-s)*dr0t/dt + s*dr1t/dt - rs0 + rs1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_r00+s*p_r10)*(-1) - ((1-s)*p_r01+s*p_r11)      !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dx/dt=(1-s)*dx0t/dt + S*dx1t/dt - xs0 + xs1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_x00+s*p_x10)*(-1) - ((1-s)*p_x01+s*p_x11)      !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dy/dt=(1-s)*dy0t/dt + S*dy1t/dt - ys0 + ys1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_y00+s*p_y10)*(-1) - ((1-s)*p_y01+s*p_y11)      !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dz/dt= (1-s)*dz0t/dt + S*dz1t/dt - zs0 + zs1  -               !*/
/*!                                                                !*/
/*!        ((1-s)*p_z00+s*p_z10)*(-1) - ((1-s)*p_z01+s*p_z11)      !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  dr(0,t)/dt = dr0t/dt = (dx0t/dt,dy0t/dt,dz0t/dt)              !*/
/*!  dr(1,t)/dt = dr1t/dt = (dx1t/dt,dy1t/dt,dz1t/dt)              !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/ds with respect to s:             !*/
/*!                                                                !*/
/*!                    _   _                              _   _    !*/
/*!            _   _  !     !    _                    _  !     !   !*/
/*!    _      !     ! ! r0t !   !                      ! ! 1-t !   !*/
/*!  d2r/ds2= ! 0 0 !*!     ! + ! d2rs0/ds2 d2rs1/ds2  !*!     ! - !*/
/*!           !_   _! ! r1t !   !_                    _! !  t  !   !*/
/*!                   !_   _!                            !_   _!   !*/
/*!                                                                !*/
/*!                       _             _   _    _                 !*/
/*!             _     _  !               ! !     !                 !*/
/*!            !       ! ! p_r00  p_r01  ! ! 1-t !                 !*/
/*!          - !  0  0 !*!               !*!     !                 !*/
/*!            !_     _! ! p_r10  p_r11  ! !     !                 !*/
/*!                      !_             _! !  t  !                 !*/
/*!                                        !_   _!                 !*/
/*!                                                                !*/
/*!  d2r/ds2=  dr2s0/ds2*(1-t)+ d2rs1/ds2*t                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!  d2x/ds2=  dx2s0/ds2*(1-t)+ d2xs1/ds2*t                        !*/
/*!  d2y/ds2=  dy2s0/ds2*(1-t)+ d2ys1/ds2*t                        !*/
/*!  d2y/ds2=  dy2s0/ds2*(1-t)+ d2ys1/ds2*t                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/dt with respect to t:             !*/
/*!                                                                !*/
/*!                       _         _               _ _            !*/
/*!           _       _  !           !  _       _  !   !           !*/
/*!     _    !         ! ! d2r0t/dt2 ! !         ! ! 0 !           !*/
/*!  d2r/dt2=! (1-s) s !*!           !+! rs0 rs1 !*!   !  -        !*/
/*!          !_       _! ! d2r1t/dt2 ! !_       _! ! 0 !           !*/
/*!                      !_         _!             !_ _!           !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! !  0  !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  0  !              !*/
/*!                                           !_   _!              !*/
/*!    _                                                           !*/
/*!  d2r/dt2= (1-s)*d2r0t/dt2 + s*d2r1t/dt2                        !*/
/*!                                                                !*/
/*!  d2x/dt2= (1-s)*d2x0t/dt2 + s*d2x1t/dt2                        !*/
/*!  d2y/dt2= (1-s)*d2y0t/dt2 + s*d2y1t/dt2                        !*/
/*!  d2z/dt2= (1-s)*d2z0t/dt2 + s*d2z1t/dt2                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/dt with respect to s:             !*/
/*!                                                                !*/
/*!                     _      _                  _   _            !*/
/*!            _     _  !        !    _        _  !     !          !*/
/*!    _      !       ! ! dr0t/dt!   !          ! ! -1  !          !*/
/*!  d2r/dsdt=! -1  1 !*!        ! + ! rs0  rs1 !*!     !  -       !*/
/*!           !_     _! ! dr1t/dt!   !_        _! !  1  !          !*/
/*!                     !_      _!                !_   _!          !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! ! -1  !              !*/
/*!          - !   -1  1 !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  1  !              !*/
/*!                                           !_   _!              !*/
/*!   _                                                            !*/
/*!  d2r/dsdt= -dr0t/dt + dr1t/dt - drs0/dt + drs1/dt              !*/
/*!          - p_r00 + p_r10 + p_r01 - p_r11                       !*/
/*!                                                                !*/
/*!  d2x/dsdt= -dx0t/dt + dx1t/dt - dxs0/dt + dxs1/dt              !*/
/*!          - p_x00 + p_x10 + p_x01 - p_x11                       !*/
/*!                                                                !*/
/*!  d2y/dsdt= -dy0t/dt + dy1t/dt - dys0/dt + dys1/dt              !*/
/*!          - p_y00 + p_y10 + p_y01 - p_y11                       !*/
/*!                                                                !*/
/*!  d2z/dsdt= -dz0t/dt + dz1t/dt - dzs0/dt + dzs1/dt              !*/
/*!          - p_z00 + p_z10 + p_z01 - p_z11                       !*/
/*!                                                                !*/
/*!                                                                !*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur245 Enter *** varkon_sur_coonseval s %7.4f t %7.4f p_patb %d\n",
          s_sur,t_sur, p_patb);
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* _________________________________________                        */
/*                                                                  */
/*  Check that c_flag is two (2), for multi-segment curves.         */
/*                                                                 !*/

   if ( p_patb->c_flag == 2 )
      {
      ;
      }
   else
      {
      sprintf(errbuf,"c_flag%%varkon_sur_coonseval (sur245)");
      return(varkon_erpush("SU2993",errbuf));
      }

/* Initialize variable EVALS                                        */
/* Call of varkon_ini_evals (sur170).                               */
/*                                                                  */

      varkon_ini_evals (p_xyz);  

/* Input s_sur, t_sur and  icase to p_xyz.                          */

   p_xyz->u      = s_sur;         /* U parameter value              */
   p_xyz->v      = t_sur;         /* V parameter value              */
   p_xyz->e_case = icase;         /* Evaluation case                */
   p_xyz->s_anal = FALSE;         /* No surface analysis            */


    tot_u0     = F_UNDEF; 
    tot_u1     = F_UNDEF;
    tot_v0     = F_UNDEF;
    tot_v1     = F_UNDEF;
    dx0tdt     = F_UNDEF;
    dy0tdt     = F_UNDEF;
    dz0tdt     = F_UNDEF;
    dx1tdt     = F_UNDEF;
    dy1tdt     = F_UNDEF;
    dz1tdt     = F_UNDEF;
    dxs0ds     = F_UNDEF;
    dys0ds     = F_UNDEF;
    dzs0ds     = F_UNDEF;
    dxs1ds     = F_UNDEF;
    dys1ds     = F_UNDEF;
    dzs1ds     = F_UNDEF;
   
    v_leng     = F_UNDEF;

/*!                                                                 */
/*  Segment addresses from GMPATB to p_u0, p_u1, p_v0, p_v1         */
/*                                                                 !*/

p_u0= p_patb->p_seg_r0t;     /* Curve segments for cur_r0t    (ptr) */
p_u1= p_patb->p_seg_r1t;     /* Curve segments for cur_r1t    (ptr) */
p_v0= p_patb->p_seg_rs0;     /* Curve segments for cur_rs0    (ptr) */
p_v1= p_patb->p_seg_rs1;     /* Curve segments for cur_rs1    (ptr) */

/*!                                                                 */
/*  Corner points to local parameters r00, r01, r10 and r11.        */
/*                                                                 !*/

r00 = p_patb->p_r00;         /* Corner point s= 0  t= 0             */
r01 = p_patb->p_r01;         /* Corner point s= 0  t= 1             */
r10 = p_patb->p_r10;         /* Corner point s= 1  t= 0             */
r11 = p_patb->p_r11;         /* Corner point s= 1  t= 1             */

/*!                                                                 */
/*  Number of segments from GMPATB to n_u0, n_u1, n_v0, n_v1        */
/*                                                                 !*/

cur_u0 = p_patb->cur_r0t;
cur_u1 = p_patb->cur_r1t;
cur_v0 = p_patb->cur_rs0;
cur_v1 = p_patb->cur_rs1;

n_u0 = (DBint)cur_u0.ns_cu;
n_u1 = (DBint)cur_u1.ns_cu;
n_v0 = (DBint)cur_v0.ns_cu;
n_v1 = (DBint)cur_v1.ns_cu;

/*!                                                                 */
/*  Parameter intervals from GMPATB to s_u0, e_u0, s_u1, .....      */
/*                                                                 !*/

s_u0 = p_patb->s_u0t;        /* Start parameter value for cur_r0t   */
e_u0 = p_patb->e_u0t;        /* End   parameter value for cur_r0t   */
s_u1 = p_patb->s_u1t;        /* Start parameter value for cur_r1t   */
e_u1 = p_patb->e_u1t;        /* End   parameter value for cur_r1t   */
s_v0 = p_patb->s_u0s;        /* Start parameter value for cur_r0s   */
e_v0 = p_patb->e_u0s;        /* End   parameter value for cur_r0s   */
s_v1 = p_patb->s_u1s;        /* Start parameter value for cur_r1s   */
e_v1 = p_patb->e_u1s;        /* End   parameter value for cur_r1s   */

/*!                                                                 */
/*  Check that the whole curves are used .....................      */
/*    ... other cases not yet implemented  ...................      */
/*                                                                 !*/

/*  Coordinate system for GE717.                                    */

p_csys = NULL;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 p_u0 %d s_u0 %f e_u0 %f n_u0 %d\n"
                 ,p_u0,s_u0,e_u0,n_u0);
fprintf(dbgfil(SURPAC),"sur245 p_u1 %d s_u1 %f e_u1 %f n_u1 %d\n"
                 ,p_u1,s_u1,e_u1,n_u1);
fprintf(dbgfil(SURPAC),"sur245 p_v0 %d s_v0 %f e_v0 %f n_v0 %d\n"
                 ,p_v0,s_v0,e_v0,n_v0);
fprintf(dbgfil(SURPAC),"sur245 p_v1 %d s_v1 %f e_v1 %f n_v1 %d\n"
                 ,p_v1,s_v1,e_v1,n_v1);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 p_u0 %d s_u0 %f e_u0 %f n_u0 %d\n"
                     ,p_u0,s_u0,e_u0,n_u0);
fprintf(dbgfil(SURPAC),"sur245 p_u1 %d s_u1 %f e_u1 %f n_u1 %d\n"
                     ,p_u1,s_u1,e_u1,n_u1);
fprintf(dbgfil(SURPAC),"sur245 p_v0 %d s_v0 %f e_v0 %f n_v0 %d\n"
                     ,p_v0,s_v0,e_v0,n_v0);
fprintf(dbgfil(SURPAC),"sur245 p_v1 %d s_v1 %f e_v1 %f n_v1 %d\n"
                     ,p_v1,s_v1,e_v1,n_v1);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 r00 %f %f %f\n", 
            r00.x_gm, r00.y_gm, r00.z_gm );
fprintf(dbgfil(SURPAC),"sur245 r01 %f %f %f\n", 
            r01.x_gm, r01.y_gm, r01.z_gm );
fprintf(dbgfil(SURPAC),"sur245 r10 %f %f %f\n", 
            r10.x_gm, r10.y_gm, r10.z_gm );
fprintf(dbgfil(SURPAC),"sur245 r11 %f %f %f\n", 
            r11.x_gm, r11.y_gm, r11.z_gm );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 2. Calculate curve parameter for the given relative arclengths   */
/* ______________________________________________________________   */
/*                                                                 !*/

  if  ( s_u0  <   e_u0 ) rel_u0 =   t_sur;
  else                   rel_u0 = 1.0 - t_sur;  /* Reversed curve   */
  if  ( s_u1  <   e_u1 ) rel_u1 =   t_sur;
  else                   rel_u1 = 1.0 - t_sur;
  if  ( s_v0  <   e_v0 ) rel_v0 =   s_sur;
  else                   rel_v0 = 1.0 - s_sur;
  if  ( s_v1  <   e_v1 ) rel_v1 =   s_sur;
  else                   rel_v1 = 1.0 - s_sur;

   status = GE717 ((DBAny*)&cur_u0,p_u0, p_csys, rel_u0 , &param_u0 );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 rel_u0%%varkon_sur_coonseval (sur245)");
        return(varkon_erpush("SU2943",errbuf));
        }

   status = GE717 ((DBAny*)&cur_u1,p_u1, p_csys, rel_u1 , &param_u1 );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 rel_u1%%varkon_sur_coonseval (sur245)");
        return(varkon_erpush("SU2943",errbuf));
        }


   status = GE717 ((DBAny*)&cur_v0,p_v0, p_csys, rel_v0 , &param_v0 );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 rel_v0%%varkon_sur_coonseval (sur245)");
        return(varkon_erpush("SU2943",errbuf));
        }

   status = GE717 ((DBAny*)&cur_v1,p_v1, p_csys, rel_v1 , &param_v1 );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 rel_v1%%varkon_sur_coonseval (sur245)");
        return(varkon_erpush("SU2943",errbuf));
        }

tot_u0 = cur_u0.al_cu;
tot_u1 = cur_u1.al_cu;
tot_v0 = cur_v0.al_cu;
tot_v1 = cur_v1.al_cu;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 tot_u0 %8.2f rel_u0 %5.4f param_u0 %10.6f \n",
                               tot_u0, rel_u0,param_u0);
fprintf(dbgfil(SURPAC),"sur245 tot_u1 %8.2f rel_u1 %5.4f param_u1 %10.6f \n",
                               tot_u1, rel_u1,param_u1);
fprintf(dbgfil(SURPAC),"sur245 tot_v0 %8.2f rel_v0 %5.4f param_v0 %10.6f \n",
                               tot_v0, rel_v0,param_v0);
fprintf(dbgfil(SURPAC),"sur245 tot_v1 %8.2f rel_v1 %5.4f param_v1 %10.6f \n",
                               tot_v1, rel_v1,param_v1);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Get coordinates and derivatives. Calls of varkon_GE109 (GE109) */
/*                                                                 !*/

   if       ( icase == 0 ) xyz_c.evltyp   = EVC_R; 
   else if  ( icase == 1 ) xyz_c.evltyp   = EVC_DR; 
   else if  ( icase == 2 ) xyz_c.evltyp   = EVC_DR; 
   else
      {
      sprintf(errbuf,"icase%%varkon_sur_coonseval (sur245)");
      return(varkon_erpush("SU2993",errbuf));
      }

/*   r(0,t) = r0t = (x0t,y0t,z0t)                                   */
/*  dr(0,t)/dt= drt0/dt= (dxt0/dt, ...)                             */

   xyz_c.t_global = param_u0;       /* Global parameter value       */

status=GE109 ((DBAny *)&cur_u0 , p_u0 , &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur245");
     return(varkon_erpush("SU2943",errbuf));
    }

  x0t = xyz_c.r.x_gm;
  y0t = xyz_c.r.y_gm;
  z0t = xyz_c.r.z_gm;

  if  ( icase == 1 || icase == 2 ) 
    {
    dx0tdt = xyz_c.drdt.x_gm; 
    dy0tdt = xyz_c.drdt.y_gm;
    dz0tdt = xyz_c.drdt.z_gm;
    v_leng = SQRT(dx0tdt*dx0tdt +  dy0tdt*dy0tdt +  dz0tdt*dz0tdt);  
    if  ( v_leng > 0.0000001 )
      {
      dx0tdt = dx0tdt/v_leng*tot_u0; 
      dy0tdt = dy0tdt/v_leng*tot_u0;
      dz0tdt = dz0tdt/v_leng*tot_u0;
      if  ( s_u0  >   e_u0 ) 
        {
        dx0tdt = -dx0tdt; 
        dy0tdt = -dy0tdt;
        dz0tdt = -dz0tdt;
        }
      }
    else
      {
      sprintf(errbuf,"dr0tdt=0%%sur245");
      return(varkon_erpush("SU2993",errbuf));
      }
    }

/*   r(1,t) = r1t = (x1t,y1t,z1t)                                   */
/*  dr(1,t)/dt= dr1t/dt= (dx1t/dt, ...)                             */

   xyz_c.t_global = param_u1;       /* Global parameter value       */

status=GE109 ((DBAny *)&cur_u1 , p_u1 , &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur245");
     return(varkon_erpush("SU2943",errbuf));
    }

  x1t = xyz_c.r.x_gm;
  y1t = xyz_c.r.y_gm;
  z1t = xyz_c.r.z_gm;

  if  ( icase == 1 || icase == 2 ) 
    {
    dx1tdt = xyz_c.drdt.x_gm; 
    dy1tdt = xyz_c.drdt.y_gm;
    dz1tdt = xyz_c.drdt.z_gm;
    v_leng = SQRT(dx1tdt*dx1tdt +  dy1tdt*dy1tdt +  dz1tdt*dz1tdt);  
    if  ( v_leng > 0.0000001 )
      {
      dx1tdt = dx1tdt/v_leng*tot_u1; 
      dy1tdt = dy1tdt/v_leng*tot_u1;
      dz1tdt = dz1tdt/v_leng*tot_u1;
      if  ( s_u1  >   e_u1 ) 
        {
        dx1tdt = -dx1tdt; 
        dy1tdt = -dy1tdt;
        dz1tdt = -dz1tdt;
        }
      }
    else
      {
      sprintf(errbuf,"dr1tdt=0%%sur245");
      return(varkon_erpush("SU2993",errbuf));
      }
    }
/*   r(s,0) = rs0 = (xs0,ys0,zs0)                                   */
/*  dr(s,0)/ds= drs0/ds= (dxs0/ds, ...)                             */

   xyz_c.t_global = param_v0;       /* Global parameter value       */

status=GE109 ((DBAny *)&cur_v0 , p_v0 , &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur245");
     return(varkon_erpush("SU2943",errbuf));
    }

  xs0 = xyz_c.r.x_gm;
  ys0 = xyz_c.r.y_gm;
  zs0 = xyz_c.r.z_gm;

  if  ( icase == 1 || icase == 2 ) 
    {
    dxs0ds = xyz_c.drdt.x_gm; 
    dys0ds = xyz_c.drdt.y_gm;
    dzs0ds = xyz_c.drdt.z_gm;
    v_leng = SQRT(dxs0ds*dxs0ds +  dys0ds*dys0ds +  dzs0ds*dzs0ds);  
    if  ( v_leng > 0.0000001 )
      {
      dxs0ds = dxs0ds/v_leng*tot_v0; 
      dys0ds = dys0ds/v_leng*tot_v0;
      dzs0ds = dzs0ds/v_leng*tot_v0;
      if  ( s_v0  >   e_v0 ) 
        {
        dxs0ds = -dxs0ds; 
        dys0ds = -dys0ds;
        dzs0ds = -dzs0ds;
        }
      }
    else
      {
      sprintf(errbuf,"drs0ds=0%%sur245");
      return(varkon_erpush("SU2993",errbuf));
      }
    }

/*   r(s,1) = rs1 = (xs1,ys1,zs1)                                   */
/*  dr(s,1)/ds= drs1/ds= (dxs1/ds, ...)                             */

   xyz_c.t_global = param_v1;       /* Global parameter value       */

status=GE109 ((DBAny *)&cur_v1 , p_v1 , &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur245");
     return(varkon_erpush("SU2943",errbuf));
    }

  xs1 = xyz_c.r.x_gm;
  ys1 = xyz_c.r.y_gm;
  zs1 = xyz_c.r.z_gm;

  if  ( icase == 1 || icase == 2 ) 
    {
    dxs1ds = xyz_c.drdt.x_gm; 
    dys1ds = xyz_c.drdt.y_gm;
    dzs1ds = xyz_c.drdt.z_gm;
    v_leng = SQRT(dxs1ds*dxs1ds +  dys1ds*dys1ds +  dzs1ds*dzs1ds);  
    if  ( v_leng > 0.0000001 )
      {
      dxs1ds = dxs1ds/v_leng*tot_v1; 
      dys1ds = dys1ds/v_leng*tot_v1;
      dzs1ds = dzs1ds/v_leng*tot_v1;
      if  ( s_v1  >   e_v1 ) 
        {
        dxs1ds = -dxs1ds; 
        dys1ds = -dys1ds;
        dzs1ds = -dzs1ds;
        }
      }
    else
      {
      sprintf(errbuf,"drs1ds=0%%sur245");
      return(varkon_erpush("SU2993",errbuf));
      }
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 x0t    %8.1f y0t    %8.1f z0t    %8.1f \n",x0t,y0t,z0t);
fprintf(dbgfil(SURPAC),"sur245 dx0tdt %8.2f dy0tdt %8.2f dz0tdt %8.2f \n",
                               dx0tdt,      dy0tdt,      dz0tdt);
fprintf(dbgfil(SURPAC),"sur245 x1t    %8.1f y1t    %8.1f z1t    %8.1f \n",x1t,y1t,z1t);
fprintf(dbgfil(SURPAC),"sur245 dx1tdt %8.2f dy1tdt %8.2f dz1tdt %8.2f \n",
                               dx1tdt,      dy1tdt,      dz1tdt);
fprintf(dbgfil(SURPAC),"sur245 xs0    %8.1f ys0    %8.1f zs0    %8.1f \n",xs0,ys0,zs0);
fprintf(dbgfil(SURPAC),"sur245 dxs0ds %8.2f dys0ds %8.2f dzs0ds %8.2f \n",
                               dxs0ds,      dys0ds,      dzs0ds);
fprintf(dbgfil(SURPAC),"sur245 xs1    %8.1f ys1    %8.1f zs1    %8.1f \n",xs1,ys1,zs1);
fprintf(dbgfil(SURPAC),"sur245 dxs1ds %8.2f dys1ds %8.2f dzs1ds %8.2f \n",
                               dxs1ds,      dys1ds,      dzs1ds);
fflush(dbgfil(SURPAC));
}
#endif

  p_xyz->r_x = (1.0-s_sur)*x0t+s_sur*x1t+xs0*(1.0-t_sur)+xs1*t_sur 
               - ((1.0-s_sur)*r00.x_gm+s_sur*r10.x_gm)*(1.0-t_sur)
               - ((1.0-s_sur)*r01.x_gm+s_sur*r11.x_gm)*t_sur;
  p_xyz->r_y = (1.0-s_sur)*y0t+s_sur*y1t+ys0*(1.0-t_sur)+ys1*t_sur 
               - ((1.0-s_sur)*r00.y_gm+s_sur*r10.y_gm)*(1.0-t_sur)
               - ((1.0-s_sur)*r01.y_gm+s_sur*r11.y_gm)*t_sur;
  p_xyz->r_z = (1.0-s_sur)*z0t+s_sur*z1t+zs0*(1.0-t_sur)+zs1*t_sur 
               - ((1.0-s_sur)*r00.z_gm+s_sur*r10.z_gm)*(1.0-t_sur)
               - ((1.0-s_sur)*r01.z_gm+s_sur*r11.z_gm)*t_sur;

  if  ( icase == 1 || icase == 2 ) 
    {
    p_xyz->u_x = -x0t + x1t + dxs0ds*(1.0-t_sur)+dxs1ds*t_sur 
                 - (-r00.x_gm+r10.x_gm)*(1.0-t_sur)
                 - ((-1.0)*r01.x_gm+r11.x_gm)*t_sur;
    p_xyz->u_y = -y0t + y1t + dys0ds*(1.0-t_sur)+dys1ds*t_sur 
                 - (-r00.y_gm+r10.y_gm)*(1.0-t_sur)
                 - ((-1.0)*r01.y_gm+r11.y_gm)*t_sur;
    p_xyz->u_z = -z0t + z1t + dzs0ds*(1.0-t_sur)+dzs1ds*t_sur 
                 - (-r00.z_gm+r10.z_gm)*(1.0-t_sur)
                 - ((-1.0)*r01.z_gm+r11.z_gm)*t_sur;

    p_xyz->v_x = (1.0-s_sur)*dx0tdt+s_sur*dx1tdt-xs0+xs1
               - ((1.0-s_sur)*r00.x_gm+s_sur*r10.x_gm)*(-1.0)
               - ((1.0-s_sur)*r01.x_gm+s_sur*r11.x_gm);
    p_xyz->v_y = (1.0-s_sur)*dy0tdt+s_sur*dy1tdt-ys0+ys1
               - ((1.0-s_sur)*r00.y_gm+s_sur*r10.y_gm)*(-1.0)
               - ((1.0-s_sur)*r01.y_gm+s_sur*r11.y_gm);
    p_xyz->v_z = (1.0-s_sur)*dz0tdt+s_sur*dz1tdt-zs0+zs1
               - ((1.0-s_sur)*r00.z_gm+s_sur*r10.z_gm)*(-1.0)
               - ((1.0-s_sur)*r01.z_gm+s_sur*r11.z_gm);

    p_xyz->n_x = p_xyz->u_y*p_xyz->v_z - p_xyz->u_z*p_xyz->v_y;
    p_xyz->n_y = p_xyz->u_z*p_xyz->v_x - p_xyz->u_x*p_xyz->v_z;
    p_xyz->n_z = p_xyz->u_x*p_xyz->v_y - p_xyz->u_y*p_xyz->v_x;

    v_leng = SQRT( p_xyz->n_x*p_xyz->n_x  + p_xyz->n_y*p_xyz->n_y  + 
                   p_xyz->n_z*p_xyz->n_z );
    if ( v_leng > 1e-8  ) 
        {
        p_xyz->n_x = p_xyz->n_x/v_leng;
        p_xyz->n_y = p_xyz->n_y/v_leng;
        p_xyz->n_z = p_xyz->n_z/v_leng;
        }
    else
      {
      sprintf(errbuf,"normal=0%%sur245");
      return(varkon_erpush("SU2993",errbuf));
      }

    }   /*  End   icase == 1 || icase == 2  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur245 S %8.4f T %8.4f  X %8.1f  Y %8.1f  Z %8.1f\n",
                 s_sur, t_sur, p_xyz->r_x, p_xyz->r_y, p_xyz->r_z);
fprintf(dbgfil(SURPAC),"sur245 XN %8.5f YN %8.5f ZN %8.5f\n",
                  p_xyz->n_x, p_xyz->n_y, p_xyz->n_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur245 S %8.4f T %8.4f XU %8.1f YU %8.1f ZU %8.1f\n",
                 s_sur, t_sur, p_xyz->u_x, p_xyz->u_y, p_xyz->u_z);
fprintf(dbgfil(SURPAC),"sur245 S %8.4f T %8.4f XV %8.1f YV %8.1f ZV %8.1f\n",
                 s_sur, t_sur, p_xyz->v_x, p_xyz->v_y, p_xyz->v_z);
fflush(dbgfil(SURPAC));
}
#endif

#ifdef  TODO_SOMETHING_I_HAVE_FORGOTTEN

   u2_x=  out_s0[6]*(1.0-t_l)+ out_s1[6]*t_l; 
   u2_y=  out_s0[7]*(1.0-t_l)+ out_s1[7]*t_l; 
   u2_z=  out_s0[8]*(1.0-t_l)+ out_s1[8]*t_l; 

   v2_x= (1.0-s_l)*out_0t[6] + s_l*out_1t[6];
   v2_y= (1.0-s_l)*out_0t[7] + s_l*out_1t[7];
   v2_z= (1.0-s_l)*out_0t[8] + s_l*out_1t[8];

   uv_x= -out_0t[3] + out_1t[3] - out_s0[3] + out_s1[3] -   
           r00.x_gm + r10.x_gm + r01.x_gm - r11.x_gm;

   uv_y= -out_0t[4] + out_1t[4] - out_s0[4] + out_s1[4] -   
           r00.y_gm + r10.y_gm + r01.y_gm - r11.y_gm;

   uv_z= -out_0t[5] + out_1t[5] - out_s0[5] + out_s1[5] -   
           r00.z_gm + r10.z_gm + r01.z_gm - r11.z_gm;

/*!                                                                 */
/* 4. Surface normal and surface normal derivatives                 */
/* ________________________________________________                 */
/*                                                                  */
/* Calculate surface normal and derivatives w.r.t u and v.          */
/* Error SU2963 for a zero length surface normal.                   */
/* Call of varkon_pat_norm (sur240).                                */
/*                                                                 !*/

   status=varkon_pat_norm (icase,p_xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }


#endif   /*  TODO_SOMETHING_I_HAVE_FORGOTTEN */

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
