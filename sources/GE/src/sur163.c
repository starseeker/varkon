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
/*  Function: varkon_sur_int2linpt                 File: sur163.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  !!! TODO 1998-04-25                                !!!!!!!      */
/*  !!! Not yet tested for given start point and NURBS !!!!!!!      */
/*                                                                  */
/*  The function calculates the intersect between a straight        */
/*  line and a surface.                                             */
/*                                                                  */
/*  Input is an external point, a projection vector  and the        */
/*  requested solution number.                                      */
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
/*  Output from the function is an array with all solutions.        */
/*                                                                  */
/*  Remark                                                          */
/*  ------                                                          */
/*  This function is nearly identical to sur161. The difference     */
/*  is that output is an array of EVALS variables, which not        */
/*  have been ordered.                                              */
/*  If any changes are made in this function it is likely that      */
/*  also sur161 need to be modified !                               */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-04-25   Originally written                                 */
/*  1998-04-29   Initialization of SPOINT only for Debug On         */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_int2linpt  Intersect line and surface       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol             * Coordinate tolerance                   */
/* varkon_ntol             * Surface normal tolerance               */
/* varkon_tra_intlinsur    * Create local coord. system             */
/* varkon_ini_spoint       * Initialize SPOINT                      */
/* varkon_sur_ppts         * Calculation of SPOINT points           */
/* varkon_sur_selsol       * Select solution points                 */
/* varkon_sur_uvmap        * Map UV point                           */
/* varkon_sur_uvmapr       * Map UV pt from NURBS to MBS            */
/* varkon_erinit           * Initialize error message stack         */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2613 = Requested solution no .. does not exist                 */
/* SU2623 = Requested solution number .. is not between 1 and SMAX  */
/* SU2943 = Called function .. failed in varkon_sur_int2linpt       */
/* SU2813 = Start point u , v outside surface  varkon_sur_int2linpt */
/* SU2633 = Ordering case .. not implemented   varkon_sur_int2linpt */
/* SU2343 = Solution selection case .. not implemented              */
/* SU2993 = Severe program error ( ) in varkon_sur_int2linpt        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_sur_int2linpt (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_sur,       /* Surface                           (ptr) */
  DBPatch  *p_pat,       /* Alloc. area for topol. patch data (ptr) */
  DBVector *p_extpt,     /* External point                    (ptr) */
  DBVector *p_v_proj,    /* Projection vector (shooting dir.) (ptr) */
  DBVector *p_s_uvpt,    /* Start U,V point                   (ptr) */
                         /* p_s_uvpt.z_gm < 0: Not defined.         */
  DBint    scase,        /* Solution selection case                 */
                         /* Eq. 1: All intersect points             */
                         /* Eq. 2: Intersects in shooting direction */
                         /* Eq. 3: As 2 but only intersect pts with */
                         /*        surface normals in the shooting  */
                         /*        direction will be output         */
                         /* Eq. 4: As 2 but only intersect pts with */
                         /*        surface normals in the opposite  */
                         /*        shooting direction will be output*/
                         /* Eq. 5: As 3 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        opposite direct. to shooting dir */
                         /* Eq. 6: As 4 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        same     direct. to shooting dir */
  DBint    acc,          /* Computation accuracy (case)             */
                         /* Undefined p_s_uvpt => all surf. patches */
                         /* Defined   p_s_uvpt => One patch  search */
                         /* Eq. 1: All patches connect. to p_s_uvpt */
                         /* Defined p_s_uvpt => Nine patches search */
                         /* Eq. 2: Next layer of patches ...        */

/* Out:                                                             */
  DBint   *p_no_points,  /* Total number of solution points   (ptr) */
  EVALS    xyz_out[] )   /* All selected output solution points     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBTmat   pat_tra;      /* Transformation matrix for patches       */
  DBSurf  *psura[ISMAX]; /* All interrogation surfaces        (ptrs)*/
  DBPatch *ppata[ISMAX]; /* Alloc. areas for patch data to psura    */
  DBVector s_uvpt_a[1];  /* Start U,V point                         */
  DBint    n_isur;       /* Number of interrogation surfaces        */
  PROJECT  prodat;       /* Projection computation data             */
  SPOINT   sptable[SPMAX];/* Surface (solution) point table  SPOINT */
                         /* TODO Should be dynamic allocation !!!   */
  DBint    snr;          /* Total number of records      in SPOINT  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint  i_r;            /* Loop index record                       */
  DBfloat ntol;          /* Surface normal tolerance                */
  DBfloat ctol;          /* Coordinate tolerance                    */
  DBVector refpt;        /* Comparison point  (R*3 or U,V)          */
  DBVector refvec;       /* Comparison vector (R*3 or U,V)          */
  EVALS  xyz_in[9*SMAX]; /* All input  (solution) points            */
  EVALS  xyz_sel[9*SMAX];/* All selected solution points            */
  DBint  n_inpts;        /* Number of input  points                 */
  DBint  n_selpts;       /* Number of selected solution points      */
  DBint  n_outpts;       /* Number of output   solution points      */
  DBint  i_s;            /* Loop index solution point               */
  DBfloat u_mbs;         /* MBS    U parameter value                */
  DBfloat v_mbs;         /* MBS    V parameter value                */
  DBfloat u_map;         /* Mapped U parameter value                */
  DBfloat v_map;         /* Mapped V parameter value                */
  DBfloat u_nurb;        /* Global (NURBS) U parameter value        */
  DBfloat v_nurb;        /* Global (NURBS) V parameter value        */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */
#ifdef  DEBUG
  SPOINT sprec;          /* Surface (solution) point   record       */
#endif

/*!-------------------- Flow chart ---------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_int2linpt !                           */
/*               !      (sur163)        !                           */
/*               !______________________!                           */
/*       ___________________!_______________________________        */
/*  ____!_____         ______!______     _____!_____   _____!_____  */
/* !    1     !  2    !      3      !   !     4     ! !     5     ! */
/* !  Check   !       ! Computation !   ! Calculate ! !  Select   ! */
/* !  input,  !       !     data    !   !    all    ! !  points   ! */
/* ! initiate !       !             !   !   points  ! !           ! */
/* !__________!       !_____________!   !___________! !___________! */
/*      !________                              !             !      */
/*  ____!___  ___!____                     ____!___      ____!___   */
/* !        !!        !                   !        !    !        !  */
/* ! varkon !! varkon !                   ! varkon !    ! varkon !  */
/* ! _ctol  !!  _ini  !                   !  _sur  !    !  _sur  !  */
/* ! _ntol  !! spoint !                   !  _ppts !    !_selsol !  */
/* ! sur752 !!        !                   !        !    !        !  */
/* ! sur751 !! sur777 !                   ! sur924 !    ! sur984 !  */
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
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur163 Enter*varkon_sur_int2linpt* scase %d acc %d\n"
 , (int)scase, (int)acc   );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur163 p_extpt     %10.2f %10.2f %10.2f\n"
 , p_extpt->x_gm , p_extpt->y_gm , p_extpt->z_gm    );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur163 p_v_proj  %10.2f %10.2f %10.2f\n"
 , p_v_proj->x_gm , p_v_proj->y_gm , p_v_proj->z_gm    );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur163 p_s_uvpt  %10.2f %10.2f %10.2f\n"
 , p_s_uvpt->x_gm , p_s_uvpt->y_gm , p_s_uvpt->z_gm    );
fflush(dbgfil(SURPAC));
}
#endif
 

/* Initialize the number of output points                           */
   *p_no_points= I_UNDEF;

/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_ntol (sur752) and varkon_ctol (sur751).           */
/*                                                                 !*/

   ntol=varkon_ntol();
   ctol=varkon_ctol();

/*!                                                                 */
/* Initialize table SPOINT for Debug On                             */
/* Calls of varkon_ini_spoint (sur777).                             */
/*                                                                 !*/

#ifdef  DEBUG
   for (i_r=1; i_r<=SPMAX; i_r= i_r+1)
     { 
    varkon_ini_spoint (&sprec);
    sptable[i_r-1] = sprec; 
    }
#endif

#ifdef  DEBUG
     u_mbs    = F_UNDEF;
     v_mbs    = F_UNDEF;
     u_map    = F_UNDEF;
     v_map    = F_UNDEF;
     u_nurb   = F_UNDEF;
     v_nurb   = F_UNDEF;
     n_selpts = I_UNDEF;
     n_outpts = I_UNDEF;
#endif

/*!                                                                 */
/* Check of start UV point p_s_uvpt                                 */
/*                                                                  */
/*                                                                 !*/

if ( p_s_uvpt->z_gm >= 0 )
{
  if ( p_s_uvpt->x_gm < 1.0 || p_s_uvpt->y_gm < 1   ||
       p_s_uvpt->x_gm > p_sur->nu_su+1.0            || 
       p_s_uvpt->y_gm > p_sur->nv_su+1.0                )
     {
#ifdef  DEBUG
     if ( dbglev(SURPAC) == 1 )
       {
       fprintf(dbgfil(SURPAC),
       "sur163 Error start UV point %f %f is defined (z_gm= %f)\n",
            p_s_uvpt->x_gm,p_s_uvpt->y_gm,p_s_uvpt->z_gm);
       }
#endif
       sprintf(errbuf,"%6.2f %6.2f %%(sur163)", 
            p_s_uvpt->x_gm,p_s_uvpt->y_gm);
       varkon_erinit();
       return(varkon_erpush("SU2813",errbuf));
     }
}

   if      ( scase == 1 || scase == 2 || scase == 3 || scase == 4 ||
             scase == 5 || scase == 6 )
     {
     ;
     }
   else
     {
     sprintf(errbuf,"%d %%(sur163)", (int)scase );
     varkon_erinit();
     return(varkon_erpush("SU2343",errbuf));
     }

/*!                                                                 */
/* 2. Calculate transformation matrix for the patches (surface)     */
/* ___________________________________________________________      */
/*                                                                  */
/* Create local system with X axis equal to input project vector    */
/* and the origin equal to the external point.                      */
/* Call of varkon_tra_intlinsur (sur690).                           */
/*                                                                 !*/

    status = varkon_tra_intlinsur (p_extpt,p_v_proj,&pat_tra);
if (status<0) 
  {
  sprintf(errbuf,"sur690%%sur163");
  return(varkon_erpush("SU2943",errbuf));
  }


/*!                                                                 */
/* 3. Data for the general surface point interrogation function     */
/* _____________________________________________________________    */
/*                                                                  */
/* Input surface to psura and ppata (one interrogation surface)     */
/*                                                                 !*/

   n_isur     = 1;         /* Number of interrogation surfaces      */
   psura[0] = p_sur;       /* All interrogation surfaces     (ptrs) */
   ppata[0] = p_pat;       /* Alloc. areas for patch data to psura  */

/*!                                                                 */
/* Interrogation (computation) data to prodat (PROJECT):            */
/* Interrogation case is line surface intersect LINTSUR             */
/* The surface is Transformed to a local system with X axis         */
/* equal to the input straight line.                                */
/* One-dimensional interpolation method is three pt parabola.       */
/* Number of restarts in the patch nstart =   5                     */
/* Maximum number of iterations    maxiter= 100                     */
/* Accuracy acc to prodat.                                          */
/*                                                                 !*/

   prodat.ctype    = LINTSUR;
   prodat.method   = I_UNDEF;
   prodat.nstart   =  5;    
   prodat.maxiter  = 50;   
   prodat.maxiter  = 100;   /* 1995-08-26 */
   prodat.ocase    = I_UNDEF;
   if ( p_sur->typ_su == LFT_SUR ) prodat.acc = 2*acc;
   else                            prodat.acc =   acc;
   prodat.ctol     = ctol;
   prodat.ntol     = ntol;
   prodat.scase    = scase;
   prodat.pat_tra  = pat_tra;
/* prodat.objfunc  = varkon_int_linsurpt; TODO Add           */

/*!                                                                 */
/* Data to the surface point table SPOINT:                          */
/* Number of records is one (1).                                    */
/* External (input) pt and UV start pt to table SPOINT, record 1    */
/* Map the UV start pt if defined. Call varkon_sur_uvmap (sur360)   */
/*                                                                 !*/

   if  (   p_s_uvpt->z_gm >= 0.0 )
     {
     u_mbs = p_s_uvpt->x_gm - 1.0;   
     v_mbs = p_s_uvpt->y_gm - 1.0;   

    status=varkon_sur_uvmap(p_sur,p_pat,u_mbs,v_mbs,&u_map,&v_map);
    if (status<0) 
      {
      sprintf(errbuf,"sur360%%sur163");
      return(varkon_erpush("SU2943",errbuf));
      }
     }
   else
     {
     u_map = p_s_uvpt->x_gm;   
     v_map = p_s_uvpt->y_gm;   
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur163 Mapped pt  %f %f %f \n",
             u_map, v_map, p_s_uvpt->z_gm);
fflush(dbgfil(SURPAC));
}
#endif

/* Start U,V point (mapped!)                                        */
   s_uvpt_a[0].x_gm = u_map;
   s_uvpt_a[0].y_gm = v_map;
   s_uvpt_a[0].z_gm = p_s_uvpt->z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur163 Mapped pt  %f %f %f (2)\n",
             u_map, v_map, p_s_uvpt->z_gm);
fflush(dbgfil(SURPAC));
}
#endif

   snr = 1;
   sptable[snr-1].extpt.x_gm    = p_extpt->x_gm;   
   sptable[snr-1].extpt.y_gm    = p_extpt->y_gm;   
   sptable[snr-1].extpt.z_gm    = p_extpt->z_gm;   
   sptable[snr-1].startuvpt.x_gm= F_UNDEF; 
   sptable[snr-1].startuvpt.y_gm= F_UNDEF; 
   sptable[snr-1].startuvpt.z_gm= F_UNDEF;
   sptable[snr-1].proj.x_gm     = p_v_proj->x_gm;
   sptable[snr-1].proj.y_gm     = p_v_proj->y_gm;
   sptable[snr-1].proj.z_gm     = p_v_proj->z_gm;
   sptable[snr-1].extptnorm.x_gm= 0.0;     
   sptable[snr-1].extptnorm.y_gm= 0.0;     
   sptable[snr-1].extptnorm.z_gm= 0.0;     
   sptable[snr-1].extuvpt.x_gm= 0.0;     
   sptable[snr-1].extuvpt.y_gm= 0.0;     
   sptable[snr-1].extuvpt.z_gm= 0.0;     


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"sur163 External point    %f %f %f \n",
             p_extpt->x_gm,p_extpt->y_gm,p_extpt->z_gm);
  fprintf(dbgfil(SURPAC),"sur163 Projection vector %f %f %f \n",
             p_v_proj->x_gm,p_v_proj->y_gm,p_v_proj->z_gm);
if ( p_s_uvpt->z_gm >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur163 Start UV point %f %f is defined (z_gm= %f)\n",
            u_map, v_map, p_s_uvpt->z_gm);
}
else
{
fprintf(dbgfil(SURPAC),
"sur163 Start UV point is not defined (p_s_uvpt->z_gm= %f)\n",
            p_s_uvpt->z_gm);
}
fprintf(dbgfil(SURPAC),
"sur163 Computation accuracy %d \n",
                          (int)acc);
fprintf(dbgfil(SURPAC),
"sur163 Solution selection scase %d \n",
                          (int)scase  );
fprintf(dbgfil(SURPAC),
"sur163 Position tolerance ctol %f Angle tolerance ntol %15.10f\n",
                          ctol, ntol );
fprintf(dbgfil(SURPAC),
"sur163 Input number of records in SPOINT snr= %d (maximum no %d)\n",
                          (int)snr , SPMAX );
}
#endif


/*!                                                                 */
/* 4. Calculate all the solution points                             */
/* ____________________________________                             */
/*                                                                  */
/* Find all surface/line intersect points.                          */
/* Call of varkon_sur_ppts (sur924).                                */
/*                                                                 !*/

   status=varkon_sur_ppts
      (psura,ppata, s_uvpt_a,n_isur,(IRUNON*)&prodat,sptable,&snr);
   if (status<0) 
        {
        sprintf(errbuf,"sur924%%sur163");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 5. Retrieve solution point from the SPOINT table                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/

/* Solution points to local array                                   */



   if  ( snr > 9*SMAX )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur163 Number of records snr %d > 9*SMAX %d\n", (int)snr, 9*SMAX);
fflush(dbgfil(SURPAC));
  }
#endif
     sprintf(errbuf,"snr %d > 9*SMAX%%sur163",(int)snr);
     return(varkon_erpush("SU2993",errbuf));
     }
   



   n_inpts = 0;
   for (i_r=1; i_r<=snr;   i_r= i_r+1)    /* Start loop all records */
     {                                    /*                        */
     if ( sptable[i_r-1].pflag > 0   )
        {
        n_inpts = n_inpts + 1;
        xyz_in[n_inpts-1] = sptable[i_r-1].spt;
        }
     }                                    /* End   loop             */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur163 Number of solution (sur924) pts n_inpts %d\n",(int)n_inpts);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Select solution points in accordance with case scase             */
/* Call of varkon_sur_selsol (sur984).                              */
/*                                                                 !*/

   refpt.x_gm  = p_extpt->x_gm; 
   refpt.y_gm  = p_extpt->y_gm; 
   refpt.z_gm  = p_extpt->z_gm; 
   refvec.x_gm = p_v_proj->x_gm; 
   refvec.y_gm = p_v_proj->y_gm; 
   refvec.z_gm = p_v_proj->z_gm; 

   status=varkon_sur_selsol
      (&refpt, &refvec, xyz_in, n_inpts, scase, xyz_sel, &n_selpts);
   if (status<0) 
        {
        sprintf(errbuf,"sur984%%sur163");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Solution points to output array. Maximum number is SMAX (assume  */
/* line in surface for SMAX, i.e. no error)                          */
/*                                                                 !*/

   if  ( n_selpts > SMAX )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur163 Number of output pts n_selpts %d > SMAX %d ==> n_outpts %d\n", 
                      (int)n_selpts, SMAX, (int)n_outpts );
fflush(dbgfil(SURPAC));
  }
#endif

/*   Assume that the line lies in the surface                       */
     n_outpts = SMAX;
     }
   else  n_outpts = n_selpts;


   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) 
     { 
     xyz_out[i_s-1]  = xyz_sel[i_s-1];
     }

/*!                                                                 */
/* Map the UV points (for a NURBS surface).                         */
/* Calls of varkon_sur_uvmapr (sur362).                              */
/*                                                                 !*/


   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) 
     { 
     u_nurb = xyz_out[i_s-1].u;
     v_nurb = xyz_out[i_s-1].v;
     status=varkon_sur_uvmapr
             (p_sur,p_pat,u_nurb, v_nurb, &u_mbs,&v_mbs);
     if (status<0) 
       {
       sprintf(errbuf,"sur362%%sur163");
       return(varkon_erpush("SU2943",errbuf));
       }
       xyz_out[i_s-1].u = u_mbs + 1.0;
       xyz_out[i_s-1].v = v_mbs + 1.0;
     } /* End loop all solution points */




/* 6. Exit                                                          */

   *p_no_points= n_outpts;     /* Total number of solution points   */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur163 Total number of records in SPOINT snr= %d\n"
    , (int)snr );
  }
if ( dbglev(SURPAC) == 1 )
  {
   for (i_s=1; i_s<= *p_no_points; i_s= i_s+1) /* Start loop solutions*/
     { 
     fprintf(dbgfil(SURPAC),
     "sur163 Solution %d uv %8.5f %8.5f r3 %9.2f %9.2f %9.2f\n"
    , (int)i_s , xyz_sel[i_s-1].u, xyz_sel[i_s-1].v 
    , xyz_sel[i_s-1].r_x , xyz_sel[i_s-1].r_y , xyz_sel[i_s-1].r_z  );
     }
  }
if ( dbglev(SURPAC) == 1 )
  {
  if ( *p_no_points > 0   )
  {
  fprintf(dbgfil(SURPAC),
  "sur163 Exit varkon_sur_int2linpt Surf type %d No of points %d\n"
    ,(int)p_sur->typ_su, (int)*p_no_points  );
  fflush(dbgfil(SURPAC));
  }
  else
  {
  fprintf(dbgfil(SURPAC),
  "sur163 Exit varkon_sur_int2linpt Surface type %d No solutions!\n",
                 (int)p_sur->typ_su);
  fflush(dbgfil(SURPAC));
  }
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
