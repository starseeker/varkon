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
/*  Function: varkon_ini_appcur                    File: sur762.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel APPCUR              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-29   Originally written                                 */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_appcur     Initialize variable APPCUR       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evalc        * Initialize EVALC                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_appcur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   APPCUR *p_app )       /* Curve approximation data          (ptr) */
/* Out:                                                             */
/*       Data to p_app                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variables                                          */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/

p_app->ctype  = I_UNDEF; /* Type of calculation                     */
                         /* Eq. 1: No of output segments is input   */
                         /* Eq. 2: Only add segments                */
                         /* Eq. 3: Try to reduce no of segments     */
p_app->method = I_UNDEF; /* Method to be used                       */
                         /* Eq. 1: Least square                     */
                         /* Eq. 2: ...                              */
p_app->nstart = I_UNDEF; /* Number of restarts                      */
p_app->maxiter= I_UNDEF; /* Maximum number of iterations            */
p_app->acase  = I_UNDEF; /* Approximation case:                     */
                         /* Eq. 1: Approximate to LINE   segments   */
                         /* Eq. 2: Approximate to CUBIC  segments   */
                         /* Eq. 3: Approximate to CIRCLE segments   */
                         /* Eq. 4: Approximate to CONIC  segments   */
                         /* Eq. 5: Approximate to RATCUB segments   */

p_app->t_incur= I_UNDEF; /* Type of input curve (what to use)       */
                         /* Eq. 1: Points (segment end points)      */
                         /* Eq. 2: Lines                            */
                         /* Eq. 3: Points and tangents              */
                         /* Eq. 4: Points, tangents and curvature   */

p_app->n_req= I_UNDEF;   /* Requested no of segments for ctype= 1   */

p_app->n_alloc= I_UNDEF; /* Size of allocated area for output segm. */

                         /* Limits and criteria for approximation:  */
p_app->clmax= F_UNDEF;   /* Maximum chord length                    */
p_app->lcrit= F_UNDEF;   /* Curvature radius for straight line      */

p_app->d_che= F_UNDEF;   /* Arclength check distance                */

                         /* Approximation tolerances:               */
                         /* ( Tolerance < 0 <==> not defined )      */
p_app->t_type= I_UNDEF;  /* Tolerance type                          */
                         /* Eq. 1: Tolerance band +/- curve         */
                         /* Eq. 2: Tolerance band  +  curve   only  */
                         /* Eq. 3: Tolerance band  -  curve   only  */
p_app->ctol= F_UNDEF;    /* Curve   coordinate  tolerance           */
p_app->ntol= F_UNDEF;    /* Curve   tangent     tolerance           */
                         /* (tolerance is in degrees)               */
p_app->rtol= F_UNDEF;    /* Radius of curvature tolerance           */

p_app->comptol= F_UNDEF; /* Computer accuracy tolerance             */

p_app->i_start= I_UNDEF; /* Current start segm. no for  input crv   */
p_app->u_start= F_UNDEF; /* Current parameter value in i_start      */
p_app->i_end= I_UNDEF;   /* Current end   segm. no for  input crv   */
p_app->u_end = F_UNDEF;  /* Current parameter value in i_end        */
p_app->i_outcur= I_UNDEF;/* Current segment number for output crv   */
p_app->u_outcur= F_UNDEF;/* Current parameter value in i_outcur     */
p_app->i_delta= I_UNDEF; /* No of segments approx by previous segm. */

/*!                                                                 */
/*    Initiate EVALC (curve point) variables                        */
/*    Calls of varkon_ini_evalc (sur776).                           */
/*                                                                 !*/

                         /* Current point from input  curve         */
      varkon_ini_evalc (&p_app->p_in ); 

                         /* Current point from output curve         */
      varkon_ini_evalc (&p_app->p_out); 


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
