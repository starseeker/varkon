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
/*  Function: varkon_cur_num1                      File: sur901.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of points on a curve.                                           */
/*                                                                  */
/*  Input is a pointer to a structure which defines the             */
/*  calculation criterion for the points (the objective             */
/*  function), the method for the numerical calculation,            */
/*  the number of restarts, the tolerances, etc.                    */
/*                                                                  */
/*  Note that the calling function handles the segments             */
/*  in the curve. Input parameters should normally be               */
/*  in the same segment. Also curve start and end                   */
/*  parameters should normally be modified in order                 */
/*  to respect the extension of the curve distance                  */
/*  ctol (the identical points criterion). Refer                    */
/*  to function varkon_cur_intplane (sur720).                       */
/*                                                                  */
/*  TODO:                                                           */
/*  1. !! Tolerances TOL4 shall be replaced  !!!!!!!                */
/*  2. !! Do not use same start value 20 times        !!            */
/*  3. !! When tolerances are working, may extend possibly be       */
/*        removed in SUR_EXDEF and SUR_CONIC ???                    */
/*                                                                  */
/*  The methods available are Newton-Rhapson, ....                  */
/*                                                                  */
/*  Author                                                          */
/*                                                                  */
/*  Gunnar Liden                                                    */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-01-23   Originally written                                 */
/*  1994-10-16   Debug och toleranser                               */
/*  1996-01-13   Not used variables                                 */
/*  1996-05-14   End of comments missing                            */
/*  1996-06-13   Debug                                              */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_num1       Curve   one param. calculation   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
static short intplan();       /* Function f_cur and dfdt for SURPLAN*/
static short addsol();        /* Add a non-existent solution.       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   ctype;             /* Case of computation            */
static DBfloat c_zero;            /* The zero (0.0) end criterion   */
static DBint   method;            /* Method which shall be used     */
static DBfloat t_s,t_e;           /* Param. interv. for calcul.     */
static DBfloat tglobal;           /* Current  parameter value       */
static DBfloat f_cur;             /* Current (objective) fctn value */
static DBfloat f_pre;             /* Previous function value        */
static DBfloat dfdt;              /* Function derivative w.r.t t    */
static DBint   numsol;            /* Number of solutions (=*p_npts) */
static DBfloat pi_x,pi_y,pi_z;    /* Intersecting plane             */
static DBfloat pi_d;              /*       - " -                    */
static EVALC   xyz;               /* Coordinates and derivatives    */
                                  /* for a point on a curve         */
static DBfloat ctol;              /* Coordinate tolerance           */
static DBint   niter;             /* Number of iterations           */
static DBfloat u_a[50];           /* All U parameter values         */
static DBfloat f_a[50];           /* All objective function values */
static DBfloat d_a[50];           /* All objective derivat. values */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol       * Retrieve computer tolerance               */
/* varkon_cur_eval      * Curve   evaluation routine                */
/* varkon_ini_evalc     * Initialize EVALC                          */
/* varkon_erpush        * Error message to terminal                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_cur_num1        */
/* SU2983 = sur901 Illegal computation case=   for varkon_cur_num1  */
/* SU2973 = Internal function xxxxxx failed in varkon_cur_num1      */
/* SU2993 = Program error in varkon_cur_num1 (sur901). Report !     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_cur_num1(

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Coefficients for curve segment    (ptr) */
   DBfloat  t_in[2],     /* Parameter curve segment limits          */
                         /* (0):   Start U    (1): End   U          */
   IRUNON  *p_comp,      /* Pointer to computation data             */
/* Out:                                                             */
   DBint   *p_npts,      /* Number of output points (a pointer)     */
   EVALC   xyz_a[SMAX] ) /* Array with all solutions                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   nstart;       /* The number of restarts                  */
   DBint   maxiter;      /* Maximum number of iterations            */
   DBfloat deltat;       /* The parameter t step for the restarts   */
   DBint   i_r;          /* Loop index for the restarts             */
   DBint   j_n;          /* Loop index for the numerical solution   */
   DBfloat h;            /* Step determined by Newton Rhapson       */

   short   icase;        /* Calculation case for varkon_cur_eval    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat comptol;      /* Computer tolerance (accuracy)           */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */
#ifdef  DEBUG
   DBint   ix1;          /* Temporarely used loop index             */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 Enter varkon_cur_num1 ** Start U %f End U %f\n", 
   t_in[0],t_in[1]);
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */

/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
    status=initial(t_in,p_comp,p_npts,xyz_a);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_cur_num1 (sur901)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Case of computation, method and number of restarts               */
   ctype   = p_comp->ipl_un.ctype;
   method  = p_comp->ipl_un.method;
   nstart  = p_comp->ipl_un.nstart;
   maxiter = p_comp->ipl_un.maxiter;
/*                                                                 !*/

/*!                                                                 */
/* Retrieve computation data for the input computation case         */
/* (for instance the intersection plane for ctype= CURPLAN)         */
/* and definition of the end criterion for c_zero                   */
/*                                                                 !*/

   if      ( ctype  == CURPLAN )
       {
       pi_x = p_comp->ipl_un.in_x;
       pi_y = p_comp->ipl_un.in_y;
       pi_z = p_comp->ipl_un.in_z;
       pi_d = p_comp->ipl_un.in_d;
       ctol = p_comp->ipl_un.ctol;
/*   End criterion for the numerical solution ( !!! geo995 !!! )    */
       c_zero  = ctol/10.0;
       icase = 3;
       }
   else
       {
       sprintf(errbuf,"(ctype )%% varkon_cur_num1 (sur901)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/* Parameter interval t_s and t_e for the numerical solution        */
/*                                                                 !*/
  t_s = t_in[0];
  t_e = t_in[1];
  if ( t_s > t_e )
     {
     t_s = t_in[1];
     t_e = t_in[0];
     }     

/*!                                                                 */
/* The parameter step for the restarts                              */
   deltat  = (t_e-t_s-comptol)/((DBfloat)nstart-1.0);
/* The number of output points                                      */
   numsol  = 0;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 t_s %f t_e %f deltat %f nstart= %d ==> End start %f\n",
   t_s,t_e,deltat,nstart,t_s+((DBfloat)nstart-1.0)*deltat);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 t_e - (nstart-1)*deltat %25.15f\n",
   t_e - t_s-((DBfloat)nstart-1.0)*deltat);
  }
#endif

/*!                                                                 */
/* 2. Numerical solution for a one parameter function               */
/* __________________________________________________               */

/* Start loop: Restarts i_r=1,2, .. nstart                          */
/*                                                                 !*/

   for (i_r = 1; i_r <= nstart ; i_r = i_r+1)
     {
/*!                                                                 */
/*   New parameter start value                                      */
/*   tglobal = t_s + deltat*(i_r-1)                                 */
/*   Start loop: Numerical i_n=1,2, ..                              */
/*                                                                 !*/

/*   Initialize number of iterations                                */
     niter = 0;

     tglobal = t_s + deltat*((DBfloat)i_r-1.0);

     for(j_n = 1; j_n <= maxiter ; j_n = j_n+1)
       {
/*!                                                                 */
/*     Calculate coordinates and derivatives.                       */
/*     Call of varkon_cur_eval (sur784)                             */
/*                                                                 !*/
/*     (Error SU2943 if called function fails)                      */
/*     One additional iteration                                     */
       niter = niter + 1;


   status=varkon_cur_eval (p_cur,p_seg,icase,tglobal,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"sur782%%varkon_cur_eval (sur784)");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 r_x %f r_y %f r_z %f tglobal %9.6f\n",
      xyz.r_x,   xyz.r_y,   xyz.r_z, tglobal);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 u_x %f u_y %f u_z %f \n",
      xyz.u_x,   xyz.u_y,   xyz.u_z);
  fprintf(dbgfil(SURPAC),
  "sur901 u2_x %f u2_y %f u2_z %f \n",
      xyz.u2_x,   xyz.u2_y,   xyz.u2_z);
  }
#endif

/*!                                                                 */
/*     Calculation of function f_cur and derivative dfdt.           */
/*     Call of intplan for ctype = CURPLAN                          */
/*     (Error SU2973 if internal function fails)                    */
/*                                                                 !*/

       f_pre       = f_cur;  /* Temporary !!!!! */

       if      ( ctype  == CURPLAN )
         {
         status=intplan();
         if (status<0) 
            {
            sprintf(errbuf,"intplan%%varkon_cur_num1 (sur901)");
            return(varkon_erpush("SU2973",errbuf));
            }
         }
#ifdef DEBUG  
       else
         {
         sprintf(errbuf,"(ctype 2)%% varkon_cur_num1 (sur901)");
         return(varkon_erpush("SU2993",errbuf));
         }
#endif         

/*                                                                  */
/*     Calculated data to u_a, f_a and d_a.                         */
/*                                                                  */
       if ( niter >= 50)
          {
          sprintf(errbuf,"(niter>50)%% varkon_cur_num1 (sur901)");
          return(varkon_erpush("SU2993",errbuf));
          }

       u_a[niter-1]= tglobal;
       f_a[niter-1]= f_cur;
       d_a[niter-1]= dfdt;

/*!                                                                 */
/*     Is function value f_cur <= c_zero ?                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 U %f f_cur %f c_zero %f dfdt %f\n"
   , tglobal, f_cur, c_zero, dfdt );
  }

if ( dbglev(SURPAC) == 1 && fabs(f_cur) < 10*c_zero )
{
for (ix1 = 1; ix1 <= niter; ix1 = ix1+1)
{
if ( fabs(f_a[ix1-1]) < 10000*c_zero )
{
  fprintf(dbgfil(SURPAC),
  "sur901 iteration %d U %25.20f f_cur %f dfdt %f\n"
   , ix1  , u_a[ix1-1], f_a[ix1-1],d_a[ix1-1]);
}
}
}
#endif

      if( fabs(f_cur) <= c_zero ) 
          {
/*!                                                                 */
/*     Yes, f_cur= 0 for curve parameter value tglobal:             */
/*          Increase the number of solutions numsol and add         */
/*          the solution (R*3 and U,V) to output array xyz_a        */
/*          if the parameter tglobal is between t_s and t_e         */
/*          and if the solution not exists (call of addsol).        */
/*                                                                 !*/
/*   ?????? Note ?: tglobal= t_s and t_e-TOL4 will always be        */
/*                  since the minimum number of restarts is 2       */
/*  TODO           !!!! oblem to have the solution as a u val */
/*           Accepting +/- TOL4 will give the wrong segment   */
/*           adress                                           */

          if ( tglobal >= t_s-TOL4)    /* Parameter is greater or   */
            {                          /* equal interval start t_s  */
            if ( tglobal <= t_e+TOL4)  /* Parameter value is less   */
              {                        /* than interval end t_e     */
              addsol(xyz_a);           /* Add non-existent solution */
              }                        /* End tglobal <= t_e        */
            }                          /* End tglobal >= t_s        */
          j_n = maxiter;               /* Refer to Note 2 above     */
          }                            /* End if f_cur <= c_zero    */

/*!                                                                 */
/*     Is derivative dfdt too small ?                               */
/*                                                                 !*/
          if(  fabs(dfdt) <= TOL3 )    /* Derivative <= TOL3 ?      */
            {                          /*                           */
/*!                                                                 */
/*     Yes, dfdt is less or equal TOL3:                             */
            j_n = maxiter;             /* ==> end of loop j_n       */
            h = TOL6;                  /* ??? Newton only ???       */
/*                                                                 !*/
            }                          /* End dfdt<= TOL3           */
          else                         /* Derivative >  TOL3        */
            {                          /*                           */
            h = -f_cur/dfdt;           /* Newton step ...           */
            }                          /* End if: dfdt <= TOL3      */


          /* Solution outside t_s < tglobal < t_e ? */
/* Why   fabs ?? if (fabs(tglobal-t_s) < 0.01*TOL4 ) */
              if (tglobal < t_s + 0.01*TOL4 )
              {
              if ( h < 0 ) j_n = maxiter;
              }

          if ( tglobal > t_e + 0.01*TOL4 )
              {
              if ( h > 0 ) j_n = maxiter;
              }
          if ( fabs(h) > 2.0 ) j_n = maxiter;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 fabs(tglobal-t_s)= %f 0.01*TOL4 %f h %f j_n %d\n",
      fabs(tglobal-t_s),0.01*TOL4 , h , j_n );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 fabs(tglobal-t_e)= %f 0.01*TOL4 %f h %f j_n %d\n",
      fabs(tglobal-t_e),0.01*TOL4 , h , j_n );
  }
#endif

          /* Compute the next parameter value    */
          tglobal = tglobal + h;
          if ( tglobal < t_s - 0.01*TOL4 ) 
              tglobal = t_s + 0.001*TOL4/(DBfloat)niter;
          if ( tglobal > t_e + 0.01*TOL4 ) 
              tglobal = t_e - 0.001*TOL4/(DBfloat)niter;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 tglobal= %f   i_r= %d  j_n= %d\n",
      tglobal, i_r, j_n );
  }
#endif
        if ( j_n > maxiter )
            {
            sprintf(errbuf,"(maxiter)%% varkon_cur_num1 (sur901)");
            return(varkon_erpush("SU2993",errbuf));
            }

        }   /* end loop j_n : Numerical solution    */
   }       /* end loop i_r : Restarts with deltat  */

/*!                                                                 */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */
/* Number of solutions to output variable                           */
   *p_npts = numsol; 
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if (*p_npts> 0 )
{
for (ix1 = 1; ix1 <= *p_npts; ix1 = ix1+1)
{
fprintf(dbgfil(SURPAC),
"sur901 Point %d U %9.6f X %9.2f Y %9.2f Z %9.2f\n",
      ix1 , xyz_a[ix1-1].u_global  ,
            xyz_a[ix1-1].r_x,xyz_a[ix1-1].r_y,xyz_a[ix1-1].r_z);
}
}
}
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 Exit varkon_cur_num1 *** *p_npts= %d\n",
     *p_npts );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef DEBUG
/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF and I_UNDEF.                */

   static short initial(t_in,p_comp,p_npts,xyz_a)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat    t_in[2];     /* Parameter curve segment limits          */
                         /* (0):   Start U    (1): End   U          */
   IRUNON  *p_comp;      /* Pointer to computation data             */
   int     *p_npts;      /* Number of output points (a pointer)     */
   EVALC   xyz_a[];      /* Array with all solutions                */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901 Enter varkon_cur_num1*initial ******\n");
  fprintf(dbgfil(SURPAC),
  "sur901 Start t_in(0) %f End t_in(1) %f\n",
   t_in[0],t_in[1]);
  fprintf(dbgfil(SURPAC),
  "sur901 Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      p_comp->ipl_un.ctype,
      p_comp->ipl_un.method, 
      p_comp->ipl_un.nstart, 
      p_comp->ipl_un.maxiter);
  if ( p_comp->ipl_un.ctype == CURPLAN)
      {
      fprintf(dbgfil(SURPAC),
      "sur901 Case CURPLAN:  Intersect plane: %f %f %f %f\n",
        p_comp->ipl_un.in_x,
        p_comp->ipl_un.in_y,
        p_comp->ipl_un.in_z,
        p_comp->ipl_un.in_d);
      }
  }

 fflush(dbgfil(SURPAC));

/*! The number of restarts must be between 2 and maxiter            */
/*  Error SU2993 if not OK.                                        !*/
    if (p_comp->ipl_un.nstart > 1 || 
        p_comp->ipl_un.nstart <= p_comp->ipl_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%% varkon_cur_num1 (sur901)");
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 2. Initialize output variables and static (internal) variables   */
/*                                                                 !*/
    *p_npts     = I_UNDEF;      

    pi_x   = F_UNDEF;     
    pi_y   = F_UNDEF;     
    pi_z   = F_UNDEF;     
    pi_d   = F_UNDEF;     

/* Initialize variabel EVALC                                        */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

      varkon_ini_evalc (&xyz_a[0]); 

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif

/*!************** Internal ** function ******************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calcultes the function f_cur and dfdt              */
/*  for calculation case:  CURPLAN                                  */
/*                                                                  */
               static  short   intplan()
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_cur_eval              */
/*     Plane pi_x, pi_y, pi_z, pi_d                                 */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*-----------------------------------------------------------------!*/


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

 f_cur  = pi_x*xyz.r_x + pi_y*xyz.r_y + pi_z*xyz.r_z  - pi_d;
 dfdt   = pi_x*xyz.u_x + pi_y*xyz.u_y + pi_z*xyz.u_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901*intplan tglobal= %f f_cur= %f dfdt= %f\n"
   , tglobal, f_cur, dfdt );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!************** Internal ** function ******************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Add a solution if it is a new solution (not already existing).  */
/*                                                                  */
/*  Note that the identical points criterion (ctol) is used for     */
/*  all types of interrogation (c_zero is not used).                */
/*                                                                  */
               static  short   addsol(xyz_a)
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALC   xyz_a[SMAX];  /* Array with all solutions                */

/*     Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_sol;         /* Loop index for previous solutions       */
 
/*-----------------------------------------------------------------!*/

   DBfloat  dist;          /* Distance between solution points (R*3)  */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur901*addsol ctol %f numsol %d \n"
   ,   ctol , numsol );
  }
#endif

/*!                                                                 */
/* 1. Check if solution already exists                              */
/* __________________________________                               */

/*  If numsol = 0 add solution (goto adds).                         */
/*                                                                 !*/

    if ( numsol == 0 ) goto  adds;

/*!                                                                 */
/*  Start loop previous solutions i_sol= 1,2, .. ,numsol.           */
/*                                                                 !*/

   for (i_sol = 1; i_sol <= numsol ; i_sol = i_sol+1)
     {

/*!                                                                 */
/*    Solution exists if the R*3 distance between xyz and xyz_a     */
/*    is less than ctol.  Goto sex if this is the case.             */
/*                                                                 !*/

     dist = SQRT (
       (xyz.r_x-xyz_a[i_sol-1].r_x)*(xyz.r_x-xyz_a[i_sol-1].r_x) +
       (xyz.r_y-xyz_a[i_sol-1].r_y)*(xyz.r_y-xyz_a[i_sol-1].r_y) +
       (xyz.r_z-xyz_a[i_sol-1].r_z)*(xyz.r_z-xyz_a[i_sol-1].r_z) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  dist > ctol )
  {
  fprintf(dbgfil(SURPAC),
  "sur901*addsol u %10.5f v %10.5f dist %f ctol %f \n",
   xyz_a[i_sol-1].u , xyz_a[i_sol-1].v , dist  ,   ctol );
  }
#endif

     if ( dist <= ctol ) goto sex;


     }  /* End loop i_sol= 1,2,..., numsol                          */



/*!                                                                 */
/* 2. Add solution                                                  */
/* _______________                                                  */


adds:  /* Label: Add solution                                       */

/*  Increase number of solutions.                                   */
    numsol = numsol + 1;
/*  Check that numsol is less or equal SMAX                         */
/*                                                                 !*/
    if ( numsol > SMAX )
       {
       sprintf(errbuf,"(SMAX)%% varkon_cur_num1 (sur901)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/*  Points and coordinates to xyz_a.                                */
/*                                                                 !*/

    xyz_a[numsol-1]=xyz;     /* The R*3 solution point    */

sex:  /*! Label sex: Solution already exists                       */

    return(SUCCED);


} /* End of function */

/*!****************************************************************!*/
