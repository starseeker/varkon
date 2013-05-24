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
/*  Function: varkon_ini_aplane                    File: sur760.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel APLANE.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-09   Originally written                                 */
/*  1996-02-05   rec_no,us,vs,ue,ve added                           */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-11-25   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_aplane     Initialize variable APLANE       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_aplane (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   APLANE *p_pl )        /* Approximative plane (facet)       (ptr) */
/* Out:                                                             */
/*       Data to p_pl                                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

   DBVector   p_ini;    /* Dummy point                              */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variables                                          */
/* ________________________                                         */
/*                                                                  */
/*                                                                 !*/

p_ini.x_gm = F_UNDEF;             /* Dummy point                    */
p_ini.y_gm = F_UNDEF;             /*                                */
p_ini.z_gm = F_UNDEF;             /*                                */

         /* Also GMRECH should be initialized !!!! */
p_pl->bpl.crd1_bp = p_ini;        /* Triangular B-plane corner 1    */
p_pl->bpl.crd2_bp = p_ini;        /*                    corner 2    */
p_pl->bpl.crd3_bp = p_ini;        /*                    corner 3    */
p_pl->bpl.crd4_bp = p_ini;        /*                    corner 4    */
p_pl->bpl.pcsy_bp = DBNULL;       /* GM pointer to coord. system    */

p_pl->u1bpl  = F_UNDEF;           /* U value for B-plane point 1    */
p_pl->v1bpl  = F_UNDEF;           /* V value for B-plane point 1    */
p_pl->u2bpl  = F_UNDEF;           /* U value for B-plane point 2    */
p_pl->v2bpl  = F_UNDEF;           /* V value for B-plane point 2    */
p_pl->u4bpl  = F_UNDEF;           /* U value for B-plane point 4    */
p_pl->v4bpl  = F_UNDEF;           /* V value for B-plane point 4    */

p_pl->rec_no = I_UNDEF;           /* Record number in PBOUND        */
p_pl->us     = F_UNDEF;           /* Patch (PBOUND) limit values    */
p_pl->vs     = F_UNDEF;           /* Patch (PBOUND) limit values    */
p_pl->ue     = F_UNDEF;           /* Patch (PBOUND) limit values    */
p_pl->ve     = F_UNDEF;           /* Patch (PBOUND) limit values    */
p_pl->sur_no = I_UNDEF;           /* Data from surface number 1 or 2*/
                                  /* (For surface/surface interr.)  */
    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
