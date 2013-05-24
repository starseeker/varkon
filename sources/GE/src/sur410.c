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
/*  Function: varkon_sur_comptwo                   File: sur410.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Creates a composite surface for an input of two surfaces.       */
/*                                                                  */
/*  Memory will be allocated for the output composite surface.      */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-13   Originally written                                 */
/*  1996-11-13   Compilation warnings, POL_SUR, ...                 */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_comptwo    Combine two surfaces             */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur      * Initialize DBSurf                        */
/* varkon_ini_gmpat      * Initialize DBPatch                       */
/* varkon_sur_nopatch    * Retrieve number of patches               */
/* varkon_sur_compana    * Determine composite case                 */
/* varkon_sur_compwho    * Combine two whole surfaces               */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_comptwo (sur410) */
/* SU2993 = Severe program error (  ) in varkon_sur_comptwo (sur410) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_comptwo (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur_1,      /* Surface 1                         (ptr) */
  DBPatch *p_pat_1,      /* Surface 1 topological patch data  (ptr) */
  DBSurf  *p_sur_2,      /* Surface 2                         (ptr) */
  DBPatch *p_pat_2,      /* Surface 2 topological patch data  (ptr) */
  SURCOMP *p_scomp,      /* Composite surface comp. data      (ptr) */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
                         /* (memory is allocated in this fctn)      */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    nu1;         /* Number of patches in U in surface 1     */
   DBint    nv1;         /* Number of patches in V in surface 1     */
   DBint    surtype1;    /* Type of surface                         */
   DBint    nu2;         /* Number of patches in U in surface 2     */
   DBint    nv2;         /* Number of patches in V in surface 2     */
   DBint    surtype2;    /* Type of surface                         */

   DBint    maxnum;      /* Maximum number of patches (alloc. area) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char    *p_tpat;      /* Allocated area topol. patch data  (ptr) */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   DBint    i_s;         /* Loop index surface patch record         */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


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
"sur410 Enter*varkon_sur_comptwo* p_sur_1 %d p_sur_2 %d p_surout %d\n",
        (int)p_sur_1, (int)p_sur_2, (int)p_surout);
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur410 p_pat_1 %d p_pat_2 %d\n",
        (int)p_pat_1, (int)p_pat_2);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur410 p_sur_1->typ_su %d p_sur_2->typ_su %d\n",
        (int)p_sur_1->typ_su, (int)p_sur_2->typ_su);
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL;                     /* Initialize output pointer */

 maxnum            = I_UNDEF;

/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);

/*!                                                                 */
/* Retrieve number of patches and surface types. Data to SURCOMP.   */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surface is of type                  */
/*               CUB_SUR, RAT_SUR, LFT_SUR, CONSUR or MIX_SUR       */
/*                                                                 !*/


    status = varkon_sur_nopatch (p_sur_1, &nu1, &nv1, &surtype1);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_comptwo (sur410)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

    status = varkon_sur_nopatch (p_sur_2, &nu2, &nv2, &surtype2);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_comptwo (sur410)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype1    == CUB_SUR ||           /* Check surface type      */
     surtype1    == RAT_SUR ||           
     surtype1    == LFT_SUR ||           
     surtype1    == CON_SUR ||           
     surtype1    ==  P3_SUR ||           
     surtype1    ==  P5_SUR ||           
     surtype1    ==  P7_SUR ||           
     surtype1    ==  P9_SUR ||           
     surtype1    == P21_SUR ||           
     surtype1    == POL_SUR ||           
     surtype1    == MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf, "type1 %d%%sur410", (int)surtype1);
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }
if ( surtype2    == CUB_SUR ||           /* Check surface type      */
     surtype2    == RAT_SUR ||           
     surtype2    == LFT_SUR ||           
     surtype2    == CON_SUR ||           
     surtype2    ==  P3_SUR ||           
     surtype2    ==  P5_SUR ||           
     surtype2    ==  P7_SUR ||           
     surtype2    ==  P9_SUR ||           
     surtype2    == P21_SUR ||           
     surtype2    == POL_SUR ||           
     surtype2    == MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type2)%%varkon_sur_comptwo (sur410)");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif


    p_scomp->nu1      = nu1;
    p_scomp->nv1      = nv1;
    p_scomp->surtype1 = surtype1;
    p_scomp->nu2      = nu2;
    p_scomp->nv2      = nv2;
    p_scomp->surtype2 = surtype2;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur410 No of patches nu1 %d nv1 %d surtype1 %d\n",
                        (int)nu1,(int)nv1,(int)surtype1);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur410 No of patches nu2 %d nv2 %d surtype2 %d\n",
                        (int)nu2,(int)nv2,(int)surtype2);
}
#endif

/*!                                                                 */
/* 2. Determine surface composite case                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/

    status = varkon_sur_compana
    (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp);
    if(status<0)
      {
      sprintf(errbuf,"varkon_sur_compana%%varkon_sur_comptwo (sur410)");
      return(varkon_erpush("SU2943",errbuf));
      }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
if         (  p_scomp->equal_case  ==         S1US_S2UE )
fprintf(dbgfil(SURPAC),
"sur410 equal_case= S1US_S2UE (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1UE_S2US )
fprintf(dbgfil(SURPAC),
"sur410 equal_case= S1UE_S2US (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VS_S2VE )
fprintf(dbgfil(SURPAC),
"sur410 equal_case= S1VS_S2VE (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VE_S2VS )
fprintf(dbgfil(SURPAC),
"sur410 equal_case= S1VE_S2VS (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VF_S2VF )
fprintf(dbgfil(SURPAC),
"sur410 equal_case= S1VF_S2VF (= %d) \n", 
(int)p_scomp->equal_case );
else 
fprintf(dbgfil(SURPAC),
"sur410 equal_case= Undefined (= %d) \n", 
(int)p_scomp->equal_case);
}
#endif


/*!                                                                 */
/* 3. Allocate topological patch area                               */
/* __________________________________                               */
/*                                                                  */
/*                                                                 !*/

   if  ( p_scomp->comp_case == BDY_IDENT )
     {
     if      ( p_scomp->equal_case  ==  S1US_S2UE ||  
               p_scomp->equal_case  ==  S1UE_S2US     )
       maxnum = nv1*(nu1+nu2); 
     else if ( p_scomp->equal_case  ==  S1VS_S2VE ||  
               p_scomp->equal_case  ==  S1VE_S2VS     )
       maxnum = nu1*(nv1+nv2); 
     else
       {
       sprintf(errbuf,"equal_case%%sur410");
       return(varkon_erpush("SU2993",errbuf));
       } /* End else equal_case */
     }   /* End comp_case == BDY_IDENT */
   else
     {
     sprintf(errbuf,"comp_case%%sur410");
     return(varkon_erpush("SU2993",errbuf));
     } /* End else comp_case  */


/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {                                      
#ifdef DEBUG                           
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur410 Allocation failed  maxnum %d\n", (int)maxnum );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
 sprintf(errbuf, "(alloc)%%varkon_sur_comptwo");   
 return(varkon_erpush("SU2993",errbuf));
 }                                 

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur410 Ptr to first patch p_frst  = %d No of patches %d\n",
  (int)p_frst  ,(int)maxnum);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Initialize patch data.                                           */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= maxnum; i_s= i_s+1)
     {
    varkon_ini_gmpat (p_frst  +i_s-1);
    }

/*!                                                                 */
/* Output surface type                                              */
/*                                                                 !*/
   if  (  surtype1 == surtype2 )
     p_surout->typ_su= (short)surtype1;
   else
     {
     sprintf(errbuf,"surtype%%sur410");
     return(varkon_erpush("SU2993",errbuf));
     } /* End else surtype    */

/*!                                                                 */
/* 4. Combine surface patches                                       */
/* ___________________________                                      */
/*                                                                  */
/*                                                                 !*/

    status = varkon_sur_compwho
    (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp,p_surout,p_frst);
    if(status<0)
      {
      sprintf(errbuf,"varkon_sur_compwho%%varkon_sur_comptwo (sur410)");
      return(varkon_erpush("SU2943",errbuf));
      }

/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur410 Exit*varkon_sur_comptwo* nu %d nv %d Type of surface %d\n",
 p_surout->nu_su ,p_surout->nv_su , p_surout->typ_su );
}
#endif


    return(SUCCED);

  }

/*********************************************************/
