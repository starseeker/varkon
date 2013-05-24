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
/*  Function: varkon_sur_uvsegeval                 File: sur214.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the coordinates and derivatives         */
/*  for a point on a segment of a U,V (surface) curve               */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1994-10-29   Debug                                              */
/*  1994-11-12   Offset curve                                       */
/*  1994-12-08   Codes for derivatives, debug                       */
/*  1995-02-25   u_t2_geod0, v_t2_geod0, Debug (DB pointer, ...)    */
/*  1995-03-15   Documentation                                      */
/*  1995-03-17   New DBread_one_patch(), J. Kjellander              */
/*  1996-02-13   Numerical noise from evaluation of UV curve        */
/*  1997-12-07   NURBS, temporary (first) implementation            */
/*  1998-02-03   NURBS bug                                          */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_uvsegeval  Coord. and derivatives for UV pt */
/*                                                              */
/*------------------------------------------------------------- */

/*!---------------------- Theory -----------------------------------*/
/*  Reference: Faux & Pratt p 110-111 and p 274                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/* DBread_surface          * Retrieve DB surface data               */
/* varkon_sur_patadr       * Patch adress for given U,V pt          */
/* varkon_pat_eval         * Evaluate patch                         */
/* varkon_seg_uveval       * Evaluate UV segment                    */
/* varkon_seg_offset       * Offset coord. & derivatives            */
/* varkon_sur_normkappa    * Normal curvature                       */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxx failed     in varkon_sur_uvsegeval */
/* SU2993 = Severe program error in varkon_sur_uvsegeval (sur214).  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_sur_uvsegeval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Curve segment                     (ptr) */
   EVALC   *p_xyz_c )    /* Curve coordinates & derivatives   (ptr) */
/* Out:                                                             */
/*       Data to p_xyz_c.                                           */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALS    xyz_s;       /* Surface coordinates & derivatives       */
   DBfloat  dsdt;        /* Length of tangent (dr/dt)               */
   DBfloat  tan[3];      /* Normalized tangent= (dr/dt)/dsdt        */
   DBfloat  nkappa;      /* Normal   curvature                      */
   DBfloat  geodesic;    /* Geodesic curvature                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBSurf   sur;         /* Surface for input U,V segment           */
   DBint    icase;       /* Evaluation case for varkon_pat_eval     */
   DBint    nu,nv;       /* Number of patches in surface            */
   DBint    iu,iv;       /* Patch number (address)                  */
   DBfloat  u_local;     /* Local patch parameter U                 */
   DBfloat  v_local;     /* Local patch parameter U                 */
   DBVector drdt;        /* First   derivative with respect to t    */
   DBVector d2rdt2;      /* Second  derivative with respect to t    */
   DBfloat  kappa;       /* Curvature                               */
   DBVector b_norm;      /* Binormal                                */
   DBVector p_norm;      /* Principal normal                        */
   bool     offseg;      /* Flag for offset segment                 */
   DBfloat  offset;      /* Offset value                            */
   DBVector n_p;         /* Curve segment plane normal              */
   DBVector r;           /* Point              non-offset           */
   DBfloat  u_t2_geod0;  /* Second U derivative for geodesic= 0     */
   DBfloat  v_t2_geod0;  /* Second V derivative for geodesic= 0     */
   DBVector v1,v2,v3,v4; /* For geodesic= 0 (UV 2'nd derivatives)   */
   DBfloat  dot_1,dot_2; /* Scalar products for geodesic = 0        */
   DBfloat  dot_3;       /* Scalar product  for geodesic = 0        */
   DBfloat  uv_t_len;    /* Length u_t,v_t  for geodesic = 0        */
   DBint    rcode;       /* Derivative calculation code             */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBPatch *p_toppat;    /* Pointer to topological patch            */
   DBPatch  toppat;      /* Topological patch for DBread_one_patch  */
   GMPATX   geopat;      /* Geometrical patch for DBread_one_patch  */

#ifdef DEBUG
   DBfloat  larserik;    /* Geodesic curvature a la Lars-Erik       */
   DBfloat  kvadrat;     /* Geodesic curvature as ..                */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/*                                                                  */
/*                                                                 !*/

        if (  p_xyz_c->evltyp & EVC_DR  ) rcode = 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && (p_xyz_c->evltyp & EVC_R) )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Enter*varkon_sur_uvsegeval Case  EVC_R Offset %f \n",
   p_seg->ofs);
  }
else if ( dbglev(SURPAC) == 1 && (p_xyz_c->evltyp & EVC_DR) )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Enter*varkon_sur_uvsegeval Case EVC_DR Offset %f \n",
   p_seg->ofs);
  }
else
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Enter*varkon_sur_uvsegeval Case= %d Offset %f \n",
   (int)p_xyz_c->evltyp , p_seg->ofs);
fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 0. Check of input data and initializations                       */
/* ____________________________________________                     */
/*                                                                  */
/* Let offset flag be true and retrieve curve plane if the          */
/* curve is in offset.                                              */
/*                                                                 !*/

/* Check that DB pointer to patch not is NULL (assume that DBSeg    */
/* data is initialized with sur779)   for Debug On                  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Patch pointer in DB p_seg->spek_gm %d\n", 
        (int)p_seg->spek_gm );
fflush(dbgfil(SURPAC)); 
  }
if ( p_seg->spek_gm == DBNULL )
       {
       sprintf(errbuf,"DB ptr is NULL%%sur214");
       return(varkon_erpush("SU2993",errbuf));
       }
#endif


/* Initialization of local variables                                */
   iu      = I_UNDEF;
   iv      = I_UNDEF;
   nu      = I_UNDEF;
   nv      = I_UNDEF;
   u_local = F_UNDEF;
   v_local = F_UNDEF;

   if ( p_seg->ofs != 0.0 )
      {
      offseg = TRUE;
      offset = p_seg->ofs;
      n_p.x_gm = p_cur->csy_cu.g31;
      n_p.y_gm = p_cur->csy_cu.g32;
      n_p.z_gm = p_cur->csy_cu.g33;
      }
    else 
      {
      offseg = FALSE;
      offset = p_seg->ofs;
      n_p.x_gm = -0.123456789;       
      n_p.y_gm = -0.123456789;       
      n_p.z_gm = -0.123456789;       
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && offseg == TRUE)
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Input segment is in offset= %f\n", offset);
  fprintf(dbgfil(SURPAC),
  "sur214 Curve plane normal n_p %f %f %f \n",
            n_p.x_gm,   n_p.y_gm ,  n_p.z_gm);
fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/*!                                                                 */
/* 1. Surface U,V curve segment evaluation                          */
/* _______________________________________                          */
/*                                                                  */
/* Calculate coordinates u,v and derivatives du/dt,dvdt, ...        */
/* for the given point on the input U,V segment.                    */
/* Call of varkon_seg_uveval (sur786).                              */
/* for UV_NURB_SEG call GEevaluvnc                                  */
/*                                                                 !*/
   if ( p_seg->typ  ==  UV_NURB_SEG ) 
     {
     status=GEevaluvnc(p_cur,p_seg,p_xyz_c); 
     }
   else 
     {
     status=varkon_seg_uveval (p_cur,p_seg,p_xyz_c);
     }
     
   if (status<0) 
       {
       sprintf(errbuf,"sur768%%sur214");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* 2. Current patch address and local parameter point.              */
/* ___________________________________________________              */
/*                                                                  */
/* Retrieve surface header data (no of patches) from DB.            */
/* Call of DBread_surface.                                          */
/*                                                                 !*/

   status= DBread_surface (&sur,p_seg->spek_gm);
   if (status<0) 
       {
       sprintf(errbuf,"DBread_surface%%varkon_sur_uvsegeval");
       return(varkon_erpush("SU2943",errbuf));
       }

    nu = sur.nu_su;
    nv = sur.nv_su;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur214 Surface data retrieved from DB with DBread_surface nu %d nv %d\n",
 (int)nu , (int)nv );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Patch address iu,iv and local patch U,V point.                   */
/* Eliminate numerical noise from evaluation of UV curve            */
/* Call of varkon_sur_patadr (sur211).                              */
/*                                                                 !*/

#ifdef DEBUG
if  ( sur.typ_su != NURB_SUR )
{
if ( dbglev(SURPAC) == 1 && p_xyz_c->u < 1.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Evaluated u= %25.15f Modified to 1.0  (Diff. %25.15f)\n", 
    p_xyz_c->u, p_xyz_c->u - 1.0 );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 && p_xyz_c->v < 1.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Evaluated v= %25.15f Modified to 1.0  (Diff. %25.15f)\n", 
    p_xyz_c->v, p_xyz_c->v - 1.0 );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 && p_xyz_c->u > (DBfloat)nu + 1.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Evaluated u= %25.15f Modified to 1.0  (Diff. %25.15f)\n", 
    p_xyz_c->u, p_xyz_c->u - (DBfloat)nu - 1.0 );
fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 && p_xyz_c->v > (DBfloat)nv + 1.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Evaluated u= %25.15f Modified to 1.0  (Diff. %25.15f)\n", 
    p_xyz_c->v, p_xyz_c->v - (DBfloat)nv - 1.0 );
fflush(dbgfil(SURPAC)); 
  }
} /* End Not NURBS */
#endif

if  ( sur.typ_su != NURB_SUR )
{
  if ( p_xyz_c->u <      1.0        ) p_xyz_c->u = 1.0;
  if ( p_xyz_c->v <      1.0        ) p_xyz_c->v = 1.0;
  if ( p_xyz_c->u > (DBfloat)nu + 1.0 ) p_xyz_c->u = (DBfloat)nu+1.0;
  if ( p_xyz_c->v > (DBfloat)nv + 1.0 ) p_xyz_c->v = (DBfloat)nv+1.0;

   status=varkon_sur_patadr
   (p_xyz_c->u,p_xyz_c->v,nu,nv,&iu,&iv,&u_local,&v_local);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 sur211 failed for u %25.15f v %25.15f \n", 
             p_xyz_c->u, p_xyz_c->v );
fflush(dbgfil(SURPAC)); 
  }
#endif
        sprintf(errbuf,"varkon_sur_patadr%%varkon_sur_uvsegeval");
        return(varkon_erpush("SU2943",errbuf));
        }

} /* End Not NURBS */

/*!                                                                 */
/* 3. Surface coordinates and derivatives                           */
/* ______________________________________                           */
/*                                                                  */
/* Get C pointer to patch (retrieve patch data).                    */
/* Call of DBread_one_patch.                                        */
/*                                                                 !*/

/* A NURBS surface consists of only one patch and cannot be         */
/* partially be retrieved. It is for all other surface types        */
/* only necessary to retrieve one patch                             */

      switch ( sur.typ_su )
        {
        case NURB_SUR:
        DBread_patches( &sur,&p_toppat);
        u_local = p_xyz_c->u;
        v_local = p_xyz_c->v;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 NURBS First topology and the geometry patch retrieved\n");
  fprintf(dbgfil(SURPAC),
  "sur214 Parameter point from sur786 EVALC calculation %f %f \n",
            u_local, v_local);
fflush(dbgfil(SURPAC)); 
  }
#endif
        break;

        default:
        toppat.spek_c = (char *)&geopat;
        if ( (status=DBread_one_patch
               ( &sur,&toppat,(short)iu,(short)iv)) < 0 )
           return(status);
        p_toppat = &toppat;
        break;
        }

/*!                                                                 */
/* Calculate coordinates and derivatives in the patch.              */
/* Call of varkon_pat_eval (sur220).                                */
/*                                                                 !*/

/* TODO:                                                            */
/* If the curve segment is an (iso) V- or U-segment can the         */
/* the calculation time be reduced if icase is set to 1 or 2        */

#ifdef TODO_INVESTIGATE_FIRST
    if      ( p_seg->c1y==0.0 && p_seg->c2y==0.0 && 
              p_seg->c3y==0.0 && rcode==1 )
      icase = 1;
    else if ( p_seg->c1x==0.0 && p_seg->c2x==0.0 && 
              p_seg->c3y==0.0 && rcode==1 )
      icase = 2;
    else
      icase = 3;
#endif

   icase = 3;



   if (offseg == TRUE ) icase = 3; /* All derivatives for offset  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Parameter for varkon_pat_eval u_local %f v_local %f \n",
            u_local, v_local);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    status= varkon_pat_eval
        (&sur,p_toppat ,icase,u_local,v_local, &xyz_s);
   if (status<0) 
     {
/*   Deallocate NURBS                                             */
     if ( sur.typ_su == NURB_SUR ) DBfree_patches( &sur,p_toppat);
     sprintf(errbuf,"varkon_pat_eval%%varkon_sur_uvsegeval");
     return(varkon_erpush("SU2943",errbuf));
     }


/* Memory must be freed for a NURBS surface                         */
   if ( sur.typ_su == NURB_SUR ) DBfree_patches( &sur,p_toppat);




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 p_xyz_c->u_t %f v_t %f u_t2 %f v_t2 %f\n",
          p_xyz_c->u_t, p_xyz_c->v_t ,p_xyz_c->u_t2,p_xyz_c->v_t2);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 xyz_s= %f %f %f \n",
          xyz_s.r_x , xyz_s.r_y  ,xyz_s.r_z );
  fflush(dbgfil(SURPAC));
  }
#endif




/*!                                                                 */
/* Coordinates from xyz_s. Goto nomore if evltyp= EVC_R.            */
/* Goto nomore if evltyp= EVC_R and not an offset curve             */
/*                                                                 !*/

        rcode  = -1;
        if (  p_xyz_c->evltyp & EVC_R   ) rcode = 0;
        if (  p_xyz_c->evltyp & EVC_DR  ) rcode = 1;
        if (  p_xyz_c->evltyp & EVC_D2R ) rcode = 2;
        if ( (p_xyz_c->evltyp & EVC_PN )  ||
             (p_xyz_c->evltyp & EVC_BN )  ||
             (p_xyz_c->evltyp & EVC_KAP) ) rcode = 3;
        if ( rcode < 0 )
        {
        sprintf(errbuf,"rcode%%varkon_sur_uvsegeval");
        return(varkon_erpush("SU2993",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 rcode %d offseg %d   (1)\n", (int)rcode, (int)offseg );
fflush(dbgfil(SURPAC));
  }
#endif

     p_xyz_c->r.x_gm=  xyz_s.r_x;
     p_xyz_c->r.y_gm=  xyz_s.r_y;
     p_xyz_c->r.z_gm=  xyz_s.r_z;

     {
     if ( rcode == 0 && offseg == FALSE )
     goto nomore;
     }

     r.x_gm=  xyz_s.r_x; /* For offset calculation sur217           */
     r.y_gm=  xyz_s.r_y;
     r.z_gm=  xyz_s.r_z;

/*!                                                                 */
/* First  derivatives to output variable drdt.                      */
/* Goto nomore if evltyp= EVC_DR and not an offset curve            */
/*                                                                 !*/

   drdt.x_gm= xyz_s.u_x*p_xyz_c->u_t + xyz_s.v_x*p_xyz_c->v_t;
   drdt.y_gm= xyz_s.u_y*p_xyz_c->u_t + xyz_s.v_y*p_xyz_c->v_t;
   drdt.z_gm= xyz_s.u_z*p_xyz_c->u_t + xyz_s.v_z*p_xyz_c->v_t;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 rcode %d offseg %d   (2)\n", (int)rcode, (int)offseg );
  }
#endif

     p_xyz_c->drdt.x_gm= drdt.x_gm;
     p_xyz_c->drdt.y_gm= drdt.y_gm;
     p_xyz_c->drdt.z_gm= drdt.z_gm;

     if ( rcode == 1 && offseg == FALSE )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214*Exit*varkon_sur_uvsegeval Output tangent %f %f %f\n", 
       p_xyz_c->drdt.x_gm,p_xyz_c->drdt.y_gm,p_xyz_c->drdt.z_gm);
  }
#endif
     goto nomore;
     }

/*!                                                                 */
/* Second derivatives to output variable d2rdt2                     */
/* Goto nomore if evltyp= EVC_D2R and not an offset curve           */
/*                                                                 !*/

   d2rdt2.x_gm= 
                xyz_s.u2_x*p_xyz_c->u_t*p_xyz_c->u_t 
         + 2.0* xyz_s.uv_x*p_xyz_c->u_t*p_xyz_c->v_t 
         +      xyz_s.v2_x*p_xyz_c->v_t*p_xyz_c->v_t   
         +      xyz_s.u_x *p_xyz_c->u_t2
         +      xyz_s.v_x *p_xyz_c->v_t2;
   d2rdt2.y_gm= 
                xyz_s.u2_y*p_xyz_c->u_t*p_xyz_c->u_t 
         + 2.0* xyz_s.uv_y*p_xyz_c->u_t*p_xyz_c->v_t 
         +      xyz_s.v2_y*p_xyz_c->v_t*p_xyz_c->v_t   
         +      xyz_s.u_y *p_xyz_c->u_t2
         +      xyz_s.v_y *p_xyz_c->v_t2;
   d2rdt2.z_gm= 
                xyz_s.u2_z*p_xyz_c->u_t*p_xyz_c->u_t 
         + 2.0* xyz_s.uv_z*p_xyz_c->u_t*p_xyz_c->v_t 
         +      xyz_s.v2_z*p_xyz_c->v_t*p_xyz_c->v_t   
         +      xyz_s.u_z *p_xyz_c->u_t2
         +      xyz_s.v_z *p_xyz_c->v_t2;

     p_xyz_c->d2rdt2.x_gm= d2rdt2.x_gm; 
     p_xyz_c->d2rdt2.y_gm= d2rdt2.y_gm; 
     p_xyz_c->d2rdt2.z_gm= d2rdt2.z_gm; 

     if ( rcode == 2 && offseg == FALSE )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214*Exit*varkon_sur_uvsegeval Output d2rdt2 %f %f %f\n", 
       p_xyz_c->d2rdt2.x_gm,p_xyz_c->d2rdt2.y_gm,p_xyz_c->d2rdt2.z_gm);
  }
#endif
     goto nomore;
     }

/*!                                                                 */
/* Calculate offset coordinates and derivatives if the curve        */
/* segment is in offset.                                            */
/* Call of varkon_seg_offset (sur217).                              */
/*                                                                 !*/

   if (offseg == TRUE )
     {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 Before sur217 r      %f %f %f\n", r.x_gm,r.y_gm,r.z_gm);
  fprintf(dbgfil(SURPAC),
  "sur214 Before sur217 drdt   %f %f %f\n", drdt.x_gm,drdt.y_gm,drdt.z_gm);
  fprintf(dbgfil(SURPAC),
  "sur214 Before sur217 d2rdt2 %f %f %f\n", d2rdt2.x_gm,d2rdt2.y_gm,d2rdt2.z_gm);
fflush(dbgfil(SURPAC)); 
  }
#endif
     status= varkon_seg_offset
    (r,drdt,d2rdt2,offset,p_xyz_c->evltyp,n_p,
        &r,&drdt,&d2rdt2);
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 After  sur217 r      %f %f %f\n", r.x_gm,r.y_gm,r.z_gm);
  fprintf(dbgfil(SURPAC),
  "sur214 After  sur217 drdt   %f %f %f\n", drdt.x_gm,drdt.y_gm,drdt.z_gm);
  fprintf(dbgfil(SURPAC),
  "sur214 After  sur217 d2rdt2 %f %f %f\n", d2rdt2.x_gm,d2rdt2.y_gm,d2rdt2.z_gm);
  fprintf(dbgfil(SURPAC),
  "sur214 After  sur217 status %d\n", (int)status );
fflush(dbgfil(SURPAC)); 
  }
#endif
     if (status<0) 
        {
        sprintf(errbuf,"varkon_seg_offset%%varkon_sur_uvsegeval");
       return(varkon_erpush("SU2943",errbuf));
        }
/* This must be wrong   ????? TODO {   */
       p_xyz_c->r.x_gm=  r.x_gm;
       p_xyz_c->r.y_gm=  r.y_gm;
       p_xyz_c->r.z_gm=  r.z_gm;
       if ( rcode == 0  )
       {
       goto nomore;
       }
       p_xyz_c->drdt.x_gm= drdt.x_gm;
       p_xyz_c->drdt.y_gm= drdt.y_gm;
       p_xyz_c->drdt.z_gm= drdt.z_gm;
       if ( rcode == 1  )
       {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214*Exit*varkon_sur_uvsegeval Output tangent %f %f %f\n", 
       p_xyz_c->drdt.x_gm,p_xyz_c->drdt.y_gm,p_xyz_c->drdt.z_gm);
fflush(dbgfil(SURPAC)); 
  }
#endif
       goto nomore;
       }
       {
       p_xyz_c->d2rdt2.x_gm= d2rdt2.x_gm; 
       p_xyz_c->d2rdt2.y_gm= d2rdt2.y_gm; 
       p_xyz_c->d2rdt2.z_gm= d2rdt2.z_gm; 
       if ( rcode == 2  )
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214*Exit*varkon_sur_uvsegeval Output tangent %f %f %f\n", 
       p_xyz_c->d2rdt2.x_gm,p_xyz_c->d2rdt2.y_gm,p_xyz_c->d2rdt2.z_gm);
fflush(dbgfil(SURPAC)); 
  }
#endif
       goto nomore;
       }
     }        /* End offseg == TRUE  */


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 4. Calculate kappa and binormal                                  */
/* _______________________________                                  */
/*                                                                 !*/

/*!                                                                 */
/* The length dsdt of the tangent (dr/dt)                           */
/*                                                                 !*/

   dsdt= SQRT(drdt.x_gm*drdt.x_gm + 
              drdt.y_gm*drdt.y_gm +
              drdt.z_gm*drdt.z_gm);
   if ( dsdt < 1e-10 ) dsdt= 1e-10;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 drdt %10.7f %10.7f %10.7f dsdt %f\n",
          drdt.x_gm,drdt.y_gm,drdt.z_gm ,dsdt);
fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* First kappa*binormal. Then kappa which is the length of          */
/* vector kappa*binormal and lastly the binormal (divide            */
/* vector kappa*binormal with kappa).                               */
/*                                                                 !*/

   b_norm.x_gm=
    (drdt.y_gm*d2rdt2.z_gm-drdt.z_gm*d2rdt2.y_gm)/dsdt/dsdt/dsdt;
   b_norm.y_gm=
    (drdt.z_gm*d2rdt2.x_gm-drdt.x_gm*d2rdt2.z_gm)/dsdt/dsdt/dsdt;
   b_norm.z_gm=
    (drdt.x_gm*d2rdt2.y_gm-drdt.y_gm*d2rdt2.x_gm)/dsdt/dsdt/dsdt;

   kappa  = SQRT(b_norm.x_gm*b_norm.x_gm+
                 b_norm.y_gm*b_norm.y_gm+
                 b_norm.z_gm*b_norm.z_gm);

   p_xyz_c->kappa = kappa;

   if ( kappa   > 1e-10 ) 
       {
       b_norm.x_gm= b_norm.x_gm/kappa;  
       b_norm.y_gm= b_norm.y_gm/kappa;  
       b_norm.z_gm= b_norm.z_gm/kappa;  
       }
   else 
       {
       b_norm.x_gm= 0.0;                
       b_norm.y_gm= 0.0;                
       b_norm.z_gm= 0.0;                
       }

   p_xyz_c->b_norm= b_norm;

/*!                                                                 */
/* 5. Calculate the principal normal                                */
/* _________________________________                                */
/*                                                                 !*/

/*!                                                                 */
/* The normalized tangent                                           */
/*                                                                 !*/

   tan[0] = drdt.x_gm/dsdt;
   tan[1] = drdt.y_gm/dsdt;
   tan[2] = drdt.z_gm/dsdt;


/*!                                                                 */
/* The principal normal= binormal X tangent                         */
/*                                                                 !*/

   p_norm.x_gm= b_norm.y_gm*tan[2]-b_norm.z_gm*tan[1];
   p_norm.y_gm= b_norm.z_gm*tan[0]-b_norm.x_gm*tan[2];
   p_norm.z_gm= b_norm.x_gm*tan[1]-b_norm.y_gm*tan[0];

   p_xyz_c->p_norm= p_norm;


/*!                                                                 */
/* 6. Calculate the normal curvature                                */
/* _________________________________                                */
/*                                                                  */
/* Calculate the normal curvature                                   */
/* Call of varkon_sur_normkappa (sur952).                           */
/*                                                                 !*/

varkon_sur_normkappa
  (p_xyz_c->u_t,p_xyz_c->v_t,&xyz_s,&nkappa);
/* No errors from this function */

   p_xyz_c->nkappa = nkappa;

/*!                                                                 */
/* 7. Geodesic curvature                                            */
/* _____________________                                            */
/*                                                                  */
/* Calculate the geodesic (tangential) curvature                    */
/*                                                                 !*/

   geodesic=((drdt.y_gm*d2rdt2.z_gm-drdt.z_gm*d2rdt2.y_gm)*xyz_s.n_x+   
             (drdt.z_gm*d2rdt2.x_gm-drdt.x_gm*d2rdt2.z_gm)*xyz_s.n_y+   
             (drdt.x_gm*d2rdt2.y_gm-drdt.y_gm*d2rdt2.x_gm)*xyz_s.n_z)
                      /dsdt/dsdt/dsdt;

   p_xyz_c->geodesic = geodesic;


/*!                                                                 */
/* Calculate the U,V second derivatives for geodesic curvature = 0  */
/* Can be used for calculation of geodesic curves or planar         */
/*  intersect curves (tangential curvature is zero for such curve)  */
/*                                                                 !*/

  v1.x_gm=       xyz_s.u2_x*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_x*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_x*p_xyz_c->v_t*p_xyz_c->v_t;  
  v1.y_gm=       xyz_s.u2_y*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_y*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_y*p_xyz_c->v_t*p_xyz_c->v_t;  
  v1.z_gm=       xyz_s.u2_z*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_z*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_z*p_xyz_c->v_t*p_xyz_c->v_t;  
  v4.x_gm = tan[1]* xyz_s.n_z-tan[2]* xyz_s.n_y;
  v4.y_gm = tan[2]* xyz_s.n_x-tan[0]* xyz_s.n_z;
  v4.z_gm = tan[0]* xyz_s.n_y-tan[1]* xyz_s.n_x;
  dot_1   = v1.x_gm*v4.x_gm + v1.y_gm*v4.y_gm + v1.z_gm*v4.z_gm; 
  v2.x_gm = xyz_s.u_x;               
  v2.y_gm = xyz_s.u_y;               
  v2.z_gm = xyz_s.u_z;               
  v3.x_gm = xyz_s.v_x;               
  v3.y_gm = xyz_s.v_y;               
  v3.z_gm = xyz_s.v_z;               
  dot_2   = v2.x_gm*v4.x_gm + v2.y_gm*v4.y_gm + v2.z_gm*v4.z_gm;  
  dot_3   = v3.x_gm*v4.x_gm + v3.y_gm*v4.y_gm + v3.z_gm*v4.z_gm;  

  uv_t_len = SQRT(p_xyz_c->u_t*p_xyz_c->u_t + p_xyz_c->v_t*p_xyz_c->v_t); 
  if ( fabs(dot_2) > fabs(dot_3) )
     {
     v_t2_geod0 = uv_t_len*0.5;  
     if ( fabs(dot_2) > 0.00000000001 )
        {
        u_t2_geod0 = (- dot_1 - v_t2_geod0*dot_3)/dot_2;  
        }
     else
        {
        sprintf(errbuf,"dot_2= 0 %%varkon_sur_uvsegeval");
       return(varkon_erpush("SU2993",errbuf));
        }
     }
  else
     {
     u_t2_geod0 = uv_t_len*0.5;  
     if ( fabs(dot_3) > 0.00000000001 )
        {
        v_t2_geod0 = (- dot_1 - u_t2_geod0*dot_2)/dot_3;  
        }
     else
        {
        sprintf(errbuf,"dot_3= 0 %%varkon_sur_uvsegeval");
       return(varkon_erpush("SU2993",errbuf));
        }
     }

  p_xyz_c->u_t2_geod0 = u_t2_geod0;
  p_xyz_c->v_t2_geod0 = v_t2_geod0;

#ifdef DEBUG
  v1.x_gm=       xyz_s.u2_x*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_x*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_x*p_xyz_c->v_t*p_xyz_c->v_t;  
  v1.y_gm=       xyz_s.u2_y*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_y*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_y*p_xyz_c->v_t*p_xyz_c->v_t;  
  v1.z_gm=       xyz_s.u2_z*p_xyz_c->u_t*p_xyz_c->u_t +
           2.0*  xyz_s.uv_z*p_xyz_c->u_t*p_xyz_c->v_t +
                 xyz_s.v2_z*p_xyz_c->v_t*p_xyz_c->v_t;  
  v2.x_gm=       xyz_s.u_x*p_xyz_c->u_t2;
  v2.y_gm=       xyz_s.u_y*p_xyz_c->u_t2;
  v2.z_gm=       xyz_s.u_z*p_xyz_c->u_t2;
  v3.x_gm=       xyz_s.v_x*p_xyz_c->v_t2;
  v3.y_gm=       xyz_s.v_y*p_xyz_c->v_t2;
  v3.z_gm=       xyz_s.v_z*p_xyz_c->v_t2;

  v2.x_gm=       xyz_s.u_x*p_xyz_c->u_t2_geod0; /* Test geodesic = 0 */
  v2.y_gm=       xyz_s.u_y*p_xyz_c->u_t2_geod0;
  v2.z_gm=       xyz_s.u_z*p_xyz_c->u_t2_geod0;
  v3.x_gm=       xyz_s.v_x*p_xyz_c->v_t2_geod0;
  v3.y_gm=       xyz_s.v_y*p_xyz_c->v_t2_geod0;
  v3.z_gm=       xyz_s.v_z*p_xyz_c->v_t2_geod0;

  v4.x_gm = tan[1]* xyz_s.n_z-tan[2]* xyz_s.n_y;
  v4.y_gm = tan[2]* xyz_s.n_x-tan[0]* xyz_s.n_z;
  v4.z_gm = tan[0]* xyz_s.n_y-tan[1]* xyz_s.n_x;
  larserik= v1.x_gm*v4.x_gm + v1.y_gm*v4.y_gm + v1.z_gm*v4.z_gm + 
            v2.x_gm*v4.x_gm + v2.y_gm*v4.y_gm + v2.z_gm*v4.z_gm + 
            v3.x_gm*v4.x_gm + v3.y_gm*v4.y_gm + v3.z_gm*v4.z_gm;  
  larserik= larserik/dsdt/dsdt;

   kvadrat = kappa*kappa - nkappa*nkappa;
   if ( kvadrat > 0.0 )
   kvadrat = SQRT(kvadrat);
   else
   kvadrat = -1.23456789;
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214 geodesic curvature %20.16f larserik %20.16f\n", 
           geodesic, larserik );
fflush(dbgfil(SURPAC)); 
  fprintf(dbgfil(SURPAC),
  "sur214 kappa %20.16f nkappa  %20.16f\n", 
           kappa  , nkappa );
fflush(dbgfil(SURPAC)); 
  fprintf(dbgfil(SURPAC),
  "sur214 geodesic kvadrat   %20.16f \n", 
           kvadrat );
fflush(dbgfil(SURPAC)); 
  }
#endif

nomore: /*! Label nomore: End of calculation for case evltyp= .... !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur214*Exit*varkon_sur_uvsegeval Output pt %f %f %f\n", 
       p_xyz_c->r.x_gm,p_xyz_c->r.y_gm,p_xyz_c->r.z_gm);
fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

