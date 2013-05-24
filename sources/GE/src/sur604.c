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
/*  Function: varkon_sur_lofttra                   File: sur604.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a conic lofting surface with the input               */
/*  transformation matrix (the input coordinate system).            */
/*                                                                  */
/*  The input surface will not be copied. The input surface         */
/*  (patch) coefficients will be modified.                          */
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
/*sdescr varkon_sur_lofttra    Transform conic lofting surface  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_pat_lofttra     * Transform conic lofting patch           */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_pat_trabound    * Transform BBOX and BCONE                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function . failed in varkon_sur_lofttra (sur604) */
/* SU2993 = Severe program error ( ) in varkon_sur_lofttra (sur604) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus     varkon_sur_lofttra (

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
   DBint    iu;          /* Loop index corresponding to u line      */
   DBint    iv;          /* Loop index corresponding to v line      */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATL  *p_p;         /* Current geometric   patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   BCONE  dum_bcone;     /* Dummy bounding cone for surface         */
   DBint  surtype;       /* Type LFT_SUR                            */
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
  "sur604 Enter ***** varkon_sur_lofttra *******\n");
  fprintf(dbgfil(SURPAC),"sur604 Input system    p_c=%d\n",p_c);
  }
#endif
 
if ( p_c == NULL ) return(SUCCED);       /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type LFT_SUR.                       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_lofttra (sur604)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (surtype == LFT_SUR )                /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_lofttra (sur604");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur604 Input surf. type=%d\n",p_sur->typ_su);
  fprintf(dbgfil(SURPAC),"sur604 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),"sur604 No patches in V direction nv=%d\n",nv);
  fprintf(dbgfil(SURPAC),"sur604 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur604 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur604 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur604 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur604 g41 %f g42 %f g43 %f g44 %f \n",
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

    if ( p_t->styp_pat == LFT_PAT )
      {
      p_p = (GMPATL *)p_t->spek_c;
      }
    else
      {
      sprintf(errbuf,"(not LFT_PAT)%%varkon_sur_lofttra (sur600)");
      return(varkon_erpush("SU2993",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
  "sur604 Patch iu= %d iv= %d Current patch p_p= %d\n", iu+1,iv+1,p_p);
}
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
{
fprintf(dbgfil(SURPAC),
  "sur604 Before sur624 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->spine.c0x,    p_p->spine.c1x,
            p_p->spine.c2x,    p_p->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->spine.c0y,    p_p->spine.c1y,
            p_p->spine.c2y,    p_p->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0z %f c1z %f c2z %f c3z %f \n"
       ,    p_p->spine.c0z,    p_p->spine.c1z,
            p_p->spine.c2z,    p_p->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->spine.c0 ,    p_p->spine.c1 ,
            p_p->spine.c2 ,    p_p->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur604 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,    p_p->spine.ofs,    p_p->spine.typ,    p_p->spine.sl );
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->lims.c0x,    p_p->lims.c1x,
            p_p->lims.c2x,    p_p->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->lims.c0y,    p_p->lims.c1y,
            p_p->lims.c2y,    p_p->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0z %f c1z %f c2z %f c3z %f \n"
       ,    p_p->lims.c0z,    p_p->lims.c1z,
            p_p->lims.c2z,    p_p->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->lims.c0 ,    p_p->lims.c1 ,
            p_p->lims.c2 ,    p_p->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur604 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,    p_p->lims.ofs,    p_p->lims.typ,    p_p->lims.sl );
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->pval.c0x ... . c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->pval.c0x,    p_p->pval.c1x,
            p_p->pval.c2x,    p_p->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->pval.c0y,    p_p->pval.c1y,
            p_p->pval.c2y,    p_p->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->pval.c0 ,    p_p->pval.c1 ,
            p_p->pval.c2 ,    p_p->pval.c3  );
  }
#endif


/*!                                                                 */
/*  Transformation of the current geometric patch p_p.              */
/*  Call of function varkon_pat_lofttra (sur624)                    */
/*                                                                 !*/

    status = varkon_pat_lofttra (p_p,p_c,p_p);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_lofttra%%varkon_sur_lofttra (sur604)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
{
fprintf(dbgfil(SURPAC),
"sur604 After sur624 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->spine.c0x,    p_p->spine.c1x,
            p_p->spine.c2x,    p_p->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->spine.c0y,    p_p->spine.c1y,
            p_p->spine.c2y,    p_p->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0z %f c1z %f c2z %f c3z %f \n"
       ,    p_p->spine.c0z,    p_p->spine.c1z,
            p_p->spine.c2z,    p_p->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->spine.c0 ,    p_p->spine.c1 ,
            p_p->spine.c2 ,    p_p->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur604 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,    p_p->spine.ofs,    p_p->spine.typ,    p_p->spine.sl );
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->lims.c0x,    p_p->lims.c1x,
            p_p->lims.c2x,    p_p->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->lims.c0y,    p_p->lims.c1y,
            p_p->lims.c2y,    p_p->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0z %f c1z %f c2z %f c3z %f \n"
       ,    p_p->lims.c0z,    p_p->lims.c1z,
            p_p->lims.c2z,    p_p->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->lims.c0 ,    p_p->lims.c1 ,
            p_p->lims.c2 ,    p_p->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur604 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,    p_p->lims.ofs,    p_p->lims.typ,    p_p->lims.sl );
fprintf(dbgfil(SURPAC),
"sur604 Coefficients:     p_p->pval.c0x ... . c3  \n");
fprintf(dbgfil(SURPAC),
"sur604 c0x %f c1x %f c2x %f c3x %f \n"
       ,    p_p->pval.c0x,    p_p->pval.c1x,
            p_p->pval.c2x,    p_p->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur604 c0y %f c1y %f c2y %f c3y %f \n"
       ,    p_p->pval.c0y,    p_p->pval.c1y,
            p_p->pval.c2y,    p_p->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur604 c0  %f c1  %f c2  %f c3  %f \n"
       ,    p_p->pval.c0 ,    p_p->pval.c1 ,
            p_p->pval.c2 ,    p_p->pval.c3  );
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
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_lofttra (sur604)");
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
"sur604 Before p_sur->box_su.ymin %f \n", p_sur->box_su.ymin);
fflush(dbgfil(SURPAC)); 
}
#endif

      varkon_ini_bcone (&dum_bcone);

status=varkon_pat_trabound
(&p_sur->box_su , &dum_bcone , p_c , &p_sur->box_su , &dum_bcone );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox surface)%%sur604");
   return(varkon_erpush("SU2943",errbuf));
   }



/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur604 Exit ****** varkon_sur_lofttra *******\n");
 fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
