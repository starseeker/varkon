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
/*  Function: varkon_detmat                      File: sur661.c     */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculates the determinant of a matrix (a 4X4 matrix)           */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1993-09-26   Originally written                                 */
/*  1996-05-28   Erased errbuf                                      */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_detmat         Determinant of a 4X4 matrix      */
/*                                                              */
/*--------------------------------------------------------------*/

/*--------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_detmat     (sur661) Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_detmat (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBTmat   *pm,          /* Pointer to the matrix  m                */
  DBfloat  *p_deter )    /* Pointer to the determinant              */

/* Out:                                                             */
/*        The derminant value to p_deter                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  det_g11;      /* Sub determinant to element g11          */
  DBfloat  det_g12;      /* Sub determinant to element g12          */
  DBfloat  det_g13;      /* Sub determinant to element g13          */
  DBfloat  det_g14;      /* Sub determinant to element g14          */
  DBfloat  deter;        /* The determinant value                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
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
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_detmat (sur661) ********\n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur661 Input matrix m : \n");
 
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm->g11 ,pm->g12 ,pm->g13 ,pm->g14 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm->g21 ,pm->g22 ,pm->g23 ,pm->g24 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm->g31 ,pm->g32 ,pm->g33 ,pm->g34 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm->g41 ,pm->g42 ,pm->g43 ,pm->g44 );
  }
#endif

/*!                                                                 */
/* 2. Calculate the determinant                                     */
/* ____________________________                                     */
/*                                                                 !*/

/*! Determinant det_g11 for element g11                            !*/

det_g11 = (*pm).g22 * (*pm).g33 * (*pm).g44 +
       (*pm).g23 * (*pm).g34 * (*pm).g42 +
       (*pm).g24 * (*pm).g32 * (*pm).g43 +
(-1.0)*(*pm).g22 * (*pm).g34 * (*pm).g43 +
(-1.0)*(*pm).g23 * (*pm).g32 * (*pm).g44 +
(-1.0)*(*pm).g24 * (*pm).g33 * (*pm).g42;

 det_g11 =  (*pm).g11 * det_g11;


/*! Determinant det_g12 for element g12                            !*/

det_g12 = (*pm).g21 * (*pm).g33 * (*pm).g44 +
       (*pm).g23 * (*pm).g34 * (*pm).g41 +
       (*pm).g24 * (*pm).g31 * (*pm).g43 +
(-1.0)*(*pm).g21 * (*pm).g34 * (*pm).g43 +
(-1.0)*(*pm).g23 * (*pm).g31 * (*pm).g44 +
(-1.0)*(*pm).g24 * (*pm).g33 * (*pm).g41;

 det_g12 = (-1.0)*(*pm).g12 * det_g12;


/*! Determinant det_g13 for element g13                            !*/

det_g13 = (*pm).g21 * (*pm).g32 * (*pm).g44 +
       (*pm).g22 * (*pm).g34 * (*pm).g41 +
       (*pm).g24 * (*pm).g31 * (*pm).g42 +
(-1.0)*(*pm).g21 * (*pm).g34 * (*pm).g42 +
(-1.0)*(*pm).g22 * (*pm).g31 * (*pm).g44 +
(-1.0)*(*pm).g24 * (*pm).g32 * (*pm).g41;

 det_g13 =  (*pm).g13 * det_g13;


/*! Determinant det_g14 for element g14                            !*/

det_g14 = (*pm).g22 * (*pm).g33 * (*pm).g41 +
       (*pm).g23 * (*pm).g31 * (*pm).g42 +
       (*pm).g21 * (*pm).g32 * (*pm).g43 +
(-1.0)*(*pm).g22 * (*pm).g31 * (*pm).g43 +
(-1.0)*(*pm).g23 * (*pm).g32 * (*pm).g41 +
(-1.0)*(*pm).g21 * (*pm).g33 * (*pm).g42;

 det_g14 = (-1.0)*(*pm).g14 * det_g14;


 deter = det_g11 + det_g12 + det_g13 + det_g14;


/*!                                                                 */
/* 3. Output determinant value                                      */
/* ___________________________                                      */
/*                                                                 !*/

 *p_deter = deter;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"det_g11 %f _g12 %f _g13 %f _g14 %f \n",
   det_g11 ,det_g12 , det_g13 , det_g14 );
  }
#endif
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Exit *** varkon_detmat (sur661) Determinant= %f \n",*p_deter);
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
