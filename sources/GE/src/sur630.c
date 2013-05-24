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
/*  Function: varkon_pat_bicrep                    File: sur630.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Reparametrisation of a bicubic patch.                           */
/*                                                                  */
/*                                                                  */
/*  The patch is redefined (the coefficients are recalculated)      */
/*  in the following way:                                           */
/*                                                                  */
/*              Output patch  !  Input patch                        */
/*                            !                                     */
/*                 u     v    !    u     v                          */
/*              ______________!______________                       */
/*                            !                                     */
/*                 0     0    !   s_u   s_v                         */
/*                            !                                     */
/*                 0     0    !   e_u   e_v                         */
/*                            !                                     */
/*                                                                  */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-11-02   Originally written                                 */
/*  1996-01-05   Comments changed                                   */
/*  1996-01-18   Missing end of comment                             */
/*  1997-01-30   Surface offset for reversed U or V                 */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_bicrep     Reparameterization bicubic patch */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_tensmult_l    * Tensor T(4X4X4)=G(4X4)*A(4X4X4)           */
/* varkon_tensmult_r    * Tensor T(4X4X4)=A(4X4X4)*G(4X4)           */
/* varkon_erpush        * Error message to terminal                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_bicrep (sur630)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_pat_bicrep (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATC *p_i,           /* Pointer to the input  patch             */
  DBfloat s_u,           /* Reparameterisation start local U value  */
  DBfloat s_v,           /* Reparameterisation start local V value  */
  DBfloat e_u,           /* Reparameterisation end   local U value  */
  DBfloat e_v,           /* Reparameterisation end   local V value  */
  GMPATC *p_o )          /* Pointer to the output patch             */

/* Out:                                                             */
/*        Coefficients of the transformed patch                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATR  t1;           /* Tensor (4X4X4) for  patch coefficients  */
   GMPATR  t2;           /* Tensor (4X4X4) for  patch coefficients  */
   GMPATR  t3;           /* Tensor (4X4X4) with patch coefficients  */
   DBTmat  m_u;          /* Reparametrisation matrix for U          */
   DBTmat  m_v;          /* Reparametrisation matrix for V          */
   DBfloat k0_u,k_u;     /* Reparametrisation coefficients for U    */
   DBfloat k0_v,k_v;     /* Reparametrisation coefficients for V    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  status;        /* Error code                              */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/*  Reparametrisation                                               */
/*  _________________                                               */
/*                                                                  */
/*  Relation between input and output parameters:                   */
/*                                                                  */
/*     U_IN = ko + k * U_OUT                                        */
/*     V_IN = ko + k * V_OUT                                        */
/*                                                                  */
/*     U_IN = s_u       <==> U_OUT = 0.0                            */
/*     U_IN = e_u       <==> U_OUT = 1.0                            */
/*                                                                  */
/*     V_IN = s_v       <==> V_OUT = 0.0                            */
/*     V_IN = e_v       <==> V_OUT = 1.0                            */
/*                                                                  */
/*     The solution is for U:                                       */
/*                                                                  */
/*     ko = u_s                                                     */
/*     k  = u_e - u_s                                               */
/*                                                                  */
/*     The solution is for V:                                       */
/*                                                                  */
/*     ko = v_s                                                     */
/*     k  = v_e - v_s                                               */
/*                                                                  */
/* The input patch is defined by:                                   */
/*                                                                  */
/*                   2      3                         2      3 T    */
/* R = (1  U_IN  U_IN   U_IN ) * A_IN * (1  V_IN  V_IN   V_IN )     */
/*                                                                  */
/* Recalculation of U is done by substitution:                      */
/*                                                                  */
/*                   2      3                                       */
/*     (1  U_IN  U_IN   U_IN ) =                                    */
/*                                                                  */
/*                                   2             3                */
/*     = (1  ko+k*U_OUT  (ko+k*U_OUT)  (ko+k*U_OUT) ) =             */
/*                                                                  */
/*                       2       3                                  */
/*     = (1  U_OUT  U_OUT   U_OUT ) * M  where M is:                */
/*                                                                  */
/*     !             2   3    !                                     */
/*     ! 1    ko   ko  ko     !                                     */
/*     !                  2   !                                     */
/*     ! 0    k  2ko*k 3ko *k !                                     */
/*     !            2       2 !                                     */
/*     ! 0    0    k   3ko*k  !                                     */
/*     !                3     !                                     */
/*     ! 0    0    0   k      !                                     */
/*                                                                  */
/*     And in the same way for the V vector:                        */
/*                                                                  */
/*     ! 1     !   ! 1             !                                */
/*     !       !   !               !                                */
/*     ! V_IN  !   ! ko+k*V_OUT    !                                */
/*     !     2 ! = !             2 ! =                              */
/*     ! V_IN  !   ! (ko+k*V_OUT)  !                                */
/*     !     3 !   !             3 !                                */
/*     ! V_IN  !   ! (ko+k*V_OUT)  !                                */
/*                                                                  */
/*        T                  2       3 T          T                 */
/*     = M * (1  V_OUT  V_OUT   V_OUT )    where M  is:             */
/*                                                                  */
/*     !                      !                                     */
/*     ! 1     0     0     0  !                                     */
/*     !                      !                                     */
/*     ! ko    k     0     0  !                                     */
/*     !   2          2       !                                     */
/*     ! ko   2ko*k  k     0  !                                     */
/*     !   3    2        2  3 !                                     */
/*     ! ko   3ko *k 3ko*k  k !                                     */
/*                                                                  */
/*  The input patch is also defined by the equivalent expression:   */
/*                                                                  */
/*                 2      3          T               2      3 T     */
/* R=(1 U_OUT U_OUT  U_OUT )*M*A_IN*M* (1 V_OUT V_OUT  V_OUT )      */
/*                                                                  */
/*                                T                                 */
/*     Define A_OUT = M * A_IN * M                                  */
/*                                                                  */
/*                                                                  */
/*     Calculate the desired matrices:                              */
/*                                                                  */
/*                           T                                      */
/*     A_OUTX = M * A_INX * M                                       */
/*                           T                                      */
/*     A_OUTY = M * A_INY * M                                       */
/*                           T                                      */
/*     A_OUTZ = M * A_INZ * M                                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur630 Enter varkon_pat_bicrep (sur630) \n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur630 s_u %f s_v %f e_u %f e_v %f\n",s_u,s_v,e_u,e_v );
  }
#endif
 
/*!                                                                 */
/*  Check input parameter values for Debug On                       */
/*                                                                 !*/

#ifdef DEBUG
    if ( fabs(s_u-e_u) <  0.001 || fabs(s_v-e_v) < 0.01 )
       {
       sprintf(errbuf,     
          "s_u=e_u or s_v=e_v%%varkon_pat_bicrep (sur630");
       return(varkon_erpush("SU2993",errbuf));
       }
#endif


/*!                                                                 */
/* 2. Matrices for the reparametrisation                            */
/* _____________________________________                            */
/*                                                                 !*/

/*!                                                                 */
/* Coefficients                                                     */
/*                                                                 !*/

   k0_u = s_u;  
   k_u  = e_u - s_u;
   k0_v = s_v;  
   k_v  = e_v - s_v;

/*!                                                                 */
/* Matrix for U                                                     */
/*                                                                 !*/

   m_u.g11 = 1.0;
   m_u.g12 = k0_u;
   m_u.g13 = k0_u*k0_u;
   m_u.g14 = k0_u*k0_u*k0_u;

   m_u.g21 = 0.0;
   m_u.g22 = k_u;
   m_u.g23 = 2.0*k0_u*k_u;
   m_u.g24 = 3.0*k0_u*k0_u*k_u;

   m_u.g31 = 0.0;
   m_u.g32 = 0.0;
   m_u.g33 = k_u*k_u;
   m_u.g34 = 3.0*k0_u*k_u*k_u;

   m_u.g41 = 0.0;
   m_u.g42 = 0.0;
   m_u.g43 = 0.0;
   m_u.g44 = k_u*k_u*k_u;

   m_v.g11 = 1.0;
   m_v.g12 = 0.0;
   m_v.g13 = 0.0;
   m_v.g14 = 0.0;

   m_v.g21 = k0_v;
   m_v.g22 = k_v;
   m_v.g23 = 0.0;
   m_v.g24 = 0.0;

   m_v.g31 = k0_v*k0_v;
   m_v.g32 = 2.0*k0_v*k_v;
   m_v.g33 = k_v*k_v;
   m_v.g34 = 0.0;

   m_v.g41 = k0_v*k0_v*k0_v;
   m_v.g42 = 3.0*k0_v*k0_v*k_v;
   m_v.g43 = 3.0*k0_v*k_v*k_v;
   m_v.g44 = k_v*k_v*k_v;

/*!                                                                 */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur630 Input bicubic patch \n");
  }
if ( dbglev(SURPAC) == 1 )
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
/*  The bicubic coefficients to tensor t1                           */
/*                                                                 !*/

    t1.a00x=p_i->a00x;
    t1.a01x=p_i->a01x;
    t1.a02x=p_i->a02x;
    t1.a03x=p_i->a03x;

    t1.a10x=p_i->a10x;
    t1.a11x=p_i->a11x;
    t1.a12x=p_i->a12x;
    t1.a13x=p_i->a13x;

    t1.a20x=p_i->a20x;
    t1.a21x=p_i->a21x;
    t1.a22x=p_i->a22x;
    t1.a23x=p_i->a23x;

    t1.a30x=p_i->a30x;
    t1.a31x=p_i->a31x;
    t1.a32x=p_i->a32x;
    t1.a33x=p_i->a33x;

    t1.a00y=p_i->a00y;
    t1.a01y=p_i->a01y;
    t1.a02y=p_i->a02y;
    t1.a03y=p_i->a03y;

    t1.a10y=p_i->a10y;
    t1.a11y=p_i->a11y;
    t1.a12y=p_i->a12y;
    t1.a13y=p_i->a13y;

    t1.a20y=p_i->a20y;
    t1.a21y=p_i->a21y;
    t1.a22y=p_i->a22y;
    t1.a23y=p_i->a23y;

    t1.a30y=p_i->a30y;
    t1.a31y=p_i->a31y;
    t1.a32y=p_i->a32y;
    t1.a33y=p_i->a33y;

    t1.a00z=p_i->a00z;
    t1.a01z=p_i->a01z;
    t1.a02z=p_i->a02z;
    t1.a03z=p_i->a03z;

    t1.a10z=p_i->a10z;
    t1.a11z=p_i->a11z;
    t1.a12z=p_i->a12z;
    t1.a13z=p_i->a13z;

    t1.a20z=p_i->a20z;
    t1.a21z=p_i->a21z;
    t1.a22z=p_i->a22z;
    t1.a23z=p_i->a23z;

    t1.a30z=p_i->a30z;
    t1.a31z=p_i->a31z;
    t1.a32z=p_i->a32z;
    t1.a33z=p_i->a33z;

    t1.a00 = 0.0;     
    t1.a01 = 0.0;     
    t1.a02 = 0.0;     
    t1.a03 = 0.0;     

    t1.a10 = 0.0;     
    t1.a11 = 0.0;     
    t1.a12 = 0.0;     
    t1.a13 = 0.0;     

    t1.a20 = 0.0;     
    t1.a21 = 0.0;     
    t1.a22 = 0.0;     
    t1.a23 = 0.0;     

    t1.a30 = 0.0;     
    t1.a31 = 0.0;     
    t1.a32 = 0.0;     
    t1.a33 = 0.0;     




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a00x, t1.a01x, t1.a02x, t1.a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a10x, t1.a11x, t1.a12x, t1.a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a20x, t1.a21x, t1.a22x, t1.a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a30x, t1.a31x, t1.a32x, t1.a33x);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a00y, t1.a01y, t1.a02y, t1.a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a10y, t1.a11y, t1.a12y, t1.a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a20y, t1.a21y, t1.a22y, t1.a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a30y, t1.a31y, t1.a32y, t1.a33y);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a00z, t1.a01z, t1.a02z, t1.a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a10z, t1.a11z, t1.a12z, t1.a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a20z, t1.a21z, t1.a22z, t1.a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t1.a30z, t1.a31z, t1.a32z, t1.a33z);

  }
#endif

/*!                                                                 */
/* 3. Calculate coefficients for the reparametrised bicubic patch   */
/* ______________________________________________________________   */
/*                                                                 !*/

/*!                                                                 */
/*  Multiplication t2= m_u*t1.                                      */
/*  Call of varkon_tensmult_l (sur650).                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur254 Calculate t2= m_u*t1. Call of varkon_tensmult_l)\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

   status= varkon_tensmult_l (&t1,&m_u,&t2);
   if (status<0) 
        {
        sprintf(errbuf,"sur650%%varkon_pat_bicrep ");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a00x, t2.a01x, t2.a02x, t2.a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a10x, t2.a11x, t2.a12x, t2.a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a20x, t2.a21x, t2.a22x, t2.a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a30x, t2.a31x, t2.a32x, t2.a33x);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a00y, t2.a01y, t2.a02y, t2.a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a10y, t2.a11y, t2.a12y, t2.a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a20y, t2.a21y, t2.a22y, t2.a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a30y, t2.a31y, t2.a32y, t2.a33y);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a00z, t2.a01z, t2.a02z, t2.a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a10z, t2.a11z, t2.a12z, t2.a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a20z, t2.a21z, t2.a22z, t2.a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
    t2.a30z, t2.a31z, t2.a32z, t2.a33z);

  }
#endif
/*!                                                                 */
/*  Multiplication t3 = t2*m_v.                                     */
/*  Call of varkon_tensmult_r (sur651).                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur254 Calculate t3 = t2*m_v. Call of varkon_tensmult_r)\n");
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

   status= varkon_tensmult_r (&t2, &m_v ,&t3 );
   if (status<0) 
        {
        sprintf(errbuf,"sur651%%varkon_pat_bicrep ");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 4. Exit                                                          */
/*!                                                                 */
/*  Patch coefficients to output patch. (Patch  t3 to patch p_o)    */
/*                                                                 !*/

    p_o->a00x =  t3.a00x;
    p_o->a01x =  t3.a01x;
    p_o->a02x =  t3.a02x;
    p_o->a03x =  t3.a03x;
    p_o->a10x =  t3.a10x;
    p_o->a11x =  t3.a11x;
    p_o->a12x =  t3.a12x;
    p_o->a13x =  t3.a13x;
    p_o->a20x =  t3.a20x;
    p_o->a21x =  t3.a21x;
    p_o->a22x =  t3.a22x;
    p_o->a23x =  t3.a23x;
    p_o->a30x =  t3.a30x;
    p_o->a31x =  t3.a31x;
    p_o->a32x =  t3.a32x;
    p_o->a33x =  t3.a33x;

    p_o->a00y =  t3.a00y;
    p_o->a01y =  t3.a01y;
    p_o->a02y =  t3.a02y;
    p_o->a03y =  t3.a03y;
    p_o->a10y =  t3.a10y;
    p_o->a11y =  t3.a11y;
    p_o->a12y =  t3.a12y;
    p_o->a13y =  t3.a13y;
    p_o->a20y =  t3.a20y;
    p_o->a21y =  t3.a21y;
    p_o->a22y =  t3.a22y;
    p_o->a23y =  t3.a23y;
    p_o->a30y =  t3.a30y;
    p_o->a31y =  t3.a31y;
    p_o->a32y =  t3.a32y;
    p_o->a33y =  t3.a33y;

    p_o->a00z =  t3.a00z;
    p_o->a01z =  t3.a01z;
    p_o->a02z =  t3.a02z;
    p_o->a03z =  t3.a03z;
    p_o->a10z =  t3.a10z;
    p_o->a11z =  t3.a11z;
    p_o->a12z =  t3.a12z;
    p_o->a13z =  t3.a13z;
    p_o->a20z =  t3.a20z;
    p_o->a21z =  t3.a21z;
    p_o->a22z =  t3.a22z;
    p_o->a23z =  t3.a23z;
    p_o->a30z =  t3.a30z;
    p_o->a31z =  t3.a31z;
    p_o->a32z =  t3.a32z;
    p_o->a33z =  t3.a33z;

/*  Offset for patch */
  if          ( s_u > e_u  &&  s_v < e_v )
    {
    p_o->ofs_pat = - p_i->ofs_pat;
    }
  else if     ( s_u < e_u  &&  s_v > e_v )
    {
    p_o->ofs_pat = - p_i->ofs_pat;
    }
  else
    {
    p_o->ofs_pat = p_i->ofs_pat;
    }



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur630 Output reparametrised bicubic patch \n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a00x,p_o->a01x,p_o->a02x,p_o->a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a10x,p_o->a11x,p_o->a12x,p_o->a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a20x,p_o->a21x,p_o->a22x,p_o->a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a30x,p_o->a31x,p_o->a32x,p_o->a33x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a00y,p_o->a01y,p_o->a02y,p_o->a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a10y,p_o->a11y,p_o->a12y,p_o->a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a20y,p_o->a21y,p_o->a22y,p_o->a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a30y,p_o->a31y,p_o->a32y,p_o->a33y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a00z,p_o->a01z,p_o->a02z,p_o->a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a10z,p_o->a11z,p_o->a12z,p_o->a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a20z,p_o->a21z,p_o->a22z,p_o->a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   p_o->a30z,p_o->a31z,p_o->a32z,p_o->a33z);

  }
#endif

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur630 Exit ****** varkon_pat_bicrep ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
