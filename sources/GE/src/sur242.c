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
/*  Function: varkon_pat_nurbs                     File: sur242.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a NURBS surface.             */
/*                                                                  */
/*  Author:   Gunnar Liden                                         !*/
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-09   Originally written                                 */
/*  1998-01-28   d2rdudv for a rational surface                     */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_nurbs      NURBS evaluation function        */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*                                                                  */
/*         Evaluation of coordinates and derivatives                */
/*             for a NURBS surface                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short findspan();      /* Determine the knot span index      */
static short derbafun();      /* Basis functions with derivatives   */
static short poideriv();      /* Calculate point and derivatives    */
static short basisfun();      /* Nonvanishing basis functions       */
static short point();         /* Calculate point coordinates        */

/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint  order_u;        /* Order  for surface in U direction  */
static DBint  order_v;        /* Order  for surface in V direction  */
static DBint  deg_u;          /* Degree for surface in U direction  */
static DBint  deg_v;          /* Degree for surface in V direction  */
static DBint  nk_u;           /* Number of nodes    in U direction  */
static DBint  nk_v;           /* Number of nodes    in V direction  */
static DBint  span_u;         /* Knot span index    in U direction  */
static DBint  span_v;         /* Knot span index    in V direction  */
static DBfloat b_u[MAX_NURBD];/* Basis functions    in U direction  */
static DBfloat b_v[MAX_NURBD];/* Basis functions    in V direction  */
static DBfloat comptol;       /* Computer tolerance (accuracy)      */
static DBfloat ctol;          /* Coordinate tolerance               */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_poi_nurbs       * Get address to a NURBS node   */
/*           varkon_comptol         * Retrieve computer tolerance   */
/*           varkon_ctol            * Coordinate tolerance          */
/*           varkon_erinit          * Initial. of error messages    */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_nurbs     */
/* SU2993 = Severe program error in varkon_pat_nurbs   (sur242).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_pat_nurbs (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATNU *p_patnu,     /* NURBS patch                       (ptr) */
   DBint   icase,        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat u_glob,       /* Global U value                          */
   DBfloat v_glob,       /* Global V value                          */

   EVALS   *p_xyz )      /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat d_u[MAX_NURBD][MAX_NURBD]; /* U Basis and derivatives     */
  DBfloat d_v[MAX_NURBD][MAX_NURBD]; /* V Basis and derivatives     */
  DBint   n_der_out;                 /* Requested no. of derivatives*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  *p_knot;     /* Knot (U or V) vector              (ptr) */
   DBfloat   t_val;      /* Current (U or V) parameter value        */
   DBint     i_span;     /* Span                                    */
   DBint     n_der;      /* Requested number of derivatives         */
   DBint     p_degree;   /* The degree of the basis functions       */
   char      errbuf[80]; /* String for error message fctn erpush    */
   short     status;     /* Error code from a called function       */

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
"sur242 Enter**varkon_pat_nurbs   p_patnu %d u %f v %f icase %d\n",
                (int)p_patnu,u_glob,v_glob,(short)icase);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives and internal      */
/*  variables for DEBUG on.                                         */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_patnu,u_glob,v_glob,p_xyz); 
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_nurbs   (sur242)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/*  Retrieve coordinate tolerance. Call of varkon_ctol (sur751)     */
/*                                                                 !*/

   ctol      = varkon_ctol();

/*!                                                                 */
/*  Requested number of derivatives n_der_out defined by icase      */
/*                                                                 !*/

  n_der_out = 2;                 /* Requested no. of derivatives */

/*  Check that degree is ..  to be added                            */

/*!                                                                 */
/* 2. Get number of nodes, order (degree) of surface                */
/* ____________________________                                     */
/*                                                                  */
/*                                                                 !*/

/*  NURBS: Order and degree in U and V direction                    */
    order_u = p_patnu->order_u;
    order_v = p_patnu->order_v;
    deg_u   = order_u - 1;
    deg_v   = order_v - 1;

/*  NURBS: Number of nodes in U and V direction                     */
    nk_u    = p_patnu->nk_u;
    nk_v    = p_patnu->nk_v;

    if  (  deg_u >= MAX_NURBD || deg_v >= MAX_NURBD )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 deg_u %d or deg_v %d >= MAX_NURBD %d\n",
         (int)deg_u, (int)deg_v, MAX_NURBD );
fflush(dbgfil(SURPAC));
}
#endif
      sprintf(errbuf,"MAX_NURBD%%sur242");
      return(varkon_erpush("SU2993",errbuf));
      }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 deg_u %d or deg_v %d \n",
         (int)deg_u, (int)deg_v);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242  nk_u %d or  nk_v %d \n",
         (int) nk_u, (int) nk_v );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Detetermine the knot span indeces span_u,span_v               */
/* __________________________________________________               */
/*                                                                  */
/* Call of internal function findspan.                              */
/*                                                                 !*/
   status= findspan (p_patnu, u_glob,v_glob); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 findspan failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"findspan%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }


/*!                                                                 */
/* 4. Compute the nonvanishing basis functions b_u and b_v          */
/* _______________________________________________________          */
/*                                                                  */
/* Call of internal function basisfun.                              */
/*                                                                 !*/
   status= basisfun (p_patnu, u_glob,v_glob); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 basisfun failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"basisfun%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 5. Calculate coordinates                                         */
/* ________________________                                         */
/*                                                                  */
/* Call of internal function point.                                 */
/*                                                                 !*/
   status= point ( p_patnu, p_xyz ); 
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 point failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"point%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }



/*!                                                                 */
/* 6. Calculate basis functions and derivatives                     */
/* ____________________________________________                     */
/*                                                                  */
/* Calls of internal function derbafun                              */
/*                                                                 !*/
   p_knot   = p_patnu->kvec_u;
   i_span   = span_u;
   t_val    = u_glob;
   p_degree = deg_u;
   n_der    = deg_u; /* Max  */
   n_der    = n_der_out;

   status=derbafun(p_knot,t_val,i_span,n_der,p_degree,d_u);
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 derbafun U failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"derbafun%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }

   p_knot   = p_patnu->kvec_v;
   i_span   = span_v;
   t_val    = v_glob;
   p_degree = deg_v;
   n_der    = deg_v; /* Max  */
   n_der    = n_der_out;

   status=derbafun(p_knot,t_val,i_span,n_der,p_degree,d_v);
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 derbafun V failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"derbafun%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 7. Calculate point and derivatives                               */
/* __________________________________                               */
/*                                                                  */
/* Call of internal function poideriv                               */
/*                                                                 !*/

   status=poideriv(p_patnu,d_u,d_v,n_der_out,p_xyz );
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242 poideriv failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"poideriv%%sur242");
   return(varkon_erpush("SU2973",errbuf));
   }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur242 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur242 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur242 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fprintf(dbgfil(SURPAC),
  "sur242 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur242 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur242 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242 Exit *** varkon_pat_nurbs x= %8.2f y= %8.2f z= %8.2f \n",
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
/* static (common) variables to 1.23456789 and 123456789.           */
/* Printout also of the conic lofting patch data.                   */

   static short initial(p_patnu,u_glob,v_glob,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATNU *p_patnu;     /* NURBS patch                       (ptr) */
   DBfloat  u_glob;      /* Global U value                          */
   DBfloat  v_glob;      /* Global V value                          */
   EVALS   *p_xyz;       /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_b;         /* Loop index basis functions              */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur242 *** initial: p_xyz= %d\n", (int)p_xyz);
  }

/*!                                                                 */
/* 1. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= 1.23456789;
    p_xyz->r_y= 1.23456789;
    p_xyz->r_z= 1.23456789;


/*  Tangent             dr/du                                       */
    p_xyz->u_x=  -1.23456789;
    p_xyz->u_y=   1.23456789;
    p_xyz->u_z=   1.23456789;

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= 1.23456789;
    p_xyz->v_y= 1.23456789;
    p_xyz->v_z= 1.23456789;

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= 1.23456789;
    p_xyz->u2_y= 1.23456789;
    p_xyz->u2_z= 1.23456789;

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= 1.23456789;
    p_xyz->v2_y= 1.23456789;
    p_xyz->v2_z= 1.23456789;

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= 1.23456789;
    p_xyz->uv_y= 1.23456789;
    p_xyz->uv_z= 1.23456789;

/*  Surface normal       r(u)                                       */
    p_xyz->n_x= 1.23456789;
    p_xyz->n_y= 1.23456789;
    p_xyz->n_z= 1.23456789;

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= 1.23456789;
    p_xyz->nu_y= 1.23456789;
    p_xyz->nu_z= 1.23456789;

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= 1.23456789;
    p_xyz->nv_y= 1.23456789;
    p_xyz->nv_z= 1.23456789;


/*  Maximum and minimum principal curvature                          */
    p_xyz->kmax= 1.23456789;
    p_xyz->kmin= 1.23456789;

/*  Direction in R*3 for kmax                                        */
    p_xyz->kmax_x= 1.23456789;
    p_xyz->kmax_y= 1.23456789;
    p_xyz->kmax_z= 1.23456789;

/*  Direction in R*3 for kmin                                        */
    p_xyz->kmin_x= 1.23456789;
    p_xyz->kmin_y= 1.23456789;
    p_xyz->kmin_z= 1.23456789;

/*  Direction in u,v for kmax                                       */
    p_xyz->kmax_u= 1.23456789;
    p_xyz->kmax_v= 1.23456789;

/*  Direction in u,v for kmin                                       */
    p_xyz->kmin_u= 1.23456789;
    p_xyz->kmin_v= 1.23456789;

/* Internal varibles                                                */
   order_u = I_UNDEF;
   order_v = I_UNDEF;
   nk_u    = I_UNDEF;  
   nk_v    = I_UNDEF; 
   span_u  = I_UNDEF; 
   span_v  = I_UNDEF; 
   deg_u   = I_UNDEF;
   deg_v   = I_UNDEF;

   for ( i_b = 0; i_b < MAX_NURBD; i_b = i_b + 1)
     {
     b_u[i_b] = F_UNDEF;
     b_v[i_b] = F_UNDEF;
     }


   return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif



/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Determine the knot span indeces span_u and span_v                */
/* The knot span indeces are integers with values                   */
/* .. I am not sure that the below code is right  !!!!              */
/* u_span = u_deg-1, ... , nk_u-1                                   */
/* v_span = v_deg-1, ... , nk_v-1                                   */

   static short findspan (p_patnu, u_glob, v_glob )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATNU  *p_patnu;    /* NURBS patch                       (ptr) */
   DBfloat   u_glob;     /* Global U value                          */
   DBfloat   v_glob;     /* Global U value                          */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_knot;      /* U knot value                            */
   DBfloat  v_knot;      /* V knot value                            */
   DBint    low;         /* Index lower limit in binary search      */
   DBint    high;        /* Index upper limit in binary search      */
   DBint    mid;         /* Index mid   point in binary search      */
/*-----------------------------------------------------------------!*/
   DBfloat *p_uval;      /* U knot value                      (ptr) */
   DBfloat *p_vval;      /* V knot value                      (ptr) */
   DBint    k_iu;        /* Loop index U knot values                */
   DBint    n_iter;      /* Number of iterations                    */
   char     errbuf[80];  /* String for error message fctn erpush    */
#ifdef  DEBUG
   DBint    k_iv;        /* Loop index V knot values                */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that input parameters have valid values for Debug On       */
/*                                                                 !*/

#ifdef  DEBUG
   k_iu   = 0;
   p_uval = p_patnu->kvec_u + k_iu;
   u_knot = *p_uval;
   if  (  u_glob < u_knot - comptol )
     {
     sprintf(errbuf,"u_glob < Start knot%%sur242*findspan");
     return(varkon_erpush("SU2993",errbuf));
     }
   k_iu   = nk_u;
   p_uval = p_patnu->kvec_u + k_iu - 1;
   u_knot = *p_uval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan u_glob %f End value u_knot= %8.2f \n",
   u_glob, u_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
   if  (  u_glob > u_knot + comptol )
     {
     sprintf(errbuf,"u_glob > End knot%%sur242*findspan");
     return(varkon_erpush("SU2993",errbuf));
     }

   k_iv   = 1;
   p_vval = p_patnu->kvec_v + k_iv - 1;
   v_knot = *p_vval;
   if  (  v_glob < v_knot - comptol )
     {
     sprintf(errbuf,"v_glob < Start knot%%sur242*findspan");
     return(varkon_erpush("SU2993",errbuf));
     }
   k_iv   = nk_v;
   p_vval = p_patnu->kvec_v + k_iv - 1;
   v_knot = *p_vval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan v_glob %f End value v_knot= %8.2f \n",
   v_glob, v_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
   if  (  v_glob > v_knot + comptol )
     {
     sprintf(errbuf,"v_glob > End knot%%sur242*findspan");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif



/* Initialization of local variables for Debug On                   */
#ifdef DEBUG
   low    = I_UNDEF;
   high   = I_UNDEF;
   mid    = I_UNDEF;
#endif

   n_iter = 0;

#ifdef  DEBUG
for ( k_iu = 1; k_iu <= nk_u; ++k_iu )
  {
  p_uval = p_patnu->kvec_u + k_iu - 1;
  u_knot = *p_uval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan Index (span) %4d U knot value u_knot= %8.2f \n",
   (int)k_iu-1, u_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
  }

for ( k_iv = 1; k_iv <= nk_v; ++k_iv )
  {
  p_vval = p_patnu->kvec_v + k_iv - 1;
  v_knot = *p_vval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan Index (span) %4d V knot value v_knot= %8.2f \n",
   (int)k_iv-1, v_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
  }
#endif


/*!                                                                 */
/* 1. Determine the U knot span index span_u                        */
/* ------------------------------------------                       */
/*                                                                  */
/*                                                                 !*/

/* Check if the input parameter value is equal to the start knot    */
   p_uval = p_patnu->kvec_u + deg_u;
   u_knot = *p_uval;
   if ( fabs(u_glob - u_knot) < comptol ) 
     {
     span_u = deg_u; 
     goto  u_end;
     }

/* Check if the input parameter value is equal to the end knot      */
   k_iu   = nk_u;
   p_uval = p_patnu->kvec_u + nk_u - deg_u - 1;
   u_knot = *p_uval;
   if ( fabs(u_glob - u_knot) < comptol ) 
     {
     span_u = nk_u - 1 - deg_u - 1;
     goto  u_end;
     }

/* Initialize lower and upper limits for the binary search          */
/* The lower limit is the degree of the NURBS surface and           */
/* the upper limit is the number of knot values.                    */
   low  =  deg_u;
   high =  nk_u - deg_u - 1;

/* The mid point is the truncated value (low+high)/2                */
   mid  = (DBint)((low+high)/2);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan low %4d high %4d mid %4d Start values\n",
   (int)low, (int)high, (int)mid );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan U[low] %4.2f U[high] %4.2f U[mid] %4.2f U[mid+1] %4.2f\n",
    *(p_patnu->kvec_u + low) ,
    *(p_patnu->kvec_u + high),
    *(p_patnu->kvec_u + mid) ,
    *(p_patnu->kvec_u + mid  + 1 ));
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/* Do the binary search                                             */
   while ( u_glob <  *(p_patnu->kvec_u + mid)  ||
           u_glob >= *(p_patnu->kvec_u + mid + 1) )
     {

     n_iter = n_iter + 1;
     if  ( n_iter > 1000 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242**findspan n_iter >1000 u_glob %f\n",
         u_glob );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"n_iter U%%sur242*findspan");
       return(varkon_erpush("SU2993",errbuf));
       }



     if  ( u_glob <  *(p_patnu->kvec_u + mid) ) high = mid;
     else                                       low  = mid;
/*   The new mid point is the truncated value (low+high)/2          */
     mid  = (DBint)((low+high)/2);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan low %4d high %4d mid %4d u_glob %f\n",
   (int)low, (int)high, (int)mid, u_glob );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan U[low] %4.2f U[high] %4.2f U[mid] %4.2f U[mid+1] %4.2f\n",
    *(p_patnu->kvec_u + low) ,
    *(p_patnu->kvec_u + high),
    *(p_patnu->kvec_u + mid) ,
    *(p_patnu->kvec_u + mid  + 1) );
  fflush(dbgfil(SURPAC)); 
  }
#endif

     } /* End loop binary search */

/* The output knot span index is found                              */

   span_u = mid;
   
u_end:; /* Label: Input U is the end knot U value                   */

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*findspan u_glob= %6.4f span_u= %d U[] %f U[+1] %f\n", 
       u_glob, (int)span_u, 
  *(p_patnu->kvec_u + span_u ) ,
  *(p_patnu->kvec_u + span_u +1));
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 2. Determine the V knot span index span_v                        */
/* ------------------------------------------                       */
/*                                                                  */
/*                                                                 !*/

/* Check if the input parameter value is equal to the start knot    */
   p_vval = p_patnu->kvec_v + deg_v;
   v_knot = *p_vval;
   if ( fabs(v_glob - v_knot) < comptol ) 
     {
     span_v = deg_v;
     goto  v_end;
     }

/* Check if the input parameter value is equal to the end knot      */
   p_vval = p_patnu->kvec_v + nk_v - deg_v - 1;
   v_knot = *p_vval;
   if ( fabs(v_glob - v_knot) < comptol ) 
     {
     span_v = nk_v - 1 - deg_v - 1;
     goto  v_end;
     }

/* Initialize lower and upper limits for the binary search          */
/* The lower limit is the degree of the NURBS surface and           */
/* the upper limit is the number of knot values.                    */
   low  =  deg_v;
   high =  nk_v - deg_v - 1;

/* The mid point is the truncated value (low+high)/2                */
   mid  = (DBint)((low+high)/2);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan low %4d high %4d mid %4d Start values\n",
   (int)low, (int)high, (int)mid );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan V[low] %4.2f V[high] %4.2f V[mid] %4.2f V[mid+1] %4.2f\n",
    *(p_patnu->kvec_v + low) ,
    *(p_patnu->kvec_v + high),
    *(p_patnu->kvec_v + mid) ,
    *(p_patnu->kvec_v + mid  + 1) );
  fflush(dbgfil(SURPAC)); 
  }
#endif

/* Do the binary search                                             */
   while ( v_glob <  *(p_patnu->kvec_v + mid)  ||
           v_glob >= *(p_patnu->kvec_v + mid + 1) )
     {

     n_iter = n_iter + 1;
     if  ( n_iter > 1000 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur242**findspan n_iter >1000 v_glob %f\n",
         v_glob );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"n_iter V%%sur242*findspan");
       return(varkon_erpush("SU2993",errbuf));
       }



     if  ( v_glob <  *(p_patnu->kvec_v + mid) ) high = mid;
     else                                       low  = mid;
/*   The new mid point is the truncated value (low+high)/2          */
     mid  = (DBint)((low+high)/2);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*findspan low %4d high %4d mid %4d v_glob %f\n",
   (int)low, (int)high, (int)mid, v_glob );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur242*findspan V[low] %4.2f V[high] %4.2f V[mid] %4.2f V[mid+1] %4.2f\n",
  *(p_patnu->kvec_v + low) ,
  *(p_patnu->kvec_v + high),
  *(p_patnu->kvec_v + mid) ,
  *(p_patnu->kvec_v + mid  + 1) );
fflush(dbgfil(SURPAC)); 
}
#endif

     } /* End loop binary search span_v */

/* The output knot span index is found                              */

   span_v = mid;
   
v_end:; /* Label: Input V is the end knot V value                   */



#ifdef  DEBUG
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur242*findspan u_glob= %6.4f span_u= %d U[] %f U[+1] %f\n", 
       u_glob, (int)span_u, 
  *(p_patnu->kvec_u + span_u ) ,
  *(p_patnu->kvec_u + span_u +1));
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur242*findspan v_glob= %6.4f span_v= %d V[] %f V[+1] %f\n", 
       v_glob, (int)span_v, 
  *(p_patnu->kvec_v + span_v ) ,
  *(p_patnu->kvec_v + span_v +1));
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
/* Compute the nonvanishing basis functions b_u and b_v             */

   static short basisfun (p_patnu, u_glob, v_glob )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATNU  *p_patnu;    /* NURBS patch                       (ptr) */
   DBfloat   u_glob;     /* Global U value                          */
   DBfloat   v_glob;     /* Global U value                          */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_knot1;         /* U knot value                        */
   DBfloat  u_knot2;         /* U knot value                        */
   DBfloat  v_knot1;         /* V knot value                        */
   DBfloat  v_knot2;         /* V knot value                        */
   DBfloat  temp;            /* Temporary basis function value      */
   DBfloat  saved;           /* Saved     basis function value      */
   DBfloat  left [MAX_NURBD];/* Parameter interval left             */
   DBfloat  right[MAX_NURBD];/* Parameter interval right            */
   DBint    j_n;             /* Loop index                          */
   DBint    r_n;             /* Loop index                          */
/*-----------------------------------------------------------------!*/
   DBfloat  denom;       /* Denominator                             */
   DBfloat *p_uval;      /* U knot value                      (ptr) */
   DBfloat *p_vval;      /* V knot value                      (ptr) */
   char    errbuf[80];   /* String for error message fctn erpush    */
#ifdef  DEBUG
   DBfloat  u_sum;       /* Sum of basis functions U                */
   DBfloat  v_sum;       /* Sum of basis functions V                */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/* Initialization of basis function values for Debug On             */
#ifdef DEBUG
   for  ( j_n = 0; j_n <= deg_u; j_n++)
     {
     left [j_n] = F_UNDEF;
     right[j_n] = F_UNDEF;
     }
#endif

/*!                                                                 */
/* 1. Basis functions for U                                         */
/* -------------------------                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* First basis function                                             */
/*                                                                 !*/

   b_u[0] = 1.0;

   for  ( j_n = 1; j_n <= deg_u; j_n++)
     {
     p_uval     = p_patnu->kvec_u + span_u + 1 - j_n;
     u_knot1    = *p_uval;
     left [j_n] = u_glob - u_knot1;
     p_uval     = p_patnu->kvec_u + span_u + j_n;
     u_knot2    = *p_uval;
     right[j_n] = u_knot2 - u_glob;
     saved      = 0.0;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun j_n= %4d left[%4d]= %f right[%4d] %f\n", 
       (int)j_n, (int)j_n, left[j_n], (int)j_n, right[j_n] );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun u_knot1 %f u_knot2 %f  \n", 
u_knot1, u_knot2 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
     for  ( r_n = 0; r_n <  j_n; r_n++)
       {
       denom    = right[r_n+1]+left[j_n-r_n];
       if  ( fabs(denom) < 10.0*comptol)
         {
         sprintf(errbuf,"denom U = 0%%sur242*basisfun");
         return(varkon_erpush("SU2993",errbuf));
         }
       temp     = b_u[r_n] / denom;
       b_u[r_n] = saved + right[r_n+1]*temp;
       saved    = left[j_n-r_n]*temp;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun j_n= %4d r_n %d b_u[%4d] %f left[  ]= %f right[  ] %f\n", 
(int)j_n, (int)r_n, (int)r_n, b_u[r_n], left[j_n-r_n], right[r_n+1] );
fflush(dbgfil(SURPAC));
}
#endif
       } /* End loop r_n */
     b_u[j_n]   = saved;
     } /* End loop j_n */


/*!                                                                 */
/* 2. Basis functions for V                                         */
/* -------------------------                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* First basis function                                             */
/*                                                                 !*/

   b_v[0] = 1.0;

   for  ( j_n = 1; j_n <= deg_v; j_n++)
     {
     p_vval     = p_patnu->kvec_v + span_v + 1 - j_n;
     v_knot1    = *p_vval;
     left [j_n] = v_glob - v_knot1;
     p_vval     = p_patnu->kvec_v + span_v + j_n;
     v_knot2    = *p_vval;
     right[j_n] = v_knot2 - v_glob;
     saved      = 0.0;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun j_n= %4d left[%4d]= %f right[%4d] %f\n", 
       (int)j_n, (int)j_n, left[j_n], (int)j_n, right[j_n] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun v_knot1 %f v_knot2 %f  \n", 
v_knot1, v_knot2 );
fflush(dbgfil(SURPAC)); 
}
#endif
     for  ( r_n = 0; r_n <  j_n; r_n++)
       {
       denom    = right[r_n+1]+left[j_n-r_n];
       if  ( fabs(denom) < 10.0*comptol)
         {
         sprintf(errbuf,"denom V = 0%%sur242*basisfun");
         return(varkon_erpush("SU2993",errbuf));
         }
       temp     = b_v[r_n] / denom;
       b_v[r_n] = saved + right[r_n+1]*temp;
       saved    = left[j_n-r_n]*temp;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun j_n= %4d r_n %d b_v[%4d] %f left[  ]= %f right[  ] %f\n", 
(int)j_n, (int)r_n, (int)r_n, b_v[r_n], left[j_n-r_n], right[r_n+1] );
fflush(dbgfil(SURPAC));
}
#endif
       } /* End loop r_n */
     b_v[j_n]   = saved;
     } /* End loop j_n */


/*!                                                                 */
/* 3. Check result for Debug On                                     */
/* -----------------------------                                    */
/*                                                                  */
/*                                                                 !*/

#ifdef  DEBUG
u_sum = 0.0;
for  ( j_n = 0; j_n <= deg_u; j_n++)
{
  u_sum = b_u[j_n] + u_sum;
}
v_sum = 0.0;
for  ( j_n = 0; j_n <= deg_v; j_n++)
{
  v_sum = b_v[j_n] + v_sum;
}


if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun u_sum=  %f v_sum= %f\n", 
       u_sum, v_sum );
fflush(dbgfil(SURPAC)); 
}

if ( fabs(u_sum-1.0) > comptol )
   {
   sprintf(errbuf,"u_sum not 1%%sur242*basisfun");
   return(varkon_erpush("SU2993",errbuf));
   }
if ( fabs(v_sum-1.0) > comptol )
   {
   sprintf(errbuf,"v_sum not 1%%sur242*basisfun");
    return(varkon_erpush("SU2993",errbuf));
   }
#endif




#ifdef  DEBUG
for  ( j_n = 0; j_n <= deg_u; j_n++)
{
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun b_u[%d]=  %f \n", 
       (int)j_n, b_u[j_n] );
fflush(dbgfil(SURPAC)); 
}
}
for  ( j_n = 0; j_n <= deg_v; j_n++)
{
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur242*basisfun b_v[%d]=  %f \n", 
       (int)j_n, b_v[j_n] );
fflush(dbgfil(SURPAC)); 
}
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
/* Compute point coordinates                                        */

   static short point (p_patnu, p_xyz )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATNU  *p_patnu;    /* NURBS patch                       (ptr) */
   EVALS    *p_xyz;      /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  tempx[MAX_NURBD];/* Temporary vector to store ...       */
   DBfloat  tempy[MAX_NURBD];/* Temporary vector to store ...       */
   DBfloat  tempz[MAX_NURBD];/* Temporary vector to store ...       */
   DBfloat  tempw[MAX_NURBD];/* Temporary vector to store ...       */
   DBfloat  polyx;           /* Polygon point X value               */
   DBfloat  polyy;           /* Polygon point Y value               */
   DBfloat  polyz;           /* Polygon point Z value               */
   DBfloat  polyw;           /* Polygon point w value               */
   DBfloat  s_x;             /* Surface point X coordinate          */
   DBfloat  s_y;             /* Surface point Y coordinate          */
   DBfloat  s_z;             /* Surface point Z coordinate          */
   DBfloat  s_w;             /* Surface point w coordinate          */
   DBint    l_n;             /* Loop index                          */
   DBint    k_n;             /* Loop index                          */
                         /* For varkon_poi_nurbs (sur243)           */
  DBint     rwcase;      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
  DBint     i_up;        /* Point address in U direction            */
  DBint     i_vp;        /* Point address in V direction            */
  DBHvector  node;        /* Polygon point                           */

/*-----------------------------------------------------------------!*/
   DBint    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/* Initialization of local variables for Debug On                   */
#ifdef DEBUG
   i_up  = I_UNDEF;
   i_vp  = I_UNDEF;
   polyx = F_UNDEF;
   polyy = F_UNDEF;
   polyz = F_UNDEF;
   polyw = F_UNDEF;
   s_x   = F_UNDEF;
   s_y   = F_UNDEF;
   s_z   = F_UNDEF;
   s_w   = F_UNDEF;
   for  ( l_n = 0; l_n < MAX_NURBD; l_n++)
     {
     tempx[l_n] = F_UNDEF;
     tempy[l_n] = F_UNDEF;
     tempz[l_n] = F_UNDEF;
     tempw[l_n] = F_UNDEF;
     }
#endif


/*!                                                                 */
/*  Printout of polygon points for Debug On                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

#ifdef  DEBUG
    rwcase = 3;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs 
    (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 List%%sur242*point");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

   for  ( l_n = 0; l_n <= deg_v; l_n++)
     {
     tempx[l_n] = 0.0;
     tempy[l_n] = 0.0;
     tempz[l_n] = 0.0;
     tempw[l_n] = 0.0;
     for  ( k_n = 0; k_n <= deg_u; k_n++)
       {
       i_up  = span_u - deg_u + k_n;
       i_vp  = span_v - deg_v + l_n;

      rwcase = 0;
      status= varkon_poi_nurbs 
      (p_patnu, rwcase, i_up, i_vp, &node);
     if  ( status < 0 ) 
       {
       sprintf(errbuf, "sur243 Read%%sur242*point");
       return(varkon_erpush("SU2943",errbuf));
       }

       polyx  =  node.x_gm;
       polyy  =  node.y_gm;
       polyz  =  node.z_gm;
       polyw  =  node.w_gm;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"i_up %3d i_vp %3d x %8.2f y %8.2f z %8.2f w %8.4f\n",
 (int)i_up,(int)i_vp, polyx, polyy, polyz, polyw );
fflush(dbgfil(SURPAC)); 
}
#endif


       tempx[l_n] = tempx[l_n] + b_u[k_n]*polyx;
       tempy[l_n] = tempy[l_n] + b_u[k_n]*polyy;
       tempz[l_n] = tempz[l_n] + b_u[k_n]*polyz;
       tempw[l_n] = tempw[l_n] + b_u[k_n]*polyw;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"l_n %3d k_n %3d polyx %8.4f b_u[k_n] %8.4f tempx[l_n] %8.2f\n",
 (int)l_n ,(int)k_n, polyx, b_u[k_n],tempx[l_n]);
fflush(dbgfil(SURPAC)); 
}
#endif
       }
     }
    s_x = 0.0;
    s_y = 0.0;
    s_z = 0.0;
    s_w = 0.0;
   for  ( l_n = 0; l_n <= deg_v; l_n++)
     {
     s_x  = s_x + b_v[l_n]*tempx[l_n];
     s_y  = s_y + b_v[l_n]*tempy[l_n];
     s_z  = s_z + b_v[l_n]*tempz[l_n];
     s_w  = s_w + b_v[l_n]*tempw[l_n];
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"l_n %3d  b_v[l_n] %8.4f tempx[l_n] %8.2f s_x %f\n",
 (int)l_n , b_v[l_n],tempx[l_n], s_x);
fflush(dbgfil(SURPAC)); 
}
#endif
     }

  if ( fabs(s_w) < 100.0*comptol )
     {
     sprintf(errbuf,"Rational denominator = 0%%sur242*point");
     return(erpush("SU2993",errbuf));
     }

  p_xyz->r_x = s_x/s_w;
  p_xyz->r_y = s_y/s_w;
  p_xyz->r_z = s_z/s_w;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*POINT X= %8.2f Y= %8.2f Z= %8.2f w= %8.4f\n",
   s_x,s_y,s_z,s_w);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*point x= %8.2f y= %8.2f z= %8.2f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
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
/* Compute nonvanishing basis functions and their derivatives       */

   static short derbafun (p_knot,t_val,i_span,n_der,p_degree,ders )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat  *p_knot;     /* Knot (U or V) vector              (ptr) */
   DBfloat   t_val;      /* Current (U or V) parameter value        */
   DBint     i_span;     /* Span                                    */
   DBint     n_der;      /* Requested number of derivatives         */
   DBint     p_degree;   /* The degree of the basis functions       */
/* Out:                                                             */
   DBfloat  ders[MAX_NURBD][MAX_NURBD]; /* Basis ftns & derivatives */

/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  k_value_1;       /* Knot value 1 from input vector      */
   DBfloat  k_value_2;       /* Knot value 2 from input vector      */
   DBfloat  temp;            /* Temporary basis function value      */
   DBfloat  saved;           /* Saved     basis function value      */

   DBfloat  left [MAX_NURBD];/* Parameter interval left             */
   DBfloat  right[MAX_NURBD];/* Parameter interval right            */
   DBint    j_n;             /* Loop index                          */
   DBint    r_n;             /* Loop index                          */
   DBint    k_n;             /* Loop index                          */
   DBint    j1;              /* Loop end value                      */
   DBint    j2;              /* Loop end value                      */
   DBint    s1;              /* Index in array                      */
   DBint    s2;              /* Index in array                      */
   DBint    rk;              /* Index in array                      */
   DBint    pk;              /* Index in array                      */
   DBfloat  deriv;           /* Derivative                          */

/*-----------------------------------------------------------------!*/
/* Basis functions and knot differences                             */
   DBfloat    ndu[MAX_NURBD][MAX_NURBD];
/* To store intermediate results                                    */
   DBfloat  a[2][MAX_NURBD];
   DBfloat  denom;       /* Denominator                             */
   DBfloat *p_tval;      /* Knot value                        (ptr) */
   char    errbuf[80];   /* String for error message fctn erpush    */
#ifdef  DEBUG
   DBfloat  u_sum;       /* Sum of basis functions                  */
#endif
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*derbafun Enter t_val %f i_span %d n_der %d p_degree %d\n", 
       t_val,(int)i_span, (int)n_der, (int)p_degree  );
fflush(dbgfil(SURPAC)); 
}
#endif





/* Initialization of basis function values for Debug On             */
#ifdef DEBUG
   for  ( j_n = 0; j_n < MAX_NURBD; j_n++)
     {
     left [j_n] = F_UNDEF;
     right[j_n] = F_UNDEF;
     a[0][j_n]  = F_UNDEF;
     a[1][j_n]  = F_UNDEF;
     }
   for  ( j_n = 0; j_n < MAX_NURBD; j_n++)
     {
       for  ( r_n = 0; r_n < MAX_NURBD; r_n++)
         {
         ndu[j_n][r_n] = F_UNDEF;
         ders[j_n][r_n] = F_UNDEF;
         }
      }
    denom     = F_UNDEF;
    temp      = F_UNDEF;
    saved     = F_UNDEF;
    k_value_1 = F_UNDEF;
    k_value_2 = F_UNDEF;
    u_sum     = F_UNDEF;
#endif



/* First basis function                                             */
   
   ndu[0][0] = 1.0;

   for  ( j_n = 1; j_n <= p_degree;  j_n++)
     {
/*   Knot value                                                     */
     p_tval       =  p_knot + i_span + 1 - j_n;
     k_value_1    = *p_tval;
     left [j_n]   =  t_val - k_value_1;
     p_tval       =  p_knot + i_span + j_n;
     k_value_2    = *p_tval;
     right[j_n]   =  k_value_2 - t_val;
     saved        = 0.0;

     for  ( r_n = 0; r_n < j_n;  r_n++)
       {
       ndu[j_n][r_n] = right[r_n+1] + left[j_n-r_n];
       denom         = ndu[j_n][r_n];
       if  ( fabs(denom) < 10.0*comptol )
         {
         sprintf(errbuf,"denom= 0%%sur242*derbafun");
         return(varkon_erpush("SU2993",errbuf));
         }
       temp       = ndu[r_n][j_n-1]/denom;
       ndu[r_n][j_n]     = saved + right[r_n+1]*temp;
       saved             = left[j_n-r_n]*temp;

       } /* End loop r_n  */

     ndu[j_n][j_n]     = saved;

     } /* End loop j_n  */

/* Load the basis functions                                         */

   for  ( j_n = 0; j_n <= p_degree;  j_n++)
     {
     ders[0][j_n] = ndu[j_n][p_degree];
     } /* End loop j_n  */



#ifdef  DEBUG
for  ( j_n = 0; j_n <= p_degree; j_n++)
{
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*derbafun ders[%d][%d]=  %f Prior to derivative calc.\n", 
       (int)0  , (int)j_n, ders[0  ][j_n] );
fflush(dbgfil(SURPAC)); 
}
}
#endif


/* Calculation of derivatives                                       */

/* Loop over function index                                         */

   for  ( r_n = 0; r_n <= p_degree;  r_n++)
     {
     s1      = 0;
     s2      = 1;
     a[0][0] = 1.0;

/* Loop to compute the kth derivative                               */

     for  ( k_n = 1; k_n <= n_der;     k_n++)
       {
       deriv = 0.0;
       rk    = r_n - k_n;
       pk    = p_degree - k_n;
       if  ( r_n >= k_n )
         {
         denom         = ndu[pk+1][rk];
         if  ( fabs(denom) < 10.0*comptol )
           {
           sprintf(errbuf,"denom= 0 (2)%%sur242*derbafun");
           return(varkon_erpush("SU2993",errbuf));
           }
         a[s2][0] = a[s1][0]/ndu[pk+1][rk];
         deriv    = a[s2][0]*ndu[rk][pk];
         }
       if (rk >= -1 ) j1 =   1;
       else           j1 = -rk;
       if ( r_n-1 <= pk ) j2 = k_n - 1;
       else               j2 = p_degree - r_n;

       for  ( j_n = j1; j_n <= j2;  j_n++)
         {
         denom         = ndu[pk+1][rk+j_n];
         if  ( fabs(denom) < 10.0*comptol )
           {
           sprintf(errbuf,"denom= 0 (3)%%sur242*derbafun");
           return(varkon_erpush("SU2993",errbuf));
           }
         a[s2][j_n] = (a[s1][j_n]-a[s1][j_n-1])/denom;
         deriv = deriv + a[s2][j_n]*ndu[rk+j_n][pk];
         } /* End loop j_n  */
       if ( r_n <= pk )
         {
         denom         = ndu[pk+1][r_n];
         if  ( fabs(denom) < 10.0*comptol )
           {
           sprintf(errbuf,"denom= 0 (4)%%sur242*derbafun");
           return(varkon_erpush("SU2993",errbuf));
           }
         a[s2][k_n] = -a[s1][k_n-1]/denom;
         deriv = deriv + a[s2][k_n]*ndu[r_n][pk];
         } /* End r_n <= pk */

       ders[k_n][r_n] = deriv;
/*     Switch rows                                                 */
       j_n = s1; 
       s1  = s2;
       s2  = j_n;
       } /* End loop k_n  */
     } /* End loop r_n  */

/* Multiply with correct factors                                  */

   r_n = p_degree;

   for  ( k_n = 1; k_n <= n_der;  k_n++)
     {
     for  ( j_n =  0; j_n <= p_degree;  j_n++)
       {
       ders[k_n][j_n] = ders[k_n][j_n]*r_n;
       } /* End loop j_n  */
     r_n = r_n*(p_degree-k_n);
     } /* End loop k_n  */




#ifdef  DEBUG
for  ( j_n = 0; j_n <= p_degree; j_n++)
{
  for  ( r_n = 0; r_n <= p_degree; r_n++)
  {
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur242*derbafun ders[%d][%d]=  %f \n", 
       (int)j_n, (int)r_n, ders[j_n][r_n] );
fflush(dbgfil(SURPAC)); 
}
}
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
/* Compute point coordinates                                        */

   static short poideriv (p_patnu,d_u,d_v, n_der, p_xyz )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*     Static variables                                             */
/*     deg_u     Degree in U                                        */
/*     deg_v     Degree in V                                        */
/*     span_u    Span   in U                                        */
/*     span_v    Span   in V                                        */
/*                                                                  */
/*                                                                  */
  GMPATNU *p_patnu;                 /* NURBS patch            (ptr) */
  DBfloat d_u[MAX_NURBD][MAX_NURBD];/* U Basis and derivatives      */
  DBfloat d_v[MAX_NURBD][MAX_NURBD];/* V Basis and derivatives      */
  DBint   n_der;                    /* Requested no. of derivatives */
  EVALS   *p_xyz;                   /* Coord's and derivatives (ptr)*/

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat SKL[MAX_NURBD][MAX_NURBD][4]; /* Point and derivatives   */
   DBfloat tempx[MAX_NURBD];             /* Temporary X vector      */
   DBfloat tempy[MAX_NURBD];             /* Temporary Y vector      */
   DBfloat tempz[MAX_NURBD];             /* Temporary Z vector      */
   DBfloat tempw[MAX_NURBD];             /* Temporary w vector      */
   DBfloat polyx;                        /* Polygon point X value   */
   DBfloat polyy;                        /* Polygon point Y value   */
   DBfloat polyz;                        /* Polygon point Z value   */
   DBfloat polyw;                        /* Polygon point w value   */


/*-----------------------------------------------------------------!*/
   DBfloat  h_x;         /* Surface point X coordinate, homogenous  */
   DBfloat  h_y;         /* Surface point Y coordinate, homogenous  */
   DBfloat  h_z;         /* Surface point Z coordinate, homogenous  */
   DBfloat  h_w;         /* Surface point w coordinate, homogenous  */
   DBfloat  hu_x  ;      /* Surface dR/du               homogenous  */
   DBfloat  hu_y  ;      /* Surface dR/du               homogenous  */
   DBfloat  hu_z  ;      /* Surface dR/du               homogenous  */
   DBfloat  hu_w  ;      /* Surface dR/du               homogenous  */
   DBfloat  hv_x  ;      /* Surface dR/dv               homogenous  */
   DBfloat  hv_y  ;      /* Surface dR/dv               homogenous  */
   DBfloat  hv_z  ;      /* Surface dR/dv               homogenous  */
   DBfloat  hv_w  ;      /* Surface dR/dv               homogenous  */
   DBfloat  hu2_x   ;    /* Surface d2R/du2             homogenous  */
   DBfloat  hu2_y   ;    /* Surface d2R/du2             homogenous  */
   DBfloat  hu2_z   ;    /* Surface d2R/du2             homogenous  */
   DBfloat  hu2_w   ;    /* Surface d2R/du2             homogenous  */
   DBfloat  hv2_x   ;    /* Surface d2R/dv2             homogenous  */
   DBfloat  hv2_y   ;    /* Surface d2R/dv2             homogenous  */
   DBfloat  hv2_z   ;    /* Surface d2R/dv2             homogenous  */
   DBfloat  hv2_w   ;    /* Surface d2R/dv2             homogenous  */
   DBfloat  huv_x   ;    /* Surface d2R/dudv            homogenous  */
   DBfloat  huv_y   ;    /* Surface d2R/dudv            homogenous  */
   DBfloat  huv_z   ;    /* Surface d2R/dudv            homogenous  */
   DBfloat  huv_w   ;    /* Surface d2R/dudv            homogenous  */
   DBfloat  h_d2xdudv;   /* Surface d2R/dudv            homogenous  */
   DBfloat  h_d2ydudv;   /* Surface d2R/dudv            homogenous  */
   DBfloat  h_d2zdudv;   /* Surface d2R/dudv            homogenous  */
   DBfloat  h_d2wdudv;   /* Surface d2R/dudv            homogenous  */

                         /* For varkon_poi_nurbs (sur243)           */
  DBint     rwcase;      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
  DBint     i_up;        /* Point address in U direction            */
  DBint     i_vp;        /* Point address in V direction            */
  DBHvector  node;       /* Polygon point                           */

   DBint    l_n;         /* Loop index                              */
   DBint    k_n;         /* Loop index                              */
   DBint    s_n;         /* Loop index                              */
   DBint    r_n;         /* Loop index                              */
   DBint    min_du;      /* Minimum of n_der and deg_u              */
   DBint    min_dv;      /* Minimum of n_der and deg_v              */
   DBint    min_dd;      /* Minimum of d_n-k_n and min_dv           */
   DBint    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/* Initialization of local variables for Debug On                   */
#ifdef DEBUG
for  ( l_n = 0; l_n < MAX_NURBD; l_n++)
  {
  for  ( k_n = 0; k_n < MAX_NURBD; k_n++)
    {
    SKL[l_n][k_n][0] = F_UNDEF;
    SKL[l_n][k_n][1] = F_UNDEF;
    SKL[l_n][k_n][2] = F_UNDEF;
    SKL[l_n][k_n][3] = F_UNDEF;
    }
  }
for  ( l_n = 0; l_n < MAX_NURBD; l_n++)
  {
  tempx[l_n] = F_UNDEF;
  tempy[l_n] = F_UNDEF;
  tempz[l_n] = F_UNDEF;
  tempw[l_n] = F_UNDEF;
  }
l_n        = I_UNDEF;
k_n        = I_UNDEF;
r_n        = I_UNDEF;
s_n        = I_UNDEF;
min_du     = I_UNDEF;
min_dv     = I_UNDEF;
min_dd     = I_UNDEF;
status     = I_UNDEF;
rwcase     = I_UNDEF;
i_up       = I_UNDEF;
i_vp       = I_UNDEF;
node.x_gm  = F_UNDEF;
node.y_gm  = F_UNDEF;
node.z_gm  = F_UNDEF;
node.w_gm  = F_UNDEF;
polyx      = F_UNDEF;
polyy      = F_UNDEF;
polyz      = F_UNDEF;
polyw      = F_UNDEF;
h_x        = F_UNDEF;
h_y        = F_UNDEF;
h_z        = F_UNDEF;
h_w        = F_UNDEF;
hu_x       = F_UNDEF;
hu_y       = F_UNDEF;
hu_z       = F_UNDEF;
hu_w       = F_UNDEF;
hv_x       = F_UNDEF;
hv_y       = F_UNDEF;
hv_z       = F_UNDEF;
hv_w       = F_UNDEF;
hu2_x      = F_UNDEF;
hu2_y      = F_UNDEF;
hu2_z      = F_UNDEF;
hu2_w      = F_UNDEF;
hv2_x      = F_UNDEF;
hv2_y      = F_UNDEF;
hv2_z      = F_UNDEF;
hv2_w      = F_UNDEF;
huv_x      = F_UNDEF;
huv_y      = F_UNDEF;
huv_z      = F_UNDEF;
huv_w      = F_UNDEF;
h_d2xdudv  = F_UNDEF;
h_d2ydudv  = F_UNDEF;
h_d2zdudv  = F_UNDEF;
h_d2wdudv  = F_UNDEF;
#endif


/*!                                                                 */
/*  Printout of polygon points for Debug On                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

#ifdef  DEBUG
    rwcase = 3;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs 
    (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 List%%sur242*poideriv");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif


/*!                                                                 */
/*  Initialization of point and derivative array SKL                */
/*                                                                 !*/

   if  (  n_der <  deg_u ) min_du = n_der;
   else                    min_du = deg_u;
   if  (  n_der <  deg_v ) min_dv = n_der;
   else                    min_dv = deg_v;


   for  ( k_n = deg_u+1; k_n <= n_der; k_n++)
     {
     for  ( l_n = 0; l_n <= n_der-k_n; l_n++)
       {
       SKL[k_n][l_n][0] = 0.0;
       SKL[k_n][l_n][1] = 0.0;
       SKL[k_n][l_n][2] = 0.0;
       SKL[k_n][l_n][3] = 0.0;
       }
     }

   for  ( l_n = deg_v+1; l_n <= n_der; l_n++)
     {
     for  ( k_n = 0; k_n <= n_der-l_n; k_n++)
       {
       SKL[k_n][l_n][0] = 0.0;
       SKL[k_n][l_n][1] = 0.0;
       SKL[k_n][l_n][2] = 0.0;
       SKL[k_n][l_n][3] = 0.0;
       }
     }


/*!                                                                 */
/*  Calculate point and derivatives                                 */
/*                                                                 !*/


   for  ( k_n = 0; k_n <= min_du; k_n++)
     {
     for  ( s_n = 0; s_n <= deg_v;  s_n++)
       {
       tempx[s_n] = 0.0;
       tempy[s_n] = 0.0;
       tempz[s_n] = 0.0;
       tempw[s_n] = 0.0;
       for  ( r_n = 0; r_n <= deg_u; r_n++)
         {
/*       Get node point                                             */
         i_up   = span_u - deg_u + r_n;
         i_vp   = span_v - deg_v + s_n;
         rwcase = 0;
         status= varkon_poi_nurbs 
         (p_patnu, rwcase, i_up, i_vp, &node);
        if  ( status < 0 ) 
          {
          sprintf(errbuf, "sur243 Read%%sur242*poideriv");
          return(varkon_erpush("SU2943",errbuf));
          }
         polyx  =  node.x_gm;
         polyy  =  node.y_gm;
         polyz  =  node.z_gm;
         polyw  =  node.w_gm;

         tempx[s_n] = tempx[s_n] + d_u[k_n][r_n]*polyx;
         tempy[s_n] = tempy[s_n] + d_u[k_n][r_n]*polyy;
         tempz[s_n] = tempz[s_n] + d_u[k_n][r_n]*polyz;
         tempw[s_n] = tempw[s_n] + d_u[k_n][r_n]*polyw;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"i_up %3d i_vp %3d           x %8.2f y %8.2f z %8.2f \n",
 (int)i_up,(int)i_vp,             polyx, polyy, polyz);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"s_n  %3d k_n  %3d r_n %3d temp(s_n)= %8.2f  %8.2f  %8.2f\n",
 (int)s_n ,(int)k_n ,(int)r_n,  
 tempx[s_n], tempy[s_n], tempz[s_n] );
fflush(dbgfil(SURPAC));
}
#endif

         }  /* End loop r_n */

       }  /* End loop s_n */


     if  (  n_der-k_n <  min_dv ) min_dd = n_der-k_n;
     else                         min_dd = min_dv;

     for  ( l_n = 0; l_n <= min_dd; l_n++)
       {

       SKL[k_n][l_n][0] = 0.0;
       SKL[k_n][l_n][1] = 0.0;
       SKL[k_n][l_n][2] = 0.0;
       SKL[k_n][l_n][3] = 0.0;

       for  ( s_n = 0; s_n <= deg_v;  s_n++)
         {
         SKL[k_n][l_n][0] = SKL[k_n][l_n][0]+d_v[l_n][s_n]*tempx[s_n];
         SKL[k_n][l_n][1] = SKL[k_n][l_n][1]+d_v[l_n][s_n]*tempy[s_n];
         SKL[k_n][l_n][2] = SKL[k_n][l_n][2]+d_v[l_n][s_n]*tempz[s_n];
         SKL[k_n][l_n][3] = SKL[k_n][l_n][3]+d_v[l_n][s_n]*tempw[s_n];
         }  /* End loop s_n */

       }  /* End loop l_n */

     }  /* End loop k_n */



/*!                                                                 */
/*  Coordinates and derivatives to output point p_xyz               */
/*                                                                 !*/


/*  Homogenous coordinates                                          */
    h_x  =  SKL[0][0][0];
    h_y  =  SKL[0][0][1];
    h_z  =  SKL[0][0][2];
    h_w  =  SKL[0][0][3];

  if ( fabs(h_w) < 100.0*comptol )
     {
     sprintf(errbuf,"Rational denominator = 0%%sur242*poideriv");
     return(varkon_erpush("SU2993",errbuf));
     }

/*  Cartesian  coordinates                                          */
  p_xyz->r_x = h_x/h_w;
  p_xyz->r_y = h_y/h_w;
  p_xyz->r_z = h_z/h_w;

/*  First derivative                                                */

  if ( n_der < 1 ) goto all_deriv;

  hu_x    =  SKL[1][0][0];
  hu_y    =  SKL[1][0][1];
  hu_z    =  SKL[1][0][2];
  hu_w    =  SKL[1][0][3];

/*!                                                                 */
/* Cartesian  first derivative  u_x,  u_y,  u_z                     */
/*                                                                 !*/

  p_xyz->u_x = ( h_w*hu_x - hu_w*h_x )/ h_w/ h_w;
  p_xyz->u_y = ( h_w*hu_y - hu_w*h_y )/ h_w/ h_w;
  p_xyz->u_z = ( h_w*hu_z - hu_w*h_z )/ h_w/ h_w;

  hv_x    =  SKL[0][1][0];
  hv_y    =  SKL[0][1][1];
  hv_z    =  SKL[0][1][2];
  hv_w    =  SKL[0][1][3];

/*!                                                                 */
/* Cartesian  first derivative  v_x,  v_y,  v_z                     */
/*                                                                 !*/

  p_xyz->v_x = ( h_w*hv_x - hv_w*h_x )/ h_w/ h_w;
  p_xyz->v_y = ( h_w*hv_y - hv_w*h_y )/ h_w/ h_w;
  p_xyz->v_z = ( h_w*hv_z - hv_w*h_z )/ h_w/ h_w;



/*  Second derivatives                                              */

  if ( n_der < 2 ) goto all_deriv;

  hu2_x     =  SKL[2][0][0];
  hu2_y     =  SKL[2][0][1];
  hu2_z     =  SKL[2][0][2];
  hu2_w     =  SKL[2][0][3];

  hv2_x     =  SKL[0][2][0];
  hv2_y     =  SKL[0][2][1];
  hv2_z     =  SKL[0][2][2];
  hv2_w     =  SKL[0][2][3];


  h_d2xdudv =  SKL[1][1][0];
  h_d2ydudv =  SKL[1][1][1];
  h_d2zdudv =  SKL[1][1][2];
  h_d2wdudv =  SKL[1][1][3];

  huv_x     =  SKL[1][1][0];
  huv_y     =  SKL[1][1][1];
  huv_z     =  SKL[1][1][2];
  huv_w     =  SKL[1][1][3];

/*!                                                                 */
/* Cartesian second derivative  u2_x,  u2_y,  u2_z                  */
/*                                                                 !*/

  p_xyz->u2_x = ( h_w*hu2_x - hu_w*hu_x)/ h_w/ h_w  -
    ((hu2_w*h_x  + hu_w*hu_x)* h_w* h_w - 2.0* h_w*h_x *hu_w*hu_w)
           / h_w/ h_w/ h_w/ h_w;
  p_xyz->u2_y = ( h_w*hu2_y - hu_w*hu_y)/ h_w/ h_w  -
    ((hu2_w*h_y  + hu_w*hu_y)* h_w* h_w - 2.0* h_w*h_y *hu_w*hu_w)
           / h_w/ h_w/ h_w/ h_w;
  p_xyz->u2_z = ( h_w*hu2_z - hu_w*hu_z)/ h_w/ h_w  -
    ((hu2_w*h_z  + hu_w*hu_z)* h_w* h_w - 2.0* h_w*h_z *hu_w*hu_w)
           / h_w/ h_w/ h_w/ h_w;



/*!                                                                 */
/* Cartesian second derivative  v2_x,  v2_y,  v2_z                  */
/*                                                                 !*/

  p_xyz->v2_x = ( h_w*hv2_x - hv_w*hv_x)/ h_w/ h_w  -
    ((hv2_w*h_x  + hv_w*hv_x)* h_w* h_w - 2.0* h_w*h_x *hv_w*hv_w)
           / h_w/ h_w/ h_w/ h_w;
  p_xyz->v2_y = ( h_w*hv2_y - hv_w*hv_y)/ h_w/ h_w  -
    ((hv2_w*h_y  + hv_w*hv_y)* h_w* h_w - 2.0* h_w*h_y *hv_w*hv_w)
           / h_w/ h_w/ h_w/ h_w;
  p_xyz->v2_z = ( h_w*hv2_z - hv_w*hv_z)/ h_w/ h_w  -
    ((hv2_w*h_z  + hv_w*hv_z)* h_w* h_w - 2.0* h_w*h_z *hv_w*hv_w)
           / h_w/ h_w/ h_w/ h_w;

/*! !! Derivative d2r/dudv= uv_x,uv_y,uv_z !!!! Not OK !!!!        !*/

/*
  uv_x= -1.23456789;
  uv_y= -1.23456789;
  uv_z= -1.23456789;
 Also sur224 must be updated if ..   */

/*  Add for rational derivatives .............                      */

#ifdef  REMOVE_980128
  p_xyz->uv_x = h_d2xdudv;
  p_xyz->uv_y = h_d2ydudv;
  p_xyz->uv_z = h_d2zdudv;
#endif  /*  REMOVE_980128  */


  p_xyz->uv_x = (h_w*h_w*(hv_w*hu_x+h_w*huv_x-huv_w*h_x-hu_w*hv_x)+
                 2.0*h_w*hv_w*(h_w*hu_x-hu_w*h_x))
                  / h_w / h_w / h_w / h_w;

  p_xyz->uv_y = (h_w*h_w*(hv_w*hu_y+h_w*huv_y-huv_w*h_y-hu_w*hv_y)+
                 2.0*h_w*hv_w*(h_w*hu_y-hu_w*h_y))
                  / h_w / h_w / h_w / h_w;

  p_xyz->uv_z = (h_w*h_w*(hv_w*hu_z+h_w*huv_z-huv_w*h_z-hu_w*hv_z)+
                 2.0*h_w*hv_w*(h_w*hu_z-hu_w*h_z))
                  / h_w / h_w / h_w / h_w;


all_deriv:; /* Label: All requested derivatives are calculated */

#ifdef  DEBUG

for  ( l_n = 0; l_n <= n_der; l_n++)
  {
  for  ( k_n = 0; k_n <= n_der; k_n++)
    {
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*poideriv SKL[%3d][%3d][0-3]= %8.2f %8.2f %8.2f %5.3f\n",
    (int)l_n, (int)k_n, 
    SKL[l_n][k_n][0] ,
    SKL[l_n][k_n][1] ,
    SKL[l_n][k_n][2] ,
    SKL[l_n][k_n][3] );
    }
    }
  }

if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*poideriv h_w %5.4f hu_w %7.4f hv_w %7.4f hu2_w %7.4f hv2_w %7.4f\n",
   h_w, hu_w, hv_w, hu2_w, hv2_w);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*poideriv x= %8.2f y= %8.2f z= %8.2f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 2  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*poideriv drdu %8.2f %8.2f %8.2f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur242*poideriv drdv %8.2f %8.2f %8.2f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

