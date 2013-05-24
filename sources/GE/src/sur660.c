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
/*  Function: varkon_invmat        SNAME: sur660 File: sur660.c     */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculates the inverse matrix (a 4X4 matrix)                    */
/*                                                                  */
/*  Remark:                                                         */
/*  There is an option to let the input matrix pointer be equal     */
/*  to the output matrix pointer. Only one matrix need to be        */
/*  declared in the calling function. But note that the input       */
/*  data of course will be destroyed in this case.                  */
/*                                                                  */
/*  TODO Function not fully tested                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1993-10-02   Originally written                                 */
/*  1996-05-28   Erased internal variable dum1                      */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_invmat         Invertation of a 4X4 matrix      */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_detmat     * Determinant of matrix                        */
/* varkon_erpush     * Error message to terminal                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_invmat     (sur660) Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_invmat ( 

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBTmat  *pm1,          /* Pointer to the matrix  m1               */
  DBTmat  *pm2,          /* Pointer to the inverse matrix  m2       */
  DBfloat *p_deter )     /* Pointer to the determinant              */

/* Out:                                                             */
/*        Matrix elements in pm2 (or in pm2) and determinant        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBTmat  ac;           /* The algebraic complement matrix         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat deter;        /* Determinant                             */
   short   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

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
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_invmat (sur660) ********\n");
  }
#endif
 

/*!                                                                 */
/* 2. Calculate the determinant                                     */
/* ____________________________                                     */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"sur660 Input matrix m1: \n");
 
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm1->g11 ,pm1->g12 ,pm1->g13 ,pm1->g14 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm1->g21 ,pm1->g22 ,pm1->g23 ,pm1->g24 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm1->g31 ,pm1->g32 ,pm1->g33 ,pm1->g34 );
  fprintf(dbgfil(SURPAC),"%f %f %f %f \n",
   pm1->g41 ,pm1->g42 ,pm1->g43 ,pm1->g44 );
  }
#endif

   status=varkon_detmat( pm1 , p_deter );
   if (status<0) 
        {
        sprintf(errbuf,"sur661%%varkon_invmat (sur660)");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 3. Albebraic complement matrix                                   */
/* _______________________________                                  */
/*                                                                 !*/

      /*  Element ac.g11                    */

      ac.g11 = (*pm1).g22 * (*pm1).g33 * (*pm1).g44 +
               (*pm1).g23 * (*pm1).g34 * (*pm1).g42 +
               (*pm1).g24 * (*pm1).g32 * (*pm1).g43 +
        (-1.0)*(*pm1).g22 * (*pm1).g34 * (*pm1).g43 +
        (-1.0)*(*pm1).g23 * (*pm1).g32 * (*pm1).g44 +
        (-1.0)*(*pm1).g24 * (*pm1).g33 * (*pm1).g42;

      ac.g11=   ac.g11;
      

      /* Element ac.g12                    */
      
      ac.g12 = (*pm1).g21 * (*pm1).g33 * (*pm1).g44 +
               (*pm1).g23 * (*pm1).g34 * (*pm1).g41 +
               (*pm1).g24 * (*pm1).g31 * (*pm1).g43 +
        (-1.0)*(*pm1).g21 * (*pm1).g34 * (*pm1).g43 +
        (-1.0)*(*pm1).g23 * (*pm1).g31 * (*pm1).g44 +
        (-1.0)*(*pm1).g24 * (*pm1).g33 * (*pm1).g41;

       ac.g12 = (-1.0)*ac.g12;


      /* Element ac.g13                   */

      ac.g13 = (*pm1).g21 * (*pm1).g32 * (*pm1).g44 +
               (*pm1).g22 * (*pm1).g34 * (*pm1).g41 +
               (*pm1).g24 * (*pm1).g31 * (*pm1).g42 +
        (-1.0)*(*pm1).g21 * (*pm1).g34 * (*pm1).g42 +
        (-1.0)*(*pm1).g22 * (*pm1).g31 * (*pm1).g44 +
        (-1.0)*(*pm1).g24 * (*pm1).g32 * (*pm1).g41;

       ac.g13 =   ac.g13;


      /* Element ac.g14                                            */

      ac.g14 = (*pm1).g22 * (*pm1).g33 * (*pm1).g41 +
               (*pm1).g23 * (*pm1).g31 * (*pm1).g42 +
               (*pm1).g21 * (*pm1).g32 * (*pm1).g43 +
        (-1.0)*(*pm1).g22 * (*pm1).g31 * (*pm1).g43 +
        (-1.0)*(*pm1).g23 * (*pm1).g32 * (*pm1).g41 +
        (-1.0)*(*pm1).g21 * (*pm1).g33 * (*pm1).g42;

       ac.g14 = (-1.0)*ac.g14;

      /*  Element ac.g21                    */

      ac.g21 = (*pm1).g12 * (*pm1).g33 * (*pm1).g44 +
               (*pm1).g13 * (*pm1).g34 * (*pm1).g42 +
               (*pm1).g14 * (*pm1).g32 * (*pm1).g43 +
        (-1.0)*(*pm1).g12 * (*pm1).g34 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g32 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g33 * (*pm1).g42;

      ac.g21=  (-1.0)*ac.g21;


      /* Element ac.g22                    */

      ac.g22 = (*pm1).g11 * (*pm1).g33 * (*pm1).g44 +
               (*pm1).g13 * (*pm1).g34 * (*pm1).g41 +
               (*pm1).g14 * (*pm1).g31 * (*pm1).g43 +
        (-1.0)*(*pm1).g11 * (*pm1).g34 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g31 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g33 * (*pm1).g41;

       ac.g22 =   ac.g22;


      /* Element ac.g23                   */

      ac.g23 = (*pm1).g11 * (*pm1).g32 * (*pm1).g44 +
               (*pm1).g12 * (*pm1).g34 * (*pm1).g41 +
               (*pm1).g14 * (*pm1).g31 * (*pm1).g42 +
        (-1.0)*(*pm1).g11 * (*pm1).g34 * (*pm1).g42 +
        (-1.0)*(*pm1).g12 * (*pm1).g31 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g32 * (*pm1).g41;

       ac.g23 =  (-1.0)*ac.g23;


      /* Element ac.g24                                            */

      ac.g24 = (*pm1).g11 * (*pm1).g32 * (*pm1).g43 +
               (*pm1).g12 * (*pm1).g33 * (*pm1).g41 +
               (*pm1).g13 * (*pm1).g31 * (*pm1).g42 +
        (-1.0)*(*pm1).g12 * (*pm1).g31 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g32 * (*pm1).g41 +
        (-1.0)*(*pm1).g11 * (*pm1).g33 * (*pm1).g42;

       ac.g24 =   ac.g24;

      /*  Element ac.g31                    */

      ac.g31 = (*pm1).g12 * (*pm1).g23 * (*pm1).g44 +
               (*pm1).g13 * (*pm1).g24 * (*pm1).g42 +
               (*pm1).g14 * (*pm1).g22 * (*pm1).g43 +
        (-1.0)*(*pm1).g12 * (*pm1).g24 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g22 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g23 * (*pm1).g42;

      ac.g31=   ac.g31;


      /* Element ac.g22                    */

      ac.g32 = (*pm1).g11 * (*pm1).g23 * (*pm1).g44 +
               (*pm1).g13 * (*pm1).g24 * (*pm1).g41 +
               (*pm1).g14 * (*pm1).g21 * (*pm1).g43 +
        (-1.0)*(*pm1).g11 * (*pm1).g24 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g21 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g23 * (*pm1).g41;

       ac.g32 = (-1.0)*ac.g32;


      /* Element ac.g33                   */

      ac.g33 = (*pm1).g11 * (*pm1).g22 * (*pm1).g44 +
               (*pm1).g12 * (*pm1).g24 * (*pm1).g41 +
               (*pm1).g14 * (*pm1).g21 * (*pm1).g42 +
        (-1.0)*(*pm1).g11 * (*pm1).g24 * (*pm1).g42 +
        (-1.0)*(*pm1).g12 * (*pm1).g21 * (*pm1).g44 +
        (-1.0)*(*pm1).g14 * (*pm1).g22 * (*pm1).g41;

       ac.g33 =   ac.g33;


      /* Element ac.g34                                            */

      ac.g34 = (*pm1).g12 * (*pm1).g23 * (*pm1).g41 +
               (*pm1).g13 * (*pm1).g21 * (*pm1).g42 +
               (*pm1).g11 * (*pm1).g22 * (*pm1).g43 +
        (-1.0)*(*pm1).g12 * (*pm1).g21 * (*pm1).g43 +
        (-1.0)*(*pm1).g13 * (*pm1).g22 * (*pm1).g41 +
        (-1.0)*(*pm1).g11 * (*pm1).g23 * (*pm1).g42;

       ac.g34 = (-1.0)*ac.g34;

      /*  Element ac.g41                    */

      ac.g41 = (*pm1).g12 * (*pm1).g23 * (*pm1).g34 +
               (*pm1).g13 * (*pm1).g24 * (*pm1).g32 +
               (*pm1).g14 * (*pm1).g22 * (*pm1).g33 +
        (-1.0)*(*pm1).g12 * (*pm1).g24 * (*pm1).g33 +
        (-1.0)*(*pm1).g13 * (*pm1).g22 * (*pm1).g34 +
        (-1.0)*(*pm1).g14 * (*pm1).g23 * (*pm1).g32;

      ac.g41=   (-1.0)*ac.g41;


      /* Element ac.g42                    */

      ac.g42 = (*pm1).g11 * (*pm1).g23 * (*pm1).g34 +
               (*pm1).g13 * (*pm1).g24 * (*pm1).g31 +
               (*pm1).g14 * (*pm1).g21 * (*pm1).g33 +
        (-1.0)*(*pm1).g11 * (*pm1).g24 * (*pm1).g33 +
        (-1.0)*(*pm1).g13 * (*pm1).g21 * (*pm1).g34 +
        (-1.0)*(*pm1).g14 * (*pm1).g23 * (*pm1).g31;

       ac.g42 =   ac.g42;


      /* Element ac.g43                   */

      ac.g43 = (*pm1).g11 * (*pm1).g22 * (*pm1).g34 +
               (*pm1).g12 * (*pm1).g24 * (*pm1).g31 +
               (*pm1).g14 * (*pm1).g21 * (*pm1).g32 +
        (-1.0)*(*pm1).g11 * (*pm1).g24 * (*pm1).g32 +
        (-1.0)*(*pm1).g12 * (*pm1).g21 * (*pm1).g34 +
        (-1.0)*(*pm1).g14 * (*pm1).g22 * (*pm1).g31;

       ac.g43 =   (-1.0)*ac.g43;


      /* Element ac.g44                                            */

      ac.g44 = (*pm1).g12 * (*pm1).g23 * (*pm1).g31 +
               (*pm1).g13 * (*pm1).g21 * (*pm1).g32 +
               (*pm1).g11 * (*pm1).g22 * (*pm1).g33 +
        (-1.0)*(*pm1).g12 * (*pm1).g21 * (*pm1).g33 +
        (-1.0)*(*pm1).g13 * (*pm1).g22 * (*pm1).g31 +
        (-1.0)*(*pm1).g11 * (*pm1).g23 * (*pm1).g32;

       ac.g44 = ac.g44;


/*!                                                                 */
/* 3. Output matrix                                                 */
/* ________________                                                 */
/*                                                                 !*/

/*! Matrix elements ppt to the output matrix pm2.                   */
/*  Note that elements in input tensor pm1 will                     */
/*  be changed if pointers pm1 and pm2 are equal.                  !*/

    deter    = *p_deter;
    pm2->g11 = ac.g11/deter;
    pm2->g12 = ac.g21/deter;
    pm2->g13 = ac.g31/deter;
    pm2->g14 = ac.g41/deter;
     
    pm2->g21 = ac.g12/deter;
    pm2->g22 = ac.g22/deter;
    pm2->g23 = ac.g32/deter;
    pm2->g24 = ac.g42/deter;

    pm2->g31 = ac.g13/deter;
    pm2->g32 = ac.g23/deter;
    pm2->g33 = ac.g33/deter;
    pm2->g34 = ac.g43/deter;

    pm2->g41 = ac.g14/deter;
    pm2->g42 = ac.g24/deter;
    pm2->g43 = ac.g34/deter;
    pm2->g44 = ac.g44/deter;

/*!                                                                 */
/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Exit ****** varkon_invmat     (sur660) ******* \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
