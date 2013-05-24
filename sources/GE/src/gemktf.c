/**********************************************************************
*
*    gemktf.c
*    ========
*
*    This file includes the following public functions:
*
*    GEmktf_3p()    Create general tf from 1 pos and 2 directions
*    GEmktf_2p()    Create general tf from 2 or 3 pos
*    GEmktf_1p()    Create general tf from 1 pos
*    GEmktf_ts()    Create translation/scaling tf
*    GEmktf_rs()    Create rotation/scaling tf
*    GEmktf_mirr()  Create mirroring tf
*    GEmktf_6p()    Create general tf from 2 pos and 4 directions
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://varkon.sourceforge.net
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

/*
***Include files.
*/
#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************/

        DBstatus GEmktf_3p(
        DBVector *por,
        DBVector *pv1,
        DBVector *pv2,
        DBTmat   *pt)

/*      Creates a basic->local transformation matrix from
 *      1 position and 2 direction vectors.
 *
 *      In: por = Pointer to origin
 *          pv1 = Pointer to vector for  X-axis
 *          pv2 = Pointer to vector for  XY-plane
 *
 *      Out: *pt = Transformation
 *
 *      (C)microform ab 1984-12-22 G.Liden
 *
 *      1999-04-06 Rewritten, J.Kjellander
 *      1999-12-18 Documentation changed G. Liden
 *
 ******************************************************!*/

 {
   DBTmat  m1,m2;
   DBVector pv3;

/*
***Normalisation and check of vector lengths
*/
   if ( GEnormalise_vector3D(pv1,pv1) != 0 )
     return(erpush("GE6012","GEmktf_3p"));

   if ( GEnormalise_vector3D(pv2,pv2) != 0 )
     return(erpush("GE6022","GEmktf_3p"));
/*
***The z axis is perpendicular to the xaxis pv1 and pv2
*/
   GEvector_product(pv1,pv2,&pv3);

   if ( GEnormalise_vector3D(&pv3,&pv3) != 0 )
     return(erpush("GE6032","GEmktf_3p"));
/*
***The y axis as the cross product of the x- and z-axis
*/
   GEvector_product(&pv3,pv1,pv2);

   GEnormalise_vector3D(pv2,pv2);
/*
***Elements to m1=rotation and m2=translation
*/
   m1.g11 = pv1->x_gm;
   m1.g12 = pv1->y_gm;
   m1.g13 = pv1->z_gm;
   m1.g14 = 0.0;

   m1.g21 = pv2->x_gm;
   m1.g22 = pv2->y_gm;
   m1.g23 = pv2->z_gm;
   m1.g24 = 0.0;

   m1.g31 = pv3.x_gm;
   m1.g32 = pv3.y_gm;
   m1.g33 = pv3.z_gm;
   m1.g34 = 0.0;

   m1.g41 = 0.0;
   m1.g42 = 0.0;
   m1.g43 = 0.0;
   m1.g44 = 1.0;

   m2.g11 = 1.0;
   m2.g12 = 0.0;
   m2.g13 = 0.0;
   m2.g14 = -por->x_gm;

   m2.g21 = 0.0;
   m2.g22 = 1.0;
   m2.g23 = 0.0;
   m2.g24 = -por->y_gm;

   m2.g31 = 0.0;
   m2.g32 = 0.0;
   m2.g33 = 1.0;
   m2.g34 = -por->z_gm;

   m2.g41 = 0.0;
   m2.g42 = 0.0;
   m2.g43 = 0.0;
   m2.g44 = 1.0;
/*
***Create the combined matrix = the output matrix
*/
   GEtform_mult(&m1,&m2,pt);
/*
***End.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

       DBstatus GEmktf_2p(
       DBVector   *p1,
       DBVector   *p2,
       DBVector   *p3,
       DBTmat  *csymat)

/*      Create transformation from 2 or 3 positions.
 *
 *      In: p1 => First point.
 *          p2 => Second point.
 *          p3 => Third point or NULL.
 *
 *      Out: *csymat = Matrix.
 *
 *      (C)microform ab 1996-07-09 J. Kjellander
 *
 *      1999-04-27 Rewritten, J.Kjellander
 *      1999-12-18 sur690->varkon_tra_intlinsur G. Liden
 *
 ******************************************************!*/

  {
    DBVector v1,v2;

/*
***Create X-axis.
*/
    v1.x_gm = p2->x_gm - p1->x_gm;
    v1.y_gm = p2->y_gm - p1->y_gm;
    v1.z_gm = p2->z_gm - p1->z_gm;
/*
***With only 2 positions, use varkon_tra_intlinsur
*/
    if ( p3 == NULL )
      {
      if ( varkon_tra_intlinsur(p1,&v1,csymat) < 0 ) 
           return(erpush("EX2102","varkon_tra_intlinsur"));
      }
/*
***Else, create Y-axis and use GEmktf_3p().
*/
    else
      {
      v2.x_gm = p3->x_gm - p1->x_gm;
      v2.y_gm = p3->y_gm - p1->y_gm;
      v2.z_gm = p3->z_gm - p1->z_gm;
      if ( GEmktf_3p(p1,&v1,&v2,csymat) < 0 )
        return(erpush("EX2102","GEmktf_3p()"));
      }

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

        DBstatus GEmktf_1p(
        DBVector *p,
        DBfloat   v1,
        DBfloat   v2,
        DBfloat   v3,
        DBTmat   *pc,
        DBTmat   *pt)

/*      Creates a basic->local transformation matrix from
 *      one position and 3 angles.
 *
 *      In: p      => Pointer to origin
 *          v1     => Rotation around local X-axis.
 *          v2     => Ditto Y.
 *          v3     => Ditto Z.
 *          pc     => Pointer to active coordinate system.
 *
 *      Out: *pt   => Transformation matrix.
 *
 *      (C)microform ab 30/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
   DBTmat tmat,rmat,tmpmat;

/*
***Create unrotated system in p-BASIC.
*/
   tmat.g11 = 1.0;
   tmat.g12 = 0.0;
   tmat.g13 = 0.0;
   tmat.g14 = -p->x_gm;

   tmat.g21 = 0.0;
   tmat.g22 = 1.0;
   tmat.g23 = 0.0;
   tmat.g24 = -p->y_gm;

   tmat.g31 = 0.0;
   tmat.g32 = 0.0;
   tmat.g33 = 1.0;
   tmat.g34 = -p->z_gm;

   tmat.g41 = 0.0;
   tmat.g42 = 0.0;
   tmat.g43 = 0.0;
   tmat.g44 = 1.0;
/*
***Rotate around X.
*/
   if ( v1 != 0.0 )
     {
     v1 *= DGTORD;
     rmat.g11 = 1.0;
     rmat.g12 = 0.0;
     rmat.g13 = 0.0;
     rmat.g14 = 0.0;

     rmat.g21 = 0.0;
     rmat.g22 = COS(v1);
     rmat.g23 = SIN(v1);
     rmat.g24 = 0.0;

     rmat.g31 = 0.0;
     rmat.g32 = -SIN(v1);
     rmat.g33 =  COS(v1);
     rmat.g34 = 0.0;

     rmat.g41 = 0.0;
     rmat.g42 = 0.0;
     rmat.g43 = 0.0;
     rmat.g44 = 1.0;

     GEtform_mult(&rmat,&tmat,&tmpmat);
     V3MOME(&tmpmat,&tmat,sizeof(DBTmat));
     }
/*
***Rotate around Y.
*/
   if ( v2 != 0.0 )
     {
     v2 *= DGTORD;
     rmat.g11 =  COS(v2);
     rmat.g12 = 0.0;
     rmat.g13 = -SIN(v2);
     rmat.g14 = 0.0;

     rmat.g21 = 0.0;
     rmat.g22 = 1.0;
     rmat.g23 = 0.0;
     rmat.g24 = 0.0;

     rmat.g31 = SIN(v2);
     rmat.g32 = 0.0;
     rmat.g33 = COS(v2);
     rmat.g34 = 0.0;

     rmat.g41 = 0.0;
     rmat.g42 = 0.0;
     rmat.g43 = 0.0;
     rmat.g44 = 1.0;

     GEtform_mult(&rmat,&tmat,&tmpmat);
     V3MOME(&tmpmat,&tmat,sizeof(DBTmat));
     }
/*
***Rotate around Z.
*/
   if ( v3 != 0.0 )
     {
     v3 *= DGTORD;
     rmat.g11 = COS(v3);
     rmat.g12 = SIN(v3);
     rmat.g13 = 0.0;
     rmat.g14 = 0.0;

     rmat.g21 = -SIN(v3);
     rmat.g22 =  COS(v3);
     rmat.g23 = 0.0;
     rmat.g24 = 0.0;

     rmat.g31 = 0.0;
     rmat.g32 = 0.0;
     rmat.g33 = 1.0;
     rmat.g34 = 0.0;

     rmat.g41 = 0.0;
     rmat.g42 = 0.0;
     rmat.g43 = 0.0;
     rmat.g44 = 1.0;

     GEtform_mult(&rmat,&tmat,&tmpmat);
     V3MOME(&tmpmat,&tmat,sizeof(DBTmat));
     }
/*
***Transform to local system.
*/
   if ( pc != NULL )
     {
     GEtform_mult(&tmat,pc,&tmpmat);
     V3MOME(&tmpmat,pt,sizeof(DBTmat));
     }
   else
     {
     V3MOME(&tmat,pt,sizeof(DBTmat));
     }
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEmktf_ts(
        DBVector *p1,
        DBVector *p2,
        DBfloat   sx,
        DBfloat   sy,
        DBfloat   sz,
        DBTmat   *ptm)
        
/*      Creates a translation and scaling matrix.
 *
 *      In: p1       => Start position.
 *          p2       => End position.
 *          sx,sy,sz => Scale factors
 *
 *      Out: *ptm  => Transformation matrix.
 *
 *      (C)microform ab 9/12/92 J. Kjellander
 *
 *      1999-04-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    DBTmat to,fo,sm,tmp;

/*
***If scaling is specified translate to p1, scale and
***and then translate to p2.
*/
    if ( sx != 1.0  || sy != 1.0  || sz != 1.0 )
      {
      to.g11 = 1.0; to.g12 = 0.0; to.g13 = 0.0; to.g14 = -p1->x_gm;
      to.g21 = 0.0; to.g22 = 1.0; to.g23 = 0.0; to.g24 = -p1->y_gm;
      to.g31 = 0.0; to.g32 = 0.0; to.g33 = 1.0; to.g34 = -p1->z_gm;
      to.g41 = 0.0; to.g42 = 0.0; to.g43 = 0.0; to.g44 = 1.0;

      sm.g11 = sx;  sm.g12 = 0.0; sm.g13 = 0.0; sm.g14 = 0.0;
      sm.g21 = 0.0; sm.g22 = sy;  sm.g23 = 0.0; sm.g24 = 0.0;
      sm.g31 = 0.0; sm.g32 = 0.0; sm.g33 = sz;  sm.g34 = 0.0;
      sm.g41 = 0.0; sm.g42 = 0.0; sm.g43 = 0.0; sm.g44 = 1.0;

      fo.g11 = 1.0; fo.g12 = 0.0; fo.g13 = 0.0; fo.g14 = p2->x_gm;
      fo.g21 = 0.0; fo.g22 = 1.0; fo.g23 = 0.0; fo.g24 = p2->y_gm;
      fo.g31 = 0.0; fo.g32 = 0.0; fo.g33 = 1.0; fo.g34 = p2->z_gm;
      fo.g41 = 0.0; fo.g42 = 0.0; fo.g43 = 0.0; fo.g44 = 1.0;

      GEtform_mult(&fo,&sm,&tmp);
      GEtform_mult(&tmp,&to,ptm);
      }
/*
***If all scale factors are = 1 make it fast.
*/
    else
      {
      ptm->g11 = 1.0; ptm->g12 = 0.0; ptm->g13 = 0.0;
      ptm->g14 = p2->x_gm - p1->x_gm;

      ptm->g21 = 0.0; ptm->g22 = 1.0; ptm->g23 = 0.0;
      ptm->g24 = p2->y_gm - p1->y_gm;

      ptm->g31 = 0.0; ptm->g32 = 0.0; ptm->g33 = 1.0;
      ptm->g34 = p2->z_gm - p1->z_gm;

      ptm->g41 = 0.0; ptm->g42 = 0.0; ptm->g43 = 0.0;
      ptm->g44 = 1.0;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEmktf_rs(
        DBVector *p1,
        DBVector *p2,      
        DBfloat   v,
        DBfloat   sx,
        DBfloat   sy,
        DBfloat   sz,
        DBTmat   *ptm)
        
/*      Creates a rotation and scaling matrix.
 *
 *      In: p1,p2    => Rotation axis
 *          v        => Rotation angle
 *          sx,sy,sz => Scale factors
 *
 *      Out: *ptm  => Transformation matrix
 *
 *      (C)microform ab 9/12/92 J. Kjellander
 *
 *      1999-04-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    gmflt  u1,u2,u3;
    DBVector  p0,dp,n;
    DBTmat to,ro,fo,sm,tmp;

/*
***Create and normalize direction vector for rotation axis.
***p1 = local origin and p2-p1 = direction.
*/
    dp.x_gm = p2->x_gm - p1->x_gm;
    dp.y_gm = p2->y_gm - p1->y_gm;
    dp.z_gm = p2->z_gm - p1->z_gm;
    GEnormalise_vector3D(&dp,&n);
    u1 = n.x_gm;
    u2 = n.y_gm;
    u3 = n.z_gm;
/*
***Angle to radians.
*/
    v *= DGTORD;
/*
***Translation to origin, to.
*/
    p0.x_gm = p0.y_gm = p0.z_gm = 0.0;
    GEmktf_ts(p1,&p0,1.0,1.0,1.0,&to);
/*
***Rotation, ro.
*/
    ro.g11 = u1*u1 + COS(v)*(1.0 - u1*u1);
    ro.g12 = u1*u2*(1.0 - COS(v)) - u3*SIN(v);
    ro.g13 = u3*u1*(1.0 - COS(v)) + u2*SIN(v);
    ro.g14 = 0.0;

    ro.g21 = u1*u2*(1.0 - COS(v)) + u3*SIN(v);
    ro.g22 = u2*u2 + COS(v)*(1.0 - u2*u2);
    ro.g23 = u2*u3*(1.0 - COS(v)) - u1*SIN(v);
    ro.g24 =  0.0;

    ro.g31 =  u3*u1*(1.0 - COS(v)) - u2*SIN(v);
    ro.g32 =  u2*u3*(1.0 - COS(v)) + u1*SIN(v);
    ro.g33 =  u3*u3 + COS(v)*(1.0 - u3*u3);
    ro.g34 =  0.0;

    ro.g41 =  0.0;
    ro.g42 =  0.0;
    ro.g43 =  0.0;
    ro.g44 =  1.0;
/*
***Translation back, fo.
*/
    GEmktf_ts(&p0,p1,1.0,1.0,1.0,&fo);
/*
***Optional scaling.
*/
    if ( sx != 1.0  || sy != 1.0  || sz != 1.0 )
      {
      sm.g11 = sx;  sm.g12 = 0.0; sm.g13 = 0.0; sm.g14 = 0.0;
      sm.g21 = 0.0; sm.g22 = sy;  sm.g23 = 0.0; sm.g24 = 0.0;
      sm.g31 = 0.0; sm.g32 = 0.0; sm.g33 = sz;  sm.g34 = 0.0;
      sm.g41 = 0.0; sm.g42 = 0.0; sm.g43 = 0.0; sm.g44 = 1.0;
      GEtform_mult(&sm,&to,&tmp);
      V3MOME(&tmp,&to,sizeof(DBTmat));
      }
/*
***Multiply from_origin * rotate_z * to_origin.
*/
    GEtform_mult(&fo,&ro,&tmp);
    GEtform_mult(&tmp,&to,ptm);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEmktf_mirr(
        DBVector *p1,
        DBVector *p2,
        DBVector *p3,      
        DBTmat   *ptm)
        
/*      Create a mirror matrix.
 *
 *      In: p1-p3 => Mirroring plane
 *
 *      Out: *ptm  => Transformation matri.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      1999-04-27 Rewritten, J.Kjellander
 *
 ******************************************************!*/

  {
    DBVector  v1,v2;
    DBTmat to,mi,fo,tmp;

/*
***Translation to and back from local system. Use
***p1 as origin, p2 as X-axis and p3 for the plane.
*/
    v1.x_gm = p2->x_gm - p1->x_gm;
    v1.y_gm = p2->y_gm - p1->y_gm;
    v1.z_gm = p2->z_gm - p1->z_gm;

    v2.x_gm = p3->x_gm - p1->x_gm;
    v2.y_gm = p3->y_gm - p1->y_gm;
    v2.z_gm = p3->z_gm - p1->z_gm;

    GEmktf_3p(p1,&v1,&v2,&to);
    GEtform_inv(&to,&fo);
/*
***Mirror in local XY-plane, g33 = -1.
*/
    mi.g11 =  1.0; mi.g12 =  0.0; mi.g13 =  0.0; mi.g14 =  0.0;
    mi.g21 =  0.0; mi.g22 =  1.0; mi.g23 =  0.0; mi.g24 =  0.0;
    mi.g31 =  0.0; mi.g32 =  0.0; mi.g33 = -1.0; mi.g34 =  0.0;
    mi.g41 =  0.0; mi.g42 =  0.0; mi.g43 =  0.0; mi.g44 =  1.0;
/*
***Multiply.
*/
    GEtform_mult(&fo,&mi,&tmp);
    GEtform_mult(&tmp,&to,ptm);
/*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus GEmktf_6p(
        DBVector *p1,
        DBVector *p2,
        DBVector *p3,
        DBVector *p4,
        DBVector *p5,
        DBVector *p6,      
        DBTmat   *ptm)
        
/*      Create general transformation by translation
 *      and rotation of a point in one plane to a
 *      point in another plane.
 *
 *      In: p1   => Original position
 *          p2   => Original vector
 *          p3   => A vector in the original plane
 *          p4   => New position
 *          p5   => New vector
 *          p6   => A vector in the new plane
 *
 *      Out: *ptm  => Transformation matrix.
 *
 *      (C)microform ab 1/3/93 J.Kjellander and G.Liden
 *
 *      1999-02-27 Rewritten, J.Kjellander
 *      1999-12-18 Documentation changed G. Liden
 *
 ******************************************************!*/

  {
    DBTmat  mor;
    DBTmat  mor_i;
    DBTmat  mtr;
    DBTmat  out_i;

/*
***Create original csys.
*/
    if ( GEmktf_3p(p1,p2,p3,&mor) != SUCCED )
       return(erpush("GE6262","GEmktf_6p (1)"));
/*
***Same for the new pos.
*/
    if ( GEmktf_3p(p4,p5,p6,&mtr) != SUCCED )
       return(erpush("GE6262","GEmktf_6p (2)"));
/*
***Invert mor.
*/
    GEtform_inv(&mor,&mor_i);
/*
***Multiply and invert the result.
*/
    GEtform_mult(&mor_i,&mtr,&out_i);
    GEtform_inv(&out_i,ptm);

    return(0);
  }

/********************************************************/
