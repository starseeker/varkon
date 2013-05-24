/*!******************************************************************
*
*    exsurusr.c 
*    ==========
*
*    EXsusr();            SUR_USRDEF
*    EXsusrh();           Retrieve surface header data
*    EXsusrt();           Retrieve topolog. patch data
*    EXsusrg();           Retrieve CUB_PAT  patch data
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */

/* varkon_ini_gmsur           * Initialize DBSurf                    */
/* varkon_ini_gmpat           * Initialize DBPatch                    */
/* varkon_sur_bound           * Bounding box & cone for surface     */
/* varkon_ini_gmpatc          * Initialize DBPatch_C                   */
/* varkon_ini_evals           * Initialize EVALS                    */
/* varkon_pat_biccre1         * Geometry to CUB_PAT                 */
/* varkon_comptol             * Retrieve computer tolerance         */
/* varkon_pat_pribic          * Print bicubic patch data For Debug  */
#ifdef  DEBUG
extern short igidst();          /* Convert identity to string       */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!******************************************************************/
/*                                                                  */
/*  Function: EXsusr                                                */
/*  =================                                               */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Function for the MBS statement SUR_USRDEF.                      */
/*                                                                  */
/*  The function SUR_USRDEF creates a surface. The input data       */
/*  is given as attributes to parts.                                */
/*                                                                  */
/*  There is no corresponding surpac function to this function,     */
/*  since there are no geometry calculations, except for simple     */
/*  conversions of coefficients from geometric format (points,      */
/*  tangents, twist vectors for instance) to the VARKON CUB_PAT     */
/*  algebraic format.                                               */
/*                                                                  */
/*  The function allocates memory for the output surface,           */
/*  retrieves the input part attribute data from the Geometry       */
/*  Memory (GM).                                                    */
/*                                                                  */
/*  The output surface is stored in the Geometry Memory (GM).       */
/*                                                                  */
/*  Memory for the surface is deallocated.                          */
/*                                                                  */
/*  Note that input parts with attributes can be defined in a       */
/*  sub-tree (a part) which can be the whole active module, i.e.    */
/*  a part with identity #0 (=> the whole of GM will be scanned).   */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-01   Originally written                                 */
/*  1999-12-18   surxxx->varkon_mmm_zzzzz                           */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsusr                Function corresp. to SUR_USRDEF  */
/*                                                              */
/*------------------------------------------------------------- */


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called   function xxxxxx failed in EXsusr               */
/* SU2993 = Severe program error (xxxxxxxx) in EXsusr               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */

   short EXsusr(
  DBId    *sur_id,       /* Reference (# id.) for output surface    */
  DBId    *geo_id,       /* Identity for sub-tree with input data   */
                         /* ( geo_id = #0 <==> whole GM )           */
  char    *hea_name,     /* Name of part with surface header data   */
  char    *top_name,     /* Name of part with topologic patch data  */
  char    *cub_name,     /* Name of part with geometric patch data  */
  V2NAPA  *pnp)          /* Attribute data for surface sur_id       */


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    n_u;          /* Number of patches in U direction        */
  DBint    n_v;          /* Number of patches in V direction        */
  DBint    sur_type;     /* Surface type                            */

  char    *p_tpat;       /* Allocated area topol. patch data  (ptr) */
  DBPatch   *p_pat;        /* Allocated area topol. patch data  (ptr) */

  DBSurf    sur;          /* Surface                                 */

  DBint    i_p;          /* Loop index patch                        */

  DBint    acc;          /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */

  DBint    status;       /* Error code from a called function       */
  char     errbuf[133];  /* Error message string                    */
#ifdef  DEBUG
  DBPatchC  *p_patc;     /* Bicubic patch                    (ptr)  */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Enter Function corresponding to MBS statement SUR_USRDEF\n");
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Input strings hea_name %s top_name %s cub_name %s\n", 
              hea_name, top_name, cub_name );
fflush(dbgfil(EXEPAC)); 
}
#endif


/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status = 0;

/*!                                                                 */
/* Initialize surface data in sur.                                  */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur ( &sur ); 


/*!                                                                 */
/* 2. Retrieve surface header data.                                 */
/* ________________________________                                 */
/*                                                                  */
/* Retrieve number of patches and surface type from part geo_id.    */
/* Call of function EXsusrh.                                        */
/*                                                                 !*/

   status= EXsusrh( geo_id, hea_name, &n_u, &n_v, &sur_type );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"EXsusrh%%EXsusr");
     erpush("SU2943",errbuf);
#endif
     goto  err3;
     }

/*!                                                                 */
/* Check header data.                                               */
/*                                                                 !*/

   if    ( n_u < 1 )
     {
     sprintf(errbuf,"%d%%EXsusrh",(int)n_u);
     erinit();
     return(erpush("SU6523",errbuf));
     }
   if    ( n_v < 1 )
     {
     sprintf(errbuf,"%d%%EXsusrh",(int)n_v);
     erinit();
     return(erpush("SU6533",errbuf));
     }
   if    ( sur_type != CUB_SUR )
     {
     sprintf(errbuf,"%d%%EXsusrh",(int)sur_type);
     erinit();
     return(erpush("SU6543",errbuf));
     }

/*!                                                                 */
/* Header data to DBSurf.                                            */
/*                                                                 !*/

   sur.typ_su = (short)sur_type; 
   sur.nu_su  = (short)n_u;
   sur.nv_su  = (short)n_v;

/*!                                                                 */
/* 3. Allocate memory for the output surface                        */
/* _________________________________________                        */
/*                                                                  */
/* Allocate memory for the output topologic patches.                */
/* Call of function DBcreate_patches.                                         */
/*                                                                 !*/

   if ((p_tpat=DBcreate_patches(TOP_PAT,n_u*n_v))==NULL)
     {
     sprintf(errbuf, "(alloc)%%EXsusr");
#ifdef LNAME
     return(varkon_erpush("SU2993",errbuf));
#endif
#ifdef SNAME
     return(erpush("SU2993",errbuf));
#endif
     }

   p_pat  = (DBPatch  *)p_tpat;


/*!                                                                 */
/* Initialize patch data in p_pat.                                  */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_p=1; i_p <= n_u*n_v; i_p= i_p+1)
     {
     varkon_ini_gmpat (p_pat +i_p - 1); 
     }


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Area has been allocated for n_u*n_v %d patches\n", (int)(n_u*n_v));
fflush(dbgfil(EXEPAC)); 
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Pointer to the first DBPatch patch p_pat %d\n", (int)p_pat);
fflush(dbgfil(EXEPAC)); 
}
#endif

/*!                                                                 */
/* 4. Retrieve topologic patch data                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                 !*/

   status= EXsusrt( p_pat, geo_id, top_name, n_u, n_v, sur_type );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"EXsusrt%%EXsusr");
     erpush("SU2943",errbuf);
#endif
     goto  err2;
     }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Topological patch data for n_u*n_v %d patches has been retrieved\n", 
         (int)(n_u*n_v));
fflush(dbgfil(EXEPAC)); 
}
#endif


/*!                                                                 */
/* 5. Retrieve geometric patch data                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                 !*/

   status= EXsusrg( p_pat, geo_id, cub_name, n_u, n_v, sur_type );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"EXsusrg%%EXsusr");
     erpush("SU2943",errbuf);
#endif
     goto  err2;
     }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Geometric patch data has been retrieved\n" );
fflush(dbgfil(EXEPAC)); 
}
#endif

/*!                                                                 */
/*   For Debug On: Printout of second bicubic patch for CUB_SUR     */
/*                                                                 !*/

#ifdef  DEBUG
  p_patc = (DBPatchC *)(p_pat+1)->spek_c;
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Second bicubic patch p_patc %d\n", (int)p_patc );
fflush(dbgfil(EXEPAC)); 
}
  varkon_pat_pribic (p_patc);
#endif

/*!                                                                 */
/* 6. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (&sur, p_pat, acc);
   if (status<0) 
     {
     sprintf(errbuf,"varkon_sur_bound(sur911)%%EXsusr");
     erpush("SU2943",errbuf);
     goto err2;
     }

/*!                                                                 */
/* 7. Output surface                                                */
/* _________________                                                */
/*                                                                  */
/* Draw surface and store in the Geometry Memory (GM).              */
/* Call of EXesur.                                                  */
/*                                                                 !*/


   status = EXesur(sur_id,&sur,p_pat,NULL,NULL,pnp);
   if (status<0) 
     {
     sprintf(errbuf,"EXesur%%EXsusr");
     erpush("SU2943",errbuf);
     goto err2;
     }


/*!                                                                 */
/* 8. Exit                                                          */
/* ________                                                         */
/*                                                                  */
/*                                                                 !*/


err2:

/*!                                                                 */
/* Deallocate memory for the output surface.                        */
/* Call of function DBfree_patches.                                         */
/*                                                                 !*/

    DBfree_patches(&sur,p_pat);

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusr Area for patches has been deallocated \n");
fflush(dbgfil(EXEPAC)); 
}
#endif

err3:

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), "EXsusr Exit\n");
fflush(dbgfil(EXEPAC));
}
#endif

    return(status);

  }
  
/********************************************************************/
/*!******************************************************************/
/*                                                                  */
/*  Function: EXsusrh                                               */
/*  ==================                                              */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function retrieves DBSurf header data for the surface        */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-01   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsusrh               Get surface header DBSurf data    */
/*                                                              */
/*------------------------------------------------------------- */


/* -------------- Externally defined variables ---------------------*/
/*----------------------------------------------------------------- */

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called   function xxxxxx failed in EXsusrh              */
/* SU2993 = Severe program error (xxxxxxxx) in EXsusrh              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
   short EXsusrh(
  DBId    *geo_id,       /* Identity for sub-tree with input data   */
                         /* ( geo_id = #0 <==> whole GM )           */
  char     hea_name[20], /* Name of part with surface header data   */
  DBint   *p_n_u,        /* Number of patches in U direction  (ptr) */
  DBint   *p_n_v,        /* Number of patches in V direction  (ptr) */
  DBint   *p_sur_type)   /* Surface type                      (ptr) */


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    trmode;       /* Traverse mode 0 = Full  1 = This level  */
  DBetype  typmsk;       /* Type(s) of entities                     */
  DBptr    prt_la;       /* GM pointer to current geometry entity   */
  DBptr    hea_la;       /* GM pointer to header part               */
  DBint    n_hea;        /* Number of input surfaces                */

/*-----------------------------------------------------------------!*/

  DBId     h_id[MXINIV]; /* hea_name part # identity                */
  PMLITVA  intval;       /* Integer                   for EXgint    */
  short    pnum;         /* Parameter number                        */
  short    status;       /* Error code from a called function       */
  char     errbuf[133];  /* Error message string                    */
#ifdef  DEBUG
  char     s_geo_id[80]; /* geo_id as string                        */
  char     s_h_id[80];   /* h_id   as string                        */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrh Enter Retrieve DBSurf surface header data \n");
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status      = 0;

   *p_n_u      = I_UNDEF;
   *p_n_v      = I_UNDEF;
   *p_sur_type = I_UNDEF;

    hea_la = DBNULL;

/*!                                                                 */
/* 2. Retrieve identity for part with surface header data           */
/* ______________________________________________________           */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Search for geometry only in input part geo_id.                   */
/* Call of DBset_root_id (SET_ROOT_GM as MBS statement).                   */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
   IGidst(geo_id,s_geo_id);
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrh geo_id as string s_geo_id= %s \n",s_geo_id);
fflush(dbgfil(EXEPAC));
}
#endif


   status= DBset_root_id( geo_id );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBset_root_id surf%%EXsusrh");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }


   n_hea  =  0;
   trmode =  0;
   typmsk = PRTTYP;

nextprt:;  /*! Label: Next part                                    !*/

/* char p_name[10], g_name[10];   Part and group  name or NULL     */

   status= DBget_next_ptr( trmode, typmsk, hea_name, NULL  , &prt_la );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBget_next_ptr%%EXsusrh");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }

#ifdef DEBUG
   if ( dbglev(EXEPAC) == 1 )
   {
   fprintf(dbgfil(EXEPAC), "EXsusrh prt_la= %d\n", (int)prt_la );
   fflush(dbgfil(EXEPAC));
   }
#endif

/* This is the way that the comparison shall be made !       */
/* Comparison between gmpek (long) and NULL is OK in Varkon  */
/* From 1.17 use DBptr and DBNULL, JK */
   if ( prt_la !=  DBNULL )
     {
     hea_la = prt_la;
     n_hea  = n_hea + 1;
#ifdef DEBUG
     if ( dbglev(EXEPAC) == 1 )
     {
     fprintf(dbgfil(EXEPAC), "EXsusrh n_hea %d prt_la %d hea_la %d\n",
                               (int)n_hea, (int)prt_la, (int)hea_la );
     fflush(dbgfil(EXEPAC));
     }
#endif
     goto  nextprt;
     }


   if        ( n_hea == 0 )
     {
     sprintf(errbuf,"%s%%EXsusrh",hea_name);
     erinit();
     return(erpush("SU6503",errbuf));
     }
   else if   ( n_hea != 1 )
     {
     sprintf(errbuf,"%s%%EXsusrh",hea_name);
     erinit();
     return(erpush("SU6513",errbuf));
     }

/*!                                                                 */
/* 3. Retrieve attribute data                                       */
/* __________________________                                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Convert GM pointer DBptr to DBId    (MBS) # identity h_id      */
/*   Call of DBget_id.                                                */
/*                                                                 !*/

     status = DBget_id(hea_la, h_id);
     if (status < 0 )
       {
#ifdef DEBUG
       sprintf(errbuf,"DBget_id%%EXsusrh");
       erpush("SU2943",errbuf);
#endif
       goto  err1;
       }


#ifdef DEBUG
   IGidst(h_id,s_h_id);
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrh h_id as string s_h_id= %s \n",s_h_id);
fflush(dbgfil(EXEPAC));
}
#endif


/*!                                                                 */
/*   Get attributes for h_id. Calls of EXgint.                      */
/*                                                                 !*/

    pnum = 1;
    status = EXgint(h_id,pnum,&intval);
    if ( status < 0 ) goto  err1;
    *p_n_u        = intval.lit.int_va;

    pnum = 2;
    status = EXgint(h_id,pnum,&intval);
    if ( status < 0 ) goto  err1;
    *p_n_v        = intval.lit.int_va;

    pnum = 3;
    status = EXgint(h_id,pnum,&intval);
    if ( status < 0 ) goto  err1;
    *p_sur_type   = intval.lit.int_va;


err1:


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrh Exit *p_n_u %d *p_n_v %d *p_sur_type %d \n",
          (int)*p_n_u, (int)*p_n_v, (int)*p_sur_type);
fflush(dbgfil(EXEPAC));
}
#endif

    return(status);

  }
  
/********************************************************************/



/*!******************************************************************/
/*                                                                  */
/*  Function: EXsusrt                                               */
/*  =================                                               */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function retrieves topological patch data for the surface   */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-01   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsusrt               Get topological patch data       */
/*                                                              */
/*------------------------------------------------------------- */


/* -------------- Externally defined variables ---------------------*/
/*----------------------------------------------------------------- */

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called   function xxxxxx failed in EXsusrt              */
/* SU2993 = Severe program error (xxxxxxxx) in EXsusrt              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
  short EXsusrt(
  DBPatch   *p_pat,        /* Topological patches               (ptr) */
  DBId    *geo_id,       /* Identity for sub-tree with input data   */
                         /* ( geo_id = #0 <==> whole GM )           */
  char     top_name[20], /* Name of parts with topolog. patch data  */
  DBint    n_u,          /* Number of patches in U direction        */
  DBint    n_v,          /* Number of patches in V direction        */
  DBint    sur_type)     /* Surface type                            */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    trmode;       /* Traverse mode 0 = Full  1 = This level  */
  DBetype  typmsk;       /* Type(s) of entities                     */
  DBptr    prt_la;       /* GM pointer to current geometry entity   */
  DBint    n_top;        /* Number of top_name patches              */
  DBint    t_case;       /* Case topological patch data             */
                         /* Eq. 1: Patch address iu, iv defined     */
                         /* Eq. 2: Patch type   also defined        */
                         /* Eq. 3: Patch limits also defined        */
  DBint    iu;           /* Patch address in U direction            */
  DBint    iv;           /* Patch address in V direction            */
  DBint    patch_type;   /* Patch type                              */
  DBfloat    us_pat;       /* Start U on the geometrical patch        */
  DBfloat    ue_pat;       /* End   U on the geometrical patch        */
  DBfloat    vs_pat;       /* Start V on the geometrical patch        */
  DBfloat    ve_pat;       /* End   V on the geometrical patch        */
  DBPatch   *p_t;          /* Current topological patch         (ptr) */

/*-----------------------------------------------------------------!*/

  DBfloat    comptol;      /* Computer tolerance (accuracy)           */
  DBId     t_id[MXINIV]; /* top_name part # identity                */
  PMLITVA  intval;       /* Integer                   for EXgint    */
  PMLITVA  fltval;       /* Float                     for EXgflt    */
  short    pnum;         /* Parameter number                        */
  short    status;       /* Error code from a called function       */
  char     errbuf[133];  /* Error message string                    */
#ifdef  DEBUG
  char     s_geo_id[80]; /* geo_id as string                        */
  char     s_t_id[80];   /* t_id   as string                        */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrt Enter Retrieve DBSurf topological patch data \n");
fflush(dbgfil(EXEPAC));
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrt n_u %d n_v %d sur_type %d \n",
          (int)n_u, (int)n_v, (int)sur_type);
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status      = 0;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* 2. Retrieve topological patch data.                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Search for geometry only in input part geo_id.                   */
/* Call of DBset_root_id (SET_ROOT_GM as MBS statement).                   */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
   IGidst(geo_id,s_geo_id);
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrt geo_id as string s_geo_id= %s \n",s_geo_id);
fflush(dbgfil(EXEPAC));
}
#endif


   status= DBset_root_id( geo_id );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBset_root_id surf%%EXsusrt");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }


   n_top  =  0;
   trmode =  0;
   typmsk = PRTTYP;

nextprt:;  /*! Label: Next part                                    !*/

/* char p_name[10], g_name[10];   Part and group  name or NULL     */

   status= DBget_next_ptr( trmode, typmsk, top_name, NULL  , &prt_la );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBget_next_ptr%%EXsusrt");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }

#ifdef DEBUG
   if ( dbglev(EXEPAC) == 2 )
   {
   fprintf(dbgfil(EXEPAC), "EXsusrt prt_la= %d\n", (int)prt_la );
   fflush(dbgfil(EXEPAC));
   }
#endif

/* This is the way that the comparison shall be made !       */

   if ( prt_la ==  DBNULL ) goto allprt;


/* An additional top_name part                               */

   n_top  = n_top + 1;


#ifdef DEBUG
     if ( dbglev(EXEPAC) == 2 )
     {
     fprintf(dbgfil(EXEPAC), "EXsusrt n_top %d prt_la %d \n",
                               (int)n_top, (int)prt_la );
     fflush(dbgfil(EXEPAC));
     }
#endif


/*!                                                                 */
/*   Convert GM pointer DBptr to DBId    (MBS) # identity t_id      */
/*   Call of DBget_id.                                                */
/*                                                                 !*/

     status = DBget_id(prt_la, t_id);
     if (status < 0 )
       {
#ifdef DEBUG
       sprintf(errbuf,"DBget_id%%EXsusrt");
       erpush("SU2943",errbuf);
#endif
       goto  err1;
       }


#ifdef DEBUG
   IGidst(t_id,s_t_id);
if ( dbglev(EXEPAC) == 2 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrt t_id as string s_t_id= %s \n",s_t_id);
fflush(dbgfil(EXEPAC));
}
#endif


/*!                                                                 */
/*   Get attributes for t_id. Calls of EXgint.                      */
/*                                                                 !*/

     pnum = 1;
     status = EXgint(t_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     t_case     = intval.lit.int_va;

    if   ( t_case == 1 || t_case == 2 || t_case == 3 )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%EXsusrt", (int)t_case );
      erinit();
      return(erpush("SU6563",errbuf));
      }

     pnum = 2;
     status = EXgint(t_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     iu         = intval.lit.int_va;

    if   ( iu >= 1  && iu <= n_u  )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%%d", (int)iu, (int)n_u );
      erinit();
      return(erpush("SU6573",errbuf));
      }

     pnum = 3;
     status = EXgint(t_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     iv         = intval.lit.int_va;

    if   ( iv >= 1  && iv <= n_v  )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%%d", (int)iv, (int)n_v );
      erinit();
      return(erpush("SU6583",errbuf));
      }

    if  (  t_case >= 2  )
      {
      pnum = 4;
      status = EXgint(t_id,pnum,&intval);
      if ( status < 0 ) goto  err1;
      patch_type = intval.lit.int_va;
      if ( patch_type != CUB_PAT )
        {
        sprintf(errbuf,"%d%%EXsusrt", (int)patch_type );
        erinit();
        return(erpush("SU6593",errbuf));
        }
      }    /* End t_case >= 2 */
    else 
      {
      if ( sur_type == CUB_SUR ) patch_type = CUB_PAT;
      else
        {
        sprintf(errbuf,"%d%%EXsusrt", (int)sur_type );
        erinit();
        return(erpush("SU6543",errbuf));
        }
      }     /* End t_case else  */

    if  (  t_case >= 3  )
      {
      pnum = 5;
      status = EXgflt(t_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      us_pat  = fltval.lit.float_va;
      pnum = 6;
      status = EXgflt(t_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      ue_pat  = fltval.lit.float_va;
      pnum = 7;
      status = EXgflt(t_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      vs_pat  = fltval.lit.float_va;
      pnum = 8;
      status = EXgflt(t_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      ve_pat  = fltval.lit.float_va;
      }    /* End t_case >= 3 */
    else 
      {
      us_pat   = (DBfloat)iu;
      ue_pat   = (DBfloat)iu+1.0-comptol;
      vs_pat   = (DBfloat)iv;
      ve_pat   = (DBfloat)iv+1.0-comptol;
      }     /* End t_case else  */

/*!                                                                 */
/*  Topological patch data to output surface (p_pat area)           */
/*                                                                 !*/

/*  Ptr to  current patch                                           */
    p_t = p_pat + (iu-1)*n_v + (iv-1);

/*  Type of secondary patch                                         */
    p_t->styp_pat = patch_type;

/*  Topological address secondary patch is not defined              */
    p_t->su_pat   = 0;
    p_t->sv_pat   = 0;

/*  Topological address to patch                                    */
    p_t->iu_pat   = (short)iu;
    p_t->iv_pat   = (short)iv;

/*  Patch parameter limits                                          */
    p_t->us_pat   = us_pat;
    p_t->ue_pat   = ue_pat;
    p_t->vs_pat   = vs_pat;
    p_t->ve_pat   = ve_pat;


/*!                                                                 */
/*  Continue scanning. Goto nextprt.                                */
/*                                                                 !*/
    goto  nextprt;



allprt:; /* Label: All parts retrieved                              */

   if        ( n_top != n_u*n_v )
     {
     sprintf(errbuf,"%s%%EXsusrt",top_name);
     erinit();
     return(erpush("SU6553",errbuf));
     }


err1:


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrt Exit Number of retrieved patches n_top %d\n", (int)n_top);
fflush(dbgfil(EXEPAC));
}
#endif

    return(status);

  }
  
/********************************************************************/


/*!******************************************************************/
/*                                                                  */
/*  Function: EXsusrg                                               */
/*  ==============================================================  */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function retrieves CUB_PAT patch data for the surface       */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-01   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsusrg               Get CUB_PAT patch data           */
/*                                                              */
/*------------------------------------------------------------- */


/* -------------- Externally defined variables ---------------------*/
/*----------------------------------------------------------------- */


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called   function xxxxxx failed in EXsusrg              */
/* SU2993 = Severe program error (xxxxxxxx) in EXsusrg              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
  short EXsusrg(
  DBPatch   *p_pat,        /* Topological patches               (ptr) */
  DBId    *geo_id,       /* Identity for sub-tree with input data   */
                         /* ( geo_id = #0 <==> whole GM )           */
  char     cub_name[20], /* Name of parts with CUB_PAT  patch data  */
  DBint    n_u,          /* Number of patches in U direction        */
  DBint    n_v,          /* Number of patches in V direction        */
  DBint    sur_type)     /* Surface type                            */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBint    trmode;       /* Traverse mode 0 = Full  1 = This level  */
  DBetype  typmsk;       /* Type(s) of entities                     */
  DBptr    prt_la;       /* GM pointer to current geometry entity   */
  DBint    n_cub;        /* Number of cub_name patches              */
  DBint    g_case;       /* Case geometrical patch data             */
                         /* Eq. 1: Coefficient format               */
                         /* Eq. 2: Geometric   format               */
                         /* Eq. 3: Bezier      format               */
  DBint    iu;           /* Patch address in U direction            */
  DBint    iv;           /* Patch address in V direction            */
  DBint    patch_type;   /* Patch type                              */
  DBPatchC  *p_g;        /* Current geometric   patch         (ptr) */
  char    *p_gpat;       /* Allocated area geom.  patch data  (ptr) */

  DBPatch   *p_t;          /* Current topological patch         (ptr) */

  EVALS    r00;          /* Corner point U=0 , V=0                  */
  EVALS    r10;          /* Corner point U=1 , V=0                  */
  EVALS    r01;          /* Corner point U=0 , V=1                  */
  EVALS    r11;          /* Corner point U=1 , V=1                  */

/*-----------------------------------------------------------------!*/

  DBfloat    comptol;      /* Computer tolerance (accuracy)           */
  DBId     g_id[MXINIV]; /* cub_name part # identity                */
  PMLITVA  intval;       /* Integer                   for EXgint    */
  PMLITVA  fltval;       /* Float                     for EXgflt    */
  PMLITVA  vecval;       /* Vector                    for EXgvec    */
  short    pnum;         /* Parameter number                        */
  short    status;       /* Error code from a called function       */
  char     errbuf[133];  /* Error message string                    */
#ifdef  DEBUG
  char     s_geo_id[80]; /* geo_id as string                        */
  char     s_g_id[80];   /* g_id   as string                        */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrg Enter Retrieve DBSurf topological patch data \n");
fflush(dbgfil(EXEPAC));
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrg n_u %d n_v %d sur_type %d \n",
          (int)n_u, (int)n_v, (int)sur_type);
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status      = 0;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/* Initialize EVALS variables                                       */
   varkon_ini_evals(&r00);
   varkon_ini_evals(&r10);
   varkon_ini_evals(&r01);
   varkon_ini_evals(&r11);

/*!                                                                 */
/* 2. Retrieve topological patch data.                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Search for geometry only in input part geo_id.                   */
/* Call of DBset_root_id (SET_ROOT_GM as MBS statement).                   */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
   IGidst(geo_id,s_geo_id);
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrg geo_id as string s_geo_id= %s \n",s_geo_id);
fflush(dbgfil(EXEPAC));
}
#endif


   status= DBset_root_id( geo_id );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBset_root_id surf%%EXsusrg");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }


   n_cub  =  0;
   trmode =  0;
   typmsk = PRTTYP;

nextprt:;  /*! Label: Next part                                    !*/

/* char p_name[10], g_name[10];   Part and group  name or NULL     */

   status= DBget_next_ptr( trmode, typmsk, cub_name, NULL  , &prt_la );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"DBget_next_ptr%%EXsusrg");
     erpush("SU2943",errbuf);
#endif
     goto  err1;
     }

#ifdef DEBUG
   if ( dbglev(EXEPAC) == 2 )
   {
   fprintf(dbgfil(EXEPAC), "EXsusrg prt_la= %d\n", (int)prt_la );
   fflush(dbgfil(EXEPAC));
   }
#endif

/* This is the way that the comparison shall be made !       */

   if ( prt_la ==  DBNULL ) goto allprt;


/* An additional cub_name part                               */

   n_cub  = n_cub + 1;


#ifdef DEBUG
     if ( dbglev(EXEPAC) == 2 )
     {
     fprintf(dbgfil(EXEPAC), "EXsusrg n_cub %d prt_la %d \n",
                               (int)n_cub, (int)prt_la );
     fflush(dbgfil(EXEPAC));
     }
#endif


/*!                                                                 */
/*   Convert GM pointer DBptr to DBId    (MBS) # identity g_id      */
/*   Call of DBget_id.                                                */
/*                                                                 !*/

     status = DBget_id(prt_la, g_id);
     if (status < 0 )
       {
#ifdef DEBUG
       sprintf(errbuf,"DBget_id%%EXsusrg");
       erpush("SU2943",errbuf);
#endif
       goto  err1;
       }


#ifdef DEBUG
   IGidst(g_id,s_g_id);
if ( dbglev(EXEPAC) == 2 )
{
fprintf(dbgfil(EXEPAC),
"EXsusrg g_id as string s_g_id= %s \n",s_g_id);
fflush(dbgfil(EXEPAC));
}
#endif


/*!                                                                 */
/*   Get attributes for g_id. Calls of EXgint.                      */
/*                                                                 !*/

     pnum = 1;
     status = EXgint(g_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     g_case     = intval.lit.int_va;

    if   ( g_case == 1 || g_case == 2 || g_case == 3 )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%EXsusrg", (int)g_case );
      erinit();
      return(erpush("SU6563",errbuf));
      }

     pnum = 2;
     status = EXgint(g_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     iu         = intval.lit.int_va;

    if   ( iu >= 1  && iu <= n_u  )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%%d", (int)iu, (int)n_u );
      erinit();
      return(erpush("SU6573",errbuf));
      }

     pnum = 3;
     status = EXgint(g_id,pnum,&intval);
     if ( status < 0 ) goto  err1;
     iv         = intval.lit.int_va;

    if   ( iv >= 1  && iv <= n_v  )
      {
      ; /* OK */
      }
    else
      {
      sprintf(errbuf,"%d%%%d", (int)iv, (int)n_v );
      erinit();
      return(erpush("SU6583",errbuf));
      }

    pnum = 4;
    status = EXgint(g_id,pnum,&intval);
    if ( status < 0 ) goto  err1;
    patch_type = intval.lit.int_va;
    if ( patch_type != CUB_PAT )
      {
      sprintf(errbuf,"%d%%EXsusrg", (int)patch_type );
      erinit();
      return(erpush("SU6593",errbuf));
      }


/*!                                                                 */
/*  Dynamic allocation of area for one geometric (bicubic) patch.   */
/*  Call of function DBcreate_patches.                                        */
/*                                                                 !*/

    if ((p_gpat=DBcreate_patches(CUB_PAT,1)) ==NULL)
     {
     sprintf(errbuf, "(allocg)%%EXsusrg");
#ifdef LNAME
     return(varkon_erpush("SU2993",errbuf));
#endif
#ifdef SNAME
     return(erpush("SU2993",errbuf));
#endif
 }

    p_g    = (DBPatchC *)p_gpat;


/*!                                                                 */
/*  Initialize patch data in p_g                                    */
/*  Call of varkon_ini_gmpatc (sur767).                             */
/*                                                                 !*/
    varkon_ini_gmpatc (p_g);

/*!                                                                 */
/*  Secondary patch (C ptr) to the topological patch data           */
/*                                                                 !*/

/*  Ptr to  current patch                                           */
    p_t = p_pat + (iu-1)*n_v + (iv-1);

/*  Secondary patch (C ptr)                                         */
    p_t->spek_c   = p_gpat;


#ifdef DEBUG
if ( dbglev(EXEPAC) == 2 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrg (iu-1)*n_v+(iv-1) %d p_t %d\n", (int)((iu-1)*n_v+(iv-1)), (int)p_t);
fflush(dbgfil(EXEPAC));
}
#endif



    if  (  g_case == 1  )
      {
      pnum =  5;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a00x = vecval.lit.vec_va.x_val;
      p_g->a00y = vecval.lit.vec_va.y_val;
      p_g->a00z = vecval.lit.vec_va.z_val;
      pnum =  6;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a01x = vecval.lit.vec_va.x_val;
      p_g->a01y = vecval.lit.vec_va.y_val;
      p_g->a01z = vecval.lit.vec_va.z_val;
      pnum =  7;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a02x = vecval.lit.vec_va.x_val;
      p_g->a02y = vecval.lit.vec_va.y_val;
      p_g->a02z = vecval.lit.vec_va.z_val;
      pnum =  8;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a03x = vecval.lit.vec_va.x_val;
      p_g->a03y = vecval.lit.vec_va.y_val;
      p_g->a03z = vecval.lit.vec_va.z_val;
      pnum =  9;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a10x = vecval.lit.vec_va.x_val;
      p_g->a10y = vecval.lit.vec_va.y_val;
      p_g->a10z = vecval.lit.vec_va.z_val;
      pnum = 10;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a11x = vecval.lit.vec_va.x_val;
      p_g->a11y = vecval.lit.vec_va.y_val;
      p_g->a11z = vecval.lit.vec_va.z_val;
      pnum = 11;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a12x = vecval.lit.vec_va.x_val;
      p_g->a12y = vecval.lit.vec_va.y_val;
      p_g->a12z = vecval.lit.vec_va.z_val;
      pnum = 12;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a13x = vecval.lit.vec_va.x_val;
      p_g->a13y = vecval.lit.vec_va.y_val;
      p_g->a13z = vecval.lit.vec_va.z_val;
      pnum = 13;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a20x = vecval.lit.vec_va.x_val;
      p_g->a20y = vecval.lit.vec_va.y_val;
      p_g->a20z = vecval.lit.vec_va.z_val;
      pnum = 14;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a21x = vecval.lit.vec_va.x_val;
      p_g->a21y = vecval.lit.vec_va.y_val;
      p_g->a21z = vecval.lit.vec_va.z_val;
      pnum = 15;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a22x = vecval.lit.vec_va.x_val;
      p_g->a22y = vecval.lit.vec_va.y_val;
      p_g->a22z = vecval.lit.vec_va.z_val;
      pnum = 16;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a23x = vecval.lit.vec_va.x_val;
      p_g->a23y = vecval.lit.vec_va.y_val;
      p_g->a23z = vecval.lit.vec_va.z_val;
      pnum = 17;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a30x = vecval.lit.vec_va.x_val;
      p_g->a30y = vecval.lit.vec_va.y_val;
      p_g->a30z = vecval.lit.vec_va.z_val;
      pnum = 18;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a31x = vecval.lit.vec_va.x_val;
      p_g->a31y = vecval.lit.vec_va.y_val;
      p_g->a31z = vecval.lit.vec_va.z_val;
      pnum = 19;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a32x = vecval.lit.vec_va.x_val;
      p_g->a32y = vecval.lit.vec_va.y_val;
      p_g->a32z = vecval.lit.vec_va.z_val;
      pnum = 20;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      p_g->a33x = vecval.lit.vec_va.x_val;
      p_g->a33y = vecval.lit.vec_va.y_val;
      p_g->a33z = vecval.lit.vec_va.z_val;
      pnum = 21;
      status = EXgflt(g_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      p_g->ofs_pat = fltval.lit.float_va;
      }    /* End g_case >= 1 */
    else if  (  g_case == 2  )
      {
      pnum = 4+1;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r00.r_x = vecval.lit.vec_va.x_val;
      r00.r_y = vecval.lit.vec_va.y_val;
      r00.r_z = vecval.lit.vec_va.z_val;

      pnum = 4+2;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r10.r_x = vecval.lit.vec_va.x_val;
      r10.r_y = vecval.lit.vec_va.y_val;
      r10.r_z = vecval.lit.vec_va.z_val;

      pnum = 4+3;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r01.r_x = vecval.lit.vec_va.x_val;
      r01.r_y = vecval.lit.vec_va.y_val;
      r01.r_z = vecval.lit.vec_va.z_val;

      pnum = 4+4;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r11.r_x = vecval.lit.vec_va.x_val;
      r11.r_y = vecval.lit.vec_va.y_val;
      r11.r_z = vecval.lit.vec_va.z_val;

      pnum = 4+5;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r00.u_x = vecval.lit.vec_va.x_val;
      r00.u_y = vecval.lit.vec_va.y_val;
      r00.u_z = vecval.lit.vec_va.z_val;

      pnum = 4+6;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r10.u_x = vecval.lit.vec_va.x_val;
      r10.u_y = vecval.lit.vec_va.y_val;
      r10.u_z = vecval.lit.vec_va.z_val;

      pnum = 4+7;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r01.u_x = vecval.lit.vec_va.x_val;
      r01.u_y = vecval.lit.vec_va.y_val;
      r01.u_z = vecval.lit.vec_va.z_val;

      pnum = 4+8;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r11.u_x = vecval.lit.vec_va.x_val;
      r11.u_y = vecval.lit.vec_va.y_val;
      r11.u_z = vecval.lit.vec_va.z_val;

      pnum = 4+9;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r00.v_x = vecval.lit.vec_va.x_val;
      r00.v_y = vecval.lit.vec_va.y_val;
      r00.v_z = vecval.lit.vec_va.z_val;

      pnum = 4+10;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r10.v_x = vecval.lit.vec_va.x_val;
      r10.v_y = vecval.lit.vec_va.y_val;
      r10.v_z = vecval.lit.vec_va.z_val;

      pnum = 4+11;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r01.v_x = vecval.lit.vec_va.x_val;
      r01.v_y = vecval.lit.vec_va.y_val;
      r01.v_z = vecval.lit.vec_va.z_val;

      pnum = 4+12;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r11.v_x = vecval.lit.vec_va.x_val;
      r11.v_y = vecval.lit.vec_va.y_val;
      r11.v_z = vecval.lit.vec_va.z_val;

      pnum = 4+13;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r00.uv_x = vecval.lit.vec_va.x_val;
      r00.uv_y = vecval.lit.vec_va.y_val;
      r00.uv_z = vecval.lit.vec_va.z_val;

      pnum = 4+14;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r10.uv_x = vecval.lit.vec_va.x_val;
      r10.uv_y = vecval.lit.vec_va.y_val;
      r10.uv_z = vecval.lit.vec_va.z_val;

      pnum = 4+15;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r01.uv_x = vecval.lit.vec_va.x_val;
      r01.uv_y = vecval.lit.vec_va.y_val;
      r01.uv_z = vecval.lit.vec_va.z_val;

      pnum = 4+16;
      status = EXgvec(g_id,pnum,&vecval);
      if ( status < 0 ) goto  err1;
      r11.uv_x = vecval.lit.vec_va.x_val;
      r11.uv_y = vecval.lit.vec_va.y_val;
      r11.uv_z = vecval.lit.vec_va.z_val;


      status = varkon_pat_biccre1 ( &r00, &r10, &r01, &r11, p_g );
      if ( status < 0 )
          {
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
           "EXsusrg varkon_pat_biccre1 sur254 failed \n");
      fflush(dbgfil(EXEPAC));
      }
#endif
          goto  err1;
          }



      pnum = 21;
      status = EXgflt(g_id,pnum,&fltval);
      if ( status < 0 ) goto  err1;
      p_g->ofs_pat = fltval.lit.float_va;
      }    /* End g_case >= 2 */

/*!                                                                 */
/*  Continue scanning. Goto nextprt.                                */
/*                                                                 !*/
    goto  nextprt;



allprt:; /* Label: All parts retrieved                              */

   if        ( n_cub == 0 )
     {
     sprintf(errbuf,"%s%%EXsusrg",cub_name);
     erinit();
     return(erpush("SU6603",errbuf));
     }


err1:


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), 
"EXsusrg Exit Number of geometry parts n_cub %d \n", (int)n_cub);
fflush(dbgfil(EXEPAC));
}
#endif

    return(status);

  }
  
/********************************************************************/
