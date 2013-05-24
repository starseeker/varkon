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
#include "../../IG/include/debug.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_ruled_l                   File: sur863.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a ruled surface (SUR_RULED), when input is two curves.   */
/*  The distribution (position) of the rulings (straight U lines)   */
/*  in the surface is defined by the arclength of the input curves. */
/*  The endpoints of a ruling will have the same relative arclength */
/*  on both curves.                                                 */
/*                                                                  */
/*  The input boundary curves will be constant V isoparameter       */
/*  curves in the output LFT_SUR surface. The input boundary        */
/*  curves will exactly be part of the output surface. There is     */
/*  no approximation (change) of the input curves                   */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-27   Originally written                                 */
/*  1997-05-22   acc 1 --> 0                                        */
/*  1999-12-01   Free source code modifications                     */
/*  2000-03-29   Bug: Start/end segment points if within tolerance  */
/*  2001-04-06   Bug: Problems with small dimensions. diff_t added  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_ruled_l    SUR_RULED as arclength LFT_SUR   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ctol             * Retrieve coordinate tolerance          */
/* GE109                   * Curve   evaluation routine             */
/* GE718                   * Relative arcl. for global U            */
/* GE717                   * Global U for relative arcl.            */
/* GE817                   * Trim a curve                           */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_ini_gmpatl       * Initialize GMPATL                      */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to buffer                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short ubounds();       /* Patch U boundary values            */
static short compare();       /* Compare two points w.r.t ctol      */
static short suralloc();      /* Allocate memory for surface        */
static short surcrea ();      /* Create the output surface          */
static short patcrea ();      /* Create one patch in surface        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint  surface_type;  /* Type of output surface              */
                             /* Eq. 3: LFT_SUR                      */
static DBint  n_ulines;      /* Number of U lines in ruled surface  */
static DBPatch *p_frst;      /* Pointer to the first patch          */
static DBfloat comptol;      /* Computer tolerance                  */
static DBfloat ctol;         /* Coordinate tolerance                */
static DBfloat spine_u[SPINE];/* Spine parameter values (curve 1)   */
static DBfloat curv2_u[SPINE];/* Boundary curve 2 parameter values  */
static DBint   n_spine_u;    /* Number of values in spine_u, curv2_u*/
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ........ failed in varkon_sur_ruled_l   */
/* SU2973 = Internal function (....) failed in varkon_sur_ruled_l   */
/* SU2993 = Severe program error (........) in varkon_sur_ruled_l   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

   DBstatus           varkon_sur_ruled_l (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1,      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1,      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2,      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2,      /* Segment data for p_cur2           (ptr) */
   DBint    r_l_case,    /* Ruled (lofting) surface case            */
                         /*   ... not yet used ....                 */
                         /*   Must be negative (= I_UNDEF)          */
   DBSurf  *p_surout,    /* Output surface                    (ptr) */
   DBPatch **pp_patout ) /* Alloc. area for topol. patch data (ptr) */
                         /* (pp_patout= NULL <==> not allocated)    */


/* Out:                                                             */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   short  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863 Enter***varkon_sur_ruled_l r_l_case %d\n", (int)r_l_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur863 p_cur1 %d p_seg1 %d\n", (int)p_cur1,  (int)p_seg1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur863 p_cur2 %d p_seg2 %d\n", (int)p_cur2,  (int)p_seg2 );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial( r_l_case, p_surout );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_ruled_l (sur863)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Calculate U patch boundary values for the output surface      */
/*                                                                 !*/

    status= ubounds ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"ubounds%%sur863");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 3. Allocate memory for the output surface                        */
/*                                                                 !*/

    status= suralloc ( p_surout, pp_patout );
    if (status<0) 
      {
      sprintf(errbuf,"suralloc%%sur863");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 4. Create the output surface                                     */
/*                                                                 !*/

    status= surcrea  ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"surcrea%%sur863");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%sur863");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 6. Exit                                                          */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863 Exit pp_patout %d\n", (int)*pp_patout );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial( r_l_case, p_surout )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    r_l_case;    /* Ruled (lofting) surface case              */
   DBSurf  *p_surout;    /* Output surface                    (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*initial* Enter ** \n");
  }
#endif

/*!                                                                 */
/* Computer   tolerance. Call of varkon_comptol (sur753).           */
/* Coordinate tolerance. Call of varkon_ctol    (sur751).           */
/*                                                                 !*/

   comptol   = varkon_comptol();
   ctol      = varkon_ctol();

/*!                                                                 */
  if ( r_l_case > 0 )
     {
     sprintf(errbuf,"r_l_case%%sur863");
     return(varkon_erpush("SU2993",errbuf));
    }

  surface_type = I_UNDEF;  /* Type of output surface              */
  n_ulines     = I_UNDEF;  /* Number of U lines in ruled surface  */
  p_frst       = NULL;     /* Pointer to the first patch          */

  p_surout->typ_su = I_UNDEF;
  p_surout->nu_su  = I_UNDEF; 
  p_surout->nv_su  = I_UNDEF;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur863*initial comptol %25.15f\n", comptol  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




/*!********* Internal ** function ** suralloc ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function allocates memory for the output surface             */

   static short suralloc( p_surout, pp_patout )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_surout;    /* Output surface                    (ptr) */
   DBPatch **pp_patout;  /* Alloc. area for topol. patch data (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   DBint  i_s;           /* Loop index surface patch record         */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*suralloc Enter ** \n");
  }
#endif


/*!                                                                 */
/* Surface type and number of patches in U and V to p_surout        */
/*                                                                 !*/

  n_ulines         = n_spine_u;
  p_surout->typ_su = LFT_SUR;
  p_surout->nu_su  = (short)(n_ulines-1);
  p_surout->nv_su  = (short)1; 

/*!                                                                 */
/* Dynamic allocation of area (pp_patout) for the topological       */
/* patches (patches of type TOP_PAT).                               */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,n_ulines-1))==NULL)
 {                                
 sprintf(errbuf, "(alloc)%%sur863*suralloc");
 return(varkon_erpush("SU2993",errbuf));
 }                                 

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_patout.                              */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= n_ulines-1;i_s= i_s+1) 
     {
     varkon_ini_gmpat (p_frst  +i_s-1);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur863*suralloc Memory allocated for %d TOP_PAT patches\n",
                   (int)n_ulines-1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*suralloc p_surout->typ_su %d  ->nu_su %d  ->nv_su %d\n",
     p_surout->typ_su, p_surout->nu_su,p_surout->nv_su);
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** surcrea  ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the surface                                 */

   static short surcrea ( p_cur1, p_seg1, p_cur2, p_seg2 )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
/*          p_frst          Pointer to the first patch              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  patch_type;    /* Type of geometric patch                 */
   DBSeg *p_curseg1;     /* Current curve 1 segment           (ptr) */
   DBSeg *p_curseg2;     /* Current curve 2 segment           (ptr) */
   DBint  i_seg;         /* Loop index segment in curve             */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATL *p_lofting;    /* Current geometric lofting   patch (ptr) */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
/*----------------------------------------------------------------- */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*surcrea  Enter ** \n");
  }
#endif

   p_lofting   = NULL;
   
   patch_type = LFT_PAT;

/*!                                                                 */
/* Start loop patches in the output surface i_seg= 1, . ,n_ulines-1 */
/*                                                                 !*/

   for ( i_seg = 0; i_seg < n_ulines-1;  ++i_seg ) 
     {

/*!                                                                 */
/*   Current pointers p_curseg1, p_curseg2 and p_t.                 */
/*                                                                 !*/

     p_curseg1 = p_seg1 + i_seg; /* Current curve 1 segment         */
     p_curseg2 = p_seg2 + i_seg; /* Current curve 2 segment         */
     p_t       = p_frst + i_seg; /* Current topological patch       */

/*!                                                                 */
/*   Dynamic allocation of area for one geometric patch.            */
/*   Call of function DBcreate_patches.                             */
/*                                                                 !*/

     if ((p_gpat=DBcreate_patches(patch_type,1)) ==NULL)
       {                                   
       sprintf(errbuf,"(allocg)%%sur863*surcrea"); 
       return(varkon_erpush("SU2993",errbuf));
       }                                 

/*   Current geometric patch p_lofting                             */
     p_lofting    = (GMPATL *)p_gpat; 

/*!                                                                 */
/*   Initialize current geometric patch p_lofting                   */
/*   Call of varkon_ini_gmpatl (sur765).                            */
/*                                                                 !*/

     varkon_ini_gmpatl (p_lofting);

/*!                                                                 */
/*   Topological patch data to current patch p_t                    */
/*                                                                 !*/

     p_t->styp_pat = (short)patch_type;  /* Type of secondary patch */
     p_t->spek_c   = p_gpat;             /* Secondary patch (C ptr) */
     p_t->su_pat   = 0;                  /* Topological adress      */
     p_t->sv_pat   = 0;                  /* secondary patch not def.*/
     p_t->iu_pat   = (short)(i_seg + 1); /* Topological adress for  */
     p_t->iv_pat   = (short)(    1    ); /* current (this) patch    */
     p_t->us_pat   = (DBfloat)i_seg+1.0; /* Start U on geom. patch  */
     p_t->ue_pat   = (DBfloat)i_seg+2.0  /* End   U on geom. patch  */
                               -comptol; /*                         */
     p_t->vs_pat   = 1.0;                /* Start V on geom. patch  */
     p_t->ve_pat   = 2.0-comptol;        /* End   V on geom. patch  */

     p_t->box_pat.xmin = 1.0;            /* BBOX  initiation        */
     p_t->box_pat.ymin = 2.0;            /*                         */
     p_t->box_pat.zmin = 3.0;            /*                         */
     p_t->box_pat.xmax = 4.0;            /*                         */
     p_t->box_pat.ymax = 5.0;            /*                         */
     p_t->box_pat.zmax = 6.0;            /*                         */
     p_t->box_pat.flag = -1;             /* Not calculated          */
    
     p_t->cone_pat.xdir = 1.0;           /* BCONE initiation        */
     p_t->cone_pat.ydir = 2.0;           /*                         */
     p_t->cone_pat.zdir = 3.0;           /*                         */
     p_t->cone_pat.ang  = 4.0;           /*                         */
     p_t->cone_pat.flag = -1;            /* Not calculated          */


/*!                                                                 */
/*   Geometrical patch data to current patch p_lofting              */
/*   Call of internal function patcrea.                             */
/*                                                                 !*/

    status= patcrea  ( p_cur1, p_seg1, p_cur2, p_seg2, 
                         p_lofting, i_seg );
    if (status<0) 
      {
      sprintf(errbuf,"patcrea%%sur863*surcrea");
      return(varkon_erpush("SU2973",errbuf));
      }


     }  /* End loop i_seg */
/*!                                                                 */
/* End   loop patches in the output surface i_seg= 0, . ,n_ulines-1 */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*surcrea  Surface created with %d LFT_PAT patches\n",
           (int)n_ulines-1 );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

/*!********* Internal ** function ** ubounds ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates U patch boundaries for the surface.      */
/* Output is an array of parameter values for the spine.            */

   static short ubounds ( p_cur1, p_seg1, p_cur2, p_seg2 )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  n_all_u;       /* Number of parameter values in all_u()   */
   DBfloat r_arcl;       /* Relative arclength boundary curve 2     */
   DBfloat u_arcl;       /* U value on boundary curve 2 for r_arcl  */
/*----------------------------------------------------------------- */

   DBTmat *p_csys;       /* Coordinate system                 (ptr) */
   DBfloat uglobs;       /* Global u start value                    */
   DBfloat uglobe;       /* Global u end   value                    */
   DBfloat all_u[SPINE]; /* All parameter values, incl. double pts  */
   DBfloat all_u_s[SPINE];/* All sorted parameter values            */
   DBfloat min_u;        /* Minimum parameter value                 */
   DBint  i_segm;        /* Loop index segment in curve             */
   DBint  i_compare;     /* Loop index solution                     */
   DBint  i_min_u;       /* Pointer to minimum value                */
   DBint  i_sol;         /* Loop index solution point               */
   DBint  comp;          /* Comparison flag                         */

   short  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds Find U patch boundary values for the surface\n");
}
#endif

/*!                                                                 */
/* 2. U boundary values from boundary curve 1 (also spine)          */
/* =======================================================          */
/*                                                                  */
/* Let all_u(i)= i for i=0, 1, 2, ... , (*p_cur1).ns_cu             */
/*                                                                 !*/

n_all_u = 0;          /* Initialize number of parameter values      */

for ( i_segm =0; i_segm <= (*p_cur1).ns_cu; ++i_segm )
  {
  n_all_u = n_all_u + 1;
  all_u[n_all_u-1] = (DBfloat)i_segm;
  }

/*!                                                                 */
/* 3. U boundary values from boundary curve 2                       */
/* ==========================================                       */
/*                                                                  */
/* Loop for i=   1, 2, ... , (*p_cur2).ns_cu - 1                    */
/*                                                                 !*/

p_csys = NULL;
uglobs = 1.0;

for ( i_segm =1; i_segm <= (*p_cur2).ns_cu-1; ++i_segm )
  {

/*!                                                                 */
/*   Calculate relative arclength to U= i on boundary curve 2       */
/*   Call of varkon_GE718 (GE718).                                */
/*                                                                 !*/

   uglobe = (DBfloat)i_segm + 1.0;

   status=GE718
   ((DBAny *)p_cur2, p_seg2, p_csys, uglobs, uglobe, &r_arcl);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds GE718 failed uglobs %f uglobe %f r_arcl %20.15f \n", 
     uglobs,  uglobe,  r_arcl  );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur863*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/*   Calculate U value on boundary curve 1 corresponding to the     */
/*   relative arclength.                                            */
/*   Call of varkon_GE717 (GE717).                                */
/*                                                                 !*/

   status=GE717
   ((DBAny *)p_cur1,p_seg1, NULL ,r_arcl, &u_arcl );
   if (status<0) 
        {
        sprintf(errbuf,"GE717%%sur863*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/*   Add the U value to array all_u.                                */
/*                                                                 !*/

  if ( n_all_u+1 > SPINE )
     {
     sprintf(errbuf,"n_all_u>SPINE%%sur863*ubounds");
     return(varkon_erpush("SU2993",errbuf));
     }

  n_all_u          = n_all_u + 1;
  all_u[n_all_u-1] = u_arcl - 1.0; 

  }

/*!                                                                 */
/* 4. Remove multiple points and order parameter values             */
/* ====================================================             */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Parameter values in increasing order (in array all_u_s).         */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*ubounds i_sol %d  all_u(i_sol-1) %f\n",(int)i_sol,all_u[i_sol-1]);
  }
}
#endif

for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  min_u   = all_u[i_sol-1];
  i_min_u = i_sol;
  for ( i_compare=1; i_compare<=n_all_u; ++i_compare  )
    {
    if ( all_u[i_compare-1] <  min_u )
      {
      min_u = all_u[i_compare-1];
      i_min_u = i_compare;
      }
    }
    all_u_s[i_sol-1] = min_u;
    all_u[i_min_u-1] = 50000.0;
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*ubounds i_sol %d  all_u_s(i_sol-1) %f\n",
         (int)i_sol,all_u_s[i_sol-1]);
  }
}
#endif

/*!                                                                 */
/* Delete equal points. Criterion varkon_ctol * 10.0                */
/*                                                                 !*/

spine_u[0]=all_u_s[0];
n_spine_u = 1;
for ( i_sol=2; i_sol<=n_all_u; ++i_sol  )
  {
   status = compare(p_cur1, p_seg1, 
     spine_u[n_spine_u-1]+1.0,all_u_s[i_sol-1]+1.0, &comp);     
   if (status<0) 
        {
        sprintf(errbuf,"compare%%sur863*ubounds");
        return(varkon_erpush("SU2973",errbuf));
        }


  if ( 1 == comp )
     {
     if ( n_spine_u + 1   >= SPINE )
        {
        sprintf(errbuf,"n_spine_u= %d >= %d%%sur863*ubounds",
                        (int)n_spine_u+1, SPINE);
        return(varkon_erpush("SU2993",errbuf));
        }
     spine_u[n_spine_u]=all_u_s[i_sol-1];
     n_spine_u = n_spine_u + 1;
     }
/* Switch to start/end segment point */
  if ( -2 == comp ) /* 2000-03-29 */
     {
     spine_u[n_spine_u-1]=all_u_s[i_sol-1];
     }
  }

/*!                                                                 */
/* 5. Calculate U parameter values for boundary curve 2             */
/* ====================================================             */
/*                                                                 !*/

/* Recalculations can be replaced by more sophisticated ordering    */
/* code (above), which order points in a table ....                 */

p_csys = NULL;
uglobs = 1.0;

for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {

/*!                                                                 */
/*   Calculate relative arclength to U= spine_u on boundary curve 1 */
/*   Call of varkon_GE718 (GE718).                                */
/*                                                                 !*/

   uglobe = spine_u[i_sol-1] + 1.0;

   status=GE718
   ((DBAny *)p_cur1, p_seg1, p_csys, uglobs, uglobe, &r_arcl);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds 2 GE718 failed uglobs %f uglobe %f r_arcl %20.15f \n", 
     uglobs,  uglobe,  r_arcl  );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur863*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*   Calculate U value on boundary curve 2 corresponding to the     */
/*   relative arclength.                                            */
/*   Call of varkon_GE717 (GE717).                                  */
/*                                                                 !*/

   status=GE717
   ((DBAny *)p_cur2,p_seg2, NULL ,r_arcl, &u_arcl );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 2%%sur863*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }

    curv2_u[i_sol-1]= u_arcl - 1.0;

/*!                                                                 */
/*   Check if value is close to segment start or end point.         */
/*   Change if compare regards them as equal points.                */
/*   Call of compare.                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds for compare  %f %f\n", 
     curv2_u[i_sol-1]+1.0,floor(curv2_u[i_sol-1])+1.0);     
fflush(dbgfil(SURPAC));
}
#endif
    status = compare(p_cur2, p_seg2, 
     curv2_u[i_sol-1]+1.0,floor(curv2_u[i_sol-1])+1.0,&comp);     
    if (status<0) 
         {
         sprintf(errbuf,"compare 2%%sur863*ubounds");
         return(varkon_erpush("SU2973",errbuf));
         }
/* Switch to start/end segment point */
    if ( comp < 1 ) /* 2000-03-29 */
       {
       curv2_u[i_sol-1]= floor(curv2_u[i_sol-1]);
       }

  }  /* End loop parameter values in curve 2 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds All U boundary curve 1 (spine) values for surface :\n");
for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*ubounds Point %d  spine_u(%2d) %f curv2_u %f\n",
         (int)i_sol,(int)i_sol-1,spine_u[i_sol-1], curv2_u[i_sol-1]);
  }
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** compare ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Compare two points with the identical points tolerance ctol      */

   static short compare(p_cur ,p_seg, u_glob1, u_glob2, p_comp )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Curve                             (ptr) */
  DBSeg   *p_seg;        /* Curve segments                    (ptr) */
  DBfloat  u_glob1;      /* Global parameter value point 1          */
  DBfloat  u_glob2;      /* Global parameter value point 2          */
  DBint   *p_comp;       /* Result flag:                            */
                         /* Eq. -1: Identical points                */
                         /* Eq. -2: Identical points. Switch  pts   */
                         /* Eq. +1: Not identical points            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz1;        /* Coordinates and derivatives for pt 1    */
   EVALC    xyz2;        /* Coordinates and derivatives for pt 2    */
/*-----------------------------------------------------------------!*/
   DBfloat  dist;        /* Distance R*3                            */
   DBfloat  dist_t;      /* Distance parameter t                    */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur863*compare parameter U= %f and U= %f Difference %f\n" ,
           u_glob1, u_glob2, u_glob2-u_glob1  );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                 !*/
   
   dist_t = fabs(u_glob2-u_glob1);

/*!                                                                 */
/* 2. Calculate points                                              */
/* ___________________                                              */
/*                                                                  */
/* Calculate point 1 and 2 for u_glob1 and u_glob2.                 */
/* Calls of varkon_GE109 (GE109).                                 */
/*                                                                 !*/

   xyz1.evltyp   = EVC_DR;

   xyz1.t_global = u_glob1;

   status=GE109 ((DBAny *)p_cur ,p_seg, &xyz1);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%sur863*compare");
        return(varkon_erpush("SU2943",errbuf));
        }

   xyz2.evltyp   = EVC_DR;

   xyz2.t_global = u_glob2;

   status=GE109 ((DBAny *)p_cur ,p_seg, &xyz2);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%sur863*compare");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 3. Calculate distance                                            */
/* _____________________                                            */
/*                                                                  */
/*                                                                 !*/
   dist=SQRT((xyz1.r.x_gm-xyz2.r.x_gm)*(xyz1.r.x_gm-xyz2.r.x_gm)+ 
             (xyz1.r.y_gm-xyz2.r.y_gm)*(xyz1.r.y_gm-xyz2.r.y_gm)+ 
             (xyz1.r.z_gm-xyz2.r.z_gm)*(xyz1.r.z_gm-xyz2.r.z_gm));

/*!                                                                 */
/* 4. Determine if points are equal                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                 !*/
   if   ( dist <= 400.0*ctol && dist_t < 0.1 ) *p_comp = -1;
   else                        *p_comp =  1;

/*!                                                                 */
/* 5. Determine if the equal points shall be switched               */
/* __________________________________________________               */
/*                                                                  */
/*                                                                 !*/

   if   (  -1 == *p_comp ) /* 2000-03-29 */
      {
      if ( fabs(xyz2.t_local)     < comptol    ||
           fabs(xyz2.t_local-1.0) < comptol       )
          {
          *p_comp = -2;
          }
      }
   
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && *p_comp == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*compare dist= %f 400*ctol= %f *p_comp= %d u_glob2 %f\n", 
           dist, 400.0*ctol , (int)*p_comp , u_glob2 );
  fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */


/********************************************************************/


/*!********* Internal ** function ** patcrea  ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates one geometrical patch in the surface        */

                 static short patcrea 
       ( p_cur1, p_seg1,p_cur2, p_seg2, p_lofting, i_seg )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
   GMPATL  *p_lofting;   /* Current geometric lofting   patch (ptr) */
   DBint    i_seg;       /* Loop index U patch                      */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBCurve tricur1;       /* Trimmed curve 1                         */
  DBCurve tricur2;       /* Trimmed curve 2                         */
  DBSeg   triseg1[3];    /* Segments for trimmed curve (1 segment)  */
  DBSeg   triseg2[3];    /* Segments for trimmed curve (1 segment)  */
/*----------------------------------------------------------------- */
  DBfloat uglobs1;       /* Global u start value, curve 1           */
  DBfloat uglobe1;       /* Global u end   value, curve 1           */
  DBfloat uglobs2;       /* Global u start value, curve 2           */
  DBfloat uglobe2;       /* Global u end   value, curve 2           */
  short   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */

#ifdef DEBUG
  DBfloat totarc1;       /* Total arclength curve 1                 */
  DBfloat totarc2;       /* Total arclength curve 2                 */
  DBfloat segarc1;       /* Arclength for current segment curve 1   */
  DBfloat segarc2;       /* Arclength for current segment curve 2   */
  DBfloat relarc1;       /* Relative arclength current segm. crv 1  */
  DBfloat relarc2;       /* Relative arclength current segm. crv 2  */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur863*patcrea  Enter ** i_seg %d\n", (int)i_seg );
  }
#endif

  uglobs1 = F_UNDEF; 
  uglobe1 = F_UNDEF;
  uglobs2 = F_UNDEF;
  uglobe2 = F_UNDEF;

/*!                                                                 */
/* 1. Spine curve, start limit curve and end tangent curve          */
/*                                                                 !*/

/*!                                                                 */
/* Trim boundary curve 1.                                           */
/* Call of varkon_GE817 (GE817).                                  */
/*                                                                 !*/

  uglobs1 = spine_u[i_seg]   + 1.0;
  uglobe1 = spine_u[i_seg+1] + 1.0;

  if  (  ABS(uglobe1-uglobs1) >= 3.0 )
    {
    sprintf(errbuf,"uglobe1-uglobs1%%sur863*patcrea");
    return(varkon_erpush("SU2993",errbuf));
    }

   status=GE817
    ((DBAny *)p_cur1, p_seg1,&tricur1,triseg1,uglobs1,uglobe1);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur863*cderiv GE817 failed uglobs1 %f uglobe1 %f diff %f \n",
                         uglobs1,uglobe1,  
                         uglobe1-uglobs1);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur863*patcrea");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Data from trimmed boundary curve 1 to current lofting patch      */
/*                                                                 !*/

  if ( tricur1.ns_cu > 1 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"uglobs1 %f uglobe1 %f diff %f tricur1.ns_cu %d\n",
                         uglobs1,uglobe1,  
                         uglobe1-uglobs1,tricur1.ns_cu);
fflush(dbgfil(SURPAC));
}
#endif
    sprintf(errbuf,"trimmed 1 ns_cu>1%%sur863*patcrea");
    return(varkon_erpush("SU2993",errbuf));
    }

  V3MOME((char *)(&triseg1),(char *)(&p_lofting->lims ),sizeof(DBSeg));
  V3MOME((char *)(&triseg1),(char *)(&p_lofting->tane ),sizeof(DBSeg));
  V3MOME((char *)(&triseg1),(char *)(&p_lofting->spine),sizeof(DBSeg));


/*!                                                                 */
/* 2. End limit curve and start tangent curve                       */
/*                                                                 !*/

/*!                                                                 */
/* Trim boundary curve 2.                                           */
/* Call of varkon_GE817 (GE817).                                  */
/*                                                                 !*/

  uglobs2 = curv2_u[i_seg]   + 1.0;
  uglobe2 = curv2_u[i_seg+1] + 1.0;

  if  (  ABS(uglobe2-uglobs2) >= 3.0 )
    {
    sprintf(errbuf,"uglobe2-uglobs2%%sur863*patcrea");
    return(varkon_erpush("SU2993",errbuf));
    }

   status=GE817
    ((DBAny *)p_cur2, p_seg2,&tricur2,triseg2,uglobs2,uglobe2);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur863*cderiv GE817 failed uglobs2 %f uglobe2 %f diff %f \n",
                         uglobs2,uglobe2,  
                         uglobe2-uglobs2);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur863*patcrea");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Data from trimmed boundary curve 2 to current lofting patch      */
/*                                                                 !*/

  if ( tricur2.ns_cu > 1 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"uglobs2 %f uglobe2 %f diff %f tricur2.ns_cu %d\n",
                         uglobs2,uglobe2,  
                         uglobe2-uglobs2,tricur2.ns_cu);
fflush(dbgfil(SURPAC));
}
#endif
    sprintf(errbuf,"trimmed 2 ns_cu>1%%sur863*patcrea");
    return(varkon_erpush("SU2993",errbuf));
    }

  V3MOME((char *)(&triseg2),(char *)(&p_lofting->lime ),sizeof(DBSeg));
  V3MOME((char *)(&triseg2),(char *)(&p_lofting->tans ),sizeof(DBSeg));

/*!                                                                 */
/* The P-value flag is used as flag for ruled arclength surface     */
/* (let p_flag= 4).                                                 */
/*                                                                 !*/

  p_lofting->p_flag = 4;

/*!                                                                 */
/* The offset for the patch is zero (0).                            */
/*                                                                 !*/

  p_lofting->ofs_pat = 0.0;

/*!                                                                 */
/* All other (unused) variables should perhaps be given values....  */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur863*patcrea uglobs1 %f uglobe1 %f diff %f i_seg %d\n",
                         uglobs1,uglobe1,  
                         uglobe1-uglobs1, (int)i_seg);
fprintf(dbgfil(SURPAC), 
"sur863*patcrea uglobs2 %f uglobe2 %f diff %f\n",
                         uglobs2,uglobe2,  
                         uglobe2-uglobs2);
fflush(dbgfil(SURPAC));
}
totarc1 = p_cur1->al_cu;
totarc2 = p_cur2->al_cu;
/*  segarc1 = tricur1.al_cu; Not calculated */
/*  segarc2 = tricur2.al_cu; Not calculated */
GEarclength((DBAny*)&tricur1,triseg1,&segarc1);
GEarclength((DBAny*)&tricur2,triseg2,&segarc2);
relarc1 = F_UNDEF;
relarc2 = F_UNDEF;
if  ( ABS(totarc1) > 0.000001 ) relarc1 = segarc1/totarc1;
if  ( ABS(totarc2) > 0.000001 ) relarc2 = segarc2/totarc2;
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur863*patcrea totarc1 %15.12f totarc2 %15.12f\n",
                         totarc1,totarc2 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur863*patcrea segarc1 %15.12f segarc2 %15.12f\n",
                         segarc1,segarc2 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur863*patcrea relarc1 %15.12f relarc2 %15.12f\n",
                         relarc1,relarc2 );
fflush(dbgfil(SURPAC));
}
/*2000-03-29 if  ( ABS(relarc1-relarc2) > 0.000001 ) */
if  ( ABS(relarc1-relarc2) > 0.001 )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
     "sur863*patcrea ERROR relarc1-relarc2 %15.12f > 0.001\n",
                         relarc1-relarc2 );
fflush(dbgfil(SURPAC));
}
sprintf(errbuf,"relarc1-relarc2%%sur863*patcrea");
varkon_erinit();
return(varkon_erpush("SU2993",errbuf));
}

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur863*patcrea  Exit \n");
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

