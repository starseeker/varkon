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
/*  Function: varkon_sur_selsol                    File: sur984.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function selects surface (solution) points which are        */
/*  within given criteria.                                          */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-08-27   Originally written                                 */
/*  1996-09-02   Case 6 and 7 bug fixed                             */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_selsol     Select surface (solution) points */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short r3dist();        /* Distance in R*3 space              */
static short ndirec();        /* Scalar product surface normal      */
static short sdirec();        /* Scalar product solution direction  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat idpttol;           /* Identical point tolerance      */
static DBfloat itol;              /* Infinite distance criterion    */
static DBfloat dist;              /* Distance                       */
static DBfloat dot_s;             /* Scalar product solution direct.*/
static DBfloat dot_n;             /* Scalar product surface normal  */
static DBint   pflag[1000];       /* Point flag                     */
                                  /* Eq. +1: Output point           */
                                  /* Eq. -1: Not an output point    */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint        * Identical point criterion                */
/* varkon_infinitetol    * Infinite distance criterion              */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_selsol  (sur984).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus   varkon_sur_selsol (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_refpt,    /* Comparison point  (R*3 or U,V)    (ptr) */
   DBVector *p_refvec,   /* Comparison vector (R*3 or U,V)    (ptr) */
   EVALS     xyz_in[],   /* All input  (solution) points            */
   DBint     n_inpts,    /* Number of input  points                 */
   DBint     scase,      /* Select solution case                    */
                         /* Eq. 1: All solution  points             */
                         /* Eq. 2: Solutions in the point vector    */
                         /*        direction (e.g line/surface int) */
                         /* Eq. 3: As 2 but only intersect pts with */
                         /*        surface normals in the shooting  */
                         /*        direction will be output         */
                         /* Eq. 4: As 2 but only intersect pts with */
                         /*        surface normals in the opposite  */
                         /*        shooting direction will be output*/
                         /* Eq. 5: As 3 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        opposite direct. to shooting dir */
                         /* Eq. 6: As 4 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        same     direct. to shooting dir */
/* Out:                                                             */
   EVALS     xyz_out[],  /* All output (solution) points            */
   DBint    *p_n_outpts )/* Number of output points           (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_p;          /* Loop index point                        */
   DBfloat min_dist;     /* Minimum distance to point (not equal    */
                         /* to the input point)                     */
   DBint   i_min;        /* Point number to the minimum dist. pt    */
   DBfloat dot_n_min;    /* Scalar product for the min. dist. pt    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char    errbuf[80];   /* String for error message fctn erpush    */
#ifdef DEBUG
   DBfloat vec_length;   /* Length of the input vector              */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
  "sur984 Enter varkon_sur_selsol**** n_inpts= %d\n",n_inpts);
for (i_p =1; i_p <=n_inpts;   i_p = i_p +1)
  {
  fprintf(dbgfil(SURPAC),
  "sur984 %d %12.4f %12.4f %12.4f \n",i_p ,
   xyz_in[i_p -1].r_x, xyz_in[i_p -1].r_y,
   xyz_in[i_p -1].r_z );
  }
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

/* Check that n_inpts <= 1000 (array pflag dimension)               */

if ( n_inpts > 1000 )
   {
   sprintf(errbuf,"(pflag dimension) %%varkon_sur_selsol");
   return(varkon_erpush("SU2993",errbuf));
   }


#ifdef DEBUG
if ( scase == 1 || scase == 2 || scase == 3 || scase == 4 ||
     scase == 5 || scase == 6  )
   {
   /*  OK  */
   }
else
   {
   sprintf(errbuf,"(scase) %%varkon_sur_selsol");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

/*!                                                                 */
/* Coordinate tolerance and infinite distance criterion.            */
/* Call of varkon_idpoint (sur741) and varkon_infinitetol (sur754). */
/*                                                                 !*/

   idpttol=varkon_idpoint();
   itol   =varkon_infinitetol();

/* Output number of points:                                         */
   *p_n_outpts = 0;

/* Initiate point flag Eq. +1: Point shall     be an output point   */
/*                     Eq. -1: Point shall not be an output point   */
for (i_p =1; i_p <=n_inpts;   i_p = i_p +1)
  {
  pflag[i_p -1] = 1;
  }

#ifdef DEBUG
/* Check vector input length for Debug On                           */
   vec_length = SQRT(
   p_refvec->x_gm *  p_refvec->x_gm + 
   p_refvec->y_gm *  p_refvec->y_gm + 
   p_refvec->z_gm *  p_refvec->z_gm );

if ( vec_length < idpttol )
   {
   sprintf(errbuf,"refvec=0 %%varkon_sur_selsol");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif


/*!                                                                 */
/* 2. Analyse all points with respect to all criteria               */
/* __________________________________________________               */
/*                                                                 !*/

min_dist = 50000.0;      /* Initialise minimum distance             */
i_min    = I_UNDEF;      /* Point number to the minimum dist. pt    */
dot_n_min= F_UNDEF;      /* Scalar product for the min. dist. pt    */

for (i_p= 1; i_p<= n_inpts; i_p =i_p +1)  /* Start point loop       */
  {

/*!                                                                 */
/*    For scase= 2, 3, 4, 5 and 6:                                  */
/*    Let pflag(i_p)= -1 for solutions where the scalar product     */
/*    dot_s = (solution point - refpt) * refvec < 0.0               */
/*          !solution point - refpt! < idpttol ==> pflag(i_p)= -1   */
/*                                                                 !*/

      if ( scase == 1 ) goto crit2;

      r3dist(p_refpt,xyz_in,i_p );        /* Distance to refpt      */
      if ( dist < idpttol  )              /* ref_pt= solution pt    */
         {
         pflag[i_p -1] = -1;
         goto crit2;
         } 

      sdirec(p_refpt,p_refvec,xyz_in,i_p);/* Scalar product dot_s   */
      if ( dot_s < 0.0    )               /* Not same direction     */
         {
         pflag[i_p-1] = -1;
         } 

crit2:;                                   /* Label criterion 2      */

/*!                                                                 */
/*    For scase= 3:                                                 */
/*    Let pflag(i_p)= -1 for solutions where the scalar product     */
/*    dot_n = (surface normal) * refvec < 0.0                       */
/*    For scase= 4:                                                 */
/*    Let pflag(i_p)= -1 for solutions where the scalar product     */
/*    dot_n = (surface normal) * refvec > 0.0                       */
/*                                                                 !*/

      if    ( scase == 3 || scase == 4 || scase == 5 || scase == 6 )
        {
        ndirec(p_refvec,xyz_in,i_p);     /* Scalar product dot_n   */
        }

      if ( (scase == 3 || scase == 5) && dot_n < 0.0 ) 
         {
         pflag[i_p-1] = -1;
         } 
      if ( (scase == 4 || scase == 6) && dot_n > 0.0 ) 
         {
         pflag[i_p-1] = -1;
         } 

/*!                                                                 */
/*    For scase= 5 and 6:                                           */
/*    Find minimum distance point                                   */
/*                                                                 !*/

      if    ( scase == 5 || scase == 6 )
        {
        if  ( dist < min_dist && dist > idpttol && dot_s > 0.0 ) 
          {
          i_min = i_p;
          min_dist = dist;
          dot_n_min = dot_n;
          }
        }
  }                                       /* End   point loop       */


/*!                                                                 */
/* 3. Selected points to the output array                           */
/* _______________________________________                          */
/*                                                                 !*/

*p_n_outpts = 0;  

for (i_p= 1; i_p<= n_inpts; i_p =i_p +1)  /* Start point loop       */
  {
  
  if ( pflag[i_p-1] == 1  )               /* Output if pflag= +1    */
     {
     *p_n_outpts = *p_n_outpts + 1;
      xyz_out[*p_n_outpts-1] = xyz_in[i_p-1];
     }

  }                                       /* End   point loop       */

/*!                                                                 */
/* Let output number of points be zero for scase 5 and 6 if first   */
/* surface normal is in a direction .......                         */
/*                                                                 !*/

      if ( scase == 5  && dot_n_min < 0.0 ) *p_n_outpts = 0;  
      if ( scase == 6  && dot_n_min > 0.0 ) *p_n_outpts = 0;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984 Exit varkon_sur_selsol No output pts %d Used tolerance %f\n",
     *p_n_outpts , idpttol );
  }
fflush(dbgfil(SURPAC)); 
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
               static  short   r3dist (p_cp,xyz_in,i_p )
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_cp;       /* Comparison point                        */
   EVALS   xyz_in[];     /* All input  (solution) points            */
   DBint   i_p ;         /* Current point in array xyz_in           */
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
  (p_cp->x_gm-xyz_in[i_p  -1].r_x)*(p_cp->x_gm-xyz_in[i_p  -1].r_x) +
  (p_cp->y_gm-xyz_in[i_p  -1].r_y)*(p_cp->y_gm-xyz_in[i_p  -1].r_y) +
  (p_cp->z_gm-xyz_in[i_p  -1].r_z)*(p_cp->z_gm-xyz_in[i_p  -1].r_z) );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984*r3dist  dist= %f \n" , dist       );
  }
fflush(dbgfil(SURPAC)); 
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/

/*!************** Internal ** function ** ndirec  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate the scalar product:                                   */
/*  dot_n = (surface normal) * refvec                               */
/*                                                                  */
       static  short   ndirec (p_refvec,xyz_in,i_p)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_refvec;   /* Comparison vector (R*3 or U,V)    (ptr) */
   EVALS   xyz_in[];     /* All input  (solution) points            */
   DBint   i_p ;         /* Current point in array xyz_in           */
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
  "sur984*ndirec  Comparison vector %f %f %f\n",
     p_refvec->x_gm, p_refvec->y_gm, p_refvec->z_gm );
  fflush(dbgfil(SURPAC));
  }
#endif

    dot_n  = 
    xyz_in[i_p-1].n_x*p_refvec->x_gm +
    xyz_in[i_p-1].n_y*p_refvec->y_gm +
    xyz_in[i_p-1].n_z*p_refvec->z_gm;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984*ndirec dot_n= %f \n" , dot_n  );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


/*!************** Internal ** function ** sdirec  *******************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*  Calculate the scalar product:                                   */
/*  dot_s = (solution point - refpt) * refvec                       */
/*                                                                  */
       static  short   sdirec (p_refpt,p_refvec,xyz_in,i_p)
/*                                                                 !*/
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector *p_refpt;    /* Comparison point  (R*3 or U,V)    (ptr) */
   DBVector *p_refvec;   /* Comparison vector (R*3 or U,V)    (ptr) */
   EVALS   xyz_in[];     /* All input  (solution) points            */
   DBint   i_p;          /* Candidate point as solution             */
/*                                                                  */
/*                                                                  */
/* Out:                                                             */
/* Global (static) variable dot_s                                   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
 
/*----------------------------------------------------------------- */

#ifdef DEBUG
   DBfloat u_leng;       /* Length of vector 1                      */
   DBfloat v_leng;       /* Length of vector 2                      */
   DBfloat dot_s_n;      /* Scalar product normalised vectors       */
#endif

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984*sdirec  Candidate as solution point %f %f %f \n",
    xyz_in[i_p-1].r_x,xyz_in[i_p-1].r_y,xyz_in[i_p-1].r_z); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984*sdirec  i_p  = %d \n" , i_p );
  fflush(dbgfil(SURPAC)); 
  }
#endif


    dot_s  = 
    (xyz_in[i_p-1].r_x-p_refpt->x_gm)*p_refvec->x_gm +
    (xyz_in[i_p-1].r_y-p_refpt->y_gm)*p_refvec->y_gm +
    (xyz_in[i_p-1].r_z-p_refpt->z_gm)*p_refvec->z_gm;  

#ifdef DEBUG
  u_leng = SQRT(
  (xyz_in[i_p-1].r_x-p_refpt->x_gm)* (xyz_in[i_p-1].r_x-p_refpt->x_gm) +
  (xyz_in[i_p-1].r_y-p_refpt->y_gm)* (xyz_in[i_p-1].r_y-p_refpt->y_gm) +
  (xyz_in[i_p-1].r_z-p_refpt->z_gm)* (xyz_in[i_p-1].r_z-p_refpt->z_gm));
    v_leng = SQRT(
    p_refvec->x_gm * p_refvec->x_gm +
    p_refvec->y_gm * p_refvec->y_gm +
    p_refvec->z_gm * p_refvec->z_gm);
    if (u_leng > 0.000001 && v_leng > 0.000001 )
      {
      dot_s_n = dot_s/u_leng/v_leng;
      }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur984*sdirec  dot_s = %f dot_s_n = %f (norm. vectrs) i_p  = %d\n" , 
      dot_s, dot_s_n, i_p );
  fflush(dbgfil(SURPAC)); 
  }
#endif

       return(SUCCED);


} /* End of function */

/********************************************************************/


