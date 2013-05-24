/*****************************************************************************/
/*                                                                           */
/*  File: geevalnc.c                                                        */
/*                                                                           */
/*  This file includes:                                                      */
/*                                                                           */ 
/*  GEevalnc()   Evaluator for NURBS curve segment(span)                     */
/*  GEevaluvnc() Evaluator in UV space for NURBS curve segment(span)         */
/*                                                                           */
/*  This file is part of the VARKON Geometry Library.                        */
/*  URL:                                                                     */
/*                                                                           */
/*  This library is free software; you can redistribute it and/or            */
/*  modify it under the terms of the GNU Library General Public              */
/*  License as published by the Free Software Foundation; either             */
/*  version 2 of the License, or (at your option) any later                  */
/*  version.                                                                 */
/*                                                                           */
/*  This library is distributed in the hope that it will be                  */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied               */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                  */
/*  PURPOSE.  See the GNU Library General Public License for more            */
/*  details.                                                                 */
/*                                                                           */
/*  You should have received a copy of the GNU Library General               */
/*  Public License along with this library; if not, write to the             */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,                 */
/*  MA 02139, USA.                                                           */
/*                                                                           */
/*****************************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"



                                 /* MAX_NURBD ,max NURBS degree (surdef.h).  */
                                         
  static DBstatus DersBasisFuns  (
  DBint i,                       /* Offset in knot vector                    */
  DBfloat t_seg,                 /* Local parameter value                    */         
  DBshort p,                     /* NURBS degree (order-1)                   */
  DBint n,                       /* Requested derivatives                    */             
  DBfloat *U ,                   /* Knot vector                              */
  DBfloat ders[3][MAX_NURBD]);   /* basis values[der_no][index] (1)          */
  
  /* (1) At most degree+1 of the basis values are nonzero */


/*****************************************************************************/
/*                                                                           */
/*  Function: GEevalnc                                     File: geevalnc.c  */
/*  =======================================================================  */
/*                                                                           */
/*  Purpose                                                                  */
/*  -------                                                                  */
/*                                                                           */
/*  The function calculates the coordinates and derivatives                  */
/*  for a point on a NURBS curve.                                            */
/*                                                                           */
/*  (C) 2002-06-24 Sören Larsson, Örebro University                          */
/*                                                                           */
/*  Revisions                                                                */
/*  ---------                                                                */
/*  2002-06-24   Originally written                                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/




/*!****************** Function ***********************************************/
/*                                                                           */
    DBstatus GEevalnc(

/*-------------- Argument declarations --------------------------------------*/
/*                                                                           */
/* In:                                                                       */
   DBCurve *p_cur,       /* Curve                             (ptr)          */
   DBSeg   *p_seg,       /* Curve segment                     (ptr)          */
   EVALC   *p_evalc )    /* Curve coordinates & derivatives   (ptr)          */
/* Out:                                                                      */
/*       Data to p_evalc                                                     */
/*---------------------------------------------------------------------------*/


   { /* Start of function */

/*!------------------------ Internal variables ------------------------------*/
        
       
   bool     offseg;        /* Flag for offset segment                        */
   bool     ratseg;        /* Flag for rational segment                      */
   DBfloat  offset;        /* Offset value                                   */      
   DBVector n_p;           /* Curve segment plane normal                     */
   DBint    status;        /* Error code from called function                */
   DBint    i;             /* loop variable                                  */
   
   DBVector r;             /* Position (non-offset)                          */
   DBVector drdu;          /* First   derivative with respect to u           */
   DBVector d2rdu2;        /* Second  derivative with respect to u           */
   DBVector d3rdu3;        /* Third  derivative with respect to u            */
   
   DBHvector w;            /* Position, homogenus coordinates                */
   DBHvector dwdu;         /* 1:st der.with respect to u, hom. coord.        */
   DBHvector d2wdu2;       /* 2:nd der.with respect to u, hom. coord.        */
   DBHvector d3wdu3;       /* 3:rd der.with respect to u, hom. coord.        */

   DBshort   degree;       /* Nurbs degree                                   */
   DBint     offs_p;       /* offset in control points array                 */
   DBint     offs_u;       /* offset in knots array                          */  
   DBHvector *P;           /* Pointer to control points for curve            */
   DBfloat   *U;           /* Pointer to knots for curve                     */
   DBfloat    u;           /* local parameter value */

   DBfloat   invHom;       /* 1/w.w_gm)                                      */
   DBfloat   invHomQuad;   /* 1/((w.w_gm)*(w.w_gm))                          */
   DBfloat   invHomCub;    /* 1/((w.w_gm)*(w.w_gm)*(w.w_gm))                 */
 
   char     errbuf[80];         /* String for error message fctn erpush      */

   DBfloat ders[3][MAX_NURBD];  /* basis values[der_no][index]               */
   DBint   no_der;              /* no off requested derivatives              */
   DBfloat kappa;               /* Curvature                                 */
   DBfloat tx,ty,tz;            /* Unit tangent                              */
   DBfloat bx,by,bz;            /* Binormal                                  */
   DBfloat nx,ny,nz;            /* Principal normal                          */
   DBfloat dsdu,dsdu3,dsdu6;    /* 1:st derivative of s with respect to u    */
   DBfloat d2sdu2;              /* 2:nd derivative of s with respect to u    */
   DBfloat xoff,yoff,zoff;      /* Position in offset                        */
   DBfloat npy,npx,npz;         /* Direction of tangent in offset            */
   DBfloat dxoff,dyoff,dzoff;   /* 1:st der. with respect to u, in offset    */ 
   DBfloat d2xoff,d2yoff,d2zoff;/* 2:nd der. with respect to u, in offset    */
   DBfloat ntpx,ntpy,ntpz;      /* Principal normal in offset                */
   DBfloat dsoffs,dsoff3;       /* 1:st derivative of s in offset            */
   DBfloat kapofs;              /* Curvature in offset                       */
   DBfloat dkappadu;            /* 1:st derivative of Curvature              */
   DBfloat tmpx,tmpy,tmpz,tmp;
   DBfloat ux,uy,uz,vx,vy,vz;

/*-------------------end-of-declarations-------------------------------------*/

   
/*
*** Copy to local variables
*/
   degree  =  p_seg -> nurbs_degree;  
   offs_p  =  p_seg -> offset_cpts;
   offs_u  =  p_seg -> offset_knots;
   P       =  p_seg -> cpts_c;         
   U       =  p_seg -> knots_c;        
   u       =  p_evalc->t_local;

/*
*** Let offset flag be true and retrieve curve plane if the
*** curve is in offset.
*/
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
      }

/*
*** Is the segment rational?     
*/
   ratseg = FALSE;
   for (i=0 ;i <= degree  ; i++ )
      {
      if (ABS((P + offs_p + i)->w_gm - 1.0) > COMPTOL) 
         {
         ratseg = TRUE;
         break;
         }
      }

/*
*** How many derivatives do we need in "DeersBasisFuns"?
*/
   no_der = -1;
   if (  p_evalc ->evltyp & EVC_R   ) no_der = 0;
   if (  p_evalc ->evltyp & EVC_DR  ) no_der = 1;

   if (offseg == TRUE ||
       (p_evalc ->evltyp & (EVC_D2R + EVC_PN + EVC_BN + EVC_KAP))) no_der = 2;

   if (offseg == TRUE &&
       (p_evalc ->evltyp & (EVC_D2R + EVC_PN + EVC_BN + EVC_KAP))) no_der = 3;

   if (no_der < 0) 
      {
      sprintf(errbuf," - evltyp - %%GEevalnc");
      return(varkon_erpush("SU2993",errbuf));
      }

/*
*** Calculate basis values
*/
   status = DersBasisFuns (offs_u,u,degree,no_der,p_seg->knots_c,ders);

/*
*** Calculate position 
*** (always needed for a rational curve)    
*/
   if ((p_evalc ->evltyp & EVC_R) || (ratseg == TRUE)) 
      {
      w.x_gm = w.y_gm = w.z_gm = w.w_gm = 0.0;

      for (i=0;i<=degree;i++)
         {
         w.x_gm = w.x_gm + (P + offs_p + i)->x_gm * ders[0][i];
         w.y_gm = w.y_gm + (P + offs_p + i)->y_gm * ders[0][i];
         w.z_gm = w.z_gm + (P + offs_p + i)->z_gm * ders[0][i];
         w.w_gm = w.w_gm + (P + offs_p + i)->w_gm * ders[0][i];     
         }

      if (ratseg == TRUE)
         {
         invHom = 1.0 / w.w_gm;
         p_evalc ->r.x_gm = r.x_gm = w.x_gm * invHom;
         p_evalc ->r.y_gm = r.y_gm = w.y_gm * invHom;
         p_evalc ->r.z_gm = r.z_gm = w.z_gm * invHom;
         }
      else
         {
         w.w_gm = 1.0; 
         p_evalc ->r.x_gm = r.x_gm = w.x_gm;
         p_evalc ->r.y_gm = r.y_gm = w.y_gm;
         p_evalc ->r.z_gm = r.z_gm = w.z_gm;
         }
      if ( no_der == 0 ) return(SUCCED);
      }
      /* end position */
/*
*** Calculate first derivative
*/
   if ((p_evalc ->evltyp & (EVC_DR+EVC_PN+EVC_BN+EVC_KAP)) || (offseg==TRUE )) 
      {
      dwdu.x_gm = dwdu.y_gm = dwdu.z_gm = dwdu.w_gm = 0.0;

      for (i=0 ;i <= degree ;i++ ) 
         {
         dwdu.x_gm = dwdu.x_gm + (P + offs_p + i)->x_gm * ders[1][i];
         dwdu.y_gm = dwdu.y_gm + (P + offs_p + i)->y_gm * ders[1][i];
         dwdu.z_gm = dwdu.z_gm + (P + offs_p + i)->z_gm * ders[1][i];
         dwdu.w_gm = dwdu.w_gm + (P + offs_p + i)->w_gm * ders[1][i];
         }

      if (ratseg == TRUE)
         {
         invHomQuad = invHom * invHom; 
         drdu.x_gm = (w.w_gm * dwdu.x_gm - dwdu.w_gm * w.x_gm) * invHomQuad;
         drdu.y_gm = (w.w_gm * dwdu.y_gm - dwdu.w_gm * w.y_gm) * invHomQuad;
         drdu.z_gm = (w.w_gm * dwdu.z_gm - dwdu.w_gm * w.z_gm) * invHomQuad;
         }
      else
         {
         drdu.x_gm = dwdu.x_gm;
         drdu.y_gm = dwdu.y_gm;
         drdu.z_gm = dwdu.z_gm;
         }

      p_evalc ->drdt.x_gm = drdu.x_gm;
      p_evalc ->drdt.y_gm = drdu.y_gm;
      p_evalc ->drdt.z_gm = drdu.z_gm;

      if ( no_der == 1 ) return(SUCCED);
      }
      /* end 1:st derivative */
/*
*** Calculate second derivative
*/
   if ( (p_evalc->evltyp & (EVC_D2R+EVC_PN+EVC_BN+EVC_KAP)) || (offseg==TRUE )) 
      {
      if (degree > 1)
         {
         d2wdu2.x_gm=0.0;
         d2wdu2.y_gm=0.0;
         d2wdu2.z_gm=0.0;
         d2wdu2.w_gm=0.0;

         for (i=0 ;i <= degree ;i++ )
            {
            d2wdu2.x_gm = d2wdu2.x_gm + (P + offs_p + i)->x_gm * ders[2][i];
            d2wdu2.y_gm = d2wdu2.y_gm + (P + offs_p + i)->y_gm * ders[2][i];
            d2wdu2.z_gm = d2wdu2.z_gm + (P + offs_p + i)->z_gm * ders[2][i];
            d2wdu2.w_gm = d2wdu2.w_gm + (P + offs_p + i)->w_gm * ders[2][i];
            }

         if (ratseg == TRUE)
            {
            invHomCub = invHomQuad * invHom;

            d2rdu2.x_gm = 
             (w.w_gm  * (w.w_gm * d2wdu2.x_gm - w.x_gm * d2wdu2.w_gm) + 2 * 
            dwdu.w_gm * (w.x_gm * dwdu.w_gm   - w.w_gm * dwdu.x_gm)) 
            * invHomCub;

            d2rdu2.y_gm = 
             (w.w_gm  * (w.w_gm * d2wdu2.y_gm - w.y_gm * d2wdu2.w_gm) + 2 * 
            dwdu.w_gm * (w.y_gm * dwdu.w_gm   - w.w_gm * dwdu.y_gm)) 
            * invHomCub;

            d2rdu2.z_gm = 
             (w.w_gm  * (w.w_gm * d2wdu2.z_gm - w.z_gm * d2wdu2.w_gm) + 2 * 
            dwdu.w_gm * (w.z_gm * dwdu.w_gm   - w.w_gm * dwdu.z_gm)) * 
            invHomCub;
            }
         else
            {
             d2rdu2.x_gm = d2wdu2.x_gm;
             d2rdu2.y_gm = d2wdu2.y_gm;
             d2rdu2.z_gm = d2wdu2.z_gm;
            }
         }
      else d2rdu2.x_gm = d2rdu2.y_gm = d2rdu2.z_gm = d2wdu2.w_gm = 0.0;

      p_evalc->d2rdt2.x_gm = d2rdu2.x_gm ;
      p_evalc->d2rdt2.y_gm = d2rdu2.y_gm ;
      p_evalc->d2rdt2.z_gm = d2rdu2.z_gm ;
      }
      /* end 2:nd derivative */

/*
*** Do we need to calculate kappa ?
*/
   if (p_evalc ->evltyp & (EVC_KAP + EVC_BN + EVC_PN) || (offseg == TRUE ))
      {
      tmpx  = drdu.y_gm * d2rdu2.z_gm - drdu.z_gm * d2rdu2.y_gm;
      tmpy  = drdu.z_gm * d2rdu2.x_gm - drdu.x_gm * d2rdu2.z_gm;
      tmpz  = drdu.x_gm * d2rdu2.y_gm - drdu.y_gm * d2rdu2.x_gm;

      tmp   = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;

      /*
      *** dr/du = (dxdu,dydu,dzdu). 
      *** The tangent vector, T = dr/ds is the unit vector with same
      *** direction as dr/du. 
      *** Calculate the length of dr/du = dsdu.
      */

      dsdu   =  SQRT( drdu.x_gm * drdu.x_gm  + 
                      drdu.y_gm * drdu.y_gm  + 
                      drdu.z_gm * drdu.z_gm) ;

      dsdu3 = dsdu*dsdu*dsdu;
      if ( dsdu3 < 1e-14 ) dsdu3 = 1e-10;

      if ( tmp > COMPTOL ) kappa = SQRT(tmp)/dsdu3;
      else                 kappa = 0.0;

      p_evalc->kappa = kappa;
      }
      /* end kappa */

/*
*** The binormal. This is calculated as:
*** drdu X d2rdu2 / dsdu ** 3 / kappa. See Faux-Pratt p. 100.
*/
   if ( p_evalc ->evltyp & (EVC_BN+EVC_PN) || (offseg == TRUE ))
      {
      bx = tmpx/dsdu3;
      by = tmpy/dsdu3;
      bz = tmpz/dsdu3;

      if ( kappa > COMPTOL )
         {
         bx /= kappa;
         by /= kappa;
         bz /= kappa;
         }
      p_evalc->bn_x = bx;
      p_evalc->bn_y = by;
      p_evalc->bn_z = bz;

      p_evalc->b_norm.x_gm = bx;
      p_evalc->b_norm.y_gm = by;
      p_evalc->b_norm.z_gm = bz;
      }
      /*end binormal*/

/*
*** Principal normal, N = B X T.
*/
   if ( (p_evalc ->evltyp & EVC_PN) ||(offseg == TRUE ))
      {
      tx = drdu.x_gm/dsdu;
      ty = drdu.y_gm/dsdu; 
      tz = drdu.z_gm/dsdu;

      nx = by*tz - ty*bz;
      ny = bz*tx - tz*bx;
      nz = bx*ty - tx*by;

      p_evalc->pn_x = nx;
      p_evalc->pn_y = ny;
      p_evalc->pn_z = nz;

      p_evalc->p_norm.x_gm = nx;
      p_evalc->p_norm.y_gm = ny;
      p_evalc->p_norm.z_gm = nz;
      }
      /*end principal normal*/

/*
*** Calculate offset coordinates and derivatives if the curve
*** segment is in offset.
*/

   if (offseg == TRUE )
      {

      /*
      *** Calculate the direction of the offset vector as the cross-product 
      *** between the tangent to the curve and the normal of the curve-plane.
      *** T and P are normalized, thus the result of T X P is a
      *** normalized vector with same direction as the curve normal,
      *** possibly with opposite sign.
      *** When the curve turns right in the curve plane, T X P = N but
      *** when the curve turbs left T X P = -N. This is varkons definition
      *** of the concept of offset. Thus positive offset is always the right
      *** side of the curve in the curve plane.
      *** Length of offset vector = the offset value of the curve.
      */

      if (p_evalc ->evltyp & EVC_R) 
         {   
         p_evalc->r.x_gm= xoff= r.x_gm + offset*(ty * n_p.z_gm- n_p.y_gm * tz);
         p_evalc->r.y_gm= yoff= r.y_gm + offset*(tz * n_p.x_gm- n_p.z_gm * tx);
         p_evalc->r.z_gm= zoff= r.z_gm + offset*(tx * n_p.y_gm- n_p.x_gm * ty);
         }

      if ((p_evalc ->evltyp & (EVC_DR+EVC_KAP)))  
         {

         /*
         *** The offset tangent has same direction as the original tangent, T.
         *** But, since offset is counted positive on the right side of the
         *** curve and negative on the left side of the curve, the sign of the
         *** tangent must be adjusted.
         *** compare with the calculation of coordinates in offset.
         *** The direction of the offset tangent is calculated as N X P.
         */
         npx = ny * n_p.z_gm - n_p.y_gm * nz;
         npy = nz * n_p.x_gm - n_p.z_gm * nx;
         npz = nx * n_p.y_gm - n_p.x_gm * ny;
     
         /*
         *** The tangent in offset = dr/du + offset*kappa * dsdu*(npx,npy,npz). 
         *** Simply a factor kappa*offset greater. The sign of np eliminates 
         *** the sign of offset, which make the result correct.
         */
         p_evalc->drdt.x_gm = dxoff = drdu.x_gm + offset * kappa * dsdu * npx; 
         p_evalc->drdt.y_gm = dyoff = drdu.y_gm + offset * kappa * dsdu * npy;  
         p_evalc->drdt.z_gm = dzoff = drdu.z_gm + offset * kappa * dsdu * npz;  

         }


      if ((p_evalc ->evltyp & (EVC_D2R+EVC_KAP)))  
         {
         
         /* Third derivative */
         
         if (degree>2)
            {

            d3wdu3.x_gm=0.0;
            d3wdu3.y_gm=0.0;
            d3wdu3.z_gm=0.0;
            d3wdu3.w_gm=0.0;
   
            for (i=0 ;i <= degree ;i++ )  
               {        
               d3wdu3.x_gm = d3wdu3.x_gm + (P + offs_p + i)->x_gm * ders[3][i];
               d3wdu3.y_gm = d3wdu3.y_gm + (P + offs_p + i)->y_gm * ders[3][i];
               d3wdu3.z_gm = d3wdu3.z_gm + (P + offs_p + i)->z_gm * ders[3][i];
               d3wdu3.w_gm = d3wdu3.w_gm + (P + offs_p + i)->w_gm * ders[3][i];
               }
         
            if (ratseg==TRUE)
               {      
               d3rdu3.x_gm = 
               (d3wdu3.x_gm -3.0 * d2rdu2.x_gm * dwdu.w_gm - 
               3.0 * drdu.x_gm * d2wdu2.w_gm - r.x_gm * d3wdu3.w_gm) * invHom;
         
               d3rdu3.y_gm = 
               (d3wdu3.y_gm -3.0 * d2rdu2.y_gm * dwdu.w_gm - 
               3.0 * drdu.y_gm * d2wdu2.w_gm - r.y_gm * d3wdu3.w_gm) * invHom;

               d3rdu3.z_gm = 
               (d3wdu3.z_gm -3.0 * d2rdu2.z_gm * dwdu.w_gm - 
               3.0 * drdu.z_gm * d2wdu2.w_gm - r.z_gm * d3wdu3.w_gm) * invHom;
               }
            else
               {
               d3rdu3.x_gm = d3wdu3.x_gm; 
               d3rdu3.y_gm = d3wdu3.y_gm;
               d3rdu3.z_gm = d3wdu3.z_gm;
               }
            }
         else d3rdu3.x_gm = d3rdu3.y_gm = d3rdu3.z_gm = d3wdu3.x_gm = 
              d3wdu3.y_gm = d3wdu3.z_gm = d3wdu3.w_gm = 0.0;

         /* Second derivative of s with respect to u. */
            d2sdu2 = d2rdu2.x_gm * tx + d2rdu2.y_gm * ty + d2rdu2.z_gm * tz;
         
         /* dr/du X d2r/du2. */
            vx = drdu.y_gm * d2rdu2.z_gm - drdu.z_gm * d2rdu2.y_gm;
            vy = drdu.z_gm * d2rdu2.x_gm - drdu.x_gm * d2rdu2.z_gm;
            vz = drdu.x_gm * d2rdu2.y_gm - drdu.y_gm * d2rdu2.x_gm;
         
         /* dr/du X d3r/du3 */
            ux = drdu.y_gm * d3rdu3.z_gm - drdu.z_gm * d3rdu3.y_gm;
            uy = drdu.z_gm * d3rdu3.x_gm - drdu.x_gm * d3rdu3.z_gm;
            uz = drdu.x_gm * d3rdu3.y_gm - drdu.y_gm * d3rdu3.x_gm;
         
         /* Scalar product ( dr/du ! dr/du )**3 */
            dsdu6 = dsdu3 * dsdu3;
         /*
         *** dkappa/du.
         */
            dkappadu = vx * ux + vy * uy + vz * uz;

            if ( dsdu6 > TOL1 && kappa > TOL1 )
               {
               dkappadu = dkappadu/dsdu6/kappa;
               }
            else
               {
               dkappadu = n_p.x_gm * ux + n_p.y_gm * uy + n_p.z_gm * uz;
               dkappadu = dkappadu/dsdu3;
               }

            dkappadu = dkappadu - 3.0 * kappa * 
            (d2rdu2.x_gm * drdu.x_gm + d2rdu2.y_gm * drdu.y_gm + 
             d2rdu2.z_gm * drdu.z_gm) / (dsdu * dsdu);
         /*
         *** Normalen i offset korrigerad för tecknet på offset.
         */
            ntpx = ty * n_p.z_gm - n_p.y_gm * tz;
            ntpy = tz * n_p.x_gm - n_p.z_gm * tx;
            ntpz = tx * n_p.y_gm - n_p.x_gm * ty;
         
         /*
         *** Calculate second derivative
         */
            p_evalc->d2rdt2.x_gm = d2xoff = 
            d2rdu2.x_gm + offset*(dkappadu * dsdu * npx +
            kappa * (d2sdu2 * npx - dsdu * dsdu * kappa * ntpx));
            
            p_evalc->d2rdt2.y_gm = d2yoff = 
            d2rdu2.y_gm + offset*(dkappadu * dsdu * npy +
            kappa * (d2sdu2 * npy - dsdu * dsdu * kappa * ntpy));
            
            p_evalc->d2rdt2.z_gm = d2zoff = 
            d2rdu2.z_gm + offset*(dkappadu*dsdu*npz +
            kappa * (d2sdu2 * npz - dsdu * dsdu*kappa*ntpz));
      }
    
      /*
      *** Kappa in offset.
      */

      if (p_evalc ->evltyp & EVC_KAP)  
         {
         tmpx = dyoff * d2zoff - dzoff * d2yoff;
         tmpy = dzoff * d2xoff - dxoff * d2zoff;
         tmpz = dxoff * d2yoff - dyoff * d2xoff;

         tmp = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;

         if ( tmp > COMPTOL )
            {
            kapofs  = SQRT(tmp);
            dsoffs  = SQRT(dxoff*dxoff + dyoff*dyoff + dzoff*dzoff);
            dsoff3  = dsoffs * dsoffs * dsoffs;
            kapofs /= dsoff3;
            }
         else kapofs = 0.0;
      
         p_evalc->kappa = kapofs;
         }

/*
*** Binormal and Pricipal normal, need not to be recalculated. 
*** (not dependet on offset value)
***
*** The binormal has same direction as for offset = 0. Both
*** The principal-normal, N = B X T. Is also same as for offset = 0.
*/

      } /* end offset curve */

   return(SUCCED);

   } /* End of function */

/*****************************************************************************/




/*****************************************************************************/
/*                                                                           */
/*  Function: GEevaluvnc                                   File: geevalnc.c  */
/*  =======================================================================  */
/*                                                                           */
/*  Purpose                                                                  */
/*  -------                                                                  */
/*                                                                           */
/*  The function calculates the coordinates and derivatives in uv space      */
/*  for a point on a NURBS uv curve                                          */
/*                                                                           */
/*  (C) 2006-11-07 Sören Larsson, Örebro University                          */
/*                                                                           */
/*  Revisions                                                                */
/*  ---------                                                                */
/*  2006-11-07   Originally written                                          */
/*  2007-02-01   bugfix, u and v was not always calculated, Sören L.         */
/*                                                                           */
/*****************************************************************************/




/*!****************** Function ***********************************************/
/*                                                                           */
    DBstatus GEevaluvnc(

/*-------------- Argument declarations --------------------------------------*/
/*                                                                           */
/* In:                                                                       */
   DBCurve *p_cur,       /* Curve                             (ptr)          */
   DBSeg   *p_seg,       /* Curve segment                     (ptr)          */
   EVALC   *p_evalc )    /* Curve coordinates & derivatives   (ptr)          */
/* Out:                                                                      */
/*       Data to p_evalc                                                     */
/*---------------------------------------------------------------------------*/


   { /* Start of function */

/*!------------------------ Internal variables ------------------------------*/
        
   bool     ratseg;        /* Flag for rational segment                      */
   DBint    status;        /* Error code from called function                */
   DBint    i;             /* loop variable                                  */
   
   DBVector r;             /* Position (non-offset)                          */
   DBVector drdu;          /* First   derivative with respect to u           */
   DBVector d2rdu2;        /* Second  derivative with respect to u           */
   
   DBHvector w;            /* Position, homogenus coordinates                */
   DBHvector dwdu;         /* 1:st der.with respect to u, hom. coord.        */
   DBHvector d2wdu2;       /* 2:nd der.with respect to u, hom. coord.        */

   DBshort   degree;       /* Nurbs degree                                   */
   DBint     offs_p;       /* offset in control points array                 */
   DBint     offs_u;       /* offset in knots array                          */  
   DBHvector *P;           /* Pointer to control points for curve            */
   DBfloat   *U;           /* Pointer to knots for curve                     */
   DBfloat    u;           /* local parameter value */

   DBfloat   invHom;       /* 1/w.w_gm)                                      */
   DBfloat   invHomQuad;   /* 1/((w.w_gm)*(w.w_gm))                          */
   DBfloat   invHomCub;    /* 1/((w.w_gm)*(w.w_gm)*(w.w_gm))                 */
 
   char     errbuf[80];         /* String for error message fctn erpush      */

   DBfloat ders[3][MAX_NURBD];  /* basis values[der_no][index]               */
   DBint   no_der;              /* no off requested derivatives              */

/*-------------------end-of-declarations-------------------------------------*/

/*
*** Let flag for UV segment be true.
*/   

   if ( p_seg->typ  ==  UV_NURB_SEG )
        p_evalc->surpat = TRUE;
   else
      {    
      sprintf(errbuf,                       
      "(not UV_NURB_SEG)%%varkon_seg_uveval");
      return(varkon_erpush("SU2993",errbuf));
      }
        
/*
*** Copy to local variables
*/
   degree  =  p_seg -> nurbs_degree;  
   offs_p  =  p_seg -> offset_cpts;
   offs_u  =  p_seg -> offset_knots;
   P       =  p_seg -> cpts_c;         
   U       =  p_seg -> knots_c;        
   u       =  p_evalc->t_local;
/*                        
*** Is the segment rational?     
*/
   ratseg = FALSE;
   for (i=0 ;i <= degree  ; i++ )
      {
      if (ABS((P + offs_p + i)->w_gm - 1.0) > COMPTOL) 
         {
         ratseg = TRUE;
         break;
         }
      }

/*                        
*** Calculate basis values    
*/
   no_der = 2;
   status = DersBasisFuns (offs_u,u,degree,no_der,p_seg->knots_c,ders);

/*                        
*** Calculate position
*/

   w.x_gm = w.y_gm = w.z_gm = w.w_gm = 0.0;

   for (i=0;i<=degree;i++)
      {
      w.x_gm = w.x_gm + (P + offs_p + i)->x_gm * ders[0][i];
      w.y_gm = w.y_gm + (P + offs_p + i)->y_gm * ders[0][i];
      w.w_gm = w.w_gm + (P + offs_p + i)->w_gm * ders[0][i];
      }

   if (ratseg == TRUE)
      {
      invHom = 1.0 / w.w_gm;
      p_evalc -> u = r.x_gm = w.x_gm * invHom;
      p_evalc -> v = r.y_gm = w.y_gm * invHom;
      }
   else
      {
      w.w_gm = 1.0;
      p_evalc -> u = r.x_gm = w.x_gm;
      p_evalc -> v = r.y_gm = w.y_gm;
      }

/*                        
*** Calculate first derivative     
*/

   dwdu.x_gm = dwdu.y_gm = dwdu.z_gm = dwdu.w_gm = 0.0;
 
   for (i=0 ;i <= degree ;i++ )
      {
      dwdu.x_gm = dwdu.x_gm + (P + offs_p + i)->x_gm * ders[1][i];
      dwdu.y_gm = dwdu.y_gm + (P + offs_p + i)->y_gm * ders[1][i];
      dwdu.w_gm = dwdu.w_gm + (P + offs_p + i)->w_gm * ders[1][i];
      }
            
   if (ratseg == TRUE)
      {
      invHomQuad = invHom * invHom;
      drdu.x_gm = (w.w_gm * dwdu.x_gm - dwdu.w_gm * w.x_gm) * invHomQuad;
      drdu.y_gm = (w.w_gm * dwdu.y_gm - dwdu.w_gm * w.y_gm) * invHomQuad;
      }
   else
      {
      drdu.x_gm = dwdu.x_gm;
      drdu.y_gm = dwdu.y_gm;
      }
         
   p_evalc ->u_t = drdu.x_gm;
   p_evalc ->v_t = drdu.y_gm;

/*
*** Calculate second derivative
*/
   if (degree > 1)
      {
      d2wdu2.x_gm=0.0;
      d2wdu2.y_gm=0.0;
      d2wdu2.w_gm=0.0;
  
      for (i=0 ;i <= degree ;i++ )
         {
         d2wdu2.x_gm = d2wdu2.x_gm + (P + offs_p + i)->x_gm * ders[2][i];
         d2wdu2.y_gm = d2wdu2.y_gm + (P + offs_p + i)->y_gm * ders[2][i];
         d2wdu2.w_gm = d2wdu2.w_gm + (P + offs_p + i)->w_gm * ders[2][i];
         }

      if (ratseg == TRUE)
         {
         invHomCub = invHomQuad * invHom;

         d2rdu2.x_gm =
          (w.w_gm  * (w.w_gm * d2wdu2.x_gm - w.x_gm * d2wdu2.w_gm) + 2 *
          dwdu.w_gm * (w.x_gm * dwdu.w_gm   - w.w_gm * dwdu.x_gm))
          * invHomCub;

         d2rdu2.y_gm =
          (w.w_gm  * (w.w_gm * d2wdu2.y_gm - w.y_gm * d2wdu2.w_gm) + 2 *
          dwdu.w_gm * (w.y_gm * dwdu.w_gm   - w.w_gm * dwdu.y_gm))
          * invHomCub;
         }
      else
         {
         d2rdu2.x_gm = d2wdu2.x_gm;
         d2rdu2.y_gm = d2wdu2.y_gm;
         }
      }
   else d2rdu2.x_gm = d2rdu2.y_gm = d2rdu2.z_gm = d2wdu2.w_gm = 0.0;

   p_evalc->u_t2 = d2rdu2.x_gm ;
   p_evalc->v_t2 = d2rdu2.y_gm ;

   return(SUCCED);

   } /* End of function */

/*****************************************************************************/




/*****************************************************************************/
/*                                                                           */
/*  Local function: DersBasisFuns                          File: geevalnc.c  */
/*  =======================================================================  */
/*                                                                           */
/*  Purpose                                                                  */
/*  -------                                                                  */
/*                                                                           */
/*  The function calculates all nonzero basis functions for degree p         */
/*  and derivatives up to dervative no "n".                                  */
/*                                                                           */
/*  (C) 2002-06-15 Sören Larsson, Örebro University                          */
/*                                                                           */
/*  Revisions                                                                */
/*                                                                           */
/*  2000-06-15   Originally written                                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/



/*----------------------- Theory --------------------------------------------*/
/*                                                                           */
/*  the NURBS book, Algorithm A2.3                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/




/*!****************** Function ***********************************************/
/*                                                                           */
      static DBstatus    DersBasisFuns  (

/*-------------- Argument declarations --------------------------------------*/
/*                                                                           */
/* In:                                                                       */
   DBint i,                      /* Offset in knot vector                    */
   DBfloat u,                    /* Local parameter value                    */         
   DBshort p,                    /* NURBS degree (order-1)                   */
   DBint n,                      /* Requested no off derivatives             */             
   DBfloat *U,                   /* Knot vector                              */
/* Out:                                                                      */
   DBfloat ders[3][MAX_NURBD])   /* basis values[der_no][index]              */
/*                                  (der_no=0 is position)                   */      
/*---------------------------------------------------------------------------*/




{ /* Start of function */

/*!--------------- Internal variables  --------------------------------------*/
        
   
   DBint j,r,s1,s2,pk,rk,k,j1,j2;  
   DBfloat ndu[MAX_NURBD+1][MAX_NURBD+1];
   DBfloat a[2][MAX_NURBD+1];
   DBfloat left[MAX_NURBD];
   DBfloat right[MAX_NURBD];
   DBfloat saved;
   DBfloat temp;
   DBfloat d;
   DBfloat f;   /* parameter factor for "scaling" knots */
   
/*--------------end-of-declarations------------------------------------------*/       

   

/*                        
*** Adjust local parameter "u" to suite NURBS-algorithm    
*** (Theory, see "Implementation of NURBS curves into the varkon CAD system",
*** ISSN 1404-7225)
*/
  
f=1 / (U[i+1] - U[i]);

/* "move" the parameter to the correct segment */
u = u + U[i] * f;


/* NURBS book, Algorithm A2.3, adjusted to suit Varkon parameterisation */

ndu[0][0]=1.0;
for (j=1; j<=p; j++)
  {
  /* Knot values multipilcated (scaled) with f to suit 
     the varkon, local (1-unit) parameterisation for this span */
  left[j]  =  u - U[i+1-j]*f; 
  right[j] =  U[i+j]*f - u;  
  saved=0.0;
  for (r=0; r<j; r++)
    {                                      /* Lower triangle */
    ndu[j][r] = right[r+1] + left[j-r];
    temp = ndu[r][j-1] / ndu[j][r];
                                           /* Upper triangle */
    ndu[r][j] = saved + right[r+1] *temp;
    saved = left[j-r] * temp;
    }
  ndu[j][j] = saved;
  }

for (j=0; j<=p; j++) /*Load the basis functions */
  ders[0][j]=ndu[j][p];

/* This section computes the derivatives (Eq [2.9]) */
for (r=0; r<=p; r++)  /* Loop over function index */
  {
  s1=0; s2=1;  /*alternate rows in array a */
  a[0][0] = 1.0;
  /* loop to compute the kth derivative */
  for (k=1; k<=n; k++)
    {
    d = 0.0;
    rk = r-k; pk = p-k;
    if (r >= k)
    {
      a[s2][0] = a[s1][0] / ndu[pk+1][rk];
      d= a[s2][0] * ndu[rk][pk];
    }
    if (rk >= -1)  j1 = 1;
      else         j1 = -rk;

    if (r-1 <= pk) j2 = k-1;
      else         j2 = p-r;
    for (j=j1; j<=j2; j++)
      {
      a[s2][j] = (a[s1][j]-a[s1][j-1])/ndu[pk+1][rk+j];  
      d += a[s2][j] * ndu[rk+j][pk];
      }
    if (r<= pk)
      {
      a[s2][k] = -a[s1][k-1] / ndu[pk+1][r];
      d += a[s2][k] * ndu [r][pk];
      }
    ders[k][r] = d;
    j=s1; s1=s2; s2=j;  /* Switch rows */
    }
  }
/* Multiply through by the correct factors */
/* (Eq. [2.9]) */

r=p;
for (k=1; k<=n; k++)
  {
  for (j=0; j<=p; j++) ders[k][j] *= r;
  r*= (p-k);
  }

  return(SUCCED);
} 

/* End of function */

/*****************************************************************************/


