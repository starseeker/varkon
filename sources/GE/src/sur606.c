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
/*  Function: varkon_sur_contra                    File: sur606.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a Consurf surface with the input transformation      */
/*  matrix (the input coordinate system).                           */
/*                                                                  */
/*  The input surface will not be copied. The input surface         */
/*  (patch) coefficients will be modified.                          */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-04   Originally written                                 */
/*  1997-12-04   Transformation of surface BBOX                     */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_contra     Transform a Consurf surface      */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_pat_contra      * Transform a Consurf patch               */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_pat_trabound    * Transform BBOX and BCONE                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function . failed in varkon_sur_contra  (sur606) */
/* SU2993 = Severe program error ( ) in varkon_sur_contra  (sur606) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus     varkon_sur_contra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input/output surface              (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBTmat  *p_c )         /* Local coordinate system           (ptr) */

/* Out:                                                             */
/*        Coefficients of surface in area p_pat is modified         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   gmint    iu;          /* Loop index corresponding to u line      */
   gmint    iv;          /* Loop index corresponding to v line      */
   gmint    nu;          /* Number of patches in U direction        */
   gmint    nv;          /* Number of patches in V direction        */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATN  *p_p;         /* Current geometric   patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   BCONE  dum_bcone;     /* Dummy bounding cone for surface         */
   gmint  surtype;       /* Type CON_SUR                            */
   gmint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Return if input system p_c not is defined (p_c= NULL)            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur606 Enter ***** varkon_sur_contra  *******\n");
  fprintf(dbgfil(SURPAC),"sur606 Input system    p_c=%d\n",p_c);
  }
#endif
 
if ( p_c == NULL ) return(SUCCED);       /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type CON_SUR.                       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_contra  (sur606)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (surtype == CON_SUR )                /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_contra (sur606");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur606 Input surf. type=%d\n",p_sur->typ_su);
  fprintf(dbgfil(SURPAC),"sur606 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),"sur606 No patches in V direction nv=%d\n",nv);
  fprintf(dbgfil(SURPAC),"sur606 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur606 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur606 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur606 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur606 g41 %f g42 %f g43 %f g44 %f \n",
                  p_c->g41,p_c->g42,p_c->g43,p_c->g44);
  }
#endif

/*!                                                                 */
/* 2. Transformation of the surface                                 */
/* _________________________________                                */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv< nv; ++iv )               /* Start loop V patches    */
  {
  for ( iu=0; iu< nu; ++iu )             /* Start loop U patches    */
    {

/*!                                                                 */
/*    Pointer to current topological patch                          */
      p_t = p_pat + iu*nv + iv; 
/*    Pointer to current geometric (secondary)                      */
/*    patch p_p from the topological patch                          */
/*                                                                 !*/

    if ( p_t->styp_pat == CON_PAT )
      {
      p_p = (GMPATN *)p_t->spek_c;
      }
    else
      {
      sprintf(errbuf,"(not CON_PAT)%%varkon_sur_contra (sur606)");
      return(varkon_erpush("SU2993",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
  "sur606 Patch iu= %d iv= %d Current patch p_p= %d\n", iu+1,iv+1,p_p);
 fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/*  Transformation of the current geometric patch p_p.              */
/*  Call of function varkon_pat_contra  (sur626)                    */
/*                                                                 !*/

    status = varkon_pat_contra  (p_p,p_c,p_p);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_contra %%varkon_sur_contra  (sur606)");
    return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/*     Transformation of BBOX and BCONE.                            */
/*     Call of varkon_pat_trabound (sur916).                        */
/*                                                                 !*/

status=varkon_pat_trabound
(&p_t->box_pat, &p_t->cone_pat, p_c , &p_t->box_pat, &p_t->cone_pat);
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_contra  (sur606)");
   return(varkon_erpush("SU2943",errbuf));
   }


    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/


/*!                                                                 */
/* Transformation of BBOX for the surface.                          */
/* Call of varkon_pat_trabound (sur916).                            */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur606 Before p_sur->box_su.ymin %f \n", p_sur->box_su.ymin);
fflush(dbgfil(SURPAC)); 
}
#endif

      varkon_ini_bcone (&dum_bcone);

status=varkon_pat_trabound
(&p_sur->box_su , &dum_bcone , p_c , &p_sur->box_su , &dum_bcone );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox surface)%%sur606");
   return(varkon_erpush("SU2943",errbuf));
   }




/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur606 Exit ****** varkon_sur_contra  *******\n");
 fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
