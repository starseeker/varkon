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
/*  Function: varkon_ini_gmpatb                    File: sur763.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initiates structure variabel GMPATB.               */
/*                                                                 !*/
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-23   Originally written                                 */
/*  1995-05-28   GMCUR no longer a pointer                          */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmcur      * Initialize DBCurve                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmpatb     Initialize variable GMPATB       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_gmpatb (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATB *p_pat )       /* Surface coordinates & derivatives (ptr) */
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

/*!                                                                 */
/* Initialize curve data in cur_r0t, .....                          */
/* Calls of varkon_ini_gmcur (sur778).                              */
/*                                                                 !*/

    varkon_ini_gmcur  (&p_pat->cur_r0t);     

p_pat->p_seg_r0t = NULL;     /* Curve segments for cur_r0t    (ptr) */
p_pat->s_u0t     = F_UNDEF;  /* Start parameter value for cur_r0t   */
p_pat->e_u0t     = F_UNDEF;  /* End   parameter value for cur_r0t   */

    varkon_ini_gmcur  (&p_pat->cur_r1t);     

p_pat->p_seg_r1t = NULL;     /* Curve segments for cur_r1t    (ptr) */
p_pat->s_u1t     = F_UNDEF;  /* Start parameter value for cur_r1t   */
p_pat->e_u1t     = F_UNDEF;  /* End   parameter value for cur_r1t   */

    varkon_ini_gmcur  (&p_pat->cur_rs0);     

p_pat->p_seg_rs0 = NULL;     /* Curve segments for cur_rs0    (ptr) */
p_pat->s_u0s     = F_UNDEF;  /* Start parameter value for cur_rs0   */
p_pat->e_u0s     = F_UNDEF;  /* End   parameter value for cur_rs0   */

    varkon_ini_gmcur  (&p_pat->cur_rs1);     

p_pat->p_seg_rs1 = NULL;     /* Curve segments for cur_rs1    (ptr) */
p_pat->s_u1s     = F_UNDEF;  /* Start parameter value for cur_rs1   */
p_pat->e_u1s     = F_UNDEF;  /* End   parameter value for cur_rs1   */

p_pat->p_r00.x_gm = F_UNDEF;     /* Corner point        for s=0,t=0 */
p_pat->p_r00.y_gm = F_UNDEF;     /*                                 */
p_pat->p_r00.z_gm = F_UNDEF;     /*                                 */
p_pat->p_r01.x_gm = F_UNDEF;     /* Corner point        for s=0,t=1 */
p_pat->p_r01.y_gm = F_UNDEF;     /*                                 */
p_pat->p_r01.z_gm = F_UNDEF;     /*                                 */
p_pat->p_r10.x_gm = F_UNDEF;     /* Corner point        for s=1,t=0 */
p_pat->p_r10.y_gm = F_UNDEF;     /*                                 */
p_pat->p_r10.z_gm = F_UNDEF;     /*                                 */
p_pat->p_r11.x_gm = F_UNDEF;     /* Corner point        for s=1,t=1 */
p_pat->p_r11.y_gm = F_UNDEF;     /*                                 */
p_pat->p_r11.z_gm = F_UNDEF;     /*                                 */

p_pat->c_flag     = I_UNDEF; /* Case flag:                          */
                             /* Eq. 1: Only one segment curves      */
                             /*        Parameterized from 0 to 1    */
                             /* Eq. 2: Multi-segment curves         */

p_pat->ofs_pat    = F_UNDEF; /* Offset for the patch                */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
