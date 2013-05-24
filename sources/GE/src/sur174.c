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
/*  Function: varkon_sur_tobspline                 File: sur174.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function converts or approximates a surface of any type     */
/*  to a B-spline surface (BSPL_SUR).                               */
/*                                                                  */
/*  Note that tolerances for the computation is input to            */
/*  the function.                                                   */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-15   Originally written                                 */
/*  1997-03-18   sur913, sur172                                     */
/*  1997-03-19   Number of patches for different angles             */
/*  1997-05-14   Dynamic allocation                                 */
/*  1997-05-22   Increase number of patches                         */
/*  1997-11-29   Topological patch data for NURB_SUR, sur911, ....  */
/*  1997-12-13   NURBS as a multiple topology patch surface, acase  */
/*  1998-01-06   offset= 0, sur757 added                            */
/*  1998-01-10   Initialization of local variables                  */
/*  1999-12-05   Free source code modifications & BBOX->BCONE (cone)*/
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_tobspline  Approximate to BSPL_SUR surface  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_poi_nurbs        * Get address to a NURBS node            */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_ini_gmpatnu      * Initiatlize GMPATNU                    */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_mboxcone     * Calculate BBOX and BCONE               */
/* varkon_sur_bicapprox    * Bicubic approximation                  */
/* varkon_pat_bicbez       * Cubic to Bezier patch                  */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error to error message stack           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short knotvec();       /* Knot vector values                 */
static short putnode();       /* Write NURBS node data              */
static short top_pat();       /* Create all topology patches        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBPatch *p_toppat;         /* Topology (NURBS) patch (ptr)   */
static DBfloat  comptol;          /* Computer tolerance (accuracy)  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_tobspline       */
/* SU2993 = Severe program error (  ) in varkon_sur_tobspline       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_tobspline (

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
                         /* Eq. 1: Number of patches defined by the */
                         /*        user (np_u and np_v)             */
                         /* Eq. 2: Necessary number of patches      */
                         /*        calculated by the program        */
                         /*        (not yet implemented)            */
                         /* Eq. 3: Graphical representation surface */
                         /*        Necessary number of patches are  */
                         /*        calculated by the program in a   */
                         /*        simplified way and bounding      */
                         /*        boxes and cones are only copied  */
                         /*        from the input surface           */
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

  DBSurf   *p_sur_nurb,  /* Surface of type NURB_SUR          (ptr) */
  DBPatch **pp_pat_nurb )/* Allocated area for patches        (ptr) */

/* Out:                                                             */
/*                                                                  */
/* Data to p_bsur                                                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint     nu_cub;      /* No. of patches in U direction CUB_SUR   */
  DBint     nv_cub;      /* No. of patches in V direction CUB_SUR   */
  DBPatch  *p_pata;      /* Approximation patches             (ptr) */
  DBPatch  *p_t;         /* Current topological patch         (ptr) */
  GMPATC   *p_patc;      /* Current bicubic     patch         (ptr) */
  GMPATBR3  patbr3;      /* Current Bezier      patch               */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  char     *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
  DBPatch  *p_b;         /* Current topological (NURBS) patch (ptr) */
  GMPATNU  *p_patnu;     /* NURBS patch                       (ptr) */
  DBint     nu;          /* No patches in U direction input surface */
  DBint     nv;          /* No patches in V direction input surface */
  BBOX      box;         /* Bounding box  for the surface           */
  BCONE     cone;        /* Bounding cone for the surface           */
  DBfloat   angmax;      /* Maximum BCONE angle for a patch         */
  DBint     nbadb;       /* Number of bad (BBOX)  patches           */
  DBint     nbadc;       /* Number of bad (BBONE) patches           */

  DBfloat  s_lim_a[2][2];/* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /* All equal zero => whole surface         */
  DBint    acase_a;      /* Approximation case:                     */
                         /* Eq.  1: Number of patches defined by    */
                         /*         the user (np_u and np_v)        */
                         /* Eq. 11: As 1 but no bound. boxes/cones  */
  DBint    np_u_a;       /* Number of approx patches per patch in U */
  DBint    np_v_a;       /* Number of approx patches per patch in V */
  DBint    tcase_a;      /* Tolerance band case                     */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
                         /* Tolerances for the approximation.       */
                         /* ( < 0 <==> not defined )                */
  DBfloat  ctol_a;       /* Coordinate          tolerance           */
  DBfloat  ntol_a;       /* Surface normal      tolerance           */
  DBfloat  rtol_a;       /* Radius of curvature tolerance           */
  DBSurf   surout_a;     /* Approximation surface                   */

                         /* For varkon_poi_nurbs (sur243)           */
  DBint     rwcase;      /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
  DBint     i_up;        /* Point address in U direction            */
  DBint     i_vp;        /* Point address in V direction            */
  DBHvector node;        /* Polygon point                           */

  DBint     iu,iv;       /* Patch address                           */
  DBint     surtype;     /* Type CUB_SUR, RAT_SUR, or ...           */
  short     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

  DBint   acc_box;       /* For sur911                              */

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
"sur174 Enter***varkon_sur_tobspline acase %d\n" , (short)acase );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 Input p_sur %d p_pat %d\n" , (int)p_sur, (int)p_pat );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialize pp_pat_nurb to NULL. Variable pp_pat_nurb is also a   */
/* flag to the calling function. Memory must be deallocated if      */
/* pp_pat_nurb not is equal to NULL (also for errors).              */
/*                                                                 !*/

  *pp_pat_nurb = NULL;

/*!                                                                 */
/* Initialize surface data in p_sur_nurb.                           */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur_nurb);


/* Initialize local variables                                       */
  p_gpat   = NULL;     /* Allocated area geom.  patch data  (ptr)   */
  p_b      = NULL;     /* Current topological (NURBS) patch (ptr)   */
  p_patnu  = NULL;     /* NURBS patch                       (ptr)   */
  p_toppat = NULL;     /* Topology NURBS patches            (ptr)   */



/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/*   Check approximation case and input data for the calculation    */
/*                                                                 !*/


    if ( acase == 3 )
      {
      ; /* OK. Input tolerances are not used for the moment */
      } /* End acase = 3 */

    else if ( acase == 1 )
      {
      if ( np_u < 1 )
        {
        sprintf(errbuf,"np_u %d %%sur174", (short)np_u );
        varkon_erinit();
        return(varkon_erpush("SU2383",errbuf));
        }
      if ( np_v <= 0 )
        {
        np_v = np_u;
        }
      }  /* End acase == 1  */

    else
      {
      sprintf(errbuf," %d %%sur174",(short)acase);
      varkon_erinit();
      return(varkon_erpush("SU2393",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && acase == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 Modified np_u %d np_v %d\n" , (short)np_u, (short)np_v );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   For Debug On: Check that the surface is of an acceptable type  */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_tobspline");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG
if      ( surtype     ==  CUB_SUR ||
          surtype     ==  RAT_SUR ||           
          surtype     ==  CON_SUR ||           
          surtype     ==  POL_SUR ||           
          surtype     ==   P3_SUR ||           
          surtype     ==   P5_SUR ||           
          surtype     ==   P7_SUR ||           
          surtype     ==   P9_SUR ||           
          surtype     ==  P21_SUR ||           
          surtype     ==  MIX_SUR ||           
          surtype     == NURB_SUR ||           
          surtype     ==  LFT_SUR )            
 ;
else if ( surtype     == FAC_SUR ) 

 {
 sprintf(errbuf, 
"(FAC_SUR)%%sur174");
 return(varkon_erpush("SU2993",errbuf));
 }
 else
 {
 sprintf(errbuf, 
 "(type)%%sur174");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif


/*!                                                                 */
/* 2. Approximate to a CUB_SUR surface                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Calculate maximum cone angle in the surface.                   */
/*   Call of varkon_sur_mboxcone (sur913).                          */
/*                                                                 !*/

    status = varkon_sur_mboxcone
    ( p_sur, p_pat, &box, &cone, &angmax, &nbadb, &nbadc );
    if(status<0)
    {
    sprintf(errbuf,"sur913%%varkon_sur_tobspline");
    return(varkon_erpush("SU2943",errbuf));
    }


/*!                                                                 */
/*   Goto noapprox for a CUB_SUR surface.                           */
/*                                                                 !*/

  if ( surtype  == CUB_SUR   )
   {
   nu_cub = nu;
   nv_cub = nv;
   p_pata = p_pat;
   goto noapprox;
   }


  s_lim_a[0][0] = s_lim_a[0][1] = s_lim_a[1][0] = s_lim_a[1][1] = 0.0;

/* No bounding boxes will be calculated for the approximate surface */
   acase_a = 11;

/*!                                                                 */
/*   Number of CUB_PAT patches is defined by max. cone angle        */
/*                                                                 !*/

  if      ( angmax <= 0.0 ) /* Undefined */
    {
    np_u_a   = 5;
    np_v_a   = 5;
    }
  else if ( angmax <  2.0 )
    {
    np_u_a   = 1;
    np_v_a   = 1;
    }
  else if ( angmax < 30.0 )
    {
    np_u_a   = 2;
    np_v_a   = 2;
    }
  else if ( angmax < 45.0 )
    {
    np_u_a   = 3;
    np_v_a   = 3;
    }
  else if ( angmax < 90.0 )
    {
    np_u_a   = 4;
    np_v_a   = 4;
    }
  else 
    {
    np_u_a   = 7;
    np_v_a   = 7;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 angmax %f np_u_a  %d  np_v_a %d\n",
  angmax, (int)np_u_a ,(int)np_v_a);
fflush(dbgfil(SURPAC));
}
#endif


  tcase_a  = I_UNDEF;
  ctol_a   = ctol;
  ntol_a   = ntol;
  rtol_a   = rtol;

    status = varkon_sur_bicapprox
   (p_sur,p_pat,s_lim_a,acase_a, np_u_a, np_v_a, tcase_a,
     ctol_a,ntol_a,rtol_a, &surout_a, &p_pata);  
    if(status<0)
    {
    sprintf(errbuf,"sur172%%varkon_sur_tobspline");
    return(varkon_erpush("SU2943",errbuf));
    }

  nu_cub = surout_a.nu_su;
  nv_cub = surout_a.nv_su;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 From sur172 nu_cub %d nv_cub %d\n",(int)nu_cub,(int)nv_cub);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

noapprox:; /* Input surface is a CUB_SUR surface                    */

/*!                                                                 */
/* 3. Surface header data                                           */
/* ______________________                                           */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Surface type NURB_SUR and number of patches (nu_cub,nv_cub)      */
/* The number of topological paches shall perhaps be defined        */
/* by the knot vector. In all functions are the calculations        */
/* based on patches, i.e. small areas of the surface. The search    */
/* for start points for is for instance based on these "patch"      */
/* boundaries.                                                      */
/* But on the other hand can "extra topological" pathes be added    */
/* for a computation. See sur910. This might be a better solution   */
/* The user may be confused if he gets the information that the     */
/* NURB_SUR has many patches. He (if he has knowledge about NURBS)  */
/* will wonder what a patch is for a NURBS surface.                 */
/* The first attempt will be to define the NURB_SUR surface as a    */
/* surface with one (1) topology path (DBPatch).                    */
/*                                                                 !*/

   p_sur_nurb->typ_su = NURB_SUR;



/*!                                                                 */
/* 4. Dynamic allocation of area for the NURBS patch                */
/* _________________________________________________                */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Note that the area for the topology patches are allocated and    */
/* defined in function top_pat. The number of patches is defined    */
/* by the knot vector for the NURBS surface.                        */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area for one geometric NURBS surface/patch */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(NURB_PAT, 1 )) == NULL )
 { 
 sprintf(errbuf, "(allocg)%%sur174");
 return(varkon_erpush("SU2993",errbuf));
 }

/* Current geometric patch */
   p_patnu = (GMPATNU *)p_gpat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 p_gpat %d p_patnu %d\n",
  (int)p_gpat ,(int)p_patnu);
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Initialize the NURBS patch                                       */
/* Call of varkon_ini_gmpatnu (sur757).                             */
/*                                                                 !*/
    varkon_ini_gmpatnu (p_patnu);    


/*!                                                                 */
/*  Number of knot values and degree to the geometric surface/patch */
/*                                                                 !*/


    p_patnu->nk_u    = 4*nu_cub+4;
    p_patnu->nk_v    = 4*nv_cub+4;
    p_patnu->order_u = 4; 
    p_patnu->order_v = 4;
    p_patnu->ofs_pat = 0.0;


/*!                                                                 */
/*  Allocate memory for polygon points and knot vectors.            */
/*  Call of DBcreate_NURBS.                                                 */
/*                                                                 !*/

    DBcreate_NURBS(p_patnu); 

/*!                                                                 */
/*  Initialize NURBS nodes.                                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

    rwcase = 2;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 init.%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/*  Printout of (initialized) polygon points for Debug On           */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

#ifdef  DEBUG
    rwcase = 3;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 Print%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

/*!                                                                 */
/* 4. Knot vector for the NURBS surface                             */
/* ____________________________________                             */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Call of internal function knotvec.                               */
/*                                                                 !*/

   status= knotvec( nu_cub, nv_cub, p_patnu );
   if  ( status < 0 ) 
     {
     if ( surtype  != CUB_SUR   ) DBfree_patches(&surout_a, p_pata);
     sprintf(errbuf, "knotvec%%sur172");
     return(varkon_erpush("SU2973",errbuf));
     }

/*!                                                                 */
/* 5. Polygon points for the NURBS surface                          */
/* _______________________________________                          */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop for all patches of the input surface                        */
/*                                                                 !*/

for ( iu=0; iu<nu_cub; ++iu )
  {
  for ( iv=0; iv<nv_cub; ++iv )
    {

/*!                                                                 */
/*  Pointer to current patch                                        */
/*                                                                 !*/

    p_t = p_pata  + iu*nv_cub + iv; 

/*!                                                                 */
/*  Convert the bicubic patch to a (rational) Bezier patch.         */
/*  Calls of varkon_sur_eval (sur210).                              */
/*                                                                 !*/

/*  Get pointer to the geometric patch                              */
#ifdef  DEBUG
    if      ( p_t->styp_pat != CUB_PAT ) 
      {
      if ( surtype  != CUB_SUR   ) DBfree_patches(&surout_a, p_pata);
      sprintf(errbuf, "(Not CUB_SUR)%%sur172");
      return(varkon_erpush("SU2993",errbuf));
      }
#endif

    p_patc= (GMPATC *)p_t->spek_c;

   status= varkon_pat_bicbez (p_patc, &patbr3 );
/* No errors from this function (status= SUCCED)                    */

/*!                                                                 */
/*  Polygon points to output surface                                */
/*  Call of internal function putnode.                              */
/*                                                                 !*/

   status= putnode( &patbr3, iu, iv, nu_cub, nv_cub, p_patnu );
   if  ( status < 0 ) 
     {
     if ( surtype  != CUB_SUR   ) DBfree_patches(&surout_a, p_pata);
     sprintf(errbuf, "putnode%%sur172");
     return(varkon_erpush("SU2973",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174 p_t %d p_patc %d \n", (int)p_t, (int)p_patc ); 
fflush(dbgfil(SURPAC)); 
  }
#endif


      }                                    /* End loop all patches  */
    }                                    /* End loop all patches    */

/*!                                                                 */
/*     End loop all records in PBOUND table i_pat=1,2,....,pbn      */

/*!                                                                 */
/*  Printout of polygon points for Debug On                         */
/*  Call of varkon_poi_nurbs (sur243).                              */
/*                                                                 !*/

#ifdef  DEBUG
    rwcase = 3;
    i_up   = I_UNDEF;
    i_vp   = I_UNDEF;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243 Print%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

/*!                                                                 */
/* 5. Create all the topology patches for the surface               */
/* __________________________________________________               */
/*                                                                  */
/*  Call of internal function putnode.                              */
/*                                                                 !*/

   status= top_pat( p_sur_nurb, p_patnu );
   if  ( status < 0 ) 
     {
     if ( surtype  != CUB_SUR   ) DBfree_patches(&surout_a, p_pata);
     sprintf(errbuf, "top_pat%%sur172");
     return(varkon_erpush("SU2973",errbuf));
     }

   *pp_pat_nurb = p_toppat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 Pointer to topology patches p_toppat %d *pp_pat_nurb %d\n", 
 (int)p_toppat, (int)(*pp_pat_nurb) ); 
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 6. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes are not calculated for acase= 3. The input        */
/* surface BBOX is copied to the output surface.                    */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/


  if  (  acase == 3 )
    {
    V3MOME((char *)(&p_sur->box_su),(char *)(&p_sur_nurb->box_su),
                    sizeof(BBOX));
    goto  no_boxes;
    } /* End acase= 3 */



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 Calculate bounding boxes and cones p_sur_nurb %d p_b %d\n"
         , (int)p_sur_nurb, (int)p_b );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174 p_toppat %d p_toppat->us_pat %f Surf. type %d\n"
         , (int)p_toppat, p_toppat->us_pat, 
  (int)p_sur_nurb->typ_su );
fflush(dbgfil(SURPAC));
}
#endif

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_sur_nurb ,p_toppat ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"sur911%%sur174 ");
        return(varkon_erpush("SU2943",errbuf));
        }

no_boxes:; /* Label: No boxes have been calculated                  */

/*!                                                                 */
/* 7. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Deallocation of memory for approximation, bicubic surface        */
/*                                                                 !*/

  if ( surtype  != CUB_SUR   ) DBfree_patches(&surout_a, p_pata);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174 Exit***varkon_sur_tobspline ** \n");
fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** knotvec ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Knot vector for NURBS surface                                 */

      static short knotvec ( nu_cub, nv_cub, p_patnu )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint     nu_cub;      /* No. of patches in U direction CUB_SUR   */
  DBint     nv_cub;      /* No. of patches in V direction CUB_SUR   */
  GMPATNU  *p_patnu;     /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat *p_uval;       /* U knot value                      (ptr) */
  DBfloat *p_vval;       /* V knot value                      (ptr) */
  DBint    i_u;          /* Loop index U knot values                */
  DBint    j_v;          /* Loop index V knot values                */
  DBint    k_u;          /* Total number of U knot values           */
  DBint    l_v;          /* Total number of V knot values           */
/*----------------------------------------------------------------- */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*knotvec Enter nu_cub %d nv_cub %d\n",(int)nu_cub,(int)nv_cub);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. U knot values                                                 */
/*                                                                 !*/

k_u = 0;
for ( i_u = 0; i_u <  nu_cub; ++i_u )
  {
  if ( i_u == 0  )        /* Two extra for start of surface */
    {
    p_uval = p_patnu->kvec_u+k_u;
    (*p_uval) = (DBfloat)i_u;             /* 1 extra Start */
    k_u = k_u + 1;
    p_uval = p_patnu->kvec_u+k_u;
    (*p_uval) = (DBfloat)i_u;             /* 2 extra Start */
    k_u = k_u + 1;
    }
/* Four value per patch. First two i_ next two i_u+1 */
  p_uval = p_patnu->kvec_u+k_u;
  (*p_uval) = (DBfloat)i_u;               /* 1 */
  k_u = k_u + 1;
  p_uval = p_patnu->kvec_u+k_u;
  (*p_uval) = (DBfloat)i_u;               /* 2 */
  k_u = k_u + 1;
  p_uval = p_patnu->kvec_u+k_u;
  (*p_uval) = (DBfloat)(i_u+1);           /* 3 */
  k_u = k_u + 1;
  p_uval = p_patnu->kvec_u+k_u;
  (*p_uval) = (DBfloat)(i_u+1);           /* 4 */
  k_u = k_u + 1;
  if ( i_u == nu_cub-1 )        /* Two extra for end   of surface */
    {
    p_uval = p_patnu->kvec_u+k_u;
    (*p_uval) = (DBfloat)(i_u+1);         /* 1 extra End   */
    k_u = k_u + 1;
    p_uval = p_patnu->kvec_u+k_u;
    (*p_uval) = (DBfloat)(i_u+1);         /* 2 extra End   */
    }
  }  /*  End loop i_u  */

/*!                                                                 */
/* Knot vector values from 1.0 to nu_cub                            */
/*                                                                 !*/

   if  ( nu_cub > 0 )
   {
   for ( i_u = 0; i_u <= k_u; ++i_u )
     {
      p_uval = p_patnu->kvec_u+i_u;
      (*p_uval) = (*p_uval) + 1.0;
     }
   }

/*!                                                                 */
/* 2. V knot values                                                 */
/*                                                                 !*/

l_v = 0;
for ( j_v = 0; j_v <  nv_cub; ++j_v )
  {
  if ( j_v == 0  )        /* Two extra for start of surface */
    {
    p_vval = p_patnu->kvec_v+l_v;
    (*p_vval) = (DBfloat)j_v;             /* 1 extra Start */
    l_v = l_v + 1;
    p_vval = p_patnu->kvec_v+l_v;
    (*p_vval) = (DBfloat)j_v;             /* 2 extra Start */
    l_v = l_v + 1;
    }
/* Four value per patch. First two i_ next two j_v+1 */
  p_vval = p_patnu->kvec_v+l_v;
  (*p_vval) = (DBfloat)j_v;               /* 1 */
  l_v = l_v + 1;
  p_vval = p_patnu->kvec_v+l_v;
  (*p_vval) = (DBfloat)j_v;               /* 2 */
  l_v = l_v + 1;
  p_vval = p_patnu->kvec_v+l_v;
  (*p_vval) = (DBfloat)(j_v+1);           /* 3 */
  l_v = l_v + 1;
  p_vval = p_patnu->kvec_v+l_v;
  (*p_vval) = (DBfloat)(j_v+1);           /* 4 */
  l_v = l_v + 1;
  if ( j_v == nv_cub-1 )        /* Two extra for end   of surface */
    {
    p_vval = p_patnu->kvec_v+l_v;
    (*p_vval) = (DBfloat)(j_v+1);         /* 1 extra End   */
    l_v = l_v + 1;
    p_vval = p_patnu->kvec_v+l_v;
    (*p_vval) = (DBfloat)(j_v+1);         /* 2 extra End   */
    }
  }  /*  End loop j_v  */

/*!                                                                 */
/* Knot vector values from 1.0 to nv_cub                            */
/*                                                                 !*/

   if  ( nv_cub > 0 )
   {
   for ( j_v = 0; j_v <= l_v; ++j_v )
     {
      p_vval = p_patnu->kvec_v+j_v;
      (*p_vval) = (*p_vval) + 1.0;
     }
   }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*knotvec Total number of U knot values k_u= %d\n",(int)k_u);
fflush(dbgfil(SURPAC));
}
for ( i_u = 0; i_u <= k_u; ++i_u )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*knotvec U[%3d]=  %f\n",(int)i_u,*(p_patnu->kvec_u+i_u));
fflush(dbgfil(SURPAC));
}
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*knotvec Total number of V knot values l_v= %d\n",(int)l_v);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
for ( j_v = 0; j_v <= l_v; ++j_v )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*knotvec V[%3d]=  %f\n",(int)j_v,*(p_patnu->kvec_v+j_v));
fflush(dbgfil(SURPAC)); 
}
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** top_pat ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Create all topology patches for the NURBS surface             */

    static short top_pat (p_sur_nurb, p_patnu)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf   *p_sur_nurb;  /* Surface of type NURB_SUR          (ptr) */
  GMPATNU  *p_patnu;     /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
  DBint     maxnum;      /* Maximum number of patches (alloc. area) */
  char     *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
  char     *p_tpat;      /* Allocated area topol. patch data  (ptr) */
  DBPatch  *p_b;         /* Current topological (NURBS) patch (ptr) */
  DBint    nu_nurb;      /* Number of NURBS U patches               */
  DBint    nv_nurb;      /* Number of NURBS V patches               */
  DBfloat  u_knot;       /* U knot value, current                   */
  DBfloat  v_knot;       /* V knot value, current                   */
  DBfloat  u_knot_pre;   /* U knot value, previous                  */
  DBfloat  v_knot_pre;   /* V knot value, previous                  */
  DBfloat *p_uval;       /* U knot value                      (ptr) */
  DBfloat *p_vval;       /* V knot value                      (ptr) */
  DBint    i_u;          /* Loop index U knot values                */
  DBint    j_v;          /* Loop index V knot values                */
  DBfloat  u_val[1000];  /* U values for patches                    */
  DBfloat  v_val[1000];  /* V values for patches                    */
/*                                                                  */
/*----------------------------------------------------------------- */

  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat Enter  Number of U knots %d\n",
  (int)p_patnu->nk_u);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/* Inititialize the output topology (NURBS) patch pointer           */
   p_toppat = NULL;
   p_b      = NULL;

/* Inititialize local variables                                     */

  u_knot_pre = F_UNDEF;
  v_knot_pre = F_UNDEF;
  maxnum     = I_UNDEF;   
  p_gpat     = NULL;
  p_tpat     = NULL; 
  p_b        = NULL;   
  nu_nurb    = I_UNDEF; 
  nv_nurb    = I_UNDEF;  
  u_knot     = F_UNDEF;  
  v_knot     = F_UNDEF;   
  p_uval     = NULL;   
  p_vval     = NULL;
  i_u        = I_UNDEF;
  j_v        = I_UNDEF;

/*!                                                                 */
/* 1. Determine the number of patches                               */
/*                                                                 !*/

   nu_nurb = 0;
   for ( i_u = 0; i_u < p_patnu->nk_u; ++i_u )
     {
      p_uval = p_patnu->kvec_u+i_u;
      u_knot = (*p_uval);
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat i_u= %d u_knot %f\n",(int)i_u,u_knot);
fflush(dbgfil(SURPAC));
}
#endif
     if  ( i_u == 0 ) 
        {
         u_knot_pre    = u_knot;
         u_val[nu_nurb] = u_knot;
         }
     else
       {
       if ( fabs(u_knot_pre-u_knot) > 0.0001 )
         {
         nu_nurb    = nu_nurb + 1;
         if ( nu_nurb > 999 )
           { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174*top_pat Increase u_val array size \n");
  }
#endif
 sprintf(errbuf, "(u_val)%%sur174*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
         }
         u_val[nu_nurb] = u_knot;
         u_knot_pre = u_knot;
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat i_u= %d nu_nurb %d u_knot %f\n",
  (int)i_u,(int)nu_nurb, u_knot);
fflush(dbgfil(SURPAC));
}
#endif
         }
       }

     } /* End loop i_u */


   nv_nurb = 0;
   for ( j_v = 0; j_v < p_patnu->nk_v; ++j_v )
     {
      p_vval = p_patnu->kvec_v+j_v;
      v_knot = (*p_vval);
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat j_v= %d v_knot %f\n",(int)j_v,v_knot);
fflush(dbgfil(SURPAC));
}
#endif
     if  ( j_v == 0 )
        {
         v_knot_pre    = v_knot;
         v_val[nv_nurb] = v_knot;
         }
     else
       {
       if ( fabs(v_knot_pre-v_knot) > 0.0001 )
         {
         nv_nurb    = nv_nurb + 1;
         if ( nv_nurb > 999 )
           { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174*top_pat Increase v_val array size \n");
  }
#endif
 sprintf(errbuf, "(v_val)%%sur174*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
         }
         v_val[nv_nurb] = v_knot;
         v_knot_pre = v_knot;
         }
       }
     } /* End loop j_v */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat Number of topology patches nu_nurb %d nv_nurb %d \n",
  (int)nu_nurb,(int)nv_nurb);
fflush(dbgfil(SURPAC));
}
#endif

 if  (  nu_nurb == 0 ||  nv_nurb == 0)
 { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174*top_pat nu_nurb %d or nv_nurb %d is zero\n", 
         (int)nu_nurb, (int)nv_nurb );
  }
#endif
 sprintf(errbuf,
 "(nu_nurb*nv_nurb=0)%%sur174*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
 } 

/*!                                                                 */
/* Number of patches to surface header                              */
/*                                                                  */
/*                                                                 !*/
   p_sur_nurb->nu_su  = nu_nurb;
   p_sur_nurb->nv_su  = nv_nurb;

/*!                                                                 */
/* 2. Dynamic allocation of area for patches                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the surface data,        */
/* i.e. for the surface topology patches.                           */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

   maxnum = nu_nurb*nv_nurb;        

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 { 
#ifdef DEBUG   
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur174*top_pat Allocation failed  maxnum %d\n", (short)maxnum );
  }
#endif
 sprintf(errbuf,
 "(alloc)%%sur174*top_pat"); 
 return(varkon_erpush("SU2993",errbuf));
 } 

/*!                                                                 */
/* Pointer to the first patch (static variable)                     */
/*                                                                 !*/

   p_toppat = (DBPatch*)p_tpat;


/*!                                                                 */
/* Dynamic allocation of area for one geometric NURBS surface/patch */
/* is already made. Vonvert PATNU pointer to char pointer.          */
/*                                                                 !*/

   p_gpat = (char *) p_patnu;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat *p_toppat %d p_gpat %d Allocation for %d patches\n", 
(int)p_toppat, (int)p_gpat, (int)maxnum);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Data to the topology patches                                  */
/*                                                                 !*/

   for ( j_v = 0; j_v < nv_nurb; ++j_v )
     {
     for ( i_u = 0; i_u < nu_nurb; ++i_u )
       {

/*     Ptr to  current patch                                        */
       p_b = p_toppat + i_u*nv_nurb + j_v;

/*     Initialize                                                   */
       varkon_ini_gmpat  (p_b); 

/*     All topology data                                            */

       if  ( i_u == 0 && j_v == 0 )
         {
         p_b->styp_pat = NURB_PAT;       /* Type of secondary patch */
         p_b->spek_c   = p_gpat;         /* Secondary patch (C ptr) */
         p_b->su_pat   = 0;              /* Topological adress      */
         p_b->sv_pat   = 0;              /* secondary patch not def.*/
         }
       else                              
         {
         p_b->styp_pat= TOP_PAT;        /* Type of secondary patch */
/*10/12  p_b->spek_c  =(char *)p_b;*/   /* Secondary patch (C ptr) */
         p_b->spek_c  =(char *)p_toppat;/* Secondary patch (C ptr) */
         p_b->su_pat  =(short)(1);      /* Topological adress      */
         p_b->sv_pat  =(short)(1);      /* secondary patch.        */
         }
       p_b->iu_pat   = (short)(i_u+1);   /* Topological adress for  */
       p_b->iv_pat   = (short)(j_v+1);   /* current (this) patch    */
       p_b->us_pat   = u_val[ i_u ] +    /* Start U on geom. patch  */
                       comptol;          /*                         */
       p_b->ue_pat   = u_val[i_u+1] -    /* End   U on geom. patch  */
                       comptol;          /*                         */
       p_b->vs_pat   = v_val[ j_v ] +    /* Start V on geom. patch  */
                       comptol;          /*                         */
       p_b->ve_pat   = v_val[j_v+1] -    /* End   V on geom. patch  */
                       comptol;          /*                         */

       p_b->box_pat.xmin = 1.0;          /* BBOX  initialization    */
       p_b->box_pat.ymin = 2.0;          /*                         */
       p_b->box_pat.zmin = 3.0;          /*                         */
       p_b->box_pat.xmax = 4.0;          /*                         */
       p_b->box_pat.ymax = 5.0;          /*                         */
       p_b->box_pat.zmax = 6.0;          /*                         */
       p_b->box_pat.flag = -1;           /* Not calculated          */
    
       p_b->cone_pat.xdir = 1.0;         /* BCONE initialization    */
       p_b->cone_pat.ydir = 2.0;         /*                         */
       p_b->cone_pat.zdir = 3.0;         /*                         */
       p_b->cone_pat.ang  = 4.0;         /*                         */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*top_pat i_u %d j_v %d us %8.4f ue %8.4f vs %8.4f ve %8.4f\n", 
(int)i_u, (int)j_v, 
p_b->us_pat, p_b->ue_pat, p_b->vs_pat, p_b->ve_pat  );
fflush(dbgfil(SURPAC));
}
#endif

       }    /* End loop i_u */
     }      /* End loop j_v */


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** putnode ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Knot vector for triple internal knots                         */

    static short putnode (p_patbr3,iu,iv,nu_cub,nv_cub,p_patnu)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATBR3 *p_patbr3;    /* Current Bezier      patch         (ptr) */
  DBint     iu,iv;       /* Patch address                           */
  DBint     nu_cub;      /* No. of patches in U direction CUB_SUR   */
  DBint     nv_cub;      /* No. of patches in V direction CUB_SUR   */
  GMPATNU  *p_patnu;     /* NURBS patch                       (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    i_u_s;        /* Address to start polygon point          */
  DBint    i_v_s;        /* Address to start polygon point          */

                         /* For varkon_poi_nurbs (sur243)           */
  DBint     rwcase;      /* Read/write case:                        */
                         /* Eq. 1: Write point                      */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
  DBint     i_up;        /* Point address in U direction            */
  DBint     i_vp;        /* Point address in V direction            */
  DBHvector node;        /* Polygon point                           */


/*----------------------------------------------------------------- */

  short     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur174*putnode Enter  p_patbr3 %d iu %d iv %d\n",
                (int)p_patbr3, (int)iu,(int)iv);
fprintf(dbgfil(SURPAC),
"sur174*putnode nu_cub %d nv_cub %d\n",(int)nu_cub,(int)nv_cub);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Address to start polygon point                                   */
/*                                                                 !*/

   rwcase = 1;

   i_u_s = iu*4;
   i_v_s = iv*4;

/*!                                                                 */
/* Polygon point r00                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r00x;
   node.y_gm = p_patbr3->r00y;
   node.z_gm = p_patbr3->r00z;
   node.w_gm = p_patbr3->r00;
   i_up      = i_u_s;
   i_vp      = i_v_s;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r00%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }



/*!                                                                 */
/* Polygon point r01                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r01x;
   node.y_gm = p_patbr3->r01y;
   node.z_gm = p_patbr3->r01z;
   node.w_gm = p_patbr3->r01;
   i_up      = i_u_s;
   i_vp      = i_v_s + 1;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r01%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r02                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r02x;
   node.y_gm = p_patbr3->r02y;
   node.z_gm = p_patbr3->r02z;
   node.w_gm = p_patbr3->r02;
   i_up      = i_u_s;
   i_vp      = i_v_s + 2;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r02%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r03                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r03x;
   node.y_gm = p_patbr3->r03y;
   node.z_gm = p_patbr3->r03z;
   node.w_gm = p_patbr3->r03;
   i_up      = i_u_s;
   i_vp      = i_v_s + 3;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r03%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* Polygon point r10                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r10x;
   node.y_gm = p_patbr3->r10y;
   node.z_gm = p_patbr3->r10z;
   node.w_gm = p_patbr3->r10;
   i_up      = i_u_s + 1;
   i_vp      = i_v_s;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r10%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r11                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r11x;
   node.y_gm = p_patbr3->r11y;
   node.z_gm = p_patbr3->r11z;
   node.w_gm = p_patbr3->r11;
   i_up      = i_u_s + 1;
   i_vp      = i_v_s + 1;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r11%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r12                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r12x;
   node.y_gm = p_patbr3->r12y;
   node.z_gm = p_patbr3->r12z;
   node.w_gm = p_patbr3->r12;
   i_up      = i_u_s + 1;
   i_vp      = i_v_s + 2;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r12%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r13                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r13x;
   node.y_gm = p_patbr3->r13y;
   node.z_gm = p_patbr3->r13z;
   node.w_gm = p_patbr3->r13;
   i_up      = i_u_s + 1;
   i_vp      = i_v_s + 3;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r13%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r20                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r20x;
   node.y_gm = p_patbr3->r20y;
   node.z_gm = p_patbr3->r20z;
   node.w_gm = p_patbr3->r20;
   i_up      = i_u_s + 2;
   i_vp      = i_v_s + 0;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r20%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r21                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r21x;
   node.y_gm = p_patbr3->r21y;
   node.z_gm = p_patbr3->r21z;
   node.w_gm = p_patbr3->r21;
   i_up      = i_u_s + 2;
   i_vp      = i_v_s + 1;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r21%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r22                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r22x;
   node.y_gm = p_patbr3->r22y;
   node.z_gm = p_patbr3->r22z;
   node.w_gm = p_patbr3->r22;
   i_up      = i_u_s + 2;
   i_vp      = i_v_s + 2;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r22%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r23                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r23x;
   node.y_gm = p_patbr3->r23y;
   node.z_gm = p_patbr3->r23z;
   node.w_gm = p_patbr3->r23;
   i_up      = i_u_s + 2;
   i_vp      = i_v_s + 3;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r23%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r30                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r30x;
   node.y_gm = p_patbr3->r30y;
   node.z_gm = p_patbr3->r30z;
   node.w_gm = p_patbr3->r30;
   i_up      = i_u_s + 3;
   i_vp      = i_v_s + 0;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r30%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* Polygon point r31                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r31x;
   node.y_gm = p_patbr3->r31y;
   node.z_gm = p_patbr3->r31z;
   node.w_gm = p_patbr3->r31;
   i_up      = i_u_s + 3;
   i_vp      = i_v_s + 1;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r31%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* Polygon point r32                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r32x;
   node.y_gm = p_patbr3->r32y;
   node.z_gm = p_patbr3->r32z;
   node.w_gm = p_patbr3->r32;
   i_up      = i_u_s + 3;
   i_vp      = i_v_s + 2;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r32%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* Polygon point r33                                                */
/*                                                                 !*/

   node.x_gm = p_patbr3->r33x;
   node.y_gm = p_patbr3->r33y;
   node.z_gm = p_patbr3->r33z;
   i_up      = i_u_s + 3;
   i_vp      = i_v_s + 3;
    status= varkon_poi_nurbs (p_patnu, rwcase, i_up, i_vp, &node);
   if  ( status < 0 ) 
     {
     sprintf(errbuf, "sur243*putnode r33%%sur172");
     return(varkon_erpush("SU2943",errbuf));
     }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

