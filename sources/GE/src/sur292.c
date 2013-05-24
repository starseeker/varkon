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
/*  Function: varkon_sur_wnmgcatia                   File: sur292.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function writes a CUB_SUR surface to an output file.        */
/*                                                                  */
/*  The function writes surface data in the NMG to Catia            */
/*  interface format.                                               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-17   Originally written                                 */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1997-02-09   Eliminated compilation warnings                    */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_wnmgcatia  CUB_SUR to NMG->Catia format     */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_wnmgcatia       */
/* SU2993 = Severe program error (  ) in varkon_sur_wnmgcatia       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_wnmgcatia (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  char    *outfile,      /* Surface file with NMG->Catia data       */
  char    *s_model )     /* Model name in Catia                     */
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

/*!----- Description of the data in the NMG->Catia interface file --*/
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
/*  dum1 No coeff. Patch no. dum2 Type dum3 dum4 nu-1 nv-1 dum5     */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/*                                                                  */
/*                                                                  */
/*  The order of the coefficients for a patch:                      */
/*  -----------------------------------------                       */
/*                                                                  */
/*  Line Coefficients   (    for    )                               */
/*                                                                  */
/*   1   a00x a00y a00z (1.0  * 1.0 )                               */
/*   2   a10x a10y a10z (U    * 1.0 )                               */
/*   3   a20x a20y a20z (U**2 * 1.0 )                               */
/*   4   a30x a30y a30z (U**3 * 1.0 )                               */
/*   5   a01x a01y a01z (1.0  * V   )                               */
/*   6   a11x a11y a11z (U    * V   )                               */
/*   7   a21x a21y a21z (U**2 * V   )                               */
/*   8   a31x a31y a31z (U**3 * V   )                               */
/*   9   a02x a02y a02z (1.0  * V**2)                               */
/*  10   a12x a12y a12z (U    * V**2)                               */
/*  11   a22x a22y a22z (U**2 * V**2)                               */
/*  12   a32x a32y a32z (U**3 * V**2)                               */
/*  13   a03x a03y a03z (1.0  * V**3)                               */
/*  14   a13x a13y a13z (U    * V**3)                               */
/*  15   a23x a23y a23z (U**2 * V**3)                               */
/*  16   a33x a33y a34z (U**3 * V**3)                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  iu;            /* Loop index corresponding to u line      */
   DBint  iv;            /* Loop index corresponding to v line      */
   DBint  nu;            /* Number of patches in U direction        */
   DBint  nv;            /* Number of patches in V direction        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  us,ue,vs,ve; /* Patch corner parameter values           */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_g;         /* Current geometric   patch         (ptr) */
   char   *p_gpat;       /* Allocated area geom.  patch data  (ptr) */
   char    s_user[13];   /* String for USER                         */
   char    s_date[10];   /* String for DATE                         */
   char    s_time[9];    /* String for TIME                         */
   char    s_type[18];   /* String for SURFACE                      */
   char    s_surf1[73];  /* String for SURF1                        */
   char    s_swdsc[81];  /* String for SWDSC                        */
   char    s_endsc[81];  /* String for ENDSC                        */
   char    s_text1[81];  /* String for TEXT1                        */
   char    s_text2[81];  /* String for TEXT2                        */
   char    s_surf[73];   /* String for surface name                 */
   char    s_space[133]; /* String for spaces                       */
   char    errbuf[80];   /* String for error message fctn erpush    */

   FILE   *f;            /* Pointer to the output file              */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
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
  "sur292 Enter*varkon_sur_wnmgcatia* outfile %s \n", outfile );
  }
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
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
 sprintf(errbuf, "(open)%%varkon_sur_wnmgcatia");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Print header for file                                            */
/*                                                                 !*/

   sprintf(s_user, "GEO_GUNNAR  ");  
   sprintf(s_date, "15-MAY-94");  
   sprintf(s_time, "12:51:16");  
   sprintf(s_type, "SURFACE         ");  

   sprintf(s_surf1, " Satellit-reflektor-till-ODIN            ");  
   sprintf(s_swdsc, 
"                                                                        ");  
   sprintf(s_endsc, 
"                                                                        ");  
   sprintf(s_text1, "                                           ");  
   sprintf(s_text2, "                                           ");  
   sprintf(s_surf, " Satellit-reflektor-ODIN       ");  

fprintf(f,"USER  = %s DATE AND TIME = %s"
,s_user, s_date);
fprintf(f," %s TYPE = %s\n"
,s_time, s_type);

fprintf(f,"MODELN= %s\n" ,s_model);
sprintf(s_space, "                          ");  
fprintf(f,"NCOM  = %4d%s\n" ,0,s_space);
fprintf(f,"SURF1 = %s\n" ,s_surf1);
fprintf(f,"SWDSC = %s\n" ,s_swdsc);
fprintf(f,"ENDSC = %s\n" ,s_endsc);
fprintf(f,"TEXT1 = %s\n" ,s_text1);
fprintf(f,"TEXT2 = %s\n" ,s_text2);
fprintf(f,"U & V = PART FROM MODEL :%4d%4d%4d%4d. APPROX SURFACE :%4d%4d             \n"
 ,1,(int)nu+1,1,(int)nv+1,(int)nu+1,(int)nv+1);

fprintf(f,"                                                      \n");

fprintf(f,"%s %5d                                         \n" 
,s_surf,(int)(nu*nv));

/*!                                                                 */
/* 2. Write NMG->Catia surface data                                 */
/* _________________________________                                */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

/*!                                                                 */
/*    Current patch pointer. Check patch type (CUB_PAT) and         */
/*    that offset is zero (0).                                      */
/*                                                                 !*/

    p_t = p_pat + iu*nv + iv;            /* Ptr to  current patch   */
    if (p_t->styp_pat != CUB_PAT)        /* Type of secondary patch */
       {                                 /* must be CUB_PAT         */
       sprintf(errbuf, "(not CUB_PAT)%%varkon_sur_wnmgcatia");
       return(varkon_erpush("SU2993",errbuf));
       }

    p_gpat = p_t->spek_c;                /* Secondary patch (C ptr) */
    p_g    = (GMPATC *)p_gpat;           /* Current geometric patch */

    if (fabs(p_g->ofs_pat) > 0.01 )      /* Offset for the patch    */
       {                                 /* must be zero (0)        */
       sprintf(errbuf, "(offset not 0)%%varkon_sur_wnmgcatia");
       return(varkon_erpush("SU2993",errbuf));
       }


/*!                                                                 */
/*    Print patch data header                                       */
/*                                                                 !*/

    fprintf(f,"  %5d%5d%5d%5d%5d%5d%5d%5d%5d%5d                            \n"
    ,1,48,(int)(iu*nv+iv+1),0,1001,0,0,(int)nu+1,(int)nv+1,0);

    us = p_t->us_pat;                    /* Start U on geom. patch  */
    ue = p_t->ue_pat;                    /* End   U on geom. patch  */
    vs = p_t->vs_pat;                    /* Start V on geom. patch  */
    ve = p_t->ve_pat;                    /* End   V on geom. patch  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur292 Patch iu %d iv %d p_t %d p_g %d p_gpat %d \n", 
     (int)iu , (int)iv, (int)p_t,(int)p_g,(int)p_gpat);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur292 Kolla !!! us %f vs %f ue %f ve %f\n", us,vs,ue,ve);
  }
 fflush(dbgfil(SURPAC)); 
#endif

/*!                                                                 */
/*    Print coefficients for the cubic patch                        */
/*                                                                 !*/

/*                                                                  */
/*    Print row  1: a00x a00y a00z                                  */
/*                                                                  */

   sprintf(s_space, "            ");  
    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a00x,p_g->a00y,p_g->a00z, s_space);

/*                                                                  */
/*    Print row  2: a10x a10y a10z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a10x,p_g->a10y,p_g->a10z, s_space);

/*                                                                  */
/*    Print row  3: a20x a20y a20z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a20x,p_g->a20y,p_g->a20z, s_space);

/*                                                                  */
/*    Print row  4: a30x a30y a30z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a30x,p_g->a30y,p_g->a30z, s_space);

/*                                                                  */
/*    Print row  5: a01x a01y a01z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a01x,p_g->a01y,p_g->a01z, s_space);

/*                                                                  */
/*    Print row  6: a11x a11y a11z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a11x,p_g->a11y,p_g->a11z, s_space);

/*                                                                  */
/*    Print row  7: a21x a21y a21z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a21x,p_g->a21y,p_g->a21z, s_space);

/*                                                                  */
/*    Print row  8: a31x a31y a31z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a31x,p_g->a31y,p_g->a31z, s_space);

/*                                                                  */
/*    Print row  9: a02x a02y a02z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a02x,p_g->a02y,p_g->a02z, s_space);

/*                                                                  */
/*    Print row 10: a12x a12y a12z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a12x,p_g->a12y,p_g->a12z, s_space);

/*                                                                  */
/*    Print row 11: a22x a22y a22z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a22x,p_g->a22y,p_g->a22z, s_space);

/*                                                                  */
/*    Print row 12: a32x a32y a32z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a32x,p_g->a32y,p_g->a32z, s_space);

/*                                                                  */
/*    Print row 13: a03x a03y a03z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a03x,p_g->a03y,p_g->a03z, s_space);

/*                                                                  */
/*    Print row 14: a13x a13y a13z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a13x,p_g->a13y,p_g->a13z, s_space);

/*                                                                  */
/*    Print row 15: a23x a23y a23z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a23x,p_g->a23y,p_g->a23z, s_space);

/*                                                                  */
/*    Print row 16: a33x a33y a33z                                  */
/*                                                                  */

    fprintf(f," %20.11f %20.11f %20.11f     %s\n"   
    ,p_g->a33x,p_g->a33y,p_g->a33z, s_space);


    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

/*!                                                                 */
/* Close the output file (C function fclose)                        */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur292 Exit*varkon_sur_wnmgcatia* No patches nu= %d nv= %d \n",
             (int)nu,(int)nv);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
