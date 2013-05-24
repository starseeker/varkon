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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_seg_parab_c                   File: sur702.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a parabola from one point, one tangent     */
/*  and one second derivative vector                                */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-02-24   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_seg_parab_c    Parabola: pt+tangent+2nd deriv.  */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* A parabola defined by point, tangent and second derivative       */
/* ----------------------------------------------------------       */
/*                                                                  */
/*                                                                  */
/* The rational cubic curve in homogenous coordinates               */
/*                                                                  */
/* P(t) = c0 + c1*t + c2*t**2 + c3*t**3                             */
/*                                                                  */
/* For a parabola is c3= 0 and the denominator w= 1                 */
/*                                                                  */
/* r(t) = c0 + c1*t + c2*t**2                                       */
/*                                                                  */
/*                                                                  */
/* One point pa, one tangent ta and second derivative t2a are       */
/* given.                                                           */
/*                                                                  */
/* r (0.0) = c0                         = pa                        */
/* t (0.0)  =      c1                   = ta                        */
/* t2(0.0) =             2*c2           = t2a                       */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1                    = ta                                  */
/*               c2           = t2a/2                               */
/*                                                                  */
/*             _______________________________                      */
/*            !                               !                     */
/*            !   c0  =    pa                 !                     */
/*            !                               !                     */
/*            !   c1  =    ta                 !                     */
/*            !                               !                     */
/*            !   c2  =    t2a/2              !                     */
/*            !                               !                     */
/*            !_______________________________!                     */
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
/* SU2993 = Program error ( ) in varkon_seg_parab_c         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_seg_parab_c (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector pt_a,         /* Start point             for parabola    */
  DBVector tan_a,        /* Start tangent           for parabola    */
  DBVector der2_a,       /* Start second derivative for parabola    */
  DBSeg   *pseg )        /* Pointer to a rational cubic segment     */

/* Out:                                                             */
/*        Coefficients and other segment data in pseg               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  tan_len;      /* Input tangent length                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  char     errbuf[80];   /* String for error message fctn erpush    */

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
"sur702 Enter*varkon_seg_parab_c  **Parabola: pt+tangent+2'nd deriv\n");
}
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur702 pt_a  %f %f %f\n",pt_a.x_gm, pt_a.y_gm, pt_a.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur702 tan_a %f %f %f\n",tan_a.x_gm, tan_a.y_gm, tan_a.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur702 der2_a %f %f %f\n",der2_a.x_gm, der2_a.y_gm, der2_a.z_gm );
}
#endif
 
/*!                                                                 */
/* Calculate tangent length                                         */
/*                                                                 !*/

tan_len = sqrt( tan_a.x_gm*tan_a.x_gm+
                tan_a.y_gm*tan_a.y_gm*
                tan_a.z_gm*tan_a.z_gm);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur702 tan_len %f\n",tan_len );
  }
#endif


if (tan_len< TOL2  )                     /* Check distance tan_len  */
 {
 sprintf(errbuf, "(tang)%%varkon_seg_parab_c  sur702");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/*                                                                  */
/* 2. Creation of the parabola                                      */
/* ___________________________                                      */
/*                                                                  */
/*                                                                 !*/

   (*pseg).c0x =     pt_a.x_gm;
   (*pseg).c1x =    tan_a.x_gm;
   (*pseg).c2x =   der2_a.x_gm/2.0;
   (*pseg).c3x =  0.0;        

   (*pseg).c0y =     pt_a.y_gm;
   (*pseg).c1y =    tan_a.y_gm;
   (*pseg).c2y =   der2_a.y_gm/2.0; 
   (*pseg).c3y =  0.0;        

   (*pseg).c0z =     pt_a.z_gm;
   (*pseg).c1z =    tan_a.z_gm;
   (*pseg).c2z =   der2_a.z_gm/2.0;
   (*pseg).c3z =  0.0;        

/*! 3. Exit                                                         */
/*  _______                                                         */


/*!                                                                 */
/*  Denominator, offset, type CUB_SEG and arclength= 0              */
/*                                                                  */
/*                                                                 !*/
   (*pseg).c0  =  1.0;  
   (*pseg).c1  =  0.0;
   (*pseg).c2  =  0.0;
   (*pseg).c3  =  0.0;

   (*pseg).ofs = 0.0;
   (*pseg).typ = CUB_SEG ;
   (*pseg).sl  = 0.0;     

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur702 c0x %f c1x %f c2x %f c3x %f\n",
   (*pseg).c0x, (*pseg).c1x, (*pseg).c2x, (*pseg).c3x );
  fprintf(dbgfil(SURPAC),
  "sur702 c0y %f c1y %f c2y %f c3y %f\n",
   (*pseg).c0y, (*pseg).c1y, (*pseg).c2y, (*pseg).c3y );
  fprintf(dbgfil(SURPAC),
  "sur702 c0z %f c1z %f c2z %f c3z %f\n",
   (*pseg).c0z, (*pseg).c1z, (*pseg).c2z, (*pseg).c3z );
  fprintf(dbgfil(SURPAC),
  "sur702 c0  %f c1  %f c2  %f c3  %f\n",
   (*pseg).c0 , (*pseg).c1 , (*pseg).c2 , (*pseg).c3  );
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur702 Exit  *** varkon_seg_parab_c  *\n");
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
