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
/*  Function: varkon_sur_nurbsnon                  File: sur892.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculate number of nodes from the the number of knot values    */
/*  and the order of the B-spline surface.                          */
/*                                                                  */
/*  Let input V values be equal to I_UNDEF for a NURBS curve.       */
/*                                                                  */
/*  The number of node points, the degree of the basis functions    */
/*  and the number of values in the knot vector shall be related    */
/*  in the following way:                                           */
/*                                                                  */
/*  m = n + p + 1 (in the U and the V direction)                    */
/*                                                                  */
/*  m is the index for the knot vector values u0, u1, .. ,un        */
/*  (i.e. the number of knot values is m+1).                        */
/*  p is the degree of the basis functions                          */
/*  n is the index for the nodes (number of nodes is n+1)           */
/*                                                                  */
/*  Remark 1:                                                       */
/*  ---------                                                       */
/*                                                                  */
/*  Note that in the evaluation functions are the maximum indeces   */
/*  for the arrays used, but the output from this function is the   */
/*  number of nodes:                                                */
/*                                                                  */
/*  m_u = np_u - 1                                                  */
/*  m_v = np_v - 1                                                  */
/*                                                                  */
/*                                                                  */
/*  Remark 2:                                                       */
/*  ---------                                                       */
/*                                                                  */
/*  This is a very simple calculation, but it is easy to make       */
/*  mistakes and in the future will this calculation be made in     */
/*  many functions, since only the number of knot values are        */
/*  stored for a B-spline surface (patch).                          */
/*                                                                  */
/*  An alternative (maybe better to reduce calculation time) is     */
/*  to store also the number of nodes.                              */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-02-15   Originally written                                 */
/*  1999-12-04   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nurbsnon   Calculate number of NURBS nodes  */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */

/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erinit          * Initial. of error messages              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_nurbsnon  */
/* SU2993 = Severe program error in varkon_sur_nurbsnon(sur892).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/

      DBstatus        varkon_sur_nurbsnon (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint  order_u,       /* Order  of U basis functions             */
   DBint  order_v,       /* Order  of V basis functions             */
   DBint  nk_u,          /* Number of U knot values                 */
   DBint  nk_v,          /* Number of V knot values                 */
   DBint *p_np_u,        /* Number of U control pts (nodes)   (ptr) */
   DBint *p_np_v,        /* Number of V control pts (nodes)   (ptr) */
   DBint *p_degree_u,    /* Degree of U basis functions       (ptr) */
   DBint *p_degree_v )   /* Degree of V basis functions       (ptr) */

/* Out:                                                             */
/*       Data to p_np_u, p_np_v, p_degree_u and p_degree_v          */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur892 Enter**varkon_sur_nurbsnon nk_u %d  nk_v %d\n",
                (int)nk_u,(int)nk_v);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 1. Checks and initializations of output data for Debug On        */
/* _________________________________________________________        */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG                  
    *p_np_u     = I_UNDEF;
    *p_np_v     = I_UNDEF;
    *p_degree_u = I_UNDEF;
    *p_degree_v = I_UNDEF;
    /* TODO Add checks .................  */
#endif

/*!                                                                 */
/* 2. Calculate number of nodes and degree.                         */
/* ________________________________________                         */
/*                                                                  */
/*                                                                 !*/

    *p_np_u     = nk_u - order_u;
    if ( nk_v != I_UNDEF ) *p_np_v     = nk_v - order_v;
    else                   *p_np_v     = I_UNDEF;

    *p_degree_u = order_u - 1;
    if ( nk_v != I_UNDEF ) *p_degree_v = order_v - 1;
    else                   *p_degree_v = I_UNDEF;


  return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

