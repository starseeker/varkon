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
/*  Function: varkon_seg_uveval                    File: sur786.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter (t) point on a given U,V curve segment.       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_seg_uveval     U,V curve segm. evaluation fctn  */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush             * Error message to terminal            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_seg_uveval  (sur786).    */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus        varkon_seg_uveval (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Curve segment                     (ptr) */
   EVALC   *p_xyz )      /* Curve coordinates & derivatives   (ptr) */
/* Out:                                                             */
/*     Data u, v, u_t, v_t, u_t2, v_t2 and surpat to p_xyz.         */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat xw,yw,w;         /* Homogenous coordinates               */
   DBfloat dxwdt,dywdt,dwdt;/*                                      */
   DBfloat d2xwdt2,d2ywdt2; /*                                      */
   DBfloat d2wdt2, w2, w4;  /*                                      */
   char    errbuf[80];      /* String for error message fctn erpush */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur786 Enter *** varkon_seg_uveval \n");
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Let flag for UV segment be true.                                 */
/*                                                                 !*/

   if ( p_seg->typ  ==  UV_CUB_SEG )
        p_xyz->surpat = TRUE;
   else
      {    
      sprintf(errbuf,                       
      "(not UV_CUB_SEG)%%varkon_seg_uveval");
      return(varkon_erpush("SU2993",errbuf));
      }                            

/*!                                                                 */
/* 2. Calculate coordinates and derivatives                         */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/

    xw =                     p_seg->c0x+
             p_xyz->t_local*(p_seg->c1x+
             p_xyz->t_local*(p_seg->c2x+
             p_xyz->t_local*p_seg->c3x));
    yw =                     p_seg->c0y+
             p_xyz->t_local*(p_seg->c1y+
             p_xyz->t_local*(p_seg->c2y+
             p_xyz->t_local* p_seg->c3y));

      w =               p_seg->c0 + 
        p_xyz->t_local*(p_seg->c1 + 
        p_xyz->t_local*(p_seg->c2 + 
        p_xyz->t_local* p_seg->c3));
      p_xyz->u = xw/w;
      p_xyz->v = yw/w;


    dxwdt =                   p_seg->c1x + 
          p_xyz->t_local*(2.0*p_seg->c2x + 
      3.0*p_xyz->t_local*     p_seg->c3x);
    dywdt =                   p_seg->c1y + 
          p_xyz->t_local*(2.0*p_seg->c2y + 
      3.0*p_xyz->t_local*     p_seg->c3y);
    dwdt  =                   p_seg->c1  + 
          p_xyz->t_local*(2.0*p_seg->c2  + 
      3.0*p_xyz->t_local*     p_seg->c3);


    w2 = w*w;
    p_xyz->u_t = (w*dxwdt - dwdt*xw)/w2;
    p_xyz->v_t = (w*dywdt - dwdt*yw)/w2;

    d2xwdt2 = 2.0*p_seg->c2x + 6.0*p_xyz->t_local*p_seg->c3x;
    d2ywdt2 = 2.0*p_seg->c2y + 6.0*p_xyz->t_local*p_seg->c3y;
    d2wdt2  = 2.0*p_seg->c2  + 6.0*p_xyz->t_local*p_seg->c3;

    w4 = w2*w2;
    p_xyz->u_t2 = (w*d2xwdt2 - dwdt*dxwdt)/w2 -
                 ((d2wdt2*xw + dwdt*dxwdt)*w2 - 
                   2.0*w*xw*dwdt*dwdt)/w4;
    p_xyz->v_t2 = (w*d2ywdt2 - dwdt*dywdt)/w2 -
                 ((d2wdt2*yw + dwdt*dywdt)*w2 - 
                   2.0*w*yw*dwdt*dwdt)/w4;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur786  u  %f  v  %f \n",
   p_xyz->u,p_xyz->v);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur786 u_t  %f v_t  %f \n",
   p_xyz->u_t ,p_xyz->v_t );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur786 u_t2 %f v_t2 %f \n",
   p_xyz->u_t2,p_xyz->v_t2);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur786 Exit *** varkon_seg_uveval  ******* \n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
