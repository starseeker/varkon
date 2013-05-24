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
/*  Function: varkon_sur_uvmapr                    File: sur362.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function modifies (maps) a (NURBS) global UV point for      */
/*  surpac to MBS U,V point.                                        */
/*                                                                  */
/*  For all surface types is the mapping simply                     */
/*  u_mbs = u_global - 1.0 and v_global = v_mbs - 1.0.              */
/*                                                                  */
/*  But not for a NURBS surface. The MBS U,V point must be          */
/*  mapped for this surface type. The mapping is made with          */
/*  the UV area defined in the topology patch for the current       */
/*  patch.                                                          */
/*                                                                  */
/*  Note that this function now only returns with the simple        */
/*  mapping -1.0 for all surface types except for a NURB_SUR.       */
/*  But in the future should the mapping be made for all            */
/*  surface types....                                               */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-02-08   Originally written                                 */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_uvmapr     Map NURBS UV pt to MBS UV point  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_uvmapr (sur362).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
          DBstatus   varkon_sur_uvmapr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Topology patches for surface      (ptr) */
   DBfloat  u_nurb,      /* Global (NURBS) U parameter value        */
   DBfloat  v_nurb,      /* Global (NURBS) V parameter value        */
   DBfloat *p_u_map,     /* Mapped U parameter value          (ptr) */
   DBfloat *p_v_map )    /* Mapped V parameter value          (ptr) */

/* Out:                                                             */
/*       Data to p_u_map, p_v_map                                   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_u;         /* Patch address    IU     (Loop index)    */
   DBint    i_v;         /*                  IV     (Loop index)    */
   DBfloat  u_s;         /* UV area for the topology patch, U start */
   DBfloat  u_e;         /* UV area for the topology patch, U end   */
   DBfloat  v_s;         /* UV area for the topology patch, V start */
   DBfloat  v_e;         /* UV area for the topology patch, V end   */
   DBPatch *p_t;         /* Current topology patch            (ptr) */
   DBint    n_u;         /* Number of topology patches in U         */
   DBint    n_v;         /* Number of topology patches in U         */
   DBfloat  u_s_g;       /* UV area, global coordinate      U start */
   DBfloat  u_e_g;       /* UV area, global coordinate      U end   */
   DBfloat  v_s_g;       /* UV area, global coordinate      V start */
   DBfloat  v_e_g;       /* UV area, global coordinate      V end   */
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
"sur362*Enter u_nurb %10.6f v_nurb %10.6f Surf type %d\n",
                      u_nurb , v_nurb,(int)p_sur->typ_su );
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
     *p_u_map = u_nurb - 1.0;
     *p_v_map = v_nurb - 1.0;
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

   p_t   = NULL;
   n_u   = I_UNDEF;
   n_v   = I_UNDEF;
   i_u   = I_UNDEF;
   i_v   = I_UNDEF;
   u_s   = F_UNDEF;
   u_e   = F_UNDEF;
   v_s   = F_UNDEF;
   v_e   = F_UNDEF;
   u_s_g = F_UNDEF;
   u_e_g = F_UNDEF;
   v_s_g = F_UNDEF;
   v_e_g = F_UNDEF;

/*!                                                                 */
/* 2. Find the topology patch for the input UV point                */
/* _________________________________________________                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get number of patches in U and V direction                       */
/*                                                                 !*/

   n_u        = p_sur->nu_su;
   n_v        = p_sur->nv_su;

/*!                                                                 */
/* Start of loop over all topology patches of the surface           */
/*                                                                 !*/

   for ( i_u=1; i_u <= n_u; i_u= i_u+1 )
     {
     for ( i_v=1; i_v <= n_v; i_v= i_v+1 )
       {
/*!                                                                 */
/*     Address p_t to the current patch.                            */
/*                                                                 !*/

       p_t = p_pat + (i_u-1)*n_v + i_v-1;

/*!                                                                 */
/*     Get U,V area from current p_t (i_u,i_v) topology patch.      */
/*                                                                 !*/

       u_s =  p_t->us_pat;
       u_e =  p_t->ue_pat;
       v_s =  p_t->vs_pat;
       v_e =  p_t->ve_pat;

/*!                                                                 */
/*     Determine if the input UV point is within the UV area.       */
/*                                                                 !*/

       if  (   u_nurb >= u_s &&  u_nurb <= u_e &&
               v_nurb >= v_s &&  v_nurb <= v_e    )
         {
           goto p_found;
         } /* Input UV point within current topology patch */
       }   /* End loop i_v */
     }     /* End loop i_u */
     
/*!                                                                 */
/* End   of loop over all topology patches of the surface           */
/*                                                                 !*/


/*!                                                                 */
/* The input UV point is not within any of the UV areas. Error !    */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur362 Mapping failed for u_nurb %f v_nurb %f \n", u_nurb, v_nurb );
  fflush(dbgfil(SURPAC));
  }
#endif

   sprintf(errbuf, "UV mapping failure%%sur362");
   return(varkon_erpush("SU2993",errbuf));

/*!                                                                 */
p_found:; /* Label: Topology patch for input point is found */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur362 Input UV pt is in topology patch i_u %d i_v %d p_t %d\n",
   (int)i_u, (int)i_v, (int)p_t  );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur362 UV area is u_s %5.3f u_e %5.3f v_s %5.3f v_e %5.3f\n",
   u_s , u_e,  v_s,  v_e );
  fflush(dbgfil(SURPAC));
  }
#endif


/*!                                                                 */
/* 3. Map U,V point                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* UV area in global UV coordinates defined by the patch            */
/*                                                                 !*/

  u_s_g = (DBfloat)i_u;
  u_e_g = (DBfloat)i_u + 1.0;
  v_s_g = (DBfloat)i_v;
  v_e_g = (DBfloat)i_v + 1.0;

/*!                                                                 */
/* Map point                                                        */
/*                                                                 !*/

/* Programming check                                                */
   if  ( fabs(u_s-u_e) < 0.00000001 )
     {
     sprintf(errbuf, "u_s-u_e=0%%sur362");
     return(varkon_erpush("SU2993",errbuf));
     }
   if  ( fabs(v_s-v_e) < 0.00000001 )
     {
     sprintf(errbuf, "v_s-v_e=0%%sur362");
     return(varkon_erpush("SU2993",errbuf));
     }

/* surpac UV point                                                  */

    *p_u_map = (u_nurb-u_s)*(u_s_g-u_e_g)/(u_s-u_e) + u_s_g;
    *p_v_map = (v_nurb-v_s)*(v_s_g-v_e_g)/(v_s-v_e) + v_s_g;

/* MBS    UV point                                                  */

    *p_u_map = *p_u_map - 1.0;
    *p_v_map = *p_v_map - 1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur362 UV area u_s_g %5.3f u_e_g %5.3f v_s_g %5.3f v_e_g %5.3f\n",
   u_s_g , u_e_g,  v_s_g,  v_e_g );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur362 Mapped MBS parameter pt *p_u_map %10.6f *p_v_map %10.6f\n",
   *p_u_map,*p_v_map);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

