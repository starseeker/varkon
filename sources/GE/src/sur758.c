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
/*  Function: varkon_ini_gmpatbr3                  File: sur758.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel GMPATBR3            */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-11   Originally written                                 */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatbr3   Initialize variable GMPATBR3     */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmpatbr3 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATBR3 *p_pat )     /* Bezier polygon                    (ptr) */
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

/*  Patch   coefficients                                            */
    p_pat->r00x=            F_UNDEF;     
    p_pat->r01x=            F_UNDEF;     
    p_pat->r02x=            F_UNDEF;     
    p_pat->r03x=            F_UNDEF;     

    p_pat->r10x=            F_UNDEF;     
    p_pat->r11x=            F_UNDEF;     
    p_pat->r12x=            F_UNDEF;     
    p_pat->r13x=            F_UNDEF;     

    p_pat->r20x=            F_UNDEF;     
    p_pat->r21x=            F_UNDEF;     
    p_pat->r22x=            F_UNDEF;     
    p_pat->r23x=            F_UNDEF;     

    p_pat->r30x=            F_UNDEF;     
    p_pat->r31x=            F_UNDEF;     
    p_pat->r32x=            F_UNDEF;     
    p_pat->r33x=            F_UNDEF;     

    p_pat->r00y=            F_UNDEF;     
    p_pat->r01y=            F_UNDEF;     
    p_pat->r02y=            F_UNDEF;     
    p_pat->r03y=            F_UNDEF;     

    p_pat->r10y=            F_UNDEF;     
    p_pat->r11y=            F_UNDEF;     
    p_pat->r12y=            F_UNDEF;     
    p_pat->r13y=            F_UNDEF;     

    p_pat->r20y=            F_UNDEF;     
    p_pat->r21y=            F_UNDEF;     
    p_pat->r22y=            F_UNDEF;     
    p_pat->r23y=            F_UNDEF;     

    p_pat->r30y=            F_UNDEF;     
    p_pat->r31y=            F_UNDEF;     
    p_pat->r32y=            F_UNDEF;     
    p_pat->r33y=            F_UNDEF;     

    p_pat->r00z=            F_UNDEF;     
    p_pat->r01z=            F_UNDEF;     
    p_pat->r02z=            F_UNDEF;     
    p_pat->r03z=            F_UNDEF;     

    p_pat->r10z=            F_UNDEF;     
    p_pat->r11z=            F_UNDEF;     
    p_pat->r12z=            F_UNDEF;     
    p_pat->r13z=            F_UNDEF;     

    p_pat->r20z=            F_UNDEF;     
    p_pat->r21z=            F_UNDEF;     
    p_pat->r22z=            F_UNDEF;     
    p_pat->r23z=            F_UNDEF;     

    p_pat->r30z=            F_UNDEF;     
    p_pat->r31z=            F_UNDEF;     
    p_pat->r32z=            F_UNDEF;     
    p_pat->r33z=            F_UNDEF;     

    p_pat->r00 =            F_UNDEF;     
    p_pat->r01 =            F_UNDEF;     
    p_pat->r02 =            F_UNDEF;     
    p_pat->r03 =            F_UNDEF;     

    p_pat->r10 =            F_UNDEF;     
    p_pat->r11 =            F_UNDEF;     
    p_pat->r12 =            F_UNDEF;     
    p_pat->r13 =            F_UNDEF;     

    p_pat->r20 =            F_UNDEF;     
    p_pat->r21 =            F_UNDEF;     
    p_pat->r22 =            F_UNDEF;     
    p_pat->r23 =            F_UNDEF;     

    p_pat->r30 =            F_UNDEF;     
    p_pat->r31 =            F_UNDEF;     
    p_pat->r32 =            F_UNDEF;     
    p_pat->r33 =            F_UNDEF;     

/*  Offset                                                          */
    p_pat->ofs_pat=         F_UNDEF;     

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
