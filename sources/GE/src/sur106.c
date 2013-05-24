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
/*  Function: varkon_sur_geodesic                  File: sur106.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  TODO Only started to program this function. Not finished        */
/*                                                                  */
/*  The function creates a geodesic curve on a surface.             */
/*                                                                  */
/*  The following types of geodesic curves can be calculated:       */
/*  Case 1: Geodesic defined by start point and vector              */
/*  Case 2:   ... not implemented .....................             */
/*                                                                  */
/*  The function allocates memory area for the UV curve segments.   */
/*  This area must be deallocated by the calling function, i.e      */
/*  deallocatiom must be made when output pointer NOT is NULL.      */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-03-08   Originally written                                 */
/*  1996-01-01   s_uv, s_tuv pointers                               */
/*  1998-03-21   pb_alloc and p_pbtable added                       */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_geodesic   Geodesic curve on a surface      */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol            * Coordinate tolerance                    */
/* varkon_geodesictol     * Geodesic zero criterion                 */
/* varkon_ini_pbound      * Initiate PBOUND                         */
/* varkon_ini_gmcur       * Initiate DBCurve                        */
/* varkon_ini_gmseg       * Initiate DBSeg                          */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/**v3mall                 * Allocate memory                         */
/* v3free                 * Free allocated memory                   */
/* varkon_sur_defpatb     * Define patch boundaries                 */
/* varkon_sum_mum3        * Differential equation                   */
/* varkon_sur_epts        * Calculate entry/exit points             */
/* varkon_sur_epts_mbs    * Create MBS code as a check              */
/* varkon_sur_epts_ord    * Order EPOINT table                      */
/* varkon_sur_curbr       * Create requested curve branch           */
/* varkon_erinit          * Initiate error message stack            */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2773 = No silhouette curve (solution)  varkon_sur_geodesic     */
/* SU2943 = Called function .. failed in varkon_sur_geodesic        */
/* SU2993 = Severe program error (  ) in varkon_sur_geodesic        */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_sur_geodesic (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBptr    pgm_sur,      /* Surface                        (DB ptr) */
  DBVector *p_s_uv,      /* Start point   for geodesic curve  (ptr) */
  DBVector *p_s_tuv,     /* Start tangent for geodesic curve  (ptr) */
  DBint    gtype,        /* Computation case                        */
                         /* Eq. 1: Start U,V point and tangent      */
                         /* Eq. 2: ...                              */
  DBint    acc,          /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of patches     */
                         /* Eq. 3: Triple the number of patches     */
                         /* Eq. 4:  ....                            */
  DBCurve *p_cur,        /* Curve                             (ptr) */
  DBSeg  **pp_seg )      /* Allocated area for UV segments    (ptr) */

/* Out:                                                             */
/*   Surface UV curve data to to p_cur and pp_seg                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, or .....         */
   GEODES  geodat;       /* Geodesic computation data               */
   DBfloat s_lim[2][2];  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBint    pb_alloc;     /* Allocated size of patch boundary table  */
  PBOUND  *p_pbtable;    /* Patch boundary   table PBOUND           */
  DBint    tnr;          /* Total number of records      in PBOUND  */


   DBint   cur_pat;      /* Current patch number in PBOUND          */
   DBint   n_in;         /* Input  number of curve segments         */
   DBint   n_out;        /* Output number of curve segments         */


/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   i_r;           /* Loop indec record                       */
  DBint   i_s;           /* Loop index segment record               */
  DBfloat geodesictol;   /* Geodesic curvature zero criterion       */
  DBfloat ctol;          /* Coordinate tolerance                    */
#ifdef DEBUG
  EPOINT  eprec;         /* Patch entry and exit point record       */
  DBfloat sumuv;         /* Sum of U,V tangent components           */
#endif
  DBSeg  *p_seg_l;       /* Local ptr =*pp_seg after allocation     */
  DBint   np_u;          /* Number of approx patches per patch in U */
  DBint   np_v;          /* Number of approx patches per patch in V */
  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!-------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_geodesic   !                          */
/*               !      (sur106)         !                          */
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
/*      !                   !           !     ! DBcreate!     !     */
/*      !                   !           !     !_segments!     !     */
/*      !                   !           !     !ini_gmseg!     !     */
/*      !                   !           !     !_________!     !     */
/*      !_________          !           !__________           !     */
/*  ____!___  ____!___  ____!____   ____!___   ____!___   ____!___  */
/* !        !!        !!         ! !        ! !        ! !        ! */
/* ! varkon !! varkon !! varkon  ! ! varkon ! ! varkon ! ! varkon ! */
/* ! _ge.tol!!  _sur  !!  _sur   ! !  _sur  ! !  _sur  ! !  _sur  ! */
/* ! _ctol  !!_nopatch!!_defpatb ! ! _epts  ! !  _epts ! ! _curbr ! */
/* ! sur755 !!        !!         ! !        ! !  _ord  ! !        ! */
/* ! sur751 !! sur230 !! sur910  ! ! sur920 ! ! sur922 ! ! sur930 ! */
/* !________!!________!!_________! !________! !________! !________! */
/*                                                                  */
/*-----------------------------------------------------------------!*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initialisations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 Enter ***** varkon_sur_geodesic   ******\n");
  fflush(dbgfil(SURPAC)); /* From buffer to file */
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 p_sur %d p_pat %d pgm_sur %d acc %d\n"
        ,p_sur, p_pat, pgm_sur, acc );
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),"sur106 D_GEODES: Start point     %f %f \n",
             p_s_uv->x_gm,p_s_uv->y_gm );
  fprintf(dbgfil(SURPAC),"sur106 D_GEODES: Start direction %f %f \n",
             p_s_tuv->x_gm,p_s_tuv->y_gm );
  fprintf(dbgfil(SURPAC),"sur106 gtype %d \n", gtype);
  fflush(dbgfil(SURPAC)); 
}
#endif
 
/*!                                                                 */
/* Surface normal tolerance and coordinate tolerance.               */
/* Call of varkon_geodesictol (sur755) and varkon_ctol (sur751).    */
/*                                                                 !*/

   geodesictol=varkon_geodesictol();
   ctol=varkon_ctol();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 geodesictol %25.10f ctol %25.10f \n",geodesictol, ctol );
  fflush(dbgfil(SURPAC));
  }
#endif
 
/*!                                                                 */
/* Let pp_seg= NULL                                                 */
/*                                                                 !*/
   *pp_seg = NULL;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur106 *pp_seg %d \n", *pp_seg );
  fflush(dbgfil(SURPAC)); 
  }
#endif
 
/* Initialize patch boundary table pointer and size                 */
   p_pbtable = NULL;
   pb_alloc  = I_UNDEF;

/*!                                                                 */
/* Initialize curve data in p_cur.                                  */
/* Call of varkon_ini_gmcur (sur778).                               */
/*                                                                 !*/
    varkon_ini_gmcur (p_cur);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 DBCurve initialized \n" );
  fflush(dbgfil(SURPAC));
  }
#endif
 
/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surface is of type                  */
/*               CUB_SUR, RAT_SUR, LFT_SUR, PRO_SUR or MIX_SUR      */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_geodesic  (sur106)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     == CUB_SUR ||           /* Check surface type      */
     surtype     == RAT_SUR ||           
     surtype     == LFT_SUR ||           
     surtype     == PRO_SUR ||           
     surtype     == MIX_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_geodesic (sur106");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur106 Surface type= %d  No patches in U nu= %d and V nv= %d\n",
        p_sur->typ_su, nu,nv);
fflush(dbgfil(SURPAC)); 
}

if ( dbglev(SURPAC) == 1 )
{
if      ( gtype == 1 )
  {
  fprintf(dbgfil(SURPAC),"sur106 D_GEODES: Start point     %f %f \n",
             p_s_uv->x_gm,p_s_uv->y_gm );
  fprintf(dbgfil(SURPAC),"sur106 D_GEODES: Start direction %f %f \n",
             p_s_tuv->x_gm,p_s_tuv->y_gm );
  fflush(dbgfil(SURPAC));
  }
else
  {
  fprintf(dbgfil(SURPAC),"sur106 Unknown case %d \n", gtype);
  fflush(dbgfil(SURPAC));
  }
}

if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur106 Geodesic zero criterion %20.15f Coordinate tolerance %f\n",
             geodesictol,ctol);
  fprintf(dbgfil(SURPAC),
  "sur106 Comput. gtype %d  accuracy %d \n",
                          gtype,acc);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Input data gtype, start point and tangent, end criteria to       */
/* structure variable geodat.                                       */
/* Selected method (PARAB3), number of restarts (3) and maximum     */
/* number of iterations (20) also to variable geodat.               */
/*                                                                 !*/


if (gtype == 1 )
    {
    geodat.ctype   = D_GEODES;
    geodat.uv_x    = p_s_uv->x_gm;      
    geodat.uv_y    = p_s_uv->y_gm;      
    geodat.tuv_x   = p_s_tuv->x_gm;      
    geodat.tuv_y   = p_s_tuv->y_gm;      
    }
 else
    {
    sprintf(errbuf,"%d %%varkon_sur_geodesic  ",(int)gtype);
    return(varkon_erpush("SU2993",errbuf));
    }

geodat.method  = PARAB3;
geodat.nstart  = 3;      
geodat.maxiter = 20;      

geodat.ctol    = ctol; 
geodat.gtol    = geodesictol; 

#ifdef DEBUG
if ( geodat.ctype == D_GEODES )
  {
  sumuv = SQRT(p_s_tuv->x_gm*p_s_tuv->x_gm +p_s_tuv->y_gm*p_s_tuv->y_gm);
  if ( sumuv < 0.01 )
        {
        sprintf(errbuf,"(p_s_tuv is zero)%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2993",errbuf));
        }

if ( p_s_uv->x_gm < 1.0 )
        {
        sprintf(errbuf,"(Start U < 1)%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2993",errbuf));
        }

if ( p_s_uv->y_gm < 1.0 )
        {
        sprintf(errbuf,"(Start V < 1)%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2993",errbuf));
        }

if ( p_s_uv->x_gm > nu+1.0  )
        {
        sprintf(errbuf,"(Start U > nu+1)%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2993",errbuf));
        }

if ( p_s_uv->y_gm > nv+1.0  )
        {
        sprintf(errbuf,"(Start V > nv+1)%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2993",errbuf));
        }


  }  /* End D_GEODES */

#endif  /* End check of input for Debug On  */

/*!                                                                 */
/* 2. Definition of rectangular grid of u,v curves                  */
/* _______________________________________________                  */
/*                                                                  */
/* Patch boundaries (the accuracy) to be used in the                */
/* geodesic curve calculation will be defined in                    */
/* function varkon_sur_defpatb (sur910).                            */
/*                                                                 !*/

if ( geodat.ctype == D_GEODES )
  {
  s_lim[0][0] = 1.0;
  s_lim[0][1] = 1.0;
  s_lim[1][0] = (DBfloat)nu;
  s_lim[1][1] = (DBfloat)nv;
  }
else
  {
  sprintf(errbuf,"(geodat.ctype)%%varkon_sur_geodesic  ");
  return(varkon_erpush("SU2993",errbuf));
  }

/*!                                                                 */
/*   Allocate memory area for the patch boundary table p_pbtable.   */
/*                                                                 !*/

/*  Number is defined .. ??                       TODO              */
    pb_alloc = PBMAX;

    p_pbtable = (PBOUND *)v3mall(pb_alloc*sizeof(PBOUND),"sur106");
    if  ( p_pbtable == NULL  ) 
      {
#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 PBOUND allocation failed  pb_alloc %d PBMAX %d\n",
    (short)pb_alloc, PBMAX );
  }
#endif
 sprintf(errbuf, "(PBOUND alloc)%%sur106");
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

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur106 Memory is allocated and initialized for %d PBOUND records\n",
        (short)pb_alloc );
fflush(dbgfil(SURPAC));
}
#endif


np_u = 0;
np_v = 0;
status=varkon_sur_defpatb
   (p_sur,p_pat,surtype,nu,nv,s_lim,geodat.ctype,
                 acc,np_u,np_v,pb_alloc,p_pbtable,&tnr);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur106");
        sprintf(errbuf,"varkon_sur_defpatb%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!New-Page--------------------------------------------------------!*/
/*!                                                                 */
/* 4. Allocate memory for the output UV curve                       */
/* __________________________________________                       */
/*                                                                 !*/

/*!                                                                 */
/* Memory for number of patches multiplied by 4 will be generated.  */
/* (normally much more than is needed)                              */
/* Call of function DBcreate_segments.                              */
/*                                                                 !*/

   if((*pp_seg = DBcreate_segments(tnr*4))==NULL)
    {
    if ( p_pbtable != NULL ) v3free(p_pbtable, "sur106");
    sprintf(errbuf, "(alloc)%%varkon_sur_geodesic  (sur106");
    return(varkon_erpush("SU2993",errbuf));
    }

/* Let locally defined pointer                                      */
   p_seg_l = *pp_seg;
/* In order to avoid problems with *'s ......                       */

/*!                                                                 */
/* Initiate segment data in pp_seg.                                 */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= tnr*4; i_s= i_s+1)
     {
     varkon_ini_gmseg ( *pp_seg+i_s-1);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur106 Memory has been allocated for tnr*4= %d UV curve segments\n",
                       tnr*4);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur106 Start allocated area *pp_seg %d = p_seg_l= %d \n",
                        *pp_seg, p_seg_l  );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 5. Create output curve                                           */
/* ______________________                                           */
/*                                                                 !*/


   cur_pat = 3;   /*  !!!!! Temporary    */
   n_in    = 0;
   n_out   = 0;

status=varkon_sur_num3    
   (p_sur,p_pat,pgm_sur,(IRUNON*)&geodat,p_pbtable,cur_pat, 
    p_cur,p_seg_l,n_in,&n_out);
   if (status<0) 
        {
        if ( p_pbtable != NULL ) v3free(p_pbtable, "sur106");
        sprintf(errbuf,"varkon_sur_num3%%varkon_sur_geodesic  ");
        return(varkon_erpush("SU2943",errbuf));
        }

p_cur->ns_cu = n_out; /* !!!!!!  Temporary    !!!!  */

/*!                                                                 */
/* 6. Exit                                                          */
/*                                                                  */
/* Curve header data to p_cur.                                      */
/*                                                                 !*/

   p_cur->hed_cu.type  = CURTYP;
   p_cur->plank_cu     = FALSE; 

/*!                                                                 */
/* Free memory for patch boundary table. Call of v3free.            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && p_pbtable != NULL )
{
fprintf(dbgfil(SURPAC),
"sur106 PBOUND memory is freed \n");
fflush(dbgfil(SURPAC));
}
#endif

   if ( p_pbtable != NULL ) v3free(p_pbtable, "sur106");

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 n_in %d n_out %d  \n", n_in , n_out );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur106 Exit*varkon_sur_geodesic  \n" );
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
