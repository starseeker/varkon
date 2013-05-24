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
/*  Function: varkon_sur_mboxcone                  File: sur913.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates BBOX and BCONE for a surface            */
/*                                                                  */
/*                                                                  */
/*  BCONE not yet implemented !!!!!  Only maximum angle !!!!!!      */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-03-18   Originally written                                 */
/*  1997-05-22   F_UNDEF for p_angmax                               */
/*  1997-12-04   NURB_SUR added                                     */
/*  1999-11-28   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_mboxcone   Calculate surface BBOX and BCONE */
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
/* varkon_ini_bbox        * Initialize BBOX                         */
/* varkon_ini_bcone       * Initialize BCONE                        */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_mboxcone  */
/* SU2993 = Severe program error ( ) in varkon_sur_mboxcone  sur913 */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus      varkon_sur_mboxcone (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  BBOX    *p_bbox,       /* Bounding box  for the surface     (ptr) */
  BCONE   *p_bcone,      /* Bounding cone for the surface     (ptr) */
  DBfloat *p_angmax,     /* Maximum BCONE angle for a patch   (ptr) */
  DBint   *p_nbadb,      /* Number of bad (BBOX)  patches     (ptr) */
  DBint   *p_nbadc )     /* Number of bad (BBONE) patches     (ptr) */

/* Out:                                                             */
/*      Data to p_bbox and p_bcone                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   BBOX    box;          /* Patch box  from surface                 */
   BCONE   cone;         /* Patch cone from surface                 */
   DBint   i_u;          /* Loop index U line in surface            */
   DBint   i_v;          /* Loop index V line in surface            */
   DBfloat xmin;         /* Surface minimum X value                 */
   DBfloat xmax;         /* Surface maximum X value                 */
   DBfloat ymin;         /* Surface minimum Y value                 */
   DBfloat ymax;         /* Surface maximum Y value                 */
   DBfloat zmin;         /* Surface minimum Z value                 */
   DBfloat zmax;         /* Surface maximum Z value                 */
/*-----------------------------------------------------------------!*/

   DBint   s_type;       /* Type CUB_SUR, RAT_SUR, or .....         */
   DBint   n_u;          /* Number of patches in U direction        */
   DBint   n_v;          /* Number of patches in V direction        */

   DBPatch *p_t;         /* Current topological patch         (ptr) */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/* -------------- Flow diagram -------------------------------------*/
/*                ______________________                            */
/*               !                      !                           */
/*               ! varkon_sur_mboxcone  !                           */
/*               !     (sur913)         !                           */
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
"sur913 Enter varkon_sur_mboxcone: Calculate surface BBOX and BCONE\n");
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Initialize variabels BBOX and BCONE.                             */
/* Call of varkon_ini_bbox (sur772) and varkon_ini_bcone (sur773)   */
/*                                                                 !*/

      varkon_ini_bbox (p_bbox);
      varkon_ini_bcone (p_bcone);

/*!                                                                 */
/* Initialize output number of bad patches.                         */
/*                                                                 !*/

  *p_nbadb = 0;
  *p_nbadc = 0;


/*!                                                                 */
/* Retrieve number of patches and surface types.                    */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surfaces are of type                              */
/* CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR                    */
/*                                                                 !*/

/* Also FAC_SUR (without BOX'es) should be added !!!!!              */

    status = varkon_sur_nopatch (p_sur, &n_u, &n_v, &s_type);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_mboxcone  (sur913)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if (  s_type     ==  CUB_SUR ||         /* Check surface type      */
      s_type     ==  RAT_SUR ||           
      s_type     ==  CON_SUR ||           
      s_type     ==  POL_SUR ||           
      s_type     ==   P3_SUR ||           
      s_type     ==   P5_SUR ||           
      s_type     ==   P7_SUR ||           
      s_type     ==   P9_SUR ||           
      s_type     ==  P21_SUR ||           
      s_type     == NURB_SUR ||           
      s_type     ==  MIX_SUR ||           
      s_type     ==  LFT_SUR )            
 ;
 else
 {
 sprintf(errbuf, "(type)%%sur913");
 return(varkon_erpush("SU2993",errbuf));
 }


/*!                                                                 */
/* 2. Calculate surface maximum and minimum values from BBOXE's     */
/* ____________________________________________________________     */
/*                                                                  */
/*                                                                 !*/

  xmin      =  500000000.0;
  xmax      = -500000000.0;
  ymin      =  500000000.0;
  ymax      = -500000000.0;
  zmin      =  500000000.0;
  zmax      = -500000000.0;
  *p_angmax = F_UNDEF;

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
/*   Retrieve BBOX and BCONE for the current patch in surface.      */
/*                                                                 !*/

     box  =  p_t->box_pat;               /* Copy BBOX  to box       */
     cone =  p_t->cone_pat;              /* Copy BCONE to cone      */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur913 box  Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box.xmin,   box.ymin,   box.zmin,
         box.xmax,   box.ymax,   box.zmax );
}
#endif

   if  ( box.flag == -99  )
     {
     *p_nbadb = *p_nbadb + 1;
     goto badpat;
     }
   if  ( box.xmin <  xmin ) xmin = box.xmin;
   if  ( box.xmax >  xmax ) xmax = box.xmax;
   if  ( box.ymin <  ymin ) ymin = box.ymin;
   if  ( box.ymax >  ymax ) ymax = box.ymax;
   if  ( box.zmin <  zmin ) zmin = box.zmin;
   if  ( box.zmax >  zmax ) zmax = box.zmax;

   if  ( cone.flag == -99 || cone.flag == -1 )
     {
     *p_nbadc = *p_nbadc + 1;
     goto badpat;
     }
   if  ( cone.ang > *p_angmax ) *p_angmax = cone.ang;

badpat:; /* Label: Bad patch                                        */

    }                           /* End   loop U patches             */
  }                             /* End   loop V patches             */
/*!                                                                 */
/*  End  all U patches  i_u= 1, 2,  ..., n_u                        */
/* End  all V patches  i_v= 1, 2,  ..., n_v                         */
/*                                                                 !*/

   if ( *p_nbadb < n_u*n_v )
     {
     p_bbox->xmin = xmin;
     p_bbox->xmax = xmax;
     p_bbox->ymin = ymin;
     p_bbox->ymax = ymax;
     p_bbox->zmin = zmin;
     p_bbox->zmax = zmax;
     p_bbox->flag =   1;
     }
   else
     {
     p_bbox->flag = -99;
     }

   if ( *p_nbadc < n_u*n_v )
     {
     ;
     }
   else
     {
/*  1997-12-01     p_bbox->flag = -1; */
     p_bcone->flag = -1;
     }


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur913  *p_nbadb %d *p_nbadc %d *p_angmax %9.4f \n" , 
        (int)*p_nbadb,(int)*p_nbadc,*p_angmax);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur913 Exit xmin %6.1f xmax %6.1f ymin %6.1f ymax %6.1f zmin %6.1f zmax %6.1f\n"
           , xmin,      xmax ,     ymin ,     ymax ,     zmin ,     zmax);
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


