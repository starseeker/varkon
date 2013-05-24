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
/*  Function: varkon_sur_interch                   File: sur186.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Interchange of U, V parameter directions in the surface         */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-08-22   Originally written                                 */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_interch    Interchange surface param direct.*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short i_cubic();       /* Interchange cubic parameters       */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBPatch *p_t_in;  /* Current input  topological patch  (ptr) */
static DBPatch *p_t_out; /* Current output topological patch  (ptr) */
static char   *p_gpat;   /* Allocated area geom.  patch data  (ptr) */
static GMPATC *p_patcin; /* Input   geometric, bicubic  patch (ptr) */
static GMPATC *p_patcout;/* Output  geometric, bicubic  patch (ptr) */
static DBint   iu;       /* Loop index U line of input  surface     */
static DBint   iv;       /* Loop index V line of input  surface     */
static DBfloat comptol;  /* Computer tolerance (accuracy)           */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_interch         */
/* SU2993 = Severe program error (  ) in varkon_sur_interch         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_interch (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_patin,      /* Alloc. area for topol. patch data (ptr) */
  DBint    rcase,        /* Reverse  case:                          */
                         /* Eq. 21: Interchange U and V             */
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
  DBint   iu_out;        /* U line of output surface                */
  DBint   iv_out;        /* V line of output surface                */
  DBint   nv_out;        /* Number of V patches in output surface   */
  DBint   nu;            /* No patches in U direction input surface */
  DBint   nv;            /* No patches in V direction input surface */
  DBint   maxnum;        /* Maximum number of patches (alloc. area) */
  DBPatch *p_frst;       /* Pointer to the first patch              */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   acc_box;       /* For sur911                              */
  char   *p_tpat;        /* Allocated area topol. patch data  (ptr) */
  DBint  surtype;        /* Type CUB_SUR, RAT_SUR, or ...           */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

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
"sur186 Enter***varkon_sur_interch  rcase %d\n" , (short)rcase );
fflush(dbgfil(SURPAC)); 
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL;

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
   nv_out = I_UNDEF;

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   Check the surface type                                         */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur186");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

if ( surtype     ==   CUB_SUR )  
 ;

else if ( surtype     ==   RAT_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%%RAT_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }


else if ( surtype     ==   LFT_SUR )
  {
  sprintf(errbuf, "LFT_SUR%%(sur186)");
  varkon_erinit();
  return(varkon_erpush("SU3613",errbuf));
  }


else if ( surtype     ==   PRO_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%%PRO_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }


else if ( surtype     ==   CON_SUR )
  {
  sprintf(errbuf, "CON_SUR%%(sur186)");
  varkon_erinit();
  return(varkon_erpush("SU3613",errbuf));
  }

else if ( surtype     ==   MIX_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%%MIX_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==  NURB_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% NURB_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P3_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% P3_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P5_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% P5_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

else if ( surtype     ==    P7_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% P7_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }
else if ( surtype     ==    P9_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% P9_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }
else if ( surtype     ==   P21_SUR )
  {
  sprintf(errbuf, "Interchange of parameters%% P21_SUR");
  varkon_erinit();
  return(varkon_erpush("SU3003",errbuf));
  }

 else
   {
   sprintf(errbuf, "Interchange of parameters%% type %d (sur186)", 
      (short)surtype );
   varkon_erinit();
   return(varkon_erpush("SU3003",errbuf));
   }



   if ( rcase == 21 )
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
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur186 Alloc. failed  maxnum %d\n", (short)maxnum );
  fflush(dbgfil(SURPAC));
  }
#endif
 sprintf(errbuf,"(alloc)%%varkon_sur_interch");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur186 Ptr to first patch p_frst  = %d No of patches %d\n",
    (int)p_frst  , (short)maxnum);
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Surface type equal to input type of surface:                     */
   p_surout->typ_su= (short)surtype;
/* Number of output patches (note interchange!):                    */
   p_surout->nu_su = (short)(nv); 
   p_surout->nv_su = (short)(nu);          
   nv_out          = nu;

/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 5. Create the interchanged surface                               */
/* __________________________________                               */
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
      iu_out = iv;
      iv_out = iu;
      p_t_out= p_frst + iu_out*(nv_out) + iv_out; 

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur186 Input  patch number %d p_t_in  %d\n",
    (short)(iu    *(nv)+iv    ),  (int)p_t_in  );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur186 Input patch %d %d Output patch %d %d\n",
    (short)iu,(short)iv , (short)iu_out, (short)iv_out );
  fflush(dbgfil(SURPAC));
  }
#endif

/*                                                                  */
/*    Interchange parameters:                                       */
/*    Call of internal function i_cubic   for CUB_PAT               */
/*                                                                 !*/

    if      ( p_t_in->styp_pat == CUB_PAT )
      {
      status= i_cubic();
      if (status<0) 
        {
        sprintf(errbuf,"i_cubic%%sur186");
        return(varkon_erpush("SU2973",errbuf));
        }
      }      /* End bicubic patch                  */

    else
      {
      sprintf(errbuf,"(patch type)%%varkon_sur_interch (sur186)");
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
     sprintf(errbuf,"sur911%%varkon_sur_interch (sur186)");
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
  "sur186 Exit***varkon_sur_interch ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** i_cubic ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Reverse a bicubic patch                                          */

   static short i_cubic ( )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur186*i_cubic iu %d iv %d \n" , (short)iu, (short)iv );
  fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2)
{
fprintf(dbgfil(SURPAC),
"sur186*i_cubic p_t_in %d  p_t_in->spek_c %d \n",
               (int)p_t_in, (int)p_t_in->spek_c  );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/


/*!                                                                 */
/* Pointer to the input geometric, bicubic patch                    */
/*                                                                 !*/
   p_patcin = (GMPATC *)p_t_in->spek_c;

/* Dynamic allocation of area for one geometric bicubic patch.      */
/* Call of function DBcreate_patches.                               */
   if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
     {                                     
     sprintf(errbuf,"(allocc)%%varkon_sur_interch");  
     return(varkon_erpush("SU2993",errbuf));
     }              
/*!                                                                 */
/* Pointer to the output geometric, bicubic patch                   */
/*                                                                 !*/
   p_patcout= (GMPATC *)p_gpat; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur186*i_cubic p_patcin %d p_patcout %d p_gpat %d\n",
        (int)p_patcin,  (int)p_patcout ,(int)p_gpat );
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* Output coefficients                                              */
/*                                                                 !*/

   p_patcout->a00x = p_patcin->a00x;
   p_patcout->a01x = p_patcin->a10x;
   p_patcout->a02x = p_patcin->a20x;
   p_patcout->a03x = p_patcin->a30x;

   p_patcout->a10x = p_patcin->a01x;
   p_patcout->a11x = p_patcin->a11x;
   p_patcout->a12x = p_patcin->a21x;
   p_patcout->a13x = p_patcin->a31x;

   p_patcout->a20x = p_patcin->a02x;
   p_patcout->a21x = p_patcin->a12x;
   p_patcout->a22x = p_patcin->a22x;
   p_patcout->a23x = p_patcin->a32x;

   p_patcout->a30x = p_patcin->a03x;
   p_patcout->a31x = p_patcin->a13x;
   p_patcout->a32x = p_patcin->a23x;
   p_patcout->a33x = p_patcin->a33x;


   p_patcout->a00y = p_patcin->a00y;
   p_patcout->a01y = p_patcin->a10y;
   p_patcout->a02y = p_patcin->a20y;
   p_patcout->a03y = p_patcin->a30y;

   p_patcout->a10y = p_patcin->a01y;
   p_patcout->a11y = p_patcin->a11y;
   p_patcout->a12y = p_patcin->a21y;
   p_patcout->a13y = p_patcin->a31y;

   p_patcout->a20y = p_patcin->a02y;
   p_patcout->a21y = p_patcin->a12y;
   p_patcout->a22y = p_patcin->a22y;
   p_patcout->a23y = p_patcin->a32y;

   p_patcout->a30y = p_patcin->a03y;
   p_patcout->a31y = p_patcin->a13y;
   p_patcout->a32y = p_patcin->a23y;
   p_patcout->a33y = p_patcin->a33y;


   p_patcout->a00z = p_patcin->a00z;
   p_patcout->a01z = p_patcin->a10z;
   p_patcout->a02z = p_patcin->a20z;
   p_patcout->a03z = p_patcin->a30z;

   p_patcout->a10z = p_patcin->a01z;
   p_patcout->a11z = p_patcin->a11z;
   p_patcout->a12z = p_patcin->a21z;
   p_patcout->a13z = p_patcin->a31z;

   p_patcout->a20z = p_patcin->a02z;
   p_patcout->a21z = p_patcin->a12z;
   p_patcout->a22z = p_patcin->a22z;
   p_patcout->a23z = p_patcin->a32z;

   p_patcout->a30z = p_patcin->a03z;
   p_patcout->a31z = p_patcin->a13z;
   p_patcout->a32z = p_patcin->a23z;
   p_patcout->a33z = p_patcin->a33z;



   p_patcout->ofs_pat = p_patcin->ofs_pat;


    return(SUCCED);

} /* End of function                                                */


/********************************************************************/

