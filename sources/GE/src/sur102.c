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
/*  Function: varkon_sur_interplane                File: sur102.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates the intersect curve between a surface      */
/*  and a plane. Output is a UV (surface) curve.                    */
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
/*  1994-06-04   Originally written                                 */
/*  1995-03-08   No patches np_u, np_v for sur910                   */
/*  1996-02-01   CON_SUR added                                      */
/*  1996-02-13   status= GEtform_inv, comptol                       */
/*  1996-05-10   idpoint, idangle, comptol to IPLANE added          */
/*  1996-05-26   p_gm, cur.ns_cu added                              */
/*  1996-06-09   Debug                                              */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1997-01-31   Allocation 9*ter->17*ter (for sur940)              */
/*  1997-03-08   Denser UV net for LFT_SUR (3x3)                    */
/*  1997-04-07   Added patches for SUR_ROT ....                     */
/*  1997-12-21   NURBS added                                        */
/*  1998-01-09   Tolerances for LFT_SUR                             */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1998-09-26   nstart+1 for Johan's test example hull             */
/*  1999-11-25   Free source code modifications                     */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function ---------------*/
/*                                                            */
/*sdescr varkon_sur_interplane Intersection surface/plane     */
/*                                                            */
/*------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_ctol            * Retrieve coordinate tolerance           */
/* varkon_ntol            * Surface normal tolerance                */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_idangle         * Identical angles criterion              */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* varkon_ini_pbound      * Initialize PBOUND                       */
/* varkon_ini_epoint      * Initialize EPOINT                       */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/* GEtform_inv            * Invertation of matrix                   */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* *v3mall                * Allocate memory                         */
/* v3free                 * Free allocated memory                   */
/* varkon_sur_defpatb     * Define patch boundaries                 */
/* varkon_sur_epts        * Calculate entry/exit points             */
/* varkon_sur_epts_mbs    * Create MBS code as a check              */
/* varkon_sur_epts_ord    * Order EPOINT table                      */
/* varkon_sur_curbr       * Create requested curve branch           */
/* varkon_erpush          * Error message to terminal               */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_pat_pritop      * Print topological patch data            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2923 = No intersection with the given plane                    */
/* SU2943 = Called function .. failed in varkon_sur_interplane      */
/* SU2993 = Severe program error ( ) in varkon_sur_interplane       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_sur_interplane (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBptr    p_gm,         /* GM address to surface     (not a C ptr) */
  DBTmat  *p_isys,       /* Intersect system (XY plane)       (ptr) */
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
   DBfloat plane[4];     /* Intersect plane data: nx,ny,nz,D        */
   DBTmat  invcrd;       /* Inverted p_isys system                  */
   DBint   rtype;        /* Computation case = SURPLANE             */
   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, or .....         */
   IPLANE  pladat;       /* Planar intersect computation data       */
   DBfloat s_lim[2][2];  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint   pb_alloc;      /* Allocated size of patch boundary table  */
  PBOUND *p_pbtable;     /* Patch boundary   table PBOUND           */
  DBint   tnr;           /* Total number of records      in PBOUND  */
  EPOINT  etable[EPMAX]; /* Patch entry and exit points             */
  DBint   ter;           /* Total number of records      in EPOINT  */
  DBint   no_br;         /* Number of curve branches     in EPOINT  */
  DBint   cbsr[CBMAX];   /* Curve branches start records in EPOINT  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   max_seg;       /* Maximum number of curve segments        */
  EPOINT  eprec;         /* Patch entry and exit point record       */
  DBint   i_r;           /* Loop indec record                       */
  DBint   i_s;           /* Loop index segment record               */
  DBfloat ctol;          /* Coordinate tolerance                    */
  DBfloat ntol;          /* Surface normal tolerance                */
  DBfloat idpoint;       /* Identical point criterion               */
  DBfloat idangle;       /* Identical angle criterion               */
  DBSeg  *pseg_l;        /* Local ptr = *pp_seg after allocation    */
  DBint   np_u;          /* Number of approx patches per patch in U */
  DBint   np_v;          /* Number of approx patches per patch in V */
  DBfloat comptol;       /* Computer   tolerance                    */
  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */

#ifdef DEBUG
  DBint    aiu;          /* Adress to patch U. Eq. -1: All patches  */
  DBint    aiv;          /* Adress to patch V.                      */
#endif

/*!-------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_interplane !                          */
/*               !      (sur102)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/* !  Check   !       ! UV grid  ! ! Entry ! ! Allocate !! Output ! */
/* !  input,  !       !  Calcul. ! ! /exit ! ! UV curve !! curve  ! */
/* ! initiate !       ! accuracy ! !  pts  ! !  memory  !! branch ! */
/* !__________!       !__________! !_______! !__________!!________! */
/*      !                   !           !      ___!_____      !     */
/*      !                   !           !     !         !     !     */
/*      !                   !           !     !DBcreate !     !     */
/*      !                   !           !     !_segments!     !     */
/*      !                   !           !     !_________!     !     */
/*      !_________          !           !__________           !     */
/*  ____!___  ____!___  ____!____   ____!___   ____!___   ____!___  */
/* !        !!        !!         ! !        ! !        ! !        ! */
/* ! varkon !! varkon !! varkon  ! ! varkon ! ! varkon ! ! varkon ! */
/* ! _ctol  !!  _sur  !!  _sur   ! !  _sur  ! !  _sur  ! !  _sur  ! */
/* ! sur751 !!_nopatch!!_defpatb ! ! _epts  ! !  _epts ! ! _curbr ! */
/* ! sur754 !!        !!         ! !        ! !  _ord  ! !        ! */
/* ! sur755 !! sur230 !! sur910  ! ! sur920 ! ! sur922 ! ! sur930 ! */
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
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Computation case is intersection between a surface and a plane:  */
   rtype= SURPLAN;
/*                                                                 !*/

      plane[0] = p_isys->g31;
      plane[1] = p_isys->g32;
      plane[2] = p_isys->g33;

      status= GEtform_inv(p_isys,&invcrd);
#ifdef DEBUG
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur102 Invertation of matrix (GEtform_inv) failed\n");
}
#endif
    sprintf(errbuf,"varkon_GEtform_inv%%sur102");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

      plane[3] = plane[0]*invcrd.g14 + 
                 plane[1]*invcrd.g24 + 
                 plane[2]*invcrd.g34;
/*!                                                                 */
/* Coordinate tolerance. Call of varkon_ctol (sur751).              */
/*                                                                 !*/



   ctol      = varkon_ctol();
   comptol   = varkon_comptol();
   ntol      = varkon_ntol();
   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();

/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;

/*!                                                                 */
/* Initialize table EPOINT                                          */
/* Calls of varkon_ini_epoint (sur775)                              */
/*                                                                 !*/

   for (i_r=1; i_r<=EPMAX; i_r= i_r+1)
     {
    varkon_ini_epoint (&eprec);
    etable[i_r-1] = eprec;
    }

/*!                                                                 */
/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surface is of type                  */
/*               CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR      */
/*                                                                 !*/

    status = varkon_sur_nopatch
    (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_interplane(sur102)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     ==  CUB_SUR ||         /* Check surface type      */
     surtype     ==  RAT_SUR ||           
     surtype     ==  MIX_SUR ||           
     surtype     ==  CON_SUR ||           
     surtype     ==  POL_SUR ||           
     surtype     ==   P3_SUR ||           
     surtype     ==   P5_SUR ||           
     surtype     ==   P7_SUR ||           
     surtype     ==   P9_SUR ||           
     surtype     ==  P21_SUR ||           
     surtype     == NURB_SUR ||           
     surtype     ==  LFT_SUR )            
 ;
 else
 {
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 Error type %d\n", (short)surtype );
  }
 sprintf(errbuf,
 "(type)%%varkon_sur_interplane(sur102");
 return(varkon_erpush("SU2993",errbuf)); 
 }
#endif

/*!                                                                 */
/* For Debug On: Printout of all topological patches                */
/*               Call of varkon_pat_pritop  (sur232).               */
/*                                                                 !*/


#ifdef DEBUG
  aiu = -1;
  aiv = -1;
    status = varkon_pat_pritop 
    (p_sur,p_pat,aiu,aiv);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pritop%%varkon_sur_interplane)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* 2. Definition of rectangular grid of u,v curves                  */
/* _______________________________________________                  */
/*                                                                  */
/* Whole surface. Let s_lim be UV points (1,1) and (nu,nv)          */
/*                                                                  */
/* Patch boundaries (the accuracy) to be used in the interplane     */
/* line calculation will be defined in function:                    */
/* varkon_sur_defpatb (sur910)                                      */
/*                                                                 !*/

s_lim[0][0] = 1.0 + comptol;
s_lim[0][1] = 1.0 + comptol;
s_lim[1][0] = (DBfloat)nu;
s_lim[1][1] = (DBfloat)nv;


if  ( surtype == LFT_SUR )
  {
  np_u = 3;
  np_v = 3;
  np_u = 5;           /* 1997-04-07 */
  np_v = 3;           /* 1997-04-07 */
  }
else
  {
  np_u = 0;
  np_v = 0;
  }

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

/*  Number is defined in sur910. Move here and input to sur910 ?    */
    pb_alloc = nu*nv*(np_u+1)*(np_v+1);

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur102");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur102 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
 sprintf(errbuf, "(PBOUND alloc)%%sur102");
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

status=varkon_sur_defpatb
   (p_sur,p_pat,surtype,nu,nv,s_lim,rtype,
         acc,np_u,np_v,pb_alloc,p_pbtable,&tnr);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
        sprintf(errbuf,"sur910%%sur102");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Calculation of entry/exit points                              */
/* ___________________________________                              */
/*                                                                  */
/* Computation case rtype and plane to structure variable pladat.   */
/* Selected method (NEWTON), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable pladat.               */
/*                                                                 !*/


pladat.ctype   = SURPLAN;
pladat.method  = NEWTON;
pladat.nstart  = 3;      
pladat.nstart  = 4;      /* 1998-09-26 For Johan's hull */
pladat.maxiter = 20;      

pladat.in_x    = plane[0];
pladat.in_y    = plane[1];
pladat.in_z    = plane[2];
pladat.in_d    = plane[3];
if (  surtype     !=  LFT_SUR )            
pladat.ctol    = ctol; 
else                                       
/* The change of tolerance ctol is probably not needed any longer */
/* In sur900 has function ana2pts been added 1998-09-26           */
pladat.ctol    = ctol*0.05;  /* 1998-01-09 for Johan's boat */
pladat.ntol    = ntol;       
pladat.idpoint = idpoint;
pladat.idangle = idangle;
pladat.comptol = comptol;
pladat.d_che   = F_UNDEF;

/*!                                                                 */
/* Calculate entry and exit points in the patches defined by        */
/* table pbtable. Call of function varkon_sur_epts (sur920).        */
/* Output entry/exit (U,V) points is stored in table etable.        */
/* The total number of records in etable is ter.                    */
/*                                                                 !*/
status=varkon_sur_epts
   (p_sur,p_pat,(IRUNON*)&pladat,p_pbtable,tnr,etable,&ter);
   if ( ter == 0 && cur_no < 0 )
      {
      *p_nobranch = 0;
      *pp_seg = NULL;
      goto  noalloc;
      }

   if ( ter == 0 ) 
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
      sprintf(errbuf,"nx %3.1f ny %3.1f nz %3.1f D %15.8f",
               plane[0], plane[1], plane[2], plane[3] );
       varkon_erinit();
      return(varkon_erpush("SU2923",errbuf));
      }

   if (status<0) 
      {
      sprintf(errbuf,"varkon_sur_epts(sur920)%%varkon_sur_interplane");
      return(varkon_erpush("SU2943",errbuf));
      }

/*!                                                                 */
/* Error message SU2923 and return if table EPOINT is empty         */
/*!                                                                 */

/*!                                                                 */
/* For DEBUG and SURPAC= 2 will MBS code for lines (LIN_FREE)       */
/* between the points in table etable be created. Also R*3          */
/* lines will be created.                                           */
/* Call of function varkon_sur_epts_mbs (sur921).                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
status=varkon_sur_epts_mbs (p_sur,p_pat,etable,ter);
if (status<0) 
   {
   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
   sprintf(errbuf,"sur921%%sur102");
   return(varkon_erpush("SU2943",errbuf));
   }
}
#endif

/*!                                                                 */
/* Order the points in the EPOINT table and find curve branches.    */
/* Call of function varkon_sur_epts_ord (sur922)                    */
/*                                                                 !*/

status= varkon_sur_epts_ord (etable,ter,&no_br,cbsr);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
  sprintf(errbuf,"sur922%%sur102");
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

/*!                                                                 */
/* 4. Allocate memory for the output UV curve                       */
/* __________________________________________                       */
/*                                                                 !*/

/*!                                                                 */
/* Memory for all PBOUND records multiplied by 17 will be generated */
/* Call of function varkon_sur_curbr (sur930)                       */
/*                                                                 !*/

   max_seg = ter*17;
   if((*pp_seg = DBcreate_segments(max_seg))==NULL)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
    sprintf(errbuf, "(alloc max_seg)%%sur102");
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

   p_cur->ns_cu = (short)(max_seg); 

/*!                                                                 */
/* Initialize segment data in pp_seg. Surface GM ptr to each segm.  */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= max_seg;  i_s= i_s+1)  /* Start loop           */
     {            
    varkon_ini_gmseg ( *pp_seg+i_s-1);  

    ( *pp_seg+i_s-1 )->subtyp   = 2;
    ( *pp_seg+i_s-1 )->spek_gm  = p_gm;

    }                                     /* End   loop             */

/*!                                                                 */
/* 5. Create output curve (branch)                                  */
/* _______________________________                                  */
/*                                                                 !*/

/*!                                                                 */
/* Create the requested (U,V) curve cur_no.                         */
/* Call of function varkon_sur_curbr (sur930)                       */
/*                                                                 !*/

status=varkon_sur_curbr
 (p_sur,p_pat,(IRUNON*)&pladat,etable,ter,no_br,cbsr,cur_no,
                      p_cur,pseg_l);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");
  sprintf(errbuf,"sur930%%sur102");
  return(varkon_erpush("SU2943",errbuf));
  }

/* 6. Exit                                                          */
/* -------                                                          */

noalloc:; /* No allocation of memory for cur_no < 0                 */

/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur102");


    return(SUCCED);

  } /* End of function */

/*********************************************************/
