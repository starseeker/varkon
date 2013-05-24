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
/*  Function: varkon_cur_fromarc                   File: sur715.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function converts a circle to a curve                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-21   Originally written                                 */
/*  1998-03-21   Curve length = 0                                   */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_fromarc    Convert a circle to a curve      */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE300                  * Create circle                           */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_fromarc          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_cur_fromarc (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBArc   *p_arc,        /* Circle                            (ptr) */
  DBCurve *p_cur,        /* Curve                             (ptr) */
  DBSeg   *p_seg )       /* Segment data                      (ptr) */

/* Out:                                                             */
/*        Header data to p_cur and segment data to p_seg for 2D arc */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBVector pos;         /* Circle origin                           */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur715 Enter*varkon_cur_fromarc**Convert circle to a curve\n");
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* 1. Initializations and checks                                    */
/* _____________________________                                    */
/*                                                                 !*/

/*!                                                                 */
/* Initialize curve header data                                     */
/* Call of varkon_ini_cur (sur778)                                  */
/*                                                                 !*/

    varkon_ini_gmcur (p_cur);


/*!                                                                 */
/* 2. Convert 2D circle data to 3D segment data                     */
/* _____________________________________________                    */
/*                                                                 !*/

   if ( p_arc->ns_a == 0 )
     {
     pos.x_gm = p_arc->x_a;
     pos.y_gm = p_arc->y_a;
     pos.z_gm = 0.0;
     status = GE300   
     (&pos,p_arc->r_a,p_arc->v1_a,p_arc->v2_a,NULL,p_arc,p_seg,3);
     if ( status < 0 ) 
     {
     sprintf(errbuf,"varkon_GE300%%sur715");
     return(varkon_erpush("SU2943",errbuf));
     }
     }

/*!                                                                 */
/* 3. Curve header data                                             */
/* ____________________                                             */
/*                                                                 !*/

   p_cur->ns_cu       = p_arc->ns_a; 
   p_cur->hed_cu.type = CURTYP;
   p_cur->plank_cu    = FALSE;    /* Should be true, but csys must..*/
   p_cur->al_cu       =  0.0;     /* Flag = not yet calcualted      */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur715 Exit \n");
fflush(dbgfil(SURPAC));
}
#endif
 
    return(SUCCED);

  } /* End of function */

/*********************************************************/
