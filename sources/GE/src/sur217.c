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
/*  Function: varkon_seg_offset                    File: sur217.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the coordinates and derivatives         */
/*  for a point on an offset curve segment.                         */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-11-12   Originally written                                 */
/*  1994-12-06   Bug EVC_DR -> EVC_R+EVC_DR                         */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_seg_offset     Offset coordinates & derivatives */
/*                                                              */
/*------------------------------------------------------------- */

/* ---------------------- Theory -----------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2533 = No 2'nd derivatives for planar offset surface curves    */
/* SU2993 = Severe program error in varkon_seg_offset    (sur217).  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_seg_offset (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBVector  r,          /* Point              non-offset           */
   DBVector  drdt,       /* First  derivative  non-offset           */
   DBVector  d2rdt2,     /* Second derivative  non-offset           */
   DBfloat   offset,     /* Offset value                            */
   DBint     evltyp,     /* Evaluation code (case)                  */
   DBVector  n_p,        /* Curve segment plane normal              */
   DBVector *p_r_o,      /* Point on offset segment           (ptr) */
   DBVector *p_drdt_o,   /* First  derivative with resp. to t (ptr) */
   DBVector *p_d2rdt2_o )/* Second derivative with resp. to t (ptr) */
/* Out:                                                             */
/*       Data to p_r_o, p_drdt_o and p_d2rdt2_o.                    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  x,y,z;         /* Point              non-offset           */
   DBfloat  dxdu,dydu,dzdu;/* First  derivative  non-offset           */
   DBfloat  d2xdu2,d2ydu2; /* Second derivative  non-offset           */
   DBfloat  d2zdu2;        /*                                         */
   DBfloat  dsdu;          /* Length of tangent (dr/du)               */
   DBfloat  tx,ty,tz;      /* Normalised tangent                      */
   DBfloat  px,py,pz;      /* Curve segment plane (normalised)        */
   DBfloat  leng;          /* Length of n_p                           */
   DBfloat  kappa;         /* Curavture for offset= 0                 */
   DBfloat  dsdu3;         /* dsdu**3                                 */
   DBfloat  bx,by,bz;      /* Binormal                                */
   DBfloat  nx,ny,nz;      /* Principal normal                        */
   DBfloat  tmpx,tmpy,tmpz;/* Temporary vector                        */
   DBfloat  npx,npy,npz;   /* Temporary vector                        */
   DBfloat  tmp;           /*                                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur217 Enter*varkon_seg_offset Non-offset pt %f %f %f \n",
        r.x_gm,r.y_gm, r.z_gm );
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Input data to local variables.                                   */
/* Check that n_p is normalised.                                    */
/*                                                                  */
/*                                                                 !*/

  x      = r.x_gm;
  y      = r.y_gm;
  z      = r.z_gm;
  dxdu   = drdt.x_gm;
  dydu   = drdt.y_gm;
  dzdu   = drdt.z_gm;
  d2xdu2 = d2rdt2.x_gm;
  d2ydu2 = d2rdt2.y_gm;
  d2zdu2 = d2rdt2.z_gm;

  px     = n_p.x_gm;
  py     = n_p.y_gm;
  pz     = n_p.z_gm;

  leng = SQRT(px*px + py*py + pz*pz);
  if (fabs(leng-1.0) > 0.0000001 )
     {
     sprintf(errbuf,"(n_p)%% varkon_seg_offset (sur714)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* 2. Point in offset                                               */
/* __________________                                               */
/*                                                                  */
/* Calculate length of (dxdu,dydu,dzdu), the normalised             */
/* tangent (tx,ty,tz) and the offset point as the cross             */
/* product of the curve plane normal (px,py,pz) and the             */
/* normalised tangent.                                              */
/*                                                                 !*/

   dsdu = SQRT(dxdu*dxdu + dydu*dydu + dzdu*dzdu);

   if ( dsdu < 1e-14 ) dsdu = 1e-10;

   tx = dxdu/dsdu; ty = dydu/dsdu; tz = dzdu/dsdu;

   p_r_o->x_gm = x + offset*(ty*pz - py*tz);
   p_r_o->y_gm = y + offset*(tz*px - pz*tx);
   p_r_o->z_gm = z + offset*(tx*py - px*ty);

/*!                                                                 */
/* Goto nomore if evltyp= EVC_R.                                    */
/*                                                                 !*/

   if ( evltyp == EVC_R ) goto nomore;


/*!                                                                 */
/* 3. First derivative in offset                                    */
/* ____________________________                                     */
/*                                                                  */
/* Calculate kappa for offset= 0 (kappa= 0 for straight line)       */
/* Calulate the binormal and the principal normal                   */
/* product of the curve plane normal (px,py,pz) and the             */
/* normalised tangent.                                              */
/*                                                                 !*/

   tmpx  = dydu*d2zdu2-dzdu*d2ydu2;
   tmpy  = dzdu*d2xdu2-dxdu*d2zdu2;
   tmpz  = dxdu*d2ydu2-dydu*d2xdu2;
   tmp   = tmpx*tmpx + tmpy*tmpy + tmpz*tmpz;
   dsdu3 = dsdu*dsdu*dsdu;

   if ( tmp > TOL1 ) kappa = SQRT(tmp)/dsdu3;
   else              kappa = 0.0;

    bx = tmpx/dsdu3;
    by = tmpy/dsdu3;
    bz = tmpz/dsdu3;

   if ( kappa > 1E-15 )
     {
     bx /= kappa;
     by /= kappa;
     bz /= kappa;
     }
   nx = by*tz - ty*bz;
   ny = bz*tx - tz*bx;
   nz = bx*ty - tx*by;
   npx = ny*pz - py*nz;
   npy = nz*px - pz*nx;
   npz = nx*py - px*ny;

   p_drdt_o->x_gm = dxdu + offset*kappa*dsdu*npx;
   p_drdt_o->y_gm = dydu + offset*kappa*dsdu*npy;
   p_drdt_o->z_gm = dzdu + offset*kappa*dsdu*npz;


   if ( evltyp == EVC_R + EVC_DR || evltyp == EVC_DR ) goto nomore;

/*!                                                                 */
/* 3. Second derivative in offset                                   */
/* _____________________________                                    */
/*                                                                  */
/*  .... Not yet implemented .....                                  */
/*                                                                 !*/


     sprintf(errbuf,"%% ");
       varkon_erinit();
     return(varkon_erpush("SU2533",errbuf));


nomore: /*! Label nomore: End of calculation for case evltyp= .... !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur217 Exit *varkon_seg_offset     Offset pt %f %f %f \n",
        p_r_o->x_gm,p_r_o->y_gm, p_r_o->z_gm );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

