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
/*  Function: varkon_sur_intlinept                 File: sur161.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
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
/*  Remark                                                          */
/*  ------                                                          */
/*  This function is nearly identical to sur162. The difference     */
/*  is that multiple surfaces can be given as input to sur162       */
/*  but only one surface to sur161.                                 */
/*  If any changes are made in this function it is likely that      */
/*  also sur162 need to be modified !                               */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-06   Originally written                                 */
/*  1995-08-21   Output argument uv_out added                       */
/*  1995-08-26   scase added                                        */
/*  1995-08-28   nstart tests (8 necessary)                         */
/*  1995-09-03   Trimming acc, nstarts, tolerances                  */
/*  1995-09-16   Transform patches with pat_tra, not whole surface  */
/*               r3_out added                                       */
/*  1995-10-08   acc= acc*2 for LFT_SUR                             */
/*  1995-11-07   Check that shooting direction not is a zero vector */
/*  1996-01-28   Comments added and error message changed           */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1997-04-04   Array SMAX check added                             */
/*  1998-02-08   Mapping of UV pts for NURBS. An old point ordering */
/*               bug that may have affected the radar calculations! */
/*  1998-02-25   Missing ifdef DEBUG, s_uvpt_a added                */
/*               GEmktf_3p -> sur690                                */
/*  1998-04-29   Initialization of SPOINT only for Debug On         */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_intlinept  Intersect line and surface       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol             * Coordinate tolerance                   */
/* varkon_ntol             * Surface normal tolerance               */
/* varkon_tra_intlinsur    * Create local coord. system             */
/* varkon_ini_spoint       * Initialize SPOINT                      */
/* varkon_sur_ppts         * Calculation of SPOINT points           */
/* varkon_sur_order        * Order surface (solut.) points          */
/* varkon_sur_selsol       * Select solution points                 */
/* varkon_sur_uvmap        * Map UV point                           */
/* varkon_sur_uvmapr       * Map UV pt from NURBS to MBS            */
/* varkon_erinit           * Initiate error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2613 = Requested solution no .. does not exist                 */
/* SU2623 = Requested solution number .. is not between 1 and SMAX  */
/* SU2943 = Called function .. failed in varkon_sur_intlinept       */
/* SU2813 = Start point u , v outside surface  varkon_sur_intlinept */
/* SU2633 = Ordering case .. not implemented   varkon_sur_intlinept */
/* SU2343 = Solution selection case .. not implemented              */
/* SU2993 = Severe program error ( ) in varkon_sur_intlinept        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_sur_intlinept (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBVector *p_extpt,     /* External point                    (ptr) */
  DBVector *p_v_proj,    /* Projection vector (shooting dir.) (ptr) */
  DBVector *p_s_uvpt,    /* Start U,V point                   (ptr) */
                         /* p_s_uvpt.z_gm < 0: Not defined.         */
  DBint    ocase,        /* Point ordering case                     */
                         /* Eq. 1: Ordered according to R*3 distance*/
                         /* Eq. 2: Ordered according to UV  distance*/
                         /* (No error exit if 1000 is added)        */
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
  DBint    sol_no,       /* Requested solution number               */

/* Out:                                                             */
  DBint    *p_no_points, /* Total number of solution points   (ptr) */
  DBVector *p_cpt,       /* Closest point in R*3 space        (ptr) */
  DBVector *p_cuvpt,     /* Closest point in U,V space        (ptr) */
                         /* *p_cuvpt.z_gm= distance to surface      */
  DBVector  uv_out[],    /* All U,V solution pts. Ordered           */
  DBVector  r3_out[] )   /* All R*3 solution pts. Ordered           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBTmat   pat_tra;       /* Transformation matrix for patches      */
  DBSurf  *psura[ISMAX];  /* All interrogation surfaces       (ptrs)*/
  DBPatch *ppata[ISMAX];  /* Alloc. areas for patch data to psura   */
  DBVector s_uvpt_a[1];   /* Start U,V point                        */
  DBint    n_isur;        /* Number of interrogation surfaces       */
  PROJECT  prodat;        /* Projection computation data            */
  SPOINT   sptable[SPMAX];/* Surface (solution) point table  SPOINT */
                          /* ..  !! Borde allokeras dynamiskt !!!!  */
  DBint    snr;           /* Total number of records      in SPOINT */
  DBint    ocase_l;       /* Point ordering case (ocase-1000 .. )   */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint    i_r;            /* Loop index record                     */
  DBfloat  ntol;           /* Surface normal tolerance              */
  DBfloat  ctol;           /* Coordinate tolerance                  */
  DBVector refpt;          /* Comparison point  (R*3 or U,V)        */
  DBVector refvec;         /* Comparison vector (R*3 or U,V)        */
  EVALS    xyz_in[9*SMAX]; /* All input  (solution) points          */
  DBint    n_inpts;        /* Number of input  points               */
  EVALS    xyz_sel[9*SMAX];/* All selected solution points          */
  DBint    n_selpts;       /* Number of selected solution points    */
  EVALS    xyz_out[9*SMAX];/* All output (solution) points          */
  DBint    n_outpts;       /* Number of output points               */
  DBint    i_s;            /* Loop index solution point             */
  DBfloat  u_mbs;          /* MBS    U parameter value              */
  DBfloat  v_mbs;          /* MBS    V parameter value              */
  DBfloat  u_map;          /* Mapped U parameter value              */
  DBfloat  v_map;          /* Mapped V parameter value              */
  DBfloat  u_nurb;         /* Global (NURBS) U parameter value      */
  DBfloat  v_nurb;         /* Global (NURBS) V parameter value      */
  DBint    status;         /* Error code from called function       */
  char     errbuf[80];     /* String for error message fctn erpush  */
#ifdef  DEBUG
  SPOINT   sprec;          /* Surface (solution) point   record     */
#endif

/*!-------------------- Flow chart ---------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_intlinept !                           */
/*               !      (sur161)        !                           */
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
"sur161 Enter***varkon_sur_intlinept** p_extpt %10.2f %10.2f %10.2f\n"
 , p_extpt->x_gm , p_extpt->y_gm , p_extpt->z_gm    );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_ntol (sur752) and varkon_ctol (sur751).           */
/*                                                                 !*/

   ntol=varkon_ntol();
   ctol=varkon_ctol();

  ntol = 0.001*ntol; /* 10*TOL7= 0.01 before. Testing !!!!!  */
  ntol = ntol; /* 10*TOL7= 0.01 before. Testing !!!!!  */

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
       "sur161 Error start UV point %f %f is defined (z_gm= %f)\n",
            p_s_uvpt->x_gm,p_s_uvpt->y_gm,p_s_uvpt->z_gm);
       }
#endif
     sprintf(errbuf,"%6.2f %6.2f %%(sur161)", 
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
     sprintf(errbuf,"%d %% %d (sur161)", (int)sol_no , SMAX );
     varkon_erinit();
     return(varkon_erpush("SU2623",errbuf));
     }

   if      ( ocase ==    1 || ocase ==    2 )
      ocase_l = ocase;
   else if ( ocase == 1001 || ocase == 1002 )
      ocase_l = ocase - 1000;
   else
     {
     sprintf(errbuf,"%d %%(sur161)", (int)ocase );
     varkon_erinit();
     return(varkon_erpush("SU2633",errbuf));
     }

   if      ( scase == 1 || scase == 2 || scase == 3 || scase == 4 ||
             scase == 5 || scase == 6 )
     {
     ;
     }
   else
     {
     sprintf(errbuf,"%d %%(sur161)", (int)scase );
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
  sprintf(errbuf,"sur690%%sur161");
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
/* Ordering case ocase (ocase_l) and accuracy acc to prodat.        */
/* !!! TODO Add ptr to fctn that calculates objective and gradient  */
/* !!! Hessian shall be added                   !!!!!!!!!!!!!!!!    */
/*                                                                 !*/

   prodat.ctype    = LINTSUR;
   prodat.method   = I_UNDEF;
   prodat.nstart   =  5;    
   prodat.maxiter  = 50;   
   prodat.maxiter  = 100;
   prodat.ocase    = ocase_l;
   if ( p_sur->typ_su == LFT_SUR ) prodat.acc = 2*acc;
   else                            prodat.acc =   acc;
   prodat.ctol     = ctol;
   prodat.ntol     = ntol;
   prodat.scase    = scase;
   prodat.pat_tra  = pat_tra;
/* prodat.objfunc  = varkon_int_linsurpt; TODO Add !!!!      */

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

     status=varkon_sur_uvmap
       (p_sur,p_pat,u_mbs, v_mbs, &u_map, &v_map);
     if (status<0) 
       {
       sprintf(errbuf,"sur360%%sur161");
       return(varkon_erpush("SU2943",errbuf));
       }
     }
   else
     {
     u_map = p_s_uvpt->x_gm;   
     v_map = p_s_uvpt->y_gm;   
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur161 Mapped pt  %f %f %f \n",
             u_map, v_map, p_s_uvpt->z_gm);
fflush(dbgfil(SURPAC));
}
#endif

/* Start U,V point (mapped!)                                        */
   s_uvpt_a[0].x_gm = u_map;
   s_uvpt_a[0].y_gm = v_map;
   s_uvpt_a[0].z_gm = p_s_uvpt->z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur161 Mapped pt  %f %f %f (2)\n",
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
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur161 External point    %f %f %f \n",
             p_extpt->x_gm,p_extpt->y_gm,p_extpt->z_gm);
  fprintf(dbgfil(SURPAC),"sur161 Projection vector %f %f %f \n",
             p_v_proj->x_gm,p_v_proj->y_gm,p_v_proj->z_gm);
if ( p_s_uvpt->z_gm >= 0 )
{
fprintf(dbgfil(SURPAC),
"sur161 Start UV point %f %f is defined (z_gm= %f)\n",
            u_map, v_map, p_s_uvpt->z_gm);
}
else
{
fprintf(dbgfil(SURPAC),
"sur161 Start UV point is not defined (p_s_uvpt->z_gm= %f)\n",
            p_s_uvpt->z_gm);
}
fprintf(dbgfil(SURPAC),
"sur161 Computation accuracy %d Requested solution no %d\n",
                          (int)acc, (int)sol_no);
fprintf(dbgfil(SURPAC),
"sur161 Computation ocase %d ocase_l %d \n",
                          (int)ocase, (int)ocase_l   );
fprintf(dbgfil(SURPAC),
"sur161 Solution selection scase %d \n",
                          (int)scase  );
fprintf(dbgfil(SURPAC),
"sur161 Position tolerance ctol %f Angle tolerance ntol %15.10f\n",
                          ctol, ntol );
fprintf(dbgfil(SURPAC),
"sur161 Input number of records in SPOINT snr= %d (maximum no %d)\n",
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
        sprintf(errbuf,"sur924%%sur161");
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
  "sur161 Number of records snr %d > 9*SMAX %d\n", (int)snr, 9*SMAX);
fflush(dbgfil(SURPAC));
  }
#endif
     sprintf(errbuf,"snr %d > 9*SMAX%%sur161",(int)snr);
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
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur161 Number of solution (sur924) pts n_inpts %d\n",(int)n_inpts);
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
        sprintf(errbuf,"sur984%%sur161");
        return(varkon_erpush("SU2943",errbuf));
        }

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
      (&refpt, xyz_sel, n_selpts, ocase_l, xyz_out, &n_outpts);
   if (status<0) 
        {
        sprintf(errbuf,"sur980%%sur161");
        return(varkon_erpush("SU2943",errbuf));
        }



   if  ( n_outpts > SMAX )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur161 Number of output points n_outpts %d > SMAX %d\n", 
                      (int)n_outpts, SMAX);
fflush(dbgfil(SURPAC));
  }
#endif

/* Assume that the line lies in the surface    */
   n_outpts = SMAX;


#ifdef  TODO_SOMETHING_ELSE_PERHAPS  
     sprintf(errbuf,"n_outpts %d > SMAX%%sur161",(int)n_outpts);
     return(varkon_erpush("SU2993",errbuf));
#endif /*   TODO_SOMETHING_ELSE_PERHAPS   */
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
       sprintf(errbuf,"sur362%%sur161");
       return(erpush("SU2943",errbuf));
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

/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur161 Total number of records in SPOINT snr= %d\n"
    , (int)snr );
  }
if ( dbglev(SURPAC) == 1 )
  {
   for (i_s=1; i_s<= *p_no_points; i_s= i_s+1) /* Loop all solutions*/
     { 
     fprintf(dbgfil(SURPAC),
     "sur161 Solution %d uv_out %f %f\n"
    , (int)i_s , uv_out[i_s-1].x_gm, uv_out[i_s-1].y_gm );
     fprintf(dbgfil(SURPAC),
     "sur161 Solution %d r3_out %f %f %f\n"
    , (int)i_s , r3_out[i_s-1].x_gm, r3_out[i_s-1].y_gm, 
           r3_out[i_s-1].z_gm );
     }
  if ( *p_no_points > 0   )
  {
  fprintf(dbgfil(SURPAC),
  "sur161 Exit varkon_sur_intlinept  Output UV pt %f %f Total no %d\n"
    , p_cuvpt->x_gm , p_cuvpt->y_gm , (int)*p_no_points  );
  fflush(dbgfil(SURPAC));
  }
  else
  {
  fprintf(dbgfil(SURPAC),
  "sur161 Exit***varkon_sur_intlinept** No solution found\n");
  fflush(dbgfil(SURPAC));
  }
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
