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
/*  Function: varkon_pat_eval                      File: sur220.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a given a surface patch.     */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1994-10-20   Test (MBS) function sur242                         */
/*  1994-11-20   Comments added                                     */
/*  1994-12-10   Error/warning for a bad patch                      */
/*  1995-12-19   GMPATN added                                       */
/*  1996-02-01   Debug added                                        */
/*  1996-06-08   ifdef DEBUG error                                  */
/*  1996-10-27   GMPATP3, GMPATP5, GMPATP7, GMPATP9, GMPATP21 added */
/*  1996-11-04   offset value to EVALS                              */
/*  1996-11-07   offset second derivatives sur219                   */
/*  1997-02-09   sur242 erased (sur222 with lots of debug)          */
/*  1997-11-08   NURB_SUR evaluation (sur242) added                 */
/*  1997-11-29   Change text of error message                       */
/*  1997-12-10   Offset for NURBS added, Debug, patch type 5        */
/*  1998-02-01   Debug NURBS                                        */
/*  1998-05-04   Initialization xyz_d_u and _d_v only for Debug On  */
/*               p_xyz->lftpat = TRUE for lofting patch             */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_eval       Surface patch evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short normal();        /* Surface normal                     */
static short c_epsil();       /* Surface normal derivatives         */
static short offder();        /* Offset coordinates and derivatives */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static bool    offpat;            /* Flag for an offset patch       */
static DBfloat offset;            /* Offset for patch               */
static DBfloat r_x,r_y,r_z;       /* Coordinates          r(u)      */
static DBfloat u_x,u_y,u_z;       /* Tangent             dr/du      */
static DBfloat v_x,v_y,v_z;       /* Tangent             dr/dv      */
static DBfloat u2_x,u2_y,u2_z;    /* Second derivative  d2r/du2     */
static DBfloat v2_x,v2_y,v2_z;    /* Second derivative  d2r/dv2     */
static DBfloat uv_x,uv_y,uv_z;    /* Twist vector       d2r/dudv    */
static DBfloat n_x,n_y,n_z;       /* Surface normal       n(u)      */
static DBfloat nu_x,nu_y,nu_z;    /* Normal derivative   dn/du      */
static DBfloat nv_x,nv_y,nv_z;    /* Normal derivative   dn/dv      */
static DBfloat n_leng;            /* Length of vector dr/du X dr/dv */
static DBfloat epsilon;           /* Epsilon for offset surfaces    */
static DBfloat idpoint;           /* Identical point criterion      */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush             * Error message to terminal  */
/*           varkon_pat_biceval        * Bicubic  patch evaluation  */
/*           varkon_pat_conloft        * Conic lofting patch eval.  */
/*           varkon_pat_rateval        * Rational patch evaluation  */
/*           varkon_pat_coneval        * Consurf  patch evaluation  */
/*           varkon_pat_p3eval         * Degree  3 patch evaluation */
/*           varkon_pat_p5eval         * Degree  5 patch evaluation */
/*           varkon_pat_p7eval         * Degree  7 patch evaluation */
/*           varkon_pat_p9eval         * Degree  9 patch evaluation */
/*           varkon_pat_p21eval        * Degree 21 patch evaluation */
/*           varkon_pat_nurbs          * NURBS           evaluation */
/*           varkon_sur_princurv       * Principal curvature        */
/*           varkon_sur_normderiv      * Normal and its derivatives */
/*           varkon_idpoint            * Identical points criterion */
/*           varkon_ini_evals          * Initiate EVALS             */
/*           varkon_erinit             * Init. error message stack  */
/*           varkon_errmes             * Warning message to terminal*/
/*                                                                  */
/*-----------------------------------------------------------------!*/
/* For debug varkon_mbs_conloft: Conic lofting patch MBS            */

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2493 = Surface patch iu,iv is not OK    (varkon_pat_eval)      */
/* SU2481 = WARNING: Conic lofting patch iu= iv= is not OK          */
/* SU2943 = Called function xxxxxx failed in varkon_pat_eval        */
/* SU2983 = sur220 Illegal computation case=  for varkon_pat_eval   */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_eval      */
/* SU2962 = sur220 Surface normal is a zero vector in u= , v=       */
/* SU2872 = sur220 Poor surface (derivatives)      in u= , v=       */
/* SU2993 = Severe program error in varkon_pat_eval (sur220).       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_pat_eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Current topological patch         (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq. 3: First  derivatives, normal       */
                         /* Eq. 4: Second derivatives, normal deriv.*/
                         /* Eq.  : All  derivatives                 */
   DBfloat  u_pat,       /* Topological patch U parameter value     */
   DBfloat  v_pat,       /* Topological patch V parameter value     */
   EVALS  *p_xyz )       /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat   u_patg;     /* Geometric   patch U parameter value     */
   DBfloat   v_patg;     /* Geometric   patch V parameter value     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   EVALS     xyz_d;      /* Coordinates and derivatives             */
   EVALS     xyz_d_u;    /* Coordinates and derivatives + epsilon U */
   EVALS     xyz_d_v;    /* Coordinates and derivatives + epsilon V */
   DBint     icase_d;    /* Case for xyz_d (varkon_sur_eval)        */
   GMPATC   *p_patc;     /* Bicubic       patch               (ptr) */
   GMPATR   *p_patr;     /* Rational      patch               (ptr) */
   GMPATL   *p_patl;     /* Conic lofting patch               (ptr) */
   GMPATN   *p_patn;     /* Consurf       patch               (ptr) */
   GMPATP3  *p_pat3;     /* Polynomial patch degree  3        (ptr) */
   GMPATP5  *p_pat5;     /* Polynomial patch degree  5        (ptr) */
   GMPATP7  *p_pat7;     /* Polynomial patch degree  7        (ptr) */
   GMPATP9  *p_pat9;     /* Polynomial patch degree  9        (ptr) */
   GMPATP21 *p_pat21;    /* Polynomial patch degree 21        (ptr) */
   GMPATNU  *p_patnu;    /* NURBS                             (ptr) */
   DBPatch  *p_patt;     /* Topological patch                 (ptr) */
   char      errbuf[80]; /* String for error message fctn erpush    */
   DBint     status;     /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/
/* Initialize variabel EVALS                                        */
/* Calls of varkon_ini_evals (sur770).                              */
/*                                                                  */

   varkon_ini_evals (&xyz_d); 

#ifdef DEBUG  
   varkon_ini_evals (&xyz_d_u); 
   varkon_ini_evals (&xyz_d_v); 
#endif 

    epsilon = F_UNDEF;

   idpoint   = varkon_idpoint();

/*!                                                                 */
/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
   initial(icase,u_pat,v_pat,p_xyz); 
#endif

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 9 ) 
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure icase= %d \n", (short)icase );
  }
#endif
    sprintf(errbuf,"%d%% varkon_pat_eval (sur220)",(short)icase);
    return(varkon_erpush("SU2983",errbuf));
  }

/* Initialization of internal variables for Debug On                */
#ifdef DEBUG
    offset  = F_UNDEF;
    icase_d = I_UNDEF;
    u_patg  = F_UNDEF;
    v_patg  = F_UNDEF;
    p_patnu = NULL;
#endif

/* Initialization to a non-lofting patch                            */
   p_xyz->lftpat = FALSE;

/*!                                                                 */
/* 2. Retrieve data from topological patch                          */
/* _______________________________________                          */
/*                                                                  */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Secondary patch: spek_c %d\n", (int)p_pat->spek_c );
  }

if      ( dbglev(SURPAC) == 1 )
{
  if      ( p_pat->styp_pat == CUB_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is CUB_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  p_patc = (GMPATC *)p_pat->spek_c;
  fprintf(dbgfil(SURPAC),
  "sur220 From the geometric patch ..  Offset %f\n", 
     p_patc->ofs_pat);
  }
  else if ( p_pat->styp_pat == RAT_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is RAT_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == CON_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is CON_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == LFT_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is LFT_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == FAC_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is FAC_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat ==  P3_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is  P3_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat ==  P5_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is  P5_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat ==  P7_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is  P7_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat ==  P9_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is  P9_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == P21_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is P21_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == NURB_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is NURB_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat ==  TOP_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is  TOP_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
  else if ( p_pat->styp_pat == NUL_PAT )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Type of secondary patch is NUL_PAT (styp_pat= %d) \n", 
     (int)p_pat->styp_pat);
  }
}

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Topological adress: This patch %d %d Secondary pat. %d %d\n", 
       (int)p_pat->iu_pat,(int)p_pat->iv_pat,(int)p_pat->su_pat,(int)p_pat->sv_pat);
  }

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Geometry limit us %8.4f vs %8.4f ue %8.4f ve %8.4f  \n", 
     p_pat->us_pat,p_pat->vs_pat,p_pat->ue_pat,p_pat->ve_pat );
  }

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Pointer to geometry patch (char)  p_pat->spek_c %d  \n", 
     (int)p_pat->spek_c );
  }

 fflush(dbgfil(SURPAC));
#endif

/*!                                                                 */
/*  Check that geometry patch type styp_pat is implemented          */
/*                                                                 !*/

/*  Check that secondary patch adress su_pat,sv_pat not is defined  */
/*  (since it is not yet implemented)                               */

  if      ( p_pat->styp_pat ==  CUB_PAT ||
            p_pat->styp_pat ==  RAT_PAT ||
            p_pat->styp_pat ==  CON_PAT ||
            p_pat->styp_pat ==   P3_PAT ||
            p_pat->styp_pat ==   P5_PAT ||
            p_pat->styp_pat ==   P7_PAT ||
            p_pat->styp_pat ==   P9_PAT ||
            p_pat->styp_pat ==  P21_PAT ||
            p_pat->styp_pat ==  TOP_PAT ||
            p_pat->styp_pat == NURB_PAT ||
            p_pat->styp_pat ==  LFT_PAT )
     {
     /* OK */
     }
  else
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure patch type %d \n", (int)p_pat->styp_pat );
  fprintf(dbgfil(SURPAC),
  "sur220 Secondary patch: spek_c %d\n", (int)p_pat->spek_c );
  fprintf(dbgfil(SURPAC),
  "sur220 Topological adress: This patch %d %d Secondary pat. %d %d\n", 
       p_pat->iu_pat,p_pat->iv_pat,p_pat->su_pat,p_pat->sv_pat);
  fprintf(dbgfil(SURPAC),
  "sur220 Geometry limit us %8.4f vs %8.4f ue %8.4f ve %8.4f  \n", 
     p_pat->us_pat,p_pat->vs_pat,p_pat->ue_pat,p_pat->ve_pat );
  fprintf(dbgfil(SURPAC),
  "sur220 Pointer to geometry patch (char)  p_pat->spek_c %d  \n", 
     (int)p_pat->spek_c );
  }
#endif
     sprintf(errbuf,"(styp_pat)%%sur220");
     return(varkon_erpush("SU2993",errbuf));
     }

/* Secondary patch  Get geometric patch. To be added !!!!!!!!!!! */ 

  if (p_pat->su_pat != 0 && p_pat->sv_pat != 0 &&
            p_pat->styp_pat ==  TOP_PAT              )
     {
     /* Secondary patch not fully implemented for NURBS !!!! */
     p_patt = NULL; /* Defined by su_pat,sv_pat but start DBPatch is  */
                    /* not known, and may not be retrieved from GM  */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure secondary patch not yet fully impl. su_pat %d sv_pat %d \n", 
         (int)p_pat->su_pat,(int)p_pat->sv_pat);
  }
#endif
     sprintf(errbuf,"(su_pat,sv_pat)%%sur220");
     return(varkon_erpush("SU2993",errbuf));
     }
     
/*!                                                                 */
/*  Check that the patch is OK. There will only be a warning        */
/*  if it is a conic lofting patch, and error for all other         */
/*  types.                                                          */
/*                                                                 !*/

if      ( p_pat->box_pat.flag == -99 ) 
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Bad patch %d %d \n", p_pat->iu_pat,p_pat->iv_pat);
  fflush(dbgfil(SURPAC));
  }
#endif
    if ( p_pat->styp_pat == LFT_PAT )
        {
        sprintf(errbuf,"%d %d%%(varkon_pat_eval)", 
                 p_pat->iu_pat,p_pat->iv_pat);
        varkon_erinit();
        varkon_erpush("SU2481",errbuf);
        varkon_errmes();
        }
    else  /* All patch types except conic lofting */
        {
        sprintf(errbuf,"%d %d%%(varkon_pat_eval)", 
                  p_pat->iu_pat,p_pat->iv_pat);
        varkon_erinit();
        return(varkon_erpush("SU2493",errbuf));
        }
    }

/*!                                                                 */
/*  Calculate geometric (local) parameter point.                    */
/*  Let u_patg=                                                     */
/*  !!! For the moment no calculation !!!!                          */
/*                                                                 !*/

   u_patg = u_pat;
   v_patg = v_pat;


/*!                                                                 */
/* 3. Calculate coordinates and derivatives (non-offset)            */
/* _____________________________________________________            */
/*                                                                  */
/*  Switch to the right surface patch evaluation routine:           */
/*  Call of varkon_pat_biceval (sur221) for styp_pat= CUB_PAT       */
/*  Call of varkon_pat_rateval (sur224) for styp_pat= RAT_PAT       */
/*  Call of varkon_pat_conloft (sur222) for styp_pat= LFT_PAT       */
/*  Call of varkon_pat_coneval (sur226) for styp_pat= CON_PAT       */
/*  Call of varkon_pat_p3eval  (sur500) for styp_pat=  P3_PAT       */
/*  Call of varkon_pat_p5eval  (sur502) for styp_pat=  P5_PAT       */
/*  Call of varkon_pat_p7eval  (sur503) for styp_pat=  P7_PAT       */
/*  Call of varkon_pat_p9eval  (sur504) for styp_pat=  P9_PAT       */
/*  Call of varkon_pat_p21eval (sur505) for styp_pat= P21_PAT       */
/*  Call of varkon_pat_nurbs   (sur242) for styp_pat= NURB_PAT      */
/*  Retrieve also patch offset ofs_pat.                             */
/*                                                                 !*/

if      ( p_pat->styp_pat == CUB_PAT ) 
    {
    p_patc= (GMPATC *)p_pat->spek_c;
    offset= p_patc->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_biceval
    ( p_patc , icase_d, u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure sur221 u_patg %f v_patg %f\n", u_patg , v_patg );
  }
#endif
        sprintf(errbuf,"bicubic%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2973",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_biceval
    ( p_patc , icase_d, u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure sur221 u_patg %f v_patg %f\n", u_patg , v_patg );
  }
#endif
        sprintf(errbuf,"bicubic U%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2973",errbuf));
        }
    status=varkon_pat_biceval
    ( p_patc , icase_d, u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure sur221 u_patg %f v_patg %f\n", u_patg , v_patg );
  }
#endif
        sprintf(errbuf,"bicubic V%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2973",errbuf));
        }
        } /* End offpat */
    }  /* End CUB_PAT */


else if ( p_pat->styp_pat == RAT_PAT ) 
    {
    p_patr= (GMPATR *)p_pat->spek_c;
    offset= p_patr->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_rateval
    ( p_patr , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
        sprintf(errbuf,"sur224%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }

    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_rateval
    ( p_patr , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
        sprintf(errbuf,"sur224 U%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_rateval
    ( p_patr , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
        sprintf(errbuf,"sur224 V%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */
    }  /* End RAT_PAT */


else if ( p_pat->styp_pat == LFT_PAT )
    {
/*  A conic lofting patch                                           */
    p_xyz->lftpat = TRUE;
    p_patl= (GMPATL *)p_pat->spek_c;
    offset= p_patl->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }

    status=varkon_pat_conloft
    ( p_patl , icase_d, u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
        sprintf(errbuf,"sur222%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
      status=varkon_pat_conloft
      ( p_patl , icase_d, u_patg+epsilon, v_patg ,&xyz_d_u);
      if (status<0) 
          {
          sprintf(errbuf,"sur222%%varkon_pat_eval (sur220)");
          return(varkon_erpush("SU2943",errbuf));
          }
      status=varkon_pat_conloft
      ( p_patl , icase_d, u_patg, v_patg+epsilon ,&xyz_d_v);
      if (status<0) 
          {
          sprintf(errbuf,"sur222%%varkon_pat_eval (sur220)");
          return(varkon_erpush("SU2943",errbuf));
          }
        } /* End offpat */
    }  /* End LFT_PAT */



else if ( p_pat->styp_pat == CON_PAT ) 
    {
    p_patn= (GMPATN *)p_pat->spek_c;
    offset= p_patn->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_coneval
    ( p_patn , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur226 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur226%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }

    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_coneval
    ( p_patn , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur226 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur226%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_coneval
    ( p_patn , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur226 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur226%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */
    }  /* End CON_PAT */


else if ( p_pat->styp_pat ==  P3_PAT ) 
    {
    p_pat3= (GMPATP3 *)p_pat->spek_c;
    offset= p_pat3->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_p3eval
    ( p_pat3 , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur500 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur500%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }



    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_p3eval
    ( p_pat3 , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur500 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur500%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_p3eval
    ( p_pat3 , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur500 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur500%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

    }  /* End  P3_PAT */

else if ( p_pat->styp_pat ==  P5_PAT ) 
    {
    p_pat5= (GMPATP5 *)p_pat->spek_c;
    offset= p_pat5->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_p5eval
    ( p_pat5 , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur501 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur500%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_p5eval
    ( p_pat5 , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur501 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur501%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_p5eval
    ( p_pat5 , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur501 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur501%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

    }  /* End  P5_PAT */

else if ( p_pat->styp_pat ==  P7_PAT ) 
    {
    p_pat7= (GMPATP7 *)p_pat->spek_c;
    offset= p_pat7->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_p7eval
    ( p_pat7 , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur502 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur502%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_p7eval
    ( p_pat7 , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur502 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur502%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_p7eval
    ( p_pat7 , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur502 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur502%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

    }  /* End  P7_PAT */

else if ( p_pat->styp_pat ==  P9_PAT ) 
    {
    p_pat9= (GMPATP9 *)p_pat->spek_c;
    offset= p_pat9->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_p9eval
    ( p_pat9 , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur503 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur503%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_p9eval
    ( p_pat9 , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur503 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur503%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_p9eval
    ( p_pat9 , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur503 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur503%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

    }  /* End  P9_PAT */

else if ( p_pat->styp_pat ==  P21_PAT ) 
    {
    p_pat21= (GMPATP21 *)p_pat->spek_c;
    offset= p_pat21->ofs_pat;           /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_p21eval
    ( p_pat21 , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur504 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur504%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_p21eval
    ( p_pat21, icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur504 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur504%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_p21eval
    ( p_pat21, icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Error sur504 failed u_pat %f v_pat %f\n", u_pat, v_pat);
  }
#endif
        sprintf(errbuf,"sur504%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

    }  /* End  P21_PAT */



/*  Add also TOP_PAT with secondary patch = NURBS !!!!!  */
else if ( p_pat->styp_pat == NURB_PAT  )
    {

    p_patnu= (GMPATNU *)p_pat->spek_c;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur220 NURB_PAT u %f v %f p_sur %d p_pat %d icase %d\n",
          u_pat,v_pat, (int)p_sur, (int)p_pat , (short)icase );
 fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Geometry limit us %8.4f vs %8.4f ue %8.4f ve %8.4f  \n", 
     p_pat->us_pat,p_pat->vs_pat,p_pat->ue_pat,p_pat->ve_pat );
  }
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur220 NURB_PAT p_patnu %d p_pat->spek_c %d\n",
          (int)p_patnu , (int)p_pat->spek_c );
 fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur220 NURB_PAT u_patg %f v_patg %f icase_d %d\n",
          u_patg,v_patg, (short)icase_d );
 fflush(dbgfil(SURPAC));
}
#endif


    offset= p_patnu->ofs_pat;  /* Offset value              */
    if ( offset != 0.0 ) 
      {
      offpat= TRUE;
      icase_d = 9;        /* Temporary   */
      }
    else    
      {
      offpat= FALSE;
      icase_d = icase;
      }
    status=varkon_pat_nurbs
    ( p_patnu , icase_d , u_patg, v_patg ,&xyz_d);
    if (status<0) 
        {
        sprintf(errbuf,"sur242%%varkon_pat_eval    (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }

    if  ( offpat && icase > 3 )
      {
      c_epsil(&xyz_d);  /* Calculate epsilon */
    status=varkon_pat_nurbs
    ( p_patnu , icase_d , u_patg+epsilon, v_patg ,&xyz_d_u);
    if (status<0) 
        {
        sprintf(errbuf,"sur242 U%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_pat_nurbs
    ( p_patnu , icase_d , u_patg, v_patg+epsilon ,&xyz_d_v);
    if (status<0) 
        {
        sprintf(errbuf,"sur242 V%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
        } /* End offpat */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur220 NURB_PAT u %8.4f v %8.4f xyz %8.2f %8.2f %8.2f\n",
          u_patg,v_patg,xyz_d.r_x , xyz_d.r_y , xyz_d.r_z  );
 fflush(dbgfil(SURPAC)); 
}
#endif
    }  /* End NURB_PAT */


else
    {
    sprintf(errbuf,"Invalid type%%varkon_pat_eval (sur220)");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*  Reparameterize derivatives to topological U,V derivatives       */
/*  !!! For the moment no calculation !!!!!                         */
/* Check derivatives                          for Debug on          */
/*                                                                 !*/

    r_x=  xyz_d.r_x;   
    r_y=  xyz_d.r_y;   
    r_z=  xyz_d.r_z;   
    u_x=  xyz_d.u_x; 
    u_y=  xyz_d.u_y; 
    u_z=  xyz_d.u_z; 
    v_x=  xyz_d.v_x; 
    v_y=  xyz_d.v_y; 
    v_z=  xyz_d.v_z; 
    u2_x= xyz_d.u2_x; 
    u2_y= xyz_d.u2_y; 
    u2_z= xyz_d.u2_z; 
    v2_x= xyz_d.v2_x; 
    v2_y= xyz_d.v2_y; 
    v2_z= xyz_d.v2_z; 
    uv_x= xyz_d.uv_x; 
    uv_y= xyz_d.uv_y; 
    uv_z= xyz_d.uv_z; 
    p_xyz->sp_x  = xyz_d.sp_x;    /* Coordinates       for spine    */
    p_xyz->sp_y  = xyz_d.sp_y;  
    p_xyz->sp_z  = xyz_d.sp_z;  
    p_xyz->spt_x = xyz_d.spt_x;   /* Tangent           for spine    */
    p_xyz->spt_y = xyz_d.spt_y;  
    p_xyz->spt_z = xyz_d.spt_z;  
    p_xyz->spt2_x= xyz_d.spt2_x;  /* Second derivative for spine    */
    p_xyz->spt2_y= xyz_d.spt2_y;  
    p_xyz->spt2_z= xyz_d.spt2_z;  


   if ( offpat ) goto  norm;
   if ( icase == 0 || icase == 1 || icase == 2 ) goto lcase;

#ifdef DEBUG
   if ( fabs(u_x)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
   if ( fabs(u_y)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
   if ( fabs(u_z)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
   if ( fabs(v_x)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
   if ( fabs(v_y)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
   if ( fabs(v_z)>TOL6 )
     {
     sprintf(errbuf,"%f%%%f",u_pat,v_pat);
     return(varkon_erpush("SU2872",errbuf));
     }
#endif

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Normalised surface normal                                     */
/* ____________________________                                     */
/*                                                                  */
norm: /* Label: offpat is true                                      */
/*                                                                  */
/* Call of internal function normal.                                */
/*                                                                 !*/

status= normal(u_pat,v_pat);
if (status<0) 
    {
    p_xyz->r_x= r_x;
    p_xyz->r_y= r_y;
    p_xyz->r_z= r_z;

    p_xyz->u_x= u_x;
    p_xyz->u_y= u_y;
    p_xyz->u_z= u_z;

    p_xyz->v_x= v_x;
    p_xyz->v_y= v_y;
    p_xyz->v_z= v_z;

    p_xyz->offset= offset;

    sprintf(errbuf,"surface normal%%sur220");
    return(varkon_erpush("SU2973",errbuf));
    }

/*!                                                                 */
/* 5. Normalised surface normal derivatives                         */
/* ________________________________________                         */
/*                                                                  */
/* Call of internal function normder.                               */
/*                                                                 !*/

    status=varkon_sur_normderiv ( &xyz_d );
    if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure sur219 for p_xyz_d\n");
  }
#endif
      sprintf(errbuf,"sur219 %%sur220");
      return(varkon_erpush("SU2943",errbuf));
      }

    nu_x  = xyz_d.nu_x;
    nu_y  = xyz_d.nu_y;
    nu_z  = xyz_d.nu_z;
    nv_x  = xyz_d.nv_x;
    nv_y  = xyz_d.nv_y;
    nv_z  = xyz_d.nv_z;

/*!                                                                 */
/* 6. Offset coordinates and derivatives                            */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && offpat && icase > 3 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 offset %f offpat %d icase_d %d icase %d\n", 
offset, offpat, (short)icase_d, (short)icase );
  }
#endif
   if ( offpat )
     { 
     status= offder(icase,&xyz_d,&xyz_d_u,&xyz_d_v );
     if (status<0) 
       {
       sprintf(errbuf,"offder%%sur220");
       return(varkon_erpush("SU2973",errbuf));
       }
     }

/*!                                                                 */
/* 7. Calculated derivatives to output variable p_xyz               */
/* __________________________________________________               */
/*                                                                  */

lcase:  /* Label for icase                                          */

/* Coordinates and derivatives to p_xyz                             */
/*                                                                 !*/

    p_xyz->r_x= r_x;
    p_xyz->r_y= r_y;
    p_xyz->r_z= r_z;

    p_xyz->u_x= u_x;
    p_xyz->u_y= u_y;
    p_xyz->u_z= u_z;

    p_xyz->v_x= v_x;
    p_xyz->v_y= v_y;
    p_xyz->v_z= v_z;

    p_xyz->u2_x= u2_x;
    p_xyz->u2_y= u2_y;
    p_xyz->u2_z= u2_z;

    p_xyz->v2_x= v2_x;
    p_xyz->v2_y= v2_y;
    p_xyz->v2_z= v2_z;

    p_xyz->uv_x= uv_x;
    p_xyz->uv_y= uv_y;
    p_xyz->uv_z= uv_z;

    p_xyz->n_x= n_x;
    p_xyz->n_y= n_y;
    p_xyz->n_z= n_z;

    p_xyz->nu_x= nu_x;
    p_xyz->nu_y= nu_y;
    p_xyz->nu_z= nu_z;

    p_xyz->nv_x= nv_x;
    p_xyz->nv_y= nv_y;
    p_xyz->nv_z= nv_z;

    p_xyz->offset= offset;

/*!                                                                 */
/* 8. Principal curvature                                           */
/* ______________________                                           */
/*                                                                  */
/* Calculate principal curvature if icase > 4                       */
/* Call of varkon_sur_princurv (sur212).                            */
/*                                                                 !*/

   if ( icase > 4 )
      {
    status=varkon_sur_princurv ( p_xyz );
    }



#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur220 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur220 v_x %f v_y %f v_z %f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur220 v2_x %f v2_y %f v2_z %f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur220 uv_x %f uv_y %f uv_z %f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  fprintf(dbgfil(SURPAC),
  "sur220 n_x %f n_y %f n_z %f \n",
   p_xyz->n_x,p_xyz->n_y,p_xyz->n_z);
  fprintf(dbgfil(SURPAC),
  "sur220 nu_x %f nu_y %f nu_z %f \n",
   p_xyz->nu_x,p_xyz->nu_y,p_xyz->nu_z);
  fprintf(dbgfil(SURPAC),
  "sur220 nv_x %f nv_y %f nv_z %f \n",
   p_xyz->nv_x,p_xyz->nv_y,p_xyz->nv_z);
  fprintf(dbgfil(SURPAC),
  "sur220 Exit *** varkon_pat_eval    ******* \n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** normal *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the normalised surface normal            */
/* for given derivatives dr/du and dr/dv.                           */

   static short normal(u_pat,v_pat)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:  !!!  TODO Not necessary Remove      !!!                     */
   DBfloat  u_pat;       /* Patch (local) U parameter value         */
   DBfloat  v_pat;       /* Patch (local) V parameter value         */

/*       Tangent in u direction u_x, u_y, u_z and                   */
/*       tangent in v direction v_x, v_y, v_z                       */

/* Out:                                                             */
/*       Surface normal n_x, n_y, n_z                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Calculate surface normal                                      */

/* The cross product vector (u_x,u_y,u_z) X (v_x,v_y,v_z) is        */
/* perpendicular to the surface.                                    */
/* Calculate the length of the cross vector product and             */
/* let n_x, n_y, n_z be the normalised vector.                      */
/* Error SU2963 if length is zero                                   */
/*                                                                 !*/

    n_x = u_y*v_z - u_z*v_y;
    n_y = u_z*v_x - u_x*v_z;
    n_z = u_x*v_y - u_y*v_x;

    n_leng = SQRT( n_x*n_x  + n_y*n_y  + n_z*n_z );
    if ( n_leng > 1e-8  ) 
        {
        n_x = n_x/n_leng;
        n_y = n_y/n_leng;
        n_z = n_z/n_leng;
        }
    else
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Failure surface normal length= %f u_pat %f v_pat %f\n", 
   n_leng,u_pat,v_pat );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 u_x %f u_y %f u_z %f\n",u_x,  u_y,  u_z ); 
  fprintf(dbgfil(SURPAC),
  "sur220 v_x %f v_y %f v_z %f\n",v_x,  v_y,  v_z ); 
  }
#endif
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!********* Internal ** function ** c_epsil ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates epsilon for calculation of offset        */
/* derivatives.                                                     */

   static short c_epsil ( p_xyz_d )
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS  *p_xyz_d;      /* Coordinates and derivatives       (ptr) */

/* Out:                                                             */


{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_leng;       /* Length of dr/du                         */
   DBfloat v_leng;       /* Length of dr/dv                         */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

    u_leng = SQRT(p_xyz_d->u_x*p_xyz_d->u_x + 
                  p_xyz_d->u_y*p_xyz_d->u_y +
                  p_xyz_d->u_z*p_xyz_d->u_z);
    v_leng = SQRT(p_xyz_d->v_x*p_xyz_d->v_x + 
                  p_xyz_d->v_y*p_xyz_d->v_y +
                  p_xyz_d->v_z*p_xyz_d->v_z);
    if       ( u_leng >  v_leng && u_leng > 0.000000000001) 
               epsilon = idpoint/u_leng;
    else if  ( u_leng <= v_leng && u_leng > 0.000000000001) 
               epsilon = idpoint/v_leng;
    else       epsilon = 0.000000001; /* Should not occur */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

#ifdef  DEBUG
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to F_UNDEF.                            */

   static short initial(icase,u_pat,v_pat,p_xyz)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   icase;        /* Calculation case ..                     */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   EVALS  *p_xyz;        /* Pointer to coordinates and derivatives  */
                         /* for a point on a surface                */

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
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 *** initial: icase= %d p_xyz= %d\n", (short)icase, (int)p_xyz);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

/*!                                                                 */
/* 2. Initialize output variables EVALS and static variables        */
/*                                                                  */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= F_UNDEF;   
    p_xyz->r_y= F_UNDEF;   
    p_xyz->r_z= F_UNDEF;   

    r_x= F_UNDEF;   
    r_y= F_UNDEF;   
    r_z= F_UNDEF;   

/*  Tangent             dr/du                                       */
    p_xyz->u_x= F_UNDEF;   
    p_xyz->u_y= F_UNDEF;   
    p_xyz->u_z= F_UNDEF;   

    u_x= F_UNDEF;   
    u_y= F_UNDEF;   
    u_z= F_UNDEF;   

/*  Tangent             dr/dv                                       */
    p_xyz->v_x= F_UNDEF;   
    p_xyz->v_y= F_UNDEF;   
    p_xyz->v_z= F_UNDEF;   

    v_x= F_UNDEF;   
    v_y= F_UNDEF;   
    v_z= F_UNDEF;   

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= F_UNDEF;   
    p_xyz->u2_y= F_UNDEF;   
    p_xyz->u2_z= F_UNDEF;   

    u2_x= F_UNDEF;   
    u2_y= F_UNDEF;   
    u2_z= F_UNDEF;   

/*  Second derivative  d2r/dv2                                      */
    p_xyz->v2_x= F_UNDEF;   
    p_xyz->v2_y= F_UNDEF;   
    p_xyz->v2_z= F_UNDEF;   

    v2_x= F_UNDEF;   
    v2_y= F_UNDEF;   
    v2_z= F_UNDEF;   

/*  Twist vector       d2r/dudv                                     */
    p_xyz->uv_x= F_UNDEF;   
    p_xyz->uv_y= F_UNDEF;   
    p_xyz->uv_z= F_UNDEF;   

    uv_x= F_UNDEF;   
    uv_y= F_UNDEF;   
    uv_z= F_UNDEF;   

/*  Surface normal       r(u)                                       */
    p_xyz->n_x= F_UNDEF;   
    p_xyz->n_y= F_UNDEF;   
    p_xyz->n_z= F_UNDEF;   

    n_x= F_UNDEF;   
    n_y= F_UNDEF;   
    n_z= F_UNDEF;   

/*  Normal derivative   dn/du                                       */
    p_xyz->nu_x= F_UNDEF;   
    p_xyz->nu_y= F_UNDEF;   
    p_xyz->nu_z= F_UNDEF;   

    nu_x= F_UNDEF;   
    nu_y= F_UNDEF;   
    nu_z= F_UNDEF;   

/*  Normal derivative   dn/dv                                       */
    p_xyz->nv_x= F_UNDEF;   
    p_xyz->nv_y= F_UNDEF;   
    p_xyz->nv_z= F_UNDEF;   

    nv_x= F_UNDEF;   
    nv_y= F_UNDEF;   
    nv_z= F_UNDEF;   

/*  Maximum and minimum principal curvature                          */
    p_xyz->kmax= F_UNDEF;   
    p_xyz->kmin= F_UNDEF;   

/*  Direction in R*3 for kmax                                        */
    p_xyz->kmax_x= F_UNDEF;   
    p_xyz->kmax_y= F_UNDEF;   
    p_xyz->kmax_z= F_UNDEF;   


/*  Direction in R*3 for kmin                                        */
    p_xyz->kmin_x= F_UNDEF;   
    p_xyz->kmin_y= F_UNDEF;   
    p_xyz->kmin_z= F_UNDEF;   


/*  Direction in u,v for kmax                                       */
    p_xyz->kmax_u= F_UNDEF;   
    p_xyz->kmax_v= F_UNDEF;   


/*  Direction in u,v for kmin                                       */
    p_xyz->kmin_u= F_UNDEF;   
    p_xyz->kmin_v= F_UNDEF;   


/*  Length of vector dr/du X dr/dv                                  */
    n_leng= F_UNDEF;   

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif  /* End DEBUG  */

/*!********* Internal ** function ** offder *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates coordinates and derivatives              */
/* in offset.                                                       */

      static short      offder(icase,p_xyz_d,p_xyz_d_u,p_xyz_d_v  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*       Surface normal and surface normal derivatives              */
   DBint   icase;        /* Calculation case:                       */
   EVALS  *p_xyz_d;      /* Coordinates and derivatives             */
   EVALS  *p_xyz_d_u;    /* Coordinates and derivatives             */
   EVALS  *p_xyz_d_v;    /* Coordinates and derivatives             */

/* Out:                                                             */
/*   Coordinates and derivatives r_x, r_y, ...... uv_x, uv_y,..     */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  nu2_x;       /* Second derivative normal                */
   DBfloat  nu2_y;       /* Second derivative normal                */
   DBfloat  nu2_z;       /* Second derivative normal                */
   DBfloat  nv2_x;       /* Second derivative normal                */
   DBfloat  nv2_y;       /* Second derivative normal                */
   DBfloat  nv2_z;       /* Second derivative normal                */
   DBfloat  nuv_x;       /* Second derivative normal                */
   DBfloat  nuv_y;       /* Second derivative normal                */
   DBfloat  nuv_z;       /* Second derivative normal                */
/*----------------------------------------------------------------- */

   DBint    status;     /* Error code from a called function       */
   char     errbuf[80]; /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Coordinates in offset                                         */
/*                                                                  */
/* r(u,v)= r(u,v) + offset*n(u,v)                                   */
/*                                                                 !*/

    r_x = r_x + offset*n_x;
    r_y = r_y + offset*n_y;
    r_z = r_z + offset*n_z;

/*!                                                                 */
/* 2. First derivatives in offset                                   */
/*                                                                  */
/* dr/du= dr/du + offset*dn/du   if icase > 0                       */
/* dr/dv= dr/dv + offset*dn/dv                                      */
/*                                                                 !*/

    if ( icase > 0 )
      {
      u_x = u_x + offset*nu_x;
      u_y = u_y + offset*nu_y;
      u_z = u_z + offset*nu_z;

      v_x = v_x + offset*nv_x;
      v_y = v_y + offset*nv_y;
      v_z = v_z + offset*nv_z;
#ifdef DEBUG
      u2_x = F_UNDEF;             
      u2_y = F_UNDEF;             
      u2_z = F_UNDEF;             
      v2_x = F_UNDEF;             
      v2_y = F_UNDEF;             
      v2_z = F_UNDEF;             
      uv_x = F_UNDEF;             
      uv_y = F_UNDEF;             
      uv_z = F_UNDEF;             
#endif
      }
#ifdef DEBUG
    else
      {
      u_x = F_UNDEF;             
      u_y = F_UNDEF;             
      u_z = F_UNDEF;             

      v_x = F_UNDEF;             
      v_y = F_UNDEF;             
      v_z = F_UNDEF;             
      }
#endif

/*!                                                                 */
/* 3. Second derivatives in offset                                  */
/*                                                                  */
/*    Not yet implemented                                           */
/*                                                                 !*/

   if ( icase <=  3 ) goto nosecond;

    status=varkon_sur_normderiv ( p_xyz_d   );
    if ( status < 0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220*offder Failure sur219 for p_xyz_d  \n");
  }
#endif
        sprintf(errbuf,"sur219  %%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }



    status=varkon_sur_normderiv ( p_xyz_d_u );
    if ( status < 0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220*offder Failure sur219 for p_xyz_d_u\n");
  }
#endif
        sprintf(errbuf,"sur219 U%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }
    status=varkon_sur_normderiv ( p_xyz_d_v );
    if ( status < 0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220*offder Failure sur219 for p_xyz_d_v\n");
  }
#endif
        sprintf(errbuf,"sur219 V%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }


   if  (  epsilon < 0.00000000001 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220*offder epsilon < 0 \n");
  }
#endif
        sprintf(errbuf,"epsilon%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2943",errbuf));
        }

   nu2_x = (p_xyz_d_u->nu_x - p_xyz_d->nu_x)/epsilon;
   nu2_y = (p_xyz_d_u->nu_y - p_xyz_d->nu_y)/epsilon;
   nu2_z = (p_xyz_d_u->nu_z - p_xyz_d->nu_z)/epsilon;
   nv2_x = (p_xyz_d_v->nv_x - p_xyz_d->nv_x)/epsilon;
   nv2_y = (p_xyz_d_v->nv_y - p_xyz_d->nv_y)/epsilon;
   nv2_z = (p_xyz_d_v->nv_z - p_xyz_d->nv_z)/epsilon;
   nuv_x = (p_xyz_d_v->nu_x - p_xyz_d->nu_x)/epsilon;
   nuv_y = (p_xyz_d_v->nu_y - p_xyz_d->nu_y)/epsilon;
   nuv_z = (p_xyz_d_v->nu_z - p_xyz_d->nu_z)/epsilon;


   u2_x = p_xyz_d->u2_x + offset*nu2_x;
   u2_y = p_xyz_d->u2_y + offset*nu2_y;
   u2_z = p_xyz_d->u2_z + offset*nu2_z;

   v2_x = p_xyz_d->v2_x + offset*nv2_x;
   v2_y = p_xyz_d->v2_y + offset*nv2_y;
   v2_z = p_xyz_d->v2_z + offset*nv2_z;

   uv_x = p_xyz_d->uv_x + offset*nuv_x;
   uv_y = p_xyz_d->uv_y + offset*nuv_y;
   uv_z = p_xyz_d->uv_z + offset*nuv_z;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder epsilon n_u %f %f %f\n", 
        p_xyz_d_u->nu_x, p_xyz_d_u->nu_y, p_xyz_d_u->nu_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder         n_u %f %f %f\n", 
        p_xyz_d->nu_x, p_xyz_d->nu_y, p_xyz_d->nu_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*differ/eps*offset  %f %f %f\n", 
        (p_xyz_d_u->nu_x-p_xyz_d->nu_x)/epsilon*offset, 
        (p_xyz_d_u->nu_y-p_xyz_d->nu_y)/epsilon*offset, 
        (p_xyz_d_u->nu_z-p_xyz_d->nu_z))/epsilon*offset;
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder epsilon n_v %f %f %f\n", 
        p_xyz_d_v->nv_x, p_xyz_d_v->nv_y, p_xyz_d_v->nv_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder         n_v %f %f %f\n", 
        p_xyz_d->nv_x, p_xyz_d->nv_y, p_xyz_d->nv_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*differ/eps*offset  %f %f %f\n", 
        (p_xyz_d_v->nv_x-p_xyz_d->nv_x)/epsilon*offset, 
        (p_xyz_d_v->nv_y-p_xyz_d->nv_y)/epsilon*offset, 
        (p_xyz_d_v->nv_z-p_xyz_d->nv_z))/epsilon*offset;
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder v epsilon n_u %f %f %f\n", 
        p_xyz_d_v->nu_x, p_xyz_d_v->nu_y, p_xyz_d_v->nu_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*offder         n_u %f %f %f\n", 
        p_xyz_d->nu_x, p_xyz_d->nu_y, p_xyz_d->nu_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && icase > 3 )
{
fprintf(dbgfil(SURPAC),
"sur220*differ/eps*offset  %f %f %f\n", 
        (p_xyz_d_v->nu_x-p_xyz_d->nu_x)/epsilon*offset, 
        (p_xyz_d_v->nu_y-p_xyz_d->nu_y)/epsilon*offset, 
        (p_xyz_d_v->nu_z-p_xyz_d->nu_z))/epsilon*offset;
fflush(dbgfil(SURPAC));
}
#endif

nosecond:;

    if ( icase > 3 && epsilon < 0.0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur220 Offset second derivatives not implemented (icase= %d)\n", 
            (short)icase);
  }
#endif
        sprintf(errbuf,"(icase-offset)%%varkon_pat_eval (sur220)");
        return(varkon_erpush("SU2993",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur220*varkon_pat_eval*offder x %f y %f z %f\n",
          r_x, r_y, r_z );
 fflush(dbgfil(SURPAC)); 
}
#endif



    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

