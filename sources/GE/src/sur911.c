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
/*  Function: varkon_sur_bound                       File: sur911.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates all boundary boxes and boundary         */
/*  cones for a surface.                                            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-04   Originally written                                 */
/*  1994-12-10   errmes (warning) for bad patch                     */
/*  1995-02-22   Error or warning for big patch angles              */
/*  1995-09-14   Error or warning for big patch angles 45 -> 70     */
/*  1996-02-04   Warnings for different (Consurf) patch types       */
/*  1996-03-11   Warnings only for Debug On                         */
/*  1996-04-03   coneang_e = 100.0 -> coneang_e = 120.0             */
/*  1996-04-22   patcone.flag = -99 for temp_sol= 1                 */
/*  1996-11-16   Compilation warnings, P5_PAT,P7_PAT, .. LFT_PAT    */
/*  1997-03-18   BBOX for the whole surface                         */
/*  1997-05-22   Debug                                              */
/*  1998-01-06   Debug                                              */
/*  1998-01-10   SURWARN added                                      */
/*  1998-01-11   Change warnings                                    */
/*  1998-02-12   Warning 2273->2271                                 */
/*  1998-02-22   Warning 2423->2421                                 */
/*  1998-03-18   Bug comment detected in AIX                        */
/*  1999-11-28   Free source code modifications Bug call sur912     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_bound      Surface boundary boxes and cones */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_pat_bound       * Bounding box & cone for patch           */
/* varkon_sur_mboxcone    * Calculate BBOX and BCONE                */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_pat_pritop      * Print topological patch data            */
/* varkon_sur_warning     * Surface quality warning msg             */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_errmes          * Warning message to terminal             */
/* varkon_erpush          * Error   message to terminal             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2421 = WARNING: Normal angle variation exceeds 70 degrees      */
/* SU2433 = Normal angle variation exceeds 100 degrees              */
/* SU2861 = WARNING: Patch iu= iv= of surface is not OK             */
/* SU2943 = Called function .. failed in varkon_sur_bound  (sur911) */
/* SU2993 = Severe program error (  ) in varkon_sur_bound  (sur911) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_bound (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf  *p_sur,        /* Pointer to the output surface           */
  DBPatch *p_pat,        /* Pointer to alloc. area for patch data   */
  DBint    acc )         /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/* Boundary box and cone data to p_pat                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/* ----- Description of the VARKON data area for the surface -------*/
/*                                                                  */
/*  The order of the patches:                                       */
/*  -------------------------                                       */
/*                                                                  */
/*  IU=1,IV=NV                              IU=NU,IV=NV             */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  3  !  6   !  9   ! 12   ! 15  ! 18   !                      */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  2  !  5   !  8   ! 11   ! 14  ! 17   !                      */
/*   ----------------------------------------                       */
/*   !     !      !      !      !     !      !                      */
/*   !  1  !  4   !  7   ! 10   ! 13  ! 16   !                      */
/*   ----------------------------------------                       */
/*                                                                  */
/*  IU=1,IV=1                                IU=NU,IV=1             */
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  iu;            /* Loop index corresponding to u line      */
   DBint  iv;            /* Loop index corresponding to v line      */
   DBint  nu;            /* Number of patches in U direction        */
   DBint  nv;            /* Number of patches in V direction        */
   DBPatch *p_t;          /* Current topological patch         (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  coneang_e;   /* Criterion for surface normal error      */
   DBfloat  coneang_w;   /* Criterion for surface normal warning    */
   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   BBOX     patbox;      /* Bounding box  for the (PBOUND) patch    */
   BCONE    patcone;     /* Bounding cone for the (PBOUND) patch    */
   DBfloat  u_s;         /* Start U for BBOX and BCONE calculation  */
   DBfloat  u_e;         /* End   U for BBOX and BCONE calculation  */
   DBfloat  v_s;         /* Start V for BBOX and BCONE calculation  */
   DBfloat  v_e;         /* End   V for BBOX and BCONE calculation  */
   DBint    temp_sol;    /* Temporary solution for bad patches      */

   BBOX    s_box;        /* Bounding box  for the surface           */
   BCONE   s_cone;       /* Bounding cone for the surface           */
   DBfloat angmax;       /* Maximum BCONE angle for a patch         */
   DBint   nbadb;        /* Number of bad (BBOX)  patches           */
   DBint   nbadc;        /* Number of bad (BBONE) patches           */

   DBint   i_w;          /* Loop index warning                      */
   DBint   n_w;          /* Number of warnings in table SURWARN     */
   SURWARN s_w[SWMAX];   /* Warnings/errors in surface              */

   char    errbuf[80];    /* String for error message fctn erpush    */

   DBint   status;        /* Error code from a called function       */
#ifdef DEBUG
   DBint   aiu;           /* Adress to patch U. Eq. -1: All patches  */
   DBint   aiv;           /* Adress to patch V.                      */
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
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur911 Enter*varkon_sur_bound* p_sur %d p_pat %d acc %d\n", 
          (int)p_sur, (int)p_pat, (int)acc );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

   temp_sol = 1;         /* Temporary solution for bad patches      */

/* Initialize warning table                                         */

n_w = 0;
for ( i_w = 0; i_w < SWMAX; ++i_w )
  {
  s_w[i_w].warning_n1 = I_UNDEF;
  sprintf(s_w[i_w].warning_c1,"Undefined_error_record_%d",(int)i_w+1);
  s_w[i_w].warning_n2 = I_UNDEF;
  sprintf(s_w[i_w].warning_c2,"Undefined_error_record_%d",(int)i_w+1);
  s_w[i_w].u_mbs      = F_UNDEF;
  s_w[i_w].v_mbs      = F_UNDEF;
  s_w[i_w].i_u        = I_UNDEF;
  s_w[i_w].i_v        = I_UNDEF;
  }

/*!                                                                 */
/* Number of patches in U and V from p_sur                          */
/*                                                                 !*/

nu = p_sur->nu_su; 
nv = p_sur->nv_su;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Criteria for unacceptable surface normal variation in a patch.   */
/* Calls of varkon_coneange (sur...) and varkon_coneangw (sur...)   */
/*                                                                 !*/
   coneang_e = 120.0;
   coneang_w = 70.0;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 No patches in U direction nu= %d nv= %d\n",(int)nu,(int)nv);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 Computer tolerance (accuracy) %25.20f\n",comptol);
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 2. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*                                                                 !*/

for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

    p_t = p_pat + iu*nv + iv;            /* Ptr to  current patch   */


/*!                                                                 */
/*     Calculate bounding box and cone for current patch            */
/*     (current patch adress p_t= p_pat+iu*nv+iv)                   */
/*     Call of varkon_pat_bound (sur912).                           */
/*     Add BBOX and BCONE to patch data                             */
/*                                                                 !*/

/*     Note that input DBPatch adress to varkon_pat_bound (sur912)  */
/*     be p_pat (and not p_t)  !!                                   */


    u_s = p_t->us_pat;                 /* Patch boundary definition */
    u_e = p_t->ue_pat;
    v_s = p_t->vs_pat;          
    v_e = p_t->ve_pat; 

   status=varkon_pat_bound
/* (p_sur,p_pat,u_s,u_e,v_s,v_e,acc,&patbox,&patcone); 1999-11-28 */
   (p_sur,p_pat,u_s,v_s,u_e,v_e,acc,&patbox,&patcone);
   if (status<0) 
     {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 Patch  %d  %d in surface not OK\n",(int)iu+1,(int)iv+1);
  fflush(dbgfil(SURPAC));
  }
#endif
     sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2861;
     sprintf(s_w[n_w].warning_c1,"%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].warning_n2 = 2861;
     sprintf(s_w[n_w].warning_c2,"%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].u_mbs      = F_UNDEF;
     s_w[n_w].v_mbs      = F_UNDEF;
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
     varkon_erpush("SU2861",errbuf);

#ifdef  REMOVE_WARNING
     varkon_errmes();
#endif  /*  REMOVE_WARNING  */

    }

/*!                                                                 */
/*     ERROR:   Surface normal variation exceeds coneang_w          */
/*     WARNING: Surface normal variation exceeds coneang_e          */
/*                                                                 !*/

   if ( patcone.ang > coneang_e )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patcone.ang > coneang_e styp_pat %d\n",p_t->styp_pat);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
    /* Temporary solution for bad patches                           */
    if      (  temp_sol == 1 && p_t->styp_pat ==  P5_PAT )
        ; /* Do nothing */
    else if (  temp_sol == 1 && p_t->styp_pat ==  P7_PAT )
        ; /* Do nothing */
    else if (  temp_sol == 1 && p_t->styp_pat ==  P9_PAT )
        ; /* Do nothing */
    else if (  temp_sol == 1 && p_t->styp_pat == P21_PAT )
        ; /* Do nothing */
    else if (  temp_sol == 1 )
       patcone.flag = -99;
    else
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patcone.ang > coneang_e styp_pat %d\n",p_t->styp_pat);
  fflush(dbgfil(SURPAC)); 
  }
#endif
    sprintf(errbuf,"%6.1f%%%6.1f%% %d %d",
          patcone.ang,coneang_e,(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2433;
     sprintf(s_w[n_w].warning_c1, "%6.1f%%%6.1f%% %d %d",
          patcone.ang,coneang_e,(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    return(varkon_erpush("SU2433",errbuf));
    }  /* End Temporary solution for bad patches      */
    }

   if ( patcone.ang >  coneang_w )
     {
     if  ( p_t->styp_pat ==  P5_PAT || p_t->styp_pat ==  P7_PAT  ||
           p_t->styp_pat ==  P9_PAT || p_t->styp_pat == P21_PAT  ||
           p_t->styp_pat == LFT_PAT || p_t->styp_pat == NURB_PAT || 
           p_t->styp_pat == TOP_PAT                                )
       {
       ; /* Big variation possible for these patch types            */
       }
     else
       {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patcone.ang > coneang_w styp_pat %d\n",p_t->styp_pat);
  fflush(dbgfil(SURPAC)); 
  }
#endif
       sprintf(errbuf,"%6.1f%%%6.1f%% %d %d",
            patcone.ang,coneang_w,(int)iu+1,(int)iv+1 );
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2421;
     sprintf(s_w[n_w].warning_c1, "%6.1f%%%6.1f%% %d %d",
            patcone.ang,coneang_w,(int)iu+1,(int)iv+1 );
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
        varkon_erinit();
       varkon_erpush("SU2421",errbuf);
#ifdef  REMOVE_WARNING
       varkon_errmes();
#endif  /*  REMOVE_WARNING  */
       }
     }




/*!                                                                 */
/*     WARNING: Surface normal(s) not well defined in regular patch */
/*                                                                 !*/

  if ( patbox.flag == 1 && patcone.flag == -99 )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patbox.flag = 1 and patcone.flag = -99\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2271;
     sprintf(s_w[n_w].warning_c1,"%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2271",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }

/*!                                                                 */
/*     WARNING: Surface normal(s) not well defined in null tile     */
/*                                                                 !*/

  else  if ( (patbox.flag == 3 || patbox.flag == 4) && patcone.flag == -99 )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patbox.flag = 3 or 4 and patcone.flag = -99\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2263;
     sprintf(s_w[n_w].warning_c1, "%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2261",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }

/*!                                                                 */
/*     WARNING: Surface normal(s) not well defined in triang. patch */
/*                                                                 !*/

  else  if ( patbox.flag == 2 && patcone.flag == -99 )
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 patbox.flag = 2 and patcone.flag = -99\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2253;
     sprintf(s_w[n_w].warning_c1, "%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2251",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }

#ifdef  DEBUG                        /* Start warnings for Debug On  */


/*!                                                                 */
/*     WARNING: Triangular patch               For Debug On         */
/*                                                                 !*/

  else if ( patbox.flag == 2 )
    {
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2241;
     sprintf(s_w[n_w].warning_c1, "%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2241",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }


/*!                                                                 */
/*     WARNING: Null tile                      For Debug On         */
/*                                                                 !*/

  else if ( patbox.flag == 3 || patbox.flag == 4 )
    {
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2241;
     sprintf(s_w[n_w].warning_c1, "%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2241",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }

/*!                                                                 */
/*     WARNING: Patch is a point               For Debug On         */
/*                                                                 !*/

  else if ( patbox.flag == 5 )
    {
    sprintf(errbuf,"%d %d %%",(int)iu+1,(int)iv+1);
/*   Warning to table SURWARN ---------------------------- Start -- */
     if  (  n_w < SWMAX )
     {
     s_w[n_w].warning_n1 = 2221;
     sprintf(s_w[n_w].warning_c1,"%d %d %%",(int)iu+1,(int)iv+1);
     s_w[n_w].i_u        = iu+1;   
     s_w[n_w].i_v        = iv+1;   
     n_w = n_w + 1;
     }
     else n_w = n_w + 1;
/*   Warning to table SURWARN ---------------------------- End ---- */
     varkon_erinit();
    varkon_erpush("SU2221",errbuf);
#ifdef  REMOVE_WARNING
    varkon_errmes();
#endif  /*  REMOVE_WARNING  */
    }

#endif /* 1998-01-10  End   warnings for Debug On  */





    p_t->box_pat = patbox;               /* BBOX  to patch          */
    p_t->cone_pat= patcone;              /* BCONE to patch          */


/*!                                                                 */
/*     For Debug On: Printout of topological patch data.            */
/*                   Call of varkon_pat_pritop  (sur232).           */
/*                                                                 !*/


#ifdef DEBUG
  aiu = iu+1;
  aiv = iv+1;
    status = varkon_pat_pritop (p_sur,p_pat,aiu,aiv);
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pritop %%varkon_sur_interplane(sur102)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 Patch cone angle  patcone.ang %f patcone.flag %d\n",
             patcone.ang, (int)patcone.flag );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/
  
/*!                                                                 */
/*   Calculate BBOX for the surface.                                */
/*   Call of varkon_sur_mboxcone (sur913).                          */
/*                                                                 !*/

    status = varkon_sur_mboxcone
    ( p_sur, p_pat, &s_box, &s_cone, &angmax, &nbadb, &nbadc );
    if(status<0)
    {
    sprintf(errbuf,"sur913%%sur911");
    return(varkon_erpush("SU2943",errbuf));
    }

  p_sur->box_su = s_box;


/*!                                                                 */
/*   Output of warnings for poor surface quality                    */
/*   Call of varkon_sur_warning (sur490).                           */
/*                                                                 !*/

    status = varkon_sur_warning (p_sur,p_pat,n_w,s_w);
    if(status<0)
    {
    sprintf(errbuf,"sur490%%sur911");
    return(varkon_erpush("SU2943",errbuf));
    }


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 No patches in direction U nu= %d and V nv= %d \n",
          (int)nu,(int)nv);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 Surface box xmin %f xmax %f \n",
          p_sur->box_su.xmin, p_sur->box_su.xmax   );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur911 Exit* No patches nu= %d nv= %d Number of warnings %d\n"
          ,(int)nu,(int)nv, (int)n_w);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
