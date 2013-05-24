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
#include "../../IG/include/debug.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_silhouette                File: sur100.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a silhouette curve on a surface.           */
/*                                                                  */
/*  The following types of silhouette curves can be calculated:     */
/*  Case 1: Silhouette defined by   finite eye point                */
/*  Case 2: Silhouette defined by infinite eye point                */
/*  Case 3: Silhouette defined by the spine and a 2D vector         */
/*  Types which will be implemented in the future ...               */
/*  Case 4: Isophote   defined by   finite eye point                */
/*  Case 5: Isophote   defined by infinite eye point                */
/*                                                                  */
/*                                                                  */
/*  The function allocates memory area for the UV curve segments.   */
/*  This area must be deallocated by the calling function, i.e      */
/*  deallocatiom must be made when output pointer NOT is NULL.      */
/*  There will not be any allocation if requested curve number      */
/*  cur_no < 0 (determine the number of curve branches). If the     */
/*  calculation fails will area normally not be allocated, but      */
/*  the calling function must check output pointer pp_seg.          */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-11-15   Debug                                              */
/*  1994-12-03   Error message, initialise pp_seg to NULL           */
/*  1995-02-18   ntol= varkon_ctol -> ntol= varkon_ntol             */
/*  1995-03-08   No patches np_u, np_v for sur910                   */
/*  1996-02-23   Debug                                              */
/*  1996-05-26   comptol, idpoint, idangle to SILHOU sur778 added   */
/*  1996-10-23   p_gm added                                         */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1998-02-08   NURB_SUR added                                     */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1999-11-24   Free source code modifications                     */
/*  2000-01-25   Include of debug.h                                 */
/*                                                                  */
/*                                                                  */
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_silhouette Silhouette curve on a surface    */
/*                                                              */
/*------------------------------------------------------------- */

/*                                                                  */
/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_ctol();         * Coordinate tolerance          */
/*           varkon_ntol();         * Surface normal tolerance      */
/*           varkon_comptol();      * Retrieve computer tolerance   */
/*           varkon_idpoint();      * Identical points criterion    */
/*           varkon_idangle();      * Identical angles criterion    */
/*           varkon_ini_pbound();   * Initiate PBOUND               */
/*           varkon_ini_epoint();   * Initiate EPOINT               */
/*           varkon_ini_gmseg();    * Initiate DBSeg                */
/*           varkon_ini_gmcur();    * Initiate DBCurve              */
/*           varkon_sur_nopatch();  * Retrieve number of patches    */
/*          *v3mall();              * Allocate memory               */
/*           v3free();              * Free allocated memory         */
/*           varkon_sur_defpatb();  * Define patch boundaries       */
/*           varkon_sur_epts();     * Calculate entry/exit points   */
/*           varkon_sur_epts_mbs(); * Create MBS code as a check    */
/*           varkon_sur_epts_ord(); * Order EPOINT table            */
/*           varkon_sur_curbr();    * Create requested curve branch */
/*           varkon_erinit();       * Initiate error message stack  */
/*           varkon_erpush();       * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2773 = No silhouette curve (solution)                          */
/* SU2763 = Calculation only for surface with a spine               */
/* SU2793 = Calculation case x not yet implemented                  */
/* SU2943 = Called function .. failed in varkon_sur_silhouette      */
/* SU2993 = Severe program error (  ) in varkon_sur_silhouette      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus     varkon_sur_silhouette (

/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBptr    p_gm,         /* GM address to surface     (not a C ptr) */
  DBfloat  eyedata[9],   /* Silhouette definition data.             */
                         /* For case F_SILH 3-5: Eye point          */
                         /* For case I_SILH 0-2: Eye vector         */
                         /* For case S_SILH 0-2: U curve tangent    */
                         /*                  3 : Start U for spine  */
                         /*                  4 : Start V            */
                         /*                  5 : End   U for spine  */
                         /*                  6 : End   V            */
                         /*                  (All 0: Whole surface) */
                         /* For case F_ISOP 3-5: Eye point          */
                         /*                  6 : Cosine value       */
                         /* For case I_ISOP 0-2: Eye vector         */
                         /*                  6 : Cosine value       */
  DBint    rtype,        /* Computation case                        */
                         /* Eq. 1: Silhouette F_SILH (finite)       */
                         /* Eq. 2: Silhouette I_SILH (infinite)     */
                         /* Eq. 3: Silhouette S_SILH (spine)        */
                         /* Eq. 4: Isophote   F_ISOP (finite)       */
                         /* Eq. 5: Isophote   I_ISOP (infinite)     */
  DBint    acc,          /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of patches     */
                         /* Eq. 3: Triple the number of patches     */
                         /* Eq. 4:  ....                            */
  DBint    cur_no,       /* Requested output curve branch number    */
  DBint   *p_nobranch,   /* Total number of branches          (ptr) */
  DBCurve *p_cur,        /* Curve                             (ptr) */
  DBSeg  **pp_seg)       /* Allocated area for UV segments    (ptr) */

/* Out:                                                             */
/*   Surface UV curve data to to p_cur and pp_seg                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  max_seg;       /* Maximum number of curve segments        */
   DBint  nu;            /* Number of patches in U direction        */
   DBint  nv;            /* Number of patches in V direction        */
   DBint  surtype;       /* Type CUB_SUR, RAT_SUR, or .....         */
   SILHOU sildat;        /* Silhouette computation data             */
   DBfloat s_lim[2][2];  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint  pb_alloc;       /* Allocated size of patch boundary table  */
  PBOUND *p_pbtable;     /* Patch boundary   table PBOUND           */
  DBint  tnr;            /* Total number of records      in PBOUND  */
  EPOINT etable[EPMAX];  /* Patch entry and exit points             */
  DBint  ter;            /* Total number of records      in EPOINT  */
  DBint  no_br;          /* Number of curve branches     in EPOINT  */
  DBint  cbsr[CBMAX];    /* Curve branches start records in EPOINT  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  EPOINT  eprec;         /* Patch entry and exit point record       */
  DBint   i_r;           /* Loop indec record                       */
  DBint   i_s;           /* Loop index segment record               */
  DBfloat ntol;          /* Surface normal tolerance                */
  DBfloat ctol;          /* Coordinate tolerance                    */
  DBfloat idpoint;       /* Identical point criterion               */
  DBfloat idangle;       /* Identical angle criterion               */
  DBfloat comptol;       /* Computer   tolerance                    */
  DBfloat sumuv;         /* Sum of U,V start/end values             */
  DBSeg  *p_seg_l;       /* Local ptr =*pp_seg after allocation     */
  DBint   np_u;          /* Number of approx patches per patch in U */
  DBint   np_v;          /* Number of approx patches per patch in V */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!-------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_silhouette !                          */
/*               !      (sur100)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/* !  Check   !       ! UV grid  ! ! Entry ! ! Allocate !! Output ! */
/* !  input,  !       !  Calcul. ! ! /exit ! ! UV curve !! curve  ! */
/* ! initiate !       ! accuracy ! !  pts  ! !  memory  !! branch ! */
/* !__________!       !__________! !_______! !__________!!________! */
/*      !                   !           !      ___!_____      !     */
/*      !                   !           !     !         !     !     */
/*      !                   !           !     !DBcreate !     !     */
/*      !                   !           !     !_segments!     !     */
/*      !                   !           !     !ini_gmseg!     !     */
/*      !                   !           !     !_________!     !     */
/*      !_________          !           !__________           !     */
/*  ____!___  ____!___  ____!____   ____!___   ____!___   ____!___  */
/* !        !!        !!         ! !        ! !        ! !        ! */
/* ! varkon !! varkon !! varkon  ! ! varkon ! ! varkon ! ! varkon ! */
/* ! _ntol  !!  _sur  !!  _sur   ! !  _sur  ! !  _sur  ! !  _sur  ! */
/* ! _ctol  !!_nopatch!!_defpatb ! ! _epts  ! !  _epts ! ! _curbr ! */
/* ! sur752 !!        !!         ! !        ! !  _ord  ! !        ! */
/* ! sur751 !! sur230 !! sur910  ! ! sur920 ! ! sur922 ! ! sur930 ! */
/* !________!!________!!_________! !________! !________! !________! */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_ntol (sur752) and varkon_ctol (sur751).           */
/*                                                                 !*/

   ntol      = varkon_ntol();
   ctol      = varkon_ctol();
   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();

/*!                                                                 */
/* Let pp_seg= NULL                                                 */
/*                                                                 !*/
   *pp_seg = NULL;

/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;


/*!                                                                 */
/* Initialize table EPOINT                                          */
/* Calls of varkon_ini_epoint (sur775)                              */
/*                                                                 !*/

   for (i_r=1; i_r<=EPMAX; i_r= i_r+1)
   {
    varkon_ini_epoint (&eprec);
    etable[i_r-1] = eprec;
   }

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surface is of type                  */
/* CUB_SUR, RAT_SUR, LFT_SUR, PRO_SUR, CON_SUR or MIX_SUR           */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur100");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     ==  CUB_SUR ||
     surtype     ==  RAT_SUR ||
     surtype     ==  LFT_SUR ||
     surtype     ==  CON_SUR ||
     surtype     ==  POL_SUR ||
     surtype     ==   P3_SUR ||
     surtype     ==   P5_SUR ||
     surtype     ==   P7_SUR ||
     surtype     ==   P9_SUR ||
     surtype     ==  P21_SUR ||
     surtype     ==  PRO_SUR ||
     surtype     == NURB_SUR ||
     surtype     ==  MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(surface type)%%sur100");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
if      ( rtype == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur100 F_SILH: Eye point %f %f %f \n",
             eyedata[3],eyedata[4],eyedata[5]);
  }
else if ( rtype == 2 )
  {
  fprintf(dbgfil(SURPAC),"sur100 I_SILH: Eye vector %f %f %f \n",
             eyedata[0],eyedata[1],eyedata[2]);
  }
else if ( rtype == 3 )
  {
  fprintf(dbgfil(SURPAC),"sur100 S_SILH: U curve tangent %f %f %f \n",
             eyedata[0],eyedata[1],eyedata[2]);
  fprintf(dbgfil(SURPAC),"sur100 S_SILH: Start U %f V %f \n",
             eyedata[3],eyedata[4]);
  fprintf(dbgfil(SURPAC),"sur100 S_SILH: End   U %f V %f \n",
             eyedata[5],eyedata[6]);
  }
else if ( rtype == 4 )
  {
  fprintf(dbgfil(SURPAC),"sur100 F_ISOP: Eye point %f %f %f \n",
             eyedata[3],eyedata[4],eyedata[5]);
  fprintf(dbgfil(SURPAC),"sur100 F_ISOP: Cosine value %f \n",
             eyedata[6]);
  }
else if ( rtype == 5 )
  {
  fprintf(dbgfil(SURPAC),"sur100 I_ISOP: Eye vector %f %f %f \n",
             eyedata[0],eyedata[1],eyedata[2]);
  fprintf(dbgfil(SURPAC),"sur100 I_ISOP: Cosine value %f \n",
             eyedata[6]);
  }
else
  {
  fprintf(dbgfil(SURPAC),"sur100 Unknown case 0-2: %f %f %f \n",
             eyedata[0],eyedata[1],eyedata[2]);
  fprintf(dbgfil(SURPAC),"sur100 Unknown case 3-5: %f %f %f \n",
             eyedata[3],eyedata[4],eyedata[5]);
  fprintf(dbgfil(SURPAC),"sur100 Unknown case 6-8: %f %f %f \n",
             eyedata[6],eyedata[7],eyedata[8]);
  }
}

#endif

/*!                                                                 */
/* Input data rtype and eyedata to structure variable sildat.       */
/* Selected method (PARAB3), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable sildat.               */
/*                                                                 !*/


if (rtype == 1 )
    {
    sildat.ctype   = F_SILH;
    sildat.eyev_x  = 0.0;       
    sildat.eyev_y  = 0.0;       
    sildat.eyev_z  = 0.0;       
    sildat.eye_x   = eyedata[3];
    sildat.eye_y   = eyedata[4];
    sildat.eye_z   = eyedata[5];
    }
else if (rtype == 2)
    {
    sildat.ctype   = I_SILH;
    sildat.eyev_x  = eyedata[0];
    sildat.eyev_y  = eyedata[1];
    sildat.eyev_z  = eyedata[2];
    sildat.eye_x   = 0.0;       
    sildat.eye_y   = 0.0;       
    sildat.eye_z   = 0.0;       
    }
else if (rtype == 3)
    {
    sildat.ctype   = S_SILH;
    sildat.eyev_x  = eyedata[0];
    sildat.eyev_y  = eyedata[1];
    sildat.eyev_z  = eyedata[2];
    sildat.eye_x   = 0.0;       
    sildat.eye_y   = 0.0;       
    sildat.eye_z   = 0.0;       
    }

 else
    {
    sprintf(errbuf,"%d %%sur100",(short)rtype);
    return(varkon_erpush("SU2793",errbuf));
    }

sildat.method  = PARAB3;
sildat.nstart  = 3;
sildat.maxiter = 20;      

sildat.ctol    = ctol; 
sildat.ntol    = ntol; 
sildat.idpoint = idpoint;
sildat.idangle = idangle;
sildat.comptol = comptol;
sildat.d_che   = F_UNDEF;

/*!                                                                 */
/* Check that surface is of type LFT_SUR       for case S_SILH      */
/*                                                                  */
/* For Debug On: Check surface limitation data for case S_SILH      */
/*                                                                 !*/

if ( sildat.ctype == S_SILH )
  {
  if ( surtype != LFT_SUR )
    {
    sprintf(errbuf,"(CUR_SIL case 3)%%sur100");
    return(varkon_erpush("SU2763",errbuf));
    }
  }

#ifdef DEBUG
if ( sildat.ctype == S_SILH )
  {
  sumuv = fabs(eyedata[3])+ fabs(eyedata[4])+ 
          fabs(eyedata[5])+ fabs(eyedata[6]); 
  }
  if ( sumuv > 0.00001 )
     {

     if ( eyedata[3] <  1.0 )
        {
        sprintf(errbuf,"(Start U < 1)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[3] >  nu  )
        {
        sprintf(errbuf,"(Start U > nu)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[4] <  1.0 )
        {
        sprintf(errbuf,"(Start V < 1)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[4] >  nv  )
        {
        sprintf(errbuf,"(Start V > nv)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[5] <  1.0 )
        {
        sprintf(errbuf,"(End   U < 1)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[5] >  nu  )
        {
        sprintf(errbuf,"(End   U > nu)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[6] <  1.0 )
        {
        sprintf(errbuf,"(End   V < 1)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     if ( eyedata[6] >  nv  )
        {
        sprintf(errbuf,"(End   V > nv)%%sur100");
        return(varkon_erpush("SU2993",errbuf));
        }

     }       /* End sumuv > 0  */
#endif

/*!                                                                 */
/* 2. Definition of rectangular grid of u,v curves                  */
/* _______________________________________________                  */
/*                                                                  */
/* The surface may be limited for case S_SILH. In all               */
/* other cases will the whole surface be used in the                */
/* computation.                                                     */
/*                                                                  */
/* Patch boundaries (the accuracy) to be used in the                */
/* sihouette line calculation will be defined in                    */
/* function varkon_sur_defpatb (sur910).                            */
/*                                                                 !*/

if ( sildat.ctype == S_SILH )
  {
  sumuv = fabs(eyedata[3])+ fabs(eyedata[4])+ 
          fabs(eyedata[5])+ fabs(eyedata[6]); 
  if ( sumuv > 0.00001 )
     {
     s_lim[0][0] = eyedata[3];
     s_lim[0][1] = eyedata[4];
     s_lim[1][0] = eyedata[5];
     s_lim[1][1] = eyedata[6];
     }
  else
     {
     s_lim[0][0] = 1.0;
     s_lim[0][1] = 1.0;
     s_lim[1][0] = (DBfloat)nu;
     s_lim[1][1] = (DBfloat)nv;
     }
  }
else
  {
  s_lim[0][0] = 1.0;
  s_lim[0][1] = 1.0;
  s_lim[1][0] = (DBfloat)nu;
  s_lim[1][1] = (DBfloat)nv;
  }

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

/*  Number is defined in sur910. Move here and input to sur910 ?    */
    pb_alloc = nu*nv*acc*acc;

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur100");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur100 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
 sprintf(errbuf, "(PBOUND alloc)%%sur100");
 return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Initialize table PBOUND                                          */
/* Calls of varkon_ini_pbound (sur774).                             */
/*                                                                 !*/

   for (i_r=1; i_r <= pb_alloc; i_r= i_r+1)
     { 
     varkon_ini_pbound (p_pbtable+i_r-1);
     } 

np_u = 0;
np_v = 0;
status=varkon_sur_defpatb
   (p_sur,p_pat,surtype,nu,nv,s_lim,sildat.ctype,
    acc,np_u,np_v,pb_alloc, p_pbtable,&tnr);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
        sprintf(errbuf,"sur910%%sur100");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Calculation of entry/exit points                              */
/* ______________________________________                           */
/*                                                                 !*/


/*!                                                                 */
/* Calculate entry and exit points in the patches defined by        */
/* table pbtable. Call of function varkon_sur_epts (sur920).        */
/* Output entry/exit (U,V) points is stored in table etable.        */
/* The total number of records in etable is ter.                    */
/*                                                                 !*/

status=varkon_sur_epts
   (p_sur,p_pat,(IRUNON*)&sildat,p_pbtable,tnr,etable,&ter);
   if ( ter == 0 && cur_no < 0 )
      {
      *p_nobranch = 0;
      *pp_seg = NULL;
      goto  noalloc;
      }

   if ( ter == 0 ) 
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
      if (rtype == 1 )
        {
        sprintf(errbuf," %8.2f %8.2f %8.2f",
               eyedata[3],eyedata[4],eyedata[5] );
        }
        else /*   if (rtype == 2 || rtype == 3 )  */
        {
        sprintf(errbuf," %8.2f %8.2f %8.2f",
               eyedata[0],eyedata[1],eyedata[2] );
        }
      varkon_erinit();
      return(varkon_erpush("SU2683",errbuf));
      }

   if (status<0) 
      {
      if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
      sprintf(errbuf,"sur920%%sur100");
      return(varkon_erpush("SU2943",errbuf));
      }

/*!                                                                 */
/* For DEBUG and SURPAC= 2 will MBS code for lines (LIN_FREE)       */
/* between the points in table etable be created. Also R*3          */
/* lines will be created.                                           */
/* Call of function varkon_sur_epts_mbs (sur921).                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
status=varkon_sur_epts_mbs
 (p_sur,p_pat,etable,ter);
if (status<0) 
   {
   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
   sprintf(errbuf,"sur921%%sur910");
   return(varkon_erpush("SU2943",errbuf));
   }
}
#endif

/*!                                                                 */
/* Order the points in the EPOINT table and find curve branches.    */
/* Call of function varkon_sur_epts_ord (sur922)                    */
/*                                                                 !*/

status=varkon_sur_epts_ord
(etable,ter,&no_br,cbsr);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
  sprintf(errbuf,"sur922%%sur100");
  return(varkon_erpush("SU2943",errbuf));
  }

/*!                                                                 */
/* Number of branches to output variable p_nobranch.                */
/* Return if cur_no < 0 (with NULL pp_seg pointer)                  */
/*                                                                 !*/

  *p_nobranch = no_br;

  if ( cur_no < 0 )
    {
    *pp_seg = NULL;
    goto  noalloc;
    }

/*!                                                                 */
/* 4. Allocate memory for the output UV curve                       */
/* __________________________________________                       */
/*                                                                 !*/

/*!                                                                 */
/* Memory for all records multiplied by 4 will be generated.        */
/* (normally much more than is needed)                              */
/* Call of function DBcreate_segments.                              */
/*                                                                 !*/

   max_seg = ter*4;      /* Maximum number of curve segments        */

   if((*pp_seg = DBcreate_segments(max_seg))==NULL)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
    sprintf(errbuf, "(alloc cur. seg.)%%(sur100");
     return(varkon_erpush("SU2993",errbuf));
    }

/* Let locally defined pointer                                      */
   p_seg_l = *pp_seg;
/* In order to avoid problems with *'s ......                       */

/*!                                                                 */
/* Initialize curve data in p_cur and let ns_su = maximum number    */
/* of segments for the curve (the size of the allocated area).      */
/* Call of varkon_ini_gmcur (sur778).                               */
/*                                                                 !*/
    varkon_ini_gmcur (p_cur);

   p_cur->ns_cu = (short)(max_seg); 

/*!                                                                 */
/* Initialize segment data in pp_seg. Surface GM ptr to each segm.  */
/* The GM pointer is necessary since the algorithm checks that      */
/* the output curve is within tolerances, i.e. calculations are     */
/* made on the UV curve and patch data must be retrieved.           */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= max_seg; i_s= i_s+1)
   {
       varkon_ini_gmseg ( *pp_seg+i_s-1);
       ( *pp_seg+i_s-1 )->subtyp   = 1;
       ( *pp_seg+i_s-1 )->spek_gm  = p_gm;
   }

/*!                                                                 */
/*!                                                                 */
/* 5. Create output curve (branch)                                  */
/* _______________________________                                  */
/*                                                                 !*/

/*!                                                                 */
/* Create the requested (U,V) curve cur_no.                         */
/* Call of function varkon_sur_curbr (sur930)                       */
/*                                                                 !*/

status=varkon_sur_curbr
   (p_sur,p_pat,(IRUNON*)&sildat,etable,ter,no_br,
    cbsr,cur_no,p_cur,p_seg_l);
if (status<0) 
  {
  if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");
  sprintf(errbuf,"sur930%%sur100");
  return(varkon_erpush("SU2943",errbuf));
  }

/* 6. Exit                                                          */

noalloc:; /* No allocation of memory for cur_no < 0                 */

/*!                                                                 */
/* Free memory for the patch boundary table. Call of v3free.        */
/*                                                                 !*/

   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur100");


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur100 Exit*varkon_sur_silhouette* Branches %d Segments %d\n",
            (short)no_br, p_cur->ns_cu );
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
