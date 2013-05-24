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
/*  Function: varkon_sur_order                     File: sur980.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function orders surface (solution) points                   */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-09   Originally written                                 */
/*  1995-08-21   varkon_ctol replaced by varkon_idpoint             */
/*  1996-05-28   errbuf's deleted                                   */
/*  1996-06-14   varkon_ctol, ocase_in and varkon_ini_evals         */
/*  1998-02-08   Debug                                              */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_order      Order surface (solution) points  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short r3dist();        /* Distance in R*3 space              */
static short uvdist();        /* Distance in U,V plane              */
static short addsol();        /* Calculate "add solution distance"  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat idpttol;           /* Identical point tolerance      */
static DBfloat itol;              /* Infinite distance criterion    */
static DBfloat dist;              /* Distance                       */
static DBfloat a_dist;            /* "Add solution distance"        */
static DBint   pflag[1000];       /* Point flag Eq. -1 Sorted point */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol           * Coordinate tolerance                     */
/* varkon_idpoint        * Identical point criterion                */
/* varkon_infinitetol    * Infinite distance criterion              */
/* varkon_ini_evals      * Initialize EVALS                         */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_order   (sur980).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_sur_order (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_refpt,    /* Comparison point (R*3 or U,V)     (ptr) */
   EVALS    xyz_in[],    /* All input  (solution) points            */
   DBint    n_inpts,     /* Number of input  points                 */
   DBint    ocase_in,    /* Ordering case                           */
                         /* See below in program !!!!!              */
/* Out:                                                             */
   EVALS    xyz_out[],   /* All output (solution) points            */
   DBint   *p_n_outpts ) /* Number of output points           (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   ocase;        /* Ordering case                           */
   DBint   i_pt;         /* Loop index point                        */
   DBint   i_o;          /* Loop index outer                        */
   DBint   i_i;          /* Loop index inner                        */
   DBint   i_min;        /* Element number for min_dist             */
   DBfloat min_dist;     /* Minimum distance                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat ctol;         /* Coordinate tolerance                    */
   DBfloat idpoint;      /* Identical point criterion               */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
  "sur980 Enter ocase_in %d n_inpts= %d\n",
       (int)ocase_in, (int)n_inpts);
  fprintf(dbgfil(SURPAC),
  "sur980 p_refpt %12.4f %12.4f %12.4f \n",
   p_refpt->x_gm,  p_refpt->y_gm,  p_refpt->z_gm);
for (i_pt=1; i_pt<=n_inpts;   i_pt= i_pt+1)
  {
  fprintf(dbgfil(SURPAC),
  "sur980 %d %12.4f %12.4f %12.4f \n",(int)i_pt,
   xyz_in[i_pt-1].r_x, xyz_in[i_pt-1].r_y,
   xyz_in[i_pt-1].r_z );
  }
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ___________________________________________                      */
/*                                                                 !*/

  if ( n_inpts == 0 )
    {
    *p_n_outpts = 0;
    varkon_ini_evals  (&xyz_out[0]);         
    return(SUCCED);
    }

/* Check that n_inpts <= 1000 (array pflag dimension)               */

if ( n_inpts > 1000 || n_inpts < 0 )
   {
   sprintf(errbuf,"(pflag dimension) %%varkon_sur_order");
   return(varkon_erpush("SU2993",errbuf));
   }


/*!                                                                 */
/* Coordinate tolerances and infinite distance criterion.           */
/* Call of varkon_idpoint (sur741), varkon_ctol (sur751) and        */
/* varkon_infinitetol (sur754).                                     */
/* The coordinate tolerance used is idpoint for ocase_in= 1 or 2    */
/* and ctol if ocase_in= 10 or 20.                                  */
/*                                                                 !*/

#ifdef  DEBUG
   idpttol = F_UNDEF; 
   ocase   = I_UNDEF;
#endif

   ctol      = varkon_ctol();
   idpoint   = varkon_idpoint();
   itol      = varkon_infinitetol();

if         ( ocase_in == 1 )
   {
   ocase   = 1;
   idpttol = idpoint;
   }
else if    ( ocase_in == 2 )
   {
   ocase   = 2;
   idpttol = idpoint;
   }
else if    ( ocase_in == 10 )
   {
   ocase   = 1;
   idpttol = ctol;
   }
else if    ( ocase_in == 20 )
   {
   ocase   = 2;
   idpttol = ctol;
   }
else
   {
   sprintf(errbuf,"(ocase_in)%%sur980");
   return(varkon_erpush("SU2993",errbuf));
   }

/* Output number of points:                                         */
   *p_n_outpts = 0;

/* Initialize point flag Eq. +1: Point is not used (sorted)         */
for (i_pt=1; i_pt<=n_inpts;   i_pt= i_pt+1)
  {
  pflag[i_pt-1] = 1;
  }

/*!                                                                 */
/* 2.                                                               */
/* ______________________________                                   */
/*                                                                 !*/

for (i_o= 1; i_o<= n_inpts; i_o =i_o +1)  /* Start outer loop       */
  {                                       /*                        */
  min_dist = itol;                        /* Start value for        */
                                          /* the next point         */
  i_min    = -1;                          /* Flag = no pt found     */
  for (i_i= 1; i_i<= n_inpts; i_i= i_i+1) /* Start inner loop       */
    {                                     /*                        */
    if ( pflag[i_i-1] > 0 )               /* Compare only if point  */
      {                                   /* not is used            */
      if      ( ocase == 1 )              /* Calculate R*3 or U,V   */
      r3dist(p_refpt,xyz_in,i_i );        /* distance depending on  */
      else if ( ocase == 2 )              /* ordering case (ocase)  */
      uvdist(p_refpt,xyz_in,i_i );        /*                        */
      if ( dist < min_dist )              /* If "shorter distance"  */
         {                                /*                        */
         min_dist = dist;                 /*                        */
         i_min    = i_i;                  /*                        */
         }                                /* End if "shorter dist." */
      }                                   /* End if "pt is not used"*/
    }                                     /* End   inner loop       */
    if ( i_min > 0 )                      /* Distance is less       */
       {                                  /* pt will be added       */
                                          /* if not already         */
                                          /* existing               */
       pflag[i_min-1] = -1;               /* Pt is always "used"    */
       addsol(xyz_in,i_min,               /* Minimum distance to    */
             xyz_out,*p_n_outpts);        /* existing solutions     */
       if ( a_dist >= idpttol )           /* Add solution if min.   */
         {                                /* distance >= idpttol    */
         *p_n_outpts= *p_n_outpts + 1;    /* An additional point    */
         xyz_out[*p_n_outpts-1] =         /* R*3 and U,V data       */
            xyz_in[i_min-1];              /* to output array        */
         }                                /* End if a_dist< idpttol */
       }                                  /* End if i_min > 0       */
  }                                       /* End   outer loop       */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980 Exit varkon_sur_order  No output pts %d Used tolerance %f\n",
     (int)*p_n_outpts , idpttol );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!************** Internal ** function ** r3dist  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate R*3 distance between current input (solution)         */
/*  point and a given point.                                        */
/*                                                                  */
               static  short   r3dist (p_cp,xyz_in,i_pt)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_cp;       /* Comparison point                        */
   EVALS    xyz_in[];    /* All input  (solution) points            */
   DBint    i_pt;        /* Current point in array xyz_in           */
/*                                                                  */
/*                                                                  */
/* Out:                                                             */
/* Global (static) variable dist                                    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

  dist = SQRT (
  (p_cp->x_gm-xyz_in[i_pt -1].r_x)*(p_cp->x_gm-xyz_in[i_pt -1].r_x) +
  (p_cp->y_gm-xyz_in[i_pt -1].r_y)*(p_cp->y_gm-xyz_in[i_pt -1].r_y) +
  (p_cp->z_gm-xyz_in[i_pt -1].r_z)*(p_cp->z_gm-xyz_in[i_pt -1].r_z) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*r3dist  dist= %f \n" , dist       );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!************** Internal ** function ** uvdist  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate U,V distance between current input (solution)         */
/*  point and a given point.                                        */
/*                                                                  */
               static  short   uvdist (p_cp,xyz_in,i_pt)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_cp;       /* Comparison point                        */
   EVALS    xyz_in[];    /* All input  (solution) points            */
   DBint    i_pt;        /* Current point in array xyz_in           */
/*                                                                  */
/*                                                                  */
/* Out:                                                             */
/* Global (static) variable dist                                    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*uvdist  Comparison point %f %f \n",
     p_cp->x_gm, p_cp->y_gm);
  fprintf(dbgfil(SURPAC),
  "sur980*uvdist  Current UV point %f %f \n",
    xyz_in[i_pt-1].u,  xyz_in[i_pt-1].v); 
  fflush(dbgfil(SURPAC)); 
  }
#endif

  dist = SQRT (
  (p_cp->x_gm-xyz_in[i_pt -1].u)*(p_cp->x_gm-xyz_in[i_pt -1].u) +
  (p_cp->y_gm-xyz_in[i_pt -1].v)*(p_cp->y_gm-xyz_in[i_pt -1].v) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*uvdist  dist= %f \n" , dist       );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** addsol  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate minimum R*3 distance to already existing              */
/*  solutions.                                                      */
/*                                                                  */
       static  short   addsol (xyz_in,i_min,xyz_out,n_outpts)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS   xyz_in[];     /* All input  (solution) points            */
   DBint   i_min;        /* Candidate point as solution             */
   EVALS   xyz_out[];    /* Already existing solution points        */
   DBint   n_outpts;     /* Curren number of output points          */
/*                                                                  */
/*                                                                  */
/* Out:                                                             */
/* Global (static) variable dist                                    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_sol;        /* Loop index solution                     */
   DBfloat c_dist;       /* Comparison distance                     */
 
/*----------------------------------------------------------------- */


/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*addsol  Candidate as solution point %f %f %f \n",
    xyz_in[i_min-1].r_x,  xyz_in[i_min-1].r_y, xyz_in[i_min-1].r_z); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*addsol  i_min= %d n_outpts= %d\n" , (int)i_min,(int)n_outpts);
  fflush(dbgfil(SURPAC)); 
  }
#endif

  a_dist = itol;

  for (i_sol= 1; i_sol<= n_outpts; i_sol= i_sol+1)    /* Start loop */
    {
    c_dist = SQRT (
    (xyz_out[i_sol-1].r_x-xyz_in[i_min-1].r_x)*
    (xyz_out[i_sol-1].r_x-xyz_in[i_min-1].r_x) +
    (xyz_out[i_sol-1].r_y-xyz_in[i_min-1].r_y)*
    (xyz_out[i_sol-1].r_y-xyz_in[i_min-1].r_y) +
    (xyz_out[i_sol-1].r_z-xyz_in[i_min-1].r_z)*
    (xyz_out[i_sol-1].r_z-xyz_in[i_min-1].r_z) );
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*addsol  c_dist= %f i_sol= %d i_min= %d\n" , 
      c_dist,(int)i_sol,(int)i_min       );
  fflush(dbgfil(SURPAC));
  }
#endif

    if ( c_dist < a_dist )                            /* Shorter   */
       {                                              /* distance  */
       a_dist = c_dist;    
       }                  
    }                                                 /* End  loop */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur980*addsol  a_dist= %f \n" , a_dist       );
  fflush(dbgfil(SURPAC)); /* From buffer to file */
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


