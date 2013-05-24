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
/*  Function: varkon_cur_ratevalh                  File: sur780.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function evaluates homogenous coordinates and               */
/*  derivatives for a rational cubic segment.                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1993-10-03   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_ratevalh   Homogen. coord. and derivatives  */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/*  Input cubic rational segment (homogenus coordinates):           */
/*                                                                  */
/*                 2   3                                            */
/*  P(u)   = (1,u,u , u ) * ! C0 !                                  */
/*                          ! C1 !                                  */
/*                          ! C2 !                                  */
/*                          ! C3 !                                  */
/*                                                                  */
/*                       2                                          */
/*  dP/du  = (0,1,2*u,3*u )*! C0 !                                  */
/*                          ! C1 !                                  */
/*                          ! C2 !                                  */
/*                          ! C3 !                                  */
/*                                                                  */
/*                                                                  */
/*  d2P/du2= (0,0, 2 ,6*u) *! C0 !                                  */
/*                          ! C1 !                                  */
/*                          ! C2 !                                  */
/*                          ! C3 !                                  */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_cur_ratevalh        Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus   varkon_cur_ratevalh(

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSeg   *pseg,         /* Pointer to input  rational segment      */
  DBfloat  u_s,          /* Parameter value for segment             */
  EVALCH  *pout )        /* Pointer to homogen. coordinates and     */
                         /* derivatives                             */

/* Out:                                                             */
/*        Coefficients and other segment data in prout              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*------------end-of-declarations-----------------------------------*/


/*! Algorithm                                                      !*/
/*! =========                                                      !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur780 Enter *** varkon_cur_ratevalh **Homogen. coord. & deriv.*\n");
}
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur780 u_s   %f \n",
   u_s    ) ;
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */

/*  There are no checks in this function                            */
/*                                                                 !*/

/*!                                                                 */
/*  2. Calculate derivatives                                        */
/*  ________________________                                        */

/*                                                                 !*/

/*!                                                                 */
/*  Output P(u)   = (pout.R_X, pout.R_Y, pout.R_Z, pout.R_w)        */
/*                                                                 !*/

    pout->R_X = (*pseg).c0x              + 
                (*pseg).c1x*u_s          + 
                (*pseg).c2x*u_s*u_s      +
                (*pseg).c3x*u_s*u_s*u_s;

    pout->R_Y = (*pseg).c0y              + 
                (*pseg).c1y*u_s          + 
                (*pseg).c2y*u_s*u_s      +
                (*pseg).c3y*u_s*u_s*u_s;

    pout->R_Z = (*pseg).c0z              + 
                (*pseg).c1z*u_s          + 
                (*pseg).c2z*u_s*u_s      +
                (*pseg).c3z*u_s*u_s*u_s;

    pout->R_w = (*pseg).c0               + 
                (*pseg).c1 *u_s          + 
                (*pseg).c2 *u_s*u_s      +
                (*pseg).c3 *u_s*u_s*u_s;

/*!                                                                 */
/*  Output dP/dU  = (pout.U_X, pout.U_Y, pout.U_Z, pout.U_w)        */
/*                                                                 !*/

    pout->U_X = (*pseg).c1x              + 
                (*pseg).c2x*2.0*u_s      +
                (*pseg).c3x*3.0*u_s*u_s;

    pout->U_Y = (*pseg).c1y              + 
                (*pseg).c2y*2.0*u_s      +
                (*pseg).c3y*3.0*u_s*u_s;

    pout->U_Z = (*pseg).c1z              + 
                (*pseg).c2z*2.0*u_s      +
                (*pseg).c3z*3.0*u_s*u_s;

    pout->U_w = (*pseg).c1               + 
                (*pseg).c2 *2.0*u_s      +
                (*pseg).c3 *3.0*u_s*u_s;

/*!                                                                 */
/*  Output dP2/du2= (pout.U2_X, pout.U2_Y, pout.U2_Z, pout.U2_w)    */
/*                                                                 !*/


    pout->U2_X= (*pseg).c2x*2.0          +
                (*pseg).c3x*6.0*u_s;    

    pout->U2_Y= (*pseg).c2y*2.0          +
                (*pseg).c3y*6.0*u_s;    

    pout->U2_Z= (*pseg).c2z*2.0          +
                (*pseg).c3z*6.0*u_s;    

    pout->U2_w= (*pseg).c2 *2.0          +
                (*pseg).c3 *6.0*u_s;    


    return(SUCCED);

} /* End of function */


/********************************************************************/
