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
/*                                                                  */
/*  Function: varkon_pat_ratcre1                   File: sur250.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a rational cubic patch.                    */
/*                                                                  */
/*  Input data is four boundary curves, which must be               */
/*  generalised conic segments (rational cubics) and                */
/*  one rational cubic which will become the u= 0.5                 */
/*  iso-parameter curve in the patch.                               */
/*                                                                  */
/*  Three of the input curves are conics, (i.e rational             */
/*  quadratics), which have been generated in a conic               */
/*  lofting patch.                                                  */
/*                                                                  */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-06   Originally written                                 */
/*  1996-05-28   Eliminate compiling warnings                       */
/*  1996-06-08   Missing % (not double) in sprintf                  */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_ratcre1    Create rational patch of 6 crv's */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
static DBfloat conpf1();      /* Consurf polygon blending fctn F1   */
static DBfloat conpf4();      /* Consurf polygon blending fctn F4   */
#endif
static short   ctoconp();     /* Coefficients to Consurf polygon    */
static DBfloat conpf2();      /* Consurf polygon blending fctn F2   */
static DBfloat conpf3();      /* Consurf polygon blending fctn F3   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
#ifdef DEBUG
static DBfloat b_conp[4][4];      /* Consurf polygon for patch (B)  */
#endif
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_invmat       * Invertation of a 4X4 matrix                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_ratcre1   */
/* SU2993 = Severe program error in varkon_pat_ratcre1 (sur250).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus varkon_pat_ratcre1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0,         /* Pointer to boundary segment U= 0.0      */
   DBSeg  *p_u1,         /* Pointer to boundary segment U= 1.0      */
   DBSeg  *p_v0,         /* Pointer to boundary segment V= 0.0      */
   DBSeg  *p_v1,         /* Pointer to boundary segment V= 1.0      */
   DBSeg  *p_um,         /* Pointer to middle   segment U= 0.5      */
   DBSeg  *p_vm,         /* Pointer to middle   segment V= 0.5      */
   GMPATR *p_patr )      /* Pointer to the rational cubic patch     */

/* Out:                                                             */
/*       Data to p_patr                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   HOMOP p00,p01,p02,p03;/* The Consurf Polygon for a surface patch */
   HOMOP p10,p13;        /*                                         */
   HOMOP p20,p23;        /*                                         */
   HOMOP p30,p31,p32,p33;/*                                         */
   HOMOP p1;             /* Start polygon point of curve segment    */
   HOMOP p4;             /* End   polygon point of curve segment    */
   DBTmat k;             /* Matrix for P11,P12,P21,P22 calculation  */
   DBTmat invk;          /* Inverted k matrix                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  deter;       /* Determinant of matrix k                 */
   DBfloat  mod_w;       /* Modification factor for weights         */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

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
/*                                                                  */
/* Create a rational patch from 4 boundary curves and 2 mid curves  */
/* ______________________________________________________________   */
/*                                                                  */
/* The u=0 boundary curve defines the polygon points:               */
/* P00, P01, P02 and P03.                                           */
/* The u=1 boundary curve defines the polygon points:               */
/* P30, P31, P32 and P33.                                           */
/* The v=0 boundary curve defines the polygon points:               */
/* P00, P10, P20 and P30.                                           */
/* The v=1 boundary curve defines the polygon points:               */
/* P03, P13, P23 and P33.                                           */
/*                                                                  */
/* The corner points must have the same weights but that            */
/* is no problem, since the homogenous coordinates can be           */
/* multiplied with any factor. The shape of the curve (or           */
/* the parametrization) will not be changed. The u=0 curve          */
/* will be the master. All boundaries are assumed to be             */
/* generalised conics with end point weights which are equal.       */
/*                                                                  */
/* The inner polygon points P11, P12, P21 and P22 will be           */
/* defined by the mid curves u= 0.5 and v= 0.5                      */
/*                                                                  */
/*                                                                  */
/* P(u,v)= (F1(u)*P00+F2(u)*P10+F3(u)*P20+F4(u)*P30) * F1(v) +      */
/*         (F1(u)*P01+F2(u)*P11+F3(u)*P21+F4(u)*P31) * F2(v) +      */
/*         (F1(u)*P02+F2(u)*P12+F3(u)*P22+F4(u)*P32) * F3(v) +      */
/*         (F1(u)*P03+F2(u)*P13+F3(u)*P23+F4(u)*P33) * F4(v)        */
/*                                                                  */
/* F2(u)*F2(v)P11+F2(u)*F3(v)*P12+F3(u)*F2(v)*P21+F3(u)*F3(v)*P22   */
/*                             =                                    */
/*                      P(u,v) -                                    */
/* -F1(u)*F1(v)*P00-F2(u)*F1(v)*P10-F3(u)*F1(v)*P20-F4(u)*F1(v)*P30 */
/* -F1(u)*F2(v)*P01-                                F4(u)*F2(v)*P31 */
/* -F1(u)*F3(v)*P02-                                F4(u)*F3(v)*P32 */
/* -F1(u)*F4(v)*P03-F2(u)*F4(v)*P13-F3(u)*F4(v)*P23-F4(u)*F4(v)*P33 */
/*                                                                  */
/*  Four points P(0.25,0.25), P(0.5,0.25), P(0.5,0.5), P(0.5,0.75)  */
/*  will define the polygon points P11, P12, P21 and P22.           */
/*                                                                  */
/*                  <===>                                           */
/*                                                                  */
/*  k11*P11 + k12*P12 + k13*P21 + k14*P22 = l1                      */
/*  k21*P11 + k22*P12 + k23*P21 + k24*P22 = l2                      */
/*  k31*P11 + k32*P12 + k33*P21 + k34*P22 = l3                      */
/*  k41*P11 + k42*P12 + k43*P21 + k44*P22 = l4                      */
/*                                                                  */
/*  where                                                           */
/*  k11=F2(.25)*F2(.25)=0....   k21=F2(.50)*F2(.25)=0....           */
/*  k12=F2(.25)*F3(.25)=0....   k22=F2(.50)*F3(.25)=0....           */
/*  k13=F3(.25)*F2(.25)=0....   k23=F3(.50)*F2(.25)=0....           */
/*  k14=F3(.25)*F3(.25)=0....   k24=F3(.50)*F3(.25)=0....           */
/*                                                                  */
/*  k31=F2(.50)*F2(.50)=0....   k41=F2(.50)*F2(.75)=0....           */
/*  k32=F2(.50)*F3(.50)=0....   k42=F2(.50)*F3(.75)=0....           */
/*  k33=F3(.50)*F2(.50)=0....   k43=F3(.50)*F2(.75)=0....           */
/*  k34=F3(.50)*F3(.50)=0....   k44=F3(.50)*F3(.75)=0....           */
/*                                                                  */
/*           !!!  {nnu inte {ndrat ..... !!!!                       */
/*  l1 =            P(0.5,0.2) -                                    */
/* -F1(5)*F1(2)*P00-F2(5)*F1(2)*P10-F3(5)*F1(2)*P20-F4(5)*F1(2)*P30 */
/* -F1(5)*F2(2)*P01-                                F4(5)*F2(2)*P31 */
/* -F1(5)*F3(2)*P02-                                F4(5)*F3(2)*P32 */
/* -F1(5)*F4(2)*P03-F2(5)*F4(2)*P13-F3(5)*F4(2)*P23-F4(5)*F4(2)*P33 */
/*                                                                  */
/*  l2 =            P(0.5,0.4) -                                    */
/* -F1(5)*F1(4)*P00-F2(5)*F1(4)*P10-F3(5)*F1(4)*P20-F4(5)*F1(4)*P30 */
/* -F1(5)*F2(4)*P01-                                F4(5)*F2(4)*P31 */
/* -F1(5)*F3(4)*P02-                                F4(5)*F3(4)*P32 */
/* -F1(5)*F4(4)*P03-F2(5)*F4(4)*P13-F3(5)*F4(4)*P23-F4(5)*F4(4)*P33 */
/*                                                                  */
/*  l3 =            P(0.5,0.6) -                                    */
/* -F1(5)*F1(6)*P00-F2(5)*F1(6)*P10-F3(5)*F1(6)*P20-F4(5)*F1(6)*P30 */
/* -F1(5)*F2(6)*P01-                                F4(5)*F2(6)*P31 */
/* -F1(5)*F3(6)*P02-                                F4(5)*F3(6)*P32 */
/* -F1(5)*F4(6)*P03-F2(5)*F4(6)*P13-F3(5)*F4(6)*P23-F4(5)*F4(6)*P33 */
/*                                                                  */
/*  l4 =            P(0.5,0.8) -                                    */
/* -F1(5)*F1(8)*P00-F2(5)*F1(8)*P10-F3(5)*F1(8)*P20-F4(5)*F1(8)*P30 */
/* -F1(5)*F2(8)*P01-                                F4(5)*F2(8)*P31 */
/* -F1(5)*F3(8)*P02-                                F4(5)*F3(8)*P32 */
/* -F1(5)*F4(8)*P03-F2(5)*F4(8)*P13-F3(5)*F4(8)*P23-F4(5)*F4(8)*P33 */
/*                                                                  */
/*                                                                  */
/* F1(u=0.5) = 1 - 2*0.5 +   0.5**2            = 0.25               */
/* F2(u=0.5) =     2*0.5 - 4*0.5**2 + 2*0.5**3 = 0.25               */
/* F3(u=0.5) =             2*0.5**2 - 2*0.5**3 = 0.25               */
/* F4(u=0.5) =               0.5**2            = 0.25               */
/*                                                                  */
/* F1(v=0.2) = 1 - 2*0.2 +   0.2**2            = 0.640              */
/* F2(v=0.2) =     2*0.2 - 4*0.2**2 + 2*0.2**3 = 0.256              */
/* F3(v=0.2) =             2*0.2**2 - 2*0.2**3 = 0.064              */
/* F4(v=0.2) =               0.2**2            = 0.040              */
/*                                                                  */
/* F1(v=0.4) = 1 - 2*0.4 +   0.4**2            = 0.960              */
/* F2(v=0.4) =     2*0.4 - 4*0.4**2 + 2*0.4**3 = 0.288              */
/* F3(v=0.4) =             2*0.4**2 - 2*0.4**3 = 0.192              */
/* F4(v=0.4) =               0.4**2            = 0.160              */
/*                                                                  */
/* F1(v=0.6) = 1 - 2*0.6 +   0.6**2            = 0.                 */
/* F2(v=0.6) =     2*0.6 - 4*0.6**2 + 2*0.6**3 = 0.192              */
/* F3(v=0.6) =             2*0.6**2 - 2*0.6**3 = 0.288              */
/* F4(v=0.6) =               0.6**2            = 0.360              */
/*                                                                  */
/*                                                                  */
/* F1(v=0.8) = 1 - 2*0.8 +   0.8**2            =                    */
/* F2(v=0.8) =     2*0.8 - 4*0.8**2 + 2*0.8**3 = 0.064              */
/* F3(v=0.8) =             2*0.8**2 - 2*0.8**3 = 0.256              */
/* F4(v=0.8) =               0.8**2            =                    */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur250 Enter *** varkon_pat_ratcre1 p_patr %d \n",
                p_patr );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */

/*  Check of input data.                        for DEBUG on.       */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_u0, p_u1, p_v0, p_v1, p_um, p_patr);
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif


/*!                                                                 */
/* 2. Consurf polygon for the rational patch                        */
/* _________________________________________                        */

/* Calculate the Consurf polygons for the boundary segments.        */
/* Call of ctoconp for each segment      Error SU2973 for failure.  */
/* Boundary u=0 to P00, P01, P02, P03                               */
/* Boundary u=1 to P30, P31, P32, P33 with modified weights.        */
/* Boundary v=0 to P1,  P10, P20, P4  with modified weights.        */
/* Boundary v=1 to P1 , P13, P23, P4  with modified weights.        */
/*                                                                 !*/
   status=ctoconp(p_u0 , &p00, &p01, &p02, &p03 );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(u0)%%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=ctoconp(p_u1 , &p30, &p31, &p32, &p33 );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(u1)%%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2973",errbuf));
   }

/* Note that weights are checked in ctoconp for Debug On            */

   mod_w = p00.w/p30.w;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur250 P00.w %f P30.w %f P00.w/P30.w %f mod_w %f\n",
  p00.w, p30.w, p00.w/p30.w, mod_w );
}
#endif

   p30.X = mod_w*p30.X;
   p30.Y = mod_w*p30.Y;
   p30.Z = mod_w*p30.Z;
   p30.w = mod_w*p30.w;

   p31.X = mod_w*p31.X;
   p31.Y = mod_w*p31.Y;
   p31.Z = mod_w*p31.Z;
   p31.w = mod_w*p31.w;

   p32.X = mod_w*p32.X;
   p32.Y = mod_w*p32.Y;
   p32.Z = mod_w*p32.Z;
   p32.w = mod_w*p32.w;

   p33.X = mod_w*p33.X;
   p33.Y = mod_w*p33.Y;
   p33.Z = mod_w*p33.Z;
   p33.w = mod_w*p33.w;

/* Boundary v=0 to  P1, P10, P20, P4    with modified weights.      */
   status=ctoconp(p_v0 , &p1 , &p10, &p20, &p4  );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(v0)%%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2973",errbuf));
   }
/* Note that weights are checked in ctoconp for Debug On            */

   mod_w = p00.w/p1.w;
 
   p10.X = mod_w*p10.X;
   p10.Y = mod_w*p10.Y;
   p10.Z = mod_w*p10.Z;
   p10.w = mod_w*p10.w;

   p20.X = mod_w*p20.X;
   p20.Y = mod_w*p20.Y;
   p20.Z = mod_w*p20.Z;
   p20.w = mod_w*p20.w;

/* Boundary v=1 to P1 , P13, P23, P4   with modified weights.       */
   status=ctoconp(p_v1 , &p1 , &p13, &p23, &p4  );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(v1)%%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2973",errbuf));
   }

/* Note that weights are checked in ctoconp for Debug On            */

   mod_w = p00.w/p1.w;
 
   p13.X = mod_w*p13.X;
   p13.Y = mod_w*p13.Y;
   p13.Z = mod_w*p13.Z;
   p13.w = mod_w*p13.w;

   p23.X = mod_w*p23.X;
   p23.Y = mod_w*p23.Y;
   p23.Z = mod_w*p23.Z;
   p23.w = mod_w*p23.w;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur250 P00 %f %f %f %f \n",
  p00.X/p00.w, p00.Y/p00.w, p00.Z/p00.w, p00.w);
fprintf(dbgfil(SURPAC),
"sur250 P01 %f %f %f %f \n",
  p01.X/p01.w, p01.Y/p01.w, p01.Z/p01.w, p01.w);
fprintf(dbgfil(SURPAC),
"sur250 P02 %f %f %f %f \n",
  p02.X/p02.w, p02.Y/p02.w, p02.Z/p02.w, p02.w);
fprintf(dbgfil(SURPAC),
"sur250 P03 %f %f %f %f \n",
  p03.X/p03.w, p03.Y/p03.w, p03.Z/p03.w, p03.w);

fprintf(dbgfil(SURPAC),
"sur250 P30 %f %f %f %f \n",
  p30.X/p30.w, p30.Y/p30.w, p30.Z/p30.w, p30.w);
fprintf(dbgfil(SURPAC),
"sur250 P31 %f %f %f %f \n",
  p31.X/p31.w, p31.Y/p31.w, p31.Z/p31.w, p31.w);
fprintf(dbgfil(SURPAC),
"sur250 P32 %f %f %f %f \n",
  p32.X/p32.w, p32.Y/p32.w, p32.Z/p32.w, p32.w);
fprintf(dbgfil(SURPAC),
"sur250 P33 %f %f %f %f \n",
  p33.X/p33.w, p33.Y/p33.w, p33.Z/p33.w, p33.w);

fprintf(dbgfil(SURPAC),
"sur250 P10 %f %f %f %f \n",
  p10.X/p10.w, p10.Y/p10.w, p10.Z/p10.w, p10.w);
fprintf(dbgfil(SURPAC),
"sur250 P20 %f %f %f %f \n",
  p20.X/p20.w, p20.Y/p20.w, p20.Z/p20.w, p20.w);

fprintf(dbgfil(SURPAC),
"sur250 P13 %f %f %f %f \n",
  p13.X/p13.w, p13.Y/p13.w, p13.Z/p13.w, p13.w);
fprintf(dbgfil(SURPAC),
"sur250 P23 %f %f %f %f \n",
  p23.X/p23.w, p23.Y/p23.w, p23.Z/p23.w, p23.w);

fprintf(dbgfil(SURPAC),
"sur250 P00+P10+P20+P30= %f %f %f\n",
  (p00.X+p10.X+p20.X+p30.X)/(p00.w+p10.w+p20.w+p30.w), 
  (p00.Y+p10.Y+p20.Y+p30.Y)/(p00.w+p10.w+p20.w+p30.w), 
  (p00.Z+p10.Z+p20.Z+p30.Z)/(p00.w+p10.w+p20.w+p30.w));
fprintf(dbgfil(SURPAC),
"sur250 P03+P13+P23+P33= %f %f %f\n",
  (p03.X+p13.X+p23.X+p33.X)/(p03.w+p13.w+p23.w+p33.w), 
  (p03.Y+p13.Y+p23.Y+p33.Y)/(p03.w+p13.w+p23.w+p33.w), 
  (p03.Z+p13.Z+p23.Z+p33.Z)/(p03.w+p13.w+p23.w+p33.w));

fprintf(dbgfil(SURPAC),
"sur250 P00+P01+P02+P03= %f %f %f\n",
  (p00.X+p01.X+p02.X+p03.X)/(p00.w+p01.w+p02.w+p03.w), 
  (p00.Y+p01.Y+p02.Y+p03.Y)/(p00.w+p01.w+p02.w+p03.w), 
  (p00.Z+p01.Z+p02.Z+p03.Z)/(p00.w+p01.w+p02.w+p03.w));
fprintf(dbgfil(SURPAC),
"sur250 P30+P31+P32+P33= %f %f %f\n",
  (p30.X+p31.X+p32.X+p33.X)/(p30.w+p31.w+p32.w+p33.w), 
  (p30.Y+p31.Y+p32.Y+p33.Y)/(p30.w+p31.w+p32.w+p33.w), 
  (p30.Z+p31.Z+p32.Z+p33.Z)/(p30.w+p31.w+p32.w+p33.w));

  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 3. Calculate Consurf Polygon points P11,P12,P21,P22              */
/* ___________________________________________________              */

/* The matrix k= k11, ...                                           */
/* The inverted K matrix invk (always the same ...... )             */
/*   !!! n{r funktionen fungerar ska matrisen h}rdprogrammeras !!!! */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur250 conpf1(.5) %f ..f2(.5) %f ..pf3(.5) %f ..f4(.5) %f\n",
  conpf1(0.5), conpf2(0.5) , conpf3(0.5) , conpf4(0.5) );
fprintf(dbgfil(SURPAC),
"sur250 conpf2(0.2) %f (0.4) %f (0.6) %f (0.8) %f \n",
  conpf2(0.2),conpf2(0.4),conpf2(0.6),conpf2(0.8));
fprintf(dbgfil(SURPAC),
"sur250 conpf3(0.2) %f (0.4) %f (0.6) %f (0.8) %f \n",
  conpf3(0.2),conpf3(0.4),conpf3(0.6),conpf3(0.8));
}
#endif

   k.g11=conpf2(0.25)*conpf2(0.25);
   k.g12=conpf2(0.25)*conpf3(0.25);
   k.g13=conpf3(0.25)*conpf2(0.25);
   k.g14=conpf3(0.25)*conpf3(0.25);

   k.g21=conpf2(0.30)*conpf2(0.25);
   k.g22=conpf2(0.30)*conpf3(0.25);
   k.g23=conpf3(0.30)*conpf2(0.25);
   k.g24=conpf3(0.30)*conpf3(0.25);

   k.g31=conpf2(0.75)*conpf2(0.50);
   k.g32=conpf2(0.75)*conpf3(0.50);
   k.g33=conpf3(0.75)*conpf2(0.50);
   k.g34=conpf3(0.75)*conpf3(0.50);

   k.g41=conpf2(0.50)*conpf2(0.95);
   k.g42=conpf2(0.50)*conpf3(0.95);
   k.g43=conpf3(0.50)*conpf2(0.95);
   k.g44=conpf3(0.50)*conpf3(0.95);

   k.g11=1.0;
   k.g12=0.0;
   k.g13=1.0;
   k.g14=0.0;

   k.g21=0.0;
   k.g22=1.0;
   k.g23=0.0;
   k.g24=1.0;

   k.g31=1.0;
   k.g32=1.0;
   k.g33=0.0;
   k.g34=0.0;

   k.g41=0.0;
   k.g42=0.0;
   k.g43=1.0;
   k.g44=1.0;

   status=varkon_invmat( &k , &invk , &deter );
   if (status<0) 
        {
        sprintf(errbuf,"sur660%%varkon_pat_ratcre1 (sur250)");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur250 Exit *** varkon_pat_ratcre1  \n"
   );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef DEBUG
/*!********* Internal ** function **Only*for*Debug*On*(start)********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data.                              */

   static short initial (p_u0, p_u1, p_v0, p_v1, p_um, p_patr)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0;         /* Pointer to boundary segment U= 0.0      */
   DBSeg  *p_u1;         /* Pointer to boundary segment U= 1.0      */
   DBSeg  *p_v0;         /* Pointer to boundary segment V= 0.0      */
   DBSeg  *p_u1;         /* Pointer to boundary segment V= 1.0      */
   DBSeg  *p_um;         /* Pointer to middle   segment U= 0.5      */
   GMPATR *p_patr;       /* Pointer to the rational cubic patch     */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u0sx,u0sy,u0sz;    /* Boundary U= 0   start point        */
   DBfloat u0ex,u0ey,u0ez;    /* Boundary U= 0   end   point        */
   DBfloat u1sx,u1sy,u1sz;    /* Boundary U= 1   start point        */
   DBfloat u1ex,u1ey,u1ez;    /* Boundary U= 1   end   point        */
   DBfloat v0sx,v0sy,v0sz;    /* Boundary V= 0   start point        */
   DBfloat v0ex,v0ey,v0ez;    /* Boundary V= 0   end   point        */
   DBfloat v1sx,v1sy,v1sz;    /* Boundary V= 1   start point        */
   DBfloat v1ex,v1ey,v1ez;    /* Boundary V= 1   end   point        */
   DBfloat u05sx,u05sy,u05sz; /* Mid crv  U= 0.5 start point        */
   DBfloat u05ex,u05ey,u05ez; /* Mid crv  U= 0.5 end   point        */
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
   short  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur250 *** initial: p_u0 %d p_u1 %d p_v0 %d p_v1 %d p_um %d\n",
                       p_u0,   p_u1,   p_v0,   p_v1,   p_um);
  fflush(dbgfil(SURPAC)); 
  }

/*!                                                                 */
/* 2. Initialize output variables GMPATC and static variables       */
/*                                                                 !*/

/*  Coefficients for X                                              */
/*  -------------------                                             */
    p_patr->a00x= 1.23456789;
    p_patr->a01x= 1.23456789;
    p_patr->a02x= 1.23456789;
    p_patr->a03x= 1.23456789;

    p_patr->a10x= 1.23456789;
    p_patr->a11x= 1.23456789;
    p_patr->a12x= 1.23456789;
    p_patr->a13x= 1.23456789;

    p_patr->a20x= 1.23456789;
    p_patr->a21x= 1.23456789;
    p_patr->a22x= 1.23456789;
    p_patr->a23x= 1.23456789;

    p_patr->a30x= 1.23456789;
    p_patr->a31x= 1.23456789;
    p_patr->a32x= 1.23456789;
    p_patr->a33x= 1.23456789;

/*  Coefficients for Y                                              */
/*  -------------------                                             */
    p_patr->a00y= 1.23456789;
    p_patr->a01y= 1.23456789;
    p_patr->a02y= 1.23456789;
    p_patr->a03y= 1.23456789;

    p_patr->a10y= 1.23456789;
    p_patr->a11y= 1.23456789;
    p_patr->a12y= 1.23456789;
    p_patr->a13y= 1.23456789;

    p_patr->a20y= 1.23456789;
    p_patr->a21y= 1.23456789;
    p_patr->a22y= 1.23456789;
    p_patr->a23y= 1.23456789;

    p_patr->a30y= 1.23456789;
    p_patr->a31y= 1.23456789;
    p_patr->a32y= 1.23456789;
    p_patr->a33y= 1.23456789;


/*  Coefficients for Z                                              */
/*  -------------------                                             */
    p_patr->a00z= 1.23456789;
    p_patr->a01z= 1.23456789;
    p_patr->a02z= 1.23456789;
    p_patr->a03z= 1.23456789;

    p_patr->a10z= 1.23456789;
    p_patr->a11z= 1.23456789;
    p_patr->a12z= 1.23456789;
    p_patr->a13z= 1.23456789;

    p_patr->a20z= 1.23456789;
    p_patr->a21z= 1.23456789;
    p_patr->a22z= 1.23456789;
    p_patr->a23z= 1.23456789;

    p_patr->a30z= 1.23456789;
    p_patr->a31z= 1.23456789;
    p_patr->a32z= 1.23456789;
    p_patr->a33z= 1.23456789;

/*  Coefficients for w                                              */
/*  -------------------                                             */
    p_patr->a00 = 1.23456789;
    p_patr->a01 = 1.23456789;
    p_patr->a02 = 1.23456789;
    p_patr->a03 = 1.23456789;

    p_patr->a10 = 1.23456789;
    p_patr->a11 = 1.23456789;
    p_patr->a12 = 1.23456789;
    p_patr->a13 = 1.23456789;

    p_patr->a20 = 1.23456789;
    p_patr->a21 = 1.23456789;
    p_patr->a22 = 1.23456789;
    p_patr->a23 = 1.23456789;

    p_patr->a30 = 1.23456789;
    p_patr->a31 = 1.23456789;
    p_patr->a32 = 1.23456789;
    p_patr->a33 = 1.23456789;

/* 3. Initiations for the curve evaluation routine                  */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Check that the input curve net is closed                      */
/*                                                                  */
/*    Calculate end points of the boundary curves.                  */
/*    Calls of GE107.                                              */
/*                                                                 !*/

t_l = 0.0;
   status=GE107(&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0sx = out[0]; 
u0sy = out[1]; 
u0sz = out[2]; 

t_l = 1.0;
   status=GE107(&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0ex = out[0]; 
u0ey = out[1]; 
u0ez = out[2]; 

t_l = 0.0;
   status=GE107(&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1sx = out[0]; 
u1sy = out[1]; 
u1sz = out[2]; 

t_l = 1.0;
   status=GE107(&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1ex = out[0]; 
u1ey = out[1]; 
u1ez = out[2]; 

t_l = 0.0;
   status=GE107(&scur,p_v0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

v0sx = out[0]; 
v0sy = out[1]; 
v0sz = out[2]; 

t_l = 1.0;
   status=GE107(&scur,p_v0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

v0ex = out[0]; 
v0ey = out[1]; 
v0ez = out[2]; 

t_l = 0.0;
   status=GE107(&scur,p_v1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

v1sx = out[0]; 
v1sy = out[1]; 
v1sz = out[2]; 

t_l = 1.0;
   status=GE107(&scur,p_v1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_ratcre1 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

v1ex = out[0]; 
v1ey = out[1]; 
v1ez = out[2]; 



if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur250 Boundary u0 start point   %f %f %f \n",
        u0sx,   u0sy,   u0sz   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary u0 end   point   %f %f %f \n",
        u0ex,   u0ey,   u0ez   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary v1 start point   %f %f %f \n",
        v1sx,   v1sy,   v1sz   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary v1 end   point   %f %f %f \n",
        v1ex,   v1ey,   v1ez   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary u1 end   point   %f %f %f \n",
        u1ex,   u1ey,   u1ez   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary u1 start point   %f %f %f \n",
        u1sx,   u1sy,   u1sz   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary v0 end   point   %f %f %f \n",
        v0ex,   v0ey,   v0ez   );
fprintf(dbgfil(SURPAC),
"sur250 Boundary v0 start point   %f %f %f \n",
        v0sx,   v0sy,   v0sz   );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

    return(SUCCED);

} /* End of function                                                */
/*!*********************************Only*for*Debug*On*(end)**********/
#endif

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function converts the coefficients of a rational cubic       */
/* segment to the Consur polygon.                                   */

   static short ctoconp (p_seg, p1, p2, p3, p4 )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_seg;        /* Pointer to a rational cubic segment     */

/* Out:                                                             */
                         /* The Consurf Polygon for a curve segment */
   HOMOP *p1;            /* P1 = (X1w1,Y1w1,Z1w1,w1)                */
   HOMOP *p2;            /* P2 = (X1w2,Y1w2,Z1w2,w2)                */
   HOMOP *p3;            /* P3 = (X1w3,Y1w3,Z1w3,w3)                */
   HOMOP *p4;            /* P4 = (X4w4,Y4w4,Z4w4,w4)                */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

#ifdef  DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur250 *** ctoconp: p_seg %d \n",
                       p_seg );
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 2. Convert from coefficients to polygon                          */
/* _______________________________________                          */
/*                                                                 !*/

/*! Point P1 =        c0                                           !*/
/*  --------                                                        */

    p1->X    = p_seg->c0x;
    p1->Y    = p_seg->c0y;
    p1->Z    = p_seg->c0z;
    p1->w    = p_seg->c0;

/*! Point P2 = (       c1  +        2 * c0 )/ 2                    !*/
/*  --------                                                        */

    p2->X    = (p_seg->c1x + 2.0*p_seg->c0x)/2.0;
    p2->Y    = (p_seg->c1y + 2.0*p_seg->c0y)/2.0;
    p2->Z    = (p_seg->c1z + 2.0*p_seg->c0z)/2.0;
    p2->w    = (p_seg->c1  + 2.0*p_seg->c0 )/2.0;

/*! Point P3 = (       c1  +        2 * c0 -       c3 )/ 2         !*/
/*  --------                                                        */

    p3->X    = (p_seg->c1x + 2.0*p_seg->c0x-p_seg->c3x)/2.0;
    p3->Y    = (p_seg->c1y + 2.0*p_seg->c0y-p_seg->c3y)/2.0;
    p3->Z    = (p_seg->c1z + 2.0*p_seg->c0z-p_seg->c3z)/2.0;
    p3->w    = (p_seg->c1  + 2.0*p_seg->c0 -p_seg->c3 )/2.0;

/*! Point P4 =        c3  +        c2  +        c1  +        c0    !*/
/*  --------                                                        */

    p4->X    = p_seg->c3x + p_seg->c2x + p_seg->c1x + p_seg->c0x;
    p4->Y    = p_seg->c3y + p_seg->c2y + p_seg->c1y + p_seg->c0y;
    p4->Z    = p_seg->c3z + p_seg->c2z + p_seg->c1z + p_seg->c0z;
    p4->w    = p_seg->c3  + p_seg->c2  + p_seg->c1  + p_seg->c0;

/*!                                                                 */
/* 3. Check weights for Debug On                                    */
/* _____________________________                                    */
/*                                                                 !*/

#ifdef DEBUG
   if (p1->w < 0.000001 )
   {
   sprintf(errbuf,"p1.w < 0 %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p2->w < 0.000001 )
   {
   sprintf(errbuf,"p2.w < 0 %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p3->w < 0.000001 )
   {
   sprintf(errbuf,"p3.w < 0 %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p4->w < 0.000001 )
   {
   sprintf(errbuf,"p4.w < 0 %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (fabs(p1->w - p4->w) > 0.000001 )
   {
   sprintf(errbuf,"p1.w ne p4.w %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (fabs(p2->w - p3->w) > 0.000001 )
   {
   sprintf(errbuf,"p2.w ne p3.w %%varkon_pat_ratcre1 (sur250)");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur250*ctoconp P1 = %f %f %f %f \n",
  p1->X/p1->w, p1->Y/p1->w, p1->Z/p1->w, p1->w);
fprintf(dbgfil(SURPAC),
"sur250*ctoconp P2 = %f %f %f %f \n",
  p2->X/p2->w, p2->Y/p2->w, p2->Z/p2->w, p2->w);
fprintf(dbgfil(SURPAC),
"sur250*ctoconp P3 = %f %f %f %f \n",
  p3->X/p3->w, p3->Y/p3->w, p3->Z/p3->w, p3->w);
fprintf(dbgfil(SURPAC),
"sur250*ctoconp P4 = %f %f %f %f \n",
  p4->X/p4->w, p4->Y/p4->w, p4->Z/p4->w, p4->w);
  fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

#ifdef  DEBUG
/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the value of Consurf polygon blending    */
/* function F1 for a given parameter value.                         */

   static DBfloat conpf1 (t_param )
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat   t_param;    /* Parameter value                         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate F1(t)= 1 - 2*t + t**2                               */
/*                                                                 !*/

    return(1.0-2.0*t_param+t_param*t_param);

} /* End of function                                                */
/********************************************************************/
#endif  /* DEBUG  */

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the value of Consurf polygon blending    */
/* function F2 for a given parameter value.                         */

   static DBfloat conpf2 (t_param )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat   t_param;    /* Parameter value                         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate F2(t)=     2*t - 4*t**2 + 2*t**3                    */
/*                                                                 !*/

return(2*t_param-4.0*t_param*t_param+2.0*t_param*t_param*t_param);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the value of Consurf polygon blending    */
/* function F3 for a given parameter value.                         */

   static DBfloat conpf3 (t_param )
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat   t_param;    /* Parameter value                         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/
/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate F3(t)= 2*t**2 - 2*t**3                              */
/*                                                                 !*/

    return(2.0*t_param*t_param-2.0*t_param*t_param*t_param);

} /* End of function                                                */
/********************************************************************/

#ifdef  DEBUG
/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the value of Consurf polygon blending    */
/* function F4 for a given parameter value.                         */

   static DBfloat conpf4 (t_param )
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat   t_param;    /* Parameter value                         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate F4(t)= t**2                                         */
/*                                                                 !*/

    return(t_param*t_param);

} /* End of function                                                */
/********************************************************************/
#endif  /* DEBUG  */
