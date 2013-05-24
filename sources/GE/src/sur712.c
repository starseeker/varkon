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
/*  Function: varkon_lin_ptclose                   File: sur712.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the shortest distance from              */
/*  an external point to a straight line.                           */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-08-22   Originally written                                 */
/*  1996-11-10   Bug: Error for p_dist= 0                           */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_lin_ptclose    Closest point to a straight line */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* Closest point to a straight line                                 */
/* --------------------------------                                 */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* Find the shortest distance dist between a point and a line       */
/* and the corresponding point p_u on the line.                     */
/*                                                                  */
/* Notations:                                                       */
/*                                                                  */
/* Line:  r(s)= p0 + s*u_n                                          */
/* Point: p_e                                                       */
/*                                                                  */
/* The vector from p_u to p_e must be perpendicular to the          */
/* direction vector for the line.                                   */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* The scalar product (1) must be zero:                             */
/*                                                                  */
/* (u_n ! (p_e - p0-s*u_n)) = 0                     (1)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* (u_n ! (p_e-p0)) - (u_n ! s*u_n) = 0             (1)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/* (u_n ! (p_e-p0)) = s*(u_n ! u_n) = 0             (1)             */
/*                                                                  */
/*                 <==>                                             */
/*                                                                  */
/*             (u_n ! (p_e-p0))                                     */
/*      s = ---------------------                                   */
/*             (u_n ! u_n)                                          */
/*                                                                  */
/*  Note that the vector u= u_n not is normalized. An output        */
/*  parameter value between 0 and 1 means that the output,          */
/*  closest point is on the line. Values outside 0-1 means          */
/*  that the closest point is on the extended line.                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_lengv           * Length of a vector                      */
/* varkon_scalar          * Vector dot product.                     */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2511 = Parallel lines in varkon_lin_ptclose                    */
/* SU2943 = Called function xxxxxx failed in varkon_lin_ptclose     */
/* SU2993 = Program error ( ) in varkon_lin_ptclose         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus  varkon_lin_ptclose (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_p0,        /* Start point      for the line     (ptr) */
  DBVector *p_u,         /* Direction vector for the line     (ptr) */
  DBVector *p_pe,        /* External point                    (ptr) */
  DBVector *p_pu,        /* Nearest point the line            (ptr) */
  DBfloat  *p_s,         /* Parameter for the nearest point   (ptr) */
  DBfloat  *p_dist )     /* Distance                          (ptr) */

/* Out:                                                             */
/*        Nearest point  and distance                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  u_n[3];       /* Direction vector for line               */
  DBfloat  s_l;          /* Parameter value for line                */
  DBfloat  pe_p0[3];     /* Vector p_e-p0                           */
  DBfloat  pe_pc[3];     /* Vector p_e-p_closest                    */
  DBfloat  dot_upep0;    /* Dot product u_n*(p_e-p0)                */
  DBfloat  dot_uu;       /* Dot product u_n*u_n                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  length;       /* Length of a vector (p1-p0)              */
  DBfloat  idpoint;      /* Identical point criterion               */
  short    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur712 Enter*varkon_lin_ptclose Closest pt on line ****\n");
}
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur712 p_p0   %f %f %f\n",p_p0->x_gm, p_p0->y_gm, p_p0->z_gm );
  fprintf(dbgfil(SURPAC),
  "sur712 p_u    %f %f %f\n",p_u->x_gm, p_u->y_gm, p_u->z_gm );
}
#endif
 

/*!                                                                 */
/* 1. Initializations and check of input data                       */
/* __________________________________________                       */
/*                                                                  */
/* Check length of u. Call of lengv.                                */
/*                                                                 !*/

   idpoint   = varkon_idpoint();

   u_n[0] = p_u->x_gm;
   u_n[1] = p_u->y_gm;
   u_n[2] = p_u->z_gm;

   status=varkon_lengv(u_n,&length);
   if (status<0) 
     {
     sprintf(errbuf,"lengv %%varkon_lin_ptclose(sur712)");
     return(varkon_erpush("SU2943",errbuf));
     }
 
   if ( length < idpoint ) 
     {
     sprintf(errbuf,"length=0%%varkon_lin_ptclose(sur712)");
     return(varkon_erpush("SU2993",errbuf));
     }
 
/*!                                                                 */
/* 2. Vectors and dot products                                      */
/* ___________________________                                      */

/* Let vector pe_p0= p_e - p0.                                      */
/* Calculate dot product u_n*(p_e-p0).  Call of scalar.             */
/* Let nearest points be p1 and p0 and return                       */
/* if the length is zero ( < TOL2 ).                                */
/*                                                                 !*/

   pe_p0[0] = p_pe->x_gm-p_p0->x_gm;
   pe_p0[1] = p_pe->y_gm-p_p0->y_gm;
   pe_p0[2] = p_pe->z_gm-p_p0->z_gm;


   status=varkon_scalar(u_n,pe_p0,&dot_upep0);

/* Negative status for zero vectors.                                */
/* u is already checked. pe_po= 0 OK                                */

   status=varkon_scalar(u_n,u_n,&dot_uu);

/* Negative status for zero vectors. This is already is checked     */

/*!                                                                 */
/* 3. Shortest distance and closest points                          */
/* _______________________________________                          */
/*                                                                  */
/* Let output distance *p_dist= dot_p1p0_vu/cross_vu_l.             */
/* Let s_l= dot_upep0 / dot_uu                                      */
/* Let output point p_pu = p0 + s_l*u_n                             */
/* Calculate output distance. Call of lengv.                        */
/*                                                                 !*/

   s_l= dot_upep0 / dot_uu;    

   (*p_pu).x_gm  = p_p0->x_gm + s_l*u_n[0];
   (*p_pu).y_gm  = p_p0->y_gm + s_l*u_n[1];
   (*p_pu).z_gm  = p_p0->z_gm + s_l*u_n[2];

   *p_s = s_l;

   pe_pc[0] = p_pe->x_gm-p_pu->x_gm;
   pe_pc[1] = p_pe->y_gm-p_pu->y_gm;
   pe_pc[2] = p_pe->z_gm-p_pu->z_gm;

   *p_dist =  SQRT( pe_pc[0]*pe_pc[0] +
                    pe_pc[1]*pe_pc[1] +
                    pe_pc[2]*pe_pc[2] );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur712 Closest point p_pu %f %f %f\n",
             p_pu->x_gm, p_pu->y_gm, p_pu->z_gm );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur712 Exit* Parameter %f Distance= %f\n",*p_s, *p_dist);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
