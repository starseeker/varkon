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
/*  Function: varkon_nmgbicubic                      File: sur201.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads bicubic coefficients from a NMG F07 file     */
/*  and creates a VARKON (non-rational) bicubic surface.            */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*                                                                  */
/*  Authors: Gunnar Liden & Johan Kjellander                        */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-05-28   Erase of unused variables                          */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_nmgbicubic     From NMG bicubic to CUB_SUR      */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur    * Initialize DBSurf                          */
/* varkon_ini_gmpat    * Initialize DBPatch                         */
/* varkon_sur_bound    * Bound. boxes & cones for surface           */
/* varkon_comptol      * Retrieve computer tolerance                */
/* varkon_erpush       * Error message to terminal                  */
/* varkon_ini_gmpatc   * Initialize GMPATC                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2861 = Patch iu= iv= of imported surface is not OK ( fctn .. ) */
/* SU2943 = Called function .. failed in varkon_nmgbicubic (sur201) */
/* SU2993 = Severe program error (  ) in varkon_nmgbicubic (sur201) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_nmgbicubic (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char     *infile,      /* Surface file with NMG F07 bicubic data  */
  DBSurf   *p_sur,       /* Output surface                    (ptr) */
  DBPatch **pp_pat )     /* Alloc. area for topol. patch data (ptr) */
                         /* This function allocates the area        */
/*                                                                  */
/* Out:                                                             */
/* Data to p_sur and pp_pat (topology and geometry patch data)      */
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
/*  Surface name  .. ... .. .. .                           (Line 1) */
/*  dum1 No coeff. Patch no. dum4 Type dum6 dum7 nu-1 nv-1 (Line 2) */
/*       ... empty line (no data) ...                      (Line 3) */
/*                                                                  */
/*                                                                  */
/*  The order of the coefficients for a patch:                      */
/*  -----------------------------------------                       */
/*                                                                  */
/*  Line Coefficients   (    for    ) Coefficients   (    for    )  */
/*                                                                  */
/*   1   a00x a00y a00z (1.0  * 1.0 ) a10x a10y a10z (U    * 1.0 )  */
/*   2   a20x a20y a20z (U**2 * 1.0 ) a30x a30y a30z (U**3 * 1.0 )  */
/*   3   a01x a01y a01z (1.0  * V   ) a11x a11y a11z (U    * V   )  */
/*   4   a21x a21y a21z (U**2 * V   ) a31x a31y a31z (U**3 * V   )  */
/*   5   a02x a02y a02z (1.0  * V**2) a12x a12y a12z (U    * V**2)  */
/*   6   a22x a22y a22z (U**2 * V**2) a32x a32y a32z (U**3 * V**2)  */
/*   7   a03x a03y a03z (1.0  * V**3) a13x a13y a13z (U    * V**3)  */
/*   8   a23x a23y a23z (U**2 * V**3) a33x a33y a34z (U**3 * V**3)  */
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
   DBint  iu;            /* Loop index corresponding to u line      */
   DBint  iv;            /* Loop index corresponding to v line      */
   int    nu;            /* Number of patches in U direction        */
   int    nv;            /* Number of patches in V direction        */
                         /* Note that nu,nv must be int for sscanf  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   DBint    i_s;         /* Loop index surface patch record         */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_g;         /* Current geometric   patch         (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
   char    *p_tpat;      /* Allocated area topol. patch data  (ptr) */
   DBint    acc;         /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   int      ntot;        /* Total number of patches                 */
   int      nmgtype;     /* NMG entity type (bicubic=1001)          */
   char     sur_name[29];/* Surface name                            */
   int      ncoeff;      /* Number of coefficients in patch (48)    */
   int      ipatch;      /* Patch number in surface 1, 2, .., ntot  */
   char     rad[133];    /* One line in the F07 file                */
   char     errbuf[80];  /* String for error message fctn erpush    */

   DBint    status;      /* Error code from a called function       */

   FILE     *f;          /* Pointer to the input ....F07 file       */

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
  "sur201 Enter*varkon_nmgbicubic* infile %s \n", infile );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initialize surface data in p_sur.                                */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur);

/*!                                                                 */
/* Open NMG F07 file with bicubic surface data (C function fopen)   */
/*                                                                 !*/
/* (Program error SU2993 for open failure)                          */

if ( (f= fopen(infile,"r")) == NULL )    /* Open of input file      */
 {
 sprintf(errbuf, "(open)%%varkon_nmgbicubic (sur201");
 return(varkon_erpush("SU2993",errbuf));
 }                                       /*                         */

/*!                                                                 */
/* Retrieve no of patches (nu,nv) and NMG surface type from F07     */
/* Check NMG surface type (shall be 1001).                          */
/*                                                                 !*/
fgets(sur_name,28,f);                    /* Read surface name       */
fgets(rad,132,f);                        /* Read the rest of line 1 */
sscanf(rad,"%d",&ntot);                  /* Total number of patches */

fgets(rad,132,f);                        /* Read line 2             */
sscanf(rad,"%d%d%d%d%d%d%d%d%d",         /* NMG surface type and    */
 &dum1,&ncoeff,&ipatch,&dum4,&nmgtype,   /* number of U and V lines */
 &dum6,&dum7,&nu,&nv);                   /* (patch no and no coeff) */

if ( nmgtype != 1001 )                   /* Check that the surface  */
 {                                       /* type (code) is 1001     */
 sprintf(errbuf,                         /* Error SU2993 for type   */
 "(type)%%varkon_nmgbicubic (sur201");   /* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }

nu = nu - 1;                             /* Number of U patches nu  */
nv = nv - 1;                             /* Number of V patches nv  */

/*!                                                                 */
/* Surface type and number of patches in U and V to p_sur           */
/*                                                                 !*/

p_sur->typ_su = CUB_SUR;                 /* Output type to header   */
p_sur->nu_su = (short)nu; 
p_sur->nv_su = (short)nv;

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,nu*nv))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_nmgbicubic (sur201");
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
  "sur201 Surface name =%s\n",sur_name);
  fprintf(dbgfil(SURPAC),
  "sur201 Total number of patches ntot=%d\n",ntot);
  fprintf(dbgfil(SURPAC),
  "sur201 Number of coefficients (48) =%d\n",ncoeff);
  fprintf(dbgfil(SURPAC),
  "sur201 Patch number in NMG         =%d\n",ipatch);
  fprintf(dbgfil(SURPAC),
  "sur201 NMG surface type (code)     =%d\n",nmgtype);
  fprintf(dbgfil(SURPAC),
  "sur201 No patches in U direction nu=%d\n",(int)nu);
  fprintf(dbgfil(SURPAC),
  "sur201 No patches in V direction nv=%d\n",(int)nv);
  fprintf(dbgfil(SURPAC),
  "sur201 Pointer to patch 1    p_frst=%d\n",(int)p_frst);
  fprintf(dbgfil(SURPAC),
  "sur201 Output patch ptr     *pp_pat=%d\n",(int)*pp_pat);
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 2. Create a varkon bicubic surface                               */
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
/* Dynamic allocation of area for one geometric (bicubic) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(CUB_PAT,1)) ==NULL)
 {
 sprintf(errbuf, "(allocg)%%varkon_nmgbicubic (sur201");
 return(varkon_erpush("SU2993",errbuf));
 }

p_g    = (GMPATC *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/* Initialize patch data in p_g for Debug On                        */
/* Call of varkon_ini_gmpatc (sur767).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatc (p_g);
#endif

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + iu*nv + iv                             */
/*                                                                 !*/

  p_t->styp_pat = CUB_PAT;               /* Type of secondary patch */
  p_t->spek_c = p_gpat;                  /* Secondary patch (C ptr) */
  p_t->su_pat = 0;                       /* Topological adress      */
  p_t->sv_pat = 0;                       /* secondary patch not def.*/
  p_t->iu_pat = (short)(iu + 1);         /* Topological adress for  */
  p_t->iv_pat = (short)(iv + 1);         /* current (this) patch    */
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

    fgets(rad,132,f);                    /* Read line 1 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients:           */
     &p_g->a00x,&p_g->a00y,&p_g->a00z,   /* a00 * 1     (for x y z) */
     &p_g->a10x,&p_g->a10y,&p_g->a10z);  /* a10 * U                 */
    fgets(rad,132,f);                    /* Read line 2 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients:           */
     &p_g->a20x,&p_g->a20y,&p_g->a20z,   /* a20 * U**2              */
     &p_g->a30x,&p_g->a30y,&p_g->a30z);  /* a30 * U**3              */
    fgets(rad,132,f);                    /* Read line 3 in patch    */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients:           */
     &p_g->a01x,&p_g->a01y,&p_g->a01z,   /* a01 * 1     *  V        */
     &p_g->a11x,&p_g->a11y,&p_g->a11z);  /* a11 * U     *  V        */
    fgets(rad,132,f);                    /* Read line 4             */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients:           */
     &p_g->a21x,&p_g->a21y,&p_g->a21z,   /* a21 * U**2  *  V        */
     &p_g->a31x,&p_g->a31y,&p_g->a31z);  /* a31 * U**3  *  V        */
    fgets(rad,132,f);                    /* Read line 5             */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients            */
     &p_g->a02x,&p_g->a02y,&p_g->a02z,   /* a02 * 1     *  V**2     */
     &p_g->a12x,&p_g->a12y,&p_g->a12z);  /* a12 * U     *  V**2     */
    fgets(rad,132,f);                    /* Read line 6             */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients            */
     &p_g->a22x,&p_g->a22y,&p_g->a22z,   /* a22 * U**2  *  V**2     */
     &p_g->a32x,&p_g->a32y,&p_g->a32z);  /* a32 * U**3  *  V**2     */
    fgets(rad,132,f);                    /* Read line 7             */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients            */
     &p_g->a03x,&p_g->a03y,&p_g->a03z,   /* a03 * 1     *  V**3     */
     &p_g->a13x,&p_g->a13y,&p_g->a13z);  /* a13 * U     *  V**3     */
    fgets(rad,132,f);                    /* Read line 8             */
    sscanf(rad,"%lf%lf%lf%lf%lf%lf",     /* Coefficients            */
    &p_g->a23x,&p_g->a23y,&p_g->a23z,    /* a23 * U**2  *  V**3     */
    &p_g->a33x,&p_g->a33y,&p_g->a33z);   /* a33 * U**3  *  V**3     */

    p_g->ofs_pat = 0.0;                  /* Offset for the patch    */

    fgets(rad,132,f);                    /* Read the empty line     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur201 Patch iu %d iv %d p_t %d p_g %d p_gpat %d \n", 
     (int)iu , (int)iv, (int)p_t,(int)p_g,(int)p_gpat);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur201 us_pat %8.2f vs_pat %8.2f ue_pat %8.2f ve_pat %8.2f\n", 
      p_t->us_pat, p_t->vs_pat, p_t->ue_pat, p_t->ve_pat  );
  fflush(dbgfil(SURPAC)); 
  }
#endif

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
        sprintf(errbuf,"varkon_sur_bound%%varkon_nmgbicubic(sur201)");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur201 No patches in direction U nu= %d and V nv= %d \n",nu,nv);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur201 Exit*varkon_nmgbicubic* No patches nu= %d nv= %d \n",nu,nv);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
