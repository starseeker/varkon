/**********************************************************************
*
*    ge208.c
*    =======
*
*    This file includes:
*
*    GE208() Compute line tangent to 2 3D arcs
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://varkon.sourceforge.net
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************************/
/*                                                                  */
/*  Author Gunnar Liden                                             */
/*                                                                  */
/*  Revisions                                                       */
/*  1998-11-15 Copied 2D parts from geo206                          */
/*  1999-03-24 Reorganized, J.Kjellander                            */
/*  1999-12-18 sur741->varkon_idpoint                Gunnar Liden   */
/*                                                                  */
/********************************************************************/


/********************************************************************/
/* Error messages and warnings in (the value of) function geo208    */
/* -------------------------------------------------------------    */
/*                                                                  */
/*      GE2183 = No tangents to the circles exist                   */
/*                                                                  */
/*      GE2192 = Tangentpoint is not between start and end angle    */
/*               of circle 1 or circle 2                            */
/*                                                                  */
/********************************************************************/


   DBstatus GE208(
   DBfloat cir1_x_a,  /* Center X coordinate for circle 1        */
   DBfloat cir1_y_a,  /* Center Y coordinate for circle 1        */
   DBfloat cir1_r_a,  /* Radius              for circle 1        */
   DBfloat cir1_v1_a, /* Start angle         for circle 2        */
   DBfloat cir1_v2_a, /* End   angle         for circle 2        */
   DBfloat cir2_x_a,  /* Center X coordinate for circle 2        */
   DBfloat cir2_y_a,  /* Center Y coordinate for circle 2        */
   DBfloat cir2_r_a,  /* Radius              for circle 2        */
   DBfloat cir2_v1_a, /* Start angle         for circle 2        */
   DBfloat cir2_v2_a, /* End   angle         for circle 2        */
   DBshort alt,       /* Defines the tangent point (1,2,3,4)     */
   DBVector *p_start,   /* Start point                       (ptr) */
   DBVector *p_end,     /* End   point                       (ptr) */
   DBint    *p_n_lin)   /* Number of possible lines          (ptr) */

 {
   gmflt  l;             /* Length from origin 1 to origin 2        */
   gmflt  vx;            /* ? */
   gmflt  vy;            /* ? */
   gmflt  b1;            /* ? */
   gmflt  b2;            /* ? */
   gmflt  fi;            /* ? */
   gmflt  f1;            /* ? */
   gmflt  f2;            /* ? */
   gmflt  f3;            /* ? */
   gmflt  f4;            /* ? */
   gmflt  a1in[4];       /* a1in = (f1,f2,f3   ,f4   )              */
   gmflt  a2in[4];       /* a2in = (f1,f2,f3+PI,f4+PI)              */
   gmflt  a1out[4];      /* Ordered angles                          */
   gmflt  a2out[4];      /* Corresponding angles to a1out           */
   short  noin;          /* Number of elements in                   */
   short  nout;          /* Number of elements out                  */
   short  ndef;          /* The number of solutions within start and*/
                         /* end angles of both the circles          */
   short  k,i;           /* Loop index                              */
   gmflt  dx,dy;         /* Temporary dx and dy                     */
   short  n_tang;        /* Number of possible tangent lines        */
   DBVector  start;      /* The start point of the line             */
   DBVector  end;        /* The end   point of the line             */
   gmflt  idpoint;       /* Identical points criterion              */

/*------------end-of-declarations-----------------------------------*/

/*
***Initializations of output variables
*/
   p_start->x_gm = -0.123456789;
   p_start->y_gm = -0.123456789;
   p_start->z_gm = -0.123456789;
   p_end->x_gm   = -0.123456789;
   p_end->y_gm   = -0.123456789;
   p_end->z_gm   = -0.123456789;
   *p_n_lin      =            0;
/*
***Initializations of local variables for Debug On
*/
#ifdef  DEBUG
      idpoint       = -0.123456789;
      n_tang        =       -12345;
#endif


/*
***Get identical points criterion
*/
   idpoint = varkon_idpoint();

#ifdef DEBUG
if ( dbglev(GEOPAC) ==    1 )
{
fprintf(dbgfil(GEOPAC),
"geo208 Enter Requested solution alt %d \n", (int)alt );
fflush(dbgfil(GEOPAC));
}
if ( dbglev(GEOPAC) ==    1 )
{
fprintf(dbgfil(GEOPAC),
"geo208 Circle 1 Origin %f %f Radius %f\n",
            cir1_x_a, cir1_y_a, cir1_r_a);
fflush(dbgfil(GEOPAC));
}
if ( dbglev(GEOPAC) ==    1 )
{
fprintf(dbgfil(GEOPAC),
"geo208 Circle 2 Origin %f %f Radius %f\n",
            cir2_x_a, cir2_y_a, cir2_r_a);
fflush(dbgfil(GEOPAC));
}
#endif


/*
***Avstånd mellan cirkelcentrum.
*/
      dx = cir2_x_a - cir1_x_a;
      dy = cir2_y_a - cir1_y_a;
      l = SQRT(dx*dx + dy*dy);

/*
***Determine the case (number of possible tangent lines):
*** - Is circle inside the other   <==> Zero lines
*** - Are the circles tangential   <==> Two  lines + one point
*** - Are the circles intersecting <==> Two  lines
*** - Else ...                     <==> Four lines
*/

/*
***Any of the circles inside the other one? 
*/
      if ( cir2_r_a < cir1_r_a && l+cir2_r_a <= cir1_r_a )
          {
          return(erpush("GE2183","geo208"));
          }
      else if ( l+cir1_r_a <= cir2_r_a )
          {
          return(erpush("GE2183","geo208"));
          }
/*
*** Are the circles tangential ?
*/
      if      ( fabs(cir2_r_a+cir1_r_a-l) < idpoint )
        n_tang = 3;
/*
*** Do the circles intersect ?
*/
      else if ( cir2_r_a+cir1_r_a >= l - idpoint )
        n_tang = 2;
/*
*** Two disjunct circles (=else) ?
*/
      else 
        n_tang = 4;

/*
*** Case two lines and a point is for the moment not handled
*/

    if ( 3 == n_tang ) n_tang = 2;


/*
*** Return with error if circles intersect or if they are
*** tangential to each other <==> Requested solution is 
*** greater than the possible number of tangent lines
*/
    if  ( alt > n_tang )
          return(erpush("GE2213","geo208"));

/*
***Beräkna alla 4 tangeringsvinklar, f1-f4.
*/
      if ( cir2_r_a <= cir1_r_a )
        {
        b1 = ACOS((cir1_r_a - cir2_r_a)/l);
        b2 = (cir1_r_a+cir2_r_a)/l;
        if ( b2 <= 1.0 ) b2 =  ACOS(b2);
        else             b2 =  100.0;
        }

      if ( cir2_r_a > cir1_r_a )
        {
        b1 = ACOS((cir2_r_a - cir1_r_a)/l);
        b2 = -(cir1_r_a+cir2_r_a)/l;
        if(-b2 <= 1.0 )b2 =  ACOS(b2);
        else           b2 = 100.0;
        }

      vx = (cir2_x_a-cir1_x_a)/l;
      vy = (cir2_y_a-cir1_y_a)/l;
      if ( cir1_r_a < cir2_r_a )
        {
        vx = -vx;
        vy = -vy;
        }

      fi = ACOS(vx);
      if ( vy < 0.0 ) fi = -fi;
      if ( fi < 0.0 ) fi = fi + PI2;

      f1 = fi + b1;
      f2 = fi - b1;
      f3 = fi + b2;
      f4 = fi - b2;
/*
***Sortering.
*/
      a1in[0] = f1*RDTODG;
      a1in[1] = f2*RDTODG;
      a1in[2] = f3*RDTODG;
      a1in[3] = f4*RDTODG;
      a2in[0] = f1*RDTODG;
      a2in[1] = f2*RDTODG;
      a2in[2] = f3*RDTODG + 180.0;
      a2in[3] = f4*RDTODG + 180.0;

      for ( i=0; i<4; ++i )
        {
        if ( a1in[i] > 360.0 ) a1in[i] -= 360.0;
        if ( a1in[i] < 0.0 ) a1in[i] += 360.0;
        }

      for ( i=0; i<4; ++i )
        {
        if ( a2in[i] > 360.0 ) a2in[i] -= 360.0;
        if ( a2in[i] < 0.0 ) a2in[i] += 360.0;
        }

      noin = 4;
      if ( GEsort_2(a1in,a2in,noin,&nout,a1out,a2out) < 0 )
          return(erpush("GE2123","geo208(GEsort_2)"));
/*
***Välj lösning.
*/
      for ( ndef=i=0; i<4; ++i )
         {
         if ( a1out[i]+360.0 < cir1_v2_a ) a1out[i] += 360.0;
         if ( a1out[i]-360.0 > cir1_v1_a ) a1out[i] -= 360.0;
         if ( a2out[i]+360.0 < cir2_v2_a ) a2out[i] += 360.0;
         if ( a2out[i]-360.0 > cir2_v1_a ) a2out[i] -= 360.0;

         if ( a1out[i] >= cir1_v1_a && a1out[i] <= cir1_v2_a &&
              a2out[i] >= cir2_v1_a && a2out[i] <= cir2_v2_a ) 
            {
            k = i;
            ++ndef;
            if (ndef == alt) goto found;
            }
          }
/*
***Ingen lösning.
*/
          return(erpush("GE2192","geo208"));
/*
***Lösning hittad.
*/
found:
      a1out[k] *= DGTORD;
      start.x_gm = cir1_x_a + cir1_r_a*COS(a1out[k]);
      start.y_gm = cir1_y_a + cir1_r_a*SIN(a1out[k]);
      start.z_gm = 0.0;

      a2out[k] *= DGTORD;
      end.x_gm   = cir2_x_a + cir2_r_a*COS(a2out[k]);
      end.y_gm   = cir2_y_a + cir2_r_a*SIN(a2out[k]);      
      end.z_gm   = 0.0;

/*
***To output parameters
*/
      *p_start =   start;
      *p_end   =     end;
      *p_n_lin =  n_tang;

      return(SUCCED);

} /* End of function */

/********************************************************************/
