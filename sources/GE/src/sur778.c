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
/*  Function: varkon_ini_gmcur                     File: sur778.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel DBCurve.            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-01-14   F_UNDEF and I_UNDEF                                */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1998-03-29   Width  added                                       */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmcur      Initialize variable DBCurve      */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmcur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur )      /* Curve                             (ptr) */
/* Out:                                                             */
/*       Data to p_cur                                              */
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
    p_cur->hed_cu.type=    I_UNDEF;      

/*  Pointer to part                                                 */
    p_cur->hed_cu.p_ptr=   DBNULL;        

/*  Pointer to previous instance                                    */
    p_cur->hed_cu.n_ptr=   DBNULL;        

/*  Sequence number                                                 */
    p_cur->hed_cu.seknr=   I_UNDEF;      

/*  Ordering number                                                 */
    p_cur->hed_cu.ordnr=   I_UNDEF;      

/*  Version  number                                                 */
    p_cur->hed_cu.vers=    I_UNDEF;      

/*  Pointers to groups                                              */
    p_cur->hed_cu.g_ptr[0]= DBNULL;        
    p_cur->hed_cu.g_ptr[1]= DBNULL;        
    p_cur->hed_cu.g_ptr[2]= DBNULL;        

/*  Blank                                                           */
    p_cur->hed_cu.blank=    FALSE;      

/*  Hit                                                             */
    p_cur->hed_cu.hit=      FALSE;      

/*  Pen                                                             */
    p_cur->hed_cu.pen=      I_UNDEF;      

/*  Level                                                           */
    p_cur->hed_cu.level=    I_UNDEF;      

/*  Font                                                            */
    p_cur->fnt_cu=          I_UNDEF;      

/*  Font                                                            */
    p_cur->lgt_cu=          F_UNDEF;     

/*  Total length of curve                                           */
    p_cur->al_cu=           F_UNDEF;     

/*  Number of segments                                              */
    p_cur->ns_cu=           I_UNDEF;      

/*  Curve plane (matrix)                                            */
    p_cur->csy_cu.g11=      F_UNDEF;     
    p_cur->csy_cu.g12=      F_UNDEF;     
    p_cur->csy_cu.g13=      F_UNDEF;     
    p_cur->csy_cu.g24=      F_UNDEF;     
    p_cur->csy_cu.g21=      F_UNDEF;     
    p_cur->csy_cu.g22=      F_UNDEF;     
    p_cur->csy_cu.g23=      F_UNDEF;     
    p_cur->csy_cu.g34=      F_UNDEF;     
    p_cur->csy_cu.g31=      F_UNDEF;     
    p_cur->csy_cu.g32=      F_UNDEF;     
    p_cur->csy_cu.g33=      F_UNDEF;     
    p_cur->csy_cu.g34=      F_UNDEF;     
    p_cur->csy_cu.g41=      F_UNDEF;     
    p_cur->csy_cu.g42=      F_UNDEF;     
    p_cur->csy_cu.g43=      F_UNDEF;     
    p_cur->csy_cu.g44=      F_UNDEF;     

/*  Planar flag                                                     */
    p_cur->plank_cu=        FALSE;      

/*  Pointer to active coordinate system                             */
    p_cur->pcsy_cu=         DBNULL;       

/*  Planar flag                                                     */
    p_cur->nsgr_cu=         I_UNDEF;      

/*  Width                                                           */
    p_cur->wdt_cu =         F_UNDEF;     

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
