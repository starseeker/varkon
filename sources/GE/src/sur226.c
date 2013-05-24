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
/*  Function: varkon_pat_coneval                   File: sur226.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a given a Consurf patch.     */
/*                                                                  */
/*  Author: Gunnar Liden                                           !*/
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-01   Originally written                                 */
/*  1996-02-23   Debug, unused parameters                           */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_coneval    Consurf patch evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short consurf();       /* Consurf patch                      */
static short initial();       /* Initialization of variables        */
#ifdef DEBUG
static short listing();       /* Listing of calculation data        */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat r_x,r_y,r_z;       /* Coordinates          r(u)      */
static DBfloat u_x,u_y,u_z;       /* Tangent             dr/du      */
static DBfloat v_x,v_y,v_z;       /* Tangent             dr/dv      */
static DBfloat u2_x,u2_y,u2_z;    /* Second derivative  d2r/du2     */
static DBfloat v2_x,v2_y,v2_z;    /* Second derivative  d2r/dv2     */
static DBfloat uv_x,uv_y,uv_z;    /* Twist vector       d2r/dudv    */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = sur226 Illegal computation case=  for varkon_pat_coneval*/
/* SU2973 = Internal function xxxxxx failed in varkon_pat_coneval   */
/* SU2993 = Severe program error in varkon_pat_coneval (sur226).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
         DBstatus varkon_pat_coneval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATN *p_patn,       /* Bicubic patch in surface          (ptr) */
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

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   icase_nmg;    /* Calculation case Uncertain ??? 1        */
                         /* Eq. 1: Only coordinates                 */
                         /* Eq. 2: First  derivatives               */
                         /* Eq. 3: Second derivatives               */
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

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
/*  Initialize output coordinates and derivatives. Call of initial. */
/*                                                                 !*/

   initial(icase,p_xyz); 

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226 Error icase= %d\n", (int)icase );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

        sprintf(errbuf,"%d%% varkon_pat_coneval (sur226)",(int)icase);
        return(varkon_erpush("SU2983",errbuf));
        }

   icase_nmg = I_UNDEF;
   if      ( icase == 0 )               icase_nmg = 1;
   else if ( icase >= 1 && icase <= 3 ) icase_nmg = 2;
   else                                 icase_nmg = 3;

/*!                                                                 */
/* 2. Calculate coordinates and derivatives (non-offset)            */
/* _____________________________________________________            */
/*                                                                  */
/*  Call of internal function consurf.                              */
/*                                                                 !*/

    status= consurf(p_patn,icase_nmg,u_pat,v_pat);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226 Error Internal function consurf failed \n" );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
        sprintf(errbuf,"consurf%%varkon_pat_coneval (sur226)");
        return(varkon_erpush("SU2973",errbuf));
        }
/*!                                                                 */
/* 6. Calculated data to output variables                           */
/* ______________________________________                           */
/*                                                                  */
/* Coordinates and derivatives to p_xyz                             */
/*                                                                 !*/

    p_xyz->r_x= r_x;
    p_xyz->r_y= r_y;
    p_xyz->r_z= r_z;

    p_xyz->u_x= u_x;
    p_xyz->u_y= u_y;
    p_xyz->u_z= u_z;

    p_xyz->v_x= v_x;
    p_xyz->v_y= v_y;
    p_xyz->v_z= v_z;

    p_xyz->u2_x= u2_x;
    p_xyz->u2_y= u2_y;
    p_xyz->u2_z= u2_z;

    p_xyz->v2_x= v2_x;
    p_xyz->v2_y= v2_y;
    p_xyz->v2_z= v2_z;

    p_xyz->uv_x= uv_x;
    p_xyz->uv_y= uv_y;
    p_xyz->uv_z= uv_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur226 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur226 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur226 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur226 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur226 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur226 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur226 Exit*varkon_pat_coneval* p_xyz %f %f %f\n",
    p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates coordinates and derivatives for          */
/* a given parameter ( u,v ) point on a consurf patch.              */

   static short consurf (p_patn,icase,u_pat,v_pat)
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATN *p_patn;       /* Pointer to the current consurf  patch   */
   DBint   icase;        /* Calculation case NMG: Os{ker !!??       */
                         /* Eq. 1: Only coordinates                 */
                         /* Eq. 2: Coordinates and dr/du derivative */
                         /* Eq. 3: Coordinates and dr/dv derivative */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Coordinates and derivatives r_x, r_y, r_z, ......          */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

   DBfloat nmg_data[100];/* Array as in NMG                         */

   DBfloat f_1_u;        /* Blending U functions                    */
   DBfloat f_2_u;        /*                                         */
   DBfloat f_3_u;        /*                                         */
   DBfloat f_4_u;        /*                                         */
   DBfloat f_5_u;        /*                                         */
   DBfloat f_6_u;        /*                                         */
   DBfloat f_1_v;        /* Blending V functions                    */
   DBfloat f_2_v;        /*                                         */
   DBfloat f_3_v;        /*                                         */
   DBfloat f_4_v;        /*                                         */
   DBfloat f_5_v;        /*                                         */
   DBfloat f_6_v;        /*                                         */

   DBfloat w[4];         /*                                         */
   DBfloat dw1;          /*                                         */
   DBfloat dw2;          /*                                         */
   DBint k_u;            /* Loop index longitudinal curve           */
   DBint i_k;            /*                                         */
   DBint i_j;            /*                                         */
   DBint i_c;            /*                                         */
   DBint j_5;            /*                                         */
   DBfloat w1;           /*                                         */
   DBfloat de1[5];       /*                                         */
   DBfloat de2[5];       /*                                         */
   DBfloat alpha1;       /*                                         */
   DBfloat alpha2;       /*                                         */
   DBfloat alpha3;       /*                                         */
   DBfloat alpha4;       /*                                         */
   DBfloat a[5];         /*                                         */
   DBfloat b[5];         /*                                         */
   DBfloat c[5];         /*                                         */
   DBfloat d[5];         /*                                         */
   DBfloat e[5];         /*                                         */

                         /* For lambda and mu calculation:          */
   DBint j_p;            /* Loop index segment of lambda, mu splines*/
   DBfloat u_lm;         /* Local parameter for lambda,mu segment   */
   DBfloat alfa;         /*                                         */
   DBfloat beta;         /*                                         */
   DBfloat gamma;        /*                                         */
   DBfloat delta;        /*                                         */
   DBfloat dl1;          /*                                         */
   DBfloat lmb;          /*                                         */
   DBfloat mu1;          /*                                         */
   DBfloat muu;          /*                                         */
   DBfloat dl2;          /*                                         */
   DBfloat mu2;          /*                                         */
   DBfloat xyz[3][6];    /* Coordinates and derivatives             */

                         /* Generatrix curve defined by Consurf     */
                         /* poylygon r1(u), r2(u), r3(u), r4(u):    */
   DBfloat r1[9];        /* Start         point                     */
   DBfloat r2[9];        /* Start tangent point                     */
   DBfloat r3[9];        /* End   tangent point                     */
   DBfloat r4[9];        /* End           point                     */

   DBfloat dir_v1[15];   /* For & aft line V1                       */
   DBfloat dir_v2[15];   /* For & aft line V2                       */
   DBfloat tan_u1[15];   /* Tangent definition line U1              */
   DBfloat tan_u2[15];   /* Tangent definition line U2              */
   DBint q;              /* Adjustment of array index               */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint i_x;            /* Loop index for initializations, ...     */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

  q = -1; /* Adjustment of array index */

  f_1_u  = F_UNDEF; 
  f_2_u  = F_UNDEF;
  f_3_u  = F_UNDEF;
  f_4_u  = F_UNDEF;
  f_5_u  = F_UNDEF;
  f_6_u  = F_UNDEF;
  f_1_v  = F_UNDEF; 
  f_2_v  = F_UNDEF;
  f_3_v  = F_UNDEF;
  f_4_v  = F_UNDEF;
  f_5_v  = F_UNDEF;
  f_6_v  = F_UNDEF;
  alpha1 = F_UNDEF;
  alpha2 = F_UNDEF;
  alpha3 = F_UNDEF;
  alpha4 = F_UNDEF;
  w1     = F_UNDEF;
  dw1    = F_UNDEF;
  dw2    = F_UNDEF;
  k_u    = I_UNDEF;
  i_k    = I_UNDEF;
  i_j    = I_UNDEF;
  i_c    = I_UNDEF;
  j_5    = I_UNDEF;
  j_p    = I_UNDEF;
  xyz[0][0] = F_UNDEF;    
  xyz[1][0] = F_UNDEF;    
  xyz[2][0] = F_UNDEF;    

  xyz[0][1] = F_UNDEF;    
  xyz[1][1] = F_UNDEF;    
  xyz[2][1] = F_UNDEF;    

  xyz[0][2] = F_UNDEF;    
  xyz[1][2] = F_UNDEF;    
  xyz[2][2] = F_UNDEF;    

  xyz[0][3] = F_UNDEF;    
  xyz[1][3] = F_UNDEF;    
  xyz[2][3] = F_UNDEF;    

  xyz[0][4] = F_UNDEF;    
  xyz[1][4] = F_UNDEF;    
  xyz[2][4] = F_UNDEF;    

  xyz[0][5] = F_UNDEF;    
  xyz[1][5] = F_UNDEF;    
  xyz[2][5] = F_UNDEF;    

  u_lm      = F_UNDEF;
  alfa      = F_UNDEF;
  beta      = F_UNDEF;
  gamma     = F_UNDEF;
  delta     = F_UNDEF;
  dl1       = F_UNDEF;
  lmb       = F_UNDEF;
  mu1       = F_UNDEF;
  muu       = F_UNDEF;
  dl2       = F_UNDEF;
  mu2       = F_UNDEF;

  for ( i_x = 0; i_x < 5; i_x= i_x+1  )
    {
    a[i_x]   = F_UNDEF; 
    b[i_x]   = F_UNDEF;
    c[i_x]   = F_UNDEF;
    d[i_x]   = F_UNDEF;
    e[i_x]   = F_UNDEF;
    de1[i_x] = F_UNDEF;
    de2[i_x] = F_UNDEF;
    }
  for ( i_x = 0; i_x < 4; i_x= i_x+1  )
    {
    w[i_x] = F_UNDEF;
    }
  for ( i_x = 0; i_x < 9; i_x= i_x+1  )
    {
    r1[i_x] = F_UNDEF;
    r2[i_x] = F_UNDEF;
    r3[i_x] = F_UNDEF;
    r4[i_x] = F_UNDEF;
    }
  for ( i_x = 0; i_x < 15; i_x= i_x+1  )
    {
    dir_v1[i_x] = F_UNDEF;
    dir_v2[i_x] = F_UNDEF;
    tan_u1[i_x] = F_UNDEF;
    tan_u2[i_x] = F_UNDEF;
    }
  
/*! 1. Patch data to array nmg_data                                !*/


  nmg_data[  0] =  p_patn->v1.x_gm;
  nmg_data[  1] =  p_patn->v1.y_gm;
  nmg_data[  2] =  p_patn->v1.z_gm;
  nmg_data[  3] =  p_patn->w11;  
  nmg_data[  4] =  p_patn->t11;  
  nmg_data[  5] =  p_patn->v2.x_gm;
  nmg_data[  6] =  p_patn->v2.y_gm;
  nmg_data[  7] =  p_patn->v2.z_gm;
  nmg_data[  8] =  p_patn->w14;  
  nmg_data[  9] =  p_patn->t14;  
  nmg_data[ 10] =  p_patn->u1.x_gm;
  nmg_data[ 11] =  p_patn->u1.y_gm;
  nmg_data[ 12] =  p_patn->u1.z_gm;
  nmg_data[ 13] =  p_patn->w12;  
  nmg_data[ 14] =  p_patn->t12;  
  nmg_data[ 15] =  p_patn->u2.x_gm;
  nmg_data[ 16] =  p_patn->u2.y_gm;
  nmg_data[ 17] =  p_patn->u2.z_gm;
  nmg_data[ 18] =  p_patn->w13;  
  nmg_data[ 19] =  p_patn->t13;  
  nmg_data[ 20] =  p_patn->dv1.x_gm;
  nmg_data[ 21] =  p_patn->dv1.y_gm;
  nmg_data[ 22] =  p_patn->dv1.z_gm;
  nmg_data[ 23] =  p_patn->w21;  
  nmg_data[ 24] =  p_patn->t21;  
  nmg_data[ 25] =  p_patn->dv2.x_gm;
  nmg_data[ 26] =  p_patn->dv2.y_gm;
  nmg_data[ 27] =  p_patn->dv2.z_gm;
  nmg_data[ 28] =  p_patn->w24;  
  nmg_data[ 29] =  p_patn->t24;  
  nmg_data[ 30] =  p_patn->du1.x_gm;
  nmg_data[ 31] =  p_patn->du1.y_gm;
  nmg_data[ 32] =  p_patn->du1.z_gm;
  nmg_data[ 33] =  p_patn->w22;  
  nmg_data[ 34] =  p_patn->t22;  
  nmg_data[ 35] =  p_patn->du2.x_gm;
  nmg_data[ 36] =  p_patn->du2.y_gm;
  nmg_data[ 37] =  p_patn->du2.z_gm;
  nmg_data[ 38] =  p_patn->w23;  
  nmg_data[ 39] =  p_patn->t23;  
  nmg_data[ 40] =  p_patn->v3.x_gm;
  nmg_data[ 41] =  p_patn->v3.y_gm;
  nmg_data[ 42] =  p_patn->v3.z_gm;
  nmg_data[ 43] =  p_patn->w31;  
  nmg_data[ 44] =  p_patn->t31;  
  nmg_data[ 45] =  p_patn->v4.x_gm;
  nmg_data[ 46] =  p_patn->v4.y_gm;
  nmg_data[ 47] =  p_patn->v4.z_gm;
  nmg_data[ 48] =  p_patn->w34;  
  nmg_data[ 49] =  p_patn->t34;  
  nmg_data[ 50] =  p_patn->u3.x_gm;
  nmg_data[ 51] =  p_patn->u3.y_gm;
  nmg_data[ 52] =  p_patn->u3.z_gm;
  nmg_data[ 53] =  p_patn->w32;  
  nmg_data[ 54] =  p_patn->t32;  
  nmg_data[ 55] =  p_patn->u4.x_gm;
  nmg_data[ 56] =  p_patn->u4.y_gm;
  nmg_data[ 57] =  p_patn->u4.z_gm;
  nmg_data[ 58] =  p_patn->w33;  
  nmg_data[ 59] =  p_patn->t33;  
  nmg_data[ 60] =  p_patn->dv3.x_gm;
  nmg_data[ 61] =  p_patn->dv3.y_gm;
  nmg_data[ 62] =  p_patn->dv3.z_gm;
  nmg_data[ 63] =  p_patn->w41;  
  nmg_data[ 64] =  p_patn->t41;  
  nmg_data[ 65] =  p_patn->dv4.x_gm;
  nmg_data[ 66] =  p_patn->dv4.y_gm;
  nmg_data[ 67] =  p_patn->dv4.z_gm;
  nmg_data[ 68] =  p_patn->w44;  
  nmg_data[ 69] =  p_patn->t44;  
  nmg_data[ 70] =  p_patn->du3.x_gm;
  nmg_data[ 71] =  p_patn->du3.y_gm;
  nmg_data[ 72] =  p_patn->du3.z_gm;
  nmg_data[ 73] =  p_patn->w42;  
  nmg_data[ 74] =  p_patn->t42;  
  nmg_data[ 75] =  p_patn->du4.x_gm;
  nmg_data[ 76] =  p_patn->du4.y_gm;
  nmg_data[ 77] =  p_patn->du4.z_gm;
  nmg_data[ 78] =  p_patn->w43;  
  nmg_data[ 79] =  p_patn->t43;  
  nmg_data[ 80] =  p_patn->lambda_0 ;
  nmg_data[ 81] =  p_patn->lambda_13;
  nmg_data[ 82] =  p_patn->lambda_23;
  nmg_data[ 83] =  p_patn->lambda_1 ;
  nmg_data[ 84] =  p_patn->mu_0 ; 
  nmg_data[ 85] =  p_patn->mu_13;
  nmg_data[ 86] =  p_patn->mu_23;
  nmg_data[ 87] =  p_patn->mu_1 ;
  nmg_data[ 88] =  p_patn->dlambda_0 ;
  nmg_data[ 89] =  p_patn->dlambda_13;
  nmg_data[ 90] =  p_patn->dlambda_23;
  nmg_data[ 91] =  p_patn->dlambda_1 ;
  nmg_data[ 92] =  p_patn->dmu_0 ; 
  nmg_data[ 93] =  p_patn->dmu_13;
  nmg_data[ 94] =  p_patn->dmu_23;
  nmg_data[ 95] =  p_patn->dmu_1 ;
  nmg_data[ 96] =  p_patn->p1;     
  nmg_data[ 97] =  p_patn->p2;     
  nmg_data[ 98] =  p_patn->p3;     
  nmg_data[ 99] =  F_UNDEF;        

#ifdef DEBUG
for ( i_x = 1; i_x <= 16*5; i_x= i_x+5  )
{
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf %15.5f %15.5f %15.5f %7.5f %7.5f\n",
 nmg_data[i_x-1], nmg_data[i_x+1-1], nmg_data[i_x+2-1], 
 nmg_data[i_x+3-1], nmg_data[i_x+4-1] );
}
}
for ( i_x = 1+16*5; i_x <= 16*5+2*8; i_x= i_x+8  )
{
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %7.5f\n",
 nmg_data[i_x-1],   nmg_data[i_x+1-1], nmg_data[i_x+2-1], 
 nmg_data[i_x+3-1], nmg_data[i_x+4-1], nmg_data[i_x+5-1], 
 nmg_data[i_x+6-1], nmg_data[i_x+7-1], nmg_data[i_x+8-1] );
}
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf %7.5f %7.5f %7.5f \n",
 nmg_data[ 96  ],   nmg_data[  97   ], nmg_data[  98   ]);
}
#endif


/*! 2. Blending U functions                                        !*/

  f_1_u = (1.0 - u_pat)*(1.0 - u_pat);  
  f_2_u = 2.0*u_pat  *f_1_u; 
  f_3_u = 2.0*(1.0-u_pat)*u_pat*u_pat;
  if  ( icase == 0 ) goto l_1;
  f_4_u = u_pat   - 1.0;  
  f_5_u = 3.0*u_pat - 2.0; 
  f_6_u = 3.0*u_pat - 1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf f_1_u %7.5f f_2_u %7.5f f_3_u %7.5f \n",
 f_1_u, f_2_u, f_3_u );
fprintf(dbgfil(SURPAC),
"sur226*consurf f_4_u %7.5f f_5_u %7.5f f_6_u %7.5f \n",
 f_4_u, f_5_u, f_6_u );
}
#endif

/*! 3. U section data                                              !*/

l_1:;

    for ( k_u = 1; k_u <= 4; ++k_u )
      {
      i_k  = (k_u-1)*5 + 4;
      w1  = f_1_u*nmg_data[i_k+q] + f_2_u*nmg_data[i_k+20+q]  +
            f_3_u*nmg_data[i_k+60+q] + nmg_data[i_k+40+q]*u_pat*u_pat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf k_u %d i_k %d \n", (int)k_u, (int)i_k );
fprintf(dbgfil(SURPAC),
"sur226*consurf  w1 %15.8f\n", w1);
}
#endif


      if  ( icase == 0 ) goto l_3;

      dw1 = 2.0*( f_4_u*(nmg_data[i_k+q] + f_6_u*nmg_data[i_k+20+q])  +
             u_pat  *(nmg_data[i_k+40+q] - f_5_u*nmg_data[i_k+60+q]));

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf dw1 %15.8f\n", dw1);
}
#endif

      if  ( icase  < 3 ) goto l_3;

      dw2 = 2.0*( nmg_data[i_k+q] + nmg_data[i_k+40+q]   +
           2.0*(f_5_u*nmg_data[i_k+20+q] - f_6_u*nmg_data[i_k+60+q]));

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf dw2 %15.8f\n", dw2);
}
#endif

l_3:;

        for ( j_5 = 1; j_5 <= 5; ++j_5 )
          {
          if  ( j_5   == 4 ) goto l_20;

          i_j  = i_k + j_5 - 4;
          if  ( j_5   == 5 ) goto l_1005;

          alpha1  = nmg_data[i_k+q   ]*nmg_data[i_j+q];
          alpha2  = nmg_data[i_k+20+q]*nmg_data[i_j+20+q];
          alpha3  = nmg_data[i_k+60+q]*nmg_data[i_j+60+q];
          alpha4  = nmg_data[i_k+40+q]*nmg_data[i_j+40+q];

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur226*consurf alpha1 %15.8f First place\n",alpha1);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha2 %15.8f\n",alpha2);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha3 %15.8f\n",alpha3);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha4 %15.8f\n",alpha4);
}
#endif

          goto l_4;
l_1005:;
          alpha1  = nmg_data[i_j+q];
          alpha2  = nmg_data[i_j+20+q];
          alpha3  = nmg_data[i_j+60+q];
          alpha4  = nmg_data[i_j+40+q];
          w1  = 1.0;
          dw1 = 0.0;
          dw2 = 0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur226*consurf alpha1 %15.8f Second place\n",alpha1);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha2 %15.8f\n",alpha2);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha3 %15.8f\n",alpha3);
fprintf(dbgfil(SURPAC),"sur226*consurf alpha4 %15.8f\n",alpha4);
fprintf(dbgfil(SURPAC),"sur226*consurf     w1 %15.8f\n",    w1);
fprintf(dbgfil(SURPAC),"sur226*consurf    dw1 %15.8f\n",   dw1);
fprintf(dbgfil(SURPAC),"sur226*consurf    dw2 %15.8f\n",   dw2);
}
#endif

l_4:;
          e[j_5+q]  = f_1_u*alpha1 + f_2_u*alpha2 + f_3_u*alpha3+alpha4*u_pat*u_pat;
          if ( fabs(w1) < 0.0000000001 )
             {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226 Error l_4 w1 = %f (=0) u_pat %7.4f v_pat %7.4f\n", w1, u_pat, v_pat );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
             sprintf(errbuf,"w1=0 (1)%%varkon_pat_coneval (sur226)");
             return(varkon_erpush("SU2993",errbuf));
             }

          e[j_5+q]  = e[j_5+q]/w1;
          if  ( icase == 0 ) goto l_20;

          de1[j_5+q]  = ( 2.0*(f_4_u*(alpha1 + f_6_u*alpha2) +
                 u_pat  *(alpha4 - f_5_u*alpha3)) - e[j_5+q]*dw1)/w1;
          if  ( icase  < 3 ) goto l_20;

          de2[j_5+q] =(2.0*(alpha1 + alpha4 + 2.0*(f_5_u*alpha2
                   -f_6_u*alpha3)) - e[j_5+q]*dw2-2.0*de1[j_5+q]*dw1)/w1;
l_20:;
        }      /*        End loop j_5           */


        if ( k_u == 1 ) 
          {
          dir_v1[ 0]  =   e[0];
          dir_v1[ 1]  =   e[1];
          dir_v1[ 2]  =   e[2];
          dir_v1[ 3]  =   e[3];
          dir_v1[ 4]  =   e[4];
          dir_v1[ 5]  = de1[0];
          dir_v1[ 6]  = de1[1];
          dir_v1[ 7]  = de1[2];
          dir_v1[ 8]  = de1[3];
          dir_v1[ 9]  = de1[4];
          dir_v1[10]  = de2[0];
          dir_v1[11]  = de2[1];
          dir_v1[12]  = de2[2];
          dir_v1[13]  = de2[3];
          dir_v1[14]  = de2[4];
          }
        else if ( k_u == 2 )
          {
          dir_v2[ 0]  =   e[0];
          dir_v2[ 1]  =   e[1];
          dir_v2[ 2]  =   e[2];
          dir_v2[ 3]  =   e[3];
          dir_v2[ 4]  =   e[4];
          dir_v2[ 5]  = de1[0];
          dir_v2[ 6]  = de1[1];
          dir_v2[ 7]  = de1[2];
          dir_v2[ 8]  = de1[3];
          dir_v2[ 9]  = de1[4];
          dir_v2[10]  = de2[0];
          dir_v2[11]  = de2[1];
          dir_v2[12]  = de2[2];
          dir_v2[13]  = de2[3];
          dir_v2[14]  = de2[4];
          }
        else if ( k_u == 3 )
          {
          tan_u1[ 0]  =   e[0];
          tan_u1[ 1]  =   e[1];
          tan_u1[ 2]  =   e[2];
          tan_u1[ 3]  =   e[3];
          tan_u1[ 4]  =   e[4];
          tan_u1[ 5]  = de1[0];
          tan_u1[ 6]  = de1[1];
          tan_u1[ 7]  = de1[2];
          tan_u1[ 8]  = de1[3];
          tan_u1[ 9]  = de1[4];
          tan_u1[10]  = de2[0];
          tan_u1[11]  = de2[1];
          tan_u1[12]  = de2[2];
          tan_u1[13]  = de2[3];
          tan_u1[14]  = de2[4];
          }
        else if ( k_u == 4 )
          {
          tan_u2[ 0]  =   e[0];
          tan_u2[ 1]  =   e[1];
          tan_u2[ 2]  =   e[2];
          tan_u2[ 3]  =   e[3];
          tan_u2[ 4]  =   e[4];
          tan_u2[ 5]  = de1[0];
          tan_u2[ 6]  = de1[1];
          tan_u2[ 7]  = de1[2];
          tan_u2[ 8]  = de1[3];
          tan_u2[ 9]  = de1[4];
          tan_u2[10]  = de2[0];
          tan_u2[11]  = de2[1];
          tan_u2[12]  = de2[2];
          tan_u2[13]  = de2[3];
          tan_u2[14]  = de2[4];
          }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur226*consurf  e[0]  %15.8f\n", e[0] );
fprintf(dbgfil(SURPAC),"sur226*consurf  e[1]  %15.8f\n", e[1] );
fprintf(dbgfil(SURPAC),"sur226*consurf  e[2]  %15.8f\n", e[2] );
fprintf(dbgfil(SURPAC),"sur226*consurf  e[3]  %15.8f\n", e[3] );
fprintf(dbgfil(SURPAC),"sur226*consurf  e[4]  %15.8f\n", e[4] );
fprintf(dbgfil(SURPAC),"sur226*consurf de1[0] %15.8f\n",de1[0]);
fprintf(dbgfil(SURPAC),"sur226*consurf de1[1] %15.8f\n",de1[1]);
fprintf(dbgfil(SURPAC),"sur226*consurf de1[2] %15.8f\n",de1[2]);
fprintf(dbgfil(SURPAC),"sur226*consurf de1[3] %15.8f\n",de1[3]);
fprintf(dbgfil(SURPAC),"sur226*consurf de1[4] %15.8f\n",de1[4]);
fprintf(dbgfil(SURPAC),"sur226*consurf de2[0] %15.8f\n",de2[0]);
fprintf(dbgfil(SURPAC),"sur226*consurf de2[1] %15.8f\n",de2[1]);
fprintf(dbgfil(SURPAC),"sur226*consurf de2[2] %15.8f\n",de2[2]);
fprintf(dbgfil(SURPAC),"sur226*consurf de2[3] %15.8f\n",de2[3]);
fprintf(dbgfil(SURPAC),"sur226*consurf de2[4] %15.8f\n",de2[4]);
}
#endif

     }         /*     End loop k_u    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 0] %15.8f\n",dir_v1[ 0]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 1] %15.8f\n",dir_v1[ 1]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 2] %15.8f\n",dir_v1[ 2]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 3] %15.8f\n",dir_v1[ 3]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 4] %15.8f\n",dir_v1[ 4]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 5] %15.8f\n",dir_v1[ 5]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 6] %15.8f\n",dir_v1[ 6]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 7] %15.8f\n",dir_v1[ 7]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 8] %15.8f\n",dir_v1[ 8]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[ 9] %15.8f\n",dir_v1[ 9]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[10] %15.8f\n",dir_v1[10]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[11] %15.8f\n",dir_v1[11]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[12] %15.8f\n",dir_v1[12]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[13] %15.8f\n",dir_v1[13]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v1[14] %15.8f\n",dir_v1[14]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 0] %15.8f\n",dir_v2[ 0]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 1] %15.8f\n",dir_v2[ 1]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 2] %15.8f\n",dir_v2[ 2]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 3] %15.8f\n",dir_v2[ 3]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 4] %15.8f\n",dir_v2[ 4]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 5] %15.8f\n",dir_v2[ 5]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 6] %15.8f\n",dir_v2[ 6]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 7] %15.8f\n",dir_v2[ 7]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 8] %15.8f\n",dir_v2[ 8]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[ 9] %15.8f\n",dir_v2[ 9]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[10] %15.8f\n",dir_v2[10]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[11] %15.8f\n",dir_v2[11]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[12] %15.8f\n",dir_v2[12]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[13] %15.8f\n",dir_v2[13]);
fprintf(dbgfil(SURPAC),"sur226*consurf dir_v2[14] %15.8f\n",dir_v2[14]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 0] %15.8f\n",tan_u1[ 0]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 1] %15.8f\n",tan_u1[ 1]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 2] %15.8f\n",tan_u1[ 2]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 3] %15.8f\n",tan_u1[ 3]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 4] %15.8f\n",tan_u1[ 4]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 5] %15.8f\n",tan_u1[ 5]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 6] %15.8f\n",tan_u1[ 6]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 7] %15.8f\n",tan_u1[ 7]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 8] %15.8f\n",tan_u1[ 8]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[ 9] %15.8f\n",tan_u1[ 9]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[10] %15.8f\n",tan_u1[10]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[11] %15.8f\n",tan_u1[11]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[12] %15.8f\n",tan_u1[12]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[13] %15.8f\n",tan_u1[13]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u1[14] %15.8f\n",tan_u1[14]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 0] %15.8f\n",tan_u2[ 0]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 1] %15.8f\n",tan_u2[ 1]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 2] %15.8f\n",tan_u2[ 2]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 3] %15.8f\n",tan_u2[ 3]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 4] %15.8f\n",tan_u2[ 4]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 5] %15.8f\n",tan_u2[ 5]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 6] %15.8f\n",tan_u2[ 6]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 7] %15.8f\n",tan_u2[ 7]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 8] %15.8f\n",tan_u2[ 8]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[ 9] %15.8f\n",tan_u2[ 9]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[10] %15.8f\n",tan_u2[10]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[11] %15.8f\n",tan_u2[11]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[12] %15.8f\n",tan_u2[12]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[13] %15.8f\n",tan_u2[13]);
fprintf(dbgfil(SURPAC),"sur226*consurf tan_u2[14] %15.8f\n",tan_u2[14]);
}
#endif


/*! 4. Lambda and mu values                                        !*/

      j_p  =1;
      if ( u_pat  >  0.3333 ) j_p  = 2;
      if ( u_pat  >  0.6667 ) j_p  = 3;

      u_lm  = 3.0*u_pat-(DBfloat)j_p+1.0;

l_25:;

      alfa   = nmg_data[j_p+80+q];
      beta   = alfa + nmg_data[j_p+88+q]/6.0;
      delta  = nmg_data[j_p+81+q];
      gamma  = delta - nmg_data[j_p+89+q]/6.0;
      a[0]   = alfa;
      b[0]   = 2.0*(beta - alfa);
      c[0]   = alfa - 4.0*beta + 2.0*gamma + delta;
      d[0]   = 2.0*(beta - gamma);
      a[1]   = a[0] +u_lm*(b[0] +u_lm*(c[0] +u_lm*d[0]));
      if  ( icase == 0 ) goto l_27;

      a[2]   = 3.0*( b[0] + u_lm*( 2.0*c[0] +  3.0*u_lm*d[0] ) );
      if  ( icase  < 3 ) goto l_27;

      a[3]   = 18.0*c[0]  +  54.0*u_lm*d[0];

l_27:;

      if  ( j_p > 3    ) goto l_30;

      j_p   = j_p + 4;
      lmb    = a[1];
      dl1    = a[2];
      dl2    = a[3];
 
      goto l_25;

l_30:;

      muu    = a[1];
      mu1    = a[2];
      mu2    = a[3];


/*! 5. Blending v functions                                        !*/

      f_1_v  = (1.0 - v_pat  )*(1.0 - v_pat  );
      f_2_v  = 2.0*v_pat  *f_1_v;
      f_3_v  = 2.0*(1.0-v_pat)*v_pat*v_pat;
      if  ( icase == 0 ) goto l_49;
      f_4_v  = 2.0*(v_pat-1.0);
      f_5_v  = 2.0*(3.0*v_pat-1.0)*(v_pat  -1.0);
      f_6_v  = 2.0*v_pat  *(2.0 - 3.0*v_pat);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226*consurf f_1_v %7.5f f_2_v %7.5f f_3_v %7.5f \n",
 f_1_v, f_2_v, f_3_v );
fprintf(dbgfil(SURPAC),
"sur226*consurf f_4_v %7.5f f_5_v %7.5f f_6_v %7.5f \n",
 f_4_v, f_5_v, f_6_v );
}
#endif

l_49:;

/*! 6. ......                                                      !*/

      for ( i_c = 1; i_c <= 3; ++i_c )
        {
        r1[i_c-1]    = dir_v1[i_c-1];
        r2[i_c-1]    = dir_v1[i_c-1] + lmb*(tan_u1[i_c-1] - dir_v1[i_c-1]);
        r3[i_c-1]    = dir_v2[i_c-1] + muu*(tan_u2[i_c-1] - dir_v2[i_c-1]);
        r4[i_c-1]    = dir_v2[i_c-1];
        if  ( icase == 0 ) goto l_50;

        r1[i_c+3-1]  = dir_v1[i_c+5-1];
        r2[i_c+3-1]  = dir_v1[i_c+5-1]+  dl1 *(tan_u1[i_c-1]   - dir_v1[i_c-1])  +  
                                     lmb *(tan_u1[i_c+5-1] - dir_v1[i_c+5-1]);
        r3[i_c+3-1]  = dir_v2[i_c+5-1] + mu1*(tan_u2[i_c-1]    - dir_v2[i_c-1])  + 
                                     muu*(tan_u2[i_c+5-1]  - dir_v2[i_c+5-1]);
        r4[i_c+3-1]  = dir_v2[i_c+5-1];
        if  ( icase  < 3 ) goto l_50;

        r1[i_c+6-1]  = dir_v1[i_c+10-1];
        r2[i_c+6-1]  = dir_v1[i_c+10-1]+dl2*(tan_u1[i_c-1]    - dir_v1[i_c-1])  +
                                2.0*dl1*(tan_u1[i_c+5-1]  - dir_v1[i_c+5-1])+
                                    lmb*(tan_u1[i_c+10-1] - dir_v1[i_c+10-1]);
        r3[i_c+6-1]  = dir_v2[i_c+10-1]+mu2*(tan_u2[i_c-1]    - dir_v2[i_c-1])  +
                                2.0*mu1*(tan_u2[i_c+5-1]  - dir_v2[i_c+5-1])+
                                    muu*(tan_u2[i_c+10-1] - dir_v2[i_c+10-1]);
        r4[i_c+6-1]  = dir_v2[i_c+10-1];

l_50:;

     }        /* End loop i_c */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur226*consurf r1[0] %15.8f\n", r1[0] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[1] %15.8f\n", r1[1] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[2] %15.8f\n", r1[2] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[3] %15.8f\n", r1[3] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[4] %15.8f\n", r1[4] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[5] %15.8f\n", r1[5] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[6] %15.8f\n", r1[6] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[7] %15.8f\n", r1[7] );
fprintf(dbgfil(SURPAC),"sur226*consurf r1[8] %15.8f\n", r1[8] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[0] %15.8f\n", r2[0] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[1] %15.8f\n", r2[1] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[2] %15.8f\n", r2[2] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[3] %15.8f\n", r2[3] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[4] %15.8f\n", r2[4] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[5] %15.8f\n", r2[5] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[6] %15.8f\n", r2[6] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[7] %15.8f\n", r2[7] );
fprintf(dbgfil(SURPAC),"sur226*consurf r2[8] %15.8f\n", r2[8] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[0] %15.8f\n", r3[0] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[1] %15.8f\n", r3[1] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[2] %15.8f\n", r3[2] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[3] %15.8f\n", r3[3] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[4] %15.8f\n", r3[4] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[5] %15.8f\n", r3[5] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[6] %15.8f\n", r3[6] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[7] %15.8f\n", r3[7] );
fprintf(dbgfil(SURPAC),"sur226*consurf r3[8] %15.8f\n", r3[8] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[0] %15.8f\n", r4[0] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[1] %15.8f\n", r4[1] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[2] %15.8f\n", r4[2] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[3] %15.8f\n", r4[3] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[4] %15.8f\n", r4[4] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[5] %15.8f\n", r4[5] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[6] %15.8f\n", r4[6] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[7] %15.8f\n", r4[7] );
fprintf(dbgfil(SURPAC),"sur226*consurf r4[8] %15.8f\n", r4[8] );
}
#endif

      if  ( icase == 0 ) goto l_55;

      dw1  = dir_v1[5-1]*f_4_v + 
             tan_u1[5-1]*f_5_v + 
             tan_u2[5-1]*f_6_v + 
             2.0*v_pat  *dir_v2[5-1];
      if  ( icase  < 3 ) goto l_55;

      dw2  = 2.0*dir_v1[5-1] +
             tan_u1[5-1]*(12.0*v_pat  -8.0) +
             tan_u2[5-1]*(4.0-12.0*v_pat  )+2.0*dir_v2[5-1];

l_55:;

    for ( i_c = 1; i_c <= 3; ++i_c )
      {
      i_j  =5*i_c;
      if  ( icase  == 1  && i_c  > 1 ) goto l_62;
      if  ( icase  <  3  && i_c == 3 ) goto l_62;

      w[i_c-1]  = f_1_v*dir_v1[i_j-1] + 
                 f_2_v*tan_u1[i_j-1] + 
                 f_3_v*tan_u2[i_j-1] + 
                 dir_v2[i_j-1]*v_pat*v_pat;
      }

l_62:;


/*! 7. Coordinates and derivatives                                 !*/



    for ( i_c = 1; i_c <= 3; ++i_c )
      {

/* Coordinates xyz(1-3,1)                    */

   if ( fabs(w[0]) < 0.0000000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226 Error  w[0] = %f (=0) u_pat %7.4f v_pat %7.4f\n", w[0], u_pat, v_pat );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       sprintf(errbuf,"w(0)=0 %%varkon_pat_coneval (sur226)");
       return(varkon_erpush("SU2993",errbuf));
       }

      xyz[i_c-1][0] =(f_1_v*dir_v1[4]*r1[i_c-1] + 
                   f_2_v*tan_u1[4]*r2[i_c-1] + 
                   f_3_v*tan_u2[4]*r3[i_c-1] +
                   r4[i_c-1]*dir_v2[4]*v_pat*v_pat)/w[0];
      if  ( icase  == 1 ) goto l_70;

/* First derivative dr/du (1-3,2)  */

      xyz[i_c-1][1] =(f_1_v*(dir_v1[ 9]*r1[i_c-1]+dir_v1[4]*r1[i_c+3-1])+
                   f_2_v*(tan_u1[ 9]*r2[i_c-1]+tan_u1[4]*r2[i_c+3-1])+
                   f_3_v*(tan_u2[ 9]*r3[i_c-1]+tan_u2[4]*r3[i_c+3-1])+
                   (dir_v2[ 9]*r4[i_c-1]+dir_v2[4]*r4[i_c+3-1])*v_pat*v_pat-
                   xyz[i_c-1][0]*w[1])/w[0];

/* First derivative dr/dv (1-3,3) */

      xyz[i_c-1][2] =(f_4_v*dir_v1[4]*r1[i_c-1]+
                   f_5_v*tan_u1[4]*r2[i_c-1]+
                   f_6_v*tan_u2[4]*r3[i_c-1]+
                   2.0*v_pat  *dir_v2[4]*r4[i_c-1]-
                   xyz[i_c-1][0]*dw1)/w[0];

      if  ( icase  < 3 ) goto l_70;

/* Second derivative d2r/du2 (1-3,4)  */

 xyz[i_c-1][3] =f_1_v*(dir_v1[14]*r1[i_c-1]+2.0*dir_v1[ 9]*
              r1[i_c+3-1]+dir_v1[4]*r1[i_c+6-1]) +
       f_2_v*(tan_u1[14]*r2[i_c-1]+2.0*tan_u1[ 9]*r2[i_c+3-1]+
              tan_u1[4]*r2[i_c+6-1])+f_3_v*
            (tan_u2[14]*r3[i_c-1] + 2.0*tan_u2[ 9]*r3[i_c+3-1] + tan_u2[4]*r3[i_c+6-1]) +
            (dir_v2[14]*r4[i_c-1] + 2.0*dir_v2[ 9]*r4[i_c+3-1] + 
             dir_v2[4]*r4[i_c+6-1])*v_pat*v_pat;
      xyz[i_c-1][3] =(xyz[i_c-1][3]-2.0*xyz[i_c-1][1]*w[1]-xyz[i_c-1][0]*w[2])/w[0];

/* Second derivative d2r/dudv (1-3,5)  */

      xyz[i_c-1][4] =(f_4_v*(dir_v1[ 9]*r1[i_c-1]+dir_v1[4]*r1[i_c+3-1])+
                   f_5_v*(tan_u1[ 9]*r2[i_c-1]+tan_u1[4]*r2[i_c+3-1])+
                   f_6_v*(tan_u2[ 9]*r3[i_c-1]+tan_u2[4]*r3[i_c+3-1])+
                   2.0*v_pat  *(dir_v2[ 9]*r4[i_c-1]+dir_v2[4]*r4[i_c+3-1])-
                   xyz[i_c-1][1]*dw1-xyz[i_c-1][2]*w[1]-xyz[i_c-1][0]*
                   (f_4_v*dir_v1[ 9]+f_5_v*tan_u1[ 9]+
                    f_6_v*tan_u2[ 9]+2.0*v_pat  *dir_v2[ 9]))/w[0];


/* Second derivative d2r/dv2 (1-3,6)   */


      xyz[i_c-1][5] =(2.0*dir_v1[4]*r1[i_c-1]+(12.0*v_pat  -8.0)*tan_u1[4]*r2[i_c-1]
            +(4.0-12.0*v_pat  )*tan_u2[4]*r3[i_c-1]+2.0*dir_v2[4]*r4[i_c-1]
            -2.0*xyz[i_c-1][2]*dw1-xyz[i_c-1][0]*dw2)/w[0];
l_70:;

    }        /*  End loop i_c  */




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226        X %15.8f      Y %15.8f      Z %15.8f\n",
 xyz[0][0], xyz[1][0], xyz[2][0] );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226     DXDU %15.8f   DYDU %15.8f    DZDU %15.8f\n",
 xyz[0][1], xyz[1][1], xyz[2][1] );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226     DXDV %15.8f   DYDV %15.8f   DZDV %15.8f\n",
 xyz[0][2], xyz[1][2], xyz[2][2] );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226   D2XDU2 %15.8f  D2YDU2 %15.8f  D2ZDU2 %15.8f\n",
 xyz[0][3], xyz[1][3], xyz[2][3] );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226   D2XDV2 %15.8f  D2YDV2 %15.8f  D2ZDV2 %15.8f\n",
 xyz[0][5], xyz[1][5], xyz[2][5] );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur226 D2DXDUDV %15.8f D2YDUDV %15.8f D2ZDUDV %15.8f\n",
 xyz[0][4], xyz[1][4], xyz[2][4] );
}
#endif





    r_x= xyz[0][0];    
    r_y= xyz[1][0];    
    r_z= xyz[2][0];    

    u_x= xyz[0][1];    
    u_y= xyz[1][1];    
    u_z= xyz[2][1];    

    v_x= xyz[0][2];    
    v_y= xyz[1][2];    
    v_z= xyz[2][2];    

    u2_x= xyz[0][3];    
    u2_y= xyz[1][3];    
    u2_z= xyz[2][3];    

    uv_x= xyz[0][4];    
    uv_y= xyz[1][4];    
    uv_z= xyz[2][4];    

    v2_x= xyz[0][5];    
    v2_y= xyz[1][5];    
    v2_z= xyz[2][5];    

#ifdef  DEBUG
   if ( SQRT(u_x*u_x + u_y*u_y + u_z*u_z ) < 0.0001 ||
        SQRT(v_x*v_x + v_y*v_y + v_z*v_z ) < 0.0001    )
     {
     listing(icase,u_pat,v_pat,xyz); 
     }
#endif



    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/





/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF.                            */

   static short initial(icase,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  icase;         /* Calculation case ..                     */
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

/* 1. Printout of input variables                                   */
/*                                                                 !*/
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur226 *** initial: icase= %d p_xyz= %d\n", (int)icase, (int)p_xyz);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
/*!                                                                 */
/* 2. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   

    r_x= F_UNDEF;   
    r_y= F_UNDEF;   
    r_z= F_UNDEF;   

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;   
    p_xyz->u_y= F_UNDEF;   
    p_xyz->u_z= F_UNDEF;   

    u_x= F_UNDEF;   
    u_y= F_UNDEF;   
    u_z= F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

    v_x= F_UNDEF;   
    v_y= F_UNDEF;   
    v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

    u2_x= F_UNDEF;   
    u2_y= F_UNDEF;   
    u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

    v2_x= F_UNDEF;   
    v2_y= F_UNDEF;   
    v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

    uv_x= F_UNDEF;   
    uv_y= F_UNDEF;   
    uv_z= F_UNDEF;   

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


/*!New-Page--------------------------------------------------------!*/

#ifdef DEBUG
/*!********* Internal ** function * listing *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* List data from computation                                       */

   static short listing(icase,u_pat,v_pat,xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   icase;        /* Calculation case ..                     */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   DBfloat xyz[3][6];    /* Coordinates and derivatives             */

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

/* 1. Printout of all data                                          */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing icase= %d u_pat %f v_pat %f\n", (int)icase, u_pat, v_pat);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing        X %15.8f      Y %15.8f      Z %15.8f\n",
 xyz[0][0], xyz[1][0], xyz[2][0] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing     DXDU %15.8f   DYDU %15.8f    DZDU %15.8f\n",
 xyz[0][1], xyz[1][1], xyz[2][1] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing     DXDV %15.8f   DYDV %15.8f   DZDV %15.8f\n",
 xyz[0][2], xyz[1][2], xyz[2][2] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing   D2XDU2 %15.8f  D2YDU2 %15.8f  D2ZDU2 %15.8f\n",
 xyz[0][3], xyz[1][3], xyz[2][3] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing   D2XDV2 %15.8f  D2YDV2 %15.8f  D2ZDV2 %15.8f\n",
 xyz[0][5], xyz[1][5], xyz[2][5] );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur226*listing D2DXDUDV %15.8f D2YDUDV %15.8f D2ZDUDV %15.8f\n",
 xyz[0][4], xyz[1][4], xyz[2][4] );
}




    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

#endif  /* End DEBUG  */
