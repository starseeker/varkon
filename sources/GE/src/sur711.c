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
/*  Function: varkon_pla_distpt                    File: sur711.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the distance to a plane                 */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-08-20   Originally written                                 */
/*  1997-02-09   Elimination of compilation warning                 */
/*  1999-11-26   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pla_distpt     Distance point to 3 point plane  */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical point criterion               */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_pla_distpt          Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_pla_distpt (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_p1,        /* Plane   point 1                   (ptr) */
  DBVector *p_p2,        /* Plane   point 2                   (ptr) */
  DBVector *p_p3,        /* Plane   point 3                   (ptr) */
  DBVector *p_ext,       /* External point                    (ptr) */
  DBVector *p_ref,       /* Reference vector for sign         (ptr) */
                         /* Zero vector for undefined vector        */
  DBfloat  *p_dist )     /* Distance to point                 (ptr) */

/* Out:                                                             */
/*        Distance to plane                                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector v1;           /* Edge vector v1= p2 - p1  (normalized)   */
  DBVector v2;           /* Edge vector v2= p3 - p2  (normalized)   */

  DBVector v12;          /* Cross vector v1 X v2                    */

  DBVector n_plane;      /* Plane normal                            */
  DBfloat  d_plane;      /* Plane D value                           */
  DBfloat  c_plane;      /* Plane D value for external point        */

  DBfloat  dot_n;        /* Scalar product ( v12 ! ref )            */

  DBfloat  length;       /* Length of vectors                       */
  DBfloat  idpoint;      /* Identical point criterion               */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 Enter varkon_pla_distpt  Calculate distance to plane \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 p1  %f %f %f\n",p_p1->x_gm,p_p1->y_gm,p_p1->z_gm );
fprintf(dbgfil(SURPAC),
"sur711 p2  %f %f %f\n",p_p2->x_gm,p_p2->y_gm,p_p2->z_gm );
fprintf(dbgfil(SURPAC),
"sur711 p3  %f %f %f\n",p_p3->x_gm,p_p3->y_gm,p_p3->z_gm );
fprintf(dbgfil(SURPAC),
"sur711 ext %f %f %f\n",p_ext->x_gm,p_ext->y_gm,p_ext->z_gm );
fprintf(dbgfil(SURPAC),
"sur711 ref %f %f %f\n",p_ref->x_gm,p_ref->y_gm,p_ref->z_gm );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/* Get identical points criterion idpoint.                          */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

   idpoint=varkon_idpoint();

  *p_dist  = F_UNDEF;       /* Initialize to negative value         */

  d_plane  = F_UNDEF;       
  c_plane  = F_UNDEF;       

/*!                                                                 */
/* 2. Vectors in the plane v1 and v2                                */
/* ________________________________                                 */
/*                                                                 !*/

/*!                                                                 */
/* Edge vectors v1, v2                                              */
/* Normalize vectors for the colinear check                         */
/*                                                                 !*/

   v1.x_gm =  p_p2->x_gm-p_p1->x_gm;
   v1.y_gm =  p_p2->y_gm-p_p1->y_gm;
   v1.z_gm =  p_p2->z_gm-p_p1->z_gm;

   length  =  SQRT(v1.x_gm*v1.x_gm+v1.y_gm*v1.y_gm+v1.z_gm*v1.z_gm);
   if ( length < idpoint ) 
        {
#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 Failure p1 to p2 distance %f \n", length );
fflush(dbgfil(SURPAC));
}
#endif
        goto  unaccept;
        }

   v1.x_gm =  v1.x_gm/length;
   v1.y_gm =  v1.y_gm/length;
   v1.z_gm =  v1.z_gm/length;

   v2.x_gm =  p_p3->x_gm-p_p1->x_gm;
   v2.y_gm =  p_p3->y_gm-p_p1->y_gm;
   v2.z_gm =  p_p3->z_gm-p_p1->z_gm;

   length  =  SQRT(v2.x_gm*v2.x_gm+v2.y_gm*v2.y_gm+v2.z_gm*v2.z_gm);
   if ( length < idpoint ) 
        {
#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 Failure p1 to p3 distance %f \n", length );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
        goto  unaccept;
        }

   v2.x_gm =  v2.x_gm/length;
   v2.y_gm =  v2.y_gm/length;
   v2.z_gm =  v2.z_gm/length;

/*!                                                                 */
/* Cross vectors v41, v12, v23 and v41                              */
/*                                                                 !*/

   v12.x_gm = v1.y_gm * v2.z_gm - v1.z_gm * v2.y_gm;  
   v12.y_gm = v1.z_gm * v2.x_gm - v1.x_gm * v2.z_gm;  
   v12.z_gm = v1.x_gm * v2.y_gm - v1.y_gm * v2.x_gm;  

/*!                                                                 */
/* Calculate normalized vector n_plane                              */
/* (Check if points are colinear, i.e if points are on a line       */
/*  The criterion is that the angle is less than 0.01 degrees .     */
/*  ... detta fasta kriterium borde ers{ttas med ...............)   */
/*                                                                 !*/


   length = SQRT(v12.x_gm*v12.x_gm+v12.y_gm*v12.y_gm+v12.z_gm*v12.z_gm);
   if ( length < 0.0001 )  
        {
#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 Failure Cross vector length %f \n", length );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
        goto  unaccept;
        }

   n_plane.x_gm =  v12.x_gm/length;
   n_plane.y_gm =  v12.y_gm/length;
   n_plane.z_gm =  v12.z_gm/length;

/*!                                                                 */
/* Compare with reference vector and reverse plane normal           */
/* if the reference vector is in the opposite direction.            */
/*                                                                 !*/

   length= SQRT(p_ref->x_gm*p_ref->x_gm+p_ref->y_gm*p_ref->y_gm+
                p_ref->z_gm*p_ref->z_gm);
   if ( length > idpoint ) 
     {
     dot_n = p_ref->x_gm*v12.x_gm + 
             p_ref->y_gm*v12.y_gm + 
             p_ref->z_gm*v12.z_gm;  
     if ( dot_n < 0.0 )
       {
       n_plane.x_gm = -n_plane.x_gm;
       n_plane.y_gm = -n_plane.y_gm;
       n_plane.z_gm = -n_plane.z_gm;
       }
     }


/*!                                                                 */
/* Calculate D value for plane with point p1                        */
/*                                                                 !*/

   d_plane =  p_p1->x_gm*n_plane.x_gm +   
              p_p1->y_gm*n_plane.y_gm +   
              p_p1->z_gm*n_plane.z_gm ;  

/*!                                                                 */
/* Calculate D value for plane with external point                  */
/*                                                                 !*/

   c_plane =  p_ext->x_gm*n_plane.x_gm +   
              p_ext->y_gm*n_plane.y_gm +   
              p_ext->z_gm*n_plane.z_gm ;  

/*!                                                                 */
/* Calculate distance with sign.                                    */
/*                                                                 !*/

   *p_dist = c_plane - d_plane;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur711 Exit varkon_pla_distpt  Distance *p_dist %f \n", *p_dist );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

unaccept:; /* Plane cannot be defined                               */

    sprintf(errbuf,"sur711%% ");
    return(varkon_erpush("SU1303",errbuf));

} /* End of function */


/********************************************************************/
