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
/*  Function: varkon_pat_rulloft                   File: sur228.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a conic lofting patch,       */
/*  which is an arclength ruled surface.                            */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-26   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_rulloft    Ruled    lofting evaluation fctn */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*                                                                  */
/*         Evaluation of coordinates and derivatives                */
/*             for an archlength ruled surface                      */
/*                                                                  */
/*                                                                  */
/* Reference: Faux & Pratt p 115 and p 118                          */
/*                                                                  */
/* Input data to (used data in) this functions is:                  */
/* - The start limit curve                                          */
/* - The end   limit curve                                          */
/* - A parameter point u_sl,v where u_sl is the parameter for       */
/*   the start limit curve (the spine curve is equal to the         */
/*   start limit curve). The v value is the relative distance       */
/*   along the ruling.                                              */
/*                                                                  */
/* The ruling end point is defined as the point with the same       */
/* relative arclength as the ruling start point on the start        */
/* limit curve. Limit curve means the current limit curve           */
/* segment (not the whole composite curve).                         */
/*                                                                  */
/* The surface is defined by:                                       */
/*                                                                  */
/* r(u,v) = (1-v)*r_sl(u_sl) + v*r_el(u_el(u_sl))                   */
/*                                                                  */
/* where u_sl = parameter for the start limit curve                 */
/* where u_el = parameter for the end   limit curve                 */
/*                                                                  */
/* The U parameter for the surface u is equal to the parameter      */
/* for the start limit curve:                                       */
/*                                                                  */
/* r(u,v) = r(u_sl,v)                                               */
/*                                                                  */
/* There is a relation between the curve parameters u_sl and u_e    */
/* defined by the "equal relative arclengths".                      */
/* The procedure to calculate point and derivatives:                */
/* Step    Description                        Formulae              */
/*   i.  Start limit curve parameter       u_sl   = u               */
/*  ii.  Calculate relative arclength      u_sl_r = u_sl_r(u_sl)    */
/*       for the start limit curve                                  */
/* iii.  Calculate relative arclength      du_sl_r/du_sl            */
/*       derivative for start curve                                 */
/*  iv.  Calculate relative arclength      u_el_r = u_sl_r          */
/*       for the end   limit curve                                  */
/*   v.  Calculate curve parameter         u_el   = u_el(u_el_r)    */
/*       for the end   limit curve                                  */
/*                                                                  */
/* ===> Relation u_el = u_el (u_el_r(u_sl_r((u_sl)))                */
/*            For the end limit curve                               */
/* ===> Relation r_el = r_el (u_el(u_el_r(u_sl_r((u_sl))))          */
/*                                                                  */
/* The values u_el and u_sl_r are calculated numerically. The       */
/* derivatives are calculated with (the chain rule is used):        */
/*                                                                  */
/*  dr_el    dr_el    du_el      du_el_r   du_sl_r                  */
/*  ----- =  ----- * -------  *  ------- * -------                  */
/*   du      du_el   du_el_r     du_sl_r    du_sl                   */
/*                                                                  */
/*                                                                  */
/*  where                                                           */
/*                                                                  */
/*   du_el                   dr_el   dr_el                          */
/*  ------- =  a_el  / SQRT( ----- * ----- )     (dudur_el)         */
/*  du_el_r                  du_el   du_el                          */
/*                                                                  */
/*  du_el_r                                                         */
/*  ------- =   1                                                   */
/*  du_sl_r                                                         */
/*                                                                  */
/*  du_sl_r     1            dr_sl   dr_sl                          */
/*  ------- = ------ * SQRT( ----- * ----- )     (durdu_sl)         */
/*   du_sl     a_sl          du_sl   du_sl                          */
/*                                                                  */
/* Same funtion for dr_el/du, i.e. dr_el/du is a function of        */
/* u_el, u_el_r, l_r,i u_sl_r and u_sl:                             */
/* dr_el/du = first = first (u_el(u_el_r(u_sl_r((u_sl))))           */
/*                                                                  */
/*  dfirst   dfirst    du_el      du_el_r   du_sl_r                 */
/*  ----- =  ----- * -------  *  ------- * -------                  */
/*   du       du_el   du_el_r     du_sl_r    du_sl                  */
/*                                                                  */
/*                                                                  */
/*  where                                                           */
/*                                                                  */
/*  dfirst   d2r_el                                                 */
/*  ------ = -------                                                */
/*  du_el    du_el2                                                 */
/*                                                                  */
/*                                                                  */
/* Differentiation with respect to u and v:                         */
/*                                                                  */
/*                                                                  */
/* dr/du  = (1-v)*dr_sl/du + v*dr_el/du                             */
/* dr/dv  =   -r_sl(s) + r_el(s)                                    */
/*                                                                  */
/* d2r/du2  = (1-v)*d2r_sl/du2 + v*d2r_el/du2                       */
/* d2r/dv2  =   0                                                   */
/* d2r/dudv =   -dr_sl/du + dr_el/du                                */
/*                                                                  */
/* where (see above)                                                */
/*                                                                  */
/* dr_sl/du   =  dr_sl/du_sl                                        */
/* d2r_sl/du2 = d2r_sl/du_sl2                                       */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short cderiv();        /* Calculate curve   derivatives      */
static short sderiv();        /* Calculate surface derivatives      */

/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static EVALC    xyz_s;            /* Start limit coord. & derivat.  */
static EVALC    xyz_e;            /* End   limit coord. & derivat.  */
static DBfloat  a_sl;             /* Curve length start limit crv   */
static DBfloat  a_el;             /* Curve length end   limit crv   */
static DBfloat  dudur_el;         /* End   limit u(u_r) derivative  */
static DBfloat  durdu_sl;         /* Start limit u_r(u) derivative  */
static DBfloat  ctol;             /* Coordinate tolerance           */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol            * Coordinate tolerance                    */
/* GE109                  * Curve evaluation routine                */
/* GE718                  * Relative arcl. for global U             */
/* GE717                  * Global U for relative arcl.             */
/* varkon_erinit          * Initial. of error messages              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_rulloft   */
/* SU2993 = Severe program error in varkon_pat_rulloft (sur228).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_pat_rulloft (

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
"sur228 Mid curve flag p_patl->p_flag %d\n", (int)p_patl->p_flag);
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
   sprintf(errbuf,"initial%%varkon_pat_rulloft (sur228)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/*  Retrieve coordinate tolerance. Call of varkon_ctol (sur751)     */
/*                                                                 !*/

   ctol      = varkon_ctol();

/*!                                                                 */
/* 2. Calculate curve derivatives                                   */
/* ______________________________                                   */
/*                                                                  */
/* Call of cderiv.                                                  */
/*                                                                 !*/
   status= cderiv (p_patl, u_pat); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur228 cderiv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"cderiv%%sur228");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 3. Calculate output surface derivatives                          */
/* _______________________________________                          */
/*                                                                  */
/* Call of sderiv.                                                  */
/*                                                                 !*/
   status= sderiv ( v_pat, p_xyz ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur228 sderiv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"sderiv%%sur228");
   return(varkon_erpush("SU2973",errbuf));
   }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur228 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur228 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur228 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fprintf(dbgfil(SURPAC),
  "sur228 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur228 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur228 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228 Exit *** varkon_pat_rulloft x= %8.2f y= %8.2f z= %8.2f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fflush(dbgfil(SURPAC)); 
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
/* static (common) variables to F_UNDEF and I_UNDEF.                */

   static short initial(p_patl,u_pat,v_pat,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
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
  "sur228 *** initial: p_xyz= %d\n", (int)p_xyz);
  }

/*!                                                                 */
/* 1. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   


/*  Tangent             dr/du                                       */
    p_xyz->u_x=   F_UNDEF;   
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

/* Static internal variables                                        */
    a_sl         = F_UNDEF;
    a_el         = F_UNDEF;
    dudur_el     = F_UNDEF;
    durdu_sl     = F_UNDEF;

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif






/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate curve derivatives for the input U                      */

   static short cderiv ( p_patl, u_pat )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat u_pat;        /* Patch (local) U parameter value         */

/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBfloat r_arcl;       /* Relative arclength start limit curve    */
   DBfloat u_arcl;       /* U value on end limit curve for r_arcl   */
   DBTmat *p_csys;       /* Coordinate system                 (ptr) */
   DBfloat uglobs;       /* Global u start value                    */
   DBfloat uglobe;       /* Global u end   value                    */
   DBfloat leng_sl;      /* Tangent length start limit curve        */
   DBfloat leng_el;      /* Tangent length end   limit curve        */
   DBCurve scur;         /* Curve header for segment (dummy) curve  */
   DBCurve ecur;         /* Curve header for segment (dummy) curve  */
   char    errbuf[80];   /* String for error message fctn erpush    */
   short   status;       /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 1. Calculate curve derivatives for the given U on start limit crv*/
/*    Call of varkon_GE109 (GE109)                                */
/*                                                                 !*/


   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      
   scur.al_cu          = 0.0;      
   ecur.hed_cu.type    = CURTYP;
   ecur.ns_cu          = 1;      
   ecur.plank_cu       = FALSE;      
   ecur.al_cu          = 0.0;      

   xyz_s.evltyp        = EVC_D2R; 

   xyz_s.t_global      = 1.0 + u_pat; 


status=GE109 ((DBAny *)&scur, &p_patl->lims, &xyz_s );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur228*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_s.r       %f %f %f\n",
xyz_s.r.x_gm,xyz_s.r.y_gm,xyz_s.r.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_s.drdt    %f %f %f\n",
xyz_s.drdt.x_gm,xyz_s.drdt.y_gm,xyz_s.drdt.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_s.d2rdt2  %f %f %f\n",
xyz_s.d2rdt2.x_gm,xyz_s.d2rdt2.y_gm,xyz_s.d2rdt2.z_gm);
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 2. Calculate curve derivatives for the end limit curve           */
/*                                                                 !*/

p_csys = NULL;
uglobs = 1.0;

/*!                                                                 */
/* Calculate relative arclength to U= spine_u on boundary curve 1   */
/* Call of varkon_GE718 (GE718).                                  */
/*                                                                 !*/

   uglobe = u_pat + 1.0;

   status=GE718
   ((DBAny *)&scur, &p_patl->lims, p_csys, uglobs, uglobe, &r_arcl);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur863*ubounds GE718 failed r_arcl %20.15f \n", r_arcl  );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%sur228*cderiv");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Calculate U value on end limit curve corresponding to the        */
/* relative arclength.                                              */
/* Call of varkon_GE717 (GE717).                                  */
/*                                                                 !*/

   status=GE717
   ((DBAny *)&ecur, &p_patl->lime, NULL ,r_arcl, &u_arcl );
   if (status<0) 
        {
        sprintf(errbuf,"GE717%%sur228*cderiv");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* Calculate curve derivatives for end limit curve.                 */
/* Call of varkon_GE109 (GE109)                                   */
/*                                                                 !*/


   xyz_e.evltyp        = EVC_D2R; 

   xyz_e.t_global      = u_arcl; 


status=GE109 ((DBAny *)&ecur, &p_patl->lime, &xyz_e );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur228*cderiv");
     return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* Curve derivatives with respect to relative arclengths            */
/* ------------------------------------------------------           */
/*                                                                  */
/* Arclengths for current segments a_sl and a_el.                   */
/* Tangent lengths start and end limit curves                       */
/*                                                                 !*/

   a_sl = p_patl->lims.sl;
   a_el = p_patl->lime.sl;

   if  ( a_sl < ctol )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv Failure a_sl %f (=0)\n", a_sl);
fflush(dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"a_sl=0%%sur228*cderiv");
     return(varkon_erpush("SU2993",errbuf));
    }
   if  ( a_el < ctol )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv Failure a_el %f (=0)\n", a_el);
fflush(dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"a_el=0%%sur228*cderiv");
     return(varkon_erpush("SU2993",errbuf));
    }


   leng_sl = SQRT( 
            xyz_s.drdt.x_gm*xyz_s.drdt.x_gm +
            xyz_s.drdt.y_gm*xyz_s.drdt.y_gm +
            xyz_s.drdt.z_gm*xyz_s.drdt.z_gm );
   leng_el = SQRT( 
            xyz_e.drdt.x_gm*xyz_e.drdt.x_gm +
            xyz_e.drdt.y_gm*xyz_e.drdt.y_gm +
            xyz_e.drdt.z_gm*xyz_e.drdt.z_gm );

   if  ( leng_sl < ctol )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv Failure leng_sl %f (=0)\n", leng_sl);
fflush(dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"leng_sl=0%%sur228*cderiv");
     return(varkon_erpush("SU2993",errbuf));
    }
   if  ( leng_el < ctol )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv Failure leng_el %f (=0)\n", leng_el);
fflush(dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"leng_el=0%%sur228*cderiv");
     return(varkon_erpush("SU2993",errbuf));
    }

   dudur_el =  a_el   / leng_el;
   durdu_sl = leng_sl /  a_sl;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv u_pat %f u_arcl %f\n", u_pat, u_arcl);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_e.r       %f %f %f\n",
xyz_e.r.x_gm,xyz_e.r.y_gm,xyz_e.r.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_e.drdt    %f %f %f\n",
xyz_e.drdt.x_gm,xyz_e.drdt.y_gm,xyz_e.drdt.z_gm);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv xyz_e.d2rdt2  %f %f %f\n",
xyz_e.d2rdt2.x_gm,xyz_e.d2rdt2.y_gm,xyz_e.d2rdt2.z_gm);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv a_sl %f a_el %f leng_sl %f leng_el %f\n",
a_sl, a_el, leng_sl, leng_el);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur228*cderiv dudur_el %f durdu_sl %f\n",
dudur_el, durdu_sl );
fflush(dbgfil(SURPAC));
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
/* Calculate surface derivatives for the input U,V point            */

   static short sderiv ( v_pat, p_xyz )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat v_pat;        /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector dr_eldu;     /* dr_el/du_sl                             */
   DBVector d2r_eldu2;   /* d2r_el/du_sl2                           */
/*-----------------------------------------------------------------!*/
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 1. Calculate coordinates                                         */
/*                                                                 !*/

    p_xyz->r_x= (1.0-v_pat)*xyz_s.r.x_gm + v_pat*xyz_e.r.x_gm;
    p_xyz->r_y= (1.0-v_pat)*xyz_s.r.y_gm + v_pat*xyz_e.r.y_gm;
    p_xyz->r_z= (1.0-v_pat)*xyz_s.r.z_gm + v_pat*xyz_e.r.z_gm;

/*!                                                                 */
/* 2. Calculate first derivatives                                   */
/*                                                                 !*/

/*  dr_el    dr_el    du_el      du_el_r   du_sl_r                  */
/*  ----- =  ----- * -------  *  ------- * -------                  */
/*   du      du_el   du_el_r     du_sl_r    du_sl                   */
/*                                                                  */

    dr_eldu.x_gm = xyz_e.drdt.x_gm*dudur_el*durdu_sl;
    dr_eldu.y_gm = xyz_e.drdt.y_gm*dudur_el*durdu_sl;
    dr_eldu.z_gm = xyz_e.drdt.z_gm*dudur_el*durdu_sl;

    p_xyz->u_x= (1.0-v_pat)*xyz_s.drdt.x_gm + v_pat*dr_eldu.x_gm;
    p_xyz->u_y= (1.0-v_pat)*xyz_s.drdt.y_gm + v_pat*dr_eldu.y_gm;
    p_xyz->u_z= (1.0-v_pat)*xyz_s.drdt.z_gm + v_pat*dr_eldu.z_gm;


    p_xyz->v_x= -xyz_s.r.x_gm + xyz_e.r.x_gm;
    p_xyz->v_y= -xyz_s.r.y_gm + xyz_e.r.y_gm;
    p_xyz->v_z= -xyz_s.r.z_gm + xyz_e.r.z_gm;

/*!                                                                 */
/* 3. Calculate second derivatives                                  */
/*                                                                 !*/

    d2r_eldu2.x_gm = xyz_e.d2rdt2.x_gm*dudur_el*durdu_sl;
    d2r_eldu2.y_gm = xyz_e.d2rdt2.y_gm*dudur_el*durdu_sl;
    d2r_eldu2.z_gm = xyz_e.d2rdt2.z_gm*dudur_el*durdu_sl;

/* d2r/du2  = (1-v)*d2r_sl/du2 + v*d2r_el/du2                       */
/* d2r/dv2  =   0                                                   */
/* d2r/dudv =   -dr_sl/du + dr_el/du                                */
/*                                                                  */


    p_xyz->u2_x= (1.0-v_pat)*xyz_s.d2rdt2.x_gm + v_pat*d2r_eldu2.x_gm;
    p_xyz->u2_y= (1.0-v_pat)*xyz_s.d2rdt2.y_gm + v_pat*d2r_eldu2.y_gm;
    p_xyz->u2_z= (1.0-v_pat)*xyz_s.d2rdt2.z_gm + v_pat*d2r_eldu2.z_gm;

    p_xyz->v2_x= 0.0;
    p_xyz->v2_y= 0.0;
    p_xyz->v2_z= 0.0;

    p_xyz->uv_x= - xyz_s.drdt.x_gm + dr_eldu.x_gm;
    p_xyz->uv_y= - xyz_s.drdt.y_gm + dr_eldu.y_gm;
    p_xyz->uv_z= - xyz_s.drdt.z_gm + dr_eldu.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228*sderiv dr_eldu   %f %f %f\n",
   dr_eldu.x_gm  , dr_eldu.y_gm  , dr_eldu.z_gm   );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228*sderiv d2r_eldu2 %f %f %f\n",
   d2r_eldu2.x_gm, d2r_eldu2.y_gm, d2r_eldu2.z_gm );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228*sderiv x= %8.2f y= %8.2f z= %8.2f v_pat %f\n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z, v_pat);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228*sderiv dxdu= %8.2f dydu= %8.2f dzdu= %8.2f\n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur228*sderiv dxdv= %8.2f dydv= %8.2f dzdv= %8.2f\n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


