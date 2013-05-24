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
/*  Function: varkon_ini_apoint                    File: sur759.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel APOINT.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-09   Originally written                                 */
/*  1996-02-05   us,vs,ue,ve,rec_no added                           */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_apoint     Initialize variable APOINT       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_apoint (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   APOINT *p_pt )        /* Intersect R*3 and UV points       (ptr) */
/* Out:                                                             */
/*       Data to p_pt                                               */
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

                                  /* Intersect points R*3:          */
p_pt->p1     = p_ini;             /* Intersect point 1              */
p_pt->p2     = p_ini;             /* Intersect point 2              */

                                  /* Interpolated UV values:        */
p_pt->u1sur1 = F_UNDEF;           /* U value for p1 in surface 1    */
p_pt->v1sur1 = F_UNDEF;           /* V value for p1 in surface 1    */
p_pt->u2sur1 = F_UNDEF;           /* U value for p2 in surface 1    */
p_pt->v2sur1 = F_UNDEF;           /* V value for p2 in surface 1    */
p_pt->u1sur2 = F_UNDEF;           /* U value for p1 in surface 2    */
p_pt->v1sur2 = F_UNDEF;           /* V value for p1 in surface 2    */
p_pt->u2sur2 = F_UNDEF;           /* U value for p2 in surface 2    */
p_pt->v2sur2 = F_UNDEF;           /* V value for p2 in surface 2    */
                                  /* U and V < 0 <==> No intersect  */

p_pt->rec_no_sur1  = I_UNDEF;     /* PBOUND record number surface 1 */
p_pt->rec_no_sur2  = I_UNDEF;     /* PBOUND record number surface 2 */
p_pt->us_sur1      = F_UNDEF;     /* Patch (PBOUND) limit values    */
p_pt->vs_sur1      = F_UNDEF;     /* for surface 1                  */
p_pt->ue_sur1      = F_UNDEF;     /*                                */
p_pt->ve_sur1      = F_UNDEF;     /*                                */
p_pt->us_sur2      = F_UNDEF;     /* Patch (PBOUND) limit values    */
p_pt->ue_sur2      = F_UNDEF;     /* for surface 2                  */
p_pt->vs_sur2      = F_UNDEF;     /*                                */
p_pt->ve_sur2      = F_UNDEF;     /*                                */

p_pt->branch = I_UNDEF;           /* Curve branch                   */
p_pt->ip1    = I_UNDEF;           /* Ordered p1 point number        */
p_pt->ip2    = I_UNDEF;           /* Ordered p2 point number        */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
