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
/*  Function: varkon_cur_order                     File: sur982.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function orders curve (solution) points                     */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-12   Originally written                                 */
/*  1996-05-28   errbuf's deleted                                   */
/*  1996-06-14   varkon_ini_evalc                                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_order      Order curve   (solution) points  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short r3dist();        /* Distance in R*3 space              */
static short udist();         /* Distance in U   space              */
static short addsol();        /* Calculate "add solution distance"  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat ctol;              /* Identical point tolerance      */
static DBfloat itol;              /* Infinite distance criterion    */
static DBfloat dist;              /* Distance                       */
static DBfloat a_dist;            /* "Add solution distance"        */
static DBint   pflag[1000];       /* Point flag Eq. -1 Sorted point */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol           * Coordinate tolerance                     */
/* varkon_infinitetol    * Infinite distance criterion              */
/* varkon_ini_evalc      * Initialize EVALC                         */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_cur_order   (sur982).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus    varkon_cur_order (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_refpt,    /* Comparison point (R*3 or U)       (ptr) */
   EVALC    xyz_in[],    /* All input  (solution) points            */
   DBint    n_inpts,     /* Number of input  points                 */
   DBint    ocase,       /* Ordering case                           */
/* Out:                                                             */
   EVALC    xyz_out[],   /* All output (solution) points            */
   DBint   *p_n_outpts ) /* Number of output points           (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_pt;         /* Loop index point                        */
   DBint   i_o;          /* Loop index outer                        */
   DBint   i_i;          /* Loop index inner                        */
   DBint   i_min;        /* Element number for min_dist             */
   DBfloat min_dist;     /* Minimum distance                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

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
  "sur982 Enter varkon_cur_order **** n_inpts= %d\n",n_inpts);
for (i_pt=1; i_pt<=n_inpts;   i_pt= i_pt+1)
  {
  fprintf(dbgfil(SURPAC),
  "sur982 %d %12.4f %12.4f %12.4f \n",i_pt,
   xyz_in[i_pt-1].r.x_gm, xyz_in[i_pt-1].r.y_gm,
   xyz_in[i_pt-1].r.z_gm );
  }
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/


  if ( n_inpts == 0 )
    {
    *p_n_outpts = 0;
    varkon_ini_evalc (&xyz_out[0]);         
    return(SUCCED);
    }

/* Check that n_inpts <= 1000 (array pflag dimension)               */

if ( n_inpts > 1000 || n_inpts < 0 )
   {
   sprintf(errbuf,"(pflag dimension) %%varkon_cur_order");
   return(varkon_erpush("SU2993",errbuf));
   }


#ifdef DEBUG
if ( ocase == 1 || ocase == 2 )
   {
   /*  OK  */
   }
else
   {
   sprintf(errbuf,"(ocase) %%varkon_cur_order");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

/*!                                                                 */
/* Coordinate tolerance and infinite distance criterion.            */
/* Call of varkon_ctol (sur751) and varkon_infinitetol (sur754).    */
/* The coordinate tolerance used is ctol*60.0 !!!!!                 */
/*                                                                 !*/

   ctol=varkon_ctol();
   itol=varkon_infinitetol();

ctol = 60.0*ctol; /* TODO Think more about this !!!!!!!!!   */


/* Output number of points:                                         */
   *p_n_outpts = 0;

/* Initiate point flag Eq. +1: Point is not used (sorted)           */
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
      if      ( ocase == 1 )              /* Calculate R*3 or U     */
      r3dist(p_refpt,xyz_in,i_i );        /* distance depending on  */
      else if ( ocase == 2 )              /* ordering case (ocase)  */
       udist(p_refpt,xyz_in,i_i );        /*                        */
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
       if ( a_dist >= ctol )              /* Add solution if min.   */
         {                                /* distance >= ctol       */
         *p_n_outpts= *p_n_outpts + 1;    /* An additional point    */
         xyz_out[*p_n_outpts-1] =         /* R*3 and  U  data       */
            xyz_in[i_min-1];              /* to output array        */
         }                                /* End if a_dist < ctol   */
       }                                  /* End if i_min > 0       */
  }                                       /* End   outer loop       */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur982 Exit varkon_cur_order  No output pts %d Used tolerance %f\n",
     *p_n_outpts , ctol );
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
   EVALC     xyz_in[];   /* All input  (solution) points            */
   DBint     i_pt;       /* Current point in array xyz_in           */
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
  (p_cp->x_gm-xyz_in[i_pt -1].r.x_gm)*
  (p_cp->x_gm-xyz_in[i_pt -1].r.x_gm) +
  (p_cp->y_gm-xyz_in[i_pt -1].r.y_gm)*
  (p_cp->y_gm-xyz_in[i_pt -1].r.y_gm) +
  (p_cp->z_gm-xyz_in[i_pt -1].r.z_gm)*
  (p_cp->z_gm-xyz_in[i_pt -1].r.z_gm) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur982*r3dist  dist= %f \n" , dist       );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!************** Internal ** function **  udist  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate  U  distance between current input (solution)         */
/*  point and a given point.                                        */
/*                                                                  */
               static  short    udist (p_cp,xyz_in,i_pt)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_cp;       /* Comparison point                        */
   EVALC     xyz_in[];   /* All input  (solution) points            */
   DBint     i_pt;       /* Current point in array xyz_in           */
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
  "sur982* udist  Comparison point %f %f \n",
     p_cp->x_gm, p_cp->y_gm);
  fprintf(dbgfil(SURPAC),
  "sur982* udist  Current U  point %f \n",
    xyz_in[i_pt-1].t_global); 
  }
#endif

  dist = SQRT (
  (p_cp->x_gm-xyz_in[i_pt -1].t_global)*
  (p_cp->x_gm-xyz_in[i_pt -1].t_global) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur982* udist  dist= %f \n" , dist       );
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
   EVALC   xyz_in[];     /* All input  (solution) points            */
   DBint   i_min;        /* Candidate point as solution             */
   EVALC   xyz_out[];    /* Already existing solution points        */
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
  "sur982*addsol  Candidate as solution point %f %f %f \n",
    xyz_in[i_min-1].r.x_gm,  xyz_in[i_min-1].r.y_gm, 
    xyz_in[i_min-1].r.z_gm); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur982*addsol  i_min= %d n_outpts= %d\n" , i_min,n_outpts);
  }
#endif

  a_dist = itol;

  for (i_sol= 1; i_sol<= n_outpts; i_sol= i_sol+1)    /* Start loop */
    {
    c_dist = SQRT (
    (xyz_out[i_sol-1].r.x_gm-xyz_in[i_min-1].r.x_gm)*
    (xyz_out[i_sol-1].r.x_gm-xyz_in[i_min-1].r.x_gm) +
    (xyz_out[i_sol-1].r.y_gm-xyz_in[i_min-1].r.y_gm)*
    (xyz_out[i_sol-1].r.y_gm-xyz_in[i_min-1].r.y_gm) +
    (xyz_out[i_sol-1].r.z_gm-xyz_in[i_min-1].r.z_gm)*
    (xyz_out[i_sol-1].r.z_gm-xyz_in[i_min-1].r.z_gm) );
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur982*addsol  c_dist= %f i_sol= %d i_min= %d\n" , 
      c_dist,i_sol,i_min       );
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
  "sur982*addsol  a_dist= %f \n" , a_dist       );
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


