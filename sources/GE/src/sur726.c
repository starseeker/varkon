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
/*  Function: varkon_cur_approxrci                 File: sur726.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Find a curve, with circle segments (rational quadratics) and    */
/*  straight lines, which approximates another curve.               */
/*  Try to reduce the number of segments.                           */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-01   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_approxrci  Approximate to circles + lines   */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short ini_che();       /* Initialize and check data          */
static short ana_cur();       /* Analyze curve curvature            */
static short cre_rat();       /* Create two rational quadratics     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBVector c_n;              /* Curve plane                    */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
static DBfloat  ctol;             /* Coordinate tolerance           */
static DBfloat  ntol;             /* Tangent (angle) tolerance      */
static DBfloat  lcrit;            /* Radius line criterion          */
static DBint    n_che;            /* Number of check pts per segm.  */
static DBint    n_seg;            /* Number of curve segments       */
static DBint    n_alloc;          /* Size allocated area output crv */
static DBint    n_u_cir_s;        /* Number of elements in u_cir_s  */
static DBfloat  u_cir_s[100];     /* Start U values for circles     */
static DBSeg    cir1;             /* Circle segment 1               */
static DBSeg    cir2;             /* Circle segment 2               */
static DBTmat   cur_csys;         /* Input (planar) curve system    */
static DBTmat   cur_csys_inv;     /* Inverted cur_csys.             */
static DBCurve  cur_dummy;        /* Curve                   GE713  */
static short    nstart;           /* Number of restarts      GE713  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_cir_twoseg      * Create two circle segments              */
/* varkon_GE705           * Intersect line/line                     */
/* varkon_GE109           * Curve   evaluation routine              */
/* varkon_GE133           * Rational cubic with P-value             */
/* varkon_GE136           * Rational cubic with mid pt              */
/* varkon_ini_gmlin       * Initialize DBLine                       */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/* GEtfpos_to_local       * Transformation of a point               */
/* GEtfvec_to_local       * Transformation of a vector              */
/* GEtform_inv            * Invertation of a matrix                 */
/* GE713                  * Closest points on a rational            */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ... failed in varkon_cur_approxrci      */
/* SU2993 = Severe program error ( )   in varkon_cur_approxrci      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_approxrci (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Input curve                       (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  APPCUR  *p_appdat,     /* Curve approximation data          (ptr) */
  DBCurve *p_ocur,       /* Output curve                      (ptr) */
  DBSeg   *p_oseg )      /* Coefficients for curve segments   (ptr) */

/* Out:                                                             */
/*        Number of intersects and ordered U values                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   n_outseg;      /* Number of output segments               */
  DBfloat u_start;       /* Current start parameter value           */
  DBfloat u_end;         /* Current end   parameter value           */
  DBint   i_u_cir_s;     /* Loop index for u_cir_s                  */
  DBint   no_cir;        /* Number of segments from cre_rat         */
  DBfloat max_dev;       /* Maximum deviation from input curve      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   n_iter;        /* Number of iterations. Program check     */
  DBSeg   *p_c;          /* Current curve segment             (ptr) */

  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726 Enter Approximate to circles and lines ctype %d \n",
             (int)p_appdat->ctype );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*  Call of internal function ini_che.                              */
/*                                                                 !*/

   status= ini_che( p_cur, p_seg, p_appdat );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "ini_che%%sur726");
     return(varkon_erpush("SU2973",errbuf));
     }


/*!                                                                 */
/* 2. Analyze the curve curvature                                   */
/* ______________________________                                   */
/*                                                                  */
/*  Call of internal function ana_cur.                              */
/*                                                                 !*/

   status= ana_cur( p_cur, p_seg );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "ana_cur%%sur726");
     return(varkon_erpush("SU2973",errbuf));
     }


/*!                                                                 */
/* 3. Create the output curve                                       */
/* __________________________                                       */
/*                                                                  */
/*                                                                 !*/

/* Initialize no of output segments                                 */
   n_outseg = 0;

/* Initialize iteration counter (for check of program)              */
   n_iter   = 0;

   for  ( i_u_cir_s = 0; i_u_cir_s < n_u_cir_s-1; ++i_u_cir_s )
     {

/*!                                                                 */
/*   Current start and end parameter value from u_cir_s             */
/*                                                                 !*/

     u_start =   u_cir_s[i_u_cir_s];
     u_end   =   u_cir_s[i_u_cir_s + 1] - comptol;

split:;

/*   Programming check.                                             */
     n_iter   = n_iter + 1;
     if  ( n_iter > 10000 ) 
       {
       sprintf(errbuf, "n_iter>10000%%sur726");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/*   Create two circles, circle + line or one line and calculate    */
/*   the maximum deviation to the input curve.                      */
/*   Call of internal function cre_rat.                             */
/*                                                                 !*/

     status= cre_rat( p_cur, p_seg, u_start, u_end,   
                      &no_cir, &max_dev );
     if  ( status < 0 ) 
       {
       sprintf(errbuf, "cre_rat%%sur726");
       return(varkon_erpush("SU2973",errbuf));
       }

/*   Programming check.                                             */
     if  ( max_dev < 0.0 ) 
       {
       sprintf(errbuf, "max_dev<0%%sur726");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/*   Add circle segments (circle+line or line) to the output curve  */
/*   if circle segments are within the given coordinate tolerance.  */
/*   Goto label split if not and try with half the interval.        */
/*                                                                 !*/

/*   Check that the number of output segments not is zero (0)       */

     if  ( no_cir <= 0  ) 
       {
       sprintf(errbuf, "no_cir<=0%%sur726");
       return(varkon_erpush("SU2973",errbuf));
       }

/*   Within tolerance ?                                             */

     if ( max_dev > ctol    )
       {
/*     No, not within tolerance. Let the new end parameter be       */
/*     the mid value between the current start and end value.       */
/*     Check that new parameter interval not has zero length.       */
       u_end   = u_start + 0.5*(u_end - u_start);
       if ( u_end-u_start < 0.0001 )
         {
         sprintf(errbuf, "U interv. = 0%%sur726");
         return(varkon_erpush("SU2973",errbuf));
         }
/*     Try the new segment. Goto split.                             */
       goto  split;
       }

/*   Add circle segments (circle+line or line) to the output curve  */

     if  (  no_cir >= 1 )
       {
       p_c = p_oseg + n_outseg;
       V3MOME((char *)(&cir1),(char *)(p_c),sizeof(DBSeg));
       n_outseg = n_outseg + 1;
       }
     if  (  no_cir == 2 )
       {
       p_c = p_oseg + n_outseg;
       V3MOME((char *)(&cir2),(char *)(p_c),sizeof(DBSeg));
       n_outseg = n_outseg + 1;
       }

/*!                                                                 */
/*   Check if circles (or lines) have been created for the current  */
/*   interval i_u_start, i.e. between the two values in the array   */
/*   s_u_start. If not, let u_start= u_end and let u_end be the     */
/*   end value from array s_u_start and goto label split.           */
/*   Otherwise, continue with the next interval from s_u_start.     */
/*                                                                 !*/

     u_start = u_end;
     u_end   =   u_cir_s[i_u_cir_s + 1] - comptol;
     if ( fabs(u_end-u_start) > 10.0*comptol ) goto split;


     } /* End loop i_u_cir_s */


/*!                                                                 */
/*  4. Output curve header data (DBCurve)                           */
/*  ___________________________________                             */
/*                                                                  */
/*                                                                 !*/

   p_ocur->ns_cu = (short)n_outseg;
   p_ocur->al_cu = 0.0;        
   p_ocur->plank_cu  = TRUE;       
   p_ocur->hed_cu.type = CURTYP;


  return(SUCCED);

} /* End of function */


/********************************************************************/



/*!********* Internal ** function ** ana_cur ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Analyze the curvature of the curve                               */
/*                                                                  */
/* The output from this function is an array of parameter values,   */
/* which define start/end of circle segments.                       */
/*                                                                  */
/* Criteria used are inflexion points, start/end of straight lines  */
/* (straight line criterion is a curvature radius that is input     */
/* to the function), and when the step in curvature between two     */
/* curve segments is too big (0.3*lcrit).                           */
/*                                                                  */
/* Note that there normally will be additional curve segments       */
/* in order to respect the identical points criterion ...           */
/*                                                                  */
/*                                                                  */
    static short ana_cur (p_cur, p_seg)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  kappa;        /* Curvature for current point             */
  DBVector bi_normal;    /* Binormal (for sign of kappa)            */
  DBVector kappa_seg[50];/* Curvature for n_che pts in segment      */
  DBint    n_u_cir_s_t;  /* Number of values in u_cir_s_t           */
  DBfloat  u_cir_s_t[100];/* Start U values for circles (temporary) */
  DBfloat  delta;        /* Delta parameter value                   */
  DBLine   lin_min;      /* Line lower limit straight line          */
  DBLine   lin_zero;     /* Inflexion line (curvature= 0)           */
  DBLine   lin_max;      /* Line lower limit straight line          */
  DBLine   lin_inter;    /* Intersecting line for current U         */
/*----------------------------------------------------------------- */

  EVALC    xyz_c;        /* Current point on curve                  */
  DBint    i_u_cir_s;    /* Loop index for u_cir_s                  */
  DBint    i_u_cir_s_t;  /* Loop index for u_cir_s_t                */
  DBint    i_seg;        /* Loop index curve segment                */
  DBint    i_poi;        /* Loop index point in curve segment       */
  DBint    i_max;        /* Index for sorting                       */
  DBfloat  u_start;      /* Circle start parameter value            */
  DBfloat  u_end;        /* Circle end   parameter value            */
  DBfloat  u_mid;        /* Curve  mid   parameter value            */
  DBVector v_kappa;      /* Vector for kappa value                  */

  DBint    i_int;        /* Loop index intersect                    */
  DBVector p_int;        /* Intersect point                         */

                         /* For GE705:                              */
  short    stat_int;     /* Error code (parallell lines)            */
  DBTmat  *p_csys;       /* Transformation matrix             (ptr) */
  short    n_int;        /* Number of intersects (in/out)           */
  DBfloat  u1_int[1];    /* Parameter value line 1                  */
  DBfloat  u2_int[1];    /* Parameter value line 2                  */

  DBfloat  s_u;          /* Current curve parameter value           */
  DBfloat  dot;          /* Scalar product bi_normal*c_n            */

  DBint    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur Enter  \n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Initialization of static array u_cir_s and all local             */
/* variables in this function                                       */
/*                                                                 !*/

   for ( i_u_cir_s= 0;   i_u_cir_s < 100;  ++i_u_cir_s )  
     {
     u_cir_s  [i_u_cir_s]    = F_UNDEF;
     u_cir_s_t[i_u_cir_s]    = F_UNDEF;
     }

   for ( i_poi = 0;   i_poi < 50;  ++i_poi )  
     {
     kappa_seg[i_poi].x_gm    = F_UNDEF;
     kappa_seg[i_poi].y_gm    = F_UNDEF;
     kappa_seg[i_poi].z_gm    = F_UNDEF;
     }

   bi_normal.x_gm   = F_UNDEF;
   bi_normal.y_gm   = F_UNDEF;
   bi_normal.z_gm   = F_UNDEF;

   kappa            = F_UNDEF;
   delta            = F_UNDEF;
   u_start          = F_UNDEF;
   u_end            = F_UNDEF;
   u_mid            = F_UNDEF;
   s_u              = F_UNDEF;
   dot              = F_UNDEF;

   i_max            = I_UNDEF;
   n_int            = I_UNDEF;
   status           = I_UNDEF;
   stat_int         = I_UNDEF;
   i_int            = I_UNDEF;

   p_int.x_gm       = F_UNDEF;
   p_int.y_gm       = F_UNDEF;
   p_int.z_gm       = F_UNDEF;

   v_kappa.x_gm     = F_UNDEF;
   v_kappa.y_gm     = F_UNDEF;
   v_kappa.z_gm     = F_UNDEF;

   p_csys           = NULL;

   u1_int[0]        = F_UNDEF; 
   u2_int[0]        = F_UNDEF;

/*!                                                                 */
/* Initialize DBLine. Calls of varkon_ini_gmlin (sur764).           */
/*                                                                 !*/

   varkon_ini_gmlin(&lin_max  );
   varkon_ini_gmlin(&lin_zero );
   varkon_ini_gmlin(&lin_min  );
   varkon_ini_gmlin(&lin_inter);

/*!                                                                 */
/* Delta parameter step in curve segment                            */
/*                                                                 !*/

#ifdef  DEBUG
  if ( n_che <  1  )
    {
    sprintf(errbuf,"n_che<1%%sur726*ana_cur");
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

   delta = 1.0/(n_che+1);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur delta %f \n", delta);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Lines used to find inflexion points and start of straight lines  */
/* (lines lin_min, lin_zero and lin_max)                            */
/*                                                                 !*/

   lin_min.crd1_l.x_gm =                   -1.0;
   lin_min.crd1_l.y_gm =             -1.0/lcrit;
   lin_min.crd1_l.z_gm =                    0.0;
   lin_min.crd2_l.x_gm =     (DBfloat)n_seg + 2.0;
   lin_min.crd2_l.y_gm =             -1.0/lcrit;
   lin_min.crd2_l.z_gm =                    0.0;

   lin_zero.crd1_l.x_gm =                  -1.0;
   lin_zero.crd1_l.y_gm =                   0.0;
   lin_zero.crd1_l.z_gm =                   0.0;
   lin_zero.crd2_l.x_gm =    (DBfloat)n_seg + 2.0;
   lin_zero.crd2_l.y_gm =                   0.0;
   lin_zero.crd2_l.z_gm =                   0.0;


   lin_max.crd1_l.x_gm =                   -1.0;
   lin_max.crd1_l.y_gm =              1.0/lcrit;
   lin_max.crd1_l.z_gm =                    0.0;
   lin_max.crd2_l.x_gm =     (DBfloat)n_seg + 2.0;
   lin_max.crd2_l.y_gm =              1.0/lcrit;
   lin_max.crd2_l.z_gm =                    0.0;

/*!                                                                 */
/* Start of curve is the first value in u_cir_s.                    */
/*                                                                 !*/

   u_cir_s_t[0]  = 1.0;
   n_u_cir_s_t   =   1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (Start value)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Start loop segments                                              */
/*                                                                 !*/

/* Evaluation of binormal, principal normal and curvature           */
   xyz_c.evltyp   = EVC_KAP;

   for ( i_seg= 0; i_seg < n_seg; ++i_seg )
     {

/*!                                                                 */
/*   Start loop points within segment. Calculate curvature values.  */
/*                                                                 !*/

     for ( i_poi= 0; i_poi < n_che+2; ++i_poi )
       {

/*!                                                                 */
/*     Current parameter value                                      */
/*                                                                 !*/

       s_u  = i_seg + 1.0 + i_poi*delta;
/*     End point of current segment                                 */
       if ( i_poi == n_che + 1 ) s_u = s_u - comptol;

/*!                                                                 */
/*     Current curvature value to array                             */
/*     Call of GE109.                                               */
/*                                                                 !*/

       xyz_c.t_global = s_u;   

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_c);
       if (status<0) 
         {
         sprintf(errbuf,"GE109%%(sur726*ana_cur)");
         return(varkon_erpush("SU2943",errbuf));
         }

       kappa     =  xyz_c.kappa;   
       bi_normal =  xyz_c.b_norm;
       dot       =  bi_normal.x_gm*c_n.x_gm +  
                    bi_normal.y_gm*c_n.y_gm +  
                    bi_normal.z_gm*c_n.z_gm;
/*     TODO Check that criterion is OK. Start bi_normal ... ?       */
       if ( dot < 0.0 ) kappa = -kappa;
       kappa_seg[i_poi].x_gm =   s_u;
       kappa_seg[i_poi].y_gm = kappa;
       kappa_seg[i_poi].z_gm =   0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur t_global %12.8f i_seg %d i_poi %d kappa %f\n",
  xyz_c.t_global , (int)i_seg , (int)i_poi, kappa );
fflush(dbgfil(SURPAC));
}
#endif


       } /* End loop i_poi */

/*!                                                                 */
/*   Start loop points within segment.                              */
/*   Find inflexion points and starts/ends of lines.                */
/*                                                                 !*/

     for ( i_poi= 0; i_poi < n_che+1; ++i_poi )
       {
/*!                                                                 */
/*     Create line from kappa_seg for current points (lin_inter)    */
/*                                                                 !*/

       lin_inter.crd1_l.x_gm =  kappa_seg[ i_poi ].x_gm; 
       lin_inter.crd1_l.y_gm =  kappa_seg[ i_poi ].y_gm; 
       lin_inter.crd1_l.z_gm =  kappa_seg[ i_poi ].z_gm; 
       lin_inter.crd2_l.x_gm =  kappa_seg[i_poi+1].x_gm; 
       lin_inter.crd2_l.y_gm =  kappa_seg[i_poi+1].y_gm; 
       lin_inter.crd2_l.z_gm =  kappa_seg[i_poi+1].z_gm; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur i_poi %d Line U start %f U end %f\n",
   (int)i_poi, lin_inter.crd1_l.x_gm,lin_inter.crd2_l.x_gm );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*     Intersect lin_inter/lin_min                                  */
/*                                                                 !*/

/*     Note that n_int is an input and output parameter             */
       n_int = 1;

       stat_int = GE705
         ((DBAny *)&lin_inter,(DBAny *)&lin_min,
          p_csys,&n_int,u1_int,u2_int);

/*!                                                                 */
/*     Add to u_cir_s_t for lin_inter/lin_min   if lines intersect  */
/*                                                                 !*/

       if  ( n_int > 0 && SUCCED == stat_int )
         {
         u_cir_s_t[n_u_cir_s_t] = 
           lin_inter.crd1_l.x_gm +
           u1_int[0]*(lin_inter.crd2_l.x_gm -
                      lin_inter.crd1_l.x_gm);
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (lin_min)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  End lin_inter/lin_min    exists */


/*!                                                                 */
/*     Intersect lin_inter/lin_zero                                 */
/*                                                                 !*/

/*     Note that n_int is an input and output parameter             */
       n_int = 1;

       stat_int = GE705
         ((DBAny *)&lin_inter,(DBAny *)&lin_zero,
          p_csys,&n_int,u1_int,u2_int);

/*!                                                                 */
/*     Add to u_cir_s_t for lin_inter/lin_zero  if lines intersect  */
/*                                                                 !*/

       if  ( n_int > 0 && SUCCED == stat_int )
         {
         u_cir_s_t[n_u_cir_s_t] = 
           lin_inter.crd1_l.x_gm +
           u1_int[0]*(lin_inter.crd2_l.x_gm -
                      lin_inter.crd1_l.x_gm);
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (lin_zero)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  End lin_inter/lin_zero   exists */


/*!                                                                 */
/*     Intersect lin_inter/lin_max                                  */
/*                                                                 !*/

/*     Note that n_int is an input and output parameter             */
       n_int = 1;

       stat_int = GE705
         ((DBAny *)&lin_inter,(DBAny *)&lin_max,
          p_csys,&n_int,u1_int,u2_int);

/*!                                                                 */
/*     Add to u_cir_s_t for lin_inter/lin_max   if lines intersect  */
/*                                                                 !*/

       if  ( n_int > 0 && SUCCED == stat_int )
         {
         u_cir_s_t[n_u_cir_s_t] = 
           lin_inter.crd1_l.x_gm +
           u1_int[0]*(lin_inter.crd2_l.x_gm -
                      lin_inter.crd1_l.x_gm);
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (lin_max)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  End lin_inter/lin_max    exists */


       } /* End loop i_poi (inflexion points, ... )*/


/*!                                                                 */
/*   Determine if a circle must start in the ent point              */
/*                                                                 !*/

     if  (  i_seg < n_seg - 1 )
       {
/*!                                                                 */
/*     Start point of the next segment                              */
/*                                                                 !*/
       s_u  = i_seg + 1.0 + 1.0 + comptol;

/*!                                                                 */
/*     Calculate curvature value                                    */
/*     Call of varkon_GE109 (GE109).                              */
/*                                                                 !*/

       xyz_c.t_global = s_u;   

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_c);
       if (status<0) 
         {
         sprintf(errbuf,"GE109%%(sur726*ana_cur)");
         return(varkon_erpush("SU2943",errbuf));
         }

       kappa     =  xyz_c.kappa;   
       bi_normal =  xyz_c.b_norm;
       dot       =  bi_normal.x_gm*c_n.x_gm +  
                    bi_normal.y_gm*c_n.y_gm +  
                    bi_normal.z_gm*c_n.z_gm;
/*     TODO Check that criterion is OK. Start bi_normal ... ?       */
       if ( dot < 0.0 ) kappa = -kappa;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur t_global %12.8f i_seg %d kappa %f kappa_seg %f\n",
  xyz_c.t_global , (int)i_seg , kappa, kappa_seg[n_che+1].y_gm);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*     Start is necessary for inflexion or big step in curvature    */
/*     Call of GE109.                                               */
/*                                                                 !*/

       if      ( fabs(kappa)                   < 1.0/lcrit && 
                 fabs(kappa_seg[n_che+1].y_gm) < 1.0/lcrit     )
         {
/*       Straight line. No start of circle necessary                */
         ;
         }


       else if ( kappa                         < 0.0       && 
                 kappa_seg[n_che+1].y_gm       > 0.0           )
         {
         u_cir_s_t[n_u_cir_s_t] = (DBfloat)i_seg + 2.0;
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (End/start inflexion 1)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  End/start inflexion 1  */


       else if ( kappa                         > 0.0       && 
                 kappa_seg[n_che+1].y_gm       < 0.0           )
         {
         u_cir_s_t[n_u_cir_s_t] = (DBfloat)i_seg + 2.0;
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (End/start inflexion 2)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  End/start inflexion 2  */


       else if (fabs(1.0/kappa-1.0/kappa_seg[n_che+1].y_gm) 
                              > 0.3*lcrit )
         {
         u_cir_s_t[n_u_cir_s_t] = (DBfloat)i_seg + 2.0;
         n_u_cir_s_t = n_u_cir_s_t + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (End/start Curv. step)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif
         }  /*  Big step in curvature  */



       }  /* End i_seg < n_seg - 1  */

     } /* End loop i_seg */


/*!                                                                 */
/* End   of curve is the last  value in u_cir_s.                    */
/*                                                                 !*/

   u_cir_s_t[n_u_cir_s_t] = (DBfloat)n_seg + 1.0;
   n_u_cir_s_t = n_u_cir_s_t + 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d u_cir_s_t %f (End value)\n",
   (int)n_u_cir_s_t  , u_cir_s_t[n_u_cir_s_t-1] );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Order the parameter values                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d\n", (int)n_u_cir_s_t );
fflush(dbgfil(SURPAC));
}
for  (i_u_cir_s_t = 0; i_u_cir_s_t < n_u_cir_s_t; ++i_u_cir_s_t)
  {
  if ( dbglev(SURPAC) == 2 )
    {
    fprintf(dbgfil(SURPAC),
    "sur726*ana_cur i_u_cir_s_t %d u_cir_s_t %f\n",
       (int)i_u_cir_s_t , u_cir_s_t[i_u_cir_s_t]);
    fflush(dbgfil(SURPAC));
    }
  }
#endif




   for  ( i_u_cir_s = 0; i_u_cir_s < n_u_cir_s_t; ++i_u_cir_s )
     {
     u_cir_s[i_u_cir_s] = 5000.0;
     i_max              = -12345;
     for  (i_u_cir_s_t = 0; i_u_cir_s_t < n_u_cir_s_t; ++i_u_cir_s_t)
       {
       if  (  u_cir_s_t[i_u_cir_s_t]  <  u_cir_s[i_u_cir_s]   )
         {
         i_max = i_u_cir_s_t;
         u_cir_s[i_u_cir_s]  =  u_cir_s_t[i_max];
         }
       }
       u_cir_s_t[i_max] = 5000.0;
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur n_u_cir_s_t %d (After ordering)\n", 
              (int)n_u_cir_s_t );
fflush(dbgfil(SURPAC));
}
for  (i_u_cir_s_t = 0; i_u_cir_s_t < n_u_cir_s_t; ++i_u_cir_s_t)
  {
  if ( dbglev(SURPAC) == 2 )
    {
    fprintf(dbgfil(SURPAC),
    "sur726*ana_cur i_u_cir_s_t %d u_cir_s %f\n",
       (int)i_u_cir_s_t , u_cir_s[i_u_cir_s_t]);
    fflush(dbgfil(SURPAC));
    }
  }
#endif


/*!                                                                 */
/* Check and put in temporary array again                           */
/*                                                                 !*/

   for  ( i_u_cir_s = 0; i_u_cir_s < n_u_cir_s_t-1; ++i_u_cir_s )
     {
     if  (  u_cir_s[i_u_cir_s]  >  u_cir_s[i_u_cir_s+1]  )
       {
       sprintf(errbuf,"U decreasing%%sur726*ana_cur");
       return(varkon_erpush("SU2993",errbuf));
       }
     u_cir_s_t[i_u_cir_s]  = u_cir_s[i_u_cir_s]; 
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur i_u_cir_s %d u_cir_s_t %f\n",
   (int)i_u_cir_s , u_cir_s_t[i_u_cir_s]);
fflush(dbgfil(SURPAC));
}
#endif
     }
   u_cir_s_t[n_u_cir_s_t-1]  = u_cir_s[n_u_cir_s_t-1]; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur i_u_cir_s %d u_cir_s_t %f\n",
   (int)n_u_cir_s_t-1 , u_cir_s_t[n_u_cir_s_t-1]);
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Remove identical points                                          */
/* TODO Make comparison in R*# and use identical points ....        */
/*                                                                 !*/

   u_cir_s[0]  = u_cir_s_t[0]; 
   n_u_cir_s = 1;
   for  ( i_u_cir_s = 0; i_u_cir_s < n_u_cir_s_t-1; ++i_u_cir_s )
     {

     if  (  u_cir_s_t[i_u_cir_s+1]-u_cir_s_t[i_u_cir_s] > 0.000001 )
       {
       u_cir_s[n_u_cir_s]  = u_cir_s_t[i_u_cir_s+1]; 
       n_u_cir_s = n_u_cir_s + 1;
       }
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ana_cur After ordering n_u_cir_s_t %d n_u_cir_s %d\n",
   (int)n_u_cir_s_t  , (int)n_u_cir_s);
fflush(dbgfil(SURPAC));
}
for  (i_u_cir_s = 0; i_u_cir_s < n_u_cir_s; ++i_u_cir_s)
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur726*ana_cur i_u_cir_s %d u_cir_s %f\n",
       (int)i_u_cir_s , u_cir_s[i_u_cir_s]);
    fflush(dbgfil(SURPAC));
    }
  }
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/





/*!********* Internal ** function ** ini_che ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Initialization of data and check of input data.                  */
/*                                                                  */
/*                                                                  */
    static short ini_che (p_cur, p_seg, p_appdat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
  APPCUR  *p_appdat;     /* Curve approximation data          (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */


  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ini_che Enter  \n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/


  if ( p_appdat->acase != 3 )
    {
    sprintf(errbuf,"acase != 5%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( p_appdat->ctype !=  2  )
    {
    sprintf(errbuf,"ctype != 3%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* Computer accuracy                                                */
/*                                                                 !*/

  comptol = p_appdat->comptol;

/*!                                                                 */
/* Coordinate tolerance                                             */
/*                                                                 !*/

  ctol    = p_appdat->ctol;

/*!                                                                 */
/* Tangent (angle) tolerance                                        */
/*                                                                 !*/

  ntol    = p_appdat->ntol;

/*!                                                                 */
/* Criterion for a straight line (radius of curvature)              */
/*                                                                 !*/

  lcrit   = p_appdat->lcrit;

/*!                                                                 */
/* Number of check points per curve segment                         */
/*                                                                 !*/

  if ( p_appdat->d_che < 0.0  )
    {
    n_che = p_appdat->n_req;
    }
  else
    {
    sprintf(errbuf,"d_che>= 0.0%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( n_che <  2  )
    {
    sprintf(errbuf,"n_che=n_req<2%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

/* Check due to the kappa_seg array in ana_cur                      */
  if ( n_che > 48  )
    {
    sprintf(errbuf,"n_che=n_req>50%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* Size (number of segments) for output curve.                      */
/*                                                                 !*/

  n_alloc = p_appdat->n_alloc;

/*!                                                                 */
/* Check tolerance type                                             */
/*                                                                 !*/

  if ( p_appdat->t_type != 1  )
    {
    sprintf(errbuf,"t_type != 1%%sur726*ini_che");
    return(varkon_erpush("SU2993",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ini_che lcrit %10.2f ctol %10.6f ntol %10.6f\n",
             lcrit, ctol, ntol );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ini_che n_che %d n_alloc %d\n",
             (int)n_che, (int)n_alloc );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Get number of segments in the curve (n_seg)                      */
/*                                                                 !*/

   n_seg  = (DBint)p_cur->ns_cu;


/*!                                                                 */
/* Get the curve plane normal.                                      */
/* Only programmed for planar curves, 3D can be implemented         */
/* where the output curve will have kinks.                          */
/* The function could also determine if the curve is planar with    */
/* the "average plane" function............                         */
/*                                                                 !*/


 if ( p_cur->plank_cu  != TRUE )
    {
    varkon_erinit();
    sprintf(errbuf,"su776*ini_che%%");
    return(varkon_erpush("SU4403",errbuf));
    }

   c_n.x_gm = p_cur->csy_cu.g31;
   c_n.y_gm = p_cur->csy_cu.g32;
   c_n.z_gm = p_cur->csy_cu.g33;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*ini_che Curve plane c_n= %12.8f %12.8f %12.8f\n",
   c_n.x_gm, c_n.y_gm, c_n.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Get the whole matrix for the planar curve. Will be used to       */
/* transform points and tangents when 2D circle segments are        */
/* calculated in internal function cre_rat.                         */
/* Create also the inverse matrix. Call of GEtform_inv              */
/*                                                                 !*/

   cur_csys = p_cur->csy_cu; /* TODO Use V3MOME ... */


status=GEtform_inv (&cur_csys, &cur_csys_inv );
   if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur726*ini_che GEtform_inv failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
   sprintf(errbuf,"GEtform_inv%%sur726*ini_che");
   return(varkon_erpush("SU2943",errbuf));
   }



/* Initialization static and local variables                        */

   n_u_cir_s = I_UNDEF;

    varkon_ini_gmseg ( &cir1 );  

    varkon_ini_gmseg ( &cir2 );  

/* Initialization of curve for GE713.                              */

    varkon_ini_gmcur   (  &cur_dummy ); 

  cur_dummy.hed_cu.type = CURTYP;

  nstart = 3;

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



/*!********* Internal ** function ** cre_rat ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create two circle segments as rational quadratics                */
/*                                                                  */
/* The output from this function can also be one curve segment      */
/* which is a straight line.                                        */
/*                                                                  */
    static short cre_rat (p_cur, p_seg, u_start, u_end, 
                            p_no_cir, p_max_dev)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
  DBfloat  u_start;      /* Current start parameter value           */
  DBfloat  u_end;        /* Current end   parameter value           */
  DBint   *p_no_cir;     /* Number of segments from cre_rat   (ptr) */
  DBfloat *p_max_dev;    /* Maximum deviation from input crv  (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  EVALC    xyz_s;        /* Current start point on curve            */
  EVALC    xyz_e;        /* Current end   point on curve            */
  DBVector poi_s_b;      /* Current start point   Basic system      */
  DBVector poi_s_l;      /* Current start point   local system      */
  DBVector poi_e_b;      /* Current end   point   Basic system      */
  DBVector poi_e_l;      /* Current end   point   local system      */
  DBVector tan_s_b;      /* Current start tangent Basic system      */
  DBVector tan_s_l;      /* Current start tangent local system      */
  DBVector tan_e_b;      /* Current end   tangent Basic system      */
  DBVector tan_e_l;      /* Current end   tangent local system      */
  DBVector poi_m_b;      /* Current mid   point   Basic system      */
  DBVector tan_m_b;      /* Current mid   tangent Basic system      */
  DBVector poi_m_l;      /* Current mid   point   local system      */
  DBVector tan_m_l;      /* Current mid   tangent local system      */
/*----------------------------------------------------------------- */

  DBfloat  poi1_x;       /* Start point   X coordinate              */
  DBfloat  poi1_y;       /* Start point   Y coordinate              */
  DBfloat  poi2_x;       /* End   point   X coordinate              */
  DBfloat  poi2_y;       /* End   point   Y coordinate              */
  DBfloat  tang1_x;      /* Start tangent X component               */
  DBfloat  tang1_y;      /* Start tangent Y component               */
  DBfloat  tang2_x;      /* End   tangent X component               */
  DBfloat  tang2_y;      /* End   tangent Y component               */

  DBfloat  centre1_x;    /* Centre circel 1 X coordinate            */
  DBfloat  centre1_y;    /* Centre circel 1 Y coordinate            */
  DBfloat  centre2_x;    /* Centre circel 2 X coordinate            */
  DBfloat  centre2_y;    /* Centre circel 2 Y coordinate            */
  DBfloat  circle1_r;    /* Radius for circle 1                     */
  DBfloat  circle2_r;    /* Radius for circle 2                     */
  DBfloat  angle_1;      /* Angle for circle 1                      */
  DBfloat  angle_2;      /* Angle for circle 2                      */
  DBfloat  poiout_x;     /* End/start point circles X coord.        */
  DBfloat  poiout_y;     /* End/start point circles Y coord.        */
  DBfloat  tangout_x;    /* Tangent for poiout_x,poiout_y           */
  DBfloat  tangout_y;    /* Tangent for poiout_x,poiout_y           */

  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBfloat  pvalue;       /* P value for the rational segment        */

  DBfloat  u_che;        /* Parameter value for check point         */
  DBfloat  d_che;        /* Delta parameter value for check points  */
  DBSeg    s_che[2];     /* Segment data to circles                 */
  DBfloat  max_dev;      /* Maximum deviation                       */
  DBfloat  dist_cur;     /* Distance between circles and input crv  */
  DBint    i_che;        /* Loop index check point                  */
  DBVector extpt;        /* External check point                    */
  short    noinse;       /* Number of closest pts from one segment  */
  DBfloat  useg[INTMAX]; /* The unordered array of u solutions      */

  DBint    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat Enter u_start %f u_end %f \n",u_start,  u_end );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Initialization of output variables.                              */
/*                                                                 !*/
   *p_no_cir   = I_UNDEF;  
   *p_max_dev  = F_UNDEF;

/*!                                                                 */
/* Initialization of local variables.                               */
/*                                                                 !*/

   poi_s_b.x_gm    = F_UNDEF;
   poi_s_b.y_gm    = F_UNDEF;
   poi_s_b.z_gm    = F_UNDEF;

   poi_s_l.x_gm    = F_UNDEF;
   poi_s_l.y_gm    = F_UNDEF;
   poi_s_l.z_gm    = F_UNDEF;

   poi_e_b.x_gm    = F_UNDEF;
   poi_e_b.y_gm    = F_UNDEF;
   poi_e_b.z_gm    = F_UNDEF;

   poi_e_l.x_gm    = F_UNDEF;
   poi_e_l.y_gm    = F_UNDEF;
   poi_e_l.z_gm    = F_UNDEF;

   tan_s_b.x_gm    = F_UNDEF;
   tan_s_b.y_gm    = F_UNDEF;
   tan_s_b.z_gm    = F_UNDEF;

   tan_s_l.x_gm    = F_UNDEF;
   tan_s_l.y_gm    = F_UNDEF;
   tan_s_l.z_gm    = F_UNDEF;

   tan_e_b.x_gm    = F_UNDEF;
   tan_e_b.y_gm    = F_UNDEF;
   tan_e_b.z_gm    = F_UNDEF;

   tan_e_l.x_gm    = F_UNDEF;
   tan_e_l.y_gm    = F_UNDEF;
   tan_e_l.z_gm    = F_UNDEF;

   poi_m_b.x_gm    = F_UNDEF;
   poi_m_b.y_gm    = F_UNDEF;
   poi_m_b.z_gm    = F_UNDEF;

   tan_m_b.x_gm    = F_UNDEF;
   tan_m_b.y_gm    = F_UNDEF;
   tan_m_b.z_gm    = F_UNDEF;

   poi_m_l.x_gm    = F_UNDEF;
   poi_m_l.y_gm    = F_UNDEF;
   poi_m_l.z_gm    = F_UNDEF;

   tan_m_l.x_gm    = F_UNDEF;
   tan_m_l.y_gm    = F_UNDEF;
   tan_m_l.z_gm    = F_UNDEF;

   extpt.x_gm     = F_UNDEF;
   extpt.y_gm     = F_UNDEF;
   extpt.z_gm     = F_UNDEF;

   noinse         = I_UNDEF;

   poi1_x         = F_UNDEF;
   poi1_y         = F_UNDEF;
   poi2_x         = F_UNDEF;
   poi2_y         = F_UNDEF;
   tang1_x        = F_UNDEF;
   tang1_y        = F_UNDEF;
   tang2_x        = F_UNDEF;
   tang2_y        = F_UNDEF;

   centre1_x      =  F_UNDEF;
   centre1_y      =  F_UNDEF;
   centre2_x      =  F_UNDEF;
   centre2_y      =  F_UNDEF;
   circle1_r      =  F_UNDEF;
   circle2_r      =  F_UNDEF;
   angle_1        =  F_UNDEF; 
   angle_2        =  F_UNDEF;
   poiout_x       =  F_UNDEF;
   poiout_y       =  F_UNDEF;
   tangout_x      =  F_UNDEF;
   tangout_y      =  F_UNDEF;

   points[0].x_gm =  F_UNDEF;
   points[0].y_gm =  F_UNDEF;
   points[0].z_gm =  F_UNDEF;
   points[1].x_gm =  F_UNDEF;
   points[1].y_gm =  F_UNDEF;
   points[1].z_gm =  F_UNDEF;
   points[2].x_gm =  F_UNDEF;
   points[2].y_gm =  F_UNDEF;
   points[2].z_gm =  F_UNDEF;
   points[3].x_gm =  F_UNDEF;
   points[3].y_gm =  F_UNDEF;
   points[3].z_gm =  F_UNDEF;
   pvalue         =  F_UNDEF;

   u_che          =  F_UNDEF;
   d_che          =  F_UNDEF;
   max_dev        =  F_UNDEF;
   dist_cur       =  F_UNDEF;
   i_che          =  I_UNDEF;

/* Evaluation of coordinates and tangents                           */
   xyz_s.evltyp   = EVC_DR;
   xyz_e.evltyp   = EVC_DR;

/*!                                                                 */
/* Calculate start and end points and tangents on curve.            */
/* Call of varkon_GE109 (GE109).                                  */
/*                                                                 !*/

       xyz_s.t_global = u_start;   

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 Start%%(sur726*cre_rat)");
         return(varkon_erpush("SU2943",errbuf));
         }

       xyz_e.t_global = u_end;   

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_e);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 End%%(sur726*cre_rat)");
         return(varkon_erpush("SU2943",errbuf));
         }
/*       kappa     =  xyz_s.kappa;   */




/*!                                                                 */
/* Transformate points and tangents to the local curve system.      */
/* Calls of GEtfpos_to_local and GEtfvec_to_local                   */
/*                                                                 !*/

   poi_s_b = xyz_s.r;

   status=GEtfpos_to_local
   (&poi_s_b , &cur_csys , &poi_s_l);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

   poi_e_b = xyz_e.r;

   status=GEtfpos_to_local
   (&poi_e_b , &cur_csys , &poi_e_l);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

   tan_s_b = xyz_s.drdt;

   status=GEtfvec_to_local
   (&tan_s_b , &cur_csys , &tan_s_l);
#ifdef  DEBUG
   if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

   tan_e_b = xyz_e.drdt;

   status=GEtfvec_to_local
   (&tan_e_b , &cur_csys , &tan_e_l);
#ifdef  DEBUG
   if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat  poi_s_l %f %f %f\n",
 poi_s_l.x_gm,  poi_s_l.y_gm,  poi_s_l.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat  tan_s_l %f %f %f\n",
 tan_s_l.x_gm,  tan_s_l.y_gm,  tan_s_l.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat  poi_e_l %f %f %f\n",
 poi_e_l.x_gm,  poi_e_l.y_gm,  poi_e_l.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat  tan_e_l %f %f %f\n",
 tan_e_l.x_gm,  tan_e_l.y_gm,  tan_e_l.z_gm );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Calculate two 2D cicle segments (or one line).                   */
/* Call of varkon_cir_twoseg (sur705).                              */
/*                                                                 !*/

/* Start point and end points to local variables.                   */
   poi1_x         = poi_s_l.x_gm;
   poi1_y         = poi_s_l.y_gm;
   tang1_x        = tan_s_l.x_gm;
   tang1_y        = tan_s_l.y_gm;
   poi2_x         = poi_e_l.x_gm;
   poi2_y         = poi_e_l.y_gm;
   tang2_x        = tan_e_l.x_gm;
   tang2_y        = tan_e_l.y_gm;


   status=varkon_cir_twoseg
    (poi1_x, poi1_y, poi2_x, poi2_y, 
     tang1_x, tang1_y, tang2_x, tang2_y,
     &centre1_x, &centre1_y, &centre2_x, &centre2_y, 
     &circle1_r, &circle2_r, &angle_1, &angle_2,
     &poiout_x, &poiout_y, &tangout_x, &tangout_y);
   if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur705*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* Transformate output (mid) point and tangent to the Basic system  */
/* Call of GEtfpos_to_local and GEtfvec_to_local                    */
/*                                                                 !*/

   poi_m_l.x_gm    = poiout_x;
   poi_m_l.y_gm    = poiout_y;
   poi_m_l.z_gm    = poi_s_l.z_gm; /* Should zero (0) */

   tan_m_l.x_gm    = tangout_x;
   tan_m_l.y_gm    = tangout_y;
   tan_m_l.z_gm    = poi_s_l.z_gm; /* Should zero (0) */

   status=GEtfpos_to_local
   (&poi_m_l , &cur_csys_inv , &poi_m_b);
#ifdef  DEBUG
if (status<0) 
     {
     sprintf(errbuf,"GEtfpos_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif


   status=GEtfvec_to_local
   (&tan_m_l , &cur_csys_inv , &tan_m_b);
#ifdef  DEBUG
   if (status<0) 
     {
     sprintf(errbuf,"GEtfvec_to_local%%sur726*ini_che");
     return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* Create one or two curve segments.                                */
/* Call of GE133.                                                   */
/*                                                                 !*/

/* Calculate P-value from the output angles.                        */


   if   ( angle_1 < 0.0 && angle_2 < 0.0 )/* Flag for straight line */
     {
     *p_no_cir = 1;
     points[0].x_gm = poi_s_b.x_gm;
     points[0].y_gm = poi_s_b.y_gm;
     points[0].z_gm = poi_s_b.z_gm;
     points[1].x_gm = poi_s_b.x_gm+10.0*tan_s_b.x_gm;
     points[1].y_gm = poi_s_b.y_gm+10.0*tan_s_b.y_gm;
     points[1].z_gm = poi_s_b.z_gm+10.0*tan_s_b.z_gm;
     points[2].x_gm = poi_e_b.x_gm+10.0*tan_e_b.x_gm;
     points[2].y_gm = poi_e_b.y_gm+10.0*tan_e_b.y_gm;
     points[2].z_gm = poi_e_b.z_gm+10.0*tan_e_b.z_gm;
     points[3].x_gm = poi_e_b.x_gm;
     points[3].y_gm = poi_e_b.y_gm;
     points[3].z_gm = poi_e_b.z_gm;
     pvalue = 0.5; 
     status=GE133 (points,pvalue,&cir1 );
     if (status<0) 
       {
       sprintf(errbuf,"GE133 lin%%(sur726*cre_rat)");
       return(varkon_erpush("SU2943",errbuf));
       }
     V3MOME((char *)(&cir1),(char *)(&s_che[0]),sizeof(DBSeg));
     cur_dummy.ns_cu       =   1;
     }   /* End one line */
   else
     {
     *p_no_cir = 2;
     points[0].x_gm = poi_s_b.x_gm;
     points[0].y_gm = poi_s_b.y_gm;
     points[0].z_gm = poi_s_b.z_gm;
     points[1].x_gm = poi_s_b.x_gm+10.0*tan_s_b.x_gm;
     points[1].y_gm = poi_s_b.y_gm+10.0*tan_s_b.y_gm;
     points[1].z_gm = poi_s_b.z_gm+10.0*tan_s_b.z_gm;
     points[2].x_gm = poi_m_b.x_gm+10.0*tan_m_b.x_gm;
     points[2].y_gm = poi_m_b.y_gm+10.0*tan_m_b.y_gm;
     points[2].z_gm = poi_m_b.z_gm+10.0*tan_m_b.z_gm;
     points[3].x_gm = poi_m_b.x_gm;
     points[3].y_gm = poi_m_b.y_gm;
     points[3].z_gm = poi_m_b.z_gm;
     pvalue = (1.0 - cos(angle_1/2.0)) / 
     (sqrt(1.0+tan(angle_1/2.0)* tan(angle_1/2.0))-cos(angle_1/2.0));
     status=GE133 (points,pvalue,&cir1 );
     if (status<0) 
       {
       sprintf(errbuf,"GE133 cir1%%(sur726*cre_rat)");
       return(varkon_erpush("SU2943",errbuf));
       }
     V3MOME((char *)(&cir1),(char *)(&s_che[0]),sizeof(DBSeg));

     points[0].x_gm = poi_m_b.x_gm;
     points[0].y_gm = poi_m_b.y_gm;
     points[0].z_gm = poi_m_b.z_gm;
     points[1].x_gm = poi_m_b.x_gm+10.0*tan_m_b.x_gm;
     points[1].y_gm = poi_m_b.y_gm+10.0*tan_m_b.y_gm;
     points[1].z_gm = poi_m_b.z_gm+10.0*tan_m_b.z_gm;
     points[2].x_gm = poi_e_b.x_gm+10.0*tan_e_b.x_gm;
     points[2].y_gm = poi_e_b.y_gm+10.0*tan_e_b.y_gm;
     points[2].z_gm = poi_e_b.z_gm+10.0*tan_e_b.z_gm;
     points[3].x_gm = poi_e_b.x_gm;
     points[3].y_gm = poi_e_b.y_gm;
     points[3].z_gm = poi_e_b.z_gm;
     pvalue = (1.0 - cos(angle_2/2.0)) / 
     (sqrt(1.0+tan(angle_2/2.0)* tan(angle_2/2.0))-cos(angle_2/2.0));
     status=GE133 (points,pvalue,&cir2 );
     if (status<0) 
       {
       sprintf(errbuf,"GE133 cir2%%(sur726*cre_rat)");
       return(varkon_erpush("SU2943",errbuf));
       }
     V3MOME((char *)(&cir2),(char *)(&s_che[1]),sizeof(DBSeg));
     cur_dummy.ns_cu       =   2;
     }   /* End two curve segments */


/*!                                                                 */
/* Calculate maximum distance between the output circle segments    */
/* and the input curve.                                             */
/* Calls of varkon_GE109 (GE109) and varkon_geo713 (GE713)       */
/*                                                                 !*/

/* Delta parameter value for check points.                          */
   d_che = (u_end-u_start)/(n_che+1);

/* Initialize maximum deviation                                     */
   max_dev  = -50000.0;

/* Start loop all check points                                      */
   for  ( i_che = 1; i_che <= n_che; ++i_che)
     {

/*   Current parameter value                                        */
     u_che = u_start + i_che*d_che;

/*   Current check point.                                           */
     xyz_s.t_global = u_che;    

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 Check%%(sur726*cre_rat)");
         return(varkon_erpush("SU2943",errbuf));
         }

/*   Calculate the closest point parameter value                    */
     extpt.x_gm = xyz_s.r.x_gm;
     extpt.y_gm = xyz_s.r.y_gm;
     extpt.z_gm = xyz_s.r.z_gm;

       status=GE713
       ((DBAny *)&cur_dummy,s_che,&extpt,&noinse,useg);

      if ( status < 0 || noinse <= 0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat  Failure geo713 u_che %f noinse %d status %d\n", 
      u_che,(int)noinse, (int)status );
fflush(dbgfil(SURPAC));
}
#endif
         goto no_close;
        }


/*   Current closest point                                          */
     xyz_e.t_global = useg[0];    

       status=GE109 ((DBAny *)&cur_dummy,s_che,&xyz_e);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 Close%%(sur726*cre_rat)");
         return(varkon_erpush("SU2943",errbuf));
         }

/*   Calculate distance                                             */

     dist_cur = sqrt(
     (extpt.x_gm - xyz_e.r.x_gm)*(extpt.x_gm - xyz_e.r.x_gm)+
     (extpt.y_gm - xyz_e.r.y_gm)*(extpt.y_gm - xyz_e.r.y_gm)+
     (extpt.z_gm - xyz_e.r.z_gm)*(extpt.z_gm - xyz_e.r.z_gm) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat noinse %d useg[0] %f dist_cur %f max_dev %f\n", 
      (int)noinse,  useg[0], dist_cur, max_dev);
fflush(dbgfil(SURPAC));
}
#endif

     if  (  dist_cur >  max_dev ) max_dev = dist_cur;

no_close:;

/* End   loop all check points                                      */
     } /* End loop check points */






   *p_max_dev  = max_dev; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur726*cre_rat u_start %6.4f u_end %6.4f *p_no_cir %d *p_max_dev %f\n", 
      u_start, u_end, (int)(*p_no_cir),  *p_max_dev );
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


