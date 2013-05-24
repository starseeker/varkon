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
/*  Function: varkon_sur_sweep                     File: sur868.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a swept surface (SUR_SWEEP)                              */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-05-02   Originally written                                 */
/*  1997-05-11   Reverse direction                                  */
/*  1997-05-21   Bug: Reverse direction -1                          */
/*  1997-12-19   Center of gravity sur680                           */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_sweep      Create swept surface SUR_SWEEP   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_ini_gmpatl       * Initialize GMPATL                      */
/* GE817                   * Trim a curve                           */
/* varkon_cur_averplan     * Average plane for a curve              */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to stack                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short fixdir();        /* Fixed direction for the sweeping   */
static short curmod();        /* Reverse curve directions           */
static short suralloc();      /* Allocate memory for surface        */
static short surcrea ();      /* Create the output surface          */
static short patcrea ();      /* Create one patch in surface        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */

static DBVector yaxis;       /* Defines Y axis for sweeping system  */
static DBfloat  p_dir_l;     /* Length of input fixed vector p_dir  */
static DBCurve trispine;     /* Reversed spine curve                */
static DBCurve tricur;       /* Reversed section curve              */
static DBSeg *p_trispineseg; /* Segment data trimmed spine    (ptr) */
static DBSeg *p_triseg;      /* Segment data trimmed section  (ptr) */
static DBCurve *p_spine_mod; /* Boundary curve 1  modified    (ptr) */
static DBSeg *p_spineseg_mod;/* Segment data for p_spine modif(ptr) */
static DBCurve *p_cur_mod;   /* Section curve modified        (ptr) */
static DBSeg *p_seg_mod;     /* Segment data for p_cur modif  (ptr) */
static DBint  surface_type;  /* Type of output surface              */
                             /* Eq. 3: LFT_SUR                      */
static DBint  n_ulines;      /* Number of U lines in swept surface  */
static DBint  n_vlines;      /* Number of V lines in swept surface  */
static DBPatch *p_frst;      /* Pointer to the first patch          */
static DBfloat  idpoint;     /* Identical point criterion           */
static DBfloat  idangle;     /* Identical angle criterion           */
static DBfloat  comptol;     /* Computer tolerance                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_sweep           */
/* SU2973 = Internal function () failed in varkon_sur_sweep         */
/* SU2993 = Severe program error (  ) in varkon_sur_sweep           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_sweep (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_spine,     /* Spine curve                       (ptr) */
   DBSeg   *p_spineseg,  /* Segment data for p_spine          (ptr) */
   DBCurve *p_cur,       /* Section curve                     (ptr) */
   DBSeg   *p_seg,       /* Segment data for p_cur            (ptr) */
   DBVector *p_dir,      /* Input fixed vector direction      (ptr) */
   DBint    s_case,      /* Sweep surface case   Not yet used ....  */
   DBint    reverse,     /* Reverse case                            */
                         /* Eq.  1: No reverse of input   curves    */
                         /* Eq. -1: Reverse    of spine   curve     */
                         /*         No reverse of section curve     */
                         /* Eq.  2: No reverse of spine   curve     */
                         /*         Reverse    of section curve     */
                         /* Eq.  3: Reverse    of spine   curve     */
                         /*         Reverse    of section curve     */
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
  short  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

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
"sur868 Enter***varkon_sur_sweep er reverse %d\n", (int)reverse );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868 p_spine %d p_spineseg %d\n", (int)p_spine,  (int)p_spineseg );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868 p_cur %d p_seg %d\n", (int)p_cur,  (int)p_seg );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial( p_surout );
    if (status<0) 
      {
#ifdef  DEBUG
      sprintf(errbuf,"initial%%sur868");
      varkon_erpush("SU2973",errbuf);
#endif
      return(status);
      }

/*!                                                                 */
/* 2. Determine the fixed direction (yaxis) for the sweeping        */
/*                                                                 !*/

    status= fixdir ( p_spine, p_spineseg, p_dir );
    if (status<0) 
      {
#ifdef DEBUG
      sprintf(errbuf,"fixdir%%sur868");
      varkon_erpush("SU2973",errbuf);
#endif
      return(status);
      }


/*!                                                                 */
/* 3. Modify curve directions and yaxis                             */
/*                                                                 !*/

    status= curmod ( p_spine, p_spineseg, p_cur, p_seg, reverse );
    if (status<0) 
      {
      sprintf(errbuf,"curmod%%sur868");
      varkon_erpush("SU2973",errbuf);
      goto err2;
      }

/*!                                                                 */
/* 4. Allocate memory for the output surface                        */
/*                                                                 !*/

    status= suralloc ( p_surout, pp_patout );
    if (status<0) 
      {
      sprintf(errbuf,"suralloc%%sur868");
      return(varkon_erpush("SU2973",errbuf));
      }


/*!                                                                 */
/* 5. Create the swept, lofted surface                              */
/*                                                                 !*/


    status= surcrea  ( );
    if (status<0) 
      {
      sprintf(errbuf,"surcrea%%sur868");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 6. Calculate bounding boxes and cones                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%sur868");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */

/*!                                                                 */
/* 7. Deallocation of memory for reversed curves                    */
/*                                                                 !*/

err2:;

    if ( p_trispineseg != NULL ) DBfree_segments(p_trispineseg);
    if ( p_triseg      != NULL ) DBfree_segments(p_triseg);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868 Allocated memory for patches *pp_patout %d\n",(int)*pp_patout);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && p_surout->typ_su == LFT_SUR )
{
fprintf(dbgfil(SURPAC),
"sur868 Exit Number of patches in U %d in V %d in the LFT_SUR surface\n", 
          p_surout->nu_su ,  p_surout->nv_su  );
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

   static short initial( p_surout )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_surout;    /* Output surface                    (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
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
fprintf(dbgfil(SURPAC), "sur868*initial*Enter \n");
}
#endif

 p_trispineseg  = NULL; 
 p_triseg       = NULL; 
 p_spine_mod    = NULL;  
 p_spineseg_mod = NULL; 
 p_cur_mod      = NULL;  
 p_seg_mod      = NULL; 

 yaxis.x_gm     = F_UNDEF;
 yaxis.y_gm     = F_UNDEF;
 yaxis.z_gm     = F_UNDEF;

 p_dir_l        = F_UNDEF;  

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();

 surface_type     = I_UNDEF; 
 n_ulines         = I_UNDEF;
 n_vlines         = I_UNDEF;
 p_frst           = NULL;  

 p_surout->typ_su = I_UNDEF;
 p_surout->nu_su  = I_UNDEF; 
 p_surout->nv_su  = I_UNDEF;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*initial idpoint %10.6f idangle %6.4f comptol %12.10f\n", 
idpoint, idangle, comptol  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** fixdir  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function determines the fixed direction vector for           */
/* the sweeping of the section curve. The vector will define        */
/* the yaxis in a local system, where the X axis is the tangent     */
/* of the spine.                                                    */

   static short fixdir ( p_spine, p_spineseg, p_dir )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_spine;     /* Spine curve                       (ptr) */
   DBSeg   *p_spineseg;  /* Segment data for p_spine          (ptr) */
   DBVector *p_dir;      /* Input fixed vector direction      (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  zaxis_l;      /* Length of curve plane Z axis            */
  DBint    n_add;        /* Number of points per curve segment      */
  DBfloat  pcrit;        /* Position criterion < 0 ==> system value */
  DBfloat  dcrit;        /* Derivat. criterion < 0 ==> system value */
  DBVector c_o_g;        /* Center of gravity                       */
  DBfloat  maxdev;       /* Maximum deviation curve/plane           */
  DBfloat  a_plane[4];   /* Average plane                           */

/*----------------------------------------------------------------- */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*fixdir*Enter *p_dir %f %f %f \n", 
            p_dir->x_gm, p_dir->y_gm, p_dir->z_gm );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initializations                                                  */
/*                                                                 !*/

    p_dir_l = F_UNDEF;
    zaxis_l = F_UNDEF;

    n_add      = 5; 
    pcrit      = F_UNDEF;    
    dcrit      = F_UNDEF;   
    maxdev     = F_UNDEF; 
    c_o_g.x_gm = F_UNDEF;
    c_o_g.y_gm = F_UNDEF;
    c_o_g.z_gm = F_UNDEF;
    a_plane[0] = F_UNDEF;  
    a_plane[1] = F_UNDEF;  
    a_plane[2] = F_UNDEF;  
    a_plane[3] = F_UNDEF;  

/*!                                                                 */
/* Calculate the length of the input direction vector *p_dir        */
/*                                                                 !*/

    p_dir_l = SQRT(
    p_dir->x_gm*p_dir->x_gm + 
    p_dir->y_gm*p_dir->y_gm + 
    p_dir->z_gm*p_dir->z_gm );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*fixdir* Length of *p_dir %f \n", p_dir_l );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Let yaxis= p_dir if p_dir is defined (not a zero vector)         */
/*                                                                 !*/

    if ( p_dir_l > idpoint )
      {
      yaxis.x_gm = p_dir->x_gm/p_dir_l;
      yaxis.y_gm = p_dir->y_gm/p_dir_l;
      yaxis.z_gm = p_dir->z_gm/p_dir_l;
      goto dirdef;
      }

/*!                                                                 */
/* Retrieve curve plane                                             */
/*                                                                 !*/

  if  ( p_spine->plank_cu == FALSE )
    {
    status = varkon_cur_averplan
    (p_spine,p_spineseg,pcrit,dcrit, n_add,&c_o_g,&maxdev,a_plane );
    if ( status < 0 )
      {
      varkon_erinit();
      sprintf(errbuf,"(sur868)%%");
      return(varkon_erpush("SU8303",errbuf));
      }
    yaxis.x_gm =  a_plane[0];
    yaxis.y_gm =  a_plane[1];
    yaxis.z_gm =  a_plane[2];
    goto  dirclc;
    }


/*!                                                                 */
/* Retrieve curve plane                                             */
/*                                                                 !*/

  yaxis.x_gm =  p_spine->csy_cu.g31;
  yaxis.y_gm =  p_spine->csy_cu.g32;
  yaxis.z_gm =  p_spine->csy_cu.g33;

  zaxis_l = SQRT(
    yaxis.x_gm*yaxis.x_gm + 
    yaxis.y_gm*yaxis.y_gm + 
    yaxis.z_gm*yaxis.z_gm );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*fixdir* Length of zaxis %f for curve plane \n", zaxis_l );
}
#endif


  if ( ABS(zaxis_l-1.0) > idpoint )
    {
    sprintf(errbuf,"zaxis not 1%%sur868*fixdir");
    return(varkon_erpush("SU2993",errbuf));
    }



dirclc:; /* Label: Direction vector is calculated                   */
dirdef:; /* Label: Direction vector is defined                      */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*fixdir*Exit yaxis %f %f %f \n", 
            yaxis.x_gm, yaxis.y_gm, yaxis.z_gm );
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




/*!********* Internal ** function ** curmod  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function modifies curves and yaxis with respect to reverse   */

   static short curmod ( p_spine, p_spineseg, p_cur, p_seg, reverse  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_spine;     /* Spine curve                       (ptr) */
   DBSeg   *p_spineseg;  /* Segment data for p_spine          (ptr) */
   DBCurve *p_cur;       /* Section curve                     (ptr) */
   DBSeg   *p_seg;       /* Segment data for p_cur            (ptr) */
   DBint    reverse;     /* Reverse case                            */
                         /* Eq.  1: No reverse of input   curves    */
                         /* Eq. -1: Reverse    of spine   curve     */
                         /*         No reverse of section curve     */
                         /* Eq.  2: No reverse of spine   curve     */
                         /*         Reverse    of section curve     */
                         /* Eq.  3: Reverse    of spine   curve     */
                         /*         Reverse    of section curve     */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  uglobs1;      /* Global u start value, curve 1           */
  DBfloat  uglobe1;      /* Global u end   value, curve 1           */
  DBfloat  uglobs2;      /* Global u start value, curve 2           */
  DBfloat  uglobe2;      /* Global u end   value, curve 2           */
/*----------------------------------------------------------------- */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*curmod*Enter \n" );
fflush(dbgfil(SURPAC));
}
#endif

/* Create recersed curves for all cases except reverse= 1          */
/* (actually not necessary .. )                                    */
  if (  reverse != 1 )
    {
    p_trispineseg = DBcreate_segments(p_spine->ns_cu);
    uglobs1       = (DBfloat)p_spine->ns_cu+1.0;
    uglobe1       =          1.0;
    status=GE817
      ((DBAny*)p_spine, p_spineseg,&trispine,p_trispineseg,
          uglobs1,uglobe1);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur868*curmod GE817 failed uglobs1 %f uglobe1 %f \n",
                         uglobs1,uglobe1);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(erpush("SU2943",errbuf));
        }
    p_triseg  = DBcreate_segments(p_cur->ns_cu);
    uglobs2   = (DBfloat)p_cur->ns_cu+1.0;
    uglobe2   =          1.0;
   status=GE817((DBAny*)p_cur, p_seg,&tricur,p_triseg,
          uglobs2,uglobe2);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur868*curmod GE817 failed uglobs2 %f uglobe2 %f \n",
                         uglobs2,uglobe2);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(varkon_erpush("SU2943",errbuf));
        }
    }


  if      (  reverse == 1 )
    {
    p_spine_mod    = p_spine;  
    p_spineseg_mod = p_spineseg; 
    p_cur_mod      = p_cur;  
    p_seg_mod      = p_seg; 
    }
  else if (  reverse == 2 )
    {
    p_spine_mod    = p_spine;  
    p_spineseg_mod = p_spineseg; 
    p_cur_mod      = &tricur;  
    p_seg_mod      = p_triseg; 
    }
  else if (  reverse == 3 )
    {
    p_spine_mod    = &trispine;  
    p_spineseg_mod = p_trispineseg; 
    p_cur_mod      = p_cur;  
    p_seg_mod      = p_seg; 
    if ( p_dir_l < idpoint )
      {
      yaxis.x_gm = - yaxis.x_gm;
      yaxis.y_gm = - yaxis.y_gm;
      yaxis.z_gm = - yaxis.z_gm;
      }
    }
  else if (  reverse == -1 )
    {
    p_spine_mod    = &trispine;  
    p_spineseg_mod = p_trispineseg; 
    p_cur_mod      = &tricur;  
    p_seg_mod      = p_triseg; 
    if ( p_dir_l < idpoint )
      {
      yaxis.x_gm = - yaxis.x_gm;
      yaxis.y_gm = - yaxis.y_gm;
      yaxis.z_gm = - yaxis.z_gm;
      }
    }

  else
    {
    varkon_erinit();
    sprintf(errbuf,"%d%%sur868*curmod", (int)reverse);
    return(varkon_erpush("SU8213",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*curmod yaxis %f %f %f \n", 
            yaxis.x_gm, yaxis.y_gm, yaxis.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur868*curmod*Exit \n" );
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
   DBint  n_alloc;       /* Size of allocated memory area           */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur868*suralloc Enter ** \n");
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Surface type and number of patches in U and V to p_surout        */
/*                                                                 !*/

  n_ulines         = p_spine_mod->ns_cu + 1;
  n_vlines         = p_cur_mod->ns_cu + 1;
  p_surout->typ_su = LFT_SUR;
  p_surout->nu_su  = (short)(n_ulines-1);
  p_surout->nv_su  = (short)(n_vlines-1);
  n_alloc          = (n_ulines-1)*(n_vlines-1);

/*!                                                                 */
/* Dynamic allocation of area (pp_patout) for the topological       */
/* patches (patches of type TOP_PAT).                               */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,n_alloc))==NULL)
 {                                
 sprintf(errbuf, "(alloc)%%sur868*suralloc");
 return(varkon_erpush("SU2993",errbuf));
 }                                 

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_patout.                              */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= n_alloc;i_s= i_s+1) 
     {
     varkon_ini_gmpat (p_frst  +i_s-1);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*suralloc Memory allocated for %d TOP_PAT patches\n",
                   (int)n_alloc );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*suralloc Number of U lines %d and V lines %d\n",
                   (int)(n_ulines-1),(int)(n_vlines-1) );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*suralloc p_surout->typ_su %d  ->nu_su %d  ->nv_su %d\n",
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

   static short surcrea (  )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*          p_frst          Pointer to the first patch              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  iu;            /* Loop index corresponding to u patch     */
   DBint  iv;            /* Loop index corresponding to v patch     */
   DBint  patch_type;    /* Type of geometric patch                 */
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
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur868*surcrea  Enter ** \n");
fflush(dbgfil(SURPAC));
}
#endif

   p_lofting   = NULL;
   
   patch_type = LFT_PAT;

/*!                                                                 */
/* Start loop patches in the output surface                         */
/*                                                                 !*/

for ( iv=0; iv<n_vlines-1; ++iv )        /* Start loop V patches    */
  {
  for ( iu=0; iu<n_ulines-1; ++iu )      /* Start loop U patches    */
    {


/*!                                                                 */
/*   Current pointer to the topological patch p_t.                  */
/*                                                                 !*/

     p_t = p_frst + iu*(n_vlines-1) + iv;

/*!                                                                 */
/*   Dynamic allocation of area for one geometric patch.            */
/*   Call of function DBcreate_patches.                             */
/*                                                                 !*/

     if ((p_gpat=DBcreate_patches(patch_type,1)) ==NULL)
       {                                   
       sprintf(errbuf,"(allocg)%%sur868*surcrea"); 
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
     p_t->spek_c = p_gpat;               /* Secondary patch (C ptr) */
     p_t->su_pat = 0;                    /* Topological adress      */
     p_t->sv_pat = 0;                    /* secondary patch not def.*/
     p_t->iu_pat = (short)(iu + 1);      /* Topological adress for  */
     p_t->iv_pat = (short)(iv + 1);      /* current (this) patch    */
     p_t->us_pat = (DBfloat)iu+1.0;        /* Start U on geo patch  */
     p_t->ue_pat = (DBfloat)iu+2.0-comptol;/* End   U on geo patch  */
     p_t->vs_pat = (DBfloat)iv+1.0;        /* Start V on geo patch  */
     p_t->ve_pat = (DBfloat)iv+2.0-comptol;/* End   V on geo patch  */

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

    status= patcrea  ( p_lofting, iu, iv );
    if (status<0) 
      {
      sprintf(errbuf,"patcrea%%sur868*surcrea");
      return(varkon_erpush("SU2973",errbuf));
      }


        }  /* End loop i_u   */
     }  /* End loop i_v   */
/*!                                                                 */
/* End   loop patches in the output surface i_seg= 0, . ,n_ulines-1 */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur868*surcrea  Surface created with %d LFT_PAT patches\n",
           (int)((n_ulines-1)*(n_vlines-1)) );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

/*!********* Internal ** function ** patcrea  ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates one geometrical patch in the surface        */

        static short patcrea ( p_lofting, iu, iv )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL  *p_lofting;   /* Current geometric lofting   patch (ptr) */
   DBint    iu;          /* Loop index corresponding to u patch     */
   DBint    iv;          /* Loop index corresponding to v patch     */
/*      p_spineseg_mod, p_cur_mod and yaxis                         */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
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
  "sur868*patcrea  Enter ** iu %d iv %d\n", (int)iu, (int)iv );
  }
#endif

/*!                                                                 */
/* 1. Spine curve segment to current lofting patch.                 */
/*                                                                 !*/

   V3MOME((char *)(p_spineseg_mod+iu),
          (char *)(&p_lofting->spine),sizeof(DBSeg));

/*!                                                                 */
/* 2. Section curve segment to current lofting patch.               */
/*                                                                 !*/

   V3MOME((char *)(p_seg_mod+iv),
          (char *)(&p_lofting->lims),sizeof(DBSeg));


/*!                                                                 */
/* 3. Fixed vector direction (yaxis) to the lofting patch.          */
/*                                                                 !*/


    p_lofting->pval.c0x = yaxis.x_gm;     
    p_lofting->pval.c0y = yaxis.y_gm;     
    p_lofting->pval.c0z = yaxis.z_gm;     
    p_lofting->pval.c0  =        0.0;     
    p_lofting->pval.c1x =        0.0;     
    p_lofting->pval.c1y =        0.0;     
    p_lofting->pval.c1z =        0.0;     
    p_lofting->pval.c1  =        0.0;     
    p_lofting->pval.c2x =        0.0;     
    p_lofting->pval.c2y =        0.0;     
    p_lofting->pval.c2z =        0.0;     
    p_lofting->pval.c2  =        0.0;     
    p_lofting->pval.c3x =        0.0;     
    p_lofting->pval.c3y =        0.0;     
    p_lofting->pval.c3z =        0.0;     
    p_lofting->pval.c3  =        0.0;     


/*!                                                                 */
/* 4. The P-value flag is used as flag for swept surface            */
/*    (let p_flag= 5).                                              */
/*                                                                 !*/

  p_lofting->p_flag = 5;

/*!                                                                 */
/* 5. The offset for the patch is zero (0).                         */
/*                                                                 !*/

  p_lofting->ofs_pat = 0.0;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur868*patcrea  Exit \n");
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




