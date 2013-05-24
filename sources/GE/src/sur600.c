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
/*  Function: varkon_sur_bictra                    File: sur600.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a bicubic surface with the input transformation      */
/*  matrix (the input coordinate system).                           */
/*                                                                  */
/*  The input surface will not be copied. The input surface         */
/*  (patch) coefficients will be modified. Also the bounding        */
/*  box and cone data will be modified.                             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-05   Originally written                                 */
/*  1997-12-04   Transformation of surface BBOX                     */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_bictra     Transform bicubic surface        */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_pat_bictra      * Transform bicubic patch                 */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_pat_trabound    * Transform BBOX and BCONE                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_bictra (sur620) */
/* SU2993 = Severe program error (  ) in varkon_sur_bictra (sur620) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_bictra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input/output surface              (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBTmat  *p_c )         /* Transform. matrix (local system)  (ptr) */

/* Out:                                                             */
/*        Coefficients of surface is modified                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    iu;          /* Loop index corresponding to u line      */
   DBint    iv;          /* Loop index corresponding to v line      */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    surtype;     /* Type CUB_SUR                            */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_p;         /* Current geometric   patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   BCONE  dum_bcone;     /* Dummy bounding cone for surface         */
   DBint  status;        /* Error code from called function         */
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
  "sur600 Enter ***** varkon_sur_bictra ******** p_pat %d\n",p_pat);
  fprintf(dbgfil(SURPAC),"sur600 Input system    p_c=%d\n",p_c);
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
 
if ( p_c == NULL ) return(SUCCED);       /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type CUB_SUR.                       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_bictra (sur600)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (surtype == CUB_SUR )                /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_bictra (sur600");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur600 Surface type=%d\n",p_sur->typ_su);
  fprintf(dbgfil(SURPAC),"sur600 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),"sur600 No patches in V direction nv=%d\n",nv);
  fprintf(dbgfil(SURPAC),"sur600 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur600 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur600 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur600 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur600 g41 %f g42 %f g43 %f g44 %f \n",
                  p_c->g41,p_c->g42,p_c->g43,p_c->g44);
 fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 2. Transformation of the surface                                 */
/* ________________________________                                 */
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

    if ( p_t->styp_pat == CUB_PAT )
      {
      p_p = (GMPATC *)p_t->spek_c;
      }
    else
      {
      sprintf(errbuf,"(not CUB_PAT)%%varkon_sur_bictra (sur600)");
      return(varkon_erpush("SU2993",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur600 Before sur620 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
  fprintf(dbgfil(SURPAC),"sur600 a00x %f a01x %f a02x %f a03x %f \n",
   p_p->a00x,p_p->a01x,p_p->a02x,p_p->a03x);
  fprintf(dbgfil(SURPAC),"sur600 a10x %f a11x %f a12x %f a13x %f \n",
   p_p->a10x,p_p->a11x,p_p->a12x,p_p->a13x);
  fprintf(dbgfil(SURPAC),"sur600 a20x %f a21x %f a22x %f a23x %f \n",
   p_p->a20x,p_p->a21x,p_p->a22x,p_p->a23x);
  fprintf(dbgfil(SURPAC),"sur600 a30x %f a31x %f a32x %f a33x %f \n",
   p_p->a30x,p_p->a31x,p_p->a32x,p_p->a33x);
  fprintf(dbgfil(SURPAC),"sur600 a00y %f a01y %f a02y %f a03y %f \n",
   p_p->a00y,p_p->a01y,p_p->a02y,p_p->a03y);
  fprintf(dbgfil(SURPAC),"sur600 a10y %f a11y %f a12y %f a13y %f \n",
   p_p->a10y,p_p->a11y,p_p->a12y,p_p->a13y);
  fprintf(dbgfil(SURPAC),"sur600 a20y %f a21y %f a22y %f a23y %f \n",
   p_p->a20y,p_p->a21y,p_p->a22y,p_p->a23y);
  fprintf(dbgfil(SURPAC),"sur600 a30y %f a31y %f a32y %f a33y %f \n",
   p_p->a30y,p_p->a31y,p_p->a32y,p_p->a33y);
  fprintf(dbgfil(SURPAC),"sur600 a00z %f a01z %f a02z %f a03z %f \n",
   p_p->a00z,p_p->a01z,p_p->a02z,p_p->a03z);
  fprintf(dbgfil(SURPAC),"sur600 a10z %f a11z %f a12z %f a13z %f \n",
   p_p->a10z,p_p->a11z,p_p->a12z,p_p->a13z);
  fprintf(dbgfil(SURPAC),"sur600 a20z %f a21z %f a22z %f a23z %f \n",
   p_p->a20z,p_p->a21z,p_p->a22z,p_p->a23z);
  fprintf(dbgfil(SURPAC),"sur600 a30z %f a31z %f a32z %f a33z %f \n",
   p_p->a30z,p_p->a31z,p_p->a32z,p_p->a33z);
 fflush(dbgfil(SURPAC));

  }
#endif


/*!                                                                 */
/*     Transformation of the current geometric patch p_p.           */
/*     Call of function varkon_pat_bictra (sur620)                  */
/*                                                                 !*/
/*     (Error SU2943 if function fails)                             */

    status = varkon_pat_bictra (p_p,p_c,p_p);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_bictra%%varkon_sur_bictra (sur600)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur600 After sur620 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
  fprintf(dbgfil(SURPAC),"sur600 a00x %f a01x %f a02x %f a03x %f \n",
   p_p->a00x,p_p->a01x,p_p->a02x,p_p->a03x);
  fprintf(dbgfil(SURPAC),"sur600 a10x %f a11x %f a12x %f a13x %f \n",
   p_p->a10x,p_p->a11x,p_p->a12x,p_p->a13x);
  fprintf(dbgfil(SURPAC),"sur600 a20x %f a21x %f a22x %f a23x %f \n",
   p_p->a20x,p_p->a21x,p_p->a22x,p_p->a23x);
  fprintf(dbgfil(SURPAC),"sur600 a30x %f a31x %f a32x %f a33x %f \n",
   p_p->a30x,p_p->a31x,p_p->a32x,p_p->a33x);
  fprintf(dbgfil(SURPAC),"sur600 a00y %f a01y %f a02y %f a03y %f \n",
   p_p->a00y,p_p->a01y,p_p->a02y,p_p->a03y);
  fprintf(dbgfil(SURPAC),"sur600 a10y %f a11y %f a12y %f a13y %f \n",
   p_p->a10y,p_p->a11y,p_p->a12y,p_p->a13y);
  fprintf(dbgfil(SURPAC),"sur600 a20y %f a21y %f a22y %f a23y %f \n",
   p_p->a20y,p_p->a21y,p_p->a22y,p_p->a23y);
  fprintf(dbgfil(SURPAC),"sur600 a30y %f a31y %f a32y %f a33y %f \n",
   p_p->a30y,p_p->a31y,p_p->a32y,p_p->a33y);
  fprintf(dbgfil(SURPAC),"sur600 a00z %f a01z %f a02z %f a03z %f \n",
   p_p->a00z,p_p->a01z,p_p->a02z,p_p->a03z);
  fprintf(dbgfil(SURPAC),"sur600 a10z %f a11z %f a12z %f a13z %f \n",
   p_p->a10z,p_p->a11z,p_p->a12z,p_p->a13z);
  fprintf(dbgfil(SURPAC),"sur600 a20z %f a21z %f a22z %f a23z %f \n",
   p_p->a20z,p_p->a21z,p_p->a22z,p_p->a23z);
  fprintf(dbgfil(SURPAC),"sur600 a30z %f a31z %f a32z %f a33z %f \n",
   p_p->a30z,p_p->a31z,p_p->a32z,p_p->a33z);
 fflush(dbgfil(SURPAC));

  }
#endif

/*!                                                                 */
/*     Transformation of BBOX and BCONE.                            */
/*     Call of varkon_pat_trabound (sur916).                        */
/*                                                                 !*/

status=varkon_pat_trabound
(&p_t->box_pat, &p_t->cone_pat, p_c , &p_t->box_pat, &p_t->cone_pat);
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_bictra (sur600)");
   return(varkon_erpush("SU2943",errbuf));
   }


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur600 p_t %d &p_t->box_pat %d\n",
          p_t ,  &p_t->box_pat   );
 fflush(dbgfil(SURPAC));
  }
#endif


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
"sur600 Before p_sur->box_su.ymin %f \n", p_sur->box_su.ymin);
fflush(dbgfil(SURPAC)); 
}
#endif

      varkon_ini_bcone (&dum_bcone);

status=varkon_pat_trabound
(&p_sur->box_su , &dum_bcone , p_c , &p_sur->box_su , &dum_bcone );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox surface)%%sur600");
   return(varkon_erpush("SU2943",errbuf));
   }




/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur600 Exit ****** varkon_sur_bictra ********\n");
 fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
