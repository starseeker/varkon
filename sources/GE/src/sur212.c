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
/*  Function: varkon_sur_princurv                  File: sur212.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the principal curvature directions      */
/*  and curvature values for given derivatives in a surface         */
/*  point.                                                          */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-06   Originally written                                 */
/*  1996-01-18   Missing end of comment, unused variables, fflush   */
/*  1997-03-06   Square root argument with tolerance                */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_princurv   Principal curvature              */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_comptol         * Retrieve computer tolerance   */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_princurv (sur212).   */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_princurv (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS   *p_xyz )      /* Surface coordinates & derivatives (ptr) */
/*                                                                  */
/* Out:                                                             */
/*       Curvature data will be added to p_xyz                      */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  kmin;        /* Minimum principal curvature             */
   DBfloat  kmax;        /* Maximum principal curvature             */
   DBVector r3minvec;    /* Normalised vector in R*3 for kmin       */
   DBVector r3maxvec;    /* Normalised vector in R*3 for kmax       */
   DBVector uvminvec;    /* Normalised vector in U,V for kmin       */
   DBVector uvmaxvec;    /* Normalised vector in U,V for kmax       */
   DBfloat  dxdu;        /* dr/du                                   */
   DBfloat  dydu;        /*                                         */
   DBfloat  dzdu;        /*                                         */
   DBfloat  dxdv;        /* dr/dv                                   */
   DBfloat  dydv;        /*                                         */
   DBfloat  dzdv;        /*                                         */
   DBfloat  d2xdu2;      /* d2r/du2                                 */
   DBfloat  d2ydu2;      /*                                         */
   DBfloat  d2zdu2;      /*                                         */
   DBfloat  d2xdv2;      /* d2r/dv2                                 */
   DBfloat  d2ydv2;      /*                                         */
   DBfloat  d2zdv2;      /*                                         */
   DBfloat  d2xdudv;     /* d2r/dudv                                */
   DBfloat  d2ydudv;     /*                                         */
   DBfloat  d2zdudv;     /*                                         */
   DBfloat  N[3];        /* Surface normal                          */
   DBfloat  g11,g12;     /* First  fundamental matrix               */
   DBfloat  g21,g22;     /*                                         */
   DBfloat  d11,d12;     /* Second fundamental matrix               */
   DBfloat  d21,d22;     /*                                         */
   DBfloat  a,b,c;       /* Coefficients for the computation        */
                         /* of the principal curvatures             */
                         /* a=Determinant(G)                        */
                         /* c=Determinant(D)                        */
   DBfloat  k1,k2;       /* Solutions to quadratic equation         */
                         /* for the principal curvatures            */
   DBfloat  c1,c2,c3;    /* Coefficients for the computation        */
                         /* of the principal curvatures             */
   DBfloat  a1,a2,a3;    /* Coefficients for the computation        */
                         /* of the principal curvatures             */
   DBfloat  dudt1,dvdt1; /* Principal curvature directions          */
   DBfloat  dudt2,dvdt2; /* (tangents) in the u,v plane.            */
   DBfloat  kappa1;      /* Principal curvature for the given       */
                         /* direction dudt1,dvdt1                   */
   DBfloat  kappa2;      /* Principal curvature for the given       */
                         /* direction dudt2,dvdt2                   */
   DBfloat  sphere_tol;  /* The tolerance for a sphere (plane)      */
                         /* The principal directions cannot be      */
                         /* calculated if the maximum and minimum   */
                         /* principal curvature are equal.          */
                         /* For the computation of the R*3 vectors  */
   DBfloat  tx_dt;       /* Output vector                           */
   DBfloat  ty_dt;       /*                                         */
   DBfloat  tz_dt;       /*                                         */
   DBfloat  txmin;       /* Normalised vector in R*3 for kmin       */
   DBfloat  tymin;       /*                                         */
   DBfloat  tzmin;       /*                                         */
   DBfloat  txmax;       /* Normalised vector in R*3 for kmax       */
   DBfloat  tymax;       /*                                         */
   DBfloat  tzmax;       /*                                         */
   DBfloat  sqsinb1;     /* (sinB1)**2. Refer to Formulae above     */
   DBfloat  sqsinb2;     /* (sinB2)**2. Refer to Formulae above     */
   DBfloat  alfa;        /* Angle (temporarely used)                */
                         /* For the calculation of the principal    */
                         /* directions:                             */
                         /* for the principal directions.           */
                         /* (Vectors in the u,v plane)              */
   DBfloat  sina1,cosa1; /* (sinA1),(cosA1) Refer to Formulae above */
                         /* Solution to equation (3)                */
   DBfloat  sina2,cosa2; /* (sinA2),(cosA2) Refer to Formulae above */
                         /* Solution to equation (3)                */
   DBfloat  sina3,cosa3; /* (sinA3),(cosA3) Refer to Formulae above */
                         /* Solution to equation (3)                */
   DBfloat  sina4,cosa4; /* (sinA4),(cosA4) Refer to Formulae above */
                         /* Solution to equation (3)                */
   DBfloat  kappamin;    /* Min. prin curvature for given direction */
   DBfloat  kappamax;    /* Max. prin curvature for given direction */
   DBfloat  sqrt_arg;    /* Square root argument                    */
   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   DBfloat  dum1;        /* Dummy variable                          */
/*                                                                  */
/*----------------------------------------------------------------- */

/*!                                                                !*/
/*!                                                                !*/

/*!----------------------- Flow chart ------------------------------*/
/*                     _____________________                        */
/*                    !                     !                       */
/*                    ! varkon_sur_princurv !                       */
/*                    !      (sur212)       !                       */
/*                    !_____________________!                       */
/*       _________________________!________________________         */
/*  ____!___  ____!___  ___!___  ____!__  __!__  __!___  ___!___    */
/* !        !!        !!       !!       !!     !!      !!       !   */
/* !    1   !!    2   !!   3   !!   4   !!  5  !!  6   !!   7   !   */
/* !        !!        !!       !!       !!     !!      !!       !   */
/* !Initiate!!Fundamen!!Princip!!Princip!!Match!! R*3  !!Analyse!   */
/* !variabl.!!matrices!!curvat.!!direct.!!     !!vector!!       !   */
/* !________!!________!!_______!!_______!!_____!!______!!_______!   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!---------------------- Theory -----------------------------------*/
/*                                                                  */
/* Refer to Computational Geometry for Design and Manufacture,      */
/* Faux & Pratt, page 107, 111 and 112.                             */
/*                                                                  */
/* Fundamental matrices                                             */
/* - - - - - - - - - - -                                            */
/*                                                                  */
/*                                   ! G11  G12 !                   */
/* The first  fundamental matrix G = !          !                   */
/*                                   ! G21  G22 !                   */
/*                                                                  */
/*                                   ! D11  D12 !                   */
/* The second fundamental matrix D = !          !                   */
/*                                   ! D21  D22 !                   */
/*                                                                  */
/*                                                                  */
/* Principal curvature                                              */
/* - - - - - - - - - -                                              */
/*                                                                  */
/* The principal curvatures Kn= K1,K2 are determined by the         */
/* quadratic equation:                                              */
/*                                                                  */
/* A*Kn**2 + B*Kn + C = 0                       (1)                 */
/*                                                                  */
/* where                                                            */
/*                                                                  */
/* A = Determinant (G) = G11*G22 - G12*G21                          */
/* B= -(G11*D22 + D11*G22 - 2*G12*D12)                              */
/* C = Determinant (D) = D11*D22 - D12*D21                          */
/*                                                                  */
/* Let KMIN = minimum of K1 and K2                                  */
/* Let KMAX = maximum of K1 and K2                                  */
/*                                                                  */
/* Principal curvature direction                                    */
/* - - - - - - - - - - - - - - -                                    */
/*                                                                  */
/* The principal curvature directions are                           */
/* determined by the quadratic equation:                            */
/*                                                                  */
/*    .         . .      .                                          */
/* C1*U**2 + C2*U*V + C3*V**2 = 0               (2)                 */
/*                                                                  */
/* where                                                            */
/* C1 = D11*G12 - D12*G11                                           */
/* C2 = D11*G22 - D22*G11                                           */
/* C3 = D12*G22 - D22*G12                                           */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                 */
/*             . .                                                  */
/* The vector (U,V) <=> (cosA,sinA), where A is the angle           */
/* in the u/v-plane.                                                */
/*                                                                  */
/*    .         . .      .                                          */
/* C1*U**2 + C2*U*V + C3*V**2 = 0    <=>                            */
/*                                                                  */
/* C1*(cosA)**2 + C2*(cosA)*(sinA) + C3*(sinA)**2 = 0               */
/*                                                                  */
/* (cosA)**2 = 1 - (sinA)**2                                        */
/*                                                                  */
/* A1*(sinA)**4 + A2*(sinA)**2 + A3 = 0         (3)                 */
/*                                                                  */
/* where                                                            */
/* A1 =  (C1-C3)*(C1-C3) + C2*C2                                    */
/* A2 = -( 2*C1*(C1-C3) + C2*C2 )                                   */
/* A3 =   C1*C1                                                     */
/*                                                                  */
/* Determine first the solution (sinB1)**2 and (sinB2)**2           */
/* to equation (3) and then                                         */
/* (sinA1) =  Squareroot of (sinB1)**2                              */
/*   A1    =  Arcsin  of (sinA1)                                    */
/* (cosA1) =  Cosine of A1                                          */
/* (sinA2) = -Squareroot of (sinB1)**2                              */
/*   A2    =  Arcsin  of (sinA2)                                    */
/* (cosA2) =  Cosine of A2                                          */
/*                                                                  */
/* (sinA3) =  Squareroot of (sinB2)**2                              */
/*   A3    =  Arcsin  of (sinA3)                                    */
/* (cosA3) =  Cosine of A3                                          */
/* (sinA4) = -Squareroot of (sinB2)**2                              */
/*   A4    =  Arcsin  of (sinA4)                                    */
/* (cosA4) =  Cosine of A4                                          */
/*                                                                  */
/* It is only two of the four solutions, which will be the right    */
/* solutions. One solution will be (cosA1,sinA1) or (cosA2,sinA2)   */
/* and the other will be (cosA3,sinA3) or (cosA4,sinA4).            */
/* Equation (2) is used to find the valid solutions:                */
/*                                                                  */
/* The solution is  (DUDT1,DVDT1)= (U,V) = (cosA1,sinA1) if         */
/* !C1*(cosA1)**2 + C2*(cosA1)*(sinA1) + C3*(sinA1)**2!  ( = 0 )    */
/*                  is less or equal to                             */
/* !C1*(cosA2)**2 + C2*(cosA2)*(sinA2) + C3*(sinA2)**2!             */
/*                                                                  */
/* The solution is  (DUDT2,DVDT2)= (U,V) = (cosA3,sinA3) if         */
/* !C1*(cosA3)**2 + C2*(cosA3)*(sinA3) + C3*(sinA3)**2!  ( = 0 )    */
/*                  is less or equal to                             */
/* !C1*(cosA4)**2 + C2*(cosA4)*(sinA4) + C3*(sinA4)**2!             */
/*                                                                  */
/*                                                                  */
/* Match of curvature values and directions                         */
/* - - - - - - - - - - - - - - - - - - - - -                        */
/*                                                                  */
/* The curvatures and directions are "matched" in equation.         */
/*                                                                  */
/*               . .     . .                                        */
/*              (U,V)*D*(U,V)                                       */
/*           Kn = -------------                    (4)              */
/*                 . .     . .                                      */
/*                (U,V)*G*(U,V)                                     */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                 */
/* The principal curvature values are recalculated.                 */
/* Let KAPPA1= Kn for DUDT1,DVDT1 and                               */
/*     KAPPA2= Kn for DUDT2,DVDT2                                   */
/*                                                                  */
/* Let UVMINVEC(1-2) = DUDT1,DVDT1                                  */
/* and UVMAXVEC(1-2) = DUDT2,DVDT2  for !KAPPA1! < !KAPPA2!         */
/* Let UVMINVEC(1-2) = DUDT2,DVDT2                                  */
/* and UVMAXVEC(1-2) = DUDT1,DVDT1  for !KAPPA1! > !KAPPA2!         */
/*                                                                  */
/*                                                                  */
/* Curvature directions in R*3 space                                */
/* - - - - - - - - - - - - - - - - -                                */
/*                                                                  */
/* The R*3 tangent for a tangent (curve) in the u,v plane is        */
/* determined by:                                                   */
/*                                                                  */
/* .    dx   du   dx   dv   dy   du   dy   dv   dz   du   dz   dv   */
/* r =  -- * -- + -- * --   -- * -- + -- * --   -- * -- + -- * --   */
/*      du   dt   dv   dt , du   dt   dv   dt , du   dt   dv   dt   */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212 Enter *** varkon_sur_princurv: \n");
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Criterion for a sphere                                           */
   sphere_tol= 0.000001;
/* Derivatives to local variables                                   */
/*                                                                 !*/

      dxdu    = p_xyz->u_x;
      dydu    = p_xyz->u_y;
      dzdu    = p_xyz->u_z;
      dxdv    = p_xyz->v_x;
      dydv    = p_xyz->v_y;
      dzdv    = p_xyz->v_z;
      d2xdu2  = p_xyz->u2_x;
      d2ydu2  = p_xyz->u2_y;
      d2zdu2  = p_xyz->u2_z;
      d2xdudv = p_xyz->uv_x;
      d2ydudv = p_xyz->uv_y;
      d2zdudv = p_xyz->uv_z;
      d2xdv2  = p_xyz->v2_x;
      d2ydv2  = p_xyz->v2_y;
      d2zdv2  = p_xyz->v2_z;

      N[0]    = p_xyz->n_x;
      N[1]    = p_xyz->n_y;
      N[2]    = p_xyz->n_z;

      if ( SQRT(N[0]*N[0]+N[1]*N[1]+N[2]*N[2]) < 0.0000001 )
       return(-1);

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/* Initialize output and internal variables to zero                 */
      txmin  = 0.0;
      tymin  = 0.0;
      tzmin  = 0.0;
      txmax  = 0.0;
      tymax  = 0.0;
      tzmax  = 0.0;
      uvminvec.x_gm = 0.0;
      uvminvec.y_gm = 0.0;
      uvmaxvec.x_gm = 0.0;
      uvmaxvec.y_gm = 0.0;
      r3minvec.x_gm = 0.0;
      r3minvec.y_gm = 0.0;
      r3minvec.z_gm = 0.0;
      r3maxvec.x_gm = 0.0;
      r3maxvec.y_gm = 0.0;
      r3maxvec.z_gm = 0.0;
      kmin   = 0.0;
      kmax   = 0.0;
      kappamin   = 0.0;
      kappamax   = 0.0;

      dudt1      = 0.0;
      dvdt1      = 0.0;
      dudt2      = 0.0;
      dvdt2      = 0.0;


/*!                                                                 */
/* 2. Fundamental matrices D and G                                  */
/* _______________________________                                  */
/*                                                                  */
/* Second fundamental matrix D= d11, d12, d21, d22                  */
/* and c= Determinant(D)                                            */
/*                                                                 !*/

      d11 = N[0]*d2xdu2 +N[1]*d2ydu2 +N[2]*d2zdu2;
      d12 = N[0]*d2xdudv+N[1]*d2ydudv+N[2]*d2zdudv;
      d21 = d12;
      d22 = N[0]*d2xdv2 +N[1]*d2ydv2 +N[2]*d2zdv2;

      c=d11*d22-d12*d21;

/*!                                                                 */
/* First fundamental matrix G= g11, g12, g21, g22                   */
/* and a= Determinant(G)                                            */
/*                                                                 !*/

      g11 = dxdu*dxdu+dydu*dydu+dzdu*dzdu;
      g12 = dxdu*dxdv+dydu*dydv+dzdu*dzdv;
      g21 = g12;
      g22 = dxdv*dxdv+dydv*dydv+dzdv*dzdv;

      a=g11*g22-g12*g21;


/*!                                                                 */
/* Let coefficient b= -(g11*d22+d11*g22-2*g12*d12)                  */
/*                                                                 !*/

      b= -(g11*d22+d11*g22-2*g12*d12);

/*!                                                                 */
/* Check of A and B                                                 */
/* If A=0 and B=0       ......                                      */
/* If A=0               ......                                      */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212 a= %25.10f  b= %25.10f \n",a,b);
  fflush(dbgfil(SURPAC));
  }
#endif

      if ( a <  1e-12  &&   fabs(b) <  1e-12  )
      return(-5);

      if ( a <  1e-12  )
      return(-4);

/*!                                                                 */
/* 3. Principal curvature values                                    */
/* _______________________________                                  */
/*                                                                  */
/* Solution K= K1,K2 to quadratic equation:                         */
/* A*K**2 + B*K + C = 0 where K= Principal curvature                */
/* (Let ...          if the square root is negative )               */
/*                                                                 !*/

   sqrt_arg = (b/a)*(b/a)/4.0-c/a;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212 Square root argument sqrt_arg %25.10f \n",
      sqrt_arg );
  fflush(dbgfil(SURPAC));
  }
#endif


      if ( ABS(sqrt_arg) < comptol ) sqrt_arg = 0.0;

      if ( sqrt_arg <  0.0 ) return(-2);

      k1 = -b/(2*a)+SQRT(sqrt_arg);
      k2 = -b/(2*a)-SQRT(sqrt_arg);
/*!                                                                 */
/* Let kmin = minimum of k1 and k2                                  */
/* Let kmax = maximum of k1 and k2                                  */
/*                                                                 !*/

      if (fabs(k1) <  fabs(k2) )
         {
         kmin = k1;
         kmax = k2;
         }
      else
         {
         kmin = k2;
         kmax = k1;
         }

/*!                                                                 */
/* Check if principal curvatures are equal which means              */
/* that the curvature values are OK but the directions              */
/* cannot be computed. The surface is (locally in the point)        */
/* a sphere or a plane:                                             */
/* Let ..         if !kmin - kmax! < sphere_tol and ...             */
/*                                                                 !*/

      if ( fabs(kmin-kmax)  <   sphere_tol ) 
         {
         p_xyz->kmin= kmin;
         p_xyz->kmax= kmax;
         return(10);
         }

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 4. Principal curvature directions                                */
/* _________________________________                                */
/*                                                                  */
/* Coefficients for the equation:                                   */
/*    .         . .      .                                          */
/* C1*U**2 + C2*U*V + C3*V**2 = 0                                   */
/*                                                                 !*/

      c1 = d11*g12 - d12*g11;
      c2 = d11*g22 - d22*g11;
      c3 = d12*g22 - d22*g12;

/*!                                                                 */
/* Coefficients for the equation:                                   */
/*                                                                  */
/* a1*(sinA)**4 + a2*(sinA)**2 + a3 = 0                             */
/*                                                                 !*/

      a1 = (c1-c3)*(c1-c3)+c2*c2;
      a2 = -(2*c1*(c1-c3)+ c2*c2);
      a3 = c1*c1;

      if ((a2/a1/2.0)*(a2/a1/2.0)-a3/a1 <  0.0)
      return(-7);

/*!                                                                 */
/* Computation of (sinA1)**2 and (sinA2)**2                         */
/*                                                                 !*/

      sqsinb1 = -a2/a1/2.+SQRT((a2/a1/2.)*(a2/a1/2.)-a3/a1);
      sqsinb2 = -a2/a1/2.-SQRT((a2/a1/2.)*(a2/a1/2.)-a3/a1);

      if ( sqsinb1 <  0.0 )
      return(-8);

      if ( sqsinb2 <  0.0 )
      return(-9);

/*!                                                                 */
/* The four solutions (cosA1,sinA1) , (cosA2,sinA2)                 */
/* and (cosA3,sinA3) , (cosA4,sinA4).                               */
/*                                                                 !*/

      sina1 = SQRT(sqsinb1);
      if (fabs(sina1) >= 1.0) sina1 = 1.0;
      alfa  = ASIN(sina1);
      cosa1 = COS(alfa);

      sina2 = -SQRT(sqsinb1);
      if (fabs(sina2) >= 1.0) sina2 = -1.0;
      alfa  = ASIN(sina2);
      cosa2 = COS(alfa);

      sina3 = SQRT(sqsinb2);
      if (fabs(sina3) >= 1.0) sina3 = 1.0;
      alfa  = ASIN(sina3);
      cosa3 = COS(alfa);

      sina4 = -SQRT(sqsinb2);
      if (fabs(sina4) >= 1.0) sina4 = -1.0;
      alfa  = ASIN(sina4);
      cosa4 = COS(alfa);

/*!                                                                 */
/* Determine if the right solution is                               */
/* (dudt1,dvdt1)= (cosA1,sinA1) or (cosA2,sinA2)                    */
/*                                                                 !*/

      if(fabs(c1*cosa1*cosa1+c2*cosa1*sina1+c3*sina1*sina1) <= 
         fabs(c1*cosa2*cosa2+c2*cosa2*sina2+c3*sina2*sina2))
          {
          dudt1 = cosa1;
          dvdt1 = sina1;
          }
      else
          {
          dudt1 = cosa2;
          dvdt1 = sina2;
          }


/*!                                                                 */
/* Determine if the right solution is                               */
/* (dudt2,dvdt2)= (cosA3,sinA3) or (cosA4,sinA4)                    */
/*                                                                 !*/

      if(fabs(c1*cosa3*cosa3+c2*cosa3*sina3+c3*sina3*sina3) <= 
         fabs(c1*cosa4*cosa4+c2*cosa4*sina4+c3*sina4*sina4)) 
          {
          dudt2 = cosa3;
          dvdt2 = sina3;
          }
      else
          {
          dudt2 = cosa4;
          dvdt2 = sina4;
          }

/*!                                                                 */
/*                  .  .                    .  .                    */
/* 5. Match vectors U1,V1 (dudt1,dvdt1) and U2,V2 (dudt2,dvdt2)     */
/*    with minimum and maximum principal curvatures kmin and kmax.  */
/*                                                                 !*/

/*
*
*       Put in the vectors (dudt1,dvdt1) resp. (dudt2,dvdt2) in
*
*                                . .      . .
*                               (U,V)*D*(U,V)
*       the equation :     Kn = -------------
*                                . .     . .
*                               (U,V)*G*(U,V)
*
*
*       and calculate kappa1 for (dudt1,dvdt1) and kappa2 for
*       (dudt2,dvdt2). Determine if kappa1 will be kmin or
*       kmax and match vectors to kmin and kmax.
*
*/

/*!                                                                 */
/*  Recalculation of principal curvatures kappa1 and kappa2         */
/*                                                                 !*/

      dum1 = dudt1*dudt1*g11+2.*dudt1*dvdt1*g12+dvdt1*dvdt1*g22;
      if  ( fabs(dum1) <  1e-12  )
      return(-96);

      kappa1 = (dudt1*dudt1*d11+2.*dudt1*dvdt1*d12+
                dvdt1*dvdt1*d22)/dum1;


      dum1 = dudt2*dudt2*g11+2.*dudt2*dvdt2*g12+dvdt2*dvdt2*g22;
      if  ( fabs(dum1) <  1e-12  )
      return(-96);

      kappa2 = (dudt2*dudt2*d11+2.*dudt2*dvdt2*d12+
                dvdt2*dvdt2*d22)/dum1;

/*!                                                                 */
/*  Compare !kappa1! and !kappa2! in order to determine if          */
/*  uvminvec= dudt1,dvdt1 and uvmaxvec= dudt2,dvdt2                 */
/*                        or                                        */
/*  uvminvec= dudt2,dvdt2 and uvmaxvec= dudt1,dvdt1                 */
/*                                                                 !*/

      if(fabs(kappa1) <= fabs(kappa2))
          {
          kappamin = kappa1;
          uvminvec.x_gm= dudt1;
          uvminvec.y_gm= dvdt1;
          kappamax = kappa2;
          uvmaxvec.x_gm= dudt2;
          uvmaxvec.y_gm= dvdt2;
          }
      else
          {
          kappamin = kappa2;
          uvminvec.x_gm= dudt2;
          uvminvec.y_gm= dvdt2;
          kappamax = kappa1;
          uvmaxvec.x_gm= dudt1;
          uvmaxvec.y_gm= dvdt1;
          }


/*!                                                                 */
/*  6. For the given derivatives in the u,v plane compute the       */
/*     corresponding principal curvature vectors in R*3 space       */
/*                                                                 !*/

/*!                                                                 */
/*  The minimum curvature vector r3minvec which                     */
/*  corresponds to uvminvec.                                        */
/*                                                                 !*/

      tx_dt = dxdu*uvminvec.x_gm + dxdv*uvminvec.y_gm;
      ty_dt = dydu*uvminvec.x_gm + dydv*uvminvec.y_gm;
      tz_dt = dzdu*uvminvec.x_gm + dzdv*uvminvec.y_gm;
      dum1    = SQRT(tx_dt*tx_dt+ty_dt*ty_dt+tz_dt*tz_dt);
      if  ( fabs(dum1) <  1e-12  )
      return(-97);

      txmin = tx_dt/dum1;
      tymin = ty_dt/dum1;
      tzmin = tz_dt/dum1;

      r3minvec.x_gm = txmin;
      r3minvec.y_gm = tymin;
      r3minvec.z_gm = tzmin;

/*!                                                                 */
/*  The maximum curvature vector r3maxvec which                     */
/*  corresponds to uvmaxvec                                         */
/*                                                                 !*/


      tx_dt = dxdu*uvmaxvec.x_gm + dxdv*uvmaxvec.y_gm;
      ty_dt = dydu*uvmaxvec.x_gm + dydv*uvmaxvec.y_gm;
      tz_dt = dzdu*uvmaxvec.x_gm + dzdv*uvmaxvec.y_gm;
      dum1    = SQRT(tx_dt*tx_dt+ty_dt*ty_dt+tz_dt*tz_dt);
      if  ( fabs(dum1) <  1e-12  )
      return(-98);

      txmax = tx_dt/dum1;
      tymax = ty_dt/dum1;
      tzmax = tz_dt/dum1;

      r3maxvec.x_gm = txmax;
      r3maxvec.y_gm = tymax;
      r3maxvec.z_gm = tzmax;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212  kappa1= %f kappa2= %f\n", kappa1,kappa2);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212      k1= %f     k2= %f\n", k1,    k2);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212    kmin= %f   kmax= %f\n", kmin,  kmax);
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/*  7. Calculated data to output variable p_xyz                     */
/*                                                                 !*/

      p_xyz->kmin  = kmin;
      p_xyz->kmax  = kmax;
      p_xyz->kmax_x= r3maxvec.x_gm;
      p_xyz->kmax_y= r3maxvec.y_gm;
      p_xyz->kmax_z= r3maxvec.z_gm;
      p_xyz->kmin_x= r3minvec.x_gm;
      p_xyz->kmin_y= r3minvec.y_gm;
      p_xyz->kmin_z= r3minvec.z_gm;
      p_xyz->kmax_u= uvmaxvec.x_gm;
      p_xyz->kmax_v= uvmaxvec.y_gm;
      p_xyz->kmin_u= uvminvec.x_gm;
      p_xyz->kmin_v= uvminvec.y_gm;

/*!                                                                 */
/*  8. Analyse the surface point with respect to curvature          */
/*                                                                 !*/



/*!                                                                 */
/*  Is the surface (locally) developable ?                          */
/*  Let  ...       if kmin = 0 and ...                              */
/*                                                                 !*/

      if (kmin == 0)
         {
         return( 3);
         }




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur212 Exit*varkon_sur_princurv kmin= %f kmax= %f\n",
                      p_xyz->kmin,p_xyz->kmax);
 fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

