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
/*  Function: varkon_sur_wferguson                   File: sur290.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function writes a CUB_SUR surface to an output file.        */
/*                                                                  */
/*  The function writes surface data in Ferguson format, i.e        */
/*  surface corner coordinates and derivatives.                     */
/*                                                                  */
/*  TODO Check and think if function will be used in the future     */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-03   Originally written                                 */
/*  1996-05-28   Declaration of sur210, erased comptol and rad      */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_wferguson  Write CUB_SUR as Ferguson        */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval     * Surface evaluation routine                 */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_wferguson       */
/* SU2993 = Severe program error (  ) in varkon_sur_wferguson       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_wferguson (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  char    *outfile )     /* Surface file with Ferguson bicubic data */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/* Data to file outfile                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

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
   DBint  nu;            /* Number of patches in U direction        */
   DBint  nv;            /* Number of patches in V direction        */
   EVALS  r00;           /* Corner point U=0 V=0                    */
   EVALS  r01;           /* Corner point U=0 V=1                    */
   EVALS  r10;           /* Corner point U=1 V=0                    */
   EVALS  r11;           /* Corner point U=1 V=1                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint    icase;       /* Derivative flag for varkon_sur_eval     */
   DBfloat  us,ue,vs,ve; /* Patch corner parameter values           */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_g;         /* Current geometric   patch         (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
   char     errbuf[80];  /* String for error message fctn erpush    */

   DBint    status;      /* Error code from a called function       */

   FILE    *f;           /* Pointer to the output file              */

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
  "sur290 Enter*varkon_sur_wferguson* outfile %s \n", outfile );
  }
#endif

/*!                                                                 */
/* Number of patches in U and V from p_sur                          */
/*                                                                 !*/

nu = p_sur->nu_su; 
nv = p_sur->nv_su;

/*!                                                                 */
/* Open the output file (C function fopen)                          */
/*                                                                 !*/
/* (Program error SU2993 for open failure)                          */

if ( (f= fopen(outfile,"w")) == NULL )
 {
 sprintf(errbuf, "(open)%%varkon_sur_wferguson");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Print total number of patches: total  NU  NV                     */
/*                                                                 !*/


   fprintf(f,"total %d %d \n",(int)nu,(int)nv);

/*!                                                                 */
/* 2. Write Ferguson surface data                                   */
/* ______________________________                                   */
/*                                                                  */
/* Loop all U patches  iu=0,1,....,nu-1                             */
/*  Loop all V patches  iv=0,1,....,nv-1                            */
/*                                                                 !*/

for ( iu=0; iu<nu; ++iu )                /* Start loop U patches    */
  {
  for ( iv=0; iv<nv; ++iv )              /* Start loop V patches    */
    {

/*!                                                                 */
/*    Current patch pointer. Check patch type (CUB_PAT) and         */
/*    that offset is zero (0).                                      */
/*                                                                 !*/

    p_t = p_pat + iu*nv + iv;            /* Ptr to  current patch   */
    if (p_t->styp_pat != CUB_PAT)        /* Type of secondary patch */
       {                                 /* must be CUB_PAT         */
 sprintf(errbuf,                         /*                         */
 "(not CUB_PAT)%%varkon_sur_wferguson"); /*                         */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
       }

    p_gpat = p_t->spek_c;                /* Secondary patch (C ptr) */
    p_g    = (GMPATC *)p_gpat;           /* Current geometric patch */

    if (fabs(p_g->ofs_pat) > 0.01 )      /* Offset for the patch    */
       {                                 /* must be zero (0)        */
 sprintf(errbuf,                         /*                         */
 "(offset not 0)%%varkon_sur_wferguson");/*                         */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
       }


/*!                                                                 */
/*    Print patch address: patch  IU  IV                            */
/*                                                                 !*/


    fprintf(f,"patch %d %d \n"           /* Patch adress            */
    ,(int)p_t->iu_pat,(int)p_t->iv_pat );          /*                         */

    us = p_t->us_pat;                    /* Start U on geom. patch  */
    ue = p_t->ue_pat;                    /* End   U on geom. patch  */
    vs = p_t->vs_pat;                    /* Start V on geom. patch  */
    ve = p_t->ve_pat;                    /* End   V on geom. patch  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 Patch iu %d iv %d p_t %d p_g %d p_gpat %d \n", 
     (int)iu , (int)iv, (int)p_t,(int)p_g,(int)p_gpat);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 us %f vs %f ue %f ve %f\n", us,vs,ue,ve);
  }
#endif

/*!                                                                 */
/*    Calculate corner points for current patch.                    */
/*    Calls of varkon_sur_eval (sur210).                            */
/*                                                                 !*/

   icase = 3;


   status= varkon_sur_eval
(p_sur,p_pat,icase,us,vs,&r00);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur290 varkon_sur_eval failed for us %f vs %f\n",us,vs); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_sur_wferguson");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 us %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us, vs, r00.r_x, r00.r_y, r00.r_z); 
  }
#endif

   status= varkon_sur_eval
(p_sur,p_pat,icase,ue,vs,&r10);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur290 varkon_sur_eval failed for ue %f vs %f\n",ue,vs); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_sur_wferguson");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 ue %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, vs, r10.r_x, r10.r_y, r10.r_z); 
  }
#endif

   status= varkon_sur_eval
(p_sur,p_pat,icase,ue,ve,&r11);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur290 varkon_sur_eval failed for ue %f ve %f\n",ue,ve); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_sur_wferguson");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 ue %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, ve, r11.r_x, r11.r_y, r11.r_z); 
  }
#endif

   status= varkon_sur_eval
(p_sur,p_pat,icase,us,ve,&r01);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur290 varkon_sur_eval failed for us %f ve %f\n",us,ve); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_sur_wferguson");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 us %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us, ve, r01.r_x, r01.r_y, r01.r_z); 
  }
#endif


/*!                                                                 */
/*    Print row 1: r00   r01   rv00  rv01                           */
/*                                                                 !*/

    fprintf(f,                           /* r00                     */
    "r00   %21.14f %21.14f %21.14f\n"    /*                         */
    ,r00.r_x,r00.r_y,r00.r_z );          /*                         */
    fprintf(f,                           /* r01                     */
    "r01   %21.14f %21.14f %21.14f\n"    /*                         */
    ,r01.r_x,r01.r_y,r01.r_z );          /*                         */
    fprintf(f,                           /* rv00                    */
    "rv00  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r00.v_x,r00.v_y,r00.v_z );          /*                         */
    fprintf(f,                           /* rv01                    */
    "rv01  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r01.v_x,r01.v_y,r01.v_z );          /*                         */

/*!                                                                 */
/*    Print row 2: r10   r11   rv10  rv11                           */
/*                                                                 !*/

    fprintf(f,                           /* r10                     */
    "r10   %21.14f %21.14f %21.14f\n"    /*                         */
    ,r10.r_x,r10.r_y,r10.r_z );          /*                         */
    fprintf(f,                           /* r11                     */
    "r11   %21.14f %21.14f %21.14f\n"    /*                         */
    ,r11.r_x,r11.r_y,r11.r_z );          /*                         */
    fprintf(f,                           /* rv10                    */
    "rv10  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r10.v_x,r10.v_y,r10.v_z );          /*                         */
    fprintf(f,                           /* rv11                    */
    "rv11  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r11.v_x,r11.v_y,r11.v_z );          /*                         */


/*!                                                                 */
/*    Print row 3: ru00  ru01  ruv00 ruv01                          */
/*                                                                 !*/

    fprintf(f,                           /* ru00                    */
    "ru00  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r00.u_x,r00.u_y,r00.u_z );          /*                         */
    fprintf(f,                           /* ru01                    */
    "ru01  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r01.u_x,r01.u_y,r01.u_z );          /*                         */
    fprintf(f,                           /* ruv00                   */
    "ruv00 %21.14f %21.14f %21.14f\n"    /*                         */
    ,r00.uv_x,r00.uv_y,r00.uv_z );       /*                         */
    fprintf(f,                           /* ruv01                   */
    "ruv01 %21.14f %21.14f %21.14f\n"    /*                         */
    ,r01.uv_x,r01.uv_y,r01.uv_z );       /*                         */

/*!                                                                 */
/*    Print row 4: ru10  ru11  ruv10 ruv11                          */
/*                                                                 !*/

    fprintf(f,                           /* ru10                    */
    "ru10  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r10.u_x,r10.u_y,r10.u_z );          /*                         */
    fprintf(f,                           /* ru11                    */
    "ru11  %21.14f %21.14f %21.14f\n"    /*                         */
    ,r11.u_x,r11.u_y,r11.u_z );          /*                         */
    fprintf(f,                           /* ruv10                   */
    "ruv10 %21.14f %21.14f %21.14f\n"    /*                         */
    ,r10.uv_x,r10.uv_y,r10.uv_z );       /*                         */
    fprintf(f,                           /* ruv11                   */
    "ruv11 %21.14f %21.14f %21.14f\n"    /*                         */
    ,r11.uv_x,r11.uv_y,r11.uv_z );       /*                         */


    }                                    /* End   loop V patches    */
  }                                      /* End   loop U patches    */
/*!                                                                 */
/*  End  all V patches  iv=0,1,....,nv-1                            */
/* End  all U patches  iu=0,1,....,nu-1                             */
/*                                                                 !*/

/*!                                                                 */
/* Print End Of File: EOF   NU*NV                                   */
/*                                                                 !*/


    fprintf(f,"EOF   %d \n",(int)(nu*nv));

/*!                                                                 */
/* Close the output file (C function fclose)                        */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur290 Exit*varkon_sur_wferguson* No patches nu= %d nv= %d \n",
             (int)nu,(int)nv);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
