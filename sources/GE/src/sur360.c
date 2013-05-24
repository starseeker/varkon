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
/*  Function: varkon_sur_uvmap                     File: sur360.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function modifies (maps) maps a MBS UV point to a           */
/*  global U,V point for surpac.                                    */
/*                                                                  */
/*  For all surface types is the mapping simply                     */
/*  u_global = u_mbs + 1.0 and v_global = v_mbs + 1.0.              */
/*                                                                  */
/*  But not for a NURBS surface. The MBS U,V point must be          */
/*  mapped for this surface type. The mapping is made with          */
/*  the UV area defined in the topology patch for the current       */
/*  patch.                                                          */
/*                                                                  */
/*  Note that this function now only returns with the simple        */
/*  mapping +1.0 for all surface types except for a NURB_SUR.       */
/*  But in the future should the mapping be made for all            */
/*  surface types....                                               */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-07   Originally written                                 */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_uvmap      Map MBS UV pt to surpac UV point */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_sur_patadr      * Patch adress for given U,V pt */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2953 = Patch iu,iv=    outside surface nu,nv=                  */
/* SU2993 = Severe program error in varkon_sur_uvmap  (sur360).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
            DBstatus varkon_sur_uvmap (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Topology patches for surface      (ptr) */
   DBfloat  u_mbs,       /* MBS    U parameter value                */
   DBfloat  v_mbs,       /* MBS    V parameter value                */
   DBfloat *p_u_map,     /* Mapped U parameter value          (ptr) */
   DBfloat *p_v_map )    /* Mapped V parameter value          (ptr) */

/* Out:                                                             */
/*       Data to p_u_map, p_v_map                                   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_u;         /* Patch address    IU                     */
   DBint    i_v;         /*                  IV                     */
   DBfloat  u_loc;       /* Patch (local) U parameter value (sur211)*/
   DBfloat  v_loc;       /* Patch (local) V parameter value (sur211)*/
   DBfloat  u_s;         /* UV area for the topology patch, U start */
   DBfloat  u_e;         /* UV area for the topology patch, U end   */
   DBfloat  v_s;         /* UV area for the topology patch, V start */
   DBfloat  v_e;         /* UV area for the topology patch, V end   */
   DBPatch *p_t;         /* Current topology patch            (ptr) */
   DBint    n_u;         /* Number of topology patches in U         */
   DBint    n_v;         /* Number of topology patches in U         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

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
     *p_u_map = u_mbs + 1.0;
     *p_v_map = v_mbs + 1.0;
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
   n_u = I_UNDEF;
   n_v = I_UNDEF;

/*!                                                                 */
/* 2. Calculate adress to current patch                             */
/* ____________________________________                             */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get number of patches in U and V direction                       */
/*                                                                 !*/

   n_u        = p_sur->nu_su;
   n_v        = p_sur->nv_su;

   status=varkon_sur_patadr
   (u_mbs+1.0,v_mbs+1.0,n_u,n_v,&i_u,&i_v,&u_loc,&v_loc);
   if (status<0) 
     {
       sprintf(errbuf,"sur211%%sur360");
       return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/*  Check that patch i_u exists  1 <= i_u <= n_u + 1                */
/*  Check that patch i_v exists  1 <= i_v <= n_v + 1                */
/*                                                                 !*/


    if (i_u > n_u + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur360 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)i_u,(int)i_v,(int)n_u,(int)n_v);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d sur360"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(varkon_erpush("SU2953",errbuf));
        }
    if (i_u < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d sur360"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(varkon_erpush("SU2953",errbuf));
        }

    if (i_v > n_v + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur360 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)i_u,(int)i_v,(int)n_u,(int)n_v);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d sur360"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(varkon_erpush("SU2953",errbuf));
        }
    if (i_v < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d sur360"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(varkon_erpush("SU2953",errbuf));
        }

/*!                                                                 */
/* Address p_t to the current patch.                                */
/*                                                                 !*/

   p_t = p_pat + (i_u-1)*n_v + i_v-1;

/*!                                                                 */
/* 3. Map U,V point                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get U,V area from current p_t (i_u,i_v) patch.                   */
/*                                                                 !*/

      u_s =  p_t->us_pat;
      u_e =  p_t->ue_pat;
      v_s =  p_t->vs_pat;
      v_e =  p_t->ve_pat;

/*!                                                                 */
/* Map point                                                        */
/*                                                                 !*/

  *p_u_map = u_s + u_loc*(u_e - u_s);
  *p_v_map = v_s + v_loc*(v_e - v_s);


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

