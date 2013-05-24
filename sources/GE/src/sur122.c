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
/*  Function: varkon_cur_2dchord                   File: sur122.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create two R*3 curves in the XY plane (2D curves) which         */
/*  are the chord length and chord angle variation between          */
/*  the two input curves.                                           */
/*                                                                  */
/*  It is assumed that the curves are from the same surface         */
/*  (that the curves are two silhouettes for FANGA analysis)        */
/*                                                                  */
/*  Memory area is allocated for the curve segment coefficients.    */
/*  This area must be deallocated by the calling function !         */
/*                                                                  */
/*  Note that spine curve not yet is used .... !                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-11-15   New derivatives                                    */
/*  1995-05-28   Erased unused variables                            */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_2dchord    Fanga chord and angle (2 silh's) */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* GE136                  * Rational cubic interm. pt               */
/* GE133                  * Rational cubic with P-value             */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_cur_2dchord     */
/* SU2993 = Severe program error in varkon_cur_2dchord  (sur122).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_cur_2dchord (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   DBCurve *p_cur1,      /* Curve 1                          (ptr)  */
   DBSeg   *p_seg1,      /* Alloc. area for UV segments      (ptr)  */
   DBCurve *p_cur2,      /* Curve 1                          (ptr)  */
   DBSeg   *p_seg2,      /* Alloc. area for UV segments      (ptr)  */
   DBCurve *p_spine,     /* Spine curve                      (ptr)  */
   DBSeg   *p_spiseg,    /* Alloc. area for UV segments      (ptr)  */
   DBVector angdir,      /* Direction for angle calculation         */
   DBCurve *p_chocur,    /* Chord curve                      (ptr)  */
   DBSeg  **p_choseg,    /* Alloc. area for UV segments      (ptr)  */
   DBCurve *p_angcur,    /* Angle curve                      (ptr)  */
   DBSeg  **p_angseg )   /* Alloc. area for UV segments      (ptr)  */
/*                                                                  */
/* Out:                                                             */
/* Cubic curve coefficients in allocated areas p_choseg & p_angseg  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  no_seg1;       /* No of segments in (UV) surface curve 1  */
   DBint  no_seg2;       /* No of segments in (UV) surface curve 2  */
   DBint  ir;            /* Loop index: Segment in  surface curve   */
   DBfloat  t_l;           /* Local parameter for the surface curve   */
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBint  i_s;           /* Loop index segment record               */
   short  rcode;         /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
   DBfloat  out[16];       /* Array with coordinates and derivatives*/
   DBfloat  uvout[16];     /* Array with UV coord.'s and derivatives*/
   DBfloat  us1,vs1;       /* Segment U,V start point   curve 1     */
   DBfloat  us2,vs2;       /* Segment U,V start point   curve 2     */
   DBfloat  tus1,tvs1;     /* Segment U,V start tangent curve 1     */
   DBfloat  tus2,tvs2;     /* Segment U,V start tangent curve 2     */
   DBfloat  ue1,ve1;       /* Segment U,V end   point   curve 1     */
   DBfloat  ue2,ve2;       /* Segment U,V end   point   curve 2     */
   DBfloat  tue1,tve1;     /* Segment U,V end   tangent curve 1     */
   DBfloat  tue2,tve2;     /* Segment U,V end   tangent curve 2     */
   DBfloat  xs1,ys1,zs1;   /* Segment start point   curve 1         */
   DBfloat  xs2,ys2,zs2;   /* Segment start point   curve 2         */
   DBfloat  xe1,ye1,ze1;   /* Segment end   point   curve 1         */
   DBfloat  xe2,ye2,ze2;   /* Segment end   point   curve 2         */
   DBfloat  txs1,tys1,tzs1;/* Segment start tangent curve 1         */
   DBfloat  txs2,tys2,tzs2;/* Segment start tangent curve 2         */
   DBfloat  txe1,tye1,tze1;/* Segment end   tangent curve 1         */
   DBfloat  txe2,tye2,tze2;/* Segment end   tangent curve 2         */
   DBfloat  xsc,ysc,zsc;   /* Chord length function start point     */
   DBfloat  xec,yec,zec;   /* Chord length function end   point     */
   DBfloat  txsc,tysc,tzsc;/* Chord length function start tangent   */
   DBfloat  txec,tyec,tzec;/* Chord length function end   tangent   */
   DBfloat  xsa,ysa,zsa;   /* Chord angle  function start point     */
   DBfloat  xea,yea,zea;   /* Chord angle  function end   point     */
   DBfloat  txsa,tysa,tzsa;/* Chord angle  function start tangent   */
   DBfloat  txea,tyea,tzea;/* Chord angle  function end   tangent   */
   DBfloat  dots;          /* Scalar product start point            */
   DBfloat  dote;          /* Scalar product end   point            */
   DBfloat  sum_length;    /* Sum of spine segment arc lengths      */
   DBfloat  segm_length;   /* Spine segment length                  */
   DBfloat  spine_s[3];    /* Simplified spine                      */
   DBfloat  dt1da;         /* Derivative dt1/da a is spine length   */
   DBfloat  dt2da;         /* Derivative dt2/da a is spine length   */
   DBVector points[4];     /* points[0]    = start point            */
                           /* points[1]    = start tangent point    */
                           /* points[2]    = end   tangent point    */
                           /* points[3]    = end   point            */
   DBfloat  pvalue;        /* P value for the rational segment      */
   DBSeg  *pdummy;         /* Pointer to output segments            */
   char   errbuf[80];      /* String for error message fctn erpush  */
   DBfloat  vleng;         /* Vector length                         */
   DBVector angd_n;        /* Normalised input angdir               */
   DBint  status;          /* Error code from a called function     */
#ifdef  DEBUG
   DBfloat  dum1;          /* Dummy float                           */
#endif
#ifdef TILLSVIDARE
   DBint  n_all_u;         /* Number of parameter values in all_u   */
   DBfloat  all_u[100];    /* All parameter values, incl. double pts*/
   DBfloat  all_u_s[100];  /* All sorted parameter values           */
   DBVector ipt;           /* Intermediate point                    */
#endif

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* Notations:                                                       */
/*                                                                  */
/* s=       arclength for the spine                                 */
/* u=       parameter for the spine and conic lofting surface       */
/* t1=      parameter for silhouette curve 1                        */
/* t2=      parameter for silhouette curve 2                        */
/* chord=   chord length between silhouette curves                  */
/* dchord=  chord length derivative with respect to s               */
/* angle=   chord angle with respect to direction angdir            */
/* dangle=  chord angle  derivative with respect to s               */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/* x(s)    = s                                                      */
/*                                                                  */
/* chord(s)= ( r1(t1(s)) - r2(t2(s)) )**2                           */
/*                                                                  */
/* angle(s)= ( angd_n*(r1-r2) )**2 / chord(s)                       */
/*                                                                  */
/* dxds    = 1                                                      */
/*                                                                  */
/* dchord  = 2*(r1-r2)*(dr1/ds-dr2/ds)                              */
/*                                                                  */
/* dangle  = (dchord*k(s) - chord*dkds)/chord(s)**2                 */
/*           where                                                  */
/*                                                                  */
/*           k(s)= ( angd_n*(r1-r2) )**2                            */
/*           dkds= 2* angd_n*(r1-r2)* (dr1/ds-dr2/ds)               */
/*           dr1/ds= dr1/dt1 * dt1/du  since r1=r1(t1(u(s)))        */
/*           dr2/ds= dr2/dt2 * dt2/du  since r1=r1(t1(u(s)))        */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur122 Enter ***** varkon_cur_2dchord*Chord length curve\n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur122 Ptr to UV curve segments p_seg1= %d Curve header p_cur1= %d\n",
        p_seg1, p_cur1 );
fprintf(dbgfil(SURPAC),
"sur122 p_seg1->c0x %f c1x %f c2x %f c3x %f\n",
        p_seg1->c0x,p_seg1->c1x,p_seg1->c2x,p_seg1->c3x );
fprintf(dbgfil(SURPAC),
"sur122 p_seg1->c0y %f c1y %f c2y %f c3y %f\n",
        p_seg1->c0y,p_seg1->c1y,p_seg1->c2y,p_seg1->c3y );
fprintf(dbgfil(SURPAC),
"sur122 p_seg1->c0  %f c1  %f c2  %f c3  %f\n",
        p_seg1->c0 ,p_seg1->c1 ,p_seg1->c2 ,p_seg1->c3  );
 if ( p_seg1->typ == CUB_SEG )
fprintf(dbgfil(SURPAC),
"sur122 Type of segment is CUB_SEG \n");
 if ( p_seg1->typ ==  UV_SEG )
fprintf(dbgfil(SURPAC),
"sur122 Type of segment is  UV_SEG \n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif

/*!                                                                 */
/* 1. Initializations                                               */
/* __________________                                               */
/*                                                                  */
/* Initiate curve data in p_chocur and p_angcur.                    */
/* Calls of varkon_ini_gmcur  (sur778).                             */
/*                                                                 !*/
    varkon_ini_gmcur (p_chocur);
    varkon_ini_gmcur (p_angcur);

/*!                                                                 */
/* Number of segments in the input UV curves:                       */
  no_seg1= (p_cur1)->ns_cu;
  no_seg2= (p_cur2)->ns_cu;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 no_seg1= %d no_seg2 %d\n",
    no_seg1 , no_seg2 );
fflush(dbgfil(SURPAC)); 
   }
#endif

  vleng= SQRT(angdir.x_gm*angdir.x_gm +
              angdir.y_gm*angdir.y_gm +
              angdir.z_gm*angdir.z_gm); 

  if ( vleng > 0.000001 )
     {
     angd_n.x_gm = angdir.x_gm/vleng;
     angd_n.y_gm = angdir.y_gm/vleng;
     angd_n.z_gm = angdir.z_gm/vleng;
     }
  else
    {
    sprintf(errbuf,"(angdir=0)%%varkon_cur_2dchord ");
    return(varkon_erpush("SU2993",errbuf));
    }

#ifdef TODO_SOMETHING
/*                                                                  */
/* 2. Array with U values for the output curve                      */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                  */

n_all_u = 0;




for ( ir=0; ir<= no_seg1-1; ++ir )       /* Start loop UV curve     */
  {
  n_all_u = n_all_u + 1;
  if ( n_all_u >  99 )
    {
    sprintf(errbuf,"(n_all_u > 100)%%varkon_cur_2dchord ");
    return(varkon_erpush("SU2993",errbuf));
    }
  all_u[n_all_u-1]= (p_seg1+ir)->c0x/(p_seg1+ir)->c0;
  }

n_all_u = n_all_u + 1;
all_u[n_all_u-1]=((p_seg1+ir)->c0x +
                  (p_seg1+ir)->c1x +
                  (p_seg1+ir)->c2x +
                  (p_seg1+ir)->c3x ) /
                 ((p_seg1+ir)->c0  +
                  (p_seg1+ir)->c1  +
                  (p_seg1+ir)->c2  +
                  (p_seg1+ir)->c3  ); 

for ( ir=0; ir<= no_seg2-1; ++ir )       /* Start loop UV curve     */
  {
  n_all_u = n_all_u + 1;
  if ( n_all_u >  99 )
    {
    sprintf(errbuf,"(n_all_u > 100)%%varkon_cur_2dchord ");
    return(varkon_erpush("SU2993",errbuf));
    }
  all_u[n_all_u-1]= (p_seg2+ir)->c0x/(p_seg2+ir)->c0;
  }

n_all_u = n_all_u + 1;
all_u[n_all_u-1]=((p_seg2+ir)->c0x +
                  (p_seg2+ir)->c1x +
                  (p_seg2+ir)->c2x +
                  (p_seg2+ir)->c3x ) /
                 ((p_seg2+ir)->c0  +
                  (p_seg2+ir)->c1  +
                  (p_seg2+ir)->c2  +
                  (p_seg2+ir)->c3  ); 


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 n_all_u= %d \n", n_all_u );
fflush(dbgfil(SURPAC)); /* From buffer to file */
   }
#endif

/* Sortering och borttagande av samma v{rden .. se sur800           */
/* Sedan loop och sk{rning f|r att f} samma u v{rden                */
/* Tills vidare f|ruts{tts samma antal segment med "samma" uv{rden  */
/* ..  END_TODO_SOMETHING                                           */
#endif


  if ( no_seg1 != no_seg2 ) /* !!!!! TODO Temporary !!!!   */
    {
    sprintf(errbuf,"(no_seg1 ne no_seg2)%%varkon_cur_2dchord ");
    return(varkon_erpush("SU2993",errbuf));
    }


/*!                                                                 */
/* 2. Allocation of memory for the output curves                    */
/* _____________________________________________                    */
/*                                                                  */
/*                                                                 !*/



/*!                                                                 */
/* Allocation of memory for no_seg1 segments.                       */
/* Calls of DBcreate_segments.                                      */
/*                                                                 !*/
if((*p_choseg = DBcreate_segments(no_seg1))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_sur_csur_gra(sur122");
 return(varkon_erpush("SU2993",errbuf));
 }
if((*p_angseg = DBcreate_segments(no_seg1))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_sur_csur_gra(sur122");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Initiate segment data in p_choseg and p_angseg.                  */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<=no_seg1; i_s= i_s+1)
     {
    varkon_ini_gmseg (*p_choseg+i_s-1);
    varkon_ini_gmseg (*p_angseg+i_s-1);
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 Start adresses *p_choseg= %d *p_angseg %d no_seg1= %d\n",
    *p_choseg, *p_angseg, no_seg1 );
   fflush(dbgfil(SURPAC)); 
   }
#endif

/*!                                                                 */
/* 3. Create output R*3 curve                                       */
/* __________________________                                       */
/*                                                                  */
/*                                                                 !*/

   sum_length= 0.0;      /* Sum of spine segment arc lengths        */

/*!                                                                 */
/* Loop all segments in the curve   ir=0,1,....,no_seg1-1           */
/*                                                                 !*/

for ( ir=0; ir<= no_seg1-1; ++ir )       /* Start loop UV curve     */
  {

/*!                                                                 */
/*   Point and derivatives for t_l= 0 and t_l= 1                    */
/*   and (mid) point for t_l= 0.5 of segment i_r.                   */
/*   Calls of (surface) curve evaluation function GE107.            */
/*                                                                 !*/

rcode=1;         /* Only necessary to calculate first derivatives   */
                 /* Refer to varkon_sur_cureval (sur950)            */

t_l = 0.0;

   status=GE107 ((DBAny  *)p_cur1,p_seg1+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

      xs1  = out[0]; 
      ys1  = out[1]; 
      zs1  = out[2]; 
      txs1 = out[3]; 
      tys1 = out[4]; 
      tzs1 = out[5]; 

    (p_seg1+ir)->typ = CUB_SEG;

status=GE107 ((DBAny  *)p_cur1,p_seg1+ir,t_l,3,uvout);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

    (p_seg1+ir)->typ = UV_SEG;

      us1  = uvout[0]; 
      vs1  = uvout[1]; 
      tus1 = uvout[3]; 
      tvs1 = uvout[4]; 


status=GE107 ((DBAny  *)p_cur2,p_seg2+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

      xs2  = out[0]; 
      ys2  = out[1]; 
      zs2  = out[2]; 
      txs2 = out[3]; 
      tys2 = out[4]; 
      tzs2 = out[5]; 

    (p_seg2+ir)->typ = CUB_SEG;

status=GE107 ((DBAny  *)p_cur1,p_seg2+ir,t_l,3,uvout);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

    (p_seg2+ir)->typ = UV_SEG;

      us2  = uvout[0]; 
      vs2  = uvout[1]; 
      tus2 = uvout[3]; 
      tvs2 = uvout[4]; 



#ifdef TODO_ADD_MID_POINT

t_l = 0.5;

status=GE107 ((DBAny  *)p_cur1,p_seg1+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

      ipt.x_gm  = out[0]; 
      ipt.y_gm  = out[1]; 
      ipt.z_gm  = out[2]; 
#endif /* TODO_ADD_MID_POINT */





t_l = 1.0;

status=GE107 ((DBAny  *)p_cur1,p_seg1+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

      xe1  = out[0]; 
      ye1  = out[1]; 
      ze1  = out[2]; 
      txe1 = out[3]; 
      tye1 = out[4]; 
      tze1 = out[5]; 

    (p_seg1+ir)->typ = CUB_SEG;

status=GE107 ((DBAny  *)p_cur1,p_seg1+ir,t_l,3,uvout);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

    (p_seg1+ir)->typ = UV_SEG;

      ue1  = uvout[0]; 
      ve1  = uvout[1]; 
      tue1 = uvout[3]; 
      tve1 = uvout[4]; 


status=GE107 ((DBAny  *)p_cur2,p_seg2+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

      xe2  = out[0]; 
      ye2  = out[1]; 
      ze2  = out[2]; 
      txe2 = out[3]; 
      tye2 = out[4]; 
      tze2 = out[5]; 

    (p_seg2+ir)->typ = CUB_SEG;

status=GE107 ((DBAny  *)p_cur1,p_seg2+ir,t_l,3,uvout);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2943",errbuf));
        }

    (p_seg2+ir)->typ = UV_SEG;

      ue2  = uvout[0]; 
      ve2  = uvout[1]; 
      tue2 = uvout[3]; 
      tve2 = uvout[4]; 


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  us1 %f vs1 %f tus1 %f tvs1 %f\n", us1, vs1, tus1, tvs1);
  fprintf(dbgfil(SURPAC),
  "sur122  ue1 %f ve1 %f tue1 %f tve1 %f\n", ue1, ve1, tue1, tve1);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  us2 %f vs2 %f tus2 %f tvs2 %f\n", us2, vs2, tus2, tvs2);
  fprintf(dbgfil(SURPAC),
  "sur122  ue2 %f ve2 %f tue2 %f tve2 %f\n", ue2, ve2, tue2, tve2);
   fflush(dbgfil(SURPAC));
   }
#endif

/*                                                                  */
/*   Create an output rational cubic segment i_r with the           */
/*   calculated start and end points and tangents and the           */
/*   mid point. Call of GE136.                                      */
/*   If GE136 fails let the mid point cubic rational segment be     */
/*   replaced by a parabola. Let p-value= 0.5 and call GE133.       */
/*                                                                  */

/*!                                                                 */
/*   Create an output rational       segment i_r with the           */
/*   calculated start and end points and tangents and               */
/*   P-value= 0.5. Call of GE133.                                   */
/*                                                                 !*/

/*   Chord length function with respect to parameter t_l            */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  xsc,  ysc,  zsc %f %f %f\n", xsc, ysc, zsc);
  fprintf(dbgfil(SURPAC),
  "sur122 txsc, tysc, tzsc %f %f %f\n", txsc, tysc, tzsc);
  fprintf(dbgfil(SURPAC),
  "sur122  xec,  yec,  zec %f %f %f\n", xec, yec, zec);
  fprintf(dbgfil(SURPAC),
  "sur122 txec, tyec, tzec %f %f %f\n", txec, tyec, tzec);
   fflush(dbgfil(SURPAC)); /* From buffer to file */
   }
#endif

/*   Chord length function with respect to spine arclength          */

     if ( p_spine == NULL  )
        {
        segm_length=     /* Spine segment length                    */
          SQRT((xs1-xe1)*(xs1-xe1)+
               (ys1-ye1)*(ys1-ye1)+
               (zs1-ze1)*(zs1-ze1));
        /* Simplified spine                        */
        spine_s[0] = xe1-xs1;
        spine_s[1] = ye1-ys1;
        spine_s[2] = ze1-zs1;
        }
      else
        {
        sprintf(errbuf,"(spine)%%varkon_cur_2dchord ");
        return(varkon_erpush("SU2993",errbuf));
        }

      xsc   = sum_length;
      ysc   = (xs1-xs2)*(xs1-xs2)+
              (ys1-ys2)*(ys1-ys2)+
              (zs1-zs2)*(zs1-zs2);
      zsc   = 0.0;

      dt1da = txs1*spine_s[0]+tys1*spine_s[1]+tzs1*spine_s[2];  
      dt2da = txs2*spine_s[0]+tys2*spine_s[1]+tzs2*spine_s[2];  
      dt1da = dt1da/segm_length
              /SQRT(txs1*txs1+tys1*tys1+tzs1*tzs1);
      dt2da = dt2da/segm_length
              /SQRT(txs2*txs2+tys2*tys2+tzs2*tzs2);
#ifdef TEST 
      if ( fabs(dt1da) > 0.0000000001 )
      dt1da = 1.0/dt1da;
      else dt2da = 50000.0;
      if ( fabs(dt2da) > 0.0000000001 )
      dt2da = 1.0/dt2da;
      else dt1da = 50000.0;
#endif
      txsc  = segm_length;
      tysc  = 2*((txs1*dt1da-txs2*dt1da)*(xs1-xs2)+
                 (tys1*dt1da-tys2*dt1da)*(ys1-ys2)+
                 (tzs1*dt1da-tzs2*dt1da)*(zs1-zs2));
      tzsc  = 0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  dt1da,  dt2da %f %f\n", dt1da, dt2da);
   fflush(dbgfil(SURPAC)); /* From buffer to file */
   }
#endif

      xec   = sum_length + segm_length;
      yec   = (xe1-xe2)*(xe1-xe2)+
              (ye1-ye2)*(ye1-ye2)+
              (ze1-ze2)*(ze1-ze2);
      zec   = 0.0;

      dt1da = txe1*spine_s[0]+tye1*spine_s[1]+tze1*spine_s[2];  
      dt2da = txe2*spine_s[0]+tye2*spine_s[1]+tze2*spine_s[2];  
      dt1da = dt1da/segm_length
              /SQRT(txe1*txe1+tye1*tye1+tze1*tze1);
      dt2da = dt2da/segm_length
              /SQRT(txe2*txe2+tye2*tye2+tze2*tze2);
#ifdef TESTA
      if ( fabs(dt1da) > 0.0000000001 )
      dt1da = 1.0/dt1da;
      else dt1da = 50000.0;
      if ( fabs(dt2da) > 0.0000000001 )
      dt2da = 1.0/dt2da;
      else dt2da = 50000.0;
#endif
      txec  = segm_length;
      tyec  = 2*((txe1*dt1da-txe2*dt1da)*(xe1-xe2)+
                 (tye1*dt1da-tye2*dt1da)*(ye1-ye2)+
                 (tze1*dt1da-tze2*dt1da)*(ze1-ze2));
      tzec  = 0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  dt1da,  dt2da %f %f\n", dt1da, dt2da);
   fflush(dbgfil(SURPAC));
   }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  segm_length %f yec-ysc %f\n",segm_length , yec-ysc );
   fflush(dbgfil(SURPAC));
   }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  (yec-ysc)/segment_length %f\n",(yec-ysc)/segm_length );
   fflush(dbgfil(SURPAC));
   }
#endif


      xsa   = xsc;
      dots  = angd_n.x_gm*(xs1-xs2)+
              angd_n.y_gm*(ys1-ys2)+
              angd_n.z_gm*(zs1-zs2);
      if ( ysc > 0.00000001 )
        {
        ysa = dots*dots / ysc;
        }
      zsa   = 0.0;

      txsa  = segm_length;
      tysa  = tysc* dots*dots - ysc* 2*dots*
                    (angd_n.x_gm*(txs1*dt1da-txs2*dt2da)+
                     angd_n.y_gm*(tys1*dt1da-tys2*dt2da)+
                     angd_n.z_gm*(tzs1*dt1da-tzs2*dt2da));
      if ( ysc > 0.00000001 )
        {
        tysa  = tysa/ysc/ysc;
        }
      tzsa  = 0.0;

      xea   = xec;
      dote  = angd_n.x_gm*(xe1-xe2)+
              angd_n.y_gm*(ye1-ye2)+
              angd_n.z_gm*(ze1-ze2);
      if ( yec > 0.00000001 )
        {
        yea = dote*dote / yec;
        }
      zea   = 0.0;

      txea  = segm_length;
      tyea  = tyec* dote*dote - yec* 2*dote*
                    (angd_n.x_gm*(txe1*dt1da-txe2*dt2da)+
                     angd_n.y_gm*(tye1*dt1da-tye2*dt2da)+
                     angd_n.z_gm*(tze1*dt1da-tze2*dt2da));
      if ( yec > 0.00000001 )
        {
        tyea  = tyea/yec/yec;
        }
      tzea  = 0.0;

      sum_length = sum_length + segm_length;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  dots %25.10f dots/SQRT(ysc) %25.10f\n", 
           dots,        dots/SQRT(ysc) );
  fprintf(dbgfil(SURPAC),
  "sur122  dote %25.10f dote/SQRT(yec) %25.10f\n", 
           dote,        dote/SQRT(yec) );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  xs1,  ys1,  zs1 %f %f %f\n", xs1, ys1, zs1);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 txs1, tys1, tzs1 %f %f %f\n", txs1, tys1, tzs1);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  xe1,  ye1,  ze1 %f %f %f\n", xe1, ye1, ze1);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 txe1, tye1, tze1 %f %f %f\n", txe1, tye1, tze1);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 tus1*(txs1 tys1 tzs1) %f %f %f\n",txs1*tus1,tys1*tus1,tzs1*tus1);
  fprintf(dbgfil(SURPAC),
  "sur122 tue1*(txe1 tye1 tze1) %f %f %f\n",txe1*tue1,tye1*tue1,tze1*tue1);
   }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  xs2,  ys2,  zs2 %f %f %f\n", xs2, ys2, zs2);
  fprintf(dbgfil(SURPAC),
  "sur122 txs2, tys2, tzs2 %f %f %f\n", txs2, tys2, tzs2);
  fprintf(dbgfil(SURPAC),
  "sur122  xe2,  ye2,  ze2 %f %f %f\n", xe2, ye2, ze2);
  fprintf(dbgfil(SURPAC),
  "sur122 txe2, tye2, tze2 %f %f %f\n", txe2, tye2, tze2);
   }
if ( dbglev(SURPAC) == 2 )
  {
  dum1 = SQRT(txs1*txs1 + tys1*tys1 + tzs1*tzs1); 
  fprintf(dbgfil(SURPAC),
  "sur122 txs1 tys1 tzs1 %f %f %f\n",txs1/dum1 ,tys1/dum1 ,tzs1/dum1 );
  dum1 = SQRT(txe1*txe1 + tye1*tye1 + tze1*tze1); 
  fprintf(dbgfil(SURPAC),
  "sur122 txe1 tye1 tze1 %f %f %f\n",txe1/dum1 ,tye1/dum1 ,tze1/dum1 );
  }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122  xsc,  ysc %f %f\n", xsc, ysc);
  fprintf(dbgfil(SURPAC),
  "sur122 txsc, tysc %f %f\n", txsc/txsc, tysc/txsc);
  fprintf(dbgfil(SURPAC),
  "sur122  xec,  yec %f %f\n", xec, yec);
  fprintf(dbgfil(SURPAC),
  "sur122 txec, tyec %f %f\n", txec/txec, tyec/txec);
  fprintf(dbgfil(SURPAC),
  "sur122  (xec-xsc)/segm_length %f %f\n", 
 (xec-xsc)/segm_length,  (yec-ysc)/segm_length);
   fflush(dbgfil(SURPAC)); 
   }
#endif

/*   Create parabola                                                */

      points[0].x_gm = xsc;
      points[0].y_gm = ysc;
      points[0].z_gm = zsc;
      points[1].x_gm = xsc + 10*txsc;
      points[1].y_gm = ysc + 10*tysc;
      points[1].z_gm = zsc + 10*tzsc;
      points[2].x_gm = xec + 10*txec;
      points[2].y_gm = yec + 10*tyec;
      points[2].z_gm = zec + 10*tzec;
      points[3].x_gm = xec;
      points[3].y_gm = yec;
      points[3].z_gm = zec;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 points(0) %f %f %f\n",
   points[0].x_gm , points[0].y_gm , points[0].z_gm );
  fprintf(dbgfil(SURPAC),
  "sur122 points(1) %f %f %f\n",
   points[1].x_gm , points[1].y_gm , points[1].z_gm );
  fprintf(dbgfil(SURPAC),
  "sur122 points(2) %f %f %f\n",
   points[2].x_gm , points[2].y_gm , points[2].z_gm );
  fprintf(dbgfil(SURPAC),
  "sur122 points(3) %f %f %f\n",
   points[3].x_gm , points[3].y_gm , points[3].z_gm );
   fflush(dbgfil(SURPAC)); /* From buffer to file */
   }
#endif


      pvalue         = 0.5;

      pdummy = *p_choseg + ir;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur122 Input DBSeg ptr GE133 pdummy= %d for segment %d\n",
   pdummy, ir+1 );
   fflush(dbgfil(SURPAC)); /* From buffer to file */
   }
#endif


#ifdef TODO_ADD_MID_POINT
status=GE136 (points,&ipt,pdummy );
#endif



status=GE133 (points,pvalue,pdummy );
      if (status<0) 
         {
         sprintf(errbuf,"GE133%%varkon_cur_2dchord ");
         return(varkon_erpush("SU2943",errbuf));
         }


         (*pdummy).sl  = 0.0;     


/*   Create parabola for chord angle                                */

      points[0].x_gm = xsa;
      points[0].y_gm = ysa;
      points[0].z_gm = zsa;
      points[1].x_gm = xsa + 10*txsa;
      points[1].y_gm = ysa + 10*tysa;
      points[1].z_gm = zsa + 10*tzsa;
      points[2].x_gm = xea + 10*txea;
      points[2].y_gm = yea + 10*tyea;
      points[2].z_gm = zea + 10*tzea;
      points[3].x_gm = xea;
      points[3].y_gm = yea;
      points[3].z_gm = zea;

      pvalue         = 0.5;

      pdummy = *p_angseg + ir;

status=GE133 (points,pvalue,pdummy );
      if (status<0) 
         {
         sprintf(errbuf,"GE133%%varkon_cur_2dchord ");
         return(varkon_erpush("SU2943",errbuf));
         }

         (*pdummy).sl  = 0.0;     



  }                                      /* End   loop UV curve     */


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/
 

/*!                                                                 */
/* Number of segments in the output R*3 curves:                     */
   p_chocur->ns_cu    = (short)no_seg1;
   p_chocur->plank_cu = FALSE; /* Temporary   */
   p_angcur->ns_cu = (short)no_seg1;
   p_angcur->plank_cu = FALSE; /* Temporary   */
/*                                                                 !*/
   p_chocur->al_cu    = 0.0;
   p_angcur->al_cu    = 0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur122 Exit *** varkon_cur_2dchord  Number of R*3 curve segments %d\n",
   no_seg1 );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
