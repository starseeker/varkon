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
/*  Function: varkon_ini_evals                     File: sur770.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel EVALS.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1998-02-24   p_sur and p_pat added                              */
/*  1998-04-25   ordnum added                                       */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_evals      Initialize variable EVALS        */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus     varkon_ini_evals (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_xyz )       /* Surface coordinates & derivatives (ptr) */
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
/* 1. Initialize all member variables                               */
/* __________________________________                               */
/*                                                                  */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;   
    p_xyz->u_y= F_UNDEF;   
    p_xyz->u_z= F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

/*  Surface normal       r(u)                                       */
    p_xyz->n_x= F_UNDEF;   
    p_xyz->n_y= F_UNDEF;   
    p_xyz->n_z= F_UNDEF;   

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= F_UNDEF;   
    p_xyz->nu_y= F_UNDEF;   
    p_xyz->nu_z= F_UNDEF;   

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= F_UNDEF;   
    p_xyz->nv_y= F_UNDEF;   
    p_xyz->nv_z= F_UNDEF;   

/*  Maximum and minimum principal curvature                          */
    p_xyz->kmax= F_UNDEF;   
    p_xyz->kmin= F_UNDEF;   

/*  Direction in R*3 for kmax                                        */
    p_xyz->kmax_x= F_UNDEF;   
    p_xyz->kmax_y= F_UNDEF;   
    p_xyz->kmax_z= F_UNDEF;   

/*  Direction in R*3 for kmin                                        */
    p_xyz->kmin_x= F_UNDEF;   
    p_xyz->kmin_y= F_UNDEF;   
    p_xyz->kmin_z= F_UNDEF;   

/*  Direction in u,v for kmax                                       */
    p_xyz->kmax_u= F_UNDEF;   
    p_xyz->kmax_v= F_UNDEF;   

/*  Direction in u,v for kmin                                       */
    p_xyz->kmin_u= F_UNDEF;   
    p_xyz->kmin_v= F_UNDEF;   

/* Parameter value on surface                                       */
    p_xyz->u     =  F_UNDEF;   
    p_xyz->v     =  F_UNDEF;   

/* Offset value for   surface                                       */
    p_xyz->offset=  F_UNDEF;   

/* Evaluation case                                                  */
    p_xyz->e_case= I_UNDEF;      
 
/* Coordinates       for spine                                      */
    p_xyz->sp_x  =  F_UNDEF;   
    p_xyz->sp_y  =  F_UNDEF;   
    p_xyz->sp_z  =  F_UNDEF;   

/* Tangent           for spine                                      */
    p_xyz->spt_x =  F_UNDEF;   
    p_xyz->spt_y =  F_UNDEF;   
    p_xyz->spt_z =  F_UNDEF;   

/* Second derivative for spine                                      */
    p_xyz->spt2_x=  F_UNDEF;   
    p_xyz->spt2_y=  F_UNDEF;   
    p_xyz->spt2_z=  F_UNDEF;   


/* Flag indicating if analysis is made                              */
    p_xyz->s_anal= FALSE;        


/* Analysis flags                                                   */
    p_xyz->cubpat= FALSE;        
    p_xyz->ratpat= FALSE;        
    p_xyz->lftpat= FALSE;        
    p_xyz->offpat= FALSE;        
    p_xyz->planar= FALSE;        
    p_xyz->sphere= FALSE;        
    p_xyz->devel = FALSE;        
    p_xyz->saddle= FALSE;        

/* Surface number for ordering of solutions                         */
    p_xyz->ordnum=  I_UNDEF;   
    
/*  Surface                                                         */
    p_xyz->p_sur    = NULL;
/*  Patches                                                         */
    p_xyz->p_pat    = NULL;

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
