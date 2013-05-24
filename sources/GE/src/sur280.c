/********************************************************************/
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
/*!                                                                 */
/*  Function: varkon_pat_bicbez                    File: sur280.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Convert a bicubic patch to a Bezier (rational) cubic patch.     */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-15   Originally written                                 */
/*  1997-11-08   Denominator zero for some points                   */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_bicbez     Bicubic to Bezier patch          */
/*                                                              */
/*--------------------------------------------------------------*/

/*!                                                                !*/
/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* References:                                                      */
/* 1. Faux & Pratt, pages                                           */
/*                                                                  */
/* The bicubic patch in algebraic form                              */
/* ------------------------------------                             */
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
/* The bicubic patch defined by Bezier points                       */
/* ------------------------------------------                       */
/*                       T                                          */
/* P(u,v) = U * M * B * M * V                                       */
/*                                                         ! 1  !   */
/*                                                         !    !   */
/*                              ! R00 R01 R02 R03 !        ! v  !   */
/*                 2  3         !                 !    T   !    !   */
/* P(u,v) = ( 1 u u  u  ) * M * ! R10 R11 R12 R13 ! * M  * !  2 !   */
/*                              !                 !        ! v  !   */
/*                              ! R20 R21 R22 R23 !        !    !   */
/* (M is defined above)         !                 !        !  3 !   */
/*                              ! R30 R31 R32 R33 !        ! v  !   */
/*                                                                  */
/*                                                                  */
/* Relation between algebraic and Bezier coefficients               */
/* ___________________________________________________              */
/*                                                                  */
/*                                                                  */
/*                 T                                                */
/* A  =  M * B * M                                                  */
/*                                                                  */
/*                                                                  */
/*        -1           -1                                           */
/*                    T                                             */
/* B  =  M  *  A  *  M                                              */
/*                                                                  */
/*                                                                  */
/*       !   1   0   0   0   !          !   1   0   0   0  !        */
/*       !                   !     -1   !                  !        */
/*       !  -3   3   0   0   !          !   1  1/3  0   0  !        */
/* M  =  !                   !   M   =  !                  !        */
/*       !   3  -6   3   0   !          !   1  2/3 1/3  0  !        */
/*       !                   !          !                  !        */
/*       !  -1   3  -3   1   !          !   1   1   1   1  !        */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*       !   1  -3   3  -1   !          !   1   1   1   1  !        */
/*       !                   !     -1   !                  !        */
/*  T    !   0   3  -6   3   !     T    !   0  1/3 2/3  1  !        */
/* M  =  !                   !   M   =  !                  !        */
/*       !   0   0   3  -3   !          !   0   0  1/3  1  !        */
/*       !                   !          !                  !        */
/*       !   0   0   0   1   !          !   0   0   0   1  !        */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmpatbr3    * Initialize Bezier patch                 */
/* varkon_tensmult_l      * Tensor T(4X4X4)=G(4X4)*A(4X4X4)         */
/* varkon_tensmult_r      * Tensor T(4X4X4)=A(4X4X4)*G(4X4)         */
/* varkon_erpush          * Error message to error buffer           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxx failed in varkon_pat_bicbez       */
/* SU2993 = Severe program error ( ) in varkon_pat_bicbez (sur280)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_bicbez (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATC   *p_i,         /* Bicubic patch                     (ptr) */
  GMPATBR3 *p_o )        /* Bezier bicubic rational patch     (ptr) */

/* Out:                                                             */
/*        Coefficients of the Bezier patch                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  GMPATR  qmat;          /* Tensor with input points/derivatives    */
  DBTmat  cmat;          /* Blending coefficients matrix            */
  DBTmat  ctmat;         /* Blending coefficients matrix transpon.  */
  GMPATR  cqmat;         /* Tensor cmat*qmat                        */
  GMPATR  cqctmat;       /* Tensor cmat*qmat*ctmat                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   short  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur280 Enter*varkon_pat_bicbez**\n");
  }
#endif
 
/*!                                                                 */
/* Initialization of Bezier patch data                              */
/*                                                                 !*/

    varkon_ini_gmpatbr3 (  p_o ); 

  
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a00x,p_i->a01x,p_i->a02x,p_i->a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a10x,p_i->a11x,p_i->a12x,p_i->a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a20x,p_i->a21x,p_i->a22x,p_i->a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a30x,p_i->a31x,p_i->a32x,p_i->a33x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a00y,p_i->a01y,p_i->a02y,p_i->a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a10y,p_i->a11y,p_i->a12y,p_i->a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a20y,p_i->a21y,p_i->a22y,p_i->a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a30y,p_i->a31y,p_i->a32y,p_i->a33y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a00z,p_i->a01z,p_i->a02z,p_i->a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a10z,p_i->a11z,p_i->a12z,p_i->a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a20z,p_i->a21z,p_i->a22z,p_i->a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_i->a30z,p_i->a31z,p_i->a32z,p_i->a33z);

  }
#endif


/*!                                                                 */
/* 2. Calculate Bezier polygon                                      */
/* ___________________________                                      */
/*                                                                 !*/

/*!                                                                 */
/*  Inverted matrices                                               */
/*                                                                 !*/

  cmat.g11 =      1.0;
  cmat.g12 =      0.0;
  cmat.g13 =      0.0;
  cmat.g14 =      0.0;

  cmat.g21 =      1.0;
  cmat.g22 =    1.0/3.0;
  cmat.g23 =      0.0;
  cmat.g24 =      0.0;

  cmat.g31 =      1.0;
  cmat.g32 =    2.0/3.0;
  cmat.g33 =    1.0/3.0;
  cmat.g34 =      0.0;

  cmat.g41 =      1.0;
  cmat.g42 =      1.0;
  cmat.g43 =      1.0;
  cmat.g44 =      1.0;

  ctmat.g11 =     1.0;
  ctmat.g12 =     1.0;
  ctmat.g13 =     1.0;
  ctmat.g14 =     1.0;

  ctmat.g21 =     0.0;
  ctmat.g22 =   1.0/3.0;
  ctmat.g23 =   2.0/3.0;
  ctmat.g24 =     1.0;

  ctmat.g31 =     0.0;
  ctmat.g32 =     0.0;
  ctmat.g33 =   1.0/3.0;
  ctmat.g34 =     1.0;

  ctmat.g41 =     0.0;
  ctmat.g42 =     0.0;
  ctmat.g43 =     0.0;
  ctmat.g44 =     1.0;

/*!                                                                 */
/*  Input coefficients to 4X4X4 matrix                              */
/*                                                                 !*/

  qmat.a00x = p_i->a00x;
  qmat.a01x = p_i->a01x;
  qmat.a02x = p_i->a02x;
  qmat.a03x = p_i->a03x;

  qmat.a10x = p_i->a10x;
  qmat.a11x = p_i->a11x;
  qmat.a12x = p_i->a12x;
  qmat.a13x = p_i->a13x;

  qmat.a20x = p_i->a20x;
  qmat.a21x = p_i->a21x;
  qmat.a22x = p_i->a22x;
  qmat.a23x = p_i->a23x;

  qmat.a30x = p_i->a30x;
  qmat.a31x = p_i->a31x;
  qmat.a32x = p_i->a32x;
  qmat.a33x = p_i->a33x;

  qmat.a00y = p_i->a00y;
  qmat.a01y = p_i->a01y;
  qmat.a02y = p_i->a02y;
  qmat.a03y = p_i->a03y;

  qmat.a10y = p_i->a10y;
  qmat.a11y = p_i->a11y;
  qmat.a12y = p_i->a12y;
  qmat.a13y = p_i->a13y;

  qmat.a20y = p_i->a20y;
  qmat.a21y = p_i->a21y;
  qmat.a22y = p_i->a22y;
  qmat.a23y = p_i->a23y;

  qmat.a30y = p_i->a30y;
  qmat.a31y = p_i->a31y;
  qmat.a32y = p_i->a32y;
  qmat.a33y = p_i->a33y;

  qmat.a00z = p_i->a00z;
  qmat.a01z = p_i->a01z;
  qmat.a02z = p_i->a02z;
  qmat.a03z = p_i->a03z;

  qmat.a10z = p_i->a10z;
  qmat.a11z = p_i->a11z;
  qmat.a12z = p_i->a12z;
  qmat.a13z = p_i->a13z;

  qmat.a20z = p_i->a20z;
  qmat.a21z = p_i->a21z;
  qmat.a22z = p_i->a22z;
  qmat.a23z = p_i->a23z;

  qmat.a30z = p_i->a30z;
  qmat.a31z = p_i->a31z;
  qmat.a32z = p_i->a32z;
  qmat.a33z = p_i->a33z;

  qmat.a00  = 0.0;         
  qmat.a01  = 0.0;         
  qmat.a02  = 0.0;         
  qmat.a03  = 0.0;         

  qmat.a10  = 0.0;         
  qmat.a11  = 0.0;         
  qmat.a12  = 0.0;         
  qmat.a13  = 0.0;         

  qmat.a20  = 0.0;         
  qmat.a21  = 0.0;         
  qmat.a22  = 0.0;         
  qmat.a23  = 0.0;         

  qmat.a30  = 0.0;         
  qmat.a31  = 0.0;         
  qmat.a32  = 0.0;         
  qmat.a33  = 0.0;         

/*!                                                                 */
/*  Multiplication cqmat= cmat*qmat.                                */
/*  Call of varkon_tensmult_l (sur650).                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur280 Calculate cqmat= cmat*qmat (Call of varkon_tensmult_l)\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

   status= varkon_tensmult_l (&qmat,&cmat,&cqmat);
   if (status<0) 
        {
        sprintf(errbuf,"sur650%%varkon_pat_bicbez");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*  Multiplication cqctmat= cqmat*ctmat.                            */
/*  Call of varkon_tensmult_r (sur651).                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur280 Calculate cqctmat=cqmat*ctmat (Call of varkon_tensmult_r)\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

   status= varkon_tensmult_r (&cqmat,&ctmat,&cqctmat);
   if (status<0) 
        {
        sprintf(errbuf,"sur651%%varkon_pat_bicbez");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*  Coefficients for the patch to output variable p_o               */
/*                                                                 !*/

    p_o->r00x = cqctmat.a00x;  
    p_o->r01x = cqctmat.a01x;  
    p_o->r02x = cqctmat.a02x;  
    p_o->r03x = cqctmat.a03x;  
    p_o->r10x = cqctmat.a10x;  
    p_o->r11x = cqctmat.a11x;  
    p_o->r12x = cqctmat.a12x;  
    p_o->r13x = cqctmat.a13x;  
    p_o->r20x = cqctmat.a20x;  
    p_o->r21x = cqctmat.a21x;  
    p_o->r22x = cqctmat.a22x;  
    p_o->r23x = cqctmat.a23x;  
    p_o->r30x = cqctmat.a30x;  
    p_o->r31x = cqctmat.a31x;  
    p_o->r32x = cqctmat.a32x;  
    p_o->r33x = cqctmat.a33x;  

    p_o->r00y = cqctmat.a00y;  
    p_o->r01y = cqctmat.a01y;  
    p_o->r02y = cqctmat.a02y;  
    p_o->r03y = cqctmat.a03y;  
    p_o->r10y = cqctmat.a10y;  
    p_o->r11y = cqctmat.a11y;  
    p_o->r12y = cqctmat.a12y;  
    p_o->r13y = cqctmat.a13y;  
    p_o->r20y = cqctmat.a20y;  
    p_o->r21y = cqctmat.a21y;  
    p_o->r22y = cqctmat.a22y;  
    p_o->r23y = cqctmat.a23y;  
    p_o->r30y = cqctmat.a30y;  
    p_o->r31y = cqctmat.a31y;  
    p_o->r32y = cqctmat.a32y;  
    p_o->r33y = cqctmat.a33y;  

    p_o->r00z = cqctmat.a00z;  
    p_o->r01z = cqctmat.a01z;  
    p_o->r02z = cqctmat.a02z;  
    p_o->r03z = cqctmat.a03z;  
    p_o->r10z = cqctmat.a10z;  
    p_o->r11z = cqctmat.a11z;  
    p_o->r12z = cqctmat.a12z;  
    p_o->r13z = cqctmat.a13z;  
    p_o->r20z = cqctmat.a20z;  
    p_o->r21z = cqctmat.a21z;  
    p_o->r22z = cqctmat.a22z;  
    p_o->r23z = cqctmat.a23z;  
    p_o->r30z = cqctmat.a30z;  
    p_o->r31z = cqctmat.a31z;  
    p_o->r32z = cqctmat.a32z;  
    p_o->r33z = cqctmat.a33z;  

/*  Only first weight was one (1). All others zer (0) 1997-11-08 */
    p_o->r00  = 1.0;  
    p_o->r01  = 1.0;
    p_o->r02  = 1.0;
    p_o->r03  = 1.0;
    p_o->r10  = 1.0;
    p_o->r11  = 1.0;
    p_o->r12  = 1.0;
    p_o->r13  = 1.0;
    p_o->r20  = 1.0;
    p_o->r21  = 1.0;
    p_o->r22  = 1.0;
    p_o->r23  = 1.0;
    p_o->r30  = 1.0;
    p_o->r31  = 1.0;
    p_o->r32  = 1.0;
    p_o->r33  = 1.0;
 
    p_o->ofs_pat = 0.0;          /* Offset for patch        */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "   r00x       r01x         r02x       r03x \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r00x,p_o->r01x,p_o->r02x,p_o->r03x);
  fprintf(dbgfil(SURPAC),
  "   r10x       r11x         r12x       r13x \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r10x,p_o->r11x,p_o->r12x,p_o->r13x);
  fprintf(dbgfil(SURPAC),
  "   r20x       r21x         r22x       r23x \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r20x,p_o->r21x,p_o->r22x,p_o->r23x);
  fprintf(dbgfil(SURPAC),
  "   r30x       r31x         r32x       r33x \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r30x,p_o->r31x,p_o->r32x,p_o->r33x);
  fprintf(dbgfil(SURPAC),
  "   r00y       r01y         r02y       r03y \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r00y,p_o->r01y,p_o->r02y,p_o->r03y);
  fprintf(dbgfil(SURPAC),
  "   r10y       r11y         r12y       r13y \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r10y,p_o->r11y,p_o->r12y,p_o->r13y);
  fprintf(dbgfil(SURPAC),
  "   r20y       r21y         r22y       r23y \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r20y,p_o->r21y,p_o->r22y,p_o->r23y);
  fprintf(dbgfil(SURPAC),
  "   r30y       r31y         r32y       r33y \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r30y,p_o->r31y,p_o->r32y,p_o->r33y);
  fprintf(dbgfil(SURPAC),
  "   r00z       r01z         r02z       r03z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r00z,p_o->r01z,p_o->r02z,p_o->r03z);
  fprintf(dbgfil(SURPAC),
  "   r10z       r11z         r12z       r13z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r10z,p_o->r11z,p_o->r12z,p_o->r13z);
  fprintf(dbgfil(SURPAC),
  "   r20z       r21z         r22z       r23z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r20z,p_o->r21z,p_o->r22z,p_o->r23z);
  fprintf(dbgfil(SURPAC),
  "   r30z       r31z         r32z       r33z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->r30z,p_o->r31z,p_o->r32z,p_o->r33z);
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "   r00x       r00y         r00z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r11 \n",
   p_o->r00x,p_o->r00y,p_o->r00z);
  fprintf(dbgfil(SURPAC),
  "   r10x       r10y         r10z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r12 \n",
   p_o->r10x,p_o->r10y,p_o->r10z);
  fprintf(dbgfil(SURPAC),
  "   r20x       r20y         r20z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r13 \n",
   p_o->r20x,p_o->r20y,p_o->r20z);
  fprintf(dbgfil(SURPAC),
  "   r30x       r30y         r30z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r14 \n",
   p_o->r30x,p_o->r30y,p_o->r30z);

  fprintf(dbgfil(SURPAC),
  "   r01x       r01y         r01z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r21 \n",
   p_o->r01x,p_o->r01y,p_o->r01z);
  fprintf(dbgfil(SURPAC),
  "   r11x       r11y         r11z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r22 \n",
   p_o->r11x,p_o->r11y,p_o->r11z);
  fprintf(dbgfil(SURPAC),
  "   r21x       r21y         r21z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r23 \n",
   p_o->r21x,p_o->r21y,p_o->r21z);
  fprintf(dbgfil(SURPAC),
  "   r31x       r31y         r31z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r24 \n",
   p_o->r31x,p_o->r31y,p_o->r31z);

  fprintf(dbgfil(SURPAC),
  "   r02x       r02y         r02z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r31 \n",
   p_o->r02x,p_o->r02y,p_o->r02z);
  fprintf(dbgfil(SURPAC),
  "   r12x       r12y         r12z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r32 \n",
   p_o->r12x,p_o->r12y,p_o->r12z);
  fprintf(dbgfil(SURPAC),
  "   r22x       r22y         r22z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r33 \n",
   p_o->r22x,p_o->r22y,p_o->r22z);
  fprintf(dbgfil(SURPAC),
  "   r32x       r32y         r32z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r34 \n",
   p_o->r32x,p_o->r32y,p_o->r32z);

  fprintf(dbgfil(SURPAC),
  "   r03x       r03y         r03z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r41 \n",
   p_o->r03x,p_o->r03y,p_o->r03z);
  fprintf(dbgfil(SURPAC),
  "   r13x       r13y         r13z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r42 \n",
   p_o->r13x,p_o->r13y,p_o->r13z);
  fprintf(dbgfil(SURPAC),
  "   r23x       r23y         r23z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r43 \n",
   p_o->r23x,p_o->r23y,p_o->r23z);
  fprintf(dbgfil(SURPAC),
  "   r33x       r33y         r33z \n");
  fprintf(dbgfil(SURPAC),"%f %f %f   r44 \n",
   p_o->r33x,p_o->r33y,p_o->r33z);

  }
#endif

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur280 Exit ****** varkon_pat_bicbez ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
