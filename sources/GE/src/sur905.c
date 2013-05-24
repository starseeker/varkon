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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_cur_develop                   File: sur905.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function finds a developable ruling from a given point on   */
/*  curve to a point on the other curve.                            */
/*                                                                  */
/*  Input data are two curves and two parameter intervals for       */
/*  the search of the developable ruling. One interval shall        */
/*  have zero length, which identifies the given point on the       */
/*  given curve. The other interval must for the moment be          */
/*  within one segment of the curve.                                */
/*                                                                  */
/*  Input is also a pointer to a structure which defines the        */
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
/*  The methods available are Newton-Rhapson, ....                  */
/*                                                                  */
/*  Author                                                          */
/*                                                                  */
/*  Gunnar Liden                                                    */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1999-01-07   Originally written                                 */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_develop    SUR_RULED: Find developable ruler*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short fdevrul();       /* Fctn f_cur and dfdt for DEVELRUL   */
static short addsol();        /* Add a non-existent solution.       */
static short fanalys();       /* Analysis of DEVELRUL solutions     */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBVector r_p;              /* Point   from point def. curve  */
static DBVector t_p;              /* Tangent from point def. curve  */
static DBint   ctype;             /* Case of computation            */
static DBfloat c_zero;            /* The zero (0.0) end criterion   */
static DBint   method;            /* Method which shall be used     */
static DBfloat t_s,t_e;           /* Param. interv. for calcul.     */
static DBfloat tglobal;           /* Current  parameter value       */
static DBfloat f_cur;             /* Current (objective) fctn value */
static DBfloat f_pre;             /* Previous function value        */
static DBfloat dfdt;              /* Function derivative w.r.t t    */
static DBint   numsol;            /* Number of solutions (=*p_npts) */
static EVALC   xyz;               /* Coordinates and derivatives    */
                                  /* for a point on a curve         */
static EVALC   xyz_p;             /* For analysis of DEVELRUL solut.*/
static DBfloat ctol;              /* Coordinate end criterion       */
static DBfloat ntol;              /* Angle      end criterion       */
static DBfloat comptol;           /* Computer tolerance (accuracy)  */
static DBfloat idpoint;           /* Identical points criterion     */
static DBint   niter;             /* Number of iterations           */
static DBfloat u_a[50];           /* All U parameter values         */
static DBfloat f_a[50];           /* All objective function values  */
static DBfloat d_a[50];           /* All objective derivat. values  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109                * Curve evaluation routine                  */
/* varkon_ini_evalc     * Initialize EVALC                          */
/* varkon_erpush        * Error message to terminal                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_cur_develop     */
/* SU2983 = sur905 Illegal computation case=   for varkon_cur_deve  */
/* SU2973 = Internal function xxxxxx failed in varkon_cur_develop   */
/* SU2993 = Program error in varkon_cur_develop (sur905). Report !  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus  varkon_cur_develop  (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1,      /* Curve 1                           (ptr) */
   DBSeg   *p_seg1,      /* Coefficients for curve p_cur1     (ptr) */
   DBfloat  t_in1[2],    /* Parameter curve segment limit p_cur1    */
                         /* (0):   Start U    (1): End   U          */
   DBCurve *p_cur2,      /* Curve 2                           (ptr) */
   DBSeg   *p_seg2,      /* Coefficients for curve p_cur2     (ptr) */
   DBfloat  t_in2[2],    /* Parameter curve segment limit p_cur2    */
                         /* (0):   Start U    (1): End   U          */
   IRUNON  *p_comp,      /* Pointer to computation data             */
/* Out:                                                             */
   DBint   *p_npts,      /* Number of output points           (ptr) */
   EVALC    xyz_a[SMAX], /* Array with all solutions                */
   DBint    d_code[SMAX])/* Developability analysis code            */
                         /* Eq.  1: Developable in both directions  */
                         /* Eq.  2: Developable in +U, cone in -U   */
                         /* Eq.  3: Developable in -U, cone in +U   */
                         /* Eq.  4: Developable but cones in +/-U   */
                         /* Eq.  0: No developable ruling d_code[0] */
                         /* Eq. -1: Failure to classify developable */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve *p_cur;       /* Curve for the numerical solution  (ptr) */
   DBSeg   *p_seg;       /* Coefficients for curve p_cur      (ptr) */
   DBCurve *p_curp;      /* Curve defining r_p and t_p        (ptr) */
   DBSeg   *p_segp;      /* Coefficients for curve p_curp     (ptr) */
   DBint    nstart;      /* The number of restarts                  */
   DBint    maxiter;     /* Maximum number of iterations            */
   DBfloat  deltat;      /* The parameter t step for the restarts   */
   DBint    i_r;         /* Loop index for the restarts             */
   DBint    j_n;         /* Loop index for the numerical solution   */
   DBfloat  h;           /* Step determined by Newton Rhapson       */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */
#ifdef  DEBUG
   DBint    ix1;         /* Temporarely used loop index             */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 Enter t_in1  %f %f  t_in2 %f %f\n", 
   t_in1[0],t_in1[1],  t_in2[0],t_in2[1]);
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
    status=initial(t_in1,t_in2,p_comp,p_npts,xyz_a,d_code);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%sur905");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* Case of computation, method and number of restarts               */
   ctype   = p_comp->dev_un.ctype;
   method  = p_comp->dev_un.method;
   nstart  = p_comp->dev_un.nstart;
   maxiter = p_comp->dev_un.maxiter;
/*                                                                 !*/

/*!                                                                 */
/* Retrieve computation data for the input computation case         */
/* and definition of the end criterion for c_zero                   */
/*                                                                 !*/

   if      ( ctype  == DEVELRUL )
       {
       ctol    = p_comp->dev_un.ctol;
       ntol    = p_comp->dev_un.ntol;
       comptol = p_comp->dev_un.comptol;
       idpoint = p_comp->dev_un.idpoint;
/*    End criterion for the numerical solution                       */
       c_zero  = ntol;
       c_zero  = 10.0;    /*  Temporary   */
       }
   else
       {
       sprintf(errbuf,"(ctype )%%sur905");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/* Determine which curve it is that shall be used in computation    */
/* Parameter interval t_s and t_e for the numerical solution        */
/*                                                                 !*/
   if      ( fabs(t_in1[1]-t_in1[0]) < comptol )
     { 
     p_cur   = p_cur2;
     p_seg   = p_seg2;
     p_curp  = p_cur1;
     p_segp  = p_seg1;
     if ( t_in2[0] < t_in2[1] )
       {
       t_s = t_in2[0];
       t_e = t_in2[1];
       }
     else
       {
       t_s = t_in2[1];
       t_e = t_in2[0];
       }
       xyz.t_global =  t_in1[0]; 
     }
   else if ( fabs(t_in2[1]-t_in2[0]) < comptol )
     { 
     p_cur   = p_cur1;
     p_seg   = p_seg1;
     p_curp  = p_cur2;
     p_segp  = p_seg2;
     if ( t_in1[0] < t_in1[1] )
       {
       t_s = t_in1[0];
       t_e = t_in1[1];
       }
     else
       {
       t_s = t_in1[1];
       t_e = t_in1[0];
       }
       xyz.t_global =  t_in2[0]; 
     }
   else
     {
     sprintf(errbuf,"No pt defined%%sur905");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* Point and tangent from curve with zero interval.                 */
/* Call of varkon_GE109 (GE109).                                  */
/*                                                                 !*/
   xyz.evltyp        = EVC_D2R; 

   status=GE109 ((DBAny *)p_curp, p_segp, &xyz );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur905");
     return(varkon_erpush("SU2943",errbuf));
    }

   xyz_p = xyz;
   r_p   = xyz_p.r;
   t_p   = xyz_p.drdt;


/*!                                                                 */
/* The parameter step for the restarts                              */
   deltat  = (t_e-t_s-comptol)/((DBfloat)nstart-1.0);
/* The number of output points                                      */
   numsol  = 0;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 Master crv pt %6.4f Slave crv interv. t_s %6.4f t_e %6.4f\n",
   xyz_p.t_global, t_s,t_e);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 t_s %f t_e %f deltat %f nstart= %d ==> End start %f\n",
   t_s,t_e,deltat,(int)nstart,t_s+((DBfloat)nstart-1.0)*deltat);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 t_e - (nstart-1)*deltat %25.15f\n",
   t_e - t_s-((DBfloat)nstart-1.0)*deltat);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 r_p %f %f %f\n", r_p.x_gm, r_p.y_gm, r_p.z_gm );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 t_p %f %f %f\n", t_p.x_gm, t_p.y_gm, t_p.z_gm );
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
/*     Call of varkon_GE109   (GE109)                             */
/*                                                                 !*/
/*     (Error SU2943 if called function fails)                      */
/*     One additional iteration                                     */
       niter = niter + 1;

       xyz.t_global =  tglobal; 

       status=GE109 ((DBAny *)p_cur, p_seg, &xyz );
     if (status<0) 
       {
       sprintf(errbuf,"GE109 (loop)%%sur905");
       return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 r_x %f r_y %f r_z %f tglobal %9.6f\n",
      xyz.r.x_gm,   xyz.r.y_gm,   xyz.r.z_gm, tglobal);
  }
#endif

/*!                                                                 */
/*     Calculation of function f_cur and derivative dfdt.           */
/*     Call of fdevrul for ctype = DEVELSUR                         */
/*     (Error SU2973 if internal function fails)                    */
/*                                                                 !*/

       f_pre       = f_cur;  /* Tillfalligt !!! */

       if      ( ctype  == DEVELRUL )
         {
         status=fdevrul();
         if (status<0) 
            {
            sprintf(errbuf,"fdevrul%%sur905");
            return(varkon_erpush("SU2973",errbuf));
            }
         }
#ifdef DEBUG  
       else
         {
         sprintf(errbuf,"(ctype 2)%%sur905");
         return(varkon_erpush("SU2993",errbuf));
         }
#endif         






/*                                                                  */
/*     Calculated data to u_a, f_a and d_a.                         */
/*                                                                  */
       if ( niter >= 50)
          {
          sprintf(errbuf,"(niter>50)%%sur905");
          return(varkon_erpush("SU2993",errbuf));
          }

       u_a[niter-1]= tglobal;
       f_a[niter-1]= f_cur;
       d_a[niter-1]= dfdt;

/*!                                                                 */
/*     Is function value f_cur <= c_zero ?                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 U %f f_cur %f c_zero %f dfdt %f\n"
   , tglobal, f_cur, c_zero, dfdt );
  }

if ( dbglev(SURPAC) == 2 && fabs(f_cur) < 10*c_zero )
{
for (ix1 = 1; ix1 <= niter; ix1 = ix1+1)
{
if ( fabs(f_a[ix1-1]) < 10000*c_zero )
{
  fprintf(dbgfil(SURPAC),
  "sur905 iteration %d U %25.20f f_cur %f dfdt %f\n"
   , (int)ix1  , u_a[ix1-1], f_a[ix1-1],d_a[ix1-1]);
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
/*  Att ta hand om !!!! oblem to have the solution as a u val */
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
  "sur905 fabs(tglobal-t_s)= %f 0.01*TOL4 %f h %f j_n %d\n",
      fabs(tglobal-t_s),0.01*TOL4 , h , (int)j_n );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 fabs(tglobal-t_e)= %f 0.01*TOL4 %f h %f j_n %d\n",
      fabs(tglobal-t_e),0.01*TOL4 , h , (int)j_n );
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
  "sur905 tglobal= %f   i_r= %d  j_n= %d\n",
      tglobal, (int)i_r, (int)j_n );
  }
#endif
        if ( j_n > maxiter )
            {
            sprintf(errbuf,"(maxiter)%% varkon_cur_develop (sur905)");
            return(erpush("SU2993",errbuf));
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

/*!                                                                 */
/* Analayse developable ruling. Call internal fctn fanalys.         */
/*                                                                 !*/

       if      ( ctype  == DEVELRUL )
         {
         status=fanalys(xyz_a,d_code);
         if (status<0) 
            {
            sprintf(errbuf,"fanalys%%sur905");
            return(varkon_erpush("SU2973",errbuf));
            }
         }
#ifdef DEBUG  
       else
         {
         sprintf(errbuf,"(ctype 3)%%sur905");
         return(varkon_erpush("SU2993",errbuf));
         }
#endif         




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if (*p_npts> 0 )
{
for (ix1 = 1; ix1 <= *p_npts; ix1 = ix1+1)
{
fprintf(dbgfil(SURPAC),
"sur905 Point %d U %9.6f X %9.2f Y %9.2f Z %9.2f Code %d\n",
      (int)ix1 , xyz_a[ix1-1].t_global  ,
    xyz_a[ix1-1].r.x_gm,xyz_a[ix1-1].r.y_gm,xyz_a[ix1-1].r.z_gm,
     (int)d_code[ix1-1]);
}
}
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && 0 == *p_npts )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 No solution! Master pt %6.4f Slave interv. %6.4f %6.4f\n",
   xyz_p.t_global, t_s,t_e);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905 Exit *p_npts= %d\n", (int)*p_npts );
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

   static short initial(t_in1,t_in2,p_comp,p_npts,xyz_a,d_code)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  t_in1[2];    /* Parameter curve segment limits          */
                         /* (0):   Start U    (1): End   U          */
   DBfloat  t_in2[2];    /* Parameter curve segment limits          */
                         /* (0):   Start U    (1): End   U          */
   IRUNON  *p_comp;      /* Pointer to computation data             */
   int     *p_npts;      /* Number of output points (a pointer)     */
   EVALC    xyz_a[];     /* Array with all solutions                */
   DBint    d_code[];    /* Developability analysis code            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  i_sol;         /* Loop index solution                     */
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
  "sur905 Enter varkon_cur_develop*initial ******\n");
  fprintf(dbgfil(SURPAC),
  "sur905 Start t_in1(0) %f End t_in1(1) %f\n",
   t_in1[0],t_in1[1]);
  fprintf(dbgfil(SURPAC),
  "sur905 Start t_in2(0) %f End t_in2(1) %f\n",
   t_in2[0],t_in2[1]);
  fprintf(dbgfil(SURPAC),
  "sur905 Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      (int)p_comp->dev_un.ctype,
      (int)p_comp->dev_un.method, 
      (int)p_comp->dev_un.nstart, 
      (int)p_comp->dev_un.maxiter);
  if ( p_comp->dev_un.ctype == DEVELRUL)
      {
      fprintf(dbgfil(SURPAC),
      "sur905 Case DEVELRUL:  \n");
      }
  }

 fflush(dbgfil(SURPAC));

/*! The number of restarts must be between 2 and maxiter            */
/*  Error SU2993 if not OK.                                        !*/
    if (p_comp->dev_un.nstart > 1 || 
        p_comp->dev_un.nstart <= p_comp->dev_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%%sur905*initial");
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 2. Initialize output variables and static (internal) variables   */
/*                                                                 !*/
    *p_npts     = I_UNDEF;      

    r_p.x_gm    =   F_UNDEF;
    r_p.y_gm    =   F_UNDEF;
    r_p.z_gm    =   F_UNDEF;
    t_p.x_gm    =   F_UNDEF;
    t_p.y_gm    =   F_UNDEF;
    t_p.z_gm    =   F_UNDEF;

/* Initialize variabel EVALC and d_code                             */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

   for (i_sol = 1; i_sol <= SMAX;  ++i_sol)
     {
     varkon_ini_evalc (&xyz_a[i_sol-1]); 
     d_code[i_sol-1] = I_UNDEF;
     }

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
/*  for calculation case:  DEVELSUR                                 */
/*                                                                  */
               static  short   fdevrul()
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_cur_eval              */
/*     Point r_p and tangent t_p.                                   */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  dist_che;    /* Distance between curve points           */
 
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
/*!--------------- Theory ------------------------------------------*/
/*                                                                  */
/*  The function calculates a the objective function value for the  */
/*  creation of a ruled, developable surface                        */
/*                                                                  */
/*  Notations                                                       */
/*  r_p         = Input point (start of ruling from master curve)   */
/*  t_p         = Input tangent (from master curve for r_p)         */
/*  u           = Parameter for the slave curve                     */
/*  r_u         = Point on slave curve for a given u                */
/*  drdu        = First  derivative on slave curve                  */
/*  d2rdu2      = Second derivative on slave curve                  */
/*  f_obj       = Objective function                                */
/*  dfdu_obj    = Derivative of objective function w.r.t. u         */
/*                                                                  */
/*  The criterion for a developable ruling is that the              */
/*  surface normal direction is the same for all points             */
/*  along the ruling.                                               */
/*                                                                  */
/*  The ruling direction (vector) is (r_u - r_p) and the            */
/*  tangent drdu gives the surface normal direction in              */
/*  point r_u as the cross product dr_du X (r_u - r_p).             */
/*                                                                  */
/*  Equal directions for surface normals in r_p and r_u             */
/*  means that the tangent t_p in r_p must be perpendicular         */
/*  to the surface normal in r_u, i.e. must be in the               */
/*  tangent plane of r_u.                                           */
/*                                                                  */
/*  The scalar product of the direction of the surface normal       */
/*  (the cross product vector) and the tangent t_p is the           */
/*  objective function. The scalar product is zero when             */
/*  the vectors are perpendicular. Note that the objective          */
/*  function will be positive and negative around zero, i.e.        */
/*  it is not the minimum point we are trying to find.              */
/*                                                                  */
/*   Surface normal (cross vector product) in r_u:                  */
/*    _                                    _                        */
/*   !      e_x          e_y        e_z     !                       */
/*   !    drdu.x       drdu.y     drdu.z    ! =                     */
/*   ! r_u.x-r_p.x  r_u.y-r_p.y r_u.z-r_p.z !                       */
/*   !_                                    _!                       */
/*                                                                  */
/*   drdu.y*(r_u.z-r_p.z) - drdu.z*(r_u.y-r_p.y)*e_x                */
/*   drdu.z*(r_u.x-r_p.x) - drdu.x*(r_u.z-r_p.z)*e_y                */
/*   drdu.x*(r_u.y-r_p.y) - drdu.y*(r_u.x-r_p.x)*e_z                */
/*                                                                  */
/*  The objective function is the scalar product between            */
/*  the surface normal in r_u and the tangent in r_p:               */
/*  f_obj       =   r_u_surface_normal(u)*t_p =                     */
/*   (drdu.y*(r_u.z-r_p.z)-drdu.z*(r_u.y-r_p.y))*t_p.x +            */
/*   (drdu.z*(r_u.x-r_p.x)-drdu.x*(r_u.z-r_p.z))*t_p.y +            */
/*   (drdu.x*(r_u.y-r_p.y)-drdu.y*(r_u.x-r_p.x))*t_p.z ;            */
/*                                                                  */
/*  The derivative with respect to parameter u of the slave         */
/*  curve                                                           */
/*  dfdu_obj   =  d r_u_surface_normal/du * t_p =                   */
/*   (d2rdu2.y*(r_u.z-r_p.z)  - d2rdu2.z*(r_u.y-r_p.y)+             */
/*    drdu.y  *(drdu.z-0.0  ) - drdu.z  *(drdu.y-0.0 ))*t_p.x +     */
/*   (d2rdu2.z*(r_u.x-r_p.x)  - d2rdu2.x*(r_u.z-r_p.z)+             */
/*    drdu.z  *(drdu.x-0.0  ) - drdu.x  *(drdu.z-0.0 ))*t_p.y +     */
/*   (d2rdu2.x*(r_u.y-r_p.y)  - d2rdu2.y*(r_u.x-r_p.x)+             */
/*    drdu.x  *(drdu.y-0.0  ) - drdu.y  *(drdu.x-0.0 ))*t_p.z ;     */
/*                                                                  */
/*  dfdu_obj   =  d r_u_surface_normal/du * t_p =                   */
/*   (d2rdu2.y*(r_u.z-r_p.z)  - d2rdu2.z*(r_u.y-r_p.y)+             */
/*    drdu.y  *   drdu.z      - drdu.z  *   drdu.y     ) *t_p.x +   */
/*   (d2rdu2.z*(r_u.x-r_p.x)  - d2rdu2.x*(r_u.z-r_p.z)+             */
/*    drdu.z  *   drdu.x      - drdu.x  *   drdu.z     ) *t_p.y +   */
/*   (d2rdu2.x*(r_u.y-r_p.y)  - d2rdu2.y*(r_u.x-r_p.x)+             */
/*    drdu.x  *   drdu.y      - drdu.y  *   drdu.x     ) *t_p.z ;   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/* Check distance between curves (curve points)                     */
/* Replace with better error message for the user  TODO             */
  dist_che = sqrt((xyz.r.x_gm-r_p.x_gm)*(xyz.r.x_gm-r_p.x_gm)+
                  (xyz.r.x_gm-r_p.x_gm)*(xyz.r.x_gm-r_p.x_gm)+
                  (xyz.r.x_gm-r_p.x_gm)*(xyz.r.x_gm-r_p.x_gm));
  if ( dist_che < 10.0*idpoint)
       {
       sprintf(errbuf,"(equal curves)%%sur905*fdevrul");
       return(varkon_erpush("SU2993",errbuf));
       }



  f_cur        = 
   (xyz.drdt.y_gm*(xyz.r.z_gm-r_p.z_gm)-
    xyz.drdt.z_gm*(xyz.r.y_gm-r_p.y_gm))*t_p.x_gm +
   (xyz.drdt.z_gm*(xyz.r.x_gm-r_p.x_gm)-
    xyz.drdt.x_gm*(xyz.r.z_gm-r_p.z_gm))*t_p.y_gm +
   (xyz.drdt.x_gm*(xyz.r.y_gm-r_p.y_gm)-
    xyz.drdt.y_gm*(xyz.r.x_gm-r_p.x_gm))*t_p.z_gm ;

  dfdt        = 
   (xyz.d2rdt2.y_gm*(xyz.r.z_gm-r_p.z_gm)  - 
    xyz.d2rdt2.z_gm*(xyz.r.y_gm-r_p.y_gm)+
    xyz.drdt.y_gm  *   xyz.drdt.z_gm      - 
    xyz.drdt.z_gm  *   xyz.drdt.y_gm     ) *t_p.x_gm +
   (xyz.d2rdt2.z_gm*(xyz.r.x_gm-r_p.x_gm)  - 
    xyz.d2rdt2.x_gm*(xyz.r.z_gm-r_p.z_gm)+
    xyz.drdt.z_gm  *   xyz.drdt.x_gm      - 
    xyz.drdt.x_gm  *   xyz.drdt.z_gm     ) *t_p.y_gm +
   (xyz.d2rdt2.x_gm*(xyz.r.y_gm-r_p.y_gm)  - 
    xyz.d2rdt2.y_gm*(xyz.r.x_gm-r_p.x_gm)+
    xyz.drdt.x_gm  *   xyz.drdt.y_gm      - 
    xyz.drdt.y_gm  *   xyz.drdt.x_gm     ) *t_p.z_gm ;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*fdevrul tglobal= %f f_cur= %f dfdt= %f\n"
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
   DBint    i_sol;       /* Loop index for previous solutions       */
 
/*-----------------------------------------------------------------!*/

   DBfloat  dist;        /* Distance between solution points (R*3)  */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*addsol ctol %f numsol %d \n"
   ,   ctol , (int)numsol );
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
if ( dbglev(SURPAC) == 2 &&  dist > ctol )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*addsol u %10.5f v %10.5f dist %f ctol %f \n",
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
       sprintf(errbuf,"(SMAX)%% varkon_cur_develop (sur905)");
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


/*!************** Internal ** function ******************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Analysis of DEVELRUL solutions                                  */
/*                                                                  */
               static  short  fanalys(xyz_a, d_code)
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALC    xyz_a[SMAX]; /* Array with all solutions                */
/*     Global (static) variables:                                   */
/*     xyz_p                                                        */
/* Out:                                                             */
   DBint    d_code[SMAX];/* Developability analysis code            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_sol;       /* Loop index solution                     */
#ifdef  DEBUG
   DBfloat  f_cur_sol;   /* Function value f_cur for output point   */
   DBfloat  dfdt_sol;    /* Derivative of  f_cur for output point   */
   DBfloat  dfdt_switch; /* Derivative when curves are switched     */
#endif
   DBfloat  eps;         /* For approximation of r_p(t+eps) point   */
   DBfloat  f_cur_p;     /* Function value f_cur for + eps          */
   DBfloat  dfdt_p;      /* Derivative of  f_cur for + eps          */
   DBfloat  f_cur_n;     /* Function value f_cur for - eps          */
   DBfloat  dfdt_n;      /* Derivative of  f_cur for - eps          */
 
/*-----------------------------------------------------------------!*/

   short  status;        /* Error code from a called function       */
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
  "sur905*fanalys numsol %d \n"
   ,   (int)numsol );
  }
#endif

/*!                                                                 */
/* 1. Return if there are no solutions                              */
/* ___________________________________                              */
/*                                                                 !*/

    if ( 0 == numsol ) 
      {
      d_code[0] = 0;
      return(SUCCED);
      }

/* Initialization of internal variables                             */

#ifdef  DEBUG
   f_cur_sol    =  F_UNDEF;
   dfdt_sol     =  F_UNDEF;
   dfdt_switch  =  F_UNDEF;
   eps          =  F_UNDEF;
   f_cur_p      =  F_UNDEF;
   dfdt_p       =  F_UNDEF;
   f_cur_n      =  F_UNDEF;
   dfdt_n       =  F_UNDEF;
#endif

/*!                                                                 */
/* 2. Analysis w.r.t. "interchanged" curves                         */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/*  Start loop solutions i_sol= 1,2, .. ,numsol.                    */
/*                                                                 !*/

   for (i_sol = 1; i_sol <= numsol ; i_sol = i_sol+1)
     {

/*!                                                                 */
/*    Recalculate function and derivative for + eps.                */
/*    Call of internal function fdevrul                             */
/*                                                                 !*/

      eps = 0.00001;
      r_p = xyz_p.r;
      t_p = xyz_p.drdt;
      r_p.x_gm=xyz_p.r.x_gm+
           eps*xyz_p.drdt.x_gm+eps*eps/2*xyz_p.d2rdt2.x_gm;
      r_p.y_gm=xyz_p.r.y_gm+eps*xyz_p.drdt.y_gm+
           eps*eps/2*xyz_p.d2rdt2.y_gm;
      r_p.z_gm=xyz_p.r.z_gm+eps*xyz_p.drdt.z_gm+
           eps*eps/2*xyz_p.d2rdt2.z_gm;
      t_p.x_gm=xyz_p.drdt.x_gm+eps/2*xyz_p.d2rdt2.x_gm;
      t_p.y_gm=xyz_p.drdt.y_gm+eps/2*xyz_p.d2rdt2.y_gm;
      t_p.z_gm=xyz_p.drdt.z_gm+eps/2*xyz_p.d2rdt2.z_gm;
      xyz = xyz_a[i_sol-1];
      status=fdevrul();
      if (status<0) 
         {
         sprintf(errbuf,"fdevrul (1)%%sur905*fanalys");
         return(varkon_erpush("SU2973",errbuf));
         }

      f_cur_p   = f_cur;
      dfdt_p    = dfdt;
/*!                                                                 */
/*    Recalculate function and derivative for - eps.                */
/*    Call of internal function fdevrul                             */
/*                                                                 !*/

      eps = - 0.00001;
      r_p = xyz_p.r;
      t_p = xyz_p.drdt;
      r_p.x_gm=xyz_p.r.x_gm+
           eps*xyz_p.drdt.x_gm+eps*eps/2*xyz_p.d2rdt2.x_gm;
      r_p.y_gm=xyz_p.r.y_gm+eps*xyz_p.drdt.y_gm+
           eps*eps/2*xyz_p.d2rdt2.y_gm;
      r_p.z_gm=xyz_p.r.z_gm+eps*xyz_p.drdt.z_gm+
           eps*eps/2*xyz_p.d2rdt2.z_gm;
      t_p.x_gm=xyz_p.drdt.x_gm+eps/2*xyz_p.d2rdt2.x_gm;
      t_p.y_gm=xyz_p.drdt.y_gm+eps/2*xyz_p.d2rdt2.y_gm;
      t_p.z_gm=xyz_p.drdt.z_gm+eps/2*xyz_p.d2rdt2.z_gm;
      xyz = xyz_a[i_sol-1];
      status=fdevrul();
      if (status<0) 
         {
         sprintf(errbuf,"fdevrul (1)%%sur905*fanalys");
         return(varkon_erpush("SU2973",errbuf));
         }


      f_cur_n   = f_cur;
      dfdt_n    = dfdt;


/*!                                                                 */
/*    Set the output code for the developable ruling:               */
/*    Eq. 1: Developable, normal surface for in both directions     */
/*    Eq. 2: Developable, normal surface in positive direction only */
/*    Eq. 3: Developable, normal surface in negative direction only */
/*                                                                 !*/

      d_code[i_sol-1] = I_UNDEF;
      if       (  f_cur_p >  0.0 && dfdt_p <  0.0 &&
                  f_cur_n >  0.0 && dfdt_n >  0.0    )
            d_code[i_sol-1] = 11;
      else if  (  f_cur_p <  0.0 && dfdt_p >  0.0 &&
                  f_cur_n <  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 12;
      else if  (  f_cur_p >  0.0 && dfdt_p <  0.0 &&
                  f_cur_n <  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 13;
      else if  (  f_cur_p <  0.0 && dfdt_p >  0.0 &&
                  f_cur_n >  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 14;

      else if  (  f_cur_p <  0.0 && dfdt_p >  0.0 &&
                  f_cur_n >  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 21;
      else if  (  f_cur_p <  0.0 && dfdt_p >  0.0 &&
                  f_cur_n <  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 22;
      else if  (  f_cur_p >  0.0 && dfdt_p <  0.0 &&
                  f_cur_n <  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 23;
      else if  (  f_cur_p >  0.0 && dfdt_p <  0.0 &&
                  f_cur_n >  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 24;

      else if  (  f_cur_p <  0.0 && dfdt_p <  0.0 &&
                  f_cur_n >  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 31;
      else if  (  f_cur_p >  0.0 && dfdt_p >  0.0 &&
                  f_cur_n <  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 32;
      else if  (  f_cur_p <  0.0 && dfdt_p <  0.0 &&
                  f_cur_n <  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 33;
      else if  (  f_cur_p >  0.0 && dfdt_p >  0.0 &&
                  f_cur_n >  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 34;



      else if  (  f_cur_p <  0.0 && dfdt_p <  0.0 &&
                  f_cur_n >  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 41;
      else if  (  f_cur_p >  0.0 && dfdt_p >  0.0 &&
                  f_cur_n <  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 42;
      else if  (  f_cur_p <  0.0 && dfdt_p <  0.0 &&
                  f_cur_n <  0.0 && dfdt_n >  0.0    ) 
            d_code[i_sol-1] = 43;
      else if  (  f_cur_p >  0.0 && dfdt_p >  0.0 &&
                  f_cur_n >  0.0 && dfdt_n <  0.0    ) 
            d_code[i_sol-1] = 44;


      else
         {
            d_code[i_sol-1] = -1;
#ifdef  TILLSVIDARE
         sprintf(errbuf,"d_code%%sur905*fanalys");
         return(varkon_erpush("SU2993",errbuf));
#endif /*  TILLSVIDARE */
         }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*fanalys f_cur_p %12.2f dfdt_p %12.2f d_code %d\n",
   f_cur_p, dfdt_p,(int)d_code[i_sol-1]);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*fanalys f_cur_n %12.2f dfdt_n %12.2f \n",
   f_cur_n, dfdt_n);
  }
#endif

  if       ( d_code[i_sol-1] >= 11 && d_code[i_sol-1] <= 14 ) 
       d_code[i_sol-1] = 1;
  else if  ( d_code[i_sol-1] >= 21 && d_code[i_sol-1] <= 24 ) 
       d_code[i_sol-1] = 2;
  else if  ( d_code[i_sol-1] >= 31 && d_code[i_sol-1] <= 34 ) 
       d_code[i_sol-1] = 3;
  else if  ( d_code[i_sol-1] >= 41 && d_code[i_sol-1] <= 44 ) 
       d_code[i_sol-1] = 4;

#ifdef  KANSKE_FUNGERAR
/*    Derivative of f_cur for output point                          */
      dfdt_sol  = dfdt;
      f_cur_sol = f_cur;

/*!                                                                 */
/*    Let the solution point be r_p and t_p and xyz_p be the        */
/*    xyz derivatives for function fdevrul, which is called.        */
/*                                                                 !*/
      
      r_p = xyz_a[0].r;
      t_p = xyz_a[0].drdt;

      xyz = xyz_p;
      status=fdevrul();
      if (status<0) 
         {
         sprintf(errbuf,"fdevrul (2)%%sur905*fanalys");
         return(varkon_erpush("SU2973",errbuf));
         }

/*    Derivative when curves are switched                           */
      dfdt_switch = dfdt;

/*                                                                  */
/*    Set the output code for the developable ruling:               */
/*    Eq. 1: Derivatives both positive  <==> Normal developable     */
/*    Eq. 2: Derivatives both negative  <==> Normal developable     */
/*    Eq. 3: Derivative crv 1 positive  <==> Normal developable     */
/*    Eq. 4: Derivative crv 2 positive  <==> Normal developable     */
/*                                                                  */

      d_code[i_sol-1] = F_UNDEF;
      if       ( dfdt_sol >= 0.0 && dfdt_switch >= 0.0 ) 
            d_code[i_sol-1] = 1;
      else if  ( dfdt_sol <  0.0 && dfdt_switch <  0.0 ) 
            d_code[i_sol-1] = 2;
      else if  ( dfdt_sol >= 0.0 && dfdt_switch <  0.0 ) 
            d_code[i_sol-1] = 3;
      else if  ( dfdt_sol <  0.0 && dfdt_switch >= 0.0 ) 
            d_code[i_sol-1] = 4;
      else 
            d_code[i_sol-1] = -1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*fanalys u %10.5f dfdt_sol %f dfdt_switch %f d_code %d\n",
   xyz_a[i_sol-1].t_global,dfdt_sol,dfdt_switch,(int)d_code[i_sol-1]);
  }
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur905*fanalys u %10.5f f_cur_sol %f f_cur_switch %f\n",
   xyz_a[i_sol-1].t_global,f_cur_sol,f_cur);
  }
#endif
#endif /*  KANSKE_FUNGERAR  */


     }  /* End loop i_sol= 1,2,..., numsol                          */



    return(SUCCED);


} /* End of function */

/*!****************************************************************!*/




