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
/*  Function: varkon_sur_secteval                  File: sur236.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates a point, tangent or second derivative   */
/*  on one of the input curves to a LFT_SUR surface for a given     */
/*  parameter ( u,v ) point on the surface.                         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-01-28   Originally written                                 */
/*  1996-05-28   Declaration of erinit                              */
/*  1997-03-08   Temporary solution for rotation LFT_SUR            */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_secteval   Evaluate LFT_SUR input curve     */
/*                                                              */
/*------------------------------------------------------------- */

/*!----- Evaluation cases described with an example ----------------*/
/*                                                                  */
/*                       Output vector p_outvec                     */
/*  ecase    1'st component   2'nd component    3'rd component      */
/*  -----    --------------   --------------    --------------      */
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
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals       * Initiate EVALS                          */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_sur_patadr      * Patch adress for given U,V pt           */
/* varkon_pat_pritop      * Print topological patch data            */
/* varkon_pat_loftcur     * Evaluate LFT_SUR curves                 */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2481 = Conic lofting patch iu,iv in surface is not OK          */
/* SU2943 = Called function xxxxxx failed in varkon_sur_secteval    */
/* SU2993 = Severe program error in varkon_sur_secteval (sur236).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus  varkon_sur_secteval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf   *p_sur,      /* Surface                           (ptr) */
   DBPatch  *p_pat,      /* Alloc. area for topol. patch data (ptr) */
   DBfloat   uglob,      /* Global u value                          */
   DBfloat   vglob,      /* Global v value                          */
   char     *ecase,      /* Evaluation case                   (ptr) */
   DBVector *p_outvec )  /* Output vector                     (ptr) */

/* Out:                                                             */
/*       Data to ...                                                */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_pat;       /* Patch (local) U parameter value         */
   DBfloat  v_pat;       /* Patch (local) V parameter value         */
   DBint    iu,iv;       /* Patch number (adress)                   */
   DBint    nu,nv;       /* Number of patches in U and V direction  */
   DBPatch *p_patcur;    /* Current patch                     (ptr) */
   GMPATL  *p_patl;      /* Conic lofting patch               (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


   DBVector s_r;         /* Spine   start point                     */
   DBVector s_drdt;      /* Spine   start tangent                   */
   DBVector s_d2rdt2;    /* Spine   start second derivative         */
   DBVector ls_r;        /* Limit   start point                     */
   DBVector ls_drdt;     /* Limit   start tangent                   */
   DBVector ls_d2rdt2;   /* Limit   start second derivative         */
   DBVector ts_r;        /* Tangent start point                     */
   DBVector ts_drdt;     /* Tangent start tangent                   */
   DBVector ts_d2rdt2;   /* Tangent start second derivative         */
   DBVector m_r;         /* Mid     start point                     */
   DBVector m_drdt;      /* Mid     start tangent                   */
   DBVector m_d2rdt2;    /* Mid     start second derivative         */
   DBint    m_f;         /* Mid flag  1: P value  2: Mid crv        */
   DBVector le_r;        /* Limit   end   point                     */
   DBVector le_drdt;     /* Limit   end   tangent                   */
   DBVector le_d2rdt2;   /* Limit   end   second derivative         */
   DBVector te_r;        /* Tangent end   point                     */
   DBVector te_drdt;     /* Tangent end   tangent                   */
   DBVector te_d2rdt2;   /* Tangent end   second derivative         */
   DBVector g_r;         /* Gener.  start point                     */
   DBVector g_drdt;      /* Gener.  start tangent                   */
   DBVector g_d2rdt2;    /* Gener.  start second derivative         */




   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR, or ...  */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */
#ifdef DEBUG
   DBint  aiu;           /* Adress to patch U. Eq. -1: All patches  */
   DBint  aiv;           /* Adress to patch V.                      */
#endif

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
  "sur236 Enter *** varkon_sur_secteval: uglob= %f vglob= %f \n",
                      uglob , vglob );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_secteval (sur236)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* 2. Current patch and local parameter point.                      */
/* __________________________________________                       */
/*                                                                  */
/* Patch address iu,iv and patch parameter pt u_pat,v_pat.          */
/* Call of varkon_sur_patadr (sur211).                              */
/*                                                                 !*/

   status=varkon_sur_patadr
   (uglob,vglob,nu,nv,&iu,&iv,&u_pat,&v_pat);
   if (status<0) 
      {
      sprintf(errbuf,"varkon_sur_patadr%%varkon_sur_secteval (sur236)");
      return(varkon_erpush("SU2943",errbuf));
      }

/*!                                                                 */
/* Determine pointer to the current topological patch:              */
/* p_patcur= p_pat + (iu-1)*nv + (iv-1)                             */
/*                                                                 !*/

   p_patcur=  p_pat +(iu-1)*nv+(iv-1);

/*!                                                                 */
/* For Debug On:     Printout of topological patch data.            */
/*                   Call of varkon_pat_pritop  (sur232).           */
/*                                                                 !*/


#ifdef DEBUG
  aiu = iu;
  aiv = iv;
    status = varkon_pat_pritop (p_sur,p_pat,aiu,aiv);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pritop %%varkon_sur_secteval (sur102)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

if ( p_pat->styp_pat == LFT_PAT )
    {
    p_patl= (GMPATL *)p_patcur->spek_c;
    if ( p_patl->ofs_pat != 0.0 ) 
      {
      sprintf(errbuf,"Offset not allowed%%varkon_sur_secteval");
      return(varkon_erpush("SU2993",errbuf));
      }
    }  /* End LFT_PAT */

else
    {
    sprintf(errbuf,"Invalid patch type%%varkon_sur_secteval");
    return(varkon_erpush("SU2993",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur236 nu %d nv %d iu %d iv %d \n"
                                      , nu,nv,iu,iv );
  fprintf(dbgfil(SURPAC), "sur236 p_pat %d  p_patcur %d p_patl %d \n", 
                                  p_pat,   p_patcur,    p_patl);
  fprintf(dbgfil(SURPAC), "sur236 u_pat %f  v_pat %f \n", u_pat,v_pat );
  }
#endif

/*!                                                                 */
/* 3. Evaluate coordinates and derivatives for all LFT_SUR curves   */
/* ______________________________________________________________   */
/*                                                                  */
/* Call of varkon_sur_loftcur (sur235).                             */
/*                                                                 !*/

   status = varkon_pat_loftcur 
   (p_patl, u_pat, v_pat ,
    &s_r  ,  &s_drdt  ,  &s_d2rdt2  ,
    &ls_r ,  &ls_drdt ,  &ls_d2rdt2 ,
    &ts_r ,  &ts_drdt ,  &ts_d2rdt2 ,
    &m_r  ,  &m_drdt  ,  &m_d2rdt2  ,  &m_f,
    &le_r ,  &le_drdt ,  &le_d2rdt2 ,
    &te_r ,  &te_drdt ,  &te_d2rdt2 ,
    &g_r  ,  &g_drdt  ,  &g_d2rdt2  );
    if( status < 0 )
      if ( strcmp(ecase,"m_flag")     == 0  ||
             strcmp(ecase,"M_FLAG")     == 0    )
      {
      ; /* Continue, temporary solution for rotation LFT_SUR */
      }
      else
    {
    sprintf(errbuf,"varkon_sur_loftcur %%varkon_sur_secteval (sur235)");
    return(varkon_erpush("SU2943",errbuf));
    }

   if      ( strcmp(ecase,"sp_xyz")  == 0  || 
             strcmp(ecase,"SP_XYZ")  == 0    )
      {
      p_outvec->x_gm = s_r.x_gm;  
      p_outvec->y_gm = s_r.y_gm;  
      p_outvec->z_gm = s_r.z_gm;  
      }

   else if ( strcmp(ecase,"sp_drdt")    == 0  ||
             strcmp(ecase,"SP_DRDT")    == 0    )
      {
      p_outvec->x_gm = s_drdt.x_gm;  
      p_outvec->y_gm = s_drdt.y_gm;  
      p_outvec->z_gm = s_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"sp_d2rdt2")  == 0  ||
             strcmp(ecase,"SP_D2RDT2")  == 0    )
      {
      p_outvec->x_gm = s_d2rdt2.x_gm;  
      p_outvec->y_gm = s_d2rdt2.y_gm;  
      p_outvec->z_gm = s_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"ls_xyz")  == 0  || 
             strcmp(ecase,"LS_XYZ")  == 0    )
      {
      p_outvec->x_gm = ls_r.x_gm;  
      p_outvec->y_gm = ls_r.y_gm;  
      p_outvec->z_gm = ls_r.z_gm;  
      }

   else if ( strcmp(ecase,"ls_drdt")    == 0  ||
             strcmp(ecase,"LS_DRDT")    == 0    )
      {
      p_outvec->x_gm = ls_drdt.x_gm;  
      p_outvec->y_gm = ls_drdt.y_gm;  
      p_outvec->z_gm = ls_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"ls_d2rdt2")  == 0  ||
             strcmp(ecase,"LS_D2RDT2")  == 0    )
      {
      p_outvec->x_gm = ls_d2rdt2.x_gm;  
      p_outvec->y_gm = ls_d2rdt2.y_gm;  
      p_outvec->z_gm = ls_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"ts_xyz")     == 0  || 
             strcmp(ecase,"TS_XYZ")     == 0    )
      {
      p_outvec->x_gm = ts_r.x_gm;  
      p_outvec->y_gm = ts_r.y_gm;  
      p_outvec->z_gm = ts_r.z_gm;  
      }

   else if ( strcmp(ecase,"ts_drdt")    == 0  ||
             strcmp(ecase,"TS_DRDT")    == 0    )
      {
      p_outvec->x_gm = ts_drdt.x_gm;  
      p_outvec->y_gm = ts_drdt.y_gm;  
      p_outvec->z_gm = ts_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"ts_d2rdt2")  == 0  ||
             strcmp(ecase,"TS_D2RDT2")  == 0    )
      {
      p_outvec->x_gm = ts_d2rdt2.x_gm;  
      p_outvec->y_gm = ts_d2rdt2.y_gm;  
      p_outvec->z_gm = ts_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"m_xyz")      == 0  || 
             strcmp(ecase,"M_XYZ")      == 0    )
      {
      p_outvec->x_gm = m_r.x_gm;  
      p_outvec->y_gm = m_r.y_gm;  
      p_outvec->z_gm = m_r.z_gm;  
      }

   else if ( strcmp(ecase,"m_drdt")     == 0  ||
             strcmp(ecase,"M_DRDT")     == 0    )
      {
      p_outvec->x_gm = m_drdt.x_gm;  
      p_outvec->y_gm = m_drdt.y_gm;  
      p_outvec->z_gm = m_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"m_d2rdt2")   == 0  ||
             strcmp(ecase,"M_D2RDT2")   == 0    )
      {
      p_outvec->x_gm = m_d2rdt2.x_gm;  
      p_outvec->y_gm = m_d2rdt2.y_gm;  
      p_outvec->z_gm = m_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"m_flag")     == 0  ||
             strcmp(ecase,"M_FLAG")     == 0    )
      {
      p_outvec->x_gm = (DBfloat)m_f; 
      p_outvec->y_gm = 0.0;         
      p_outvec->z_gm = 0.0;         
      }

   else if ( strcmp(ecase,"le_xyz")     == 0  || 
             strcmp(ecase,"LE_XYZ")     == 0    )
      {
      p_outvec->x_gm = le_r.x_gm;  
      p_outvec->y_gm = le_r.y_gm;  
      p_outvec->z_gm = le_r.z_gm;  
      }

   else if ( strcmp(ecase,"le_drdt")    == 0  ||
             strcmp(ecase,"LE_DRDT")    == 0    )
      {
      p_outvec->x_gm = le_drdt.x_gm;  
      p_outvec->y_gm = le_drdt.y_gm;  
      p_outvec->z_gm = le_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"le_d2rdt2")  == 0  ||
             strcmp(ecase,"LE_D2RDT2")  == 0    )
      {
      p_outvec->x_gm = le_d2rdt2.x_gm;  
      p_outvec->y_gm = le_d2rdt2.y_gm;  
      p_outvec->z_gm = le_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"te_xyz")     == 0  || 
             strcmp(ecase,"TE_XYZ")     == 0    )
      {
      p_outvec->x_gm = te_r.x_gm;  
      p_outvec->y_gm = te_r.y_gm;  
      p_outvec->z_gm = te_r.z_gm;  
      }

   else if ( strcmp(ecase,"te_drdt")    == 0  ||
             strcmp(ecase,"TE_DRDT")    == 0    )
      {
      p_outvec->x_gm = te_drdt.x_gm;  
      p_outvec->y_gm = te_drdt.y_gm;  
      p_outvec->z_gm = te_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"te_d2rdt2")  == 0  ||
             strcmp(ecase,"TE_D2RDT2")  == 0     )
      {
      p_outvec->x_gm = te_d2rdt2.x_gm;  
      p_outvec->y_gm = te_d2rdt2.y_gm;  
      p_outvec->z_gm = te_d2rdt2.z_gm;  
      }

   else if ( strcmp(ecase,"g_xyz")      == 0  || 
             strcmp(ecase,"G_XYZ")      == 0    )
      {
      p_outvec->x_gm = g_r.x_gm;  
      p_outvec->y_gm = g_r.y_gm;  
      p_outvec->z_gm = g_r.z_gm;  
      }

   else if ( strcmp(ecase,"g_drdt")     == 0  ||
             strcmp(ecase,"G_DRDT")     == 0    )
      {
      p_outvec->x_gm = g_drdt.x_gm;  
      p_outvec->y_gm = g_drdt.y_gm;  
      p_outvec->z_gm = g_drdt.z_gm;  
      }

   else if ( strcmp(ecase,"g_d2rdt2")   == 0  ||
             strcmp(ecase,"G_D2RDT2")   == 0    )
      {
      p_outvec->x_gm = g_d2rdt2.x_gm;  
      p_outvec->y_gm = g_d2rdt2.y_gm;  
      p_outvec->z_gm = g_d2rdt2.z_gm;  
      }

    else
      {
      sprintf(errbuf," %s %% (varkon_sur_secteval)",ecase);
       varkon_erinit();
       return(varkon_erpush("SU2743",errbuf));
      }





#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur236 Exit*varkon_sur_secteval \n" );
 fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

