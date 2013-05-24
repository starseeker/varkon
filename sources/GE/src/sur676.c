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
/*  Function: varkon_bpl_barycen                   File: sur676.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Barycentric coordinates for a triangular B-plane                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-09   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_bpl_barycen    Barycentric coord.'s for B-plane */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------------- Theory -------------------------------------*/
/*                                                                  */
/* Reference: Curves and Surfaces of Computer-aided design          */
/*            Robert C. Beach, p 194-197                            */
/*                                                                  */
/*                                                                  */
/*  p1    = Corner point 1 of B-plane triangle                      */
/*  p2    = Corner point 2 of B-plane triangle                      */
/*  p3    = p2                                                      */
/*  p4    = Corner point 4 of B-plane triangle                      */
/*  pi    = point inside triangle      p1 - p2 - p4                 */
/*  area  = area of the whole triangle p1 - p2 - p4                 */
/*  area1 = area of part triangle      p2 - pi - p4                 */
/*  area2 = area of part triangle      p1 - pi - p4                 */
/*  area3 = area of part triangle      p2 - pi - p1                 */
/*  alpha = Barycentric coordinate for point p1                     */
/*  beta  = Barycentric coordinate for point p2                     */
/*  gamma = Barycentric coordinate for point p4                     */
/*                                                                  */
/*  alpha = area1/area;                                             */
/*  beta  = area2/area;                                             */
/*  gamma = area3/area;                                             */
/*                                                                  */
/*  Area is calculated as half the length of the vector product     */
/*  for two sides of an triangle                                    */
/*                                                                  */
/*  A point is inside the triangle p1 - p2 - p4 provided that the   */
/*  the sum of alpha, beta and gamma is one (1.0).                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_bpl_barycen          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus  varkon_bpl_barycen (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBBplane *p_bpl,       /* Triangular B-plane p1-p2-p4       (ptr) */
  DBVector *p_poi,       /* Point in the B-plane              (ptr) */
  DBfloat  *p_alpha,     /* Barycentric coordinate for p1     (ptr) */
  DBfloat  *p_beta,      /* Barycentric coordinate for p2     (ptr) */
  DBfloat  *p_gamma )    /* Barycentric coordinate for p4     (ptr) */

/* Out:                                                             */
/*        Data to p_alpha, p_beta and p_gamma                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector p_1;          /* B-plane   corner point 1                */
  DBVector p_2;          /* B-plane   corner point 2                */
  DBVector p_3;          /* B-plane   corner point 3                */
  DBVector p_4;          /* B-plane   corner point 4                */
  DBVector p_i;          /* Input point p_poi in B-plane            */
  DBfloat  area;         /* Area of whole triangle p1-p2-p4         */
  DBfloat  area1;        /* Area of part  triangle p2-pi-p4         */
  DBfloat  area2;        /* Area of part  triangle p1-pi-p4         */
  DBfloat  area3;        /* Area of part  triangle p1-pi-p2         */
  DBfloat  sum;          /* Check sum (criterion if pi is inside)   */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector v_cross;      /* Vector product                          */
  DBfloat  dist;         /* Check distance between p2 and p3        */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur676 Enter*varkon_bpl_barycen* Barycentric coordinates\n");
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
 

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                  */
/*                                                                 !*/

   *p_alpha      = F_UNDEF;        /* Initialize output points      */
   *p_beta       = F_UNDEF;    
   *p_gamma      = F_UNDEF;    

/* Corner points and input B-plane point to local variables         */

   p_1 = p_bpl->crd1_bp;                   /* B-plane corner 1      */
   p_2 = p_bpl->crd2_bp;                   /* B-plane corner 2      */
   p_3 = p_bpl->crd3_bp;                   /* B-plane corner 3      */
   p_4 = p_bpl->crd4_bp;                   /* B-plane corner 4      */
   p_i.x_gm = p_poi->x_gm;
   p_i.y_gm = p_poi->y_gm;
   p_i.z_gm = p_poi->z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur676  p_1 %f %f %f\n", p_1.x_gm,  p_1.y_gm,  p_1.z_gm);
fprintf(dbgfil(SURPAC),
"sur676  p_2 %f %f %f\n", p_2.x_gm,  p_2.y_gm,  p_2.z_gm);
fprintf(dbgfil(SURPAC),
"sur676  p_4 %f %f %f\n", p_4.x_gm,  p_4.y_gm,  p_4.z_gm);
fflush (dbgfil(SURPAC));
}
#endif

   dist      =  SQRT( (p_2.x_gm-p_3.x_gm)*(p_2.x_gm-p_3.x_gm) + 
                      (p_2.y_gm-p_3.y_gm)*(p_2.y_gm-p_3.y_gm) + 
                      (p_2.z_gm-p_3.z_gm)*(p_2.z_gm-p_3.z_gm) );  

   if ( dist  > 0.02  )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur676  Error Not triangular B-plane p2-p3 distance is %25.10g\n",
              dist);
fflush (dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"(non-triangular)%%varkon_bpl_barycen(sur676)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* 2. Area of the whole triangle area                               */
/*                                                                 !*/

   v_cross.x_gm = (p_2.y_gm-p_1.y_gm) * (p_4.z_gm-p_1.z_gm) - 
                  (p_2.z_gm-p_1.z_gm) * (p_4.y_gm-p_1.y_gm);  
   v_cross.y_gm = (p_2.z_gm-p_1.z_gm) * (p_4.x_gm-p_1.x_gm) - 
                  (p_2.x_gm-p_1.x_gm) * (p_4.z_gm-p_1.z_gm);  
   v_cross.z_gm = (p_2.x_gm-p_1.x_gm) * (p_4.y_gm-p_1.y_gm) - 
                  (p_2.y_gm-p_1.y_gm) * (p_4.x_gm-p_1.x_gm);  

   area      =  SQRT( v_cross.x_gm*v_cross.x_gm + 
                      v_cross.y_gm*v_cross.y_gm + 
                      v_cross.z_gm*v_cross.z_gm ) / 2.0;  

   if ( area   < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur676  Error exit Input B-plane area is %25.10g (zero)\n", area );
fflush (dbgfil(SURPAC));
}
#endif
     sprintf(errbuf,"(area zero)%%varkon_bpl_barycen(sur676)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* 3. Area of the part  triangle area p2 - pi - p4                  */
/*                                                                 !*/

   v_cross.x_gm = (p_2.y_gm-p_i.y_gm) * (p_4.z_gm-p_i.z_gm) - 
                  (p_2.z_gm-p_i.z_gm) * (p_4.y_gm-p_i.y_gm);  
   v_cross.y_gm = (p_2.z_gm-p_i.z_gm) * (p_4.x_gm-p_i.x_gm) - 
                  (p_2.x_gm-p_i.x_gm) * (p_4.z_gm-p_i.z_gm);  
   v_cross.z_gm = (p_2.x_gm-p_i.x_gm) * (p_4.y_gm-p_i.y_gm) - 
                  (p_2.y_gm-p_i.y_gm) * (p_4.x_gm-p_i.x_gm);  

   area1     =  SQRT( v_cross.x_gm*v_cross.x_gm + 
                      v_cross.y_gm*v_cross.y_gm + 
                      v_cross.z_gm*v_cross.z_gm ) / 2.0;  


/*!                                                                 */
/* 4. Area of the part  triangle area p1 - pi - p4                  */
/*                                                                 !*/

   v_cross.x_gm = (p_1.y_gm-p_i.y_gm) * (p_4.z_gm-p_i.z_gm) - 
                  (p_1.z_gm-p_i.z_gm) * (p_4.y_gm-p_i.y_gm);  
   v_cross.y_gm = (p_1.z_gm-p_i.z_gm) * (p_4.x_gm-p_i.x_gm) - 
                  (p_1.x_gm-p_i.x_gm) * (p_4.z_gm-p_i.z_gm);  
   v_cross.z_gm = (p_1.x_gm-p_i.x_gm) * (p_4.y_gm-p_i.y_gm) - 
                  (p_1.y_gm-p_i.y_gm) * (p_4.x_gm-p_i.x_gm);  

   area2     =  SQRT( v_cross.x_gm*v_cross.x_gm + 
                      v_cross.y_gm*v_cross.y_gm + 
                      v_cross.z_gm*v_cross.z_gm ) / 2.0;  


/*!                                                                 */
/* 5. Area of the part  triangle area p2 - pi - p1                  */
/*                                                                 !*/

   v_cross.x_gm = (p_2.y_gm-p_i.y_gm) * (p_1.z_gm-p_i.z_gm) - 
                  (p_2.z_gm-p_i.z_gm) * (p_1.y_gm-p_i.y_gm);  
   v_cross.y_gm = (p_2.z_gm-p_i.z_gm) * (p_1.x_gm-p_i.x_gm) - 
                  (p_2.x_gm-p_i.x_gm) * (p_1.z_gm-p_i.z_gm);  
   v_cross.z_gm = (p_2.x_gm-p_i.x_gm) * (p_1.y_gm-p_i.y_gm) - 
                  (p_2.y_gm-p_i.y_gm) * (p_1.x_gm-p_i.x_gm);  

   area3     =  SQRT( v_cross.x_gm*v_cross.x_gm + 
                      v_cross.y_gm*v_cross.y_gm + 
                      v_cross.z_gm*v_cross.z_gm ) / 2.0;  

/*!                                                                 */
/* 6. Barycentric coordinates alpha, beta and gamma                 */
/*                                                                 !*/

   *p_alpha   = area1 / area;
   *p_beta    = area2 / area;
   *p_gamma   = area3 / area;


/*!                                                                 */
/* 7. Check if point pi is inside triangle p1 - p2 - p4             */
/*                                                                 !*/

   sum  = fabs ( *p_alpha + *p_beta + *p_gamma - 1.0 );

   if  ( sum > 0.0001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur676  Error exit Check sum alpha+beta+gamma-1= %25.10g > 0.0\n"
                     , sum );
fflush (dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"(pi outside)%%varkon_bpl_barycen(sur676)");
     return(varkon_erpush("SU2993",errbuf));
     }



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur676 Exit*varkon_bpl_barycen alpha %f beta %f gamma %f\n", 
    *p_alpha, *p_beta, *p_gamma);
fflush (dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
