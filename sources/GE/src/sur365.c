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
/*  Function: varkon_sur_uvnmap      SNAME: sur365 File: sur365.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function maps the UV net defined for an interrogation       */
/*  function.                                                       */
/*                                                                  */
/*  This function should be a part of the varkon_sur_defpatb        */
/*  (sur910), but is programmed as a separate function as a start.  */
/*  The function is necessary for the NURBS surfaces, but will      */
/*  be necessary for all surfaces in the future. The UV area        */
/*  can (is planned) to have any values in the future .....         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1998-02-01   Originally written                                 */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_uvnmap     Map a UV net (for NURBS surface) */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_sur_uvmap       * Map UV point                  */
/*           varkon_comptol         * Retrieve computer tolerance   */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_sur_uvnmap           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_sur_uvnmap (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  PBOUND  *p_pbtable,    /* Patch boundary table (UV net)     (ptr) */
  DBint    no_rec )      /* Number of records in pbtable            */

/* Out:                                                             */
/*        p_seg data will be modified                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_mbs;       /* MBS    U parameter value                */
   DBfloat  v_mbs;       /* MBS    V parameter value                */
   DBfloat  u_map;       /* Mapped U parameter value                */
   DBfloat  v_map;       /* Mapped V parameter value                */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBint   i_cr;          /* Loop index record in table              */
  PBOUND  *p_cr;         /* Current record in table           (ptr) */
  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 0. Return for a non-NURBS surface. Temporarely ....              */
/* _________________________________________________________        */
/*                                                                  */
/*                                                                 !*/

   if ( p_sur->typ_su != NURB_SUR )
     {
     return(SUCCED);
     }


/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  ___________________________________________                     */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

/* Initialization of local variables                                */
   u_mbs = F_UNDEF;
   v_mbs = F_UNDEF;
   u_map = F_UNDEF;
   v_map = F_UNDEF;

/*!                                                                 */
/* 2. Map UV net values                                             */
/* ____________________                                             */
/*                                                                  */
/*  Start loop records in UV table i_cr= 1, ..., no_rec             */
/*                                                                 !*/

   for ( i_cr = 1; i_cr <= no_rec; ++i_cr )
     {

/*!                                                                 */
/*   Pointer to current record p_cr                                 */
/*                                                                 !*/
     p_cr= p_pbtable + i_cr - 1; 

/*!                                                                 */
/*    Map UV point us,vs                                            */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur365 %d %6.3f %6.3f %6.3f %6.3f Input\n",
  (int)i_cr, p_cr->us,p_cr->vs,p_cr->ue,p_cr->ve );
fflush(dbgfil(SURPAC));
}
#endif



     u_mbs = p_cr->us - 1.0;
     v_mbs = p_cr->vs - 1.0;

     status=varkon_sur_uvmap
      (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
     if (status<0) 
       {
       sprintf(errbuf,"sur360 (us,vs)%%sur365");
       return(varkon_erpush("SU2943",errbuf));
       }

     p_cr->us = u_map;
     p_cr->vs = v_map;


/*!                                                                 */
/*    Map UV point ue,ve                                            */
/*                                                                 !*/

     u_mbs = p_cr->ue - 1.0;
     v_mbs = p_cr->ve - 1.0;

     status=varkon_sur_uvmap
      (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
     if (status<0) 
       {
       sprintf(errbuf,"sur360 (ue,ve)%%sur365");
       return(varkon_erpush("SU2943",errbuf));
       }

     p_cr->ue = u_map;
     p_cr->ve = v_map;


#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur365 %d %6.3f %6.3f %6.3f %6.3f Output\n",
  (int)i_cr, p_cr->us,p_cr->vs,p_cr->ue,p_cr->ve );
fflush(dbgfil(SURPAC));
}
#endif

    } /* End loop i_cr */


/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

    return(SUCCED);

} /* End of function */


/********************************************************************/
