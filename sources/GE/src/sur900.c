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
/*  Function: varkon_sur_num1                      File: sur900.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of points on an iso-parametric curve on a surface.              */
/*                                                                  */
/*  Input is a pointer to a structure which defines the             */
/*  calculation criterion for the points (the objective             */
/*  function), the method for the numerical calculation,            */
/*  the number of restarts, the tolerances, etc.                    */
/*                                                                  */
/*  The methods available are Newton-Rhapson, parabola inter-       */
/*  polation, ....                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1996-02-23   icase= 4 for I_SILHOU and F_SILHOU, unused variabl */
/*  1996-04-29   analyze added                                      */
/*  1996-05-02   analyze trimming for silhouettes                   */
/*  1998-09-26   analyze Tolerance idpoint for intersects,          */
/*               TOL2->idpoint, ana2pts added                       */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_num1       Surface one param. calculation   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef DEBUG
static short initial();       /* Initialization of variables        */
#endif
static short fsilhou();       /* Function f_c and dfdu for F_SILH   */
static short isilhou();       /* Function f_c and dfdu for I_SILH   */
static short ssilhou();       /* Function f_c and dfdu for S_SILH   */
static short intplan();       /* Function f_c and dfdu for SURPLAN  */
static short addsol();        /* Add a non-existent solution.       */
static short analyze();       /* Analyze boundary                   */
static short ana2pts();       /* Additional comparison of solutions */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef NOT_YET_IMPLEMENTED
static short newton1();       /* Newton Rhapson method              */
static short fisopho();       /* Function f_c and dfdu for F_ISOP   */
static short iisopho();       /* Function f_c and dfdu for I_ISOP   */
#endif /*  NOT_YET_IMPLEMENTED  */
#ifdef NOT_USED_FOR_THE_MOMENT
static DBfloat tglobal_pre;       /* Previous parameter value       */
static short parab2d();       /* Parabola interp.: 2 pts+derivative */
#endif /* NOT_USED_FOR_THE_MOMENT */

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   ctype;             /* Case of computation            */
static DBfloat c_zero;            /* The zero (0.0) end criterion   */
static DBfloat idpoint;           /* Identical points   criterion   */
static DBfloat i_zero;            /* Criterion for isoparameter     */
static DBint method;              /* Method which shall be used     */
static DBfloat t_s,t_e;           /* Param. interv. for calcul.     */
static DBfloat t_c;               /* Constant parameter value       */
static DBint iso_p;               /* Eq. 1: Const U  Eq. 2: Const V */
static DBfloat tglobal;           /* Current  parameter value       */
static DBfloat f_cur;             /* Current (objective) fctn value */
static DBfloat f_pre;             /* Previous function value        */
static DBfloat dfdt;              /* Function derivative w.r.t t    */
static DBint numsol;              /* Number of solutions (=*p_npts) */
static DBfloat ctol;              /* Coordinate tolerance           */
static DBfloat ntol;              /* Surface normal tolerance       */
static DBfloat comptol;           /* Computer tolerance             */
static DBfloat pi_x,pi_y,pi_z,pi_d;/* Intersecting plane            */
static DBfloat eye_x,eye_y,eye_z; /* Eye point for reflection crvs  */
static DBfloat eyev_x,eyev_y,eyev_z;/* Eye vect. for reflection crvs */
static DBVector sdir;             /* Direction for spine silhouette */
static DBfloat angle;             /* Angle in degrees for isophotes */
static EVALS xyz;                 /* Coordinates and derivatives    */
                                  /* for a point on a surface       */
/*-----------------------------------------------------------------!*/
static DBint  i_rg;               /* Temporary  !!!!                */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_comptol        * Retrieve computer tolerance    */
/*           varkon_sur_eval       * Surface evaluation routine     */
/*           varkon_vec_projpla    * Project vector onto plane      */
/*           varkon_ini_evals      * Initiate EVALS                 */
/*           varkon_erpush         * Error message to terminal      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_num1        */
/* SU2983 = sur900 Illegal computation case=   for varkon_sur_num1  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_num1      */
/* SU2993 = Severe program error in varkon_sur_num1 (sur900).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus     varkon_sur_num1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   DBfloat  t_in[2][2],  /* Iso-parameter curve limits              */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
/* Out:                                                             */
   DBint   *p_npts,      /* Number of output points           (ptr) */
   EVALS   xyz_a[SMAX],  /* Array with all solutions (R*3 and UV)   */
   DBint  *p_b_case )    /* Boundary point case:                    */
                         /* Eq. 0: No     points                    */
                         /* Eq. 1: Normal points                    */
                         /* Eq. 2: Curve coincides w. isoparameter  */
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
   DBfloat h;            /* Step determined by Newton !!! Temp. !!  */

   DBint   icase;        /* Calculation case for varkon_sur_eval    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   f_a;          /* Flag for analyzis                       */
                         /* Eq. 0: No point      (numsol= 0)        */
                         /* Eq. 1: One end point (numsol= 0)        */
                         /* Eq. 2: Two points    (numsol= 2)        */

   char    errbuf[80];   /* String for error message fctn erpush    */
   DBfloat dum1;         /* Dummy float                             */
   DBint   status;       /* Error code from a called function       */
#ifdef DEBUG
   DBint   ix1;          /* Temporarely used loop index             */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*Enter varkon_sur_num1: us %f vs %f ue %f ve %f\n", 
   t_in[0][0],t_in[0][1],t_in[1][0],t_in[1][1]);
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

    icase = I_UNDEF;

#ifdef DEBUG
    status=initial(p_sur,p_pat,t_in,p_comp,p_npts,xyz_a);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_num1 (sur900)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

   comptol   = varkon_comptol();

   *p_b_case = I_UNDEF;

/*!                                                                 */
/* Case of computation, method, idpoint and number of restarts      */
   ctype   = p_comp->ipl_un.ctype;
   method  = p_comp->ipl_un.method;
   nstart  = p_comp->ipl_un.nstart;
   maxiter = p_comp->ipl_un.maxiter;
/* 1998-09-26 TODO TOL2= 0.001 and idpoint= 0.01 in SURPAC.TOL      */
/* The default tolerance idpoint should be changed to 0.001         */
/* and /10.0 should be deleted.                                     */
   idpoint = p_comp->ipl_un.idpoint/10.0;
/* Determine with t_in if U (iso_p=1) or V (iso_p=2) is constant    */
/*                                                                 !*/

   if      ( fabs(t_in[0][0]-t_in[1][0]) < TOL4 ) 
       iso_p   = 1;     
   else if ( fabs(t_in[0][1]-t_in[1][1]) < TOL4 ) 
       iso_p   = 2;     
   else
       {
       sprintf(errbuf,"(non-isop)%% varkon_sur_num1 (sur900)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* Retrieve computation data for the input computation case         */
/* (for instance the intersection plane for ctype= SURPLAN)         */
/* and definition of the end criterion for c_zero                   */
/*                                                                 !*/

   if      ( ctype  == F_SILH  )
       {
       eye_x  = p_comp->sil_un.eye_x ;
       eye_y  = p_comp->sil_un.eye_y ;
       eye_z  = p_comp->sil_un.eye_z ;
       icase = 4;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       c_zero  = p_comp->sil_un.ntol;
       i_zero  = 50.0*c_zero; /* TODO use idpoint, but test first */
       }
   else if ( ctype  == I_SILH  )
       {
       eyev_x = p_comp->sil_un.eyev_x ;
       eyev_y = p_comp->sil_un.eyev_y ;
       eyev_z = p_comp->sil_un.eyev_z ;
       icase = 4;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       ntol = p_comp->sil_un.ntol;
       c_zero  = ntol;      
       i_zero  = 50.0*c_zero; /* TODO use idpoint, but test first */
       }
   else if ( ctype  == S_SILH  )
       {
       sdir.x_gm = p_comp->sil_un.eyev_x ;
       sdir.y_gm = p_comp->sil_un.eyev_y ;
       sdir.z_gm = p_comp->sil_un.eyev_z ;
       icase = 2;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       ntol = p_comp->sil_un.ntol;
       c_zero  = ntol;      
       i_zero  = 50.0*c_zero; /* TODO use idangle, but test first */
       }
   else if ( ctype  == F_ISOP  )
       {
       eye_x  = p_comp->iso_un.eye_x ;
       eye_y  = p_comp->iso_un.eye_y ;
       eye_z  = p_comp->iso_un.eye_z ;
       angle  = p_comp->iso_un.angle ;
       icase = 4;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       ntol = p_comp->sil_un.ntol;
       c_zero  = ntol;      
       i_zero  = 50.0*c_zero; /* TODO use idangle, but test first */
       }
   else if ( ctype  == I_ISOP  )
       {
       eye_x  = p_comp->iso_un.eye_x ;
       eye_y  = p_comp->iso_un.eye_y ;
       eye_z  = p_comp->iso_un.eye_z ;
       angle  = p_comp->iso_un.angle ;
       icase = 4;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       ntol = p_comp->sil_un.ntol;
       c_zero  = ntol;      
       i_zero  = 50.0*c_zero; /* TODO use idangle, but test first */
       }
   else if ( ctype  == SURPLAN )
       {
       pi_x = p_comp->ipl_un.in_x;
       pi_y = p_comp->ipl_un.in_y;
       pi_z = p_comp->ipl_un.in_z;
       pi_d = p_comp->ipl_un.in_d;
       if ( iso_p == 1 ) icase = 2;
       if ( iso_p == 2 ) icase = 1;
       ctol = p_comp->ipl_un.ctol;
/*    End criterion for the numerical solution ( !!! geo995 !!! )    */
       c_zero  = ctol/10.0;
       i_zero  = p_comp->ipl_un.idpoint;
       }
   else
       {
       sprintf(errbuf,"(ctype )%% varkon_sur_num1 (sur900)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/* Parameter interval t_s and t_e for the numerical solution        */
/*                                                                 !*/
   if ( iso_p == 1)
       {
       t_s = t_in[0][1];
       t_e = t_in[1][1];
       t_c = t_in[0][0];
       }
   else if ( iso_p == 2)
       {
       t_s = t_in[0][0];
       t_e = t_in[1][0];
       t_c = t_in[0][1];
       }
   if ( t_s >  t_e )
       {
       dum1 = t_s;
       t_s  = t_e;
       t_e  = dum1;
       }

/*!                                                                 */
/* The parameter step for the restarts                              */
   deltat  = (t_e-t_s-TOL4)/((DBfloat)nstart-1.0);
/* The number of output points                                      */
   numsol  = 0;
/*                                                                 !*/

/*!                                                                 */
/* Analyze if whole, or part of boundary is the solution            */
/* Call of internal function analyze.                               */
/*                                                                 !*/
   analyze (p_sur,p_pat,nstart,icase,xyz_a,&f_a);
   if ( f_a >=  1   ) goto boundsol;



/*!                                                                 */
/* 2. Numerical solution for a one parameter function               */
/* __________________________________________________               */
/*                                                                  */
/* Start loop: Restarts i_r=1,2, .. nstart                          */
/*                                                                 !*/

     i_rg = 0;      /* Temporary   !!!!! */

   for (i_r = 1; i_r <= nstart ; i_r = i_r+1)
     {
/*!                                                                 */
/*   New parameter start value                                      */
     tglobal = t_s + deltat*((DBfloat)i_r-1.0);
/*   Start loop: Numerical i_n=1,2, ..                              */
/*                                                                 !*/

     for(j_n = 1; j_n <= maxiter ; j_n = j_n+1)
       {
/*!                                                                 */
/*     Call of varkon_sur_eval (sur210) for all surface types       */
/*     (Error SU2943 if called function fails)                      */
/*                                                                 !*/
       i_rg = i_rg+1; /* Temporary   !!!!! */

       if (iso_p == 2)
       status= varkon_sur_eval
       (p_sur,p_pat,icase,tglobal,t_c,&xyz);
       else
       status= varkon_sur_eval
       (p_sur,p_pat,icase,t_c,tglobal,&xyz);
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_num1 (sur900)");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*     Calculation of function f_cur and derivative dfdu.           */
/*     Call of fsilhou for ctype = F_SILH                           */
/*     Call of isilhou for ctype = I_SILH                           */
/*     Call of ssilhou for ctype = S_SILH                           */
/*     Call of fisopho for ctype = F_ISOP                           */
/*     Call of iisopho for ctype = I_ISOP                           */
/*     Call of intplan for ctype = SURPLAN                          */
/*     (Error SU2973 if internal function fails)                    */
/*                                                                 !*/

       f_pre       = f_cur;  /* Temporary   !!! */

       if      ( ctype  == F_SILH  )
         status=fsilhou();
         if (status<0) 
            {
            sprintf(errbuf,"fsilhou%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
       else if ( ctype  == I_SILH  )
         status=isilhou();
         if (status<0) 
            {
            sprintf(errbuf,"isilhou%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
       else if ( ctype  == S_SILH  )
         status=ssilhou();
         if (status<0) 
            {
            sprintf(errbuf,"ssilhou%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
       else if ( ctype  == F_ISOP  )
         status=intplan();
         if (status<0) 
            {
            sprintf(errbuf,"fisopho%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
       else if ( ctype  == I_ISOP  )
         status=intplan();
         if (status<0) 
            {
            sprintf(errbuf,"iisopho%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
       else if ( ctype  == SURPLAN )
         status=intplan();
         if (status<0) 
            {
            sprintf(errbuf,"intplan%%varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2973",errbuf));
            }
/* !!!! ??????
       else
         {
         sprintf(errbuf,"(ctype 2)%% varkon_sur_num1 (sur900)");
         return(varkon_erpush("SU2993",errbuf));
         }
!!!! ????  */


/*!                                                                 */
/*     Is function value f_cur <= c_zero ?                          */
/*                                                                 !*/

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
/*  TODO         !!!! Problem to have the solution as a u val */
/*           Accepting +/- TOL4 will give the wrong segment   */
/*           adress                                           */

          if ( tglobal >= t_s  )       /* Parameter is greater or   */
            {                          /* equal interval start t_s  */
            if ( tglobal < t_e  )      /* Parameter value is less   */
              {                        /* than interval end t_e     */
/* 1998-09-26 addsol(p_sur,p_pat,xyz_a);*//* Add non-existent solution */
              /* Add non-existent solution */
              status = addsol(p_sur,p_pat,xyz_a);
              if ( status < 0 ) return(status);
              }                        /* End tglobal <  t_e        */
            }                          /* End tglobal >= t_s        */
          j_n = maxiter;               /* Refer to Note 2 above     */
          }                            /* End if f_cur <= c_zero    */

/*!                                                                 */
/*     Is derivative dfdu too small ?                               */
/*                                                                 !*/
          if(  fabs(dfdt) <= TOL3 )    /* Derivative <= TOL3 ?      */
            {                          /*                           */
/*!                                                                 */
/*     Yes, dfdu is less or equal TOL3:                             */
            j_n = maxiter;             /* ==> end of loop j_n       */
            h = TOL6;                  /* ??? Newton only ???       */
/*                                                                 !*/
            }                          /* End dfdu<= TOL3           */
          else                         /* Derivative >  TOL3        */
            {                          /*                           */
            h = -f_cur/dfdt;           /* Newton step ...           */
            }                          /* End if: dfdu <= TOL3      */



          /* Solution outside 0 < u < 1 ? */
          if (fabs(tglobal-t_s) < TOL4 )
              {
              if ( h < 0 ) j_n = maxiter;
              }

          if ( fabs(tglobal-t_e) < TOL4 )
              {
              if ( h > 0 ) j_n = maxiter;
              }
          if ( fabs(h) > 2.0 ) j_n = maxiter;

          /* Compute the next parameter value    */
          tglobal = tglobal + h;
          if ( tglobal < t_s - TOL4 ) tglobal = t_s + 0.1*TOL4;
          if ( tglobal > t_e + TOL4 ) tglobal = t_e - 0.1*TOL4;

#ifdef NOT_USED_FOR_THE_MOMENT
   status=parab2d(); 
   if (status<0) 
        {
        sprintf(errbuf,"parab2d%%varkon_sur_num1 (sur900)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif /* NOT_USED_FOR_THE_MOMENT */

        if ( j_n > maxiter )
            {
            sprintf(errbuf,"(maxiter)%% varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2993",errbuf));
            }

        }   /* end loop j_n : Numerical solution    */
   }       /* end loop i_r : Restarts with deltat  */

boundsol:;

/*!                                                                 */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */
/* Number of solutions to output variable                           */
   *p_npts = numsol; 
/*                                                                 !*/

   if      ( f_a == 2 )                *p_b_case = 2;
   else if ( f_a != 2 && *p_npts > 0 ) *p_b_case = 1;
   else                                *p_b_case = 0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if (*p_npts> 0 )
{
for (ix1 = 1; ix1 <= *p_npts; ix1 = ix1+1)
{
fprintf(dbgfil(SURPAC),
"sur900 Point %d U %9.6f V %9.6f X %9.2f Y %9.2f Z %9.2f\n",
      (int)ix1 , xyz_a[ix1-1].u  ,xyz_a[ix1-1].v  ,
            xyz_a[ix1-1].r_x,xyz_a[ix1-1].r_y,xyz_a[ix1-1].r_z);
}
}
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

#ifdef DEBUG

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to 1.23456789 and 123456789.           */

   static short initial(p_sur,p_pat,t_in,p_comp,p_npts,xyz_a)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   DBfloat  t_in[2][2];  /* Iso-parameter curve limits              */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBint   *p_npts;      /* Number of output points           (ptr) */
   EVALS   xyz_a[SMAX];  /* Array with all solutions (R*3 and UV)   */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint iso;            /* Eq. 1: Const U  Eq. 2: Const V          */
/*----------------------------------------------------------------- */

   EVALS xyz_d;          /* Temporary used variable                 */
   DBint i_p;            /* Loop index                              */
   char  errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_sur_num1 (sur900) ** initial ******\n");
  fprintf(dbgfil(SURPAC),
  "sur900 Start t_in(0,0-1) %f %f   End t_in(1,0-1) %f %f  \n",
   t_in[0][0],t_in[0][1],t_in[1][0],t_in[1][1]);
  fprintf(dbgfil(SURPAC),
  "sur900 Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      (int)p_comp->ipl_un.ctype,
      (int)p_comp->ipl_un.method, 
      (int)p_comp->ipl_un.nstart, 
      (int)p_comp->ipl_un.maxiter);
  if ( p_comp->ipl_un.ctype == I_SILH)
      {
      fprintf(dbgfil(SURPAC),
      "sur900 Case I_SILH:  Eye vector= %f %f %f \n",
        p_comp->sil_un.eyev_x,
        p_comp->sil_un.eyev_y,
        p_comp->sil_un.eyev_z);
      }
  if ( p_comp->ipl_un.ctype == S_SILH)
      {
      fprintf(dbgfil(SURPAC),
      "sur900 Case S_SILH:  U crv tangent %f %f %f\n",
        p_comp->sil_un.eyev_x,
        p_comp->sil_un.eyev_y,
        p_comp->sil_un.eyev_z);
      }
  if ( p_comp->ipl_un.ctype == F_SILH)
      {
      fprintf(dbgfil(SURPAC),
      "sur900 Case F_SILH:  Eye point=  %f %f %f \n",
        p_comp->sil_un.eye_x,
        p_comp->sil_un.eye_y,
        p_comp->sil_un.eye_z);
      }
  if ( p_comp->ipl_un.ctype == SURPLAN)
      {
      fprintf(dbgfil(SURPAC),
      "sur900 Case SURPLAN:  Intersect plane: %f %f %f %f\n",
        p_comp->ipl_un.in_x,
        p_comp->ipl_un.in_y,
        p_comp->ipl_un.in_z,
        p_comp->ipl_un.in_d);
      }
  }

 fflush(dbgfil(SURPAC)); /* To file from buffer      */

   if      ( fabs(t_in[0][0]-t_in[1][0]) < TOL4 ) 
       iso   = 1;     
   else if ( fabs(t_in[0][1]-t_in[1][1]) < TOL4 ) 
       iso   = 2;     
   else
       {
       sprintf(errbuf,"(non-isop)%% varkon_sur_num1 (sur900)");
       return(varkon_erpush("SU2993",errbuf));
       }


/*! Parameter values must be in same patch Error SU2993 if not OK. !*/
    if (iso == 2 )
        {
/* Not complete  !!!! */
        if ( fabs(t_in[0][0]-t_in[1][0]) > 1.0 ) 
            {
            sprintf(errbuf,"t_in(0-1)%% varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2993",errbuf));
            }
        if ( fabs(t_in[0][0]-t_in[1][0]) < TOL4 ) 
            {
            sprintf(errbuf,"t_in(0-1)=0%% varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2993",errbuf));
            }
        }
    if (iso == 1 )
        {
/* Not complete  !!!! */
        if ( fabs(t_in[0][1]-t_in[1][1]) > 1.0 ) 
            {
            sprintf(errbuf,"t_in(2-3)%% varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2993",errbuf));
            }
        if ( fabs(t_in[0][1]-t_in[1][1]) < TOL4 ) 
            {
            sprintf(errbuf,"t_in(2-3)=0%% varkon_sur_num1 (sur900)");
            return(varkon_erpush("SU2993",errbuf));
            }
        }
/*! The number of restarts must be between 2 and maxiter            */
/*  Error SU2993 if not OK.                                        !*/
    if (p_comp->ipl_un.nstart > 1 || 
        p_comp->ipl_un.nstart <= p_comp->ipl_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%% varkon_sur_num1 (sur900)");
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 2. Initiate output variables and static (internal) variables     */
/*                                                                 !*/
    *p_npts     = -123456789;

    eyev_x = -1.23456789;
    eyev_y = -1.23456789;
    eyev_z = -1.23456789;
    eye_x  = -1.23456789;
    eye_y  = -1.23456789;
    eye_z  = -1.23456789;
    angle  = -1.23456789;
    pi_x   = -1.23456789;
    pi_y   = -1.23456789;
    pi_z   = -1.23456789;
    pi_d   = -1.23456789;

   for (i_p=1; i_p<=SMAX; i_p= i_p+1)     /* Start loop             */
     {
/*   Initialize variabel EVALS                                      */
/*   Call of varkon_ini_evals (sur170).                             */
/*                                                                  */

      varkon_ini_evals (&xyz_d);
    xyz_a[i_p-1] = xyz_d;
    }                                     /* End   loop             */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** fsilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calcultes the function f_cur and dfdt              */
/*  for calculation case:  F_SILH                                   */
/*                                                                  */
               static  short   fsilhou()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The eye point eye_x, eye_y, eye_z and flag iso_p             */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Function value:                                                  */
 f_cur  = xyz.n_x*xyz.r_x + xyz.n_y*xyz.r_y + xyz.n_z*xyz.r_z -
            eye_x*xyz.n_x -   eye_y*xyz.n_y -   eye_z*xyz.n_z; 
/*                                                                 !*/

 if  (  iso_p == 2 )
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant V line (iso_p=2):  */
 dfdt   = xyz.nu_x*xyz.r_x + xyz.nu_y*xyz.r_y + xyz.nu_z*xyz.r_z +
           xyz.n_x*xyz.u_x +  xyz.n_y*xyz.u_y +  xyz.n_z*xyz.u_z -
          xyz.nu_x*eye_x   - xyz.nu_y*eye_y   - xyz.nu_z*eye_z; 
/*                                                                 !*/
 }
 else
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant U line (iso_p=1):  */
 dfdt   = xyz.nv_x*xyz.r_x + xyz.nv_y*xyz.r_y + xyz.nv_z*xyz.r_z +
           xyz.n_x*xyz.v_x +  xyz.n_y*xyz.v_y +  xyz.n_z*xyz.v_z -
          xyz.nv_x*eye_x   - xyz.nv_y*eye_y   - xyz.nv_z*eye_z; 
/*                                                                 !*/
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*fsilhou t_c %f tglobal %f f_cur %f dfdt %f\n"
   , t_c , tglobal, f_cur, dfdt );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** isilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calcultes the function f_cur and dfdt              */
/*  for calculation case:  I_SILH                                   */
/*                                                                  */
               static  short   isilhou()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The eye vector eyev_x, eyev_y, eyev_z and flag iso_p         */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/* Function value:                                                  */
 f_cur  = xyz.n_x*eyev_x  + xyz.n_y*eyev_y  + xyz.n_z*eyev_z;    
/*                                                                 !*/

 if  (  iso_p == 2 )  /* 1 eller 2 ????? */
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant V line (iso_p=2):  */
 dfdt   = xyz.nu_x*eyev_x + xyz.nu_y*eyev_y + xyz.nu_z*eyev_z; 
/*                                                                 !*/
 }
 else
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant U line (iso_p=1):  */
 dfdt   = xyz.nv_x*eyev_x + xyz.nv_y*eyev_y + xyz.nv_z*eyev_z; 
/*                                                                 !*/
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
/*
  fprintf(dbgfil(SURPAC),
  "POI_FREE(#%d,VEC(%f,%f)); ! t xyz.n_y xyz.nu_y= %f \n",
      (int)i_rg, tglobal, xyz.n_y,   xyz.nu_y);
  fprintf(dbgfil(SURPAC),
  "POI_FREE(#%d,VEC(%f,%f)); ! t xyz.n_z xyz.nu_z= %f \n",
      (int)i_rg+100, tglobal, xyz.n_z,   xyz.nu_z);
*/
  fprintf(dbgfil(SURPAC),
  "sur900*isilhou t_c %f tglobal %f f_cur %f dfdt %f\n"
   , t_c , tglobal, f_cur, dfdt );
  fprintf(dbgfil(SURPAC),
  "sur900*isilhou xyz.nu_x %f xyz.nu_y %f xyz.nu_z %f\n"
   ,  xyz.nu_x,xyz.nu_y,xyz.nu_z );
  fprintf(dbgfil(SURPAC),
  "sur900*isilhou xyz.nv_x %f xyz.nv_y %f xyz.nv_z %f\n"
   ,  xyz.nv_x,xyz.nv_y,xyz.nv_z );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/


/*!************** Internal ** function ** ssilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calcultes the function f_cur and dfdt              */
/*  for calculation case:  S_SILH                                   */
/*                                                                  */
               static  short   ssilhou()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The vector  sdir                                             */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector plane;        /* Plane  normal                           */
  DBint    ocase;        /* Output case. Eq. 1: Normalised vector   */
  DBVector psdir;        /* U crv tangent projected in spine plane  */
 
/*----------------------------------------------------------------- */

   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */


   plane.x_gm= xyz.spt_x; 
   plane.y_gm= xyz.spt_y; 
   plane.z_gm= xyz.spt_z;

   ocase= 1;      
       status= varkon_vec_projpla
       ( sdir, plane, ocase, &psdir );
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%sur900*ssilhou");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ssilhou spt_x %f spt_y %f spt_z %f\n"
   , xyz.spt_x, xyz.spt_y, xyz.spt_z );
  }
#endif

/* Function value:                                                  */
   f_cur= psdir.y_gm*xyz.v_z+psdir.x_gm*xyz.v_y+psdir.z_gm*xyz.v_x 
         -psdir.y_gm*xyz.v_x-psdir.x_gm*xyz.v_z-psdir.z_gm*xyz.v_y;
/*                                                                 !*/

 if  (  iso_p == 1 )
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant V line (iso_p=1):  */
  dfdt=psdir.y_gm*xyz.v2_z+psdir.x_gm*xyz.v2_y+psdir.z_gm*xyz.v2_x 
      -psdir.y_gm*xyz.v2_x-psdir.x_gm*xyz.v2_z-psdir.z_gm*xyz.v2_y;

/*                                                                 !*/
 }
 else
 {
 sprintf(errbuf,"(ssilhou)%% varkon_sur_num1 (sur900)");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ssilhou t_c %f tglobal %f f_cur %f dfdt %f\n"
   , t_c , tglobal, f_cur, dfdt );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** intplan *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calcultes the function f_cur and dfdt              */
/*  for calculation case:  SURPLAN                                  */
/*                                                                  */
               static  short   intplan()
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     Plane pi_x, pi_y, pi_z, pi_d and flag iso_p                  */
/* Out: Function f_cur and derivative dfdt                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Function value:                                                  */
 f_cur  = pi_x*xyz.r_x + pi_y*xyz.r_y + pi_z*xyz.r_z  - pi_d;
/*                                                                 !*/

 if  (  iso_p == 2 )
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant V line (iso_p=2):  */
 dfdt   = pi_x*xyz.u_x + pi_y*xyz.u_y + pi_z*xyz.u_z;
/*                                                                 !*/
 }
 else
 {
/*!                                                                 */
/* Function derivative value dfdt for a constant U line (iso_p=1):  */
 dfdt   = pi_x*xyz.v_x + pi_y*xyz.v_y + pi_z*xyz.v_z;
/*                                                                 !*/
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && fabs(f_cur) < 0.1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*intplan* tglobal= %15.10f f_cur= %15.10f\n"
   , tglobal, f_cur);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*intplan* tglobal= %f f_cur= %f dfdt= %f\n"
   , tglobal, f_cur, dfdt );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

#ifdef NOT_USED_FOR_THE_MOMENT
/*!************** Internal ** function ** parab2d *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function interpolates a start point, an end point and       */
/*  and an end derivative with a parabola and calculates X          */
/*  for F(X)=0                                                      */
/*                                                                  */
                 static  short   parab2d()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In and out: Global (static) variables:                           */
/*             tglobal, tglobal_pre, f_cur, f_pre                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u1;          /* Local coordinate for tglobal_pre        */
   DBfloat  v1;          /* Local coordinate for f_pre              */
   DBfloat  b1,b2;       /* Coefficients for parabola               */
   DBfloat  v;           /* Local coordinate corresp. to parab=0    */
   DBfloat  u_r1,u_r2;   /* Two solutions (roots)                   */
 
/*-----------------------------------------------------------------!*/

   DBfloat  dum1;        /* Dummy DBfloat                           */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_sur_num1 (sur900) ** parab2d ******\n");
  fprintf(dbgfil(SURPAC),
  "sur900 tglobal_pre= %f  f_pre= %f (dfdt= %f)\n",
     tglobal_pre,f_pre,dfdt);
  fprintf(dbgfil(SURPAC),
  "sur900 tglobal=     %f  f_cur=     %f \n",tglobal    ,f_cur);
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* --------------------------------------                           */
/*                                                                  */
/* Determine case. Goto _a for tglobal_pre <= tglobal               */
/*                 Goto _b for tglobal_pre >  tglobal               */
/*                                                                 !*/

    if   ( tglobal_pre >  tglobal ) goto _b;
/*  else ( tglobal_pre <= tglobal ) goto _a; */
/*_a:  Label for tglobal_pre <= tglobal */
    
/*! 2.a Create parabola                                            !*/

/*  Input points to local coordinates                               */

    u1 = tglobal - tglobal_pre;
    v1 =   f_cur-    f_pre  ;

/*! Coefficients for the parabola                                  !*/
    
    b1 = 2.0*v1 - u1*dfdt;
    b2 = v1 - b1;

/*!                                                                 */
/* 3.a Calculate new tglobal for parabola= 0                        */
/*                                                                 !*/

    v = 0.0 - f_pre;

    if ( fabs(b2) > TOL1 )
        {
        u_r1 = -b1*u1/(2.0*b2) + 
           SQRT( (b1*u1/(2.0*b2))*(b1*u1/(2.0*b2)) + u1*u1/b2*v );

        u_r2 = -b1*u1/(2.0*b2) - 
           SQRT( (b1*u1/(2.0*b2))*(b1*u1/(2.0*b2)) + u1*u1/b2*v );
        }
    else
        {
        if ( fabs(b1) > TOL1 )
            {
            u_r1 = v*u1/b1;
            u_r2 = u_r1;
            }
        else
            {
            /* Error */
            u_r1 = u1;
            u_r2 = u_r1;
            }
        }
      
/*  Choose root nearest current u value (tglobal) in the loop      */

        if ( fabs(u_r1 - u1) < fabs(u_r2-u1) )
            {
            dum1       = tglobal;
            tglobal     = u_r1 + tglobal_pre;
            tglobal_pre = dum1;
            f_pre      = f_cur;
            }
        else
            {
            dum1       = tglobal;
            tglobal     = u_r2 + tglobal_pre;
            tglobal_pre = dum1;
            f_pre      = f_cur;
            }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Exit *** varkon_sur_num1 *** parab2d *** tglobal= %f \n"
   ,tglobal);
/*
  fprintf(dbgfil(SURPAC),
  "sur900 Out (_a) tglobal_pre= %f  f_pre= %f \n",
     tglobal_pre,f_pre);
  fprintf(dbgfil(SURPAC),
  "sur900 tglobal=     %f  \n",tglobal );
*/
  }
#endif
       return(SUCCED);

/*!                                                                 */
_b: /* Label for tglobal_pre >  tglobal */

    
/* 2.b Create parabola                                              */

/*  Input points to local coordinates                               */
/*                                                                 !*/

    u1 = tglobal_pre - tglobal;
    v1 =   f_pre    -   f_cur   ;

/*!                                                                 */
/* Coefficients for the parabola                                    */
/*                                                                 !*/
    
    b1 = u1*dfdt;
    b2 = v1 - b1;

/*!                                                                 */
/* 3.b Calculate new tglobal for parabola= 0                        */
/*                                                                 !*/

/*  Parabola= 0 in local coordinates                                */
    v = 0.0 - f_cur;

    if ( fabs(b2) > TOL1 )
        {
        u_r1 = -b1*u1/(2.0*b2) + 
           SQRT( (b1*u1/(2.0*b2))*(b1*u1/(2.0*b2)) + u1*u1/b2*v );

        u_r2 = -b1*u1/(2.0*b2) - 
           SQRT( (b1*u1/(2.0*b2))*(b1*u1/(2.0*b2)) + u1*u1/b2*v );
        }
    else
        {
        if ( fabs(b1) > TOL1 )
            {
            u_r1 = v*u1/b1;
            u_r2 = u_r1;
            }
        else
            {
            /* Error */
            u_r1 = u1;
            u_r2 = u_r1;
            }
        }
      

/*  Choose root nearest current u value (tglobal) in the loop       */

        if ( fabs(u_r1 - u1) < fabs(u_r2-u1) )
            {
            tglobal_pre = tglobal;
            tglobal     = u_r1 + tglobal;
            f_pre      = f_cur;
            }
        else
            {
            tglobal_pre = tglobal;
            tglobal     = u_r2 + tglobal;
            f_pre      = f_cur;
            }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Exit *** varkon_sur_num1 *** parab2d *** tglobal= %f \n"
   ,tglobal);
/*
  fprintf(dbgfil(SURPAC),
  "sur900 Out (_b) tglobal_pre= %f  f_pre= %f \n",
     tglobal_pre,f_pre);
  fprintf(dbgfil(SURPAC),
  "sur900 tglobal=     %f  \n",tglobal );
*/
  }
#endif

       return(SUCCED);


} /* End of function */

#endif /* NOT_USED_FOR_THE_MOMENT */

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** addsol ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Add a solution if it is a new solution (not already existing).  */
/*                                                                  */
/*  Note that the identical points criterion (idpoint) is used for  */
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
/*     The eye point eye_x, eye_y, eye_z and flag iso_p             */
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
   DBint   f_e;          /* Flag equal points: 1=Equal 0=Not equal  */
   short   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*addsol idpoint %f numsol %d \n"
   ,   idpoint , (int)numsol );
  }
#endif

/*!                                                                 */
/* 1. Check if solution already exists                              */
/* ___________________________________                              */
/*                                                                  */
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
/*    is less than idpoint.  Goto sex if this is the case.          */
/*    Make additional comparison (call ana2pts) if points are       */
/*    nearly equal.                                                 */
/*                                                                 !*/

     dist = SQRT (
       (xyz.r_x-xyz_a[i_sol-1].r_x)*(xyz.r_x-xyz_a[i_sol-1].r_x) +
       (xyz.r_y-xyz_a[i_sol-1].r_y)*(xyz.r_y-xyz_a[i_sol-1].r_y) +
       (xyz.r_z-xyz_a[i_sol-1].r_z)*(xyz.r_z-xyz_a[i_sol-1].r_z) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  dist > idpoint )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*addsol u %10.5f v %10.5f dist %f idpoint %f \n",
   xyz_a[i_sol-1].u , xyz_a[i_sol-1].v , dist  ,   idpoint );
  }
#endif

/*QQ 1998-09-26     if ( dist <= TOL2 ) goto sex;  */
     if      ( dist <= idpoint )       goto sex;
     else if ( dist <= 100.0*idpoint )
       {
       f_e = I_UNDEF;
       status = ana2pts (p_sur,p_pat,xyz_a,i_sol, &f_e);
       if  ( status < 0 )
         {
         sprintf(errbuf,"ana2pts%%(sur900*addsol)");
         return(varkon_erpush("SU2973",errbuf));
         }
/*     The additional comparison result ........                    */
       if (1 == f_e) goto sex;
       }


     }  /* End loop i_sol= 1,2,..., numsol                          */



/*!                                                                 */
/* 2. Add solution                                                  */
/* _______________                                                  */
/*                                                                  */

adds:  /* Label: Add solution                                       */

/*  Increase number of solutions.                                   */
    numsol = numsol + 1;
/*  Check that numsol is less or equal SMAX                         */
/*                                                                 !*/
    if ( numsol > SMAX )
       {
       sprintf(errbuf,"(SMAX)%% varkon_sur_num1 (sur900)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/*  Recompute xyz with icase_l= 3 for SURPLAN or S_SILH             */
/*                                                                 !*/

    if ( ctype  == SURPLAN || ctype == S_SILH )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*addsol Recompute _sur_eval u %10.5f v %10.5f\n",
   xyz.u , xyz.v );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       icase_l = 3;  
       status= varkon_sur_eval
       (p_sur,p_pat,icase_l,xyz.u,xyz.v,&xyz);
       if (status<0) 
        {
        sprintf(errbuf,"sur210%%(sur900*addsol)");
        return(varkon_erpush("SU2943",errbuf));
        }
       }

/*!                                                                 */
/*  Points and coordinates to xyz_a.                                */
/*                                                                 !*/

    xyz_a[numsol-1]=xyz;     /* The R*3 solution point    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*addsol x %10.2f y %10.2f z %10.2f\n",
   xyz.r_x , xyz.r_y , xyz.r_z);
  fflush(dbgfil(SURPAC)); 
  }
#endif

sex:  /*! Label sex: Solution already exists                       !*/

    return(SUCCED);


} /* End of function */

/********************************************************************/


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** analyze********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Determine if the whole, or part of the boundary curve is the    */
/*  solution.                                                       */
/*                                                                  */
     static  short   analyze (p_sur,p_pat,nstart,icase,xyz_a,p_f_a)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   DBint   nstart;       /* The number of restarts                  */
   DBint   icase;        /* Calculation case for varkon_sur_eval    */
/* Out:                                                             */
/* Global static variable numsol.                                   */
   EVALS   xyz_a[SMAX];  /* Array with all solutions (R*3 and UV)   */
   DBint   *p_f_a;       /* Flag for analyzis                 (ptr) */
                         /* Eq. 0: No point      (numsol= 0)        */
                         /* Eq. 1: One end point (numsol= 0)        */
                         /* Eq. 2: Two points    (numsol= 2)        */

/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat deltap;       /* The parameter t step for points         */
   DBint   i_poi;        /* Loop index point along the boundary     */
   DBint   n_poi;        /* Number of points along the boundary     */
   DBint   i_start;      /* Start point inside c_zero               */
   DBint   i_end;        /* End   point inside c_zero               */
 
/*-----------------------------------------------------------------!*/

   EVALS   xyz_start;    /* Coordinates and derivatives for i_start */
   EVALS   xyz_end;      /* Coordinates and derivatives for i_end   */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*analyze i_zero %f numsol %d \n" , i_zero , (int)numsol );
  }
#endif

/*!                                                                 */
/* 1. Check of input data for Debug On                              */
/*                                                                  */
/*                                                                 !*/

#ifdef  DEBUG
    if ( numsol != 0 ) 
      {
      sprintf(errbuf,"(numsol)%% varkon_sur_num1*analyze");
      return(varkon_erpush("SU2993",errbuf));
      }
    if ( nstart <  1 ) 
      {
      sprintf(errbuf,"(nstart)%% varkon_sur_num1*analyze");
      return(varkon_erpush("SU2993",errbuf));
      }
#endif

/*!                                                                 */
/* Parameter step for along the boundary curve for n_poi= 2*nstart  */
/*  .... more programming for number of of points (R*3 distance) .. */
/*  .... more programming use dfdt, curve tangents ................ */
/*                                                                 !*/

   n_poi = 2*nstart;
   deltap  = (t_e-t_s-comptol)/((DBfloat)n_poi-1.0);

/*!                                                                 */
/* Start loop i_poi= 1, 2, 3, .... n_poi                            */
/*                                                                 !*/

   *p_f_a  =  0;
   i_start = -1;
   i_end   = -1;

   for (i_poi = 1; i_poi <= n_poi; i_poi = i_poi+1)
     {
/*!                                                                 */
/*   Current parameter value tglobal                                */
     tglobal = t_s + deltap*((DBfloat)i_poi-1.0);

/*!                                                                 */
/*   Call of varkon_sur_eval (sur210) for all surface types         */
/*                                                                 !*/

       if (iso_p == 2)
       status= varkon_sur_eval
       (p_sur,p_pat,icase,tglobal,t_c,&xyz);
       else
       status= varkon_sur_eval
       (p_sur,p_pat,icase,t_c,tglobal,&xyz);
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_num1*analyze");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*   Calculation of function f_cur and derivative dfdu.             */
/*   Call of fsilhou for ctype = F_SILH                             */
/*   Call of isilhou for ctype = I_SILH                             */
/*   Call of ssilhou for ctype = S_SILH                             */
/*   Call of fisopho for ctype = F_ISOP                             */
/*   Call of iisopho for ctype = I_ISOP                             */
/*   Call of intplan for ctype = SURPLAN                            */
/*   (Error SU2973 if internal function fails)                      */
/*                                                                 !*/

       if      ( ctype  == F_SILH  ) status=fsilhou();
       else if ( ctype  == I_SILH  ) status=isilhou();
       else if ( ctype  == S_SILH  ) status=ssilhou();
       else if ( ctype  == F_ISOP  ) status=intplan();
       else if ( ctype  == I_ISOP  ) status=intplan();
       else if ( ctype  == SURPLAN ) status=intplan();
       else
         {
         sprintf(errbuf,"(ctype 2)%% varkon_sur_num1 (sur900)");
         return(varkon_erpush("SU2993",errbuf));
         }

       if (status<0) 
            {
            sprintf(errbuf,"f, dfdt%%varkon_sur_num1*analyze");
            return(varkon_erpush("SU2973",errbuf));
            }

/* 1998-09-26 if ( fabs(f_cur) <  50.0*c_zero && i_start <  0 ) */
     if      ( fabs(f_cur) <  i_zero && i_start <  0 ) 
       {
       xyz_start = xyz;
       i_start   = i_poi;
       }
/*1998-09-26 else if ( fabs(f_cur) <  50.0*c_zero && i_start >  0 ) */
     else if ( fabs(f_cur) <  i_zero && i_start >  0 ) 
       {
       xyz_end   = xyz;
       i_end     = i_poi;
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  && fabs(f_cur) < 100.0*i_zero && 
                             fabs(f_cur) >=   i_zero   )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*analyze i_start %d i_end %d i_poi %d t %f f_cur %f Outside\n" , 
                   (int)i_start, (int)i_end, (int)i_poi, tglobal,f_cur );
  }
if ( dbglev(SURPAC) == 1 && fabs(f_cur) < i_zero   )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*analyze i_start %d i_end %d i_poi %d t %f f_cur %f  Inside\n" , 
                   (int)i_start, (int)i_end, (int)i_poi, tglobal,f_cur );
  }
#endif

     }
/*!                                                                 */
/* End   loop i_poi= 1, 2, 3, .... n_poi                            */
/*                                                                 !*/


  if       ( i_start == 1 && i_end == n_poi )
    {
    tglobal = t_s;
    xyz = xyz_start;
    addsol(p_sur,p_pat,xyz_a);         /* Add non-existent solution */
    tglobal = t_e;
    xyz = xyz_end;   
    addsol(p_sur,p_pat,xyz_a);         /* Add non-existent solution */
    *p_f_a  =  2;
    }
  else if  ( i_start == 1 && i_end < 0  )
    {
    *p_f_a  =  1;
    }
  else if  ( i_start == n_poi && i_end < 0  )
    {
    *p_f_a  =  1;
    }
  else if  ( i_start >= 1 && i_end >= n_poi )
    {
    tglobal = t_s + deltap*((DBfloat)i_start-1.0);
    xyz = xyz_start;
    addsol(p_sur,p_pat,xyz_a);         /* Add non-existent solution */
    tglobal = t_s + deltap*((DBfloat)i_end  -1.0);
    xyz = xyz_end;   
    addsol(p_sur,p_pat,xyz_a);         /* Add non-existent solution */
    *p_f_a  =  2;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*analyze i_start %d i_end %d n_poi %d *p_f_a %d numsol %d\n" , 
    (int)i_start, (int)i_end, (int)n_poi, (int)*p_f_a, (int)numsol );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*analyze Output numsol %d \n" ,   (int)numsol );
  }
#endif

/*!                                                                 */
    return(SUCCED);


} /* End of function */

/********************************************************************/


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!************** Internal ** function ** ana2pts********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Additional analysis in order to find out if two solutions are   */
/*  identical.                                                      */
/*                                                                  */
/*  Input points is one existing solution in the output array xyz_a */
/*  and the global (static) variable xyz.                           */
/*  This function is used to find a more exact solution when,       */
/*  for the plane/surface intersect, the plane is nearly tangential */
/*  to a patch boundary. The numerical method will in these cases   */
/*  give multiple solutions which can be at a greater distance      */
/*  idpoint even if the end criterion ctol (c_zero) is many times   */
/*  less than idpoint.                                              */
/*  An alternative solution could be to run this function (sur900)  */
/*  with an end criterion c_zero = c_zero/10.0 or c_zero/100.0      */
/*                                                                  */
/*  This function assumes that function analyze finds an interval   */
/*  of solutions (more programming is actually needed in analyze ..)*/
/*                                                                  */
/*  Note that the input (existing) solution point will be adjusted  */
/*  if the soltions are equal                                       */
/*                                                                  */
     static short ana2pts (p_sur,p_pat,xyz_a,i_sol,p_f_e)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
/*     Points and derivatives xyz from varkon_sur_eval              */
/* Out:                                                             */
/* Global static variable numsol.                                   */
   EVALS   xyz_a[SMAX];  /* Array with all solutions (R*3 and UV)   */
   DBint   i_sol;        /* Index to comparison point               */
   DBint  *p_f_e;        /* Flag equal points: 1=Equal 0=Not equal  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat deltap;       /* The parameter t step for points         */
   DBint   i_poi;        /* Loop index point along the boundary     */
   DBint   n_poi;        /* Number of points along the boundary     */
   DBfloat t_s_ana;      /* Start parameter value for analysis      */
   DBfloat t_e_ana;      /* End   parameter value for analysis      */

 
/*-----------------------------------------------------------------!*/

   DBint   i_start;      /* Start point inside c_zero               */
   DBint   i_end;        /* End   point inside c_zero               */
   DBint   i_minim;      /* Start point inside c_zero               */
   DBfloat min_dist;     /* Start parameter value for analysis      */
   DBint   icase_l;      /* Calculation case for varkon_sur_eval    */
   EVALS   xyz_input;    /* Input coordinates and derivatives       */
   EVALS   xyz_minim;    /* Coordinates and derivatives for minimum */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts idpoint %f numsol %d \n" , idpoint , (int)numsol );
  }
#endif

/*!                                                                 */
/* 1. Check of input data for Debug On                              */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Initialize output flag to 1 <==> Points are equal                */
/*                                                                 !*/
   *p_f_e = 1;

/*!                                                                 */
/* Input point xyz to local parameter (xyz used for the calulation  */
/* of the objective function. Necessary to reset xyz at exit of     */
/* of this function. Refer to addsol, which calls this function)... */
/*                                                                 !*/
   xyz_input = xyz; /* TODO Use V3MOME */
/* Initialize minimum value TODO: Call initial instead ........     */
   xyz_minim = xyz; /* TODO Use V3MOME */

/*!                                                                 */
/* Parameter step for points between the two solutions              */
/*  .... more programming for number of of points (R*3 distance) .. */
/*  .... more programming use dfdt, curve tangents ................ */
/*                                                                 !*/

/* Isoparameter curve is U or V                                     */
   if ( 2 == iso_p )
      {
      t_s_ana = xyz_a[i_sol-1].u;
      t_e_ana = xyz.u;
      }
   else
      {
      t_s_ana = xyz_a[i_sol-1].v;
      t_e_ana = xyz.v;
      }

   n_poi = 5;
   deltap  = (t_e_ana-t_s_ana)/((DBfloat)n_poi-1.0);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts t_s_ana %f t_e_ana %f deltap %f n_poi %d \n" , 
               t_s_ana, t_e_ana, deltap, (int)n_poi);
  }
#endif

/*!                                                                 */
/* Start loop i_poi= 1, 2, 3, .... n_poi                            */
/*                                                                 !*/

   i_minim  = -1;
   min_dist = 50000.0;
   i_start  = -1;
   i_end    = -1;

/* Calculate all derivatives, since solution may be modified        */
/* Loss of performance, not necessary.. Refer to addsol             */
   icase_l = 3;  

   for (i_poi = 1; i_poi <= n_poi; i_poi = i_poi+1)
     {
/*!                                                                 */
/*   Current parameter value tglobal                                */
     tglobal = t_s_ana + deltap*((DBfloat)i_poi-1.0);

/*!                                                                 */
/*   Call of varkon_sur_eval (sur210) for all surface types         */
/*                                                                 !*/

       if (iso_p == 2)
       status= varkon_sur_eval
       (p_sur,p_pat,icase_l,tglobal,t_c,&xyz);
       else
       status= varkon_sur_eval
       (p_sur,p_pat,icase_l,t_c,tglobal,&xyz);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  && status < 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts sur210 failed status= %d \n" , (int)status );
  }
#endif
       if (status<0) 
        {
        sprintf(errbuf,"sur210%%sur900*ana2pts");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*   Calculation of function f_cur and derivative dfdu.             */
/*   Call of fsilhou for ctype = F_SILH                             */
/*   Call of isilhou for ctype = I_SILH                             */
/*   Call of ssilhou for ctype = S_SILH                             */
/*   Call of fisopho for ctype = F_ISOP                             */
/*   Call of iisopho for ctype = I_ISOP                             */
/*   Call of intplan for ctype = SURPLAN                            */
/*   (Error SU2973 if internal function fails)                      */
/*                                                                 !*/

       if      ( ctype  == F_SILH  ) status=fsilhou();
       else if ( ctype  == I_SILH  ) status=isilhou();
       else if ( ctype  == S_SILH  ) status=ssilhou();
       else if ( ctype  == F_ISOP  ) status=intplan();
       else if ( ctype  == I_ISOP  ) status=intplan();
       else if ( ctype  == SURPLAN ) status=intplan();
       else
         {
         sprintf(errbuf,"(ctype)%% (sur900*ana2pts)");
         return(varkon_erpush("SU2993",errbuf));
         }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  && status < 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts Calculation f, dfdu failed status= %d \n" , (int)status );
  }
#endif
       if (status<0) 
            {
            sprintf(errbuf,"f, dfdt%%varkon_sur_num1*ana2pts");
            return(varkon_erpush("SU2973",errbuf));
            }

/*!                                                                 */
/*   Compare minimum distance                                       */
/*                                                                 !*/
     if      ( fabs(f_cur) <  min_dist ) 
       {
       min_dist  = fabs(f_cur);
       xyz_minim = xyz;
       i_minim   = i_poi;
       }

/*!                                                                 */
/*   Change flag to not equal points if objective function value    */
/*   is greater than criterion i_zero.                              */
/*                                                                 !*/
     if  ( fabs(f_cur) >=  i_zero ) *p_f_e = 0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  && fabs(f_cur) >=   i_zero   )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts *p_f_e  %d i_poi %d f_cur %f Outside\n" , 
                   (int)*p_f_e , (int)i_poi, f_cur );
  }
if ( dbglev(SURPAC) == 1 && fabs(f_cur) < i_zero   )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts *p_f_e  %d i_poi %d f_cur %15.12f dfdt %f\n" , 
                   (int)*p_f_e , (int)i_poi, f_cur, dfdt );
  }
#endif

     }
/*!                                                                 */
/* End   loop i_poi= 1, 2, 3, .... n_poi                            */
/*                                                                 !*/


/*!                                                                 */
/* Modify existing solution if minimum point                        */
/*                                                                 !*/

   if  ( 1 == *p_f_e ) xyz_a[i_sol-1] = xyz_minim;


/*!                                                                 */
/* Reset point xyz to the input value                               */
/*                                                                 !*/
   xyz = xyz_input; /* TODO Use V3MOME */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur900*ana2pts n_poi %d *p_f_e %d i_minim %d min_dist %15.10f\n" , 
    (int)n_poi, (int)*p_f_e, (int)i_minim, min_dist );
  }
#endif

    return(SUCCED);


} /* End of function */

/********************************************************************/

