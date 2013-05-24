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
/*  Function: varkon_sur_selpatseg                 File: sur919.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function select curve segments and surface patches which    */
/*  may intersect. The output is a (PBOUND) table with records      */
/*  (patches) for the surface and an array with curve segments (?)  */
/*                                                                  */
/*  The output table also defines the iso-parametric curves, which  */
/*  shall be the "patch boundaries" for the surface/curve           */
/*  interrogation (intersect) function. The output UV net defines   */
/*  the accuracy in the calculation search phases. The density of   */
/*  the UV curve net may for instance determine how many of the     */
/*  curve branches that will be found in a surface/surface          */
/*  intersect calculation.                                          */
/*                                                                  */
/*  Note that the output is an "irregular" net of UV rectangles     */
/*                                                                  */
/*                                                                  */
/*           Regular UV net                   Irregular UV net      */
/*        ___________________                  ________________     */
/*       !         !         !                !    !     !     !    */
/*       !         !         !                !    !     !     !    */
/*       !    7    !    8    !                ! 14 ! 15  ! 16  !    */
/*       !_________!_________!              __!____!_____!_____!    */
/*       !         !         !             !  10   !_12__!     !    */
/*       !    5    !    6    !             !_______! 11  ! 13  !    */
/*       !_________!_________!              _______!_____!_____!    */
/*       !         !         !             !    !  !     !          */
/*       !         !         !             !    !  ! 8   !_____     */
/*       !    3    !    4    !             !    !  !_____!     !    */
/*       !         !         !             ! 5  !6 ! 7   !  9  !    */
/*       !_________!_________!             !____!__!_____!_____!    */
/*       !         !         !             !  !    !     !     !    */
/*       !    1    !    2    !             !1 ! 2  ! 3   !  4  !    */
/*       !_________!_________!             !__!____!_____!_____!    */
/*                                                                  */
/*                                                                  */
/*  Memory allocated by this function for the PBOUND table must     */
/*  be deallocated by the calling function!                         */
/*                                                                  */
/*                                                                  */
/*!New-Page--------------------------------------------------------!*/
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-28   Originally written                                 */
/*  1996-05-28   Deleted p_cseg                                     */
/*  1998-02-09   Mapping of UV values for NURBS (sur365) added      */
/*  1998-03-20   Allocation of memory for PBOUND table              */
/*  1999-11-28   Free source code modifications                     */
/*  2000-09-26   Error message changed                              */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_selpatseg  Select segment intersect patches */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short addrecs();       /* Divide patch, add PBOUND records   */
/*1998-02-09 static short sur2pat();*//* Add surface 2 patch address*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static BBOX    box;               /* Patch box from surface         */
static DBint   numrec;            /* Record number  in PBOUND table */
static DBfloat comptol;           /* Computer tolerance (accuracy)  */
/*-----------------------------------------------------------------!*/




/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_uvnmap     * Map UV net for NURBS                     */
/* varkon_ini_pbound     * Initialize PBOUND                        */
/* GE109                 * Curve evaluation function                */
/* v3mall                * Allocate memory                          */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_selpatseg */
/* SU2993 = Severe program error ( ) in varkon_sur_selpatsegsur919  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus      varkon_sur_selpatseg (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf   *p_sur,       /* Surface                           (ptr) */
  DBPatch  *p_pat,       /* Alloc. area for topol. patch data (ptr) */
  DBint    s_type,       /* Type CUB_SUR, RAT_SUR, or .....         */
  DBint    n_u,          /* Number of patches in U direction        */
  DBint    n_v,          /* Number of patches in V direction        */
  DBfloat  s_lim[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBCurve *p_cur,        /* Curve                             (ptr) */
  DBSeg   *p_seg,        /* Curve segment data                (ptr) */
  IRUNON  *p_comp,       /* Computation data                  (ptr) */
  PBOUND **pp_pbtable,   /* Patch boundary table              (ptr) */
  DBint   *p_nr )        /* Number of records in pbtable            */

/* Out:                                                             */
/*        Table with "patches"                                      */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   PBOUND *p_pbtable;    /* Pointer to patch boundary table         */
   DBint   pb_alloc;     /* Allocated size of patch boundary table  */
   DBint   i_u;          /* Loop index U line in surface            */
   DBint   i_v;          /* Loop index V line in surface            */
   DBint   i_seg;        /* Loop index segment in curve             */
   DBint   i_p;          /* Loop index point in segment             */
                         /* Density of the UV net:                  */
   DBint   u_add ;       /* For surface 1 in U direction            */
   DBint   v_add ;       /* For surface 1 in V direction            */
                         /* (Values 1, 2, 3, 4, .... )              */
/*-----------------------------------------------------------------!*/

   DBfloat  u_par;       /* Global U parameter for curve            */
   DBfloat  d_che;       /* Maximum check distance                  */
   DBfloat  idpoint;     /* Identical point criterion               */
   DBfloat  u_delta;     /* Delta parameter value                   */
   DBfloat  l_seg;       /* Length of curve segment                 */
   DBint    n_p;         /* Number of check points                  */
   DBfloat  curv_xmin;   /* Minimum X value for line segment        */
   DBfloat  curv_xmax;   /* Minimum X value for line segment        */
   DBfloat  curv_ymin;   /* Minimum Y value for line segment        */
   DBfloat  curv_ymax;   /* Minimum Y value for line segment        */
   DBfloat  curv_zmin;   /* Minimum Z value for line segment        */
   DBfloat  curv_zmax;   /* Minimum Z value for line segment        */
   EVALC    xyz;         /* Coordinates and derivatives for crv pt  */
   DBVector poi_cur;     /* Current  curve point (line end   point) */
   DBVector poi_pre;     /* Previous curve point (line start point) */
   DBint    s_iu ;       /* Start loop value in   U direction       */
   DBint    s_iv ;       /* Start loop value in   V direction       */
   DBint    e_iu ;       /* End   loop value in   U direction       */
   DBint    e_iv ;       /* End   loop value in   V direction       */
   DBint    f_in;        /* = -1: Outside box  = 1: Inside box      */
   DBint    sur_no;      /* Surface number 1 or 2                   */
   DBint    ctype;       /* Type of computation SURCUR, ..          */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   DBint    i_r;         /* Loop index PBOUND record                */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_selpatseg !                           */
/*               !     (sur919)         !                           */
/*               !______________________!                           */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*  ! Initiate  ! ! Data to the PBOUND table ! !  Exit  !           */
/*  !___________! !__________________________! !________!           */
/*  Debug ! On                 !                                    */
/*   _____!_____               !                                    */
/*  ! initial   !              !                                    */
/*  !___________!              !                                    */
/*       ______________________!_____________                       */
/*  ____!_____   __________!__________    ___!___                   */
/* !          ! !                   * !  !       !                  */
/* ! Initiate ! ! U direction patches !  !varkon_!                  */
/* !   loop   ! !_____________________!  ! normv !                  */
/* ! variabl. !  __________!__________   !_______!                  */
/* !__________! !                   * !                             */
/*              ! V direction patches !                             */
/*              !_____________________!                             */
/*       __________________!                                        */
/*  ____!____    __________!__________                              */
/* !         !  !                   * !                             */
/* ! addsur2 !  ! Within one patch    !                             */
/* !_________!  !_____________________!                             */
/*                     ____!___                                     */
/*                    !        !                                    */
/*                    ! addrec !                                    */
/*                    !________!                                   !*/
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur919 Enter varkon_sur_selpatseg : Select intersecting curve/BBOX patches \n");
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Initialize output pointer to PBOUND table                        */
/*                                                                 !*/

   *pp_pbtable = NULL;

/* Local pointer */
   p_pbtable  = NULL;

   u_add    = I_UNDEF;
   v_add    = I_UNDEF;
   f_in     = I_UNDEF;

/*!                                                                 */
/* Retrieve computation data p_comp                                 */
/*                                                                 !*/

   ctype   = p_comp->icu_un.ctype;
   if  (  ctype == SURCUR )
     {
     comptol  = p_comp->icu_un.comptol;
     u_add    = p_comp->icu_un.u_add ;
     v_add    = p_comp->icu_un.v_add ;
     d_che    = p_comp->icu_un.d_che ;
     idpoint  = p_comp->icu_un.idpoint;
     }
#ifdef  DEBUG
   else
     {
     sprintf(errbuf, "(ctype)%%varkon_sur_selpatseg"); 
      return(varkon_erpush("SU2993",errbuf));
     }
#endif  /* End DEBUG  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur919 u_add  %d v_add  %d d_che %f comptol %15.10g\n",
 (int)u_add ,(int)v_add , d_che,  comptol );
}
#endif
 
#ifdef DEBUG
  if ( fabs(d_che) < 2.0*idpoint )
     {
     sprintf(errbuf, "(d_che)%%varkon_sur_selpatseg"); 
      return(varkon_erpush("SU2993",errbuf));
     }
#endif

  *p_nr = I_UNDEF;      /* Number of records in pbtable            */
  numrec = 0;  

  poi_pre.x_gm  =  0.0;
  poi_pre.y_gm  =  0.0;
  poi_pre.z_gm  =  0.0;

/*!                                                                 */
/* 2. Data to PBOUND table pbtable                                  */
/* _______________________________                                  */
/*                                                                  */
/*                                                                 !*/

/* Initiate loop variables:                                         */



   s_iu   = (DBint)s_lim [0][0] - 1; /* Start loop value U          */
   s_iv   = (DBint)s_lim [0][1] - 1; /* Start loop value V          */
   e_iu   = (DBint)s_lim [1][0] - 1; /* End   loop value U          */
   e_iv   = (DBint)s_lim [1][1] - 1; /* End   loop value V          */

   if ( s_iu  < 0 || s_iv  < 0 || e_iu  > n_u || e_iv  > n_v )
      {
      sprintf(errbuf,                    /* Error SU2993 for invalid*/
      "(s_iu ...)%%varkon_sur_selpatseg"); /* type                    */
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/*   Allocate memory area for the patch boundary table pp_pbtable.  */
/*                                                                 !*/

/*  Start value which can change if there are more patches TODO ..  */
    pb_alloc = n_u*n_v;
    if ( pb_alloc < 1000 ) pb_alloc = 1000;

    *pp_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur919");
    if  ( *pp_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur919 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
 sprintf(errbuf, "(PBOUND alloc)%%sur919");
 return(varkon_erpush("SU2993",errbuf));
      }

   p_pbtable = *pp_pbtable;

/*!                                                                 */
/* Initialize table PBOUND                                          */
/* Calls of varkon_ini_pbound (sur774).                             */
/*                                                                 !*/

   for (i_r=1; i_r <= pb_alloc; i_r= i_r+1)
     { 
     varkon_ini_pbound (p_pbtable+i_r-1);
     } 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur919 Memory is allocated and initialized for %d PBOUND records\n",
        (short)pb_alloc );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Loop all V patches  i_v= s_iu , ...., e_iu                       */
/*  Loop all U patches  i_u= s_iv , ..., e_iv                       */
/*                                                                 !*/

for ( i_u=s_iu ; i_u<= e_iu ; ++i_u )    /* Start loop U patches    */
  {
  for ( i_v=s_iv ; i_v<= e_iv ; ++i_v )  /* Start loop V patches    */
    {


/*!                                                                 */
/*   Pointer to current topological patch                           */
     p_t = p_pat  + i_u*n_v + i_v; 
/*   Retrieve BBOX for the current patch in surface 1.              */
/*                                                                 !*/

     box  =  p_t->box_pat;              /* Copy BBOX  to box       */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur914 box  Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box.xmin,   box.ymin,   box.zmin,
         box.xmax,   box.ymax,   box.zmax );
}
#endif

/*!                                                                 */
/*   Loop all segments   i_s= 1, 2,  ...., n_seg                    */
/*    Loop points         i_p= 1, 2, 3, 4                           */
/*                                                                 !*/

/*!                                                                 */
/*  Start loop all curve segments i_seg= 1,2,..., p_cur->ns_cu      */
/*                                                                 !*/

    for ( i_seg = 1 ; i_seg <= p_cur->ns_cu ; i_seg = i_seg + 1 )
      {

      l_seg = p_seg->sl;
      n_p   = (DBint)(l_seg/d_che);
      if  (  n_p <  2  ) n_p =  3;
      if  (  n_p > 25  ) n_p = 25;
      u_delta = 1.0/(DBfloat)(n_p-1);

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur919 i_seg %d n_p %d l_seg %f u_delta %g\n",
 (int)i_seg,  (int)n_p ,l_seg , u_delta );
}
#endif

      for ( i_p = 1 ; i_p  <= n_p; i_p = i_p + 1 )
        {

        if ( i_p > 1 ) poi_pre = poi_cur;  /* Previous crv point */

        u_par  = (DBfloat)i_seg + (DBfloat)(i_p-1)*u_delta;


/*!                                                                 */
/*      Calculate point on the curve                                */
/*      Call of varkon_GE109 (GE109).                             */
/*                                                                 !*/

        xyz.evltyp   = EVC_R;
        xyz.t_global = u_par; 

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_selpatseg (sur919)");
        return(varkon_erpush("SU2943",errbuf));
        }

          poi_cur = xyz.r;
          if ( i_p == 1 ) goto first;  /* First point  */



          f_in = -1;

          if  ( poi_cur.x_gm  >  poi_pre.x_gm )
            {
            curv_xmin = poi_pre.x_gm;
            curv_xmax = poi_cur.x_gm;
            }
          else
            {
            curv_xmax = poi_pre.x_gm;
            curv_xmin = poi_cur.x_gm;
            }
          if  ( poi_cur.y_gm  >  poi_pre.y_gm )
            {
            curv_ymin = poi_pre.y_gm;
            curv_ymax = poi_cur.y_gm;
            }
          else
            {
            curv_ymax = poi_pre.y_gm;
            curv_ymin = poi_cur.y_gm;
            }
          if  ( poi_cur.z_gm  >  poi_pre.z_gm )
            {
            curv_zmin = poi_pre.z_gm;
            curv_zmax = poi_cur.z_gm;
            }
          else
            {
            curv_zmax = poi_pre.z_gm;
            curv_zmin = poi_cur.z_gm;
            }

          /*   X  values                                             */
          if        ( box.xmin >  curv_xmin &&
                      box.xmin >  curv_xmax &&
                      box.xmax >  curv_xmin &&
                      box.xmax >  curv_xmax    )  goto nointers;
          if        ( box.xmin <  curv_xmin &&
                      box.xmin <  curv_xmax &&
                      box.xmax <  curv_xmin &&
                      box.xmax <  curv_xmax    )  goto nointers;
          /*   Y  values                                             */
          if        ( box.ymin >  curv_ymin &&
                      box.ymin >  curv_ymax &&
                      box.ymax >  curv_ymin &&
                      box.ymax >  curv_ymax    )  goto nointers;
          if        ( box.ymin <  curv_ymin &&
                      box.ymin <  curv_ymax &&
                      box.ymax <  curv_ymin &&
                      box.ymax <  curv_ymax    )  goto nointers;
          /*   Z  values                                             */
          if        ( box.zmin >  curv_zmin &&
                      box.zmin >  curv_zmax &&
                      box.zmax >  curv_zmin &&
                      box.zmax >  curv_zmax    )  goto nointers;
          if        ( box.zmin <  curv_zmin &&
                      box.zmin <  curv_zmax &&
                      box.zmax <  curv_zmin &&
                      box.zmax <  curv_zmax    )  goto nointers;

          f_in =  1;  /* There is a possible intersect with the patch  */
          goto  inside;


nointers:;


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 && f_in == -1 )
{
  fprintf(dbgfil(SURPAC),
  "sur919 i_u= %d i_v= %d i_p %d f_in %d u_par %f\n", 
          (int)i_u,    (int)i_v,  (int)i_p,  (int)f_in, u_par );
}
#endif

first:;

       } /* End loop point in curve segment  */
     } /* End loop curve segment  */
/*!                                                                 */
/*     End  point in curve segment i_p= 1, 2, 3, ...........        */
/*   End  curve segments i_seg= 1, 2 , 3, .........                 */
/*   Add records in PBOUND table if f_in = 1                        */
/*                                                                 !*/

inside:; /* Curve is inside the current box iu,iv                   */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && f_in ==  1 )
{
  fprintf(dbgfil(SURPAC),
  "sur919 i_u= %d i_v= %d i_p %d f_in %d u_par %f\n", 
          (int)i_u,    (int)i_v,  (int)i_p,  (int)f_in, u_par );
}
#endif

     if  (  f_in == 1 )
       {
       sur_no = 1;
       status=addrecs (pb_alloc, p_pbtable,sur_no,u_add ,v_add ,i_u,i_v);
       if ( -1 == status )
        {
        sprintf(errbuf,"alloc additional PBOUND's%%sur919");
        return(varkon_erpush("SU2973",errbuf));
       }

       if (status<0) 
        {
        sprintf(errbuf,"addrecs%%sur919");
        return(varkon_erpush("SU2973",errbuf));
        }
       }

    }                           /* End   loop U patches surface 2   */
  }                             /* End   loop V patches surface 2   */
/*!                                                                 */
/*  End  all U patches  i_u= s_iu , ..., e_iu                       */
/* End  all V patches  i_v= s_iv , ..., e_iv                        */
/*                                                                 !*/

/*!                                                                 */
/* 3. Map UV net (for a NURBS surface)                              */
/* ___________________________________                              */
/*                                                                  */
/*                                                                 !*/

   status= varkon_sur_uvnmap
   (p_sur, p_pat, p_pbtable, numrec );
    if (status<0) 
      {
      sprintf(errbuf,"sur365%%sur919");
      return(varkon_erpush("SU2943",errbuf));
      }


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Total number of records to output variable                       */
   *p_nr = numrec;
/*                                                                 !*/

#ifdef DEBUG
      if ( *p_nr == 0 ) 
        {
        sprintf(errbuf,"*p_nr=0%%sur919");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur919 s_iu = %d s_iv = %d e_iu = %d e_iv = %d\n",
   (int)s_iu ,(int)s_iv ,(int)e_iu ,(int)e_iv );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur919 Exit *** varkon_sur_selpatseg *pcone !*No of PBOUND records %d\n"
     , (int)*p_nr );
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/



/*!********* Internal ** function ** addrecs ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Divide found intersection patch in smaller, output parts.        */

   static short addrecs (pb_alloc,p_pbtable,sur_no,np_u,np_v,iu,iv)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint    pb_alloc;     /* Allocated size of patch boundary table  */
  PBOUND  *p_pbtable;    /* Patch boundary table              (ptr) */
  DBint    sur_no;       /* Surface number 1 or 2                   */
  DBint    np_u;         /* No of addition. patches per patch in U  */
  DBint    np_v;         /* No of addition. patches per patch in V  */
  DBint    iu;           /* Current patch IU                        */
  DBint    iv;           /* Current patch IV                        */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat us_p;         /* Start U value for the patch             */
   DBfloat vs_p;         /* Start V value for the patch             */
   DBfloat ue_p;         /* End   U value for the patch             */
   DBfloat ve_p;         /* End   V value for the patch             */
   DBfloat u_delta;      /* Delta U value                           */
   DBfloat v_delta;      /* Delta V value                           */
   DBint   iu_d;         /* Loop index for the U patch splitting    */
   DBint   iv_d;         /* Loop index for the V patch splitting    */
   DBint   nu_d;         /* Loop end for iu_d                       */
   DBint   nv_d;         /* Loop end for iv_d                       */
   PBOUND *p_cr;         /* Current record in PBOUND          (ptr) */
#ifdef  DEBUG
   char    errbuf[80];   /* String for error message fctn erpush    */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiate and check                                            */
/* ==========================                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( np_u <= 1 || np_u > 30 ||
     np_v <= 1 || np_v > 30    )
   {
   sprintf(errbuf,"addrecs %d or %d > 30%%varkon_sur_selpatseg",
                    (int)np_u , (int)np_v  );
   return(varkon_erpush("SU2993",errbuf));
   }
#endif

nu_d    = np_u - 1;                      /* End loop iu_d           */
nv_d    = np_v - 1;                      /* End loop iv_d           */

  us_p    =(DBfloat)iu+1.0+comptol;        /* U start point           */
  ue_p    =(DBfloat)iu+2.0-comptol;        /* U end   point           */
  u_delta =(ue_p - us_p)/(DBfloat)np_u;    /* Delta U                 */
  for ( iu_d= 0; iu_d <= nu_d; ++iu_d )  /* Start loop extra U lines*/
    {
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point           */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point           */
      v_delta =(ve_p - vs_p)/(DBfloat)np_v;/* Delta V                 */

      for ( iv_d= 0; iv_d <= nv_d; ++iv_d )
        {
        numrec = numrec + 1;             
        if ( numrec >= pb_alloc ) return -1;
        p_cr= p_pbtable + numrec - 1;     /* Ptr to current record   */
        p_cr->rec_no= numrec;             /* Record number           */
        p_cr->us=                         /* U start point           */
               us_p+ iu_d   *u_delta;
        p_cr->vs=                         /* V start point           */
               vs_p+ iv_d   *v_delta; 
        p_cr->ue=                         /* U end   point           */
               us_p+(iu_d+1)*u_delta;
        p_cr->ve=                         /* V end   point           */
               vs_p+(iv_d+1)*v_delta; 
        p_cr->sur_no = sur_no;            /* Surface number 1 or 2   */

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur919*addrecs %f %f %f %f\n",
  p_cr->us,p_cr->vs,p_cr->ue,p_cr->ve );
fflush(dbgfil(SURPAC)); 
}
#endif
       }
   } /* End loop            */


    return(SUCCED);

} /* End of function                                                */


/*********************************************************/
