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
/*  Function: varkon_pat_p3eval                    File: sur500.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Evaluation of GMPATP3                                           */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-22   Created by MBS program p_evalpxv0                  */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_p3eval     Evaluation of GMPATP3            */
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
static DBfloat pow_v_0;           /* V**0                           */
static DBfloat pow_v_1;           /* V**1                           */
static DBfloat pow_v_2;           /* V**2                           */
static DBfloat pow_v_3;           /* V**3                           */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2983 = Illegal computation case=   varkon_pat_p3eval  (sur500) */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_p3eval    */
/* SU2993 = Severe program error in varkon_pat_p3eval  (sur500).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
         DBstatus varkon_pat_p3eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATP3  *p_pat3,     /* Patch GMPATP3                     (ptr) */
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
"sur500 Enter *** varkon_pat_p3eval : u= %f v= %f p_pat3= %d\n",
         u_l  ,v_l  , (int)p_pat3 );
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
"sur500 Error Internal function initial failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"initial%%sur500");
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
"sur500 Error Internal function c_power failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_power%%sur500");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/* Calculate coordinates. Call of internal function c_coord.        */
/* Goto nomore if icase =  0.                                       */
/*                                                                 !*/

   status= c_coord(p_pat3, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur500 Error Internal function c_coord failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_coord%%sur500");
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

   status= c_drdu (p_pat3, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur500 Error Internal function c_drdu  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdu %%sur500");
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

   status= c_drdv (p_pat3, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur500 Error Internal function c_drdv  failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_drdv %%sur500");
       return(varkon_erpush("SU2973",errbuf));
       }

   p_xyz->v_x= v_x;
   p_xyz->v_y= v_y;
   p_xyz->v_z= v_z;

   if  (  icase == 2 || icase == 3 ) goto nomore;


/*!                                                                 */
/* Calculate second derivatives. Call of internal function c_second */
/*                                                                 !*/

   status= c_second(p_pat3, u_l,v_l);
   if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur500 Error Internal function c_second failed \n" );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"c_second%%sur500");
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
  "sur500 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
if ( dbglev(SURPAC) == 1 && icase >= 3 )
  {
  fprintf(dbgfil(SURPAC),
  "sur500 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur500 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  }
if ( dbglev(SURPAC) == 2 && icase >= 4 )
  {
  fprintf(dbgfil(SURPAC),
  "sur500 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur500 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur500 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur500 Exit*varkon_pat_p3eval * p_xyz %f %f %f\n",
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
"sur500*initial: icase= %d p_xyz= %d\n",(short)icase,(int)p_xyz);
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
"sur500 Error icase= %d\n", (short)icase );
fflush(dbgfil(SURPAC)); 
}
#endif

        sprintf(errbuf,"%d%% varkon_pat_p3eval  (sur500)",(short)icase);
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
"sur500*c_power u_l= %f v_l= %f\n",u_l,v_l);
}
#endif


   pow_u_0  = pow( u_l, 0.0 );
   pow_u_1  = pow( u_l, 1.0 );
   pow_u_2  = pow( u_l, 2.0 );
   pow_u_3  = pow( u_l, 3.0 );
   pow_v_0  = pow( v_l, 0.0 );
   pow_v_1  = pow( v_l, 1.0 );
   pow_v_2  = pow( v_l, 2.0 );
   pow_v_3  = pow( v_l, 3.0 );




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur500*c_power Exit \n");
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
/* The function calculates coordinates for GMPATP3                  */

   static short c_coord ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP3   *p_p;       /* Patch GMPATP3                     (ptr) */
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
"sur500*c_coord p_p= %d  u_l= %f v_l= %f\n",(int)p_p,u_l,v_l);
}
#endif


r_x     = p_p->c0000x * pow_u_0   * pow_v_0  
        + p_p->c0001x * pow_u_0   * pow_v_1  
        + p_p->c0002x * pow_u_0   * pow_v_2  
        + p_p->c0003x * pow_u_0   * pow_v_3  
        + p_p->c0100x * pow_u_1   * pow_v_0  
        + p_p->c0101x * pow_u_1   * pow_v_1  
        + p_p->c0102x * pow_u_1   * pow_v_2  
        + p_p->c0103x * pow_u_1   * pow_v_3  
        + p_p->c0200x * pow_u_2   * pow_v_0  
        + p_p->c0201x * pow_u_2   * pow_v_1  
        + p_p->c0202x * pow_u_2   * pow_v_2  
        + p_p->c0203x * pow_u_2   * pow_v_3  
        + p_p->c0300x * pow_u_3   * pow_v_0  
        + p_p->c0301x * pow_u_3   * pow_v_1  
        + p_p->c0302x * pow_u_3   * pow_v_2  
        + p_p->c0303x * pow_u_3   * pow_v_3  ;

r_y     = p_p->c0000y * pow_u_0   * pow_v_0  
        + p_p->c0001y * pow_u_0   * pow_v_1  
        + p_p->c0002y * pow_u_0   * pow_v_2  
        + p_p->c0003y * pow_u_0   * pow_v_3  
        + p_p->c0100y * pow_u_1   * pow_v_0  
        + p_p->c0101y * pow_u_1   * pow_v_1  
        + p_p->c0102y * pow_u_1   * pow_v_2  
        + p_p->c0103y * pow_u_1   * pow_v_3  
        + p_p->c0200y * pow_u_2   * pow_v_0  
        + p_p->c0201y * pow_u_2   * pow_v_1  
        + p_p->c0202y * pow_u_2   * pow_v_2  
        + p_p->c0203y * pow_u_2   * pow_v_3  
        + p_p->c0300y * pow_u_3   * pow_v_0  
        + p_p->c0301y * pow_u_3   * pow_v_1  
        + p_p->c0302y * pow_u_3   * pow_v_2  
        + p_p->c0303y * pow_u_3   * pow_v_3  ;

r_z     = p_p->c0000z * pow_u_0   * pow_v_0  
        + p_p->c0001z * pow_u_0   * pow_v_1  
        + p_p->c0002z * pow_u_0   * pow_v_2  
        + p_p->c0003z * pow_u_0   * pow_v_3  
        + p_p->c0100z * pow_u_1   * pow_v_0  
        + p_p->c0101z * pow_u_1   * pow_v_1  
        + p_p->c0102z * pow_u_1   * pow_v_2  
        + p_p->c0103z * pow_u_1   * pow_v_3  
        + p_p->c0200z * pow_u_2   * pow_v_0  
        + p_p->c0201z * pow_u_2   * pow_v_1  
        + p_p->c0202z * pow_u_2   * pow_v_2  
        + p_p->c0203z * pow_u_2   * pow_v_3  
        + p_p->c0300z * pow_u_3   * pow_v_0  
        + p_p->c0301z * pow_u_3   * pow_v_1  
        + p_p->c0302z * pow_u_3   * pow_v_2  
        + p_p->c0303z * pow_u_3   * pow_v_3  ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC), "sur500*c_coord Exit \n");
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
/* The function calculates derivatives dr/du for GMPATP3            */

   static short c_drdu  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP3  *p_p;         /* Patch GMPATP3                     (ptr) */
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
"sur500*c_drdu  p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l);
}
#endif


u_x     = p_p->c0000x*         0.0         * pow_v_0  
        + p_p->c0001x*         0.0         * pow_v_1  
        + p_p->c0002x*         0.0         * pow_v_2  
        + p_p->c0003x*         0.0         * pow_v_3  
        + p_p->c0100x* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101x* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102x* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103x* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0200x* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201x* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202x* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203x* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0300x* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301x* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302x* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303x* 3.0*pow_u_2 * pow_v_3  ;

u_y     = p_p->c0000y*         0.0         * pow_v_0  
        + p_p->c0001y*         0.0         * pow_v_1  
        + p_p->c0002y*         0.0         * pow_v_2  
        + p_p->c0003y*         0.0         * pow_v_3  
        + p_p->c0100y* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101y* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102y* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103y* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0200y* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201y* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202y* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203y* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0300y* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301y* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302y* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303y* 3.0*pow_u_2 * pow_v_3  ;

u_z     = p_p->c0000z*         0.0         * pow_v_0  
        + p_p->c0001z*         0.0         * pow_v_1  
        + p_p->c0002z*         0.0         * pow_v_2  
        + p_p->c0003z*         0.0         * pow_v_3  
        + p_p->c0100z* 1.0*pow_u_0 * pow_v_0  
        + p_p->c0101z* 1.0*pow_u_0 * pow_v_1  
        + p_p->c0102z* 1.0*pow_u_0 * pow_v_2  
        + p_p->c0103z* 1.0*pow_u_0 * pow_v_3  
        + p_p->c0200z* 2.0*pow_u_1 * pow_v_0  
        + p_p->c0201z* 2.0*pow_u_1 * pow_v_1  
        + p_p->c0202z* 2.0*pow_u_1 * pow_v_2  
        + p_p->c0203z* 2.0*pow_u_1 * pow_v_3  
        + p_p->c0300z* 3.0*pow_u_2 * pow_v_0  
        + p_p->c0301z* 3.0*pow_u_2 * pow_v_1  
        + p_p->c0302z* 3.0*pow_u_2 * pow_v_2  
        + p_p->c0303z* 3.0*pow_u_2 * pow_v_3  ;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur500*c_drdu  Exit \n");
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
/* The function calculates derivatives dr/dv for GMPATP3            */

   static short c_drdv  ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP3  *p_p;         /* Patch GMPATP3                     (ptr) */
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
"sur500*c_drdv  p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l );
}
#endif


v_x     = p_p->c0000x * pow_u_0  *         0.0        
        + p_p->c0001x * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002x * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003x * pow_u_0  * 3.0*pow_v_2
        + p_p->c0100x * pow_u_1  *         0.0        
        + p_p->c0101x * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102x * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103x * pow_u_1  * 3.0*pow_v_2
        + p_p->c0200x * pow_u_2  *         0.0        
        + p_p->c0201x * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202x * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203x * pow_u_2  * 3.0*pow_v_2
        + p_p->c0300x * pow_u_3  *         0.0        
        + p_p->c0301x * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302x * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303x * pow_u_3  * 3.0*pow_v_2;

v_y     = p_p->c0000y * pow_u_0  *         0.0        
        + p_p->c0001y * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002y * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003y * pow_u_0  * 3.0*pow_v_2
        + p_p->c0100y * pow_u_1  *         0.0        
        + p_p->c0101y * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102y * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103y * pow_u_1  * 3.0*pow_v_2
        + p_p->c0200y * pow_u_2  *         0.0        
        + p_p->c0201y * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202y * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203y * pow_u_2  * 3.0*pow_v_2
        + p_p->c0300y * pow_u_3  *         0.0        
        + p_p->c0301y * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302y * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303y * pow_u_3  * 3.0*pow_v_2;

v_z     = p_p->c0000z * pow_u_0  *         0.0        
        + p_p->c0001z * pow_u_0  * 1.0*pow_v_0
        + p_p->c0002z * pow_u_0  * 2.0*pow_v_1
        + p_p->c0003z * pow_u_0  * 3.0*pow_v_2
        + p_p->c0100z * pow_u_1  *         0.0        
        + p_p->c0101z * pow_u_1  * 1.0*pow_v_0
        + p_p->c0102z * pow_u_1  * 2.0*pow_v_1
        + p_p->c0103z * pow_u_1  * 3.0*pow_v_2
        + p_p->c0200z * pow_u_2  *         0.0        
        + p_p->c0201z * pow_u_2  * 1.0*pow_v_0
        + p_p->c0202z * pow_u_2  * 2.0*pow_v_1
        + p_p->c0203z * pow_u_2  * 3.0*pow_v_2
        + p_p->c0300z * pow_u_3  *         0.0        
        + p_p->c0301z * pow_u_3  * 1.0*pow_v_0
        + p_p->c0302z * pow_u_3  * 2.0*pow_v_1
        + p_p->c0303z * pow_u_3  * 3.0*pow_v_2;






#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur500*c_drdv  Exit \n");
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
/* The function calculates the second derivatives for GMPATP3       */

   static short c_second ( p_p, u_l, v_l )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATP3  *p_p;         /* Patch GMPATP3                     (ptr) */
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
"sur500*c_second p_p= %d  u_l= %f v_l= %f\n"
           ,(int)p_p, u_l, v_l );
}
#endif


u2_x    = p_p->c0000x*            0.0           * pow_v_0  
        + p_p->c0001x*            0.0           * pow_v_1  
        + p_p->c0002x*            0.0           * pow_v_2  
        + p_p->c0003x*            0.0           * pow_v_3  
        + p_p->c0100x*            0.0           * pow_v_0  
        + p_p->c0101x*            0.0           * pow_v_1  
        + p_p->c0102x*            0.0           * pow_v_2  
        + p_p->c0103x*            0.0           * pow_v_3  
        + p_p->c0200x* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201x* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202x* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203x* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0300x* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301x* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302x* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303x* 2.0* 3.0*pow_u_1 * pow_v_3  ;

u2_y    = p_p->c0000y*            0.0           * pow_v_0  
        + p_p->c0001y*            0.0           * pow_v_1  
        + p_p->c0002y*            0.0           * pow_v_2  
        + p_p->c0003y*            0.0           * pow_v_3  
        + p_p->c0100y*            0.0           * pow_v_0  
        + p_p->c0101y*            0.0           * pow_v_1  
        + p_p->c0102y*            0.0           * pow_v_2  
        + p_p->c0103y*            0.0           * pow_v_3  
        + p_p->c0200y* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201y* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202y* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203y* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0300y* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301y* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302y* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303y* 2.0* 3.0*pow_u_1 * pow_v_3  ;

u2_z    = p_p->c0000z*            0.0           * pow_v_0  
        + p_p->c0001z*            0.0           * pow_v_1  
        + p_p->c0002z*            0.0           * pow_v_2  
        + p_p->c0003z*            0.0           * pow_v_3  
        + p_p->c0100z*            0.0           * pow_v_0  
        + p_p->c0101z*            0.0           * pow_v_1  
        + p_p->c0102z*            0.0           * pow_v_2  
        + p_p->c0103z*            0.0           * pow_v_3  
        + p_p->c0200z* 1.0* 2.0*pow_u_0 * pow_v_0  
        + p_p->c0201z* 1.0* 2.0*pow_u_0 * pow_v_1  
        + p_p->c0202z* 1.0* 2.0*pow_u_0 * pow_v_2  
        + p_p->c0203z* 1.0* 2.0*pow_u_0 * pow_v_3  
        + p_p->c0300z* 2.0* 3.0*pow_u_1 * pow_v_0  
        + p_p->c0301z* 2.0* 3.0*pow_u_1 * pow_v_1  
        + p_p->c0302z* 2.0* 3.0*pow_u_1 * pow_v_2  
        + p_p->c0303z* 2.0* 3.0*pow_u_1 * pow_v_3  ;

v2_x    = p_p->c0000x * pow_u_0  *            0.0          
        + p_p->c0001x * pow_u_0  *            0.0          
        + p_p->c0002x * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003x * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0100x * pow_u_1  *            0.0          
        + p_p->c0101x * pow_u_1  *            0.0          
        + p_p->c0102x * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103x * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0200x * pow_u_2  *            0.0          
        + p_p->c0201x * pow_u_2  *            0.0          
        + p_p->c0202x * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203x * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0300x * pow_u_3  *            0.0          
        + p_p->c0301x * pow_u_3  *            0.0          
        + p_p->c0302x * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303x * pow_u_3  * 2.0* 3.0*pow_v_1;

v2_y    = p_p->c0000y * pow_u_0  *            0.0          
        + p_p->c0001y * pow_u_0  *            0.0          
        + p_p->c0002y * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003y * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0100y * pow_u_1  *            0.0          
        + p_p->c0101y * pow_u_1  *            0.0          
        + p_p->c0102y * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103y * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0200y * pow_u_2  *            0.0          
        + p_p->c0201y * pow_u_2  *            0.0          
        + p_p->c0202y * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203y * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0300y * pow_u_3  *            0.0          
        + p_p->c0301y * pow_u_3  *            0.0          
        + p_p->c0302y * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303y * pow_u_3  * 2.0* 3.0*pow_v_1;

v2_z    = p_p->c0000z * pow_u_0  *            0.0          
        + p_p->c0001z * pow_u_0  *            0.0          
        + p_p->c0002z * pow_u_0  * 1.0* 2.0*pow_v_0
        + p_p->c0003z * pow_u_0  * 2.0* 3.0*pow_v_1
        + p_p->c0100z * pow_u_1  *            0.0          
        + p_p->c0101z * pow_u_1  *            0.0          
        + p_p->c0102z * pow_u_1  * 1.0* 2.0*pow_v_0
        + p_p->c0103z * pow_u_1  * 2.0* 3.0*pow_v_1
        + p_p->c0200z * pow_u_2  *            0.0          
        + p_p->c0201z * pow_u_2  *            0.0          
        + p_p->c0202z * pow_u_2  * 1.0* 2.0*pow_v_0
        + p_p->c0203z * pow_u_2  * 2.0* 3.0*pow_v_1
        + p_p->c0300z * pow_u_3  *            0.0          
        + p_p->c0301z * pow_u_3  *            0.0          
        + p_p->c0302z * pow_u_3  * 1.0* 2.0*pow_v_0
        + p_p->c0303z * pow_u_3  * 2.0* 3.0*pow_v_1;

uv_x    = p_p->c0000x*         0.0        *         0.0        
        + p_p->c0001x*         0.0        * 1.0*pow_v_0
        + p_p->c0002x*         0.0        * 2.0*pow_v_1
        + p_p->c0003x*         0.0        * 3.0*pow_v_2
        + p_p->c0100x* 1.0*pow_u_0*         0.0        
        + p_p->c0101x* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102x* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103x* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0200x* 2.0*pow_u_1*         0.0        
        + p_p->c0201x* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202x* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203x* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0300x* 3.0*pow_u_2*         0.0        
        + p_p->c0301x* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302x* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303x* 3.0*pow_u_2* 3.0*pow_v_2;

uv_y    = p_p->c0000y*         0.0        *         0.0        
        + p_p->c0001y*         0.0        * 1.0*pow_v_0
        + p_p->c0002y*         0.0        * 2.0*pow_v_1
        + p_p->c0003y*         0.0        * 3.0*pow_v_2
        + p_p->c0100y* 1.0*pow_u_0*         0.0        
        + p_p->c0101y* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102y* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103y* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0200y* 2.0*pow_u_1*         0.0        
        + p_p->c0201y* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202y* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203y* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0300y* 3.0*pow_u_2*         0.0        
        + p_p->c0301y* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302y* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303y* 3.0*pow_u_2* 3.0*pow_v_2;

uv_z    = p_p->c0000z*         0.0        *         0.0        
        + p_p->c0001z*         0.0        * 1.0*pow_v_0
        + p_p->c0002z*         0.0        * 2.0*pow_v_1
        + p_p->c0003z*         0.0        * 3.0*pow_v_2
        + p_p->c0100z* 1.0*pow_u_0*         0.0        
        + p_p->c0101z* 1.0*pow_u_0* 1.0*pow_v_0
        + p_p->c0102z* 1.0*pow_u_0* 2.0*pow_v_1
        + p_p->c0103z* 1.0*pow_u_0* 3.0*pow_v_2
        + p_p->c0200z* 2.0*pow_u_1*         0.0        
        + p_p->c0201z* 2.0*pow_u_1* 1.0*pow_v_0
        + p_p->c0202z* 2.0*pow_u_1* 2.0*pow_v_1
        + p_p->c0203z* 2.0*pow_u_1* 3.0*pow_v_2
        + p_p->c0300z* 3.0*pow_u_2*         0.0        
        + p_p->c0301z* 3.0*pow_u_2* 1.0*pow_v_0
        + p_p->c0302z* 3.0*pow_u_2* 2.0*pow_v_1
        + p_p->c0303z* 3.0*pow_u_2* 3.0*pow_v_2;





#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur500*c_second Exit \n");
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

