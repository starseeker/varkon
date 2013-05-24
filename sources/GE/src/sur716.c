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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_cur_fromlin                   File: sur716.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function converts a line to a curve                         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-21   Originally written                                 */
/*  1998-03-21   Curve length                                       */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_fromlin    Convert a line to a curve        */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_fromlin          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_cur_fromlin (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBLine  *p_lin,        /* Line                              (ptr) */
  DBCurve *p_cur,        /* Curve                             (ptr) */
  DBSeg   *p_seg )       /* Segment data                      (ptr) */

/* Out:                                                             */
/*        Header data to p_cur and segment data to p_seg            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur716 Enter*varkon_cur_fromlin**Convert line to a curve\n");
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* 1. Initializations and checks                                    */
/* _____________________________                                    */
/*                                                                 !*/

/*!                                                                 */
/* Initialize segment data and curve header data                    */
/* Calls of varkon_ini_gmseg (sur779) and varkon_ini_cur (sur778)   */
/*                                                                 !*/

    varkon_ini_gmseg ( p_seg );  
    varkon_ini_gmcur (p_cur);


/*!                                                                 */
/* 2. Curve header data                                             */
/* ____________________                                             */
/*                                                                 !*/

   p_cur->ns_cu       = (short)1; 
   p_cur->hed_cu.type = CURTYP;
   p_cur->plank_cu    = FALSE;

/* Curve length below ... */

/*!                                                                 */
/* 3. Segment data                                                  */
/* _______________                                                  */
/*                                                                 !*/

   p_seg->c0x    = p_lin->crd1_l.x_gm;
   p_seg->c0y    = p_lin->crd1_l.y_gm;
   p_seg->c0z    = p_lin->crd1_l.z_gm;
   p_seg->c0     = 1.0;
   p_seg->c1x    = p_lin->crd2_l.x_gm - p_lin->crd1_l.x_gm;
   p_seg->c1y    = p_lin->crd2_l.y_gm - p_lin->crd1_l.y_gm;
   p_seg->c1z    = p_lin->crd2_l.z_gm - p_lin->crd1_l.z_gm;
   p_seg->c1     = 0.0;
   p_seg->c2x    = 0.0;
   p_seg->c2y    = 0.0;
   p_seg->c2z    = 0.0;
   p_seg->c2     = 0.0;
   p_seg->c3x    = 0.0;
   p_seg->c3y    = 0.0;
   p_seg->c3z    = 0.0;
   p_seg->c3     = 0.0;

   p_seg->ofs    = 0.0;

   p_seg->sl     = SQRT(p_seg->c1x*p_seg->c1x+
                        p_seg->c1y*p_seg->c1y+
                        p_seg->c1z*p_seg->c1z);

   p_seg->typ    = CUB_SEG;

   p_seg->subtyp = 0;

   p_cur->al_cu  = p_seg->sl;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur716 Exit \n");
fflush(dbgfil(SURPAC));
}
#endif
 
    return(SUCCED);

  } /* End of function */

/*********************************************************/
