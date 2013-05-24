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
/*  Function: varkon_evals_transf                  File: sur640.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function transforms structure variable EVALS with the       */
/*  input transformation matrix.                                    */
/*                                                                  */
/* TODO: Not certain how much icase taht is implemented.            */
/*       I may transform F_UNDEF data !                             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-09-16   Originally written                                 */
/*  1998-05-04   Check of e_case only for Debug On                  */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_evals_transf   Transformation of EVALS          */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals          * Initiate EVALS                       */
/* GEtfpos_to_local          * Transformate a point                 */
/* GEtfpos_to_basic          * Transformate a vector                */
/* varkon_erpush             * Error message to terminal            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_evals_transf  (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_xyz_in,     /* Surface coordinates & derivatives (ptr) */
   DBTmat *p_transf,     /* Transformation matrix for EVALS   (ptr) */
   EVALS  *p_xyz_out )   /* Surface coordinates & derivatives (ptr) */
/* Out:                                                             */
/*       Data to p_xyz_out                                          */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_evals_transf        */
/* SU2993 = Severe program error ( )  in varkon_evals_transf         */
/*                                                                  */
/*-----------------------------------------------------------------!*/



{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector poi_in;       /* Point  from p_xyz_in                    */
  DBVector poi_tra;      /* Transformated point                     */
  DBVector vec_in;       /* Vector from p_xyz_in                    */
  DBVector vec_tra;      /* Transformated vector                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Initialize all data in p_xyz_out for Debug On                    */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
      varkon_ini_evals (p_xyz_out);
#endif

/*!                                                                 */
/* Copy all "non_transformation" data to p_xyz_out                  */
/*                                                                 !*/

/*  Maximum and minimum principal curvature                          */
    p_xyz_out->kmax= p_xyz_in->kmax;
    p_xyz_out->kmax= p_xyz_in->kmin;

/*  Direction in u,v for kmax                                       */
    p_xyz_out->kmax_u = p_xyz_in->kmax_u;
    p_xyz_out->kmax_v = p_xyz_in->kmax_v;

/*  Direction in u,v for kmin                                       */
    p_xyz_out->kmin_u = p_xyz_in->kmin_u;
    p_xyz_out->kmin_v = p_xyz_in->kmin_v;

/* Parameter value on surface                                       */
    p_xyz_out->u = p_xyz_in->u;
    p_xyz_out->v = p_xyz_in->v;

/* Offset value for   surface                                       */
    p_xyz_out->offset = p_xyz_in->offset;

/* Evaluation case                                                  */
    p_xyz_out->e_case = p_xyz_in->e_case; 
 
/* Flag indicating if analysis is made                              */
    p_xyz_out->s_anal = p_xyz_in->s_anal;        


/* Analysis flags                                                   */
    p_xyz_out->cubpat = p_xyz_in->cubpat;        
    p_xyz_out->ratpat = p_xyz_in->ratpat;        
    p_xyz_out->lftpat = p_xyz_in->lftpat;        
    p_xyz_out->offpat = p_xyz_in->offpat;        
    p_xyz_out->planar = p_xyz_in->planar;        
    p_xyz_out->sphere = p_xyz_in->sphere;        
    p_xyz_out->devel  = p_xyz_in->devel;        
    p_xyz_out->saddle = p_xyz_in->saddle;        

/*!                                                                 */
/* Transformate coordinates. Call of GEtfpos_to_local.              */
/*                                                                  */
/*                                                                 !*/

#ifdef  DEBUG 
    if ( p_xyz_out->e_case < 0 )
      {
      sprintf(errbuf,"e_case%%varkon_evals_transf");
      return(varkon_erpush("SU2993",errbuf));
      }
#endif


/*  Coordinates          r(u)                                       */
    poi_in.x_gm = p_xyz_in->r_x;
    poi_in.y_gm = p_xyz_in->r_y;
    poi_in.z_gm = p_xyz_in->r_z;

    status = GEtfpos_to_local (&poi_in, p_transf, &poi_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfpos_to_local%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->r_x = poi_tra.x_gm;
    p_xyz_out->r_y = poi_tra.y_gm;
    p_xyz_out->r_z = poi_tra.z_gm;

    if ( p_xyz_out->lftpat == FALSE )
      {
      goto tangents;
      }

/* Coordinates       for spine                                      */
    poi_in.x_gm = p_xyz_in->sp_x;
    poi_in.y_gm = p_xyz_in->sp_y;
    poi_in.z_gm = p_xyz_in->sp_z;

    status = GEtfpos_to_local (&poi_in, p_transf, &poi_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfpos_to_local sp%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->sp_x = poi_tra.x_gm;
    p_xyz_out->sp_y = poi_tra.y_gm;
    p_xyz_out->sp_z = poi_tra.z_gm;


/* Tangent           for spine                                      */

    vec_in.x_gm = p_xyz_in->spt_x;
    vec_in.y_gm = p_xyz_in->spt_y;
    vec_in.z_gm = p_xyz_in->spt_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local spt%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->spt_x = vec_tra.x_gm;
    p_xyz_out->spt_y = vec_tra.y_gm;
    p_xyz_out->spt_z = vec_tra.z_gm;


/* Second derivative for spine                                      */

    vec_in.x_gm = p_xyz_in->spt2_x;
    vec_in.y_gm = p_xyz_in->spt2_y;
    vec_in.z_gm = p_xyz_in->spt2_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local spt2%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->spt2_x = vec_tra.x_gm;
    p_xyz_out->spt2_y = vec_tra.y_gm;
    p_xyz_out->spt2_z = vec_tra.z_gm;


tangents:;

    if ( p_xyz_out->e_case == 0 )
      {
      goto nomore;
      }

/*!                                                                 */
/* Transformate vectors.     Call of GEtfvec_to_local.              */
/*                                                                  */
/*                                                                 !*/

/*  Tangent             dr/du                                       */

    vec_in.x_gm = p_xyz_in->u_x;
    vec_in.y_gm = p_xyz_in->u_y;
    vec_in.z_gm = p_xyz_in->u_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->u_x = vec_tra.x_gm;
    p_xyz_out->u_y = vec_tra.y_gm;
    p_xyz_out->u_z = vec_tra.z_gm;





    if ( p_xyz_out->e_case == 1 )
      {
      goto nomore;
      }

/*  Tangent             dr/dv                                       */

    vec_in.x_gm = p_xyz_in->v_x;
    vec_in.y_gm = p_xyz_in->v_y;
    vec_in.z_gm = p_xyz_in->v_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local drdv%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->v_x = vec_tra.x_gm;
    p_xyz_out->v_y = vec_tra.y_gm;
    p_xyz_out->v_z = vec_tra.z_gm;

    if ( p_xyz_out->e_case == 2 )
      {
      goto nomore;
      }

/*  Second derivative  d2r/du2                                      */

    vec_in.x_gm = p_xyz_in->u2_x;
    vec_in.y_gm = p_xyz_in->u2_y;
    vec_in.z_gm = p_xyz_in->u2_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local d2rdu2%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->u2_x = vec_tra.x_gm;
    p_xyz_out->u2_y = vec_tra.y_gm;
    p_xyz_out->u2_z = vec_tra.z_gm;


/*  Second derivative  d2r/dv2                                      */

    vec_in.x_gm = p_xyz_in->v2_x;
    vec_in.y_gm = p_xyz_in->v2_y;
    vec_in.z_gm = p_xyz_in->v2_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local d2rdv2%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->v2_x = vec_tra.x_gm;
    p_xyz_out->v2_y = vec_tra.y_gm;
    p_xyz_out->v2_z = vec_tra.z_gm;


/*  Twist vector       d2r/dudv                                     */

    vec_in.x_gm = p_xyz_in->uv_x;
    vec_in.y_gm = p_xyz_in->uv_y;
    vec_in.z_gm = p_xyz_in->uv_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"GEtfvec_to_local d2rdudv%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->uv_x = vec_tra.x_gm;
    p_xyz_out->uv_y = vec_tra.y_gm;
    p_xyz_out->uv_z = vec_tra.z_gm;


/*  Surface normal       n(u,v)                                     */

    vec_in.x_gm = p_xyz_in->n_x;
    vec_in.y_gm = p_xyz_in->n_y;
    vec_in.z_gm = p_xyz_in->n_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"GEtfvec_to_local normal%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->n_x = vec_tra.x_gm;
    p_xyz_out->n_y = vec_tra.y_gm;
    p_xyz_out->n_z = vec_tra.z_gm;

/*  Normal derivative   dn/du                                       */

    vec_in.x_gm = p_xyz_in->nu_x;
    vec_in.y_gm = p_xyz_in->nu_y;
    vec_in.z_gm = p_xyz_in->nu_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local dndu%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->nu_x = vec_tra.x_gm;
    p_xyz_out->nu_y = vec_tra.y_gm;
    p_xyz_out->nu_z = vec_tra.z_gm;

/*  Normal derivative   dn/dv                                       */

    vec_in.x_gm = p_xyz_in->nv_x;
    vec_in.y_gm = p_xyz_in->nv_y;
    vec_in.z_gm = p_xyz_in->nv_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"GEtfvec_to_local dndv%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->nv_x = vec_tra.x_gm;
    p_xyz_out->nv_y = vec_tra.y_gm;
    p_xyz_out->nv_z = vec_tra.z_gm;


/*  Direction in R*3 for kmax                                        */

    vec_in.x_gm = p_xyz_in->kmax_x;
    vec_in.y_gm = p_xyz_in->kmax_y;
    vec_in.z_gm = p_xyz_in->kmax_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->kmax_x = vec_tra.x_gm;
    p_xyz_out->kmax_y = vec_tra.y_gm;
    p_xyz_out->kmax_z = vec_tra.z_gm;


/*  Direction in R*3 for kmin                                        */

    vec_in.x_gm = p_xyz_in->kmin_x;
    vec_in.y_gm = p_xyz_in->kmin_y;
    vec_in.z_gm = p_xyz_in->kmin_z;

    status = GEtfvec_to_local (&vec_in, p_transf, &vec_tra);
if (status<0) 
  {
  sprintf(errbuf,"varkon_GEtfvec_to_local%%varkon_evals_transf");
  return(varkon_erpush("SU2943",errbuf));
  }

    p_xyz_out->kmin_x = vec_tra.x_gm;
    p_xyz_out->kmin_y = vec_tra.y_gm;
    p_xyz_out->kmin_z = vec_tra.z_gm;


nomore:;    /* Label: No more transformations */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
