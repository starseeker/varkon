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
/*  Function: varkon_ini_gmlin                     File: sur764.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initiates structure variabel DBLine                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1997-02-09   Eliminated compilation warnings                    */
/*  1998-03-29   Width  added                                       */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmlin      Initialize variable DBLine       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmlin (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBLine *p_lin )       /* Line                              (ptr) */
/* Out:                                                             */
/*       Data to p_lin                                              */
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

/*  Header data                                                     */
/*  ------------                                                    */
/*                                                                  */
/*  Type of entity                                                  */
    p_lin->hed_l.type=    I_UNDEF;       

/*  Pointer to part                                                 */
    p_lin->hed_l.p_ptr=   DBNULL;             

/*  Pointer to previous instance                                    */
    p_lin->hed_l.n_ptr=   DBNULL;             

/*  Sequence number                                                 */
    p_lin->hed_l.seknr=   I_UNDEF;       

/*  Ordering number                                                 */
    p_lin->hed_l.ordnr=   I_UNDEF;       

/*  Version  number                                                 */
    p_lin->hed_l.vers=    I_UNDEF;       

/*  Pointers to groups                                              */
    p_lin->hed_l.g_ptr[0]= DBNULL;        
    p_lin->hed_l.g_ptr[1]= DBNULL;        
    p_lin->hed_l.g_ptr[2]= DBNULL;        

/*  Blank                                                           */
    p_lin->hed_l.blank=    FALSE;      

/*  Hit                                                             */
    p_lin->hed_l.hit=      FALSE;      

/*  Pen                                                             */
    p_lin->hed_l.pen=      I_UNDEF;       

/*  Level                                                           */
    p_lin->hed_l.level=    I_UNDEF;       

/*  Font                                                            */
    p_lin->fnt_l=          I_UNDEF;       

/*  Dash length                                                     */
    p_lin->lgt_l=          F_UNDEF;          

/*  Start point                                                     */
    p_lin->crd1_l.x_gm=    F_UNDEF;          
    p_lin->crd1_l.y_gm=    F_UNDEF;          
    p_lin->crd1_l.z_gm=    F_UNDEF;          
    p_lin->crd2_l.x_gm=    F_UNDEF;          
    p_lin->crd2_l.y_gm=    F_UNDEF;          
    p_lin->crd2_l.z_gm=    F_UNDEF;          

/*  Pointer to active coordinate system                             */
    p_lin->pcsy_l=          DBNULL;       

/*  Width                                                           */
    p_lin->wdt_l  =        F_UNDEF;     

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
