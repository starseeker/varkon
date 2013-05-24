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
/*  Function: varkon_pat_trabound                  File: sur916.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function transformates the bounding box and the bounding    */
/*  direction cone for a surface patch.                             */
/*                                                                  */
/*  The box BBOX should be recomputed. The box will always become   */
/*  bigger with the used method in this function. But recomputing   */
/*  boxes is a heavy computation. Should be a parameter like the    */
/*  tolerances in the future. The user (application) must decide    */
/*  the optimal solution. Heavy transformation calculations and     */
/*  fewer patches ......                                            */
/*                                                                  */
/*                                                                  */
/*  The input box and cone will not be copied if the input and      */
/*  output adresses are equal.                                      */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-05   Originally written                                 */
/*  1995-09-03   Bug BBOX transf theory and ydir and zdir (=xdir)   */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_trabound   Transformate boundary box & cone */
/*                                                              */
/*--------------------------------------------------------------*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush       * Error message to terminal        */
/*                                                                  */
/*----------------------------------------------------------------- */

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_pat_trabound (sur916)    */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus   varkon_pat_trabound (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   BBOX    *p_inpbox,    /* Bounding box  for the patch. In.  (ptr) */
   BCONE   *p_inpcone,   /* Bounding cone for the patch. In.  (ptr) */
   DBTmat  *p_c,         /* Local coordinate system           (ptr) */
   BBOX    *p_outpbox,   /* Bounding box  for the patch. Out. (ptr) */
   BCONE   *p_outpcone ) /* Bounding cone for the patch. Out. (ptr) */
/*                                                                  */
/* Out:                                                             */
/*         Data to p_pbox and p_pcone                               */
/*         Flags = -99 for a bad patch                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

   DBVector p_box[8];    /* Transformed corner points               */
   DBint    i_p;         /* Loop index point                        */
   DBfloat  xmin;        /* Minimum x coordinate   for the patch    */
   DBfloat  ymin;        /* Minimum y coordinate   for the patch    */
   DBfloat  zmin;        /* Minimum z coordinate   for the patch    */
   DBfloat  xmax;        /* Maximum x coordinate   for the patch    */
   DBfloat  ymax;        /* Maximum y coordinate   for the patch    */
   DBfloat  zmax;        /* Maximum z coordinate   for the patch    */
   DBfloat  xdir;        /* Direction x for bounding cone           */
   DBfloat  ydir;        /* Direction y for bounding cone           */
   DBfloat  zdir;        /* Direction z for bounding cone           */
   DBfloat  ang;         /* Cone angle                              */

/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                _____________________                             */
/*               !                     !                            */
/*               ! varkon_pat_trabound !                            */
/*               !      (sur916)       !                            */
/*               !_____________________!                            */
/*         _________________!_________________________              */
/*   _____!_______  ______!_______  ______!_______  __!___          */
/*  !             !!              !!              !!      !         */
/*  !     1       !!      2       !!      3       !!  4   !         */
/*  ! Checks and  !! Transformate !! Transformate !! Exit !         */
/*  ! initiations !!    BBOX      !!    BCONE     !!      !         */
/*  !_____________!!______________!!______________!!______!         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur916 p_inpcone->xdir %f ydir %f zdir %f ang %f flag %d\n", 
        p_inpcone->xdir , p_inpcone->ydir, p_inpcone->zdir ,
        p_inpcone->ang,   p_inpcone->flag );
}
 fflush(dbgfil(SURPAC)); 
#endif

/*                                                                  */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                  */

/*!                                                                 */
/* 1. Transformation of BBOX                                        */
/* __________________________                                       */
/*                                                                  */
/* Transformate xmin, ymin, ...            (if p_inpbox->flag=1)    */
/*                                                                 !*/

   if      ( p_inpbox->flag ==  1  )
      {
      p_box[0].x_gm= ((*p_c).g11 * (*p_inpbox).xmin +  /* Point 1   */
                      (*p_c).g12 * (*p_inpbox).ymin +
                      (*p_c).g13 * (*p_inpbox).zmin +
                      (*p_c).g14) / (*p_c).g44;
      p_box[0].y_gm= ((*p_c).g21 * (*p_inpbox).xmin +
                      (*p_c).g22 * (*p_inpbox).ymin +
                      (*p_c).g23 * (*p_inpbox).zmin +
                      (*p_c).g24) / (*p_c).g44;
      p_box[0].z_gm= ((*p_c).g31 * (*p_inpbox).xmin +
                      (*p_c).g32 * (*p_inpbox).ymin +
                      (*p_c).g33 * (*p_inpbox).zmin +
                      (*p_c).g34) / (*p_c).g44;

      p_box[1].x_gm= ((*p_c).g11 * (*p_inpbox).xmax +  /* Point 2   */
                      (*p_c).g12 * (*p_inpbox).ymin +
                      (*p_c).g13 * (*p_inpbox).zmin +
                      (*p_c).g14) / (*p_c).g44;
      p_box[1].y_gm= ((*p_c).g21 * (*p_inpbox).xmax +
                      (*p_c).g22 * (*p_inpbox).ymin +
                      (*p_c).g23 * (*p_inpbox).zmin +
                      (*p_c).g24) / (*p_c).g44;
      p_box[1].z_gm= ((*p_c).g31 * (*p_inpbox).xmax +
                      (*p_c).g32 * (*p_inpbox).ymin +
                      (*p_c).g33 * (*p_inpbox).zmin +
                      (*p_c).g34) / (*p_c).g44;

      p_box[2].x_gm= ((*p_c).g11 * (*p_inpbox).xmax +  /* Point 3   */
                      (*p_c).g12 * (*p_inpbox).ymax +
                      (*p_c).g13 * (*p_inpbox).zmin +
                      (*p_c).g14) / (*p_c).g44;
      p_box[2].y_gm= ((*p_c).g21 * (*p_inpbox).xmax +
                      (*p_c).g22 * (*p_inpbox).ymax +
                      (*p_c).g23 * (*p_inpbox).zmin +
                      (*p_c).g24) / (*p_c).g44;
      p_box[2].z_gm= ((*p_c).g31 * (*p_inpbox).xmax +
                      (*p_c).g32 * (*p_inpbox).ymax +
                      (*p_c).g33 * (*p_inpbox).zmin +
                      (*p_c).g34) / (*p_c).g44;

      p_box[3].x_gm= ((*p_c).g11 * (*p_inpbox).xmin +  /* Point 4   */
                      (*p_c).g12 * (*p_inpbox).ymax +
                      (*p_c).g13 * (*p_inpbox).zmin +
                      (*p_c).g14) / (*p_c).g44;
      p_box[3].y_gm= ((*p_c).g21 * (*p_inpbox).xmin +
                      (*p_c).g22 * (*p_inpbox).ymax +
                      (*p_c).g23 * (*p_inpbox).zmin +
                      (*p_c).g24) / (*p_c).g44;
      p_box[3].z_gm= ((*p_c).g31 * (*p_inpbox).xmin +
                      (*p_c).g32 * (*p_inpbox).ymax +
                      (*p_c).g33 * (*p_inpbox).zmin +
                      (*p_c).g34) / (*p_c).g44;


      p_box[4].x_gm= ((*p_c).g11 * (*p_inpbox).xmin +  /* Point 5   */
                      (*p_c).g12 * (*p_inpbox).ymin +
                      (*p_c).g13 * (*p_inpbox).zmax +
                      (*p_c).g14) / (*p_c).g44;
      p_box[4].y_gm= ((*p_c).g21 * (*p_inpbox).xmin +
                      (*p_c).g22 * (*p_inpbox).ymin +
                      (*p_c).g23 * (*p_inpbox).zmax +
                      (*p_c).g24) / (*p_c).g44;
      p_box[4].z_gm= ((*p_c).g31 * (*p_inpbox).xmin +
                      (*p_c).g32 * (*p_inpbox).ymin +
                      (*p_c).g33 * (*p_inpbox).zmax +
                      (*p_c).g34) / (*p_c).g44;

      p_box[5].x_gm= ((*p_c).g11 * (*p_inpbox).xmax +  /* Point 6   */
                      (*p_c).g12 * (*p_inpbox).ymin +
                      (*p_c).g13 * (*p_inpbox).zmax +
                      (*p_c).g14) / (*p_c).g44;
      p_box[5].y_gm= ((*p_c).g21 * (*p_inpbox).xmax +
                      (*p_c).g22 * (*p_inpbox).ymin +
                      (*p_c).g23 * (*p_inpbox).zmax +
                      (*p_c).g24) / (*p_c).g44;
      p_box[5].z_gm= ((*p_c).g31 * (*p_inpbox).xmax +
                      (*p_c).g32 * (*p_inpbox).ymin +
                      (*p_c).g33 * (*p_inpbox).zmax +
                      (*p_c).g34) / (*p_c).g44;

      p_box[6].x_gm= ((*p_c).g11 * (*p_inpbox).xmax +  /* Point 7   */
                      (*p_c).g12 * (*p_inpbox).ymax +
                      (*p_c).g13 * (*p_inpbox).zmax +
                      (*p_c).g14) / (*p_c).g44;
      p_box[6].y_gm= ((*p_c).g21 * (*p_inpbox).xmax +
                      (*p_c).g22 * (*p_inpbox).ymax +
                      (*p_c).g23 * (*p_inpbox).zmax +
                      (*p_c).g24) / (*p_c).g44;
      p_box[6].z_gm= ((*p_c).g31 * (*p_inpbox).xmax +
                      (*p_c).g32 * (*p_inpbox).ymax +
                      (*p_c).g33 * (*p_inpbox).zmax +
                      (*p_c).g34) / (*p_c).g44;

      p_box[7].x_gm= ((*p_c).g11 * (*p_inpbox).xmin +  /* Point 8   */
                      (*p_c).g12 * (*p_inpbox).ymax +
                      (*p_c).g13 * (*p_inpbox).zmax +
                      (*p_c).g14) / (*p_c).g44;
      p_box[7].y_gm= ((*p_c).g21 * (*p_inpbox).xmin +
                      (*p_c).g22 * (*p_inpbox).ymax +
                      (*p_c).g23 * (*p_inpbox).zmax +
                      (*p_c).g24) / (*p_c).g44;
      p_box[7].z_gm= ((*p_c).g31 * (*p_inpbox).xmin +
                      (*p_c).g32 * (*p_inpbox).ymax +
                      (*p_c).g33 * (*p_inpbox).zmax +
                      (*p_c).g34) / (*p_c).g44;

      xmin =  50000.0;
      xmax = -50000.0;
      ymin =  50000.0;
      ymax = -50000.0;
      zmin =  50000.0;
      zmax = -50000.0;

      for ( i_p = 0; i_p <= 7; ++i_p )
         {
         if ( p_box[i_p].x_gm < xmin ) xmin = p_box[i_p].x_gm;
         if ( p_box[i_p].x_gm > xmax ) xmax = p_box[i_p].x_gm;
         if ( p_box[i_p].y_gm < ymin ) ymin = p_box[i_p].y_gm;
         if ( p_box[i_p].y_gm > ymax ) ymax = p_box[i_p].y_gm;
         if ( p_box[i_p].z_gm < zmin ) zmin = p_box[i_p].z_gm;
         if ( p_box[i_p].z_gm > zmax ) zmax = p_box[i_p].z_gm;
         }


      }
   else
      {
      p_outpbox->xmin = p_inpbox->xmin;      
      p_outpbox->ymin = p_inpbox->ymin;      
      p_outpbox->zmin = p_inpbox->zmin;      
      p_outpbox->xmax = p_inpbox->xmax;      
      p_outpbox->ymax = p_inpbox->ymax;      
      p_outpbox->zmax = p_inpbox->zmax;      
      p_outpbox->flag = p_inpbox->flag;      
      goto nobox; 
      }

/*!                                                                 */
/* 2. Transformation of BCONE                                       */
/* __________________________                                       */
/*                                                                  */
/* Transformate xdir, ydir, ...            (if p_inpcone->flag=1)   */
/*                                                                 !*/

   if ( p_inpcone->flag == 1 )
      {
      xdir = ((*p_c).g11 * (*p_inpcone).xdir +
              (*p_c).g12 * (*p_inpcone).ydir +
              (*p_c).g13 * (*p_inpcone).zdir ) / (*p_c).g44;

      ydir = ((*p_c).g21 * (*p_inpcone).xdir +
              (*p_c).g22 * (*p_inpcone).ydir +
              (*p_c).g23 * (*p_inpcone).zdir ) / (*p_c).g44;

      zdir = ((*p_c).g31 * (*p_inpcone).xdir +
              (*p_c).g32 * (*p_inpcone).ydir +
              (*p_c).g33 * (*p_inpcone).zdir ) / (*p_c).g44;
      ang  = (*p_inpcone).ang;
      }
   else
      {
      p_outpcone->xdir = p_inpcone->xdir;      
      p_outpcone->ydir = p_inpcone->ydir;      
      p_outpcone->zdir = p_inpcone->zdir;      
      p_outpcone->flag = p_inpcone->flag;      
      goto nocone;
      }


/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Data to output parameters p_pbox and p_pcone                     */
/*                                                                 !*/

   p_outpcone->xdir = xdir;  
   p_outpcone->ydir = ydir;  
   p_outpcone->zdir = zdir;  
   p_outpcone->ang  = ang;    
   p_outpcone->flag = 1;      


/*!                                                                 */
nocone: /* Label: No cone transformation  ( -> flag is not = 1 )    */
/*                                                                 !*/

   p_outpbox->xmin = xmin;
   p_outpbox->ymin = ymin;
   p_outpbox->zmin = zmin;
   p_outpbox->xmax = xmax;
   p_outpbox->ymax = ymax;
   p_outpbox->zmax = zmax;
   p_outpbox->flag = 1;

/*!                                                                 */
nobox:  /* Label: No box  transformation  ( -> flag is not = 1 )    */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur916 p_outpbox->flag %d p_outpcone->flag %d\n",
          p_outpbox->flag,   p_outpcone->flag  );
  }
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur916 p_outpbox->xmin %f ymin %f zmin %f flag %d\n", 
        p_outpbox->xmin , p_outpbox->ymin, p_outpbox->zmin ,
        p_outpbox->flag );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur916 p_outpbox->xmax %f ymax %f zmax %f flag %d\n", 
        p_outpbox->xmax , p_outpbox->ymax, p_outpbox->zmax ,
        p_outpbox->flag );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur916 p_outpcone->xdir %f ydir %f zdir %f ang %f flag %d\n", 
        p_outpcone->xdir , p_outpcone->ydir, p_outpcone->zdir ,
        p_outpcone->ang,   p_outpcone->flag );
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
