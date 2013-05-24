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
/*  Function: varkon_sur_planplan                  File: sur105.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates the intersect line between two B-planes.   */
/*  Output is a UV (surface) curve.                                 */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-05   Originally written                                 */
/*  1998-03-21   Eliminate compiler warning                         */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function ---------------*/
/*                                                            */
/*sdescr varkon_sur_planplan   Intersection B-plane/B-plane   */
/*                                                            */
/*------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical point criterion               */
/* varkon_ini_gmlin       * Initialize DBLine                       */
/* varkon_lin_2plane      * Plane/plane intersect                   */
/* varkon_erpush          * Error message to terminal               */
/* varkon_erinit          * Initiate error message stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2923 = No intersection with the given plane                    */
/* SU2943 = Called function .. failed in varkon_sur_planplan        */
/* SU2993 = Severe program error ( ) in varkon_sur_planplan         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus    varkon_sur_planplan (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBBplane *p_bpl1,      /* B-plane 1                         (ptr) */
  DBBplane *p_bpl2,      /* B-plane 2                         (ptr) */
  DBint    lin_no,       /* Requested output line                   */
                         /* Eq.  12: Common part inside polygons    */
                         /* Eq.   1: Intersect B-plane 1 polygon    */
                         /* Eq.   2: Intersect B-plane 2 polygon    */
                         /* Eq.  99: Line in unbounded planes       */
  DBLine  *p_lin,        /* Intersect line                    (ptr) */
  DBint   *p_nlin )      /* No of output lines 0 or 1         (ptr) */

/* Out:                                                             */
/*     Line data to p_lin                                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector ps_out;      /* Start point intersect line              */
   DBVector pe_out;      /* End   point intersect line              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat dist;          /* Distance ps_out to pe_out               */
  DBfloat c_idpoint;     /* Identical point criterion               */
  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */

/* -------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_planplan   !                          */
/*               !      (sur105)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Coordinate tolerance. Call of varkon_idpoint (sur741).           */
/*                                                                 !*/

   c_idpoint = varkon_idpoint();

/*!                                                                 */
/* Initialize DBLine. Call of varkon_ini_gmlin (sur764).            */
/*                                                                 !*/

   varkon_ini_gmlin(p_lin);

/*!                                                                 */
/* 2. Calculate intersect between the planes                        */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/

  status=varkon_lin_2plane 
   (p_bpl1, p_bpl2, lin_no, &ps_out, &pe_out, p_nlin );
   if (status<0) 
     {
     sprintf(errbuf,"(lin_no %d)%%varkon_sur_planplan",(int)lin_no );
     return(varkon_erpush("SU2993",errbuf));
     }

  if ( lin_no < 0 && *p_nlin == 0 ) goto no_line;

  dist = SQRT((ps_out.x_gm-pe_out.x_gm)*(ps_out.x_gm-pe_out.x_gm) + 
              (ps_out.y_gm-pe_out.y_gm)*(ps_out.y_gm-pe_out.y_gm) + 
              (ps_out.z_gm-pe_out.z_gm)*(ps_out.z_gm-pe_out.z_gm));

   if ( dist <= c_idpoint ) 
        {
        *p_nlin = 0;
        if ( lin_no < 0 ) goto no_line;
        sprintf(errbuf,"(idpoint %5.4f)%%sur105",c_idpoint );
        return(varkon_erpush("SU2993",errbuf));
        }
 
/*!                                                                 */
/* 3. Create line                                                   */
/* ______________                                                   */
/*                                                                  */
/*                                                                 !*/

  p_lin->crd1_l = ps_out;
  p_lin->crd2_l = pe_out;


no_line:; /* Case no line. Only determine if intersect exists       */

/* 4. Exit                                                          */
/* -------                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur105 Exit**varkon_sur_planplan** No. of lines %d\n",*p_nlin);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
