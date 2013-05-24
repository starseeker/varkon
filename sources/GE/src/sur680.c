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
/*  Function: varkon_cur_averplan                  File: sur680.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates an avearge plane for a curve            */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-29   Originally written                                 */
/*  1997-12-20   Center of gravity output                           */
/*  1999-12-14   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_averplan   Curve average plane              */
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
/* varkon_idangle         * Identical angles criterion              */
/* GE109                  * Curve   evaluation routine              */
/* varkon_xprod           * Cross product calculation               */
/* varkon_normv           * Normalization of a vector               */
/* varkon_angd            * Angle between vectors (degr.)           */
/* varkon_erpush          * Error message to terminal               */
/* varkon_ini_evalc       * Initialize EVALS                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
extern short sur776();             /* Initialize EVALS              */
#endif


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_averplan         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_averplan (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBfloat  pcrit,        /* Position criterion < 0 ==> system value */
  DBfloat  dcrit,        /* Derivat. criterion < 0 ==> system value */
  DBint    n_add,        /* Number of points per curve segment      */
  DBVector *p_cog,       /* Center of gravity                 (ptr) */
  DBfloat *p_maxdev,     /* Maximum deviation curv/plane      (ptr) */
  DBfloat  a_plane[] )   /* Average plane                           */

/* Out:                                                             */
/*        Values to p_maxdev and a_plane                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint    istart;       /* Start segment ( < 0 == whole curve )    */
  DBint    iend;         /* End   segment                           */
  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBint    i_seg;        /* Loop index segment in curve             */
  DBint    i_poi;        /* Loop index point in segment             */
  DBVector c_pt;         /* Current curve point                     */
  DBVector c_pt1;        /* Current curve point 1                   */
  DBVector c_pt2;        /* Current curve point 2                   */
  EVALC    xyz;          /* Coordinates and derivatives for crv pt  */
  DBfloat  delta;        /* Delta parameter value                   */
  DBint    n_tot;        /* Total number of points                  */
  DBfloat  xsum;         /* Sum of x components                     */
  DBfloat  ysum;         /* Sum of y components                     */
  DBfloat  zsum;         /* Sum of z components                     */
  DBVector tp;           /* Center of gravity                       */
  DBfloat  u_vec[3];     /* Vector for cross product calculation    */
  DBfloat  v_vec[3];     /* Vector for cross product calculation    */
  DBfloat  cross[3];     /* Cross product vector                    */
  DBfloat  idpoint;      /* Identical points criterion              */
  DBfloat  idangle;      /* Identical angles criterion              */

  DBfloat  max_dev;      /* Maximum deviation to average plane      */
  DBfloat  d_r;          /* D value for the output plane            */
  DBfloat  d_p;          /* D value for the current point           */

  short    status;       /* Error code from a called function       */
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
"sur680 Enter varkon_cur_averplan Average plane for a curve \n");
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur680 p_cur->ns_cu %d n_add %d\n", 
        (int)p_cur->ns_cu, (int)n_add );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur680 pcrit %12.8f dcrit %12.8f \n",pcrit,dcrit);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

    tp.x_gm    = F_UNDEF;
    tp.y_gm    = F_UNDEF;
    tp.z_gm    = F_UNDEF;
    *p_cog     = tp;
    *p_maxdev  = F_UNDEF;
    a_plane[0] = F_UNDEF;
    a_plane[1] = F_UNDEF;
    a_plane[2] = F_UNDEF;
    a_plane[3] = F_UNDEF;
    delta      = F_UNDEF;
    xsum       = F_UNDEF;
    ysum       = F_UNDEF;
    zsum       = F_UNDEF;
    max_dev    = F_UNDEF;
    d_r        = F_UNDEF;
    d_p        = F_UNDEF;

/*!                                                                 */
/* Retrieve identical points criterion if input pcrit < 0.          */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

if ( pcrit < 0.0 )
   {
   idpoint   =varkon_idpoint();
   }
else
   {
   idpoint = pcrit;
   }

/*!                                                                 */
/* Retrieve identical angles criterion if input dcrit < 0.          */
/* Call of varkon_idangle (sur742).                                 */
/*                                                                 !*/

if ( dcrit < 0.0 )
   {
   idangle   =varkon_idangle();
   }
else
   {
   idangle = dcrit;
   }

#ifdef DEBUG  
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 idpoint %12.8f idangle %12.8f \n", idpoint , idangle);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 comptol %25.15f\n", comptol );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Initialize EVALC for DEBUG On                                    */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

#ifdef  DEBUG
      status=varkon_ini_evalc (&xyz);
#endif

   xyz.evltyp   = EVC_DR;

/*!                                                                 */
/* Check of n_add                                                   */
/*                                                                 !*/

if      ( n_add < 1  )
   {
   sprintf(errbuf,"n_add %d < 1%%sur680",
                     (int)n_add );
        return(varkon_erpush("SU2993",errbuf));
   }

/* Initialize the center of gravity                                 */
   tp.x_gm    = 0.0;
   tp.y_gm    = 0.0;
   tp.z_gm    = 0.0;

/* Initialize the number of points                                  */
   n_tot = 0;

/* Initialize the number of points                                  */
   delta = 1.0/(1+n_add);

/* All segments of the curve                                        */
   istart = 1;
   iend   = p_cur->ns_cu;

/*!                                                                 */
/* 2. Calculate center of gravity                                   */
/* ______________________________                                   */
/*                                                                  */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
      for ( i_poi = 1; i_poi <= n_add; i_poi = i_poi + 1 )
        {
/*!                                                                 */
/*    Curve evaluation for i_poi in segment i_seg                   */
/*    Call of GE109.                                                */
/*                                                                 !*/

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + (i_poi-1)*delta;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 i_seg %d xyz.t_global %25.15f for point e_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(varkon_erpush("SU2943",errbuf));
        }

      c_pt = xyz.r;

/*!                                                                 */
/*    Center of gravity.                                            */
/*                                                                 !*/

     tp.x_gm  = tp.x_gm + c_pt.x_gm;
     tp.y_gm  = tp.y_gm + c_pt.y_gm;
     tp.z_gm  = tp.z_gm + c_pt.z_gm;

     n_tot    = n_tot + 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 i_seg %d c_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,c_pt.x_gm,c_pt.y_gm,c_pt.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

        }/* End loop segment points i_poi= 1,2,3                    */
      }  /* End loop curve segments i_seg= 1,2,3                    */

    if ( n_tot == 0 )
      {
      sprintf(errbuf,"n_tot = 0%%sur680" );
       return(varkon_erpush("SU2993",errbuf));
   }



    tp.x_gm  = tp.x_gm/(DBfloat)n_tot;
    tp.y_gm  = tp.y_gm/(DBfloat)n_tot;
    tp.z_gm  = tp.z_gm/(DBfloat)n_tot;


#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur680 Center of gravity tp %f %f %f\n", 
  tp.x_gm,tp.y_gm,tp.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 2. Calculate average plane                                       */
/* __________________________                                       */
/*                                                                  */
/*                                                                 !*/


    xsum  = 0.0;
    ysum  = 0.0;
    zsum  = 0.0;
    n_tot = 0;

/*!                                                                 */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
      for ( i_poi = 1; i_poi <= n_add-1; i_poi = i_poi + 1 )
        {
/*!                                                                 */
/*    Curve evaluation for i_poi in segment i_seg                   */
/*    Call of GE109.                                                */
/*                                                                 !*/

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + (i_poi-1)*delta;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 i_seg %d xyz.t_global %25.15f for point e_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(erpush("SU2943",errbuf));
        }

      c_pt1 = xyz.r;

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + i_poi*delta;

      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(erpush("SU2943",errbuf));
        }

      c_pt2 = xyz.r;

/*!                                                                 */
/*    Cross product vector                                          */
/*                                                                 !*/

     u_vec[0] = - tp.x_gm + c_pt1.x_gm;
     u_vec[1] = - tp.y_gm + c_pt1.y_gm;
     u_vec[2] = - tp.z_gm + c_pt1.z_gm;
     v_vec[0] = - tp.x_gm + c_pt2.x_gm;
     v_vec[1] = - tp.y_gm + c_pt2.y_gm;
     v_vec[2] = - tp.z_gm + c_pt2.z_gm;

      status=varkon_xprod (v_vec, u_vec, cross);
      if (status<0) 
        {
        goto nocross;
        }

     n_tot    = n_tot + 1;

     xsum = xsum + cross[0]; 
     ysum = ysum + cross[1];
     zsum = zsum + cross[2];

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 i_seg %d i_poi %d c_pt1 %10.2f %10.2f %10.2f\n",
  (int)i_seg,(int)i_poi,c_pt1.x_gm,c_pt1.y_gm,c_pt1.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

nocross:;
        }/* End loop segment points i_poi= 1,2,3                    */
      }  /* End loop curve segments i_seg= 1,2,3                    */

    if ( n_tot == 0 )
      {
      sprintf(errbuf,"n_tot = 0%%sur680" );
       return(varkon_erpush("SU2993",errbuf));
   }


    a_plane[0] = xsum/(DBfloat)n_tot; 
    a_plane[1] = ysum/(DBfloat)n_tot;
    a_plane[2] = zsum/(DBfloat)n_tot;

      status=varkon_normv (a_plane);
      if (status<0) 
        {
        sprintf(errbuf,"a_plane= 0%%sur680" );
         return(varkon_erpush("SU2993",errbuf));
        }


   a_plane[3] = tp.x_gm*a_plane[0] +   
                tp.y_gm*a_plane[1] +   
                tp.z_gm*a_plane[2];

/* Cross product vector                                             */
   *p_cog     = tp;


/*!                                                                 */
/* 2. Calculate the maximum deviation with sign                     */
/* ____________________________________________                     */
/*                                                                  */
/*                                                                 !*/

/*  Initialize maximum deviation                                    */
    max_dev = 0.0;

/*  D coefficient for the output plane                              */
    d_r = a_plane[3];

/*!                                                                 */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
      for ( i_poi = 1; i_poi <= n_add; i_poi = i_poi + 1 )
        {
/*!                                                                 */
/*    Curve evaluation for i_poi in segment i_seg                   */
/*    Call of GE109.                                                */
/*                                                                 !*/

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + (i_poi-1)*delta;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur680 i_seg %d xyz.t_global %25.15f for point c_pt1\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(varkon_erpush("SU2943",errbuf));
        }

      c_pt1 = xyz.r;

/*   D coefficient for the current point                            */
     d_p      = c_pt1.x_gm*a_plane[0] +   
                c_pt1.y_gm*a_plane[1] +   
                c_pt1.z_gm*a_plane[2];

/*   Maximum deviation                                              */
     if ( fabs(d_r-d_p) > fabs(max_dev) )
        {
         max_dev = d_r - d_p;
        }

        }/* End loop segment points i_poi= 1,2,3                    */
      }  /* End loop curve segments i_seg= 1,2,3                    */


   *p_maxdev = max_dev;

/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur680 Exit a_plane %f %f %f %f\n", 
  a_plane[0], a_plane[1], a_plane[2], a_plane[3] );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
