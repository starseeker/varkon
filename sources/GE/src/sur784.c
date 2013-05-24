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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_cur_eval                      File: sur784.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u ) point on a curve.                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-10-16   Call of sur785 added                               */
/*  1996-01-13   t_global added                                     */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_eval       Curve   evaluation routine       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evalc       * Initialize EVALC                        */
/* varkon_cur_segadr      * Curve segment address                   */
/* varkon_seg_eval        * Curve segment evaluation                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = sur784 Illegal computation case=   for varkon_cur_eval  */
/* SU2533 = Parameter point u= is outside curve 0 - nu              */
/* SU2943 = Called function xxxxxx failed in varkon_cur_eval        */
/* SU2993 = Program error in varkon_cur_eval (sur784). Report !     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus          varkon_cur_eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Allocated area for curve segments (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Coordinates only                 */
                         /* Eq. 1: First  derivative added          */
                         /* Eq. 2: Second derivative added          */
                         /* Eq. 3: Frenet vectors added and         */
                         /*        curvature added                  */
   DBfloat  u_global,    /* Curve (global) parameter value          */
   EVALC   *p_xyz )      /* Coordinates and derivatives       (ptr) */
/*                                                                  */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_seg;        /* Segment (local) U parameter value       */
   DBint   iu;           /* Segment number (adress)                 */
   DBint   nu;           /* Number of segments in the curve         */
   DBSeg  *p_cseg;       /* Current segment                   (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur784 Enter *** varkon_cur_eval: u_global= %f \n",
                      u_global );
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Check computation case.               Error SU2983 if not OK.    */
/*                                                                 !*/

   if (icase > 3 ) 
       {
       sprintf(errbuf,"%d%% varkon_cur_eval (sur784)",(int)icase);
       return(erpush("SU2983",errbuf));
       }

/* Initiate variabel EVALC                                          */
/* Call of varkon_ini_evalc (sur176).                               */
/*                                                                 !*/

      varkon_ini_evalc (p_xyz); 

/*!                                                                 */
/* Input parameter value to u_global to output variable p_xyz       */
/*                                                                 !*/

  p_xyz->u_global = u_global;  /* To be removed !!!!! */
  p_xyz->t_global = u_global;

/*!                                                                 */
/* Retrieve number of segments nu.                                  */
/*                                                                 !*/

   nu = (*p_cur).ns_cu;

/*!                                                                 */
/* 2. Segment adress (iu) and segment parameter (u_seg).            */
/* _____________________________________________________            */
/*                                                                  */
/*                                                                 !*/

   status=varkon_cur_segadr
   (u_global,nu,&iu,&u_seg);
   if (status<0) 
        {
        sprintf(errbuf,"sur785%%varkon_cur_segadr (sur784)");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Segment address corresponding to iu                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/
    p_cseg = p_seg + iu - 1;

/*!                                                                 */
/* 4. Curve segment evaluation                                      */
/* ___________________________                                      */
/*                                                                  */
/* Calculate coordinates and derivatives in segment p_cseg          */
/* for (local) parameter point u_seg.                               */
/* Call of varkon_seg_eval (sur782).                                */
/*                                                                 !*/

   status=varkon_seg_eval
       (p_cur,p_cseg,icase,u_seg,p_xyz);
   if (status<0) 
        {
        sprintf(errbuf,"sur782%%varkon_cur_eval (sur784)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur784 nu %d iu %d u_seg %f\n",
   (int)nu , (int)iu , u_seg );
  }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur784 Exit  *** varkon_cur_eval: x= %f y= %f z= %f\n",
                      p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

