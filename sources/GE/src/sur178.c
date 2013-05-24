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
/*  Function: varkon_sur_aplane                    File: sur178.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Approximation of a surface with B-planes for table APLANE       */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-10   Originally written                                 */
/*  1996-06-15   Debug                                              */
/*  1996-08-20   Offset facets                                      */
/*  1998-02-08   Debug                                              */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_aplane     Approximate a surface for APLANE */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_pla_distpt       * Distance to a plane                    */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short crefac ();       /* Create two facets                  */
static short offset ();       /* Calculate offset (dist. to mid pt) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   no_fac;       /* Number of facets  in table APLANE   */
static DBfloat us_f;         /* U start point current two facets    */
static DBfloat vs_f;         /* V start point current two facets    */
static DBfloat ue_f;         /* U end   point current two facets    */
static DBfloat ve_f;         /* V end   point current two facets    */
static DBfloat off_f;        /* Offset for    current two facets    */
static DBfloat us;           /* U start point current PBOUND patch  */
static DBfloat vs;           /* V start point current PBOUND patch  */
static DBfloat ue;           /* U end   point current PBOUND patch  */
static DBfloat ve;           /* V end   point current PBOUND patch  */
static DBint   sur_no;       /* Surface number                      */
static DBint   rec_no;       /* Record number in PBOUND             */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_aplane          */
/* SU2973 = Internal function () failed in varkon_sur_aplane        */
/* SU2993 = Severe program error (  ) in varkon_sur_aplane          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

   DBstatus           varkon_sur_aplane (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Input surface                     (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   PBOUND  *p_pbtable,   /* Patch boundary table              (ptr) */
   DBint    pbn,         /* Number of records in pbtable            */
   DBint    req_sur_no,  /* Requested surface number 1 or 2         */
   APLANE  *p_pltable,   /* Approximation of surface          (ptr) */
   DBint   *p_pl )       /* Number of records in aplane             */

/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*      Data to p_pltable                                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  PBOUND *p_cr;          /* Current record in p_pbtable       (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   i_pat;         /* Loop index record in table PBOUND       */

  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */
#ifdef DEBUG
  DBint   i_fac;         /* Loop index facet  in table APLANE       */
#endif

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
"sur178 Enter*varkon_sur_aplane Surface number %d\n",(int)req_sur_no);
fflush(dbgfil(SURPAC)); 
}
#endif
 
   *p_pl = 0;            /* Number of records in aplane             */

/*!                                                                 */
/* 2. Create the approximate surface                                */
/* _________________________________                                */
/*                                                                  */
/*                                                                 !*/

   no_fac = 0;

/*!                                                                 */
/* Loop all records in PBOUND table i_pat=1,2,....,pbn              */
/*                                                                 !*/

for ( i_pat=0; i_pat<  pbn  ; ++i_pat)   /* Start loop PBOUND       */
  {

    if( no_fac >= PLMAX - 4 )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 PLMAX must be increased in surint.h !!! \n"); 
#endif
    sprintf(errbuf,"PLMAX too small%%varkon_sur_aplane");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*   Retrieve us, ue, vs and ve from PBOUND table                   */
/*                                                                 !*/

    p_cr   = p_pbtable + i_pat;          /* Ptr to current record   */
    us     = p_cr->us;                   /* U start point           */
    vs     = p_cr->vs;                   /* V start point           */
    ue     = p_cr->ue;                   /* U end   point           */
    ve     = p_cr->ve;                   /* V end   point           */
    sur_no = p_cr->sur_no;               /* Surface number          */
    rec_no = p_cr->rec_no;               /* Record number           */
    
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur178 i_pat %d sur_no %d, req_sur_no %d\n", 
      (int)i_pat, (int)sur_no , (int)req_sur_no ); 
fflush(dbgfil(SURPAC));
}
#endif

   if ( sur_no != req_sur_no ) goto nxtreg;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur178 i_pat %d Area: us %8.4f ue %8.4f vs %8.4f ve %8.4f\n",
   (int)i_pat, us,  ue,  vs,  ve   ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*   Approximate the patch with two B-plane triangles               */
/*   Call  of crefac.                                               */
/*                                                                 !*/

    us_f = us; /* New parameters for a loop with additional facets. */
    ue_f = ue; 
    vs_f = vs; 
    ve_f = ve; 

    off_f = 0.0;
    status= crefac (p_sur, p_pat, p_pltable);
    if (status<0) 
      {
      sprintf(errbuf,"crefac%%varkon_sur_aplane (sur178)");
      return(varkon_erpush("SU2973",errbuf));
    }

    status= offset (p_sur, p_pat, p_pltable);
    if (status<0) 
      {
      sprintf(errbuf,"offset%%varkon_sur_aplane (sur178)");
      return(varkon_erpush("SU2973",errbuf));
    }

    no_fac = no_fac + 2;

    if ( fabs(off_f) <  0.5 ) goto noadd; /* Should be input tolerance */


    status= crefac (p_sur, p_pat, p_pltable);
    if (status<0) 
      {
      sprintf(errbuf,"crefac%%varkon_sur_aplane (sur178)");
      return(varkon_erpush("SU2973",errbuf));
    }

    no_fac = no_fac + 2;

noadd:;
nxtreg:;

    }                                    /* End loop PBOUND         */

/*!                                                                 */
/*     End loop all records in PBOUND table i_pat=1,2,....,pbn      */

/*!                                                                 */
/* 5. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                 !*/

   *p_pl = no_fac;       /* Number of records in aplane             */

#ifdef DEBUG                             /* Debug printout          */


if ( dbglev(SURPAC) == 2 )
{
for ( i_fac=0; i_fac< no_fac; ++i_fac)   /* Start loop APLANE       */
{
fprintf(dbgfil(SURPAC),
"sur178 p1 %9.1f %9.1f %9.1f uv %7.2f %7.2f %d\n",
 (p_pltable+ i_fac  )->bpl.crd1_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd1_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd1_bp.z_gm,  
 (p_pltable+ i_fac  )->u1bpl,
 (p_pltable+ i_fac  )->u1bpl, (int)i_fac+1);
fprintf(dbgfil(SURPAC),
"sur178 p2 %9.1f %9.1f %9.1f uv %7.2f %7.2f\n",
 (p_pltable+ i_fac  )->bpl.crd2_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd2_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd2_bp.z_gm,  
 (p_pltable+ i_fac  )->u2bpl,
 (p_pltable+ i_fac  )->u2bpl);
fprintf(dbgfil(SURPAC),
"sur178 p3 %9.1f %9.1f %9.1f \n",
 (p_pltable+ i_fac  )->bpl.crd3_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd3_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd3_bp.z_gm); 
fprintf(dbgfil(SURPAC),
"sur178 p4 %9.1f %9.1f %9.1f uv %7.2f %7.2f\n",
 (p_pltable+ i_fac  )->bpl.crd4_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd4_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd4_bp.z_gm,  
 (p_pltable+ i_fac  )->u4bpl,
 (p_pltable+ i_fac  )->u4bpl);
}
}

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "GLOBAL GEOMETRY MODULE surcura1(); \n" );
  fprintf(dbgfil(SURPAC), "BEGINMODULE  \n" );
  }

if ( dbglev(SURPAC) == 2 )
{
for ( i_fac=0; i_fac< no_fac; ++i_fac)   /* Start loop APLANE       */
{

fprintf(dbgfil(SURPAC),
"  B_PLANE( #%d, VEC(%9.1f,%9.1f,%9.1f),VEC(%9.1f,%9.1f,%9.1f), \n",
          (int)i_fac+1,
 (p_pltable+ i_fac  )->bpl.crd1_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd1_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd1_bp.z_gm,  
 (p_pltable+ i_fac  )->bpl.crd2_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd2_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd2_bp.z_gm);
fprintf(dbgfil(SURPAC),
"                VEC(%9.1f,%9.1f,%9.1f),VEC(%9.1f,%9.1f,%9.1f));\n",
 (p_pltable+ i_fac  )->bpl.crd3_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd3_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd3_bp.z_gm,  
 (p_pltable+ i_fac  )->bpl.crd4_bp.x_gm,  
 (p_pltable+ i_fac  )->bpl.crd4_bp.y_gm,  
 (p_pltable+ i_fac  )->bpl.crd4_bp.z_gm);
}
}

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "ENDMODULE    \n" );
  }



if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur178 Exit*varkon_sur_aplane*Number of p_pltable records %d (PLMAX %d)\n"
         ,(int)*p_pl ,PLMAX );
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/

/*!********* Internal ** function ** crefac  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates two facets                                  */

   static short crefac (p_sur, p_pat, p_pltable )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Input surface                     (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   APLANE  *p_pltable;   /* Approximation of surface          (ptr) */
/*        no_fac            Number of facets  in table APLANE       */
/* us_f, vs_f, ue_f, ve_f   UV values for two facets                */
/*        off_f             Offset for current two facets           */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

  DBint  icase;          /* Case for varkon_sur_eval                */
  EVALS  xyz;            /* Coordinates and derivatives for a       */
                         /* point on a surface                      */

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
  "sur178*crefac * Enter ** no_fac= %d \n", (int)no_fac);
  }
#endif


/*!                                                                 */
/*   Approximate the patch with two B-plane triangles               */
/*   Calls of varkon_sur_eval (sur210)                              */
/*                                                                 !*/

   icase = 3;

   status= varkon_sur_eval
(p_sur,p_pat,icase,us_f,vs_f,&xyz);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 sur210 failed for us_f %f vs_f %f\n",us_f,vs_f); 
#endif
    sprintf(errbuf,"sur210(us_f,vs_f)%%sur178");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178*crefac us_f %8.4f vs_f %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us_f, vs_f, xyz.r_x, xyz.r_y, xyz.r_z ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178*crefac NX %10.2f NY %10.2f NZ %10.2f off_f %7.2f\n",
     xyz.n_x, xyz.n_y, xyz.n_z, off_f ); 
  }
#endif

  (p_pltable+no_fac  )->bpl.crd4_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac  )->bpl.crd4_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac  )->bpl.crd4_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac+1)->bpl.crd1_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac+1)->bpl.crd1_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac+1)->bpl.crd1_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac  )->u4bpl            = us_f;         
  (p_pltable+no_fac  )->v4bpl            = vs_f;         
  (p_pltable+no_fac+1)->u1bpl            = us_f;         
  (p_pltable+no_fac+1)->v1bpl            = vs_f;         
 

   status= varkon_sur_eval
(p_sur,p_pat,icase,ue_f,vs_f,&xyz);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 sur210 failed for ue_f %f vs_f %f icase %d\n",
   ue_f,vs_f, (int)icase); 
#endif
    sprintf(errbuf,"sur210(ue_f,vs_f)%%sur178");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178 ue_f %8.4f vs_f %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue_f, vs_f, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  (p_pltable+no_fac+1)->bpl.crd2_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac+1)->bpl.crd2_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac+1)->bpl.crd2_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac+1)->bpl.crd3_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac+1)->bpl.crd3_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac+1)->bpl.crd3_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac+1)->u2bpl            = ue_f;         
  (p_pltable+no_fac+1)->v2bpl            = vs_f;         

   status= varkon_sur_eval
(p_sur,p_pat,icase,ue_f,ve_f,&xyz);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 sur210 failed for ue_f %f ve_f %f\n",ue_f,ve_f); 
#endif
    sprintf(errbuf,"sur210 ue_f,ve_f%%sur178");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178 ue_f %8.4f ve_f %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue_f, ve_f, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  (p_pltable+no_fac  )->bpl.crd1_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac  )->bpl.crd1_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac  )->bpl.crd1_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac+1)->bpl.crd4_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac+1)->bpl.crd4_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac+1)->bpl.crd4_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac  )->u1bpl            = ue_f;         
  (p_pltable+no_fac  )->v1bpl            = ve_f;         
  (p_pltable+no_fac+1)->u4bpl            = ue_f;         
  (p_pltable+no_fac+1)->v4bpl            = ve_f;         

   status= varkon_sur_eval
(p_sur,p_pat,icase,us_f,ve_f,&xyz);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 sur210 failed for us_f %f ve_f %f\n",us_f,ve_f); 
#endif
    sprintf(errbuf,"sur210(us_f,ve_f)%%sur178");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178 us %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us_f, ve_f, xyz.r_x, xyz.r_y, xyz.r_z); 
  }
#endif

  (p_pltable+no_fac  )->bpl.crd2_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac  )->bpl.crd2_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac  )->bpl.crd2_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac  )->bpl.crd3_bp.x_gm = xyz.r_x+off_f*xyz.n_x; 
  (p_pltable+no_fac  )->bpl.crd3_bp.y_gm = xyz.r_y+off_f*xyz.n_y; 
  (p_pltable+no_fac  )->bpl.crd3_bp.z_gm = xyz.r_z+off_f*xyz.n_z; 
  (p_pltable+no_fac  )->u2bpl            = us_f;         
  (p_pltable+no_fac  )->v2bpl            = ve_f;         


  (p_pltable+no_fac  )->rec_no = rec_no;   
  (p_pltable+no_fac  )->us     = us;      
  (p_pltable+no_fac  )->vs     = vs;     
  (p_pltable+no_fac  )->ue     = ue;    
  (p_pltable+no_fac  )->ve     = ve;   
  (p_pltable+no_fac  )->sur_no = sur_no;
  (p_pltable+no_fac+1)->rec_no = rec_no;   
  (p_pltable+no_fac+1)->us     = us;      
  (p_pltable+no_fac+1)->vs     = vs;     
  (p_pltable+no_fac+1)->ue     = ue;    
  (p_pltable+no_fac+1)->ve     = ve;   
  (p_pltable+no_fac+1)->sur_no = sur_no;





    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** offset  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates distance to mid point on surface         */

   static short offset (p_sur, p_pat, p_pltable )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Input surface                     (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   APLANE  *p_pltable;   /* Approximation of surface          (ptr) */
/*        no_fac            Number of facets  in table APLANE       */
/* us_f, vs_f, ue_f, ve_f   UV values for two facets                */
/*                                                                  */
/* Out:                                                             */
/*        off_f             Offset for next two facets              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
  DBfloat u_m;           /* U value for midpoint                    */
  DBfloat v_m;           /* V value for midpoint                    */
/*                                                                  */
/*----------------------------------------------------------------- */

  DBVector p1;           /* Plane   point 1                   (ptr) */
  DBVector p2;           /* Plane   point 2                   (ptr) */
  DBVector p3;           /* Plane   point 3                   (ptr) */
  DBVector ext;          /* External point                    (ptr) */
  DBVector ref;          /* Reference vector for sign         (ptr) */
                         /* Zero vector for undefined vector        */

  DBint  icase;          /* Case for varkon_sur_eval                */
  EVALS  xyz;            /* Coordinates and derivatives for a       */
                         /* point on a surface                      */

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
  "sur178*offset * Enter ** no_fac= %d \n", (int)no_fac);
  }
#endif

    off_f = F_UNDEF;

/*!                                                                 */
/*   Calculate the mid point                                        */
/*   Call of varkon_sur_eval (sur210)                               */
/*                                                                 !*/

   icase = 3;

   u_m = us_f + 0.5*(ue_f-us_f);
   v_m = vs_f + 0.5*(ve_f-vs_f);


   status= varkon_sur_eval
(p_sur,p_pat,icase,u_m,v_m,&xyz);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 sur210 failed for u_m %f v_m %f\n",u_m,v_m); 
#endif
    sprintf(errbuf,"sur210(u_m,v_m)%%sur178");
    return(varkon_erpush("SU2943",errbuf));
    }

  ext.x_gm = xyz.r_x;
  ext.y_gm = xyz.r_y;
  ext.z_gm = xyz.r_z;
  ref.x_gm = xyz.n_x;
  ref.y_gm = xyz.n_y;
  ref.z_gm = xyz.n_z;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur178*offset u_m %8.4f v_m %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_m, v_m, xyz.r_x, xyz.r_y, xyz.r_z ); 
  }
#endif

/*!                                                                 */
/*   Calculate the plane from previous facet                        */
/*                                                                 !*/

  p1.x_gm = (p_pltable+no_fac+1)->bpl.crd1_bp.x_gm; 
  p1.y_gm = (p_pltable+no_fac+1)->bpl.crd1_bp.y_gm; 
  p1.z_gm = (p_pltable+no_fac+1)->bpl.crd1_bp.z_gm; 

  p2.x_gm = (p_pltable+no_fac+1)->bpl.crd2_bp.x_gm; 
  p2.y_gm = (p_pltable+no_fac+1)->bpl.crd2_bp.y_gm; 
  p2.z_gm = (p_pltable+no_fac+1)->bpl.crd2_bp.z_gm; 

  p3.x_gm = (p_pltable+no_fac+1)->bpl.crd4_bp.x_gm; 
  p3.y_gm = (p_pltable+no_fac+1)->bpl.crd4_bp.y_gm; 
  p3.z_gm = (p_pltable+no_fac+1)->bpl.crd4_bp.z_gm; 

   status= varkon_pla_distpt
(  &p1,  &p2,  &p3,  &ext,  &ref,  &off_f );
    if(status<0)
    {
    off_f = 0.0;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 varkon_pla_distp (sur711) failed off_f %f\n",off_f); 
#endif
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  fprintf(dbgfil(SURPAC),
  "sur178*offset Exit off_f %f\n",off_f); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




