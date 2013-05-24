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
/*  Function: varkon_pat_ratcre2                   File: sur252.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a ruled surface patch.                     */
/*                                                                  */
/*  Input data is two boundary curve segments.                      */
/*  The output patch will be a cubic rational polynomial            */
/*  in one direction and straight lines (defree one) curves         */
/*  in the other direction.                                         */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-06   Originally written                                 */
/*  1994-10-21   varkon_pat_prirat (sur234) added                   */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1996-09-07   Change of short description, elim. comp. warning   */
/*  1999-12-02   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_ratcre2    Create a ruled rational patch    */
/*                                                              */
/*------------------------------------------------------------- */

#ifdef  DEBUG
/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Initialization of variables        */
/*                                                                  */
/*-----------------------------------------------------------------!*/
#endif

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_pat_prirat    * Printout of rational patch data           */
/* GE107                * Curve segment evaluation. For Debug On    */
/* varkon_erpush        * Error message             For Debug On    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_ratcre2   */
/* SU2993 = Severe program error in varkon_pat_ratcre2 (sur252).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_pat_ratcre2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0,         /* Boundary segment U= 0.0           (ptr) */
   DBSeg  *p_u1,         /* Boundary segment U= 1.0           (ptr) */
   GMPATR *p_patr )      /* Rational cubic patch              (ptr) */

/* Out:                                                             */
/*       Data to p_patr                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

#ifdef  DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */
#endif

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* References:                                                      */
/* 1. Faux & Pratt, pages 147,                                      */
/* 2. Ball's paper about Consurf                                    */
/*                                                                  */
/* Rational cubic curve segment (Generalised conic)                 */
/* ------------------------------------------------                 */
/*                                                                  */
/* The generalised conic is described in reference 1 and 2.         */
/* The curve is a rational cubic with a Consurf polygon,            */
/* where P1 is the start point, P2 and P3 are tangent points        */
/* and P4 is the end point. Points P1, P2, P3 and P4 are            */
/* 4D points (homogenous coordinates) and for a generalised         */
/* conic will the weights w1 be equal to w4 and w2 be equal         */
/* to w3 (w2=w3=1 and w1=w4=1/p-1 where p is the P value            */
/* for the conic).                                                  */
/*                                                                  */
/* The rational curve defined by the Consurf polygon:               */
/*                                                                  */
/*               2  3                      -           -            */
/* P(t) = ( 1 t t  t  ) * M * ! P1 !      !  1  0  0  0 !           */
/*                            ! P2 !  M = ! -2  2  0  0 !           */
/*                            ! P3 !      !  1 -4  2  1 !           */
/*                            ! P4 !      !  0  2 -2  0 !           */
/*                                        !_           _!           */
/*                   <==>                                           */
/*                                                                  */
/* The Consurf polygon with blending functions F1, F2, F3 and F4:   */
/*                                                                  */
/* P(t) = ( F1(t) F2(t) F3(t) F4(t) ) * ! P1 !                      */
/*                                      ! P2 !                      */
/*                                      ! P3 !                      */
/* where                                ! P4 !                      */
/* F1(t) = 1 - 2*t +   t**2                                         */
/* F2(t) =     2*t - 4*t**2 + 2*t**3                                */
/* F3(t) =           2*t**2 - 2*t**3                                */
/* F4(t) =             t**2                                         */
/*                                                                  */
/*                                                                  */
/*                   <==>                                           */
/*                                                                  */
/* The rational curve defined by polynomial coefficients:           */
/*                                                                  */
/* P(t) = c0 + c1*t + c2*t**2 + c3*t**3                             */
/* where                                                            */
/* c0=    P1                       P1 =   c0                        */
/* c1= -2*P1 + 2*P2             or P2= (2*c0 + c1)/2                */
/* c2=    P1 - 4*P2 + 2*P3 + P4    P3= (2*c0 + c1 -         c3)/2   */
/* c3=         2*P2 - 2*P3         P4=    c0 + c1 +  c2 +   c3      */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/*                                                                  */
/* The rational cubic patch in algebraic form                       */
/* -------------------------------------------                      */
/*                                                                  */
/*                                                                  */
/* P(u,v) = U * A * V                                               */
/*                                                         ! 1  !   */
/*                                                         !    !   */
/*                              ! A00 A01 A02 A03 !        ! v  !   */
/*                 2  3         !                 !        !    !   */
/* P(u,v) = ( 1 u u  u  )   *   ! A10 A11 A12 A13 !   *    !  2 !   */
/*                              !                 !        ! v  !   */
/*                              ! A20 A21 A22 A23 !        !    !   */
/*                              !                 !        !  3 !   */
/*                              ! A30 A31 A32 A33 !        ! v  !   */
/*                                                                  */
/*                                                                  */
/* P(u,v)= (  1  *A00+  u  *A10+u**2 *A20+u**3 *A30) *   1   +      */
/*         (  1  *A01+  u  *A11+u**2 *A21+u**3 *A31) *   v   +      */
/*         (  1  *A02+  u  *A12+u**2 *A22+u**3 *A32) *  v**2 +      */
/*         (  1  *A03+  u  *A13+u**2  A23+u**3 *A33) *  v**3        */
/*                                                                  */
/*                                                                  */
/* The rational cubic patch defined by the Consurf polygon          */
/* -------------------------------------------------------          */
/*                       T                                          */
/* P(u,v) = U * M * B * M * V                                       */
/*                                                         ! 1  !   */
/*                                                         !    !   */
/*                              ! P00 P01 P02 P03 !        ! v  !   */
/*                 2  3         !                 !    T   !    !   */
/* P(u,v) = ( 1 u u  u  ) * M * ! P10 P11 P12 P13 ! * M  * !  2 !   */
/*                              !                 !        ! v  !   */
/*                              ! P20 P21 P22 P23 !        !    !   */
/* (M is defined above)         !                 !        !  3 !   */
/*                              ! P30 P31 P32 P33 !        ! v  !   */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                   ! P00 P01 P02 P03 !  ! F1(v) ! */
/*                                   !                 !  !       ! */
/* P(u,v)=(F1(u) F2(u) F3(u) F4(u)) *! P10 P11 P12 P13 !* ! F2(v) ! */
/*                                   !                 !  !       ! */
/*                                   ! P20 P21 P22 P23 !  ! F3(v) ! */
/*                                   !                 !  !       ! */
/*                                   ! P30 P31 P32 P33 !  ! F4(v) ! */
/*                                                                  */
/*                                                                  */
/* P(u,v)= (F1(u)*P00+F2(u)*P10+F3(u)*P20+F4(u)*P30) * F1(v) +      */
/*         (F1(u)*P01+F2(u)*P11+F3(u)*P21+F4(u)*P31) * F2(v) +      */
/*         (F1(u)*P02+F2(u)*P12+F3(u)*P22+F4(u)*P32) * F3(v) +      */
/*         (F1(u)*P03+F2(u)*P13+F3(u)*P23+F4(u)*P33) * F4(v)        */
/*                                                                  */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/* Create a rational ruled patch from 2 boundary curves             */
/* _____________________________________________________            */
/*                                                                  */
/* A ruled, rational surface patch is defined by:                   */
/*                                                                  */
/* r(u,v)= (1-v)*ru0(u) + v*ru1(u)                                  */
/*                                                                  */
/* where ru0 and ru1 are rational cubic curve segments.             */
/*                                                                  */
/* r(u,v)= (1-v)*Ru0(u)/wu0(u) + v*Ru1(u)/wu1(u)                    */
/*                                                                  */
/* In homogenous coordinates:                                       */
/* P(u,v)= (1-v)*Pu0(u) + v*Pu1(u)                                  */
/*                                                                  */
/* P(u,v)= (1-v)*(C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*           v  *(C0u1+C1u1*u+C2u1*u**2+C3u1*u**3)                  */
/*                                                                  */
/* P(u,v)=       (C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*          (-v)*(C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*           v  *(C0u1+C1u1*u+C2u1*u**2+C3u1*u**3)                  */
/*                                                                  */
/* P(u,v)=       (C0u0+C1u0*u+C2u0*u**2+C3u0*u**3)            *1 +  */
/* (C0u1-C0u0+(C1u1-C1u0)*u+(C2u1-C2u0)*u**2+(C3u1-C3u0)*u**3)*v    */
/*                                                                  */
/*                                                                  */
/* Identify coefficients in matrix A:                               */
/*                                                                  */
/* P(u,v) = U * A * V                                               */
/*                                                         ! 1  !   */
/*                                                         !    !   */
/*                            ! C0u0  C0u1-C0u0  0  0 !    ! v  !   */
/*                 2  3       !                       !    !    !   */
/* P(u,v) = ( 1 u u  u  )  *  ! C1u0  C1u1-C1u0  0  0 !    !  2 !   */
/*                            !                       !  * ! v  !   */
/*                            ! C2u0  C2u1-C2u0  0  0 !    !    !   */
/*                            !                       !    !  3 !   */
/*                            ! C3u0  C3u1-C3u0  0  0 !    ! v  !   */
/*                                                                  */
/*                                                                  */
/*        ! A00= C0u0  A01= C0u1-C0u0  A02= 0  A03= 0 !             */
/*        !                                           !             */
/*   A =  ! A10= C1u0  A11= C1u1-C1u0  A12= 0  A13= 0 !             */
/*        !                                           !             */
/*        ! A20= C2u0  A21= C0u2-C2u0  A22= 0  A23= 0 !             */
/*        !                                           !             */
/*        ! A30= C3u0  A31= C0u3-C3u0  A32= 0  A33= 0 !             */
/*                                                                  */
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
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur252 Enter *** varkon_pat_ratcre2 p_patr %d \n",
                p_patr );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  Check of input data.                        for DEBUG on.       */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_u0, p_u1, p_patr);
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_ratcre2 (sur252)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif


/*!                                                                 */
/* 2. Coefficients for the ruled rational patch                     */
/* ____________________________________________                     */
/*                                                                 !*/

/*!                                                                 */
/*  Coefficients for X                                              */
/*                                                                 !*/

    p_patr->a00x= (*p_u0).c0x;
    p_patr->a01x= (*p_u1).c0x - (*p_u0).c0x;
    p_patr->a02x= 0.0;        
    p_patr->a03x= 0.0;        

    p_patr->a10x= (*p_u0).c1x;
    p_patr->a11x= (*p_u1).c1x - (*p_u0).c1x;
    p_patr->a12x= 0.0;        
    p_patr->a13x= 0.0;        

    p_patr->a20x= (*p_u0).c2x;
    p_patr->a21x= (*p_u1).c2x - (*p_u0).c2x;
    p_patr->a22x= 0.0;        
    p_patr->a23x= 0.0;        

    p_patr->a30x= (*p_u0).c3x;
    p_patr->a31x= (*p_u1).c3x - (*p_u0).c3x;
    p_patr->a32x= 0.0;        
    p_patr->a33x= 0.0;        

/*!                                                                 */
/*  Coefficients for Y                                              */
/*                                                                 !*/

    p_patr->a00y= (*p_u0).c0y;
    p_patr->a01y= (*p_u1).c0y - (*p_u0).c0y;
    p_patr->a02y= 0.0;        
    p_patr->a03y= 0.0;        

    p_patr->a10y= (*p_u0).c1y;
    p_patr->a11y= (*p_u1).c1y - (*p_u0).c1y;
    p_patr->a12y= 0.0;        
    p_patr->a13y= 0.0;        

    p_patr->a20y= (*p_u0).c2y;
    p_patr->a21y= (*p_u1).c2y - (*p_u0).c2y;
    p_patr->a22y= 0.0;        
    p_patr->a23y= 0.0;        

    p_patr->a30y= (*p_u0).c3y;
    p_patr->a31y= (*p_u1).c3y - (*p_u0).c3y;
    p_patr->a32y= 0.0;        
    p_patr->a33y= 0.0;        

/*!                                                                 */
/*  Coefficients for Z                                              */
/*                                                                 !*/

    p_patr->a00z= (*p_u0).c0z;
    p_patr->a01z= (*p_u1).c0z - (*p_u0).c0z;
    p_patr->a02z= 0.0;        
    p_patr->a03z= 0.0;        

    p_patr->a10z= (*p_u0).c1z;
    p_patr->a11z= (*p_u1).c1z - (*p_u0).c1z;
    p_patr->a12z= 0.0;        
    p_patr->a13z= 0.0;        

    p_patr->a20z= (*p_u0).c2z;
    p_patr->a21z= (*p_u1).c2z - (*p_u0).c2z;
    p_patr->a22z= 0.0;        
    p_patr->a23z= 0.0;        

    p_patr->a30z= (*p_u0).c3z;
    p_patr->a31z= (*p_u1).c3z - (*p_u0).c3z;
    p_patr->a32z= 0.0;        
    p_patr->a33z= 0.0;        


/*!                                                                 */
/*  Coefficients for w                                              */
/*                                                                 !*/

    p_patr->a00 = (*p_u0).c0 ;
    p_patr->a01 = (*p_u1).c0  - (*p_u0).c0 ;
    p_patr->a02 = 0.0;        
    p_patr->a03 = 0.0;        

    p_patr->a10 = (*p_u0).c1 ;
    p_patr->a11 = (*p_u1).c1  - (*p_u0).c1 ;
    p_patr->a12 = 0.0;        
    p_patr->a13 = 0.0;        

    p_patr->a20 = (*p_u0).c2 ;
    p_patr->a21 = (*p_u1).c2  - (*p_u0).c2 ;
    p_patr->a22 = 0.0;        
    p_patr->a23 = 0.0;        

    p_patr->a30 = (*p_u0).c3 ;
    p_patr->a31 = (*p_u1).c3  - (*p_u0).c3 ;
    p_patr->a32 = 0.0;        
    p_patr->a33 = 0.0;        


/*!                                                                 */
/*  Let offset for the patch be zero                                */
/*                                                                 !*/
    p_patr->ofs_pat = 0.0;        

/*!                                                                 */
/*  Printout of patch data for Debug On                             */
/*  Call of varkon_pat_prirat (sur234).                             */
/*                                                                 !*/

#ifdef DEBUG
   status=varkon_pat_prirat (p_patr);
   if (status<0) 
   {
   sprintf(errbuf,"sur234%%varkon_pat_ratcre2 (sur252)");
   return(varkon_erpush("SU2943",errbuf));
   }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur252 Exit *** varkon_pat_ratcre2  \n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

#ifdef DEBUG
/*!********* Internal ** function **Only*for*Debug*On*(start)********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data.                              */

   static short initial (p_u0, p_u1, p_patr)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0;         /* Boundary segment U= 0.0           (ptr) */
   DBSeg  *p_u1;         /* Boundary segment U= 1.0           (ptr) */
   GMPATR *p_patr;       /* Rational cubic patch              (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u0sx,u0sy,u0sz;    /* Boundary U= 0   start point        */
   DBfloat u0ex,u0ey,u0ez;    /* Boundary U= 0   end   point        */
   DBfloat u1sx,u1sy,u1sz;    /* Boundary U= 1   start point        */
   DBfloat u1ex,u1ey,u1ez;    /* Boundary U= 1   end   point        */
/*-----------------------------------------------------------------!*/

   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors and      out[9-14]*/
                         /*        curvature added         out[ 15 ]*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur252 *** initial: p_u0 %d p_u1 %d d\n",
                       p_u0,   p_u1 );
  fflush(dbgfil(SURPAC));
  }

/*!                                                                 */
/* 2. Initialize output variables GMPATR and static variables       */
/*                                                                 !*/

/*  Coefficients for X                                              */
/*  -------------------                                             */
    p_patr->a00x= F_UNDEF;   
    p_patr->a01x= F_UNDEF;   
    p_patr->a02x= F_UNDEF;   
    p_patr->a03x= F_UNDEF;   

    p_patr->a10x= F_UNDEF;   
    p_patr->a11x= F_UNDEF;   
    p_patr->a12x= F_UNDEF;   
    p_patr->a13x= F_UNDEF;   

    p_patr->a20x= F_UNDEF;   
    p_patr->a21x= F_UNDEF;   
    p_patr->a22x= F_UNDEF;   
    p_patr->a23x= F_UNDEF;   

    p_patr->a30x= F_UNDEF;   
    p_patr->a31x= F_UNDEF;   
    p_patr->a32x= F_UNDEF;   
    p_patr->a33x= F_UNDEF;   

/*  Coefficients for Y                                              */
/*  -------------------                                             */
    p_patr->a00y= F_UNDEF;   
    p_patr->a01y= F_UNDEF;   
    p_patr->a02y= F_UNDEF;   
    p_patr->a03y= F_UNDEF;   

    p_patr->a10y= F_UNDEF;   
    p_patr->a11y= F_UNDEF;   
    p_patr->a12y= F_UNDEF;   
    p_patr->a13y= F_UNDEF;   

    p_patr->a20y= F_UNDEF;   
    p_patr->a21y= F_UNDEF;   
    p_patr->a22y= F_UNDEF;   
    p_patr->a23y= F_UNDEF;   

    p_patr->a30y= F_UNDEF;   
    p_patr->a31y= F_UNDEF;   
    p_patr->a32y= F_UNDEF;   
    p_patr->a33y= F_UNDEF;   


/*  Coefficients for Z                                              */
/*  -------------------                                             */
    p_patr->a00z= F_UNDEF;   
    p_patr->a01z= F_UNDEF;   
    p_patr->a02z= F_UNDEF;   
    p_patr->a03z= F_UNDEF;   

    p_patr->a10z= F_UNDEF;   
    p_patr->a11z= F_UNDEF;   
    p_patr->a12z= F_UNDEF;   
    p_patr->a13z= F_UNDEF;   

    p_patr->a20z= F_UNDEF;   
    p_patr->a21z= F_UNDEF;   
    p_patr->a22z= F_UNDEF;   
    p_patr->a23z= F_UNDEF;   

    p_patr->a30z= F_UNDEF;   
    p_patr->a31z= F_UNDEF;   
    p_patr->a32z= F_UNDEF;   
    p_patr->a33z= F_UNDEF;   

/*  Coefficients for w                                              */
/*  -------------------                                             */
    p_patr->a00 = F_UNDEF;   
    p_patr->a01 = F_UNDEF;   
    p_patr->a02 = F_UNDEF;   
    p_patr->a03 = F_UNDEF;   

    p_patr->a10 = F_UNDEF;   
    p_patr->a11 = F_UNDEF;   
    p_patr->a12 = F_UNDEF;   
    p_patr->a13 = F_UNDEF;   

    p_patr->a20 = F_UNDEF;   
    p_patr->a21 = F_UNDEF;   
    p_patr->a22 = F_UNDEF;   
    p_patr->a23 = F_UNDEF;   

    p_patr->a30 = F_UNDEF;   
    p_patr->a31 = F_UNDEF;   
    p_patr->a32 = F_UNDEF;   
    p_patr->a33 = F_UNDEF;   

/* 3. Initializations for the curve evaluation routine              */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 3. Check that the input curve net is closed                      */
/*                                                                  */
/*    Calculate end points of the boundary curves.                  */
/*    Calls of GE107.                                              */
/*                                                                 !*/

t_l = 0.0;
   status=GE107 (&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0sx = out[0]; 
u0sy = out[1]; 
u0sz = out[2]; 

t_l = 1.0;
   status=GE107 (&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0ex = out[0]; 
u0ey = out[1]; 
u0ez = out[2]; 

t_l = 0.0;
   status=GE107 (&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1sx = out[0]; 
u1sy = out[1]; 
u1sz = out[2]; 

t_l = 1.0;
   status=GE107 (&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1ex = out[0]; 
u1ey = out[1]; 
u1ez = out[2]; 


if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur252 Boundary u0 start point   %f %f %f \n",
        u0sx,   u0sy,   u0sz   );
fprintf(dbgfil(SURPAC),
"sur252 Boundary u0 end   point   %f %f %f \n",
        u0ex,   u0ey,   u0ez   );
fprintf(dbgfil(SURPAC),
"sur252 Boundary u1 end   point   %f %f %f \n",
        u1ex,   u1ey,   u1ez   );
fprintf(dbgfil(SURPAC),
"sur252 Boundary u1 start point   %f %f %f \n",
        u1sx,   u1sy,   u1sz   );
  fflush(dbgfil(SURPAC)); 
}

    return(SUCCED);

} /* End of function                                                */
/***********************************Only*for*Debug*On*(end)**********/
#endif

