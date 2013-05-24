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
/*  Function: varkon_pat_uvloc                     File: sur311.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function modifies (maps) the local parameter (U,V)          */
/*  point calculated in varkon_sur_patadr (sur211) to the           */
/*  U,V area defined in the topology patch.                         */
/*                                                                  */
/*  The U,V area are for most surface (patch) types:                */
/*  u_s = 0  u_e = 1  v_s = 0  v_e = 1                              */
/*  For this case will the output local U,V point be equal          */
/*  to the input local U,V point.                                   */
/*                                                                  */
/*  But not for a NURBS surface. The local U,V point must be        */
/*  mapped for this surface type, i.e. this function must be        */
/*  called for a NURBS surface (patch).                             */
/*                                                                  */
/*  Note that there is another function varkon_sur_uvloc (surxxx)   */
/*  that calls this function, varkon_sur_patadr (sur211) and        */
/*  varkon_sur_nopatch (sur230).                                    */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-07   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_uvloc      Map UV pt to the patch UV area   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_pat_uvloc  (sur311).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
            DBstatus varkon_pat_uvloc (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Topology patches for surface      (ptr) */
   DBint    i_u,         /* Patch address    IU                     */
   DBint    i_v,         /*                  IV                     */
   DBfloat  u_loc,       /* Patch (local) U parameter value (sur211)*/
   DBfloat  v_loc,       /* Patch (local) V parameter value (sur211)*/
   DBfloat *p_u_map,     /* Mapped local U parameter value          */
   DBfloat *p_v_map,     /* Mapped local V parameter value          */
   DBfloat *p_u_s,       /* UV area for the topology patch, U start */
   DBfloat *p_u_e,       /* UV area for the topology patch, U end   */
   DBfloat *p_v_s,       /* UV area for the topology patch, V start */
   DBfloat *p_v_e )      /* UV area for the topology patch, V end   */

/* Out:                                                             */
/*       Data to p_u_map, p_v_map, p_u_s, p_u_e, p_v_s, p_v_e       */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBPatch *p_t;         /* Current topology patch            (ptr) */
   DBint    n_u;         /* Number of topology patches in U         */
   DBint    n_v;         /* Number of topology patches in U         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

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
"sur311*Enter u_loc %10.6f v_loc %10.6f Surf type %d\n",
                      u_loc , v_loc,(int)p_sur->typ_su );
  fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 0. Return for a non-NURBS surface. Temporarely ....              */
/* _________________________________________________________        */
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
   n_u = I_UNDEF;
   n_v = I_UNDEF;

/*!                                                                 */
/* Check that input UV point is between 0 and 1.  To be added ...   */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 2. Map U,V point                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get number of patches in U and V direction                       */
/*                                                                 !*/

   n_u        = p_sur->nu_su;
   n_v        = p_sur->nv_su;


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
  "sur311 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)i_u,(int)i_v,(int)n_u,(int)n_v);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d sur311"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(erpush("SU2953",errbuf));
        }
    if (i_u < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d sur311"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(erpush("SU2953",errbuf));
        }

    if (i_v > n_v + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur311 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)i_u,(int)i_v,(int)n_u,(int)n_v);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d sur311"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(erpush("SU2953",errbuf));
        }
    if (i_v < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d sur311"
               ,(int)i_u,(int)i_v,(int)n_u,(int)n_v);
        return(erpush("SU2953",errbuf));
        }

/*!                                                                 */
/* Address p_t to the current patch.                                */
/*                                                                 !*/

   p_t = p_pat + i_u*n_v + i_v;


/*!                                                                 */
/* Get U,V area from current p_t (i_u,i_v) patch.                   */
/*                                                                 !*/

   *p_u_s =  p_t->us_pat;
   *p_u_e =  p_t->ue_pat;
   *p_v_s =  p_t->vs_pat;
   *p_v_e =  p_t->ve_pat;

/*!                                                                 */
/* Map point                                                        */
/*                                                                 !*/

  *p_u_map = p_t->us_pat + u_loc*(p_t->ue_pat-p_t->us_pat);
  *p_v_map = p_t->vs_pat + v_loc*(p_t->ve_pat-p_t->vs_pat);


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur311 Mapped parameter pt *p_u_map %10.6f *p_v_map %10.6f\n",
   *p_u_map,*p_v_map);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

