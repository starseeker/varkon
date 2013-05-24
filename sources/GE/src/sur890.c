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
/*  Function: varkon_sur_nurbsarr                  File: sur890.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a surface from control points              */
/*  and knot vectors. Input to the function are arrays              */
/*  with points, knot values and weights.                           */
/*                                                                  */
/*  This function corresponds to the SUR_NURBSARR MBS function.     */
/*                                                                  */
/*  Arrays with data are considered to be undefined if all          */
/*  data is zero.                                                   */
/*                                                                  */
/*  Order of points in input control point array poi:               */
/*                                                                  */
/*   V                                                              */
/* !         np_u= 7  and  np_v= 3                                  */
/* !                                                                */
/* !                                                                */
/* !   3     6     9    12    15    18    21  -->- V isoparam 3     */
/* !                                                                */
/* !   2     5     8    11    14    17    20  -->- V isoparam 2     */
/* !                                                                */
/* !   1     4     7    10    13    16    19  -->- V isoparam 1     */
/* !                                                                */
/* !   !     !     !     !     !     !     !                        */
/* !   !     !     !     !     !     !     !                        */
/* !   v     v     v     v     v     v     v                        */
/* !   !     !     !     !     !     !     !                        */
/* !                                                                */
/* !   U     U     U     U     U     U     U                        */
/* !                                                                */
/* !   i     i     i     i     i     i     i                        */
/* !   s     s     s     s     s     s     s                        */
/* !   o     o     o     o     o     o     o                        */
/* !   p     p     p     p     p     p     p                        */
/* !   a     a     a     a     a     a     a                        */
/* !   m     m     m     m     m     m     m                        */
/* !                                                                */
/* !   1     2     3     4     5     6     7                        */
/* !                                                                */
/*  ------------------------------------------------------------> U */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-21   Originally written                                 */
/*  1998-01-02   fflush, check of order                             */
/*  1998-01-06   offset, sur757 added                               */
/*  1998-01-11   Check multiplicity and weights > 0, idknot         */
/*  1998-02-15   Check nk_u and nk_v < 2 bug fixed, sur892 added    */
/*               Declaration sur243 DBint -> short                  */
/*               Call of sur893 and sur 891                         */
/*  1998-03-19   knotdef added                                      */
/*  1998-03-28   No change of input knot vectors                    */
/*  1999-12-04   Free source code modifications                     */
/*  2003-02-18   Buggfix, rational surfaces, Sören Larsson          */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short top_pat();       /* Create all topology patches        */
static short knotdef();       /* Calculate knot vectors             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBPatch *p_toppat;         /* Topology (NURBS) patch (ptr)   */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
static DBfloat  idknot;           /* Identical knots criterion      */
static DBfloat  u_knots_u[1000];  /* U knots (if not input)         */
static DBfloat  v_knots_u[1000];  /* V knots (if not input)         */
/*-----------------------------------------------------------------!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nurbsarr   Create NURB_SUR with SUR_NURBSARR*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer  tolerance           */
/* varkon_idknot           * Retrieve parameter tolerance           */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_ini_gmpatnu      * Initialize GMPATNU                     */
/* varkon_sur_nurbsche     * Check NURBS data                       */
/* varkon_sur_nurbsnon     * Calculate number of nodes              */
/* varkon_sur_nurbsacp     * Is NURBS closed/periodic ?             */
/* varkon_poi_nurbs        * Get address to a NURBS node            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_nurbsarr        */
/* SU2993 = Severe program error (  ) in varkon_sur_nurbsarr        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

      DBstatus        varkon_sur_nurbsarr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  
  DBint     order_u,     /* Order of surface in U direction         */
  DBint     order_v,     /* Order of surface in V direction         */
  DBint     nk_u,        /* Number of knot values in U direction    */
  DBint     nk_v,        /* Number of knot values in V direction    */
  DBVector *p_cpts,      /* Control points (nodes)            (ptr) */
  DBfloat  *p_uknots,    /* Knot vector in U direction        (ptr) */
  DBfloat  *p_vknots,    /* Knot vector in V direction        (ptr) */
  DBfloat  *p_w,         /* Weights for the control points    (ptr) */
  DBSurf   *p_surout,    /* Output surface                    (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    np_u;         /* Number of control points in U direction */
  DBint    np_v;         /* Number of control points in V direction */
  DBPatch *p_b;          /* Current topological (NURBS) patch (ptr) */
  GMPATNU *p_patnu;      /* NURBS patch                       (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

                         /* For varkon_sur_nurbsche (sur893):       */
  DBint     f_surcur;    /* Flag for surface or curve check;        */
                         /* Eq. 1: Surface   Eq. 2: Curve           */
  DBint     error_code;  /* Error code                              */

  DBint     degree_u;    /* Degree in U direction                   */
  DBint     degree_v;    /* Degree in V direction                   */
  DBfloat  *p_w_mod;     /* =p_w but changed to NULL for      (ptr) */
                         /* weights which all are one (1).          */
                         /* For varkon_poi_nurbs (sur243)           */
  DBint     rwcase;      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of node    points       */
                         /* Eq. 4: Check NURBS data                 */
  DBint     i_up;        /* Point address in U direction            */
  DBint     i_vp;        /* Point address in V direction            */
  DBHvector node;        /* Node    point                           */

  DBint     i_u;         /* Loop index U knot values                */
  DBint     j_v;         /* Loop index V knot values                */
  DBfloat  *p_uval;      /* U knot value                      (ptr) */
  DBfloat  *p_vval;      /* V knot value                      (ptr) */

  DBint     acc_box;     /* For sur911                              */
  char     *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
  DBint     status;      /* Error code from called function         */

  char      errbuf[80];  /* String for error message fctn erpush    */

  DBint     i_w;         /* Loop index Weight                       */
  DBint     w_flag;      /* Rational flag                           */
  DBfloat   weight;      /* Weight                                  */

#ifdef  DEBUG        
  DBint     form_u;      /* Closed/periodic flag in U               */
  DBint     form_v;      /* Closed/periodic flag in V               */
                         /* Eq. 0: Not closed and periodic          */
                         /* Eq. 1: Closed                           */
                         /* Eq. 2: Closed and periodic              */
  DBint   i;             /* Temporarely used loop index             */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

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
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 Enter***varkon_sur_nurbsarr\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur890 order_u= %d, order_v= %d\n",
       (int)order_u,(int)order_v);
fprintf(dbgfil(SURPAC),"sur890 nk_u= %d, nk_v= %d\n",
       (int)nk_u,(int)nk_v);
}
#endif

/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/


*pp_patout = NULL; 

/* Local pointer for the case that all weights are one (1.0)        */
   p_w_mod = p_w;


/*!                                                                 */
/* Calculate number of nodes.  Call of varkon_sur_nurbsnon (sur892) */
/*                                                                 !*/

    status= varkon_sur_nurbsnon
    (order_u, order_v, nk_u, nk_v, 
     &np_u,  &np_v,  &degree_u,  &degree_v );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur892%%sur890");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* Calculate knotvectors if not defined.                            */
/* Call of internal function knotdef.                               */
/*                                                                 !*/


  if ( p_uknots == NULL && p_vknots == NULL) 
    {
      status= knotdef
     (order_u, order_v, nk_u, nk_v, np_u, np_v, &p_uknots, &p_vknots);
      if  ( status < 0 ) 
        {
        sprintf(errbuf, "knotdef%%sur890");
        return(varkon_erpush("SU2973",errbuf));
        }
     }

/*!                                                                 */
/* Check the input NURBS data. Call of varkon_sur_nurbsche (sur893) */
/*                                                                 !*/

    f_surcur = 1;

    status= varkon_sur_nurbsche
    (f_surcur, order_u, order_v, nk_u, nk_v, p_cpts, 
              p_uknots, p_vknots, p_w,  &error_code );
   if  ( status < 0 ) 
     {
#ifdef DEBUG  /* Error message for Debug On  */
     sprintf(errbuf, "sur893%%sur890");
     return(varkon_erpush("SU2943",errbuf));
#else  /* No error message for Debug Off */
     return(status);
#endif /* End DEBUG */
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 Input NURBS data is checked error_code= %d\n",
               (int)error_code  );
fflush(dbgfil(SURPAC));
}
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
if ( p_uknots != NULL ) for ( i=0; i<nk_u; ++i )
  fprintf(dbgfil(SURPAC),"sur890 U-knot%d=%g\n",
       (int)i,*(p_uknots+i));
if ( p_vknots != NULL ) for ( i=0; i<nk_v; ++i )
  fprintf(dbgfil(SURPAC),"sur890 V-knot%d=%g\n",
       (int)i,*(p_vknots+i));
if ( p_w != NULL ) for ( i=0; i<np_u*np_v; ++i )
  fprintf(dbgfil(SURPAC),"sur890 w%d=%g\n",
       (int)i,*(p_w+i));

for ( i=0; i<np_u*np_v; ++i )
  {
  fprintf(dbgfil(SURPAC),"sur890 p%d= %g,%g,%g\n",
     (int)i,(double)(p_cpts+i)->x_gm,
    (double)(p_cpts+i)->y_gm,(double)(p_cpts+i)->z_gm);
  }

fflush(dbgfil(SURPAC));
}
#endif



/*!                                                                 */
/* Initialize local variables.                                      */
/*                                                                 !*/

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Identical knots criterion. Call of varkon_idknot (sur744).       */
/*                                                                 !*/

   idknot =varkon_idknot();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
weight = -0.123456789;
fprintf(dbgfil(SURPAC),
"sur890 comptol %18.15f idknot %14.10f\n",
comptol, idknot  );
}
#endif

/*!                                                                 */
/* Initialize surface data in p_surout                              */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);


/*!                                                                 */
/* Check if the weights are equal to one (1.0). Tolerance comptol.  */
/* Change p_w_mod to NULL in this case.                             */
/* The flag can be changed also for any constant value. Normally    */
/* (the standard in interface output files) is however that the     */
/* weights will be one (1.0) for a non-rational surface.            */
/* Weights with another constant value than one (1) can be of       */
/* interest for tests of derivative calculations ....               */
/*                                                                 !*/

   if  ( p_w != NULL )
     { 
     w_flag = 0;
     for ( i_w=0; i_w<np_u*np_v; ++i_w )
       {
       weight=   *(p_w+i_w);
       if ( fabs(weight-1.0) > comptol ) 
         {
         w_flag= 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur890 fabs(weight-1.0) %14.10f comptol %14.10f\n",
fabs(weight-1.0), comptol );
}
#endif
         } /* End weight is not one (1.0) */
       } /* End loop weights */
       if  (  w_flag == 0 )
         {
         p_w_mod = NULL; 
         }
     } /* End weights are defined */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if ( p_w != NULL && p_w_mod == NULL )
{
fprintf(dbgfil(SURPAC),
"sur890 Weights are defined but all are equal to one (1.0)\n");
fprintf(dbgfil(SURPAC),
"sur890 Calculations will be made as for a non-rational\n");
fprintf(dbgfil(SURPAC),
"sur890 surface (p_w_mod = NULL)\n");
fflush(dbgfil(SURPAC));
}
if ( p_w != NULL && p_w_mod != NULL )
{
fprintf(dbgfil(SURPAC),
"sur890 Weights are defined and are not equal to one (1.0)\n");
fprintf(dbgfil(SURPAC),
"sur890 Surface is handled as a rational surface\n");
fflush(dbgfil(SURPAC));
}
}
#endif

/*!                                                                 */
/* For Debug On:                                                    */
/* Determine if the NURBS surface is closed and periodic.           */
/* Call of varkon_sur_nurbsacp (sur891)                             */
/*                                                                 !*/

#ifdef  DEBUG
    f_surcur = 1;

    status= varkon_sur_nurbsacp
     (f_surcur, order_u, order_v, nk_u, nk_v, p_cpts,
                p_uknots, p_vknots, p_w,
                  &form_u,  &form_v );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur893%%sur890");
     return(varkon_erpush("SU2943",errbuf));
     }

if ( dbglev(SURPAC) == 1 && form_u == 0 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is not closed or periodic in U form_u= %d\n",
      (int)form_u);
fflush(dbgfil(SURPAC));
}
else if ( dbglev(SURPAC) == 1 && form_u == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is closed in U form_u= %d\n",
      (int)form_u);
fflush(dbgfil(SURPAC));
}
else if ( dbglev(SURPAC) == 1 && form_u == 2 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is closed and periodic in U form_u= %d\n",
      (int)form_u);
fflush(dbgfil(SURPAC));
}
else
{
fprintf(dbgfil(SURPAC),
"sur890 Not a valid U value from sur891 form_u= %d\n",
      (int)form_u);
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 && form_v == 0 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is not closed or periodic in V form_v= %d\n",
      (int)form_v);
fflush(dbgfil(SURPAC));
}
else if ( dbglev(SURPAC) == 1 && form_v == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is closed in V form_v= %d\n",
      (int)form_u);
fflush(dbgfil(SURPAC));
}
else if ( dbglev(SURPAC) == 1 && form_v == 2 )
{
fprintf(dbgfil(SURPAC),
"sur890 NURBS surface is closed and periodic in V form_v= %d\n",
      (int)form_v);
fflush(dbgfil(SURPAC));
}
else
{
fprintf(dbgfil(SURPAC),
"sur890 Not a valid V value from sur891 form_v= %d\n",
      (int)form_v);
fflush(dbgfil(SURPAC));
}

#endif /* End DEBUG */



/*!                                                                 */
/* 2. Dynamic allocation of area for patches                        */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Surface header data                                              */
/*                                                                 !*/
   p_surout->typ_su = NURB_SUR;

/*!                                                                 */
/* Dynamic allocation of area for one geometric NURBS surface/patch */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(NURB_PAT, 1 )) == NULL )
 { 
 sprintf(errbuf, "(allocg)%%sur890");
 return(varkon_erpush("SU2993",errbuf));
 }

/* Current geometric patch */
   p_patnu = (GMPATNU *)p_gpat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 p_gpat %d p_patnu %d\n",
  (int)p_gpat ,(int)p_patnu);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize the NURBS patch                                       */
/* Call of varkon_ini_gmpatnu (sur757).                             */
/*                                                                 !*/
    varkon_ini_gmpatnu (p_patnu);    

/*!                                                                 */
/*  Number of knot values and degree to the geometric surface/patch */
/*                                                                 !*/


    p_patnu->nk_u    = nk_u;
    p_patnu->nk_v    = nk_v;
    p_patnu->order_u = order_u; 
    p_patnu->order_v = order_v;
    p_patnu->ofs_pat = 0.0;


/*!                                                                 */
/*  Allocate memory for node    points and knot vectors.            */
/*  Call of DBcreate_NURBS.                                         */
/*                                                                 !*/

    DBcreate_NURBS(p_patnu); 


/*!                                                                 */
/*  Initialize NURBS nodes.                                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

    rwcase = 2;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 init.%%sur890");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* 3. Data from input arrays to surface (allocated area)            */
/* _____________________________________________________            */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Knot vector values to allocated area for the surface.            */
/*                                                                 !*/

/* Programming check for Debug On                                   */
#ifdef  DEBUG
   if  (  p_uknots == NULL ||  p_vknots == NULL )
     {
     sprintf(errbuf, "Undef. knot vetors%%sur890");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif


   for ( i_u = 0; i_u < nk_u; ++i_u )
     {
     p_uval    = p_patnu->kvec_u+i_u;
     (*p_uval) = *(p_uknots+i_u);
     }

   for ( j_v = 0; j_v < nk_v; ++j_v )
     {
     p_vval    = p_patnu->kvec_v+j_v;
     (*p_vval) = *(p_vknots+j_v);
     }

/*!                                                                 */
/* Node    points                                                   */
/*                                                                 !*/

   rwcase = 1;

for  ( i_up = 0; i_up < np_u;  i_up++)
  {
  for  ( i_vp = 0; i_vp < np_v;  i_vp++)
    {




/* start soren buggfix 2003-02-18 */   
    
    /* --- OLD CODE, before buggfix -----
    
    node.x_gm = (p_cpts+np_v*i_up+i_vp)->x_gm;
    node.y_gm = (p_cpts+np_v*i_up+i_vp)->y_gm;
    node.z_gm = (p_cpts+np_v*i_up+i_vp)->z_gm;
    if (  p_w_mod == NULL ) node.w_gm = 1.0;
    else  node.w_gm = *(p_w_mod+np_v*i_up+i_vp);  */
    
    
    /* --- NEW CODE, after buggfix ----- */
    
    if (  p_w_mod == NULL ) 
       {
       node.w_gm = 1.0;
       node.x_gm = (p_cpts+np_v*i_up+i_vp)->x_gm;
       node.y_gm = (p_cpts+np_v*i_up+i_vp)->y_gm;
       node.z_gm = (p_cpts+np_v*i_up+i_vp)->z_gm;      
       }
  
    else  
       {
       node.w_gm = *(p_w_mod+np_v*i_up+i_vp);
       node.x_gm = (p_cpts+np_v*i_up+i_vp)->x_gm * node.w_gm;
       node.y_gm = (p_cpts+np_v*i_up+i_vp)->y_gm * node.w_gm;
       node.z_gm = (p_cpts+np_v*i_up+i_vp)->z_gm * node.w_gm;           
       }
/* end soren soren buggfix 2003-02-18 */

    
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
    
    if  ( status < 0 ) 
       {
       sprintf(errbuf, "sur243*putnode r03%%sur172");
       return(varkon_erpush("SU2943",errbuf));
       }
    }
  }

/*!                                                                 */
/*  Printout of node    points for Debug On                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

#ifdef  DEBUG
    rwcase = 3;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 init.%%sur890");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif


/*!                                                                 */
/* 5. Create all the topology patches for the surface               */
/* __________________________________________________               */
/*                                                                  */
/*  Call of internal function top_pat.                              */
/*                                                                 !*/

   status= top_pat( p_surout, p_patnu );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "top_pat%%sur890");
     return(varkon_erpush("SU2973",errbuf));
     }

   *pp_patout = p_toppat;


/* Local pointer p_b= first topology patch.                         */

    p_b = *pp_patout;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890 Pointer to topology patches p_toppat %d *pp_patout   %d\n", 
 (int)p_toppat, (int)(*pp_patout  ) ); 
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890 Calculate bounding boxes and cones p_surout %d p_b %d\n"
         , (int)p_surout, (int)p_b );
  fflush(dbgfil(SURPAC));
  }
#endif

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout ,p_b ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_nurbsarr   ");
        return(varkon_erpush("SU2943",errbuf));
        }


/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890 Exit***varkon_sur_nurbsarr    ** Output nu %d nv %d type %d\n"
    , (int)p_surout->nu_su,(int)p_surout->nv_su, (int)p_surout->typ_su);
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** top_pat ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Create all topology patches for the NURBS surface             */

    static short top_pat (p_sur_nurb, p_patnu)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf   *p_sur_nurb;  /* Surface of type NURB_SUR          (ptr) */
  GMPATNU  *p_patnu;     /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
  DBint     maxnum;      /* Maximum number of patches (alloc. area) */
  char     *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
  char     *p_tpat;      /* Allocated area topol. patch data  (ptr) */
  DBPatch  *p_b;         /* Current topological (NURBS) patch (ptr) */
  DBint    nu_nurb;      /* Number of NURBS U patches               */
  DBint    nv_nurb;      /* Number of NURBS V patches               */
  DBfloat  u_knot;       /* U knot value, current                   */
  DBfloat  v_knot;       /* V knot value, current                   */
  DBfloat  u_knot_pre;   /* U knot value, previous                  */
  DBfloat  v_knot_pre;   /* V knot value, previous                  */
  DBfloat *p_uval;       /* U knot value                      (ptr) */
  DBfloat *p_vval;       /* V knot value                      (ptr) */
  DBint    i_u;          /* Loop index U knot values                */
  DBint    j_v;          /* Loop index V knot values                */
  DBfloat  u_val[1000];  /* U values for patches                    */
  DBfloat  v_val[1000];  /* V values for patches                    */
/*                                                                  */
/*----------------------------------------------------------------- */

  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat Enter  Number of U knots %d\n",
  (int)p_patnu->nk_u);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/* Inititialize the output topology (NURBS) patch pointer           */
   p_toppat = NULL;
   p_b      = NULL;

/*!                                                                 */
/* 1. Determine the number of patches                               */
/*                                                                 !*/

   nu_nurb = 0;
   for ( i_u = 0; i_u < p_patnu->nk_u; ++i_u )
     {
      p_uval = p_patnu->kvec_u+i_u;
      u_knot = (*p_uval);
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat i_u= %d u_knot %f\n",(int)i_u,u_knot);
fflush(dbgfil(SURPAC));
}
#endif
     if  ( i_u == 0 ) 
        {
         u_knot_pre    = u_knot;
         u_val[nu_nurb] = u_knot;
         }
     else
       {
       if ( fabs(u_knot_pre-u_knot) > 0.0001 )
         {
         nu_nurb    = nu_nurb + 1;
         if ( nu_nurb > 999 )
           { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890*top_pat Increase u_val array size \n");
  }
#endif
 sprintf(errbuf, "(u_val)%%sur890*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
         }
         u_val[nu_nurb] = u_knot;
         u_knot_pre = u_knot;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat i_u= %d nu_nurb %d u_knot %f\n",
  (int)i_u,(int)nu_nurb, u_knot);
fflush(dbgfil(SURPAC));
}
#endif
         }
       }

     } /* End loop i_u */


   nv_nurb = 0;
   for ( j_v = 0; j_v < p_patnu->nk_v; ++j_v )
     {
      p_vval = p_patnu->kvec_v+j_v;
      v_knot = (*p_vval);
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat j_v= %d v_knot %f\n",(int)j_v,v_knot);
fflush(dbgfil(SURPAC));
}
#endif
     if  ( j_v == 0 )
        {
         v_knot_pre    = v_knot;
         v_val[nv_nurb] = v_knot;
         }
     else
       {
       if ( fabs(v_knot_pre-v_knot) > 0.0001 )
         {
         nv_nurb    = nv_nurb + 1;
         if ( nv_nurb > 999 )
           { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890*top_pat Increase v_val array size \n");
  }
#endif
 sprintf(errbuf, "(v_val)%%sur890*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
         }
         v_val[nv_nurb] = v_knot;
         v_knot_pre = v_knot;
         }
       }
     } /* End loop j_v */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat Number of topology patches nu_nurb %d nv_nurb %d \n",
  (int)nu_nurb,(int)nv_nurb);
fflush(dbgfil(SURPAC));
}
#endif

 if  (  nu_nurb == 0 ||  nv_nurb == 0)
 { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890*top_pat nu_nurb %d or nv_nurb %d is zero\n", 
         (int)nu_nurb, (int)nv_nurb );
  }
#endif
 sprintf(errbuf,
 "(nu_nurb*nv_nurb=0)%%sur890*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
 } 

/*!                                                                 */
/* Number of patches to surface header                              */
/*                                                                  */
/*                                                                 !*/
   p_sur_nurb->nu_su  = nu_nurb;
   p_sur_nurb->nv_su  = nv_nurb;

/*!                                                                 */
/* 2. Dynamic allocation of area for patches                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the surface data,        */
/* i.e. for the surface topology patches.                           */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

   maxnum = nu_nurb*nv_nurb;        

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur890*top_pat Allocation failed  maxnum %d\n", (short)maxnum );
  }
#endif
 sprintf(errbuf,
 "(alloc)%%sur890*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
 } 

/*!                                                                 */
/* Pointer to the first patch (static variable)                     */
/*                                                                 !*/

   p_toppat = (DBPatch*)p_tpat;


/*!                                                                 */
/* Dynamic allocation of area for one geometric NURBS surface/patch */
/* is already made. Vonvert PATNU pointer to char pointer.          */
/*                                                                 !*/

   p_gpat = (char *) p_patnu;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat *p_toppat %d p_gpat %d Area allocated for %d patches\n", 
(int)p_toppat, (int)p_gpat, (int)maxnum);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Data to the topology patches                                  */
/*                                                                 !*/

   for ( j_v = 0; j_v < nv_nurb; ++j_v )
     {
     for ( i_u = 0; i_u < nu_nurb; ++i_u )
       {

/*     Ptr to  current patch                                        */
       p_b = p_toppat + i_u*nv_nurb + j_v;

/*     Initialize                                                   */
       varkon_ini_gmpat  (p_b); 

/*     All topology data                                            */

       if  ( i_u == 0 && j_v == 0 )
         {
         p_b->styp_pat = NURB_PAT;       /* Type of secondary patch */
         p_b->spek_c   = p_gpat;         /* Secondary patch (C ptr) */
         p_b->su_pat   = 0;              /* Topological adress      */
         p_b->sv_pat   = 0;              /* secondary patch not def.*/
         }
       else                              
         {
         p_b->styp_pat = TOP_PAT;        /* Type of secondary patch */
         p_b->spek_c   = (char *)p_b;    /* Secondary patch (C ptr) */
         p_b->su_pat   = (short)(1);     /* Topological adress      */
         p_b->sv_pat   = (short)(1);     /* secondary patch.        */
         }
       p_b->iu_pat   = (short)(i_u+1);   /* Topological adress for  */
       p_b->iv_pat   = (short)(j_v+1);   /* current (this) patch    */
       p_b->us_pat   = u_val[ i_u ] +    /* Start U on geom. patch  */
                       comptol;          /*                         */
       p_b->ue_pat   = u_val[i_u+1] -    /* End   U on geom. patch  */
                       comptol;          /*                         */
       p_b->vs_pat   = v_val[ j_v ] +    /* Start V on geom. patch  */
                       comptol;          /*                         */
       p_b->ve_pat   = v_val[j_v+1] -    /* End   V on geom. patch  */
                       comptol;          /*                         */

       p_b->box_pat.xmin = 1.0;          /* BBOX  initialization    */
       p_b->box_pat.ymin = 2.0;          /*                         */
       p_b->box_pat.zmin = 3.0;          /*                         */
       p_b->box_pat.xmax = 4.0;          /*                         */
       p_b->box_pat.ymax = 5.0;          /*                         */
       p_b->box_pat.zmax = 6.0;          /*                         */
       p_b->box_pat.flag = -1;           /* Not calculated          */
    
       p_b->cone_pat.xdir = 1.0;         /* BCONE initialization    */
       p_b->cone_pat.ydir = 2.0;         /*                         */
       p_b->cone_pat.zdir = 3.0;         /*                         */
       p_b->cone_pat.ang  = 4.0;         /*                         */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*top_pat i_u %d j_v %d us %8.4f ue %8.4f vs %8.4f ve %8.4f\n", 
(int)i_u, (int)j_v, 
p_b->us_pat, p_b->ue_pat, p_b->vs_pat, p_b->ve_pat  );
fflush(dbgfil(SURPAC));
}
#endif

       }    /* End loop i_u */
     }      /* End loop j_v */


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/






/*!********* Internal ** function ** knotdef ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Calculate knot vectors if not input to the function           */

    static short knotdef 
  (order_u, order_v, nk_u, nk_v, np_u, np_v, pp_uknots, pp_vknots)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint    order_u;      /* Order of surface in U direction         */
  DBint    order_v;      /* Order of surface in V direction         */
  DBint    nk_u;         /* Number of knot values in U direction    */
  DBint    nk_v;         /* Number of knot values in V direction    */
  DBint    np_u;         /* Number of control points in U direction */
  DBint    np_v;         /* Number of control points in V direction */
  DBfloat **pp_uknots;   /* Knot vector in U direction        (ptr) */
  DBfloat **pp_vknots;   /* Knot vector in V direction        (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    i_u;          /* Loop index U knot                       */
  DBint    j_v;          /* Loop index V knot                       */
  DBfloat  d_u;          /* Delta U                                 */
  DBfloat  d_v;          /* Delta V                                 */
/*----------------------------------------------------------------- */

  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur890*knotdef Enter  Number of U knots %d \n",
  (int)nk_u);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check array sizes                                                */
/*                                                                 !*/

  if  ( nk_u > 1000 || nk_v > 1000 )
 { 
 sprintf(errbuf, "Increase u_knot_u/v_knot_u%%sur890*knotdef");
 return(varkon_erpush("SU2993",errbuf));
 }


/*!                                                                 */
/* Initialize knot vector arrays                                    */
/*                                                                 !*/

   for ( i_u = 0; i_u < 1000; ++i_u )
     {
     u_knots_u[i_u] = F_UNDEF;
     v_knots_u[i_u] = F_UNDEF;
     }    /* End loop i_u */


/*!                                                                 */
/* Delta U and delta V equal to one (1.0)                           */
/*                                                                 !*/

   d_u = 1.0;
   d_v = 1.0;

/*!                                                                 */
/* First knot values (multiplicity order_u and order_v)             */
/*                                                                 !*/

   for ( i_u = 0; i_u < order_u; ++i_u )
     {
     u_knots_u[i_u] = 1.0;
     }    /* End loop i_u */

   for ( j_v = 0; j_v < order_v; ++j_v )
     {
     v_knots_u[j_v] = 1.0;
     }    /* End loop j_v */

/*!                                                                 */
/* Knot values with delta equal to d_u and d_v.                     */
/*                                                                 !*/

   for ( i_u = order_u; i_u < nk_u - order_u + 1; ++i_u )
     {
     u_knots_u[i_u] = u_knots_u[i_u-1] + d_u;
     }    /* End loop i_u */

   for ( j_v = order_v; j_v < nk_v - order_v + 1; ++j_v )
     {
     v_knots_u[j_v] = v_knots_u[j_v-1] + d_v;
     }    /* End loop j_v */

/*!                                                                 */
/* Last  knot values (multiplicity order_u and order_v)             */
/*                                                                 !*/

   for ( i_u = nk_u - order_u + 1; i_u < nk_u; ++i_u )
     {
     u_knots_u[i_u] = u_knots_u[i_u-1];
     }    /* End loop i_u */

   for ( j_v = nk_v - order_v + 1; j_v < nk_v; ++j_v )
     {
     v_knots_u[j_v] = v_knots_u[j_v-1];
     }    /* End loop j_v */

/*!                                                                 */
/* Let input ptrs point at static arrays. Not very nice programming */
/*                                                                 !*/

   *pp_uknots = &u_knots_u[0];
   *pp_vknots = &v_knots_u[0];


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur890*knotdef Knot vectors have been defined \n");
for ( i_u = 0; i_u < nk_u; ++i_u )
fprintf(dbgfil(SURPAC),
"sur890*knotdef i_u %d u_knots_u %f\n",(int)i_u, u_knots_u[i_u] );
for ( j_v = 0; j_v < nk_v; ++j_v )
fprintf(dbgfil(SURPAC),
"sur890*knotdef j_v %d v_knots_u %f\n",(int)j_v, v_knots_u[j_v] );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

