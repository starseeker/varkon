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
/*  Function: varkon_sur_num3                      File: sur903.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the solution of          */
/*  a differential equation. A numerical method is used.            */
/*                                                                  */
/*  TODO This function is not programmed                            */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-02-24   Originally written                                 */
/*  1999-12-01   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_num3       Differential equation solution   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Check and initiations For Debug On */
#endif
static short retrieve();      /* Retrieve input to local variables  */
static short objgeodes();     /* Geodesic objective function        */
#ifdef  NOT_YET_USED
static short parab3p();       /* Parabola interp.: 3 pts            */
#endif  /*  NOT_YET_USED */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Flow diagram -------------------------------------*/
/*                _________________                                 */
/*               !                 !                                */
/*               ! varkon_sur_num3 !                                */
/*               !    (sur903)     !                                */
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
/*          ! nearpt ! !        !        ! stepleng ! ! parab3p !   */
/*          !________! !________!        !__________! !_________!  !*/
/*                                                                  */
/*------------------------------------------------------------------*/
/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint    ctype;            /* Case of computation            */
static DBint    method;           /* Method which shall be used     */
static DBint    nstart;           /* The number of restarts         */
static DBint    maxiter;          /* Maximum number of iterations   */
static DBfloat  us,vs,ue,ve;      /* The search area (a patch)      */
static PBOUND  *p_cr;             /* Current point record     (ptr) */
static DBfloat  gtol;             /* Geodesic zero criterion        */
static DBfloat  uv_x,uv_y;        /* Start U,V point                */
static DBfloat  tuv_x,tuv_y;      /* Start U,V tangent              */
static DBint    n_iter;           /* Number of iterations           */
static DBVector pt_a;             /* Start UV point   of parabola   */
static DBVector tan_a;            /* Start UV tangent of parabola   */
static DBVector pt_c;             /* End   UV point   of parabola   */
static DBVector der2_a;           /* Start UV 2nd deriv. parabola   */
static DBCurve  parabola;         /* Parabola                       */
static DBSeg    c_uvseg;          /* Current UV parabola            */
static DBfloat  uv_t_len;         /* Length of UV tangent           */
static DBfloat  tan_len;          /* Length of UV tangent           */


#ifdef  DEBUG
static DBfloat  obj_1;            /* Objective function value 1     */
static DBfloat  obj_2;            /* Objective function value 2     */
static DBfloat  obj_3;            /* Objective function value 3     */
static DBfloat  dmdu,dmdv;        /* Search direction               */
static DBfloat  u_pat,v_pat;      /* Current U,V point on surface   */
static DBfloat  hstep;            /* Step for one-dimen. search     */
static char     errbuf[132];      /* Error message, JK 20070104     */
#endif  /*  NOT_YET_USED */
#ifdef  DEBUG
static DBint    icase;            /* Case for varkon_sur_eval       */
static EVALS    xyz;              /* Coordinates and derivatives    */
#endif
                                  /* for a point on a surface       */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_seg_parab_b    * Parabola 2 pts + 1 tangent               */
/* varkon_seg_parab_c    * Parabola pt+tangent+2nd deriv.           */
/* varkon_sur_eval       * Surface evaluation routine               */
/* varkon_angd           * Angle in degrees betw. vectors           */
/* varkon_erpush         * Error message to terminal                */
/* varkon_ini_gmseg      * Initialize DBSeg                         */
/* varkon_sur_uvsegeval  * Coordinates and derivatives for UV point */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_num3        */
/* SU2983 = sur903 Illegal computation case=   for varkon_sur_num3  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_num3      */
/* SU2993 = Severe program error in varkon_sur_num3 (sur903).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus      varkon_sur_num3 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   DBptr    pgm_sur,     /* Surface                        (DB ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable,   /* Patch boundary table              (ptr) */
   DBint    cur_pat,     /* Current patch number in PBOUND          */
   DBCurve *p_cur,       /* Surface curve                     (ptr) */
   DBSeg   *p_seg,       /* Allocated area for UV segments    (ptr) */
   DBint    n_in,        /* Input  number of curve segments         */
   DBint   *p_n_out )    /* Output number of curve segments   (ptr) */
/* Out:                                                             */

/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint    o_case;      /* Eq. 1: First point Eq. 2: Whole segment */
   DBfloat  o_delta;     /* Minimum delta step in R*3               */
   DBfloat  obj_c;       /* Objective funtion value                 */
   DBint    u_case;      /* Tangent length case:                    */
                         /* Eq. 1: Chord length  pt_ to pt_c        */
                         /* Eq. 2: Chord length (pt_ to pt_c)/2     */
                         /* Eq. 3: Input tangent length is used     */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

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
  "** Enter varkon_sur_num3 ** \n" );
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  Initiate output coordinates and derivatives For DEBUG On.       */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

#ifdef DEBUG
    status=initial (p_sur,p_pat,p_comp,p_pbtable,cur_pat,
              p_cur,p_seg,n_in,p_n_out);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_num3 (sur903)");
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

    status=retrieve (p_sur,p_pat,p_comp,p_pbtable,cur_pat,
              p_cur,p_seg,n_in,p_n_out);
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"retrieve%%varkon_sur_num3 (sur903)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* 3. Initializations of loop variables                             */
/* ____________________________________                             */
/*                                                                  */
/*                                                                 !*/

/* Initialization of c_uvseg for Debug On                           */
#ifdef DEBUG
    varkon_ini_gmseg   ( &c_uvseg );  
#endif            

   c_uvseg.spek_gm      = pgm_sur;
   c_uvseg.spek2_gm     = DBNULL;    
   parabola.ns_cu       = 1;  
   parabola.hed_cu.type = CURTYP;
   parabola.plank_cu    = FALSE; 

   u_case = 3; /* Ska {ndras till 3 tror jag */

   pt_a.x_gm  =   uv_x;
   pt_a.y_gm  =   uv_y;
   pt_a.z_gm  =    0.0; 
   tan_a.x_gm =  tuv_x;
   tan_a.y_gm =  tuv_y;
   tan_a.z_gm =    0.0; 
   tan_len    = SQRT(tan_a.x_gm*tan_a.x_gm + tan_a.y_gm*tan_a.y_gm); 
   
   if ( tan_len > 0.000000000001 )
     {
     tan_a.x_gm =  tan_a.x_gm/tan_len*uv_t_len;  
     tan_a.y_gm =  tan_a.y_gm/tan_len*uv_t_len;  
     }
   else
     {
     sprintf(errbuf,"(tan_len)%% varkon_sur_num3 (sur903)");
     return(varkon_erpush("SU2993",errbuf));
     }

   pt_c.x_gm  =   uv_x + tan_a.x_gm;
   pt_c.y_gm  =   uv_y + tan_a.y_gm;  
   pt_c.z_gm  =    0.0; 



   status= varkon_seg_parab_b 
   (pt_a , tan_a, pt_c , u_case, &c_uvseg );
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"varkon_seg_parab_b%%varkon_sur_num3");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif




    o_case = 1;
    o_delta= 10.0;
    status=objgeodes (p_sur,p_pat, o_case, o_delta,  &obj_c);
    if (status<0) 
        {
        sprintf(errbuf,"objgeodes 1%%varkon_sur_num3");
        return(varkon_erpush("SU2973",errbuf));
        }


   status= varkon_seg_parab_c 
   (pt_a , tan_a, der2_a , &c_uvseg );
#ifdef DEBUG
    if (status<0) 
        {
        sprintf(errbuf,"varkon_seg_parab_c%%varkon_sur_num3");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif


    o_case = 1;
    o_delta= 10.0;
    status=objgeodes (p_sur,p_pat, o_case, o_delta,  &obj_c);
    if (status<0) 
        {
        sprintf(errbuf,"objgeodes 2%%varkon_sur_num3");
        return(varkon_erpush("SU2973",errbuf));
        }



   *p_seg = c_uvseg;

   (*p_seg).typ = UV_CUB_SEG ; /* Temporary ! */
   *p_n_out = 1;   /* Output number of curve segments!! Temporary !! */



#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
fprintf(dbgfil(SURPAC),
"sur903 Exit*varkon_sur_num3 \n");
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

   static short initial (p_sur,p_pat,p_comp,p_pbtable,cur_pat,
              p_cur,p_seg,n_in,p_n_out)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable;   /* Patch boundary table              (ptr) */
   DBint    cur_pat;     /* Current patch number in PBOUND          */
   DBCurve *p_cur;       /* Surface curve                     (ptr) */
   DBSeg   *p_seg;       /* Allocated area for UV segments    (ptr) */
   DBint    n_in;        /* Input  number of curve segments         */
   DBint   *p_n_out;     /* Output number of curve segments   (ptr) */

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

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur903*initial ******\n");
fprintf(dbgfil(SURPAC),
"sur903*initial Comp. case %d  Method %d  Restrts %d Max iter %d\n",
      p_comp->geo_un.ctype,
      p_comp->geo_un.method, 
      p_comp->geo_un.nstart, 
      p_comp->geo_un.maxiter);
if ( p_comp->geo_un.ctype == D_GEODES )
{
fprintf(dbgfil(SURPAC),
"sur903*initial Case D_GEODES: \n");
}

}

 fflush(dbgfil(SURPAC)); 


/*!                                                                 */
/* 2. Check of input data                                           */
/* ======================                                           */
/*                                                                  */
/* Check ....                                                       */
/*                                                                 !*/


/*!                                                                 */
/* The number of maxiter  must be between 10 and 100.               */
/* The number of restarts must be between 2 and maxiter.            */
/*                                                                 !*/

    if (p_comp->geo_un.maxiter>=  10     || 
        p_comp->geo_un.maxiter<= 100   ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(maxiter)%% varkon_sur_num3 (sur903)");
        return(varkon_erpush("SU2993",errbuf));
        }

    if (p_comp->geo_un.nstart > 1 || 
        p_comp->geo_un.nstart <= p_comp->geo_un.maxiter ) 
        {
        ;
        }
    else
        {
        sprintf(errbuf,"(nstart)%% varkon_sur_num3 (sur903)");
        return(varkon_erpush("SU2993",errbuf));
        }

  if ( p_comp->geo_un.ctype == D_GEODES )
       {
       ;
       }
   else
       {
       sprintf(errbuf,"(ctype)%% varkon_sur_num3 (sur903)");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* 3. Initiate output variables and static (internal) variables     */
/* ============================================================     */
/*                                                                 !*/
    us          = -1.23456789;
    ue          = -1.23456789;
    vs          = -1.23456789;
    ve          = -1.23456789;

    ctype       = -123456789;
    method      = -123456789;
    nstart      = -123456789;
    maxiter     = -123456789;

    obj_1 = -1.23456789;          /* Objective function value 1     */
    obj_2 = -1.23456789;          /* Objective function value 2     */
    obj_3 = -1.23456789;          /* Objective function value 3     */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

/*!New-Page--------------------------------------------------------!*/

#ifdef  NOT_YET_USED
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
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur903*parab3p obj_1 %f obj_2 %f obj_3 %f hstep %f\n",
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
   if ( ABS(a2) > TOL1 )
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
       sprintf(errbuf,"(minm)%% varkon_sur_num3 (sur903)");
       return(varkon_erpush("SU2993",errbuf));
       }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && n_iter > maxiter-5 )
  {
  fprintf(dbgfil(SURPAC),
  "sur903*parab3p minm  %f hmin  %f u_pat %f v_pat %f\n",
                  minm   , hmin  ,  u_pat ,  v_pat );
  }
fflush(dbgfil(SURPAC));
#endif


    
       return(SUCCED);


} /* End of function */

#endif  /*  NOT_YET_USED */

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** retrieve ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function retrieves input data to local static variables.     */

   static short retrieve (p_sur,p_pat,p_comp,p_pbtable,cur_pat,
              p_cur,p_seg,n_in,p_n_out)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable;   /* Patch boundary table              (ptr) */
   DBint    cur_pat;     /* Current patch number in PBOUND          */
   DBCurve *p_cur;       /* Surface curve                     (ptr) */
   DBSeg   *p_seg;       /* Allocated area for UV segments    (ptr) */
   DBint    n_in;        /* Input  number of curve segments         */
   DBint   *p_n_out;     /* Output number of curve segments   (ptr) */

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
/*                                                                  */
/* Case of computation, method and number of restarts, etc          */
/* Retrieve ctype, method, nstart and maxiter from p_comp.          */
/*                                                                 !*/

   ctype   = p_comp->geo_un.ctype;
   method  = p_comp->geo_un.method;
   nstart  = p_comp->geo_un.nstart;
   maxiter = p_comp->geo_un.maxiter;

/*!                                                                 */
/*                                                                 !*/

/*!                                                                 */
/* 2. Start point, start tangent and end criterion gtol             */
/*                                                                 !*/

/*    End criterion for the numerical solution                       */
      if ( p_comp->geo_un.ctype == D_GEODES )
          {
          gtol   = p_comp->geo_un.gtol;
          uv_x   = p_comp->geo_un.uv_x;
          uv_y   = p_comp->geo_un.uv_y;
          tuv_x  = p_comp->geo_un.tuv_x;
          tuv_y  = p_comp->geo_un.tuv_y;
          }

/*!                                                                 */
/* 3. Patch area us,vs,ue,ve from PBOUND record cur_pat (p_cr)      */
/*                                                                 !*/

/*  Data from the PBOUND table:                                     */
    p_cr= p_pbtable + cur_pat;           /* Ptr to current record   */
    us   =p_cr->us;                      /* U start point           */
    vs   =p_cr->vs;                      /* V start point           */
    ue   =p_cr->ue;                      /* U end   point           */
    ve   =p_cr->ve;                      /* V end   point           */

/*!                                                                 */
/* 4. Initialise variables icase, n_iter                            */
/*                                                                 !*/

   n_iter = 0;

/*!                                                                 */
/* 5. Check that start point and tangent is inside patch            */
/*    (for Debug On)                                                */
/*                                                                 !*/
 
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur903*retrieve ctype %d method %d nstart %d maxiter %d gtol %18.15f\n",
        ctype,    method,    nstart,    maxiter ,gtol);

fprintf(dbgfil(SURPAC),
"sur903*retrieve Patch us %8.4f vs %8.4f ue %8.4f ve %8.4f\n",
    us ,  vs ,  ue ,  ve );
fprintf(dbgfil(SURPAC),
"sur903*retrieve uv_x %8.4f uv_y %8.4f tuv_x %8.4f tuv_y %8.4f\n",
    uv_x ,  uv_y ,  tuv_x ,  tuv_y );

}  /* End dbglev */

fflush(dbgfil(SURPAC)); /* To file from buffer      */

   if ( uv_x >= us  && uv_x <= ue )
     {
     ;
     }
   else
     {
     sprintf(errbuf,"(uv_x outside patch)%% varkon_sur_num3 (sur903)");
     return(varkon_erpush("SU2993",errbuf));
     }
   if ( uv_y >= vs  && uv_y <= ve )
     {
     ;
     }
   else
     {
     sprintf(errbuf,"(uv_y outside patch)%% varkon_sur_num3 (sur903)");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

/*!                                                                 */
/* 6. Length of UV tangent which results in parabolas which will    */
/*    intersect the patch boundary                                  */
/*                                                                 !*/
 
    uv_t_len = fabs(ue-us) + fabs(ve-vs);
/*??    uv_t_len = uv_t_len*0.01;?*/  /*  !!!!!  Temporary    !!!!  */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** objgeodes **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Geodesic objective function value                                */

   static short objgeodes (p_sur,p_pat, o_case, o_delta, p_obj_c)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   DBint    o_case;      /* Eq. 1: First point Eq. 2: Whole segment */
   DBfloat  o_delta;     /* Minimum delta step in R*3               */
   DBfloat *p_obj_c;     /* Objective funtion value           (ptr) */
/* Current UV parabola segment c_uvseg                              */
/*                                                                  */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   EVALC  xyz_c;         /* Curve coordinates & derivatives         */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. ..                                                            */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur903*objgeodes o_case %d o_delta %f\n", o_case,o_delta);
  }
#endif


   xyz_c.t_local=  0.0;          
   xyz_c.evltyp = EVC_KAP;
   c_uvseg.typ  = UV_CUB_SEG ; 

   status= varkon_sur_uvsegeval (&parabola,&c_uvseg,&xyz_c);
   if ( status < 0 )
     {
     sprintf(errbuf,"varkon_sur_uvsegeval%% varkon_sur_num3 (sur903)");
     return(varkon_erpush("SU2943",errbuf));
     }

  der2_a.x_gm = xyz_c.u_t2_geod0;
  der2_a.y_gm = xyz_c.v_t2_geod0;
  der2_a.z_gm =      0.0;          


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur903*objgeodes geodesic %25.10f\n", xyz_c.geodesic );
  fprintf(dbgfil(SURPAC),
  "sur903*objgeodes u_t        %25.10f v_t        %25.10f\n", 
                xyz_c.u_t     ,   xyz_c.v_t         );
  fprintf(dbgfil(SURPAC),
  "sur903*objgeodes u_t2_geod0 %25.10f v_t2_geod0 %25.10f\n", 
                xyz_c.u_t2_geod0, xyz_c.v_t2_geod0  );
  }
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

