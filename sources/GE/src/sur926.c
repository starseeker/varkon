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
/*  Function: varkon_sur_uvtang                    File: sur926.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the tangent in the uv plane for         */
/*  given surface point (EVALS) and given interrogation data.       */
/*                                                                  */
/*  The calculated tangent and the coordinates of the R*3 point     */
/*  is output.                                                      */
/*                                                                  */
/*  NOTE !!!!  This is a copy of sur940. calculation of tangents    */
/*             shall only be made in one function in the future     */
/*             If changes are made here, they must also be made     */
/*             in sur940                         !!!!! NOTE !!!!    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-11   Originally written                                 */
/*  1994-10-14   varkon_erinit                                      */
/*  1996-04-29   b_flag added                                       */
/*  1999-11-20   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_uvtang     Calc. UV "interrogation" tangent */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short uvtang();        /* The U,V vector for given UV point  */
static short fsilhou();       /* The U,V vector        for F_SILH   */
static short isilhou();       /* The U,V vector        for I_SILH   */
static short ssilhou();       /* The U,V vector        for S_SILH   */
static short intplan();       /* The U,V vector        for SURPLAN  */
static short isoparm();       /* The U,V vector for iso-segment     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef   NOT_YET_IMPLEMENTED
static short fisopho();       /* The U,V vector        for F_ISOP   */
static short iisopho();       /* The U,V vector        for I_ISOP   */
#endif /*  NOT_YET_IMPLEMENTED */

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint ctype;               /* Case of computation            */
static DBfloat pi_x,pi_y;         /* Intersecting plane             */
static DBfloat pi_z,pi_d;         /*                                */
static DBfloat eye_x,eye_y,eye_z; /* Eye point for reflection crvs  */
static DBfloat eyev_x,eyev_y;     /* Eye vect. for reflection crvs  */
static DBfloat eyev_z;            /*                                */
static DBVector sdir;             /* Direction for spine silhouette */
static DBfloat angle;             /* Angle in degrees for isophotes */
static DBfloat uvt[3];            /* Vector in U,V plane (note 3!)  */
static EVALS xyz;                 /* Coordinates and derivatives    */
                                  /* for a point on a surface       */
/*-----------------------------------------------------------------!*/
static DBint status;              /* Error code from a called fctn  */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_normv          * Normalization of (3D) vector   */
/*           varkon_vec_projpla    * Project vector onto plane      */
/*           varkon_erinit         * Initiate error message stack   */
/*           varkon_erpush         * Error message to terminal      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2803 = Spine must be a straight line    varkon_sur_uvtang      */
/* SU2943 = Called function xxxxxx failed in varkon_sur_uvtang      */
/* SU2993 = Severe program error in varkon_sur_uvtang  (sur926).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/
     DBstatus       varkon_sur_uvtang (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   n_points,     /* No points in arrays xyzpt and curpt     */
   EVALS   xyzpt[9],     /* Array with all solutions (R*3 and UV)   */
   EVALC   curpt[9],     /* All entry/exit curve points             */
   IRUNON *p_comp,       /* Pointer to computation data             */
   short   b_flag )      /* Boundary point flag Refer to EPOINT     */
                         /* Eq. 1: Two "normal" points              */
                         /* Eq. 2: Curve coincides iso-p.           */
                         /* Eq.-1: Points "used" in curve           */
/* Out:                                                             */
/* Add UV tangent data and R*3 coord. for points in curpt (EVALC)   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_p;           /* Loop index for points                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */

   if ( n_points == 0 ) goto nopt;  

/* Case of computation ctype from input IRUNON (p_comp)             */
/*                                                                 !*/
   ctype   = p_comp->ipl_un.ctype;

   if ( b_flag == 0 || b_flag == 1 || b_flag == 2 )
       {
       ;
       }
   else
       {
       sprintf(errbuf,"(b_flag)%% varkon_sur_uvtang");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* Retrieve computation data for the input computation case         */
/* (for instance the intersection plane for ctype= SURPLAN)         */
/*                                                                 !*/

   if      ( ctype  == F_SILH  )
       {
       eye_x  = p_comp->sil_un.eye_x ;
       eye_y  = p_comp->sil_un.eye_y ;
       eye_z  = p_comp->sil_un.eye_z ;
       }
   else if ( ctype  == I_SILH  )
       {
       eyev_x = p_comp->sil_un.eyev_x ;
       eyev_y = p_comp->sil_un.eyev_y ;
       eyev_z = p_comp->sil_un.eyev_z ;
       }
   else if ( ctype  == S_SILH  )
       {
       sdir.x_gm = p_comp->sil_un.eyev_x ;
       sdir.y_gm = p_comp->sil_un.eyev_y ;
       sdir.z_gm = p_comp->sil_un.eyev_z ;
       }
   else if ( ctype  == F_ISOP  )
       {
       eye_x  = p_comp->iso_un.eye_x ;
       eye_y  = p_comp->iso_un.eye_y ;
       eye_z  = p_comp->iso_un.eye_z ;
       angle  = p_comp->iso_un.angle ;
       }
   else if ( ctype  == I_ISOP  )
       {
       eye_x  = p_comp->iso_un.eye_x ;
       eye_y  = p_comp->iso_un.eye_y ;
       eye_z  = p_comp->iso_un.eye_z ;
       angle  = p_comp->iso_un.angle ;
       }
   else if ( ctype  == SURPLAN )
       {
       pi_x = p_comp->ipl_un.in_x;
       pi_y = p_comp->ipl_un.in_y;
       pi_z = p_comp->ipl_un.in_z;
       pi_d = p_comp->ipl_un.in_d;
       }
   else
       {
       sprintf(errbuf,"(ctype )%% varkon_sur_uvtang       ");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/* 2. Calculate tangents for iso-parameter segment (b_flag= 2)      */
/* ___________________________________________________________      */
/*                                                                 !*/

   if  ( b_flag == 2 )
     {
     status=isoparm(xyzpt, curpt);
     if (status<0) 
        {
        sprintf(errbuf,"isoparm%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     goto  isop;
     }

/*!                                                                 */
/* 3. Calculate tangents for all input points                       */
/* __________________________________________                       */

/*                                                                 !*/

/*!                                                                 */
/* Start loop all points in xyzpt and curpt. i_p=1,2,3,..,n_points  */
/*                                                                 !*/

for (  i_p=1; i_p<=n_points; ++i_p )     /* Start loop point        */
  {

/*!                                                                 */
/* Retrieve data from input derivative arrary:                      */
   xyz = xyzpt[i_p-1];
/*                                                                 !*/

/*!                                                                 */
/* Calculate the tangent.                                           */
/* Call of uvtang.                                                  */
/*                                                                 !*/

   status=uvtang();
   if (status<0) 
      {
      sprintf(errbuf,"uvtang%%varkon_sur_uvtang       ");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* Output data to structure variable curpt (EVALC).                 */
/*                                                                 !*/
   curpt[i_p-1].u_t = uvt[0];
   curpt[i_p-1].v_t = uvt[1];
   curpt[i_p-1].r_x = xyzpt[i_p-1].r_x;
   curpt[i_p-1].r_y = xyzpt[i_p-1].r_y;
   curpt[i_p-1].r_z = xyzpt[i_p-1].r_z;

   }                                /* End loop point               */

/*!                                                                 */
/* End   loop all points in xyzpt and curpt. i_p=1,2,3,..,n_points  */
/*                                                                 !*/


/* 7. Exit                                                          */
/* _______                                                          */
/*!                                                                 */
nopt:          /*  Label nopt: No points in input arrays            */
isop:          /*  Label isop: Iso-parameter segment                */
/*                                                                 !*/

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!************** Internal ** function ** uvtang ********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for a given UV point                                            */
/*                                                                  */
               static  short   uvtang ()
/*                                                                  */
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
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
        sprintf(errbuf,"fsilhou%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == I_SILH  )
     {
     status=isilhou();
     if (status<0) 
        {
        sprintf(errbuf,"isilhou%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == S_SILH  )
     {
     status=ssilhou();
     if (status<0) 
        {
        sprintf(errbuf,"ssilhou%%varkon_sur_uvtang");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == F_ISOP  )
     {
    /*     status=intplan();   */
     if (status<0) 
        {
        sprintf(errbuf,"fisopho%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == I_ISOP  )
     {
     /*    status=intplan();   */
     if (status<0) 
        {
        sprintf(errbuf,"iisopho%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else if ( ctype  == SURPLAN )
     {
     status=intplan();
     if (status<0) 
        {
        sprintf(errbuf,"intplan%%varkon_sur_uvtang       ");
        return(varkon_erpush("SU2973",errbuf));
        }
     }
   else
       {
       sprintf(errbuf,"(ctype m)%% varkon_sur_uvtang       ");
       return(varkon_erpush("SU2993",errbuf));
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur926*uvtang  uvt= ( %f, %f ) \n"
   , uvt[0], uvt[1]       );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** isilhou *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
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
/*     The eye vector eyev_x, eyev_y, eyev_z                        */
/* Out: The vector uvt                                                */
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
/*                                                                 !*/

/*!                                                                 */
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
/* Call of varkon_normv (normv).                                    */
/*                                                                 !*/
   varkon_normv(uvt);                                                  

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
  DBVector  plane;       /* Plane  normal                           */
  DBint     ocase;       /* Output case. Eq. 1: Normalised vector   */
  DBVector  psdir;       /* U crv tangent projected in spine plane  */
  DBfloat   dvdu;        /* Derivative dV/dU                        */
 
/*----------------------------------------------------------------- */

   DBfloat  sumder;      /* Sum of second derivatives for spine     */
   DBfloat  denom;       /* Denominator for dV/dU                   */
   char     errbuf[80];  /* String for error message fctn erpush    */

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
        sprintf(errbuf,"ssilhou%% %f varkon_sur_uvtang",sumder);
        varkon_erinit();
        return(varkon_erpush("SU2803",errbuf));
        }

   plane.x_gm= xyz.spt_x; 
   plane.y_gm= xyz.spt_y; 
   plane.z_gm= xyz.spt_z;

   ocase= 1;      
   status= varkon_vec_projpla ( sdir, plane, ocase, &psdir );
   if (status<0) 
      {
      sprintf(errbuf,"varkon_sur_eval%%varkon_sur_uvtang");
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
   varkon_normv(uvt);                                                  

   return(SUCCED);


} /* End of function */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
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
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The eye point  eye_x, eye_y, eye_z                           */
/* Out: The vector uvt                                              */
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
/*                                                                 !*/

/*!                                                                 */
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
/* Call of varkon_normv (normv).                                    */
/*                                                                 !*/
   varkon_normv(uvt);                                                  

   return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** intplan *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for calculation case:  SURPLAN                                  */
/*                                                                  */
               static  short   intplan()
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In: Global (static) variables:                                   */
/*     Points and derivatives xyz from varkon_sur_eval              */
/*     The intersecting plane pi_x,pi_y,pi_z,pi_d                   */
/* Out: The vector uvt                                              */
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
/* Call of varkon_normv (normv).                                    */
/*                                                                 !*/
   varkon_normv(uvt);                                                  

   return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** isoparm *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  The function calculates the vector direction in the UV plane    */
/*  for calculation case: iso-parameter (b_flag= 2)                 */
/*                                                                  */
               static  short   isoparm(xyzpt, curpt)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS   xyzpt[9];     /* Array with all solutions (R*3 and UV)   */
   EVALC   curpt[9];     /* All entry/exit curve points             */
/* Out: The vector uvt                                              */
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

   uvt[0] =  xyzpt[1].u - xyzpt[0].u;
   uvt[1] =  xyzpt[1].v - xyzpt[0].v;
   uvt[2] =   0.0; 

/*!                                                                 */
/* Normalization (vector with 3 components):                        */
/* Call of varkon_normv (normv).                                    */
/*                                                                 !*/
   varkon_normv(uvt);                                                  

   curpt[0].u_t = uvt[0];
   curpt[0].v_t = uvt[1];
   curpt[0].r_x = xyzpt[0].r_x;
   curpt[0].r_y = xyzpt[0].r_y;
   curpt[0].r_z = xyzpt[0].r_z;
   curpt[1].u_t = uvt[0];
   curpt[1].v_t = uvt[1];
   curpt[1].r_x = xyzpt[1].r_x;
   curpt[1].r_y = xyzpt[1].r_y;
   curpt[1].r_z = xyzpt[1].r_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur926*isoparm u1= ( %4.2f , %4.2f ) uvt= ( %3.1f, %3.1f ) \n" , 
    xyzpt[0].u,  xyzpt[0].v, uvt[0], uvt[1]  );
  fprintf(dbgfil(SURPAC),
  "sur926*isoparm u2= ( %4.2f , %4.2f ) uvt= ( %3.1f, %3.1f ) \n" , 
    xyzpt[1].u,  xyzpt[1].v, uvt[0], uvt[1]  );
  }
#endif

   return(SUCCED);


} /* End of function */

/********************************************************************/


