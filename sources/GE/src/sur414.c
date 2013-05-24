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
/*  Function: varkon_sur_compide                   File: sur414.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Determine if two surface boundaries are identical               */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-12   Originally written                                 */
/*  1996-11-13   Compilation warnings ...                           */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_compide    Identical boundaries? (SUR_COMP) */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_compide (sur414) */
/* SU2993 = Severe program error (  ) in varkon_sur_compide (sur414) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_sur_compide (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur_1,      /* Surface 1                         (ptr) */
  DBPatch *p_pat_1,      /* Surface 1 topological patch data  (ptr) */
  DBSurf  *p_sur_2,      /* Surface 2                         (ptr) */
  DBPatch *p_pat_2,      /* Surface 2 topological patch data  (ptr) */
  SURCOMP *p_scomp,      /* Composite surface comp. data      (ptr) */
  DBint    uv_type1,     /* UV type curve 1= U  2= V surface 1      */
  DBfloat  par_val1,     /* Iso-parameter value surface 1           */
  DBint    uv_type2,     /* UV type curve 1= U  2= V surface 2      */
  DBfloat  par_val2 )    /* Iso-parameter value surface 2           */
/*                                                                  */
/* Out:        Analysis result in SURCOMP                           */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   i_pat1;        /* Loop index patch in surface 1          */
   DBint   i_pat2;        /* Loop index patch in surface 2          */
   DBint   s_pat2;        /* Start patch      in surface 2          */
   DBfloat dist;          /* Distance between boundaries            */
   DBfloat diru;          /* Scalar product U tangents              */
   DBfloat dirv;          /* Scalar product V tangents              */
   DBfloat dirc;          /* Scalar product current direction       */
   DBfloat idpoint;       /* Criterion for equal point              */
   DBint   f_equal;       /* Flag Eq. -1: Not 1: Equal 2: End equal */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat t_par1;       /* Current parameter value     surface 1   */
   DBfloat t_par2;       /* Current parameter value     surface 2   */
   DBint   icase;        /* Case for varkon_sur_eval                */
   DBint   n_pat1;       /* End patch           surface 1           */
   DBint   n_pat2;       /* End patch           surface 2           */
   EVALS   xyz1;         /* Coordinates and derivatives surface 1   */
   DBfloat u_par1;       /* Surface U parameter         surface 1   */
   DBfloat v_par1;       /* Surface V parameter         surface 1   */
   EVALS   xyz2;         /* Coordinates and derivatives surface 2   */
   DBfloat u_par2;       /* Surface U parameter         surface 2   */
   DBfloat v_par2;       /* Surface V parameter         surface 2   */
   DBint   nu1;          /* Number of patches in U in surface 1     */
   DBint   nv1;          /* Number of patches in V in surface 1     */
   DBint   surtype1;     /* Type of surface                         */
   DBint   nu2;          /* Number of patches in U in surface 2     */
   DBint   nv2;          /* Number of patches in V in surface 2     */
   DBint   surtype2;     /* Type of surface                         */
   DBfloat comptol;      /* Computer accuracy                       */
   DBfloat lenu1;        /* Length of U vector surface 1            */
   DBfloat lenv1;        /* Length of V vector surface 1            */
   DBfloat lenu2;        /* Length of U vector surface 2            */
   DBfloat lenv2;        /* Length of V vector surface 2            */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

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
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 Enter*varkon_sur_compide* p_sur_1 %d p_sur_2 %d \n",
        (int)p_sur_1, (int)p_sur_2);
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 p_pat_1 %d p_pat_2 %d\n",
        (int)p_pat_1, (int)p_pat_2);
fflush(dbgfil(SURPAC)); 
}
#endif
 
    nu1               = p_scomp->nu1;
    nv1               = p_scomp->nv1;
    surtype1          = p_scomp->surtype1;
    nu2               = p_scomp->nu2;
    nv2               = p_scomp->nv2;
    surtype2          = p_scomp->surtype2;
    idpoint           = p_scomp->idpoint;   
    comptol           = p_scomp->comptol;   

    if      (  uv_type1 == 1  )
      {
      n_pat1 = nv1;
      }
    else if (  uv_type1 == 2  )
      {
      n_pat1 = nu1;
      }
    else
      {
      sprintf(errbuf,"uv_type1%%sur414");
      return(varkon_erpush("SU4013",errbuf));
      }
    if      (  uv_type2 == 1  )
      {
      n_pat2 = nv2;
      }
    else if (  uv_type2 == 2  )
      {
      n_pat2 = nu2;
      }
    else
      {
      sprintf(errbuf,"uv_type2%%sur414");
      return(varkon_erpush("SU4013",errbuf));
      }


    icase = 3;

/*!                                                                 */
/* 2. Determine surface composite case                              */
/* ___________________________________                              */
/*                                                                 !*/

/*!                                                                 */
/* Compare points on surface 1 and 2 for the given iso values       */
/*                                                                 !*/

   f_equal = -1;         /* Flag Eq. -1: Not 1: Equal 2: End equal  */
   s_pat2  =  1;         /* Start patch      in surface 2           */

/*!                                                                 */
/* Loop all surfaces i_pat1= 1,....,nv1+1                           */
/*                                                                 !*/

for ( i_pat1=1; i_pat1 <= n_pat1+1; ++i_pat1 ) /* Loop surface 1    */
  {

/*!                                                                 */
/*   Current point on surface 1 boundary.                           */
/*   Call of varkon_sur_eval (sur210).                              */
/*                                                                 !*/

     t_par1 = (DBfloat)i_pat1 + p_scomp->comptol;
     if ( i_pat1 == n_pat1+1 ) t_par1 = t_par1 - 2.0*p_scomp->comptol;
     if      (    uv_type1 == 1  )
       {
       u_par1 = par_val1;
       v_par1 = t_par1;
       }
     else if (    uv_type1 == 2  )
       {
       u_par1 = t_par1;
       v_par1 = par_val1;
       }

     status= varkon_sur_eval
    (p_sur_1,p_pat_1,icase,u_par1,v_par1,&xyz1);
      if(status<0)
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur414 varkon_sur_eval (sur210) failed for u_par1 %f v_par1 %f\n",
               u_par1,v_par1); 
#endif
      goto null1;
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 u_par1 %3.1f v_par1 %3.1f X %8.1f Y %8.1f Z %8.1f\n",
        u_par1, v_par1, xyz1.r_x, xyz1.r_y, xyz1.r_z);
fflush(dbgfil(SURPAC)); 
}
#endif

  dirc = F_UNDEF;

/*!                                                                 */
/* Loop all surfaces i_pat2= 1,....,n_pat2+1                        */
/*                                                                 !*/

for ( i_pat2=s_pat2; i_pat2 <= n_pat2+1; ++i_pat2 ) 
  {

/*!                                                                 */
/*   Current point on surface 2 boundary.                           */
/*   Call of varkon_sur_eval (sur210).                              */
/*                                                                 !*/

     t_par2 = (DBfloat)i_pat2 + p_scomp->comptol;
     if ( i_pat2 == n_pat2+1 ) t_par2 = t_par2 - 2.0*p_scomp->comptol;
     if      (  uv_type2 == 1  )
       {
       u_par2 = par_val2;
       v_par2 = t_par2;
       }
     else if (  uv_type2 == 2  )
       {
       u_par2 = t_par2;
       v_par2 = par_val2;
       }

     status= varkon_sur_eval
         (p_sur_2,p_pat_2,icase,u_par2,v_par2,&xyz2);
      if(status<0)
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur414 varkon_sur_eval (sur210) failed for u_par2 %f v_par2 %f\n"
      ,u_par2,v_par2); 
#endif
      goto null2;
      }
 
    dist = SQRT(  (xyz2.r_x-xyz1.r_x)* (xyz2.r_x-xyz1.r_x)+ 
                  (xyz2.r_y-xyz1.r_y)* (xyz2.r_y-xyz1.r_y)+ 
                  (xyz2.r_z-xyz1.r_z)* (xyz2.r_z-xyz1.r_z) );

    lenu1 =  SQRT(xyz1.u_x*xyz1.u_x + xyz1.u_y*xyz1.u_y + 
                  xyz1.u_z*xyz1.u_z);
    lenu2 =  SQRT(xyz2.u_x*xyz2.u_x + xyz2.u_y*xyz2.u_y + 
                  xyz2.u_z*xyz2.u_z);
    diru  =  xyz2.u_x*xyz1.u_x+ xyz2.u_y*xyz1.u_y+ 
                  xyz2.u_z*xyz1.u_z;
    if  (  lenu1* lenu1 > 0.0000000001 ) diru = diru/lenu1/lenu2;
    lenv1 =  SQRT(xyz1.v_x*xyz1.v_x + xyz1.v_y*xyz1.v_y + 
                  xyz1.v_z*xyz1.v_z);
    lenv2 =  SQRT(xyz2.v_x*xyz2.v_x + xyz2.v_y*xyz2.v_y + 
                  xyz2.v_z*xyz2.v_z);
    dirv  =  xyz2.v_x*xyz1.v_x+ xyz2.v_y*xyz1.v_y+ xyz2.v_z*xyz1.v_z;
    if  (  lenv1* lenv1 > 0.0000000001 ) dirv = dirv/lenv1/lenv2;
    
    if      (  uv_type1 == 1  )
      {
      dirc = dirv;   
      }
    else if (  uv_type1 == 2  )
      {
      dirc = diru;   
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 u_par2 %3.1f v_par2 %3.1f X %8.1f Y %8.1f Z %8.1f\n",
        u_par2, v_par2, xyz2.r_x, xyz2.r_y, xyz2.r_z);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 u_par1 %3.1f v_par1 %3.1f u_par2 %3.1f v_par2 %3.1f diru %15.8f dirv %f\n",
       u_par1, v_par1,  u_par2, v_par2, diru , dirv );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 u_par1 %3.1f v_par1 %3.1f u_par2 %3.1f v_par2 %3.1f dist %15.8f dirc %f\n",
       u_par1, v_par1,  u_par2, v_par2, dist , dirc );
fflush(dbgfil(SURPAC)); 
}
#endif


    if ( f_equal == -1 && dist <= idpoint && 
                 fabs(dirc-1.0) < 0.0001   )
      {
      f_equal = 1;
      p_scomp->s_p_sur1 = t_par1; /* Start parameter surface 1      */
      p_scomp->s_p_sur2 = t_par2; /* Start parameter surface 2      */
      s_pat2 = i_pat2+1;
      goto  equal;
      }

    else if ( f_equal ==  1 && dist <= idpoint && 
                 fabs(dirc-1.0) < 0.0001  )
      {
      f_equal = 1;
      s_pat2 = i_pat2+1;
      goto  equal; 
      }

    else if   ( f_equal ==  1 && dist <= idpoint && 
                i_pat1 == n_pat1+1 && i_pat2 == n_pat2+1  && 
                fabs(dirc-1.0) < 0.0001  )
      {
      f_equal = 2;
      p_scomp->e_p_sur1 = t_par1; /* End   parameter surface 1      */
      p_scomp->e_p_sur2 = t_par2; /* End   parameter surface 2      */
      goto  nomore;
      }

    else if   ( f_equal ==  1 && dist >  idpoint )
      {
      f_equal = 2;
      p_scomp->e_p_sur1 = t_par1 - 1.0; /* End   param. surface 1   */
      p_scomp->e_p_sur2 = t_par2 - 1.0; /* End   param. surface 2   */
      goto  nomore;
      }

null2:;     /* Label: Assume null patch in surface 2                */

    }                                    /* End   loop surface 2    */
/*!                                                                 */
/*   End  all patches  i_pat2= 1,....,n_pat2+1                      */
/*                                                                 !*/
equal:;     /* Label: Points are equal                              */
null1:;     /* Label: Assume null patch in surface 1                */

  }                                      /* End   loop surface 1    */
/*!                                                                 */
/* End  all patches  i_pat1= 1,....,n_pat1+1                        */
/*                                                                 !*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

   if   ( f_equal ==  1 )
      {
      f_equal = 2;
      p_scomp->e_p_sur1 = t_par1; /* End   parameter surface 1      */
      p_scomp->e_p_sur2 = t_par2; /* End   parameter surface 2      */
      }

nomore:;    /* Label: End equal point found                         */

  if     ( f_equal == 2 &&
        fabs( p_scomp->s_p_sur1- p_scomp->s_p_sur2) <  0.0001 &&
        fabs( p_scomp->e_p_sur1- p_scomp->e_p_sur2) <  0.0001   )
    {
    if (  fabs( p_scomp->s_p_sur1          - 1.0 ) < 10*comptol  &&
          fabs( p_scomp->e_p_sur1 - (DBfloat)n_pat1 - 1.0 ) 
                                            < 10*comptol  &&
          fabs( p_scomp->s_p_sur2            - 1.0 ) < 10*comptol  &&
          fabs( p_scomp->e_p_sur2 - (DBfloat)n_pat2 - 1.0 ) 
                                            < 10*comptol  ) 
      p_scomp->comp_case = BDY_IDENT;
    else  
      p_scomp->comp_case = BDY_TRIMP;
    }
  else  if ( f_equal == 2  )
    {
    p_scomp->comp_case = BDY_NULLP;
    }
  else  if ( f_equal == 1  )
    {
    p_scomp->comp_case = BDY_AFAIL;
    }

/* n. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && p_scomp->comp_case > 0  )
{
fprintf(dbgfil(SURPAC),
"sur414 Surface 1 boundary p_scomp->s_p_sur1  %3.1f e_p_sur1  %3.1f \n",
         p_scomp->s_p_sur1, p_scomp->e_p_sur1);
fprintf(dbgfil(SURPAC),
"sur414 Surface 2 boundary p_scomp->s_p_sur2  %3.1f e_p_sur2 %3.1f\n",
          p_scomp->s_p_sur2, p_scomp->e_p_sur2);
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur414 f_equal = %d\n",(int)f_equal);
}
if ( dbglev(SURPAC) == 1 )
{
if         (  p_scomp->comp_case == BDY_IDENT )
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* comp_case= BDY_IDENT (= %d) \n"
                 , (int)p_scomp->comp_case);
else  if   (  p_scomp->comp_case == BDY_NULLP )
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* comp_case= BDY_NULLP (= %d) \n"
                 , (int)p_scomp->comp_case);
else  if   (  p_scomp->comp_case == BDY_TRIMP )
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* comp_case= BDY_TRIMP (= %d) \n"
                 , (int)p_scomp->comp_case);
else  if   (  p_scomp->comp_case == BDY_AFAIL )
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* comp_case= BDY_AFAIL (= %d) \n"
                 , (int)p_scomp->comp_case);
else  if   (  p_scomp->comp_case == I_UNDEF   )
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* Boundary %d %3.1f and %d %3.1f are not identical\n",
           (int)uv_type1, par_val1, (int)uv_type2, par_val2   );
else 
fprintf(dbgfil(SURPAC),
"sur414 Exit*varkon_sur_compide* Error comp_case= %d \n"
                 , (int)p_scomp->comp_case);
}
#endif


    return(SUCCED);

  }

/*********************************************************/
