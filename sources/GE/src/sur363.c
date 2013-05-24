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
/*  Function: varkon_cur_uvmap                     File: sur363.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function maps an iso-parameter curve to UV area defined     */
/*  in the topology patches of the surface.                         */
/*                                                                  */
/*  This function should be a part of the varkon_cur_isoparam       */
/*  (sur150), but is programmed as a separate function as a start.  */
/*  The function is necessary for the NURBS surfaces, but will      */
/*  be necessary for all surfaces in the future. The UV area        */
/*  can (is planned) to have any values in the future .....         */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-07   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_uvmap      Map a UV (sur150) curve          */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_uvmap       * Map UV point                            */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_uvmap            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_uvmap (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg )       /* Coefficients for curve segments   (ptr) */

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
   DBfloat  start_u;     /* Start U point                           */
   DBfloat  start_v;     /* Start V point                           */
   DBfloat  end_u;       /* End   U point                           */
   DBfloat  end_v;       /* End   V point                           */
   DBfloat  start_u_map; /* Start U point, mapped                   */
   DBfloat  start_v_map; /* Start V point, mapped                   */
   DBfloat  end_u_map;   /* End   U point, mapped                   */
   DBfloat  end_v_map;   /* End   V point, mapped                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBint   i_seg;         /* Loop index segment in curve             */
  DBSeg   *p_s;          /* Current segment                   (ptr) */
  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur363 Enter varkon_cur_uvmap   Map UV curve \n");
}

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur363 p_cur->ns_cu %d p_sur->typ_su %d\n", 
      (int)p_cur->ns_cu  , (int)p_sur->typ_su);
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

/*!                                                                 */
/* 2. Map curve coefficients                                        */
/* _________________________                                        */
/*                                                                  */
/*  Start loop curve segments i_seg= istart, .... ,iend - 1         */
/*                                                                 !*/

    for ( i_seg = 0 ; i_seg < p_cur->ns_cu; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Current curve segment p_s                                     */
/*                                                                 !*/
      p_s = p_seg + i_seg;

/*!                                                                 */
/*    Calculate start and U,V point in global U                     */
/*                                                                 !*/

      start_u     = p_s->c0x;
      start_v     = p_s->c0y;
      end_u       = p_s->c0x +p_s->c1x;
      end_v       = p_s->c0y +p_s->c1y;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur363 start_u     %f start_v     %f\n", start_u, start_v );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur363   end_u     %f   end_v     %f\n",   end_u,   end_v );
fflush(dbgfil(SURPAC)); 
}
#endif

/*    Check that other coefficients are 0 for Debug On To be added  */

/*!                                                                 */
/*    Map the U,V values                                            */
/*                                                                 !*/

     u_mbs = start_u - 1.0;
     v_mbs = start_v - 1.0;

      status=varkon_sur_uvmap
    (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
    if (status<0) 
      {
      sprintf(errbuf,"sur360%%sur361 ");
      return(erpush("SU2943",errbuf));
      }

     start_u_map = u_map;
     start_v_map = v_map;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur363 start_u_map %f start_v_map %f\n", start_u_map, start_v_map );
fflush(dbgfil(SURPAC)); 
}
#endif

     u_mbs =   end_u - 1.0;
     v_mbs =   end_v - 1.0;

      status=varkon_sur_uvmap
    (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
    if (status<0) 
      {
      sprintf(errbuf,"sur360%%sur361 ");
      return(erpush("SU2943",errbuf));
      }

     end_u_map = u_map;
     end_v_map = v_map;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur363   end_u_map %f   end_v_map %f\n", end_u_map, end_v_map );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/*    Modify the coefficients                                       */
/*                                                                 !*/

      p_s->c0x  = start_u_map;
      p_s->c0y  = start_v_map;
      p_s->c1x  = end_u_map - start_u_map;
      p_s->c1y  = end_v_map - start_v_map;



      }  /* End loop curve segments i_seg= 0,1,2,3                  */


/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur363 Exit UV curve has been mapped\n");
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
