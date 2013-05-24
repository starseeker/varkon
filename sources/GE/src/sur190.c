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
/*  Function: varkon_sur_aface                     File: sur190.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Creation of an approximate face, i.e. a part of a surface       */
/*  limited by a UV boundary curve.                                 */
/*                                                                  */
/*  Input to the function is on composite curve. All segments       */
/*  must be of UV type, i.e. they must be surface curves.           */
/*                                                                  */
/*  The surfaces are defined by the segments (DBSeg member spek_gm) */
/*  Areas will be allocated and deallocated for surfaces used in    */
/*  the computation.                                                */
/*                                                                  */
/*  The function allocates memory for the output surface. Memory    */
/*  that must be deallocated by the calling function!               */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-06-09   Originally written                                 */
/*  1997-02-09   Elimination of compilation warning                 */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_aface      Create an approximate face       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/**v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_pat_coonscre4    * Create   Coons surface                 */
/* varkon_sur_coonseval    * Evaluate Coons surface                 */
/* varkon_sur_splarr       * Create bicubic surface                 */
/* varkon_sur_aface_l      * Line intersects for face               */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_errmes           * Warning message to terminal            */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_aface           */
/* SU2993 = Severe program error (  ) in varkon_sur_aface           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_aface (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Composite boundary UV curve       (ptr) */
  DBSeg   *p_seg,        /* Segment data for p_cur            (ptr) */

  DBint    s_typ,        /* Output surface type:                    */
                         /* Eq. 1: Approximate to CUB_PAT patches   */
                         /* Eq. 2: Approximate to RAT_PAT patches   */
                         /* Eq. 3: Approximate to LFT_PAT patches   */
                         /* Eq. 4: Approximate to BPL_PAT patches   */
                         /* Eq. 5: Approximate to CON_PAT patches   */
  DBint    method,       /* Method for approximation:               */
                         /* Eq. 1: Arclength Coons with given       */
                         /*        number of output U and V lines   */
  DBint    a_case,       /* Case for interrogation of surface:      */
                         /* Eq. 1: Use Coons (no interrogation)     */
                         /* Eq. 2: Line,   error   for no intersect */
                         /* Eq. 3: Line,   warning for no intersect */
                         /* Eq. 4: Line,   use Coons for failure    */
                         /* Eq. 5: Closest error   for no intersect */
                         /* Eq. 6: Closest warning for no intersect */
                         /* Eq. 7: Closest use Coons for failure    */
  DBint    acc,          /* Computation accuracy                    */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
  DBint    n_ulin,       /* Number of U lines in output surface     */
  DBint    n_vlin,       /* Number of V lines in output surface     */
  DBfloat    c_crit,       /* Criterion for a face boundary corner    */
  DBfloat    ctol,         /* Coordinate          tolerance           */
  DBfloat    ntol,         /* Surface normal      tolerance           */
  DBfloat    rtol,         /* Radius of curvature tolerance           */

  DBSurf  *p_surout,     /* Output surface                    (ptr) */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
                         /* (pp_patout= NULL <==> not allocated)    */

/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    n_ulin_m;     /* No of U lines in output surface, modif. */
  DBint    n_vlin_m;     /* No of V lines in output surface, modif. */


  GMPATB   coon_sur;     /* Coons surface                           */

  DBint    icase;        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
  DBfloat  s_sur;        /* Relative arclength in S (U) direction   */
  DBfloat  t_sur;        /* Relative arclength in T (V) direction   */
  EVALS    xyz_s;        /* Coordinates and derivatives             */

  APPROX   appdat;        /* Approximation computation data          */

  DBint    n_sur;         /* Number of surfaces                     */
  DBint    i_seg;         /* Loop index segment in boundary curve   */
  DBint    i_sur;         /* Loop index surface                     */
  DBptr    all_gm[SFMAX]; /* All surfaces                  (DB ptr) */
  DBSurf   all_sur[SFMAX];/* All surfaces                           */
  DBSurf  *p_all_sur[SFMAX];/* All surfaces                         */
  DBPatch *all_pat[SFMAX];/* Allocated areas for patches            */

  char     metod_s[132]; /* Method SUR_SPLARR:                      */
                         /* ZEROTWIST_1: Zero twist                 */
                         /* ADINI_1:     Adini twist calculation    */
  DBint    np_u;         /* Number of points in U direction         */
  DBint    np_v;         /* Number of points in V direction         */
  DBVector *p_poi;       /* Points                            (ptr) */
  DBVector *p_utan;      /* Tangents in U direction           (ptr) */
  DBVector *p_vtan;      /* Tangents in V direction           (ptr) */
  DBVector *p_twist;     /* Twist vectors                     (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint    i_ptan;       /* Current number of points and tangents   */
  DBint    i_ulin;       /* Loop index U line                       */
  DBint    i_vlin;       /* Loop index V line                       */
  DBfloat   u_delta;     /* Delta U value                           */
  DBfloat   v_delta;     /* Delta V value                           */
  DBVector  poi;         /* Point                                   */
  DBVector  utan;        /* U tangent                               */
  DBVector  vtan;        /* V tangent                               */
  DBfloat   v_leng;      /* Length of a vector                      */
  DBSurf    current_sur; /* Current surface                         */
  DBPatch  *current_pat; /* Current allocated patches area    (ptr) */

  EVALS    xyz_s_out;    /* Coordinates and derivatives             */
  DBint    n_int;        /* Number of intersect points              */

  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

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
"sur190 Enter***varkon_sur_aface s_typ %d a_case %d\n" , 
         (int)s_typ,  (int)a_case );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur190 n_ulin %d n_vlin %d acc %d method %d c_crit %f \n"
 , (int)n_ulin, (int)n_vlin, (int)acc, (int)method, c_crit  );
fflush(dbgfil(SURPAC));
}
#endif
 
   status = SUCCED;                      /* For deallocation exit   */

   *pp_patout = NULL;                    /* Initialize patch ptr    */

/*!                                                                 */
/* Initialize surface data in p_surout.                             */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/

    varkon_ini_gmsur   (p_surout);   

/* Initialize pointers to allocated areas for patches               */
/* Also DBSurf should be initialized .. at least for debug on ...   */

for ( i_sur = 1 ; i_sur  <= SFMAX; ++i_sur  ) 
  {
  all_pat[i_sur-1]   = NULL;        /* Allocated areas for patches  */
  p_all_sur[i_sur-1] = NULL;
  }

/*!                                                                 */
/* Check method                                                     */
/*                                                                 !*/

  if (   method == 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf, " %d %%sur190",(int)method);
    varkon_erinit();
    return(varkon_erpush("SU5023",errbuf));
    }

/*!                                                                 */
/* Modify input number of U and V lines ...                         */
/*                                                                 !*/

  n_ulin_m = n_ulin;  
  if  ( n_vlin <= 0 )
    {
    n_vlin_m = n_ulin; 
    }
  else
    {
    n_vlin_m = n_vlin; 
    }
  
  if  (  n_ulin_m < 2  ||  n_vlin_m < 2 )
    {
    varkon_erinit();
    sprintf(errbuf," %d%% %d",(int)n_ulin_m,(int)n_vlin_m );
    return(varkon_erpush("SU5013",errbuf));
    }


  np_u    = n_ulin_m;      /* No of points in U direction           */
  np_v    = n_vlin_m;      /* No of points in V direction           */
  p_poi   = NULL;          /* Points                          (ptr) */
  p_utan  = NULL;          /* Tangents in U direction         (ptr) */
  p_vtan  = NULL;          /* Tangents in V direction         (ptr) */
  p_twist = NULL;          /* Twist vectors                   (ptr) */

  v_leng     = F_UNDEF;
  n_int      = I_UNDEF;

/*!                                                                 */
/* Get data for surfaces for all cases except a_case = 1            */
/* Check that at least one surface is defined.                      */
/*                                                                 !*/

  n_sur = 0;
  if ( a_case == 1 ) goto nosurf;

  for ( i_seg = 1 ; i_seg  <= p_cur->ns_cu; ++i_seg  ) 
    {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur190 i_seg %d typ %d spek_gm %d n_sur %d\n" , 
(int)i_seg, (int)(p_seg-1+i_seg)->typ, 
 (int)(p_seg-1+i_seg)->spek_gm, (int)n_sur );
fflush(dbgfil(SURPAC)); 
}
#endif

    if ( (p_seg-1+i_seg)->typ  == UV_SEG )
      {
      for ( i_sur = 1 ; i_sur  <= n_sur ; ++i_sur  ) 
        {
        if  (   (p_seg-1+i_seg)->spek_gm == all_gm[i_sur-1]  )
          {
          goto nxtseg;
          }
        }                    /* End loop i_sur        */
      all_gm[n_sur] = (p_seg-1+i_seg)->spek_gm;
      n_sur = n_sur + 1;     /* An additional surface */
      if ( n_sur > SFMAX )
        {
        sprintf(errbuf, "n_sur>SFMAX %%sur190");  
        return(varkon_erpush("SU2993",errbuf));
        }
      }                      /* UV_SEG                */
nxtseg:;
    }                        /* End loop i_seg        */

  if ( n_sur == 0 )
    {
    sprintf(errbuf, "%d%%sur190",(int)a_case);
    varkon_erinit();
    return(varkon_erpush("SU5053",errbuf));
    }


nosurf:; /* Label: No surface for a_case =1 */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur190 Number of surfaces n_sur %d\n" , (int)n_sur );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 2. Data for the calculation to APPROX                            */
/* _____________________________________                            */
/*                                                                  */
/* Approximation (computation) data to appdat:                      */
/* Tolerances ctol, ntol and rtol.                                  */
/*                                                                 !*/

   appdat.ctol     = ctol;
   appdat.ntol     = ntol;
   appdat.rtol     = rtol;

/*!                                                                 */
/* Number of restarts in the patch nstart =   4                     */
/* Maximum number of iterations    maxiter=  50                     */
/* Approximation case acase and accuracy acc to appdat.             */
/*                                                                 !*/

   appdat.ctype    = SURAPPR;
   appdat.method   = I_UNDEF;  
   appdat.nstart   =  4;    
   appdat.maxiter  = 50;   
   appdat.acase    = I_UNDEF;
   appdat.acc      = acc;
   appdat.offset   = F_UNDEF;     
   appdat.thick    = F_UNDEF;     

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur190 APPROX: ctype %d method %d nstart %d\n",
(int)appdat.ctype,(int)appdat.method,(int)appdat.nstart);

fprintf(dbgfil(SURPAC),
"sur190 APPROX: maxiter %d acase %d acc %d ctol %f ntol %f\n",
(int)appdat.maxiter,(int)appdat.acase ,(int)appdat.acc, 
      appdat.ctol, appdat.ntol  );
}
#endif

/*!                                                                 */
/* 3. Allocate memory areas for point/vector arrays and surfaces    */
/* _____________________________________________________________    */
/*                                                                 !*/

if((p_poi= (DBVector*)
     v3mall((unsigned)(np_u*np_v*sizeof(DBVector)),"sur190"))==NULL) 
 {                          
#ifdef DEBUG               
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
      "sur190 Allocation p_poi failed  np_u %d np_v %d\n", 
                                  (int)np_u, (int)np_v );
  }
#endif
 sprintf(errbuf,"(alloc p_poi)%%sur190");  
 return(varkon_erpush("SU2993",errbuf));
 }                                 
if((p_utan= (DBVector *)
      v3mall((unsigned)(np_u*np_v*sizeof(DBVector)),"sur190"))==NULL) 
 {                          
#ifdef DEBUG               
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
    "sur190 Allocation p_utan  failed  np_u %d np_v %d\n", 
                                  (int)np_u, (int)np_v );
  }
#endif
 sprintf(errbuf, "(alloc p_utan)%%sur190");  
 return(varkon_erpush("SU2993",errbuf));
 }                                 
if((p_vtan= (DBVector *)
    v3mall((unsigned)(np_u*np_v*sizeof(DBVector)),"sur190"))==NULL) 
 {                          
#ifdef DEBUG               
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
       "sur190 Allocation p_vtan  failed  np_u %d np_v %d\n", 
                                  (int)np_u, (int)np_v );
  }
#endif
 sprintf(errbuf, "(alloc p_vtan)%%sur190");  
 return(varkon_erpush("SU2993",errbuf));
 }                                 

for ( i_sur = 1 ; i_sur  <= n_sur ; ++i_sur  ) 
  {
  status = DBread_surface(&current_sur, all_gm[i_sur-1]);
  if ( status < 0 )
    {
    sprintf(errbuf, "DBread_surface%%sur190");  
    return(varkon_erpush("SU2943",errbuf));
    }                                 
  V3MOME((char *)( &current_sur),
           (char *)(&all_sur[i_sur-1]),sizeof(DBSurf));
  p_all_sur[i_sur-1] = &all_sur[i_sur-1];

  status=DBread_patches(&current_sur,&current_pat);
  if ( status < 0 )
    {
    sprintf(errbuf, "DBread_patches%%sur190");  
    return(varkon_erpush("SU2943",errbuf));
    }                                 

  all_pat[i_sur-1] = current_pat;   /* Allocated areas for patches  */

  }  /* End loop all surfaces i_sur= 1,2..,n_sur */


#ifdef DEBUG               
if ( dbglev(SURPAC) == 1 )
{
for ( i_sur = 1 ; i_sur  <= n_sur ; ++i_sur  ) 
{
fprintf(dbgfil(SURPAC), 
   "sur190 all_sur[%d].nu_su %d .nv_su %d all_pat[%d] %d\n", 
(int)i_sur-1,(int)all_sur[i_sur-1].nu_su, (int)all_sur[i_sur-1].nv_su, 
(int)i_sur-1,(int)all_pat[i_sur-1] );
fprintf(dbgfil(SURPAC), "sur190 p_all_sur[%d] %d \n", 
(int)i_sur-1,(int)p_all_sur[i_sur-1] );
}
}
#endif

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 4. Create Coon's patch                                           */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/

    status = varkon_pat_coonscre4
    (p_cur, p_seg, c_crit, &coon_sur );
    if ( status < 0 )
      {
      sprintf(errbuf,"sur273%%varkon_sur_aface (sur190");
      varkon_erpush("SU2943",errbuf);
      goto dealloc;
      }                            

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur190 Coons patch created\n");
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 5. Calculate data for all patches.                               */
/* __________________________________                               */
/*                                                                 !*/

     icase = 2;                         /* Coordinates and tangents */

     u_delta = 1.0/(DBfloat)(np_u-1);
     v_delta = 1.0/(DBfloat)(np_v-1);

     i_ptan = 0;

/*!                                                                 */
/* Loop all V lines    i_vlin = 1, 2, .... , np_v                   */
/*  Loop all U lines     i_ulin = 1, 2, .... , np_u                 */
/*                                                                 !*/

/* .. too tired to understand .. trial and error for loop param's   */

for ( i_vlin= 1 ; i_vlin <= np_u; ++i_vlin )   /* Start loop V line */
  {
  for ( i_ulin = 1; i_ulin <= np_v; ++i_ulin ) /* Start loop U line */
    {
    s_sur = (DBfloat)(i_ulin-1)*v_delta;
    t_sur = (DBfloat)(i_vlin-1)*u_delta;

/*!                                                                 */
/*    Evaluation in Coons patch.                                    */
/*                                                                 !*/

    status = varkon_sur_coonseval
    (&coon_sur, icase, s_sur, t_sur, &xyz_s );
    if ( status < 0 )
      {
      sprintf(errbuf,"sur245%%varkon_sur_aface (sur190");
      varkon_erpush("SU2943",errbuf);
      goto dealloc;
      }                            

     if ( a_case == 1 ) goto nointer;

/*!                                                                 */
/*    Intersect line and surfaces and project tangents.             */
/*    Call of varkon_sur_aface_l (sur191).                          */
/*                                                                 !*/

    status = varkon_sur_aface_l   
    ( &xyz_s, n_sur, p_all_sur, all_pat, &n_int, &xyz_s_out ); 
    if ( status < 0 )
      {
      sprintf(errbuf,"sur191%%varkon_sur_aface (sur190");
      varkon_erpush("SU2943",errbuf);
      goto dealloc;
      }                            

nointer:;      /* No interrogation     */

    if       (  a_case  ==  1 )
      {
      poi.x_gm  = xyz_s.r_x;
      poi.y_gm  = xyz_s.r_y;
      poi.z_gm  = xyz_s.r_z;
      utan.x_gm = xyz_s.u_x;
      utan.y_gm = xyz_s.u_y;
      utan.z_gm = xyz_s.u_z;
      vtan.x_gm = xyz_s.v_x;
      vtan.y_gm = xyz_s.v_y;
      vtan.z_gm = xyz_s.v_z;
      }
    else if  (  a_case  ==  2 && n_int >   0 )
      {
      poi.x_gm  = xyz_s_out.r_x;
      poi.y_gm  = xyz_s_out.r_y;
      poi.z_gm  = xyz_s_out.r_z;
      utan.x_gm = xyz_s_out.u_x;
      utan.y_gm = xyz_s_out.u_y;
      utan.z_gm = xyz_s_out.u_z;
      vtan.x_gm = xyz_s_out.v_x;
      vtan.y_gm = xyz_s_out.v_y;
      vtan.z_gm = xyz_s_out.v_z;
      }
    else if  (  a_case  ==  2 && n_int ==  0 )
      {
      sprintf(errbuf,"%8.2f %8.2f %8.2f %%sur190",
              xyz_s.r_x, xyz_s.r_y, xyz_s.r_z );
      varkon_erinit();
      varkon_erpush("SU5043",errbuf);
      goto dealloc;
      }
    else if  (  a_case  ==  3 && n_int >   0 )
      {
      poi.x_gm  = xyz_s_out.r_x;
      poi.y_gm  = xyz_s_out.r_y;
      poi.z_gm  = xyz_s_out.r_z;
      utan.x_gm = xyz_s_out.u_x;
      utan.y_gm = xyz_s_out.u_y;
      utan.z_gm = xyz_s_out.u_z;
      vtan.x_gm = xyz_s_out.v_x;
      vtan.y_gm = xyz_s_out.v_y;
      vtan.z_gm = xyz_s_out.v_z;
      }
    else if  (  a_case  ==  3 && n_int ==  0 )
      {
      poi.x_gm  = xyz_s.r_x;
      poi.y_gm  = xyz_s.r_y;
      poi.z_gm  = xyz_s.r_z;
      utan.x_gm = xyz_s.u_x;
      utan.y_gm = xyz_s.u_y;
      utan.z_gm = xyz_s.u_z;
      vtan.x_gm = xyz_s.v_x;
      vtan.y_gm = xyz_s.v_y;
      vtan.z_gm = xyz_s.v_z;

      sprintf(errbuf,"%8.2f %8.2f %8.2f %%sur190",
              xyz_s.r_x, xyz_s.r_y, xyz_s.r_z );
      varkon_erinit();
      varkon_erpush("SU5061",errbuf);
      varkon_errmes();
      }


    else if  (  a_case  ==  4 && n_int >   0 )
      {
      poi.x_gm  = xyz_s_out.r_x;
      poi.y_gm  = xyz_s_out.r_y;
      poi.z_gm  = xyz_s_out.r_z;
      utan.x_gm = xyz_s_out.u_x;
      utan.y_gm = xyz_s_out.u_y;
      utan.z_gm = xyz_s_out.u_z;
      vtan.x_gm = xyz_s_out.v_x;
      vtan.y_gm = xyz_s_out.v_y;
      vtan.z_gm = xyz_s_out.v_z;
      }
    else if  (  a_case  ==  4 && n_int ==  0 )
      {
      poi.x_gm  = xyz_s.r_x;
      poi.y_gm  = xyz_s.r_y;
      poi.z_gm  = xyz_s.r_z;
      utan.x_gm = xyz_s.u_x;
      utan.y_gm = xyz_s.u_y;
      utan.z_gm = xyz_s.u_z;
      vtan.x_gm = xyz_s.v_x;
      vtan.y_gm = xyz_s.v_y;
      vtan.z_gm = xyz_s.v_z;
      }
    else if  (  a_case  ==  5 || a_case == 6 || a_case == 7 )
      {
      sprintf(errbuf, "a_case %d not impl.%%sur190",(int)a_case);
      return(varkon_erpush("SU2993",errbuf));
      }
    else
      {
      sprintf(errbuf, 
          "a_case %d n_int %d%%sur190",(int)a_case, (int)n_int);
      return(varkon_erpush("SU2993",errbuf));
      }




    v_leng = SQRT(utan.x_gm*utan.x_gm+utan.y_gm*utan.y_gm+
                  utan.z_gm*utan.z_gm);  
    if  ( v_leng > 0.0000001 )
      {
      utan.x_gm = utan.x_gm/v_leng; 
      utan.y_gm = utan.y_gm/v_leng;
      utan.z_gm = utan.z_gm/v_leng;
      }
    else
      {
      sprintf(errbuf,"utan=0%%sur190");
      varkon_erpush("SU2993",errbuf);
      goto dealloc;
      }
    v_leng = SQRT(vtan.x_gm*vtan.x_gm+vtan.y_gm*vtan.y_gm+
                  vtan.z_gm*vtan.z_gm);  
    if  ( v_leng > 0.0000001 )
      {
      vtan.x_gm = vtan.x_gm/v_leng; 
      vtan.y_gm = vtan.y_gm/v_leng;
      vtan.z_gm = vtan.z_gm/v_leng;
      }
    else
      {
      sprintf(errbuf,"vtan=0%%sur190");
      varkon_erpush("SU2993",errbuf);
      goto dealloc;
      }


    V3MOME((char *)( &poi ),(char *)(p_poi  + i_ptan),sizeof(DBVector));
    V3MOME((char *)( &utan),(char *)(p_utan + i_ptan),sizeof(DBVector));
    V3MOME((char *)( &vtan),(char *)(p_vtan + i_ptan),sizeof(DBVector));
    i_ptan = i_ptan + 1;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur190 s_sur %8.4f t_sur %8.4f i_ptan %d\n" 
          , s_sur, t_sur, (int)i_ptan-1 );
fflush(dbgfil(SURPAC)); 
}
#endif

    }  /* End loop U lines */
  }    /* End loop V lines */


/*!                                                                 */
/* 6. Create surface                                                */
/* _________________                                                */
/*                                                                 !*/

 sprintf(metod_s, "ZEROTWIST_1");  

     status = varkon_sur_splarr
     (metod_s, np_u, np_v , p_poi, p_utan ,p_vtan, p_twist,
                    p_surout,pp_patout); 
     if ( status < 0 )
       {
       sprintf(errbuf,"sur294%%varkon_sur_aface (sur190");
       varkon_erpush("SU2943",errbuf);
       goto dealloc;
       }                            

/*!                                                                 */
/* 7. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/* Deallocate memory area for points and tangents. Call v3free.     */
/*                                                                 !*/

dealloc:;  /* Label: Deallocate when function has failed            */

 if ( p_poi  != NULL ) v3free(p_poi   ,"sur190"); 
 if ( p_utan != NULL ) v3free(p_utan  ,"sur190");
 if ( p_vtan != NULL ) v3free(p_vtan  ,"sur190");

/*!                                                                 */
/* Deallocate patch areas. Calls of DBfree_patches.                 */
/*                                                                 !*/

for ( i_sur = 1 ; i_sur  <= n_sur; ++i_sur  ) 
  {
  V3MOME((char *)( &all_sur[i_sur-1]),
                 (char *)(&current_sur),sizeof(DBSurf));
  if ( all_pat[i_sur-1] != NULL ) 
              DBfree_patches(&current_sur,all_pat[i_sur-1]);
  }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur190 Exit***varkon_sur_aface     ** Output nu %d nv %d type %d\n"
    , (int)p_surout->nu_su,(int)p_surout->nv_su, (int)p_surout->typ_su);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(status);

  } /* End of function */

/*********************************************************/
