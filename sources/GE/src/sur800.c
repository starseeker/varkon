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
/*  Function: varkon_sur_creloft                   File: sur800.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/* !!!  TODO Decide if sur720 shall add 1.0 to parameter value      */
/* !!!  Or is it an error in GE703 that return MBS param. value!!   */
/* !!!  Check/handling of curve directions should be added !!!!!    */
/*                                                                  */
/* !!! It should be required that the P-value function is defined   */
/* !!! from 0 to length of spine. If the spine is shortened will    */
/* !!! the p values not be OK unless the function is trimmed.       */
/*                                                                  */
/* !!! To make offset allowed must curve plane be aded in LFT_PAT   */
/* !!! (enough with a vector that sur222 converts to coordinate     */
/* !!! system when the offset curve is evaluated).                  */
/*                                                                  */
/* !!! This routine should hopefully be able to handle UV_CUB_SEG   */
/* !!! curves. It is probably only handling of the patch data       */
/* !!! that is retrieved by DBread_one_patch in sur214 that need    */
/* !!! to be added. Use the new functions GE109 and/or GE110.       */
/*                                                                  */
/*  The function creates a conic lofting surface of type LFT_SUR,   */
/*  i.e. a surface with patches of type LFT_PAT.                    */
/*                                                                  */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*  Deallocation must not be made for pointer pp_pat= NULL          */
/*  but in all other cases (also if there is an error).             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-10-11   Debug added, bug arcpoi fixed n_int ne 1  G Liden  */
/*  1994-11-22   Debug added                               G Liden  */
/*  1994-11-27   pp_at= NULL                               G Liden  */
/*  1995-02-17   Check also last point of P value fctn     G Liden  */
/*  1995-02-26   Debug ..                                  G Liden  */
/*  1995-03-07   UV_CUB_SEG and offset curves              G Liden  */
/*  1995-03-19   Documentation                             G Liden  */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1996-08-29   Debug                                              */
/*  1997-02-09   Elimination of compilation warning                 */
/*  1997-03-08   Check mflag,  fflush                               */
/*  1999-12-18   Free source code modifications                     */
/*  2007-01-11   acc=1 => acc=0  calculation also of cones, Sören L.*/
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_creloft    Create a LFT_SUR surface         */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Check and initiations For Debug On */
#endif
static short chepval();       /* Check P value curve                */
static short ubounds();       /* Patch U boundary values            */
static short compare();       /* Compare two points w.r.t ctol      */
static short clopoi();        /* Closest   spine curve pt (U value) */
static short arcpoi();        /* Arclength spine curve pt (U value) */
static short spipla();        /* Spine directrix and P-value planes */
static short credir();        /* Create directrix or Pvalue segment */
static short crevlim();       /* Create V limit curves              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  ctol;             /* Coordinate tolerance           */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
static DBPatch *p_t;              /* Current topological patch (ptr)*/
static DBfloat  spine_u[SPINE];   /* Spine parameter values         */
static DBint    n_spine_u;        /* Number of values in spine_u    */
static DBint    nu;               /* Number of patches i U direct.  */
static DBint    nv;               /* Number of patches i V direct.  */
static DBint    iu;               /* Loop index patch in U direct.  */
static DBint    iv;               /* Loop index patch in V direct.  */
static DBfloat  splane[4];        /* Start spine plane directrix    */
static DBfloat  eplane[4];        /* End   spine plane directrix    */
static DBfloat  splane_p[4];      /* Start spine plane P-value      */
static DBfloat  eplane_p[4];      /* End   spine plane P-value      */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol        * Retrieve computer tolerance              */
/* varkon_ctol           * Retrieve coordinate tolerance            */
/* varkon_ini_gmsur      * Initiate DBSurf                          */
/* varkon_ini_gmpat      * Initiate DBPatch                         */
/* varkon_ini_gmpatl     * Initiate GMPATL                          */
/* GE109                 * Curve   evaluation routine               */
/* GE703                 * Closest point on a curve                 */
/* GE200                 * Create a straight line                   */
/* GE718                 * Relative arclength for global U          */
/* GE717                 * Global U for relative arclength          */
/* GEtfseg_to_local      * Transformate curve segment               */
/* varkon_cur_intplan    * Curve and plane intersection             */
/* varkon_cur_reparam    * Reparam. a rational segment              */
/* varkon_sur_bound      * Bounding box & cone for surface          */
/* varkon_normv          * Vector normalization                     */
/* varkon_pat_priconloft * Printout lofting patch                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2413 = Offset curve segments not yet implemented               */
/* SU2583 = Intersect with .. curve failed. (Plane d= ..)           */
/* SU2593 = Number of plane/curve intersect points .. (not 1)       */
/* SU2603 = Spine length .. is less than .. mm                      */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_creloft   */
/* SU2993 = Severe program error ( ) in varkon_sur_creloft (sur800) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus           varkon_sur_creloft (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine,      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg,   /* Spine segments                    (ptr) */
  DBint    no_strips,    /* Number of surface strips                */
  DBCurve  lim[STRIP],   /* Limit   curves                          */
  DBSeg   *p_lseg[STRIP],/* Limit   curves segments           (ptr) */
  DBCurve  tan[STRIP],   /* Tangent curves                          */
  DBSeg   *p_tseg[STRIP],/* Tangent curves segments           (ptr) */
  DBCurve  mid[STRIP],   /* Mid     curves                          */
  DBSeg   *p_mseg[STRIP],/* Mid     curves segments           (ptr) */
  DBint    mflag[STRIP], /* Flag for P-value or Mid curve           */
                         /* Eq. 1: P value    Eq. 2: Mid curve      */
                         /* Eq. 3: Rotation                         */
  DBSurf  *p_sur,        /* Output surface                    (ptr) */
  DBPatch **pp_pat )      /* Allocated area for patch data     (ptr) */

/* Out:                                                             */
/*                                                                  */
/*       Data to p_sur and coefficients to the pp_pat area          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATL *p_cpat;       /* Current geometric patch           (ptr) */
/*-----------------------------------------------------------------!*/

   DBint  i_s;           /* Loop index surface patch record         */
   DBPatch *p_frst;       /* First patch in allocated area     (ptr) */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   DBint  pat_no;        /* Patch number in surface                 */
   DBint  pflag;         /* P value flag for function credir        */
   DBint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String 1 for error message fctn erpush  */

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ____________________                              */
/*               !                    !                             */
/*               ! varkon_sur_creloft !                             */
/*               !     (sur800)       !                             */
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

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 Enter varkon_sur_creloft: Create a LFT_SUR surface \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Initialize output and internal variables    For Debug On.       */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

#ifdef DEBUG
    status=initial
   (p_spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag,p_sur,pp_pat);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* Initiate pp_pat to NULL. Variable pp_pat is also a flag          */
/* to the calling function. Memory must be deallocated if           */
/* pp_pat not is equal to NULL (also for errors).                   */
/*                                                                 !*/

*pp_pat = NULL;                          /* Initiate output pointer */

/*!                                                                 */
/* Computer   tolerance. Call of varkon_comptol (sur753).           */
/* Coordinate tolerance. Call of varkon_ctol    (sur751).           */
/*                                                                 !*/

   comptol=varkon_comptol();
   ctol   =varkon_ctol();

/*!                                                                 */
/* Initiate surface data in p_sur.                                  */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur); 

/*!                                                                 */
/* Check P value curves and mflag values                            */
/* Calls of chepval.                                                */
/*                                                                 !*/
  for ( iv =1; iv <= no_strips ; ++iv )
    {
    if ( mflag[iv-1] == 1 )
      {
      status=chepval (&mid[iv-1],p_mseg[iv-1]);
      if (status<0) 
        {
        return(status);
        }
       }
    else if ( mflag[iv-1] == 2 )
      {
      /* Mid curve */
      }
    else
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 Mid curve flag not Ok mflag[%d]= %d (not 1 or 2)\n",
(int)iv-1, (int)mflag[iv-1]);
}
#endif
        sprintf(errbuf ,"%d %%sur800", (int)mflag[iv-1]);
       varkon_erinit();
        return(varkon_erpush("SU2173",errbuf));
      }


     }  /* End loop */


/*!                                                                 */
/* 2. Find U patch boundaries                                       */
/* __________________________                                       */
/*                                                                  */
/* The output surface will be a "regular" UV patch net. The U       */
/* patch boundaries is defined by the start and end points of       */
/* of the input, longitudinal curve segments.                       */
/* Call of internal function ubounds.                               */
/*                                                                 !*/

    status=ubounds
   (p_spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag);
    if (status<0) 
        {
        sprintf(errbuf,"ubounds%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/* 3. Memory area for the patches                                   */
/* ______________________________                                   */
/*                                                                  */
/* Let nu= n_spine_u-1 and nv= no_strips                            */
/*                                                                 !*/

   nu = n_spine_u - 1;
   nv = no_strips;

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                                         */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,nu*nv))==NULL)/* Dynamic allocation of   */
 {                                       /* memory for patches      */
 sprintf(errbuf,                         /* Error SU2993 for alloc  */
 "(alloc)%%varkon_sur_creloft (sur800"); /* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }                                       /*                         */

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_pat = p_frst;                        /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_pat.                                 */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= nu*nv  ;i_s= i_s+1)
     {
     varkon_ini_gmpat (p_frst  +i_s-1);
     }

pat_no = 0;

/*!                                                                 */
/* 4. Output surface data                                           */
/* _______________________                                          */
/*                                                                  */
/* Header data to p_sur (number of patches nu,nv and type LFT_SUR)  */
/*                                                                 !*/

p_sur->nu_su = (short)nu;                /* Number of patches in U  */
p_sur->nv_su = (short)nv;                /* and V to surface header */
p_sur->typ_su = LFT_SUR;                 /* Output type to header   */

/*!                                                                 */
/* 5. Calculate surface patches                                     */
/* ____________________________                                     */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Start loop U sections (patches) iu= 1,2,...., nu                 */
/*   Start loop V strips   (patches) iv= 1,2,...., nv               */
/*                                                                 !*/

for ( iu =1; iu <=nu ; ++iu )
  {

/*!                                                                 */
/*     Spine planes splane and eplane. Calls of spipla.             */
/*                                                                 !*/

    status=spipla (p_spine,p_spineseg,spine_u[iu-1],splane,splane_p );
    if (status<0) 
        {
        sprintf(errbuf,"(spipla s)%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }


    status=spipla (p_spine,p_spineseg,spine_u[iu],eplane,eplane_p );
    if (status<0) 
        {
        sprintf(errbuf,"(spipla e)%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }


  for ( iv =1; iv <=nv ; ++iv )
    {

/*!                                                                 */
/*       Dynamic allocation of area for one geometrical patch.      */
/*       Call of function DBcreate_patches.                                   */
/*       Current geometric patch address p_cpat.                    */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(LFT_PAT,1)) == NULL ) /* Dynamic allocation of   */
 {                                       /* memory for the patch    */
 sprintf(errbuf,                         /* Error SU2993 for alloc  */
 "(allocg)%%varkon_sur_creloft (sur800");/* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }                                       /*                         */

         p_cpat= (GMPATL *)p_gpat;       /* Current geometric patch */

/*!                                                                 */
/* Initiate patch data in p_cpat for Debug On                       */
/* Call of varkon_ini_gmpatl (sur765).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatl (p_cpat);
#endif

/*!                                                                 */
/*       Call of function DBcreate_patches.                         */
/*       Topological patch data to current patch p_t where          */
/*       p_t= (PATMAT  *)*pp_at + (iu-2)*nv + (iv-1)                */
/*                                                                 !*/

  p_t = p_frst + (iu-1)*nv + (iv-1);     /* Ptr to  current patch   */

  p_t->styp_pat = LFT_PAT;               /* Type of secondary patch */
  p_t->spek_c = p_gpat;                  /* Secondary patch (C ptr) */
  p_t->su_pat = 0;                       /* Topological adress      */
  p_t->sv_pat = 0;                       /* secondary patch not def.*/
  p_t->iu_pat = (short)iu;               /* Topological adress for  */
  p_t->iv_pat = (short)iv;               /* current (this) patch    */
  p_t->us_pat = (DBfloat)iu;             /* Start U on geom. patch  */
  p_t->ue_pat = (DBfloat)iu+1.0-comptol; /* End   U on geom. patch  */
  p_t->vs_pat = (DBfloat)iv;             /* Start V on geom. patch  */
  p_t->ve_pat = (DBfloat)iv+1.0-comptol; /* End   V on geom. patch  */
  p_t->box_pat.xmin = 1.0;               /* BBOX  initiation        */
  p_t->box_pat.ymin = 2.0;               /*                         */
  p_t->box_pat.zmin = 3.0;               /*                         */
  p_t->box_pat.xmax = 4.0;               /*                         */
  p_t->box_pat.ymax = 5.0;               /*                         */
  p_t->box_pat.zmax = 6.0;               /*                         */
  p_t->box_pat.flag =  -1;               /* Not calculated          */
    
  p_t->cone_pat.xdir = 1.0;              /* BCONE initiation        */
  p_t->cone_pat.ydir = 2.0;              /*                         */
  p_t->cone_pat.zdir = 3.0;              /*                         */
  p_t->cone_pat.ang  = 4.0;              /*                         */
  p_t->cone_pat.flag =  -1;              /* Not calculated          */

    
/*!                                                                 */
/*       Create lims, lime, tans, tane. Calls of credir.            */
/*                                                                 !*/

         pat_no = pat_no + 1;

/*       Let pflag be 1 for directrix curves                        */
         pflag = 1;

    status=credir (&lim[iv-1],p_lseg[iv-1],&p_cpat->lims,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"limit%%%d",(int)iv);
        return(varkon_erpush("SU2583",errbuf));
        }

    status=credir (&lim[iv],p_lseg[iv],&p_cpat->lime,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"limit%%%d",(int)iv+1);
        return(varkon_erpush("SU2583",errbuf));
        }

    status=credir (&tan[iv-1],p_tseg[iv-1],&p_cpat->tans,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"tangent%%%d",(int)iv);
        return(varkon_erpush("SU2583",errbuf));
        }

    status=credir (&tan[iv],p_tseg[iv],&p_cpat->tane,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"tangent%%%d",(int)iv+1);
        return(varkon_erpush("SU2583",errbuf));
        }

/*!                                                                 */
/*       Create mid directrix curve segment if mflag(iv-1)= 2 or    */
/*       create P value       curve segment if mflag(iv-1)= 1.      */
/*       Call of credir with pflag= 1 or pflag= 2.                  */
/*                                                                 !*/

    if ( mflag[iv-1] == 2 )
    {
    pflag = 1;
    status=credir (&mid[iv-1],p_mseg[iv-1],&p_cpat->midc,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"mid%%%d",(int)iv);
        return(varkon_erpush("SU2583",errbuf));
        }
    }
    else if ( mflag[iv-1] == 1 )
    {
    pflag = 2;
    status=credir (&mid[iv-1],p_mseg[iv-1],&p_cpat->pval,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"P value%%%d",(int)iv);
        return(varkon_erpush("SU2583",errbuf));
        }
    }
    else if ( mflag[iv-1] == 3 )
    {
    ; /* Rotation, no mid curve is needed */
    }

/*!                                                                 */
/*       Create V limit curves. Call of crevlim.                    */
/*                                                                 !*/

    status=crevlim (p_cpat);
    if (status<0) 
        {
        sprintf(errbuf,"(crevlim)%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/*       Create spine. Call of credir.                              */
/*                                                                 !*/

/*       En on|dig sk{rningsber{kning kommer att g|ras !!           */
/*       Kan fixas med pflag= 9 om man vill .............           */

/*       Let pflag be 1 for the spine curve                         */
         pflag = 9;

    status=credir (p_spine,p_spineseg,&p_cpat->spine,pflag);
    if (status<0) 
        {
        sprintf(errbuf,"(credir spine)%%varkon_sur_creloft (sur800)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/*       P value flag for patch defined by midpflag                 */
/*       Let offset for patch be zero (0).                          */
/*                                                                 !*/

         if      ( mflag[iv-1] == 1 )
             p_cpat->p_flag  = 1;
         else if ( mflag[iv-1] == 2 )
             p_cpat->p_flag  = 2;

         p_cpat->ofs_pat = 0.0; 

/*!                                                                 */
/*       Printout of patch data                     For Debug On    */
/*       Call of varkon_pat_priconloft (sur223).                    */
/*                                                                 !*/

#ifdef DEBUG
if  ( pat_no  <   9 )              /* !!! Tillf{lligt inga patches  */
{
status=varkon_pat_priconloft
    (p_cpat);
    if (status<0) 
       {
       sprintf(errbuf,"sur223%%varkon_sur_creloft (credir)");
       return(varkon_erpush("SU2943",errbuf));
       }
}
#endif


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 p_t %d p_cpat %d pat_no %d iu %d iv %d\n", 
        (int)p_t ,  (int)p_cpat,   (int)pat_no,   (int)iu,   (int)iv);
fflush(dbgfil(SURPAC)); 
}
#endif

    }  /* End loop iv */
  }  /* End loop iu */
/*!                                                                 */
/*   End   loop V strips   (patches) iv= 1,2,...., nv               */
/* End   loop U sections (patches) iu= 1,2,...., nu                 */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 Surface is created. Start loop BBOX calculation\n");
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 6. Calculate bounding boxes and cones                            */
/* ______________________________________                           */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 1;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_sur,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_creloft(sur800)");
        return(varkon_erpush("SU2943",errbuf));
        }

/*                                                                  */
/* 7. Exit                                                          */
/* _______                                                          */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 Exit *** varkon_sur_creloft  nu %d nv %d\n",
                (int)nu,(int)nv);
fflush(dbgfil(SURPAC)); 
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
   (p_spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag,p_sur,pp_pat)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBint    no_strips;    /* Number of surface strips                */
  DBCurve  lim[STRIP];   /* Limit   curves                          */
  DBSeg   *p_lseg[STRIP];/* Limit   curves segments           (ptr) */
  DBCurve  tan[STRIP];   /* Tangent curves                          */
  DBSeg   *p_tseg[STRIP];/* Tangent curves segments           (ptr) */
  DBCurve  mid[STRIP];   /* Mid     curves                          */
  DBSeg   *p_mseg[STRIP];/* Mid     curves segments           (ptr) */
  DBint    mflag[STRIP]; /* Flag for P-value or Mid curve           */
                         /* Eq. 1: P value    Eq. 2: Mid curve      */
  DBSurf  *p_sur;        /* Output surface                    (ptr) */
  DBPatch **pp_pat;       /* Allocated area for patch data     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint  i_strip;       /* Loop index strip in surface             */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/* ==============================                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*initial p_sur %d\n", (int)p_sur );
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial no_strips= %d\n",(int)no_strips);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_spine %d p_spineseg %d (*p_spine).ns_cu %d\n",
                  (int)p_spine,   (int)p_spineseg,   (int)(*p_spine).ns_cu );
  fflush(dbgfil(SURPAC));
  }

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial &lim[0] %d p_lseg[0] %d lim[0].ns_cu %d\n",
                  (int)&lim[0],   (int)p_lseg[0],   (int)lim[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &tan[0] %d p_tseg[0] %d tan[0].ns_cu %d\n",
                  (int)&tan[0],   (int)p_tseg[0],   (int)tan[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &mid[0] %d p_mseg[0] %d mid[0].ns_cu %d\n",
                  (int)&mid[0],   (int)p_mseg[0],   (int)mid[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &lim[1] %d p_lseg[1] %d lim[1].ns_cu %d\n",
                  (int)&lim[1],   (int)p_lseg[1],   (int)lim[1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &tan[1] %d p_tseg[1] %d tan[1].ns_cu %d\n",
                  (int)&tan[1],   (int)p_tseg[1],   (int)tan[1].ns_cu );
  fflush(dbgfil(SURPAC));
  }

if ( no_strips > 1 )
{
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial &mid[1] %d p_mseg[1] %d mid[1].ns_cu %d\n",
                  (int)&mid[1],   (int)p_mseg[1],   (int)mid[1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &lim[2] %d p_lseg[2] %d lim[2].ns_cu %d\n",
                  (int)&lim[2],   (int)p_lseg[2],   (int)lim[2].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial &tan[2] %d p_tseg[2] %d tan[2].ns_cu %d\n",
                  (int)&tan[2],   (int)p_tseg[2],   (int)tan[2].ns_cu );
  fflush(dbgfil(SURPAC)); 
  }
}

/*!                                                                 */
/* Start loop all strips in the surface i_strip=1,2,....,no_strips  */
/*                                                                 !*/

for ( i_strip=1; i_strip<=no_strips; ++i_strip)
  {

/*!                                                                 */
/*   Printout of input curve data                                   */
/*                                                                 !*/
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_lseg %d lim().ns_cu %d            Strip %d\n",
   (int)p_lseg[i_strip-1],   (int)lim[i_strip-1].ns_cu, (int)i_strip );
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_tseg %d tan().ns_cu %d\n",
   (int)p_tseg[i_strip-1],   (int)tan[i_strip-1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_mseg %d mid().ns_cu %d p_flag %d\n",
   (int)p_mseg[i_strip-1],   (int)mid[i_strip-1].ns_cu, (int)mflag[i_strip-1] );
   }
if ( i_strip == no_strips )
{
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_lseg %d lim().ns_cu %d\n",
   (int)p_lseg[i_strip],     (int)lim[i_strip].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur800*initial p_tseg %d tan().ns_cu %d\n",
   (int)p_tseg[i_strip],   (int)tan[i_strip].ns_cu );
  } 
}

    }                                    /* End loop surface strip  */

/*!                                                                 */
/*  End   loop all strips in the surface i_strip=1,2,....,no_strips */
/*                                                                 !*/


/*!                                                                 */
/* 2. Check of input data                                           */
/* ======================                                           */
/*                                                                  */
/* Check that no_strips <= STRIP                                    */
/*                                                                 !*/

   if ( no_strips > STRIP )
     {
     sprintf(errbuf,"no_strips>STRIP%%varkon_sur_creloft (sur800)");
#ifdef LNAME
     return(varkon_erpush("SU2993",errbuf));
#endif
#ifdef SNAME
     return(erpush("SU2993",errbuf));
#endif
     }

/*!                                                                 */
/* 3. Initiate output variables and static (internal) variables     */
/* ============================================================     */
/*                                                                 !*/

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif

/*!********* Internal ** function ** ubounds ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates U patch boundaries for the surface.      */
/* Output is an array of parameter values for the spine.            */

   static short ubounds
   (p_spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBint    no_strips;    /* Number of surface strips                */
  DBCurve  lim[STRIP];   /* Limit   curves                          */
  DBSeg   *p_lseg[STRIP];/* Limit   curves segments           (ptr) */
  DBCurve  tan[STRIP];   /* Tangent curves                          */
  DBSeg   *p_tseg[STRIP];/* Tangent curves segments           (ptr) */
  DBCurve  mid[STRIP];   /* Mid     curves                          */
  DBSeg   *p_mseg[STRIP];/* Mid     curves segments           (ptr) */
  DBint    mflag[STRIP]; /* Flag for P-value or Mid curve           */
                         /* Eq. 1: P value    Eq. 2: Mid curve      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_strip;       /* Loop index strip in surface             */
   DBint  n_all_u;       /* Number of paramter values in all_u()    */
/*----------------------------------------------------------------- */

   DBCurve *p_cur;       /* Pointer to the current curve            */
   DBSeg   *p_startseg;  /* Pointer to the current start segment    */
   DBfloat  all_u[STRIP*SPINE];    
                         /* All parameter values, incl. double pts  */
   DBfloat  all_u_s[STRIP*SPINE];
                         /* All sorted parameter values             */
   DBfloat  min_u;       /* Minimum parameter value                 */
   DBint    i_segm;      /* Loop index segment in curve             */
   DBint    i_compare;   /* Loop index solution                     */
   DBint    i_min_u;     /* Pointer to minimum value                */
   DBfloat  outu[SPINE]; /* Output spine parameters from clopoi     */
   DBint    n_outu;      /* Number of values in array outu          */
   DBint    i_sol;       /* Loop index solution closest point       */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors and      out[9-14]*/
                         /*        curvature added         out[ 15 ]*/

   DBint  comp;          /* Comparison flag                         */

   DBint  status;        /* Error code from a called function       */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ==============                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Find spine planes for all segments in all curves\n");
fprintf(dbgfil(SURPAC),
"sur800*ubounds (Calc. closest pts on spine from all start/end pts)\n");
}
#endif

   rcode   = 3;       /* Only coordinates are actually used ....    */

/*!                                                                 */
/* 2. U boundary values from the spine                              */
/* ====================================                             */
/*                                                                  */
/* Let all_u(i)= i for i=0, 1, 2, ... , (*p_spine).ns_cu            */
/*                                                                 !*/

n_all_u = 0;          /* Initiate number of parameter values        */

for ( i_segm =0; i_segm <= (*p_spine).ns_cu; ++i_segm )
  {
  n_all_u = n_all_u + 1;
  all_u[i_segm] = (DBfloat)i_segm;
  }

/*!                                                                 */
/* 3. U boundary values from the longitudinal curves                */
/* =================================================                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*!                                                                 */
/* Start loop all strips in the surface i_strip=1,2,....,no_strips  */
/*                                                                 !*/

for ( i_strip=1; i_strip<=no_strips; ++i_strip)
  {


/*!                                                                 */
/*   Check mid flag values for Debug On                             */
/*                                                                 !*/
#ifdef DEBUG
    if ( mflag[i_strip-1] == 1 || 
         mflag[i_strip-1] == 2    )
      {
      /* Mid flag values are OK  */
      }
    else
      {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Mid curve flag not Ok mflag[%d]= %d (not 1, 2 or 3)\n",
(int)i_strip-1, (int)mflag[i_strip-1]);
}
       sprintf(errbuf ,"%d %%sur800*ubounds", (int)mflag[i_strip-1]);
       varkon_erinit();
       return(varkon_erpush("SU2173",errbuf));
      }
#endif



/*!                                                                 */
/*   U (spine) values from limit   curve. Call clopoi.              */
/*                                                                 !*/
    p_cur      = &lim[i_strip-1];
    p_startseg = p_lseg[i_strip-1];
    status=clopoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }

if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }


for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Limit     ==>  %d planes Total no %d     Strip %d \n", 
                        (int)n_outu ,  (int)n_all_u , (int)i_strip );
}
#endif

/*!                                                                 */
/*   U (spine) values from tangent curve. Call clopoi.              */
/*                                                                 !*/
    p_cur      = &tan[i_strip-1];
    p_startseg = p_tseg[i_strip-1];
    status=clopoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }

if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }

for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Tangent   ==>  %d planes Total no %d \n",
           (int)n_outu , (int)n_all_u );
}
#endif

/*!                                                                 */
/*   U (spine) values from mid     curve. Call clopoi if mflag=2    */
/*                                                                 !*/

    if ( mflag[i_strip-1] == 2 )
    {
    p_cur      = &mid[i_strip-1];
    p_startseg = p_mseg[i_strip-1];
    status=clopoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }
if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }

for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Mid       ==>  %d planes Total no %d \n",
           (int)n_outu , (int)n_all_u );
}
#endif

    }         /* End mflag = 2         */

/*!                                                                 */
/*   U (spine) values from mid Pval curve. Call arcpoi if mflag=1   */
/*                                                                 !*/

    if ( mflag[i_strip-1] == 1      )
    {
    p_cur      = &mid[i_strip-1];
    p_startseg = p_mseg[i_strip-1];
    status=arcpoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"arcpoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }
if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }

for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Pvalue    ==>  %d planes Total no %d \n",
                 (int)n_outu , (int)n_all_u );
}
#endif

    }         /* End mflag = 1         */

if ( i_strip == no_strips )
{

/*!                                                                 */
/*   End surface curves (i_strip=no_strips):                        */
/*                                                                 !*/

/*!                                                                 */
/*   U (spine) values from limit   curve. Call clopoi.              */
/*                                                                 !*/

    p_cur      = &lim[i_strip];
    p_startseg = p_lseg[i_strip];
    status=clopoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }
if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }

for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Limit     ==>  %d planes Total no %d (last strip)\n", 
                        (int)n_outu , (int)n_all_u );
}
#endif

/*!                                                                 */
/*   U (spine) values from tangent curve. Call clopoi.              */
/*                                                                 !*/

    p_cur      = &tan[i_strip];
    p_startseg = p_tseg[i_strip];
    status=clopoi 
   (p_spine,p_spineseg,p_cur,p_startseg, outu, &n_outu);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_creloft (ubounds)");
        return(varkon_erpush("SU2973",errbuf));
        }
if ( n_all_u+n_outu >= STRIP*SPINE )
   {
   sprintf(errbuf,"n_all_u%%varkon_sur_creloft (ubounds)");
   return(varkon_erpush("SU2993",errbuf));
   }

for ( i_sol=1; i_sol<=n_outu; ++i_sol  )
  {
  all_u[n_all_u] = outu[i_sol-1];
  n_all_u = n_all_u + 1; 
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds Tangent   ==>  %d planes Total no %d (last strip)\n", 
        (int)n_outu , (int)n_all_u );
}
#endif


}                                      /* End if i_strips=no_strips */

    }                                    /* End loop surface strip  */

/*!                                                                 */
/*  End   loop all strips in the surface i_strip=1,2,....,no_strips */
/*                                                                 !*/


/*!                                                                 */
/* 2. Remove multiple points and order parameter values             */
/* ====================================================             */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Parameter values in increasing order (in array all_u_s).         */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*ubounds i_sol %d  all_u(i_sol-1) %f\n",(int)i_sol,all_u[i_sol-1]);
  }
}
#endif

for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  min_u   = all_u[i_sol-1];
  i_min_u = i_sol;
  for ( i_compare=1; i_compare<=n_all_u; ++i_compare  )
    {
    if ( all_u[i_compare-1] <  min_u )
      {
      min_u = all_u[i_compare-1];
      i_min_u = i_compare;
      }
    }
    all_u_s[i_sol-1] = min_u;
    all_u[i_min_u-1] = 50000.0;
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
for ( i_sol=1; i_sol<=n_all_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*ubounds i_sol %d  all_u_s(i_sol-1) %f\n",
         (int)i_sol,all_u_s[i_sol-1]);
  }
}
#endif

/*!                                                                 */
/* Delete equal points. Criterion varkon_ctol * 10.0                */
/*                                                                 !*/

spine_u[0]=all_u_s[0];
n_spine_u = 1;
for ( i_sol=2; i_sol<=n_all_u; ++i_sol  )
  {
   compare(p_spine,p_spineseg, 
     spine_u[n_spine_u-1]+1.0,all_u_s[i_sol-1]+1.0, &comp);     

  if ( comp == 1 )
     {
     if ( n_spine_u + 1   >= SPINE )
        {
        sprintf(errbuf,"n_spine_u= %d >= %d%%sur800 ubounds",
                        (int)n_spine_u+1, SPINE);
        return(varkon_erpush("SU2993",errbuf));
        }
     spine_u[n_spine_u]=all_u_s[i_sol-1];
     n_spine_u = n_spine_u + 1;
     }
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*ubounds All planes defined by spine parameter values:\n");
for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*ubounds Plane %d  spine_u(%2d) %f\n",
         (int)i_sol,(int)i_sol-1,spine_u[i_sol-1]);
  }
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ** clopoi  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the closest points to all start and      */
/* segment points of a directrix curve. Output is an array of       */
/* spine parameter values.                                          */

   static short clopoi 
   (p_spine,p_spineseg, p_cur, p_startseg , outu , p_n_outu )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBCurve *p_cur;        /* Current curve                     (ptr) */
  DBSeg   *p_startseg;   /* Current start segment             (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBfloat  outu[SPINE];  /* Output spine parameter values           */
  DBint   *p_n_outu;     /* Number of values in array outu    (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_segm;        /* Loop index segment in a curve           */
/*----------------------------------------------------------------- */

   DBSeg   *p_seg;       /* Current segment                   (ptr) */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors and      out[9-14]*/
                         /*        curvature added         out[ 15 ]*/
   DBVector extpt;       /* External point                          */
   short    intnr;       /* Defines the output closest point        */
   DBfloat  u_clo;       /* Output (spine) parameter value          */

   short  status;        /* Error code from a called function       */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ==============                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  p_cur= %d  p_startseg %d\n",(int)p_cur , (int)p_startseg );
fflush(dbgfil(SURPAC)); 
}
#endif

   *p_n_outu = 0;     /* Initiate the no of output points           */

   rcode   = 3;       /* Only coordinates are actually used ....    */

/*!                                                                 */
/* 2. Closest points on the spine curve                             */
/* ====================================                             */
/*                                                                 !*/

/*!                                                                 */
/*   Start curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/

  for ( i_segm =1; i_segm <=p_cur->ns_cu; ++i_segm )
    {

    p_seg = p_startseg + i_segm - 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  p_cur %d p_seg %d Segment %d\n",
  (int)p_cur , (int)p_seg,  (int)i_segm );
fflush(dbgfil(SURPAC)); 
}
#endif

    t_l = 0.0;
   status=GE107
       ((GMUNON *)p_cur,p_seg,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_sur_creloft (clopoi )");
        return(varkon_erpush("SU2943",errbuf));
        }

    intnr = 1;
    extpt.x_gm = out[0];
    extpt.y_gm = out[1];
    extpt.z_gm = out[2];

   status=GE703
       ((DBAny *)p_spine,p_spineseg,&extpt,intnr,&u_clo);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  External curve point %8.2f %8.2f %8.2f (no solution)\n",
 out[0] , out[1] , out[2] );
}
#endif
        goto  nosol;
        }

   if ( *p_n_outu >=  SPINE )
      {
      sprintf(errbuf,"*p_n_outu%%varkon_sur_creloft (clopoi)");
      return(varkon_erpush("SU2993",errbuf));
      }

   outu[*p_n_outu] = u_clo;
   *p_n_outu       = *p_n_outu + 1; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  External curve point %8.2f %8.2f %8.2f u_clo %f\n",
 out[0] , out[1] , out[2] , u_clo );
fflush(dbgfil(SURPAC)); 
}
#endif

nosol:;    /* No solution */
  if ( i_segm < p_cur->ns_cu ) goto nxtseg;

    t_l = 1.0;
   status=GE107
       ((GMUNON *)p_cur,p_seg,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_sur_creloft (clopoi )");
        return(erpush("SU2943",errbuf));
        }

    intnr = 1;
    extpt.x_gm = out[0];
    extpt.y_gm = out[1];
    extpt.z_gm = out[2];

   status=GE703
       ((DBAny *)p_spine,p_spineseg,&extpt,intnr,&u_clo);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  External curve point %8.2f %8.2f %8.2f (no solution)\n",
 out[0] , out[1] , out[2] );
}
#endif
        goto  nxtseg;
        }

   if ( *p_n_outu >=  SPINE )
      {
      sprintf(errbuf,"*p_n_outu%%varkon_sur_creloft (clopoi)");
      return(varkon_erpush("SU2993",errbuf));
      }

   outu[*p_n_outu] = u_clo;
   *p_n_outu       = *p_n_outu + 1; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*clopoi  External curve point %8.2f %8.2f %8.2f u_clo %f\n",
 out[0] , out[1] , out[2] , u_clo );
fflush(dbgfil(SURPAC)); 
}
#endif

nxtseg:;  /* Label: Not the last segment                            */

    }  /* End loop                                                  */


/*!                                                                 */
/*   End   curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ** spipla *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate the spine plane                                        */

   static short spipla (p_spine,p_spineseg,u_val,plane , plane_p )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBfloat  u_val;        /* Patch (local) U parameter value         */
  DBfloat  plane[4];     /* Spine plane for directrix               */
  DBfloat  plane_p[4];   /* Spine plane for P-value function        */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz;         /* Coordinates and derivatives for crv pt  */
/*-----------------------------------------------------------------!*/
   DBTmat  *p_csys;      /* Coordinate system                 (ptr) */
   DBfloat  uglobs;      /* Global u start value                    */
   DBfloat  uglobe;      /* Global u end   value                    */
   DBfloat  rleng;       /* Relative arclength                      */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur800*spipla Parameter U= %f \n" ,u_val);
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* 2. Spine point                                                   */
/* ______________                                                   */
/*                                                                  */
/* Calculate point on spline for the given u_val value              */
/* Call of varkon_GE109 (GE109).                                  */
/*                                                                 !*/

   xyz.evltyp   = EVC_DR;
   xyz.t_global = u_val + 1.0; /* u_val borde vara med + 1.0 */

   status=GE109
   ((DBAny *)p_spine,p_spineseg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }




#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*spipla Spine point   %f %f %f\n",
  xyz.r.x_gm,xyz.r.y_gm,xyz.r.z_gm );
fprintf(dbgfil(SURPAC),
"sur800*spipla Spine tangent %f %f %f \n",
  xyz.drdt.x_gm,xyz.drdt.y_gm,xyz.drdt.z_gm );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Spine plane for directrix                                     */
/* ____________________________                                     */
/*                                                                  */
/* Let the spine plane normal be the normalized tangent             */
/* Call of varkon_normv (normv).                                    */
/* Let the plane coefficient d= scalar product of the spine         */
/* plane normal and (the vector origin to) the spine point          */
/*                                                                 !*/

   plane[0] = xyz.drdt.x_gm;
   plane[1] = xyz.drdt.y_gm;
   plane[2] = xyz.drdt.z_gm;

   status=varkon_normv(plane);
   if (status<0) 
        {
        sprintf(errbuf,"normv%%varkon_sur_creloft (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }

   plane[3] = plane[0]*xyz.r.x_gm+
              plane[1]*xyz.r.y_gm+
              plane[2]*xyz.r.z_gm;


/*!                                                                 */
/* 4. Spine plane for 2D functions                                  */
/* _______________________________                                  */
/*                                                                  */
/* Let the spine plane normal for plane_p be (1,0,0) and the        */
/* plane coefficient d= spine arclength from U= 0 to U= u_val       */
/* Call of varkon_GE718 (GE718).                                  */
/*                                                                 !*/

   plane_p[0] = 1.0;   
   plane_p[1] = 0.0;   
   plane_p[2] = 0.0;   

   p_csys = NULL;
   uglobs = 1.0;
   uglobe = u_val + 1.0; /* Borde redan vara +1.0 */

   status=GE718
   ((DBAny *)p_spine,p_spineseg,p_csys,uglobs,uglobe,&rleng);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*spipla GE718 failed rleng %20.15f \n", rleng  );
  fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE718%%varkon_sur_creloft (spipla)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*spipla uglobs %f uglobe %f rleng %f \n", uglobs ,  uglobe ,  rleng  );
  fflush(dbgfil(SURPAC));
}
#endif

   plane_p[3] = rleng*p_spine->al_cu;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*spipla nx %7.4f ny %7.4f nz %7.4f d %f Pval d %f\n",
        plane[0], plane[1], plane[2], plane[3], plane_p[3] );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */


/********************************************************************/

/*!********* Internal ** function ** credir *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create patch directrix segment or P-value curve segment.         */
/*                                                                  */
   static short credir (p_cur,p_startseg,p_segpat,pflag)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBCurve *p_cur;        /* Current curve                     (ptr) */
  DBSeg   *p_startseg;   /* Current start segment             (ptr) */
  DBSeg   *p_segpat;     /* Output  curve segment of patch    (ptr) */
  DBint    pflag;        /* P value flag.                           */
                         /* Eq. 1: Directrix   Eq. 2: P-value       */
                         /*                    Eq. 9: Spine         */
/*                                                                  */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  s_u;         /* Curve parameter for splane intersect    */
   DBfloat  e_u;         /* Curve parameter for eplane intersect    */
   DBfloat  us_in;       /* Parameter value for input  start point  */
   DBfloat  ue_in;       /* Parameter value for input  end   point  */
   DBSeg   *p_cseg;      /* Current curve segment            (ptr)  */
/*-----------------------------------------------------------------!*/
   DBfloat  mod_tol4;    /* Scale factor for tolerance TOL4         */
   DBTmat   tra;         /* Translation matrix                      */
   DBfloat  intplane[4]; /* Intersect plane                         */
   DBint    n_int;       /* Number of intersect points              */
   DBfloat  uout[INTMAX];/* Ordered array of u solutions 1,., n_int */
   DBint    i_seg;       /* Current curve segment                   */
   DBint    i_seg_s_u;   /* Current curve segment for s_u           */
   DBint    i_seg_e_u;   /* Current curve segment for e_u           */
   DBfloat  us_out;      /* Parameter value for output start point  */
   DBfloat  ue_out;      /* Parameter value for output end   point  */
   char     errbuf[80];  /* String 1 for error message fctn erpush  */
   char     errbuf2[80]; /* String 2 for error message fctn erpush  */
   short    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir p_cur %d p_startseg %d p_segpat %d \n"
     , (int)p_cur , (int)p_startseg , (int)p_segpat );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
   mod_tol4 = 100.0;
/*                                                                 !*/

/*!                                                                 */
/* 1. Parameter values for splane and eplane intersect points       */
/* __________________________________________________________       */
/*                                                                  */
/* Calculate intersection points between the spine plane and the    */
/* longitudinal 3D curve, i.e. parameters values s_u and e_u.       */
/* Calls of varkon_cur_intplan (sur720) with planes defined         */
/* by pflag: splane and eplane or splane_p and eplane_p.            */
/*                                                                 !*/

   if       ( pflag == 1 || pflag == 9 )
      {
      intplane[0] = splane[0];
      intplane[1] = splane[1];
      intplane[2] = splane[2];
      intplane[3] = splane[3];
      }
   else if  ( pflag == 2 )
      {
      intplane[0] = splane_p[0];
      intplane[1] = splane_p[1];
      intplane[2] = splane_p[2];
      intplane[3] = splane_p[3];
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && pflag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Before sur720 (1) p_cur %d p_startseg %d pflag %d\n",
                                 (int)p_cur  , (int)p_startseg , (int)pflag );
fflush(dbgfil(SURPAC)); 
}
#endif

   status = varkon_cur_intplan
   (p_cur,p_startseg,intplane,&n_int,uout);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Intersection calculation (splane) failed n_int= %d \n",
(int)n_int);
}
#endif
        sprintf(errbuf ,"%7.3f %7.3f %7.3f%%%10.2f",
           intplane[0],intplane[1],intplane[2],intplane[3]);
        sprintf(errbuf2,"%d%%",(int)n_int);
       varkon_erinit();
        varkon_erpush("SU2573",errbuf);
        return(varkon_erpush("SU2593",errbuf2));
        }




#ifdef DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur800*credir After  sur720 (1) p_cur %d p_startseg %d\n",
                                 (int)p_cur  , (int)p_startseg );
fflush(dbgfil(SURPAC)); 
}
#endif

   if ( n_int == 1 )
     {
     s_u = uout[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Intersection calculation failed n_int= %d \n",(int)n_int);
}
#endif
        sprintf(errbuf ,"%7.3f %7.3f %7.3f%%%10.2f",
           intplane[0],intplane[1],intplane[2],intplane[3]);
        sprintf(errbuf2,"%d%%",(int)n_int);
       varkon_erinit();
     varkon_erpush("SU2573",errbuf);
     return(varkon_erpush("SU2593",errbuf2));
     }

   if       ( pflag == 1 || pflag == 9 )
      {
      intplane[0] = eplane[0];
      intplane[1] = eplane[1];
      intplane[2] = eplane[2];
      intplane[3] = eplane[3];
      }
   else if  ( pflag == 2 )
      {
      intplane[0] = eplane_p[0];
      intplane[1] = eplane_p[1];
      intplane[2] = eplane_p[2];
      intplane[3] = eplane_p[3];
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Before sur720 (2) p_cur %d p_startseg %d\n",
                                 (int)p_cur  , (int)p_startseg );
fflush(dbgfil(SURPAC)); 
}
#endif

   status = varkon_cur_intplan
         (p_cur,p_startseg,intplane,&n_int,uout);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Intersection calculation (eplane) failed n_int= %d \n",
(int)n_int);
}
#endif
        sprintf(errbuf ,"%7.3f %7.3f %7.3f%%%10.2f",
           intplane[0],intplane[1],intplane[2],intplane[3]);
        sprintf(errbuf2,"%d%%",(int)n_int);
       varkon_erinit();
        varkon_erpush("SU2573",errbuf);
        return(varkon_erpush("SU2593",errbuf2));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir After  sur720 (2) p_cur %d p_startseg %d\n",
                                 (int)p_cur  , (int)p_startseg );
fflush(dbgfil(SURPAC)); 
}
#endif

   if ( n_int == 1 )
     {
     e_u = uout[0];
     }
   else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Intersection calculation failed n_int= %d \n",(int)n_int);
}
#endif
        sprintf(errbuf ,"%7.3f %7.3f %7.3f%%%10.2f",
           intplane[0],intplane[1],intplane[2],intplane[3]);
        sprintf(errbuf2,"%d%%",(int)n_int);
       varkon_erinit();
        varkon_erpush("SU2573",errbuf);
        return(varkon_erpush("SU2593",errbuf2));
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir splane intersect calculation s_u %f e_u %f\n", 
              s_u , e_u );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 2. Calculate relimited curve segment                             */
/* ____________________________________                             */
/*                                                                  */
/* Calculate curve segment adress p_cseg and parameter              */
/* limitation values us_in and ue_in for p_cseg.                    */
/* Relimit curve segment and create patch curve segment p_segpat.   */
/* Call of varkon_cur_reparam (sur790).                             */
/*                                                                 !*/

   i_seg_s_u= (DBint)floor(s_u+mod_tol4*TOL4);
   if ( i_seg_s_u == 0 ) i_seg_s_u = 1;
   if ( i_seg_s_u == p_cur->ns_cu+1 )i_seg_s_u= p_cur->ns_cu;

   i_seg_e_u= (DBint)floor(e_u+mod_tol4*TOL4);
   if ( i_seg_e_u == 0 ) i_seg_e_u = 1;
   if ( i_seg_e_u == p_cur->ns_cu+1 )i_seg_e_u= p_cur->ns_cu;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && pflag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir i_seg_s_u %d i_seg_e_u %d \n", 
              (int)i_seg_s_u , (int)i_seg_e_u );
fprintf(dbgfil(SURPAC),
"sur800*credir splane intersect calculation s_u %f e_u %f\n", 
              s_u , e_u );
fprintf(dbgfil(SURPAC),
"sur800*credir Differences %f  %f \n", 
          fabs(s_u-(DBfloat)i_seg_s_u) ,
          fabs(e_u-(DBfloat)i_seg_e_u)    ); 
fflush(dbgfil(SURPAC)); 
}
#endif

   if      ( i_seg_s_u == i_seg_e_u )
     {
     i_seg= i_seg_e_u;
     p_cseg= p_startseg+i_seg-1;
     us_in= s_u - (DBfloat)i_seg;
     ue_in= e_u - (DBfloat)i_seg;
     }
   else if ( i_seg_s_u == i_seg_e_u - 1 )
     {
     if ( fabs(s_u-(DBfloat)i_seg_s_u) < mod_tol4*TOL4  &&
          fabs(e_u-(DBfloat)i_seg_e_u) < mod_tol4*TOL4      )
        {
        i_seg= i_seg_s_u;
        p_cseg= p_startseg+i_seg-1;
        us_in= s_u - (DBfloat)i_seg;
        ue_in= e_u - (DBfloat)i_seg;
        }
     else if ( fabs(e_u-(DBfloat)i_seg_e_u) < mod_tol4*TOL4  )
        {
        i_seg= i_seg_s_u;
        p_cseg= p_startseg+i_seg-1;
        us_in= s_u - (DBfloat)i_seg;
        ue_in= e_u - (DBfloat)i_seg;
        }
     else
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Not the same segment (1): i_seg_s_u %d i_seg_e_u %d \n", 
              (int)i_seg_s_u , (int)i_seg_e_u );
fprintf(dbgfil(SURPAC),
"sur800*credir s_u %f e_u %f TOL4 %f mod_tol4*TOL4 %f\n", 
              s_u , e_u , TOL4, mod_tol4*TOL4);
fprintf(dbgfil(SURPAC),
"sur800*credir Differences %f  %f \n", 
          fabs(s_u-(DBfloat)i_seg_s_u) ,
          fabs(e_u-(DBfloat)i_seg_e_u)    ); 
fflush(dbgfil(SURPAC)); 
}
#endif
       sprintf(errbuf,"(not same segm. 1)%%varkon_sur_creloft");
       return(varkon_erpush("SU2993",errbuf));
       }
     }
   else if ( i_seg_s_u == i_seg_e_u + 1 )
     {
     if ( fabs(s_u-(DBfloat)i_seg_s_u) < mod_tol4*TOL4  &&
          fabs(e_u-(DBfloat)i_seg_e_u) < mod_tol4*TOL4      )
        {
        i_seg= i_seg_e_u;
        p_cseg= p_startseg+i_seg-1;
        us_in= s_u - (DBfloat)i_seg;
        ue_in= e_u - (DBfloat)i_seg;
        }
     else
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Not the same segment (2): i_seg_s_u %d i_seg_e_u %d \n", 
              (int)i_seg_s_u , (int)i_seg_e_u );
fprintf(dbgfil(SURPAC),
"sur800*credir s_u %f e_u %f TOL4 %f mod_tol4*TOL4 %f\n", 
              s_u , e_u , TOL4, mod_tol4*TOL4);
fprintf(dbgfil(SURPAC),
"sur800*credir Differences %f  %f \n", 
          fabs(s_u-(DBfloat)i_seg_s_u) ,
          fabs(e_u-(DBfloat)i_seg_e_u)    ); 
fflush(dbgfil(SURPAC)); 
}
#endif
       sprintf(errbuf,"(not same segm. 2)%%varkon_sur_creloft");
       return(varkon_erpush("SU2993",errbuf));
       }
     }
   else
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Not the same segment (3): i_seg_s_u %d i_seg_e_u %d \n", 
              (int)i_seg_s_u , (int)i_seg_e_u );
fprintf(dbgfil(SURPAC),
"sur800*credir s_u %f e_u %f TOL4 %f mod_tol4*TOL4 %f\n", 
              s_u , e_u , TOL4, mod_tol4*TOL4);
fprintf(dbgfil(SURPAC),
"sur800*credir Differences %f  %f \n", 
          fabs(s_u-(DBfloat)i_seg_s_u) ,
          fabs(e_u-(DBfloat)i_seg_e_u)    ); 
fflush(dbgfil(SURPAC)); 
}
#endif
       sprintf(errbuf,"(not same segm. 3)%%varkon_sur_creloft");
       return(varkon_erpush("SU2993",errbuf));
       }

   us_out =  0.0;
   ue_out =  1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Reparameterize us_in %f ue_in %f (sur790)\n",us_in,ue_in ); 
}
if ( dbglev(SURPAC) == 1 )
{
if ( p_cseg->typ == CUB_SEG )
fprintf(dbgfil(SURPAC),
"sur800*credir CUB_SEG p_cseg->typ %d p_cseg->ofs %f\n"
     , (int)p_cseg->typ , p_cseg->ofs );
if ( p_cseg->typ == UV_CUB_SEG )
   {
fprintf(dbgfil(SURPAC),
"sur800*credir UV_CUB_SEG p_cseg->typ %d p_cseg->ofs %f\n"
     , (int)p_cseg->typ , p_cseg->ofs );
fprintf(dbgfil(SURPAC),
"sur800*credir UV_CUB_SEG p_cseg->subtyp %d p_cseg->spek_gm %d spek2_gm %d nxt_seg %d\n"
     , (int)p_cseg->subtyp , (int)p_cseg->spek_gm, (int)p_cseg->spek2_gm, (int)p_cseg->nxt_seg );
   }
}
#endif

  if ( fabs(p_cseg->ofs) > 0.00000001 )
     {
     sprintf(errbuf,"%%varkon_sur_creloft");
     varkon_erinit();
     return(varkon_erpush("SU2413",errbuf));
     }

  if ( p_cseg->typ == UV_CUB_SEG )  /* Tas bort n{r Johan inf|rt nytt i sur214 */
     {
     sprintf(errbuf,"%%varkon_sur_creloft");
     varkon_erinit();
     return(varkon_erpush("SU2403",errbuf));
     }


  if ( p_cseg->typ == UV_CUB_SEG ) 
     {
     p_segpat->c0x = p_cseg->c0x;
     p_segpat->c0y = p_cseg->c0y;
     p_segpat->c0z = p_cseg->c0z;
     p_segpat->c0  = p_cseg->c0 ;

     p_segpat->c1x = p_cseg->c1x;
     p_segpat->c1y = p_cseg->c1y;
     p_segpat->c1z = p_cseg->c1z;
     p_segpat->c1  = p_cseg->c1 ;

     p_segpat->c2x = p_cseg->c2x;
     p_segpat->c2y = p_cseg->c2y;
     p_segpat->c2z = p_cseg->c2z;
     p_segpat->c2  = p_cseg->c2 ;

     p_segpat->c3x = p_cseg->c3x;
     p_segpat->c3y = p_cseg->c3y;
     p_segpat->c3z = p_cseg->c3z;
     p_segpat->c3  = p_cseg->c3 ;

     p_segpat->ofs = p_cseg->ofs;

     p_segpat->typ = p_cseg->typ;
  
     p_segpat->sl  = p_cseg->sl;
 
     p_segpat->subtyp   = p_cseg->subtyp;
     p_segpat->spek_gm  = p_cseg->spek_gm;
     p_segpat->spek2_gm = p_cseg->spek2_gm;
     p_segpat->nxt_seg  = p_cseg->nxt_seg;

     goto noreparam;
     }


status=varkon_cur_reparam
    (p_cseg,us_in,ue_in,us_out,ue_out,p_segpat);
    if (status<0) 
       {
       sprintf(errbuf,"sur790%%varkon_sur_creloft (credir)");
       return(varkon_erpush("SU2943",errbuf));
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir Curve segment %d    From u= %7.4f to u= %7.4f \n" 
              ,     (int)i_seg , us_in , ue_in );
}
if ( dbglev(SURPAC) == 1 &&  pflag == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*credir s_u %8.4f e_u %8.4f i_seg %d us_in %7.4f ue_in %7.4f \n" 
              , s_u,    e_u ,     (int)i_seg , us_in , ue_in );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Translate the P value curve with d= splane_p(3)                  */
/* (P value curve must correspond to the spine                      */
/*  segments arclength and start with zero    )                     */
/*                                                                 !*/

   if       ( pflag == 2 )
      {
      tra.g11 = 1.0;
      tra.g12 = 0.0;
      tra.g13 = 0.0;
      tra.g14 = -splane_p[3];
      tra.g21 = 0.0;
      tra.g22 = 1.0;
      tra.g23 = 0.0;
      tra.g24 = 0.0;
      tra.g31 = 0.0;
      tra.g32 = 0.0;
      tra.g33 = 1.0;
      tra.g34 = 0.0;
      tra.g41 = 0.0;
      tra.g42 = 0.0;
      tra.g43 = 0.0;
      tra.g44 = 1.0;
      status  = GEtfseg_to_local
      (p_segpat,&tra,p_segpat);
      if (status<0) 
       {
       sprintf(errbuf,"GEtfseg_to_local%%varkon_sur_creloft (credir)");
       return(varkon_erpush("SU2943",errbuf));
       }
      }

/*!                                                                 */
/* Extrapolate all segments except the spine (pflag=9).             */
/* Call of varkon_cur_reparam (sur790).                             */
/*                                                                 !*/

   if ( pflag != 9 )
     {
     us_in  = -0.01;
     ue_in  =  1.01;
     us_out =  0.0;
     ue_out =  1.0;

status=varkon_cur_reparam
    (p_segpat,us_in,ue_in,us_out,ue_out,p_segpat);
    if (status<0) 
       {
       sprintf(errbuf,"sur790%%varkon_sur_creloft (credir)");
       return(varkon_erpush("SU2943",errbuf));
       }
    }

noreparam:  /* No reparametrization for a UV segment */

    return(SUCCED);

} /* End of function                                                */


/********************************************************************/


/*!********* Internal ** function ** crevlim ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the V start and V end line                  */

   static short crevlim(p_segpat)
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  GMPATL  *p_segpat;     /* Output  curve segment of patch    (ptr) */

/* Out:                                                             */
/*       Line data to p_segpat->vs and p_segpat->ve                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector p1;            /* Line start point                        */
   DBVector p2;            /* Line end   point                        */
/*-----------------------------------------------------------------!*/

   short  status;        /* Error code from a called fctn           */
   char   errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur800*crevlim iu= %d iv= %d &p_segpat->vs %d &p_segpat->ve %d \n"
   ,(int)iu,(int)iv,(int)&p_segpat->vs, (int)&p_segpat->ve);
  }
#endif

/*!                                                                 */
/* 1. Create line  segment vs                                       */

/*    Segment data to local variable p1 and p2                      */
/*                                                                 !*/

    p1.x_gm  = (DBfloat)iu  - 1.0;                /* Start point    */
    p1.y_gm  = (DBfloat)iu  + 1.0;                /*                */
    p1.z_gm  =   0.0;                             /*                */
    p2.x_gm  = (DBfloat)iu ;                      /* End   point    */
    p2.y_gm  = (DBfloat)iv  + 1.0;                /*                */
    p2.z_gm  =   0.0;                             /*                */

/*!                                                                 */
/*    Call of GE200.                                                */
/*                                                                 !*/

    status=GE200
    (&p1 , &p2 , &p_segpat->vs ); 
    if (status<0) 
       {
       sprintf(errbuf,"GE200(vs)%%varkon_sur_creloft");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* 2. Create line  segment ve                                       */

/*    Segment data to local variable p1 and p2                      */
/*                                                                 !*/

    p1.x_gm  =(DBfloat)iu  - 1.0;                 /* Start point    */
    p1.y_gm  =(DBfloat)iv  + 2.0;                 /*                */
    p1.z_gm  =   0.0;                             /*                */
    p2.x_gm  =(DBfloat)iu ;                       /* End   point    */
    p2.y_gm  =(DBfloat)iv  + 2.0;                 /*                */
    p2.z_gm  =   0.0;                             /*                */

/*!                                                                 */
/*    Call of GE200.                                                */
/*                                                                 !*/

    status=GE200
    (&p1 , &p2 , &p_segpat->ve ); 
    if (status<0) 
       {
       sprintf(errbuf,"GE200(ve)%%varkon_sur_creloft");
       return(varkon_erpush("SU2943",errbuf));
       }

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

/*!********* Internal ** function ** arcpoi  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the points (U values) on the spine       */
/* curve corresponding to all the segment start/end points of       */
/* the input P-value curve.                                         */
/* Output is an array of spine parameter values.                    */

   static short arcpoi 
   (p_spine,p_spineseg, p_cur, p_startseg , outu , p_n_outu )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBCurve *p_cur;        /* Current P-value curve             (ptr) */
  DBSeg   *p_startseg;   /* Current start segment             (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBfloat  outu[SPINE];  /* Output spine parameter values           */
  DBint   *p_n_outu;     /* Number of values in array outu    (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_segm;        /* Loop index segment in a curve           */
/*----------------------------------------------------------------- */
   EVALC    xyz;         /* Coordinates and derivatives for crv pt  */
   DBfloat  u_global;    /* Global curve parameter value            */

   DBfloat  rel_leng;    /* Relative arclength                      */

   short   status;       /* Error code from a called function       */

   char    errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBint   i_sol;        /* Temporarely used loop index             */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ==============                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*arcpoi  p_cur= %d  p_startseg %d p_cur->ns_cu %d\n",
        (int)p_cur , (int)p_startseg,  (int)p_cur->ns_cu );
}
#endif

   *p_n_outu = 0;     /* Initiate the no of output points           */

/*!                                                                 */
/* 2. Corresponding arclength points on the spine curve             */
/* ====================================================             */
/*                                                                 !*/

/*!                                                                 */
/*   Start curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/

  for ( i_segm =1; i_segm <=p_cur->ns_cu; ++i_segm )
    {

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*arcpoi  p_cur %d Segment %d\n",
  (int)p_cur , (int)i_segm );
fflush(dbgfil(SURPAC)); 
}
#endif

   xyz.evltyp   = EVC_R;
   xyz.t_global = (DBfloat)i_segm;

   status=GE109
   ((DBAny *)p_cur,p_startseg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (arcpoi)");
        return(erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur800*arcpoi P-value point %f %f %f\n",
  xyz.r.x_gm, xyz.r.y_gm, xyz.r.z_gm );
fflush(dbgfil(SURPAC));
}
#endif

   
   if ( p_spine->al_cu  < 1000.0*ctol )
      {
      sprintf(errbuf," %f %% %f ",p_spine->al_cu,1000.0*ctol);
       varkon_erinit();
      return(erpush("SU2603",errbuf));
      }

   rel_leng = xyz.r.x_gm/p_spine->al_cu;


if ( rel_leng >= 0.0  &&  rel_leng <= 1.0 )
   {
   status=GE717
   ((DBAny *)p_spine,p_spineseg, NULL ,rel_leng, &u_global );
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (arcpoi)");
        return(varkon_erpush("SU2943",errbuf));
        }
   outu[*p_n_outu]= u_global - 1.0;  /* Ta bort - 1.0 !!!! */
   *p_n_outu = *p_n_outu + 1;
   } /* End rel_leng <= 1.0  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*arcpoi rel_leng %f u_global %f\n",
  rel_leng ,  u_global );
fflush(dbgfil(SURPAC)); 
}
#endif



    if ( i_segm < p_cur->ns_cu )  goto  nxt;

   xyz.evltyp   = EVC_R;
   xyz.t_global = (DBfloat)i_segm + 1.0;

   status=GE109 ((DBAny *)p_cur,p_startseg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (arcpoi)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*arcpoi P-value point %f %f %f (end of curve)\n",
  xyz.r.x_gm, xyz.r.y_gm, xyz.r.z_gm );
fflush(dbgfil(SURPAC));
}
#endif


   rel_leng= 0.123456789;
   if ( fabs( p_spine->al_cu ) > TOL1 )
   rel_leng = xyz.r.x_gm/p_spine->al_cu;

if ( rel_leng >= 0.0  &&  rel_leng <= 1.0 )
   {
   status=GE717
   ((DBAny *)p_spine,p_spineseg, NULL ,rel_leng, &u_global );
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (arcpoi)");
        return(varkon_erpush("SU2943",errbuf));
        }
   outu[*p_n_outu]= u_global - 1.0;  /* Ta bort - 1.0 !!!! */
   *p_n_outu = *p_n_outu + 1;
   } /* End rel_leng <= 1.0  */


nxt:; /* Label: Not the last segment  */

    }  /* End loop                                                  */


/*!                                                                 */
/*   End   curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
  for ( i_sol  =1; i_sol <= *p_n_outu; ++i_sol  )
    {
    fprintf(dbgfil(SURPAC),
    "sur800*arcpoi i_sol %d  outu() %f\n",
          (int)i_sol,outu[i_sol-1] );
    }
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*!********* Internal ** function ** compare ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Compare two points with the identical points tolerance ctol      */

   static short compare(p_cur ,p_seg, u_glob1, u_glob2, p_comp )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Curve                             (ptr) */
  DBSeg   *p_seg;        /* Curve segments                    (ptr) */
  DBfloat  u_glob1;      /* Global parameter value point 1          */
  DBfloat  u_glob2;      /* Global parameter value point 2          */
  DBint   *p_comp;       /* Result flag:                            */
                         /* Eq. -1: Identical points                */
                         /* Eq. +1: Not identical points            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC    xyz1;        /* Coordinates and derivatives for pt 1    */
   EVALC    xyz2;        /* Coordinates and derivatives for pt 2    */
/*-----------------------------------------------------------------!*/
   DBfloat  dist;        /* Distance                                */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur800*compare parameter U= %f and U= %f Difference %f\n" ,
           u_glob1, u_glob2, u_glob2-u_glob1  );
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* 2. Calculate points                                              */
/* ___________________                                              */
/*                                                                  */
/* Calculate point 1 and 2 for u_glob1 and u_glob2.                 */
/* Calls of varkon_GE109 (GE109).                                 */
/*                                                                 !*/

   xyz1.evltyp   = EVC_DR;

   xyz1.t_global = u_glob1;

   status=GE109 ((DBAny *)p_cur ,p_seg, &xyz1);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (compare");
        return(varkon_erpush("SU2943",errbuf));
        }

   xyz2.evltyp   = EVC_DR;

   xyz2.t_global = u_glob2;

   status=GE109 ((DBAny *)p_cur ,p_seg, &xyz2);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_creloft (compare)");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 3. Calculate distance                                            */
/* _____________________                                            */
/*                                                                  */
/*                                                                 !*/
   dist=SQRT((xyz1.r.x_gm-xyz2.r.x_gm)*(xyz1.r.x_gm-xyz2.r.x_gm)+ 
             (xyz1.r.y_gm-xyz2.r.y_gm)*(xyz1.r.y_gm-xyz2.r.y_gm)+ 
             (xyz1.r.z_gm-xyz2.r.z_gm)*(xyz1.r.z_gm-xyz2.r.z_gm));

/*!                                                                 */
/* 4. Determine if points are equal                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                 !*/
   if   ( dist <= 400.0*ctol ) *p_comp = -1;
   else                      *p_comp =  1;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && *p_comp == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*compare dist= %f 400*ctol= %f *p_comp= %d u_glob2 %f\n", 
           dist, 400.0*ctol , (int)*p_comp , u_glob2 );
  fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */


/********************************************************************/




/*!********* Internal ** function ** chepval  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks that the P value functions is between        */
/* values 0.05 and 0.85.                                            */

   static short chepval 
   (p_cur, p_startseg )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_cur;        /* Current curve                     (ptr) */
  DBSeg   *p_startseg;   /* Current start segment             (ptr) */

/* Out:                                                             */
/*           Exit with error if P value not OK                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_segm;        /* Loop index segment in a curve           */
   DBint  i_poi ;        /* Loop index point in a segment           */
/*----------------------------------------------------------------- */

   DBfloat  p_max;       /* Maximum P value                         */
   DBfloat  p_min;       /* Minimum P value                         */
   DBSeg   *p_seg;       /* Current segment                   (ptr) */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors and      out[9-14]*/
                         /*        curvature added         out[ 15 ]*/
   DBint  status;        /* Error code from a called function       */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ==============                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*chepval  p_cur= %d  p_startseg %d p_cur->ns_cu %d\n",
                 (int)p_cur ,    (int)p_startseg ,  (int)p_cur->ns_cu );
}
#endif

   rcode   = 3;       /* Only coordinates are actually used ....    */

  p_max = -1.0;
  p_min =  2.0;

/*!                                                                 */
/* 2. Check P values ( the Y coordinate )                           */
/* ====================================                             */
/*                                                                 !*/

/*!                                                                 */
/*   Start curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/

  for ( i_segm =1; i_segm <= p_cur->ns_cu; ++i_segm )
    {

    p_seg = p_startseg + i_segm - 1;

    for ( i_poi  =1; i_poi  <= 6; ++i_poi  )
      {

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*chepval  p_cur %d p_seg %d Segment %d i_poi %d p_cur->ns_cu %d\n",
  (int)p_cur , (int)p_seg,  (int)i_segm,  (int)i_poi ,  (int)p_cur->ns_cu );
fflush(dbgfil(SURPAC)); 
}
#endif

    t_l = 0.0 + (i_poi-1.0)*0.2;

   status=GE107
       ((GMUNON *)p_cur,p_seg,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_sur_creloft (chepval )");
        return(varkon_erpush("SU2943",errbuf));
        }

        if ( out[1] < p_min ) p_min = out[1];
        if ( out[1] > p_max ) p_max = out[1];


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800*chepval Segment %d i_poi %d Pvalue %f p_min %f p_max %f\n",
  (int)i_segm,  (int)i_poi,out[1], p_min, p_max );
fflush(dbgfil(SURPAC)); 
}
#endif

        } /* End loop point in segment */
    }  /* End loop                                                  */

/*!                                                                 */
/*     End   point loop     i_poi =1,2,..5                          */
/*   End   curve loop       i_segm=1,2,..,p_cur->ns_cu              */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur800 chepval Minium P value %f  Maximum P value %f\n", 
        p_min, p_max );
fflush(dbgfil(SURPAC));
}
#endif

    if ( p_min < 0.05 )
      {
      sprintf(errbuf,"%f < 0.05 %%",p_min);
       varkon_erinit();
      return(varkon_erpush("SU2473",errbuf));
      }


    if ( p_max > 0.85 )
      {
      sprintf(errbuf,"%f > 0.85 %%",p_max);
      varkon_erinit();
      return(varkon_erpush("SU2473",errbuf));
      }


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
