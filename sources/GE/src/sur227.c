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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_pat_rotloft                   File: sur227.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a conic lofting patch,       */
/*  which is a rotation surface.                                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-12   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_rotloft    Rotation lofting evaluation fctn */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*                                                                  */
/*         Evaluation of coordinates and derivatives                */
/*             for a surface of revolution                          */
/*                                                                  */
/*                                                                  */
/* Reference: Faux & Pratt p 92                                     */
/*                                                                  */
/* Input data to (used data in) this functions is:                  */
/* - The spine curve, assumed to be a straight line rotation axis   */
/* - The start limit curve, assumed to be a plane rotation curve    */
/* - The end   limit curve, assumed to be a plane rotation curve    */
/* - A parameter point urot,phi, where urot is the parameter for    */
/*   the start and end limit curves, which are equal except for     */
/*   a rotation transformation. The parameter phi is a relative     */
/*   angle between the start and end limit curve positions.         */
/*                                                                  */
/* The input limit curves (two rational segments) are transformed   */
/* to a local coordinate system. This local system is defined by    */
/* the spine and the mid point on the start limit curve. The        */
/* plane rotation curve (transformed start limit curve) will be     */
/* in the X/Y plane of the local coordinate system.                 */
/*                                                                  */
/* The surface of revolution is in the local system defined by      */
/* the rotation axis, which is the X axis and the plane rotation    */
/* curve:                                                           */
/*                                                                  */
/*              Xrot(urot) = Third degree rational polynomial       */
/* Rrot(urot) = Yrot(urot) = Third degree rational polynomial       */
/*              Zrot(urot) =  0.0                                   */
/*                                                                  */
/*                                                                  */
/* The surface of revolution has the equation:                      */
/*                                                                  */
/*                                                                  */
/*                  = X(urot,phi)   = Xrot(urot)                    */
/* Rsur(urot,phi)   = Y(urot,phi)   = Yrot(urot)*cos(phi)           */
/*                  = Z(urot,phi)   = Yrot(urot)*sin(phi)           */
/*                                                                  */
/*                    xyz_s.r_x     = xyz_l.r.x_gm;                 */
/* Program code:      xyz_s.r_y     = xyz_l.r.y_gm*COS(phi);        */
/*                    xyz_s.r_z     = xyz_l.r.y_gm*SIN(phi);        */
/*                                                                  */
/* The U parameter for the surface of revolution is:                */
/* urot = u_pat            where                                    */
/* u_pat is the input U parameter ( 0 <= u_pat <= 1).               */
/*                                                                  */
/* The angle parameter phi is calculated as:                        */
/* phi = v_pat*tot_phi     where                                    */
/* tot_phi is the rotation angle between the start and end limit    */
/* curves and v_pat is the input V parameter value (0 <= v_pat <= 1)*/
/*                                                                  */
/* Differentiation with respect to urot and phi:                    */
/*                                                                  */
/*                                                                  */
/*                  = dX/durot      =  dXrot/durot                  */
/* dRsur/durot      = dY/durot      =  dYrot/durot*cos(phi)         */
/*                  = dZ/durot      =  dYrot/durot*sin(phi)         */
/*                                                                  */
/*                    xyz_s.u_x     =  xyz_l.drdt.x_gm;             */
/* Program code:      xyz_s.u_y     =  xyz_l.drdt.y_gm*COS(phi);    */
/*                    xyz_s.u_z     =  xyz_l.drdt.y_gm*SIN(phi);    */
/*                                                                  */
/*                                                                  */
/*                  = dX/dphi       =  0                            */
/* dRsur/dphi       = dY/dphi       = -Yrot(urot)*sin(phi)          */
/*                  = dZ/dphi       =  Yrot(urot)*cos(phi)          */
/*                                                                  */
/*                    xyz_s.v_x     =   0.0;                        */
/* Program code:      xyz_s.v_y     = -xyz_l.r.y_gm*SIN(phi);       */
/*                    xyz_s.v_z     =  xyz_l.r.y_gm*COS(phi);       */
/*                                                                  */
/*                                                                  */
/*                  = d2X/durot2    =  d2Xrot/durot2                */
/* d2Rsur/durot2    = d2Y/durot2    =  d2Yrot/durot2*cos(phi)       */
/*                  = d2Z/durot2    =  d2Yrot/durot2*sin(phi)       */
/*                                                                  */
/*                    xyz_s.u2_x    =  xyz_l.d2rdt2.x_gm;           */
/* Program code:      xyz_s.u2_y    =  xyz_l.d2rdt2.y_gm*COS(phi);  */
/*                    xyz_s.u2_z    =  xyz_l.d2rdt2.y_gm*SIN(phi);  */
/*                                                                  */
/*                  = d2X/dphi2     =  0                            */
/* d2Rsur/dphi2     = d2Y/dphi2     = -Yrot(urot)*cos(phi)          */
/*                  = d2Z/dphi2     = -Yrot(urot)*sin(phi)          */
/*                                                                  */
/*                    xyz_s.v2_x    =   0.0;                        */
/* Program code:      xyz_s.v2_y    = -xyz_l.r.y_gm*COS(phi);       */
/*                    xyz_s.v2_z    = -xyz_l.r.y_gm*SIN(phi);       */
/*                                                                  */
/*                  = d2X/durotdphi =  0                            */
/* d2Rsur/durotdphi = d2Y/durotdphi = -dYrot/durot*sin(phi)         */
/*                  = d2Z/durotdphi =  dYrot/durot*cos(phi)         */
/*                                                                  */
/*                    xyz_s.uv_x    =   0.0;                        */
/* Program code:      xyz_s.uv_y    = -xyz_l.drdt.y_gm*SIN(phi);    */
/*                    xyz_s.uv_z    =  xyz_l.drdt.y_gm*COS(phi);    */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* Derivatives with respect to v_pat:                               */
/*                                                                  */
/* The derivatives above is for urot= u_pat and phi= v_pat*phi_tot. */
/* Output shall be derivatives with respect to u_pat and v_pat.     */
/* Derivatives for phi must be multiplied with phi_tot (chain rule) */
/*                                                                  */
/*                xyz_s.v_x         = xyz_s.v_x*tot_phi;            */
/*                xyz_s.v_y         = xyz_s.v_y*tot_phi;            */
/*                xyz_s.v_z         = xyz_s.v_z*tot_phi;            */
/*                xyz_s.v2_x        = xyz_s.v2_x*tot_phi*tot_phi;   */
/* Program code:  xyz_s.v2_y        = xyz_s.v2_y*tot_phi*tot_phi;   */
/*                xyz_s.v2_z        = xyz_s.v2_z*tot_phi*tot_phi;   */
/*                xyz_s.uv_x        = xyz_s.uv_x*tot_phi;           */
/*                xyz_s.uv_y        = xyz_s.uv_y*tot_phi;           */
/*                xyz_s.uv_z        = xyz_s.uv_z*tot_phi;           */
/*                                                                  */
/* Transformation of coordinates and derivatives to global system:  */
/*                                                                  */
/* The point and derivative vectors are transformed to the          */
/* global system .....                                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* Remaining problems:                                              */
/* Derivative cannot be calculated for Yrot(urot)= 0                */
/* Curvature is not OK in Yrot(urot)= 0                             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short r_csys();        /* Create rotation coordinate system  */
static short cderiv();        /* Calculate curve   derivatives      */
static short vangle();        /* Calculate angle for input V        */
static short sderiv();        /* Calculate surface derivatives      */

/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBTmat   rot_csys;         /* Rotation system                */
static DBTmat   rot_csys_inv;     /* Rotation system, inverted      */
static DBSeg    lims_t;           /* Transformed start limit segm.  */
static EVALC    xyz_l;            /* Local curve   derivatives strt */
static EVALC    xyz_le;           /* Local curve   derivatives end  */
static DBVector tan_spine;        /* Current spine tangent          */
static DBVector poi_spine;        /* Current spine point            */
static EVALS    xyz_s;            /* Local surface derivatives      */
static DBVector poi_lime;         /* Mid point on end limit curve   */
static DBfloat  tot_phi;          /* Total angle= rotation direct.  */
static DBfloat  v_phi;            /* Angle corresponding to input V */
static DBfloat  ctol;             /* Coordinate tolerance           */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol            * Coordinate tolerance                    */
/* GE109                  * Curve evaluation routine                */
/* GEmktf_3p              * Create local coord. system              */
/* GEtfseg_to_local       * Transform rational segment              */
/* GEtfpos_to_basic       * Transformation of point                 */
/* GEtform_inv            * Invertation of a matrix                 */
/* varkon_evals_transf    * Transformation of EVALS                 */
/* varkon_erinit          * Initial. of error messages              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_rotloft   */
/* SU2993 = Severe program error in varkon_pat_rotloft (sur227).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_pat_rotloft (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl,       /* Pointer to the conic lofting patch      */
   DBint   icase,        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat u_pat,        /* Patch (local) U parameter value         */
   DBfloat v_pat,        /* Patch (local) V parameter value         */

   EVALS  *p_xyz )       /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   EVALS  xyz_tra;       /* Coordinates and derivatives transformed */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
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
"sur227 Mid curve flag p_patl->p_flag %d\n", (int)p_patl->p_flag);
  fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_patl,u_pat,v_pat,p_xyz); 
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/*  Retrieve coordinate tolerance. Call of varkon_ctol (sur751)     */
/*                                                                 !*/

   ctol      = varkon_ctol();

/*!                                                                 */
/* 2. Calculate rotation system                                     */
/* ____________________________                                     */
/*                                                                  */
/* Call of r_csys.                                                  */
/*                                                                 !*/
   status= r_csys (p_patl); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur227 r_csys failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"r_csys%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2973",errbuf));
   }



/*!                                                                 */
/* 2. Calculate curve derivatives in local system                   */
/* ______________________________________________                   */
/*                                                                  */
/* Call of cderiv.                                                  */
/*                                                                 !*/
   status= cderiv (p_patl, u_pat); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur227 cderiv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"cderiv%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 3. Calculate angle corresponding to input V value                */
/* _________________________________________________                */
/*                                                                  */
/* Call of vangle.                                                  */
/*                                                                 !*/
   status= vangle ( v_pat); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur227 vangle failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"vangle%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 4. Calculate surface derivatives in local system                 */
/* ________________________________________________                 */
/*                                                                  */
/* Call of sderiv.                                                  */
/*                                                                 !*/
   status= sderiv (); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur227 sderiv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"sderiv%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 5. Transformation of derivatives                                 */
/* ________________________________                                 */
/*                                                                  */
/* Invertation of matrix and transformation of EVALS.               */
/* Call of varkon_geo_612 and varkon_evals_transf (sur640).         */
/*                                                                 !*/


status=GEtform_inv (&rot_csys, &rot_csys_inv );
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "GEtform_inv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"GEtform_inv%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2943",errbuf));
   }

     xyz_s.e_case = 4;

status=varkon_evals_transf
     (&xyz_s, &rot_csys_inv, &xyz_tra );
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "GEtform_inv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"GEtform_inv%%varkon_pat_rotloft (sur227)");
   return(varkon_erpush("SU2943",errbuf));
   }

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= xyz_tra.r_x;
    p_xyz->r_y= xyz_tra.r_y;
    p_xyz->r_z= xyz_tra.r_z;


/*  Tangent             dr/du                                       */
    p_xyz->u_x=  xyz_tra.u_x;
    p_xyz->u_y=  xyz_tra.u_y;
    p_xyz->u_z=  xyz_tra.u_z;

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= xyz_tra.v_x;
    p_xyz->v_y= xyz_tra.v_y;
    p_xyz->v_z= xyz_tra.v_z;

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= xyz_tra.u2_x;
    p_xyz->u2_y= xyz_tra.u2_y;
    p_xyz->u2_z= xyz_tra.u2_z;

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= xyz_tra.v2_x;
    p_xyz->v2_y= xyz_tra.v2_y;
    p_xyz->v2_z= xyz_tra.v2_z;

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= xyz_tra.uv_x;
    p_xyz->uv_y= xyz_tra.uv_y;
    p_xyz->uv_z= xyz_tra.uv_z;

   p_xyz->sp_x  = poi_spine.x_gm;
   p_xyz->sp_y  = poi_spine.y_gm;
   p_xyz->sp_z  = poi_spine.z_gm; 
   p_xyz->spt_x = tan_spine.x_gm;
   p_xyz->spt_y = tan_spine.y_gm;
   p_xyz->spt_z = tan_spine.z_gm;
   p_xyz->spt2_x= 0.0;
   p_xyz->spt2_y= 0.0;
   p_xyz->spt2_z= 0.0;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur227 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur227 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur227 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fprintf(dbgfil(SURPAC),
  "sur227 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur227 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur227 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur227 Exit *** varkon_pat_rotloft x= %8.2f y= %8.2f z= %8.2f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef DEBUG        
/*!********* Internal ** function **Defined*Only*For*Debug*On********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF.                            */
/* Printout also of the conic lofting patch data.                   */

   static short initial(p_patl,u_pat,v_pat,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat  u_pat;         /* Patch (local) U parameter value         */
   DBfloat  v_pat;         /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur227 *** initial: p_xyz= %d\n", (int)p_xyz);
  }

/*!                                                                 */
/* 1. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   


/*  Tangent             dr/du                                       */
    p_xyz->u_x=  -F_UNDEF;   
    p_xyz->u_y=   F_UNDEF;   
    p_xyz->u_z=   F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

/*  Surface normal       r(u)                                       */
    p_xyz->n_x= F_UNDEF;   
    p_xyz->n_y= F_UNDEF;   
    p_xyz->n_z= F_UNDEF;   

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= F_UNDEF;   
    p_xyz->nu_y= F_UNDEF;   
    p_xyz->nu_z= F_UNDEF;   

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= F_UNDEF;   
    p_xyz->nv_y= F_UNDEF;   
    p_xyz->nv_z= F_UNDEF;   


/*  Maximum and minimum principal curvature                          */
    p_xyz->kmax= F_UNDEF;   
    p_xyz->kmin= F_UNDEF;   

/*  Direction in R*3 for kmax                                        */
    p_xyz->kmax_x= F_UNDEF;   
    p_xyz->kmax_y= F_UNDEF;   
    p_xyz->kmax_z= F_UNDEF;   

/*  Direction in R*3 for kmin                                        */
    p_xyz->kmin_x= F_UNDEF;   
    p_xyz->kmin_y= F_UNDEF;   
    p_xyz->kmin_z= F_UNDEF;   

/*  Direction in u,v for kmax                                       */
    p_xyz->kmax_u= F_UNDEF;   
    p_xyz->kmax_v= F_UNDEF;   

/*  Direction in u,v for kmin                                       */
    p_xyz->kmin_u= F_UNDEF;   
    p_xyz->kmin_v= F_UNDEF;   

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif


/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create rotation coordinate system                                */

   static short r_csys ( p_patl )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz_c;       /* Point and derivatives for curve         */
/*-----------------------------------------------------------------!*/
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBVector origin;      /* Origin for line coordinate  system      */
   DBVector vecx;        /* X axis for line coordinate  system      */
   DBVector vecy;        /* Y axis for line coordinate  system      */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Start and end points from the spine                           */
/*    Calls of varkon_GE109 (GE109)                               */
/*                                                                 !*/

   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      

   xyz_c.evltyp        = EVC_R; 

   xyz_c.t_global      = 1.0;   /* Global parameter start point     */


status=GE109 ((DBAny *)&scur, &p_patl->spine, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur227");
     return(varkon_erpush("SU2943",errbuf));
    }

   origin = xyz_c.r;

   xyz_c.t_global      = 2.0;   /* Global parameter end   point     */

status=GE109 ((DBAny *)&scur, &p_patl->spine, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur227");
     return(varkon_erpush("SU2943",errbuf));
    }


   vecx.x_gm = xyz_c.r.x_gm - origin.x_gm;
   vecx.y_gm = xyz_c.r.y_gm - origin.y_gm;
   vecx.z_gm = xyz_c.r.z_gm - origin.z_gm;

   xyz_c.t_global      = 1.5;   /* Global parameter mid point curve */

status=GE109 ((DBAny *)&scur, &p_patl->lims , &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur227");
     return(varkon_erpush("SU2943",errbuf));
    }


   vecy.x_gm = xyz_c.r.x_gm - origin.x_gm;
   vecy.y_gm = xyz_c.r.y_gm - origin.y_gm;
   vecy.z_gm = xyz_c.r.z_gm - origin.z_gm;


    status = GEmktf_3p (&origin,&vecx,&vecy,&rot_csys);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEmktf_3p%%varkon_sur_intlinept");
  return(varkon_erpush("SU2943",errbuf));
  }

/*!                                                                 */
/* 2. Current spine tangent                                         */
/*    Assume (with no check) that spine is a straight line          */
/*                                                                 !*/

   tan_spine = vecx;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*r_csys origin %f %f %f\n",origin.x_gm,origin.y_gm,origin.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*r_csys vecx   %f %f %f\n",vecx.x_gm,vecx.y_gm,vecx.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*r_csys vecy   %f %f %f\n",vecy.x_gm,vecy.y_gm,vecy.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur227 Rotation transformation matrix \n");
fprintf(dbgfil(SURPAC),"sur227 g11 %f g12 %f g13 %f g14 %f \n",
                rot_csys.g11,rot_csys.g12,rot_csys.g13,rot_csys.g14);
fprintf(dbgfil(SURPAC),"sur227 g21 %f g22 %f g23 %f g24 %f \n",
                rot_csys.g21,rot_csys.g22,rot_csys.g23,rot_csys.g24);
fprintf(dbgfil(SURPAC),"sur227 g31 %f g32 %f g33 %f g34 %f \n",
                rot_csys.g31,rot_csys.g32,rot_csys.g33,rot_csys.g34);
fprintf(dbgfil(SURPAC),"sur227 g41 %f g42 %f g43 %f g44 %f \n",
                rot_csys.g41,rot_csys.g42,rot_csys.g43,rot_csys.g44);
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/




/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate curve derivatives in local system for the input U      */

   static short cderiv ( p_patl, u_pat )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat   u_pat;        /* Patch (local) U parameter value         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBSeg   lime_t;       /* Transformed end limit curve             */
/*-----------------------------------------------------------------!*/
   DBCurve scur;         /* Curve header for segment (dummy) curve  */
   char    errbuf[80];   /* String for error message fctn erpush    */
   short   status;       /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Transformation of the start limit curve                       */
/*    Call of GEtfseg_to_local                                      */
/*                                                                 !*/

status=GEtfseg_to_local (&p_patl->lims , &rot_csys , &lims_t);
if (status<0) 
     {
     sprintf(errbuf,"GEtfseg_to_local%%sur227*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }


/*!                                                                 */
/* 2. Calculate curve derivatives for the given U                   */
/*    Call of varkon_GE109 (GE109)                                */
/*                                                                 !*/


   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      

   xyz_l.evltyp        = EVC_D2R; 

   xyz_l.t_global      = 1.0 + u_pat; 


status=GE109 ((DBAny *)&scur, &lims_t, &xyz_l );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur227*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }


/*!                                                                 */
/* 3. Transformation of the end   limit curve                       */
/*    Call of GEtfseg_to_local                                      */
/*                                                                 !*/

status=GEtfseg_to_local (&p_patl->lime , &rot_csys , &lime_t);
if (status<0) 
     {
     sprintf(errbuf,"GEtfseg_to_local%%sur227*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* 4. Calculate curve coordinates for the mid pt on end limit crv   */
/*    Call of varkon_GE109 (GE109)                                */
/*                                                                 !*/


   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      

   xyz_le.evltyp       = EVC_D2R; 

   xyz_le.t_global     = 1.5; 


status=GE109 ((DBAny *)&scur, &lime_t, &xyz_le );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur227*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }

   poi_lime = xyz_le.r;



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*cderiv xyz_l.r       %f %f %f\n",
xyz_l.r.x_gm,xyz_l.r.y_gm,xyz_l.r.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*cderiv xyz_l.drdt    %f %f %f\n",
xyz_l.drdt.x_gm,xyz_l.drdt.y_gm,xyz_l.drdt.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*cderiv xyz_l.d2rdt2  %f %f %f\n",
xyz_l.d2rdt2.x_gm,xyz_l.d2rdt2.y_gm,xyz_l.d2rdt2.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*cderiv poi_lime      %f %f %f\n",
poi_lime.x_gm,poi_lime.y_gm,poi_lime.z_gm);
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/




/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate surface derivatives in local system                    */

   static short sderiv (  )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */


/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBfloat   phi;          /* Angle                                   */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Calculate derivatives                                         */
/*                                                                 !*/

  phi               =  v_phi;

  xyz_s.r_x         =  xyz_l.r.x_gm;
  xyz_s.r_y         =  xyz_l.r.y_gm*COS(phi);
  xyz_s.r_z         =  xyz_l.r.y_gm*SIN(phi);

/* Derivative cannot be calculated for y= 0 .. Program more ...  */
/* Curvature is not OK in y= 0                                   */
  if  ( ABS(xyz_l.r.y_gm) < ctol ) xyz_l.r.y_gm = ctol; 
  xyz_s.u_x         =  xyz_l.drdt.x_gm;
  xyz_s.u_y         =  xyz_l.drdt.y_gm*COS(phi);
  xyz_s.u_z         =  xyz_l.drdt.y_gm*SIN(phi);

  xyz_s.v_x         =   0.0;
  xyz_s.v_y         = -xyz_l.r.y_gm*SIN(phi);
  xyz_s.v_z         =  xyz_l.r.y_gm*COS(phi);

  xyz_s.u2_x        =  xyz_l.d2rdt2.x_gm;
  xyz_s.u2_y        =  xyz_l.d2rdt2.y_gm*COS(phi);
  xyz_s.u2_z        =  xyz_l.d2rdt2.y_gm*SIN(phi);

  xyz_s.v2_x        =   0.0;
  xyz_s.v2_y        = -xyz_l.r.y_gm*COS(phi);
  xyz_s.v2_z        = -xyz_l.r.y_gm*SIN(phi);

  xyz_s.uv_x        =   0.0;
  xyz_s.uv_y        = -xyz_l.drdt.y_gm*SIN(phi);
  xyz_s.uv_z        =  xyz_l.drdt.y_gm*COS(phi);


/*!                                                                 */
/* Derivative with respect to v_pat                                 */
/*                                                                 !*/

  xyz_s.v_x         = xyz_s.v_x*tot_phi;
  xyz_s.v_y         = xyz_s.v_y*tot_phi;
  xyz_s.v_z         = xyz_s.v_z*tot_phi;

  xyz_s.v2_x        = xyz_s.v2_x*tot_phi*tot_phi;
  xyz_s.v2_y        = xyz_s.v2_y*tot_phi*tot_phi;
  xyz_s.v2_z        = xyz_s.v2_z*tot_phi*tot_phi;

  xyz_s.uv_x        = xyz_s.uv_x*tot_phi;
  xyz_s.uv_y        = xyz_s.uv_y*tot_phi;
  xyz_s.uv_z        = xyz_s.uv_z*tot_phi;



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s XYZ     %f %f %f\n",
xyz_s.r_x,xyz_s.r_y,xyz_s.r_z);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s DRDU    %f %f %f\n",
xyz_s.u_x,xyz_s.u_y,xyz_s.u_z);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s DRDV    %f %f %f\n",
xyz_s.v_x,xyz_s.v_y,xyz_s.v_z);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s D2RDU2  %f %f %f\n",
xyz_s.u2_x,xyz_s.u2_y,xyz_s.u2_z);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s D2RDV2  %f %f %f\n",
xyz_s.v2_x,xyz_s.v2_y,xyz_s.v2_z);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*sderiv xyz_s D2RDUDV %f %f %f\n",
xyz_s.uv_x,xyz_s.uv_y,xyz_s.uv_z);
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate angle corresponding to input V parameter               */

   static short vangle ( v_pat )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat v_pat;        /* Patch (local) V parameter value         */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/
   char    errbuf[80];   /* String for error message fctn erpush    */
   short   status;       /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Current spine point in local coordinates                      */
/*                                                                 !*/

   poi_spine.x_gm = poi_lime.x_gm;
   poi_spine.y_gm =   0.0;
   poi_spine.z_gm =   0.0;


/*                                                                  */
/* 2. Total angle for generatrix segment                            */
/*                                                                 !*/

   if  ( ABS(poi_lime.y_gm) > 0.001 ||
         ABS(poi_lime.z_gm) > 0.001    )
     tot_phi = atan2 ( poi_lime.z_gm, poi_lime.y_gm );
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*vangle poi_lime.z= %f .y= %f < 0.0001\n",
poi_lime.z_gm, poi_lime.y_gm ) ;
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*vangle Curve mid pt too close to the rotation line (spine)\n");
}
#endif
     sprintf(errbuf,"poi_lime%%sur227*vangle");
     return(varkon_erpush("SU2993",errbuf));
    }


/*                                                                  */
/* 3. Angle corresponding to input parameter V                      */
/*                                                                 !*/

   v_phi = v_pat*tot_phi;
   

/*                                                                  */
/* 4. Current spine point in global coordinates                     */
/*    Call of GEtfpos_to_basic                                      */
/*                                                                 !*/

status=GEtfpos_to_basic (&poi_spine , &rot_csys , &poi_spine);
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_basic%%sur227*vangle");
     return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*vangle poi_spine    %f %f %f\n",
poi_spine.x_gm, poi_spine.y_gm, poi_spine.z_gm) ;
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur227*vangle tot_phi %f v_phi %f \n",
tot_phi, v_phi);
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



