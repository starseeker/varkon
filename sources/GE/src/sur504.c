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
/*  Function: varkon_pat_p21eval                   File: sur504.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Evaluation of GMPATP21                                          */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-22   Created by MBS program p_evalpxv0                  */
/*  1998-03-22   DEF_P21 added manually                             */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_p21eval    Evaluation of GMPATP21           */
/*                                                              */
/*------------------------------------------------------------- */

#ifdef  DEF_P21
/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Initialization of variables        */
static short c_power();       /* Calculate all u**i and u**j        */
static short c_coord();       /* Calculate coordinates              */
static short c_drdu();        /* Calculate dr/du                    */
static short c_drdv();        /* Calculate dr/dv                    */
static short c_second();      /* Calculate second derivatives       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat r_x,r_y,r_z;       /* Coordinates          r(u)      */
static DBfloat u_x,u_y,u_z;       /* Tangent             dr/du      */
static DBfloat v_x,v_y,v_z;       /* Tangent             dr/dv      */
static DBfloat u2_x,u2_y,u2_z;    /* Second derivative  d2r/du2     */
static DBfloat v2_x,v2_y,v2_z;    /* Second derivative  d2r/dv2     */
static DBfloat uv_x,uv_y,uv_z;    /* Twist vector       d2r/dudv    */
static DBfloat pow_u_0;           /* U**0                           */
static DBfloat pow_u_1;           /* U**1                           */
static DBfloat pow_u_2;           /* U**2                           */
static DBfloat pow_u_3;           /* U**3                           */
static DBfloat pow_u_4;           /* U**4                           */
static DBfloat pow_u_5;           /* U**5                           */
static DBfloat pow_u_6;           /* U**6                           */
static DBfloat pow_u_7;           /* U**7                           */
static DBfloat pow_u_8;           /* U**8                           */
static DBfloat pow_u_9;           /* U**9                           */
static DBfloat pow_u_10;          /* U**10                          */
static DBfloat pow_u_11;          /* U**11                          */
static DBfloat pow_u_12;          /* U**12                          */
static DBfloat pow_u_13;          /* U**13                          */
static DBfloat pow_u_14;          /* U**14                          */
static DBfloat pow_u_15;          /* U**15                          */
static DBfloat pow_u_16;          /* U**16                          */
static DBfloat pow_u_17;          /* U**17                          */
static DBfloat pow_u_18;          /* U**18                          */
static DBfloat pow_u_19;          /* U**19                          */
static DBfloat pow_u_20;          /* U**20                          */
static DBfloat pow_u_21;          /* U**21                          */
static DBfloat pow_v_0;           /* V**0                           */
static DBfloat pow_v_1;           /* V**1                           */
static DBfloat pow_v_2;           /* V**2                           */
static DBfloat pow_v_3;           /* V**3                           */
static DBfloat pow_v_4;           /* V**4                           */
static DBfloat pow_v_5;           /* V**5                           */
static DBfloat pow_v_6;           /* V**6                           */
static DBfloat pow_v_7;           /* V**7                           */
static DBfloat pow_v_8;           /* V**8                           */
static DBfloat pow_v_9;           /* V**9                           */
static DBfloat pow_v_10;          /* V**10                          */
static DBfloat pow_v_11;          /* V**11                          */
static DBfloat pow_v_12;          /* V**12                          */
static DBfloat pow_v_13;          /* V**13                          */
static DBfloat pow_v_14;          /* V**14                          */
static DBfloat pow_v_15;          /* V**15                          */
static DBfloat pow_v_16;          /* V**16                          */
static DBfloat pow_v_17;          /* V**17                          */
static DBfloat pow_v_18;          /* V**18                          */
static DBfloat pow_v_19;          /* V**19                          */
static DBfloat pow_v_20;          /* V**20                          */
static DBfloat pow_v_21;          /* V**21                          */

/*-----------------------------------------------------------------!*/

#endif  /*  DEF_P21  */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = Illegal computation case=   varkon_pat_p21eval (sur504) */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_p21eval   */
/* SU2993 = Severe program error in varkon_pat_p21eval (sur504).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_p21eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATP21  *p_pat21,   /* Patch GMPATP21                    (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Add dr/du derivatives            */
                         /* Eq. 2: Add dr/dv derivatives            */
                         /* Eq. 3: Add dr/du and dr/dv derivatives  */
                         /* >=  4: Add d2r/du2, d2r/dv2, d2r/dudv   */
   DBfloat  u_l,         /* Patch (local) U parameter value         */
   DBfloat  v_l,         /* Patch (local) V parameter value         */
   EVALS   *p_xyz )      /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
#ifdef  DEF_P21
   DBint  status;        /* Error code from a called function       */
#endif  /*  DEF_P21  */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef  DEF_P21
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Enter *** varkon_pat_p21eval: u= %f v= %f p_pat21= %d\n",
         u_l  ,v_l  , (int)p_pat21 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
#else   /*  DEF_P21  */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Enter *** varkon_pat_p21eval\n" );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Function is not compiled with constant DEF_P21. Return error\n" );
fflush(dbgfil(SURPAC)); 
}
#endif
       sprintf(errbuf,"DEF_P21 undefined%%sur504");
       return(varkon_erpush("SU2993",errbuf));
#endif  /*  DEF_P21  */


#ifdef  DEF_P21
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output coordinates and derivatives. Call initial.    */
/*                                                                 !*/

   status = initial(icase,p_xyz); 
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function initial failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"initial%%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }

/*!                                                                 */
/* 2. Calculate coordinates and derivatives (non-offset)            */
/* _____________________________________________________            */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Calculate all u**i and v**i, i= 1, 2, 3 , ..                     */
/*                                                                 !*/

   status= c_power(u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function c_power failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_power%%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/* Calculate coordinates. Call of internal function c_coord.        */
/* Goto nomore if icase =  0.                                       */
/*                                                                 !*/

   status= c_coord(p_pat21, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function c_coord failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_coord%%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }

/* Coordinates to output variable p_xyz                             */

   p_xyz->r_x= r_x;
   p_xyz->r_y= r_y;
   p_xyz->r_z= r_z;

   if  (  icase == 0 ) goto nomore;



/*!                                                                 */
/* Calculate dr/du.       Call of internal function c_drdu.         */
/* Goto nomore if icase =  1.                                       */
/*                                                                 !*/

   status= c_drdu (p_pat21, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function c_drdu  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdu %%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }

   p_xyz->u_x= u_x;
   p_xyz->u_y= u_y;
   p_xyz->u_z= u_z;

   if  (  icase == 1 ) goto nomore;


/*!                                                                 */
/* Calculate dr/dv.       Call of internal function c_drdv.         */
/* Goto nomore if icase =  2 or 3.                                  */
/*                                                                 !*/

   status= c_drdv (p_pat21, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function c_drdv  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdv %%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }

   p_xyz->v_x= v_x;
   p_xyz->v_y= v_y;
   p_xyz->v_z= v_z;

   if  (  icase == 2 || icase == 3 ) goto nomore;


/*!                                                                 */
/* Calculate second derivatives. Call of internal function c_second */
/*                                                                 !*/

   status= c_second(p_pat21, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error Internal function c_second failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_second%%sur504");
       return(varkon_erpush("SU2973",errbuf));
       }


   p_xyz->u2_x= u2_x;
   p_xyz->u2_y= u2_y;
   p_xyz->u2_z= u2_z;

   p_xyz->v2_x= v2_x;
   p_xyz->v2_y= v2_y;
   p_xyz->v2_z= v2_z;

   p_xyz->uv_x= uv_x;
   p_xyz->uv_y= uv_y;
   p_xyz->uv_z= uv_z;

nomore:; /*! Label: No more derivativatives                        !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur504 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
if ( dbglev(SURPAC) == 1 && icase >= 3 )
  {
  fprintf(dbgfil(SURPAC),
  "sur504 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur504 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  }
if ( dbglev(SURPAC) == 2 && icase >= 4 )
  {
  fprintf(dbgfil(SURPAC),
  "sur504 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur504 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur504 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur504 Exit*varkon_pat_p21eval* p_xyz %f %f %f\n",
    p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
  }
#endif

#endif  /*  DEF_P21  */

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef  DEF_P21
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF and I_UNDEF.                */

   static short initial ( icase, p_xyz )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  icase;         /* Calculation case                        */
   EVALS  *p_xyz;        /* Coordinates and derivatives       (ptr) */

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

/*!                                                                 */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*initial: icase= %d p_xyz= %d\n",(short)icase,(int)p_xyz);
fflush(dbgfil(SURPAC)); 
}

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur504 Error icase= %d\n", (short)icase );
fflush(dbgfil(SURPAC)); 
}
#endif

        sprintf(errbuf,"%d%% varkon_pat_p21eval (sur504)",(short)icase);
        return(varkon_erpush("SU2983",errbuf));
        }

/*!                                                                 */
/* 2. Initialize output variables EVALS and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   

    r_x= F_UNDEF;   
    r_y= F_UNDEF;   
    r_z= F_UNDEF;   

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;   
    p_xyz->u_y= F_UNDEF;   
    p_xyz->u_z= F_UNDEF;   

    u_x= F_UNDEF;   
    u_y= F_UNDEF;   
    u_z= F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

    v_x= F_UNDEF;   
    v_y= F_UNDEF;   
    v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

    u2_x= F_UNDEF;   
    u2_y= F_UNDEF;   
    u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

    v2_x= F_UNDEF;   
    v2_y= F_UNDEF;   
    v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

    uv_x= F_UNDEF;   
    uv_y= F_UNDEF;   
    uv_z= F_UNDEF;   

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

/*!********* Internal ** function ** c_power ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates u**i and v**i, i= 1,2,3,4, ...           */

   static short c_power ( u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBfloat    u_l;       /* Patch (local) U parameter value         */
   DBfloat    v_l;       /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Static variables pow_u_0, pow_u_1, ...........             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_power u_l= %f v_l= %f\n",u_l,v_l);
}
#endif


   pow_u_0  = pow( u_l, 0.0 );
   pow_u_1  = pow( u_l, 1.0 );
   pow_u_2  = pow( u_l, 2.0 );
   pow_u_3  = pow( u_l, 3.0 );
   pow_u_4  = pow( u_l, 4.0 );
   pow_u_5  = pow( u_l, 5.0 );
   pow_u_6  = pow( u_l, 6.0 );
   pow_u_7  = pow( u_l, 7.0 );
   pow_u_8  = pow( u_l, 8.0 );
   pow_u_9  = pow( u_l, 9.0 );
   pow_u_10 = pow( u_l,10.0 );
   pow_u_11 = pow( u_l,11.0 );
   pow_u_12 = pow( u_l,12.0 );
   pow_u_13 = pow( u_l,13.0 );
   pow_u_14 = pow( u_l,14.0 );
   pow_u_15 = pow( u_l,15.0 );
   pow_u_16 = pow( u_l,16.0 );
   pow_u_17 = pow( u_l,17.0 );
   pow_u_18 = pow( u_l,18.0 );
   pow_u_19 = pow( u_l,19.0 );
   pow_u_20 = pow( u_l,20.0 );
   pow_u_21 = pow( u_l,21.0 );
   pow_v_0  = pow( v_l, 0.0 );
   pow_v_1  = pow( v_l, 1.0 );
   pow_v_2  = pow( v_l, 2.0 );
   pow_v_3  = pow( v_l, 3.0 );
   pow_v_4  = pow( v_l, 4.0 );
   pow_v_5  = pow( v_l, 5.0 );
   pow_v_6  = pow( v_l, 6.0 );
   pow_v_7  = pow( v_l, 7.0 );
   pow_v_8  = pow( v_l, 8.0 );
   pow_v_9  = pow( v_l, 9.0 );
   pow_v_10 = pow( v_l,10.0 );
   pow_v_11 = pow( v_l,11.0 );
   pow_v_12 = pow( v_l,12.0 );
   pow_v_13 = pow( v_l,13.0 );
   pow_v_14 = pow( v_l,14.0 );
   pow_v_15 = pow( v_l,15.0 );
   pow_v_16 = pow( v_l,16.0 );
   pow_v_17 = pow( v_l,17.0 );
   pow_v_18 = pow( v_l,18.0 );
   pow_v_19 = pow( v_l,19.0 );
   pow_v_20 = pow( v_l,20.0 );
   pow_v_21 = pow( v_l,21.0 );




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur504*c_power Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ** c_coord ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates coordinates for GMPATP21                 */

   static short c_coord ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP21  *p_p;       /* Patch GMPATP21                    (ptr) */
   DBfloat    u_l;       /* Patch (local) U parameter value         */
   DBfloat    v_l;       /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Coordinates r_x, r_y, r_z                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_coord p_p= %d  u_l= %f v_l= %f\n",(int)p_p,u_l,v_l);
}
#endif


r_x     = p_p->c0000x * pow_u_0   * pow_v_0  
        + p_p->c0001x * pow_u_0   * pow_v_1  
        + p_p->c0002x * pow_u_0   * pow_v_2  
        + p_p->c0003x * pow_u_0   * pow_v_3  
        + p_p->c0004x * pow_u_0   * pow_v_4  
        + p_p->c0005x * pow_u_0   * pow_v_5  
        + p_p->c0006x * pow_u_0   * pow_v_6  
        + p_p->c0007x * pow_u_0   * pow_v_7  
        + p_p->c0008x * pow_u_0   * pow_v_8  
        + p_p->c0009x * pow_u_0   * pow_v_9  
        + p_p->c0010x * pow_u_0   * pow_u_10 
        + p_p->c0011x * pow_u_0   * pow_u_11 
        + p_p->c0012x * pow_u_0   * pow_u_12 
        + p_p->c0013x * pow_u_0   * pow_u_13 
        + p_p->c0014x * pow_u_0   * pow_u_14 
        + p_p->c0015x * pow_u_0   * pow_u_15 
        + p_p->c0016x * pow_u_0   * pow_u_16 
        + p_p->c0017x * pow_u_0   * pow_u_17 
        + p_p->c0018x * pow_u_0   * pow_u_18 
        + p_p->c0019x * pow_u_0   * pow_u_19 
        + p_p->c0020x * pow_u_0   * pow_u_20 
        + p_p->c0021x * pow_u_0   * pow_u_21 ;
r_x=r_x + p_p->c0100x * pow_u_1   * pow_v_0  
        + p_p->c0101x * pow_u_1   * pow_v_1  
        + p_p->c0102x * pow_u_1   * pow_v_2  
        + p_p->c0103x * pow_u_1   * pow_v_3  
        + p_p->c0104x * pow_u_1   * pow_v_4  
        + p_p->c0105x * pow_u_1   * pow_v_5  
        + p_p->c0106x * pow_u_1   * pow_v_6  
        + p_p->c0107x * pow_u_1   * pow_v_7  
        + p_p->c0108x * pow_u_1   * pow_v_8  
        + p_p->c0109x * pow_u_1   * pow_v_9  
        + p_p->c0110x * pow_u_1   * pow_u_10 
        + p_p->c0111x * pow_u_1   * pow_u_11 
        + p_p->c0112x * pow_u_1   * pow_u_12 
        + p_p->c0113x * pow_u_1   * pow_u_13 
        + p_p->c0114x * pow_u_1   * pow_u_14 
        + p_p->c0115x * pow_u_1   * pow_u_15 
        + p_p->c0116x * pow_u_1   * pow_u_16 
        + p_p->c0117x * pow_u_1   * pow_u_17 
        + p_p->c0118x * pow_u_1   * pow_u_18 
        + p_p->c0119x * pow_u_1   * pow_u_19 
        + p_p->c0120x * pow_u_1   * pow_u_20 
        + p_p->c0121x * pow_u_1   * pow_u_21 ;
r_x=r_x + p_p->c0200x * pow_u_2   * pow_v_0  
        + p_p->c0201x * pow_u_2   * pow_v_1  
        + p_p->c0202x * pow_u_2   * pow_v_2  
        + p_p->c0203x * pow_u_2   * pow_v_3  
        + p_p->c0204x * pow_u_2   * pow_v_4  
        + p_p->c0205x * pow_u_2   * pow_v_5  
        + p_p->c0206x * pow_u_2   * pow_v_6  
        + p_p->c0207x * pow_u_2   * pow_v_7  
        + p_p->c0208x * pow_u_2   * pow_v_8  
        + p_p->c0209x * pow_u_2   * pow_v_9  
        + p_p->c0210x * pow_u_2   * pow_u_10 
        + p_p->c0211x * pow_u_2   * pow_u_11 
        + p_p->c0212x * pow_u_2   * pow_u_12 
        + p_p->c0213x * pow_u_2   * pow_u_13 
        + p_p->c0214x * pow_u_2   * pow_u_14 
        + p_p->c0215x * pow_u_2   * pow_u_15 
        + p_p->c0216x * pow_u_2   * pow_u_16 
        + p_p->c0217x * pow_u_2   * pow_u_17 
        + p_p->c0218x * pow_u_2   * pow_u_18 
        + p_p->c0219x * pow_u_2   * pow_u_19 
        + p_p->c0220x * pow_u_2   * pow_u_20 
        + p_p->c0221x * pow_u_2   * pow_u_21 ;
r_x=r_x + p_p->c0300x * pow_u_3   * pow_v_0  
        + p_p->c0301x * pow_u_3   * pow_v_1  
        + p_p->c0302x * pow_u_3   * pow_v_2  
        + p_p->c0303x * pow_u_3   * pow_v_3  
        + p_p->c0304x * pow_u_3   * pow_v_4  
        + p_p->c0305x * pow_u_3   * pow_v_5  
        + p_p->c0306x * pow_u_3   * pow_v_6  
        + p_p->c0307x * pow_u_3   * pow_v_7  
        + p_p->c0308x * pow_u_3   * pow_v_8  
        + p_p->c0309x * pow_u_3   * pow_v_9  
        + p_p->c0310x * pow_u_3   * pow_u_10 
        + p_p->c0311x * pow_u_3   * pow_u_11 
        + p_p->c0312x * pow_u_3   * pow_u_12 
        + p_p->c0313x * pow_u_3   * pow_u_13 
        + p_p->c0314x * pow_u_3   * pow_u_14 
        + p_p->c0315x * pow_u_3   * pow_u_15 
        + p_p->c0316x * pow_u_3   * pow_u_16 
        + p_p->c0317x * pow_u_3   * pow_u_17 
        + p_p->c0318x * pow_u_3   * pow_u_18 
        + p_p->c0319x * pow_u_3   * pow_u_19 
        + p_p->c0320x * pow_u_3   * pow_u_20 
        + p_p->c0321x * pow_u_3   * pow_u_21 ;
r_x=r_x + p_p->c0400x * pow_u_4   * pow_v_0  
        + p_p->c0401x * pow_u_4   * pow_v_1  
        + p_p->c0402x * pow_u_4   * pow_v_2  
        + p_p->c0403x * pow_u_4   * pow_v_3  
        + p_p->c0404x * pow_u_4   * pow_v_4  
        + p_p->c0405x * pow_u_4   * pow_v_5  
        + p_p->c0406x * pow_u_4   * pow_v_6  
        + p_p->c0407x * pow_u_4   * pow_v_7  
        + p_p->c0408x * pow_u_4   * pow_v_8  
        + p_p->c0409x * pow_u_4   * pow_v_9  
        + p_p->c0410x * pow_u_4   * pow_u_10 
        + p_p->c0411x * pow_u_4   * pow_u_11 
        + p_p->c0412x * pow_u_4   * pow_u_12 
        + p_p->c0413x * pow_u_4   * pow_u_13 
        + p_p->c0414x * pow_u_4   * pow_u_14 
        + p_p->c0415x * pow_u_4   * pow_u_15 
        + p_p->c0416x * pow_u_4   * pow_u_16 
        + p_p->c0417x * pow_u_4   * pow_u_17 
        + p_p->c0418x * pow_u_4   * pow_u_18 
        + p_p->c0419x * pow_u_4   * pow_u_19 
        + p_p->c0420x * pow_u_4   * pow_u_20 
        + p_p->c0421x * pow_u_4   * pow_u_21 ;
r_x=r_x + p_p->c0500x * pow_u_5   * pow_v_0  
        + p_p->c0501x * pow_u_5   * pow_v_1  
        + p_p->c0502x * pow_u_5   * pow_v_2  
        + p_p->c0503x * pow_u_5   * pow_v_3  
        + p_p->c0504x * pow_u_5   * pow_v_4  
        + p_p->c0505x * pow_u_5   * pow_v_5  
        + p_p->c0506x * pow_u_5   * pow_v_6  
        + p_p->c0507x * pow_u_5   * pow_v_7  
        + p_p->c0508x * pow_u_5   * pow_v_8  
        + p_p->c0509x * pow_u_5   * pow_v_9  
        + p_p->c0510x * pow_u_5   * pow_u_10 
        + p_p->c0511x * pow_u_5   * pow_u_11 
        + p_p->c0512x * pow_u_5   * pow_u_12 
        + p_p->c0513x * pow_u_5   * pow_u_13 
        + p_p->c0514x * pow_u_5   * pow_u_14 
        + p_p->c0515x * pow_u_5   * pow_u_15 
        + p_p->c0516x * pow_u_5   * pow_u_16 
        + p_p->c0517x * pow_u_5   * pow_u_17 
        + p_p->c0518x * pow_u_5   * pow_u_18 
        + p_p->c0519x * pow_u_5   * pow_u_19 
        + p_p->c0520x * pow_u_5   * pow_u_20 
        + p_p->c0521x * pow_u_5   * pow_u_21 ;
r_x=r_x + p_p->c0600x * pow_u_6   * pow_v_0  
        + p_p->c0601x * pow_u_6   * pow_v_1  
        + p_p->c0602x * pow_u_6   * pow_v_2  
        + p_p->c0603x * pow_u_6   * pow_v_3  
        + p_p->c0604x * pow_u_6   * pow_v_4  
        + p_p->c0605x * pow_u_6   * pow_v_5  
        + p_p->c0606x * pow_u_6   * pow_v_6  
        + p_p->c0607x * pow_u_6   * pow_v_7  
        + p_p->c0608x * pow_u_6   * pow_v_8  
        + p_p->c0609x * pow_u_6   * pow_v_9  
        + p_p->c0610x * pow_u_6   * pow_u_10 
        + p_p->c0611x * pow_u_6   * pow_u_11 
        + p_p->c0612x * pow_u_6   * pow_u_12 
        + p_p->c0613x * pow_u_6   * pow_u_13 
        + p_p->c0614x * pow_u_6   * pow_u_14 
        + p_p->c0615x * pow_u_6   * pow_u_15 
        + p_p->c0616x * pow_u_6   * pow_u_16 
        + p_p->c0617x * pow_u_6   * pow_u_17 
        + p_p->c0618x * pow_u_6   * pow_u_18 
        + p_p->c0619x * pow_u_6   * pow_u_19 
        + p_p->c0620x * pow_u_6   * pow_u_20 
        + p_p->c0621x * pow_u_6   * pow_u_21 ;
r_x=r_x + p_p->c0700x * pow_u_7   * pow_v_0  
        + p_p->c0701x * pow_u_7   * pow_v_1  
        + p_p->c0702x * pow_u_7   * pow_v_2  
        + p_p->c0703x * pow_u_7   * pow_v_3  
        + p_p->c0704x * pow_u_7   * pow_v_4  
        + p_p->c0705x * pow_u_7   * pow_v_5  
        + p_p->c0706x * pow_u_7   * pow_v_6  
        + p_p->c0707x * pow_u_7   * pow_v_7  
        + p_p->c0708x * pow_u_7   * pow_v_8  
        + p_p->c0709x * pow_u_7   * pow_v_9  
        + p_p->c0710x * pow_u_7   * pow_u_10 
        + p_p->c0711x * pow_u_7   * pow_u_11 
        + p_p->c0712x * pow_u_7   * pow_u_12 
        + p_p->c0713x * pow_u_7   * pow_u_13 
        + p_p->c0714x * pow_u_7   * pow_u_14 
        + p_p->c0715x * pow_u_7   * pow_u_15 
        + p_p->c0716x * pow_u_7   * pow_u_16 
        + p_p->c0717x * pow_u_7   * pow_u_17 
        + p_p->c0718x * pow_u_7   * pow_u_18 
        + p_p->c0719x * pow_u_7   * pow_u_19 
        + p_p->c0720x * pow_u_7   * pow_u_20 
        + p_p->c0721x * pow_u_7   * pow_u_21 ;
r_x=r_x + p_p->c0800x * pow_u_8   * pow_v_0  
        + p_p->c0801x * pow_u_8   * pow_v_1  
        + p_p->c0802x * pow_u_8   * pow_v_2  
        + p_p->c0803x * pow_u_8   * pow_v_3  
        + p_p->c0804x * pow_u_8   * pow_v_4  
        + p_p->c0805x * pow_u_8   * pow_v_5  
        + p_p->c0806x * pow_u_8   * pow_v_6  
        + p_p->c0807x * pow_u_8   * pow_v_7  
        + p_p->c0808x * pow_u_8   * pow_v_8  
        + p_p->c0809x * pow_u_8   * pow_v_9  
        + p_p->c0810x * pow_u_8   * pow_u_10 
        + p_p->c0811x * pow_u_8   * pow_u_11 
        + p_p->c0812x * pow_u_8   * pow_u_12 
        + p_p->c0813x * pow_u_8   * pow_u_13 
        + p_p->c0814x * pow_u_8   * pow_u_14 
        + p_p->c0815x * pow_u_8   * pow_u_15 
        + p_p->c0816x * pow_u_8   * pow_u_16 
        + p_p->c0817x * pow_u_8   * pow_u_17 
        + p_p->c0818x * pow_u_8   * pow_u_18 
        + p_p->c0819x * pow_u_8   * pow_u_19 
        + p_p->c0820x * pow_u_8   * pow_u_20 
        + p_p->c0821x * pow_u_8   * pow_u_21 ;
r_x=r_x + p_p->c0900x * pow_u_9   * pow_v_0  
        + p_p->c0901x * pow_u_9   * pow_v_1  
        + p_p->c0902x * pow_u_9   * pow_v_2  
        + p_p->c0903x * pow_u_9   * pow_v_3  
        + p_p->c0904x * pow_u_9   * pow_v_4  
        + p_p->c0905x * pow_u_9   * pow_v_5  
        + p_p->c0906x * pow_u_9   * pow_v_6  
        + p_p->c0907x * pow_u_9   * pow_v_7  
        + p_p->c0908x * pow_u_9   * pow_v_8  
        + p_p->c0909x * pow_u_9   * pow_v_9  
        + p_p->c0910x * pow_u_9   * pow_u_10 
        + p_p->c0911x * pow_u_9   * pow_u_11 
        + p_p->c0912x * pow_u_9   * pow_u_12 
        + p_p->c0913x * pow_u_9   * pow_u_13 
        + p_p->c0914x * pow_u_9   * pow_u_14 
        + p_p->c0915x * pow_u_9   * pow_u_15 
        + p_p->c0916x * pow_u_9   * pow_u_16 
        + p_p->c0917x * pow_u_9   * pow_u_17 
        + p_p->c0918x * pow_u_9   * pow_u_18 
        + p_p->c0919x * pow_u_9   * pow_u_19 
        + p_p->c0920x * pow_u_9   * pow_u_20 
        + p_p->c0921x * pow_u_9   * pow_u_21 ;
r_x=r_x + p_p->c1000x * pow_u_10  * pow_v_0  
        + p_p->c1001x * pow_u_10  * pow_v_1  
        + p_p->c1002x * pow_u_10  * pow_v_2  
        + p_p->c1003x * pow_u_10  * pow_v_3  
        + p_p->c1004x * pow_u_10  * pow_v_4  
        + p_p->c1005x * pow_u_10  * pow_v_5  
        + p_p->c1006x * pow_u_10  * pow_v_6  
        + p_p->c1007x * pow_u_10  * pow_v_7  
        + p_p->c1008x * pow_u_10  * pow_v_8  
        + p_p->c1009x * pow_u_10  * pow_v_9  
        + p_p->c1010x * pow_u_10  * pow_u_10 
        + p_p->c1011x * pow_u_10  * pow_u_11 
        + p_p->c1012x * pow_u_10  * pow_u_12 
        + p_p->c1013x * pow_u_10  * pow_u_13 
        + p_p->c1014x * pow_u_10  * pow_u_14 
        + p_p->c1015x * pow_u_10  * pow_u_15 
        + p_p->c1016x * pow_u_10  * pow_u_16 
        + p_p->c1017x * pow_u_10  * pow_u_17 
        + p_p->c1018x * pow_u_10  * pow_u_18 
        + p_p->c1019x * pow_u_10  * pow_u_19 
        + p_p->c1020x * pow_u_10  * pow_u_20 
        + p_p->c1021x * pow_u_10  * pow_u_21 ;
r_x=r_x + p_p->c1100x * pow_u_11  * pow_v_0  
        + p_p->c1101x * pow_u_11  * pow_v_1  
        + p_p->c1102x * pow_u_11  * pow_v_2  
        + p_p->c1103x * pow_u_11  * pow_v_3  
        + p_p->c1104x * pow_u_11  * pow_v_4  
        + p_p->c1105x * pow_u_11  * pow_v_5  
        + p_p->c1106x * pow_u_11  * pow_v_6  
        + p_p->c1107x * pow_u_11  * pow_v_7  
        + p_p->c1108x * pow_u_11  * pow_v_8  
        + p_p->c1109x * pow_u_11  * pow_v_9  
        + p_p->c1110x * pow_u_11  * pow_u_10 
        + p_p->c1111x * pow_u_11  * pow_u_11 
        + p_p->c1112x * pow_u_11  * pow_u_12 
        + p_p->c1113x * pow_u_11  * pow_u_13 
        + p_p->c1114x * pow_u_11  * pow_u_14 
        + p_p->c1115x * pow_u_11  * pow_u_15 
        + p_p->c1116x * pow_u_11  * pow_u_16 
        + p_p->c1117x * pow_u_11  * pow_u_17 
        + p_p->c1118x * pow_u_11  * pow_u_18 
        + p_p->c1119x * pow_u_11  * pow_u_19 
        + p_p->c1120x * pow_u_11  * pow_u_20 
        + p_p->c1121x * pow_u_11  * pow_u_21 ;
r_x=r_x + p_p->c1200x * pow_u_12  * pow_v_0  
        + p_p->c1201x * pow_u_12  * pow_v_1  
        + p_p->c1202x * pow_u_12  * pow_v_2  
        + p_p->c1203x * pow_u_12  * pow_v_3  
        + p_p->c1204x * pow_u_12  * pow_v_4  
        + p_p->c1205x * pow_u_12  * pow_v_5  
        + p_p->c1206x * pow_u_12  * pow_v_6  
        + p_p->c1207x * pow_u_12  * pow_v_7  
        + p_p->c1208x * pow_u_12  * pow_v_8  
        + p_p->c1209x * pow_u_12  * pow_v_9  
        + p_p->c1210x * pow_u_12  * pow_u_10 
        + p_p->c1211x * pow_u_12  * pow_u_11 
        + p_p->c1212x * pow_u_12  * pow_u_12 
        + p_p->c1213x * pow_u_12  * pow_u_13 
        + p_p->c1214x * pow_u_12  * pow_u_14 
        + p_p->c1215x * pow_u_12  * pow_u_15 
        + p_p->c1216x * pow_u_12  * pow_u_16 
        + p_p->c1217x * pow_u_12  * pow_u_17 
        + p_p->c1218x * pow_u_12  * pow_u_18 
        + p_p->c1219x * pow_u_12  * pow_u_19 
        + p_p->c1220x * pow_u_12  * pow_u_20 
        + p_p->c1221x * pow_u_12  * pow_u_21 ;
r_x=r_x + p_p->c1300x * pow_u_13  * pow_v_0  
        + p_p->c1301x * pow_u_13  * pow_v_1  
        + p_p->c1302x * pow_u_13  * pow_v_2  
        + p_p->c1303x * pow_u_13  * pow_v_3  
        + p_p->c1304x * pow_u_13  * pow_v_4  
        + p_p->c1305x * pow_u_13  * pow_v_5  
        + p_p->c1306x * pow_u_13  * pow_v_6  
        + p_p->c1307x * pow_u_13  * pow_v_7  
        + p_p->c1308x * pow_u_13  * pow_v_8  
        + p_p->c1309x * pow_u_13  * pow_v_9  
        + p_p->c1310x * pow_u_13  * pow_u_10 
        + p_p->c1311x * pow_u_13  * pow_u_11 
        + p_p->c1312x * pow_u_13  * pow_u_12 
        + p_p->c1313x * pow_u_13  * pow_u_13 
        + p_p->c1314x * pow_u_13  * pow_u_14 
        + p_p->c1315x * pow_u_13  * pow_u_15 
        + p_p->c1316x * pow_u_13  * pow_u_16 
        + p_p->c1317x * pow_u_13  * pow_u_17 
        + p_p->c1318x * pow_u_13  * pow_u_18 
        + p_p->c1319x * pow_u_13  * pow_u_19 
        + p_p->c1320x * pow_u_13  * pow_u_20 
        + p_p->c1321x * pow_u_13  * pow_u_21 ;
r_x=r_x + p_p->c1400x * pow_u_14  * pow_v_0  
        + p_p->c1401x * pow_u_14  * pow_v_1  
        + p_p->c1402x * pow_u_14  * pow_v_2  
        + p_p->c1403x * pow_u_14  * pow_v_3  
        + p_p->c1404x * pow_u_14  * pow_v_4  
        + p_p->c1405x * pow_u_14  * pow_v_5  
        + p_p->c1406x * pow_u_14  * pow_v_6  
        + p_p->c1407x * pow_u_14  * pow_v_7  
        + p_p->c1408x * pow_u_14  * pow_v_8  
        + p_p->c1409x * pow_u_14  * pow_v_9  
        + p_p->c1410x * pow_u_14  * pow_u_10 
        + p_p->c1411x * pow_u_14  * pow_u_11 
        + p_p->c1412x * pow_u_14  * pow_u_12 
        + p_p->c1413x * pow_u_14  * pow_u_13 
        + p_p->c1414x * pow_u_14  * pow_u_14 
        + p_p->c1415x * pow_u_14  * pow_u_15 
        + p_p->c1416x * pow_u_14  * pow_u_16 
        + p_p->c1417x * pow_u_14  * pow_u_17 
        + p_p->c1418x * pow_u_14  * pow_u_18 
        + p_p->c1419x * pow_u_14  * pow_u_19 
        + p_p->c1420x * pow_u_14  * pow_u_20 
        + p_p->c1421x * pow_u_14  * pow_u_21 ;
r_x=r_x + p_p->c1500x * pow_u_15  * pow_v_0  
        + p_p->c1501x * pow_u_15  * pow_v_1  
        + p_p->c1502x * pow_u_15  * pow_v_2  
        + p_p->c1503x * pow_u_15  * pow_v_3  
        + p_p->c1504x * pow_u_15  * pow_v_4  
        + p_p->c1505x * pow_u_15  * pow_v_5  
        + p_p->c1506x * pow_u_15  * pow_v_6  
        + p_p->c1507x * pow_u_15  * pow_v_7  
        + p_p->c1508x * pow_u_15  * pow_v_8  
        + p_p->c1509x * pow_u_15  * pow_v_9  
        + p_p->c1510x * pow_u_15  * pow_u_10 
        + p_p->c1511x * pow_u_15  * pow_u_11 
        + p_p->c1512x * pow_u_15  * pow_u_12 
        + p_p->c1513x * pow_u_15  * pow_u_13 
        + p_p->c1514x * pow_u_15  * pow_u_14 
        + p_p->c1515x * pow_u_15  * pow_u_15 
        + p_p->c1516x * pow_u_15  * pow_u_16 
        + p_p->c1517x * pow_u_15  * pow_u_17 
        + p_p->c1518x * pow_u_15  * pow_u_18 
        + p_p->c1519x * pow_u_15  * pow_u_19 
        + p_p->c1520x * pow_u_15  * pow_u_20 
        + p_p->c1521x * pow_u_15  * pow_u_21 ;
r_x=r_x + p_p->c1600x * pow_u_16  * pow_v_0  
        + p_p->c1601x * pow_u_16  * pow_v_1  
        + p_p->c1602x * pow_u_16  * pow_v_2  
        + p_p->c1603x * pow_u_16  * pow_v_3  
        + p_p->c1604x * pow_u_16  * pow_v_4  
        + p_p->c1605x * pow_u_16  * pow_v_5  
        + p_p->c1606x * pow_u_16  * pow_v_6  
        + p_p->c1607x * pow_u_16  * pow_v_7  
        + p_p->c1608x * pow_u_16  * pow_v_8  
        + p_p->c1609x * pow_u_16  * pow_v_9  
        + p_p->c1610x * pow_u_16  * pow_u_10 
        + p_p->c1611x * pow_u_16  * pow_u_11 
        + p_p->c1612x * pow_u_16  * pow_u_12 
        + p_p->c1613x * pow_u_16  * pow_u_13 
        + p_p->c1614x * pow_u_16  * pow_u_14 
        + p_p->c1615x * pow_u_16  * pow_u_15 
        + p_p->c1616x * pow_u_16  * pow_u_16 
        + p_p->c1617x * pow_u_16  * pow_u_17 
        + p_p->c1618x * pow_u_16  * pow_u_18 
        + p_p->c1619x * pow_u_16  * pow_u_19 
        + p_p->c1620x * pow_u_16  * pow_u_20 
        + p_p->c1621x * pow_u_16  * pow_u_21 ;
r_x=r_x + p_p->c1700x * pow_u_17  * pow_v_0  
        + p_p->c1701x * pow_u_17  * pow_v_1  
        + p_p->c1702x * pow_u_17  * pow_v_2  
        + p_p->c1703x * pow_u_17  * pow_v_3  
        + p_p->c1704x * pow_u_17  * pow_v_4  
        + p_p->c1705x * pow_u_17  * pow_v_5  
        + p_p->c1706x * pow_u_17  * pow_v_6  
        + p_p->c1707x * pow_u_17  * pow_v_7  
        + p_p->c1708x * pow_u_17  * pow_v_8  
        + p_p->c1709x * pow_u_17  * pow_v_9  
        + p_p->c1710x * pow_u_17  * pow_u_10 
        + p_p->c1711x * pow_u_17  * pow_u_11 
        + p_p->c1712x * pow_u_17  * pow_u_12 
        + p_p->c1713x * pow_u_17  * pow_u_13 
        + p_p->c1714x * pow_u_17  * pow_u_14 
        + p_p->c1715x * pow_u_17  * pow_u_15 
        + p_p->c1716x * pow_u_17  * pow_u_16 
        + p_p->c1717x * pow_u_17  * pow_u_17 
        + p_p->c1718x * pow_u_17  * pow_u_18 
        + p_p->c1719x * pow_u_17  * pow_u_19 
        + p_p->c1720x * pow_u_17  * pow_u_20 
        + p_p->c1721x * pow_u_17  * pow_u_21 ;
r_x=r_x + p_p->c1800x * pow_u_18  * pow_v_0  
        + p_p->c1801x * pow_u_18  * pow_v_1  
        + p_p->c1802x * pow_u_18  * pow_v_2  
        + p_p->c1803x * pow_u_18  * pow_v_3  
        + p_p->c1804x * pow_u_18  * pow_v_4  
        + p_p->c1805x * pow_u_18  * pow_v_5  
        + p_p->c1806x * pow_u_18  * pow_v_6  
        + p_p->c1807x * pow_u_18  * pow_v_7  
        + p_p->c1808x * pow_u_18  * pow_v_8  
        + p_p->c1809x * pow_u_18  * pow_v_9  
        + p_p->c1810x * pow_u_18  * pow_u_10 
        + p_p->c1811x * pow_u_18  * pow_u_11 
        + p_p->c1812x * pow_u_18  * pow_u_12 
        + p_p->c1813x * pow_u_18  * pow_u_13 
        + p_p->c1814x * pow_u_18  * pow_u_14 
        + p_p->c1815x * pow_u_18  * pow_u_15 
        + p_p->c1816x * pow_u_18  * pow_u_16 
        + p_p->c1817x * pow_u_18  * pow_u_17 
        + p_p->c1818x * pow_u_18  * pow_u_18 
        + p_p->c1819x * pow_u_18  * pow_u_19 
        + p_p->c1820x * pow_u_18  * pow_u_20 
        + p_p->c1821x * pow_u_18  * pow_u_21 ;
r_x=r_x + p_p->c1900x * pow_u_19  * pow_v_0  
        + p_p->c1901x * pow_u_19  * pow_v_1  
        + p_p->c1902x * pow_u_19  * pow_v_2  
        + p_p->c1903x * pow_u_19  * pow_v_3  
        + p_p->c1904x * pow_u_19  * pow_v_4  
        + p_p->c1905x * pow_u_19  * pow_v_5  
        + p_p->c1906x * pow_u_19  * pow_v_6  
        + p_p->c1907x * pow_u_19  * pow_v_7  
        + p_p->c1908x * pow_u_19  * pow_v_8  
        + p_p->c1909x * pow_u_19  * pow_v_9  
        + p_p->c1910x * pow_u_19  * pow_u_10 
        + p_p->c1911x * pow_u_19  * pow_u_11 
        + p_p->c1912x * pow_u_19  * pow_u_12 
        + p_p->c1913x * pow_u_19  * pow_u_13 
        + p_p->c1914x * pow_u_19  * pow_u_14 
        + p_p->c1915x * pow_u_19  * pow_u_15 
        + p_p->c1916x * pow_u_19  * pow_u_16 
        + p_p->c1917x * pow_u_19  * pow_u_17 
        + p_p->c1918x * pow_u_19  * pow_u_18 
        + p_p->c1919x * pow_u_19  * pow_u_19 
        + p_p->c1920x * pow_u_19  * pow_u_20 
        + p_p->c1921x * pow_u_19  * pow_u_21 ;
r_x=r_x + p_p->c2000x * pow_u_20  * pow_v_0  
        + p_p->c2001x * pow_u_20  * pow_v_1  
        + p_p->c2002x * pow_u_20  * pow_v_2  
        + p_p->c2003x * pow_u_20  * pow_v_3  
        + p_p->c2004x * pow_u_20  * pow_v_4  
        + p_p->c2005x * pow_u_20  * pow_v_5  
        + p_p->c2006x * pow_u_20  * pow_v_6  
        + p_p->c2007x * pow_u_20  * pow_v_7  
        + p_p->c2008x * pow_u_20  * pow_v_8  
        + p_p->c2009x * pow_u_20  * pow_v_9  
        + p_p->c2010x * pow_u_20  * pow_u_10 
        + p_p->c2011x * pow_u_20  * pow_u_11 
        + p_p->c2012x * pow_u_20  * pow_u_12 
        + p_p->c2013x * pow_u_20  * pow_u_13 
        + p_p->c2014x * pow_u_20  * pow_u_14 
        + p_p->c2015x * pow_u_20  * pow_u_15 
        + p_p->c2016x * pow_u_20  * pow_u_16 
        + p_p->c2017x * pow_u_20  * pow_u_17 
        + p_p->c2018x * pow_u_20  * pow_u_18 
        + p_p->c2019x * pow_u_20  * pow_u_19 
        + p_p->c2020x * pow_u_20  * pow_u_20 
        + p_p->c2021x * pow_u_20  * pow_u_21 ;
r_x=r_x + p_p->c2100x * pow_u_21  * pow_v_0  
        + p_p->c2101x * pow_u_21  * pow_v_1  
        + p_p->c2102x * pow_u_21  * pow_v_2  
        + p_p->c2103x * pow_u_21  * pow_v_3  
        + p_p->c2104x * pow_u_21  * pow_v_4  
        + p_p->c2105x * pow_u_21  * pow_v_5  
        + p_p->c2106x * pow_u_21  * pow_v_6  
        + p_p->c2107x * pow_u_21  * pow_v_7  
        + p_p->c2108x * pow_u_21  * pow_v_8  
        + p_p->c2109x * pow_u_21  * pow_v_9  
        + p_p->c2110x * pow_u_21  * pow_u_10 
        + p_p->c2111x * pow_u_21  * pow_u_11 
        + p_p->c2112x * pow_u_21  * pow_u_12 
        + p_p->c2113x * pow_u_21  * pow_u_13 
        + p_p->c2114x * pow_u_21  * pow_u_14 
        + p_p->c2115x * pow_u_21  * pow_u_15 
        + p_p->c2116x * pow_u_21  * pow_u_16 
        + p_p->c2117x * pow_u_21  * pow_u_17 
        + p_p->c2118x * pow_u_21  * pow_u_18 
        + p_p->c2119x * pow_u_21  * pow_u_19 
        + p_p->c2120x * pow_u_21  * pow_u_20 
        + p_p->c2121x * pow_u_21  * pow_u_21 ;

r_y     = p_p->c0000y * pow_u_0   * pow_v_0  
        + p_p->c0001y * pow_u_0   * pow_v_1  
        + p_p->c0002y * pow_u_0   * pow_v_2  
        + p_p->c0003y * pow_u_0   * pow_v_3  
        + p_p->c0004y * pow_u_0   * pow_v_4  
        + p_p->c0005y * pow_u_0   * pow_v_5  
        + p_p->c0006y * pow_u_0   * pow_v_6  
        + p_p->c0007y * pow_u_0   * pow_v_7  
        + p_p->c0008y * pow_u_0   * pow_v_8  
        + p_p->c0009y * pow_u_0   * pow_v_9  
        + p_p->c0010y * pow_u_0   * pow_u_10 
        + p_p->c0011y * pow_u_0   * pow_u_11 
        + p_p->c0012y * pow_u_0   * pow_u_12 
        + p_p->c0013y * pow_u_0   * pow_u_13 
        + p_p->c0014y * pow_u_0   * pow_u_14 
        + p_p->c0015y * pow_u_0   * pow_u_15 
        + p_p->c0016y * pow_u_0   * pow_u_16 
        + p_p->c0017y * pow_u_0   * pow_u_17 
        + p_p->c0018y * pow_u_0   * pow_u_18 
        + p_p->c0019y * pow_u_0   * pow_u_19 
        + p_p->c0020y * pow_u_0   * pow_u_20 
        + p_p->c0021y * pow_u_0   * pow_u_21 ;
r_y=r_y + p_p->c0100y * pow_u_1   * pow_v_0  
        + p_p->c0101y * pow_u_1   * pow_v_1  
        + p_p->c0102y * pow_u_1   * pow_v_2  
        + p_p->c0103y * pow_u_1   * pow_v_3  
        + p_p->c0104y * pow_u_1   * pow_v_4  
        + p_p->c0105y * pow_u_1   * pow_v_5  
        + p_p->c0106y * pow_u_1   * pow_v_6  
        + p_p->c0107y * pow_u_1   * pow_v_7  
        + p_p->c0108y * pow_u_1   * pow_v_8  
        + p_p->c0109y * pow_u_1   * pow_v_9  
        + p_p->c0110y * pow_u_1   * pow_u_10 
        + p_p->c0111y * pow_u_1   * pow_u_11 
        + p_p->c0112y * pow_u_1   * pow_u_12 
        + p_p->c0113y * pow_u_1   * pow_u_13 
        + p_p->c0114y * pow_u_1   * pow_u_14 
        + p_p->c0115y * pow_u_1   * pow_u_15 
        + p_p->c0116y * pow_u_1   * pow_u_16 
        + p_p->c0117y * pow_u_1   * pow_u_17 
        + p_p->c0118y * pow_u_1   * pow_u_18 
        + p_p->c0119y * pow_u_1   * pow_u_19 
        + p_p->c0120y * pow_u_1   * pow_u_20 
        + p_p->c0121y * pow_u_1   * pow_u_21 ;
r_y=r_y + p_p->c0200y * pow_u_2   * pow_v_0  
        + p_p->c0201y * pow_u_2   * pow_v_1  
        + p_p->c0202y * pow_u_2   * pow_v_2  
        + p_p->c0203y * pow_u_2   * pow_v_3  
        + p_p->c0204y * pow_u_2   * pow_v_4  
        + p_p->c0205y * pow_u_2   * pow_v_5  
        + p_p->c0206y * pow_u_2   * pow_v_6  
        + p_p->c0207y * pow_u_2   * pow_v_7  
        + p_p->c0208y * pow_u_2   * pow_v_8  
        + p_p->c0209y * pow_u_2   * pow_v_9  
        + p_p->c0210y * pow_u_2   * pow_u_10 
        + p_p->c0211y * pow_u_2   * pow_u_11 
        + p_p->c0212y * pow_u_2   * pow_u_12 
        + p_p->c0213y * pow_u_2   * pow_u_13 
        + p_p->c0214y * pow_u_2   * pow_u_14 
        + p_p->c0215y * pow_u_2   * pow_u_15 
        + p_p->c0216y * pow_u_2   * pow_u_16 
        + p_p->c0217y * pow_u_2   * pow_u_17 
        + p_p->c0218y * pow_u_2   * pow_u_18 
        + p_p->c0219y * pow_u_2   * pow_u_19 
        + p_p->c0220y * pow_u_2   * pow_u_20 
        + p_p->c0221y * pow_u_2   * pow_u_21 ;
r_y=r_y + p_p->c0300y * pow_u_3   * pow_v_0  
        + p_p->c0301y * pow_u_3   * pow_v_1  
        + p_p->c0302y * pow_u_3   * pow_v_2  
        + p_p->c0303y * pow_u_3   * pow_v_3  
        + p_p->c0304y * pow_u_3   * pow_v_4  
        + p_p->c0305y * pow_u_3   * pow_v_5  
        + p_p->c0306y * pow_u_3   * pow_v_6  
        + p_p->c0307y * pow_u_3   * pow_v_7  
        + p_p->c0308y * pow_u_3   * pow_v_8  
        + p_p->c0309y * pow_u_3   * pow_v_9  
        + p_p->c0310y * pow_u_3   * pow_u_10 
        + p_p->c0311y * pow_u_3   * pow_u_11 
        + p_p->c0312y * pow_u_3   * pow_u_12 
        + p_p->c0313y * pow_u_3   * pow_u_13 
        + p_p->c0314y * pow_u_3   * pow_u_14 
        + p_p->c0315y * pow_u_3   * pow_u_15 
        + p_p->c0316y * pow_u_3   * pow_u_16 
        + p_p->c0317y * pow_u_3   * pow_u_17 
        + p_p->c0318y * pow_u_3   * pow_u_18 
        + p_p->c0319y * pow_u_3   * pow_u_19 
        + p_p->c0320y * pow_u_3   * pow_u_20 
        + p_p->c0321y * pow_u_3   * pow_u_21 ;
r_y=r_y + p_p->c0400y * pow_u_4   * pow_v_0  
        + p_p->c0401y * pow_u_4   * pow_v_1  
        + p_p->c0402y * pow_u_4   * pow_v_2  
        + p_p->c0403y * pow_u_4   * pow_v_3  
        + p_p->c0404y * pow_u_4   * pow_v_4  
        + p_p->c0405y * pow_u_4   * pow_v_5  
        + p_p->c0406y * pow_u_4   * pow_v_6  
        + p_p->c0407y * pow_u_4   * pow_v_7  
        + p_p->c0408y * pow_u_4   * pow_v_8  
        + p_p->c0409y * pow_u_4   * pow_v_9  
        + p_p->c0410y * pow_u_4   * pow_u_10 
        + p_p->c0411y * pow_u_4   * pow_u_11 
        + p_p->c0412y * pow_u_4   * pow_u_12 
        + p_p->c0413y * pow_u_4   * pow_u_13 
        + p_p->c0414y * pow_u_4   * pow_u_14 
        + p_p->c0415y * pow_u_4   * pow_u_15 
        + p_p->c0416y * pow_u_4   * pow_u_16 
        + p_p->c0417y * pow_u_4   * pow_u_17 
        + p_p->c0418y * pow_u_4   * pow_u_18 
        + p_p->c0419y * pow_u_4   * pow_u_19 
        + p_p->c0420y * pow_u_4   * pow_u_20 
        + p_p->c0421y * pow_u_4   * pow_u_21 ;
r_y=r_y + p_p->c0500y * pow_u_5   * pow_v_0  
        + p_p->c0501y * pow_u_5   * pow_v_1  
        + p_p->c0502y * pow_u_5   * pow_v_2  
        + p_p->c0503y * pow_u_5   * pow_v_3  
        + p_p->c0504y * pow_u_5   * pow_v_4  
        + p_p->c0505y * pow_u_5   * pow_v_5  
        + p_p->c0506y * pow_u_5   * pow_v_6  
        + p_p->c0507y * pow_u_5   * pow_v_7  
        + p_p->c0508y * pow_u_5   * pow_v_8  
        + p_p->c0509y * pow_u_5   * pow_v_9  
        + p_p->c0510y * pow_u_5   * pow_u_10 
        + p_p->c0511y * pow_u_5   * pow_u_11 
        + p_p->c0512y * pow_u_5   * pow_u_12 
        + p_p->c0513y * pow_u_5   * pow_u_13 
        + p_p->c0514y * pow_u_5   * pow_u_14 
        + p_p->c0515y * pow_u_5   * pow_u_15 
        + p_p->c0516y * pow_u_5   * pow_u_16 
        + p_p->c0517y * pow_u_5   * pow_u_17 
        + p_p->c0518y * pow_u_5   * pow_u_18 
        + p_p->c0519y * pow_u_5   * pow_u_19 
        + p_p->c0520y * pow_u_5   * pow_u_20 
        + p_p->c0521y * pow_u_5   * pow_u_21 ;
r_y=r_y + p_p->c0600y * pow_u_6   * pow_v_0  
        + p_p->c0601y * pow_u_6   * pow_v_1  
        + p_p->c0602y * pow_u_6   * pow_v_2  
        + p_p->c0603y * pow_u_6   * pow_v_3  
        + p_p->c0604y * pow_u_6   * pow_v_4  
        + p_p->c0605y * pow_u_6   * pow_v_5  
        + p_p->c0606y * pow_u_6   * pow_v_6  
        + p_p->c0607y * pow_u_6   * pow_v_7  
        + p_p->c0608y * pow_u_6   * pow_v_8  
        + p_p->c0609y * pow_u_6   * pow_v_9  
        + p_p->c0610y * pow_u_6   * pow_u_10 
        + p_p->c0611y * pow_u_6   * pow_u_11 
        + p_p->c0612y * pow_u_6   * pow_u_12 
        + p_p->c0613y * pow_u_6   * pow_u_13 
        + p_p->c0614y * pow_u_6   * pow_u_14 
        + p_p->c0615y * pow_u_6   * pow_u_15 
        + p_p->c0616y * pow_u_6   * pow_u_16 
        + p_p->c0617y * pow_u_6   * pow_u_17 
        + p_p->c0618y * pow_u_6   * pow_u_18 
        + p_p->c0619y * pow_u_6   * pow_u_19 
        + p_p->c0620y * pow_u_6   * pow_u_20 
        + p_p->c0621y * pow_u_6   * pow_u_21 ;
r_y=r_y + p_p->c0700y * pow_u_7   * pow_v_0  
        + p_p->c0701y * pow_u_7   * pow_v_1  
        + p_p->c0702y * pow_u_7   * pow_v_2  
        + p_p->c0703y * pow_u_7   * pow_v_3  
        + p_p->c0704y * pow_u_7   * pow_v_4  
        + p_p->c0705y * pow_u_7   * pow_v_5  
        + p_p->c0706y * pow_u_7   * pow_v_6  
        + p_p->c0707y * pow_u_7   * pow_v_7  
        + p_p->c0708y * pow_u_7   * pow_v_8  
        + p_p->c0709y * pow_u_7   * pow_v_9  
        + p_p->c0710y * pow_u_7   * pow_u_10 
        + p_p->c0711y * pow_u_7   * pow_u_11 
        + p_p->c0712y * pow_u_7   * pow_u_12 
        + p_p->c0713y * pow_u_7   * pow_u_13 
        + p_p->c0714y * pow_u_7   * pow_u_14 
        + p_p->c0715y * pow_u_7   * pow_u_15 
        + p_p->c0716y * pow_u_7   * pow_u_16 
        + p_p->c0717y * pow_u_7   * pow_u_17 
        + p_p->c0718y * pow_u_7   * pow_u_18 
        + p_p->c0719y * pow_u_7   * pow_u_19 
        + p_p->c0720y * pow_u_7   * pow_u_20 
        + p_p->c0721y * pow_u_7   * pow_u_21 ;
r_y=r_y + p_p->c0800y * pow_u_8   * pow_v_0  
        + p_p->c0801y * pow_u_8   * pow_v_1  
        + p_p->c0802y * pow_u_8   * pow_v_2  
        + p_p->c0803y * pow_u_8   * pow_v_3  
        + p_p->c0804y * pow_u_8   * pow_v_4  
        + p_p->c0805y * pow_u_8   * pow_v_5  
        + p_p->c0806y * pow_u_8   * pow_v_6  
        + p_p->c0807y * pow_u_8   * pow_v_7  
        + p_p->c0808y * pow_u_8   * pow_v_8  
        + p_p->c0809y * pow_u_8   * pow_v_9  
        + p_p->c0810y * pow_u_8   * pow_u_10 
        + p_p->c0811y * pow_u_8   * pow_u_11 
        + p_p->c0812y * pow_u_8   * pow_u_12 
        + p_p->c0813y * pow_u_8   * pow_u_13 
        + p_p->c0814y * pow_u_8   * pow_u_14 
        + p_p->c0815y * pow_u_8   * pow_u_15 
        + p_p->c0816y * pow_u_8   * pow_u_16 
        + p_p->c0817y * pow_u_8   * pow_u_17 
        + p_p->c0818y * pow_u_8   * pow_u_18 
        + p_p->c0819y * pow_u_8   * pow_u_19 
        + p_p->c0820y * pow_u_8   * pow_u_20 
        + p_p->c0821y * pow_u_8   * pow_u_21 ;
r_y=r_y + p_p->c0900y * pow_u_9   * pow_v_0  
        + p_p->c0901y * pow_u_9   * pow_v_1  
        + p_p->c0902y * pow_u_9   * pow_v_2  
        + p_p->c0903y * pow_u_9   * pow_v_3  
        + p_p->c0904y * pow_u_9   * pow_v_4  
        + p_p->c0905y * pow_u_9   * pow_v_5  
        + p_p->c0906y * pow_u_9   * pow_v_6  
        + p_p->c0907y * pow_u_9   * pow_v_7  
        + p_p->c0908y * pow_u_9   * pow_v_8  
        + p_p->c0909y * pow_u_9   * pow_v_9  
        + p_p->c0910y * pow_u_9   * pow_u_10 
        + p_p->c0911y * pow_u_9   * pow_u_11 
        + p_p->c0912y * pow_u_9   * pow_u_12 
        + p_p->c0913y * pow_u_9   * pow_u_13 
        + p_p->c0914y * pow_u_9   * pow_u_14 
        + p_p->c0915y * pow_u_9   * pow_u_15 
        + p_p->c0916y * pow_u_9   * pow_u_16 
        + p_p->c0917y * pow_u_9   * pow_u_17 
        + p_p->c0918y * pow_u_9   * pow_u_18 
        + p_p->c0919y * pow_u_9   * pow_u_19 
        + p_p->c0920y * pow_u_9   * pow_u_20 
        + p_p->c0921y * pow_u_9   * pow_u_21 ;
r_y=r_y + p_p->c1000y * pow_u_10  * pow_v_0  
        + p_p->c1001y * pow_u_10  * pow_v_1  
        + p_p->c1002y * pow_u_10  * pow_v_2  
        + p_p->c1003y * pow_u_10  * pow_v_3  
        + p_p->c1004y * pow_u_10  * pow_v_4  
        + p_p->c1005y * pow_u_10  * pow_v_5  
        + p_p->c1006y * pow_u_10  * pow_v_6  
        + p_p->c1007y * pow_u_10  * pow_v_7  
        + p_p->c1008y * pow_u_10  * pow_v_8  
        + p_p->c1009y * pow_u_10  * pow_v_9  
        + p_p->c1010y * pow_u_10  * pow_u_10 
        + p_p->c1011y * pow_u_10  * pow_u_11 
        + p_p->c1012y * pow_u_10  * pow_u_12 
        + p_p->c1013y * pow_u_10  * pow_u_13 
        + p_p->c1014y * pow_u_10  * pow_u_14 
        + p_p->c1015y * pow_u_10  * pow_u_15 
        + p_p->c1016y * pow_u_10  * pow_u_16 
        + p_p->c1017y * pow_u_10  * pow_u_17 
        + p_p->c1018y * pow_u_10  * pow_u_18 
        + p_p->c1019y * pow_u_10  * pow_u_19 
        + p_p->c1020y * pow_u_10  * pow_u_20 
        + p_p->c1021y * pow_u_10  * pow_u_21 ;
r_y=r_y + p_p->c1100y * pow_u_11  * pow_v_0  
        + p_p->c1101y * pow_u_11  * pow_v_1  
        + p_p->c1102y * pow_u_11  * pow_v_2  
        + p_p->c1103y * pow_u_11  * pow_v_3  
        + p_p->c1104y * pow_u_11  * pow_v_4  
        + p_p->c1105y * pow_u_11  * pow_v_5  
        + p_p->c1106y * pow_u_11  * pow_v_6  
        + p_p->c1107y * pow_u_11  * pow_v_7  
        + p_p->c1108y * pow_u_11  * pow_v_8  
        + p_p->c1109y * pow_u_11  * pow_v_9  
        + p_p->c1110y * pow_u_11  * pow_u_10 
        + p_p->c1111y * pow_u_11  * pow_u_11 
        + p_p->c1112y * pow_u_11  * pow_u_12 
        + p_p->c1113y * pow_u_11  * pow_u_13 
        + p_p->c1114y * pow_u_11  * pow_u_14 
        + p_p->c1115y * pow_u_11  * pow_u_15 
        + p_p->c1116y * pow_u_11  * pow_u_16 
        + p_p->c1117y * pow_u_11  * pow_u_17 
        + p_p->c1118y * pow_u_11  * pow_u_18 
        + p_p->c1119y * pow_u_11  * pow_u_19 
        + p_p->c1120y * pow_u_11  * pow_u_20 
        + p_p->c1121y * pow_u_11  * pow_u_21 ;
r_y=r_y + p_p->c1200y * pow_u_12  * pow_v_0  
        + p_p->c1201y * pow_u_12  * pow_v_1  
        + p_p->c1202y * pow_u_12  * pow_v_2  
        + p_p->c1203y * pow_u_12  * pow_v_3  
        + p_p->c1204y * pow_u_12  * pow_v_4  
        + p_p->c1205y * pow_u_12  * pow_v_5  
        + p_p->c1206y * pow_u_12  * pow_v_6  
        + p_p->c1207y * pow_u_12  * pow_v_7  
        + p_p->c1208y * pow_u_12  * pow_v_8  
        + p_p->c1209y * pow_u_12  * pow_v_9  
        + p_p->c1210y * pow_u_12  * pow_u_10 
        + p_p->c1211y * pow_u_12  * pow_u_11 
        + p_p->c1212y * pow_u_12  * pow_u_12 
        + p_p->c1213y * pow_u_12  * pow_u_13 
        + p_p->c1214y * pow_u_12  * pow_u_14 
        + p_p->c1215y * pow_u_12  * pow_u_15 
        + p_p->c1216y * pow_u_12  * pow_u_16 
        + p_p->c1217y * pow_u_12  * pow_u_17 
        + p_p->c1218y * pow_u_12  * pow_u_18 
        + p_p->c1219y * pow_u_12  * pow_u_19 
        + p_p->c1220y * pow_u_12  * pow_u_20 
        + p_p->c1221y * pow_u_12  * pow_u_21 ;
r_y=r_y + p_p->c1300y * pow_u_13  * pow_v_0  
        + p_p->c1301y * pow_u_13  * pow_v_1  
        + p_p->c1302y * pow_u_13  * pow_v_2  
        + p_p->c1303y * pow_u_13  * pow_v_3  
        + p_p->c1304y * pow_u_13  * pow_v_4  
        + p_p->c1305y * pow_u_13  * pow_v_5  
        + p_p->c1306y * pow_u_13  * pow_v_6  
        + p_p->c1307y * pow_u_13  * pow_v_7  
        + p_p->c1308y * pow_u_13  * pow_v_8  
        + p_p->c1309y * pow_u_13  * pow_v_9  
        + p_p->c1310y * pow_u_13  * pow_u_10 
        + p_p->c1311y * pow_u_13  * pow_u_11 
        + p_p->c1312y * pow_u_13  * pow_u_12 
        + p_p->c1313y * pow_u_13  * pow_u_13 
        + p_p->c1314y * pow_u_13  * pow_u_14 
        + p_p->c1315y * pow_u_13  * pow_u_15 
        + p_p->c1316y * pow_u_13  * pow_u_16 
        + p_p->c1317y * pow_u_13  * pow_u_17 
        + p_p->c1318y * pow_u_13  * pow_u_18 
        + p_p->c1319y * pow_u_13  * pow_u_19 
        + p_p->c1320y * pow_u_13  * pow_u_20 
        + p_p->c1321y * pow_u_13  * pow_u_21 ;
r_y=r_y + p_p->c1400y * pow_u_14  * pow_v_0  
        + p_p->c1401y * pow_u_14  * pow_v_1  
        + p_p->c1402y * pow_u_14  * pow_v_2  
        + p_p->c1403y * pow_u_14  * pow_v_3  
        + p_p->c1404y * pow_u_14  * pow_v_4  
        + p_p->c1405y * pow_u_14  * pow_v_5  
        + p_p->c1406y * pow_u_14  * pow_v_6  
        + p_p->c1407y * pow_u_14  * pow_v_7  
        + p_p->c1408y * pow_u_14  * pow_v_8  
        + p_p->c1409y * pow_u_14  * pow_v_9  
        + p_p->c1410y * pow_u_14  * pow_u_10 
        + p_p->c1411y * pow_u_14  * pow_u_11 
        + p_p->c1412y * pow_u_14  * pow_u_12 
        + p_p->c1413y * pow_u_14  * pow_u_13 
        + p_p->c1414y * pow_u_14  * pow_u_14 
        + p_p->c1415y * pow_u_14  * pow_u_15 
        + p_p->c1416y * pow_u_14  * pow_u_16 
        + p_p->c1417y * pow_u_14  * pow_u_17 
        + p_p->c1418y * pow_u_14  * pow_u_18 
        + p_p->c1419y * pow_u_14  * pow_u_19 
        + p_p->c1420y * pow_u_14  * pow_u_20 
        + p_p->c1421y * pow_u_14  * pow_u_21 ;
r_y=r_y + p_p->c1500y * pow_u_15  * pow_v_0  
        + p_p->c1501y * pow_u_15  * pow_v_1  
        + p_p->c1502y * pow_u_15  * pow_v_2  
        + p_p->c1503y * pow_u_15  * pow_v_3  
        + p_p->c1504y * pow_u_15  * pow_v_4  
        + p_p->c1505y * pow_u_15  * pow_v_5  
        + p_p->c1506y * pow_u_15  * pow_v_6  
        + p_p->c1507y * pow_u_15  * pow_v_7  
        + p_p->c1508y * pow_u_15  * pow_v_8  
        + p_p->c1509y * pow_u_15  * pow_v_9  
        + p_p->c1510y * pow_u_15  * pow_u_10 
        + p_p->c1511y * pow_u_15  * pow_u_11 
        + p_p->c1512y * pow_u_15  * pow_u_12 
        + p_p->c1513y * pow_u_15  * pow_u_13 
        + p_p->c1514y * pow_u_15  * pow_u_14 
        + p_p->c1515y * pow_u_15  * pow_u_15 
        + p_p->c1516y * pow_u_15  * pow_u_16 
        + p_p->c1517y * pow_u_15  * pow_u_17 
        + p_p->c1518y * pow_u_15  * pow_u_18 
        + p_p->c1519y * pow_u_15  * pow_u_19 
        + p_p->c1520y * pow_u_15  * pow_u_20 
        + p_p->c1521y * pow_u_15  * pow_u_21 ;
r_y=r_y + p_p->c1600y * pow_u_16  * pow_v_0  
        + p_p->c1601y * pow_u_16  * pow_v_1  
        + p_p->c1602y * pow_u_16  * pow_v_2  
        + p_p->c1603y * pow_u_16  * pow_v_3  
        + p_p->c1604y * pow_u_16  * pow_v_4  
        + p_p->c1605y * pow_u_16  * pow_v_5  
        + p_p->c1606y * pow_u_16  * pow_v_6  
        + p_p->c1607y * pow_u_16  * pow_v_7  
        + p_p->c1608y * pow_u_16  * pow_v_8  
        + p_p->c1609y * pow_u_16  * pow_v_9  
        + p_p->c1610y * pow_u_16  * pow_u_10 
        + p_p->c1611y * pow_u_16  * pow_u_11 
        + p_p->c1612y * pow_u_16  * pow_u_12 
        + p_p->c1613y * pow_u_16  * pow_u_13 
        + p_p->c1614y * pow_u_16  * pow_u_14 
        + p_p->c1615y * pow_u_16  * pow_u_15 
        + p_p->c1616y * pow_u_16  * pow_u_16 
        + p_p->c1617y * pow_u_16  * pow_u_17 
        + p_p->c1618y * pow_u_16  * pow_u_18 
        + p_p->c1619y * pow_u_16  * pow_u_19 
        + p_p->c1620y * pow_u_16  * pow_u_20 
        + p_p->c1621y * pow_u_16  * pow_u_21 ;
r_y=r_y + p_p->c1700y * pow_u_17  * pow_v_0  
        + p_p->c1701y * pow_u_17  * pow_v_1  
        + p_p->c1702y * pow_u_17  * pow_v_2  
        + p_p->c1703y * pow_u_17  * pow_v_3  
        + p_p->c1704y * pow_u_17  * pow_v_4  
        + p_p->c1705y * pow_u_17  * pow_v_5  
        + p_p->c1706y * pow_u_17  * pow_v_6  
        + p_p->c1707y * pow_u_17  * pow_v_7  
        + p_p->c1708y * pow_u_17  * pow_v_8  
        + p_p->c1709y * pow_u_17  * pow_v_9  
        + p_p->c1710y * pow_u_17  * pow_u_10 
        + p_p->c1711y * pow_u_17  * pow_u_11 
        + p_p->c1712y * pow_u_17  * pow_u_12 
        + p_p->c1713y * pow_u_17  * pow_u_13 
        + p_p->c1714y * pow_u_17  * pow_u_14 
        + p_p->c1715y * pow_u_17  * pow_u_15 
        + p_p->c1716y * pow_u_17  * pow_u_16 
        + p_p->c1717y * pow_u_17  * pow_u_17 
        + p_p->c1718y * pow_u_17  * pow_u_18 
        + p_p->c1719y * pow_u_17  * pow_u_19 
        + p_p->c1720y * pow_u_17  * pow_u_20 
        + p_p->c1721y * pow_u_17  * pow_u_21 ;
r_y=r_y + p_p->c1800y * pow_u_18  * pow_v_0  
        + p_p->c1801y * pow_u_18  * pow_v_1  
        + p_p->c1802y * pow_u_18  * pow_v_2  
        + p_p->c1803y * pow_u_18  * pow_v_3  
        + p_p->c1804y * pow_u_18  * pow_v_4  
        + p_p->c1805y * pow_u_18  * pow_v_5  
        + p_p->c1806y * pow_u_18  * pow_v_6  
        + p_p->c1807y * pow_u_18  * pow_v_7  
        + p_p->c1808y * pow_u_18  * pow_v_8  
        + p_p->c1809y * pow_u_18  * pow_v_9  
        + p_p->c1810y * pow_u_18  * pow_u_10 
        + p_p->c1811y * pow_u_18  * pow_u_11 
        + p_p->c1812y * pow_u_18  * pow_u_12 
        + p_p->c1813y * pow_u_18  * pow_u_13 
        + p_p->c1814y * pow_u_18  * pow_u_14 
        + p_p->c1815y * pow_u_18  * pow_u_15 
        + p_p->c1816y * pow_u_18  * pow_u_16 
        + p_p->c1817y * pow_u_18  * pow_u_17 
        + p_p->c1818y * pow_u_18  * pow_u_18 
        + p_p->c1819y * pow_u_18  * pow_u_19 
        + p_p->c1820y * pow_u_18  * pow_u_20 
        + p_p->c1821y * pow_u_18  * pow_u_21 ;
r_y=r_y + p_p->c1900y * pow_u_19  * pow_v_0  
        + p_p->c1901y * pow_u_19  * pow_v_1  
        + p_p->c1902y * pow_u_19  * pow_v_2  
        + p_p->c1903y * pow_u_19  * pow_v_3  
        + p_p->c1904y * pow_u_19  * pow_v_4  
        + p_p->c1905y * pow_u_19  * pow_v_5  
        + p_p->c1906y * pow_u_19  * pow_v_6  
        + p_p->c1907y * pow_u_19  * pow_v_7  
        + p_p->c1908y * pow_u_19  * pow_v_8  
        + p_p->c1909y * pow_u_19  * pow_v_9  
        + p_p->c1910y * pow_u_19  * pow_u_10 
        + p_p->c1911y * pow_u_19  * pow_u_11 
        + p_p->c1912y * pow_u_19  * pow_u_12 
        + p_p->c1913y * pow_u_19  * pow_u_13 
        + p_p->c1914y * pow_u_19  * pow_u_14 
        + p_p->c1915y * pow_u_19  * pow_u_15 
        + p_p->c1916y * pow_u_19  * pow_u_16 
        + p_p->c1917y * pow_u_19  * pow_u_17 
        + p_p->c1918y * pow_u_19  * pow_u_18 
        + p_p->c1919y * pow_u_19  * pow_u_19 
        + p_p->c1920y * pow_u_19  * pow_u_20 
        + p_p->c1921y * pow_u_19  * pow_u_21 ;
r_y=r_y + p_p->c2000y * pow_u_20  * pow_v_0  
        + p_p->c2001y * pow_u_20  * pow_v_1  
        + p_p->c2002y * pow_u_20  * pow_v_2  
        + p_p->c2003y * pow_u_20  * pow_v_3  
        + p_p->c2004y * pow_u_20  * pow_v_4  
        + p_p->c2005y * pow_u_20  * pow_v_5  
        + p_p->c2006y * pow_u_20  * pow_v_6  
        + p_p->c2007y * pow_u_20  * pow_v_7  
        + p_p->c2008y * pow_u_20  * pow_v_8  
        + p_p->c2009y * pow_u_20  * pow_v_9  
        + p_p->c2010y * pow_u_20  * pow_u_10 
        + p_p->c2011y * pow_u_20  * pow_u_11 
        + p_p->c2012y * pow_u_20  * pow_u_12 
        + p_p->c2013y * pow_u_20  * pow_u_13 
        + p_p->c2014y * pow_u_20  * pow_u_14 
        + p_p->c2015y * pow_u_20  * pow_u_15 
        + p_p->c2016y * pow_u_20  * pow_u_16 
        + p_p->c2017y * pow_u_20  * pow_u_17 
        + p_p->c2018y * pow_u_20  * pow_u_18 
        + p_p->c2019y * pow_u_20  * pow_u_19 
        + p_p->c2020y * pow_u_20  * pow_u_20 
        + p_p->c2021y * pow_u_20  * pow_u_21 ;
r_y=r_y + p_p->c2100y * pow_u_21  * pow_v_0  
        + p_p->c2101y * pow_u_21  * pow_v_1  
        + p_p->c2102y * pow_u_21  * pow_v_2  
        + p_p->c2103y * pow_u_21  * pow_v_3  
        + p_p->c2104y * pow_u_21  * pow_v_4  
        + p_p->c2105y * pow_u_21  * pow_v_5  
        + p_p->c2106y * pow_u_21  * pow_v_6  
        + p_p->c2107y * pow_u_21  * pow_v_7  
        + p_p->c2108y * pow_u_21  * pow_v_8  
        + p_p->c2109y * pow_u_21  * pow_v_9  
        + p_p->c2110y * pow_u_21  * pow_u_10 
        + p_p->c2111y * pow_u_21  * pow_u_11 
        + p_p->c2112y * pow_u_21  * pow_u_12 
        + p_p->c2113y * pow_u_21  * pow_u_13 
        + p_p->c2114y * pow_u_21  * pow_u_14 
        + p_p->c2115y * pow_u_21  * pow_u_15 
        + p_p->c2116y * pow_u_21  * pow_u_16 
        + p_p->c2117y * pow_u_21  * pow_u_17 
        + p_p->c2118y * pow_u_21  * pow_u_18 
        + p_p->c2119y * pow_u_21  * pow_u_19 
        + p_p->c2120y * pow_u_21  * pow_u_20 
        + p_p->c2121y * pow_u_21  * pow_u_21 ;

r_z     = p_p->c0000z * pow_u_0   * pow_v_0  
        + p_p->c0001z * pow_u_0   * pow_v_1  
        + p_p->c0002z * pow_u_0   * pow_v_2  
        + p_p->c0003z * pow_u_0   * pow_v_3  
        + p_p->c0004z * pow_u_0   * pow_v_4  
        + p_p->c0005z * pow_u_0   * pow_v_5  
        + p_p->c0006z * pow_u_0   * pow_v_6  
        + p_p->c0007z * pow_u_0   * pow_v_7  
        + p_p->c0008z * pow_u_0   * pow_v_8  
        + p_p->c0009z * pow_u_0   * pow_v_9  
        + p_p->c0010z * pow_u_0   * pow_u_10 
        + p_p->c0011z * pow_u_0   * pow_u_11 
        + p_p->c0012z * pow_u_0   * pow_u_12 
        + p_p->c0013z * pow_u_0   * pow_u_13 
        + p_p->c0014z * pow_u_0   * pow_u_14 
        + p_p->c0015z * pow_u_0   * pow_u_15 
        + p_p->c0016z * pow_u_0   * pow_u_16 
        + p_p->c0017z * pow_u_0   * pow_u_17 
        + p_p->c0018z * pow_u_0   * pow_u_18 
        + p_p->c0019z * pow_u_0   * pow_u_19 
        + p_p->c0020z * pow_u_0   * pow_u_20 
        + p_p->c0021z * pow_u_0   * pow_u_21 ;
r_z=r_z + p_p->c0100z * pow_u_1   * pow_v_0  
        + p_p->c0101z * pow_u_1   * pow_v_1  
        + p_p->c0102z * pow_u_1   * pow_v_2  
        + p_p->c0103z * pow_u_1   * pow_v_3  
        + p_p->c0104z * pow_u_1   * pow_v_4  
        + p_p->c0105z * pow_u_1   * pow_v_5  
        + p_p->c0106z * pow_u_1   * pow_v_6  
        + p_p->c0107z * pow_u_1   * pow_v_7  
        + p_p->c0108z * pow_u_1   * pow_v_8  
        + p_p->c0109z * pow_u_1   * pow_v_9  
        + p_p->c0110z * pow_u_1   * pow_u_10 
        + p_p->c0111z * pow_u_1   * pow_u_11 
        + p_p->c0112z * pow_u_1   * pow_u_12 
        + p_p->c0113z * pow_u_1   * pow_u_13 
        + p_p->c0114z * pow_u_1   * pow_u_14 
        + p_p->c0115z * pow_u_1   * pow_u_15 
        + p_p->c0116z * pow_u_1   * pow_u_16 
        + p_p->c0117z * pow_u_1   * pow_u_17 
        + p_p->c0118z * pow_u_1   * pow_u_18 
        + p_p->c0119z * pow_u_1   * pow_u_19 
        + p_p->c0120z * pow_u_1   * pow_u_20 
        + p_p->c0121z * pow_u_1   * pow_u_21 ;
r_z=r_z + p_p->c0200z * pow_u_2   * pow_v_0  
        + p_p->c0201z * pow_u_2   * pow_v_1  
        + p_p->c0202z * pow_u_2   * pow_v_2  
        + p_p->c0203z * pow_u_2   * pow_v_3  
        + p_p->c0204z * pow_u_2   * pow_v_4  
        + p_p->c0205z * pow_u_2   * pow_v_5  
        + p_p->c0206z * pow_u_2   * pow_v_6  
        + p_p->c0207z * pow_u_2   * pow_v_7  
        + p_p->c0208z * pow_u_2   * pow_v_8  
        + p_p->c0209z * pow_u_2   * pow_v_9  
        + p_p->c0210z * pow_u_2   * pow_u_10 
        + p_p->c0211z * pow_u_2   * pow_u_11 
        + p_p->c0212z * pow_u_2   * pow_u_12 
        + p_p->c0213z * pow_u_2   * pow_u_13 
        + p_p->c0214z * pow_u_2   * pow_u_14 
        + p_p->c0215z * pow_u_2   * pow_u_15 
        + p_p->c0216z * pow_u_2   * pow_u_16 
        + p_p->c0217z * pow_u_2   * pow_u_17 
        + p_p->c0218z * pow_u_2   * pow_u_18 
        + p_p->c0219z * pow_u_2   * pow_u_19 
        + p_p->c0220z * pow_u_2   * pow_u_20 
        + p_p->c0221z * pow_u_2   * pow_u_21 ;
r_z=r_z + p_p->c0300z * pow_u_3   * pow_v_0  
        + p_p->c0301z * pow_u_3   * pow_v_1  
        + p_p->c0302z * pow_u_3   * pow_v_2  
        + p_p->c0303z * pow_u_3   * pow_v_3  
        + p_p->c0304z * pow_u_3   * pow_v_4  
        + p_p->c0305z * pow_u_3   * pow_v_5  
        + p_p->c0306z * pow_u_3   * pow_v_6  
        + p_p->c0307z * pow_u_3   * pow_v_7  
        + p_p->c0308z * pow_u_3   * pow_v_8  
        + p_p->c0309z * pow_u_3   * pow_v_9  
        + p_p->c0310z * pow_u_3   * pow_u_10 
        + p_p->c0311z * pow_u_3   * pow_u_11 
        + p_p->c0312z * pow_u_3   * pow_u_12 
        + p_p->c0313z * pow_u_3   * pow_u_13 
        + p_p->c0314z * pow_u_3   * pow_u_14 
        + p_p->c0315z * pow_u_3   * pow_u_15 
        + p_p->c0316z * pow_u_3   * pow_u_16 
        + p_p->c0317z * pow_u_3   * pow_u_17 
        + p_p->c0318z * pow_u_3   * pow_u_18 
        + p_p->c0319z * pow_u_3   * pow_u_19 
        + p_p->c0320z * pow_u_3   * pow_u_20 
        + p_p->c0321z * pow_u_3   * pow_u_21 ;
r_z=r_z + p_p->c0400z * pow_u_4   * pow_v_0  
        + p_p->c0401z * pow_u_4   * pow_v_1  
        + p_p->c0402z * pow_u_4   * pow_v_2  
        + p_p->c0403z * pow_u_4   * pow_v_3  
        + p_p->c0404z * pow_u_4   * pow_v_4  
        + p_p->c0405z * pow_u_4   * pow_v_5  
        + p_p->c0406z * pow_u_4   * pow_v_6  
        + p_p->c0407z * pow_u_4   * pow_v_7  
        + p_p->c0408z * pow_u_4   * pow_v_8  
        + p_p->c0409z * pow_u_4   * pow_v_9  
        + p_p->c0410z * pow_u_4   * pow_u_10 
        + p_p->c0411z * pow_u_4   * pow_u_11 
        + p_p->c0412z * pow_u_4   * pow_u_12 
        + p_p->c0413z * pow_u_4   * pow_u_13 
        + p_p->c0414z * pow_u_4   * pow_u_14 
        + p_p->c0415z * pow_u_4   * pow_u_15 
        + p_p->c0416z * pow_u_4   * pow_u_16 
        + p_p->c0417z * pow_u_4   * pow_u_17 
        + p_p->c0418z * pow_u_4   * pow_u_18 
        + p_p->c0419z * pow_u_4   * pow_u_19 
        + p_p->c0420z * pow_u_4   * pow_u_20 
        + p_p->c0421z * pow_u_4   * pow_u_21 ;
r_z=r_z + p_p->c0500z * pow_u_5   * pow_v_0  
        + p_p->c0501z * pow_u_5   * pow_v_1  
        + p_p->c0502z * pow_u_5   * pow_v_2  
        + p_p->c0503z * pow_u_5   * pow_v_3  
        + p_p->c0504z * pow_u_5   * pow_v_4  
        + p_p->c0505z * pow_u_5   * pow_v_5  
        + p_p->c0506z * pow_u_5   * pow_v_6  
        + p_p->c0507z * pow_u_5   * pow_v_7  
        + p_p->c0508z * pow_u_5   * pow_v_8  
        + p_p->c0509z * pow_u_5   * pow_v_9  
        + p_p->c0510z * pow_u_5   * pow_u_10 
        + p_p->c0511z * pow_u_5   * pow_u_11 
        + p_p->c0512z * pow_u_5   * pow_u_12 
        + p_p->c0513z * pow_u_5   * pow_u_13 
        + p_p->c0514z * pow_u_5   * pow_u_14 
        + p_p->c0515z * pow_u_5   * pow_u_15 
        + p_p->c0516z * pow_u_5   * pow_u_16 
        + p_p->c0517z * pow_u_5   * pow_u_17 
        + p_p->c0518z * pow_u_5   * pow_u_18 
        + p_p->c0519z * pow_u_5   * pow_u_19 
        + p_p->c0520z * pow_u_5   * pow_u_20 
        + p_p->c0521z * pow_u_5   * pow_u_21 ;
r_z=r_z + p_p->c0600z * pow_u_6   * pow_v_0  
        + p_p->c0601z * pow_u_6   * pow_v_1  
        + p_p->c0602z * pow_u_6   * pow_v_2  
        + p_p->c0603z * pow_u_6   * pow_v_3  
        + p_p->c0604z * pow_u_6   * pow_v_4  
        + p_p->c0605z * pow_u_6   * pow_v_5  
        + p_p->c0606z * pow_u_6   * pow_v_6  
        + p_p->c0607z * pow_u_6   * pow_v_7  
        + p_p->c0608z * pow_u_6   * pow_v_8  
        + p_p->c0609z * pow_u_6   * pow_v_9  
        + p_p->c0610z * pow_u_6   * pow_u_10 
        + p_p->c0611z * pow_u_6   * pow_u_11 
        + p_p->c0612z * pow_u_6   * pow_u_12 
        + p_p->c0613z * pow_u_6   * pow_u_13 
        + p_p->c0614z * pow_u_6   * pow_u_14 
        + p_p->c0615z * pow_u_6   * pow_u_15 
        + p_p->c0616z * pow_u_6   * pow_u_16 
        + p_p->c0617z * pow_u_6   * pow_u_17 
        + p_p->c0618z * pow_u_6   * pow_u_18 
        + p_p->c0619z * pow_u_6   * pow_u_19 
        + p_p->c0620z * pow_u_6   * pow_u_20 
        + p_p->c0621z * pow_u_6   * pow_u_21 ;
r_z=r_z + p_p->c0700z * pow_u_7   * pow_v_0  
        + p_p->c0701z * pow_u_7   * pow_v_1  
        + p_p->c0702z * pow_u_7   * pow_v_2  
        + p_p->c0703z * pow_u_7   * pow_v_3  
        + p_p->c0704z * pow_u_7   * pow_v_4  
        + p_p->c0705z * pow_u_7   * pow_v_5  
        + p_p->c0706z * pow_u_7   * pow_v_6  
        + p_p->c0707z * pow_u_7   * pow_v_7  
        + p_p->c0708z * pow_u_7   * pow_v_8  
        + p_p->c0709z * pow_u_7   * pow_v_9  
        + p_p->c0710z * pow_u_7   * pow_u_10 
        + p_p->c0711z * pow_u_7   * pow_u_11 
        + p_p->c0712z * pow_u_7   * pow_u_12 
        + p_p->c0713z * pow_u_7   * pow_u_13 
        + p_p->c0714z * pow_u_7   * pow_u_14 
        + p_p->c0715z * pow_u_7   * pow_u_15 
        + p_p->c0716z * pow_u_7   * pow_u_16 
        + p_p->c0717z * pow_u_7   * pow_u_17 
        + p_p->c0718z * pow_u_7   * pow_u_18 
        + p_p->c0719z * pow_u_7   * pow_u_19 
        + p_p->c0720z * pow_u_7   * pow_u_20 
        + p_p->c0721z * pow_u_7   * pow_u_21 ;
r_z=r_z + p_p->c0800z * pow_u_8   * pow_v_0  
        + p_p->c0801z * pow_u_8   * pow_v_1  
        + p_p->c0802z * pow_u_8   * pow_v_2  
        + p_p->c0803z * pow_u_8   * pow_v_3  
        + p_p->c0804z * pow_u_8   * pow_v_4  
        + p_p->c0805z * pow_u_8   * pow_v_5  
        + p_p->c0806z * pow_u_8   * pow_v_6  
        + p_p->c0807z * pow_u_8   * pow_v_7  
        + p_p->c0808z * pow_u_8   * pow_v_8  
        + p_p->c0809z * pow_u_8   * pow_v_9  
        + p_p->c0810z * pow_u_8   * pow_u_10 
        + p_p->c0811z * pow_u_8   * pow_u_11 
        + p_p->c0812z * pow_u_8   * pow_u_12 
        + p_p->c0813z * pow_u_8   * pow_u_13 
        + p_p->c0814z * pow_u_8   * pow_u_14 
        + p_p->c0815z * pow_u_8   * pow_u_15 
        + p_p->c0816z * pow_u_8   * pow_u_16 
        + p_p->c0817z * pow_u_8   * pow_u_17 
        + p_p->c0818z * pow_u_8   * pow_u_18 
        + p_p->c0819z * pow_u_8   * pow_u_19 
        + p_p->c0820z * pow_u_8   * pow_u_20 
        + p_p->c0821z * pow_u_8   * pow_u_21 ;
r_z=r_z + p_p->c0900z * pow_u_9   * pow_v_0  
        + p_p->c0901z * pow_u_9   * pow_v_1  
        + p_p->c0902z * pow_u_9   * pow_v_2  
        + p_p->c0903z * pow_u_9   * pow_v_3  
        + p_p->c0904z * pow_u_9   * pow_v_4  
        + p_p->c0905z * pow_u_9   * pow_v_5  
        + p_p->c0906z * pow_u_9   * pow_v_6  
        + p_p->c0907z * pow_u_9   * pow_v_7  
        + p_p->c0908z * pow_u_9   * pow_v_8  
        + p_p->c0909z * pow_u_9   * pow_v_9  
        + p_p->c0910z * pow_u_9   * pow_u_10 
        + p_p->c0911z * pow_u_9   * pow_u_11 
        + p_p->c0912z * pow_u_9   * pow_u_12 
        + p_p->c0913z * pow_u_9   * pow_u_13 
        + p_p->c0914z * pow_u_9   * pow_u_14 
        + p_p->c0915z * pow_u_9   * pow_u_15 
        + p_p->c0916z * pow_u_9   * pow_u_16 
        + p_p->c0917z * pow_u_9   * pow_u_17 
        + p_p->c0918z * pow_u_9   * pow_u_18 
        + p_p->c0919z * pow_u_9   * pow_u_19 
        + p_p->c0920z * pow_u_9   * pow_u_20 
        + p_p->c0921z * pow_u_9   * pow_u_21 ;
r_z=r_z + p_p->c1000z * pow_u_10  * pow_v_0  
        + p_p->c1001z * pow_u_10  * pow_v_1  
        + p_p->c1002z * pow_u_10  * pow_v_2  
        + p_p->c1003z * pow_u_10  * pow_v_3  
        + p_p->c1004z * pow_u_10  * pow_v_4  
        + p_p->c1005z * pow_u_10  * pow_v_5  
        + p_p->c1006z * pow_u_10  * pow_v_6  
        + p_p->c1007z * pow_u_10  * pow_v_7  
        + p_p->c1008z * pow_u_10  * pow_v_8  
        + p_p->c1009z * pow_u_10  * pow_v_9  
        + p_p->c1010z * pow_u_10  * pow_u_10 
        + p_p->c1011z * pow_u_10  * pow_u_11 
        + p_p->c1012z * pow_u_10  * pow_u_12 
        + p_p->c1013z * pow_u_10  * pow_u_13 
        + p_p->c1014z * pow_u_10  * pow_u_14 
        + p_p->c1015z * pow_u_10  * pow_u_15 
        + p_p->c1016z * pow_u_10  * pow_u_16 
        + p_p->c1017z * pow_u_10  * pow_u_17 
        + p_p->c1018z * pow_u_10  * pow_u_18 
        + p_p->c1019z * pow_u_10  * pow_u_19 
        + p_p->c1020z * pow_u_10  * pow_u_20 
        + p_p->c1021z * pow_u_10  * pow_u_21 ;
r_z=r_z + p_p->c1100z * pow_u_11  * pow_v_0  
        + p_p->c1101z * pow_u_11  * pow_v_1  
        + p_p->c1102z * pow_u_11  * pow_v_2  
        + p_p->c1103z * pow_u_11  * pow_v_3  
        + p_p->c1104z * pow_u_11  * pow_v_4  
        + p_p->c1105z * pow_u_11  * pow_v_5  
        + p_p->c1106z * pow_u_11  * pow_v_6  
        + p_p->c1107z * pow_u_11  * pow_v_7  
        + p_p->c1108z * pow_u_11  * pow_v_8  
        + p_p->c1109z * pow_u_11  * pow_v_9  
        + p_p->c1110z * pow_u_11  * pow_u_10 
        + p_p->c1111z * pow_u_11  * pow_u_11 
        + p_p->c1112z * pow_u_11  * pow_u_12 
        + p_p->c1113z * pow_u_11  * pow_u_13 
        + p_p->c1114z * pow_u_11  * pow_u_14 
        + p_p->c1115z * pow_u_11  * pow_u_15 
        + p_p->c1116z * pow_u_11  * pow_u_16 
        + p_p->c1117z * pow_u_11  * pow_u_17 
        + p_p->c1118z * pow_u_11  * pow_u_18 
        + p_p->c1119z * pow_u_11  * pow_u_19 
        + p_p->c1120z * pow_u_11  * pow_u_20 
        + p_p->c1121z * pow_u_11  * pow_u_21 ;
r_z=r_z + p_p->c1200z * pow_u_12  * pow_v_0  
        + p_p->c1201z * pow_u_12  * pow_v_1  
        + p_p->c1202z * pow_u_12  * pow_v_2  
        + p_p->c1203z * pow_u_12  * pow_v_3  
        + p_p->c1204z * pow_u_12  * pow_v_4  
        + p_p->c1205z * pow_u_12  * pow_v_5  
        + p_p->c1206z * pow_u_12  * pow_v_6  
        + p_p->c1207z * pow_u_12  * pow_v_7  
        + p_p->c1208z * pow_u_12  * pow_v_8  
        + p_p->c1209z * pow_u_12  * pow_v_9  
        + p_p->c1210z * pow_u_12  * pow_u_10 
        + p_p->c1211z * pow_u_12  * pow_u_11 
        + p_p->c1212z * pow_u_12  * pow_u_12 
        + p_p->c1213z * pow_u_12  * pow_u_13 
        + p_p->c1214z * pow_u_12  * pow_u_14 
        + p_p->c1215z * pow_u_12  * pow_u_15 
        + p_p->c1216z * pow_u_12  * pow_u_16 
        + p_p->c1217z * pow_u_12  * pow_u_17 
        + p_p->c1218z * pow_u_12  * pow_u_18 
        + p_p->c1219z * pow_u_12  * pow_u_19 
        + p_p->c1220z * pow_u_12  * pow_u_20 
        + p_p->c1221z * pow_u_12  * pow_u_21 ;
r_z=r_z + p_p->c1300z * pow_u_13  * pow_v_0  
        + p_p->c1301z * pow_u_13  * pow_v_1  
        + p_p->c1302z * pow_u_13  * pow_v_2  
        + p_p->c1303z * pow_u_13  * pow_v_3  
        + p_p->c1304z * pow_u_13  * pow_v_4  
        + p_p->c1305z * pow_u_13  * pow_v_5  
        + p_p->c1306z * pow_u_13  * pow_v_6  
        + p_p->c1307z * pow_u_13  * pow_v_7  
        + p_p->c1308z * pow_u_13  * pow_v_8  
        + p_p->c1309z * pow_u_13  * pow_v_9  
        + p_p->c1310z * pow_u_13  * pow_u_10 
        + p_p->c1311z * pow_u_13  * pow_u_11 
        + p_p->c1312z * pow_u_13  * pow_u_12 
        + p_p->c1313z * pow_u_13  * pow_u_13 
        + p_p->c1314z * pow_u_13  * pow_u_14 
        + p_p->c1315z * pow_u_13  * pow_u_15 
        + p_p->c1316z * pow_u_13  * pow_u_16 
        + p_p->c1317z * pow_u_13  * pow_u_17 
        + p_p->c1318z * pow_u_13  * pow_u_18 
        + p_p->c1319z * pow_u_13  * pow_u_19 
        + p_p->c1320z * pow_u_13  * pow_u_20 
        + p_p->c1321z * pow_u_13  * pow_u_21 ;
r_z=r_z + p_p->c1400z * pow_u_14  * pow_v_0  
        + p_p->c1401z * pow_u_14  * pow_v_1  
        + p_p->c1402z * pow_u_14  * pow_v_2  
        + p_p->c1403z * pow_u_14  * pow_v_3  
        + p_p->c1404z * pow_u_14  * pow_v_4  
        + p_p->c1405z * pow_u_14  * pow_v_5  
        + p_p->c1406z * pow_u_14  * pow_v_6  
        + p_p->c1407z * pow_u_14  * pow_v_7  
        + p_p->c1408z * pow_u_14  * pow_v_8  
        + p_p->c1409z * pow_u_14  * pow_v_9  
        + p_p->c1410z * pow_u_14  * pow_u_10 
        + p_p->c1411z * pow_u_14  * pow_u_11 
        + p_p->c1412z * pow_u_14  * pow_u_12 
        + p_p->c1413z * pow_u_14  * pow_u_13 
        + p_p->c1414z * pow_u_14  * pow_u_14 
        + p_p->c1415z * pow_u_14  * pow_u_15 
        + p_p->c1416z * pow_u_14  * pow_u_16 
        + p_p->c1417z * pow_u_14  * pow_u_17 
        + p_p->c1418z * pow_u_14  * pow_u_18 
        + p_p->c1419z * pow_u_14  * pow_u_19 
        + p_p->c1420z * pow_u_14  * pow_u_20 
        + p_p->c1421z * pow_u_14  * pow_u_21 ;
r_z=r_z + p_p->c1500z * pow_u_15  * pow_v_0  
        + p_p->c1501z * pow_u_15  * pow_v_1  
        + p_p->c1502z * pow_u_15  * pow_v_2  
        + p_p->c1503z * pow_u_15  * pow_v_3  
        + p_p->c1504z * pow_u_15  * pow_v_4  
        + p_p->c1505z * pow_u_15  * pow_v_5  
        + p_p->c1506z * pow_u_15  * pow_v_6  
        + p_p->c1507z * pow_u_15  * pow_v_7  
        + p_p->c1508z * pow_u_15  * pow_v_8  
        + p_p->c1509z * pow_u_15  * pow_v_9  
        + p_p->c1510z * pow_u_15  * pow_u_10 
        + p_p->c1511z * pow_u_15  * pow_u_11 
        + p_p->c1512z * pow_u_15  * pow_u_12 
        + p_p->c1513z * pow_u_15  * pow_u_13 
        + p_p->c1514z * pow_u_15  * pow_u_14 
        + p_p->c1515z * pow_u_15  * pow_u_15 
        + p_p->c1516z * pow_u_15  * pow_u_16 
        + p_p->c1517z * pow_u_15  * pow_u_17 
        + p_p->c1518z * pow_u_15  * pow_u_18 
        + p_p->c1519z * pow_u_15  * pow_u_19 
        + p_p->c1520z * pow_u_15  * pow_u_20 
        + p_p->c1521z * pow_u_15  * pow_u_21 ;
r_z=r_z + p_p->c1600z * pow_u_16  * pow_v_0  
        + p_p->c1601z * pow_u_16  * pow_v_1  
        + p_p->c1602z * pow_u_16  * pow_v_2  
        + p_p->c1603z * pow_u_16  * pow_v_3  
        + p_p->c1604z * pow_u_16  * pow_v_4  
        + p_p->c1605z * pow_u_16  * pow_v_5  
        + p_p->c1606z * pow_u_16  * pow_v_6  
        + p_p->c1607z * pow_u_16  * pow_v_7  
        + p_p->c1608z * pow_u_16  * pow_v_8  
        + p_p->c1609z * pow_u_16  * pow_v_9  
        + p_p->c1610z * pow_u_16  * pow_u_10 
        + p_p->c1611z * pow_u_16  * pow_u_11 
        + p_p->c1612z * pow_u_16  * pow_u_12 
        + p_p->c1613z * pow_u_16  * pow_u_13 
        + p_p->c1614z * pow_u_16  * pow_u_14 
        + p_p->c1615z * pow_u_16  * pow_u_15 
        + p_p->c1616z * pow_u_16  * pow_u_16 
        + p_p->c1617z * pow_u_16  * pow_u_17 
        + p_p->c1618z * pow_u_16  * pow_u_18 
        + p_p->c1619z * pow_u_16  * pow_u_19 
        + p_p->c1620z * pow_u_16  * pow_u_20 
        + p_p->c1621z * pow_u_16  * pow_u_21 ;
r_z=r_z + p_p->c1700z * pow_u_17  * pow_v_0  
        + p_p->c1701z * pow_u_17  * pow_v_1  
        + p_p->c1702z * pow_u_17  * pow_v_2  
        + p_p->c1703z * pow_u_17  * pow_v_3  
        + p_p->c1704z * pow_u_17  * pow_v_4  
        + p_p->c1705z * pow_u_17  * pow_v_5  
        + p_p->c1706z * pow_u_17  * pow_v_6  
        + p_p->c1707z * pow_u_17  * pow_v_7  
        + p_p->c1708z * pow_u_17  * pow_v_8  
        + p_p->c1709z * pow_u_17  * pow_v_9  
        + p_p->c1710z * pow_u_17  * pow_u_10 
        + p_p->c1711z * pow_u_17  * pow_u_11 
        + p_p->c1712z * pow_u_17  * pow_u_12 
        + p_p->c1713z * pow_u_17  * pow_u_13 
        + p_p->c1714z * pow_u_17  * pow_u_14 
        + p_p->c1715z * pow_u_17  * pow_u_15 
        + p_p->c1716z * pow_u_17  * pow_u_16 
        + p_p->c1717z * pow_u_17  * pow_u_17 
        + p_p->c1718z * pow_u_17  * pow_u_18 
        + p_p->c1719z * pow_u_17  * pow_u_19 
        + p_p->c1720z * pow_u_17  * pow_u_20 
        + p_p->c1721z * pow_u_17  * pow_u_21 ;
r_z=r_z + p_p->c1800z * pow_u_18  * pow_v_0  
        + p_p->c1801z * pow_u_18  * pow_v_1  
        + p_p->c1802z * pow_u_18  * pow_v_2  
        + p_p->c1803z * pow_u_18  * pow_v_3  
        + p_p->c1804z * pow_u_18  * pow_v_4  
        + p_p->c1805z * pow_u_18  * pow_v_5  
        + p_p->c1806z * pow_u_18  * pow_v_6  
        + p_p->c1807z * pow_u_18  * pow_v_7  
        + p_p->c1808z * pow_u_18  * pow_v_8  
        + p_p->c1809z * pow_u_18  * pow_v_9  
        + p_p->c1810z * pow_u_18  * pow_u_10 
        + p_p->c1811z * pow_u_18  * pow_u_11 
        + p_p->c1812z * pow_u_18  * pow_u_12 
        + p_p->c1813z * pow_u_18  * pow_u_13 
        + p_p->c1814z * pow_u_18  * pow_u_14 
        + p_p->c1815z * pow_u_18  * pow_u_15 
        + p_p->c1816z * pow_u_18  * pow_u_16 
        + p_p->c1817z * pow_u_18  * pow_u_17 
        + p_p->c1818z * pow_u_18  * pow_u_18 
        + p_p->c1819z * pow_u_18  * pow_u_19 
        + p_p->c1820z * pow_u_18  * pow_u_20 
        + p_p->c1821z * pow_u_18  * pow_u_21 ;
r_z=r_z + p_p->c1900z * pow_u_19  * pow_v_0  
        + p_p->c1901z * pow_u_19  * pow_v_1  
        + p_p->c1902z * pow_u_19  * pow_v_2  
        + p_p->c1903z * pow_u_19  * pow_v_3  
        + p_p->c1904z * pow_u_19  * pow_v_4  
        + p_p->c1905z * pow_u_19  * pow_v_5  
        + p_p->c1906z * pow_u_19  * pow_v_6  
        + p_p->c1907z * pow_u_19  * pow_v_7  
        + p_p->c1908z * pow_u_19  * pow_v_8  
        + p_p->c1909z * pow_u_19  * pow_v_9  
        + p_p->c1910z * pow_u_19  * pow_u_10 
        + p_p->c1911z * pow_u_19  * pow_u_11 
        + p_p->c1912z * pow_u_19  * pow_u_12 
        + p_p->c1913z * pow_u_19  * pow_u_13 
        + p_p->c1914z * pow_u_19  * pow_u_14 
        + p_p->c1915z * pow_u_19  * pow_u_15 
        + p_p->c1916z * pow_u_19  * pow_u_16 
        + p_p->c1917z * pow_u_19  * pow_u_17 
        + p_p->c1918z * pow_u_19  * pow_u_18 
        + p_p->c1919z * pow_u_19  * pow_u_19 
        + p_p->c1920z * pow_u_19  * pow_u_20 
        + p_p->c1921z * pow_u_19  * pow_u_21 ;
r_z=r_z + p_p->c2000z * pow_u_20  * pow_v_0  
        + p_p->c2001z * pow_u_20  * pow_v_1  
        + p_p->c2002z * pow_u_20  * pow_v_2  
        + p_p->c2003z * pow_u_20  * pow_v_3  
        + p_p->c2004z * pow_u_20  * pow_v_4  
        + p_p->c2005z * pow_u_20  * pow_v_5  
        + p_p->c2006z * pow_u_20  * pow_v_6  
        + p_p->c2007z * pow_u_20  * pow_v_7  
        + p_p->c2008z * pow_u_20  * pow_v_8  
        + p_p->c2009z * pow_u_20  * pow_v_9  
        + p_p->c2010z * pow_u_20  * pow_u_10 
        + p_p->c2011z * pow_u_20  * pow_u_11 
        + p_p->c2012z * pow_u_20  * pow_u_12 
        + p_p->c2013z * pow_u_20  * pow_u_13 
        + p_p->c2014z * pow_u_20  * pow_u_14 
        + p_p->c2015z * pow_u_20  * pow_u_15 
        + p_p->c2016z * pow_u_20  * pow_u_16 
        + p_p->c2017z * pow_u_20  * pow_u_17 
        + p_p->c2018z * pow_u_20  * pow_u_18 
        + p_p->c2019z * pow_u_20  * pow_u_19 
        + p_p->c2020z * pow_u_20  * pow_u_20 
        + p_p->c2021z * pow_u_20  * pow_u_21 ;
r_z=r_z + p_p->c2100z * pow_u_21  * pow_v_0  
        + p_p->c2101z * pow_u_21  * pow_v_1  
        + p_p->c2102z * pow_u_21  * pow_v_2  
        + p_p->c2103z * pow_u_21  * pow_v_3  
        + p_p->c2104z * pow_u_21  * pow_v_4  
        + p_p->c2105z * pow_u_21  * pow_v_5  
        + p_p->c2106z * pow_u_21  * pow_v_6  
        + p_p->c2107z * pow_u_21  * pow_v_7  
        + p_p->c2108z * pow_u_21  * pow_v_8  
        + p_p->c2109z * pow_u_21  * pow_v_9  
        + p_p->c2110z * pow_u_21  * pow_u_10 
        + p_p->c2111z * pow_u_21  * pow_u_11 
        + p_p->c2112z * pow_u_21  * pow_u_12 
        + p_p->c2113z * pow_u_21  * pow_u_13 
        + p_p->c2114z * pow_u_21  * pow_u_14 
        + p_p->c2115z * pow_u_21  * pow_u_15 
        + p_p->c2116z * pow_u_21  * pow_u_16 
        + p_p->c2117z * pow_u_21  * pow_u_17 
        + p_p->c2118z * pow_u_21  * pow_u_18 
        + p_p->c2119z * pow_u_21  * pow_u_19 
        + p_p->c2120z * pow_u_21  * pow_u_20 
        + p_p->c2121z * pow_u_21  * pow_u_21 ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur504*c_coord Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ** c_drdu *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates derivatives dr/du for GMPATP21           */

   static short c_drdu  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP21  *p_p;         /* Patch GMPATP21                    (ptr) */
   DBfloat    u_l;         /* Patch (local) U parameter value         */
   DBfloat    v_l;         /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Derivatives u_x, u_y, u_z                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_drdu  p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l);
}
#endif


u_x     = p_p->c0000x*         0.0         * pow_v_0  
        + p_p->c0001x*         0.0         * pow_v_1  
        + p_p->c0002x*         0.0         * pow_v_2  
        + p_p->c0003x*         0.0         * pow_v_3  
        + p_p->c0004x*         0.0         * pow_v_4  
        + p_p->c0005x*         0.0         * pow_v_5  
        + p_p->c0006x*         0.0         * pow_v_6  
        + p_p->c0007x*         0.0         * pow_v_7  
        + p_p->c0008x*         0.0         * pow_v_8  
        + p_p->c0009x*         0.0         * pow_v_9  
        + p_p->c0010x*         0.0         * pow_v_10  
        + p_p->c0011x*         0.0         * pow_v_11  
        + p_p->c0012x*         0.0         * pow_v_12  
        + p_p->c0013x*         0.0         * pow_v_13  
        + p_p->c0014x*         0.0         * pow_v_14  
        + p_p->c0015x*         0.0         * pow_v_15  
        + p_p->c0016x*         0.0         * pow_v_16  
        + p_p->c0017x*         0.0         * pow_v_17  
        + p_p->c0018x*         0.0         * pow_v_18  
        + p_p->c0019x*         0.0         * pow_v_19  
        + p_p->c0020x*         0.0         * pow_v_20  
        + p_p->c0021x*         0.0         * pow_v_21  ;
u_x=u_x + p_p->c0100x* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101x* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102x* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103x* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104x* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105x* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106x* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107x* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0108x* 1.0*pow_u_0 * pow_v_8  
        + p_p->c0109x* 1.0*pow_u_0 * pow_v_9  
        + p_p->c0110x* 1.0*pow_u_0 * pow_v_10  
        + p_p->c0111x* 1.0*pow_u_0 * pow_v_11  
        + p_p->c0112x* 1.0*pow_u_0 * pow_v_12  
        + p_p->c0113x* 1.0*pow_u_0 * pow_v_13  
        + p_p->c0114x* 1.0*pow_u_0 * pow_v_14  
        + p_p->c0115x* 1.0*pow_u_0 * pow_v_15  
        + p_p->c0116x* 1.0*pow_u_0 * pow_v_16  
        + p_p->c0117x* 1.0*pow_u_0 * pow_v_17  
        + p_p->c0118x* 1.0*pow_u_0 * pow_v_18  
        + p_p->c0119x* 1.0*pow_u_0 * pow_v_19  
        + p_p->c0120x* 1.0*pow_u_0 * pow_v_20  
        + p_p->c0121x* 1.0*pow_u_0 * pow_v_21  ;
u_x=u_x + p_p->c0200x* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201x* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202x* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203x* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204x* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205x* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206x* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207x* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0208x* 2.0*pow_u_1 * pow_v_8  
        + p_p->c0209x* 2.0*pow_u_1 * pow_v_9  
        + p_p->c0210x* 2.0*pow_u_1 * pow_v_10  
        + p_p->c0211x* 2.0*pow_u_1 * pow_v_11  
        + p_p->c0212x* 2.0*pow_u_1 * pow_v_12  
        + p_p->c0213x* 2.0*pow_u_1 * pow_v_13  
        + p_p->c0214x* 2.0*pow_u_1 * pow_v_14  
        + p_p->c0215x* 2.0*pow_u_1 * pow_v_15  
        + p_p->c0216x* 2.0*pow_u_1 * pow_v_16  
        + p_p->c0217x* 2.0*pow_u_1 * pow_v_17  
        + p_p->c0218x* 2.0*pow_u_1 * pow_v_18  
        + p_p->c0219x* 2.0*pow_u_1 * pow_v_19  
        + p_p->c0220x* 2.0*pow_u_1 * pow_v_20  
        + p_p->c0221x* 2.0*pow_u_1 * pow_v_21  ;
u_x=u_x + p_p->c0300x* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301x* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302x* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303x* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304x* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305x* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306x* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307x* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0308x* 3.0*pow_u_2 * pow_v_8  
        + p_p->c0309x* 3.0*pow_u_2 * pow_v_9  
        + p_p->c0310x* 3.0*pow_u_2 * pow_v_10  
        + p_p->c0311x* 3.0*pow_u_2 * pow_v_11  
        + p_p->c0312x* 3.0*pow_u_2 * pow_v_12  
        + p_p->c0313x* 3.0*pow_u_2 * pow_v_13  
        + p_p->c0314x* 3.0*pow_u_2 * pow_v_14  
        + p_p->c0315x* 3.0*pow_u_2 * pow_v_15  
        + p_p->c0316x* 3.0*pow_u_2 * pow_v_16  
        + p_p->c0317x* 3.0*pow_u_2 * pow_v_17  
        + p_p->c0318x* 3.0*pow_u_2 * pow_v_18  
        + p_p->c0319x* 3.0*pow_u_2 * pow_v_19  
        + p_p->c0320x* 3.0*pow_u_2 * pow_v_20  
        + p_p->c0321x* 3.0*pow_u_2 * pow_v_21  ;
u_x=u_x + p_p->c0400x* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401x* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402x* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403x* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404x* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405x* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406x* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407x* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0408x* 4.0*pow_u_3 * pow_v_8  
        + p_p->c0409x* 4.0*pow_u_3 * pow_v_9  
        + p_p->c0410x* 4.0*pow_u_3 * pow_v_10  
        + p_p->c0411x* 4.0*pow_u_3 * pow_v_11  
        + p_p->c0412x* 4.0*pow_u_3 * pow_v_12  
        + p_p->c0413x* 4.0*pow_u_3 * pow_v_13  
        + p_p->c0414x* 4.0*pow_u_3 * pow_v_14  
        + p_p->c0415x* 4.0*pow_u_3 * pow_v_15  
        + p_p->c0416x* 4.0*pow_u_3 * pow_v_16  
        + p_p->c0417x* 4.0*pow_u_3 * pow_v_17  
        + p_p->c0418x* 4.0*pow_u_3 * pow_v_18  
        + p_p->c0419x* 4.0*pow_u_3 * pow_v_19  
        + p_p->c0420x* 4.0*pow_u_3 * pow_v_20  
        + p_p->c0421x* 4.0*pow_u_3 * pow_v_21  ;
u_x=u_x + p_p->c0500x* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501x* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502x* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503x* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504x* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505x* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506x* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507x* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0508x* 5.0*pow_u_4 * pow_v_8  
        + p_p->c0509x* 5.0*pow_u_4 * pow_v_9  
        + p_p->c0510x* 5.0*pow_u_4 * pow_v_10  
        + p_p->c0511x* 5.0*pow_u_4 * pow_v_11  
        + p_p->c0512x* 5.0*pow_u_4 * pow_v_12  
        + p_p->c0513x* 5.0*pow_u_4 * pow_v_13  
        + p_p->c0514x* 5.0*pow_u_4 * pow_v_14  
        + p_p->c0515x* 5.0*pow_u_4 * pow_v_15  
        + p_p->c0516x* 5.0*pow_u_4 * pow_v_16  
        + p_p->c0517x* 5.0*pow_u_4 * pow_v_17  
        + p_p->c0518x* 5.0*pow_u_4 * pow_v_18  
        + p_p->c0519x* 5.0*pow_u_4 * pow_v_19  
        + p_p->c0520x* 5.0*pow_u_4 * pow_v_20  
        + p_p->c0521x* 5.0*pow_u_4 * pow_v_21  ;
u_x=u_x + p_p->c0600x* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601x* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602x* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603x* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604x* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605x* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606x* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607x* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0608x* 6.0*pow_u_5 * pow_v_8  
        + p_p->c0609x* 6.0*pow_u_5 * pow_v_9  
        + p_p->c0610x* 6.0*pow_u_5 * pow_v_10  
        + p_p->c0611x* 6.0*pow_u_5 * pow_v_11  
        + p_p->c0612x* 6.0*pow_u_5 * pow_v_12  
        + p_p->c0613x* 6.0*pow_u_5 * pow_v_13  
        + p_p->c0614x* 6.0*pow_u_5 * pow_v_14  
        + p_p->c0615x* 6.0*pow_u_5 * pow_v_15  
        + p_p->c0616x* 6.0*pow_u_5 * pow_v_16  
        + p_p->c0617x* 6.0*pow_u_5 * pow_v_17  
        + p_p->c0618x* 6.0*pow_u_5 * pow_v_18  
        + p_p->c0619x* 6.0*pow_u_5 * pow_v_19  
        + p_p->c0620x* 6.0*pow_u_5 * pow_v_20  
        + p_p->c0621x* 6.0*pow_u_5 * pow_v_21  ;
u_x=u_x + p_p->c0700x* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701x* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702x* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703x* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704x* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705x* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706x* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707x* 7.0*pow_u_6 * pow_v_7  
        + p_p->c0708x* 7.0*pow_u_6 * pow_v_8  
        + p_p->c0709x* 7.0*pow_u_6 * pow_v_9  
        + p_p->c0710x* 7.0*pow_u_6 * pow_v_10  
        + p_p->c0711x* 7.0*pow_u_6 * pow_v_11  
        + p_p->c0712x* 7.0*pow_u_6 * pow_v_12  
        + p_p->c0713x* 7.0*pow_u_6 * pow_v_13  
        + p_p->c0714x* 7.0*pow_u_6 * pow_v_14  
        + p_p->c0715x* 7.0*pow_u_6 * pow_v_15  
        + p_p->c0716x* 7.0*pow_u_6 * pow_v_16  
        + p_p->c0717x* 7.0*pow_u_6 * pow_v_17  
        + p_p->c0718x* 7.0*pow_u_6 * pow_v_18  
        + p_p->c0719x* 7.0*pow_u_6 * pow_v_19  
        + p_p->c0720x* 7.0*pow_u_6 * pow_v_20  
        + p_p->c0721x* 7.0*pow_u_6 * pow_v_21  ;
u_x=u_x + p_p->c0800x* 8.0*pow_u_7 * pow_v_0  
        + p_p->c0801x* 8.0*pow_u_7 * pow_v_1  
        + p_p->c0802x* 8.0*pow_u_7 * pow_v_2  
        + p_p->c0803x* 8.0*pow_u_7 * pow_v_3  
        + p_p->c0804x* 8.0*pow_u_7 * pow_v_4  
        + p_p->c0805x* 8.0*pow_u_7 * pow_v_5  
        + p_p->c0806x* 8.0*pow_u_7 * pow_v_6  
        + p_p->c0807x* 8.0*pow_u_7 * pow_v_7  
        + p_p->c0808x* 8.0*pow_u_7 * pow_v_8  
        + p_p->c0809x* 8.0*pow_u_7 * pow_v_9  
        + p_p->c0810x* 8.0*pow_u_7 * pow_v_10  
        + p_p->c0811x* 8.0*pow_u_7 * pow_v_11  
        + p_p->c0812x* 8.0*pow_u_7 * pow_v_12  
        + p_p->c0813x* 8.0*pow_u_7 * pow_v_13  
        + p_p->c0814x* 8.0*pow_u_7 * pow_v_14  
        + p_p->c0815x* 8.0*pow_u_7 * pow_v_15  
        + p_p->c0816x* 8.0*pow_u_7 * pow_v_16  
        + p_p->c0817x* 8.0*pow_u_7 * pow_v_17  
        + p_p->c0818x* 8.0*pow_u_7 * pow_v_18  
        + p_p->c0819x* 8.0*pow_u_7 * pow_v_19  
        + p_p->c0820x* 8.0*pow_u_7 * pow_v_20  
        + p_p->c0821x* 8.0*pow_u_7 * pow_v_21  ;
u_x=u_x + p_p->c0900x* 9.0*pow_u_8 * pow_v_0  
        + p_p->c0901x* 9.0*pow_u_8 * pow_v_1  
        + p_p->c0902x* 9.0*pow_u_8 * pow_v_2  
        + p_p->c0903x* 9.0*pow_u_8 * pow_v_3  
        + p_p->c0904x* 9.0*pow_u_8 * pow_v_4  
        + p_p->c0905x* 9.0*pow_u_8 * pow_v_5  
        + p_p->c0906x* 9.0*pow_u_8 * pow_v_6  
        + p_p->c0907x* 9.0*pow_u_8 * pow_v_7  
        + p_p->c0908x* 9.0*pow_u_8 * pow_v_8  
        + p_p->c0909x* 9.0*pow_u_8 * pow_v_9  
        + p_p->c0910x* 9.0*pow_u_8 * pow_v_10  
        + p_p->c0911x* 9.0*pow_u_8 * pow_v_11  
        + p_p->c0912x* 9.0*pow_u_8 * pow_v_12  
        + p_p->c0913x* 9.0*pow_u_8 * pow_v_13  
        + p_p->c0914x* 9.0*pow_u_8 * pow_v_14  
        + p_p->c0915x* 9.0*pow_u_8 * pow_v_15  
        + p_p->c0916x* 9.0*pow_u_8 * pow_v_16  
        + p_p->c0917x* 9.0*pow_u_8 * pow_v_17  
        + p_p->c0918x* 9.0*pow_u_8 * pow_v_18  
        + p_p->c0919x* 9.0*pow_u_8 * pow_v_19  
        + p_p->c0920x* 9.0*pow_u_8 * pow_v_20  
        + p_p->c0921x* 9.0*pow_u_8 * pow_v_21  ;
u_x=u_x + p_p->c1000x*10.0*pow_u_9 * pow_v_0  
        + p_p->c1001x*10.0*pow_u_9 * pow_v_1  
        + p_p->c1002x*10.0*pow_u_9 * pow_v_2  
        + p_p->c1003x*10.0*pow_u_9 * pow_v_3  
        + p_p->c1004x*10.0*pow_u_9 * pow_v_4  
        + p_p->c1005x*10.0*pow_u_9 * pow_v_5  
        + p_p->c1006x*10.0*pow_u_9 * pow_v_6  
        + p_p->c1007x*10.0*pow_u_9 * pow_v_7  
        + p_p->c1008x*10.0*pow_u_9 * pow_v_8  
        + p_p->c1009x*10.0*pow_u_9 * pow_v_9  
        + p_p->c1010x*10.0*pow_u_9 * pow_v_10  
        + p_p->c1011x*10.0*pow_u_9 * pow_v_11  
        + p_p->c1012x*10.0*pow_u_9 * pow_v_12  
        + p_p->c1013x*10.0*pow_u_9 * pow_v_13  
        + p_p->c1014x*10.0*pow_u_9 * pow_v_14  
        + p_p->c1015x*10.0*pow_u_9 * pow_v_15  
        + p_p->c1016x*10.0*pow_u_9 * pow_v_16  
        + p_p->c1017x*10.0*pow_u_9 * pow_v_17  
        + p_p->c1018x*10.0*pow_u_9 * pow_v_18  
        + p_p->c1019x*10.0*pow_u_9 * pow_v_19  
        + p_p->c1020x*10.0*pow_u_9 * pow_v_20  
        + p_p->c1021x*10.0*pow_u_9 * pow_v_21  ;
u_x=u_x + p_p->c1100x*11.0*pow_u_10 * pow_v_0  
        + p_p->c1101x*11.0*pow_u_10 * pow_v_1  
        + p_p->c1102x*11.0*pow_u_10 * pow_v_2  
        + p_p->c1103x*11.0*pow_u_10 * pow_v_3  
        + p_p->c1104x*11.0*pow_u_10 * pow_v_4  
        + p_p->c1105x*11.0*pow_u_10 * pow_v_5  
        + p_p->c1106x*11.0*pow_u_10 * pow_v_6  
        + p_p->c1107x*11.0*pow_u_10 * pow_v_7  
        + p_p->c1108x*11.0*pow_u_10 * pow_v_8  
        + p_p->c1109x*11.0*pow_u_10 * pow_v_9  
        + p_p->c1110x*11.0*pow_u_10 * pow_v_10  
        + p_p->c1111x*11.0*pow_u_10 * pow_v_11  
        + p_p->c1112x*11.0*pow_u_10 * pow_v_12  
        + p_p->c1113x*11.0*pow_u_10 * pow_v_13  
        + p_p->c1114x*11.0*pow_u_10 * pow_v_14  
        + p_p->c1115x*11.0*pow_u_10 * pow_v_15  
        + p_p->c1116x*11.0*pow_u_10 * pow_v_16  
        + p_p->c1117x*11.0*pow_u_10 * pow_v_17  
        + p_p->c1118x*11.0*pow_u_10 * pow_v_18  
        + p_p->c1119x*11.0*pow_u_10 * pow_v_19  
        + p_p->c1120x*11.0*pow_u_10 * pow_v_20  
        + p_p->c1121x*11.0*pow_u_10 * pow_v_21  ;
u_x=u_x + p_p->c1200x*12.0*pow_u_11 * pow_v_0  
        + p_p->c1201x*12.0*pow_u_11 * pow_v_1  
        + p_p->c1202x*12.0*pow_u_11 * pow_v_2  
        + p_p->c1203x*12.0*pow_u_11 * pow_v_3  
        + p_p->c1204x*12.0*pow_u_11 * pow_v_4  
        + p_p->c1205x*12.0*pow_u_11 * pow_v_5  
        + p_p->c1206x*12.0*pow_u_11 * pow_v_6  
        + p_p->c1207x*12.0*pow_u_11 * pow_v_7  
        + p_p->c1208x*12.0*pow_u_11 * pow_v_8  
        + p_p->c1209x*12.0*pow_u_11 * pow_v_9  
        + p_p->c1210x*12.0*pow_u_11 * pow_v_10  
        + p_p->c1211x*12.0*pow_u_11 * pow_v_11  
        + p_p->c1212x*12.0*pow_u_11 * pow_v_12  
        + p_p->c1213x*12.0*pow_u_11 * pow_v_13  
        + p_p->c1214x*12.0*pow_u_11 * pow_v_14  
        + p_p->c1215x*12.0*pow_u_11 * pow_v_15  
        + p_p->c1216x*12.0*pow_u_11 * pow_v_16  
        + p_p->c1217x*12.0*pow_u_11 * pow_v_17  
        + p_p->c1218x*12.0*pow_u_11 * pow_v_18  
        + p_p->c1219x*12.0*pow_u_11 * pow_v_19  
        + p_p->c1220x*12.0*pow_u_11 * pow_v_20  
        + p_p->c1221x*12.0*pow_u_11 * pow_v_21  ;
u_x=u_x + p_p->c1300x*13.0*pow_u_12 * pow_v_0  
        + p_p->c1301x*13.0*pow_u_12 * pow_v_1  
        + p_p->c1302x*13.0*pow_u_12 * pow_v_2  
        + p_p->c1303x*13.0*pow_u_12 * pow_v_3  
        + p_p->c1304x*13.0*pow_u_12 * pow_v_4  
        + p_p->c1305x*13.0*pow_u_12 * pow_v_5  
        + p_p->c1306x*13.0*pow_u_12 * pow_v_6  
        + p_p->c1307x*13.0*pow_u_12 * pow_v_7  
        + p_p->c1308x*13.0*pow_u_12 * pow_v_8  
        + p_p->c1309x*13.0*pow_u_12 * pow_v_9  
        + p_p->c1310x*13.0*pow_u_12 * pow_v_10  
        + p_p->c1311x*13.0*pow_u_12 * pow_v_11  
        + p_p->c1312x*13.0*pow_u_12 * pow_v_12  
        + p_p->c1313x*13.0*pow_u_12 * pow_v_13  
        + p_p->c1314x*13.0*pow_u_12 * pow_v_14  
        + p_p->c1315x*13.0*pow_u_12 * pow_v_15  
        + p_p->c1316x*13.0*pow_u_12 * pow_v_16  
        + p_p->c1317x*13.0*pow_u_12 * pow_v_17  
        + p_p->c1318x*13.0*pow_u_12 * pow_v_18  
        + p_p->c1319x*13.0*pow_u_12 * pow_v_19  
        + p_p->c1320x*13.0*pow_u_12 * pow_v_20  
        + p_p->c1321x*13.0*pow_u_12 * pow_v_21  ;
u_x=u_x + p_p->c1400x*14.0*pow_u_13 * pow_v_0  
        + p_p->c1401x*14.0*pow_u_13 * pow_v_1  
        + p_p->c1402x*14.0*pow_u_13 * pow_v_2  
        + p_p->c1403x*14.0*pow_u_13 * pow_v_3  
        + p_p->c1404x*14.0*pow_u_13 * pow_v_4  
        + p_p->c1405x*14.0*pow_u_13 * pow_v_5  
        + p_p->c1406x*14.0*pow_u_13 * pow_v_6  
        + p_p->c1407x*14.0*pow_u_13 * pow_v_7  
        + p_p->c1408x*14.0*pow_u_13 * pow_v_8  
        + p_p->c1409x*14.0*pow_u_13 * pow_v_9  
        + p_p->c1410x*14.0*pow_u_13 * pow_v_10  
        + p_p->c1411x*14.0*pow_u_13 * pow_v_11  
        + p_p->c1412x*14.0*pow_u_13 * pow_v_12  
        + p_p->c1413x*14.0*pow_u_13 * pow_v_13  
        + p_p->c1414x*14.0*pow_u_13 * pow_v_14  
        + p_p->c1415x*14.0*pow_u_13 * pow_v_15  
        + p_p->c1416x*14.0*pow_u_13 * pow_v_16  
        + p_p->c1417x*14.0*pow_u_13 * pow_v_17  
        + p_p->c1418x*14.0*pow_u_13 * pow_v_18  
        + p_p->c1419x*14.0*pow_u_13 * pow_v_19  
        + p_p->c1420x*14.0*pow_u_13 * pow_v_20  
        + p_p->c1421x*14.0*pow_u_13 * pow_v_21  ;
u_x=u_x + p_p->c1500x*15.0*pow_u_14 * pow_v_0  
        + p_p->c1501x*15.0*pow_u_14 * pow_v_1  
        + p_p->c1502x*15.0*pow_u_14 * pow_v_2  
        + p_p->c1503x*15.0*pow_u_14 * pow_v_3  
        + p_p->c1504x*15.0*pow_u_14 * pow_v_4  
        + p_p->c1505x*15.0*pow_u_14 * pow_v_5  
        + p_p->c1506x*15.0*pow_u_14 * pow_v_6  
        + p_p->c1507x*15.0*pow_u_14 * pow_v_7  
        + p_p->c1508x*15.0*pow_u_14 * pow_v_8  
        + p_p->c1509x*15.0*pow_u_14 * pow_v_9  
        + p_p->c1510x*15.0*pow_u_14 * pow_v_10  
        + p_p->c1511x*15.0*pow_u_14 * pow_v_11  
        + p_p->c1512x*15.0*pow_u_14 * pow_v_12  
        + p_p->c1513x*15.0*pow_u_14 * pow_v_13  
        + p_p->c1514x*15.0*pow_u_14 * pow_v_14  
        + p_p->c1515x*15.0*pow_u_14 * pow_v_15  
        + p_p->c1516x*15.0*pow_u_14 * pow_v_16  
        + p_p->c1517x*15.0*pow_u_14 * pow_v_17  
        + p_p->c1518x*15.0*pow_u_14 * pow_v_18  
        + p_p->c1519x*15.0*pow_u_14 * pow_v_19  
        + p_p->c1520x*15.0*pow_u_14 * pow_v_20  
        + p_p->c1521x*15.0*pow_u_14 * pow_v_21  ;
u_x=u_x + p_p->c1600x*16.0*pow_u_15 * pow_v_0  
        + p_p->c1601x*16.0*pow_u_15 * pow_v_1  
        + p_p->c1602x*16.0*pow_u_15 * pow_v_2  
        + p_p->c1603x*16.0*pow_u_15 * pow_v_3  
        + p_p->c1604x*16.0*pow_u_15 * pow_v_4  
        + p_p->c1605x*16.0*pow_u_15 * pow_v_5  
        + p_p->c1606x*16.0*pow_u_15 * pow_v_6  
        + p_p->c1607x*16.0*pow_u_15 * pow_v_7  
        + p_p->c1608x*16.0*pow_u_15 * pow_v_8  
        + p_p->c1609x*16.0*pow_u_15 * pow_v_9  
        + p_p->c1610x*16.0*pow_u_15 * pow_v_10  
        + p_p->c1611x*16.0*pow_u_15 * pow_v_11  
        + p_p->c1612x*16.0*pow_u_15 * pow_v_12  
        + p_p->c1613x*16.0*pow_u_15 * pow_v_13  
        + p_p->c1614x*16.0*pow_u_15 * pow_v_14  
        + p_p->c1615x*16.0*pow_u_15 * pow_v_15  
        + p_p->c1616x*16.0*pow_u_15 * pow_v_16  
        + p_p->c1617x*16.0*pow_u_15 * pow_v_17  
        + p_p->c1618x*16.0*pow_u_15 * pow_v_18  
        + p_p->c1619x*16.0*pow_u_15 * pow_v_19  
        + p_p->c1620x*16.0*pow_u_15 * pow_v_20  
        + p_p->c1621x*16.0*pow_u_15 * pow_v_21  ;
u_x=u_x + p_p->c1700x*17.0*pow_u_16 * pow_v_0  
        + p_p->c1701x*17.0*pow_u_16 * pow_v_1  
        + p_p->c1702x*17.0*pow_u_16 * pow_v_2  
        + p_p->c1703x*17.0*pow_u_16 * pow_v_3  
        + p_p->c1704x*17.0*pow_u_16 * pow_v_4  
        + p_p->c1705x*17.0*pow_u_16 * pow_v_5  
        + p_p->c1706x*17.0*pow_u_16 * pow_v_6  
        + p_p->c1707x*17.0*pow_u_16 * pow_v_7  
        + p_p->c1708x*17.0*pow_u_16 * pow_v_8  
        + p_p->c1709x*17.0*pow_u_16 * pow_v_9  
        + p_p->c1710x*17.0*pow_u_16 * pow_v_10  
        + p_p->c1711x*17.0*pow_u_16 * pow_v_11  
        + p_p->c1712x*17.0*pow_u_16 * pow_v_12  
        + p_p->c1713x*17.0*pow_u_16 * pow_v_13  
        + p_p->c1714x*17.0*pow_u_16 * pow_v_14  
        + p_p->c1715x*17.0*pow_u_16 * pow_v_15  
        + p_p->c1716x*17.0*pow_u_16 * pow_v_16  
        + p_p->c1717x*17.0*pow_u_16 * pow_v_17  
        + p_p->c1718x*17.0*pow_u_16 * pow_v_18  
        + p_p->c1719x*17.0*pow_u_16 * pow_v_19  
        + p_p->c1720x*17.0*pow_u_16 * pow_v_20  
        + p_p->c1721x*17.0*pow_u_16 * pow_v_21  ;
u_x=u_x + p_p->c1800x*18.0*pow_u_17 * pow_v_0  
        + p_p->c1801x*18.0*pow_u_17 * pow_v_1  
        + p_p->c1802x*18.0*pow_u_17 * pow_v_2  
        + p_p->c1803x*18.0*pow_u_17 * pow_v_3  
        + p_p->c1804x*18.0*pow_u_17 * pow_v_4  
        + p_p->c1805x*18.0*pow_u_17 * pow_v_5  
        + p_p->c1806x*18.0*pow_u_17 * pow_v_6  
        + p_p->c1807x*18.0*pow_u_17 * pow_v_7  
        + p_p->c1808x*18.0*pow_u_17 * pow_v_8  
        + p_p->c1809x*18.0*pow_u_17 * pow_v_9  
        + p_p->c1810x*18.0*pow_u_17 * pow_v_10  
        + p_p->c1811x*18.0*pow_u_17 * pow_v_11  
        + p_p->c1812x*18.0*pow_u_17 * pow_v_12  
        + p_p->c1813x*18.0*pow_u_17 * pow_v_13  
        + p_p->c1814x*18.0*pow_u_17 * pow_v_14  
        + p_p->c1815x*18.0*pow_u_17 * pow_v_15  
        + p_p->c1816x*18.0*pow_u_17 * pow_v_16  
        + p_p->c1817x*18.0*pow_u_17 * pow_v_17  
        + p_p->c1818x*18.0*pow_u_17 * pow_v_18  
        + p_p->c1819x*18.0*pow_u_17 * pow_v_19  
        + p_p->c1820x*18.0*pow_u_17 * pow_v_20  
        + p_p->c1821x*18.0*pow_u_17 * pow_v_21  ;
u_x=u_x + p_p->c1900x*19.0*pow_u_18 * pow_v_0  
        + p_p->c1901x*19.0*pow_u_18 * pow_v_1  
        + p_p->c1902x*19.0*pow_u_18 * pow_v_2  
        + p_p->c1903x*19.0*pow_u_18 * pow_v_3  
        + p_p->c1904x*19.0*pow_u_18 * pow_v_4  
        + p_p->c1905x*19.0*pow_u_18 * pow_v_5  
        + p_p->c1906x*19.0*pow_u_18 * pow_v_6  
        + p_p->c1907x*19.0*pow_u_18 * pow_v_7  
        + p_p->c1908x*19.0*pow_u_18 * pow_v_8  
        + p_p->c1909x*19.0*pow_u_18 * pow_v_9  
        + p_p->c1910x*19.0*pow_u_18 * pow_v_10  
        + p_p->c1911x*19.0*pow_u_18 * pow_v_11  
        + p_p->c1912x*19.0*pow_u_18 * pow_v_12  
        + p_p->c1913x*19.0*pow_u_18 * pow_v_13  
        + p_p->c1914x*19.0*pow_u_18 * pow_v_14  
        + p_p->c1915x*19.0*pow_u_18 * pow_v_15  
        + p_p->c1916x*19.0*pow_u_18 * pow_v_16  
        + p_p->c1917x*19.0*pow_u_18 * pow_v_17  
        + p_p->c1918x*19.0*pow_u_18 * pow_v_18  
        + p_p->c1919x*19.0*pow_u_18 * pow_v_19  
        + p_p->c1920x*19.0*pow_u_18 * pow_v_20  
        + p_p->c1921x*19.0*pow_u_18 * pow_v_21  ;
u_x=u_x + p_p->c2000x*20.0*pow_u_19 * pow_v_0  
        + p_p->c2001x*20.0*pow_u_19 * pow_v_1  
        + p_p->c2002x*20.0*pow_u_19 * pow_v_2  
        + p_p->c2003x*20.0*pow_u_19 * pow_v_3  
        + p_p->c2004x*20.0*pow_u_19 * pow_v_4  
        + p_p->c2005x*20.0*pow_u_19 * pow_v_5  
        + p_p->c2006x*20.0*pow_u_19 * pow_v_6  
        + p_p->c2007x*20.0*pow_u_19 * pow_v_7  
        + p_p->c2008x*20.0*pow_u_19 * pow_v_8  
        + p_p->c2009x*20.0*pow_u_19 * pow_v_9  
        + p_p->c2010x*20.0*pow_u_19 * pow_v_10  
        + p_p->c2011x*20.0*pow_u_19 * pow_v_11  
        + p_p->c2012x*20.0*pow_u_19 * pow_v_12  
        + p_p->c2013x*20.0*pow_u_19 * pow_v_13  
        + p_p->c2014x*20.0*pow_u_19 * pow_v_14  
        + p_p->c2015x*20.0*pow_u_19 * pow_v_15  
        + p_p->c2016x*20.0*pow_u_19 * pow_v_16  
        + p_p->c2017x*20.0*pow_u_19 * pow_v_17  
        + p_p->c2018x*20.0*pow_u_19 * pow_v_18  
        + p_p->c2019x*20.0*pow_u_19 * pow_v_19  
        + p_p->c2020x*20.0*pow_u_19 * pow_v_20  
        + p_p->c2021x*20.0*pow_u_19 * pow_v_21  ;
u_x=u_x + p_p->c2100x*21.0*pow_u_20 * pow_v_0  
        + p_p->c2101x*21.0*pow_u_20 * pow_v_1  
        + p_p->c2102x*21.0*pow_u_20 * pow_v_2  
        + p_p->c2103x*21.0*pow_u_20 * pow_v_3  
        + p_p->c2104x*21.0*pow_u_20 * pow_v_4  
        + p_p->c2105x*21.0*pow_u_20 * pow_v_5  
        + p_p->c2106x*21.0*pow_u_20 * pow_v_6  
        + p_p->c2107x*21.0*pow_u_20 * pow_v_7  
        + p_p->c2108x*21.0*pow_u_20 * pow_v_8  
        + p_p->c2109x*21.0*pow_u_20 * pow_v_9  
        + p_p->c2110x*21.0*pow_u_20 * pow_v_10  
        + p_p->c2111x*21.0*pow_u_20 * pow_v_11  
        + p_p->c2112x*21.0*pow_u_20 * pow_v_12  
        + p_p->c2113x*21.0*pow_u_20 * pow_v_13  
        + p_p->c2114x*21.0*pow_u_20 * pow_v_14  
        + p_p->c2115x*21.0*pow_u_20 * pow_v_15  
        + p_p->c2116x*21.0*pow_u_20 * pow_v_16  
        + p_p->c2117x*21.0*pow_u_20 * pow_v_17  
        + p_p->c2118x*21.0*pow_u_20 * pow_v_18  
        + p_p->c2119x*21.0*pow_u_20 * pow_v_19  
        + p_p->c2120x*21.0*pow_u_20 * pow_v_20  
        + p_p->c2121x*21.0*pow_u_20 * pow_v_21  ;

u_y     = p_p->c0000y*         0.0         * pow_v_0  
        + p_p->c0001y*         0.0         * pow_v_1  
        + p_p->c0002y*         0.0         * pow_v_2  
        + p_p->c0003y*         0.0         * pow_v_3  
        + p_p->c0004y*         0.0         * pow_v_4  
        + p_p->c0005y*         0.0         * pow_v_5  
        + p_p->c0006y*         0.0         * pow_v_6  
        + p_p->c0007y*         0.0         * pow_v_7  
        + p_p->c0008y*         0.0         * pow_v_8  
        + p_p->c0009y*         0.0         * pow_v_9  
        + p_p->c0010y*         0.0         * pow_v_10  
        + p_p->c0011y*         0.0         * pow_v_11  
        + p_p->c0012y*         0.0         * pow_v_12  
        + p_p->c0013y*         0.0         * pow_v_13  
        + p_p->c0014y*         0.0         * pow_v_14  
        + p_p->c0015y*         0.0         * pow_v_15  
        + p_p->c0016y*         0.0         * pow_v_16  
        + p_p->c0017y*         0.0         * pow_v_17  
        + p_p->c0018y*         0.0         * pow_v_18  
        + p_p->c0019y*         0.0         * pow_v_19  
        + p_p->c0020y*         0.0         * pow_v_20  
        + p_p->c0021y*         0.0         * pow_v_21  ;
u_y=u_y + p_p->c0100y* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101y* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102y* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103y* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104y* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105y* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106y* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107y* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0108y* 1.0*pow_u_0 * pow_v_8  
        + p_p->c0109y* 1.0*pow_u_0 * pow_v_9  
        + p_p->c0110y* 1.0*pow_u_0 * pow_v_10  
        + p_p->c0111y* 1.0*pow_u_0 * pow_v_11  
        + p_p->c0112y* 1.0*pow_u_0 * pow_v_12  
        + p_p->c0113y* 1.0*pow_u_0 * pow_v_13  
        + p_p->c0114y* 1.0*pow_u_0 * pow_v_14  
        + p_p->c0115y* 1.0*pow_u_0 * pow_v_15  
        + p_p->c0116y* 1.0*pow_u_0 * pow_v_16  
        + p_p->c0117y* 1.0*pow_u_0 * pow_v_17  
        + p_p->c0118y* 1.0*pow_u_0 * pow_v_18  
        + p_p->c0119y* 1.0*pow_u_0 * pow_v_19  
        + p_p->c0120y* 1.0*pow_u_0 * pow_v_20  
        + p_p->c0121y* 1.0*pow_u_0 * pow_v_21  ;
u_y=u_y + p_p->c0200y* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201y* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202y* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203y* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204y* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205y* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206y* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207y* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0208y* 2.0*pow_u_1 * pow_v_8  
        + p_p->c0209y* 2.0*pow_u_1 * pow_v_9  
        + p_p->c0210y* 2.0*pow_u_1 * pow_v_10  
        + p_p->c0211y* 2.0*pow_u_1 * pow_v_11  
        + p_p->c0212y* 2.0*pow_u_1 * pow_v_12  
        + p_p->c0213y* 2.0*pow_u_1 * pow_v_13  
        + p_p->c0214y* 2.0*pow_u_1 * pow_v_14  
        + p_p->c0215y* 2.0*pow_u_1 * pow_v_15  
        + p_p->c0216y* 2.0*pow_u_1 * pow_v_16  
        + p_p->c0217y* 2.0*pow_u_1 * pow_v_17  
        + p_p->c0218y* 2.0*pow_u_1 * pow_v_18  
        + p_p->c0219y* 2.0*pow_u_1 * pow_v_19  
        + p_p->c0220y* 2.0*pow_u_1 * pow_v_20  
        + p_p->c0221y* 2.0*pow_u_1 * pow_v_21  ;
u_y=u_y + p_p->c0300y* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301y* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302y* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303y* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304y* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305y* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306y* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307y* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0308y* 3.0*pow_u_2 * pow_v_8  
        + p_p->c0309y* 3.0*pow_u_2 * pow_v_9  
        + p_p->c0310y* 3.0*pow_u_2 * pow_v_10  
        + p_p->c0311y* 3.0*pow_u_2 * pow_v_11  
        + p_p->c0312y* 3.0*pow_u_2 * pow_v_12  
        + p_p->c0313y* 3.0*pow_u_2 * pow_v_13  
        + p_p->c0314y* 3.0*pow_u_2 * pow_v_14  
        + p_p->c0315y* 3.0*pow_u_2 * pow_v_15  
        + p_p->c0316y* 3.0*pow_u_2 * pow_v_16  
        + p_p->c0317y* 3.0*pow_u_2 * pow_v_17  
        + p_p->c0318y* 3.0*pow_u_2 * pow_v_18  
        + p_p->c0319y* 3.0*pow_u_2 * pow_v_19  
        + p_p->c0320y* 3.0*pow_u_2 * pow_v_20  
        + p_p->c0321y* 3.0*pow_u_2 * pow_v_21  ;
u_y=u_y + p_p->c0400y* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401y* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402y* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403y* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404y* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405y* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406y* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407y* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0408y* 4.0*pow_u_3 * pow_v_8  
        + p_p->c0409y* 4.0*pow_u_3 * pow_v_9  
        + p_p->c0410y* 4.0*pow_u_3 * pow_v_10  
        + p_p->c0411y* 4.0*pow_u_3 * pow_v_11  
        + p_p->c0412y* 4.0*pow_u_3 * pow_v_12  
        + p_p->c0413y* 4.0*pow_u_3 * pow_v_13  
        + p_p->c0414y* 4.0*pow_u_3 * pow_v_14  
        + p_p->c0415y* 4.0*pow_u_3 * pow_v_15  
        + p_p->c0416y* 4.0*pow_u_3 * pow_v_16  
        + p_p->c0417y* 4.0*pow_u_3 * pow_v_17  
        + p_p->c0418y* 4.0*pow_u_3 * pow_v_18  
        + p_p->c0419y* 4.0*pow_u_3 * pow_v_19  
        + p_p->c0420y* 4.0*pow_u_3 * pow_v_20  
        + p_p->c0421y* 4.0*pow_u_3 * pow_v_21  ;
u_y=u_y + p_p->c0500y* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501y* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502y* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503y* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504y* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505y* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506y* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507y* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0508y* 5.0*pow_u_4 * pow_v_8  
        + p_p->c0509y* 5.0*pow_u_4 * pow_v_9  
        + p_p->c0510y* 5.0*pow_u_4 * pow_v_10  
        + p_p->c0511y* 5.0*pow_u_4 * pow_v_11  
        + p_p->c0512y* 5.0*pow_u_4 * pow_v_12  
        + p_p->c0513y* 5.0*pow_u_4 * pow_v_13  
        + p_p->c0514y* 5.0*pow_u_4 * pow_v_14  
        + p_p->c0515y* 5.0*pow_u_4 * pow_v_15  
        + p_p->c0516y* 5.0*pow_u_4 * pow_v_16  
        + p_p->c0517y* 5.0*pow_u_4 * pow_v_17  
        + p_p->c0518y* 5.0*pow_u_4 * pow_v_18  
        + p_p->c0519y* 5.0*pow_u_4 * pow_v_19  
        + p_p->c0520y* 5.0*pow_u_4 * pow_v_20  
        + p_p->c0521y* 5.0*pow_u_4 * pow_v_21  ;
u_y=u_y + p_p->c0600y* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601y* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602y* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603y* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604y* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605y* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606y* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607y* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0608y* 6.0*pow_u_5 * pow_v_8  
        + p_p->c0609y* 6.0*pow_u_5 * pow_v_9  
        + p_p->c0610y* 6.0*pow_u_5 * pow_v_10  
        + p_p->c0611y* 6.0*pow_u_5 * pow_v_11  
        + p_p->c0612y* 6.0*pow_u_5 * pow_v_12  
        + p_p->c0613y* 6.0*pow_u_5 * pow_v_13  
        + p_p->c0614y* 6.0*pow_u_5 * pow_v_14  
        + p_p->c0615y* 6.0*pow_u_5 * pow_v_15  
        + p_p->c0616y* 6.0*pow_u_5 * pow_v_16  
        + p_p->c0617y* 6.0*pow_u_5 * pow_v_17  
        + p_p->c0618y* 6.0*pow_u_5 * pow_v_18  
        + p_p->c0619y* 6.0*pow_u_5 * pow_v_19  
        + p_p->c0620y* 6.0*pow_u_5 * pow_v_20  
        + p_p->c0621y* 6.0*pow_u_5 * pow_v_21  ;
u_y=u_y + p_p->c0700y* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701y* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702y* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703y* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704y* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705y* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706y* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707y* 7.0*pow_u_6 * pow_v_7  
        + p_p->c0708y* 7.0*pow_u_6 * pow_v_8  
        + p_p->c0709y* 7.0*pow_u_6 * pow_v_9  
        + p_p->c0710y* 7.0*pow_u_6 * pow_v_10  
        + p_p->c0711y* 7.0*pow_u_6 * pow_v_11  
        + p_p->c0712y* 7.0*pow_u_6 * pow_v_12  
        + p_p->c0713y* 7.0*pow_u_6 * pow_v_13  
        + p_p->c0714y* 7.0*pow_u_6 * pow_v_14  
        + p_p->c0715y* 7.0*pow_u_6 * pow_v_15  
        + p_p->c0716y* 7.0*pow_u_6 * pow_v_16  
        + p_p->c0717y* 7.0*pow_u_6 * pow_v_17  
        + p_p->c0718y* 7.0*pow_u_6 * pow_v_18  
        + p_p->c0719y* 7.0*pow_u_6 * pow_v_19  
        + p_p->c0720y* 7.0*pow_u_6 * pow_v_20  
        + p_p->c0721y* 7.0*pow_u_6 * pow_v_21  ;
u_y=u_y + p_p->c0800y* 8.0*pow_u_7 * pow_v_0  
        + p_p->c0801y* 8.0*pow_u_7 * pow_v_1  
        + p_p->c0802y* 8.0*pow_u_7 * pow_v_2  
        + p_p->c0803y* 8.0*pow_u_7 * pow_v_3  
        + p_p->c0804y* 8.0*pow_u_7 * pow_v_4  
        + p_p->c0805y* 8.0*pow_u_7 * pow_v_5  
        + p_p->c0806y* 8.0*pow_u_7 * pow_v_6  
        + p_p->c0807y* 8.0*pow_u_7 * pow_v_7  
        + p_p->c0808y* 8.0*pow_u_7 * pow_v_8  
        + p_p->c0809y* 8.0*pow_u_7 * pow_v_9  
        + p_p->c0810y* 8.0*pow_u_7 * pow_v_10  
        + p_p->c0811y* 8.0*pow_u_7 * pow_v_11  
        + p_p->c0812y* 8.0*pow_u_7 * pow_v_12  
        + p_p->c0813y* 8.0*pow_u_7 * pow_v_13  
        + p_p->c0814y* 8.0*pow_u_7 * pow_v_14  
        + p_p->c0815y* 8.0*pow_u_7 * pow_v_15  
        + p_p->c0816y* 8.0*pow_u_7 * pow_v_16  
        + p_p->c0817y* 8.0*pow_u_7 * pow_v_17  
        + p_p->c0818y* 8.0*pow_u_7 * pow_v_18  
        + p_p->c0819y* 8.0*pow_u_7 * pow_v_19  
        + p_p->c0820y* 8.0*pow_u_7 * pow_v_20  
        + p_p->c0821y* 8.0*pow_u_7 * pow_v_21  ;
u_y=u_y + p_p->c0900y* 9.0*pow_u_8 * pow_v_0  
        + p_p->c0901y* 9.0*pow_u_8 * pow_v_1  
        + p_p->c0902y* 9.0*pow_u_8 * pow_v_2  
        + p_p->c0903y* 9.0*pow_u_8 * pow_v_3  
        + p_p->c0904y* 9.0*pow_u_8 * pow_v_4  
        + p_p->c0905y* 9.0*pow_u_8 * pow_v_5  
        + p_p->c0906y* 9.0*pow_u_8 * pow_v_6  
        + p_p->c0907y* 9.0*pow_u_8 * pow_v_7  
        + p_p->c0908y* 9.0*pow_u_8 * pow_v_8  
        + p_p->c0909y* 9.0*pow_u_8 * pow_v_9  
        + p_p->c0910y* 9.0*pow_u_8 * pow_v_10  
        + p_p->c0911y* 9.0*pow_u_8 * pow_v_11  
        + p_p->c0912y* 9.0*pow_u_8 * pow_v_12  
        + p_p->c0913y* 9.0*pow_u_8 * pow_v_13  
        + p_p->c0914y* 9.0*pow_u_8 * pow_v_14  
        + p_p->c0915y* 9.0*pow_u_8 * pow_v_15  
        + p_p->c0916y* 9.0*pow_u_8 * pow_v_16  
        + p_p->c0917y* 9.0*pow_u_8 * pow_v_17  
        + p_p->c0918y* 9.0*pow_u_8 * pow_v_18  
        + p_p->c0919y* 9.0*pow_u_8 * pow_v_19  
        + p_p->c0920y* 9.0*pow_u_8 * pow_v_20  
        + p_p->c0921y* 9.0*pow_u_8 * pow_v_21  ;
u_y=u_y + p_p->c1000y*10.0*pow_u_9 * pow_v_0  
        + p_p->c1001y*10.0*pow_u_9 * pow_v_1  
        + p_p->c1002y*10.0*pow_u_9 * pow_v_2  
        + p_p->c1003y*10.0*pow_u_9 * pow_v_3  
        + p_p->c1004y*10.0*pow_u_9 * pow_v_4  
        + p_p->c1005y*10.0*pow_u_9 * pow_v_5  
        + p_p->c1006y*10.0*pow_u_9 * pow_v_6  
        + p_p->c1007y*10.0*pow_u_9 * pow_v_7  
        + p_p->c1008y*10.0*pow_u_9 * pow_v_8  
        + p_p->c1009y*10.0*pow_u_9 * pow_v_9  
        + p_p->c1010y*10.0*pow_u_9 * pow_v_10  
        + p_p->c1011y*10.0*pow_u_9 * pow_v_11  
        + p_p->c1012y*10.0*pow_u_9 * pow_v_12  
        + p_p->c1013y*10.0*pow_u_9 * pow_v_13  
        + p_p->c1014y*10.0*pow_u_9 * pow_v_14  
        + p_p->c1015y*10.0*pow_u_9 * pow_v_15  
        + p_p->c1016y*10.0*pow_u_9 * pow_v_16  
        + p_p->c1017y*10.0*pow_u_9 * pow_v_17  
        + p_p->c1018y*10.0*pow_u_9 * pow_v_18  
        + p_p->c1019y*10.0*pow_u_9 * pow_v_19  
        + p_p->c1020y*10.0*pow_u_9 * pow_v_20  
        + p_p->c1021y*10.0*pow_u_9 * pow_v_21  ;
u_y=u_y + p_p->c1100y*11.0*pow_u_10 * pow_v_0  
        + p_p->c1101y*11.0*pow_u_10 * pow_v_1  
        + p_p->c1102y*11.0*pow_u_10 * pow_v_2  
        + p_p->c1103y*11.0*pow_u_10 * pow_v_3  
        + p_p->c1104y*11.0*pow_u_10 * pow_v_4  
        + p_p->c1105y*11.0*pow_u_10 * pow_v_5  
        + p_p->c1106y*11.0*pow_u_10 * pow_v_6  
        + p_p->c1107y*11.0*pow_u_10 * pow_v_7  
        + p_p->c1108y*11.0*pow_u_10 * pow_v_8  
        + p_p->c1109y*11.0*pow_u_10 * pow_v_9  
        + p_p->c1110y*11.0*pow_u_10 * pow_v_10  
        + p_p->c1111y*11.0*pow_u_10 * pow_v_11  
        + p_p->c1112y*11.0*pow_u_10 * pow_v_12  
        + p_p->c1113y*11.0*pow_u_10 * pow_v_13  
        + p_p->c1114y*11.0*pow_u_10 * pow_v_14  
        + p_p->c1115y*11.0*pow_u_10 * pow_v_15  
        + p_p->c1116y*11.0*pow_u_10 * pow_v_16  
        + p_p->c1117y*11.0*pow_u_10 * pow_v_17  
        + p_p->c1118y*11.0*pow_u_10 * pow_v_18  
        + p_p->c1119y*11.0*pow_u_10 * pow_v_19  
        + p_p->c1120y*11.0*pow_u_10 * pow_v_20  
        + p_p->c1121y*11.0*pow_u_10 * pow_v_21  ;
u_y=u_y + p_p->c1200y*12.0*pow_u_11 * pow_v_0  
        + p_p->c1201y*12.0*pow_u_11 * pow_v_1  
        + p_p->c1202y*12.0*pow_u_11 * pow_v_2  
        + p_p->c1203y*12.0*pow_u_11 * pow_v_3  
        + p_p->c1204y*12.0*pow_u_11 * pow_v_4  
        + p_p->c1205y*12.0*pow_u_11 * pow_v_5  
        + p_p->c1206y*12.0*pow_u_11 * pow_v_6  
        + p_p->c1207y*12.0*pow_u_11 * pow_v_7  
        + p_p->c1208y*12.0*pow_u_11 * pow_v_8  
        + p_p->c1209y*12.0*pow_u_11 * pow_v_9  
        + p_p->c1210y*12.0*pow_u_11 * pow_v_10  
        + p_p->c1211y*12.0*pow_u_11 * pow_v_11  
        + p_p->c1212y*12.0*pow_u_11 * pow_v_12  
        + p_p->c1213y*12.0*pow_u_11 * pow_v_13  
        + p_p->c1214y*12.0*pow_u_11 * pow_v_14  
        + p_p->c1215y*12.0*pow_u_11 * pow_v_15  
        + p_p->c1216y*12.0*pow_u_11 * pow_v_16  
        + p_p->c1217y*12.0*pow_u_11 * pow_v_17  
        + p_p->c1218y*12.0*pow_u_11 * pow_v_18  
        + p_p->c1219y*12.0*pow_u_11 * pow_v_19  
        + p_p->c1220y*12.0*pow_u_11 * pow_v_20  
        + p_p->c1221y*12.0*pow_u_11 * pow_v_21  ;
u_y=u_y + p_p->c1300y*13.0*pow_u_12 * pow_v_0  
        + p_p->c1301y*13.0*pow_u_12 * pow_v_1  
        + p_p->c1302y*13.0*pow_u_12 * pow_v_2  
        + p_p->c1303y*13.0*pow_u_12 * pow_v_3  
        + p_p->c1304y*13.0*pow_u_12 * pow_v_4  
        + p_p->c1305y*13.0*pow_u_12 * pow_v_5  
        + p_p->c1306y*13.0*pow_u_12 * pow_v_6  
        + p_p->c1307y*13.0*pow_u_12 * pow_v_7  
        + p_p->c1308y*13.0*pow_u_12 * pow_v_8  
        + p_p->c1309y*13.0*pow_u_12 * pow_v_9  
        + p_p->c1310y*13.0*pow_u_12 * pow_v_10  
        + p_p->c1311y*13.0*pow_u_12 * pow_v_11  
        + p_p->c1312y*13.0*pow_u_12 * pow_v_12  
        + p_p->c1313y*13.0*pow_u_12 * pow_v_13  
        + p_p->c1314y*13.0*pow_u_12 * pow_v_14  
        + p_p->c1315y*13.0*pow_u_12 * pow_v_15  
        + p_p->c1316y*13.0*pow_u_12 * pow_v_16  
        + p_p->c1317y*13.0*pow_u_12 * pow_v_17  
        + p_p->c1318y*13.0*pow_u_12 * pow_v_18  
        + p_p->c1319y*13.0*pow_u_12 * pow_v_19  
        + p_p->c1320y*13.0*pow_u_12 * pow_v_20  
        + p_p->c1321y*13.0*pow_u_12 * pow_v_21  ;
u_y=u_y + p_p->c1400y*14.0*pow_u_13 * pow_v_0  
        + p_p->c1401y*14.0*pow_u_13 * pow_v_1  
        + p_p->c1402y*14.0*pow_u_13 * pow_v_2  
        + p_p->c1403y*14.0*pow_u_13 * pow_v_3  
        + p_p->c1404y*14.0*pow_u_13 * pow_v_4  
        + p_p->c1405y*14.0*pow_u_13 * pow_v_5  
        + p_p->c1406y*14.0*pow_u_13 * pow_v_6  
        + p_p->c1407y*14.0*pow_u_13 * pow_v_7  
        + p_p->c1408y*14.0*pow_u_13 * pow_v_8  
        + p_p->c1409y*14.0*pow_u_13 * pow_v_9  
        + p_p->c1410y*14.0*pow_u_13 * pow_v_10  
        + p_p->c1411y*14.0*pow_u_13 * pow_v_11  
        + p_p->c1412y*14.0*pow_u_13 * pow_v_12  
        + p_p->c1413y*14.0*pow_u_13 * pow_v_13  
        + p_p->c1414y*14.0*pow_u_13 * pow_v_14  
        + p_p->c1415y*14.0*pow_u_13 * pow_v_15  
        + p_p->c1416y*14.0*pow_u_13 * pow_v_16  
        + p_p->c1417y*14.0*pow_u_13 * pow_v_17  
        + p_p->c1418y*14.0*pow_u_13 * pow_v_18  
        + p_p->c1419y*14.0*pow_u_13 * pow_v_19  
        + p_p->c1420y*14.0*pow_u_13 * pow_v_20  
        + p_p->c1421y*14.0*pow_u_13 * pow_v_21  ;
u_y=u_y + p_p->c1500y*15.0*pow_u_14 * pow_v_0  
        + p_p->c1501y*15.0*pow_u_14 * pow_v_1  
        + p_p->c1502y*15.0*pow_u_14 * pow_v_2  
        + p_p->c1503y*15.0*pow_u_14 * pow_v_3  
        + p_p->c1504y*15.0*pow_u_14 * pow_v_4  
        + p_p->c1505y*15.0*pow_u_14 * pow_v_5  
        + p_p->c1506y*15.0*pow_u_14 * pow_v_6  
        + p_p->c1507y*15.0*pow_u_14 * pow_v_7  
        + p_p->c1508y*15.0*pow_u_14 * pow_v_8  
        + p_p->c1509y*15.0*pow_u_14 * pow_v_9  
        + p_p->c1510y*15.0*pow_u_14 * pow_v_10  
        + p_p->c1511y*15.0*pow_u_14 * pow_v_11  
        + p_p->c1512y*15.0*pow_u_14 * pow_v_12  
        + p_p->c1513y*15.0*pow_u_14 * pow_v_13  
        + p_p->c1514y*15.0*pow_u_14 * pow_v_14  
        + p_p->c1515y*15.0*pow_u_14 * pow_v_15  
        + p_p->c1516y*15.0*pow_u_14 * pow_v_16  
        + p_p->c1517y*15.0*pow_u_14 * pow_v_17  
        + p_p->c1518y*15.0*pow_u_14 * pow_v_18  
        + p_p->c1519y*15.0*pow_u_14 * pow_v_19  
        + p_p->c1520y*15.0*pow_u_14 * pow_v_20  
        + p_p->c1521y*15.0*pow_u_14 * pow_v_21  ;
u_y=u_y + p_p->c1600y*16.0*pow_u_15 * pow_v_0  
        + p_p->c1601y*16.0*pow_u_15 * pow_v_1  
        + p_p->c1602y*16.0*pow_u_15 * pow_v_2  
        + p_p->c1603y*16.0*pow_u_15 * pow_v_3  
        + p_p->c1604y*16.0*pow_u_15 * pow_v_4  
        + p_p->c1605y*16.0*pow_u_15 * pow_v_5  
        + p_p->c1606y*16.0*pow_u_15 * pow_v_6  
        + p_p->c1607y*16.0*pow_u_15 * pow_v_7  
        + p_p->c1608y*16.0*pow_u_15 * pow_v_8  
        + p_p->c1609y*16.0*pow_u_15 * pow_v_9  
        + p_p->c1610y*16.0*pow_u_15 * pow_v_10  
        + p_p->c1611y*16.0*pow_u_15 * pow_v_11  
        + p_p->c1612y*16.0*pow_u_15 * pow_v_12  
        + p_p->c1613y*16.0*pow_u_15 * pow_v_13  
        + p_p->c1614y*16.0*pow_u_15 * pow_v_14  
        + p_p->c1615y*16.0*pow_u_15 * pow_v_15  
        + p_p->c1616y*16.0*pow_u_15 * pow_v_16  
        + p_p->c1617y*16.0*pow_u_15 * pow_v_17  
        + p_p->c1618y*16.0*pow_u_15 * pow_v_18  
        + p_p->c1619y*16.0*pow_u_15 * pow_v_19  
        + p_p->c1620y*16.0*pow_u_15 * pow_v_20  
        + p_p->c1621y*16.0*pow_u_15 * pow_v_21  ;
u_y=u_y + p_p->c1700y*17.0*pow_u_16 * pow_v_0  
        + p_p->c1701y*17.0*pow_u_16 * pow_v_1  
        + p_p->c1702y*17.0*pow_u_16 * pow_v_2  
        + p_p->c1703y*17.0*pow_u_16 * pow_v_3  
        + p_p->c1704y*17.0*pow_u_16 * pow_v_4  
        + p_p->c1705y*17.0*pow_u_16 * pow_v_5  
        + p_p->c1706y*17.0*pow_u_16 * pow_v_6  
        + p_p->c1707y*17.0*pow_u_16 * pow_v_7  
        + p_p->c1708y*17.0*pow_u_16 * pow_v_8  
        + p_p->c1709y*17.0*pow_u_16 * pow_v_9  
        + p_p->c1710y*17.0*pow_u_16 * pow_v_10  
        + p_p->c1711y*17.0*pow_u_16 * pow_v_11  
        + p_p->c1712y*17.0*pow_u_16 * pow_v_12  
        + p_p->c1713y*17.0*pow_u_16 * pow_v_13  
        + p_p->c1714y*17.0*pow_u_16 * pow_v_14  
        + p_p->c1715y*17.0*pow_u_16 * pow_v_15  
        + p_p->c1716y*17.0*pow_u_16 * pow_v_16  
        + p_p->c1717y*17.0*pow_u_16 * pow_v_17  
        + p_p->c1718y*17.0*pow_u_16 * pow_v_18  
        + p_p->c1719y*17.0*pow_u_16 * pow_v_19  
        + p_p->c1720y*17.0*pow_u_16 * pow_v_20  
        + p_p->c1721y*17.0*pow_u_16 * pow_v_21  ;
u_y=u_y + p_p->c1800y*18.0*pow_u_17 * pow_v_0  
        + p_p->c1801y*18.0*pow_u_17 * pow_v_1  
        + p_p->c1802y*18.0*pow_u_17 * pow_v_2  
        + p_p->c1803y*18.0*pow_u_17 * pow_v_3  
        + p_p->c1804y*18.0*pow_u_17 * pow_v_4  
        + p_p->c1805y*18.0*pow_u_17 * pow_v_5  
        + p_p->c1806y*18.0*pow_u_17 * pow_v_6  
        + p_p->c1807y*18.0*pow_u_17 * pow_v_7  
        + p_p->c1808y*18.0*pow_u_17 * pow_v_8  
        + p_p->c1809y*18.0*pow_u_17 * pow_v_9  
        + p_p->c1810y*18.0*pow_u_17 * pow_v_10  
        + p_p->c1811y*18.0*pow_u_17 * pow_v_11  
        + p_p->c1812y*18.0*pow_u_17 * pow_v_12  
        + p_p->c1813y*18.0*pow_u_17 * pow_v_13  
        + p_p->c1814y*18.0*pow_u_17 * pow_v_14  
        + p_p->c1815y*18.0*pow_u_17 * pow_v_15  
        + p_p->c1816y*18.0*pow_u_17 * pow_v_16  
        + p_p->c1817y*18.0*pow_u_17 * pow_v_17  
        + p_p->c1818y*18.0*pow_u_17 * pow_v_18  
        + p_p->c1819y*18.0*pow_u_17 * pow_v_19  
        + p_p->c1820y*18.0*pow_u_17 * pow_v_20  
        + p_p->c1821y*18.0*pow_u_17 * pow_v_21  ;
u_y=u_y + p_p->c1900y*19.0*pow_u_18 * pow_v_0  
        + p_p->c1901y*19.0*pow_u_18 * pow_v_1  
        + p_p->c1902y*19.0*pow_u_18 * pow_v_2  
        + p_p->c1903y*19.0*pow_u_18 * pow_v_3  
        + p_p->c1904y*19.0*pow_u_18 * pow_v_4  
        + p_p->c1905y*19.0*pow_u_18 * pow_v_5  
        + p_p->c1906y*19.0*pow_u_18 * pow_v_6  
        + p_p->c1907y*19.0*pow_u_18 * pow_v_7  
        + p_p->c1908y*19.0*pow_u_18 * pow_v_8  
        + p_p->c1909y*19.0*pow_u_18 * pow_v_9  
        + p_p->c1910y*19.0*pow_u_18 * pow_v_10  
        + p_p->c1911y*19.0*pow_u_18 * pow_v_11  
        + p_p->c1912y*19.0*pow_u_18 * pow_v_12  
        + p_p->c1913y*19.0*pow_u_18 * pow_v_13  
        + p_p->c1914y*19.0*pow_u_18 * pow_v_14  
        + p_p->c1915y*19.0*pow_u_18 * pow_v_15  
        + p_p->c1916y*19.0*pow_u_18 * pow_v_16  
        + p_p->c1917y*19.0*pow_u_18 * pow_v_17  
        + p_p->c1918y*19.0*pow_u_18 * pow_v_18  
        + p_p->c1919y*19.0*pow_u_18 * pow_v_19  
        + p_p->c1920y*19.0*pow_u_18 * pow_v_20  
        + p_p->c1921y*19.0*pow_u_18 * pow_v_21  ;
u_y=u_y + p_p->c2000y*20.0*pow_u_19 * pow_v_0  
        + p_p->c2001y*20.0*pow_u_19 * pow_v_1  
        + p_p->c2002y*20.0*pow_u_19 * pow_v_2  
        + p_p->c2003y*20.0*pow_u_19 * pow_v_3  
        + p_p->c2004y*20.0*pow_u_19 * pow_v_4  
        + p_p->c2005y*20.0*pow_u_19 * pow_v_5  
        + p_p->c2006y*20.0*pow_u_19 * pow_v_6  
        + p_p->c2007y*20.0*pow_u_19 * pow_v_7  
        + p_p->c2008y*20.0*pow_u_19 * pow_v_8  
        + p_p->c2009y*20.0*pow_u_19 * pow_v_9  
        + p_p->c2010y*20.0*pow_u_19 * pow_v_10  
        + p_p->c2011y*20.0*pow_u_19 * pow_v_11  
        + p_p->c2012y*20.0*pow_u_19 * pow_v_12  
        + p_p->c2013y*20.0*pow_u_19 * pow_v_13  
        + p_p->c2014y*20.0*pow_u_19 * pow_v_14  
        + p_p->c2015y*20.0*pow_u_19 * pow_v_15  
        + p_p->c2016y*20.0*pow_u_19 * pow_v_16  
        + p_p->c2017y*20.0*pow_u_19 * pow_v_17  
        + p_p->c2018y*20.0*pow_u_19 * pow_v_18  
        + p_p->c2019y*20.0*pow_u_19 * pow_v_19  
        + p_p->c2020y*20.0*pow_u_19 * pow_v_20  
        + p_p->c2021y*20.0*pow_u_19 * pow_v_21  ;
u_y=u_y + p_p->c2100y*21.0*pow_u_20 * pow_v_0  
        + p_p->c2101y*21.0*pow_u_20 * pow_v_1  
        + p_p->c2102y*21.0*pow_u_20 * pow_v_2  
        + p_p->c2103y*21.0*pow_u_20 * pow_v_3  
        + p_p->c2104y*21.0*pow_u_20 * pow_v_4  
        + p_p->c2105y*21.0*pow_u_20 * pow_v_5  
        + p_p->c2106y*21.0*pow_u_20 * pow_v_6  
        + p_p->c2107y*21.0*pow_u_20 * pow_v_7  
        + p_p->c2108y*21.0*pow_u_20 * pow_v_8  
        + p_p->c2109y*21.0*pow_u_20 * pow_v_9  
        + p_p->c2110y*21.0*pow_u_20 * pow_v_10  
        + p_p->c2111y*21.0*pow_u_20 * pow_v_11  
        + p_p->c2112y*21.0*pow_u_20 * pow_v_12  
        + p_p->c2113y*21.0*pow_u_20 * pow_v_13  
        + p_p->c2114y*21.0*pow_u_20 * pow_v_14  
        + p_p->c2115y*21.0*pow_u_20 * pow_v_15  
        + p_p->c2116y*21.0*pow_u_20 * pow_v_16  
        + p_p->c2117y*21.0*pow_u_20 * pow_v_17  
        + p_p->c2118y*21.0*pow_u_20 * pow_v_18  
        + p_p->c2119y*21.0*pow_u_20 * pow_v_19  
        + p_p->c2120y*21.0*pow_u_20 * pow_v_20  
        + p_p->c2121y*21.0*pow_u_20 * pow_v_21  ;

u_z     = p_p->c0000z*         0.0         * pow_v_0  
        + p_p->c0001z*         0.0         * pow_v_1  
        + p_p->c0002z*         0.0         * pow_v_2  
        + p_p->c0003z*         0.0         * pow_v_3  
        + p_p->c0004z*         0.0         * pow_v_4  
        + p_p->c0005z*         0.0         * pow_v_5  
        + p_p->c0006z*         0.0         * pow_v_6  
        + p_p->c0007z*         0.0         * pow_v_7  
        + p_p->c0008z*         0.0         * pow_v_8  
        + p_p->c0009z*         0.0         * pow_v_9  
        + p_p->c0010z*         0.0         * pow_v_10  
        + p_p->c0011z*         0.0         * pow_v_11  
        + p_p->c0012z*         0.0         * pow_v_12  
        + p_p->c0013z*         0.0         * pow_v_13  
        + p_p->c0014z*         0.0         * pow_v_14  
        + p_p->c0015z*         0.0         * pow_v_15  
        + p_p->c0016z*         0.0         * pow_v_16  
        + p_p->c0017z*         0.0         * pow_v_17  
        + p_p->c0018z*         0.0         * pow_v_18  
        + p_p->c0019z*         0.0         * pow_v_19  
        + p_p->c0020z*         0.0         * pow_v_20  
        + p_p->c0021z*         0.0         * pow_v_21  ;
u_z=u_z + p_p->c0100z* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101z* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102z* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103z* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104z* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105z* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106z* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107z* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0108z* 1.0*pow_u_0 * pow_v_8  
        + p_p->c0109z* 1.0*pow_u_0 * pow_v_9  
        + p_p->c0110z* 1.0*pow_u_0 * pow_v_10  
        + p_p->c0111z* 1.0*pow_u_0 * pow_v_11  
        + p_p->c0112z* 1.0*pow_u_0 * pow_v_12  
        + p_p->c0113z* 1.0*pow_u_0 * pow_v_13  
        + p_p->c0114z* 1.0*pow_u_0 * pow_v_14  
        + p_p->c0115z* 1.0*pow_u_0 * pow_v_15  
        + p_p->c0116z* 1.0*pow_u_0 * pow_v_16  
        + p_p->c0117z* 1.0*pow_u_0 * pow_v_17  
        + p_p->c0118z* 1.0*pow_u_0 * pow_v_18  
        + p_p->c0119z* 1.0*pow_u_0 * pow_v_19  
        + p_p->c0120z* 1.0*pow_u_0 * pow_v_20  
        + p_p->c0121z* 1.0*pow_u_0 * pow_v_21  ;
u_z=u_z + p_p->c0200z* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201z* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202z* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203z* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204z* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205z* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206z* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207z* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0208z* 2.0*pow_u_1 * pow_v_8  
        + p_p->c0209z* 2.0*pow_u_1 * pow_v_9  
        + p_p->c0210z* 2.0*pow_u_1 * pow_v_10  
        + p_p->c0211z* 2.0*pow_u_1 * pow_v_11  
        + p_p->c0212z* 2.0*pow_u_1 * pow_v_12  
        + p_p->c0213z* 2.0*pow_u_1 * pow_v_13  
        + p_p->c0214z* 2.0*pow_u_1 * pow_v_14  
        + p_p->c0215z* 2.0*pow_u_1 * pow_v_15  
        + p_p->c0216z* 2.0*pow_u_1 * pow_v_16  
        + p_p->c0217z* 2.0*pow_u_1 * pow_v_17  
        + p_p->c0218z* 2.0*pow_u_1 * pow_v_18  
        + p_p->c0219z* 2.0*pow_u_1 * pow_v_19  
        + p_p->c0220z* 2.0*pow_u_1 * pow_v_20  
        + p_p->c0221z* 2.0*pow_u_1 * pow_v_21  ;
u_z=u_z + p_p->c0300z* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301z* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302z* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303z* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304z* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305z* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306z* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307z* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0308z* 3.0*pow_u_2 * pow_v_8  
        + p_p->c0309z* 3.0*pow_u_2 * pow_v_9  
        + p_p->c0310z* 3.0*pow_u_2 * pow_v_10  
        + p_p->c0311z* 3.0*pow_u_2 * pow_v_11  
        + p_p->c0312z* 3.0*pow_u_2 * pow_v_12  
        + p_p->c0313z* 3.0*pow_u_2 * pow_v_13  
        + p_p->c0314z* 3.0*pow_u_2 * pow_v_14  
        + p_p->c0315z* 3.0*pow_u_2 * pow_v_15  
        + p_p->c0316z* 3.0*pow_u_2 * pow_v_16  
        + p_p->c0317z* 3.0*pow_u_2 * pow_v_17  
        + p_p->c0318z* 3.0*pow_u_2 * pow_v_18  
        + p_p->c0319z* 3.0*pow_u_2 * pow_v_19  
        + p_p->c0320z* 3.0*pow_u_2 * pow_v_20  
        + p_p->c0321z* 3.0*pow_u_2 * pow_v_21  ;
u_z=u_z + p_p->c0400z* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401z* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402z* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403z* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404z* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405z* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406z* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407z* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0408z* 4.0*pow_u_3 * pow_v_8  
        + p_p->c0409z* 4.0*pow_u_3 * pow_v_9  
        + p_p->c0410z* 4.0*pow_u_3 * pow_v_10  
        + p_p->c0411z* 4.0*pow_u_3 * pow_v_11  
        + p_p->c0412z* 4.0*pow_u_3 * pow_v_12  
        + p_p->c0413z* 4.0*pow_u_3 * pow_v_13  
        + p_p->c0414z* 4.0*pow_u_3 * pow_v_14  
        + p_p->c0415z* 4.0*pow_u_3 * pow_v_15  
        + p_p->c0416z* 4.0*pow_u_3 * pow_v_16  
        + p_p->c0417z* 4.0*pow_u_3 * pow_v_17  
        + p_p->c0418z* 4.0*pow_u_3 * pow_v_18  
        + p_p->c0419z* 4.0*pow_u_3 * pow_v_19  
        + p_p->c0420z* 4.0*pow_u_3 * pow_v_20  
        + p_p->c0421z* 4.0*pow_u_3 * pow_v_21  ;
u_z=u_z + p_p->c0500z* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501z* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502z* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503z* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504z* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505z* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506z* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507z* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0508z* 5.0*pow_u_4 * pow_v_8  
        + p_p->c0509z* 5.0*pow_u_4 * pow_v_9  
        + p_p->c0510z* 5.0*pow_u_4 * pow_v_10  
        + p_p->c0511z* 5.0*pow_u_4 * pow_v_11  
        + p_p->c0512z* 5.0*pow_u_4 * pow_v_12  
        + p_p->c0513z* 5.0*pow_u_4 * pow_v_13  
        + p_p->c0514z* 5.0*pow_u_4 * pow_v_14  
        + p_p->c0515z* 5.0*pow_u_4 * pow_v_15  
        + p_p->c0516z* 5.0*pow_u_4 * pow_v_16  
        + p_p->c0517z* 5.0*pow_u_4 * pow_v_17  
        + p_p->c0518z* 5.0*pow_u_4 * pow_v_18  
        + p_p->c0519z* 5.0*pow_u_4 * pow_v_19  
        + p_p->c0520z* 5.0*pow_u_4 * pow_v_20  
        + p_p->c0521z* 5.0*pow_u_4 * pow_v_21  ;
u_z=u_z + p_p->c0600z* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601z* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602z* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603z* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604z* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605z* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606z* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607z* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0608z* 6.0*pow_u_5 * pow_v_8  
        + p_p->c0609z* 6.0*pow_u_5 * pow_v_9  
        + p_p->c0610z* 6.0*pow_u_5 * pow_v_10  
        + p_p->c0611z* 6.0*pow_u_5 * pow_v_11  
        + p_p->c0612z* 6.0*pow_u_5 * pow_v_12  
        + p_p->c0613z* 6.0*pow_u_5 * pow_v_13  
        + p_p->c0614z* 6.0*pow_u_5 * pow_v_14  
        + p_p->c0615z* 6.0*pow_u_5 * pow_v_15  
        + p_p->c0616z* 6.0*pow_u_5 * pow_v_16  
        + p_p->c0617z* 6.0*pow_u_5 * pow_v_17  
        + p_p->c0618z* 6.0*pow_u_5 * pow_v_18  
        + p_p->c0619z* 6.0*pow_u_5 * pow_v_19  
        + p_p->c0620z* 6.0*pow_u_5 * pow_v_20  
        + p_p->c0621z* 6.0*pow_u_5 * pow_v_21  ;
u_z=u_z + p_p->c0700z* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701z* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702z* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703z* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704z* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705z* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706z* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707z* 7.0*pow_u_6 * pow_v_7  
        + p_p->c0708z* 7.0*pow_u_6 * pow_v_8  
        + p_p->c0709z* 7.0*pow_u_6 * pow_v_9  
        + p_p->c0710z* 7.0*pow_u_6 * pow_v_10  
        + p_p->c0711z* 7.0*pow_u_6 * pow_v_11  
        + p_p->c0712z* 7.0*pow_u_6 * pow_v_12  
        + p_p->c0713z* 7.0*pow_u_6 * pow_v_13  
        + p_p->c0714z* 7.0*pow_u_6 * pow_v_14  
        + p_p->c0715z* 7.0*pow_u_6 * pow_v_15  
        + p_p->c0716z* 7.0*pow_u_6 * pow_v_16  
        + p_p->c0717z* 7.0*pow_u_6 * pow_v_17  
        + p_p->c0718z* 7.0*pow_u_6 * pow_v_18  
        + p_p->c0719z* 7.0*pow_u_6 * pow_v_19  
        + p_p->c0720z* 7.0*pow_u_6 * pow_v_20  
        + p_p->c0721z* 7.0*pow_u_6 * pow_v_21  ;
u_z=u_z + p_p->c0800z* 8.0*pow_u_7 * pow_v_0  
        + p_p->c0801z* 8.0*pow_u_7 * pow_v_1  
        + p_p->c0802z* 8.0*pow_u_7 * pow_v_2  
        + p_p->c0803z* 8.0*pow_u_7 * pow_v_3  
        + p_p->c0804z* 8.0*pow_u_7 * pow_v_4  
        + p_p->c0805z* 8.0*pow_u_7 * pow_v_5  
        + p_p->c0806z* 8.0*pow_u_7 * pow_v_6  
        + p_p->c0807z* 8.0*pow_u_7 * pow_v_7  
        + p_p->c0808z* 8.0*pow_u_7 * pow_v_8  
        + p_p->c0809z* 8.0*pow_u_7 * pow_v_9  
        + p_p->c0810z* 8.0*pow_u_7 * pow_v_10  
        + p_p->c0811z* 8.0*pow_u_7 * pow_v_11  
        + p_p->c0812z* 8.0*pow_u_7 * pow_v_12  
        + p_p->c0813z* 8.0*pow_u_7 * pow_v_13  
        + p_p->c0814z* 8.0*pow_u_7 * pow_v_14  
        + p_p->c0815z* 8.0*pow_u_7 * pow_v_15  
        + p_p->c0816z* 8.0*pow_u_7 * pow_v_16  
        + p_p->c0817z* 8.0*pow_u_7 * pow_v_17  
        + p_p->c0818z* 8.0*pow_u_7 * pow_v_18  
        + p_p->c0819z* 8.0*pow_u_7 * pow_v_19  
        + p_p->c0820z* 8.0*pow_u_7 * pow_v_20  
        + p_p->c0821z* 8.0*pow_u_7 * pow_v_21  ;
u_z=u_z + p_p->c0900z* 9.0*pow_u_8 * pow_v_0  
        + p_p->c0901z* 9.0*pow_u_8 * pow_v_1  
        + p_p->c0902z* 9.0*pow_u_8 * pow_v_2  
        + p_p->c0903z* 9.0*pow_u_8 * pow_v_3  
        + p_p->c0904z* 9.0*pow_u_8 * pow_v_4  
        + p_p->c0905z* 9.0*pow_u_8 * pow_v_5  
        + p_p->c0906z* 9.0*pow_u_8 * pow_v_6  
        + p_p->c0907z* 9.0*pow_u_8 * pow_v_7  
        + p_p->c0908z* 9.0*pow_u_8 * pow_v_8  
        + p_p->c0909z* 9.0*pow_u_8 * pow_v_9  
        + p_p->c0910z* 9.0*pow_u_8 * pow_v_10  
        + p_p->c0911z* 9.0*pow_u_8 * pow_v_11  
        + p_p->c0912z* 9.0*pow_u_8 * pow_v_12  
        + p_p->c0913z* 9.0*pow_u_8 * pow_v_13  
        + p_p->c0914z* 9.0*pow_u_8 * pow_v_14  
        + p_p->c0915z* 9.0*pow_u_8 * pow_v_15  
        + p_p->c0916z* 9.0*pow_u_8 * pow_v_16  
        + p_p->c0917z* 9.0*pow_u_8 * pow_v_17  
        + p_p->c0918z* 9.0*pow_u_8 * pow_v_18  
        + p_p->c0919z* 9.0*pow_u_8 * pow_v_19  
        + p_p->c0920z* 9.0*pow_u_8 * pow_v_20  
        + p_p->c0921z* 9.0*pow_u_8 * pow_v_21  ;
u_z=u_z + p_p->c1000z*10.0*pow_u_9 * pow_v_0  
        + p_p->c1001z*10.0*pow_u_9 * pow_v_1  
        + p_p->c1002z*10.0*pow_u_9 * pow_v_2  
        + p_p->c1003z*10.0*pow_u_9 * pow_v_3  
        + p_p->c1004z*10.0*pow_u_9 * pow_v_4  
        + p_p->c1005z*10.0*pow_u_9 * pow_v_5  
        + p_p->c1006z*10.0*pow_u_9 * pow_v_6  
        + p_p->c1007z*10.0*pow_u_9 * pow_v_7  
        + p_p->c1008z*10.0*pow_u_9 * pow_v_8  
        + p_p->c1009z*10.0*pow_u_9 * pow_v_9  
        + p_p->c1010z*10.0*pow_u_9 * pow_v_10  
        + p_p->c1011z*10.0*pow_u_9 * pow_v_11  
        + p_p->c1012z*10.0*pow_u_9 * pow_v_12  
        + p_p->c1013z*10.0*pow_u_9 * pow_v_13  
        + p_p->c1014z*10.0*pow_u_9 * pow_v_14  
        + p_p->c1015z*10.0*pow_u_9 * pow_v_15  
        + p_p->c1016z*10.0*pow_u_9 * pow_v_16  
        + p_p->c1017z*10.0*pow_u_9 * pow_v_17  
        + p_p->c1018z*10.0*pow_u_9 * pow_v_18  
        + p_p->c1019z*10.0*pow_u_9 * pow_v_19  
        + p_p->c1020z*10.0*pow_u_9 * pow_v_20  
        + p_p->c1021z*10.0*pow_u_9 * pow_v_21  ;
u_z=u_z + p_p->c1100z*11.0*pow_u_10 * pow_v_0  
        + p_p->c1101z*11.0*pow_u_10 * pow_v_1  
        + p_p->c1102z*11.0*pow_u_10 * pow_v_2  
        + p_p->c1103z*11.0*pow_u_10 * pow_v_3  
        + p_p->c1104z*11.0*pow_u_10 * pow_v_4  
        + p_p->c1105z*11.0*pow_u_10 * pow_v_5  
        + p_p->c1106z*11.0*pow_u_10 * pow_v_6  
        + p_p->c1107z*11.0*pow_u_10 * pow_v_7  
        + p_p->c1108z*11.0*pow_u_10 * pow_v_8  
        + p_p->c1109z*11.0*pow_u_10 * pow_v_9  
        + p_p->c1110z*11.0*pow_u_10 * pow_v_10  
        + p_p->c1111z*11.0*pow_u_10 * pow_v_11  
        + p_p->c1112z*11.0*pow_u_10 * pow_v_12  
        + p_p->c1113z*11.0*pow_u_10 * pow_v_13  
        + p_p->c1114z*11.0*pow_u_10 * pow_v_14  
        + p_p->c1115z*11.0*pow_u_10 * pow_v_15  
        + p_p->c1116z*11.0*pow_u_10 * pow_v_16  
        + p_p->c1117z*11.0*pow_u_10 * pow_v_17  
        + p_p->c1118z*11.0*pow_u_10 * pow_v_18  
        + p_p->c1119z*11.0*pow_u_10 * pow_v_19  
        + p_p->c1120z*11.0*pow_u_10 * pow_v_20  
        + p_p->c1121z*11.0*pow_u_10 * pow_v_21  ;
u_z=u_z + p_p->c1200z*12.0*pow_u_11 * pow_v_0  
        + p_p->c1201z*12.0*pow_u_11 * pow_v_1  
        + p_p->c1202z*12.0*pow_u_11 * pow_v_2  
        + p_p->c1203z*12.0*pow_u_11 * pow_v_3  
        + p_p->c1204z*12.0*pow_u_11 * pow_v_4  
        + p_p->c1205z*12.0*pow_u_11 * pow_v_5  
        + p_p->c1206z*12.0*pow_u_11 * pow_v_6  
        + p_p->c1207z*12.0*pow_u_11 * pow_v_7  
        + p_p->c1208z*12.0*pow_u_11 * pow_v_8  
        + p_p->c1209z*12.0*pow_u_11 * pow_v_9  
        + p_p->c1210z*12.0*pow_u_11 * pow_v_10  
        + p_p->c1211z*12.0*pow_u_11 * pow_v_11  
        + p_p->c1212z*12.0*pow_u_11 * pow_v_12  
        + p_p->c1213z*12.0*pow_u_11 * pow_v_13  
        + p_p->c1214z*12.0*pow_u_11 * pow_v_14  
        + p_p->c1215z*12.0*pow_u_11 * pow_v_15  
        + p_p->c1216z*12.0*pow_u_11 * pow_v_16  
        + p_p->c1217z*12.0*pow_u_11 * pow_v_17  
        + p_p->c1218z*12.0*pow_u_11 * pow_v_18  
        + p_p->c1219z*12.0*pow_u_11 * pow_v_19  
        + p_p->c1220z*12.0*pow_u_11 * pow_v_20  
        + p_p->c1221z*12.0*pow_u_11 * pow_v_21  ;
u_z=u_z + p_p->c1300z*13.0*pow_u_12 * pow_v_0  
        + p_p->c1301z*13.0*pow_u_12 * pow_v_1  
        + p_p->c1302z*13.0*pow_u_12 * pow_v_2  
        + p_p->c1303z*13.0*pow_u_12 * pow_v_3  
        + p_p->c1304z*13.0*pow_u_12 * pow_v_4  
        + p_p->c1305z*13.0*pow_u_12 * pow_v_5  
        + p_p->c1306z*13.0*pow_u_12 * pow_v_6  
        + p_p->c1307z*13.0*pow_u_12 * pow_v_7  
        + p_p->c1308z*13.0*pow_u_12 * pow_v_8  
        + p_p->c1309z*13.0*pow_u_12 * pow_v_9  
        + p_p->c1310z*13.0*pow_u_12 * pow_v_10  
        + p_p->c1311z*13.0*pow_u_12 * pow_v_11  
        + p_p->c1312z*13.0*pow_u_12 * pow_v_12  
        + p_p->c1313z*13.0*pow_u_12 * pow_v_13  
        + p_p->c1314z*13.0*pow_u_12 * pow_v_14  
        + p_p->c1315z*13.0*pow_u_12 * pow_v_15  
        + p_p->c1316z*13.0*pow_u_12 * pow_v_16  
        + p_p->c1317z*13.0*pow_u_12 * pow_v_17  
        + p_p->c1318z*13.0*pow_u_12 * pow_v_18  
        + p_p->c1319z*13.0*pow_u_12 * pow_v_19  
        + p_p->c1320z*13.0*pow_u_12 * pow_v_20  
        + p_p->c1321z*13.0*pow_u_12 * pow_v_21  ;
u_z=u_z + p_p->c1400z*14.0*pow_u_13 * pow_v_0  
        + p_p->c1401z*14.0*pow_u_13 * pow_v_1  
        + p_p->c1402z*14.0*pow_u_13 * pow_v_2  
        + p_p->c1403z*14.0*pow_u_13 * pow_v_3  
        + p_p->c1404z*14.0*pow_u_13 * pow_v_4  
        + p_p->c1405z*14.0*pow_u_13 * pow_v_5  
        + p_p->c1406z*14.0*pow_u_13 * pow_v_6  
        + p_p->c1407z*14.0*pow_u_13 * pow_v_7  
        + p_p->c1408z*14.0*pow_u_13 * pow_v_8  
        + p_p->c1409z*14.0*pow_u_13 * pow_v_9  
        + p_p->c1410z*14.0*pow_u_13 * pow_v_10  
        + p_p->c1411z*14.0*pow_u_13 * pow_v_11  
        + p_p->c1412z*14.0*pow_u_13 * pow_v_12  
        + p_p->c1413z*14.0*pow_u_13 * pow_v_13  
        + p_p->c1414z*14.0*pow_u_13 * pow_v_14  
        + p_p->c1415z*14.0*pow_u_13 * pow_v_15  
        + p_p->c1416z*14.0*pow_u_13 * pow_v_16  
        + p_p->c1417z*14.0*pow_u_13 * pow_v_17  
        + p_p->c1418z*14.0*pow_u_13 * pow_v_18  
        + p_p->c1419z*14.0*pow_u_13 * pow_v_19  
        + p_p->c1420z*14.0*pow_u_13 * pow_v_20  
        + p_p->c1421z*14.0*pow_u_13 * pow_v_21  ;
u_z=u_z + p_p->c1500z*15.0*pow_u_14 * pow_v_0  
        + p_p->c1501z*15.0*pow_u_14 * pow_v_1  
        + p_p->c1502z*15.0*pow_u_14 * pow_v_2  
        + p_p->c1503z*15.0*pow_u_14 * pow_v_3  
        + p_p->c1504z*15.0*pow_u_14 * pow_v_4  
        + p_p->c1505z*15.0*pow_u_14 * pow_v_5  
        + p_p->c1506z*15.0*pow_u_14 * pow_v_6  
        + p_p->c1507z*15.0*pow_u_14 * pow_v_7  
        + p_p->c1508z*15.0*pow_u_14 * pow_v_8  
        + p_p->c1509z*15.0*pow_u_14 * pow_v_9  
        + p_p->c1510z*15.0*pow_u_14 * pow_v_10  
        + p_p->c1511z*15.0*pow_u_14 * pow_v_11  
        + p_p->c1512z*15.0*pow_u_14 * pow_v_12  
        + p_p->c1513z*15.0*pow_u_14 * pow_v_13  
        + p_p->c1514z*15.0*pow_u_14 * pow_v_14  
        + p_p->c1515z*15.0*pow_u_14 * pow_v_15  
        + p_p->c1516z*15.0*pow_u_14 * pow_v_16  
        + p_p->c1517z*15.0*pow_u_14 * pow_v_17  
        + p_p->c1518z*15.0*pow_u_14 * pow_v_18  
        + p_p->c1519z*15.0*pow_u_14 * pow_v_19  
        + p_p->c1520z*15.0*pow_u_14 * pow_v_20  
        + p_p->c1521z*15.0*pow_u_14 * pow_v_21  ;
u_z=u_z + p_p->c1600z*16.0*pow_u_15 * pow_v_0  
        + p_p->c1601z*16.0*pow_u_15 * pow_v_1  
        + p_p->c1602z*16.0*pow_u_15 * pow_v_2  
        + p_p->c1603z*16.0*pow_u_15 * pow_v_3  
        + p_p->c1604z*16.0*pow_u_15 * pow_v_4  
        + p_p->c1605z*16.0*pow_u_15 * pow_v_5  
        + p_p->c1606z*16.0*pow_u_15 * pow_v_6  
        + p_p->c1607z*16.0*pow_u_15 * pow_v_7  
        + p_p->c1608z*16.0*pow_u_15 * pow_v_8  
        + p_p->c1609z*16.0*pow_u_15 * pow_v_9  
        + p_p->c1610z*16.0*pow_u_15 * pow_v_10  
        + p_p->c1611z*16.0*pow_u_15 * pow_v_11  
        + p_p->c1612z*16.0*pow_u_15 * pow_v_12  
        + p_p->c1613z*16.0*pow_u_15 * pow_v_13  
        + p_p->c1614z*16.0*pow_u_15 * pow_v_14  
        + p_p->c1615z*16.0*pow_u_15 * pow_v_15  
        + p_p->c1616z*16.0*pow_u_15 * pow_v_16  
        + p_p->c1617z*16.0*pow_u_15 * pow_v_17  
        + p_p->c1618z*16.0*pow_u_15 * pow_v_18  
        + p_p->c1619z*16.0*pow_u_15 * pow_v_19  
        + p_p->c1620z*16.0*pow_u_15 * pow_v_20  
        + p_p->c1621z*16.0*pow_u_15 * pow_v_21  ;
u_z=u_z + p_p->c1700z*17.0*pow_u_16 * pow_v_0  
        + p_p->c1701z*17.0*pow_u_16 * pow_v_1  
        + p_p->c1702z*17.0*pow_u_16 * pow_v_2  
        + p_p->c1703z*17.0*pow_u_16 * pow_v_3  
        + p_p->c1704z*17.0*pow_u_16 * pow_v_4  
        + p_p->c1705z*17.0*pow_u_16 * pow_v_5  
        + p_p->c1706z*17.0*pow_u_16 * pow_v_6  
        + p_p->c1707z*17.0*pow_u_16 * pow_v_7  
        + p_p->c1708z*17.0*pow_u_16 * pow_v_8  
        + p_p->c1709z*17.0*pow_u_16 * pow_v_9  
        + p_p->c1710z*17.0*pow_u_16 * pow_v_10  
        + p_p->c1711z*17.0*pow_u_16 * pow_v_11  
        + p_p->c1712z*17.0*pow_u_16 * pow_v_12  
        + p_p->c1713z*17.0*pow_u_16 * pow_v_13  
        + p_p->c1714z*17.0*pow_u_16 * pow_v_14  
        + p_p->c1715z*17.0*pow_u_16 * pow_v_15  
        + p_p->c1716z*17.0*pow_u_16 * pow_v_16  
        + p_p->c1717z*17.0*pow_u_16 * pow_v_17  
        + p_p->c1718z*17.0*pow_u_16 * pow_v_18  
        + p_p->c1719z*17.0*pow_u_16 * pow_v_19  
        + p_p->c1720z*17.0*pow_u_16 * pow_v_20  
        + p_p->c1721z*17.0*pow_u_16 * pow_v_21  ;
u_z=u_z + p_p->c1800z*18.0*pow_u_17 * pow_v_0  
        + p_p->c1801z*18.0*pow_u_17 * pow_v_1  
        + p_p->c1802z*18.0*pow_u_17 * pow_v_2  
        + p_p->c1803z*18.0*pow_u_17 * pow_v_3  
        + p_p->c1804z*18.0*pow_u_17 * pow_v_4  
        + p_p->c1805z*18.0*pow_u_17 * pow_v_5  
        + p_p->c1806z*18.0*pow_u_17 * pow_v_6  
        + p_p->c1807z*18.0*pow_u_17 * pow_v_7  
        + p_p->c1808z*18.0*pow_u_17 * pow_v_8  
        + p_p->c1809z*18.0*pow_u_17 * pow_v_9  
        + p_p->c1810z*18.0*pow_u_17 * pow_v_10  
        + p_p->c1811z*18.0*pow_u_17 * pow_v_11  
        + p_p->c1812z*18.0*pow_u_17 * pow_v_12  
        + p_p->c1813z*18.0*pow_u_17 * pow_v_13  
        + p_p->c1814z*18.0*pow_u_17 * pow_v_14  
        + p_p->c1815z*18.0*pow_u_17 * pow_v_15  
        + p_p->c1816z*18.0*pow_u_17 * pow_v_16  
        + p_p->c1817z*18.0*pow_u_17 * pow_v_17  
        + p_p->c1818z*18.0*pow_u_17 * pow_v_18  
        + p_p->c1819z*18.0*pow_u_17 * pow_v_19  
        + p_p->c1820z*18.0*pow_u_17 * pow_v_20  
        + p_p->c1821z*18.0*pow_u_17 * pow_v_21  ;
u_z=u_z + p_p->c1900z*19.0*pow_u_18 * pow_v_0  
        + p_p->c1901z*19.0*pow_u_18 * pow_v_1  
        + p_p->c1902z*19.0*pow_u_18 * pow_v_2  
        + p_p->c1903z*19.0*pow_u_18 * pow_v_3  
        + p_p->c1904z*19.0*pow_u_18 * pow_v_4  
        + p_p->c1905z*19.0*pow_u_18 * pow_v_5  
        + p_p->c1906z*19.0*pow_u_18 * pow_v_6  
        + p_p->c1907z*19.0*pow_u_18 * pow_v_7  
        + p_p->c1908z*19.0*pow_u_18 * pow_v_8  
        + p_p->c1909z*19.0*pow_u_18 * pow_v_9  
        + p_p->c1910z*19.0*pow_u_18 * pow_v_10  
        + p_p->c1911z*19.0*pow_u_18 * pow_v_11  
        + p_p->c1912z*19.0*pow_u_18 * pow_v_12  
        + p_p->c1913z*19.0*pow_u_18 * pow_v_13  
        + p_p->c1914z*19.0*pow_u_18 * pow_v_14  
        + p_p->c1915z*19.0*pow_u_18 * pow_v_15  
        + p_p->c1916z*19.0*pow_u_18 * pow_v_16  
        + p_p->c1917z*19.0*pow_u_18 * pow_v_17  
        + p_p->c1918z*19.0*pow_u_18 * pow_v_18  
        + p_p->c1919z*19.0*pow_u_18 * pow_v_19  
        + p_p->c1920z*19.0*pow_u_18 * pow_v_20  
        + p_p->c1921z*19.0*pow_u_18 * pow_v_21  ;
u_z=u_z + p_p->c2000z*20.0*pow_u_19 * pow_v_0  
        + p_p->c2001z*20.0*pow_u_19 * pow_v_1  
        + p_p->c2002z*20.0*pow_u_19 * pow_v_2  
        + p_p->c2003z*20.0*pow_u_19 * pow_v_3  
        + p_p->c2004z*20.0*pow_u_19 * pow_v_4  
        + p_p->c2005z*20.0*pow_u_19 * pow_v_5  
        + p_p->c2006z*20.0*pow_u_19 * pow_v_6  
        + p_p->c2007z*20.0*pow_u_19 * pow_v_7  
        + p_p->c2008z*20.0*pow_u_19 * pow_v_8  
        + p_p->c2009z*20.0*pow_u_19 * pow_v_9  
        + p_p->c2010z*20.0*pow_u_19 * pow_v_10  
        + p_p->c2011z*20.0*pow_u_19 * pow_v_11  
        + p_p->c2012z*20.0*pow_u_19 * pow_v_12  
        + p_p->c2013z*20.0*pow_u_19 * pow_v_13  
        + p_p->c2014z*20.0*pow_u_19 * pow_v_14  
        + p_p->c2015z*20.0*pow_u_19 * pow_v_15  
        + p_p->c2016z*20.0*pow_u_19 * pow_v_16  
        + p_p->c2017z*20.0*pow_u_19 * pow_v_17  
        + p_p->c2018z*20.0*pow_u_19 * pow_v_18  
        + p_p->c2019z*20.0*pow_u_19 * pow_v_19  
        + p_p->c2020z*20.0*pow_u_19 * pow_v_20  
        + p_p->c2021z*20.0*pow_u_19 * pow_v_21  ;
u_z=u_z + p_p->c2100z*21.0*pow_u_20 * pow_v_0  
        + p_p->c2101z*21.0*pow_u_20 * pow_v_1  
        + p_p->c2102z*21.0*pow_u_20 * pow_v_2  
        + p_p->c2103z*21.0*pow_u_20 * pow_v_3  
        + p_p->c2104z*21.0*pow_u_20 * pow_v_4  
        + p_p->c2105z*21.0*pow_u_20 * pow_v_5  
        + p_p->c2106z*21.0*pow_u_20 * pow_v_6  
        + p_p->c2107z*21.0*pow_u_20 * pow_v_7  
        + p_p->c2108z*21.0*pow_u_20 * pow_v_8  
        + p_p->c2109z*21.0*pow_u_20 * pow_v_9  
        + p_p->c2110z*21.0*pow_u_20 * pow_v_10  
        + p_p->c2111z*21.0*pow_u_20 * pow_v_11  
        + p_p->c2112z*21.0*pow_u_20 * pow_v_12  
        + p_p->c2113z*21.0*pow_u_20 * pow_v_13  
        + p_p->c2114z*21.0*pow_u_20 * pow_v_14  
        + p_p->c2115z*21.0*pow_u_20 * pow_v_15  
        + p_p->c2116z*21.0*pow_u_20 * pow_v_16  
        + p_p->c2117z*21.0*pow_u_20 * pow_v_17  
        + p_p->c2118z*21.0*pow_u_20 * pow_v_18  
        + p_p->c2119z*21.0*pow_u_20 * pow_v_19  
        + p_p->c2120z*21.0*pow_u_20 * pow_v_20  
        + p_p->c2121z*21.0*pow_u_20 * pow_v_21  ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_drdu  Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ** c_drdv *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates derivatives dr/dv for GMPATP21           */

   static short c_drdv  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP21  *p_p;       /* Patch GMPATP21                    (ptr) */
   DBfloat    u_l;       /* Patch (local) U parameter value         */
   DBfloat    v_l;       /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Derivatives v_x, v_y, v_z                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_drdv  p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l );
}
#endif


v_x     = p_p->c0000x * pow_u_0  *         0.0        
        + p_p->c0001x * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002x * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003x * pow_u_0  * 3.0*pow_v_2
        + p_p->c0004x * pow_u_0  * 4.0*pow_v_3
        + p_p->c0005x * pow_u_0  * 5.0*pow_v_4
        + p_p->c0006x * pow_u_0  * 6.0*pow_v_5
        + p_p->c0007x * pow_u_0  * 7.0*pow_v_6
        + p_p->c0008x * pow_u_0  * 8.0*pow_v_7
        + p_p->c0009x * pow_u_0  * 9.0*pow_v_8
        + p_p->c0010x * pow_u_0  *10.0*pow_v_9
        + p_p->c0011x * pow_u_0  *11.0*pow_v_10
        + p_p->c0012x * pow_u_0  *12.0*pow_v_11
        + p_p->c0013x * pow_u_0  *13.0*pow_v_12
        + p_p->c0014x * pow_u_0  *14.0*pow_v_13
        + p_p->c0015x * pow_u_0  *15.0*pow_v_14
        + p_p->c0016x * pow_u_0  *16.0*pow_v_15
        + p_p->c0017x * pow_u_0  *17.0*pow_v_16
        + p_p->c0018x * pow_u_0  *18.0*pow_v_17
        + p_p->c0019x * pow_u_0  *19.0*pow_v_18
        + p_p->c0020x * pow_u_0  *20.0*pow_v_19
        + p_p->c0021x * pow_u_0  *21.0*pow_v_20;
v_x=v_x + p_p->c0100x * pow_u_1  *         0.0        
        + p_p->c0101x * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102x * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103x * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104x * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105x * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106x * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107x * pow_u_1  * 7.0*pow_v_6
        + p_p->c0108x * pow_u_1  * 8.0*pow_v_7
        + p_p->c0109x * pow_u_1  * 9.0*pow_v_8
        + p_p->c0110x * pow_u_1  *10.0*pow_v_9
        + p_p->c0111x * pow_u_1  *11.0*pow_v_10
        + p_p->c0112x * pow_u_1  *12.0*pow_v_11
        + p_p->c0113x * pow_u_1  *13.0*pow_v_12
        + p_p->c0114x * pow_u_1  *14.0*pow_v_13
        + p_p->c0115x * pow_u_1  *15.0*pow_v_14
        + p_p->c0116x * pow_u_1  *16.0*pow_v_15
        + p_p->c0117x * pow_u_1  *17.0*pow_v_16
        + p_p->c0118x * pow_u_1  *18.0*pow_v_17
        + p_p->c0119x * pow_u_1  *19.0*pow_v_18
        + p_p->c0120x * pow_u_1  *20.0*pow_v_19
        + p_p->c0121x * pow_u_1  *21.0*pow_v_20;
v_x=v_x + p_p->c0200x * pow_u_2  *         0.0        
        + p_p->c0201x * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202x * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203x * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204x * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205x * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206x * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207x * pow_u_2  * 7.0*pow_v_6
        + p_p->c0208x * pow_u_2  * 8.0*pow_v_7
        + p_p->c0209x * pow_u_2  * 9.0*pow_v_8
        + p_p->c0210x * pow_u_2  *10.0*pow_v_9
        + p_p->c0211x * pow_u_2  *11.0*pow_v_10
        + p_p->c0212x * pow_u_2  *12.0*pow_v_11
        + p_p->c0213x * pow_u_2  *13.0*pow_v_12
        + p_p->c0214x * pow_u_2  *14.0*pow_v_13
        + p_p->c0215x * pow_u_2  *15.0*pow_v_14
        + p_p->c0216x * pow_u_2  *16.0*pow_v_15
        + p_p->c0217x * pow_u_2  *17.0*pow_v_16
        + p_p->c0218x * pow_u_2  *18.0*pow_v_17
        + p_p->c0219x * pow_u_2  *19.0*pow_v_18
        + p_p->c0220x * pow_u_2  *20.0*pow_v_19
        + p_p->c0221x * pow_u_2  *21.0*pow_v_20;
v_x=v_x + p_p->c0300x * pow_u_3  *         0.0        
        + p_p->c0301x * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302x * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303x * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304x * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305x * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306x * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307x * pow_u_3  * 7.0*pow_v_6
        + p_p->c0308x * pow_u_3  * 8.0*pow_v_7
        + p_p->c0309x * pow_u_3  * 9.0*pow_v_8
        + p_p->c0310x * pow_u_3  *10.0*pow_v_9
        + p_p->c0311x * pow_u_3  *11.0*pow_v_10
        + p_p->c0312x * pow_u_3  *12.0*pow_v_11
        + p_p->c0313x * pow_u_3  *13.0*pow_v_12
        + p_p->c0314x * pow_u_3  *14.0*pow_v_13
        + p_p->c0315x * pow_u_3  *15.0*pow_v_14
        + p_p->c0316x * pow_u_3  *16.0*pow_v_15
        + p_p->c0317x * pow_u_3  *17.0*pow_v_16
        + p_p->c0318x * pow_u_3  *18.0*pow_v_17
        + p_p->c0319x * pow_u_3  *19.0*pow_v_18
        + p_p->c0320x * pow_u_3  *20.0*pow_v_19
        + p_p->c0321x * pow_u_3  *21.0*pow_v_20;
v_x=v_x + p_p->c0400x * pow_u_4  *         0.0        
        + p_p->c0401x * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402x * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403x * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404x * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405x * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406x * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407x * pow_u_4  * 7.0*pow_v_6
        + p_p->c0408x * pow_u_4  * 8.0*pow_v_7
        + p_p->c0409x * pow_u_4  * 9.0*pow_v_8
        + p_p->c0410x * pow_u_4  *10.0*pow_v_9
        + p_p->c0411x * pow_u_4  *11.0*pow_v_10
        + p_p->c0412x * pow_u_4  *12.0*pow_v_11
        + p_p->c0413x * pow_u_4  *13.0*pow_v_12
        + p_p->c0414x * pow_u_4  *14.0*pow_v_13
        + p_p->c0415x * pow_u_4  *15.0*pow_v_14
        + p_p->c0416x * pow_u_4  *16.0*pow_v_15
        + p_p->c0417x * pow_u_4  *17.0*pow_v_16
        + p_p->c0418x * pow_u_4  *18.0*pow_v_17
        + p_p->c0419x * pow_u_4  *19.0*pow_v_18
        + p_p->c0420x * pow_u_4  *20.0*pow_v_19
        + p_p->c0421x * pow_u_4  *21.0*pow_v_20;
v_x=v_x + p_p->c0500x * pow_u_5  *         0.0        
        + p_p->c0501x * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502x * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503x * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504x * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505x * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506x * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507x * pow_u_5  * 7.0*pow_v_6
        + p_p->c0508x * pow_u_5  * 8.0*pow_v_7
        + p_p->c0509x * pow_u_5  * 9.0*pow_v_8
        + p_p->c0510x * pow_u_5  *10.0*pow_v_9
        + p_p->c0511x * pow_u_5  *11.0*pow_v_10
        + p_p->c0512x * pow_u_5  *12.0*pow_v_11
        + p_p->c0513x * pow_u_5  *13.0*pow_v_12
        + p_p->c0514x * pow_u_5  *14.0*pow_v_13
        + p_p->c0515x * pow_u_5  *15.0*pow_v_14
        + p_p->c0516x * pow_u_5  *16.0*pow_v_15
        + p_p->c0517x * pow_u_5  *17.0*pow_v_16
        + p_p->c0518x * pow_u_5  *18.0*pow_v_17
        + p_p->c0519x * pow_u_5  *19.0*pow_v_18
        + p_p->c0520x * pow_u_5  *20.0*pow_v_19
        + p_p->c0521x * pow_u_5  *21.0*pow_v_20;
v_x=v_x + p_p->c0600x * pow_u_6  *         0.0        
        + p_p->c0601x * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602x * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603x * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604x * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605x * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606x * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607x * pow_u_6  * 7.0*pow_v_6
        + p_p->c0608x * pow_u_6  * 8.0*pow_v_7
        + p_p->c0609x * pow_u_6  * 9.0*pow_v_8
        + p_p->c0610x * pow_u_6  *10.0*pow_v_9
        + p_p->c0611x * pow_u_6  *11.0*pow_v_10
        + p_p->c0612x * pow_u_6  *12.0*pow_v_11
        + p_p->c0613x * pow_u_6  *13.0*pow_v_12
        + p_p->c0614x * pow_u_6  *14.0*pow_v_13
        + p_p->c0615x * pow_u_6  *15.0*pow_v_14
        + p_p->c0616x * pow_u_6  *16.0*pow_v_15
        + p_p->c0617x * pow_u_6  *17.0*pow_v_16
        + p_p->c0618x * pow_u_6  *18.0*pow_v_17
        + p_p->c0619x * pow_u_6  *19.0*pow_v_18
        + p_p->c0620x * pow_u_6  *20.0*pow_v_19
        + p_p->c0621x * pow_u_6  *21.0*pow_v_20;
v_x=v_x + p_p->c0700x * pow_u_7  *         0.0        
        + p_p->c0701x * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702x * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703x * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704x * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705x * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706x * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707x * pow_u_7  * 7.0*pow_v_6
        + p_p->c0708x * pow_u_7  * 8.0*pow_v_7
        + p_p->c0709x * pow_u_7  * 9.0*pow_v_8
        + p_p->c0710x * pow_u_7  *10.0*pow_v_9
        + p_p->c0711x * pow_u_7  *11.0*pow_v_10
        + p_p->c0712x * pow_u_7  *12.0*pow_v_11
        + p_p->c0713x * pow_u_7  *13.0*pow_v_12
        + p_p->c0714x * pow_u_7  *14.0*pow_v_13
        + p_p->c0715x * pow_u_7  *15.0*pow_v_14
        + p_p->c0716x * pow_u_7  *16.0*pow_v_15
        + p_p->c0717x * pow_u_7  *17.0*pow_v_16
        + p_p->c0718x * pow_u_7  *18.0*pow_v_17
        + p_p->c0719x * pow_u_7  *19.0*pow_v_18
        + p_p->c0720x * pow_u_7  *20.0*pow_v_19
        + p_p->c0721x * pow_u_7  *21.0*pow_v_20;
v_x=v_x + p_p->c0800x * pow_u_8  *         0.0        
        + p_p->c0801x * pow_u_8  * 1.0*pow_v_0
        + p_p->c0802x * pow_u_8  * 2.0*pow_v_1
        + p_p->c0803x * pow_u_8  * 3.0*pow_v_2
        + p_p->c0804x * pow_u_8  * 4.0*pow_v_3
        + p_p->c0805x * pow_u_8  * 5.0*pow_v_4
        + p_p->c0806x * pow_u_8  * 6.0*pow_v_5
        + p_p->c0807x * pow_u_8  * 7.0*pow_v_6
        + p_p->c0808x * pow_u_8  * 8.0*pow_v_7
        + p_p->c0809x * pow_u_8  * 9.0*pow_v_8
        + p_p->c0810x * pow_u_8  *10.0*pow_v_9
        + p_p->c0811x * pow_u_8  *11.0*pow_v_10
        + p_p->c0812x * pow_u_8  *12.0*pow_v_11
        + p_p->c0813x * pow_u_8  *13.0*pow_v_12
        + p_p->c0814x * pow_u_8  *14.0*pow_v_13
        + p_p->c0815x * pow_u_8  *15.0*pow_v_14
        + p_p->c0816x * pow_u_8  *16.0*pow_v_15
        + p_p->c0817x * pow_u_8  *17.0*pow_v_16
        + p_p->c0818x * pow_u_8  *18.0*pow_v_17
        + p_p->c0819x * pow_u_8  *19.0*pow_v_18
        + p_p->c0820x * pow_u_8  *20.0*pow_v_19
        + p_p->c0821x * pow_u_8  *21.0*pow_v_20;
v_x=v_x + p_p->c0900x * pow_u_9  *         0.0        
        + p_p->c0901x * pow_u_9  * 1.0*pow_v_0
        + p_p->c0902x * pow_u_9  * 2.0*pow_v_1
        + p_p->c0903x * pow_u_9  * 3.0*pow_v_2
        + p_p->c0904x * pow_u_9  * 4.0*pow_v_3
        + p_p->c0905x * pow_u_9  * 5.0*pow_v_4
        + p_p->c0906x * pow_u_9  * 6.0*pow_v_5
        + p_p->c0907x * pow_u_9  * 7.0*pow_v_6
        + p_p->c0908x * pow_u_9  * 8.0*pow_v_7
        + p_p->c0909x * pow_u_9  * 9.0*pow_v_8
        + p_p->c0910x * pow_u_9  *10.0*pow_v_9
        + p_p->c0911x * pow_u_9  *11.0*pow_v_10
        + p_p->c0912x * pow_u_9  *12.0*pow_v_11
        + p_p->c0913x * pow_u_9  *13.0*pow_v_12
        + p_p->c0914x * pow_u_9  *14.0*pow_v_13
        + p_p->c0915x * pow_u_9  *15.0*pow_v_14
        + p_p->c0916x * pow_u_9  *16.0*pow_v_15
        + p_p->c0917x * pow_u_9  *17.0*pow_v_16
        + p_p->c0918x * pow_u_9  *18.0*pow_v_17
        + p_p->c0919x * pow_u_9  *19.0*pow_v_18
        + p_p->c0920x * pow_u_9  *20.0*pow_v_19
        + p_p->c0921x * pow_u_9  *21.0*pow_v_20;
v_x=v_x + p_p->c1000x * pow_u_10  *         0.0        
        + p_p->c1001x * pow_u_10  * 1.0*pow_v_0
        + p_p->c1002x * pow_u_10  * 2.0*pow_v_1
        + p_p->c1003x * pow_u_10  * 3.0*pow_v_2
        + p_p->c1004x * pow_u_10  * 4.0*pow_v_3
        + p_p->c1005x * pow_u_10  * 5.0*pow_v_4
        + p_p->c1006x * pow_u_10  * 6.0*pow_v_5
        + p_p->c1007x * pow_u_10  * 7.0*pow_v_6
        + p_p->c1008x * pow_u_10  * 8.0*pow_v_7
        + p_p->c1009x * pow_u_10  * 9.0*pow_v_8
        + p_p->c1010x * pow_u_10  *10.0*pow_v_9
        + p_p->c1011x * pow_u_10  *11.0*pow_v_10
        + p_p->c1012x * pow_u_10  *12.0*pow_v_11
        + p_p->c1013x * pow_u_10  *13.0*pow_v_12
        + p_p->c1014x * pow_u_10  *14.0*pow_v_13
        + p_p->c1015x * pow_u_10  *15.0*pow_v_14
        + p_p->c1016x * pow_u_10  *16.0*pow_v_15
        + p_p->c1017x * pow_u_10  *17.0*pow_v_16
        + p_p->c1018x * pow_u_10  *18.0*pow_v_17
        + p_p->c1019x * pow_u_10  *19.0*pow_v_18
        + p_p->c1020x * pow_u_10  *20.0*pow_v_19
        + p_p->c1021x * pow_u_10  *21.0*pow_v_20;
v_x=v_x + p_p->c1100x * pow_u_11  *         0.0        
        + p_p->c1101x * pow_u_11  * 1.0*pow_v_0
        + p_p->c1102x * pow_u_11  * 2.0*pow_v_1
        + p_p->c1103x * pow_u_11  * 3.0*pow_v_2
        + p_p->c1104x * pow_u_11  * 4.0*pow_v_3
        + p_p->c1105x * pow_u_11  * 5.0*pow_v_4
        + p_p->c1106x * pow_u_11  * 6.0*pow_v_5
        + p_p->c1107x * pow_u_11  * 7.0*pow_v_6
        + p_p->c1108x * pow_u_11  * 8.0*pow_v_7
        + p_p->c1109x * pow_u_11  * 9.0*pow_v_8
        + p_p->c1110x * pow_u_11  *10.0*pow_v_9
        + p_p->c1111x * pow_u_11  *11.0*pow_v_10
        + p_p->c1112x * pow_u_11  *12.0*pow_v_11
        + p_p->c1113x * pow_u_11  *13.0*pow_v_12
        + p_p->c1114x * pow_u_11  *14.0*pow_v_13
        + p_p->c1115x * pow_u_11  *15.0*pow_v_14
        + p_p->c1116x * pow_u_11  *16.0*pow_v_15
        + p_p->c1117x * pow_u_11  *17.0*pow_v_16
        + p_p->c1118x * pow_u_11  *18.0*pow_v_17
        + p_p->c1119x * pow_u_11  *19.0*pow_v_18
        + p_p->c1120x * pow_u_11  *20.0*pow_v_19
        + p_p->c1121x * pow_u_11  *21.0*pow_v_20;
v_x=v_x + p_p->c1200x * pow_u_12  *         0.0        
        + p_p->c1201x * pow_u_12  * 1.0*pow_v_0
        + p_p->c1202x * pow_u_12  * 2.0*pow_v_1
        + p_p->c1203x * pow_u_12  * 3.0*pow_v_2
        + p_p->c1204x * pow_u_12  * 4.0*pow_v_3
        + p_p->c1205x * pow_u_12  * 5.0*pow_v_4
        + p_p->c1206x * pow_u_12  * 6.0*pow_v_5
        + p_p->c1207x * pow_u_12  * 7.0*pow_v_6
        + p_p->c1208x * pow_u_12  * 8.0*pow_v_7
        + p_p->c1209x * pow_u_12  * 9.0*pow_v_8
        + p_p->c1210x * pow_u_12  *10.0*pow_v_9
        + p_p->c1211x * pow_u_12  *11.0*pow_v_10
        + p_p->c1212x * pow_u_12  *12.0*pow_v_11
        + p_p->c1213x * pow_u_12  *13.0*pow_v_12
        + p_p->c1214x * pow_u_12  *14.0*pow_v_13
        + p_p->c1215x * pow_u_12  *15.0*pow_v_14
        + p_p->c1216x * pow_u_12  *16.0*pow_v_15
        + p_p->c1217x * pow_u_12  *17.0*pow_v_16
        + p_p->c1218x * pow_u_12  *18.0*pow_v_17
        + p_p->c1219x * pow_u_12  *19.0*pow_v_18
        + p_p->c1220x * pow_u_12  *20.0*pow_v_19
        + p_p->c1221x * pow_u_12  *21.0*pow_v_20;
v_x=v_x + p_p->c1300x * pow_u_13  *         0.0        
        + p_p->c1301x * pow_u_13  * 1.0*pow_v_0
        + p_p->c1302x * pow_u_13  * 2.0*pow_v_1
        + p_p->c1303x * pow_u_13  * 3.0*pow_v_2
        + p_p->c1304x * pow_u_13  * 4.0*pow_v_3
        + p_p->c1305x * pow_u_13  * 5.0*pow_v_4
        + p_p->c1306x * pow_u_13  * 6.0*pow_v_5
        + p_p->c1307x * pow_u_13  * 7.0*pow_v_6
        + p_p->c1308x * pow_u_13  * 8.0*pow_v_7
        + p_p->c1309x * pow_u_13  * 9.0*pow_v_8
        + p_p->c1310x * pow_u_13  *10.0*pow_v_9
        + p_p->c1311x * pow_u_13  *11.0*pow_v_10
        + p_p->c1312x * pow_u_13  *12.0*pow_v_11
        + p_p->c1313x * pow_u_13  *13.0*pow_v_12
        + p_p->c1314x * pow_u_13  *14.0*pow_v_13
        + p_p->c1315x * pow_u_13  *15.0*pow_v_14
        + p_p->c1316x * pow_u_13  *16.0*pow_v_15
        + p_p->c1317x * pow_u_13  *17.0*pow_v_16
        + p_p->c1318x * pow_u_13  *18.0*pow_v_17
        + p_p->c1319x * pow_u_13  *19.0*pow_v_18
        + p_p->c1320x * pow_u_13  *20.0*pow_v_19
        + p_p->c1321x * pow_u_13  *21.0*pow_v_20;
v_x=v_x + p_p->c1400x * pow_u_14  *         0.0        
        + p_p->c1401x * pow_u_14  * 1.0*pow_v_0
        + p_p->c1402x * pow_u_14  * 2.0*pow_v_1
        + p_p->c1403x * pow_u_14  * 3.0*pow_v_2
        + p_p->c1404x * pow_u_14  * 4.0*pow_v_3
        + p_p->c1405x * pow_u_14  * 5.0*pow_v_4
        + p_p->c1406x * pow_u_14  * 6.0*pow_v_5
        + p_p->c1407x * pow_u_14  * 7.0*pow_v_6
        + p_p->c1408x * pow_u_14  * 8.0*pow_v_7
        + p_p->c1409x * pow_u_14  * 9.0*pow_v_8
        + p_p->c1410x * pow_u_14  *10.0*pow_v_9
        + p_p->c1411x * pow_u_14  *11.0*pow_v_10
        + p_p->c1412x * pow_u_14  *12.0*pow_v_11
        + p_p->c1413x * pow_u_14  *13.0*pow_v_12
        + p_p->c1414x * pow_u_14  *14.0*pow_v_13
        + p_p->c1415x * pow_u_14  *15.0*pow_v_14
        + p_p->c1416x * pow_u_14  *16.0*pow_v_15
        + p_p->c1417x * pow_u_14  *17.0*pow_v_16
        + p_p->c1418x * pow_u_14  *18.0*pow_v_17
        + p_p->c1419x * pow_u_14  *19.0*pow_v_18
        + p_p->c1420x * pow_u_14  *20.0*pow_v_19
        + p_p->c1421x * pow_u_14  *21.0*pow_v_20;
v_x=v_x + p_p->c1500x * pow_u_15  *         0.0        
        + p_p->c1501x * pow_u_15  * 1.0*pow_v_0
        + p_p->c1502x * pow_u_15  * 2.0*pow_v_1
        + p_p->c1503x * pow_u_15  * 3.0*pow_v_2
        + p_p->c1504x * pow_u_15  * 4.0*pow_v_3
        + p_p->c1505x * pow_u_15  * 5.0*pow_v_4
        + p_p->c1506x * pow_u_15  * 6.0*pow_v_5
        + p_p->c1507x * pow_u_15  * 7.0*pow_v_6
        + p_p->c1508x * pow_u_15  * 8.0*pow_v_7
        + p_p->c1509x * pow_u_15  * 9.0*pow_v_8
        + p_p->c1510x * pow_u_15  *10.0*pow_v_9
        + p_p->c1511x * pow_u_15  *11.0*pow_v_10
        + p_p->c1512x * pow_u_15  *12.0*pow_v_11
        + p_p->c1513x * pow_u_15  *13.0*pow_v_12
        + p_p->c1514x * pow_u_15  *14.0*pow_v_13
        + p_p->c1515x * pow_u_15  *15.0*pow_v_14
        + p_p->c1516x * pow_u_15  *16.0*pow_v_15
        + p_p->c1517x * pow_u_15  *17.0*pow_v_16
        + p_p->c1518x * pow_u_15  *18.0*pow_v_17
        + p_p->c1519x * pow_u_15  *19.0*pow_v_18
        + p_p->c1520x * pow_u_15  *20.0*pow_v_19
        + p_p->c1521x * pow_u_15  *21.0*pow_v_20;
v_x=v_x + p_p->c1600x * pow_u_16  *         0.0        
        + p_p->c1601x * pow_u_16  * 1.0*pow_v_0
        + p_p->c1602x * pow_u_16  * 2.0*pow_v_1
        + p_p->c1603x * pow_u_16  * 3.0*pow_v_2
        + p_p->c1604x * pow_u_16  * 4.0*pow_v_3
        + p_p->c1605x * pow_u_16  * 5.0*pow_v_4
        + p_p->c1606x * pow_u_16  * 6.0*pow_v_5
        + p_p->c1607x * pow_u_16  * 7.0*pow_v_6
        + p_p->c1608x * pow_u_16  * 8.0*pow_v_7
        + p_p->c1609x * pow_u_16  * 9.0*pow_v_8
        + p_p->c1610x * pow_u_16  *10.0*pow_v_9
        + p_p->c1611x * pow_u_16  *11.0*pow_v_10
        + p_p->c1612x * pow_u_16  *12.0*pow_v_11
        + p_p->c1613x * pow_u_16  *13.0*pow_v_12
        + p_p->c1614x * pow_u_16  *14.0*pow_v_13
        + p_p->c1615x * pow_u_16  *15.0*pow_v_14
        + p_p->c1616x * pow_u_16  *16.0*pow_v_15
        + p_p->c1617x * pow_u_16  *17.0*pow_v_16
        + p_p->c1618x * pow_u_16  *18.0*pow_v_17
        + p_p->c1619x * pow_u_16  *19.0*pow_v_18
        + p_p->c1620x * pow_u_16  *20.0*pow_v_19
        + p_p->c1621x * pow_u_16  *21.0*pow_v_20;
v_x=v_x + p_p->c1700x * pow_u_17  *         0.0        
        + p_p->c1701x * pow_u_17  * 1.0*pow_v_0
        + p_p->c1702x * pow_u_17  * 2.0*pow_v_1
        + p_p->c1703x * pow_u_17  * 3.0*pow_v_2
        + p_p->c1704x * pow_u_17  * 4.0*pow_v_3
        + p_p->c1705x * pow_u_17  * 5.0*pow_v_4
        + p_p->c1706x * pow_u_17  * 6.0*pow_v_5
        + p_p->c1707x * pow_u_17  * 7.0*pow_v_6
        + p_p->c1708x * pow_u_17  * 8.0*pow_v_7
        + p_p->c1709x * pow_u_17  * 9.0*pow_v_8
        + p_p->c1710x * pow_u_17  *10.0*pow_v_9
        + p_p->c1711x * pow_u_17  *11.0*pow_v_10
        + p_p->c1712x * pow_u_17  *12.0*pow_v_11
        + p_p->c1713x * pow_u_17  *13.0*pow_v_12
        + p_p->c1714x * pow_u_17  *14.0*pow_v_13
        + p_p->c1715x * pow_u_17  *15.0*pow_v_14
        + p_p->c1716x * pow_u_17  *16.0*pow_v_15
        + p_p->c1717x * pow_u_17  *17.0*pow_v_16
        + p_p->c1718x * pow_u_17  *18.0*pow_v_17
        + p_p->c1719x * pow_u_17  *19.0*pow_v_18
        + p_p->c1720x * pow_u_17  *20.0*pow_v_19
        + p_p->c1721x * pow_u_17  *21.0*pow_v_20;
v_x=v_x + p_p->c1800x * pow_u_18  *         0.0        
        + p_p->c1801x * pow_u_18  * 1.0*pow_v_0
        + p_p->c1802x * pow_u_18  * 2.0*pow_v_1
        + p_p->c1803x * pow_u_18  * 3.0*pow_v_2
        + p_p->c1804x * pow_u_18  * 4.0*pow_v_3
        + p_p->c1805x * pow_u_18  * 5.0*pow_v_4
        + p_p->c1806x * pow_u_18  * 6.0*pow_v_5
        + p_p->c1807x * pow_u_18  * 7.0*pow_v_6
        + p_p->c1808x * pow_u_18  * 8.0*pow_v_7
        + p_p->c1809x * pow_u_18  * 9.0*pow_v_8
        + p_p->c1810x * pow_u_18  *10.0*pow_v_9
        + p_p->c1811x * pow_u_18  *11.0*pow_v_10
        + p_p->c1812x * pow_u_18  *12.0*pow_v_11
        + p_p->c1813x * pow_u_18  *13.0*pow_v_12
        + p_p->c1814x * pow_u_18  *14.0*pow_v_13
        + p_p->c1815x * pow_u_18  *15.0*pow_v_14
        + p_p->c1816x * pow_u_18  *16.0*pow_v_15
        + p_p->c1817x * pow_u_18  *17.0*pow_v_16
        + p_p->c1818x * pow_u_18  *18.0*pow_v_17
        + p_p->c1819x * pow_u_18  *19.0*pow_v_18
        + p_p->c1820x * pow_u_18  *20.0*pow_v_19
        + p_p->c1821x * pow_u_18  *21.0*pow_v_20;
v_x=v_x + p_p->c1900x * pow_u_19  *         0.0        
        + p_p->c1901x * pow_u_19  * 1.0*pow_v_0
        + p_p->c1902x * pow_u_19  * 2.0*pow_v_1
        + p_p->c1903x * pow_u_19  * 3.0*pow_v_2
        + p_p->c1904x * pow_u_19  * 4.0*pow_v_3
        + p_p->c1905x * pow_u_19  * 5.0*pow_v_4
        + p_p->c1906x * pow_u_19  * 6.0*pow_v_5
        + p_p->c1907x * pow_u_19  * 7.0*pow_v_6
        + p_p->c1908x * pow_u_19  * 8.0*pow_v_7
        + p_p->c1909x * pow_u_19  * 9.0*pow_v_8
        + p_p->c1910x * pow_u_19  *10.0*pow_v_9
        + p_p->c1911x * pow_u_19  *11.0*pow_v_10
        + p_p->c1912x * pow_u_19  *12.0*pow_v_11
        + p_p->c1913x * pow_u_19  *13.0*pow_v_12
        + p_p->c1914x * pow_u_19  *14.0*pow_v_13
        + p_p->c1915x * pow_u_19  *15.0*pow_v_14
        + p_p->c1916x * pow_u_19  *16.0*pow_v_15
        + p_p->c1917x * pow_u_19  *17.0*pow_v_16
        + p_p->c1918x * pow_u_19  *18.0*pow_v_17
        + p_p->c1919x * pow_u_19  *19.0*pow_v_18
        + p_p->c1920x * pow_u_19  *20.0*pow_v_19
        + p_p->c1921x * pow_u_19  *21.0*pow_v_20;
v_x=v_x + p_p->c2000x * pow_u_20  *         0.0        
        + p_p->c2001x * pow_u_20  * 1.0*pow_v_0
        + p_p->c2002x * pow_u_20  * 2.0*pow_v_1
        + p_p->c2003x * pow_u_20  * 3.0*pow_v_2
        + p_p->c2004x * pow_u_20  * 4.0*pow_v_3
        + p_p->c2005x * pow_u_20  * 5.0*pow_v_4
        + p_p->c2006x * pow_u_20  * 6.0*pow_v_5
        + p_p->c2007x * pow_u_20  * 7.0*pow_v_6
        + p_p->c2008x * pow_u_20  * 8.0*pow_v_7
        + p_p->c2009x * pow_u_20  * 9.0*pow_v_8
        + p_p->c2010x * pow_u_20  *10.0*pow_v_9
        + p_p->c2011x * pow_u_20  *11.0*pow_v_10
        + p_p->c2012x * pow_u_20  *12.0*pow_v_11
        + p_p->c2013x * pow_u_20  *13.0*pow_v_12
        + p_p->c2014x * pow_u_20  *14.0*pow_v_13
        + p_p->c2015x * pow_u_20  *15.0*pow_v_14
        + p_p->c2016x * pow_u_20  *16.0*pow_v_15
        + p_p->c2017x * pow_u_20  *17.0*pow_v_16
        + p_p->c2018x * pow_u_20  *18.0*pow_v_17
        + p_p->c2019x * pow_u_20  *19.0*pow_v_18
        + p_p->c2020x * pow_u_20  *20.0*pow_v_19
        + p_p->c2021x * pow_u_20  *21.0*pow_v_20;
v_x=v_x + p_p->c2100x * pow_u_21  *         0.0        
        + p_p->c2101x * pow_u_21  * 1.0*pow_v_0
        + p_p->c2102x * pow_u_21  * 2.0*pow_v_1
        + p_p->c2103x * pow_u_21  * 3.0*pow_v_2
        + p_p->c2104x * pow_u_21  * 4.0*pow_v_3
        + p_p->c2105x * pow_u_21  * 5.0*pow_v_4
        + p_p->c2106x * pow_u_21  * 6.0*pow_v_5
        + p_p->c2107x * pow_u_21  * 7.0*pow_v_6
        + p_p->c2108x * pow_u_21  * 8.0*pow_v_7
        + p_p->c2109x * pow_u_21  * 9.0*pow_v_8
        + p_p->c2110x * pow_u_21  *10.0*pow_v_9
        + p_p->c2111x * pow_u_21  *11.0*pow_v_10
        + p_p->c2112x * pow_u_21  *12.0*pow_v_11
        + p_p->c2113x * pow_u_21  *13.0*pow_v_12
        + p_p->c2114x * pow_u_21  *14.0*pow_v_13
        + p_p->c2115x * pow_u_21  *15.0*pow_v_14
        + p_p->c2116x * pow_u_21  *16.0*pow_v_15
        + p_p->c2117x * pow_u_21  *17.0*pow_v_16
        + p_p->c2118x * pow_u_21  *18.0*pow_v_17
        + p_p->c2119x * pow_u_21  *19.0*pow_v_18
        + p_p->c2120x * pow_u_21  *20.0*pow_v_19
        + p_p->c2121x * pow_u_21  *21.0*pow_v_20;

v_y     = p_p->c0000y * pow_u_0  *         0.0        
        + p_p->c0001y * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002y * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003y * pow_u_0  * 3.0*pow_v_2
        + p_p->c0004y * pow_u_0  * 4.0*pow_v_3
        + p_p->c0005y * pow_u_0  * 5.0*pow_v_4
        + p_p->c0006y * pow_u_0  * 6.0*pow_v_5
        + p_p->c0007y * pow_u_0  * 7.0*pow_v_6
        + p_p->c0008y * pow_u_0  * 8.0*pow_v_7
        + p_p->c0009y * pow_u_0  * 9.0*pow_v_8
        + p_p->c0010y * pow_u_0  *10.0*pow_v_9
        + p_p->c0011y * pow_u_0  *11.0*pow_v_10
        + p_p->c0012y * pow_u_0  *12.0*pow_v_11
        + p_p->c0013y * pow_u_0  *13.0*pow_v_12
        + p_p->c0014y * pow_u_0  *14.0*pow_v_13
        + p_p->c0015y * pow_u_0  *15.0*pow_v_14
        + p_p->c0016y * pow_u_0  *16.0*pow_v_15
        + p_p->c0017y * pow_u_0  *17.0*pow_v_16
        + p_p->c0018y * pow_u_0  *18.0*pow_v_17
        + p_p->c0019y * pow_u_0  *19.0*pow_v_18
        + p_p->c0020y * pow_u_0  *20.0*pow_v_19
        + p_p->c0021y * pow_u_0  *21.0*pow_v_20;
v_y=v_y + p_p->c0100y * pow_u_1  *         0.0        
        + p_p->c0101y * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102y * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103y * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104y * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105y * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106y * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107y * pow_u_1  * 7.0*pow_v_6
        + p_p->c0108y * pow_u_1  * 8.0*pow_v_7
        + p_p->c0109y * pow_u_1  * 9.0*pow_v_8
        + p_p->c0110y * pow_u_1  *10.0*pow_v_9
        + p_p->c0111y * pow_u_1  *11.0*pow_v_10
        + p_p->c0112y * pow_u_1  *12.0*pow_v_11
        + p_p->c0113y * pow_u_1  *13.0*pow_v_12
        + p_p->c0114y * pow_u_1  *14.0*pow_v_13
        + p_p->c0115y * pow_u_1  *15.0*pow_v_14
        + p_p->c0116y * pow_u_1  *16.0*pow_v_15
        + p_p->c0117y * pow_u_1  *17.0*pow_v_16
        + p_p->c0118y * pow_u_1  *18.0*pow_v_17
        + p_p->c0119y * pow_u_1  *19.0*pow_v_18
        + p_p->c0120y * pow_u_1  *20.0*pow_v_19
        + p_p->c0121y * pow_u_1  *21.0*pow_v_20;
v_y=v_y + p_p->c0200y * pow_u_2  *         0.0        
        + p_p->c0201y * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202y * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203y * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204y * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205y * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206y * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207y * pow_u_2  * 7.0*pow_v_6
        + p_p->c0208y * pow_u_2  * 8.0*pow_v_7
        + p_p->c0209y * pow_u_2  * 9.0*pow_v_8
        + p_p->c0210y * pow_u_2  *10.0*pow_v_9
        + p_p->c0211y * pow_u_2  *11.0*pow_v_10
        + p_p->c0212y * pow_u_2  *12.0*pow_v_11
        + p_p->c0213y * pow_u_2  *13.0*pow_v_12
        + p_p->c0214y * pow_u_2  *14.0*pow_v_13
        + p_p->c0215y * pow_u_2  *15.0*pow_v_14
        + p_p->c0216y * pow_u_2  *16.0*pow_v_15
        + p_p->c0217y * pow_u_2  *17.0*pow_v_16
        + p_p->c0218y * pow_u_2  *18.0*pow_v_17
        + p_p->c0219y * pow_u_2  *19.0*pow_v_18
        + p_p->c0220y * pow_u_2  *20.0*pow_v_19
        + p_p->c0221y * pow_u_2  *21.0*pow_v_20;
v_y=v_y + p_p->c0300y * pow_u_3  *         0.0        
        + p_p->c0301y * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302y * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303y * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304y * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305y * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306y * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307y * pow_u_3  * 7.0*pow_v_6
        + p_p->c0308y * pow_u_3  * 8.0*pow_v_7
        + p_p->c0309y * pow_u_3  * 9.0*pow_v_8
        + p_p->c0310y * pow_u_3  *10.0*pow_v_9
        + p_p->c0311y * pow_u_3  *11.0*pow_v_10
        + p_p->c0312y * pow_u_3  *12.0*pow_v_11
        + p_p->c0313y * pow_u_3  *13.0*pow_v_12
        + p_p->c0314y * pow_u_3  *14.0*pow_v_13
        + p_p->c0315y * pow_u_3  *15.0*pow_v_14
        + p_p->c0316y * pow_u_3  *16.0*pow_v_15
        + p_p->c0317y * pow_u_3  *17.0*pow_v_16
        + p_p->c0318y * pow_u_3  *18.0*pow_v_17
        + p_p->c0319y * pow_u_3  *19.0*pow_v_18
        + p_p->c0320y * pow_u_3  *20.0*pow_v_19
        + p_p->c0321y * pow_u_3  *21.0*pow_v_20;
v_y=v_y + p_p->c0400y * pow_u_4  *         0.0        
        + p_p->c0401y * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402y * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403y * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404y * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405y * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406y * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407y * pow_u_4  * 7.0*pow_v_6
        + p_p->c0408y * pow_u_4  * 8.0*pow_v_7
        + p_p->c0409y * pow_u_4  * 9.0*pow_v_8
        + p_p->c0410y * pow_u_4  *10.0*pow_v_9
        + p_p->c0411y * pow_u_4  *11.0*pow_v_10
        + p_p->c0412y * pow_u_4  *12.0*pow_v_11
        + p_p->c0413y * pow_u_4  *13.0*pow_v_12
        + p_p->c0414y * pow_u_4  *14.0*pow_v_13
        + p_p->c0415y * pow_u_4  *15.0*pow_v_14
        + p_p->c0416y * pow_u_4  *16.0*pow_v_15
        + p_p->c0417y * pow_u_4  *17.0*pow_v_16
        + p_p->c0418y * pow_u_4  *18.0*pow_v_17
        + p_p->c0419y * pow_u_4  *19.0*pow_v_18
        + p_p->c0420y * pow_u_4  *20.0*pow_v_19
        + p_p->c0421y * pow_u_4  *21.0*pow_v_20;
v_y=v_y + p_p->c0500y * pow_u_5  *         0.0        
        + p_p->c0501y * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502y * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503y * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504y * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505y * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506y * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507y * pow_u_5  * 7.0*pow_v_6
        + p_p->c0508y * pow_u_5  * 8.0*pow_v_7
        + p_p->c0509y * pow_u_5  * 9.0*pow_v_8
        + p_p->c0510y * pow_u_5  *10.0*pow_v_9
        + p_p->c0511y * pow_u_5  *11.0*pow_v_10
        + p_p->c0512y * pow_u_5  *12.0*pow_v_11
        + p_p->c0513y * pow_u_5  *13.0*pow_v_12
        + p_p->c0514y * pow_u_5  *14.0*pow_v_13
        + p_p->c0515y * pow_u_5  *15.0*pow_v_14
        + p_p->c0516y * pow_u_5  *16.0*pow_v_15
        + p_p->c0517y * pow_u_5  *17.0*pow_v_16
        + p_p->c0518y * pow_u_5  *18.0*pow_v_17
        + p_p->c0519y * pow_u_5  *19.0*pow_v_18
        + p_p->c0520y * pow_u_5  *20.0*pow_v_19
        + p_p->c0521y * pow_u_5  *21.0*pow_v_20;
v_y=v_y + p_p->c0600y * pow_u_6  *         0.0        
        + p_p->c0601y * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602y * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603y * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604y * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605y * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606y * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607y * pow_u_6  * 7.0*pow_v_6
        + p_p->c0608y * pow_u_6  * 8.0*pow_v_7
        + p_p->c0609y * pow_u_6  * 9.0*pow_v_8
        + p_p->c0610y * pow_u_6  *10.0*pow_v_9
        + p_p->c0611y * pow_u_6  *11.0*pow_v_10
        + p_p->c0612y * pow_u_6  *12.0*pow_v_11
        + p_p->c0613y * pow_u_6  *13.0*pow_v_12
        + p_p->c0614y * pow_u_6  *14.0*pow_v_13
        + p_p->c0615y * pow_u_6  *15.0*pow_v_14
        + p_p->c0616y * pow_u_6  *16.0*pow_v_15
        + p_p->c0617y * pow_u_6  *17.0*pow_v_16
        + p_p->c0618y * pow_u_6  *18.0*pow_v_17
        + p_p->c0619y * pow_u_6  *19.0*pow_v_18
        + p_p->c0620y * pow_u_6  *20.0*pow_v_19
        + p_p->c0621y * pow_u_6  *21.0*pow_v_20;
v_y=v_y + p_p->c0700y * pow_u_7  *         0.0        
        + p_p->c0701y * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702y * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703y * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704y * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705y * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706y * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707y * pow_u_7  * 7.0*pow_v_6
        + p_p->c0708y * pow_u_7  * 8.0*pow_v_7
        + p_p->c0709y * pow_u_7  * 9.0*pow_v_8
        + p_p->c0710y * pow_u_7  *10.0*pow_v_9
        + p_p->c0711y * pow_u_7  *11.0*pow_v_10
        + p_p->c0712y * pow_u_7  *12.0*pow_v_11
        + p_p->c0713y * pow_u_7  *13.0*pow_v_12
        + p_p->c0714y * pow_u_7  *14.0*pow_v_13
        + p_p->c0715y * pow_u_7  *15.0*pow_v_14
        + p_p->c0716y * pow_u_7  *16.0*pow_v_15
        + p_p->c0717y * pow_u_7  *17.0*pow_v_16
        + p_p->c0718y * pow_u_7  *18.0*pow_v_17
        + p_p->c0719y * pow_u_7  *19.0*pow_v_18
        + p_p->c0720y * pow_u_7  *20.0*pow_v_19
        + p_p->c0721y * pow_u_7  *21.0*pow_v_20;
v_y=v_y + p_p->c0800y * pow_u_8  *         0.0        
        + p_p->c0801y * pow_u_8  * 1.0*pow_v_0
        + p_p->c0802y * pow_u_8  * 2.0*pow_v_1
        + p_p->c0803y * pow_u_8  * 3.0*pow_v_2
        + p_p->c0804y * pow_u_8  * 4.0*pow_v_3
        + p_p->c0805y * pow_u_8  * 5.0*pow_v_4
        + p_p->c0806y * pow_u_8  * 6.0*pow_v_5
        + p_p->c0807y * pow_u_8  * 7.0*pow_v_6
        + p_p->c0808y * pow_u_8  * 8.0*pow_v_7
        + p_p->c0809y * pow_u_8  * 9.0*pow_v_8
        + p_p->c0810y * pow_u_8  *10.0*pow_v_9
        + p_p->c0811y * pow_u_8  *11.0*pow_v_10
        + p_p->c0812y * pow_u_8  *12.0*pow_v_11
        + p_p->c0813y * pow_u_8  *13.0*pow_v_12
        + p_p->c0814y * pow_u_8  *14.0*pow_v_13
        + p_p->c0815y * pow_u_8  *15.0*pow_v_14
        + p_p->c0816y * pow_u_8  *16.0*pow_v_15
        + p_p->c0817y * pow_u_8  *17.0*pow_v_16
        + p_p->c0818y * pow_u_8  *18.0*pow_v_17
        + p_p->c0819y * pow_u_8  *19.0*pow_v_18
        + p_p->c0820y * pow_u_8  *20.0*pow_v_19
        + p_p->c0821y * pow_u_8  *21.0*pow_v_20;
v_y=v_y + p_p->c0900y * pow_u_9  *         0.0        
        + p_p->c0901y * pow_u_9  * 1.0*pow_v_0
        + p_p->c0902y * pow_u_9  * 2.0*pow_v_1
        + p_p->c0903y * pow_u_9  * 3.0*pow_v_2
        + p_p->c0904y * pow_u_9  * 4.0*pow_v_3
        + p_p->c0905y * pow_u_9  * 5.0*pow_v_4
        + p_p->c0906y * pow_u_9  * 6.0*pow_v_5
        + p_p->c0907y * pow_u_9  * 7.0*pow_v_6
        + p_p->c0908y * pow_u_9  * 8.0*pow_v_7
        + p_p->c0909y * pow_u_9  * 9.0*pow_v_8
        + p_p->c0910y * pow_u_9  *10.0*pow_v_9
        + p_p->c0911y * pow_u_9  *11.0*pow_v_10
        + p_p->c0912y * pow_u_9  *12.0*pow_v_11
        + p_p->c0913y * pow_u_9  *13.0*pow_v_12
        + p_p->c0914y * pow_u_9  *14.0*pow_v_13
        + p_p->c0915y * pow_u_9  *15.0*pow_v_14
        + p_p->c0916y * pow_u_9  *16.0*pow_v_15
        + p_p->c0917y * pow_u_9  *17.0*pow_v_16
        + p_p->c0918y * pow_u_9  *18.0*pow_v_17
        + p_p->c0919y * pow_u_9  *19.0*pow_v_18
        + p_p->c0920y * pow_u_9  *20.0*pow_v_19
        + p_p->c0921y * pow_u_9  *21.0*pow_v_20;
v_y=v_y + p_p->c1000y * pow_u_10  *         0.0        
        + p_p->c1001y * pow_u_10  * 1.0*pow_v_0
        + p_p->c1002y * pow_u_10  * 2.0*pow_v_1
        + p_p->c1003y * pow_u_10  * 3.0*pow_v_2
        + p_p->c1004y * pow_u_10  * 4.0*pow_v_3
        + p_p->c1005y * pow_u_10  * 5.0*pow_v_4
        + p_p->c1006y * pow_u_10  * 6.0*pow_v_5
        + p_p->c1007y * pow_u_10  * 7.0*pow_v_6
        + p_p->c1008y * pow_u_10  * 8.0*pow_v_7
        + p_p->c1009y * pow_u_10  * 9.0*pow_v_8
        + p_p->c1010y * pow_u_10  *10.0*pow_v_9
        + p_p->c1011y * pow_u_10  *11.0*pow_v_10
        + p_p->c1012y * pow_u_10  *12.0*pow_v_11
        + p_p->c1013y * pow_u_10  *13.0*pow_v_12
        + p_p->c1014y * pow_u_10  *14.0*pow_v_13
        + p_p->c1015y * pow_u_10  *15.0*pow_v_14
        + p_p->c1016y * pow_u_10  *16.0*pow_v_15
        + p_p->c1017y * pow_u_10  *17.0*pow_v_16
        + p_p->c1018y * pow_u_10  *18.0*pow_v_17
        + p_p->c1019y * pow_u_10  *19.0*pow_v_18
        + p_p->c1020y * pow_u_10  *20.0*pow_v_19
        + p_p->c1021y * pow_u_10  *21.0*pow_v_20;
v_y=v_y + p_p->c1100y * pow_u_11  *         0.0        
        + p_p->c1101y * pow_u_11  * 1.0*pow_v_0
        + p_p->c1102y * pow_u_11  * 2.0*pow_v_1
        + p_p->c1103y * pow_u_11  * 3.0*pow_v_2
        + p_p->c1104y * pow_u_11  * 4.0*pow_v_3
        + p_p->c1105y * pow_u_11  * 5.0*pow_v_4
        + p_p->c1106y * pow_u_11  * 6.0*pow_v_5
        + p_p->c1107y * pow_u_11  * 7.0*pow_v_6
        + p_p->c1108y * pow_u_11  * 8.0*pow_v_7
        + p_p->c1109y * pow_u_11  * 9.0*pow_v_8
        + p_p->c1110y * pow_u_11  *10.0*pow_v_9
        + p_p->c1111y * pow_u_11  *11.0*pow_v_10
        + p_p->c1112y * pow_u_11  *12.0*pow_v_11
        + p_p->c1113y * pow_u_11  *13.0*pow_v_12
        + p_p->c1114y * pow_u_11  *14.0*pow_v_13
        + p_p->c1115y * pow_u_11  *15.0*pow_v_14
        + p_p->c1116y * pow_u_11  *16.0*pow_v_15
        + p_p->c1117y * pow_u_11  *17.0*pow_v_16
        + p_p->c1118y * pow_u_11  *18.0*pow_v_17
        + p_p->c1119y * pow_u_11  *19.0*pow_v_18
        + p_p->c1120y * pow_u_11  *20.0*pow_v_19
        + p_p->c1121y * pow_u_11  *21.0*pow_v_20;
v_y=v_y + p_p->c1200y * pow_u_12  *         0.0        
        + p_p->c1201y * pow_u_12  * 1.0*pow_v_0
        + p_p->c1202y * pow_u_12  * 2.0*pow_v_1
        + p_p->c1203y * pow_u_12  * 3.0*pow_v_2
        + p_p->c1204y * pow_u_12  * 4.0*pow_v_3
        + p_p->c1205y * pow_u_12  * 5.0*pow_v_4
        + p_p->c1206y * pow_u_12  * 6.0*pow_v_5
        + p_p->c1207y * pow_u_12  * 7.0*pow_v_6
        + p_p->c1208y * pow_u_12  * 8.0*pow_v_7
        + p_p->c1209y * pow_u_12  * 9.0*pow_v_8
        + p_p->c1210y * pow_u_12  *10.0*pow_v_9
        + p_p->c1211y * pow_u_12  *11.0*pow_v_10
        + p_p->c1212y * pow_u_12  *12.0*pow_v_11
        + p_p->c1213y * pow_u_12  *13.0*pow_v_12
        + p_p->c1214y * pow_u_12  *14.0*pow_v_13
        + p_p->c1215y * pow_u_12  *15.0*pow_v_14
        + p_p->c1216y * pow_u_12  *16.0*pow_v_15
        + p_p->c1217y * pow_u_12  *17.0*pow_v_16
        + p_p->c1218y * pow_u_12  *18.0*pow_v_17
        + p_p->c1219y * pow_u_12  *19.0*pow_v_18
        + p_p->c1220y * pow_u_12  *20.0*pow_v_19
        + p_p->c1221y * pow_u_12  *21.0*pow_v_20;
v_y=v_y + p_p->c1300y * pow_u_13  *         0.0        
        + p_p->c1301y * pow_u_13  * 1.0*pow_v_0
        + p_p->c1302y * pow_u_13  * 2.0*pow_v_1
        + p_p->c1303y * pow_u_13  * 3.0*pow_v_2
        + p_p->c1304y * pow_u_13  * 4.0*pow_v_3
        + p_p->c1305y * pow_u_13  * 5.0*pow_v_4
        + p_p->c1306y * pow_u_13  * 6.0*pow_v_5
        + p_p->c1307y * pow_u_13  * 7.0*pow_v_6
        + p_p->c1308y * pow_u_13  * 8.0*pow_v_7
        + p_p->c1309y * pow_u_13  * 9.0*pow_v_8
        + p_p->c1310y * pow_u_13  *10.0*pow_v_9
        + p_p->c1311y * pow_u_13  *11.0*pow_v_10
        + p_p->c1312y * pow_u_13  *12.0*pow_v_11
        + p_p->c1313y * pow_u_13  *13.0*pow_v_12
        + p_p->c1314y * pow_u_13  *14.0*pow_v_13
        + p_p->c1315y * pow_u_13  *15.0*pow_v_14
        + p_p->c1316y * pow_u_13  *16.0*pow_v_15
        + p_p->c1317y * pow_u_13  *17.0*pow_v_16
        + p_p->c1318y * pow_u_13  *18.0*pow_v_17
        + p_p->c1319y * pow_u_13  *19.0*pow_v_18
        + p_p->c1320y * pow_u_13  *20.0*pow_v_19
        + p_p->c1321y * pow_u_13  *21.0*pow_v_20;
v_y=v_y + p_p->c1400y * pow_u_14  *         0.0        
        + p_p->c1401y * pow_u_14  * 1.0*pow_v_0
        + p_p->c1402y * pow_u_14  * 2.0*pow_v_1
        + p_p->c1403y * pow_u_14  * 3.0*pow_v_2
        + p_p->c1404y * pow_u_14  * 4.0*pow_v_3
        + p_p->c1405y * pow_u_14  * 5.0*pow_v_4
        + p_p->c1406y * pow_u_14  * 6.0*pow_v_5
        + p_p->c1407y * pow_u_14  * 7.0*pow_v_6
        + p_p->c1408y * pow_u_14  * 8.0*pow_v_7
        + p_p->c1409y * pow_u_14  * 9.0*pow_v_8
        + p_p->c1410y * pow_u_14  *10.0*pow_v_9
        + p_p->c1411y * pow_u_14  *11.0*pow_v_10
        + p_p->c1412y * pow_u_14  *12.0*pow_v_11
        + p_p->c1413y * pow_u_14  *13.0*pow_v_12
        + p_p->c1414y * pow_u_14  *14.0*pow_v_13
        + p_p->c1415y * pow_u_14  *15.0*pow_v_14
        + p_p->c1416y * pow_u_14  *16.0*pow_v_15
        + p_p->c1417y * pow_u_14  *17.0*pow_v_16
        + p_p->c1418y * pow_u_14  *18.0*pow_v_17
        + p_p->c1419y * pow_u_14  *19.0*pow_v_18
        + p_p->c1420y * pow_u_14  *20.0*pow_v_19
        + p_p->c1421y * pow_u_14  *21.0*pow_v_20;
v_y=v_y + p_p->c1500y * pow_u_15  *         0.0        
        + p_p->c1501y * pow_u_15  * 1.0*pow_v_0
        + p_p->c1502y * pow_u_15  * 2.0*pow_v_1
        + p_p->c1503y * pow_u_15  * 3.0*pow_v_2
        + p_p->c1504y * pow_u_15  * 4.0*pow_v_3
        + p_p->c1505y * pow_u_15  * 5.0*pow_v_4
        + p_p->c1506y * pow_u_15  * 6.0*pow_v_5
        + p_p->c1507y * pow_u_15  * 7.0*pow_v_6
        + p_p->c1508y * pow_u_15  * 8.0*pow_v_7
        + p_p->c1509y * pow_u_15  * 9.0*pow_v_8
        + p_p->c1510y * pow_u_15  *10.0*pow_v_9
        + p_p->c1511y * pow_u_15  *11.0*pow_v_10
        + p_p->c1512y * pow_u_15  *12.0*pow_v_11
        + p_p->c1513y * pow_u_15  *13.0*pow_v_12
        + p_p->c1514y * pow_u_15  *14.0*pow_v_13
        + p_p->c1515y * pow_u_15  *15.0*pow_v_14
        + p_p->c1516y * pow_u_15  *16.0*pow_v_15
        + p_p->c1517y * pow_u_15  *17.0*pow_v_16
        + p_p->c1518y * pow_u_15  *18.0*pow_v_17
        + p_p->c1519y * pow_u_15  *19.0*pow_v_18
        + p_p->c1520y * pow_u_15  *20.0*pow_v_19
        + p_p->c1521y * pow_u_15  *21.0*pow_v_20;
v_y=v_y + p_p->c1600y * pow_u_16  *         0.0        
        + p_p->c1601y * pow_u_16  * 1.0*pow_v_0
        + p_p->c1602y * pow_u_16  * 2.0*pow_v_1
        + p_p->c1603y * pow_u_16  * 3.0*pow_v_2
        + p_p->c1604y * pow_u_16  * 4.0*pow_v_3
        + p_p->c1605y * pow_u_16  * 5.0*pow_v_4
        + p_p->c1606y * pow_u_16  * 6.0*pow_v_5
        + p_p->c1607y * pow_u_16  * 7.0*pow_v_6
        + p_p->c1608y * pow_u_16  * 8.0*pow_v_7
        + p_p->c1609y * pow_u_16  * 9.0*pow_v_8
        + p_p->c1610y * pow_u_16  *10.0*pow_v_9
        + p_p->c1611y * pow_u_16  *11.0*pow_v_10
        + p_p->c1612y * pow_u_16  *12.0*pow_v_11
        + p_p->c1613y * pow_u_16  *13.0*pow_v_12
        + p_p->c1614y * pow_u_16  *14.0*pow_v_13
        + p_p->c1615y * pow_u_16  *15.0*pow_v_14
        + p_p->c1616y * pow_u_16  *16.0*pow_v_15
        + p_p->c1617y * pow_u_16  *17.0*pow_v_16
        + p_p->c1618y * pow_u_16  *18.0*pow_v_17
        + p_p->c1619y * pow_u_16  *19.0*pow_v_18
        + p_p->c1620y * pow_u_16  *20.0*pow_v_19
        + p_p->c1621y * pow_u_16  *21.0*pow_v_20;
v_y=v_y + p_p->c1700y * pow_u_17  *         0.0        
        + p_p->c1701y * pow_u_17  * 1.0*pow_v_0
        + p_p->c1702y * pow_u_17  * 2.0*pow_v_1
        + p_p->c1703y * pow_u_17  * 3.0*pow_v_2
        + p_p->c1704y * pow_u_17  * 4.0*pow_v_3
        + p_p->c1705y * pow_u_17  * 5.0*pow_v_4
        + p_p->c1706y * pow_u_17  * 6.0*pow_v_5
        + p_p->c1707y * pow_u_17  * 7.0*pow_v_6
        + p_p->c1708y * pow_u_17  * 8.0*pow_v_7
        + p_p->c1709y * pow_u_17  * 9.0*pow_v_8
        + p_p->c1710y * pow_u_17  *10.0*pow_v_9
        + p_p->c1711y * pow_u_17  *11.0*pow_v_10
        + p_p->c1712y * pow_u_17  *12.0*pow_v_11
        + p_p->c1713y * pow_u_17  *13.0*pow_v_12
        + p_p->c1714y * pow_u_17  *14.0*pow_v_13
        + p_p->c1715y * pow_u_17  *15.0*pow_v_14
        + p_p->c1716y * pow_u_17  *16.0*pow_v_15
        + p_p->c1717y * pow_u_17  *17.0*pow_v_16
        + p_p->c1718y * pow_u_17  *18.0*pow_v_17
        + p_p->c1719y * pow_u_17  *19.0*pow_v_18
        + p_p->c1720y * pow_u_17  *20.0*pow_v_19
        + p_p->c1721y * pow_u_17  *21.0*pow_v_20;
v_y=v_y + p_p->c1800y * pow_u_18  *         0.0        
        + p_p->c1801y * pow_u_18  * 1.0*pow_v_0
        + p_p->c1802y * pow_u_18  * 2.0*pow_v_1
        + p_p->c1803y * pow_u_18  * 3.0*pow_v_2
        + p_p->c1804y * pow_u_18  * 4.0*pow_v_3
        + p_p->c1805y * pow_u_18  * 5.0*pow_v_4
        + p_p->c1806y * pow_u_18  * 6.0*pow_v_5
        + p_p->c1807y * pow_u_18  * 7.0*pow_v_6
        + p_p->c1808y * pow_u_18  * 8.0*pow_v_7
        + p_p->c1809y * pow_u_18  * 9.0*pow_v_8
        + p_p->c1810y * pow_u_18  *10.0*pow_v_9
        + p_p->c1811y * pow_u_18  *11.0*pow_v_10
        + p_p->c1812y * pow_u_18  *12.0*pow_v_11
        + p_p->c1813y * pow_u_18  *13.0*pow_v_12
        + p_p->c1814y * pow_u_18  *14.0*pow_v_13
        + p_p->c1815y * pow_u_18  *15.0*pow_v_14
        + p_p->c1816y * pow_u_18  *16.0*pow_v_15
        + p_p->c1817y * pow_u_18  *17.0*pow_v_16
        + p_p->c1818y * pow_u_18  *18.0*pow_v_17
        + p_p->c1819y * pow_u_18  *19.0*pow_v_18
        + p_p->c1820y * pow_u_18  *20.0*pow_v_19
        + p_p->c1821y * pow_u_18  *21.0*pow_v_20;
v_y=v_y + p_p->c1900y * pow_u_19  *         0.0        
        + p_p->c1901y * pow_u_19  * 1.0*pow_v_0
        + p_p->c1902y * pow_u_19  * 2.0*pow_v_1
        + p_p->c1903y * pow_u_19  * 3.0*pow_v_2
        + p_p->c1904y * pow_u_19  * 4.0*pow_v_3
        + p_p->c1905y * pow_u_19  * 5.0*pow_v_4
        + p_p->c1906y * pow_u_19  * 6.0*pow_v_5
        + p_p->c1907y * pow_u_19  * 7.0*pow_v_6
        + p_p->c1908y * pow_u_19  * 8.0*pow_v_7
        + p_p->c1909y * pow_u_19  * 9.0*pow_v_8
        + p_p->c1910y * pow_u_19  *10.0*pow_v_9
        + p_p->c1911y * pow_u_19  *11.0*pow_v_10
        + p_p->c1912y * pow_u_19  *12.0*pow_v_11
        + p_p->c1913y * pow_u_19  *13.0*pow_v_12
        + p_p->c1914y * pow_u_19  *14.0*pow_v_13
        + p_p->c1915y * pow_u_19  *15.0*pow_v_14
        + p_p->c1916y * pow_u_19  *16.0*pow_v_15
        + p_p->c1917y * pow_u_19  *17.0*pow_v_16
        + p_p->c1918y * pow_u_19  *18.0*pow_v_17
        + p_p->c1919y * pow_u_19  *19.0*pow_v_18
        + p_p->c1920y * pow_u_19  *20.0*pow_v_19
        + p_p->c1921y * pow_u_19  *21.0*pow_v_20;
v_y=v_y + p_p->c2000y * pow_u_20  *         0.0        
        + p_p->c2001y * pow_u_20  * 1.0*pow_v_0
        + p_p->c2002y * pow_u_20  * 2.0*pow_v_1
        + p_p->c2003y * pow_u_20  * 3.0*pow_v_2
        + p_p->c2004y * pow_u_20  * 4.0*pow_v_3
        + p_p->c2005y * pow_u_20  * 5.0*pow_v_4
        + p_p->c2006y * pow_u_20  * 6.0*pow_v_5
        + p_p->c2007y * pow_u_20  * 7.0*pow_v_6
        + p_p->c2008y * pow_u_20  * 8.0*pow_v_7
        + p_p->c2009y * pow_u_20  * 9.0*pow_v_8
        + p_p->c2010y * pow_u_20  *10.0*pow_v_9
        + p_p->c2011y * pow_u_20  *11.0*pow_v_10
        + p_p->c2012y * pow_u_20  *12.0*pow_v_11
        + p_p->c2013y * pow_u_20  *13.0*pow_v_12
        + p_p->c2014y * pow_u_20  *14.0*pow_v_13
        + p_p->c2015y * pow_u_20  *15.0*pow_v_14
        + p_p->c2016y * pow_u_20  *16.0*pow_v_15
        + p_p->c2017y * pow_u_20  *17.0*pow_v_16
        + p_p->c2018y * pow_u_20  *18.0*pow_v_17
        + p_p->c2019y * pow_u_20  *19.0*pow_v_18
        + p_p->c2020y * pow_u_20  *20.0*pow_v_19
        + p_p->c2021y * pow_u_20  *21.0*pow_v_20;
v_y=v_y + p_p->c2100y * pow_u_21  *         0.0        
        + p_p->c2101y * pow_u_21  * 1.0*pow_v_0
        + p_p->c2102y * pow_u_21  * 2.0*pow_v_1
        + p_p->c2103y * pow_u_21  * 3.0*pow_v_2
        + p_p->c2104y * pow_u_21  * 4.0*pow_v_3
        + p_p->c2105y * pow_u_21  * 5.0*pow_v_4
        + p_p->c2106y * pow_u_21  * 6.0*pow_v_5
        + p_p->c2107y * pow_u_21  * 7.0*pow_v_6
        + p_p->c2108y * pow_u_21  * 8.0*pow_v_7
        + p_p->c2109y * pow_u_21  * 9.0*pow_v_8
        + p_p->c2110y * pow_u_21  *10.0*pow_v_9
        + p_p->c2111y * pow_u_21  *11.0*pow_v_10
        + p_p->c2112y * pow_u_21  *12.0*pow_v_11
        + p_p->c2113y * pow_u_21  *13.0*pow_v_12
        + p_p->c2114y * pow_u_21  *14.0*pow_v_13
        + p_p->c2115y * pow_u_21  *15.0*pow_v_14
        + p_p->c2116y * pow_u_21  *16.0*pow_v_15
        + p_p->c2117y * pow_u_21  *17.0*pow_v_16
        + p_p->c2118y * pow_u_21  *18.0*pow_v_17
        + p_p->c2119y * pow_u_21  *19.0*pow_v_18
        + p_p->c2120y * pow_u_21  *20.0*pow_v_19
        + p_p->c2121y * pow_u_21  *21.0*pow_v_20;

v_z     = p_p->c0000z * pow_u_0  *         0.0        
        + p_p->c0001z * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002z * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003z * pow_u_0  * 3.0*pow_v_2
        + p_p->c0004z * pow_u_0  * 4.0*pow_v_3
        + p_p->c0005z * pow_u_0  * 5.0*pow_v_4
        + p_p->c0006z * pow_u_0  * 6.0*pow_v_5
        + p_p->c0007z * pow_u_0  * 7.0*pow_v_6
        + p_p->c0008z * pow_u_0  * 8.0*pow_v_7
        + p_p->c0009z * pow_u_0  * 9.0*pow_v_8
        + p_p->c0010z * pow_u_0  *10.0*pow_v_9
        + p_p->c0011z * pow_u_0  *11.0*pow_v_10
        + p_p->c0012z * pow_u_0  *12.0*pow_v_11
        + p_p->c0013z * pow_u_0  *13.0*pow_v_12
        + p_p->c0014z * pow_u_0  *14.0*pow_v_13
        + p_p->c0015z * pow_u_0  *15.0*pow_v_14
        + p_p->c0016z * pow_u_0  *16.0*pow_v_15
        + p_p->c0017z * pow_u_0  *17.0*pow_v_16
        + p_p->c0018z * pow_u_0  *18.0*pow_v_17
        + p_p->c0019z * pow_u_0  *19.0*pow_v_18
        + p_p->c0020z * pow_u_0  *20.0*pow_v_19
        + p_p->c0021z * pow_u_0  *21.0*pow_v_20;
v_z=v_z + p_p->c0100z * pow_u_1  *         0.0        
        + p_p->c0101z * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102z * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103z * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104z * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105z * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106z * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107z * pow_u_1  * 7.0*pow_v_6
        + p_p->c0108z * pow_u_1  * 8.0*pow_v_7
        + p_p->c0109z * pow_u_1  * 9.0*pow_v_8
        + p_p->c0110z * pow_u_1  *10.0*pow_v_9
        + p_p->c0111z * pow_u_1  *11.0*pow_v_10
        + p_p->c0112z * pow_u_1  *12.0*pow_v_11
        + p_p->c0113z * pow_u_1  *13.0*pow_v_12
        + p_p->c0114z * pow_u_1  *14.0*pow_v_13
        + p_p->c0115z * pow_u_1  *15.0*pow_v_14
        + p_p->c0116z * pow_u_1  *16.0*pow_v_15
        + p_p->c0117z * pow_u_1  *17.0*pow_v_16
        + p_p->c0118z * pow_u_1  *18.0*pow_v_17
        + p_p->c0119z * pow_u_1  *19.0*pow_v_18
        + p_p->c0120z * pow_u_1  *20.0*pow_v_19
        + p_p->c0121z * pow_u_1  *21.0*pow_v_20;
v_z=v_z + p_p->c0200z * pow_u_2  *         0.0        
        + p_p->c0201z * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202z * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203z * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204z * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205z * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206z * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207z * pow_u_2  * 7.0*pow_v_6
        + p_p->c0208z * pow_u_2  * 8.0*pow_v_7
        + p_p->c0209z * pow_u_2  * 9.0*pow_v_8
        + p_p->c0210z * pow_u_2  *10.0*pow_v_9
        + p_p->c0211z * pow_u_2  *11.0*pow_v_10
        + p_p->c0212z * pow_u_2  *12.0*pow_v_11
        + p_p->c0213z * pow_u_2  *13.0*pow_v_12
        + p_p->c0214z * pow_u_2  *14.0*pow_v_13
        + p_p->c0215z * pow_u_2  *15.0*pow_v_14
        + p_p->c0216z * pow_u_2  *16.0*pow_v_15
        + p_p->c0217z * pow_u_2  *17.0*pow_v_16
        + p_p->c0218z * pow_u_2  *18.0*pow_v_17
        + p_p->c0219z * pow_u_2  *19.0*pow_v_18
        + p_p->c0220z * pow_u_2  *20.0*pow_v_19
        + p_p->c0221z * pow_u_2  *21.0*pow_v_20;
v_z=v_z + p_p->c0300z * pow_u_3  *         0.0        
        + p_p->c0301z * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302z * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303z * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304z * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305z * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306z * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307z * pow_u_3  * 7.0*pow_v_6
        + p_p->c0308z * pow_u_3  * 8.0*pow_v_7
        + p_p->c0309z * pow_u_3  * 9.0*pow_v_8
        + p_p->c0310z * pow_u_3  *10.0*pow_v_9
        + p_p->c0311z * pow_u_3  *11.0*pow_v_10
        + p_p->c0312z * pow_u_3  *12.0*pow_v_11
        + p_p->c0313z * pow_u_3  *13.0*pow_v_12
        + p_p->c0314z * pow_u_3  *14.0*pow_v_13
        + p_p->c0315z * pow_u_3  *15.0*pow_v_14
        + p_p->c0316z * pow_u_3  *16.0*pow_v_15
        + p_p->c0317z * pow_u_3  *17.0*pow_v_16
        + p_p->c0318z * pow_u_3  *18.0*pow_v_17
        + p_p->c0319z * pow_u_3  *19.0*pow_v_18
        + p_p->c0320z * pow_u_3  *20.0*pow_v_19
        + p_p->c0321z * pow_u_3  *21.0*pow_v_20;
v_z=v_z + p_p->c0400z * pow_u_4  *         0.0        
        + p_p->c0401z * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402z * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403z * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404z * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405z * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406z * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407z * pow_u_4  * 7.0*pow_v_6
        + p_p->c0408z * pow_u_4  * 8.0*pow_v_7
        + p_p->c0409z * pow_u_4  * 9.0*pow_v_8
        + p_p->c0410z * pow_u_4  *10.0*pow_v_9
        + p_p->c0411z * pow_u_4  *11.0*pow_v_10
        + p_p->c0412z * pow_u_4  *12.0*pow_v_11
        + p_p->c0413z * pow_u_4  *13.0*pow_v_12
        + p_p->c0414z * pow_u_4  *14.0*pow_v_13
        + p_p->c0415z * pow_u_4  *15.0*pow_v_14
        + p_p->c0416z * pow_u_4  *16.0*pow_v_15
        + p_p->c0417z * pow_u_4  *17.0*pow_v_16
        + p_p->c0418z * pow_u_4  *18.0*pow_v_17
        + p_p->c0419z * pow_u_4  *19.0*pow_v_18
        + p_p->c0420z * pow_u_4  *20.0*pow_v_19
        + p_p->c0421z * pow_u_4  *21.0*pow_v_20;
v_z=v_z + p_p->c0500z * pow_u_5  *         0.0        
        + p_p->c0501z * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502z * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503z * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504z * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505z * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506z * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507z * pow_u_5  * 7.0*pow_v_6
        + p_p->c0508z * pow_u_5  * 8.0*pow_v_7
        + p_p->c0509z * pow_u_5  * 9.0*pow_v_8
        + p_p->c0510z * pow_u_5  *10.0*pow_v_9
        + p_p->c0511z * pow_u_5  *11.0*pow_v_10
        + p_p->c0512z * pow_u_5  *12.0*pow_v_11
        + p_p->c0513z * pow_u_5  *13.0*pow_v_12
        + p_p->c0514z * pow_u_5  *14.0*pow_v_13
        + p_p->c0515z * pow_u_5  *15.0*pow_v_14
        + p_p->c0516z * pow_u_5  *16.0*pow_v_15
        + p_p->c0517z * pow_u_5  *17.0*pow_v_16
        + p_p->c0518z * pow_u_5  *18.0*pow_v_17
        + p_p->c0519z * pow_u_5  *19.0*pow_v_18
        + p_p->c0520z * pow_u_5  *20.0*pow_v_19
        + p_p->c0521z * pow_u_5  *21.0*pow_v_20;
v_z=v_z + p_p->c0600z * pow_u_6  *         0.0        
        + p_p->c0601z * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602z * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603z * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604z * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605z * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606z * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607z * pow_u_6  * 7.0*pow_v_6
        + p_p->c0608z * pow_u_6  * 8.0*pow_v_7
        + p_p->c0609z * pow_u_6  * 9.0*pow_v_8
        + p_p->c0610z * pow_u_6  *10.0*pow_v_9
        + p_p->c0611z * pow_u_6  *11.0*pow_v_10
        + p_p->c0612z * pow_u_6  *12.0*pow_v_11
        + p_p->c0613z * pow_u_6  *13.0*pow_v_12
        + p_p->c0614z * pow_u_6  *14.0*pow_v_13
        + p_p->c0615z * pow_u_6  *15.0*pow_v_14
        + p_p->c0616z * pow_u_6  *16.0*pow_v_15
        + p_p->c0617z * pow_u_6  *17.0*pow_v_16
        + p_p->c0618z * pow_u_6  *18.0*pow_v_17
        + p_p->c0619z * pow_u_6  *19.0*pow_v_18
        + p_p->c0620z * pow_u_6  *20.0*pow_v_19
        + p_p->c0621z * pow_u_6  *21.0*pow_v_20;
v_z=v_z + p_p->c0700z * pow_u_7  *         0.0        
        + p_p->c0701z * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702z * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703z * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704z * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705z * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706z * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707z * pow_u_7  * 7.0*pow_v_6
        + p_p->c0708z * pow_u_7  * 8.0*pow_v_7
        + p_p->c0709z * pow_u_7  * 9.0*pow_v_8
        + p_p->c0710z * pow_u_7  *10.0*pow_v_9
        + p_p->c0711z * pow_u_7  *11.0*pow_v_10
        + p_p->c0712z * pow_u_7  *12.0*pow_v_11
        + p_p->c0713z * pow_u_7  *13.0*pow_v_12
        + p_p->c0714z * pow_u_7  *14.0*pow_v_13
        + p_p->c0715z * pow_u_7  *15.0*pow_v_14
        + p_p->c0716z * pow_u_7  *16.0*pow_v_15
        + p_p->c0717z * pow_u_7  *17.0*pow_v_16
        + p_p->c0718z * pow_u_7  *18.0*pow_v_17
        + p_p->c0719z * pow_u_7  *19.0*pow_v_18
        + p_p->c0720z * pow_u_7  *20.0*pow_v_19
        + p_p->c0721z * pow_u_7  *21.0*pow_v_20;
v_z=v_z + p_p->c0800z * pow_u_8  *         0.0        
        + p_p->c0801z * pow_u_8  * 1.0*pow_v_0
        + p_p->c0802z * pow_u_8  * 2.0*pow_v_1
        + p_p->c0803z * pow_u_8  * 3.0*pow_v_2
        + p_p->c0804z * pow_u_8  * 4.0*pow_v_3
        + p_p->c0805z * pow_u_8  * 5.0*pow_v_4
        + p_p->c0806z * pow_u_8  * 6.0*pow_v_5
        + p_p->c0807z * pow_u_8  * 7.0*pow_v_6
        + p_p->c0808z * pow_u_8  * 8.0*pow_v_7
        + p_p->c0809z * pow_u_8  * 9.0*pow_v_8
        + p_p->c0810z * pow_u_8  *10.0*pow_v_9
        + p_p->c0811z * pow_u_8  *11.0*pow_v_10
        + p_p->c0812z * pow_u_8  *12.0*pow_v_11
        + p_p->c0813z * pow_u_8  *13.0*pow_v_12
        + p_p->c0814z * pow_u_8  *14.0*pow_v_13
        + p_p->c0815z * pow_u_8  *15.0*pow_v_14
        + p_p->c0816z * pow_u_8  *16.0*pow_v_15
        + p_p->c0817z * pow_u_8  *17.0*pow_v_16
        + p_p->c0818z * pow_u_8  *18.0*pow_v_17
        + p_p->c0819z * pow_u_8  *19.0*pow_v_18
        + p_p->c0820z * pow_u_8  *20.0*pow_v_19
        + p_p->c0821z * pow_u_8  *21.0*pow_v_20;
v_z=v_z + p_p->c0900z * pow_u_9  *         0.0        
        + p_p->c0901z * pow_u_9  * 1.0*pow_v_0
        + p_p->c0902z * pow_u_9  * 2.0*pow_v_1
        + p_p->c0903z * pow_u_9  * 3.0*pow_v_2
        + p_p->c0904z * pow_u_9  * 4.0*pow_v_3
        + p_p->c0905z * pow_u_9  * 5.0*pow_v_4
        + p_p->c0906z * pow_u_9  * 6.0*pow_v_5
        + p_p->c0907z * pow_u_9  * 7.0*pow_v_6
        + p_p->c0908z * pow_u_9  * 8.0*pow_v_7
        + p_p->c0909z * pow_u_9  * 9.0*pow_v_8
        + p_p->c0910z * pow_u_9  *10.0*pow_v_9
        + p_p->c0911z * pow_u_9  *11.0*pow_v_10
        + p_p->c0912z * pow_u_9  *12.0*pow_v_11
        + p_p->c0913z * pow_u_9  *13.0*pow_v_12
        + p_p->c0914z * pow_u_9  *14.0*pow_v_13
        + p_p->c0915z * pow_u_9  *15.0*pow_v_14
        + p_p->c0916z * pow_u_9  *16.0*pow_v_15
        + p_p->c0917z * pow_u_9  *17.0*pow_v_16
        + p_p->c0918z * pow_u_9  *18.0*pow_v_17
        + p_p->c0919z * pow_u_9  *19.0*pow_v_18
        + p_p->c0920z * pow_u_9  *20.0*pow_v_19
        + p_p->c0921z * pow_u_9  *21.0*pow_v_20;
v_z=v_z + p_p->c1000z * pow_u_10  *         0.0        
        + p_p->c1001z * pow_u_10  * 1.0*pow_v_0
        + p_p->c1002z * pow_u_10  * 2.0*pow_v_1
        + p_p->c1003z * pow_u_10  * 3.0*pow_v_2
        + p_p->c1004z * pow_u_10  * 4.0*pow_v_3
        + p_p->c1005z * pow_u_10  * 5.0*pow_v_4
        + p_p->c1006z * pow_u_10  * 6.0*pow_v_5
        + p_p->c1007z * pow_u_10  * 7.0*pow_v_6
        + p_p->c1008z * pow_u_10  * 8.0*pow_v_7
        + p_p->c1009z * pow_u_10  * 9.0*pow_v_8
        + p_p->c1010z * pow_u_10  *10.0*pow_v_9
        + p_p->c1011z * pow_u_10  *11.0*pow_v_10
        + p_p->c1012z * pow_u_10  *12.0*pow_v_11
        + p_p->c1013z * pow_u_10  *13.0*pow_v_12
        + p_p->c1014z * pow_u_10  *14.0*pow_v_13
        + p_p->c1015z * pow_u_10  *15.0*pow_v_14
        + p_p->c1016z * pow_u_10  *16.0*pow_v_15
        + p_p->c1017z * pow_u_10  *17.0*pow_v_16
        + p_p->c1018z * pow_u_10  *18.0*pow_v_17
        + p_p->c1019z * pow_u_10  *19.0*pow_v_18
        + p_p->c1020z * pow_u_10  *20.0*pow_v_19
        + p_p->c1021z * pow_u_10  *21.0*pow_v_20;
v_z=v_z + p_p->c1100z * pow_u_11  *         0.0        
        + p_p->c1101z * pow_u_11  * 1.0*pow_v_0
        + p_p->c1102z * pow_u_11  * 2.0*pow_v_1
        + p_p->c1103z * pow_u_11  * 3.0*pow_v_2
        + p_p->c1104z * pow_u_11  * 4.0*pow_v_3
        + p_p->c1105z * pow_u_11  * 5.0*pow_v_4
        + p_p->c1106z * pow_u_11  * 6.0*pow_v_5
        + p_p->c1107z * pow_u_11  * 7.0*pow_v_6
        + p_p->c1108z * pow_u_11  * 8.0*pow_v_7
        + p_p->c1109z * pow_u_11  * 9.0*pow_v_8
        + p_p->c1110z * pow_u_11  *10.0*pow_v_9
        + p_p->c1111z * pow_u_11  *11.0*pow_v_10
        + p_p->c1112z * pow_u_11  *12.0*pow_v_11
        + p_p->c1113z * pow_u_11  *13.0*pow_v_12
        + p_p->c1114z * pow_u_11  *14.0*pow_v_13
        + p_p->c1115z * pow_u_11  *15.0*pow_v_14
        + p_p->c1116z * pow_u_11  *16.0*pow_v_15
        + p_p->c1117z * pow_u_11  *17.0*pow_v_16
        + p_p->c1118z * pow_u_11  *18.0*pow_v_17
        + p_p->c1119z * pow_u_11  *19.0*pow_v_18
        + p_p->c1120z * pow_u_11  *20.0*pow_v_19
        + p_p->c1121z * pow_u_11  *21.0*pow_v_20;
v_z=v_z + p_p->c1200z * pow_u_12  *         0.0        
        + p_p->c1201z * pow_u_12  * 1.0*pow_v_0
        + p_p->c1202z * pow_u_12  * 2.0*pow_v_1
        + p_p->c1203z * pow_u_12  * 3.0*pow_v_2
        + p_p->c1204z * pow_u_12  * 4.0*pow_v_3
        + p_p->c1205z * pow_u_12  * 5.0*pow_v_4
        + p_p->c1206z * pow_u_12  * 6.0*pow_v_5
        + p_p->c1207z * pow_u_12  * 7.0*pow_v_6
        + p_p->c1208z * pow_u_12  * 8.0*pow_v_7
        + p_p->c1209z * pow_u_12  * 9.0*pow_v_8
        + p_p->c1210z * pow_u_12  *10.0*pow_v_9
        + p_p->c1211z * pow_u_12  *11.0*pow_v_10
        + p_p->c1212z * pow_u_12  *12.0*pow_v_11
        + p_p->c1213z * pow_u_12  *13.0*pow_v_12
        + p_p->c1214z * pow_u_12  *14.0*pow_v_13
        + p_p->c1215z * pow_u_12  *15.0*pow_v_14
        + p_p->c1216z * pow_u_12  *16.0*pow_v_15
        + p_p->c1217z * pow_u_12  *17.0*pow_v_16
        + p_p->c1218z * pow_u_12  *18.0*pow_v_17
        + p_p->c1219z * pow_u_12  *19.0*pow_v_18
        + p_p->c1220z * pow_u_12  *20.0*pow_v_19
        + p_p->c1221z * pow_u_12  *21.0*pow_v_20;
v_z=v_z + p_p->c1300z * pow_u_13  *         0.0        
        + p_p->c1301z * pow_u_13  * 1.0*pow_v_0
        + p_p->c1302z * pow_u_13  * 2.0*pow_v_1
        + p_p->c1303z * pow_u_13  * 3.0*pow_v_2
        + p_p->c1304z * pow_u_13  * 4.0*pow_v_3
        + p_p->c1305z * pow_u_13  * 5.0*pow_v_4
        + p_p->c1306z * pow_u_13  * 6.0*pow_v_5
        + p_p->c1307z * pow_u_13  * 7.0*pow_v_6
        + p_p->c1308z * pow_u_13  * 8.0*pow_v_7
        + p_p->c1309z * pow_u_13  * 9.0*pow_v_8
        + p_p->c1310z * pow_u_13  *10.0*pow_v_9
        + p_p->c1311z * pow_u_13  *11.0*pow_v_10
        + p_p->c1312z * pow_u_13  *12.0*pow_v_11
        + p_p->c1313z * pow_u_13  *13.0*pow_v_12
        + p_p->c1314z * pow_u_13  *14.0*pow_v_13
        + p_p->c1315z * pow_u_13  *15.0*pow_v_14
        + p_p->c1316z * pow_u_13  *16.0*pow_v_15
        + p_p->c1317z * pow_u_13  *17.0*pow_v_16
        + p_p->c1318z * pow_u_13  *18.0*pow_v_17
        + p_p->c1319z * pow_u_13  *19.0*pow_v_18
        + p_p->c1320z * pow_u_13  *20.0*pow_v_19
        + p_p->c1321z * pow_u_13  *21.0*pow_v_20;
v_z=v_z + p_p->c1400z * pow_u_14  *         0.0        
        + p_p->c1401z * pow_u_14  * 1.0*pow_v_0
        + p_p->c1402z * pow_u_14  * 2.0*pow_v_1
        + p_p->c1403z * pow_u_14  * 3.0*pow_v_2
        + p_p->c1404z * pow_u_14  * 4.0*pow_v_3
        + p_p->c1405z * pow_u_14  * 5.0*pow_v_4
        + p_p->c1406z * pow_u_14  * 6.0*pow_v_5
        + p_p->c1407z * pow_u_14  * 7.0*pow_v_6
        + p_p->c1408z * pow_u_14  * 8.0*pow_v_7
        + p_p->c1409z * pow_u_14  * 9.0*pow_v_8
        + p_p->c1410z * pow_u_14  *10.0*pow_v_9
        + p_p->c1411z * pow_u_14  *11.0*pow_v_10
        + p_p->c1412z * pow_u_14  *12.0*pow_v_11
        + p_p->c1413z * pow_u_14  *13.0*pow_v_12
        + p_p->c1414z * pow_u_14  *14.0*pow_v_13
        + p_p->c1415z * pow_u_14  *15.0*pow_v_14
        + p_p->c1416z * pow_u_14  *16.0*pow_v_15
        + p_p->c1417z * pow_u_14  *17.0*pow_v_16
        + p_p->c1418z * pow_u_14  *18.0*pow_v_17
        + p_p->c1419z * pow_u_14  *19.0*pow_v_18
        + p_p->c1420z * pow_u_14  *20.0*pow_v_19
        + p_p->c1421z * pow_u_14  *21.0*pow_v_20;
v_z=v_z + p_p->c1500z * pow_u_15  *         0.0        
        + p_p->c1501z * pow_u_15  * 1.0*pow_v_0
        + p_p->c1502z * pow_u_15  * 2.0*pow_v_1
        + p_p->c1503z * pow_u_15  * 3.0*pow_v_2
        + p_p->c1504z * pow_u_15  * 4.0*pow_v_3
        + p_p->c1505z * pow_u_15  * 5.0*pow_v_4
        + p_p->c1506z * pow_u_15  * 6.0*pow_v_5
        + p_p->c1507z * pow_u_15  * 7.0*pow_v_6
        + p_p->c1508z * pow_u_15  * 8.0*pow_v_7
        + p_p->c1509z * pow_u_15  * 9.0*pow_v_8
        + p_p->c1510z * pow_u_15  *10.0*pow_v_9
        + p_p->c1511z * pow_u_15  *11.0*pow_v_10
        + p_p->c1512z * pow_u_15  *12.0*pow_v_11
        + p_p->c1513z * pow_u_15  *13.0*pow_v_12
        + p_p->c1514z * pow_u_15  *14.0*pow_v_13
        + p_p->c1515z * pow_u_15  *15.0*pow_v_14
        + p_p->c1516z * pow_u_15  *16.0*pow_v_15
        + p_p->c1517z * pow_u_15  *17.0*pow_v_16
        + p_p->c1518z * pow_u_15  *18.0*pow_v_17
        + p_p->c1519z * pow_u_15  *19.0*pow_v_18
        + p_p->c1520z * pow_u_15  *20.0*pow_v_19
        + p_p->c1521z * pow_u_15  *21.0*pow_v_20;
v_z=v_z + p_p->c1600z * pow_u_16  *         0.0        
        + p_p->c1601z * pow_u_16  * 1.0*pow_v_0
        + p_p->c1602z * pow_u_16  * 2.0*pow_v_1
        + p_p->c1603z * pow_u_16  * 3.0*pow_v_2
        + p_p->c1604z * pow_u_16  * 4.0*pow_v_3
        + p_p->c1605z * pow_u_16  * 5.0*pow_v_4
        + p_p->c1606z * pow_u_16  * 6.0*pow_v_5
        + p_p->c1607z * pow_u_16  * 7.0*pow_v_6
        + p_p->c1608z * pow_u_16  * 8.0*pow_v_7
        + p_p->c1609z * pow_u_16  * 9.0*pow_v_8
        + p_p->c1610z * pow_u_16  *10.0*pow_v_9
        + p_p->c1611z * pow_u_16  *11.0*pow_v_10
        + p_p->c1612z * pow_u_16  *12.0*pow_v_11
        + p_p->c1613z * pow_u_16  *13.0*pow_v_12
        + p_p->c1614z * pow_u_16  *14.0*pow_v_13
        + p_p->c1615z * pow_u_16  *15.0*pow_v_14
        + p_p->c1616z * pow_u_16  *16.0*pow_v_15
        + p_p->c1617z * pow_u_16  *17.0*pow_v_16
        + p_p->c1618z * pow_u_16  *18.0*pow_v_17
        + p_p->c1619z * pow_u_16  *19.0*pow_v_18
        + p_p->c1620z * pow_u_16  *20.0*pow_v_19
        + p_p->c1621z * pow_u_16  *21.0*pow_v_20;
v_z=v_z + p_p->c1700z * pow_u_17  *         0.0        
        + p_p->c1701z * pow_u_17  * 1.0*pow_v_0
        + p_p->c1702z * pow_u_17  * 2.0*pow_v_1
        + p_p->c1703z * pow_u_17  * 3.0*pow_v_2
        + p_p->c1704z * pow_u_17  * 4.0*pow_v_3
        + p_p->c1705z * pow_u_17  * 5.0*pow_v_4
        + p_p->c1706z * pow_u_17  * 6.0*pow_v_5
        + p_p->c1707z * pow_u_17  * 7.0*pow_v_6
        + p_p->c1708z * pow_u_17  * 8.0*pow_v_7
        + p_p->c1709z * pow_u_17  * 9.0*pow_v_8
        + p_p->c1710z * pow_u_17  *10.0*pow_v_9
        + p_p->c1711z * pow_u_17  *11.0*pow_v_10
        + p_p->c1712z * pow_u_17  *12.0*pow_v_11
        + p_p->c1713z * pow_u_17  *13.0*pow_v_12
        + p_p->c1714z * pow_u_17  *14.0*pow_v_13
        + p_p->c1715z * pow_u_17  *15.0*pow_v_14
        + p_p->c1716z * pow_u_17  *16.0*pow_v_15
        + p_p->c1717z * pow_u_17  *17.0*pow_v_16
        + p_p->c1718z * pow_u_17  *18.0*pow_v_17
        + p_p->c1719z * pow_u_17  *19.0*pow_v_18
        + p_p->c1720z * pow_u_17  *20.0*pow_v_19
        + p_p->c1721z * pow_u_17  *21.0*pow_v_20;
v_z=v_z + p_p->c1800z * pow_u_18  *         0.0        
        + p_p->c1801z * pow_u_18  * 1.0*pow_v_0
        + p_p->c1802z * pow_u_18  * 2.0*pow_v_1
        + p_p->c1803z * pow_u_18  * 3.0*pow_v_2
        + p_p->c1804z * pow_u_18  * 4.0*pow_v_3
        + p_p->c1805z * pow_u_18  * 5.0*pow_v_4
        + p_p->c1806z * pow_u_18  * 6.0*pow_v_5
        + p_p->c1807z * pow_u_18  * 7.0*pow_v_6
        + p_p->c1808z * pow_u_18  * 8.0*pow_v_7
        + p_p->c1809z * pow_u_18  * 9.0*pow_v_8
        + p_p->c1810z * pow_u_18  *10.0*pow_v_9
        + p_p->c1811z * pow_u_18  *11.0*pow_v_10
        + p_p->c1812z * pow_u_18  *12.0*pow_v_11
        + p_p->c1813z * pow_u_18  *13.0*pow_v_12
        + p_p->c1814z * pow_u_18  *14.0*pow_v_13
        + p_p->c1815z * pow_u_18  *15.0*pow_v_14
        + p_p->c1816z * pow_u_18  *16.0*pow_v_15
        + p_p->c1817z * pow_u_18  *17.0*pow_v_16
        + p_p->c1818z * pow_u_18  *18.0*pow_v_17
        + p_p->c1819z * pow_u_18  *19.0*pow_v_18
        + p_p->c1820z * pow_u_18  *20.0*pow_v_19
        + p_p->c1821z * pow_u_18  *21.0*pow_v_20;
v_z=v_z + p_p->c1900z * pow_u_19  *         0.0        
        + p_p->c1901z * pow_u_19  * 1.0*pow_v_0
        + p_p->c1902z * pow_u_19  * 2.0*pow_v_1
        + p_p->c1903z * pow_u_19  * 3.0*pow_v_2
        + p_p->c1904z * pow_u_19  * 4.0*pow_v_3
        + p_p->c1905z * pow_u_19  * 5.0*pow_v_4
        + p_p->c1906z * pow_u_19  * 6.0*pow_v_5
        + p_p->c1907z * pow_u_19  * 7.0*pow_v_6
        + p_p->c1908z * pow_u_19  * 8.0*pow_v_7
        + p_p->c1909z * pow_u_19  * 9.0*pow_v_8
        + p_p->c1910z * pow_u_19  *10.0*pow_v_9
        + p_p->c1911z * pow_u_19  *11.0*pow_v_10
        + p_p->c1912z * pow_u_19  *12.0*pow_v_11
        + p_p->c1913z * pow_u_19  *13.0*pow_v_12
        + p_p->c1914z * pow_u_19  *14.0*pow_v_13
        + p_p->c1915z * pow_u_19  *15.0*pow_v_14
        + p_p->c1916z * pow_u_19  *16.0*pow_v_15
        + p_p->c1917z * pow_u_19  *17.0*pow_v_16
        + p_p->c1918z * pow_u_19  *18.0*pow_v_17
        + p_p->c1919z * pow_u_19  *19.0*pow_v_18
        + p_p->c1920z * pow_u_19  *20.0*pow_v_19
        + p_p->c1921z * pow_u_19  *21.0*pow_v_20;
v_z=v_z + p_p->c2000z * pow_u_20  *         0.0        
        + p_p->c2001z * pow_u_20  * 1.0*pow_v_0
        + p_p->c2002z * pow_u_20  * 2.0*pow_v_1
        + p_p->c2003z * pow_u_20  * 3.0*pow_v_2
        + p_p->c2004z * pow_u_20  * 4.0*pow_v_3
        + p_p->c2005z * pow_u_20  * 5.0*pow_v_4
        + p_p->c2006z * pow_u_20  * 6.0*pow_v_5
        + p_p->c2007z * pow_u_20  * 7.0*pow_v_6
        + p_p->c2008z * pow_u_20  * 8.0*pow_v_7
        + p_p->c2009z * pow_u_20  * 9.0*pow_v_8
        + p_p->c2010z * pow_u_20  *10.0*pow_v_9
        + p_p->c2011z * pow_u_20  *11.0*pow_v_10
        + p_p->c2012z * pow_u_20  *12.0*pow_v_11
        + p_p->c2013z * pow_u_20  *13.0*pow_v_12
        + p_p->c2014z * pow_u_20  *14.0*pow_v_13
        + p_p->c2015z * pow_u_20  *15.0*pow_v_14
        + p_p->c2016z * pow_u_20  *16.0*pow_v_15
        + p_p->c2017z * pow_u_20  *17.0*pow_v_16
        + p_p->c2018z * pow_u_20  *18.0*pow_v_17
        + p_p->c2019z * pow_u_20  *19.0*pow_v_18
        + p_p->c2020z * pow_u_20  *20.0*pow_v_19
        + p_p->c2021z * pow_u_20  *21.0*pow_v_20;
v_z=v_z + p_p->c2100z * pow_u_21  *         0.0        
        + p_p->c2101z * pow_u_21  * 1.0*pow_v_0
        + p_p->c2102z * pow_u_21  * 2.0*pow_v_1
        + p_p->c2103z * pow_u_21  * 3.0*pow_v_2
        + p_p->c2104z * pow_u_21  * 4.0*pow_v_3
        + p_p->c2105z * pow_u_21  * 5.0*pow_v_4
        + p_p->c2106z * pow_u_21  * 6.0*pow_v_5
        + p_p->c2107z * pow_u_21  * 7.0*pow_v_6
        + p_p->c2108z * pow_u_21  * 8.0*pow_v_7
        + p_p->c2109z * pow_u_21  * 9.0*pow_v_8
        + p_p->c2110z * pow_u_21  *10.0*pow_v_9
        + p_p->c2111z * pow_u_21  *11.0*pow_v_10
        + p_p->c2112z * pow_u_21  *12.0*pow_v_11
        + p_p->c2113z * pow_u_21  *13.0*pow_v_12
        + p_p->c2114z * pow_u_21  *14.0*pow_v_13
        + p_p->c2115z * pow_u_21  *15.0*pow_v_14
        + p_p->c2116z * pow_u_21  *16.0*pow_v_15
        + p_p->c2117z * pow_u_21  *17.0*pow_v_16
        + p_p->c2118z * pow_u_21  *18.0*pow_v_17
        + p_p->c2119z * pow_u_21  *19.0*pow_v_18
        + p_p->c2120z * pow_u_21  *20.0*pow_v_19
        + p_p->c2121z * pow_u_21  *21.0*pow_v_20;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_drdv  Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ** c_second ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the second derivatives for GMPATP21      */

   static short c_second ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP21  *p_p;       /* Patch GMPATP21                    (ptr) */
   DBfloat    u_l;       /* Patch (local) U parameter value         */
   DBfloat    v_l;       /* Patch (local) V parameter value         */

/* Out:                                                             */
/*  Second derivatives u2_x,u2_y,u2_z,v2_x,v2_y,v2_z,uv_x,uv_y,uv_z */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_second p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l );
}
#endif


u2_x    = p_p->c0000x*            0.0           * pow_v_0  
        + p_p->c0001x*            0.0           * pow_v_1  
        + p_p->c0002x*            0.0           * pow_v_2  
        + p_p->c0003x*            0.0           * pow_v_3  
        + p_p->c0004x*            0.0           * pow_v_4  
        + p_p->c0005x*            0.0           * pow_v_5  
        + p_p->c0006x*            0.0           * pow_v_6  
        + p_p->c0007x*            0.0           * pow_v_7  
        + p_p->c0008x*            0.0           * pow_v_8  
        + p_p->c0009x*            0.0           * pow_v_9  
        + p_p->c0010x*            0.0           * pow_v_10  
        + p_p->c0011x*            0.0           * pow_v_11  
        + p_p->c0012x*            0.0           * pow_v_12  
        + p_p->c0013x*            0.0           * pow_v_13  
        + p_p->c0014x*            0.0           * pow_v_14  
        + p_p->c0015x*            0.0           * pow_v_15  
        + p_p->c0016x*            0.0           * pow_v_16  
        + p_p->c0017x*            0.0           * pow_v_17  
        + p_p->c0018x*            0.0           * pow_v_18  
        + p_p->c0019x*            0.0           * pow_v_19  
        + p_p->c0020x*            0.0           * pow_v_20  
        + p_p->c0021x*            0.0           * pow_v_21  ;
u2_x=u2_x + p_p->c0100x*            0.0           * pow_v_0  
        + p_p->c0101x*            0.0           * pow_v_1  
        + p_p->c0102x*            0.0           * pow_v_2  
        + p_p->c0103x*            0.0           * pow_v_3  
        + p_p->c0104x*            0.0           * pow_v_4  
        + p_p->c0105x*            0.0           * pow_v_5  
        + p_p->c0106x*            0.0           * pow_v_6  
        + p_p->c0107x*            0.0           * pow_v_7  
        + p_p->c0108x*            0.0           * pow_v_8  
        + p_p->c0109x*            0.0           * pow_v_9  
        + p_p->c0110x*            0.0           * pow_v_10  
        + p_p->c0111x*            0.0           * pow_v_11  
        + p_p->c0112x*            0.0           * pow_v_12  
        + p_p->c0113x*            0.0           * pow_v_13  
        + p_p->c0114x*            0.0           * pow_v_14  
        + p_p->c0115x*            0.0           * pow_v_15  
        + p_p->c0116x*            0.0           * pow_v_16  
        + p_p->c0117x*            0.0           * pow_v_17  
        + p_p->c0118x*            0.0           * pow_v_18  
        + p_p->c0119x*            0.0           * pow_v_19  
        + p_p->c0120x*            0.0           * pow_v_20  
        + p_p->c0121x*            0.0           * pow_v_21  ;
u2_x=u2_x + p_p->c0200x* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201x* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202x* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203x* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204x* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205x* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206x* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207x* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0208x* 1.0* 2.0*pow_u_0 * pow_v_8  
        + p_p->c0209x* 1.0* 2.0*pow_u_0 * pow_v_9  
        + p_p->c0210x* 1.0* 2.0*pow_u_0 * pow_v_10  
        + p_p->c0211x* 1.0* 2.0*pow_u_0 * pow_v_11  
        + p_p->c0212x* 1.0* 2.0*pow_u_0 * pow_v_12  
        + p_p->c0213x* 1.0* 2.0*pow_u_0 * pow_v_13  
        + p_p->c0214x* 1.0* 2.0*pow_u_0 * pow_v_14  
        + p_p->c0215x* 1.0* 2.0*pow_u_0 * pow_v_15  
        + p_p->c0216x* 1.0* 2.0*pow_u_0 * pow_v_16  
        + p_p->c0217x* 1.0* 2.0*pow_u_0 * pow_v_17  
        + p_p->c0218x* 1.0* 2.0*pow_u_0 * pow_v_18  
        + p_p->c0219x* 1.0* 2.0*pow_u_0 * pow_v_19  
        + p_p->c0220x* 1.0* 2.0*pow_u_0 * pow_v_20  
        + p_p->c0221x* 1.0* 2.0*pow_u_0 * pow_v_21  ;
u2_x=u2_x + p_p->c0300x* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301x* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302x* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303x* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304x* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305x* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306x* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307x* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0308x* 2.0* 3.0*pow_u_1 * pow_v_8  
        + p_p->c0309x* 2.0* 3.0*pow_u_1 * pow_v_9  
        + p_p->c0310x* 2.0* 3.0*pow_u_1 * pow_v_10  
        + p_p->c0311x* 2.0* 3.0*pow_u_1 * pow_v_11  
        + p_p->c0312x* 2.0* 3.0*pow_u_1 * pow_v_12  
        + p_p->c0313x* 2.0* 3.0*pow_u_1 * pow_v_13  
        + p_p->c0314x* 2.0* 3.0*pow_u_1 * pow_v_14  
        + p_p->c0315x* 2.0* 3.0*pow_u_1 * pow_v_15  
        + p_p->c0316x* 2.0* 3.0*pow_u_1 * pow_v_16  
        + p_p->c0317x* 2.0* 3.0*pow_u_1 * pow_v_17  
        + p_p->c0318x* 2.0* 3.0*pow_u_1 * pow_v_18  
        + p_p->c0319x* 2.0* 3.0*pow_u_1 * pow_v_19  
        + p_p->c0320x* 2.0* 3.0*pow_u_1 * pow_v_20  
        + p_p->c0321x* 2.0* 3.0*pow_u_1 * pow_v_21  ;
u2_x=u2_x + p_p->c0400x* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401x* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402x* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403x* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404x* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405x* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406x* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407x* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0408x* 3.0* 4.0*pow_u_2 * pow_v_8  
        + p_p->c0409x* 3.0* 4.0*pow_u_2 * pow_v_9  
        + p_p->c0410x* 3.0* 4.0*pow_u_2 * pow_v_10  
        + p_p->c0411x* 3.0* 4.0*pow_u_2 * pow_v_11  
        + p_p->c0412x* 3.0* 4.0*pow_u_2 * pow_v_12  
        + p_p->c0413x* 3.0* 4.0*pow_u_2 * pow_v_13  
        + p_p->c0414x* 3.0* 4.0*pow_u_2 * pow_v_14  
        + p_p->c0415x* 3.0* 4.0*pow_u_2 * pow_v_15  
        + p_p->c0416x* 3.0* 4.0*pow_u_2 * pow_v_16  
        + p_p->c0417x* 3.0* 4.0*pow_u_2 * pow_v_17  
        + p_p->c0418x* 3.0* 4.0*pow_u_2 * pow_v_18  
        + p_p->c0419x* 3.0* 4.0*pow_u_2 * pow_v_19  
        + p_p->c0420x* 3.0* 4.0*pow_u_2 * pow_v_20  
        + p_p->c0421x* 3.0* 4.0*pow_u_2 * pow_v_21  ;
u2_x=u2_x + p_p->c0500x* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501x* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502x* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503x* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504x* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505x* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506x* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507x* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0508x* 4.0* 5.0*pow_u_3 * pow_v_8  
        + p_p->c0509x* 4.0* 5.0*pow_u_3 * pow_v_9  
        + p_p->c0510x* 4.0* 5.0*pow_u_3 * pow_v_10  
        + p_p->c0511x* 4.0* 5.0*pow_u_3 * pow_v_11  
        + p_p->c0512x* 4.0* 5.0*pow_u_3 * pow_v_12  
        + p_p->c0513x* 4.0* 5.0*pow_u_3 * pow_v_13  
        + p_p->c0514x* 4.0* 5.0*pow_u_3 * pow_v_14  
        + p_p->c0515x* 4.0* 5.0*pow_u_3 * pow_v_15  
        + p_p->c0516x* 4.0* 5.0*pow_u_3 * pow_v_16  
        + p_p->c0517x* 4.0* 5.0*pow_u_3 * pow_v_17  
        + p_p->c0518x* 4.0* 5.0*pow_u_3 * pow_v_18  
        + p_p->c0519x* 4.0* 5.0*pow_u_3 * pow_v_19  
        + p_p->c0520x* 4.0* 5.0*pow_u_3 * pow_v_20  
        + p_p->c0521x* 4.0* 5.0*pow_u_3 * pow_v_21  ;
u2_x=u2_x + p_p->c0600x* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601x* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602x* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603x* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604x* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605x* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606x* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607x* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0608x* 5.0* 6.0*pow_u_4 * pow_v_8  
        + p_p->c0609x* 5.0* 6.0*pow_u_4 * pow_v_9  
        + p_p->c0610x* 5.0* 6.0*pow_u_4 * pow_v_10  
        + p_p->c0611x* 5.0* 6.0*pow_u_4 * pow_v_11  
        + p_p->c0612x* 5.0* 6.0*pow_u_4 * pow_v_12  
        + p_p->c0613x* 5.0* 6.0*pow_u_4 * pow_v_13  
        + p_p->c0614x* 5.0* 6.0*pow_u_4 * pow_v_14  
        + p_p->c0615x* 5.0* 6.0*pow_u_4 * pow_v_15  
        + p_p->c0616x* 5.0* 6.0*pow_u_4 * pow_v_16  
        + p_p->c0617x* 5.0* 6.0*pow_u_4 * pow_v_17  
        + p_p->c0618x* 5.0* 6.0*pow_u_4 * pow_v_18  
        + p_p->c0619x* 5.0* 6.0*pow_u_4 * pow_v_19  
        + p_p->c0620x* 5.0* 6.0*pow_u_4 * pow_v_20  
        + p_p->c0621x* 5.0* 6.0*pow_u_4 * pow_v_21  ;
u2_x=u2_x + p_p->c0700x* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701x* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702x* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703x* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704x* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705x* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706x* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707x* 6.0* 7.0*pow_u_5 * pow_v_7  
        + p_p->c0708x* 6.0* 7.0*pow_u_5 * pow_v_8  
        + p_p->c0709x* 6.0* 7.0*pow_u_5 * pow_v_9  
        + p_p->c0710x* 6.0* 7.0*pow_u_5 * pow_v_10  
        + p_p->c0711x* 6.0* 7.0*pow_u_5 * pow_v_11  
        + p_p->c0712x* 6.0* 7.0*pow_u_5 * pow_v_12  
        + p_p->c0713x* 6.0* 7.0*pow_u_5 * pow_v_13  
        + p_p->c0714x* 6.0* 7.0*pow_u_5 * pow_v_14  
        + p_p->c0715x* 6.0* 7.0*pow_u_5 * pow_v_15  
        + p_p->c0716x* 6.0* 7.0*pow_u_5 * pow_v_16  
        + p_p->c0717x* 6.0* 7.0*pow_u_5 * pow_v_17  
        + p_p->c0718x* 6.0* 7.0*pow_u_5 * pow_v_18  
        + p_p->c0719x* 6.0* 7.0*pow_u_5 * pow_v_19  
        + p_p->c0720x* 6.0* 7.0*pow_u_5 * pow_v_20  
        + p_p->c0721x* 6.0* 7.0*pow_u_5 * pow_v_21  ;
u2_x=u2_x + p_p->c0800x* 7.0* 8.0*pow_u_6 * pow_v_0  
        + p_p->c0801x* 7.0* 8.0*pow_u_6 * pow_v_1  
        + p_p->c0802x* 7.0* 8.0*pow_u_6 * pow_v_2  
        + p_p->c0803x* 7.0* 8.0*pow_u_6 * pow_v_3  
        + p_p->c0804x* 7.0* 8.0*pow_u_6 * pow_v_4  
        + p_p->c0805x* 7.0* 8.0*pow_u_6 * pow_v_5  
        + p_p->c0806x* 7.0* 8.0*pow_u_6 * pow_v_6  
        + p_p->c0807x* 7.0* 8.0*pow_u_6 * pow_v_7  
        + p_p->c0808x* 7.0* 8.0*pow_u_6 * pow_v_8  
        + p_p->c0809x* 7.0* 8.0*pow_u_6 * pow_v_9  
        + p_p->c0810x* 7.0* 8.0*pow_u_6 * pow_v_10  
        + p_p->c0811x* 7.0* 8.0*pow_u_6 * pow_v_11  
        + p_p->c0812x* 7.0* 8.0*pow_u_6 * pow_v_12  
        + p_p->c0813x* 7.0* 8.0*pow_u_6 * pow_v_13  
        + p_p->c0814x* 7.0* 8.0*pow_u_6 * pow_v_14  
        + p_p->c0815x* 7.0* 8.0*pow_u_6 * pow_v_15  
        + p_p->c0816x* 7.0* 8.0*pow_u_6 * pow_v_16  
        + p_p->c0817x* 7.0* 8.0*pow_u_6 * pow_v_17  
        + p_p->c0818x* 7.0* 8.0*pow_u_6 * pow_v_18  
        + p_p->c0819x* 7.0* 8.0*pow_u_6 * pow_v_19  
        + p_p->c0820x* 7.0* 8.0*pow_u_6 * pow_v_20  
        + p_p->c0821x* 7.0* 8.0*pow_u_6 * pow_v_21  ;
u2_x=u2_x + p_p->c0900x* 8.0* 9.0*pow_u_7 * pow_v_0  
        + p_p->c0901x* 8.0* 9.0*pow_u_7 * pow_v_1  
        + p_p->c0902x* 8.0* 9.0*pow_u_7 * pow_v_2  
        + p_p->c0903x* 8.0* 9.0*pow_u_7 * pow_v_3  
        + p_p->c0904x* 8.0* 9.0*pow_u_7 * pow_v_4  
        + p_p->c0905x* 8.0* 9.0*pow_u_7 * pow_v_5  
        + p_p->c0906x* 8.0* 9.0*pow_u_7 * pow_v_6  
        + p_p->c0907x* 8.0* 9.0*pow_u_7 * pow_v_7  
        + p_p->c0908x* 8.0* 9.0*pow_u_7 * pow_v_8  
        + p_p->c0909x* 8.0* 9.0*pow_u_7 * pow_v_9  
        + p_p->c0910x* 8.0* 9.0*pow_u_7 * pow_v_10  
        + p_p->c0911x* 8.0* 9.0*pow_u_7 * pow_v_11  
        + p_p->c0912x* 8.0* 9.0*pow_u_7 * pow_v_12  
        + p_p->c0913x* 8.0* 9.0*pow_u_7 * pow_v_13  
        + p_p->c0914x* 8.0* 9.0*pow_u_7 * pow_v_14  
        + p_p->c0915x* 8.0* 9.0*pow_u_7 * pow_v_15  
        + p_p->c0916x* 8.0* 9.0*pow_u_7 * pow_v_16  
        + p_p->c0917x* 8.0* 9.0*pow_u_7 * pow_v_17  
        + p_p->c0918x* 8.0* 9.0*pow_u_7 * pow_v_18  
        + p_p->c0919x* 8.0* 9.0*pow_u_7 * pow_v_19  
        + p_p->c0920x* 8.0* 9.0*pow_u_7 * pow_v_20  
        + p_p->c0921x* 8.0* 9.0*pow_u_7 * pow_v_21  ;
u2_x=u2_x + p_p->c1000x* 9.0*10.0*pow_u_8 * pow_v_0  
        + p_p->c1001x* 9.0*10.0*pow_u_8 * pow_v_1  
        + p_p->c1002x* 9.0*10.0*pow_u_8 * pow_v_2  
        + p_p->c1003x* 9.0*10.0*pow_u_8 * pow_v_3  
        + p_p->c1004x* 9.0*10.0*pow_u_8 * pow_v_4  
        + p_p->c1005x* 9.0*10.0*pow_u_8 * pow_v_5  
        + p_p->c1006x* 9.0*10.0*pow_u_8 * pow_v_6  
        + p_p->c1007x* 9.0*10.0*pow_u_8 * pow_v_7  
        + p_p->c1008x* 9.0*10.0*pow_u_8 * pow_v_8  
        + p_p->c1009x* 9.0*10.0*pow_u_8 * pow_v_9  
        + p_p->c1010x* 9.0*10.0*pow_u_8 * pow_v_10  
        + p_p->c1011x* 9.0*10.0*pow_u_8 * pow_v_11  
        + p_p->c1012x* 9.0*10.0*pow_u_8 * pow_v_12  
        + p_p->c1013x* 9.0*10.0*pow_u_8 * pow_v_13  
        + p_p->c1014x* 9.0*10.0*pow_u_8 * pow_v_14  
        + p_p->c1015x* 9.0*10.0*pow_u_8 * pow_v_15  
        + p_p->c1016x* 9.0*10.0*pow_u_8 * pow_v_16  
        + p_p->c1017x* 9.0*10.0*pow_u_8 * pow_v_17  
        + p_p->c1018x* 9.0*10.0*pow_u_8 * pow_v_18  
        + p_p->c1019x* 9.0*10.0*pow_u_8 * pow_v_19  
        + p_p->c1020x* 9.0*10.0*pow_u_8 * pow_v_20  
        + p_p->c1021x* 9.0*10.0*pow_u_8 * pow_v_21  ;
u2_x=u2_x + p_p->c1100x*10.0*11.0*pow_u_9 * pow_v_0  
        + p_p->c1101x*10.0*11.0*pow_u_9 * pow_v_1  
        + p_p->c1102x*10.0*11.0*pow_u_9 * pow_v_2  
        + p_p->c1103x*10.0*11.0*pow_u_9 * pow_v_3  
        + p_p->c1104x*10.0*11.0*pow_u_9 * pow_v_4  
        + p_p->c1105x*10.0*11.0*pow_u_9 * pow_v_5  
        + p_p->c1106x*10.0*11.0*pow_u_9 * pow_v_6  
        + p_p->c1107x*10.0*11.0*pow_u_9 * pow_v_7  
        + p_p->c1108x*10.0*11.0*pow_u_9 * pow_v_8  
        + p_p->c1109x*10.0*11.0*pow_u_9 * pow_v_9  
        + p_p->c1110x*10.0*11.0*pow_u_9 * pow_v_10  
        + p_p->c1111x*10.0*11.0*pow_u_9 * pow_v_11  
        + p_p->c1112x*10.0*11.0*pow_u_9 * pow_v_12  
        + p_p->c1113x*10.0*11.0*pow_u_9 * pow_v_13  
        + p_p->c1114x*10.0*11.0*pow_u_9 * pow_v_14  
        + p_p->c1115x*10.0*11.0*pow_u_9 * pow_v_15  
        + p_p->c1116x*10.0*11.0*pow_u_9 * pow_v_16  
        + p_p->c1117x*10.0*11.0*pow_u_9 * pow_v_17  
        + p_p->c1118x*10.0*11.0*pow_u_9 * pow_v_18  
        + p_p->c1119x*10.0*11.0*pow_u_9 * pow_v_19  
        + p_p->c1120x*10.0*11.0*pow_u_9 * pow_v_20  
        + p_p->c1121x*10.0*11.0*pow_u_9 * pow_v_21  ;
u2_x=u2_x + p_p->c1200x*11.0*12.0*pow_u_10 * pow_v_0  
        + p_p->c1201x*11.0*12.0*pow_u_10 * pow_v_1  
        + p_p->c1202x*11.0*12.0*pow_u_10 * pow_v_2  
        + p_p->c1203x*11.0*12.0*pow_u_10 * pow_v_3  
        + p_p->c1204x*11.0*12.0*pow_u_10 * pow_v_4  
        + p_p->c1205x*11.0*12.0*pow_u_10 * pow_v_5  
        + p_p->c1206x*11.0*12.0*pow_u_10 * pow_v_6  
        + p_p->c1207x*11.0*12.0*pow_u_10 * pow_v_7  
        + p_p->c1208x*11.0*12.0*pow_u_10 * pow_v_8  
        + p_p->c1209x*11.0*12.0*pow_u_10 * pow_v_9  
        + p_p->c1210x*11.0*12.0*pow_u_10 * pow_v_10  
        + p_p->c1211x*11.0*12.0*pow_u_10 * pow_v_11  
        + p_p->c1212x*11.0*12.0*pow_u_10 * pow_v_12  
        + p_p->c1213x*11.0*12.0*pow_u_10 * pow_v_13  
        + p_p->c1214x*11.0*12.0*pow_u_10 * pow_v_14  
        + p_p->c1215x*11.0*12.0*pow_u_10 * pow_v_15  
        + p_p->c1216x*11.0*12.0*pow_u_10 * pow_v_16  
        + p_p->c1217x*11.0*12.0*pow_u_10 * pow_v_17  
        + p_p->c1218x*11.0*12.0*pow_u_10 * pow_v_18  
        + p_p->c1219x*11.0*12.0*pow_u_10 * pow_v_19  
        + p_p->c1220x*11.0*12.0*pow_u_10 * pow_v_20  
        + p_p->c1221x*11.0*12.0*pow_u_10 * pow_v_21  ;
u2_x=u2_x + p_p->c1300x*12.0*13.0*pow_u_11 * pow_v_0  
        + p_p->c1301x*12.0*13.0*pow_u_11 * pow_v_1  
        + p_p->c1302x*12.0*13.0*pow_u_11 * pow_v_2  
        + p_p->c1303x*12.0*13.0*pow_u_11 * pow_v_3  
        + p_p->c1304x*12.0*13.0*pow_u_11 * pow_v_4  
        + p_p->c1305x*12.0*13.0*pow_u_11 * pow_v_5  
        + p_p->c1306x*12.0*13.0*pow_u_11 * pow_v_6  
        + p_p->c1307x*12.0*13.0*pow_u_11 * pow_v_7  
        + p_p->c1308x*12.0*13.0*pow_u_11 * pow_v_8  
        + p_p->c1309x*12.0*13.0*pow_u_11 * pow_v_9  
        + p_p->c1310x*12.0*13.0*pow_u_11 * pow_v_10  
        + p_p->c1311x*12.0*13.0*pow_u_11 * pow_v_11  
        + p_p->c1312x*12.0*13.0*pow_u_11 * pow_v_12  
        + p_p->c1313x*12.0*13.0*pow_u_11 * pow_v_13  
        + p_p->c1314x*12.0*13.0*pow_u_11 * pow_v_14  
        + p_p->c1315x*12.0*13.0*pow_u_11 * pow_v_15  
        + p_p->c1316x*12.0*13.0*pow_u_11 * pow_v_16  
        + p_p->c1317x*12.0*13.0*pow_u_11 * pow_v_17  
        + p_p->c1318x*12.0*13.0*pow_u_11 * pow_v_18  
        + p_p->c1319x*12.0*13.0*pow_u_11 * pow_v_19  
        + p_p->c1320x*12.0*13.0*pow_u_11 * pow_v_20  
        + p_p->c1321x*12.0*13.0*pow_u_11 * pow_v_21  ;
u2_x=u2_x + p_p->c1400x*13.0*14.0*pow_u_12 * pow_v_0  
        + p_p->c1401x*13.0*14.0*pow_u_12 * pow_v_1  
        + p_p->c1402x*13.0*14.0*pow_u_12 * pow_v_2  
        + p_p->c1403x*13.0*14.0*pow_u_12 * pow_v_3  
        + p_p->c1404x*13.0*14.0*pow_u_12 * pow_v_4  
        + p_p->c1405x*13.0*14.0*pow_u_12 * pow_v_5  
        + p_p->c1406x*13.0*14.0*pow_u_12 * pow_v_6  
        + p_p->c1407x*13.0*14.0*pow_u_12 * pow_v_7  
        + p_p->c1408x*13.0*14.0*pow_u_12 * pow_v_8  
        + p_p->c1409x*13.0*14.0*pow_u_12 * pow_v_9  
        + p_p->c1410x*13.0*14.0*pow_u_12 * pow_v_10  
        + p_p->c1411x*13.0*14.0*pow_u_12 * pow_v_11  
        + p_p->c1412x*13.0*14.0*pow_u_12 * pow_v_12  
        + p_p->c1413x*13.0*14.0*pow_u_12 * pow_v_13  
        + p_p->c1414x*13.0*14.0*pow_u_12 * pow_v_14  
        + p_p->c1415x*13.0*14.0*pow_u_12 * pow_v_15  
        + p_p->c1416x*13.0*14.0*pow_u_12 * pow_v_16  
        + p_p->c1417x*13.0*14.0*pow_u_12 * pow_v_17  
        + p_p->c1418x*13.0*14.0*pow_u_12 * pow_v_18  
        + p_p->c1419x*13.0*14.0*pow_u_12 * pow_v_19  
        + p_p->c1420x*13.0*14.0*pow_u_12 * pow_v_20  
        + p_p->c1421x*13.0*14.0*pow_u_12 * pow_v_21  ;
u2_x=u2_x + p_p->c1500x*14.0*15.0*pow_u_13 * pow_v_0  
        + p_p->c1501x*14.0*15.0*pow_u_13 * pow_v_1  
        + p_p->c1502x*14.0*15.0*pow_u_13 * pow_v_2  
        + p_p->c1503x*14.0*15.0*pow_u_13 * pow_v_3  
        + p_p->c1504x*14.0*15.0*pow_u_13 * pow_v_4  
        + p_p->c1505x*14.0*15.0*pow_u_13 * pow_v_5  
        + p_p->c1506x*14.0*15.0*pow_u_13 * pow_v_6  
        + p_p->c1507x*14.0*15.0*pow_u_13 * pow_v_7  
        + p_p->c1508x*14.0*15.0*pow_u_13 * pow_v_8  
        + p_p->c1509x*14.0*15.0*pow_u_13 * pow_v_9  
        + p_p->c1510x*14.0*15.0*pow_u_13 * pow_v_10  
        + p_p->c1511x*14.0*15.0*pow_u_13 * pow_v_11  
        + p_p->c1512x*14.0*15.0*pow_u_13 * pow_v_12  
        + p_p->c1513x*14.0*15.0*pow_u_13 * pow_v_13  
        + p_p->c1514x*14.0*15.0*pow_u_13 * pow_v_14  
        + p_p->c1515x*14.0*15.0*pow_u_13 * pow_v_15  
        + p_p->c1516x*14.0*15.0*pow_u_13 * pow_v_16  
        + p_p->c1517x*14.0*15.0*pow_u_13 * pow_v_17  
        + p_p->c1518x*14.0*15.0*pow_u_13 * pow_v_18  
        + p_p->c1519x*14.0*15.0*pow_u_13 * pow_v_19  
        + p_p->c1520x*14.0*15.0*pow_u_13 * pow_v_20  
        + p_p->c1521x*14.0*15.0*pow_u_13 * pow_v_21  ;
u2_x=u2_x + p_p->c1600x*15.0*16.0*pow_u_14 * pow_v_0  
        + p_p->c1601x*15.0*16.0*pow_u_14 * pow_v_1  
        + p_p->c1602x*15.0*16.0*pow_u_14 * pow_v_2  
        + p_p->c1603x*15.0*16.0*pow_u_14 * pow_v_3  
        + p_p->c1604x*15.0*16.0*pow_u_14 * pow_v_4  
        + p_p->c1605x*15.0*16.0*pow_u_14 * pow_v_5  
        + p_p->c1606x*15.0*16.0*pow_u_14 * pow_v_6  
        + p_p->c1607x*15.0*16.0*pow_u_14 * pow_v_7  
        + p_p->c1608x*15.0*16.0*pow_u_14 * pow_v_8  
        + p_p->c1609x*15.0*16.0*pow_u_14 * pow_v_9  
        + p_p->c1610x*15.0*16.0*pow_u_14 * pow_v_10  
        + p_p->c1611x*15.0*16.0*pow_u_14 * pow_v_11  
        + p_p->c1612x*15.0*16.0*pow_u_14 * pow_v_12  
        + p_p->c1613x*15.0*16.0*pow_u_14 * pow_v_13  
        + p_p->c1614x*15.0*16.0*pow_u_14 * pow_v_14  
        + p_p->c1615x*15.0*16.0*pow_u_14 * pow_v_15  
        + p_p->c1616x*15.0*16.0*pow_u_14 * pow_v_16  
        + p_p->c1617x*15.0*16.0*pow_u_14 * pow_v_17  
        + p_p->c1618x*15.0*16.0*pow_u_14 * pow_v_18  
        + p_p->c1619x*15.0*16.0*pow_u_14 * pow_v_19  
        + p_p->c1620x*15.0*16.0*pow_u_14 * pow_v_20  
        + p_p->c1621x*15.0*16.0*pow_u_14 * pow_v_21  ;
u2_x=u2_x + p_p->c1700x*16.0*17.0*pow_u_15 * pow_v_0  
        + p_p->c1701x*16.0*17.0*pow_u_15 * pow_v_1  
        + p_p->c1702x*16.0*17.0*pow_u_15 * pow_v_2  
        + p_p->c1703x*16.0*17.0*pow_u_15 * pow_v_3  
        + p_p->c1704x*16.0*17.0*pow_u_15 * pow_v_4  
        + p_p->c1705x*16.0*17.0*pow_u_15 * pow_v_5  
        + p_p->c1706x*16.0*17.0*pow_u_15 * pow_v_6  
        + p_p->c1707x*16.0*17.0*pow_u_15 * pow_v_7  
        + p_p->c1708x*16.0*17.0*pow_u_15 * pow_v_8  
        + p_p->c1709x*16.0*17.0*pow_u_15 * pow_v_9  
        + p_p->c1710x*16.0*17.0*pow_u_15 * pow_v_10  
        + p_p->c1711x*16.0*17.0*pow_u_15 * pow_v_11  
        + p_p->c1712x*16.0*17.0*pow_u_15 * pow_v_12  
        + p_p->c1713x*16.0*17.0*pow_u_15 * pow_v_13  
        + p_p->c1714x*16.0*17.0*pow_u_15 * pow_v_14  
        + p_p->c1715x*16.0*17.0*pow_u_15 * pow_v_15  
        + p_p->c1716x*16.0*17.0*pow_u_15 * pow_v_16  
        + p_p->c1717x*16.0*17.0*pow_u_15 * pow_v_17  
        + p_p->c1718x*16.0*17.0*pow_u_15 * pow_v_18  
        + p_p->c1719x*16.0*17.0*pow_u_15 * pow_v_19  
        + p_p->c1720x*16.0*17.0*pow_u_15 * pow_v_20  
        + p_p->c1721x*16.0*17.0*pow_u_15 * pow_v_21  ;
u2_x=u2_x + p_p->c1800x*17.0*18.0*pow_u_16 * pow_v_0  
        + p_p->c1801x*17.0*18.0*pow_u_16 * pow_v_1  
        + p_p->c1802x*17.0*18.0*pow_u_16 * pow_v_2  
        + p_p->c1803x*17.0*18.0*pow_u_16 * pow_v_3  
        + p_p->c1804x*17.0*18.0*pow_u_16 * pow_v_4  
        + p_p->c1805x*17.0*18.0*pow_u_16 * pow_v_5  
        + p_p->c1806x*17.0*18.0*pow_u_16 * pow_v_6  
        + p_p->c1807x*17.0*18.0*pow_u_16 * pow_v_7  
        + p_p->c1808x*17.0*18.0*pow_u_16 * pow_v_8  
        + p_p->c1809x*17.0*18.0*pow_u_16 * pow_v_9  
        + p_p->c1810x*17.0*18.0*pow_u_16 * pow_v_10  
        + p_p->c1811x*17.0*18.0*pow_u_16 * pow_v_11  
        + p_p->c1812x*17.0*18.0*pow_u_16 * pow_v_12  
        + p_p->c1813x*17.0*18.0*pow_u_16 * pow_v_13  
        + p_p->c1814x*17.0*18.0*pow_u_16 * pow_v_14  
        + p_p->c1815x*17.0*18.0*pow_u_16 * pow_v_15  
        + p_p->c1816x*17.0*18.0*pow_u_16 * pow_v_16  
        + p_p->c1817x*17.0*18.0*pow_u_16 * pow_v_17  
        + p_p->c1818x*17.0*18.0*pow_u_16 * pow_v_18  
        + p_p->c1819x*17.0*18.0*pow_u_16 * pow_v_19  
        + p_p->c1820x*17.0*18.0*pow_u_16 * pow_v_20  
        + p_p->c1821x*17.0*18.0*pow_u_16 * pow_v_21  ;
u2_x=u2_x + p_p->c1900x*18.0*19.0*pow_u_17 * pow_v_0  
        + p_p->c1901x*18.0*19.0*pow_u_17 * pow_v_1  
        + p_p->c1902x*18.0*19.0*pow_u_17 * pow_v_2  
        + p_p->c1903x*18.0*19.0*pow_u_17 * pow_v_3  
        + p_p->c1904x*18.0*19.0*pow_u_17 * pow_v_4  
        + p_p->c1905x*18.0*19.0*pow_u_17 * pow_v_5  
        + p_p->c1906x*18.0*19.0*pow_u_17 * pow_v_6  
        + p_p->c1907x*18.0*19.0*pow_u_17 * pow_v_7  
        + p_p->c1908x*18.0*19.0*pow_u_17 * pow_v_8  
        + p_p->c1909x*18.0*19.0*pow_u_17 * pow_v_9  
        + p_p->c1910x*18.0*19.0*pow_u_17 * pow_v_10  
        + p_p->c1911x*18.0*19.0*pow_u_17 * pow_v_11  
        + p_p->c1912x*18.0*19.0*pow_u_17 * pow_v_12  
        + p_p->c1913x*18.0*19.0*pow_u_17 * pow_v_13  
        + p_p->c1914x*18.0*19.0*pow_u_17 * pow_v_14  
        + p_p->c1915x*18.0*19.0*pow_u_17 * pow_v_15  
        + p_p->c1916x*18.0*19.0*pow_u_17 * pow_v_16  
        + p_p->c1917x*18.0*19.0*pow_u_17 * pow_v_17  
        + p_p->c1918x*18.0*19.0*pow_u_17 * pow_v_18  
        + p_p->c1919x*18.0*19.0*pow_u_17 * pow_v_19  
        + p_p->c1920x*18.0*19.0*pow_u_17 * pow_v_20  
        + p_p->c1921x*18.0*19.0*pow_u_17 * pow_v_21  ;
u2_x=u2_x + p_p->c2000x*19.0*20.0*pow_u_18 * pow_v_0  
        + p_p->c2001x*19.0*20.0*pow_u_18 * pow_v_1  
        + p_p->c2002x*19.0*20.0*pow_u_18 * pow_v_2  
        + p_p->c2003x*19.0*20.0*pow_u_18 * pow_v_3  
        + p_p->c2004x*19.0*20.0*pow_u_18 * pow_v_4  
        + p_p->c2005x*19.0*20.0*pow_u_18 * pow_v_5  
        + p_p->c2006x*19.0*20.0*pow_u_18 * pow_v_6  
        + p_p->c2007x*19.0*20.0*pow_u_18 * pow_v_7  
        + p_p->c2008x*19.0*20.0*pow_u_18 * pow_v_8  
        + p_p->c2009x*19.0*20.0*pow_u_18 * pow_v_9  
        + p_p->c2010x*19.0*20.0*pow_u_18 * pow_v_10  
        + p_p->c2011x*19.0*20.0*pow_u_18 * pow_v_11  
        + p_p->c2012x*19.0*20.0*pow_u_18 * pow_v_12  
        + p_p->c2013x*19.0*20.0*pow_u_18 * pow_v_13  
        + p_p->c2014x*19.0*20.0*pow_u_18 * pow_v_14  
        + p_p->c2015x*19.0*20.0*pow_u_18 * pow_v_15  
        + p_p->c2016x*19.0*20.0*pow_u_18 * pow_v_16  
        + p_p->c2017x*19.0*20.0*pow_u_18 * pow_v_17  
        + p_p->c2018x*19.0*20.0*pow_u_18 * pow_v_18  
        + p_p->c2019x*19.0*20.0*pow_u_18 * pow_v_19  
        + p_p->c2020x*19.0*20.0*pow_u_18 * pow_v_20  
        + p_p->c2021x*19.0*20.0*pow_u_18 * pow_v_21  ;
u2_x=u2_x + p_p->c2100x*20.0*21.0*pow_u_19 * pow_v_0  
        + p_p->c2101x*20.0*21.0*pow_u_19 * pow_v_1  
        + p_p->c2102x*20.0*21.0*pow_u_19 * pow_v_2  
        + p_p->c2103x*20.0*21.0*pow_u_19 * pow_v_3  
        + p_p->c2104x*20.0*21.0*pow_u_19 * pow_v_4  
        + p_p->c2105x*20.0*21.0*pow_u_19 * pow_v_5  
        + p_p->c2106x*20.0*21.0*pow_u_19 * pow_v_6  
        + p_p->c2107x*20.0*21.0*pow_u_19 * pow_v_7  
        + p_p->c2108x*20.0*21.0*pow_u_19 * pow_v_8  
        + p_p->c2109x*20.0*21.0*pow_u_19 * pow_v_9  
        + p_p->c2110x*20.0*21.0*pow_u_19 * pow_v_10  
        + p_p->c2111x*20.0*21.0*pow_u_19 * pow_v_11  
        + p_p->c2112x*20.0*21.0*pow_u_19 * pow_v_12  
        + p_p->c2113x*20.0*21.0*pow_u_19 * pow_v_13  
        + p_p->c2114x*20.0*21.0*pow_u_19 * pow_v_14  
        + p_p->c2115x*20.0*21.0*pow_u_19 * pow_v_15  
        + p_p->c2116x*20.0*21.0*pow_u_19 * pow_v_16  
        + p_p->c2117x*20.0*21.0*pow_u_19 * pow_v_17  
        + p_p->c2118x*20.0*21.0*pow_u_19 * pow_v_18  
        + p_p->c2119x*20.0*21.0*pow_u_19 * pow_v_19  
        + p_p->c2120x*20.0*21.0*pow_u_19 * pow_v_20  
        + p_p->c2121x*20.0*21.0*pow_u_19 * pow_v_21  ;

u2_y    = p_p->c0000y*            0.0           * pow_v_0  
        + p_p->c0001y*            0.0           * pow_v_1  
        + p_p->c0002y*            0.0           * pow_v_2  
        + p_p->c0003y*            0.0           * pow_v_3  
        + p_p->c0004y*            0.0           * pow_v_4  
        + p_p->c0005y*            0.0           * pow_v_5  
        + p_p->c0006y*            0.0           * pow_v_6  
        + p_p->c0007y*            0.0           * pow_v_7  
        + p_p->c0008y*            0.0           * pow_v_8  
        + p_p->c0009y*            0.0           * pow_v_9  
        + p_p->c0010y*            0.0           * pow_v_10  
        + p_p->c0011y*            0.0           * pow_v_11  
        + p_p->c0012y*            0.0           * pow_v_12  
        + p_p->c0013y*            0.0           * pow_v_13  
        + p_p->c0014y*            0.0           * pow_v_14  
        + p_p->c0015y*            0.0           * pow_v_15  
        + p_p->c0016y*            0.0           * pow_v_16  
        + p_p->c0017y*            0.0           * pow_v_17  
        + p_p->c0018y*            0.0           * pow_v_18  
        + p_p->c0019y*            0.0           * pow_v_19  
        + p_p->c0020y*            0.0           * pow_v_20  
        + p_p->c0021y*            0.0           * pow_v_21  ;
u2_y=u2_y + p_p->c0100y*            0.0           * pow_v_0  
        + p_p->c0101y*            0.0           * pow_v_1  
        + p_p->c0102y*            0.0           * pow_v_2  
        + p_p->c0103y*            0.0           * pow_v_3  
        + p_p->c0104y*            0.0           * pow_v_4  
        + p_p->c0105y*            0.0           * pow_v_5  
        + p_p->c0106y*            0.0           * pow_v_6  
        + p_p->c0107y*            0.0           * pow_v_7  
        + p_p->c0108y*            0.0           * pow_v_8  
        + p_p->c0109y*            0.0           * pow_v_9  
        + p_p->c0110y*            0.0           * pow_v_10  
        + p_p->c0111y*            0.0           * pow_v_11  
        + p_p->c0112y*            0.0           * pow_v_12  
        + p_p->c0113y*            0.0           * pow_v_13  
        + p_p->c0114y*            0.0           * pow_v_14  
        + p_p->c0115y*            0.0           * pow_v_15  
        + p_p->c0116y*            0.0           * pow_v_16  
        + p_p->c0117y*            0.0           * pow_v_17  
        + p_p->c0118y*            0.0           * pow_v_18  
        + p_p->c0119y*            0.0           * pow_v_19  
        + p_p->c0120y*            0.0           * pow_v_20  
        + p_p->c0121y*            0.0           * pow_v_21  ;
u2_y=u2_y + p_p->c0200y* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201y* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202y* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203y* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204y* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205y* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206y* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207y* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0208y* 1.0* 2.0*pow_u_0 * pow_v_8  
        + p_p->c0209y* 1.0* 2.0*pow_u_0 * pow_v_9  
        + p_p->c0210y* 1.0* 2.0*pow_u_0 * pow_v_10  
        + p_p->c0211y* 1.0* 2.0*pow_u_0 * pow_v_11  
        + p_p->c0212y* 1.0* 2.0*pow_u_0 * pow_v_12  
        + p_p->c0213y* 1.0* 2.0*pow_u_0 * pow_v_13  
        + p_p->c0214y* 1.0* 2.0*pow_u_0 * pow_v_14  
        + p_p->c0215y* 1.0* 2.0*pow_u_0 * pow_v_15  
        + p_p->c0216y* 1.0* 2.0*pow_u_0 * pow_v_16  
        + p_p->c0217y* 1.0* 2.0*pow_u_0 * pow_v_17  
        + p_p->c0218y* 1.0* 2.0*pow_u_0 * pow_v_18  
        + p_p->c0219y* 1.0* 2.0*pow_u_0 * pow_v_19  
        + p_p->c0220y* 1.0* 2.0*pow_u_0 * pow_v_20  
        + p_p->c0221y* 1.0* 2.0*pow_u_0 * pow_v_21  ;
u2_y=u2_y + p_p->c0300y* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301y* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302y* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303y* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304y* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305y* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306y* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307y* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0308y* 2.0* 3.0*pow_u_1 * pow_v_8  
        + p_p->c0309y* 2.0* 3.0*pow_u_1 * pow_v_9  
        + p_p->c0310y* 2.0* 3.0*pow_u_1 * pow_v_10  
        + p_p->c0311y* 2.0* 3.0*pow_u_1 * pow_v_11  
        + p_p->c0312y* 2.0* 3.0*pow_u_1 * pow_v_12  
        + p_p->c0313y* 2.0* 3.0*pow_u_1 * pow_v_13  
        + p_p->c0314y* 2.0* 3.0*pow_u_1 * pow_v_14  
        + p_p->c0315y* 2.0* 3.0*pow_u_1 * pow_v_15  
        + p_p->c0316y* 2.0* 3.0*pow_u_1 * pow_v_16  
        + p_p->c0317y* 2.0* 3.0*pow_u_1 * pow_v_17  
        + p_p->c0318y* 2.0* 3.0*pow_u_1 * pow_v_18  
        + p_p->c0319y* 2.0* 3.0*pow_u_1 * pow_v_19  
        + p_p->c0320y* 2.0* 3.0*pow_u_1 * pow_v_20  
        + p_p->c0321y* 2.0* 3.0*pow_u_1 * pow_v_21  ;
u2_y=u2_y + p_p->c0400y* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401y* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402y* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403y* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404y* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405y* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406y* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407y* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0408y* 3.0* 4.0*pow_u_2 * pow_v_8  
        + p_p->c0409y* 3.0* 4.0*pow_u_2 * pow_v_9  
        + p_p->c0410y* 3.0* 4.0*pow_u_2 * pow_v_10  
        + p_p->c0411y* 3.0* 4.0*pow_u_2 * pow_v_11  
        + p_p->c0412y* 3.0* 4.0*pow_u_2 * pow_v_12  
        + p_p->c0413y* 3.0* 4.0*pow_u_2 * pow_v_13  
        + p_p->c0414y* 3.0* 4.0*pow_u_2 * pow_v_14  
        + p_p->c0415y* 3.0* 4.0*pow_u_2 * pow_v_15  
        + p_p->c0416y* 3.0* 4.0*pow_u_2 * pow_v_16  
        + p_p->c0417y* 3.0* 4.0*pow_u_2 * pow_v_17  
        + p_p->c0418y* 3.0* 4.0*pow_u_2 * pow_v_18  
        + p_p->c0419y* 3.0* 4.0*pow_u_2 * pow_v_19  
        + p_p->c0420y* 3.0* 4.0*pow_u_2 * pow_v_20  
        + p_p->c0421y* 3.0* 4.0*pow_u_2 * pow_v_21  ;
u2_y=u2_y + p_p->c0500y* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501y* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502y* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503y* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504y* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505y* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506y* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507y* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0508y* 4.0* 5.0*pow_u_3 * pow_v_8  
        + p_p->c0509y* 4.0* 5.0*pow_u_3 * pow_v_9  
        + p_p->c0510y* 4.0* 5.0*pow_u_3 * pow_v_10  
        + p_p->c0511y* 4.0* 5.0*pow_u_3 * pow_v_11  
        + p_p->c0512y* 4.0* 5.0*pow_u_3 * pow_v_12  
        + p_p->c0513y* 4.0* 5.0*pow_u_3 * pow_v_13  
        + p_p->c0514y* 4.0* 5.0*pow_u_3 * pow_v_14  
        + p_p->c0515y* 4.0* 5.0*pow_u_3 * pow_v_15  
        + p_p->c0516y* 4.0* 5.0*pow_u_3 * pow_v_16  
        + p_p->c0517y* 4.0* 5.0*pow_u_3 * pow_v_17  
        + p_p->c0518y* 4.0* 5.0*pow_u_3 * pow_v_18  
        + p_p->c0519y* 4.0* 5.0*pow_u_3 * pow_v_19  
        + p_p->c0520y* 4.0* 5.0*pow_u_3 * pow_v_20  
        + p_p->c0521y* 4.0* 5.0*pow_u_3 * pow_v_21  ;
u2_y=u2_y + p_p->c0600y* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601y* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602y* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603y* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604y* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605y* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606y* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607y* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0608y* 5.0* 6.0*pow_u_4 * pow_v_8  
        + p_p->c0609y* 5.0* 6.0*pow_u_4 * pow_v_9  
        + p_p->c0610y* 5.0* 6.0*pow_u_4 * pow_v_10  
        + p_p->c0611y* 5.0* 6.0*pow_u_4 * pow_v_11  
        + p_p->c0612y* 5.0* 6.0*pow_u_4 * pow_v_12  
        + p_p->c0613y* 5.0* 6.0*pow_u_4 * pow_v_13  
        + p_p->c0614y* 5.0* 6.0*pow_u_4 * pow_v_14  
        + p_p->c0615y* 5.0* 6.0*pow_u_4 * pow_v_15  
        + p_p->c0616y* 5.0* 6.0*pow_u_4 * pow_v_16  
        + p_p->c0617y* 5.0* 6.0*pow_u_4 * pow_v_17  
        + p_p->c0618y* 5.0* 6.0*pow_u_4 * pow_v_18  
        + p_p->c0619y* 5.0* 6.0*pow_u_4 * pow_v_19  
        + p_p->c0620y* 5.0* 6.0*pow_u_4 * pow_v_20  
        + p_p->c0621y* 5.0* 6.0*pow_u_4 * pow_v_21  ;
u2_y=u2_y + p_p->c0700y* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701y* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702y* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703y* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704y* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705y* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706y* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707y* 6.0* 7.0*pow_u_5 * pow_v_7  
        + p_p->c0708y* 6.0* 7.0*pow_u_5 * pow_v_8  
        + p_p->c0709y* 6.0* 7.0*pow_u_5 * pow_v_9  
        + p_p->c0710y* 6.0* 7.0*pow_u_5 * pow_v_10  
        + p_p->c0711y* 6.0* 7.0*pow_u_5 * pow_v_11  
        + p_p->c0712y* 6.0* 7.0*pow_u_5 * pow_v_12  
        + p_p->c0713y* 6.0* 7.0*pow_u_5 * pow_v_13  
        + p_p->c0714y* 6.0* 7.0*pow_u_5 * pow_v_14  
        + p_p->c0715y* 6.0* 7.0*pow_u_5 * pow_v_15  
        + p_p->c0716y* 6.0* 7.0*pow_u_5 * pow_v_16  
        + p_p->c0717y* 6.0* 7.0*pow_u_5 * pow_v_17  
        + p_p->c0718y* 6.0* 7.0*pow_u_5 * pow_v_18  
        + p_p->c0719y* 6.0* 7.0*pow_u_5 * pow_v_19  
        + p_p->c0720y* 6.0* 7.0*pow_u_5 * pow_v_20  
        + p_p->c0721y* 6.0* 7.0*pow_u_5 * pow_v_21  ;
u2_y=u2_y + p_p->c0800y* 7.0* 8.0*pow_u_6 * pow_v_0  
        + p_p->c0801y* 7.0* 8.0*pow_u_6 * pow_v_1  
        + p_p->c0802y* 7.0* 8.0*pow_u_6 * pow_v_2  
        + p_p->c0803y* 7.0* 8.0*pow_u_6 * pow_v_3  
        + p_p->c0804y* 7.0* 8.0*pow_u_6 * pow_v_4  
        + p_p->c0805y* 7.0* 8.0*pow_u_6 * pow_v_5  
        + p_p->c0806y* 7.0* 8.0*pow_u_6 * pow_v_6  
        + p_p->c0807y* 7.0* 8.0*pow_u_6 * pow_v_7  
        + p_p->c0808y* 7.0* 8.0*pow_u_6 * pow_v_8  
        + p_p->c0809y* 7.0* 8.0*pow_u_6 * pow_v_9  
        + p_p->c0810y* 7.0* 8.0*pow_u_6 * pow_v_10  
        + p_p->c0811y* 7.0* 8.0*pow_u_6 * pow_v_11  
        + p_p->c0812y* 7.0* 8.0*pow_u_6 * pow_v_12  
        + p_p->c0813y* 7.0* 8.0*pow_u_6 * pow_v_13  
        + p_p->c0814y* 7.0* 8.0*pow_u_6 * pow_v_14  
        + p_p->c0815y* 7.0* 8.0*pow_u_6 * pow_v_15  
        + p_p->c0816y* 7.0* 8.0*pow_u_6 * pow_v_16  
        + p_p->c0817y* 7.0* 8.0*pow_u_6 * pow_v_17  
        + p_p->c0818y* 7.0* 8.0*pow_u_6 * pow_v_18  
        + p_p->c0819y* 7.0* 8.0*pow_u_6 * pow_v_19  
        + p_p->c0820y* 7.0* 8.0*pow_u_6 * pow_v_20  
        + p_p->c0821y* 7.0* 8.0*pow_u_6 * pow_v_21  ;
u2_y=u2_y + p_p->c0900y* 8.0* 9.0*pow_u_7 * pow_v_0  
        + p_p->c0901y* 8.0* 9.0*pow_u_7 * pow_v_1  
        + p_p->c0902y* 8.0* 9.0*pow_u_7 * pow_v_2  
        + p_p->c0903y* 8.0* 9.0*pow_u_7 * pow_v_3  
        + p_p->c0904y* 8.0* 9.0*pow_u_7 * pow_v_4  
        + p_p->c0905y* 8.0* 9.0*pow_u_7 * pow_v_5  
        + p_p->c0906y* 8.0* 9.0*pow_u_7 * pow_v_6  
        + p_p->c0907y* 8.0* 9.0*pow_u_7 * pow_v_7  
        + p_p->c0908y* 8.0* 9.0*pow_u_7 * pow_v_8  
        + p_p->c0909y* 8.0* 9.0*pow_u_7 * pow_v_9  
        + p_p->c0910y* 8.0* 9.0*pow_u_7 * pow_v_10  
        + p_p->c0911y* 8.0* 9.0*pow_u_7 * pow_v_11  
        + p_p->c0912y* 8.0* 9.0*pow_u_7 * pow_v_12  
        + p_p->c0913y* 8.0* 9.0*pow_u_7 * pow_v_13  
        + p_p->c0914y* 8.0* 9.0*pow_u_7 * pow_v_14  
        + p_p->c0915y* 8.0* 9.0*pow_u_7 * pow_v_15  
        + p_p->c0916y* 8.0* 9.0*pow_u_7 * pow_v_16  
        + p_p->c0917y* 8.0* 9.0*pow_u_7 * pow_v_17  
        + p_p->c0918y* 8.0* 9.0*pow_u_7 * pow_v_18  
        + p_p->c0919y* 8.0* 9.0*pow_u_7 * pow_v_19  
        + p_p->c0920y* 8.0* 9.0*pow_u_7 * pow_v_20  
        + p_p->c0921y* 8.0* 9.0*pow_u_7 * pow_v_21  ;
u2_y=u2_y + p_p->c1000y* 9.0*10.0*pow_u_8 * pow_v_0  
        + p_p->c1001y* 9.0*10.0*pow_u_8 * pow_v_1  
        + p_p->c1002y* 9.0*10.0*pow_u_8 * pow_v_2  
        + p_p->c1003y* 9.0*10.0*pow_u_8 * pow_v_3  
        + p_p->c1004y* 9.0*10.0*pow_u_8 * pow_v_4  
        + p_p->c1005y* 9.0*10.0*pow_u_8 * pow_v_5  
        + p_p->c1006y* 9.0*10.0*pow_u_8 * pow_v_6  
        + p_p->c1007y* 9.0*10.0*pow_u_8 * pow_v_7  
        + p_p->c1008y* 9.0*10.0*pow_u_8 * pow_v_8  
        + p_p->c1009y* 9.0*10.0*pow_u_8 * pow_v_9  
        + p_p->c1010y* 9.0*10.0*pow_u_8 * pow_v_10  
        + p_p->c1011y* 9.0*10.0*pow_u_8 * pow_v_11  
        + p_p->c1012y* 9.0*10.0*pow_u_8 * pow_v_12  
        + p_p->c1013y* 9.0*10.0*pow_u_8 * pow_v_13  
        + p_p->c1014y* 9.0*10.0*pow_u_8 * pow_v_14  
        + p_p->c1015y* 9.0*10.0*pow_u_8 * pow_v_15  
        + p_p->c1016y* 9.0*10.0*pow_u_8 * pow_v_16  
        + p_p->c1017y* 9.0*10.0*pow_u_8 * pow_v_17  
        + p_p->c1018y* 9.0*10.0*pow_u_8 * pow_v_18  
        + p_p->c1019y* 9.0*10.0*pow_u_8 * pow_v_19  
        + p_p->c1020y* 9.0*10.0*pow_u_8 * pow_v_20  
        + p_p->c1021y* 9.0*10.0*pow_u_8 * pow_v_21  ;
u2_y=u2_y + p_p->c1100y*10.0*11.0*pow_u_9 * pow_v_0  
        + p_p->c1101y*10.0*11.0*pow_u_9 * pow_v_1  
        + p_p->c1102y*10.0*11.0*pow_u_9 * pow_v_2  
        + p_p->c1103y*10.0*11.0*pow_u_9 * pow_v_3  
        + p_p->c1104y*10.0*11.0*pow_u_9 * pow_v_4  
        + p_p->c1105y*10.0*11.0*pow_u_9 * pow_v_5  
        + p_p->c1106y*10.0*11.0*pow_u_9 * pow_v_6  
        + p_p->c1107y*10.0*11.0*pow_u_9 * pow_v_7  
        + p_p->c1108y*10.0*11.0*pow_u_9 * pow_v_8  
        + p_p->c1109y*10.0*11.0*pow_u_9 * pow_v_9  
        + p_p->c1110y*10.0*11.0*pow_u_9 * pow_v_10  
        + p_p->c1111y*10.0*11.0*pow_u_9 * pow_v_11  
        + p_p->c1112y*10.0*11.0*pow_u_9 * pow_v_12  
        + p_p->c1113y*10.0*11.0*pow_u_9 * pow_v_13  
        + p_p->c1114y*10.0*11.0*pow_u_9 * pow_v_14  
        + p_p->c1115y*10.0*11.0*pow_u_9 * pow_v_15  
        + p_p->c1116y*10.0*11.0*pow_u_9 * pow_v_16  
        + p_p->c1117y*10.0*11.0*pow_u_9 * pow_v_17  
        + p_p->c1118y*10.0*11.0*pow_u_9 * pow_v_18  
        + p_p->c1119y*10.0*11.0*pow_u_9 * pow_v_19  
        + p_p->c1120y*10.0*11.0*pow_u_9 * pow_v_20  
        + p_p->c1121y*10.0*11.0*pow_u_9 * pow_v_21  ;
u2_y=u2_y + p_p->c1200y*11.0*12.0*pow_u_10 * pow_v_0  
        + p_p->c1201y*11.0*12.0*pow_u_10 * pow_v_1  
        + p_p->c1202y*11.0*12.0*pow_u_10 * pow_v_2  
        + p_p->c1203y*11.0*12.0*pow_u_10 * pow_v_3  
        + p_p->c1204y*11.0*12.0*pow_u_10 * pow_v_4  
        + p_p->c1205y*11.0*12.0*pow_u_10 * pow_v_5  
        + p_p->c1206y*11.0*12.0*pow_u_10 * pow_v_6  
        + p_p->c1207y*11.0*12.0*pow_u_10 * pow_v_7  
        + p_p->c1208y*11.0*12.0*pow_u_10 * pow_v_8  
        + p_p->c1209y*11.0*12.0*pow_u_10 * pow_v_9  
        + p_p->c1210y*11.0*12.0*pow_u_10 * pow_v_10  
        + p_p->c1211y*11.0*12.0*pow_u_10 * pow_v_11  
        + p_p->c1212y*11.0*12.0*pow_u_10 * pow_v_12  
        + p_p->c1213y*11.0*12.0*pow_u_10 * pow_v_13  
        + p_p->c1214y*11.0*12.0*pow_u_10 * pow_v_14  
        + p_p->c1215y*11.0*12.0*pow_u_10 * pow_v_15  
        + p_p->c1216y*11.0*12.0*pow_u_10 * pow_v_16  
        + p_p->c1217y*11.0*12.0*pow_u_10 * pow_v_17  
        + p_p->c1218y*11.0*12.0*pow_u_10 * pow_v_18  
        + p_p->c1219y*11.0*12.0*pow_u_10 * pow_v_19  
        + p_p->c1220y*11.0*12.0*pow_u_10 * pow_v_20  
        + p_p->c1221y*11.0*12.0*pow_u_10 * pow_v_21  ;
u2_y=u2_y + p_p->c1300y*12.0*13.0*pow_u_11 * pow_v_0  
        + p_p->c1301y*12.0*13.0*pow_u_11 * pow_v_1  
        + p_p->c1302y*12.0*13.0*pow_u_11 * pow_v_2  
        + p_p->c1303y*12.0*13.0*pow_u_11 * pow_v_3  
        + p_p->c1304y*12.0*13.0*pow_u_11 * pow_v_4  
        + p_p->c1305y*12.0*13.0*pow_u_11 * pow_v_5  
        + p_p->c1306y*12.0*13.0*pow_u_11 * pow_v_6  
        + p_p->c1307y*12.0*13.0*pow_u_11 * pow_v_7  
        + p_p->c1308y*12.0*13.0*pow_u_11 * pow_v_8  
        + p_p->c1309y*12.0*13.0*pow_u_11 * pow_v_9  
        + p_p->c1310y*12.0*13.0*pow_u_11 * pow_v_10  
        + p_p->c1311y*12.0*13.0*pow_u_11 * pow_v_11  
        + p_p->c1312y*12.0*13.0*pow_u_11 * pow_v_12  
        + p_p->c1313y*12.0*13.0*pow_u_11 * pow_v_13  
        + p_p->c1314y*12.0*13.0*pow_u_11 * pow_v_14  
        + p_p->c1315y*12.0*13.0*pow_u_11 * pow_v_15  
        + p_p->c1316y*12.0*13.0*pow_u_11 * pow_v_16  
        + p_p->c1317y*12.0*13.0*pow_u_11 * pow_v_17  
        + p_p->c1318y*12.0*13.0*pow_u_11 * pow_v_18  
        + p_p->c1319y*12.0*13.0*pow_u_11 * pow_v_19  
        + p_p->c1320y*12.0*13.0*pow_u_11 * pow_v_20  
        + p_p->c1321y*12.0*13.0*pow_u_11 * pow_v_21  ;
u2_y=u2_y + p_p->c1400y*13.0*14.0*pow_u_12 * pow_v_0  
        + p_p->c1401y*13.0*14.0*pow_u_12 * pow_v_1  
        + p_p->c1402y*13.0*14.0*pow_u_12 * pow_v_2  
        + p_p->c1403y*13.0*14.0*pow_u_12 * pow_v_3  
        + p_p->c1404y*13.0*14.0*pow_u_12 * pow_v_4  
        + p_p->c1405y*13.0*14.0*pow_u_12 * pow_v_5  
        + p_p->c1406y*13.0*14.0*pow_u_12 * pow_v_6  
        + p_p->c1407y*13.0*14.0*pow_u_12 * pow_v_7  
        + p_p->c1408y*13.0*14.0*pow_u_12 * pow_v_8  
        + p_p->c1409y*13.0*14.0*pow_u_12 * pow_v_9  
        + p_p->c1410y*13.0*14.0*pow_u_12 * pow_v_10  
        + p_p->c1411y*13.0*14.0*pow_u_12 * pow_v_11  
        + p_p->c1412y*13.0*14.0*pow_u_12 * pow_v_12  
        + p_p->c1413y*13.0*14.0*pow_u_12 * pow_v_13  
        + p_p->c1414y*13.0*14.0*pow_u_12 * pow_v_14  
        + p_p->c1415y*13.0*14.0*pow_u_12 * pow_v_15  
        + p_p->c1416y*13.0*14.0*pow_u_12 * pow_v_16  
        + p_p->c1417y*13.0*14.0*pow_u_12 * pow_v_17  
        + p_p->c1418y*13.0*14.0*pow_u_12 * pow_v_18  
        + p_p->c1419y*13.0*14.0*pow_u_12 * pow_v_19  
        + p_p->c1420y*13.0*14.0*pow_u_12 * pow_v_20  
        + p_p->c1421y*13.0*14.0*pow_u_12 * pow_v_21  ;
u2_y=u2_y + p_p->c1500y*14.0*15.0*pow_u_13 * pow_v_0  
        + p_p->c1501y*14.0*15.0*pow_u_13 * pow_v_1  
        + p_p->c1502y*14.0*15.0*pow_u_13 * pow_v_2  
        + p_p->c1503y*14.0*15.0*pow_u_13 * pow_v_3  
        + p_p->c1504y*14.0*15.0*pow_u_13 * pow_v_4  
        + p_p->c1505y*14.0*15.0*pow_u_13 * pow_v_5  
        + p_p->c1506y*14.0*15.0*pow_u_13 * pow_v_6  
        + p_p->c1507y*14.0*15.0*pow_u_13 * pow_v_7  
        + p_p->c1508y*14.0*15.0*pow_u_13 * pow_v_8  
        + p_p->c1509y*14.0*15.0*pow_u_13 * pow_v_9  
        + p_p->c1510y*14.0*15.0*pow_u_13 * pow_v_10  
        + p_p->c1511y*14.0*15.0*pow_u_13 * pow_v_11  
        + p_p->c1512y*14.0*15.0*pow_u_13 * pow_v_12  
        + p_p->c1513y*14.0*15.0*pow_u_13 * pow_v_13  
        + p_p->c1514y*14.0*15.0*pow_u_13 * pow_v_14  
        + p_p->c1515y*14.0*15.0*pow_u_13 * pow_v_15  
        + p_p->c1516y*14.0*15.0*pow_u_13 * pow_v_16  
        + p_p->c1517y*14.0*15.0*pow_u_13 * pow_v_17  
        + p_p->c1518y*14.0*15.0*pow_u_13 * pow_v_18  
        + p_p->c1519y*14.0*15.0*pow_u_13 * pow_v_19  
        + p_p->c1520y*14.0*15.0*pow_u_13 * pow_v_20  
        + p_p->c1521y*14.0*15.0*pow_u_13 * pow_v_21  ;
u2_y=u2_y + p_p->c1600y*15.0*16.0*pow_u_14 * pow_v_0  
        + p_p->c1601y*15.0*16.0*pow_u_14 * pow_v_1  
        + p_p->c1602y*15.0*16.0*pow_u_14 * pow_v_2  
        + p_p->c1603y*15.0*16.0*pow_u_14 * pow_v_3  
        + p_p->c1604y*15.0*16.0*pow_u_14 * pow_v_4  
        + p_p->c1605y*15.0*16.0*pow_u_14 * pow_v_5  
        + p_p->c1606y*15.0*16.0*pow_u_14 * pow_v_6  
        + p_p->c1607y*15.0*16.0*pow_u_14 * pow_v_7  
        + p_p->c1608y*15.0*16.0*pow_u_14 * pow_v_8  
        + p_p->c1609y*15.0*16.0*pow_u_14 * pow_v_9  
        + p_p->c1610y*15.0*16.0*pow_u_14 * pow_v_10  
        + p_p->c1611y*15.0*16.0*pow_u_14 * pow_v_11  
        + p_p->c1612y*15.0*16.0*pow_u_14 * pow_v_12  
        + p_p->c1613y*15.0*16.0*pow_u_14 * pow_v_13  
        + p_p->c1614y*15.0*16.0*pow_u_14 * pow_v_14  
        + p_p->c1615y*15.0*16.0*pow_u_14 * pow_v_15  
        + p_p->c1616y*15.0*16.0*pow_u_14 * pow_v_16  
        + p_p->c1617y*15.0*16.0*pow_u_14 * pow_v_17  
        + p_p->c1618y*15.0*16.0*pow_u_14 * pow_v_18  
        + p_p->c1619y*15.0*16.0*pow_u_14 * pow_v_19  
        + p_p->c1620y*15.0*16.0*pow_u_14 * pow_v_20  
        + p_p->c1621y*15.0*16.0*pow_u_14 * pow_v_21  ;
u2_y=u2_y + p_p->c1700y*16.0*17.0*pow_u_15 * pow_v_0  
        + p_p->c1701y*16.0*17.0*pow_u_15 * pow_v_1  
        + p_p->c1702y*16.0*17.0*pow_u_15 * pow_v_2  
        + p_p->c1703y*16.0*17.0*pow_u_15 * pow_v_3  
        + p_p->c1704y*16.0*17.0*pow_u_15 * pow_v_4  
        + p_p->c1705y*16.0*17.0*pow_u_15 * pow_v_5  
        + p_p->c1706y*16.0*17.0*pow_u_15 * pow_v_6  
        + p_p->c1707y*16.0*17.0*pow_u_15 * pow_v_7  
        + p_p->c1708y*16.0*17.0*pow_u_15 * pow_v_8  
        + p_p->c1709y*16.0*17.0*pow_u_15 * pow_v_9  
        + p_p->c1710y*16.0*17.0*pow_u_15 * pow_v_10  
        + p_p->c1711y*16.0*17.0*pow_u_15 * pow_v_11  
        + p_p->c1712y*16.0*17.0*pow_u_15 * pow_v_12  
        + p_p->c1713y*16.0*17.0*pow_u_15 * pow_v_13  
        + p_p->c1714y*16.0*17.0*pow_u_15 * pow_v_14  
        + p_p->c1715y*16.0*17.0*pow_u_15 * pow_v_15  
        + p_p->c1716y*16.0*17.0*pow_u_15 * pow_v_16  
        + p_p->c1717y*16.0*17.0*pow_u_15 * pow_v_17  
        + p_p->c1718y*16.0*17.0*pow_u_15 * pow_v_18  
        + p_p->c1719y*16.0*17.0*pow_u_15 * pow_v_19  
        + p_p->c1720y*16.0*17.0*pow_u_15 * pow_v_20  
        + p_p->c1721y*16.0*17.0*pow_u_15 * pow_v_21  ;
u2_y=u2_y + p_p->c1800y*17.0*18.0*pow_u_16 * pow_v_0  
        + p_p->c1801y*17.0*18.0*pow_u_16 * pow_v_1  
        + p_p->c1802y*17.0*18.0*pow_u_16 * pow_v_2  
        + p_p->c1803y*17.0*18.0*pow_u_16 * pow_v_3  
        + p_p->c1804y*17.0*18.0*pow_u_16 * pow_v_4  
        + p_p->c1805y*17.0*18.0*pow_u_16 * pow_v_5  
        + p_p->c1806y*17.0*18.0*pow_u_16 * pow_v_6  
        + p_p->c1807y*17.0*18.0*pow_u_16 * pow_v_7  
        + p_p->c1808y*17.0*18.0*pow_u_16 * pow_v_8  
        + p_p->c1809y*17.0*18.0*pow_u_16 * pow_v_9  
        + p_p->c1810y*17.0*18.0*pow_u_16 * pow_v_10  
        + p_p->c1811y*17.0*18.0*pow_u_16 * pow_v_11  
        + p_p->c1812y*17.0*18.0*pow_u_16 * pow_v_12  
        + p_p->c1813y*17.0*18.0*pow_u_16 * pow_v_13  
        + p_p->c1814y*17.0*18.0*pow_u_16 * pow_v_14  
        + p_p->c1815y*17.0*18.0*pow_u_16 * pow_v_15  
        + p_p->c1816y*17.0*18.0*pow_u_16 * pow_v_16  
        + p_p->c1817y*17.0*18.0*pow_u_16 * pow_v_17  
        + p_p->c1818y*17.0*18.0*pow_u_16 * pow_v_18  
        + p_p->c1819y*17.0*18.0*pow_u_16 * pow_v_19  
        + p_p->c1820y*17.0*18.0*pow_u_16 * pow_v_20  
        + p_p->c1821y*17.0*18.0*pow_u_16 * pow_v_21  ;
u2_y=u2_y + p_p->c1900y*18.0*19.0*pow_u_17 * pow_v_0  
        + p_p->c1901y*18.0*19.0*pow_u_17 * pow_v_1  
        + p_p->c1902y*18.0*19.0*pow_u_17 * pow_v_2  
        + p_p->c1903y*18.0*19.0*pow_u_17 * pow_v_3  
        + p_p->c1904y*18.0*19.0*pow_u_17 * pow_v_4  
        + p_p->c1905y*18.0*19.0*pow_u_17 * pow_v_5  
        + p_p->c1906y*18.0*19.0*pow_u_17 * pow_v_6  
        + p_p->c1907y*18.0*19.0*pow_u_17 * pow_v_7  
        + p_p->c1908y*18.0*19.0*pow_u_17 * pow_v_8  
        + p_p->c1909y*18.0*19.0*pow_u_17 * pow_v_9  
        + p_p->c1910y*18.0*19.0*pow_u_17 * pow_v_10  
        + p_p->c1911y*18.0*19.0*pow_u_17 * pow_v_11  
        + p_p->c1912y*18.0*19.0*pow_u_17 * pow_v_12  
        + p_p->c1913y*18.0*19.0*pow_u_17 * pow_v_13  
        + p_p->c1914y*18.0*19.0*pow_u_17 * pow_v_14  
        + p_p->c1915y*18.0*19.0*pow_u_17 * pow_v_15  
        + p_p->c1916y*18.0*19.0*pow_u_17 * pow_v_16  
        + p_p->c1917y*18.0*19.0*pow_u_17 * pow_v_17  
        + p_p->c1918y*18.0*19.0*pow_u_17 * pow_v_18  
        + p_p->c1919y*18.0*19.0*pow_u_17 * pow_v_19  
        + p_p->c1920y*18.0*19.0*pow_u_17 * pow_v_20  
        + p_p->c1921y*18.0*19.0*pow_u_17 * pow_v_21  ;
u2_y=u2_y + p_p->c2000y*19.0*20.0*pow_u_18 * pow_v_0  
        + p_p->c2001y*19.0*20.0*pow_u_18 * pow_v_1  
        + p_p->c2002y*19.0*20.0*pow_u_18 * pow_v_2  
        + p_p->c2003y*19.0*20.0*pow_u_18 * pow_v_3  
        + p_p->c2004y*19.0*20.0*pow_u_18 * pow_v_4  
        + p_p->c2005y*19.0*20.0*pow_u_18 * pow_v_5  
        + p_p->c2006y*19.0*20.0*pow_u_18 * pow_v_6  
        + p_p->c2007y*19.0*20.0*pow_u_18 * pow_v_7  
        + p_p->c2008y*19.0*20.0*pow_u_18 * pow_v_8  
        + p_p->c2009y*19.0*20.0*pow_u_18 * pow_v_9  
        + p_p->c2010y*19.0*20.0*pow_u_18 * pow_v_10  
        + p_p->c2011y*19.0*20.0*pow_u_18 * pow_v_11  
        + p_p->c2012y*19.0*20.0*pow_u_18 * pow_v_12  
        + p_p->c2013y*19.0*20.0*pow_u_18 * pow_v_13  
        + p_p->c2014y*19.0*20.0*pow_u_18 * pow_v_14  
        + p_p->c2015y*19.0*20.0*pow_u_18 * pow_v_15  
        + p_p->c2016y*19.0*20.0*pow_u_18 * pow_v_16  
        + p_p->c2017y*19.0*20.0*pow_u_18 * pow_v_17  
        + p_p->c2018y*19.0*20.0*pow_u_18 * pow_v_18  
        + p_p->c2019y*19.0*20.0*pow_u_18 * pow_v_19  
        + p_p->c2020y*19.0*20.0*pow_u_18 * pow_v_20  
        + p_p->c2021y*19.0*20.0*pow_u_18 * pow_v_21  ;
u2_y=u2_y + p_p->c2100y*20.0*21.0*pow_u_19 * pow_v_0  
        + p_p->c2101y*20.0*21.0*pow_u_19 * pow_v_1  
        + p_p->c2102y*20.0*21.0*pow_u_19 * pow_v_2  
        + p_p->c2103y*20.0*21.0*pow_u_19 * pow_v_3  
        + p_p->c2104y*20.0*21.0*pow_u_19 * pow_v_4  
        + p_p->c2105y*20.0*21.0*pow_u_19 * pow_v_5  
        + p_p->c2106y*20.0*21.0*pow_u_19 * pow_v_6  
        + p_p->c2107y*20.0*21.0*pow_u_19 * pow_v_7  
        + p_p->c2108y*20.0*21.0*pow_u_19 * pow_v_8  
        + p_p->c2109y*20.0*21.0*pow_u_19 * pow_v_9  
        + p_p->c2110y*20.0*21.0*pow_u_19 * pow_v_10  
        + p_p->c2111y*20.0*21.0*pow_u_19 * pow_v_11  
        + p_p->c2112y*20.0*21.0*pow_u_19 * pow_v_12  
        + p_p->c2113y*20.0*21.0*pow_u_19 * pow_v_13  
        + p_p->c2114y*20.0*21.0*pow_u_19 * pow_v_14  
        + p_p->c2115y*20.0*21.0*pow_u_19 * pow_v_15  
        + p_p->c2116y*20.0*21.0*pow_u_19 * pow_v_16  
        + p_p->c2117y*20.0*21.0*pow_u_19 * pow_v_17  
        + p_p->c2118y*20.0*21.0*pow_u_19 * pow_v_18  
        + p_p->c2119y*20.0*21.0*pow_u_19 * pow_v_19  
        + p_p->c2120y*20.0*21.0*pow_u_19 * pow_v_20  
        + p_p->c2121y*20.0*21.0*pow_u_19 * pow_v_21  ;

u2_z    = p_p->c0000z*            0.0           * pow_v_0  
        + p_p->c0001z*            0.0           * pow_v_1  
        + p_p->c0002z*            0.0           * pow_v_2  
        + p_p->c0003z*            0.0           * pow_v_3  
        + p_p->c0004z*            0.0           * pow_v_4  
        + p_p->c0005z*            0.0           * pow_v_5  
        + p_p->c0006z*            0.0           * pow_v_6  
        + p_p->c0007z*            0.0           * pow_v_7  
        + p_p->c0008z*            0.0           * pow_v_8  
        + p_p->c0009z*            0.0           * pow_v_9  
        + p_p->c0010z*            0.0           * pow_v_10  
        + p_p->c0011z*            0.0           * pow_v_11  
        + p_p->c0012z*            0.0           * pow_v_12  
        + p_p->c0013z*            0.0           * pow_v_13  
        + p_p->c0014z*            0.0           * pow_v_14  
        + p_p->c0015z*            0.0           * pow_v_15  
        + p_p->c0016z*            0.0           * pow_v_16  
        + p_p->c0017z*            0.0           * pow_v_17  
        + p_p->c0018z*            0.0           * pow_v_18  
        + p_p->c0019z*            0.0           * pow_v_19  
        + p_p->c0020z*            0.0           * pow_v_20  
        + p_p->c0021z*            0.0           * pow_v_21  ;
u2_z=u2_z + p_p->c0100z*            0.0           * pow_v_0  
        + p_p->c0101z*            0.0           * pow_v_1  
        + p_p->c0102z*            0.0           * pow_v_2  
        + p_p->c0103z*            0.0           * pow_v_3  
        + p_p->c0104z*            0.0           * pow_v_4  
        + p_p->c0105z*            0.0           * pow_v_5  
        + p_p->c0106z*            0.0           * pow_v_6  
        + p_p->c0107z*            0.0           * pow_v_7  
        + p_p->c0108z*            0.0           * pow_v_8  
        + p_p->c0109z*            0.0           * pow_v_9  
        + p_p->c0110z*            0.0           * pow_v_10  
        + p_p->c0111z*            0.0           * pow_v_11  
        + p_p->c0112z*            0.0           * pow_v_12  
        + p_p->c0113z*            0.0           * pow_v_13  
        + p_p->c0114z*            0.0           * pow_v_14  
        + p_p->c0115z*            0.0           * pow_v_15  
        + p_p->c0116z*            0.0           * pow_v_16  
        + p_p->c0117z*            0.0           * pow_v_17  
        + p_p->c0118z*            0.0           * pow_v_18  
        + p_p->c0119z*            0.0           * pow_v_19  
        + p_p->c0120z*            0.0           * pow_v_20  
        + p_p->c0121z*            0.0           * pow_v_21  ;
u2_z=u2_z + p_p->c0200z* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201z* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202z* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203z* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204z* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205z* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206z* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207z* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0208z* 1.0* 2.0*pow_u_0 * pow_v_8  
        + p_p->c0209z* 1.0* 2.0*pow_u_0 * pow_v_9  
        + p_p->c0210z* 1.0* 2.0*pow_u_0 * pow_v_10  
        + p_p->c0211z* 1.0* 2.0*pow_u_0 * pow_v_11  
        + p_p->c0212z* 1.0* 2.0*pow_u_0 * pow_v_12  
        + p_p->c0213z* 1.0* 2.0*pow_u_0 * pow_v_13  
        + p_p->c0214z* 1.0* 2.0*pow_u_0 * pow_v_14  
        + p_p->c0215z* 1.0* 2.0*pow_u_0 * pow_v_15  
        + p_p->c0216z* 1.0* 2.0*pow_u_0 * pow_v_16  
        + p_p->c0217z* 1.0* 2.0*pow_u_0 * pow_v_17  
        + p_p->c0218z* 1.0* 2.0*pow_u_0 * pow_v_18  
        + p_p->c0219z* 1.0* 2.0*pow_u_0 * pow_v_19  
        + p_p->c0220z* 1.0* 2.0*pow_u_0 * pow_v_20  
        + p_p->c0221z* 1.0* 2.0*pow_u_0 * pow_v_21  ;
u2_z=u2_z + p_p->c0300z* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301z* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302z* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303z* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304z* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305z* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306z* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307z* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0308z* 2.0* 3.0*pow_u_1 * pow_v_8  
        + p_p->c0309z* 2.0* 3.0*pow_u_1 * pow_v_9  
        + p_p->c0310z* 2.0* 3.0*pow_u_1 * pow_v_10  
        + p_p->c0311z* 2.0* 3.0*pow_u_1 * pow_v_11  
        + p_p->c0312z* 2.0* 3.0*pow_u_1 * pow_v_12  
        + p_p->c0313z* 2.0* 3.0*pow_u_1 * pow_v_13  
        + p_p->c0314z* 2.0* 3.0*pow_u_1 * pow_v_14  
        + p_p->c0315z* 2.0* 3.0*pow_u_1 * pow_v_15  
        + p_p->c0316z* 2.0* 3.0*pow_u_1 * pow_v_16  
        + p_p->c0317z* 2.0* 3.0*pow_u_1 * pow_v_17  
        + p_p->c0318z* 2.0* 3.0*pow_u_1 * pow_v_18  
        + p_p->c0319z* 2.0* 3.0*pow_u_1 * pow_v_19  
        + p_p->c0320z* 2.0* 3.0*pow_u_1 * pow_v_20  
        + p_p->c0321z* 2.0* 3.0*pow_u_1 * pow_v_21  ;
u2_z=u2_z + p_p->c0400z* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401z* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402z* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403z* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404z* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405z* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406z* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407z* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0408z* 3.0* 4.0*pow_u_2 * pow_v_8  
        + p_p->c0409z* 3.0* 4.0*pow_u_2 * pow_v_9  
        + p_p->c0410z* 3.0* 4.0*pow_u_2 * pow_v_10  
        + p_p->c0411z* 3.0* 4.0*pow_u_2 * pow_v_11  
        + p_p->c0412z* 3.0* 4.0*pow_u_2 * pow_v_12  
        + p_p->c0413z* 3.0* 4.0*pow_u_2 * pow_v_13  
        + p_p->c0414z* 3.0* 4.0*pow_u_2 * pow_v_14  
        + p_p->c0415z* 3.0* 4.0*pow_u_2 * pow_v_15  
        + p_p->c0416z* 3.0* 4.0*pow_u_2 * pow_v_16  
        + p_p->c0417z* 3.0* 4.0*pow_u_2 * pow_v_17  
        + p_p->c0418z* 3.0* 4.0*pow_u_2 * pow_v_18  
        + p_p->c0419z* 3.0* 4.0*pow_u_2 * pow_v_19  
        + p_p->c0420z* 3.0* 4.0*pow_u_2 * pow_v_20  
        + p_p->c0421z* 3.0* 4.0*pow_u_2 * pow_v_21  ;
u2_z=u2_z + p_p->c0500z* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501z* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502z* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503z* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504z* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505z* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506z* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507z* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0508z* 4.0* 5.0*pow_u_3 * pow_v_8  
        + p_p->c0509z* 4.0* 5.0*pow_u_3 * pow_v_9  
        + p_p->c0510z* 4.0* 5.0*pow_u_3 * pow_v_10  
        + p_p->c0511z* 4.0* 5.0*pow_u_3 * pow_v_11  
        + p_p->c0512z* 4.0* 5.0*pow_u_3 * pow_v_12  
        + p_p->c0513z* 4.0* 5.0*pow_u_3 * pow_v_13  
        + p_p->c0514z* 4.0* 5.0*pow_u_3 * pow_v_14  
        + p_p->c0515z* 4.0* 5.0*pow_u_3 * pow_v_15  
        + p_p->c0516z* 4.0* 5.0*pow_u_3 * pow_v_16  
        + p_p->c0517z* 4.0* 5.0*pow_u_3 * pow_v_17  
        + p_p->c0518z* 4.0* 5.0*pow_u_3 * pow_v_18  
        + p_p->c0519z* 4.0* 5.0*pow_u_3 * pow_v_19  
        + p_p->c0520z* 4.0* 5.0*pow_u_3 * pow_v_20  
        + p_p->c0521z* 4.0* 5.0*pow_u_3 * pow_v_21  ;
u2_z=u2_z + p_p->c0600z* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601z* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602z* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603z* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604z* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605z* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606z* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607z* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0608z* 5.0* 6.0*pow_u_4 * pow_v_8  
        + p_p->c0609z* 5.0* 6.0*pow_u_4 * pow_v_9  
        + p_p->c0610z* 5.0* 6.0*pow_u_4 * pow_v_10  
        + p_p->c0611z* 5.0* 6.0*pow_u_4 * pow_v_11  
        + p_p->c0612z* 5.0* 6.0*pow_u_4 * pow_v_12  
        + p_p->c0613z* 5.0* 6.0*pow_u_4 * pow_v_13  
        + p_p->c0614z* 5.0* 6.0*pow_u_4 * pow_v_14  
        + p_p->c0615z* 5.0* 6.0*pow_u_4 * pow_v_15  
        + p_p->c0616z* 5.0* 6.0*pow_u_4 * pow_v_16  
        + p_p->c0617z* 5.0* 6.0*pow_u_4 * pow_v_17  
        + p_p->c0618z* 5.0* 6.0*pow_u_4 * pow_v_18  
        + p_p->c0619z* 5.0* 6.0*pow_u_4 * pow_v_19  
        + p_p->c0620z* 5.0* 6.0*pow_u_4 * pow_v_20  
        + p_p->c0621z* 5.0* 6.0*pow_u_4 * pow_v_21  ;
u2_z=u2_z + p_p->c0700z* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701z* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702z* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703z* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704z* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705z* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706z* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707z* 6.0* 7.0*pow_u_5 * pow_v_7  
        + p_p->c0708z* 6.0* 7.0*pow_u_5 * pow_v_8  
        + p_p->c0709z* 6.0* 7.0*pow_u_5 * pow_v_9  
        + p_p->c0710z* 6.0* 7.0*pow_u_5 * pow_v_10  
        + p_p->c0711z* 6.0* 7.0*pow_u_5 * pow_v_11  
        + p_p->c0712z* 6.0* 7.0*pow_u_5 * pow_v_12  
        + p_p->c0713z* 6.0* 7.0*pow_u_5 * pow_v_13  
        + p_p->c0714z* 6.0* 7.0*pow_u_5 * pow_v_14  
        + p_p->c0715z* 6.0* 7.0*pow_u_5 * pow_v_15  
        + p_p->c0716z* 6.0* 7.0*pow_u_5 * pow_v_16  
        + p_p->c0717z* 6.0* 7.0*pow_u_5 * pow_v_17  
        + p_p->c0718z* 6.0* 7.0*pow_u_5 * pow_v_18  
        + p_p->c0719z* 6.0* 7.0*pow_u_5 * pow_v_19  
        + p_p->c0720z* 6.0* 7.0*pow_u_5 * pow_v_20  
        + p_p->c0721z* 6.0* 7.0*pow_u_5 * pow_v_21  ;
u2_z=u2_z + p_p->c0800z* 7.0* 8.0*pow_u_6 * pow_v_0  
        + p_p->c0801z* 7.0* 8.0*pow_u_6 * pow_v_1  
        + p_p->c0802z* 7.0* 8.0*pow_u_6 * pow_v_2  
        + p_p->c0803z* 7.0* 8.0*pow_u_6 * pow_v_3  
        + p_p->c0804z* 7.0* 8.0*pow_u_6 * pow_v_4  
        + p_p->c0805z* 7.0* 8.0*pow_u_6 * pow_v_5  
        + p_p->c0806z* 7.0* 8.0*pow_u_6 * pow_v_6  
        + p_p->c0807z* 7.0* 8.0*pow_u_6 * pow_v_7  
        + p_p->c0808z* 7.0* 8.0*pow_u_6 * pow_v_8  
        + p_p->c0809z* 7.0* 8.0*pow_u_6 * pow_v_9  
        + p_p->c0810z* 7.0* 8.0*pow_u_6 * pow_v_10  
        + p_p->c0811z* 7.0* 8.0*pow_u_6 * pow_v_11  
        + p_p->c0812z* 7.0* 8.0*pow_u_6 * pow_v_12  
        + p_p->c0813z* 7.0* 8.0*pow_u_6 * pow_v_13  
        + p_p->c0814z* 7.0* 8.0*pow_u_6 * pow_v_14  
        + p_p->c0815z* 7.0* 8.0*pow_u_6 * pow_v_15  
        + p_p->c0816z* 7.0* 8.0*pow_u_6 * pow_v_16  
        + p_p->c0817z* 7.0* 8.0*pow_u_6 * pow_v_17  
        + p_p->c0818z* 7.0* 8.0*pow_u_6 * pow_v_18  
        + p_p->c0819z* 7.0* 8.0*pow_u_6 * pow_v_19  
        + p_p->c0820z* 7.0* 8.0*pow_u_6 * pow_v_20  
        + p_p->c0821z* 7.0* 8.0*pow_u_6 * pow_v_21  ;
u2_z=u2_z + p_p->c0900z* 8.0* 9.0*pow_u_7 * pow_v_0  
        + p_p->c0901z* 8.0* 9.0*pow_u_7 * pow_v_1  
        + p_p->c0902z* 8.0* 9.0*pow_u_7 * pow_v_2  
        + p_p->c0903z* 8.0* 9.0*pow_u_7 * pow_v_3  
        + p_p->c0904z* 8.0* 9.0*pow_u_7 * pow_v_4  
        + p_p->c0905z* 8.0* 9.0*pow_u_7 * pow_v_5  
        + p_p->c0906z* 8.0* 9.0*pow_u_7 * pow_v_6  
        + p_p->c0907z* 8.0* 9.0*pow_u_7 * pow_v_7  
        + p_p->c0908z* 8.0* 9.0*pow_u_7 * pow_v_8  
        + p_p->c0909z* 8.0* 9.0*pow_u_7 * pow_v_9  
        + p_p->c0910z* 8.0* 9.0*pow_u_7 * pow_v_10  
        + p_p->c0911z* 8.0* 9.0*pow_u_7 * pow_v_11  
        + p_p->c0912z* 8.0* 9.0*pow_u_7 * pow_v_12  
        + p_p->c0913z* 8.0* 9.0*pow_u_7 * pow_v_13  
        + p_p->c0914z* 8.0* 9.0*pow_u_7 * pow_v_14  
        + p_p->c0915z* 8.0* 9.0*pow_u_7 * pow_v_15  
        + p_p->c0916z* 8.0* 9.0*pow_u_7 * pow_v_16  
        + p_p->c0917z* 8.0* 9.0*pow_u_7 * pow_v_17  
        + p_p->c0918z* 8.0* 9.0*pow_u_7 * pow_v_18  
        + p_p->c0919z* 8.0* 9.0*pow_u_7 * pow_v_19  
        + p_p->c0920z* 8.0* 9.0*pow_u_7 * pow_v_20  
        + p_p->c0921z* 8.0* 9.0*pow_u_7 * pow_v_21  ;
u2_z=u2_z + p_p->c1000z* 9.0*10.0*pow_u_8 * pow_v_0  
        + p_p->c1001z* 9.0*10.0*pow_u_8 * pow_v_1  
        + p_p->c1002z* 9.0*10.0*pow_u_8 * pow_v_2  
        + p_p->c1003z* 9.0*10.0*pow_u_8 * pow_v_3  
        + p_p->c1004z* 9.0*10.0*pow_u_8 * pow_v_4  
        + p_p->c1005z* 9.0*10.0*pow_u_8 * pow_v_5  
        + p_p->c1006z* 9.0*10.0*pow_u_8 * pow_v_6  
        + p_p->c1007z* 9.0*10.0*pow_u_8 * pow_v_7  
        + p_p->c1008z* 9.0*10.0*pow_u_8 * pow_v_8  
        + p_p->c1009z* 9.0*10.0*pow_u_8 * pow_v_9  
        + p_p->c1010z* 9.0*10.0*pow_u_8 * pow_v_10  
        + p_p->c1011z* 9.0*10.0*pow_u_8 * pow_v_11  
        + p_p->c1012z* 9.0*10.0*pow_u_8 * pow_v_12  
        + p_p->c1013z* 9.0*10.0*pow_u_8 * pow_v_13  
        + p_p->c1014z* 9.0*10.0*pow_u_8 * pow_v_14  
        + p_p->c1015z* 9.0*10.0*pow_u_8 * pow_v_15  
        + p_p->c1016z* 9.0*10.0*pow_u_8 * pow_v_16  
        + p_p->c1017z* 9.0*10.0*pow_u_8 * pow_v_17  
        + p_p->c1018z* 9.0*10.0*pow_u_8 * pow_v_18  
        + p_p->c1019z* 9.0*10.0*pow_u_8 * pow_v_19  
        + p_p->c1020z* 9.0*10.0*pow_u_8 * pow_v_20  
        + p_p->c1021z* 9.0*10.0*pow_u_8 * pow_v_21  ;
u2_z=u2_z + p_p->c1100z*10.0*11.0*pow_u_9 * pow_v_0  
        + p_p->c1101z*10.0*11.0*pow_u_9 * pow_v_1  
        + p_p->c1102z*10.0*11.0*pow_u_9 * pow_v_2  
        + p_p->c1103z*10.0*11.0*pow_u_9 * pow_v_3  
        + p_p->c1104z*10.0*11.0*pow_u_9 * pow_v_4  
        + p_p->c1105z*10.0*11.0*pow_u_9 * pow_v_5  
        + p_p->c1106z*10.0*11.0*pow_u_9 * pow_v_6  
        + p_p->c1107z*10.0*11.0*pow_u_9 * pow_v_7  
        + p_p->c1108z*10.0*11.0*pow_u_9 * pow_v_8  
        + p_p->c1109z*10.0*11.0*pow_u_9 * pow_v_9  
        + p_p->c1110z*10.0*11.0*pow_u_9 * pow_v_10  
        + p_p->c1111z*10.0*11.0*pow_u_9 * pow_v_11  
        + p_p->c1112z*10.0*11.0*pow_u_9 * pow_v_12  
        + p_p->c1113z*10.0*11.0*pow_u_9 * pow_v_13  
        + p_p->c1114z*10.0*11.0*pow_u_9 * pow_v_14  
        + p_p->c1115z*10.0*11.0*pow_u_9 * pow_v_15  
        + p_p->c1116z*10.0*11.0*pow_u_9 * pow_v_16  
        + p_p->c1117z*10.0*11.0*pow_u_9 * pow_v_17  
        + p_p->c1118z*10.0*11.0*pow_u_9 * pow_v_18  
        + p_p->c1119z*10.0*11.0*pow_u_9 * pow_v_19  
        + p_p->c1120z*10.0*11.0*pow_u_9 * pow_v_20  
        + p_p->c1121z*10.0*11.0*pow_u_9 * pow_v_21  ;
u2_z=u2_z + p_p->c1200z*11.0*12.0*pow_u_10 * pow_v_0  
        + p_p->c1201z*11.0*12.0*pow_u_10 * pow_v_1  
        + p_p->c1202z*11.0*12.0*pow_u_10 * pow_v_2  
        + p_p->c1203z*11.0*12.0*pow_u_10 * pow_v_3  
        + p_p->c1204z*11.0*12.0*pow_u_10 * pow_v_4  
        + p_p->c1205z*11.0*12.0*pow_u_10 * pow_v_5  
        + p_p->c1206z*11.0*12.0*pow_u_10 * pow_v_6  
        + p_p->c1207z*11.0*12.0*pow_u_10 * pow_v_7  
        + p_p->c1208z*11.0*12.0*pow_u_10 * pow_v_8  
        + p_p->c1209z*11.0*12.0*pow_u_10 * pow_v_9  
        + p_p->c1210z*11.0*12.0*pow_u_10 * pow_v_10  
        + p_p->c1211z*11.0*12.0*pow_u_10 * pow_v_11  
        + p_p->c1212z*11.0*12.0*pow_u_10 * pow_v_12  
        + p_p->c1213z*11.0*12.0*pow_u_10 * pow_v_13  
        + p_p->c1214z*11.0*12.0*pow_u_10 * pow_v_14  
        + p_p->c1215z*11.0*12.0*pow_u_10 * pow_v_15  
        + p_p->c1216z*11.0*12.0*pow_u_10 * pow_v_16  
        + p_p->c1217z*11.0*12.0*pow_u_10 * pow_v_17  
        + p_p->c1218z*11.0*12.0*pow_u_10 * pow_v_18  
        + p_p->c1219z*11.0*12.0*pow_u_10 * pow_v_19  
        + p_p->c1220z*11.0*12.0*pow_u_10 * pow_v_20  
        + p_p->c1221z*11.0*12.0*pow_u_10 * pow_v_21  ;
u2_z=u2_z + p_p->c1300z*12.0*13.0*pow_u_11 * pow_v_0  
        + p_p->c1301z*12.0*13.0*pow_u_11 * pow_v_1  
        + p_p->c1302z*12.0*13.0*pow_u_11 * pow_v_2  
        + p_p->c1303z*12.0*13.0*pow_u_11 * pow_v_3  
        + p_p->c1304z*12.0*13.0*pow_u_11 * pow_v_4  
        + p_p->c1305z*12.0*13.0*pow_u_11 * pow_v_5  
        + p_p->c1306z*12.0*13.0*pow_u_11 * pow_v_6  
        + p_p->c1307z*12.0*13.0*pow_u_11 * pow_v_7  
        + p_p->c1308z*12.0*13.0*pow_u_11 * pow_v_8  
        + p_p->c1309z*12.0*13.0*pow_u_11 * pow_v_9  
        + p_p->c1310z*12.0*13.0*pow_u_11 * pow_v_10  
        + p_p->c1311z*12.0*13.0*pow_u_11 * pow_v_11  
        + p_p->c1312z*12.0*13.0*pow_u_11 * pow_v_12  
        + p_p->c1313z*12.0*13.0*pow_u_11 * pow_v_13  
        + p_p->c1314z*12.0*13.0*pow_u_11 * pow_v_14  
        + p_p->c1315z*12.0*13.0*pow_u_11 * pow_v_15  
        + p_p->c1316z*12.0*13.0*pow_u_11 * pow_v_16  
        + p_p->c1317z*12.0*13.0*pow_u_11 * pow_v_17  
        + p_p->c1318z*12.0*13.0*pow_u_11 * pow_v_18  
        + p_p->c1319z*12.0*13.0*pow_u_11 * pow_v_19  
        + p_p->c1320z*12.0*13.0*pow_u_11 * pow_v_20  
        + p_p->c1321z*12.0*13.0*pow_u_11 * pow_v_21  ;
u2_z=u2_z + p_p->c1400z*13.0*14.0*pow_u_12 * pow_v_0  
        + p_p->c1401z*13.0*14.0*pow_u_12 * pow_v_1  
        + p_p->c1402z*13.0*14.0*pow_u_12 * pow_v_2  
        + p_p->c1403z*13.0*14.0*pow_u_12 * pow_v_3  
        + p_p->c1404z*13.0*14.0*pow_u_12 * pow_v_4  
        + p_p->c1405z*13.0*14.0*pow_u_12 * pow_v_5  
        + p_p->c1406z*13.0*14.0*pow_u_12 * pow_v_6  
        + p_p->c1407z*13.0*14.0*pow_u_12 * pow_v_7  
        + p_p->c1408z*13.0*14.0*pow_u_12 * pow_v_8  
        + p_p->c1409z*13.0*14.0*pow_u_12 * pow_v_9  
        + p_p->c1410z*13.0*14.0*pow_u_12 * pow_v_10  
        + p_p->c1411z*13.0*14.0*pow_u_12 * pow_v_11  
        + p_p->c1412z*13.0*14.0*pow_u_12 * pow_v_12  
        + p_p->c1413z*13.0*14.0*pow_u_12 * pow_v_13  
        + p_p->c1414z*13.0*14.0*pow_u_12 * pow_v_14  
        + p_p->c1415z*13.0*14.0*pow_u_12 * pow_v_15  
        + p_p->c1416z*13.0*14.0*pow_u_12 * pow_v_16  
        + p_p->c1417z*13.0*14.0*pow_u_12 * pow_v_17  
        + p_p->c1418z*13.0*14.0*pow_u_12 * pow_v_18  
        + p_p->c1419z*13.0*14.0*pow_u_12 * pow_v_19  
        + p_p->c1420z*13.0*14.0*pow_u_12 * pow_v_20  
        + p_p->c1421z*13.0*14.0*pow_u_12 * pow_v_21  ;
u2_z=u2_z + p_p->c1500z*14.0*15.0*pow_u_13 * pow_v_0  
        + p_p->c1501z*14.0*15.0*pow_u_13 * pow_v_1  
        + p_p->c1502z*14.0*15.0*pow_u_13 * pow_v_2  
        + p_p->c1503z*14.0*15.0*pow_u_13 * pow_v_3  
        + p_p->c1504z*14.0*15.0*pow_u_13 * pow_v_4  
        + p_p->c1505z*14.0*15.0*pow_u_13 * pow_v_5  
        + p_p->c1506z*14.0*15.0*pow_u_13 * pow_v_6  
        + p_p->c1507z*14.0*15.0*pow_u_13 * pow_v_7  
        + p_p->c1508z*14.0*15.0*pow_u_13 * pow_v_8  
        + p_p->c1509z*14.0*15.0*pow_u_13 * pow_v_9  
        + p_p->c1510z*14.0*15.0*pow_u_13 * pow_v_10  
        + p_p->c1511z*14.0*15.0*pow_u_13 * pow_v_11  
        + p_p->c1512z*14.0*15.0*pow_u_13 * pow_v_12  
        + p_p->c1513z*14.0*15.0*pow_u_13 * pow_v_13  
        + p_p->c1514z*14.0*15.0*pow_u_13 * pow_v_14  
        + p_p->c1515z*14.0*15.0*pow_u_13 * pow_v_15  
        + p_p->c1516z*14.0*15.0*pow_u_13 * pow_v_16  
        + p_p->c1517z*14.0*15.0*pow_u_13 * pow_v_17  
        + p_p->c1518z*14.0*15.0*pow_u_13 * pow_v_18  
        + p_p->c1519z*14.0*15.0*pow_u_13 * pow_v_19  
        + p_p->c1520z*14.0*15.0*pow_u_13 * pow_v_20  
        + p_p->c1521z*14.0*15.0*pow_u_13 * pow_v_21  ;
u2_z=u2_z + p_p->c1600z*15.0*16.0*pow_u_14 * pow_v_0  
        + p_p->c1601z*15.0*16.0*pow_u_14 * pow_v_1  
        + p_p->c1602z*15.0*16.0*pow_u_14 * pow_v_2  
        + p_p->c1603z*15.0*16.0*pow_u_14 * pow_v_3  
        + p_p->c1604z*15.0*16.0*pow_u_14 * pow_v_4  
        + p_p->c1605z*15.0*16.0*pow_u_14 * pow_v_5  
        + p_p->c1606z*15.0*16.0*pow_u_14 * pow_v_6  
        + p_p->c1607z*15.0*16.0*pow_u_14 * pow_v_7  
        + p_p->c1608z*15.0*16.0*pow_u_14 * pow_v_8  
        + p_p->c1609z*15.0*16.0*pow_u_14 * pow_v_9  
        + p_p->c1610z*15.0*16.0*pow_u_14 * pow_v_10  
        + p_p->c1611z*15.0*16.0*pow_u_14 * pow_v_11  
        + p_p->c1612z*15.0*16.0*pow_u_14 * pow_v_12  
        + p_p->c1613z*15.0*16.0*pow_u_14 * pow_v_13  
        + p_p->c1614z*15.0*16.0*pow_u_14 * pow_v_14  
        + p_p->c1615z*15.0*16.0*pow_u_14 * pow_v_15  
        + p_p->c1616z*15.0*16.0*pow_u_14 * pow_v_16  
        + p_p->c1617z*15.0*16.0*pow_u_14 * pow_v_17  
        + p_p->c1618z*15.0*16.0*pow_u_14 * pow_v_18  
        + p_p->c1619z*15.0*16.0*pow_u_14 * pow_v_19  
        + p_p->c1620z*15.0*16.0*pow_u_14 * pow_v_20  
        + p_p->c1621z*15.0*16.0*pow_u_14 * pow_v_21  ;
u2_z=u2_z + p_p->c1700z*16.0*17.0*pow_u_15 * pow_v_0  
        + p_p->c1701z*16.0*17.0*pow_u_15 * pow_v_1  
        + p_p->c1702z*16.0*17.0*pow_u_15 * pow_v_2  
        + p_p->c1703z*16.0*17.0*pow_u_15 * pow_v_3  
        + p_p->c1704z*16.0*17.0*pow_u_15 * pow_v_4  
        + p_p->c1705z*16.0*17.0*pow_u_15 * pow_v_5  
        + p_p->c1706z*16.0*17.0*pow_u_15 * pow_v_6  
        + p_p->c1707z*16.0*17.0*pow_u_15 * pow_v_7  
        + p_p->c1708z*16.0*17.0*pow_u_15 * pow_v_8  
        + p_p->c1709z*16.0*17.0*pow_u_15 * pow_v_9  
        + p_p->c1710z*16.0*17.0*pow_u_15 * pow_v_10  
        + p_p->c1711z*16.0*17.0*pow_u_15 * pow_v_11  
        + p_p->c1712z*16.0*17.0*pow_u_15 * pow_v_12  
        + p_p->c1713z*16.0*17.0*pow_u_15 * pow_v_13  
        + p_p->c1714z*16.0*17.0*pow_u_15 * pow_v_14  
        + p_p->c1715z*16.0*17.0*pow_u_15 * pow_v_15  
        + p_p->c1716z*16.0*17.0*pow_u_15 * pow_v_16  
        + p_p->c1717z*16.0*17.0*pow_u_15 * pow_v_17  
        + p_p->c1718z*16.0*17.0*pow_u_15 * pow_v_18  
        + p_p->c1719z*16.0*17.0*pow_u_15 * pow_v_19  
        + p_p->c1720z*16.0*17.0*pow_u_15 * pow_v_20  
        + p_p->c1721z*16.0*17.0*pow_u_15 * pow_v_21  ;
u2_z=u2_z + p_p->c1800z*17.0*18.0*pow_u_16 * pow_v_0  
        + p_p->c1801z*17.0*18.0*pow_u_16 * pow_v_1  
        + p_p->c1802z*17.0*18.0*pow_u_16 * pow_v_2  
        + p_p->c1803z*17.0*18.0*pow_u_16 * pow_v_3  
        + p_p->c1804z*17.0*18.0*pow_u_16 * pow_v_4  
        + p_p->c1805z*17.0*18.0*pow_u_16 * pow_v_5  
        + p_p->c1806z*17.0*18.0*pow_u_16 * pow_v_6  
        + p_p->c1807z*17.0*18.0*pow_u_16 * pow_v_7  
        + p_p->c1808z*17.0*18.0*pow_u_16 * pow_v_8  
        + p_p->c1809z*17.0*18.0*pow_u_16 * pow_v_9  
        + p_p->c1810z*17.0*18.0*pow_u_16 * pow_v_10  
        + p_p->c1811z*17.0*18.0*pow_u_16 * pow_v_11  
        + p_p->c1812z*17.0*18.0*pow_u_16 * pow_v_12  
        + p_p->c1813z*17.0*18.0*pow_u_16 * pow_v_13  
        + p_p->c1814z*17.0*18.0*pow_u_16 * pow_v_14  
        + p_p->c1815z*17.0*18.0*pow_u_16 * pow_v_15  
        + p_p->c1816z*17.0*18.0*pow_u_16 * pow_v_16  
        + p_p->c1817z*17.0*18.0*pow_u_16 * pow_v_17  
        + p_p->c1818z*17.0*18.0*pow_u_16 * pow_v_18  
        + p_p->c1819z*17.0*18.0*pow_u_16 * pow_v_19  
        + p_p->c1820z*17.0*18.0*pow_u_16 * pow_v_20  
        + p_p->c1821z*17.0*18.0*pow_u_16 * pow_v_21  ;
u2_z=u2_z + p_p->c1900z*18.0*19.0*pow_u_17 * pow_v_0  
        + p_p->c1901z*18.0*19.0*pow_u_17 * pow_v_1  
        + p_p->c1902z*18.0*19.0*pow_u_17 * pow_v_2  
        + p_p->c1903z*18.0*19.0*pow_u_17 * pow_v_3  
        + p_p->c1904z*18.0*19.0*pow_u_17 * pow_v_4  
        + p_p->c1905z*18.0*19.0*pow_u_17 * pow_v_5  
        + p_p->c1906z*18.0*19.0*pow_u_17 * pow_v_6  
        + p_p->c1907z*18.0*19.0*pow_u_17 * pow_v_7  
        + p_p->c1908z*18.0*19.0*pow_u_17 * pow_v_8  
        + p_p->c1909z*18.0*19.0*pow_u_17 * pow_v_9  
        + p_p->c1910z*18.0*19.0*pow_u_17 * pow_v_10  
        + p_p->c1911z*18.0*19.0*pow_u_17 * pow_v_11  
        + p_p->c1912z*18.0*19.0*pow_u_17 * pow_v_12  
        + p_p->c1913z*18.0*19.0*pow_u_17 * pow_v_13  
        + p_p->c1914z*18.0*19.0*pow_u_17 * pow_v_14  
        + p_p->c1915z*18.0*19.0*pow_u_17 * pow_v_15  
        + p_p->c1916z*18.0*19.0*pow_u_17 * pow_v_16  
        + p_p->c1917z*18.0*19.0*pow_u_17 * pow_v_17  
        + p_p->c1918z*18.0*19.0*pow_u_17 * pow_v_18  
        + p_p->c1919z*18.0*19.0*pow_u_17 * pow_v_19  
        + p_p->c1920z*18.0*19.0*pow_u_17 * pow_v_20  
        + p_p->c1921z*18.0*19.0*pow_u_17 * pow_v_21  ;
u2_z=u2_z + p_p->c2000z*19.0*20.0*pow_u_18 * pow_v_0  
        + p_p->c2001z*19.0*20.0*pow_u_18 * pow_v_1  
        + p_p->c2002z*19.0*20.0*pow_u_18 * pow_v_2  
        + p_p->c2003z*19.0*20.0*pow_u_18 * pow_v_3  
        + p_p->c2004z*19.0*20.0*pow_u_18 * pow_v_4  
        + p_p->c2005z*19.0*20.0*pow_u_18 * pow_v_5  
        + p_p->c2006z*19.0*20.0*pow_u_18 * pow_v_6  
        + p_p->c2007z*19.0*20.0*pow_u_18 * pow_v_7  
        + p_p->c2008z*19.0*20.0*pow_u_18 * pow_v_8  
        + p_p->c2009z*19.0*20.0*pow_u_18 * pow_v_9  
        + p_p->c2010z*19.0*20.0*pow_u_18 * pow_v_10  
        + p_p->c2011z*19.0*20.0*pow_u_18 * pow_v_11  
        + p_p->c2012z*19.0*20.0*pow_u_18 * pow_v_12  
        + p_p->c2013z*19.0*20.0*pow_u_18 * pow_v_13  
        + p_p->c2014z*19.0*20.0*pow_u_18 * pow_v_14  
        + p_p->c2015z*19.0*20.0*pow_u_18 * pow_v_15  
        + p_p->c2016z*19.0*20.0*pow_u_18 * pow_v_16  
        + p_p->c2017z*19.0*20.0*pow_u_18 * pow_v_17  
        + p_p->c2018z*19.0*20.0*pow_u_18 * pow_v_18  
        + p_p->c2019z*19.0*20.0*pow_u_18 * pow_v_19  
        + p_p->c2020z*19.0*20.0*pow_u_18 * pow_v_20  
        + p_p->c2021z*19.0*20.0*pow_u_18 * pow_v_21  ;
u2_z=u2_z + p_p->c2100z*20.0*21.0*pow_u_19 * pow_v_0  
        + p_p->c2101z*20.0*21.0*pow_u_19 * pow_v_1  
        + p_p->c2102z*20.0*21.0*pow_u_19 * pow_v_2  
        + p_p->c2103z*20.0*21.0*pow_u_19 * pow_v_3  
        + p_p->c2104z*20.0*21.0*pow_u_19 * pow_v_4  
        + p_p->c2105z*20.0*21.0*pow_u_19 * pow_v_5  
        + p_p->c2106z*20.0*21.0*pow_u_19 * pow_v_6  
        + p_p->c2107z*20.0*21.0*pow_u_19 * pow_v_7  
        + p_p->c2108z*20.0*21.0*pow_u_19 * pow_v_8  
        + p_p->c2109z*20.0*21.0*pow_u_19 * pow_v_9  
        + p_p->c2110z*20.0*21.0*pow_u_19 * pow_v_10  
        + p_p->c2111z*20.0*21.0*pow_u_19 * pow_v_11  
        + p_p->c2112z*20.0*21.0*pow_u_19 * pow_v_12  
        + p_p->c2113z*20.0*21.0*pow_u_19 * pow_v_13  
        + p_p->c2114z*20.0*21.0*pow_u_19 * pow_v_14  
        + p_p->c2115z*20.0*21.0*pow_u_19 * pow_v_15  
        + p_p->c2116z*20.0*21.0*pow_u_19 * pow_v_16  
        + p_p->c2117z*20.0*21.0*pow_u_19 * pow_v_17  
        + p_p->c2118z*20.0*21.0*pow_u_19 * pow_v_18  
        + p_p->c2119z*20.0*21.0*pow_u_19 * pow_v_19  
        + p_p->c2120z*20.0*21.0*pow_u_19 * pow_v_20  
        + p_p->c2121z*20.0*21.0*pow_u_19 * pow_v_21  ;

v2_x    = p_p->c0000x * pow_u_0  *            0.0          
        + p_p->c0001x * pow_u_0  *            0.0          
        + p_p->c0002x * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003x * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004x * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005x * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006x * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007x * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0008x * pow_u_0  * 7.0* 8.0*pow_v_6
        + p_p->c0009x * pow_u_0  * 8.0* 9.0*pow_v_7
        + p_p->c0010x * pow_u_0  * 9.0*10.0*pow_v_8
        + p_p->c0011x * pow_u_0  *10.0*11.0*pow_v_9
        + p_p->c0012x * pow_u_0  *11.0*12.0*pow_v_10
        + p_p->c0013x * pow_u_0  *12.0*13.0*pow_v_11
        + p_p->c0014x * pow_u_0  *13.0*14.0*pow_v_12
        + p_p->c0015x * pow_u_0  *14.0*15.0*pow_v_13
        + p_p->c0016x * pow_u_0  *15.0*16.0*pow_v_14
        + p_p->c0017x * pow_u_0  *16.0*17.0*pow_v_15
        + p_p->c0018x * pow_u_0  *17.0*18.0*pow_v_16
        + p_p->c0019x * pow_u_0  *18.0*19.0*pow_v_17
        + p_p->c0020x * pow_u_0  *19.0*20.0*pow_v_18
        + p_p->c0021x * pow_u_0  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0100x * pow_u_1  *            0.0          
        + p_p->c0101x * pow_u_1  *            0.0          
        + p_p->c0102x * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103x * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104x * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105x * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106x * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107x * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0108x * pow_u_1  * 7.0* 8.0*pow_v_6
        + p_p->c0109x * pow_u_1  * 8.0* 9.0*pow_v_7
        + p_p->c0110x * pow_u_1  * 9.0*10.0*pow_v_8
        + p_p->c0111x * pow_u_1  *10.0*11.0*pow_v_9
        + p_p->c0112x * pow_u_1  *11.0*12.0*pow_v_10
        + p_p->c0113x * pow_u_1  *12.0*13.0*pow_v_11
        + p_p->c0114x * pow_u_1  *13.0*14.0*pow_v_12
        + p_p->c0115x * pow_u_1  *14.0*15.0*pow_v_13
        + p_p->c0116x * pow_u_1  *15.0*16.0*pow_v_14
        + p_p->c0117x * pow_u_1  *16.0*17.0*pow_v_15
        + p_p->c0118x * pow_u_1  *17.0*18.0*pow_v_16
        + p_p->c0119x * pow_u_1  *18.0*19.0*pow_v_17
        + p_p->c0120x * pow_u_1  *19.0*20.0*pow_v_18
        + p_p->c0121x * pow_u_1  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0200x * pow_u_2  *            0.0          
        + p_p->c0201x * pow_u_2  *            0.0          
        + p_p->c0202x * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203x * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204x * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205x * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206x * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207x * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0208x * pow_u_2  * 7.0* 8.0*pow_v_6
        + p_p->c0209x * pow_u_2  * 8.0* 9.0*pow_v_7
        + p_p->c0210x * pow_u_2  * 9.0*10.0*pow_v_8
        + p_p->c0211x * pow_u_2  *10.0*11.0*pow_v_9
        + p_p->c0212x * pow_u_2  *11.0*12.0*pow_v_10
        + p_p->c0213x * pow_u_2  *12.0*13.0*pow_v_11
        + p_p->c0214x * pow_u_2  *13.0*14.0*pow_v_12
        + p_p->c0215x * pow_u_2  *14.0*15.0*pow_v_13
        + p_p->c0216x * pow_u_2  *15.0*16.0*pow_v_14
        + p_p->c0217x * pow_u_2  *16.0*17.0*pow_v_15
        + p_p->c0218x * pow_u_2  *17.0*18.0*pow_v_16
        + p_p->c0219x * pow_u_2  *18.0*19.0*pow_v_17
        + p_p->c0220x * pow_u_2  *19.0*20.0*pow_v_18
        + p_p->c0221x * pow_u_2  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0300x * pow_u_3  *            0.0          
        + p_p->c0301x * pow_u_3  *            0.0          
        + p_p->c0302x * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303x * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304x * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305x * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306x * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307x * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0308x * pow_u_3  * 7.0* 8.0*pow_v_6
        + p_p->c0309x * pow_u_3  * 8.0* 9.0*pow_v_7
        + p_p->c0310x * pow_u_3  * 9.0*10.0*pow_v_8
        + p_p->c0311x * pow_u_3  *10.0*11.0*pow_v_9
        + p_p->c0312x * pow_u_3  *11.0*12.0*pow_v_10
        + p_p->c0313x * pow_u_3  *12.0*13.0*pow_v_11
        + p_p->c0314x * pow_u_3  *13.0*14.0*pow_v_12
        + p_p->c0315x * pow_u_3  *14.0*15.0*pow_v_13
        + p_p->c0316x * pow_u_3  *15.0*16.0*pow_v_14
        + p_p->c0317x * pow_u_3  *16.0*17.0*pow_v_15
        + p_p->c0318x * pow_u_3  *17.0*18.0*pow_v_16
        + p_p->c0319x * pow_u_3  *18.0*19.0*pow_v_17
        + p_p->c0320x * pow_u_3  *19.0*20.0*pow_v_18
        + p_p->c0321x * pow_u_3  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0400x * pow_u_4  *            0.0          
        + p_p->c0401x * pow_u_4  *            0.0          
        + p_p->c0402x * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403x * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404x * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405x * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406x * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407x * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0408x * pow_u_4  * 7.0* 8.0*pow_v_6
        + p_p->c0409x * pow_u_4  * 8.0* 9.0*pow_v_7
        + p_p->c0410x * pow_u_4  * 9.0*10.0*pow_v_8
        + p_p->c0411x * pow_u_4  *10.0*11.0*pow_v_9
        + p_p->c0412x * pow_u_4  *11.0*12.0*pow_v_10
        + p_p->c0413x * pow_u_4  *12.0*13.0*pow_v_11
        + p_p->c0414x * pow_u_4  *13.0*14.0*pow_v_12
        + p_p->c0415x * pow_u_4  *14.0*15.0*pow_v_13
        + p_p->c0416x * pow_u_4  *15.0*16.0*pow_v_14
        + p_p->c0417x * pow_u_4  *16.0*17.0*pow_v_15
        + p_p->c0418x * pow_u_4  *17.0*18.0*pow_v_16
        + p_p->c0419x * pow_u_4  *18.0*19.0*pow_v_17
        + p_p->c0420x * pow_u_4  *19.0*20.0*pow_v_18
        + p_p->c0421x * pow_u_4  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0500x * pow_u_5  *            0.0          
        + p_p->c0501x * pow_u_5  *            0.0          
        + p_p->c0502x * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503x * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504x * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505x * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506x * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507x * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0508x * pow_u_5  * 7.0* 8.0*pow_v_6
        + p_p->c0509x * pow_u_5  * 8.0* 9.0*pow_v_7
        + p_p->c0510x * pow_u_5  * 9.0*10.0*pow_v_8
        + p_p->c0511x * pow_u_5  *10.0*11.0*pow_v_9
        + p_p->c0512x * pow_u_5  *11.0*12.0*pow_v_10
        + p_p->c0513x * pow_u_5  *12.0*13.0*pow_v_11
        + p_p->c0514x * pow_u_5  *13.0*14.0*pow_v_12
        + p_p->c0515x * pow_u_5  *14.0*15.0*pow_v_13
        + p_p->c0516x * pow_u_5  *15.0*16.0*pow_v_14
        + p_p->c0517x * pow_u_5  *16.0*17.0*pow_v_15
        + p_p->c0518x * pow_u_5  *17.0*18.0*pow_v_16
        + p_p->c0519x * pow_u_5  *18.0*19.0*pow_v_17
        + p_p->c0520x * pow_u_5  *19.0*20.0*pow_v_18
        + p_p->c0521x * pow_u_5  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0600x * pow_u_6  *            0.0          
        + p_p->c0601x * pow_u_6  *            0.0          
        + p_p->c0602x * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603x * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604x * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605x * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606x * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607x * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0608x * pow_u_6  * 7.0* 8.0*pow_v_6
        + p_p->c0609x * pow_u_6  * 8.0* 9.0*pow_v_7
        + p_p->c0610x * pow_u_6  * 9.0*10.0*pow_v_8
        + p_p->c0611x * pow_u_6  *10.0*11.0*pow_v_9
        + p_p->c0612x * pow_u_6  *11.0*12.0*pow_v_10
        + p_p->c0613x * pow_u_6  *12.0*13.0*pow_v_11
        + p_p->c0614x * pow_u_6  *13.0*14.0*pow_v_12
        + p_p->c0615x * pow_u_6  *14.0*15.0*pow_v_13
        + p_p->c0616x * pow_u_6  *15.0*16.0*pow_v_14
        + p_p->c0617x * pow_u_6  *16.0*17.0*pow_v_15
        + p_p->c0618x * pow_u_6  *17.0*18.0*pow_v_16
        + p_p->c0619x * pow_u_6  *18.0*19.0*pow_v_17
        + p_p->c0620x * pow_u_6  *19.0*20.0*pow_v_18
        + p_p->c0621x * pow_u_6  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0700x * pow_u_7  *            0.0          
        + p_p->c0701x * pow_u_7  *            0.0          
        + p_p->c0702x * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703x * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704x * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705x * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706x * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707x * pow_u_7  * 6.0* 7.0*pow_v_5
        + p_p->c0708x * pow_u_7  * 7.0* 8.0*pow_v_6
        + p_p->c0709x * pow_u_7  * 8.0* 9.0*pow_v_7
        + p_p->c0710x * pow_u_7  * 9.0*10.0*pow_v_8
        + p_p->c0711x * pow_u_7  *10.0*11.0*pow_v_9
        + p_p->c0712x * pow_u_7  *11.0*12.0*pow_v_10
        + p_p->c0713x * pow_u_7  *12.0*13.0*pow_v_11
        + p_p->c0714x * pow_u_7  *13.0*14.0*pow_v_12
        + p_p->c0715x * pow_u_7  *14.0*15.0*pow_v_13
        + p_p->c0716x * pow_u_7  *15.0*16.0*pow_v_14
        + p_p->c0717x * pow_u_7  *16.0*17.0*pow_v_15
        + p_p->c0718x * pow_u_7  *17.0*18.0*pow_v_16
        + p_p->c0719x * pow_u_7  *18.0*19.0*pow_v_17
        + p_p->c0720x * pow_u_7  *19.0*20.0*pow_v_18
        + p_p->c0721x * pow_u_7  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0800x * pow_u_8  *            0.0          
        + p_p->c0801x * pow_u_8  *            0.0          
        + p_p->c0802x * pow_u_8  * 1.0* 2.0*pow_v_0
        + p_p->c0803x * pow_u_8  * 2.0* 3.0*pow_v_1
        + p_p->c0804x * pow_u_8  * 3.0* 4.0*pow_v_2
        + p_p->c0805x * pow_u_8  * 4.0* 5.0*pow_v_3
        + p_p->c0806x * pow_u_8  * 5.0* 6.0*pow_v_4
        + p_p->c0807x * pow_u_8  * 6.0* 7.0*pow_v_5
        + p_p->c0808x * pow_u_8  * 7.0* 8.0*pow_v_6
        + p_p->c0809x * pow_u_8  * 8.0* 9.0*pow_v_7
        + p_p->c0810x * pow_u_8  * 9.0*10.0*pow_v_8
        + p_p->c0811x * pow_u_8  *10.0*11.0*pow_v_9
        + p_p->c0812x * pow_u_8  *11.0*12.0*pow_v_10
        + p_p->c0813x * pow_u_8  *12.0*13.0*pow_v_11
        + p_p->c0814x * pow_u_8  *13.0*14.0*pow_v_12
        + p_p->c0815x * pow_u_8  *14.0*15.0*pow_v_13
        + p_p->c0816x * pow_u_8  *15.0*16.0*pow_v_14
        + p_p->c0817x * pow_u_8  *16.0*17.0*pow_v_15
        + p_p->c0818x * pow_u_8  *17.0*18.0*pow_v_16
        + p_p->c0819x * pow_u_8  *18.0*19.0*pow_v_17
        + p_p->c0820x * pow_u_8  *19.0*20.0*pow_v_18
        + p_p->c0821x * pow_u_8  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c0900x * pow_u_9  *            0.0          
        + p_p->c0901x * pow_u_9  *            0.0          
        + p_p->c0902x * pow_u_9  * 1.0* 2.0*pow_v_0
        + p_p->c0903x * pow_u_9  * 2.0* 3.0*pow_v_1
        + p_p->c0904x * pow_u_9  * 3.0* 4.0*pow_v_2
        + p_p->c0905x * pow_u_9  * 4.0* 5.0*pow_v_3
        + p_p->c0906x * pow_u_9  * 5.0* 6.0*pow_v_4
        + p_p->c0907x * pow_u_9  * 6.0* 7.0*pow_v_5
        + p_p->c0908x * pow_u_9  * 7.0* 8.0*pow_v_6
        + p_p->c0909x * pow_u_9  * 8.0* 9.0*pow_v_7
        + p_p->c0910x * pow_u_9  * 9.0*10.0*pow_v_8
        + p_p->c0911x * pow_u_9  *10.0*11.0*pow_v_9
        + p_p->c0912x * pow_u_9  *11.0*12.0*pow_v_10
        + p_p->c0913x * pow_u_9  *12.0*13.0*pow_v_11
        + p_p->c0914x * pow_u_9  *13.0*14.0*pow_v_12
        + p_p->c0915x * pow_u_9  *14.0*15.0*pow_v_13
        + p_p->c0916x * pow_u_9  *15.0*16.0*pow_v_14
        + p_p->c0917x * pow_u_9  *16.0*17.0*pow_v_15
        + p_p->c0918x * pow_u_9  *17.0*18.0*pow_v_16
        + p_p->c0919x * pow_u_9  *18.0*19.0*pow_v_17
        + p_p->c0920x * pow_u_9  *19.0*20.0*pow_v_18
        + p_p->c0921x * pow_u_9  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1000x * pow_u_10  *            0.0          
        + p_p->c1001x * pow_u_10  *            0.0          
        + p_p->c1002x * pow_u_10  * 1.0* 2.0*pow_v_0
        + p_p->c1003x * pow_u_10  * 2.0* 3.0*pow_v_1
        + p_p->c1004x * pow_u_10  * 3.0* 4.0*pow_v_2
        + p_p->c1005x * pow_u_10  * 4.0* 5.0*pow_v_3
        + p_p->c1006x * pow_u_10  * 5.0* 6.0*pow_v_4
        + p_p->c1007x * pow_u_10  * 6.0* 7.0*pow_v_5
        + p_p->c1008x * pow_u_10  * 7.0* 8.0*pow_v_6
        + p_p->c1009x * pow_u_10  * 8.0* 9.0*pow_v_7
        + p_p->c1010x * pow_u_10  * 9.0*10.0*pow_v_8
        + p_p->c1011x * pow_u_10  *10.0*11.0*pow_v_9
        + p_p->c1012x * pow_u_10  *11.0*12.0*pow_v_10
        + p_p->c1013x * pow_u_10  *12.0*13.0*pow_v_11
        + p_p->c1014x * pow_u_10  *13.0*14.0*pow_v_12
        + p_p->c1015x * pow_u_10  *14.0*15.0*pow_v_13
        + p_p->c1016x * pow_u_10  *15.0*16.0*pow_v_14
        + p_p->c1017x * pow_u_10  *16.0*17.0*pow_v_15
        + p_p->c1018x * pow_u_10  *17.0*18.0*pow_v_16
        + p_p->c1019x * pow_u_10  *18.0*19.0*pow_v_17
        + p_p->c1020x * pow_u_10  *19.0*20.0*pow_v_18
        + p_p->c1021x * pow_u_10  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1100x * pow_u_11  *            0.0          
        + p_p->c1101x * pow_u_11  *            0.0          
        + p_p->c1102x * pow_u_11  * 1.0* 2.0*pow_v_0
        + p_p->c1103x * pow_u_11  * 2.0* 3.0*pow_v_1
        + p_p->c1104x * pow_u_11  * 3.0* 4.0*pow_v_2
        + p_p->c1105x * pow_u_11  * 4.0* 5.0*pow_v_3
        + p_p->c1106x * pow_u_11  * 5.0* 6.0*pow_v_4
        + p_p->c1107x * pow_u_11  * 6.0* 7.0*pow_v_5
        + p_p->c1108x * pow_u_11  * 7.0* 8.0*pow_v_6
        + p_p->c1109x * pow_u_11  * 8.0* 9.0*pow_v_7
        + p_p->c1110x * pow_u_11  * 9.0*10.0*pow_v_8
        + p_p->c1111x * pow_u_11  *10.0*11.0*pow_v_9
        + p_p->c1112x * pow_u_11  *11.0*12.0*pow_v_10
        + p_p->c1113x * pow_u_11  *12.0*13.0*pow_v_11
        + p_p->c1114x * pow_u_11  *13.0*14.0*pow_v_12
        + p_p->c1115x * pow_u_11  *14.0*15.0*pow_v_13
        + p_p->c1116x * pow_u_11  *15.0*16.0*pow_v_14
        + p_p->c1117x * pow_u_11  *16.0*17.0*pow_v_15
        + p_p->c1118x * pow_u_11  *17.0*18.0*pow_v_16
        + p_p->c1119x * pow_u_11  *18.0*19.0*pow_v_17
        + p_p->c1120x * pow_u_11  *19.0*20.0*pow_v_18
        + p_p->c1121x * pow_u_11  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1200x * pow_u_12  *            0.0          
        + p_p->c1201x * pow_u_12  *            0.0          
        + p_p->c1202x * pow_u_12  * 1.0* 2.0*pow_v_0
        + p_p->c1203x * pow_u_12  * 2.0* 3.0*pow_v_1
        + p_p->c1204x * pow_u_12  * 3.0* 4.0*pow_v_2
        + p_p->c1205x * pow_u_12  * 4.0* 5.0*pow_v_3
        + p_p->c1206x * pow_u_12  * 5.0* 6.0*pow_v_4
        + p_p->c1207x * pow_u_12  * 6.0* 7.0*pow_v_5
        + p_p->c1208x * pow_u_12  * 7.0* 8.0*pow_v_6
        + p_p->c1209x * pow_u_12  * 8.0* 9.0*pow_v_7
        + p_p->c1210x * pow_u_12  * 9.0*10.0*pow_v_8
        + p_p->c1211x * pow_u_12  *10.0*11.0*pow_v_9
        + p_p->c1212x * pow_u_12  *11.0*12.0*pow_v_10
        + p_p->c1213x * pow_u_12  *12.0*13.0*pow_v_11
        + p_p->c1214x * pow_u_12  *13.0*14.0*pow_v_12
        + p_p->c1215x * pow_u_12  *14.0*15.0*pow_v_13
        + p_p->c1216x * pow_u_12  *15.0*16.0*pow_v_14
        + p_p->c1217x * pow_u_12  *16.0*17.0*pow_v_15
        + p_p->c1218x * pow_u_12  *17.0*18.0*pow_v_16
        + p_p->c1219x * pow_u_12  *18.0*19.0*pow_v_17
        + p_p->c1220x * pow_u_12  *19.0*20.0*pow_v_18
        + p_p->c1221x * pow_u_12  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1300x * pow_u_13  *            0.0          
        + p_p->c1301x * pow_u_13  *            0.0          
        + p_p->c1302x * pow_u_13  * 1.0* 2.0*pow_v_0
        + p_p->c1303x * pow_u_13  * 2.0* 3.0*pow_v_1
        + p_p->c1304x * pow_u_13  * 3.0* 4.0*pow_v_2
        + p_p->c1305x * pow_u_13  * 4.0* 5.0*pow_v_3
        + p_p->c1306x * pow_u_13  * 5.0* 6.0*pow_v_4
        + p_p->c1307x * pow_u_13  * 6.0* 7.0*pow_v_5
        + p_p->c1308x * pow_u_13  * 7.0* 8.0*pow_v_6
        + p_p->c1309x * pow_u_13  * 8.0* 9.0*pow_v_7
        + p_p->c1310x * pow_u_13  * 9.0*10.0*pow_v_8
        + p_p->c1311x * pow_u_13  *10.0*11.0*pow_v_9
        + p_p->c1312x * pow_u_13  *11.0*12.0*pow_v_10
        + p_p->c1313x * pow_u_13  *12.0*13.0*pow_v_11
        + p_p->c1314x * pow_u_13  *13.0*14.0*pow_v_12
        + p_p->c1315x * pow_u_13  *14.0*15.0*pow_v_13
        + p_p->c1316x * pow_u_13  *15.0*16.0*pow_v_14
        + p_p->c1317x * pow_u_13  *16.0*17.0*pow_v_15
        + p_p->c1318x * pow_u_13  *17.0*18.0*pow_v_16
        + p_p->c1319x * pow_u_13  *18.0*19.0*pow_v_17
        + p_p->c1320x * pow_u_13  *19.0*20.0*pow_v_18
        + p_p->c1321x * pow_u_13  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1400x * pow_u_14  *            0.0          
        + p_p->c1401x * pow_u_14  *            0.0          
        + p_p->c1402x * pow_u_14  * 1.0* 2.0*pow_v_0
        + p_p->c1403x * pow_u_14  * 2.0* 3.0*pow_v_1
        + p_p->c1404x * pow_u_14  * 3.0* 4.0*pow_v_2
        + p_p->c1405x * pow_u_14  * 4.0* 5.0*pow_v_3
        + p_p->c1406x * pow_u_14  * 5.0* 6.0*pow_v_4
        + p_p->c1407x * pow_u_14  * 6.0* 7.0*pow_v_5
        + p_p->c1408x * pow_u_14  * 7.0* 8.0*pow_v_6
        + p_p->c1409x * pow_u_14  * 8.0* 9.0*pow_v_7
        + p_p->c1410x * pow_u_14  * 9.0*10.0*pow_v_8
        + p_p->c1411x * pow_u_14  *10.0*11.0*pow_v_9
        + p_p->c1412x * pow_u_14  *11.0*12.0*pow_v_10
        + p_p->c1413x * pow_u_14  *12.0*13.0*pow_v_11
        + p_p->c1414x * pow_u_14  *13.0*14.0*pow_v_12
        + p_p->c1415x * pow_u_14  *14.0*15.0*pow_v_13
        + p_p->c1416x * pow_u_14  *15.0*16.0*pow_v_14
        + p_p->c1417x * pow_u_14  *16.0*17.0*pow_v_15
        + p_p->c1418x * pow_u_14  *17.0*18.0*pow_v_16
        + p_p->c1419x * pow_u_14  *18.0*19.0*pow_v_17
        + p_p->c1420x * pow_u_14  *19.0*20.0*pow_v_18
        + p_p->c1421x * pow_u_14  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1500x * pow_u_15  *            0.0          
        + p_p->c1501x * pow_u_15  *            0.0          
        + p_p->c1502x * pow_u_15  * 1.0* 2.0*pow_v_0
        + p_p->c1503x * pow_u_15  * 2.0* 3.0*pow_v_1
        + p_p->c1504x * pow_u_15  * 3.0* 4.0*pow_v_2
        + p_p->c1505x * pow_u_15  * 4.0* 5.0*pow_v_3
        + p_p->c1506x * pow_u_15  * 5.0* 6.0*pow_v_4
        + p_p->c1507x * pow_u_15  * 6.0* 7.0*pow_v_5
        + p_p->c1508x * pow_u_15  * 7.0* 8.0*pow_v_6
        + p_p->c1509x * pow_u_15  * 8.0* 9.0*pow_v_7
        + p_p->c1510x * pow_u_15  * 9.0*10.0*pow_v_8
        + p_p->c1511x * pow_u_15  *10.0*11.0*pow_v_9
        + p_p->c1512x * pow_u_15  *11.0*12.0*pow_v_10
        + p_p->c1513x * pow_u_15  *12.0*13.0*pow_v_11
        + p_p->c1514x * pow_u_15  *13.0*14.0*pow_v_12
        + p_p->c1515x * pow_u_15  *14.0*15.0*pow_v_13
        + p_p->c1516x * pow_u_15  *15.0*16.0*pow_v_14
        + p_p->c1517x * pow_u_15  *16.0*17.0*pow_v_15
        + p_p->c1518x * pow_u_15  *17.0*18.0*pow_v_16
        + p_p->c1519x * pow_u_15  *18.0*19.0*pow_v_17
        + p_p->c1520x * pow_u_15  *19.0*20.0*pow_v_18
        + p_p->c1521x * pow_u_15  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1600x * pow_u_16  *            0.0          
        + p_p->c1601x * pow_u_16  *            0.0          
        + p_p->c1602x * pow_u_16  * 1.0* 2.0*pow_v_0
        + p_p->c1603x * pow_u_16  * 2.0* 3.0*pow_v_1
        + p_p->c1604x * pow_u_16  * 3.0* 4.0*pow_v_2
        + p_p->c1605x * pow_u_16  * 4.0* 5.0*pow_v_3
        + p_p->c1606x * pow_u_16  * 5.0* 6.0*pow_v_4
        + p_p->c1607x * pow_u_16  * 6.0* 7.0*pow_v_5
        + p_p->c1608x * pow_u_16  * 7.0* 8.0*pow_v_6
        + p_p->c1609x * pow_u_16  * 8.0* 9.0*pow_v_7
        + p_p->c1610x * pow_u_16  * 9.0*10.0*pow_v_8
        + p_p->c1611x * pow_u_16  *10.0*11.0*pow_v_9
        + p_p->c1612x * pow_u_16  *11.0*12.0*pow_v_10
        + p_p->c1613x * pow_u_16  *12.0*13.0*pow_v_11
        + p_p->c1614x * pow_u_16  *13.0*14.0*pow_v_12
        + p_p->c1615x * pow_u_16  *14.0*15.0*pow_v_13
        + p_p->c1616x * pow_u_16  *15.0*16.0*pow_v_14
        + p_p->c1617x * pow_u_16  *16.0*17.0*pow_v_15
        + p_p->c1618x * pow_u_16  *17.0*18.0*pow_v_16
        + p_p->c1619x * pow_u_16  *18.0*19.0*pow_v_17
        + p_p->c1620x * pow_u_16  *19.0*20.0*pow_v_18
        + p_p->c1621x * pow_u_16  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1700x * pow_u_17  *            0.0          
        + p_p->c1701x * pow_u_17  *            0.0          
        + p_p->c1702x * pow_u_17  * 1.0* 2.0*pow_v_0
        + p_p->c1703x * pow_u_17  * 2.0* 3.0*pow_v_1
        + p_p->c1704x * pow_u_17  * 3.0* 4.0*pow_v_2
        + p_p->c1705x * pow_u_17  * 4.0* 5.0*pow_v_3
        + p_p->c1706x * pow_u_17  * 5.0* 6.0*pow_v_4
        + p_p->c1707x * pow_u_17  * 6.0* 7.0*pow_v_5
        + p_p->c1708x * pow_u_17  * 7.0* 8.0*pow_v_6
        + p_p->c1709x * pow_u_17  * 8.0* 9.0*pow_v_7
        + p_p->c1710x * pow_u_17  * 9.0*10.0*pow_v_8
        + p_p->c1711x * pow_u_17  *10.0*11.0*pow_v_9
        + p_p->c1712x * pow_u_17  *11.0*12.0*pow_v_10
        + p_p->c1713x * pow_u_17  *12.0*13.0*pow_v_11
        + p_p->c1714x * pow_u_17  *13.0*14.0*pow_v_12
        + p_p->c1715x * pow_u_17  *14.0*15.0*pow_v_13
        + p_p->c1716x * pow_u_17  *15.0*16.0*pow_v_14
        + p_p->c1717x * pow_u_17  *16.0*17.0*pow_v_15
        + p_p->c1718x * pow_u_17  *17.0*18.0*pow_v_16
        + p_p->c1719x * pow_u_17  *18.0*19.0*pow_v_17
        + p_p->c1720x * pow_u_17  *19.0*20.0*pow_v_18
        + p_p->c1721x * pow_u_17  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1800x * pow_u_18  *            0.0          
        + p_p->c1801x * pow_u_18  *            0.0          
        + p_p->c1802x * pow_u_18  * 1.0* 2.0*pow_v_0
        + p_p->c1803x * pow_u_18  * 2.0* 3.0*pow_v_1
        + p_p->c1804x * pow_u_18  * 3.0* 4.0*pow_v_2
        + p_p->c1805x * pow_u_18  * 4.0* 5.0*pow_v_3
        + p_p->c1806x * pow_u_18  * 5.0* 6.0*pow_v_4
        + p_p->c1807x * pow_u_18  * 6.0* 7.0*pow_v_5
        + p_p->c1808x * pow_u_18  * 7.0* 8.0*pow_v_6
        + p_p->c1809x * pow_u_18  * 8.0* 9.0*pow_v_7
        + p_p->c1810x * pow_u_18  * 9.0*10.0*pow_v_8
        + p_p->c1811x * pow_u_18  *10.0*11.0*pow_v_9
        + p_p->c1812x * pow_u_18  *11.0*12.0*pow_v_10
        + p_p->c1813x * pow_u_18  *12.0*13.0*pow_v_11
        + p_p->c1814x * pow_u_18  *13.0*14.0*pow_v_12
        + p_p->c1815x * pow_u_18  *14.0*15.0*pow_v_13
        + p_p->c1816x * pow_u_18  *15.0*16.0*pow_v_14
        + p_p->c1817x * pow_u_18  *16.0*17.0*pow_v_15
        + p_p->c1818x * pow_u_18  *17.0*18.0*pow_v_16
        + p_p->c1819x * pow_u_18  *18.0*19.0*pow_v_17
        + p_p->c1820x * pow_u_18  *19.0*20.0*pow_v_18
        + p_p->c1821x * pow_u_18  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c1900x * pow_u_19  *            0.0          
        + p_p->c1901x * pow_u_19  *            0.0          
        + p_p->c1902x * pow_u_19  * 1.0* 2.0*pow_v_0
        + p_p->c1903x * pow_u_19  * 2.0* 3.0*pow_v_1
        + p_p->c1904x * pow_u_19  * 3.0* 4.0*pow_v_2
        + p_p->c1905x * pow_u_19  * 4.0* 5.0*pow_v_3
        + p_p->c1906x * pow_u_19  * 5.0* 6.0*pow_v_4
        + p_p->c1907x * pow_u_19  * 6.0* 7.0*pow_v_5
        + p_p->c1908x * pow_u_19  * 7.0* 8.0*pow_v_6
        + p_p->c1909x * pow_u_19  * 8.0* 9.0*pow_v_7
        + p_p->c1910x * pow_u_19  * 9.0*10.0*pow_v_8
        + p_p->c1911x * pow_u_19  *10.0*11.0*pow_v_9
        + p_p->c1912x * pow_u_19  *11.0*12.0*pow_v_10
        + p_p->c1913x * pow_u_19  *12.0*13.0*pow_v_11
        + p_p->c1914x * pow_u_19  *13.0*14.0*pow_v_12
        + p_p->c1915x * pow_u_19  *14.0*15.0*pow_v_13
        + p_p->c1916x * pow_u_19  *15.0*16.0*pow_v_14
        + p_p->c1917x * pow_u_19  *16.0*17.0*pow_v_15
        + p_p->c1918x * pow_u_19  *17.0*18.0*pow_v_16
        + p_p->c1919x * pow_u_19  *18.0*19.0*pow_v_17
        + p_p->c1920x * pow_u_19  *19.0*20.0*pow_v_18
        + p_p->c1921x * pow_u_19  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c2000x * pow_u_20  *            0.0          
        + p_p->c2001x * pow_u_20  *            0.0          
        + p_p->c2002x * pow_u_20  * 1.0* 2.0*pow_v_0
        + p_p->c2003x * pow_u_20  * 2.0* 3.0*pow_v_1
        + p_p->c2004x * pow_u_20  * 3.0* 4.0*pow_v_2
        + p_p->c2005x * pow_u_20  * 4.0* 5.0*pow_v_3
        + p_p->c2006x * pow_u_20  * 5.0* 6.0*pow_v_4
        + p_p->c2007x * pow_u_20  * 6.0* 7.0*pow_v_5
        + p_p->c2008x * pow_u_20  * 7.0* 8.0*pow_v_6
        + p_p->c2009x * pow_u_20  * 8.0* 9.0*pow_v_7
        + p_p->c2010x * pow_u_20  * 9.0*10.0*pow_v_8
        + p_p->c2011x * pow_u_20  *10.0*11.0*pow_v_9
        + p_p->c2012x * pow_u_20  *11.0*12.0*pow_v_10
        + p_p->c2013x * pow_u_20  *12.0*13.0*pow_v_11
        + p_p->c2014x * pow_u_20  *13.0*14.0*pow_v_12
        + p_p->c2015x * pow_u_20  *14.0*15.0*pow_v_13
        + p_p->c2016x * pow_u_20  *15.0*16.0*pow_v_14
        + p_p->c2017x * pow_u_20  *16.0*17.0*pow_v_15
        + p_p->c2018x * pow_u_20  *17.0*18.0*pow_v_16
        + p_p->c2019x * pow_u_20  *18.0*19.0*pow_v_17
        + p_p->c2020x * pow_u_20  *19.0*20.0*pow_v_18
        + p_p->c2021x * pow_u_20  *20.0*21.0*pow_v_19;
v2_x=v2_x + p_p->c2100x * pow_u_21  *            0.0          
        + p_p->c2101x * pow_u_21  *            0.0          
        + p_p->c2102x * pow_u_21  * 1.0* 2.0*pow_v_0
        + p_p->c2103x * pow_u_21  * 2.0* 3.0*pow_v_1
        + p_p->c2104x * pow_u_21  * 3.0* 4.0*pow_v_2
        + p_p->c2105x * pow_u_21  * 4.0* 5.0*pow_v_3
        + p_p->c2106x * pow_u_21  * 5.0* 6.0*pow_v_4
        + p_p->c2107x * pow_u_21  * 6.0* 7.0*pow_v_5
        + p_p->c2108x * pow_u_21  * 7.0* 8.0*pow_v_6
        + p_p->c2109x * pow_u_21  * 8.0* 9.0*pow_v_7
        + p_p->c2110x * pow_u_21  * 9.0*10.0*pow_v_8
        + p_p->c2111x * pow_u_21  *10.0*11.0*pow_v_9
        + p_p->c2112x * pow_u_21  *11.0*12.0*pow_v_10
        + p_p->c2113x * pow_u_21  *12.0*13.0*pow_v_11
        + p_p->c2114x * pow_u_21  *13.0*14.0*pow_v_12
        + p_p->c2115x * pow_u_21  *14.0*15.0*pow_v_13
        + p_p->c2116x * pow_u_21  *15.0*16.0*pow_v_14
        + p_p->c2117x * pow_u_21  *16.0*17.0*pow_v_15
        + p_p->c2118x * pow_u_21  *17.0*18.0*pow_v_16
        + p_p->c2119x * pow_u_21  *18.0*19.0*pow_v_17
        + p_p->c2120x * pow_u_21  *19.0*20.0*pow_v_18
        + p_p->c2121x * pow_u_21  *20.0*21.0*pow_v_19;

v2_y    = p_p->c0000y * pow_u_0  *            0.0          
        + p_p->c0001y * pow_u_0  *            0.0          
        + p_p->c0002y * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003y * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004y * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005y * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006y * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007y * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0008y * pow_u_0  * 7.0* 8.0*pow_v_6
        + p_p->c0009y * pow_u_0  * 8.0* 9.0*pow_v_7
        + p_p->c0010y * pow_u_0  * 9.0*10.0*pow_v_8
        + p_p->c0011y * pow_u_0  *10.0*11.0*pow_v_9
        + p_p->c0012y * pow_u_0  *11.0*12.0*pow_v_10
        + p_p->c0013y * pow_u_0  *12.0*13.0*pow_v_11
        + p_p->c0014y * pow_u_0  *13.0*14.0*pow_v_12
        + p_p->c0015y * pow_u_0  *14.0*15.0*pow_v_13
        + p_p->c0016y * pow_u_0  *15.0*16.0*pow_v_14
        + p_p->c0017y * pow_u_0  *16.0*17.0*pow_v_15
        + p_p->c0018y * pow_u_0  *17.0*18.0*pow_v_16
        + p_p->c0019y * pow_u_0  *18.0*19.0*pow_v_17
        + p_p->c0020y * pow_u_0  *19.0*20.0*pow_v_18
        + p_p->c0021y * pow_u_0  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0100y * pow_u_1  *            0.0          
        + p_p->c0101y * pow_u_1  *            0.0          
        + p_p->c0102y * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103y * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104y * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105y * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106y * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107y * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0108y * pow_u_1  * 7.0* 8.0*pow_v_6
        + p_p->c0109y * pow_u_1  * 8.0* 9.0*pow_v_7
        + p_p->c0110y * pow_u_1  * 9.0*10.0*pow_v_8
        + p_p->c0111y * pow_u_1  *10.0*11.0*pow_v_9
        + p_p->c0112y * pow_u_1  *11.0*12.0*pow_v_10
        + p_p->c0113y * pow_u_1  *12.0*13.0*pow_v_11
        + p_p->c0114y * pow_u_1  *13.0*14.0*pow_v_12
        + p_p->c0115y * pow_u_1  *14.0*15.0*pow_v_13
        + p_p->c0116y * pow_u_1  *15.0*16.0*pow_v_14
        + p_p->c0117y * pow_u_1  *16.0*17.0*pow_v_15
        + p_p->c0118y * pow_u_1  *17.0*18.0*pow_v_16
        + p_p->c0119y * pow_u_1  *18.0*19.0*pow_v_17
        + p_p->c0120y * pow_u_1  *19.0*20.0*pow_v_18
        + p_p->c0121y * pow_u_1  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0200y * pow_u_2  *            0.0          
        + p_p->c0201y * pow_u_2  *            0.0          
        + p_p->c0202y * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203y * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204y * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205y * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206y * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207y * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0208y * pow_u_2  * 7.0* 8.0*pow_v_6
        + p_p->c0209y * pow_u_2  * 8.0* 9.0*pow_v_7
        + p_p->c0210y * pow_u_2  * 9.0*10.0*pow_v_8
        + p_p->c0211y * pow_u_2  *10.0*11.0*pow_v_9
        + p_p->c0212y * pow_u_2  *11.0*12.0*pow_v_10
        + p_p->c0213y * pow_u_2  *12.0*13.0*pow_v_11
        + p_p->c0214y * pow_u_2  *13.0*14.0*pow_v_12
        + p_p->c0215y * pow_u_2  *14.0*15.0*pow_v_13
        + p_p->c0216y * pow_u_2  *15.0*16.0*pow_v_14
        + p_p->c0217y * pow_u_2  *16.0*17.0*pow_v_15
        + p_p->c0218y * pow_u_2  *17.0*18.0*pow_v_16
        + p_p->c0219y * pow_u_2  *18.0*19.0*pow_v_17
        + p_p->c0220y * pow_u_2  *19.0*20.0*pow_v_18
        + p_p->c0221y * pow_u_2  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0300y * pow_u_3  *            0.0          
        + p_p->c0301y * pow_u_3  *            0.0          
        + p_p->c0302y * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303y * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304y * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305y * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306y * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307y * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0308y * pow_u_3  * 7.0* 8.0*pow_v_6
        + p_p->c0309y * pow_u_3  * 8.0* 9.0*pow_v_7
        + p_p->c0310y * pow_u_3  * 9.0*10.0*pow_v_8
        + p_p->c0311y * pow_u_3  *10.0*11.0*pow_v_9
        + p_p->c0312y * pow_u_3  *11.0*12.0*pow_v_10
        + p_p->c0313y * pow_u_3  *12.0*13.0*pow_v_11
        + p_p->c0314y * pow_u_3  *13.0*14.0*pow_v_12
        + p_p->c0315y * pow_u_3  *14.0*15.0*pow_v_13
        + p_p->c0316y * pow_u_3  *15.0*16.0*pow_v_14
        + p_p->c0317y * pow_u_3  *16.0*17.0*pow_v_15
        + p_p->c0318y * pow_u_3  *17.0*18.0*pow_v_16
        + p_p->c0319y * pow_u_3  *18.0*19.0*pow_v_17
        + p_p->c0320y * pow_u_3  *19.0*20.0*pow_v_18
        + p_p->c0321y * pow_u_3  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0400y * pow_u_4  *            0.0          
        + p_p->c0401y * pow_u_4  *            0.0          
        + p_p->c0402y * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403y * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404y * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405y * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406y * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407y * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0408y * pow_u_4  * 7.0* 8.0*pow_v_6
        + p_p->c0409y * pow_u_4  * 8.0* 9.0*pow_v_7
        + p_p->c0410y * pow_u_4  * 9.0*10.0*pow_v_8
        + p_p->c0411y * pow_u_4  *10.0*11.0*pow_v_9
        + p_p->c0412y * pow_u_4  *11.0*12.0*pow_v_10
        + p_p->c0413y * pow_u_4  *12.0*13.0*pow_v_11
        + p_p->c0414y * pow_u_4  *13.0*14.0*pow_v_12
        + p_p->c0415y * pow_u_4  *14.0*15.0*pow_v_13
        + p_p->c0416y * pow_u_4  *15.0*16.0*pow_v_14
        + p_p->c0417y * pow_u_4  *16.0*17.0*pow_v_15
        + p_p->c0418y * pow_u_4  *17.0*18.0*pow_v_16
        + p_p->c0419y * pow_u_4  *18.0*19.0*pow_v_17
        + p_p->c0420y * pow_u_4  *19.0*20.0*pow_v_18
        + p_p->c0421y * pow_u_4  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0500y * pow_u_5  *            0.0          
        + p_p->c0501y * pow_u_5  *            0.0          
        + p_p->c0502y * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503y * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504y * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505y * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506y * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507y * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0508y * pow_u_5  * 7.0* 8.0*pow_v_6
        + p_p->c0509y * pow_u_5  * 8.0* 9.0*pow_v_7
        + p_p->c0510y * pow_u_5  * 9.0*10.0*pow_v_8
        + p_p->c0511y * pow_u_5  *10.0*11.0*pow_v_9
        + p_p->c0512y * pow_u_5  *11.0*12.0*pow_v_10
        + p_p->c0513y * pow_u_5  *12.0*13.0*pow_v_11
        + p_p->c0514y * pow_u_5  *13.0*14.0*pow_v_12
        + p_p->c0515y * pow_u_5  *14.0*15.0*pow_v_13
        + p_p->c0516y * pow_u_5  *15.0*16.0*pow_v_14
        + p_p->c0517y * pow_u_5  *16.0*17.0*pow_v_15
        + p_p->c0518y * pow_u_5  *17.0*18.0*pow_v_16
        + p_p->c0519y * pow_u_5  *18.0*19.0*pow_v_17
        + p_p->c0520y * pow_u_5  *19.0*20.0*pow_v_18
        + p_p->c0521y * pow_u_5  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0600y * pow_u_6  *            0.0          
        + p_p->c0601y * pow_u_6  *            0.0          
        + p_p->c0602y * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603y * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604y * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605y * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606y * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607y * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0608y * pow_u_6  * 7.0* 8.0*pow_v_6
        + p_p->c0609y * pow_u_6  * 8.0* 9.0*pow_v_7
        + p_p->c0610y * pow_u_6  * 9.0*10.0*pow_v_8
        + p_p->c0611y * pow_u_6  *10.0*11.0*pow_v_9
        + p_p->c0612y * pow_u_6  *11.0*12.0*pow_v_10
        + p_p->c0613y * pow_u_6  *12.0*13.0*pow_v_11
        + p_p->c0614y * pow_u_6  *13.0*14.0*pow_v_12
        + p_p->c0615y * pow_u_6  *14.0*15.0*pow_v_13
        + p_p->c0616y * pow_u_6  *15.0*16.0*pow_v_14
        + p_p->c0617y * pow_u_6  *16.0*17.0*pow_v_15
        + p_p->c0618y * pow_u_6  *17.0*18.0*pow_v_16
        + p_p->c0619y * pow_u_6  *18.0*19.0*pow_v_17
        + p_p->c0620y * pow_u_6  *19.0*20.0*pow_v_18
        + p_p->c0621y * pow_u_6  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0700y * pow_u_7  *            0.0          
        + p_p->c0701y * pow_u_7  *            0.0          
        + p_p->c0702y * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703y * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704y * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705y * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706y * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707y * pow_u_7  * 6.0* 7.0*pow_v_5
        + p_p->c0708y * pow_u_7  * 7.0* 8.0*pow_v_6
        + p_p->c0709y * pow_u_7  * 8.0* 9.0*pow_v_7
        + p_p->c0710y * pow_u_7  * 9.0*10.0*pow_v_8
        + p_p->c0711y * pow_u_7  *10.0*11.0*pow_v_9
        + p_p->c0712y * pow_u_7  *11.0*12.0*pow_v_10
        + p_p->c0713y * pow_u_7  *12.0*13.0*pow_v_11
        + p_p->c0714y * pow_u_7  *13.0*14.0*pow_v_12
        + p_p->c0715y * pow_u_7  *14.0*15.0*pow_v_13
        + p_p->c0716y * pow_u_7  *15.0*16.0*pow_v_14
        + p_p->c0717y * pow_u_7  *16.0*17.0*pow_v_15
        + p_p->c0718y * pow_u_7  *17.0*18.0*pow_v_16
        + p_p->c0719y * pow_u_7  *18.0*19.0*pow_v_17
        + p_p->c0720y * pow_u_7  *19.0*20.0*pow_v_18
        + p_p->c0721y * pow_u_7  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0800y * pow_u_8  *            0.0          
        + p_p->c0801y * pow_u_8  *            0.0          
        + p_p->c0802y * pow_u_8  * 1.0* 2.0*pow_v_0
        + p_p->c0803y * pow_u_8  * 2.0* 3.0*pow_v_1
        + p_p->c0804y * pow_u_8  * 3.0* 4.0*pow_v_2
        + p_p->c0805y * pow_u_8  * 4.0* 5.0*pow_v_3
        + p_p->c0806y * pow_u_8  * 5.0* 6.0*pow_v_4
        + p_p->c0807y * pow_u_8  * 6.0* 7.0*pow_v_5
        + p_p->c0808y * pow_u_8  * 7.0* 8.0*pow_v_6
        + p_p->c0809y * pow_u_8  * 8.0* 9.0*pow_v_7
        + p_p->c0810y * pow_u_8  * 9.0*10.0*pow_v_8
        + p_p->c0811y * pow_u_8  *10.0*11.0*pow_v_9
        + p_p->c0812y * pow_u_8  *11.0*12.0*pow_v_10
        + p_p->c0813y * pow_u_8  *12.0*13.0*pow_v_11
        + p_p->c0814y * pow_u_8  *13.0*14.0*pow_v_12
        + p_p->c0815y * pow_u_8  *14.0*15.0*pow_v_13
        + p_p->c0816y * pow_u_8  *15.0*16.0*pow_v_14
        + p_p->c0817y * pow_u_8  *16.0*17.0*pow_v_15
        + p_p->c0818y * pow_u_8  *17.0*18.0*pow_v_16
        + p_p->c0819y * pow_u_8  *18.0*19.0*pow_v_17
        + p_p->c0820y * pow_u_8  *19.0*20.0*pow_v_18
        + p_p->c0821y * pow_u_8  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c0900y * pow_u_9  *            0.0          
        + p_p->c0901y * pow_u_9  *            0.0          
        + p_p->c0902y * pow_u_9  * 1.0* 2.0*pow_v_0
        + p_p->c0903y * pow_u_9  * 2.0* 3.0*pow_v_1
        + p_p->c0904y * pow_u_9  * 3.0* 4.0*pow_v_2
        + p_p->c0905y * pow_u_9  * 4.0* 5.0*pow_v_3
        + p_p->c0906y * pow_u_9  * 5.0* 6.0*pow_v_4
        + p_p->c0907y * pow_u_9  * 6.0* 7.0*pow_v_5
        + p_p->c0908y * pow_u_9  * 7.0* 8.0*pow_v_6
        + p_p->c0909y * pow_u_9  * 8.0* 9.0*pow_v_7
        + p_p->c0910y * pow_u_9  * 9.0*10.0*pow_v_8
        + p_p->c0911y * pow_u_9  *10.0*11.0*pow_v_9
        + p_p->c0912y * pow_u_9  *11.0*12.0*pow_v_10
        + p_p->c0913y * pow_u_9  *12.0*13.0*pow_v_11
        + p_p->c0914y * pow_u_9  *13.0*14.0*pow_v_12
        + p_p->c0915y * pow_u_9  *14.0*15.0*pow_v_13
        + p_p->c0916y * pow_u_9  *15.0*16.0*pow_v_14
        + p_p->c0917y * pow_u_9  *16.0*17.0*pow_v_15
        + p_p->c0918y * pow_u_9  *17.0*18.0*pow_v_16
        + p_p->c0919y * pow_u_9  *18.0*19.0*pow_v_17
        + p_p->c0920y * pow_u_9  *19.0*20.0*pow_v_18
        + p_p->c0921y * pow_u_9  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1000y * pow_u_10  *            0.0          
        + p_p->c1001y * pow_u_10  *            0.0          
        + p_p->c1002y * pow_u_10  * 1.0* 2.0*pow_v_0
        + p_p->c1003y * pow_u_10  * 2.0* 3.0*pow_v_1
        + p_p->c1004y * pow_u_10  * 3.0* 4.0*pow_v_2
        + p_p->c1005y * pow_u_10  * 4.0* 5.0*pow_v_3
        + p_p->c1006y * pow_u_10  * 5.0* 6.0*pow_v_4
        + p_p->c1007y * pow_u_10  * 6.0* 7.0*pow_v_5
        + p_p->c1008y * pow_u_10  * 7.0* 8.0*pow_v_6
        + p_p->c1009y * pow_u_10  * 8.0* 9.0*pow_v_7
        + p_p->c1010y * pow_u_10  * 9.0*10.0*pow_v_8
        + p_p->c1011y * pow_u_10  *10.0*11.0*pow_v_9
        + p_p->c1012y * pow_u_10  *11.0*12.0*pow_v_10
        + p_p->c1013y * pow_u_10  *12.0*13.0*pow_v_11
        + p_p->c1014y * pow_u_10  *13.0*14.0*pow_v_12
        + p_p->c1015y * pow_u_10  *14.0*15.0*pow_v_13
        + p_p->c1016y * pow_u_10  *15.0*16.0*pow_v_14
        + p_p->c1017y * pow_u_10  *16.0*17.0*pow_v_15
        + p_p->c1018y * pow_u_10  *17.0*18.0*pow_v_16
        + p_p->c1019y * pow_u_10  *18.0*19.0*pow_v_17
        + p_p->c1020y * pow_u_10  *19.0*20.0*pow_v_18
        + p_p->c1021y * pow_u_10  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1100y * pow_u_11  *            0.0          
        + p_p->c1101y * pow_u_11  *            0.0          
        + p_p->c1102y * pow_u_11  * 1.0* 2.0*pow_v_0
        + p_p->c1103y * pow_u_11  * 2.0* 3.0*pow_v_1
        + p_p->c1104y * pow_u_11  * 3.0* 4.0*pow_v_2
        + p_p->c1105y * pow_u_11  * 4.0* 5.0*pow_v_3
        + p_p->c1106y * pow_u_11  * 5.0* 6.0*pow_v_4
        + p_p->c1107y * pow_u_11  * 6.0* 7.0*pow_v_5
        + p_p->c1108y * pow_u_11  * 7.0* 8.0*pow_v_6
        + p_p->c1109y * pow_u_11  * 8.0* 9.0*pow_v_7
        + p_p->c1110y * pow_u_11  * 9.0*10.0*pow_v_8
        + p_p->c1111y * pow_u_11  *10.0*11.0*pow_v_9
        + p_p->c1112y * pow_u_11  *11.0*12.0*pow_v_10
        + p_p->c1113y * pow_u_11  *12.0*13.0*pow_v_11
        + p_p->c1114y * pow_u_11  *13.0*14.0*pow_v_12
        + p_p->c1115y * pow_u_11  *14.0*15.0*pow_v_13
        + p_p->c1116y * pow_u_11  *15.0*16.0*pow_v_14
        + p_p->c1117y * pow_u_11  *16.0*17.0*pow_v_15
        + p_p->c1118y * pow_u_11  *17.0*18.0*pow_v_16
        + p_p->c1119y * pow_u_11  *18.0*19.0*pow_v_17
        + p_p->c1120y * pow_u_11  *19.0*20.0*pow_v_18
        + p_p->c1121y * pow_u_11  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1200y * pow_u_12  *            0.0          
        + p_p->c1201y * pow_u_12  *            0.0          
        + p_p->c1202y * pow_u_12  * 1.0* 2.0*pow_v_0
        + p_p->c1203y * pow_u_12  * 2.0* 3.0*pow_v_1
        + p_p->c1204y * pow_u_12  * 3.0* 4.0*pow_v_2
        + p_p->c1205y * pow_u_12  * 4.0* 5.0*pow_v_3
        + p_p->c1206y * pow_u_12  * 5.0* 6.0*pow_v_4
        + p_p->c1207y * pow_u_12  * 6.0* 7.0*pow_v_5
        + p_p->c1208y * pow_u_12  * 7.0* 8.0*pow_v_6
        + p_p->c1209y * pow_u_12  * 8.0* 9.0*pow_v_7
        + p_p->c1210y * pow_u_12  * 9.0*10.0*pow_v_8
        + p_p->c1211y * pow_u_12  *10.0*11.0*pow_v_9
        + p_p->c1212y * pow_u_12  *11.0*12.0*pow_v_10
        + p_p->c1213y * pow_u_12  *12.0*13.0*pow_v_11
        + p_p->c1214y * pow_u_12  *13.0*14.0*pow_v_12
        + p_p->c1215y * pow_u_12  *14.0*15.0*pow_v_13
        + p_p->c1216y * pow_u_12  *15.0*16.0*pow_v_14
        + p_p->c1217y * pow_u_12  *16.0*17.0*pow_v_15
        + p_p->c1218y * pow_u_12  *17.0*18.0*pow_v_16
        + p_p->c1219y * pow_u_12  *18.0*19.0*pow_v_17
        + p_p->c1220y * pow_u_12  *19.0*20.0*pow_v_18
        + p_p->c1221y * pow_u_12  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1300y * pow_u_13  *            0.0          
        + p_p->c1301y * pow_u_13  *            0.0          
        + p_p->c1302y * pow_u_13  * 1.0* 2.0*pow_v_0
        + p_p->c1303y * pow_u_13  * 2.0* 3.0*pow_v_1
        + p_p->c1304y * pow_u_13  * 3.0* 4.0*pow_v_2
        + p_p->c1305y * pow_u_13  * 4.0* 5.0*pow_v_3
        + p_p->c1306y * pow_u_13  * 5.0* 6.0*pow_v_4
        + p_p->c1307y * pow_u_13  * 6.0* 7.0*pow_v_5
        + p_p->c1308y * pow_u_13  * 7.0* 8.0*pow_v_6
        + p_p->c1309y * pow_u_13  * 8.0* 9.0*pow_v_7
        + p_p->c1310y * pow_u_13  * 9.0*10.0*pow_v_8
        + p_p->c1311y * pow_u_13  *10.0*11.0*pow_v_9
        + p_p->c1312y * pow_u_13  *11.0*12.0*pow_v_10
        + p_p->c1313y * pow_u_13  *12.0*13.0*pow_v_11
        + p_p->c1314y * pow_u_13  *13.0*14.0*pow_v_12
        + p_p->c1315y * pow_u_13  *14.0*15.0*pow_v_13
        + p_p->c1316y * pow_u_13  *15.0*16.0*pow_v_14
        + p_p->c1317y * pow_u_13  *16.0*17.0*pow_v_15
        + p_p->c1318y * pow_u_13  *17.0*18.0*pow_v_16
        + p_p->c1319y * pow_u_13  *18.0*19.0*pow_v_17
        + p_p->c1320y * pow_u_13  *19.0*20.0*pow_v_18
        + p_p->c1321y * pow_u_13  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1400y * pow_u_14  *            0.0          
        + p_p->c1401y * pow_u_14  *            0.0          
        + p_p->c1402y * pow_u_14  * 1.0* 2.0*pow_v_0
        + p_p->c1403y * pow_u_14  * 2.0* 3.0*pow_v_1
        + p_p->c1404y * pow_u_14  * 3.0* 4.0*pow_v_2
        + p_p->c1405y * pow_u_14  * 4.0* 5.0*pow_v_3
        + p_p->c1406y * pow_u_14  * 5.0* 6.0*pow_v_4
        + p_p->c1407y * pow_u_14  * 6.0* 7.0*pow_v_5
        + p_p->c1408y * pow_u_14  * 7.0* 8.0*pow_v_6
        + p_p->c1409y * pow_u_14  * 8.0* 9.0*pow_v_7
        + p_p->c1410y * pow_u_14  * 9.0*10.0*pow_v_8
        + p_p->c1411y * pow_u_14  *10.0*11.0*pow_v_9
        + p_p->c1412y * pow_u_14  *11.0*12.0*pow_v_10
        + p_p->c1413y * pow_u_14  *12.0*13.0*pow_v_11
        + p_p->c1414y * pow_u_14  *13.0*14.0*pow_v_12
        + p_p->c1415y * pow_u_14  *14.0*15.0*pow_v_13
        + p_p->c1416y * pow_u_14  *15.0*16.0*pow_v_14
        + p_p->c1417y * pow_u_14  *16.0*17.0*pow_v_15
        + p_p->c1418y * pow_u_14  *17.0*18.0*pow_v_16
        + p_p->c1419y * pow_u_14  *18.0*19.0*pow_v_17
        + p_p->c1420y * pow_u_14  *19.0*20.0*pow_v_18
        + p_p->c1421y * pow_u_14  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1500y * pow_u_15  *            0.0          
        + p_p->c1501y * pow_u_15  *            0.0          
        + p_p->c1502y * pow_u_15  * 1.0* 2.0*pow_v_0
        + p_p->c1503y * pow_u_15  * 2.0* 3.0*pow_v_1
        + p_p->c1504y * pow_u_15  * 3.0* 4.0*pow_v_2
        + p_p->c1505y * pow_u_15  * 4.0* 5.0*pow_v_3
        + p_p->c1506y * pow_u_15  * 5.0* 6.0*pow_v_4
        + p_p->c1507y * pow_u_15  * 6.0* 7.0*pow_v_5
        + p_p->c1508y * pow_u_15  * 7.0* 8.0*pow_v_6
        + p_p->c1509y * pow_u_15  * 8.0* 9.0*pow_v_7
        + p_p->c1510y * pow_u_15  * 9.0*10.0*pow_v_8
        + p_p->c1511y * pow_u_15  *10.0*11.0*pow_v_9
        + p_p->c1512y * pow_u_15  *11.0*12.0*pow_v_10
        + p_p->c1513y * pow_u_15  *12.0*13.0*pow_v_11
        + p_p->c1514y * pow_u_15  *13.0*14.0*pow_v_12
        + p_p->c1515y * pow_u_15  *14.0*15.0*pow_v_13
        + p_p->c1516y * pow_u_15  *15.0*16.0*pow_v_14
        + p_p->c1517y * pow_u_15  *16.0*17.0*pow_v_15
        + p_p->c1518y * pow_u_15  *17.0*18.0*pow_v_16
        + p_p->c1519y * pow_u_15  *18.0*19.0*pow_v_17
        + p_p->c1520y * pow_u_15  *19.0*20.0*pow_v_18
        + p_p->c1521y * pow_u_15  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1600y * pow_u_16  *            0.0          
        + p_p->c1601y * pow_u_16  *            0.0          
        + p_p->c1602y * pow_u_16  * 1.0* 2.0*pow_v_0
        + p_p->c1603y * pow_u_16  * 2.0* 3.0*pow_v_1
        + p_p->c1604y * pow_u_16  * 3.0* 4.0*pow_v_2
        + p_p->c1605y * pow_u_16  * 4.0* 5.0*pow_v_3
        + p_p->c1606y * pow_u_16  * 5.0* 6.0*pow_v_4
        + p_p->c1607y * pow_u_16  * 6.0* 7.0*pow_v_5
        + p_p->c1608y * pow_u_16  * 7.0* 8.0*pow_v_6
        + p_p->c1609y * pow_u_16  * 8.0* 9.0*pow_v_7
        + p_p->c1610y * pow_u_16  * 9.0*10.0*pow_v_8
        + p_p->c1611y * pow_u_16  *10.0*11.0*pow_v_9
        + p_p->c1612y * pow_u_16  *11.0*12.0*pow_v_10
        + p_p->c1613y * pow_u_16  *12.0*13.0*pow_v_11
        + p_p->c1614y * pow_u_16  *13.0*14.0*pow_v_12
        + p_p->c1615y * pow_u_16  *14.0*15.0*pow_v_13
        + p_p->c1616y * pow_u_16  *15.0*16.0*pow_v_14
        + p_p->c1617y * pow_u_16  *16.0*17.0*pow_v_15
        + p_p->c1618y * pow_u_16  *17.0*18.0*pow_v_16
        + p_p->c1619y * pow_u_16  *18.0*19.0*pow_v_17
        + p_p->c1620y * pow_u_16  *19.0*20.0*pow_v_18
        + p_p->c1621y * pow_u_16  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1700y * pow_u_17  *            0.0          
        + p_p->c1701y * pow_u_17  *            0.0          
        + p_p->c1702y * pow_u_17  * 1.0* 2.0*pow_v_0
        + p_p->c1703y * pow_u_17  * 2.0* 3.0*pow_v_1
        + p_p->c1704y * pow_u_17  * 3.0* 4.0*pow_v_2
        + p_p->c1705y * pow_u_17  * 4.0* 5.0*pow_v_3
        + p_p->c1706y * pow_u_17  * 5.0* 6.0*pow_v_4
        + p_p->c1707y * pow_u_17  * 6.0* 7.0*pow_v_5
        + p_p->c1708y * pow_u_17  * 7.0* 8.0*pow_v_6
        + p_p->c1709y * pow_u_17  * 8.0* 9.0*pow_v_7
        + p_p->c1710y * pow_u_17  * 9.0*10.0*pow_v_8
        + p_p->c1711y * pow_u_17  *10.0*11.0*pow_v_9
        + p_p->c1712y * pow_u_17  *11.0*12.0*pow_v_10
        + p_p->c1713y * pow_u_17  *12.0*13.0*pow_v_11
        + p_p->c1714y * pow_u_17  *13.0*14.0*pow_v_12
        + p_p->c1715y * pow_u_17  *14.0*15.0*pow_v_13
        + p_p->c1716y * pow_u_17  *15.0*16.0*pow_v_14
        + p_p->c1717y * pow_u_17  *16.0*17.0*pow_v_15
        + p_p->c1718y * pow_u_17  *17.0*18.0*pow_v_16
        + p_p->c1719y * pow_u_17  *18.0*19.0*pow_v_17
        + p_p->c1720y * pow_u_17  *19.0*20.0*pow_v_18
        + p_p->c1721y * pow_u_17  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1800y * pow_u_18  *            0.0          
        + p_p->c1801y * pow_u_18  *            0.0          
        + p_p->c1802y * pow_u_18  * 1.0* 2.0*pow_v_0
        + p_p->c1803y * pow_u_18  * 2.0* 3.0*pow_v_1
        + p_p->c1804y * pow_u_18  * 3.0* 4.0*pow_v_2
        + p_p->c1805y * pow_u_18  * 4.0* 5.0*pow_v_3
        + p_p->c1806y * pow_u_18  * 5.0* 6.0*pow_v_4
        + p_p->c1807y * pow_u_18  * 6.0* 7.0*pow_v_5
        + p_p->c1808y * pow_u_18  * 7.0* 8.0*pow_v_6
        + p_p->c1809y * pow_u_18  * 8.0* 9.0*pow_v_7
        + p_p->c1810y * pow_u_18  * 9.0*10.0*pow_v_8
        + p_p->c1811y * pow_u_18  *10.0*11.0*pow_v_9
        + p_p->c1812y * pow_u_18  *11.0*12.0*pow_v_10
        + p_p->c1813y * pow_u_18  *12.0*13.0*pow_v_11
        + p_p->c1814y * pow_u_18  *13.0*14.0*pow_v_12
        + p_p->c1815y * pow_u_18  *14.0*15.0*pow_v_13
        + p_p->c1816y * pow_u_18  *15.0*16.0*pow_v_14
        + p_p->c1817y * pow_u_18  *16.0*17.0*pow_v_15
        + p_p->c1818y * pow_u_18  *17.0*18.0*pow_v_16
        + p_p->c1819y * pow_u_18  *18.0*19.0*pow_v_17
        + p_p->c1820y * pow_u_18  *19.0*20.0*pow_v_18
        + p_p->c1821y * pow_u_18  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c1900y * pow_u_19  *            0.0          
        + p_p->c1901y * pow_u_19  *            0.0          
        + p_p->c1902y * pow_u_19  * 1.0* 2.0*pow_v_0
        + p_p->c1903y * pow_u_19  * 2.0* 3.0*pow_v_1
        + p_p->c1904y * pow_u_19  * 3.0* 4.0*pow_v_2
        + p_p->c1905y * pow_u_19  * 4.0* 5.0*pow_v_3
        + p_p->c1906y * pow_u_19  * 5.0* 6.0*pow_v_4
        + p_p->c1907y * pow_u_19  * 6.0* 7.0*pow_v_5
        + p_p->c1908y * pow_u_19  * 7.0* 8.0*pow_v_6
        + p_p->c1909y * pow_u_19  * 8.0* 9.0*pow_v_7
        + p_p->c1910y * pow_u_19  * 9.0*10.0*pow_v_8
        + p_p->c1911y * pow_u_19  *10.0*11.0*pow_v_9
        + p_p->c1912y * pow_u_19  *11.0*12.0*pow_v_10
        + p_p->c1913y * pow_u_19  *12.0*13.0*pow_v_11
        + p_p->c1914y * pow_u_19  *13.0*14.0*pow_v_12
        + p_p->c1915y * pow_u_19  *14.0*15.0*pow_v_13
        + p_p->c1916y * pow_u_19  *15.0*16.0*pow_v_14
        + p_p->c1917y * pow_u_19  *16.0*17.0*pow_v_15
        + p_p->c1918y * pow_u_19  *17.0*18.0*pow_v_16
        + p_p->c1919y * pow_u_19  *18.0*19.0*pow_v_17
        + p_p->c1920y * pow_u_19  *19.0*20.0*pow_v_18
        + p_p->c1921y * pow_u_19  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c2000y * pow_u_20  *            0.0          
        + p_p->c2001y * pow_u_20  *            0.0          
        + p_p->c2002y * pow_u_20  * 1.0* 2.0*pow_v_0
        + p_p->c2003y * pow_u_20  * 2.0* 3.0*pow_v_1
        + p_p->c2004y * pow_u_20  * 3.0* 4.0*pow_v_2
        + p_p->c2005y * pow_u_20  * 4.0* 5.0*pow_v_3
        + p_p->c2006y * pow_u_20  * 5.0* 6.0*pow_v_4
        + p_p->c2007y * pow_u_20  * 6.0* 7.0*pow_v_5
        + p_p->c2008y * pow_u_20  * 7.0* 8.0*pow_v_6
        + p_p->c2009y * pow_u_20  * 8.0* 9.0*pow_v_7
        + p_p->c2010y * pow_u_20  * 9.0*10.0*pow_v_8
        + p_p->c2011y * pow_u_20  *10.0*11.0*pow_v_9
        + p_p->c2012y * pow_u_20  *11.0*12.0*pow_v_10
        + p_p->c2013y * pow_u_20  *12.0*13.0*pow_v_11
        + p_p->c2014y * pow_u_20  *13.0*14.0*pow_v_12
        + p_p->c2015y * pow_u_20  *14.0*15.0*pow_v_13
        + p_p->c2016y * pow_u_20  *15.0*16.0*pow_v_14
        + p_p->c2017y * pow_u_20  *16.0*17.0*pow_v_15
        + p_p->c2018y * pow_u_20  *17.0*18.0*pow_v_16
        + p_p->c2019y * pow_u_20  *18.0*19.0*pow_v_17
        + p_p->c2020y * pow_u_20  *19.0*20.0*pow_v_18
        + p_p->c2021y * pow_u_20  *20.0*21.0*pow_v_19;
v2_y=v2_y + p_p->c2100y * pow_u_21  *            0.0          
        + p_p->c2101y * pow_u_21  *            0.0          
        + p_p->c2102y * pow_u_21  * 1.0* 2.0*pow_v_0
        + p_p->c2103y * pow_u_21  * 2.0* 3.0*pow_v_1
        + p_p->c2104y * pow_u_21  * 3.0* 4.0*pow_v_2
        + p_p->c2105y * pow_u_21  * 4.0* 5.0*pow_v_3
        + p_p->c2106y * pow_u_21  * 5.0* 6.0*pow_v_4
        + p_p->c2107y * pow_u_21  * 6.0* 7.0*pow_v_5
        + p_p->c2108y * pow_u_21  * 7.0* 8.0*pow_v_6
        + p_p->c2109y * pow_u_21  * 8.0* 9.0*pow_v_7
        + p_p->c2110y * pow_u_21  * 9.0*10.0*pow_v_8
        + p_p->c2111y * pow_u_21  *10.0*11.0*pow_v_9
        + p_p->c2112y * pow_u_21  *11.0*12.0*pow_v_10
        + p_p->c2113y * pow_u_21  *12.0*13.0*pow_v_11
        + p_p->c2114y * pow_u_21  *13.0*14.0*pow_v_12
        + p_p->c2115y * pow_u_21  *14.0*15.0*pow_v_13
        + p_p->c2116y * pow_u_21  *15.0*16.0*pow_v_14
        + p_p->c2117y * pow_u_21  *16.0*17.0*pow_v_15
        + p_p->c2118y * pow_u_21  *17.0*18.0*pow_v_16
        + p_p->c2119y * pow_u_21  *18.0*19.0*pow_v_17
        + p_p->c2120y * pow_u_21  *19.0*20.0*pow_v_18
        + p_p->c2121y * pow_u_21  *20.0*21.0*pow_v_19;

v2_z    = p_p->c0000z * pow_u_0  *            0.0          
        + p_p->c0001z * pow_u_0  *            0.0          
        + p_p->c0002z * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003z * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004z * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005z * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006z * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007z * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0008z * pow_u_0  * 7.0* 8.0*pow_v_6
        + p_p->c0009z * pow_u_0  * 8.0* 9.0*pow_v_7
        + p_p->c0010z * pow_u_0  * 9.0*10.0*pow_v_8
        + p_p->c0011z * pow_u_0  *10.0*11.0*pow_v_9
        + p_p->c0012z * pow_u_0  *11.0*12.0*pow_v_10
        + p_p->c0013z * pow_u_0  *12.0*13.0*pow_v_11
        + p_p->c0014z * pow_u_0  *13.0*14.0*pow_v_12
        + p_p->c0015z * pow_u_0  *14.0*15.0*pow_v_13
        + p_p->c0016z * pow_u_0  *15.0*16.0*pow_v_14
        + p_p->c0017z * pow_u_0  *16.0*17.0*pow_v_15
        + p_p->c0018z * pow_u_0  *17.0*18.0*pow_v_16
        + p_p->c0019z * pow_u_0  *18.0*19.0*pow_v_17
        + p_p->c0020z * pow_u_0  *19.0*20.0*pow_v_18
        + p_p->c0021z * pow_u_0  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0100z * pow_u_1  *            0.0          
        + p_p->c0101z * pow_u_1  *            0.0          
        + p_p->c0102z * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103z * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104z * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105z * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106z * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107z * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0108z * pow_u_1  * 7.0* 8.0*pow_v_6
        + p_p->c0109z * pow_u_1  * 8.0* 9.0*pow_v_7
        + p_p->c0110z * pow_u_1  * 9.0*10.0*pow_v_8
        + p_p->c0111z * pow_u_1  *10.0*11.0*pow_v_9
        + p_p->c0112z * pow_u_1  *11.0*12.0*pow_v_10
        + p_p->c0113z * pow_u_1  *12.0*13.0*pow_v_11
        + p_p->c0114z * pow_u_1  *13.0*14.0*pow_v_12
        + p_p->c0115z * pow_u_1  *14.0*15.0*pow_v_13
        + p_p->c0116z * pow_u_1  *15.0*16.0*pow_v_14
        + p_p->c0117z * pow_u_1  *16.0*17.0*pow_v_15
        + p_p->c0118z * pow_u_1  *17.0*18.0*pow_v_16
        + p_p->c0119z * pow_u_1  *18.0*19.0*pow_v_17
        + p_p->c0120z * pow_u_1  *19.0*20.0*pow_v_18
        + p_p->c0121z * pow_u_1  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0200z * pow_u_2  *            0.0          
        + p_p->c0201z * pow_u_2  *            0.0          
        + p_p->c0202z * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203z * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204z * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205z * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206z * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207z * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0208z * pow_u_2  * 7.0* 8.0*pow_v_6
        + p_p->c0209z * pow_u_2  * 8.0* 9.0*pow_v_7
        + p_p->c0210z * pow_u_2  * 9.0*10.0*pow_v_8
        + p_p->c0211z * pow_u_2  *10.0*11.0*pow_v_9
        + p_p->c0212z * pow_u_2  *11.0*12.0*pow_v_10
        + p_p->c0213z * pow_u_2  *12.0*13.0*pow_v_11
        + p_p->c0214z * pow_u_2  *13.0*14.0*pow_v_12
        + p_p->c0215z * pow_u_2  *14.0*15.0*pow_v_13
        + p_p->c0216z * pow_u_2  *15.0*16.0*pow_v_14
        + p_p->c0217z * pow_u_2  *16.0*17.0*pow_v_15
        + p_p->c0218z * pow_u_2  *17.0*18.0*pow_v_16
        + p_p->c0219z * pow_u_2  *18.0*19.0*pow_v_17
        + p_p->c0220z * pow_u_2  *19.0*20.0*pow_v_18
        + p_p->c0221z * pow_u_2  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0300z * pow_u_3  *            0.0          
        + p_p->c0301z * pow_u_3  *            0.0          
        + p_p->c0302z * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303z * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304z * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305z * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306z * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307z * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0308z * pow_u_3  * 7.0* 8.0*pow_v_6
        + p_p->c0309z * pow_u_3  * 8.0* 9.0*pow_v_7
        + p_p->c0310z * pow_u_3  * 9.0*10.0*pow_v_8
        + p_p->c0311z * pow_u_3  *10.0*11.0*pow_v_9
        + p_p->c0312z * pow_u_3  *11.0*12.0*pow_v_10
        + p_p->c0313z * pow_u_3  *12.0*13.0*pow_v_11
        + p_p->c0314z * pow_u_3  *13.0*14.0*pow_v_12
        + p_p->c0315z * pow_u_3  *14.0*15.0*pow_v_13
        + p_p->c0316z * pow_u_3  *15.0*16.0*pow_v_14
        + p_p->c0317z * pow_u_3  *16.0*17.0*pow_v_15
        + p_p->c0318z * pow_u_3  *17.0*18.0*pow_v_16
        + p_p->c0319z * pow_u_3  *18.0*19.0*pow_v_17
        + p_p->c0320z * pow_u_3  *19.0*20.0*pow_v_18
        + p_p->c0321z * pow_u_3  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0400z * pow_u_4  *            0.0          
        + p_p->c0401z * pow_u_4  *            0.0          
        + p_p->c0402z * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403z * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404z * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405z * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406z * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407z * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0408z * pow_u_4  * 7.0* 8.0*pow_v_6
        + p_p->c0409z * pow_u_4  * 8.0* 9.0*pow_v_7
        + p_p->c0410z * pow_u_4  * 9.0*10.0*pow_v_8
        + p_p->c0411z * pow_u_4  *10.0*11.0*pow_v_9
        + p_p->c0412z * pow_u_4  *11.0*12.0*pow_v_10
        + p_p->c0413z * pow_u_4  *12.0*13.0*pow_v_11
        + p_p->c0414z * pow_u_4  *13.0*14.0*pow_v_12
        + p_p->c0415z * pow_u_4  *14.0*15.0*pow_v_13
        + p_p->c0416z * pow_u_4  *15.0*16.0*pow_v_14
        + p_p->c0417z * pow_u_4  *16.0*17.0*pow_v_15
        + p_p->c0418z * pow_u_4  *17.0*18.0*pow_v_16
        + p_p->c0419z * pow_u_4  *18.0*19.0*pow_v_17
        + p_p->c0420z * pow_u_4  *19.0*20.0*pow_v_18
        + p_p->c0421z * pow_u_4  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0500z * pow_u_5  *            0.0          
        + p_p->c0501z * pow_u_5  *            0.0          
        + p_p->c0502z * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503z * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504z * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505z * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506z * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507z * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0508z * pow_u_5  * 7.0* 8.0*pow_v_6
        + p_p->c0509z * pow_u_5  * 8.0* 9.0*pow_v_7
        + p_p->c0510z * pow_u_5  * 9.0*10.0*pow_v_8
        + p_p->c0511z * pow_u_5  *10.0*11.0*pow_v_9
        + p_p->c0512z * pow_u_5  *11.0*12.0*pow_v_10
        + p_p->c0513z * pow_u_5  *12.0*13.0*pow_v_11
        + p_p->c0514z * pow_u_5  *13.0*14.0*pow_v_12
        + p_p->c0515z * pow_u_5  *14.0*15.0*pow_v_13
        + p_p->c0516z * pow_u_5  *15.0*16.0*pow_v_14
        + p_p->c0517z * pow_u_5  *16.0*17.0*pow_v_15
        + p_p->c0518z * pow_u_5  *17.0*18.0*pow_v_16
        + p_p->c0519z * pow_u_5  *18.0*19.0*pow_v_17
        + p_p->c0520z * pow_u_5  *19.0*20.0*pow_v_18
        + p_p->c0521z * pow_u_5  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0600z * pow_u_6  *            0.0          
        + p_p->c0601z * pow_u_6  *            0.0          
        + p_p->c0602z * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603z * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604z * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605z * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606z * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607z * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0608z * pow_u_6  * 7.0* 8.0*pow_v_6
        + p_p->c0609z * pow_u_6  * 8.0* 9.0*pow_v_7
        + p_p->c0610z * pow_u_6  * 9.0*10.0*pow_v_8
        + p_p->c0611z * pow_u_6  *10.0*11.0*pow_v_9
        + p_p->c0612z * pow_u_6  *11.0*12.0*pow_v_10
        + p_p->c0613z * pow_u_6  *12.0*13.0*pow_v_11
        + p_p->c0614z * pow_u_6  *13.0*14.0*pow_v_12
        + p_p->c0615z * pow_u_6  *14.0*15.0*pow_v_13
        + p_p->c0616z * pow_u_6  *15.0*16.0*pow_v_14
        + p_p->c0617z * pow_u_6  *16.0*17.0*pow_v_15
        + p_p->c0618z * pow_u_6  *17.0*18.0*pow_v_16
        + p_p->c0619z * pow_u_6  *18.0*19.0*pow_v_17
        + p_p->c0620z * pow_u_6  *19.0*20.0*pow_v_18
        + p_p->c0621z * pow_u_6  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0700z * pow_u_7  *            0.0          
        + p_p->c0701z * pow_u_7  *            0.0          
        + p_p->c0702z * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703z * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704z * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705z * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706z * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707z * pow_u_7  * 6.0* 7.0*pow_v_5
        + p_p->c0708z * pow_u_7  * 7.0* 8.0*pow_v_6
        + p_p->c0709z * pow_u_7  * 8.0* 9.0*pow_v_7
        + p_p->c0710z * pow_u_7  * 9.0*10.0*pow_v_8
        + p_p->c0711z * pow_u_7  *10.0*11.0*pow_v_9
        + p_p->c0712z * pow_u_7  *11.0*12.0*pow_v_10
        + p_p->c0713z * pow_u_7  *12.0*13.0*pow_v_11
        + p_p->c0714z * pow_u_7  *13.0*14.0*pow_v_12
        + p_p->c0715z * pow_u_7  *14.0*15.0*pow_v_13
        + p_p->c0716z * pow_u_7  *15.0*16.0*pow_v_14
        + p_p->c0717z * pow_u_7  *16.0*17.0*pow_v_15
        + p_p->c0718z * pow_u_7  *17.0*18.0*pow_v_16
        + p_p->c0719z * pow_u_7  *18.0*19.0*pow_v_17
        + p_p->c0720z * pow_u_7  *19.0*20.0*pow_v_18
        + p_p->c0721z * pow_u_7  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0800z * pow_u_8  *            0.0          
        + p_p->c0801z * pow_u_8  *            0.0          
        + p_p->c0802z * pow_u_8  * 1.0* 2.0*pow_v_0
        + p_p->c0803z * pow_u_8  * 2.0* 3.0*pow_v_1
        + p_p->c0804z * pow_u_8  * 3.0* 4.0*pow_v_2
        + p_p->c0805z * pow_u_8  * 4.0* 5.0*pow_v_3
        + p_p->c0806z * pow_u_8  * 5.0* 6.0*pow_v_4
        + p_p->c0807z * pow_u_8  * 6.0* 7.0*pow_v_5
        + p_p->c0808z * pow_u_8  * 7.0* 8.0*pow_v_6
        + p_p->c0809z * pow_u_8  * 8.0* 9.0*pow_v_7
        + p_p->c0810z * pow_u_8  * 9.0*10.0*pow_v_8
        + p_p->c0811z * pow_u_8  *10.0*11.0*pow_v_9
        + p_p->c0812z * pow_u_8  *11.0*12.0*pow_v_10
        + p_p->c0813z * pow_u_8  *12.0*13.0*pow_v_11
        + p_p->c0814z * pow_u_8  *13.0*14.0*pow_v_12
        + p_p->c0815z * pow_u_8  *14.0*15.0*pow_v_13
        + p_p->c0816z * pow_u_8  *15.0*16.0*pow_v_14
        + p_p->c0817z * pow_u_8  *16.0*17.0*pow_v_15
        + p_p->c0818z * pow_u_8  *17.0*18.0*pow_v_16
        + p_p->c0819z * pow_u_8  *18.0*19.0*pow_v_17
        + p_p->c0820z * pow_u_8  *19.0*20.0*pow_v_18
        + p_p->c0821z * pow_u_8  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c0900z * pow_u_9  *            0.0          
        + p_p->c0901z * pow_u_9  *            0.0          
        + p_p->c0902z * pow_u_9  * 1.0* 2.0*pow_v_0
        + p_p->c0903z * pow_u_9  * 2.0* 3.0*pow_v_1
        + p_p->c0904z * pow_u_9  * 3.0* 4.0*pow_v_2
        + p_p->c0905z * pow_u_9  * 4.0* 5.0*pow_v_3
        + p_p->c0906z * pow_u_9  * 5.0* 6.0*pow_v_4
        + p_p->c0907z * pow_u_9  * 6.0* 7.0*pow_v_5
        + p_p->c0908z * pow_u_9  * 7.0* 8.0*pow_v_6
        + p_p->c0909z * pow_u_9  * 8.0* 9.0*pow_v_7
        + p_p->c0910z * pow_u_9  * 9.0*10.0*pow_v_8
        + p_p->c0911z * pow_u_9  *10.0*11.0*pow_v_9
        + p_p->c0912z * pow_u_9  *11.0*12.0*pow_v_10
        + p_p->c0913z * pow_u_9  *12.0*13.0*pow_v_11
        + p_p->c0914z * pow_u_9  *13.0*14.0*pow_v_12
        + p_p->c0915z * pow_u_9  *14.0*15.0*pow_v_13
        + p_p->c0916z * pow_u_9  *15.0*16.0*pow_v_14
        + p_p->c0917z * pow_u_9  *16.0*17.0*pow_v_15
        + p_p->c0918z * pow_u_9  *17.0*18.0*pow_v_16
        + p_p->c0919z * pow_u_9  *18.0*19.0*pow_v_17
        + p_p->c0920z * pow_u_9  *19.0*20.0*pow_v_18
        + p_p->c0921z * pow_u_9  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1000z * pow_u_10  *            0.0          
        + p_p->c1001z * pow_u_10  *            0.0          
        + p_p->c1002z * pow_u_10  * 1.0* 2.0*pow_v_0
        + p_p->c1003z * pow_u_10  * 2.0* 3.0*pow_v_1
        + p_p->c1004z * pow_u_10  * 3.0* 4.0*pow_v_2
        + p_p->c1005z * pow_u_10  * 4.0* 5.0*pow_v_3
        + p_p->c1006z * pow_u_10  * 5.0* 6.0*pow_v_4
        + p_p->c1007z * pow_u_10  * 6.0* 7.0*pow_v_5
        + p_p->c1008z * pow_u_10  * 7.0* 8.0*pow_v_6
        + p_p->c1009z * pow_u_10  * 8.0* 9.0*pow_v_7
        + p_p->c1010z * pow_u_10  * 9.0*10.0*pow_v_8
        + p_p->c1011z * pow_u_10  *10.0*11.0*pow_v_9
        + p_p->c1012z * pow_u_10  *11.0*12.0*pow_v_10
        + p_p->c1013z * pow_u_10  *12.0*13.0*pow_v_11
        + p_p->c1014z * pow_u_10  *13.0*14.0*pow_v_12
        + p_p->c1015z * pow_u_10  *14.0*15.0*pow_v_13
        + p_p->c1016z * pow_u_10  *15.0*16.0*pow_v_14
        + p_p->c1017z * pow_u_10  *16.0*17.0*pow_v_15
        + p_p->c1018z * pow_u_10  *17.0*18.0*pow_v_16
        + p_p->c1019z * pow_u_10  *18.0*19.0*pow_v_17
        + p_p->c1020z * pow_u_10  *19.0*20.0*pow_v_18
        + p_p->c1021z * pow_u_10  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1100z * pow_u_11  *            0.0          
        + p_p->c1101z * pow_u_11  *            0.0          
        + p_p->c1102z * pow_u_11  * 1.0* 2.0*pow_v_0
        + p_p->c1103z * pow_u_11  * 2.0* 3.0*pow_v_1
        + p_p->c1104z * pow_u_11  * 3.0* 4.0*pow_v_2
        + p_p->c1105z * pow_u_11  * 4.0* 5.0*pow_v_3
        + p_p->c1106z * pow_u_11  * 5.0* 6.0*pow_v_4
        + p_p->c1107z * pow_u_11  * 6.0* 7.0*pow_v_5
        + p_p->c1108z * pow_u_11  * 7.0* 8.0*pow_v_6
        + p_p->c1109z * pow_u_11  * 8.0* 9.0*pow_v_7
        + p_p->c1110z * pow_u_11  * 9.0*10.0*pow_v_8
        + p_p->c1111z * pow_u_11  *10.0*11.0*pow_v_9
        + p_p->c1112z * pow_u_11  *11.0*12.0*pow_v_10
        + p_p->c1113z * pow_u_11  *12.0*13.0*pow_v_11
        + p_p->c1114z * pow_u_11  *13.0*14.0*pow_v_12
        + p_p->c1115z * pow_u_11  *14.0*15.0*pow_v_13
        + p_p->c1116z * pow_u_11  *15.0*16.0*pow_v_14
        + p_p->c1117z * pow_u_11  *16.0*17.0*pow_v_15
        + p_p->c1118z * pow_u_11  *17.0*18.0*pow_v_16
        + p_p->c1119z * pow_u_11  *18.0*19.0*pow_v_17
        + p_p->c1120z * pow_u_11  *19.0*20.0*pow_v_18
        + p_p->c1121z * pow_u_11  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1200z * pow_u_12  *            0.0          
        + p_p->c1201z * pow_u_12  *            0.0          
        + p_p->c1202z * pow_u_12  * 1.0* 2.0*pow_v_0
        + p_p->c1203z * pow_u_12  * 2.0* 3.0*pow_v_1
        + p_p->c1204z * pow_u_12  * 3.0* 4.0*pow_v_2
        + p_p->c1205z * pow_u_12  * 4.0* 5.0*pow_v_3
        + p_p->c1206z * pow_u_12  * 5.0* 6.0*pow_v_4
        + p_p->c1207z * pow_u_12  * 6.0* 7.0*pow_v_5
        + p_p->c1208z * pow_u_12  * 7.0* 8.0*pow_v_6
        + p_p->c1209z * pow_u_12  * 8.0* 9.0*pow_v_7
        + p_p->c1210z * pow_u_12  * 9.0*10.0*pow_v_8
        + p_p->c1211z * pow_u_12  *10.0*11.0*pow_v_9
        + p_p->c1212z * pow_u_12  *11.0*12.0*pow_v_10
        + p_p->c1213z * pow_u_12  *12.0*13.0*pow_v_11
        + p_p->c1214z * pow_u_12  *13.0*14.0*pow_v_12
        + p_p->c1215z * pow_u_12  *14.0*15.0*pow_v_13
        + p_p->c1216z * pow_u_12  *15.0*16.0*pow_v_14
        + p_p->c1217z * pow_u_12  *16.0*17.0*pow_v_15
        + p_p->c1218z * pow_u_12  *17.0*18.0*pow_v_16
        + p_p->c1219z * pow_u_12  *18.0*19.0*pow_v_17
        + p_p->c1220z * pow_u_12  *19.0*20.0*pow_v_18
        + p_p->c1221z * pow_u_12  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1300z * pow_u_13  *            0.0          
        + p_p->c1301z * pow_u_13  *            0.0          
        + p_p->c1302z * pow_u_13  * 1.0* 2.0*pow_v_0
        + p_p->c1303z * pow_u_13  * 2.0* 3.0*pow_v_1
        + p_p->c1304z * pow_u_13  * 3.0* 4.0*pow_v_2
        + p_p->c1305z * pow_u_13  * 4.0* 5.0*pow_v_3
        + p_p->c1306z * pow_u_13  * 5.0* 6.0*pow_v_4
        + p_p->c1307z * pow_u_13  * 6.0* 7.0*pow_v_5
        + p_p->c1308z * pow_u_13  * 7.0* 8.0*pow_v_6
        + p_p->c1309z * pow_u_13  * 8.0* 9.0*pow_v_7
        + p_p->c1310z * pow_u_13  * 9.0*10.0*pow_v_8
        + p_p->c1311z * pow_u_13  *10.0*11.0*pow_v_9
        + p_p->c1312z * pow_u_13  *11.0*12.0*pow_v_10
        + p_p->c1313z * pow_u_13  *12.0*13.0*pow_v_11
        + p_p->c1314z * pow_u_13  *13.0*14.0*pow_v_12
        + p_p->c1315z * pow_u_13  *14.0*15.0*pow_v_13
        + p_p->c1316z * pow_u_13  *15.0*16.0*pow_v_14
        + p_p->c1317z * pow_u_13  *16.0*17.0*pow_v_15
        + p_p->c1318z * pow_u_13  *17.0*18.0*pow_v_16
        + p_p->c1319z * pow_u_13  *18.0*19.0*pow_v_17
        + p_p->c1320z * pow_u_13  *19.0*20.0*pow_v_18
        + p_p->c1321z * pow_u_13  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1400z * pow_u_14  *            0.0          
        + p_p->c1401z * pow_u_14  *            0.0          
        + p_p->c1402z * pow_u_14  * 1.0* 2.0*pow_v_0
        + p_p->c1403z * pow_u_14  * 2.0* 3.0*pow_v_1
        + p_p->c1404z * pow_u_14  * 3.0* 4.0*pow_v_2
        + p_p->c1405z * pow_u_14  * 4.0* 5.0*pow_v_3
        + p_p->c1406z * pow_u_14  * 5.0* 6.0*pow_v_4
        + p_p->c1407z * pow_u_14  * 6.0* 7.0*pow_v_5
        + p_p->c1408z * pow_u_14  * 7.0* 8.0*pow_v_6
        + p_p->c1409z * pow_u_14  * 8.0* 9.0*pow_v_7
        + p_p->c1410z * pow_u_14  * 9.0*10.0*pow_v_8
        + p_p->c1411z * pow_u_14  *10.0*11.0*pow_v_9
        + p_p->c1412z * pow_u_14  *11.0*12.0*pow_v_10
        + p_p->c1413z * pow_u_14  *12.0*13.0*pow_v_11
        + p_p->c1414z * pow_u_14  *13.0*14.0*pow_v_12
        + p_p->c1415z * pow_u_14  *14.0*15.0*pow_v_13
        + p_p->c1416z * pow_u_14  *15.0*16.0*pow_v_14
        + p_p->c1417z * pow_u_14  *16.0*17.0*pow_v_15
        + p_p->c1418z * pow_u_14  *17.0*18.0*pow_v_16
        + p_p->c1419z * pow_u_14  *18.0*19.0*pow_v_17
        + p_p->c1420z * pow_u_14  *19.0*20.0*pow_v_18
        + p_p->c1421z * pow_u_14  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1500z * pow_u_15  *            0.0          
        + p_p->c1501z * pow_u_15  *            0.0          
        + p_p->c1502z * pow_u_15  * 1.0* 2.0*pow_v_0
        + p_p->c1503z * pow_u_15  * 2.0* 3.0*pow_v_1
        + p_p->c1504z * pow_u_15  * 3.0* 4.0*pow_v_2
        + p_p->c1505z * pow_u_15  * 4.0* 5.0*pow_v_3
        + p_p->c1506z * pow_u_15  * 5.0* 6.0*pow_v_4
        + p_p->c1507z * pow_u_15  * 6.0* 7.0*pow_v_5
        + p_p->c1508z * pow_u_15  * 7.0* 8.0*pow_v_6
        + p_p->c1509z * pow_u_15  * 8.0* 9.0*pow_v_7
        + p_p->c1510z * pow_u_15  * 9.0*10.0*pow_v_8
        + p_p->c1511z * pow_u_15  *10.0*11.0*pow_v_9
        + p_p->c1512z * pow_u_15  *11.0*12.0*pow_v_10
        + p_p->c1513z * pow_u_15  *12.0*13.0*pow_v_11
        + p_p->c1514z * pow_u_15  *13.0*14.0*pow_v_12
        + p_p->c1515z * pow_u_15  *14.0*15.0*pow_v_13
        + p_p->c1516z * pow_u_15  *15.0*16.0*pow_v_14
        + p_p->c1517z * pow_u_15  *16.0*17.0*pow_v_15
        + p_p->c1518z * pow_u_15  *17.0*18.0*pow_v_16
        + p_p->c1519z * pow_u_15  *18.0*19.0*pow_v_17
        + p_p->c1520z * pow_u_15  *19.0*20.0*pow_v_18
        + p_p->c1521z * pow_u_15  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1600z * pow_u_16  *            0.0          
        + p_p->c1601z * pow_u_16  *            0.0          
        + p_p->c1602z * pow_u_16  * 1.0* 2.0*pow_v_0
        + p_p->c1603z * pow_u_16  * 2.0* 3.0*pow_v_1
        + p_p->c1604z * pow_u_16  * 3.0* 4.0*pow_v_2
        + p_p->c1605z * pow_u_16  * 4.0* 5.0*pow_v_3
        + p_p->c1606z * pow_u_16  * 5.0* 6.0*pow_v_4
        + p_p->c1607z * pow_u_16  * 6.0* 7.0*pow_v_5
        + p_p->c1608z * pow_u_16  * 7.0* 8.0*pow_v_6
        + p_p->c1609z * pow_u_16  * 8.0* 9.0*pow_v_7
        + p_p->c1610z * pow_u_16  * 9.0*10.0*pow_v_8
        + p_p->c1611z * pow_u_16  *10.0*11.0*pow_v_9
        + p_p->c1612z * pow_u_16  *11.0*12.0*pow_v_10
        + p_p->c1613z * pow_u_16  *12.0*13.0*pow_v_11
        + p_p->c1614z * pow_u_16  *13.0*14.0*pow_v_12
        + p_p->c1615z * pow_u_16  *14.0*15.0*pow_v_13
        + p_p->c1616z * pow_u_16  *15.0*16.0*pow_v_14
        + p_p->c1617z * pow_u_16  *16.0*17.0*pow_v_15
        + p_p->c1618z * pow_u_16  *17.0*18.0*pow_v_16
        + p_p->c1619z * pow_u_16  *18.0*19.0*pow_v_17
        + p_p->c1620z * pow_u_16  *19.0*20.0*pow_v_18
        + p_p->c1621z * pow_u_16  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1700z * pow_u_17  *            0.0          
        + p_p->c1701z * pow_u_17  *            0.0          
        + p_p->c1702z * pow_u_17  * 1.0* 2.0*pow_v_0
        + p_p->c1703z * pow_u_17  * 2.0* 3.0*pow_v_1
        + p_p->c1704z * pow_u_17  * 3.0* 4.0*pow_v_2
        + p_p->c1705z * pow_u_17  * 4.0* 5.0*pow_v_3
        + p_p->c1706z * pow_u_17  * 5.0* 6.0*pow_v_4
        + p_p->c1707z * pow_u_17  * 6.0* 7.0*pow_v_5
        + p_p->c1708z * pow_u_17  * 7.0* 8.0*pow_v_6
        + p_p->c1709z * pow_u_17  * 8.0* 9.0*pow_v_7
        + p_p->c1710z * pow_u_17  * 9.0*10.0*pow_v_8
        + p_p->c1711z * pow_u_17  *10.0*11.0*pow_v_9
        + p_p->c1712z * pow_u_17  *11.0*12.0*pow_v_10
        + p_p->c1713z * pow_u_17  *12.0*13.0*pow_v_11
        + p_p->c1714z * pow_u_17  *13.0*14.0*pow_v_12
        + p_p->c1715z * pow_u_17  *14.0*15.0*pow_v_13
        + p_p->c1716z * pow_u_17  *15.0*16.0*pow_v_14
        + p_p->c1717z * pow_u_17  *16.0*17.0*pow_v_15
        + p_p->c1718z * pow_u_17  *17.0*18.0*pow_v_16
        + p_p->c1719z * pow_u_17  *18.0*19.0*pow_v_17
        + p_p->c1720z * pow_u_17  *19.0*20.0*pow_v_18
        + p_p->c1721z * pow_u_17  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1800z * pow_u_18  *            0.0          
        + p_p->c1801z * pow_u_18  *            0.0          
        + p_p->c1802z * pow_u_18  * 1.0* 2.0*pow_v_0
        + p_p->c1803z * pow_u_18  * 2.0* 3.0*pow_v_1
        + p_p->c1804z * pow_u_18  * 3.0* 4.0*pow_v_2
        + p_p->c1805z * pow_u_18  * 4.0* 5.0*pow_v_3
        + p_p->c1806z * pow_u_18  * 5.0* 6.0*pow_v_4
        + p_p->c1807z * pow_u_18  * 6.0* 7.0*pow_v_5
        + p_p->c1808z * pow_u_18  * 7.0* 8.0*pow_v_6
        + p_p->c1809z * pow_u_18  * 8.0* 9.0*pow_v_7
        + p_p->c1810z * pow_u_18  * 9.0*10.0*pow_v_8
        + p_p->c1811z * pow_u_18  *10.0*11.0*pow_v_9
        + p_p->c1812z * pow_u_18  *11.0*12.0*pow_v_10
        + p_p->c1813z * pow_u_18  *12.0*13.0*pow_v_11
        + p_p->c1814z * pow_u_18  *13.0*14.0*pow_v_12
        + p_p->c1815z * pow_u_18  *14.0*15.0*pow_v_13
        + p_p->c1816z * pow_u_18  *15.0*16.0*pow_v_14
        + p_p->c1817z * pow_u_18  *16.0*17.0*pow_v_15
        + p_p->c1818z * pow_u_18  *17.0*18.0*pow_v_16
        + p_p->c1819z * pow_u_18  *18.0*19.0*pow_v_17
        + p_p->c1820z * pow_u_18  *19.0*20.0*pow_v_18
        + p_p->c1821z * pow_u_18  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c1900z * pow_u_19  *            0.0          
        + p_p->c1901z * pow_u_19  *            0.0          
        + p_p->c1902z * pow_u_19  * 1.0* 2.0*pow_v_0
        + p_p->c1903z * pow_u_19  * 2.0* 3.0*pow_v_1
        + p_p->c1904z * pow_u_19  * 3.0* 4.0*pow_v_2
        + p_p->c1905z * pow_u_19  * 4.0* 5.0*pow_v_3
        + p_p->c1906z * pow_u_19  * 5.0* 6.0*pow_v_4
        + p_p->c1907z * pow_u_19  * 6.0* 7.0*pow_v_5
        + p_p->c1908z * pow_u_19  * 7.0* 8.0*pow_v_6
        + p_p->c1909z * pow_u_19  * 8.0* 9.0*pow_v_7
        + p_p->c1910z * pow_u_19  * 9.0*10.0*pow_v_8
        + p_p->c1911z * pow_u_19  *10.0*11.0*pow_v_9
        + p_p->c1912z * pow_u_19  *11.0*12.0*pow_v_10
        + p_p->c1913z * pow_u_19  *12.0*13.0*pow_v_11
        + p_p->c1914z * pow_u_19  *13.0*14.0*pow_v_12
        + p_p->c1915z * pow_u_19  *14.0*15.0*pow_v_13
        + p_p->c1916z * pow_u_19  *15.0*16.0*pow_v_14
        + p_p->c1917z * pow_u_19  *16.0*17.0*pow_v_15
        + p_p->c1918z * pow_u_19  *17.0*18.0*pow_v_16
        + p_p->c1919z * pow_u_19  *18.0*19.0*pow_v_17
        + p_p->c1920z * pow_u_19  *19.0*20.0*pow_v_18
        + p_p->c1921z * pow_u_19  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c2000z * pow_u_20  *            0.0          
        + p_p->c2001z * pow_u_20  *            0.0          
        + p_p->c2002z * pow_u_20  * 1.0* 2.0*pow_v_0
        + p_p->c2003z * pow_u_20  * 2.0* 3.0*pow_v_1
        + p_p->c2004z * pow_u_20  * 3.0* 4.0*pow_v_2
        + p_p->c2005z * pow_u_20  * 4.0* 5.0*pow_v_3
        + p_p->c2006z * pow_u_20  * 5.0* 6.0*pow_v_4
        + p_p->c2007z * pow_u_20  * 6.0* 7.0*pow_v_5
        + p_p->c2008z * pow_u_20  * 7.0* 8.0*pow_v_6
        + p_p->c2009z * pow_u_20  * 8.0* 9.0*pow_v_7
        + p_p->c2010z * pow_u_20  * 9.0*10.0*pow_v_8
        + p_p->c2011z * pow_u_20  *10.0*11.0*pow_v_9
        + p_p->c2012z * pow_u_20  *11.0*12.0*pow_v_10
        + p_p->c2013z * pow_u_20  *12.0*13.0*pow_v_11
        + p_p->c2014z * pow_u_20  *13.0*14.0*pow_v_12
        + p_p->c2015z * pow_u_20  *14.0*15.0*pow_v_13
        + p_p->c2016z * pow_u_20  *15.0*16.0*pow_v_14
        + p_p->c2017z * pow_u_20  *16.0*17.0*pow_v_15
        + p_p->c2018z * pow_u_20  *17.0*18.0*pow_v_16
        + p_p->c2019z * pow_u_20  *18.0*19.0*pow_v_17
        + p_p->c2020z * pow_u_20  *19.0*20.0*pow_v_18
        + p_p->c2021z * pow_u_20  *20.0*21.0*pow_v_19;
v2_z=v2_z + p_p->c2100z * pow_u_21  *            0.0          
        + p_p->c2101z * pow_u_21  *            0.0          
        + p_p->c2102z * pow_u_21  * 1.0* 2.0*pow_v_0
        + p_p->c2103z * pow_u_21  * 2.0* 3.0*pow_v_1
        + p_p->c2104z * pow_u_21  * 3.0* 4.0*pow_v_2
        + p_p->c2105z * pow_u_21  * 4.0* 5.0*pow_v_3
        + p_p->c2106z * pow_u_21  * 5.0* 6.0*pow_v_4
        + p_p->c2107z * pow_u_21  * 6.0* 7.0*pow_v_5
        + p_p->c2108z * pow_u_21  * 7.0* 8.0*pow_v_6
        + p_p->c2109z * pow_u_21  * 8.0* 9.0*pow_v_7
        + p_p->c2110z * pow_u_21  * 9.0*10.0*pow_v_8
        + p_p->c2111z * pow_u_21  *10.0*11.0*pow_v_9
        + p_p->c2112z * pow_u_21  *11.0*12.0*pow_v_10
        + p_p->c2113z * pow_u_21  *12.0*13.0*pow_v_11
        + p_p->c2114z * pow_u_21  *13.0*14.0*pow_v_12
        + p_p->c2115z * pow_u_21  *14.0*15.0*pow_v_13
        + p_p->c2116z * pow_u_21  *15.0*16.0*pow_v_14
        + p_p->c2117z * pow_u_21  *16.0*17.0*pow_v_15
        + p_p->c2118z * pow_u_21  *17.0*18.0*pow_v_16
        + p_p->c2119z * pow_u_21  *18.0*19.0*pow_v_17
        + p_p->c2120z * pow_u_21  *19.0*20.0*pow_v_18
        + p_p->c2121z * pow_u_21  *20.0*21.0*pow_v_19;

uv_x    = p_p->c0000x*         0.0        *         0.0        
        + p_p->c0001x*         0.0        * 1.0*pow_v_0
        + p_p->c0002x*         0.0        * 2.0*pow_v_1
        + p_p->c0003x*         0.0        * 3.0*pow_v_2
        + p_p->c0004x*         0.0        * 4.0*pow_v_3
        + p_p->c0005x*         0.0        * 5.0*pow_v_4
        + p_p->c0006x*         0.0        * 6.0*pow_v_5
        + p_p->c0007x*         0.0        * 7.0*pow_v_6
        + p_p->c0008x*         0.0        * 8.0*pow_v_7
        + p_p->c0009x*         0.0        * 9.0*pow_v_8
        + p_p->c0010x*         0.0        *10.0*pow_v_9
        + p_p->c0011x*         0.0        *11.0*pow_v_10
        + p_p->c0012x*         0.0        *12.0*pow_v_11
        + p_p->c0013x*         0.0        *13.0*pow_v_12
        + p_p->c0014x*         0.0        *14.0*pow_v_13
        + p_p->c0015x*         0.0        *15.0*pow_v_14
        + p_p->c0016x*         0.0        *16.0*pow_v_15
        + p_p->c0017x*         0.0        *17.0*pow_v_16
        + p_p->c0018x*         0.0        *18.0*pow_v_17
        + p_p->c0019x*         0.0        *19.0*pow_v_18
        + p_p->c0020x*         0.0        *20.0*pow_v_19
        + p_p->c0021x*         0.0        *21.0*pow_v_20;
uv_x=uv_x + p_p->c0100x* 1.0*pow_u_0*         0.0        
        + p_p->c0101x* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102x* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103x* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104x* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105x* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106x* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107x* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0108x* 1.0*pow_u_0* 8.0*pow_v_7
        + p_p->c0109x* 1.0*pow_u_0* 9.0*pow_v_8
        + p_p->c0110x* 1.0*pow_u_0*10.0*pow_v_9
        + p_p->c0111x* 1.0*pow_u_0*11.0*pow_v_10
        + p_p->c0112x* 1.0*pow_u_0*12.0*pow_v_11
        + p_p->c0113x* 1.0*pow_u_0*13.0*pow_v_12
        + p_p->c0114x* 1.0*pow_u_0*14.0*pow_v_13
        + p_p->c0115x* 1.0*pow_u_0*15.0*pow_v_14
        + p_p->c0116x* 1.0*pow_u_0*16.0*pow_v_15
        + p_p->c0117x* 1.0*pow_u_0*17.0*pow_v_16
        + p_p->c0118x* 1.0*pow_u_0*18.0*pow_v_17
        + p_p->c0119x* 1.0*pow_u_0*19.0*pow_v_18
        + p_p->c0120x* 1.0*pow_u_0*20.0*pow_v_19
        + p_p->c0121x* 1.0*pow_u_0*21.0*pow_v_20;
uv_x=uv_x + p_p->c0200x* 2.0*pow_u_1*         0.0        
        + p_p->c0201x* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202x* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203x* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204x* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205x* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206x* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207x* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0208x* 2.0*pow_u_1* 8.0*pow_v_7
        + p_p->c0209x* 2.0*pow_u_1* 9.0*pow_v_8
        + p_p->c0210x* 2.0*pow_u_1*10.0*pow_v_9
        + p_p->c0211x* 2.0*pow_u_1*11.0*pow_v_10
        + p_p->c0212x* 2.0*pow_u_1*12.0*pow_v_11
        + p_p->c0213x* 2.0*pow_u_1*13.0*pow_v_12
        + p_p->c0214x* 2.0*pow_u_1*14.0*pow_v_13
        + p_p->c0215x* 2.0*pow_u_1*15.0*pow_v_14
        + p_p->c0216x* 2.0*pow_u_1*16.0*pow_v_15
        + p_p->c0217x* 2.0*pow_u_1*17.0*pow_v_16
        + p_p->c0218x* 2.0*pow_u_1*18.0*pow_v_17
        + p_p->c0219x* 2.0*pow_u_1*19.0*pow_v_18
        + p_p->c0220x* 2.0*pow_u_1*20.0*pow_v_19
        + p_p->c0221x* 2.0*pow_u_1*21.0*pow_v_20;
uv_x=uv_x + p_p->c0300x* 3.0*pow_u_2*         0.0        
        + p_p->c0301x* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302x* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303x* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304x* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305x* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306x* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307x* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0308x* 3.0*pow_u_2* 8.0*pow_v_7
        + p_p->c0309x* 3.0*pow_u_2* 9.0*pow_v_8
        + p_p->c0310x* 3.0*pow_u_2*10.0*pow_v_9
        + p_p->c0311x* 3.0*pow_u_2*11.0*pow_v_10
        + p_p->c0312x* 3.0*pow_u_2*12.0*pow_v_11
        + p_p->c0313x* 3.0*pow_u_2*13.0*pow_v_12
        + p_p->c0314x* 3.0*pow_u_2*14.0*pow_v_13
        + p_p->c0315x* 3.0*pow_u_2*15.0*pow_v_14
        + p_p->c0316x* 3.0*pow_u_2*16.0*pow_v_15
        + p_p->c0317x* 3.0*pow_u_2*17.0*pow_v_16
        + p_p->c0318x* 3.0*pow_u_2*18.0*pow_v_17
        + p_p->c0319x* 3.0*pow_u_2*19.0*pow_v_18
        + p_p->c0320x* 3.0*pow_u_2*20.0*pow_v_19
        + p_p->c0321x* 3.0*pow_u_2*21.0*pow_v_20;
uv_x=uv_x + p_p->c0400x* 4.0*pow_u_3*         0.0        
        + p_p->c0401x* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402x* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403x* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404x* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405x* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406x* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407x* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0408x* 4.0*pow_u_3* 8.0*pow_v_7
        + p_p->c0409x* 4.0*pow_u_3* 9.0*pow_v_8
        + p_p->c0410x* 4.0*pow_u_3*10.0*pow_v_9
        + p_p->c0411x* 4.0*pow_u_3*11.0*pow_v_10
        + p_p->c0412x* 4.0*pow_u_3*12.0*pow_v_11
        + p_p->c0413x* 4.0*pow_u_3*13.0*pow_v_12
        + p_p->c0414x* 4.0*pow_u_3*14.0*pow_v_13
        + p_p->c0415x* 4.0*pow_u_3*15.0*pow_v_14
        + p_p->c0416x* 4.0*pow_u_3*16.0*pow_v_15
        + p_p->c0417x* 4.0*pow_u_3*17.0*pow_v_16
        + p_p->c0418x* 4.0*pow_u_3*18.0*pow_v_17
        + p_p->c0419x* 4.0*pow_u_3*19.0*pow_v_18
        + p_p->c0420x* 4.0*pow_u_3*20.0*pow_v_19
        + p_p->c0421x* 4.0*pow_u_3*21.0*pow_v_20;
uv_x=uv_x + p_p->c0500x* 5.0*pow_u_4*         0.0        
        + p_p->c0501x* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502x* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503x* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504x* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505x* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506x* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507x* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0508x* 5.0*pow_u_4* 8.0*pow_v_7
        + p_p->c0509x* 5.0*pow_u_4* 9.0*pow_v_8
        + p_p->c0510x* 5.0*pow_u_4*10.0*pow_v_9
        + p_p->c0511x* 5.0*pow_u_4*11.0*pow_v_10
        + p_p->c0512x* 5.0*pow_u_4*12.0*pow_v_11
        + p_p->c0513x* 5.0*pow_u_4*13.0*pow_v_12
        + p_p->c0514x* 5.0*pow_u_4*14.0*pow_v_13
        + p_p->c0515x* 5.0*pow_u_4*15.0*pow_v_14
        + p_p->c0516x* 5.0*pow_u_4*16.0*pow_v_15
        + p_p->c0517x* 5.0*pow_u_4*17.0*pow_v_16
        + p_p->c0518x* 5.0*pow_u_4*18.0*pow_v_17
        + p_p->c0519x* 5.0*pow_u_4*19.0*pow_v_18
        + p_p->c0520x* 5.0*pow_u_4*20.0*pow_v_19
        + p_p->c0521x* 5.0*pow_u_4*21.0*pow_v_20;
uv_x=uv_x + p_p->c0600x* 6.0*pow_u_5*         0.0        
        + p_p->c0601x* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602x* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603x* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604x* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605x* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606x* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607x* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0608x* 6.0*pow_u_5* 8.0*pow_v_7
        + p_p->c0609x* 6.0*pow_u_5* 9.0*pow_v_8
        + p_p->c0610x* 6.0*pow_u_5*10.0*pow_v_9
        + p_p->c0611x* 6.0*pow_u_5*11.0*pow_v_10
        + p_p->c0612x* 6.0*pow_u_5*12.0*pow_v_11
        + p_p->c0613x* 6.0*pow_u_5*13.0*pow_v_12
        + p_p->c0614x* 6.0*pow_u_5*14.0*pow_v_13
        + p_p->c0615x* 6.0*pow_u_5*15.0*pow_v_14
        + p_p->c0616x* 6.0*pow_u_5*16.0*pow_v_15
        + p_p->c0617x* 6.0*pow_u_5*17.0*pow_v_16
        + p_p->c0618x* 6.0*pow_u_5*18.0*pow_v_17
        + p_p->c0619x* 6.0*pow_u_5*19.0*pow_v_18
        + p_p->c0620x* 6.0*pow_u_5*20.0*pow_v_19
        + p_p->c0621x* 6.0*pow_u_5*21.0*pow_v_20;
uv_x=uv_x + p_p->c0700x* 7.0*pow_u_6*         0.0        
        + p_p->c0701x* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702x* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703x* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704x* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705x* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706x* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707x* 7.0*pow_u_6* 7.0*pow_v_6
        + p_p->c0708x* 7.0*pow_u_6* 8.0*pow_v_7
        + p_p->c0709x* 7.0*pow_u_6* 9.0*pow_v_8
        + p_p->c0710x* 7.0*pow_u_6*10.0*pow_v_9
        + p_p->c0711x* 7.0*pow_u_6*11.0*pow_v_10
        + p_p->c0712x* 7.0*pow_u_6*12.0*pow_v_11
        + p_p->c0713x* 7.0*pow_u_6*13.0*pow_v_12
        + p_p->c0714x* 7.0*pow_u_6*14.0*pow_v_13
        + p_p->c0715x* 7.0*pow_u_6*15.0*pow_v_14
        + p_p->c0716x* 7.0*pow_u_6*16.0*pow_v_15
        + p_p->c0717x* 7.0*pow_u_6*17.0*pow_v_16
        + p_p->c0718x* 7.0*pow_u_6*18.0*pow_v_17
        + p_p->c0719x* 7.0*pow_u_6*19.0*pow_v_18
        + p_p->c0720x* 7.0*pow_u_6*20.0*pow_v_19
        + p_p->c0721x* 7.0*pow_u_6*21.0*pow_v_20;
uv_x=uv_x + p_p->c0800x* 8.0*pow_u_7*         0.0        
        + p_p->c0801x* 8.0*pow_u_7* 1.0*pow_v_0
        + p_p->c0802x* 8.0*pow_u_7* 2.0*pow_v_1
        + p_p->c0803x* 8.0*pow_u_7* 3.0*pow_v_2
        + p_p->c0804x* 8.0*pow_u_7* 4.0*pow_v_3
        + p_p->c0805x* 8.0*pow_u_7* 5.0*pow_v_4
        + p_p->c0806x* 8.0*pow_u_7* 6.0*pow_v_5
        + p_p->c0807x* 8.0*pow_u_7* 7.0*pow_v_6
        + p_p->c0808x* 8.0*pow_u_7* 8.0*pow_v_7
        + p_p->c0809x* 8.0*pow_u_7* 9.0*pow_v_8
        + p_p->c0810x* 8.0*pow_u_7*10.0*pow_v_9
        + p_p->c0811x* 8.0*pow_u_7*11.0*pow_v_10
        + p_p->c0812x* 8.0*pow_u_7*12.0*pow_v_11
        + p_p->c0813x* 8.0*pow_u_7*13.0*pow_v_12
        + p_p->c0814x* 8.0*pow_u_7*14.0*pow_v_13
        + p_p->c0815x* 8.0*pow_u_7*15.0*pow_v_14
        + p_p->c0816x* 8.0*pow_u_7*16.0*pow_v_15
        + p_p->c0817x* 8.0*pow_u_7*17.0*pow_v_16
        + p_p->c0818x* 8.0*pow_u_7*18.0*pow_v_17
        + p_p->c0819x* 8.0*pow_u_7*19.0*pow_v_18
        + p_p->c0820x* 8.0*pow_u_7*20.0*pow_v_19
        + p_p->c0821x* 8.0*pow_u_7*21.0*pow_v_20;
uv_x=uv_x + p_p->c0900x* 9.0*pow_u_8*         0.0        
        + p_p->c0901x* 9.0*pow_u_8* 1.0*pow_v_0
        + p_p->c0902x* 9.0*pow_u_8* 2.0*pow_v_1
        + p_p->c0903x* 9.0*pow_u_8* 3.0*pow_v_2
        + p_p->c0904x* 9.0*pow_u_8* 4.0*pow_v_3
        + p_p->c0905x* 9.0*pow_u_8* 5.0*pow_v_4
        + p_p->c0906x* 9.0*pow_u_8* 6.0*pow_v_5
        + p_p->c0907x* 9.0*pow_u_8* 7.0*pow_v_6
        + p_p->c0908x* 9.0*pow_u_8* 8.0*pow_v_7
        + p_p->c0909x* 9.0*pow_u_8* 9.0*pow_v_8
        + p_p->c0910x* 9.0*pow_u_8*10.0*pow_v_9
        + p_p->c0911x* 9.0*pow_u_8*11.0*pow_v_10
        + p_p->c0912x* 9.0*pow_u_8*12.0*pow_v_11
        + p_p->c0913x* 9.0*pow_u_8*13.0*pow_v_12
        + p_p->c0914x* 9.0*pow_u_8*14.0*pow_v_13
        + p_p->c0915x* 9.0*pow_u_8*15.0*pow_v_14
        + p_p->c0916x* 9.0*pow_u_8*16.0*pow_v_15
        + p_p->c0917x* 9.0*pow_u_8*17.0*pow_v_16
        + p_p->c0918x* 9.0*pow_u_8*18.0*pow_v_17
        + p_p->c0919x* 9.0*pow_u_8*19.0*pow_v_18
        + p_p->c0920x* 9.0*pow_u_8*20.0*pow_v_19
        + p_p->c0921x* 9.0*pow_u_8*21.0*pow_v_20;
uv_x=uv_x + p_p->c1000x*10.0*pow_u_9*         0.0        
        + p_p->c1001x*10.0*pow_u_9* 1.0*pow_v_0
        + p_p->c1002x*10.0*pow_u_9* 2.0*pow_v_1
        + p_p->c1003x*10.0*pow_u_9* 3.0*pow_v_2
        + p_p->c1004x*10.0*pow_u_9* 4.0*pow_v_3
        + p_p->c1005x*10.0*pow_u_9* 5.0*pow_v_4
        + p_p->c1006x*10.0*pow_u_9* 6.0*pow_v_5
        + p_p->c1007x*10.0*pow_u_9* 7.0*pow_v_6
        + p_p->c1008x*10.0*pow_u_9* 8.0*pow_v_7
        + p_p->c1009x*10.0*pow_u_9* 9.0*pow_v_8
        + p_p->c1010x*10.0*pow_u_9*10.0*pow_v_9
        + p_p->c1011x*10.0*pow_u_9*11.0*pow_v_10
        + p_p->c1012x*10.0*pow_u_9*12.0*pow_v_11
        + p_p->c1013x*10.0*pow_u_9*13.0*pow_v_12
        + p_p->c1014x*10.0*pow_u_9*14.0*pow_v_13
        + p_p->c1015x*10.0*pow_u_9*15.0*pow_v_14
        + p_p->c1016x*10.0*pow_u_9*16.0*pow_v_15
        + p_p->c1017x*10.0*pow_u_9*17.0*pow_v_16
        + p_p->c1018x*10.0*pow_u_9*18.0*pow_v_17
        + p_p->c1019x*10.0*pow_u_9*19.0*pow_v_18
        + p_p->c1020x*10.0*pow_u_9*20.0*pow_v_19
        + p_p->c1021x*10.0*pow_u_9*21.0*pow_v_20;
uv_x=uv_x + p_p->c1100x*11.0*pow_u_10*         0.0        
        + p_p->c1101x*11.0*pow_u_10* 1.0*pow_v_0
        + p_p->c1102x*11.0*pow_u_10* 2.0*pow_v_1
        + p_p->c1103x*11.0*pow_u_10* 3.0*pow_v_2
        + p_p->c1104x*11.0*pow_u_10* 4.0*pow_v_3
        + p_p->c1105x*11.0*pow_u_10* 5.0*pow_v_4
        + p_p->c1106x*11.0*pow_u_10* 6.0*pow_v_5
        + p_p->c1107x*11.0*pow_u_10* 7.0*pow_v_6
        + p_p->c1108x*11.0*pow_u_10* 8.0*pow_v_7
        + p_p->c1109x*11.0*pow_u_10* 9.0*pow_v_8
        + p_p->c1110x*11.0*pow_u_10*10.0*pow_v_9
        + p_p->c1111x*11.0*pow_u_10*11.0*pow_v_10
        + p_p->c1112x*11.0*pow_u_10*12.0*pow_v_11
        + p_p->c1113x*11.0*pow_u_10*13.0*pow_v_12
        + p_p->c1114x*11.0*pow_u_10*14.0*pow_v_13
        + p_p->c1115x*11.0*pow_u_10*15.0*pow_v_14
        + p_p->c1116x*11.0*pow_u_10*16.0*pow_v_15
        + p_p->c1117x*11.0*pow_u_10*17.0*pow_v_16
        + p_p->c1118x*11.0*pow_u_10*18.0*pow_v_17
        + p_p->c1119x*11.0*pow_u_10*19.0*pow_v_18
        + p_p->c1120x*11.0*pow_u_10*20.0*pow_v_19
        + p_p->c1121x*11.0*pow_u_10*21.0*pow_v_20;
uv_x=uv_x + p_p->c1200x*12.0*pow_u_11*         0.0        
        + p_p->c1201x*12.0*pow_u_11* 1.0*pow_v_0
        + p_p->c1202x*12.0*pow_u_11* 2.0*pow_v_1
        + p_p->c1203x*12.0*pow_u_11* 3.0*pow_v_2
        + p_p->c1204x*12.0*pow_u_11* 4.0*pow_v_3
        + p_p->c1205x*12.0*pow_u_11* 5.0*pow_v_4
        + p_p->c1206x*12.0*pow_u_11* 6.0*pow_v_5
        + p_p->c1207x*12.0*pow_u_11* 7.0*pow_v_6
        + p_p->c1208x*12.0*pow_u_11* 8.0*pow_v_7
        + p_p->c1209x*12.0*pow_u_11* 9.0*pow_v_8
        + p_p->c1210x*12.0*pow_u_11*10.0*pow_v_9
        + p_p->c1211x*12.0*pow_u_11*11.0*pow_v_10
        + p_p->c1212x*12.0*pow_u_11*12.0*pow_v_11
        + p_p->c1213x*12.0*pow_u_11*13.0*pow_v_12
        + p_p->c1214x*12.0*pow_u_11*14.0*pow_v_13
        + p_p->c1215x*12.0*pow_u_11*15.0*pow_v_14
        + p_p->c1216x*12.0*pow_u_11*16.0*pow_v_15
        + p_p->c1217x*12.0*pow_u_11*17.0*pow_v_16
        + p_p->c1218x*12.0*pow_u_11*18.0*pow_v_17
        + p_p->c1219x*12.0*pow_u_11*19.0*pow_v_18
        + p_p->c1220x*12.0*pow_u_11*20.0*pow_v_19
        + p_p->c1221x*12.0*pow_u_11*21.0*pow_v_20;
uv_x=uv_x + p_p->c1300x*13.0*pow_u_12*         0.0        
        + p_p->c1301x*13.0*pow_u_12* 1.0*pow_v_0
        + p_p->c1302x*13.0*pow_u_12* 2.0*pow_v_1
        + p_p->c1303x*13.0*pow_u_12* 3.0*pow_v_2
        + p_p->c1304x*13.0*pow_u_12* 4.0*pow_v_3
        + p_p->c1305x*13.0*pow_u_12* 5.0*pow_v_4
        + p_p->c1306x*13.0*pow_u_12* 6.0*pow_v_5
        + p_p->c1307x*13.0*pow_u_12* 7.0*pow_v_6
        + p_p->c1308x*13.0*pow_u_12* 8.0*pow_v_7
        + p_p->c1309x*13.0*pow_u_12* 9.0*pow_v_8
        + p_p->c1310x*13.0*pow_u_12*10.0*pow_v_9
        + p_p->c1311x*13.0*pow_u_12*11.0*pow_v_10
        + p_p->c1312x*13.0*pow_u_12*12.0*pow_v_11
        + p_p->c1313x*13.0*pow_u_12*13.0*pow_v_12
        + p_p->c1314x*13.0*pow_u_12*14.0*pow_v_13
        + p_p->c1315x*13.0*pow_u_12*15.0*pow_v_14
        + p_p->c1316x*13.0*pow_u_12*16.0*pow_v_15
        + p_p->c1317x*13.0*pow_u_12*17.0*pow_v_16
        + p_p->c1318x*13.0*pow_u_12*18.0*pow_v_17
        + p_p->c1319x*13.0*pow_u_12*19.0*pow_v_18
        + p_p->c1320x*13.0*pow_u_12*20.0*pow_v_19
        + p_p->c1321x*13.0*pow_u_12*21.0*pow_v_20;
uv_x=uv_x + p_p->c1400x*14.0*pow_u_13*         0.0        
        + p_p->c1401x*14.0*pow_u_13* 1.0*pow_v_0
        + p_p->c1402x*14.0*pow_u_13* 2.0*pow_v_1
        + p_p->c1403x*14.0*pow_u_13* 3.0*pow_v_2
        + p_p->c1404x*14.0*pow_u_13* 4.0*pow_v_3
        + p_p->c1405x*14.0*pow_u_13* 5.0*pow_v_4
        + p_p->c1406x*14.0*pow_u_13* 6.0*pow_v_5
        + p_p->c1407x*14.0*pow_u_13* 7.0*pow_v_6
        + p_p->c1408x*14.0*pow_u_13* 8.0*pow_v_7
        + p_p->c1409x*14.0*pow_u_13* 9.0*pow_v_8
        + p_p->c1410x*14.0*pow_u_13*10.0*pow_v_9
        + p_p->c1411x*14.0*pow_u_13*11.0*pow_v_10
        + p_p->c1412x*14.0*pow_u_13*12.0*pow_v_11
        + p_p->c1413x*14.0*pow_u_13*13.0*pow_v_12
        + p_p->c1414x*14.0*pow_u_13*14.0*pow_v_13
        + p_p->c1415x*14.0*pow_u_13*15.0*pow_v_14
        + p_p->c1416x*14.0*pow_u_13*16.0*pow_v_15
        + p_p->c1417x*14.0*pow_u_13*17.0*pow_v_16
        + p_p->c1418x*14.0*pow_u_13*18.0*pow_v_17
        + p_p->c1419x*14.0*pow_u_13*19.0*pow_v_18
        + p_p->c1420x*14.0*pow_u_13*20.0*pow_v_19
        + p_p->c1421x*14.0*pow_u_13*21.0*pow_v_20;
uv_x=uv_x + p_p->c1500x*15.0*pow_u_14*         0.0        
        + p_p->c1501x*15.0*pow_u_14* 1.0*pow_v_0
        + p_p->c1502x*15.0*pow_u_14* 2.0*pow_v_1
        + p_p->c1503x*15.0*pow_u_14* 3.0*pow_v_2
        + p_p->c1504x*15.0*pow_u_14* 4.0*pow_v_3
        + p_p->c1505x*15.0*pow_u_14* 5.0*pow_v_4
        + p_p->c1506x*15.0*pow_u_14* 6.0*pow_v_5
        + p_p->c1507x*15.0*pow_u_14* 7.0*pow_v_6
        + p_p->c1508x*15.0*pow_u_14* 8.0*pow_v_7
        + p_p->c1509x*15.0*pow_u_14* 9.0*pow_v_8
        + p_p->c1510x*15.0*pow_u_14*10.0*pow_v_9
        + p_p->c1511x*15.0*pow_u_14*11.0*pow_v_10
        + p_p->c1512x*15.0*pow_u_14*12.0*pow_v_11
        + p_p->c1513x*15.0*pow_u_14*13.0*pow_v_12
        + p_p->c1514x*15.0*pow_u_14*14.0*pow_v_13
        + p_p->c1515x*15.0*pow_u_14*15.0*pow_v_14
        + p_p->c1516x*15.0*pow_u_14*16.0*pow_v_15
        + p_p->c1517x*15.0*pow_u_14*17.0*pow_v_16
        + p_p->c1518x*15.0*pow_u_14*18.0*pow_v_17
        + p_p->c1519x*15.0*pow_u_14*19.0*pow_v_18
        + p_p->c1520x*15.0*pow_u_14*20.0*pow_v_19
        + p_p->c1521x*15.0*pow_u_14*21.0*pow_v_20;
uv_x=uv_x + p_p->c1600x*16.0*pow_u_15*         0.0        
        + p_p->c1601x*16.0*pow_u_15* 1.0*pow_v_0
        + p_p->c1602x*16.0*pow_u_15* 2.0*pow_v_1
        + p_p->c1603x*16.0*pow_u_15* 3.0*pow_v_2
        + p_p->c1604x*16.0*pow_u_15* 4.0*pow_v_3
        + p_p->c1605x*16.0*pow_u_15* 5.0*pow_v_4
        + p_p->c1606x*16.0*pow_u_15* 6.0*pow_v_5
        + p_p->c1607x*16.0*pow_u_15* 7.0*pow_v_6
        + p_p->c1608x*16.0*pow_u_15* 8.0*pow_v_7
        + p_p->c1609x*16.0*pow_u_15* 9.0*pow_v_8
        + p_p->c1610x*16.0*pow_u_15*10.0*pow_v_9
        + p_p->c1611x*16.0*pow_u_15*11.0*pow_v_10
        + p_p->c1612x*16.0*pow_u_15*12.0*pow_v_11
        + p_p->c1613x*16.0*pow_u_15*13.0*pow_v_12
        + p_p->c1614x*16.0*pow_u_15*14.0*pow_v_13
        + p_p->c1615x*16.0*pow_u_15*15.0*pow_v_14
        + p_p->c1616x*16.0*pow_u_15*16.0*pow_v_15
        + p_p->c1617x*16.0*pow_u_15*17.0*pow_v_16
        + p_p->c1618x*16.0*pow_u_15*18.0*pow_v_17
        + p_p->c1619x*16.0*pow_u_15*19.0*pow_v_18
        + p_p->c1620x*16.0*pow_u_15*20.0*pow_v_19
        + p_p->c1621x*16.0*pow_u_15*21.0*pow_v_20;
uv_x=uv_x + p_p->c1700x*17.0*pow_u_16*         0.0        
        + p_p->c1701x*17.0*pow_u_16* 1.0*pow_v_0
        + p_p->c1702x*17.0*pow_u_16* 2.0*pow_v_1
        + p_p->c1703x*17.0*pow_u_16* 3.0*pow_v_2
        + p_p->c1704x*17.0*pow_u_16* 4.0*pow_v_3
        + p_p->c1705x*17.0*pow_u_16* 5.0*pow_v_4
        + p_p->c1706x*17.0*pow_u_16* 6.0*pow_v_5
        + p_p->c1707x*17.0*pow_u_16* 7.0*pow_v_6
        + p_p->c1708x*17.0*pow_u_16* 8.0*pow_v_7
        + p_p->c1709x*17.0*pow_u_16* 9.0*pow_v_8
        + p_p->c1710x*17.0*pow_u_16*10.0*pow_v_9
        + p_p->c1711x*17.0*pow_u_16*11.0*pow_v_10
        + p_p->c1712x*17.0*pow_u_16*12.0*pow_v_11
        + p_p->c1713x*17.0*pow_u_16*13.0*pow_v_12
        + p_p->c1714x*17.0*pow_u_16*14.0*pow_v_13
        + p_p->c1715x*17.0*pow_u_16*15.0*pow_v_14
        + p_p->c1716x*17.0*pow_u_16*16.0*pow_v_15
        + p_p->c1717x*17.0*pow_u_16*17.0*pow_v_16
        + p_p->c1718x*17.0*pow_u_16*18.0*pow_v_17
        + p_p->c1719x*17.0*pow_u_16*19.0*pow_v_18
        + p_p->c1720x*17.0*pow_u_16*20.0*pow_v_19
        + p_p->c1721x*17.0*pow_u_16*21.0*pow_v_20;
uv_x=uv_x + p_p->c1800x*18.0*pow_u_17*         0.0        
        + p_p->c1801x*18.0*pow_u_17* 1.0*pow_v_0
        + p_p->c1802x*18.0*pow_u_17* 2.0*pow_v_1
        + p_p->c1803x*18.0*pow_u_17* 3.0*pow_v_2
        + p_p->c1804x*18.0*pow_u_17* 4.0*pow_v_3
        + p_p->c1805x*18.0*pow_u_17* 5.0*pow_v_4
        + p_p->c1806x*18.0*pow_u_17* 6.0*pow_v_5
        + p_p->c1807x*18.0*pow_u_17* 7.0*pow_v_6
        + p_p->c1808x*18.0*pow_u_17* 8.0*pow_v_7
        + p_p->c1809x*18.0*pow_u_17* 9.0*pow_v_8
        + p_p->c1810x*18.0*pow_u_17*10.0*pow_v_9
        + p_p->c1811x*18.0*pow_u_17*11.0*pow_v_10
        + p_p->c1812x*18.0*pow_u_17*12.0*pow_v_11
        + p_p->c1813x*18.0*pow_u_17*13.0*pow_v_12
        + p_p->c1814x*18.0*pow_u_17*14.0*pow_v_13
        + p_p->c1815x*18.0*pow_u_17*15.0*pow_v_14
        + p_p->c1816x*18.0*pow_u_17*16.0*pow_v_15
        + p_p->c1817x*18.0*pow_u_17*17.0*pow_v_16
        + p_p->c1818x*18.0*pow_u_17*18.0*pow_v_17
        + p_p->c1819x*18.0*pow_u_17*19.0*pow_v_18
        + p_p->c1820x*18.0*pow_u_17*20.0*pow_v_19
        + p_p->c1821x*18.0*pow_u_17*21.0*pow_v_20;
uv_x=uv_x + p_p->c1900x*19.0*pow_u_18*         0.0        
        + p_p->c1901x*19.0*pow_u_18* 1.0*pow_v_0
        + p_p->c1902x*19.0*pow_u_18* 2.0*pow_v_1
        + p_p->c1903x*19.0*pow_u_18* 3.0*pow_v_2
        + p_p->c1904x*19.0*pow_u_18* 4.0*pow_v_3
        + p_p->c1905x*19.0*pow_u_18* 5.0*pow_v_4
        + p_p->c1906x*19.0*pow_u_18* 6.0*pow_v_5
        + p_p->c1907x*19.0*pow_u_18* 7.0*pow_v_6
        + p_p->c1908x*19.0*pow_u_18* 8.0*pow_v_7
        + p_p->c1909x*19.0*pow_u_18* 9.0*pow_v_8
        + p_p->c1910x*19.0*pow_u_18*10.0*pow_v_9
        + p_p->c1911x*19.0*pow_u_18*11.0*pow_v_10
        + p_p->c1912x*19.0*pow_u_18*12.0*pow_v_11
        + p_p->c1913x*19.0*pow_u_18*13.0*pow_v_12
        + p_p->c1914x*19.0*pow_u_18*14.0*pow_v_13
        + p_p->c1915x*19.0*pow_u_18*15.0*pow_v_14
        + p_p->c1916x*19.0*pow_u_18*16.0*pow_v_15
        + p_p->c1917x*19.0*pow_u_18*17.0*pow_v_16
        + p_p->c1918x*19.0*pow_u_18*18.0*pow_v_17
        + p_p->c1919x*19.0*pow_u_18*19.0*pow_v_18
        + p_p->c1920x*19.0*pow_u_18*20.0*pow_v_19
        + p_p->c1921x*19.0*pow_u_18*21.0*pow_v_20;
uv_x=uv_x + p_p->c2000x*20.0*pow_u_19*         0.0        
        + p_p->c2001x*20.0*pow_u_19* 1.0*pow_v_0
        + p_p->c2002x*20.0*pow_u_19* 2.0*pow_v_1
        + p_p->c2003x*20.0*pow_u_19* 3.0*pow_v_2
        + p_p->c2004x*20.0*pow_u_19* 4.0*pow_v_3
        + p_p->c2005x*20.0*pow_u_19* 5.0*pow_v_4
        + p_p->c2006x*20.0*pow_u_19* 6.0*pow_v_5
        + p_p->c2007x*20.0*pow_u_19* 7.0*pow_v_6
        + p_p->c2008x*20.0*pow_u_19* 8.0*pow_v_7
        + p_p->c2009x*20.0*pow_u_19* 9.0*pow_v_8
        + p_p->c2010x*20.0*pow_u_19*10.0*pow_v_9
        + p_p->c2011x*20.0*pow_u_19*11.0*pow_v_10
        + p_p->c2012x*20.0*pow_u_19*12.0*pow_v_11
        + p_p->c2013x*20.0*pow_u_19*13.0*pow_v_12
        + p_p->c2014x*20.0*pow_u_19*14.0*pow_v_13
        + p_p->c2015x*20.0*pow_u_19*15.0*pow_v_14
        + p_p->c2016x*20.0*pow_u_19*16.0*pow_v_15
        + p_p->c2017x*20.0*pow_u_19*17.0*pow_v_16
        + p_p->c2018x*20.0*pow_u_19*18.0*pow_v_17
        + p_p->c2019x*20.0*pow_u_19*19.0*pow_v_18
        + p_p->c2020x*20.0*pow_u_19*20.0*pow_v_19
        + p_p->c2021x*20.0*pow_u_19*21.0*pow_v_20;
uv_x=uv_x + p_p->c2100x*21.0*pow_u_20*         0.0        
        + p_p->c2101x*21.0*pow_u_20* 1.0*pow_v_0
        + p_p->c2102x*21.0*pow_u_20* 2.0*pow_v_1
        + p_p->c2103x*21.0*pow_u_20* 3.0*pow_v_2
        + p_p->c2104x*21.0*pow_u_20* 4.0*pow_v_3
        + p_p->c2105x*21.0*pow_u_20* 5.0*pow_v_4
        + p_p->c2106x*21.0*pow_u_20* 6.0*pow_v_5
        + p_p->c2107x*21.0*pow_u_20* 7.0*pow_v_6
        + p_p->c2108x*21.0*pow_u_20* 8.0*pow_v_7
        + p_p->c2109x*21.0*pow_u_20* 9.0*pow_v_8
        + p_p->c2110x*21.0*pow_u_20*10.0*pow_v_9
        + p_p->c2111x*21.0*pow_u_20*11.0*pow_v_10
        + p_p->c2112x*21.0*pow_u_20*12.0*pow_v_11
        + p_p->c2113x*21.0*pow_u_20*13.0*pow_v_12
        + p_p->c2114x*21.0*pow_u_20*14.0*pow_v_13
        + p_p->c2115x*21.0*pow_u_20*15.0*pow_v_14
        + p_p->c2116x*21.0*pow_u_20*16.0*pow_v_15
        + p_p->c2117x*21.0*pow_u_20*17.0*pow_v_16
        + p_p->c2118x*21.0*pow_u_20*18.0*pow_v_17
        + p_p->c2119x*21.0*pow_u_20*19.0*pow_v_18
        + p_p->c2120x*21.0*pow_u_20*20.0*pow_v_19
        + p_p->c2121x*21.0*pow_u_20*21.0*pow_v_20;

uv_y    = p_p->c0000y*         0.0        *         0.0        
        + p_p->c0001y*         0.0        * 1.0*pow_v_0
        + p_p->c0002y*         0.0        * 2.0*pow_v_1
        + p_p->c0003y*         0.0        * 3.0*pow_v_2
        + p_p->c0004y*         0.0        * 4.0*pow_v_3
        + p_p->c0005y*         0.0        * 5.0*pow_v_4
        + p_p->c0006y*         0.0        * 6.0*pow_v_5
        + p_p->c0007y*         0.0        * 7.0*pow_v_6
        + p_p->c0008y*         0.0        * 8.0*pow_v_7
        + p_p->c0009y*         0.0        * 9.0*pow_v_8
        + p_p->c0010y*         0.0        *10.0*pow_v_9
        + p_p->c0011y*         0.0        *11.0*pow_v_10
        + p_p->c0012y*         0.0        *12.0*pow_v_11
        + p_p->c0013y*         0.0        *13.0*pow_v_12
        + p_p->c0014y*         0.0        *14.0*pow_v_13
        + p_p->c0015y*         0.0        *15.0*pow_v_14
        + p_p->c0016y*         0.0        *16.0*pow_v_15
        + p_p->c0017y*         0.0        *17.0*pow_v_16
        + p_p->c0018y*         0.0        *18.0*pow_v_17
        + p_p->c0019y*         0.0        *19.0*pow_v_18
        + p_p->c0020y*         0.0        *20.0*pow_v_19
        + p_p->c0021y*         0.0        *21.0*pow_v_20;
uv_y=uv_y + p_p->c0100y* 1.0*pow_u_0*         0.0        
        + p_p->c0101y* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102y* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103y* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104y* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105y* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106y* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107y* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0108y* 1.0*pow_u_0* 8.0*pow_v_7
        + p_p->c0109y* 1.0*pow_u_0* 9.0*pow_v_8
        + p_p->c0110y* 1.0*pow_u_0*10.0*pow_v_9
        + p_p->c0111y* 1.0*pow_u_0*11.0*pow_v_10
        + p_p->c0112y* 1.0*pow_u_0*12.0*pow_v_11
        + p_p->c0113y* 1.0*pow_u_0*13.0*pow_v_12
        + p_p->c0114y* 1.0*pow_u_0*14.0*pow_v_13
        + p_p->c0115y* 1.0*pow_u_0*15.0*pow_v_14
        + p_p->c0116y* 1.0*pow_u_0*16.0*pow_v_15
        + p_p->c0117y* 1.0*pow_u_0*17.0*pow_v_16
        + p_p->c0118y* 1.0*pow_u_0*18.0*pow_v_17
        + p_p->c0119y* 1.0*pow_u_0*19.0*pow_v_18
        + p_p->c0120y* 1.0*pow_u_0*20.0*pow_v_19
        + p_p->c0121y* 1.0*pow_u_0*21.0*pow_v_20;
uv_y=uv_y + p_p->c0200y* 2.0*pow_u_1*         0.0        
        + p_p->c0201y* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202y* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203y* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204y* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205y* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206y* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207y* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0208y* 2.0*pow_u_1* 8.0*pow_v_7
        + p_p->c0209y* 2.0*pow_u_1* 9.0*pow_v_8
        + p_p->c0210y* 2.0*pow_u_1*10.0*pow_v_9
        + p_p->c0211y* 2.0*pow_u_1*11.0*pow_v_10
        + p_p->c0212y* 2.0*pow_u_1*12.0*pow_v_11
        + p_p->c0213y* 2.0*pow_u_1*13.0*pow_v_12
        + p_p->c0214y* 2.0*pow_u_1*14.0*pow_v_13
        + p_p->c0215y* 2.0*pow_u_1*15.0*pow_v_14
        + p_p->c0216y* 2.0*pow_u_1*16.0*pow_v_15
        + p_p->c0217y* 2.0*pow_u_1*17.0*pow_v_16
        + p_p->c0218y* 2.0*pow_u_1*18.0*pow_v_17
        + p_p->c0219y* 2.0*pow_u_1*19.0*pow_v_18
        + p_p->c0220y* 2.0*pow_u_1*20.0*pow_v_19
        + p_p->c0221y* 2.0*pow_u_1*21.0*pow_v_20;
uv_y=uv_y + p_p->c0300y* 3.0*pow_u_2*         0.0        
        + p_p->c0301y* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302y* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303y* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304y* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305y* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306y* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307y* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0308y* 3.0*pow_u_2* 8.0*pow_v_7
        + p_p->c0309y* 3.0*pow_u_2* 9.0*pow_v_8
        + p_p->c0310y* 3.0*pow_u_2*10.0*pow_v_9
        + p_p->c0311y* 3.0*pow_u_2*11.0*pow_v_10
        + p_p->c0312y* 3.0*pow_u_2*12.0*pow_v_11
        + p_p->c0313y* 3.0*pow_u_2*13.0*pow_v_12
        + p_p->c0314y* 3.0*pow_u_2*14.0*pow_v_13
        + p_p->c0315y* 3.0*pow_u_2*15.0*pow_v_14
        + p_p->c0316y* 3.0*pow_u_2*16.0*pow_v_15
        + p_p->c0317y* 3.0*pow_u_2*17.0*pow_v_16
        + p_p->c0318y* 3.0*pow_u_2*18.0*pow_v_17
        + p_p->c0319y* 3.0*pow_u_2*19.0*pow_v_18
        + p_p->c0320y* 3.0*pow_u_2*20.0*pow_v_19
        + p_p->c0321y* 3.0*pow_u_2*21.0*pow_v_20;
uv_y=uv_y + p_p->c0400y* 4.0*pow_u_3*         0.0        
        + p_p->c0401y* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402y* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403y* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404y* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405y* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406y* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407y* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0408y* 4.0*pow_u_3* 8.0*pow_v_7
        + p_p->c0409y* 4.0*pow_u_3* 9.0*pow_v_8
        + p_p->c0410y* 4.0*pow_u_3*10.0*pow_v_9
        + p_p->c0411y* 4.0*pow_u_3*11.0*pow_v_10
        + p_p->c0412y* 4.0*pow_u_3*12.0*pow_v_11
        + p_p->c0413y* 4.0*pow_u_3*13.0*pow_v_12
        + p_p->c0414y* 4.0*pow_u_3*14.0*pow_v_13
        + p_p->c0415y* 4.0*pow_u_3*15.0*pow_v_14
        + p_p->c0416y* 4.0*pow_u_3*16.0*pow_v_15
        + p_p->c0417y* 4.0*pow_u_3*17.0*pow_v_16
        + p_p->c0418y* 4.0*pow_u_3*18.0*pow_v_17
        + p_p->c0419y* 4.0*pow_u_3*19.0*pow_v_18
        + p_p->c0420y* 4.0*pow_u_3*20.0*pow_v_19
        + p_p->c0421y* 4.0*pow_u_3*21.0*pow_v_20;
uv_y=uv_y + p_p->c0500y* 5.0*pow_u_4*         0.0        
        + p_p->c0501y* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502y* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503y* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504y* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505y* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506y* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507y* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0508y* 5.0*pow_u_4* 8.0*pow_v_7
        + p_p->c0509y* 5.0*pow_u_4* 9.0*pow_v_8
        + p_p->c0510y* 5.0*pow_u_4*10.0*pow_v_9
        + p_p->c0511y* 5.0*pow_u_4*11.0*pow_v_10
        + p_p->c0512y* 5.0*pow_u_4*12.0*pow_v_11
        + p_p->c0513y* 5.0*pow_u_4*13.0*pow_v_12
        + p_p->c0514y* 5.0*pow_u_4*14.0*pow_v_13
        + p_p->c0515y* 5.0*pow_u_4*15.0*pow_v_14
        + p_p->c0516y* 5.0*pow_u_4*16.0*pow_v_15
        + p_p->c0517y* 5.0*pow_u_4*17.0*pow_v_16
        + p_p->c0518y* 5.0*pow_u_4*18.0*pow_v_17
        + p_p->c0519y* 5.0*pow_u_4*19.0*pow_v_18
        + p_p->c0520y* 5.0*pow_u_4*20.0*pow_v_19
        + p_p->c0521y* 5.0*pow_u_4*21.0*pow_v_20;
uv_y=uv_y + p_p->c0600y* 6.0*pow_u_5*         0.0        
        + p_p->c0601y* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602y* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603y* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604y* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605y* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606y* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607y* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0608y* 6.0*pow_u_5* 8.0*pow_v_7
        + p_p->c0609y* 6.0*pow_u_5* 9.0*pow_v_8
        + p_p->c0610y* 6.0*pow_u_5*10.0*pow_v_9
        + p_p->c0611y* 6.0*pow_u_5*11.0*pow_v_10
        + p_p->c0612y* 6.0*pow_u_5*12.0*pow_v_11
        + p_p->c0613y* 6.0*pow_u_5*13.0*pow_v_12
        + p_p->c0614y* 6.0*pow_u_5*14.0*pow_v_13
        + p_p->c0615y* 6.0*pow_u_5*15.0*pow_v_14
        + p_p->c0616y* 6.0*pow_u_5*16.0*pow_v_15
        + p_p->c0617y* 6.0*pow_u_5*17.0*pow_v_16
        + p_p->c0618y* 6.0*pow_u_5*18.0*pow_v_17
        + p_p->c0619y* 6.0*pow_u_5*19.0*pow_v_18
        + p_p->c0620y* 6.0*pow_u_5*20.0*pow_v_19
        + p_p->c0621y* 6.0*pow_u_5*21.0*pow_v_20;
uv_y=uv_y + p_p->c0700y* 7.0*pow_u_6*         0.0        
        + p_p->c0701y* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702y* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703y* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704y* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705y* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706y* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707y* 7.0*pow_u_6* 7.0*pow_v_6
        + p_p->c0708y* 7.0*pow_u_6* 8.0*pow_v_7
        + p_p->c0709y* 7.0*pow_u_6* 9.0*pow_v_8
        + p_p->c0710y* 7.0*pow_u_6*10.0*pow_v_9
        + p_p->c0711y* 7.0*pow_u_6*11.0*pow_v_10
        + p_p->c0712y* 7.0*pow_u_6*12.0*pow_v_11
        + p_p->c0713y* 7.0*pow_u_6*13.0*pow_v_12
        + p_p->c0714y* 7.0*pow_u_6*14.0*pow_v_13
        + p_p->c0715y* 7.0*pow_u_6*15.0*pow_v_14
        + p_p->c0716y* 7.0*pow_u_6*16.0*pow_v_15
        + p_p->c0717y* 7.0*pow_u_6*17.0*pow_v_16
        + p_p->c0718y* 7.0*pow_u_6*18.0*pow_v_17
        + p_p->c0719y* 7.0*pow_u_6*19.0*pow_v_18
        + p_p->c0720y* 7.0*pow_u_6*20.0*pow_v_19
        + p_p->c0721y* 7.0*pow_u_6*21.0*pow_v_20;
uv_y=uv_y + p_p->c0800y* 8.0*pow_u_7*         0.0        
        + p_p->c0801y* 8.0*pow_u_7* 1.0*pow_v_0
        + p_p->c0802y* 8.0*pow_u_7* 2.0*pow_v_1
        + p_p->c0803y* 8.0*pow_u_7* 3.0*pow_v_2
        + p_p->c0804y* 8.0*pow_u_7* 4.0*pow_v_3
        + p_p->c0805y* 8.0*pow_u_7* 5.0*pow_v_4
        + p_p->c0806y* 8.0*pow_u_7* 6.0*pow_v_5
        + p_p->c0807y* 8.0*pow_u_7* 7.0*pow_v_6
        + p_p->c0808y* 8.0*pow_u_7* 8.0*pow_v_7
        + p_p->c0809y* 8.0*pow_u_7* 9.0*pow_v_8
        + p_p->c0810y* 8.0*pow_u_7*10.0*pow_v_9
        + p_p->c0811y* 8.0*pow_u_7*11.0*pow_v_10
        + p_p->c0812y* 8.0*pow_u_7*12.0*pow_v_11
        + p_p->c0813y* 8.0*pow_u_7*13.0*pow_v_12
        + p_p->c0814y* 8.0*pow_u_7*14.0*pow_v_13
        + p_p->c0815y* 8.0*pow_u_7*15.0*pow_v_14
        + p_p->c0816y* 8.0*pow_u_7*16.0*pow_v_15
        + p_p->c0817y* 8.0*pow_u_7*17.0*pow_v_16
        + p_p->c0818y* 8.0*pow_u_7*18.0*pow_v_17
        + p_p->c0819y* 8.0*pow_u_7*19.0*pow_v_18
        + p_p->c0820y* 8.0*pow_u_7*20.0*pow_v_19
        + p_p->c0821y* 8.0*pow_u_7*21.0*pow_v_20;
uv_y=uv_y + p_p->c0900y* 9.0*pow_u_8*         0.0        
        + p_p->c0901y* 9.0*pow_u_8* 1.0*pow_v_0
        + p_p->c0902y* 9.0*pow_u_8* 2.0*pow_v_1
        + p_p->c0903y* 9.0*pow_u_8* 3.0*pow_v_2
        + p_p->c0904y* 9.0*pow_u_8* 4.0*pow_v_3
        + p_p->c0905y* 9.0*pow_u_8* 5.0*pow_v_4
        + p_p->c0906y* 9.0*pow_u_8* 6.0*pow_v_5
        + p_p->c0907y* 9.0*pow_u_8* 7.0*pow_v_6
        + p_p->c0908y* 9.0*pow_u_8* 8.0*pow_v_7
        + p_p->c0909y* 9.0*pow_u_8* 9.0*pow_v_8
        + p_p->c0910y* 9.0*pow_u_8*10.0*pow_v_9
        + p_p->c0911y* 9.0*pow_u_8*11.0*pow_v_10
        + p_p->c0912y* 9.0*pow_u_8*12.0*pow_v_11
        + p_p->c0913y* 9.0*pow_u_8*13.0*pow_v_12
        + p_p->c0914y* 9.0*pow_u_8*14.0*pow_v_13
        + p_p->c0915y* 9.0*pow_u_8*15.0*pow_v_14
        + p_p->c0916y* 9.0*pow_u_8*16.0*pow_v_15
        + p_p->c0917y* 9.0*pow_u_8*17.0*pow_v_16
        + p_p->c0918y* 9.0*pow_u_8*18.0*pow_v_17
        + p_p->c0919y* 9.0*pow_u_8*19.0*pow_v_18
        + p_p->c0920y* 9.0*pow_u_8*20.0*pow_v_19
        + p_p->c0921y* 9.0*pow_u_8*21.0*pow_v_20;
uv_y=uv_y + p_p->c1000y*10.0*pow_u_9*         0.0        
        + p_p->c1001y*10.0*pow_u_9* 1.0*pow_v_0
        + p_p->c1002y*10.0*pow_u_9* 2.0*pow_v_1
        + p_p->c1003y*10.0*pow_u_9* 3.0*pow_v_2
        + p_p->c1004y*10.0*pow_u_9* 4.0*pow_v_3
        + p_p->c1005y*10.0*pow_u_9* 5.0*pow_v_4
        + p_p->c1006y*10.0*pow_u_9* 6.0*pow_v_5
        + p_p->c1007y*10.0*pow_u_9* 7.0*pow_v_6
        + p_p->c1008y*10.0*pow_u_9* 8.0*pow_v_7
        + p_p->c1009y*10.0*pow_u_9* 9.0*pow_v_8
        + p_p->c1010y*10.0*pow_u_9*10.0*pow_v_9
        + p_p->c1011y*10.0*pow_u_9*11.0*pow_v_10
        + p_p->c1012y*10.0*pow_u_9*12.0*pow_v_11
        + p_p->c1013y*10.0*pow_u_9*13.0*pow_v_12
        + p_p->c1014y*10.0*pow_u_9*14.0*pow_v_13
        + p_p->c1015y*10.0*pow_u_9*15.0*pow_v_14
        + p_p->c1016y*10.0*pow_u_9*16.0*pow_v_15
        + p_p->c1017y*10.0*pow_u_9*17.0*pow_v_16
        + p_p->c1018y*10.0*pow_u_9*18.0*pow_v_17
        + p_p->c1019y*10.0*pow_u_9*19.0*pow_v_18
        + p_p->c1020y*10.0*pow_u_9*20.0*pow_v_19
        + p_p->c1021y*10.0*pow_u_9*21.0*pow_v_20;
uv_y=uv_y + p_p->c1100y*11.0*pow_u_10*         0.0        
        + p_p->c1101y*11.0*pow_u_10* 1.0*pow_v_0
        + p_p->c1102y*11.0*pow_u_10* 2.0*pow_v_1
        + p_p->c1103y*11.0*pow_u_10* 3.0*pow_v_2
        + p_p->c1104y*11.0*pow_u_10* 4.0*pow_v_3
        + p_p->c1105y*11.0*pow_u_10* 5.0*pow_v_4
        + p_p->c1106y*11.0*pow_u_10* 6.0*pow_v_5
        + p_p->c1107y*11.0*pow_u_10* 7.0*pow_v_6
        + p_p->c1108y*11.0*pow_u_10* 8.0*pow_v_7
        + p_p->c1109y*11.0*pow_u_10* 9.0*pow_v_8
        + p_p->c1110y*11.0*pow_u_10*10.0*pow_v_9
        + p_p->c1111y*11.0*pow_u_10*11.0*pow_v_10
        + p_p->c1112y*11.0*pow_u_10*12.0*pow_v_11
        + p_p->c1113y*11.0*pow_u_10*13.0*pow_v_12
        + p_p->c1114y*11.0*pow_u_10*14.0*pow_v_13
        + p_p->c1115y*11.0*pow_u_10*15.0*pow_v_14
        + p_p->c1116y*11.0*pow_u_10*16.0*pow_v_15
        + p_p->c1117y*11.0*pow_u_10*17.0*pow_v_16
        + p_p->c1118y*11.0*pow_u_10*18.0*pow_v_17
        + p_p->c1119y*11.0*pow_u_10*19.0*pow_v_18
        + p_p->c1120y*11.0*pow_u_10*20.0*pow_v_19
        + p_p->c1121y*11.0*pow_u_10*21.0*pow_v_20;
uv_y=uv_y + p_p->c1200y*12.0*pow_u_11*         0.0        
        + p_p->c1201y*12.0*pow_u_11* 1.0*pow_v_0
        + p_p->c1202y*12.0*pow_u_11* 2.0*pow_v_1
        + p_p->c1203y*12.0*pow_u_11* 3.0*pow_v_2
        + p_p->c1204y*12.0*pow_u_11* 4.0*pow_v_3
        + p_p->c1205y*12.0*pow_u_11* 5.0*pow_v_4
        + p_p->c1206y*12.0*pow_u_11* 6.0*pow_v_5
        + p_p->c1207y*12.0*pow_u_11* 7.0*pow_v_6
        + p_p->c1208y*12.0*pow_u_11* 8.0*pow_v_7
        + p_p->c1209y*12.0*pow_u_11* 9.0*pow_v_8
        + p_p->c1210y*12.0*pow_u_11*10.0*pow_v_9
        + p_p->c1211y*12.0*pow_u_11*11.0*pow_v_10
        + p_p->c1212y*12.0*pow_u_11*12.0*pow_v_11
        + p_p->c1213y*12.0*pow_u_11*13.0*pow_v_12
        + p_p->c1214y*12.0*pow_u_11*14.0*pow_v_13
        + p_p->c1215y*12.0*pow_u_11*15.0*pow_v_14
        + p_p->c1216y*12.0*pow_u_11*16.0*pow_v_15
        + p_p->c1217y*12.0*pow_u_11*17.0*pow_v_16
        + p_p->c1218y*12.0*pow_u_11*18.0*pow_v_17
        + p_p->c1219y*12.0*pow_u_11*19.0*pow_v_18
        + p_p->c1220y*12.0*pow_u_11*20.0*pow_v_19
        + p_p->c1221y*12.0*pow_u_11*21.0*pow_v_20;
uv_y=uv_y + p_p->c1300y*13.0*pow_u_12*         0.0        
        + p_p->c1301y*13.0*pow_u_12* 1.0*pow_v_0
        + p_p->c1302y*13.0*pow_u_12* 2.0*pow_v_1
        + p_p->c1303y*13.0*pow_u_12* 3.0*pow_v_2
        + p_p->c1304y*13.0*pow_u_12* 4.0*pow_v_3
        + p_p->c1305y*13.0*pow_u_12* 5.0*pow_v_4
        + p_p->c1306y*13.0*pow_u_12* 6.0*pow_v_5
        + p_p->c1307y*13.0*pow_u_12* 7.0*pow_v_6
        + p_p->c1308y*13.0*pow_u_12* 8.0*pow_v_7
        + p_p->c1309y*13.0*pow_u_12* 9.0*pow_v_8
        + p_p->c1310y*13.0*pow_u_12*10.0*pow_v_9
        + p_p->c1311y*13.0*pow_u_12*11.0*pow_v_10
        + p_p->c1312y*13.0*pow_u_12*12.0*pow_v_11
        + p_p->c1313y*13.0*pow_u_12*13.0*pow_v_12
        + p_p->c1314y*13.0*pow_u_12*14.0*pow_v_13
        + p_p->c1315y*13.0*pow_u_12*15.0*pow_v_14
        + p_p->c1316y*13.0*pow_u_12*16.0*pow_v_15
        + p_p->c1317y*13.0*pow_u_12*17.0*pow_v_16
        + p_p->c1318y*13.0*pow_u_12*18.0*pow_v_17
        + p_p->c1319y*13.0*pow_u_12*19.0*pow_v_18
        + p_p->c1320y*13.0*pow_u_12*20.0*pow_v_19
        + p_p->c1321y*13.0*pow_u_12*21.0*pow_v_20;
uv_y=uv_y + p_p->c1400y*14.0*pow_u_13*         0.0        
        + p_p->c1401y*14.0*pow_u_13* 1.0*pow_v_0
        + p_p->c1402y*14.0*pow_u_13* 2.0*pow_v_1
        + p_p->c1403y*14.0*pow_u_13* 3.0*pow_v_2
        + p_p->c1404y*14.0*pow_u_13* 4.0*pow_v_3
        + p_p->c1405y*14.0*pow_u_13* 5.0*pow_v_4
        + p_p->c1406y*14.0*pow_u_13* 6.0*pow_v_5
        + p_p->c1407y*14.0*pow_u_13* 7.0*pow_v_6
        + p_p->c1408y*14.0*pow_u_13* 8.0*pow_v_7
        + p_p->c1409y*14.0*pow_u_13* 9.0*pow_v_8
        + p_p->c1410y*14.0*pow_u_13*10.0*pow_v_9
        + p_p->c1411y*14.0*pow_u_13*11.0*pow_v_10
        + p_p->c1412y*14.0*pow_u_13*12.0*pow_v_11
        + p_p->c1413y*14.0*pow_u_13*13.0*pow_v_12
        + p_p->c1414y*14.0*pow_u_13*14.0*pow_v_13
        + p_p->c1415y*14.0*pow_u_13*15.0*pow_v_14
        + p_p->c1416y*14.0*pow_u_13*16.0*pow_v_15
        + p_p->c1417y*14.0*pow_u_13*17.0*pow_v_16
        + p_p->c1418y*14.0*pow_u_13*18.0*pow_v_17
        + p_p->c1419y*14.0*pow_u_13*19.0*pow_v_18
        + p_p->c1420y*14.0*pow_u_13*20.0*pow_v_19
        + p_p->c1421y*14.0*pow_u_13*21.0*pow_v_20;
uv_y=uv_y + p_p->c1500y*15.0*pow_u_14*         0.0        
        + p_p->c1501y*15.0*pow_u_14* 1.0*pow_v_0
        + p_p->c1502y*15.0*pow_u_14* 2.0*pow_v_1
        + p_p->c1503y*15.0*pow_u_14* 3.0*pow_v_2
        + p_p->c1504y*15.0*pow_u_14* 4.0*pow_v_3
        + p_p->c1505y*15.0*pow_u_14* 5.0*pow_v_4
        + p_p->c1506y*15.0*pow_u_14* 6.0*pow_v_5
        + p_p->c1507y*15.0*pow_u_14* 7.0*pow_v_6
        + p_p->c1508y*15.0*pow_u_14* 8.0*pow_v_7
        + p_p->c1509y*15.0*pow_u_14* 9.0*pow_v_8
        + p_p->c1510y*15.0*pow_u_14*10.0*pow_v_9
        + p_p->c1511y*15.0*pow_u_14*11.0*pow_v_10
        + p_p->c1512y*15.0*pow_u_14*12.0*pow_v_11
        + p_p->c1513y*15.0*pow_u_14*13.0*pow_v_12
        + p_p->c1514y*15.0*pow_u_14*14.0*pow_v_13
        + p_p->c1515y*15.0*pow_u_14*15.0*pow_v_14
        + p_p->c1516y*15.0*pow_u_14*16.0*pow_v_15
        + p_p->c1517y*15.0*pow_u_14*17.0*pow_v_16
        + p_p->c1518y*15.0*pow_u_14*18.0*pow_v_17
        + p_p->c1519y*15.0*pow_u_14*19.0*pow_v_18
        + p_p->c1520y*15.0*pow_u_14*20.0*pow_v_19
        + p_p->c1521y*15.0*pow_u_14*21.0*pow_v_20;
uv_y=uv_y + p_p->c1600y*16.0*pow_u_15*         0.0        
        + p_p->c1601y*16.0*pow_u_15* 1.0*pow_v_0
        + p_p->c1602y*16.0*pow_u_15* 2.0*pow_v_1
        + p_p->c1603y*16.0*pow_u_15* 3.0*pow_v_2
        + p_p->c1604y*16.0*pow_u_15* 4.0*pow_v_3
        + p_p->c1605y*16.0*pow_u_15* 5.0*pow_v_4
        + p_p->c1606y*16.0*pow_u_15* 6.0*pow_v_5
        + p_p->c1607y*16.0*pow_u_15* 7.0*pow_v_6
        + p_p->c1608y*16.0*pow_u_15* 8.0*pow_v_7
        + p_p->c1609y*16.0*pow_u_15* 9.0*pow_v_8
        + p_p->c1610y*16.0*pow_u_15*10.0*pow_v_9
        + p_p->c1611y*16.0*pow_u_15*11.0*pow_v_10
        + p_p->c1612y*16.0*pow_u_15*12.0*pow_v_11
        + p_p->c1613y*16.0*pow_u_15*13.0*pow_v_12
        + p_p->c1614y*16.0*pow_u_15*14.0*pow_v_13
        + p_p->c1615y*16.0*pow_u_15*15.0*pow_v_14
        + p_p->c1616y*16.0*pow_u_15*16.0*pow_v_15
        + p_p->c1617y*16.0*pow_u_15*17.0*pow_v_16
        + p_p->c1618y*16.0*pow_u_15*18.0*pow_v_17
        + p_p->c1619y*16.0*pow_u_15*19.0*pow_v_18
        + p_p->c1620y*16.0*pow_u_15*20.0*pow_v_19
        + p_p->c1621y*16.0*pow_u_15*21.0*pow_v_20;
uv_y=uv_y + p_p->c1700y*17.0*pow_u_16*         0.0        
        + p_p->c1701y*17.0*pow_u_16* 1.0*pow_v_0
        + p_p->c1702y*17.0*pow_u_16* 2.0*pow_v_1
        + p_p->c1703y*17.0*pow_u_16* 3.0*pow_v_2
        + p_p->c1704y*17.0*pow_u_16* 4.0*pow_v_3
        + p_p->c1705y*17.0*pow_u_16* 5.0*pow_v_4
        + p_p->c1706y*17.0*pow_u_16* 6.0*pow_v_5
        + p_p->c1707y*17.0*pow_u_16* 7.0*pow_v_6
        + p_p->c1708y*17.0*pow_u_16* 8.0*pow_v_7
        + p_p->c1709y*17.0*pow_u_16* 9.0*pow_v_8
        + p_p->c1710y*17.0*pow_u_16*10.0*pow_v_9
        + p_p->c1711y*17.0*pow_u_16*11.0*pow_v_10
        + p_p->c1712y*17.0*pow_u_16*12.0*pow_v_11
        + p_p->c1713y*17.0*pow_u_16*13.0*pow_v_12
        + p_p->c1714y*17.0*pow_u_16*14.0*pow_v_13
        + p_p->c1715y*17.0*pow_u_16*15.0*pow_v_14
        + p_p->c1716y*17.0*pow_u_16*16.0*pow_v_15
        + p_p->c1717y*17.0*pow_u_16*17.0*pow_v_16
        + p_p->c1718y*17.0*pow_u_16*18.0*pow_v_17
        + p_p->c1719y*17.0*pow_u_16*19.0*pow_v_18
        + p_p->c1720y*17.0*pow_u_16*20.0*pow_v_19
        + p_p->c1721y*17.0*pow_u_16*21.0*pow_v_20;
uv_y=uv_y + p_p->c1800y*18.0*pow_u_17*         0.0        
        + p_p->c1801y*18.0*pow_u_17* 1.0*pow_v_0
        + p_p->c1802y*18.0*pow_u_17* 2.0*pow_v_1
        + p_p->c1803y*18.0*pow_u_17* 3.0*pow_v_2
        + p_p->c1804y*18.0*pow_u_17* 4.0*pow_v_3
        + p_p->c1805y*18.0*pow_u_17* 5.0*pow_v_4
        + p_p->c1806y*18.0*pow_u_17* 6.0*pow_v_5
        + p_p->c1807y*18.0*pow_u_17* 7.0*pow_v_6
        + p_p->c1808y*18.0*pow_u_17* 8.0*pow_v_7
        + p_p->c1809y*18.0*pow_u_17* 9.0*pow_v_8
        + p_p->c1810y*18.0*pow_u_17*10.0*pow_v_9
        + p_p->c1811y*18.0*pow_u_17*11.0*pow_v_10
        + p_p->c1812y*18.0*pow_u_17*12.0*pow_v_11
        + p_p->c1813y*18.0*pow_u_17*13.0*pow_v_12
        + p_p->c1814y*18.0*pow_u_17*14.0*pow_v_13
        + p_p->c1815y*18.0*pow_u_17*15.0*pow_v_14
        + p_p->c1816y*18.0*pow_u_17*16.0*pow_v_15
        + p_p->c1817y*18.0*pow_u_17*17.0*pow_v_16
        + p_p->c1818y*18.0*pow_u_17*18.0*pow_v_17
        + p_p->c1819y*18.0*pow_u_17*19.0*pow_v_18
        + p_p->c1820y*18.0*pow_u_17*20.0*pow_v_19
        + p_p->c1821y*18.0*pow_u_17*21.0*pow_v_20;
uv_y=uv_y + p_p->c1900y*19.0*pow_u_18*         0.0        
        + p_p->c1901y*19.0*pow_u_18* 1.0*pow_v_0
        + p_p->c1902y*19.0*pow_u_18* 2.0*pow_v_1
        + p_p->c1903y*19.0*pow_u_18* 3.0*pow_v_2
        + p_p->c1904y*19.0*pow_u_18* 4.0*pow_v_3
        + p_p->c1905y*19.0*pow_u_18* 5.0*pow_v_4
        + p_p->c1906y*19.0*pow_u_18* 6.0*pow_v_5
        + p_p->c1907y*19.0*pow_u_18* 7.0*pow_v_6
        + p_p->c1908y*19.0*pow_u_18* 8.0*pow_v_7
        + p_p->c1909y*19.0*pow_u_18* 9.0*pow_v_8
        + p_p->c1910y*19.0*pow_u_18*10.0*pow_v_9
        + p_p->c1911y*19.0*pow_u_18*11.0*pow_v_10
        + p_p->c1912y*19.0*pow_u_18*12.0*pow_v_11
        + p_p->c1913y*19.0*pow_u_18*13.0*pow_v_12
        + p_p->c1914y*19.0*pow_u_18*14.0*pow_v_13
        + p_p->c1915y*19.0*pow_u_18*15.0*pow_v_14
        + p_p->c1916y*19.0*pow_u_18*16.0*pow_v_15
        + p_p->c1917y*19.0*pow_u_18*17.0*pow_v_16
        + p_p->c1918y*19.0*pow_u_18*18.0*pow_v_17
        + p_p->c1919y*19.0*pow_u_18*19.0*pow_v_18
        + p_p->c1920y*19.0*pow_u_18*20.0*pow_v_19
        + p_p->c1921y*19.0*pow_u_18*21.0*pow_v_20;
uv_y=uv_y + p_p->c2000y*20.0*pow_u_19*         0.0        
        + p_p->c2001y*20.0*pow_u_19* 1.0*pow_v_0
        + p_p->c2002y*20.0*pow_u_19* 2.0*pow_v_1
        + p_p->c2003y*20.0*pow_u_19* 3.0*pow_v_2
        + p_p->c2004y*20.0*pow_u_19* 4.0*pow_v_3
        + p_p->c2005y*20.0*pow_u_19* 5.0*pow_v_4
        + p_p->c2006y*20.0*pow_u_19* 6.0*pow_v_5
        + p_p->c2007y*20.0*pow_u_19* 7.0*pow_v_6
        + p_p->c2008y*20.0*pow_u_19* 8.0*pow_v_7
        + p_p->c2009y*20.0*pow_u_19* 9.0*pow_v_8
        + p_p->c2010y*20.0*pow_u_19*10.0*pow_v_9
        + p_p->c2011y*20.0*pow_u_19*11.0*pow_v_10
        + p_p->c2012y*20.0*pow_u_19*12.0*pow_v_11
        + p_p->c2013y*20.0*pow_u_19*13.0*pow_v_12
        + p_p->c2014y*20.0*pow_u_19*14.0*pow_v_13
        + p_p->c2015y*20.0*pow_u_19*15.0*pow_v_14
        + p_p->c2016y*20.0*pow_u_19*16.0*pow_v_15
        + p_p->c2017y*20.0*pow_u_19*17.0*pow_v_16
        + p_p->c2018y*20.0*pow_u_19*18.0*pow_v_17
        + p_p->c2019y*20.0*pow_u_19*19.0*pow_v_18
        + p_p->c2020y*20.0*pow_u_19*20.0*pow_v_19
        + p_p->c2021y*20.0*pow_u_19*21.0*pow_v_20;
uv_y=uv_y + p_p->c2100y*21.0*pow_u_20*         0.0        
        + p_p->c2101y*21.0*pow_u_20* 1.0*pow_v_0
        + p_p->c2102y*21.0*pow_u_20* 2.0*pow_v_1
        + p_p->c2103y*21.0*pow_u_20* 3.0*pow_v_2
        + p_p->c2104y*21.0*pow_u_20* 4.0*pow_v_3
        + p_p->c2105y*21.0*pow_u_20* 5.0*pow_v_4
        + p_p->c2106y*21.0*pow_u_20* 6.0*pow_v_5
        + p_p->c2107y*21.0*pow_u_20* 7.0*pow_v_6
        + p_p->c2108y*21.0*pow_u_20* 8.0*pow_v_7
        + p_p->c2109y*21.0*pow_u_20* 9.0*pow_v_8
        + p_p->c2110y*21.0*pow_u_20*10.0*pow_v_9
        + p_p->c2111y*21.0*pow_u_20*11.0*pow_v_10
        + p_p->c2112y*21.0*pow_u_20*12.0*pow_v_11
        + p_p->c2113y*21.0*pow_u_20*13.0*pow_v_12
        + p_p->c2114y*21.0*pow_u_20*14.0*pow_v_13
        + p_p->c2115y*21.0*pow_u_20*15.0*pow_v_14
        + p_p->c2116y*21.0*pow_u_20*16.0*pow_v_15
        + p_p->c2117y*21.0*pow_u_20*17.0*pow_v_16
        + p_p->c2118y*21.0*pow_u_20*18.0*pow_v_17
        + p_p->c2119y*21.0*pow_u_20*19.0*pow_v_18
        + p_p->c2120y*21.0*pow_u_20*20.0*pow_v_19
        + p_p->c2121y*21.0*pow_u_20*21.0*pow_v_20;

uv_z    = p_p->c0000z*         0.0        *         0.0        
        + p_p->c0001z*         0.0        * 1.0*pow_v_0
        + p_p->c0002z*         0.0        * 2.0*pow_v_1
        + p_p->c0003z*         0.0        * 3.0*pow_v_2
        + p_p->c0004z*         0.0        * 4.0*pow_v_3
        + p_p->c0005z*         0.0        * 5.0*pow_v_4
        + p_p->c0006z*         0.0        * 6.0*pow_v_5
        + p_p->c0007z*         0.0        * 7.0*pow_v_6
        + p_p->c0008z*         0.0        * 8.0*pow_v_7
        + p_p->c0009z*         0.0        * 9.0*pow_v_8
        + p_p->c0010z*         0.0        *10.0*pow_v_9
        + p_p->c0011z*         0.0        *11.0*pow_v_10
        + p_p->c0012z*         0.0        *12.0*pow_v_11
        + p_p->c0013z*         0.0        *13.0*pow_v_12
        + p_p->c0014z*         0.0        *14.0*pow_v_13
        + p_p->c0015z*         0.0        *15.0*pow_v_14
        + p_p->c0016z*         0.0        *16.0*pow_v_15
        + p_p->c0017z*         0.0        *17.0*pow_v_16
        + p_p->c0018z*         0.0        *18.0*pow_v_17
        + p_p->c0019z*         0.0        *19.0*pow_v_18
        + p_p->c0020z*         0.0        *20.0*pow_v_19
        + p_p->c0021z*         0.0        *21.0*pow_v_20;
uv_z=uv_z + p_p->c0100z* 1.0*pow_u_0*         0.0        
        + p_p->c0101z* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102z* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103z* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104z* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105z* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106z* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107z* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0108z* 1.0*pow_u_0* 8.0*pow_v_7
        + p_p->c0109z* 1.0*pow_u_0* 9.0*pow_v_8
        + p_p->c0110z* 1.0*pow_u_0*10.0*pow_v_9
        + p_p->c0111z* 1.0*pow_u_0*11.0*pow_v_10
        + p_p->c0112z* 1.0*pow_u_0*12.0*pow_v_11
        + p_p->c0113z* 1.0*pow_u_0*13.0*pow_v_12
        + p_p->c0114z* 1.0*pow_u_0*14.0*pow_v_13
        + p_p->c0115z* 1.0*pow_u_0*15.0*pow_v_14
        + p_p->c0116z* 1.0*pow_u_0*16.0*pow_v_15
        + p_p->c0117z* 1.0*pow_u_0*17.0*pow_v_16
        + p_p->c0118z* 1.0*pow_u_0*18.0*pow_v_17
        + p_p->c0119z* 1.0*pow_u_0*19.0*pow_v_18
        + p_p->c0120z* 1.0*pow_u_0*20.0*pow_v_19
        + p_p->c0121z* 1.0*pow_u_0*21.0*pow_v_20;
uv_z=uv_z + p_p->c0200z* 2.0*pow_u_1*         0.0        
        + p_p->c0201z* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202z* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203z* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204z* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205z* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206z* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207z* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0208z* 2.0*pow_u_1* 8.0*pow_v_7
        + p_p->c0209z* 2.0*pow_u_1* 9.0*pow_v_8
        + p_p->c0210z* 2.0*pow_u_1*10.0*pow_v_9
        + p_p->c0211z* 2.0*pow_u_1*11.0*pow_v_10
        + p_p->c0212z* 2.0*pow_u_1*12.0*pow_v_11
        + p_p->c0213z* 2.0*pow_u_1*13.0*pow_v_12
        + p_p->c0214z* 2.0*pow_u_1*14.0*pow_v_13
        + p_p->c0215z* 2.0*pow_u_1*15.0*pow_v_14
        + p_p->c0216z* 2.0*pow_u_1*16.0*pow_v_15
        + p_p->c0217z* 2.0*pow_u_1*17.0*pow_v_16
        + p_p->c0218z* 2.0*pow_u_1*18.0*pow_v_17
        + p_p->c0219z* 2.0*pow_u_1*19.0*pow_v_18
        + p_p->c0220z* 2.0*pow_u_1*20.0*pow_v_19
        + p_p->c0221z* 2.0*pow_u_1*21.0*pow_v_20;
uv_z=uv_z + p_p->c0300z* 3.0*pow_u_2*         0.0        
        + p_p->c0301z* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302z* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303z* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304z* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305z* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306z* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307z* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0308z* 3.0*pow_u_2* 8.0*pow_v_7
        + p_p->c0309z* 3.0*pow_u_2* 9.0*pow_v_8
        + p_p->c0310z* 3.0*pow_u_2*10.0*pow_v_9
        + p_p->c0311z* 3.0*pow_u_2*11.0*pow_v_10
        + p_p->c0312z* 3.0*pow_u_2*12.0*pow_v_11
        + p_p->c0313z* 3.0*pow_u_2*13.0*pow_v_12
        + p_p->c0314z* 3.0*pow_u_2*14.0*pow_v_13
        + p_p->c0315z* 3.0*pow_u_2*15.0*pow_v_14
        + p_p->c0316z* 3.0*pow_u_2*16.0*pow_v_15
        + p_p->c0317z* 3.0*pow_u_2*17.0*pow_v_16
        + p_p->c0318z* 3.0*pow_u_2*18.0*pow_v_17
        + p_p->c0319z* 3.0*pow_u_2*19.0*pow_v_18
        + p_p->c0320z* 3.0*pow_u_2*20.0*pow_v_19
        + p_p->c0321z* 3.0*pow_u_2*21.0*pow_v_20;
uv_z=uv_z + p_p->c0400z* 4.0*pow_u_3*         0.0        
        + p_p->c0401z* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402z* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403z* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404z* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405z* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406z* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407z* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0408z* 4.0*pow_u_3* 8.0*pow_v_7
        + p_p->c0409z* 4.0*pow_u_3* 9.0*pow_v_8
        + p_p->c0410z* 4.0*pow_u_3*10.0*pow_v_9
        + p_p->c0411z* 4.0*pow_u_3*11.0*pow_v_10
        + p_p->c0412z* 4.0*pow_u_3*12.0*pow_v_11
        + p_p->c0413z* 4.0*pow_u_3*13.0*pow_v_12
        + p_p->c0414z* 4.0*pow_u_3*14.0*pow_v_13
        + p_p->c0415z* 4.0*pow_u_3*15.0*pow_v_14
        + p_p->c0416z* 4.0*pow_u_3*16.0*pow_v_15
        + p_p->c0417z* 4.0*pow_u_3*17.0*pow_v_16
        + p_p->c0418z* 4.0*pow_u_3*18.0*pow_v_17
        + p_p->c0419z* 4.0*pow_u_3*19.0*pow_v_18
        + p_p->c0420z* 4.0*pow_u_3*20.0*pow_v_19
        + p_p->c0421z* 4.0*pow_u_3*21.0*pow_v_20;
uv_z=uv_z + p_p->c0500z* 5.0*pow_u_4*         0.0        
        + p_p->c0501z* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502z* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503z* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504z* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505z* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506z* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507z* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0508z* 5.0*pow_u_4* 8.0*pow_v_7
        + p_p->c0509z* 5.0*pow_u_4* 9.0*pow_v_8
        + p_p->c0510z* 5.0*pow_u_4*10.0*pow_v_9
        + p_p->c0511z* 5.0*pow_u_4*11.0*pow_v_10
        + p_p->c0512z* 5.0*pow_u_4*12.0*pow_v_11
        + p_p->c0513z* 5.0*pow_u_4*13.0*pow_v_12
        + p_p->c0514z* 5.0*pow_u_4*14.0*pow_v_13
        + p_p->c0515z* 5.0*pow_u_4*15.0*pow_v_14
        + p_p->c0516z* 5.0*pow_u_4*16.0*pow_v_15
        + p_p->c0517z* 5.0*pow_u_4*17.0*pow_v_16
        + p_p->c0518z* 5.0*pow_u_4*18.0*pow_v_17
        + p_p->c0519z* 5.0*pow_u_4*19.0*pow_v_18
        + p_p->c0520z* 5.0*pow_u_4*20.0*pow_v_19
        + p_p->c0521z* 5.0*pow_u_4*21.0*pow_v_20;
uv_z=uv_z + p_p->c0600z* 6.0*pow_u_5*         0.0        
        + p_p->c0601z* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602z* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603z* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604z* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605z* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606z* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607z* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0608z* 6.0*pow_u_5* 8.0*pow_v_7
        + p_p->c0609z* 6.0*pow_u_5* 9.0*pow_v_8
        + p_p->c0610z* 6.0*pow_u_5*10.0*pow_v_9
        + p_p->c0611z* 6.0*pow_u_5*11.0*pow_v_10
        + p_p->c0612z* 6.0*pow_u_5*12.0*pow_v_11
        + p_p->c0613z* 6.0*pow_u_5*13.0*pow_v_12
        + p_p->c0614z* 6.0*pow_u_5*14.0*pow_v_13
        + p_p->c0615z* 6.0*pow_u_5*15.0*pow_v_14
        + p_p->c0616z* 6.0*pow_u_5*16.0*pow_v_15
        + p_p->c0617z* 6.0*pow_u_5*17.0*pow_v_16
        + p_p->c0618z* 6.0*pow_u_5*18.0*pow_v_17
        + p_p->c0619z* 6.0*pow_u_5*19.0*pow_v_18
        + p_p->c0620z* 6.0*pow_u_5*20.0*pow_v_19
        + p_p->c0621z* 6.0*pow_u_5*21.0*pow_v_20;
uv_z=uv_z + p_p->c0700z* 7.0*pow_u_6*         0.0        
        + p_p->c0701z* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702z* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703z* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704z* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705z* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706z* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707z* 7.0*pow_u_6* 7.0*pow_v_6
        + p_p->c0708z* 7.0*pow_u_6* 8.0*pow_v_7
        + p_p->c0709z* 7.0*pow_u_6* 9.0*pow_v_8
        + p_p->c0710z* 7.0*pow_u_6*10.0*pow_v_9
        + p_p->c0711z* 7.0*pow_u_6*11.0*pow_v_10
        + p_p->c0712z* 7.0*pow_u_6*12.0*pow_v_11
        + p_p->c0713z* 7.0*pow_u_6*13.0*pow_v_12
        + p_p->c0714z* 7.0*pow_u_6*14.0*pow_v_13
        + p_p->c0715z* 7.0*pow_u_6*15.0*pow_v_14
        + p_p->c0716z* 7.0*pow_u_6*16.0*pow_v_15
        + p_p->c0717z* 7.0*pow_u_6*17.0*pow_v_16
        + p_p->c0718z* 7.0*pow_u_6*18.0*pow_v_17
        + p_p->c0719z* 7.0*pow_u_6*19.0*pow_v_18
        + p_p->c0720z* 7.0*pow_u_6*20.0*pow_v_19
        + p_p->c0721z* 7.0*pow_u_6*21.0*pow_v_20;
uv_z=uv_z + p_p->c0800z* 8.0*pow_u_7*         0.0        
        + p_p->c0801z* 8.0*pow_u_7* 1.0*pow_v_0
        + p_p->c0802z* 8.0*pow_u_7* 2.0*pow_v_1
        + p_p->c0803z* 8.0*pow_u_7* 3.0*pow_v_2
        + p_p->c0804z* 8.0*pow_u_7* 4.0*pow_v_3
        + p_p->c0805z* 8.0*pow_u_7* 5.0*pow_v_4
        + p_p->c0806z* 8.0*pow_u_7* 6.0*pow_v_5
        + p_p->c0807z* 8.0*pow_u_7* 7.0*pow_v_6
        + p_p->c0808z* 8.0*pow_u_7* 8.0*pow_v_7
        + p_p->c0809z* 8.0*pow_u_7* 9.0*pow_v_8
        + p_p->c0810z* 8.0*pow_u_7*10.0*pow_v_9
        + p_p->c0811z* 8.0*pow_u_7*11.0*pow_v_10
        + p_p->c0812z* 8.0*pow_u_7*12.0*pow_v_11
        + p_p->c0813z* 8.0*pow_u_7*13.0*pow_v_12
        + p_p->c0814z* 8.0*pow_u_7*14.0*pow_v_13
        + p_p->c0815z* 8.0*pow_u_7*15.0*pow_v_14
        + p_p->c0816z* 8.0*pow_u_7*16.0*pow_v_15
        + p_p->c0817z* 8.0*pow_u_7*17.0*pow_v_16
        + p_p->c0818z* 8.0*pow_u_7*18.0*pow_v_17
        + p_p->c0819z* 8.0*pow_u_7*19.0*pow_v_18
        + p_p->c0820z* 8.0*pow_u_7*20.0*pow_v_19
        + p_p->c0821z* 8.0*pow_u_7*21.0*pow_v_20;
uv_z=uv_z + p_p->c0900z* 9.0*pow_u_8*         0.0        
        + p_p->c0901z* 9.0*pow_u_8* 1.0*pow_v_0
        + p_p->c0902z* 9.0*pow_u_8* 2.0*pow_v_1
        + p_p->c0903z* 9.0*pow_u_8* 3.0*pow_v_2
        + p_p->c0904z* 9.0*pow_u_8* 4.0*pow_v_3
        + p_p->c0905z* 9.0*pow_u_8* 5.0*pow_v_4
        + p_p->c0906z* 9.0*pow_u_8* 6.0*pow_v_5
        + p_p->c0907z* 9.0*pow_u_8* 7.0*pow_v_6
        + p_p->c0908z* 9.0*pow_u_8* 8.0*pow_v_7
        + p_p->c0909z* 9.0*pow_u_8* 9.0*pow_v_8
        + p_p->c0910z* 9.0*pow_u_8*10.0*pow_v_9
        + p_p->c0911z* 9.0*pow_u_8*11.0*pow_v_10
        + p_p->c0912z* 9.0*pow_u_8*12.0*pow_v_11
        + p_p->c0913z* 9.0*pow_u_8*13.0*pow_v_12
        + p_p->c0914z* 9.0*pow_u_8*14.0*pow_v_13
        + p_p->c0915z* 9.0*pow_u_8*15.0*pow_v_14
        + p_p->c0916z* 9.0*pow_u_8*16.0*pow_v_15
        + p_p->c0917z* 9.0*pow_u_8*17.0*pow_v_16
        + p_p->c0918z* 9.0*pow_u_8*18.0*pow_v_17
        + p_p->c0919z* 9.0*pow_u_8*19.0*pow_v_18
        + p_p->c0920z* 9.0*pow_u_8*20.0*pow_v_19
        + p_p->c0921z* 9.0*pow_u_8*21.0*pow_v_20;
uv_z=uv_z + p_p->c1000z*10.0*pow_u_9*         0.0        
        + p_p->c1001z*10.0*pow_u_9* 1.0*pow_v_0
        + p_p->c1002z*10.0*pow_u_9* 2.0*pow_v_1
        + p_p->c1003z*10.0*pow_u_9* 3.0*pow_v_2
        + p_p->c1004z*10.0*pow_u_9* 4.0*pow_v_3
        + p_p->c1005z*10.0*pow_u_9* 5.0*pow_v_4
        + p_p->c1006z*10.0*pow_u_9* 6.0*pow_v_5
        + p_p->c1007z*10.0*pow_u_9* 7.0*pow_v_6
        + p_p->c1008z*10.0*pow_u_9* 8.0*pow_v_7
        + p_p->c1009z*10.0*pow_u_9* 9.0*pow_v_8
        + p_p->c1010z*10.0*pow_u_9*10.0*pow_v_9
        + p_p->c1011z*10.0*pow_u_9*11.0*pow_v_10
        + p_p->c1012z*10.0*pow_u_9*12.0*pow_v_11
        + p_p->c1013z*10.0*pow_u_9*13.0*pow_v_12
        + p_p->c1014z*10.0*pow_u_9*14.0*pow_v_13
        + p_p->c1015z*10.0*pow_u_9*15.0*pow_v_14
        + p_p->c1016z*10.0*pow_u_9*16.0*pow_v_15
        + p_p->c1017z*10.0*pow_u_9*17.0*pow_v_16
        + p_p->c1018z*10.0*pow_u_9*18.0*pow_v_17
        + p_p->c1019z*10.0*pow_u_9*19.0*pow_v_18
        + p_p->c1020z*10.0*pow_u_9*20.0*pow_v_19
        + p_p->c1021z*10.0*pow_u_9*21.0*pow_v_20;
uv_z=uv_z + p_p->c1100z*11.0*pow_u_10*         0.0        
        + p_p->c1101z*11.0*pow_u_10* 1.0*pow_v_0
        + p_p->c1102z*11.0*pow_u_10* 2.0*pow_v_1
        + p_p->c1103z*11.0*pow_u_10* 3.0*pow_v_2
        + p_p->c1104z*11.0*pow_u_10* 4.0*pow_v_3
        + p_p->c1105z*11.0*pow_u_10* 5.0*pow_v_4
        + p_p->c1106z*11.0*pow_u_10* 6.0*pow_v_5
        + p_p->c1107z*11.0*pow_u_10* 7.0*pow_v_6
        + p_p->c1108z*11.0*pow_u_10* 8.0*pow_v_7
        + p_p->c1109z*11.0*pow_u_10* 9.0*pow_v_8
        + p_p->c1110z*11.0*pow_u_10*10.0*pow_v_9
        + p_p->c1111z*11.0*pow_u_10*11.0*pow_v_10
        + p_p->c1112z*11.0*pow_u_10*12.0*pow_v_11
        + p_p->c1113z*11.0*pow_u_10*13.0*pow_v_12
        + p_p->c1114z*11.0*pow_u_10*14.0*pow_v_13
        + p_p->c1115z*11.0*pow_u_10*15.0*pow_v_14
        + p_p->c1116z*11.0*pow_u_10*16.0*pow_v_15
        + p_p->c1117z*11.0*pow_u_10*17.0*pow_v_16
        + p_p->c1118z*11.0*pow_u_10*18.0*pow_v_17
        + p_p->c1119z*11.0*pow_u_10*19.0*pow_v_18
        + p_p->c1120z*11.0*pow_u_10*20.0*pow_v_19
        + p_p->c1121z*11.0*pow_u_10*21.0*pow_v_20;
uv_z=uv_z + p_p->c1200z*12.0*pow_u_11*         0.0        
        + p_p->c1201z*12.0*pow_u_11* 1.0*pow_v_0
        + p_p->c1202z*12.0*pow_u_11* 2.0*pow_v_1
        + p_p->c1203z*12.0*pow_u_11* 3.0*pow_v_2
        + p_p->c1204z*12.0*pow_u_11* 4.0*pow_v_3
        + p_p->c1205z*12.0*pow_u_11* 5.0*pow_v_4
        + p_p->c1206z*12.0*pow_u_11* 6.0*pow_v_5
        + p_p->c1207z*12.0*pow_u_11* 7.0*pow_v_6
        + p_p->c1208z*12.0*pow_u_11* 8.0*pow_v_7
        + p_p->c1209z*12.0*pow_u_11* 9.0*pow_v_8
        + p_p->c1210z*12.0*pow_u_11*10.0*pow_v_9
        + p_p->c1211z*12.0*pow_u_11*11.0*pow_v_10
        + p_p->c1212z*12.0*pow_u_11*12.0*pow_v_11
        + p_p->c1213z*12.0*pow_u_11*13.0*pow_v_12
        + p_p->c1214z*12.0*pow_u_11*14.0*pow_v_13
        + p_p->c1215z*12.0*pow_u_11*15.0*pow_v_14
        + p_p->c1216z*12.0*pow_u_11*16.0*pow_v_15
        + p_p->c1217z*12.0*pow_u_11*17.0*pow_v_16
        + p_p->c1218z*12.0*pow_u_11*18.0*pow_v_17
        + p_p->c1219z*12.0*pow_u_11*19.0*pow_v_18
        + p_p->c1220z*12.0*pow_u_11*20.0*pow_v_19
        + p_p->c1221z*12.0*pow_u_11*21.0*pow_v_20;
uv_z=uv_z + p_p->c1300z*13.0*pow_u_12*         0.0        
        + p_p->c1301z*13.0*pow_u_12* 1.0*pow_v_0
        + p_p->c1302z*13.0*pow_u_12* 2.0*pow_v_1
        + p_p->c1303z*13.0*pow_u_12* 3.0*pow_v_2
        + p_p->c1304z*13.0*pow_u_12* 4.0*pow_v_3
        + p_p->c1305z*13.0*pow_u_12* 5.0*pow_v_4
        + p_p->c1306z*13.0*pow_u_12* 6.0*pow_v_5
        + p_p->c1307z*13.0*pow_u_12* 7.0*pow_v_6
        + p_p->c1308z*13.0*pow_u_12* 8.0*pow_v_7
        + p_p->c1309z*13.0*pow_u_12* 9.0*pow_v_8
        + p_p->c1310z*13.0*pow_u_12*10.0*pow_v_9
        + p_p->c1311z*13.0*pow_u_12*11.0*pow_v_10
        + p_p->c1312z*13.0*pow_u_12*12.0*pow_v_11
        + p_p->c1313z*13.0*pow_u_12*13.0*pow_v_12
        + p_p->c1314z*13.0*pow_u_12*14.0*pow_v_13
        + p_p->c1315z*13.0*pow_u_12*15.0*pow_v_14
        + p_p->c1316z*13.0*pow_u_12*16.0*pow_v_15
        + p_p->c1317z*13.0*pow_u_12*17.0*pow_v_16
        + p_p->c1318z*13.0*pow_u_12*18.0*pow_v_17
        + p_p->c1319z*13.0*pow_u_12*19.0*pow_v_18
        + p_p->c1320z*13.0*pow_u_12*20.0*pow_v_19
        + p_p->c1321z*13.0*pow_u_12*21.0*pow_v_20;
uv_z=uv_z + p_p->c1400z*14.0*pow_u_13*         0.0        
        + p_p->c1401z*14.0*pow_u_13* 1.0*pow_v_0
        + p_p->c1402z*14.0*pow_u_13* 2.0*pow_v_1
        + p_p->c1403z*14.0*pow_u_13* 3.0*pow_v_2
        + p_p->c1404z*14.0*pow_u_13* 4.0*pow_v_3
        + p_p->c1405z*14.0*pow_u_13* 5.0*pow_v_4
        + p_p->c1406z*14.0*pow_u_13* 6.0*pow_v_5
        + p_p->c1407z*14.0*pow_u_13* 7.0*pow_v_6
        + p_p->c1408z*14.0*pow_u_13* 8.0*pow_v_7
        + p_p->c1409z*14.0*pow_u_13* 9.0*pow_v_8
        + p_p->c1410z*14.0*pow_u_13*10.0*pow_v_9
        + p_p->c1411z*14.0*pow_u_13*11.0*pow_v_10
        + p_p->c1412z*14.0*pow_u_13*12.0*pow_v_11
        + p_p->c1413z*14.0*pow_u_13*13.0*pow_v_12
        + p_p->c1414z*14.0*pow_u_13*14.0*pow_v_13
        + p_p->c1415z*14.0*pow_u_13*15.0*pow_v_14
        + p_p->c1416z*14.0*pow_u_13*16.0*pow_v_15
        + p_p->c1417z*14.0*pow_u_13*17.0*pow_v_16
        + p_p->c1418z*14.0*pow_u_13*18.0*pow_v_17
        + p_p->c1419z*14.0*pow_u_13*19.0*pow_v_18
        + p_p->c1420z*14.0*pow_u_13*20.0*pow_v_19
        + p_p->c1421z*14.0*pow_u_13*21.0*pow_v_20;
uv_z=uv_z + p_p->c1500z*15.0*pow_u_14*         0.0        
        + p_p->c1501z*15.0*pow_u_14* 1.0*pow_v_0
        + p_p->c1502z*15.0*pow_u_14* 2.0*pow_v_1
        + p_p->c1503z*15.0*pow_u_14* 3.0*pow_v_2
        + p_p->c1504z*15.0*pow_u_14* 4.0*pow_v_3
        + p_p->c1505z*15.0*pow_u_14* 5.0*pow_v_4
        + p_p->c1506z*15.0*pow_u_14* 6.0*pow_v_5
        + p_p->c1507z*15.0*pow_u_14* 7.0*pow_v_6
        + p_p->c1508z*15.0*pow_u_14* 8.0*pow_v_7
        + p_p->c1509z*15.0*pow_u_14* 9.0*pow_v_8
        + p_p->c1510z*15.0*pow_u_14*10.0*pow_v_9
        + p_p->c1511z*15.0*pow_u_14*11.0*pow_v_10
        + p_p->c1512z*15.0*pow_u_14*12.0*pow_v_11
        + p_p->c1513z*15.0*pow_u_14*13.0*pow_v_12
        + p_p->c1514z*15.0*pow_u_14*14.0*pow_v_13
        + p_p->c1515z*15.0*pow_u_14*15.0*pow_v_14
        + p_p->c1516z*15.0*pow_u_14*16.0*pow_v_15
        + p_p->c1517z*15.0*pow_u_14*17.0*pow_v_16
        + p_p->c1518z*15.0*pow_u_14*18.0*pow_v_17
        + p_p->c1519z*15.0*pow_u_14*19.0*pow_v_18
        + p_p->c1520z*15.0*pow_u_14*20.0*pow_v_19
        + p_p->c1521z*15.0*pow_u_14*21.0*pow_v_20;
uv_z=uv_z + p_p->c1600z*16.0*pow_u_15*         0.0        
        + p_p->c1601z*16.0*pow_u_15* 1.0*pow_v_0
        + p_p->c1602z*16.0*pow_u_15* 2.0*pow_v_1
        + p_p->c1603z*16.0*pow_u_15* 3.0*pow_v_2
        + p_p->c1604z*16.0*pow_u_15* 4.0*pow_v_3
        + p_p->c1605z*16.0*pow_u_15* 5.0*pow_v_4
        + p_p->c1606z*16.0*pow_u_15* 6.0*pow_v_5
        + p_p->c1607z*16.0*pow_u_15* 7.0*pow_v_6
        + p_p->c1608z*16.0*pow_u_15* 8.0*pow_v_7
        + p_p->c1609z*16.0*pow_u_15* 9.0*pow_v_8
        + p_p->c1610z*16.0*pow_u_15*10.0*pow_v_9
        + p_p->c1611z*16.0*pow_u_15*11.0*pow_v_10
        + p_p->c1612z*16.0*pow_u_15*12.0*pow_v_11
        + p_p->c1613z*16.0*pow_u_15*13.0*pow_v_12
        + p_p->c1614z*16.0*pow_u_15*14.0*pow_v_13
        + p_p->c1615z*16.0*pow_u_15*15.0*pow_v_14
        + p_p->c1616z*16.0*pow_u_15*16.0*pow_v_15
        + p_p->c1617z*16.0*pow_u_15*17.0*pow_v_16
        + p_p->c1618z*16.0*pow_u_15*18.0*pow_v_17
        + p_p->c1619z*16.0*pow_u_15*19.0*pow_v_18
        + p_p->c1620z*16.0*pow_u_15*20.0*pow_v_19
        + p_p->c1621z*16.0*pow_u_15*21.0*pow_v_20;
uv_z=uv_z + p_p->c1700z*17.0*pow_u_16*         0.0        
        + p_p->c1701z*17.0*pow_u_16* 1.0*pow_v_0
        + p_p->c1702z*17.0*pow_u_16* 2.0*pow_v_1
        + p_p->c1703z*17.0*pow_u_16* 3.0*pow_v_2
        + p_p->c1704z*17.0*pow_u_16* 4.0*pow_v_3
        + p_p->c1705z*17.0*pow_u_16* 5.0*pow_v_4
        + p_p->c1706z*17.0*pow_u_16* 6.0*pow_v_5
        + p_p->c1707z*17.0*pow_u_16* 7.0*pow_v_6
        + p_p->c1708z*17.0*pow_u_16* 8.0*pow_v_7
        + p_p->c1709z*17.0*pow_u_16* 9.0*pow_v_8
        + p_p->c1710z*17.0*pow_u_16*10.0*pow_v_9
        + p_p->c1711z*17.0*pow_u_16*11.0*pow_v_10
        + p_p->c1712z*17.0*pow_u_16*12.0*pow_v_11
        + p_p->c1713z*17.0*pow_u_16*13.0*pow_v_12
        + p_p->c1714z*17.0*pow_u_16*14.0*pow_v_13
        + p_p->c1715z*17.0*pow_u_16*15.0*pow_v_14
        + p_p->c1716z*17.0*pow_u_16*16.0*pow_v_15
        + p_p->c1717z*17.0*pow_u_16*17.0*pow_v_16
        + p_p->c1718z*17.0*pow_u_16*18.0*pow_v_17
        + p_p->c1719z*17.0*pow_u_16*19.0*pow_v_18
        + p_p->c1720z*17.0*pow_u_16*20.0*pow_v_19
        + p_p->c1721z*17.0*pow_u_16*21.0*pow_v_20;
uv_z=uv_z + p_p->c1800z*18.0*pow_u_17*         0.0        
        + p_p->c1801z*18.0*pow_u_17* 1.0*pow_v_0
        + p_p->c1802z*18.0*pow_u_17* 2.0*pow_v_1
        + p_p->c1803z*18.0*pow_u_17* 3.0*pow_v_2
        + p_p->c1804z*18.0*pow_u_17* 4.0*pow_v_3
        + p_p->c1805z*18.0*pow_u_17* 5.0*pow_v_4
        + p_p->c1806z*18.0*pow_u_17* 6.0*pow_v_5
        + p_p->c1807z*18.0*pow_u_17* 7.0*pow_v_6
        + p_p->c1808z*18.0*pow_u_17* 8.0*pow_v_7
        + p_p->c1809z*18.0*pow_u_17* 9.0*pow_v_8
        + p_p->c1810z*18.0*pow_u_17*10.0*pow_v_9
        + p_p->c1811z*18.0*pow_u_17*11.0*pow_v_10
        + p_p->c1812z*18.0*pow_u_17*12.0*pow_v_11
        + p_p->c1813z*18.0*pow_u_17*13.0*pow_v_12
        + p_p->c1814z*18.0*pow_u_17*14.0*pow_v_13
        + p_p->c1815z*18.0*pow_u_17*15.0*pow_v_14
        + p_p->c1816z*18.0*pow_u_17*16.0*pow_v_15
        + p_p->c1817z*18.0*pow_u_17*17.0*pow_v_16
        + p_p->c1818z*18.0*pow_u_17*18.0*pow_v_17
        + p_p->c1819z*18.0*pow_u_17*19.0*pow_v_18
        + p_p->c1820z*18.0*pow_u_17*20.0*pow_v_19
        + p_p->c1821z*18.0*pow_u_17*21.0*pow_v_20;
uv_z=uv_z + p_p->c1900z*19.0*pow_u_18*         0.0        
        + p_p->c1901z*19.0*pow_u_18* 1.0*pow_v_0
        + p_p->c1902z*19.0*pow_u_18* 2.0*pow_v_1
        + p_p->c1903z*19.0*pow_u_18* 3.0*pow_v_2
        + p_p->c1904z*19.0*pow_u_18* 4.0*pow_v_3
        + p_p->c1905z*19.0*pow_u_18* 5.0*pow_v_4
        + p_p->c1906z*19.0*pow_u_18* 6.0*pow_v_5
        + p_p->c1907z*19.0*pow_u_18* 7.0*pow_v_6
        + p_p->c1908z*19.0*pow_u_18* 8.0*pow_v_7
        + p_p->c1909z*19.0*pow_u_18* 9.0*pow_v_8
        + p_p->c1910z*19.0*pow_u_18*10.0*pow_v_9
        + p_p->c1911z*19.0*pow_u_18*11.0*pow_v_10
        + p_p->c1912z*19.0*pow_u_18*12.0*pow_v_11
        + p_p->c1913z*19.0*pow_u_18*13.0*pow_v_12
        + p_p->c1914z*19.0*pow_u_18*14.0*pow_v_13
        + p_p->c1915z*19.0*pow_u_18*15.0*pow_v_14
        + p_p->c1916z*19.0*pow_u_18*16.0*pow_v_15
        + p_p->c1917z*19.0*pow_u_18*17.0*pow_v_16
        + p_p->c1918z*19.0*pow_u_18*18.0*pow_v_17
        + p_p->c1919z*19.0*pow_u_18*19.0*pow_v_18
        + p_p->c1920z*19.0*pow_u_18*20.0*pow_v_19
        + p_p->c1921z*19.0*pow_u_18*21.0*pow_v_20;
uv_z=uv_z + p_p->c2000z*20.0*pow_u_19*         0.0        
        + p_p->c2001z*20.0*pow_u_19* 1.0*pow_v_0
        + p_p->c2002z*20.0*pow_u_19* 2.0*pow_v_1
        + p_p->c2003z*20.0*pow_u_19* 3.0*pow_v_2
        + p_p->c2004z*20.0*pow_u_19* 4.0*pow_v_3
        + p_p->c2005z*20.0*pow_u_19* 5.0*pow_v_4
        + p_p->c2006z*20.0*pow_u_19* 6.0*pow_v_5
        + p_p->c2007z*20.0*pow_u_19* 7.0*pow_v_6
        + p_p->c2008z*20.0*pow_u_19* 8.0*pow_v_7
        + p_p->c2009z*20.0*pow_u_19* 9.0*pow_v_8
        + p_p->c2010z*20.0*pow_u_19*10.0*pow_v_9
        + p_p->c2011z*20.0*pow_u_19*11.0*pow_v_10
        + p_p->c2012z*20.0*pow_u_19*12.0*pow_v_11
        + p_p->c2013z*20.0*pow_u_19*13.0*pow_v_12
        + p_p->c2014z*20.0*pow_u_19*14.0*pow_v_13
        + p_p->c2015z*20.0*pow_u_19*15.0*pow_v_14
        + p_p->c2016z*20.0*pow_u_19*16.0*pow_v_15
        + p_p->c2017z*20.0*pow_u_19*17.0*pow_v_16
        + p_p->c2018z*20.0*pow_u_19*18.0*pow_v_17
        + p_p->c2019z*20.0*pow_u_19*19.0*pow_v_18
        + p_p->c2020z*20.0*pow_u_19*20.0*pow_v_19
        + p_p->c2021z*20.0*pow_u_19*21.0*pow_v_20;
uv_z=uv_z + p_p->c2100z*21.0*pow_u_20*         0.0        
        + p_p->c2101z*21.0*pow_u_20* 1.0*pow_v_0
        + p_p->c2102z*21.0*pow_u_20* 2.0*pow_v_1
        + p_p->c2103z*21.0*pow_u_20* 3.0*pow_v_2
        + p_p->c2104z*21.0*pow_u_20* 4.0*pow_v_3
        + p_p->c2105z*21.0*pow_u_20* 5.0*pow_v_4
        + p_p->c2106z*21.0*pow_u_20* 6.0*pow_v_5
        + p_p->c2107z*21.0*pow_u_20* 7.0*pow_v_6
        + p_p->c2108z*21.0*pow_u_20* 8.0*pow_v_7
        + p_p->c2109z*21.0*pow_u_20* 9.0*pow_v_8
        + p_p->c2110z*21.0*pow_u_20*10.0*pow_v_9
        + p_p->c2111z*21.0*pow_u_20*11.0*pow_v_10
        + p_p->c2112z*21.0*pow_u_20*12.0*pow_v_11
        + p_p->c2113z*21.0*pow_u_20*13.0*pow_v_12
        + p_p->c2114z*21.0*pow_u_20*14.0*pow_v_13
        + p_p->c2115z*21.0*pow_u_20*15.0*pow_v_14
        + p_p->c2116z*21.0*pow_u_20*16.0*pow_v_15
        + p_p->c2117z*21.0*pow_u_20*17.0*pow_v_16
        + p_p->c2118z*21.0*pow_u_20*18.0*pow_v_17
        + p_p->c2119z*21.0*pow_u_20*19.0*pow_v_18
        + p_p->c2120z*21.0*pow_u_20*20.0*pow_v_19
        + p_p->c2121z*21.0*pow_u_20*21.0*pow_v_20;





#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur504*c_second Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

#endif  /*  DEF_P21  */
