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
/*  Function: varkon_pat_biccre2                   File: sur256.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a ruled surface patch.                     */
/*                                                                  */
/*  Input data is two boundary curve segments which must be         */
/*  cubic (non-rational) segments.                                  */
/*  The output patch will be a cubic polynomial in one              */
/*  direction and straight lines (degree one) curves in             */
/*  the other direction.                                            */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-22   Originally written                                 */
/*  1996-05-28   Elimination of compilation warning                 */
/*  1996-09-07   Change of short description, elim. comp. warning   */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_biccre2    Create a ruled cubic patch       */
/*                                                              */
/*------------------------------------------------------------- */

#ifdef DEBUG
/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Initialization of variables        */
/*                                                                  */
/*-----------------------------------------------------------------!*/
#endif

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_pat_pribic       * Printout of bicubic patch data         */
/* GE107                   * Curve segment evaluation. For Debug On */
/* varkon_erpush           * Error message             For Debug On */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_biccre2   */
/* SU2993 = Severe program error in varkon_pat_biccre2 (sur256).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus    varkon_pat_biccre2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0,         /* Boundary segment U= 0.0           (ptr) */
   DBSeg  *p_u1,         /* Boundary segment U= 1.0           (ptr) */
   GMPATC *p_patc )      /* Bicubic patch                     (ptr) */

/* Out:                                                             */
/*       Data to p_patc                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

  DBfloat   comptol;       /* Computer tolerance (accuracy)           */
   char   errbuf[80];    /* String for error message fctn erpush    */
#ifdef  DEBUG
   DBint  status;        /* Error code from a called function       */
#endif

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* Create a bicubic ruled patch from 2 boundary curves              */
/* _____________________________________________________            */
/*                                                                  */
/* A ruled, bicubic surface patch is defined by:                    */
/*                                                                  */
/* r(u,v)= (1-v)*ru0(u) + v*ru1(u)                                  */
/*                                                                  */
/* where ru0 and ru1 are (non-rational) cubic curve segments.       */
/*                                                                  */
/* r(u,v)= (1-v)*(C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*           v  *(C0u1+C1u1*u+C2u1*u**2+C3u1*u**3)                  */
/*                                                                  */
/* r(u,v)=       (C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*          (-v)*(C0u0+C1u0*u+C2u0*u**2+C3u0*u**3) +                */
/*           v  *(C0u1+C1u1*u+C2u1*u**2+C3u1*u**3)                  */
/*                                                                  */
/* r(u,v)=       (C0u0+C1u0*u+C2u0*u**2+C3u0*u**3)            *1 +  */
/* (C0u1-C0u0+(C1u1-C1u0)*u+(C2u1-C2u0)*u**2+(C3u1-C3u0)*u**3)*v    */
/*                                                                  */
/*                                                                  */
/* Identify coefficients in matrix A:                               */
/*                                                                  */
/* r(u,v) = U * A * V                                               */
/*                                                         ! 1  !   */
/*                                                         !    !   */
/*                            ! C0u0  C0u1-C0u0  0  0 !    ! v  !   */
/*                 2  3       !                       !    !    !   */
/* r(u,v) = ( 1 u u  u  )  *  ! C1u0  C1u1-C1u0  0  0 !    !  2 !   */
/*                            !                       !  * ! v  !   */
/*                            ! C2u0  C2u1-C2u0  0  0 !    !    !   */
/*                            !                       !    !  3 !   */
/*                            ! C3u0  C3u1-C3u0  0  0 !    ! v  !   */
/*                                                                  */
/*                                                                  */
/*        ! A00= C0u0  A01= C0u1-C0u0  A02= 0  A03= 0 !             */
/*        !                                           !             */
/*   A =  ! A10= C1u0  A11= C1u1-C1u0  A12= 0  A13= 0 !             */
/*        !                                           !             */
/*        ! A20= C2u0  A21= C0u2-C2u0  A22= 0  A23= 0 !             */
/*        !                                           !             */
/*        ! A30= C3u0  A31= C0u3-C3u0  A32= 0  A33= 0 !             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur256 Enter varkon_pat_biccre2: Ruled bicubic patch p_patc %d \n",
                p_patc );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Check of input data.                        for DEBUG on.       */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_u0, p_u1, p_patc);
   if (status<0) 
   {
   sprintf(errbuf,"initial%%varkon_pat_biccre2 (sur256)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/*  Check that input segments are non-rational functions            */
/*                                                                 !*/

    if ( fabs((*p_u0).c1 ) > comptol || 
         fabs((*p_u0).c2 ) > comptol || 
         fabs((*p_u0).c3 ) > comptol    )
        {

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur256 Boundary u0 c1 %f c2 %f c3 %f \n",
        (*p_u0).c1,(*p_u0).c2,(*p_u0).c3   );
  fflush(dbgfil(SURPAC)); 
}
#endif

        sprintf(errbuf,"u0 rational%%varkon_pat_biccre2 (sur256)");
        return(varkon_erpush("SU2993",errbuf));
        }

    if ( fabs((*p_u1).c1 ) > comptol || 
         fabs((*p_u1).c2 ) > comptol || 
         fabs((*p_u1).c3 ) > comptol    )
        {

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur256 Boundary u1 c1 %f c2 %f c3 %f \n",
        (*p_u1).c1,(*p_u1).c2,(*p_u1).c3   );
  fflush(dbgfil(SURPAC));
}
#endif

        sprintf(errbuf,"u1 rational%%varkon_pat_biccre2 (sur256)");
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 2. Coefficients for the ruled bicubic patch                      */
/* ___________________________________________                      */
/*                                                                 !*/

/*!                                                                 */
/*  Coefficients for X                                              */
/*                                                                 !*/

    p_patc->a00x= (*p_u0).c0x;
    p_patc->a01x= (*p_u1).c0x - (*p_u0).c0x;
    p_patc->a02x= 0.0;        
    p_patc->a03x= 0.0;        

    p_patc->a10x= (*p_u0).c1x;
    p_patc->a11x= (*p_u1).c1x - (*p_u0).c1x;
    p_patc->a12x= 0.0;        
    p_patc->a13x= 0.0;        

    p_patc->a20x= (*p_u0).c2x;
    p_patc->a21x= (*p_u1).c2x - (*p_u0).c2x;
    p_patc->a22x= 0.0;        
    p_patc->a23x= 0.0;        

    p_patc->a30x= (*p_u0).c3x;
    p_patc->a31x= (*p_u1).c3x - (*p_u0).c3x;
    p_patc->a32x= 0.0;        
    p_patc->a33x= 0.0;        

/*!                                                                 */
/*  Coefficients for Y                                              */
/*                                                                 !*/

    p_patc->a00y= (*p_u0).c0y;
    p_patc->a01y= (*p_u1).c0y - (*p_u0).c0y;
    p_patc->a02y= 0.0;        
    p_patc->a03y= 0.0;        

    p_patc->a10y= (*p_u0).c1y;
    p_patc->a11y= (*p_u1).c1y - (*p_u0).c1y;
    p_patc->a12y= 0.0;        
    p_patc->a13y= 0.0;        

    p_patc->a20y= (*p_u0).c2y;
    p_patc->a21y= (*p_u1).c2y - (*p_u0).c2y;
    p_patc->a22y= 0.0;        
    p_patc->a23y= 0.0;        

    p_patc->a30y= (*p_u0).c3y;
    p_patc->a31y= (*p_u1).c3y - (*p_u0).c3y;
    p_patc->a32y= 0.0;        
    p_patc->a33y= 0.0;        

/*!                                                                 */
/*  Coefficients for Z                                              */
/*                                                                 !*/

    p_patc->a00z= (*p_u0).c0z;
    p_patc->a01z= (*p_u1).c0z - (*p_u0).c0z;
    p_patc->a02z= 0.0;        
    p_patc->a03z= 0.0;        

    p_patc->a10z= (*p_u0).c1z;
    p_patc->a11z= (*p_u1).c1z - (*p_u0).c1z;
    p_patc->a12z= 0.0;        
    p_patc->a13z= 0.0;        

    p_patc->a20z= (*p_u0).c2z;
    p_patc->a21z= (*p_u1).c2z - (*p_u0).c2z;
    p_patc->a22z= 0.0;        
    p_patc->a23z= 0.0;        

    p_patc->a30z= (*p_u0).c3z;
    p_patc->a31z= (*p_u1).c3z - (*p_u0).c3z;
    p_patc->a32z= 0.0;        
    p_patc->a33z= 0.0;        



/*!                                                                 */
/*  Let offset for the patch be zero                                */
/*                                                                 !*/
    p_patc->ofs_pat = 0.0;        

/*!                                                                 */
/*  Printout of patch data for Debug On                             */
/*  Call of varkon_pat_pribic (sur233).                             */
/*                                                                 !*/

#ifdef DEBUG
   status=varkon_pat_pribic (p_patc);
   if (status<0) 
   {
   sprintf(errbuf,"sur233%%varkon_pat_biccre2 (sur256)");
   return(varkon_erpush("SU2943",errbuf));
   }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur256 Exit *** varkon_pat_biccre2  \n"
   );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

#ifdef DEBUG
/*!********* Internal ** function **Only*for*Debug*On*(start)********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data.                              */

   static short initial (p_u0, p_u1, p_patc)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0;         /* Boundary segment U= 0.0           (ptr) */
   DBSeg  *p_u1;         /* Boundary segment U= 1.0           (ptr) */
   GMPATC *p_patc;       /* Bicubic patch                     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u0sx,u0sy,u0sz;    /* Boundary U= 0   start point        */
   DBfloat u0ex,u0ey,u0ez;    /* Boundary U= 0   end   point        */
   DBfloat u1sx,u1sy,u1sz;    /* Boundary U= 1   start point        */
   DBfloat u1ex,u1ey,u1ez;    /* Boundary U= 1   end   point        */
/*-----------------------------------------------------------------!*/

   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors and      out[9-14]*/
                         /*        curvature added         out[ 15 ]*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur256 *** initial: p_u0 %d p_u1 %d d\n",
                       p_u0,   p_u1 );
  fflush(dbgfil(SURPAC));
  }

/*!                                                                 */
/* 2. Initialize output variables GMPATC and static variables       */
/*                                                                 !*/

/*  Coefficients for X                                              */
/*  -------------------                                             */
    p_patc->a00x= 1.23456789;
    p_patc->a01x= 1.23456789;
    p_patc->a02x= 1.23456789;
    p_patc->a03x= 1.23456789;

    p_patc->a10x= 1.23456789;
    p_patc->a11x= 1.23456789;
    p_patc->a12x= 1.23456789;
    p_patc->a13x= 1.23456789;

    p_patc->a20x= 1.23456789;
    p_patc->a21x= 1.23456789;
    p_patc->a22x= 1.23456789;
    p_patc->a23x= 1.23456789;

    p_patc->a30x= 1.23456789;
    p_patc->a31x= 1.23456789;
    p_patc->a32x= 1.23456789;
    p_patc->a33x= 1.23456789;

/*  Coefficients for Y                                              */
/*  -------------------                                             */
    p_patc->a00y= 1.23456789;
    p_patc->a01y= 1.23456789;
    p_patc->a02y= 1.23456789;
    p_patc->a03y= 1.23456789;

    p_patc->a10y= 1.23456789;
    p_patc->a11y= 1.23456789;
    p_patc->a12y= 1.23456789;
    p_patc->a13y= 1.23456789;

    p_patc->a20y= 1.23456789;
    p_patc->a21y= 1.23456789;
    p_patc->a22y= 1.23456789;
    p_patc->a23y= 1.23456789;

    p_patc->a30y= 1.23456789;
    p_patc->a31y= 1.23456789;
    p_patc->a32y= 1.23456789;
    p_patc->a33y= 1.23456789;


/*  Coefficients for Z                                              */
/*  -------------------                                             */
    p_patc->a00z= 1.23456789;
    p_patc->a01z= 1.23456789;
    p_patc->a02z= 1.23456789;
    p_patc->a03z= 1.23456789;

    p_patc->a10z= 1.23456789;
    p_patc->a11z= 1.23456789;
    p_patc->a12z= 1.23456789;
    p_patc->a13z= 1.23456789;

    p_patc->a20z= 1.23456789;
    p_patc->a21z= 1.23456789;
    p_patc->a22z= 1.23456789;
    p_patc->a23z= 1.23456789;

    p_patc->a30z= 1.23456789;
    p_patc->a31z= 1.23456789;
    p_patc->a32z= 1.23456789;
    p_patc->a33z= 1.23456789;

/* 3. Initializations for the curve evaluation routine              */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 3. Check that the input curve net is closed                      */
/*                                                                  */
/*    Calculate end points of the boundary curves.                  */
/*    Calls of GE107.                                               */
/*                                                                 !*/

t_l = 0.0;
   status=GE107 (&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_biccre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0sx = out[0]; 
u0sy = out[1]; 
u0sz = out[2]; 

t_l = 1.0;
   status=GE107 (&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_biccre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u0ex = out[0]; 
u0ey = out[1]; 
u0ez = out[2]; 

t_l = 0.0;
   status=GE107 (&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_biccre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1sx = out[0]; 
u1sy = out[1]; 
u1sz = out[2]; 

t_l = 1.0;
   status=GE107 (&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_biccre2 (initial)");
        return(varkon_erpush("SU2943",errbuf));
        }

u1ex = out[0]; 
u1ey = out[1]; 
u1ez = out[2]; 


if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur256 Boundary u0 start point   %f %f %f \n",
        u0sx,   u0sy,   u0sz   );
fprintf(dbgfil(SURPAC),
"sur256 Boundary u0 end   point   %f %f %f \n",
        u0ex,   u0ey,   u0ez   );
fprintf(dbgfil(SURPAC),
"sur256 Boundary u1 end   point   %f %f %f \n",
        u1ex,   u1ey,   u1ez   );
fprintf(dbgfil(SURPAC),
"sur256 Boundary u1 start point   %f %f %f \n",
        u1sx,   u1sy,   u1sz   );
  fflush(dbgfil(SURPAC)); 
}

    return(SUCCED);

} /* End of function                                                */
/***********************************Only*for*Debug*On*(end)**********/
#endif

