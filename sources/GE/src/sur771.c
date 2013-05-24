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
/*  Function: varkon_ini_surpoi                    File: sur771.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel SURPOI.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals        * Initialize EVALS                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_surpoi     Initialize variable SURPOI       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_surpoi (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   SURPOI *p_sp )        /* Surface creation pt and derivat.  (ptr) */
/* Out:                                                             */
/*       Data to p_sp                                               */
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

/*  Point address                                                   */
    p_sp->iu = -1;         
    p_sp->iv = -1;         

/*!                                                                 */
/*    Initialize variabel EVALS                                     */
/*    Call of varkon_ini_evals (sur170).                            */
/*                                                                 !*/

      varkon_ini_evals(&p_sp->spt);

/*  Lengths of U tangents                                           */
    p_sp->u_l1= F_UNDEF;    
    p_sp->u_l2= F_UNDEF;    
    p_sp->u_l3= F_UNDEF;    
    p_sp->u_l4= F_UNDEF;    

/*  Lengths of V tangents                                           */
    p_sp->v_l1= F_UNDEF;    
    p_sp->v_l2= F_UNDEF;    
    p_sp->v_l3= F_UNDEF;    
    p_sp->v_l4= F_UNDEF;    

/*  Lengths of twist vectors                                        */
    p_sp->uv_l1= F_UNDEF;    
    p_sp->uv_l2= F_UNDEF;    
    p_sp->uv_l3= F_UNDEF;    
    p_sp->uv_l4= F_UNDEF;    

/*  Lengths of d2r/du2 vectors                                      */
    p_sp->u2_l1= F_UNDEF;    
    p_sp->u2_l2= F_UNDEF;    
    p_sp->u2_l3= F_UNDEF;    
    p_sp->u2_l4= F_UNDEF;    

/*  Lengths of d2r/dv2 vectors                                      */
    p_sp->v2_l1= F_UNDEF;    
    p_sp->v2_l2= F_UNDEF;    
    p_sp->v2_l3= F_UNDEF;    
    p_sp->v2_l4= F_UNDEF;    

/* Vector ratio in U and V direction                                */
    p_sp->u_ratio= F_UNDEF;    
    p_sp->u_ratio= F_UNDEF;    

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
