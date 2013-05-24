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
/*  Function: varkon_ini_evalc                     File: sur776.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel EVALC.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1995-02-25   u_t2_geod0, v_t2_geod0                             */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_evalc      Initialize variable EVALC        */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_evalc (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALC  *p_xyz )       /* Curve   coordinates & derivatives (ptr) */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variable                                           */
/* ______________________                                           */
/*                                                                  */
/*                                                                 !*/

/*  Type of evaluation                                              */
    p_xyz->evltyp =   I_UNDEF;

/*  Local parameter value                                           */
    p_xyz->t_local=   F_UNDEF;

/*  Global parameter value                                          */
    p_xyz->t_global=  F_UNDEF;

/*  Position                                                        */
    p_xyz->r.x_gm = F_UNDEF;
    p_xyz->r.y_gm = F_UNDEF;
    p_xyz->r.z_gm = F_UNDEF;

/*  First derivative                                                */
    p_xyz->drdt.x_gm = F_UNDEF;
    p_xyz->drdt.y_gm = F_UNDEF;
    p_xyz->drdt.z_gm = F_UNDEF;

/*  Second derivative                                               */
    p_xyz->d2rdt2.x_gm = F_UNDEF;
    p_xyz->d2rdt2.y_gm = F_UNDEF;
    p_xyz->d2rdt2.z_gm = F_UNDEF;

/*  Principal normal                                                */
    p_xyz->p_norm.x_gm = F_UNDEF;
    p_xyz->p_norm.y_gm = F_UNDEF;
    p_xyz->p_norm.z_gm = F_UNDEF;

/*  Bi        normal                                                */
    p_xyz->b_norm.x_gm = F_UNDEF;
    p_xyz->b_norm.y_gm = F_UNDEF;
    p_xyz->b_norm.z_gm = F_UNDEF;

/*  Curvature                                                       */
    p_xyz->kappa       = F_UNDEF;
    p_xyz->nkappa      = F_UNDEF;
    p_xyz->geodesic    = F_UNDEF;
    p_xyz->torsion     = F_UNDEF;

/*  Curve segment number                                            */
    p_xyz->iseg        = I_UNDEF;

/*  Parameter U,V curve point and derivatives                       */
    p_xyz->u           = F_UNDEF;
    p_xyz->v           = F_UNDEF;
    p_xyz->u_t         = F_UNDEF;
    p_xyz->v_t         = F_UNDEF;
    p_xyz->u_t2        = F_UNDEF;
    p_xyz->v_t2        = F_UNDEF;

/*  Second derivative for geodesic= 0                               */
    p_xyz->u_t2_geod0  = F_UNDEF;
    p_xyz->v_t2_geod0  = F_UNDEF;

/*  Shall be deleted when replaced ...                              */
    p_xyz->u_global    = F_UNDEF;

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;
    p_xyz->r_y= F_UNDEF;
    p_xyz->r_z= F_UNDEF;

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;
    p_xyz->u_y= F_UNDEF;
    p_xyz->u_z= F_UNDEF;

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;
    p_xyz->u2_y= F_UNDEF;
    p_xyz->u2_z= F_UNDEF;

/*  Princip normal       r(u)                                       */
    p_xyz->pn_x= F_UNDEF;
    p_xyz->pn_y= F_UNDEF;
    p_xyz->pn_z= F_UNDEF;

/*  Princip normal       r(u)                                       */
    p_xyz->bn_x= F_UNDEF;
    p_xyz->bn_y= F_UNDEF;
    p_xyz->bn_z= F_UNDEF;

/*  Princip normal       r(u)                                       */
    p_xyz->offset= F_UNDEF;

/*  Evaluation case                                                 */
    p_xyz->e_case= I_UNDEF;

/* Flag indicating if analysis is made                              */
    p_xyz->c_anal= FALSE;        

/* Analyzis flags                                                   */
    p_xyz->cubseg=   FALSE;        
    p_xyz->ratseg=   FALSE;        
    p_xyz->offseg=   FALSE;        
    p_xyz->surpat=   FALSE;        
    p_xyz->straight= FALSE;        

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
