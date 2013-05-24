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
/*  Function: varkon_ini_epoint                    File: sur775.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel EPOINT.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1997-04-04   branch added                                       */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_ini_evalc();     * Initialize EVALC             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_epoint     Initialize variable EPOINT       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_epoint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EPOINT *p_ep )        /* Patch entry/exit point            (ptr) */
/* Out:                                                             */
/*       Data to p_ep                                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

EVALC cur_rec;                    /* Curve evaluation record        */
DBint i_r;                        /* Loop index for initialization  */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variable                                           */
/* _____________________                                            */
/*                                                                  */
/*                                                                 !*/

/*  Record number                                                   */
    p_ep->rec_no = I_UNDEF;   

/*  Exit entry points and tangents                                  */
    p_ep->u1  =   F_UNDEF;    
    p_ep->v1  =   F_UNDEF;    
    p_ep->u1_t=   F_UNDEF;    
    p_ep->v1_t=   F_UNDEF;    
    p_ep->u2  =   F_UNDEF;    
    p_ep->v2  =   F_UNDEF;    
    p_ep->u2_t=   F_UNDEF;    
    p_ep->v2_t=   F_UNDEF;    

/*  Patch boundary                                                  */
    p_ep->us=   F_UNDEF;    
    p_ep->vs=   F_UNDEF;    
    p_ep->ue=   F_UNDEF;    
    p_ep->ve=   F_UNDEF;    

/*  Pointers between points                                         */
    p_ep->ptr1   = I_UNDEF;   
    p_ep->ptr2   = I_UNDEF;   

/*  Branch                                                          */
    p_ep->branch = I_UNDEF;   

/*  Boundary point flag                                             */
    p_ep->b_flag = I_UNDEF;   

/*  Derivative point flag                                           */
    p_ep->d_flag = I_UNDEF;   

/*  Number of exit/entry points                                     */
    p_ep->no_ep  = I_UNDEF;   


/*!                                                                 */
/*  Initialize variable EVALC                                       */
/*  Calls of varkon_ini_evalc (sur776).                             */
/*                                                                 !*/

   for (i_r=1; i_r<= 9;    i_r= i_r+1)
   {
      varkon_ini_evalc ( &cur_rec  );   
      p_ep->curpt[i_r-1] = cur_rec;   
    }


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
