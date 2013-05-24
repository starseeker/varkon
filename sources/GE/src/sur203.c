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
/*  Function: varkon_ratimport                       File: sur203.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads rational coefficients from a file            */
/*  and creates a VARKON rational bicubic surface.                  */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-05-28   Erased unused variables                            */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ratimport      Import a rational surface        */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_bound    * Bound. boxes & cones for surface           */
/* varkon_ini_gmsur    * Initialize DBSurf                          */
/* varkon_ini_gmpat    * Initialize DBPatch                         */
/* varkon_comptol      * Retrieve computer tolerance                */
/* varkon_erpush       * Error message to terminal                  */
/* varkon_ini_gmpatr   * Initialize GMPATR                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_ratimport  (sur203)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_ratimport (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *infile,       /* Surface file with rational cubic data   */
  DBSurf  *p_sur,        /* Surface                           (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_pat )     /* Alloc. area for topol. patch data (ptr) */
/*                                                                  */
/* Data to p_sur and pp_pat (topological and geometry patch data)   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!New-Page--------------------------------------------------------!*/
/*!----- Description of the data in the input file -----------------*/
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
/*  Surface: ...................                           (Line 1) */
/*  No patches:    nu    nv                                (Line 2) */
/*                                                                  */
/*                                                                  */
/*  The order of the coefficients for a patch:                      */
/*  -----------------------------------------                       */
/*                                                                  */
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

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   int    iu;            /* Loop index corresponding to u line      */
   int    iv;            /* Loop index corresponding to v line      */
   int    nu;            /* Number of patches in U direction        */
   int    nv;            /* Number of patches in V direction        */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATR *p;            /* Current geometric   patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   DBfloat  comptol;       /* Computer tolerance (accuracy)           */
   DBint  i_s;           /* Loop index surface patch record         */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   char   sur_name[133]; /* Surface name                            */
   char   rad[133];      /* One line in the F07 file                */
   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

   FILE   *f;            /* Pointer to the input ....F07 file       */

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
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur203 Enter ***** varkon_ratimport  *****\n");
  fprintf(dbgfil(SURPAC),"sur203 Input file  infile=%s\n",infile);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

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
p_sur->typ_su = RAT_SUR;                 /* Output type to header   */

/* Open file with rational bicubic surface data (C function fopen)  */
/* (Program error SU2993 for open failure)                          */
/*                                                                 !*/
if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "(open)%%varkon_ratimport  (sur203");
 return(varkon_erpush("SU2993",errbuf)); 
 }

/*!                                                                 */
/* Retrieve no of patches (nu,nv)                                   */
/*                                                                 !*/

fgets(sur_name,132,f);                   /* Read surface name       */

fgets(rad,132,f);                        /* Read line 2 in file     */
sscanf(rad,"%d%d",                       /* nu= no patches in U     */
 &nu  ,&nv  );                           /* nv= no patches in V     */

/*!                                                                 */
/* Number of patches to surface header (p_sur->nu_su p_sur->nv_su)  */
/*                                                                 !*/

p_sur->nu_su = (short)nu; 
p_sur->nv_su = (short)nv;

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,nu*nv))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_ratimport (sur203");
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
  "sur203 Surface name =%s\n",sur_name);
  fprintf(dbgfil(SURPAC),
  "sur203 No patches in U direction nu=%d\n",nu);
  fprintf(dbgfil(SURPAC),
  "sur203 No patches in V direction nv=%d\n",nv);
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 2. Create a varkon rational bicubic surface                      */
/* ___________________________________________                      */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

    p_t = p_frst + iu*nv + iv;           /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (rational) patch.   */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(RAT_PAT,1)) ==NULL )
 {
 sprintf(errbuf, "(allocg)%%varkon_ratimport  (sur203");
 return(varkon_erpush("SU2993",errbuf));
 }

p      = (GMPATR *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/* Initiate patch data in p   for Debug On                          */
/* Call of varkon_ini_gmpatr (sur766).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatr ( p );
#endif

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + iu*nv + iv                             */
/*                                                                 !*/

  p_t->styp_pat = RAT_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)(iu + 1);       /* Topological adress for  */
  p_t->iv_pat   = (short)(iv + 1);       /* current (this) patch    */
  p_t->us_pat = (DBfloat)iu+1.0;         /* Start U on geom. patch  */
  p_t->ue_pat = (DBfloat)iu+2.0-comptol; /* End   U on geom. patch  */
  p_t->vs_pat = (DBfloat)iv+1.0;         /* Start V on geom. patch  */
  p_t->ve_pat = (DBfloat)iv+2.0-comptol; /* End   V on geom. patch  */

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
/*  Retrieve patch data from file and                               */
/*  store data in patch area p                                      */
/*  (C functions fgets, sscanf)                                     */
/*                                                                 !*/

    fgets(rad,132,f);                    /* Read header for patch   */

    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* X coefficients (row 1)  */
    &p->a00x,&p->a01x,&p->a02x,&p->a03x);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* X coefficients (row 2)  */
    &p->a10x,&p->a11x,&p->a12x,&p->a13x);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* X coefficients (row 3)  */
    &p->a20x,&p->a21x,&p->a22x,&p->a23x);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* X coefficients (row 4)  */
    &p->a30x,&p->a31x,&p->a32x,&p->a33x);/*                         */

    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Y coefficients (row 1)  */
    &p->a00y,&p->a01y,&p->a02y,&p->a03y);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Y coefficients (row 2)  */
    &p->a10y,&p->a11y,&p->a12y,&p->a13y);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Y coefficients (row 3)  */
    &p->a20y,&p->a21y,&p->a22y,&p->a23y);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Y coefficients (row 4)  */
    &p->a30y,&p->a31y,&p->a32y,&p->a33y);/*                         */

    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Z coefficients (row 1)  */
    &p->a00z,&p->a01z,&p->a02z,&p->a03z);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Z coefficients (row 2)  */
    &p->a10z,&p->a11z,&p->a12z,&p->a13z);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Z coefficients (row 3)  */
    &p->a20z,&p->a21z,&p->a22z,&p->a23z);/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* Z coefficients (row 4)  */
    &p->a30z,&p->a31z,&p->a32z,&p->a33z);/*                         */

    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* w coefficients (row 1)  */
    &p->a00 ,&p->a01 ,&p->a02 ,&p->a03 );/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* w coefficients (row 2)  */
    &p->a10 ,&p->a11 ,&p->a12 ,&p->a13 );/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* w coefficients (row 3)  */
    &p->a20 ,&p->a21 ,&p->a22 ,&p->a23 );/*                         */
    fgets(rad,132,f);                    /*                         */
    sscanf(rad,"%lf%lf%lf%lf",           /* w coefficients (row 4)  */
    &p->a30 ,&p->a31 ,&p->a32 ,&p->a33 );/*                         */

/*!                                                                 */
    p->ofs_pat = 0.0;                    /* Offset for the patch    */
/*                                                                 !*/

    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

/*!                                                                 */
/* Close the file (C function fclose)                               */
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
        sprintf(errbuf,"varkon_sur_bound%%varkon_ratimport (sur203)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur203 No patches in direction U nu= %d and V nv= %d \n",nu,nv);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur203 Exit ***** varkon_ratimport ****** nu %d nv %d\n",nu,nv);
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
