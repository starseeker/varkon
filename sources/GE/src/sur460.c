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
/*  Function: varkon_cur_spline                    File: sur460.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a spline curve from points and tangents.   */
/*                                                                  */
/*  An input array is considered to be undefined if the pointer     */
/*  is NULL.                                                        */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-01-01   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Checks and initializations         */
static short saabspl();       /* Hasse Persson interpolation        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  idpoint;          /* Identical points criterion     */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
/*-----------------------------------------------------------------!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_spline     Spline with CUR_SPLARR(..chord..)*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmseg        * Initialize DBSeg                       */
/* varkon_ini_gmcur        * Initialize DBCurve                     */
/* v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_cur_saabspl      * Create Saab spline                     */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_cur_spline          */
/* SU2993 = Severe program error (  ) in varkon_cur_spline          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/

     DBstatus         varkon_cur_spline (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  
  DBVector *p_pts,       /* Points                            (ptr) */
  DBVector *p_tan,       /* Tangents                          (ptr) */
  DBint    *p_pcase,     /* Point/tangent case                (ptr) */
                         /* Eq. 1: ....                             */
  DBint     n_poi,       /* Size of arrays p_pts, p_tan and p_pcase */
  DBint     s_case,      /* Spline calculation case:                */
                         /* Eq. 1: Interpolation (no smoothing)     */
                         /* Eq. 2: Smoothing with method ....       */
  DBfloat   c_tol,       /* Fitness tolerance                       */
  DBCurve  *p_cur,       /* Output curve                      (ptr) */
  DBSeg    *p_seg,       /* Alloc. area for segment data      (ptr) */
  DBfloat  *p_c_act )    /* Actual (output) tolerance         (ptr) */

/* Out:                                                             */
/*                                                                  */
/*      Data to p_cur, p_seg and p_c_out.                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

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
"sur460 Enter***varkon_cur_spline  \n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize local variables.                                      */
/*                                                                 !*/

/*!                                                                 */
/* Identical points criterion. Call of varkon_idpoint (sur741).     */
/*                                                                 !*/

   idpoint   = varkon_idpoint();

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Call of internal function initial.                               */
/*                                                                 !*/

   status= initial
     ( p_pts, p_tan, p_pcase, n_poi, s_case, c_tol, 
            p_cur, p_seg, p_c_act );
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "initial%%sur460");
     return(varkon_erpush("SU2973",errbuf));
     }

/*                                                                  */
/* 2. Interpolating spline with end conditions (s_case= 1)          */
/* _______________________________________________________          */
/*                                                                  */
/*                                                                  */

   if  ( s_case == 1 )
     {
     status= saabspl
       ( p_pts, p_tan, n_poi, p_cur, p_seg );
     if  ( status < 0 ) 
       {
#ifdef  DEBUG
       sprintf(errbuf, "saabspl%%sur460");
       varkon_erpush("SU2973",errbuf);
#endif
       return(status);
       }
     }


/*                                                                  */
/* 3. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur460 Exit***varkon_cur_spline \n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/




/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Check input data and initialize output data                   */

    static short initial 
     ( p_pts, p_tan, p_pcase, n_poi, s_case, c_tol, 
           p_cur, p_seg, p_c_act )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_pts;       /* Points                            (ptr) */
  DBVector *p_tan;       /* Tangents                          (ptr) */
  DBint    *p_pcase;     /* Point/tangent case                (ptr) */
                         /* Eq. 1: ....                             */
  DBint     n_poi;       /* Size of arrays p_pts, p_tan and p_pcase */
  DBint     s_case;      /* Spline calculation case:                */
                         /* Eq. 1: Interpolation (no smoothing)     */
                         /* Eq. 2: Smoothing with method ....       */
  DBfloat   c_tol;       /* Fitness tolerance                       */
  DBCurve  *p_cur;       /* Output curve                      (ptr) */
  DBSeg    *p_seg;       /* Alloc. area for segment data      (ptr) */
  DBfloat  *p_c_act;     /* Actual (output) tolerance         (ptr) */


/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

  DBint     i_s;         /* Loop index segment record               */
  char      errbuf[80];  /* String for error message fctn erpush    */

#ifdef DEBUG
  DBint  i;              /* Temporarely used loop index             */
#endif

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*initial Enter  Number of data in arrays n_poi %d\n",
  (int)n_poi);
fflush(dbgfil(SURPAC));
}
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
for ( i=0; i<n_poi; ++i )
  {
  fprintf(dbgfil(SURPAC),"sur460*initial p%d= %g,%g,%g\n",
     (int)i,(double)(p_pts+i)->x_gm,
    (double)(p_pts+i)->y_gm,(double)(p_pts+i)->z_gm);
  }

if ( p_tan    != NULL )
  {
  for ( i=0; i<n_poi; ++i )
    {
    fprintf(dbgfil(SURPAC),"sur460*initial t%d= %g,%g,%g\n",
       (int)i,(double)(p_tan+i)->x_gm,
      (double)(p_tan+i)->y_gm,(double)(p_tan+i)->z_gm);
    }
  }

fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize actual, output tolerance to Undefined                 */
/*                                                                 !*/

  *p_c_act = F_UNDEF;

/*!                                                                 */
/* Check number of data in arrays                                   */
/*                                                                 !*/

   if  ( n_poi < 2  ) 
     {
     sprintf(errbuf, "(sur460)%%");
     return(varkon_erpush("SU4623",errbuf));
     }


/*!                                                                 */
/* Case for segments not yet implemented                            */
/*                                                                 !*/

   if  ( p_pcase != NULL  ) 
     {
     sprintf(errbuf, "p_pcase not impl.%%sur460");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* Check spline calculation case                                    */
/*                                                                 !*/

   if  ( s_case != 1  ) 
     {
     sprintf(errbuf, "s_case not 1%%sur460");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* Tolerance shall be 0 for s_case = 1                              */
/*                                                                 !*/

   if  ( s_case == 1 && fabs(c_tol) > comptol  ) 
     {
     sprintf(errbuf, "c_tol not 0%%sur460");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* Initialize segment data and curve header data                    */
/* Calls of varkon_ini_gmseg (sur779) and varkon_ini_cur (sur778)   */
/*                                                                 !*/

   for (i_s=0; i_s< n_poi-1; i_s= i_s+1) 
     {
     varkon_ini_gmseg ( p_seg + i_s );  
     }


    varkon_ini_gmcur (p_cur);

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** saabspl ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Spline developed by Hasse Persson, Saab. Used in the Saab        */
/* surface definition system FORMELA, in the Gerber CAD system,..   */
/*                                                                  */
/*                                                                  */

    static short saabspl ( p_pts, p_tan, n_poi, p_cur, p_seg )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector *p_pts;        /* Points                            (ptr) */
  DBVector *p_tan;        /* Tangents                          (ptr) */
  DBint    n_poi;        /* Size of arrays p_pts, p_tan and p_pcase */
  DBCurve *p_cur;        /* Output curve                      (ptr) */
  DBSeg   *p_seg;        /* Alloc. area for segment data      (ptr) */


/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  double  *p_x;          /* X coordinates  area (array)       (ptr) */
  double  *p_y;          /* Y coordinates  area (array)       (ptr) */
  double  *p_z;          /* Z coordinates  area (array)       (ptr) */
  double  *p_cl;         /* Chord lengths  area (array)       (ptr) */
  double  *p_cx;         /* X coefficients area (array)       (ptr) */
  double  *p_cy;         /* Y coefficients area (array)       (ptr) */
  double  *p_cz;         /* Z coefficients area (array)       (ptr) */
  double   t_start[3];   /* Start tangent                           */
  double   t_end[3];     /* End   tangent                           */
  int      type;         /* End condition type                      */
                         /* Eq. 1: Use t_start and t_end            */
                         /* Eq. 2: Use t_start only                 */
                         /* Eq. 3: Use t_end   only                 */
                         /* Eq. 4: Do not use t_start and t_end     */
  int      reparam;      /* Reparameterize the output coefficients  */
                         /* Segment paramenter interval:            */
                         /* Eq. 0: 0.0 to chord length              */
                         /* Eq. 1: 0.0 to 1.0                       */
  int      i_start;      /* Start point in x_ , y_ , z_coord        */
  int      i_end;        /* End   point in x_ , y_ , z_coord        */
  DBint   *p_dtan;       /* Indeces for defined tangents      (ptr) */
  DBint    n_dtan;       /* Number  of  defined tangents            */
  DBint    i_dtan;       /* Index for   defined tangents            */
  DBint    i_s;          /* Loop index segment record               */
/*----------------------------------------------------------------- */

  DBfloat  x_comp;       /* Tangent vector X component              */
  DBfloat  y_comp;       /* Tangent vector Y component              */
  DBfloat  z_comp;       /* Tangent vector Z component              */
  DBint    i_poi;        /* Loop index point                        */
  DBint    i_branch;     /* Loop index curve branch                 */
  DBint    n_branch;     /* Number of branches                      */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl Enter  Number of data in arrays n_poi %d\n",
  (int)n_poi);
fflush(dbgfil(SURPAC));
}
#endif

/* Initialize local variables                                       */
   i_dtan   = I_UNDEF;

   p_x   = NULL;
   p_y    = NULL;
   p_z    = NULL;
   p_cl   = NULL;
   p_cx   = NULL;
   p_cy   = NULL;
   p_cz   = NULL;
   p_dtan = NULL;

   status   =  0;

/*!                                                                 */
/* Allocate memory for coefficient arrays                           */
/*                                                                 !*/

   p_x = (double *)v3mall(n_poi*sizeof(double),"sur460");
   if  ( p_x   == NULL  ) 
     {
     sprintf(errbuf, "p_x alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }
   p_y = (double *)v3mall(n_poi*sizeof(double),"sur460");
   if  ( p_y   == NULL  ) 
     {
     sprintf(errbuf, "p_y alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }
   p_z = (double *)v3mall(n_poi*sizeof(double),"sur460");
   if  ( p_z   == NULL  ) 
     {
     sprintf(errbuf, "p_z alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }

   p_cl = (double *)v3mall(n_poi*sizeof(double),"sur460");
   if  ( p_cl   == NULL  ) 
     {
     sprintf(errbuf, "p_cl alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }

   p_cx = (double *)v3mall(4*n_poi*sizeof(double),"sur460");
   if  ( p_cx   == NULL  ) 
     {
     sprintf(errbuf, "p_cx alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }
   p_cy = (double *)v3mall(4*n_poi*sizeof(double),"sur460");
   if  ( p_cy   == NULL  ) 
     {
     sprintf(errbuf, "p_cy alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }
   p_cz = (double *)v3mall(4*n_poi*sizeof(double),"sur460");
   if  ( p_cz   == NULL  ) 
     {
     sprintf(errbuf, "p_cz alloc.%%sur460");
     varkon_erpush("SU2993",errbuf);
     goto err1;
     }

   p_dtan = (int  *)v3mall(4*n_poi*sizeof(DBint),"sur460");
   if  ( p_dtan   == NULL  ) 
     {
     sprintf(errbuf, "p_dtan alloc.%%sur460");
     return(varkon_erpush("SU2993",errbuf));
     goto err1;
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl Memory has been allocated for arrays \n");
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Points to coordinate arrays                                      */
/*                                                                 !*/

for ( i_poi=0; i_poi<n_poi; ++i_poi )
  {
     *(p_x+i_poi) = (double)(p_pts+i_poi)->x_gm;
     *(p_y+i_poi) = (double)(p_pts+i_poi)->y_gm;
     *(p_z+i_poi) = (double)(p_pts+i_poi)->z_gm;
  }

/*!                                                                 */
/* Initialize coefficient arrays for Debug On                       */
/*                                                                 !*/

#ifdef DEBUG
for ( i_poi=0; i_poi<n_poi; ++i_poi )
  {
     *(p_cx+i_poi)    = F_UNDEF;
     *(p_cy+i_poi)    = F_UNDEF;
     *(p_cz+i_poi)    = F_UNDEF;
     *(p_dtan+i_poi)  = I_UNDEF;
  }
#endif

/*!                                                                 */
/* Find points with defined tangents                                */
/*                                                                 !*/

  n_dtan   = 0;
  n_branch = 1;
  if ( p_tan != NULL )
    {
    for ( i_poi=0; i_poi<n_poi; ++i_poi )
      {
         x_comp = (p_tan+i_poi)->x_gm;
         y_comp = (p_tan+i_poi)->y_gm;
         z_comp = (p_tan+i_poi)->z_gm;

         if ( fabs(x_comp) > idpoint ||
              fabs(y_comp) > idpoint ||
              fabs(z_comp) > idpoint )
           {
           /* Tangent is defined */
           *(p_dtan+n_dtan) = i_poi+1;
           n_dtan = n_dtan + 1;
           if ( i_poi != 0 && i_poi != n_poi-1) n_branch = n_branch+1; 
           }
      } /* i_poi= 1,2, ..,n_poi */
    } /* p_tan != NULL */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl n_dtan %d n_branch %d\n",
  (int)n_dtan , (int)n_branch);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && n_dtan > 0 )
{
for ( i_dtan=0; i_dtan<n_dtan; ++i_dtan )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl i_dtan %d *(p_dtan+i_dtan) %d\n",
  (int)i_dtan , (int)(*(p_dtan+i_dtan)));
fflush(dbgfil(SURPAC));
}
}
#endif

/*!                                                                 */
/* Calculate the spline coefficients                                */
/*                                                                 !*/

  i_start = I_UNDEF;
  i_end   = I_UNDEF;
  i_dtan  = I_UNDEF;
  for ( i_branch = 1; i_branch <= n_branch; ++i_branch )
    {
    t_start[0] = 0.0;
    t_start[1] = 0.0;
    t_start[2] = 0.0;
    t_end[0]   = 0.0;
    t_end[1]   = 0.0;
    t_end[2]   = 0.0;
    if ( i_branch == 1 )
      {
      i_start = 1;
      if ( n_dtan > 0 ) 
        {
        if ( *(p_dtan+0) != i_start ) 
          {
          i_dtan = 0;
          i_end  = *(p_dtan+i_dtan);
          }
        else if ( n_dtan == 1 && *(p_dtan+0) == i_start ) 
          {
          i_dtan = 0;
          i_end  = n_poi;
          }
        else                          
          {
          i_dtan = 1;
          i_end  = *(p_dtan+i_dtan);
          }
        }
      else              i_end = n_poi;
      }
    else if ( i_branch == n_branch )
      {
      i_start = i_end;
      i_end   = n_poi;
      if ( i_dtan == n_dtan-2 ) i_dtan = i_dtan + 1;
      }
    else
      {
      i_start = i_end;
      i_dtan  = i_dtan + 1;
      i_end   = *(p_dtan+i_dtan);
      }
    type = 4;
    if ( (i_dtan   >= 0 && *(p_dtan+i_dtan)   == i_start) ||
         (i_dtan-1 >= 0 && *(p_dtan+i_dtan-1) == i_start)   )
      {
      type       =  2;
      x_comp     = (p_tan+i_start-1)->x_gm;
      y_comp     = (p_tan+i_start-1)->y_gm;
      z_comp     = (p_tan+i_start-1)->z_gm;
      t_start[0] = x_comp;
      t_start[1] = y_comp;
      t_start[2] = z_comp;
      }
    if ( (i_dtan   >= 0 && *(p_dtan+i_dtan)   == i_end) ||
         (i_dtan-1 >= 0 && *(p_dtan+i_dtan-1) == i_end)   )
      {
      if ( type == 2 ) type       =  1;
      else             type       =  3;
      x_comp     = (p_tan+i_end-1)->x_gm;
      y_comp     = (p_tan+i_end-1)->y_gm;
      z_comp     = (p_tan+i_end-1)->z_gm;
      t_end[0]   = x_comp;
      t_end[1]   = y_comp;
      t_end[2]   = z_comp;
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl i_branch %d i_start %d i_end %d type %d i_dtan %d\n",
  (int)i_branch, (int)i_start, (int)i_end, (int)type ,(int)i_dtan);
fflush(dbgfil(SURPAC));
}
#endif

  reparam = 1;

/* TODO Eliminate compiler warning. But how ? */
   status = varkon_cur_saabspl
  ((int)n_poi,i_start,i_end,p_x,p_y,p_z,t_start,t_end,type,idpoint,
            reparam, p_cl, p_cx, p_cy, p_cz );

   if  ( status == -5 )
     {
     varkon_erinit();
     sprintf(errbuf, "(sur460)%%");
     varkon_erpush("SU4603",errbuf);
     goto err1;
     }
   else if  ( status == -6 || status == -7 )
     {
     varkon_erinit();
     sprintf(errbuf, "(sur460)%%");
     varkon_erpush("SU4613",errbuf);
     goto err1;
     }

   else if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur461%%sur460");
     varkon_erpush("SU2943",errbuf);
     goto err1;
     }
    }

/*!                                                                 */
/* Output curve and segment data                                    */
/*                                                                 !*/

/*!                                                                 */
/* Curve header data                                                */
/*                                                                 !*/

   p_cur->ns_cu       = (short)(n_poi-1); 
   p_cur->hed_cu.type = CURTYP;
   p_cur->plank_cu    = FALSE;

/*!                                                                 */
/* Segment data                                                     */
/*                                                                 !*/

   for (i_s=0; i_s< n_poi-1; i_s= i_s+1) 
     {
     (p_seg+i_s)->c0x    = *(p_cx+4*i_s+0);
     (p_seg+i_s)->c0y    = *(p_cy+4*i_s+0);
     (p_seg+i_s)->c0z    = *(p_cz+4*i_s+0);
     (p_seg+i_s)->c0     = 1.0;
     (p_seg+i_s)->c1x    = *(p_cx+4*i_s+1);
     (p_seg+i_s)->c1y    = *(p_cy+4*i_s+1);
     (p_seg+i_s)->c1z    = *(p_cz+4*i_s+1);
     (p_seg+i_s)->c1     = 0.0;
     (p_seg+i_s)->c2x    = *(p_cx+4*i_s+2);
     (p_seg+i_s)->c2y    = *(p_cy+4*i_s+2);
     (p_seg+i_s)->c2z    = *(p_cz+4*i_s+2);
     (p_seg+i_s)->c2     = 0.0;
     (p_seg+i_s)->c3x    = *(p_cx+4*i_s+3);
     (p_seg+i_s)->c3y    = *(p_cy+4*i_s+3);
     (p_seg+i_s)->c3z    = *(p_cz+4*i_s+3);
     (p_seg+i_s)->c3     = 0.0;

     (p_seg+i_s)->ofs    = 0.0;

     (p_seg+i_s)->sl     = 0.0;

     (p_seg+i_s)->typ    = CUB_SEG;

     (p_seg+i_s)->subtyp = 0;
     }
/*!                                                                 */
/* Free memory for coefficient arrays                               */
/*                                                                 !*/

err1:; /* Label: Failure after allocation of memory                 */

   if ( p_x    != NULL ) v3free(p_x,   "sur460");
   if ( p_y    != NULL ) v3free(p_y,   "sur460");
   if ( p_z    != NULL ) v3free(p_z,   "sur460");
   if ( p_cl   != NULL ) v3free(p_cl,  "sur460");
   if ( p_cx   != NULL ) v3free(p_cx,  "sur460");
   if ( p_cy   != NULL ) v3free(p_cy,  "sur460");
   if ( p_cz   != NULL ) v3free(p_cz,  "sur460");
   if ( p_dtan != NULL ) v3free(p_dtan,"sur460");

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur460*saabspl Memory has been deallocated\n");
fflush(dbgfil(SURPAC));
}
#endif

    return(status);

} /* End of function                                                */
/********************************************************************/

