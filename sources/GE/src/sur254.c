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
/*  Function: varkon_pat_biccre1                   File: sur254.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a bicubic surface patch.                   */
/*                                                                  */
/*  Input data is four corner points with derivatives.              */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-29   Originally written                                 */
/*  1996-09-07   Elimination of compiler warning                    */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_biccre1    Bicubic patch from 4 corner pts  */
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
/* varkon_tensmult_l    * Tensor T(4X4X4)=G(4X4)*A(4X4X4)           */
/* varkon_tensmult_r    * Tensor T(4X4X4)=A(4X4X4)*G(4X4)           */
/* varkon_erpush        * Error message to terminal                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxx failed in varkon_pat_biccre1      */
/* SU2993 = Severe program error         in varkon_pat_biccre1      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus  varkon_pat_biccre1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_r00,        /* Corner point U=0 , V=0            (ptr) */
   EVALS  *p_r10,        /* Corner point U=1 , V=0            (ptr) */
   EVALS  *p_r01,        /* Corner point U=0 , V=1            (ptr) */
   EVALS  *p_r11,        /* Corner point U=1 , V=1            (ptr) */
   GMPATC *p_patc )      /* Bicubic patch                     (ptr) */

/* Out:                                                             */
/*       Data to p_patc                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  GMPATR  qmat;          /* Tensor with input points/derivatives    */
  DBTmat  cmat;          /* Blending coefficients matrix            */
  DBTmat  ctmat;         /* Blending coefficients matrix transpon.  */
  GMPATR  cqmat;         /* Tensor cmat*qmat                        */
  GMPATR  cqctmat;       /* Tensor cmat*qmat*ctmat                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*!New-Page--------------------------------------------------------!*/
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
/* The bicubic patch defined by the corner points                   */
/* ----------------------------------------------                   */
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
"sur254 Enter *** varkon_pat_biccre1 p_patc %d \n",
                p_patc );
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
   status=initial (p_r00, p_r10, p_r01, p_r11, p_patc);
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_biccre1 (sur254)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/*  Blending coefficient matrices cmat and ctmat                    */
/*                                                                 !*/

  cmat.g11 = 1.0;
  cmat.g12 = 0.0;
  cmat.g13 = 0.0;
  cmat.g14 = 0.0;

  cmat.g21 = 0.0;
  cmat.g22 = 0.0;
  cmat.g23 = 1.0;
  cmat.g24 = 0.0;

  cmat.g31 =  -3.0;
  cmat.g32 =   3.0;
  cmat.g33 =  -2.0;
  cmat.g34 =  -1.0;

  cmat.g41 =   2.0;
  cmat.g42 =  -2.0;
  cmat.g43 =   1.0;
  cmat.g44 =   1.0;

  ctmat.g11 =   1.0;
  ctmat.g12 =   0.0;
  ctmat.g13 =  -3.0;
  ctmat.g14 =   2.0;

  ctmat.g21 =   0.0;
  ctmat.g22 =   0.0;
  ctmat.g23 =   3.0;
  ctmat.g24 =  -2.0;

  ctmat.g31 =   0.0;
  ctmat.g32 =   1.0;
  ctmat.g33 =  -2.0;
  ctmat.g34 =   1.0;

  ctmat.g41 =   0.0;
  ctmat.g42 =   0.0;
  ctmat.g43 =  -1.0;
  ctmat.g44 =   1.0;

/*!                                                                 */
/*  Input coordinates and derivatives to tensor qmat                */
/*                                                                 !*/

  qmat.a00x = p_r00->r_x;
  qmat.a01x = p_r01->r_x;
  qmat.a02x = p_r00->v_x;
  qmat.a03x = p_r01->v_x;

  qmat.a10x = p_r10->r_x;
  qmat.a11x = p_r11->r_x;
  qmat.a12x = p_r10->v_x;
  qmat.a13x = p_r11->v_x;

  qmat.a20x = p_r00->u_x;
  qmat.a21x = p_r01->u_x;
  qmat.a22x = p_r00->uv_x;
  qmat.a23x = p_r01->uv_x;

  qmat.a30x = p_r10->u_x;
  qmat.a31x = p_r11->u_x;
  qmat.a32x = p_r10->uv_x;
  qmat.a33x = p_r11->uv_x;

  qmat.a00y = p_r00->r_y;
  qmat.a01y = p_r01->r_y;
  qmat.a02y = p_r00->v_y;
  qmat.a03y = p_r01->v_y;

  qmat.a10y = p_r10->r_y;
  qmat.a11y = p_r11->r_y;
  qmat.a12y = p_r10->v_y;
  qmat.a13y = p_r11->v_y;

  qmat.a20y = p_r00->u_y;
  qmat.a21y = p_r01->u_y;
  qmat.a22y = p_r00->uv_y;
  qmat.a23y = p_r01->uv_y;

  qmat.a30y = p_r10->u_y;
  qmat.a31y = p_r11->u_y;
  qmat.a32y = p_r10->uv_y;
  qmat.a33y = p_r11->uv_y;

  qmat.a00z = p_r00->r_z;
  qmat.a01z = p_r01->r_z;
  qmat.a02z = p_r00->v_z;
  qmat.a03z = p_r01->v_z;

  qmat.a10z = p_r10->r_z;
  qmat.a11z = p_r11->r_z;
  qmat.a12z = p_r10->v_z;
  qmat.a13z = p_r11->v_z;

  qmat.a20z = p_r00->u_z;
  qmat.a21z = p_r01->u_z;
  qmat.a22z = p_r00->uv_z;
  qmat.a23z = p_r01->uv_z;

  qmat.a30z = p_r10->u_z;
  qmat.a31z = p_r11->u_z;
  qmat.a32z = p_r10->uv_z;
  qmat.a33z = p_r11->uv_z;

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
/* 2. Calculate coefficients for the bicubic patch                  */
/* _______________________________________________                  */
/*                                                                 !*/

/*!                                                                 */
/*  Multiplication cqmat= cmat*qmat.                                */
/*  Call of varkon_tensmult_l (sur650).                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur254 Calculate cqmat= cmat*qmat (Call of varkon_tensmult_l)\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

   status= varkon_tensmult_l (&qmat,&cmat,&cqmat);
   if (status<0) 
        {
        sprintf(errbuf,"sur650%%varkon_pat_biccre1");
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
  "sur254 Calculate cqctmat=cqmat*ctmat (Call of varkon_tensmult_r)\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

   status= varkon_tensmult_r (&cqmat,&ctmat,&cqctmat);
   if (status<0) 
        {
        sprintf(errbuf,"sur651%%varkon_pat_biccre1");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*  Coefficients for the patch to output variable p_patc            */
/*                                                                 !*/

    p_patc->a00x = cqctmat.a00x;  
    p_patc->a01x = cqctmat.a01x;  
    p_patc->a02x = cqctmat.a02x;  
    p_patc->a03x = cqctmat.a03x;  
    p_patc->a10x = cqctmat.a10x;  
    p_patc->a11x = cqctmat.a11x;  
    p_patc->a12x = cqctmat.a12x;  
    p_patc->a13x = cqctmat.a13x;  
    p_patc->a20x = cqctmat.a20x;  
    p_patc->a21x = cqctmat.a21x;  
    p_patc->a22x = cqctmat.a22x;  
    p_patc->a23x = cqctmat.a23x;  
    p_patc->a30x = cqctmat.a30x;  
    p_patc->a31x = cqctmat.a31x;  
    p_patc->a32x = cqctmat.a32x;  
    p_patc->a33x = cqctmat.a33x;  

    p_patc->a00y = cqctmat.a00y;  
    p_patc->a01y = cqctmat.a01y;  
    p_patc->a02y = cqctmat.a02y;  
    p_patc->a03y = cqctmat.a03y;  
    p_patc->a10y = cqctmat.a10y;  
    p_patc->a11y = cqctmat.a11y;  
    p_patc->a12y = cqctmat.a12y;  
    p_patc->a13y = cqctmat.a13y;  
    p_patc->a20y = cqctmat.a20y;  
    p_patc->a21y = cqctmat.a21y;  
    p_patc->a22y = cqctmat.a22y;  
    p_patc->a23y = cqctmat.a23y;  
    p_patc->a30y = cqctmat.a30y;  
    p_patc->a31y = cqctmat.a31y;  
    p_patc->a32y = cqctmat.a32y;  
    p_patc->a33y = cqctmat.a33y;  

    p_patc->a00z = cqctmat.a00z;  
    p_patc->a01z = cqctmat.a01z;  
    p_patc->a02z = cqctmat.a02z;  
    p_patc->a03z = cqctmat.a03z;  
    p_patc->a10z = cqctmat.a10z;  
    p_patc->a11z = cqctmat.a11z;  
    p_patc->a12z = cqctmat.a12z;  
    p_patc->a13z = cqctmat.a13z;  
    p_patc->a20z = cqctmat.a20z;  
    p_patc->a21z = cqctmat.a21z;  
    p_patc->a22z = cqctmat.a22z;  
    p_patc->a23z = cqctmat.a23z;  
    p_patc->a30z = cqctmat.a30z;  
    p_patc->a31z = cqctmat.a31z;  
    p_patc->a32z = cqctmat.a32z;  
    p_patc->a33z = cqctmat.a33z;  

/*!                                                                 */
/*  Let offset for the patch be zero                                */
/*                                                                 !*/
    p_patc->ofs_pat = 0.0;        

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur254 Exit *** varkon_pat_biccre1  \n"
   );
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

   static short initial (p_r00, p_r10, p_r01, p_r11, p_patc)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_r00;        /* Corner point U=0 , V=0            (ptr) */
   EVALS  *p_r10;        /* Corner point U=1 , V=0            (ptr) */
   EVALS  *p_r01;        /* Corner point U=0 , V=1            (ptr) */
   EVALS  *p_r11;        /* Corner point U=1 , V=1            (ptr) */
   GMPATC *p_patc;       /* Pointer to the rational cubic patch     */


/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur254*initial p_r00 %d p_r10 %d p_r01 %d p_r11 %d\n",
                       p_r00,   p_r10, p_r01, p_r11 );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

if ( dbglev(SURPAC) == 1 )
  {

  fprintf(dbgfil(SURPAC),
  "sur254*initial r00      %f %f %fd\n",
        p_r00->r_x,p_r00->r_y,p_r00->r_z    );

  fprintf(dbgfil(SURPAC),
  "sur254*initial r00     %f %f %f\n",
     p_r00->r_x ,p_r00->r_y ,p_r00->r_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial r01     %f %f %f\n",
       p_r01->r_x ,p_r01->r_y ,p_r01->r_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial rv00    %f %f %f\n",
       p_r00->v_x ,p_r00->v_y ,p_r00->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial rv01    %f %f %f\n",
       p_r01->v_x ,p_r01->v_y ,p_r01->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial r10     %f %f %f\n",
       p_r10->r_x ,p_r10->r_y ,p_r10->r_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial r11     %f %f %f\n",
       p_r11->r_x ,p_r11->r_y ,p_r11->r_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial rv10    %f %f %f\n",
       p_r10->v_x ,p_r10->v_y ,p_r10->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial rv11    %f %f %f\n",
       p_r11->v_x ,p_r11->v_y ,p_r11->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ru00    %f %f %f\n",
       p_r00->u_x ,p_r00->u_y ,p_r00->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ru01    %f %f %f\n",
       p_r01->u_x ,p_r01->u_y ,p_r01->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ruv00   %f %f %f\n",
       p_r00->uv_x ,p_r00->uv_y ,p_r00->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ruv01   %f %f %f\n",
       p_r01->uv_x ,p_r01->uv_y ,p_r01->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ru10    %f %f %f\n",
       p_r10->u_x ,p_r10->u_y ,p_r10->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ru11    %f %f %f\n",
       p_r11->u_x ,p_r11->u_y ,p_r11->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ruv10   %f %f %f\n",
       p_r10->uv_x ,p_r10->uv_y ,p_r10->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur254*initial ruv11   %f %f %f\n",
       p_r11->uv_x ,p_r11->uv_y ,p_r11->uv_z  );

  }

/*!                                                                 */
/* 2. Initialize output variables GMPATC and static variables       */
/*                                                                 !*/

/*  Coefficients for X                                              */
/*  -------------------                                             */
    p_patc->a00x= F_UNDEF;   
    p_patc->a01x= F_UNDEF;   
    p_patc->a02x= F_UNDEF;   
    p_patc->a03x= F_UNDEF;   

    p_patc->a10x= F_UNDEF;   
    p_patc->a11x= F_UNDEF;   
    p_patc->a12x= F_UNDEF;   
    p_patc->a13x= F_UNDEF;   

    p_patc->a20x= F_UNDEF;   
    p_patc->a21x= F_UNDEF;   
    p_patc->a22x= F_UNDEF;   
    p_patc->a23x= F_UNDEF;   

    p_patc->a30x= F_UNDEF;   
    p_patc->a31x= F_UNDEF;   
    p_patc->a32x= F_UNDEF;   
    p_patc->a33x= F_UNDEF;   

/*  Coefficients for Y                                              */
/*  -------------------                                             */
    p_patc->a00y= F_UNDEF;   
    p_patc->a01y= F_UNDEF;   
    p_patc->a02y= F_UNDEF;   
    p_patc->a03y= F_UNDEF;   

    p_patc->a10y= F_UNDEF;   
    p_patc->a11y= F_UNDEF;   
    p_patc->a12y= F_UNDEF;   
    p_patc->a13y= F_UNDEF;   

    p_patc->a20y= F_UNDEF;   
    p_patc->a21y= F_UNDEF;   
    p_patc->a22y= F_UNDEF;   
    p_patc->a23y= F_UNDEF;   

    p_patc->a30y= F_UNDEF;   
    p_patc->a31y= F_UNDEF;   
    p_patc->a32y= F_UNDEF;   
    p_patc->a33y= F_UNDEF;   


/*  Coefficients for Z                                              */
/*  -------------------                                             */
    p_patc->a00z= F_UNDEF;   
    p_patc->a01z= F_UNDEF;   
    p_patc->a02z= F_UNDEF;   
    p_patc->a03z= F_UNDEF;   

    p_patc->a10z= F_UNDEF;   
    p_patc->a11z= F_UNDEF;   
    p_patc->a12z= F_UNDEF;   
    p_patc->a13z= F_UNDEF;   

    p_patc->a20z= F_UNDEF;   
    p_patc->a21z= F_UNDEF;   
    p_patc->a22z= F_UNDEF;   
    p_patc->a23z= F_UNDEF;   

    p_patc->a30z= F_UNDEF;   
    p_patc->a31z= F_UNDEF;   
    p_patc->a32z= F_UNDEF;   
    p_patc->a33z= F_UNDEF;   


    return(SUCCED);

} /* End of function                                                */
/***********************************Only*for*Debug*On*(end)**********/
#endif

