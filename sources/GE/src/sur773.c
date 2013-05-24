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
/*  Function: varkon_ini_bcone                     File: sur773.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel BCONE               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_bcone      Initialize variable BCONE        */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_bcone (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   BCONE  *p_con )       /* Cone                              (ptr) */
/* Out:                                                             */
/*       Data to p_con                                              */
/*-----------------------------------------------------------------!*/

/******************************************************************!*/

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

/*  Minimum and maximum values for object                           */

    p_con->xdir= F_UNDEF;   
    p_con->ydir= F_UNDEF;   
    p_con->zdir= F_UNDEF;   
    p_con->ang = F_UNDEF;   
    p_con->flag= I_UNDEF;   

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
