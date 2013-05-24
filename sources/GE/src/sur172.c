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
/*  Function: varkon_sur_bicapprox                 File: sur172.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function approximates a surface of any type to a            */
/*  bicubic surface (CUB_SUR).                                      */
/*                                                                  */
/*  Note that tolerances for the computation is input to            */
/*  the function.                                                   */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  TODO Note that the function checks an aprroximate surface       */
/*       without checking that the new surface is within any        */
/*       position or surface normal tolerances                      */
/*       The calculation of the tangent vector lengths is also      */
/*       too simple. Lengths should be calculated based on a        */
/*       equation system, or some simple numerical method           */
/*       making for instance the curvature equal in the             */
/*       corner points                                              */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1995-03-03   No LFT_SUR patches 5 -> 10                         */
/*  1995-03-10   No patches np_u, np_v input data for case 1        */
/*  1996-05-28   Erase of unused variables                          */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*               CON_SUR added. Debug outprint changes              */
/*  1997-03-17   Bug np_v= 0                                        */
/*  1998-02-01   NURBS added                                        */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_bicapprox  Approximate to CUB_SUR surface   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol();       * Retrieve computer tolerance            */
/* varkon_ini_gmsur();     * Initiate DBSurf                        */
/* varkon_ini_pbound();    * Initiate PBOUND                        */
/* varkon_sur_nopatch();   * Retrieve number of patches             */
/* *v3mall();              * Allocate memory                        */
/* v3free();               * Free allocated memory                  */
/* varkon_sur_defpatb();   * Define calc. patch boundaries          */
/* varkon_sur_biccre1();   * Bicubic patch from corner pts          */
/* varkon_sur_eval();      * Surface evaluation routine             */
/* varkon_sur_bound();     * Bounding boxes and cones               */
/* varkon_pat_pribic();    * Print bicubic patch data               */
/* varkon_erinit();        * Initiate error message stack           */
/* varkon_erpush();        * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short adjustm();       /* Adjustment of vector lengths       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_bicapprox       */
/* SU2993 = Severe program error (  ) in varkon_sur_bicapprox       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_bicapprox (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBfloat  s_lim[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /* All equal zero => whole surface         */
  DBint    acase,        /* Approximation case:                     */
                         /* Eq.  1: Number of patches defined by    */
                         /*         the user (np_u and np_v)        */
                         /* Eq. 11: As 1 but no bound. boxes/cones  */
  DBint    np_u,         /* Number of approx patches per patch in U */
  DBint    np_v,         /* Number of approx patches per patch in V */
  DBint    tcase,        /* Tolerance band case                     */
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
  DBfloat us,ue,vs,ve;   /* Limits        for the (PBOUND) patch    */
  DBint   maxnum;        /* Maximum number of patches (alloc. area) */
  DBint   acc;           /* Computation accuracy                    */
  DBPatch *p_frst;       /* Pointer to the first patch              */
  DBPatch *p_t;          /* Current topological patch         (ptr) */
  GMPATC *p_patcc;       /* Current geometric   patch         (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   i_r;           /* Loop index segment record               */
  DBint   acc_box;       /* For sur911                              */
  DBint   iu,iv;         /* Patch address                           */
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  char   *p_gpat;        /* Allocated area geom.  patch data  (ptr) */
  char   *p_tpat;        /* Allocated area topol. patch data  (ptr) */
  DBint   icase;         /* Case for varkon_sur_eval                */
  EVALS   r00;           /* Corner point U=0 V=0                    */
  EVALS   r01;           /* Corner point U=0 V=1                    */
  EVALS   r10;           /* Corner point U=1 V=0                    */
  EVALS   r11;           /* Corner point U=1 V=1                    */
  DBint   surtype;       /* Type CUB_SUR, RAT_SUR, or ...           */
  DBint   pat_no;        /* Patch number                            */
  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */
  DBfloat dum1;          /* Dummy float                             */

#ifdef  DEBUG 
  GMPATC *p_patc;        /* Bicubic patch                    (ptr)  */
#endif


/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur172 Enter***varkon_sur_bicapprox acase %d\n" , (short)acase );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur172 Input    np_u %d np_v %d\n" , (short)np_u, (short)np_v );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

   *pp_patout = NULL; 

/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur (p_surout); 

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   For Debug On: Check that the surface type is implemented       */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_bicapprox");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     ==  CUB_SUR || 
     surtype     ==  RAT_SUR ||           
     surtype     ==  LFT_SUR ||           
     surtype     ==  CON_SUR ||           
     surtype     ==  POL_SUR ||           
     surtype     ==  PRO_SUR ||           
     surtype     ==   P3_SUR ||           
     surtype     ==   P5_SUR ||           
     surtype     ==   P7_SUR ||           
     surtype     ==   P9_SUR ||           
     surtype     ==  P21_SUR ||           
     surtype     == NURB_SUR ||           
     surtype     ==  MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf,                         /* Error SU2993 for invalid*/
 "(type)%%varkon_sur_bicapprox");        /* type                    */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }
#endif

/*!                                                                 */
/*   For Debug On: Printout of second bicubic patch for CUB_SUR     */
/*                                                                 !*/

#ifdef DEBUG
if ( surtype     == CUB_SUR )
  {
  p_patc = (GMPATC *)(p_pat+1)->spek_c;
    status = varkon_pat_pribic  (p_patc); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pribic%%varkon_sur_bicapprox");
    return(varkon_erpush("SU2943",errbuf));
    }
  }
#endif

/*!                                                                 */
/*   Check approximation case and input data for the calculation    */
/*                                                                 !*/

    if ( acase == 1 || acase == 11 )
      {
      if ( np_u < 1 )
        {
        sprintf(errbuf,"np_u %d %%varkon_sur_bicapprox", (short)np_u );
        varkon_erinit();
        return(varkon_erpush("SU2383",errbuf));
        }
      if ( np_v <= 0 )
        {
/*      np_u = np_v;      Bug 1997-03-17 */
        np_v = np_u;
        }
      }  /* End acase == 1 or acase == 11 */


    else
      {
      sprintf(errbuf," %d %%varkon_sur_bicapprox",(short)acase);
      varkon_erinit();
      return(varkon_erpush("SU2393",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur172 Modified np_u %d np_v %d\n" , (short)np_u, (short)np_v );
fflush(dbgfil(SURPAC));
}
#endif

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

   acc             = 1;       /* Not used  */
   appdat.ctype    = BICAPPR;
   appdat.method   = -1234;
   appdat.nstart   =  4;    
   appdat.maxiter  = 50;   
   appdat.acase    = acase;
   appdat.acc      = acc;
   appdat.offset   = -1.23456789;
   appdat.thick    = -1.23456789;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur172 APPROX: ctype %d method %d nstart %d\n",
(short)appdat.ctype,(short)appdat.method,(short)appdat.nstart);

fprintf(dbgfil(SURPAC),
"sur172 APPROX: maxiter %d acase %d acc %d ctol %f ntol %f\n",
(short)appdat.maxiter,(short)appdat.acase ,(short)appdat.acc, 
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
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

   maxnum = nu*nv*np_u*np_v;        

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 Allocation failed  nu %d nv %d DMAX %d maxnum %d\n",
    (short)nu, (short)nv, DMAX, (short)maxnum );
  }
#endif
 sprintf(errbuf, "(alloc)%%sur172");
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
  "sur172 Ptr to first patch p_frst  = %d No patches %d\n",
    (int)p_frst  ,(short)maxnum);
fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Surface type and number of patches:                              */
          p_surout->typ_su= CUB_SUR;
          p_surout->nu_su = (short)(nu*np_u);    
          p_surout->nv_su = (short)(nv*np_v);  
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Define area of surface for approximation                      */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

    if ( acase == 1 || acase == 11 ) pb_alloc = maxnum;        
    else                             pb_alloc = PBMAX;

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur172");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 PBOUND allocation failed  pb_alloc %d maxnum %d PBMAX %d\n",
    (short)pb_alloc, (short)maxnum, PBMAX );
  }
#endif
      sprintf(errbuf, "(PBOUND alloc)%%sur172");
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
"sur172 Memory is allocated and initialized for %d PBOUND records\n",
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
   (p_sur,p_pat,surtype,nu,nv,s_lim,BICAPPR,
      acc,np_u,np_v,pb_alloc,p_pbtable,&pbn);
   if (status<0) 
     {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
     sprintf(errbuf,"sur910%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur172 Surface nu %d nv %d surtype %d PBOUND records %d\n",
           (short)nu, (short)nv, (short)surtype , (short)pbn );
fflush(dbgfil(SURPAC));
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

iu = 1;
iv = 1;

for ( i_pat=1; i_pat<= pbn  ; ++i_pat)   /* Start loop PBOUND       */
  {

/*!                                                                 */
/*   Pointer to current patch ( !!  bara p_patcc f|r n{rvarande !!! */
/*                                                                 !*/

     pat_no = pat_no + 1;                /* Patch number 1, 2, 3, ..*/
#ifdef DEBUG
     if ( pat_no > maxnum )
       {
       if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
       sprintf(errbuf, "(maxnum)%%(sur172");
       return(varkon_erpush("SU2993",errbuf));
       }
#endif

    p_t = p_frst + pat_no - 1;           /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (bicubic) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
 {
 if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
 sprintf(errbuf, "(allocg)%%sur172"); 
 return(varkon_erpush("SU2993",errbuf));
 }

p_patcc= (GMPATC *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/*  Topological patch data to current patch p_t where               */
/*  p_t= (PATMAT  *)*pp_at + pat_no + 1                             */
/*                                                                 !*/

  p_t->styp_pat = CUB_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)iu;             /* Topological adress for  */
  p_t->iv_pat   = (short)iv;             /* current (this) patch    */
  p_t->us_pat= (DBfloat)iu+    comptol;  /* Start U on geom. patch  */
  p_t->ue_pat= (DBfloat)iu+1.0-comptol;  /* End   U on geom. patch  */
  p_t->vs_pat= (DBfloat)iv+    comptol;  /* Start V on geom. patch  */
  p_t->ve_pat= (DBfloat)iv+1.0-comptol;  /* End   V on geom. patch  */
  iv  = iv + 1;

  if ( iv == nv*np_v + 1 )
     {
     iv = 1;
     iu = iu + 1;
     }

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
/*   Retrieve us, ue, vs and ve from PBOUND table                   */
/*                                                                 !*/

     us     = (p_pbtable+i_pat-1)->us;
     vs     = (p_pbtable+i_pat-1)->vs;
     ue     = (p_pbtable+i_pat-1)->ue;
     ve     = (p_pbtable+i_pat-1)->ve;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 i_pat %d pat_no %d p_t %d p_patcc %d\n",
     (short)i_pat, (short)pat_no, (int)p_t, (int)p_patcc ); 
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 i_pat %d Area: us %8.4f ue %8.4f vs %8.4f ve %8.4f\n",
     (short)i_pat, us,  ue,  vs,  ve   ); 
fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/*   Calculate corner points for output patch.                      */
/*   Calls of varkon_sur_eval (sur210).                             */
/*                                                                 !*/

   icase = 3;


   status= varkon_sur_eval (p_sur,p_pat,icase,us,vs,&r00);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur172 varkon_sur_eval (sur210) failed for us %f vs %f\n",us,vs); 
#endif
    sprintf(errbuf,"sur210%%sur172");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 us %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f pat_no %d\n",
     us, vs, r00.r_x, r00.r_y, r00.r_z, (short)pat_no); 
fflush(dbgfil(SURPAC));
  }
#endif

   status= varkon_sur_eval (p_sur,p_pat,icase,ue,vs,&r10);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur172 varkon_sur_eval (sur210) failed for ue %f vs %f\n",ue,vs); 
#endif
    sprintf(errbuf,"sur210%%(sur172)");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 ue %8.4f vs %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, vs, r10.r_x, r10.r_y, r10.r_z); 
fflush(dbgfil(SURPAC));
  }
#endif

   status= varkon_sur_eval (p_sur,p_pat,icase,ue,ve,&r11);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur172 varkon_sur_eval (sur210) failed for ue %f ve %f\n",ue,ve); 
#endif
    sprintf(errbuf,"sur210%%sur172");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 ue %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     ue, ve, r11.r_x, r11.r_y, r11.r_z); 
  }
#endif

   status= varkon_sur_eval (p_sur,p_pat,icase,us,ve,&r01);
    if(status<0)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur172 varkon_sur_eval failed for us %f ve %f\n",us,ve); 
#endif
    sprintf(errbuf,"sur210%%sur172");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 us %8.4f ve %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     us, ve, r01.r_x, r01.r_y, r01.r_z); 
fflush(dbgfil(SURPAC));
  }
#endif


   status=adjustm ( us,ue,vs,ve, &r00,  &r10,  &r01,  &r11);
   if (status<0) 
   {
   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
   sprintf(errbuf,"adjustm%%sur172");
   return(varkon_erpush("SU2973",errbuf));
   }


   status= varkon_pat_biccre1 (&r00, &r10, &r01, &r11, p_patcc);
    if(status<0)
    {
     if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur172 varkon_pat_biccre1 failed \n"); 
#endif
    sprintf(errbuf,"sur254%%sur172");
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
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur172");
    sprintf(errbuf,"sur233%%sur172");
    return(varkon_erpush("SU2943",errbuf));
    }
  }
#endif


    }                                    /* End loop PBOUND         */

/*!                                                                 */
/*     End loop all records in PBOUND table i_pat=1,2,....,pbn      */


/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

   v3free(p_pbtable, "sur172");

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur172 PBOUND memory is freed \n");
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   if ( acase == 11 ) goto nocheck;

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout ,p_frst ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_bicapprox");
        return(varkon_erpush("SU2943",errbuf));
        }

nocheck:; /* Label: No check of surface                             */

/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 pbn %d pat_no %d \n", (short)pbn , (short)pat_no );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur172 Exit***varkon_sur_bicapprox ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
fflush(dbgfil(SURPAC));
  }
#endif

#ifdef TILLFALL
  if ( pbn != p_surout->nu_su*p_surout->nv_su )
    sprintf(errbuf,"nu,nv error %%varkon_sur_bicapprox");
#ifdef LNAME
    return(varkon_erpush("SU2943",errbuf));
#endif
#ifdef SNAME
    return(erpush("SU2943",errbuf));
#endif
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** adjustm ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adjusts derivative vector lengths                   */

   static short adjustm (us,ue,vs,ve, p_r00, p_r10, p_r01, p_r11)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat us,ue,vs,ve;  /* Limits        for the (PBOUND) patch    */
   EVALS  *p_r00;        /* Corner point U=0 , V=0            (ptr) */
   EVALS  *p_r10;        /* Corner point U=1 , V=0            (ptr) */
   EVALS  *p_r01;        /* Corner point U=0 , V=1            (ptr) */
   EVALS  *p_r11;        /* Corner point U=1 , V=1            (ptr) */


/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm rv00    %f %f %f\n",
       p_r00->v_x ,p_r00->v_y ,p_r00->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm rv01    %f %f %f\n",
       p_r01->v_x ,p_r01->v_y ,p_r01->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm rv10    %f %f %f\n",
       p_r10->v_x ,p_r10->v_y ,p_r10->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm rv11    %f %f %f\n",
       p_r11->v_x ,p_r11->v_y ,p_r11->v_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ru00    %f %f %f\n",
       p_r00->u_x ,p_r00->u_y ,p_r00->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ru01    %f %f %f\n",
       p_r01->u_x ,p_r01->u_y ,p_r01->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ruv00   %f %f %f\n",
       p_r00->uv_x ,p_r00->uv_y ,p_r00->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ruv01   %f %f %f\n",
       p_r01->uv_x ,p_r01->uv_y ,p_r01->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ru10    %f %f %f\n",
       p_r10->u_x ,p_r10->u_y ,p_r10->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ru11    %f %f %f\n",
       p_r11->u_x ,p_r11->u_y ,p_r11->u_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ruv10   %f %f %f\n",
       p_r10->uv_x ,p_r10->uv_y ,p_r10->uv_z  );

  fprintf(dbgfil(SURPAC),
  "sur172*adjustm ruv11   %f %f %f\n",
       p_r11->uv_x ,p_r11->uv_y ,p_r11->uv_z  );

  }
#endif

/*!                                                                 */
/* 2. Adjustment of vector lengths                                  */
/*                                                                 !*/

    p_r00->u_x = (ue-us)*p_r00->u_x;
    p_r00->u_y = (ue-us)*p_r00->u_y;
    p_r00->u_z = (ue-us)*p_r00->u_z;
    p_r01->u_x = (ue-us)*p_r01->u_x;
    p_r01->u_y = (ue-us)*p_r01->u_y;
    p_r01->u_z = (ue-us)*p_r01->u_z;
    p_r10->u_x = (ue-us)*p_r10->u_x;
    p_r10->u_y = (ue-us)*p_r10->u_y;
    p_r10->u_z = (ue-us)*p_r10->u_z;
    p_r11->u_x = (ue-us)*p_r11->u_x;
    p_r11->u_y = (ue-us)*p_r11->u_y;
    p_r11->u_z = (ue-us)*p_r11->u_z;

    p_r00->v_x = (ve-vs)*p_r00->v_x;
    p_r00->v_y = (ve-vs)*p_r00->v_y;
    p_r00->v_z = (ve-vs)*p_r00->v_z;
    p_r01->v_x = (ve-vs)*p_r01->v_x;
    p_r01->v_y = (ve-vs)*p_r01->v_y;
    p_r01->v_z = (ve-vs)*p_r01->v_z;
    p_r10->v_x = (ve-vs)*p_r10->v_x;
    p_r10->v_y = (ve-vs)*p_r10->v_y;
    p_r10->v_z = (ve-vs)*p_r10->v_z;
    p_r11->v_x = (ve-vs)*p_r11->v_x;
    p_r11->v_y = (ve-vs)*p_r11->v_y;
    p_r11->v_z = (ve-vs)*p_r11->v_z;

    p_r00->uv_x = (ue-us)*(ve-vs)*p_r00->uv_x;
    p_r00->uv_y = (ue-us)*(ve-vs)*p_r00->uv_y;
    p_r00->uv_z = (ue-us)*(ve-vs)*p_r00->uv_z;
    p_r01->uv_x = (ue-us)*(ve-vs)*p_r01->uv_x;
    p_r01->uv_y = (ue-us)*(ve-vs)*p_r01->uv_y;
    p_r01->uv_z = (ue-us)*(ve-vs)*p_r01->uv_z;
    p_r10->uv_x = (ue-us)*(ve-vs)*p_r10->uv_x;
    p_r10->uv_y = (ue-us)*(ve-vs)*p_r10->uv_y;
    p_r10->uv_z = (ue-us)*(ve-vs)*p_r10->uv_z;
    p_r11->uv_x = (ue-us)*(ve-vs)*p_r11->uv_x;
    p_r11->uv_y = (ue-us)*(ve-vs)*p_r11->uv_y;
    p_r11->uv_z = (ue-us)*(ve-vs)*p_r11->uv_z;

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

