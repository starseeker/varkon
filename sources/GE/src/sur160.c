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
/*  Function: varkon_sur_closestpt                 File: sur160.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the closest point on a surface.         */
/*                                                                  */
/*  Input is an external point and the requested solution number.   */
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
/*  Output is the closest point for the requested solution number.  */
/*  The points will be ordered either according to R*3 distance     */
/*  to the given external R*3 point or in the u,v plane to the      */
/*  given U,V start point (if there is no start u,v point given     */
/*  will U= 1.0 and V= 1.0 be used).                                */
/*                                                                  */
/*  Note that this function not will end with an error if the       */
/*  requested solution number not exists and ocase > 1000.          */
/*  The output U,V point will only be (-1.0,-1.0) in this case.     */
/*  This is unlike all other geometry functions in surpac & geopac! */
/*  The reason for this option is that the closest point            */
/*  computation is so heavy. First finding out if a solution        */
/*  exists (N_INV_ONS) would take too much time in many cases.      */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-09   Originally written                                 */
/*  1994-10-09   Call of varkon_sur_order, p_no_points and          */
/*               ocase+1000 added                                   */
/*  1995-08-26   scase added (initialized). nu,nv,s_lim erased      */
/*  1995-09-16   pat_tra added (initialized).                       */
/*  1996-06-12   ntol -> 100*ntol                                   */
/*  1998-02-08   NURBS added                                        */
/*  1999-03-17   Bug s_uvpt_a was not added like in sur162/163      */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_closestpt  Closest point on a surface       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol            * Coordinate tolerance                    */
/* varkon_ntol            * Surface normal tolerance                */
/* varkon_ini_spoint      * Initialize SPOINT                       */
/* varkon_sur_ppts        * Calculation of SPOINT points            */
/* varkon_sur_order       * Order surface (solut.) points           */
/* varkon_sur_uvmap       * Map UV point                            */
/* varkon_sur_uvmapr      * Map UV pt from NURBS to MBS             */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2613 = Requested solution no .. does not exist                 */
/* SU2623 = Requested solution number .. is not between 1 and SMAX  */
/* SU2943 = Called function .. failed in varkon_sur_closestpt       */
/* SU2813 = Start point u , v outside surface  varkon_sur_closestpt */
/* SU2633 = Ordering case .. not implemented   varkon_sur_closestpt */
/* SU2993 = Severe program error ( ) in varkon_sur_closestpt        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_sur_closestpt (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBVector *p_extpt,     /* External point                    (ptr) */
  DBVector *p_s_uvpt,    /* Start U,V point                         */
                         /* p_s_uvpt.z_gm < 0: Not defined.         */
  DBint    ocase,        /* Point ordering case                     */
                         /* Eq. 1: Ordered according to R*3 distance*/
                         /* Eq. 2: Ordered according to UV  distance*/
                         /* (No error exit if 1000 is added)        */
  DBint    acc,          /* Computation accuracy (case)             */
                         /* Undefined p_s_uvpt => all surf. patches */
                         /* Defined   p_s_uvpt => One patch  search */
                         /* Eq. 1: All patches connect. to p_s_uvpt */
                         /* Defined p_s_uvpt => Nine patches search */
                         /* Eq. 2: Next layer of patches ...        */
  DBint    sol_no,       /* Requested solution number               */

/* Out:                                                             */
  DBint   *p_no_points,  /* Total number of solution points   (ptr) */
  DBVector *p_cpt,       /* Closest point in R*3 space        (ptr) */
  DBVector *p_cuvpt )    /* Closest point in U,V space        (ptr) */
                         /* *p_cuvpt.z_gm= distance to surface      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/* TODO                                                             */
/* Make uv_out and r3_out to output variables, just as in           */
/* function sur161 !!!!!!!!                                         */
/*                                                                  */
  DBVector uv_out[SMAX]; /* All U,V solution pts. Ordered           */
  DBVector r3_out[SMAX]; /* All R*3 solution pts. Ordered           */


  DBSurf  *psura[ISMAX]; /* All interrogation surfaces        (ptrs)*/
  DBPatch *ppata[ISMAX]; /* Alloc. areas for patch data to psura    */
  DBint    n_isur;       /* Number of interrogation surfaces        */
  PROJECT  prodat;       /* Projection computation data             */
  SPOINT   sptable[SPMAX];/* Surface (solution) point table  SPOINT */
                         /* TODO Change to dynamic allocation !!!   */
  DBint    snr;          /* Total number of records      in SPOINT  */
  DBint    ocase_l;      /* Point ordering case (ocase-1000 .. )    */
  DBVector s_uvpt_a[1];  /* Start U,V point                         */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  SPOINT   sprec;        /* Surface (solution) point   record       */
  DBint    i_r;          /* Loop indec record                       */
  DBfloat  ntol;         /* Surface normal tolerance                */
  DBfloat  ctol;         /* Coordinate tolerance                    */
  DBVector refpt;        /* Comparison point (R*3 or U,V)           */
  EVALS    xyz_in[SMAX]; /* All input  (solution) points            */
  DBint    n_inpts;      /* Number of input  points                 */
  EVALS    xyz_out[SMAX];/* All output (solution) points            */
  DBint    n_outpts;     /* Number of output points                 */
  DBint    i_s;          /* Loop index solution point               */
  DBfloat  u_mbs;        /* MBS    U parameter value                */
  DBfloat  v_mbs;        /* MBS    V parameter value                */
  DBfloat  u_map;        /* Mapped U parameter value                */
  DBfloat  v_map;        /* Mapped V parameter value                */
  DBfloat  u_nurb;       /* Global (NURBS) U parameter value        */
  DBfloat  v_nurb;       /* Global (NURBS) V parameter value        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*!-------------------- Flow chart ---------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_closestpt !                           */
/*               !      (sur160)        !                           */
/*               !______________________!                           */
/*       ___________________!_______________________________        */
/*  ____!_____         ______!______     _____!_____   _____!_____  */
/* !    1     !       !      2      !   !     3     ! !     4     ! */
/* !  Check   !       ! Computation !   ! Calculate ! ! Order and ! */
/* !  input,  !       !     data    !   !    all    ! !  delete   ! */
/* ! initiate !       !             !   !   points  ! ! equal pts ! */
/* !__________!       !_____________!   !___________! !___________! */
/*      !________                              !             !      */
/*  ____!___  ___!____                     ____!___      ____!___   */
/* !        !!        !                   !        !    !        !  */
/* ! varkon !! varkon !                   ! varkon !    ! varkon !  */
/* ! _ctol  !!  _ini  !                   !  _sur  !    !  _sur  !  */
/* ! _ntol  !! spoint !                   !  _ppts !    ! _order !  */
/* ! sur752 !!        !                   !        !    !        !  */
/* ! sur751 !! sur777 !                   ! sur924 !    ! sur980 !  */
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
  "sur160 Enter***varkon_sur_closestpt** p_extpt %10.2f %10.2f %10.2f\n"
   , p_extpt->x_gm , p_extpt->y_gm , p_extpt->z_gm    );
  }
#endif
 
/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_ntol (sur752) and varkon_ctol (sur751).           */
/*                                                                 !*/

   ntol=varkon_ntol();
   ctol=varkon_ctol();

   ntol = ntol*100.0; 

/*!                                                                 */
/* Initialize table SPOINT                                          */
/* Calls of varkon_ini_spoint (sur777).                             */
/*                                                                 !*/

   for (i_r=1; i_r<=SPMAX; i_r= i_r+1)
     {
    varkon_ini_spoint (&sprec);
    sptable[i_r-1] = sprec;
    }

   for (i_s=1; i_s<=SMAX;     i_s= i_s+1) /* Initialize UV  solutions*/
     { 
     uv_out[i_s-1].x_gm = F_UNDEF;
     uv_out[i_s-1].y_gm = F_UNDEF;
     uv_out[i_s-1].z_gm = F_UNDEF;
     r3_out[i_s-1].x_gm = F_UNDEF;
     r3_out[i_s-1].y_gm = F_UNDEF;
     r3_out[i_s-1].z_gm = F_UNDEF;
     }

#ifdef  DEBUG
     u_mbs   = F_UNDEF;
     v_mbs   = F_UNDEF;
     u_map   = F_UNDEF;
     v_map   = F_UNDEF;
     u_nurb  = F_UNDEF;
     v_nurb  = F_UNDEF;
#endif


/*!                                                                 */
/* Partial check of p_s_uvpt (if < 1 )                                */
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
       "sur160 Error start UV point %f %f is defined (z_gm= %f)\n",
            p_s_uvpt->x_gm,p_s_uvpt->y_gm,p_s_uvpt->z_gm);
       }
#endif
     sprintf(errbuf,"%6.2f %6.2f %%varkon_sur_closestpt", 
            p_s_uvpt->x_gm,p_s_uvpt->y_gm);
     varkon_erinit();
     return(varkon_erpush("SU2813",errbuf));
     }
}

   if ( sol_no >= 1 && sol_no <= SMAX )
      {
      /* OK */
      }
   else
     {
     sprintf(errbuf,"%d %% %d sur160", (int)sol_no , SMAX );
     varkon_erinit();
     return(varkon_erpush("SU2623",errbuf));
     }

   if      ( ocase ==    1 || ocase ==    2 )
      ocase_l = ocase;
   else if ( ocase == 1001 || ocase == 1002 )
      ocase_l = ocase - 1000;
   else
     {
     sprintf(errbuf,"%d %%varkon_sur_closestpt", (int)ocase );
     varkon_erinit();
     return(varkon_erpush("SU2633",errbuf));
     }


/*!                                                                 */
/* 2. Data for the general surface point interrogation function     */
/* _____________________________________________________________    */

/* Input surface to psura and ppata (one interrogation surface)     */
/*                                                                 !*/

   n_isur     = 1;       /* Number of interrogation surfaces        */
   psura[0] = p_sur;     /* All interrogation surfaces        (ptrs)*/
   ppata[0] = p_pat;     /* Alloc. areas for patch data to psura    */

/*!                                                                 */
/* Interrogation (computation) data to prodat (PROJECT):            */
/* Interrogation case is closest (projected) point CLOSEPT          */
/* One-dimensional interpolation method is three pt parabola.       */
/* Number of restarts in the patch nstart =   4                     */
/* Maximum number of iterations    maxiter=  50                     */
/* Ordering case ocase (ocase_l) and accuracy acc to prodat.        */
/* !!! TODO Add ptr to fctn that calculates obj. fctn, gradient,    */
/* !!! Hessian shall be added                   !!!!!!!!!!!!!!!!    */
/*                                                                 !*/

   prodat.ctype    = CLOSEPT;
   prodat.method   = I_UNDEF;
   prodat.nstart   =  4;    
   prodat.maxiter  = 50;   
   prodat.ocase    = ocase_l;
   prodat.acc      = acc;
   prodat.ctol     = ctol;
   prodat.ntol     = ntol;
   prodat.scase    = 1;
   prodat.pat_tra.g11 = 0.0;
   prodat.pat_tra.g12 = 0.0;
   prodat.pat_tra.g13 = 0.0;
   prodat.pat_tra.g14 = 0.0;
   prodat.pat_tra.g21 = 0.0;
   prodat.pat_tra.g22 = 0.0;
   prodat.pat_tra.g23 = 0.0;
   prodat.pat_tra.g24 = 0.0;
   prodat.pat_tra.g31 = 0.0;
   prodat.pat_tra.g32 = 0.0;
   prodat.pat_tra.g33 = 0.0;
   prodat.pat_tra.g34 = 0.0;
   prodat.pat_tra.g41 = 0.0;
   prodat.pat_tra.g42 = 0.0;
   prodat.pat_tra.g43 = 0.0;
   prodat.pat_tra.g44 = 0.0;
/* prodat.objfunc  = varkon_int_nearpt; TODO To be added   */

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
       sprintf(errbuf,"sur360%%sur160");
       return(varkon_erpush("SU2943",errbuf));
       }
     }
   else
     {
     u_map = p_s_uvpt->x_gm;   
     v_map = p_s_uvpt->y_gm;   
     }

/* Start U,V point (mapped!)                                        */
   s_uvpt_a[0].x_gm = u_map;
   s_uvpt_a[0].y_gm = v_map;
   s_uvpt_a[0].z_gm = p_s_uvpt->z_gm;

   snr = 1;
   sptable[snr-1].extpt.x_gm    = p_extpt->x_gm;   
   sptable[snr-1].extpt.y_gm    = p_extpt->y_gm;   
   sptable[snr-1].extpt.z_gm    = p_extpt->z_gm;   
   sptable[snr-1].startuvpt.x_gm= u_map;   
   sptable[snr-1].startuvpt.y_gm= v_map;   
   sptable[snr-1].startuvpt.z_gm= p_s_uvpt->z_gm;   
   sptable[snr-1].proj.x_gm= 0.0;     
   sptable[snr-1].proj.y_gm= 0.0;     
   sptable[snr-1].proj.z_gm= 0.0;     
   sptable[snr-1].extptnorm.x_gm= 0.0;     
   sptable[snr-1].extptnorm.y_gm= 0.0;     
   sptable[snr-1].extptnorm.z_gm= 0.0;     
   sptable[snr-1].extuvpt.x_gm= 0.0;     
   sptable[snr-1].extuvpt.y_gm= 0.0;     
   sptable[snr-1].extuvpt.z_gm= 0.0;     


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur160 External point %f %f %f \n",
             p_extpt->x_gm,p_extpt->y_gm,p_extpt->z_gm);
if ( p_s_uvpt->z_gm >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur160 Start UV point %f %f is defined (z_gm= %f)\n",
            u_map, v_map, p_s_uvpt->z_gm);
}
else
{
fprintf(dbgfil(SURPAC),
"sur160 Start UV point is not defined (p_s_uvpt->z_gm= %f)\n",
            p_s_uvpt->z_gm);
}
fprintf(dbgfil(SURPAC),
"sur160 Computation accuracy %d Requested solution no %d\n",
                          (int)acc, (int)sol_no);
fprintf(dbgfil(SURPAC),
"sur160 Computation ocase %d ocase_l %d \n",
                          (int)ocase, (int)ocase_l   );
fprintf(dbgfil(SURPAC),
"sur160 Position tolerance ctol %f Angle tolerance ntol %15.10f\n",
                          ctol, ntol );
fprintf(dbgfil(SURPAC),
"sur160 Input number of records in SPOINT snr= %d (maximum no %d)\n",
                          (int)snr , SPMAX );
}
#endif


/*!                                                                 */
/* 3. Calculate all the solution points                             */
/* ____________________________________                             */
/*                                                                  */
/* Find all nearest points to the input external point.             */
/* Call of varkon_sur_ppts (sur924).                                */
/*                                                                 !*/

   status=varkon_sur_ppts
      (psura,ppata,s_uvpt_a,n_isur,(IRUNON*)&prodat,sptable,&snr);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_ppts%%varkon_sur_closestpt ");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 4. Retrieve solution point from the SPOINT table                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Order solution points and delete equal solutions.                */
/* Call of varkon_sur_order (sur980).                               */
/*                                                                 !*/

/* Solution points to local array                                   */

   n_inpts = 0;
   for (i_r=1; i_r<=snr;   i_r= i_r+1)    /* Start loop all records */
     {                                    /*                        */
     if ( sptable[i_r-1].pflag > 0   )
        {
        n_inpts = n_inpts + 1;
        xyz_in[n_inpts-1] = sptable[i_r-1].spt;
        }
     }                                    /* End   loop             */

/*!                                                                 */
/* Order solution points and delete equal solutions.                */
/* Call of varkon_sur_order (sur980).                               */
/*                                                                 !*/

   if ( ocase_l == 1 )
     {
     refpt.x_gm = p_extpt->x_gm;
     refpt.y_gm = p_extpt->y_gm;
     refpt.z_gm = p_extpt->z_gm;
     }
   else if ( ocase_l == 2 )
     {
     if ( p_s_uvpt->z_gm >= 0 )
       {
       refpt.x_gm = u_map;
       refpt.y_gm = v_map;
       refpt.z_gm = 0.0;
       }
     else
       {
       refpt.x_gm = 1.0;
       refpt.y_gm = 1.0;
       refpt.z_gm = 0.0;
       }
     }

   status=varkon_sur_order
      (&refpt, xyz_in, n_inpts, ocase_l, xyz_out, &n_outpts);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_order%%varkon_sur_closestpt ");
        return(varkon_erpush("SU2943",errbuf));
        }

   *p_no_points= n_outpts;     /* Total number of solution points   */

   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) /* Start loop all solutions*/
     { 
     uv_out[i_s-1].x_gm = xyz_out[i_s-1].u;
     uv_out[i_s-1].y_gm = xyz_out[i_s-1].v;
     uv_out[i_s-1].z_gm = 0.0;               
     r3_out[i_s-1].x_gm = xyz_out[i_s-1].r_x;
     r3_out[i_s-1].y_gm = xyz_out[i_s-1].r_y;
     r3_out[i_s-1].z_gm = xyz_out[i_s-1].r_z;
     }

/*!                                                                 */
/* Map the UV points (for a NURBS surface).                         */
/* Calls of varkon_sur_uvmapr (sur362).                             */
/*                                                                 !*/


   for (i_s=1; i_s<=n_outpts; i_s= i_s+1) /* Start loop all solutions*/
     { 
     u_nurb = uv_out[i_s-1].x_gm;
     v_nurb = uv_out[i_s-1].y_gm;
     status=varkon_sur_uvmapr
             (p_sur,p_pat,u_nurb, v_nurb, &u_mbs,&v_mbs);
     if (status<0) 
       {
       sprintf(errbuf,"sur362%%sur160");
       return(varkon_erpush("SU2943",errbuf));
       }
     uv_out[i_s-1].x_gm = u_mbs + 1.0;
     uv_out[i_s-1].y_gm = v_mbs + 1.0;
     } /* End loop all solution points */



   if ( sol_no <= n_outpts ) 
      {
      p_cpt->x_gm   = xyz_out[sol_no-1].r_x;
      p_cpt->y_gm   = xyz_out[sol_no-1].r_y;
      p_cpt->z_gm   = xyz_out[sol_no-1].r_z;

      p_cuvpt->x_gm = uv_out[sol_no-1].x_gm;
      p_cuvpt->y_gm = uv_out[sol_no-1].y_gm;
      p_cuvpt->z_gm = 0.0;               
      }
   else
      {
      p_cpt->x_gm   =   1.23456789;         
      p_cpt->y_gm   =   1.23456789;         
      p_cpt->z_gm   =   1.23456789;         

      p_cuvpt->x_gm =  -1.0;                
      p_cuvpt->y_gm =  -1.0;                
      p_cuvpt->z_gm =  -1.0;               
      if ( ocase < 1000 )
        {
        sprintf(errbuf,"%d %% %d ", (int)sol_no , (int)n_outpts );
        varkon_erinit();
        return(varkon_erpush("SU2613",errbuf));
        }
     } /* End no solution */

/* 5. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur160 Total number of records in SPOINT snr= %d\n"
    , (int)snr );
  }
if ( dbglev(SURPAC) == 1 )
  {
  if ( sptable[0].pflag > 0   )
  {
  fprintf(dbgfil(SURPAC),
  "sur160 Exit varkon_sur_closestpt  Output UV pt %f %f Total no %d\n"
    , p_cuvpt->x_gm , p_cuvpt->y_gm , (int)*p_no_points  );
  }
  else
  {
  fprintf(dbgfil(SURPAC),
  "sur160 Exit***varkon_sur_closestpt** No solution found\n");
  }
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
