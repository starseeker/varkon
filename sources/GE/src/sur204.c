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
/*  Function: varkon_nmgconsurf                      File: sur204.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads Consurf coefficients from a NMG F07 file     */
/*  and creates a VARKON Consurf surface identical to the           */
/*  surface in NMG.                                                 */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*                                                                  */
/*  Authors: Gunnar Liden                                           */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-31   Originally written                                 */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_nmgconsurf     From NMG Consurf to CON_SUR      */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur    * Initialize DBSurf                          */
/* varkon_ini_gmpat    * Initialize DBPatch                         */
/* varkon_ini_gmpatn   * Initialize GMPATN                          */
/* varkon_sur_bound    * Bound. boxes & cones for surface           */
/* varkon_comptol      * Retrieve computer tolerance                */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2861 = Patch iu= iv= of imported surface is not OK ( fctn .. ) */
/* SU2943 = Called function .. failed in varkon_nmgconsurf (sur204) */
/* SU2993 = Severe program error (  ) in varkon_nmgconsurf (sur204) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_nmgconsurf (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char    *infile,       /* Surface file with NMG F07 Consurf data  */
  DBSurf  *p_sur,        /* Output surface                    (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_pat )     /* Alloc. area for topol. patch data (ptr) */
                         /* This function allocates the area        */
/*                                                                  */
/* Data to p_sur and pp_pat (topological and geometry patch data)   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!----- Description of the data in the F07 file -------------------*/
/*                                                                  */
/*  The order of the patches:                                       */
/*  -------------------------                                       */
/*                                                                  */
/*  IU=1,IV=NV                              IU=NU,IV=NV             */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   ! 13  ! 14   ! 15   ! 16   ! 17  ! 18   !                      */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  7  !  8   !  9   ! 10   ! 11  ! 12   !                      */
/*   ----------------------------------------                       */
/*   !     !      !      !      !     !      !                      */
/*   !  1  !  2   !  3   !  4   !  5  !  6   !                      */
/*   ----------------------------------------                       */
/*                                                                  */
/*  IU=1,IV=1                                IU=NU,IV=1             */
/*                                                                  */
/* (Loop index iu= IU-1 and iv= IV-1 in program below)              */
/*                                                                  */
/*                                                                  */
/*  The order of data in the patch data header:                     */
/*  -------------------------------------------                     */
/*                                                                  */
/*  Surface name  .. ... .. .. .                           (Line A) */
/*                                                                  */
/*                                                                  */
/*  The order of the coefficients for a patch:                      */
/*  -----------------------------------------                       */
/*                                                                  */
/*   1  dum1 No_coeff Patch_no dum4 Sur_type dum6 dum7 nu-1 nv-1    */
/*   2   v1-x  v1-y  v1-z  w11  t11                                 */
/*   3   v2-x  v2-y  v2-z  w14  t14                                 */
/*   4   u1-x  u1-y  u1-z  w12  t12                                 */
/*   5   u2-x  u2-y  u2-z  w13  t13                                 */
/*   6  dv1-x dv1-y dv1-z  w21  t21                                 */
/*   7  dv2-x dv2-y dv2-z  w24  t24                                 */
/*   8  du1-x du1-y du1-z  w22  t22                                 */
/*   9  du2-x du2-y du2-z  w23  t23                                 */
/*  10   v3-x  v3-y  v3-z  w31  t31                                 */
/*  11   v4-x  v4-y  v4-z  w34  t34                                 */
/*  12   u3-x  u3-y  u3-z  w32  t32                                 */
/*  13   u4-x  u4-y  u4-z  w33  t33                                 */
/*  14  dv3-x dv3-y dv3-z  w41  t41                                 */
/*  15  dv4-x dv4-y dv4-z  w44  t44                                 */
/*  16  du3-x du3-y du3-z  w42  t42                                 */
/*  17  du4-x du4-y du4-z  w43  t43                                 */
/*  18   la_0  la_13  la_23  la_1  mu_0  mu_13  mu_23  mu_1         */
/*  19  dla_0 dla_13 dla_23 dla_1 dmu_0 dmu_13 dmu_23 dmu_1         */
/*  20   p1    p2    p3                                             */
/*                                                                  */
/*----------------------------------------------------------------- */

/*!----- Description of the VARKON data area for the surface -------*/
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
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   int    iu;            /* Loop index corresponding to u line      */
   int    iv;            /* Loop index corresponding to v line      */
   int    nu;            /* Number of patches in U direction        */
   int    nv;            /* Number of patches in V direction        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   int      i_s;         /* Loop index surface patch record         */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATN  *p_g;         /* Current geometric   patch         (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
   char    *p_tpat;      /* Allocated area topol. patch data  (ptr) */
   DBint    acc;         /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   int      ntot;        /* Total number of patches                 */
   int      nmgtype;     /* NMG entity type (consurf=1003)          */
   char     sur_name[29];/* Surface name                            */
   int      ncoeff;      /* Number of coefficients in patch (99)    */
   int      ipatch;      /* Patch number in surface 1, 2, .., ntot  */
   char     rad[133];    /* One line in the F07 file                */
   char     errbuf[80];  /* String for error message fctn erpush    */

   DBint    status;      /* Error code from a called function       */

   FILE    *f;           /* Pointer to the input ....F07 file       */

   int      dum1,dum4;   /* Dummy integers                          */
   int      dum6,dum7;   /* Dummy integers                          */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 Enter*varkon_nmgconsurf* infile %s \n", infile );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

  *pp_pat = NULL;        /* Initialize patch pointer                */

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initiate surface data in p_sur.                                  */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur);

/*!                                                                 */
/* Open NMG F07 file with consurf surface data (C function fopen)   */
/*                                                                 !*/
/* (Program error SU2993 for open failure)                          */

if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "(open)%%varkon_nmgconsurf (sur204");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Retrieve no of patches (nu,nv) and NMG surface type from F07     */
/* Check NMG surface type (shall be 1003).                          */
/*                                                                 !*/
fgets(sur_name,28,f);                    /* Read surface name       */
fgets(rad,132,f);                        /* Read the rest of line   */
sscanf(rad,"%d",&ntot);                  /* Total number of patches */

fgets(rad,132,f);                        /* Read line 1             */
sscanf(rad,"%d%d%d%d%d%d%d%d%d",         /* NMG surface type and    */
 &dum1,&ncoeff,&ipatch,&dum4,&nmgtype,   /* number of U and V lines */
 &dum6,&dum7,&nu,&nv);                   /* (patch no and no coeff) */

if ( nmgtype != 1003 )                   /* Check that the surface  */
 {                                       /* type (code) is 1003     */
 sprintf(errbuf,                         /* Error SU2993 for type   */
 "(type)%%varkon_nmgconsurf (sur204");   /* failure                 */
#ifdef LNAME                             /*                         */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
#endif                                   /*                         */
#ifdef SNAME                             /*                         */
 return(erpush("SU2993",errbuf));        /*                         */
#endif                                   /*                         */
 }

nu = nu - 1;                             /* Number of U patches nu  */
nv = nv - 1;                             /* Number of V patches nv  */

/*!                                                                 */
/* Surface type and number of patches in U and V to p_sur           */
/*                                                                 !*/

p_sur->typ_su = CON_SUR;                 /* Output type to header   */
p_sur->nu_su = (short)nu; 
p_sur->nv_su = (short)nv;

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,nu*nv))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_nmgconsurf (sur204");
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

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 Surface name =%s\n",sur_name);
  fprintf(dbgfil(SURPAC),
  "sur204 Total number of patches ntot=%d\n",ntot);
  fprintf(dbgfil(SURPAC),
  "sur204 Number of coefficients (48) =%d\n",ncoeff);
  fprintf(dbgfil(SURPAC),
  "sur204 Patch number in NMG         =%d\n",ipatch);
  fprintf(dbgfil(SURPAC),
  "sur204 NMG surface type (code)     =%d\n",nmgtype);
  fprintf(dbgfil(SURPAC),
  "sur204 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),
  "sur204 No patches in V direction nv=%d\n",nv);
  fprintf(dbgfil(SURPAC),
  "sur204 Pointer to patch 1    p_frst=%d\n",(int)p_frst);
  fprintf(dbgfil(SURPAC),
  "sur204 Output patch ptr     *pp_pat=%d\n",(int)*pp_pat);
  fprintf(dbgfil(SURPAC),
  "sur204 sizeof(GMPATN) %d\n",sizeof(GMPATN) );
  fprintf(dbgfil(SURPAC),
  "sur204 sizeof(GMPATL) %d\n",sizeof(GMPATL) );
  fprintf(dbgfil(SURPAC),
  "sur204 sizeof(GMPATR) %d\n",sizeof(GMPATR) );
  fprintf(dbgfil(SURPAC),
  "sur204 sizeof(GMPATF) %d\n",sizeof(GMPATF) );
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 2. Create a Varkon Consurf surface                               */
/* __________________________________                               */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

    if ( iu+iv > 0 ) fgets(rad,132,f);   /* Read line 1 for patch   */
                                         /* (except for patch 1)    */

    p_t = p_frst + iu*nv + iv;           /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (consurf) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(CON_PAT,1)) ==NULL)
 {
 sprintf(errbuf, "(allocg)%%varkon_nmgconsurf (sur204");
 return(varkon_erpush("SU2993",errbuf));
 }

p_g    = (GMPATN *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/* Initialize patch data in p_g for Debug On                        */
/* Call of varkon_ini_gmpatn (sur761).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatn (p_g);
#endif

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + iu*nv + iv                             */
/*                                                                 !*/

  p_t->styp_pat = CON_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)(iu + 1);       /* Topological adress for  */
  p_t->iv_pat   = (short)(iv + 1);       /* current (this) patch    */
  p_t->us_pat = (DBfloat)iu+1.0;         /* Start U on geom. patch  */
  p_t->ue_pat = (DBfloat)iu+2.0-comptol; /* End   U on geom. patch  */
  p_t->vs_pat = (DBfloat)iv+1.0;         /* Start V on geom. patch  */
  p_t->ve_pat = (DBfloat)iv+2.0-comptol; /* End   V on geom. patch  */

  p_t->box_pat.xmin = 1.0;               /* BBOX  initialization    */
  p_t->box_pat.ymin = 2.0;               /*                         */
  p_t->box_pat.zmin = 3.0;               /*                         */
  p_t->box_pat.xmax = 4.0;               /*                         */
  p_t->box_pat.ymax = 5.0;               /*                         */
  p_t->box_pat.zmax = 6.0;               /*                         */
  p_t->box_pat.flag = -1;                /* Not calculated          */
    
  p_t->cone_pat.xdir = 1.0;              /* BCONE initialization    */
  p_t->cone_pat.ydir = 2.0;              /*                         */
  p_t->cone_pat.zdir = 3.0;              /*                         */
  p_t->cone_pat.ang  = 4.0;              /*                         */
  p_t->cone_pat.flag = -1;               /* Not calculated          */

/*!                                                                 */
/*  Retrieve patch data from F07 file and store data in area        */
/*  allocated for the geometric patch.                              */
/*  Calls of C functions fgets and sscanf. Let patch offset = 0.    */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 Patch iu %d iv %d p_t %d p_g %d p_gpat %d \n", 
     iu , iv, (int)p_t,(int)p_g,(int)p_gpat);
 fflush(dbgfil(SURPAC)); 
  }
#endif


    fgets(rad,132,f);                    /* Read line 2 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->v1.x_gm,&p_g->v1.y_gm,&p_g->v1.z_gm,  
     &p_g->w11 ,&p_g->t11);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  v1 %12.2f %12.2f %12.2f w11 %6.4f t11 %6.4f\n", 
     p_g->v1.x_gm,p_g->v1.y_gm,p_g->v1.z_gm, p_g->w11 ,p_g->t11);
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

    fgets(rad,132,f);                    /* Read line 3 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->v2.x_gm,&p_g->v2.y_gm,&p_g->v2.z_gm,  
     &p_g->w14 ,&p_g->t14 );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  v2 %12.2f %12.2f %12.2f w14 %6.4f t14 %6.4f\n", 
      p_g->v2.x_gm, p_g->v2.y_gm, p_g->v2.z_gm,  p_g->w14 , p_g->t14 );           
 fflush(dbgfil(SURPAC)); 
  }
#endif

    fgets(rad,132,f);                    /* Read line 4 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->u1.x_gm,&p_g->u1.y_gm,&p_g->u1.z_gm,  
     &p_g->w12 ,&p_g->t12);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  u1 %12.2f %12.2f %12.2f w12 %6.4f t12 %6.4f\n", 
      p_g->u1.x_gm, p_g->u1.y_gm, p_g->u1.z_gm,  p_g->w12 , p_g->t12);
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 5 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->u2.x_gm,&p_g->u2.y_gm,&p_g->u2.z_gm,  
     &p_g->w13 ,&p_g->t13 );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  u2 %12.2f %12.2f %12.2f w13 %6.4f t13 %6.4f\n", 
      p_g->u2.x_gm, p_g->u2.y_gm, p_g->u2.z_gm,  p_g->w13 , p_g->t13 );           
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 6 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->dv1.x_gm,&p_g->dv1.y_gm,&p_g->dv1.z_gm,  
     &p_g->w21 ,&p_g->t21);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 dv1 %12.2f %12.2f %12.2f w21 %6.4f t21 %6.4f\n", 
      p_g->dv1.x_gm, p_g->dv1.y_gm, p_g->dv1.z_gm, p_g->w21 , p_g->t21);
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 7 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->dv2.x_gm,&p_g->dv2.y_gm,&p_g->dv2.z_gm,  
     &p_g->w24 ,&p_g->t24 );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 dv2 %12.2f %12.2f %12.2f w24 %6.4f t24 %6.4f\n", 
      p_g->dv2.x_gm, p_g->dv2.y_gm, p_g->dv2.z_gm, p_g->w24 , p_g->t24 );           
 fflush(dbgfil(SURPAC)); 
  }
#endif

    fgets(rad,132,f);                    /* Read line 8 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->du1.x_gm,&p_g->du1.y_gm,&p_g->du1.z_gm,  
     &p_g->w22 ,&p_g->t22);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 du1 %12.2f %12.2f %12.2f w22 %6.4f t22 %6.4f\n", 
      p_g->du1.x_gm, p_g->du1.y_gm, p_g->du1.z_gm, p_g->w22 , p_g->t22);
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 9 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->du2.x_gm,&p_g->du2.y_gm,&p_g->du2.z_gm,  
     &p_g->w23 ,&p_g->t23 );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 du2 %12.2f %12.2f %12.2f w23 %6.4f t23 %6.4f\n", 
      p_g->du2.x_gm, p_g->du2.y_gm, p_g->du2.z_gm, p_g->w23 , p_g->t23 );           
 fflush(dbgfil(SURPAC));
  }
#endif

    fgets(rad,132,f);                    /* Read line 10 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->v3.x_gm,&p_g->v3.y_gm,&p_g->v3.z_gm,  
     &p_g->w31 ,&p_g->t31);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  v3 %12.2f %12.2f %12.2f w31 %6.4f t31 %6.4f\n", 
      p_g->v3.x_gm, p_g->v3.y_gm, p_g->v3.z_gm,  p_g->w31 , p_g->t31);
 fflush(dbgfil(SURPAC)); 
  }
#endif


    fgets(rad,132,f);                    /* Read line 11 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->v4.x_gm,&p_g->v4.y_gm,&p_g->v4.z_gm,  
     &p_g->w34 ,&p_g->t34 );           


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  v4 %12.2f %12.2f %12.2f w34 %6.4f t34 %6.4f\n", 
      p_g->v4.x_gm, p_g->v4.y_gm, p_g->v4.z_gm,  p_g->w34 , p_g->t34 );           
 fflush(dbgfil(SURPAC)); 
  }
#endif

    fgets(rad,132,f);                    /* Read line 12 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->u3.x_gm,&p_g->u3.y_gm,&p_g->u3.z_gm,  
     &p_g->w32 ,&p_g->t32);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  u3 %12.2f %12.2f %12.2f w32 %6.4f t32 %6.4f\n", 
      p_g->u3.x_gm, p_g->u3.y_gm, p_g->u3.z_gm, p_g->w32 , p_g->t32);
 fflush(dbgfil(SURPAC)); 
  }
#endif



    fgets(rad,132,f);                    /* Read line 13 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->u4.x_gm,&p_g->u4.y_gm,&p_g->u4.z_gm,  
     &p_g->w33 ,&p_g->t33 );           


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  u4 %12.2f %12.2f %12.2f w33 %6.4f t33 %6.4f\n", 
      p_g->u4.x_gm, p_g->u4.y_gm, p_g->u4.z_gm, p_g->w33 , p_g->t33 );           
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 14 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->dv3.x_gm,&p_g->dv3.y_gm,&p_g->dv3.z_gm,  
     &p_g->w41 ,&p_g->t41);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 dv3 %12.2f %12.2f %12.2f w41 %6.4f t41 %6.4f\n", 
      p_g->dv3.x_gm, p_g->dv3.y_gm, p_g->dv3.z_gm,  p_g->w41 , p_g->t41);
 fflush(dbgfil(SURPAC));
  }
#endif


    fgets(rad,132,f);                    /* Read line 15 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->dv4.x_gm,&p_g->dv4.y_gm,&p_g->dv4.z_gm,  
     &p_g->w44 ,&p_g->t44 );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 dv4 %12.2f %12.2f %12.2f w44 %6.4f t44 %6.4f\n", 
      p_g->dv4.x_gm, p_g->dv4.y_gm, p_g->dv4.z_gm, p_g->w44 , p_g->t44 );           
 fflush(dbgfil(SURPAC));
  }
#endif

    fgets(rad,132,f);                    /* Read line 16 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->du3.x_gm,&p_g->du3.y_gm,&p_g->du3.z_gm,  
     &p_g->w42 ,&p_g->t42);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 du3 %12.2f %12.2f %12.2f w42 %6.4f t42 %6.4f\n", 
      p_g->du3.x_gm, p_g->du3.y_gm, p_g->du3.z_gm, p_g->w42 , p_g->t42);
 fflush(dbgfil(SURPAC)); 
  }
#endif


    fgets(rad,132,f);                    /* Read line 17 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf",    
     &p_g->du4.x_gm,&p_g->du4.y_gm,&p_g->du4.z_gm,  
     &p_g->w43 ,&p_g->t43 );           


#ifdef DEBUG
if ( dbglev(SURPAC) == 2  && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 du4 %12.2f %12.2f %12.2f w43 %6.4f t43 %6.4f\n", 
      p_g->du4.x_gm, p_g->du4.y_gm, p_g->du4.z_gm, p_g->w43 , p_g->t43 );           
 fflush(dbgfil(SURPAC)); 
  }
#endif

    fgets(rad,132,f);                    /* Read line 18 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf%lf%lf",    
     &p_g->lambda_0,&p_g->lambda_13,&p_g->lambda_23, &p_g->lambda_1,
     &p_g->mu_0 ,   &p_g->mu_13 ,   &p_g->mu_23 ,    &p_g->mu_1   );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  lambda %8.5f %8.5f %8.5f %8.5f  mu %8.5f %8.5f %8.5f %8.5f\n", 
      p_g->lambda_0, p_g->lambda_13, p_g->lambda_23,  p_g->lambda_1,
      p_g->mu_0 ,    p_g->mu_13 ,    p_g->mu_23 ,     p_g->mu_1   );           
 fflush(dbgfil(SURPAC)); 
  }
#endif

    fgets(rad,132,f);                    /* Read line 19 in patch   */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf%lf%lf",    
     &p_g->dlambda_0,&p_g->dlambda_13,&p_g->dlambda_23, &p_g->dlambda_1,
     &p_g->dmu_0 ,   &p_g->dmu_13 ,   &p_g->dmu_23 ,    &p_g->dmu_1   );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && iu == 0 && iv == 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 dlambda %8.5f %8.5f %8.5f %8.5f dmu %8.5f %8.5f %8.5f %8.5f\n", 
      p_g->dlambda_0, p_g->dlambda_13, p_g->dlambda_23,  p_g->dlambda_1,
      p_g->dmu_0 ,    p_g->dmu_13 ,    p_g->dmu_23 ,     p_g->dmu_1   );           
 fflush(dbgfil(SURPAC));
  }
#endif

    fgets(rad,132,f);                    /* Read line 20 in patch   */
    sscanf(rad,"%lf%lf%lf",    
     &p_g->p1,  &p_g->p2, &p_g->p3  );           

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  p1 %12.2f p2 %12.2f p3 %12.2f \n", 
      p_g->p1,   p_g->p2,  p_g->p3  );           
 fflush(dbgfil(SURPAC)); 
  }
#endif


    p_g->cl_p.x_gm = F_UNDEF;     /* Core Line Point  X coordinate   */
    p_g->cl_p.y_gm = F_UNDEF;     /* Core Line Point  Y coordinate   */
    p_g->cl_p.z_gm = F_UNDEF;     /* Core Line Point  Z coordinate   */
    p_g->cl_v.x_gm = F_UNDEF;     /* Core Line Vector X component    */
    p_g->cl_v.y_gm = F_UNDEF;     /* Core Line Vector Y component    */
    p_g->cl_v.z_gm = F_UNDEF;     /* Core Line Vector Z component    */
    p_g->ofs_pat   = 0.0;         /* Offset for the patch            */

    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

/*!                                                                 */
/* Close the NMG F07 file (C function fclose)                       */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204  All data retrieved from file. Surface is created \n");
 fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 3. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_sur,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_nmgconsurf(sur204)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur204 Exit*varkon_nmgconsurf* No patches nu= %d nv= %d \n",nu,nv);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
