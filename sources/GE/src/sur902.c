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
/*  Function: varkon_sur_num2                      File: sur902.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of points on a surface, i.e a function which numerically        */
/*  finds the solution to a two variable problem.                   */
/*                                                                  */
/*  Input is a pointer to a structure which defines the             */
/*  calculation criterion for the points (the objective             */
/*  function), the method for the numerical calculation,            */
/*  the number of restarts, the tolerances, etc.                    */
/*                                                                  */
/*  The problem is solved as a non-linear optimization problem.     */
/*  The gradient method, with parabola interpolation for the        */
/*  one-dimensional search, is used.                                */
/*  The convergence is normally slow using the gradient and         */
/*  the search direction is modified with the Hessian matrix.       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-09   Originally written                                 */
/*  1995-06-07   LINTSUR - lintsur added                            */
/*  1995-08-27   ntol -> TOL7 for LINTSUR istart=5,6,7,8,9          */
/*               core for too many solutions !!!! ?????             */
/*  1995-09-16   pat_tra for LINTSUR                                */
/*  1995-09-30   Bug: hstep not initialized for _newst              */
/*               Minimum step TOL1/100 --> TOL1/10000               */
/*  1995-10-23   Tests: lintsur X value for angle computation       */
/*  1995-12-31   stepmin, lintsur objf_crit and tolerances          */
/*  1997-02-27   Bug for boundary                                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_num2       Surface two param. calculation   */
/*                                                              */
/*------------------------------------------------------------- */

/*!New-Page--------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short   initial();     /* Check and initiations For Debug On */
#endif
static short   retrieve();    /* Retrieve input to local variables  */
static short   newstart();    /* Calculate a new UV start point     */
static short   sdirect();     /* One-dimensional search direction   */
static short   stepleng();    /* Step length for one-dimen. search  */
static short   nearpt();      /* Objective, gradient.. for CLOSEPT  */
static short   lintsur();     /* Objective, gradient.. for LINTSUR  */
static short   parab3p();     /* Parabola interp.: 3 pts            */
static short   addsol();      /* Add a non-existent solution.       */
static DBfloat stepmin();     /* Minimum U,V step length            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Flow diagram -------------------------------------*/
/*                _________________                                 */
/*               !                 !                                */
/*               ! varkon_sur_num2 !                                */
/*               !    (sur902)     !                                */
/*               !_________________!                                */
/*    ____________________!__________________                       */
/*  _!_   _!_     _!_     _!_   _!_   _!_   _!_                     */
/* !   ! !   !   !   !   !   ! !   ! !   ! !   !                    */
/* ! 1 ! ! 2 ! L ! 3 ! L ! 4 ! ! 5 ! ! 6 ! ! 7 !                    */
/* !   ! !   ! a !   ! a !   ! !   ! !   ! !   !                    */
/* ! i ! ! r ! b ! n ! b ! O ! ! a ! ! s ! ! S !                    */
/* ! n ! ! e ! e ! e ! e ! b ! ! d ! ! d ! ! t !                    */
/* ! i ! ! t ! l ! w ! l ! j ! ! d ! ! i ! ! e !                    */
/* ! t ! ! r !   ! s !   ! e ! ! s ! ! r ! ! p !                    */
/* ! i ! ! i ! n ! t ! n ! c ! ! o ! ! e ! !   !                    */
/* ! a ! ! e ! e ! a ! o ! t ! ! l ! ! c ! !___!                    */
/* ! l ! ! v ! w ! r ! p ! i ! !___! ! t !   !                      */
/* !___! ! e ! s ! t ! t ! v !       !___!   !                      */
/*       !___! t !___!   ! e !               !                      */
/*                       !___!               !                      */
/*               __________!                 !_____________         */
/*           ___!____   ___!____          ___!______   ____!____    */
/*          ! nearpt ! !lintsur !        ! stepleng ! ! parab3p !   */
/*          !________! !________!        !__________! !_________!  !*/
/*                                                                  */
/*------------------------------------------------------------------*/
/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint    ctype;            /* Case of computation            */
static DBint    method;           /* Method which shall be used     */
static DBint    nstart;           /* The number of restarts         */
static DBint    istart;           /* The current restart number:    */
                                  /* Eq. -1: No restart             */
                                  /* Eq.  0: Start with input s_uvpt*/
                                  /* Eq.  1: Start with mid    pt   */
                                  /* Eq.  2: Start with corner pt 1 */
                                  /*   (Refer to newstart ..  )     */
static DBint    maxiter;          /* Maximum number of iterations   */
static DBTmat   pat_tra;          /* Transform. matrix for patches  */
static DBint    nopt;             /* Number of optimiz. iterations  */
static DBVector extpt;            /* External point                 */
static DBVector v_proj;           /* Projection vector (LINTSUR)    */
static DBVector s_uvpt;           /* Start UV point                 */
static DBint    uvstart;          /* Eq. 1: s_uvpt is defined       */
static DBfloat  us,vs,ue,ve;      /* The search area (a patch)      */
static DBfloat  u_pat,v_pat;      /* Current U,V point on surface   */
static DBfloat  objf;             /* Objective function             */
static DBfloat  objf_crit;        /* Objective fctn end criterion   */
static DBfloat  c_zero;           /* The zero (0.0) end criterion   */
static DBfloat  c_zero2;          /* Optional c_zero                */
static DBfloat  c_infi;           /* Infinite criterion             */
static DBfloat  c_denom;          /* Criterion for zero denominator */
static DBfloat  ctol;             /* Criterion for identical points */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
static DBfloat  dobjdu,dobjdv;    /* Gradient                       */
static DBfloat  d2objdu2,d2objdv2;/* Elements in the Hessian matrix */
static DBfloat  d2objdudv;        /* Element  in the Hessian matrix */
static DBfloat  dmdu,dmdv;        /* Search direction               */
static DBfloat  hstep;            /* Step U,V for one-dimen. search */
static DBfloat  hstep_r3min;      /* Minimum R*3 hstep              */
static DBfloat  gradl;            /* Length of gradient vector      */
static DBfloat  deterh;           /* Determinant of  Hessian matrix */
static DBfloat  obj_1;            /* Objective function value 1     */
static DBfloat  obj_2;            /* Objective function value 2     */
static DBfloat  obj_3;            /* Objective function value 3     */
static DBint    numsol;           /* Number of solutions (=*p_npts) */
static DBint    icase;            /* Case for varkon_sur_eval       */
static EVALS    xyz;              /* Coordinates and derivatives    */
                                  /* for a point on a surface       */
/*-----------------------------------------------------------------!*/
static DBint  n_iter;    /* For the maxiter check  !Temp.  static   */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval        * Surface evaluation routine              */
/* varkon_evals_transf    * Transformate EVALS                      */
/* varkon_angd            * Angle in deg. between vectors           */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_num2        */
/* SU2983 = sur902 Illegal computation case=   for varkon_sur_num2  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_num2      */
/* SU2993 = Severe program error in varkon_sur_num2 (sur902).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus       varkon_sur_num2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   SPOINT  *p_spt,       /* Current record in point table     (ptr) */
/* Out:                                                             */
   DBint   *p_npts,      /* Number of output points           (ptr) */
   EVALS   xyz_a[SMAX] ) /* Array with all solutions (R*3 and UV)   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

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
/*  Initialize output coordinates and derivatives For DEBUG On.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

#ifdef DEBUG
    status=initial(p_spt,p_comp,p_npts);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif


/*!                                                                 */
/* 2. Retrieve the input data                                       */
/* __________________________                                       */
/*                                                                  */
/*  Retrieve input data to local static variables.                  */
/*  Call of retrieve.                                               */
/*                                                                 !*/

    status=retrieve (p_spt,p_comp);
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"retrieve%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* 3. Restart (or start) of calculation                             */
/* ____________________________________                             */
/*                                                                  */
_newst:
/*                                                                  */
/*  Calculate a (new) UV start point.                               */
/*  Call of newstart  if istart <= nstart                           */
/*  Goto  _exit       if istart  > nstart                           */
/*                                                                 !*/

    if ( istart > nstart ) goto _exit;

    status=newstart ();
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"newstart%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* 4. Calculate objective function                                  */
/* _______________________________                                  */
/*                                                                  */
   n_iter = 0;            
/*                                                                  */
_nopt: /* Label: New optimization iteration                         */
/*                                                                 !*/

   n_iter = n_iter + 1 ;
   if ( n_iter > maxiter )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
fprintf(dbgfil(SURPAC),
"sur902 Number iterations n_iter= %d > maxiter= %d\n"
   ,n_iter , maxiter );
fprintf(dbgfil(SURPAC),
"sur902 Temporary fix: Make a new start \n");
  }
#endif
     sprintf(errbuf,"(maxiter)%%varkon_sur_num2 (sur902)");
  goto _newst;
/* Temporary fix   return(varkon_erpush("SU2993",errbuf));  */
     }

/*!                                                                 */
/*  Check that point u_pat,v_pat is within patch limit us,vs,ue,ve  */
/*  Calculate objective function, gradient and gradient length.     */
/*  Call of nearpt   for ctype= CLOSEPT                             */
/*                                                                 !*/

   if ( u_pat >=  us && u_pat  <= ue && 
         v_pat >=  vs && v_pat  <= ve    )
      {
      ;
      }
    else
      {
      u_pat = u_pat-hstep*dmdu;
      v_pat = v_pat-hstep*dmdv;
      nopt= -10; /* Calculate a new direction */
      }

    if ( ctype == CLOSEPT )
      {
      status= nearpt (p_sur,p_pat);
#ifdef DEBUG
      if (status<0) 
        {
        sprintf(errbuf,"nearpt%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif
      }
    else if ( ctype == LINTSUR )
      {
      status= lintsur (p_sur,p_pat);
#ifdef DEBUG
      if (status<0) 
        {
        sprintf(errbuf,"lintsur%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif
      }


#ifdef  DEBUG
if ( objf_crit < 0.1  )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur902 n_iter= %d u_pat %7.4f v_pat %7.4f objf_crit %15.12f nopt %d\n"
   ,n_iter,u_pat , v_pat , objf_crit , nopt );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur902 n_iter= %d dobjdu %f dobjdv %f\n"
   ,n_iter, dobjdu ,  dobjdv );
}
}
#endif



/*!                                                                 */
/* 5. Optimal point ?                                               */
/* ___________________                                              */
/*                                                                  */
/*  There is an optimal point if objf_crit < c_zero :               */
/*     i. Add the solution. Call of addsol.                         */
/*    ii. Make a new start. Goto _newst.                            */
/*                                                                 !*/

    if ( objf_crit <=  c_zero )
      {
      status= addsol (p_sur,p_pat,xyz_a);
#ifdef DEBUG
      if (status<0) 
        {
        sprintf(errbuf,"addsol%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif
      goto  _newst;
      }

/*!                                                                 */
/* 6. Search direction                                              */
/* ___________________                                              */
/*                                                                  */
/*  Calculate the one-dimensional search direction                  */
/*  Call of sdirect if nopt < 0.                                    */
/*                                                                 !*/

    if ( nopt < 0 ) status=sdirect();
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"sdirect%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 7. Step length for the one-dimensional search                    */
/* ______________________________________________                   */
/*                                                                  */
/*  Calculate or adjust the step length hstep.                      */
/*  Call of stepleng if nopt < 0.                                   */
/*  Solution is outside patch <==> hstep= 0  ==> Goto _newst.       */
/*  (Search direction in the current point)                         */
/*                                                                 !*/

    if ( nopt < 0 ) status=stepleng ();
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"stepleng%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif


#ifdef DEBUG
if ( objf_crit < 0.1 &&  dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur902 n_iter= %d hstep %18.12f dmdu %f dmdv %f\n"
   ,n_iter, hstep, dmdu ,  dmdv );
}

if ( dbglev(SURPAC) == 2 &&  fabs(hstep) < 100.0*stepmin() )
  {
  fprintf(dbgfil(SURPAC),
  "sur902 !hstep! %18.15f  stepmin %18.15f \n"
   , fabs(hstep), stepmin()  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

if ( dbglev(SURPAC) == 1 &&  fabs(hstep) < stepmin() )
  {
  fprintf(dbgfil(SURPAC),
  "sur902 !hstep! %18.15f < stepmin %18.15f ==> Goto _newst\n"
   , fabs(hstep), stepmin()  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

    if ( fabs(hstep) < stepmin() ) 
      {
      hstep = -c_infi;
      goto _newst;
      }

/*!                                                                 */
/*  If nopt < 0 (search direction and step length has been calc.)   */
/*     i. Let obj_1= objf                                           */
/*    ii. Let nopt= 1                                               */
/*   iii. Let u_pat= u_pat+hstep*dmdv  v_pat= v_pat+hstep*dmdv      */
/*    iv. Goto _nopt;                                               */
/*                                                                 !*/

   if ( nopt < 0 )
     {
     nopt  = 1;
     obj_1 = objf;
     u_pat = u_pat+hstep*dmdu;
     v_pat = v_pat+hstep*dmdv;
     goto  _nopt;
     }

/*!                                                                 */
/*  If      nopt = 1 and objf <  obj_1                              */
/*     i. Let obj_2= objf                                           */
/*    ii. Let nopt= 2                                               */
/*   iii. Let u_pat= u_pat+hstep*dmdv  v_pat= v_pat+hstep*dmdv      */
/*    iv. Goto _nopt;                                               */
/*  Else if nopt = 1 and objf >= obj_1                              */
/*     i. hstep = hstep/2 (if hstep > 0.1*stepmin)                  */
/*    ii. Goto _nopt;                                               */
/*                                                                 !*/

   if ( nopt == 1 && objf < obj_1 )
     {
     nopt  = 2;
     obj_2 = objf;
     u_pat = u_pat+hstep*dmdu;
     v_pat = v_pat+hstep*dmdv;
     goto  _nopt;
     }
   else if ( nopt == 1 && objf >= obj_1 )
     {
     hstep = hstep/2;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  fabs(hstep) < 100.0*stepmin() )
  {
  fprintf(dbgfil(SURPAC),
  "sur902 hstep %18.15f divided by two (2) and nopt = -10\n", fabs(hstep) );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/* Ugly programmed fix that only partly solves a problem */
     u_pat = u_pat-hstep*dmdu;
     v_pat = v_pat-hstep*dmdv;
     nopt = -10;
     if ( fabs(hstep) < 0.1*stepmin() )
        {
        sprintf(errbuf,"(hstep/2)%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2993",errbuf));
        }
     goto  _nopt;
     }

/*!                                                                 */
/*  If      nopt >= 2 and objf <  obj_2                             */
/*     i. Let obj_1= obj_2 and obj_2= objf                          */
/*    ii. Let nopt= nopt + 1                                        */
/*   iii. Let u_pat= u_pat+hstep*dmdv  v_pat= v_pat+hstep*dmdv      */
/*    iv. Goto _nopt;                                               */
/*  Else if nopt >= 2 and objf >= obj_2                             */
/*     i. Calculate hmin. Call of parab3p.                          */
/*    ii. Let nopt = -10                                            */
/*   iii. Goto _nopt.                                               */
/*                                                                 !*/

   if ( nopt >= 2 && objf < obj_2 )
     {
     nopt  = nopt + 1;
     obj_1 = obj_2;
     obj_2 = objf;
     u_pat = u_pat+hstep*dmdu;
     v_pat = v_pat+hstep*dmdv;
     goto  _nopt;
     }
   else if ( nopt >= 2 && objf >= obj_2 )
     {
     obj_3 = objf;
     status=parab3p();
     if (status<0) 
        {
        sprintf(errbuf,"parab3p%%varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2973",errbuf));
        }
     nopt  = -10;
     goto _nopt;
     }

/*!                                                                 */
_exit:    /* Last restart                                           */
/*                                                                 !*/

     *p_npts = numsol;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
fprintf(dbgfil(SURPAC),
"sur902 Exit*varkon_sur_num2 us %6.2f ue %6.2f vs %6.2f ve %6.2f n %d\n"
   ,p_spt->us ,  p_spt->ue ,  p_spt->vs ,  p_spt->ve , *p_npts);
  }
fflush(dbgfil(SURPAC));
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

#ifdef DEBUG
/*!********* Internal ** function ** initial **For*Debug*On**********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data and it initializes the        */
/* output variables and the static (common) variables to the        */
/* values 1.23456789 and 123456789.                                 */

   static short initial(p_spt,p_comp,p_npts)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   SPOINT  *p_spt;       /* Current record in point table     (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBint   *p_npts;      /* Number of output points           (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/* ==============================                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"Enter ***** varkon_sur_num2 (sur902) ** initial ******\n");
fprintf(dbgfil(SURPAC),
"sur902*initial Comp. case %d  Method %d  Restrts %d Max iter %d\n",
      p_comp->pro_un.ctype,
      p_comp->pro_un.method, 
      p_comp->pro_un.nstart, 
      p_comp->pro_un.maxiter);
if ( p_comp->pro_un.ctype == CLOSEPT )
{
fprintf(dbgfil(SURPAC),
"sur902*initial Case CLOSEPT: External point=  %10.2f %10.2f %10.2f \n",
        p_spt->extpt.x_gm,
        p_spt->extpt.y_gm,
        p_spt->extpt.z_gm); 
}
if ( p_comp->pro_un.ctype == LINTSUR )
{
fprintf(dbgfil(SURPAC),
"sur902*initial Case LINTSUR: External point=  %10.2f %10.2f %10.2f \n",
        p_spt->extpt.x_gm,
        p_spt->extpt.y_gm,
        p_spt->extpt.z_gm); 
fprintf(dbgfil(SURPAC),
"sur902*initial Case LINTSUR: Projection vec=  %10.2f %10.2f %10.2f \n",
        p_spt->proj.x_gm,
        p_spt->proj.y_gm,
        p_spt->proj.z_gm); 
}

fprintf(dbgfil(SURPAC),
"sur902*initial startuvpt %8.4f %8.4f %8.4f\n",
           p_spt->startuvpt.x_gm ,  
           p_spt->startuvpt.y_gm ,  
           p_spt->startuvpt.z_gm );
fprintf(dbgfil(SURPAC),
"sur902*initial Patch us %8.4f vs %8.4f ue %8.4f ve %8.4f\n",
           p_spt->us ,  
           p_spt->vs ,  
           p_spt->ue ,  
           p_spt->ve );
  }

 fflush(dbgfil(SURPAC)); /* To file from buffer      */


/*!                                                                 */
/* 2. Check of input data                                           */
/* ======================                                           */
/*                                                                  */
/* Check the UV area. Mininum rectangle sides are 10*TOL4.          */
/*                                                                 !*/

   if      ( fabs(p_spt->ue - p_spt->us) < 10.0*TOL4 ) 
       {
       sprintf(errbuf,"(ue-us<10*TOL4)%% varkon_sur_num2 (sur902)");
       return(varkon_erpush("SU2993",errbuf));
       }
   else if ( fabs(p_spt->ve - p_spt->vs) < 10.0*TOL4 ) 
       {
       sprintf(errbuf,"(ve-vs<10*TOL4)%% varkon_sur_num2 (sur902)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/* The number of maxiter  must be between 10 and 100.               */
/* The number of restarts must be between 2 and maxiter.            */
/*                                                                 !*/

    if (p_comp->pro_un.maxiter>=  10     || 
        p_comp->pro_un.maxiter<= 100   ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(maxiter)%% varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2993",errbuf));
        }

    if (p_comp->pro_un.nstart > 1 || 
        p_comp->pro_un.nstart <= p_comp->pro_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%% varkon_sur_num2 (sur902)");
        return(varkon_erpush("SU2993",errbuf));
        }

  if      ( p_comp->pro_un.ctype == CLOSEPT )
       {
       ;
       }
  else if ( p_comp->pro_un.ctype == LINTSUR )
       {
       ;
       }
   else
       {
       sprintf(errbuf,"(ctype)%% varkon_sur_num2 (sur902)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* 3. Initiate output variables and static (internal) variables     */
/* ============================================================     */
/*                                                                 !*/
    *p_npts     = -123456789;

    us          = -1.23456789;
    ue          = -1.23456789;
    vs          = -1.23456789;
    ve          = -1.23456789;

    ctype       = -12345;
    method      = -12345;
    nstart      = -12345;
    maxiter     = -12345;

    obj_1 = -1.23456789;          /* Objective function value 1     */
    obj_2 = -1.23456789;          /* Objective function value 2     */
    obj_3 = -1.23456789;          /* Objective function value 3     */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** nearpt ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the function f_cur and dfdt             */
/*  for calculation case:  CLOSEPT                                  */
/*                                                                  */
               static  short   nearpt(p_sur,p_pat)
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables: extpt, u_pat,v_pat                */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */

/* Out: Objective function objf, gradient and Hessian matrix        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------------- Theory --------------------------------------*/
/*                                                                  */
/* Objective function objf:                                         */
/* ------------------------                                         */
/*                                                                  */
/* The objective function is the distance from an external          */
/* point extpt to a point on the surface. The u,v point which       */
/* gives the minimum distance is the solution to the problem.       */
/* In this point is the surface normal parallell with the           */
/* vector from the surface point to the external point extpt.       */
/*                                                                  */
/* objf(u,v) = !r(u,v)-extpt!**2 =                                  */
/*       ( xyz.r_x-extpt.x_gm)**2 +                                 */
/*       ( xyz.r_y-extpt.y_gm)**2 +                                 */
/*       ( xyz.r_z-extpt.z_gm)**2                                   */
/*                                                                  */
/*                                                                  */
/* dobjdu    = d(objf)/du     =                                     */
/*       2*( xyz.r_x-extpt.x_gm)* xyz.u_x  +                        */
/*       2*( xyz.r_y-extpt.y_gm)* xyz.u_y  +                        */
/*       2*( xyz.r_z-extpt.z_gm)* xyz.u_z                           */
/*                                                                  */
/*                                                                  */
/* dobjdv    = d(objf)/dv     =                                     */
/*       2*( xyz.r_x-extpt.x_gm)* xyz.v_x  +                        */
/*       2*( xyz.r_y-extpt.y_gm)* xyz.v_y  +                        */
/*       2*( xyz.r_z-extpt.z_gm)* xyz.v_z                           */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* d2objdu2  = d2(objf)/du2   =                                     */
/*       2*xyz.u_x* xyz.u_x  +                                      */
/*       2*( xyz.r_x-extpt.x_gm)* xyz.u2_x +                        */
/*       2*xyz.u_y* xyz.u_y  +                                      */
/*       2*( xyz.r_y-extpt.y_gm)* xyz.u2_y +                        */
/*       2*xyz.u_z* xyz.u_z  +                                      */
/*       2*( xyz.r_z-extpt.z_gm)* xyz.u2_z                          */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/* d2objdv2  = d2(objf)/dv2   =                                     */
/*       2*xyz.v_x* xyz.v_x  +                                      */
/*       2*( xyz.r_x-extpt.x_gm)* xyz.v2_x +                        */
/*       2*xyz.v_y* xyz.v_y  +                                      */
/*       2*( xyz.r_y-extpt.y_gm)* xyz.v2_y +                        */
/*       2*xyz.v_z* xyz.v_z  +                                      */
/*       2*( xyz.r_z-extpt.z_gm)* xyz.v2_z                          */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* d2objdudv = d2(objf)/dudv  =                                     */
/*       2*xyz.v_x* xyz.u_x  +                                      */
/*       2*( xyz.r_x-extpt.x_gm)* xyz.uv_x +                        */
/*       2*xyz.v_y* xyz.u_y  +                                      */
/*       2*( xyz.r_y-extpt.y_gm)* xyz.uv_y +                        */
/*       2*xyz.v_z* xyz.u_z  +                                      */
/*       2*( xyz.r_z-extpt.z_gm)* xyz.uv_z                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat snorm[3];     /* Surface normal                          */
   DBfloat vextp[3];     /* Vector to external point                */
   DBfloat angle;        /* Angle between vectors                   */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Coordinates and derivatives                                   */
/* ==============================                                   */
/*                                                                  */
/* Evaluate coordinates and derivatives for u_pat,v_pat             */
/* Call of varkon_sur_eval (sur210).                                */
/*                                                                 !*/

   status=varkon_sur_eval
   (p_sur,p_pat,icase,u_pat,v_pat,&xyz);

#ifdef DEBUG
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_num2 (nearpt)");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif

/*!                                                                 */
/* Objective function value objf.                                   */
/*                                                                 !*/

   objf= ( xyz.r_x-extpt.x_gm )*( xyz.r_x-extpt.x_gm ) +
         ( xyz.r_y-extpt.y_gm )*( xyz.r_y-extpt.y_gm ) +
         ( xyz.r_z-extpt.z_gm )*( xyz.r_z-extpt.z_gm ); 

/*!                                                                 */
/* Gradient=  (dobjdu,dobjdv)                                       */
/*                                                                 !*/

   dobjdu= 2.0*( xyz.r_x-extpt.x_gm )*xyz.u_x +
           2.0*( xyz.r_y-extpt.y_gm )*xyz.u_y +
           2.0*( xyz.r_z-extpt.z_gm )*xyz.u_z; 

   dobjdv= 2.0*( xyz.r_x-extpt.x_gm )*xyz.v_x +
           2.0*( xyz.r_y-extpt.y_gm )*xyz.v_y +
           2.0*( xyz.r_z-extpt.z_gm )*xyz.v_z; 

/*!                                                                 */
/*            ! d2objdu2   d2objdudv !                              */
/* Hessian=   !                      !                              */
/*            ! d2objdudv  d2objdv2  !                              */
/*                                                                 !*/

   d2objdu2  = 
         2.0*  xyz.u_x* xyz.u_x  +               
         2.0*( xyz.r_x-extpt.x_gm)* xyz.u2_x +
         2.0*  xyz.u_y* xyz.u_y  +             
         2.0*( xyz.r_y-extpt.y_gm)* xyz.u2_y +
         2.0*  xyz.u_z* xyz.u_z  +             
         2.0*( xyz.r_z-extpt.z_gm)* xyz.u2_z;

   d2objdv2  = 
         2.0*  xyz.v_x* xyz.v_x  +               
         2.0*( xyz.r_x-extpt.x_gm)* xyz.v2_x +
         2.0*  xyz.v_y* xyz.v_y  +             
         2.0*( xyz.r_y-extpt.y_gm)* xyz.v2_y +
         2.0*  xyz.v_z* xyz.v_z  +             
         2.0*( xyz.r_z-extpt.z_gm)* xyz.v2_z;

   d2objdudv = 
         2.0*  xyz.v_x* xyz.u_x  +             
         2.0*( xyz.r_x-extpt.x_gm)* xyz.uv_x +
         2.0*  xyz.v_y* xyz.u_y  + 
         2.0*( xyz.r_y-extpt.y_gm)* xyz.uv_y + 
         2.0*  xyz.v_z* xyz.u_z  +           
         2.0*( xyz.r_z-extpt.z_gm)* xyz.uv_z;

/*!                                                                 */
/* Gradient vector length  gradl.                                   */
/*                                                                 !*/

   gradl= SQRT(dobjdu*dobjdu + dobjdv*dobjdv);

/*!                                                                 */
/* Angle between surface normal and vector to extpt.                */
/* Call of varkon_angd (angd).                                      */
/* Let objf_crit= angle between the two vectors.                    */
/*                                                                 !*/

   snorm[0] = xyz.n_x;
   snorm[1] = xyz.n_y;
   snorm[2] = xyz.n_z;
   vextp[0] = extpt.x_gm - xyz.r_x;
   vextp[1] = extpt.y_gm - xyz.r_y;
   vextp[2] = extpt.z_gm - xyz.r_z;

   if ( objf > 2*ctol )
     {
     status=varkon_angd (snorm , vextp, &angle );
     if (status<0) 
        {
        sprintf(errbuf,"angd%%varkon_sur_num2 (nearpt)");
        return(varkon_erpush("SU2943",errbuf));
        }
        if ( angle > 90.0 )
          {
          objf_crit = 180.0 - angle;
          }
        else
          {
          objf_crit = angle;
          }
     }
     else    /* Vector does not exist Added 1995-08-26 */
             /* Hopefully will gradient length be zero !!!! */
       {
       objf_crit = SQRT(objf);
       c_zero    = c_zero2;      /* Change to coordinate criterion */
       }




#ifdef DEBUG
/*  if ( dbglev(SURPAC) == 1 && objf_crit <  1.0 )  ODIN */
if ( dbglev(SURPAC) == 1 )
  {
  if ( angle > 90.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*nearpt Dist %8.1f Angle %f u_pat %8.4f v_pat %8.4f \n"
   ,  -SQRT(objf) , objf_crit , u_pat , v_pat );
   }
   else
   {
  fprintf(dbgfil(SURPAC),
  "sur902*nearpt Dist %8.1f Angle %f u_pat %8.4f v_pat %8.4f \n"
   ,   SQRT(objf) , objf_crit , u_pat , v_pat );
   }
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*nearpt objf %f dobjdu %f dobjdv %f\n"
   ,   objf , dobjdu, dobjdv );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*nearpt objf %10.1f gradl %f u_pat %8.4f v_pat %8.4f \n"
   ,   objf , gradl , u_pat , v_pat );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*nearpt d2objdu2 %f d2objdv2 %f d2objdudv %f \n"
   , d2objdu2, d2objdv2 , d2objdudv );
  }
fflush(dbgfil(SURPAC)); /* To file from buffer      */
#endif

       return(SUCCED);


} /* End of function */

/*!****************************************************************!*/

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** lintsur *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the function f_cur and dfdt             */
/*  for calculation case:  LINTSUR                                  */
/*                                                                  */
               static  short lintsur (p_sur,p_pat)
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables: extpt, (v_proj), u_pat,v_pat      */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */

/* Out: Objective function objf, gradient and Hessian matrix        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------------- Theory --------------------------------------*/
/*                                                                  */
/* Objective function objf:                                         */
/* ------------------------                                         */
/*                                                                  */
/* The objective function is the distance in the YZ plane           */
/* to the point (0,0,0). The surface has been transformated...      */
/* The u,v point which gives the minimum distance is the solution   */
/* to the problem. This distance is zero .....                      */
/* In this point is the X axis         parallell with the           */
/* vector from the surface point to the external point (0,0,0).     */
/*                                                                  */
/* objf(u,v) = !r(u,v)!**2 in YZ plane =                            */
/*       ( xyz.r_y)**2 +                                            */
/*       ( xyz.r_z)**2                                              */
/*                                                                  */
/*                                                                  */
/* dobjdu    = d(objf)/du     =                                     */
/*       2*( xyz.r_y)* xyz.u_y  +                                   */
/*       2*( xyz.r_z)* xyz.u_z                                      */
/*                                                                  */
/*                                                                  */
/* dobjdv    = d(objf)/dv     =                                     */
/*       2*( xyz.r_y)* xyz.v_y  +                                   */
/*       2*( xyz.r_z)* xyz.v_z                                      */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* d2objdu2  = d2(objf)/du2   =                                     */
/*       2*xyz.u_y* xyz.u_y  +                                      */
/*       2*( xyz.r_y)* xyz.u2_y +                                   */
/*       2*xyz.u_z* xyz.u_z  +                                      */
/*       2*( xyz.r_z)* xyz.u2_z                                     */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/* d2objdv2  = d2(objf)/dv2   =                                     */
/*       2*xyz.v_y* xyz.v_y  +                                      */
/*       2*( xyz.r_y)* xyz.v2_y +                                   */
/*       2*xyz.v_z* xyz.v_z  +                                      */
/*       2*( xyz.r_z)* xyz.v2_z                                     */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* d2objdudv = d2(objf)/dudv  =                                     */
/*       2*xyz.v_y* xyz.u_y  +                                      */
/*       2*( xyz.r_y)* xyz.uv_y +                                   */
/*       2*xyz.v_z* xyz.u_z  +                                      */
/*       2*( xyz.r_z)* xyz.uv_z                                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALS xyz_tra;        /* Transformed oordinates and derivatives  */
                         /* for a point on a surface                */
/*----------------------------------------------------------------- */

   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Coordinates and derivatives                                   */
/* ==============================                                   */
/*                                                                  */
/* Evaluate coordinates and derivatives for u_pat,v_pat             */
/* Call of varkon_sur_eval (sur210).                                */
/*                                                                 !*/

   status=varkon_sur_eval
   (p_sur,p_pat,icase,u_pat,v_pat,&xyz);

#ifdef DEBUG
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_num2 (lintsur)");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif

/*!                                                                 */
/* Transformate coordinates and derivatives to "xaxis=line system"  */
/* Call of varkon_evals_transf (sur640).                            */
/*                                                                 !*/

   status=varkon_evals_transf
   ( &xyz, &pat_tra, &xyz_tra );

   if (status<0) 
   {
     sprintf(errbuf,"varkon_evals_transf%%varkon_sur_num2 (lintsur)");
     return(varkon_erpush("SU2943",errbuf));
   }

/*!                                                                 */
/* Objective function value objf.                                   */
/*                                                                 !*/

   objf= 
         ( xyz_tra.r_y)*( xyz_tra.r_y) +
         ( xyz_tra.r_z)*( xyz_tra.r_z); 

/*!                                                                 */
/* Gradient=  (dobjdu,dobjdv)                                       */
/*                                                                 !*/

   dobjdu= 
           2.0*( xyz_tra.r_y)*xyz_tra.u_y +
           2.0*( xyz_tra.r_z)*xyz_tra.u_z; 

   dobjdv= 
           2.0*( xyz_tra.r_y)*xyz_tra.v_y +
           2.0*( xyz_tra.r_z)*xyz_tra.v_z; 

/*!                                                                 */
/*            ! d2objdu2   d2objdudv !                              */
/* Hessian=   !                      !                              */
/*            ! d2objdudv  d2objdv2  !                              */
/*                                                                 !*/

   d2objdu2  = 
         2.0*  xyz_tra.u_y* xyz_tra.u_y  +             
         2.0*( xyz_tra.r_y)* xyz_tra.u2_y +
         2.0*  xyz_tra.u_z* xyz_tra.u_z  +             
         2.0*( xyz_tra.r_z)* xyz_tra.u2_z;

   d2objdv2  = 
         2.0*  xyz_tra.v_y* xyz_tra.v_y  +             
         2.0*( xyz_tra.r_y)* xyz_tra.v2_y +
         2.0*  xyz_tra.v_z* xyz_tra.v_z  +             
         2.0*( xyz_tra.r_z)* xyz_tra.v2_z;

   d2objdudv = 
         2.0*  xyz_tra.v_y* xyz_tra.u_y  + 
         2.0*( xyz_tra.r_y)* xyz_tra.uv_y + 
         2.0*  xyz_tra.v_z* xyz_tra.u_z  +           
         2.0*( xyz_tra.r_z)* xyz_tra.uv_z;

/*!                                                                 */
/* Gradient vector length  gradl.                                   */
/*                                                                 !*/

   gradl= SQRT(dobjdu*dobjdu + dobjdv*dobjdv);

   objf_crit = SQRT(objf);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && objf_crit < 5.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*lintsur Distance (objf_crit) %15.10f u_pat %8.4f v_pat %8.4f \n"
   ,  objf_crit , u_pat , v_pat );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*lintsur objf %f dobjdu %f dobjdv %f\n"
   ,   objf , dobjdu, dobjdv );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*lintsur objf %10.1f gradl %f u_pat %8.4f v_pat %8.4f \n"
   ,   objf , gradl , u_pat , v_pat );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*lintsur d2objdu2 %f d2objdv2 %f d2objdudv %f \n"
   , d2objdu2, d2objdv2 , d2objdudv );
  }
fflush(dbgfil(SURPAC)); 
#endif

       return(SUCCED);


} /* End of function */

/*!****************************************************************!*/

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** parab3p *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function interpolates a start point, a mid point and        */
/*  an end point with a parabola and calculates hmin for the        */
/*  minimum point.                                                  */
/*                                                                  */
                 static  short   parab3p()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In and out: Global (static) variables:                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  a0,a1,a2;    /* Parabola coefficients                   */
   DBfloat  hmin;        /* Minimum for X=hmin                      */
   DBfloat  minm;        /* Minimum minm = a1*hmin+a2*hmin**2       */
 
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*parab3p obj_1 %f obj_2 %f obj_3 %f hstep %f\n",
                  obj_1  , obj_2 ,  obj_3 ,  hstep );
  }
#endif

/*!                                                                 */
/* 1. Coefficients for the parabola                                 */
/* --------------------------------                                 */

/* Parabola interpolation of the three points                       */
/* Y = a0 + a1*X + a2*X**2         (parabola)                       */
/*                                                                 !*/

   a0 = obj_1;
   a2 = (obj_1/2.0 - obj_2 + obj_3/2.0)/hstep/hstep;
   a1 = (obj_2 - obj_1)/hstep - a2*hstep;
   /* Minimum point */
   if ( ABS(a2) > c_denom )
      {
      hmin = - a1/2.0/a2;
      minm = a0 + a1*hmin + a2*hmin*hmin;
      }
   else 
      {
      hmin = hstep;
      minm = obj_2;
      }


   if ( minm <= obj_1 && minm <= obj_2 && minm <= obj_3 )
       {
       /* Go (back) to hmin  */
       u_pat = u_pat - (2.0*hstep - hmin)*dmdu;
       v_pat = v_pat - (2.0*hstep - hmin)*dmdv;
       }
   else 
       {
       sprintf(errbuf,"(minm)%% varkon_sur_num2 (sur902)");
       return(varkon_erpush("SU2993",errbuf));
       }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && n_iter > maxiter-5 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*parab3p minm  %f hmin  %f u_pat %f v_pat %f\n",
                  minm   , hmin  ,  u_pat ,  v_pat );
  }
fflush(dbgfil(SURPAC)); 
#endif


    
       return(SUCCED);


} /* End of function */

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** addsol ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Add a solution if it is a new solution (not already existing).  */
/*                                                                  */
/*  Note that the identical points criterion (ctol) is used for     */
/*  all types of interrogation (c_zero is not used).                */
/*                                                                  */
               static  short   addsol(p_sur,p_pat,xyz_a)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   EVALS    xyz_a[SMAX]; /* Array with all solutions (R*3 and UV)   */

/*     Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*      .....                                                       */
/* Out: Function                                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_sol;        /* Loop index for previous solutions       */
   DBint   icase_l;      /* Calculation case for varkon_sur_eval    */
 
/*-----------------------------------------------------------------!*/

   DBfloat dist;         /* Distance between solution points (R*3)  */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*addsol ctol %f numsol %d \n"
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
/* 1995-08-27 if ( dbglev(SURPAC) == 1 &&  dist > 10*ctol )  */
if ( dbglev(SURPAC) == 1  )
 {
 fprintf(dbgfil(SURPAC),
 "sur902*addsol u %10.5f v %10.5f dist to prev. solut. pt %25.10f\n",
  xyz_a[i_sol-1].u , xyz_a[i_sol-1].v , dist );
 }
#endif

     if ( dist <= 10.0*ctol ) goto sex;


     }  /* End loop i_sol= 1,2,..., numsol                          */



/*!                                                                 */
/* 2. Add solution                                                  */
/* _______________                                                  */
/*                                                                  */
adds:  /* Label: Add solution                                       */
/*                                                                  */
/*  Increase number of solutions.                                   */
    numsol = numsol + 1;
/*  Check that numsol is less or equal SMAX                         */
/*                                                                 !*/
    if ( numsol > SMAX )
       {
       sprintf(errbuf,"(SMAX)%% varkon_sur_num2 (sur902)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/*  Recompute xyz with icase_l= 3 for  xxx???                       */
/*                                                                 !*/

    if ( ctype  == SURPLAN )
       {
       icase_l = 3;  
       status=varkon_sur_eval
       (p_sur,p_pat,icase_l,xyz.u,xyz.v,&xyz);
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_num2 (addsol)");
        return(varkon_erpush("SU2943",errbuf));
        }
       }

/*!                                                                 */
/*  Points and coordinates to xyz_a.                                */
/*                                                                 !*/

    xyz_a[numsol-1]=xyz;     /* The R*3 solution point    */



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*addsol numsol %d u %8.5f v %8.5f n_iter %d objf_crit %f\n",
   numsol , xyz_a[numsol-1].u , xyz_a[numsol-1].v , n_iter, objf_crit );
  }
fflush(dbgfil(SURPAC)); 
#endif





sex:  /*! Label sex: Solution already exists                        !*/

    return(SUCCED);


} /* End of function */

/*!****************************************************************!*/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** retrieve ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function retrieves input data to local static variables.     */

   static short retrieve(p_spt,p_comp)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   SPOINT  *p_spt;       /* Current record in point table     (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Computation data.                                             */
/* ====================                                             */
/*                                                                  */
/* Case of computation, method and number of restarts, etc          */
/* Retrieve ctype, method, nstart and maxiter from p_comp.          */
/*                                                                 !*/

   ctype   = p_comp->pro_un.ctype;
   method  = p_comp->pro_un.method;
   nstart  = p_comp->pro_un.nstart;
   maxiter = p_comp->pro_un.maxiter;
   pat_tra = p_comp->pro_un.pat_tra; /* 1995-09-16 */

/*!                                                                 */
/* Let istart= -1      (start value for number of restarts)         */
/* Let hstep = -c_infi (start value for search step)                */
/* Let numsol= 0       (start value for number of solutions)        */
/*                                                                 !*/

   c_infi      = 50000.0;  
   istart    = -1;
   hstep     = -c_infi;
   numsol    = 0;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

  c_denom = comptol*1000.0;

/*!                                                                 */
/* 2. Point data.                                                   */
/* ==============                                                   */

/* Retrieve extpt       for ctype= CLOSEPT from SPOINT record p_spt */
/* Let c_zero= ntol     and icase= 3 for ctype= CLOSEPT             */
/* Retrieve extpt, proj for ctype= LINTSUR from SPOINT record p_spt */
/* Let c_zero= ctol     and icase= 3 for ctype= LINTSUR             */
/*                                                                 !*/

   if      ( ctype  == CLOSEPT )
       {
       extpt.x_gm  = p_spt->extpt.x_gm ;
       extpt.y_gm  = p_spt->extpt.y_gm ;
       extpt.z_gm  = p_spt->extpt.z_gm ;
       v_proj.x_gm = 0.0;               
       v_proj.y_gm = 0.0;               
       v_proj.z_gm = 0.0;               

       icase = 3;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       c_zero      = p_comp->pro_un.ntol;
       c_zero2     = p_comp->pro_un.ctol;
       hstep_r3min = p_comp->pro_un.ctol/100.0;
       ctol        = p_comp->pro_un.ctol/10.0;  /* Previous value  (/10)... */
       }
   else if ( ctype  == LINTSUR )
       {
       extpt.x_gm  = p_spt->extpt.x_gm ;
       extpt.y_gm  = p_spt->extpt.y_gm ;
       extpt.z_gm  = p_spt->extpt.z_gm ;
       v_proj.x_gm = p_spt->proj.x_gm ;
       v_proj.y_gm = p_spt->proj.y_gm ;
       v_proj.z_gm = p_spt->proj.z_gm ;

       icase = 3;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       c_zero      = p_comp->pro_un.ctol;
       c_zero2     = c_zero;
       hstep_r3min = p_comp->pro_un.ctol/100.0;
       ctol        = p_comp->pro_un.ctol/10.0;  /* Previous value ... (/10) */
       }


/*!                                                                 */
/* Retrieve search area us,vs,ue,ve        from SPOINT record p_spt */
/*                                                                 !*/

   us =    p_spt->us;   
   vs =    p_spt->vs;   
   ue =    p_spt->ue;   
   ve =    p_spt->ve;

/*!                                                                 */
/* If the start UV point is defined:                                */
/*    i. Retrieve s_uvpt from SPOINT record p_spt                   */
/*   ii. Let flag uvstart=  1                                       */
/* If the start UV point not is defined:                            */
/*    i. Let flag uvstart= -1                                       */
/*                                                                 !*/

   if ( p_spt->startuvpt.z_gm >= 0.0 ) 
      {
      s_uvpt  = p_spt->startuvpt;   
      uvstart = 1;
      }
   else
      {
      uvstart =  -1;
      }



#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur902*retrieve ctype %d method %d nstart %d maxiter %d istart %d\n",
        ctype,    method,    nstart,    maxiter , istart );

if ( ctype == CLOSEPT )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*retrieve Case CLOSEPT: extpt   %10.2f %10.2f %10.2f\n",
   extpt.x_gm, extpt.y_gm, extpt.z_gm ); 
  }

if ( ctype == LINTSUR )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*retrieve Case LINTSUR: extpt   %10.2f %10.2f %10.2f\n",
   extpt.x_gm, extpt.y_gm, extpt.z_gm ); 
  }

fprintf(dbgfil(SURPAC),
"sur902*retrieve s_uvpt %8.4f %8.4f %8.4f uvstart %d\n",
    s_uvpt.x_gm , s_uvpt.y_gm , s_uvpt.z_gm , uvstart );

fprintf(dbgfil(SURPAC),
"sur902*retrieve Patch us %8.4f vs %8.4f ue %8.4f ve %8.4f\n",
    us ,  vs ,  ue ,  ve );

fprintf(dbgfil(SURPAC),
"sur902*retrieve icase %d   c_zero %f   ctol %f hstep %f\n",
    icase , c_zero , ctol , hstep);

}  /* End dbglev */

#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** newstart ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates a new UV start value                     */

   static short newstart()
/*                                                                 !*/
/********************************************************************/


/* ------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

/* Out:                                                             */
/*----------------------------------------------------------------- */

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate restart value                                       */
/* ===========================                                      */
/*                                                                  */
/* Start value for the calculation if a  start UV point is defined. */
/* If      istart = -1   and  uvstart = 1:                          */
/*    i. Let current UV point be u_pat,v_pat= startuvpt             */
/*   ii. Let istart = 0                                             */
/*                                                                 !*/

   if ( istart == -1  &&  uvstart == 1 )
     {
     u_pat   = s_uvpt.x_gm;
     v_pat   = s_uvpt.y_gm;
     istart  = 0;
     }


/*!                                                                 */
/* Start value for the calculation if no start UV point is defined. */
/* Else if istart = -1   and  uvstart = -1:                         */
/*    i. Let current UV point be u_pat,v_pat be patch mid point     */
/*   ii. Let istart = 1                                             */
/*                                                                 !*/

   else if ( istart == -1  &&  uvstart == -1 )
     {
     u_pat   = us + 0.5*(ue-us);
     v_pat   = vs + 0.5*(ve-vs);
     istart  = 1;
     }

/*!                                                                 */
/* Start value when the start UV point s_uvpt is used               */
/* Else if istart =  0                                              */
/*    i. Let current UV point be u_pat,v_pat be patch mid point     */
/*   ii. Let istart = 1                                             */
/*                                                                 !*/

   else if ( istart ==  0  )
     {
     u_pat   = us + 0.5*(ue-us);
     v_pat   = vs + 0.5*(ve-vs);
     istart  = 1;
     }

/*!                                                                 */
/* Else if istart =  1                                              */
/*    i. Let current UV point be u_pat= us and v_pat= vs            */
/*   ii. Let istart = 2                                             */
/*                                                                 !*/

   else if ( istart ==  1  )
     {
     u_pat   = us;
     v_pat   = vs;
     istart  = 2;
     }

/*!                                                                 */
/* Else if istart =  2                                              */
/*    i. Let current UV point be u_pat= us and v_pat= ve            */
/*   ii. Let istart = 3                                             */
/*                                                                 !*/

   else if ( istart ==  2  )
     {
     u_pat   = us;
     v_pat   = ve;
     istart  = 3;
     }

/*!                                                                 */
/* Else if istart =  3                                              */
/*    i. Let current UV point be u_pat= ue and v_pat= ve            */
/*   ii. Let istart = 4                                             */
/*                                                                 !*/

   else if ( istart ==  3  )
     {
     u_pat   = ue;
     v_pat   = ve;
     istart  = 4;
     }

/*!                                                                 */
/* Else if istart =  4                                              */
/*    i. Let current UV point be u_pat= ue and v_pat= vs            */
/*   ii. Let istart = 5                                             */
/*                                                                 !*/

   else if ( istart ==  4  )
     {
     u_pat   = ue;
     v_pat   = vs;
     istart  = 5;
     }

/*!                                                                 */
/* Else if istart =  5                                              */
/*    i. Let current UV point be u_pat= (ue-us)/4 and               */
/*                               v_pat= (ve-vs)/2                   */
/*   ii. Let istart = 6                                             */
/*                                                                 !*/

   else if ( istart ==  5  )
     {
     u_pat   = us + 0.25*(ue-us);
     v_pat   = vs + 0.50*(ve-vs);
     istart  = 6;
     }

/*!                                                                 */
/* Else if istart =  6                                              */
/*    i. Let current UV point be u_pat= 3*(ue-us)/4 and             */
/*                               v_pat=   (ve-vs)/2                 */
/*   ii. Let istart = 7                                             */
/*                                                                 !*/

   else if ( istart ==  6  )
     {
     u_pat   = us + 0.75*(ue-us);
     v_pat   = vs + 0.50*(ve-vs);
     istart  = 7;
     }

/*!                                                                 */
/* Else if istart =  7                                              */
/*    i. Let current UV point be u_pat=   (ue-us)/2 and             */
/*                               v_pat=   (ve-vs)/4                 */
/*   ii. Let istart = 8                                             */
/*                                                                 !*/

   else if ( istart ==  7  )
     {
     u_pat   = us + 0.50*(ue-us);
     v_pat   = vs + 0.25*(ve-vs);
     istart  = 8;
     }

/*!                                                                 */
/* Else if istart =  8                                              */
/*    i. Let current UV point be u_pat=   (ue-us)/2 and             */
/*                               v_pat= 3*(ve-vs)/4                 */
/*   ii. Let istart = 9                                             */
/*                                                                 !*/

   else if ( istart ==  8  )
     {
     u_pat   = us + 0.50*(ue-us);
     v_pat   = vs + 0.75*(ve-vs);
     istart  = 9;
     }

/*!                                                                 */
/* Else : Error                                                     */
/*                                                                 !*/

   else
     {
#ifdef  DEBUG
     if ( dbglev(SURPAC) == 1 )
     {
     fprintf(dbgfil(SURPAC),
     "sur902*newstart Error istart %d uvstart %d\n",
         istart , uvstart );
     } 
#endif
     sprintf(errbuf,"(istart)%% varkon_sur_num2 (sur902)");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* 2. Start value loop variables                                    */
/* =============================                                    */
/*                                                                  */
/* Start value for the the number of optimization iterations:       */
   nopt = -10;
/*                                                                 !*/




#ifdef  DEBUG

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur902*newstart u_pat %f v_pat %f  istart %d nopt %d hstep %18.12f\n",
        u_pat ,    v_pat , istart , nopt , hstep);

}  /* End dbglev */
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** stepleng ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the step length hstep for the           */
/*  one-dimensional search.                                         */
/*                                                                  */
               static  short stepleng()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables: gradient, ..                      */

/* Out: Step length hstep                                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------------- Theory --------------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  ustep;       /* Step in the U direction                 */
   DBfloat  vstep;       /* Step in the V direction                 */
   DBfloat  gradm;       /* Length of (dmdu,dmdv) search direction  */
 
/*-----------------------------------------------------------------!*/

#ifdef DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif

/*------------end-of-declarations-----------------------------------*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 &&  fabs(hstep) < 10.0*stepmin()  )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng Input hstep %18.15f\n" , hstep );
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Step length                                                   */
/* ==============                                                   */
/*                                                                  */
/* Search vector length    gradm                                    */
/*                                                                 !*/

   gradm= SQRT(dmdu*dmdu + dmdv*dmdv);

   if ( gradm < c_denom ) goto noadj;

/*!                                                                 */
/* Maximum step in U direction ustep (minimum 3 steps)              */
/*                                                                 !*/

   if  ( fabs(dmdu) < c_denom )
     {
     ustep = c_infi;           
     }
   else if ( dmdu >  0.0  )
     {
/*     ustep  = (u_pat-us)/3.0/dmdu;   1997-02-27  */
     ustep  = (u_pat-us-comptol)/3.0/dmdu;
     if ( ustep < 0 ) ustep = c_infi;
     }
   else if ( dmdu < 0.0  )
     {
/*   ustep = -(ue-u_pat)/3.0/dmdu;  1997-02-27  */
     ustep = -(ue+comptol-u_pat)/3.0/dmdu;
     if ( ustep < 0 ) ustep = c_infi; 
     }


/*!                                                                 */
/* Maximum step in V direction vstep (minimum 3 steps)              */
/*                                                                 !*/

   if  ( fabs(dmdv) < c_denom )
     {
     vstep = c_infi;           
     }
   else if ( dmdv >  0.0  )
     {
/*   vstep  = (v_pat-vs)/3.0/dmdv;     1997-02-27  */
     vstep  = (v_pat-vs-comptol)/3.0/dmdv;
     if ( vstep < 0 ) vstep = c_infi;
     }
   else if ( dmdv < 0.0  )
     {
/*   vstep = -(ve-v_pat)/3.0/dmdv;   1997-02-27 */
     vstep = -(ve+comptol-v_pat)/3.0/dmdv;
     if ( vstep < 0 ) vstep = c_infi;
     }


/*!                                                                 */
/* Choose the minimum value of (input) hstep, ustep and vstep.      */
/* If hstep has the minimum value, modify hstep to exactly          */
/* "hit" a patch border.                                            */
/*                                                                 !*/

#ifdef DEBUG
if(ustep < 0.0 || vstep < 0.0 || hstep > 0.0 )
     {
     sprintf(errbuf,"(step sign)%% varkon_sur_num2 (sur902)");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

   if      ( ustep < -hstep  &&  ustep < vstep )
     {
     hstep = -ustep;
     }
   else if ( vstep < -hstep  &&  vstep < ustep )
     {
     hstep = -vstep;
     }
   else /* hstep has the minimum value */
     {
     if ( ustep < vstep )
        {
        while ( ustep > -hstep ) ustep= ustep/2.0;
        hstep = -ustep;
        }
     else
        {
        while ( vstep > -hstep ) vstep= vstep/2.0;
        hstep = -vstep;
        }
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  objf_crit < 1000.0*c_zero                       )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng ustep %12.9f vstep %12.9f hstep %12.9f (out)\n",
                   ustep,vstep,hstep );
  fflush(dbgfil(SURPAC));
  }
#endif


/*!                                                                 */


/*!                                                                 */
/* If the first point is outside us,ue,vs,ve is the current         */
/* point on the border and search will be outside area.             */
/* Let hstep= 0 in this case.                                       */
/*                                                                 !*/


   if ( u_pat + hstep*dmdu >=  us && u_pat + hstep*dmdu  <= ue && 
        v_pat + hstep*dmdv >=  vs && v_pat + hstep*dmdv  <= ve    )
      {
      ;
      }
    else
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng Outside u_pat            %f  v_pat            %f\n"
   ,   u_pat           ,v_pat            );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng Outside u_pat+hstep*dmdu %f  v_pat+hstep*dmdv %f\n"
   ,   u_pat+hstep*dmdu,v_pat+hstep*dmdv );
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng ustep %f vstep %f hstep %f \n"
   ,   ustep, vstep, hstep );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng dmdu %f dmdv %f gradm %f \n"
   , dmdu , dmdv , gradm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng us %6.3f ue %6.3f vs %6.3f ve %6.3f\n"
   , us, ue, vs, ve      );
  }
#endif
      hstep = 0.0;               
      }


noadj:    /* No adjustment of the step length   (gradm= 0)          */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 &&  (n_iter > maxiter-5 || fabs(hstep) < 10.0*stepmin()  ))
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng ue-u_pat %f  ve-v_pat %f\n"
   ,   ue-u_pat,ve-v_pat );
  }
if ( dbglev(SURPAC) == 2 &&  (n_iter > maxiter-5 || fabs(hstep) < 10.0*stepmin()  ))
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng ustep %f vstep %f hstep %f \n"
   ,   ustep, vstep, hstep );
  }
if ( dbglev(SURPAC) == 2 &&  (n_iter > maxiter-5 || fabs(hstep) < 10.0*stepmin()  ))
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepleng dmdu %f dmdv %f gradm %f hstep %18.15f\n"
   , dmdu , dmdv , gradm, hstep );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/





/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** sdirect *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the direction for the                   */
/*  one-dimensional search  (dmdu,dmdv).                            */
/*                                                                  */
               static  short sdirect ()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables: gradient dmdu,dmdv                */

/* Out: The search direction dmdu,dmdv                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------------- Theory --------------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  gradm;       /* Length of (dmdu,dmdv) search direction  */
/*-----------------------------------------------------------------!*/

   DBfloat  ellip;       /* For the analysis of the Hessian matrix  */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Hessian adjustment                                            */
/* ======================                                           */
/*                                                                  */
/* If .. ? .. adjustment of the search direction                    */
/*                                                                  */
/* Calculation of the Hessian determinant deterh                    */
/* and the new search direction (dmdu,dmdv).                        */
/*                                                                 !*/

   deterh = d2objdu2*d2objdv2 - d2objdudv*d2objdudv;


/* Multiplication with the inverse (with deterh ! )                 */
   dmdu =   d2objdv2*dobjdu  - d2objdudv*dobjdv;
   dmdv = - d2objdudv*dobjdu + d2objdu2*dobjdv;

   if( ABS(deterh) > c_denom  )
      {
      dmdu =  dmdu/deterh;
      dmdv =  dmdv/deterh;
      }

/*!                                                                 */
/* Analysis of the Hessian determinant deterh                       */
/* Let hstep= -0.8*gradm and goto _ellip for an elliptic point.     */
/* (Note that the length of (dmdu,dmdv) is to the minimum pt) .     */
/* Goto _saddl for a saddle point.                                  */
/*                                                                 !*/
/* For the analysation of an elliptic point                         */

   ellip   = d2objdu2*u_pat*u_pat + d2objdv2*v_pat*v_pat + 
                      2.0*d2objdudv*u_pat*v_pat;


   if ( ellip > 0.0 && deterh  > c_denom ) 
       {  /* Start elliptic point              */
       gradm= SQRT(dmdu*dmdu + dmdv*dmdv);
       hstep = -0.80*gradm;
       goto _ellip;
       }

   if ( deterh < -c_denom )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*sdirect Saddle pt deterh %f u_pat %8.4f v_pat %8.4f \n"
   ,   deterh  , u_pat, v_pat  );
  }
#endif
      goto _saddl;
      }


/*!                                                                 */
/* 3. Search direction equal to gradient                            */
/* =====================================                            */
/*                                                                  */
_saddl:
/*                                                                  */
/* Let (dmdu,dmdv)= (dobjdu,dobjdv).                                */
/*                                                                 !*/

   dmdu = dobjdu;
   dmdv = dobjdv;

/*!                                                                 */
/* 4. Normalise search direction vector                             */
/* =====================================                            */
/*                                                                  */
_ellip:
/*                                                                  */
/* Let (dmdu,dmdv)= (dmdu/gradm,dmdv/gradm)                         */
/*                                                                 !*/

   gradm= SQRT(dmdu*dmdu + dmdv*dmdv);

   if ( gradm > c_denom )
     {
     dmdu = dmdu/gradm;
     dmdv = dmdv/gradm;
     }



#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*sdirect  deterh %f \n"
   ,   deterh );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*sdirect deterh %f ellip %f dmdu %8.4f dmdv %8.4f \n"
   ,   deterh  , ellip , dmdu, dmdv    );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** stepmin  ******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the U,V minimum step length for hstep   */
/*                                                                  */
/*  Note that this internal function is DBfloat (and not short)!    */
/*                                                                  */
/*                                                                  */
               static  DBfloat stepmin ()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables: hstep_r3min, xyz and dmdu,dmdv    */

/* Out: The function value is the minimum step                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------------- Theory --------------------------------------*/
/*                                                                  */
/* Reference: Faux & Pratt p 107-108                                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  hstep_uvmin; /* Minimum U,V length for hstep            */
   DBfloat  tang_x;      /* Search direction tangent, X component   */
   DBfloat  tang_y;      /* Search direction tangent, Y component   */
   DBfloat  tang_z;      /* Search direction tangent, Z component   */
   DBfloat  tang_length; /* Search direction tangent length         */
 
/*-----------------------------------------------------------------!*/

   DBfloat  dmdu_n;      /* dmdu normalized U component             */
   DBfloat  dmdv_n;      /* dmdv normalized V component             */
   DBfloat  v_length;    /* Length of a vector                      */

/*------------end-of-declarations-----------------------------------*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin  hstep_r3min %18.15f\n" , hstep_r3min );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/* 0. Initializations                                               */

   hstep_uvmin =  12345.6789;
   dmdu_n      =  0.0;
   dmdv_n      =  0.0;
   tang_x      =  0.0;
   tang_y      =  0.0;
   tang_z      =  0.0;
   tang_length = -0.123456789;

/*!                                                                 */
/* 1. Tangent in the search direction                               */
/*                                                                  */
/*                                                                 !*/

   v_length= SQRT(dmdu*dmdu + dmdv*dmdv);

   if ( v_length  < c_denom ) 
      {
      hstep_uvmin = c_infi;       
      goto undef;
      }

   dmdu_n      = dmdu/v_length;
   dmdv_n      = dmdv/v_length;

   tang_x      = xyz.u_x*dmdu_n + xyz.v_x*dmdv_n;
   tang_y      = xyz.u_y*dmdu_n + xyz.v_y*dmdv_n;
   tang_z      = xyz.u_z*dmdu_n + xyz.v_z*dmdv_n;

/*!                                                                 */
/* 2. Minimum U,V step = Minimum R*3 step / Search tangent length   */
/*                                                                  */
/*                                                                 !*/

   tang_length = SQRT(tang_x*tang_x + tang_y*tang_y + tang_z*tang_z);

   if ( tang_length  < c_denom ) 
      {
      hstep_uvmin = c_infi;       
      goto undef;
      }

   hstep_uvmin = hstep_r3min/tang_length;

undef:    /* Surface or search direction is undefined               */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && hstep_uvmin > 100.0  )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin  hstep_r3min %18.15f\n" , hstep_r3min );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin   dmdu   %25.15f dmdv   %25.15f\n" , dmdu   , dmdv   );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin   dmdu_n %25.15f dmdv_n %25.15f\n" , dmdu_n , dmdv_n );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin dr/du                    %10.5f %10.5f %10.5f\n",
       xyz.u_x, xyz.u_y, xyz.u_z );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin dr/dv                    %10.5f %10.5f %10.5f\n",
       xyz.v_x, xyz.v_y, xyz.v_z );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin Search direction tangent %10.5f %10.5f %10.5f\n",
                                            tang_x,tang_y,tang_z );
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin  Search direction tangent lenght %25.15f \n" , tang_length );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 &&  hstep_uvmin > 100.0  )
  {
  fprintf(dbgfil(SURPAC),
  "sur902*stepmin  hstep %25.15f hstep_uvmin %25.15f\n" , 
                   hstep, hstep_uvmin );
  fflush(dbgfil(SURPAC));
  }
#endif

       return(hstep_uvmin);


} /* End of function */

/********************************************************************/

