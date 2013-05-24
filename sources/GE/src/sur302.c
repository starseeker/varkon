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
#include "../../IG/include/debug.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_cur_secta                     File: sur302.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculation of section area, center of gravity and moments      */
/*  of inertia for the planar area bounded by a curve.              */
/*                                                                  */
/*  The curve need not to be closed. As an option will the curve    */
/*  be closed by a straight line.                                   */
/*                                                                  */
/*  The function calculates the maximum deviation to an average     */
/*  plane for the curve. This calculation is based on a planar      */
/*  curve. The maximum deviation will not be checked against any    */
/*  criterion. The calling function must do this check.             */
/*                                                                  */
/*  There is no check in the function if the curve is self-         */
/*  intersecting.                                                   */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-20   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*  2000-03-22   Error fixed with sign for triangles                */
/*  2010-09-07   Bugfix n_add < 0, J. Kjellander                    */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_secta      Section area c.o.g. m.o. inertia */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_cur_averplan    * Average plane for a curve               */
/* varkon_xprod           * Cross product calculation               */
/* varkon_scalar          * Scalar product                          */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_ini_evalc       * Initialize EVALS                        */
/* GEarclength            * Arclength for a curve                   */
/* GE109                  * Curve evaluation function               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_cur_secta  (sur302) */
/* SU2993 = Severe program error (  ) in varkon_cur_secta  (sur302) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_cur_secta (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBTmat  *p_csys,       /* Coordinate system for m.o.i.      (ptr) */
  DBfloat  delta,        /* Calculation start delta value which     */
                         /* defines the number of points/segment    */
                         /* ( < 0.0  ==>  100.0*idpoint )           */ 
  DBfloat  a_crit,       /* End area calculation criterion          */
                         /* ( < 0.0  ==> 0.005 = 0.5 % error )      */ 
  DBint    c_case,       /* Calculation case:                       */
                         /* Eq.  1: Use only delta and not a_crit   */
                         /* Eq.  2: Use delta as start and a_crit   */
                         /* Eq. 11: As 1, but error unclosed curve  */
                         /* Eq. 12: As 2, but error unclosed curve  */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBfloat  *p_maxdev,    /* Maximum deviation curve/plane     (ptr) */
  DBfloat  *p_sarea,     /* Section area                      (ptr) */
  DBVector *p_cog,       /* Center of gravity                 (ptr) */
  DBVector *p_moi,       /* Moments of inertia (jx,jy,jz)     (ptr) */
                         /* w.r.t to coordinate system p_csys       */
  DBVector *p_axis1,     /* Principal axis 1                  (ptr) */
  DBVector *p_axis2,     /* Principal axis 2                  (ptr) */
  DBVector *p_axis3 )    /* Principal axis 3                  (ptr) */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   DBfloat  idpoint;     /* Identical points criterion              */
   DBfloat  pcrit;       /* Position criterion < 0 ==> system value */
   DBfloat  dcrit;       /* Derivat. criterion < 0 ==> system value */
   DBVector curve_cog;   /* Start center of gravity                 */
   DBfloat  maxdev;      /* Maximum deviation curve/plane           */
   DBfloat  a_plane[4];  /* Average plane                           */
   DBint    istart;      /* Start segment                           */
   DBint    iend;        /* End   segment                           */
   DBfloat  close_area;  /* Area for closing triangle               */
   DBVector close_cog;   /* Center of gravity for closing triangle  */
   DBint    i_seg;       /* Loop index segment in curve             */
   DBint    i_poi;       /* Loop index point in segment             */
   DBVector c_pt1;       /* Current curve point 1                   */
   DBVector c_pt2;       /* Current curve point 2                   */
   DBfloat  dist;        /* Distance between start and end point    */
   EVALC    xyz;         /* Coordinates and derivatives for crv pt  */
   DBint    n_tot;       /* Total number of points                  */
   DBfloat  u_vec[3];    /* Vector for cross product calculation    */
   DBfloat  v_vec[3];    /* Vector for cross product calculation    */
   DBfloat  cross[3];    /* Cross product vector                    */
   DBfloat  sectarea;    /* Section    area                         */
   DBfloat  sign;        /* Sign +/- 1.0 for current triangle       */
   DBfloat  trian_area;  /* Area for current triangle               */
   DBVector trian_cog;   /* Center of gravity for current triangle  */
   DBint    n_add;       /* Number of points per curve segment      */
   DBfloat  tot_leng;    /* Total arclength for the input curve     */
   DBfloat  seg_leng;    /* Arclength for a segment of input curve  */
   DBfloat  sum_leng;    /* Arclength for a segment of input curve  */
   DBfloat  delta_iter;  /* Changed (decreased) delta value         */
   DBfloat  delta_par;   /* Delta parameter value for delta_iter    */



   DBfloat  startarea;   /* Section start area                      */
   DBfloat  a_all[10];   /* Surface area from all iterations        */
   DBint    a_max;       /* Maximum number of area calc. iterations */
   DBint    a_cur;       /* Current number of area calc. iterations */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat dot_sign;     /* Scalar product for the sign calculation */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


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
  "sur302 Enter*varkon_cur_secta a_crit %f delta %f c_case %d\n",
          a_crit , delta, (int)c_case );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur302 p_cur %d p_seg %d p_csys %d \n", 
         (int)p_cur, (int)p_seg, (int)p_csys );
  }
#endif
 
#ifdef DEBUG
 *p_maxdev      = F_UNDEF;        /* Maximum deviation from plane   */
 *p_sarea       = F_UNDEF;        /* Surface area                   */
  p_cog->x_gm   = F_UNDEF;        /* Center of gravity              */
  p_cog->y_gm   = F_UNDEF;        /*                                */
  p_cog->z_gm   = F_UNDEF;        /*                                */
  p_moi->x_gm   = F_UNDEF;        /* Moments of inertia             */
  p_moi->y_gm   = F_UNDEF;        /*                                */
  p_moi->z_gm   = F_UNDEF;        /*                                */
  p_axis1->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis1->y_gm = F_UNDEF;        /*                                */
  p_axis1->z_gm = F_UNDEF;        /*                                */
  p_axis2->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis2->y_gm = F_UNDEF;        /*                                */
  p_axis2->z_gm = F_UNDEF;        /*                                */
  p_axis3->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis3->y_gm = F_UNDEF;        /*                                */
  p_axis3->z_gm = F_UNDEF;        /*                                */
  close_area    = F_UNDEF;
  close_cog.x_gm = F_UNDEF;      /* Center of gravity              */
  close_cog.y_gm = F_UNDEF;      /*                                */
  close_cog.z_gm = F_UNDEF;      /*                                */
  curve_cog.x_gm = F_UNDEF;      /* Center of gravity              */
  curve_cog.y_gm = F_UNDEF;      /*                                */
  curve_cog.z_gm = F_UNDEF;      /*                                */
  maxdev         = F_UNDEF;
  sectarea       = F_UNDEF;
  tot_leng       = F_UNDEF;
  seg_leng       = F_UNDEF;
  sum_leng       = F_UNDEF;
  delta_par      = F_UNDEF;
  sign           = F_UNDEF;
  dot_sign       = F_UNDEF;
#endif

/*!                                                                 */
/* Retrieve identical points criterion                              */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

   idpoint   =varkon_idpoint();

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur302 comptol %25.15f\n", comptol );
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
/* Retrieve the total arclength for the curve.                      */
/* Call of GEarclength if not calculated.                           */
/*                                                                 !*/

   tot_leng = p_cur->al_cu;
   if ( tot_leng < comptol )
      {
      status=GEarclength ((DBAny *)p_cur,p_seg,&tot_leng);
      if (status<0) 
         {
         sprintf(errbuf,"GEarclength%%sur302");
         return(varkon_erpush("SU2943",errbuf));
         }
       }  /* Calculate arclength */

/*!                                                                 */
/* Check delta and a_crit and set n_add for for average plane       */
/*                                                                 !*/


if      ( a_crit <= 0.0  )
   {
   a_crit = 0.005;
   }

if      ( delta <= 0.0  )
   {
   delta =  100.0*idpoint;
   }

/* All segments of the curve                                        */
   istart = 1;
   iend   = p_cur->ns_cu;

#ifdef  DEBUG
   if  ( iend == 0 )
     {
     sprintf(errbuf,"iend=0%%sur302");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif

   n_add = (DBint)(tot_leng/delta/(DBfloat)iend);
   if      ( n_add > 5000 ) n_add = 5000;
   else if ( n_add < 1 )    n_add = 2;        /* Bugfix 2010-09-08 J. Kjellander */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur302 istart %d iend %d n_add %d tot_leng %f delta %f\n", 
  (int)istart, (int)iend, (int)n_add, tot_leng, delta );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Maximum number of area calculations is 10 for c_case= 2          */
/*                                                                 !*/

  if      ( c_case == 1 || c_case == 11 ) a_max =  1;
  else if ( c_case == 2 || c_case == 12 ) a_max = 10;
  else 
    {
    sprintf(errbuf,"c_case%%sur302");
    return(varkon_erpush("SU2993",errbuf));
    }

  a_cur     = 0;
  a_all[0]  = F_UNDEF;
  a_all[1]  = F_UNDEF;
  a_all[2]  = F_UNDEF;
  a_all[3]  = F_UNDEF;
  a_all[4]  = F_UNDEF;
  a_all[5]  = F_UNDEF;
  a_all[6]  = F_UNDEF;
  a_all[7]  = F_UNDEF;
  a_all[8]  = F_UNDEF;
  a_all[9]  = F_UNDEF;

/*!                                                                 */
/* 2. Calculate an average plane, maximum deviation                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/


   pcrit      = F_UNDEF;    
   dcrit      = F_UNDEF;   
   maxdev     = F_UNDEF; 
   a_plane[0] = F_UNDEF;  
   a_plane[1] = F_UNDEF;  
   a_plane[2] = F_UNDEF;  
   a_plane[3] = F_UNDEF;  

   status = varkon_cur_averplan
    (p_cur,p_seg,pcrit,dcrit, n_add,&curve_cog, &maxdev, a_plane );
    if ( status < 0 )
      {
      varkon_erinit();
      sprintf(errbuf,"(sur302)%%");
      return(varkon_erpush("SU8403",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur302 Curve cog %10.4f %10.4f %10.4f maxdev %f\n", 
 curve_cog.x_gm , curve_cog.y_gm , curve_cog.z_gm, maxdev );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Calculate triangle area for a non-closed curve                */
/* __________________________________________________               */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Curve evaluation for start and end point of curve                */
/* Calls of GE109.                                                  */
/*                                                                 !*/

/* Parameter value for start of curve                               */

   xyz.t_global = (DBfloat)istart;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur302");
     return(varkon_erpush("SU2943",errbuf));
     }

   c_pt1 = xyz.r;

/* Parameter value for end   of curve                               */

   xyz.t_global = (DBfloat)iend + 1.0;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur302");
     return(varkon_erpush("SU2943",errbuf));
     }

   c_pt2 = xyz.r;

/*!                                                                 */
/* Calculate distance between points. Check if curve is closed.     */
/*                                                                 !*/

  dist = sqrt( 
         (c_pt2.x_gm-c_pt1.x_gm)*(c_pt2.x_gm-c_pt1.x_gm)+
         (c_pt2.y_gm-c_pt1.y_gm)*(c_pt2.y_gm-c_pt1.y_gm)+
         (c_pt2.z_gm-c_pt1.z_gm)*(c_pt2.z_gm-c_pt1.z_gm)  );

#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur302 Start and end pt dist %f idpoint %f\n",
     dist, idpoint  );
fflush(dbgfil(SURPAC)); 
}
#endif

  if     ( dist > idpoint  &&  c_case > 10 )
      {
      varkon_erinit();
      sprintf(errbuf,"(sur302)%%");
      return(varkon_erpush("SU8413",errbuf));
      }
  else if ( dist <= idpoint )
      {
      close_cog.x_gm = c_pt1.x_gm;
      close_cog.y_gm = c_pt1.y_gm;
      close_cog.z_gm = c_pt1.z_gm;
      close_area = 0.0;
      }
  else
      { /* Curve is not closed, a triangle shall be added  Start    */
/*!                                                                 */
/*    Case: Curve is not closed, a triangle shall be added          */
/*    Cross product vector                                          */
/*                                                                 !*/

      u_vec[0] = - curve_cog.x_gm + c_pt1.x_gm;
      u_vec[1] = - curve_cog.y_gm + c_pt1.y_gm;
      u_vec[2] = - curve_cog.z_gm + c_pt1.z_gm;
      v_vec[0] = - curve_cog.x_gm + c_pt2.x_gm;
      v_vec[1] = - curve_cog.y_gm + c_pt2.y_gm;
      v_vec[2] = - curve_cog.z_gm + c_pt2.z_gm;

      status=varkon_xprod (v_vec, u_vec, cross);
      if (status<0) 
        {
        /* Center of gravity is on line start-end point */
        close_area = 0.0;
        goto nocross;
        }

/*    Area for triangle                                             */
      close_area = 0.5 * sqrt( cross[0]*cross[0]+
                               cross[1]*cross[1]+
                               cross[2]*cross[2]);

/*    Center of gravity for triangle                                */

     close_cog.x_gm = curve_cog.x_gm +
       2.0/3.0*(( c_pt1.x_gm - curve_cog.x_gm) +
             0.5*(c_pt2.x_gm - c_pt1.x_gm));
     close_cog.y_gm = curve_cog.y_gm +
       2.0/3.0*(( c_pt1.y_gm - curve_cog.y_gm) +
             0.5*(c_pt2.y_gm - c_pt1.y_gm));
     close_cog.z_gm = curve_cog.z_gm +
       2.0/3.0*(( c_pt1.z_gm - curve_cog.z_gm) +
             0.5*(c_pt2.z_gm - c_pt1.z_gm));

      } /* Curve is not closed, a triangle shall be added   End     */


nocross:; /* Label: Closing triangle is a line                      */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur302 Closing triangle cog %10.4f %10.4f %10.4f area %f\n", 
 close_cog.x_gm , close_cog.y_gm , close_cog.z_gm, close_area );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 2. Calculate area and center of gravity                          */
/* ________________________________________                         */
/*                                                                  */
/*                                                                 !*/

/* Initialize the delta iteration value                             */
   delta_iter = delta;

/* Initialize total number of calculations                          */
   a_cur       = 0;

nxt_area:; /* Label: Next area calculation                          */

/* Initialize the number of points                                  */
   n_tot = 0;

/* Initialize total section area                                    */
   sectarea    = 0.0;

/* Initialize output center of gravity                              */
   p_cog->x_gm = 0.0; 
   p_cog->y_gm = 0.0;
   p_cog->z_gm = 0.0; 
/* Initialize total number of linear segments                       */
   n_tot       = 0;

/* Intitialize arclength which is a sum of straight lines           */
   sum_leng = 0.0;

/*!                                                                 */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Get arclength for the current segment                         */
/*                                                                 !*/

      seg_leng = (p_seg+i_seg-1)->sl;

      n_add = (DBint)(seg_leng/delta_iter);
      if ( n_add < 2  )        n_add = 2;
      else if ( n_add > 5000 ) n_add = 5000;
      delta_par = 1.0/(DBfloat)(n_add+1);

      for ( i_poi = 1; i_poi <= n_add-1; i_poi = i_poi + 1 )
        {
/*!                                                                 */
/*    Curve evaluation for i_poi in segment i_seg                   */
/*    Call of GE109.                                                */
/*                                                                 !*/

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + (i_poi-1)*delta_par;

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

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + i_poi*delta_par;

      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(varkon_erpush("SU2943",errbuf));
        }

      c_pt2 = xyz.r;

/*!                                                                 */
/*    Sum of straight line lengths                                  */
/*                                                                 !*/

      dist = sqrt( 
         (c_pt2.x_gm-c_pt1.x_gm)*(c_pt2.x_gm-c_pt1.x_gm)+
         (c_pt2.y_gm-c_pt1.y_gm)*(c_pt2.y_gm-c_pt1.y_gm)+
         (c_pt2.z_gm-c_pt1.z_gm)*(c_pt2.z_gm-c_pt1.z_gm)  );

      sum_leng = sum_leng + dist;

      n_tot = n_tot + 1;

/*!                                                                 */
/*    Cross product vector for current triangle                     */
/*                                                                 !*/

      u_vec[0] = - curve_cog.x_gm + c_pt1.x_gm;
      u_vec[1] = - curve_cog.y_gm + c_pt1.y_gm;
      u_vec[2] = - curve_cog.z_gm + c_pt1.z_gm;
      v_vec[0] = - curve_cog.x_gm + c_pt2.x_gm;
      v_vec[1] = - curve_cog.y_gm + c_pt2.y_gm;
      v_vec[2] = - curve_cog.z_gm + c_pt2.z_gm;

      status=varkon_xprod (v_vec, u_vec, cross);
      if (status<0) 
        {
        /* Center of gravity is on line start-curve_cog-end point   */
        trian_area = 0.0;
        goto on_line;
        }

/*   Note that the first three components of a_plane is the normal  */
     varkon_scalar(cross, a_plane, &dot_sign);
     if  ( dot_sign < 0.0 ) sign = -1.0;
     else                   sign =  1.0;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur302 Cross product vector %10.4f %10.4f %10.4f sign %3.1f\n", 
 cross[0],cross[1],cross[2], sign);
fflush(dbgfil(SURPAC)); 
}
#endif

/*    Area for current triangle                                     */
      trian_area = sign*0.5 * sqrt( cross[0]*cross[0]+
                                    cross[1]*cross[1]+
                                    cross[2]*cross[2]);

/*    Add to total section area                                     */
      sectarea = sectarea + trian_area;

/*    Center of gravity for current triangle                        */

     trian_cog.x_gm = curve_cog.x_gm +
       2.0/3.0*(( c_pt1.x_gm - curve_cog.x_gm) +
             0.5*(c_pt2.x_gm - c_pt1.x_gm));
     trian_cog.y_gm = curve_cog.y_gm +
       2.0/3.0*(( c_pt1.y_gm - curve_cog.y_gm) +
             0.5*(c_pt2.y_gm - c_pt1.y_gm));
     trian_cog.z_gm = curve_cog.z_gm +
       2.0/3.0*(( c_pt1.z_gm - curve_cog.z_gm) +
             0.5*(c_pt2.z_gm - c_pt1.z_gm));

/*   Add to   output center of gravity                              */
     p_cog->x_gm = p_cog->x_gm + trian_cog.x_gm*trian_area;
     p_cog->y_gm = p_cog->y_gm + trian_cog.y_gm*trian_area;
     p_cog->z_gm = p_cog->z_gm + trian_cog.z_gm*trian_area;

on_line:; /* Label: Closing triangle is a line                      */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur302 Current triangle cog %10.4f %10.4f %10.4f area %f\n", 
 trian_cog.x_gm , trian_cog.y_gm , trian_cog.z_gm, trian_area );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 && fabs(sectarea) > 0.00001)
{
fprintf(dbgfil(SURPAC),
"sur302 cog %10.4f %10.4f %10.4f area %10.1f n_add %4d delta %6.4f\n", 
  p_cog->x_gm/sectarea,p_cog->y_gm/sectarea,p_cog->z_gm/sectarea, 
    sectarea, (int)n_add, delta_iter );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */

        }/* End loop segment points i_poi= 1,2,3                    */
      }  /* End loop curve segments i_seg= 1,2,3                    */

/*!                                                                 */
/*  Add closing triangle                                            */
/*                                                                 !*/

  if  (  fabs(close_area) > comptol )
    {
/*   Add to total section area                                      */
     sectarea = sectarea + close_area;
/*   Add to   output center of gravity                              */
     p_cog->x_gm = p_cog->x_gm + close_cog.x_gm*close_area;
     p_cog->y_gm = p_cog->y_gm + close_cog.y_gm*close_area;
     p_cog->z_gm = p_cog->z_gm + close_cog.z_gm*close_area;
    }


/*!                                                                 */
/*  Output center of gravity and area                               */
/*                                                                 !*/

  if  ( fabs(sectarea) > 100.0*comptol )
    {
     p_cog->x_gm = p_cog->x_gm/sectarea;
     p_cog->y_gm = p_cog->y_gm/sectarea;
     p_cog->z_gm = p_cog->z_gm/sectarea;
    }
  else
    {
    sprintf(errbuf,"sectarea=0%%sur302");
    return(varkon_erpush("SU2993",errbuf));
    }


/*!                                                                 */
/*  Check i calculation shall be remade                             */
/*                                                                 !*/

  a_all[a_cur]  = sectarea;
  a_cur         = a_cur + 1;


  if  ( a_cur == 1 )
    {
    startarea = sectarea;
    if  ( fabs(startarea) < 0.000001 )
      {
       sprintf(errbuf,"(startarea=0)%%sur302");
       return(varkon_erpush("SU2993",errbuf)); 
       }
    }




#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && a_cur == 1 )
{
fprintf(dbgfil(SURPAC),
"sur302 a_cur %d sectarea %25.10f\n",(int)a_cur, sectarea);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && fabs(tot_leng) > 0.0000001 )
{
fprintf(dbgfil(SURPAC),
"sur302 a_cur %d tot_leng %f sum_leng %f diff %f  n_tot %d\n",
     (int)a_cur, tot_leng, sum_leng, (tot_leng-sum_leng)/tot_leng,
      (int)n_tot);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && a_cur >= 2 )
{
fprintf(dbgfil(SURPAC),
"sur302 a_cur %d sectarea %25.10f Difference %f delta_iter %f\n",
    (int)a_cur, sectarea,
  (a_all[a_cur-1]-a_all[a_cur-2])/startarea, delta_iter );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Check if the requested accuracy is reached                       */
/* Recalculate if not ..                                            */
/*                                                                 !*/

  if       (   a_cur == 1 && a_max == 1 ) 
    {
    ; /* End of calculation */
    }
  else if  (   a_cur >= 10  ) 
    {
    ; /* End of calculation */
    }
  else if  (   delta_iter < idpoint  ) 
    {
    ; /* End of calculation */
    }
  else if  ( fabs((tot_leng-sum_leng)/tot_leng) < a_crit )
    {
    ; /* End of calculation */
    }
  else
    {
    delta_iter = delta_iter/2.0;
    goto nxt_area;
    }

  if ( sectarea < 0.0 ) 
    {
    sectarea = -sectarea;
#ifdef DEBUG                             /* Debug printout          */
    if ( dbglev(SURPAC) == 1 )
      {
      fprintf(dbgfil(SURPAC),
      "sur302 Programming WEAKNESS sectarea < 0 \n" );
      fflush(dbgfil(SURPAC)); 
      }
#endif
    }

  *p_sarea = sectarea;

/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur302 p_cog %f %f %f\n",p_cog->x_gm, p_cog->y_gm, p_cog->z_gm  );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur302 Exit varkon_cur_secta *p_sarea %25.10f\n",*p_sarea);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
