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
/*  Function: varkon_sur_rotloft                   File: sur851.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*                                                                  */
/*  The function creates a conic lofting rotation surface of type   */
/*  LOFT_SUR, i.e. a surface with patches of type LFT_PAT.          */
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
/*  1997-03-09   Originally written                                 */
/*  1997-03-18   Calculate BCONE's                                  */
/*  1999-12-01   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_rotloft    Create a LFT_SUR rotation surface*/
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Check and initiations For Debug On */
#endif
static short ubounds();       /* Patch U boundary values            */
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
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol        * Retrieve computer tolerance              */
/* varkon_ctol           * Retrieve coordinate tolerance            */
/* varkon_ini_gmsur      * Initialize DBSurf                        */
/* varkon_ini_gmpat      * Initialize DBPatch                       */
/* varkon_ini_gmpatl     * Initialize GMPATL                        */
/* varkon_sur_bound      * Bounding box & cone for surface          */
/* varkon_erinit         * Initialize error message stack           */
/* varkon_erpush         * Error to error message stack             */
/* varkon_pat_priconloft * Printout lofting patch                   */
/* GE200                 * Create a line (DBLine)                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_rotloft   */
/* SU2993 = Severe program error ( ) in varkon_sur_rotloft (sur851) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus           varkon_sur_rotloft (

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
  DBPatch **pp_pat )     /* Allocated area for patch data     (ptr) */

/* Out:                                                             */
/*                                                                  */
/*       Data to p_sur and coefficients to the pp_pat area          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATL  *p_cpat;      /* Current geometric patch           (ptr) */
/*-----------------------------------------------------------------!*/

   DBint    i_s;         /* Loop index surface patch record         */
   DBPatch *p_frst;      /* First patch in allocated area     (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
   char    *p_tpat;      /* Allocated area topol. patch data  (ptr) */
   DBint    acc;         /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   DBint    pat_no;      /* Patch number in surface                 */
   DBSeg   *p_startseg;  /* Current start segment             (ptr) */
   DBSeg   *p_seg;       /* Current segment                   (ptr) */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String 1 for error message fctn erpush  */

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ____________________                              */
/*               !                    !                             */
/*               ! varkon_sur_rotloft !                             */
/*               !     (sur851)       !                             */
/*               !____________________!                             */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*  ! Initial.  ! !                          ! !  Exit  !           */
/*  !___________! !__________________________! !________!           */
/*  Debug ! On                 !                                    */
/*   _____!_____               !                                    */
/*  ! initial   !              !                                    */
/*  !___________!              !                                    */
/*       ______________________!_____________                       */
/*  ____!_____   __________!__________    ___!___                   */
/* !          ! !                   * !  !       !                  */
/* ! Initial. ! ! U direction patches !  !       !                  */
/* !   loop   ! !_____________________!  !       !                  */
/* ! variabl. !  __________!__________   !_______!                  */
/* !__________! !                   * !                             */
/*              ! V direction patches !                             */
/*              !_____________________!                             */
/*       __________________!                                        */
/*  ____!____    __________!__________                              */
/* !         !  !                   * !                             */
/* !         !  ! Within one patch    !                             */
/* !_________!  !_____________________!                             */
/*                     ____!___                                     */
/*                    !        !                                    */
/*                    !        !                                    */
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
"sur851 Enter varkon_sur_rotloft: LFT_SUR rotation surface \n");
fflush(dbgfil(SURPAC)); 
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
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
        sprintf(errbuf,"initial%%varkon_sur_rotloft (sur851)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* Initialize pp_pat to NULL. Variable pp_pat is also a flag        */
/* to the calling function. Memory must be deallocated if           */
/* pp_pat not is equal to NULL (also for errors).                   */
/*                                                                 !*/

*pp_pat = NULL;                          /* Initiate output pointer */

/*!                                                                 */
/* Computer   tolerance. Call of varkon_comptol (sur753).           */
/* Coordinate tolerance. Call of varkon_ctol    (sur751).           */
/*                                                                 !*/

   comptol= varkon_comptol();
   ctol   = varkon_ctol();

/*!                                                                 */
/* Initialize surface data in p_sur.                                */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur);

/*!                                                                 */
/* Check mflag values                                               */
/*                                                                 !*/
  for ( iv =1; iv <= no_strips ; ++iv )
    {
    if ( mflag[iv-1] == 3 )
      {
      ;/* Rotation */
      }
    else
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur851 Mid curve flag not Ok mflag[%d]= %d (not 3)\n",
(int)iv-1, (int)mflag[iv-1]);
}
#endif
      sprintf(errbuf ,"%d %%sur851", (int)mflag[iv-1]);
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
/* of the input, longitudinal limit curves.                         */
/* Call of internal function ubounds.                               */
/*                                                                 !*/

    status=ubounds
   (p_spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag);
    if (status<0) 
        {
        sprintf(errbuf,"ubounds%%varkon_sur_rotloft (sur851)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/* For Debug On:                                                    */
/* Check that the number of U patch boundaries is equal to the      */
/* number of segments for all longitudinal limit and tangent        */
/* curves. The input data to this function is assumed to be         */
/* calculated in varkon_sur_rot (sur850), i.e. the same (one)       */
/* curve is the originate for all limit and tangent curves.         */
/* The number of segments in the curves shall be n_spine_u-1.       */
/* The mid curves (P-value or R*3) are not checked, since           */
/* they not are used for a LFT_SUR rotation surface.                */
/*                                                                 !*/

#ifdef DEBUG    /* Check number of segments Start */
  for ( iv =1; iv <= no_strips+1 ; ++iv ) 
    {
    if ( lim[iv-1].ns_cu == n_spine_u - 1 &&
         tan[iv-1].ns_cu == n_spine_u - 1     )
      {
      ;/* The number of segments is OK */
      }
    else
      {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur851 Number of segments not OK lim[%d].ns_cu= %d n_spine_u-1= %d\n",
(int)iv-1, (int)lim[iv-1].ns_cu, (int)n_spine_u-1);
fprintf(dbgfil(SURPAC),
"sur851                           tan[%d].ns_cu= %d \n",
(int)iv-1, (int)tan[iv-1].ns_cu);
}
      sprintf(errbuf ,"n_spine_u %d %%sur851", (int)n_spine_u );
      varkon_erinit();
      return(varkon_erpush("SU2993",errbuf));
      }
    }  /* End loop all limit and tangent curves */
#endif    /* Check number of segments End   */


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
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_tpat=DBcreate_patches(TOP_PAT,nu*nv))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_sur_rotloft (sur851");
 return(varkon_erpush("SU2993",errbuf));
 }

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

  for ( iv =1; iv <=nv ; ++iv )
    {

/*!                                                                 */
/*       Dynamic allocation of area for one geometrical patch.      */
/*       Call of function DBcreate_patches.                         */
/*       Current geometric patch address p_cpat.                    */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(LFT_PAT,1)) == NULL )
 {
 sprintf(errbuf, "(allocg)%%varkon_sur_rotloft (sur851");
 return(varkon_erpush("SU2993",errbuf));
 }

         p_cpat= (GMPATL *)p_gpat;       /* Current geometric patch */

/*!                                                                 */
/* Initialize patch data in p_cpat for Debug On                     */
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
/*       Create lims, lime, tans, tane and spine.                   */
/*       Copy segments to current patch                             */
/*                                                                 !*/

         pat_no = pat_no + 1;

         p_startseg = p_lseg[iv-1];
         p_seg      = p_startseg + iu - 1; 
         V3MOME((char *)(p_seg),(char *)(&p_cpat->lims),sizeof(DBSeg));

         p_startseg = p_lseg[iv];
         p_seg      = p_startseg + iu - 1; 
         V3MOME((char *)(p_seg),(char *)(&p_cpat->lime),sizeof(DBSeg));

         p_startseg = p_tseg[iv-1];
         p_seg      = p_startseg + iu - 1; 
         V3MOME((char *)(p_seg),(char *)(&p_cpat->tans),sizeof(DBSeg));

         p_startseg = p_tseg[iv];
         p_seg      = p_startseg + iu - 1; 
         V3MOME((char *)(p_seg),(char *)(&p_cpat->tane),sizeof(DBSeg));

         V3MOME((char *)(p_spineseg),(char *)(&p_cpat->spine),sizeof(DBSeg));

/*!                                                                 */
/*       Create V limit curves. Call of crevlim.                    */
/*                                                                 !*/

    status=crevlim (p_cpat);
    if (status<0) 
        {
        sprintf(errbuf,"(crevlim)%%varkon_sur_rotloft (sur851)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/*       P value flag for patch defined by midpflag                 */
/*       Let offset for patch be zero (0).                          */
/*                                                                 !*/

         if      ( mflag[iv-1] == 3 )
             p_cpat->p_flag  = 3;
         else 
          {
          sprintf(errbuf,"(p_flag not 3)%%sur851");
          return(varkon_erpush("SU2993",errbuf));
          }


         p_cpat->ofs_pat = 0.0; 

/*!                                                                 */
/*       Printout of patch data                     For Debug On    */
/*       Call of varkon_pat_priconloft (sur223).                    */
/*                                                                 !*/

#ifdef DEBUG
if  ( pat_no  <   9 )              /* !!! Temporary no patches      */
{
status=varkon_pat_priconloft (p_cpat);
    if (status<0) 
       {
       sprintf(errbuf,"sur223%%varkon_sur_rotloft");
       return(varkon_erpush("SU2943",errbuf));
       }
}
#endif


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur851 p_t %d p_cpat %d pat_no %d iu %d iv %d\n", 
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
"sur851 Surface is created. Start loop BBOX calculation\n");
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

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_sur,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_rotloft(sur851)");
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
"sur851 Exit *** varkon_sur_rotloft  nu %d nv %d\n",
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
  DBPatch **pp_pat;      /* Allocated area for patch data     (ptr) */

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
"sur851*initial p_sur %d\n", (int)p_sur );
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*initial no_strips= %d\n",(int)no_strips);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*initial p_spine %d p_spineseg %d (*p_spine).ns_cu %d\n",
                  (int)p_spine,   (int)p_spineseg,   (int)(*p_spine).ns_cu );
  fflush(dbgfil(SURPAC));
  }

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*initial &lim[0] %d p_lseg[0] %d lim[0].ns_cu %d\n",
                  (int)&lim[0],   (int)p_lseg[0],   (int)lim[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &tan[0] %d p_tseg[0] %d tan[0].ns_cu %d\n",
                  (int)&tan[0],   (int)p_tseg[0],   (int)tan[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &mid[0] %d p_mseg[0] %d mid[0].ns_cu %d\n",
                  (int)&mid[0],   (int)p_mseg[0],   (int)mid[0].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &lim[1] %d p_lseg[1] %d lim[1].ns_cu %d\n",
                  (int)&lim[1],   (int)p_lseg[1],   (int)lim[1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &tan[1] %d p_tseg[1] %d tan[1].ns_cu %d\n",
                  (int)&tan[1],   (int)p_tseg[1],   (int)tan[1].ns_cu );
  fflush(dbgfil(SURPAC)); 
  }

if ( no_strips > 1 )
{
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*initial &mid[1] %d p_mseg[1] %d mid[1].ns_cu %d\n",
                  (int)&mid[1],   (int)p_mseg[1],   (int)mid[1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &lim[2] %d p_lseg[2] %d lim[2].ns_cu %d\n",
                  (int)&lim[2],   (int)p_lseg[2],   (int)lim[2].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial &tan[2] %d p_tseg[2] %d tan[2].ns_cu %d\n",
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
  "sur851*initial p_lseg %d lim().ns_cu %d            Strip %d\n",
   (int)p_lseg[i_strip-1],   (int)lim[i_strip-1].ns_cu, (int)i_strip );
  fprintf(dbgfil(SURPAC),
  "sur851*initial p_tseg %d tan().ns_cu %d\n",
   (int)p_tseg[i_strip-1],   (int)tan[i_strip-1].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial p_mseg %d mid().ns_cu %d p_flag %d\n",
   (int)p_mseg[i_strip-1],   (int)mid[i_strip-1].ns_cu, (int)mflag[i_strip-1] );
   }
if ( i_strip == no_strips )
{
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*initial p_lseg %d lim().ns_cu %d\n",
   (int)p_lseg[i_strip],     (int)lim[i_strip].ns_cu );
  fprintf(dbgfil(SURPAC),
  "sur851*initial p_tseg %d tan().ns_cu %d\n",
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
     sprintf(errbuf,"no_strips>STRIP%%varkon_sur_rotloft (sur851)");
     return(varkon_erpush("SU2993",errbuf));
     }

/*!                                                                 */
/* 3. Initialize output variables and static (internal) variables   */
/* ==============================================================   */
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
                         /* Eq. 3: Rotation                         */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBCurve *p_cur;       /* Pointer to the current curve            */
   DBint    i_segm;      /* Loop index segment in curve             */
#ifdef  DEBUG
   DBint    i_sol;       /* Loop index solution closest point       */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ==================                                               */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur851*ubounds Find spine planes for all segments in all curves\n");
fprintf(dbgfil(SURPAC),
"sur851*ubounds (Calc. closest pts on spine from all start/end pts)\n");
}
#endif

/*!                                                                 */
/* 2. U boundary values from start limit curve                      */
/* ===========================================                      */
/*                                                                  */
/* Let spine_u(i)= i for i=0, 1, 2, .. , ns_cu for start limit crv  */
/*                                                                 !*/

   n_spine_u = 0;

   p_cur      = &lim[0];
   for ( i_segm =0; i_segm <= p_cur->ns_cu; ++i_segm )
     {
     n_spine_u = n_spine_u + 1;
     spine_u[i_segm]= (DBfloat)i_segm;
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur851*ubounds Patch boundaries are defined by U values:\n");
for ( i_sol=1; i_sol<=n_spine_u; ++i_sol  )
  {
  fprintf(dbgfil(SURPAC),
  "sur851*ubounds Plane %d  spine_u(%2d) %f\n",
         (int)i_sol,(int)i_sol-1,spine_u[i_sol-1]);
  }
}
#endif

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
   DBVector p1;          /* Line start point                        */
   DBVector p2;          /* Line end   point                        */
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
  "sur851*crevlim iu= %d iv= %d &p_segpat->vs %d &p_segpat->ve %d \n"
   ,(int)iu,(int)iv,(int)&p_segpat->vs, (int)&p_segpat->ve);
  }
#endif

/*!                                                                 */
/* 1. Create line  segment vs                                       */

/*    Segment data to local variable p1 and p2                      */
/*                                                                 !*/

    p1.x_gm  =(DBfloat)iu  - 1.0;                 /* Start point    */
    p1.y_gm  =(DBfloat)iu  + 1.0;                 /*                */
    p1.z_gm  =   0.0;                             /*                */
    p2.x_gm  =(DBfloat)iu ;                       /* End   point    */
    p2.y_gm  =(DBfloat)iv  + 1.0;                 /*                */
    p2.z_gm  =   0.0;                             /*                */

/*!                                                                 */
/*    Call of GE200.                                                */
/*                                                                 !*/

    status=GE200 (&p1 , &p2 , &p_segpat->vs ); 
    if (status<0) 
       {
       sprintf(errbuf,"GE200(vs)%%varkon_sur_rotloft");
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

    status=GE200 (&p1 , &p2 , &p_segpat->ve ); 
    if (status<0) 
       {
       sprintf(errbuf,"GE200(ve)%%varkon_sur_rotloft");
       return(varkon_erpush("SU2943",errbuf));
       }

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


