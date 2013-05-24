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
/*  Function: varkon_sur_reverse                   File: sur184.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Reverse of U, V or both parameter directions in the surface     */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-20   Originally written                                 */
/*  1998-01-03   NURB_SUR, POL_SUR, P3, P5, P7, P9, P21 error       */
/*  1998-08-22   fflush not within if statement                     */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_reverse    Reverse surface parameter direct.*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short r_cubic();       /* Local UV parameters for reparam.   */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBPatch *p_t_in;  /* Current input  topological patch  (ptr) */
static DBPatch *p_t_out; /* Current output topological patch  (ptr) */
static char   *p_gpat;   /* Allocated area geom.  patch data  (ptr) */
static GMPATC *p_patcin; /* Input   geometric, bicubic  patch (ptr) */
static GMPATC *p_patcout;/* Output  geometric, bicubic  patch (ptr) */
static DBfloat r_s_u;    /* Reparameterisation start local U value  */
static DBfloat r_s_v;    /* Reparameterisation start local V value  */
static DBfloat r_e_u;    /* Reparameterisation end   local U value  */
static DBfloat r_e_v;    /* Reparameterisation end   local V value  */
static DBint   iu;       /* Loop index U line of input  surface     */
static DBint   iv;       /* Loop index V line of input  surface     */
static DBfloat comptol;  /* Computer tolerance (accuracy)           */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_bicrep       * Bicubic patch reparametris.            */
/* varkon_pat_loftrev      * Lofting patch param. reverse           */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_reverse         */
/* SU2993 = Severe program error (  ) in varkon_sur_reverse         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_reverse (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_patin,      /* Alloc. area for topol. patch data (ptr) */
  DBint    rcase,        /* Reverse  case:                          */
                         /* Eq.  1: Reverse U                       */
                         /* Eq.  2: Reverse V                       */
                         /* Eq. 12: Reverse U and V                 */
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
  DBint    iu_out;       /* U line of output surface                */
  DBint    iv_out;       /* V line of output surface                */
  DBint    nu;           /* No patches in U direction input surface */
  DBint    nv;           /* No patches in V direction input surface */
  DBint    maxnum;       /* Maximum number of patches (alloc. area) */
  DBPatch *p_frst;       /* Pointer to the first patch              */
  GMPATR  *p_patrin;     /* Input   geometric, rational patch (ptr) */
  GMPATL  *p_patlin;     /* Input   geometric, lofting  patch (ptr) */
  GMPATN  *p_patnin;     /* Input   geometric, Consurf  patch (ptr) */
  GMPATN  *p_patnout;    /* Output  geometric, Consurf  patch (ptr) */
  GMPATR  *p_patrout;    /* Output  geometric, rational patch (ptr) */
  GMPATL  *p_patlout;    /* Output  geometric, lofting  patch (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint    acc_box;      /* For sur911                              */
  char    *p_tpat;       /* Allocated area topol. patch data  (ptr) */
  DBint    surtype;      /* Type CUB_SUR, RAT_SUR, or ...           */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

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
"sur184 Enter***varkon_sur_reverse  rcase %d\n" , (short)rcase );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL;                       /* Initiate output pointer */

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);

/* Initialize local variables.                                      */
   iu_out = I_UNDEF;
   iv_out = I_UNDEF;

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   For Debug On: Check that the surface is of type                */
/*                 CUB_SUR, RAT_SUR, LFT_SUR, PRO_SUR or MIX_SUR    */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur184");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

if ( surtype     ==   CUB_SUR ||
     surtype     ==   RAT_SUR ||
     surtype     ==   LFT_SUR ||
     surtype     ==   PRO_SUR ||
     surtype     ==   CON_SUR ||
     surtype     ==   MIX_SUR )
 ;

else if ( surtype     ==  NURB_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% NURB_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P3_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% P3_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P5_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% P5_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P7_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% P7_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }
else if ( surtype     ==    P9_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% P9_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }
else if ( surtype     ==   P21_SUR )
  {
  sprintf(errbuf, "Reverse trimming%% P21_SUR (sur184)");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }


 else
   {
   sprintf(errbuf, "Reverse trimming%% type %d (sur184)", 
     (short)surtype );
   varkon_erinit();
   return(varkon_erpush("SU3003",errbuf));
   }



   if ( rcase == 1 || rcase == 2 || rcase == 12 )
      {
      ;
      }
   else
      {
      sprintf(errbuf, " rcase %d %% ", (short)rcase );
      varkon_erinit();
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* 3. Allocate memory area for the output surface                   */
/* ______________________________________________                   */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

   maxnum = nu*nv; 

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Alloc. failed  maxnum %d\n", (short)maxnum );
  fflush(dbgfil(SURPAC)); 
  }
#endif
 sprintf(errbuf, "(alloc)%%varkon_sur_reverse");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Ptr to first patch p_frst  = %d No of patches %d\n",
    (int)p_frst  , (short)maxnum);
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Surface type equal to input type of surface:                     */
   p_surout->typ_su= (short)surtype;
/* Number of output patches:                                        */
   p_surout->nu_su = (short)(nu); 
   p_surout->nv_su = (short)(nv);          

/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 5. Create the reversed surface                                   */
/* ______________________________                                   */
/*                                                                  */
/*                                                                 !*/

for ( iv=0; iv< nv; ++iv )               /* Start loop V patches    */
  {
  for ( iu=0; iu< nu; ++iu )             /* Start loop U patches    */
    {

/*!                                                                 */
/*    Pointer to current input  topological patch                   */
      p_t_in = p_patin + iu*nv + iv; 
/*    Pointer to current output topological patch                   */
      if      ( rcase ==  1 )
        {
        iu_out = nu - iu - 1;
        iv_out = iv;
        p_t_out= p_frst + iu_out*(nv) + iv_out; 
        }
      else if ( rcase ==  2 )
        {
        iu_out = iu;
        iv_out = nv - iv - 1;
        p_t_out= p_frst + iu_out*(nv) + iv_out; 
        }
      else if ( rcase == 12 )
        {
        iu_out = nu - iu - 1;
        iv_out = nv - iv - 1;
        p_t_out= p_frst + iu_out*(nv) + iv_out; 
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Output patch number %d p_t_out %d\n",
    (short)(iu_out*(nv)+iv_out),  (int)p_t_out );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Input  patch number %d p_t_in  %d\n",
    (short)(iu    *(nv)+iv    ),  (int)p_t_in  );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Input patch %d %d Output patch %d %d\n",
    (short)iu,(short)iv , (short)iu_out, (short)iv_out );
  fflush(dbgfil(SURPAC));
  }
#endif

/*                                                                  */
/*    Reverse parameters:                                           */
/*    Call of internal function r_cubic   for CUB_PAT               */
/*                                                                 !*/

    if      ( p_t_in->styp_pat == CUB_PAT )
      {
      status= r_cubic(rcase);
      if (status<0) 
        {
        sprintf(errbuf,"r_cubic%%sur184");
        return(varkon_erpush("SU2973",errbuf));
        }
      }      /* End bicubic patch                  */

    else if ( p_t_in->styp_pat == RAT_PAT )
      {


      sprintf(errbuf,"RAT_PAT%%sur184");
      return(varkon_erpush("SU2993",errbuf));

      p_patrin = (GMPATR *)p_t_in->spek_c;

/*    Dynamic allocation of area for one geometric RAT_PAT patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(RAT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocr)%%varkon_sur_reverse");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patrout= (GMPATR *)p_gpat;        /* Current geometric patch */

      }      /* End RAT_PAT patch                    */

    else if ( p_t_in->styp_pat == LFT_PAT )
      {


      p_patlin = (GMPATL *)p_t_in->spek_c;

/*    Dynamic allocation of area for one geometric LFT_PAT patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(LFT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocl)%%varkon_sur_reverse");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patlout= (GMPATL *)p_gpat;        /* Current geometric patch */

    status = varkon_pat_loftrev ( p_patlin, rcase, p_patlout );
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_loftrev%%varkon_sur_reverse");
    return(varkon_erpush("SU2943",errbuf));
    }
      }       /* End LFT_PAT patch                    */



    else if ( p_t_in->styp_pat == CON_PAT )
      {
      sprintf(errbuf,"CON_PAT%%sur184");
      return(varkon_erpush("SU2993",errbuf));

      p_patnin = (GMPATN *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric Consurf patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(CON_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocn)%%varkon_sur_reverse");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patnout= (GMPATN *)p_gpat;        /* Current geometric patch */

      }      /* End Consurf patch                    */

    else
      {
      sprintf(errbuf,"(patch type)%%varkon_sur_reverse (sur184)");
      return(varkon_erpush("SU2993",errbuf));
      }

(*p_t_out)   = (*p_t_in);                   /* Copy topological data  */
p_t_out->spek_c   = p_gpat;                 /* Secondary patch (C ptr)*/
p_t_out->us_pat=(DBfloat)iu_out+    comptol+1;/* Start U on ge. patch */
p_t_out->ue_pat=(DBfloat)iu_out+1.0-comptol+1;/* End   U on ge. patch */
p_t_out->vs_pat=(DBfloat)iv_out+    comptol+1;/* Start V on ge. patch */
p_t_out->ve_pat=(DBfloat)iv_out+1.0-comptol+1;/* End   V on ge. patch */
p_t_out->su_pat= 0;                         /* Topological adress     */
p_t_out->sv_pat= 0;                         /* secondary patch not def*/
p_t_out->iu_pat= (short)iu_out;             /* Topological adress for */
p_t_out->iv_pat= (short)iv_out;             /* current (this) patch   */
    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */



/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout ,p_frst ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_reverse");
        return(varkon_erpush("SU2943",errbuf));
        }

/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184 Exit***varkon_sur_reverse ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** r_cubic ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Reverse a bicubic patch                                          */

   static short r_cubic ( rcase )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    rcase;        /* Trimming case:                          */
                         /* Eq.  1: Reverse U                       */
                         /* Eq.  2: Reverse V                       */
                         /* Eq. 12: Reverse U and V                 */


/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1)
{
fprintf(dbgfil(SURPAC),
"sur184*r_cubic iu %d iv %d \n" , (short)iu, (short)iv );
  fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1)
{
fprintf(dbgfil(SURPAC),
"sur184*r_cubic p_t_in %d  p_t_in->spek_c %d \n",
               (int)p_t_in, (int)p_t_in->spek_c  );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Calculate parameter values                                    */
/* _____________________________                                    */
/*                                                                 !*/


   if           ( rcase ==  1 )
      {
      r_s_u= 1.0; r_s_v= 0.0; r_e_u= 0.0; r_e_v= 1.0;
      }
   else if      ( rcase ==  2 )
      {
      r_s_u= 0.0; r_s_v= 1.0; r_e_u= 1.0; r_e_v= 0.0;
      }
   else if      ( rcase == 12 )
      {
      r_s_u= 1.0; r_s_v= 1.0; r_e_u= 0.0; r_e_v= 0.0;
      }


      p_patcin = (GMPATC *)p_t_in->spek_c;

/*    Dynamic allocation of area for one geometric bicubic patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocc)%%varkon_sur_reverse");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patcout= (GMPATC *)p_gpat;        /* Current geometric patch */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184*r_cubic p_patcin %d p_patcout %d p_gpat %d\n",
        (int)p_patcin,  (int)p_patcout ,(int)p_gpat );
  fflush(dbgfil(SURPAC)); 
  }
#endif


   status= varkon_pat_bicrep
   (p_patcin,r_s_u,r_s_v,r_e_u,r_e_v,p_patcout); 
   if (status<0) 
     {
     sprintf(errbuf,"varkon_pat_bicrep%%varkon_sur_reverse (sur184)");
     return(varkon_erpush("SU2943",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur184*r_cubic r_s_u %f r_s_v %f r_e_u %f r_e_v %f\n",
                  r_s_u,r_s_v,r_e_u,r_e_v );
  fflush(dbgfil(SURPAC)); 
  }
#endif
 
    return(SUCCED);

} /* End of function                                                */


/********************************************************************/

