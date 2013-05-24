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
/*  Function: varkon_vec_projpla                   File: sur670.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function projects a vector onto a plane.                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-17   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vec_projpla    Project vector onto a plane      */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* Notations:                                                       */
/*                                                                  */
/* v_in  =  Input vector                                            */
/* plane =  Plane normal                                            */
/* v_out =  Output (projected) vector                               */
/* vcross=  plane X v_in                                            */
/*                                                                  */
/* Calculate a vector vcross perpendicular to v_in and plane        */
/*                                                                  */
/*                         !      ex          ey          ez     !  */
/*                         !                                     !  */
/* vcross=  plane X v_in = !  plane.x_gm  plane.y_gm  plane.z_gm !  */
/*                         !                                     !  */
/*                         !   v_in.x_gm   v_in.y_gm   v_in.z_gm !  */
/*                                                                  */
/*          vcross.x_gm = plane.y_gm*v_in.z_gm-plane.z_gm*v_in.y_gm */
/* vcross=  vcross.y_gm = plane.z_gm*v_in.x_gm-plane.x_gm*v_in.z_gm */
/*          vcross.z_gm = plane.x_gm*v_in.y_gm-plane.y_gm*v_in.x_gm */
/*                                                                  */
/*                                                                  */
/* Calculate the vector v_out perpendicular to plane and vcross     */
/*                                                                  */
/*                         !      ex          ey          ez     !  */
/*                         !                                     !  */
/* v_out = vcross X plane= ! vcross.x_gm vcross.y_gm vcross.z_gm !  */
/*                         !                                     !  */
/*                         !  plane.x_gm  plane.y_gm  plane.z_gm !  */
/*                                                                  */
/*        v_out.x_gm= vcross.y_gm*plane.z_gm-vcross.z_gm*plane.y_gm */
/* v_out= v_out.y_gm= vcross.z_gm*plane.x_gm-vcross.x_gm*plane.z_gm */
/*        v_out.z_gm= vcross.x_gm*plane.y_gm-vcross.y_gm*plane.x_gm */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_vec_projpla          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus  varkon_vec_projpla (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector v_in,         /* Input  vector                           */
  DBVector plane,        /* Plane  normal                           */
  DBint    ocase,        /* Output case. Eq. 1: Normalised vector   */
  DBVector *p_v_out )    /* Output vector projected on plane  (ptr) */

/* Out:                                                             */
/*        Vector data to *p_v_out                                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector v_out;        /* Projected vector                        */
  DBVector vcross;       /* Cross product vector                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  length;       /* Length of a vector                      */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data                                           */
/* ______________________                                           */
/*                                                                  */
/* Check lengths of input vectors                                   */
/*                                                                 !*/

   length = v_in.x_gm*v_in.x_gm + 
            v_in.y_gm*v_in.y_gm + 
            v_in.z_gm*v_in.z_gm;  

   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_in zero)%%varkon_vec_projpla (sur670)");
     return(varkon_erpush("SU2993",errbuf));
     }
 
/*!                                                                 */
/* 2. Projection of vector onto plane                               */
/* __________________________________                               */
/*                                                                  */
/* Calculate vcross.                                                */
/*                                                                  */
/* Calculate v_out.                                                 */
/*                                                                  */
/* Vector v_out to output vector p_v_out                            */
/*                                                                 !*/

   vcross.x_gm = plane.y_gm*v_in.z_gm-plane.z_gm*v_in.y_gm;  
   vcross.y_gm = plane.z_gm*v_in.x_gm-plane.x_gm*v_in.z_gm;  
   vcross.z_gm = plane.x_gm*v_in.y_gm-plane.y_gm*v_in.x_gm;  

   length = vcross.x_gm*vcross.x_gm + 
            vcross.y_gm*vcross.y_gm + 
            vcross.z_gm*vcross.z_gm;  

   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_cross zero)%%varkon_vec_projpla (sur670)");
     return(varkon_erpush("SU2993",errbuf));
     }
 

   v_out.x_gm= vcross.y_gm*plane.z_gm-vcross.z_gm*plane.y_gm;  
   v_out.y_gm= vcross.z_gm*plane.x_gm-vcross.x_gm*plane.z_gm;  
   v_out.z_gm= vcross.x_gm*plane.y_gm-vcross.y_gm*plane.x_gm;  

   length = SQRT(v_out.x_gm*v_out.x_gm + 
                 v_out.y_gm*v_out.y_gm + 
                 v_out.z_gm*v_out.z_gm);  

   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_out zero)%%varkon_vec_projpla (sur670)");
     return(varkon_erpush("SU2993",errbuf));
     }
 
   if ( ocase == 1 )
     {
     p_v_out->x_gm = v_out.x_gm/length;
     p_v_out->y_gm = v_out.y_gm/length;
     p_v_out->z_gm = v_out.z_gm/length;
     }
   else
     {
     p_v_out->x_gm = v_out.x_gm;
     p_v_out->y_gm = v_out.y_gm;
     p_v_out->z_gm = v_out.z_gm;
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur670 Exit*varkon_vec_projpla v_out= %f %f %f\n",
     p_v_out->x_gm, p_v_out->y_gm, p_v_out->z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
