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
/*  Function: varkon_pat_bictra                    File: sur620.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a bicubic patch with the input transformation matrix */
/*  (the input coordinate system).                                  */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-05   Originally written                                 */
/*  1996-05-28   Deleted status                                     */
/*  1997-04-19   Debug                                              */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_bictra     Transform bicubic patch          */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_bictra (sur620)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_bictra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATC *ppi,           /* Pointer to the input  patch             */
  DBTmat *pc,            /* Local coordinate system                 */
  GMPATC *ppo )          /* Pointer to the output patch             */

/* Out:                                                             */
/*        Coefficients of the transformed patch                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATC  ppt;          /* Transformed (temporary used) patch      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coefficients for the transformed matrix is given by:         */
/*                                                                  */
/*  TX (transf. X coeff)  = G (transf. matrix) * AX (X coeff.)      */
/*  TY (transf. Y coeff)  = G (transf. matrix) * AY (Y coeff.)      */
/*  TZ (transf. Z coeff)  = G (transf. matrix) * AZ (Z coeff.)      */
/*                                                                  */
/*  TX = G * AX with all matrix elements:                           */
/*                                                                  */
/*  !t00x t01x t02x t03x! !g11 g12 g13 g14! !a00x a01x a02x a03x!   */
/*  !t10x t11x t12x t13x!=!g21 g22 g23 g24!*!a00y a01y a02y a03y!   */
/*  !t20x t21x t22x t23x! !g31 g32 g33 g34! !a00z a01z a02z a03z!   */
/*  !t30x t31x t32x t33x! !g41 g42 g43 g44! !a00  a01  a02  a03 !   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_pat_bictra (sur620) ********\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix pc.  Error SU2993 if not defined    */
/*                                                                 !*/

if ( pc == NULL )
 {
 sprintf(errbuf, "(pc)%%varkon_pat_bictra (sur620");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* 2. Transform the coefficients for the patch                      */
/* ___________________________________________                      */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
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
#endif

/*!                                                                 */
/*  The X coefficients t00x, t01x, ..... t33x  (patch ppt)          */
/*                                                                 !*/

    ppt.a00x=ppi->a00x*pc->g11+          /*  Coefficient t00x =     */
             ppi->a00y*pc->g12+          /*  a00x*g11+a00y*g12+     */
             ppi->a00z*pc->g13+          /*  a00z*g13+ 1.0*g14      */
                1.0  *pc->g14;           /*                         */
    ppt.a01x=ppi->a01x*pc->g11+          /*  Coefficient t01x =     */
             ppi->a01y*pc->g12+          /*  a01x*g11+a01y*g12+     */
             ppi->a01z*pc->g13+          /*  a01z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a02x=ppi->a02x*pc->g11+          /*  Coefficient t02x =     */
             ppi->a02y*pc->g12+          /*  a02x*g11+a02y*g12+     */
             ppi->a02z*pc->g13+          /*  a02z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a03x=ppi->a03x*pc->g11+          /*  Coefficient t03x =     */
             ppi->a03y*pc->g12+          /*  a03x*g11+a03y*g12+     */
             ppi->a03z*pc->g13+          /*  a03z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a10x=ppi->a10x*pc->g11+          /*  Coefficient t10x =     */
             ppi->a10y*pc->g12+          /*  a10x*g11+a10y*g12+     */
             ppi->a10z*pc->g13+          /*  a10z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a11x=ppi->a11x*pc->g11+          /*  Coefficient t11x =     */
             ppi->a11y*pc->g12+          /*  a11x*g11+a11y*g12+     */
             ppi->a11z*pc->g13+          /*  a11z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a12x=ppi->a12x*pc->g11+          /*  Coefficient t12x =     */
             ppi->a12y*pc->g12+          /*  a12x*g11+a12y*g12+     */
             ppi->a12z*pc->g13+          /*  a12z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a13x=ppi->a13x*pc->g11+          /*  Coefficient t13x =     */
             ppi->a13y*pc->g12+          /*  a13x*g11+a13y*g12+     */
             ppi->a13z*pc->g13+          /*  a13z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a20x=ppi->a20x*pc->g11+          /*  Coefficient t20x =     */
             ppi->a20y*pc->g12+          /*  a20x*g11+a20y*g12+     */
             ppi->a20z*pc->g13+          /*  a20z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a21x=ppi->a21x*pc->g11+          /*  Coefficient t21x =     */
             ppi->a21y*pc->g12+          /*  a21x*g11+a21y*g12+     */
             ppi->a21z*pc->g13+          /*  a21z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a22x=ppi->a22x*pc->g11+          /*  Coefficient t22x =     */
             ppi->a22y*pc->g12+          /*  a22x*g11+a22y*g12+     */
             ppi->a22z*pc->g13+          /*  a22z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a23x=ppi->a23x*pc->g11+          /*  Coefficient t23x =     */
             ppi->a23y*pc->g12+          /*  a23x*g11+a23y*g12+     */
             ppi->a23z*pc->g13+          /*  a23z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a30x=ppi->a30x*pc->g11+          /*  Coefficient t30x =     */
             ppi->a30y*pc->g12+          /*  a30x*g11+a30y*g12+     */
             ppi->a30z*pc->g13+          /*  a30z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a31x=ppi->a31x*pc->g11+          /*  Coefficient t31x =     */
             ppi->a31y*pc->g12+          /*  a31x*g11+a31y*g12+     */
             ppi->a31z*pc->g13+          /*  a31z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a32x=ppi->a32x*pc->g11+          /*  Coefficient t32x =     */
             ppi->a32y*pc->g12+          /*  a32x*g11+a32y*g12+     */
             ppi->a32z*pc->g13+          /*  a32z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */
    ppt.a33x=ppi->a33x*pc->g11+          /*  Coefficient t33x =     */
             ppi->a33y*pc->g12+          /*  a33x*g11+a33y*g12+     */
             ppi->a33z*pc->g13+          /*  a33z*g13+ 0.0*g14      */
                0.0  *pc->g14;           /*                         */

/*!                                                                 */
/*  The Y coefficients t00y, t01y, ..... t33y                       */
/*                                                                 !*/

    ppt.a00y=ppi->a00x*pc->g21+          /*  Coefficient t00y =...  */
             ppi->a00y*pc->g22+          /*                         */
             ppi->a00z*pc->g23+          /*                         */
                1.0  *pc->g24;           /*                         */
    ppt.a01y=ppi->a01x*pc->g21+          /*  Coefficient t01y =...  */
             ppi->a01y*pc->g22+          /*                         */
             ppi->a01z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a02y=ppi->a02x*pc->g21+          /*  Coefficient t02y =...  */
             ppi->a02y*pc->g22+          /*                         */
             ppi->a02z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a03y=ppi->a03x*pc->g21+          /*  Coefficient t03y =...  */
             ppi->a03y*pc->g22+          /*                         */
             ppi->a03z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a10y=ppi->a10x*pc->g21+          /*  Coefficient t10y =...  */
             ppi->a10y*pc->g22+          /*                         */
             ppi->a10z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a11y=ppi->a11x*pc->g21+          /*  Coefficient t11y =...  */
             ppi->a11y*pc->g22+          /*                         */
             ppi->a11z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a12y=ppi->a12x*pc->g21+          /*  Coefficient t12y =...  */
             ppi->a12y*pc->g22+          /*                         */
             ppi->a12z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a13y=ppi->a13x*pc->g21+          /*  Coefficient t13y =...  */
             ppi->a13y*pc->g22+          /*                         */
             ppi->a13z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a20y=ppi->a20x*pc->g21+          /*  Coefficient t20y =...  */
             ppi->a20y*pc->g22+          /*                         */
             ppi->a20z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a21y=ppi->a21x*pc->g21+          /*  Coefficient t21y =...  */
             ppi->a21y*pc->g22+          /*                         */
             ppi->a21z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a22y=ppi->a22x*pc->g21+          /*  Coefficient t22y =...  */
             ppi->a22y*pc->g22+          /*                         */
             ppi->a22z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a23y=ppi->a23x*pc->g21+          /*  Coefficient t23y =...  */
             ppi->a23y*pc->g22+          /*                         */
             ppi->a23z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a30y=ppi->a30x*pc->g21+          /*  Coefficient t30y =...  */
             ppi->a30y*pc->g22+          /*                         */
             ppi->a30z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a31y=ppi->a31x*pc->g21+          /*  Coefficient t31y =...  */
             ppi->a31y*pc->g22+          /*                         */
             ppi->a31z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a32y=ppi->a32x*pc->g21+          /*  Coefficient t32y =...  */
             ppi->a32y*pc->g22+          /*                         */
             ppi->a32z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */
    ppt.a33y=ppi->a33x*pc->g21+          /*  Coefficient t33y =...  */
             ppi->a33y*pc->g22+          /*                         */
             ppi->a33z*pc->g23+          /*                         */
                0.0  *pc->g24;           /*                         */

/*!                                                                 */
/*  The Z coefficients t00z, t01z, ..... t33z                       */
/*                                                                 !*/

    ppt.a00z=ppi->a00x*pc->g31+          /*  Coefficient t00z =...  */
             ppi->a00y*pc->g32+          /*                         */
             ppi->a00z*pc->g33+          /*                         */
                1.0  *pc->g34;           /*                         */
    ppt.a01z=ppi->a01x*pc->g31+          /*  Coefficient t01z =...  */
             ppi->a01y*pc->g32+          /*                         */
             ppi->a01z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a02z=ppi->a02x*pc->g31+          /*  Coefficient t02z =...  */
             ppi->a02y*pc->g32+          /*                         */
             ppi->a02z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a03z=ppi->a03x*pc->g31+          /*  Coefficient t03z =...  */
             ppi->a03y*pc->g32+          /*                         */
             ppi->a03z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a10z=ppi->a10x*pc->g31+          /*  Coefficient t10z =...  */
             ppi->a10y*pc->g32+          /*                         */
             ppi->a10z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a11z=ppi->a11x*pc->g31+          /*  Coefficient t11z =...  */
             ppi->a11y*pc->g32+          /*                         */
             ppi->a11z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a12z=ppi->a12x*pc->g31+          /*  Coefficient t12z =...  */
             ppi->a12y*pc->g32+          /*                         */
             ppi->a12z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a13z=ppi->a13x*pc->g31+          /*  Coefficient t13z =...  */
             ppi->a13y*pc->g32+          /*                         */
             ppi->a13z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a20z=ppi->a20x*pc->g31+          /*  Coefficient t20z =...  */
             ppi->a20y*pc->g32+          /*                         */
             ppi->a20z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a21z=ppi->a21x*pc->g31+          /*  Coefficient t21z =...  */
             ppi->a21y*pc->g32+          /*                         */
             ppi->a21z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a22z=ppi->a22x*pc->g31+          /*  Coefficient t22z =...  */
             ppi->a22y*pc->g32+          /*                         */
             ppi->a22z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a23z=ppi->a23x*pc->g31+          /*  Coefficient t23z =...  */
             ppi->a23y*pc->g32+          /*                         */
             ppi->a23z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a30z=ppi->a30x*pc->g31+          /*  Coefficient t30z =...  */
             ppi->a30y*pc->g32+          /*                         */
             ppi->a30z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a31z=ppi->a31x*pc->g31+          /*  Coefficient t31z =...  */
             ppi->a31y*pc->g32+          /*                         */
             ppi->a31z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a32z=ppi->a32x*pc->g31+          /*  Coefficient t32z =...  */
             ppi->a32y*pc->g32+          /*                         */
             ppi->a32z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */
    ppt.a33z=ppi->a33x*pc->g31+          /*  Coefficient t33z =...  */
             ppi->a33y*pc->g32+          /*                         */
             ppi->a33z*pc->g33+          /*                         */
                0.0  *pc->g34;           /*                         */

    ppt.ofs_pat = ppi->ofs_pat;          /* Offset for patch        */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
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

  }
#endif

/*!                                                                 */
/*  Patch coefficients to output patch. (Patch ppt to patch ppo)    */
/*                                                                 !*/

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

    ppo->ofs_pat = ppt.ofs_pat;          /* Offset for patch        */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
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
#endif

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur620 Exit ****** varkon_pat_bictra ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
