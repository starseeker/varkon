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
/*  Function: varkon_cur_intplan                   File: sur720.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the intersect points between a curve    */
/*  and a plane.                                                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-01-23   Originally written                                 */
/*  1994-10-15   GEsort_1->sur982, GE109, comptol and ctol           */
/*               start and end segment parameter values   G Liden   */
/*  1995-03-07   UV_CUB_SEG (CUB_SEG)                                   */
/*  1996-01-12   fflush. Check INTMAX                               */
/*  1996-01-27   Debug                                              */
/*  1999-11-27   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_intplan    Curve/plane intersect            */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_ctol            * Coordinate tolerance                    */
/* varkon_cur_extidpoint  * Extend curve ctol parameters.           */
/* varkon_cur_num1        * Curve one parameter calculat.           */
/* GE109                  * Curve   evaluation routine              */
/* varkon_cur_order       * Ordering of U values                    */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_cur_intplan         Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_intplan (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBfloat  plane[4],     /* Intersect plane                         */
  DBint   *p_nint,       /* Number of intersect points              */
  DBfloat  u_out[INTMAX])/* Ordered array of u solutions 1,., nint  */

/* Out:                                                             */
/*        Number of intersects and ordered U values                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   i_seg;         /* Loop index segment in curve             */
  DBint   i_sol;         /* Loop index solutions in one segment     */
  short   noall;         /* The total number of intersects incl.    */
                         /* possible double points ( and before     */
                         /* ordering of the u values )              */
  DBfloat uglob[INTMAX]; /* The global u before ordering            */
  IPLANE  pladat;        /* Planar intersect computation data       */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint    c_case;       /* Computation case _cur_extidpoint        */
  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBfloat  ctol;         /* Coordinate tolerance                    */
  DBVector refpt;        /* Comparison point (R*3 or U)             */
  DBfloat  t_in[2];      /* Parameter curve segment limits          */
                         /* (0):   Start U    (1): End   U          */
  DBint    npts;         /* Number of output points                 */
  EVALC    xyz_a[SMAX];  /* Array with all solutions                */
  EVALC    xyz_in [100]; /* Array with all input  solutions         */
  EVALC    xyz_out[100]; /* Array with all output solutions         */
  DBfloat   t_start;     /* Start parameter for intersect calcul.   */
  DBfloat   t_end;       /* End   parameter for intersect calcul.   */
  DBint    ocase_l;      /* Ordering case                           */
  DBint    n_outpts;     /* Number of output points                 */
  DBint    status;       /* Error code from a called function       */
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
"sur720 Enter varkon_cur_intplan: Curve/plane intersect Plane d= %f\n",
             plane[3]);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initiations                          */
/*  ______________________________________                          */
/*                                                                  */
/* Surface computer accuracy and coordinate tolerance.              */
/* Call of varkon_comptol (sur753) and varkon_ctol (sur751).        */
/*                                                                 !*/

   comptol=varkon_comptol();
   ctol   =varkon_ctol();

/*!                                                                 */
/* Computation case rtype and plane to structure variable pladat.   */
/* Selected method (NEWTON), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable pladat.               */
/* Let the criterion for identical points be pladat.ctol= 0.4*ctol  */
/*                                                                 !*/


pladat.ctype   = CURPLAN;
pladat.method  = NEWTON;
pladat.nstart  = 3;      
pladat.maxiter = 20;      

pladat.in_x    = plane[0];
pladat.in_y    = plane[1];
pladat.in_z    = plane[2];
pladat.in_d    = plane[3];

pladat.ctol    = 0.4*ctol;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur720 pladat.ctol %f ctol %f .2*TOL2 %f .4*ctol %f\n", 
        pladat.ctol ,  ctol ,  0.2*TOL2 , 0.4*ctol );
fflush(dbgfil(SURPAC));
}
#endif

/* Initiate loop variables                                          */
    noall  = 0;
    *p_nint = 0;

/*!                                                                 */
/* 2. Parameter values for start and end of curve                   */
/* ______________________________________________                   */
/*                                                                  */
/* Calculate point and tangent at ends of curve.                    */
/* Call of varkon_cur_extidpoint (sur787).                          */
/*                                                                 !*/

c_case = 1;

   status=varkon_cur_extidpoint
   (p_cur,p_seg,c_case,&t_start,&t_end);
   if (status<0) 
        {
        sprintf(errbuf,"sur787%%varkon_sur_intplan");
        return(erpush("SU2943",errbuf));
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur720 Extend curve with ctol: t_start %f t_end %f ns_cu %d\n",
                t_start, t_end, p_cur->ns_cu );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  2. Calculate intersection points                                */
/*  ________________________________                                */
/*                                                                  */
/*  Start loop all curve segments i_seg= 1,2,..., p_cur->ns_cu      */
/*                                                                 !*/

    for ( i_seg = 1 ; i_seg <= p_cur->ns_cu ; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Calculate intersect points in segment i_seg.                  */
/*    Call of varkon_cur_num1 (sur901).                             */
/*                                                                 !*/

      if      ( i_seg == 1 && p_seg->typ == CUB_SEG )
          {
          t_in[0] = t_start;
          t_in[1] = (DBfloat)i_seg + 1.0 - comptol;
          }
      else if ( i_seg == p_cur->ns_cu && (p_seg+i_seg-1)->typ == CUB_SEG )
          {
          t_in[0] = (DBfloat)i_seg       + comptol;
          t_in[1] = t_end;
          }
      else
          {
          t_in[0] = (DBfloat)i_seg       + comptol;
          t_in[1] = (DBfloat)i_seg + 1.0 - comptol;
          }

      status = varkon_cur_num1
     (p_cur,p_seg,t_in,(IRUNON*)&pladat,&npts,xyz_a);
      if (status<0) 
        {
        sprintf(errbuf,"geo901%%varkon_cur_intplan (sur720)");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/*    Add global U values to array uglob and increase the           */
/*    total number of intersect points noall.                       */
/*                                                                 !*/

      if ( noall + (short)npts > INTMAX )
        {
        sprintf(errbuf,"Increase INTMAX%%varkon_cur_intplan");
        return(varkon_erpush("SU2993",errbuf));
        }



      for ( i_sol = noall; i_sol <= noall+npts-1; i_sol++ )
          {
          uglob[i_sol] = xyz_a[i_sol-noall].u_global;
          xyz_in[i_sol]= xyz_a[i_sol-noall];
/*  Temporary until replaced in fctns  GE109, geo110, ... ? */
          xyz_in[i_sol].t_global= xyz_a[i_sol-noall].u_global;
          }
        
      noall = noall + (short)npts;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur720 i_seg  %d npts %d noall %d\n", i_seg , npts   , noall );
fflush(dbgfil(SURPAC)); 
}
#endif

      }  /* End loop i_seg  */

/*!                                                                 */
/*  End   loop all curve segments i_seg= 1,2,..., p_cur->ns_cu      */
/*                                                                 !*/

/*!                                                                 */
/*  3. Ordered output U values                                      */
/*  __________________________                                      */
/*                                                                  */
/*  Ordering of uglob and elimination of identical solutions.       */
/*  Call of varkon_cur_order (sur982)                               */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
for ( i_sol = 1; i_sol <= noall; i_sol++ )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur720 i_sol %d i_sol uglob(i_sol-1) %f\n", 
      i_sol , uglob[i_sol-1] );
    fflush(dbgfil(SURPAC)); 
  }
}
#endif


   if ( noall > 1 )
     {

     refpt.x_gm = 1.0;            
     refpt.y_gm = 0.0;              
     refpt.z_gm = 0.0;

     ocase_l    = 2;

status=varkon_cur_order
      (&refpt, xyz_in, (DBint)noall, ocase_l, xyz_out, &n_outpts);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_cur_order%%varkon_cur_intplan");
        return(varkon_erpush("SU2943",errbuf));
        }
     *p_nint = n_outpts;
     for ( i_sol = 1; i_sol <= n_outpts; i_sol++ )
       {
       u_out[i_sol-1] = xyz_out[i_sol-1].u_global;
       }
     }




    if ( noall == 1 ) 
     {
     *p_nint = 1;
     u_out[0] = uglob[0];
     }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur720 Exit varkon_cur_intplan *p_nint %d \n", *p_nint );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
