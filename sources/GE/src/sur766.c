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
/*  Function: varkon_ini_gmpatr                    File: sur766.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel GMPATR              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatr     Initialize variable GMPATR       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_gmpatr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATR *p_pat )       /* Rational bicubic patch coeff.'s   (ptr) */
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
    p_pat->a00x=            F_UNDEF;
    p_pat->a01x=            F_UNDEF;
    p_pat->a02x=            F_UNDEF;
    p_pat->a03x=            F_UNDEF;

    p_pat->a10x=            F_UNDEF;
    p_pat->a11x=            F_UNDEF;
    p_pat->a12x=            F_UNDEF;
    p_pat->a13x=            F_UNDEF;

    p_pat->a20x=            F_UNDEF;
    p_pat->a21x=            F_UNDEF;
    p_pat->a22x=            F_UNDEF;
    p_pat->a23x=            F_UNDEF;

    p_pat->a30x=            F_UNDEF;
    p_pat->a31x=            F_UNDEF;
    p_pat->a32x=            F_UNDEF;
    p_pat->a33x=            F_UNDEF;

    p_pat->a00y=            F_UNDEF;
    p_pat->a01y=            F_UNDEF;
    p_pat->a02y=            F_UNDEF;
    p_pat->a03y=            F_UNDEF;

    p_pat->a10y=            F_UNDEF;
    p_pat->a11y=            F_UNDEF;
    p_pat->a12y=            F_UNDEF;
    p_pat->a13y=            F_UNDEF;

    p_pat->a20y=            F_UNDEF;
    p_pat->a21y=            F_UNDEF;
    p_pat->a22y=            F_UNDEF;
    p_pat->a23y=            F_UNDEF;

    p_pat->a30y=            F_UNDEF;
    p_pat->a31y=            F_UNDEF;
    p_pat->a32y=            F_UNDEF;
    p_pat->a33y=            F_UNDEF;

    p_pat->a00z=            F_UNDEF;
    p_pat->a01z=            F_UNDEF;
    p_pat->a02z=            F_UNDEF;
    p_pat->a03z=            F_UNDEF;

    p_pat->a10z=            F_UNDEF;
    p_pat->a11z=            F_UNDEF;
    p_pat->a12z=            F_UNDEF;
    p_pat->a13z=            F_UNDEF;

    p_pat->a20z=            F_UNDEF;
    p_pat->a21z=            F_UNDEF;
    p_pat->a22z=            F_UNDEF;
    p_pat->a23z=            F_UNDEF;

    p_pat->a30z=            F_UNDEF;
    p_pat->a31z=            F_UNDEF;
    p_pat->a32z=            F_UNDEF;
    p_pat->a33z=            F_UNDEF;

    p_pat->a00 =            F_UNDEF;
    p_pat->a01 =            F_UNDEF;
    p_pat->a02 =            F_UNDEF;
    p_pat->a03 =            F_UNDEF;

    p_pat->a10 =            F_UNDEF;
    p_pat->a11 =            F_UNDEF;
    p_pat->a12 =            F_UNDEF;
    p_pat->a13 =            F_UNDEF;

    p_pat->a20 =            F_UNDEF;
    p_pat->a21 =            F_UNDEF;
    p_pat->a22 =            F_UNDEF;
    p_pat->a23 =            F_UNDEF;

    p_pat->a30 =            F_UNDEF;
    p_pat->a31 =            F_UNDEF;
    p_pat->a32 =            F_UNDEF;
    p_pat->a33 =            F_UNDEF;

/*  Offset                                                          */
    p_pat->ofs_pat=         F_UNDEF;

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
