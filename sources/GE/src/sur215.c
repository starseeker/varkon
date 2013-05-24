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
/*  Function: varkon_sur_mbseval                   File: sur215.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a surface.                   */
/*                                                                  */
/*  Output is one (1) vector with the requested derivative.         */
/*  The function corresponds to MBS statement: EVAL (surface pt)    */
/*                                                                  */
/*  Not yet implemented:                                            */
/*  - Handling of curvature cases planar, developable,              */
/*    spherical, i.e. the logical variables in structure            */
/*    variable EVALS (function value of varkon_sur_eval).           */
/*  - Add u_local, v_local, iu, iv in EVALS structure variable      */  
/*                                                                  */ 
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1995-01-28   Spine, limit, tangent, ... data for LFT_SUR        */
/*  1995-12-09   sur210 -> sur209                                   */
/*  1996-01-11   Initialization of xyz                              */
/*  1996-10-24   D3TODUV added                                      */
/*  1996-10-25   icase= for D3TODUV (problem with offset            */
/*               derivatives. icase should not be 9 for all cases!) */
/*               DUVTOD3 added                                      */
/*  1996-11-06   Bug DUVTOD3, OFFSET added                          */
/*  1997-12-07   MBS U,V point to sur209                            */
/*  1998-02-01   UVLOCAL for NURBS surface ... not finished .. !!!  */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_mbseval    Surface MBS evaluation routine   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_sur_patadr      * Patch adress for given U,V pt           */
/* varkon_sur_eval_mbs    * Surface evaluation routine              */
/* varkon_sur_secteval    * Evaluate LFT_SUR input curve            */
/* varkon_ini_evals       * Initialize EVALS                        */
/* varkon_sur_d3toduv     * UV tangent to R*3 tangent               */
/* varkon_angd            * Angle (degrees) betw. vectors           */
/* varkon_sur_uvmap       * Map UV point                            */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2743 = Surface EVAL case is not valid      varkon_sur_mbseval  */
/* SU2943 = Called function xxxx failed         varkon_sur_mbseval  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
/* SU2993 = Severe program error in varkon_sur_mbseval (sur215)     */


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_sur_mbseval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf   *p_sur,      /* Surface                           (ptr) */
   DBPatch  *p_pat,      /* Alloc. area for topol. patch data (ptr) */
   char     *ecase,      /* Evaluation case                         */
   DBfloat   uglob,      /* Global u value                          */
   DBfloat   vglob,      /* Global v value                          */
   DBVector *p_outvec )  /* Output derivative vector          (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!----- Evaluation cases described with an example ----------------*/
/*                                                                  */
/*                       Output vector p_outvec                     */
/*  ecase    1'st component   2'nd component    3'rd component      */
/*  -----    --------------   --------------    --------------      */
/*                                                                  */
/* uvlocal     0.00000000        0.80000000                         */
/* patadr     15.00000000        3.00000000                         */
/*                                                                  */
/* xyz       340.70799475      199.79094154      231.72592175       */
/* drdu       20.14624928       11.77375914       17.47012382       */
/* drdv      -37.70054617       64.31806409      -18.54518397       */
/* d2rdu2      1.22951878       -2.14683277        0.96903519       */
/* d2rdv2    -11.22194044       -8.73248094       -5.52981629       */
/* d2rdudv    -1.80740000        3.69035400       -0.88672075       */
/*                                                                  */
/* normal     -0.60572220       -0.12864565        0.78520756       */
/* dndu       -0.00674424       -0.00447978       -0.00593657       */
/* dndv        0.02021893       -0.04124046        0.00884053       */
/*                                                                  */
/* kappamin    0.00034307                                           */
/* kappamax    0.00061779                                           */
/* gaussian    0.00000021                                           */
/* mean        0.00048043                                           */
/*                                                                  */
/* r3min       0.71328818        0.34951063        0.60750498       */
/* r3max      -0.35259126        0.92805852       -0.11994493       */
/* uvmin       0.99973618       -0.02296874                         */
/* uvmax       0.47432194        0.88035146                         */
/*                                                                  */
/* uvminderiv -0.02297480                                           */
/* uvmaxderiv  1.85602095                                           */
/* angkappa   90.00000000       63.00091649                         */
/*                                                                  */
/* sp_xyz    6070.00000000      0.00000000      2260.00000000       */
/* sp_drdu    627.91290000      0.00000000         0.00000000       */
/* sp_d2rdu2    0.00000000      0.00000000         0.00000000       */
/*                                                                  */
/* ls_xyz    6069.99999998    325.01050000      3814.08930000       */
/* ls_drdt    735.84725263     -8.87535236         3.29114308       */
/* ls_d2rdt2 -195.09344245    -70.61830639       -58.13600994       */
/* ts_xyz    6069.99999998    550.25750000      3794.62340000       */
/* ts_drdt    735.52942172    -15.01988582        13.75495991       */
/* ts_d2rdt2 -193.99046918     -8.61916372       -85.81253822       */
/*                                                                  */
/* m_flag       1.00000000                                          */
/* m_xyz        0.00000000      0.60759610                          */
/* m_drdt     640.47115792     -0.06951360                          */
/* m_d2rdt2    -0.00000035     -0.00340651                          */
/*                                                                  */
/* le_xyz    6070.00000000    550.25750000      3090.13770000       */
/* le_drdt    614.15368045    -12.54133142       143.92075737       */
/* le_d2rdt2   53.71010821    -10.13211576       -70.57432111       */
/*                                                                  */
/* te_xyz    6070.00000000    550.25750000      2060.00000000       */
/* te_drdt    628.16223098    -12.82739328       138.46179300       */
/* te_d2rdt2   25.10619593     -9.95695524        30.24162363       */
/*                                                                  */
/* g_xyz     6069.99999999    506.06359937      3660.22117945       */
/* g_drdt       0.00000002    176.77560253      -568.16286250       */
/* g_d2rdt2     0.00000000   -429.63266668     -1306.59624059       */
/*                                                                  */
/* d3toduv Special (temporary ?) function                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/
{ /* Start of function */

/*!                                                                !*/
/*!                                                                !*/
/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALS   xyz;          /* Coordinates and derivatives for surface */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  u_pat;       /* Patch (local) U parameter value         */
   DBfloat  v_pat;       /* Patch (local) V parameter value         */
   DBint    iu,iv;       /* Patch number (adress)                   */
   DBint    nu,nv;       /* Number of patches in U and V direction  */
   DBint    surtype;     /* Type CUB_SUR, RAT_SUR, LFT_SUR, or ...  */
   DBfloat  v_min[3];    /* Minimum principal curvature vector      */
   DBfloat  v_max[3];    /* Maximum principal curvature vector      */
   DBfloat  r3_ang;      /* Angle in R*3 between curvature vectors  */
   DBfloat  uv_ang;      /* Angle in U,V between curvature vectors  */
   DBint    icase;       /* Derivative flag for varkon_sur_eval     */
   char     e_r3uv[20];  /* String for d3toduv                      */
   DBVector r3tang;      /* R*3 tangent vector                      */
   DBVector uvtang;      /* U,V tangent vector                      */
/* DBfloat  u_map;NotY*/ /* Mapped U parameter value                */
/* DBfloat  v_map;NotY*/ /* Mapped V parameter value                */
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
  "sur215 Enter *** varkon_sur_mbseval: uglob= %f vglob= %f \n",
                      uglob , vglob );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/* Initialize EVALS                                                 */

   status = varkon_ini_evals  (&xyz);

   r3tang.x_gm = F_UNDEF;
   r3tang.y_gm = F_UNDEF;
   r3tang.z_gm = F_UNDEF;
   uvtang.x_gm = F_UNDEF;
   uvtang.y_gm = F_UNDEF;
   uvtang.z_gm = F_UNDEF;
/* Not finished! See below   u_map       = F_UNDEF;   */
/* Not finished! See below   v_map       = F_UNDEF;   */


/*!                                                                 */
/* 2. Surface evaluation                                            */
/* _____________________                                            */
/*                                                                  */
/* Calculate coordinates and derivatives.                           */
/* Call of varkon_sur_eval_gm (sur209).                             */
/*                                                                 !*/

   sscanf( ecase ,"%s", e_r3uv); /* For the d3toduv case */
   if      ( strcmp(e_r3uv,"d3toduv")  == 0  || 
             strcmp(e_r3uv,"D3TODUV")  == 0      )
     {
     icase = 3;
     }
   else if ( strcmp(e_r3uv,"duvtod3")  == 0  || 
             strcmp(e_r3uv,"DUVTOD3")  == 0      )
     {
     icase = 3; 
     }
   else if ( strcmp(ecase,"xyz")  == 0  || 
             strcmp(ecase,"XYZ")  == 0    )
     {
     icase = 1;
     }
   else if ( strcmp(ecase,"drdu") == 0  || 
             strcmp(ecase,"DRDU") == 0    )
     {
     icase = 3;
     }
   else if ( strcmp(ecase,"drdv") == 0  || 
             strcmp(ecase,"DRDV") == 0    )
     {
     icase = 3;
     }
   else if ( strcmp(ecase,"offset")  == 0  || 
             strcmp(ecase,"OFFSET")  == 0    )
     {
     icase = 1; 
     }
   else if ( strcmp(ecase,"normal") == 0  || 
             strcmp(ecase,"NORMAL") == 0    )
     {
     icase = 4; 
     }
   else if ( strcmp(ecase,"uvlocal")   == 0  || 
             strcmp(ecase,"UVLOCAL")   == 0    )
     {
     icase = 1; 
     }
   else if ( strcmp(ecase,"patadr")   == 0  || 
             strcmp(ecase,"PATADR")   == 0    )
     {
     icase = 1; 
     }
   else
     icase = 9; /* Set for case. Offset surface does not work ....   */

status=varkon_sur_eval_gm (p_sur,icase,uglob-1.0,vglob-1.0,&xyz);
if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur215 sur209 failed for uglob %f  vglob %f \n",
   uglob,vglob );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
   sprintf(errbuf,"%f %f%%(varkon_sur_eval)",uglob-1.0,vglob-1.0);
   return(varkon_erpush("SU2843",errbuf));
   }

/*!                                                                 */
/* 3. Output vector                                                 */
/* ________________                                                 */
/*                                                                  */
/* Requested derivative vector to output variable p_outvec.         */
/*                                                                  */
/* When necessary (for ecase) will the above declared functions     */
/* be called (varkon_sur_patadr, varkon_angd, .... ).               */
/*                                                                 !*/


   sscanf( ecase ,"%s", e_r3uv); /* For the d3toduv case */
   if      ( strcmp(e_r3uv,"d3toduv")  == 0  || 
             strcmp(e_r3uv,"D3TODUV")  == 0    )
     {

     sscanf( ecase ,"%s%lf%lf%lf", e_r3uv,
              &r3tang.x_gm,&r3tang.y_gm,&r3tang.z_gm); 

      status = varkon_sur_d3toduv (  &xyz,  &r3tang, p_outvec );
     if (status<0) 
        {
        sprintf(errbuf,"sur925%%varkon_sur_mbseval ");
        return(varkon_erpush("SU2943",errbuf));
        }
     }
   else if ( strcmp(e_r3uv,"duvtod3")  == 0  || 
             strcmp(e_r3uv,"DUVTOD3")  == 0      )
     {
     sscanf( ecase ,"%s%lf%lf", e_r3uv,
              &uvtang.x_gm,&uvtang.y_gm); 

/*   dx/dt = dx/du*du/dt + dx/dv*dv/dt     (1)  */
/*   dy/dt = dy/du*du/dt + dy/dv*dv/dt     (2)  */
/*   dz/dt = dz/dt*du/dt + dz/dv*dv/dt     (3)  */
      p_outvec->x_gm = xyz.u_x*uvtang.x_gm + xyz.v_x*uvtang.y_gm;
      p_outvec->y_gm = xyz.u_y*uvtang.x_gm + xyz.v_y*uvtang.y_gm;
      p_outvec->z_gm = xyz.u_z*uvtang.x_gm + xyz.v_z*uvtang.y_gm;
     }






   else if ( strcmp(ecase,"xyz")  == 0  || 
             strcmp(ecase,"XYZ")  == 0    )
      {
      p_outvec->x_gm = xyz.r_x;
      p_outvec->y_gm = xyz.r_y;
      p_outvec->z_gm = xyz.r_z;
      }
   else if ( strcmp(ecase,"drdu") == 0  || 
             strcmp(ecase,"DRDU") == 0    )
      {
      p_outvec->x_gm = xyz.u_x;
      p_outvec->y_gm = xyz.u_y;
      p_outvec->z_gm = xyz.u_z;
      }
   else if ( strcmp(ecase,"drdv") == 0  || 
             strcmp(ecase,"DRDV") == 0    )
      {
      p_outvec->x_gm = xyz.v_x;
      p_outvec->y_gm = xyz.v_y;
      p_outvec->z_gm = xyz.v_z;
      }
   else if ( strcmp(ecase,"d2rdu2") == 0  || 
             strcmp(ecase,"D2RDU2") == 0    )
      {
      p_outvec->x_gm = xyz.u2_x;
      p_outvec->y_gm = xyz.u2_y;
      p_outvec->z_gm = xyz.u2_z;
      }
   else if ( strcmp(ecase,"d2rdv2") == 0  || 
             strcmp(ecase,"D2RDV2") == 0    )
      {
      p_outvec->x_gm = xyz.v2_x;
      p_outvec->y_gm = xyz.v2_y;
      p_outvec->z_gm = xyz.v2_z;
      }
   else if ( strcmp(ecase,"d2rdudv") == 0  || 
             strcmp(ecase,"D2RDUDV") == 0    )
      {
      p_outvec->x_gm = xyz.uv_x;
      p_outvec->y_gm = xyz.uv_y;
      p_outvec->z_gm = xyz.uv_z;
      }
   else if ( strcmp(ecase,"offset")  == 0  || 
             strcmp(ecase,"OFFSET")  == 0    )
     {
      p_outvec->x_gm = xyz.offset;
      p_outvec->y_gm = 0.0;       
      p_outvec->z_gm = 0.0;       
     }
   else if ( strcmp(ecase,"normal") == 0  || 
             strcmp(ecase,"NORMAL") == 0    )
      {
      p_outvec->x_gm = xyz.n_x;
      p_outvec->y_gm = xyz.n_y;
      p_outvec->z_gm = xyz.n_z;
      }
   else if ( strcmp(ecase,"dndu")   == 0  || 
             strcmp(ecase,"DNDU")   == 0    )
      {
      p_outvec->x_gm = xyz.nu_x;
      p_outvec->y_gm = xyz.nu_y;
      p_outvec->z_gm = xyz.nu_z;
      }
   else if ( strcmp(ecase,"dndv")   == 0  || 
             strcmp(ecase,"DNDV")   == 0    )
      {
      p_outvec->x_gm = xyz.nv_x;
      p_outvec->y_gm = xyz.nv_y;
      p_outvec->z_gm = xyz.nv_z;
      }
   else if ( strcmp(ecase,"r3max")   == 0  || 
             strcmp(ecase,"R3MAX")   == 0    )
      {
      p_outvec->x_gm = xyz.kmax_x;
      p_outvec->y_gm = xyz.kmax_y;
      p_outvec->z_gm = xyz.kmax_z;
      }
   else if ( strcmp(ecase,"r3min")   == 0  || 
             strcmp(ecase,"R3MIN")   == 0    )
      {
      p_outvec->x_gm = xyz.kmin_x;
      p_outvec->y_gm = xyz.kmin_y;
      p_outvec->z_gm = xyz.kmin_z;
      }
   else if ( strcmp(ecase,"uvmax")   == 0  || 
             strcmp(ecase,"UVMAX")   == 0    )
      {
      p_outvec->x_gm = xyz.kmax_u;
      p_outvec->y_gm = xyz.kmax_v;
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"uvmin")   == 0  || 
             strcmp(ecase,"UVMIN")   == 0    )
      {
      p_outvec->x_gm = xyz.kmin_u;
      p_outvec->y_gm = xyz.kmin_v;
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"kappamax")   == 0  || 
             strcmp(ecase,"KAPPAMAX")   == 0    )
      {
      p_outvec->x_gm = xyz.kmax;
      p_outvec->y_gm = 0.0;      
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"kappamin")   == 0  || 
             strcmp(ecase,"KAPPAMIN")   == 0    )
      {
      p_outvec->x_gm = xyz.kmin;
      p_outvec->y_gm = 0.0;      
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"gaussian")   == 0  || 
             strcmp(ecase,"GAUSSIAN")   == 0    )
      {
      p_outvec->x_gm = xyz.kmin*xyz.kmax;
      p_outvec->y_gm = 0.0;      
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"mean")   == 0  || 
             strcmp(ecase,"MEAN")   == 0    )
      {
      p_outvec->x_gm = (xyz.kmin+xyz.kmax)/2.0;
      p_outvec->y_gm = 0.0;      
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"uvminderiv")   == 0  || 
             strcmp(ecase,"UVMINDERIV")   == 0    )
      {
      if (fabs(xyz.kmin_u) > 0.00000001 )
         {
         p_outvec->x_gm = xyz.kmin_v/xyz.kmin_u;
         }
      else
         {
         p_outvec->x_gm = 50000.0;
         }
      p_outvec->y_gm = 0.0;        
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"uvmaxderiv")   == 0  || 
             strcmp(ecase,"UVMAXDERIV")   == 0    )
      {
      if (fabs(xyz.kmax_u) > 0.00000001 )
         {
         p_outvec->x_gm = xyz.kmax_v/xyz.kmax_u;
         }
      else
         {
         p_outvec->x_gm = 50000.0;
         }
      p_outvec->y_gm = 0.0;        
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"angkappa")   == 0  || 
             strcmp(ecase,"ANGKAPPA")   == 0    )
      {
      v_min[0]= xyz.kmin_x;
      v_min[1]= xyz.kmin_y;
      v_min[2]= xyz.kmin_z;
      v_max[0]= xyz.kmax_x;
      v_max[1]= xyz.kmax_y;
      v_max[2]= xyz.kmax_z;
      status=varkon_angd (v_min , v_max, &r3_ang );
     if (status<0) 
        {
        sprintf(errbuf,"angd%%varkon_sur_mbseval ");
        return(varkon_erpush("SU2943",errbuf));
        }
      v_min[0]= xyz.kmin_u;
      v_min[1]= xyz.kmin_v;
      v_min[2]= 0.0;       
      v_max[0]= xyz.kmax_u;
      v_max[1]= xyz.kmax_v;
      v_max[2]= 0.0;       
      status=varkon_angd (v_min , v_max, &uv_ang );
     if (status<0) 
        {
        sprintf(errbuf,"angd%%varkon_sur_mbseval ");
        return(varkon_erpush("SU2943",errbuf));
        }
      p_outvec->x_gm = r3_ang;  
      p_outvec->y_gm = uv_ang;   
      p_outvec->z_gm = 0.0;        
      }



   else if ( strcmp(ecase,"uvlocal")   == 0  || 
             strcmp(ecase,"UVLOCAL")   == 0    )
      {

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mbseval (sur215)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

   status=varkon_sur_patadr
   (uglob,vglob,nu,nv,&iu,&iv,&u_pat,&v_pat);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_patadr%%varkon_sur_mbseval sur215 ");
        return(varkon_erpush("SU2943",errbuf));
        }
      p_outvec->x_gm = u_pat ;  
      p_outvec->y_gm = v_pat ;  
      p_outvec->z_gm = 0.0;        
  
#ifdef  NURBS_NOT_FINISHED
/*    Map UV (for a NURBS surface)     1998-02-01  */
/*    p_pat must be first patch, or .... NURBS only one patch   */
      
      status=varkon_sur_uvmap
    (p_sur,p_pat,uglob-1.0, vglob-1.0, &u_map, &v_map);
    if (status<0) 
      {
      sprintf(errbuf,"sur360%%sur215");
      return(erpush("SU2943",errbuf));
        }
      if ( fabs(uglob-u_map) > 0.00000001 ||
           fabs(vglob-v_map) > 0.00000001    )
        {
        /* NURBS surface */
         p_outvec->x_gm = u_map ;  
         p_outvec->y_gm = v_map ;  
         p_outvec->z_gm = 0.0;        
#ifdef DEBUG
         if ( dbglev(SURPAC) == 1 )
           {
           fprintf(dbgfil(SURPAC),
           "sur215 UVLOCAL u_map= %f v_map= %f NURBS surface\n",
                               u_map , v_map );
           fflush(dbgfil(SURPAC));
           }
#endif
        }
#endif  /*  NURBS_NOT_FINISHED  */

      }

   else if ( strcmp(ecase,"patadr")   == 0  || 
             strcmp(ecase,"PATADR")   == 0    )
      {

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mbseval (sur215)");
    return(erpush("SU2943",errbuf));
    }
#endif

   status=varkon_sur_patadr
   (uglob,vglob,nu,nv,&iu,&iv,&u_pat,&v_pat);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_patadr%%varkon_sur_mbseval sur215 ");
/* No return for error 1999-12-05 ??? */
        return(varkon_erpush("SU2943",errbuf));
        }
      p_outvec->x_gm = (DBfloat)iu+0.000000000001;
      p_outvec->y_gm = (DBfloat)iv+0.000000000001;
      p_outvec->z_gm = 0.0;        
      }

   else if ( strcmp(ecase,"sp_xyz") == 0  || 
             strcmp(ecase,"SP_XYZ") == 0    )
      {


    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mbseval (sur215)");
    return(erpush("SU2943",errbuf));
    }
#endif

      if ( surtype == LFT_SUR )
          {
          ; /* OK */
          }
      else
          {
          sprintf(errbuf,"SP_XYZ%%");
          varkon_erinit();
          return(varkon_erpush("SU2443",errbuf));
          }

      p_outvec->x_gm = xyz.sp_x;
      p_outvec->y_gm = xyz.sp_y;
      p_outvec->z_gm = xyz.sp_z;
      }

   else if ( strcmp(ecase,"sp_drdu") == 0  || 
             strcmp(ecase,"SP_DRDU") == 0    )
      {

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mbseval (sur215)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

      if ( surtype == LFT_SUR )
          {
          ; /* OK */
          }
      else
          {
          sprintf(errbuf,"SP_DRDU%%");
          varkon_erinit();
          return(varkon_erpush("SU2443",errbuf));
          }

      p_outvec->x_gm = xyz.spt_x;
      p_outvec->y_gm = xyz.spt_y;
      p_outvec->z_gm = xyz.spt_z;
      }
   else if ( strcmp(ecase,"sp_d2rdu2") == 0  || 
             strcmp(ecase,"SP_D2RDU2") == 0    )
      {


    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mbseval (sur215)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

      if ( surtype == LFT_SUR )
          {
          ; /* OK */
          }
      else
          {
          sprintf(errbuf,"SP_D2RDU2%%");
          varkon_erinit();
          return(varkon_erpush("SU2443",errbuf));
          }

      p_outvec->x_gm = xyz.spt2_x;
      p_outvec->y_gm = xyz.spt2_y;
      p_outvec->z_gm = xyz.spt2_z;
      }

   else if ( strcmp(ecase,"ls_xyz")     == 0  || 
             strcmp(ecase,"LS_XYZ")     == 0  ||
             strcmp(ecase,"ls_drdt")    == 0  ||
             strcmp(ecase,"LS_DRDT")    == 0  ||
             strcmp(ecase,"ls_d2rdt2")  == 0  ||
             strcmp(ecase,"LS_D2RDT2")  == 0  ||
             strcmp(ecase,"ts_xyz")     == 0  || 
             strcmp(ecase,"TS_XYZ")     == 0  ||
             strcmp(ecase,"ts_drdt")    == 0  ||
             strcmp(ecase,"TS_DRDT")    == 0  ||
             strcmp(ecase,"ts_d2rdt2")  == 0  ||
             strcmp(ecase,"TS_D2RDT2")  == 0  ||
             strcmp(ecase,"m_xyz")      == 0  || 
             strcmp(ecase,"M_XYZ")      == 0  ||
             strcmp(ecase,"m_drdt")     == 0  ||
             strcmp(ecase,"M_DRDT")     == 0  ||
             strcmp(ecase,"m_d2rdt2")   == 0  ||
             strcmp(ecase,"M_D2RDT2")   == 0  ||
             strcmp(ecase,"m_flag")     == 0  ||
             strcmp(ecase,"M_FLAG")     == 0  ||
             strcmp(ecase,"le_xyz")     == 0  || 
             strcmp(ecase,"LE_XYZ")     == 0  ||
             strcmp(ecase,"le_drdt")    == 0  ||
             strcmp(ecase,"LE_DRDT")    == 0  ||
             strcmp(ecase,"le_d2rdt2")  == 0  ||
             strcmp(ecase,"LE_D2RDT2")  == 0  ||
             strcmp(ecase,"te_xyz")     == 0  || 
             strcmp(ecase,"TE_XYZ")     == 0  ||
             strcmp(ecase,"te_drdt")    == 0  ||
             strcmp(ecase,"TE_DRDT")    == 0  ||
             strcmp(ecase,"te_d2rdt2")  == 0  ||
             strcmp(ecase,"TE_D2RDT2")  == 0  ||  
             strcmp(ecase,"g_xyz")      == 0  || 
             strcmp(ecase,"G_XYZ")      == 0  ||
             strcmp(ecase,"g_drdt")     == 0  ||
             strcmp(ecase,"G_DRDT")     == 0  ||
             strcmp(ecase,"g_d2rdt2")   == 0  ||
             strcmp(ecase,"G_D2RDT2")   == 0    )
      {

    status = varkon_sur_secteval
   (p_sur,p_pat,uglob,vglob, ecase, p_outvec );
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_secteval%%varkon_sur_mbseval");
       return(varkon_erpush("SU2943",errbuf));
        }

     } /* End LFT_SUR input curve evaluation  */


    else
      {
      sprintf(errbuf," %s %% (varkon_sur_mbseval)",ecase);
       varkon_erinit();
       return(varkon_erpush("SU2743",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur215 Exit*varkon_sur_mbseval ecase %s outvec %f %f %f\n",
    ecase, p_outvec->x_gm,p_outvec->y_gm,p_outvec->z_gm );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

