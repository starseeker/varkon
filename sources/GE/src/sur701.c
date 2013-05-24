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
/*  Function: varkon_seg_parab_b                   File: sur701.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function interpolates two points and one tangent            */
/*  with a parabola.                                                */
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
/*sdescr varkon_seg_parab_b    Parabola from 2 pts and 1 tangent*/
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* A parabola defined by two points and one tangent                 */
/* ------------------------------------------------                 */
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
/* Two points pa and pc and one tangent ta are given. The           */
/* parameter values are t_a= 0 and t_c= 1.                          */
/*                                                                  */
/* r(0.0) = c0                         = pa                         */
/* t(0.0) =      c1                    = ta                         */
/* r(1.0) = c0 + c1*1.0 + c2*1.0*1.0   = pc                         */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1                    = ta                                  */
/* pa + ta*1.0 + c2*1.0*1.0   = pc                                  */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1                    = ta                                  */
/*               c2           = pc - pa - ta                        */
/*                                                                  */
/*             _______________________________                      */
/*            !                               !                     */
/*            !   c0  =    pa                 !                     */
/*            !                               !                     */
/*            !   c1  =    ta                 !                     */
/*            !                               !                     */
/*            !   c2  =    pc - pa - ta       !                     */
/*            !                               !                     */
/*            !_______________________________!                     */
/*                                                                  */
/*                                                                  */
/* Case 1: Tangent length=  !pc-pa!                                 */
/* Case 2: Tangent length=  !pc-pa!*0.5                             */
/* Case 3: Input tangent length is used                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_seg_parab_b         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_seg_parab_b (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector pt_a,         /* Start point of parabola                 */
  DBVector tan_a,        /* Start tangent for parabola              */
  DBVector pt_c,         /* End   point of parabola                 */
  DBint    u_case,       /* Tangent length case:                    */
                         /* Eq. 1: Chord length  pt_ to pt_c        */
                         /* Eq. 2: Chord length (pt_ to pt_c)/2     */
                         /* Eq. 3: Input tangent length is used     */
  DBSeg   *pseg )        /* Pointer to a rational cubic segment     */

/* Out:                                                             */
/*        Coefficients and other segment data in pseg               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  cl_ac;        /* Chord length from pt_b to pt_c          */
  DBfloat  tan_len;      /* Input tangent length                    */
  DBVector tan_a_m;      /* Modified start tangent for parabola     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  dot;          /* Scalar product for chords               */
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
"sur701 Enter*varkon_seg_parab_b  **Interpolate 2 pts and 1 tangent\n");
}
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur701 pt_a  %f %f %f\n",pt_a.x_gm, pt_a.y_gm, pt_a.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur701 tan_a %f %f %f\n",tan_a.x_gm, tan_a.y_gm, tan_a.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur701 pt_c  %f %f %f\n",pt_c.x_gm, pt_c.y_gm, pt_c.z_gm );
}
#endif
 
/*!                                                                 */
/* Calculate chord length, tangent length and scalar product.       */
/*                                                                 !*/

cl_ac = sqrt( (pt_a.x_gm-pt_c.x_gm)*
              (pt_a.x_gm-pt_c.x_gm)  +
              (pt_a.y_gm-pt_c.y_gm)*
              (pt_a.y_gm-pt_c.y_gm)  +
              (pt_a.z_gm-pt_c.z_gm)*   
              (pt_a.z_gm-pt_c.z_gm)  );

tan_len = sqrt( tan_a.x_gm*tan_a.x_gm+
                tan_a.y_gm*tan_a.y_gm*
                tan_a.z_gm*tan_a.z_gm);

dot   =       (pt_c.x_gm-pt_a.x_gm)*tan_a.x_gm + 
              (pt_c.y_gm-pt_a.y_gm)*tan_a.y_gm + 
              (pt_c.z_gm-pt_a.z_gm)*tan_a.z_gm;    

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur701 cl_ac %f tan_len %f dot %f\n",cl_ac , tan_len ,   dot);
  }
#endif


if ( cl_ac < TOL2  )                     /* Check distance cl_ac    */
 {
 sprintf(errbuf, "(cl_ac)%%varkon_seg_parab_b  sur701");
 return(varkon_erpush("SU2993",errbuf));
 }

if (tan_len< TOL2  )                     /* Check distance tan_len  */
 {
 sprintf(errbuf, "(tang)%%varkon_seg_parab_b  sur701");
 return(varkon_erpush("SU2993",errbuf));
 }

if ( dot   < 0.0    )                    /* Check directions        */
 {
 sprintf(errbuf, "(direc)%%varkon_seg_parab_b  sur701");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/*                                                                  */
/* 2. Creation of the parabola                                      */
/* ___________________________                                      */
/*                                                                  */
/*                                                                 !*/

   if      ( u_case == 1 ) 
      {
      tan_a_m.x_gm = cl_ac/tan_len*tan_a.x_gm;
      tan_a_m.y_gm = cl_ac/tan_len*tan_a.y_gm;
      tan_a_m.z_gm = cl_ac/tan_len*tan_a.z_gm;
      }
   else if ( u_case == 2 ) 
      {
      tan_a_m.x_gm = 0.5*cl_ac/tan_len*tan_a.x_gm;
      tan_a_m.y_gm = 0.5*cl_ac/tan_len*tan_a.y_gm;
      tan_a_m.z_gm = 0.5*cl_ac/tan_len*tan_a.z_gm;
      }
   else if ( u_case == 3 ) 
      {
      tan_a_m.x_gm = tan_a.x_gm;
      tan_a_m.y_gm = tan_a.y_gm;
      tan_a_m.z_gm = tan_a.z_gm;
      }
    else
      {
      sprintf(errbuf, "(u_case)%%varkon_seg_parab_b  sur701");
      return(varkon_erpush("SU2993",errbuf));
      }


   (*pseg).c0x =     pt_a.x_gm;
   (*pseg).c1x =  tan_a_m.x_gm;
   (*pseg).c2x =     pt_c.x_gm - pt_a.x_gm - tan_a_m.x_gm;
   (*pseg).c3x =  0.0;        

   (*pseg).c0y =     pt_a.y_gm;
   (*pseg).c1y =  tan_a_m.y_gm;
   (*pseg).c2y =     pt_c.y_gm - pt_a.y_gm - tan_a_m.y_gm;
   (*pseg).c3y =  0.0;        

   (*pseg).c0z =     pt_a.z_gm;
   (*pseg).c1z =  tan_a_m.z_gm;
   (*pseg).c2z =     pt_c.z_gm - pt_a.z_gm - tan_a_m.z_gm;
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
  "sur701 c0x %f c1x %f c2x %f c3x %f\n",
   (*pseg).c0x, (*pseg).c1x, (*pseg).c2x, (*pseg).c3x );
  fprintf(dbgfil(SURPAC),
  "sur701 c0y %f c1y %f c2y %f c3y %f\n",
   (*pseg).c0y, (*pseg).c1y, (*pseg).c2y, (*pseg).c3y );
  fprintf(dbgfil(SURPAC),
  "sur701 c0z %f c1z %f c2z %f c3z %f\n",
   (*pseg).c0z, (*pseg).c1z, (*pseg).c2z, (*pseg).c3z );
  fprintf(dbgfil(SURPAC),
  "sur701 c0  %f c1  %f c2  %f c3  %f\n",
   (*pseg).c0 , (*pseg).c1 , (*pseg).c2 , (*pseg).c3  );
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur701 Exit  *** varkon_seg_parab_b  *\n");
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
