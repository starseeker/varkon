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
/*  Function: varkon_sur_scur_gra                  File: sur960.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create an approximate R*3 curve of an (exact) UV surface        */
/*  curve. The R*3 curve is used by the graphical routines          */
/*  The function can also be used for CUB_SEG segments.             */
/*                                                                  */
/*  Memory area is allocated for the curve segment coefficients.    */
/*  This area must be deallocated by the calling function!          */
/*                                                                  */
/*  There might be missing segments in the output graphical         */
/*  curve e.g the curve will not be continuous in position.         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-10-30   Missing segments in output curve                   */
/*  1994-11-23   Also (mixed or only) CUB_SEG segments              */
/*  1995-03-02   Initialize pp_graseg= NULL and                     */
/*               check surface (temporary fix surface               */
/*               should be checked more instead)                    */
/*  1995-03-08   Check of p_seg->spek_gm for UV_CUB_SEG                 */
/*  1995-05-16   Comments added                                     */
/*  1995-11-03   Debug    added                                     */
/*  1996-02-23   Bug when first segment is missing                  */
/*  1996-11-03   Add segments (with sur722)                         */
/*  1997-12-07   Error messages, Debug printout                     */
/*  1997-12-13   Return if sur722 fails                             */
/*  1998-01-06   Debug                                              */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_scur_gra   Approx. UV curve for graphics    */
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
/* varkon_ini_gmseg       * Initiate DBSeg                          */
/* varkon_cur_approx      * Approximate a curve                     */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_idangle         * Identical angles criterion              */
/* GE136                  * Rational cubic interm. pt               */
/* GE133                  * Rational cubic with P-value             */
/* varkon_sur_scur_mbs    * Write curve as MBS for Debug On         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_scur_gra    */
/* SU2993 = Severe program error in varkon_sur_scur_gra (sur960).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_scur_gra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Alloc. area for U,V segments      (ptr) */
   DBSeg  **pp_graseg )  /* Alloc. area for graphic segments  (ptr) */
/*                                                                  */
/* Out:                                                             */
/* Rational cubic curve (coefficients) in allocated area pp_graseg  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    no_seg;      /* No of segments in (UV)  surface curve   */
   DBint    no_seg_out;  /* No of segments in (R3)  output  curve   */
   DBint    ir;          /* Loop index: Segment in  surface curve   */
   DBfloat  t_l;         /* Local parameter for the surface curve   */
   DBint    n_check;     /* Number of check points in segment       */
   DBint    i_p;         /* Loop index: Point in segment            */
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBint    i_s;         /* Loop index segment record               */
   DBVector ipt;         /* Intermediate point                      */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
   DBfloat  out[16];     /* Array with coordinates and derivatives  */
   DBfloat  x1,y1,z1;    /* Segment start point   1                 */
   DBfloat  x2,y2,z2;    /* Segment end   point   2                 */
   DBfloat  tx1,ty1,tz1; /* Segment start tangent 1                 */
   DBfloat  tx2,ty2,tz2; /* Segment end   tangent 2                 */
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   DBfloat  pvalue;      /* P value for the rational segment        */
   DBSeg   *pdummy;      /* Pointer to output segments              */


  DBCurve   appcur;      /* Approximate curve                       */
  DBSeg    *p_appseg;    /* Approximate curve segments       (ptr)  */
  DBfloat   idpoint;     /* Identical point criterion               */
  DBfloat   idangle;     /* Identical angle criterion               */

                         /* For the curve approximation function:   */
  DBint     c_type;      /* Type of calculation                     */
                         /* Eq. 1: No of output segments is input   */
                         /* Eq. 2: Add only segments                */
                         /* Eq. 3: Try to reduce no of segments     */
  DBint     n_req;       /* Requested number for c_type             */
  DBint     t_incur;     /* Type of input curve (what to use)       */
                         /* Eq. 1: Points (segment end points)      */
                         /* Eq. 2: Lines                            */
                         /* Eq. 3: Points and tangents              */
                         /* Eq. 4: Points, tangents and curvature   */

  DBint     a_case;      /* Approximation case                      */
                         /* Eq. 1: Approximate to LINE   segments   */
                         /* Eq. 2: Approximate to CUBIC  segments   */
                         /* Eq. 3: Approximate to CIRCLE segments   */
                         /* Eq. 4: Approximate to CONIC  segments   */
                         /* Eq. 5: Approximate to RATCUB segments   */

  DBfloat   d_che;       /* Arclength check distance                */

                         /* Input tolerances ( < 0 ==> default )    */
  DBfloat   in_ctol;     /* Coordinate       tolerance              */
  DBfloat   in_ntol;     /* Tangent          tolerance (degrees)    */
  DBfloat   in_rtol;     /* Curvature radius tolerance              */

   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

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
"sur960 Enter*varkon_sur_scur_gra *Graphic curve from UV crv*\n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 Ptr to UV curve segments p_seg= %d Curve header p_cur= %d\n",
        (int)p_seg, (int)p_cur );
fprintf(dbgfil(SURPAC),
"sur960 p_seg->c0x %f c1x %f c2x %f c3x %f\n",
        p_seg->c0x,p_seg->c1x,p_seg->c2x,p_seg->c3x );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* This function shall in the future only exist of a call to the    */
/* curve approximation routine varkon_cur_approx (sur722)           */
/* In a temporary step will this function be called and if it       */
/* fails will the original algorithm (below) be used.               */
/*                                                                 !*/

/*!                                                                 */
/* Initialize pp_graseg to NULL. Variable pp_graseg is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_graseg not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_graseg = NULL;

/*!                                                                 */
/* Retrieve idpoint and idangle......                               */
/* .. to be replaced by special graphic tolerances ... !!! ...      */
/*                                                                 !*/

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();

     a_case   =      5;  
     c_type   =      2;
     n_req    =     15;  /* Number of check points for d_che< 0 */
     t_incur  =      3;
     in_ctol  =   10.0*idpoint; /* Implement graphic tolerances */
     in_ntol  =   1000.0*idangle; /* Implement graphic tolerances */
     in_rtol  =  F_UNDEF;
     d_che    =  F_UNDEF;
     p_appseg = NULL;

status = varkon_cur_approx
  (p_cur, p_seg, c_type, n_req, t_incur, 
   a_case, in_ctol, in_ntol, in_rtol , d_che, 
   &appcur, &p_appseg );
  if ( p_appseg  != NULL ) *pp_graseg = p_appseg;

  if ( status < 0  ) 
  {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 sur 722 failed status= %d. Try old (this) routine\n",
  (int)status );
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"sur722%%sur960");
        return(varkon_erpush("SU2973",errbuf));
  }

/*!                                                                 */
/* Number of segments in the output graphic R*3 curve:              */
   p_cur->nsgr_cu = appcur.ns_cu;
/*                                                                 !*/

   goto  slut;

/*!                                                                 */
/* 1. Initializations                                               */
/* __________________                                               */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Number of segments in the input UV curve:                        */
  no_seg= (p_cur)->ns_cu;
/* Number of check points (temporary fix .. )                       */
  n_check = 5;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Prior to allocation  *pp_graseg= %d no_seg= %d\n",
    (int)*pp_graseg, (short)no_seg );
fflush(dbgfil(SURPAC)); 
   }
#endif



/*!                                                                 */
/* Allocation of memory for no_seg segments. Call of DBcreate_segments.        */
/*                                                                 !*/
if ( *pp_graseg  == NULL )
{
if((*pp_graseg = DBcreate_segments(no_seg))==NULL)
 {                                    
 sprintf(errbuf, "(alloc)%%sur960");
 return(varkon_erpush("SU2993",errbuf));
 }                                    
}  /* End *pp_graseg == NULL */

/*!                                                                 */
/* Initialize segment data in pp_graseg.                            */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= no_seg; i_s= i_s+1)
     {
    varkon_ini_gmseg ( *pp_graseg+i_s-1);
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Start adress for allocated area *pp_graseg= %d no_seg= %d\n",
    (int)*pp_graseg, (short)no_seg );
fflush(dbgfil(SURPAC));
   }
#endif

/*!                                                                 */
/* Initialize number of output segments no_seg_out= 0               */
/*                                                                 !*/

   no_seg_out= 0;

/*!                                                                 */
/* 2. Create output R*3 curve                                       */
/* __________________________                                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop all segments in the curve   ir=0,1,....,no_seg-1            */
/*                                                                 !*/

for ( ir=0; ir<= no_seg-1; ++ir )        /* Start loop UV curve     */
  {

  if ( (p_seg+ir)->typ == CUB_SEG )
     {
     pdummy = *pp_graseg + no_seg_out;
     *pdummy = *(p_seg+ir);
     goto r3seg;
     }

/*!                                                                 */
/*   Check (p_seg+ir)->spek_gm for Debug On                         */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d DB-pointer to surface (p_seg+ir)->spek_gm= %d  \n",
    (short)ir,(int)(p_seg+ir)->spek_gm);     
   fflush(dbgfil(SURPAC)); 
   }
     if ( (p_seg+ir)->spek_gm ==  0   )
        {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Error (p_seg+ir)->spek_gm= %d ( 0   for UV_CUB_SEG) \n",
    (int)(p_seg+ir)->spek_gm);     
   fflush(dbgfil(SURPAC)); 
   }
        sprintf(errbuf,"spek_gm =  0%%sur960");
        return(varkon_erpush("SU2993",errbuf));
        }
#endif

/*!                                                                 */
/*   Point and derivatives for t_l= 0 and t_l= 1                    */
/*   and (mid) point for t_l= 0.5 of segment i_r.                   */
/*   Calls of (surface) curve evaluation function GE107.           */
/*                                                                 !*/

rcode=1;         /* Only necessary to calculate first derivatives   */
                 /* Refer to varkon_sur_cureval (sur950)            */

t_l = 0.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d Call of GE107 for t_l %f\n", (short)ir,  t_l );
  fflush(dbgfil(SURPAC)); 
  }
#endif


status=GE107 ((DBAny*)p_cur,p_seg+ir,t_l,rcode,out);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE107 failed ir= %d t_l= %f. Segment will be missing\n",
                      (short)ir  ,  t_l);
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d DB-pointer to surface (p_seg+ir)->spek_gm= %d  \n",
    (short)ir,(int)(p_seg+ir)->spek_gm);     
fflush(dbgfil(SURPAC));
}
#endif
        goto sfail;
        }

      x1  = out[0]; 
      y1  = out[1]; 
      z1  = out[2]; 
      tx1 = out[3]; 
      ty1 = out[4]; 
      tz1 = out[5]; 

t_l = 0.5;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d Call of GE107 for t_l %f\n", (short)ir, t_l );
  fflush(dbgfil(SURPAC)); /* From buffer to file */
  }
#endif

status=GE107 ((DBAny*)p_cur,p_seg+ir,t_l,rcode,out);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE107 failed ir= %d t_l= %f. Segment will be missing\n",
                      (short)ir  ,  t_l);
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d DB-pointer to surface (p_seg+ir)->spek_gm= %d  \n",
    (short)ir,(int)(p_seg+ir)->spek_gm);     
fflush(dbgfil(SURPAC));
}
#endif
        goto sfail;
        }

      ipt.x_gm  = out[0]; 
      ipt.y_gm  = out[1]; 
      ipt.z_gm  = out[2]; 

t_l = 1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d Call of GE107 for t_l %f\n", (short)ir,t_l );
  fflush(dbgfil(SURPAC));
  }
#endif

status=GE107 ((DBAny*)p_cur,p_seg+ir,t_l,rcode,out);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE107 failed ir= %d t_l= %f. Segment will be missing\n",
                      (short)ir  ,  t_l);
  fprintf(dbgfil(SURPAC),
  "sur960 Segment %d DB-pointer to surface (p_seg+ir)->spek_gm= %d \n",
    (short)ir,(int)(p_seg+ir)->spek_gm);     
fflush(dbgfil(SURPAC)); 
}
#endif
        goto sfail;
        }

      x2  = out[0]; 
      y2  = out[1]; 
      z2  = out[2]; 
      tx2 = out[3]; 
      ty2 = out[4]; 
      tz2 = out[5]; 

/*!                                                                 */
/*   Create an output rational cubic segment i_r with the           */
/*   calculated start and end points and tangents and the           */
/*   mid point. Call of GE136.                                      */
/*   If GE136 fails let the mid point cubic rational segment be     */
/*   replaced by a parabola. Let p-value= 0.5 and call GE133.       */
/*   Increase number of output segments no_seg_out= no_seg_out+1    */
/*                                                                 !*/

      points[0].x_gm = x1;
      points[0].y_gm = y1;
      points[0].z_gm = z1;
      points[1].x_gm = x1+10.0*tx1;
      points[1].y_gm = y1+10.0*ty1;
      points[1].z_gm = z1+10.0*tz1;
      points[2].x_gm = x2+10.0*tx2;
      points[2].y_gm = y2+10.0*ty2;
      points[2].z_gm = z2+10.0*tz2;
      points[3].x_gm = x2;
      points[3].y_gm = y2;
      points[3].z_gm = z2;
      pvalue         = 0.5;

      pdummy = *pp_graseg + no_seg_out;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur960 Input DBSeg ptr GE136 (GE133) pdummy= %d for segment %d\n",
 (int)pdummy, (short)ir+1 );
 fflush(dbgfil(SURPAC));
}
#endif


status=GE136 (points,&ipt,pdummy );
 if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE136 (intermed. pt) failed. GE133 creates parabola\n");
fflush(dbgfil(SURPAC)); 
}
#endif
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur960 Tillf{lligt !!!  ptr    GE133  pdummy= %d for segment %d\n",
   (int)pdummy, (short)(ir+1) );
   fflush(dbgfil(SURPAC)); 
   }
#endif
status=GE133 (points,pvalue,pdummy );
      if (status<0) 
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE133 (P value pt) failed. Curve segment will be missing\n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif
        goto sfail;
        }
     }

/*!                                                                 */
/*   Check surface for points within segment (temporary fix ... )   */
/*                                                                 !*/


for ( i_p=1; i_p<= n_check; ++i_p )  
  {

   t_l = i_p/((DBfloat)n_check+1.0);

status=GE107 ((DBAny*)p_cur,p_seg+ir,t_l,rcode,out);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 GE107 failed ir= %d t_l= %f i_p= %d. Segment will be missing\n",
                      (short)ir  ,  t_l , (short)i_p);
fflush(dbgfil(SURPAC)); 
}
#endif
        goto sfail_t;
        }
   }


         (*pdummy).sl  = 0.0;     

r3seg:  /*! Label r3seg: A CUB_SEG segment                 !*/


         no_seg_out= no_seg_out + 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 An additional output segment no_seg_out %d ir %d pdummy %d\n",
              (short)no_seg_out  , (short)ir,  (int)pdummy );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "CUR_CONIC(#%d,QFREEQ,VEC(%f,%f,%f),\n", (short)ir+1,
   points[0].x_gm,points[0].y_gm,points[0].z_gm );
  fprintf(dbgfil(SURPAC),
  "                    VEC(%f,%f,%f), %f ,\n",
   points[1].x_gm,points[1].y_gm,points[1].z_gm,pvalue );
  fprintf(dbgfil(SURPAC),
  "                    VEC(%f,%f,%f), \n",
   points[3].x_gm,points[3].y_gm,points[3].z_gm );
  fprintf(dbgfil(SURPAC),
  "                    VEC(%f,%f,%f));\n",
   points[2].x_gm,points[2].y_gm,points[2].z_gm );
   fflush(dbgfil(SURPAC)); /* From buffer to file */
   }

#endif

sfail:; /*! Label sfail: Failure creating graphical segment         */

  }                                      /* End   loop UV curve     */


/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/
 
sfail_t:; /* Label sfail: Can the graphic routines handle steps ?   */

/*!                                                                 */
/* Number of segments in the output R*3 curve:                      */
   p_cur->nsgr_cu = (short)no_seg_out;
/*                                                                 !*/

  if ( no_seg_out == 0 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 Exit varkon_sur_scur_gra with failure no_seg_out= %d\n",
   (short)no_seg_out );
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif
     sprintf(errbuf,"no_seg_out=0%%sur960");
     return(varkon_erpush("SU2993",errbuf));
     }

#ifdef DEBUG
varkon_sur_scur_mbs ( p_cur, *pp_graseg );
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 Exit varkon_sur_scur_gra Number of R*3 curve segments %d\n",
   (short)no_seg_out );
fflush(dbgfil(SURPAC));
}
#endif

slut:; /* Label: Function sur722 used succesfully                   */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur960 Exit varkon_sur_scur_gra Number of R*3 curve segments %d\n",
   p_cur->nsgr_cu);
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/








