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
/*  Function: varkon_ini_gmpatn                    File: sur761.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel GMPATN              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-30   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatn     Initialize variable GMPATN       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmpatn (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATN *p_pat )       /* Consurf patch data                (ptr) */
/* Out:                                                             */
/*       Data to p_pat                                              */
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

p_pat->v1.x_gm   = F_UNDEF;     /* Tile corner point 1               */
p_pat->v1.y_gm   = F_UNDEF;     /* Tile corner point 1               */
p_pat->v1.z_gm   = F_UNDEF;     /* Tile corner point 1               */
p_pat->w11       = F_UNDEF;     /* Weight for v1                     */
p_pat->t11       = F_UNDEF;     /* P value function value            */
p_pat->v2.x_gm   = F_UNDEF;     /* Tile corner point 2               */
p_pat->v2.y_gm   = F_UNDEF;     /* Tile corner point 2               */
p_pat->v2.z_gm   = F_UNDEF;     /* Tile corner point 2               */
p_pat->w14       = F_UNDEF;     /* Weight for v2                     */
p_pat->t14       = F_UNDEF;     /* P value function value            */
p_pat->u1.x_gm   = F_UNDEF;     /* Cross slope defining point for v1 */
p_pat->u1.y_gm   = F_UNDEF;     /* Cross slope defining point for v1 */
p_pat->u1.z_gm   = F_UNDEF;     /* Cross slope defining point for v1 */
p_pat->w12       = F_UNDEF;     /* Weight for u1                     */
p_pat->t12       = F_UNDEF;     /* P value function value            */
p_pat->u2.x_gm   = F_UNDEF;     /* Cross slope defining point for v2 */
p_pat->u2.y_gm   = F_UNDEF;     /* Cross slope defining point for v2 */
p_pat->u2.z_gm   = F_UNDEF;     /* Cross slope defining point for v2 */
p_pat->w13       = F_UNDEF;     /* Weight for u2                     */
p_pat->t13       = F_UNDEF;     /* P value function value            */
p_pat->dv1.x_gm  = F_UNDEF;     /* Fore/aft tangent point for v1     */
p_pat->dv1.y_gm  = F_UNDEF;     /* Fore/aft tangent point for v1     */
p_pat->dv1.z_gm  = F_UNDEF;     /* Fore/aft tangent point for v1     */
p_pat->w21       = F_UNDEF;     /* Weight for dv1                    */
p_pat->t21       = F_UNDEF;     /* P value function value            */
p_pat->dv2.x_gm  = F_UNDEF;     /* Fore/aft tangent point for v2     */
p_pat->dv2.y_gm  = F_UNDEF;     /* Fore/aft tangent point for v2     */
p_pat->dv2.z_gm  = F_UNDEF;     /* Fore/aft tangent point for v2     */
p_pat->w24       = F_UNDEF;     /* Weight for dv2                    */
p_pat->t24       = F_UNDEF;     /* P value function value            */
p_pat->du1.x_gm  = F_UNDEF;     /* Fore/aft tangent point for u1     */
p_pat->du1.y_gm  = F_UNDEF;     /* Fore/aft tangent point for u1     */
p_pat->du1.z_gm  = F_UNDEF;     /* Fore/aft tangent point for u1     */
p_pat->w22       = F_UNDEF;     /* Weight for du1                    */
p_pat->t22       = F_UNDEF;     /* P value function value            */
p_pat->du2.x_gm  = F_UNDEF;     /* Fore/aft tangent point for u2     */
p_pat->du2.y_gm  = F_UNDEF;     /* Fore/aft tangent point for u2     */
p_pat->du2.z_gm  = F_UNDEF;     /* Fore/aft tangent point for u2     */
p_pat->w23       = F_UNDEF;     /* Weight for du2                    */
p_pat->t23       = F_UNDEF;     /* P value function value            */
p_pat->v3.x_gm   = F_UNDEF;     /* Tile corner point 3               */
p_pat->v3.y_gm   = F_UNDEF;     /* Tile corner point 3               */
p_pat->v3.z_gm   = F_UNDEF;     /* Tile corner point 3               */
p_pat->w31       = F_UNDEF;     /* Weight for v3                     */
p_pat->t31       = F_UNDEF;     /* P value function value            */
p_pat->v4.x_gm   = F_UNDEF;     /* Tile corner point 4               */
p_pat->v4.y_gm   = F_UNDEF;     /* Tile corner point 4               */
p_pat->v4.z_gm   = F_UNDEF;     /* Tile corner point 4               */
p_pat->w34       = F_UNDEF;     /* Weight for v4                     */
p_pat->t34       = F_UNDEF;     /* P value function value            */
p_pat->u3.x_gm   = F_UNDEF;     /* Cross slope defining point for v3 */
p_pat->u3.y_gm   = F_UNDEF;     /* Cross slope defining point for v3 */
p_pat->u3.z_gm   = F_UNDEF;     /* Cross slope defining point for v3 */
p_pat->w32       = F_UNDEF;     /* Weight for u3                     */
p_pat->t32       = F_UNDEF;     /* P value function value            */
p_pat->u4.x_gm   = F_UNDEF;     /* Cross slope defining point for v4 */
p_pat->u4.y_gm   = F_UNDEF;     /* Cross slope defining point for v4 */
p_pat->u4.z_gm   = F_UNDEF;     /* Cross slope defining point for v4 */
p_pat->w33       = F_UNDEF;     /* Weight for u4                     */
p_pat->t33       = F_UNDEF;     /* P value function value            */
p_pat->dv3.x_gm  = F_UNDEF;     /* Fore/aft tangent point for v3     */
p_pat->dv3.y_gm  = F_UNDEF;     /* Fore/aft tangent point for v3     */
p_pat->dv3.z_gm  = F_UNDEF;     /* Fore/aft tangent point for v3     */
p_pat->w41       = F_UNDEF;     /* Weight for dv3                    */
p_pat->t41       = F_UNDEF;     /* P value function value            */
p_pat->dv4.x_gm  = F_UNDEF;     /* Fore/aft tangent point for v4     */
p_pat->dv4.y_gm  = F_UNDEF;     /* Fore/aft tangent point for v4     */
p_pat->dv4.z_gm  = F_UNDEF;     /* Fore/aft tangent point for v4     */
p_pat->w44       = F_UNDEF;     /* Weight for dv4                    */
p_pat->t44       = F_UNDEF;     /* P value function value            */
p_pat->du3.x_gm  = F_UNDEF;     /* Fore/aft tangent point for u3     */
p_pat->du3.y_gm  = F_UNDEF;     /* Fore/aft tangent point for u3     */
p_pat->du3.z_gm  = F_UNDEF;     /* Fore/aft tangent point for u3     */
p_pat->w42       = F_UNDEF;     /* Weight for du3                    */
p_pat->t42       = F_UNDEF;     /* P value function value            */
p_pat->du4.x_gm  = F_UNDEF;     /* Fore/aft tangent point for u4     */
p_pat->du4.y_gm  = F_UNDEF;     /* Fore/aft tangent point for u4     */
p_pat->du4.z_gm  = F_UNDEF;     /* Fore/aft tangent point for u4     */
p_pat->w43       = F_UNDEF;     /* Weight for du4                    */
p_pat->t43       = F_UNDEF;     /* P value function value            */
p_pat->lambda_0  = F_UNDEF;     /* Lambda  U=  0                     */
p_pat->lambda_13 = F_UNDEF;     /* Lambda  U= 1/3                    */
p_pat->lambda_23 = F_UNDEF;     /* Lambda  U= 2/3                    */
p_pat->lambda_1  = F_UNDEF;     /* Lambda  U=  1                     */
p_pat->mu_0      = F_UNDEF;     /* Mu      U=  0                     */
p_pat->mu_13     = F_UNDEF;     /* Mu      U= 1/3                    */
p_pat->mu_23     = F_UNDEF;     /* Mu      U= 2/3                    */
p_pat->mu_1      = F_UNDEF;     /* Mu      U=  1                     */
p_pat->dlambda_0 = F_UNDEF;     /* dLambda/dU   U=  0                */
p_pat->dlambda_13= F_UNDEF;     /* dLambda/dU   U= 1/3               */
p_pat->dlambda_23= F_UNDEF;     /* dLambda/dU   U= 2/3               */
p_pat->dlambda_1 = F_UNDEF;     /* dLambda/dU   U=  1                */
p_pat->dmu_0     = F_UNDEF;     /* dMu/dU       U=  0                */
p_pat->dmu_13    = F_UNDEF;     /* dMu/dU       U= 1/3               */
p_pat->dmu_23    = F_UNDEF;     /* dMu/dU       U= 2/3               */
p_pat->dmu_1     = F_UNDEF;     /* dMu/dU       U=  1                */
p_pat->p1        = F_UNDEF;     /* Flag 1                            */
p_pat->p2        = F_UNDEF;     /* Flag 2                            */
p_pat->p3        = F_UNDEF;     /* Flag 3                            */
p_pat->cl_p.x_gm = F_UNDEF;     /* Core Line Point  X coordinate     */
p_pat->cl_p.y_gm = F_UNDEF;     /* Core Line Point  Y coordinate     */
p_pat->cl_p.z_gm = F_UNDEF;     /* Core Line Point  Z coordinate     */
p_pat->cl_v.x_gm = F_UNDEF;     /* Core Line Vector X component      */
p_pat->cl_v.y_gm = F_UNDEF;     /* Core Line Vector Y component      */
p_pat->cl_v.z_gm = F_UNDEF;     /* Core Line Vector Z component      */
p_pat->ofs_pat   = F_UNDEF;     /* Offset for the patch              */


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
