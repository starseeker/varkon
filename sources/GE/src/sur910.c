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
/*  Function: varkon_sur_defpatb                   File: sur910.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function defines the iso-parametric curves, which           */
/*  shall be the "patch boundaries" for a surface interrogation     */
/*  function. The output (fictitious) UV net defines the accuracy   */
/*  in the calculation search phase. The density of the UV curve    */
/*  net will for instance determine how many of the curve branches  */
/*  that will be found in surface/plane intersect calculation.      */
/*                                                                  */
/*  The output is normally a rectangular grid of UV rectangles      */
/*  (defined in the PBOUND table). In some cases will however       */
/*  the set of UV rectangles form "an irregular" UV net.            */
/*                                                                  */
/*           Regular UV net                   Irregular UV net      */
/*        ___________________                  ________________     */
/*       !         !         !                !    !     !     !    */
/*       !         !         !                !    !     !     !    */
/*       !    7    !    8    !                ! 14 ! 15  ! 16  !    */
/*       !_________!_________!              __!____!_____!_____!    */
/*       !         !         !             !  10   !_12__!     !    */
/*       !    5    !    6    !             !_______! 11  ! 13  !    */
/*       !_________!_________!              _______!_____!_____!    */
/*       !         !         !             !    !  !     !          */
/*       !         !         !             !    !  ! 8   !_____     */
/*       !    3    !    4    !             !    !  !_____!     !    */
/*       !         !         !             ! 5  !6 ! 7   !  9  !    */
/*       !_________!_________!             !____!__!_____!_____!    */
/*       !         !         !             !  !    !     !     !    */
/*       !    1    !    2    !             !1 ! 2  ! 3   !  4  !    */
/*       !_________!_________!             !__!____!_____!_____!    */
/*                                                                  */
/*                                                                  */
/*  This function will for some interrogation cases only create     */
/*  an UV net (the PBOUND table) any computations/estimations.      */
/*  For a surface/plane intersect with a surface consisting of      */
/*  bicubic patches only it is normally not necessary to make       */
/*  the UV net denser than for the input surface.                   */
/*                                                                  */
/*  For other interrogation cases will the necessary densisty       */
/*  of the UV net be calculated (estimated) in this function.       */
/*                                                                  */
/*                                                                  */
/*  The following examples, for the main different types of         */
/*  surface interrogations, show how the UV net is used:            */
/*                                                                  */
/*  - Calculate a curve on a surface (ex. plane/surface intersect)  */
/*                                                                  */
/*    The search phase is the calculation of entry/exit points      */
/*    in the UV net. The interrogation is made only in the curve    */
/*    net. For a plane/surface intersect calculation this means     */
/*    that the entry/exit points will be the curves/plane intersect */
/*    points. This implies that a curve branch or a part of a curve */
/*    will be "lost" if the UV net not is dense enough.             */
/*                                                                  */
/*  - Calculate a point on a surface (ex. closest point)            */
/*                                                                  */
/*    The restarts of the calculation from different start points   */
/*    can be regarded a the search phase. Solutions will be "lost"  */
/*    if the UV net not is dense enough.                            */
/*                                                                  */
/*  - Approximate a surface (ex. approximate with B_PLANE patches)  */
/*                                                                  */
/*    The UV net is the initial guess of the patch boundaries       */
/*    for the output surface. For a B_PLANE approximation is        */
/*    the necessary number of B_PLANE patches estimated ....        */
/*                                                                  */
/*  This is not a full description of how the UV net is used.       */
/*  Refer to the interrogation routines and the corresponding       */
/*  include files (surint.h, surdef.h, surevx.h, etc.)              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-02   Originally written                                 */
/*  1994-12-08   divappr 36 facettes for LFT_PAT                    */
/*  1995-02-18   D_GEODES                                           */
/*  1995-03-03   acc <= 10 for bicappr                              */
/*  1995-03-08   No patches np_u, np_v for bicappr                  */
/*  1996-01-31   No patches np_u, np_v for SURAPPR                  */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1996-06-12   Elimination of compilation warnings                */
/*  1997-03-08   bicappr (divide) for SURPLAN                       */
/*  1998-02-01   sur365 (NURBS) added, fflush moved                 */
/*  1998-02-08   case LINTSUR added in initial                      */
/*  1998-03-21   PBMAX -> pb_alloc                                  */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_defpatb    Define calcul. patch boundaries  */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short addrec ();       /* Add record to the PBOUND table     */
static short divappr();       /* Divide patch for the SURAPPR case  */
static short extrauv();       /* Add extra U and V lines            */
static short extrau();        /* Add extra U       lines            */
static short bicappr();       /* Divide patch for the BICAPPR case  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
static short initial();       /* Check and initializ.  For Debug On */
#endif

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat comptol;           /* Computer tolerance (accuracy)  */
static DBint   iu;                /* Loop index U line in surface   */
static DBint   iv;                /* Loop index V line in surface   */
static DBint   ip;                /* Loop index inside iu,iv patch  */
static DBint   numrec;            /* Record number  in PBOUND table */
static PBOUND *p_cr;              /* Current record in PBOUND (ptr) */
static DBint   e_ip;              /* No of "inside" PBOUND patches  */
static DBfloat us_a[DMAX];        /* Start U for PBOUND patches     */
static DBfloat vs_a[DMAX];        /* Start V for PBOUND patches     */
static DBfloat ue_a[DMAX];        /* End   U for PBOUND patches     */
static DBfloat ve_a[DMAX];        /* End   V for PBOUND patches     */
/*-----------------------------------------------------------------!*/




/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_sur_uvnmap     * Map UV net for NURBS           */
/*           varkon_comptol        * Retrieve computer tolerance    */
/*           varkon_erpush         * Error message to terminal      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_defpatb   */
/* SU2993 = Severe program error ( ) in varkon_sur_defpatb (sur910) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus      varkon_sur_defpatb (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBint   s_type,        /* Type CUB_SUR, RAT_SUR, or .....         */
  DBint   nu,            /* Number of patches in U direction        */
  DBint   nv,            /* Number of patches in V direction        */
  DBfloat s_lim[2][2],   /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint   ctype,         /* Computation case F_SILH, ... , SURPLAN  */
                         /* Eq. 1: Silhouette with  finite eyepoint */
                         /* Eq. 2: Silhouette,    infinite eyepoint */
                         /* Eq. 3: Isophote   with  finite eyepoint */
                         /* Eq. 4: Isophote,      infinite eyepoint */
                         /* Eq. 5: Plane/surface intersect          */
                         /* Eq. 6: Closest point on surface         */
                         /* Eq. 7: Closest curve on surface         */
                         /* Eq. 8: Surface approximation            */
                         /* Eq. 9: Silhouette defined by spine      */
                         /* Eq. 11: Geodesic start pt and tangent   */
  DBint   acc,           /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of patches     */
                         /* Eq. 3: Triple the number of patches     */
                         /* Eq. 4: ....                             */
  DBint   np_u,          /* No of approxim. patches per patch in U  */
  DBint   np_v,          /* No of approxim. patches per patch in V  */
  DBint   pb_alloc,      /* Size of the patch boundary table        */
  PBOUND *p_pbtable,     /* Pointer to patch boundary table         */
  DBint  *p_nr )         /* Number of records in pbtable            */

/* Out:                                                             */
/*        Start U,V value for all "calculation" patches             */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  s_iu;          /* Start loop value in   U direction       */
   DBint  s_iv;          /* Start loop value in   V direction       */
   DBint  e_iu;          /* End   loop value in   U direction       */
   DBint  e_iv;          /* End   loop value in   V direction       */
/*-----------------------------------------------------------------!*/

   DBint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ____________________                              */
/*               !                    !                             */
/*               ! varkon_sur_defpatb !                             */
/*               !     (sur910)       !                             */
/*               !____________________!                             */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*  ! Initiate  ! ! Data to the PBOUND table ! !  Exit  !           */
/*  !___________! !__________________________! !________!           */
/*  Debug ! On                 !                                    */
/*   _____!_____               !                                    */
/*  ! initial   !              !                                    */
/*  !___________!              !                                    */
/*       ______________________!_____________                       */
/*  ____!_____   __________!__________    ___!___                   */
/* !          ! !                   * !  !       !                  */
/* ! Initiate ! ! U direction patches !  !varkon_!                  */
/* !   loop   ! !_____________________!  ! normv !                  */
/* ! variabl. !  __________!__________   !_______!                  */
/* !__________! !                   * !                             */
/*              ! V direction patches !                             */
/*              !_____________________!                             */
/*       __________________!                                        */
/*  ____!____    __________!__________                              */
/* !         !  !                   * !                             */
/* ! divappr !  ! Within one patch    !                             */
/* !_________!  !_____________________!                             */
/*                     ____!___                                     */
/*                    !        !                                    */
/*                    ! addrec !                                    */
/*                    !________!                                   !*/
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  For Debug On: Initiate output and internal variables            */
/*                Call of initial (also printout of input data).    */
/*                                                                  */
/*  Check that  1 <= acc <= 5  for all cases                        */
/*                                                                 !*/

#ifdef DEBUG
  status=initial
   (p_sur,p_pat,s_type,nu,nv,s_lim,ctype,acc,np_u,np_v,p_pbtable,p_nr);
  if (status<0) 
      {
      sprintf(errbuf,"initial%%(sur910)");
      return(varkon_erpush("SU2973",errbuf));
      }
#endif

if ( acc >= 1 || acc <= 5 )
   {
    ; /* OK  */
   }
else
   {
   sprintf(errbuf,"(acc= %d)%%(sur910)",
                    (int)acc );
   return(varkon_erpush("SU2993",errbuf));
   }

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* 2. Data to PBOUND table pbtable                                  */
/* _______________________________                                  */
/*                                                                  */
/* Initiate loop variables: Let numrec= 0 and loop limits           */
/* s_iu, s_iv, e_iu, e_iv be the integer parts of s_lim - 1         */
/*                                                                 !*/

/* Initialize loop variables:                                       */
   numrec = 0;
   s_iu   = (DBint)s_lim[0][0] - 1; /* Start loop value U           */
   s_iv   = (DBint)s_lim[0][1] - 1; /* Start loop value V           */
   e_iu   = (DBint)s_lim[1][0] - 1; /* End   loop value U           */
   e_iv   = (DBint)s_lim[1][1] - 1; /* End   loop value V           */

/*!                                                                 */
/* Loop all V patches  iv=s_iu,....,e_iu                            */
/*  Loop all U patches  iu=s_iv,...,e_iv                            */
/*                                                                 !*/

   if ( s_iu < 0 || s_iv < 0 || e_iu > nu || e_iv > nv )
      {
      sprintf(errbuf, "(s_iu...)%%sur910");
      return(varkon_erpush("SU2993",errbuf));
      }

 if ( ctype == BICAPPR )
    {
    status=bicappr (pb_alloc, p_pbtable,
             np_u,np_v,s_iu,s_iv,e_iu,e_iv,p_pat,nv);
    if (status<0) 
      {
      sprintf(errbuf,"bicappr%%sur910");
      return(varkon_erpush("SU2973",errbuf));
      }

    goto  nomore;
    }

 if ( ctype == SURAPPR && np_u > 0 && np_v > 0 )
    {
    status=bicappr (pb_alloc, p_pbtable,
             np_u,np_v,s_iu,s_iv,e_iu,e_iv,p_pat,nv);
    if (status<0) 
      {
      sprintf(errbuf,"bicappr(FAC_SUR)%%sur910");
      return(varkon_erpush("SU2973",errbuf));
      }

    goto  nomore;
    }


 if ( ctype == SURPLAN && np_u > 0 && np_v > 0 )
    {
    status=bicappr (pb_alloc, p_pbtable,
             np_u,np_v,s_iu,s_iv,e_iu,e_iv,p_pat,nv);
    if (status<0) 
      {
      sprintf(errbuf,"bicappr(SURPLAN)%%sur910");
      return(varkon_erpush("SU2973",errbuf));
      }

    goto  nomore;
    }


for ( iu=s_iu; iu<= e_iu; ++iu )         /* Start loop U patches    */
  {
  for ( iv=s_iv; iv<= e_iv; ++iv )       /* Start loop V patches    */
    {


/*!                                                                 */
/*    Define the loop limit e_ip and all the UV patch rectangles    */
/*    (us_a,vs_a,ue_a,ve_a) in the current patch iu,iv.             */
/*    The loop limit e_ip is defined by the computation type        */
/*    the patch type and the accuracy.                              */
/*                                                                  */
/*    Let e_ip= 1 for all computation cases and patch types and     */
/*    let the UV rectangle be iu,iv,iu+1,iv+1 with the following    */
/*    exceptions:                                                   */
/*    Call of function divappr if ctype= SURAPPR.                   */
/*    Call of function extrau  if ctype= S_SILH and acc > 1         */
/*    Call of function extrauv if                   acc > 1         */
/*                                                                 !*/

      e_ip = 1; 
      us_a[0] =(DBfloat)iu+1.0+comptol;    /* U start point         */
      vs_a[0] =(DBfloat)iv+1.0+comptol;    /* V start point         */
      ue_a[0] =(DBfloat)iu+2.0-comptol;    /* U end   point         */
      ve_a[0] =(DBfloat)iv+2.0-comptol;    /* V end   point         */

      if ( ctype == SURAPPR )
        {
        status=divappr (p_sur,p_pat,nv, s_type);
        if (status<0) 
          {
          sprintf(errbuf,"divappr%%(sur910)");
          return(varkon_erpush("SU2973",errbuf));
          }
        }

      else if ( ctype == S_SILH && acc >  1 )
        {
        status=extrau  (acc);
        if (status<0) 
          {
          sprintf(errbuf,"extrau%%sur910");
          return(varkon_erpush("SU2973",errbuf));
          }
        }

      else if ( acc >   1 )
        {
        status=extrauv (acc);
        if (status<0) 
          {
          sprintf(errbuf,"extrauv%%(sur910)");
          return(varkon_erpush("SU2973",errbuf));
          }
        }

/*!                                                                 */
/*    Loop inside patch   ip=1,2,....,e_ip                          */
/*                                                                 !*/
      for ( ip=1; ip<= e_ip; ++ip )      /* Start loop inside patch */
        {

/*!                                                                 */
/*    Add record to the PBOUND table                                */
/*    Call of addrec.                                               */
/*                                                                 !*/

      status=addrec (p_pat , nv , pb_alloc, p_pbtable );
      if (status<0) 
        {
        sprintf(errbuf,"addrec%%(sur910)");
        return(varkon_erpush("SU2973",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur910 numrec %d  p_cr %d u_s %6.1f v_s %6.1f u_e %6.1f v_e %6.1f\n",
     (int)numrec , (int)p_cr , p_cr->us ,p_cr->vs ,p_cr->ue ,p_cr->ve );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur910 iu= %d iv= %d p_cr->pbox.flag= %d p_cr->pcone.flag %d\n",
        (int)iu,(int)iv,(int)p_cr->pbox.flag, (int)p_cr->pcone.flag);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur910 pcone xdir %8.5f ydir %8.5f zdir %8.5f ang %8.4f flag %d\n",
       p_cr->pcone.xdir, p_cr->pcone.ydir, p_cr->pcone.zdir, 
       p_cr->pcone.ang , (int)p_cr->pcone.flag);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur910 pbox  xmin %8.5f ymin %8.5f zmin %8.5f  flag %d\n",
       p_cr->pbox.xmin, p_cr->pbox.ymin, p_cr->pbox.zmin, 
       (int)p_cr->pbox.flag);
  fprintf(dbgfil(SURPAC),
  "sur910 pbox  xmax %8.5f ymax %8.5f zmax %8.5f \n",
       p_cr->pbox.xmax, p_cr->pbox.ymax, p_cr->pbox.zmax);
  }
#endif

      }                                  /* End   loop inside patch */
    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*   End                 ip=1,2,..,e_ip                             */
/*  End  all U patches  iu=s_iu,...,e_iu                            */
/* End  all V patches  iv=s_iv,...,e_iv                             */
/*                                                                 !*/

nomore:; /* Label: Bicubic approximation */

/*!                                                                 */
/* 3. Map UV net (for a NURBS surface)                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/

   status= varkon_sur_uvnmap
   (p_sur, p_pat, p_pbtable, numrec );
   if (status<0) 
      {
      sprintf(errbuf,"sur365%%sur910");
      return(varkon_erpush("SU2943",errbuf));
      }


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Total number of records to output variable                       */
   *p_nr = numrec;
/*                                                                 !*/

#ifdef DEBUG
      if ( *p_nr == 0 ) 
        {
        sprintf(errbuf,"*p_nr=0%%(sur910)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910 Exit *** varkon_sur_defpatb No of PBOUND records %d\n"
     , (int)*p_nr );
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!New-Page--------------------------------------------------------!*/

#ifdef DEBUG
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data and it initializes the        */
/* output variables and the static (common) variables to the        */
/* values 1.23456789 and 123456789.                                 */

   static short initial
   (p_sur,p_pat,s_type,nu,nv,s_lim,ctype,acc,np_u,np_v,p_pbtable,p_nr)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur;        /* Surface                           (ptr) */
  DBPatch *p_pat;        /* Alloc. area for topol. patch data (ptr) */
  DBint   s_type;        /* Type CUB_SUR, RAT_SUR, or .....         */
  DBint   nu;            /* Number of patches in U direction        */
  DBint   nv;            /* Number of patches in V direction        */
  DBfloat s_lim[2][2];   /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint   ctype;         /* Computation case F_SILH, ... , SURPLAN  */
                         /* Eq. 1: Silhouette with  finite eyepoint */
                         /* Eq. 2: Silhouette,    infinite eyepoint */
                         /* Eq. 3: Isophote   with  finite eyepoint */
                         /* Eq. 4: Isophote,      infinite eyepoint */
                         /* Eq. 5: Plane/surface intersect          */
                         /* Eq. 6: Closest point on surface         */
                         /* Eq. 7: Closest curve on surface         */
                         /* Eq. 8: Surface approximation            */
  DBint   acc;           /* Computation accuracy (case)             */
  DBint   np_u;          /* No of approxim. patches per patch in U  */
  DBint   np_v;          /* No of approxim. patches per patch in V  */
  PBOUND *p_pbtable;     /* Pointer to patch boundary table         */
  DBint  *p_nr;          /* Number of records in pbtable            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/* ==============================                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910*initial s_type %d nu %d nv %d ctype %d  acc %d\n",
      (int)s_type, (int)nu, (int)nv, (int)ctype , (int)acc );
fprintf(dbgfil(SURPAC),
"sur910*initial np_u  %d np_v %d \n", (int)np_u, (int)np_v );
fprintf(dbgfil(SURPAC),
"sur910*initial s_lim: Start U= %4.1f V= %4.1f End U= %4.1f V= %4.1f\n"
        , s_lim[0][0],s_lim[0][1],s_lim[1][0],s_lim[1][1]);
}

/*!                                                                 */
/* 2. Check of input data                                           */
/* ======================                                           */
/*                                                                  */
/* Check case of computation ctype and printout of case             */
/*                                                                 !*/

if      ( ctype == F_SILH  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case F_SILH:  Accuracy case acc= %d\n",(int)acc);
else if ( ctype == I_SILH  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case I_SILH : Accuracy case acc= %d\n",(int)acc);
else if ( ctype == F_ISOP  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case F_ISOP : Accuracy case acc= %d\n",(int)acc);
else if ( ctype == I_ISOP  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case I_ISOP : Accuracy case acc= %d\n",(int)acc);
else if ( ctype == SURPLAN )
fprintf(dbgfil(SURPAC),
"sur910*initial Case SURPLAN: Accuracy case acc= %d\n",(int)acc);
else if ( ctype == CLOSEPT )
fprintf(dbgfil(SURPAC),
"sur910*initial Case CLOSEPT: Accuracy case acc= %d\n",(int)acc);
else if ( ctype == LINTSUR )
fprintf(dbgfil(SURPAC),
"sur910*initial Case LINTSUR: Accuracy case acc= %d\n",(int)acc);
else if ( ctype == SURAPPR )
fprintf(dbgfil(SURPAC),
"sur910*initial Case SURAPPR: Accuracy case acc= %d\n",(int)acc);
else if ( ctype == S_SILH  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case S_SILH : Accuracy case acc= %d\n",(int)acc);
else if ( ctype == BICAPPR )
fprintf(dbgfil(SURPAC),
"sur910*initial Case BICAPPR: No patches np_u %d np_v %d\n",
                (int)np_u ,  (int)np_v);
else if ( ctype == D_GEODES  )
fprintf(dbgfil(SURPAC),
"sur910*initial Case D_GEODES: Accuracy case acc= %d\n",(int)acc);
else
  {
fprintf(dbgfil(SURPAC),
"sur910*initial Case ctype= %d is unknown (Error)\n",(int)ctype);
  sprintf(errbuf,"(ctype)%%(sur910)");
  return(varkon_erpush("SU2993",errbuf));
  }


/*!                                                                 */
/* Check that the surface is of type                                */
/* CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR                    */
/*                                                                 !*/

if ( s_type      ==  CUB_SUR ||         /* Check surface type      */
     s_type      ==  RAT_SUR ||           
     s_type      ==  MIX_SUR ||           
     s_type      ==  CON_SUR ||           
     s_type      ==  POL_SUR ||           
     s_type      ==   P3_SUR ||           
     s_type      ==   P5_SUR ||           
     s_type      ==   P7_SUR ||           
     s_type      ==   P9_SUR ||           
     s_type      ==  P21_SUR ||           
     s_type      == NURB_SUR ||           
     s_type      ==  LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf,   
 "(s_type)%%sur910"); 
 return(varkon_erpush("SU2993",errbuf)); 
 }


/*!                                                                 */
/* 3. Initiate output variables and static (internal) variables     */
/* ============================================================     */
/*                                                                 !*/
    *p_nr       = I_UNDEF;

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** addrec *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adds a record in table PBOUND.                      */

   static short addrec (p_pat , nv, pb_alloc, p_pbtable)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBPatch *p_pat;        /* Alloc. area for topol. patch data (ptr) */
  DBint    nv;           /* Number of patches in V direction        */
  DBint   pb_alloc;      /* Size of the patch boundary table        */
  PBOUND  *p_pbtable;    /* Patch boundary table              (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBPatch *p_t;          /* Current topological patch         (ptr) */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur910*addrec iu %d iv %d Input numrec %d ip %d\n",
      (int)iu, (int)iv , (int)numrec , (int)ip );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur910*addrec us_a %f vs_a %f ue_a %f ve_a %f\n",
      us_a[ip-1], vs_a[ip-1], ue_a[ip-1], ve_a[ip-1] );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 1. Update pointers to the PBOUND table                           */
/* ======================================                           */
/*                                                                  */
/* Let record number numrec = numrec+1. Check numrec <= pb_alloc    */
/* Let pointer to current record be p_cr= p_pbtable + numrec - 1    */
/*                                                                 !*/

    numrec = numrec+1;                   /* Record number           */
/*    if ( numrec > PBMAX ) 1998-03-21 */
    if ( numrec > pb_alloc )
      {
      sprintf(errbuf, "(pb_alloc/PBMAX)%%sur910");
      return(varkon_erpush("SU2993",errbuf));
      }

    p_cr= p_pbtable + numrec - 1;        /* Ptr to current record   */

/*!                                                                 */
/* 2. Data to the current PBOUND record                             */
/* ====================================                             */
/*                                                                  */
/* Record number rec_no= numrec                                     */
/* UV patch us,vs,ue,ve = us_a(ip),vs_a(ip),ue_a(ip),ve_a(ip)       */
/*                                                                 !*/


    p_cr->rec_no= numrec;                /* Record number           */
    p_cr->us=     us_a[ip-1];            /* U start point           */
    p_cr->vs=     vs_a[ip-1];            /* V start point           */
    p_cr->ue=     ue_a[ip-1];            /* U end   point           */
    p_cr->ve=     ve_a[ip-1];            /* V end   point           */

/*!                                                                 */
/* Retrieve BBOX and BCONE for current patch and copy to PBOUND     */
/* Pointer to current topological patch                             */
   p_t = p_pat + iu*nv + iv; 
/*                                                                 !*/

   p_cr->pbox=  p_t->box_pat;            /* BBOX  to PBOUND         */
   p_cr->pcone= p_t->cone_pat;           /* BCONE to PBOUND         */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** divappr ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function divides a patch for computation case SURAPPR        */

   static short divappr (p_sur,p_pat,nv,s_type)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur;        /* Surface                           (ptr) */
  DBPatch *p_pat;        /* Alloc. area for topol. patch data (ptr) */
  DBint   nv;            /* Number of patches in V direction        */
  DBint   s_type;        /* Type CUB_SUR, RAT_SUR, LFT_SUR, ...     */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   DBfloat cone_angle;   /* Angle of the BCONE                      */
/*-----------------------------------------------------------------!*/

   DBfloat us_p;         /* Start U value for the patch             */
   DBfloat vs_p;         /* Start V value for the patch             */
   DBfloat ue_p;         /* End   U value for the patch             */
   DBfloat ve_p;         /* End   V value for the patch             */
   DBfloat u_delta;      /* Delta U value                           */
   DBfloat v_delta;      /* Delta V value                           */
   DBint   iu_d;         /* Loop index for the U patch splitting    */
   DBint   iv_d;         /* Loop index for the V patch splitting    */
   char    errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBint   ix1;          /* Loop index for Debug Printout           */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Divide the patch                                              */
/* ===================                                              */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve BCONE angle for the current patch                       */
/* Pointer to current topological patch                             */
   p_t = p_pat + iu*nv + iv; 
/*                                                                 !*/

   cone_angle= p_t->cone_pat.ang;        /* BCONE angle             */

/*!                                                                 */
/* Determine the number of times the patch shall be divided         */
/* Let no of patches be  one  if       cone_angle < 10 degrees      */
/* Let no of patches be four  if 10 <= cone_angle < 30 degrees      */
/* Let no of patches be nine  if 30 <= cone_angle                   */
/* The cone angle is not calculated for LFT_SUR !!! tillf{lligt !   */
/* Let no of patches be nine  for a surface of type LFT_SUR         */
/*                                                                 !*/

   if      ( fabs(cone_angle) < 10.0 )
      {
      e_ip = 1; 
      us_a[0] =(DBfloat)iu+1.0+comptol;    /* U start point         */
      vs_a[0] =(DBfloat)iv+1.0+comptol;    /* V start point         */
      ue_a[0] =(DBfloat)iu+2.0-comptol;    /* U end   point         */
      ve_a[0] =(DBfloat)iv+2.0-comptol;    /* V end   point         */
      }
   else if ( fabs(cone_angle) >= 10.0 && fabs(cone_angle) < 30.0 )
      {
      e_ip = 0; 
      us_p    =(DBfloat)iu+1.0+comptol;    /* U start point         */
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point         */
      ue_p    =(DBfloat)iu+2.0-comptol;    /* U end   point         */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point         */
      u_delta = (ue_p - us_p)/2.0;
      v_delta = (ve_p - vs_p)/2.0;
      for ( iu_d= 0; iu_d <= 1; ++iu_d )
        {
        for ( iv_d= 0; iv_d <= 1; ++iv_d )
          {
          us_a[e_ip] =us_p+ iu_d   *u_delta; /* U start point       */
          vs_a[e_ip] =vs_p+ iv_d   *v_delta; /* V start point       */
          ue_a[e_ip] =us_p+(iu_d+1)*u_delta; /* U end   point       */
          ve_a[e_ip] =vs_p+(iv_d+1)*v_delta; /* V end   point       */
          e_ip = e_ip + 1;
          }
        }
      }
   else 
      {
      e_ip = 0; 
      us_p    =(DBfloat)iu+1.0+comptol;    /* U start point         */
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point         */
      ue_p    =(DBfloat)iu+2.0-comptol;    /* U end   point         */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point         */
      u_delta = (ue_p - us_p)/3.0;
      v_delta = (ve_p - vs_p)/3.0;
      for ( iu_d= 0; iu_d <= 2; ++iu_d )
        {
        for ( iv_d= 0; iv_d <= 2; ++iv_d )
          {
          us_a[e_ip] =us_p+ iu_d   *u_delta; /* U start point       */
          vs_a[e_ip] =vs_p+ iv_d   *v_delta; /* V start point       */
          ue_a[e_ip] =us_p+(iu_d+1)*u_delta; /* U end   point       */
          ve_a[e_ip] =vs_p+(iv_d+1)*v_delta; /* V end   point       */
          e_ip = e_ip + 1;
          if ( e_ip == DMAX )
             {
   sprintf(errbuf,"(Increase DMAX %d !)%%sur910", DMAX );
   return(varkon_erpush("SU2993",errbuf));
            }
          }
        }
      }


   if (  s_type == LFT_SUR )
      {
      e_ip = 0; 
      us_p    =(DBfloat)iu+1.0+comptol;    /* U start point         */
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point         */
      ue_p    =(DBfloat)iu+2.0-comptol;    /* U end   point         */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point         */
      u_delta = (ue_p - us_p)/5.0;
      v_delta = (ve_p - vs_p)/5.0;
      for ( iu_d= 0; iu_d <= 4; ++iu_d )
        {
        for ( iv_d= 0; iv_d <= 4; ++iv_d )
          {
          us_a[e_ip] =us_p+ iu_d   *u_delta; /* U start point       */
          vs_a[e_ip] =vs_p+ iv_d   *v_delta; /* V start point       */
          ue_a[e_ip] =us_p+(iu_d+1)*u_delta; /* U end   point       */
          ve_a[e_ip] =vs_p+(iv_d+1)*v_delta; /* V end   point       */
          e_ip = e_ip + 1;
          if ( e_ip == DMAX )
             {
   sprintf(errbuf,"(Increase DMAX %d !)%%sur910",
                    DMAX );
   return(varkon_erpush("SU2993",errbuf));
            }
          }
        }
      }


#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 && s_type == LFT_SUR )
{
fprintf(dbgfil(SURPAC),
"sur910*divappr cone_angle %f e_ip %d  s_type %d Temporary 9 \n",
       cone_angle , (int)e_ip ,  (int)s_type );
}
if  ( e_ip > 1 )
{
for ( ix1 = 1; ix1  <= e_ip; ++ix1  )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910*divappr %f %f %f %f\n",
  us_a[ix1-1], vs_a[ix1-1], ue_a[ix1-1], ve_a[ix1-1]);
fflush(dbgfil(SURPAC));
}
}
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** extrau  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adds extra patches in the U direction               */

   static short extrau  (acc)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint   acc;           /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of U patches   */
                         /* Eq. 3: Triple the number of U patches   */
                         /* Eq. 4: ....                             */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat   us_p;       /* Start U value for the patch             */
   DBfloat   vs_p;       /* Start V value for the patch             */
   DBfloat   ue_p;       /* End   U value for the patch             */
   DBfloat   ve_p;       /* End   V value for the patch             */
   DBfloat   u_delta;    /* Delta U value                           */
   DBfloat   v_delta;    /* Delta V value                           */
   DBint   iu_d;         /* Loop index for the U patch splitting    */
   DBint   iv_d;         /* Loop index for the V patch splitting    */
   DBint   nu_d;         /* Loop end for iu_d                       */
   DBint   nv_d;         /* Loop end for iv_d                       */
   char    errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBint   ix1;          /* Loop index for Debug Printout           */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Add extra U lines                                             */
/* ====================                                             */
/*                                                                 !*/

#ifdef DEBUG
if ( acc <= 1 || acc > 5 )
   {
   sprintf(errbuf,"(extrau acc %d)%%(sur910)",
                    (int)acc );
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

/*!                                                                 */
/* Determine the number of output (extra) U patches:                */
/* Let no of patches be   two if acc= 2                             */
/* Let no of patches be three if acc= 3                             */
/* Let no of patches be  ....                                       */
/*                                                                 !*/

   e_ip    = 0;                            /* Initiate no output    */
   us_p    =(DBfloat)iu+1.0+comptol;       /* U start point         */
   vs_p    =(DBfloat)iv+1.0+comptol;       /* V start point         */
   ue_p    =(DBfloat)iu+2.0-comptol;       /* U end   point         */
   ve_p    =(DBfloat)iv+2.0-comptol;       /* V end   point         */
   u_delta = (ue_p - us_p)/(DBfloat)acc;   /* Delta U               */
   v_delta = (ve_p - vs_p);                /* Delta V               */
   nu_d    = acc - 1;                      /* End loop iu_d         */
   nv_d    = 0;                            /* End loop iv_d         */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur910*extrau nu_d %d nv_d %d u_delta %f v_delta %f\n", 
      (int)nu_d,(int)nv_d,u_delta,v_delta);
}
#endif

   for ( iu_d= 0; iu_d <= nu_d; ++iu_d )
     {
     for ( iv_d= 0; iv_d <= nv_d; ++iv_d )
       {
       us_a[e_ip] =us_p+ iu_d   *u_delta;    /* U start point       */
       vs_a[e_ip] =vs_p+ iv_d   *v_delta;    /* V start point       */
       ue_a[e_ip] =us_p+(iu_d+1)*u_delta;    /* U end   point       */
       ve_a[e_ip] =vs_p+(iv_d+1)*v_delta;    /* V end   point       */
       e_ip = e_ip + 1;
       if ( e_ip == DMAX )
             {
   sprintf(errbuf,"(Increase DMAX %d !)%%sur910",
                    DMAX );
   return(varkon_erpush("SU2993",errbuf));
            }
       }
     }



#ifdef  DEBUG
for ( ix1 = 1; ix1  <= e_ip; ++ix1  )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910*extrau  %f %f %f %f\n",
  us_a[ix1-1], vs_a[ix1-1], ue_a[ix1-1], ve_a[ix1-1]);
fflush(dbgfil(SURPAC));
}
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** extrauv ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adds extra patches in the U and V direction.        */

   static short extrauv (acc)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint   acc;           /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of U patches   */
                         /* Eq. 3: Triple the number of U patches   */
                         /* Eq. 4: ....                             */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat us_p;         /* Start U value for the patch             */
   DBfloat vs_p;         /* Start V value for the patch             */
   DBfloat ue_p;         /* End   U value for the patch             */
   DBfloat ve_p;         /* End   V value for the patch             */
   DBfloat u_delta;      /* Delta U value                           */
   DBfloat v_delta;      /* Delta V value                           */
   DBint   iu_d;         /* Loop index for the U patch splitting    */
   DBint   iv_d;         /* Loop index for the V patch splitting    */
   DBint   nu_d;         /* Loop end for iu_d                       */
   DBint   nv_d;         /* Loop end for iv_d                       */
   char    errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBint   ix1;          /* Loop index for Debug Printout           */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Add extra U and V lines                                       */
/* ==========================                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( acc <= 1 || acc > 5 )
   {
   sprintf(errbuf,"(extrau acc %d)%%(sur910)", (int)acc );
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

/*!                                                                 */
/* Determine the number of output (extra) U patches:                */
/* Let no of patches be  four if acc= 2                             */
/* Let no of patches be  nine if acc= 3                             */
/* Let no of patches be  ....                                       */
/*                                                                 !*/

   e_ip    = 0;                            /* Initiate no output    */
   us_p    =(DBfloat)iu+1.0+comptol;       /* U start point         */
   vs_p    =(DBfloat)iv+1.0+comptol;       /* V start point         */
   ue_p    =(DBfloat)iu+2.0-comptol;       /* U end   point         */
   ve_p    =(DBfloat)iv+2.0-comptol;       /* V end   point         */
   u_delta = (ue_p - us_p)/(DBfloat)acc;   /* Delta U               */
   v_delta = (ve_p - vs_p)/(DBfloat)acc;   /* Delta V               */
   nu_d    = acc - 1;                      /* End loop iu_d         */
   nv_d    = acc - 1;                      /* End loop iv_d         */

   for ( iu_d= 0; iu_d <= nu_d; ++iu_d )
     {
     for ( iv_d= 0; iv_d <= nv_d; ++iv_d )
       {
       us_a[e_ip] =us_p+ iu_d   *u_delta;    /* U start point       */
       vs_a[e_ip] =vs_p+ iv_d   *v_delta;    /* V start point       */
       ue_a[e_ip] =us_p+(iu_d+1)*u_delta;    /* U end   point       */
       ve_a[e_ip] =vs_p+(iv_d+1)*v_delta;    /* V end   point       */
       e_ip = e_ip + 1;
       if ( e_ip == DMAX )
             {
   sprintf(errbuf,"(Increase DMAX %d !)%%sur910",
                    DMAX );
   return(varkon_erpush("SU2993",errbuf));
            }
       }
     }



#ifdef  DEBUG
for ( ix1 = 1; ix1  <= e_ip; ++ix1  )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910*extrauv %f %f %f %f\n",
  us_a[ix1-1], vs_a[ix1-1], ue_a[ix1-1], ve_a[ix1-1]);
fflush(dbgfil(SURPAC));
}
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ** bicappr ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates UV net for Bicubic surface approximation    */

   static short bicappr (pb_alloc, p_pbtable,
             np_u,np_v,s_iu,s_iv,e_iu,e_iv,p_pat,nv)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint    pb_alloc;     /* Size of the patch boundary table        */
  PBOUND  *p_pbtable;    /* Patch boundary table              (ptr) */
  DBint    np_u;         /* No of approxim. patches per patch in U  */
  DBint    np_v;         /* No of approxim. patches per patch in V  */
  DBint    s_iu;         /* Start loop value in   U direction       */
  DBint    s_iv;         /* Start loop value in   V direction       */
  DBint    e_iu;         /* End   loop value in   U direction       */
  DBint    e_iv;         /* End   loop value in   V direction       */
  DBPatch *p_pat;        /* Alloc. area for topol. patch data (ptr) */
  DBint    nv;           /* Number of patches in V direction        */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat  us_p;        /* Start U value for the patch             */
   DBfloat  vs_p;        /* Start V value for the patch             */
   DBfloat  ue_p;        /* End   U value for the patch             */
   DBfloat  ve_p;        /* End   V value for the patch             */
   DBfloat  u_delta;     /* Delta U value                           */
   DBfloat  v_delta;     /* Delta V value                           */
   DBint    iu_d;        /* Loop index for the U patch splitting    */
   DBint    iv_d;        /* Loop index for the V patch splitting    */
   DBint    nu_d;        /* Loop end for iu_d                       */
   DBint    nv_d;        /* Loop end for iv_d                       */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiate and check                                            */
/* ==========================                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( np_u <= 1 || np_u > 30 ||
     np_v <= 1 || np_v > 30    )
   {
   sprintf(errbuf,"bicappr %d or %d > 30%%sur910",
                    (int)np_u , (int)np_v  );
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

nu_d    = np_u - 1;                      /* End loop iu_d           */
nv_d    = np_v - 1;                      /* End loop iv_d           */

for ( iu=s_iu; iu<= e_iu; ++iu )         /* Start loop U patches    */
  {
  us_p    =(DBfloat)iu+1.0+comptol;        /* U start point           */
  ue_p    =(DBfloat)iu+2.0-comptol;        /* U end   point           */
  u_delta =(ue_p - us_p)/(DBfloat)np_u;    /* Delta U                 */
  for ( iu_d= 0; iu_d <= nu_d; ++iu_d )  /* Start loop extra U lines*/
    {
    for ( iv=s_iv; iv<= e_iv; ++iv )     /* Start loop V patches    */
      {
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point           */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point           */
      v_delta =(ve_p - vs_p)/(DBfloat)np_v;/* Delta V                 */

      for ( iv_d= 0; iv_d <= nv_d; ++iv_d )
        {
        numrec = numrec + 1;             
        if ( numrec > pb_alloc )
          {
          sprintf(errbuf, "pb_alloc(PBMAX)%%sur910");
          return(varkon_erpush("SU2993",errbuf));
          }

        p_cr= p_pbtable + numrec - 1;     /* Ptr to current record   */
        p_cr->rec_no= numrec;             /* Record number           */
        p_cr->us=                         /* U start point           */
               us_p+ iu_d   *u_delta;
        p_cr->vs=                         /* V start point           */
               vs_p+ iv_d   *v_delta; 
        p_cr->ue=                         /* U end   point           */
               us_p+(iu_d+1)*u_delta;
        p_cr->ve=                         /* V end   point           */
               vs_p+(iv_d+1)*v_delta; 

/*!                                                                 */
/* Retrieve BBOX and BCONE for current patch and copy to PBOUND     */
/* Pointer to current topological patch 1996-01-31                  */
   p_t = p_pat + iu*nv + iv; 
/*                                                                 !*/

   p_cr->pbox=  p_t->box_pat;            /* BBOX  to PBOUND         */
   p_cr->pcone= p_t->cone_pat;           /* BCONE to PBOUND         */

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur910*bicappr %f %f %f %f\n",
  p_cr->us,p_cr->vs,p_cr->ue,p_cr->ve );
fflush(dbgfil(SURPAC));
}
#endif
       }
     }
   } /* End loop            */
} /* End loop            */


    return(SUCCED);

} /* End of function                                                */
