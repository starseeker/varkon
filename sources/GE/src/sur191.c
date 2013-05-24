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
/*  Function: varkon_sur_aface_l                   File: sur191.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Intersect between straight line and all surfaces for SUR_AFACE. */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-06-08   Originally written                                 */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_aface_l    Line intersects for SUR_AFACE    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_evals        * Initialize EVALS                       */
/* varkon_sur_intlinept    * Intersect line/surface                 */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_sur_order        * Order surface (solut.) pts             */
/* varkon_vec_projpla      * Project vector onto plane              */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_aface_l         */
/* SU2993 = Severe program error (  ) in varkon_sur_aface_l         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_aface_l (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  EVALS   *p_xyz_s_in,      /* Coordinates and normal which   (ptr) */
                            /* defines the line                     */
  DBint    n_sur,           /* Number of surfaces                   */
  DBSurf  *p_all_sur[],     /* All surfaces                  (ptrs) */
  DBPatch *p_all_pat[],     /* Allocated areas for patches   (ptrs) */
  DBint   *p_n_int,         /* Total number of intersect pts  (ptr) */

  EVALS   *p_xyz_s_out )    /* Coordinates and derivatives    (ptr) */

/* Out:                                                             */
/*                                                                  */
/*      Data to p_xyz_s_out.                                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector extpoi;       /* External point                          */
  DBVector snorm;        /* Surface normal                          */

  DBVector s_uvpt;       /* Start U,V point                         */
                         /* p_s_uvpt.z_gm < 0: Not defined.         */
  DBint    scase;        /* Solution selection case                 */
                         /* Eq. 1: All intersect points             */
                         /* Eq. 2: Intersects in shooting direction */
                         /* Eq. 3: As 2 but only intersect pts with */
                         /*        surface normals in the shooting  */
                         /*        direction will be output         */
                         /* Eq. 4: As 2 but only intersect pts with */
                         /*        surface normals in the opposite  */
                         /*        shooting direction will be output*/
                         /* Eq. 5: As 3 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        opposite direct. to shooting dir */
                         /* Eq. 6: As 4 but output number of sol's  */
                         /*        is 0 if closest pt normal is in  */
                         /*        same     direct. to shooting dir */
  DBint    sol_no;       /* Requested solution number               */
  DBint    ocase;        /* Point ordering case                     */
                         /* Eq. 1: Ordered according to R*3 distance*/
                         /* Eq. 2: Ordered according to UV  distance*/
                         /* (No error exit if 1000 is added)        */
  DBint    acc;          /* Computation accuracy (case)             */
                         /* Undefined p_s_uvpt => all surf. patches */
                         /* Defined   p_s_uvpt => One patch  search */
                         /* Eq. 1: All patches connect. to p_s_uvpt */
                         /* Defined p_s_uvpt => Nine patches search */
                         /* Eq. 2: Next layer of patches ...        */
  DBint    no_points;    /* Total number of solution points         */
  DBVector cpt;          /* Closest point in R*3 space              */
  DBVector cuvpt;        /* Closest point in U,V space              */
  DBVector uv_out[SMAX]; /* All U,V solution pts. Ordered           */
  DBVector r3_out[SMAX]; /* All R*3 solution pts. Ordered           */
  DBint    no_points_a;  /* All solutions for all surfaces          */
  EVALS    xyz_s_a[SMAX];/* All solution points for all surfaces    */
  DBint    ocase_l;      /* Ordering case for sur980                */
  EVALS    xyz_out[SMAX];/* Sorted solution points                  */

  DBint    icase;        /* Case for varkon_sur_eval                */
  EVALS    xyz_l;        /* Coordinates and derivatives for the     */
                         /* line/surface intersect point            */

  DBint    i_sur;        /* Loop index surface                      */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector sdir;         /* Vector that shall be projected          */
  DBVector plane;        /* Plane  normal                           */
  DBint    ocase_p;      /* Output case. Eq. 1: Normalised vector   */
  DBVector psdir;        /* Projected vector                        */

  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

#ifdef  DEBUG
  DBfloat  v_leng;       /* Length of a vector                      */
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
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur191 Enter***varkon_sur_aface_l n_sur %d\n" , n_sur );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
for ( i_sur = 1 ; i_sur  <= n_sur ; ++i_sur  ) 
{
fprintf(dbgfil(SURPAC), "sur191 p_all_sur[%d] %d p_all_pat[%d] %d\n", 
  i_sur-1,p_all_sur[i_sur-1], i_sur-1,p_all_pat[i_sur-1] );
}
}
#endif
 
  *p_n_int = 0;  /* Initialize total number of output intersect pts */

/*!                                                                 */
/* Initialize output variable p_xyz_s_out (EVALS)                   */
/* Call of varkon_ini_evals (sur170).                               */
/*                                                                 !*/

      varkon_ini_evals (p_xyz_s_out);    

/*!                                                                 */
/* Check number of surfaces for Debug On                            */
/*                                                                 !*/

#ifdef  DEBUG
  if (  n_sur <= 0  )
    {
    sprintf(errbuf,                         
   "n_sur %d<= 0%%sur191",n_sur);
    return(varkon_erpush("SU2993",errbuf));
    }
  if (  n_sur > SFMAX  )
    {
    sprintf(errbuf,                         
   "n_sur %d>SFMAX %d%%sur191",n_sur,SFMAX);
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

/*!                                                                 */
/* Point and normal from p_xyz_s_in to extpoi and snorm.            */
/*                                                                 !*/

    extpoi.x_gm  = p_xyz_s_in->r_x;
    extpoi.y_gm  = p_xyz_s_in->r_y;
    extpoi.z_gm  = p_xyz_s_in->r_z;
    snorm.x_gm   = p_xyz_s_in->n_x;
    snorm.y_gm   = p_xyz_s_in->n_y;
    snorm.z_gm   = p_xyz_s_in->n_z;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur191 extpoi %f %f %f\n", extpoi.x_gm, extpoi.y_gm, extpoi.z_gm );
fprintf(dbgfil(SURPAC),
"sur191 snorm  %f %f %f\n",  snorm.x_gm,  snorm.y_gm,  snorm.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Check length surface normal for Debug On.                        */
/*                                                                 !*/

#ifdef  DEBUG
    v_leng = SQRT(snorm.x_gm*snorm.x_gm+snorm.y_gm*snorm.y_gm+
                  snorm.z_gm*snorm.z_gm);  
    if  ( v_leng < 0.99  )
      {
      sprintf(errbuf,"snorm=0%%sur191");
      varkon_erpush("SU2993",errbuf);
      goto err1;       
      }
#endif


/*!                                                                 */
/* 2. Calculate intersects for all surfaces                         */
/* ________________________________________                         */
/*                                                                 !*/

   no_points_a =    0;

   ocase       = 1001;
   scase       =    1;
   acc         =    1;
   sol_no      =    1;
   s_uvpt.x_gm = F_UNDEF;
   s_uvpt.y_gm = F_UNDEF;
   s_uvpt.z_gm =   -1.0;  /* No start point in UV plane */

  for ( i_sur = 1 ; i_sur  <= n_sur ; ++i_sur  ) 
    {
    status = varkon_sur_intlinept 
    (p_all_sur[i_sur-1],p_all_pat[i_sur-1],&extpoi,&snorm , 
     &s_uvpt,ocase,scase,acc,sol_no,
     &no_points,  &cpt, &cuvpt, uv_out, r3_out );
    if ( status < 0 )
      {
      sprintf(errbuf,"sur161%%varkon_sur_aface_l (sur191");
      varkon_erpush("SU2943",errbuf);
      goto err1;      
      }                            

    if ( no_points > 0 )
      {
      no_points_a = no_points_a + 1;
      if  ( no_points_a > SMAX )
        {
        sprintf(errbuf,"no_points_a>SMAX%%varkon_sur_aface_l (sur191");
        varkon_erpush("SU2993",errbuf);
        goto err1;      
        }

/*!                                                                 */
/*     Points are already ordered. Select number one and add to all */
/*     Call of varkon_sur_eval (sur210).                            */
/*                                                                 !*/

      icase = 3;


   status= varkon_sur_eval
      (p_all_sur[i_sur-1],p_all_pat[i_sur-1],icase,
         cuvpt.x_gm,cuvpt.y_gm,&xyz_l);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur191 varkon_sur_eval (sur210) failed for us %f vs %f\n",
           cuvpt.x_gm, cuvpt.y_gm); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%sur191");
    return(varkon_erpush("SU2943",errbuf));
    }

      xyz_s_a[no_points_a-1] = xyz_l; /* Should be V3MOME ... */
      }
      
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur191 i_sur %d no_points %d no_points_a %d\n", 
         i_sur, no_points, no_points_a );
fflush(dbgfil(SURPAC)); 
}
#endif

    }

/*!                                                                 */
/* 3. Select closest point and project tangents                     */
/* ____________________________________________                     */
/*                                                                 !*/

/*!                                                                 */
/* Order solution points and delete equal solutions.                */
/* Call of varkon_sur_order (sur980).                               */
/*                                                                 !*/

   ocase_l = 1;

status=varkon_sur_order
      (&extpoi, xyz_s_a, no_points_a, ocase_l, xyz_out, p_n_int);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_order%%varkon_sur_intlinept");
        return(varkon_erpush("SU2943",errbuf));
        }

    if ( *p_n_int == 0 )
      {
      sprintf(errbuf,"p_n_int=0%%varkon_sur_aface_l (sur191");
      varkon_erpush("SU2993",errbuf);
      goto err1;      
      }

/*!                                                                 */
/* Output coordinates to p_xyz_s_out.                               */
/*                                                                 !*/

   p_xyz_s_out->r_x = xyz_out[0].r_x;
   p_xyz_s_out->r_y = xyz_out[0].r_y;
   p_xyz_s_out->r_z = xyz_out[0].r_z;

/*!                                                                 */
/* Output tangents    to p_xyz_out.                                 */
/*                                                                 !*/

   plane.x_gm = p_xyz_s_in->n_x; 
   plane.y_gm = p_xyz_s_in->n_y; 
   plane.z_gm = p_xyz_s_in->n_z;

   sdir.x_gm  = p_xyz_s_in->u_x; 
   sdir.y_gm  = p_xyz_s_in->u_y; 
   sdir.z_gm  = p_xyz_s_in->u_z;

   ocase_p = 1;      

       status= varkon_vec_projpla
       ( sdir, plane, ocase_p, &psdir );
       if (status<0) 
        {
        sprintf(errbuf,"sur670%%sur191");
        return(varkon_erpush("SU2943",errbuf));
        }

   p_xyz_s_out->u_x = psdir.x_gm;
   p_xyz_s_out->u_y = psdir.y_gm;
   p_xyz_s_out->u_z = psdir.z_gm;


   sdir.x_gm  = p_xyz_s_in->v_x; 
   sdir.y_gm  = p_xyz_s_in->v_y; 
   sdir.z_gm  = p_xyz_s_in->v_z;

   ocase_p = 1;      

       status= varkon_vec_projpla
       ( sdir, plane, ocase_p, &psdir );
       if (status<0) 
        {
        sprintf(errbuf,"sur670%%sur191");
        return(varkon_erpush("SU2943",errbuf));
        }

   p_xyz_s_out->v_x = psdir.x_gm;
   p_xyz_s_out->v_y = psdir.y_gm;
   p_xyz_s_out->v_z = psdir.z_gm;


/*!                                                                 */
/* 4. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                 !*/

err1:;     /* Label: Exit with error                                */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 && *p_n_int > 0 )
{
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_out->r_x %f r_y %f r_z %f\n",
 p_xyz_s_out->r_x, p_xyz_s_out->r_y, p_xyz_s_out->r_z );
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_in->r_x  %f r_y %f r_z %f\n",
 p_xyz_s_in->r_x, p_xyz_s_in->r_y, p_xyz_s_in->r_z );
fprintf(dbgfil(SURPAC),
"sur191 Distance %25.15f \n",
 SQRT((p_xyz_s_in->r_x-p_xyz_s_out->r_x)*(p_xyz_s_in->r_x-
       p_xyz_s_out->r_x)+
      (p_xyz_s_in->r_y-p_xyz_s_out->r_y)*(p_xyz_s_in->r_y-
       p_xyz_s_out->r_y)+
      (p_xyz_s_in->r_z-p_xyz_s_out->r_z)*(p_xyz_s_in->r_z-
       p_xyz_s_out->r_z) ));
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 2 && *p_n_int > 0 )
{
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_out->u_x %f u_y %f u_z %f\n",
 p_xyz_s_out->u_x, p_xyz_s_out->u_y, p_xyz_s_out->u_z );
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_in->u_x  %f u_y %f u_z %f\n",
 p_xyz_s_in->u_x, p_xyz_s_in->u_y, p_xyz_s_in->u_z );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 && *p_n_int > 0 )
{
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_out->v_x %f v_y %f v_z %f\n",
 p_xyz_s_out->v_x, p_xyz_s_out->v_y, p_xyz_s_out->v_z );
fprintf(dbgfil(SURPAC),
"sur191 p_xyz_s_in->v_x  %f v_y %f v_z %f\n",
 p_xyz_s_in->v_x, p_xyz_s_in->v_y, p_xyz_s_in->v_z );
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur191 Exit*varkon_sur_aface_l* *p_n_int %d\n", *p_n_int);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(status);

  } /* End of function */

/*********************************************************/
