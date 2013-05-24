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

#ifdef  WIN32
#include  <windows.h>
#endif

#ifdef  V3_OPENGL
#include  <GL/gl.h>
#else
#define  GLfloat  float
#endif

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_granurbs                 File: sur963.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates an approximate NURBS (Open GL) surface     */
/*  for graphical representation of a surface.                      */
/*                                                                  */
/*  Remark:                                                         */
/*  Note that the calling function must deallocate memory           */
/*  for the created NURBS surface (if the pointers not are          */
/*  equal to NULL).                                                 */
/*                                                                  */
/* Rational NURBS is implemented with the "approximation"           */
/* method, i.e. a non-rational NURBS surface is the graphical       */
/* representation of a rational NURBS surface.                      */
/* The graphic library OpenGl can also take a rational NURBS        */
/* surface as input, and the BEST solution is probably to           */
/* add a parameter (a flag if the surface is rational or not)       */
/* in this function and in the exepac graphic functions, make       */
/* it possible to save rational NURBS as graphics in DB, etc.       */
/* The SECOND BEST solution is selected, due to Johan's current     */
/* workload ....                                                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-16   Originally written                                 */
/*  1997-03-19   Bug: No return after sur174 at error               */
/*  1997-05-14   NURBS dynamic allocation                           */
/*  1997-11-27   Handling of an input NURB_SUR  added               */
/*  1997-12-13   Calculation case No surface BBOX                   */
/*  1998-01-06   Debug                                              */
/*  1998-02-01   Rational NURBS                                     */
/*  1998-02-07   V3_OPENGL added                                    */
/*  1998-04-25   Missing (last) argument to v3mall                  */
/*  1999-12-18   Free source code modifications                     */
/*  2001-04-08   U and V order for NURBS input surfaces not OK      */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_granurbs   Surface graphical NURBS represen.*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_tobspline    * Approximate to B-spline surf.          */
/**v3mall                  * Allocate memory                        */
/* DBfree_patches          * Deallocates surface memory             */
/* varkon_erpush           * Error message to stack                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_sur_granurbs (sur963)*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus        varkon_sur_granurbs (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_sur,       /* Surface                       (C  ptr)  */
  DBPatch  *p_pat,       /* Allocated area for patch data (C  ptr)  */
  DBptr     pgm_sur,     /* Surface                       (DB ptr)  */
  gmint     g_case,      /* Graphical case (not yet used)           */
  GLfloat **pp_kvu,      /* Knot vector U                    (ptr)  */
  GLfloat **pp_kvv,      /* Knot vector V                    (ptr)  */
  GLfloat **pp_cpts )    /* Polygon points                   (ptr)  */

/* Out:                                                             */
/*        NURBS Open GL data in pp_kvu, pp_kvv and pp_cpts          */
/*        Size of NURBS arrays to p_sur                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  surtype;           /* Type CUB_SUR, RAT_SUR, LFT_SUR ...  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */

   gmint    k_nu;        /* Number of values in U knot vector       */
   gmint    k_nv;        /* Number of values in V knot vector       */
   gmint    k_iu;        /* Loop index value in U knot vector       */
   gmint    k_iv;        /* Loop index value in V knot vector       */
   GLfloat *p_glf;       /* Current GLfloat value             (ptr) */
   DBfloat *p_uval;      /* U knot value                      (ptr) */
   DBfloat *p_vval;      /* V knot value                      (ptr) */
   gmint    c_nuv;       /* Number of control points                */
   gmint    c_iuv;       /* Loop index control point                */


                         /* For varkon_sur_tobspline (sur174):      */
  DBfloat  s_lim[2][2];  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /* All equal zero => whole surface         */
  gmint    acase;        /* Approximation case:                     */
                         /* Eq. 1: Number of patches defined by the */
                         /*        user (np_u and np_v)             */
                         /* Eq. 2: Necessary number of patches      */
                         /*        calculated by the program        */
                         /*        (not yet implemented)            */
                         /* Eq. 3: Graphical representation surface */
                         /*        Necessary number of patches are  */
                         /*        calculated by the program in a   */
                         /*        simplified way and bounding      */
                         /*        boxes and cones are only copied  */
                         /*        from the input surface           */
  gmint    np_u;         /* Number of approx patches per patch in U */
  gmint    np_v;         /* Number of approx patches per patch in V */
  gmint    tcase;        /* Tolerance band case                     */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
                         /* Tolerances for the approximation.       */
                         /* ( < 0 <==> not defined )                */
  DBfloat  ctol;         /* Coordinate          tolerance           */
  DBfloat  ntol;         /* Surface normal      tolerance           */
  DBfloat  rtol;         /* Radius of curvature tolerance           */
  DBSurf   sur_nurb;     /* Surface of type NURB_SUR                */
  DBPatch *p_pat_nurb;   /* Allocated area for patches        (ptr) */

  GMPATNU *p_patnu;      /* NURBS patch                       (ptr) */
  DBHvector  *p_pp;      /* Polygon point                     (ptr) */

   short  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                _____________________                             */
/*               !                     !                            */
/*               ! varkon_sur_granurbs !                            */
/*               !      (sur963)       !                            */
/*               !_____________________!                            */
/*         _______________!________________________ ..._            */
/*   _____!_______  ____!______  _____!_____       _____!______     */
/*  !             !!           !!           !     !            !    */
/*  !     1       !!    2      !!     3     !     !     8      !    */
/*  ! Checks and  !!           !!           !     !   Exit     !    */
/*  ! initializat.!!           !!           !     !            !    */
/*  !_____________!!___________!!___________!     !____________!    */
/*        !              !                                          */
/*   _____!______   _____!_________                                 */
/*  !            ! !               !                                */
/*  ! _sur_nopat ! !               !                                */
/*  !  (sur230)  ! !               !                                */
/*  !____________! !_______________!                                */
/*                        !                                         */
/*                  ______!________                                 */
/*                 !               !                                */
/*                 !               !                                */
/*                 !               !                                */
/*                 !_______________!                                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963*Enter*Surface graphical NURBS representation\n");
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialize output pointers to NULL.                              */
/*                                                                 !*/

  *pp_kvu   = NULL;
  *pp_kvv   = NULL;
  *pp_cpts  = NULL;

/*!                                                                 */
/* Initialize output DBSurf NURBS data                              */
/*                                                                 !*/

/*  NURBS: Order in U and V direction                               */
    p_sur->uorder_su =      I_UNDEF;    
    p_sur->vorder_su =      I_UNDEF;    

/*  NURBS: Number of nodes in U and V direction                     */
    p_sur->nku_su=          I_UNDEF;    
    p_sur->nkv_su=          I_UNDEF;    

/* Initialize internal variables                                    */

   p_pat_nurb = NULL;
   p_patnu    = NULL;

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type                                */
/* CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR, MIX_SUR                      */
/* POL_SUR,  P3_SUR,  P7_SUR,  P9_SUR or P21_SUR                    */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);

#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_granurbs (sur963)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if ( surtype     ==  CUB_SUR ||           /* Check surface type      */
     surtype     ==  RAT_SUR ||           
     surtype     ==  CON_SUR ||           
     surtype     ==  MIX_SUR ||           
     surtype     ==  POL_SUR ||           
     surtype     == NURB_SUR ||           
     surtype     ==   P3_SUR ||           
     surtype     ==   P5_SUR ||           
     surtype     ==   P7_SUR ||           
     surtype     ==   P9_SUR ||           
     surtype     ==  P21_SUR ||           
     surtype     ==  LFT_SUR )            
 ;
else if ( surtype == FAC_SUR )           /* Facetted surface not OK */
 {
 sprintf(errbuf, "(FAC_SUR)%%sur963");
 return(varkon_erpush("SU2993",errbuf));
 }
else if ( surtype == BOX_SUR )
 {
 sprintf(errbuf, "(BOX_SUR)%%sur963");
 return(varkon_erpush("SU2993",errbuf));
 }
 else
 {
 sprintf(errbuf, "(type)%%sur963");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur963 Input surface type=%d\n", p_sur->typ_su);
fprintf(dbgfil(SURPAC),
  "sur963 No patches in direction U nu= %d and V nv= %d\n",
                      (int)nu,(int)nv);
fprintf(dbgfil(SURPAC),
   "sur963 Calculation case %d \n", (int)g_case );
}
#endif



/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 2. Approximate to a NURBS surface                                */
/* _________________________________                                */
/*                                                                 !*/

/*!                                                                 */
/* No approximation if the input surface is of type NURB_SUR        */
/* and if the surface not is rational.                              */
/* Goto no_approx in this case.                                     */
/* Approximate also if multiplicity = order. OpenGl seems to have   */
/* problems (that Varkon not have)                                  */ 
/*                                                                 !*/

if ( surtype  == NURB_SUR )
{

/* Assume that the first topology patch geometry pointer is         */
/* pointing at a NURBS patch (and all other to the first topology   */
/* patch)                                                           */
   p_patnu= (GMPATNU *)p_pat->spek_c;

/* Determine if the NURBS surface is rational                       */
   c_nuv =  (p_patnu->nk_u-p_patnu->order_u)* 
            (p_patnu->nk_v-p_patnu->order_v);
   for ( c_iuv = 0; c_iuv <  c_nuv; ++c_iuv )
      {
/*    Pointer to current polygon point                              */
      p_pp  = p_patnu->cpts + c_iuv;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 NURBS surface weight %f \n", p_pp->w_gm );
fflush(dbgfil(SURPAC));
}
#endif
      if ( fabs(p_pp->w_gm-1.0) > 0.0001 )
        {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Approximation since it is rational NURB_SUR\n");
fflush(dbgfil(SURPAC));
}
#endif
        goto rat_nurbs; 
        }
      } /* End loop polygon points */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 No approximation since input surface is NURB_SUR\n");
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 NURB_SUR: p_sur %d p_pat %d\n",(int)p_sur, (int)p_pat);
fflush(dbgfil(SURPAC));
}
#endif
/*  Move data to from input surface to NURBS surface                */
    V3MOME((char *)(p_sur),(char *)(&sur_nurb),sizeof(DBSurf));
    p_pat_nurb = p_pat;
    goto  no_approx;
}

rat_nurbs:; /* Label: Input NURBS surface is rational               */

/*!                                                                 */
/* Approximate to a NURBS surface.                                  */
/* Call of varkon_sur_tobspline (sur174).                           */
/*                                                                 !*/

/* No trimming of the surface                                       */
   s_lim[0][0] = 0.0;
   s_lim[0][1] = 0.0;
   s_lim[1][0] = 0.0;
   s_lim[1][1] = 0.0;

/* No calculation of bounding boxes. Just copying of the surface    */
/* box (acase= 3).                                                  */
   acase  =       3;
   tcase  = I_UNDEF;
   np_u   = I_UNDEF;
   np_v   = I_UNDEF; 
   ctol   = F_UNDEF;
   ntol   = F_UNDEF;
   rtol   = F_UNDEF;

    status = varkon_sur_tobspline
   (p_sur, p_pat, s_lim, acase,np_u,np_v,tcase,
    ctol,ntol,rtol, &sur_nurb, &p_pat_nurb);
    if(status<0)
    {
    if ( p_pat_nurb != NULL ) DBfree_patches(&sur_nurb, p_pat_nurb);
    sprintf(errbuf,"sur174%%sur963");
    return(varkon_erpush("SU2943",errbuf));
    }


/*!                                                                 */
/* 3. NURBS surface data to Open GL arrays                          */
/* _______________________________________                          */
/*                                                                 !*/

no_approx:; /*! Label: Input surface is NURB_SUR                   !*/

/*!                                                                 */
/*  NURBS: Number of nodes in U and V direction                     */
/*                                                                 !*/



/*  Check that there is only one geometry patch. To be added !!!    */
#ifdef  TODO_ADD_CHECK  
/*  Current geometric patch                                         */
    if ( sur_nurb.nu_su == 1 && sur_nurb.nv_su == 1 )
      {
      ; /* One NURBS patch surface */
      }
    else
      {
      if ( surtype  != NURB_SUR ) DBfree_patches(&sur_nurb, p_pat_nurb);
      sprintf(errbuf,"Not 1 patch%%sur963");
      return(varkon_erpush("SU2993",errbuf));
      }
#endif /*   TODO_ADD_CHECK    */

    p_patnu = (GMPATNU *)(p_pat_nurb->spek_c);
    k_nu    = p_patnu->nk_u;
    k_nv    = p_patnu->nk_v;

    p_sur->nku_su = (int)k_nu;
    p_sur->nkv_su = (int)k_nv;

/*  NURBS: Order in U and V direction                               */
    p_sur->uorder_su = (int)p_patnu->order_u;    
    p_sur->vorder_su = (int)p_patnu->order_v;    



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Number of U and V knot values k_nu= %d k_nv= %d\n",
            (int)k_nu,(int)k_nv);
fflush(dbgfil(SURPAC));
}
for ( k_iu = 1; k_iu <= k_nu; ++k_iu )
{
p_uval = p_patnu->kvec_u + k_iu - 1;
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 U[%3d]= %f\n",
            (int)k_iu-1,*(p_uval));
fflush(dbgfil(SURPAC));
}
}
for ( k_iv = 1; k_iv <= k_nv; ++k_iv )
{
p_vval = p_patnu->kvec_v + k_iv - 1;
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 V[%3d]= %f\n",
            (int)k_iv-1,*(p_vval));
fflush(dbgfil(SURPAC));
}
}
#endif


/*!                                                                 */
/*  Allocate memory for U and V knot vectors                        */
/*                                                                 !*/

    *pp_kvu = (GLfloat *)v3mall(k_nu*sizeof(GLfloat),"sur963");
    *pp_kvv = (GLfloat *)v3mall(k_nv*sizeof(GLfloat),"sur963");

/*!                                                                 */
/*  Knot vector values to output Open GL arrays                     */
/*                                                                 !*/

    for ( k_iu = 1; k_iu <= k_nu; ++k_iu )
      {
      p_glf  = *pp_kvu + k_iu - 1;
      p_uval = p_patnu->kvec_u + k_iu - 1;
      *p_glf = (GLfloat)*p_uval;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 U[%3d]= %f\n",
            (int)k_iu-1,*(p_uval));
fflush(dbgfil(SURPAC));
}
#endif
      }

    for ( k_iv = 1; k_iv <= k_nv; ++k_iv )
      {
      p_glf  = *pp_kvv + k_iv - 1;
      p_vval = p_patnu->kvec_v + k_iv - 1;
      *p_glf = (GLfloat)*p_vval;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 V[%3d]= %f\n",
            (int)k_iv-1,*(p_vval));
fflush(dbgfil(SURPAC));
}
#endif
      }


/*!                                                                 */
/*  NURBS: Number of control points for the B-spline surface        */
/*                                                                 !*/

    c_nuv =  (p_patnu->nk_u-p_patnu->order_u)* 
             (p_patnu->nk_v-p_patnu->order_v);

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Number of control points     c_nuv= %d \n", (int)c_nuv);
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/*  Allocate memory for the control points                          */
/*                                                                 !*/

    *pp_cpts = (GLfloat *)v3mall(c_nuv*3*sizeof(GLfloat),"sur963");

    for ( c_iuv = 0; c_iuv <  c_nuv; ++c_iuv )
      {
      p_pp  = p_patnu->cpts + c_iuv;
      p_glf  = *pp_cpts + 3*c_iuv;
      *p_glf = (GLfloat)p_pp->x_gm;
      p_glf  = *pp_cpts + 3*c_iuv + 1;
      *p_glf = (GLfloat)p_pp->y_gm;
      p_glf  = *pp_cpts + 3*c_iuv + 2;
      *p_glf = (GLfloat)p_pp->z_gm;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Node(%3d)= %f %f %f\n",
   (int)c_iuv, p_pp->x_gm,  p_pp->y_gm,  p_pp->z_gm );
fflush(dbgfil(SURPAC));
}
#endif
      }

/*!                                                                 */
/* n. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Deallocation of memory area for the NURBS surface                */
/*                                                                 !*/

   if ( surtype  != NURB_SUR ) DBfree_patches(&sur_nurb, p_pat_nurb);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && surtype != NURB_SUR )
{
fprintf(dbgfil(SURPAC),"sur963 Memory area for NURBS deallocated\n");
}
#endif


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Number of U and V knot values k_nu= %d k_nv= %d\n",
            (int)k_nu,(int)k_nv);
fprintf(dbgfil(SURPAC),
"sur963 Number of control points     c_nuv= %d \n", (int)c_nuv);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur963 Exit  *** varkon_sur_granurbs \n");
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/



