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
/*  Function: varkon_sur_nurbsacp                  File: sur891.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function determines if a NURBS surface is closed            */
/*  and periodic.                                                   */
/*                                                                  */
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
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-02-17   Originally written                                 */
/*  1998-03-19   Bug: Not programmed for p_w= NULL                  */
/*  1999-12-04   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nurbsacp   Is NURB_SUR closed/periodic ?    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer  tolerance           */
/* varkon_idknot           * Retrieve parameter tolerance           */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_sur_nurbsnon     * Calculate number of nodes              */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
#ifdef  DEBUG
/* varkon_sur_nurbsche     * Check NURBS data                       */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_nurbsacp        */
/* SU2993 = Severe program error (  ) in varkon_sur_nurbsacp        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/

     DBstatus         varkon_sur_nurbsacp (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  
  DBint     f_surcur,    /* Flag for surface or curve check;        */
                         /* Eq. 1: Surface   Eq. 2: Curve           */
  DBint     order_u,     /* Order of surface in U direction         */
  DBint     order_v,     /* Order of surface in V direction         */
  DBint     nk_u,        /* Number of knot values in U direction    */
  DBint     nk_v,        /* Number of knot values in V direction    */
  DBVector *p_cpts,      /* Control points (nodes)            (ptr) */
  DBfloat  *p_uknots,    /* Knot vector in U direction        (ptr) */
  DBfloat  *p_vknots,    /* Knot vector in V direction        (ptr) */
  DBfloat  *p_w,         /* Weights for the control points    (ptr) */
  DBint    *p_form_u,    /* Closed/periodic flag in U         (ptr) */
  DBint    *p_form_v )   /* Closed/periodic flag in V         (ptr) */
                         /* Eq. 0: Not closed and periodic          */
                         /* Eq. 1: Closed                           */
                         /* Eq. 2: Closed and periodic              */

/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*      Data to p_form_u, p_form_v                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    np_u;         /* Number of control points in U direction */
  DBint    np_v;         /* Number of control points in V direction */
  DBint    f_close_u;    /* Eq. 0: Open in U  Eq. 1: Closed in U    */
  DBint    f_close_v;    /* Eq. 0: Open in V  Eq. 1: Closed in V    */
  DBint    f_period_u;   /* Eq. 0: Per. in U  Eq. 1: Not per.  U    */
  DBint    f_period_v;   /* Eq. 0: Per. in V  Eq. 1: Not per.  V    */
  DBint    i_ku;         /* Loop index U knot value                 */
  DBint    j_kv;         /* Loop index V knot value                 */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBfloat  idknot;       /* Identical knots criterion               */
  DBfloat  idpoint;      /* Identical point criterion               */

  DBfloat  diff;         /* Difference nodes and knots              */
  DBfloat  diff_start;   /* Difference in knots for start tangent   */
  DBfloat  diff_end;     /* Difference in knots for start tangent   */

  DBint    i_i_start;    /* Index for start point                   */
  DBint    i_i_end;      /* Index for end   point                   */
  DBint    i_i_t_start;  /* Index for start tangent point           */
  DBint    i_i_t_end;    /* Index for end   tangent point           */
  DBVector poi_start;    /* Start point                             */
  DBVector poi_end;      /* End   point                             */
  DBVector poi_stang;    /* Start tangent point                     */
  DBVector poi_etang;    /* End   tangent point                     */

  DBint    degree_u;     /* Degree in U direction                   */
  DBint    degree_v;     /* Degree in V direction                   */
  DBint    i_u;          /* Loop index U nodes and weights          */
  DBint    j_v;          /* Loop index V nodes and weights          */
  DBfloat  weight;       /* Weight for the node                     */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

#ifdef DEBUG
                         /* For varkon_sur_nurbsche (sur893):       */
  DBint    error_code;   /* Error code                              */
  DBint    i_i;          /* Loop index used for the input data      */
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
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 Enter***varkon_sur_nurbsacp\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur891 order_u= %d, order_v= %d\n",
       (int)order_u,(int)order_v);
fprintf(dbgfil(SURPAC),"sur891 nk_u= %d, nk_v= %d\n",
       (int)nk_u,(int)nk_v);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize output values.                                        */
/*                                                                 !*/

   *p_form_u = I_UNDEF;
   *p_form_v = I_UNDEF;

/*!                                                                 */
/* Check surface/curve flag                                         */
/*                                                                 !*/

   if  (  f_surcur == 1 )
     {
     /* OK, surface */
     }
   else if ( f_surcur == 2 )
     {
     /* Curve case is not yet implemented */
     sprintf(errbuf, "(Curve case not impl.)%%sur891");
     return(varkon_erpush("SU2993",errbuf));
     }
   else
     { 
     sprintf(errbuf, "(f_surcur not OK)%%sur891");
     return(varkon_erpush("SU2993",errbuf));
     }

/* Initialize local parameters.                                     */

   np_u           = I_UNDEF;
   np_v           = I_UNDEF;
   degree_u       = F_UNDEF;
   degree_v       = F_UNDEF;
   f_close_u      = I_UNDEF;
   f_close_u      = I_UNDEF;
   f_close_v      = I_UNDEF;
   f_period_u     = I_UNDEF;
   f_period_v     = I_UNDEF;
   diff           = F_UNDEF;
   diff_start     = F_UNDEF;
   diff_end       = F_UNDEF;
   i_i_start      = I_UNDEF;
   i_i_end        = I_UNDEF; 
   i_i_t_start    = I_UNDEF;
   i_i_t_end      = I_UNDEF; 
   poi_start.x_gm = F_UNDEF;
   poi_start.y_gm = F_UNDEF;
   poi_start.z_gm = F_UNDEF;
   poi_end.x_gm   = F_UNDEF;
   poi_end.y_gm   = F_UNDEF;
   poi_end.z_gm   = F_UNDEF;
   poi_stang.x_gm = F_UNDEF;
   poi_stang.y_gm = F_UNDEF;
   poi_stang.z_gm = F_UNDEF;
   poi_etang.x_gm = F_UNDEF;
   poi_etang.y_gm = F_UNDEF;
   poi_etang.z_gm = F_UNDEF;

/*!                                                                 */
/* Check the input NURBS data for Debug On                          */
/* Call of varkon_sur_nurbsche (sur893).                            */
/*                                                                 !*/

#ifdef DEBUG
    status= varkon_sur_nurbsche
    (f_surcur, order_u, order_v, nk_u, nk_v, p_cpts, 
              p_uknots, p_vknots, p_w,  &error_code );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur893%%sur891");
     return(varkon_erpush("SU2943",errbuf));
     }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur891 Input NURBS data is checked error_code= %d\n",
               (int)error_code  );
fflush(dbgfil(SURPAC));
}
#endif


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
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Identical knots criterion. Call of varkon_idknot (sur744).       */
/*                                                                 !*/

   idknot =varkon_idknot();

/*!                                                                 */
/* Identical point criterion. Call of varkon_idpoint (sur741).      */
/*                                                                 !*/

   idpoint   = varkon_idpoint();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur891 comptol %18.15f idknot %14.10f\n",
comptol, idknot  );
fflush(dbgfil(SURPAC));
}
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur891 Knot vector in U direction:\n");
fflush(dbgfil(SURPAC));
if ( p_uknots != NULL ) for ( i_i=0; i_i<nk_u; ++i_i )
  fprintf(dbgfil(SURPAC),"sur891 U-knot %3d= %12.8f\n",
       (int)i_i,*(p_uknots+i_i));
fprintf(dbgfil(SURPAC),"sur891 Knot vector in V direction:\n");
fflush(dbgfil(SURPAC));
if ( p_vknots != NULL ) for ( i_i=0; i_i<nk_v; ++i_i )
  fprintf(dbgfil(SURPAC),"sur891 V-knot %3d=%12.8f\n",
       (int)i_i,*(p_vknots+i_i));

for ( j_v = 0; j_v < np_v;    ++j_v )
  {
  fprintf(dbgfil(SURPAC),"sur891 New U curve:\n");
  fflush(dbgfil(SURPAC));
    for ( i_u = 0; i_u < np_u;    ++i_u )
    {
/*  Index in array                                            */
    i_i = i_u*np_v + j_v;

    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

    fprintf(dbgfil(SURPAC),
    "sur891 i_u %2d j_v %2d %12.4f %12.4f %12.4f w %8.6f\n",
    (int)i_u,(int)j_v, 
    (double)(p_cpts+i_i)->x_gm/weight,
    (double)(p_cpts+i_i)->y_gm/weight,
    (double)(p_cpts+i_i)->z_gm/weight,
       weight );
    fflush(dbgfil(SURPAC));
    }
  }

fflush(dbgfil(SURPAC));
}
#endif



/*!                                                                 */
/* 2. Closed in U ?                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that the multiplicity for the start point is order_u       */
/*                                                                 !*/
   
  f_close_u = 1;
  for ( i_ku = 0; i_ku < order_u-1; ++i_ku )
    {
    diff = fabs( *(p_uknots+i_ku+1) - *(p_uknots+i_ku));
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 i_ku= %d diff= %12.8f\n", (int)i_ku, diff  );
}
#endif
    if  ( diff > idknot) f_close_u = 0;
    }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_u == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is OK for the U start point\n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is not OK for the start point ERROR\n");
  }
  fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Check that the multiplicity for the end   point is order_u       */
/*                                                                 !*/
   

  for ( i_ku = nk_u-order_u; i_ku < nk_u-1; ++i_ku )
    {
    diff = fabs( *(p_uknots+i_ku+1) - *(p_uknots+i_ku));
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 i_ku= %d diff= %12.8f\n", (int)i_ku, diff  );
}
#endif
    if  ( diff > idknot) f_close_u = 0;
    }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_u == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is OK for the U end   point\n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is not OK for the end   point ERROR\n");
  }
  fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Check that start node is equal to end node for all U "curves".   */
/*                                                                 !*/
   

  for ( j_v = 0; j_v < np_v; ++j_v )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
    fprintf(dbgfil(SURPAC),"sur891 New U curve:\n");
}
#endif
/*  Index in array for start point                            */
    i_i_start =   0*np_v + j_v;

/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (s)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start point                                               */
    poi_start.x_gm = (double)(p_cpts+i_i_start)->x_gm/weight;
    poi_start.y_gm = (double)(p_cpts+i_i_start)->y_gm/weight;
    poi_start.z_gm = (double)(p_cpts+i_i_start)->z_gm/weight;

/*  Index in array for end   point                            */
    i_i_end   = (np_u-1)*np_v + j_v;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (e)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   point                                               */
    poi_end.x_gm = (double)(p_cpts+i_i_end)->x_gm/weight;
    poi_end.y_gm = (double)(p_cpts+i_i_end)->y_gm/weight;
    poi_end.z_gm = (double)(p_cpts+i_i_end)->z_gm/weight;


/*  Distance between the points                               */
    diff = sqrt (
    (poi_start.x_gm-poi_end.x_gm)*(poi_start.x_gm-poi_end.x_gm)+ 
    (poi_start.y_gm-poi_end.y_gm)*(poi_start.y_gm-poi_end.y_gm)+ 
    (poi_start.z_gm-poi_end.z_gm)*(poi_start.z_gm-poi_end.z_gm));


#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 j_v=  %d diff= %12.8f\n", (int)j_v,  diff  );
}
#endif

/*  Not closed if difference exceeds idpoint.                 */
    if  ( diff > idpoint ) f_close_u = 0;


  }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_u == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is closed in U direction \n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is open in U direction \n");
  }
  fflush(dbgfil(SURPAC));
}
#endif

  if ( f_close_u == 0 ) goto n_uclo;




/*!                                                                 */
/* 3. Periodic in U ?                                               */
/* _________________                                                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that line from the start node to start node + 1 is         */
/* equal to to the line between the end node and the end node -1,   */
/* i.e. that the lines are parallell and have the same length.      */
/*                                                                 !*/
   

  f_period_u = 1;
  for ( j_v = 0; j_v < np_v; ++j_v )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
    fprintf(dbgfil(SURPAC),"sur891 (Periodic) New U curve:\n");
}
#endif

/*  Index in array for start point                            */
    i_i_start =   0*np_v + j_v;

/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (s)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start point                                               */
    poi_start.x_gm = (double)(p_cpts+i_i_start)->x_gm/weight;
    poi_start.y_gm = (double)(p_cpts+i_i_start)->y_gm/weight;
    poi_start.z_gm = (double)(p_cpts+i_i_start)->z_gm/weight;

/*  Index in array for end   point                            */
    i_i_end   = (np_u-1)*np_v + j_v;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (e)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   point                                               */
    poi_end.x_gm = (double)(p_cpts+i_i_end)->x_gm/weight;
    poi_end.y_gm = (double)(p_cpts+i_i_end)->y_gm/weight;
    poi_end.z_gm = (double)(p_cpts+i_i_end)->z_gm/weight;


/*  Index in array for start tangent point                    */
    i_i_t_start =   1*np_v + j_v;

/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_t_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (st)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start tangent point                                       */
    poi_stang.x_gm = (double)(p_cpts+i_i_t_start)->x_gm/weight;
    poi_stang.y_gm = (double)(p_cpts+i_i_t_start)->y_gm/weight;
    poi_stang.z_gm = (double)(p_cpts+i_i_t_start)->z_gm/weight;

/*  Index in array for end   tangent point                    */
    i_i_t_end   = (np_u-2)*np_v + j_v;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_t_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (et)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   tangent point                                       */
    poi_etang.x_gm = (double)(p_cpts+i_i_t_end)->x_gm/weight;
    poi_etang.y_gm = (double)(p_cpts+i_i_t_end)->y_gm/weight;
    poi_etang.z_gm = (double)(p_cpts+i_i_t_end)->z_gm/weight;

/*  End   tangent point "mirrored" in the end point           */
    poi_etang.x_gm = poi_end.x_gm+poi_end.x_gm-poi_etang.x_gm;
    poi_etang.y_gm = poi_end.y_gm+poi_end.y_gm-poi_etang.y_gm;
    poi_etang.z_gm = poi_end.z_gm+poi_end.z_gm-poi_etang.z_gm;




/*  Distance between the tangent points                       */
    diff = sqrt (
    (poi_stang.x_gm-poi_etang.x_gm)*(poi_stang.x_gm-poi_etang.x_gm)+ 
    (poi_stang.y_gm-poi_etang.y_gm)*(poi_stang.y_gm-poi_etang.y_gm)+ 
    (poi_stang.z_gm-poi_etang.z_gm)*(poi_stang.z_gm-poi_etang.z_gm));


#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 j_v=  %d diff= %12.8f (tangent points)\n", 
               (int)j_v, diff);
}
#endif

/*  Not periodic if difference exceeds idpoint.              */
    if  ( diff > idpoint ) f_period_u = 0;


  } /* End "curve" loop */


/*!                                                                 */
/* Check that the knot value differences are equal.                 */
/*                                                                 !*/
   
  i_ku = order_u+1;
  diff_start = *(p_uknots+i_ku+1) - *(p_uknots+i_ku);
  i_ku = nk_u - order_u - 1;
  diff_end   = *(p_uknots+i_ku+1) - *(p_uknots+i_ku);

  if  ( fabs(diff_start-diff_end) > idknot  ) f_period_u = 0;



#ifdef DEBUG  
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur891  i_ku= %d diff_start %f\n",
   (int)order_u, diff_start);
  fprintf(dbgfil(SURPAC),
  "sur891  i_ku= %d diff_end   %f\n",
   (int)(nk_u-order_u), diff_end  );
  fprintf(dbgfil(SURPAC),
  "sur891  Difference  %12.8f Criterion %12.8f\n",
   diff_start-diff_end, idknot );
}
if ( dbglev(SURPAC) == 2 )
{
if ( f_period_u == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is periodic in U direction \n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is not periodic in U direction \n");
  }
  fflush(dbgfil(SURPAC));
}
#endif

n_uclo:; /* Label: Not closed in U */


/*!                                                                 */
/* 4. Closed in V ?                                                 */
/* ________________                                                 */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
for ( i_u = 0; i_u < np_u;    ++i_u )
  {
  fprintf(dbgfil(SURPAC),"sur891 New V curve:\n");
    for ( j_v = 0; j_v < np_v;    ++j_v )
    {
/*  Index in array                                            */
    i_i = i_u*np_v + j_v;

    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

    fprintf(dbgfil(SURPAC),
    "sur891 i_u %2d j_v %2d %12.4f %12.4f %12.4f w %8.6f\n",
    (int)i_u,(int)j_v, 
    (double)(p_cpts+i_i)->x_gm/weight,
    (double)(p_cpts+i_i)->y_gm/weight,
    (double)(p_cpts+i_i)->z_gm/weight,
       weight );
    }
  }

fflush(dbgfil(SURPAC));
}
#endif



/*!                                                                 */
/* Check that the multiplicity for the start point is order_v       */
/*                                                                 !*/
   
  f_close_v = 1;
  for ( j_kv = 0; j_kv < order_v-1; ++j_kv )
    {
    diff = fabs( *(p_vknots+j_kv+1) - *(p_vknots+j_kv));
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 j_kv= %d diff= %12.8f\n", (int)j_kv, diff  );
}
#endif
    if  ( diff > idknot) f_close_v = 0;
    }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_v == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is OK for the V start point\n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is not OK for the V start point ERROR\n");
  }
  fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Check that the multiplicity for the end   point is order_v       */
/*                                                                 !*/
   

  for ( j_kv = nk_v-order_v; j_kv < nk_v-1; ++j_kv )
    {
    diff = fabs( *(p_vknots+j_kv+1) - *(p_vknots+j_kv));
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 j_kv= %d diff= %12.8f\n", (int)j_kv, diff  );
}
#endif
    if  ( diff > idknot) f_close_v = 0;
    }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_v == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is OK for the V end   point\n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  Multiplicity is not OK for the V end   point ERROR\n");
  }
  fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Check that start node is equal to end node for all V "curves".   */
/*                                                                 !*/
   

  for ( i_u = 0; i_u < np_u; ++i_u )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
    fprintf(dbgfil(SURPAC),"sur891 New V curve:\n");
}
#endif
/*  Index in array for start point                            */
    i_i_start = i_u*np_v +  0 ;


/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (s)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start point                                               */
    poi_start.x_gm = (double)(p_cpts+i_i_start)->x_gm/weight;
    poi_start.y_gm = (double)(p_cpts+i_i_start)->y_gm/weight;
    poi_start.z_gm = (double)(p_cpts+i_i_start)->z_gm/weight;

/*  Index in array for end   point                            */
    i_i_end   = i_u*np_v +np_v-1;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (e)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   point                                               */
    poi_end.x_gm = (double)(p_cpts+i_i_end)->x_gm/weight;
    poi_end.y_gm = (double)(p_cpts+i_i_end)->y_gm/weight;
    poi_end.z_gm = (double)(p_cpts+i_i_end)->z_gm/weight;


/*  Distance between the points                               */
    diff = sqrt (
    (poi_start.x_gm-poi_end.x_gm)*(poi_start.x_gm-poi_end.x_gm)+ 
    (poi_start.y_gm-poi_end.y_gm)*(poi_start.y_gm-poi_end.y_gm)+ 
    (poi_start.z_gm-poi_end.z_gm)*(poi_start.z_gm-poi_end.z_gm));


#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 i_u=  %d diff= %12.8f\n", (int)i_u,  diff  );
}
#endif

/*  Not closed if difference exceeds idpoint.                 */
    if  ( diff > idpoint ) f_close_v = 0;


  }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
if ( f_close_v == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is closed in V direction \n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is open in V direction \n");
  }
  fflush(dbgfil(SURPAC));
}
#endif


  if ( f_close_v == 0 ) goto n_vclo;

/*!                                                                 */
/* 5. Periodic in V ?                                               */
/* _________________                                                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that line from the start node to start node + 1 is         */
/* equal to to the line between the end node and the end node -1,   */
/* i.e. that the lines are parallell and have the same length.      */
/*                                                                 !*/
   

  f_period_v = 1;
  for ( i_u = 0; i_u < np_u; ++i_u )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
    fprintf(dbgfil(SURPAC),"sur891 (Periodic) New V curve:\n");
}
#endif

/*  Index in array for start point                            */
    i_i_start = i_u*np_v +  0 ;

/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (s)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start point                                               */
    poi_start.x_gm = (double)(p_cpts+i_i_start)->x_gm/weight;
    poi_start.y_gm = (double)(p_cpts+i_i_start)->y_gm/weight;
    poi_start.z_gm = (double)(p_cpts+i_i_start)->z_gm/weight;

/*  Index in array for end   point                            */
    i_i_end   = i_u*np_v +np_v-1;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (e)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   point                                               */
    poi_end.x_gm = (double)(p_cpts+i_i_end)->x_gm/weight;
    poi_end.y_gm = (double)(p_cpts+i_i_end)->y_gm/weight;
    poi_end.z_gm = (double)(p_cpts+i_i_end)->z_gm/weight;


/*  Index in array for start tangent point                    */
    i_i_t_start = i_u*np_v +  1 ;

/*  Weight for the start point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_t_start));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (st)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  Start tangent point                                       */
    poi_stang.x_gm = (double)(p_cpts+i_i_t_start)->x_gm/weight;
    poi_stang.y_gm = (double)(p_cpts+i_i_t_start)->y_gm/weight;
    poi_stang.z_gm = (double)(p_cpts+i_i_t_start)->z_gm/weight;

/*  Index in array for end   tangent point                    */
    i_i_t_end   = i_u*np_v +np_v-2;

/*  Weight for the end   point                                */
    if ( p_w == NULL ) weight = 1.0;
    else               weight =   (double)(*(p_w+i_i_t_end  ));
    if ( fabs(weight) < 10.0*comptol)
      {
      sprintf(errbuf, "weight=0 (et)%%sur890");
      return(varkon_erpush("SU2993",errbuf));
      }

/*  End   tangent point                                       */
    poi_etang.x_gm = (double)(p_cpts+i_i_t_end)->x_gm/weight;
    poi_etang.y_gm = (double)(p_cpts+i_i_t_end)->y_gm/weight;
    poi_etang.z_gm = (double)(p_cpts+i_i_t_end)->z_gm/weight;

/*  End   tangent point "mirrored" in the end point           */
    poi_etang.x_gm = poi_end.x_gm+poi_end.x_gm-poi_etang.x_gm;
    poi_etang.y_gm = poi_end.y_gm+poi_end.y_gm-poi_etang.y_gm;
    poi_etang.z_gm = poi_end.z_gm+poi_end.z_gm-poi_etang.z_gm;




/*  Distance between the tangent points                       */
    diff = sqrt (
    (poi_stang.x_gm-poi_etang.x_gm)*(poi_stang.x_gm-poi_etang.x_gm)+ 
    (poi_stang.y_gm-poi_etang.y_gm)*(poi_stang.y_gm-poi_etang.y_gm)+ 
    (poi_stang.z_gm-poi_etang.z_gm)*(poi_stang.z_gm-poi_etang.z_gm));


#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur891 i_u=  %d diff= %12.8f (tangent points)\n", 
               (int)i_u, diff);
}
#endif

/*  Not periodic if difference exceeds idpoint.              */
    if  ( diff > idpoint ) f_period_v = 0;


  } /* End "curve" loop */


/*!                                                                 */
/* Check that the knot value differences are equal.                 */
/*                                                                 !*/
   
  j_kv = order_v+1;
  diff_start = *(p_vknots+j_kv+1) - *(p_vknots+j_kv);
  j_kv = nk_v - order_v - 1;
  diff_end   = *(p_vknots+j_kv+1) - *(p_vknots+j_kv);

  if  ( fabs(diff_start-diff_end) > idknot  ) f_period_v = 0;



#ifdef DEBUG  
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur891  j_kv= %d diff_start %f\n",
   (int)order_v, diff_start);
  fprintf(dbgfil(SURPAC),
  "sur891  j_kv= %d diff_end   %f\n",
   (int)(nk_v-order_v), diff_end  );
  fprintf(dbgfil(SURPAC),
  "sur891  Difference  %12.8f Criterion %12.8f\n",
   diff_start-diff_end, idknot );
}
if ( dbglev(SURPAC) == 2 )
{
if ( f_period_v == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is periodic in V direction \n");
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur891  The curve is not periodic in V direction \n");
  }
  fflush(dbgfil(SURPAC));
}
#endif

n_vclo:; /* Label: Not closed in V */

/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

  if       (  f_period_u == 1 && f_close_u == 1 ) *p_form_u = 2;
  else if  (  f_close_u  == 1 )                   *p_form_u = 1;
  else                                            *p_form_u = 0;
  if       (  f_period_v == 1 && f_close_v == 1 ) *p_form_v = 2;
  else if  (  f_close_v  == 1 )                   *p_form_v = 1;
  else                                            *p_form_v = 0;

#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur891 Exit*varkon_sur_nurbsacp  *p_form_u %d *p_form_v %d\n"
    , (int)(*p_form_u), (int)(*p_form_v) );
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


