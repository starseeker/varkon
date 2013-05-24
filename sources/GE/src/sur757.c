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
/*  Function: varkon_ini_gmpatnu                   File: sur757.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel GMPATNU             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-01-06   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatnu    Initialize variable GMPATNU      */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_gmpatnu (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATNU  *p_pat )     /* NURBS patch                       (ptr) */
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

p_pat->order_u = I_UNDEF;     /* Order in  U-led                    */
p_pat->order_u = I_UNDEF;     /* Order in  U-led                    */
p_pat->order_v = I_UNDEF;     /* Order in  V-led                    */
p_pat->nk_u    = I_UNDEF;     /* Number of U knot values            */
p_pat->nk_v    = I_UNDEF;     /* Number of V knot values            */
p_pat->kvec_u  = NULL;        /* C-pointer  to  U- knot vector      */
p_pat->kvec_v  = NULL;        /* C-pointer  to  V- knot vector      */
p_pat->cpts    = NULL;        /* C-pointer  to  node points         */
p_pat->pku     = DBNULL;      /* GM-pointer to  U- knot vector      */
p_pat->pkv     = DBNULL;      /* GM-pointer to  V- knot vector      */
p_pat->pcpts   = DBNULL;      /* GM-pointer to  node points         */
p_pat->ofs_pat = F_UNDEF;     /* Offset for the patch               */


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
