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

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_ini_spoint                    File: sur777.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initiates structure variabel SPOINT.               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals        * Initiate EVALS                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_spoint     Initialize variable SPOINT       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_spoint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   SPOINT *p_sp )        /* Surface creation pt and derivat.  (ptr) */
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
/* 1. Initialize variables                                          */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/

/*  External point                                                  */
    p_sp->extpt.x_gm    = F_UNDEF;         
    p_sp->extpt.y_gm    = F_UNDEF;         
    p_sp->extpt.z_gm    = F_UNDEF;         

/*  External point normal                                           */
    p_sp->extptnorm.x_gm= F_UNDEF;    
    p_sp->extptnorm.y_gm= F_UNDEF;    
    p_sp->extptnorm.z_gm= F_UNDEF;    

/*  External point U,V value                                        */
    p_sp->extuvpt.x_gm  = F_UNDEF;    
    p_sp->extuvpt.y_gm  = F_UNDEF;    
    p_sp->extuvpt.z_gm  = F_UNDEF;    

/*  Pointer to interrogation surface                                */
    p_sp->pisur         = NULL;       

/*  Pointer to allocated pisur patch area                           */
    p_sp->pppat         = NULL;       

/*  Start U,V point                                                 */
    p_sp->startuvpt.x_gm= F_UNDEF;          
    p_sp->startuvpt.y_gm= F_UNDEF;          
    p_sp->startuvpt.z_gm= F_UNDEF;          

/*  Patch limit points                                              */
    p_sp->us            = F_UNDEF;          
    p_sp->vs            = F_UNDEF;          
    p_sp->ue            = F_UNDEF;          
    p_sp->ve            = F_UNDEF;          

/*  External curve segment (????? Vector ???? )                     */
    p_sp->extseg.x_gm   = F_UNDEF;          
    p_sp->extseg.y_gm   = F_UNDEF;          
    p_sp->extseg.z_gm   = F_UNDEF;          

/*  External curve segment parameter                                */
    p_sp->extseg_t      = F_UNDEF;          

/*  Projection vector                                               */
    p_sp->proj.x_gm     = F_UNDEF;    
    p_sp->proj.y_gm     = F_UNDEF;    
    p_sp->proj.z_gm     = F_UNDEF;    

/*!                                                                 */
/*    Initialize variable EVALS                                     */
/*    Call of varkon_ini_evals (sur170).                            */
/*                                                                 !*/

      varkon_ini_evals(&p_sp->spt); 

/*  Point flag                                                      */
    p_sp->pflag=  12345;        

/*  Pointer to next record                                          */
    p_sp->ptrs = I_UNDEF;       



    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
