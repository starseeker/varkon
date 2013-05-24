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
/*  Function: varkon_sur_poltra                    File: sur520.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a polynomial surface with the input transformation   */
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
/*  1996-10-29   Originally written                                 */
/*  1997-12-09   Transformation of surface BBOX                     */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poltra     Transform a polynomial surface   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_pat_trap3       * Transform a  P3_PAT patch               */
/* varkon_pat_trap5       * Transform a  P5_PAT patch               */
/* varkon_pat_trap7       * Transform a  P7_PAT patch               */
/* varkon_pat_trap9       * Transform a  P9_PAT patch               */
/* varkon_pat_trap21      * Transform a P21_PAT patch               */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_pat_trabound    * Transform BBOX and BCONE                */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function . failed in varkon_sur_poltra  (sur520) */
/* SU2993 = Severe program error ( ) in varkon_sur_poltra  (sur520) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus      varkon_sur_poltra (

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
   DBint     iu;         /* Loop index corresponding to u line      */
   DBint     iv;         /* Loop index corresponding to v line      */
   DBint     nu;         /* Number of patches in U direction        */
   DBint     nv;         /* Number of patches in V direction        */
   DBPatch  *p_t;        /* Current topological patch         (ptr) */
   GMPATP3  *p_p3;       /* Current geometric  P3_PAT patch   (ptr) */
   GMPATP5  *p_p5;       /* Current geometric  P5_PAT patch   (ptr) */
   GMPATP7  *p_p7;       /* Current geometric  P7_PAT patch   (ptr) */
   GMPATP9  *p_p9;       /* Current geometric  P9_PAT patch   (ptr) */
   GMPATP21 *p_p21;      /* Current geometric P21_PAT patch   (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   BCONE     dum_bcone;  /* Dummy bounding cone for surface         */
   DBint     surtype;    /* Type CON_SUR                            */
   DBint     status;     /* Error code from called function         */
   char      errbuf[80]; /* String for error message fctn erpush    */

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
"sur520 Enter ***** varkon_sur_poltra  *******\n");
fprintf(dbgfil(SURPAC),"sur520 Input system p_c=%d\n",(int)p_c);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
if ( p_c == NULL ) return(SUCCED);       /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type POL_SUR.                       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_poltra  (sur520)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (surtype == POL_SUR  ||              /* Check surface type      */
    surtype ==  P3_SUR  ||
    surtype ==  P5_SUR  ||
    surtype ==  P7_SUR  ||
    surtype ==  P9_SUR  ||
    surtype == P21_SUR     )
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_poltra (sur520");
 return(varkon_erpush("SU2993",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur520 Input surf. type=%d\n",p_sur->typ_su);
  fprintf(dbgfil(SURPAC),"sur520 No patches in U direction nu=%d\n",
                                  (short)nu);
  fprintf(dbgfil(SURPAC),"sur520 No patches in V direction nv=%d\n",
                                  (short)nv);
  fprintf(dbgfil(SURPAC),"sur520 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur520 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur520 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur520 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur520 g41 %f g42 %f g43 %f g44 %f \n",
                  p_c->g41,p_c->g42,p_c->g43,p_c->g44);
 fflush(dbgfil(SURPAC)); 
  }
#endif

   p_p3  = NULL;
   p_p5  = NULL;
   p_p7  = NULL;
   p_p9  = NULL;
   p_p21 = NULL;

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
/*    Pointer to current geometric (secondary) patch p_p3, p_p5,    */
/*    p_p7, p_p9 or p_21 from the topological patch                 */
/*    Transformation of the current geometric patch.                */
/*    Call of function varkon_pat_trap3 (sur510), _trap5 (sur511),  */
/*    _trap7 (sur512), _trap9 (sur513) or _trap21 (sur514).         */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur520 IU %d IV %d Patch type %d\n",
                   (int)iu+1, (int)iv+1, p_t->styp_pat);
fflush(dbgfil(SURPAC));
}
#endif

    switch ( p_t->styp_pat )
      {
      case  P3_PAT:
      p_p3  = (GMPATP3 *)p_t->spek_c;
      status = varkon_pat_trap3 ( p_p3 , p_c, p_p3  );
      break;

      case  P5_PAT:
      p_p5  = (GMPATP5 *)p_t->spek_c;
      status = varkon_pat_trap5 ( p_p5 , p_c, p_p5  );
      break;

      case  P7_PAT:
      p_p7  = (GMPATP7 *)p_t->spek_c;
      status = varkon_pat_trap7 ( p_p7 , p_c, p_p7  );
      break;

      case  P9_PAT:
      p_p9  = (GMPATP9 *)p_t->spek_c;
      status = varkon_pat_trap9 ( p_p9 , p_c, p_p9  );
      break;

      case P21_PAT:
      p_p21 = (GMPATP21 *)p_t->spek_c;
      status = varkon_pat_trap21 ( p_p21, p_c, p_p21 );
      break;

      default:
      sprintf(errbuf,"Patch type %d",(short)p_t->styp_pat);
      return(varkon_erpush("SU2993",errbuf));
      break;
      }

  if  ( status < 0 )
      {
      sprintf(errbuf,"(transf. failed)%%sur520");
      return(varkon_erpush("SU2993",errbuf));
      }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
  "sur520 Patch iu= %d iv= %d Current patch p_p3= %d\n",
   (short)(iu+1),(short)(iv+1),(int)p_p3);
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
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_poltra  (sur520)");
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
"sur520 Before p_sur->box_su.ymin %f \n", p_sur->box_su.ymin);
fflush(dbgfil(SURPAC)); 
}
#endif

      varkon_ini_bcone (&dum_bcone);

status=varkon_pat_trabound
(&p_sur->box_su , &dum_bcone , p_c , &p_sur->box_su , &dum_bcone );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox surface)%%sur520");
   return(varkon_erpush("SU2943",errbuf));
   }



/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur520 Exit ****** varkon_sur_poltra  *******\n");
 fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
