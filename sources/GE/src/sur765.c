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
/*  Function: varkon_ini_gmpatl                    File: sur765.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel GMPATL              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1995-03-08   123456789->12345                                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatl     Initialize variable GMPATL       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmseg        * Initiate DBSeg                         */
/* varkon_ini_gmlin        * Initiate DBLine                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_gmpatl (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_pat )       /* Conic lofting patch coefficients  (ptr) */
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
/* 1. Initiate variables                                            */
/* _____________________                                            */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*    Initiate DBSeg (curve segment) variables                      */
/*    Calls of varkon_ini_gmseg (sur779).                           */
/*                                                                 !*/

/* Spine curve segment                                              */
      varkon_ini_gmseg (&p_pat->spine);

/* Start limit curve segment                                        */
      varkon_ini_gmseg (&p_pat->lims);

/* End   limit curve segment                                        */
      varkon_ini_gmseg (&p_pat->lime);

/* Start tangent curve segment                                      */
      varkon_ini_gmseg (&p_pat->tans);

/* End   tangent curve segment                                      */
      varkon_ini_gmseg (&p_pat->tane);

/* Mid curve segment                                                */
      varkon_ini_gmseg (&p_pat->midc);

/* P value function                                                 */
      varkon_ini_gmseg (&p_pat->pval);

/*  Offset                                                          */
    p_pat->p_flag=          I_UNDEF;

/* V start line                                                     */
      varkon_ini_gmlin (&p_pat->vs);

/* V end   line                                                     */
      varkon_ini_gmlin (&p_pat->ve);

/*  Offset                                                          */
    p_pat->ofs_pat=         F_UNDEF;     

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
