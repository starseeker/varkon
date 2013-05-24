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
/*  Function: varkon_bpl_extend                    File: sur719.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function extends a B-plane, i.e moves the edges of the      */
/*  B-plane a given (parallell) distance. The function calculates   */
/*  new corner points and output is new B-plane. Header data is     */
/*  copied from the input B-plane.                                  */
/*                                                                  */
/*  The input B-plane will be modified if the input and output      */
/*  pointers to the B-planes are equal.                             */
/*                                                                  */
/*  This function is used in intersect calculations and the extend  */
/*  distance is typically the idpoint (identical point) criterion.  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-27   Originally written                                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_bpl_extend     Extend a B-plane                 */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_bpl_analyse     * Analyse (classify) B-plane              */
/* varkon_bpl_extend3     * Extend a 3-point B-plane                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_bpl_extend          Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_bpl_extend (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBBplane *p_bplin,     /* Input  B-plane                    (ptr) */
  DBfloat   dist,        /* Extend distance                         */
  DBBplane *p_bplout )   /* Output B-plane                    (ptr) */

/* Out:                                                             */
/*        B-plane in p_bplout                                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector p1;           /* B-plane corner point 1                  */
  DBVector p2;           /* B-plane corner point 2                  */
  DBVector p3;           /* B-plane corner point 3                  */
  DBVector p4;           /* B-plane corner point 4                  */
  DBint    btype;        /* Type of B-plane polygon                 */
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
  DBint    pnr;          /* Point number for:                       */
                         /* Non-convex point   (p_btype= 3)         */
                         /* Colinear mid point (p_btype= 4,5,6,7)   */
  DBVector v1;           /* Normalized edge vector v1= p2 - p1      */
  DBVector v2;           /* Normalized edge vector v2= p3 - p2      */
  DBVector v3;           /* Normalized edge vector v3= p4 - p3      */
  DBVector v4;           /* Normalized edge vector v4= p1 - p4      */
  DBfloat  sign1;        /* Sign for translation distance. Point 1  */
  DBfloat  sign2;        /* Sign for translation distance. Point 2  */
  DBfloat  sign3;        /* Sign for translation distance. Point 3  */
  DBfloat  sign4;        /* Sign for translation distance. Point 4  */
  DBVector vdist1;       /* Translation direction for point 1       */
  DBVector vdist2;       /* Translation direction for point 2       */
  DBVector vdist3;       /* Translation direction for point 3       */
  DBVector vdist4;       /* Translation direction for point 4       */
  DBfloat  dot1;         /* Scalar product for dist1 calculation    */
  DBfloat  dot2;         /* Scalar product for dist2 calculation    */
  DBfloat  dot3;         /* Scalar product for dist3 calculation    */
  DBfloat  dot4;         /* Scalar product for dist4 calculation    */
  DBfloat  length1;      /* Length         for dist1 calculation    */
  DBfloat  length2;      /* Length         for dist2 calculation    */
  DBfloat  length3;      /* Length         for dist3 calculation    */
  DBfloat  length4;      /* Length         for dist4 calculation    */
  DBfloat  sin_alfa1;    /* Sinus   for point 1 distance calulation */
  DBfloat  sin_alfa2;    /* Sinus   for point 2 distance calulation */
  DBfloat  sin_alfa3;    /* Sinus   for point 3 distance calulation */
  DBfloat  sin_alfa4;    /* Sinus   for point 4 distance calulation */
  DBfloat  cos_alfa1;    /* Cosinus for point 1 distance calulation */
  DBfloat  cos_alfa2;    /* Cosinus for point 2 distance calulation */
  DBfloat  cos_alfa3;    /* Cosinus for point 3 distance calulation */
  DBfloat  cos_alfa4;    /* Cosinus for point 4 distance calulation */
  DBfloat  dist1;        /* Translation distance for point 1        */
  DBfloat  dist2;        /* Translation distance for point 2        */
  DBfloat  dist3;        /* Translation distance for point 3        */
  DBfloat  dist4;        /* Translation distance for point 4        */
  DBfloat  l_t;          /* Total length for bcase= 4,5,6,7         */
  DBfloat  l_p;          /* Part  length for bcase= 4,5,6,7         */
  DBVector p1_e;         /* B-plane corner point 1 extrapolated     */
  DBVector p2_e;         /* B-plane corner point 2 extrapolated     */
  DBVector p3_e;         /* B-plane corner point 3 extrapolated     */
  DBVector p4_e;         /* B-plane corner point 4 extrapolated     */

  DBint    status;       /* Error code from a called function       */

  DBfloat  length;       /* Length of vectors                       */
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
"sur719 Enter varkon_bpl_extend: Extend a B-plane with distance %f\n",
                  dist );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Copy input B-plane data header to output B-plane p_bplout header */
/* Copy also the DB pointer to the coordinate system                */
/*                                                                 !*/
    
   p_bplout->hed_bp   =   p_bplin->hed_bp;     /* Header            */
   p_bplout->pcsy_bp  =   p_bplin->pcsy_bp;    /* DB ptr coord. syst*/

/* B-plane polygon points will be replaced with calculated points ! */
   p_bplout->crd1_bp  =   p_bplin->crd1_bp;    /* Corner point 1    */
   p_bplout->crd2_bp  =   p_bplin->crd2_bp;    /* Corner point 2    */
   p_bplout->crd3_bp  =   p_bplin->crd3_bp;    /* Corner point 3    */
   p_bplout->crd4_bp  =   p_bplin->crd4_bp;    /* Corner point 4    */

/*!                                                                 */
/* Corner points to local variables p1, p2, p3 and p4               */
/*                                                                 !*/

   p1 = p_bplin->crd1_bp;                /* B-plane corner 1        */
   p2 = p_bplin->crd2_bp;                /* B-plane corner 2        */
   p3 = p_bplin->crd3_bp;                /* B-plane corner 3        */
   p4 = p_bplin->crd4_bp;                /* B-plane corner 4        */

#ifdef DEBUG
   v1.x_gm     =   F_UNDEF;
   v1.y_gm     =   F_UNDEF;
   v1.z_gm     =   F_UNDEF;
   v2.x_gm     =   F_UNDEF;
   v2.y_gm     =   F_UNDEF;
   v2.z_gm     =   F_UNDEF;
   v3.x_gm     =   F_UNDEF;
   v3.y_gm     =   F_UNDEF;
   v3.z_gm     =   F_UNDEF;
   v4.x_gm     =   F_UNDEF;
   v4.y_gm     =   F_UNDEF;
   v4.z_gm     =   F_UNDEF;

   sign1       =   F_UNDEF;
   sign2       =   F_UNDEF;
   sign3       =   F_UNDEF;
   sign4       =   F_UNDEF;

   vdist1.x_gm =   F_UNDEF;
   vdist1.y_gm =   F_UNDEF;
   vdist1.z_gm =   F_UNDEF;
   vdist2.x_gm =   F_UNDEF;
   vdist2.y_gm =   F_UNDEF;
   vdist2.z_gm =   F_UNDEF;
   vdist3.x_gm =   F_UNDEF;
   vdist3.y_gm =   F_UNDEF;
   vdist3.z_gm =   F_UNDEF;
   vdist4.x_gm =   F_UNDEF;
   vdist4.y_gm =   F_UNDEF;
   vdist4.z_gm =   F_UNDEF;

   sin_alfa1   =   F_UNDEF;
   sin_alfa2   =   F_UNDEF;
   sin_alfa3   =   F_UNDEF;
   sin_alfa4   =   F_UNDEF;
   cos_alfa1   =   F_UNDEF;
   cos_alfa2   =   F_UNDEF;
   cos_alfa3   =   F_UNDEF;
   cos_alfa4   =   F_UNDEF;

   p1_e.x_gm   =   F_UNDEF;
   p1_e.y_gm   =   F_UNDEF;
   p1_e.z_gm   =   F_UNDEF;
   p2_e.x_gm   =   F_UNDEF;
   p2_e.y_gm   =   F_UNDEF;
   p2_e.z_gm   =   F_UNDEF;
   p3_e.x_gm   =   F_UNDEF;
   p3_e.y_gm   =   F_UNDEF;
   p3_e.z_gm   =   F_UNDEF;
   p4_e.x_gm   =   F_UNDEF;
   p4_e.y_gm   =   F_UNDEF;
   p4_e.z_gm   =   F_UNDEF;

#endif


/*!                                                                 */
/* 2. Determine type of (classify) input B-plane (polygon)          */
/* _______________________________________________________          */
/*                                                                  */
/* Classify) input polygon. Call of varkon_bpl_analyse (sur718)     */
/* Goto triangle if points p2 and p3 are equal or colinear pts      */
/*                                                                 !*/

   varkon_bpl_analyse ( &p1, &p2, &p3, &p4, &btype, &pnr );
/* No error from this function */
   if ( btype < 0  )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Input B-plane not OK btype= %d \n", btype );
fflush (dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"btype %d %%varkon_bpl_extend (sur719)",btype);
     return(varkon_erpush("SU2993",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur719 Type of B-plane btype= %d Point pnr= %d\n", btype, pnr );
fflush(dbgfil(SURPAC)); 
}
#endif

   if (btype == 1 || btype == 4 || btype == 5 || btype == 6 || btype == 7)
     {
     goto triangle;
     }


/*!                                                                 */
/* 3. Extend a four corner B-plane polygon                          */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/* Edge vectors v1, v2, v3 and v4                                   */
/*                                                                 !*/

   v1.x_gm =  p2.x_gm-p1.x_gm;
   v1.y_gm =  p2.y_gm-p1.y_gm;
   v1.z_gm =  p2.z_gm-p1.z_gm;

   length  =  SQRT(v1.x_gm*v1.x_gm+v1.y_gm*v1.y_gm+v1.z_gm*v1.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v1=0    %%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v1.x_gm =  v1.x_gm/length;
   v1.y_gm =  v1.y_gm/length;
   v1.z_gm =  v1.z_gm/length;

   v2.x_gm =  p3.x_gm-p2.x_gm;
   v2.y_gm =  p3.y_gm-p2.y_gm;
   v2.z_gm =  p3.z_gm-p2.z_gm;

   length  =  SQRT(v2.x_gm*v2.x_gm+v2.y_gm*v2.y_gm+v2.z_gm*v2.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v2=0%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v2.x_gm =  v2.x_gm/length;
   v2.y_gm =  v2.y_gm/length;
   v2.z_gm =  v2.z_gm/length;

   v3.x_gm =  p4.x_gm-p3.x_gm;
   v3.y_gm =  p4.y_gm-p3.y_gm;
   v3.z_gm =  p4.z_gm-p3.z_gm;

   length  =  SQRT(v3.x_gm*v3.x_gm+v3.y_gm*v3.y_gm+v3.z_gm*v3.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v3=0%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v3.x_gm =  v3.x_gm/length;
   v3.y_gm =  v3.y_gm/length;
   v3.z_gm =  v3.z_gm/length;

   v4.x_gm =  p1.x_gm-p4.x_gm;
   v4.y_gm =  p1.y_gm-p4.y_gm;
   v4.z_gm =  p1.z_gm-p4.z_gm;

   length  =  SQRT(v4.x_gm*v4.x_gm+v4.y_gm*v4.y_gm+v4.z_gm*v4.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v4=0%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v4.x_gm =  v4.x_gm/length;
   v4.y_gm =  v4.y_gm/length;
   v4.z_gm =  v4.z_gm/length;

/*!                                                                 */
/* Determine the sign for the translation distance                  */
/*                                                                 !*/

   if     (  btype == 2 )
     {
     sign1  =  1.0;
     sign2  =  1.0;
     sign3  =  1.0;
     sign4  =  1.0;
     }
   else if (  btype == 3 )
     {
     if      ( pnr == 1 )
       {
       sign1  = -1.0;
       sign2  =  1.0;
       sign3  =  1.0;
       sign4  =  1.0;
       }
     else if ( pnr == 2 )
       {
       sign1  =  1.0;
       sign2  = -1.0;
       sign3  =  1.0;
       sign4  =  1.0;
       }
     else if ( pnr == 3 )
       {
       sign1  =  1.0;
       sign2  =  1.0;
       sign3  = -1.0;
       sign4  =  1.0;
       }
     else if ( pnr == 4 )
       {
       sign1  =  1.0;
       sign2  =  1.0;
       sign3  =  1.0;
       sign4  = -1.0;
       }
     }
#ifdef DEBUG
    else
        {
        sprintf(errbuf,"switch btype%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif


/*!                                                                 */
/* Translation direction vectors vdist1, vdist2, vdist3, vdist4     */
/*                                                                 !*/

   vdist1.x_gm =  v1.x_gm-v4.x_gm;
   vdist1.y_gm =  v1.y_gm-v4.y_gm;
   vdist1.z_gm =  v1.z_gm-v4.z_gm;



   length  =  SQRT(vdist1.x_gm*vdist1.x_gm+
                   vdist1.y_gm*vdist1.y_gm+
                   vdist1.z_gm*vdist1.z_gm);

#ifdef DEBUG
/* Angle (length) is checked in varkon_bpl_analyse (sur718)         */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"vdist1%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   vdist1.x_gm =  vdist1.x_gm/length;
   vdist1.y_gm =  vdist1.y_gm/length;
   vdist1.z_gm =  vdist1.z_gm/length;


   vdist2.x_gm =  v2.x_gm-v1.x_gm;
   vdist2.y_gm =  v2.y_gm-v1.y_gm;
   vdist2.z_gm =  v2.z_gm-v1.z_gm;

   length  =  SQRT(vdist2.x_gm*vdist2.x_gm+
                   vdist2.y_gm*vdist2.y_gm+
                   vdist2.z_gm*vdist2.z_gm);

#ifdef DEBUG
/* Angle (length) is checked in varkon_bpl_analyse (sur718)         */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"vdist2%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   vdist2.x_gm =  vdist2.x_gm/length;
   vdist2.y_gm =  vdist2.y_gm/length;
   vdist2.z_gm =  vdist2.z_gm/length;



   vdist3.x_gm =  v3.x_gm-v2.x_gm;
   vdist3.y_gm =  v3.y_gm-v2.y_gm;
   vdist3.z_gm =  v3.z_gm-v2.z_gm;


   length  =  SQRT(vdist3.x_gm*vdist3.x_gm+
                   vdist3.y_gm*vdist3.y_gm+
                   vdist3.z_gm*vdist3.z_gm);

#ifdef DEBUG
/* Angle (length) is checked in varkon_bpl_analyse (sur718)         */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"vdist3%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   vdist3.x_gm =  vdist3.x_gm/length;
   vdist3.y_gm =  vdist3.y_gm/length;
   vdist3.z_gm =  vdist3.z_gm/length;


   vdist4.x_gm =  v4.x_gm-v3.x_gm;
   vdist4.y_gm =  v4.y_gm-v3.y_gm;
   vdist4.z_gm =  v4.z_gm-v3.z_gm;


   length  =  SQRT(vdist4.x_gm*vdist4.x_gm+
                   vdist4.y_gm*vdist4.y_gm+
                   vdist4.z_gm*vdist4.z_gm);

#ifdef DEBUG
/* Angle (length) is checked in varkon_bpl_analyse (sur718)         */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"vdist4%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   vdist4.x_gm =  vdist4.x_gm/length;
   vdist4.y_gm =  vdist4.y_gm/length;
   vdist4.z_gm =  vdist4.z_gm/length;


/*!                                                                 */
/* Translation distances                                            */
/*                                                                 !*/


   length1 =  SQRT((vdist1.x_gm+v1.x_gm)*(vdist1.x_gm+v1.x_gm)+
                   (vdist1.y_gm+v1.y_gm)*(vdist1.y_gm+v1.y_gm)+
                   (vdist1.z_gm+v1.z_gm)*(vdist1.z_gm+v1.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length1 < 0.000001 ) 
        {
        sprintf(errbuf,"length1%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot1 = vdist1.x_gm*v1.x_gm+vdist1.y_gm*v1.y_gm+vdist1.z_gm*v1.z_gm;   

   cos_alfa1 = (1.0 + dot1)/length1;
   sin_alfa1 = SQRT(1.0 - cos_alfa1*cos_alfa1);

   if    ( sin_alfa1 > 0.0001 )
     {
     dist1 = sign1*dist/sin_alfa1;
     }
   else
     {
     sprintf(errbuf,"sin_alfa1=0%%varkon_bpl_extend (sur719)");
     return(varkon_erpush("SU2993",errbuf));
     }


   length2 =  SQRT((vdist2.x_gm+v2.x_gm)*(vdist2.x_gm+v2.x_gm)+
                   (vdist2.y_gm+v2.y_gm)*(vdist2.y_gm+v2.y_gm)+
                   (vdist2.z_gm+v2.z_gm)*(vdist2.z_gm+v2.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length2 < 0.000001 ) 
        {
        sprintf(errbuf,"length2%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot2 = vdist2.x_gm*v2.x_gm+vdist2.y_gm*v2.y_gm+vdist2.z_gm*v2.z_gm;   

   cos_alfa2 = (1.0 + dot2)/length2;
   sin_alfa2 = SQRT(1.0 - cos_alfa2*cos_alfa2);

   if    ( sin_alfa2 > 0.0001 )
     {
     dist2 = sign2*dist/sin_alfa2;
     }
   else
     {
     sprintf(errbuf,"sin_alfa2=0%%varkon_bpl_extend (sur719)");
     return(varkon_erpush("SU2993",errbuf));
     }


   length3 =  SQRT((vdist3.x_gm+v3.x_gm)*(vdist3.x_gm+v3.x_gm)+
                   (vdist3.y_gm+v3.y_gm)*(vdist3.y_gm+v3.y_gm)+
                   (vdist3.z_gm+v3.z_gm)*(vdist3.z_gm+v3.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length3 < 0.000001 ) 
        {
        sprintf(errbuf,"length3%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot3 = vdist3.x_gm*v3.x_gm+vdist3.y_gm*v3.y_gm+vdist3.z_gm*v3.z_gm;   

   cos_alfa3 = (1.0 + dot3)/length3;
   sin_alfa3 = SQRT(1.0 - cos_alfa3*cos_alfa3);

   if    ( sin_alfa3 > 0.0001 )
     {
     dist3 = sign3*dist/sin_alfa3;
     }
   else
     {
     sprintf(errbuf,"sin_alfa3=0%%varkon_bpl_extend (sur719)");
     return(varkon_erpush("SU2993",errbuf));
     }


   length4 =  SQRT((vdist4.x_gm+v4.x_gm)*(vdist4.x_gm+v4.x_gm)+
                   (vdist4.y_gm+v4.y_gm)*(vdist4.y_gm+v4.y_gm)+
                   (vdist4.z_gm+v4.z_gm)*(vdist4.z_gm+v4.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length4 < 0.000001 ) 
        {
        sprintf(errbuf,"length4%%varkon_bpl_extend (sur719)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot4 = vdist4.x_gm*v4.x_gm+vdist4.y_gm*v4.y_gm+vdist4.z_gm*v4.z_gm;   

   cos_alfa4 = (1.0 + dot4)/length4;
   sin_alfa4 = SQRT(1.0 - cos_alfa4*cos_alfa4);

   if    ( sin_alfa4 > 0.0001 )
     {
     dist4 = sign4*dist/sin_alfa4;
     }
   else
     {
     sprintf(errbuf,"sin_alfa4=0%%varkon_bpl_extend (sur719)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* Translated (new) corner points for the extended B-plane          */
/*                                                                 !*/

   p1_e.x_gm        = p1.x_gm - dist1*vdist1.x_gm;
   p1_e.y_gm        = p1.y_gm - dist1*vdist1.y_gm;
   p1_e.z_gm        = p1.z_gm - dist1*vdist1.z_gm;

   p2_e.x_gm        = p2.x_gm - dist2*vdist2.x_gm;
   p2_e.y_gm        = p2.y_gm - dist2*vdist2.y_gm;
   p2_e.z_gm        = p2.z_gm - dist2*vdist2.z_gm;

   p3_e.x_gm        = p3.x_gm - dist3*vdist3.x_gm;
   p3_e.y_gm        = p3.y_gm - dist3*vdist3.y_gm;
   p3_e.z_gm        = p3.z_gm - dist3*vdist3.z_gm;

   p4_e.x_gm        = p4.x_gm - dist4*vdist4.x_gm;
   p4_e.y_gm        = p4.y_gm - dist4*vdist4.y_gm;
   p4_e.z_gm        = p4.z_gm - dist4*vdist4.z_gm;

   goto  cre_plane;











/*!                                                                 */
/* 4. Extend a three corner B-plane polygon                         */
/* ________________________________________                         */
/*                                                                 !*/

triangle:;  /*! Label: Input polygon is a triangle                 !*/


  if        ( btype == 1 )       
    {
   status = varkon_bpl_extend3
   (  &p1,  &p2,  &p4, dist,  &p1_e,  &p2_e,  &p4_e );

   p3_e.x_gm = p2_e.x_gm;
   p3_e.y_gm = p2_e.y_gm;
   p3_e.z_gm = p2_e.z_gm;

   }   /* End btype == 1 */

  else if   ( btype == 4 )       
    {
   status = varkon_bpl_extend3
   (  &p2,  &p3,  &p4, dist,  &p2_e,  &p3_e,  &p4_e );

   l_t = SQRT( (p4.x_gm-p2.x_gm)*(p4.x_gm-p2.x_gm)+
               (p4.y_gm-p2.y_gm)*(p4.y_gm-p2.y_gm)+
               (p4.z_gm-p2.z_gm)*(p4.z_gm-p2.z_gm));
   l_p = SQRT( (p1.x_gm-p4.x_gm)*(p1.x_gm-p4.x_gm)+
               (p1.y_gm-p4.y_gm)*(p1.y_gm-p4.y_gm)+
               (p1.z_gm-p4.z_gm)*(p1.z_gm-p4.z_gm));

   p1_e.x_gm = p4_e.x_gm + l_p/l_t * (p2_e.x_gm - p4_e.x_gm);
   p1_e.y_gm = p4_e.y_gm + l_p/l_t * (p2_e.y_gm - p4_e.y_gm);
   p1_e.z_gm = p4_e.z_gm + l_p/l_t * (p2_e.z_gm - p4_e.z_gm);
   
   }   /* End btype == 4 */

  else if   ( btype == 5 )       
    {
   status = varkon_bpl_extend3
   (  &p3,  &p4,  &p1, dist,  &p3_e,  &p4_e,  &p1_e );

   l_t = SQRT( (p3.x_gm-p1.x_gm)*(p3.x_gm-p1.x_gm)+
               (p3.y_gm-p1.y_gm)*(p3.y_gm-p1.y_gm)+
               (p3.z_gm-p1.z_gm)*(p3.z_gm-p1.z_gm));
   l_p = SQRT( (p2.x_gm-p1.x_gm)*(p2.x_gm-p1.x_gm)+
               (p2.y_gm-p1.y_gm)*(p2.y_gm-p1.y_gm)+
               (p2.z_gm-p1.z_gm)*(p2.z_gm-p1.z_gm));

   p2_e.x_gm = p1_e.x_gm + l_p/l_t * (p3_e.x_gm - p1_e.x_gm);
   p2_e.y_gm = p1_e.y_gm + l_p/l_t * (p3_e.y_gm - p1_e.y_gm);
   p2_e.z_gm = p1_e.z_gm + l_p/l_t * (p3_e.z_gm - p1_e.z_gm);
   
   }   /* End btype == 5 */


  else if   ( btype == 6 )       
    {
   status = varkon_bpl_extend3
   (  &p4,  &p1,  &p2, dist,  &p4_e,  &p1_e,  &p2_e );

   l_t = SQRT( (p4.x_gm-p2.x_gm)*(p4.x_gm-p2.x_gm)+
               (p4.y_gm-p2.y_gm)*(p4.y_gm-p2.y_gm)+
               (p4.z_gm-p2.z_gm)*(p4.z_gm-p2.z_gm));
   l_p = SQRT( (p3.x_gm-p2.x_gm)*(p3.x_gm-p2.x_gm)+
               (p3.y_gm-p2.y_gm)*(p3.y_gm-p2.y_gm)+
               (p3.z_gm-p2.z_gm)*(p3.z_gm-p2.z_gm));

   p3_e.x_gm = p2_e.x_gm + l_p/l_t * (p4_e.x_gm - p2_e.x_gm);
   p3_e.y_gm = p2_e.y_gm + l_p/l_t * (p4_e.y_gm - p2_e.y_gm);
   p3_e.z_gm = p2_e.z_gm + l_p/l_t * (p4_e.z_gm - p2_e.z_gm);
   
   }   /* End btype == 6 */

  else if   ( btype == 7 )       
    {
   status = varkon_bpl_extend3
   (  &p1,  &p2,  &p3, dist,  &p1_e,  &p2_e,  &p3_e );

   l_t = SQRT( (p1.x_gm-p3.x_gm)*(p1.x_gm-p3.x_gm)+
               (p1.y_gm-p3.y_gm)*(p1.y_gm-p3.y_gm)+
               (p1.z_gm-p3.z_gm)*(p1.z_gm-p3.z_gm));
   l_p = SQRT( (p4.x_gm-p3.x_gm)*(p4.x_gm-p3.x_gm)+
               (p4.y_gm-p3.y_gm)*(p4.y_gm-p3.y_gm)+
               (p4.z_gm-p3.z_gm)*(p4.z_gm-p3.z_gm));

   p4_e.x_gm = p3_e.x_gm + l_p/l_t * (p1_e.x_gm - p3_e.x_gm);
   p4_e.y_gm = p3_e.y_gm + l_p/l_t * (p1_e.y_gm - p3_e.y_gm);
   p4_e.z_gm = p3_e.z_gm + l_p/l_t * (p1_e.z_gm - p3_e.z_gm);
   
   }   /* End btype == 7 */



cre_plane:; /* Label: Create output B-plane                         */

   p_bplout->crd1_bp.x_gm = p1_e.x_gm;
   p_bplout->crd1_bp.y_gm = p1_e.y_gm;
   p_bplout->crd1_bp.z_gm = p1_e.z_gm;

   p_bplout->crd2_bp.x_gm = p2_e.x_gm;
   p_bplout->crd2_bp.y_gm = p2_e.y_gm;
   p_bplout->crd2_bp.z_gm = p2_e.z_gm;

   p_bplout->crd3_bp.x_gm = p3_e.x_gm;
   p_bplout->crd3_bp.y_gm = p3_e.y_gm;
   p_bplout->crd3_bp.z_gm = p3_e.z_gm;

   p_bplout->crd4_bp.x_gm = p4_e.x_gm;
   p_bplout->crd4_bp.y_gm = p4_e.y_gm;
   p_bplout->crd4_bp.z_gm = p4_e.z_gm;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur719 Exit varkon_bpl_extend Extend B-plane with distance %f \n", 
             dist );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
