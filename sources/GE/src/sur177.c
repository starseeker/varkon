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
/*  Function: varkon_sur_boxapprox                 File: sur177.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function approximates a surface with the bounding boxes.    */
/*  Output is an array with B-planes, which define the patch        */
/*  bounding boxes.                                                 */
/*                                                                  */
/*  This is a function for the trimming of calculation parameters.  */
/*  A function for the programmer and not the user.                 */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-28   Originally written                                 */
/*  1996-02-24   CON_SUR added                                      */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_boxapprox  Approximate surface with BBOX'es */
/*                                                              */
/*--------------------------------------------------------------*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_boxapprox */
/* SU2993 = Severe program error ( ) in varkon_sur_boxapprox sur177 */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus     varkon_sur_boxapprox (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf   *p_sur,       /* Surface                           (ptr) */
  DBPatch  *p_pat,       /* Alloc. area for topol. patch data (ptr) */
  DBBplane *p_bpltable,  /* B-planes which define BBOX'es     (ptr) */
  DBint    *p_nbpl )     /* Number of B-planes in p_bpltable        */

/* Out:                                                             */
/*       Array of B-planes                                          */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   BBOX     box;         /* Patch box from surface                  */
   DBint    i_u;         /* Loop index U line in surface            */
   DBint    i_v;         /* Loop index V line in surface            */
/*-----------------------------------------------------------------!*/

   DBBplane bpl;         /* Current B-plane                         */
   DBint    s_type;      /* Type CUB_SUR, RAT_SUR, or .....         */
   DBint    n_u;         /* Number of patches in U direction        */
   DBint    n_v;         /* Number of patches in V direction        */

   DBPatch *p_t;         /* Current topological patch         (ptr) */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/* -------------- Flow diagram -------------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_boxapprox !                           */
/*               !     (sur177)         !                           */
/*               !______________________!                           */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
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
"sur177 Enter varkon_sur_boxapprox : Create BBOX'es for surface\n");
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve number of patches and surface types.                    */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surfaces are of type                              */
/* CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR                    */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &n_u, &n_v, &s_type);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_boxapprox (sur177)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (  s_type     == CUB_SUR ||           /* Check surface type      */
      s_type     == RAT_SUR ||           
      s_type     == CON_SUR ||           
      s_type     == POL_SUR ||           
      s_type     ==  P3_SUR ||           
      s_type     ==  P5_SUR ||           
      s_type     ==  P7_SUR ||           
      s_type     ==  P9_SUR ||           
      s_type     == P21_SUR ||           
      s_type     == MIX_SUR ||           
      s_type     == LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_boxapprox (sur177");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/*   Header for B-plane                                             */
/*                                                                 !*/

   bpl.hed_bp.blank = FALSE;       
   bpl.hed_bp.pen   = I_UNDEF;     
   bpl.hed_bp.level = I_UNDEF;     
   bpl.pcsy_bp      = I_UNDEF;       



/*!                                                                 */
/* 2. Create B-planes                                               */
/* ___________________                                              */
/*                                                                  */
/*                                                                 !*/

   *p_nbpl = 0; /* Initialize number of output B-planes             */

/*!                                                                 */
/* Loop all V patches  i_v=  0, 1, ...., n_v                        */
/*  Loop all U patches  i_u= 0, 1,  ..., n_u                        */
/*                                                                 !*/

for (   i_u= 0;  i_u <  n_u  ; ++i_u )   /* Start loop U patches    */
  {
  for ( i_v= 0;  i_v <  n_v  ; ++i_v )   /* Start loop V patches    */
    {


/*!                                                                 */
/*   Pointer to current topological patch                           */
     p_t = p_pat  + i_u*n_v + i_v; 
/*   Retrieve BBOX for the current patch in surface 1.              */
/*                                                                 !*/

     box  =  p_t->box_pat;               /* Copy BBOX  to box       */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur177 box  Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box.xmin,   box.ymin,   box.zmin,
         box.xmax,   box.ymax,   box.zmax );
}
#endif

     if ( *p_nbpl + 6 > BPLMAX )
       {
       sprintf(errbuf,
              "Increase BPLMAX!%%varkon_sur_boxapprox (sur177)");
       return(varkon_erpush("SU2973",errbuf));
       }


/*!                                                                 */
/*   Front B-plane                                                  */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmin;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymin; 
     bpl.crd1_bp.z_gm  =   box.zmin;

     bpl.crd4_bp.x_gm  =   box.xmax;           /* Corner point 2    */
     bpl.crd4_bp.y_gm  =   box.ymin; 
     bpl.crd4_bp.z_gm  =   box.zmin;

     bpl.crd3_bp.x_gm  =   box.xmax;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymax;
     bpl.crd3_bp.z_gm  =   box.zmin;

     bpl.crd2_bp.x_gm  =   box.xmin;           /* Corner point 4    */
     bpl.crd2_bp.y_gm  =   box.ymax;
     bpl.crd2_bp.z_gm  =   box.zmin;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */


/*!                                                                 */
/*   Back  B-plane                                                  */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmin;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymin; 
     bpl.crd1_bp.z_gm  =   box.zmax;

     bpl.crd2_bp.x_gm  =   box.xmax;           /* Corner point 2    */
     bpl.crd2_bp.y_gm  =   box.ymin; 
     bpl.crd2_bp.z_gm  =   box.zmax;

     bpl.crd3_bp.x_gm  =   box.xmax;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymax;
     bpl.crd3_bp.z_gm  =   box.zmax;

     bpl.crd4_bp.x_gm  =   box.xmin;           /* Corner point 4    */
     bpl.crd4_bp.y_gm  =   box.ymax;
     bpl.crd4_bp.z_gm  =   box.zmax;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */

/*!                                                                 */
/*   Left  B-plane                                                  */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmin;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymin; 
     bpl.crd1_bp.z_gm  =   box.zmin;

     bpl.crd2_bp.x_gm  =   box.xmin;           /* Corner point 2    */
     bpl.crd2_bp.y_gm  =   box.ymin; 
     bpl.crd2_bp.z_gm  =   box.zmax;

     bpl.crd3_bp.x_gm  =   box.xmin;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymax;
     bpl.crd3_bp.z_gm  =   box.zmax;

     bpl.crd4_bp.x_gm  =   box.xmin;           /* Corner point 4    */
     bpl.crd4_bp.y_gm  =   box.ymax;
     bpl.crd4_bp.z_gm  =   box.zmin;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */


/*!                                                                 */
/*   Right B-plane                                                  */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmax;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymin; 
     bpl.crd1_bp.z_gm  =   box.zmin;

     bpl.crd4_bp.x_gm  =   box.xmax;           /* Corner point 2    */
     bpl.crd4_bp.y_gm  =   box.ymin; 
     bpl.crd4_bp.z_gm  =   box.zmax;

     bpl.crd3_bp.x_gm  =   box.xmax;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymax;
     bpl.crd3_bp.z_gm  =   box.zmax;

     bpl.crd2_bp.x_gm  =   box.xmax;           /* Corner point 4    */
     bpl.crd2_bp.y_gm  =   box.ymax;
     bpl.crd2_bp.z_gm  =   box.zmin;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */

/*!                                                                 */
/*   Bottom B-plane                                                 */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmin;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymin; 
     bpl.crd1_bp.z_gm  =   box.zmin;

     bpl.crd2_bp.x_gm  =   box.xmax;           /* Corner point 2    */
     bpl.crd2_bp.y_gm  =   box.ymin; 
     bpl.crd2_bp.z_gm  =   box.zmin;

     bpl.crd3_bp.x_gm  =   box.xmax;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymin;
     bpl.crd3_bp.z_gm  =   box.zmax;

     bpl.crd4_bp.x_gm  =   box.xmin;           /* Corner point 4    */
     bpl.crd4_bp.y_gm  =   box.ymin;
     bpl.crd4_bp.z_gm  =   box.zmax;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */


/*!                                                                 */
/*   Top   B-plane                                                  */
/*                                                                 !*/

     bpl.crd1_bp.x_gm  =   box.xmin;           /* Corner point 1    */
     bpl.crd1_bp.y_gm  =   box.ymax; 
     bpl.crd1_bp.z_gm  =   box.zmin;

     bpl.crd4_bp.x_gm  =   box.xmax;           /* Corner point 2    */
     bpl.crd4_bp.y_gm  =   box.ymax; 
     bpl.crd4_bp.z_gm  =   box.zmin;

     bpl.crd3_bp.x_gm  =   box.xmax;           /* Corner point 3    */
     bpl.crd3_bp.y_gm  =   box.ymax;
     bpl.crd3_bp.z_gm  =   box.zmax;

     bpl.crd2_bp.x_gm  =   box.xmin;           /* Corner point 4    */
     bpl.crd2_bp.y_gm  =   box.ymax;
     bpl.crd2_bp.z_gm  =   box.zmax;

     *(p_bpltable+ *p_nbpl) = bpl;             /* To output table    */
     *p_nbpl = *p_nbpl + 1; /* An additional B-plane                */


    }                           /* End   loop U patches             */
  }                             /* End   loop V patches             */
/*!                                                                 */
/*  End  all U patches  i_u= 1, 2,  ..., n_u                        */
/* End  all V patches  i_v= 1, 2,  ..., n_v                         */
/*                                                                 !*/


/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Total number of B-planes to output variable                      */
/*                                                                 !*/

#ifdef DEBUG
      if ( *p_nbpl == 0 ) 
        {
        sprintf(errbuf,"*p_bpl=0%%varkon_sur_boxapprox (sur177)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur177 Exit varkon_sur_boxapprox B-planes n_u*n_v*6= %d *p_nbpl %d\n"
     , (short)(n_u*n_v*6), (short)*p_nbpl );
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


