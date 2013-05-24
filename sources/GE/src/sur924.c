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
/*  Function: varkon_sur_ppts                      File: sur924.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of surface points defined by the input and output point         */
/*  table SPOINT.                                                   */
/*                                                                  */
/*  Multiple interrogation surfaces may be defined in input         */
/*  arrays psura and ppata.                                         */
/*                                                                  */
/*  Input data is also the surface interrogation calculation        */
/*  case, the method that shall be used for the numerical           */
/*  calculation and corresponding data such as number of            */
/*  restarts, tolerances, pointer to an objective function, etc.    */
/*  This is defined in a structure variable (Class) like for        */
/*  instance PROJECT (input is however union IRUNON).               */
/*  This data is passed on to a general (point creation)            */
/*  patch interrogation routine.                                    */
/*                                                                  */
/*  Note that records (points) will be added in table SPOINT        */
/*  if there are multiple solutions.                                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-20   Originally written                                 */
/*  1994-06-22   acc=3                                              */
/*  1995-03-08   No patches np_u, np_v for sur910                   */
/*  1995-08-27   Bug Something is still wrong when solutions are    */
/*               added in the table. There are double points, but   */
/*               these are erased when solutions are sorted ....    */
/*  1996-05-28   Label first deleted                                */
/*  1996-06-12   CON_SUR added                                      */
/*  1997-12-01   acc_add = 0 for bicubic surfaces with many patches */
/*               Surface types added                                */
/*  1998-02-08   NURB_SUR added                                     */
/*  1998-02-25   s_uvpt_a added, start UV not from input SPOINT     */
/*               Use of surface BBOX                                */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1998-04-18   Time tests surface box                             */
/*  1998-04-28   Dynamic allocation ==> 50% increase of calc. time! */
/*               (pb_alloc= PBMAX three times)                      */
/*  1998-04-29   Initialization of PBOUND only for Debug On         */
/*               Number of records in PBOUND defined by the surface */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_ppts       Calculate surface SPOINT points  */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short uvgiven();       /* A UV start point is given          */
static short addspoi();       /* Add solution to table SPOINT       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
static short pripts();        /* Print surface points               */
#endif
/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function   xxxxxx failed in varkon_sur_ppts      */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_ppts      */
/* SU2993 = Severe program error xxx in varkon_sur_ppts (sur924).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_patadr       * Patch iu,iv address                    */
/* varkon_sur_num2         * Patch two parameter calculation        */
/* varkon_ini_bcone        * Initialize BCONE                       */
/* varkon_ini_pbound       * Initialize PBOUND                      */
/**v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_pat_chebound     * Check bounding box & cone              */
/* varkon_sur_defpatb      * Define patch boundaries                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                                                                  */
/*     _________________________________________________            */
/*    !                                                 !           */
/*    !   Retrieve computation data from input IRUNON   !           */
/*    !    Let in_sn= input number of SPOINT records    !           */
/*    !_________________________________________________!           */
/*                           !                                      */
/*                           !--------------<----------------       */
/*     ______________________!__________________________     !      */
/*    !                                                 !    !      */
/*    !       Loop i_sur = i_sur + 1  (to n_isur)       !    !      */
/*    !     Current surface from psura and ppata        !    !      */
/*    ! Retrieve number of patches (varkon_sur_nopatch) !    !      */
/*    !_________________________________________________!    !      */
/*                           !                               !      */
/*     ______________________!__________________________     !      */
/*    !                                                 !    !      */
/*    ! Define the UV search area (varkon_sur_defpatb)  !    !      */
/*    ! (if the area is the same for all SPOINT pts)    !    !      */
/*    !_________________________________________________!    !      */
/*                           !---------------<-------------- !      */
/*     ______________________!__________________________    !!      */
/*    !                                                 !   !!      */
/*    !       Loop i_poi = i_poi + 1  (to in_sn)        !   !!      */
/*    !   Retrieve point data from the SPOINT table     !   !!      */
/*    !                                                 !   !!      */
/*    !_________________________________________________!   !!      */
/*   ------------>-----------!-------------<--------------- !!      */
/* !   ______________________!__________________________   !!!      */
/* !  !                                                 !  !!!      */
/* !  !       Loop i_pat = i_pat + 1  (to pbn)          !  !!!      */
/* !  !_________________________________________________!  !!!      */
/* !                         !                             !!!      */
/* !   ______________________!__________________________   !!!      */
/* !  !                                                 !  !!!      */
/* !  ! For a given UV start point only:                !  !!!      */
/*  <-! Point calculation and definition of UV search   !  !!!      */
/*    ! area if calculation fails                       !  !!!      */
/*    !_________________________________________________!  !!!      */
/*                           !                             !!!      */
/*     ______________________!__________________________   !!!      */
/*    !                                                 !  !!!      */
/*    ! Check if solution exists (varkon_pat_chebound)  !  !!!      */
/*    ! Point calculation (varkon_sur_num2 or _num1)    !  !!!      */
/*    ! Add record (solution)       (addrec)            !  !!!      */
/*    !_________________________________________________!  !!!      */
/*                           !                             !!!      */
/*                            ------------>------------------       */
/*                           !                                      */
/*     ______________________!__________________________            */
/*    !                                                 !           */
/*    ! Order (link) SPOINT points (varkon_           ) !           */
/*    !_________________________________________________!           */
/*                           !                                      */
/*                         Exit                                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBSurf *p_sur;             /* Current input surface    (ptr) */
static DBPatch *p_pat;            /* Allocat. p_sur patch area (ptr)*/
static DBVector s_uvpt;           /* Current U,V start point        */
static DBint   ctype;             /* Interrogation (comput.) case   */
static DBint   surtype;           /* Type CUB_SUR, RAT_SUR, or ...  */
static DBint   nu;                /* Number of patches in U direct. */
static DBint   nv;                /* Number of patches in V direct. */
static DBint   surtype_all[SMAX]; /* Type CUB_SUR, RAT_SUR, or ...  */
static DBint   nu_all[SMAX];      /* Number of patches in U direct. */
static DBint   nv_all[SMAX];      /* Number of patches in V direct. */
static DBint   in_sn;             /* Input no of records in SPOINT  */
static DBint   addpts;            /* Added no of records in SPOINT  */
static SPOINT  poirec;            /* Record corresp. to i_poi       */
                                  /* Input/output SPOINT record     */
static SPOINT *p_crs;             /* Ptr to current SPOINT record   */
static DBint   pb_alloc;          /* Size of patch boundary table   */
static PBOUND *p_pbtable;         /* Patch boundary   table PBOUND  */
static DBint   pbn;               /* No of records in table PBOUND  */
static DBfloat   s_lim[2][2];       /* Surface limitation U,V points  */
                                  /* (0,0): Start U  (0,1): Start V */
                                  /* (1,0): End   U  (1,1): End   V */
static EVALS   xyz_a[SMAX];       /* All varkon_sur_num2 solutions  */
static DBint   npts;              /* Number of xyz_a solutions      */

static DBint   acc;               /* Computation accuracy (case)    */
                         /* Undefined s_uvpt => all surface patches */
                         /* Defined   s_uvpt => One patch    search */
                         /* Eq. 1: All patches connected to s_uvpt  */
                         /* Defined   s_uvpt => Nine patches search */
                         /* Eq. 2: Next layer of patches ...        */
static DBint  acc_add_all[SMAX]; /* No. of patches to be added      */
static DBint  acc_add;           /* No. of patches to be added      */
static DBint  np_u;      /* Number of approx patches per patch in U */
static DBint  np_v;      /* Number of approx patches per patch in V */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus     varkon_sur_ppts (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *psura[ISMAX],/* All interrogation surfaces        (ptrs)*/
   DBPatch *ppata[ISMAX],/* Alloc. patch data areas to psura  (ptrs)*/
   DBVector s_uvpt_a[],  /* Start U,V points                        */
   DBint    n_isur,      /* Number of interrogation surfaces        */
   IRUNON  *p_comp,      /* Interrogation computation data     (ptr)*/
   SPOINT  *p_stable,    /* Surface point         table SPOINT (ptr)*/
   DBint   *p_sn )       /* Number of records in  table SPOINT (ptr)*/
/* Out:                                                             */
/*     Data to the SPOINT table                                     */
/*     Note that the number of records (*p_sn) may be increased     */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_sur;         /* Loop index corresp. to psura and ppata  */
   DBint  i_poi;         /* Loop index corresp. to SPOINT record    */
   DBint  i_pat;         /* Loop index corresp. to PBOUND record    */
   BBOX   pbox;          /* Bounding box  for the (PBOUND) patch    */
   BCONE  pcone;         /* Bounding cone for the (PBOUND) patch    */
   BBOX   sur_bbox;      /* Bounding box  for the surface           */
   BCONE  sur_bcone;     /* Bounding cone for the surface (dummy)   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  sflag;         /* Eq. -1: No solution Eq. 1: Solution     */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */
#ifdef  DEBUG
   DBint  i_r;           /* Loop index PBOUND record                */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Enter *** varkon_sur_ppts *Calculate SPOINT points ****\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Number of input records i SPOINT *p_sn= %d\n",
           (int)*p_sn );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*  Input number of records in SPOINT to local variable:            */
    in_sn = *p_sn;
/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial, which also prints out the input data.          */
/*  Retrieve interrogation case ctype from the computation data.    */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
status=initial(psura,ppata,n_isur,p_comp);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%(sur924)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

   ctype = p_comp->pro_un.ctype;
   if      ( ctype == CLOSEPT )
      {
      acc = p_comp->pro_un.acc;
      }
   else if ( ctype == LINTSUR )
      {
      acc = p_comp->pro_un.acc;
      }
#ifdef DEBUG
    else
      {
      sprintf(errbuf,"ctype %d%%sur924",(int)ctype);
      return(varkon_erpush("SU2993",errbuf));
      }
#endif

/* Initialization of the (dummy) surface bounding cone              */

      varkon_ini_bcone (&sur_bcone);

/* Initialization of input SPOINT records that there is no solution */

   for ( i_poi=1; i_poi<= in_sn; ++i_poi)   /* Start loop SPOINT    */
     {
      p_crs= p_stable + i_poi - 1;
      p_crs->pflag    = -1; 
     }

   addpts = 0 ; /* Number of added records in SPOINT                */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Computation type ctype= %d Accuracy acc= %d addpts %d\n",
           (int)ctype, (int)acc, (int)addpts );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Allocate memory area for the patch boundary table p_pbtable.     */
/* One allocation which is sufficient for all surfaces is made.     */
/* The number of records is defined by the number of patches and    */
/* the surface type. Table data is set in sur910. The number        */
/* of patches that should be added for each is determined here.     */
/* Loop for all surfaces in order to determine the necessary        */
/* number of patches. Function varkon_sur_nopatch (sur230)          */
/* returns the surface type and the number of patches.              */
/*                                                                 !*/

/*  Initialize number of records in PBOUND table                    */
    pb_alloc = 0;
/*  Start loop all surfaces                                         */
    for ( i_sur=1; i_sur<= n_isur; ++i_sur)
      {
/*    Current input surface                                         */
      p_sur =psura[i_sur-1];
/*    Retrieve surface type and number of patches                   */
      status = varkon_sur_nopatch
        (p_sur, &nu_all[i_sur-1], &nv_all[i_sur-1], 
           &surtype_all[i_sur-1]); 
#ifdef DEBUG
      if(status<0)
        {
        sprintf(errbuf,"sur230%%(sur924)");
        return(varkon_erpush("SU2943",errbuf));
        }
#endif


/*    Check surface type for Debug On                               */
#ifdef DEBUG
      if ( surtype_all[i_sur-1]     ==  CUB_SUR ||
           surtype_all[i_sur-1]     ==  RAT_SUR ||
           surtype_all[i_sur-1]     ==  POL_SUR ||
           surtype_all[i_sur-1]     ==   P3_SUR ||
           surtype_all[i_sur-1]     ==   P5_SUR ||
           surtype_all[i_sur-1]     ==   P7_SUR ||
           surtype_all[i_sur-1]     ==   P9_SUR ||
           surtype_all[i_sur-1]     ==  P21_SUR ||
           surtype_all[i_sur-1]     ==  LFT_SUR ||
           surtype_all[i_sur-1]     ==  CON_SUR ||
           surtype_all[i_sur-1]     == NURB_SUR ||
           surtype_all[i_sur-1]     ==  MIX_SUR )
             ;
       else
         {
         sprintf(errbuf, "(surface type)%%(sur924)");
         return(varkon_erpush("SU2993",errbuf));
         }
#endif   /* End DEBUG */

/*    Determine the number of patches that shall be added for       */
/*    each patch of the input surface.                              */
      if (  surtype_all[i_sur-1] == CON_SUR  ||  
            surtype_all[i_sur-1] == LFT_SUR       )
        {
        acc_add_all[i_sur-1] = 5; 
        }
      else if (  surtype_all[i_sur-1] == CUB_SUR && 
                 nu_all[i_sur-1]*nv_all[i_sur-1] >= 1000 )
        {
        acc_add_all[i_sur-1] = 0;
        }
      else
        {
        acc_add_all[i_sur-1] = 2; 
        }

/*    Determine the necessary number of PBOUND records              */
      if ( nu_all[i_sur-1]*nv_all[i_sur-1]*(1+acc_add_all[i_sur-1])
                 *(1+acc_add_all[i_sur-1]) > pb_alloc )
        pb_alloc =  nu_all[i_sur-1]*nv_all[i_sur-1]
                   *(1+acc_add_all[i_sur-1])*(1+acc_add_all[i_sur-1]);

      } /* End loop i_sur */


/* Allocate memory area for the patch boundary table p_pbtable.     */
    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur924"); 
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
      sprintf(errbuf, "(PBOUND alloc)%%sur924");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Initialize table PBOUND for Debug On                             */
/* Calls of varkon_ini_pbound (sur774).                             */
/*                                                                 !*/

#ifdef  DEBUG
   for (i_r=1; i_r <= pb_alloc; i_r= i_r+1)
     { 
     varkon_ini_pbound (p_pbtable+i_r-1);
     } 
#endif /* End DEBUG */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Memory is allocated for %d PBOUND records\n",
        (short)pb_alloc );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* 2. Calculate all closest points                                  */
/* _______________________________                                  */
/*                                                                  */
/* Loop all input surfaces psura (ppata) i_sur=1,.,n_isur           */
/*                                                                 !*/

for ( i_sur=1; i_sur<= n_isur; ++i_sur)   /* Start loop psura       */
  {

/*!                                                                 */
/*   Retrieve surface ptrs from psura and ppata and                 */
/*   start point from s_uvpt_a.                                     */
/*                                                                 !*/

     p_sur =psura[i_sur-1];        /* Current input surface         */
     p_pat=ppata[i_sur-1];         /* Patch data for p_sur          */

/*   Current surface type and number of patches                     */
     surtype = surtype_all[i_sur-1];
     nu      =      nu_all[i_sur-1];
     nv      =      nv_all[i_sur-1];
     acc_add = acc_add_all[i_sur-1];

/*   Current start U,V point                                        */
     s_uvpt.x_gm= s_uvpt_a[i_sur-1].x_gm;
     s_uvpt.y_gm= s_uvpt_a[i_sur-1].y_gm;
     s_uvpt.z_gm= s_uvpt_a[i_sur-1].z_gm;


/*!                                                                 */
/* Get the surface bounding box.                                    */
/*                                                                 !*/

     sur_bbox = p_sur->box_su;

/*!                                                                 */
/*   Check for solution using the bounding box for the surface      */
/*   Call of varkon_pat_chebound (sur914).                          */
/*                                                                 !*/

   status=varkon_pat_chebound
      (p_sur,p_pat,p_comp, 1.0 , (DBfloat)nu+1.0,1.0, 
                  (DBfloat)nv+1.0, &sur_bbox,&sur_bcone,&sflag);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");
        sprintf(errbuf,"sur914(surf)%%sur924");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Result of intersect check with the surface box. sflag %d\n",
           (int)sflag );
fflush(dbgfil(SURPAC));
}
#endif

    if ( sflag == -1 )
       {
/*     Goto next surface                                            */
       goto  _nxtsur;
       }

#ifdef  TODO_SOMETHING_ELSE_PERHAPS
    if ( sflag == -99 )
       /* Surface patch is not OK */
       {
       goto  _nxtpat;
       }
#endif

/*!                                                                 */
/*   Define the UV search area for the whole surface                */
/*   for computation case (or acc case ....          )              */
/*   Call of varkon_sur_defpatb (sur910).                           */
/*                                                                 !*/

   s_lim[0][0] = 1.0;
   s_lim[0][1] = 1.0;
   s_lim[1][0] = (DBfloat)nu;
   s_lim[1][1] = (DBfloat)nv;

np_u = 0;
np_v = 0;
   status=varkon_sur_defpatb
      (p_sur,p_pat,surtype, nu,nv,s_lim,ctype,
        acc_add,np_u,np_v,pb_alloc,p_pbtable,&pbn);
   if (status<0) 
        {
        if( p_pbtable != NULL )v3free(p_pbtable,"sur924");
        sprintf(errbuf,"sur910%%sur924");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924 Surface i_sur %d nu %d nv %d surtype %d PBOUND rec's %d in_sn %d\n",
      (int)i_sur, (int)nu, (int)nv, 
      (int)surtype , (int)pbn , (int)in_sn );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/*   Loop all records in SPOINT table i_poi=1,2,....,in_sn          */
/*                                                                 !*/

for ( i_poi=1; i_poi<= in_sn; ++i_poi)   /* Start loop SPOINT       */
  {

/*!                                                                 */
/*     Data from the SPOINT table:                                  */
       p_crs= p_stable + i_poi - 1;      /* Ptr to current record   */
       poirec= *p_crs;                   /* Copy record             */
       p_crs->us       = 0.0;            /* Necessary if there is   */
       p_crs->vs       = 0.0;            /* no solution for the pt  */
       p_crs->ue       = 0.0;
       p_crs->ve       = 0.0;
/*                                                                 !*/

/*!                                                                 */
/*     Start UV point (mapped) from the input array.                */
/*     (Previously from p_crs! Added   1998-02-25)                  */
/*                                                                 !*/

       poirec.startuvpt.x_gm = s_uvpt.x_gm;
       poirec.startuvpt.y_gm = s_uvpt.y_gm;
       poirec.startuvpt.z_gm = s_uvpt.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 Loop SPOINT i_poi= %d p_crs= %d s_uvpt.z_gm= %f\n",
     (int)i_poi , (int)p_crs  , s_uvpt.z_gm );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 Loop SPOINT extpt %8.2f %8.2f %8.2f s-uvpt %8.4f %8.4f (p_crs)\n",
     p_crs->extpt.x_gm ,  
     p_crs->extpt.y_gm ,  
     p_crs->extpt.z_gm ,  
     p_crs->startuvpt.x_gm ,  
     p_crs->startuvpt.y_gm );
  fprintf(dbgfil(SURPAC),
  "sur924 Loop SPOINT extpt %8.2f %8.2f %8.2f s-uvpt %8.4f %8.4f (poirec)\n",
     poirec.extpt.x_gm ,  
     poirec.extpt.y_gm ,  
     poirec.extpt.z_gm ,  
     poirec.startuvpt.x_gm ,  
     poirec.startuvpt.y_gm );
  }
#endif

/*!                                                                 */
/*     If the UV start point is given                               */
/*         i. Let pflag= -1   1994-03-09 !!!!                       */
/*        ii. Call of uvgiven                                       */
/*       iii. Call of addspoi                                       */
/*        iv. Let poirec.startuvpt.z_gm= -9.9 if no solution        */
/*            Goto _solex if solution exists                        */
/*                                                                 !*/

    if ( poirec.startuvpt.z_gm >= 0 )
      {
         poirec.pisur          = p_sur;
         poirec.pppat          = p_pat;
         poirec.pflag          = -1;   
         poirec.ptrs           = -1;   
      status=uvgiven(p_comp);
      if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");
        sprintf(errbuf,"uvgiven%%varkon_sur_ppts (sur924)");
        return(varkon_erpush("SU2973",errbuf));
        }
      status=addspoi (p_stable);
      if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");
        sprintf(errbuf,"addspoi%%varkon_sur_ppts (sur924)");
        return(varkon_erpush("SU2973",errbuf));
        }
      if ( npts == 0 )
        {
        poirec.startuvpt.z_gm = -9.9;
        }
      else
        {
        goto  _solex;
        }
      }    /* End UV start point is defined                 */


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*     Loop all records in PBOUND table i_pat=1,2,....,pbn          */
/*                                                                 !*/

for ( i_pat=1; i_pat<= pbn  ; ++i_pat)   /* Start loop PBOUND       */
  {

/*!                                                                 */
/*       The UV search area from PBOUND table to SPOINT table       */
/*       (us,vs,ue,ve PBOUND record i_pat to record SPOINT p_csr)   */
/*       Retrieve pbox and pcone table PBOUND                       */
/*       Let z coordinate of startuvpt be -0.99 (not defined)       */
/*       Let pflag be -1 (start value= no solution)                 */
/*                                                                 !*/

         pbox                  = (p_pbtable+i_pat-1)->pbox;
         pcone                 = (p_pbtable+i_pat-1)->pcone;
         poirec.us             = (p_pbtable+i_pat-1)->us;
         poirec.vs             = (p_pbtable+i_pat-1)->vs;
         poirec.ue             = (p_pbtable+i_pat-1)->ue;
         poirec.ve             = (p_pbtable+i_pat-1)->ve;
         poirec.startuvpt.z_gm = -0.99;
         poirec.pisur          = p_sur;
         poirec.pppat          = p_pat;
         poirec.pflag          = -1;   
         poirec.ptrs           = -1;   

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 i_pat %d Area: us %8.4f ue %8.4f vs %8.4f ve %8.4f\n",
     (int)i_pat, poirec.us,  poirec.ue,  poirec.vs,  poirec.ve   ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 LOOP PBOUND i_pat %d pbn %d poirec.startuvpt.z_gm %f \n",
     (int)i_pat , (int)pbn , poirec.startuvpt.z_gm ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 pcone xdir %8.5f ydir %8.5f zdir %8.5f ang %8.4f flag %d\n",
       pcone.xdir, pcone.ydir, pcone.zdir, pcone.ang , (int)pcone.flag);
  fprintf(dbgfil(SURPAC),
  "sur924 pbox  xmin %8.5f ymin %8.5f zmin %8.5f  flag %d\n",
       pbox.xmin, pbox.ymin, pbox.zmin, (int)pbox.flag);
  fprintf(dbgfil(SURPAC),
  "sur924 pbox  xmax %8.5f ymax %8.5f zmax %8.5f  flag %d\n",
       pbox.xmax, pbox.ymax, pbox.zmax, (int)pbox.flag);
fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/*       Check for solution using the bounding box and/or cone.     */
/*       Call of varkon_pat_chebound (sur914).                      */
/*       Goto _nxtpat if no solution (sflag < 0)                    */
/*                                                                 !*/

   status=varkon_pat_chebound
           (p_sur,p_pat,p_comp,
            poirec.us,poirec.ue,poirec.vs,poirec.ve,
            &pbox,&pcone,&sflag);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");
        sprintf(errbuf,"sur914%%sur924");
        return(varkon_erpush("SU2943",errbuf));
        }

    if ( sflag == -99 )
       /* Surface patch is not OK */
       {
       goto  _nxtpat;
       }

    if ( sflag == -1 )
       {
       goto  _nxtpat;
       }

/*!                                                                 */
/*       Calculate the surface point with u,v as variables in a     */
/*       given patch.                                               */
/*       This is formulated as a non-linear optimization problem    */
/*       which is solved in varkon_sur_num2 (sur902).               */
/*                                                                 !*/

   status=varkon_sur_num2
    (p_sur,p_pat,p_comp,&poirec,&npts,xyz_a);
    if(status<0)
    {
/* if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924"); */
    sprintf(errbuf,"varkon_sur_num2%%sur924");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 Number of solutions from varkon_sur_num2 npts= %d addpts %d\n",
     (int)npts , (int)addpts ); 
fflush(dbgfil(SURPAC));
  }
#endif



/*!                                                                 */
/*     Add solution (data) to current SPOINT record                 */
/*     Call of addspoi.                                             */
/*                                                                 !*/

       status=addspoi (p_stable); 
       if (status<0) 
           {
           sprintf(errbuf,"addspoi%%(sur924)");
           return(varkon_erpush("SU2973",errbuf));
           }

/*!                                                                 */
/*     Goto _solex if solution exists (npts > 0)                    */
/*                                                                 !*/

       if ( npts == 0 )
         {
         }
       else
         {
/* for special case ??         goto  _solex;   */
         }

_nxtpat:;     /*! Label _nxtpat: No solution in patch              !*/

    }                                    /* End loop PBOUND         */

/*!                                                                 */
/*     End loop all records in PBOUND table i_pat=1,2,....,pbn      */
/*                                                                 !*/


_solex:;/* Label _solex: Solution exists for given UV pt            */

    }                                    /* End loop SPOINT         */

/*!                                                                 */
/*   End loop all records in SPOINT table i_poi=1,2,....,in_sn      */
/*                                                                 !*/

_nxtsur:; /*! Label: No solution in the surface                    !*/
  }                                      /* End loop psura          */

/*!                                                                 */
/* End loop all psura surfaces          i_sur=1,2,....,i_sur        */
/*                                                                 !*/

/* Note that ordering of the solution points must be done in the    */
/* calling routine. Elimination of equal solutions could be done    */
/* here ...                                                         */

/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Number of records in SPOINT to output variable                   */
   *p_sn   = in_sn + addpts;

/*                                                                 !*/

/*                                                                  */
/* Free memory for patch boundary table. Call of v3free. No ....    */
/*                                                                  */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && p_pbtable != NULL )
{
fprintf(dbgfil(SURPAC),
"sur924 PBOUND memory is freed \n");
fflush(dbgfil(SURPAC));
}
#endif

   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");

/*!                                                                 */
/* Printout of table SPOINT              For Debug ON               */
/* Call of pripts.                                                  */
/*                                                                 !*/

#ifdef DEBUG
status=pripts (p_stable);
    if (status<0) 
        {
        sprintf(errbuf,"pripts%%(sur924)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 Exit *** varkon_sur_ppts **No records in SPOINT*** = %d\n",
     (int)*p_sn   );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

#ifdef DEBUG
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to 1.23456789 and 123456789.           */

static short initial
         (psura,ppata,n_isur,p_comp)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *psura[ISMAX];/* All interrogation surfaces        (ptrs)*/
   DBPatch *ppata[ISMAX];/* Alloc. patch data areas to psura  (ptrs)*/
   DBint    n_isur;      /* Number of interrogation surfaces        */
   IRUNON  *p_comp;      /* Pointer to computation data             */

/* Out:                                                             */
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
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      (int)p_comp->ipl_un.ctype,
      (int)p_comp->ipl_un.method, 
      (int)p_comp->ipl_un.nstart, 
      (int)p_comp->ipl_un.maxiter);
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924 First surface psura %d  with patch area ptr ppata %d\n",
      (int)psura[0] , (int)ppata[0] ) ;      
  }

  if ( n_isur < 1 || n_isur > 100 ) /* 100 = unrealistic value      */
    {
    sprintf(errbuf,"n_isur not OK%%sur924*initial");
    return(varkon_erpush("SU2993",errbuf));
    }


/*!                                                                 */
/* 2. Initialize output variables and static (internal) variables   */
/*                                                                 !*/

    addpts = I_UNDEF;

    acc_add = I_UNDEF;   /* Number of patches to be added in sur910 */
    np_u = I_UNDEF;      /* Number of approx patches per patch in U */
    np_v = I_UNDEF;      /* Number of approx patches per patch in V */

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif


/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

#ifdef DEBUG
/*!********* Internal ** function ** pripts *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function prints out the calculated surface points.           */

    static short      pripts(p_stable)
/*                                                                  */
/******************************************************************!*/


/* ------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   SPOINT  *p_stable;    /* Pointer to surface point table  SPOINT  */
/* Out:                                                             */
/*----------------------------------------------------------------- */

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   SPOINT *p_pri;        /* Ptr to printout record in SPOINT        */
   DBint  i_rec;         /* Loop index record in SPOINT             */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of all records in SPOINT                             */
/*                                                                 !*/

for ( i_rec=1; i_rec<= in_sn+addpts; ++i_rec)
{
p_pri =p_stable + i_rec - 1;
if ( dbglev(SURPAC) == 1 )
{

fprintf(dbgfil(SURPAC),
"sur924*pripts extpt     %f %f %f  Record no %d\n",
p_pri->extpt.x_gm,p_pri->extpt.y_gm,p_pri->extpt.z_gm , (int)i_rec); 

fprintf(dbgfil(SURPAC),
"sur924*pripts extptnorm %f %f %f\n",
p_pri->extptnorm.x_gm,p_pri->extptnorm.y_gm,p_pri->extptnorm.z_gm ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts extuvpt   %f %f %f\n",
p_pri->extuvpt.x_gm,p_pri->extuvpt.y_gm,p_pri->extuvpt.z_gm ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts Pointers pisur %d pppat %d \n",
(int)p_pri->pisur,(int)p_pri->pppat ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts startuvpt %f %f %f\n",
p_pri->startuvpt.x_gm,p_pri->startuvpt.y_gm,p_pri->startuvpt.z_gm ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts us %6.2f ue %6.2f vs %6.2f ve %6.2f\n",
p_pri->us,p_pri->ue,p_pri->vs,p_pri->ve ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts proj      %f %f %f\n",
p_pri->proj.x_gm,p_pri->proj.y_gm,p_pri->proj.z_gm ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts Point spt %f %f %f\n",
p_pri->spt.r_x ,p_pri->spt.r_y ,p_pri->spt.r_z ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts UV pt spt %f %f\n",
p_pri->spt.u   ,p_pri->spt.v  ); 

fprintf(dbgfil(SURPAC),
"sur924*pripts pflag %d ptrs %d  \n",
(int)p_pri->pflag   ,(int)p_pri->ptrs ); 

} /* End dbglev */
} /* End loop   */



    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!********* Internal ** function ** uvgiven ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the closest points when a UV             */
/* start point is given.                                            */

    static short    uvgiven(p_comp)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   IRUNON  *p_comp;      /* Pointer to computation data             */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   iu;           /* Patch address for s_uvpt                */
   DBint   iv;           /*                                         */
/*-----------------------------------------------------------------!*/

   DBfloat   uglob,vglob;  /* Global patch parameter value for s_uvpt */
   DBfloat   u_pat,v_pat;  /* Local  patch parameter value for s_uvpt */

   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*uvgiven  startuvpt %8.4f %8.4f\n",
     poirec.startuvpt.x_gm ,  
     poirec.startuvpt.y_gm ); 
  }
#endif

/*!                                                                 */
/* 2. Define the UV search area when there is a start point         */
/* ========================================================         */
/*                                                                  */
/* Retrieve UV start point coordinates from SPOINT, record p_crs.   */
/*                                                                 !*/

   uglob =  poirec.startuvpt.x_gm;   
   vglob =  poirec.startuvpt.y_gm;   

/*!                                                                 */
/* Calculate patch address iu,iv.                                   */
/* Call of varkon_sur_patadr  (sur211).                             */
/*                                                                 !*/

status=varkon_sur_patadr
    (uglob,vglob,nu,nv,&iu,&iv,&u_pat,&v_pat);
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_patadr%%sur924");
    return(varkon_erpush("SU2943",errbuf));
    }

/*!                                                                 */
/* The UV search area to the current record in the SPOINT table     */
/* us= iu, vs= iv, ue= iu+1 and ve= iv+1 to record p_crs            */
/*                                                                 !*/

   poirec.us  = (DBfloat)iu;
   poirec.vs  = (DBfloat)iv;
   poirec.ue  = (DBfloat)iu+1.0;
   poirec.ve  = (DBfloat)iv+1.0;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur924*uvgiven uglob %8.4f vglob %8.4f u_pat %8.4f u_pat %8.4f\n",
     uglob ,  vglob ,  u_pat   ,  v_pat ); 
fprintf(dbgfil(SURPAC),
"sur924*uvgiven nu %d nv %d iu %d iv %d\n",
     (int)nu      ,  (int)nv      ,  (int)iu      ,  (int)iv    ); 
}
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*uvgiven UV search area (patch): %8.4f %8.4f %8.4f %8.4f\n",
     poirec.us   ,  
     poirec.vs   ,  
     poirec.ue   ,  
     poirec.ve   ); 
  }
#endif

/*!                                                                 */
/* 3. Try to find surface point(s) in the given patch               */
/* ==================================================               */
/*                                                                  */
/* Calculate the surface point with u,v as variables in a           */
/* given patch.                                                     */
/* This is formulated as a non-linear optimization problem          */
/* which is solved in varkon_sur_num2 (sur902) for a given patch.   */
/* Goto _sexist if solution(s) exist                                */
/*                                                                 !*/

status=varkon_sur_num2
    (p_sur,p_pat,p_comp,&poirec,&npts,xyz_a);
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_num2%%sur924");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*uvgiven Number of solutions from varkon_sur_num2 npts= %d\n",
     (int)npts ); 
  }
#endif

  if ( npts > 0 ) goto _sexist;

/*!                                                                 */
/* 4. Extend the UV search area if calculation failed               */
/* ==================================================               */
/*                                                                  */
/* If there are no surface points (solutions) in the given          */
/* patch will the UV search area be extendend. The size of          */
/* the search area is defined by parameter acc.                     */
/* UV point defined and acc=1: Only the patch of the input point    */
/* UV point defined and acc=2: Input patch and up to 8 additional   */
/*                                                                 !*/

if        ( acc == 1 &&  npts == 0 )
   {
   if ( iu-1 >=  1 ) s_lim[0][0] = (DBfloat)iu - 1.0;
   else              s_lim[0][0] = 1.0;
   if ( iv-1 >=  1 ) s_lim[0][1] = (DBfloat)iv - 1.0;
   else              s_lim[0][1] = 1.0;
   if ( iu+1 <= nu ) s_lim[1][0] = (DBfloat)iu + 1.0;
   else              s_lim[1][0] = (DBfloat)nu;
   if ( iv+1 <= nv ) s_lim[1][1] = (DBfloat)iv + 1.0;
   else              s_lim[1][1] = (DBfloat)nv;
   }
else if   ( acc == 2 &&  npts == 0 )
   {
   if ( iu-2 >=  1 ) s_lim[0][0] = (DBfloat)iu - 2.0;
   else              s_lim[0][0] = 1.0;
   if ( iv-2 >=  1 ) s_lim[0][1] = (DBfloat)iv - 2.0;
   else              s_lim[0][1] = 1.0;
   if ( iu+2 <= nu ) s_lim[1][0] = (DBfloat)iu + 2.0;
   else              s_lim[1][0] = (DBfloat)nu;
   if ( iv+2 <= nv ) s_lim[1][1] = (DBfloat)iv + 2.0;
   else              s_lim[1][1] = (DBfloat)nv;
   }
else if   ( acc == 3 &&  npts == 0 )
   {
   if ( iu-3 >=  1 ) s_lim[0][0] = (DBfloat)iu - 3.0;
   else              s_lim[0][0] = 1.0;
   if ( iv-3 >=  1 ) s_lim[0][1] = (DBfloat)iv - 3.0;
   else              s_lim[0][1] = 1.0;
   if ( iu+3 <= nu ) s_lim[1][0] = (DBfloat)iu + 3.0;
   else              s_lim[1][0] = (DBfloat)nu;
   if ( iv+3 <= nv ) s_lim[1][1] = (DBfloat)iv + 3.0;
   else              s_lim[1][1] = (DBfloat)nv;
   }
#ifdef DEBUG
else
   {
   sprintf(errbuf,"(acc value)%%varkon_sur_closestpt ");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

np_u = 0;
np_v = 0;
status=varkon_sur_defpatb
   (p_sur,p_pat,surtype,nu,nv,s_lim,ctype,
       acc_add,np_u,np_v,pb_alloc,p_pbtable,&pbn);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur924");
        sprintf(errbuf,"varkon_sur_defpatb%%varkon_sur_closestpt ");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  npts == 0 )
{
fprintf(dbgfil(SURPAC),
"sur924*uvgiven iu %d iv %d acc %d PBOUND records pbn %d\n",
           (int)iu, (int)iv, (int)acc , (int)pbn );
}
#endif

_sexist: /*! Label: Solution exists                                !*/

    return(SUCCED);

} /* End of function                                                */

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!********* Internal ** function ** addspoi ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adds a solution (surface) point in table SPOINT.    */
/* The number of records will be increased if there are multiple    */
/* solutions.                                                       */

   static short     addspoi(p_stable)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   SPOINT  *p_stable;    /* Pointer to surface point table  SPOINT  */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   SPOINT *p_add;        /* Ptr to added SPOINT record              */
   DBint   i_sol;        /* Loop index corresp. to solution         */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables   For Debug On                    */
/* =============================================                    */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && npts > 0 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi Number of solution npts %d poirec.pflag %d addpts %d p_crs %d\n",
     (int)npts, (int)poirec.pflag, (int)addpts,  (int)p_crs ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi UV search area (patch): %8.4f %8.4f %8.4f %8.4f\n",
     poirec.us   ,  
     poirec.vs   ,  
     poirec.ue   ,  
     poirec.ve   ); 
  }
#endif

/*!                                                                 */
/* 2. No solutions in current patch                                 */
/* ================================                                 */
/*                                                                  */
/* For no solutions do nothing in table SPOINT                      */
/*                                                                 !*/

   if ( npts == 0 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi There was no solution in current patch.  pflag= %d\n",
     (int)p_crs->pflag); 
  }
#endif
     }

/*!                                                                 */
/* 3. One (and first) solution in current patch                     */
/* ============================================                     */
/*                                                                  */
/* For the first solution update table SPOINT, record p_crs:        */
/*     i. Copy all data (actually us,ue,vs,ve) from poirec          */
/*    ii. Let point flag pflag=  1                                  */
/*   iii. Let pointer    ptrs = -1                                  */
/*    iv. Let point spt= xyz_a(0)                                   */
/*                                                                 !*/

   if ( npts == 1 && poirec.pflag < 0 )
     {
    *p_crs            =  poirec;
     p_crs->pflag     =  1;   
     p_crs->ptrs      = -1;   
     p_crs->spt       = xyz_a[0];
     poirec.pflag     =  1;      /* Added 1995-08-27 */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi One solution in current patch.  p_crs->pflag= %d poirec.pflag %d\n",
     (int)p_crs->pflag, (int)poirec.pflag ); 
  }
#endif
/* N}got {r fel f|r m}nga punkter skapas  goto  first;  Added 1995-08-27 */
     }

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 4. Multiple (and first) solutions in current patch               */
/* ===================================================              */
/*                                                                  */
/* For first solutions update and add records in table SPOINT:      */
/*     i. Copy all data (actually us,ue,vs,ve) from poirec          */
/*    ii. Let point flag pflag=  2              in record p_crs     */
/*   iii. Let pointer    ptrs = in_sn+addpts    in record p_crs     */
/*    iv. Let point spt= xyz_a(0)               in record p_crs     */
/*     v. Loop for solution i_sol=2,3,.., npts                      */
/*        a. Let number of added records addpts= addpts+1           */
/*        b. Record number p_add= pstable+in_sn-1+addpts            */
/*        c. Copy everything from poirec record to p_add record     */
/*        d. Let point flag pflag=  2                               */
/*        e. Let pointer ptrs= in_sn+addpts (-1 for last solution)  */
/*        f. Let point spt= xyz_a(i_sol-1)                          */
/*                                                                 !*/

   if ( npts >  1 &&  poirec.pflag < 0 )
     {
    *p_crs            =  poirec;
     p_crs->pflag     =  2;   
     p_crs->ptrs      = in_sn+addpts;
     p_crs->spt       = xyz_a[0];
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi Multiple solutions in current patch.  pflag= %d\n",
     (int)p_crs->pflag); 
  }
#endif
     for ( i_sol=2; i_sol<= npts; ++i_sol) /* Start loop solution   */
       {
       addpts= addpts + 1;
       p_add =p_stable + in_sn - 1 + addpts;
      *p_add            =  poirec;
       p_add->pflag     =  2;   

       if ( i_sol < npts )
       p_add->ptrs      =  in_sn+addpts;
       else
       p_add->ptrs      =  -1;

       p_add->spt       =  xyz_a[i_sol-1];

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi Solution addpts= %d Record number %d added SPOINT\n",
     (int)addpts , (int)in_sn+(int)addpts ); 
  }
#endif
       }
     }

/*!                                                                 */
/* 5. Additional (not first) solutions in current patch             */
/* =====================================================            */
/*                                                                  */
/* For additional solutions add records in table SPOINT:            */
/*     i. Loop for solution i_sol=1,2,3,.., npts                    */
/*        a. Let number of added records addpts= addpts+1           */
/*        b. Record number p_add= pstable+in_sn-1+addpts            */
/*        c. Copy everything from poirec record to p_add record     */
/*        d. Let point flag pflag=  2                               */
/*        e. Let pointer ptrs= in_sn+addpts (-1 for last solution)  */
/*        f. Let point spt= xyz_a(i_sol-1)                          */
/*  !!  Ingen l{nkning till tidigare punkter dvs pointer ptrs .. !! */
/*                                                                 !*/

   if ( npts >= 1 &&  poirec.pflag >= 0 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi Additional solutions in another patch.  pflag= %d\n",
     (int)poirec.pflag); 
  }
#endif
     for ( i_sol=1; i_sol<= npts; ++i_sol) /* Start loop solution   */
       {
       addpts= addpts + 1;
       p_add =p_stable + in_sn - 1 + addpts;
      *p_add            =  poirec;
       p_add->pflag     =  2;   

       if ( i_sol < npts )
       p_add->ptrs      =  in_sn+addpts;
       else
       p_add->ptrs      =  -1;

       p_add->spt       =  xyz_a[i_sol-1];

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur924*addspoi Solution addpts= %d Record number %d added SPOINT\n",
     (int)addpts , (int)in_sn+(int)addpts ); 
  }
#endif
       }
     }

    return(SUCCED);

} /* End of function                                                */
