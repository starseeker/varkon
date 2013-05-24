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
/*  Function: varkon_sur_offset                    File: sur180.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Creates an offset surface defined as the input surface          */
/*  with an offset: r_off(u,v)= r(u,v) + offset*n(u,v).             */
/*      where       r_off(u,v)= Offset surface                      */
/*                  r(u,v)    = Input surface                       */
/*                  offset    = Offset value                        */
/*                  n(u,v)    = Surface normal                      */
/*                                                                  */
/*  The function copies all the patches and changes the             */
/*  offset value for each patch.                                    */
/*                                                                  */
/*  The function checks that the offset surface is well             */
/*  defined. An offset value greater than the minimum               */
/*  radius of curvature will result in an error.                    */
/*                                                                  */
/*  TODO                                                            */
/*  !! Not yet programmed that surface data is copied               */
/*  !! Add calls of varkon_ini_gmpat and _gmsur   !!                */
/*  The function will allocate memory area for the patch data       */
/*  if input surface pointers (DBSurf) not are equal. The new       */
/*  allocated area must be deallocated by the calling function.     */
/*                                                                  */
/*  There will be no allocation of memory if the input surface      */
/*  pointers (DBSurf) are equal.                                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-02-26   CON_SUR, CON_PAT added                             */
/*  1998-02-07   NURB_SUR, NURB_PAT added                           */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_offset     Create an offset surface         */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch    * Retrieve number of patches               */
/* varkon_sur_bound      * Bound. boxes & cones for surf.           */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_offset (sur180) */
/* SU2993 = Severe program error (  ) in varkon_sur_offset (sur180) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_offset (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_surin,      /* Input surface                     (ptr) */
  DBPatch *p_patin,      /* Alloc. area for topol. patch data (ptr) */
  DBfloat  offset,       /* Offset value                            */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
                         /* This function allocates the area        */
                         /* if p_surin not is equal to p_surout     */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    iu;          /* Loop index corresponding to u line      */
   DBint    iv;          /* Loop index corresponding to v line      */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    surtype;     /* Type of surface                         */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_patcin;    /* Input   geometric, bicubic  patch (ptr) */
   GMPATR  *p_patrin;    /* Input   geometric, rational patch (ptr) */
   GMPATL  *p_patlin;    /* Input   geometric, lofting  patch (ptr) */
   GMPATN  *p_patnin;    /* Input   geometric, Consurf  patch (ptr) */
   GMPATNU *p_patnuin;   /* Input   geometric, NURBS    patch (ptr) */
#ifdef EJUTNYTTJADE
   GMPATC  *p_patcout;   /* Output  geometric, bicubic  patch (ptr) */
   GMPATR  *p_patrout;   /* Output  geometric, rational patch (ptr) */
   GMPATL  *p_patlout;   /* Output  geometric, lofting  patch (ptr) */
   GMPATN  *p_patnout;   /* Output  geometric, Consurf  patch (ptr) */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   DBint  nurb_flag;     /* NURBS flag for programming check        */
   DBint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur180 Enter* p_surin %d p_surout %d offset %f\n",
          (int)p_surin, (int)p_surout, offset);
  }
#endif
 
/* Initialize NURBS flag used only for a programming check          */

   nurb_flag = I_UNDEF;

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of an acceptable type                  */
/*                                                                 !*/


    status = varkon_sur_nopatch
  (p_surin, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_offset (sur180)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if ( surtype     ==  CUB_SUR ||
     surtype     ==  RAT_SUR ||
     surtype     ==  LFT_SUR ||
     surtype     ==  CON_SUR ||
     surtype     == NURB_SUR ||
     surtype     ==  MIX_SUR )
 ;
 else
 {
 sprintf(errbuf,  
 "surf type not impl.%%sur180"); 
 return(varkon_erpush("SU2993",errbuf)); 
 }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur180 Surface type=%d\n",
      (int)p_surin->typ_su);
  fprintf(dbgfil(SURPAC),"sur180 No patches in U direction nu=%d\n",
      (int)nu);
  fprintf(dbgfil(SURPAC),"sur180 No patches in V direction nv=%d\n",
      (int)nv);
  }
#endif

/*!                                                                 */
/* 2. Allocate topological patch area                               */
/* __________________________________                               */
/*                                                                  */
/*                                                                  */
/* If p_surin not is equal p_surout:                                */
/*        i. Allocate area for output topological patches           */
/* If p_surin is equal p_surout:                                    */
/*        i. Let pp_patout = NULL                                   */
/*                                                                 !*/
   if ( p_surin == p_surout)
      {
      *pp_patout = NULL;                 /* Output pointer          */
      }
   else
      {
      sprintf(errbuf,"(no copy)%%varkon_sur_offset (sur180)");
      return(varkon_erpush("SU2993",errbuf));
      }
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 3. Create the offset surface                                     */
/* ____________________________                                     */
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
      p_t = p_patin + iu*nv + iv; 
/*                                                                  */
/*    Pointer to current geometric (secondary)                      */
/*    patch p_patcin, p_patrin or p_patlin from                     */
/*    the topological patch p_t.                                    */
/*                                                                  */
/*    Retrieve offset ofs_pat.                                      */
/*                                                                  */
/*    If p_surin not is equal p_surout:                             */
/*        i. Allocate area for output geometric patch               */
/*       ii. Copy patch data to new offset surface                  */
/*      iii. Change the offset ofs_pat= ofs_pat+offset              */
/*    If p_surin is equal p_surout:                                 */
/*        i. Change the offset ofs_pat= ofs_pat+offset              */
/*                                                                 !*/

    if      ( p_t->styp_pat == CUB_PAT )
      {
      p_patcin = (GMPATC *)p_t->spek_c;
      p_patcin->ofs_pat= p_patcin->ofs_pat + offset;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur180 iu %d iv %d Modifified offset: p_patcin->ofs_pat %f\n",
          (int)iu,(int)iv,p_patcin->ofs_pat );
  }
#endif
 
      }
    else if ( p_t->styp_pat == RAT_PAT )
      {
      p_patrin = (GMPATR *)p_t->spek_c;
      p_patrin->ofs_pat= p_patrin->ofs_pat + offset;
      }
    else if ( p_t->styp_pat == LFT_PAT )
      {
      p_patlin = (GMPATL *)p_t->spek_c;
      p_patlin->ofs_pat= p_patlin->ofs_pat + offset;
      }
    else if ( p_t->styp_pat == CON_PAT )
      {
      p_patnin = (GMPATN *)p_t->spek_c;
      p_patnin->ofs_pat= p_patnin->ofs_pat + offset;
      }
    else if ( p_t->styp_pat == NURB_PAT )
      {
      if  ( iu == 0 && iv == 0 && nurb_flag == I_UNDEF ) 
        nurb_flag = 1;
      else
        {
        sprintf(errbuf,"(Not only one NURB_PAT)%%sur180");
        return(varkon_erpush("SU2993",errbuf));
        }
      p_patnuin = (GMPATNU *)p_t->spek_c;
      p_patnuin->ofs_pat= p_patnuin->ofs_pat + offset;
      }
    else if ( p_t->styp_pat == TOP_PAT )
      {
      if  ( (iu != 0 || iv != 0) && nurb_flag == 1 ) ; /* OK */
      else
        {
        sprintf(errbuf,"(TOP_PAT but not NURBS)%%sur180");
        return(varkon_erpush("SU2993",errbuf));
        }
      }
    else
      {
      sprintf(errbuf,"(patch type)%%sur180");
      return(varkon_erpush("SU2993",errbuf));
    }

    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

/*!                                                                 */
/* 4. Check the offset surface                                      */
/* ____________________________                                     */
/*                                                                  */
/*  ...  Not yet implemented ...                                    */
/*                                                                 !*/

/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*  .. Only for a not copied surface is implemented ...             */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surin,p_patin,acc);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_offset(sur180)");
        return(varkon_erpush("SU2943",errbuf));
        }


/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur180 Exit ****** varkon_sur_offset ********\n");
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
