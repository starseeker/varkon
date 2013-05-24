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
/*  Function: varkon_sur_eval_gm                   File: sur209.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a surface.                   */
/*  Data for the current patch is retrieved from GM in this         */
/*  function.                                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-12-09   Originally written                                 */
/*  1996-05-15   BOX_SUR added                                      */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1997-12-05   NURBS, J.Kjellander                                */
/*  1997-12-10   Deallocate NURBS for sur220 failure                */
/*               Input U,V changed to the MBS U,V point             */
/*  1998-01-06   Debug                                              */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_eval_gm    Surface (GM) evaluation routine  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals       * Initiate EVALS                          */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_sur_patadr      * Patch adress for given U,V pt           */
/* varkon_pat_uvmap       * Map U,V point                           */
/* DBread_one_patch       * Retrieve GM patch data                  */
/* DBread_patches         * Reads all patches from GM               */
/* DBfree_patches         * Free NURBS patch memory                 */
/* varkon_surpat_eval     * Surface patch evaluation                */
/* varkon_pat_evalbox     * BOX_SUR patch evaluation                */
/* varkon_pat_pritop      * Print topological patch data            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2481 = Conic lofting patch iu,iv in surface is not OK          */
/* SU2983 = sur209 Illegal computation case= for varkon_sur_eval_gm */
/* SU2953 = Patch iu,iv=  outside surface nu,nv=                    */
/* SU2943 = Called function xxxxxx failed in varkon_sur_eval_gm     */
/* SU2993 = Severe program error in varkon_sur_eval_gm (sur209).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus varkon_sur_eval_gm (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat  u_mbs,       /* Global u value                          */
   DBfloat  v_mbs,       /* Global v value                          */

   EVALS   *p_xyz )      /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_loc;        /* Patch (local) U parameter value         */
   DBfloat v_loc;        /* Patch (local) V parameter value         */
   DBfloat u_pat;        /* Patch (local) U parameter value, mapped */
   DBfloat v_pat;        /* Patch (local) V parameter value, mapped */
   DBint   iu,iv;        /* Patch number (adress)                   */
   DBint   nu,nv;        /* Number of patches in U and V direction  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBPatch toppat;       /* Topological patch for DBread_one_patch  */
   DBPatch *p_toppat;    /* Pointer to topological patch            */
   DBPatch *p_toppat_uv; /* Pointer to topological patch for sur361 */
   GMPATX  geopat;       /* Geometrical patch for DBread_one_patch  */
   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR, or ...  */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */
#ifdef DEBUG
   DBint   aiu;          /* Adress to patch U. Eq. -1: All patches  */
   DBint   aiv;          /* Adress to patch V.                      */
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
  "sur209 Enter *** varkon_sur_eval_gm: u_mbs= %f v_mbs= %f \n",
                      u_mbs , v_mbs );
  }
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Initiate variabel EVALS                                          */
/* Call of varkon_ini_evals (sur170).                               */
/*                                                                 !*/

      varkon_ini_evals (p_xyz);

/*!                                                                 */
/* Input uglobal, vglobal, icase to p_xyz.                          */
/* Check computation case.               Error SU2983 if not OK.    */
/*                                                                 !*/

/* TODO         Move   !!!!!!!!!!!!!!!!!!                           */
   p_xyz->u      = u_mbs+1.0;     /* U parameter value              */
   p_xyz->v      = v_mbs+1.0;     /* V parameter value              */
   p_xyz->e_case = icase;         /* Evaluation case                */
   p_xyz->s_anal = FALSE;         /* No surface analysis            */


   if (icase > 9 ) 
       {
       sprintf(errbuf,"%d%% varkon_sur_eval_gm (sur209)",(int)icase);
       return(varkon_erpush("SU2983",errbuf));
       }

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_eval_gm (sur209)");
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
   (u_mbs+1.0,v_mbs+1.0,nu,nv,&iu,&iv,&u_loc,&v_loc);
   if (status<0) 
        {
        sprintf(errbuf,"sur211%%sur209");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* Retrieve patch data from GM. Call of DBread_one_patch.           */
/*                                                                 !*/

/*
***NURBS-ytor består bara av en enda patch och kan 
***inte hämtas partiellt. Övriga ytor räcker att
***hämta patchen ifråga.
*/
/* A NURBS surface consists of one surface. It is not possible      */
/* to get data only for a part of the NURBS surface.                */
/* TODO Add possibility to read only part of the NURBS data         */
/* It is for all other surfaces only necessary to retrieve one      */
/* patch.                                                           */

      p_toppat_uv = NULL;

      switch ( p_sur->typ_su )
        {
        case NURB_SUR:
        DBread_patches(p_sur,&p_toppat);
/*      Patch with UV definition area                               */
/*      Temporary solution ! Refer to sur220                        */
        p_toppat_uv = p_toppat + (iu-1)*nv + iv - 1;
        break;

        default:
        toppat.spek_c = (char *)&geopat;
        if ( (status=
             DBread_one_patch(p_sur,&toppat,(short)iu,(short)iv)) < 0 )
                       return(status);
        p_toppat = &toppat;
        break;
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur209 nu %d nv %d iu %d iv %d \n",
   (int)nu,(int)nv,(int)iu,(int)iv );
  fprintf(dbgfil(SURPAC),
  "sur209 u_pat %f  v_pat %f u_loc %f v_loc %f\n",
   u_pat,v_pat, u_loc, v_loc );
  }
#endif

/*!                                                                 */
/* Map the U,V point. Call of varkon_pat_uvmap (sur361).            */
/*                                                                 !*/

    status = varkon_pat_uvmap
    (p_sur,p_toppat_uv,u_loc, v_loc,  &u_pat, &v_pat);
    if(status<0)
    {
    sprintf(errbuf,"su361%%sur209");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur209 nu %d nv %d iu %d iv %d After sur361\n",
   (int)nu,(int)nv,(int)iu,(int)iv );
  fprintf(dbgfil(SURPAC),
  "sur209 u_pat %f  v_pat %f u_loc %f v_loc %f\n",
   u_pat,v_pat, u_loc, v_loc );
  }
#endif

/*!                                                                 */
/* For Debug On:     Printout of topological patch data.            */
/*                   Call of varkon_pat_pritop  (sur232).           */
/*                                                                 !*/


#ifdef DEBUG_NO_NOT_MORE
  aiu = 1; 
  aiv = 1; 
    status = varkon_pat_pritop (p_sur,p_toppat,aiu,aiv);
    if(status<0)
    {
    sprintf(errbuf,"su232%%sur904");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* Evaluation in special function if surface is of type BOX_SUR.    */
/* Call of varkon_pat_evalbox (sur213) in this case.                */
/*                                                                 !*/

/* Temporary graphic and surdef.h   if  (  surtype  ==  BOX_SUR )  */


   if  (  surtype  ==  FAC_SUR )
     {
     status = varkon_pat_evalbox
     (p_sur,p_toppat,icase,u_pat,v_pat,p_xyz);
     if(status<0)
       {
       sprintf(errbuf,"varkon_pat_evalbox %%varkon_sur_eval_gm");
       return(varkon_erpush("SU2943",errbuf));
       }
     goto  boxsur;
     }



/*!                                                                 */
/* 3. Surface patch evaluation                                      */
/* ___________________________                                      */
/*                                                                  */
/* Calculate coordinates and derivatives in patch toppat            */
/* for local parameter point (u_pat,v_pat).                         */
/* Call of varkon_pat_eval (sur220).                                */
/*                                                                 !*/


status=varkon_pat_eval (p_sur,p_toppat,icase,u_pat,v_pat,p_xyz);
if (status<0) 
   {
/*  Deallocate NURBS                                                */
    if ( p_sur->typ_su == NURB_SUR ) DBfree_patches(p_sur,p_toppat);
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur209 sur220 failed for u_pat %f  v_pat %f \n",
   u_pat,v_pat );
  fprintf(dbgfil(SURPAC),
  "sur209 nu %d nv %d iu %d iv %d \n",
   (int)nu,(int)nv,(int)iu,(int)iv );
  }
 fflush(dbgfil(SURPAC)); 
#endif
   sprintf(errbuf,"%f %f%%(varkon_sur_eval_gm)",u_mbs-1.0,v_mbs-1.0);
   return(varkon_erpush("SU2843",errbuf));
   }

boxsur:; /* Label: Surface is BOX_SUR */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur209 Exit*varkon_sur_eval_gm x= %f y= %f z= %f\n",
                      p_xyz->r_x,p_xyz->r_y,p_xyz->r_z );
  }
 fflush(dbgfil(SURPAC)); 
#endif

/* Free memory for a NURBS surface                                  */
   if ( p_sur->typ_su == NURB_SUR ) DBfree_patches(p_sur,p_toppat);

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

