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
/*  Function: varkon_cur_saabspl                   File: sur461.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a Hasse Persson (Saab) spline from         */
/*  points and optionally start and end tangents.                   */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-01-01   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_saabspl    Create a Saab spline             */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/* v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*----------------------------------------------------------------- */

/*!------------ Error messages and warnings ------------------------*/
/* Return value  -1: Start point index < 1                          */
/* Return value  -2: End   point index > n_poi                      */
/* Return value  -3: End - start point index < 1                    */
/* Return value  -4: Identical point criterion <= 0.0               */
/* Return value  -5: Two or more consecutive points are equal       */
/* Return value  -6: Start tangent not in start chord direction     */
/* Return value  -7: End   tangent not in end   chord direction     */
/* Return value  -8: End condition type not OK                      */
/* Return value  -9: Zero start tangent                             */
/* Return value -10: Zero end   tangent                             */
/* Return value -11: Reparameterization flag not 0 or 1             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

           DBstatus   varkon_cur_saabspl (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  
  int      n_poi,        /* Total number of points (data) in arrays */
                         /* Note that the number of data in the     */
                         /* chord length and coefficient arrays is  */
                         /* the (maximum possible) number of spline */
                         /* segments, i.e. n_poi - 1                */
  int      i_start,      /* Start point in x_ , y_ , z_coord        */
  int      i_end,        /* End   point in x_ , y_ , z_coord        */
  double   x_coord[],    /* X coordinates                     (ptr) */
  double   y_coord[],    /* Y coordinates                     (ptr) */
  double   z_coord[],    /* Z coordinates                     (ptr) */
  double   t_start[],    /* Start tangent                           */
  double   t_end[],      /* End   tangent                           */
  int      type,         /* End condition type                      */
                         /* Eq. 1: Use t_start and t_end            */
                         /* Eq. 2: Use t_start only                 */
                         /* Eq. 3: Use t_end   only                 */
                         /* Eq. 4: Do not use t_start and t_end     */
  double   idpoint,      /* Identical point criterion               */
  int      reparam,      /* Reparameterize the output coefficients  */
                         /* Segment paramenter interval:            */
                         /* Eq. 0: 0.0 to chord length              */
                         /* Eq. 1: 0.0 to 1.0                       */
  double   l_chord[],    /* Chord lengths                     (ptr) */
                         /* Minimum (allocated) size n_poi-1        */
  double   x_coeff[][4], /* X coefficients                    (ptr) */
                         /* Minimum (allocated) size n_poi-1        */
  double   y_coeff[][4], /* Y coefficients                    (ptr) */
                         /* Minimum (allocated) size n_poi-1        */
  double   z_coeff[][4] )/* Z coefficients                    (ptr) */
                         /* Minimum (allocated) size n_poi-1        */

/* Out:                                                             */
/*                                                                  */
/*      Data to l_coord, x_coeff, y_coeff, z_coeff                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  int      i_seg;        /* Loop index segment                      */
  int      n_seg;        /* Number of segments                      */
  double  *p_rx;         /* Area (array) rix, i=1, ..,n_seg         */
  double  *p_ry;         /* Area (array) riy, i=1, ..,n_seg         */
  double  *p_rz;         /* Area (array) riz, i=1, ..,n_seg         */
  double  *p_k1x;        /* Area (array) k1ix, i=1, ..,n_seg        */
  double  *p_k1y;        /* Area (array) k1iy, i=1, ..,n_seg        */
  double  *p_k1z;        /* Area (array) k1iz, i=1, ..,n_seg        */
  double  *p_k2x;        /* Area (array) k2ix, i=1, ..,n_seg        */
  double  *p_k2y;        /* Area (array) k2iy, i=1, ..,n_seg        */
  double  *p_k2z;        /* Area (array) k2iz, i=1, ..,n_seg        */
  double  *p_mx;         /* Area (array) mix, i=1, ..,n_seg         */
  double  *p_my;         /* Area (array) miy, i=1, ..,n_seg         */
  double  *p_mz;         /* Area (array) miz, i=1, ..,n_seg         */
  double   t1_x;         /* Start normalised tangent X component    */
  double   t1_y;         /* Start normalised tangent Y component    */
  double   t1_z;         /* Start normalised tangent Z component    */
  double   tn_x;         /* End   normalised tangent X component    */
  double   tn_y;         /* End   normalised tangent Y component    */
  double   tn_z;         /* End   normalised tangent Z component    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  double   r1_x;         /* First r1 X value                        */
  double   r1_y;         /* First r1 Y value                        */
  double   r1_z;         /* First r1 Z value                        */
  double   k1_pre;       /* Previous k1 value                       */
  double   k2_pre;       /* Previous k2 value                       */
  double   k1_cur;       /* Current  k1 value                       */
  double   k2_cur;       /* Current  k2 value                       */
  double   r_pre;        /* Previous r  value                       */
  double   r_cur;        /* Current  r  value                       */
  double   m_nxt;        /* Next     m  value                       */
  double   m_cur;        /* Current  m  value                       */
  double   denom;        /* Denominator                             */
  double   v_length;     /* Length of a vector                      */
  double   dot;          /* Scalar product                          */
  short    status;       /* Error code                              */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 Enter***varkon_cur_saabspl \n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize local variables.                                      */
/*                                                                 !*/

   i_seg    = I_UNDEF;
   n_seg    = I_UNDEF;
   r1_x     = F_UNDEF;
   r1_y     = F_UNDEF;
   r1_z     = F_UNDEF;
   t1_x     = F_UNDEF;
   t1_y     = F_UNDEF;
   t1_z     = F_UNDEF;
   tn_x     = F_UNDEF;
   tn_y     = F_UNDEF;
   tn_z     = F_UNDEF;
   v_length = F_UNDEF;
   denom    = F_UNDEF;
   k1_pre   = F_UNDEF;
   k1_cur   = F_UNDEF;
   k2_pre   = F_UNDEF;
   k2_cur   = F_UNDEF;
   r_pre    = F_UNDEF;
   r_cur    = F_UNDEF;
   m_nxt    = F_UNDEF;
   m_cur    = F_UNDEF;
   dot      = F_UNDEF;

   status   = 0;

/*!                                                                 */
/* Check end condition type.                                        */
/*                                                                 !*/

  if  ( type == 1 || type == 2 || type == 3 || type == 4 )
    {
    /* OK  */
    }
  else
    {
    status = -8;
    goto  err1;
    }

/*!                                                                 */
/* Check index for start point                                      */
/*                                                                 !*/

   if  ( i_start < 1 )
    {
    status = -1;
    goto  err1;
    }

/*!                                                                 */
/* Check index for end   point                                      */
/*                                                                 !*/

   if  ( i_end > n_poi )
    {
    status = -2;
    goto  err1;
    }

/*!                                                                 */
/* Check difference between start and end index (min. 2 points)     */
/*                                                                 !*/

   if  ( i_end - i_start < 1 )
    {
    status = -3;
    goto  err1;
    }

/*!                                                                 */
/* Check idpoint criterion                                          */
/*                                                                 !*/

   if  ( idpoint <= 0 )
    {
    status = -4;
    goto  err1;
    }

/*!                                                                 */
/* Check start vector length and normalize for type= 1 and type= 2  */
/*                                                                 !*/

   if  ( type == 1 || type == 2 )
    {
    t1_x = t_start[0];
    t1_y = t_start[1];
    t1_z = t_start[2];
    v_length = sqrt(t1_x*t1_x+t1_y*t1_y+t1_z*t1_z);
    if ( v_length < idpoint )
      {
      status = -9;
      goto  err1;
      }
    t1_x = t1_x/v_length;
    t1_y = t1_y/v_length;
    t1_z = t1_z/v_length;
    }

/*!                                                                 */
/* Check end   vector length and normalize for type= 1 and type= 3  */
/*                                                                 !*/

   if  ( type == 1 || type == 3 )
    {
    tn_x = t_end[0];
    tn_y = t_end[1];
    tn_z = t_end[2];
    v_length = sqrt(tn_x*tn_x+tn_y*tn_y+tn_z*tn_z);
    if ( v_length < idpoint )
      {
      status = -10;
      goto  err1;
      }
    tn_x = tn_x/v_length;
    tn_y = tn_y/v_length;
    tn_z = tn_z/v_length;
    }

/*!                                                                 */
/* Check reparameterization flag                                    */
/*                                                                 !*/

   if  ( reparam == 0 || reparam == 1 );
   else
    {
    status = -11;
    goto  err1;
    }

/*!                                                                 */
/* 2. Calculate chord lengths (for all points)                      */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

  n_seg = n_poi - 1;
for ( i_seg=0; i_seg<n_seg;   ++i_seg )
  {
  l_chord[i_seg] = sqrt (
  (x_coord[i_seg+1]-x_coord[i_seg])*(x_coord[i_seg+1]-x_coord[i_seg])+
  (y_coord[i_seg+1]-y_coord[i_seg])*(y_coord[i_seg+1]-y_coord[i_seg])+
  (z_coord[i_seg+1]-z_coord[i_seg])*(z_coord[i_seg+1]-z_coord[i_seg]) );
  if ( l_chord[i_seg] < idpoint ) status = -5;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d l_chord %f\n", (int)i_seg, l_chord[i_seg]);
fflush(dbgfil(SURPAC));
}
#endif
  } /* End loop i_seg */

  if ( status < 0 ) goto err1;

/*!                                                                 */
/* Check start vector direction with start chord for type= 1 and 2  */
/*                                                                 !*/

   if  ( type == 1 || type == 2 )
    {
    dot = 
          t1_x*(x_coord[i_start]-x_coord[i_start-1])+
          t1_y*(y_coord[i_start]-y_coord[i_start-1])+
          t1_z*(z_coord[i_start]-z_coord[i_start-1]);
    if ( dot < 0 ) 
      {
      status = -6;
      goto  err1;
      }
    }

/*!                                                                 */
/* Check end   vector direction with end   chord for type= 1 and 3  */
/*                                                                 !*/

   if  ( type == 1 || type == 3 )
    {
    dot = 
          tn_x*(x_coord[i_end-1]-x_coord[i_end-2])+
          tn_y*(y_coord[i_end-1]-y_coord[i_end-2])+
          tn_z*(z_coord[i_end-1]-z_coord[i_end-2]);
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur461 x_coord(%3d)= %f x_coord[%3d]= %f Difference %f\n", 
(int)(i_end-1), x_coord[i_end-1], (int)(i_end-2),x_coord[i_end-2]
            , x_coord[i_end-1]-x_coord[i_end-2] );
fflush(dbgfil(SURPAC));
}
#endif
    if ( dot < 0 ) 
      {
      status = -7;
      goto  err1;
      }
    }
/*!                                                                 */
/* 3. Allocate memory for arrays                                    */
/* ______________________________                                   */
/*                                                                 !*/

   p_rx = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_rx == NULL  ) 
     {
     sprintf(errbuf, "p_rx alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_ry = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_ry == NULL  ) 
     {
     sprintf(errbuf, "p_ry alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_rz = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_rz == NULL  ) 
     {
     sprintf(errbuf, "p_rz alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }






   p_k1x = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k1x == NULL  ) 
     {
     sprintf(errbuf, "p_k1x alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_k1y = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k1y == NULL  ) 
     {
     sprintf(errbuf, "p_k1y alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_k1z = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k1z == NULL  ) 
     {
     sprintf(errbuf, "p_k1z alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }


   p_k2x = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k2x == NULL  ) 
     {
     sprintf(errbuf, "p_k2x alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_k2y = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k2y == NULL  ) 
     {
     sprintf(errbuf, "p_k2y alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_k2z = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_k2z == NULL  ) 
     {
     sprintf(errbuf, "p_k2z alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }


   p_mx = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_mx == NULL  ) 
     {
     sprintf(errbuf, "p_mx alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_my = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_my == NULL  ) 
     {
     sprintf(errbuf, "p_my alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }
   p_mz = (double *)v3mall(n_poi*sizeof(double),"sur461");
   if  ( p_mz == NULL  ) 
     {
     sprintf(errbuf, "p_mz alloc.%%sur461");
     return(varkon_erpush("SU2993",errbuf));
     }


/* Initialize  memory for arrays                                    */

for ( i_seg=0; i_seg<n_seg;   ++i_seg )
  {
  *(p_rx +i_seg) = F_UNDEF;
  *(p_ry +i_seg) = F_UNDEF;
  *(p_rz +i_seg) = F_UNDEF;
  *(p_k1x+i_seg) = F_UNDEF;
  *(p_k1y+i_seg) = F_UNDEF;
  *(p_k1z+i_seg) = F_UNDEF;
  *(p_k2x+i_seg) = F_UNDEF;
  *(p_k2y+i_seg) = F_UNDEF;
  *(p_k2z+i_seg) = F_UNDEF;
  *(p_mx +i_seg) = F_UNDEF;
  *(p_my +i_seg) = F_UNDEF;
  *(p_mz +i_seg) = F_UNDEF;
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 Memory (arrays) has been allocated and initialized \n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 4. Tri-diagonal equation system solution                         */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/* Calculate rix, riy and riz                                       */
/*                                                                 !*/

for ( i_seg=i_start-1; i_seg<i_end-1;   ++i_seg )
  {
    *(p_rx+i_seg) = (x_coord[i_seg+1]-x_coord[i_seg])
                          /l_chord[i_seg];
    *(p_ry+i_seg) = (y_coord[i_seg+1]-y_coord[i_seg])
                          /l_chord[i_seg];
    *(p_rz+i_seg) = (z_coord[i_seg+1]-z_coord[i_seg])
                          /l_chord[i_seg];
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur461 x_coord(%3d)= %f x_coord[%3d]= %f\n", 
(int)(i_seg+1), x_coord[i_seg+1], (int)i_seg,x_coord[i_seg] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d rix %f riy %f riz %f\n", 
(int)i_seg, *(p_rx+i_seg), *(p_ry+i_seg), *(p_rz+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif
  }

/*!                                                                 */
/* Calculate k1ix, k1iy and k1iz                                    */
/* Note that k1ix= k1iy =   k1iz                                    */
/*                                                                 !*/

/* Start k1ix, k1iy and k1iz, i= 1                                  */
   if  ( type == 1 || type == 2 )
     {
     *(p_k1x + i_start - 1 ) = -0.5;
     *(p_k1y + i_start - 1 ) = -0.5;
     *(p_k1z + i_start - 1 ) = -0.5;
     }
   else if  ( type == 4 || type == 3 )
     {
     *(p_k1x + i_start - 1 ) =  0.0;
     *(p_k1y + i_start - 1 ) =  0.0;
     *(p_k1z + i_start - 1 ) =  0.0;
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
i_seg = i_start-1;
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k1ix %f = k1iy %f = k1iz %f\n", 
(int)i_seg, *(p_k1x+i_seg), *(p_k1y+i_seg), *(p_k1z+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif

   for ( i_seg=i_start; i_seg<i_end-1;   ++i_seg )
     {
     k1_pre = *(p_k1x + i_seg - 1);
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k1_pre %f denom %f (x)\n", 
(int)i_seg, k1_pre, denom );
fflush(dbgfil(SURPAC));
}
#endif
     if ( fabs(denom) < 0.000001 )
       {
       status = -99;
       goto err2;
       }
     *(p_k1x+i_seg) = - l_chord[i_seg]/denom;

     k1_pre = *(p_k1y + i_seg - 1);
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k1_pre %f denom %f (y)\n", 
(int)i_seg, k1_pre, denom );
fflush(dbgfil(SURPAC));
}
#endif
     if ( fabs(denom) < 0.000001 )
       {
       status = -98;
       goto err2;
       }
     *(p_k1y+i_seg) = - l_chord[i_seg]/denom;

     k1_pre = *(p_k1z + i_seg - 1);
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -97;
       goto err2;
       }
     *(p_k1z+i_seg) = - l_chord[i_seg]/denom;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k1ix %f = k1iy %f = k1iz %f\n", 
(int)i_seg, *(p_k1x+i_seg), *(p_k1y+i_seg), *(p_k1z+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif
     } /* End i_seg loop */

/*!                                                                 */
/* Calculate k2ix, k2iy and k2iz                                    */
/*                                                                 !*/

/* Start k2ix, k2iy and k2iz, i= 1                                  */
  if  ( type == 1 || type == 2 )
    {
     i_seg = i_start - 1;
     denom = 2.0*l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -96;
       goto err2;
       }
     r1_x = *(p_rx + i_start - 1);
     *(p_k2x + i_start - 1 ) =  (r1_x-t1_x)/denom;
     r1_y = *(p_ry + i_start - 1);
     *(p_k2y + i_start - 1 ) =  (r1_y-t1_y)/denom;
     r1_z = *(p_rz + i_start - 1);
     *(p_k2z + i_start - 1 ) =  (r1_z-t1_z)/denom;
    }
  else if  ( type == 4 || type == 3 )
    {
     *(p_k2x + i_start - 1 ) =  0.0;
     *(p_k2y + i_start - 1 ) =  0.0;
     *(p_k2z + i_start - 1 ) =  0.0;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
i_seg = i_start-1;
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k2ix %f k2iy %f k2iz %f\n", 
(int)i_seg, *(p_k2x+i_seg), *(p_k2y+i_seg), *(p_k2z+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif

   for ( i_seg=i_start; i_seg<i_end-1;   ++i_seg )
     {
     k1_pre = *(p_k1x + i_seg - 1);
     k2_pre = *(p_k2x + i_seg - 1);
     r_pre  = *(p_rx  + i_seg - 1);
     r_cur  = *(p_rx  + i_seg );
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -95;
       goto err2;
       }
     *(p_k2x+i_seg)= (r_cur-r_pre-l_chord[i_seg-1]*k2_pre)/denom;


     k1_pre = *(p_k1y + i_seg - 1);
     k2_pre = *(p_k2y + i_seg - 1);
     r_pre  = *(p_ry  + i_seg - 1);
     r_cur  = *(p_ry  + i_seg );
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -94;
       goto err2;
       }
     *(p_k2y+i_seg)= (r_cur-r_pre-l_chord[i_seg-1]*k2_pre)/denom;

     k1_pre = *(p_k1z + i_seg - 1);
     k2_pre = *(p_k2z + i_seg - 1);
     r_pre  = *(p_rz  + i_seg - 1);
     r_cur  = *(p_rz  + i_seg );
     denom = l_chord[i_seg-1]*(k1_pre+2.0) + 2.0*l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -93;
       goto err2;
       }
     *(p_k2z+i_seg)= (r_cur-r_pre-l_chord[i_seg-1]*k2_pre)/denom;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d k2ix %f k2iy %f k2iz %f\n", 
(int)i_seg, *(p_k2x+i_seg), *(p_k2y+i_seg), *(p_k2z+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif
     } /* End i_seg loop */

/*!                                                                 */
/* Calculate mix, miy and miz                                       */
/*                                                                 !*/

/* End       mix, miy and miz , i = i_end-1                         */
   i_seg = i_end - 2;
   if  ( type == 1 || type == 3 )
     {
      k1_pre = *(p_k1x + i_seg );
      k2_pre = *(p_k2x + i_seg );
      r_pre  = *(p_rx  + i_seg );
      denom = l_chord[i_seg]*k1_pre + 2.0*l_chord[i_seg];
      if ( fabs(denom) < 0.000001 )
        {
        status = -92;
        goto err2;
        }
      *(p_mx+i_end-1)= (tn_x-r_pre-l_chord[i_seg]*k2_pre)/denom;

      k1_pre = *(p_k1y + i_seg  );
      k2_pre = *(p_k2y + i_seg  );
      r_pre  = *(p_ry  + i_seg  );
      denom = l_chord[i_seg  ]*k1_pre + 2.0*l_chord[i_seg  ];
      if ( fabs(denom) < 0.000001 )
        {
        status = -91;
        goto err2;
        }
      *(p_my+i_end-1)= (tn_y-r_pre-l_chord[i_seg  ]*k2_pre)/denom;

      k1_pre = *(p_k1z + i_seg  );
      k2_pre = *(p_k2z + i_seg  );
      r_pre  = *(p_rz  + i_seg  );
      denom = l_chord[i_seg  ]*k1_pre + 2.0*l_chord[i_seg  ];
      if ( fabs(denom) < 0.000001 )
        {
        status = -90;
        goto err2;
        }
      *(p_mz+i_end-1)= (tn_z-r_pre-l_chord[i_seg  ]*k2_pre)/denom;
      }
    else if  ( type == 4 || type == 2 )
      {
      *(p_mx+i_end-1) = 0.0;
      *(p_my+i_end-1) = 0.0;
      *(p_mz+i_end-1) = 0.0;
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d mix %f miy %f miz %f\n", 
(int)i_end-1, *(p_mx+i_end-1), *(p_my+i_end-1), *(p_mz+i_end-1) );
fflush(dbgfil(SURPAC));
}
#endif

   for ( i_seg=i_end-2; i_seg>=i_start-1;   --i_seg )
     {
     k1_cur = *(p_k1x + i_seg);
     k2_cur = *(p_k2x + i_seg);
     m_nxt  = *(p_mx  + i_seg + 1);
     *(p_mx+i_seg)= k1_cur*m_nxt + k2_cur;

     k1_cur = *(p_k1y + i_seg);
     k2_cur = *(p_k2y + i_seg);
     m_nxt  = *(p_my  + i_seg + 1);
     *(p_my+i_seg)= k1_cur*m_nxt + k2_cur;

     k1_cur = *(p_k1z + i_seg);
     k2_cur = *(p_k2z + i_seg);
     m_nxt  = *(p_mz  + i_seg + 1);
     *(p_mz+i_seg)= k1_cur*m_nxt + k2_cur;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d mix %f miy %f miz %f\n", 
(int)i_seg, *(p_mx+i_seg), *(p_my+i_seg), *(p_mz+i_seg) );
fflush(dbgfil(SURPAC));
}
#endif
     } /* End i_seg loop */

/*!                                                                 */
/* Calculate coeffients                                             */
/*                                                                 !*/

   for ( i_seg=i_start-1; i_seg<i_end-1;   ++i_seg )
     {

     x_coeff[i_seg][0] = x_coord[i_seg]; 
     r_cur  = *(p_rx  + i_seg );
     m_cur  = *(p_mx  + i_seg );
     m_nxt  = *(p_mx  + i_seg + 1);
     x_coeff[i_seg][1] = r_cur-l_chord[i_seg]*(2.0*m_cur+m_nxt); 
     x_coeff[i_seg][2] = 3.0*m_cur; 
     denom = l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -89;
       goto err2;
       }
     x_coeff[i_seg][3] = (m_nxt-m_cur)/denom; 


     y_coeff[i_seg][0] = y_coord[i_seg]; 
     r_cur  = *(p_ry  + i_seg );
     m_cur  = *(p_my  + i_seg );
     m_nxt  = *(p_my  + i_seg + 1);
     y_coeff[i_seg][1] = r_cur-l_chord[i_seg]*(2.0*m_cur+m_nxt); 
     y_coeff[i_seg][2] = 3.0*m_cur; 
     denom = l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -89;
       goto err2;
       }
     y_coeff[i_seg][3] = (m_nxt-m_cur)/denom; 

     z_coeff[i_seg][0] = z_coord[i_seg]; 
     r_cur  = *(p_rz  + i_seg );
     m_cur  = *(p_mz  + i_seg );
     m_nxt  = *(p_mz  + i_seg + 1);
     z_coeff[i_seg][1] = r_cur-l_chord[i_seg]*(2.0*m_cur+m_nxt); 
     z_coeff[i_seg][2] = 3.0*m_cur; 
     denom = l_chord[i_seg];
     if ( fabs(denom) < 0.000001 )
       {
       status = -89;
       goto err2;
       }
     z_coeff[i_seg][3] = (m_nxt-m_cur)/denom; 


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d x_coeff %f %f %f %f\n", 
(int)i_seg, x_coeff[i_seg][0], x_coeff[i_seg][1], 
            x_coeff[i_seg][2], x_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d y_coeff %f %f %f %f\n", 
(int)i_seg, y_coeff[i_seg][0], y_coeff[i_seg][1], 
            y_coeff[i_seg][2], y_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d z_coeff %f %f %f %f\n", 
(int)i_seg, z_coeff[i_seg][0], z_coeff[i_seg][1], 
            z_coeff[i_seg][2], z_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
#endif
     } /* End i_seg loop */

/*!                                                                 */
/* Reparameterize if reparam= 1                                     */
/*                                                                  */
/*  Cubic segments defined by:                                      */
/*                                                                  */
/*              2   3                                               */
/*  P(u)= (1,u,u , u ) * ! C0 !                                     */
/*                       ! C1 !                                     */
/*                       ! C2 !                                     */
/*                       ! C3 !                                     */
/*                                                                  */
/*  Reparameterise t = u/chord_length                               */
/*                                                                  */
/*              2   3                                               */
/*  P(u)= (1,t,t , t ) * ! C0                 !                     */
/*                       ! C1*chord_length    !                     */
/*                       ! C2*chord_length**2 !                     */
/*                       ! C3*chord_length**3 !                     */
/*                                                                  */
/*                                                                 !*/

   if ( reparam == 1 )
     {
     for ( i_seg=i_start-1; i_seg<i_end-1;   ++i_seg )
       {

       x_coeff[i_seg][0] = x_coeff[i_seg][0]; 
       x_coeff[i_seg][1] = x_coeff[i_seg][1]
               *l_chord[i_seg]; 
       x_coeff[i_seg][2] = x_coeff[i_seg][2]
               *l_chord[i_seg]*l_chord[i_seg]; 
       x_coeff[i_seg][3] = x_coeff[i_seg][3]
               *l_chord[i_seg]*l_chord[i_seg]*l_chord[i_seg]; 

       y_coeff[i_seg][0] = y_coeff[i_seg][0]; 
       y_coeff[i_seg][1] = y_coeff[i_seg][1]
               *l_chord[i_seg]; 
       y_coeff[i_seg][2] = y_coeff[i_seg][2]
               *l_chord[i_seg]*l_chord[i_seg]; 
       y_coeff[i_seg][3] = y_coeff[i_seg][3]
               *l_chord[i_seg]*l_chord[i_seg]*l_chord[i_seg]; 

       z_coeff[i_seg][0] = z_coeff[i_seg][0]; 
       z_coeff[i_seg][1] = z_coeff[i_seg][1]
               *l_chord[i_seg]; 
       z_coeff[i_seg][2] = z_coeff[i_seg][2]
               *l_chord[i_seg]*l_chord[i_seg]; 
       z_coeff[i_seg][3] = z_coeff[i_seg][3]
               *l_chord[i_seg]*l_chord[i_seg]*l_chord[i_seg]; 


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d x_coeff %f %f %f %f\n", 
(int)i_seg, x_coeff[i_seg][0], x_coeff[i_seg][1], 
            x_coeff[i_seg][2], x_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d y_coeff %f %f %f %f\n", 
(int)i_seg, y_coeff[i_seg][0], y_coeff[i_seg][1], 
            y_coeff[i_seg][2], y_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 i_seg %d z_coeff %f %f %f %f\n", 
(int)i_seg, z_coeff[i_seg][0], z_coeff[i_seg][1], 
            z_coeff[i_seg][2], z_coeff[i_seg][3] );
fflush(dbgfil(SURPAC));
}
#endif
       } /* End i_seg loop */
     } /* End reparam=1  */

/*!                                                                 */
/* Reparameterize if reparam= 1                                     */
/*                                                                  */
/*  Cubic segments defined by:                                      */
/*                                                                  */
/*                                                                  */
/* 5. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

/*!                                                                 */
/* Free memory for coefficient arrays                               */
/*                                                                 !*/

err2:;

   v3free(p_rx,   "sur461");
   v3free(p_ry,   "sur461");
   v3free(p_rz,   "sur461");
   v3free(p_k1x,  "sur461");
   v3free(p_k1y,  "sur461");
   v3free(p_k1z,  "sur461");
   v3free(p_k2x,  "sur461");
   v3free(p_k2y,  "sur461");
   v3free(p_k2z,  "sur461");
   v3free(p_mx,   "sur461");
   v3free(p_my,   "sur461");
   v3free(p_mz,   "sur461");

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur461 Memory has been deallocated\n");
fflush(dbgfil(SURPAC));
}
#endif

err1:; /* Label: Exit with error                                    */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur461 Exit***varkon_cur_saabspl status= %d\n",(int)status);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(status);

  } /* End of function */

/*********************************************************/

