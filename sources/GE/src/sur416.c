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
/*  Function: varkon_sur_compwho                   File: sur416.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Creates a composite surface with two whole, input surfaces,     */
/*  i.e. the surfaces have a common, identical boundary.            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-12   Originally written                                 */
/*  1996-11-13   Compilation warnings ....                          */
/*  1998-01-09   Initialization of local variables                  */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_compwho    Combine two whole surfaces       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_compwho (sur416) */
/* SU2993 = Severe program error (  ) in varkon_sur_compwho (sur416) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_compwho (

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
  DBPatch *p_patout )    /* Alloc. area for topol. patch data (ptr) */
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
   DBint    comp_case;   /* Composite boundary case                 */ 
   DBint    equal_case;  /* Equal     boundary case                 */
   DBint    nu;          /* Number of patches in U in output surf.  */
   DBint    nv;          /* Number of patches in V in output surf.  */
   DBint    iu;          /* Loop index current output U patch       */
   DBint    iv;          /* Loop index current output V patch       */
   DBint    iuc;         /* Loop index current input  U patch       */
   DBint    ivc;         /* Loop index current input  V patch       */
   DBint    nvc;         /* Number of V patches in current surface  */
   DBPatch *p_pat_c;     /* Patch data for current surface    (ptr) */
   DBPatch *p_t;         /* Current input  topological patch  (ptr) */
   DBPatch *p_t_out;     /* Current output topological patch  (ptr) */
   GMPATC  *p_patcin;    /* Input   geometric, bicubic  patch (ptr) */
   GMPATR  *p_patrin;    /* Input   geometric, rational patch (ptr) */
   GMPATL  *p_patlin;    /* Input   geometric, lofting  patch (ptr) */
   GMPATN  *p_patnin;    /* Input   geometric, Consurf  patch (ptr) */
   GMPATC  *p_patcout;   /* Output  geometric, bicubic  patch (ptr) */
   GMPATR  *p_patrout;   /* Output  geometric, rational patch (ptr) */
   GMPATL  *p_patlout;   /* Output  geometric, lofting  patch (ptr) */
   GMPATN  *p_patnout;   /* Output  geometric, Consurf  patch (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  comptol;     /* Computer accuracy                       */
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
"sur416 Enter*varkon_sur_compwho* p_sur_1 %d p_sur_2 %d p_surout %d\n",
        (int)p_sur_1, (int)p_sur_2, (int)p_surout);
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur416 p_pat_1 %d p_pat_2 %d p_patout %d\n",
        (int)p_pat_1, (int)p_pat_2, (int)p_patout );
fflush(dbgfil(SURPAC)); 
}
#endif
 
/* Initialization of local variables                                */

   p_pat_c     = NULL;
   p_t         = NULL;   
   p_t_out     = NULL; 
   p_patcin    = NULL;
   p_patrin    = NULL;
   p_patlin    = NULL;
   p_patnin    = NULL;
   p_patcout   = NULL;
   p_patrout   = NULL;
   p_patlout   = NULL;
   p_patnout   = NULL;

   nu          = I_UNDEF;
   nv          = I_UNDEF;

/*!                                                                 */
/* Retrieve number of patches, composite case, etc. from SURCOMP.   */
/*                                                                 !*/


    nu1               = p_scomp->nu1;
    nv1               = p_scomp->nv1;
    surtype1          = p_scomp->surtype1;
    nu2               = p_scomp->nu2;
    nv2               = p_scomp->nv2;
    surtype2          = p_scomp->surtype2;

    comptol           = p_scomp->comptol;

    comp_case         = p_scomp->comp_case;
    equal_case        = p_scomp->equal_case;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
       "sur416 No of patches nu1 %d nv1 %d surtype1 %d\n",
                        (int)nu1,(int)nv1,(int)surtype1);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
       "sur416 No of patches nu2 %d nv2 %d surtype2 %d\n",
                        (int)nu2,(int)nv2,(int)surtype2);
}
#endif

/*!                                                                 */
/* Check data from SURCOMP                                          */
/*                                                                 !*/

   if  ( comp_case == BDY_IDENT );
   else
     {
     sprintf(errbuf,"comp_case%%sur416");
     return(varkon_erpush("SU2993",errbuf));
     } /* End else comp_case  */

   if      ( equal_case  ==  S1US_S2UE ||  
             equal_case  ==  S1UE_S2US ||
             equal_case  ==  S1VS_S2VE ||  
             equal_case  ==  S1VE_S2VS     );
   else
     {
     sprintf(errbuf,"equal_case%%sur416");
     return(varkon_erpush("SU2993",errbuf));
     } /* End else equal_case */

   if      ( equal_case  ==  S1US_S2UE ||  
             equal_case  ==  S1UE_S2US     )
     {
     nu = nu1 + nu2;
     nv = nv1;
     }
   else if ( equal_case  ==  S1VS_S2VE ||  
             equal_case  ==  S1VE_S2VS     )
     {
     nu = nu1;
     nv = nv1 + nv2;
     }

/*!                                                                 */
/* Number of patches to surface header DBSurf                       */
/*                                                                 !*/

  p_surout->nu_su = (short)nu; 
  p_surout->nv_su = (short)nv;


/* Initialization loop variables                                    */

  iuc = I_UNDEF;
  nvc = I_UNDEF;
  ivc = -1;

/*!                                                                 */
/* 3. Create the composite surface                                  */
/* _______________________________                                  */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv< nv; ++iv )               /* Start loop V patches    */
  {
  iuc = -1;
  if      ( equal_case  ==  S1VE_S2VS  )
    {
    if  ( iv  <  nv1 )
      {
      ivc = iv;
      p_pat_c = p_pat_1;
      nvc     = nv1;
      }
    else
      {
      ivc = iv - nv1;
      p_pat_c = p_pat_2;
      nvc     = nv2;
      }
    }
  else if ( equal_case  ==  S1VS_S2VE  )
    {
    if  ( iv  <  nv2 )
      {
      ivc = iv;
      p_pat_c = p_pat_2;
      nvc     = nv2;
      }
    else
      {
      ivc = iv - nv2;
      p_pat_c = p_pat_1;
      nvc     = nv1;
      }
    }
    else ivc = iv;

  for ( iu=0; iu< nu; ++iu )             /* Start loop U patches    */
    {

    if      ( equal_case  ==  S1UE_S2US  )
      {
      if  ( iu  <  nu1 )
        {
        iuc = iu;
        p_pat_c = p_pat_1;
        nvc     = nv1;
        }
      else
        {
        iuc = iu - nu1;
        p_pat_c = p_pat_2;
        nvc     = nv2;
        }
      }
    else if ( equal_case  ==  S1US_S2UE  )
      {
      if  ( iu  <  nu2 )
        {
        iuc = iu;
        p_pat_c = p_pat_2;
        nvc     = nv2;
        }
      else
        {
        iuc = iu - nu2;
        p_pat_c = p_pat_1;
        nvc     = nv1;
        }
      }
    else  iuc = iu;


/*!                                                                 */
/*    Pointer to input current topological patch                    */
      p_t = p_pat_c + iuc*nvc + ivc; 
/*                                                                  */
/*    Pointer to output current topological patch                   */
      p_t_out = p_patout + iu*nv + iv; 
/*                                                                  */
/*    Copy topological patch data to p_t_out                        */
/*    New values to iu_pat, iv_pat, us_pat, ue_pat, vs_pat, ve_pat  */
/*                                                                 !*/

    V3MOME((char *)(p_t),(char *)(p_t_out),sizeof(DBPatch));

    p_t_out->iu_pat   = (short)(iu + 1); /* Topological adress for  */
    p_t_out->iv_pat   = (short)(iv + 1); /* current (this) patch    */
/*  !!!!! Should not assume that parameters goes from iu1+1 ....    */
    p_t_out->us_pat   = (DBfloat)iu+1.0;         /* Start U on patch*/
    p_t_out->ue_pat   = (DBfloat)iu+2.0-comptol; /* End   U on patch*/
    p_t_out->vs_pat   = (DBfloat)iv+1.0;         /* Start V on patch*/
    p_t_out->ve_pat   = (DBfloat)iv+2.0-comptol; /* End   V on patch*/



/*!                                                                 */
/*   Dynamic allocation of area for one geometric patch.            */
/*   Call of function DBcreate_patches.                             */
/*                                                                 !*/

    if      ( p_t->styp_pat == CUB_PAT )
      {
      p_patcin = (GMPATC *)p_t->spek_c;

      if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(bic allocg)%%sur416 varkon_sur_compwho");  
        return(varkon_erpush("SU2993",errbuf));
        }                                 

      p_t_out->spek_c   = p_gpat;        /* Secondary patch (C ptr) */

      p_patcout= (GMPATC *)p_gpat;       /* Current geometric patch */
      V3MOME((char *)(p_patcin),(char *)(p_patcout),sizeof(GMPATC));
      }



    else if ( p_t->styp_pat == RAT_PAT )
      {
      p_patrin = (GMPATR *)p_t->spek_c;
      if ( (p_gpat=DBcreate_patches(RAT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(rat allocg)%%sur416 varkon_sur_compwho");  
        return(varkon_erpush("SU2993",errbuf));
        }                                 
      p_t_out->spek_c   = p_gpat;        /* Secondary patch (C ptr) */
      p_patrout= (GMPATR *)p_gpat;       /* Current geometric patch */
      V3MOME((char *)(p_patrin),(char *)(p_patrout),sizeof(GMPATR));
      }

    else if ( p_t->styp_pat == LFT_PAT )
      {
      p_patlin = (GMPATL *)p_t->spek_c;
      if ( (p_gpat=DBcreate_patches(LFT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(lft allocg)%%sur416 varkon_sur_compwho");  
        return(varkon_erpush("SU2993",errbuf));
        }                                 
      p_t_out->spek_c   = p_gpat;        /* Secondary patch (C ptr) */
      p_patlout= (GMPATL *)p_gpat;       /* Current geometric patch */
      V3MOME((char *)(p_patlin),(char *)(p_patlout),sizeof(GMPATL));
      }

    else if ( p_t->styp_pat == CON_PAT )
      {
      p_patnin = (GMPATN *)p_t->spek_c;
      if ( (p_gpat=DBcreate_patches(CON_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(con allocg)%%sur416 varkon_sur_compwho");  
        return(varkon_erpush("SU2993",errbuf));
        }                                 
      p_t_out->spek_c   = p_gpat;        /* Secondary patch (C ptr) */
      p_patnout= (GMPATN *)p_gpat;       /* Current geometric patch */
      V3MOME((char *)(p_patnin),(char *)(p_patnout),sizeof(GMPATN));
      }

    else
      {
      sprintf(errbuf,"(patch type)%%sur416");
      return(varkon_erpush("SU2993",errbuf));
    }



#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && p_pat_c == p_pat_1 )
{
fprintf(dbgfil(SURPAC),
"sur416 iu %d iv %d iuc %d ivc %d nvc %d p_pat_c %d p_t %d Surface 1\n",
(int)iu,(int)iv,(int)iuc,(int)ivc,(int)nvc,(int)p_pat_c,(int)p_t );
}
if ( dbglev(SURPAC) == 2 && p_pat_c == p_pat_2 )
{
fprintf(dbgfil(SURPAC),
"sur416 iu %d iv %d iuc %d ivc %d nvc %d p_pat_c %d p_t %d Surface 2\n",
(int)iu,(int)iv,(int)iuc,(int)ivc,(int)nvc,(int)p_pat_c,(int)p_t );
}
#endif


    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/


/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur416 Exit*varkon_sur_compwho*Patch data copied to nu= %d * nv= %d patches\n",
            (int)nu, (int)nv );
}
#endif


    return(SUCCED);

  }

/*********************************************************/
