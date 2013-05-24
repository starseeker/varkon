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
/*  Function: varkon_bpl_on                        File: sur709.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates a point on a B-plane.                   */
/*  The function corresponds to MBS statement ON for a B-plane.     */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-09-26   Originally written                                 */
/*  1998-09-28   Bug in interpolation                               */
/*  1999-12-05   Free source code modifications                     */
/*  2004-07-26   Trimmed, J.Kjellander, Örebro university           */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_bpl_on         Point on B-plane for U,V         */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_bpl_analyse     * Analyse (classify) B-plane              */
/* varkon_lin_linlind     * Closest point for line/line             */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_bpl_on              Report! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_bpl_on (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBBplane *p_bplin,     /* Input  B-plane                    (ptr) */
  DBfloat   u_b,         /* U value on B-plane                      */
  DBfloat   v_b,         /* V value on B-plane                      */
  DBVector *p_point )    /* Output point                      (ptr) */

/* Out:                                                             */
/*        Coordinates in p_point                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector p1;           /* B-plane corner point 1                  */
  DBVector p2;           /* B-plane corner point 2                  */
  DBVector p3;           /* B-plane corner point 3                  */
  DBVector p4;           /* B-plane corner point 4                  */
  DBint    btype;        /* Type of B-plane polygon                 */
                         /* Type of polygon                         */
                         /* Eq.  1: Triangle                        */
                         /* Eq.  2: Convex 4-point polygon          */
                         /* Eq.  3: Non-convex polygon              */
                         /* Eq.  4: p4, p1, p2 are colinear         */
                         /* Eq.  5: p1, p2, p3 are colinear         */
                         /* Eq.  6: p2, p3, p4 are colinear         */
                         /* Eq.  7: p3, p4, p1 are colinear         */
                         /* Eq. -1: p1 = p2                         */
                         /* Eq. -2: p3 = p4                         */
                         /* Eq. -3: p1 = p4                         */
                         /* Eq. -4: p1, p2, p3, p4 not coplanar     */
                         /* Eq. -5: Self-intersecting polygon       */
  DBint    pnr;          /* Point number for:                       */
                         /* Non-convex point   (p_btype= 3)         */
                         /* Colinear mid point (p_btype= 4,5,6,7)   */

  DBVector pl_1;         /* Start point      for line 1             */
  DBVector v_1;          /* Direction vector for line 1             */
  DBVector pl_2;         /* Start point      for line 2             */
  DBVector v_2;          /* Direction vector for line 2             */
  DBVector clop_1;       /* Nearest point    on  line 1             */
  DBVector clop_2;       /* Nearest point    on  line 2             */
  DBfloat  t_1;          /* Parameter for line 1 (unit vector)      */
  DBfloat  t_2;          /* Parameter for line 1 (unit vector)      */
  DBfloat  dist;         /* Distance                                */

  DBfloat  idpoint;      /* Identical points criterion              */
  DBfloat  comptol;      /* Computer   tolerance                    */

  short    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Retrive computer tolerance. Call of varkon_comptol (sur753)      */
/* Retrive identical pts crit. Call of varkon_idpoint (sur741)      */
/*                                                                 !*/

   comptol   = varkon_comptol();
   idpoint   = varkon_idpoint();

/*!                                                                 */
/* Check input U,V point.                                           */
/*                                                                 !*/

  if   (  u_b < -comptol || u_b > 1.0 + comptol ||
          v_b < -comptol || u_b > 1.0 + comptol    )
   {
   varkon_erinit();
   sprintf(errbuf,"%f,%f%%sur709",u_b,v_b);
   return(varkon_erpush("SU1293",errbuf));
   }

/*!                                                                 */
/* Corner points to local variables p1, p2, p3 and p4               */
/*                                                                 !*/

   p1 = p_bplin->crd1_bp;                /* B-plane corner 1        */
   p2 = p_bplin->crd2_bp;                /* B-plane corner 2        */
   p3 = p_bplin->crd3_bp;                /* B-plane corner 3        */
   p4 = p_bplin->crd4_bp;                /* B-plane corner 4        */

/*!                                                                 */
/* 2. Determine type of (classify) input B-plane (polygon)          */
/* _______________________________________________________          */
/*                                                                  */
/* Classify) input polygon. Call of varkon_bpl_analyse (sur718)     */
/* Cases with three points on one line not implemented              */
/*                                                                 !*/

   varkon_bpl_analyse ( &p1, &p2, &p3, &p4, &btype, &pnr );

/* Exit with error if B-plane not is OK                             */

   if ( btype < 0  )
     {
     sprintf(errbuf,"%d %%sur709",(int)btype);
     return(varkon_erpush("SU1273",errbuf));
     }

/* Cases with three points on one line not implemented              */

   if (btype == 4 || btype == 5 || btype == 6 || btype == 7 || 
       btype == 3                                              )
     {
     varkon_erinit();
     if         ( 4 == btype ) sprintf(errbuf,"p4-p1-p2 %%sur709");
     else if    ( 5 == btype ) sprintf(errbuf,"p1-p2-p3 %%sur709");
     else if    ( 6 == btype ) sprintf(errbuf,"p2-p3-p4 %%sur709");
     else if    ( 7 == btype ) sprintf(errbuf,"p3-p4-p1 %%sur709");
     else if    ( 3 == btype ) sprintf(errbuf,"%d %%sur709",(int)pnr);
     else                      sprintf(errbuf,"p?-p?-p? %%sur709");
     if  ( 3 == btype ) return(varkon_erpush("SU1283",errbuf));
     else               return(varkon_erpush("SU1263",errbuf));
     }


/*!                                                                 */
/* 3. Calculate point                                               */
/* __________________                                               */
/*                                                                 !*/


/*!                                                                 */
/* Case triangle or convex 4-sided B-plane                          */
/*                                                                 !*/

   if (  2 == btype || 1 == btype )
     {
     pl_1.x_gm =  p1.x_gm  + u_b*(p2.x_gm-p1.x_gm);
     pl_1.y_gm =  p1.y_gm  + u_b*(p2.y_gm-p1.y_gm);
     pl_1.z_gm =  p1.z_gm  + u_b*(p2.z_gm-p1.z_gm);
     v_1.x_gm  =  p4.x_gm  + u_b*(p3.x_gm-p4.x_gm);
     v_1.y_gm  =  p4.y_gm  + u_b*(p3.y_gm-p4.y_gm);
     v_1.z_gm  =  p4.z_gm  + u_b*(p3.z_gm-p4.z_gm);
     v_1.x_gm  =  v_1.x_gm - pl_1.x_gm;
     v_1.y_gm  =  v_1.y_gm - pl_1.y_gm;
     v_1.z_gm  =  v_1.z_gm - pl_1.z_gm;

     pl_2.x_gm =  p1.x_gm  + v_b*(p4.x_gm-p1.x_gm);
     pl_2.y_gm =  p1.y_gm  + v_b*(p4.y_gm-p1.y_gm);
     pl_2.z_gm =  p1.z_gm  + v_b*(p4.z_gm-p1.z_gm);
     v_2.x_gm  =  p2.x_gm  + v_b*(p3.x_gm-p2.x_gm);
     v_2.y_gm  =  p2.y_gm  + v_b*(p3.y_gm-p2.y_gm);
     v_2.z_gm  =  p2.z_gm  + v_b*(p3.z_gm-p2.z_gm);
     v_2.x_gm  =  v_2.x_gm - pl_2.x_gm;
     v_2.y_gm  =  v_2.y_gm - pl_2.y_gm;
     v_2.z_gm  =  v_2.z_gm - pl_2.z_gm;
     }

   else
     {
     sprintf(errbuf,"btype %%sur709");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* Calculate intersect between the lines. For U close to 1.0        */
/* will the vector v_1 be close to zero. No closest point           */
/* calculation in this case.                                        */
/*                                                                 !*/

/* Check if (triangle and) vector v_1 is zero                       */

   if (sqrt(v_1.x_gm*v_1.x_gm + v_1.y_gm*v_1.y_gm + v_1.z_gm*v_1.z_gm) 
              < idpoint )
     {
     p_point->x_gm = pl_1.x_gm + v_b*v_1.x_gm;
     p_point->y_gm = pl_1.y_gm + v_b*v_1.y_gm;
     p_point->z_gm = pl_1.z_gm + v_b*v_1.z_gm;
     goto  top_pt;
     }

   status = varkon_lin_linlind   
    (pl_1, v_1, pl_2, v_2, &clop_1, &clop_2, &t_1,&t_2,&dist );

   if ( status < 0 )
     {
     sprintf(errbuf,"sur710%%sur709");
     return(varkon_erpush("SU2943",errbuf));
    }
/*!                                                                 */
/* Output point                                                     */
/*                                                                 !*/

  p_point->x_gm = clop_1.x_gm;
  p_point->y_gm = clop_1.y_gm;
  p_point->z_gm = clop_1.z_gm;

top_pt:; /* Label: Top point of triangle                            */


    return(SUCCED);

} /* End of function */


/********************************************************************/
