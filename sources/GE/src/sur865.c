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
/*  Function: varkon_sur_ruled_d                   File: sur865.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  TODO: Function is not finished                                  */
/*                                                                  */
/*  Create a ruled surface (SUR_RULED), when input is two curves.   */
/*  The distribution (position) of the rulings (straight U lines)   */
/*  in the surface is defined by a developability criterion. The    */
/*  surface normal will have the same direction along the whole     */
/*  ruling (for all rulings).                                       */
/*                                                                  */
/*  The input boundary curves will be constant V isoparameter       */
/*  curves in the output LFT_SUR surface. The input boundary        */
/*  curves will exactly be part of the output surface. There is     */
/*  no approximation (change) of the input curves.                  */
/*                                                                  */
/*  There is a special evaluation function for the calculation      */
/*  of the position of a developable ruling ..............          */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1999-01-09   Originally written                                 */
/*  1999-12-02   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_ruled_d    SUR_RULED developable LFT_SUR    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Flow chart ---------------------------------------*/
/*                 ____________________                             */
/*                !                    !                            */
/*                ! varkon_sur_ruled_d !                            */
/*                !     (sur865)       !                            */
/*                !____________________!                            */
/*                         !                                        */
/*        _________________!                                        */
/*   ____!____      _______!__________                              */
/*  !         !    !                  !                             */
/*  ! initial !    !     dbounds      !                             */
/*  !_________!    ! U patch boundary !                             */
/*                 !  values for the  !                             */
/*                 !  output surface  !                             */
/*                 !     Output:      !                             */
/*                 ! spine_u  curv2_u !                             */
/*                 !__________________!                             */
/*                         !                                        */
/*        _________________!_____________________________           */
/*  _____!________    _____!________                                */
/* !              !  !              !                               */
/* !   dcurvei    !  !   dcurvei    !                               */
/* ! Curve p_cur1 !  ! Curve p_cur2 !                               */
/* !  is master   !  !  is master   !                               */
/* !   Output:    !  !   Output:    !                               */
/* !   n_all_1    !  !   n_all_2    !                               */
/* !  all_1_2[]   !  !  all_2_1[]   !                               */
/* !   d_c_1[]    !  !   d_c_2[]    !                               */
/* !______________!  !______________!                               */
/*       !                 !                                        */
/*  _____!_______     _____!_______                                 */
/* !             !   !             !                                */
/* ! varkon_cur  !   ! varkon_cur  !                                */
/* ! _develop    !   ! _develop    !                                */
/* !  (sur905)   !   !  (sur905)   !                                */
/* !  Input:     !   !  Input:     !                                */
/* ! Pt + curve  !   !  Pt + curve !                                */
/* !  devdat     !   !  devdat     !                                */
/* ! Output:     !   ! Output:     !                                */
/* ! Developable !   ! Developable !                                */
/* ! rulings and !   ! rulings and !                                */
/* ! codes/types !   ! codes/types !                                */
/* !_____________!   !_____________!                                */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
/*                                                                  */
/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_cur_develop      * Developable ruling                     */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ctol             * Retrieve coordinate tolerance          */
/* varkon_ntol             * Surface normal tolerance               */
/* varkon_idpoint          * Identical points criterion             */
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
static short dbounds();       /* Patch U develop. boundary values   */
static short dcurvei();       /* Patch U develop. boundary curve i  */
static short rulcand();       /* Get next ruling candidate          */
static short rulcomp();       /* Select ruling from candidates      */
static short rulupda();       /* Update current index for a curve   */
static short add_rul();       /* Add ruling to spine_u and curv2_u  */
#ifdef  DEBUG
static short mbsprog();       /* Create MBS program                 */
#endif

static short compare();       /* Compare two points w.r.t ctol      */
static short suralloc();      /* Allocate memory for surface        */
static short surcrea ();      /* Create the output surface          */
static short patcrea ();      /* Create one patch in surface        */
static short ubounds();       /* Patch U boundary values            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint  surface_type;  /* Type of output surface              */
                             /* Eq. 3: LFT_SUR                      */
static DBint   n_ulines;     /* Number of U lines in ruled surface  */
static DBPatch *p_frst;      /* Pointer to the first patch          */
static DBfloat comptol;      /* Computer tolerance                  */
static DBfloat ctol;         /* Coordinate tolerance                */
static DBfloat ntol;         /* Surface normal tolerance.           */
static DBfloat idpoint;      /* Identical point criterion           */
static DBfloat spine_u[SPINE];/* Spine parameter values (curve 1)   */
static DBfloat curv2_u[SPINE];/* Boundary curve 2 parameter values  */
static DBint   n_spine_u;    /* Number of values in spine_u, curv2_u*/
static DEVRUL  devdat;       /* Developable ruling computation data */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ........ failed in varkon_sur_ruled_d   */
/* SU2973 = Internal function (....) failed in varkon_sur_ruled_d   */
/* SU2993 = Severe program error (........) in varkon_sur_ruled_d   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_ruled_d (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1,      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1,      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2,      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2,      /* Segment data for p_cur2           (ptr) */
   DBint    r_d_case,    /* Ruled (developable) surface case        */
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
"sur865 Enter***varkon_sur_ruled_d r_d_case %d\n", (int)r_d_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865 p_cur1 %d p_seg1 %d\n", (int)p_cur1,  (int)p_seg1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865 p_cur2 %d p_seg2 %d\n", (int)p_cur2,  (int)p_seg2 );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial( r_d_case, p_surout );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_ruled_d (sur865)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Calculate U patch boundary values for the output surface      */
/*                                                                 !*/

    status= dbounds ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"dbounds%%sur865");
      return(varkon_erpush("SU2973",errbuf));
      }

    status= ubounds ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"ubounds%%sur865");
      return(varkon_erpush("SU2973",errbuf));
      }


/*!                                                                 */
/* 3. Allocate memory for the output surface                        */
/*                                                                 !*/

    status= suralloc ( p_surout, pp_patout );
    if (status<0) 
      {
      sprintf(errbuf,"suralloc%%sur865");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 4. Create the output surface                                     */
/*                                                                 !*/

    status= surcrea  ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"surcrea%%sur865");
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
        sprintf(errbuf,"varkon_sur_bound%%sur865");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 6. Exit                                                          */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865 Exit pp_patout %d\n", (int)*pp_patout );
fflush(dbgfil(SURPAC)); 
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

   static short initial( r_d_case, p_surout )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    r_d_case;    /* Ruled (developable) surface case        */
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
  "sur865*initial* Enter ** \n");
  }
#endif

/*!                                                                 */
/* Computer       tolerance. Call of varkon_comptol (sur753).       */
/* Coordinate     tolerance. Call of varkon_ctol    (sur751).       */
/* Surface normal tolerance. Call of varkon_ntol    (sur752).       */
/* Identical pts  criterion. Call of varkon_idpoint (sur741).       */
/*                                                                 !*/

   comptol   = varkon_comptol();
   ctol      = varkon_ctol();
   ntol      = varkon_ntol();
   idpoint   = varkon_idpoint();

/*!                                                                 */
  if ( r_d_case > 0 )
     {
     sprintf(errbuf,"r_d_case%%sur865");
     return(varkon_erpush("SU2993",errbuf));
     }

  surface_type = I_UNDEF;  /* Type of output surface              */
  n_ulines     = I_UNDEF;  /* Number of U lines in ruled surface  */
  p_frst       = NULL;     /* Pointer to the first patch          */

  p_surout->typ_su = I_UNDEF;
  p_surout->nu_su  = I_UNDEF; 
  p_surout->nv_su  = I_UNDEF;

/*!                                                                 */
/* Computation case rtype and plane to structure variable devdat.   */
/* Selected method (NEWTON), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable devdat.               */
/*                                                                 !*/


  devdat.ctype   = DEVELRUL;
  devdat.method  =   NEWTON;
  devdat.nstart  =        3;      
  devdat.maxiter =       20;      
  
  devdat.ctol    =     ctol;
  devdat.ntol    =     ntol;
  devdat.comptol =  comptol;
  devdat.idpoint =  idpoint;
  devdat.idangle =  F_UNDEF;
  devdat.d_dist  =  F_UNDEF;
  devdat.d_ang   =  F_UNDEF;
  
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*initial comptol %25.15f\n", comptol  );
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
  "sur865*suralloc Enter ** \n");
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
 sprintf(errbuf, "(alloc)%%sur865*suralloc");
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
"sur865*suralloc Memory allocated for %d TOP_PAT patches\n",
                   (int)n_ulines-1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*suralloc p_surout->typ_su %d  ->nu_su %d  ->nv_su %d\n",
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
  "sur865*surcrea  Enter ** \n");
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
       sprintf(errbuf,"(allocg)%%sur865*surcrea"); 
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
      sprintf(errbuf,"patcrea%%sur865*surcrea");
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
"sur865*surcrea  Surface created with %d LFT_PAT patches\n",
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
   DBint   n_all_u;      /* Number of parameter values in all_u()   */
   DBfloat r_arcl;       /* Relative arclength boundary curve 2     */
   DBfloat u_arcl;       /* U value on boundary curve 2 for r_arcl  */
/*----------------------------------------------------------------- */

   DBTmat *p_csys;       /* Coordinate system                 (ptr) */
   DBfloat uglobs;       /* Global u start value                    */
   DBfloat uglobe;       /* Global u end   value                    */
   DBfloat all_u[SPINE]; /* All parameter values, incl. double pts  */
   DBfloat all_u_s[SPINE];/* All sorted parameter values            */
   DBfloat min_u;        /* Minimum parameter value                 */
   DBint   i_segm;       /* Loop index segment in curve             */
   DBint   i_compare;    /* Loop index solution                     */
   DBint   i_min_u;      /* Pointer to minimum value                */
   DBint   i_sol;        /* Loop index solution point               */
   DBint   comp;         /* Comparison flag                         */

   short   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

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
"sur865*ubounds Find U patch boundary values for the surface\n");
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
/*   Call of GE718.                                                 */
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
"sur865*ubounds GE718 failed uglobs %f uglobe %f r_arcl %20.15f \n", 
     uglobs,  uglobe,  r_arcl  );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur865*ubounds");
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
        sprintf(errbuf,"GE717%%sur865*ubounds");
        return(erpush("SU2943",errbuf));
        }


/*!                                                                 */
/*   Add the U value to array all_u.                                */
/*                                                                 !*/

  if ( n_all_u+1 > SPINE )
     {
     sprintf(errbuf,"n_all_u>SPINE%%sur865*ubounds");
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
  "sur865*ubounds i_sol %d  all_u(i_sol-1) %f\n",
             (int)i_sol,all_u[i_sol-1]);
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
  "sur865*ubounds i_sol %d  all_u_s(i_sol-1) %f\n",
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
        sprintf(errbuf,"compare%%sur865*ubounds");
        return(varkon_erpush("SU2973",errbuf));
        }


  if ( comp == 1 )
     {
     if ( n_spine_u + 1   >= SPINE )
        {
        sprintf(errbuf,"n_spine_u= %d >= %d%%sur865*ubounds",
                        (int)n_spine_u+1, SPINE);
        return(varkon_erpush("SU2993",errbuf));
        }
     spine_u[n_spine_u]=all_u_s[i_sol-1];
     n_spine_u = n_spine_u + 1;
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
"sur865*ubounds 2 GE718 failed uglobs %f uglobe %f r_arcl %20.15f \n", 
     uglobs,  uglobe,  r_arcl  );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur865*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*   Calculate U value on boundary curve 2 corresponding to the     */
/*   relative arclength.                                            */
/*   Call of varkon_GE717 (GE717).                                */
/*                                                                 !*/

   status=GE717
   ((DBAny *)p_cur2,p_seg2, NULL ,r_arcl, &u_arcl );
   if (status<0) 
        {
        sprintf(errbuf,"GE717 2%%sur865*ubounds");
        return(varkon_erpush("SU2943",errbuf));
        }

    curv2_u[i_sol-1]= u_arcl - 1.0;

  }  /* End loop parameter values in curve 2 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*ubounds All U boundary curve 1 (spine) values for surface :\n");
for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur865*ubounds Point %d  spine_u(%2d) %f curv2_u %f\n",
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
                         /* Eq. +1: Not identical points            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz1;        /* Coordinates and derivatives for pt 1    */
   EVALC    xyz2;        /* Coordinates and derivatives for pt 2    */
/*-----------------------------------------------------------------!*/
   DBfloat  dist;        /* Distance                                */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur865*compare parameter U= %f and U= %f Difference %f\n" ,
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
        sprintf(errbuf,"GE109%%sur865*compare");
        return(varkon_erpush("SU2943",errbuf));
        }

   xyz2.evltyp   = EVC_DR;

   xyz2.t_global = u_glob2;

   status=GE109 ((DBAny *)p_cur ,p_seg, &xyz2);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%sur865*compare");
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
   if   ( dist <= 400.0*ctol ) *p_comp = -1;
   else                        *p_comp =  1;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && *p_comp == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*compare dist= %f 400*ctol= %f *p_comp= %d u_glob2 %f\n", 
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
  DBCurve  tricur1;      /* Trimmed curve 1                         */
  DBCurve  tricur2;      /* Trimmed curve 2                         */
  DBSeg    triseg1[3];   /* Segments for trimmed curve (1 segment)  */
  DBSeg    triseg2[3];   /* Segments for trimmed curve (1 segment)  */
/*----------------------------------------------------------------- */
  DBfloat  uglobs1;      /* Global u start value, curve 1           */
  DBfloat  uglobe1;      /* Global u end   value, curve 1           */
  DBfloat  uglobs2;      /* Global u start value, curve 2           */
  DBfloat  uglobe2;      /* Global u end   value, curve 2           */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

#ifdef DEBUG
  DBfloat  totarc1;      /* Total arclength curve 1                 */
  DBfloat  totarc2;      /* Total arclength curve 2                 */
  DBfloat  segarc1;      /* Arclength for current segment curve 1   */
  DBfloat  segarc2;      /* Arclength for current segment curve 2   */
  DBfloat  relarc1;      /* Relative arclength current segm. crv 1  */
  DBfloat  relarc2;      /* Relative arclength current segm. crv 2  */
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
  "sur865*patcrea  Enter ** i_seg %d\n", (int)i_seg );
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
/* Call of GE817.                                                   */
/*                                                                 !*/

  uglobs1 = spine_u[i_seg]   + 1.0;
  uglobe1 = spine_u[i_seg+1] + 1.0;

  if  (  ABS(uglobe1-uglobs1) >= 3.0 )
    {
    sprintf(errbuf,"uglobe1-uglobs1%%sur865*patcrea");
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
"sur865*cderiv GE817 failed uglobs1 %f uglobe1 %f diff %f \n",
                         uglobs1,uglobe1,  
                         uglobe1-uglobs1);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur865*patcrea");
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
    sprintf(errbuf,"trimmed 1 ns_cu>1%%sur865*patcrea");
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
    sprintf(errbuf,"uglobe2-uglobs2%%sur865*patcrea");
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
"sur865*cderiv GE817 failed uglobs2 %f uglobe2 %f diff %f \n",
                         uglobs2,uglobe2,  
                         uglobe2-uglobs2);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur865*patcrea");
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
    sprintf(errbuf,"trimmed 2 ns_cu>1%%sur865*patcrea");
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
"sur865*patcrea uglobs1 %f uglobe1 %f diff %f i_seg %d\n",
                         uglobs1,uglobe1,  
                         uglobe1-uglobs1, (int)i_seg);
fprintf(dbgfil(SURPAC), 
"sur865*patcrea uglobs2 %f uglobe2 %f diff %f\n",
                         uglobs2,uglobe2,  
                         uglobe2-uglobs2);
fflush(dbgfil(SURPAC));
}
totarc1 = p_cur1->al_cu;
totarc2 = p_cur2->al_cu;
/*  segarc1 = tricur1.al_cu; Not calculated */
/*  segarc2 = tricur2.al_cu; Not calculated */
GEarclength(&tricur1,triseg1,&segarc1);
GEarclength(&tricur2,triseg2,&segarc2);
relarc1 = F_UNDEF;
relarc2 = F_UNDEF;
if  ( ABS(totarc1) > 0.000001 ) relarc1 = segarc1/totarc1;
if  ( ABS(totarc2) > 0.000001 ) relarc2 = segarc2/totarc2;
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur865*patcrea totarc1 %15.12f totarc2 %15.12f\n",
                         totarc1,totarc2 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur865*patcrea segarc1 %15.12f segarc2 %15.12f\n",
                         segarc1,segarc2 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
             "sur865*patcrea relarc1 %15.12f relarc2 %15.12f\n",
                         relarc1,relarc2 );
fflush(dbgfil(SURPAC));
}
if  ( ABS(relarc1-relarc2) > 0.000001 )
{
sprintf(errbuf,"relarc1-relarc2%%sur865*patcrea");
#ifdef LNAME
return(erpush("SU2993",errbuf));
#endif
#ifdef SNAME
erinit();
return(erpush("SU2993",errbuf));
#endif
}

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur865*patcrea  Exit \n");
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** dbounds ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates U patch boundaries for the surface.      */
/* Output is an array of parameter values for the spine.            */

   static short dbounds ( p_cur1, p_seg1, p_cur2, p_seg2 )

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
   DBint   n_all_1;      /* Number of parameter values in all_1()   */
   DBfloat all_1[SPINE]; /* All parameter values for curve 1        */
   DBfloat all_1_2[SPINE];/* Curve 2 parameter values corresp. all_1*/
   DBint   d_c_1[SPINE]; /* Developability analysis code, curve 1   */
   DBint   n_all_2;      /* Number of parameter values in all_2()   */
   DBfloat all_2[SPINE]; /* All parameter values for curve 2        */
   DBfloat all_2_1[SPINE];/* Curve 1 parameter values corresp. all_2*/
   DBint   d_c_2[SPINE]; /* Developability analysis code, curve 2   */

   DBint   i_cur1;       /* Current index for curve 1 arrays        */
   DBint   i_cur2;       /* Current index for curve 2 arrays        */

/*----------------------------------------------------------------- */

   DBint   i_cur1_c;     /* Index for next      curve 1 candidate   */
   DBfloat i_cur1_m;     /* Parameter value     curve 1 candidate   */
   DBfloat i_cur1_s;     /* Parameter value     curve 1 candidate   */
   DBint   i_cur1_d;     /* Developability code curve 1 candidate   */
   DBint   i_cur2_c;     /* Index for next      curve 2 candidate   */
   DBfloat i_cur2_m;     /* Parameter value     curve 2 candidate   */
   DBfloat i_cur2_s;     /* Parameter value     curve 2 candidate   */
   DBint   i_cur2_d;     /* Developability code curve 2 candidate   */

   DBint   n_iter;       /* For programming check                   */


#ifdef  DEBUG
   DBint   i_sol;        /* Loop index solution point               */

   DBint   d_npts;       /* Number of output points                 */
   DBint   d_code[SMAX]; /* Developability analysis code            */
#endif
   short   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*dbounds U patch boundary values for developable surface\n");
}
#endif

#ifdef  DEBUG
   d_npts     =  I_UNDEF;
   d_code[0]  =  I_UNDEF;
   i_cur1     =  I_UNDEF; 
   i_cur2     =  I_UNDEF;
   n_iter     =  I_UNDEF;
#endif


/*!                                                                 */
/* 2. U boundary values from boundary curves                        */
/* =========================================                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Developable ruling parameter values with curve 1 as the master   */
/* curve. Curve segment start/end points and start/end points       */
/* for cones will be calculated.                                    */
/* Call of internal function dcurvei.                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*dbounds Find developable rulings with curve 1 as master\n");
}
#endif

   status = dcurvei ( p_cur1, p_seg1, p_cur2, p_seg2 ,
                    &n_all_1, all_1, all_1_2, d_c_1);
   if ( status < 0 )
     {
     sprintf(errbuf,"dcurvei (1)%%sur865*dbounds");
     return(varkon_erpush("SU2973",errbuf));
     }

/*!                                                                 */
/* Developable ruling parameter values with curve 2 as the master   */
/* curve. Curve segment start/end points and start/end points       */
/* for cones will be calculated.                                    */
/* Call of internal function dcurvei.                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*dbounds Find developable rulings with curve 2 as master\n");
}
#endif

   status = dcurvei ( p_cur2, p_seg2, p_cur1, p_seg1 ,
                    &n_all_2, all_2, all_2_1, d_c_2);
   if ( status < 0 )
     {
     sprintf(errbuf,"dcurvei (2)%%sur865*dbounds");
     return(varkon_erpush("SU2973",errbuf));
     }


#ifdef  DEBUG
   mbsprog ( n_all_1, all_1, all_1_2, d_c_1,
             n_all_2, all_2, all_2_1, d_c_2);
#endif

/*!                                                                 */
/* 3. Order parameter values for the output surface                 */
/* ================================================                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Initialize current index i_cur1 and index i_cur2 to zero         */
/*                                                                 !*/

    i_cur1 = 0;
    i_cur2 = 0;

    n_iter = 0; /* For programming check */

/*!                                                                 */
/* First ruling to spine_u and curv2_u.                             */
/*                                                                 !*/

   n_spine_u    = 0;
   add_rul( 1.0, 1.0 );

/*!                                                                 */
next_rul:; /* Label: Next ruling                                    */
/*                                                                 !*/

    n_iter = n_iter + 1; /* For programming check */
    if ( n_iter > 100 ) /* Tillfalligt !!!!!!!!!!!!!!!!!!!!!!!! */
     {
     sprintf(errbuf,"n_iter > 1000%%sur865*dbounds");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* Get next ruling candidate from curve 1                           */
/*                                                                 !*/

   status = rulcand ( i_cur1, n_all_1, all_1, all_1_2, d_c_1,
             &i_cur1_c,&i_cur1_m,&i_cur1_s,&i_cur1_d );
   if ( status < 0 )
     {
     sprintf(errbuf,"rulcand (1)%%sur865*dbounds");
     return(varkon_erpush("SU2973",errbuf));
     }


/*!                                                                 */
/* Get next ruling candidate from curve 2                           */
/*                                                                 !*/

   status = rulcand ( i_cur2, n_all_2, all_2, all_2_1, d_c_2,
             &i_cur2_c,&i_cur2_m,&i_cur2_s,&i_cur2_d );
   if ( status < 0 )
     {
     sprintf(errbuf,"rulcand (2)%%sur865*dbounds");
     return(varkon_erpush("SU2973",errbuf));
     }

/*!                                                                 */
/* End of loop if there are no additional candidates                */
/*                                                                 !*/

  if ( i_cur1_c < 0 && i_cur2_c < 0 ) goto all_rulings;

/*!                                                                 */
/* Select next developable ruling and update indeces i_cur1, i_cur2 */
/*                                                                 !*/

   status = rulcomp ( p_cur1, p_seg1, p_cur2, p_seg2, 
                          &i_cur1, &i_cur2, 
                          n_all_1, all_1, all_1_2, d_c_1,
                          n_all_2, all_2, all_2_1, d_c_2,
                          i_cur1_c,i_cur1_m,i_cur1_s,i_cur1_d ,
                          i_cur2_c,i_cur2_m,i_cur2_s,i_cur2_d );
   if ( status < 0 )
     {
     sprintf(errbuf,"rulcomp%%sur865*dbounds");
     return(varkon_erpush("SU2973",errbuf));
     }


/*!                                                                 */
/* Continue with next ruling if ....... TODO                        */
/*                                                                 !*/

   if ( i_cur1 >= 0 && i_cur2 >= 0 ) goto next_rul;


all_rulings:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*dbounds All U boundary curve 1 (spine) values for surface :\n");
for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur865*dbounds Ruling %3d spine_u(%2d) %12.8f curv2_u %12.8f\n",
         (int)i_sol,(int)i_sol-1,spine_u[i_sol-1], curv2_u[i_sol-1]);
  }
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/





/*!********* Internal ** function ** dcurvei ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates U patch boundaries with input curve i    */
/* as the master curve. The segment start points and 3*nstart       */
/* points within in each segment. For each point is a developable   */
/* ruling calculated (if possible). Output data are arrays with     */
/* parameter values for the master curve (curve i), corresponding   */
/* parameter values for the slave curve (the other curve), code     */
/* for each output ruling and the number of output rulings.         */

   static short dcurvei ( p_curm, p_segm, p_curs, p_segs ,
                          p_n_all_m, all_m, all_m_s, d_c_m)

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBCurve *p_curm;      /* Master boundary curve             (ptr) */
   DBSeg   *p_segm;      /* Segment data for p_curm           (ptr) */
   DBCurve *p_curs;      /* Slave boundary curve              (ptr) */
   DBSeg   *p_segs;      /* Segment data for p_curs           (ptr) */
   DBint   *p_n_all_m;   /* No. of parameter values in all_m  (ptr) */
   DBfloat  all_m[];     /* All parameter values for master curve   */
   DBfloat  all_m_s[];   /* Corresponding values on the slave curve */
   DBint    d_c_m[];     /* Developability analysis code, curve m   */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   n_all_m;       /* Number of parameter values in all_m()   */
   DBint   n_addpt;       /* Number of added points per segment      */
   DBfloat d_addpt;       /* Delta parameter corresponding to n_addpt*/


/*----------------------------------------------------------------- */
   DBint   i_segmm;      /* Loop index segment in curve m           */
   DBint   i_segms;      /* Loop index segment in curve s           */

   DBint   i_addpt;      /* Loop index segment added pts in curve m */


   DBint   i_sol;        /* Loop index solution point               */

   DBfloat t_inm[2];     /* Parameter curve segment limit p_curm    */
                         /* (0):   Start U    (1): End   U          */
   DBfloat t_ins[2];     /* Parameter curve segment limit p_curs    */
                         /* (0):   Start U    (1): End   U          */
   DBint   d_npts;       /* Number of output points                 */
   EVALC   xyz_a[SMAX];  /* Array with all solutions                */
   DBint   d_code[SMAX]; /* Developability analysis code            */
   short   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*dcurvei U patch boundary values for developable surface\n");
}
#endif

/* Initialization of output variables                              */
   *p_n_all_m =  I_UNDEF;

/* Initialization of internal variables                            */
#ifdef  DEBUG
   t_inm[0]   =  F_UNDEF;
   t_inm[1]   =  F_UNDEF;
   t_ins[0]   =  F_UNDEF;
   t_ins[1]   =  F_UNDEF;
   d_code[0]  =  I_UNDEF;
#endif

/*!                                                                 */
/* Number of points in each segment n_addpt = 3*nstart and          */
/* the corresponding delta parameter value d_addpt.                 */
/*                                                                  */
/*                                                                 !*/

  n_addpt = 3*devdat.nstart;      
  d_addpt = 1.0/n_addpt;

/*!                                                                 */
/* 2. U boundary values from the master boundary curve              */
/* ===================================================              */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Initialize the number of output parameter values to zero (0).    */
/*                                                                 !*/
   n_all_m = 0;

/*!                                                                 */
/* Start loop segments (start and end points) of master curve       */
/*                                                                 !*/
   for ( i_segmm =0; i_segmm <= (*p_curm).ns_cu-1; ++i_segmm )
     {
/*!                                                                 */
/*   Start loop points within segment (n_addpt points)              */
/*                                                                 !*/
     for ( i_addpt =0; i_addpt <=  n_addpt; ++i_addpt )
     {
/*!                                                                 */
/*     Current point within segment (start point for dev. ruling)   */
/*                                                                 !*/
       t_inm[0]   =  (DBfloat)i_segmm + 1.0 + i_addpt*d_addpt;
       t_inm[1]   =  (DBfloat)i_segmm + 1.0 + i_addpt*d_addpt;
/*!                                                                 */
/*     Reduce with 2*comptol for last point in segment              */
/*                                                                 !*/
       if ( i_addpt == n_addpt && i_segmm != (*p_curm).ns_cu-1 )
         {
         t_inm[0]   =  t_inm[0] - 2.0*comptol; 
         t_inm[1]   =  t_inm[1] - 2.0*comptol; 
         }

/*!                                                                 */
/*     Start loop (search) segments in the slave curve              */
/*                                                                 !*/
       for ( i_segms =0; i_segms <  (*p_curs).ns_cu; ++i_segms )
         {
/*!                                                                 */
/*       Current search segment in the slave curve                  */
/*                                                                 !*/
         t_ins[0]   =  (DBfloat)i_segms + 1.0;
         t_ins[1]   =  (DBfloat)i_segms + 2.0 - comptol;

/*!                                                                 */
/*       Find developable ruling(s) = slave crv parameter value(s)  */
/*       Call of varkon_cur_develop (sur905).                       */
/*                                                                 !*/
         status=varkon_cur_develop
         (p_curm,p_segm,t_inm,p_curs,p_segs,t_ins,
                          (IRUNON*)&devdat,&d_npts,xyz_a,d_code);
         if (status<0) 
           {
           sprintf(errbuf,"sur905%%sur865*dcurvei");
           return(varkon_erpush("SU2943",errbuf));
           }
/*!                                                                 */
/*       Master and slave parameter values to output arrays         */
/*       if the point is a start/end curve segment point            */
/*                                                                 !*/
         if  ( d_npts >= 1 && ( 0 == i_addpt || n_addpt == i_addpt) )
           {
           for ( i_sol=1; i_sol<=d_npts;    ++i_sol  )
             {
             all_m[n_all_m]    = t_inm[0];
             all_m_s[n_all_m]  = xyz_a[i_sol-1].t_global;
             d_c_m[n_all_m]    = d_code[i_sol-1];
             n_all_m           =  n_all_m + 1;
             /* Check maximum array size  */
             if  ( n_all_m >= SPINE )
               {
               sprintf(errbuf,"Increase SPINE%%sur865*dcurvei");
               return(varkon_erpush("SU2993",errbuf));
               } /* End program error */
           }     /* End loop i_sol    */
         }       /* End d_npts >= 1   */
         else if  ( 0 == d_npts  && 
                     ( 0 == i_addpt || n_addpt == i_addpt) )
           {
           all_m[n_all_m]    = t_inm[0];
           all_m_s[n_all_m]  = F_UNDEF;
           d_c_m[n_all_m]    = -(i_segms+1);
           n_all_m           =  n_all_m + 1;
           /* Check maximum array size  */
           if  ( n_all_m >= SPINE )
             {
             sprintf(errbuf,"Increase SPINE (2)%%sur865*dcurvei");
             return(varkon_erpush("SU2993",errbuf));
             } /* End program error */
           }    /* End d_npts = 0     */

/*!                                                                 */
/*     End   loop (search) segments in the slave curve              */
/*   End   loop points within segment (n_addpt points)              */
/* End   loop segments (start and end points) of master curve       */
/*                                                                 !*/
       } /* End loop i_segms */
     } /* End loop i_addpt */
   } /* End loop i_segmm */

/*!                                                                 */
/* 3. Exit                                                          */
/* =======                                                          */
/*                                                                  */
/*                                                                 !*/

  *p_n_all_m = n_all_m;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
for ( i_sol=1; i_sol<=n_all_m; ++i_sol  )
  {
  if ( d_c_m[i_sol-1] >= 1 && d_c_m[i_sol-1] <= 3  )
  fprintf(dbgfil(SURPAC),
  "sur865*dcurvei %3d Curve m %f Curve s %f Code %d\n", 
  (int)i_sol,all_m[i_sol-1],all_m_s[i_sol-1], (int)d_c_m[i_sol-1]);
  else if ( 4 == d_c_m[i_sol-1]  ) 
  fprintf(dbgfil(SURPAC),
  "sur865*dcurvei %3d Curve m %f Curve s %f Code %d Singular pt\n", 
  (int)i_sol,all_m[i_sol-1],all_m_s[i_sol-1], (int)d_c_m[i_sol-1]);
  else if ( 0 == d_c_m[i_sol-1]  ) 
  fprintf(dbgfil(SURPAC),
  "sur865*dcurvei %3d Curve m %f Curve s No developable ruling\n", 
     (int)i_sol,all_m[i_sol-1]);
  else if ( d_c_m[i_sol-1] <= -1  &&  d_c_m[i_sol-1] >= -SPINE ) 
  fprintf(dbgfil(SURPAC),
  "sur865*dcurvei %3d Curve m %f No dev. ruling in slave segment %d\n", 
     (int)i_sol,all_m[i_sol-1], (int)-d_c_m[i_sol-1]);
  else 
  fprintf(dbgfil(SURPAC),
  "sur865*dcurvei %3d Curve m %f Code %d Classification failure\n", 
     (int)i_sol,all_m[i_sol-1], (int)d_c_m[i_sol-1]);
  }
}

#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/










/*!********* Internal ** function ** rulcand ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Get the next candidate ruling for the input curve (arrays).      */
/* The input curve is called the master curve since.                */

   static short rulcand ( i_curm, n_all_m, all_m, all_m_s, d_c_m,
                       p_i_curm_c,p_i_curm_m,p_i_curm_s,p_i_curm_d )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   i_curm;       /* Current index for curve m arrays        */
   DBint   n_all_m;      /* Number of parameter values in all_m()   */
   DBfloat all_m[];      /* All parameter values for curve m        */
   DBfloat all_m_s[];    /* Curve 2 parameter values corresp. all_m */
   DBint   d_c_m[];      /* Developability analysis code, curve m   */
   DBint   *p_i_curm_c;  /* Index for output (next) candidate (ptr) */
   DBfloat *p_i_curm_m;  /* Parameter value input curve       (ptr) */
   DBfloat *p_i_curm_s;  /* Parameter value slave curve       (ptr) */
   DBint   *p_i_curm_d;  /* Developability code               (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/

   DBint  i_rul;         /* Loop index for curve arrays             */
/*----------------------------------------------------------------- */

#ifdef  DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcand Get next ruling candidate. Input index i_curm %d\n",
          (int)i_curm);
}
#endif

/*!                                                                 */
/* Initialize output variables to undefined (I_UNDEF and F_UNDEF).  */
/*                                                                 !*/

   *p_i_curm_c    =  I_UNDEF; 
   *p_i_curm_m    =  F_UNDEF;
   *p_i_curm_s    =  F_UNDEF;
   *p_i_curm_d    =  I_UNDEF;

/* Initialize internal variables for Debug On                       */
#ifdef  DEBUG
   i_rul    =  I_UNDEF;
#endif

/* Check input for Debug On                                         */
#ifdef  DEBUG
   if ( n_all_m <= 0 )
     {
     sprintf(errbuf,"n_all_m <= 0%%sur865*rulcand");
     return(varkon_erpush("SU2993",errbuf));
     } /* End program error */
#endif

/*!                                                                 */
/* 2. Get next (unused) developable ruling                          */
/* =======================================                          */
/*                                                                 !*/


/*!                                                                 */
/* Start of loop, where start index is the input i_curm             */
/*                                                                 !*/
   for ( i_rul= i_curm; i_rul< n_all_m; ++i_rul  )
     {
/*!                                                                 */
/*   If current element defines a ruling (code > 0) is data         */
/*   retrieved from arrays                                          */
/*                                                                 !*/
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcand i_rul %d d_c_m[i_rul] %d\n",
          (int)i_rul, (int)d_c_m[i_rul]);
}
#endif
     if  ( d_c_m[i_rul] >  0 && d_c_m[i_rul] != 4 )
       {
       *p_i_curm_c    =  i_rul; 
       *p_i_curm_m    =  all_m[i_rul];
       *p_i_curm_s    =  all_m_s[i_rul];
       *p_i_curm_d    =  d_c_m[i_rul];
       goto  ruling_found;
       }
/*!                                                                 */
/* End   of loop                                                    */
/*                                                                 !*/
     } /* End loop i_rul */

/*!                                                                 */
/* No developable ruling existed. Let index to ruling candidate     */
/* be equal to minus one (-1).                                      */
/*                                                                 !*/

    *p_i_curm_c    = -1;

ruling_found:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && *p_i_curm_c >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcand _curm_c %d _curm_m %f _curm_s %f _curm_d %d\n",
(int)*p_i_curm_c, *p_i_curm_m,  *p_i_curm_s,  (int)*p_i_curm_d );
}
if ( dbglev(SURPAC) == 1 && *p_i_curm_c >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcand Candidate %f to %f \n",
 *p_i_curm_m,  *p_i_curm_s );
}
if ( dbglev(SURPAC) == 1 && *p_i_curm_c < 0 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcand No candidate ruling *p_i_curm_c %d for i_curm %d\n",
      (int)*p_i_curm_c, (int)i_curm);
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/








/*!********* Internal ** function ** rulcomp ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Select next ruling and update indeces                            */

   static short rulcomp ( p_cur1, p_seg1, p_cur2, p_seg2, 
                          p_i_cur1, p_i_cur2, 
                          n_all_1, all_1, all_1_2, d_c_1,
                          n_all_2, all_2, all_2_1, d_c_2,
                          i_cur1_c,i_cur1_m,i_cur1_s,i_cur1_d ,
                          i_cur2_c,i_cur2_m,i_cur2_s,i_cur2_d )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
   DBint   *p_i_cur1;    /* Current index for curve 1 arrays  (ptr) */
   DBint   *p_i_cur2;    /* Current index for curve 2 arrays  (ptr) */
   DBint    n_all_1;     /* Number of parameter values in all_1()   */
   DBfloat  all_1[];     /* All parameter values for curve 1        */
   DBfloat  all_1_2[];   /* Curve 2 parameter values corresp. all_1 */
   DBint    d_c_1[];     /* Developability analysis code, curve 2   */
   DBint    n_all_2;     /* Number of parameter values in all_2()   */
   DBfloat  all_2[];     /* All parameter values for curve 2        */
   DBfloat  all_2_1[];   /* Curve 2 parameter values corresp. all_2 */
   DBint    d_c_2[];     /* Developability analysis code, curve 2   */
   DBint    i_cur1_c;    /* Index for next candidate curve 1        */
   DBfloat  i_cur1_m;    /* Parameter value input curve 1           */
   DBfloat  i_cur1_s;    /* Parameter value slave curve             */
   DBint    i_cur1_d;    /* Developability code curve 1             */
   DBint    i_cur2_c;    /* Index for next candidate curve 2        */
   DBfloat  i_cur2_m;    /* Parameter value input curve 2           */
   DBfloat  i_cur2_s;    /* Parameter value slave curve             */
   DBint    i_cur2_d;    /* Developability code curve 2             */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/

   EVALC    xyz_cur1_m;  /* Point corresponding to i_cur1_m         */
   EVALC    xyz_cur1_s;  /* Point corresponding to i_cur1_s         */
   EVALC    xyz_cur2_m;  /* Point corresponding to i_cur2_m         */
   EVALC    xyz_cur2_s;  /* Point corresponding to i_cur2_s         */
   DBfloat  dist_cur1;   /* Distance master-slave point curve 1     */
   DBfloat  dist_cur2;   /* Distance master-slave point curve 2     */
   DBint    i_cur1_in;   /* Current index for curve 1 arrays, in    */
   DBint    i_cur2_in;   /* Current index for curve 2 arrays, in    */
   DBint    i_cur1_out;  /* Current index for curve 1 arrays, out   */
   DBint    i_cur2_out;  /* Current index for curve 2 arrays, out   */
/*----------------------------------------------------------------- */

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
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Select ruling. Candidates i_cur1_c %d i_cur2_c %d\n",
          (int)i_cur1_c, (int)i_cur2_c );
}
#endif

/*!                                                                 */
/* Initialize output variables to undefined (I_UNDEF and F_UNDEF).  */
/*                                                                 !*/

/* Initialize internal variables for Debug On                       */
#ifdef  DEBUG
   dist_cur1    = F_UNDEF;
   dist_cur2    = F_UNDEF;
   i_cur1_in    = I_UNDEF;
   i_cur2_in    = I_UNDEF;
   i_cur1_out   = I_UNDEF;
   i_cur2_out   = I_UNDEF;
#endif

/* Check input for Debug On                                         */
#ifdef  DEBUG
   if ( i_cur1_c == I_UNDEF || i_cur2_c == I_UNDEF  )
     {
     sprintf(errbuf,"Undefined candidates %%sur865*rulcomp");
     return(varkon_erpush("SU2993",errbuf));
     } /* End program error */
#endif

/*!                                                                 */
/* 2. Calculate R*3 points and distances                            */
/* =====================================                            */
/*                                                                 !*/

  if ( i_cur1_c < 0 || i_cur2_c < 0 )
    {
    dist_cur1 = 12345.6789;
    dist_cur2 = 98765.4321;
    goto only_one_rul;
    }

/*!                                                                 */
/* Calculate curve 1 and curve 2 (totally 4) points                 */
/* (Curve references in names are confusing TODO)                   */
/* Calls of GE109.                                                  */
/*                                                                 !*/

   xyz_cur1_m.evltyp   = EVC_DR;
   xyz_cur1_m.t_global = i_cur1_m;

   status=GE109
   ((DBAny *)p_cur1 ,p_seg1, &xyz_cur1_m);
   if (status<0) 
        {
        sprintf(errbuf,"GE109 (1)%%sur865*rulcomp");
        return(varkon_erpush("SU2943",errbuf));
        }
   xyz_cur1_s.evltyp   = EVC_DR;
   xyz_cur1_s.t_global = i_cur1_s;

   status=GE109
   ((DBAny *)p_cur2 ,p_seg2, &xyz_cur1_s); /* Note curve 2 */
   if (status<0) 
        {
        sprintf(errbuf,"GE109 (2)%%sur865*rulcomp");
        return(varkon_erpush("SU2943",errbuf));
        }

   xyz_cur2_m.evltyp   = EVC_DR;
   xyz_cur2_m.t_global = i_cur2_m;

   status=GE109
   ((DBAny *)p_cur2 ,p_seg2, &xyz_cur2_m);
   if (status<0) 
        {
        sprintf(errbuf,"GE109 (3)%%sur865*rulcomp");
        return(varkon_erpush("SU2943",errbuf));
        }
   xyz_cur2_s.evltyp   = EVC_DR;
   xyz_cur2_s.t_global = i_cur2_s;

   status=GE109
   ((DBAny *)p_cur1 ,p_seg1, &xyz_cur2_s); /* Note curve 1 */
   if (status<0) 
        {
        sprintf(errbuf,"GE109 (4)%%sur865*rulcomp");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Calculate distances                                              */
/*                                                                 !*/

   dist_cur1 = SQRT(
   (xyz_cur1_m.r.x_gm-xyz_cur2_s.r.x_gm)*
   (xyz_cur1_m.r.x_gm-xyz_cur2_s.r.x_gm)+ 
   (xyz_cur1_m.r.y_gm-xyz_cur2_s.r.y_gm)*
   (xyz_cur1_m.r.y_gm-xyz_cur2_s.r.y_gm)+ 
   (xyz_cur1_m.r.z_gm-xyz_cur2_s.r.z_gm)*
   (xyz_cur1_m.r.z_gm-xyz_cur2_s.r.z_gm));

   dist_cur2 = SQRT(
   (xyz_cur2_m.r.x_gm-xyz_cur1_s.r.x_gm)*
   (xyz_cur2_m.r.x_gm-xyz_cur1_s.r.x_gm)+ 
   (xyz_cur2_m.r.y_gm-xyz_cur1_s.r.y_gm)*
   (xyz_cur2_m.r.y_gm-xyz_cur1_s.r.y_gm)+ 
   (xyz_cur2_m.r.z_gm-xyz_cur1_s.r.z_gm)*
   (xyz_cur2_m.r.z_gm-xyz_cur1_s.r.z_gm));

only_one_rul:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp dist_cur1 %f dist_cur2 %f\n",
           dist_cur1, dist_cur2 );
}
#endif


/*!                                                                 */
/* 3. Select ruling and update indeces                              */
/* ===================================                              */
/*                                                                 !*/

                         /* Developability analysis code: d_code    */
                         /* Eq.  1: Developable in both directions  */
                         /* Eq.  2: Developable in +U, cone in -U   */
                         /* Eq.  3: Developable in -U, cone in +U   */
                         /* Eq.  4: Developable but cones in +/-U   */
                         /* Eq.  0: No developable ruling           */
                         /* Eq. -1: Failure to classify developable */
/*!                                                                 */
/* Input indeces to local variables                                 */
/*                                                                 !*/

   i_cur1_in  =  *p_i_cur1;
   i_cur2_in  =  *p_i_cur2;

/*!                                                                 */
/* Case 1: Master and slave parameter values (points) are equal     */
/*                                                                 !*/

   if       ( dist_cur1 < idpoint && dist_cur2 < idpoint )
     {
/*   Ruling to spine_u and curv2_u.                                 */
     status = add_rul(i_cur1_m, i_cur1_s);
     if ( status < 0 )
       {
       sprintf(errbuf,"add_rul Case 1%%sur865*rulcomp");
       return(varkon_erpush("SU2973",errbuf));
       }

     i_cur1_out = i_cur1_c + 1;
     i_cur2_out = i_cur2_c + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 1: dist_cur1 %f = dist_cur2 %f\n",
           dist_cur1, dist_cur2 );
}
#endif
     }


/*!                                                                 */
/* Case 4: Curve 1 master point is the next ruling                  */
/*         No ruling from curve 2                                   */
/*                                                                 !*/
   else if ( i_cur1_c >= 0  && i_cur2_c <  0  )
     {
/*   Ruling to spine_u and curv2_u.                                 */
     status = add_rul(i_cur1_m, i_cur1_s);
     if ( status < 0 )
       {
       sprintf(errbuf,"add_rul Case 4%%sur865*rulcomp");
       return(varkon_erpush("SU2973",errbuf));
       }
     i_cur1_out = i_cur1_c + 1;
     status =   rulupda( i_cur2_in, n_all_2, all_2, all_2_1, d_c_2,
                          i_cur1_s, &i_cur2_out );

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 4: Curve 1 master pt. No curve 2 ruling\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 4: Output ruling is %f to %f\n",
            i_cur1_m, i_cur1_s);
}
#endif
     }

/*!                                                                 */
/* Case 5: Curve 2 master point is the next ruling                  */
/*         No ruling from curve 1                                   */
/*                                                                 !*/
   else if ( i_cur1_c <  0  && i_cur2_c >= 0  )
     {
/*   Ruling to spine_u and curv2_u.                                 */
     status = add_rul(i_cur2_s, i_cur2_m);
     if ( status < 0 )
       {
       sprintf(errbuf,"add_rul Case 5%%sur865*rulcomp");
       return(varkon_erpush("SU2973",errbuf));
       }
     status =   rulupda( i_cur1_in, n_all_1, all_1, all_1_2, d_c_1,
                          i_cur2_s, &i_cur1_out );
     i_cur2_out = i_cur2_c + 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 5: Curve 2 master pt. No curve 1 ruling\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 5: Output ruling is %f to %f\n",
            i_cur2_s, i_cur2_m);
}
#endif
     }


/*!                                                                 */
/* Case 2: Curve 1 master point is the next ruling                  */
/*                                                                 !*/
   else if ( i_cur1_m < i_cur2_s && i_cur1_s < i_cur2_m )
     {
/*   Ruling to spine_u and curv2_u.                                 */
     status = add_rul(i_cur1_m, i_cur1_s);
     if ( status < 0 )
       {
       sprintf(errbuf,"add_rul Case 2%%sur865*rulcomp");
       return(varkon_erpush("SU2973",errbuf));
       }
     i_cur1_out = i_cur1_c + 1;
     status =   rulupda( i_cur2_in, n_all_2, all_2, all_2_1, d_c_2,
                          i_cur1_s, &i_cur2_out );

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 2: Curve 1 master point is the next ruling\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 2: Output ruling is %f to %f\n",
            i_cur1_m, i_cur1_s);
}
#endif
     }

/*!                                                                 */
/* Case 3: Curve 2 master point is the next ruling                  */
/*                                                                 !*/
   else if ( i_cur2_m < i_cur1_s && i_cur2_s < i_cur1_m )
     {
/*   Ruling to spine_u and curv2_u.                                 */
     status = add_rul(i_cur2_s, i_cur2_m);
     if ( status < 0 )
       {
       sprintf(errbuf,"add_rul Case 3%%sur865*rulcomp");
       return(varkon_erpush("SU2973",errbuf));
       }
     status =   rulupda( i_cur1_in, n_all_1, all_1, all_1_2, d_c_1,
                          i_cur2_s, &i_cur1_out );
     i_cur2_out = i_cur2_c + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 3: Curve 2 master point is the next ruling\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case 3: Output ruling is %f to %f\n",
            i_cur2_s, i_cur2_m);
}
#endif
     }

  else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Case Undefined: Programming error\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp i_cur1_d %d i_cur2_d %d\n", 
       (int)i_cur1_d, (int)i_cur2_d   );
}
#endif
     }

/*!                                                                 */
/* Output indeces                                                   */
/*                                                                 !*/

   if (  i_cur1_out >= 0 && i_cur2_out >= 0 )
     {
     *p_i_cur1  =  i_cur1_out;
     *p_i_cur2  =  i_cur2_out;
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulcomp Output *p_i_cur1 %d *p_i_cur2 %d\n",
         (int)*p_i_cur1  , (int)*p_i_cur2 );
}
#endif



    return(SUCCED);

} /* End of function                                                */
/********************************************************************/




/*!********* Internal ** function ** rulupda ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Update current index for a curve                                 */
/* The input curve is called the master curve.                      */

   static short rulupda ( i_curm_in, n_all_m, all_m, all_m_s, d_c_m,
                          i_curm_m, p_i_curm_out )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   i_curm_in;    /* Current index for curve m arrays        */
   DBint   n_all_m;      /* Number of parameter values in all_m()   */
   DBfloat all_m[];      /* All parameter values for curve m        */
   DBfloat all_m_s[];    /* Curve 2 parameter values corresp. all_m */
   DBint   d_c_m[];      /* Developability analysis code, curve m   */
   DBfloat i_curm_m;     /* Parameter defining the output index     */
   DBint  *p_i_curm_out; /* Output (updated) index            (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/

   DBint  i_rul;         /* Loop index for curve arrays             */
/*----------------------------------------------------------------- */

#ifdef  DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif
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
"sur865*rulupda Update index. Input index is i_curm_in %d\n",
          (int)i_curm_in);
}
#endif

/*!                                                                 */
/* Initialize output variable to undefined (I_UNDEF)                */
/*                                                                 !*/

   *p_i_curm_out  =  I_UNDEF; 

/* Initialize internal variables for Debug On                       */
#ifdef  DEBUG
   i_rul    =  I_UNDEF;
#endif

/* Check input for Debug On                                         */
#ifdef  DEBUG
   if ( n_all_m <= 0 )
     {
     sprintf(errbuf,"n_all_m <= 0%%sur865*rulupda");
     return(varkon_erpush("SU2993",errbuf));
     } /* End program error */
#endif

/*!                                                                 */
/* 2. Determine the output index                                    */
/* =============================                                    */
/*                                                                 !*/


/*!                                                                 */
/* Start of loop, where start index is the input i_curm_in          */
/*                                                                 !*/
   for ( i_rul= i_curm_in; i_rul< n_all_m; ++i_rul  )
     {
/*!                                                                 */
/*   If the parameter value for the current point (ruling) exceeds  */
/*   the input parameter value ...........                          */
/*                                                                 !*/
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulupda i_rul %d all_m[i_rul] %f Input i_curm_m %f\n",
          (int)i_rul, all_m[i_rul], i_curm_m);
}
#endif
     if  ( all_m[i_rul] >= i_curm_m  )
       {
       *p_i_curm_out  =  i_rul; 
       goto  ruling_found;
       }
/*!                                                                 */
/* End   of loop                                                    */
/*                                                                 !*/
     } /* End loop i_rul */

/*!                                                                 */
/* No developable ruling existed. Let index to the output ruling    */
/* be equal to minus one (-1).                                      */
/*                                                                 !*/

    *p_i_curm_out  = -1;

ruling_found:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && *p_i_curm_out >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulupda Output *p_i_curm_out %d for i_curm_m %f \n",
(int)*p_i_curm_out, i_curm_m );
}
if ( dbglev(SURPAC) == 1 && *p_i_curm_out < 0 )
{
fprintf(dbgfil(SURPAC),
"sur865*rulupda No output ruling *p_i_curm_out %d for i_curm_in %d\n",
      (int)*p_i_curm_out, (int)i_curm_in);
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/






#ifdef DEBUG







/*!********* Internal ** function ** mbsprog ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create MBS program for Debug On                                  */

   static short mbsprog ( n_all_1, all_1, all_1_2, d_c_1,
                          n_all_2, all_2, all_2_1, d_c_2)

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    n_all_1;     /* Number of parameter values in all_1()   */
   DBfloat  all_1[];     /* All parameter values for curve 1        */
   DBfloat  all_1_2[];   /* Curve 2 parameter values corresp. all_1 */
   DBint    d_c_1[];     /* Developability analysis code, curve 2   */
   DBint    n_all_2;     /* Number of parameter values in all_2()   */
   DBfloat  all_2[];     /* All parameter values for curve 2        */
   DBfloat  all_2_1[];   /* Curve 2 parameter values corresp. all_2 */
   DBint    d_c_2[];     /* Developability analysis code, curve 2   */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/

   DBint  i_sol;         /* Loop index solution                     */
   char   lin_id[80];    /* String for line identity                */
   char   txt_id[80];    /* String for text identity                */
   char   code_s[80];    /* String for code                         */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"!sur865*mbsprog Create MBS program \n");
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"GLOBAL GEOMETRY MODULE sur865(); \n");
fprintf(dbgfil(SURPAC),
"BEGINMODULE\n");
}

if ( dbglev(SURPAC) == 1 )
{
for ( i_sol=1; i_sol<=n_all_1; ++i_sol  )
  {

   if      ( i_sol <=  9 ) sprintf(lin_id,"#%1d",(int)i_sol);
   else if ( i_sol <= 99 ) sprintf(lin_id,"#%2d",(int)i_sol);
   else                    sprintf(lin_id,"#%3d",(int)i_sol);

  sprintf(txt_id,"#%3d",(int)i_sol+100);
  sprintf(code_s,"Q%1dQ",(int)d_c_1[i_sol-1]);

  if ( d_c_1[i_sol-1] >= 1 && d_c_1[i_sol-1] <= 3  )
  {
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d for Line %s\n", (int)d_c_1[i_sol-1], lin_id);
  fprintf(dbgfil(SURPAC),
  "LIN_FREE(%s, VEC(%f,0.0,0.0),VEC(%f,1.0,0):PEN=2);\n", 
  lin_id, all_1[i_sol-1],all_1_2[i_sol-1]);
  fprintf(dbgfil(SURPAC),
  "TEXT(%s,ON(%s,0.5),0.0,%s:PEN=2,BLANK=0,TSIZE=0.1);\n",
                 txt_id,lin_id,code_s);
  }
  else if ( 4 == d_c_1[i_sol-1] ) 
  {
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d for Line %s (Singular point)\n", 
          (int)d_c_1[i_sol-1], lin_id);
  fprintf(dbgfil(SURPAC),
  "LIN_FREE(%s, VEC(%f,0.0,0.0),VEC(%f,1.0,0):PEN=1,BLANK=1);\n", 
  lin_id, all_1[i_sol-1],all_1_2[i_sol-1]);
  fprintf(dbgfil(SURPAC),
  "TEXT(%s,ON(%s,0.5),0.0,%s:PEN=1,BLANK=1);\n",txt_id,lin_id,code_s);
  }
  else if ( 0 == d_c_1[i_sol-1] ) 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d No developable ruling\n", (int)d_c_1[i_sol-1]);
  else if ( d_c_1[i_sol-1] <= -1  &&  d_c_1[i_sol-1] >= -SPINE ) 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d No developable ruling in slave segment\n", 
        (int)d_c_1[i_sol-1]);
  else 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d Classification failure\n", (int)d_c_1[i_sol-1]);
  }
}


if ( dbglev(SURPAC) == 1 )
{
for ( i_sol=1; i_sol<=n_all_2; ++i_sol  )
  {

   if      ( i_sol+n_all_1 <=  9 ) sprintf(lin_id,"#%1d",
                                (int)(i_sol+n_all_1));
   else if ( i_sol+n_all_1 <= 99 ) sprintf(lin_id,"#%2d",
                                (int)(i_sol+n_all_1));
   else                            sprintf(lin_id,"#%3d",
                                (int)(i_sol+n_all_1));

  sprintf(txt_id,"#%3d",(int)(i_sol+n_all_1)+100);
  sprintf(code_s,"Q%1dQ",(int)d_c_2[i_sol-1]);

  if ( d_c_2[i_sol-1] >= 1 && d_c_2[i_sol-1] <= 3  )
  {
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d for Line %s\n", (int)d_c_2[i_sol-1], lin_id);
  fprintf(dbgfil(SURPAC),
  "LIN_FREE(%s, VEC(%f, 1.0,0.0),VEC(%f, 0.0,0):PEN=4);\n", 
  lin_id, all_2[i_sol-1],all_2_1[i_sol-1]);
  fprintf(dbgfil(SURPAC),
  "TEXT(%s,ON(%s,0.5),0.0,%s:PEN=4,BLANK=0);\n",txt_id,lin_id,code_s);
  }
  else if ( 4 == d_c_2[i_sol-1] ) 
  {
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d for Line %s (Singular point)\n", 
          (int)d_c_2[i_sol-1], lin_id);
  fprintf(dbgfil(SURPAC),
  "LIN_FREE(%s, VEC(%f,1.0,0.0),VEC(%f, 0.0,0):PEN=3,BLANK=1);\n", 
  lin_id, all_2[i_sol-1],all_2_1[i_sol-1]);
  fprintf(dbgfil(SURPAC),
  "TEXT(%s,ON(%s,0.5),0.0,%s:PEN=3,BLANK=1,TSIZE=0.1);\n",
                 txt_id,lin_id,code_s);
  }
  else if ( 0 == d_c_2[i_sol-1] ) 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d No developable ruling\n", (int)d_c_2[i_sol-1]);
  else if ( d_c_2[i_sol-1] <= -1  &&  d_c_2[i_sol-1] >= -SPINE ) 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d No developable ruling in slave segment\n", 
        (int)d_c_2[i_sol-1]);
  else 
  fprintf(dbgfil(SURPAC),
  "! Ruling code %2d Classification failure\n", (int)d_c_2[i_sol-1]);
  }
}


if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"ENDMODULE\n");
}


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif  /* End DEBUG */






/*!********* Internal ** function ** add_rul ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Add ruling                                                       */

   static short add_rul ( curve_1_p, curve_2_p )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  curve_1_p;   /* Curve 1 parameter value                 */
   DBfloat  curve_2_p;   /* Curve 2 parameter value                 */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/

/*----------------------------------------------------------------- */

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
"sur865*add_rul n_spine_u %d\n",
          (int)n_spine_u);
}
#endif


  if ( n_spine_u >= SPINE )
     {
     sprintf(errbuf,"n_spine_u>=SPINE%%sur865*add_rul");
     return(varkon_erpush("SU2993",errbuf));
     }

     spine_u[n_spine_u]   = curve_1_p;
     curv2_u[n_spine_u]   = curve_2_p;
     n_spine_u            = n_spine_u + 1;



    return(SUCCED);

} /* End of function                                                */
/********************************************************************/




