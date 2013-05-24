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
/*  Function: varkon_nmglftsurf                      File: sur202.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads Consurf coefficients from a NMG F07 file     */
/*  and creates a VARKON conic lofting surface.                     */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1995-01-28   Debug (length cur_name must be 28 in F07 file)     */
/*  1996-05-28   Erase of unused variables                          */
/*  1997-02-09   Elimination of compilation warning                 */
/*  1999-12-18   Free source code modifications+_nmglftsurf         */
/*                                                                  */
/******************************************************************!*/


/*-------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_nmglftsurf     From NMG Consurf to LFT_SUR      */
/*                                                              */
/*--------------------------------------------------------------*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol        * Retrieve computer tolerance              */
/* varkon_ini_gmsur      * Initialize DBSurf                        */
/* varkon_ini_gmpat      * Initialize DBPatch                       */
/* varkon_ini_gmpatl     * Initialize GMPATL                        */
/* GE133                 * Rational cubic with P-value              */
/* GE200                 * Create a line                            */
/* varkon_cur_reparam    * Reparam. a rational segment              */
/* varkon_sur_bound      * Bounding box & cone for surface          */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short crelims();       /* Create start limit   crv segment   */
static short crelime();       /* Create end   limit   crv segment   */
static short cretans();       /* Create start tangent crv segment   */
static short cretane();       /* Create end   tangent crv segment   */
static short crepval();       /* Create P value       crv segment   */
static short crevlim();       /* Create V start and end limit lines */
static short crespine();      /* Create spine line (curve) segment  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  NOT_USED_FOR_THE_MOMENT
static short cremidc();       /* Create mid           crv segment   */
#endif
/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static GMPATL  *p;           /* Current geometric   patch     (ptr) */
static DBPatch *p_t;         /* Current topological patch     (ptr) */
static int     pat_no;       /* Patch number                        */
static int     iu;           /* Loop index correspond. to u line    */
static int     iv;           /* Loop index correspond. to v line    */
static int     nu;           /* Number of patches in U direction    */
static int     nv;           /* Number of patches in V direction    */
static DBfloat uplim[100][3];/* Limit   points   in prev U section  */
static DBfloat uplimt[100][3];/* Limit   tangents in prev U section */
static DBfloat uptan[100][3];/* Tangent points   in prev U section  */
static DBfloat uptant[100][3];/* Tangent tangents in prev U section */
static DBfloat uplimp[100];  /* P-values for limit   segments       */
static DBfloat uptanp[100];  /* P-values for tangent segments       */
static DBfloat upsecp[100];  /* P-values    for sections            */
static DBfloat upsecdp[100]; /* dPds-values for sections            */
static DBfloat uclim[100][3];/* Limit   points   in curr U section  */
static DBfloat uclimt[100][3];/* Limit   tangents in curr U section */
static DBfloat uctan[100][3];/* Tangent points   in curr U section  */
static DBfloat uctant[100][3];/* Tangent tangents in curr U section */
static DBfloat uclimp[100];  /* P-values for limit   segments curr  */
static DBfloat uctanp[100];  /* P-values for tangent segments curr  */
static DBfloat ucsecp[100];  /* P-values    for sections      curr  */
static DBfloat ucsecdp[100]; /* dPds-values for sections      curr  */
static DBfloat spine[2][3];  /* Spine (straight line is assumed)    */
static DBVector points[4];   /* points[0]    = start point          */
                             /* points[1]    = start tangent point  */
                             /* points[2]    = end   tangent point  */
                             /* points[3]    = end   point          */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function   () failed in varkon_nmglftsurf        */
/* SU2973 = Internal function () failed in varkon_nmglftsurf        */
/* SU2993 = Severe program error ( ) in varkon_nmglftsurf (sur202)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_nmglftsurf (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *infile,       /* Surface file with F07 Consurf sections  */
  DBSurf  *p_sur,        /* Output surface                    (ptr) */
  DBPatch **pp_pat )     /* Alloc. area for topol. patch data (ptr) */
                         /* This function allocates the area        */

/* Out:                                                             */
/* Data to p_sur and pp_pat (topological and geometry patch data)   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!----- Description of the data in the F07 file -------------------*/
/*                                                                  */
/* Data is described with an example of a Consurf section line:     */
/*                                                                  */
/* LINE  MOG1/U1                                                    */
/*    SECTION  5 POINTS                                             */
/*    NORMAL  4700.0000  0.0000 2260.0000 1.00000 0.00000 0.00000   */
/*      1.1 4700.0000    0.0000 3686.0777 0.98012 0.00000 0.19836   */
/*      1.2 4700.0000  214.9970 3686.0777 0.97700 0.07978 0.19773   */
/*      1.3 PVALUES  0.500000  0.50000  0.519118570  0.0000144050   */
/*      2.1 4700.0000  312.1706 3670.9325 0.97512 0.08659 0.20403   */
/*      2.2 4700.0000  548.7431 3634.0609 0.97234 0.03694 0.23060   */
/*      2.3 PVALUES  0.500000  0.50000  0.691330320 -0.0000295300   */
/*      3.1 4700.0000  548.7431 2558.0431 0.93040 0.03535 0.36483   */
/*      3.2 4700.0000  548.7431 1824.3740 0.99257 0.03771 0.11567   */
/*      3.3 PVALUES  0.500000  0.50000  0.500000000  0.0000000000   */
/*      4.1 4700.0000  548.7431 2308.8454 0.99702 0.03788 0.06713   */
/*      4.2 4700.0000  548.7431 1824.3740 0.99257 0.03771 0.11567   */
/*      4.3 PVALUES  0.500000  0.50000  0.414213562  0.0000000000   */
/*      5.1 4700.0000    0.0000 1824.3740 0.99327 0.00000 0.11575   */
/*   R  5.2 4700.0000  548.7431 1824.3740 0.99257 0.03771 0.11567   */
/*      5.3 PVALUES  0.500000  0.50000                              */
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
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   DBint  i_s;           /* Loop index surface patch record         */
   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
   DBint  status;        /* Error code from a called fctn           */
   int    nv_check;      /* For the check of data in the F07 file   */
   char   cur_name[29];  /* Section curve name                      */
   char   lin_type[6];   /* The word LINE    for check of F07 file  */
   char   sec_type[12];  /* The word SECTION for check of F07 file  */
   char   poi_type[11];  /* The point number 1.1 1.2 2.1 .........  */
   char   pva_type[18];  /* The point number 1.3 2.3 3.3 .........  */
   char   nor_type[11];  /* The word NORMAL                         */
   char   rad[133];      /* One line in the F07 file                */
   char   errbuf[80];    /* String for error message fctn erpush    */

   FILE   *f;            /* Pointer to the input ....F07 file       */

   int    ix1,ix2;       /* Temporarely used loop indeces           */
   double k1,k2,k3;      /* Dummy reals                             */
   double k4,k5,k6;      /* Dummy reals                             */

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
  "sur202 Enter *** varkon_nmglftsurf***Convert Consurf to LFT_SUR\n");
  fprintf(dbgfil(SURPAC),"sur202 Input file  infile=  %s\n",infile);
  fflush(dbgfil(SURPAC));    /* T|m buffert */
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
p_sur->typ_su = LFT_SUR;                  /* Output type to header   */

/* Open NMG F07 file with Consurf surface data (C function fopen)   */
/* (Program error SU2993 for open failure)                          */
/*                                                                 !*/
if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "(open)%%varkon_nmglftsurf (sur202");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Retrieve no of patches in V direction nv (from line 2 )          */
/*                                                                 !*/
fgets(lin_type, 5,f);                    /* Read the word LINE      */
fgets(cur_name,28,f);                    /* Read curve   name       */
fgets(rad,132,f);                        /* Read the rest of line 1 */
fgets(sec_type,11,f);                    /* Read the word SECTION   */
fgets(rad,132,f);                        /* Read the rest of line 2 */
sscanf(rad,"%d",&nv);                    /* The number of V lines   */
nv = nv - 1;                             /* The number of V patches */
fgets(nor_type,10,f);                    /* Read the word NORMAL    */
fgets(rad,132,f);                        /* Read rest of line NORMAL*/
sscanf(rad,"%lf%lf%lf",&k1,&k2,&k3);     /* The NORMAL point coord. */
spine[0][0]= k1;                         /* Spine start point x     */
spine[0][1]= k2;                         /* Spine start point y     */
spine[0][2]= k3;                         /* Spine start point z     */


for ( iv=0; iv<=nv; ++iv )               /* Start loop V line data  */
  {
  fgets(poi_type,9,f);                   /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  sscanf(rad,"%lf%lf%lf%lf%lf%lf",       /* The limit  point coord. */
            &k1,&k2,&k3,&k4,&k5,&k6);    /*                         */
  uplim[iv][0]=  k1;                     /* Previous U lim section  */
  uplim[iv][1]=  k2;                     /*                         */
  uplim[iv][2]=  k3;                     /*                         */
  uplimt[iv][0]= k4;                     /* Previous U tan section  */
  uplimt[iv][1]= k5;                     /*                         */
  uplimt[iv][2]= k6;                     /*                         */
  fgets(poi_type,9,f);                   /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  sscanf(rad,"%lf%lf%lf%lf%lf%lf",       /* The tangent point       */
            &k1,&k2,&k3,&k4,&k5,&k6);    /*                         */
  uptan[iv][0]=  k1;                     /* Previous U tan section  */
  uptan[iv][1]=  k2;                     /*                         */
  uptan[iv][2]=  k3;                     /*                         */
  uptant[iv][0]= k4;                     /* Previous U tan section  */
  uptant[iv][1]= k5;                     /*                         */
  uptant[iv][2]= k6;                     /*                         */
  fgets(pva_type,17,f);                  /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  if ( iv == nv )
    {
    sscanf(rad,"%lf%lf",                 /* The P-values            */
            &k1,&k2 );                   /*                         */
    uplimp[iv] =  k1;                    /*                         */
    uptanp[iv] =  k2;                    /*                         */
    upsecp[iv] =  1.2345;                /*                         */
    upsecdp[iv]=  1.2345;                /*                         */
    }
  else
    {
    sscanf(rad,"%lf%lf%lf%lf",           /* The P-values            */
            &k1,&k2,&k3,&k4);            /*                         */
    uplimp[iv] =  k1;                    /*                         */
    uptanp[iv] =  k2;                    /*                         */
    upsecp[iv] =  k3;                    /*                         */
    upsecdp[iv]=  k4;                    /*                         */
    }
  }

/*!                                                                 */
/* Determine the number of U sections by reading the whole file     */
/* Rewind by closing and opening of file                            */
/*                                                                 !*/
fclose(f);                               /* Close the input file    */
if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "(open2)%%varkon_nmglftsurf (sur202");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Start loop iu=1,2,3, .... , 200 (maximum number of sections)     */
/*   Read the first three lines. End of loop for End Of File        */
/*   Start loop iv=1,2,3, .., (nv+1)*3                              */
/*                                                                 !*/

nu = -123456789;
for ( iu=1; iu<200; ++iu )               /* Start loop U sections   */
  {
  if (fgets(lin_type, 5,f) == NULL )     /* Read the word LINE      */
    {                                    /* Failure <==> EOF        */
    nu = iu - 2;                         /* Number of U patches     */
    iu = 201;                            /* End of loop iu          */
    goto eloop;                          /* End of loop             */
    }
  fgets(cur_name,28,f);                  /* Read curve   name       */
  fgets(rad,132,f);                      /* Read the rest of line 1 */
  fgets(sec_type,11,f);                  /* Read the word SECTION   */
  fgets(rad,132,f);                      /* Read the rest of line 2 */
  sscanf(rad,"%d",&nv_check);            /* The number of V lines   */
  fgets(nor_type,10,f);                  /* Read the word NORMAL    */
  fgets(rad,132,f);                      /* Read rest of line NORMAL*/
  sscanf(rad,"%lf%lf%lf",&k1,&k2,&k3);   /* The NORMAL point coord. */
  for ( iv=1; iv<=3*(nv+1); ++iv )       /* Start loop V line data  */
    {
    fgets(rad,132,f);                    /* Read the whole line     */
    }                                    /* End   loop V line data  */
  }                                      /* End   loop U sections   */

eloop: ;                                 /* End of loop for EOF     */
fclose(f);                               /* Close the input file    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202 The number of V patches nv= %d\n",nv);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 The number of U patches nu= %d\n",nu);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Word LINE    for test lin_type= %s\n",lin_type);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Section curve name= %s\n",cur_name);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Word SECTION for test sec_type= %s\n",sec_type);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Word NORMAL  for test nor_type= %s\n",nor_type);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Spine start point %f %f %f\n",
   spine[0][0],spine[0][1],spine[0][2]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Word 1.1     for test poi_type= %s\n",poi_type);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Word 3.1 PVALUES for test pva_type= %s\n",pva_type);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Limit 1    %f %f %f %f %f %f\n",
    uplim[0][0], uplim[0][1], uplim[0][2],
   uplimt[0][0],uplimt[0][1],uplimt[0][2]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Tangent 1 %f %f %f %f %f %f\n",
    uptan[0][0], uptan[0][1], uptan[0][2],
   uptant[0][0],uptant[0][1],uptant[0][2]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 P-value 1 %f %f %f %f \n",
    uplimp[0], uptanp[0], upsecp[0],
   upsecdp[0]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Limit nv+1 %f %f %f %f %f %f\n",
    uplim[nv][0], uplim[nv][1], uplim[nv][2],
   uplimt[nv][0],uplimt[nv][1],uplimt[nv][2]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 Tang nv+1 %f %f %f %f %f %f\n",
    uptan[nv][0], uptan[nv][1], uptan[nv][2],
   uptant[nv][0],uptant[nv][1],uptant[nv][2]);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur202 P-value nv+1 %f %f %f %f \n",
    uplimp[nv], uptanp[nv], upsecp[nv],
   upsecdp[nv]);
  fflush(dbgfil(SURPAC));
  }
#endif

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
 sprintf(errbuf, "(alloc)%%varkon_nmglftsurf (sur202");
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

/*!                                                                 */
pat_no = 0;                              /* Patch number 1, 2, 3, ..*/
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202 Pointer to the first patch in the surface: p_frst= %d\n",
    (int)p_frst );
  fprintf(dbgfil(SURPAC),
  "sur202 sizeof(GMPATL)= %d (sizeof(GMPATC)= %d sizeof(EVALS)= %d)\n",
    (int)sizeof(GMPATL),(int)sizeof(GMPATC),(int)sizeof(EVALS));
  }
#endif

if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "(open3)%%varkon_nmglftsurf (sur202");
 return(varkon_erpush("SU2993",errbuf));
 }

for ( iu=1; iu<=nu+1;++iu )              /* Start loop U sections   */
 {
 fgets(lin_type, 5,f);                   /* Read the word LINE      */
 fgets(cur_name,28,f);                   /* Read curve   name       */
 fgets(rad,132,f);                       /* Read the rest of line 1 */
 fgets(sec_type,11,f);                   /* Read the word SECTION   */
 fgets(rad,132,f);                       /* Read the rest of line 2 */
 sscanf(rad,"%d",&nv);                   /* The number of V lines   */
 nv = nv - 1;                            /* The number of V patches */
 fgets(nor_type,10,f);                   /* Read the word NORMAL    */
 fgets(rad,132,f);                       /* Read rest of line NORMAL*/
 sscanf(rad,"%lf%lf%lf",&k1,&k2,&k3);    /* The NORMAL point coord. */


 for ( iv=0; iv<=nv; ++iv )              /* Start loop V line data  */
  {
  fgets(poi_type,9,f);                   /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  sscanf(rad,"%lf%lf%lf%lf%lf%lf",       /* The limit  point coord. */
            &k1,&k2,&k3,&k4,&k5,&k6);    /*                         */
  uclim[iv][0]=  k1;                     /* Current  U lim section  */
  uclim[iv][1]=  k2;                     /*                         */
  uclim[iv][2]=  k3;                     /*                         */
  uclimt[iv][0]= k4;                     /* Current  U tan section  */
  uclimt[iv][1]= k5;                     /*                         */
  uclimt[iv][2]= k6;                     /*                         */
  fgets(poi_type,9,f);                   /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  sscanf(rad,"%lf%lf%lf%lf%lf%lf",       /* The tangent point       */
            &k1,&k2,&k3,&k4,&k5,&k6);    /*                         */
  uctan[iv][0]=  k1;                     /* Previous U tan section  */
  uctan[iv][1]=  k2;                     /*                         */
  uctan[iv][2]=  k3;                     /*                         */
  uctant[iv][0]= k4;                     /* Previous U tan section  */
  uctant[iv][1]= k5;                     /*                         */
  uctant[iv][2]= k6;                     /*                         */
  fgets(pva_type,17,f);                  /* Read the word i.j       */
  fgets(rad,132,f);                      /* Read rest of line       */
  if ( iv == nv )
    {
    sscanf(rad,"%lf%lf",                 /* The P-values            */
            &k1,&k2 );                   /*                         */
    uclimp[iv] =  k1;                    /*                         */
    uctanp[iv] =  k2;                    /*                         */
    ucsecp[iv] =  1.2345;                /*                         */
    ucsecdp[iv]=  1.2345;                /*                         */
    if ( iu == nu+1 )
      {
      spine[1][0]= uclim[0][0];          /* Spine end   point x     */
      spine[1][1]= spine[0][1];          /* Spine end   point y     */
      spine[1][2]= spine[0][2];          /* Spine end   point z     */
      }
    }
  else
    {
    sscanf(rad,"%lf%lf%lf%lf",           /* The P-values            */
            &k1,&k2,&k3,&k4);            /*                         */
    uclimp[iv] =  k1;                    /*                         */
    uctanp[iv] =  k2;                    /*                         */
    ucsecp[iv] =  k3;                    /*                         */
    ucsecdp[iv]=  k4;                    /*                         */
    }
/*!                                                                 */
/*  Write patch data for iu > 1 and iv < nv                         */
/*        i. Let pat_no= pat_no+1 and p= p_frst+pat_no-1            */
/*       ii. Call of crelims.                                       */
/*      iii. Call of cretans.                                       */
/*       iv. Call of crespine.                                      */
/*        v. Call of crepval.                                       */
/*       vi. Call of crevlim.                                       */
/*      vii. Patch offset = 0.0                                     */
/*     viii. Call of crelime also for iv=nv (to previous patch)     */
/*       ix. Call of cretane also for iv=nv (to previous patch)     */
/*                                                                 !*/


    if ( iu > 1 && iv < nv )
      {
      pat_no = pat_no + 1;
      p_t = p_frst + (iu-2)*nv + iv;     /* Ptr to  current patch   */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202 iu-2 %d iv %d pat_no %d p_t %d (=alternativ %d)\n",
   (int)iu-2 , (int)iv , (int)pat_no , (int)p_t, (int)(p_frst+pat_no-1));
  }
#endif


/*!                                                                 */
/* Dynamic allocation of area for one geometrical patch.            */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(LFT_PAT,1)) == NULL )
 {
 sprintf(errbuf, "(allocg)%%varkon_nmglftsurf (sur202");
 return(varkon_erpush("SU2993",errbuf));
 }

      p= (GMPATL *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/* Initiate patch data in p   for Debug On                          */
/* Call of varkon_ini_gmpatl (sur765).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatl ( p );
#endif

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + (iu-2)*nv + iv                         */
/*                                                                 !*/
/*  p_t= (PATMAT  *)*pp_at + pat_no - 1  (alternative)              */

  p_t->styp_pat = LFT_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)(iu - 1);       /* Topological adress for  */
  p_t->iv_pat   = (short)(iv + 1);       /* current (this) patch    */
  p_t->us_pat = (DBfloat)(iu)-1.0;       /* Start U on geom. patch  */
  p_t->ue_pat = (DBfloat)(iu)-comptol;   /* End   U on geom. patch  */
  p_t->vs_pat = (DBfloat)iv+1.0;         /* Start V on geom. patch  */
  p_t->ve_pat = (DBfloat)iv+2.0-comptol; /* End   V on geom. patch  */
    
      status= crelims();                 /* Call of crelims         */
      if (status<0) 
      {
      sprintf(errbuf,"crelims%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      status= cretans();                 /* Call of cretans         */
      if (status<0) 
      {
      sprintf(errbuf,"cretans%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      status= crespine();                /* Call of crespine        */
      if (status<0) 
      {
      sprintf(errbuf,"crespine%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      status= crepval();                 /* Call of crepval         */
      if (status<0) 
      {
      sprintf(errbuf,"crepval%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      status= crevlim();                 /* Call of crevlim         */
      if (status<0) 
      {
      sprintf(errbuf,"crevlim%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      p->ofs_pat = 0.0;                  /* Offset for the patch    */

      }                                  /* End iu > 1 and iv < nv  */
    if ( iu > 1 )
      {
      status= crelime();                 /* Call of crelime         */
      if (status<0) 
      {
      sprintf(errbuf,"crelime%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      status= cretane();                 /* Call of cretane         */
      if (status<0) 
      {
      sprintf(errbuf,"cretane%%varkon_nmglftsurf (sur202)");
      return(varkon_erpush("SU2973",errbuf));
      }
      }                                  /* End iu > 1              */


   }                                     /* End loop iv             */

  if ( iu > 1 )
    {
    for ( ix1=0; ix1<=nv; ++ix1 )        /* Start loop ix1(V lines) */
      {
      for ( ix2=0; ix2<=2 ; ++ix2 )      /* Start loop ix2(V lines) */
        {
        uplim[ix1][ix2]=  uclim[ix1][ix2];
        uplimt[ix1][ix2]= uclimt[ix1][ix2];
        uptan[ix1][ix2]=  uctan[ix1][ix2];
        uptant[ix1][ix2]= uctant[ix1][ix2];
        }
      uplimp[ix1]= uclimp[ix1];  
      uptanp[ix1]= uctanp[ix1]; 
      upsecp[ix1]= ucsecp[ix1];
      upsecdp[ix1]= ucsecdp[ix1]; 
      }
    }

  }                                      /* End loop iu             */

/*!                                                                 */
/* Close the NMG F07 file (C function fclose)                       */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202 Spine end   point %f %f %f\n",
   spine[1][0],spine[1][1],spine[1][2]);
  fprintf(dbgfil(SURPAC),
  "sur202 Limit nv+1 %f %f %f %f %f %f\n",
    uclim[nv][0], uclim[nv][1], uclim[nv][2],
   uclimt[nv][0],uclimt[nv][1],uclimt[nv][2]);
  fprintf(dbgfil(SURPAC),
  "sur202 Tang nv+1 %f %f %f %f %f %f\n",
    uctan[nv][0], uctan[nv][1], uctan[nv][2],
   uctant[nv][0],uctant[nv][1],uctant[nv][2]);
  fprintf(dbgfil(SURPAC),
  "sur202 P-value nv+1 %f %f %f %f \n",
    uclimp[nv], uctanp[nv], ucsecp[nv],
   ucsecdp[nv]);
  fprintf(dbgfil(SURPAC),
  "sur202 IU= %d IV= %d p= %d p_frst= %d\n",
    iu, iv+1, (int)p , (int)p_frst );
  fprintf(dbgfil(SURPAC),
  "sur202 No patches in direction U nu= %d and V nv= %d \n"
                 ,(int)nu,(int)nv);
  }
  fflush(dbgfil(SURPAC));
#endif


/*!                                                                 */
/* 3. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 1;         /* Calculation of BBOX only and not BCONE       */

   status=varkon_sur_bound (p_sur,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_nmgbicubic(sur201)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202 Exit *** varkon_nmglftsurf***No of patches nu= %d nv= %d\n"
              , (int)nu , (int)nv);
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /*  End of function */

/*********************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** crelims ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the start limit curve segment               */

   static short crelims()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       uplim, uplimt, uplimp, uclim, uclimt, iu, iv and p         */
/* Out:                                                             */
/*       Curve data to p->lims                                      */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

  DBfloat us_in;         /* Parameter value for input  start point  */
  DBfloat ue_in;         /* Parameter value for input  end   point  */
  DBfloat us_out;        /* Parameter value for output start point  */
  DBfloat ue_out;        /* Parameter value for output end   point  */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crelims* Enter ** IU= %d IV= %d  Patch p= %d pat_no= %d\n",
    (int)iu-1,(int)iv+1,(int)p,(int)pat_no);
  }
#endif

/*!                                                                 */
/* 1. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/*                                                                 !*/

    points[0].x_gm=uplim[iv][0];                  /* Start point    */
    points[0].y_gm=uplim[iv][1];                  /*                */
    points[0].z_gm=uplim[iv][2];                  /*                */
    points[1].x_gm=uplim[iv][0]+100*uplimt[iv][0];/* Start tangent  */
    points[1].y_gm=uplim[iv][1]+100*uplimt[iv][1];/*                */
    points[1].z_gm=uplim[iv][2]+100*uplimt[iv][2];/*                */
    points[3].x_gm=uclim[iv][0];                  /* End   point    */
    points[3].y_gm=uclim[iv][1];                  /*                */
    points[3].z_gm=uclim[iv][2];                  /*                */
    points[2].x_gm=uclim[iv][0]+100*uclimt[iv][0];/* End   tangent  */
    points[2].y_gm=uclim[iv][1]+100*uclimt[iv][1];/*                */
    points[2].z_gm=uclim[iv][2]+100*uclimt[iv][2];/*                */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/*                                                                 !*/

    status=GE133 (points,uplimp[iv],&p->lims); 

    if (status<0) 
       {
       sprintf(errbuf,"GE133(lims)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* Extrapolate segment. Call of varkon_cur_reparam (sur790).        */
/*                                                                 !*/

us_in  = -0.01;
ue_in  =  1.01;
us_out =  0.0;
ue_out =  1.0;

status=varkon_cur_reparam
    (&p->lims,us_in,ue_in,us_out,ue_out,&p->lims);
    if (status<0) 
       {
       sprintf(errbuf,"sur790(lims)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** cretans ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the start tangent curve segment             */

   static short cretans()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       uptan, uptant, uptanp, uctan, uctant, iu, iv and p         */

/* Out:                                                             */
/*       Curve data to p->tans                                      */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  DBfloat us_in;         /* Parameter value for input  start point  */
  DBfloat ue_in;         /* Parameter value for input  end   point  */
  DBfloat us_out;        /* Parameter value for output start point  */
  DBfloat ue_out;        /* Parameter value for output end   point  */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*cretans* Enter ** IU= %d IV= %d  Patch p= %d \n",
                   (int)iu-1,(int)iv+1,(int)p);
  }
#endif

/*!                                                                 */
/* 1. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/*                                                                 !*/

    points[0].x_gm=uptan[iv][0];                  /* Start point    */
    points[0].y_gm=uptan[iv][1];                  /*                */
    points[0].z_gm=uptan[iv][2];                  /*                */
    points[1].x_gm=uptan[iv][0]+100*uptant[iv][0];/* Start tangent  */
    points[1].y_gm=uptan[iv][1]+100*uptant[iv][1];/*                */
    points[1].z_gm=uptan[iv][2]+100*uptant[iv][2];/*                */
    points[3].x_gm=uctan[iv][0];                  /* End   point    */
    points[3].y_gm=uctan[iv][1];                  /*                */
    points[3].z_gm=uctan[iv][2];                  /*                */
    points[2].x_gm=uctan[iv][0]+100*uctant[iv][0];/* End   tangent  */
    points[2].y_gm=uctan[iv][1]+100*uctant[iv][1];/*                */
    points[2].z_gm=uctan[iv][2]+100*uctant[iv][2];/*                */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/*                                                                 !*/

    status=GE133 (points,uptanp[iv],&p->tans); 
    if (status<0) 
       {
       sprintf(errbuf,"GE133(tans)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* Extrapolate segment. Call of varkon_cur_reparam (sur790).        */
/*                                                                 !*/

us_in  = -0.01;
ue_in  =  1.01;
us_out =  0.0;
ue_out =  1.0;

status=varkon_cur_reparam
    (&p->tans,us_in,ue_in,us_out,ue_out,&p->tans);
    if (status<0) 
       {
       sprintf(errbuf,"sur790(tans)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** crelime ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the end   limit curve segment               */

   static short crelime()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       uplim, uplimt, uplimp, uclim, uclimt, iu, iv and p         */

/* Out:                                                             */
/*       Curve data to pp->lime , where pp= p-1 (previous patch)    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   int    ia;            /* Array index corresp. to v line ia=iv-1  */
   GMPATL *pp;           /* Previous geometric   patch        (ptr) */
   DBPatch *ppt;         /* Previous topological patch        (ptr) */
/*-----------------------------------------------------------------!*/
  DBfloat us_in;         /* Parameter value for input  start point  */
  DBfloat ue_in;         /* Parameter value for input  end   point  */
  DBfloat us_out;        /* Parameter value for output start point  */
  DBfloat ue_out;        /* Parameter value for output end   point  */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crelime* Enter ** IU= %d IV= %d  Patch p= %d \n",
                   (int)iu-1,(int)iv+1,(int)p);
  }
#endif

/*!                                                                 */
/* 1. Initiations                                                   */
/* _______________________                                          */
/*                                                                  */
/* If not the first V line, e.g iv > 0:                             */
/*     i. Let patch  ptr point at previous patch  pp= "p_t-1"       */
/*        except for iv= nv. In this case let     pp=  p            */
/*    ii. Let V line ptr point at previous V line ia= iv-1          */
/* If it is the first V line, e.g iv = 0:                           */
/*     i. No curve segment is created (return only)                 */
/*                                                                 !*/

   if ( iv > 0 )
     {
     if( iv < nv )
       {
       ppt = p_t-1;
       pp  = (GMPATL *)ppt->spek_c;    
       }
     else
       {
       pp = p;
       }
     ia = iv;
     }
   else
     {
     return(SUCCED);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crelime  IU= %d IV= %d &pp->lime= %d Patch p= %d \n"
   ,(int)iu-1,(int)ia,(int)&pp->lime , (int)pp );
  }
#endif

/*!                                                                 */

/* 2. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/*                                                                 !*/

    points[0].x_gm=uplim[ia][0];                  /* Start point    */
    points[0].y_gm=uplim[ia][1];                  /*                */
    points[0].z_gm=uplim[ia][2];                  /*                */
    points[1].x_gm=uplim[ia][0]+100*uplimt[ia][0];/* Start tangent  */
    points[1].y_gm=uplim[ia][1]+100*uplimt[ia][1];/*                */
    points[1].z_gm=uplim[ia][2]+100*uplimt[ia][2];/*                */
    points[3].x_gm=uclim[ia][0];                  /* End   point    */
    points[3].y_gm=uclim[ia][1];                  /*                */
    points[3].z_gm=uclim[ia][2];                  /*                */
    points[2].x_gm=uclim[ia][0]+100*uclimt[ia][0];/* End   tangent  */
    points[2].y_gm=uclim[ia][1]+100*uclimt[ia][1];/*                */
    points[2].z_gm=uclim[ia][2]+100*uclimt[ia][2];/*                */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/*                                                                 !*/

    status=GE133 (points,uplimp[ia],&pp->lime); 
    if (status<0) 
       {
       sprintf(errbuf,"GE133(lims)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* Extrapolate segment. Call of varkon_cur_reparam (sur790).        */
/*                                                                 !*/

us_in  = -0.01;
ue_in  =  1.01;
us_out =  0.0;
ue_out =  1.0;

status=varkon_cur_reparam
    (&pp->lime,us_in,ue_in,us_out,ue_out,&pp->lime);
    if (status<0) 
       {
       sprintf(errbuf,"sur790(lime)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }



    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!New-Page--------------------------------------------------------!*/
/*!********* Internal ** function ** cretane ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the end   tangent curve segment             */

   static short cretane()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       uptan, uptant, uptanp, uctan, uctant, iu, iv and p         */

/* Out:                                                             */
/*       Curve data to pp->tane , where pp= p-1 (previous patch)    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   int    ia;            /* Array index corresp. to v line ia=iv-1  */
   GMPATL *pp;           /* Pointer to the previous patch           */
   DBPatch *ppt;         /* Previous topological patch        (ptr) */
/*-----------------------------------------------------------------!*/

  DBfloat us_in;         /* Parameter value for input  start point  */
  DBfloat ue_in;         /* Parameter value for input  end   point  */
  DBfloat us_out;        /* Parameter value for output start point  */
  DBfloat ue_out;        /* Parameter value for output end   point  */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*cretane* Enter ** IU= %d IV= %d  Patch p= %d \n",
                    (int)iu-1,(int)iv+1,(int)p);
  }
#endif

/*!                                                                 */
/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                  */
/* If not the first V line, e.g iv > 0:                             */
/*      i. Let patch  ptr point at previous patch pp= "p_t-1"       */
/*        except for iv= nv. In this case let     pp=  p            */
/*     ii. Let V line ptr point at previous V line ia= iv-1         */
/* If it is the first V line, e.g iv = 0:                           */
/*      i. No curve segment is created (return only)                */
/*                                                                 !*/

   if ( iv > 0 )
     {
     if( iv < nv )
       {
       ppt = p_t-1;
       pp  = (GMPATL *)ppt->spek_c;    
       }
     else
       {
       pp = p;
       }
     ia = iv;
     }
   else
     {
     return(SUCCED);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*cretane  IU= %d IV= %d &pp->tane= %d Patch p= %d \n"
   ,(int)iu-1,(int)ia,(int)&pp->tane , (int)pp );
  }
#endif

/*!                                                                 */
/*!                                                                 */
/* 2. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/*                                                                 !*/

    points[0].x_gm=uptan[ia][0];                  /* Start point    */
    points[0].y_gm=uptan[ia][1];                  /*                */
    points[0].z_gm=uptan[ia][2];                  /*                */
    points[1].x_gm=uptan[ia][0]+100*uptant[ia][0];/* Start tangent  */
    points[1].y_gm=uptan[ia][1]+100*uptant[ia][1];/*                */
    points[1].z_gm=uptan[ia][2]+100*uptant[ia][2];/*                */
    points[3].x_gm=uctan[ia][0];                  /* End   point    */
    points[3].y_gm=uctan[ia][1];                  /*                */
    points[3].z_gm=uctan[ia][2];                  /*                */
    points[2].x_gm=uctan[ia][0]+100*uctant[ia][0];/* End   tangent  */
    points[2].y_gm=uctan[ia][1]+100*uctant[ia][1];/*                */
    points[2].z_gm=uctan[ia][2]+100*uctant[ia][2];/*                */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/*                                                                 !*/

    status=GE133 (points,uptanp[ia],&pp->tane); 
    if (status<0) 
       {
       sprintf(errbuf,"GE133(tane)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* Extrapolate segment. Call of varkon_cur_reparam (sur790).        */
/*                                                                 !*/

us_in  = -0.01;
ue_in  =  1.01;
us_out =  0.0;
ue_out =  1.0;

status=varkon_cur_reparam
    (&pp->tane,us_in,ue_in,us_out,ue_out,&pp->tane);
    if (status<0) 
       {
       sprintf(errbuf,"sur790(tane)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }



    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!New-Page--------------------------------------------------------!*/
/*!********* Internal ** function ** crepval ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the start limit curve segment               */

   static short crepval()
/*                                                                  */
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       upsecp, upsecpt, upsecdp, ucsecp, ucsecpt, iu, iv and p    */

/* Out:                                                             */
/*       Curve data to p->pval and p->p_flag= 1 (P value function)  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  DBfloat us_in;         /* Parameter value for input  start point  */
  DBfloat ue_in;         /* Parameter value for input  end   point  */
  DBfloat us_out;        /* Parameter value for output start point  */
  DBfloat ue_out;        /* Parameter value for output end   point  */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crepval* Enter ** IU= %d IV= %d  Patch p= %d \n",
             (int)iu-1,(int)iv+1,(int)p);
  }
#endif

/*!                                                                 */
/* 1. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/* Start x=0 and end x= spine segment length                        */
/*                                                                 !*/

    points[0].x_gm=0.0;                           /* Start point X  */
    points[0].y_gm=upsecp[iv];                    /* P value        */
    points[0].z_gm= 0.0;                          /* 0.0            */
    points[1].x_gm=100.0;                         /* Start tangent X*/
    points[1].y_gm=upsecp[iv]+100*upsecdp[iv];    /* dPds point     */
    points[1].z_gm= 0.0;                          /* 0.0            */
    points[3].x_gm=p->spine.sl;                   /* End   point X  */
    points[3].y_gm=ucsecp[iv];                    /* P value        */
    points[3].z_gm= 0.0;                          /* 0.0            */
    points[2].x_gm=p->spine.sl+100.0;             /* End   tangent X*/
    points[2].y_gm=ucsecp[iv]+100*ucsecdp[iv];    /* dPds point     */
    points[2].z_gm= 0.0;                          /* 0.0            */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/*                                                                 !*/

    status=GE133 (points,  0.5     ,&p->pval); 
    if (status<0) 
       {
       sprintf(errbuf,"GE133(pval)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* Extrapolate segment. Call of varkon_cur_reparam (sur790).        */
/*                                                                 !*/

us_in  = -0.01;
ue_in  =  1.01;
us_out =  0.0;
ue_out =  1.0;

status=varkon_cur_reparam
    (&p->pval,us_in,ue_in,us_out,ue_out,&p->pval);
    if (status<0) 
       {
       sprintf(errbuf,"sur790(pval)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* P value flag for P value function (p->p_flag= 1)                 */
/*                                                                 !*/
   p->p_flag  = 1;


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** crevlim ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the V start and V end line                  */
/*  !!! Os{ker p} vad och hur jag ska spara data !!!!!!!            */

   static short crevlim()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       iu, iv and p                                               */
/* Out:                                                             */
/*       Line data to p->vs and p->ve                               */
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
  "sur202*crevlim IU= %d IV= %d  &p->vs %d &p->ve %d p %d\n"
   ,(int)iu,(int)iv+1,(int)&p->vs, (int)&p->ve , (int)p);
  }
#endif

/*!                                                                 */
/* 1. Create line  segment vs                                       */
/* __________________________                                       */
/*                                                                  */
/* Segment data to local variable p1 and p2                         */
/*                                                                 !*/

    p1.x_gm  =(DBfloat)iu  - 1.0;                   /* Start point    */
    p1.y_gm  =(DBfloat)iu  + 1.0;                   /*                */
    p1.z_gm  =   0.0;                             /*                */
    p2.x_gm  =(DBfloat)iu ;                         /* End   point    */
    p2.y_gm  =(DBfloat)iv  + 1.0;                   /*                */
    p2.z_gm  =   0.0;                             /*                */

/*!                                                                 */
/* Create a straight line segment. Call of GE200.                  */
/*                                                                 !*/

    status=GE200 (&p1 , &p2 , &p->vs ); 
    if (status<0) 
       {
       sprintf(errbuf,"GE200(vs)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

/*!                                                                 */
/* 2. Create line  segment ve                                       */
/* __________________________                                       */
/*                                                                  */
/* Segment data to local variable p1 and p2                         */
/*                                                                 !*/

    p1.x_gm  =(DBfloat)iu  - 1.0;                 /* Start point    */
    p1.y_gm  =(DBfloat)iv  + 2.0;                 /*                */
    p1.z_gm  =   0.0;                             /*                */
    p2.x_gm  =(DBfloat)iu ;                       /* End   point    */
    p2.y_gm  =(DBfloat)iv  + 2.0;                 /*                */
    p2.z_gm  =   0.0;                             /*                */

/*!                                                                 */
/* Create a straight line segment. Call of GE200.                  */
/*                                                                 !*/

    status=GE200 (&p1 , &p2 , &p->ve ); 
    if (status<0) 
       {
       sprintf(errbuf,"EG200(ve)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!********* Internal ** function ** crespine ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the spine curve segment.                    */
/* Note that it is assumed that the spine is a                      */
/* straight line equal to the X axis !!!                            */
/* ....  mycket mer att programmera om fler Consurf ytor ska klaras */
/*                                                                  */

   static short crespine()
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       spine, uplim, uclim, iu, iv and p                          */
/* Out:                                                             */
/*       Curve data to p->spine                                     */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   short  status;        /* Error code from a called fctn           */
   char   errbuf[80];    /* String for error message fctn erpush    */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crespine IU= %d IV= %d &p->spine= %d Patch p= %d pat_no= %d\n"
   ,(int)iu-1,(int)iv+1,(int)&p->spine, (int)p, (int)pat_no);
  }
#endif

/*!                                                                 */
/* 1. Create curve segment                                          */
/* _______________________                                          */
/*                                                                  */
/* Segment data to array points()                                   */
/*                                                                 !*/

    points[0].x_gm=uplim[iv][0];                  /* Start point    */
    points[0].y_gm=spine[0][1];                   /*                */
    points[0].z_gm=spine[0][2];                   /*                */
    points[1].x_gm=uplim[iv][0]+100;              /* Start tangent  */
    points[1].y_gm=spine[0][1];                   /*                */
    points[1].z_gm=spine[0][2];                   /*                */
    points[3].x_gm=uclim[iv][0];                  /* End   point    */
    points[3].y_gm=spine[0][1];                   /*                */
    points[3].z_gm=spine[0][2];                   /*                */
    points[2].x_gm=uclim[iv][0]+100;              /* End   tangent  */
    points[2].y_gm=spine[0][1];                   /*                */
    points[2].z_gm=spine[0][2];                   /*                */

/*!                                                                 */
/* Create conic segment with P-value. Call of GE133.               */
/* Let also p->spine.sl= length of the line.                        */
/*                                                                 !*/

    status=GE133 (points,    0.5   ,&p->spine); 
    if (status<0) 
       {
       sprintf(errbuf,"GE133(spine)%%varkon_nmglftsurf");
       return(varkon_erpush("SU2943",errbuf));
       }

    p->spine.sl = fabs(points[0].x_gm-points[3].x_gm);

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur202*crespine Pointer to rational segm. data &p->spine= %d\n"
       ,(int)&p->spine );
  fprintf(dbgfil(SURPAC),
  "sur202*crespine p->spine.c0x= %f   &p->spine.c0x= %d\n"
       ,p->spine.c0x, (int)&p->spine.c0x );
  fprintf(dbgfil(SURPAC),
  "sur202*crespine p->spine.c2x= %f   &p->spine.c2x= %d\n"
       ,p->spine.c2x, (int)&p->spine.c2x );
  fprintf(dbgfil(SURPAC),
  "sur202*crespine p->spine.c3x= %f   &p->spine.c3x= %d\n"
       ,p->spine.c3x, (int)&p->spine.c3x );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


