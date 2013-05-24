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
/*  Function: varkon_sur_rbictra                   File: sur602.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a rational bicubic surface with the input            */
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
/*sdescr varkon_sur_rbictra    Transform rat. bicubic surface   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_pat_rbictra     * Transform rat. bicubic patch            */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_pat_trabound    * Transform BBOX and BCONE                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function . failed in varkon_sur_rbictra (sur602) */
/* SU2993 = Severe program error ( ) in varkon_sur_rbictra (sur602) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus  varkon_sur_rbictra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input/output surface              (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBTmat  *pc )          /* Transformation matrix             (ptr) */

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
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATR  *pp;          /* Current geometric   patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   BCONE  dum_bcone;     /* Dummy bounding cone for surface         */
   DBint  surtype;       /* Type RAT_SUR                            */
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
/* Return if input system pc not is defined (pc= NULL)              */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur602 Enter ***** varkon_sur_rbictra *******\n");
  fprintf(dbgfil(SURPAC),"sur602 Input system    pc=%d\n",pc);
  }
#endif
 
if ( pc == NULL ) return(SUCCED);        /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type RAT_SUR.                       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_rbictra (sur602)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (surtype == RAT_SUR )                /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_rbictra (sur602");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur602 Surface type=%d\n",p_sur->typ_su);
  fprintf(dbgfil(SURPAC),"sur602 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),"sur602 No patches in V direction nv=%d\n",nv);
  fprintf(dbgfil(SURPAC),"sur602 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur602 g11 %f g12 %f g13 %f g14 %f \n",
                  pc->g11,pc->g12,pc->g13,pc->g14);
  fprintf(dbgfil(SURPAC),"sur602 g21 %f g22 %f g23 %f g24 %f \n",
                  pc->g21,pc->g22,pc->g23,pc->g24);
  fprintf(dbgfil(SURPAC),"sur602 g31 %f g32 %f g33 %f g34 %f \n",
                  pc->g31,pc->g32,pc->g33,pc->g34);
  fprintf(dbgfil(SURPAC),"sur602 g41 %f g42 %f g43 %f g44 %f \n",
                  pc->g41,pc->g42,pc->g43,pc->g44);
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
/*    patch pp from the topological patch                           */
/*                                                                 !*/

    if ( p_t->styp_pat == RAT_PAT )
      {
      pp = (GMPATR *)p_t->spek_c;
      }
    else
      {
      sprintf(errbuf,"(not RAT_PAT)%%varkon_sur_rbictra (sur600)");
      return(varkon_erpush("SU2993",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur602 Before sur620 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
  fprintf(dbgfil(SURPAC),"sur602 a00x %f a01x %f a02x %f a03x %f \n",
   pp->a00x,pp->a01x,pp->a02x,pp->a03x);
  fprintf(dbgfil(SURPAC),"sur602 a10x %f a11x %f a12x %f a13x %f \n",
   pp->a10x,pp->a11x,pp->a12x,pp->a13x);
  fprintf(dbgfil(SURPAC),"sur602 a20x %f a21x %f a22x %f a23x %f \n",
   pp->a20x,pp->a21x,pp->a22x,pp->a23x);
  fprintf(dbgfil(SURPAC),"sur602 a30x %f a31x %f a32x %f a33x %f \n",
   pp->a30x,pp->a31x,pp->a32x,pp->a33x);
  fprintf(dbgfil(SURPAC),"sur602 a00y %f a01y %f a02y %f a03y %f \n",
   pp->a00y,pp->a01y,pp->a02y,pp->a03y);
  fprintf(dbgfil(SURPAC),"sur602 a10y %f a11y %f a12y %f a13y %f \n",
   pp->a10y,pp->a11y,pp->a12y,pp->a13y);
  fprintf(dbgfil(SURPAC),"sur602 a20y %f a21y %f a22y %f a23y %f \n",
   pp->a20y,pp->a21y,pp->a22y,pp->a23y);
  fprintf(dbgfil(SURPAC),"sur602 a30y %f a31y %f a32y %f a33y %f \n",
   pp->a30y,pp->a31y,pp->a32y,pp->a33y);
  fprintf(dbgfil(SURPAC),"sur602 a00z %f a01z %f a02z %f a03z %f \n",
   pp->a00z,pp->a01z,pp->a02z,pp->a03z);
  fprintf(dbgfil(SURPAC),"sur602 a10z %f a11z %f a12z %f a13z %f \n",
   pp->a10z,pp->a11z,pp->a12z,pp->a13z);
  fprintf(dbgfil(SURPAC),"sur602 a20z %f a21z %f a22z %f a23z %f \n",
   pp->a20z,pp->a21z,pp->a22z,pp->a23z);
  fprintf(dbgfil(SURPAC),"sur602 a30z %f a31z %f a32z %f a33z %f \n",
   pp->a30z,pp->a31z,pp->a32z,pp->a33z);
  fprintf(dbgfil(SURPAC),"sur602 a00  %f a01  %f a02  %f a03  %f \n",
   pp->a00 ,pp->a01 ,pp->a02 ,pp->a03 );
  fprintf(dbgfil(SURPAC),"sur602 a10  %f a11  %f a12  %f a13  %f \n",
   pp->a10 ,pp->a11 ,pp->a12 ,pp->a13 );
  fprintf(dbgfil(SURPAC),"sur602 a20  %f a21  %f a22  %f a23  %f \n",
   pp->a20 ,pp->a21 ,pp->a22 ,pp->a23 );
  fprintf(dbgfil(SURPAC),"sur602 a30  %f a31  %f a32  %f a33  %f \n",
   pp->a30 ,pp->a31 ,pp->a32 ,pp->a33 );

  }
#endif


/*!                                                                 */
/*  Transformation of the current geometric patch pp.               */
/*  Call of function varkon_pat_rbictra (sur622)                    */
/*                                                                 !*/
/*  (Error SU2943 if function fails)                                */
/*                                                                 !*/

    status = varkon_pat_rbictra (pp,pc,pp);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_rbictra%%varkon_sur_rbictra (sur602)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && iu == nu-1 && iv == nv-1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur602 After sur620 transformation Patch iu=%d iv=%d\n",iu+1,iv+1);
  fprintf(dbgfil(SURPAC),"sur602 a00x %f a01x %f a02x %f a03x %f \n",
   pp->a00x,pp->a01x,pp->a02x,pp->a03x);
  fprintf(dbgfil(SURPAC),"sur602 a10x %f a11x %f a12x %f a13x %f \n",
   pp->a10x,pp->a11x,pp->a12x,pp->a13x);
  fprintf(dbgfil(SURPAC),"sur602 a20x %f a21x %f a22x %f a23x %f \n",
   pp->a20x,pp->a21x,pp->a22x,pp->a23x);
  fprintf(dbgfil(SURPAC),"sur602 a30x %f a31x %f a32x %f a33x %f \n",
   pp->a30x,pp->a31x,pp->a32x,pp->a33x);
  fprintf(dbgfil(SURPAC),"sur602 a00y %f a01y %f a02y %f a03y %f \n",
   pp->a00y,pp->a01y,pp->a02y,pp->a03y);
  fprintf(dbgfil(SURPAC),"sur602 a10y %f a11y %f a12y %f a13y %f \n",
   pp->a10y,pp->a11y,pp->a12y,pp->a13y);
  fprintf(dbgfil(SURPAC),"sur602 a20y %f a21y %f a22y %f a23y %f \n",
   pp->a20y,pp->a21y,pp->a22y,pp->a23y);
  fprintf(dbgfil(SURPAC),"sur602 a30y %f a31y %f a32y %f a33y %f \n",
   pp->a30y,pp->a31y,pp->a32y,pp->a33y);
  fprintf(dbgfil(SURPAC),"sur602 a00z %f a01z %f a02z %f a03z %f \n",
   pp->a00z,pp->a01z,pp->a02z,pp->a03z);
  fprintf(dbgfil(SURPAC),"sur602 a10z %f a11z %f a12z %f a13z %f \n",
   pp->a10z,pp->a11z,pp->a12z,pp->a13z);
  fprintf(dbgfil(SURPAC),"sur602 a20z %f a21z %f a22z %f a23z %f \n",
   pp->a20z,pp->a21z,pp->a22z,pp->a23z);
  fprintf(dbgfil(SURPAC),"sur602 a30z %f a31z %f a32z %f a33z %f \n",
   pp->a30z,pp->a31z,pp->a32z,pp->a33z);
  fprintf(dbgfil(SURPAC),"sur602 a00  %f a01  %f a02  %f a03  %f \n",
   pp->a00 ,pp->a01 ,pp->a02 ,pp->a03 );
  fprintf(dbgfil(SURPAC),"sur602 a10  %f a11  %f a12  %f a13  %f \n",
   pp->a10 ,pp->a11 ,pp->a12 ,pp->a13 );
  fprintf(dbgfil(SURPAC),"sur602 a20  %f a21  %f a22  %f a23  %f \n",
   pp->a20 ,pp->a21 ,pp->a22 ,pp->a23 );
  fprintf(dbgfil(SURPAC),"sur602 a30  %f a31  %f a32  %f a33  %f \n",
   pp->a30 ,pp->a31 ,pp->a32 ,pp->a33 );

  }
#endif

/*!                                                                 */
/*     Transformation of BBOX and BCONE.                            */
/*     Call of varkon_pat_trabound (sur916).                        */
/*                                                                 !*/

status=varkon_pat_trabound
(&p_t->box_pat, &p_t->cone_pat, pc , &p_t->box_pat, &p_t->cone_pat);
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_bictra (sur600)");
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
"sur602 Before p_sur->box_su.ymin %f \n", p_sur->box_su.ymin);
fflush(dbgfil(SURPAC)); 
}
#endif

      varkon_ini_bcone (&dum_bcone);

status=varkon_pat_trabound
(&p_sur->box_su , &dum_bcone , pc  , &p_sur->box_su , &dum_bcone );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox surface)%%sur602");
   return(varkon_erpush("SU2943",errbuf));
   }


/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur602 Exit ****** varkon_sur_rbictra ********\n");
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
