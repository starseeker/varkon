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
/*  Function: varkon_sur_ruled_e                   File: sur861.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a ruled surface (SUR_RULED), when input is two curves    */
/*  with only cubic or rational segments. Segments on a surface     */
/*  is not allowed and the number of segments must be equal in      */
/*  both curves.                                                    */
/*                                                                  */
/*  The input boundary curves will constant V isoparameter curves   */
/*  in the output surface.                                          */
/*                                                                  */
/*  The output ruled surface will either be a CUB_SUR or a RAT_SUR  */
/*  The input boundary curves will exactly be part of the output    */
/*  surface. There is no approximation (change) of the input curves */
/*                                                                  */
/*  It is possible to create a ruled surface with the boundary      */
/*  "exactly" in the surface even if the number of segments are     */
/*  different in the input curves. Extra curve segments can be      */
/*  added as divided, reparametrized segments. There will only      */
/*  be a minor, numerical approximation when the segments are       */
/*  reparametrized.                                                 */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-09-07   Originally written                                 */
/*  1997-04-20   sur768 among internal functions                    */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_ruled_e    SUR_RULED for equal no. of segm. */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_pat_ratcre2      * Create ruled rational patch            */
/* varkon_pat_biccre2      * Create ruled cubic    patch            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short surtype();       /* Determine the output surface type  */
static short suralloc();      /* Allocate memory for surface        */
static short surcrea ();      /* Create the output surface          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint  surface_type;  /* Type of output surface              */
                             /* Eq. 1: CUB_SUR                      */
                             /* Eq. 2: RAT_SUR                      */
static DBint  n_ulines;      /* Number of U lines in ruled surface  */
static DBPatch *p_frst;      /* Pointer to the first patch          */
static DBfloat  comptol;     /* Computer tolerance                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_ruled_e         */
/* SU2973 = Internal function () failed in varkon_sur_ruled_e       */
/* SU2993 = Severe program error (  ) in varkon_sur_ruled_e         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

   DBstatus           varkon_sur_ruled_e (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1,      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1,      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2,      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2,      /* Segment data for p_cur2           (ptr) */
   DBint    r_e_case,    /* Ruled (exact) surface case              */
                         /*   ... not yet used ....                 */
                         /*   Must be negative (= I_UNDEF)          */
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

   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861 Enter***varkon_sur_ruled_e r_e_case %d\n", r_e_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861 p_cur1 %d p_seg1 %d\n", p_cur1,  p_seg1 );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861 p_cur2 %d p_seg2 %d\n", p_cur2,  p_seg2 );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial( r_e_case, p_surout );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_ruled_e (sur861)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Determine the output surface type                             */
/*                                                                 !*/

    status= surtype ( p_cur1, p_seg1, p_cur2, p_seg2 );
    if ( status < 0 ) return(status);

/*!                                                                 */
/* 3. Allocate memory for the output surface                        */
/*                                                                 !*/

    status= suralloc ( p_surout, pp_patout );
    if (status<0) 
      {
      sprintf(errbuf,"suralloc%%varkon_sur_ruled_e (sur861)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 4. Create the output surface                                     */
/*                                                                 !*/

    status= surcrea  ( p_seg1, p_seg2 );
    if (status<0) 
      {
      sprintf(errbuf,"surcrea%%varkon_sur_ruled_e (sur861)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%sur861");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 6. Exit                                                          */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861 Exit pp_patout %d\n", *pp_patout );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial( r_e_case, p_surout )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    r_e_case;    /* Ruled (exact) surface case              */
   DBSurf  *p_surout;    /* Output surface                    (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur861*initial* Enter ** \n");
  }
#endif

   comptol   = varkon_comptol();

  if ( r_e_case > 0 )
     {
     sprintf(errbuf,"r_e_case%%sur861");
     return(varkon_erpush("SU2993",errbuf));
    }

  surface_type = I_UNDEF;  /* Type of output surface              */
  n_ulines     = I_UNDEF;  /* Number of U lines in ruled surface  */
  p_frst       = NULL;     /* Pointer to the first patch          */

  p_surout->typ_su = I_UNDEF;
  p_surout->nu_su  = I_UNDEF; 
  p_surout->nv_su  = I_UNDEF;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861*initial comptol %25.15f\n", comptol  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




/*!********* Internal ** function ** surtype ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function determines the output surface type                  */

   static short surtype ( p_cur1, p_seg1, p_cur2, p_seg2 )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBCurve *p_cur2;      /* Boundary curve 2                  (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  DBSeg *p_seg;          /* Current curve segment             (ptr) */
  DBint  i_seg;          /* Loop index segment in curve             */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur861*surtype* Enter ** \n");
  }
#endif

/*!                                                                 */
/* Number of segments in input curves determines the number of      */
/* U lines in the output surface                                    */
/*                                                                 !*/

  if ( p_cur1->ns_cu  ==  p_cur2->ns_cu  )
    {
    n_ulines  = (DBint)p_cur1->ns_cu + (DBint)1;
    }
  else
    {
    varkon_erinit();
    sprintf(errbuf,"%d%%%d",p_cur1->ns_cu,  p_cur2->ns_cu);
    return(varkon_erpush("SU8103",errbuf));
    }

/*!                                                                 */
/* Determine the surface type and check that there are no surface   */
/* curve segments (UV_SEG) in the input curve segments              */
/*                                                                 !*/

  surface_type = 1; /* Initialize to CUB_SUR surface                */

  for ( i_seg = 0; i_seg <  p_cur1->ns_cu; ++i_seg ) 
    {
    p_seg = p_seg1 + i_seg;
    if ( p_seg->typ != CUB_SEG )
      {
      varkon_erinit();
      sprintf(errbuf,"first%%CUB_SEG");
      return(varkon_erpush("SU8113",errbuf));
      }

      /* Denominator coefficients non-zero for a rational segment   */
      if ( fabs(p_seg->c1 ) > comptol ||
           fabs(p_seg->c2 ) > comptol ||
           fabs(p_seg->c3 ) > comptol    )
        surface_type = 2; /* Change to RAT_SUR surface              */

    }   /* End loop i_seg */


  for ( i_seg = 0; i_seg <  p_cur2->ns_cu; ++i_seg ) 
    {
    p_seg = p_seg2 + i_seg;
    if ( p_seg->typ != CUB_SEG )
      {
      varkon_erinit();
      sprintf(errbuf,"second%%CUB_SEG");
      return(varkon_erpush("SU8113",errbuf));
      }

      /* Denominator coefficients non-zero for a rational segment   */
      if ( fabs(p_seg->c1 ) > comptol ||
           fabs(p_seg->c2 ) > comptol ||
           fabs(p_seg->c3 ) > comptol    )
        surface_type = 2; /* Change to RAT_SUR surface              */

    }   /* End loop i_seg */




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && surface_type == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861*surtype surface_type %d (CUB_SUR) n_ulines %d\n",
                surface_type,n_ulines );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && surface_type == 2 )
{
fprintf(dbgfil(SURPAC),
"sur861*surtype surface_type %d (RAT_SUR) n_ulines %d\n",
                surface_type,n_ulines );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** suralloc ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function allocates memory for the output surface             */

   static short suralloc( p_surout, pp_patout )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_surout;    /* Output surface                    (ptr) */
   DBPatch **pp_patout;  /* Alloc. area for topol. patch data (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   DBint  i_s;           /* Loop index surface patch record         */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur861*suralloc Enter ** \n");
  }
#endif


/*!                                                                 */
/* Surface type and number of patches in U and V to p_surout        */
/*                                                                 !*/

  if  (  surface_type == 1 ) p_surout->typ_su = CUB_SUR;
  if  (  surface_type == 2 ) p_surout->typ_su = RAT_SUR;
  p_surout->nu_su = (short)(n_ulines-1);
  p_surout->nv_su = (short)1; 

/*!                                                                 */
/* Dynamic allocation of area (pp_patout) for the topological       */
/* patches (patches of type TOP_PAT).                               */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/
if ((p_tpat=DBcreate_patches(TOP_PAT,n_ulines-1))==NULL)
 {                                
 sprintf(errbuf, "(alloc)%%sur861*suralloc");
 return(varkon_erpush("SU2993",errbuf));
 }                                 

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_patout.                              */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= n_ulines-1;i_s= i_s+1) 
     {
    varkon_ini_gmpat (p_frst  +i_s-1);
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861*suralloc Memory allocated for %d TOP_PAT patches\n",
                    n_ulines-1 );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861*suralloc p_surout->typ_su %d  ->nu_su %d  ->nv_su %d\n",
     p_surout->typ_su, p_surout->nu_su,p_surout->nv_su);
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** surcrea  ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function allocates memory for the output surface             */

   static short surcrea ( p_seg1, p_seg2 )

/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBSeg   *p_seg2;      /* Segment data for p_cur2           (ptr) */
/*          p_frst          Pointer to the first patch              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    patch_type;  /* Type of geometric patch                 */
   DBSeg   *p_curseg1;   /* Current curve 1 segment           (ptr) */
   DBSeg   *p_curseg2;   /* Current curve 2 segment           (ptr) */
   DBint    i_seg;       /* Loop index segment in curve             */
   DBPatch *p_t;         /* Current topological patch         (ptr) */
   GMPATC  *p_cubic;     /* Current geometric cubic     patch (ptr) */
   GMPATR  *p_rational;  /* Current geometric rational  patch (ptr) */
   char    *p_gpat;      /* Allocated area geom.  patch data  (ptr) */
/*----------------------------------------------------------------- */
  DBint     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur861*surcrea  Enter ** \n");
  }
#endif

   p_cubic     = NULL;
   p_rational  = NULL;
   
   patch_type = I_UNDEF;
   if  (  surface_type == 1 ) patch_type = CUB_PAT;
   if  (  surface_type == 2 ) patch_type = RAT_PAT;

/*!                                                                 */
/* Start loop patches in the output surface i_seg= 1, . ,n_ulines-1 */
/*                                                                 !*/

   for ( i_seg = 0; i_seg < n_ulines-1;  ++i_seg ) 
     {

/*!                                                                 */
/*   Current pointers p_curseg1, p_curseg2 and p_t.                 */
/*                                                                 !*/

     p_curseg1 = p_seg1 + i_seg; /* Current curve 1 segment         */
     p_curseg2 = p_seg2 + i_seg; /* Current curve 2 segment         */
     p_t       = p_frst + i_seg; /* Current topological patch       */

/*!                                                                 */
/*   Dynamic allocation of area for one geometric patch.            */
/*   Call of function DBcreate_patches.                             */
/*                                                                 !*/

     if ((p_gpat=DBcreate_patches(patch_type,1)) ==NULL)
       {                                   
       sprintf(errbuf,"(allocg)%%sur861*surcrea"); 
       return(varkon_erpush("SU2993",errbuf));
       }                                 

/*   Current geometric patch p_cubic or p_rational                 */
     if  (  patch_type == 1 ) p_cubic     = (GMPATC *)p_gpat; 
     if  (  patch_type == 2 ) p_rational  = (GMPATR *)p_gpat; 

/*!                                                                 */
/*   Topological patch data to current patch p_t                    */
/*                                                                 !*/

     p_t->styp_pat = (short)patch_type;  /* Type of secondary patch */
     p_t->spek_c   = p_gpat;             /* Secondary patch (C ptr) */
     p_t->su_pat   = 0;                  /* Topological adress      */
     p_t->sv_pat   = 0;                  /* secondary patch not def.*/
     p_t->iu_pat   = (short)(i_seg + 1); /* Topological adress for  */
     p_t->iv_pat   = (short)(    1    ); /* current (this) patch    */
     p_t->us_pat   = (DBfloat)i_seg+1.0; /* Start U on geom. patch  */
     p_t->ue_pat   = (DBfloat)i_seg+2.0  /* End   U on geom. patch  */
                               -comptol; /*                         */
     p_t->vs_pat   = 1.0;                /* Start V on geom. patch  */
     p_t->ve_pat   = 2.0-comptol;        /* End   V on geom. patch  */

     p_t->box_pat.xmin = 1.0;            /* BBOX  initiation        */
     p_t->box_pat.ymin = 2.0;            /*                         */
     p_t->box_pat.zmin = 3.0;            /*                         */
     p_t->box_pat.xmax = 4.0;            /*                         */
     p_t->box_pat.ymax = 5.0;            /*                         */
     p_t->box_pat.zmax = 6.0;            /*                         */
     p_t->box_pat.flag = -1;             /* Not calculated          */
    
     p_t->cone_pat.xdir = 1.0;           /* BCONE initiation        */
     p_t->cone_pat.ydir = 2.0;           /*                         */
     p_t->cone_pat.zdir = 3.0;           /*                         */
     p_t->cone_pat.ang  = 4.0;           /*                         */
     p_t->cone_pat.flag = -1;            /* Not calculated          */


     if      (  patch_type == CUB_PAT )
       {
       status = varkon_pat_biccre2
       ( p_curseg1,p_curseg2,p_cubic);
       if (status<0) 
         {
         sprintf(errbuf,"sur256%%sur861*surcrea");
         return(varkon_erpush("SU2943",errbuf));
         }
       }      /* End CUB_PAT */

     if      (  patch_type == RAT_PAT )
       {
       status = varkon_pat_ratcre2
       ( p_curseg1,p_curseg2,p_rational);
       if (status<0) 
         {
         sprintf(errbuf,"sur252%%sur861*surcrea");
         return(varkon_erpush("SU2943",errbuf));
         }
       }      /* End RAT_PAT */



     }  /* End loop i_seg */
/*!                                                                 */
/* End   loop patches in the output surface i_seg= 1, . ,n_ulines-1 */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && surface_type == 1 )
{
fprintf(dbgfil(SURPAC),
"sur861*surcrea  Surface created with %d CUB_PAT patches\n",
                   n_ulines-1 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && surface_type == 2 )
{
fprintf(dbgfil(SURPAC),
"sur861*surcrea  Surface created with %d RAT_PAT patches\n",
                   n_ulines-1 );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

