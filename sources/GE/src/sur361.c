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
/*  Function: varkon_pat_uvmap                     File: sur361.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function modifies (maps) maps a MBS UV point to a           */
/*  local U,V point for surpac.                                     */
/*                                                                  */
/*  Refer also to varkon_sur_uvmap (sur360).                        */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-07   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_uvmap      Map MBS UV pt to patch  UV point */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_pat_uvmap  (sur361).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
            DBstatus varkon_pat_uvmap (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Current patch                     (ptr) */
   DBfloat  u_loc,       /* Patch (local) U parameter value (sur211)*/
   DBfloat  v_loc,       /* Patch (local) V parameter value (sur211)*/
   DBfloat *p_u_map,     /* Mapped U local parameter value          */
   DBfloat *p_v_map )    /* Mapped V local parameter value          */

/* Out:                                                             */
/*       Data to p_u_map, p_v_map                                   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_s;         /* UV area for the topology patch, U start */
   DBfloat  u_e;         /* UV area for the topology patch, U end   */
   DBfloat  v_s;         /* UV area for the topology patch, V start */
   DBfloat  v_e;         /* UV area for the topology patch, V end   */
   DBPatch *p_t;         /* Current topology patch            (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur361*Enter u_loc %10.6f v_loc %10.6f Surf type %d\n",
                      u_loc , v_loc,(int)p_sur->typ_su );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*  0. Return for a non-NURBS surface.                              */
/*                                                                  */
/*                                                                 !*/

   if ( p_sur->typ_su != NURB_SUR )
     {
     *p_u_map = u_loc;
     *p_v_map = v_loc;
     return(SUCCED);
     }


/*!                                                                 */
/* 1. Checks and initializations                                    */
/* _____________________________                                    */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Initialize output parameter point                                */
/*                                                                  */
/*                                                                 !*/

   *p_u_map = F_UNDEF;
   *p_v_map = F_UNDEF;

/* Initialize local variables                                       */

   p_t = NULL;

/*!                                                                 */
/* Address p_t to the current patch is input.                       */
/*                                                                 !*/

   p_t = p_pat;


/*!                                                                 */
/* 2. Map U,V point                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get U,V area from current p_t patch.                             */
/*                                                                 !*/

      u_s =  p_t->us_pat;
      u_e =  p_t->ue_pat;
      v_s =  p_t->vs_pat;
      v_e =  p_t->ve_pat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur361 u_s %10.6f u_e %10.6f v_s %10.6f v_e %10.6f\n",
   u_s, u_e, v_s, v_e );
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Map point                                                        */
/*                                                                 !*/

  *p_u_map = u_s + u_loc*(u_e - u_s);
  *p_v_map = v_s + v_loc*(v_e - v_s);


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur361 Mapped parameter pt *p_u_map %10.6f *p_v_map %10.6f\n",
   *p_u_map,*p_v_map);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

