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
/*  Function: varkon_ini_gmsur                     File: sur769.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variable DBSurf.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1995-02-26   123456789 -> 12345                                 */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1997-03-16   NURBS and BBOX added                               */
/*  1999-11-27   Free source code modifications                     */
/*  2007-01-08   Trim curves and tesselation steps, Sören L         */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmsur      Initialize variable DBSurf       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_bbox         * Initialize BBOX                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_ini_gmsur (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf *p_sur )       /* Surface data                      (ptr) */
/* Out:                                                             */
/*       Data to p_sur                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variables                                          */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/

/*  Header data                                                     */
/*  ------------                                                    */
/*                                                                  */
/*  Type of entity                                                  */
    p_sur->hed_su.type=    I_UNDEF;   

/*  Pointer to part                                                 */
    p_sur->hed_su.p_ptr=   DBNULL;        

/*  Pointer to previous instance                                    */
    p_sur->hed_su.n_ptr=   DBNULL;        

/*  Sequence number                                                 */
    p_sur->hed_su.seknr=   I_UNDEF;     

/*  Ordering number                                                 */
    p_sur->hed_su.ordnr=   I_UNDEF;     

/*  Version  number                                                 */
    p_sur->hed_su.vers=    I_UNDEF;     

/*  Pointers to groups                                              */
    p_sur->hed_su.g_ptr[0]= DBNULL;        
    p_sur->hed_su.g_ptr[1]= DBNULL;        
    p_sur->hed_su.g_ptr[2]= DBNULL;        

/*  Blank                                                           */
    p_sur->hed_su.blank=    FALSE;      

/*  Hit                                                             */
    p_sur->hed_su.hit=      FALSE;      

/*  Pen                                                             */
    p_sur->hed_su.pen=      I_UNDEF;    

/*  Level                                                           */
    p_sur->hed_su.level=    I_UNDEF;    

/*  Type of surface                                                 */
    p_sur->typ_su=          I_UNDEF;    

/*  Font                                                            */
    p_sur->fnt_su=          I_UNDEF;    

/*  Dash length                                                     */
    p_sur->lgt_su=          F_UNDEF;     

/*  Number of patches in U and V direction                          */
    p_sur->nu_su=           I_UNDEF;    
    p_sur->nv_su=           I_UNDEF;    

/*  Not used variables ....                                         */
    p_sur->ngu_su=          I_UNDEF;    
    p_sur->ngv_su=          I_UNDEF;    

/*  GM pointer to patch table                                       */
    p_sur->ptab_su=         DBNULL;       

/*  Pointer to active coordinate system                             */
    p_sur->pcsy_su=         DBNULL;       

/*  Number of segments for graphical representation curves (not used)*/
    p_sur->ngseg_su[0]=     0;    
    p_sur->ngseg_su[1]=     0;    
    p_sur->ngseg_su[2]=     0;    
    p_sur->ngseg_su[3]=     0;    
    p_sur->ngseg_su[4]=     0;    
    p_sur->ngseg_su[5]=     0;    

/*  GMSEG array pointers for font= 0                                */
    p_sur->pgseg_su[0]=     DBNULL;       
    p_sur->pgseg_su[1]=     DBNULL;       
    p_sur->pgseg_su[2]=     DBNULL;       
    p_sur->pgseg_su[3]=     DBNULL;       
    p_sur->pgseg_su[4]=     DBNULL;       
    p_sur->pgseg_su[5]=     DBNULL;       

/*  NURBS: Order in U and V direction                               */
    p_sur->uorder_su=       I_UNDEF;    
    p_sur->vorder_su=       I_UNDEF;    

/*  NURBS: Number of nodes in U and V direction                     */
    p_sur->nku_su=          I_UNDEF;    
    p_sur->nkv_su=          I_UNDEF;    

/*  NURBS: GM pointer to U and V knot vectors                       */
    p_sur->pkvu_su=         DBNULL;    
    p_sur->pkvv_su=         DBNULL;    

/*  NURBS: GM pointer to B-spline control points                    */
    p_sur->pcpts_su=        DBNULL;    

/*!                                                                 */
/*    Initialize variabel BBOX                                      */
/*    Call of varkon_ini_bbox  (sur772).                            */
/*                                                                 !*/

      varkon_ini_bbox (&p_sur->box_su);

/*  Trim curves and tesselation steps                               */

      p_sur->ntrim_su         = 0;
      p_sur->getrim_su        = DBNULL;
      p_sur->ngrwborder_su    = 0;
      p_sur->grwborder_su     = DBNULL;
      p_sur->ngrwiso_su       = 0;
      p_sur->grwiso_su        = DBNULL;
      p_sur->vertextype_su    = GL_MAP2_VERTEX_3;
      p_sur->grstrim_su       = DBNULL;
      p_sur->nustep_su        = -1.0;
      p_sur->nvstep_su        = -1.0;


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
