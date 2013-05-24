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
/*  Function: varkon_sur_intersurf                 File: sur104.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  TODO This function is not finished !                            */
/*                                                                  */
/*  The function creates the intersect curve between two surfaces.  */
/*  Output is a UV (surface) curve.                                 */
/*                                                                  */
/*  The function allocates memory area for the UV curve segments.   */
/*  This area must be deallocated by the calling function.          */
/*  (No allocation if requested curve number cur_no < 0 or          */
/*   if function fails)                                             */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-10   Originally written                                 */
/*  1996-05-26   Initialization of GMCUR added                      */
/*  1996-06-15   CON_SUR                                            */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1998-09-04   compilation warnings, documentation                */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function ---------------*/
/*                                                            */
/*sdescr varkon_sur_intersurf  Intersection surface/surface   */
/*                                                            */
/*------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_ctol            * Retrieve coordinate tolerance           */
/* varkon_ntol            * Surface normal tolerance                */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_idangle         * Identical angles criterion              */
/* varkon_ini_gmseg       * Initiate DBSeg                          */
/* varkon_ini_pbound      * Initiate PBOUND                         */
/* varkon_ini_aplane      * Initiate APLANE                         */
/* varkon_ini_apoint      * Initiate APOINT                         */
/* varkon_ini_epoint      * Initiate EPOINT                         */
/* varkon_ini_gmcur       * Initiate DBCurve                        */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* *v3mall                * Allocate memory                         */
/* v3free                 * Free allocated memory                   */
/* varkon_sur_selpati     * Select intersecting patches             */
/* varkon_sur_aplane      * Approximate surface                     */
/* varkon_sur_interaplan  * Intersect APLANE surfaces               */
/* varkon_sur_epts2       * Calculate entry/exit points             */
/* varkon_sur_epts_mbs    * Create MBS code as a check              */
/* varkon_sur_epts_ord    * Order EPOINT table                      */
/* varkon_sur_curbr       * Create requested curve branch           */
/* varkon_erpush          * Error message to terminal               */
/* varkon_erinit          * Initiate error message stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2923 = No intersection with the given plane                    */
/* SU2943 = Called function .. failed in varkon_sur_intersurf       */
/* SU2993 = Severe program error ( ) in varkon_sur_intersurf        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_sur_intersurf (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur1,       /* Surface 1                         (ptr) */
  DBPatch *p_pat1,       /* Alloc. area for topol. patch data (ptr) */
  DBptr    p_gm1,        /* GM address to surface 1   (not a C ptr) */
  DBSurf  *p_sur2,       /* Surface 2                         (ptr) */
  DBPatch *p_pat2,       /* Alloc. area for topol. patch data (ptr) */
  DBptr    p_gm2,        /* GM address to surface 2   (not a C ptr) */
  DBint    acc,          /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
  DBint    cur_no,       /* Requested output curve branch number    */
                         /* cur_no < 0: Calculate p_nobranch only   */
  DBint   *p_nobranch,   /* Requested output curve branch number    */
  DBCurve *p_cur,        /* Surface curve                           */
  DBSeg  **pp_seg )      /* Allocated area for UV segments    (ptr) */

/* Out:                                                             */
/*     Surface UV curve data to to p_cur and pp_seg                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   ISURSUR surdat;       /* Surface/surface intersect data          */
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBint   rtype;        /* Computation case = SURSUR               */
   DBint   i_r;          /* Loop index record in PBOUND             */
   DBfloat ctol;         /* Coordinate tolerance                    */
   DBfloat comptol;      /* Computer   tolerance                    */
   DBfloat ntol;         /* Surface normal tolerance                */
   DBfloat idpoint;      /* Identical point criterion               */
   DBfloat idangle;      /* Identical angle criterion               */

   DBint  nu1;           /* Number of patches in U direction p_sur1 */
   DBint  nv1;           /* Number of patches in V direction p_sur1 */
   DBint  surtype1;      /* Type CUB_SUR, RAT_SUR, or .....  p_sur1 */
   DBfloat s_lim1[2][2]; /* Surface limitation U,V points    p_sur1 */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
   DBint  nu2;           /* Number of patches in U direction p_sur2 */
   DBint  nv2;           /* Number of patches in V direction p_sur2 */
   DBint  surtype2;      /* Type CUB_SUR, RAT_SUR, or .....  p_sur2 */
   DBfloat s_lim2[2][2]; /* Surface limitation U,V points    p_sur2 */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint   pb_alloc;      /* Allocated size of patch boundary table  */
  PBOUND *p_pbtable;     /* Patch boundary   table PBOUND           */
#ifdef  TABORT
  PBOUND  pbtable[PBMAX];/* Patch boundary table                    */
  PBOUND  pbrec;         /* Patch boundary record for initiation    */
#endif
  DBint   tnr;           /* Total number of records      in PBOUND  */
  APLANE  asur1[PLMAX];  /* Surface 1 approximation table           */
  DBint   nasur1;        /* Total number of records in asur1        */
  APLANE  asur2[PLMAX];  /* Surface 2 approximation table           */
  DBint   nasur2;        /* Total number of records in asur2        */
  APOINT  apts[APMAX];   /* APLANE intersect points                 */
  DBint   napts;         /* Total number of records in apts         */

  DBint   sur_no;        /* Current surface for approximation       */

  EPOINT  etable1[EPSMA];/* Patch entry and exit points surface 1   */
  DBint   ter1;          /* Total number of records in etable1      */
  EPOINT  etable2[EPSMA];/* Patch entry and exit points surface 2   */
  DBint   ter2;          /* Total number of records in etable2      */


  APLANE  plrec;         /* Approximative patch to input surface    */
  APOINT  aprec;         /* Intersect point                         */

  EPOINT  eprec;         /* Patch entry and exit point record       */
#ifdef  TILLSVIDARE
  DBint   i_s;           /* Loop index segment record               */
  DBSeg  *pseg_l;        /* Local ptr = *pp_seg after allocation    */
  DBint   np_u;          /* Number of approx patches per patch in U */
  DBint   np_v;          /* Number of approx patches per patch in V */
  DBint   no_br;         /* Number of curve branches     in EPOINT  */
  DBint   cbsr[CBMAX];   /* Curve branches start records in EPOINT  */
#endif
  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */

#ifdef DEBUG_TABORT_KANSKE
  DBint    aiu;          /* Adress to patch U. Eq. -1: All patches  */
  DBint    aiv;          /* Adress to patch V.                      */
#endif

/*!-------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_intersurf  !                          */
/*               !      (sur104)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/* !  Check   !       ! UV grid  ! ! Entry ! ! Allocate !! Output ! */
/* !  input,  !       !  Calcul. ! ! /exit ! ! UV curve !! curve  ! */
/* ! initiate !       ! accuracy ! !  pts  ! !  memory  !! branch ! */
/* !__________!       !__________! !_______! !__________!!________! */
/*      !                   !           !      ___!____       !     */
/*      !                   !           !     !        !      !     */
/*      !                   !           !     !DBcreate!      !     */
/*      !                   !           !     !segments!      !     */
/*      !                   !           !     !________!      !     */
/*      !_________          !           !__________           !     */
/*  ____!___  ____!___  ____!____   ____!___   ____!___   ____!___  */
/* !        !!        !!         ! !        ! !        ! !        ! */
/* ! varkon !! varkon !! varkon  ! ! varkon ! ! varkon ! ! varkon ! */
/* ! _ctol  !!  _sur  !!  _sur   ! !  _sur  ! !  _sur  ! !  _sur  ! */
/* ! sur751 !!_nopatch!!_defpatb ! ! _epts2 ! !  _epts ! ! _curbr ! */
/* ! sur754 !!        !!         ! !        ! !  _ord  ! !        ! */
/* ! sur755 !! sur230 !! sur910  ! ! sur923 ! ! sur922 ! ! sur930 ! */
/* !________!!________!!_________! !________! !________! !________! */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* Computation case is intersection between two surfaces:           */
   rtype= SURSUR;  
/*                                                                 !*/

/*!                                                                 */
/* Retrieve tolerances, end criteria calculations, etc              */
/*                                                                 !*/

   comptol   = varkon_comptol();
   ctol      = varkon_ctol();
   ntol      = varkon_ntol();
   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();

/*!                                                                 */
/* Let output pointer to segments pp_seg be NULL                    */
/*                                                                 !*/

  *pp_seg     = NULL;
  *p_nobranch = 0;

/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;

/*!                                                                 */
/* Initialize tables PBOUND, EPOINT, APLANE and APOINT              */
/* Calls of varkon_ini_epoint (sur775)                              */
/* and varkon_ini_aplane (sur760)                                   */
/*                                                                 !*/

   for (i_r=1; i_r<=EPSMA; i_r= i_r+1)
     {
    varkon_ini_epoint (&eprec);
    etable1[i_r-1] = eprec;
    }

   for (i_r=1; i_r<=EPSMA; i_r= i_r+1)
     {
    varkon_ini_epoint (&eprec);
    etable2[i_r-1] = eprec;
    }

   for (i_r=1; i_r<=PLMAX; i_r= i_r+1)
     {
    varkon_ini_aplane (&plrec);
    asur1[i_r-1] = plrec;
    }
   for (i_r=1; i_r<=PLMAX; i_r= i_r+1)
     {
    varkon_ini_aplane (&plrec);
    asur2[i_r-1] = plrec;
    }

   for (i_r=1; i_r<=APMAX; i_r= i_r+1)
     {
    varkon_ini_apoint (&aprec);
    apts[i_r-1] = aprec;
    }
/*!                                                                 */
/* Retrieve number of patches and surface types.                    */
/* Calls of varkon_sur_nopatch (sur230).                            */
/* For Debug On: Check that the surfaces are of type                */
/*               CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR      */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur1, &nu1, &nv1, &surtype1);
#ifdef DEBUG
  if(status<0)
  {
    sprintf(errbuf,"varkon_sur_nopatch(1)%%varkon_sur_intersurf)");
    return(varkon_erpush("SU2943",errbuf));
  }
#endif


    status = varkon_sur_nopatch (p_sur2, &nu2, &nv2, &surtype2);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch(2)%%varkon_sur_intersurf");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG


if ( surtype1    == CUB_SUR ||           /* Check surface type      */
     surtype1    == RAT_SUR ||           
     surtype1    == CON_SUR ||           
     surtype1    == MIX_SUR ||           
     surtype1    == LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type1)%%varkon_sur_intsurf (sur104");
 return(varkon_erpush("SU2993",errbuf));
 }





if ( surtype2    == CUB_SUR ||           /* Check surface type      */
     surtype2    == RAT_SUR ||           
     surtype2    == CON_SUR ||           
     surtype2    == MIX_SUR ||           
     surtype2    == LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type2)%%varkon_sur_intsurf (sur104");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif


/*!                                                                 */
/* Define tolerances, methods, etc for the intersect calulation.    */
/*                                                                 !*/

surdat.ctype    = rtype;          /* Type of calculation            */
surdat.method   = NEWTON;         /* Method to be used              */
surdat.nstart   = 10;             /* Number of restarts             */
surdat.maxiter  = 20;             /* Maximum number of restarts     */
surdat.s_method = 1;              /* Select/search method           */
                                  /* Eq. 1: BBOX'es only            */

surdat.ctol     = ctol;           /* Coordinate     tolerance       */
surdat.ntol     = ntol;           /* Surface normal tolerance       */
surdat.idpoint  = idpoint;        /* Identical point criterion      */
surdat.idangle  = idangle;        /* Identical angle criterion      */
surdat.comptol  = comptol;        /* Computer accuracy tolerance    */
surdat.factol   = 0.500;          /* Facets (B-planes) tolerance    */
surdat.d_che    = 20.0;           /* Maximum check R*3 distance     */

                                  /* Density of the UV net:         */
surdat.u1_add   = -1;             /* For surface 1 in U direction   */
surdat.v1_add   = -1;             /* For surface 1 in V direction   */
surdat.u2_add   = -1;             /* For surface 2 in U direction   */
surdat.v2_add   = -1;             /* For surface 2 in V direction   */
                                  /* (Values 1, 2, 3, 4, .... )     */

                                  /* Patch limit pts (search area): */
surdat.us1      = F_UNDEF;        /* U start surface 1              */
surdat.vs1      = F_UNDEF;        /* V start surface 1              */
surdat.ue1      = F_UNDEF;        /* U end   surface 1              */
surdat.ve1      = F_UNDEF;        /* V end   surface 1              */
surdat.us2      = F_UNDEF;        /* U start surface 2              */
surdat.vs2      = F_UNDEF;        /* V start surface 2              */
surdat.ue2      = F_UNDEF;        /* U end   surface 2              */
surdat.ve2      = F_UNDEF;        /* V end   surface 2              */

                                  /* Segment limits  (search area): */
surdat.usc      = F_UNDEF;        /* U start for curve              */
surdat.uec      = F_UNDEF;        /* U end   for curve              */

surdat.startuvpt1.x_gm= F_UNDEF;  /* Start U,V point surface 1      */
surdat.startuvpt1.y_gm= F_UNDEF; 
surdat.startuvpt1.z_gm= F_UNDEF; 
surdat.startuvpt2.x_gm= F_UNDEF;  /* Start U,V point surface 2      */
surdat.startuvpt2.y_gm= F_UNDEF; 
surdat.startuvpt2.z_gm= F_UNDEF; 


/*!                                                                 */
/* 2. Select patches which intersect and create PBOUND table.       */
/* __________________________________________________________       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

/*  Number is defined in ???        TODO                            */
    pb_alloc = PBMAX;

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur104");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur104 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
      sprintf(errbuf, "(PBOUND alloc)%%sur104");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Initialize table PBOUND                                          */
/* Calls of varkon_ini_pbound (sur774).                             */
/*                                                                 !*/

   for (i_r=1; i_r <= pb_alloc; i_r= i_r+1)
     { 
     varkon_ini_pbound (p_pbtable+i_r-1);
     } 

s_lim1[0][0] = 1.0;
s_lim1[0][1] = 1.0;
s_lim1[1][0] = (DBfloat)nu1;
s_lim1[1][1] = (DBfloat)nv1;
s_lim2[0][0] = 1.0;
s_lim2[0][1] = 1.0;
s_lim2[1][0] = (DBfloat)nu2;
s_lim2[1][1] = (DBfloat)nv2;


status=varkon_sur_selpati
     (p_sur1,p_pat1,surtype1,nu1,nv1,s_lim1,
      p_sur2,p_pat2,surtype2,nu2,nv2,s_lim2,
           (IRUNON*)&surdat, p_pbtable, &tnr);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
        sprintf(errbuf,"sur918%%sur104 ");
        return(varkon_erpush("SU2943",errbuf));
        }

    if ( tnr == 0 )
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
      sprintf(errbuf,"(box approximation)%%(sur104)");
      return(varkon_erpush("SU6003",errbuf));
      }

/*!                                                                 */
/* 3. Approximate the selected patches with triangular planes.      */
/* __________________________________________________________       */
/*                                                                  */
/*                                                                 !*/

sur_no = 1;
status=varkon_sur_aplane 
     (p_sur1,p_pat1, p_pbtable, tnr, sur_no, asur1, &nasur1 );
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
        sprintf(errbuf,"sur178 (1)%%sur104 ");
        return(varkon_erpush("SU2943",errbuf));
        }
sur_no = 2;
status=varkon_sur_aplane 
     (p_sur2,p_pat2, p_pbtable, tnr, sur_no, asur2, &nasur2 );
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
        sprintf(errbuf,"sur178 (2)%%sur104");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur104 Number of records in APLANE table asur1 nasur1 %d \n",
        (int)nasur1);
fprintf(dbgfil(SURPAC),
"sur104 Number of records in APLANE table asur2 nasur2 %d \n",
        (int)nasur2);
fflush(dbgfil(SURPAC));
}
#endif

status=varkon_sur_interaplan
     ( asur1, nasur1, asur2, nasur2 , apts, &napts );
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
        sprintf(errbuf,"sur108%%sur104");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur104 Number of records in APOINT table apts   napts %d \n",
        (int)napts );
fflush(dbgfil(SURPAC));
}
#endif

    if ( napts == 0 )
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
      sprintf(errbuf,"(facet approximation)%%(sur104)");
      return(varkon_erpush("SU6003",errbuf));
      }

/*!                                                                 */
/* 4. Calculation of entry/exit points                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Calculate entry and exit points in the patches defined by        */
/* table p_pbtable. Call of function varkon_sur_epts2 (sur923).     */
/* Output entry/exit (U,V) points is stored in table etable1.       */
/* The total number of records in etable1 is ter1.                  */
/*                                                                 !*/

status=varkon_sur_epts2
  (p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,(IRUNON*)&surdat,
   p_pbtable,tnr, apts, napts, etable1,&ter1,etable2,&ter2);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur104 Number of records in EPOINT table etable1 ter1 %d\n",
        (int)ter1  );
fprintf(dbgfil(SURPAC),
"sur104 Number of records in EPOINT table etable2 ter2 %d\n",
        (int)ter2  );
fflush(dbgfil(SURPAC));
}
#endif


#ifdef  TILLSVIDARE

   if ( ter1 == 0 && cur_no < 0 )
      {
      *p_nobranch = 0;
      *pp_seg = NULL;
      goto  noalloc;
      }

   if ( ter1 == 0 ) 
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
      sprintf(errbuf,"nx %3.1f ny %3.1f nz %3.1f D %15.8f",
               plane[0], plane[1], plane[2], plane[3] );
       varkon_erinit();
      return(varkon_erpush("SU2923",errbuf));
      }

   if (status<0) 
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
      sprintf(errbuf,"varkon_sur_epts(sur920)%%varkon_sur_intersurf ");
      return(varkon_erpush("SU2943",errbuf));
      }

/*!                                                                 */
/* Error message SU2923 and return if table EPOINT is empty         */
/*!                                                                 */

/*!                                                                 */
/* For DEBUG and SURPAC= 2 will MBS code for lines (LIN_FREE)       */
/* between the points in table etable1 be created. Also R*3          */
/* lines will be created.                                           */
/* Call of function varkon_sur_epts_mbs (sur921).                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
status=varkon_sur_epts_mbs (p_sur1,p_pat1,etable1,ter1);
if (status<0) 
   {
   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
   sprintf(errbuf,"varkon_sur_epts_mbs(sur921)%%varkon_sur_intersurf ");
   return(varkon_erpush("SU2943",errbuf));
   }
}
#endif

/*!                                                                 */
/* Order the points in the EPOINT table and find curve branches.    */
/* Call of function varkon_sur_epts_ord (sur922)                    */
/*                                                                 !*/

status= varkon_sur_epts_ord (etable1,ter1,&no_br,cbsr);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
  sprintf(errbuf,"varkon_sur_epts_ord(sur922)%%varkon_sur_intersurf ");
  return(varkon_erpush("SU2943",errbuf));
  }

  *p_nobranch = no_br;

/*!                                                                 */
/* Number of branches to output variable p_nobranch.                */
/* Return if cur_no < 0 (with NULL pp_seg pointer)                  */
/*                                                                 !*/

  if ( cur_no < 0 )
    {
    *pp_seg = NULL;
    goto  noalloc;
    }

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Allocate memory for the output UV curve                       */
/* __________________________________________                       */
/*                                                                 !*/

/*!                                                                 */
/* Memory for all records multiplied by 4 will be generated.        */
/* (normally much more than is needed)                              */
/* Call of function varkon_sur_curbr (sur930)                       */
/*                                                                 !*/

   if((*pp_seg = DBcreate_segments(ter1*4))==NULL) /* Allocation    */
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
    sprintf(errbuf,
    "(alloc)%%varkon_sur_intersurf (sur104");
     return(varkon_erpush("SU2993",errbuf));
    }

/* Let locally defined pointer                                      */
   pseg_l = *pp_seg;
/* (in order to avoid problems with double *'s                      */

/*!                                                                 */
/* Initialize curve data in p_cur and let ns_su = maximum number    */
/* of segments for the curve (the size of the allocated area).      */
/* Call of varkon_ini_gmcur (sur778).                               */
/*                                                                 !*/
    varkon_ini_gmcur (p_cur);

   p_cur->ns_cu = (short)(ter1*4); 

/*!                                                                 */
/* Initialize segment data in pp_seg.                               */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= ter1*4;  i_s= i_s+1)
     {
    varkon_ini_gmseg ( *pp_seg+i_s-1);
    } 

/*!                                                                 */
/* 5. Create output curve (branch)                                  */
/* _______________________________                                  */
/*                                                                 !*/

/*!                                                                 */
/* Create the requested (U,V) curve cur_no.                         */
/* Call of function varkon_sur_curbr (sur930)                       */
/*                                                                 !*/

status=varkon_sur_curbr
 (p_sur1,p_pat1,&pladat,etable1,ter1,no_br,cbsr,cur_no,p_cur,pseg_l);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");
  sprintf(errbuf,"varkon_sur_curbr(sur930)%%varkon_sur_intersurf ");
  return(varkon_erpush("SU2943",errbuf));
  }

#endif  /* Slut TILLSVIDARE  */

/* 6. Exit                                                          */
/* -------                                                          */

#ifdef  TILLSVIDARE
noalloc:; /* No allocation of memory for cur_no < 0                 */
#endif

/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur104");


       varkon_erinit();

    return(-12345);   /*  !!!!! Temporary     */

    return(SUCCED);

  } /* End of function */

/*********************************************************/
