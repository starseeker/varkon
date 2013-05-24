/**********************************************************************
*
*    gepca.c
*    =======
*
*    This file includes the following public functions:
*
*    GEmktf_pca() Creates a PCA transformation 
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
*    ViewsOnCad 2008 Gunnar Liden, ViewsOnCad.ch
*
***********************************************************************/

/*
***Include files.
*/

#include "../../DB/include/DB.h"
#include "../include/GE.h"


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short pca_scale();   /* Scale to avoid big covariance values */
static short pca_mean();    /* Calculate mean (cog) value           */
static short pca_covar();   /* Calculate the covariance matrix      */
static short pca_eigen();   /* Calculate eigen values and vectors   */
static short pca_sweep();   /* One sweep for the Jacobi method      */
static short pca_matrix();  /* One element for the Jacobi method    */
static short pca_order();   /* Order the eigen values and vectors   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/********************************************************/

        DBstatus GEmktf_pca(
        DBVector *ppts,
        DBint     npts,
        DBVector *pevals,
        DBTmat   *pt)

/*      Creates a Principal Components Analysis (PCA) 
 *      transformation matrix from a set of points
 *
 *      In:  ppts   = Pointer to set of points
 *           npts   = Number of points
 *
 *      Out: pevals = Pointer to eigen values
 *           pt     = Pointer to transformation matrix
 *
 *      2008-01-05 ViewsOnCad Gunnar Liden
 *
 *
 ******************************************************!*/

 {
   DBVector mean;
   DBfloat  scale;
   DBTmat   m_covar;
   DBfloat  val_1_eigen;
   DBfloat  val_2_eigen;
   DBfloat  val_3_eigen;
   DBVector vec_1_eigen;
   DBVector vec_2_eigen;
   DBVector vec_3_eigen;
   DBfloat  val_x_eigen;
   DBfloat  val_y_eigen;
   DBfloat  val_z_eigen;
   DBVector vec_x_eigen;
   DBVector vec_y_eigen;
   DBVector vec_z_eigen;
   DBVector cog;

   if (npts < 4) 
     return(erpush("GE6262","GEmktf_pca Number of points < 4"));

   if (pca_scale(ppts, npts, &scale) != SUCCED)
      return(erpush("GE6262","GEmktf_pca pca_scale failed"));

   if (pca_mean(ppts, npts, scale, &mean) != SUCCED)
      return(erpush("GE6262","GEmktf_pca pca_mean failed"));

   if (pca_covar(ppts, npts, scale, mean, &m_covar) != SUCCED)
      return(erpush("GE6262","GEmktf_pca pca_covar failed"));

   if (pca_eigen(m_covar, &vec_1_eigen, &vec_2_eigen, &vec_3_eigen, &val_1_eigen, &val_2_eigen, &val_3_eigen) != SUCCED)
      return(erpush("GE6262","GEmktf_pca pca_eigen failed"));

   if (pca_order( val_1_eigen, val_2_eigen, val_3_eigen,
                  vec_1_eigen, vec_2_eigen, vec_3_eigen,
                 &val_x_eigen,&val_y_eigen,&val_z_eigen,
                 &vec_x_eigen,&vec_y_eigen,&vec_z_eigen ) != SUCCED)
      return(erpush("GE6262","GEmktf_pca pca_order failed"));

   cog.x_gm = mean.x_gm/scale;
   cog.y_gm = mean.y_gm/scale;
   cog.z_gm = mean.z_gm/scale;

   if ( GEmktf_3p(&cog,&vec_x_eigen,&vec_y_eigen,pt) < 0 )
       return(erpush("GE6262","GEmktf_pca & GEmktf_3p failed"));

   pevals->x_gm = val_x_eigen;
   pevals->y_gm = val_y_eigen;
   pevals->z_gm = val_z_eigen;

   return(0);
 }

/********************************************************/

/*!********* Internal ** function ** pca_scale **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Determines scale factor in order to avoid big covariance numbers */


   static short pca_scale( 
        DBVector *ppts,
        DBint     npts,
        DBfloat  *p_scale)

/*                                                                  */
/******************************************************************!*/
{ 
   DBint i_pt;
   DBVector* p_point;
   DBfloat max_value;
   DBfloat   x_c;
   DBfloat   y_c;
   DBfloat   z_c;
   DBint     i_scale;
   DBfloat   min_scale;
   DBfloat   max_scale;
 
   *p_scale = -0.123456789;

   if (npts < 4) 
     return(erpush("GE6262","pca_scale Number of points < 4"));

   max_value = -5000000000.0;

   for (i_pt=0; i_pt<npts; i_pt++)
       {
       p_point = ppts + i_pt;

       x_c = p_point->x_gm;
       y_c = p_point->y_gm;
       z_c = p_point->z_gm;

       if (fabs(x_c) > max_value)
           {
           max_value = fabs(x_c);
           }
       if (fabs(y_c) > max_value)
           {
           max_value = fabs(y_c);
           }
       if (fabs(z_c) > max_value)
           {
           max_value = fabs(z_c);
           }
       }

   *p_scale = 1.0;

    min_scale = 1.0;
    for (i_scale=0; i_scale<10; i_scale++)
        {
        min_scale = min_scale*10.0;
        max_scale = min_scale*10.0;
        if (max_value > min_scale && max_value <=  max_scale)
            {
            *p_scale = 1.0/min_scale;
             break;
            }
        } 

  return(SUCCED);

}
/********************************************************************/

/*!********* Internal ** function ** pca_mean ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate the mean value that also is the center of gravity      */


   static short pca_mean( 
        DBVector *ppts,
        DBint     npts,
        DBfloat   scale,
        DBVector *p_mean)

/*                                                                  */
/******************************************************************!*/
{ 
   DBint i_pt;
   DBVector* p_point;
   DBVector  mean;

   if (npts < 4) 
     return(erpush("GE6262","pca_mean Number of points < 4"));

   if (scale < 0.00000001)
     return(erpush("GE6262","pca_mean scale < 0.00000001"));

   mean.x_gm = 0.0;
   mean.y_gm = 0.0;
   mean.z_gm = 0.0;

   for (i_pt=0; i_pt<npts; i_pt++)
       {
       p_point = ppts + i_pt;

       mean.x_gm = mean.x_gm + scale*p_point->x_gm;
       mean.y_gm = mean.y_gm + scale*p_point->y_gm;
       mean.z_gm = mean.z_gm + scale*p_point->z_gm;

       }

   mean.x_gm = mean.x_gm/npts;
   mean.y_gm = mean.y_gm/npts;
   mean.z_gm = mean.z_gm/npts;

   *p_mean = mean;

  return(SUCCED);

}
/********************************************************************/

/*!********* Internal ** function ** pca_covar **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate the covariance matrix                                  */


   static short pca_covar( 
        DBVector *ppts,
        DBint     npts,
        DBfloat   scale,
        DBVector  mean,
        DBTmat   *p_m_covar)

/*                                                                  */
/******************************************************************!*/
{ 
   DBint i_pt;
   DBVector* p_point;
   DBfloat   xx_covar;
   DBfloat   xy_covar;
   DBfloat   xz_covar;
   DBfloat   yy_covar;
   DBfloat   yz_covar;
   DBfloat   zz_covar;
   DBfloat   x_c;
   DBfloat   y_c;
   DBfloat   z_c;
   DBTmat    m_covar;

   if (npts < 4) 
     return(erpush("GE6262","pca_covar Number of points < 4"));

   if (scale < 0.00000001)
     return(erpush("GE6262","pca_covar scale < 0.00000001"));

   xx_covar = 0.0;
   xy_covar = 0.0;
   xz_covar = 0.0;
   yy_covar = 0.0;
   yz_covar = 0.0;
   zz_covar = 0.0;

   for (i_pt=0; i_pt<npts; i_pt++)
       {
       p_point = ppts + i_pt;

       x_c = scale*p_point->x_gm;
       y_c = scale*p_point->y_gm;
       z_c = scale*p_point->z_gm;

       xx_covar = xx_covar + (x_c-mean.x_gm)*(x_c-mean.x_gm);
       xy_covar = xy_covar + (x_c-mean.y_gm)*(y_c-mean.y_gm);
       xz_covar = xz_covar + (x_c-mean.x_gm)*(z_c-mean.z_gm);
       yy_covar = yy_covar + (y_c-mean.y_gm)*(y_c-mean.y_gm);
       yz_covar = yz_covar + (y_c-mean.y_gm)*(z_c-mean.z_gm);
       zz_covar = zz_covar + (z_c-mean.z_gm)*(z_c-mean.z_gm);

       }

   xx_covar = xx_covar/(npts-1);
   xy_covar = xy_covar/(npts-1);
   xz_covar = xz_covar/(npts-1);
   yy_covar = yy_covar/(npts-1);
   yz_covar = yz_covar/(npts-1);
   zz_covar = zz_covar/(npts-1);

   m_covar.g11 = xx_covar;
   m_covar.g12 = xy_covar;
   m_covar.g13 = xz_covar;
   m_covar.g14 =      0.0;

   m_covar.g21 = xy_covar;
   m_covar.g22 = yy_covar;
   m_covar.g23 = yz_covar;
   m_covar.g24 =      0.0;

   m_covar.g31 = xz_covar;
   m_covar.g32 = yz_covar;
   m_covar.g33 = zz_covar;
   m_covar.g34 =      0.0;

   m_covar.g41 =      0.0;
   m_covar.g42 =      0.0;
   m_covar.g43 =      0.0;
   m_covar.g44 =      1.0;

   *p_m_covar = m_covar;

  return(SUCCED);

}
/********************************************************************/

/*!********* Internal ** function ** pca_eigen **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate eigen values and eigen vectors with the Jaobi method   */


   static short pca_eigen( 
        DBTmat   m_covar,
	DBVector *p_vec_1_eigen,
	DBVector *p_vec_2_eigen,
	DBVector *p_vec_3_eigen,
        DBfloat  *p_val_1_eigen,
        DBfloat  *p_val_2_eigen,
        DBfloat  *p_val_3_eigen)

/*                                                                  */
/******************************************************************!*/
{ 
   DBTmat Q_mat_in_out;
   DBTmat PSI_mat_in_out;
   int i_iter;
   const int n_iter_max = 10;
   const double null_crit = 0.000001;

   p_vec_1_eigen->x_gm = -0.123456789;
   p_vec_1_eigen->y_gm = -0.123456789;
   p_vec_1_eigen->z_gm = -0.123456789;
   p_vec_2_eigen->x_gm = -0.123456789;
   p_vec_2_eigen->y_gm = -0.123456789;
   p_vec_2_eigen->z_gm = -0.123456789;
   p_vec_3_eigen->x_gm = -0.123456789;
   p_vec_3_eigen->y_gm = -0.123456789;
   p_vec_3_eigen->z_gm = -0.123456789;

   *p_val_1_eigen = -0.123456789;
   *p_val_2_eigen = -0.123456789;
   *p_val_3_eigen = -0.123456789;

   Q_mat_in_out = m_covar;

   PSI_mat_in_out.g11 = 1.0;
   PSI_mat_in_out.g12 = 0.0;
   PSI_mat_in_out.g13 = 0.0;
   PSI_mat_in_out.g14 = 0.0;

   PSI_mat_in_out.g21 = 0.0;
   PSI_mat_in_out.g22 = 1.0;
   PSI_mat_in_out.g23 = 0.0;
   PSI_mat_in_out.g24 = 0.0;

   PSI_mat_in_out.g31 = 0.0;
   PSI_mat_in_out.g32 = 0.0;
   PSI_mat_in_out.g33 = 1.0;
   PSI_mat_in_out.g34 = 0.0;

   PSI_mat_in_out.g41 = 0.0;
   PSI_mat_in_out.g42 = 0.0;
   PSI_mat_in_out.g43 = 0.0;
   PSI_mat_in_out.g44 = 1.0;

   for (i_iter=0; i_iter<n_iter_max; i_iter++)
   {
      if (pca_sweep(&Q_mat_in_out, &PSI_mat_in_out) != SUCCED)
         return(erpush("GE6262","pca_eigen pca_eigen_sweep failed"));

      if (fabs(Q_mat_in_out.g12) < null_crit && fabs(Q_mat_in_out.g13) < null_crit && fabs(Q_mat_in_out.g23) < null_crit )
      {
         break;
      }
   }

   *p_val_1_eigen = Q_mat_in_out.g11;
   *p_val_2_eigen = Q_mat_in_out.g22;
   *p_val_3_eigen = Q_mat_in_out.g33;
   p_vec_1_eigen->x_gm = PSI_mat_in_out.g11;
   p_vec_1_eigen->y_gm = PSI_mat_in_out.g21;
   p_vec_1_eigen->z_gm = PSI_mat_in_out.g31;
   p_vec_2_eigen->x_gm = PSI_mat_in_out.g12;
   p_vec_2_eigen->y_gm = PSI_mat_in_out.g22;
   p_vec_2_eigen->z_gm = PSI_mat_in_out.g32;
   p_vec_3_eigen->x_gm = PSI_mat_in_out.g13;
   p_vec_3_eigen->y_gm = PSI_mat_in_out.g23;
   p_vec_3_eigen->z_gm = PSI_mat_in_out.g33;

  return(SUCCED);

}
/********************************************************************/

/*!********* Internal ** function ** pca_sweep **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* One sweep for the Jacobi method                                  */


   static short pca_sweep( 
   DBTmat* p_Q_mat_in_out,
   DBTmat* p_PSI_mat_in_out)

/*                                                                  */
/******************************************************************!*/
{ 

   DBTmat Q_mat_in;
   DBTmat PSI_mat_in;
   int index_p;
   int index_q;
   DBTmat Q_mat_out;
   DBTmat PSI_mat_out;

   Q_mat_in = *p_Q_mat_in_out;
   PSI_mat_in = *p_PSI_mat_in_out;

   index_p = 1;
   index_q = 2;
   if (pca_matrix(&Q_mat_in, &PSI_mat_in, index_p, index_q, &Q_mat_out, &PSI_mat_out) != SUCCED)
      return(erpush("GE6262","pca_eigen_sweep pca_matrix failed"));

   Q_mat_in = Q_mat_out;
   PSI_mat_in = PSI_mat_out;

   index_p = 1;
   index_q = 3;
   if (pca_matrix(&Q_mat_in, &PSI_mat_in, index_p, index_q, &Q_mat_out, &PSI_mat_out) != SUCCED)
      return(erpush("GE6262","pca_eigen_sweep pca_matrix failed"));

   Q_mat_in = Q_mat_out;
   PSI_mat_in = PSI_mat_out;

   index_p = 2;
   index_q = 3;
   if (pca_matrix(&Q_mat_in, &PSI_mat_in, index_p, index_q, &Q_mat_out, &PSI_mat_out) != SUCCED)
      return(erpush("GE6262","pca_eigen_sweep pca_matrix failed"));

   *p_Q_mat_in_out = Q_mat_out;
   *p_PSI_mat_in_out = PSI_mat_out;

  return(SUCCED);
}
/********************************************************************/

/*!********* Internal ** function ** pca_matrix *********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* One element for the Jacobi method                                  */

   static short pca_matrix( 
   DBTmat* p_Q_mat_in,
   DBTmat* p_PSI_mat_in,
   int index_p,
   int index_q,
   DBTmat* p_Q_mat_out,
   DBTmat* p_PSI_mat_out)

/*                                                                  */
/******************************************************************!*/
{ 
   double Q_pq;
   double Q_pp;
   double Q_qq;
   const double element_equal = 0.000001;
   double jacobi_rot;
   double tang_value;
   DBTmat jacobi_mat;
   DBTmat jacobi_t_mat;
   DBTmat temp_mat;

   Q_pq = -0.123456789;
   Q_pp = -0.123456789;
   Q_qq = -0.123456789;
   jacobi_rot = -0.123456789;
   tang_value = -0.123456789;

   if (index_p == 1 && index_q == 2)
   {
      Q_pq = p_Q_mat_in->g12;
      Q_pp = p_Q_mat_in->g11;
      Q_qq = p_Q_mat_in->g22;
   }
   else if (index_p == 1 && index_q == 3)
   {
      Q_pq = p_Q_mat_in->g13;
      Q_pp = p_Q_mat_in->g11;
      Q_qq = p_Q_mat_in->g33;
   }
   else if (index_p == 2 && index_q == 3)
   {
      Q_pq = p_Q_mat_in->g23;
      Q_pp = p_Q_mat_in->g22;
      Q_qq = p_Q_mat_in->g33;
   }
   else
   {
      return(erpush("GE6262","pca_matrix Indices not OK"));
   }

   if (fabs(Q_qq - Q_pp) > element_equal)
   {
      tang_value = -2.0*Q_pq/(Q_qq - Q_pp);
      jacobi_rot = 0.5*atan(tang_value);
   }
   else
   {
      jacobi_rot = 0.5*PI;
   }

   jacobi_mat.g11 = 1.0;
   jacobi_mat.g12 = 0.0;
   jacobi_mat.g13 = 0.0;
   jacobi_mat.g14 = 0.0;

   jacobi_mat.g21 = 0.0;
   jacobi_mat.g22 = 1.0;
   jacobi_mat.g23 = 0.0;
   jacobi_mat.g24 = 0.0;

   jacobi_mat.g31 = 0.0;
   jacobi_mat.g32 = 0.0;
   jacobi_mat.g33 = 1.0;
   jacobi_mat.g34 = 0.0;

   jacobi_mat.g41 = 0.0;
   jacobi_mat.g42 = 0.0;
   jacobi_mat.g43 = 0.0;
   jacobi_mat.g44 = 1.0;

   if (index_p == 1 && index_q == 2)
   {
      jacobi_mat.g11 =  cos(jacobi_rot);
      jacobi_mat.g12 = -sin(jacobi_rot);
      jacobi_mat.g21 =  sin(jacobi_rot);
      jacobi_mat.g22 =  cos(jacobi_rot);
   }
   else if (index_p == 1 && index_q == 3)
   {
      jacobi_mat.g11 =  cos(jacobi_rot);
      jacobi_mat.g13 = -sin(jacobi_rot);
      jacobi_mat.g31 =  sin(jacobi_rot);
      jacobi_mat.g33 =  cos(jacobi_rot);
   }
   else if (index_p == 2 && index_q == 3)
   {
      jacobi_mat.g22 =  cos(jacobi_rot);
      jacobi_mat.g23 = -sin(jacobi_rot);
      jacobi_mat.g32 =  sin(jacobi_rot);
      jacobi_mat.g33 =  cos(jacobi_rot);
   }
   else
   {
      return(erpush("GE6262","pca_matrix Indices not OK"));
   }

   jacobi_t_mat.g11 = jacobi_mat.g11;
   jacobi_t_mat.g12 = jacobi_mat.g21;
   jacobi_t_mat.g13 = jacobi_mat.g31;
   jacobi_t_mat.g14 = jacobi_mat.g41;

   jacobi_t_mat.g21 = jacobi_mat.g12;
   jacobi_t_mat.g22 = jacobi_mat.g22;
   jacobi_t_mat.g23 = jacobi_mat.g32;
   jacobi_t_mat.g24 = jacobi_mat.g11;

   jacobi_t_mat.g31 = jacobi_mat.g13;
   jacobi_t_mat.g32 = jacobi_mat.g23;
   jacobi_t_mat.g33 = jacobi_mat.g33;
   jacobi_t_mat.g34 = jacobi_mat.g43;

   jacobi_t_mat.g41 = 0.0;
   jacobi_t_mat.g42 = 0.0;
   jacobi_t_mat.g43 = 0.0;
   jacobi_t_mat.g44 = 1.0;

   GEtform_mult(&jacobi_t_mat,p_Q_mat_in,&temp_mat);
   GEtform_mult(&temp_mat,&jacobi_mat,p_Q_mat_out);
   GEtform_mult(p_PSI_mat_in,&jacobi_mat,p_PSI_mat_out);

  return(SUCCED);
}
/********************************************************************/


/*!********* Internal ** function ** pca_order **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Order the eigen values and vectors                               */

   static short pca_order( 
        DBfloat  val_1_eigen,
        DBfloat  val_2_eigen,
        DBfloat  val_3_eigen,
        DBVector vec_1_eigen,
        DBVector vec_2_eigen,
        DBVector vec_3_eigen,
        DBfloat  *p_val_x_eigen,
        DBfloat  *p_val_y_eigen,
        DBfloat  *p_val_z_eigen,
        DBVector *p_vec_x_eigen,
        DBVector *p_vec_y_eigen,
        DBVector *p_vec_z_eigen)

/*                                                                  */
/******************************************************************!*/
{ 

   if (fabs(val_1_eigen) >= fabs(val_2_eigen) && fabs(val_1_eigen) >= fabs(val_3_eigen))
       {

       *p_val_x_eigen = val_1_eigen;
       *p_vec_x_eigen = vec_1_eigen;

       if ( fabs(val_2_eigen) >= fabs(val_3_eigen))
           {
           *p_val_y_eigen = val_2_eigen;
           *p_vec_y_eigen = vec_2_eigen;
           *p_val_z_eigen = val_3_eigen;
           *p_vec_z_eigen = vec_3_eigen;
           }
        else 
           {
           *p_val_y_eigen = val_3_eigen;
           *p_vec_y_eigen = vec_3_eigen;
           *p_val_z_eigen = val_2_eigen;
           *p_vec_z_eigen = vec_2_eigen;
           }
       }
   else if (fabs(val_2_eigen) >= fabs(val_1_eigen) && fabs(val_2_eigen) >= fabs(val_3_eigen))
       {

       *p_val_x_eigen = val_2_eigen;
       *p_vec_x_eigen = vec_2_eigen;

       if ( fabs(val_1_eigen) >= fabs(val_3_eigen))
           {
           *p_val_y_eigen = val_1_eigen;
           *p_vec_y_eigen = vec_1_eigen;
           *p_val_z_eigen = val_3_eigen;
           *p_vec_z_eigen = vec_3_eigen;
           }
        else 
           {
           *p_val_y_eigen = val_3_eigen;
           *p_vec_y_eigen = vec_3_eigen;
           *p_val_z_eigen = val_1_eigen;
           *p_vec_z_eigen = vec_1_eigen;
           }
       }
   else if (fabs(val_3_eigen) >= fabs(val_2_eigen) && fabs(val_3_eigen) >= fabs(val_1_eigen))
       {

       *p_val_x_eigen = val_3_eigen;
       *p_vec_x_eigen = vec_3_eigen;

       if ( fabs(val_1_eigen) >= fabs(val_2_eigen))
           {
           *p_val_y_eigen = val_1_eigen;
           *p_vec_y_eigen = vec_1_eigen;
           *p_val_z_eigen = val_2_eigen;
           *p_vec_z_eigen = vec_2_eigen;
           }
        else 
           {
           *p_val_y_eigen = val_2_eigen;
           *p_vec_y_eigen = vec_2_eigen;
           *p_val_z_eigen = val_1_eigen;
           *p_vec_z_eigen = vec_1_eigen;
           }
       }
   else
       {
       return(erpush("GE6262","pca_sort Sorting failed"));
       }
 
  return(SUCCED);
}
/********************************************************************/



