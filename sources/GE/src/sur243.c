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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_poi_nurbs                     File: sur243.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Set the pointer to a NURB_SUR node point in the memory area     */
/*  The function also have options for initialization of values     */
/*  and printout of all node points to the Debug File.              */
/*                                                                  */
/*  Input to the function is the set_pointer/initialize/ print case */
/*  and an integer address (index) for the node point.              */
/*                                                                  */
/*  An address to a node point is defined as:                       */
/*                                                                  */
/*  P          where i_up= 0,1..., m_u and i_vp= 0,1,... m_v        */
/*   i_up,i_vp                                                      */
/*                                                                  */
/*  The number of polygon points in the U direction is m_u + 1      */
/*  and in the V direction m_v + 1 corresponding to the members     */
/*  nk_u and nk_v of the input structure variable PATNU.            */
/*                                                                  */
/*  The node is a point with homogenous coordinates.                */
/*                                                                  */
/*  The order of the polygon points in the memory area is:          */
/*                                                                  */
/*  V   0,0   0,1   0,2   0,3   0,4  0,5  0,6  0,7    U--->         */
/*  !    0     8    16    24    32   40   48   56                   */
/*  !                                                               */
/*  v                                                               */
/*      1,0   1,1   1,2   1,3   1,4  1,5  1,6  1,7                  */
/*       1     9    17    25    33   41   49   57                   */
/*                                                                  */
/*                                                                  */
/*      2,0   2,1   2,2   2,3   2,4  2,5  2,6  2,7                  */
/*       2    10    18    26    34   42   50   58                   */
/*                                                                  */
/*                                                                  */
/*      3,0   3,1   3,2   3,3   3,4  3,5  3,6  3,7                  */
/*       3    11    19    27    35   43   51   59                   */
/*                                                                  */
/*                                                                  */
/*      4,0   4,1   4,2   4,3   4,4  4,5  4,6  4,7                  */
/*       4    12    20    28    36   44   52   60                   */
/*                                                                  */
/*                                                                  */
/*      5,0   5,1   5,2   5,3   5,4  5,5  5,6  5,7                  */
/*       5    13    21    29    37   45   53   61                   */
/*                                                                  */
/*                                                                  */
/*      6,0   6,1   6,2   6,3   6,4  6,5  6,6  6,7                  */
/*       6    14    22    30    38   46   54   62                   */
/*                                                                  */
/*                                                                  */
/*      7,0   7,1   7,2   7,3   7,4  7,5  7,6  7,7                  */
/*       7    15    23    31    39   47   55   63                   */
/*                                                                  */
/*                                                                  */
/*  The knot values and the degree (the order) of the splines       */
/*  can as an option be written to the Degug File. Please note      */
/*  the following (check the following):                            */
/*                                                                  */
/*  The number of polygon points, the degree of the basis functions */
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
/*                                                                  */
/*  The knot vector shall start and end with a number of "extra"    */
/*  (compared to the number of nodes) values. These values are      */
/*  normally identical in order to make the B-spline start in the   */
/*  first node. The number of extra values is defined by the degree */
/*  of the B-spline basis functions.  For a third degree spline     */
/*  the number of identical values is four (4).                     */
/*                                                                  */
/*  There may also be identical knot values. For instance the       */
/*  SUR_APPROX function (sur174) converts Bezier polygon points     */
/*  to NURBS nodes with multiplicity two (2), i.e four identical    */
/*  knot values and two (identical) node points.                    */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-11-27   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_poi_nurbs      Set pointer to a NURBS node      */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short setvalue();      /* Initialize values in memory area   */
static short printval();      /* Printout of nodes to Debug file    */

/*                                                                  */
#ifdef DEBUG
static short initial();       /* Checks of input data               */
#endif
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */

/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erinit          * Initial. of error messages    */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_poi_nurbs     */
/* SU2993 = Severe program error in varkon_poi_nurbs   (sur243).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus  varkon_poi_nurbs   (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATNU *p_patnu,     /* NURBS patch                       (ptr) */
   DBint    rwcase,      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
   DBint    i_up,        /* Point address in U direction            */
   DBint    i_vp,        /* Point address in V direction            */
   DBHvector  *p_pp )    /* Polygon point                     (ptr) */
/* Out:                                                             */
/*       Data to or from p_pp                                       */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  order_u;       /* Order  for surface in U direction       */
   DBint  order_v;       /* Order  for surface in V direction       */
   DBint  deg_u;         /* Degree for surface in U direction       */
   DBint  deg_v;         /* Degree for surface in V direction       */
   DBint  nk_u;          /* Number of nodes    in U direction       */
   DBint  nk_v;          /* Number of nodes    in V direction       */
   DBint  m_u;           /* Maximum node index value in U direction */
   DBint  m_v;           /* Maximum node index value in V direction */
   DBint  c_iuv;         /* Index to node in memory area            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBHvector *p_pp_l;    /* Polygon point                     (ptr) */
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur243 Enter**varkon_poi_nurbs p_patnu %d rwcase %d\n",
                (int)p_patnu,(int)rwcase);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations for Debug On          */
/* ________________________________________________________         */
/*                                                                  */
/*  Initialize output coordinates and derivatives and internal      */
/*  variables for DEBUG on.                                         */
/*  Call of initial.                                                */
/*                                                                 !*/

#ifdef DEBUG
   status=initial(p_patnu, rwcase, i_up, i_vp, p_pp ); 
   if (status<0) 
   {
   sprintf(errbuf,"initial%%sur243");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* 2. Initialize values in memory area for rwcase= 2                */
/* _________________________________________________                */
/*                                                                  */
/*  Initialization of values is made in internal function setvalue. */
/*                                                                 !*/

   if ( rwcase == 2 ) 
     {
     status= setvalue (p_patnu); 
     if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur243 setvalue failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
      sprintf(errbuf,"setvalue%%sur243");
      return(varkon_erpush("SU2973",errbuf));
      }
     return(SUCCED);
     }

/*!                                                                 */
/* 3. Printout of nodes for rwcase = 3                              */
/* ____________________________________                             */
/*                                                                  */
/*  Printout of values is made in internal function printval.       */
/*                                                                 !*/

   if ( rwcase == 3 ) 
     {
     status= printval (p_patnu); 
     if (status<0) 
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur243 printval failed\n" );
fflush(dbgfil(SURPAC));
}
#endif
      sprintf(errbuf,"printval%%sur243");
      return(varkon_erpush("SU2973",errbuf));
      }
     return(SUCCED);
     }


/*!                                                                 */
/* 4. Calculate address to node point in memory area.               */
/* __________________________________________________               */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get number of knot values                                        */
/*                                                                 !*/

    nk_u    = p_patnu->nk_u;
    nk_v    = p_patnu->nk_v;

/*!                                                                 */
/* Get the order (degree) of the basis functions.                   */
/*                                                                 !*/

    order_u = p_patnu->order_u;
    order_v = p_patnu->order_v;
    deg_u   = order_u - 1;
    deg_v   = order_v - 1;

/*!                                                                 */
/* Calculate the number of nodes                                    */
/*                                                                 !*/

    m_u     = nk_u - 1 - deg_u - 1;
    m_v     = nk_v - 1 - deg_v - 1;


/*!                                                                 */
/* Calculate the index c_iuv to the node in memory area             */
/*                                                                 !*/

   c_iuv = i_up*(m_v+1) + i_vp;

/*!                                                                 */
/* Read or write node                                               */
/*                                                                 !*/

    p_pp_l = p_patnu->cpts + c_iuv;

    if  ( rwcase == 0 )
      {
      p_pp->x_gm   = p_pp_l->x_gm;
      p_pp->y_gm   = p_pp_l->y_gm;
      p_pp->z_gm   = p_pp_l->z_gm;
      p_pp->w_gm   = p_pp_l->w_gm;
      }
    else if  ( rwcase == 1 )
      {
      p_pp_l->x_gm = p_pp->x_gm;
      p_pp_l->y_gm = p_pp->y_gm;
      p_pp_l->z_gm = p_pp->z_gm;
      p_pp_l->w_gm = p_pp->w_gm;
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur243 Exit rwcase %d i_up %d i_vp %d c_iuv %d\n",
         (int)rwcase , (int)i_up  , (int)i_vp  , (int)c_iuv );
  fflush(dbgfil(SURPAC));
}
#endif

  return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef DEBUG        
/*!********* Internal ** function **Defined*Only*For*Debug*On********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF, I_UNDEF and NULL.          */

   static short initial(p_patnu,rwcase, i_up, i_vp, p_pp )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATNU *p_patnu;     /* NURBS patch                       (ptr) */
   DBint    rwcase;      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
   DBint    i_up;        /* Point address in U direction            */
   DBint    i_vp;        /* Point address in V direction            */
   DBHvector  *p_pp;     /* Polygon point                     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  order_u;       /* Order  for surface in U direction       */
   DBint  order_v;       /* Order  for surface in V direction       */
   DBint  deg_u;         /* Degree for surface in U direction       */
   DBint  deg_v;         /* Degree for surface in V direction       */
   DBint  nk_u;          /* Number of nodes    in U direction       */
   DBint  nk_v;          /* Number of nodes    in V direction       */
   DBint  m_u;           /* Maximum node index value in U direction */
   DBint  m_v;           /* Maximum node index value in V direction */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur243*initial Enter\n");
  }

/*!                                                                 */
/* Check case                                                       */
/*                                                                 !*/

   if  ( rwcase == 0 || rwcase == 1 || rwcase == 2 || rwcase == 3)
     {
     }
   else
     {
     sprintf(errbuf,"rwcase not 0,1,2,3%%sur243*initial");
     return(varkon_erpush("SU2993",errbuf));
     }
    
/*!                                                                 */
/* Initialize output data                                           */
/*                                                                 !*/

/* Internal varibles                                                */
   order_u = I_UNDEF;
   order_v = I_UNDEF;
   nk_u    = I_UNDEF;  
   nk_v    = I_UNDEF; 
   deg_u   = I_UNDEF;
   deg_v   = I_UNDEF;

/*!                                                                 */
/*  Get number of knot values and order (degree) of surface         */
/*                                                                 !*/

/*  NURBS: Order and degree in U and V direction                    */
    order_u = p_patnu->order_u;
    order_v = p_patnu->order_v;
    deg_u   = order_u - 1;
    deg_v   = order_v - 1;

/*  NURBS: Number of knot values in U and V direction               */
    nk_u    = p_patnu->nk_u;
    nk_v    = p_patnu->nk_v;

/*!                                                                 */
/*  The number of nodes is defined by the relation:                 */
/*  m = n + p + 1    n = m - p - 1                                  */
/*                                                                 !*/

    m_u     = nk_u - 1 - deg_u - 1;
    m_v     = nk_v - 1 - deg_v - 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur243*initial The number of knot values in U is %d and V is %d\n",
         (int)nk_u,   (int)nk_v );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur243*initial The number of nodes in U is %d and V is %d\n",
         (int)m_u+1,   (int)m_v+1 );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*  Check the degree of the basis functions                         */
/*                                                                 !*/

    if  (  deg_u >= MAX_NURBD || deg_v >= MAX_NURBD )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur243*initial deg_u %d or deg_v %d >= MAX_NURBD %d\n",
         (int)deg_u, (int)deg_v, MAX_NURBD );
fflush(dbgfil(SURPAC));
}
#endif
      sprintf(errbuf,"MAX_NURBD%%sur243*initial");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/*  Check point address for wcase set pointer                       */
/*                                                                 !*/


   if  ( rwcase == 1 )
     {
     if  (  i_up < 0 || i_up > m_u )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur243*initial i_up  %d not between 0 and m_u= %d\n",
         (int)i_up , (int)m_u );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"i_up not OK%%sur243*initial");
       return(varkon_erpush("SU2993",errbuf));
       }
     if  (  i_vp < 0 || i_vp > m_v )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur243*initial i_vp  %d not between 0 and m_v= %d\n",
         (int)i_vp , (int)m_v );
fflush(dbgfil(SURPAC));
}
#endif
       sprintf(errbuf,"i_vp not OK%%sur243*initial");
       return(varkon_erpush("SU2993",errbuf));
       }

     }


   if  ( rwcase != 1 )
     {
     p_pp->x_gm = F_UNDEF;
     p_pp->y_gm = F_UNDEF;
     p_pp->z_gm = F_UNDEF;
     p_pp->w_gm = F_UNDEF;
     }


if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur243*initial Exit   \n");
  }

   return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif



/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Initialize the memory area for NURBS nodes                       */

   static short setvalue (p_patnu )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATNU  *p_patnu;    /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBHvector *p_pp;          /* Node                          (ptr) */
   DBint    m_u;             /* Maximum node index in U direction   */
   DBint    m_v;             /* Maximum node index in V direction   */
   DBint    i_up;            /* Point U address                     */
   DBint    i_vp;            /* Point V address                     */
   DBfloat  polyx;           /* Polygon point X value               */
   DBfloat  polyy;           /* Polygon point Y value               */
   DBfloat  polyz;           /* Polygon point Z value               */
   DBfloat  polyw;           /* Polygon point w value               */
   DBint    order_u;         /* Order  for surface in U direction   */
   DBint    order_v;         /* Order  for surface in V direction   */
   DBint    deg_u;           /* Degree for surface in U direction   */
   DBint    deg_v;           /* Degree for surface in V direction   */
   DBint    nk_u;            /* Number of nodes    in U direction   */
   DBint    nk_v;            /* Number of nodes    in V direction   */
   DBint    c_iuv;           /* Index to node in memory area        */
/*-----------------------------------------------------------------!*/
   DBfloat  i_vp_dec;        /* i_vp as decimal                     */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur243*setvalue Enter\n");
  }
#endif

  i_vp_dec = F_UNDEF;

/*!                                                                 */
/* Get number of knot values                                        */
/*                                                                 !*/

    nk_u    = p_patnu->nk_u;
    nk_v    = p_patnu->nk_v;

/*!                                                                 */
/* Get the order (degree) of the basis functions.                   */
/*                                                                 !*/

    order_u = p_patnu->order_u;
    order_v = p_patnu->order_v;
    deg_u   = order_u - 1;
    deg_v   = order_v - 1;

/*!                                                                 */
/* Calculate the number of nodes                                    */
/*                                                                 !*/

    m_u     = nk_u - 1 - deg_u - 1;
    m_v     = nk_v - 1 - deg_v - 1;

for  ( i_up = 0; i_up <=  m_u; i_up++)
{
for  ( i_vp = 0; i_vp <=  m_v; i_vp++)
{
       if         ( i_vp <    10 ) i_vp_dec = (DBfloat)(i_vp)/10.0;
       else if    ( i_vp <   100 ) i_vp_dec = (DBfloat)(i_vp)/100.0;
       else if    ( i_vp <  1000 ) i_vp_dec = (DBfloat)(i_vp)/1000.0;
       else if    ( i_vp < 10000 ) i_vp_dec = (DBfloat)(i_vp)/10000.0;
       else                        i_vp_dec = (DBfloat)(i_vp)/100000.0;

       polyx  = (DBfloat)i_up + i_vp_dec;
       polyy  = polyx;
       polyz  = polyx;
       polyw  = polyx;
/*     Calculate the index c_iuv to the node in memory area         */
       c_iuv = i_up*(m_v+1) + i_vp;
/*     Set the pointer p_pp                                         */
       p_pp = p_patnu->cpts + c_iuv;
/*     Write data                                                   */
       p_pp->x_gm = polyx;
       p_pp->y_gm = polyy;
       p_pp->z_gm = polyz;
       p_pp->w_gm = polyw;

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"i_up %3d i_vp %3d x %12.4f y %12.4f z %12.4f c_iuv %d\n",
 (int)i_up,(int)i_vp, polyx, polyy, polyz, (int)c_iuv  );
fflush(dbgfil(SURPAC));
}
#endif
}
}

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur243*setvalue Node data initialized \n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of nodes to Debug File for Debug On.                    */

   static short printval (p_patnu )

/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   GMPATNU  *p_patnu;    /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*        The value of the function                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    m_u;             /* Maximum node index in U direction   */
   DBint    m_v;             /* Maximum node index in V direction   */
   DBint    order_u;         /* Order  for surface in U direction   */
   DBint    order_v;         /* Order  for surface in V direction   */
   DBint    deg_u;           /* Degree for surface in U direction   */
   DBint    deg_v;           /* Degree for surface in V direction   */
   DBint    nk_u;            /* Number of nodes    in U direction   */
   DBint    nk_v;            /* Number of nodes    in V direction   */
#ifdef  DEBUG
   DBHvector *p_pp;          /* Node                          (ptr) */
   DBint    i_up;            /* Point U address                     */
   DBint    i_vp;            /* Point V address                     */
   DBfloat  polyx;           /* Polygon point X value               */
   DBfloat  polyy;           /* Polygon point Y value               */
   DBfloat  polyz;           /* Polygon point Z value               */
   DBfloat  polyw;           /* Polygon point w value               */
   DBint    c_iuv;           /* Index to node in memory area        */
#endif
/*-----------------------------------------------------------------!*/
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Get number of knot values                                        */
/*                                                                 !*/

    nk_u    = p_patnu->nk_u;
    nk_v    = p_patnu->nk_v;

/*!                                                                 */
/* Get the order (degree) of the basis functions.                   */
/*                                                                 !*/

    order_u = p_patnu->order_u;
    order_v = p_patnu->order_v;
    deg_u   = order_u - 1;
    deg_v   = order_v - 1;

/*!                                                                 */
/* Calculate the number of nodes                                    */
/*                                                                 !*/

    m_u     = nk_u - 1 - deg_u - 1;
    m_v     = nk_v - 1 - deg_v - 1;

#ifdef  DEBUG
c_iuv = 0;
for  ( i_up = 0; i_up <=  m_u; i_up++)
{
for  ( i_vp = 0; i_vp <=  m_v; i_vp++)
{
/*     Calculate the index c_iuv to the node in memory area         */
/* Do not calculate here! Check !  c_iuv = i_up*(m_v+1) + i_vp;     */
/*     Set the pointer p_pp                                         */
       p_pp = p_patnu->cpts + c_iuv;
       polyx  = p_pp->x_gm;
       polyy  = p_pp->y_gm;
       polyz  = p_pp->z_gm;
       polyw  = p_pp->w_gm;

       c_iuv = c_iuv + 1;

if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"i_up %3d i_vp %3d x %8.2f y %8.2f z %8.2f w %8.2f No: %d\n",
 (int)i_up,(int)i_vp, polyx, polyy, polyz, polyw, (int)c_iuv);
fflush(dbgfil(SURPAC));
}
}
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


