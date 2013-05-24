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

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_tra_intlinsur                 File: sur690.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates the patch transformation matrix for        */
/*  the line/surface intersect calculation                          */
/*                                                                  */
/*  Input is an external point and a projection vector.             */
/*                                                                  */
/*  Output is a transformation matrix (coordinate system)           */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-28   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_tra_intlinsur  Matrix for line/surface intersect*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GEmktf_3p              * Create local coord. system              */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_tra_intlinsur        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus     varkon_tra_intlinsur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_extpt,     /* External point                    (ptr) */
  DBVector *p_v_proj,    /* Projection vector (shooting dir.) (ptr) */
  DBTmat   *p_pat_tra )  /* Transformation matrix for patches (ptr) */

/* Out:  Transformation matrix in p_pat_tra                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector origin;       /* Origin for line coordinate  system      */
  DBVector vecx;         /* X axis for line coordinate  system      */
  DBVector vecy;         /* Y axis for line coordinate  system      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  l_vecx;       /* Length of input projection vector       */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/* -------------------- Flow chart ---------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_tra_intlinsur !                           */
/*               !      (sur690)        !                           */
/*               !______________________!                           */
/*       ___________________!_______________________________        */
/*  ____!_____         ______!______     _____!_____   _____!_____  */
/* !    1     !  2    !      3      !   !     4     ! !     5     ! */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 1. Calculate transformation matrix for the patches (surface)     */
/* ___________________________________________________________      */
/*                                                                  */
/* Create local system with X axis equal to input project vector    */
/* and the origin equal to the external point.                      */
/* Call of GEmktf_3p.                                               */
/*                                                                 !*/

  origin =  *p_extpt;
  vecx   =  *p_v_proj;  
  l_vecx = SQRT( vecx.x_gm*vecx.x_gm + 
                 vecx.y_gm*vecx.y_gm + 
                 vecx.z_gm*vecx.z_gm );

  if  ( l_vecx <  0.005 )
      {
      sprintf(errbuf,"vecx=0 %%varkon_tra_intlinsur");
       varkon_erinit();
      return(varkon_erpush("SU2333",errbuf));
      }


  if      ( ABS(vecx.x_gm) <=  ABS(vecx.y_gm) &&  
            ABS(vecx.y_gm) <=  ABS(vecx.z_gm) ) 
      {
      vecy.x_gm = 1.0;
      vecy.y_gm = 0.0;
      vecy.z_gm = 0.0;
      }
  else if ( ABS(vecx.y_gm) <=  ABS(vecx.z_gm) &&  
            ABS(vecx.z_gm) <=  ABS(vecx.x_gm) ) 
      {
      vecy.x_gm = 0.0;
      vecy.y_gm = 1.0;
      vecy.z_gm = 0.0;
      }
  else if ( ABS(vecx.z_gm) <=  ABS(vecx.x_gm) &&  
            ABS(vecx.x_gm) <=  ABS(vecx.y_gm) ) 
      {
      vecy.x_gm = 0.0;
      vecy.y_gm = 0.0;
      vecy.z_gm = 1.0;
      }
  else if ( ABS(vecx.x_gm) <=  ABS(vecx.z_gm) &&  
            ABS(vecx.z_gm) <=  ABS(vecx.y_gm) ) 
      {
      vecy.x_gm = 1.0;
      vecy.y_gm = 0.0;
      vecy.z_gm = 0.0;
      }
  else if ( ABS(vecx.z_gm) <=  ABS(vecx.y_gm) &&  
            ABS(vecx.y_gm) <=  ABS(vecx.x_gm) ) 
      {
      vecy.x_gm = 0.0;
      vecy.y_gm = 0.0;
      vecy.z_gm = 1.0;
      }
  else if ( ABS(vecx.y_gm) <=  ABS(vecx.x_gm) &&  
            ABS(vecx.x_gm) <=  ABS(vecx.z_gm) ) 
      {
      vecy.x_gm = 0.0;
      vecy.y_gm = 1.0;
      vecy.z_gm = 0.0;
      }
   else
      {
      sprintf(errbuf,"Y axis%%varkon_tra_intlinsur");
      return(varkon_erpush("SU2993",errbuf));
      }

    status = GEmktf_3p (&origin,&vecx,&vecy,p_pat_tra);
    if (status<0) 
    {
      sprintf(errbuf,"GEmktf_3p%%varkon_tra_intlinsur");
      return(varkon_erpush("SU2943",errbuf));
    }


/* 3. Exit                                                          */

    return(SUCCED);

  } /* End of function */

/*********************************************************/
