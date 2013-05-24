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
/*  Function: varkon_pat_bound                     File: sur912.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the bounding box and the bounding       */
/*  direction cone for a surface topological patch.                 */
/*                                                                  */
/*  Input parameter values shall be given in the global             */
/*  topological U,V plane of the surface.                           */
/*                                                                  */
/*  The bounding boxes are used by intersection functions           */
/*  to determine if an intersect between objects exist.             */
/*  The bounding cones are used by surface (or curve) calculation   */
/*  functions to determine if a solution exists. One example is     */
/*  the silhouette calculation. The BCONE is used to find out       */
/*  if none of the surface normals in a patch will be perpen-       */
/*  dicular to the viewing direction (vector).                      */
/*                                                                  */
/*  The BBOX and BCONE will normally be computed for one (1)        */
/*  topological patch, i.e. the area defined by us_pat, vs_pat,     */
/*  ue_pat and ve_pat. This function may however also be called     */
/*  by an interrogation routine, which calculates a bounding        */
/*  box and cone for a (smaller) part of the patch.                 */
/*                                                                  */
/* TODO                                                             */
/*  ... Modify Debug test (sur211) for NURBS surfaces !!!!!         */
/*      sur311 shall be added !!!!!!!!!!!!!!!!!!!!!!!!              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-03   Originally written                                 */
/*  1995-09-02   Debug added                                        */
/*  1996-01-18   Missing end of comment, fflush                     */
/*  1996-02-03   Detection ov (Consurf) Null Tiles                  */
/*  1996-11-15   Compilation warnings                               */
/*  1997-03-20   Null tile --> NUL_PAT preparations (not impl.)     */
/*  1997-05-22   Debug                                              */
/*  1997-12-07   Comments about NURBS                               */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_bound      Boundary box and cone for patch  */
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
/* varkon_idpoint       * Identical points criterion                */
/* varkon_sur_eval      * Surface evaluation routine                */
/* varkon_normv         * Normalisation of a vector                 */
/* varkon_angd          * Vector angle in degrees                   */
/* varkon_erpush        * Error message to terminal                 */
/* varkon_sur_patadr    * Patch adress for a given U,V pt           */
/* varkon_pat_pritop    * Print topological patch data              */
/* varkon_ini_bbox      * Initialize BBOX   for Debug On            */
/* varkon_ini_bcone     * Initialize BCONE  for Debug On            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_pat_bound (sur912)  */
/* SU2993 = Severe program error in varkon_pat_bound (sur912).      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
             DBstatus varkon_pat_bound (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Topological patch data area       (ptr) */
   DBfloat  u_s,         /* U start value in topological U,V plane  */
   DBfloat  v_s,         /* V start value in topological U,V plane  */
   DBfloat  u_e,         /* U end   value in topological U,V plane  */
   DBfloat  v_e,         /* V end   value in topological U,V plane  */
   DBint    acc,         /* Calculation accuracy (cases):           */
                         /* Eq. 0: Standard BBOX and BCONE          */
                         /* Eq. 1: Only     BBOX calculation        */
   BBOX    *p_pbox,      /* Bounding box  for the patch       (ptr) */
   BCONE   *p_pcone )    /* Bounding cone for the patch       (ptr) */
/*                                                                  */
/* Out:                                                             */
/*         Data to p_pbox and p_pcone                               */
/*         Flags = -99 for a bad patch                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

   DBfloat  xmin;        /* Minimum x coordinate   for the patch    */
   DBfloat  ymin;        /* Minimum y coordinate   for the patch    */
   DBfloat  zmin;        /* Minimum z coordinate   for the patch    */
   DBfloat  xmax;        /* Maximum x coordinate   for the patch    */
   DBfloat  ymax;        /* Maximum y coordinate   for the patch    */
   DBfloat  zmax;        /* Maximum z coordinate   for the patch    */

   DBint    n_poi;       /* Total number of points                  */
   DBint    n_tot;       /* Total number of normals                 */

   DBfloat  xsum;        /* Sum of x components                     */
   DBfloat  ysum;        /* Sum of y components                     */
   DBfloat  zsum;        /* Sum of z components                     */
   DBfloat  ang;         /* Cone angle                              */

   DBint    ip_u;        /* Loop index corresp. to a U point        */
   DBint    ip_v;        /* Loop index corresp. to a V point        */
   DBint    np_u;        /* Number of points in direction U         */
   DBint    np_v;        /* Number of points in direction V         */
   DBfloat  deltau;      /* Delta in U direction                    */
   DBfloat  deltav;      /* Delta in V direction                    */
   DBfloat  u_pat;       /* Current U parameter value               */
   DBfloat  v_pat;       /* Current V parameter value               */
   DBint    i_norm;      /* Loop index corresp. to surface normal   */
   DBfloat  idcurve;     /* Identical curve criterion               */
/*                                                                  */
/*-----------------------------------------------------------------!*/






   DBVector snorm[100];  /* All surface normals                     */
   DBVector u0[10];      /* All U= u_s points                       */
   DBVector u1[10];      /* All U= u_e points                       */
   DBVector v0[10];      /* All V= v_s points                       */
   DBVector v1[10];      /* All V= v_e points                       */
   EVALS    xyz;         /* Coordinates and derivatives             */
   DBint    icase;       /* Calculation case for varkon_sur_eval    */
   DBint    f_eval;      /* Flag for evaluation failure             */
   DBint    patch_type;  /* Type of patch                           */
                         /* Eq. 1: Regular patch                    */
                         /* Eq. 2: Triangular patch                 */
                         /* Eq. 3: U boundaries equal (Null Tile)   */
                         /* Eq. 4: V boundaries equal (Null Tile)   */
                         /* Eq. 5: Patch is a point                 */
                         /* (Negative patch_type for undefined      */
                         /*  surface normals)                       */
   DBfloat  dir[3];      /* Average surface normal                  */
   DBfloat  comp[3];     /* Comparison vector from snorm            */
   DBfloat  cang;        /* Comparison angle between dir and comp   */
   DBfloat  b_dist;      /* Distance between boundary curves        */
   DBfloat  u_min;       /* Minimum distance between U boundaries   */
   DBfloat  u_max;       /* Maximum distance between U boundaries   */
   DBfloat  v_min;       /* Minimum distance between V boundaries   */
   DBfloat  v_max;       /* Maximum distance between V boundaries   */
   DBint    i_p;         /* Loop index boundary point               */
   DBfloat  idpoint;     /* Identical point criterion               */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

   DBPatch *p_pat_n;     /* Badly defined patch               (ptr) */
   DBint    iu_n;        /* Patch address    IU  for NUL_PAT        */
   DBint    iv_n;        /*                  IV  for NUL_PAT        */
   DBint    nu,nv;       /* Number of patches in U and V direction  */
   DBfloat  u_local;     /* Patch (local) U parameter value         */
   DBfloat  v_local;     /* Patch (local) V parameter value         */

#ifdef DEBUG
   DBint    iu;          /* Patch address    IU                     */
   DBint    iv;          /*                  IV                     */
   DBint    iu_s;        /* Patch address    IU for first point     */
   DBint    iv_s;        /*                  IV for first point     */
   DBint    aiu;         /* Adress to patch U. Eq. -1: All patches  */
   DBint    aiv;         /* Adress to patch V.                      */
#endif

/*!New-Page--------------------------------------------------------!*/






/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                __________________                                */
/*               !                  !                               */
/*               ! varkon_pat_bound !                               */
/*               !    (sur912)      !                               */
/*               !__________________!                               */
/*         _______________!___________________________________      */
/*   _____!_______  ___________!____________  _____!______  __!___  */
/*  !             !!                        !!            !!      ! */
/*  !     1       !!           2            !!     3      !!  4   ! */
/*  ! Checks and  !!  Min. and max. values  !! Cone angle !! Exit ! */
/*  ! initiations !! Average surface normal !!            !!      ! */
/*  !_____________!!________________________!!____________!!______! */
/*    ____!____________        !                 __!________        */
/*   !                 !       !           _____!______  ___!___    */
/*   !varkon_sur_patadr!       !          !          * !!       !   */
/*   !  (sur211)       !       !          ! All points !!varkon_!   */
/*   ! For Debug On    !       !          !____________!! angd  !   */
/*   !_________________!       !                        !_______!   */
/*       ______________________!_____________                       */
/*  ____!_____   __________!__________    ___!___                   */
/* !          ! !                   * !  !       !                  */
/* ! Initiate ! ! U values for points !  !varkon_!                  */
/* !   loop   ! !_____________________!  ! normv !                  */
/* ! variabl. !  __________!__________   !_______!                  */
/* !__________! !                   * !                             */
/*              ! V values for points !                             */
/*              !_____________________!                             */
/*               __________!__________                              */
/*              !                     !                             */
/*              !  varkon_sur_eval    !                             */
/*              !      (sur210)       !                             */
/*              !_____________________!                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur912 Enter* Bounding box & cone for patch    acc %d\n",(int)acc);
}
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912  u_s %6.4f v_s %6.4f u_e %6.4f v_e %6.4f\n",
       u_s ,v_s ,u_e ,v_e );
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* Number of points in U and V direction:                           */
   np_u = 4;
   np_v = 4;

/* Parameter steps for the points:                                  */
   deltau= (u_e-u_s)/((DBfloat)np_u-1.0);
   deltav= (v_e-v_s)/((DBfloat)np_v-1.0);
/*                                                                 !*/

#ifdef  DEBUG
/* It is assumed that data is initialized in the surface creation   */
/* Can be confusing if flag is changed from -1 to I_UNDEF           */
varkon_ini_bbox (p_pbox);
varkon_ini_bcone(p_pcone);
#endif

/* The number of points in U and V direction can be changed but     */
/* check that arrays snorm, u0, u1, v0 and v1 are properly          */
/* dimensioned.                                                     */
   
for ( i_p=1; i_p<= 10  ; ++i_p )         /* Start loop   boundaries */
  {
  u0[i_p-1].x_gm = F_UNDEF;
  u0[i_p-1].y_gm = F_UNDEF;
  u0[i_p-1].z_gm = F_UNDEF;
  u1[i_p-1].x_gm = F_UNDEF;
  u1[i_p-1].y_gm = F_UNDEF;
  u1[i_p-1].z_gm = F_UNDEF;
  v0[i_p-1].x_gm = F_UNDEF;
  v0[i_p-1].y_gm = F_UNDEF;
  v0[i_p-1].z_gm = F_UNDEF;
  v1[i_p-1].x_gm = F_UNDEF;
  v1[i_p-1].y_gm = F_UNDEF;
  v1[i_p-1].z_gm = F_UNDEF;
  }

/*  Case for varkon_sur_eval (sur210). Only coordinates for acc= 0  */
    if ( acc == 0 )
      {
      icase = 3;
      f_eval = 1;  
      }
    else
      {
      icase = 0;
      f_eval = 0;  
      }

    patch_type = I_UNDEF;

/*!                                                                 */
/* For Debug On:                                                    */
/* Check that u_s, v_s, u_e, v_e is in the same patch.              */
/* Calls of varkon_sur_patadr (sur211).                             */
/*                                                                 !*/

#ifdef DEBUG
/* The test is not applicable for a NURBS surface */
if ( p_sur->typ_su == NURB_SUR ) goto not_appl;
nu = p_sur->nu_su; 
nv = p_sur->nv_su;
status=varkon_sur_patadr
  (u_s  ,v_s  ,nu,nv,&iu_s,&iv_s,&u_local,&v_local);
if (status<0) 
  {
  sprintf(errbuf,"varkon_sur_patadr%%varkon_pat_bound (sur912)");
  return(varkon_erpush("SU2943",errbuf));
  }

status=varkon_sur_patadr
   (u_e  ,v_s  ,nu,nv,&iu,&iv,&u_local,&v_local);
if (status<0) 
  {
  sprintf(errbuf,"varkon_sur_patadr%%varkon_pat_bound (sur912)");
  return(varkon_erpush("SU2943",errbuf));
  }

if (iu_s != iu )
   {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e %25.20f v_s %25.20f\n", u_e ,v_s );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e iu_s= %d not equal iu= %d\n", (int)iu_s , (int)iu );
  fflush(dbgfil(SURPAC)); 
  }
    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if (iv_s != iv )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e %25.20f v_s %25.20f\n", u_e ,v_s );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e iv_s= %d not equal iv= %d\n", (int)iv_s , (int)iv );
  fflush(dbgfil(SURPAC)); 
  }

    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

status=varkon_sur_patadr
   (u_s  ,v_e  ,nu,nv,&iu,&iv,&u_local,&v_local);
if (status<0) 
  {
  sprintf(errbuf,"varkon_sur_patadr%%varkon_pat_bound (sur912)");
  return(varkon_erpush("SU2943",errbuf));
  }

  if (iu_s != iu )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_s %25.20f v_e %25.20f\n", u_s ,v_e );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_s iu_s= %d not equal iu= %d\n", (int)iu_s , (int)iu );
  fflush(dbgfil(SURPAC)); 
  }

    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if (iv_s != iv )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_s %25.20f v_e %25.20f\n", u_s ,v_e );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_s iv_s= %d not equal iv= %d\n", (int)iv_s , (int)iv );
  fflush(dbgfil(SURPAC));
  }

    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

status=varkon_sur_patadr
 (u_e  ,v_e  ,nu,nv,&iu,&iv,&u_local,&v_local);
if (status<0) 
  {
  sprintf(errbuf,"varkon_sur_patadr%%varkon_pat_bound (sur912)");
  return(varkon_erpush("SU2943",errbuf));
  }

  if (iu_s != iu )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e %25.20f v_e %25.20f\n", u_e ,v_e );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e iu_s= %d not equal iu= %d\n", (int)iu_s , (int)iu );
  fflush(dbgfil(SURPAC));
  }

    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
       return(varkon_erpush("SU2993",errbuf));
    }

  if (iv_s != iv )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e %25.20f v_e %25.20f\n", u_e ,v_e );
  fprintf(dbgfil(SURPAC),
  "sur912 Error u_e iv_s= %d not equal iv= %d\n", (int)iv_s , (int)iv );
  fflush(dbgfil(SURPAC)); 
  }

    sprintf(errbuf,"(not same patch)%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

not_appl:; /* Test is not applicable for a Nurbs surface */
#endif

/*!                                                                 */
/* For the classification of patches: idcurve= 10*idpoint           */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

   idpoint   =varkon_idpoint();

   idcurve = 50.0*idpoint;

/* Debug printout of input data                                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912  deltau %f deltav %f, idcurve %8.4f\n", 
                 deltau,deltav, idcurve);
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 2. Calculate min and max coordinates and average normal          */
/* _______________________________________________________          */
/*                                                                  */
/* Initiate loop variables xsum, ysum, .. xmax, ... ang             */
/*                                                                 !*/

   n_poi = 0;      /* Initiate number of calculated points          */
   n_tot = 0;      /* Initiate number of calculated normals         */

   xsum  = 0.0;    /* Initiate for average calculation              */
   ysum  = 0.0; 
   zsum  = 0.0;
   xmax  =  -50000.; /* Initiate coordinate extreme values          */
   ymax  =  -50000.;
   zmax  =  -50000.;
   xmin  =   50000.;
   ymin  =   50000.;
   zmin  =   50000.;

   ang   = 0.0;

/*!                                                                 */
/* Start loop points in the patch ip_u=1,2,..np_u ip_v=1,2,..np_v   */
/*                                                                 !*/

for ( ip_u=1; ip_u<= np_u; ++ip_u )      /* Start loop ip_u         */
  {
  for ( ip_v=1; ip_v<= np_v; ++ip_v )    /* Start loop ip_v         */
    {

/*!                                                                 */
/*                                                                 !*/

/*!                                                                 */
/*   Current parameter point:                                       */
     u_pat= u_s + deltau*(ip_u-1);
     v_pat= v_s + deltav*(ip_v-1);
/*   Call of varkon_sur_eval (sur210).                              */
/*                                                                 !*/

     n_poi= n_poi + 1;     /* One additional point                  */

   status=varkon_sur_eval
   (p_sur,p_pat,icase,u_pat,v_pat,&xyz);
   if (status < 0 && f_eval == 0 )  /* Failure for icase= 0  */
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912  sur210 failed   U= %f V= %f status= %d\n",
       u_pat,v_pat, (int)status);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 P= %10.2f %10.2f %10.2f n= %6.4f %6.4f %6.4f\n",
       xyz.r_x,xyz.r_y,xyz.r_z,xyz.n_x,xyz.n_y,xyz.n_z);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 drdu= %f %f %f \n", xyz.u_x,xyz.u_y,xyz.u_z);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 drdv= %f %f %f \n", xyz.v_x,xyz.v_y,xyz.v_z);
  fflush(dbgfil(SURPAC)); 
  }
#endif

        p_pbox->flag  = -99;
        p_pcone->flag = -99;      
        sprintf(errbuf,"sur210%%sur912");
       return(varkon_erpush("SU2943",errbuf));
        }


   else if (status < 0 && f_eval == 1 )  /* Failure for icase= 3  */
     {
     f_eval = -1;  /* Flag: Surface normal calculation failed */
     status=varkon_sur_eval
     (p_sur,p_pat,(DBint)0,u_pat,v_pat,&xyz);
     if (status < 0  )  /* Failure also for icase= 0  */
       {
       p_pbox->flag  = -99;
       p_pcone->flag = -99;      
       sprintf(errbuf,"varkon_sur_eval(2)%%varkon_pat_bound (sur912)");
       return(varkon_erpush("SU2943",errbuf));
       }
     }     /* End surface normal icase= 3 failed  */





#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912  U= %f V= %f ip_u= %d ip_v %d\n",
       u_pat,v_pat, (int)ip_u, (int)ip_v);
  }
#endif

   if ( ip_u == 1     )               /* A U= u_s point             */
     {
     u0[ip_v-1].x_gm = xyz.r_x;  
     u0[ip_v-1].y_gm = xyz.r_y;
     u0[ip_v-1].z_gm = xyz.r_z;
     }
   if ( ip_u == np_u  )               /* A U= u_e point             */
     {
     u1[ip_v-1].x_gm = xyz.r_x;  
     u1[ip_v-1].y_gm = xyz.r_y;
     u1[ip_v-1].z_gm = xyz.r_z;
     }
   if ( ip_v == 1     )               /* A V= v_s point             */
     {
     v0[ip_u-1].x_gm = xyz.r_x;  
     v0[ip_u-1].y_gm = xyz.r_y;
     v0[ip_u-1].z_gm = xyz.r_z;
     }
   if ( ip_v == np_v  )               /* A V= v_e point             */
     {
     v1[ip_u-1].x_gm = xyz.r_x;  
     v1[ip_u-1].y_gm = xyz.r_y;
     v1[ip_u-1].z_gm = xyz.r_z;
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912  ip_u %d ip_v %d u_pat %f v_pat %f \n",
       (int)ip_u , (int)ip_v , u_pat,v_pat );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 P= %10.2f %10.2f %10.2f n= %6.4f %6.4f %6.4f\n",
       xyz.r_x,xyz.r_y,xyz.r_z,xyz.n_x,xyz.n_y,xyz.n_z);
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/*   Compare current maximum and minimum coordinates                */
/*   Add surface normal components to xsum, ysum and zsum.          */
/*                                                                 !*/
     if ( xyz.r_x > xmax )
       {
       xmax = xyz.r_x;
       }
     if ( xyz.r_y > ymax )
       {
       ymax = xyz.r_y;
       }
     if ( xyz.r_z > zmax )
       {
       zmax = xyz.r_z;
       }

     if ( xyz.r_x < xmin )
       {
       xmin = xyz.r_x;
       }
     if ( xyz.r_y < ymin )
       {
       ymin = xyz.r_y;
       }
     if ( xyz.r_z < zmin )
       {
       zmin = xyz.r_z;
       }

     if ( acc == 0 && f_eval > 0 )
     {
     n_tot= n_tot + 1;     /* One additional normal                 */
#ifdef DEBUG
     if ( n_tot > 99 )
       {
       sprintf(errbuf,"n_tot>99%%varkon_pat_bound (sur912)");
       return(varkon_erpush("SU2993",errbuf));
       }
#endif

     xsum = xsum + xyz.n_x;
     ysum = ysum + xyz.n_y;
     zsum = zsum + xyz.n_z;

     snorm[n_tot-1].x_gm = xyz.n_x;
     snorm[n_tot-1].y_gm = xyz.n_y;
     snorm[n_tot-1].z_gm = xyz.n_z;
     }

    }                                    /* End   loop ip_v         */
  }                                      /* End   loop ip_u         */

/*!                                                                 */
/* End  points in the patch ip_u=1,2,..np_u ip_v=1,2,..np_v         */
/*                                                                 !*/

/*!                                                                 */
/* Analysation of distances between boundary curves                 */
/* (No closest point or intersection implemented for                */
/*  the moment. It is assumed that parameters can be used ... )     */
/*                                                                 !*/

   u_min =  50000.0;     /* Minimum distance between U boundaries   */
   u_max = -50000.0;     /* Maximum distance between U boundaries   */
   v_min =  50000.0;     /* Minimum distance between V boundaries   */
   v_max = -50000.0;     /* Maximum distance between V boundaries   */

for ( i_p=1; i_p<= np_u; ++i_p )         /* Start loop U boundaries */
  {
  b_dist =  SQRT(  
         (u0[i_p-1].x_gm - u1[i_p-1].x_gm)*
         (u0[i_p-1].x_gm - u1[i_p-1].x_gm)  +
         (u0[i_p-1].y_gm - u1[i_p-1].y_gm)*
         (u0[i_p-1].y_gm - u1[i_p-1].y_gm)  +
         (u0[i_p-1].z_gm - u1[i_p-1].z_gm)*
         (u0[i_p-1].z_gm - u1[i_p-1].z_gm)   );
   if ( b_dist < u_min ) u_min = b_dist;
   if ( b_dist > u_max ) u_max = b_dist;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 u0 %8.1f %8.1f %8.1f u1 %8.1f %8.1f %8.1f \n",
       u0[i_p-1].x_gm,  u0[i_p-1].y_gm, u0[i_p-1].z_gm,
       u1[i_p-1].x_gm,  u1[i_p-1].y_gm, u1[i_p-1].z_gm);
  fprintf(dbgfil(SURPAC),
  "sur912 b_dist %25.10f u_min %f u_max %f\n",
       b_dist, u_min, u_max  );
  }
#endif
  }       /* End loop point in U boundary curves */


for ( i_p=1; i_p<= np_u; ++i_p )         /* Start loop V boundaries */
  {
  b_dist =  SQRT(  
         (v0[i_p-1].x_gm - v1[i_p-1].x_gm)*
         (v0[i_p-1].x_gm - v1[i_p-1].x_gm)  +
         (v0[i_p-1].y_gm - v1[i_p-1].y_gm)*
         (v0[i_p-1].y_gm - v1[i_p-1].y_gm)  +
         (v0[i_p-1].z_gm - v1[i_p-1].z_gm)*
         (v0[i_p-1].z_gm - v1[i_p-1].z_gm)   );
   if ( b_dist < v_min ) v_min = b_dist;
   if ( b_dist > v_max ) v_max = b_dist;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 v0 %8.1f %8.1f %8.1f v1 %8.1f %8.1f %8.1f \n",
       v0[i_p-1].x_gm,  v0[i_p-1].y_gm, v0[i_p-1].z_gm,
       v1[i_p-1].x_gm,  v1[i_p-1].y_gm, v1[i_p-1].z_gm);
  fprintf(dbgfil(SURPAC),
  "sur912 b_dist %25.10f v_min %f v_max %f\n",
       b_dist, v_min, v_max  );
  }
#endif
  }       /* End loop point in V boundary curves */


/* Regular patch with well defined surface normals                  */
  if      ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&
              f_eval == 1                          )
    {
    patch_type =  1;
    }
/* Regular patch    with an undefined normal                        */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&
              f_eval < 0                           )
    {
    patch_type = -1;
    }
/* Regular patch  (surface normal not calculated)                   */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&
              f_eval == 0                          )
    {
    patch_type =  1;
    }

/* Patch is a point with an undefined normal                        */
  else if ( u_min < idcurve && u_max < idcurve &&
            v_min < idcurve && v_max < idcurve &&
              f_eval < 0                           )
    {
    patch_type = -5;
    }
/* Patch is a point with a well defined normal                      */
  else if ( u_min < idcurve && u_max < idcurve &&
            v_min < idcurve && v_max < idcurve &&
              f_eval == 1                          )
    {
    patch_type =  5;
    }
/* Patch is a point (surface normal not calculated)                 */
  else if ( u_min < idcurve && u_max < idcurve &&
            v_min < idcurve && v_max < idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  5;
    }
/* Null tile with equal U curves and undefined normal               */
  else if ( u_min <  idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -3;
    }
/* Null tile with equal U curves and well defined normals           */
  else if ( u_min <  idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  3;
    }
/* Null tile with equal U curves (surface normals not calculated)   */
  else if ( u_min <  idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  3;
    }

/* Null tile with equal V curves and undefined normal               */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max <  idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -4;
    }
/* Null tile with equal V curves and well defined normals           */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max <  idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  4;
    }
/* Null tile with equal V curves (surface normals not calculated)   */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max <  idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  4;
    }


/* Triangular patch                                                 */
  else if ( u_min <  idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max >= idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max <  idcurve &&  
              f_eval == 1                          )
    {
    patch_type =  2;
    }



/* Triangular patch                                                 */
  else if ( u_min <  idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max >= idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max <  idcurve &&  
              f_eval <  0                          )
    {
    patch_type = -2;
    }

/* Triangular patch                                                 */
  else if ( u_min <  idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max <  idcurve &&
            v_min >= idcurve && v_max >= idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min <  idcurve && v_max >= idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  2;
    }
/* Triangular patch                                                 */
  else if ( u_min >= idcurve && u_max >= idcurve &&
            v_min >= idcurve && v_max <  idcurve &&  
              f_eval == 0                          )
    {
    patch_type =  2;
    }
  else
    {
    sprintf(errbuf,"switch patch_type%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2993",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && patch_type != 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 u_min %f u_max %f v_min %f v_max %f   patch_type= %d\n",
        u_min,  u_max,  v_min, v_max, (int)patch_type );
  }
#endif

                         /* Eq. 1: Regular patch                    */
                         /* Eq. 2: Triangular patch                 */
                         /* Eq. 3: U boundaries equal (Null Tile)   */
                         /* Eq. 4: V boundaries equal (Null Tile)   */
                         /* Eq. 5: Patch is a point                 */
                         /* (Negative patch_type for undefined      */
                         /*  surface normals)                       */

/*!                                                                 */
/* Change patch type to NUL_PAT for "badly" defined patch           */
/*                                                                 !*/
/* Added 1997-03-20                                                 */


  if   (  patch_type == -2  || 
          patch_type == -3  || 
          patch_type == -4  || 
          patch_type ==  5     )
    {
    nu = p_sur->nu_su; 
    nv = p_sur->nv_su;
    status=varkon_sur_patadr
   (u_s  ,v_s  ,nu,nv,&iu_n,&iv_n,&u_local,&v_local);
   if (status<0) 
        {
        sprintf(errbuf,"sur211%%sur912");
       return(varkon_erpush("SU2943",errbuf));
        }
    p_pat_n = p_pat + (iu_n-1)*nv + (iv_n-1); /* Ptr to  patch   */
/* To be implemented later !!!    p_pat_n->styp_pat = NUL_PAT;   */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur912  u_s %6.4f v_s %6.4f f_eval %d patch_type %d ..->styp_pat %d\n",
     u_s ,v_s , (int)f_eval, (int)patch_type,  (int)p_pat_n->styp_pat );
fprintf(dbgfil(SURPAC),
"sur912  Change of patch type to NUL_PAT not yet implemented !!!\n");
fflush(dbgfil(SURPAC));
}
#endif

    }



/*!                                                                 */
/* Calculate average surface normal for acc= 0                      */
/* Normalise. Call of varkon_normv (normv).                         */
/*                                                                 !*/

     if ( n_tot <  0 && acc == 0 && f_eval == 1 )
       {
       sprintf(errbuf,"n_tot= 0%%varkon_pat_bound (sur912)");
       return(varkon_erpush("SU2993",errbuf));
       }


   if ( acc == 0 && f_eval == 1 )
   {
   dir[0] = xsum/(DBfloat)n_tot;
   dir[1] = ysum/(DBfloat)n_tot;
   dir[2] = zsum/(DBfloat)n_tot;

   status=varkon_normv(dir);
#ifdef DEBUG
   if (status<0) 
     {
     sprintf(errbuf,"normv%%varkon_pat_bound (sur912)");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif
  }       /* End acc= 0  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
       xmin,ymin,zmin,xmax,ymax,zmax );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 f_eval %d  n_tot %d\n",
       (int)f_eval, (int)n_tot );
  }
#endif


/*!                                                                 */
/* 3. Calculate the cone angle for acc= 0                           */
/* ______________________________________                           */
/*                                                                 !*/

   if ( acc == 0 ) 
   {
/*!                                                                 */
/* Loop surface normals in snorm i_norm=1,2,3,.. n_tot              */
/*                                                                 !*/

for ( i_norm=1; i_norm<= n_tot; ++i_norm ) /* Start loop i_norm     */
  {

/*!                                                                 */
/*   Calculate angle between surface normal and average normal.     */
/*   Call of varkon_angd (angd).                                    */
/*   Compare current angle with output (so far) max angle.          */
/*                                                                 !*/

     
     comp[0] = snorm[i_norm-1].x_gm;
     comp[1] = snorm[i_norm-1].y_gm;
     comp[2] = snorm[i_norm-1].z_gm;

     status=varkon_angd(dir,comp,&cang);
#ifdef DEBUG
   if (status<0) 
     {
     sprintf(errbuf,"angd%%varkon_pat_bound (sur912)");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

     if ( fabs(cang) > ang )
       {
       ang = fabs(cang);
       }


  }                                      /* End   loop i_norm       */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Average normal %10.6f  %10.6f  %10.6f  ang %15.8f\n",
       dir[0],dir[1],dir[2],ang );
  }
#endif

  }  /* End acc= 0      */

/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Data to output parameters p_pbox and p_pcone                     */

/*                                                                 !*/

   p_pbox->xmin = xmin;
   p_pbox->ymin = ymin;
   p_pbox->zmin = zmin;
   p_pbox->xmax = xmax;
   p_pbox->ymax = ymax;
   p_pbox->zmax = zmax;

  if      ( patch_type > 0 ) p_pbox->flag =  (short)patch_type;      
  else if ( patch_type < 0 ) p_pbox->flag = -(short)patch_type;      
  else                       p_pbox->flag =  I_UNDEF;          

   if ( acc  ==  0  )
   {
   p_pcone->xdir = dir[0];
   p_pcone->ydir = dir[1];
   p_pcone->zdir = dir[2];
   p_pcone->ang  = ang;    
   }
   else
   {
   p_pcone->xdir = 0.0;    
   p_pcone->ydir = 0.0;    
   p_pcone->zdir = 0.0;    
   p_pcone->ang  = 0.0;    
   }

  if      ( acc == 1 )                   p_pcone->flag =  -1;      
  else if ( acc == 0 && patch_type > 0 ) p_pcone->flag =   1;      
  else if ( acc == 0 && patch_type < 0 ) p_pcone->flag =  -99;      
  else                                   p_pcone->flag =  I_UNDEF;          


/*!                                                                 */
/* For Debug On:     Printout of topological patch data.            */
/*                   Call of varkon_pat_pritop  (sur232).           */
/*                                                                 !*/

#ifdef DEBUG
  nu = p_sur->nu_su; 
  nv = p_sur->nv_su;
  status=varkon_sur_patadr
   (u_s  ,v_s  ,nu,nv,&iu,&iv,&u_local,&v_local);
  if (status<0) 
    {
    sprintf(errbuf,"varkon_sur_patadr%%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2943",errbuf));
    }

  aiu = iu;
  aiv = iv;
  status = varkon_pat_pritop 
  (p_sur,p_pat,aiu,aiv);
  if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pritop %%varkon_pat_bound (sur912)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 xdir %7.4f ydir %7.4f zdir %7.4f ang %f flag %d\n",
    p_pcone->xdir, p_pcone->ydir, p_pcone->zdir, p_pcone->ang,p_pcone->flag  );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 p_pbox->flag %d p_pcone->flag %d\n",
          p_pbox->flag,   p_pcone->flag  );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur912 Exit *** varkon_pat_bound **\n");
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/
