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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_tensmult_r                  File: sur651.c     */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Multiplies a 4X4 matrix (from right) with a 4X4X4 tensor.       */
/*  Output is a 4X4X4 tensor.                                       */
/*                                                                  */
/*  Remark:                                                         */
/*  There is an option to let the input tensor pointer be equal     */
/*  to the output tensor pointer. Only one tensor need to be        */
/*  declared in the calling function. But note that the input       */
/*  data of course will be destroyed in this case.                  */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-02   Originally written                                 */
/*  1996-05-28   Deleted errbuf                                     */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_tensmult_r     Tensor T(4X4X4)=A(4X4X4X)*G(4X4) */
/*                                                              */
/*--------------------------------------------------------------*/

/*--------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/

/*------------- Error messages and warnings ------------------------*/
/*                                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_tensmult_r (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATR *ppi,           /* Input  tensor (4X4X4)            (ptr) */
  DBTmat *pc,            /* Input  matrix (4X4)              (ptr) */
  GMPATR *ppo )          /* Output patch  (4X4X4)            (ptr) */

/* Out:                                                             */
/*        Tensor elements in ppo (or in ppi)                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATR  ppt;          /* Temporarely used 4X4X4 tensor           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The input tensor A can for instance be the coordinates and       */
/* derivatives for a rational cubic patch and the matrix G a        */
/* blending coefficient matrix.                                     */
/*                                                                  */
/* The elements in the output tensor is given by:                   */
/*                                                                  */
/*  TX (X coeff)  = AX (X coeff) * G (Blending matrix)              */
/*  TY (Y coeff)  = AY (Y coeff) * G (Blending matrix)              */
/*  TZ (Z coeff)  = AZ (Z coeff) * G (Blending matrix)              */
/*  Tw (w coeff)  = Aw (w coeff) * G (Blending matrix)              */
/*                                                                  */
/*  TX = AX * G with all matrix elements:                           */
/*                                                                  */
/*  !t00x t01x t02x t03x! !a00x a01x a02x a03x! !g11 g12 g13 g14!   */
/*  !t10x t11x t12x t13x!=!a10x a11x a12x a13x!*!g21 g22 g23 g24!   */
/*  !t20x t21x t22x t23x! !a20x a21x a22x a23x! !g31 g32 g33 g34!   */
/*  !t30x t31x t32x t33x! !a30x a31x a32x a33x! !g41 g42 g43 g44!   */
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

/*!                                                                 */
/* 2. Multiply the matrix and the tensor                            */
/* _____________________________________                            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur651 Input tensor \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a00x,ppi->a01x,ppi->a02x,ppi->a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a10x,ppi->a11x,ppi->a12x,ppi->a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a20x,ppi->a21x,ppi->a22x,ppi->a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a30x,ppi->a31x,ppi->a32x,ppi->a33x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a00y,ppi->a01y,ppi->a02y,ppi->a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a10y,ppi->a11y,ppi->a12y,ppi->a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a20y,ppi->a21y,ppi->a22y,ppi->a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a30y,ppi->a31y,ppi->a32y,ppi->a33y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a00z,ppi->a01z,ppi->a02z,ppi->a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a10z,ppi->a11z,ppi->a12z,ppi->a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a20z,ppi->a21z,ppi->a22z,ppi->a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a30z,ppi->a31z,ppi->a32z,ppi->a33z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a00 ,ppi->a01 ,ppi->a02 ,ppi->a03 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a10 ,ppi->a11 ,ppi->a12 ,ppi->a13 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a20 ,ppi->a21 ,ppi->a22 ,ppi->a23 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppi->a30 ,ppi->a31 ,ppi->a32 ,ppi->a33 );

  }
#endif

/*! The X elements     t00x, t01x, ..... t33x  (tensor ppt)        !*/

    ppt.a00x=ppi->a00x*pc->g11+ 
             ppi->a01x*pc->g21+
             ppi->a02x*pc->g31+
             ppi->a03x*pc->g41;
    ppt.a01x=ppi->a00x*pc->g12+
             ppi->a01x*pc->g22+
             ppi->a02x*pc->g32+
             ppi->a03x*pc->g42;
    ppt.a02x=ppi->a00x*pc->g13+
             ppi->a01x*pc->g23+
             ppi->a02x*pc->g33+
             ppi->a03x*pc->g43;
    ppt.a03x=ppi->a00x*pc->g14+
             ppi->a01x*pc->g24+
             ppi->a02x*pc->g34+
             ppi->a03x*pc->g44;

    ppt.a10x=ppi->a10x*pc->g11+ 
             ppi->a11x*pc->g21+
             ppi->a12x*pc->g31+
             ppi->a13x*pc->g41;
    ppt.a11x=ppi->a10x*pc->g12+
             ppi->a11x*pc->g22+
             ppi->a12x*pc->g32+
             ppi->a13x*pc->g42;
    ppt.a12x=ppi->a10x*pc->g13+
             ppi->a11x*pc->g23+
             ppi->a12x*pc->g33+
             ppi->a13x*pc->g43;
    ppt.a13x=ppi->a10x*pc->g14+
             ppi->a11x*pc->g24+
             ppi->a12x*pc->g34+
             ppi->a13x*pc->g44;

    ppt.a20x=ppi->a20x*pc->g11+ 
             ppi->a21x*pc->g21+
             ppi->a22x*pc->g31+
             ppi->a23x*pc->g41;
    ppt.a21x=ppi->a20x*pc->g12+
             ppi->a21x*pc->g22+
             ppi->a22x*pc->g32+
             ppi->a23x*pc->g42;
    ppt.a22x=ppi->a20x*pc->g13+
             ppi->a21x*pc->g23+
             ppi->a22x*pc->g33+
             ppi->a23x*pc->g43;
    ppt.a23x=ppi->a20x*pc->g14+
             ppi->a21x*pc->g24+
             ppi->a22x*pc->g34+
             ppi->a23x*pc->g44;

    ppt.a30x=ppi->a30x*pc->g11+ 
             ppi->a31x*pc->g21+
             ppi->a32x*pc->g31+
             ppi->a33x*pc->g41;
    ppt.a31x=ppi->a30x*pc->g12+
             ppi->a31x*pc->g22+
             ppi->a32x*pc->g32+
             ppi->a33x*pc->g42;
    ppt.a32x=ppi->a30x*pc->g13+
             ppi->a31x*pc->g23+
             ppi->a32x*pc->g33+
             ppi->a33x*pc->g43;
    ppt.a33x=ppi->a30x*pc->g14+
             ppi->a31x*pc->g24+
             ppi->a32x*pc->g34+
             ppi->a33x*pc->g44;


/*! The Y elements     t00y, t01y, ..... t33y                      !*/

    ppt.a00y=ppi->a00y*pc->g11+ 
             ppi->a01y*pc->g21+
             ppi->a02y*pc->g31+
             ppi->a03y*pc->g41;
    ppt.a01y=ppi->a00y*pc->g12+
             ppi->a01y*pc->g22+
             ppi->a02y*pc->g32+
             ppi->a03y*pc->g42;
    ppt.a02y=ppi->a00y*pc->g13+
             ppi->a01y*pc->g23+
             ppi->a02y*pc->g33+
             ppi->a03y*pc->g43;
    ppt.a03y=ppi->a00y*pc->g14+
             ppi->a01y*pc->g24+
             ppi->a02y*pc->g34+
             ppi->a03y*pc->g44;

    ppt.a10y=ppi->a10y*pc->g11+ 
             ppi->a11y*pc->g21+
             ppi->a12y*pc->g31+
             ppi->a13y*pc->g41;
    ppt.a11y=ppi->a10y*pc->g12+
             ppi->a11y*pc->g22+
             ppi->a12y*pc->g32+
             ppi->a13y*pc->g42;
    ppt.a12y=ppi->a10y*pc->g13+
             ppi->a11y*pc->g23+
             ppi->a12y*pc->g33+
             ppi->a13y*pc->g43;
    ppt.a13y=ppi->a10y*pc->g14+
             ppi->a11y*pc->g24+
             ppi->a12y*pc->g34+
             ppi->a13y*pc->g44;

    ppt.a20y=ppi->a20y*pc->g11+ 
             ppi->a21y*pc->g21+
             ppi->a22y*pc->g31+
             ppi->a23y*pc->g41;
    ppt.a21y=ppi->a20y*pc->g12+
             ppi->a21y*pc->g22+
             ppi->a22y*pc->g32+
             ppi->a23y*pc->g42;
    ppt.a22y=ppi->a20y*pc->g13+
             ppi->a21y*pc->g23+
             ppi->a22y*pc->g33+
             ppi->a23y*pc->g43;
    ppt.a23y=ppi->a20y*pc->g14+
             ppi->a21y*pc->g24+
             ppi->a22y*pc->g34+
             ppi->a23y*pc->g44;

    ppt.a30y=ppi->a30y*pc->g11+ 
             ppi->a31y*pc->g21+
             ppi->a32y*pc->g31+
             ppi->a33y*pc->g41;
    ppt.a31y=ppi->a30y*pc->g12+
             ppi->a31y*pc->g22+
             ppi->a32y*pc->g32+
             ppi->a33y*pc->g42;
    ppt.a32y=ppi->a30y*pc->g13+
             ppi->a31y*pc->g23+
             ppi->a32y*pc->g33+
             ppi->a33y*pc->g43;
    ppt.a33y=ppi->a30y*pc->g14+
             ppi->a31y*pc->g24+
             ppi->a32y*pc->g34+
             ppi->a33y*pc->g44;


/*! The Z elements     t00z, t01z, ..... t33z                      !*/

    ppt.a00z=ppi->a00z*pc->g11+ 
             ppi->a01z*pc->g21+
             ppi->a02z*pc->g31+
             ppi->a03z*pc->g41;
    ppt.a01z=ppi->a00z*pc->g12+
             ppi->a01z*pc->g22+
             ppi->a02z*pc->g32+
             ppi->a03z*pc->g42;
    ppt.a02z=ppi->a00z*pc->g13+
             ppi->a01z*pc->g23+
             ppi->a02z*pc->g33+
             ppi->a03z*pc->g43;
    ppt.a03z=ppi->a00z*pc->g14+
             ppi->a01z*pc->g24+
             ppi->a02z*pc->g34+
             ppi->a03z*pc->g44;

    ppt.a10z=ppi->a10z*pc->g11+ 
             ppi->a11z*pc->g21+
             ppi->a12z*pc->g31+
             ppi->a13z*pc->g41;
    ppt.a11z=ppi->a10z*pc->g12+
             ppi->a11z*pc->g22+
             ppi->a12z*pc->g32+
             ppi->a13z*pc->g42;
    ppt.a12z=ppi->a10z*pc->g13+
             ppi->a11z*pc->g23+
             ppi->a12z*pc->g33+
             ppi->a13z*pc->g43;
    ppt.a13z=ppi->a10z*pc->g14+
             ppi->a11z*pc->g24+
             ppi->a12z*pc->g34+
             ppi->a13z*pc->g44;

    ppt.a20z=ppi->a20z*pc->g11+ 
             ppi->a21z*pc->g21+
             ppi->a22z*pc->g31+
             ppi->a23z*pc->g41;
    ppt.a21z=ppi->a20z*pc->g12+
             ppi->a21z*pc->g22+
             ppi->a22z*pc->g32+
             ppi->a23z*pc->g42;
    ppt.a22z=ppi->a20z*pc->g13+
             ppi->a21z*pc->g23+
             ppi->a22z*pc->g33+
             ppi->a23z*pc->g43;
    ppt.a23z=ppi->a20z*pc->g14+
             ppi->a21z*pc->g24+
             ppi->a22z*pc->g34+
             ppi->a23z*pc->g44;

    ppt.a30z=ppi->a30z*pc->g11+ 
             ppi->a31z*pc->g21+
             ppi->a32z*pc->g31+
             ppi->a33z*pc->g41;
    ppt.a31z=ppi->a30z*pc->g12+
             ppi->a31z*pc->g22+
             ppi->a32z*pc->g32+
             ppi->a33z*pc->g42;
    ppt.a32z=ppi->a30z*pc->g13+
             ppi->a31z*pc->g23+
             ppi->a32z*pc->g33+
             ppi->a33z*pc->g43;
    ppt.a33z=ppi->a30z*pc->g14+
             ppi->a31z*pc->g24+
             ppi->a32z*pc->g34+
             ppi->a33z*pc->g44;


/*! The w elements     t00 , t01 , ..... t33                       !*/

    ppt.a00 =ppi->a00 *pc->g11+ 
             ppi->a01 *pc->g21+
             ppi->a02 *pc->g31+
             ppi->a03 *pc->g41;
    ppt.a01 =ppi->a00 *pc->g12+
             ppi->a01 *pc->g22+
             ppi->a02 *pc->g32+
             ppi->a03 *pc->g42;
    ppt.a02 =ppi->a00 *pc->g13+
             ppi->a01 *pc->g23+
             ppi->a02 *pc->g33+
             ppi->a03 *pc->g43;
    ppt.a03 =ppi->a00 *pc->g14+
             ppi->a01 *pc->g24+
             ppi->a02 *pc->g34+
             ppi->a03 *pc->g44;

    ppt.a10 =ppi->a10 *pc->g11+ 
             ppi->a11 *pc->g21+
             ppi->a12 *pc->g31+
             ppi->a13 *pc->g41;
    ppt.a11 =ppi->a10 *pc->g12+
             ppi->a11 *pc->g22+
             ppi->a12 *pc->g32+
             ppi->a13 *pc->g42;
    ppt.a12 =ppi->a10 *pc->g13+
             ppi->a11 *pc->g23+
             ppi->a12 *pc->g33+
             ppi->a13 *pc->g43;
    ppt.a13 =ppi->a10 *pc->g14+
             ppi->a11 *pc->g24+
             ppi->a12 *pc->g34+
             ppi->a13 *pc->g44;

    ppt.a20 =ppi->a20 *pc->g11+ 
             ppi->a21 *pc->g21+
             ppi->a22 *pc->g31+
             ppi->a23 *pc->g41;
    ppt.a21 =ppi->a20 *pc->g12+
             ppi->a21 *pc->g22+
             ppi->a22 *pc->g32+
             ppi->a23 *pc->g42;
    ppt.a22 =ppi->a20 *pc->g13+
             ppi->a21 *pc->g23+
             ppi->a22 *pc->g33+
             ppi->a23 *pc->g43;
    ppt.a23 =ppi->a20 *pc->g14+
             ppi->a21 *pc->g24+
             ppi->a22 *pc->g34+
             ppi->a23 *pc->g44;

    ppt.a30 =ppi->a30 *pc->g11+ 
             ppi->a31 *pc->g21+
             ppi->a32 *pc->g31+
             ppi->a33 *pc->g41;
    ppt.a31 =ppi->a30 *pc->g12+
             ppi->a31 *pc->g22+
             ppi->a32 *pc->g32+
             ppi->a33 *pc->g42;
    ppt.a32 =ppi->a30 *pc->g13+
             ppi->a31 *pc->g23+
             ppi->a32 *pc->g33+
             ppi->a33 *pc->g43;
    ppt.a33 =ppi->a30 *pc->g14+
             ppi->a31 *pc->g24+
             ppi->a32 *pc->g34+
             ppi->a33 *pc->g44;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"sur651 Temporary tensor \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a00x,ppt.a01x,ppt.a02x,ppt.a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a10x,ppt.a11x,ppt.a12x,ppt.a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a20x,ppt.a21x,ppt.a22x,ppt.a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a30x,ppt.a31x,ppt.a32x,ppt.a33x);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a00y,ppt.a01y,ppt.a02y,ppt.a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a10y,ppt.a11y,ppt.a12y,ppt.a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a20y,ppt.a21y,ppt.a22y,ppt.a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a30y,ppt.a31y,ppt.a32y,ppt.a33y);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a00z,ppt.a01z,ppt.a02z,ppt.a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a10z,ppt.a11z,ppt.a12z,ppt.a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a20z,ppt.a21z,ppt.a22z,ppt.a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a30z,ppt.a31z,ppt.a32z,ppt.a33z);

  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a00 ,ppt.a01 ,ppt.a02 ,ppt.a03 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a10 ,ppt.a11 ,ppt.a12 ,ppt.a13 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a20 ,ppt.a21 ,ppt.a22 ,ppt.a23 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppt.a30 ,ppt.a31 ,ppt.a32 ,ppt.a33 );

  }
#endif

/*!                                                                 */
/* 3. Output tensor                                                 */
/* ________________                                                 */
/*                                                                 !*/

/*! Tensor elements ppt to the output tensor ppo.                   */
/*  Note that elements in input tensor ppi will                     */
/*  be changed if pointers ppo and ppi are equal.                  !*/

    ppo->a00x = ppt.a00x;
    ppo->a01x = ppt.a01x;
    ppo->a02x = ppt.a02x;
    ppo->a03x = ppt.a03x;
    ppo->a10x = ppt.a10x;
    ppo->a11x = ppt.a11x;
    ppo->a12x = ppt.a12x;
    ppo->a13x = ppt.a13x;
    ppo->a20x = ppt.a20x;
    ppo->a21x = ppt.a21x;
    ppo->a22x = ppt.a22x;
    ppo->a23x = ppt.a23x;
    ppo->a30x = ppt.a30x;
    ppo->a31x = ppt.a31x;
    ppo->a32x = ppt.a32x;
    ppo->a33x = ppt.a33x;

    ppo->a00y = ppt.a00y;
    ppo->a01y = ppt.a01y;
    ppo->a02y = ppt.a02y;
    ppo->a03y = ppt.a03y;
    ppo->a10y = ppt.a10y;
    ppo->a11y = ppt.a11y;
    ppo->a12y = ppt.a12y;
    ppo->a13y = ppt.a13y;
    ppo->a20y = ppt.a20y;
    ppo->a21y = ppt.a21y;
    ppo->a22y = ppt.a22y;
    ppo->a23y = ppt.a23y;
    ppo->a30y = ppt.a30y;
    ppo->a31y = ppt.a31y;
    ppo->a32y = ppt.a32y;
    ppo->a33y = ppt.a33y;

    ppo->a00z = ppt.a00z;
    ppo->a01z = ppt.a01z;
    ppo->a02z = ppt.a02z;
    ppo->a03z = ppt.a03z;
    ppo->a10z = ppt.a10z;
    ppo->a11z = ppt.a11z;
    ppo->a12z = ppt.a12z;
    ppo->a13z = ppt.a13z;
    ppo->a20z = ppt.a20z;
    ppo->a21z = ppt.a21z;
    ppo->a22z = ppt.a22z;
    ppo->a23z = ppt.a23z;
    ppo->a30z = ppt.a30z;
    ppo->a31z = ppt.a31z;
    ppo->a32z = ppt.a32z;
    ppo->a33z = ppt.a33z;

    ppo->a00  = ppt.a00 ;
    ppo->a01  = ppt.a01 ;
    ppo->a02  = ppt.a02 ;
    ppo->a03  = ppt.a03 ;
    ppo->a10  = ppt.a10 ;
    ppo->a11  = ppt.a11 ;
    ppo->a12  = ppt.a12 ;
    ppo->a13  = ppt.a13 ;
    ppo->a20  = ppt.a20 ;
    ppo->a21  = ppt.a21 ;
    ppo->a22  = ppt.a22 ;
    ppo->a23  = ppt.a23 ;
    ppo->a30  = ppt.a30 ;
    ppo->a31  = ppt.a31 ;
    ppo->a32  = ppt.a32 ;
    ppo->a33  = ppt.a33 ;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur651 Output tensor \n");
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a00x,ppo->a01x,ppo->a02x,ppo->a03x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a10x,ppo->a11x,ppo->a12x,ppo->a13x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a20x,ppo->a21x,ppo->a22x,ppo->a23x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a30x,ppo->a31x,ppo->a32x,ppo->a33x);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a00y,ppo->a01y,ppo->a02y,ppo->a03y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a10y,ppo->a11y,ppo->a12y,ppo->a13y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a20y,ppo->a21y,ppo->a22y,ppo->a23y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a30y,ppo->a31y,ppo->a32y,ppo->a33y);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a00z,ppo->a01z,ppo->a02z,ppo->a03z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a10z,ppo->a11z,ppo->a12z,ppo->a13z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a20z,ppo->a21z,ppo->a22z,ppo->a23z);
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a30z,ppo->a31z,ppo->a32z,ppo->a33z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a00 ,ppo->a01 ,ppo->a02 ,ppo->a03 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a10 ,ppo->a11 ,ppo->a12 ,ppo->a13 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a20 ,ppo->a21 ,ppo->a22 ,ppo->a23 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   ppo->a30 ,ppo->a31 ,ppo->a32 ,ppo->a33 );
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
  "sur651 Exit***varkon_tensmult_r **** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
