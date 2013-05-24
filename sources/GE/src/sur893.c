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
/*  Function: varkon_sur_nurbsche                  File: sur893.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function checks control points, knot vectors, weights       */
/*  and the order of the basis functions for NURBS surfaces         */
/*  and curves.                                                     */
/*                                                                  */
/*                                                                  */
/*  Order of points in input control point array poi:               */
/*                                                                  */
/*   V                                                              */
/* !         np_u= 7  and  np_v= 3                                  */
/* !                                                                */
/* !                                                                */
/* !   3     6     9    12    15    18    21  -->- V isoparam 3     */
/* !                                                                */
/* !   2     5     8    11    14    17    20  -->- V isoparam 2     */
/* !                                                                */
/* !   1     4     7    10    13    16    19  -->- V isoparam 1     */
/* !                                                                */
/* !   !     !     !     !     !     !     !                        */
/* !   !     !     !     !     !     !     !                        */
/* !   v     v     v     v     v     v     v                        */
/* !   !     !     !     !     !     !     !                        */
/* !                                                                */
/* !   U     U     U     U     U     U     U                        */
/* !                                                                */
/* !   i     i     i     i     i     i     i                        */
/* !   s     s     s     s     s     s     s                        */
/* !   o     o     o     o     o     o     o                        */
/* !   p     p     p     p     p     p     p                        */
/* !   a     a     a     a     a     a     a                        */
/* !   m     m     m     m     m     m     m                        */
/* !                                                                */
/* !   1     2     3     4     5     6     7                        */
/* !                                                                */
/*  ------------------------------------------------------------> U */
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

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nurbsche   Check NURBS surface/curve data   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer  tolerance           */
/* varkon_idknot           * Retrieve parameter tolerance           */
/* varkon_sur_nurbsnon     * Calculate number of nodes              */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_nurbsche        */
/* SU2993 = Severe program error (  ) in varkon_sur_nurbsche        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_nurbsche (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  
  DBint     f_surcur,    /* Flag for surface or curve check;        */
                         /* Eq. 1: Surface   Eq. 2: Curve           */
  DBint     order_u,     /* Order of surface in U direction         */
  DBint     order_v,     /* Order of surface in V direction         */
  DBint     nk_u,        /* Number of knot values in U direction    */
  DBint     nk_v,        /* Number of knot values in V direction    */
  DBVector *p_cpts,      /* Control points (nodes)            (ptr) */
  DBfloat  *p_uknots,    /* Knot vector in U direction        (ptr) */
  DBfloat  *p_vknots,    /* Knot vector in V direction        (ptr) */
  DBfloat  *p_w,         /* Weights for the control points    (ptr) */
  DBint    *p_error_code)/* Error code                        (ptr) */
                         /* Eq.   0: Data is OK                     */
                         /* Eq.  -1: U order < 2                    */
                         /* Eq.  -2: U order > MAX_NURBD            */
                         /* Eq.  -3: V order < 2                    */
                         /* Eq.  -4: V order > MAX_NURBD            */
                         /* Eq.  -5: Number of U knots < 2          */
                         /* Eq.  -6: Number of V knots < 2          */
                         /* Eq.  -7: Surface weight(s) < 0.0        */
                         /* Eq.  -8: Curve   weight(s) < 0.0        */
                         /* Eq.  -9: U & V knot vectors not defined */
                         /* Eq. -10: Only U knots defined (surface) */
                         /* Eq. -11: Only V knots ared defined      */
                         /* Eq.  -2:                                */
                         /* Eq. -97: Curve case not yet tested.     */
                         /* Eq. -98: sur892 failed                  */
                         /* Eq. -99: Flag surface/curve not 1 or 2  */

/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*      Data to p_error_code and the function return value          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBfloat  idknot;       /* Identical knots criterion               */

  DBint    np_u;         /* Number of control points in U direction */
  DBint    np_v;         /* Number of control points in V direction */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint    degree_u;     /* Degree in U direction                   */
  DBint    degree_v;     /* Degree in V direction                   */

  DBint    i_u;          /* Loop index U knot values                */
  DBint    j_v;          /* Loop index V knot values                */
  DBfloat  u_value;      /* U value from the U knot vector (array)  */
  DBfloat  v_value;      /* V value from the V knot vector (array)  */
  DBfloat  prev_u;       /* Previous value in U knot vector         */
  DBfloat  prev_v;       /* Previous value in V knot vector         */
  DBfloat  curr_u;       /* Current  value in U knot vector         */
  DBfloat  curr_v;       /* Current  value in V knot vector         */
  DBint    n_mult;       /* Number of equal knots (multiplicity)    */
  DBint    m_flag_u;     /* Multiplicity error flag for U knots     */
  DBint    m_flag_v;     /* Multiplicity error flag for V knots     */


  short    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

  DBint    i_w;          /* Loop index Weight                       */
  DBfloat  weight;       /* Weight                                  */

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
"sur893 Enter***varkon_sur_nurbsche\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur893 order_u= %d, order_v= %d\n",
       (int)order_u,(int)order_v);
fprintf(dbgfil(SURPAC),"sur893 nk_u= %d, nk_v= %d\n",
       (int)nk_u,(int)nk_v);
}
#endif

/*!                                                                 */
/* Initialize the output error code.                                */
/*                                                                 !*/

   *p_error_code = I_UNDEF;

/*!                                                                 */
/* Check the surface/curve flag.                                    */
/* Curve case is not yet implemented.                               */
/*                                                                 !*/

   if  (  f_surcur == 1 )
     {
     /* OK, surface */
     }
   else if ( f_surcur == 2 )
     {
     *p_error_code = -97; 
     /* Curve case is not yet tested      */
     sprintf(errbuf, "(Curve case not tested)%%sur893");
     return(varkon_erpush("SU2993",errbuf));
     }
   else
     { 
     *p_error_code = -99; 
     sprintf(errbuf, "(f_surcur not OK)%%sur893");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Identical knots criterion. Call of varkon_idknot (sur744).       */
/*                                                                 !*/

   idknot =varkon_idknot();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
weight = -0.123456789;
fprintf(dbgfil(SURPAC),
"sur893 comptol %18.15f idknot %14.10f\n",
comptol, idknot  );
}
#endif

/*!                                                                 */
/* 2. Check the order of the surface                                */
/*                                                                 !*/


/*!                                                                 */
/* Check minimum value in U direction                               */
/*                                                                 !*/

  if  (  order_u <  2   )
    {
     *p_error_code = -1; 
     sprintf(errbuf, "%d %% (sur893)", 2 );
     varkon_erinit();
     return(varkon_erpush("SU7173",errbuf));
     }  /*  End error */


/*!                                                                 */
/* Check maximum value in U direction                               */
/*                                                                 !*/


  if  (  order_u > MAX_NURBD )
    {
     *p_error_code = -2; 
     sprintf(errbuf, "%d %% (sur893)",MAX_NURBD );
     varkon_erinit();
     return(varkon_erpush("SU7093",errbuf));
     }  /*  End error */



/*!                                                                 */
/* Check minimum value in V direction (only for surface)            */
/*                                                                 !*/

  if  (  order_v <  2  && f_surcur == 1 )
    {
     *p_error_code = -3; 
     sprintf(errbuf, "%d %% (sur893)", 2 );
     varkon_erinit();
     return(varkon_erpush("SU7173",errbuf));
     }  /*  End error */


/*!                                                                 */
/* Check maximum value in V direction (only for surface)            */
/*                                                                 !*/


  if  (  order_v > MAX_NURBD && f_surcur == 1 )
    {
     *p_error_code = -4; 
     sprintf(errbuf, "%d %% (sur893)",MAX_NURBD );
     varkon_erinit();
     return(varkon_erpush("SU7093",errbuf));
     }  /*  End error */


/*!                                                                 */
/* 2. Check the number of knot values                               */
/*                                                                 !*/

/*!                                                                 */
/* Check minimum value in U direction                               */
/*                                                                 !*/

  if  (  nk_u  <  2   ) 
    {
     *p_error_code = -5; 
     sprintf(errbuf, "%d %% %d", (int)nk_u, (int)order_u );
     varkon_erinit();
     return(varkon_erpush("SU7183",errbuf));
     }  /*  End error */

/*!                                                                 */
/* Check minimum value in V direction (only for surface)            */
/*                                                                 !*/


  if  (  nk_v  <  2  && f_surcur == 1 ) 
    {
     *p_error_code = -6; 
     sprintf(errbuf, "%d %% %d", (int)nk_v, (int)order_v );
     varkon_erinit();
     return(varkon_erpush("SU7193",errbuf));
     }  /*  End error */


/*!                                                                 */
/* 3. Calculate the number of nodes                                 */
/*                                                                 !*/

    status= varkon_sur_nurbsnon
    (order_u, order_v, nk_u, nk_v, 
     &np_u,  &np_v,  &degree_u,  &degree_v );
   if  ( status < 0 ) 
     {
     *p_error_code = -98; 
     sprintf(errbuf, "sur892%%sur893");
     return(varkon_erpush("SU2943",errbuf));
     }



#ifdef TABORT
if ( dbglev(SURPAC) == 1 )
{
if ( p_uknots != NULL ) for ( i=0; i<nk_u; ++i )
  fprintf(dbgfil(SURPAC),"sur893 U-knot%d=%g\n",
       (int)i,*(p_uknots+i));
if ( p_vknots != NULL ) for ( i=0; i<nk_v; ++i )
  fprintf(dbgfil(SURPAC),"sur893 V-knot%d=%g\n",
       (int)i,*(p_vknots+i));
if ( p_w != NULL ) for ( i=0; i<np_u*np_v; ++i )
  fprintf(dbgfil(SURPAC),"sur893 w%d=%g\n",
       (int)i,*(p_w+i));

for ( i=0; i<np_u*np_v; ++i )
  {
  fprintf(dbgfil(SURPAC),"sur893 p%d= %g,%g,%g\n",
     (int)i,(double)(p_cpts+i)->x_gm,
    (double)(p_cpts+i)->y_gm,(double)(p_cpts+i)->z_gm);
  }

fflush(dbgfil(SURPAC));
}
#endif



/*!                                                                 */
/* 4. Check weights                                                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check weights if defined. Undefined means in sur890 SUR_NURBSARR */
/* that all the weights will be set to one (1).                     */
/* Weights must be greater than zero (=comptol)                     */
/*                                                                 !*/

/*!                                                                 */
/* Check weights for a surface                                      */
/*                                                                 !*/

   if  ( p_w != NULL && f_surcur == 1 )
     { 
     for ( i_w=0; i_w<np_u*np_v; ++i_w )
       {
       weight=   *(p_w+i_w);
       if  (  weight < comptol )
         {
          *p_error_code = -7; 
          sprintf(errbuf, " %% (sur893)" );
          varkon_erinit();
          return(varkon_erpush("SU7103",errbuf));
          }  /*  End error */
       } /* End loop i_w */
      } /* End weights are defined for a surface */


/*!                                                                 */
/* Check weights for a curve                                        */
/*                                                                 !*/

   if  ( p_w != NULL && f_surcur == 2 )
     { 
     for ( i_w=0; i_w<np_u; ++i_w )
       {
       weight=   *(p_w+i_w);
       if  (  weight < comptol )
         {
          *p_error_code = -8; 
          sprintf(errbuf, " %% (sur893)" );
          varkon_erinit();
          return(varkon_erpush("SU7103",errbuf));
          }  /*  End error */
       } /* End loop i_w */
      } /* End weights are defined for a curve   */


/*!                                                                 */
/* 5. Check the knot values.                                        */
/* _________________________                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Check that U knots and V knots not both are NULL                 */
/*                                                                 !*/


  if  (  p_uknots == NULL &&  p_vknots == NULL )
    {
     *p_error_code = -9; 
     sprintf(errbuf, " %% (sur893)" );
     varkon_erinit();
     return(varkon_erpush("SU7073",errbuf));
     }  /*  End error */

/*!                                                                 */
/* Check that not only U knots are defined for a surface            */
/*                                                                 !*/


  if  (  p_uknots != NULL &&  p_vknots == NULL && f_surcur == 1 )
    {
     *p_error_code = -10; 
     sprintf(errbuf, " %% (sur893)" );
     varkon_erinit();
     return(varkon_erpush("SU7053",errbuf));
     }  /*  End error */

/*!                                                                 */
/* Check that not only V knots are defined                          */
/*                                                                 !*/

  if  (  p_uknots == NULL &&  p_vknots != NULL )
    {
     *p_error_code = -11; 
     sprintf(errbuf, " %% (sur893)" );
     varkon_erinit();
     return(varkon_erpush("SU7063",errbuf));
     }  /*  End error */

/*!                                                                 */
/* Check the multiplicity of the input U knots                      */
/*                                                                 !*/


    m_flag_u = 0;
    n_mult = 1;
    for ( i_u = 0; i_u < nk_u; ++i_u )
       {
       u_value   = *(p_uknots+i_u);
       if ( i_u ==    0   ) prev_u = u_value  ;
       else
         {
          curr_u = u_value  ;
         if  ( fabs(prev_u - curr_u) < idknot  )
           {
           n_mult = n_mult + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 i_u+1 %d n_mult %d order_u %d nk_u-order_u+1 %d\n",
(int)i_u+1,(int)n_mult,(int)order_u,(int)(nk_u-order_u+1));
}
#endif
           if       (  i_u + 1 <= order_u + 1 )
             {
             if ( n_mult > order_u ) m_flag_u = 1;
             }
           else if  (  i_u + 1 >= nk_u - order_u + 1)
             {
             if ( n_mult > order_u ) m_flag_u = 2;
             }
           else
             {
             if ( n_mult > order_u-1 ) m_flag_u = 3;
             }
             
           }
         else n_mult = 1;
         } /* End i_u > 1 */

       prev_u = u_value  ;
       }   /* End loop i_u */




/*!                                                                 */
/* Check the multiplicity of the input V knots for a surface        */
/*                                                                 !*/


  m_flag_v = I_UNDEF;
  if  (  f_surcur == 1 )
    {

    m_flag_v = 0;
    n_mult = 1;
    for ( j_v = 0; j_v < nk_v; ++j_v )
       {
       v_value   = *(p_vknots+j_v);
       if ( j_v ==    0   ) prev_v = v_value  ;
       else
         {
          curr_v = v_value  ;
         if  ( fabs(prev_v - curr_v) < idknot  )
           {
           n_mult = n_mult + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 j_v+1 %d n_mult %d order_v %d nk_v-order_v+1 %d\n",
(int)j_v+1,(int)n_mult,(int)order_v,(int)(nk_v-order_v+1));
}
#endif
           if       (  j_v + 1 <= order_v + 1 )
             {
             if ( n_mult > order_v ) m_flag_v = 1;
             }
           else if  (  j_v + 1 >= nk_v - order_v + 1)
             {
             if ( n_mult > order_v ) m_flag_v = 2;
             }
           else
             {
             if ( n_mult > order_v-1 ) m_flag_v = 3;
             }
             
           }
         else n_mult = 1;
         } /* End j_v > 1 */

       prev_v = v_value  ;
       }   /* End loop j_v */

     } /* Multiplicity check of V knots */


/*!                                                                 */
/* Output of errors (and return) if the multiplicity of the input   */
/* U knots not are OK.                                              */
/*                                                                 !*/

     if       (  m_flag_u == 0  )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 Multiplicity of the U knot vector is OK\n");
}
#endif
       }
     else if  (  m_flag_u == 1  )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_u %d equal start knot U values \n",
(int)order_u);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_u);
       varkon_erinit();
       return(varkon_erpush("SU7113",errbuf));
       }
     else if  (  m_flag_u == 2  )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_u %d equal end   knot U values \n",
(int)order_u);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_u);
       varkon_erinit();
       return(varkon_erpush("SU7123",errbuf));
       }
     else if  (  m_flag_u == 3 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_u-1 %d equal mid knot U values \n",
(int)order_u-1);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_u-1);
       varkon_erinit();
       return(varkon_erpush("SU7133",errbuf));
       }

/*!                                                                 */
/* Output of errors (and return) if the multiplicity of the input   */
/* V knots not are OK for a surface.                                */
/*                                                                 !*/


     if       (  m_flag_v == 0 && f_surcur == 1    )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 Multiplicity of the V knot vector is OK\n");
}
#endif
       }
     else if  (  m_flag_v == 1 && f_surcur == 1    )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_v %d equal start knot V values \n",
(int)order_v);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_v);
       varkon_erinit();
       return(varkon_erpush("SU7143",errbuf));
       }
     else if  (  m_flag_v == 2 && f_surcur == 1    )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_v %d equal end   knot V values \n",
(int)order_v);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_v);
       varkon_erinit();
       return(varkon_erpush("SU7153",errbuf));
       }
     else if  (  m_flag_v == 3 && f_surcur == 1    )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur893 More than order_v-1 %d equal mid knot V values \n",
(int)order_v-1);
}
#endif
       sprintf(errbuf, "%d%%(sur893)",(int)order_v-1);
       varkon_erinit();
       return(varkon_erpush("SU7163",errbuf));
       }

/*!                                                                 */
/* Check that the U knot values not are descending                  */
/*                                                                 !*/

  curr_u = F_UNDEF;
  curr_v = F_UNDEF;
  prev_u = F_UNDEF;
  prev_v = F_UNDEF;

/*   Start loop U knot values                                       */
     for ( i_u = 0; i_u < nk_u; ++i_u )
       {
        u_value   = *(p_uknots+i_u);
        if ( i_u ==    0   ) prev_u = u_value  ;
        else
          {
           curr_u = u_value  ;
          if  ( prev_u > curr_u + idknot  )
            {
            sprintf(errbuf, "%d%% %d",
            (short)(i_u),(short)(i_u+1) );
             varkon_erinit();
            return(varkon_erpush("SU7033",errbuf));
            }  /*  End error */
          prev_u = curr_u;
          }   /* Endif: Not the first knot value */
       }   /* End loop i_u */

  if  (  f_surcur == 1 )
  {
     for ( j_v = 0; j_v < nk_v; ++j_v )
       {
        v_value   = *(p_vknots+j_v);
        if ( j_v ==    0   ) prev_v = v_value  ;
        else
          {
           curr_v = v_value  ;
          if  ( prev_v > curr_v + idknot  )
            {
            sprintf(errbuf, "%d%% %d",
            (short)(j_v),(short)(j_v+1) );
             varkon_erinit();
            return(varkon_erpush("SU7043",errbuf));
            }  /*  End error */
          prev_v = curr_v;
          }   /* Endif: Not the first knot value */
       }   /* End loop j_v */


    } /* End descending test for surface V knots  */

/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

/*!                                                                 */
/* Input data is OK. Let output error code be zero (0).             */
/*                                                                 !*/

   *p_error_code = 0;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur893 Exit*varkon_sur_nurbsche *p_error_code %d\n"
    , (int)(*p_error_code) );
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


