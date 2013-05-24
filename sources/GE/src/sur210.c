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
/*  Function: varkon_sur_eval                      File: sur210.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a surface.                   */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1994-11-20   Error/warning for a bad patch                      */
/*  1994-11-22   Error/warning for a bad patch (bug correction)     */
/*  1994-12-10   Error/warning for a bad patch to sur220 (not here) */
/*  1996-02-01   Debug                                              */
/*  1997-11-02   Global parameters to sur220 for NURB_SUR           */
/*  1997-11-29   Local  parameters to sur220 for NURB_SUR           */
/*  1997-12-07   Global parameters to sur220 for NURB_SUR           */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_eval       Surface evaluation routine       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_ini_evals       * Initiate EVALS                */
/*           varkon_sur_nopatch     * Retrieve number of patches    */
/*           varkon_sur_patadr      * Patch adress for given U,V pt */
/*           varkon_pat_eval        * Surface patch evaluation      */
/*           varkon_pat_pritop      * Print topological patch data  */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2481 = Conic lofting patch iu,iv in surface is not OK          */
/* SU2983 = sur210 Illegal computation case=   for varkon_sur_eval  */
/* SU2953 = Patch iu,iv=  outside surface nu,nv=                    */
/* SU2943 = Called function xxxxxx failed in varkon_sur_eval        */
/* SU2993 = Severe program error in varkon_sur_eval (sur210).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_sur_eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat  uglob,       /* Global u value                          */
   DBfloat  vglob,       /* Global v value                          */

   EVALS   *p_xyz )      /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   DBint   iu,iv;        /* Patch number (adress)                   */
   DBint   nu,nv;        /* Number of patches in U and V direction  */
   DBPatch *p_patcur;    /* Pointer to the current patch            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR, or ...  */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */
#ifdef DEBUG
   DBint  aiu;           /* Adress to patch U. Eq. -1: All patches  */
   DBint  aiv;           /* Adress to patch V.                      */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

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

   p_xyz->p_sur  = p_sur;         /* Surface                        */
   p_xyz->p_pat  = p_pat;         /* Patches                        */
   p_xyz->u      = uglob;         /* U parameter value              */
   p_xyz->v      = vglob;         /* V parameter value              */
   p_xyz->e_case = icase;         /* Evaluation case                */
   p_xyz->s_anal = FALSE;         /* No surface analysis            */


   if (icase > 9 ) 
       {
       sprintf(errbuf,"%d%% varkon_sur_eval (sur210)",(int)icase);
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
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_eval (sur210)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* Goto nurbs for NURB_SUR                                          */
/*   ..  One patch for NURB_SUR and local between 0 and 1           */
/*   ..  Address and checks will work as for the other surfaces ... */
/*                                                                 !*/

  if  ( surtype == NURB_SUR )
  {
      p_patcur =  p_pat;
      u_pat    =  uglob;
      v_pat    =  vglob;
      goto  nurbs;
  }


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
       sprintf(errbuf,"sur211%%sur210");
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
    status = varkon_pat_pritop 
    (p_sur,p_pat,aiu,aiv);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pritop %%varkon_sur_eval (sur102)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


/*!                                                                 */
/* 3. Surface patch evaluation                                      */
/* ___________________________                                      */
/*                                                                  */
/* Calculate coordinates and derivatives in patch p_patcur          */
/* for local parameter point (u_pat,v_pat).                         */
/* Call of varkon_pat_eval (sur220).                                */
/*                                                                 !*/

nurbs:; /* Label: NURB_SUR have global parameter values             */

status=varkon_pat_eval
 (p_sur,p_patcur,icase,u_pat,v_pat,p_xyz);
if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur210 sur220 failed for u_pat %f  v_pat %f \n",
   u_pat,v_pat );
  fprintf(dbgfil(SURPAC),
  "sur210 nu %d nv %d iu %d iv %d p_patcur %d\n",
   (int)nu,(int)nv,(int)iu,(int)iv, (int)p_patcur );
  fflush(dbgfil(SURPAC)); 
  }
#endif
   sprintf(errbuf,"%f %f%%(varkon_sur_eval)",uglob-1.0,vglob-1.0);
       return(varkon_erpush("SU2843",errbuf));
   }

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

