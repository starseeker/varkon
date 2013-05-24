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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_pat_norm                      File: sur240.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the surface normal and the              */
/*  surface normal derivatives.                                     */
/*                                                                  */
/*  Input data is (coordinates and) derivatives calculated          */
/*  by a so called surface patch evaluation routine.                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*  1993-11-07   Originally written                                 */
/*  1996-05-28   Erase of ix1, short->gmint for icase               */
/*  1997-02-09   Elimination of compilation warning                 */
/*  1999-11-22   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_norm       Surface normal with derivatives  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short normal();        /* Surface normal                     */
static short normder();       /* Surface normal derivatives         */
static short initial();       /* Initialization of variables        */
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
static DBfloat n_x,n_y,n_z;       /* Surface normal       n(u)      */
static DBfloat nu_x,nu_y,nu_z;    /* Normal derivative   dn/du      */
static DBfloat nv_x,nv_y,nv_z;    /* Normal derivative   dn/dv      */
static DBfloat n_leng;            /* Length of vector dr/du X dr/dv */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_pat_norm        */
/* SU2983 = sur240 Illegal computation case=  for varkon_pat_norm   */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_norm      */
/* SU2962 = sur240 Surface normal is a zero vector in u= , v=       */
/* SU2993 = Program error in varkon_pat_norm    (sur240). Report !  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus   varkon_pat_norm    (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  icase,         /* Calculation case ..                     */
   EVALS  *p_xyz )       /* Coordinates and derivatives       (ptr) */
/* Out:                                                             */
/*       Surface normal and derivatives data are added to p_xyz     */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */

/*  Input derivatives to local static variables.                    */
/*  Call of initial.                                                */
/*                                                                 !*/

   status=initial(icase,p_xyz); 
   if (status < 0 )
        {
        sprintf(errbuf,"initial%%varkon_pat_norm    (sur240)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
        {
        sprintf(errbuf,"%d%% varkon_pat_norm    (sur240)",(int)icase);
        return(varkon_erpush("SU2983",errbuf));
        }


/*!                                                                 */
/* 2. Normalised surface normal                                     */
/* ____________________________                                     */

/* Call of internal function normal.                                */
/*                                                                 !*/

status= normal();
if (status<0) 
    {
    sprintf(errbuf,"normal%%varkon_pat_norm    (sur240)");
    return(varkon_erpush("SU2973",errbuf));
    }

/*!                                                                 */
/* 3. Normalised surface normal derivatives                         */
/* ________________________________________                         */

/* Call of internal function normder.                               */
/*                                                                 !*/

status= normder();
if (status<0) 
    {
    sprintf(errbuf,"normder%%varkon_pat_norm    (sur240)");
    return(varkon_erpush("SU2973",errbuf));
    }

/*!                                                                 */
/* 4. Calculated data to output variables                           */
/* ______________________________________                           */

/* Surface normal and its derivatives to p_xyz                      */
/*                                                                 !*/

    p_xyz->n_x= n_x;
    p_xyz->n_y= n_y;
    p_xyz->n_z= n_z;

    p_xyz->nu_x= nu_x;
    p_xyz->nu_y= nu_y;
    p_xyz->nu_z= nu_z;

    p_xyz->nv_x= nv_x;
    p_xyz->nv_y= nv_y;
    p_xyz->nv_z= nv_z;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur240 n_x %f n_y %f n_z %f \n",
   p_xyz->n_x,p_xyz->n_y,p_xyz->n_z);
  fprintf(dbgfil(SURPAC),
  "sur240 nu_x %f nu_y %f nu_z %f \n",
   p_xyz->nu_x,p_xyz->nu_y,p_xyz->nu_z);
  fprintf(dbgfil(SURPAC),
  "sur240 nv_x %f nv_y %f nv_z %f \n",
   p_xyz->nv_x,p_xyz->nv_y,p_xyz->nv_z);
  fprintf(dbgfil(SURPAC),
  "sur240 Exit *** varkon_pat_norm    ******* \n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur240 Exit *** varkon_pat_norm    n= %f %f %f\n",
   p_xyz->n_x,p_xyz->n_y,p_xyz->n_z);
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the normalised surface normal            */
/* for given derivatives dr/du and dr/dv.                           */

   static short normal()
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

/*       Tangent in u direction u_x, u_y, u_z and                   */
/*       tangent in v direction v_x, v_y, v_z                       */

/* Out:                                                             */
/*       Surface normal n_x, n_y, n_z                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate surface normal                                      */

/* The cross product vector (u_x,u_y,u_z) X (v_x,v_y,v_z) is        */
/* perpendicular to the surface.                                    */
/* Calculate the length of the cross vector product and             */
/* let n_x, n_y, n_z be the normalised vector.                      */
/* Error SU2963 if length is zero                                   */
/*                                                                 !*/

    n_x = u_y*v_z - u_z*v_y;
    n_y = u_z*v_x - u_x*v_z;
    n_z = u_x*v_y - u_y*v_x;

    n_leng = SQRT( n_x*n_x  + n_y*n_y  + n_z*n_z );
    if ( n_leng > 1e-8  ) 
        {
        n_x = n_x/n_leng;
        n_y = n_y/n_leng;
        n_z = n_z/n_leng;
        }
    else
        {
        sprintf(errbuf," %% ");
        return(varkon_erpush("SU2962",errbuf));
        }

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the normalised surface normal            */
/* derivatives.                                                     */

   static short normder()
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       Derivatives (u_x,u_y,u_z), (v_x,v_y,v_z), (u2_x,u2_y,...   */
/*       and n_leng.                                                */

/* Out:                                                             */
/*       Surface normal derivatives nu_x,nu_y,nu_z and              */
/*       nv_x,nv_y,nv_z.                                            */

/* Formulae                                                         */
/* ........                                                         */
/*                                                                  */
/*         dr/du X dr/dv                                            */
/* n(u) = ---------------                                           */
/*        !dr/du X dr/dv!                                           */
/*                                                                  */
/*         d2r/du2 X dr/dv + dr/du X d2r/dudv                       */
/* dn/du = ----------------------------------  -                    */
/*                !dr/du X dr/dv!                                   */
/*                                                                  */
/*         n(u)*((d2r/du2 X dr/dv + dr/du X d2r/dudv )!n(u))        */
/*         -------------------------------------------------        */
/*                           !dr/du X dr/dv!                        */
/*                                                                  */
/*                                                                  */
/* dn/dv = ........                                                 */
/*                                                                  */
/*                                                                 !*/


{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  cuv_x;       /* x component of cross vector product cuv */
   DBfloat  cuv_y;       /* y component of cross vector product cuv */
   DBfloat  cuv_z;       /* z component of cross vector product cuv */
   DBfloat  cuu_x;       /* x component of cross vector product cuu */
   DBfloat  cuu_y;       /* y component of cross vector product cuu */
   DBfloat  cuu_z;       /* z component of cross vector product cuu */
   DBfloat  cvu_x;       /* x component of cross vector product cvu */
   DBfloat  cvu_y;       /* y component of cross vector product cvu */
   DBfloat  cvu_z;       /* z component of cross vector product cvu */
   DBfloat  cvv_x;       /* x component of cross vector product cvv */
   DBfloat  cvv_y;       /* y component of cross vector product cvv */
   DBfloat  cvv_z;       /* z component of cross vector product cvv */
   DBfloat  f1,f2;       /* Scalar products                         */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Cross vector products                                         */

/* Let cuv= dr/du X d2r/dudv= (u_x,u_y,u_z)X(uv_x,uv_y,uv_z)        */
/*                                                                 !*/

    cuv_x = u_y*uv_z - u_z*uv_y;
    cuv_y = u_z*uv_x - u_x*uv_z;
    cuv_z = u_x*uv_y - u_y*uv_x;

/*!                                                                 */
/* Let cuu= d2r/du2 X dr/dv= (u2_x,u2_y,u2_z)X(v_x,v_y,v_z)         */
/*                                                                 !*/

    cuu_x = u2_y*v_z - u2_z*v_y;
    cuu_y = u2_z*v_x - u2_x*v_z;
    cuu_z = u2_x*v_y - u2_y*v_x;

/*!                                                                 */
/* Let cvu= d2r/dudv X dr/dv= (uv_x,uv_y,uv_z)X(v_x,v_y,v_z)        */
/*                                                                 !*/

    cvu_x = uv_y*v_z - uv_z*v_y;
    cvu_y = uv_z*v_x - uv_x*v_z;
    cvu_z = uv_x*v_y - uv_y*v_x;

/*!                                                                 */
/* Let cvv= dr/du X d2r/dv2= (u_x,u_y,u_z)X(v2_x,v2_y,v2_z)         */
/*                                                                 !*/

    cvv_x = u_y*v2_z - u_z*v2_y;
    cvv_y = u_z*v2_x - u_x*v2_z;
    cvv_z = u_x*v2_y - u_y*v2_x;

/*!                                                                 */
/* 2. Output surface normal derivatives                             */

/* Calculate (d2r/du2 + d2r/dudv)/n_leng                            */
/*                                                                 !*/

    nu_x  = ( cuu_x + cuv_x )/n_leng;
    nu_y  = ( cuu_y + cuv_y )/n_leng;
    nu_z  = ( cuu_z + cuv_z )/n_leng;

/*!                                                                 */
/* Calculate (d2r/dudv + d2r/du2)/n_leng                            */
/*                                                                 !*/

    nv_x  = ( cvu_x + cvv_x )/n_leng;
    nv_y  = ( cvu_y + cvv_y )/n_leng;
    nv_z  = ( cvu_z + cvv_z )/n_leng;

/*!                                                                 */
/* Calculate dot product f1= n(u) * (d2r/du2 + d2r/dudv)/n_leng     */
/*                                                                 !*/

    f1    = nu_x*n_x + nu_y*n_y + nu_z*n_z;

/*!                                                                 */
/* Calculate dot product f2= n(u) * (d2r/dudv + d2r/du2)/n_leng     */
/*                                                                 !*/

    f2    = nv_x*n_x + nv_y*n_y + nv_z*n_z;

/*!                                                                 */
/* Let dn/du= (d2r/du2 + d2r/dudv)/n_leng - n(u)*f1                 */
/*                                                                 !*/

    nu_x  = nu_x - n_x*f1;
    nu_y  = nu_y - n_y*f1;
    nu_z  = nu_z - n_z*f1;

/*!                                                                 */
/* Let dn/dv= (d2r/dudv + d2r/du2)/n_leng - n(u)*f2                 */
/*                                                                 !*/

    nv_x  = nv_x - n_x*f2;
    nv_y  = nv_y - n_y*f2;
    nv_z  = nv_z - n_z*f2;

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the local static variables              */

   static short initial(icase,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  icase;         /* Calculation case ..                     */
   EVALS  *p_xyz;        /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur240 *** initial: icase= %d \n", (int)icase);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 2. Input derivatives to local static variables                   */
/*                                                                 !*/

/*  Coordinates          r(u)    (not used in this function)        */
    r_x=  p_xyz->r_x;   
    r_y=  p_xyz->r_y;   
    r_z=  p_xyz->r_z;   
/*  Tangent             dr/du                                       */
    u_x=  p_xyz->u_x; 
    u_y=  p_xyz->u_y; 
    u_z=  p_xyz->u_z; 
/*  Tangent             dr/dv                                       */
    v_x=  p_xyz->v_x; 
    v_y=  p_xyz->v_y; 
    v_z=  p_xyz->v_z; 
/*  Second derivative  d2r/du2                                      */
    u2_x= p_xyz->u2_x; 
    u2_y= p_xyz->u2_y; 
    u2_z= p_xyz->u2_z; 
/*  Second derivative  d2r/dv2                                      */
    v2_x= p_xyz->v2_x; 
    v2_y= p_xyz->v2_y; 
    v2_z= p_xyz->v2_z; 
/*  Twist vector       d2r/dudv                                     */
    uv_x= p_xyz->uv_x; 
    uv_y= p_xyz->uv_y; 
    uv_z= p_xyz->uv_z; 



/*!                                                                 */
/* 3. Initiate output variables for Debug On                        */
/*                                                                 !*/



#ifdef  DEBUG
/*  Surface normal       r(u)                                       */
    p_xyz->n_x= F_UNDEF;    
    p_xyz->n_y= F_UNDEF;    
    p_xyz->n_z= F_UNDEF;    

    n_x= F_UNDEF;    
    n_y= F_UNDEF;    
    n_z= F_UNDEF;    

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= F_UNDEF;    
    p_xyz->nu_y= F_UNDEF;    
    p_xyz->nu_z= F_UNDEF;    

    nu_x= F_UNDEF;    
    nu_y= F_UNDEF;    
    nu_z= F_UNDEF;    

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= F_UNDEF;    
    p_xyz->nv_y= F_UNDEF;    
    p_xyz->nv_z= F_UNDEF;    

    nv_x= F_UNDEF;    
    nv_y= F_UNDEF;    
    nv_z= F_UNDEF;    

/*  Length of vector dr/du X dr/dv                                  */
    n_leng= F_UNDEF;    

#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
