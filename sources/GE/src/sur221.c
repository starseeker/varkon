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
/*  Function: varkon_pat_biceval                   File: sur221.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a given a bicubic patch.     */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-03-30   Originally written                                 */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1998-05-04   Speed optimizations, J.Kjellander                  */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_biceval    Bicubic patch evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = sur221 Illegal computation case=  for varkon_pat_biceval*/
/* SU2973 = Internal function xxxxxx failed in varkon_pat_biceval   */
/* SU2993 = Severe program error in varkon_pat_biceval (sur221).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus  varkon_pat_biceval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATC *p_patc,       /* Bicubic patch in surface          (ptr) */
   DBint   icase,        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat u,            /* Patch (local) U parameter value         */
   DBfloat v,            /* Patch (local) V parameter value         */
   EVALS  *p_xyz  )      /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/

   DBfloat f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,
           f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,
           f30,f31,f32,f33,f34,f35,u3,v3,u6,v6;

#ifdef  DEBUG
   char      errbuf[80]; /* String for error message fctn erpush    */
#endif
/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initiate output coordinates and derivatives for DEBUG on.       */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
   initial(icase,u,v,p_xyz); 

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
    {
      sprintf(errbuf,"%d%% varkon_pat_biceval (sur221)",(int)icase);
      return(varkon_erpush("SU2983",errbuf));
    }
#endif



/*! 1. Coordinates are always calculated                           !*/

   f0  = p_patc->a00x + u*(p_patc->a10x + u*(p_patc->a20x + u*p_patc->a30x));
   f1  = p_patc->a01x + u*(p_patc->a11x + u*(p_patc->a21x + u*p_patc->a31x));
   f2  = p_patc->a02x + u*(p_patc->a12x + u*(p_patc->a22x + u*p_patc->a32x));
   f3  = p_patc->a03x + u*(p_patc->a13x + u*(p_patc->a23x + u*p_patc->a33x));
   p_xyz->r_x = f0 + v*(f1 + v*(f2 + v*f3));

   f4  = p_patc->a00y + u*(p_patc->a10y + u*(p_patc->a20y + u*p_patc->a30y));
   f5  = p_patc->a01y + u*(p_patc->a11y + u*(p_patc->a21y + u*p_patc->a31y));
   f6  = p_patc->a02y + u*(p_patc->a12y + u*(p_patc->a22y + u*p_patc->a32y));
   f7  = p_patc->a03y + u*(p_patc->a13y + u*(p_patc->a23y + u*p_patc->a33y));
   p_xyz->r_y = f4 + v*(f5 + v*(f6 + v*f7));

   f8  = p_patc->a00z + u*(p_patc->a10z + u*(p_patc->a20z + u*p_patc->a30z));
   f9  = p_patc->a01z + u*(p_patc->a11z + u*(p_patc->a21z + u*p_patc->a31z));
   f10 = p_patc->a02z + u*(p_patc->a12z + u*(p_patc->a22z + u*p_patc->a32z));
   f11 = p_patc->a03z + u*(p_patc->a13z + u*(p_patc->a23z + u*p_patc->a33z));
   p_xyz->r_z = f8 + v*(f9 + v*(f10 + v*f11));

/*! 2. Derivative dr/du if icase > ?                               !*/

   u3  = 3.0*u;
   f12 = p_patc->a10x + u*(2.0*p_patc->a20x + u3*p_patc->a30x) ;
   f13 = p_patc->a11x + u*(2.0*p_patc->a21x + u3*p_patc->a31x) ;
   f14 = p_patc->a12x + u*(2.0*p_patc->a22x + u3*p_patc->a32x) ;
   f15 = p_patc->a13x + u*(2.0*p_patc->a23x + u3*p_patc->a33x) ;
   p_xyz->u_x = f12 + v*(f13 + v*(f14 + v*f15));

   f16 = p_patc->a10y + u*(2.0*p_patc->a20y + u3*p_patc->a30y) ;
   f17 = p_patc->a11y + u*(2.0*p_patc->a21y + u3*p_patc->a31y) ;
   f18 = p_patc->a12y + u*(2.0*p_patc->a22y + u3*p_patc->a32y) ;
   f19 = p_patc->a13y + u*(2.0*p_patc->a23y + u3*p_patc->a33y) ;
   p_xyz->u_y = f16 + v*(f17 + v*(f18 + v*f19));

   f20 = p_patc->a10z + u*(2.0*p_patc->a20z + u3*p_patc->a30z) ;
   f21 = p_patc->a11z + u*(2.0*p_patc->a21z + u3*p_patc->a31z) ;
   f22 = p_patc->a12z + u*(2.0*p_patc->a22z + u3*p_patc->a32z) ;
   f23 = p_patc->a13z + u*(2.0*p_patc->a23z + u3*p_patc->a33z) ;
   p_xyz->u_z = f20 + v*(f21 + v*(f22 + v*f23));

/*! 3. Derivative dr/dv if icase > ?                               !*/

   v3 = 3.0*v;
   p_xyz->v_x = f1 + v*(2.0*f2  + v3*f3);
   p_xyz->v_y = f5 + v*(2.0*f6  + v3*f7);
   p_xyz->v_z = f9 + v*(2.0*f10 + v3*f11);

/*! 4. Derivative d2r/du2                                          !*/

   u6  = 6.0*u;
   f24 = 2.0*p_patc->a20x + u6*p_patc->a30x;
   f25 = 2.0*p_patc->a21x + u6*p_patc->a31x;
   f26 = 2.0*p_patc->a22x + u6*p_patc->a32x;
   f27 = 2.0*p_patc->a23x + u6*p_patc->a33x;
   p_xyz->u2_x = f24 + v*(f25 + v*(f26 + v*f27));

   f28 = 2.0*p_patc->a20y + u6*p_patc->a30y;
   f29 = 2.0*p_patc->a21y + u6*p_patc->a31y;
   f30 = 2.0*p_patc->a22y + u6*p_patc->a32y;
   f31 = 2.0*p_patc->a23y + u6*p_patc->a33y;
   p_xyz->u2_y = f28 + v*(f29 + v*(f30 + v*f31));

   f32 = 2.0*p_patc->a20z + u6*p_patc->a30z;
   f33 = 2.0*p_patc->a21z + u6*p_patc->a31z;
   f34 = 2.0*p_patc->a22z + u6*p_patc->a32z;
   f35 = 2.0*p_patc->a23z + u6*p_patc->a33z;
   p_xyz->u2_z = f32 + v*(f33 + v*(f34 + v*f35));

/*! 5. Derivative d2r/dv2                                          !*/

   v6 = 6.0*v;
   p_xyz->v2_x = 2.0*f2  + v6*f3;
   p_xyz->v2_y = 2.0*f6  + v6*f7;
   p_xyz->v2_z = 2.0*f10 + v6*f11;

/*! 6. Derivative d2r/dudv                                         !*/

   p_xyz->uv_x = f13 + v*(2.0*f14 + v3*f15);
   p_xyz->uv_y = f17 + v*(2.0*f18 + v3*f19);
   p_xyz->uv_z = f21 + v*(2.0*f22 + v3*f23);


   return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!New-Page--------------------------------------------------------!*/

#ifdef  DEBUG
/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF.                            */

   static short initial(icase,u_pat,v_pat,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   icase;        /* Calculation case ..                     */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Coordinates and derivatives       (ptr) */

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

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur221 *** initial: icase= %d p_xyz= %d\n", (int)icase, (int)p_xyz);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

/*!                                                                 */
/* 2. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;
    p_xyz->r_y= F_UNDEF;
    p_xyz->r_z= F_UNDEF;

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;
    p_xyz->u_y= F_UNDEF;
    p_xyz->u_z= F_UNDEF;

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
#endif  /*  DEBUG  */
