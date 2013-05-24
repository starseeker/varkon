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
/*  Function: varkon_lin_2plane                    File: sur674.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculate line (two points) as intersect beween two B_PLANE's   */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-06   Originally written                                 */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_lin_2plane     Two B-plane intersect line       */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_poi_3plane      * Three plane intersect point             */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2293 = B-plane inters. line within polygons but no common part */
/* SU2303 = B-plane intersect line not within polygon               */
/* SU2313 = No B-plane intersect line. Planes parallell             */
/* SU2993 = Severe program error ( ) in varkon_lin_2plane           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus   varkon_lin_2plane (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBBplane *p_bpl1,      /* B-plane 1                         (ptr) */
  DBBplane *p_bpl2,      /* B-plane 2                         (ptr) */
  DBint    in_lin_no,    /* Requested output line                   */
                         /* Eq.  12: Common part inside polygons    */
                         /* Eq.   1: Intersect B-plane 1 polygon    */
                         /* Eq.   2: Intersect B-plane 2 polygon    */
                         /* Eq.  99: Line in unbounded planes       */
                         /* If < 0 determine if intersect exists    */
                         /* There will be no error in this case     */
  DBVector *p_ps_out,    /* Start point intersect line        (ptr) */
  DBVector *p_pe_out,    /* End   point intersect line        (ptr) */
  DBint    *p_nlin )     /* No of output lines 0 or 1         (ptr) */

/* Out:                                                             */
/*        Point data to *p_ps_out and *p_pe_out and p_nlin          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    lin_no;       /* Requested output line                   */
                         /* Eq.  12: Common part inside polygons    */
                         /* Eq.   1: Intersect B-plane 1 polygon    */
                         /* Eq.   2: Intersect B-plane 2 polygon    */
                         /* Eq.  99: Line in unbounded planes       */
  DBVector p11;          /* B-plane 1 corner point 1                */
  DBVector p12;          /* B-plane 1 corner point 2                */
  DBVector p13;          /* B-plane 1 corner point 3                */
  DBVector p14;          /* B-plane 1 corner point 4                */
  DBVector p21;          /* B-plane 2 corner point 1                */
  DBVector p22;          /* B-plane 2 corner point 2                */
  DBVector p23;          /* B-plane 2 corner point 3                */
  DBVector p24;          /* B-plane 2 corner point 4                */
  DBVector v_b1;         /* B-plane 1 normal                        */
  DBVector v_b2;         /* B-plane 2 normal                        */
  DBfloat  d_b1;         /* B-plane 1 D value                       */
  DBfloat  d_b2;         /* B-plane 2 D value                       */


  DBint    n_plane1;     /* Number of intersect points plane 1      */
  DBint    n_plane2;     /* Number of intersect points plane 2      */
  DBVector p_start1;     /* Start point line in plane 1             */
  DBVector p_end1;       /* End   point line in plane 1             */
  DBVector p_start2;     /* Start point line in plane 2             */
  DBVector p_end2;       /* End   point line in plane 2             */

  DBVector v_p11_p12;    /* Plane p11-p12/v_b1 normal               */
  DBVector v_p12_p13;    /* Plane p12-p13/v_b1 normal               */
  DBVector v_p13_p14;    /* Plane p13-p14/v_b1 normal               */
  DBVector v_p14_p11;    /* Plane p14-p11/v_b1 normal               */
  DBVector v_p21_p22;    /* Plane p21-p22/v_b2 normal               */
  DBVector v_p22_p23;    /* Plane p22-p23/v_b2 normal               */
  DBVector v_p23_p24;    /* Plane p23-p24/v_b2 normal               */
  DBVector v_p24_p21;    /* Plane p24-p21/v_b2 normal               */
  DBfloat  d_p11_p12;    /* Plane p11-p12/v_b1 D value              */
  DBfloat  d_p12_p13;    /* Plane p12-p13/v_b1 D value              */
  DBfloat  d_p13_p14;    /* Plane p13-p14/v_b1 D value              */
  DBfloat  d_p14_p11;    /* Plane p14-p11/v_b1 D value              */
  DBfloat  d_p21_p22;    /* Plane p21-p22/v_b2 D value              */
  DBfloat  d_p22_p23;    /* Plane p22-p23/v_b2 D value              */
  DBfloat  d_p23_p24;    /* Plane p23-p24/v_b2 D value              */
  DBfloat  d_p24_p21;    /* Plane p24-p21/v_b2 D value              */
  DBVector p11_p12;      /* Intersect pt p11-p12/v_b1               */
  DBVector p12_p13;      /* Intersect pt p12-p13/v_b1               */
  DBVector p13_p14;      /* Intersect pt p13-p14/v_b1               */
  DBVector p14_p11;      /* Intersect pt p14-p11/v_b1               */
  DBVector p21_p22;      /* Intersect pt p21-p22/v_b2               */
  DBVector p22_p23;      /* Intersect pt p22-p23/v_b2               */
  DBVector p23_p24;      /* Intersect pt p23-p24/v_b2               */
  DBVector p24_p21;      /* Intersect pt p24-p21/v_b2               */

  DBfloat  mid_che;      /* For the check if intersect pt           */
                         /* is between start and end line pt        */
  DBfloat  mids_che;     /* For the check if intersect pt           */
                         /* is between start and end line pt        */
  DBfloat  mide_che;     /* For the check if intersect pt           */
                         /* is between start and end line pt        */
  DBint    stat_int;     /* Error code poi3planv0                   */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector p_temp;       /* Temporary point for shift of points     */
  DBfloat  length;       /* Length of input plane vectors           */
  DBfloat  dot;          /* Scalar product (check parallell planes) */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                  */
/*                                                                 !*/

   p_ps_out->x_gm = 12345.6789;    /* Initialize output points      */
   p_ps_out->y_gm = 12345.6789;
   p_ps_out->z_gm = 12345.6789; 
   p_pe_out->x_gm = 12345.6789; 
   p_pe_out->y_gm = 12345.6789;
   p_pe_out->z_gm = 12345.6789; 
   *p_nlin        = 0;   

   if   ( in_lin_no < 0 ) lin_no = -in_lin_no;
   else                   lin_no =  in_lin_no;

   if      ( lin_no == 12 || lin_no == 1 || lin_no == 2 )  
     {
     ;
     }
   else if ( lin_no == 99 )  
     {
     sprintf(errbuf,"(lin_no %d not yet implemented)%%sur674",lin_no);
     return(varkon_erpush("SU2993",errbuf));
     }
   else
     {
     sprintf(errbuf,"(lin_no %d)%%varkon_lin_2plane (sur674)",lin_no);
     return(varkon_erpush("SU2993",errbuf));
     }

   n_plane1 = 0;              /* Number of intersect points plane 1 */
   n_plane2 = 0;              /* Number of intersect points plane 2 */

   p_start1.x_gm  = 12345.6789;
   p_start1.y_gm  = 12345.6789;
   p_start1.z_gm  = 12345.6789;
   p_end1.x_gm    = 12345.6789;
   p_end1.y_gm    = 12345.6789;
   p_end1.z_gm    = 12345.6789;
   p_start2.x_gm  = 12345.6789;
   p_start2.y_gm  = 12345.6789;
   p_start2.z_gm  = 12345.6789;
   p_end2.x_gm    = 12345.6789;
   p_end2.y_gm    = 12345.6789;
   p_end2.z_gm    = 12345.6789;

/* Corner points to local variables                                 */

   p11 = p_bpl1->crd1_bp;                /* B-plane 1 corner 1      */
   p12 = p_bpl1->crd2_bp;                /* B-plane 1 corner 2      */
   p13 = p_bpl1->crd3_bp;                /* B-plane 1 corner 3      */
   p14 = p_bpl1->crd4_bp;                /* B-plane 1 corner 4      */
   p21 = p_bpl2->crd1_bp;                /* B-plane 2 corner 1      */
   p22 = p_bpl2->crd2_bp;                /* B-plane 2 corner 2      */
   p23 = p_bpl2->crd3_bp;                /* B-plane 2 corner 3      */
   p24 = p_bpl2->crd4_bp;                /* B-plane 2 corner 4      */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur674  p11 %f %f %f\n", p11.x_gm,  p11.y_gm,  p11.z_gm);
fprintf(dbgfil(SURPAC),
"sur674  p12 %f %f %f\n", p12.x_gm,  p12.y_gm,  p12.z_gm);
fflush (dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 2. Input plane normals and D values                              */
/*                                                                 !*/

   v_b1.x_gm = (p12.y_gm-p11.y_gm) * (p14.z_gm-p11.z_gm) - 
               (p12.z_gm-p11.z_gm) * (p14.y_gm-p11.y_gm);  
   v_b1.y_gm = (p12.z_gm-p11.z_gm) * (p14.x_gm-p11.x_gm) - 
               (p12.x_gm-p11.x_gm) * (p14.z_gm-p11.z_gm);  
   v_b1.z_gm = (p12.x_gm-p11.x_gm) * (p14.y_gm-p11.y_gm) - 
               (p12.y_gm-p11.y_gm) * (p14.x_gm-p11.x_gm);  

   length    =  SQRT( v_b1.x_gm*v_b1.x_gm + 
                      v_b1.y_gm*v_b1.y_gm + 
                      v_b1.z_gm*v_b1.z_gm );  

   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Input plane normal v_b1 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_b1 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_b1.x_gm = v_b1.x_gm / length;
   v_b1.y_gm = v_b1.y_gm / length;
   v_b1.z_gm = v_b1.z_gm / length;

   d_b1 = p11.x_gm*v_b1.x_gm + p11.y_gm*v_b1.y_gm + p11.z_gm*v_b1.z_gm;


   v_b2.x_gm = (p22.y_gm-p21.y_gm) * (p24.z_gm-p21.z_gm) - 
               (p22.z_gm-p21.z_gm) * (p24.y_gm-p21.y_gm);  
   v_b2.y_gm = (p22.z_gm-p21.z_gm) * (p24.x_gm-p21.x_gm) - 
               (p22.x_gm-p21.x_gm) * (p24.z_gm-p21.z_gm);  
   v_b2.z_gm = (p22.x_gm-p21.x_gm) * (p24.y_gm-p21.y_gm) - 
               (p22.y_gm-p21.y_gm) * (p24.x_gm-p21.x_gm);  

   length    =  SQRT( v_b2.x_gm*v_b2.x_gm + 
                      v_b2.y_gm*v_b2.y_gm + 
                      v_b2.z_gm*v_b2.z_gm );  

 
   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Input plane normal v_b2 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_b2 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_b2.x_gm = v_b2.x_gm / length;
   v_b2.y_gm = v_b2.y_gm / length;
   v_b2.z_gm = v_b2.z_gm / length;

   d_b2 = p21.x_gm*v_b2.x_gm + p21.y_gm*v_b2.y_gm + p21.z_gm*v_b2.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur674 v_b1 %f %f %f d_b1 %f\n",v_b1.x_gm, v_b1.y_gm, v_b1.z_gm, d_b1);
fprintf(dbgfil(SURPAC),
"sur674 v_b2 %f %f %f d_b2 %f\n",v_b2.x_gm, v_b2.y_gm, v_b2.z_gm, d_b1);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif


/*!                                                                 */
/* 3. Check if planes are parallell (no intersection)               */
/*                                                                  */
/*                                                                 !*/

   dot = v_b1.x_gm*v_b2.x_gm+v_b1.y_gm*v_b2.y_gm+v_b1.z_gm*v_b2.z_gm;  

   if ( fabs(dot-1.0) < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error Input planes are parallell fabs(dot-1) %25.10g\n", 
                  fabs(dot-1));
fflush (dbgfil(SURPAC)); 
}
#endif
     if   ( in_lin_no < 0 ) goto no_error; 
     sprintf(errbuf," %%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2313",errbuf));
     }

/*!                                                                 */
/* 4. Calculate intersects between plane 1 boundaries and plane 2   */
/*                                                                 !*/

/*!                                                                 */
/* All calculated as three planes intersect                         */
/* Intersect p11_p12: Plane p11-p12 and v_b1, plane 1 and plane 2   */
/*                                                                 !*/

   v_p11_p12.x_gm = (p11.y_gm-p12.y_gm) * v_b1.z_gm - 
                    (p11.z_gm-p12.z_gm) * v_b1.y_gm;  
   v_p11_p12.y_gm = (p11.z_gm-p12.z_gm) * v_b1.x_gm - 
                    (p11.x_gm-p12.x_gm) * v_b1.z_gm;  
   v_p11_p12.z_gm = (p11.x_gm-p12.x_gm) * v_b1.y_gm - 
                    (p11.y_gm-p12.y_gm) * v_b1.x_gm;  

   length    =  SQRT( v_p11_p12.x_gm*v_p11_p12.x_gm + 
                      v_p11_p12.y_gm*v_p11_p12.y_gm + 
                      v_p11_p12.z_gm*v_p11_p12.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Normal v_p11_p12 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_p11_p12 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p11_p12.x_gm = v_p11_p12.x_gm / length;
   v_p11_p12.y_gm = v_p11_p12.y_gm / length;
   v_p11_p12.z_gm = v_p11_p12.z_gm / length;

   d_p11_p12 = p11.x_gm*v_p11_p12.x_gm +
               p11.y_gm*v_p11_p12.y_gm +
               p11.z_gm*v_p11_p12.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p11_p12, d_p11_p12, v_b1, d_b1, v_b2, d_b2, &p11_p12 );

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur674 v_p11_p12 %f %f %f d_p11_p12 %f\n",
v_p11_p12.x_gm, v_p11_p12.y_gm, v_p11_p12.z_gm, d_p11_p12);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif

    if (stat_int == 0)
      {
      mid_che = (p11_p12.x_gm-p11.x_gm)*(p12.x_gm-p11_p12.x_gm) +
                (p11_p12.y_gm-p11.y_gm)*(p12.y_gm-p11_p12.y_gm) +
                (p11_p12.z_gm-p11.z_gm)*(p12.z_gm-p11_p12.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if ( mid_che >= 0 )
      {
      n_plane1 = n_plane1 + 1;   /* Intersect point on boundary */
      p_start1 = p11_p12; 
      }

/*!                                                                 */
/* Intersect p12_p13: Plane p12-p13 and v_b1, plane 1 and plane 2   */
/*                                                                 !*/

   length    =  SQRT( (p12.x_gm-p13.x_gm)*(p12.x_gm-p13.x_gm) + 
                      (p12.y_gm-p13.y_gm)*(p12.y_gm-p13.y_gm) + 
                      (p12.z_gm-p13.z_gm)*(p12.z_gm-p13.z_gm) );  

  if ( length < 0.001 ) goto no_p12_p13;  /* Borde vara ctol */

   v_p12_p13.x_gm = (p12.y_gm-p13.y_gm) * v_b1.z_gm - 
                    (p12.z_gm-p13.z_gm) * v_b1.y_gm;  
   v_p12_p13.y_gm = (p12.z_gm-p13.z_gm) * v_b1.x_gm - 
                    (p12.x_gm-p13.x_gm) * v_b1.z_gm;  
   v_p12_p13.z_gm = (p12.x_gm-p13.x_gm) * v_b1.y_gm - 
                    (p12.y_gm-p13.y_gm) * v_b1.x_gm;  

   length    =  SQRT( v_p12_p13.x_gm*v_p12_p13.x_gm + 
                      v_p12_p13.y_gm*v_p12_p13.y_gm + 
                      v_p12_p13.z_gm*v_p12_p13.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Normal v_p12_p13 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_p12_p13 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p12_p13.x_gm = v_p12_p13.x_gm / length;
   v_p12_p13.y_gm = v_p12_p13.y_gm / length;
   v_p12_p13.z_gm = v_p12_p13.z_gm / length;

   d_p12_p13 = p12.x_gm*v_p12_p13.x_gm +
               p12.y_gm*v_p12_p13.y_gm +
               p12.z_gm*v_p12_p13.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p12_p13, d_p12_p13, v_b1, d_b1, v_b2, d_b2, &p12_p13 );

    if (stat_int == 0)
      {
      mid_che = (p12_p13.x_gm-p12.x_gm)*(p13.x_gm-p12_p13.x_gm) +
                (p12_p13.y_gm-p12.y_gm)*(p13.y_gm-p12_p13.y_gm) +
                (p12_p13.z_gm-p12.z_gm)*(p13.z_gm-p12_p13.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane1 == 0 )
      {
      n_plane1 = n_plane1 + 1;   /* Start line point on boundary */
      p_start1 = p12_p13; 
      }
    else if ( mid_che >= 0 && n_plane1 == 1 )
      {
      n_plane1 = n_plane1 + 1;   /* End   line point on boundary */
      p_end1   = p12_p13; 
      }
    else if ( mid_che >= 0 && n_plane1 == 2 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit n_plane1 > 2  \n");
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
      sprintf(errbuf,"(n_plane1 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }

no_p12_p13:;  /* No line p12-p13 */



/*!                                                                 */
/* Intersect p13_p14: Plane p13-p14 and v_b1, plane 1 and plane 2   */
/*                                                                 !*/

   v_p13_p14.x_gm = (p13.y_gm-p14.y_gm) * v_b1.z_gm - 
                    (p13.z_gm-p14.z_gm) * v_b1.y_gm;  
   v_p13_p14.y_gm = (p13.z_gm-p14.z_gm) * v_b1.x_gm - 
                    (p13.x_gm-p14.x_gm) * v_b1.z_gm;  
   v_p13_p14.z_gm = (p13.x_gm-p14.x_gm) * v_b1.y_gm - 
                    (p13.y_gm-p14.y_gm) * v_b1.x_gm;  

   length    =  SQRT( v_p13_p14.x_gm*v_p13_p14.x_gm + 
                      v_p13_p14.y_gm*v_p13_p14.y_gm + 
                      v_p13_p14.z_gm*v_p13_p14.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Normal v_p13_p14 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_p13_p14 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p13_p14.x_gm = v_p13_p14.x_gm / length;
   v_p13_p14.y_gm = v_p13_p14.y_gm / length;
   v_p13_p14.z_gm = v_p13_p14.z_gm / length;

   d_p13_p14 = p13.x_gm*v_p13_p14.x_gm +
               p13.y_gm*v_p13_p14.y_gm +
               p13.z_gm*v_p13_p14.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p13_p14, d_p13_p14, v_b1, d_b1, v_b2, d_b2, &p13_p14 );

    if (stat_int == 0)
      {
      mid_che = (p13_p14.x_gm-p13.x_gm)*(p14.x_gm-p13_p14.x_gm) +
                (p13_p14.y_gm-p13.y_gm)*(p14.y_gm-p13_p14.y_gm) +
                (p13_p14.z_gm-p13.z_gm)*(p14.z_gm-p13_p14.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane1 == 0 )
      {
      n_plane1 = n_plane1 + 1;   /* Start line point on boundary */
      p_start1 = p13_p14; 
      }
    else if ( mid_che >= 0 && n_plane1 == 1 )
      {
      n_plane1 = n_plane1 + 1;   /* End   line point on boundary */
      p_end1   = p13_p14; 
      }
    else if ( mid_che >= 0 && n_plane1 == 2 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit n_plane1 > 2  \n");
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
      sprintf(errbuf,"(n_plane1 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Intersect p14_p11: Plane p14-p11 and v_b1, plane 1 and plane 2   */
/*                                                                 !*/

   v_p14_p11.x_gm = (p14.y_gm-p11.y_gm) * v_b1.z_gm - 
                    (p14.z_gm-p11.z_gm) * v_b1.y_gm;  
   v_p14_p11.y_gm = (p14.z_gm-p11.z_gm) * v_b1.x_gm - 
                    (p14.x_gm-p11.x_gm) * v_b1.z_gm;  
   v_p14_p11.z_gm = (p14.x_gm-p11.x_gm) * v_b1.y_gm - 
                    (p14.y_gm-p11.y_gm) * v_b1.x_gm;  

   length    =  SQRT( v_p14_p11.x_gm*v_p14_p11.x_gm + 
                      v_p14_p11.y_gm*v_p14_p11.y_gm + 
                      v_p14_p11.z_gm*v_p14_p11.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Normal v_p14_p11 length %25.10g\n", length);
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"(v_p14_p11 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p14_p11.x_gm = v_p14_p11.x_gm / length;
   v_p14_p11.y_gm = v_p14_p11.y_gm / length;
   v_p14_p11.z_gm = v_p14_p11.z_gm / length;

   d_p14_p11 = p14.x_gm*v_p14_p11.x_gm +
               p14.y_gm*v_p14_p11.y_gm +
               p14.z_gm*v_p14_p11.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p14_p11, d_p14_p11, v_b1, d_b1, v_b2, d_b2, &p14_p11 );

    if (stat_int == 0)
      {
      mid_che = (p14_p11.x_gm-p14.x_gm)*(p11.x_gm-p14_p11.x_gm) +
                (p14_p11.y_gm-p14.y_gm)*(p11.y_gm-p14_p11.y_gm) +
                (p14_p11.z_gm-p14.z_gm)*(p11.z_gm-p14_p11.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane1 == 0 )
      {
      n_plane1 = n_plane1 + 1;   /* Start line point on boundary */
      p_start1 = p14_p11; 
      }
    else if ( mid_che >= 0 && n_plane1 == 1 )
      {
      n_plane1 = n_plane1 + 1;   /* End   line point on boundary */
      p_end1   = p14_p11; 
      }
    else if ( mid_che >= 0 && n_plane1 == 2 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit n_plane1 > 2  \n");
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
#endif
      sprintf(errbuf,"(n_plane1 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* The number of boundary intersects must 0 or 2.                   */
/*                                                                 !*/

#ifdef  DEBUG
   if ( n_plane1 == 1  )  /* Programming check  */
     {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit n_plane1 = 1  \n");
fflush (dbgfil(SURPAC)); /* From buffer to file */
}
     sprintf(errbuf,"n_plane1=1%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 ) fprintf(dbgfil(SURPAC), 
"sur674 n_plane1 %d in_lin_no %d\n",n_plane1, in_lin_no );
#endif

   if ( n_plane1 != 2  && (lin_no == 12 || lin_no == 1) )
     {
     if   ( in_lin_no < 0 ) goto no_error; 
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Error exit n_plane1 %d is not equal to two (2)\n",n_plane1 );
#endif
     sprintf(errbuf,"1 %%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2303",errbuf));
     }





/*!                                                                 */
/* 5. Calculate intersects between plane 2 boundaries and plane 1   */
/*                                                                 !*/

/*!                                                                 */
/* Intersect p21_p22: Plane p21-p22 and v_b2, plane 1 and plane 2   */
/*                                                                 !*/

   v_p21_p22.x_gm = (p21.y_gm-p22.y_gm) * v_b2.z_gm - 
                    (p21.z_gm-p22.z_gm) * v_b2.y_gm;  
   v_p21_p22.y_gm = (p21.z_gm-p22.z_gm) * v_b2.x_gm - 
                    (p21.x_gm-p22.x_gm) * v_b2.z_gm;  
   v_p21_p22.z_gm = (p21.x_gm-p22.x_gm) * v_b2.y_gm - 
                    (p21.y_gm-p22.y_gm) * v_b2.x_gm;  

   length    =  SQRT( v_p21_p22.x_gm*v_p21_p22.x_gm + 
                      v_p21_p22.y_gm*v_p21_p22.y_gm + 
                      v_p21_p22.z_gm*v_p21_p22.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_p21_p22 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p21_p22.x_gm = v_p21_p22.x_gm / length;
   v_p21_p22.y_gm = v_p21_p22.y_gm / length;
   v_p21_p22.z_gm = v_p21_p22.z_gm / length;

   d_p21_p22 = p21.x_gm*v_p21_p22.x_gm +
               p21.y_gm*v_p21_p22.y_gm +
               p21.z_gm*v_p21_p22.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p21_p22, d_p21_p22, v_b1, d_b1, v_b2, d_b2, &p21_p22 );

    if (stat_int == 0)
      {
      mid_che = (p21_p22.x_gm-p21.x_gm)*(p22.x_gm-p21_p22.x_gm) +
                (p21_p22.y_gm-p21.y_gm)*(p22.y_gm-p21_p22.y_gm) +
                (p21_p22.z_gm-p21.z_gm)*(p22.z_gm-p21_p22.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if ( mid_che >= 0 )
      {
      n_plane2 = n_plane2 + 1;   /* Intersect point on boundary */
      p_start2 = p21_p22; 
      }

/*!                                                                 */
/* Intersect p22_p23: Plane p22-p23 and v_b2, plane 1 and plane 2   */
/*                                                                 !*/

   length    =  SQRT( (p22.x_gm-p23.x_gm)*(p22.x_gm-p23.x_gm) + 
                      (p22.y_gm-p23.y_gm)*(p22.y_gm-p23.y_gm) + 
                      (p22.z_gm-p23.z_gm)*(p22.z_gm-p23.z_gm) );  

  if ( length < 0.001 ) goto no_p22_p23;  /* Borde vara ctol */

   v_p22_p23.x_gm = (p22.y_gm-p23.y_gm) * v_b2.z_gm - 
                    (p22.z_gm-p23.z_gm) * v_b2.y_gm;  
   v_p22_p23.y_gm = (p22.z_gm-p23.z_gm) * v_b2.x_gm - 
                    (p22.x_gm-p23.x_gm) * v_b2.z_gm;  
   v_p22_p23.z_gm = (p22.x_gm-p23.x_gm) * v_b2.y_gm - 
                    (p22.y_gm-p23.y_gm) * v_b2.x_gm;  

   length    =  SQRT( v_p22_p23.x_gm*v_p22_p23.x_gm + 
                      v_p22_p23.y_gm*v_p22_p23.y_gm + 
                      v_p22_p23.z_gm*v_p22_p23.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_p22_p23 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p22_p23.x_gm = v_p22_p23.x_gm / length;
   v_p22_p23.y_gm = v_p22_p23.y_gm / length;
   v_p22_p23.z_gm = v_p22_p23.z_gm / length;

   d_p22_p23 = p22.x_gm*v_p22_p23.x_gm +
               p22.y_gm*v_p22_p23.y_gm +
               p22.z_gm*v_p22_p23.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p22_p23, d_p22_p23, v_b1, d_b1, v_b2, d_b2, &p22_p23 );

    if (stat_int == 0)
      {
      mid_che = (p22_p23.x_gm-p22.x_gm)*(p23.x_gm-p22_p23.x_gm) +
                (p22_p23.y_gm-p22.y_gm)*(p23.y_gm-p22_p23.y_gm) +
                (p22_p23.z_gm-p22.z_gm)*(p23.z_gm-p22_p23.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane2 == 0 )
      {
      n_plane2 = n_plane2 + 1;   /* Start line point on boundary */
      p_start2 = p22_p23; 
      }
    else if ( mid_che >= 0 && n_plane2 == 1 )
      {
      n_plane2 = n_plane2 + 1;   /* End   line point on boundary */
      p_end2   = p22_p23; 
      }
    else if ( mid_che >= 0 && n_plane2 == 2 )
      {
      sprintf(errbuf,"(n_plane2 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }

no_p22_p23:;  /* No line p22-p23 */


/*!                                                                 */
/* Intersect p23_p24: Plane p23-p24 and v_b2, plane 1 and plane 2   */
/*                                                                 !*/

   v_p23_p24.x_gm = (p23.y_gm-p24.y_gm) * v_b2.z_gm - 
                    (p23.z_gm-p24.z_gm) * v_b2.y_gm;  
   v_p23_p24.y_gm = (p23.z_gm-p24.z_gm) * v_b2.x_gm - 
                    (p23.x_gm-p24.x_gm) * v_b2.z_gm;  
   v_p23_p24.z_gm = (p23.x_gm-p24.x_gm) * v_b2.y_gm - 
                    (p23.y_gm-p24.y_gm) * v_b2.x_gm;  

   length    =  SQRT( v_p23_p24.x_gm*v_p23_p24.x_gm + 
                      v_p23_p24.y_gm*v_p23_p24.y_gm + 
                      v_p23_p24.z_gm*v_p23_p24.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_p23_p24 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p23_p24.x_gm = v_p23_p24.x_gm / length;
   v_p23_p24.y_gm = v_p23_p24.y_gm / length;
   v_p23_p24.z_gm = v_p23_p24.z_gm / length;

   d_p23_p24 = p23.x_gm*v_p23_p24.x_gm +
               p23.y_gm*v_p23_p24.y_gm +
               p23.z_gm*v_p23_p24.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p23_p24, d_p23_p24, v_b1, d_b1, v_b2, d_b2, &p23_p24 );

    if (stat_int == 0)
      {
      mid_che = (p23_p24.x_gm-p23.x_gm)*(p24.x_gm-p23_p24.x_gm) +
                (p23_p24.y_gm-p23.y_gm)*(p24.y_gm-p23_p24.y_gm) +
                (p23_p24.z_gm-p23.z_gm)*(p24.z_gm-p23_p24.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane2 == 0 )
      {
      n_plane2 = n_plane2 + 1;   /* Start line point on boundary */
      p_start2 = p23_p24; 
      }
    else if ( mid_che >= 0 && n_plane2 == 1 )
      {
      n_plane2 = n_plane2 + 1;   /* End   line point on boundary */
      p_end2   = p23_p24; 
      }
    else if ( mid_che >= 0 && n_plane2 == 2 )
      {
      sprintf(errbuf,"(n_plane2 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }


/*!                                                                 */
/* Intersect p24_p21: Plane p24-p21 and v_b2, plane 1 and plane 2   */
/*                                                                 !*/

   v_p24_p21.x_gm = (p24.y_gm-p21.y_gm) * v_b2.z_gm - 
                    (p24.z_gm-p21.z_gm) * v_b2.y_gm;  
   v_p24_p21.y_gm = (p24.z_gm-p21.z_gm) * v_b2.x_gm - 
                    (p24.x_gm-p21.x_gm) * v_b2.z_gm;  
   v_p24_p21.z_gm = (p24.x_gm-p21.x_gm) * v_b2.y_gm - 
                    (p24.y_gm-p21.y_gm) * v_b2.x_gm;  

   length    =  SQRT( v_p24_p21.x_gm*v_p24_p21.x_gm + 
                      v_p24_p21.y_gm*v_p24_p21.y_gm + 
                      v_p24_p21.z_gm*v_p24_p21.z_gm );  

/* Ska kontrolleras bara vid DEBUG om kontroll av B-planes ??? */
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(v_p24_p21 zero)%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_p24_p21.x_gm = v_p24_p21.x_gm / length;
   v_p24_p21.y_gm = v_p24_p21.y_gm / length;
   v_p24_p21.z_gm = v_p24_p21.z_gm / length;

   d_p24_p21 = p24.x_gm*v_p24_p21.x_gm +
               p24.y_gm*v_p24_p21.y_gm +
               p24.z_gm*v_p24_p21.z_gm; 

    stat_int = varkon_poi_3plane 
    ( v_p24_p21, d_p24_p21, v_b1, d_b1, v_b2, d_b2, &p24_p21 );

    if (stat_int == 0)
      {
      mid_che = (p24_p21.x_gm-p24.x_gm)*(p21.x_gm-p24_p21.x_gm) +
                (p24_p21.y_gm-p24.y_gm)*(p21.y_gm-p24_p21.y_gm) +
                (p24_p21.z_gm-p24.z_gm)*(p21.z_gm-p24_p21.z_gm); 
      }
    else
      {
      mid_che = -1.0;  
      }

    if      ( mid_che >= 0 && n_plane2 == 0 )
      {
      n_plane2 = n_plane2 + 1;   /* Start line point on boundary */
      p_start2 = p24_p21; 
      }
    else if ( mid_che >= 0 && n_plane2 == 1 )
      {
      n_plane2 = n_plane2 + 1;   /* End   line point on boundary */
      p_end2   = p24_p21; 
      }
    else if ( mid_che >= 0 && n_plane2 == 2 )
      {
      sprintf(errbuf,"(n_plane2 > 2)%%varkon_lin_2plane (sur674)");
      return(varkon_erpush("SU2993",errbuf));
      }


/*!                                                                 */
/* The number of boundary intersects must 0 or 2. Return if 0       */
/*                                                                 !*/

#ifdef  DEBUG
   if ( n_plane2 == 1  )  /* Programming check  */
     {
     sprintf(errbuf,"n_plane2=1%%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 ) fprintf(dbgfil(SURPAC), 
"sur674 n_plane2 %d in_lin_no %d\n",n_plane2, in_lin_no );
#endif


   if ( n_plane2 != 2  && ( lin_no == 12  || lin_no == 2 ) )
     {
     if   ( in_lin_no < 0 ) goto no_error; 
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Error exit n_plane2 %d is not equal to two (2)\n",n_plane2 );
#endif
     sprintf(errbuf,"2 %%varkon_lin_2plane (sur674)");
     return(varkon_erpush("SU2303",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
"sur674 Intersects in both polygons n_plane1 %d n_plane2 %d in_lin_no %d\n",
n_plane1, n_plane2, in_lin_no );
#endif


  if     ( lin_no == 12 )
    {
    ;/* Continue in program */
    }
  else if ( lin_no == 1 )
    {
    p_ps_out->x_gm  = p_start1.x_gm;
    p_ps_out->y_gm  = p_start1.y_gm;
    p_ps_out->z_gm  = p_start1.z_gm;
    p_pe_out->x_gm  = p_end1.x_gm;
    p_pe_out->y_gm  = p_end1.y_gm;
    p_pe_out->z_gm  = p_end1.z_gm;
    *p_nlin = 1;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Intersect in polygon 1 only requested (in_lin_no %d)\n",in_lin_no );
#endif
    goto one_poly;
    }
  else if ( lin_no == 2 )
    {
    p_ps_out->x_gm  = p_start2.x_gm;
    p_ps_out->y_gm  = p_start2.y_gm;
    p_ps_out->z_gm  = p_start2.z_gm;
    p_pe_out->x_gm  = p_end2.x_gm;
    p_pe_out->y_gm  = p_end2.y_gm;
    p_pe_out->z_gm  = p_end2.z_gm;
    *p_nlin = 1;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Intersect in polygon 2 only requested (in_lin_no %d)\n",in_lin_no );
#endif
    goto one_poly;
    }


  mids_che = (p_start2.x_gm-p_start1.x_gm)*(p_end1.x_gm-p_start2.x_gm) +
             (p_start2.y_gm-p_start1.y_gm)*(p_end1.y_gm-p_start2.y_gm) +
             (p_start2.z_gm-p_start1.z_gm)*(p_end1.z_gm-p_start2.z_gm); 

  mide_che = (p_end2.x_gm-p_start1.x_gm)*(p_end1.x_gm-p_end2.x_gm) +
             (p_end2.y_gm-p_start1.y_gm)*(p_end1.y_gm-p_end2.y_gm) +
             (p_end2.z_gm-p_start1.z_gm)*(p_end1.z_gm-p_end2.z_gm); 

#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Scalar products mids_che %f mide_che %f)\n",mids_che, mide_che );
#endif

  if      (mids_che>=0 &&  mide_che>=0 ) /* s1----s2-----e2---e1          */
    {
    p_ps_out->x_gm  = p_start2.x_gm;
    p_ps_out->y_gm  = p_start2.y_gm;
    p_ps_out->z_gm  = p_start2.z_gm;
    p_pe_out->x_gm  = p_end2.x_gm;
    p_pe_out->y_gm  = p_end2.y_gm;
    p_pe_out->z_gm  = p_end2.z_gm;
    *p_nlin = 1;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case     s1----s2----e2---e1     \n");
#endif
    }
  else if (mids_che>=0 &&  mide_che< 0 ) /* *-------s------*  e           */
    {
    mid_che = 
    (p_start1.x_gm-p_start2.x_gm)*
    (p_end2.x_gm-p_start1.x_gm) +
    (p_start1.y_gm-p_start2.y_gm)*
    (p_end2.y_gm-p_start1.y_gm) +
    (p_start1.z_gm-p_start2.z_gm)*
    (p_end2.z_gm-p_start1.z_gm); 

    if  ( mid_che >= 0 )                 /* e----s---------s  e           */
      {
      p_ps_out->x_gm  = p_start1.x_gm;
      p_ps_out->y_gm  = p_start1.y_gm;
      p_ps_out->z_gm  = p_start1.z_gm;
      p_pe_out->x_gm  = p_start2.x_gm;
      p_pe_out->y_gm  = p_start2.y_gm;
      p_pe_out->z_gm  = p_start2.z_gm;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case     e----s--------s  e  \n");
#endif
      }
    else                                /*  s-------s------e  e           */
      {
      p_ps_out->x_gm  = p_end1.x_gm;
      p_ps_out->y_gm  = p_end1.y_gm;
      p_ps_out->z_gm  = p_end1.z_gm;
      p_pe_out->x_gm  = p_start2.x_gm;
      p_pe_out->y_gm  = p_start2.y_gm;
      p_pe_out->z_gm  = p_start2.z_gm;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case     s----s--------e  e  \n");
#endif
      }
    *p_nlin = 1;
    }

  else if (mids_che< 0 &&  mide_che>=0 ) /* *-------e------*  s           */
    {
    mid_che = 
    (p_start1.x_gm-p_end2.x_gm)* 
    (p_start2.x_gm-p_start1.x_gm) +
    (p_start1.y_gm-p_end2.y_gm)* 
    (p_start2.y_gm-p_start1.y_gm) +
    (p_start1.z_gm-p_end2.z_gm)* 
    (p_start2.z_gm-p_start1.z_gm); 

    if  ( mid_che >= 0 )                 /* e-------e------s  s           */
      {
      p_ps_out->x_gm  = p_end2.x_gm;
      p_ps_out->y_gm  = p_end2.y_gm;
      p_ps_out->z_gm  = p_end2.z_gm;
      p_pe_out->x_gm  = p_start1.x_gm;
      p_pe_out->y_gm  = p_start1.y_gm;
      p_pe_out->z_gm  = p_start1.z_gm;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case     e----e--------s  s  \n");
#endif
      }
    else                                /*  s-------e------e  s           */
      {
      p_ps_out->x_gm  = p_end1.x_gm;
      p_ps_out->y_gm  = p_end1.y_gm;
      p_ps_out->z_gm  = p_end1.z_gm;
      p_pe_out->x_gm  = p_end2.x_gm;
      p_pe_out->y_gm  = p_end2.y_gm;
      p_pe_out->z_gm  = p_end2.z_gm;
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case     e----e--------e  s  \n");
#endif
      }
    *p_nlin = 1;
    }

  else if (mids_che< 0 &&  mide_che< 0 ) /*  s2 e2 s1----------e1  s2  e2  */
    {
    /* Shift of polygon 1 and polygon 2 points */
    p_temp   = p_start1; p_start1 = p_start2; p_start2 = p_temp;
    p_temp   = p_end1;   p_end1   = p_end2;   p_end2   = p_temp;
    mids_che = (p_start2.x_gm-p_start1.x_gm)*(p_end1.x_gm-p_start2.x_gm) +
               (p_start2.y_gm-p_start1.y_gm)*(p_end1.y_gm-p_start2.y_gm) +
               (p_start2.z_gm-p_start1.z_gm)*(p_end1.z_gm-p_start2.z_gm); 

    mide_che = (p_end2.x_gm-p_start1.x_gm)*(p_end1.x_gm-p_end2.x_gm) +
               (p_end2.y_gm-p_start1.y_gm)*(p_end1.y_gm-p_end2.y_gm) +
               (p_end2.z_gm-p_start1.z_gm)*(p_end1.z_gm-p_end2.z_gm); 

#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Cases  s2 e2 s1-------------e1 s2 e2 Points 1 <--> 2 shifted\n");
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Scalar products mids_che %f mide_che %f)\n",mids_che, mide_che );
#endif

    if      (mids_che>=0 &&  mide_che>=0 ) /* s1----s2-----e2---e1          */
      {
      p_ps_out->x_gm  = p_start2.x_gm;
      p_ps_out->y_gm  = p_start2.y_gm;
      p_ps_out->z_gm  = p_start2.z_gm;
      p_pe_out->x_gm  = p_end2.x_gm;
      p_pe_out->y_gm  = p_end2.y_gm;
      p_pe_out->z_gm  = p_end2.z_gm;
      *p_nlin = 1;
#ifdef DEBUG
       if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
       "sur674 Case     s1----s2----e2---e1     \n");
#endif
      }
    else
      {
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 ) fprintf(dbgfil(SURPAC), 
     "sur674 Case    s1------------e1  s2  e2  (Exit error)\n");
#endif
    if   ( in_lin_no < 0 ) goto no_error; 
    sprintf(errbuf," %%varkon_lin_2plane (sur674)");
    return(varkon_erpush("SU2293",errbuf));
       }   /* Shifted points */
     }     /* Negative scalar products */
 else
  {
  sprintf(errbuf,"mids_che/mide_che %%varkon_lin_2plane (sur674)");
  return(varkon_erpush("SU2993",errbuf));
  }

one_poly:;       /* Case lin_no = 1 or lin_no = 2 */
no_error:;       /* No error. Determine only if intersect exists */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur674 p_ps_out %f %f %f\n",p_ps_out->x_gm,p_ps_out->y_gm,p_ps_out->z_gm);
fprintf(dbgfil(SURPAC),
"sur674 p_pe_out %f %f %f\n",p_pe_out->x_gm,p_pe_out->y_gm,p_pe_out->z_gm);
}

if ( dbglev(SURPAC) == 1 )
{
if  ( *p_nlin == 0 )
{
fprintf(dbgfil(SURPAC), 
"sur674 Exit*varkon_lin_2plane No intersect line! (in_lin_no %d)\n", in_lin_no);
}
else
{
fprintf(dbgfil(SURPAC), 
"sur674 Exit*varkon_lin_2plane Intersect exists ! (in_lin_no %d)\n", in_lin_no);
}
fflush (dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
