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
/*  Function: varkon_pat_p7eval                    File: sur502.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Evaluation of GMPATP7                                           */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-22   Created by MBS program p_evalpxv0                  */
/*  1999-11-22   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_p7eval     Evaluation of GMPATP7            */
/*                                                              */
/*------------------------------------------------------------- */

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
static DBfloat pow_v_0;           /* V**0                           */
static DBfloat pow_v_1;           /* V**1                           */
static DBfloat pow_v_2;           /* V**2                           */
static DBfloat pow_v_3;           /* V**3                           */
static DBfloat pow_v_4;           /* V**4                           */
static DBfloat pow_v_5;           /* V**5                           */
static DBfloat pow_v_6;           /* V**6                           */
static DBfloat pow_v_7;           /* V**7                           */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = Illegal computation case=   varkon_pat_p7eval  (sur502) */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_p7eval    */
/* SU2993 = Severe program error in varkon_pat_p7eval  (sur502).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_pat_p7eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATP7  *p_pat7,     /* Patch GMPATP7                     (ptr) */
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
   DBint  status;        /* Error code from a called function       */

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

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur502 Enter *** varkon_pat_p7eval : u= %f v= %f p_pat7= %d\n",
         u_l  ,v_l  , (int)p_pat7 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

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
"sur502 Error Internal function initial failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"initial%%sur502");
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
"sur502 Error Internal function c_power failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_power%%sur502");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/* Calculate coordinates. Call of internal function c_coord.        */
/* Goto nomore if icase =  0.                                       */
/*                                                                 !*/

   status= c_coord(p_pat7, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur502 Error Internal function c_coord failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_coord%%sur502");
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

   status= c_drdu (p_pat7, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur502 Error Internal function c_drdu  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdu %%sur502");
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

   status= c_drdv (p_pat7, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur502 Error Internal function c_drdv  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdv %%sur502");
       return(varkon_erpush("SU2973",errbuf));
       }

   p_xyz->v_x= v_x;
   p_xyz->v_y= v_y;
   p_xyz->v_z= v_z;

   if  (  icase == 2 || icase == 3 ) goto nomore;


/*!                                                                 */
/* Calculate second derivatives. Call of internal function c_second */
/*                                                                 !*/

   status= c_second(p_pat7, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur502 Error Internal function c_second failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_second%%sur502");
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
  "sur502 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
if ( dbglev(SURPAC) == 1 && icase >= 3 )
  {
  fprintf(dbgfil(SURPAC),
  "sur502 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur502 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  }
if ( dbglev(SURPAC) == 2 && icase >= 4 )
  {
  fprintf(dbgfil(SURPAC),
  "sur502 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur502 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur502 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur502 Exit*varkon_pat_p7eval * p_xyz %f %f %f\n",
    p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

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
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur502*initial: icase= %d p_xyz= %d\n",(short)icase,(int)p_xyz);
fflush(dbgfil(SURPAC)); 
}
#endif
/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur502 Error icase= %d\n", (short)icase );
fflush(dbgfil(SURPAC)); 
}
#endif

        sprintf(errbuf,"%d%% varkon_pat_p7eval  (sur502)",(short)icase);
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
"sur502*c_power u_l= %f v_l= %f\n",u_l,v_l);
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
   pow_v_0  = pow( v_l, 0.0 );
   pow_v_1  = pow( v_l, 1.0 );
   pow_v_2  = pow( v_l, 2.0 );
   pow_v_3  = pow( v_l, 3.0 );
   pow_v_4  = pow( v_l, 4.0 );
   pow_v_5  = pow( v_l, 5.0 );
   pow_v_6  = pow( v_l, 6.0 );
   pow_v_7  = pow( v_l, 7.0 );




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur502*c_power Exit \n");
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
/* The function calculates coordinates for GMPATP7                  */

   static short c_coord ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP7   *p_p;       /* Patch GMPATP7                     (ptr) */
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
"sur502*c_coord p_p= %d  u_l= %f v_l= %f\n",(int)p_p,u_l,v_l);
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
        + p_p->c0100x * pow_u_1   * pow_v_0  
        + p_p->c0101x * pow_u_1   * pow_v_1  
        + p_p->c0102x * pow_u_1   * pow_v_2  
        + p_p->c0103x * pow_u_1   * pow_v_3  
        + p_p->c0104x * pow_u_1   * pow_v_4  
        + p_p->c0105x * pow_u_1   * pow_v_5  
        + p_p->c0106x * pow_u_1   * pow_v_6  
        + p_p->c0107x * pow_u_1   * pow_v_7  
        + p_p->c0200x * pow_u_2   * pow_v_0  
        + p_p->c0201x * pow_u_2   * pow_v_1  
        + p_p->c0202x * pow_u_2   * pow_v_2  
        + p_p->c0203x * pow_u_2   * pow_v_3  
        + p_p->c0204x * pow_u_2   * pow_v_4  
        + p_p->c0205x * pow_u_2   * pow_v_5  
        + p_p->c0206x * pow_u_2   * pow_v_6  
        + p_p->c0207x * pow_u_2   * pow_v_7  
        + p_p->c0300x * pow_u_3   * pow_v_0  
        + p_p->c0301x * pow_u_3   * pow_v_1  
        + p_p->c0302x * pow_u_3   * pow_v_2  
        + p_p->c0303x * pow_u_3   * pow_v_3  
        + p_p->c0304x * pow_u_3   * pow_v_4  
        + p_p->c0305x * pow_u_3   * pow_v_5  
        + p_p->c0306x * pow_u_3   * pow_v_6  
        + p_p->c0307x * pow_u_3   * pow_v_7  
        + p_p->c0400x * pow_u_4   * pow_v_0  
        + p_p->c0401x * pow_u_4   * pow_v_1  
        + p_p->c0402x * pow_u_4   * pow_v_2  
        + p_p->c0403x * pow_u_4   * pow_v_3  
        + p_p->c0404x * pow_u_4   * pow_v_4  
        + p_p->c0405x * pow_u_4   * pow_v_5  
        + p_p->c0406x * pow_u_4   * pow_v_6  
        + p_p->c0407x * pow_u_4   * pow_v_7  
        + p_p->c0500x * pow_u_5   * pow_v_0  
        + p_p->c0501x * pow_u_5   * pow_v_1  
        + p_p->c0502x * pow_u_5   * pow_v_2  
        + p_p->c0503x * pow_u_5   * pow_v_3  
        + p_p->c0504x * pow_u_5   * pow_v_4  
        + p_p->c0505x * pow_u_5   * pow_v_5  
        + p_p->c0506x * pow_u_5   * pow_v_6  
        + p_p->c0507x * pow_u_5   * pow_v_7  
        + p_p->c0600x * pow_u_6   * pow_v_0  
        + p_p->c0601x * pow_u_6   * pow_v_1  
        + p_p->c0602x * pow_u_6   * pow_v_2  
        + p_p->c0603x * pow_u_6   * pow_v_3  
        + p_p->c0604x * pow_u_6   * pow_v_4  
        + p_p->c0605x * pow_u_6   * pow_v_5  
        + p_p->c0606x * pow_u_6   * pow_v_6  
        + p_p->c0607x * pow_u_6   * pow_v_7  
        + p_p->c0700x * pow_u_7   * pow_v_0  
        + p_p->c0701x * pow_u_7   * pow_v_1  
        + p_p->c0702x * pow_u_7   * pow_v_2  
        + p_p->c0703x * pow_u_7   * pow_v_3  
        + p_p->c0704x * pow_u_7   * pow_v_4  
        + p_p->c0705x * pow_u_7   * pow_v_5  
        + p_p->c0706x * pow_u_7   * pow_v_6  
        + p_p->c0707x * pow_u_7   * pow_v_7  ;

r_y     = p_p->c0000y * pow_u_0   * pow_v_0  
        + p_p->c0001y * pow_u_0   * pow_v_1  
        + p_p->c0002y * pow_u_0   * pow_v_2  
        + p_p->c0003y * pow_u_0   * pow_v_3  
        + p_p->c0004y * pow_u_0   * pow_v_4  
        + p_p->c0005y * pow_u_0   * pow_v_5  
        + p_p->c0006y * pow_u_0   * pow_v_6  
        + p_p->c0007y * pow_u_0   * pow_v_7  
        + p_p->c0100y * pow_u_1   * pow_v_0  
        + p_p->c0101y * pow_u_1   * pow_v_1  
        + p_p->c0102y * pow_u_1   * pow_v_2  
        + p_p->c0103y * pow_u_1   * pow_v_3  
        + p_p->c0104y * pow_u_1   * pow_v_4  
        + p_p->c0105y * pow_u_1   * pow_v_5  
        + p_p->c0106y * pow_u_1   * pow_v_6  
        + p_p->c0107y * pow_u_1   * pow_v_7  
        + p_p->c0200y * pow_u_2   * pow_v_0  
        + p_p->c0201y * pow_u_2   * pow_v_1  
        + p_p->c0202y * pow_u_2   * pow_v_2  
        + p_p->c0203y * pow_u_2   * pow_v_3  
        + p_p->c0204y * pow_u_2   * pow_v_4  
        + p_p->c0205y * pow_u_2   * pow_v_5  
        + p_p->c0206y * pow_u_2   * pow_v_6  
        + p_p->c0207y * pow_u_2   * pow_v_7  
        + p_p->c0300y * pow_u_3   * pow_v_0  
        + p_p->c0301y * pow_u_3   * pow_v_1  
        + p_p->c0302y * pow_u_3   * pow_v_2  
        + p_p->c0303y * pow_u_3   * pow_v_3  
        + p_p->c0304y * pow_u_3   * pow_v_4  
        + p_p->c0305y * pow_u_3   * pow_v_5  
        + p_p->c0306y * pow_u_3   * pow_v_6  
        + p_p->c0307y * pow_u_3   * pow_v_7  
        + p_p->c0400y * pow_u_4   * pow_v_0  
        + p_p->c0401y * pow_u_4   * pow_v_1  
        + p_p->c0402y * pow_u_4   * pow_v_2  
        + p_p->c0403y * pow_u_4   * pow_v_3  
        + p_p->c0404y * pow_u_4   * pow_v_4  
        + p_p->c0405y * pow_u_4   * pow_v_5  
        + p_p->c0406y * pow_u_4   * pow_v_6  
        + p_p->c0407y * pow_u_4   * pow_v_7  
        + p_p->c0500y * pow_u_5   * pow_v_0  
        + p_p->c0501y * pow_u_5   * pow_v_1  
        + p_p->c0502y * pow_u_5   * pow_v_2  
        + p_p->c0503y * pow_u_5   * pow_v_3  
        + p_p->c0504y * pow_u_5   * pow_v_4  
        + p_p->c0505y * pow_u_5   * pow_v_5  
        + p_p->c0506y * pow_u_5   * pow_v_6  
        + p_p->c0507y * pow_u_5   * pow_v_7  
        + p_p->c0600y * pow_u_6   * pow_v_0  
        + p_p->c0601y * pow_u_6   * pow_v_1  
        + p_p->c0602y * pow_u_6   * pow_v_2  
        + p_p->c0603y * pow_u_6   * pow_v_3  
        + p_p->c0604y * pow_u_6   * pow_v_4  
        + p_p->c0605y * pow_u_6   * pow_v_5  
        + p_p->c0606y * pow_u_6   * pow_v_6  
        + p_p->c0607y * pow_u_6   * pow_v_7  
        + p_p->c0700y * pow_u_7   * pow_v_0  
        + p_p->c0701y * pow_u_7   * pow_v_1  
        + p_p->c0702y * pow_u_7   * pow_v_2  
        + p_p->c0703y * pow_u_7   * pow_v_3  
        + p_p->c0704y * pow_u_7   * pow_v_4  
        + p_p->c0705y * pow_u_7   * pow_v_5  
        + p_p->c0706y * pow_u_7   * pow_v_6  
        + p_p->c0707y * pow_u_7   * pow_v_7  ;

r_z     = p_p->c0000z * pow_u_0   * pow_v_0  
        + p_p->c0001z * pow_u_0   * pow_v_1  
        + p_p->c0002z * pow_u_0   * pow_v_2  
        + p_p->c0003z * pow_u_0   * pow_v_3  
        + p_p->c0004z * pow_u_0   * pow_v_4  
        + p_p->c0005z * pow_u_0   * pow_v_5  
        + p_p->c0006z * pow_u_0   * pow_v_6  
        + p_p->c0007z * pow_u_0   * pow_v_7  
        + p_p->c0100z * pow_u_1   * pow_v_0  
        + p_p->c0101z * pow_u_1   * pow_v_1  
        + p_p->c0102z * pow_u_1   * pow_v_2  
        + p_p->c0103z * pow_u_1   * pow_v_3  
        + p_p->c0104z * pow_u_1   * pow_v_4  
        + p_p->c0105z * pow_u_1   * pow_v_5  
        + p_p->c0106z * pow_u_1   * pow_v_6  
        + p_p->c0107z * pow_u_1   * pow_v_7  
        + p_p->c0200z * pow_u_2   * pow_v_0  
        + p_p->c0201z * pow_u_2   * pow_v_1  
        + p_p->c0202z * pow_u_2   * pow_v_2  
        + p_p->c0203z * pow_u_2   * pow_v_3  
        + p_p->c0204z * pow_u_2   * pow_v_4  
        + p_p->c0205z * pow_u_2   * pow_v_5  
        + p_p->c0206z * pow_u_2   * pow_v_6  
        + p_p->c0207z * pow_u_2   * pow_v_7  
        + p_p->c0300z * pow_u_3   * pow_v_0  
        + p_p->c0301z * pow_u_3   * pow_v_1  
        + p_p->c0302z * pow_u_3   * pow_v_2  
        + p_p->c0303z * pow_u_3   * pow_v_3  
        + p_p->c0304z * pow_u_3   * pow_v_4  
        + p_p->c0305z * pow_u_3   * pow_v_5  
        + p_p->c0306z * pow_u_3   * pow_v_6  
        + p_p->c0307z * pow_u_3   * pow_v_7  
        + p_p->c0400z * pow_u_4   * pow_v_0  
        + p_p->c0401z * pow_u_4   * pow_v_1  
        + p_p->c0402z * pow_u_4   * pow_v_2  
        + p_p->c0403z * pow_u_4   * pow_v_3  
        + p_p->c0404z * pow_u_4   * pow_v_4  
        + p_p->c0405z * pow_u_4   * pow_v_5  
        + p_p->c0406z * pow_u_4   * pow_v_6  
        + p_p->c0407z * pow_u_4   * pow_v_7  
        + p_p->c0500z * pow_u_5   * pow_v_0  
        + p_p->c0501z * pow_u_5   * pow_v_1  
        + p_p->c0502z * pow_u_5   * pow_v_2  
        + p_p->c0503z * pow_u_5   * pow_v_3  
        + p_p->c0504z * pow_u_5   * pow_v_4  
        + p_p->c0505z * pow_u_5   * pow_v_5  
        + p_p->c0506z * pow_u_5   * pow_v_6  
        + p_p->c0507z * pow_u_5   * pow_v_7  
        + p_p->c0600z * pow_u_6   * pow_v_0  
        + p_p->c0601z * pow_u_6   * pow_v_1  
        + p_p->c0602z * pow_u_6   * pow_v_2  
        + p_p->c0603z * pow_u_6   * pow_v_3  
        + p_p->c0604z * pow_u_6   * pow_v_4  
        + p_p->c0605z * pow_u_6   * pow_v_5  
        + p_p->c0606z * pow_u_6   * pow_v_6  
        + p_p->c0607z * pow_u_6   * pow_v_7  
        + p_p->c0700z * pow_u_7   * pow_v_0  
        + p_p->c0701z * pow_u_7   * pow_v_1  
        + p_p->c0702z * pow_u_7   * pow_v_2  
        + p_p->c0703z * pow_u_7   * pow_v_3  
        + p_p->c0704z * pow_u_7   * pow_v_4  
        + p_p->c0705z * pow_u_7   * pow_v_5  
        + p_p->c0706z * pow_u_7   * pow_v_6  
        + p_p->c0707z * pow_u_7   * pow_v_7  ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur502*c_coord Exit \n");
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
/* The function calculates derivatives dr/du for GMPATP7            */

   static short c_drdu  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP7  *p_p;         /* Patch GMPATP7                     (ptr) */
   DBfloat   u_l;         /* Patch (local) U parameter value         */
   DBfloat   v_l;         /* Patch (local) V parameter value         */

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
"sur502*c_drdu  p_p= %d  u_l= %f v_l= %f\n"
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
        + p_p->c0100x* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101x* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102x* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103x* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104x* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105x* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106x* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107x* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0200x* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201x* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202x* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203x* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204x* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205x* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206x* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207x* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0300x* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301x* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302x* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303x* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304x* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305x* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306x* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307x* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0400x* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401x* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402x* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403x* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404x* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405x* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406x* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407x* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0500x* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501x* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502x* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503x* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504x* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505x* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506x* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507x* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0600x* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601x* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602x* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603x* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604x* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605x* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606x* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607x* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0700x* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701x* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702x* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703x* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704x* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705x* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706x* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707x* 7.0*pow_u_6 * pow_v_7  ;

u_y     = p_p->c0000y*         0.0         * pow_v_0  
        + p_p->c0001y*         0.0         * pow_v_1  
        + p_p->c0002y*         0.0         * pow_v_2  
        + p_p->c0003y*         0.0         * pow_v_3  
        + p_p->c0004y*         0.0         * pow_v_4  
        + p_p->c0005y*         0.0         * pow_v_5  
        + p_p->c0006y*         0.0         * pow_v_6  
        + p_p->c0007y*         0.0         * pow_v_7  
        + p_p->c0100y* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101y* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102y* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103y* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104y* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105y* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106y* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107y* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0200y* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201y* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202y* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203y* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204y* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205y* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206y* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207y* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0300y* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301y* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302y* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303y* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304y* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305y* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306y* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307y* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0400y* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401y* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402y* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403y* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404y* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405y* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406y* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407y* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0500y* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501y* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502y* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503y* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504y* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505y* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506y* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507y* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0600y* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601y* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602y* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603y* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604y* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605y* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606y* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607y* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0700y* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701y* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702y* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703y* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704y* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705y* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706y* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707y* 7.0*pow_u_6 * pow_v_7  ;

u_z     = p_p->c0000z*         0.0         * pow_v_0  
        + p_p->c0001z*         0.0         * pow_v_1  
        + p_p->c0002z*         0.0         * pow_v_2  
        + p_p->c0003z*         0.0         * pow_v_3  
        + p_p->c0004z*         0.0         * pow_v_4  
        + p_p->c0005z*         0.0         * pow_v_5  
        + p_p->c0006z*         0.0         * pow_v_6  
        + p_p->c0007z*         0.0         * pow_v_7  
        + p_p->c0100z* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101z* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102z* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103z* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0104z* 1.0*pow_u_0 * pow_v_4  
        + p_p->c0105z* 1.0*pow_u_0 * pow_v_5  
        + p_p->c0106z* 1.0*pow_u_0 * pow_v_6  
        + p_p->c0107z* 1.0*pow_u_0 * pow_v_7  
        + p_p->c0200z* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201z* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202z* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203z* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0204z* 2.0*pow_u_1 * pow_v_4  
        + p_p->c0205z* 2.0*pow_u_1 * pow_v_5  
        + p_p->c0206z* 2.0*pow_u_1 * pow_v_6  
        + p_p->c0207z* 2.0*pow_u_1 * pow_v_7  
        + p_p->c0300z* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301z* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302z* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303z* 3.0*pow_u_2 * pow_v_3  
        + p_p->c0304z* 3.0*pow_u_2 * pow_v_4  
        + p_p->c0305z* 3.0*pow_u_2 * pow_v_5  
        + p_p->c0306z* 3.0*pow_u_2 * pow_v_6  
        + p_p->c0307z* 3.0*pow_u_2 * pow_v_7  
        + p_p->c0400z* 4.0*pow_u_3 * pow_v_0  
        + p_p->c0401z* 4.0*pow_u_3 * pow_v_1  
        + p_p->c0402z* 4.0*pow_u_3 * pow_v_2  
        + p_p->c0403z* 4.0*pow_u_3 * pow_v_3  
        + p_p->c0404z* 4.0*pow_u_3 * pow_v_4  
        + p_p->c0405z* 4.0*pow_u_3 * pow_v_5  
        + p_p->c0406z* 4.0*pow_u_3 * pow_v_6  
        + p_p->c0407z* 4.0*pow_u_3 * pow_v_7  
        + p_p->c0500z* 5.0*pow_u_4 * pow_v_0  
        + p_p->c0501z* 5.0*pow_u_4 * pow_v_1  
        + p_p->c0502z* 5.0*pow_u_4 * pow_v_2  
        + p_p->c0503z* 5.0*pow_u_4 * pow_v_3  
        + p_p->c0504z* 5.0*pow_u_4 * pow_v_4  
        + p_p->c0505z* 5.0*pow_u_4 * pow_v_5  
        + p_p->c0506z* 5.0*pow_u_4 * pow_v_6  
        + p_p->c0507z* 5.0*pow_u_4 * pow_v_7  
        + p_p->c0600z* 6.0*pow_u_5 * pow_v_0  
        + p_p->c0601z* 6.0*pow_u_5 * pow_v_1  
        + p_p->c0602z* 6.0*pow_u_5 * pow_v_2  
        + p_p->c0603z* 6.0*pow_u_5 * pow_v_3  
        + p_p->c0604z* 6.0*pow_u_5 * pow_v_4  
        + p_p->c0605z* 6.0*pow_u_5 * pow_v_5  
        + p_p->c0606z* 6.0*pow_u_5 * pow_v_6  
        + p_p->c0607z* 6.0*pow_u_5 * pow_v_7  
        + p_p->c0700z* 7.0*pow_u_6 * pow_v_0  
        + p_p->c0701z* 7.0*pow_u_6 * pow_v_1  
        + p_p->c0702z* 7.0*pow_u_6 * pow_v_2  
        + p_p->c0703z* 7.0*pow_u_6 * pow_v_3  
        + p_p->c0704z* 7.0*pow_u_6 * pow_v_4  
        + p_p->c0705z* 7.0*pow_u_6 * pow_v_5  
        + p_p->c0706z* 7.0*pow_u_6 * pow_v_6  
        + p_p->c0707z* 7.0*pow_u_6 * pow_v_7  ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur502*c_drdu  Exit \n");
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
/* The function calculates derivatives dr/dv for GMPATP7            */

   static short c_drdv  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP7  *p_p;         /* Patch GMPATP7                     (ptr) */
   DBfloat   u_l;         /* Patch (local) U parameter value         */
   DBfloat   v_l;         /* Patch (local) V parameter value         */

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
"sur502*c_drdv  p_p= %d  u_l= %f v_l= %f\n"
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
        + p_p->c0100x * pow_u_1  *         0.0        
        + p_p->c0101x * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102x * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103x * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104x * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105x * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106x * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107x * pow_u_1  * 7.0*pow_v_6
        + p_p->c0200x * pow_u_2  *         0.0        
        + p_p->c0201x * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202x * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203x * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204x * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205x * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206x * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207x * pow_u_2  * 7.0*pow_v_6
        + p_p->c0300x * pow_u_3  *         0.0        
        + p_p->c0301x * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302x * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303x * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304x * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305x * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306x * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307x * pow_u_3  * 7.0*pow_v_6
        + p_p->c0400x * pow_u_4  *         0.0        
        + p_p->c0401x * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402x * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403x * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404x * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405x * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406x * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407x * pow_u_4  * 7.0*pow_v_6
        + p_p->c0500x * pow_u_5  *         0.0        
        + p_p->c0501x * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502x * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503x * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504x * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505x * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506x * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507x * pow_u_5  * 7.0*pow_v_6
        + p_p->c0600x * pow_u_6  *         0.0        
        + p_p->c0601x * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602x * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603x * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604x * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605x * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606x * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607x * pow_u_6  * 7.0*pow_v_6
        + p_p->c0700x * pow_u_7  *         0.0        
        + p_p->c0701x * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702x * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703x * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704x * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705x * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706x * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707x * pow_u_7  * 7.0*pow_v_6;

v_y     = p_p->c0000y * pow_u_0  *         0.0        
        + p_p->c0001y * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002y * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003y * pow_u_0  * 3.0*pow_v_2
        + p_p->c0004y * pow_u_0  * 4.0*pow_v_3
        + p_p->c0005y * pow_u_0  * 5.0*pow_v_4
        + p_p->c0006y * pow_u_0  * 6.0*pow_v_5
        + p_p->c0007y * pow_u_0  * 7.0*pow_v_6
        + p_p->c0100y * pow_u_1  *         0.0        
        + p_p->c0101y * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102y * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103y * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104y * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105y * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106y * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107y * pow_u_1  * 7.0*pow_v_6
        + p_p->c0200y * pow_u_2  *         0.0        
        + p_p->c0201y * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202y * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203y * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204y * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205y * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206y * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207y * pow_u_2  * 7.0*pow_v_6
        + p_p->c0300y * pow_u_3  *         0.0        
        + p_p->c0301y * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302y * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303y * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304y * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305y * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306y * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307y * pow_u_3  * 7.0*pow_v_6
        + p_p->c0400y * pow_u_4  *         0.0        
        + p_p->c0401y * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402y * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403y * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404y * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405y * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406y * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407y * pow_u_4  * 7.0*pow_v_6
        + p_p->c0500y * pow_u_5  *         0.0        
        + p_p->c0501y * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502y * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503y * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504y * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505y * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506y * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507y * pow_u_5  * 7.0*pow_v_6
        + p_p->c0600y * pow_u_6  *         0.0        
        + p_p->c0601y * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602y * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603y * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604y * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605y * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606y * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607y * pow_u_6  * 7.0*pow_v_6
        + p_p->c0700y * pow_u_7  *         0.0        
        + p_p->c0701y * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702y * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703y * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704y * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705y * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706y * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707y * pow_u_7  * 7.0*pow_v_6;

v_z     = p_p->c0000z * pow_u_0  *         0.0        
        + p_p->c0001z * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002z * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003z * pow_u_0  * 3.0*pow_v_2
        + p_p->c0004z * pow_u_0  * 4.0*pow_v_3
        + p_p->c0005z * pow_u_0  * 5.0*pow_v_4
        + p_p->c0006z * pow_u_0  * 6.0*pow_v_5
        + p_p->c0007z * pow_u_0  * 7.0*pow_v_6
        + p_p->c0100z * pow_u_1  *         0.0        
        + p_p->c0101z * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102z * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103z * pow_u_1  * 3.0*pow_v_2
        + p_p->c0104z * pow_u_1  * 4.0*pow_v_3
        + p_p->c0105z * pow_u_1  * 5.0*pow_v_4
        + p_p->c0106z * pow_u_1  * 6.0*pow_v_5
        + p_p->c0107z * pow_u_1  * 7.0*pow_v_6
        + p_p->c0200z * pow_u_2  *         0.0        
        + p_p->c0201z * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202z * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203z * pow_u_2  * 3.0*pow_v_2
        + p_p->c0204z * pow_u_2  * 4.0*pow_v_3
        + p_p->c0205z * pow_u_2  * 5.0*pow_v_4
        + p_p->c0206z * pow_u_2  * 6.0*pow_v_5
        + p_p->c0207z * pow_u_2  * 7.0*pow_v_6
        + p_p->c0300z * pow_u_3  *         0.0        
        + p_p->c0301z * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302z * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303z * pow_u_3  * 3.0*pow_v_2
        + p_p->c0304z * pow_u_3  * 4.0*pow_v_3
        + p_p->c0305z * pow_u_3  * 5.0*pow_v_4
        + p_p->c0306z * pow_u_3  * 6.0*pow_v_5
        + p_p->c0307z * pow_u_3  * 7.0*pow_v_6
        + p_p->c0400z * pow_u_4  *         0.0        
        + p_p->c0401z * pow_u_4  * 1.0*pow_v_0
        + p_p->c0402z * pow_u_4  * 2.0*pow_v_1
        + p_p->c0403z * pow_u_4  * 3.0*pow_v_2
        + p_p->c0404z * pow_u_4  * 4.0*pow_v_3
        + p_p->c0405z * pow_u_4  * 5.0*pow_v_4
        + p_p->c0406z * pow_u_4  * 6.0*pow_v_5
        + p_p->c0407z * pow_u_4  * 7.0*pow_v_6
        + p_p->c0500z * pow_u_5  *         0.0        
        + p_p->c0501z * pow_u_5  * 1.0*pow_v_0
        + p_p->c0502z * pow_u_5  * 2.0*pow_v_1
        + p_p->c0503z * pow_u_5  * 3.0*pow_v_2
        + p_p->c0504z * pow_u_5  * 4.0*pow_v_3
        + p_p->c0505z * pow_u_5  * 5.0*pow_v_4
        + p_p->c0506z * pow_u_5  * 6.0*pow_v_5
        + p_p->c0507z * pow_u_5  * 7.0*pow_v_6
        + p_p->c0600z * pow_u_6  *         0.0        
        + p_p->c0601z * pow_u_6  * 1.0*pow_v_0
        + p_p->c0602z * pow_u_6  * 2.0*pow_v_1
        + p_p->c0603z * pow_u_6  * 3.0*pow_v_2
        + p_p->c0604z * pow_u_6  * 4.0*pow_v_3
        + p_p->c0605z * pow_u_6  * 5.0*pow_v_4
        + p_p->c0606z * pow_u_6  * 6.0*pow_v_5
        + p_p->c0607z * pow_u_6  * 7.0*pow_v_6
        + p_p->c0700z * pow_u_7  *         0.0        
        + p_p->c0701z * pow_u_7  * 1.0*pow_v_0
        + p_p->c0702z * pow_u_7  * 2.0*pow_v_1
        + p_p->c0703z * pow_u_7  * 3.0*pow_v_2
        + p_p->c0704z * pow_u_7  * 4.0*pow_v_3
        + p_p->c0705z * pow_u_7  * 5.0*pow_v_4
        + p_p->c0706z * pow_u_7  * 6.0*pow_v_5
        + p_p->c0707z * pow_u_7  * 7.0*pow_v_6;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur502*c_drdv  Exit \n");
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
/* The function calculates the second derivatives for GMPATP7       */

   static short c_second ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP7  *p_p;         /* Patch GMPATP7                     (ptr) */
   DBfloat   u_l;         /* Patch (local) U parameter value         */
   DBfloat   v_l;         /* Patch (local) V parameter value         */

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
"sur502*c_second p_p= %d  u_l= %f v_l= %f\n"
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
        + p_p->c0100x*            0.0           * pow_v_0  
        + p_p->c0101x*            0.0           * pow_v_1  
        + p_p->c0102x*            0.0           * pow_v_2  
        + p_p->c0103x*            0.0           * pow_v_3  
        + p_p->c0104x*            0.0           * pow_v_4  
        + p_p->c0105x*            0.0           * pow_v_5  
        + p_p->c0106x*            0.0           * pow_v_6  
        + p_p->c0107x*            0.0           * pow_v_7  
        + p_p->c0200x* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201x* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202x* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203x* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204x* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205x* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206x* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207x* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0300x* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301x* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302x* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303x* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304x* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305x* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306x* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307x* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0400x* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401x* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402x* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403x* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404x* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405x* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406x* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407x* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0500x* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501x* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502x* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503x* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504x* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505x* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506x* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507x* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0600x* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601x* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602x* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603x* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604x* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605x* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606x* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607x* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0700x* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701x* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702x* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703x* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704x* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705x* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706x* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707x* 6.0* 7.0*pow_u_5 * pow_v_7  ;

u2_y    = p_p->c0000y*            0.0           * pow_v_0  
        + p_p->c0001y*            0.0           * pow_v_1  
        + p_p->c0002y*            0.0           * pow_v_2  
        + p_p->c0003y*            0.0           * pow_v_3  
        + p_p->c0004y*            0.0           * pow_v_4  
        + p_p->c0005y*            0.0           * pow_v_5  
        + p_p->c0006y*            0.0           * pow_v_6  
        + p_p->c0007y*            0.0           * pow_v_7  
        + p_p->c0100y*            0.0           * pow_v_0  
        + p_p->c0101y*            0.0           * pow_v_1  
        + p_p->c0102y*            0.0           * pow_v_2  
        + p_p->c0103y*            0.0           * pow_v_3  
        + p_p->c0104y*            0.0           * pow_v_4  
        + p_p->c0105y*            0.0           * pow_v_5  
        + p_p->c0106y*            0.0           * pow_v_6  
        + p_p->c0107y*            0.0           * pow_v_7  
        + p_p->c0200y* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201y* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202y* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203y* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204y* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205y* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206y* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207y* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0300y* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301y* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302y* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303y* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304y* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305y* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306y* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307y* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0400y* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401y* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402y* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403y* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404y* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405y* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406y* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407y* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0500y* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501y* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502y* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503y* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504y* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505y* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506y* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507y* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0600y* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601y* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602y* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603y* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604y* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605y* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606y* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607y* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0700y* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701y* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702y* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703y* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704y* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705y* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706y* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707y* 6.0* 7.0*pow_u_5 * pow_v_7  ;

u2_z    = p_p->c0000z*            0.0           * pow_v_0  
        + p_p->c0001z*            0.0           * pow_v_1  
        + p_p->c0002z*            0.0           * pow_v_2  
        + p_p->c0003z*            0.0           * pow_v_3  
        + p_p->c0004z*            0.0           * pow_v_4  
        + p_p->c0005z*            0.0           * pow_v_5  
        + p_p->c0006z*            0.0           * pow_v_6  
        + p_p->c0007z*            0.0           * pow_v_7  
        + p_p->c0100z*            0.0           * pow_v_0  
        + p_p->c0101z*            0.0           * pow_v_1  
        + p_p->c0102z*            0.0           * pow_v_2  
        + p_p->c0103z*            0.0           * pow_v_3  
        + p_p->c0104z*            0.0           * pow_v_4  
        + p_p->c0105z*            0.0           * pow_v_5  
        + p_p->c0106z*            0.0           * pow_v_6  
        + p_p->c0107z*            0.0           * pow_v_7  
        + p_p->c0200z* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201z* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202z* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203z* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0204z* 1.0* 2.0*pow_u_0 * pow_v_4  
        + p_p->c0205z* 1.0* 2.0*pow_u_0 * pow_v_5  
        + p_p->c0206z* 1.0* 2.0*pow_u_0 * pow_v_6  
        + p_p->c0207z* 1.0* 2.0*pow_u_0 * pow_v_7  
        + p_p->c0300z* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301z* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302z* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303z* 2.0* 3.0*pow_u_1 * pow_v_3  
        + p_p->c0304z* 2.0* 3.0*pow_u_1 * pow_v_4  
        + p_p->c0305z* 2.0* 3.0*pow_u_1 * pow_v_5  
        + p_p->c0306z* 2.0* 3.0*pow_u_1 * pow_v_6  
        + p_p->c0307z* 2.0* 3.0*pow_u_1 * pow_v_7  
        + p_p->c0400z* 3.0* 4.0*pow_u_2 * pow_v_0  
        + p_p->c0401z* 3.0* 4.0*pow_u_2 * pow_v_1  
        + p_p->c0402z* 3.0* 4.0*pow_u_2 * pow_v_2  
        + p_p->c0403z* 3.0* 4.0*pow_u_2 * pow_v_3  
        + p_p->c0404z* 3.0* 4.0*pow_u_2 * pow_v_4  
        + p_p->c0405z* 3.0* 4.0*pow_u_2 * pow_v_5  
        + p_p->c0406z* 3.0* 4.0*pow_u_2 * pow_v_6  
        + p_p->c0407z* 3.0* 4.0*pow_u_2 * pow_v_7  
        + p_p->c0500z* 4.0* 5.0*pow_u_3 * pow_v_0  
        + p_p->c0501z* 4.0* 5.0*pow_u_3 * pow_v_1  
        + p_p->c0502z* 4.0* 5.0*pow_u_3 * pow_v_2  
        + p_p->c0503z* 4.0* 5.0*pow_u_3 * pow_v_3  
        + p_p->c0504z* 4.0* 5.0*pow_u_3 * pow_v_4  
        + p_p->c0505z* 4.0* 5.0*pow_u_3 * pow_v_5  
        + p_p->c0506z* 4.0* 5.0*pow_u_3 * pow_v_6  
        + p_p->c0507z* 4.0* 5.0*pow_u_3 * pow_v_7  
        + p_p->c0600z* 5.0* 6.0*pow_u_4 * pow_v_0  
        + p_p->c0601z* 5.0* 6.0*pow_u_4 * pow_v_1  
        + p_p->c0602z* 5.0* 6.0*pow_u_4 * pow_v_2  
        + p_p->c0603z* 5.0* 6.0*pow_u_4 * pow_v_3  
        + p_p->c0604z* 5.0* 6.0*pow_u_4 * pow_v_4  
        + p_p->c0605z* 5.0* 6.0*pow_u_4 * pow_v_5  
        + p_p->c0606z* 5.0* 6.0*pow_u_4 * pow_v_6  
        + p_p->c0607z* 5.0* 6.0*pow_u_4 * pow_v_7  
        + p_p->c0700z* 6.0* 7.0*pow_u_5 * pow_v_0  
        + p_p->c0701z* 6.0* 7.0*pow_u_5 * pow_v_1  
        + p_p->c0702z* 6.0* 7.0*pow_u_5 * pow_v_2  
        + p_p->c0703z* 6.0* 7.0*pow_u_5 * pow_v_3  
        + p_p->c0704z* 6.0* 7.0*pow_u_5 * pow_v_4  
        + p_p->c0705z* 6.0* 7.0*pow_u_5 * pow_v_5  
        + p_p->c0706z* 6.0* 7.0*pow_u_5 * pow_v_6  
        + p_p->c0707z* 6.0* 7.0*pow_u_5 * pow_v_7  ;

v2_x    = p_p->c0000x * pow_u_0  *            0.0          
        + p_p->c0001x * pow_u_0  *            0.0          
        + p_p->c0002x * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003x * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004x * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005x * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006x * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007x * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0100x * pow_u_1  *            0.0          
        + p_p->c0101x * pow_u_1  *            0.0          
        + p_p->c0102x * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103x * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104x * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105x * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106x * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107x * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0200x * pow_u_2  *            0.0          
        + p_p->c0201x * pow_u_2  *            0.0          
        + p_p->c0202x * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203x * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204x * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205x * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206x * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207x * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0300x * pow_u_3  *            0.0          
        + p_p->c0301x * pow_u_3  *            0.0          
        + p_p->c0302x * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303x * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304x * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305x * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306x * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307x * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0400x * pow_u_4  *            0.0          
        + p_p->c0401x * pow_u_4  *            0.0          
        + p_p->c0402x * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403x * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404x * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405x * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406x * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407x * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0500x * pow_u_5  *            0.0          
        + p_p->c0501x * pow_u_5  *            0.0          
        + p_p->c0502x * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503x * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504x * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505x * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506x * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507x * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0600x * pow_u_6  *            0.0          
        + p_p->c0601x * pow_u_6  *            0.0          
        + p_p->c0602x * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603x * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604x * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605x * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606x * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607x * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0700x * pow_u_7  *            0.0          
        + p_p->c0701x * pow_u_7  *            0.0          
        + p_p->c0702x * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703x * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704x * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705x * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706x * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707x * pow_u_7  * 6.0* 7.0*pow_v_5;

v2_y    = p_p->c0000y * pow_u_0  *            0.0          
        + p_p->c0001y * pow_u_0  *            0.0          
        + p_p->c0002y * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003y * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004y * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005y * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006y * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007y * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0100y * pow_u_1  *            0.0          
        + p_p->c0101y * pow_u_1  *            0.0          
        + p_p->c0102y * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103y * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104y * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105y * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106y * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107y * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0200y * pow_u_2  *            0.0          
        + p_p->c0201y * pow_u_2  *            0.0          
        + p_p->c0202y * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203y * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204y * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205y * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206y * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207y * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0300y * pow_u_3  *            0.0          
        + p_p->c0301y * pow_u_3  *            0.0          
        + p_p->c0302y * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303y * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304y * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305y * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306y * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307y * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0400y * pow_u_4  *            0.0          
        + p_p->c0401y * pow_u_4  *            0.0          
        + p_p->c0402y * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403y * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404y * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405y * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406y * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407y * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0500y * pow_u_5  *            0.0          
        + p_p->c0501y * pow_u_5  *            0.0          
        + p_p->c0502y * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503y * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504y * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505y * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506y * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507y * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0600y * pow_u_6  *            0.0          
        + p_p->c0601y * pow_u_6  *            0.0          
        + p_p->c0602y * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603y * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604y * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605y * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606y * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607y * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0700y * pow_u_7  *            0.0          
        + p_p->c0701y * pow_u_7  *            0.0          
        + p_p->c0702y * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703y * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704y * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705y * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706y * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707y * pow_u_7  * 6.0* 7.0*pow_v_5;

v2_z    = p_p->c0000z * pow_u_0  *            0.0          
        + p_p->c0001z * pow_u_0  *            0.0          
        + p_p->c0002z * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003z * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0004z * pow_u_0  * 3.0* 4.0*pow_v_2
        + p_p->c0005z * pow_u_0  * 4.0* 5.0*pow_v_3
        + p_p->c0006z * pow_u_0  * 5.0* 6.0*pow_v_4
        + p_p->c0007z * pow_u_0  * 6.0* 7.0*pow_v_5
        + p_p->c0100z * pow_u_1  *            0.0          
        + p_p->c0101z * pow_u_1  *            0.0          
        + p_p->c0102z * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103z * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0104z * pow_u_1  * 3.0* 4.0*pow_v_2
        + p_p->c0105z * pow_u_1  * 4.0* 5.0*pow_v_3
        + p_p->c0106z * pow_u_1  * 5.0* 6.0*pow_v_4
        + p_p->c0107z * pow_u_1  * 6.0* 7.0*pow_v_5
        + p_p->c0200z * pow_u_2  *            0.0          
        + p_p->c0201z * pow_u_2  *            0.0          
        + p_p->c0202z * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203z * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0204z * pow_u_2  * 3.0* 4.0*pow_v_2
        + p_p->c0205z * pow_u_2  * 4.0* 5.0*pow_v_3
        + p_p->c0206z * pow_u_2  * 5.0* 6.0*pow_v_4
        + p_p->c0207z * pow_u_2  * 6.0* 7.0*pow_v_5
        + p_p->c0300z * pow_u_3  *            0.0          
        + p_p->c0301z * pow_u_3  *            0.0          
        + p_p->c0302z * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303z * pow_u_3  * 2.0* 3.0*pow_v_1
        + p_p->c0304z * pow_u_3  * 3.0* 4.0*pow_v_2
        + p_p->c0305z * pow_u_3  * 4.0* 5.0*pow_v_3
        + p_p->c0306z * pow_u_3  * 5.0* 6.0*pow_v_4
        + p_p->c0307z * pow_u_3  * 6.0* 7.0*pow_v_5
        + p_p->c0400z * pow_u_4  *            0.0          
        + p_p->c0401z * pow_u_4  *            0.0          
        + p_p->c0402z * pow_u_4  * 1.0* 2.0*pow_v_0
        + p_p->c0403z * pow_u_4  * 2.0* 3.0*pow_v_1
        + p_p->c0404z * pow_u_4  * 3.0* 4.0*pow_v_2
        + p_p->c0405z * pow_u_4  * 4.0* 5.0*pow_v_3
        + p_p->c0406z * pow_u_4  * 5.0* 6.0*pow_v_4
        + p_p->c0407z * pow_u_4  * 6.0* 7.0*pow_v_5
        + p_p->c0500z * pow_u_5  *            0.0          
        + p_p->c0501z * pow_u_5  *            0.0          
        + p_p->c0502z * pow_u_5  * 1.0* 2.0*pow_v_0
        + p_p->c0503z * pow_u_5  * 2.0* 3.0*pow_v_1
        + p_p->c0504z * pow_u_5  * 3.0* 4.0*pow_v_2
        + p_p->c0505z * pow_u_5  * 4.0* 5.0*pow_v_3
        + p_p->c0506z * pow_u_5  * 5.0* 6.0*pow_v_4
        + p_p->c0507z * pow_u_5  * 6.0* 7.0*pow_v_5
        + p_p->c0600z * pow_u_6  *            0.0          
        + p_p->c0601z * pow_u_6  *            0.0          
        + p_p->c0602z * pow_u_6  * 1.0* 2.0*pow_v_0
        + p_p->c0603z * pow_u_6  * 2.0* 3.0*pow_v_1
        + p_p->c0604z * pow_u_6  * 3.0* 4.0*pow_v_2
        + p_p->c0605z * pow_u_6  * 4.0* 5.0*pow_v_3
        + p_p->c0606z * pow_u_6  * 5.0* 6.0*pow_v_4
        + p_p->c0607z * pow_u_6  * 6.0* 7.0*pow_v_5
        + p_p->c0700z * pow_u_7  *            0.0          
        + p_p->c0701z * pow_u_7  *            0.0          
        + p_p->c0702z * pow_u_7  * 1.0* 2.0*pow_v_0
        + p_p->c0703z * pow_u_7  * 2.0* 3.0*pow_v_1
        + p_p->c0704z * pow_u_7  * 3.0* 4.0*pow_v_2
        + p_p->c0705z * pow_u_7  * 4.0* 5.0*pow_v_3
        + p_p->c0706z * pow_u_7  * 5.0* 6.0*pow_v_4
        + p_p->c0707z * pow_u_7  * 6.0* 7.0*pow_v_5;

uv_x    = p_p->c0000x*         0.0        *         0.0        
        + p_p->c0001x*         0.0        * 1.0*pow_v_0
        + p_p->c0002x*         0.0        * 2.0*pow_v_1
        + p_p->c0003x*         0.0        * 3.0*pow_v_2
        + p_p->c0004x*         0.0        * 4.0*pow_v_3
        + p_p->c0005x*         0.0        * 5.0*pow_v_4
        + p_p->c0006x*         0.0        * 6.0*pow_v_5
        + p_p->c0007x*         0.0        * 7.0*pow_v_6
        + p_p->c0100x* 1.0*pow_u_0*         0.0        
        + p_p->c0101x* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102x* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103x* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104x* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105x* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106x* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107x* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0200x* 2.0*pow_u_1*         0.0        
        + p_p->c0201x* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202x* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203x* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204x* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205x* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206x* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207x* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0300x* 3.0*pow_u_2*         0.0        
        + p_p->c0301x* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302x* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303x* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304x* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305x* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306x* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307x* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0400x* 4.0*pow_u_3*         0.0        
        + p_p->c0401x* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402x* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403x* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404x* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405x* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406x* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407x* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0500x* 5.0*pow_u_4*         0.0        
        + p_p->c0501x* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502x* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503x* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504x* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505x* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506x* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507x* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0600x* 6.0*pow_u_5*         0.0        
        + p_p->c0601x* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602x* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603x* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604x* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605x* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606x* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607x* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0700x* 7.0*pow_u_6*         0.0        
        + p_p->c0701x* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702x* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703x* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704x* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705x* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706x* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707x* 7.0*pow_u_6* 7.0*pow_v_6;

uv_y    = p_p->c0000y*         0.0        *         0.0        
        + p_p->c0001y*         0.0        * 1.0*pow_v_0
        + p_p->c0002y*         0.0        * 2.0*pow_v_1
        + p_p->c0003y*         0.0        * 3.0*pow_v_2
        + p_p->c0004y*         0.0        * 4.0*pow_v_3
        + p_p->c0005y*         0.0        * 5.0*pow_v_4
        + p_p->c0006y*         0.0        * 6.0*pow_v_5
        + p_p->c0007y*         0.0        * 7.0*pow_v_6
        + p_p->c0100y* 1.0*pow_u_0*         0.0        
        + p_p->c0101y* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102y* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103y* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104y* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105y* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106y* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107y* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0200y* 2.0*pow_u_1*         0.0        
        + p_p->c0201y* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202y* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203y* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204y* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205y* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206y* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207y* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0300y* 3.0*pow_u_2*         0.0        
        + p_p->c0301y* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302y* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303y* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304y* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305y* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306y* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307y* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0400y* 4.0*pow_u_3*         0.0        
        + p_p->c0401y* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402y* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403y* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404y* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405y* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406y* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407y* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0500y* 5.0*pow_u_4*         0.0        
        + p_p->c0501y* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502y* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503y* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504y* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505y* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506y* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507y* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0600y* 6.0*pow_u_5*         0.0        
        + p_p->c0601y* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602y* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603y* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604y* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605y* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606y* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607y* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0700y* 7.0*pow_u_6*         0.0        
        + p_p->c0701y* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702y* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703y* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704y* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705y* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706y* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707y* 7.0*pow_u_6* 7.0*pow_v_6;

uv_z    = p_p->c0000z*         0.0        *         0.0        
        + p_p->c0001z*         0.0        * 1.0*pow_v_0
        + p_p->c0002z*         0.0        * 2.0*pow_v_1
        + p_p->c0003z*         0.0        * 3.0*pow_v_2
        + p_p->c0004z*         0.0        * 4.0*pow_v_3
        + p_p->c0005z*         0.0        * 5.0*pow_v_4
        + p_p->c0006z*         0.0        * 6.0*pow_v_5
        + p_p->c0007z*         0.0        * 7.0*pow_v_6
        + p_p->c0100z* 1.0*pow_u_0*         0.0        
        + p_p->c0101z* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102z* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103z* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0104z* 1.0*pow_u_0* 4.0*pow_v_3
        + p_p->c0105z* 1.0*pow_u_0* 5.0*pow_v_4
        + p_p->c0106z* 1.0*pow_u_0* 6.0*pow_v_5
        + p_p->c0107z* 1.0*pow_u_0* 7.0*pow_v_6
        + p_p->c0200z* 2.0*pow_u_1*         0.0        
        + p_p->c0201z* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202z* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203z* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0204z* 2.0*pow_u_1* 4.0*pow_v_3
        + p_p->c0205z* 2.0*pow_u_1* 5.0*pow_v_4
        + p_p->c0206z* 2.0*pow_u_1* 6.0*pow_v_5
        + p_p->c0207z* 2.0*pow_u_1* 7.0*pow_v_6
        + p_p->c0300z* 3.0*pow_u_2*         0.0        
        + p_p->c0301z* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302z* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303z* 3.0*pow_u_2* 3.0*pow_v_2
        + p_p->c0304z* 3.0*pow_u_2* 4.0*pow_v_3
        + p_p->c0305z* 3.0*pow_u_2* 5.0*pow_v_4
        + p_p->c0306z* 3.0*pow_u_2* 6.0*pow_v_5
        + p_p->c0307z* 3.0*pow_u_2* 7.0*pow_v_6
        + p_p->c0400z* 4.0*pow_u_3*         0.0        
        + p_p->c0401z* 4.0*pow_u_3* 1.0*pow_v_0
        + p_p->c0402z* 4.0*pow_u_3* 2.0*pow_v_1
        + p_p->c0403z* 4.0*pow_u_3* 3.0*pow_v_2
        + p_p->c0404z* 4.0*pow_u_3* 4.0*pow_v_3
        + p_p->c0405z* 4.0*pow_u_3* 5.0*pow_v_4
        + p_p->c0406z* 4.0*pow_u_3* 6.0*pow_v_5
        + p_p->c0407z* 4.0*pow_u_3* 7.0*pow_v_6
        + p_p->c0500z* 5.0*pow_u_4*         0.0        
        + p_p->c0501z* 5.0*pow_u_4* 1.0*pow_v_0
        + p_p->c0502z* 5.0*pow_u_4* 2.0*pow_v_1
        + p_p->c0503z* 5.0*pow_u_4* 3.0*pow_v_2
        + p_p->c0504z* 5.0*pow_u_4* 4.0*pow_v_3
        + p_p->c0505z* 5.0*pow_u_4* 5.0*pow_v_4
        + p_p->c0506z* 5.0*pow_u_4* 6.0*pow_v_5
        + p_p->c0507z* 5.0*pow_u_4* 7.0*pow_v_6
        + p_p->c0600z* 6.0*pow_u_5*         0.0        
        + p_p->c0601z* 6.0*pow_u_5* 1.0*pow_v_0
        + p_p->c0602z* 6.0*pow_u_5* 2.0*pow_v_1
        + p_p->c0603z* 6.0*pow_u_5* 3.0*pow_v_2
        + p_p->c0604z* 6.0*pow_u_5* 4.0*pow_v_3
        + p_p->c0605z* 6.0*pow_u_5* 5.0*pow_v_4
        + p_p->c0606z* 6.0*pow_u_5* 6.0*pow_v_5
        + p_p->c0607z* 6.0*pow_u_5* 7.0*pow_v_6
        + p_p->c0700z* 7.0*pow_u_6*         0.0        
        + p_p->c0701z* 7.0*pow_u_6* 1.0*pow_v_0
        + p_p->c0702z* 7.0*pow_u_6* 2.0*pow_v_1
        + p_p->c0703z* 7.0*pow_u_6* 3.0*pow_v_2
        + p_p->c0704z* 7.0*pow_u_6* 4.0*pow_v_3
        + p_p->c0705z* 7.0*pow_u_6* 5.0*pow_v_4
        + p_p->c0706z* 7.0*pow_u_6* 6.0*pow_v_5
        + p_p->c0707z* 7.0*pow_u_6* 7.0*pow_v_6;





#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur502*c_second Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

