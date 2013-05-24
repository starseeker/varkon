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
/*  Function: varkon_sur_rferguson                 File: sur291.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads a Ferguson surface from an input file.       */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  TODO Check and think if function shall be kept                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_rferguson  Patch corner points to CUB_SUR   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_evals        * Initiate EVALS                         */
/* varkon_ini_gmsur        * Initiate DBSurf                        */
/* varkon_ini_gmpat        * Initiate DBPatch                       */
/* varkon_ini_gmpatc       * Initiate GMPATC                        */
/* varkon_sur_biccre1      * Patch corner pts to CUB_PAT            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_pat_pribic       * Print bicubic patch data               */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_rferguson       */
/* SU2993 = Severe program error (  ) in varkon_sur_rferguson       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

      DBstatus        varkon_sur_rferguson (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *infile,       /* Surface file with Ferguson bicubic data */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  int     nu;            /* No patches in U direction input surface */
  int     nv;            /* No patches in V direction input surface */
  DBint   maxnum;        /* Maximum number of patches (alloc. area) */
  DBPatch *p_frst;       /* Pointer to the first patch              */
  DBPatch *p_t;          /* Current topological patch         (ptr) */
  GMPATC *p_patcc;       /* Current geometric   patch         (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   acc_box;       /* For sur911                              */
  DBint   iu,iv;         /* Patch address                           */
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBint   i_s;           /* Loop index surface patch record         */
  char   *p_gpat;        /* Allocated area geom.  patch data  (ptr) */
  char   *p_tpat;        /* Allocated area topol. patch data  (ptr) */
  EVALS  r00;            /* Corner point U=0 V=0                    */
  EVALS  r01;            /* Corner point U=0 V=1                    */
  EVALS  r10;            /* Corner point U=1 V=0                    */
  EVALS  r11;            /* Corner point U=1 V=1                    */
  DBint  pat_no;         /* Patch number                            */
  DBint  status;         /* Error code from called function         */
  FILE  *f;              /* Pointer to the input ....F07 file       */

  char   row_code[9];    /* Code for row in file                    */
  char   rad[133];       /* One line in the input file              */
  int    iu_f,iv_f;      /* Patch address from file                 */
  double k1,k2,k3;       /* Dummy reals                             */

  char   errbuf[80];     /* String for error message fctn erpush    */

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
"sur291 Enter***varkon_sur_rferguson infile: %s\n" , infile );
}
#endif
 
/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initiate surface data in p_surout.                               */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);

/*!                                                                 */
/* Initialize corner points r00, r01, r10, r11.                     */
/* Calls of varkon_ini_evals (sur770).                              */
/*                                                                 !*/
    varkon_ini_evals (&r00);
    varkon_ini_evals (&r01);
    varkon_ini_evals (&r10);
    varkon_ini_evals (&r11);

/*!                                                                 */
/* Open the output file (C function fopen)                          */
/*                                                                 !*/
/* (Program error SU2993 for open failure)                          */

if ( (f= fopen( infile,"r")) == NULL )
 { 
 sprintf(errbuf, "(open)%%varkon_sur_rferguson");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* 3. Allocate memory area for output surface                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

fgets(row_code,6,f);                     /* Read code for row       */
fgets(rad,132,f);                        /* Read the rest of line 1 */
sscanf(rad,"%d%d",&nu,&nv);              /* Total number of patches */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur291 row_code %s nu %d nv %d\n",
     row_code , nu, nv ); 
  }
fflush(dbgfil(SURPAC));
#endif


/*!                                                                 */
/* Dynamic allocation of area (pp_patout) for the surface data.     */
/* Call of function DBcreate_patches. Error SU2993 for failure.     */
/*                                                                 !*/

   maxnum = nu*nv;

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur291 Allocation failed  nu %d nv %d maxnum %d\n",
    nu, nv, (int)maxnum );
  }
#endif
 sprintf(errbuf,                         /* Error SU2993 for alloc  */
 "(alloc)%%varkon_sur_rferguson");       /* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }                                       /*                         */

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
/* Initiate patch data in pp_patout.                                */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= maxnum ;i_s= i_s+1)
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
  "sur291 Ptr to first patch p_frst  = %d No patches %d\n",
    (int)p_frst  ,(int)maxnum);
  }
#endif

/*!                                                                 */
/* Surface type and number of patches:                              */
          p_surout->typ_su= CUB_SUR;
          p_surout->nu_su = (short)nu;  
          p_surout->nv_su = (short)nv;
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Create the bicubic surface                                    */
/* ______________________________                                   */
/*                                                                  */
/*                                                                 !*/

/* Loop all U patches  iu=0,1,....,nu-1                             */
/*  Loop all V patches  iv=0,1,....,nv-1                            */
/*                                                                 !*/

for ( iu=0; iu<nu; ++iu )                /* Start loop U patches    */
  {
  for ( iv=0; iv<nv; ++iv )              /* Start loop V patches    */
    {

/*!                                                                 */

/*!                                                                 */
/*   Pointer to current patch ( !!  bara p_patcc f|r n{rvarande !!! */
/*                                                                 !*/

     pat_no = pat_no + 1;                /* Patch number 1, 2, 3, ..*/
     p_t = p_frst + pat_no - 1;          /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (bicubic) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
 {
 sprintf(errbuf, "(allocg)%%varkon_sur_rferguson");
 return(varkon_erpush("SU2993",errbuf));
 }

p_patcc= (GMPATC *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/* Initiate patch data in p_patcc for Debug On                      */
/* Call of varkon_ini_gmpatc (sur767).                              */
/*                                                                 !*/
#ifdef DEBUG
    varkon_ini_gmpatc (p_patcc);
#endif

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + pat_no + 1                             */
/*                                                                 !*/

  p_t->styp_pat = CUB_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)(iu + 1);       /* Topological adress for  */
  p_t->iv_pat   = (short)(iv + 1);       /* current (this) patch    */
  p_t->us_pat= (DBfloat)iu+1.0+comptol;  /* Start U on geom. patch  */
  p_t->ue_pat= (DBfloat)iu+2.0-comptol;  /* End   U on geom. patch  */
  p_t->vs_pat= (DBfloat)iv+1.0+comptol;  /* Start V on geom. patch  */
  p_t->ve_pat= (DBfloat)iv+2.0-comptol;  /* End   V on geom. patch  */

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

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur291 pat_no %d p_t %d p_patcc %d\n", 
         (int)pat_no, (int)p_t, (int)p_patcc ); 
  }
fflush(dbgfil(SURPAC));
#endif

/*!                                                                 */
/*    Retrieve corner points for output patch from input file       */
/*                                                                 !*/

fgets(row_code,6,f);                     /* Read code for row       */
fgets(rad,132,f);                        /* Read the rest of line   */
sscanf(rad,"%d%d",&iu_f,&iv_f);          /* Patch adress            */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur291 row_code %s iu_f %d iv_f %d p_t->iu_pat %d p_t->iv_pat %d\n",
     row_code , iu_f,iv_f, (int)p_t->iu_pat, (int)p_t->iv_pat  ); 
  }
fflush(dbgfil(SURPAC));
#endif

/*!                                                                 */
/*    Read  row 1: r00   r01   rv00  rv01                           */
/*                                                                 !*/

    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r00.r_x   = (DBfloat)k1;             /* r00                     */
    r00.r_y   = (DBfloat)k2;             /*                         */
    r00.r_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 r00   %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r01.r_x   = (DBfloat)k1;             /* r01                     */
    r01.r_y   = (DBfloat)k2;             /*                         */
    r01.r_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 r01   %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r00.v_x   = (DBfloat)k1;             /* rv00                    */
    r00.v_y   = (DBfloat)k2;             /*                         */
    r00.v_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 rv00  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r01.v_x   = (DBfloat)k1;             /* rv01                    */
    r01.v_y   = (DBfloat)k2;             /*                         */
    r01.v_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 rv01  %f %f %f \n",k1,k2,k3); 
  }
#endif

/*!                                                                 */
/*    Read  row 2: r10   r11   rv10  rv11                           */
/*                                                                 !*/

    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r10.r_x   = (DBfloat)k1;             /* r10                     */
    r10.r_y   = (DBfloat)k2;             /*                         */
    r10.r_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 r10   %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r11.r_x   = (DBfloat)k1;             /* r11                     */
    r11.r_y   = (DBfloat)k2;             /*                         */
    r11.r_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 r11   %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r10.v_x   = (DBfloat)k1;             /* rv10                    */
    r10.v_y   = (DBfloat)k2;             /*                         */
    r10.v_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 rv10  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r11.v_x   = (DBfloat)k1;             /* rv11                    */
    r11.v_y   = (DBfloat)k2;             /*                         */
    r11.v_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 rv11  %f %f %f \n",k1,k2,k3); 
  }
#endif

/*!                                                                 */
/*    Read  row 3: ru00  ru01  ruv00 ruv01                          */
/*                                                                 !*/

    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r00.u_x   = (DBfloat)k1;             /* ru00                    */
    r00.u_y   = (DBfloat)k2;             /*                         */
    r00.u_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ru00  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r01.u_x   = (DBfloat)k1;             /* ru01                    */
    r01.u_y   = (DBfloat)k2;             /*                         */
    r01.u_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ru01  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r00.uv_x  = (DBfloat)k1;             /* ruv00                   */
    r00.uv_y  = (DBfloat)k2;             /*                         */
    r00.uv_z  = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ruv00 %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r01.uv_x  = (DBfloat)k1;             /* ruv01                   */
    r01.uv_y  = (DBfloat)k2;             /*                         */
    r01.uv_z  = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ruv01 %f %f %f \n",k1,k2,k3); 
  }
#endif

/*!                                                                 */
/*    Read  row 4: ru10  ru11  ruv10 ruv11                          */
/*                                                                 !*/

    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r10.u_x   = (DBfloat)k1;             /* ru10                    */
    r10.u_y   = (DBfloat)k2;             /*                         */
    r10.u_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ru10  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r11.u_x   = (DBfloat)k1;             /* ru11                    */
    r11.u_y   = (DBfloat)k2;             /*                         */
    r11.u_z   = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ru11  %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r10.uv_x  = (DBfloat)k1;             /* ruv10                   */
    r10.uv_y  = (DBfloat)k2;             /*                         */
    r10.uv_z  = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ruv10 %f %f %f \n",k1,k2,k3); 
  }
#endif
    fgets(row_code,6,f);                 /* Read code for row       */
    fgets(rad,132,f);                    /* Read rest of line       */
    sscanf(rad,"%lf%lf%lf",              /* Coordinates/derivatives:*/
             &k1,&k2,&k3);               /*                         */
    r11.uv_x  = (DBfloat)k1;             /* ruv11                   */
    r11.uv_y  = (DBfloat)k2;             /*                         */
    r11.uv_z  = (DBfloat)k3;             /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur291 ruv11 %f %f %f \n",k1,k2,k3); 
  }
#endif

/*!                                                                 */
/*   Convert Ferguson format to CUB_PAT format.                     */
/*   Call of varkon_sur_biccre1 (sur254).                           */
/*                                                                 !*/

   status= varkon_pat_biccre1(&r00, &r10, &r01, &r11, p_patcc);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur291 varkon_pat_biccre1 failed \n"); 
#endif
    sprintf(errbuf,"varkon_pat_biccre1%%varkon_sur_rferguson");
    return(varkon_erpush("SU2943",errbuf));
    }

    p_patcc->ofs_pat = 0.0;              /* Offset for the patch    */

/*!                                                                 */
/*   For Debug On: Printout of second output bicubic patch          */
/*                                                                 !*/

#ifdef DEBUG
if ( pat_no == 2            )
  {
    status = varkon_pat_pribic  (p_patcc); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pribic%%varkon_sur_rferguson");
    return(varkon_erpush("SU2943",errbuf));
    }
  }
#endif

    }                                    /* End   loop V patches    */
  }                                      /* End   loop U patches    */
/*!                                                                 */
/*  End  all V patches  iv=0,1,....,nv-1                            */
/* End  all U patches  iu=0,1,....,nu-1                             */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur291 Close input file\n");
  }
fflush(dbgfil(SURPAC));
#endif

/*!                                                                 */
/* Close the input file (C function fclose)                         */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */


/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur291 Calculate bounding boxes and cones p_surout %d p_frst %d\n"
         , (int)p_surout, (int)p_frst );
  }
fflush(dbgfil(SURPAC)); 
#endif

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout ,p_frst ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_rferguson");
        return(varkon_erpush("SU2943",errbuf));
        }


/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur291 Total number of patches %d \n", (int)pat_no );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur291 Exit***varkon_sur_rferguson ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
  }
fflush(dbgfil(SURPAC)); 
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
