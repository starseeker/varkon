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
/*  Function: varkon_ini_gmpat                     File: sur768.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initiates structure variabel DBPatch.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1995-02-26   -123456789 -> -12345                               */
/*  1996-05-13   NUL_PAT, F_UNDEF, I_UNDEF                          */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_bbox         * Initialize BBOX                        */
/* varkon_ini_bcone        * Initialize BCONE                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpat      Initialize variable DBPatch      */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmpat (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBPatch *p_pat )      /* Topological patch                 (ptr) */
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
/*  Type of secondary patch                                         */
    p_pat->styp_pat = NUL_PAT;

/*  GM pointer to secondary patch                                   */
    p_pat->spek_gm=    DBNULL;       

/*  C  pointer to secondary patch                                   */
    p_pat->spek_c =    (char)NULL;       

/*  Topological address secondary patch                             */
    p_pat->su_pat=     I_UNDEF;
    p_pat->sv_pat=     I_UNDEF;

/*  Topological address this      patch                             */
    p_pat->iu_pat=     I_UNDEF;
    p_pat->iv_pat=     I_UNDEF;

/*  Patch boundary points                                           */
    p_pat->us_pat=     F_UNDEF;     
    p_pat->ue_pat=     F_UNDEF;     
    p_pat->vs_pat=     F_UNDEF;     
    p_pat->ve_pat=     F_UNDEF;     

/*!                                                                 */
/*    Initiate variabel BBOX                                        */
/*    Call of varkon_ini_bbox  (sur772).                            */
/*                                                                 !*/

      varkon_ini_bbox (&p_pat->box_pat);

/*!                                                                 */
/*    Initiate variabel BCONE                                       */
/*    Call of varkon_ini_bcone (sur773).                            */
/*                                                                 !*/

      varkon_ini_bcone (&p_pat->cone_pat);


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
