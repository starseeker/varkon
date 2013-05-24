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
/*  Function: varkon_pat_evalbox                   File: sur213.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point in a BOX_SUR patch.             */
/*                                                                  */
/*  A BOX_SUR is a FAC_SUR surface with 6 4-corner B-planes         */
/*  where the six planes defines a box. The BOX_SUR surface         */
/*  is normally defined by SUR_APPROX, i.e. the surface is a        */
/*  bounding box for a whole surface.                               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-15   Originally written                                 */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-12-05   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_evalbox    BOX_SUR patch evaluation routine */
/*                                                              */
/*--------------------------------------------------------------*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */




/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_evalbox   */
/* SU2993 = Severe program error ( ) in varkon_pat_evalbox   sur213 */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_pat_evalbox (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBSurf  *p_sur,       /* Output surface                    (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1:  not allowed                     */
                         /* Eq. 2:  not allowed                     */
                         /* Eq.  :  not allowed                     */
   DBfloat  u_pat,       /* Patch (local) U parameter value         */
   DBfloat  v_pat,       /* Patch (local) V parameter value         */

   EVALS   *p_xyz )      /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */

/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   GMPATF *p_patfc;      /* Current geometric   patch         (ptr) */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */

   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/* -------------- Flow diagram -------------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_pat_evalbox   !                           */
/*               !     (sur213)         !                           */
/*               !______________________!                           */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur213 Enter varkon_pat_evalbox: SUR_BOX evaluation U %f V %f \n",
             u_pat, v_pat );
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Only icase= 0 is allowed                                         */
/*                                                                 !*/

  if  ( icase != 0 )
    {
    sprintf(errbuf,"icase %d %%sur213",(int)icase);
    return(varkon_erpush("SU2993",errbuf));
    }


/*!                                                                 */
/* Get pointer to the current geometric patch                       */
/*                                                                 !*/

   p_gpat = p_pat->spek_c;               /* Secondary patch (C ptr) */
   p_patfc= (GMPATF *)p_gpat;            /* Current geometric patch */


#ifdef  TODO_ADD_ERROR_HANDLING
   if ( p_patfc->styp_pat != FAC_PAT )
      {
      }
      fprintf(dbgfil(SURPAC),
      "sur220 Type of secondary patch is FAC_PAT (styp_pat= %d) \n",
       p_patfc->styp_pat);
#endif




/*!                                                                 */
/* 3. Coordinates from corner points of box.                        */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Corner point 1 for (u_pat,v_pat)= (0,0)                          */
/*                                                                 !*/

     if           ( fabs(u_pat-0.0)< 0.0000001 && 
                    fabs(v_pat-0.0)< 0.0000001     )
       {
       p_xyz->r_x = p_patfc->p1.x_gm; 
       p_xyz->r_y = p_patfc->p1.y_gm; 
       p_xyz->r_z = p_patfc->p1.z_gm; 
       }

/*!                                                                 */
/* Corner point 2 for (u_pat,v_pat)= (1,0)                          */
/*                                                                 !*/

     else  if     ( fabs(u_pat-1.0)< 0.0000001 && 
                    fabs(v_pat-0.0)< 0.0000001     )
       {
       p_xyz->r_x = p_patfc->p2.x_gm; 
       p_xyz->r_y = p_patfc->p2.y_gm; 
       p_xyz->r_z = p_patfc->p2.z_gm; 
       }

/*!                                                                 */
/* Corner point 3 for (u_pat,v_pat)= (1,1)                          */
/*                                                                 !*/

     else  if     ( fabs(u_pat-1.0)< 0.0000001 && 
                    fabs(v_pat-1.0)< 0.0000001     )
       {
       p_xyz->r_x = p_patfc->p3.x_gm; 
       p_xyz->r_y = p_patfc->p3.y_gm; 
       p_xyz->r_z = p_patfc->p3.z_gm; 
       }

/*!                                                                 */
/* Corner point 4 for (u_pat,v_pat)= (0,1)                          */
/*                                                                 !*/

     else  if     ( fabs(u_pat-0.0)< 0.0000001 && 
                    fabs(v_pat-1.0)< 0.0000001     )
       {
       p_xyz->r_x = p_patfc->p4.x_gm; 
       p_xyz->r_y = p_patfc->p4.y_gm; 
       p_xyz->r_z = p_patfc->p4.z_gm; 
       }

     else 
       {
       sprintf(errbuf," %%sur213");
       return(varkon_erpush("SU2213",errbuf));
       }



/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur213 Exit*varkon_pat_evalbox x= %f y= %f z= %f\n",
                      p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


