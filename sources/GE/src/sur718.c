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
/*  Function: varkon_bpl_analyse                   File: sur718.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function analysis a B-plane polygon                         */
/*                                                                  */
/*  Note that function return value always is SUCCED.               */
/*                                                                  */
/*  Mer finns att g|ra n{r det g{ller kriteriet f|r fallen d}       */
/*  tre punkter ligger p} r{t linje. Kriteriet borde vara           */
/*  avst}nd till r{ta linjen (ber{kning finns i anabplav0.MBS)      */
/*  men det betyder fler r{kneoperationer. F|r extrapolering        */
/*  av B-plan (sur719) beh|vs annat kriterium (men dessa            */
/*  ber{kningarna hellre ska g|ras i sur719 .... )                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-27   Originally written                                 */
/*  1996-09-06   Comments dimension of o_n                          */
/*  1999-11-28   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_bpl_analyse    Analyse (classify) a B-plane     */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*  Colinear check with distance idpoint (extract from anabplanv0)  */
/*  FLOAT      cos_alfa;    ! Cosinus for closest point calculation */
/*  FLOAT      dist_clo;    ! Closest point distance                */
/*                                                                  */
/*  Triangle (p1, p2, p3 colinear) ?                                */
/*                                                                  */
/*  cos_alfa := v1*(p3-p1);                                         */
/*  cos_alfa := cos_alfa/VECL(v1)/VECL(p3-p1);                      */
/*  dist_clo := (1.0-cos_alfa*cos_alfa)*v1*v1;                      */
/*  dist_clo := SQRT(dist_clo);                                     */
/*  IF   dist_clo < idpoint THEN                                    */
/*    b_type :=  4;                                                 */
/*    p_nr   :=  2;                                                 */
/*    GOTO triangle;                                                */
/*  ENDIF;                                                          */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical point criterion               */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_bpl_analyse         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_bpl_analyse (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_p1,         /* Polygon point 1                   (ptr) */
  DBVector *p_p2,         /* Polygon point 2                   (ptr) */
  DBVector *p_p3,         /* Polygon point 3                   (ptr) */
  DBVector *p_p4,         /* Polygon point 4                   (ptr) */
  DBint   *p_btype,      /* Type of B-plane polygon           (ptr) */
                         /* Type of polygon                         */
                         /* Eq.  1: Triangle                        */
                         /* Eq.  2: Convex 4-point polygon          */
                         /* Eq.  3: Non-convex polygon              */
                         /* Eq.  4: p4, p1, p2 are colinear         */
                         /* Eq.  5: p1, p2, p3 are colinear         */
                         /* Eq.  6: p2, p3, p4 are colinear         */
                         /* Eq.  7: p3, p4, p1 are colinear         */
                         /* Eq. -1: p1 = p2                         */
                         /* Eq. -2: p3 = p4                         */
                         /* Eq. -3: p1 = p4                         */
                         /* Eq. -4: p1, p2, p3, p4 not coplanar     */
                         /* Eq. -5: Self-intersecting polygon       */
  DBint   *p_pnr )       /* Point number for:                 (ptr) */
                         /* Non-convex point   (p_btype= 3)         */
                         /* Colinear mid point (p_btype= 4,5,6,7)   */

/* Out:                                                             */
/*        Type of B-plane and point number (p_btype and p_pnr)      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector v1;           /* Edge vector v1= p2 - p1  (normalized)   */
  DBVector v2;           /* Edge vector v2= p3 - p2  (normalized)   */
  DBVector v3;           /* Edge vector v3= p4 - p3  (normalized)   */
  DBVector v4;           /* Edge vector v4= p1 - p4  (normalized)   */

  DBVector v41;          /* Cross vector v4 X v1                    */
  DBVector v12;          /* Cross vector v1 X v2                    */
  DBVector v23;          /* Cross vector v2 X v3                    */
  DBVector v34;          /* Cross vector v3 X v4                    */

  DBVector n_plane;      /* Plane normal                            */
  DBfloat  d_plane;      /* Plane D value                           */
  DBfloat  c_plane;      /* Plane D value as check (point 3)        */

  DBfloat  dot12;        /* Scalar product ( v41 ! v12 )            */
  DBfloat  dot23;        /* Scalar product ( v41 ! v23 )            */
  DBfloat  dot34;        /* Scalar product ( v41 ! v34 )            */

  DBint    o_nr[4];      /* Flag for opposite direction             */
                         /* (o_n[0] is not used)                    */
  DBint    n_opposit;    /* Number of vectors in opposite direct.   */

  DBfloat  length;       /* Length of vectors                       */
  DBfloat  idpoint;      /* Identical point criterion               */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur718 Enter varkon_bpl_analyse Analyse (classify) a B-plane\n");
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

  *p_btype = I_UNDEF;        /* Type of B-plane polygon            */
  *p_pnr   = I_UNDEF;        /* Non-convex point number            */

  d_plane  = F_UNDEF;       
  c_plane  = F_UNDEF;       

/*!                                                                 */
/* 2. Classify the B-plane polygon                                  */
/* ________________________________                                 */
/*                                                                 !*/

/*!                                                                 */
/* Edge vectors v1, v2, v3 and v4                                   */
/* Normalize vectors for the colinear check                         */
/*                                                                 !*/

   v1.x_gm =  p_p2->x_gm-p_p1->x_gm;
   v1.y_gm =  p_p2->y_gm-p_p1->y_gm;
   v1.z_gm =  p_p2->z_gm-p_p1->z_gm;

   length  =  SQRT(v1.x_gm*v1.x_gm+v1.y_gm*v1.y_gm+v1.z_gm*v1.z_gm);
   if ( length < idpoint ) 
        {
        *p_btype = -1;
        *p_pnr   =  0;
        goto  unaccept;
        }

   v1.x_gm =  v1.x_gm/length;
   v1.y_gm =  v1.y_gm/length;
   v1.z_gm =  v1.z_gm/length;

   v3.x_gm =  p_p4->x_gm-p_p3->x_gm;
   v3.y_gm =  p_p4->y_gm-p_p3->y_gm;
   v3.z_gm =  p_p4->z_gm-p_p3->z_gm;

   length  =  SQRT(v3.x_gm*v3.x_gm+v3.y_gm*v3.y_gm+v3.z_gm*v3.z_gm);
   if ( length < idpoint ) 
        {
        *p_btype = -2;
        *p_pnr   =  0;
        goto  unaccept;
        }

   v3.x_gm =  v3.x_gm/length;
   v3.y_gm =  v3.y_gm/length;
   v3.z_gm =  v3.z_gm/length;

   v4.x_gm =  p_p1->x_gm-p_p4->x_gm;
   v4.y_gm =  p_p1->y_gm-p_p4->y_gm;
   v4.z_gm =  p_p1->z_gm-p_p4->z_gm;

   length  =  SQRT(v4.x_gm*v4.x_gm+v4.y_gm*v4.y_gm+v4.z_gm*v4.z_gm);
   if ( length < idpoint ) 
        {
        *p_btype = -3;
        *p_pnr   =  0;
        goto  unaccept;
        }

   v4.x_gm =  v4.x_gm/length;
   v4.y_gm =  v4.y_gm/length;
   v4.z_gm =  v4.z_gm/length;

   v2.x_gm =  p_p3->x_gm-p_p2->x_gm;
   v2.y_gm =  p_p3->y_gm-p_p2->y_gm;
   v2.z_gm =  p_p3->z_gm-p_p2->z_gm;

   length  =  SQRT(v2.x_gm*v2.x_gm+v2.y_gm*v2.y_gm+v2.z_gm*v2.z_gm);
   if ( length < idpoint ) 
        {
        *p_btype =  1;
        *p_pnr   =  0;
        goto  triangle;
        }

   v2.x_gm =  v2.x_gm/length;
   v2.y_gm =  v2.y_gm/length;
   v2.z_gm =  v2.z_gm/length;

/*!                                                                 */
/* Cross vectors v41, v12, v23 and v41                              */
/*                                                                 !*/

   v41.x_gm = v4.y_gm * v1.z_gm - v4.z_gm * v1.y_gm;  
   v41.y_gm = v4.z_gm * v1.x_gm - v4.x_gm * v1.z_gm;  
   v41.z_gm = v4.x_gm * v1.y_gm - v4.y_gm * v1.x_gm;  

   v12.x_gm = v1.y_gm * v2.z_gm - v1.z_gm * v2.y_gm;  
   v12.y_gm = v1.z_gm * v2.x_gm - v1.x_gm * v2.z_gm;  
   v12.z_gm = v1.x_gm * v2.y_gm - v1.y_gm * v2.x_gm;  

   v23.x_gm = v2.y_gm * v3.z_gm - v2.z_gm * v3.y_gm;  
   v23.y_gm = v2.z_gm * v3.x_gm - v2.x_gm * v3.z_gm;  
   v23.z_gm = v2.x_gm * v3.y_gm - v2.y_gm * v3.x_gm;  

   v34.x_gm = v3.y_gm * v4.z_gm - v3.z_gm * v4.y_gm;  
   v34.y_gm = v3.z_gm * v4.x_gm - v3.x_gm * v4.z_gm;  
   v34.z_gm = v3.x_gm * v4.y_gm - v3.y_gm * v4.x_gm;  

/*!                                                                 */
/* Check if points are colinear, i.e if polygon is a triangle       */
/* The criterion is that the angle is less than 0.01 degrees ...    */
/* ... detta fasta kriterium borde ers{ttas med ................    */
/* (calculate normalized vector n_plane for the planar check)       */
/*                                                                 !*/


   length  =  SQRT(v41.x_gm*v41.x_gm+v41.y_gm*v41.y_gm+v41.z_gm*v41.z_gm);
   if ( length < 0.0001 )  
        {
        *p_btype =  4;
        *p_pnr   =  1;
        goto  triangle;
        }

   n_plane.x_gm =  v41.x_gm/length;
   n_plane.y_gm =  v41.y_gm/length;
   n_plane.z_gm =  v41.z_gm/length;

   length  =  SQRT(v12.x_gm*v12.x_gm+v12.y_gm*v12.y_gm+v12.z_gm*v12.z_gm);
   if ( length < 0.0001 )  
        {
        *p_btype =  5;
        *p_pnr   =  2;
        goto  triangle;
        }

   length  =  SQRT(v23.x_gm*v23.x_gm+v23.y_gm*v23.y_gm+v23.z_gm*v23.z_gm);
   if ( length < 0.0001 )  
        {
        *p_btype =  6;
        *p_pnr   =  3;
        goto  triangle;
        }

   length  =  SQRT(v34.x_gm*v34.x_gm+v34.y_gm*v34.y_gm+v34.z_gm*v34.z_gm);
   if ( length < 0.0001 )  
        {
        *p_btype =  7;
        *p_pnr   =  4;
        goto  triangle;
        }

/*!                                                                 */
/* Check if polygon is planar                                       */
/*                                                                 !*/

   d_plane =  p_p1->x_gm*n_plane.x_gm +   
              p_p1->y_gm*n_plane.y_gm +   
              p_p1->z_gm*n_plane.z_gm ;  

   c_plane =  p_p3->x_gm*n_plane.x_gm +   
              p_p3->y_gm*n_plane.y_gm +   
              p_p3->z_gm*n_plane.z_gm ;  


   if ( fabs(d_plane-c_plane) > idpoint ) 
        {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur718 Not coplanar pts d_plane %15.8f c_plane %15.8f \n", 
             d_plane, c_plane  );
fprintf(dbgfil(SURPAC),
"sur718 Diff. d_plane-c_plane %25.10f idpoint %25.10f\n", 
             d_plane-c_plane, idpoint  );
fflush(dbgfil(SURPAC)); 
}
#endif
        *p_btype = -4;
        *p_pnr   =  0;
        goto  unaccept;
        }

/*!                                                                 */
/* Compare vector directions with first corner vector v41           */
/*                                                                 !*/

   dot12 = v41.x_gm*v12.x_gm + v41.y_gm*v12.y_gm + v41.z_gm*v12.z_gm;  
   dot23 = v41.x_gm*v23.x_gm + v41.y_gm*v23.y_gm + v41.z_gm*v23.z_gm;  
   dot34 = v41.x_gm*v34.x_gm + v41.y_gm*v34.y_gm + v41.z_gm*v34.z_gm;  


/*!                                                                 */
/* Classification of the corner points                              */
/*                                                                 !*/

  o_nr[0]   =  0;     
  o_nr[1]   =  0;    
  o_nr[2]   =  0;   
  o_nr[3]   =  0;  
  n_opposit =  0;

  if ( dot12 <  0 )       
    {
    o_nr[1]    = -1;
    n_opposit  = n_opposit + 1;
    }      
  if ( dot23 <  0 )       
    {
    o_nr[2]    = -1;
    n_opposit  = n_opposit + 1;
    }      
  if ( dot34 <  0 )       
    {
    o_nr[3]    = -1;
    n_opposit  = n_opposit + 1;
    }      

  if      ( n_opposit == 0 )   
    {
    *p_btype =  2;
    *p_pnr   =  0;
    }
  else if ( n_opposit == 1 )   
    {
    *p_btype =  3;
    if   ( o_nr[1] < 0 ) *p_pnr   =  2;
    if   ( o_nr[2] < 0 ) *p_pnr   =  3;
    if   ( o_nr[3] < 0 ) *p_pnr   =  4;
    }
  else if ( n_opposit == 3 )   
    {
    *p_btype =  3;
    *p_pnr   =  1;
    }
  else if ( n_opposit == 2 )   
    {
    *p_btype = -5;
    *p_pnr   =  0;
    }


unaccept:; /* Unacceptable B-plane polygon                          */
triangle:; /* The input B-plane has three corner points             */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur718 Exit varkon_bpl_analyse Analyse classify B-plane *p_btype %d *p_pnr %d \n", 
             *p_btype , *p_pnr  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
