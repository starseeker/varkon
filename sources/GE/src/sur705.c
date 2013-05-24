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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_cir_twoseg                    File: sur705.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create two 2D circle segments from two points and two tangents  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-01   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cir_twoseg     Create two 2D circle segments    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_linecrit         * Criterion straight line                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cir_twoseg           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_cir_twoseg (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBfloat  poi1_x,       /* Start point X coordinate                */
  DBfloat  poi1_y,       /* Start point Y coordinate                */
  DBfloat  poi2_x,       /* End   point X coordinate                */
  DBfloat  poi2_y,       /* End   point Y coordinate                */
  DBfloat  tang1_x_i,    /* Start input tangent X component         */
  DBfloat  tang1_y_i,    /* Start input tangent Y component         */
  DBfloat  tang2_x_i,    /* End   input tangent X component         */
  DBfloat  tang2_y_i,    /* End   input tangent Y component         */
  DBfloat *p_centre1_x,  /* Centre circel 1 X coordinate      (ptr) */
  DBfloat *p_centre1_y,  /* Centre circel 1 Y coordinate      (ptr) */
  DBfloat *p_centre2_x,  /* Centre circel 2 X coordinate      (ptr) */
  DBfloat *p_centre2_y,  /* Centre circel 2 Y coordinate      (ptr) */
  DBfloat *p_circle1_r,  /* Radius for circle 1               (ptr) */
  DBfloat *p_circle2_r,  /* Radius for circle 2               (ptr) */
  DBfloat *p_angle_1,    /* Angle for circle 1  (radians)     (ptr) */
  DBfloat *p_angle_2,    /* Angle for circle 2  (radians)     (ptr) */
  DBfloat *p_poiout_x,   /* End/start point circles X coord.  (ptr) */
  DBfloat *p_poiout_y,   /* End/start point circles Y coord.  (ptr) */
  DBfloat *p_tangout_x,  /* Tangent for poiout_x,poiout_y     (ptr) */
  DBfloat *p_tangout_y ) /* Tangent for poiout_x,poiout_y     (ptr) */

/* Out:                                                             */
/*        Circle data                                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  chord_x;      /* Chord vector (End Pt - Start Pt) X comp */
  DBfloat  chord_y;      /* Chord vector (End Pt - Start Pt) X comp */
  DBfloat  chord_l;      /* Length of the chord                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBfloat  centre1_x;    /* Centre circel 1 X coordinate            */
  DBfloat  centre1_y;    /* Centre circel 1 Y coordinate            */
  DBfloat  centre2_x;    /* Centre circel 2 X coordinate            */
  DBfloat  centre2_y;    /* Centre circel 2 Y coordinate            */
  DBfloat  circle1_r;    /* Radius for circle 1                     */
  DBfloat  circle2_r;    /* Radius for circle 2                     */
  DBfloat  angle_1;      /* Angle for circle 1                      */
  DBfloat  angle_2;      /* Angle for circle 2                      */
  DBfloat  poiout_x;     /* End/start point circles X coord.        */
  DBfloat  poiout_y;     /* End/start point circles Y coord.        */
  DBfloat  tangout_x;    /* Tangent for poiout_x,poiout_y           */
  DBfloat  tangout_y;    /* Tangent for poiout_x,poiout_y           */

  DBfloat  tang1_x;      /* Start tangent (normalized) X component  */
  DBfloat  tang1_y;      /* Start tangent (normalized) Y component  */
  DBfloat  tang2_x;      /* End   tangent (normalized) X component  */
  DBfloat  tang2_y;      /* End   tangent (normalized) Y component  */
  DBfloat  fdum_1;       /* Dummy float                             */
  DBfloat  dot_1;        /* Cosine for angle Chord-Start Tangent    */
  DBfloat  dot_2;        /* Cosine for angle Chord-End   Tangent    */
  DBfloat  r_k;          /* Dummy float                             */
  DBfloat  rsin_1;       /* Sine for angle Chord-Start Tangent      */
  DBfloat  rsin_2;       /* Sine for angle Chord-End   Tangent      */
  DBfloat  dist_x;       /* Distance x from start point to poiout   */
  DBfloat  dist_y;       /* Distance y from start point to poiout   */
  DBfloat  dist_y2;      /* Dummy float dist_y*dist_y               */
  DBfloat  fdum_2;       /* Dummy float                             */
  DBfloat  sign_r_k;     /* Sign of r_k (+1.0 or -1.0)              */

  DBfloat  comptol;      /* Computer accuracy                       */
  DBfloat  idpoint;      /* Identical points criterion              */
  DBfloat  idangle;      /* Identical angles criterion              */
  DBfloat  linecrit;     /* Straight line criterion                 */

  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 Enter*varkon_cir_twoseg*Create two circle segments\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 Start pt      %f %f\n",poi1_x,poi1_y );
fprintf(dbgfil(SURPAC),
"sur705 Start tangent %f %f (Input)\n",tang1_x_i, tang1_y_i );
fprintf(dbgfil(SURPAC),
"sur705 End   pt      %f %f\n",poi2_x,poi2_y );
fprintf(dbgfil(SURPAC),
"sur705 End   tangent %f %f (Input)\n",tang2_x_i, tang2_y_i );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialization of output data                                    */
/*                                                                 !*/

   *p_centre1_x   =  F_UNDEF;
   *p_centre1_y   =  F_UNDEF;
   *p_centre2_x   =  F_UNDEF;
   *p_centre2_y   =  F_UNDEF;
   *p_circle1_r   =  F_UNDEF;
   *p_circle2_r   =  F_UNDEF;
   *p_angle_1     =  F_UNDEF; 
   *p_angle_2     =  F_UNDEF;
   *p_poiout_x    =  F_UNDEF;
   *p_poiout_y    =  F_UNDEF;
   *p_tangout_x   =  F_UNDEF;
   *p_tangout_y   =  F_UNDEF;

/*!                                                                 */
/* Initialization of local variables                                */
/*                                                                 !*/

   centre1_x      =  F_UNDEF;
   centre1_y      =  F_UNDEF;
   centre2_x      =  F_UNDEF;
   centre2_y      =  F_UNDEF;
   circle1_r      =  F_UNDEF;
   circle2_r      =  F_UNDEF;
   angle_1        =  F_UNDEF; 
   angle_2        =  F_UNDEF;
   poiout_x       =  F_UNDEF;
   poiout_y       =  F_UNDEF;
   tangout_x      =  F_UNDEF;
   tangout_y      =  F_UNDEF;
   chord_x        =  F_UNDEF;
   chord_y        =  F_UNDEF;
   chord_l        =  F_UNDEF;

   tang1_x        =  F_UNDEF;
   tang1_y        =  F_UNDEF;
   tang2_x        =  F_UNDEF;
   tang2_y        =  F_UNDEF;
   fdum_1         =  F_UNDEF;
   dot_1          =  F_UNDEF;
   dot_2          =  F_UNDEF;
   r_k            =  F_UNDEF;
   rsin_1         =  F_UNDEF;
   rsin_2         =  F_UNDEF;
   dist_x         =  F_UNDEF;
   dist_y         =  F_UNDEF;
   dist_y2        =  F_UNDEF;
   fdum_2         =  F_UNDEF;
   sign_r_k       =  F_UNDEF;
   comptol        =  F_UNDEF;
   idpoint        =  F_UNDEF;
   idangle        =  F_UNDEF;
   linecrit       =  F_UNDEF;

/*!                                                                 */
/* Surface computer accuracy and coordinate tolerance.              */
/* Call of varkon_comptol (sur753) and varkon_ctol (sur751).        */
/*                                                                 !*/

   comptol     = varkon_comptol();
   idpoint     = varkon_idpoint();
   idangle     = varkon_idangle();
   linecrit    = varkon_linecrit();

/*!                                                                 */
/* 2. Calculation                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* Calculate chord (normalized) and chord length                    */
/*                                                                 !*/

   chord_x  = poi2_x - poi1_x;
   chord_y  = poi2_y - poi1_y;

   chord_l  = sqrt(chord_x*chord_x + chord_y*chord_y);

/*!                                                                 */
/* Check that chord length is at least 10.0*idpoint                 */
/*                                                                 !*/

   if  (  chord_l <  10.0*idpoint )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 Chord length= %f < 10*idpoint= %f\n",
           chord_l, 10.0*idpoint );
  }
#endif
     sprintf(errbuf, "Chord zero%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }

   chord_x   =  chord_x/chord_l;
   chord_y   =  chord_y/chord_l;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 Chord         %f %f\n",chord_x,chord_y   );
fprintf(dbgfil(SURPAC),
"sur705 Chord length  %f\n", chord_l );
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* Normalize (and check) input tangents                             */
/*                                                                 !*/


  fdum_1   = sqrt(tang1_x_i*tang1_x_i + tang1_y_i*tang1_y_i);

   if  (  fdum_1  <  idpoint )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 Start tangent length= %f < idpoint= %f\n",
           fdum_1, idpoint );
  }
#endif
     sprintf(errbuf, "Start tangent 0%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }

   tang1_x    =  tang1_x_i/fdum_1;
   tang1_y    =  tang1_y_i/fdum_1;



   fdum_1     = sqrt(tang2_x_i*tang2_x_i + tang2_y_i*tang2_y_i);

   if  (  fdum_1  <  idpoint )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 End   tangent length= %f < idpoint= %f\n",
           fdum_1, idpoint );
  }
#endif
     sprintf(errbuf, "End tangent 0%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }


   tang2_x    =  tang2_x_i/fdum_1;
   tang2_y    =  tang2_y_i/fdum_1;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 Start tangent %f %f (Normal.)\n",tang1_x,tang1_y   );
fprintf(dbgfil(SURPAC),
"sur705 End   tangent %f %f (Normal.)\n",tang2_x,tang2_y   );
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* Cosines for start and end vectors to the chord vector            */
/* Angles may not exceed 84 degrees!                                */
/*                                                                 !*/

   dot_1   =  tang1_x*chord_x + tang1_y*chord_y;
   dot_2   =  tang2_x*chord_x + tang2_y*chord_y;

   if  (  dot_1   <  0.1     )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 Start tangent angle > 84 degrees dot_1= %f\n",
           dot_1 );
  }
#endif
     sprintf(errbuf, "Start angle > 84%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }

   if  (  dot_2   <  0.1     )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 End   tangent angle > 84 degrees dot_2= %f\n",
           dot_2 );
  }
#endif
     sprintf(errbuf, "End   angle > 84%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }


/* Numerical adjustment of cosine values if > 1.0                   */

   if ( dot_1 > 1.0 ) dot_1 = 1.0;
   if ( dot_2 > 1.0 ) dot_2 = 1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 dot_1 %f dot_2 %f\n", dot_1, dot_2   );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Check if it is a straight line. Tolerance idangle                */
/*                                                                 !*/

   if  ( acos(dot_1)*180.0/4.0/atan(1.0) < idangle &&
         acos(dot_2)*180.0/4.0/atan(1.0) < idangle     )
     {
     angle_1        =  F_UNDEF; 
     angle_2        =  F_UNDEF;
     circle1_r      =  50000.0;
     circle2_r      =  50000.0;
     poiout_x       =  poi1_x + 0.5*(poi2_x-poi1_x);
     poiout_y       =  poi1_y + 0.5*(poi2_y-poi1_y);
     tangout_x      =  chord_x;
     tangout_y      =  chord_y;

     centre1_x      =  poiout_x - circle1_r*tangout_y;
     centre1_y      =  poiout_y + circle1_r*tangout_x;
     centre2_x      =  centre1_x;
     centre2_y      =  centre1_y;
     goto      _exit;
     }


/*!                                                                 */
/* Calculate r_k                                                    */
/*                                                                 !*/


   fdum_1  =  tang1_x*tang2_y - tang2_x*tang1_y;

   if  ( fabs(fdum_1) < comptol ) r_k = 0.0;
   else 
    r_k  = (1.0 + tang1_y*tang2_y + tang1_x*tang2_x)/fdum_1;

   if ( r_k > 0.0 ) sign_r_k =  1.0;
   else             sign_r_k = -1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 r_k   %f sign_r_k %f\n", r_k, sign_r_k );
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* Sines for start and end vectors to the chord vector              */
/*                                                                 !*/

   rsin_1  = sqrt((1.0-dot_1)*(1.0+dot_1));
   rsin_2  = sqrt((1.0+dot_2)*(1.0-dot_2));

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 rsin_1 %f rsin_2 %f\n", rsin_1, rsin_2   );
fflush(dbgfil(SURPAC));
}
#endif
 

/*!                                                                 */
/* Delta distances from start point to output point                 */
/* (note that factor 0.2 may be changed in the formula below)       */
/*                                                                 !*/

   dist_x   = 2.0*(rsin_2+0.1)/(rsin_1+rsin_2+0.2);
   dist_y   = 0.5*chord_l*
              (r_k-sqrt(r_k*r_k+2.0*dist_x-dist_x*dist_x)*sign_r_k);
   dist_x   = dist_x*0.5*chord_l;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 dist_x %f dist_y %f\n", dist_x, dist_y );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Output point, which is the end point of the first circle segment */
/* and the start point of the seconf circle.                        */
/*                                                                 !*/

  poiout_x  =  dist_x*chord_x - dist_y*chord_y + poi1_x;
  poiout_y  =  dist_x*chord_y + dist_y*chord_x + poi1_y;

/*!                                                                 */
/* Radius and centre for the first  circle segment                  */
/*                                                                 !*/

   rsin_1  = tang1_x*(poiout_y-poi1_y) - tang1_y*(poiout_x-poi1_x);
   dist_y2 = dist_y*dist_y;
   fdum_2  = idpoint*0.01;

   if ( fabs(rsin_1) > fdum_2 )
     circle1_r   = (dist_x*dist_x + dist_y2)/rsin_1*0.5;
   else  
     circle1_r   = 50000.0; 

   centre1_x   = poi1_x - circle1_r*tang1_y;
   centre1_y   = poi1_y + circle1_r*tang1_x;

   circle1_r    = fabs(circle1_r);


/*!                                                                 */
/* Radius and centre for the second circle segment                  */
/*                                                                 !*/

   rsin_1 = tang2_x*(poiout_y-poi2_y) - tang2_y*(poiout_x-poi2_x);
   dist_x = dist_x - chord_l;

   if ( fabs(rsin_1) > fdum_2 )
     circle2_r  = (dist_x*dist_x + dist_y2)/rsin_1*0.5;
   else 
     circle2_r = 50000.0;

   centre2_x  = poi2_x - circle2_r*tang2_y;
   centre2_y  = poi2_y + circle2_r*tang2_x;
 
   circle2_r  = fabs(circle2_r);

/*!                                                                 */
/* Tangent for the output point                                     */
/*                                                                 !*/


   tangout_x  = -poiout_y + centre1_y;
   tangout_y  =  poiout_x - centre1_x;
   fdum_1     = sqrt(tangout_x*tangout_x + tangout_y*tangout_y);

   if  (  fdum_1  <  idpoint )
     {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 Output tangent length= %f < idpoint= %f\n",
           fdum_1, idpoint );
  }
#endif
     sprintf(errbuf, "Output tangent 0%%(sur705");
     return(varkon_erpush("SU2993",errbuf));
     }

   tangout_x = tangout_x/fdum_1;
   tangout_y = tangout_y/fdum_1;

/*!                                                                 */
/* Angles for the output circle segments                            */
/*                                                                 !*/

   dot_1   =  ((poi1_x - centre1_x )*(poiout_x - centre1_x)+
               (poi1_y - centre1_y )*(poiout_y - centre1_y)) 
               /circle1_r/circle1_r;

   if ( dot_1 >  1.0 ) dot_1 =  1.0;
   if ( dot_1 < -1.0 ) dot_1 = -1.0;

   angle_1 = acos(dot_1);

   dot_2   =  ((poi2_x - centre2_x )*(poiout_x - centre2_x)+
               (poi2_y - centre2_y )*(poiout_y - centre2_y)) 
               /circle2_r/circle2_r;

   if ( dot_2 >  1.0 ) dot_2 =  1.0;
   if ( dot_2 < -1.0 ) dot_2 = -1.0;

   angle_2 = acos(dot_2);


/* Angles in degrees No output in radians !!!!                      */

/*   angle_1 = angle_1*180.0/4.0/atan(1.0); */
/*   angle_2 = angle_2*180.0/4.0/atan(1.0); */


/*! 3. Exit                                                         */
/*  _______                                                         */

_exit: /* Label:  Exit                                             !*/

   *p_centre1_x   =  centre1_x;
   *p_centre1_y   =  centre1_y;
   *p_centre2_x   =  centre2_x;
   *p_centre2_y   =  centre2_y;
   *p_circle1_r   =  circle1_r;
   *p_circle2_r   =  circle2_r;
   *p_angle_1     =  angle_1; 
   *p_angle_2     =  angle_2;
   *p_poiout_x    =  poiout_x;
   *p_poiout_y    =  poiout_y;
   *p_tangout_x   =  tangout_x;
   *p_tangout_y   =  tangout_y;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 *p_poiout_x  %f *p_poiout_y  %f\n", 
        *p_poiout_x,    *p_poiout_y );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 *p_tangout_x %f *p_tangout_y %f\n", 
        *p_tangout_x,   *p_tangout_y );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 *p_centre1_x %f *p_centre1_y %f *p_circle1_r %f\n", 
        *p_centre1_x,   *p_centre1_y,   *p_circle1_r );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 *p_centre2_x %f *p_centre2_y %f *p_circle2_r %f\n", 
        *p_centre2_x,   *p_centre2_y,   *p_circle2_r );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur705 Exit  *** varkon_cir_twoseg   *\n");
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/

