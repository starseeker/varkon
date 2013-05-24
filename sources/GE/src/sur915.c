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
/*  Function: varkon_cur_bound                     File: sur915.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the bounding box for a curve.           */
/*                                                                  */
/*  The bounding boxes may be used by intersection functions        */
/*  to determine if an intersect between objects exist, or          */
/*  to find out the extreme values for a UV curve (a loop in        */
/*  a face) and at the same time check that the Z values are zero.  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-29   Originally written                                 */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_bound      Boundary box and cone for curve  */
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
/* varkon_ini_bbox      * Initialize BBOX                           */
/* varkon_normv         * Normalisation of a vector                 */
/* varkon_erinit        * Initialize error message stack            */
/* varkon_erpush        * Error message to terminal                 */
/* varkon_ini_evalc     * Initialize EVALC for Debug On             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_cur_bound (sur915)  */
/* SU2993 = Severe program error in varkon_cur_bound (sur915).      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
            DBstatus varkon_cur_bound (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   DBCurve *p_cur,       /* Current curve                     (ptr) */
   DBSeg   *p_seg,       /* Coefficients for curve segments   (ptr) */
   BBOX    *p_cbox )     /* Bounding box for the curve        (ptr) */
/*                                                                  */
/* Out:                                                             */
/*         Data to p_cbox                                           */
/*         Flags = -99 for a bad curve                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

   DBfloat  xmin;          /* Minimum x coordinate   for the curve    */
   DBfloat  ymin;          /* Minimum y coordinate   for the curve    */
   DBfloat  zmin;          /* Minimum z coordinate   for the curve    */
   DBfloat  xmax;          /* Maximum x coordinate   for the curve    */
   DBfloat  ymax;          /* Maximum y coordinate   for the curve    */
   DBfloat  zmax;          /* Maximum z coordinate   for the curve    */

   DBint  n_add;         /* Number of points per curve segment      */
   DBint  istart;        /* Start segment                           */
   DBint  iend;          /* End   segment                           */
   DBint  i_seg;         /* Loop index segment in curve             */
   DBint  i_poi;         /* Loop index point in segment             */

   EVALC  xyz;           /* Coordinates and derivatives for crv pt  */
   DBfloat  delta;         /* Delta parameter value                   */

   DBint  n_tot;         /* Total number of points                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/*!New-Page--------------------------------------------------------!*/

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                __________________                                */
/*               !                  !                               */
/*               ! varkon_cur_bound !                               */
/*               !    (sur915)      !                               */
/*               !__________________!                               */
/*         _______________!___________________________________      */
/*   _____!_______  ___________!____________                __!___  */
/*  !             !!                        !              !      ! */
/*  !     1       !!           2            !              !  4   ! */
/*  ! Checks and  !!  Min. and max. values  !              ! Exit ! */
/*  ! initiations !!                        !              !      ! */
/*  !_____________!!________________________!              !______! */
/*                             !                                    */
/*                             !                                    */
/*                             !                                    */
/*                             !                                    */
/*                             !                                    */
/*                             !                                    */
/*       ______________________!                                    */
/*  ____!_____   __________!__________                              */
/* !          ! !                   * !                             */
/* ! Initiate ! ! U values for points !                             */
/* !   loop   ! !_____________________!                             */
/* ! variabl. !            !                                        */
/* !__________!            !                                        */
/*                         !                                        */
/*                         !                                        */
/*               __________!__________                              */
/*              !                     !                             */
/*              !  varkon_cur_eval    !                             */
/*              !      (geo911)       !                             */
/*              !_____________________!                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur915 Enter* Bounding box for a curve\n");
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                  */
/* Initialize EVALC for DEBUG On                                    */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

#ifdef  DEBUG
      status=varkon_ini_evalc (&xyz);
#endif

   xyz.evltyp   = EVC_R;


/*!                                                                 */
/*    Initialize variabel BBOX                                      */
/*    Call of varkon_ini_bbox  (sur772).                            */
/*                                                                 !*/

      varkon_ini_bbox (p_cbox);


/* Initialize the number of points                                  */
   n_tot = 0;

/* Number of points in per segment                                  */
   n_add = 5;

/* Delta value corresponding to n_add                               */
   delta = 1.0/(1+n_add);

/* All segments of the curve                                        */
   istart = 1;
   iend   = p_cur->ns_cu;


/* Initialize coordinate extreme values                             */
   xmax  =  -500000.0; 
   ymax  =  -500000.0;
   zmax  =  -500000.0;
   xmin  =   500000.0;
   ymin  =   500000.0;
   zmin  =   500000.0;


/*!                                                                 */
/* 2. Calculate the bounding box                                    */
/* ______________________________                                   */
/*                                                                  */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
      for ( i_poi = 1; i_poi <= n_add; i_poi = i_poi + 1 )
        {
/*!                                                                 */
/*    Curve evaluation for i_poi in segment i_seg                   */
/*    Call of varkon_GE109 (GE109).                               */
/*                                                                 !*/

/*    Current parameter value                                       */

      xyz.t_global = (DBfloat)i_seg + (i_poi-1)*delta;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur915 i_seg %d Current parameter value xyz.t_global %25.15f\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        p_cbox->flag  = -99;
        sprintf(errbuf,"GE109%%varkon_cur_averplan");
        return(varkon_erpush("SU2943",errbuf));
        }

/*   An additional point                                            */

     n_tot    = n_tot + 1;


/*!                                                                 */
/*   Compare current maximum and minimum coordinates                */
/*                                                                 !*/
     if ( xyz.r.x_gm > xmax )
       {
       xmax = xyz.r.x_gm;
       }
     if ( xyz.r.y_gm > ymax )
       {
       ymax = xyz.r.y_gm;
       }
     if ( xyz.r.z_gm > zmax )
       {
       zmax = xyz.r.z_gm;
       }

     if ( xyz.r.x_gm < xmin )
       {
       xmin = xyz.r.x_gm;
       }
     if ( xyz.r.y_gm < ymin )
       {
       ymin = xyz.r.y_gm;
       }
     if ( xyz.r.z_gm < zmin )
       {
       zmin = xyz.r.z_gm;
       }


        }/* End loop segment points i_poi= 1,2,3                    */
      }  /* End loop curve segments i_seg= 1,2,3                    */

    if ( n_tot == 0 )
      {
      sprintf(errbuf,"n_tot = 0%%sur915" );
       return(varkon_erpush("SU2993",errbuf));
   }


/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Data to output parameter p_cbox                                  */
/*                                                                 !*/

   p_cbox->xmin = xmin;
   p_cbox->ymin = ymin;
   p_cbox->zmin = zmin;
   p_cbox->xmax = xmax;
   p_cbox->ymax = ymax;
   p_cbox->zmax = zmax;

   p_cbox->flag =  I_UNDEF;          



#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur915 Exit xmin %f xmax %f\n", xmin, xmax);
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/
