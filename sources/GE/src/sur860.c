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
/*  Function: varkon_sur_ruled                     File: sur860.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a ruled surface (SUR_RULED)                              */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-09-06   Originally written                                 */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1997-04-20   sur863 added                                       */
/*  1997-04-29   reverse added                                      */
/*  1997-05-11   r_case numbering                                   */
/*  1998-10-10   Developable surface added                          */
/*  1999-12-03   Free source code modifications                     */
/*  2001-04-06   Bug: Did not return with error forn err1           */
/*  2007-01-08   Call to varkon_ini_gmsur , Sören L                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_ruled      Create a ruled surface SUR_RULED */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ruled_e          * Ruled, exact surface                   */
/* varkon_ruled_l          * Ruled, lofting surface                 */
/* varkon_ruled_d          * Ruled, developable surface             */
/* GE817                   * Trim a curve                           */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_sur_creloft      * Create a LFT_SUR                       */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short curmod();        /* Reverse curve directions           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBCurve *p_cur1_mod;  /* Boundary curve 1  modified    (ptr) */
static DBSeg   *p_seg1_mod;  /* Segment data for p_cur2 modif (ptr) */
static DBCurve *p_cur2_mod;  /* Boundary curve 1  modified    (ptr) */
static DBSeg   *p_seg2_mod;  /* Segment data for p_cur2 modif (ptr) */
static DBCurve  tricur1;     /* Reversed boundary curve 1           */
static DBCurve  tricur2;     /* Reversed boundary curve 1           */
static DBSeg   *p_triseg1;   /* Segment data trimmed crv 1    (ptr) */
static DBSeg   *p_triseg2;   /* Segment data trimmed crv 2    (ptr) */
static DBint    r_e_case;    /* Case for varkon_ruled_e (sur861)    */
static DBint    r_l_case;    /* Case for varkon_ruled_l (sur863)    */
static DBint    r_d_case;    /* Case for varkon_ruled_d (sur865)    */
static DBfloat  idpoint;     /* Identical point criterion           */
static DBfloat  idangle;     /* Identical angle criterion           */
static DBfloat  comptol;     /* Computer tolerance                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_ruled           */
/* SU2973 = Internal function () failed in varkon_sur_ruled         */
/* SU2993 = Severe program error (  ) in varkon_sur_ruled           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_ruled (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1,      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1,      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2,      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2,      /* Segment data for p_cur2           (ptr) */
   DBint    r_case,      /* Ruled surface case                      */
                         /* Eq. 1: Arclength distribution of rulings*/
                         /*        Output surface type LFT_SUR      */
                         /* Eq. 2: Use input curve segments         */
                         /*        Output surface type RAT_SUR or   */
                         /*        output surface type CUB_SUR      */
                         /* Eq. 3: As 1 but add curve segments      */
                         /*        Output surface type RAT_SUR or   */
                         /*        output surface type CUB_SUR      */
                         /* Eq. 4: Developable (approximation)      */
                         /*        Output surface type LFT_SUR      */
                         /* Eq. 5: Input curves and a spine         */
                         /*        Output surface type LFT_SUR      */
   DBCurve *p_spine,     /* Spine curve                       (ptr) */
   DBSeg   *p_spineseg,  /* Segments for the spine curve      (ptr) */
   DBint    s_case,      /* Reverse case                            */
                         /* Eq.  1: Input curve 1 is V= 0           */
                         /*         Input curve 2 is V= 1           */
                         /*         No reverse of input curves      */
                         /* Eq. -1: Input curve 1 is V= 0           */
                         /*         Input curve 2 is V= 1           */
                         /*         Reverse of input curves         */
                         /* Eq.  2: Input curve 1 is V= 1           */
                         /*         Input curve 2 is V= 0           */
                         /*         No reverse of input curves      */
                         /* Eq.  3: Input curve 1 is V= 1           */
                         /*         Input curve 2 is V= 0           */
                         /*         Reverse of input curves         */
   DBfloat  idpoint_in,  /* Point tolerance for approximation       */
   DBfloat  idangle_in,  /* Angle tolerance for approximation       */
   DBSurf  *p_surout,    /* Output surface                    (ptr) */
   DBPatch **pp_patout ) /* Alloc. area for topol. patch data (ptr) */
                         /* (pp_patout= NULL <==> not allocated)    */


/* Out:                                                             */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


  DBint    status;       /* Error code from called function         */
  DBstatus stat_err1;    /* Error code for the err1 return value    */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

/*!                                                                 */
/* Initialize surface data in sur.                                  */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur ( p_surout ); 

/*!                                                                 */
/* Initialize surface data in sur.                                  */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur ( p_surout ); 


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 Enter***varkon_sur_ruled s_case %d r_case %d\n", 
          (int)s_case, (int)r_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 p_cur1 %d p_seg1 %d\n", (int)p_cur1,  (int)p_seg1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 p_cur2 %d p_seg2 %d\n", (int)p_cur2,  (int)p_seg2 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 p_spine %d p_spineseg %d\n", (int)p_spine,  (int)p_spineseg );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 r_case %d idpoint_in %f idangle_in %f\n", 
        (int)r_case,   idpoint_in,   idangle_in);
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;
   stat_err1  = 0;

    status= initial(  );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_ruled (sur860)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Reverse curve directions if necessary                         */
/*                                                                 !*/

    status= curmod ( p_cur1, p_seg1, p_cur2, p_seg2, s_case );
    if (status<0) 
      {
      sprintf(errbuf,"curmod%%sur860");
      stat_err1 = varkon_erpush("SU2973",errbuf);
      goto err1;
      }



/*!                                                                 */
/* 3. Switch to the right ruled surface creation routine            */
/*                                                                 !*/


  if            ( r_case == 2 )
    {
    r_e_case = I_UNDEF; 
    status = varkon_sur_ruled_e
    ( p_cur1_mod,p_seg1_mod,p_cur2_mod,p_seg2_mod,r_e_case,
       p_surout,pp_patout);
    if (status<0) 
      {
      sprintf(errbuf,"sur861%%sur860");
      stat_err1 = varkon_erpush("SU2943",errbuf);
      goto err1;
      }
    }       /* r_case == 2 */

  else if       ( r_case == 1 )
    {
    r_l_case = I_UNDEF; 
    status = varkon_sur_ruled_l
    ( p_cur1_mod,p_seg1_mod,p_cur2_mod,p_seg2_mod,r_l_case,
       p_surout,pp_patout);
    if (status<0) 
      {
      sprintf(errbuf,"sur863%%sur860");
      stat_err1 = varkon_erpush("SU2943",errbuf);
      goto err1;
      }
    }       /* r_case == 1 */

  else if       ( r_case == 4 )
    {
    r_d_case = I_UNDEF; 
    status = varkon_sur_ruled_d
    ( p_cur1_mod,p_seg1_mod,p_cur2_mod,p_seg2_mod,r_d_case,
       p_surout,pp_patout);
    if (status<0) 
      {
      sprintf(errbuf,"sur865%%sur860");
      stat_err1 = varkon_erpush("SU2943",errbuf);
      goto err1;
      }
    }       /* r_case == 4 */

  else
    {
    varkon_erinit();
    sprintf(errbuf,"%d%%",(int)r_case);
    stat_err1 = varkon_erpush("SU8123",errbuf);
    goto err1;
    }



/*!                                                                 */
/* 9. Exit                                                          */
/*                                                                 !*/

err1:;

    if ( p_triseg1 != NULL ) DBfree_segments(p_triseg1);
    if ( p_triseg2 != NULL ) DBfree_segments(p_triseg2);



#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860 *pp_patout %d\n", (int)*pp_patout );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && p_surout->typ_su == CUB_SUR )
{
fprintf(dbgfil(SURPAC),
"sur860 Exit Number  of patches in U %d in V %d in the CUB_SUR surface\n", 
         (int)p_surout->nu_su ,  (int)p_surout->nv_su  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && p_surout->typ_su == RAT_SUR )
{
fprintf(dbgfil(SURPAC),
"sur860 Exit Number of patches in U %d in V %d in the RAT_SUR surface\n", 
          (int)p_surout->nu_su ,  (int)p_surout->nv_su  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && p_surout->typ_su == LFT_SUR )
{
fprintf(dbgfil(SURPAC),
"sur860 Exit Number of patches in U %d in V %d in the LFT_SUR surface\n", 
          p_surout->nu_su ,  p_surout->nv_su  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

   if ( 0 != stat_err1 )
      return(stat_err1);
   else
      return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial( )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur860*initial* Enter ** \n");
  }
#endif

 r_e_case = I_UNDEF;        /* Case for varkon_ruled_e (sur861)    */
 r_l_case = I_UNDEF;        /* Case for varkon_ruled_l (sur863)    */
 r_d_case = I_UNDEF;        /* Case for varkon_ruled_d (sur865)    */

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();

  p_cur1_mod = NULL;    /* Boundary curve 1  modified    (ptr) */
  p_seg1_mod = NULL;    /* Segment data for p_cur2 modif (ptr) */
  p_cur2_mod = NULL;    /* Boundary curve 1  modified    (ptr) */
  p_seg2_mod = NULL;    /* Segment data for p_cur2 modif (ptr) */
  p_triseg1  = NULL; 
  p_triseg2  = NULL; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860*initial idpoint %10.6f idangle %6.4f comptol %12.10f\n", 
idpoint, idangle, comptol  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** curmod  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function modifies curves with respect to case s_case         */

   static short curmod ( p_cur1, p_seg1, p_cur2, p_seg2, s_case  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
   DBint    s_case;      /* Reverse case                            */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  uglobs1;      /* Global u start value, curve 1           */
  DBfloat  uglobe1;      /* Global u end   value, curve 1           */
  DBfloat  uglobs2;      /* Global u start value, curve 2           */
  DBfloat  uglobe2;      /* Global u end   value, curve 2           */
/*----------------------------------------------------------------- */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860*curmod*Enter \n" );
}
#endif

  if (  s_case == -1 || s_case == 3 )
    {
    p_triseg1 = DBcreate_segments(p_cur1->ns_cu);
    uglobs1   = (DBfloat)p_cur1->ns_cu+1.0;
    uglobe1   =          1.0;
   status=GE817
    ((DBAny*)p_cur1, p_seg1,&tricur1,p_triseg1,uglobs1,uglobe1);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur860*curmod GE817 failed uglobs1 %f uglobe1 %f \n",
                         uglobs1,uglobe1);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(varkon_erpush("SU2943",errbuf));
        }
    p_triseg2 = DBcreate_segments(p_cur2->ns_cu);
    uglobs2   = (DBfloat)p_cur2->ns_cu+1.0;
    uglobe2   =          1.0;
   status=GE817
    ((DBAny*)p_cur2, p_seg2,&tricur2,p_triseg2,uglobs2,uglobe2);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur860*curmod GE817 failed uglobs2 %f uglobe2 %f \n",
                         uglobs2,uglobe2);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(varkon_erpush("SU2943",errbuf));
        }
    }



  if      (  s_case == 1 )
    {
    p_cur1_mod = p_cur1;  
    p_seg1_mod = p_seg1; 
    p_cur2_mod = p_cur2;  
    p_seg2_mod = p_seg2; 
    }
  else if (  s_case == 2 )
    {
    p_cur1_mod = p_cur2;  
    p_seg1_mod = p_seg2; 
    p_cur2_mod = p_cur1;  
    p_seg2_mod = p_seg1; 
    }
  else if (  s_case == -1 )
    {
    p_cur1_mod = &tricur1;  
    p_seg1_mod = p_triseg1; 
    p_cur2_mod = &tricur2;  
    p_seg2_mod = p_triseg2; 
    }
  else if (  s_case == 3 )
    {
    p_cur1_mod = &tricur2;  
    p_seg1_mod = p_triseg2; 
    p_cur2_mod = &tricur1;  
    p_seg2_mod = p_triseg1; 
    }

  else
    {
    varkon_erinit();
    sprintf(errbuf,"%d%%sur860*curmod", (int)s_case);
    return(varkon_erpush("SU8213",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur860*curmod*Exit \n" );
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/






