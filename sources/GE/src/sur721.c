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
/*  Function: varkon_cur_intbplan                  File: sur721.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the intersect points between a curve    */
/*  and a B-plane.                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-12   Originally written                                 */
/*  1996-01-27   Debug                                              */
/*  1999-11-28   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_intbplan   Curve/B-plane intersect          */
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
/* varkon_idpoint         * Identical point criterion               */
/* varkon_cur_intplan     * Intersect curve/plane                   */
/* GE109                  * Curve evaluation function               */
/* varkon_bpl_barycen     * Barycentric coordinates                 */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_cur_intbplan        Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_intbplan (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve  *p_cur,       /* Current curve                     (ptr) */
  DBSeg    *p_seg,       /* Coefficients for curve segments   (ptr) */
  DBBplane *p_bpl,       /* B-plane                           (ptr) */
  DBint    *p_nint,      /* Number of intersect points              */
  DBfloat  u_out[INTMAX],/* Ordered array of u solutions 1,., nint  */
  DBVector r_out[INTMAX])/* Ordered array of R*3 solutions          */

/* Out:                                                             */
/*        Number of intersects and ordered U values                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat    plane[4];     /* Intersect plane                         */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector p1;           /* B-plane corner point 1                  */
  DBVector p2;           /* B-plane corner point 2                  */
  DBVector p3;           /* B-plane corner point 3                  */
  DBVector p4;           /* B-plane corner point 4                  */
  DBVector v_b;          /* B-plane normal                          */
  DBfloat  d_b;          /* B-plane D value                         */
  DBfloat  length;       /* RLength of plane normal                 */

  DBfloat  u_all[INTMAX];/* Ordered array of u solutions 1,., n_all */
  DBint    n_all;        /* Number of unbounded plane solutions     */

  DBint    i_poi;        /* Loop index solution point               */
  DBVector p_fac;        /* Intersect point in B-plane              */
  EVALC    xyz;          /* Coordinates and derivatives for curve   */

  DBfloat  alpha;        /* Barycentric coordinate for p1           */
  DBfloat  beta;         /* Barycentric coordinate for p2           */
  DBfloat  gamma;        /* Barycentric coordinate for p4           */

  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBfloat  ctol;         /* Coordinate tolerance                    */
  DBint    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur721 Enter varkon_cur_intbplan: Curve/B-plane intersect \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/* Surface computer accuracy and coordinate tolerance.              */
/* Call of varkon_comptol (sur753) and varkon_ctol (sur751).        */
/*                                                                 !*/

   comptol=varkon_comptol();
   ctol   =varkon_ctol();

/* Initialize output variables                                      */
    
    *p_nint = 0;

/*!                                                                 */
/* 2. Calculate all the intersects                                  */
/* ________________________________                                 */
/*                                                                  */
/* Calculate all intersects with the unbounded plane.               */
/* Call of varkon_cur_intplan (sur720).                             */
/*                                                                 !*/

/* Corner points to local variables                                 */

   p1 = p_bpl->crd1_bp;                  /* B-plane corner 1        */
   p2 = p_bpl->crd2_bp;                  /* B-plane corner 2        */
   p3 = p_bpl->crd3_bp;                  /* B-plane corner 3        */
   p4 = p_bpl->crd4_bp;                  /* B-plane corner 4        */


/*!                                                                 */
/* Input plane normal and D value                                   */
/*                                                                 !*/

   v_b.x_gm =  ( p2.y_gm- p1.y_gm) * ( p4.z_gm- p1.z_gm) - 
               ( p2.z_gm- p1.z_gm) * ( p4.y_gm- p1.y_gm);  
   v_b.y_gm =  ( p2.z_gm- p1.z_gm) * ( p4.x_gm- p1.x_gm) - 
               ( p2.x_gm- p1.x_gm) * ( p4.z_gm- p1.z_gm);  
   v_b.z_gm =  ( p2.x_gm- p1.x_gm) * ( p4.y_gm- p1.y_gm) - 
               ( p2.y_gm- p1.y_gm) * ( p4.x_gm- p1.x_gm);  

   length    =  SQRT( v_b.x_gm*v_b.x_gm + 
                      v_b.y_gm*v_b.y_gm + 
                      v_b.z_gm*v_b.z_gm );  

   if ( length < 0.000001 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur674  Error exit Input plane normal v_b length %25.10g\n", length);
fflush (dbgfil(SURPAC)); 
}
#endif
     sprintf(errbuf,"(v_b zero)%%varkon_cur_intbplan (sur721)");
     return(varkon_erpush("SU2993",errbuf));
     }

   v_b.x_gm = v_b.x_gm / length;
   v_b.y_gm = v_b.y_gm / length;
   v_b.z_gm = v_b.z_gm / length;

   d_b =  p1.x_gm*v_b.x_gm +  p1.y_gm*v_b.y_gm +  p1.z_gm*v_b.z_gm;  


   plane[0] = v_b.x_gm;
   plane[1] = v_b.y_gm;
   plane[2] = v_b.z_gm;
   plane[3] = d_b;


   status=varkon_cur_intplan (p_cur,p_seg,plane,&n_all,u_all);
   if (status<0) 
        {
        sprintf(errbuf,"sur720%%varkon_sur_intbplan");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*  2. Keep only the points within the B-plane polygon              */
/*  __________________________________________________              */
/*                                                                  */
/*  Start loop all intersect points i_p= 1,2,...,                   */
/*                                                                 !*/

    *p_nint = 0;            

    for ( i_poi = 1 ; i_poi <= n_all; i_poi = i_poi + 1 )
      {
/*!                                                                 */
/*    Calculate the barycentric coordinates.                        */
/*    Call of varkon_bpl_barycen (sur676).                          */
/*                                                                 !*/

/*    Calculate point on the curve                                  */
/*    Call of varkon_GE109 (GE109).                               */

        xyz.evltyp   = EVC_R;
        xyz.t_global = u_all[i_poi-1];

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_intbplan (sur721)");
        return(varkon_erpush("SU2943",errbuf));
        }

          p_fac = xyz.r;

/* TODO B-plane should be extended with idpoint.  !! */
/*      Check tolerances in varkon_bpl_barycen (sur676) !! */

status=varkon_bpl_barycen
       ( p_bpl, &p_fac , &alpha, &beta, &gamma );
       if ( status < 0  )
         {
         ;
         }
       else
         {
         u_out[*p_nint] = u_all[i_poi-1];
         r_out[*p_nint] = p_fac;
         *p_nint = *p_nint + 1;
         }
     }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur721 Exit varkon_cur_intbplan Curve/B-plane intersect *p_nint %d \n", 
             *p_nint );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
