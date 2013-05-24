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
/*  Function: varkon_sur_facapprox                 File: sur170.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function approximates a surface with facets, i.e four       */
/*  points which define two triangels or a plane bounded by         */
/*  the four-point polygon.                                         */
/*                                                                  */
/*  Note that tolerances for the computation is input to            */
/*  the function.                                                   */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  TODO Created facets should be checked. No equal points ..       */
/*       Macby the B_PLANE check function could be used             */
/*  TODO Surface BBOX shall be added in sur913 !!!                  */
/*  TODO Add maximum number of pathes. Varkon crasches for too many */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-12-08   maxnum=nu*nv*DMAX (not 9) for LFT_SUR              */
/*  1995-03-08   No patches np_u, np_v for sur910                   */
/*  1996-01-24   New and old GMPATF                                 */
/*  1996-01-31   New GMPATF and other changes                       */
/*  1996-02-11   Case 11 for LFT_SUR surfaces                       */
/*  1996-11-18   Case  4                                            */
/*  1997-03-12   ngseg_su and pgseg_su initialized                  */
/*  1997-12-13   Comments added                                     */
/*  1998-03-21   pb_alloc, p_pbtable and NURB_SUR added             */
/*  1998-04-29   Check of np_u and np_v                             */
/*  1998-11-23   Surface box added                                  */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_facapprox  Approximate a surface with facets*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_ini_pbound       * Initialize PBOUND                      */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_sur_defpatb      * Define calc. patch boundaries          */
/* varkon_pat_chebound     * Check bounding box & cone              */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_erinit           * Initiate error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_facapprox       */
/* SU2993 = Severe program error (  ) in varkon_sur_facapprox       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_facapprox (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBfloat  offset,       /* Surface offset value                    */
  DBfloat  thick,        /* Thickness for the patches               */
  DBfloat  s_lim[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /* All equal zero => whole surface         */
  DBint    acase,        /* Approximation case:                     */
                         /* Eq.  1: No of facets is input           */
                         /* Eq.  2: No of facets calculated         */
                         /* Eq.  3: No of facets and offset input   */
                         /* Eq.  4: As 3 but with reversed facets   */
                         /* Eq. 11: As 1 but double patches         */
                         /* Eq. 12: As 2 but double patches         */
                         /* Eq. 13: As 3 but double patches         */
                         /* Eq. 21: As 1 but reversed normals       */
                         /* Eq. 22: As 2 but reversed normals       */
  DBint    np_u,         /* Number of approx patches per patch in U */
  DBint    np_v,         /* Number of approx patches per patch in V */
  DBint    acc,          /* Computation accuracy                    */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
                         /* Tolerances for the approximation.       */
                         /* ( < 0 <==> not defined )                */
  DBfloat  ctol,         /* Coordinate          tolerance           */
  DBfloat  ntol,         /* Surface normal      tolerance           */
  DBfloat  rtol,         /* Radius of curvature tolerance           */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */

/* Out:                                                             */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  APPROX  appdat;        /* Approximation computation data          */
  DBint   nu;            /* No patches in U direction input surface */
  DBint   nv;            /* No patches in V direction input surface */
  DBint   pb_alloc;      /* Allocated size of patch boundary table  */
  PBOUND *p_pbtable;     /* Patch boundary   table PBOUND           */
  DBint   pbn;           /* No of records in table PBOUND           */
  DBint   i_pat;         /* Loop index corresp. to PBOUND record    */
  BBOX    pbox;          /* Bounding box  for the (PBOUND) patch    */
  BCONE   pcone;         /* Bounding cone for the (PBOUND) patch    */
  DBfloat us,ue,vs,ve;   /* Limits        for the (PBOUND) patch    */
  DBint   maxnum;        /* Maximum number of patches (alloc. area) */
  DBPatch *p_frst;       /* Pointer to the first patch              */
  DBPatch *p_t;          /* Current topological patch         (ptr) */
  GMPATF  *p_patfc;      /* Current geometric   patch         (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint     i_r;         /* Loop index segment record               */
  char    *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
  char    *p_tpat;       /* Allocated area topol. patch data  (ptr) */
  DBint    sflag;        /* Flag for          the (PBOUND) patch    */
  DBint    icase;        /* Case for varkon_sur_eval                */
  EVALS    xyz;          /* Coordinates and derivatives for a       */
                         /* point on a surface                      */
  DBint    surtype;      /* Type CUB_SUR, RAT_SUR, or ...           */
  DBint    pat_no;       /* Patch number                            */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */
  DBfloat  dum1;         /* Dummy float                             */
  DBVector p1_e;         /* Point 1 for the double patch            */
  DBfloat  u1_e;         /* U for point 1                           */
  DBfloat  v1_e;         /* V for point 1                           */
  DBVector p2_e;         /* Point 2 for the double patch            */
  DBfloat  u2_e;         /* U for point 2                           */
  DBfloat  v2_e;         /* V for point 2                           */
  DBVector p3_e;         /* Point 3 for the double patch            */
  DBfloat  u3_e;         /* U for point 3                           */
  DBfloat  v3_e;         /* V for point 3                           */
  DBVector p4_e;         /* Point 4 for the double patch            */
  DBfloat  u4_e;         /* U for point 4                           */
  DBfloat  v4_e;         /* V for point 4                           */

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
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur170 Enter***varkon_sur_facapprox acase %d\n" , (int)acase );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur170 offset %8.4f thick %8.4f np_u %d np_v %d \n"
 , offset, thick, (int)np_u, (int)np_v );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);

/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   For Debug On: Check that the surface is of type                */
/*          CUB_SUR, RAT_SUR, LFT_SUR, PRO_SUR, CON_SUR, MIX_SUR    */
/*          POL_SUR,  P3_SUR,  P7_SUR,  P9_SUR or P21_SUR           */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur170");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     ==  CUB_SUR ||
     surtype     ==  RAT_SUR ||
     surtype     ==  LFT_SUR ||
     surtype     ==  CON_SUR ||
     surtype     ==  POL_SUR ||
     surtype     ==   P3_SUR ||
     surtype     ==   P5_SUR ||
     surtype     ==   P7_SUR ||
     surtype     ==   P9_SUR ||
     surtype     ==  P21_SUR ||
     surtype     ==  PRO_SUR ||
     surtype     == NURB_SUR ||
     surtype     ==  MIX_SUR )
 ;
 else
 {
 sprintf(errbuf, "(surface type)%%sur170");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

#ifdef  DO_NOT_UNDERSTAND_TODO
  if ( acase == 1 || acase == 11 || acase == 21 )
    ; /* Use input values */
  else
    {
    np_u = 0;
    np_v = 0;
    }
  if ( acase == 21 || acase == 22 )
  {
  sprintf(errbuf, 
  "(acase)%%varkon_sur_facapprox (sur170");
#ifdef LNAME
  return(varkon_erpush("SU2993",errbuf));
#endif
#ifdef SNAME
  return(erpush("SU2993",errbuf));
#endif
  }
#endif

  if ( acase == 1 || acase == 11 || acase == 2 || acase == 12 ||
       acase == 3 || acase == 13 || acase == 4 )
    ; /* acase is OK */
  else
  {
  sprintf(errbuf, "(acase)%%(sur170");
  return(varkon_erpush("SU2993",errbuf));
  }

/* Check number of added patches per patch                          */
  if ( acase == 1 || acase == 11 ||
       acase == 3 || acase == 13 ||
       acase == 4                )  
    {
    if  ( np_u  <  1 )
      {
      sprintf(errbuf, "(sur170)%%");
      varkon_erinit();
      return(varkon_erpush("SU7403",errbuf));
      }
    if  ( np_v  <  1 )
      {
      sprintf(errbuf, "(np_v sur170)%%");
      varkon_erinit();
      return(varkon_erpush("SU7403",errbuf));
      }
    }


/*!                                                                 */
/* 2. Data for the calculation to APPROX                            */
/* _____________________________________                            */
/*                                                                  */
/* Approximation (computation) data to appdat:                      */
/* Tolerances ctol, ntol and rtol.                                  */
/*                                                                 !*/

   appdat.ctol     = ctol;
   appdat.ntol     = ntol;
   appdat.rtol     = rtol;

/*!                                                                 */
/* Number of restarts in the patch nstart =   4                     */
/* Maximum number of iterations    maxiter=  50                     */
/* Approximation case acase and accuracy acc to appdat.             */
/*                                                                 !*/

   appdat.ctype    = SURAPPR;
   appdat.method   = I_UNDEF; 
   appdat.nstart   =  4;    
   appdat.maxiter  = 50;   
   appdat.acase    = acase;
   appdat.acc      = acc;
   appdat.offset   = F_UNDEF;
   appdat.thick    = F_UNDEF;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur170 APPROX: ctype %d method %d nstart %d\n",
(int)appdat.ctype,(int)appdat.method,(int)appdat.nstart);

fprintf(dbgfil(SURPAC),
"sur170 APPROX: maxiter %d acase %d acc %d ctol %f ntol %f\n",
(int)appdat.maxiter,(int)appdat.acase ,(int)appdat.acc, 
 appdat.ctol, appdat.ntol  );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Allocate memory area for output surface                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_patout) for the surface data.     */
/* Call of function DBcreate_patches. Error SU2993 for failure.               */
/* Maximum number of patches maxnum=nu*nv*4 for CUB_SUR and RAT_SUR */
/* Maximum number of patches maxnum=nu*nv*DMAX for LFT_SUR Temporary*/
/*                                                                 !*/
/*  BCONE is not calculated for LFT_SUR for performance reasons!    */

if ( acase == 1 || acase == 11 ||
     acase == 3 || acase == 13 ||
     acase == 4                )  maxnum = np_u*np_v*nu*nv;
else if ( surtype == LFT_SUR )    maxnum = nu*nv*DMAX;    
else                              maxnum = nu*nv*4;    
if ( acase == 11 || acase == 12 
     || acase == 13 )             maxnum = 2*maxnum;

/* TODO Add maximum number of pathes. Varkon crasches for too many  */

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 Allocation failed  nu %d nv %d DMAX %d maxnum %d\n",
    (int)nu, (int)nv, DMAX, (int)maxnum );
  fflush(dbgfil(SURPAC));
  }
#endif
 sprintf(errbuf, "(alloc TOP_PAT)%%sur170");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
pat_no = 0;                              /* Patch number 1, 2, 3, ..*/
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 Ptr to first patch p_frst  = %d No patches %d\n",
    (int)p_frst  ,(int)maxnum);
  fflush(dbgfil(SURPAC));
  }
#endif

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Define area of surface for facet approximation                */
/* _________________________________________________                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

    pb_alloc = maxnum;        

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur172");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 PBOUND allocation failed  pb_alloc %d maxnum %d PBMAX %d\n",
    (short)pb_alloc, (short)maxnum, PBMAX );
  }
#endif
      sprintf(errbuf, "(PBOUND alloc)%%sur170");
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

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur170 Memory is allocated and initialized for %d PBOUND records\n",
        (short)pb_alloc );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*   The area is the whole surface if s_lim = 0                     */
/*   A start approximation will be made ....                        */
/*   Call of varkon_sur_defpatb (sur910).                           */
/*                                                                 !*/

   dum1=s_lim[0][0]+s_lim[0][1]+s_lim[1][0]+s_lim[1][1];     
   if ( fabs(dum1) < TOL1 )
     {
     s_lim[0][0] = 1.0;
     s_lim[0][1] = 1.0;
     s_lim[1][0] = (DBfloat)nu;
     s_lim[1][1] = (DBfloat)nv;
     }

status=varkon_sur_defpatb
   (p_sur,p_pat,surtype,nu,nv,s_lim,SURAPPR,
          acc,np_u,np_v,pb_alloc, p_pbtable,&pbn);
   if (status<0) 
     {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
     sprintf(errbuf,"sur910%%sur170");
     return(varkon_erpush("SU2943",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur170 Surface nu %d nv %d surtype %d PBOUND records %d\n",
           (int)nu, (int)nv, (int)surtype , (int)pbn );
}
#endif

/*!                                                                 */
/* 5. Create the approximate surface                                */
/* _________________________________                                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop all records in PBOUND table i_pat=1,2,....,pbn              */
/*                                                                 !*/

for ( i_pat=1; i_pat<= pbn  ; ++i_pat)   /* Start loop PBOUND       */
  {

/*!                                                                 */
/*   Pointer to current patch p_t                                   */
/*                                                                 !*/

     pat_no = pat_no + 1;                /* Patch number 1, 2, 3, ..*/

     if ( pat_no > maxnum )
       {
       sprintf(errbuf, 
       "(maxnum)%%(sur170");
       return(varkon_erpush("SU2993",errbuf));
       }


    p_t = p_frst + pat_no - 1;           /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (facet)   patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(FAC_PAT, 1 ))==NULL)
 { 
 if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
 sprintf(errbuf, "(allocg)%%sur170");
 return(varkon_erpush("SU2993",errbuf));
 }

p_patfc= (GMPATF *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + pat_no + 1                             */
/*                                                                 !*/

  p_t->styp_pat = FAC_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = 1;                     /* Topological adress for  */
  p_t->iv_pat   = (short)pat_no;         /* current (this) patch    */
  p_t->us_pat   = F_UNDEF;               /* Start U on geom. patch  */
  p_t->ue_pat   = F_UNDEF;               /* End   U on geom. patch  */
  p_t->vs_pat   = F_UNDEF;               /* Start V on geom. patch  */
  p_t->ve_pat   = F_UNDEF;               /* End   V on geom. patch  */

  p_t->box_pat.xmin = 1.0;               /* BBOX  initiation        */
  p_t->box_pat.ymin = 2.0;               /*                         */
  p_t->box_pat.zmin = 3.0;               /*                         */
  p_t->box_pat.xmax = 4.0;               /*                         */
  p_t->box_pat.ymax = 5.0;               /*                         */
  p_t->box_pat.zmax = 6.0;               /*                         */
  p_t->box_pat.flag = -1;                /* Not calculated          */
    
  p_t->cone_pat.xdir = 1.0;              /* BCONE initiation        */
  p_t->cone_pat.ydir = 2.0;              /*                         */
  p_t->cone_pat.zdir = 3.0;              /*                         */
  p_t->cone_pat.ang  = 4.0;              /*                         */
  p_t->cone_pat.flag = -1;               /* Not calculated          */

/*!                                                                 */
/*   Retrieve pbox, pcone, us, ue, vs and ve from PBOUND table      */
/*                                                                 !*/

     pbox   = (p_pbtable+i_pat-1)->pbox;
     pcone  = (p_pbtable+i_pat-1)->pcone;

     us     = (p_pbtable+i_pat-1)->us;
     vs     = (p_pbtable+i_pat-1)->vs;
     ue     = (p_pbtable+i_pat-1)->ue;
     ve     = (p_pbtable+i_pat-1)->ve;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 i_pat %d pat_no %d p_t %d p_patfc %d\n",
     (int)i_pat, (int)pat_no, (int)p_t, (int)p_patfc ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 i_pat %d Area: us %8.4f ue %8.4f vs %8.4f ve %8.4f\n",
     (int)i_pat, us,  ue,  vs,  ve   ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 pcone xdir %8.5f ydir %8.5f zdir %8.5f ang %8.4f flag %d\n",
       pcone.xdir, pcone.ydir, pcone.zdir, pcone.ang , pcone.flag);
  fprintf(dbgfil(SURPAC),
  "sur170 pbox  xmin %8.5f ymin %8.5f zmin %8.5f  flag %d\n",
       pbox.xmin, pbox.ymin, pbox.zmin, pbox.flag);
  fprintf(dbgfil(SURPAC),
  "sur170 pbox  xmax %8.5f ymax %8.5f zmax %8.5f  flag %d\n",
       pbox.xmax, pbox.ymax, pbox.zmax, pbox.flag);
fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/*   Check if the patch can be approximated                         */
/*   Call of varkon_pat_chebound (sur914).                          */
/*   Goto _nxtpat if patch not is OK                                */
/*                                                                 !*/

   status=varkon_pat_chebound
        (p_sur,p_pat,(IRUNON*)&appdat, us,ue,vs,ve, 
         &pbox,&pcone,&sflag);
   if (status<0) 
     {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
     sprintf(errbuf,"sur914%%sur170");
     return(varkon_erpush("SU2943",errbuf));
     }

   if ( sflag == -99 )
       /* Surface patch is not OK */
       {
       p_patfc->defined = FALSE;  
       goto  _nxtpat;
       }

/*!                                                                 */
/*   Approximate the patch. Calculate four points.                  */
/*   Calls of varkon_sur_eval (sur210).                             */
/*                                                                 !*/

   icase = 3;


   status= varkon_sur_eval (p_sur,p_pat,icase,us,vs,&xyz);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur170 varkon_sur_eval (sur210) failed for us %f vs %f\n",us,vs); 
#endif
    sprintf(errbuf,"sur210%%sur170");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 us %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f pat_no %d\n",
     us, vs, xyz.r_x, xyz.r_y, xyz.r_z, (int)pat_no); 
  }
#endif

  p_patfc->p1.x_gm = xyz.r_x + offset*xyz.n_x; 
  p_patfc->p1.y_gm = xyz.r_y + offset*xyz.n_y; 
  p_patfc->p1.z_gm = xyz.r_z + offset*xyz.n_z; 
  p_patfc->u1      = us;
  p_patfc->v1      = vs;

  p1_e.x_gm = xyz.r_x + (offset+thick)*xyz.n_x; 
  p1_e.y_gm = xyz.r_y + (offset+thick)*xyz.n_y; 
  p1_e.z_gm = xyz.r_z + (offset+thick)*xyz.n_z; 
  u1_e      = us;
  v1_e      = vs;

   status= varkon_sur_eval (p_sur,p_pat,icase,ue,vs,&xyz);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur170 varkon_sur_eval (sur210) failed for ue %f vs %f\n",ue,vs); 
#endif
    sprintf(errbuf,"sur210%%sur170");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 ue %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, vs, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  p_patfc->p2.x_gm = xyz.r_x + offset*xyz.n_x; 
  p_patfc->p2.y_gm = xyz.r_y + offset*xyz.n_y; 
  p_patfc->p2.z_gm = xyz.r_z + offset*xyz.n_z; 

  p_patfc->u2      = ue;
  p_patfc->v2      = vs;
 
  p4_e.x_gm = xyz.r_x + (offset+thick)*xyz.n_x; 
  p4_e.y_gm = xyz.r_y + (offset+thick)*xyz.n_y; 
  p4_e.z_gm = xyz.r_z + (offset+thick)*xyz.n_z; 
  u4_e      = ue;
  v4_e      = vs;

   status= varkon_sur_eval (p_sur,p_pat,icase,ue,ve,&xyz);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur170 varkon_sur_eval (sur210) failed for ue %f ve %f\n",ue,ve); 
#endif
    sprintf(errbuf,"sur210%%sur170");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 ue %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, ve, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  p_patfc->p3.x_gm = xyz.r_x + offset*xyz.n_x; 
  p_patfc->p3.y_gm = xyz.r_y + offset*xyz.n_y; 
  p_patfc->p3.z_gm = xyz.r_z + offset*xyz.n_z; 
  p_patfc->u3      = ue;
  p_patfc->v3      = ve;
 
  p3_e.x_gm = xyz.r_x + (offset+thick)*xyz.n_x; 
  p3_e.y_gm = xyz.r_y + (offset+thick)*xyz.n_y; 
  p3_e.z_gm = xyz.r_z + (offset+thick)*xyz.n_z; 
  u3_e      = ue;
  v3_e      = ve;

   status= varkon_sur_eval (p_sur,p_pat,icase,us,ve,&xyz);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur170 varkon_sur_eval (sur210) failed for us %f ve %f\n",us,ve); 
#endif
    sprintf(errbuf,"sur210%%sur170");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 us %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us, ve, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  p_patfc->p4.x_gm = xyz.r_x + offset*xyz.n_x; 
  p_patfc->p4.y_gm = xyz.r_y + offset*xyz.n_y; 
  p_patfc->p4.z_gm = xyz.r_z + offset*xyz.n_z; 
  p_patfc->u4      = us;
  p_patfc->v4      = ve;

  p2_e.x_gm = xyz.r_x + (offset+thick)*xyz.n_x; 
  p2_e.y_gm = xyz.r_y + (offset+thick)*xyz.n_y; 
  p2_e.z_gm = xyz.r_z + (offset+thick)*xyz.n_z; 
  u2_e      = us;
  v2_e      = ve;

  p_patfc->triangles= TRUE;              /* Two triangles           */
  p_patfc->defined  = TRUE;              /* Patch data exists       */
  p_patfc->p_att    =    0;              /* Additional attributes   */

/* Start extra patch for acase= 11, acase= 12 and acase= 13         */
/* or acase = 4 when reversed facets shall be calculated. Note      */
/* that facet data is redefined below, i.e an additional patch      */
/* is not created (not very nice programmed ... )                   */

  if ( acase == 11 || acase == 12 || acase == 13 || acase == 4 ) 
  {
  if ( acase != 4 )
    {
    pat_no = pat_no + 1;                 /* Patch number 1, 2, 3, ..*/
    p_t = p_frst + pat_no - 1;           /* Ptr to  current patch   */
    p_gpat=DBcreate_patches(FAC_PAT, 1 );
    if ( p_gpat == 0 )
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur170");
      sprintf(errbuf, "(allocg)%%sur170");
      return(varkon_erpush("SU2993",errbuf));
      }
    p_patfc= (GMPATF *)p_gpat;           /* Current geometric patch */
    p_t->styp_pat = FAC_PAT;             /* Type of secondary patch */
    p_t->spek_c   = p_gpat;              /* Secondary patch (C ptr) */
    p_t->su_pat   = 0;                   /* Topological adress      */
    p_t->sv_pat   = 0;                   /* secondary patch not def.*/
    p_t->iu_pat   = 1;                   /* Topological adress for  */
    p_t->iv_pat   = (short)pat_no;       /* current (this) patch    */
    } /* End acase != 4 */

  p_patfc->p1 = p1_e;    
  p_patfc->p2 = p2_e;    
  p_patfc->p3 = p3_e;    
  p_patfc->p4 = p4_e;    

  p_patfc->u1 = u1_e;
  p_patfc->v1 = v1_e;
  p_patfc->u2 = u2_e;
  p_patfc->v2 = v2_e;
  p_patfc->u3 = u3_e;
  p_patfc->v3 = v3_e;
  p_patfc->u4 = u4_e;
  p_patfc->v4 = v4_e;

  p_patfc->triangles= TRUE;              /* Two triangles           */
  p_patfc->defined  = TRUE;              /* Patch data exists       */
  p_patfc->p_att    =    0;              /* Additional attributes   */
  }  /*   End  acase == 11 || acase == 12  || acase == 13 || acase=4*/


_nxtpat:;     /*! Label _nxtpat: The patch is not OK               !*/

    }                                    /* End loop PBOUND         */

/*!                                                                 */
/*     End loop all records in PBOUND table i_pat=1,2,....,pbn      */

/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

   v3free(p_pbtable, "sur170");

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur170 PBOUND memory is freed \n");
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 5. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/* Surface type and number of patches:                              */
          p_surout->typ_su= FAC_SUR;
          p_surout->nu_su = 1;  
          if ( acase == 11 || acase == 12 || acase == 13 ) 
            p_surout->nv_su = (short)(pbn*2); 
          else
            p_surout->nv_su = (short)pbn;
          p_surout->ngu_su = I_UNDEF; 
          p_surout->ngv_su = I_UNDEF;
          p_surout->ngseg_su[0] = 0;
          p_surout->ngseg_su[1] = 0;
          p_surout->ngseg_su[2] = 0;
          p_surout->ngseg_su[3] = 0;
          p_surout->ngseg_su[4] = 0;
          p_surout->ngseg_su[5] = 0;
          p_surout->pgseg_su[0] = DBNULL;
          p_surout->pgseg_su[1] = DBNULL;
          p_surout->pgseg_su[2] = DBNULL;
          p_surout->pgseg_su[3] = DBNULL;
          p_surout->pgseg_su[4] = DBNULL;
          p_surout->pgseg_su[5] = DBNULL;
            
/*       Surface box copied from the input surface:                 */
         p_surout->box_su =  p_sur->box_su;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
 fprintf(dbgfil(SURPAC),
   "sur170 box  Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
p_surout->box_su.xmin,p_surout->box_su.ymin,p_surout->box_su.zmin,
p_surout->box_su.xmax,p_surout->box_su.ymax,p_surout->box_su.zmax );
}
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur170 Exit***varkon_sur_facapprox ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
