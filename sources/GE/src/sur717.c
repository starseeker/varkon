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
/*  Function: varkon_bpl_extend3                   File: sur717.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function extends a three point B-plane polygon, i.e moves   */
/*  the edges of the B-plane a given (parallell) distance.          */
/*                                                                  */
/*  This function is used in intersect calculations and the extend  */
/*  distance is typically the idpoint (identical point) criterion.  */
/*                                                                  */
/*  Input pointers may be equal to output pointers                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-27   Originally written                                 */
/*  1999-11-28   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_bpl_extend3    Extend a 3-point B-plane polygon */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_bpl_extend3         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_bpl_extend3 (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_p1,        /* B-plane corner point 1            (ptr) */
  DBVector *p_p2,        /* B-plane corner point 2  =         (ptr) */
                         /* B-plane corner point 3                  */
  DBVector *p_p4,        /* B-plane corner point 4            (ptr) */
  DBfloat   dist,        /* Extend distance                         */
  DBVector *p_p1e,       /* B-plane corner point 1 extended   (ptr) */
  DBVector *p_p2e,       /* B-plane corner point 2 extended   (ptr) */
  DBVector *p_p4e )      /* B-plane corner point 4 extended   (ptr) */

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
  DBVector p4;           /* B-plane corner point 4                  */

  DBVector v1;           /* Normalized edge vector v1= p2 - p1      */
  DBVector v2;           /* Normalized edge vector v2= p4 - p2      */
  DBVector v4;           /* Normalized edge vector v4= p1 - p4      */

  DBVector vdist1;       /* Translation direction for point 1       */
  DBVector vdist2;       /* Translation direction for point 2       */
  DBVector vdist4;       /* Translation direction for point 4       */
  DBfloat  dot1;         /* Scalar product for dist1 calculation    */
  DBfloat  dot2;         /* Scalar product for dist2 calculation    */
  DBfloat  dot4;         /* Scalar product for dist4 calculation    */
  DBfloat  length1;      /* Length         for dist1 calculation    */
  DBfloat  length2;      /* Length         for dist2 calculation    */
  DBfloat  length4;      /* Length         for dist4 calculation    */
  DBfloat  sin_alfa1;    /* Sinus   for point 1 distance calulation */
  DBfloat  sin_alfa2;    /* Sinus   for point 2 distance calulation */
  DBfloat  sin_alfa4;    /* Sinus   for point 4 distance calulation */
  DBfloat  cos_alfa1;    /* Cosinus for point 1 distance calulation */
  DBfloat  cos_alfa2;    /* Cosinus for point 2 distance calulation */
  DBfloat  cos_alfa4;    /* Cosinus for point 4 distance calulation */
  DBfloat  dist1;        /* Translation distance for point 1        */
  DBfloat  dist2;        /* Translation distance for point 2        */
  DBfloat  dist4;        /* Translation distance for point 4        */

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
"sur717 Enter varkon_bpl_extend3 Extend a 3-point B-plane polygon with distance %f\n",
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
/* Corner points to local variables p1, p2 and p4                   */
/*                                                                 !*/

   p1.x_gm = p_p1->x_gm;                 /* B-plane corner 1        */
   p1.y_gm = p_p1->y_gm;  
   p1.z_gm = p_p1->z_gm; 

   p2.x_gm = p_p2->x_gm;                 /* B-plane corner 2        */
   p2.y_gm = p_p2->y_gm;  
   p2.z_gm = p_p2->z_gm; 

   p4.x_gm = p_p4->x_gm;                 /* B-plane corner 4        */
   p4.y_gm = p_p4->y_gm;  
   p4.z_gm = p_p4->z_gm; 


#ifdef DEBUG
   v1.x_gm     =   F_UNDEF;
   v1.y_gm     =   F_UNDEF;
   v1.z_gm     =   F_UNDEF;
   v2.x_gm     =   F_UNDEF;
   v2.y_gm     =   F_UNDEF;
   v2.z_gm     =   F_UNDEF;
   v4.x_gm     =   F_UNDEF;
   v4.y_gm     =   F_UNDEF;
   v4.z_gm     =   F_UNDEF;

   vdist1.x_gm =   F_UNDEF;
   vdist1.y_gm =   F_UNDEF;
   vdist1.z_gm =   F_UNDEF;
   vdist2.x_gm =   F_UNDEF;
   vdist2.y_gm =   F_UNDEF;
   vdist2.z_gm =   F_UNDEF;
   vdist4.x_gm =   F_UNDEF;
   vdist4.y_gm =   F_UNDEF;
   vdist4.z_gm =   F_UNDEF;

   sin_alfa1   =   F_UNDEF;
   sin_alfa2   =   F_UNDEF;
   sin_alfa4   =   F_UNDEF;
   cos_alfa1   =   F_UNDEF;
   cos_alfa2   =   F_UNDEF;
   cos_alfa4   =   F_UNDEF;

   /* Do not initialize p_p1e, p_p2e, p_p3e and p_p4e !!!!!         */

#endif

/*!                                                                 */
/* 2. Extend a three corner B-plane polygon                         */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/* Edge vectors v1, v2 and v4                                       */
/*                                                                 !*/

   v1.x_gm =  p2.x_gm-p1.x_gm;
   v1.y_gm =  p2.y_gm-p1.y_gm;
   v1.z_gm =  p2.z_gm-p1.z_gm;

   length  =  SQRT(v1.x_gm*v1.x_gm+v1.y_gm*v1.y_gm+v1.z_gm*v1.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v1=0    %%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v1.x_gm =  v1.x_gm/length;
   v1.y_gm =  v1.y_gm/length;
   v1.z_gm =  v1.z_gm/length;

   v2.x_gm =  p4.x_gm-p2.x_gm;  /* Note point p4 and not p3 */
   v2.y_gm =  p4.y_gm-p2.y_gm;
   v2.z_gm =  p4.z_gm-p2.z_gm;

   length  =  SQRT(v2.x_gm*v2.x_gm+v2.y_gm*v2.y_gm+v2.z_gm*v2.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v2=0%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v2.x_gm =  v2.x_gm/length;
   v2.y_gm =  v2.y_gm/length;
   v2.z_gm =  v2.z_gm/length;

   v4.x_gm =  p1.x_gm-p4.x_gm;
   v4.y_gm =  p1.y_gm-p4.y_gm;
   v4.z_gm =  p1.z_gm-p4.z_gm;

   length  =  SQRT(v4.x_gm*v4.x_gm+v4.y_gm*v4.y_gm+v4.z_gm*v4.z_gm);

#ifdef DEBUG
/* Length is checked in varkon_bpl_analyse (sur718)                 */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"v4=0%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   v4.x_gm =  v4.x_gm/length;
   v4.y_gm =  v4.y_gm/length;
   v4.z_gm =  v4.z_gm/length;

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
        sprintf(errbuf,"vdist1%%varkon_bpl_extend3 (sur717)");
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
        sprintf(errbuf,"vdist2%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

   vdist2.x_gm =  vdist2.x_gm/length;
   vdist2.y_gm =  vdist2.y_gm/length;
   vdist2.z_gm =  vdist2.z_gm/length;



   vdist4.x_gm =  v4.x_gm-v2.x_gm;   /* Not v2 and not v3 */
   vdist4.y_gm =  v4.y_gm-v2.y_gm;
   vdist4.z_gm =  v4.z_gm-v2.z_gm;


   length  =  SQRT(vdist4.x_gm*vdist4.x_gm+
                   vdist4.y_gm*vdist4.y_gm+
                   vdist4.z_gm*vdist4.z_gm);

#ifdef DEBUG
/* Angle (length) is checked in varkon_bpl_analyse (sur718)         */
   if ( length < 0.000001 ) 
        {
        sprintf(errbuf,"vdist4%%varkon_bpl_extend3 (sur717)");
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
        sprintf(errbuf,"length1%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot1 = vdist1.x_gm*v1.x_gm+vdist1.y_gm*v1.y_gm+vdist1.z_gm*v1.z_gm;   

   cos_alfa1 = (1.0 + dot1)/length1;
   sin_alfa1 = SQRT(1.0 - cos_alfa1*cos_alfa1);

   if    ( sin_alfa1 > 0.0001 )
     {
     dist1 =       dist/sin_alfa1;
     }
   else
     {
     sprintf(errbuf,"sin_alfa1=0%%varkon_bpl_extend3 (sur717)");
     return(varkon_erpush("SU2993",errbuf));
     }


   length2 =  SQRT((vdist2.x_gm+v2.x_gm)*(vdist2.x_gm+v2.x_gm)+
                   (vdist2.y_gm+v2.y_gm)*(vdist2.y_gm+v2.y_gm)+
                   (vdist2.z_gm+v2.z_gm)*(vdist2.z_gm+v2.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length2 < 0.000001 ) 
        {
        sprintf(errbuf,"length2%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot2 = vdist2.x_gm*v2.x_gm+vdist2.y_gm*v2.y_gm+vdist2.z_gm*v2.z_gm;   

   cos_alfa2 = (1.0 + dot2)/length2;
   sin_alfa2 = SQRT(1.0 - cos_alfa2*cos_alfa2);

   if    ( sin_alfa2 > 0.0001 )
     {
     dist2 =       dist/sin_alfa2;
     }
   else
     {
     sprintf(errbuf,"sin_alfa2=0%%varkon_bpl_extend3 (sur717)");
     return(varkon_erpush("SU2993",errbuf));
     }


   length4 =  SQRT((vdist4.x_gm+v4.x_gm)*(vdist4.x_gm+v4.x_gm)+
                   (vdist4.y_gm+v4.y_gm)*(vdist4.y_gm+v4.y_gm)+
                   (vdist4.z_gm+v4.z_gm)*(vdist4.z_gm+v4.z_gm));

/* Should not be zero, but I am not sure .......   */
   if ( length4 < 0.000001 ) 
        {
        sprintf(errbuf,"length4%%varkon_bpl_extend3 (sur717)");
        return(varkon_erpush("SU2993",errbuf));
        }

   dot4 = vdist4.x_gm*v4.x_gm+vdist4.y_gm*v4.y_gm+vdist4.z_gm*v4.z_gm;   

   cos_alfa4 = (1.0 + dot4)/length4;
   sin_alfa4 = SQRT(1.0 - cos_alfa4*cos_alfa4);

   if    ( sin_alfa4 > 0.0001 )
     {
     dist4 =       dist/sin_alfa4;
     }
   else
     {
     sprintf(errbuf,"sin_alfa4=0%%varkon_bpl_extend3 (sur717)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* Translated (new) corner points for the extended B-plane          */
/*                                                                 !*/

   p_p1e->x_gm        = p1.x_gm - dist1*vdist1.x_gm;
   p_p1e->y_gm        = p1.y_gm - dist1*vdist1.y_gm;
   p_p1e->z_gm        = p1.z_gm - dist1*vdist1.z_gm;

   p_p2e->x_gm        = p2.x_gm - dist2*vdist2.x_gm;
   p_p2e->y_gm        = p2.y_gm - dist2*vdist2.y_gm;
   p_p2e->z_gm        = p2.z_gm - dist2*vdist2.z_gm;

   p_p4e->x_gm        = p4.x_gm - dist4*vdist4.x_gm;
   p_p4e->y_gm        = p4.y_gm - dist4*vdist4.y_gm;
   p_p4e->z_gm        = p4.z_gm - dist4*vdist4.z_gm;


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur717 Exit varkon_bpl_extend3 Extend B-plane with distance %f \n", 
             dist );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
