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
/*  Function: varkon_pat_pritop                      File: sur232.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Printout of all data in a topological patch (or all patches)    */
/*  of type TOP_PAT (structure DBPatch). Refer to DB.h              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-04   Originally written                                 */
/*  1996-05-28   Eliminate compilation warnings                     */
/*  1996-10-20   Eliminate compilation warnings and fflush          */
/*  1996-11-15   CON_PAT, P3_PAT, ....                              */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_pritop     Print topological patch data     */
/*                                                              */
/*------------------------------------------------------------- */


/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush       * Error message to terminal        */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_pat_pritop (sur232) */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_pritop (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBint    aiu,          /* Adress to patch U. Eq. -1: All patches  */
  DBint    aiv )         /* Adress to patch V.                      */
/*                                                                  */
/* Out:                                                             */
/*   Printout to Debug file                                         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/* ----- Description of the VARKON data area for the surface -------*/
/*                                                                  */
/*  The order of the patches:                                       */
/*  -------------------------                                       */
/*                                                                  */
/*  IU=1,IV=NV                              IU=NU,IV=NV             */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  3  !  6   !  9   ! 12   ! 15  ! 18   !                      */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  2  !  5   !  8   ! 11   ! 14  ! 17   !                      */
/*   ----------------------------------------                       */
/*   !     !      !      !      !     !      !                      */
/*   !  1  !  4   !  7   ! 10   ! 13  ! 16   !                      */
/*   ----------------------------------------                       */
/*                                                                  */
/*  IU=1,IV=1                                IU=NU,IV=1             */
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    iu;          /* Loop index corresponding to u line      */
   DBint    iv;          /* Loop index corresponding to v line      */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
   GMPATC  *p_patc;      /* Current geometric, bicubic  patch (ptr) */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Enter*varkon_pat_pritop p_sur %d p_pat %d aiu %d aiv %d\n", 
          (int)p_sur, (int)p_pat, (short)aiu , (short)aiv );
  fprintf(dbgfil(SURPAC), "sur232   \n");
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* Number of patches in U and V from p_sur                          */
/*                                                                 !*/

nu = p_sur->nu_su; 
nv = p_sur->nv_su;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 No of patches in U direction nu= %d and V direction nv= %d\n",
     (short)nu,(short)nv);
 fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 2. Printout of topological patch data                            */
/* _____________________________________                            */
/*                                                                  */
/* Loop all V patches  iu=0,1,....,nu-1                             */
/*  Loop all U patches  iv=0,1,....,nv-1                            */
/*                                                                 !*/

for ( iu=0; iu<nu; ++iu )                /* Start loop V patches    */
  {
  for ( iv=0; iv<nv; ++iv )              /* Start loop U patches    */
    {

/*!                                                                 */
/*    Goto pri    if aiu= -1             (printout of all patches)  */
/*    Goto pri    if aiu= iu+1 aiv= iv+1 (printout of one patch)    */
/*    Goto nopri  in all other cases                                */
/*                                                                 !*/
    if ( aiu == -1 ) goto pri;           /* Printout patch (all)    */
    if ( aiu == iu+1 && aiv == iv+1)     /* Printout this patch     */
       goto pri;
    else                                 /* No printout of patch    */
       goto nopri;             

/*!                                                                 */
pri:   /* Label: Printout of current patch                          */
/*                                                                  */
/*    Pointer to current patch:                                     */
    p_t = p_pat + iu*nv + iv;
/*                                                                  */
/*    Printout of patch data for Debug On                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {

  fprintf(dbgfil(SURPAC), "sur232   \n");

  fprintf(dbgfil(SURPAC),
  "sur232 Topological adress iu_pat %d iv_pat %d     p_t %d No %d\n", 
       p_t->iu_pat,p_t->iv_pat,(int)p_t,(short)(iu*nv+iv+1));

  fflush(dbgfil(SURPAC)); 

  fprintf(dbgfil(SURPAC),
  "sur232 Secondary patch    su_pat %d su_pat %d  spek_c %d\n", 
       p_t->su_pat,p_t->sv_pat, (int)p_t->spek_c );

  fflush(dbgfil(SURPAC)); 

  if      ( p_t->styp_pat == CUB_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is CUB_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  fflush(dbgfil(SURPAC)); 
  p_patc = (GMPATC *)p_t->spek_c;
  fprintf(dbgfil(SURPAC),
  "sur232 From the geometric (bicubic) patch Offset %f\n", 
     p_patc->ofs_pat );
  fflush(dbgfil(SURPAC)); 
  }
  else if ( p_t->styp_pat == RAT_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is RAT_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == LFT_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is LFT_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == FAC_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is FAC_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == CON_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is CON_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat ==  P3_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is  P3_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat ==  P5_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is  P5_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat ==  P7_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is  P7_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat ==  P9_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is  P9_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == P21_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is P21_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == NUL_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is NUL_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else if ( p_t->styp_pat == TOP_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is TOP_PAT (styp_pat= %d) \n", 
     p_t->styp_pat);
  }
  else
  {
  fprintf(dbgfil(SURPAC),
  "sur232 Type of secondary patch is unknown (styp_pat= %d) \n", 
     p_t->styp_pat);
  }

  fflush(dbgfil(SURPAC)); 

  fprintf(dbgfil(SURPAC),
  "sur232 Geometry limit us %8.4f vs %8.4f ue %8.4f ve %8.4f  \n", 
     p_t->us_pat,p_t->vs_pat,p_t->ue_pat,p_t->ve_pat );

  fflush(dbgfil(SURPAC)); 

  fprintf(dbgfil(SURPAC),
  "sur232 box  %6.1f %6.1f %6.1f %6.1f %6.1f %6.1f flag= %d \n",
        p_t->box_pat.xmin, p_t->box_pat.ymin,p_t->box_pat.zmin,
        p_t->box_pat.xmax, p_t->box_pat.ymax,p_t->box_pat.zmax,
        p_t->box_pat.flag);

  fflush(dbgfil(SURPAC)); 

  fprintf(dbgfil(SURPAC),
  "sur232 cone %6.1f %6.1f %6.1f %6.1f flag= %d \n",
     p_t->cone_pat.xdir, p_t->cone_pat.ydir,p_t->cone_pat.zdir,
     p_t->cone_pat.ang , p_t->cone_pat.flag);

  if ( p_t->cone_pat.flag == -99 )
  fprintf(dbgfil(SURPAC),
  "sur232 Surface normals not OK (cone_pat.flag= %d )\n",
                         p_t->cone_pat.flag);


  fflush(dbgfil(SURPAC));

} /* End dbglev  */

#endif

/*!                                                                 */
nopri:;/* Label: No printout of patch                               */
/*                                                                 !*/

    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur232   \n");

  fprintf(dbgfil(SURPAC),
  "sur232 Exit*varkon_pat_pritop * No patches nu= %d nv= %d \n",
         (short)nu,(short)nv);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
