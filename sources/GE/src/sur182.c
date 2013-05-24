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
/*  Function: varkon_sur_postrim                   File: sur182.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function trims a surface in positive direction, i.e         */
/*  without reverse.                                                */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-11-01   Originally written                                 */
/*  1994-11-26   Bug trimming exact in patch boundaries             */
/*  1995-03-10   Initialize pp_patout to NULL                       */
/*  1996-05-28   Erase unused variables                             */
/*  1996-06-10   CON_PAT, RAT_PAT, LFT_PAT added                    */
/*  1996-10-22   varkon_sur_trim->varkon_sur_postrim                */
/*  1998-01-03   POL_SUR, P3, P5, P7, P9, P21 added                 */
/*  1998-03-21   Elimination of compilation warning                 */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_postrim    Trim a surface without reverse   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short uvlocal();       /* Local UV parameters for reparam.   */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  r_s_u;     /* Reparameterisation start local U value  */
static DBfloat  r_s_v;     /* Reparameterisation start local V value  */
static DBfloat  r_e_u;     /* Reparameterisation end   local U value  */
static DBfloat  r_e_v;     /* Reparameterisation end   local V value  */
static DBint  s_iu;      /* Start loop value in   U direction       */
static DBint  s_iv;      /* Start loop value in   V direction       */
static DBint  e_iu;      /* End   loop value in   U direction       */
static DBint  e_iv;      /* End   loop value in   V direction       */
static DBint  iu;        /* Loop index U line of input  surface     */
static DBint  iv;        /* Loop index V line of input  surface     */
static DBfloat  s_u;       /* Start local U value                     */
static DBfloat  s_v;       /* Start local V value                     */
static DBfloat  e_u;       /* End   local U value                     */
static DBfloat  e_v;       /* End   local V value                     */
static DBfloat  comptol;   /* Computer tolerance (accuracy)           */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_bicrep       * Bicubic patch reparametris.            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_postrim         */
/* SU2993 = Severe program error (  ) in varkon_sur_postrim         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/

        DBstatus      varkon_sur_postrim (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_patin,      /* Alloc. area for topol. patch data (ptr) */
  DBfloat    s_lim[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /*                                         */
  DBint    tcase,        /* Trimming case:                          */
                         /* Eq.  1: Reparameterise                  */
                         /* Eq.  2: No reparameterisation           */
                         /* Eq. 11: As 1 but keep surface direction */
                         /* Eq. 12: As 2 but keep surface direction */
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
  DBint     iu_out;      /* U line of output surface                */
  DBint     iv_out;      /* V line of output surface                */
  DBint     nu;          /* No patches in U direction input surface */
  DBint     nv;          /* No patches in V direction input surface */
  DBint     maxnum;      /* Maximum number of patches (alloc. area) */
  DBPatch  *p_frst;      /* Pointer to the first patch              */
  DBPatch  *p_t_in;      /* Current input  topological patch  (ptr) */
  DBPatch  *p_t_out;     /* Current output topological patch  (ptr) */
  GMPATC   *p_patcin;    /* Input   geometric, bicubic  patch (ptr) */
  GMPATR   *p_patrin;    /* Input   geometric, rational patch (ptr) */
  GMPATL   *p_patlin;    /* Input   geometric, lofting  patch (ptr) */
  GMPATN   *p_patnin;    /* Input   geometric, Consurf  patch (ptr) */
  GMPATP3  *p_patp3in;   /* Input   geometric, P3       patch (ptr) */
  GMPATP5  *p_patp5in;   /* Input   geometric, P5       patch (ptr) */
  GMPATP7  *p_patp7in;   /* Input   geometric, P7       patch (ptr) */
  GMPATP9  *p_patp9in;   /* Input   geometric, P9       patch (ptr) */
  GMPATP21 *p_patp21in;  /* Input   geometric, P21      patch (ptr) */
  GMPATC   *p_patcout;   /* Output  geometric, bicubic  patch (ptr) */
  GMPATN   *p_patnout;   /* Output  geometric, Consurf  patch (ptr) */
  GMPATR   *p_patrout;   /* Output  geometric, rational patch (ptr) */
  GMPATL   *p_patlout;   /* Output  geometric, lofting  patch (ptr) */
  GMPATP3  *p_patp3out;  /* Output  geometric, P3       patch (ptr) */
  GMPATP5  *p_patp5out;  /* Output  geometric, P5       patch (ptr) */
  GMPATP7  *p_patp7out;  /* Output  geometric, P7       patch (ptr) */
  GMPATP9  *p_patp9out;  /* Output  geometric, P9       patch (ptr) */
  GMPATP21 *p_patp21out; /* Output  geometric, P21      patch (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   acc_box;       /* For sur911                              */
  char   *p_gpat;        /* Allocated area geom.  patch data  (ptr) */
  char   *p_tpat;        /* Allocated area topol. patch data  (ptr) */
  DBint  surtype;        /* Type CUB_SUR, RAT_SUR, or ...           */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */
  DBint  idum1;          /* Dummy integer                           */
  DBfloat  dum1;           /* Dummy float                             */

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
"sur182 Enter***varkon_sur_postrim  tcase %d\n" , (short)tcase );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
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

/*   Initialize local variables                                     */
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
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_postrim");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     == CUB_SUR ||           /* Check surface type      */
     surtype     == RAT_SUR ||           
     surtype     == LFT_SUR ||           
     surtype     == PRO_SUR ||           
     surtype     == CON_SUR ||           
     surtype     == POL_SUR ||           
     surtype     ==  P3_SUR ||           
     surtype     ==  P5_SUR ||           
     surtype     ==  P7_SUR ||           
     surtype     ==  P9_SUR ||           
     surtype     == P21_SUR ||           
     surtype     == MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(surface type)%%sur182");  
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

/*!                                                                 */
/* Initialize loop variables: Let input trim values define loop     */
/* limits s_iu, s_iv, e_iu, e_iv be the integer parts of s_lim-1    */
/* Check input trimming values.                                     */
/*                                                                 !*/

/* Initialize loop variables:                                       */
   s_iu   = (DBint)s_lim[0][0] - 1; /* Start loop value U           */
   s_iv   = (DBint)s_lim[0][1] - 1; /* Start loop value V           */
   e_iu   = (DBint)s_lim[1][0] - 1; /* End   loop value U           */
   e_iv   = (DBint)s_lim[1][1] - 1; /* End   loop value V           */

   s_u    =      s_lim[0][0]-(DBint)s_lim[0][0]; /* Start local U   */
   s_v    =      s_lim[0][1]-(DBint)s_lim[0][1]; /* Start local V   */
   e_u    =      s_lim[1][0]-(DBint)s_lim[1][0] ;/* End   local U   */
   e_v    =      s_lim[1][1]-(DBint)s_lim[1][1] ;/* End   local V   */

if ( tcase == 11 || tcase == 12 )
   {
   if ( s_iu == e_iu  &&  s_u > e_u )
      {
      dum1= s_u; s_u= e_u; e_u= dum1;
      }
   if ( s_iu > e_iu )
      {
      idum1= s_iu; s_iu= e_iu; e_iu= idum1;
      dum1 = s_u;  s_u = e_u;  e_u = dum1;
      }
   if ( s_iv == e_iv  &&  s_v > e_v )
      {
      dum1= s_v; s_v= e_v; e_v= dum1;
      }
   if ( s_iv > e_iv )
      {
      idum1= s_iv; s_iv= e_iv; e_iv= idum1;
      dum1 = s_v;  s_v = e_v;  e_v = dum1;
      }
/* Modify for patch boundaries                                      */
   if ( fabs(e_u) < 0.00001 && e_iu >= 1 )
      {
      e_u = 1.0; e_iu = e_iu - 1;
      }
   if ( fabs(e_v) < 0.00001 && e_iv >= 1 )
      {
      e_v = 1.0; e_iv = e_iv - 1;
      }
   }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182 Start U %f Local U %f Loop start %d\n",
                s_lim[0][0],s_u,(short)s_iu);
  fprintf(dbgfil(SURPAC),
  "sur182 Start V %f Local V %f Loop start %d\n",
                s_lim[0][1],s_v,(short)s_iv);
  fprintf(dbgfil(SURPAC),
  "sur182 End   U %f Local U %f Loop end   %d\n",
                s_lim[1][0],e_u,(short)e_iu);
  fprintf(dbgfil(SURPAC),
  "sur182 End   V %f Local V %f Loop end   %d\n",
                s_lim[1][1],e_v,(short)e_iv);
  fprintf(dbgfil(SURPAC),
  "sur182 Whole surface loop variables would be (nu*nv= %d):\n",
                (short)(nu*nv));
  fprintf(dbgfil(SURPAC),
  "sur182 For U from %d to nu-1 %d For V from %d to nv-1 %d\n",
           (short)0,(short)(nu-1),(short)0,(short)(nv-1));
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

   if ( s_iu < 0 || s_iv < 0  )
      {
      sprintf(errbuf, "%5.2f %5.2f%% %d %d",
      s_lim[0][0]-1.0, s_lim[0][1]-1.0 ,(short)nu,(short)nv );
      varkon_erinit();
      return(varkon_erpush("SU2553",errbuf));
      }
   if ( s_iu < 0 || s_iv < 0 || e_iu > nu-1 || e_iv > nv-1 )
      {
      sprintf(errbuf, "%5.2f %5.2f%% %d %d",
      s_lim[1][0]-1.0, s_lim[1][1]-1.0 ,(short)nu,(short)nv );
      varkon_erinit();
      return(varkon_erpush("SU2553",errbuf));
      }


   if ( s_iu > e_iu )
      {
      sprintf(errbuf, "start U %5.2f > end U %5.2f%% (case %d)",
      s_lim[0][0]-1.0, s_lim[1][0]-1.0 , (short)tcase );
      varkon_erinit();
      return(varkon_erpush("SU2543",errbuf));
      }
   if ( s_iv > e_iv )
      {
      sprintf(errbuf, "start V %5.2f > end V %5.2f%% (case %d)",
      s_lim[0][1]-1.0, s_lim[1][1]-1.0 , (short)tcase );
      varkon_erinit();
      return(varkon_erpush("SU2543",errbuf));
      }

   if ( tcase == 1 || tcase == 11 )
      {
      ;
      }
   else
      {
      sprintf(errbuf, " = %d %% ", (short)tcase );
      varkon_erinit();
      return(varkon_erpush("SU2543",errbuf));
      }

/*!                                                                 */
/* 3. Allocate memory area for output surface                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Calculation of memory area for output patches                    */
/*                                                                 !*/

   maxnum = (e_iu-s_iu+1)*(e_iv-s_iv+1); 

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182 Allocation for e_iu-s_iu+1 %d e_iv-s_iv+1 %d maxnum %d\n",
    (short)(e_iu-s_iu+1), (short)(e_iv-s_iv+1), (short)maxnum );
  fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 { 
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182 Alloc. failed  e_iu-s_iu+1 %d e_iv-s_iv+1 %d maxnum %d\n",
    (short)(e_iu-s_iu+1), (short)(e_iv-s_iv+1), (short)maxnum );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
 sprintf(errbuf, "(alloc)%%varkon_sur_postrim");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182 Ptr to first patch p_frst  = %d No patches %d\n",
    (int)p_frst  ,(short)maxnum);
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* Surface type equal to input type of surface:                     */
   p_surout->typ_su= (short)surtype;
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 5. Create the trimmed surface                                    */
/* _____________________________                                    */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop all V patches  iv=s_iv,....,e_iv                            */
/*  Loop all U patches  iu=s_iu,...,e_iu                            */
/*                                                                 !*/

iv_out = -1;                             /* Initiate current V line */

for ( iv=s_iv; iv<= e_iv; ++iv )         /* Start loop V patches    */
  {
  iv_out = iv_out + 1;                   /* Output current V line   */
  iu_out = -1;                           /* Initiate current U line */
  for ( iu=s_iu; iu<= e_iu; ++iu )       /* Start loop U patches    */
    {

    iu_out = iu_out + 1;                 /* Output current U line   */

/*!                                                                 */
/*    Pointer to current input  topological patch                   */
      p_t_in = p_patin + iu*nv + iv; 
/*    Pointer to current output topological patch                   */
      p_t_out= p_frst + iu_out*(e_iv-s_iv+1) + iv_out; 
/*                                                                  */
/*    Calculate local UV parameter values for reparameterization.   */
/*    Call of internal function uvlocal.                            */
/*                                                                  */
/*    Pointer to current geometric (secondary)                      */
/*    patch p_patcin, p_patrin or p_patlin from                     */
/*    the topological patch p_t_in.                                 */
/*                                                                  */
/*        i. Allocate area for output geometric patch               */
/*       ii. Copy patch data to new trimmed surface                 */
/*                                                                 !*/

   status= uvlocal();
   if (status<0) 
     {
     sprintf(errbuf,"uvlocal%%varkon_sur_postrim (sur182)");
     return(varkon_erpush("SU2973",errbuf));
     }

    if      ( p_t_in->styp_pat == CUB_PAT )
      {
      p_patcin = (GMPATC *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric bicubic patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocc)%%varkon_sur_postrim");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patcout= (GMPATC *)p_gpat;        /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patcout) = (*p_patcin);        /* Copy geometric data     */
        }
      else
        {
        status= varkon_pat_bicrep
        (p_patcin,r_s_u,r_s_v,r_e_u,r_e_v,p_patcout); 
        if (status<0) 
          {
          sprintf(errbuf,"varkon_pat_bicrep%%varkon_sur_postrim (sur182)");
          return(varkon_erpush("SU2943",errbuf));
          }
        }    /* End reparameterize  bicubic patch  */
      }      /* End bicubic patch                  */

    else if ( p_t_in->styp_pat == RAT_PAT )
      {
      p_patrin = (GMPATR *)p_t_in->spek_c;

/*    Dynamic allocation of area for one geometric RAT_PAT patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(RAT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocr)%%varkon_sur_postrim");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patrout= (GMPATR *)p_gpat;        /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patrout) = (*p_patrin);        /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf,"RAT_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the RAT_PAT patch */
      }      /* End RAT_PAT patch                    */

    else if ( p_t_in->styp_pat == LFT_PAT )
      {
      p_patlin = (GMPATL *)p_t_in->spek_c;

/*    Dynamic allocation of area for one geometric LFT_PAT patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(LFT_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocl)%%varkon_sur_postrim");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patlout= (GMPATL *)p_gpat;        /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patlout) = (*p_patlin);        /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf,"LFT_PAT%% ");
        return(varkon_erpush("SU2183",errbuf));
        }     /* End reparameterize the LFT_PAT patch */
      }       /* End LFT_PAT patch                    */



    else if ( p_t_in->styp_pat == CON_PAT )
      {
      p_patnin = (GMPATN *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric Consurf patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(CON_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocn)%%varkon_sur_postrim");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patnout= (GMPATN *)p_gpat;        /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patnout) = (*p_patnin);        /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf,"CON_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the Consurf patch */
      }      /* End Consurf patch                    */





    else if ( p_t_in->styp_pat ==  P3_PAT )
      {
      p_patp3in = (GMPATP3  *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric P3      patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(  P3_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocp3)%%sur182");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patp3out= (GMPATP3  *)p_gpat;   /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patp3out) = (*p_patp3in);    /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf," P3_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the  P3     patch */
      }      /* End  P3     patch                    */


    else if ( p_t_in->styp_pat ==  P5_PAT )
      {
      p_patp5in = (GMPATP5  *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric P5      patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(  P5_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocp5)%%sur182");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patp5out= (GMPATP5  *)p_gpat;   /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patp5out) = (*p_patp5in);    /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf," P5_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the  P5     patch */
      }      /* End  P5     patch                    */


    else if ( p_t_in->styp_pat ==  P7_PAT )
      {
      p_patp7in = (GMPATP7  *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric P7      patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(  P7_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocp7)%%sur182");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patp7out= (GMPATP7  *)p_gpat;   /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patp7out) = (*p_patp7in);    /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf," P7_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the  P7     patch */
      }      /* End  P7     patch                    */


    else if ( p_t_in->styp_pat ==  P9_PAT )
      {
      p_patp9in = (GMPATP9  *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric P9      patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches(  P9_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocp9)%%sur182");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patp9out= (GMPATP9  *)p_gpat;   /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patp9out) = (*p_patp9in);    /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf," P9_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the  P9     patch */
      }      /* End  P9     patch                    */

    else if ( p_t_in->styp_pat == P21_PAT )
      {
      p_patp21in = (GMPATP21 *)p_t_in->spek_c;
/*    Dynamic allocation of area for one geometric P21     patch.   */
/*    Call of function DBcreate_patches.                            */
      if ( (p_gpat=DBcreate_patches( P21_PAT, 1 ))==NULL)
        {                                     
        sprintf(errbuf,"(allocp21)%%sur182");  
        return(varkon_erpush("SU2993",errbuf));
        }              
      p_patp21out= (GMPATP21 *)p_gpat;   /* Current geometric patch */

      if  (  r_s_u < 0.0 && r_s_v < 0.0 && r_e_u < 0.0 && r_e_v < 0.0 )
        {
        (*p_patp21out) = (*p_patp21in);  /* Copy geometric data     */
        }
      else
        {
        sprintf(errbuf,"P21_PAT%%sur182");
        return(varkon_erpush("SU2183",errbuf));
        }    /* End reparameterize the  P21    patch */
      }      /* End  P21    patch                    */


    else
      {
      sprintf(errbuf,"patch type not impl.%%sur182");
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

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182 Input patch %d %d Output patch %d %d\n",
    (short)iu,(short)iv , (short)iu_out, (short)iv_out );
  fflush(dbgfil(SURPAC));
  }
#endif
    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */

/* Number of output patches:                                        */
   p_surout->nu_su = (short)(iu_out + 1); 
   p_surout->nv_su = (short)(iv_out + 1);          

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
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_postrim");
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
  "sur182 Exit***varkon_sur_postrim ** Output nu %d nv %d type %d\n"
    , p_surout->nu_su,p_surout->nv_su, p_surout->typ_su);
fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** uvlocal ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate local UV parameters for the reparameterization of one  */
/* patch. Negative (=F_UNDEF) output parameter values for patches   */
/* that not need to be reparameterized.                             */

   static short uvlocal ( )
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
"sur182*uvlocal iu %d iv %d s_iu %d e_iu %d s_iv %d e_iv %d \n" ,
(short)iu,(short)iv,(short)s_iu,(short)e_iu,(short)s_iv,(short)e_iv);
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Calculate parameter values                                    */
/* _____________________________                                    */
/*                                                                 !*/


   if      ( s_iu == e_iu && s_iv == e_iv )
      {
      r_s_u= s_u; r_s_v= s_v; r_e_u= e_u; r_e_v= e_v;
      }
   else if ( iu == s_iu && s_iv == e_iv )
      {
      r_s_u= s_u; r_s_v= s_v; r_e_u= 1.0; r_e_v= e_v;
      }
   else if ( s_iu == e_iu && iv == s_iv )
      {
      r_s_u= s_u; r_s_v= s_v; r_e_u= e_u; r_e_v= 1.0;
      }
   else if ( iu == e_iu && s_iv == e_iv )
      {
      r_s_u= 0.0; r_s_v= s_v; r_e_u= e_u; r_e_v= e_v;
      }
   else if ( s_iu == e_iu && iv == e_iv )
      {
      r_s_u= s_u; r_s_v= 0.0; r_e_u= e_u; r_e_v= e_v;
      }
   else if ( s_iv == e_iv )
      {
      r_s_u= 0.0; r_s_v= s_v; r_e_u= 1.0; r_e_v= e_v;
      }
   else if ( s_iu == e_iu )
      {
      r_s_u= s_u; r_s_v= 0.0; r_e_u= e_u; r_e_v= 1.0;
      }
   else if ( iu == s_iu && iv == s_iv )
      {
      r_s_u= s_u; r_s_v= s_v; r_e_u= 1.0; r_e_v= 1.0;
      }
   else if ( iu == e_iu && iv == e_iv )
      {
      r_s_u= 0.0; r_s_v= 0.0; r_e_u= e_u; r_e_v= e_v;
      }
   else if ( iu == e_iu && iv == s_iv   )
      {
      r_s_u= 0.0; r_s_v= s_v; r_e_u= e_u; r_e_v= 1.0;
      }
   else if ( iu == s_iu   && iv == e_iv )
      {
      r_s_u= s_u; r_s_v= 0.0; r_e_u= 1.0; r_e_v= e_v;
      }
   else if ( iu == s_iu )
      {
      r_s_u= s_u; r_s_v= 0.0; r_e_u= 1.0; r_e_v= 1.0;
      }
   else if ( iv == s_iv )
      {
      r_s_u= 0.0; r_s_v= s_v; r_e_u= 1.0; r_e_v= 1.0;
      }
   else if ( iu == e_iu )
      {
      r_s_u= 0.0; r_s_v= 0.0; r_e_u= e_u; r_e_v= 1.0;
      }
   else if ( iv == e_iv )
      {
      r_s_u= 0.0; r_s_v= 0.0; r_e_u= 1.0; r_e_v= e_v;
      }
    else
      {
      r_s_u= F_UNDEF; r_s_v= F_UNDEF; r_e_u= F_UNDEF; r_e_v= F_UNDEF;
      goto noreparam;
      }

    if (  fabs(r_s_u)         <  10.0*comptol &&
          fabs(r_e_u-1.0)     <  10.0*comptol &&
          fabs(r_s_v)         <  10.0*comptol &&
          fabs(r_e_v-1.0)     <  10.0*comptol     )
      {
      r_s_u= F_UNDEF; r_s_v= F_UNDEF; r_e_u= F_UNDEF; r_e_v= F_UNDEF;
      goto noreparam;
      }


/*  Program check                                                   */
      if ( fabs(r_s_u-r_e_u) < 0.000001 )
          {
          sprintf(errbuf,                  
          "r_s_u-r_e_u %f%%varkon_sur_postrim",r_s_u-r_e_u);
          return(varkon_erpush("SU2993",errbuf));
          }
      if ( fabs(r_s_v-r_e_v) < 0.000001 )
          {
          sprintf(errbuf,                  
          "r_s_v-r_e_v %f%%varkon_sur_postrim",r_s_v-r_e_v);
          return(varkon_erpush("SU2993",errbuf));
          }

noreparam:;         /* No reparameterization                        */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && r_s_u >= 0.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182*uvlocal r_s_u %f r_s_v %f r_e_u %f r_e_v %f\n",
                  r_s_u,r_s_v,r_e_u,r_e_v );
  }
if ( dbglev(SURPAC) == 2 && r_s_u <  0.0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur182*uvlocal Copy whole patch (no reparameterization) \n");
  }
#endif
 
    return(SUCCED);

} /* End of function                                                */


/********************************************************************/

