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
/*  Function: varkon_pat_contra                    File: sur626.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a Consurf patch with the input transformation matrix */
/*  (the input coordinate system).                                  */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-04   Originally written                                 */
/*  1996-02-03   Core line point and normal                         */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_contra     Transform Consurf patch          */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GEtform_det            * Determinant                             */
/* GEtfpos_to_local       * Transform a point                       */
/* GEtfvec_to_local       * Transform a vector                      */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_contra (sur626)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_contra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATN *p_patin,       /* Input patch                       (ptr) */
  DBTmat *p_c,           /* Transformation matrix             (ptr) */
  GMPATN *p_patout )     /* Output patch                      (ptr) */

/* Out:                                                             */
/*        Transformed points to p_patout                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBVector poi_in;      /* Input point for GEtfpos_to_local        */
   DBVector poi_tra;     /* Transformed point                       */
   DBfloat  deter;         /* Determinant of p_c                      */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The Consurf patch is defined by points and scalar functions.     */
/* It is for a orthogonal (no scaling) transformation sufficient    */
/* to transform the points. The scalar function data shall not      */
/* be changed for this case.                                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_pat_contra (sur626) ********\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )
 {
 sprintf(errbuf, "(p_c)%%varkon_pat_contra (sur626");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/*  Check that the transformation matrix is orthogonal.             */
/*  The determinant is checked .......                              */
/*                                                                 !*/
/*!                                                                 */

    GEtform_det(p_c,&deter);
    if ( fabs(deter-1.0) > 0.0001  ) 
       {    
       sprintf(errbuf, "(determinant)%%varkon_pat_contra (sur626"); 
       return(varkon_erpush("SU2993",errbuf));
       }

/* 2. Transform data                                                */
/* ___________________                                              */
/*                                                                 !*/


   poi_in = p_patin->v1;       /* Tile corner point 1               */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->v1 = poi_tra;
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," p_patin->v1 %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC),"p_patout->v1 %f %f %f \n",
   p_patout->v1.x_gm, p_patout->v1.x_gm, p_patout->v1.x_gm  );
  }
#endif

p_patout->w11 = p_patin->w11;   /* Weight for v1                     */
p_patout->t11 = p_patin->t11;   /* P value function value            */

   poi_in = p_patin->v2;        /* Tile corner point 2               */
   GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->v2 = poi_tra;
 
p_patout->w14 = p_patin->w14;   /* Weight for v2                     */
p_patout->t14 = p_patin->t14;   /* P value function value            */

   poi_in = p_patin->u1;        /* Cross slope defining point for v1 */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->u1 = poi_tra;
 
p_patout->w12 = p_patin->w12;   /* Weight for u1                     */
p_patout->t12 = p_patin->t12;   /* P value function value            */

   poi_in = p_patin->u2;        /* Cross slope defining point for v2 */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->u2 = poi_tra;
 
p_patout->w13 = p_patin->w13;   /* Weight for u2                     */
p_patout->t13 = p_patin->t13;   /* P value function value            */

   poi_in = p_patin->dv1;       /* Fore/aft tangent point for v1     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->dv1 = poi_tra;
 
p_patout->w21 = p_patin->w21;   /* Weight for dv1                    */
p_patout->t21 = p_patin->t21;   /* P value function value            */

   poi_in = p_patin->dv2;       /* Fore/aft tangent point for v2     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->dv2 = poi_tra;
 
p_patout->w24 = p_patin->w24;   /* Weight for dv2                    */
p_patout->t24 = p_patin->t24;   /* P value function value            */

   poi_in = p_patin->du1;       /* Fore/aft tangent point for u1     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->du1 = poi_tra;
 
p_patout->w22 = p_patin->w22;   /* Weight for du1                    */
p_patout->t22 = p_patin->t22;   /* P value function value            */

   poi_in = p_patin->du2;       /* Fore/aft tangent point for u2     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->du2 = poi_tra;
 
p_patout->w23 = p_patin->w23;   /* Weight for du2                    */
p_patout->t23 = p_patin->t23;   /* P value function value            */

   poi_in = p_patin->v3;        /* Tile corner point 3               */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->v3 = poi_tra;
 
p_patout->w31 = p_patin->w31;   /* Weight for v3                     */
p_patout->t31 = p_patin->t31;   /* P value function value            */

   poi_in = p_patin->v4;        /* Tile corner point 4               */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->v4 = poi_tra;
 
p_patout->w34 = p_patin->w34;   /* Weight for v4                     */
p_patout->t34 = p_patin->t34;   /* P value function value            */

   poi_in = p_patin->u3;        /* Cross slope defining point for v3 */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->u3 = poi_tra;
 
p_patout->w32 = p_patin->w32;   /* Weight for u3                     */
p_patout->t32 = p_patin->t32;   /* P value function value            */

   poi_in = p_patin->u4;        /* Cross slope defining point for v4 */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->u4 = poi_tra;
 
p_patout->w33 = p_patin->w33;   /* Weight for u4                     */
p_patout->t33 = p_patin->t33;   /* P value function value            */

   poi_in = p_patin->dv3;       /* Fore/aft tangent point for v3     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->dv3 = poi_tra;
 
p_patout->w41 = p_patin->w41;   /* Weight for dv3                    */
p_patout->t41 = p_patin->t41;   /* P value function value            */

   poi_in = p_patin->dv4;       /* Fore/aft tangent point for v4     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->dv4 = poi_tra;
 
p_patout->w44 = p_patin->w44;   /* Weight for dv4                    */
p_patout->t44 = p_patin->t44;   /* P value function value            */

   poi_in = p_patin->du3;       /* Fore/aft tangent point for u3     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->du3 = poi_tra;
 
p_patout->w42 = p_patin->w42;   /* Weight for du3                    */
p_patout->t42 = p_patin->t42;   /* P value function value            */

   poi_in = p_patin->du4;       /* Fore/aft tangent point for u4     */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->du4 = poi_tra;
 
p_patout->w43 = p_patin->w43;   /* Weight for du4                    */
p_patout->t43 = p_patin->t43;   /* P value function value            */

p_patout->lambda_0   = p_patin->lambda_0; /* Lambda  U=  0           */
p_patout->lambda_13  = p_patin->lambda_13;/* Lambda  U= 1/3          */
p_patout->lambda_23  = p_patin->lambda_23;/* Lambda  U= 2/3          */
p_patout->lambda_1   = p_patin->lambda_1; /* Lambda  U=  1           */
p_patout->mu_0       = p_patin->mu_0;     /* Mu      U=  0           */
p_patout->mu_13      = p_patin->mu_13;    /* Mu      U= 1/3          */
p_patout->mu_23      = p_patin->mu_23;    /* Mu      U= 2/3          */
p_patout->mu_1       = p_patin->mu_1;     /* Mu      U=  1           */
p_patout->dlambda_0  = p_patin->dlambda_0;/* dLambda/dU   U=  0      */
p_patout->dlambda_13 = p_patin->dlambda_13;/* dLambda/dU   U= 1/3     */
p_patout->dlambda_23 = p_patin->dlambda_23;/* dLambda/dU   U= 2/3     */
p_patout->dlambda_1  = p_patin->dlambda_1;/* dLambda/dU   U=  1      */
p_patout->dmu_0      = p_patin->dmu_0;    /* dMu/dU       U=  0      */
p_patout->dmu_13     = p_patin->dmu_13;   /* dMu/dU       U= 1/3     */
p_patout->dmu_23     = p_patin->dmu_23;   /* dMu/dU       U= 2/3     */
p_patout->dmu_1      = p_patin->dmu_1;    /* dMu/dU       U=  1      */
p_patout->p1         = p_patin->p1;       /* Flag 1                  */
p_patout->p2         = p_patin->p2;       /* Flag 2                  */
p_patout->p3         = p_patin->p3;       /* Flag 3                  */

   poi_in = p_patin->cl_p;      /* Core Line Point                   */
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->cl_p = poi_tra;
 
   poi_in = p_patin->cl_v;      /* Core Line Vector                  */
    GEtfvec_to_local (&poi_in, p_c, &poi_tra);
   p_patout->cl_v = poi_tra;
 
p_patout->ofs_pat    = p_patin->ofs_pat; /* Offset for the patch    */

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur626 Exit ****** varkon_pat_contra ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
