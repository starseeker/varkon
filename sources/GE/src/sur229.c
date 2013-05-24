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
/*  Function: varkon_pat_sweeploft                 File: sur229.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a conic lofting patch,       */
/*  which is a swept surface.                                       */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1997-05-02   Originally written                                 */
/*  1997-05-11   Debug                                              */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_sweeploft  Sweep lofting evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*                                                                  */
/*         Evaluation of coordinates and derivatives                */
/*                  for a swept surface                             */
/*                                                                  */
/*                                                                  */
/* Reference: Curved spine for FORMELA, Gunnar Liden 1980           */
/*                                                                  */
/*                                                                  */
/*  The swep surface is defined by:                                 */
/*                                                                  */
/*   r(u,v) = T(u)*r(v)     where T is a transformation matrix      */
/*                                                                  */
/*  Differentiation:                                                */
/*                                                                  */
/*                                                                  */
/*  dr/du   =   dT/du  *  r(v)                                      */
/*                                                                  */
/*  dr/dv   =    T(u)  *  dr/dv                                     */
/*                                                                  */
/* d2r/du2  =  d2T/du2 *  r(v)                                      */
/*                                                                  */
/* d2r/dv2  =    T(u)  * d2r/dr2                                    */
/*                                                                  */
/* d2r/dudv =   dT/du  * dr/dv                                      */
/*                                                                  */
/*                                                                  */
/* The U derivate matrices are numerically calculated in this       */
/* function. They could be more analytically calculated (like       */
/* in sur222) or analytically (like in the referenced paper).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short t_csys();        /* Create transformation matrix       */
static short dermat();        /* Calculate U derivative matrices    */
static short sderiv();        /* Calculate surface derivatives      */

/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */

static DBTmat  tra_csys;          /* Transformation   T(u)          */
static DBTmat  tra_csys_du;       /* Transformation  dT/du          */
static DBTmat  tra_csys_du2;      /* Transformation d2T/du2         */
static DBfloat ctol;              /* Coordinate tolerance           */
static DBfloat idpoint;           /* Identical point criterion      */
static DBfloat comptol;           /* Computer tolerance             */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol            * Coordinate tolerance                    */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_comptol         * Retrieve computer tolerance             */
/* GE109                  * Curve evaluation routine                */
/* GEmktf_3p              * Create local coord. system              */
/* GEtfpos_to_local       * Transformation of a point               */
/* GEtfvec_to_local       * Transformation of a vector              */
/* GEtform_inv            * Invertation of a matrix                 */
/* varkon_erinit          * Initial. of error messages              */
/* varkon_erpush          * Error message to stack                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_sweeploft */
/* SU2993 = Severe program error in varkon_pat_sweeploft (sur229).  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_pat_sweeploft (

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
   DBTmat  tra0_csys;    /* Transformation system                   */
   DBTmat  tra1_csys;    /* Transformation system           eps     */
   DBTmat  tra2_csys;    /* Transformation system         2*eps     */
   DBfloat t_eps;        /* Epsilon for parameter                   */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   char    errbuf[80];   /* String for error message fctn erpush    */
   short   status;       /* Error code from a called function       */

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
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229 Type of LFT_PAT p_patl->p_flag= %d\n", (int)p_patl->p_flag);
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
   sprintf(errbuf,"initial%%varkon_pat_sweeploft (sur229)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/*  Retrieve tolerances.                                            */
/*                                                                 !*/


   ctol      = varkon_ctol();
   idpoint   = varkon_idpoint();
   comptol   = varkon_comptol();

/*!                                                                 */
/* 2. Calculate transformation matrices                             */
/* ____________________________________                             */
/*                                                                  */
/* Call of t_csys.                                                  */
/*                                                                 !*/

   t_eps = 0;



   status= t_csys (u_pat, p_patl, &t_eps, &tra0_csys ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229 t_csys failed 0\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"t_csys%%sur229");
   return(varkon_erpush("SU2973",errbuf));
   }

   V3MOME((char *)(&tra0_csys),(char *)(&tra_csys),sizeof(DBTmat));

   status= t_csys (u_pat+t_eps, p_patl, &t_eps, &tra1_csys ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229 t_csys failed 1\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"t_csys%%sur229");
   return(varkon_erpush("SU2973",errbuf));
   }

   status= t_csys (u_pat+2.0*t_eps, p_patl, &t_eps, &tra2_csys ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229 t_csys failed 2\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"t_csys%%sur229");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 3. Calculate U derivative transformation matrices.               */
/* _________________________________________________                */
/*                                                                  */
/* Call of dermat.                                                  */
/*                                                                 !*/
   status= dermat ( t_eps, &tra0_csys, &tra1_csys, &tra2_csys ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229 dermat failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"dermat%%sur229");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 2. Calculate surface derivatives                                 */
/* ________________________________                                 */
/*                                                                  */
/* Call of sderiv.                                                  */
/*                                                                 !*/
   status= sderiv (p_patl, v_pat, p_xyz ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229 sderiv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"sderiv%%sur229");
   return(varkon_erpush("SU2973",errbuf));
   }



#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur229 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur229 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur229 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fprintf(dbgfil(SURPAC),
  "sur229 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur229 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur229 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur229 Exit *** varkon_pat_sweeploft x= %8.2f y= %8.2f z= %8.2f \n",
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
  "sur229 *** initial: p_xyz= %d\n", (int)p_xyz);
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
/* Create transformation matrix                                     */

   static short t_csys ( t_local, p_patl, p_t_eps, p_tra_csys )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat  t_local;     /* Spine/patch (local) U parameter value   */
   GMPATL  *p_patl;      /* Pointer to the conic lofting patch      */
   DBfloat *p_t_eps;     /* Epsilon for parameter             (ptr) */
   DBTmat  *p_tra_csys;  /* Transformation system             (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  EVALC    xyz_c;        /* Point and derivatives for curve         */
  DBTmat   spine_csys;   /* Transformation system defined by spine  */
/*-----------------------------------------------------------------!*/
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBVector origin;      /* Origin for line coordinate  system      */
   DBVector vecx;        /* X axis for line coordinate  system      */
   DBVector vecy;        /* Y axis for line coordinate  system      */
   DBVector vecz;        /* Z axis for line coordinate  system      */
   DBVector yaxis;       /* Fixed direction defining the Y axis     */
   DBfloat  l_tang;      /* Length of tangent vector                */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys Enter t_local %15.10f *p_t_eps %12.10f\n",
                   t_local,*p_t_eps);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Point and derivatives for the spine                              */
/* Call of varkon_GE109 (GE109)                                   */
/*                                                                 !*/
   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      

   xyz_c.evltyp        = EVC_DR; 

   xyz_c.t_global      = 1.0 + t_local; 


status=GE109 ((DBAny *)&scur, &p_patl->spine, &xyz_c );
   if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229*t_csys GE109 failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"GE109%%sur229");
     return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* Origin is current point and Z axis is the tangent                */
/*                                                                 !*/

   origin = xyz_c.r;
   vecz   = xyz_c.drdt;

/*!                                                                 */
/* Epsilon parameter value if not defined                           */
/*                                                                 !*/

   if  ( ABS(*p_t_eps) < comptol )
     {
     l_tang = SQRT( vecz.x_gm*vecz.x_gm +
                    vecz.y_gm*vecz.y_gm +
                    vecz.z_gm*vecz.z_gm );
     if  (  l_tang <  idpoint )
       {
       varkon_erinit();
       sprintf(errbuf,"%5.3f%%sur829",t_local);
       return(varkon_erpush("SU8313",errbuf));
       }
     *p_t_eps = idpoint/l_tang;
     if (t_local + 2.0*(*p_t_eps) > 1.0) *p_t_eps = -1.0*(*p_t_eps);
     }

/*!                                                                 */
/* Retrieve fixed direction vector from current patch.              */
/*                                                                 !*/

   yaxis.x_gm = p_patl->pval.c0x;     
   yaxis.y_gm = p_patl->pval.c0y;     
   yaxis.z_gm = p_patl->pval.c0z;     

/*!                                                                 */
/* The X axis is the vector product yaxis X vecz.                   */
/*                                                                 !*/

   vecx.x_gm = yaxis.y_gm*vecz.z_gm - yaxis.z_gm*vecz.y_gm;
   vecx.y_gm = yaxis.z_gm*vecz.x_gm - yaxis.x_gm*vecz.z_gm;
   vecx.z_gm = yaxis.x_gm*vecz.y_gm - yaxis.y_gm*vecz.x_gm;


/*!                                                                 */
/* The Y axis is the vector product  vecz X vecx.                   */
/*                                                                 !*/


   vecy.x_gm = vecz.y_gm*vecx.z_gm - vecz.z_gm*vecx.y_gm;
   vecy.y_gm = vecz.z_gm*vecx.x_gm - vecz.x_gm*vecx.z_gm;
   vecy.z_gm = vecz.x_gm*vecx.y_gm - vecz.y_gm*vecx.x_gm;

/*!                                                                 */
/* Calculate local system defined by origin, vecx and vecy          */
/* Call of GEmktf_3p.                                               */
/*                                                                 !*/

    status = GEmktf_3p (&origin,&vecx,&vecy, &spine_csys);
if (status<0) 
  {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229*t_csys GEmktf_3p failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
  sprintf(errbuf,"varkon_GEmktf_3p%%sur229*t_csys");
  return(varkon_erpush("SU2943",errbuf));
  }

/*!                                                                 */
/* The output transformation matrix is the inverted spine system    */
/* Call of GEtform_inv.                                             */
/*                                                                 !*/

status=GEtform_inv (&spine_csys, p_tra_csys );
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur229*t_csys GEtform_inv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"GEtform_inv%%sur229t_csys");
   return(varkon_erpush("SU2943",errbuf));
   }



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys l_tang %f *p_t_eps %f\n",l_tang, *p_t_eps );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys origin %f %f %f\n",origin.x_gm,origin.y_gm,origin.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys vecx   %f %f %f\n",vecx.x_gm,vecx.y_gm,vecx.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys vecy   %f %f %f\n",vecy.x_gm,vecy.y_gm,vecy.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys vecz   %f %f %f\n",vecz.x_gm,vecz.y_gm,vecz.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*t_csys yaxis  %f %f %f\n",yaxis.x_gm,yaxis.y_gm,yaxis.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur229 Transformation matrix \n");
fprintf(dbgfil(SURPAC),"sur229 g11 %f g12 %f g13 %f g14 %f \n",
   p_tra_csys->g11,p_tra_csys->g12,p_tra_csys->g13,p_tra_csys->g14);
fprintf(dbgfil(SURPAC),"sur229 g21 %f g22 %f g23 %f g24 %f \n",
   p_tra_csys->g21,p_tra_csys->g22,p_tra_csys->g23,p_tra_csys->g24);
fprintf(dbgfil(SURPAC),"sur229 g31 %f g32 %f g33 %f g34 %f \n",
   p_tra_csys->g31,p_tra_csys->g32,p_tra_csys->g33,p_tra_csys->g34);
fprintf(dbgfil(SURPAC),"sur229 g41 %f g42 %f g43 %f g44 %f \n",
   p_tra_csys->g41,p_tra_csys->g42,p_tra_csys->g43,p_tra_csys->g44);
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
/* Calculate surface derivatives                                    */

   static short sderiv ( p_patl, v_pat, p_xyz  )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz_l;       /* Section crv coord's deriv's local system*/
   DBVector r_p;         /* Coordinates         r(u)                */
   DBVector drdu;        /* First  derivative  dr/du                */
   DBVector drdv;        /* First  derivative  dr/dv                */
   DBVector d2rdu2;      /* Second derivative d2r/du2               */
   DBVector d2rdv2;      /* Second derivative d2r/dv2               */
   DBVector d2rduv;      /* Second derivative d2r/dudv              */
/*-----------------------------------------------------------------!*/
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from a called function       */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 1. Calculate curve coordinates and derivatives for the section   */
/*    curve in local coordinates.                                   */
/*    Call of GE109                                                 */
/*                                                                 !*/


   scur.hed_cu.type    = CURTYP;
   scur.ns_cu          = 1;      
   scur.plank_cu       = FALSE;      

   xyz_l.evltyp        = EVC_D2R; 
   xyz_l.t_global      = 1.0 + v_pat; 


status=GE109 ((DBAny *)&scur, &p_patl->lims, &xyz_l );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv lims_t xyz_l.r    %f %f %f\n",
xyz_l.r.x_gm,xyz_l.r.y_gm,xyz_l.r.z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 2. Transformate with derivative transformation matrices.         */
/*    Calls of GEtfpos_to_local and GEtfvec_to_local                */
/*                                                                 !*/

status=GEtfpos_to_local (&xyz_l.r , &tra_csys , &r_p);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif


status=GEtfpos_to_local (&xyz_l.r , &tra_csys_du , &drdu);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif


status=GEtfvec_to_local (&xyz_l.drdt , &tra_csys , &drdv);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

status=GEtfpos_to_local (&xyz_l.r , &tra_csys_du2 , &d2rdu2);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

status=GEtfvec_to_local (&xyz_l.d2rdt2 , &tra_csys , &d2rdv2);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

status=GEtfvec_to_local (&xyz_l.drdt , &tra_csys_du , &d2rduv);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur229*sderiv");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv r        %f %f %f\n",
    r_p.x_gm,    r_p.y_gm,    r_p.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv drdu     %f %f %f\n",
drdu.x_gm,drdu.y_gm,drdu.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv drdv     %f %f %f\n",
   drdv.x_gm,   drdv.y_gm,   drdv.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv d2rdu2   %f %f %f\n",
 d2rdu2.x_gm, d2rdu2.y_gm, d2rdu2.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv d2rdv2   %f %f %f\n",
 d2rdv2.x_gm, d2rdv2.y_gm, d2rdv2.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur229*sderiv d2rduv   %f %f %f\n",
d2rduv.x_gm,d2rduv.y_gm,d2rduv.z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Output derivatives                                            */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x=     r_p.x_gm;
    p_xyz->r_y=     r_p.y_gm;
    p_xyz->r_z=     r_p.z_gm;

/*  Tangent             dr/du                                       */
    p_xyz->u_x=    drdu.x_gm;
    p_xyz->u_y=    drdu.y_gm;
    p_xyz->u_z=    drdu.z_gm;

/*  Tangent             dr/dv                                       */
    p_xyz->v_x=    drdv.x_gm;
    p_xyz->v_y=    drdv.y_gm;
    p_xyz->v_z=    drdv.z_gm;

/*  Second derivative d2r/du2                                       */
    p_xyz->u2_x=  d2rdu2.x_gm;
    p_xyz->u2_y=  d2rdu2.y_gm;
    p_xyz->u2_z=  d2rdu2.z_gm;

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x=  d2rdv2.x_gm;
    p_xyz->v2_y=  d2rdv2.y_gm;
    p_xyz->v2_z=  d2rdv2.z_gm;

/*  Second derivative  d2r/duv                                      */
    p_xyz->uv_x=  d2rduv.x_gm;
    p_xyz->uv_y=  d2rduv.y_gm;
    p_xyz->uv_z=  d2rduv.z_gm;


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate U derivative matrices dTdu and d2Tdu2                  */

   static short dermat(t_eps,p_tra0_csys,p_tra1_csys,p_tra2_csys)

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat t_eps;        /* Epsilon for parameter                   */
   DBTmat *p_tra0_csys;  /* Transformation system             (ptr) */
   DBTmat *p_tra1_csys;  /* Transformation system     eps     (ptr) */
   DBTmat *p_tra2_csys;  /* Transformation system   2*eps     (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBTmat tra_csys_eu;   /* Transformation  dT/du  for 2*eps        */
/*-----------------------------------------------------------------!*/
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* First derivative of transformation matrix.                       */
/*                                                                 !*/

  tra_csys_du.g11 = ( p_tra1_csys->g11 - p_tra0_csys->g11)/t_eps;
  tra_csys_du.g12 = ( p_tra1_csys->g12 - p_tra0_csys->g12)/t_eps;
  tra_csys_du.g13 = ( p_tra1_csys->g13 - p_tra0_csys->g13)/t_eps;
  tra_csys_du.g14 = ( p_tra1_csys->g14 - p_tra0_csys->g14)/t_eps;

  tra_csys_du.g21 = ( p_tra1_csys->g21 - p_tra0_csys->g21)/t_eps;
  tra_csys_du.g22 = ( p_tra1_csys->g22 - p_tra0_csys->g22)/t_eps;
  tra_csys_du.g23 = ( p_tra1_csys->g23 - p_tra0_csys->g23)/t_eps;
  tra_csys_du.g24 = ( p_tra1_csys->g24 - p_tra0_csys->g24)/t_eps;

  tra_csys_du.g31 = ( p_tra1_csys->g31 - p_tra0_csys->g31)/t_eps;
  tra_csys_du.g32 = ( p_tra1_csys->g32 - p_tra0_csys->g32)/t_eps;
  tra_csys_du.g33 = ( p_tra1_csys->g33 - p_tra0_csys->g33)/t_eps;
  tra_csys_du.g34 = ( p_tra1_csys->g34 - p_tra0_csys->g34)/t_eps;

  tra_csys_du.g41 = 0.0;
  tra_csys_du.g42 = 0.0;
  tra_csys_du.g43 = 0.0;
  tra_csys_du.g44 = 1.0;


  tra_csys_eu.g11 = ( p_tra2_csys->g11 - p_tra1_csys->g11)/t_eps;
  tra_csys_eu.g12 = ( p_tra2_csys->g12 - p_tra1_csys->g12)/t_eps;
  tra_csys_eu.g13 = ( p_tra2_csys->g13 - p_tra1_csys->g13)/t_eps;
  tra_csys_eu.g14 = ( p_tra2_csys->g14 - p_tra1_csys->g14)/t_eps;

  tra_csys_eu.g21 = ( p_tra2_csys->g21 - p_tra1_csys->g21)/t_eps;
  tra_csys_eu.g22 = ( p_tra2_csys->g22 - p_tra1_csys->g22)/t_eps;
  tra_csys_eu.g23 = ( p_tra2_csys->g23 - p_tra1_csys->g23)/t_eps;
  tra_csys_eu.g24 = ( p_tra2_csys->g24 - p_tra1_csys->g24)/t_eps;

  tra_csys_eu.g31 = ( p_tra2_csys->g31 - p_tra1_csys->g31)/t_eps;
  tra_csys_eu.g32 = ( p_tra2_csys->g32 - p_tra1_csys->g32)/t_eps;
  tra_csys_eu.g33 = ( p_tra2_csys->g33 - p_tra1_csys->g33)/t_eps;
  tra_csys_eu.g34 = ( p_tra2_csys->g34 - p_tra1_csys->g34)/t_eps;

  tra_csys_eu.g41 = 0.0;
  tra_csys_eu.g42 = 0.0;
  tra_csys_eu.g43 = 0.0;
  tra_csys_eu.g44 = 1.0;

  tra_csys_du2.g11 = (  tra_csys_du.g11 -  tra_csys_eu.g11)/t_eps;
  tra_csys_du2.g12 = (  tra_csys_du.g12 -  tra_csys_eu.g12)/t_eps;
  tra_csys_du2.g13 = (  tra_csys_du.g13 -  tra_csys_eu.g13)/t_eps;
  tra_csys_du2.g14 = (  tra_csys_du.g14 -  tra_csys_eu.g14)/t_eps;

  tra_csys_du2.g21 = (  tra_csys_du.g21 -  tra_csys_eu.g21)/t_eps;
  tra_csys_du2.g22 = (  tra_csys_du.g22 -  tra_csys_eu.g22)/t_eps;
  tra_csys_du2.g23 = (  tra_csys_du.g23 -  tra_csys_eu.g23)/t_eps;
  tra_csys_du2.g24 = (  tra_csys_du.g24 -  tra_csys_eu.g24)/t_eps;

  tra_csys_du2.g31 = (  tra_csys_du.g31 -  tra_csys_eu.g31)/t_eps;
  tra_csys_du2.g32 = (  tra_csys_du.g32 -  tra_csys_eu.g32)/t_eps;
  tra_csys_du2.g33 = (  tra_csys_du.g33 -  tra_csys_eu.g33)/t_eps;
  tra_csys_du2.g34 = (  tra_csys_du.g34 -  tra_csys_eu.g34)/t_eps;

  tra_csys_du2.g41 = 0.0;
  tra_csys_du2.g42 = 0.0;
  tra_csys_du2.g43 = 0.0;
  tra_csys_du2.g44 = 1.0;


#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur229 Transformation derivative dT/du \n");
fprintf(dbgfil(SURPAC),"sur229 g11 %f g12 %f g13 %f g14 %f \n",
   tra_csys_du.g11,tra_csys_du.g12,tra_csys_du.g13,tra_csys_du.g14);
fprintf(dbgfil(SURPAC),"sur229 g21 %f g22 %f g23 %f g24 %f \n",
   tra_csys_du.g21,tra_csys_du.g22,tra_csys_du.g23,tra_csys_du.g24);
fprintf(dbgfil(SURPAC),"sur229 g31 %f g32 %f g33 %f g34 %f \n",
   tra_csys_du.g31,tra_csys_du.g32,tra_csys_du.g33,tra_csys_du.g34);
fprintf(dbgfil(SURPAC),"sur229 g41 %f g42 %f g43 %f g44 %f \n",
   tra_csys_du.g41,tra_csys_du.g42,tra_csys_du.g43,tra_csys_du.g44);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur229 Transformation derivative dT/du 2*eps\n");
fprintf(dbgfil(SURPAC),"sur229 g11 %f g12 %f g13 %f g14 %f \n",
   tra_csys_eu.g11,tra_csys_eu.g12,tra_csys_eu.g13,tra_csys_eu.g14);
fprintf(dbgfil(SURPAC),"sur229 g21 %f g22 %f g23 %f g24 %f \n",
   tra_csys_eu.g21,tra_csys_eu.g22,tra_csys_eu.g23,tra_csys_eu.g24);
fprintf(dbgfil(SURPAC),"sur229 g31 %f g32 %f g33 %f g34 %f \n",
   tra_csys_eu.g31,tra_csys_eu.g32,tra_csys_eu.g33,tra_csys_eu.g34);
fprintf(dbgfil(SURPAC),"sur229 g41 %f g42 %f g43 %f g44 %f \n",
   tra_csys_eu.g41,tra_csys_eu.g42,tra_csys_eu.g43,tra_csys_eu.g44);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur229 Transformation derivative dT2/du2\n");
fprintf(dbgfil(SURPAC),"sur229 g11 %f g12 %f g13 %f g14 %f \n",
tra_csys_du2.g11,tra_csys_du2.g12,tra_csys_du2.g13,tra_csys_du2.g14);
fprintf(dbgfil(SURPAC),"sur229 g21 %f g22 %f g23 %f g24 %f \n",
tra_csys_du2.g21,tra_csys_du2.g22,tra_csys_du2.g23,tra_csys_du2.g24);
fprintf(dbgfil(SURPAC),"sur229 g31 %f g32 %f g33 %f g34 %f \n",
tra_csys_du2.g31,tra_csys_du2.g32,tra_csys_du2.g33,tra_csys_du2.g34);
fprintf(dbgfil(SURPAC),"sur229 g41 %f g42 %f g43 %f g44 %f \n",
tra_csys_du2.g41,tra_csys_du2.g42,tra_csys_du2.g43,tra_csys_du2.g44);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur229*dermat  Exit \n");
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



