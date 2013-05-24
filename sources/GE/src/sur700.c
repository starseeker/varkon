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
/*  Function: varkon_seg_parabola                  File: sur700.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function interpolates three points in space                 */
/*  with a parabola.                                                */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-26   Originally written                                 */
/*  1995-05-16   Comments added                                     */
/*  1998-01-09   Debug                                              */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_seg_parabola   Parabola interpolation of 3 pts  */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* A parabola defined by three points                               */
/* ----------------------------------                               */
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
/* Case 1: The parameter value is determined by pt distances        */
/*                                                                  */
/* Three points are given pa, pb and pc. The corresponding          */
/* parameter values are t_a= 0 and t_c= 1. The parameter value      */
/* for the mid point is determined by the relative distances        */
/* to the other two points:                                         */
/*                                                                  */
/* t_b = cl_ab/(cl_ab+cl_bc) where                                  */
/*       cl_ab is the length of the chord pa-pb                     */
/*       cl_bc is the length of the chord pb-pc                     */
/*                                                                  */
/* r(0.0) = c0                         = pa                         */
/* r(0.5) = c0 + c1*t_b + c2*t_b*t_b   = pb                         */
/* r(1.0) = c0 + c1*1.0 + c2*1.0*1.0   = pc                         */
/*                                                                  */
/* c0                         = pa                                  */
/* pa + c1*t_b + c2*t_b*t_b   = pb                                  */
/* pa + c1*1.0 + c2*1.0*1.0   = pc                                  */
/*                                                                  */
/* c0                         =  pa                                 */
/*      c1*t_b + c2*t_b*t_b   =  pb-pa                              */
/*      c1*t_b + c2*t_b       = (pc-pa)*t_b                         */
/*                                                                  */
/* c0                         =  pa                                 */
/*             c2*t_b*(t_b-1) = pb-pa-(pc-pa)*t_b                   */
/*      c1*t_b + c2*t_b       = (pc-pa)*t_b                         */
/*                                                                  */
/* c0                         =  pa                                 */
/*               c2           =(pb-pa-(pc-pa)*t_b)/(t_b*(t_b-1))    */
/*      c1*t_b + c2*t_b       = (pc-pa)*t_b                         */
/*                                                                  */
/*                                                                 !*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/*             _______________________________                      */
/*            !                               !                     */
/*            !   c0  =    pa                 !                     */
/*            !                               !                     */
/*            !         pb-pa-(pc-pa)*t_b     !                     */
/*            !   c2  = -------------------   !                     */
/*            !           t_b*(t_b-1)         !                     */
/*            !                               !                     */
/*            !   c1  = pc - pa - c2          !                     */
/*            !                               !                     */
/*            !_______________________________!                     */
/*                                                                  */
/*                                                                  */
/* Case 2: The parameter value is t= 0.5                            */
/*                                                                  */
/* This formula may be used provided that the mid point             */
/* is in the middle of the other two points. A parabola             */
/* will always be created, but the shape will probably              */
/* not be satisfactory if the point not is in the middle.           */
/*                                                                  */
/* Three points are given pa, pb and pc. The corresponding          */
/* parameter values are t=0.0 , t=0.5 and t=1.0                     */
/*                                                                  */
/* r(0.0) = c0                         = pa                         */
/* r(0.5) = c0 + c1*0.5 + c2*0.5*0.5   = pb                         */
/* r(1.0) = c0 + c1*1.0 + c2*1.0*1.0   = pc                         */
/*                                                                  */
/* c0                         = pa                                  */
/* pa + c1*0.5 + c2*0.25      = pb                                  */
/* pa + c1     + c2           = pc                                  */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1     + c2*0.5       = 2*pb - 2*pa                         */
/*      c1     + c2           =   pc -   pa                         */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1     + c2*0.5       = 2*pb - 2*pa                         */
/*               c2*0.5       =   pc -   pa - 2*pb + 2*pa           */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1     + c2*0.5       = 2*pb - 2*pa                         */
/*               c2           = 2*pc - 4*pb + 2*pa                  */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1                    = 2*pb - 2*pa - pc + 2*pb - pa        */
/*               c2           = 2*pc - 4*pb + 2*pa                  */
/*                                                                  */
/* c0                         = pa                                  */
/*      c1                    = 4*pb - 3*pa - pc                    */
/*               c2           = 2*pc - 4*pb + 2*pa                  */
/*                                                                  */
/*             _______________________________                      */
/*            !                               !                     */
/*            !   c0  =    pa                 !                     */
/*            !   c1  = -3*pa + 4*pb -   pc   !                     */
/*            !   c2  =  2*pa - 4*pb + 2*pc   !                     */
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
/* SU2993 = Program error ( ) in varkon_seg_parabola        Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_seg_parabola (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector pt_a,         /* Start point of parabola                 */
  DBVector pt_b,         /* Mid   point of parabola                 */
  DBVector pt_c,         /* End   point of parabola                 */
  DBint    u_case,       /* Parameterisation case:                  */
                         /* Eq. 1: Parameter defined by distances   */
                         /* Eq. 2: Mid point for u= 0.5             */
  DBSeg   *pseg )        /* Pointer to a rational cubic segment     */

/* Out:                                                             */
/*        Coefficients and other segment data in pseg               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  t_b;          /* Parameter value for pt_b (for u_case=1) */
  DBfloat  cl_ab;        /* Chord length from pt_a to pt_b          */
  DBfloat  cl_bc;        /* Chord length from pt_b to pt_c          */
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

/*!                                                                 */
/* Calculate chord lengths and direction vectors                    */
/*                                                                 !*/

cl_ab = sqrt( (pt_a.x_gm-pt_b.x_gm)*
              (pt_a.x_gm-pt_b.x_gm)  +
              (pt_a.y_gm-pt_b.y_gm)*
              (pt_a.y_gm-pt_b.y_gm)  +
              (pt_a.z_gm-pt_b.z_gm)*   
              (pt_a.z_gm-pt_b.z_gm)  );

cl_bc = sqrt( (pt_b.x_gm-pt_c.x_gm)*
              (pt_b.x_gm-pt_c.x_gm)  +
              (pt_b.y_gm-pt_c.y_gm)*
              (pt_b.y_gm-pt_c.y_gm)  +
              (pt_b.z_gm-pt_c.z_gm)*   
              (pt_b.z_gm-pt_c.z_gm)  );

dot   =       (pt_b.x_gm-pt_a.x_gm)*
              (pt_c.x_gm-pt_b.x_gm)  +
              (pt_b.y_gm-pt_a.y_gm)*
              (pt_c.y_gm-pt_b.y_gm)  +
              (pt_b.z_gm-pt_a.z_gm)*   
              (pt_c.z_gm-pt_b.z_gm)  ;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur700 cl_ab %f cl_bc %f dot %f\n",cl_ab , cl_bc ,  dot);
  }
#endif


if ( cl_ab < TOL2  )                     /* Check distance cl_ab    */
 {
 sprintf(errbuf,                         /* Error SU2993 if chord   */
 "(cl_ab)%%sur700");                     /* is too short            */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }
if ( cl_bc < TOL2  )                     /* Check distance cl_bc    */
 {
 sprintf(errbuf,                         /* Error SU2993 if chord   */
 "(cl_bc)%%sur700");                     /* is too short            */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }

if ( dot  < 0.0    )                     /* Check directions        */
 {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur700 Direction not OK. dot= %f < 0.0 for\n",dot );
  fprintf(dbgfil(SURPAC),
  "sur700 pt_a %f %f %f\n",pt_a.x_gm, pt_a.y_gm, pt_a.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur700 pt_b %f %f %f\n",pt_b.x_gm, pt_b.y_gm, pt_b.z_gm );
  fprintf(dbgfil(SURPAC),
  "sur700 pt_c %f %f %f\n",pt_c.x_gm, pt_c.y_gm, pt_c.z_gm );
}
#endif
 sprintf(errbuf,                         /* Error SU2993 if direct. */
 "(direc)%%sur700");                     /* not the same            */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }

   if ( u_case == 2 ) goto _c2;
   
/*!                                                                 */
/* Goto _c2 for u_case= 2                                           */
/*                                                                  */
/* 2. Creation of the parabola for u_case = 1                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   t_b         = cl_ab/(cl_ab+cl_bc);

   (*pseg).c0x =      pt_a.x_gm;
   (*pseg).c2x = pt_b.x_gm - pt_a.x_gm - (pt_c.x_gm-pt_a.x_gm)*t_b;
   (*pseg).c2x = (*pseg).c2x / t_b / (t_b-1.0);                      
   (*pseg).c1x =  pt_c.x_gm - pt_a.x_gm - (*pseg).c2x;   
   (*pseg).c3x =  0.0;        

   (*pseg).c0y =      pt_a.y_gm;
   (*pseg).c2y = pt_b.y_gm - pt_a.y_gm - (pt_c.y_gm-pt_a.y_gm)*t_b;
   (*pseg).c2y = (*pseg).c2y / t_b / (t_b-1.0);                      
   (*pseg).c1y =  pt_c.y_gm - pt_a.y_gm - (*pseg).c2y;   
   (*pseg).c3y =  0.0;        

   (*pseg).c0z =      pt_a.z_gm;
   (*pseg).c2z = pt_b.z_gm - pt_a.z_gm - (pt_c.z_gm-pt_a.z_gm)*t_b;
   (*pseg).c2z = (*pseg).c2z / t_b / (t_b-1.0);                      
   (*pseg).c1z =  pt_c.z_gm - pt_a.z_gm - (*pseg).c2z;   
   (*pseg).c3z =  0.0;        


   goto _exit;

/*!                                                                 */
/* 3. Creation of the parabola for u_case = 2                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

_c2:  /*! Label:  u_case= 2                                        !*/

   (*pseg).c0x =      pt_a.x_gm;
   (*pseg).c1x = -3.0*pt_a.x_gm + 4.0*pt_b.x_gm -     pt_c.x_gm;
   (*pseg).c2x =  2.0*pt_a.x_gm - 4.0*pt_b.x_gm + 2.0*pt_c.x_gm;
   (*pseg).c3x =  0.0;        

   (*pseg).c0y =      pt_a.y_gm;
   (*pseg).c1y = -3.0*pt_a.y_gm + 4.0*pt_b.y_gm -     pt_c.y_gm;
   (*pseg).c2y =  2.0*pt_a.y_gm - 4.0*pt_b.y_gm + 2.0*pt_c.y_gm;
   (*pseg).c3y =  0.0;        

   (*pseg).c0z =      pt_a.z_gm;
   (*pseg).c1z = -3.0*pt_a.z_gm + 4.0*pt_b.z_gm -     pt_c.z_gm;
   (*pseg).c2z =  2.0*pt_a.z_gm - 4.0*pt_b.z_gm + 2.0*pt_c.z_gm;
   (*pseg).c3z =  0.0;        


/*! 4. Exit                                                         */
/*  _______                                                         */

_exit: /* Label:  Exit                                             !*/

/*!                                                                 */
/*  Denominator, offset, type and arclength                         */
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
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur700 c0x %f c1x %f c2x %f c3x %f\n",
   (*pseg).c0x, (*pseg).c1x, (*pseg).c2x, (*pseg).c3x );
  fprintf(dbgfil(SURPAC),
  "sur700 c0y %f c1y %f c2y %f c3y %f\n",
   (*pseg).c0y, (*pseg).c1y, (*pseg).c2y, (*pseg).c3y );
  fprintf(dbgfil(SURPAC),
  "sur700 c0z %f c1z %f c2z %f c3z %f\n",
   (*pseg).c0z, (*pseg).c1z, (*pseg).c2z, (*pseg).c3z );
  fprintf(dbgfil(SURPAC),
  "sur700 c0  %f c1  %f c2  %f c3  %f\n",
   (*pseg).c0 , (*pseg).c1 , (*pseg).c2 , (*pseg).c3  );
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
