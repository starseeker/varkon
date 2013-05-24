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
/*  Function: varkon_sur_compana                   File: sur412.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Analyses surface composite case.                                */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-12   Originally written                                 */
/*  1996-11-13   Compilation warnings ..                            */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_compana    Analyses surface composite case  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_compide    * Identical boundary curves ?              */
/* varkon_erpush         * Error message to terminal                */
/* varkon_erinit         * Initialize error message stack           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_compana (sur412) */
/* SU4023 = Surfaces cannot be combined                     (sur412) */
/* SU2993 = Severe program error (  ) in varkon_sur_compana (sur412) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_compana (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur_1,      /* Surface 1                         (ptr) */
  DBPatch *p_pat_1,      /* Surface 1 topological patch data  (ptr) */
  DBSurf  *p_sur_2,      /* Surface 2                         (ptr) */
  DBPatch *p_pat_2,      /* Surface 2 topological patch data  (ptr) */
  SURCOMP *p_scomp )     /* Composite surface comp. data      (ptr) */
/*                                                                  */
/* Out:        Analysis result in SURCOMP                           */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   uv_type1;     /* UV type curve 1= U  2= V surface 1      */
   DBfloat par_val1;     /* Iso-parameter value surface 1           */
   DBint   uv_type2;     /* UV type curve 1= U  2= V surface 2      */
   DBfloat par_val2;     /* Iso-parameter value surface 2           */
   DBint   nu1;          /* Number of patches in U in surface 1     */
   DBint   nv1;          /* Number of patches in V in surface 1     */
   DBint   surtype1;     /* Type of surface                         */
   DBint   nu2;          /* Number of patches in U in surface 2     */
   DBint   nv2;          /* Number of patches in V in surface 2     */
   DBint   surtype2;     /* Type of surface                         */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

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
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur412 Enter*varkon_sur_compana* p_sur_1 %d p_sur_2 %d \n",
        (int)p_sur_1, (int)p_sur_2);
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur412 p_pat_1 %d p_pat_2 %d\n",
        (int)p_pat_1, (int)p_pat_2);
fflush(dbgfil(SURPAC));
}
#endif
 
    nu1               = p_scomp->nu1;
    nv1               = p_scomp->nv1;
    surtype1          = p_scomp->surtype1;
    nu2               = p_scomp->nu2;
    nv2               = p_scomp->nv2;
    surtype2          = p_scomp->surtype2;

/*!                                                                 */
/* Initialize output variables comp_case and equal_case in SURCOMP  */
/*                                                                 !*/

    p_scomp->comp_case  = I_UNDEF;
    p_scomp->equal_case = I_UNDEF;

/*!                                                                 */
/* 2. Determine surface composite case                              */
/* ___________________________________                              */
/*                                                                 !*/

/*!                                                                 */
/* Compare points on surface 1 U start and surface 2 U end          */
/* Call of varkon_sur_compide (sur414).                             */
/*                                                                 !*/

     uv_type1 = 1;
     par_val1 = 1.0  + p_scomp->comptol;
     uv_type2 = 1;
     par_val2 = nu2  + 1.0 - p_scomp->comptol;

     status= varkon_sur_compide
       (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp,
        uv_type1,par_val1,uv_type2,par_val2);

      if(status<0)
      {
      sprintf(errbuf,"sur414%%sur412");
      return(varkon_erpush("SU2943",errbuf));
      }

   if       ( p_scomp->comp_case == BDY_IDENT )
     {
     p_scomp->equal_case = S1US_S2UE;
     goto ident;
     }
   else if  ( p_scomp->comp_case == BDY_NULLP )
     {
     p_scomp->equal_case = S1US_S2UE;
     goto nullp;
     }
   else if  ( p_scomp->comp_case == BDY_TRIMP )
     {
     p_scomp->equal_case = S1US_S2UE;
     goto trimp;
     }

/*!                                                                 */
/* Compare points on surface 1 U end   and surface 2 U start        */
/* Call of varkon_sur_compide (sur414).                             */
/*                                                                 !*/

     uv_type1 = 1;
     par_val1 = nu1  + 1.0 - p_scomp->comptol;
     uv_type2 = 1;
     par_val2 = 1.0  + p_scomp->comptol;

     status= varkon_sur_compide
       (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp,
        uv_type1,par_val1,uv_type2,par_val2);

      if(status<0)
      {
      sprintf(errbuf,"sur414%%sur412");
      return(varkon_erpush("SU2943",errbuf));
      }


   if  ( p_scomp->comp_case == BDY_IDENT )
     {
     p_scomp->equal_case = S1UE_S2US;
     goto ident;
     }
   else if  ( p_scomp->comp_case == BDY_NULLP )
     {
     p_scomp->equal_case = S1UE_S2US;
     goto nullp;
     }
   else if  ( p_scomp->comp_case == BDY_TRIMP )
     {
     p_scomp->equal_case = S1UE_S2US;
     goto trimp;
     }



/*!                                                                 */
/* Compare points on surface 1 V start and surface 2 V end          */
/* Call of varkon_sur_compide (sur414).                             */
/*                                                                 !*/

     uv_type1 = 2;
     par_val1 = 1.0  + p_scomp->comptol;
     uv_type2 = 2;
     par_val2 = nv2  + 1.0 - p_scomp->comptol;

     status= varkon_sur_compide
       (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp,
        uv_type1,par_val1,uv_type2,par_val2);

      if(status<0)
      {
      sprintf(errbuf,"sur414%%sur412");
      return(varkon_erpush("SU2943",errbuf));
      }


   if  ( p_scomp->comp_case == BDY_IDENT )
     {
     p_scomp->equal_case = S1VS_S2VE;
     goto ident;
     }
   else if  ( p_scomp->comp_case == BDY_NULLP )
     {
     p_scomp->equal_case = S1VS_S2VE;
     goto nullp;
     }
   else if  ( p_scomp->comp_case == BDY_TRIMP )
     {
     p_scomp->equal_case = S1VS_S2VE;
     goto trimp;
     }

/*!                                                                 */
/* Compare points on surface 1 V end   and surface 2 V start        */
/* Call of varkon_sur_compide (sur414).                             */
/*                                                                 !*/

     uv_type1 = 2;
     par_val1 = nv1  + 1.0 - p_scomp->comptol;
     uv_type2 = 2;
     par_val2 = 1.0  + p_scomp->comptol;

     status= varkon_sur_compide
       (p_sur_1,p_pat_1,p_sur_2,p_pat_2,p_scomp,
        uv_type1,par_val1,uv_type2,par_val2);

      if(status<0)
      {
      sprintf(errbuf,"sur414%%sur412");
      return(varkon_erpush("SU2943",errbuf));
      }


   if  ( p_scomp->comp_case == BDY_IDENT )
     {
     p_scomp->equal_case = S1VE_S2VS;
     goto ident;
     }
   else if  ( p_scomp->comp_case == BDY_NULLP )
     {
     p_scomp->equal_case = S1VE_S2VS;
     goto nullp;
     }
   else if  ( p_scomp->comp_case == BDY_TRIMP )
     {
     p_scomp->equal_case = S1VE_S2VS;
     goto trimp;
     }

/*!                                                                 */
/* Surfaces cannot be combined. Return with error                   */
/*                                                                 !*/

     varkon_erinit();
     sprintf(errbuf,"%d%%sur412", (int)p_scomp->equal_case );
     return(varkon_erpush("SU4023",errbuf));

nullp:;

     varkon_erinit();
     sprintf(errbuf,"%d%%sur412", (int)p_scomp->equal_case );
     return(varkon_erpush("SU4043",errbuf));

trimp:;

     varkon_erinit();
     sprintf(errbuf,"%d%%sur412", (int)p_scomp->equal_case );
     return(varkon_erpush("SU4053",errbuf));

ident:; /*! Label: Identical boundary exists                       !*/

/* n. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
if         (  p_scomp->equal_case  ==         S1US_S2UE )
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= S1US_S2UE (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1UE_S2US )
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= S1UE_S2US (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VS_S2VE )
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= S1VS_S2VE (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VE_S2VS )
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= S1VE_S2VS (= %d) \n", 
(int)p_scomp->equal_case );
else  if   (  p_scomp->equal_case  ==         S1VF_S2VF )
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= S1VF_S2VF (= %d) \n", 
(int)p_scomp->equal_case );
else 
fprintf(dbgfil(SURPAC),
"sur412 Exit*varkon_sur_compana* equal_case= Undefined (= %d) \n", 
(int)p_scomp->equal_case);
}
#endif



    return(SUCCED);

  }

/*********************************************************/
