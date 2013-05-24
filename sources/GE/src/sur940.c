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
/*  Function: varkon_pat_uvcur                     File: sur940.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a U,V curve in one "patch".                */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-11   Originally written                                 */
/*  1994-10-14   varkon_erinit                   G. Liden           */
/*  1996-02-13   Exit = Entry point              G. Liden           */
/*  1996-02-23   icase= 4 for silhouettes, unused variables ....    */
/*  1996-04-27   b_case added                                       */
/*  1996-05-26   Multiple segments in the patch and trimming ...    */
/*  1996-06-09   Criterion for warning X 10.0, utol, bug UV interv. */
/*  1996-10-22   Debug                                              */
/*  1997-01-31   n_interv 8 --> 16                                  */
/*  1997-03-12   Debug                                              */
/*  1997-04-04   Bug: One short segment                             */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_uvcur      Create a UV curve in a patch     */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Initializations and checks         */
static short oneseg();        /* Only one short output segment      */
static short creseg();        /* Create current UV segment          */
static short newpoi();        /* Calculate new UV point in patch    */
static short chepoi();        /* Check distance to current segm.    */
static short uvtang();        /* The U,V vector for given UV point  */
static short fsilhou();       /* The U,V vector        for F_SILH   */
static short isilhou();       /* The U,V vector        for I_SILH   */
static short ssilhou();       /* The U,V vector        for S_SILH   */
/* static short fisopho(); */ /* The U,V vector        for F_ISOP   */
/* static short iisopho(); */ /* The U,V vector        for I_ISOP   */
static short intplan();       /* The U,V vector        for SURPLAN  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBSeg cur_seg;             /* Current rational segment       */
static UVPT  uvall[UVPTMAX];      /* All calculated UV points       */
static DBint n_uvpt;              /* No of records in table UVPT    */
static DBfloat l_ints;            /* Current start curve "parameter"*/
static DBfloat l_inte;            /* Current end   curve "parameter"*/
static DBfloat u_ints;            /* Current start U parameter      */
static DBfloat v_ints;            /* Current start V parameter      */
static DBfloat u_inte;            /* Current end   U parameter      */
static DBfloat v_inte;            /* Current end   V parameter      */
static DBint ctype;               /* Case of computation            */
static DBVector u_25;             /* Check point U= 0.25            */
static DBVector u_50;             /* Check point U= 0.50            */
static DBVector u_75;             /* Check point U= 0.75            */
static DBfloat idpoint;           /* Identical point criterion      */
static DBfloat m_idpoint;         /* Identical point criterion (mod)*/
static DBfloat max_dev;           /* Maximum deviation              */
static DBfloat idangle;           /* Identical angle criterion      */
static DBfloat comptol;           /* Computer   tolerance           */
static DBfloat utol;              /* Retrieve point U tolerance     */
static DBfloat pi_x,pi_y,pi_z;    /* Intersecting plane             */
static DBfloat pi_d;              /*                                */
static DBfloat eye_x,eye_y,eye_z; /* Eye point for reflection crvs  */
static DBfloat eyev_x,eyev_y;     /* Eye vect. for reflection crvs  */
static DBfloat eyev_z;            /*                                */
static DBVector sdir;             /* Direction for spine silhouette */
static DBfloat angle;             /* Angle in degrees for isophotes */
static DBfloat uv[2];             /* Point  in U,V plane            */
static DBfloat uvt[3];            /* Vector in U,V plane (note 3!)  */
static EVALS xyz;                 /* Coordinates and derivatives    */
                                  /* for a point on a surface       */
/*-----------------------------------------------------------------!*/
static DBCurve cur;               /* Curve                  GE143   */
static short nstart;              /* The number of restarts GE143   */
static EVALC xyz_c;               /* Point and derivatives  GE109   */
static DBint icase;               /* varkon_sur_eval derivat. flag  */
static DBint status;              /* Error code from a called fctn  */
#ifdef  DEBUG
static DBVector u_0;              /* Check point U= 0.00            */
static DBVector u_100;            /* Check point U= 1.00            */
#endif

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_sur_eval       * Surface evaluation routine     */
/*           varkon_normv          * Normalization of (3D) vector   */
/*           varkon_sur_num1       * Patch one parameter calculat.  */
/*           GE136                 * Rational cubic with interm. pt */
/*           GE133                 * Rational cubic with P-value    */
/*           GE143                 * Closest point on segment       */
/*           varkon_vec_projpla    * Project vector onto plane      */
/*           GE109                 * Curve evaluation routine       */
/*           varkon_ini_gmseg      * Initialize DBSeg               */
/*           varkon_ini_gmcur      * Initialize DBCurve             */
/*           varkon_ini_evalc      * Initialize EVALC               */
/*           varkon_erinit         * Initiate error message stack   */
/*           varkon_errmes         * Warning message to terminal    */
/*           varkon_erpush         * Error message to terminal      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2803 = Spine must be a straight line    varkon_sur_uvcur       */
/* SU2943 = Called function xxxxxx failed in varkon_pat_uvcur       */
/* SU2993 = Severe program error in varkon_pat_uvcur   (sur940).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus   varkon_pat_uvcur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   DBfloat  uls,         /* Patch limit start point                 */
   DBfloat  vls,         /* Patch limit start point                 */
   DBfloat  ule,         /* Patch limit end   point                 */
   DBfloat  vle,         /* Patch limit end   point                 */
   DBfloat  ustart,      /* Start U,V point for "patch" curve       */
   DBfloat  vstart,      /* Start U,V point for "patch" curve       */
   DBfloat  uend,        /* End   U,V point for "patch" curve       */
   DBfloat  vend,        /* End   U,V point for "patch" curve       */
   DBfloat  us_t,        /* Start U,V tangent to ustart,vstart      */
   DBfloat  vs_t,        /* Start U,V tangent to ustart,vstart      */
   DBfloat  ue_t,        /* End   U,V tangent to uend  ,vend        */
   DBfloat  ve_t,        /* End   U,V tangent to uend  ,vend        */
   DBint    start_seg,   /* Start segment number for curve in patch */
   DBint    max_seg,     /* Maximum number of segments for curve    */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Alloc. area for UV curve segments (ptr) */
                         /* (start adress for added segments)       */
                         /* Note that the DB pointer to the surface */
                         /* must be initialized in all segments !   */
   DBint   *p_nseg )     /* Number of output segments in the patch  */
/* Out:                                                             */
/*      Coefficients for added segments to DBSeg                    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_interv;     /* Loop index interval                     */
   DBint   n_interv;     /* Number of  intervals                    */
   DBfloat d_interv;     /* Delta      interval                     */
   DBint   f_check;      /* Flag for check of tolerances            */
                         /* Eq. 1: Check tolerances                 */
                         /* Eq. 2: No check of tolerances           */
                         /*        Warning to user !                */
   DBint   f_add;        /* Flag defining if segments must be added */
                         /* Eq. 0: Not necessary to add segments    */
                         /* Eq. 1: Add segments                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   no_iter;      /* Number of iterations                    */

   DBfloat dist_25;      /* Distance p_25 to current segment        */
   DBfloat dist_50;      /* Distance p_50 to current segment        */
   DBfloat dist_75;      /* Distance p_75 to current segment        */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* Call of initial.                                                 */
/*                                                                 !*/


   status = initial ( p_comp, ustart,vstart,uend,vend,
                      us_t,vs_t,ue_t,ve_t,p_seg );
   if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_pat_uvcur");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Calculate UV segments                                         */
/* ________________________                                         */
/*                                                                 !*/

/*!                                                                 */
/* Only one short segment ? Goto only_one for this case.            */
/* Call of internal function oneseg.                                */
/*                                                                 !*/


     status = oneseg (p_sur, p_pat , 
       ustart,vstart,uend,vend,us_t,vs_t,ue_t,ve_t,
        p_cur,p_seg,p_nseg);
     if      ( status < 0 ) 
       {
       sprintf(errbuf,"oneseg%%sur940");
       return(varkon_erpush("SU2973",errbuf));
       }

   if ( status == 1 ) 
     {
     max_dev = F_UNDEF;
     goto only_one;
     }

   n_interv = 0;
   no_iter  = 0;

   f_check  = 1;

addsegm:; /* Label: Segment is out of tolerance                     */
chord_0:; /* Label: Chord is zero                                   */

/* In order to get new segment start/end pts calculated             */
   f_add    = 0; 

#ifdef  DEBUG
   no_iter  = no_iter + 1; /* Programming check           */
   if ( no_iter > 10 )    
     {
     sprintf(errbuf,"no_iter%%sur940");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif  /*  End DEBUG  */


     max_dev = -50000.0;

   /* Ta bort medlem flagga i UVPT  som inte utnyttjas  */

   if ( n_interv >= 16 )  /*  1996-06-09  9->8 1997-01-31 8->16 */
     {
     /* Create segments and give warning tolerance that segments are out of tolerance */
     f_check = 2;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*Maximum number of intervals n_interv= %d f_check= %d\n",
                            (short)n_interv,    (short)f_check );
fflush(dbgfil(SURPAC)); 
}
#endif

     }

   if         ( f_check == 2 )
     ; /* Maximum number of segments already created */
   else if    ( n_interv <= 1  )
     {
     n_interv = n_interv + 1;
     }
   else
     {
     n_interv = 2*n_interv;
     }

   d_interv = 1.0/(DBfloat)n_interv;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*Start loop i_interv n_interv= %d d_interv= %f \n",
                            (short)n_interv,    d_interv );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940* f_add= %d f_check= %d \n",
                            (short)f_add, (short)f_check );
fflush(dbgfil(SURPAC)); 
}
#endif

   if  ( start_seg + 1 + n_interv > max_seg )
   {
     sprintf(errbuf,"%d%% ",(short)max_seg);
     varkon_erinit();
     return(varkon_erpush("SU2193",errbuf));
   }


   for  ( i_interv = 1; i_interv <= n_interv; ++i_interv )
     {

     l_ints = (DBfloat)(i_interv-1)*d_interv;   /* Start "parameter"  */
     l_inte = (DBfloat)(i_interv)  *d_interv;   /* End   "parameter"  */

/*!                                                                 */
/*   Create current UV segment. Call of creseg.                     */
/*   Restart with previous n_interv if chord= 0.                    */
/*                                                                 !*/

     status = creseg (p_sur, p_pat , p_comp ,uls,vls,ule,vle );
     if      ( status == -1 ) 
       {
       if  ( n_interv > 1 )
         {
         n_interv = n_interv/2;
         f_check  = 2;
         goto  chord_0;
         }
       else 
         {
         sprintf(errbuf,"chord=0%%varkon_pat_uvcur");
         return(varkon_erpush("SU2993",errbuf));
         }
       }
     else if ( status <   0 ) 
        {
        sprintf(errbuf,"creseg%%varkon_pat_uvcur");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/*   Add current UV segment to output area p_seg                    */
/*                                                                 !*/

/*   Output number of segments                                      */
     *p_nseg = i_interv;

     cur_seg.typ = UV_CUB_SEG ;

 V3MOME((char *)(&cur_seg),(char *)(p_seg+*p_nseg-1),sizeof(DBSeg));

     if ( f_add == 1 ) goto  segcalc; /* No check, just calculate   */

/*!                                                                 */
/*   Distance between check points and current rational segment.    */
/*   Calls of chepoi.                                               */
/*   Goto addsegm if out of tolerance m_idpoint.                    */
/*                                                                 !*/

     cur_seg.typ = UV_CUB_SEG ;
     status = chepoi (p_sur,p_pat,u_25, &dist_25 );
     if      ( status < 0 && f_check != 2 ) f_add = 1;      
     else if ( status < 0 )                 dist_25 = 1.23456789;
     if ( dist_25 > max_dev   ) max_dev = dist_25;
     if ( dist_25 > m_idpoint && f_check != 2 ) f_add = 1;      

     cur_seg.typ = UV_CUB_SEG ;
     status = chepoi (p_sur,p_pat,u_75, &dist_75 );
     if      ( status < 0 && f_check != 2 ) f_add = 1;      
     else if ( status < 0 )                 dist_75 = 1.23456789;
     if ( dist_75 > max_dev   ) max_dev = dist_75;
     if ( dist_75 > m_idpoint && f_check != 2 ) f_add = 1;      

     cur_seg.typ = UV_CUB_SEG ;
     status = chepoi (p_sur,p_pat,u_50, &dist_50 );
     if      ( status < 0 && f_check != 2 ) goto  addsegm;
     else if ( status < 0 )                 dist_50 = 1.23456789;
     if ( dist_50 > max_dev   ) max_dev = dist_50;
     if ( dist_50 > m_idpoint && f_check != 2 ) f_add = 1;      

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && f_check != 2 )
{
fprintf(dbgfil(SURPAC),
"sur940 Inside dist_25 %14.10f dist_50 %14.10f dist_75 %14.10f f_add %d\n",
        dist_25 ,       dist_50 ,       dist_75, (short)f_add );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && f_check == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940 Check  dist_25 %14.10f dist_50 %14.10f dist_75 %14.10f f_add %d\n",
        dist_25 ,       dist_50 ,       dist_75, (short)f_add );
fflush(dbgfil(SURPAC)); 
}
#endif

segcalc:; /* Label: Only calculate start/end points, no check           */

     if ( i_interv == n_interv && f_add == 1 ) goto  addsegm;

     } /* End loop intervals */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && f_check == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940 All segments *p_nseg= %d are inside tolerance m_idpoint %20.18f \n",
        (short)*p_nseg, m_idpoint  );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && f_check == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940 Some segments are outside tolerance m_idpoint %20.18f \n", m_idpoint  );
fflush(dbgfil(SURPAC));
}
#endif

/* 3. Exit                                                          */
/* _______                                                          */
 
   if (  f_check == 2 && max_dev > 10.0*idpoint )    
     {
     sprintf(errbuf,"%4.1f%%%4.1f",
           (DBfloat)start_seg,(DBfloat)start_seg+*p_nseg);
     varkon_erinit();
     varkon_erpush("SU2201",errbuf);
     varkon_errmes();
     }

only_one:; /*! Label: Only one short segment !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*Exit*varkon_pat_uvcur Segments %d Max. deviation %20.10f\n",
   (short)*p_nseg, max_dev );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!************** Internal ** function ** uvtang ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for a given UV point                                            */
/*                                                                  */
               static  short   uvtang (p_sur,p_pat)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Pointer to the input surface            */
   DBPatch *p_pat;       /* Pointer to alloc. area for patch data   */
/*      The point  uv[2]                                            */
/* Out: The vector uvt[2]                                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/*!                                                                 */
/* Calculate the tangent for the derivatives EVALS (xyz)            */
/* Call of varkon_sur_eval (sur210)                                 */
/*                                                                 !*/


status=varkon_sur_eval
   (p_sur,p_pat,icase,uv[0] ,uv[1] ,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_pat_uvcur ");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Calculate the tangent for the derivatives EVALS (xyz)            */
/* Call of fsilhou for ctype = F_SILH                               */
/* Call of isilhou for ctype = I_SILH                               */
/* Call of ssilhou for ctype = S_SILH                               */
/* Call of fisopho for ctype = F_ISOP                               */
/* Call of iisopho for ctype = I_ISOP                               */
/* Call of intplan for ctype = SURPLAN                              */
/* (Error SU2973 if internal function fails)                        */
/*                                                                 !*/

   if      ( ctype  == F_SILH  )
     {
     status=fsilhou(); 
     if (status<0) 
        {
        sprintf(errbuf,"fsilhou%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == I_SILH  )
     {
     status=isilhou();
     if (status<0) 
        {
        sprintf(errbuf,"isilhou%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == S_SILH  )
     {
     status=ssilhou();
     if (status<0) 
        {
        sprintf(errbuf,"ssilhou%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == F_ISOP  )
     {
    /*     status=intplan();   */
     if (status<0) 
        {
        sprintf(errbuf,"fisopho%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == I_ISOP  )
     {
     /*    status=intplan();   */
     if (status<0) 
        {
        sprintf(errbuf,"iisopho%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == SURPLAN )
     {
     status=intplan();
     if (status<0) 
        {
        sprintf(errbuf,"intplan%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else
       {
       sprintf(errbuf,"(ctype m)%% varkon_pat_uvcur        ");
       return(varkon_erpush("SU2993",errbuf));
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*uvtang  uv %f %f uvt= %12.10f %12.10f\n"
   , uv[0], uv[1], uvt[0], uvt[1]       );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!************** Internal ** function ** isilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for calculation case:  I_SILH                                   */
/*                                                                  */
               static  short   isilhou()
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The eye vector eyev_x, eyev_y, eyev_z                        */
/* Out: The vector uvt                                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  dvdu;        /* Derivative dV/dU                        */
 
/*----------------------------------------------------------------- */

   DBfloat  denom;       /* Denominator for dV/dU                   */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Denominator:                                                     */
 denom  = xyz.nv_x*eyev_x  + xyz.nv_y*eyev_y  + xyz.nv_z*eyev_z;    
/*  ?????     :                                                     */
 dvdu   = xyz.nu_x*eyev_x + xyz.nu_y*eyev_y + xyz.nu_z*eyev_z; 
/*                                                                 !*/

 if (fabs(denom) > TOL1 )
   {
   uvt[0] =  -denom;
   uvt[1] =   dvdu;
   uvt[2] =   0.0; 
   dvdu   =  -dvdu/denom;
   }
 else
   {
   uvt[0] =   denom;
   uvt[1] =  -dvdu;
   uvt[2] =   0.0; 
   dvdu   =   500000.0;
   }

/*!                                                                 */
/* Normalization (vector with 3 components):                        */
   varkon_normv(uvt);                                                  
/*                                                                 !*/

   return(SUCCED);


} /* End of function */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** ssilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
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
/*     The vector  sdir.                                            */
/* Out: The vector uvt                                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector plane;        /* Plane  normal                           */
  DBint    ocase;        /* Output case. Eq. 1: Normalised vector   */
  DBVector psdir;        /* U crv tangent projected in spine plane  */
  DBfloat  dvdu;         /* Derivative dV/dU                        */
 
/*----------------------------------------------------------------- */

   DBfloat sumder;       /* Sum of second derivatives for spine     */
   DBfloat denom;        /* Denominator for dV/dU                   */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

   sumder =fabs(xyz.spt2_x) +
           fabs(xyz.spt2_y) +
           fabs(xyz.spt2_z);

  if ( sumder > 0.000001 )
        {
        sprintf(errbuf,"ssilhou%%varkon_pat_uvcur ");
       varkon_erinit();
        return(varkon_erpush("SU2803",errbuf));
        }

   plane.x_gm= xyz.spt_x; 
   plane.y_gm= xyz.spt_y; 
   plane.z_gm= xyz.spt_z;

   ocase= 1;      
       status= varkon_vec_projpla
       ( sdir, plane, ocase, &psdir );
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_pat_uvcur        ");
        return(varkon_erpush("SU2943",errbuf));
        }

/* Denominator:                                                     */
 denom= xyz.nv_x*psdir.x_gm+xyz.nv_y*psdir.y_gm+xyz.nv_z*psdir.z_gm;   
/*                                                                 !*/

/*!                                                                 */
/*  ?????     :                                                     */
 dvdu= xyz.nu_x*psdir.x_gm+xyz.nu_y*psdir.y_gm+xyz.nu_z*psdir.z_gm;
/*                                                                 !*/

 if (fabs(denom) > TOL1 )
   {
   uvt[0] =  -denom;
   uvt[1] =   dvdu;
   uvt[2] =   0.0; 
   dvdu   =  -dvdu/denom;
   }
 else
   {
   uvt[0] =   denom;
   uvt[1] =  -dvdu;
   uvt[2] =   0.0; 
   dvdu   =   500000.0;
   }

/*!                                                                 */
/* Normalization (vector with 3 components):                        */
/* Call of varkon_normv (normv).                                    */
/*                                                                 !*/
   status= varkon_normv(uvt);
#ifdef  DEBUG
       if (status<0) 
        {
        sprintf(errbuf,"varkon_normv%%varkon_pat_uvcur");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif



   return(SUCCED);


} /* End of function */

/********************************************************************/

/*!************** Internal ** function ** fsilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for calculation case:  F_SILH                                   */
/*                                                                  */
               static  short   fsilhou()
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The eye point  eye_x, eye_y, eye_z                           */
/* Out: The vector uvt                                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  dvdu;        /* Derivative dV/dU                        */
 
/*----------------------------------------------------------------- */

   DBfloat  denom;       /* Denominator for dV/dU                   */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Denominator:                                                     */
 denom  = xyz.nv_x*xyz.r_x + xyz.nv_y*xyz.r_y + xyz.nv_z*xyz.r_z   
        + xyz.n_x *xyz.v_x + xyz.n_y *xyz.v_y + xyz.n_z *xyz.v_z   
        - xyz.nv_x*eye_x   - xyz.nv_y*eye_y   - xyz.nv_z*eye_z;    
/*  ?????     :                                                     */
 dvdu   = xyz.nu_x*xyz.r_x + xyz.nu_y*xyz.r_y + xyz.nu_z*xyz.r_z   
        + xyz.n_x *xyz.u_x + xyz.n_y *xyz.u_y + xyz.n_z *xyz.u_z   
        - xyz.nu_x*eye_x   - xyz.nu_y*eye_y   - xyz.nu_z*eye_z;    
/*                                                                 !*/

 if (fabs(denom) > TOL1 )
   {
   uvt[0] =  -denom;
   uvt[1] =   dvdu;
   uvt[2] =   0.0; 
   dvdu   =  -dvdu/denom;
   }
 else
   {
   uvt[0] =   denom;
   uvt[1] =  -dvdu;
   uvt[2] =   0.0; 
   dvdu   =   500000.0;
   }

/*!                                                                 */
/* Normalization (vector with 3 components):                        */
   varkon_normv(uvt);                                                  
/*                                                                 !*/

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
/*  The function calculates the vector direction in the UV plane    */
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
/*     The intersecting plane pi_x,pi_y,pi_z,pi_d                   */
/* Out: The vector uvt                                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  dvdu;        /* Derivative dV/dU                        */
 
/*----------------------------------------------------------------- */

   DBfloat  denom;       /* Denominator for dV/dU                   */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/* Denominator:                                                     */
 denom  = xyz.v_x*pi_x + xyz.v_y*pi_y + xyz.v_z*pi_z;    
/*                                                                 !*/

/*!                                                                 */
/*  ?????     :                                                     */
 dvdu   = xyz.u_x*pi_x + xyz.u_y*pi_y + xyz.u_z*pi_z;    
/*                                                                 !*/

 if (fabs(denom) > TOL1 )
   {
   uvt[0] =  -denom;
   uvt[1] =   dvdu;
   uvt[2] =   0.0; 
   dvdu   =  -dvdu/denom;
   }
 else
   {
   uvt[0] =   denom;
   uvt[1] =  -dvdu;
   uvt[2] =   0.0; 
   dvdu   =   500000.0;
   }

/*!                                                                 */
/* Normalization (vector with 3 components):                        */
   varkon_normv(uvt);                                                  
/*                                                                 !*/

   return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** initial********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Initializations and checks of input data                        */
/*                                                                  */
    static  short   initial ( p_comp, ustart,vstart,uend,vend,
                          us_t,vs_t,ue_t,ve_t,p_seg )
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   IRUNON *p_comp;       /* Computation data                  (ptr) */
   DBfloat ustart,vstart;/* Start U,V point for "patch" curve       */
   DBfloat uend  ,vend;  /* End   U,V point for "patch" curve       */
   DBfloat us_t,vs_t;    /* Start U,V tangent to ustart,vstart      */
   DBfloat ue_t,ve_t;    /* End   U,V tangent to uend  ,vend        */
   DBSeg  *p_seg;        /* Alloc. area for UV curve segments (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */

   DBint   i_r;          /* Loop index UVPT record                  */
#ifdef  DEBUG
   DBfloat dist;         /* Distance between start and end point    */
#endif
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Initialization of cur and nstart= 3                              */
/*                                                                 !*/

   varkon_ini_gmcur   (  &cur ); 

   cur.ns_cu       =   1;
   cur.hed_cu.type = CURTYP;

   nstart = 3;

/*!                                                                 */
/* Initialize variabel EVALC                                        */
/* Call of varkon_ini_evals (sur776).                               */
/*                                                                 !*/

   varkon_ini_evalc (&xyz_c);

   xyz_c.evltyp   = EVC_R; 

/*!                                                                 */
/* Initialization of table UVPT and n_uvpt= 0                       */
/*                                                                 !*/

 for ( i_r = 0; i_r < UVPTMAX; ++i_r )
   {
   uvall[i_r].f = I_UNDEF;        /* Flag for the UV point          */
                                  /* <   0: Not defined             */
                                  /* Eq. 1: Defined                 */
                                  /* Eq. 2: Used as start point     */
                                  /* Eq. 3: Used as end   point     */
   uvall[i_r].l_interv = F_UNDEF; /* Curve "parameter"              */
   uvall[i_r].u        = F_UNDEF; /* U value                        */
   uvall[i_r].v        = F_UNDEF; /* V value                        */
   uvall[i_r].tu       = F_UNDEF; /* Tangent U                      */
   uvall[i_r].tv       = F_UNDEF; /* Tangent V                      */
   }

 n_uvpt = 0;

#ifdef DEBUG
  if ( p_seg->spek_gm == DBNULL )
    {
    sprintf(errbuf,"speg_gm=NULL%%sur940");
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

/*!                                                                 */
/* Initialization of cur_seg (DBSeg) from input segments !          */
/*                                                                 !*/

 V3MOME((char *)(p_seg),(char *)(&cur_seg),sizeof(DBSeg));

/*!                                                                 */
/* Case of computation ctype from input IRUNON  (p_comp)            */
/* Check distance between start and end point for Debug On          */
/* Check also that ustart and vstart >= 1.0                         */
/*                                                                 !*/

   ctype   = p_comp->ipl_un.ctype;

#ifdef DEBUG
  dist = SQRT( (ustart-uend)*(ustart-uend)  +
               (vstart-vend)*(vstart-vend));  
  if ( dist < 0.0001 )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940  ustart %f vstart %f dist %20.10f < 0.0001 \n"
   , ustart, vstart, dist );
fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940  uend   %f vend   %f \n"
   , uend  , vend );
fflush(dbgfil(SURPAC)); 
  }
       sprintf(errbuf,"dist< 0.0001%%sur940");
       return(varkon_erpush("SU2993",errbuf));
    }

  if ( ustart < 1.0 || vstart < 1.0 )
    {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940  ustart %20.15f vstart %20.15f < 1.0 \n"
   , ustart, vstart );
fflush(dbgfil(SURPAC)); 
  }
       sprintf(errbuf,"ustart or vstart< 1.0 %%sur940");
       return(varkon_erpush("SU2993",errbuf));
    }

#endif


/*!                                                                 */
/* Retrieve computation data for the input computation case         */
/* (for instance the intersection plane for ctype= SURPLAN)         */
/*                                                                 !*/

   if      ( ctype  == F_SILH  )
       {
       eye_x     = p_comp->sil_un.eye_x ;
       eye_y     = p_comp->sil_un.eye_y ;
       eye_z     = p_comp->sil_un.eye_z ;
       idpoint   = p_comp->ipl_un.idpoint;
       m_idpoint = idpoint;
       idangle   = p_comp->ipl_un.idangle;
       comptol   = p_comp->ipl_un.comptol;
       icase     = 4;
       }
   else if ( ctype  == I_SILH  )
       {
       eyev_x    = p_comp->sil_un.eyev_x ;
       eyev_y    = p_comp->sil_un.eyev_y ;
       eyev_z    = p_comp->sil_un.eyev_z ;
       idpoint   = p_comp->ipl_un.idpoint;
       m_idpoint = idpoint;
       idangle   = p_comp->ipl_un.idangle;
       comptol   = p_comp->ipl_un.comptol;
       icase     = 4;
       }
   else if ( ctype  == S_SILH  )
       {
       sdir.x_gm    = p_comp->sil_un.eyev_x ;
       sdir.y_gm    = p_comp->sil_un.eyev_y ;
       sdir.z_gm    = p_comp->sil_un.eyev_z ;
       idpoint      = p_comp->ipl_un.idpoint;
       m_idpoint    = idpoint*100.0;
       idangle      = p_comp->ipl_un.idangle;
       comptol      = p_comp->ipl_un.comptol;
       }
   else if ( ctype  == F_ISOP  )
       {
       eye_x     = p_comp->iso_un.eye_x ;
       eye_y     = p_comp->iso_un.eye_y ;
       eye_z     = p_comp->iso_un.eye_z ;
       angle     = p_comp->iso_un.angle ;
       idpoint   = p_comp->ipl_un.idpoint;
       m_idpoint = idpoint;
       idangle   = p_comp->ipl_un.idangle;
       comptol   = p_comp->ipl_un.comptol;
       icase = 4;
       }
   else if ( ctype  == I_ISOP  )
       {
       eye_x     = p_comp->iso_un.eye_x ;
       eye_y     = p_comp->iso_un.eye_y ;
       eye_z     = p_comp->iso_un.eye_z ;
       angle     = p_comp->iso_un.angle ;
       idpoint   = p_comp->ipl_un.idpoint;
       m_idpoint = idpoint;
       idangle   = p_comp->ipl_un.idangle;
       comptol   = p_comp->ipl_un.comptol;
       icase = 4;
       }
   else if ( ctype  == SURPLAN )
       {
       pi_x      = p_comp->ipl_un.in_x;
       pi_y      = p_comp->ipl_un.in_y;
       pi_z      = p_comp->ipl_un.in_z;
       pi_d      = p_comp->ipl_un.in_d;
       idpoint   = p_comp->ipl_un.idpoint;
       m_idpoint = idpoint/10.0;     /* For other interr. cases ? */
       idangle   = p_comp->ipl_un.idangle;
       comptol   = p_comp->ipl_un.comptol;
       icase     = 3;
       }
   else
       {
       sprintf(errbuf,"(ctype)%% varkon_pat_uvcur");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* Criterion for equal points (utol) when previously calculated     */
/* points are retrieved from table UVPT.                            */
/*                                                                 !*/

   utol = 10.0*comptol;

/*!                                                                 */
/* First two records to table UVPT (n_uvpt= 2)                      */
/*                                                                 !*/

   n_uvpt = n_uvpt + 1;               /* Additional record in UVPT  */

   uvall[n_uvpt-1].f        = 1;      /* Flag for the UV point      */
   uvall[n_uvpt-1].l_interv = 0.0;    /* Curve "parameter"          */
   uvall[n_uvpt-1].u        = ustart; /* U value                    */
   uvall[n_uvpt-1].v        = vstart; /* V value                    */
   uvall[n_uvpt-1].tu       = us_t;   /* Tangent U                  */
   uvall[n_uvpt-1].tv       = vs_t;   /* Tangent V                  */

   n_uvpt = n_uvpt + 1;               /* Additional record in UVPT  */

   uvall[n_uvpt-1].f        = 1;      /* Flag for the UV point      */
   uvall[n_uvpt-1].l_interv = 1.0;    /* Curve "parameter"          */
   uvall[n_uvpt-1].u        = uend;   /* U value                    */
   uvall[n_uvpt-1].v        = vend;   /* V value                    */
   uvall[n_uvpt-1].tu       = ue_t;   /* Tangent U                  */
   uvall[n_uvpt-1].tv       = ve_t;   /* Tangent V                  */

/*!                                                                 */
/* Current (start) interval start and end parameter values          */
/*                                                                 !*/

   l_ints = 0.0;                  /* Current start curve "parameter"*/
   l_inte = 0.0;                  /* Current end   curve "parameter"*/
   u_ints = ustart;               /* Current start U parameter      */
   v_ints = vstart;               /* Current start V parameter      */
   u_inte = uend;                 /* Current end   U parameter      */
   v_inte = vend;                 /* Current end   V parameter      */


   u_25.x_gm = F_UNDEF;
   u_25.y_gm = F_UNDEF;
   u_25.z_gm = F_UNDEF;
   u_50.x_gm = F_UNDEF;
   u_50.y_gm = F_UNDEF;
   u_50.z_gm = F_UNDEF;
   u_75.x_gm = F_UNDEF;
   u_75.y_gm = F_UNDEF;
   u_75.z_gm = F_UNDEF;

#ifdef  DEBUG
   u_0.x_gm   = F_UNDEF;
   u_0.y_gm   = F_UNDEF;
   u_0.z_gm   = F_UNDEF;
   u_100.x_gm = F_UNDEF;
   u_100.y_gm = F_UNDEF;
   u_100.z_gm = F_UNDEF;
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*initial idpoint %15.10f m_idpoint %15.10f\n", 
                  idpoint,  m_idpoint );
fflush(dbgfil(SURPAC)); 
  }
#endif


       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** creseg ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Create one rational segment and calculate two checkpoints       */
/*                                                                  */
    static  short   creseg  (p_sur, p_pat, p_comp ,uls,vls,ule,vle  )
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBfloat  uls,vls;     /* Patch limit start point                 */
   DBfloat  ule,vle;     /* Patch limit end   point                 */
/* Out:                                                             */
/*                                                                  */
/* Value (error code) of function creseg = -1 for chord = 0         */
/* (Criterion for chord= 0 defined in GE133)                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector ipt;         /* Intermediate point                      */
 
/*----------------------------------------------------------------- */

   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   DBint   i_r;          /* Loop index UVPT record                  */
   char    errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBfloat chord;        /* Chord length for rational segment       */
#endif
/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

  ipt.x_gm = F_UNDEF;
  ipt.y_gm = F_UNDEF;
  ipt.z_gm = F_UNDEF;

/*!                                                                 */
/* Retrieve start point and tangent from table UVPT.                */
/* Calculate new start UV point if not existing in table.           */
/*                                                                  */

 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Retrieve start    pt uv( %4.2f, %4.2f) l_interv %8.5f l_ints %8.5f\n", 
   uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv,l_ints  );            
fflush(dbgfil(SURPAC)); 
  }
#endif
   if  ( fabs(uvall[i_r].l_interv-l_ints) < utol )
     {
     uv[0]  = uvall[i_r].u;                     /*    Start point   */
     uv[1]  = uvall[i_r].v;                     /*                  */
     uvt[0] = uvall[i_r].tu;                    /* Start tangent    */
     uvt[1] = uvall[i_r].tv;     
     uvall[i_r].f = 1;
     u_ints =  uv[0];
     v_ints =  uv[1];
     goto uvs;
     }
   }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Programming error: Start point l_ints %10.8f not in UVPT\n", 
                     l_ints  );
fflush(dbgfil(SURPAC)); 
  }
#endif


   sprintf(errbuf,"Start UV%%sur940*creseg");
   return(varkon_erpush("SU2993",errbuf));


uvs:;  /* Label: UV start existed in table UVPT                     */


   points[0].x_gm= uv[0];                       /* Start point      */
   points[0].y_gm= uv[1];                       /*                  */
   points[0].z_gm= 0.0;                         /*                  */
   points[1].x_gm= uv[0] +  0.2*uvt[0];         /* Start tangent    */
   points[1].y_gm= uv[1] +  0.2*uvt[1];         /*                  */
   points[1].z_gm= 0.0;                         /*                  */

#ifdef  DEBUG
   u_0.x_gm  = uv[0];
   u_0.y_gm  = uv[1];
   u_0.z_gm  =   0.0;
#endif

/*!                                                                 */
/* Retrieve end   point and tangent from table UVPT.                */
/* Calculate new end UV point if not existing in table.             */
/*                                                                  */

 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Retrieve end      pt uv( %4.2f, %4.2f) l_interv %8.5f l_inte %8.5f\n", 
   uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv,l_inte  );            
fflush(dbgfil(SURPAC)); 
  }
#endif
   if  ( fabs(uvall[i_r].l_interv-l_inte) < utol )
     {
     uv[0]  = uvall[i_r].u;                     /* End   point      */
     uv[1]  = uvall[i_r].v;                     /*                  */
     uvt[0] = uvall[i_r].tu;                    /* End   tangent    */
     uvt[1] = uvall[i_r].tv;     
     u_inte =  uv[0];
     v_inte =  uv[1];
     goto uve;
     }
   }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Programming error: End   point l_inte %10.8f not in UVPT\n", 
                     l_inte  );
fflush(dbgfil(SURPAC)); 
  }
#endif


   sprintf(errbuf,"End   UV%%sur940*creseg");
   return(varkon_erpush("SU2993",errbuf));

uve:;  /* Label: UV end   existed in table UVPT                     */


   points[3].x_gm= uv[0];                       /* End   point      */
   points[3].y_gm= uv[1];                       /*                  */
   points[3].z_gm= 0.0;                         /*                  */
   points[2].x_gm= uv[0] +  0.2*uvt[0];         /* End   tangent    */
   points[2].y_gm= uv[1] +  0.2*uvt[1];         /*                  */
   points[2].z_gm= 0.0;                         /*                  */

#ifdef  DEBUG
   u_100.x_gm  = uv[0];
   u_100.y_gm  = uv[1];
   u_100.z_gm  =   0.0;
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*creseg From UVPT: u_ints %7.4f u_inte %7.4f v_ints %7.4f v_inte %7.4f\n", 
                   u_ints,      u_inte,      v_ints,      v_inte   );
fflush(dbgfil(SURPAC)); 
}
#endif



/*!                                                                 */
/* Calculate or retrieve intermediate point for current segment     */
/*                                                                 !*/

 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Retrieve interm.  pt uv( %4.2f, %4.2f) l_interv %8.5f l_50   %8.5f\n", 
   uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv,l_ints+(l_inte-l_ints)/2.0);            
fflush(dbgfil(SURPAC)); 
  }
#endif
   if  ( fabs(uvall[i_r].l_interv-(l_ints+(l_inte-l_ints)/2.0)) < utol )
     {
     uv[0]  = uvall[i_r].u;                     /* Interm.  point   */
     uv[1]  = uvall[i_r].v;                     /*                  */
     uvt[0] = uvall[i_r].tu;                    /* Interm. tangent  */
     uvt[1] = uvall[i_r].tv;     
     ipt.x_gm=  uv[0];
     ipt.y_gm=  uv[1]; 
     ipt.z_gm= 0.0;
     goto uvm;
     }
   }

/*!                                                                 */
/*   Create new mid UV point. Call of newpoi (output is uv, uvt).   */
/*                                                                 !*/

     status = newpoi (p_sur, p_pat , p_comp ,uls,vls,ule,vle, 0.5 );
     if (status<0) 
        {
        sprintf(errbuf,"newpoi%%varkon_pat_uvcur");
        return(varkon_erpush("SU2973",errbuf));
        }

  if ( uv[0] < 0.0 )
    {
    goto  noipt;
    }

  ipt.x_gm = uv[0];
  ipt.y_gm = uv[1];
  ipt.z_gm =   0.0;


uvm:;  /* Label: UV mid   existed in table UVPT                     */

  u_50 = ipt; /* Check point for case when GE136 failed            */

/*!                                                                 */
/* Create current rational cubic in the UV plane                    */
/* Try first with the intermediate point. Call of GE136.           */
/* Since curve often is close to a straight line will point         */
/* be outside conic triangle and GE136 will fail.                  */
/* Create rational with P-value 0.5 in this case (GE133)           */
/*                                                                 !*/

 status=GE136 (points,&ipt,&cur_seg);    
 if (status < 0) 
     {
#ifdef DEBUG
  chord = SQRT(
   (points[3].x_gm-points[0].x_gm)*(points[3].x_gm-points[0].x_gm)+ 
   (points[3].y_gm-points[0].y_gm)*(points[3].y_gm-points[0].y_gm)+ 
   (points[3].z_gm-points[0].z_gm)*(points[3].z_gm-points[0].z_gm));

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*creseg Conic with P-value= 0.5 (GE136 failed) chord %25.15f\n",chord);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && chord < 100.0*idpoint )
{
fprintf(dbgfil(SURPAC),
"sur940*creseg GE136 failure u_0   %12.8f %12.8f \n" , 
    u_0.x_gm,u_0.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && chord < 100.0*idpoint )
{
fprintf(dbgfil(SURPAC),
"sur940*creseg GE136 failure u_50  %12.8f %12.8f \n" , 
    u_50.x_gm,u_50.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && chord < 100.0*idpoint )
{
fprintf(dbgfil(SURPAC),
"sur940*creseg GE136 failure u_100 %12.8f %12.8f \n" , 
    u_100.x_gm,u_100.y_gm );
fflush(dbgfil(SURPAC)); 
}
 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
if ( dbglev(SURPAC) == 1  && chord < 100.0*idpoint )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg UVPT %d pt uv( %12.8f, %12.8f) l_interv %8.5f \n", 
   (short)i_r+1,uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv  );            
fflush(dbgfil(SURPAC)); 
  }
  }
#endif
    goto noipt;
    }

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
"sur940*creseg Start  pt GE136 %12.8f %12.8f %12.8f\n" , 
   points[0].x_gm,points[0].y_gm,points[0].z_gm );
  fprintf(dbgfil(SURPAC),
"sur940*creseg Start tpt GE136 %12.8f %12.8f %12.8f\n" , 
   points[1].x_gm,points[1].y_gm,points[1].z_gm );
  fprintf(dbgfil(SURPAC),
"sur940*creseg Mid    pt GE136 %12.8f %12.8f %12.8f\n" , 
         ipt.x_gm,      ipt.y_gm,      ipt.z_gm );
  fprintf(dbgfil(SURPAC),
"sur940*creseg End   tpt GE136 %12.8f %12.8f %12.8f\n" , 
   points[2].x_gm,points[2].y_gm,points[2].z_gm );
  fprintf(dbgfil(SURPAC),
"sur940*creseg End    pt GE136 %12.8f %12.8f %12.8f\n" , 
   points[3].x_gm,points[3].y_gm,points[3].z_gm );
}
#endif

/* The mid point was used for the UV segment                        */
   goto  midused;

noipt:; /* Label: Mid point calculation failed                  */


     status=GE133 (points,0.5,&cur_seg);    
     if (status < 0 ) 
        {
        return(-1);
        }

midused:;  /* Label: Mid point was used                             */

/*!                                                                 */
/* Let segment .ofs= 0.0, .typ= UV_CUB_SEG and .sl= 0.0                 */
/*                                                                 !*/

   cur_seg.ofs = 0.0;
   cur_seg.typ = UV_CUB_SEG ;
   cur_seg.sl  = 0.0;     

/*!                                                                 */
/* Calculate or retrieve two check points u_25 and u_75.            */
/*                                                                 !*/

 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Retrieve check  u_25 uv( %4.2f, %4.2f) l_interv %8.5f l_25   %8.5f\n", 
   uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv,l_ints+(l_inte-l_ints)*0.25);            
fflush(dbgfil(SURPAC)); 
  }
#endif
   if  ( fabs(uvall[i_r].l_interv-(l_ints+(l_inte-l_ints)*0.25)) < utol )
     {
     uv[0]  = uvall[i_r].u;                     /*    Start point   */
     uv[1]  = uvall[i_r].v;                     /*                  */
     uvt[0] = uvall[i_r].tu;                    /* Start tangent    */
     uvt[1] = uvall[i_r].tv;     
     ipt.x_gm=  uv[0];
     ipt.y_gm=  uv[1]; 
     ipt.z_gm= 0.0;
     goto u25;
     }
   }



/*!                                                                 */
/*   Calculate check UV point u_25. Call of newpoi.                 */
/*                                                                 !*/

     status = newpoi (p_sur, p_pat , p_comp ,uls,vls,ule,vle, 0.25 );
     if (status<0) 
        {
        sprintf(errbuf,"newpoi%%varkon_pat_uvcur");
        return(varkon_erpush("SU2973",errbuf));
        }

  if ( uv[0] < 0.0 )   /*  G|r n}got annat !!!!!!!   */
    {
    sprintf(errbuf,"u_25%%varkon_pat_uvcur");
    return(varkon_erpush("SU2993",errbuf));
    }

u25:;  /* Label: u_25   existed in table UVPT                     */

  u_25.x_gm =  uv[0]; 
  u_25.y_gm =  uv[1]; 
  u_25.z_gm =    0.0; 

 for ( i_r = 0; i_r < n_uvpt; ++i_r )
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Retrieve check  u_75 uv( %4.2f, %4.2f) l_interv %8.5f l_75   %8.5f\n", 
   uvall[i_r].u, uvall[i_r].v, uvall[i_r].l_interv,l_ints+(l_inte-l_ints)*0.75);
fflush(dbgfil(SURPAC)); 
  }
#endif
   if  ( fabs(uvall[i_r].l_interv-(l_ints+(l_inte-l_ints)*0.75)) < utol )
     {
     uv[0]  = uvall[i_r].u;                     /* Check point      */
     uv[1]  = uvall[i_r].v;                     /*                  */
     uvt[0] = uvall[i_r].tu;                    /* Check tangent    */
     uvt[1] = uvall[i_r].tv;     
     ipt.x_gm=  uv[0];
     ipt.y_gm=  uv[1]; 
     ipt.z_gm= 0.0;
     goto u75;
     }
   }

/*!                                                                 */
/*   Calculate check UV point u_75. Call of newpoi.                 */
/*                                                                 !*/

     status = newpoi (p_sur, p_pat , p_comp ,uls,vls,ule,vle, 0.75 );
     if (status<0) 
        {
        sprintf(errbuf,"newpoi%%varkon_pat_uvcur");
        return(varkon_erpush("SU2973",errbuf));
        }

  if ( uv[0] < 0.0 )   /*  G|r n}got annat !!!!!!!   */
    {
    sprintf(errbuf,"u_75%%varkon_pat_uvcur");
    return(varkon_erpush("SU2993",errbuf));
    }

u75:;  /* Label: u_75   existed in table UVPT                     */

  u_75.x_gm =  uv[0]; 
  u_75.y_gm =  uv[1]; 
  u_75.z_gm =    0.0; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*creseg Out u_ints %7.4f u_inte %7.4f v_ints %7.4f v_inte %7.4f\n", 
                     u_ints,      u_inte,      v_ints,      v_inte   );
fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/



/*!************** Internal ** function ** newpoi ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate a new UV point within the patch                       */
/*                                                                  */
    static  short newpoi(p_sur, p_pat, p_comp ,uls,vls,ule,vle,new_u  )
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBfloat  uls,vls;     /* Patch limit start point                 */
   DBfloat  ule,vle;     /* Patch limit end   point                 */
   DBfloat  new_u;       /* Parameter value for new point w.r.t     */
                         /* u_ints, v_ints, u_inte, v_inte          */
/* Out:                                                             */
/*      static variables uv and uvt (uv < 0,0 <==> error)           */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector   npt;          /* Intermediate point                      */
 
/*----------------------------------------------------------------- */

                         /* For sur900:                             */
   DBint   npts;         /* Number of output points                 */
   EVALS   xyz_a[SMAX];  /* Array with all solutions (R*3 and UV)   */
   DBint   b_case;       /* Boundary point case sur900:             */
                         /* Eq. 0: No     points                    */
                         /* Eq. 1: Normal points                    */
                         /* Eq. 2: Curve coincides w. isoparameter  */
   DBfloat t_in[2][2];   /* Iso-parameter curve limits              */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
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
  "sur940*newpoi u_ints %f u_inte %f new_u %f \n", u_ints,  u_inte, new_u );
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi v_ints %f v_inte %f\n", v_ints,  v_inte);
fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi l_ints %f l_inte %f \n", l_ints,  l_inte );
fflush(dbgfil(SURPAC)); 
  }
#endif

  npt.x_gm = F_UNDEF;
  npt.y_gm = F_UNDEF;
  npt.z_gm = F_UNDEF;

  uv[0]    = F_UNDEF;
  uv[1]    = F_UNDEF;
  uvt[0]   = F_UNDEF;
  uvt[1]   = F_UNDEF;

/*!                                                                 */
/* Determine the optimal iso-parameter (maximu distance)            */
/* First attempt with varkon_sur_num1 (sur900).                     */
/* Check that a UV square u_ints, v_ints, u_inte, v_inte            */
/* is defined for Debug On.                                         */
/*                                                                 !*/

#ifdef  DEBUG
   if (fabs(u_ints-u_inte) < utol  && fabs(v_ints-v_inte) < utol )
     {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*newpoi Programming error: A UV interval is not defined ! \n");
fprintf(dbgfil(SURPAC),
"sur940*newpoi u_ints %12.8f v_ints %12.8f new_u %12.8f\n", u_ints, v_ints, new_u);
fprintf(dbgfil(SURPAC),
"sur940*newpoi u_inte %12.8f v_inte %12.8f\n", u_inte, v_inte );
fflush(dbgfil(SURPAC)); 
}
     sprintf(errbuf,"UV interval%%sur940*newpoi");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

   if (fabs(u_ints-u_inte) > fabs(v_ints-v_inte))
     {
     t_in[0][0] = u_ints+new_u*(u_inte-u_ints); /* Start U          */
     t_in[0][1] = vls;                          /* Start V          */
     t_in[1][0] = u_ints+new_u*(u_inte-u_ints); /* End   U          */
     t_in[1][1] = vle;                          /* End   V          */
     }
   else                                       
     {
     t_in[0][0] = uls;                          /* Start U          */
     t_in[0][1] = v_ints+new_u*(v_inte-v_ints); /* Start V          */
     t_in[1][0] = ule;                          /* End   U          */
     t_in[1][1] = v_ints+new_u*(v_inte-v_ints); /* End   V          */
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*newpoi u_ints %12.8f v_ints %12.8f new_u %12.8f\n", u_ints, v_ints, new_u);
fprintf(dbgfil(SURPAC),
"sur940*newpoi u_inte %12.8f v_inte %12.8f\n", u_inte, v_inte );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*newpoi fabs(u_ints-u_inte) %12.8f\n", fabs(u_ints-u_inte) );
fprintf(dbgfil(SURPAC),
"sur940*newpoi fabs(v_ints-v_inte) %12.8f\n", fabs(v_ints-v_inte) );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*newpoi Start U t_in[0][0] %12.8f End U t_in[1][0] %12.8f\n", 
  t_in[0][0], t_in[1][0] );
fprintf(dbgfil(SURPAC),
"sur940*newpoi Start V t_in[0][1] %12.8f End V t_in[1][1] %12.8f\n", 
  t_in[0][1], t_in[1][1] );
fflush(dbgfil(SURPAC)); 
}
#endif

   status=varkon_sur_num1
   (p_sur,p_pat,t_in,p_comp,&npts,xyz_a,&b_case);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_num1%%varkon_pat_uvcur ");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Goto first if midpoint solution exists.                          */
/*                                                                 !*/


 if      (npts == 1 )
    {
    npt.x_gm= xyz_a[0].u;
    npt.y_gm= xyz_a[0].v; 
    npt.z_gm= 0.0;
    goto first;
    }
 else if (npts >  1 )
    {
    npt.x_gm= xyz_a[0].u;
    npt.y_gm= xyz_a[0].v; 
    npt.z_gm= 0.0;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi Number intermediate pts from sur900 npts= %d\n", (short)npts);
fflush(dbgfil(SURPAC)); 
  }
#endif
    goto first;
    }
 else if (npts == 0 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi No intermediate pt from first attempt with sur900! Call sur900 again!\n");
fflush(dbgfil(SURPAC)); 
  }
#endif
    }

/*!                                                                 */
/* Second attempt with varkon_sur_num1 (sur900).                    */
/*                                                                 !*/

   if (fabs(u_ints-u_inte) > fabs(v_ints-v_inte))
     {
     t_in[0][0] = uls;                          /* Start U          */
     t_in[0][1] = v_ints+new_u*(v_inte-v_ints); /* Start V          */
     t_in[1][0] = ule;                          /* End   U          */
     t_in[1][1] = v_ints+new_u*(v_inte-v_ints); /* End   V          */
     }
   else                                       
     {
     t_in[0][0] = u_ints+new_u*(u_inte-u_ints); /* Start U          */
     t_in[0][1] = vls;                          /* Start V          */
     t_in[1][0] = u_ints+new_u*(u_inte-u_ints); /* End   U          */
     t_in[1][1] = vle;                          /* End   V          */
     }

   status=varkon_sur_num1
   (p_sur,p_pat,t_in,p_comp,&npts,xyz_a,&b_case);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_num1%%varkon_pat_uvcur ");
        return(varkon_erpush("SU2943",errbuf));
        }

 if      (npts == 1 )
    {
    npt.x_gm= xyz_a[0].u;
    npt.y_gm= xyz_a[0].v; 
    npt.z_gm= 0.0;
    }
 else if (npts >  1 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi Number intermediate pts from 2'nd sur900 npts= %d\n", (short)npts);
fflush(dbgfil(SURPAC)); 
  }
#endif
    npt.x_gm= xyz_a[0].u;
    npt.y_gm= xyz_a[0].v; 
    npt.z_gm= 0.0;
    }
 else if (npts == 0 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi No intermediate pt from second call of sur900! \n");
fflush(dbgfil(SURPAC)); 
  }
#endif
    goto  nonpt;
    }

/*!                                                                 */
/* Calculate tangent for the new UV point.                          */
/* Call of uvtang.                                                  */
first:; /* Label: There was a solution for the first attempt */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*newpoi Intermediate pt from sur900 npt=( %f, %f) \n", 
 npt.x_gm,npt.y_gm);            
fflush(dbgfil(SURPAC)); 
}
#endif

   uv[0]= npt.x_gm;                             /* Interm.  point   */
   uv[1]= npt.y_gm; 

/* Point not in UVPT. Calculate uvt in uvtang.                      */

   status=uvtang(p_sur,p_pat);
   if (status<0) 
      {
      sprintf(errbuf,"uvtang%%varkon_pat_uvcur        ");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* Add record in UVPT for the new point                             */
/*                                                                 !*/

   n_uvpt = n_uvpt + 1;               /* Additional record in UVPT  */
   if  ( n_uvpt > UVPTMAX )
     {
     sprintf(errbuf,"UVPTMAX%%varkon_pat_uvcur");
     return(varkon_erpush("SU2993",errbuf));
     }

   uvall[n_uvpt-1].f        = 1;                     /* Flag for the UV point      */
   uvall[n_uvpt-1].l_interv = l_ints +               /* "Parameter"   */
                              (l_inte-l_ints)*new_u; /*               */
   uvall[n_uvpt-1].u        = uv[0];                 /* U value       */
   uvall[n_uvpt-1].v        = uv[1];                 /* V value       */
   uvall[n_uvpt-1].tu       = uvt[0];                /* Tangent U     */
   uvall[n_uvpt-1].tv       = uvt[1];                /* Tangent V     */

nonpt:; /* No new point */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*newpoi Point uv( %4.2f, %4.2f) uvt( %6.4f, %6.4f) l_interv %f\n", 
   uv[0],uv[1], uvt[0],uvt[1],  uvall[n_uvpt-1].l_interv  );            
fflush(dbgfil(SURPAC)); 
  }
#endif


       return(SUCCED);


} /* End of function */

/********************************************************************/



/*!************** Internal ** function ** chepoi ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Distance between a check point and the current segment          */
/*                                                                  */
    static  short chepoi(p_sur,p_pat,upt, p_dist )
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Current rational segment cur_seg                             */

   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   DBVector upt;         /* Check point in the U,V plane            */

/* Out:                                                             */

   DBfloat *p_dist;      /* Distance to curve                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector cpt;         /* Check point in R*3                      */

                         /* For varkon_GE143 (GE143)              */
   short   n_clo;        /* Number of closest points                */
                         /* Note that double points normally exist  */
                         /* due to the restarts.                    */
   DBfloat uout[3];      /* Curve parameter values (u) for the      */
                         /* closest points. The u values are not    */
                         /* sorted.                                 */
  
 
/*----------------------------------------------------------------- */

   EVALS   xyz_check;    /* Coordinates and derivatives             */
   DBint   icase_check;  /* Only coordinates for check              */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur940*chepoi Check point upt %f %f \n", upt.x_gm, upt.y_gm   );
fflush(dbgfil(SURPAC)); 
  }
#endif

  *p_dist  = F_UNDEF;

/*!                                                                 */
/* Calculate the cpt (R*3 point) corresponding to upt (UV point)    */
/* Call of varkon_sur_eval (sur210)                                 */
/*                                                                 !*/

   icase_check = 0;

   status=varkon_sur_eval
      (p_sur,p_pat,icase_check,upt.x_gm ,upt.y_gm ,&xyz_check);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi sur210 failed Point upt %f %f\n",upt.x_gm,upt.y_gm);
fflush(dbgfil(SURPAC)); 
}
#endif
        sprintf(errbuf,"sur210%%sur940*chepoi");
        return(varkon_erpush("SU2943",errbuf));
        }

   cpt.x_gm = xyz_check.r_x;
   cpt.y_gm = xyz_check.r_y;
   cpt.z_gm = xyz_check.r_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi Check point sur210 is evaluated \n");
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Calculate closest point. Call of varkon_GE143 (GE143).         */
/*                                                                 !*/


   status=GE143 ((DBAny *)&cur,&cur_seg,&cpt,nstart,&n_clo,uout);
   if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failed Point cpt %f %f\n",cpt.x_gm,cpt.y_gm);
fflush(dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"GE143%%sur940*chepoi");
     return(varkon_erpush("SU2943",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi Closest point GE143 is calculated\n");
fflush(dbgfil(SURPAC)); 
}
#endif

   if ( n_clo == 0   ) /* Handled in main routine */
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failed for cpt  %10.4f %10.4f %10.4f\n" , 
    cpt.x_gm,cpt.y_gm,cpt.z_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure upt   %12.8f %12.8f \n", upt.x_gm, upt.y_gm);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure u_0   %12.8f %12.8f \n" , 
    u_0.x_gm,u_0.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure u_25  %12.8f %12.8f \n" , 
    u_25.x_gm,u_25.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure u_50  %12.8f %12.8f \n" , 
    u_50.x_gm,u_50.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure u_75  %12.8f %12.8f \n" , 
    u_75.x_gm,u_75.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE143 failure u_100 %12.8f %12.8f \n" , 
    u_100.x_gm,u_100.y_gm );
fflush(dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"no_clo=0%%sur940*chepoi");
     return(varkon_erpush("SU2993",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940 u_0   %7.4f %7.4f \n" , 
    u_0.x_gm,u_0.y_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940  upt  %7.4f %7.4f No of closest pts n_clo= %d uout[0] %f\n" , 
     upt.x_gm, upt.y_gm,  (short)n_clo, uout[0] );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940 u_100 %7.4f %7.4f \n" , 
    u_100.x_gm,u_100.y_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Get coordinates for closest pt. Call of varkon_GE109 (GE109).  */
/*                                                                 !*/

   xyz_c.t_global = 1.0 + uout[0];  /* Global parameter value       */

   status=GE109 ((DBAny *)&cur ,&cur_seg ,&xyz_c  );
   if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi GE109 failed Point t_global %f \n",xyz_c.t_global);
fflush(dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"GE109%%sur940*chepoi");
     return(varkon_erpush("SU2943",errbuf));
     }

  *p_dist = SQRT( (xyz_c.r.x_gm - cpt.x_gm) * (xyz_c.r.x_gm - cpt.x_gm) + 
                  (xyz_c.r.y_gm - cpt.y_gm) * (xyz_c.r.y_gm - cpt.y_gm) + 
                  (xyz_c.r.z_gm - cpt.z_gm) * (xyz_c.r.z_gm - cpt.z_gm) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*chepoi Check point cpt %f %f n_clo %d Distance %15.10f\n", 
                cpt.x_gm, cpt.y_gm,(short)n_clo,  *p_dist );
fflush(dbgfil(SURPAC)); 
}
#endif


       return(SUCCED);


} /* End of function */

/********************************************************************/




/*!************** Internal ** function ** oneseg ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Create only one short segment ?                                 */
/*                                                                  */
    static  short   oneseg  (p_sur, p_pat, 
       ustart,vstart,uend,vend,us_t,vs_t,ue_t,ve_t,
       p_cur,p_seg,p_nseg)
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   DBfloat ustart,vstart;/* Start U,V point for "patch" curve       */
   DBfloat uend  ,vend;  /* End   U,V point for "patch" curve       */
   DBfloat us_t,vs_t;    /* Start U,V tangent to ustart,vstart      */
   DBfloat ue_t,ve_t;    /* End   U,V tangent to uend  ,vend        */
   DBCurve *p_cur;       /* Curve                             (ptr) */
   DBSeg   *p_seg;       /* Alloc. area for UV curve segments (ptr) */
                         /* (start adress for added segments)       */
                         /* Note that the DB pointer to the surface */
                         /* must be initialized in all segments !   */
   DBint   *p_nseg;      /* Number of output segments in the patch  */
/* Out:                                                             */
/*                                                                  */
/* Value (error code) of function oneseg = -1 for chord = 0         */
/* (Criterion for chord= 0 defined in GE133)                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat chord_uv;     /* Chord length U,V                        */
   DBfloat chord_r3;     /* Chord length R*3                        */
 
/*----------------------------------------------------------------- */

   EVALS   xyz_chord;    /* Coordinates and derivatives             */
   DBint   icase_chord;  /* Only coordinates for check              */
   DBVector spt;         /* Start point                             */
   DBVector ept;         /* End   point                             */
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg u_ints %7.4f u_inte %7.4f v_ints %7.4f v_inte %7.4f\n", 
                   u_ints,      u_inte,      v_ints,      v_inte   );
fflush(dbgfil(SURPAC)); 
}
#endif

   chord_uv = F_UNDEF;
   chord_r3 = F_UNDEF;

/*!                                                                 */
/* Calculate start pt (R*3 point) corresponding to ustart, vstart   */
/* Call of varkon_sur_eval (sur210)                                 */
/*                                                                 !*/

   icase_chord = 0;

   status=varkon_sur_eval
   (p_sur,p_pat,icase_chord,ustart ,vstart ,&xyz_chord);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg sur210 failed for ustart %f vstart %f\n",
                            ustart  ,vstart  );
fflush(dbgfil(SURPAC)); 
}
#endif
        sprintf(errbuf,"sur210%%sur940*oneseg");
        return(varkon_erpush("SU2943",errbuf));
        }

   spt.x_gm = xyz_chord.r_x;
   spt.y_gm = xyz_chord.r_y;
   spt.z_gm = xyz_chord.r_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg spt %f %f %f \n",spt.x_gm,spt.y_gm,spt.z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* Calculate end   pt (R*3 point) corresponding to uend  , vend     */
/* Call of varkon_sur_eval (sur210)                                 */
/*                                                                 !*/

   icase_chord = 0;

   status=varkon_sur_eval
      (p_sur,p_pat,icase_chord,uend   ,vend   ,&xyz_chord);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg sur210 failed for uend   %f vend   %f\n",
                            uend    ,vend    );
fflush(dbgfil(SURPAC)); 
}
#endif
        sprintf(errbuf,"sur210%%sur940*oneseg");
        return(varkon_erpush("SU2943",errbuf));
        }

   ept.x_gm = xyz_chord.r_x;
   ept.y_gm = xyz_chord.r_y;
   ept.z_gm = xyz_chord.r_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg ept %f %f %f \n",ept.x_gm,ept.y_gm,ept.z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif


  chord_r3 = SQRT(
   ( spt.x_gm - ept.x_gm )*( spt.x_gm - ept.x_gm )+ 
   ( spt.y_gm - ept.y_gm )*( spt.y_gm - ept.y_gm )+ 
   ( spt.z_gm - ept.z_gm )*( spt.z_gm - ept.z_gm ));

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg chord_r3 %f Criterion 10*idpoint %f\n", 
              chord_r3, 10*idpoint);
fflush(dbgfil(SURPAC)); 
}
#endif

  if   (  chord_r3 >  10*idpoint ) return(2);


/*!                                                                 */
/* Create output UV segment.                                        */
/* Call of varkon_GE133 (GE133).                                  */
/*                                                                 !*/

   points[0].x_gm= ustart;                      /* Start point      */
   points[0].y_gm= vstart;                      /*                  */
   points[0].z_gm= 0.0;                         /*                  */
   points[1].x_gm= ustart+ us_t;                /* Start tangent    */
   points[1].y_gm= vstart+ vs_t;                /*                  */
   points[1].z_gm= 0.0;                         /*                  */
   points[3].x_gm= uend ;                       /* End   point      */
   points[3].y_gm= vend ;                       /*                  */
   points[3].z_gm= 0.0;                         /*                  */
   points[2].x_gm= uend  + ue_t;                /* End   tangent    */
   points[2].y_gm= vend  + ve_t;                /*                  */
   points[2].z_gm= 0.0;                         /*                  */


  chord_uv = SQRT(
   (points[3].x_gm-points[0].x_gm)*(points[3].x_gm-points[0].x_gm)+ 
   (points[3].y_gm-points[0].y_gm)*(points[3].y_gm-points[0].y_gm)+ 
   (points[3].z_gm-points[0].z_gm)*(points[3].z_gm-points[0].z_gm));


     status=GE133 (points,0.5,&cur_seg);    
     if (status < 0 ) 
        {
        return(-1);
        }

   cur_seg.ofs = 0.0;
   cur_seg.typ = UV_CUB_SEG ;
   cur_seg.sl  = 0.0;     

   *p_nseg = 1;

 V3MOME((char *)(&cur_seg),(char *)(p_seg+*p_nseg-1),sizeof(DBSeg));


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur940*oneseg Exit chord_uv %f *p_seg %d\n", chord_uv,(int)*p_nseg  );
fflush(dbgfil(SURPAC)); 
}
#endif

       return(1);


} /* End of function */

/********************************************************************/


