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
/*  Function: varkon_sur_minmaxbox                 File: sur179.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function approximates a surface with one bounding box.      */
/*  Output is a BOX_SUR, i.e. a FAC_SUR surface with 6 4-corner     */
/*  B-planes.                                                       */
/*                                                                  */
/*  This is a function which can be used in MBS interrogation       */
/*  functions of all kinds in the solution search phase.            */
/*                                                                  */
/* TODO Also FAC_SUR (without BOX'es) should be added !!!!!         */
/* TODO Add BOX_SUR in graphic routines (for Johan TODO)            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-15   Originally written                                 */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1997-03-12   ngseg_su and pgseg_su initialized                  */
/*  1997-12-06   NURB_SUR                                           */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_minmaxbox  Create a SUR_BOX surface.        */
/*                                                              */
/*--------------------------------------------------------------*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur       * Initialize DBSurf                       */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_minmaxbox */
/* SU2993 = Severe program error ( ) in varkon_sur_minmaxbox sur179 */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus     varkon_sur_minmaxbox (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */
  DBPatch **pp_patout )   /* Alloc. area for topol. patch data (ptr) */

/* Out:                                                             */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   BBOX     box;         /* Patch box from surface                  */
   DBint    i_u;         /* Loop index U line in surface            */
   DBint    i_v;         /* Loop index V line in surface            */
   DBfloat  xmin;        /* Surface minimum X value                 */
   DBfloat  xmax;        /* Surface maximum X value                 */
   DBfloat  ymin;        /* Surface minimum Y value                 */
   DBfloat  ymax;        /* Surface maximum Y value                 */
   DBfloat  zmin;        /* Surface minimum Z value                 */
   DBfloat  zmax;        /* Surface maximum Z value                 */
/*-----------------------------------------------------------------!*/

   DBint    maxnum;      /* Maximum number of patches (alloc. area) */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   GMPATF  *p_patfc;     /* Current geometric   patch         (ptr) */
   DBint    i_pat;       /* Loop index patch                        */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
   char    *p_tpat;      /* Allocated area topol. patch data  (ptr) */

   DBint    s_type;      /* Type CUB_SUR, RAT_SUR, or .....         */
   DBint    n_u;         /* Number of patches in U direction        */
   DBint    n_v;         /* Number of patches in V direction        */

   DBPatch *p_t;         /* Current topological patch         (ptr) */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/* -------------- Flow diagram -------------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_minmaxbox !                           */
/*               !     (sur179)         !                           */
/*               !______________________!                           */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur179 Enter varkon_sur_minmaxbox : Create a SUR_BOX surface \n");
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   *pp_patout = NULL;                    /* Output pointer initial. */

/*!                                                                 */
/* Retrieve number of patches and surface types.                    */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surfaces are of type                              */
/* CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR                    */
/*                                                                 !*/

/* TODO Also FAC_SUR (without BOX'es) should be added !!!!!         */

    status = varkon_sur_nopatch (p_sur, &n_u, &n_v, &s_type);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_minmaxbox (sur179)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (  s_type     ==  CUB_SUR ||
      s_type     ==  RAT_SUR ||           
      s_type     ==  CON_SUR ||           
      s_type     ==  POL_SUR ||           
      s_type     ==   P3_SUR ||           
      s_type     ==   P5_SUR ||           
      s_type     ==   P7_SUR ||           
      s_type     ==   P9_SUR ||           
      s_type     ==  P21_SUR ||           
      s_type     == NURB_SUR ||           
      s_type     ==  MIX_SUR ||           
      s_type     ==  LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type)%%sur179");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur (p_surout);


/*!                                                                 */
/* 2. Calculate surface maximum and minimum values from BBOXE's     */
/* ____________________________________________________________     */
/*                                                                  */
/*                                                                 !*/

  xmin =  500000000.0;
  xmax = -500000000.0;
  ymin =  500000000.0;
  ymax = -500000000.0;
  zmin =  500000000.0;
  zmax = -500000000.0;

/*!                                                                 */
/* Loop all V patches  i_v=  0, 1, ...., n_v                        */
/*  Loop all U patches  i_u= 0, 1,  ..., n_u                        */
/*                                                                 !*/

for (   i_u= 0;  i_u <  n_u  ; ++i_u )   /* Start loop U patches    */
  {
  for ( i_v= 0;  i_v <  n_v  ; ++i_v )   /* Start loop V patches    */
    {


/*!                                                                 */
/*   Pointer to current topological patch                           */
     p_t = p_pat  + i_u*n_v + i_v; 
/*   Retrieve BBOX for the current patch in surface.                */
/*                                                                 !*/

     box  =  p_t->box_pat;               /* Copy BBOX  to box       */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur179 box  Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box.xmin,   box.ymin,   box.zmin,
         box.xmax,   box.ymax,   box.zmax );
}
#endif

   if  ( box.xmin <  xmin ) xmin = box.xmin;
   if  ( box.xmax >  xmax ) xmax = box.xmax;
   if  ( box.ymin <  ymin ) ymin = box.ymin;
   if  ( box.ymax >  ymax ) ymax = box.ymax;
   if  ( box.zmin <  zmin ) zmin = box.zmin;
   if  ( box.zmax >  zmax ) zmax = box.zmax;

    }                           /* End   loop U patches             */
  }                             /* End   loop V patches             */
/*!                                                                 */
/*  End  all U patches  i_u= 1, 2,  ..., n_u                        */
/* End  all V patches  i_v= 1, 2,  ..., n_v                         */
/*                                                                 !*/

/*!                                                                 */
/* 3. Allocate memory area for output surface                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/* Number of patches maxnum= 6                                      */
/*                                                                 !*/


   maxnum = 6;

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 Allocation failed  n_u %d n_v %d maxnum %d\n",
    (short)n_u, (short)n_v, (short)maxnum );
  }
#endif
 sprintf(errbuf,"(alloc)%%varkon_sur_minmaxbox (sur179");
 return(varkon_erpush("SU2993",errbuf));
 }                                 

   p_frst = (DBPatch*)p_tpat;            /* Pointer to first patch  */
   *pp_patout = p_frst;                  /* Output pointer          */

/*!                                                                 */
/* Loop all for all six patches     i_pat=1,2,3,4,5,6               */
/*                                                                 !*/

for ( i_pat=1; i_pat<= 6    ; ++i_pat)   /* Start loop patches      */
  {

/*!                                                                 */
/*   Pointer to current patch p_t                                   */
/*                                                                 !*/

    p_t = p_frst + i_pat  - 1;           /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (facet)   patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(FAC_PAT, 1 ))==NULL)
 {                                     
 sprintf(errbuf, "(allocg)%%varkon_sur_minmaxbox sur179");
 return(varkon_erpush("SU2993",errbuf));
 }                                 

p_patfc= (GMPATF *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (DBPatch *)*p_tpat + i_pat - 1                             */
/*                                                                 !*/

  p_t->styp_pat = FAC_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = 1;                     /* Topological adress for  */
  p_t->iv_pat   = (short)i_pat;          /* current (this) patch    */
  p_t->us_pat   = F_UNDEF;               /* Start U on geom. patch  */
  p_t->ue_pat   = F_UNDEF;               /* End   U on geom. patch  */
  p_t->vs_pat   = F_UNDEF;               /* Start V on geom. patch  */
  p_t->ve_pat   = F_UNDEF;               /* End   V on geom. patch  */

  p_t->box_pat.xmin = 1.0;               /* BBOX  initiation        */
  p_t->box_pat.ymin = 2.0;               /*                         */
  p_t->box_pat.zmin = 3.0;               /*                         */
  p_t->box_pat.xmax = 4.0;               /*                         */
  p_t->box_pat.ymax = 5.0;               /*                         */
  p_t->box_pat.zmax = 6.0;               /*                         */
  p_t->box_pat.flag = -1;                /* Not calculated          */
    
  p_t->cone_pat.xdir = 1.0;              /* BCONE initiation        */
  p_t->cone_pat.ydir = 2.0;              /*                         */
  p_t->cone_pat.zdir = 3.0;              /*                         */
  p_t->cone_pat.ang  = 4.0;              /*                         */
  p_t->cone_pat.flag = -1;               /* Not calculated          */


/*!                                                                 */
/*   Front B-plane                                                  */
/*                                                                 !*/

     if           ( i_pat == 1 )
       {
       p_patfc->p1.x_gm = xmin;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymin; 
       p_patfc->p1.z_gm = zmin; 
       p_patfc->p4.x_gm = xmax;                /* Corner point 2    */
       p_patfc->p4.y_gm = ymin; 
       p_patfc->p4.z_gm = zmin; 
       p_patfc->p3.x_gm = xmax;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymax; 
       p_patfc->p3.z_gm = zmin; 
       p_patfc->p2.x_gm = xmin;                /* Corner point 4    */
       p_patfc->p2.y_gm = ymax; 
       p_patfc->p2.z_gm = zmin; 
       }

/*!                                                                 */
/*   Bottom B-plane                                                 */
/*                                                                 !*/

     else  if     ( i_pat == 2 )
       {
       p_patfc->p1.x_gm = xmin;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymin; 
       p_patfc->p1.z_gm = zmin; 
       p_patfc->p2.x_gm = xmax;                /* Corner point 2    */
       p_patfc->p2.y_gm = ymin; 
       p_patfc->p2.z_gm = zmin; 
       p_patfc->p3.x_gm = xmax;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymin; 
       p_patfc->p3.z_gm = zmax; 
       p_patfc->p4.x_gm = xmin;                /* Corner point 4    */
       p_patfc->p4.y_gm = ymin; 
       p_patfc->p4.z_gm = zmax; 
       }

/*!                                                                 */
/*   Right B-plane                                                  */
/*                                                                 !*/

     else  if     ( i_pat == 3 )
       {
       p_patfc->p1.x_gm = xmax;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymin; 
       p_patfc->p1.z_gm = zmin; 
       p_patfc->p4.x_gm = xmax;                /* Corner point 2    */
       p_patfc->p4.y_gm = ymin; 
       p_patfc->p4.z_gm = zmax; 
       p_patfc->p3.x_gm = xmax;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymax; 
       p_patfc->p3.z_gm = zmax; 
       p_patfc->p2.x_gm = xmax;                /* Corner point 4    */
       p_patfc->p2.y_gm = ymax; 
       p_patfc->p2.z_gm = zmin; 
       }

/*!                                                                 */
/*   Top   B-plane                                                  */
/*                                                                 !*/

     else  if     ( i_pat == 4 )
       {
       p_patfc->p1.x_gm = xmin;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymax; 
       p_patfc->p1.z_gm = zmin; 
       p_patfc->p4.x_gm = xmax;                /* Corner point 2    */
       p_patfc->p4.y_gm = ymax; 
       p_patfc->p4.z_gm = zmin; 
       p_patfc->p3.x_gm = xmax;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymax; 
       p_patfc->p3.z_gm = zmax; 
       p_patfc->p2.x_gm = xmin;                /* Corner point 4    */
       p_patfc->p2.y_gm = ymax; 
       p_patfc->p2.z_gm = zmax; 
       }

/*!                                                                 */
/*   Left  B-plane                                                  */
/*                                                                 !*/

     else  if     ( i_pat == 5 )
       {
       p_patfc->p1.x_gm = xmin;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymin; 
       p_patfc->p1.z_gm = zmin; 
       p_patfc->p2.x_gm = xmin;                /* Corner point 2    */
       p_patfc->p2.y_gm = ymin; 
       p_patfc->p2.z_gm = zmax; 
       p_patfc->p3.x_gm = xmin;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymax; 
       p_patfc->p3.z_gm = zmax; 
       p_patfc->p4.x_gm = xmin;                /* Corner point 4    */
       p_patfc->p4.y_gm = ymax; 
       p_patfc->p4.z_gm = zmin; 
       }

/*!                                                                 */
/*   Back  B-plane                                                  */
/*                                                                 !*/

     else  if     ( i_pat == 6 )
       {
       p_patfc->p1.x_gm = xmin;                /* Corner point 1    */
       p_patfc->p1.y_gm = ymin; 
       p_patfc->p1.z_gm = zmax; 
       p_patfc->p2.x_gm = xmax;                /* Corner point 2    */
       p_patfc->p2.y_gm = ymin; 
       p_patfc->p2.z_gm = zmax; 
       p_patfc->p3.x_gm = xmax;                /* Corner point 3    */
       p_patfc->p3.y_gm = ymax; 
       p_patfc->p3.z_gm = zmax; 
       p_patfc->p4.x_gm = xmin;                /* Corner point 4    */
       p_patfc->p4.y_gm = ymax; 
       p_patfc->p4.z_gm = zmax; 
       }



     p_patfc->u1        = F_UNDEF;
     p_patfc->v1        = F_UNDEF;
     p_patfc->u2        = F_UNDEF;
     p_patfc->v2        = F_UNDEF;
     p_patfc->u3        = F_UNDEF;
     p_patfc->v3        = F_UNDEF;
     p_patfc->u4        = F_UNDEF;
     p_patfc->v4        = F_UNDEF;
     p_patfc->triangles = FALSE;         /* Two triangles           */
     p_patfc->defined   = TRUE;          /* Patch data exists       */
     p_patfc->p_att     = I_UNDEF;       /* Additional attributes   */


    }                                    /* End loop patches        */

/*!                                                                 */
/*     End loop all patches                 i_pat=1,2,3,4,5,6       */


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Surface header data (BOX_SUR, number of u and v patches, ... )   */
/*                                                                 !*/

/* Surface type and number of patches:                              */
          p_surout->typ_su = BOX_SUR;
          /* TODO Temporary until graphic routines accept BOX_SUR */
          p_surout->typ_su = FAC_SUR;
          p_surout->nu_su  = 1;  
          p_surout->nv_su  = 6; 
          p_surout->ngu_su = I_UNDEF; 
          p_surout->ngv_su = I_UNDEF;
          p_surout->ngseg_su[0] = 0;
          p_surout->ngseg_su[1] = 0;
          p_surout->ngseg_su[2] = 0;
          p_surout->ngseg_su[3] = 0;
          p_surout->ngseg_su[4] = 0;
          p_surout->ngseg_su[5] = 0;
          p_surout->pgseg_su[0] = DBNULL;
          p_surout->pgseg_su[1] = DBNULL;
          p_surout->pgseg_su[2] = DBNULL;
          p_surout->pgseg_su[3] = DBNULL;
          p_surout->pgseg_su[4] = DBNULL;
          p_surout->pgseg_su[5] = DBNULL;
            

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur179 Exit xmin %6.1f xmax %6.1f ymin %6.1f ymax %6.1f zmin %6.1f zmax %6.1f\n"
           , xmin,      xmax ,     ymin ,     ymax ,     zmin ,     zmax);
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


