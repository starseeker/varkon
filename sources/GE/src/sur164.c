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
/*  Function: varkon_sur_curint                    File: sur164.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the intersect between a curve           */
/*  and a surface.                                                  */
/*                                                                  */
/*  Input is a surface, a curve and the requested solution number.  */
/*  Optionally may a start u,v point be input. In this case will    */
/*  the function first try to find the solution in the patch of     */
/*  the start UV point. If there is no solution then will the       */
/*  function try to find solutions in the surrounding patches.      */
/*  The input parameter (acc) defines the UV search area.           */
/*                                                                  */
/*  The UV search area will be the whole surface if no start        */
/*  UV point is defined. Note that this normally is a very          */
/*  heavy calculation !                                             */
/*                                                                  */
/*  Output is the intersect point for the requested solution number */
/*  The points will be ordered either according to R*3 distance     */
/*  to the given external R*3 point or in the u,v plane to the      */
/*  given U,V start point (if there is no start u,v point given     */
/*  will U= 1.0 and V= 1.0 be used).                                */
/*                                                                  */
/*  Note that this function not will end with an error if the       */
/*  requested solution number not exists and ocase > 1000.          */
/*  The output U,V point will only be (-1.0,-1.0) in this case.     */
/*  This is unlike all other geometry functions in surpac & geopac! */
/*  The reason for this option is that the surface/line intersect   */
/*  computation is so heavy. Using the MBS function N_INTERSECT     */
/*  first and then calculate the intersect if the solution exists   */
/*  is much too time-consuming.                                     */
/*                                                                  */
/*  Output from the function is also an array with all solutions.   */
/*  The solutions are ordered and there is also an option to        */
/*  select wanted solutions. For "ray-tracing" calculations are     */
/*  often only solutions in one shooting direction from the given   */
/*  point of interest. For rays which shall bounce on a surface is  */
/*  the surface normal direction of interest. A normal case is that */
/*  rays only shall bounce if the surface normal is in the same (or */
/*  opposite) direction of the shooting direction.                  */
/*                                                                  */
/*  TODO                                                            */
/*  ...  Extend curve ends                 !!!!!!                   */
/*       Facets not close to surface        !!!                     */
/*                                                                  */
/*                                                                  */
/* sur178 UV values must be mapped for NURBS !!!!!!                 */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-04   Originally written                                 */
/*  1996-06-13   Trimming                                           */
/*  1996-08-19   ocase > 1000 for N_INTERSECT                       */
/*  1996-10-11   Array size t_all, ... 4*INTMAX                     */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1998-02-09   Started to add NURBS ... probably finished ??      */
/*  1998-03-21   Dynamic allocation of PBOUND in sur919             */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_curint     Intersect curve and surface      */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_aplane       * Initialize APLANE                      */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ctol             * Coordinate tolerance                   */
/* varkon_ntol             * Surface normal tolerance               */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_ini_pbound       * Initialize PBOUND                      */
/* varkon_sur_selpatseg    * Select patches                         */
/* varkon_sur_aplane       * Approximate surface w APLANE           */
/* varkon_bpl_extend       * Extrapolate a B-plane                  */
/* varkon_cur_intbplan     * Intersect curve/B-plane                */
/* varkon_bpl_barycen      * Barycentric coordinates                */
/* varkon_pat_segint       * Segment/patch intersect                */
/* varkon_cur_order_2      * Ordering of U values                   */
/* varkon_sur_uvmap        * Map UV point                           */
/* varkon_sur_uvmapr       * Map UV pt from NURBS to MBS            */
/* v3free                  * Free allocated memory                  */
/* varkon_erinit           * Initialize error message stack         */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2613 = Requested solution no .. does not exist                 */
/* SU2623 = Requested solution number .. is not between 1 and SMAX  */
/* SU2943 = Called function .. failed in varkon_sur_curint          */
/* SU2813 = Start point u , v outside surface  varkon_sur_curint    */
/* SU2633 = Ordering case .. not implemented   varkon_sur_curint    */
/* SU2343 = Solution selection case .. not implemented              */
/* SU2993 = Severe program error ( ) in varkon_sur_curint           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_sur_curint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_sur,       /* Surface                           (ptr) */
  DBPatch  *p_pat,       /* Alloc. area for topol. patch data (ptr) */
  DBCurve  *p_cur,       /* Curve                             (ptr) */
  DBSeg    *p_seg,       /* Curve segment data                (ptr) */
  DBVector *p_s_uvpt,    /* Start U,V point                   (ptr) */
                         /* p_s_uvpt.z_gm < 0: Not defined.         */
  DBint     ocase,       /* Point ordering case                     */
                         /* Eq. 1: Ordered according to R*3 distance*/
                         /* Eq. 2: Ordered according to UV  distance*/
                         /* (No error exit if 1000 is added)        */
  DBint     acc,         /* Computation accuracy (case)             */
                         /* Undefined p_s_uvpt => all surf. patches */
                         /* Defined   p_s_uvpt => One patch  search */
                         /* Eq. 1: All patches connect. to p_s_uvpt */
                         /* Defined p_s_uvpt => Nine patches search */
                         /* Eq. 2: Next layer of patches ...        */
  DBint     sol_no,      /* Requested solution number               */

/* Out:                                                             */
  DBint    *p_no_points, /* Total number of solution points   (ptr) */
  DBVector *p_cpt,       /* Intersect point in R*3 space      (ptr) */
  DBVector *p_cuvpt,     /* Intersect point in U,V space      (ptr) */
  DBfloat  *p_uvalue,    /* Intersect U curve parameter value (ptr) */
  DBfloat   u_out[],     /* All U   curve   solution pts. Ordered   */
  DBVector  uv_out[],    /* All U,V surface solution pts. Ordered   */
  DBVector  r3_out[] )   /* All R*3 solution pts. Ordered           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  PBOUND *p_pbtable;     /* Patch boundary   table PBOUND           */
  PBOUND  pbrec;         /* Patch boundary record for initiation    */
  DBint   bnr;           /* Total number of records      in PBOUND  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  ISURCUR  curdat;       /* Surface/curve computation data          */
  DBint    rtype;        /* Computation case = SURCUR               */
  DBint    i_r;          /* Loop index record                       */
  DBfloat  comptol;      /* Computer   tolerance                    */
  DBfloat  ntol;         /* Surface normal tolerance                */
  DBfloat  ctol;         /* Coordinate tolerance                    */
  DBfloat  idpoint;      /* Identical point criterion               */
  DBfloat  idangle;      /* Identical angle criterion               */
  DBint    n_u;          /* Number of patches in U direction p_sur  */
  DBint    n_v;          /* Number of patches in V direction p_sur  */
  DBint    surtype;      /* Type CUB_SUR, RAT_SUR, or .....  p_sur  */
  DBfloat  s_lim[2][2];  /* Surface limitation U,V points    p_sur  */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  APLANE   asur[PLMAX];  /* Surface approximation table             */
  DBint    nasur;        /* Total number of records in asur         */
  APLANE   plrec;        /* Approximative patch to input surface    */
  DBint    sur_no;       /* Current surface for approximation       */
  DBint    i_fac;        /* Loop index facet in surface             */
  DBBplane sur_bpl;      /* B_PLANE (facet) from  surface           */
  DBint    nfac;         /* Number of intersect points in a facet   */
  DBfloat  u_fac[INTMAX];/* Ordered array of u solutions 1,., nint  */
  DBVector r_fac[INTMAX];/* Ordered array of R*3 solutions 1, nint  */
  DBint    n_all;        /* No of all approxim. solutions           */
  DBfloat  t_all[4*INTMAX];/* All approx. (not ordered) crv solut's */
  DBfloat  u_all[4*INTMAX];/* All approx. surface (U)       solut's */
  DBfloat  v_all[4*INTMAX];/* All approx. surface (V)       soluti' */
  DBint    i_sol;        /* Loop index solution point               */
  DBVector poi_fac;      /* Intersect point in B-plane              */
  DBfloat  alpha;        /* Barycentric coordinate for p1           */
  DBfloat  beta;         /* Barycentric coordinate for p2           */
  DBfloat  gamma;        /* Barycentric coordinate for p4           */
  EVALS    xyz_ps[SMAX]; /* All patch/segment     points Surface    */
  EVALC    xyz_psc[SMAX];/* All patch/segment     points Curve      */
  DBint    n_pspts;      /* Number of patch/segment points          */
  EVALS    xyz_in[SMAX]; /* All input  (solution) points Surface    */
  EVALC    xyz_inc[SMAX];/* All input  (solution) points Curve      */
  DBint    n_inpts;      /* Number of input  points                 */
  DBint    ocase_l;      /* Ordering case                           */
  DBVector refpt;        /* Comparison point (R*3 or U)             */
  EVALS    xyz_out[SMAX];/* All output (solution) points Surface    */
  EVALC    xyz_outc[SMAX];/* All output (solution) points Curve     */
  DBint    ord_outc[SMAX];/* The order of the output points         */
  DBint    n_outpts;     /* Number of output points                 */
  DBint    i_add;        /* Loop index additional intersect point   */
  DBint    i_s;          /* Loop index solution point               */
  DBfloat  u_mbs;        /* MBS    U parameter value                */
  DBfloat  v_mbs;        /* MBS    V parameter value                */
  DBfloat  u_map;        /* Mapped U parameter value                */
  DBfloat  v_map;        /* Mapped V parameter value                */
  DBfloat  u_nurb;       /* Global (NURBS) U parameter value        */
  DBfloat  v_nurb;       /* Global (NURBS) V parameter value        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */


#ifdef TILLFALLIG_TEST
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
#endif



/*!-------------------- Flow chart ---------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_curint    !                           */
/*               !      (sur164)        !                           */
/*               !______________________!                           */
/*       ___________________!_______________________________        */
/*  ____!_____         ______!______     _____!_____   _____!_____  */
/* !    1     !  2    !      3      !   !     4     ! !     5     ! */
/* !  Check   !       ! Computation !   ! Calculate ! ! Order and ! */
/* !  input,  !       !     data    !   !    all    ! !  delete   ! */
/* ! initiate !       !             !   !   points  ! ! equal pts ! */
/* !__________!       !_____________!   !___________! !___________! */
/*      !________                              !             !      */
/*  ____!___  ___!____                     ____!___      ____!___   */
/* !        !!        !                   !        !    !        !  */
/* ! varkon !! varkon !                   ! varkon !    ! varkon !  */
/* ! _ctol  !!        !                   !        !    !        !  */
/* ! _ntol  !!        !                   !        !    !        !  */
/* ! sur752 !!        !                   !        !    !        !  */
/* ! sur751 !!        !                   !        !    !        !  */
/* !________!!________!                   !________!    !________!  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 Enter***varkon_sur_curint   ** \n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 ocase %d\n",(int)ocase );
  fflush(dbgfil(SURPAC));
  }
#endif
 
#ifdef TILLFALLIG_TEST
if ((p_gpat=DBcreate_patches(CON_PAT,1)) ==NULL)
 { 
 sprintf(errbuf,
 "(allocg)%%varkon_nmgconsurf (sur204");
 return(varkon_erpush("SU2993",errbuf));
 }                                 
#endif

#ifdef DEBUG
   bnr     = I_UNDEF;
   nfac    = I_UNDEF;
   nasur   = I_UNDEF;
   n_inpts = I_UNDEF;
   u_mbs   = F_UNDEF;
   v_mbs   = F_UNDEF;
   u_map   = F_UNDEF;
   v_map   = F_UNDEF;
   u_nurb  = F_UNDEF;
   v_nurb  = F_UNDEF;
#endif


   rtype= SURCUR;  

/* Initialize patch boundary table pointer                          */
   p_pbtable = NULL;

/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_ntol (sur752) and varkon_ctol (sur751).           */
/*                                                                 !*/

   comptol   = varkon_comptol();
   ntol      = varkon_ntol();
   ctol      = varkon_ctol();
   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();


   for (i_s=1; i_s<=SMAX;     i_s= i_s+1) /* Initialize UV  solutions*/
     { 
     u_out [i_s-1]      = F_UNDEF;          
     uv_out[i_s-1].x_gm = F_UNDEF;          
     uv_out[i_s-1].y_gm = F_UNDEF;          
     uv_out[i_s-1].z_gm = F_UNDEF;           
     r3_out[i_s-1].x_gm = F_UNDEF;          
     r3_out[i_s-1].y_gm = F_UNDEF;          
     r3_out[i_s-1].z_gm = F_UNDEF;           
     }

   for ( i_sol = 1; i_sol <= 4*INTMAX; i_sol++ )
      {
      t_all[i_sol-1] = F_UNDEF;             
      u_all[i_sol-1] = F_UNDEF;             
      v_all[i_sol-1] = F_UNDEF;             
      }

   *p_uvalue = F_UNDEF;   


/*!                                                                 */
/* Partial check of p_s_uvpt (if < 1 )                              */
/*                                                                  */
/*                                                                 !*/

if ( p_s_uvpt->z_gm >= 0 )
{
  if ( p_s_uvpt->x_gm < 1.0 || p_s_uvpt->y_gm < 1 )
     {
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 )
       {
       fprintf(dbgfil(SURPAC),
       "sur164 Error start UV point %f %f is defined (z_gm= %f)\n",
            p_s_uvpt->x_gm,p_s_uvpt->y_gm,p_s_uvpt->z_gm);
       fflush(dbgfil(SURPAC));
       }
#endif
     sprintf(errbuf,"%6.2f %6.2f %%varkon_sur_curint   ", 
            p_s_uvpt->x_gm,p_s_uvpt->y_gm);
     varkon_erinit();
     return(varkon_erpush("SU2813",errbuf));
     }
} 

/* Map the UV start pt if defined. Call varkon_sur_uvmap (sur360)   */
/*   ..... p_s_uvpt seem not to be used ??? !!! 1998-02-08          */


   if  (   p_s_uvpt->z_gm >= 0.0 )
     {
     u_mbs = p_s_uvpt->x_gm - 1.0;   
     v_mbs = p_s_uvpt->y_gm - 1.0;   

      status=varkon_sur_uvmap
    (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
    if (status<0) 
      {
      sprintf(errbuf,"sur360%%sur164");
      return(erpush("SU2943",errbuf));
        }
     }
   else
     {
     u_map = p_s_uvpt->x_gm;   
     v_map = p_s_uvpt->y_gm;   
     }


   if ( sol_no >= 1 && sol_no <= SMAX )
      {
      /* OK */
      }
   else
     {
     sprintf(errbuf,"%d %% %d varkon_sur_curint   ", (int)sol_no , SMAX );
       varkon_erinit();
      return(varkon_erpush("SU2623",errbuf));
     }

   if      ( ocase ==    1 || ocase ==    2 )
      ocase_l = ocase;
   else if ( ocase == 1001 || ocase == 1002 )
      ocase_l = ocase - 1000;
   else
     {
     sprintf(errbuf,"%d %%varkon_sur_curint   ", (int)ocase );
       varkon_erinit();
      return(varkon_erpush("SU2633",errbuf));
     }


/*!                                                                 */
/* Initialize table APLANE                                          */
/* Calls of varkon_ini_aplane (sur760).                             */
/*                                                                 !*/

   for (i_r=1; i_r<=PLMAX; i_r= i_r+1)
     {                               
     varkon_ini_aplane (&plrec);
     asur[i_r-1] = plrec;
     }

/*!                                                                 */
/* Retrieve number of patches and surface types.                    */
/* Calls of varkon_sur_nopatch (sur230).                            */
/* Check that the surfaces are of an acceptable type                */
/*                                                                 !*/

    status = varkon_sur_nopatch
    (p_sur, &n_u, &n_v, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%% sur164");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if ( surtype     ==   CUB_SUR ||
     surtype     ==   RAT_SUR ||
     surtype     ==   MIX_SUR ||
     surtype     ==   CON_SUR ||
     surtype     ==  NURB_SUR ||
     surtype     ==   LFT_SUR )
 ;
 else
 {
 sprintf(errbuf, "(surface type)%% sur164"); 
 return(varkon_erpush("SU2993",errbuf));
 }




/*!                                                                 */
/* 2  Computation data                                              */
/* ____________________                                             */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Define tolerances, methods, etc for the intersect calculation.   */
/*                                                                 !*/

curdat.ctype    = rtype;          /* Type of calculation            */
curdat.method   = NEWTON;         /* Method to be used              */
curdat.nstart   = 10;             /* Number of restarts             */
curdat.maxiter  = 20;             /* Maximum number of restarts     */
curdat.s_method = 1;              /* Select/search method           */
                                  /* Eq. 1: BBOX'es only            */

/* Get sysyem tolerances      !!!!!! */

curdat.ctol     = ctol;           /* Coordinate     tolerance       */
curdat.ntol     = ntol;           /* Surface normal tolerance       */
curdat.idpoint  = idpoint;        /* Identical point criterion      */
curdat.idangle  = idangle;        /* Identical angle criterion      */
curdat.comptol  = comptol;        /* Computer accuracy tolerance    */
curdat.factol   = 0.500;          /* Facets (B-planes) tolerance    */
curdat.d_che    = 20.0;           /* Maximum check R*3 distance     */

                                  /* Density of the UV net:         */
curdat.u_add   =  5;              /* For surface 1 in U direction   */
curdat.v_add   =  5;              /* For surface 1 in V direction   */
                                  /* (Values 1, 2, 3, 4, .... )     */

                                  /* Patch limit pts (search area): */
curdat.us       = F_UNDEF;        /* U start surface                */
curdat.vs       = F_UNDEF;        /* V start surface                */
curdat.ue       = F_UNDEF;        /* U end   surface                */
curdat.ve       = F_UNDEF;        /* V end   surface                */

                                  /* Segment limits  (search area): */
curdat.usc      = F_UNDEF;        /* U start for curve              */
curdat.uec      = F_UNDEF;        /* U end   for curve              */

curdat.startuvpt.x_gm = F_UNDEF;  /* Start U,V point surface        */
curdat.startuvpt.y_gm = F_UNDEF; 
curdat.startuvpt.z_gm = F_UNDEF; 
curdat.startupt.x_gm  = F_UNDEF;  /* Start U   point curve          */
curdat.startupt.y_gm  = F_UNDEF; 
curdat.startupt.z_gm  = F_UNDEF; 


/*!                                                                 */
/* 3. Find the parts of the surface which may intersect the curve   */
/* ______________________________________________________________   */
/*                                                                  */
/* Select patches (boxes) which intersect the curve                 */
/* Call of varkon_sur_selpatseg (sur919).                           */
/*                                                                 !*/

s_lim[0][0] = 1.0;
s_lim[0][1] = 1.0;
s_lim[1][0] = (DBfloat)n_u;
s_lim[1][1] = (DBfloat)n_v;

    status = varkon_sur_selpatseg
(p_sur,p_pat,surtype, n_u, n_v, s_lim, 
 p_cur,p_seg,(IRUNON*)&curdat,&p_pbtable, &bnr );
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
  sprintf(errbuf,"sur919%%sur164");
  return(varkon_erpush("SU2943",errbuf));
  }

  if (  bnr  == 0 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 Curve does not intersect the (approximative BBOX) surface \n") ;
fflush(dbgfil(SURPAC));
}
#endif
    n_outpts  = 0;
    goto no_solut;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 Curve intersect %d patches (BBOXES) \n", (int)bnr) ;
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 4. Approximate the selected patches with triangular planes.      */
/* __________________________________________________________       */
/*                                                                  */
/*                                                                 !*/

sur_no = 1;
status=varkon_sur_aplane 
     (p_sur ,p_pat , p_pbtable, bnr, sur_no, asur , &nasur  );
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
        sprintf(errbuf,"sur178%%sur164 ");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 The surface is approximated with %d facets (triangular B-planes)\n", 
           (int)nasur ) ;
 fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 5. Calculate approximative intersect points                      */
/* ____________________________________________                     */
/*                                                                  */
/*                                                                 !*/

  n_all = 0;

/*!                                                                 */
/* Loop all patches in surface   i_fac =1,2,...., nasur             */
/*                                                                 !*/

for (i_fac =1; i_fac <= nasur ;  ++i_fac ) /* Start loop facets     */
  {

/*!                                                                 */
/*   Get B-plane (facet) i_fac from table asur (approx. surface)    */
/*   Extrapolate plane with distance idpoint and calculate          */
/*   intersect between the (extended) plane and the curve.          */
/*   Call of varkon_bpl_extend  and varkon_cur_intbplan             */
/*   !!! Extend ska flyttas till sur721 !!!!!                       */
/*   !!! Kanske ändå inte. Planen måste göras överlappande till     */
/*   !!! omkringliggande patches för att inte missa skärningar      */
/*   !!! som ligger nära patch kanter                               */
/*   !!! Alternativt, när approximativ skärning inte finns, kan     */
/*   !!! man söka skärning med alla patchar (PBOUND) och då         */
/*   !!! utan start punkt i U,V patchen 1996-02-04                  */
/*                                                                 !*/

     sur_bpl = asur[i_fac-1].bpl; 

     status = varkon_bpl_extend 
     ( &sur_bpl , 100*idpoint, &sur_bpl ); 
     if(status<0)
     {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
     sprintf(errbuf,"sur719%%sur164");
     return(varkon_erpush("SU2943",errbuf));
    }

     status = varkon_cur_intbplan
     (p_cur,p_seg,&sur_bpl , &nfac ,u_fac, r_fac );
#ifdef DEBUG
     if(status<0)
     {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
     sprintf(errbuf,"sur721%%sur164");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/*    Add global U values to arrays t_all, u_all, v_all and         */
/*    increase the total number of intersect points n_all.          */
/*                                                                 !*/

      if ( n_all + nfac > 4*INTMAX )
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 Error n_all %d + nfac %d > 4*INTMAX %d\n",(int)n_all,(int)nfac,4*INTMAX) ;
}
#endif
        sprintf(errbuf,"Increase 4*INTMAX%%sur164");
        return(varkon_erpush("SU2993",errbuf));
        }

      for ( i_sol = n_all; i_sol <= n_all+nfac-1; i_sol++ )
          {
          t_all[i_sol] = u_fac[i_sol-n_all];
          /* Interpolate with Barycentric coordinates for U,V in surface */
          poi_fac = r_fac[i_sol-n_all];
          status=varkon_bpl_barycen
         ( &sur_bpl, &poi_fac , &alpha, &beta, &gamma );
         if ( status < 0  )
           {
           if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
           sprintf(errbuf,"sur676%%sur164");
           return(varkon_erpush("SU2943",errbuf));
           }
          else
           {
           u_all[i_sol] = 
           alpha*asur[i_fac-1].u1bpl+ 
           beta *asur[i_fac-1].u2bpl+ 
           gamma*asur[i_fac-1].u4bpl; 
           v_all[i_sol] = 
           alpha*asur[i_fac-1].v1bpl+ 
           beta *asur[i_fac-1].v2bpl+ 
           gamma*asur[i_fac-1].v4bpl; 
          }
        
    }  /*  End loop solution points    */

      n_all = n_all + nfac;  /* Add solutions for current facet */

  }  /*  End loop facets    */

  if ( n_all == 0 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 Curve does not intersect the (approximative facet) surface \n") ;
}
#endif
    n_outpts  = 0;
    goto no_solut;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur164 There are %d approximative curve/facet surface intersect points\n", 
                (int)n_all ) ;
}
if ( dbglev(SURPAC) == 1 )
{
for ( i_sol = 1; i_sol <= n_all;  i_sol++ )
{
fprintf(dbgfil(SURPAC),
"sur164 Pt i_sol %d n_all %d u_sol %7.3f v_sol %7.3f t_sol %7.3f\n", 
 (int)i_sol, (int)n_all, u_all[i_sol-1], v_all[i_sol-1], t_all[i_sol-1] );
if  ( i_sol == 1 ) /* No r_fac_all !!! */
{
fprintf(dbgfil(SURPAC),
"sur164 Pt i_sol %d n_all %d r_fac %15.8f %15.8f %15.8f\n", 
 (int)i_sol, (int)n_all, r_fac[i_sol-1].x_gm, r_fac[i_sol-1].y_gm, r_fac[i_sol-1].z_gm );
}
}
}
#endif

/*!                                                                 */

/*!                                                                 */
/* 6. Calculate the exact solution points                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Start loop all (approxim.) solutions i_sol= 1, 2, .... n_all     */
/*                                                                 !*/

   n_inpts = 0;          /* Number of input  points                 */

   for ( i_sol = 1; i_sol <= n_all; i_sol++ )
      {
/*!                                                                 */
/*    Current patch, segment and start points to ISURCUR            */
/*                                                                 !*/

      /* Curve segment limitations                                  */
      curdat.usc = floor(t_all[i_sol-1]+comptol);
      curdat.uec = floor(t_all[i_sol-1]+comptol) + 1.0 -comptol;

      /* Surface patch limitations                                  */
      for (i_r=1; i_r<= bnr ; i_r= i_r+1) /* Start loop pbound      */
        {                                 /*                        */
        pbrec = *(p_pbtable+i_r-1);       /* Current record         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 Approximate solution point i_sol %d PBOUND record i_r %d\n", 
   (int)i_sol, (int)i_r);
  fprintf(dbgfil(SURPAC),
  "sur164 u_all %f v_all %f t_all %f \n",
   u_all[i_sol-1], v_all[i_sol-1], t_all[i_sol-1] );
  fprintf(dbgfil(SURPAC),
  "sur164 us %f vs %f ue %f ve %f\n",
   pbrec.us, pbrec.ue, pbrec.vs, pbrec.ve );
  }
#endif
        if (u_all[i_sol-1] >= pbrec.us - comptol &&
            u_all[i_sol-1] <= pbrec.ue + comptol &&
            v_all[i_sol-1] >= pbrec.vs - comptol &&
            v_all[i_sol-1] <= pbrec.ve + comptol   )
          {
          /* PBOUND patch found                                     */
          curdat.us = pbrec.us;  
          curdat.ue = pbrec.ue;  
          curdat.vs = pbrec.vs;  
          curdat.ve = pbrec.ve;   
          goto found;
          }
        }                                 /* End   loop pbound      */

#ifdef DEBUG  /* There must be a PBOUND patch  */
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
        sprintf(errbuf,"PBOUND not found%%varkon_sur_curint");
        return(varkon_erpush("SU2993",errbuf));
#endif        /* There must be a PBOUND patch  */

found:;

      curdat.startuvpt.x_gm = u_all[i_sol-1];  /* Start pt surface  */
      curdat.startuvpt.y_gm = v_all[i_sol-1]; 
      curdat.startuvpt.z_gm = 0.0;       
      curdat.startupt.x_gm  = t_all[i_sol-1];  /* Start pt curve    */
      curdat.startupt.y_gm  = 0.0;
      curdat.startupt.z_gm  = 0.0;
   
    status = varkon_pat_segint
(p_sur,p_pat,p_cur,p_seg,(IRUNON*)&curdat,&n_pspts,xyz_ps,xyz_psc);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 varkon_pat_seg_int (sur904) failed \n" );
  }
#endif
  sprintf(errbuf,"sur904%%sur164");
  return(varkon_erpush("SU2943",errbuf));
  }

      if  (  n_pspts > 0 )
        {
        for ( i_add = n_inpts; i_add <= n_inpts + n_pspts; i_add++ )
           {
           xyz_in[i_add]  = xyz_ps[i_add-n_inpts];
           xyz_inc[i_add] = xyz_psc[i_add-n_inpts];
           }
        }

      n_inpts = n_inpts + n_pspts; /* Number of input  points       */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 Result i_sol %d n_pspts (sur904) %d n_inpts (total) %d\n", 
        (int)i_sol, (int)n_pspts, (int)n_inpts );
  }
#endif
 

      }  /*  End loop solution points    */
/*!                                                                 */
/* End   loop all (approxim.) solutions i_sol= 1, 2, .... n_all     */
/*                                                                 !*/


/*!                                                                 */
/* Order solution points and delete equal solutions.                */
/* Call of varkon_cur_order (sur983)                                */
/*                                                                 !*/


   if      ( n_inpts  == 0  ) n_outpts = 0;         
   else if ( n_inpts  == 1  ) 
     {
     n_outpts = 1;
     xyz_outc[0] = xyz_inc[0];
     ord_outc[0] = 1;
     xyz_out[0] = xyz_in[0];
     }
   else if ( n_inpts  >  1 )
     {

     refpt.x_gm = 1.0;            
     refpt.y_gm = 0.0;              
     refpt.z_gm = 0.0;

     ocase_l    = 2;

status=varkon_cur_order_2
   (&refpt, xyz_inc, n_inpts , ocase_l, xyz_outc, ord_outc, &n_outpts);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 varkon_cur_order (sur983) failed \n" );
  }
#endif
        sprintf(errbuf,"sur983%%sur164");
        return(varkon_erpush("SU2943",errbuf));
        }

     for (i_s=1; i_s<=n_outpts; i_s= i_s+1) /* Reorder xyz_out         */
       { 
       xyz_out[i_s-1] = xyz_in[ord_outc[i_s-1]-1];
       }
     }       /* End n_inpts > 0 */


   *p_no_points= n_outpts;     /* Total number of solution points   */

   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) /* Start loop all solutions*/
     { 
     u_out [i_s-1]      = xyz_outc[i_s-1].t_global;
     uv_out[i_s-1].x_gm = xyz_out [i_s-1].u;
     uv_out[i_s-1].y_gm = xyz_out [i_s-1].v;
     uv_out[i_s-1].z_gm = 0.0;               
     r3_out[i_s-1].x_gm = xyz_out [i_s-1].r_x;
     r3_out[i_s-1].y_gm = xyz_out [i_s-1].r_y;
     r3_out[i_s-1].z_gm = xyz_out [i_s-1].r_z;
     }


/*!                                                                 */
/* Map the UV points (for a NURBS surface).                         */
/* Calls of varkon_sur_uvmapr (sur362).                              */
/*                                                                 !*/


   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) /* Start loop all solutions*/
     { 
     u_nurb = uv_out[i_s-1].x_gm;
     v_nurb = uv_out[i_s-1].y_gm;
       status=varkon_sur_uvmapr
     (p_sur,p_pat,u_nurb, v_nurb, &u_mbs,&v_mbs);
     if (status<0) 
       {
       if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
       sprintf(errbuf,"sur362%%sur164");
       return(erpush("SU2943",errbuf));
         }
       uv_out[i_s-1].x_gm = u_mbs + 1.0;
       uv_out[i_s-1].y_gm = v_mbs + 1.0;
     } /* End loop all solution points */



no_solut:; /* No solution */

   if ( sol_no <= n_outpts ) 
      {
      p_cpt->x_gm   = xyz_out[sol_no-1].r_x;
      p_cpt->y_gm   = xyz_out[sol_no-1].r_y;
      p_cpt->z_gm   = xyz_out[sol_no-1].r_z;

/* 1998-02-08 p_cuvpt->x_gm = xyz_out[sol_no-1].u; */
/* 1998-02-08 p_cuvpt->y_gm = xyz_out[sol_no-1].v; */
      p_cuvpt->x_gm = uv_out[sol_no-1].x_gm;
      p_cuvpt->y_gm = uv_out[sol_no-1].y_gm;
      p_cuvpt->z_gm = 0.0;               
      *p_uvalue     = xyz_outc[sol_no-1].t_global; 
      }
   else
      {
      p_cpt->x_gm   =   1.23456789;         
      p_cpt->y_gm   =   1.23456789;         
      p_cpt->z_gm   =   1.23456789;         

      p_cuvpt->x_gm =  -1.0;                
      p_cuvpt->y_gm =  -1.0;                
      p_cuvpt->z_gm =  -1.0;               
      *p_uvalue     =  -1.0;
      if ( ocase < 1000 )
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");
        sprintf(errbuf,"%d %% %d ", (int)sol_no , (int)n_outpts );
       varkon_erinit();
       return(varkon_erpush("SU2283",errbuf));
        }
     } /* End no solution */

/* 6. Exit                                                          */

/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && p_pbtable != NULL )
{
fprintf(dbgfil(SURPAC),
"sur164 PBOUND memory is freed \n");
fflush(dbgfil(SURPAC));
}
#endif

  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur164");


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
   for (i_s=1; i_s<= *p_no_points; i_s= i_s+1) /* Start loop all solutions*/
     { 
     fprintf(dbgfil(SURPAC),
     "sur164 Solution %d u_out %f uv_out %f %f\n"
    , (int)i_s , u_out[i_s-1], uv_out[i_s-1].x_gm, uv_out[i_s-1].y_gm );
     fprintf(dbgfil(SURPAC),
     "sur164 Solution %d r3_out %f %f %f\n"
    , (int)i_s , r3_out[i_s-1].x_gm, r3_out[i_s-1].y_gm, r3_out[i_s-1].z_gm );
     }
  if ( *p_no_points > 0   )
  {
  fprintf(dbgfil(SURPAC),
  "sur164 Exit varkon_sur_curint     Output U %f UV pt %f %f Total no %d\n"
    ,*p_uvalue, p_cuvpt->x_gm , p_cuvpt->y_gm , (int)*p_no_points  );
  }
  else
  {
  fprintf(dbgfil(SURPAC),
  "sur164 Exit***varkon_sur_curint   ** No solution found (=%d)\n", 
                     (int)*p_no_points);
  }
  fflush(dbgfil(SURPAC)); /* From buffer to file */
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
