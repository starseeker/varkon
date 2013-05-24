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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_sur_hardptcre1                File: sur260.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a hard point (quadratic) curve segment     */
/*  by parabola interpolation of three conic hard points.           */
/*  Input to the function are three rational quadratic              */
/*  segments.                                                       */
/*                                                                  */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-26   Originally written                                 */
/*  1996-05-28   Eliminated compilation warning                     */
/*  1997-02-09   Eliminated compilation warning                     */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_hardptcre1 Create parabola hardpoint segment*/
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short ctoconp();       /* Coefficients to Consurf polygon    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_seg_parabola    * Parabola interpolat. of 3 pts           */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_hardptcre1*/
/* SU2993 = Program error in varkon_sur_hardptcre1 (sur260). Report!*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_sur_hardptcre1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_u0,         /* Pointer to boundary segment U= 0.0      */
   DBSeg  *p_um,         /* Pointer to middle   segment U= 0.5      */
   DBSeg  *p_u1,         /* Pointer to boundary segment U= 1.0      */
   DBSeg  *p_hp )        /* Pointer to output hard point segment    */

/* Out:                                                             */
/*       Data to p_hp                                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   HOMOP u01,u02,u03,u04;/* The Consurf Polygon for segment p_u0    */
   HOMOP um1,um2,um3,um4;/* The Consurf Polygon for segment p_um    */
   HOMOP u11,u12,u13,u14;/* The Consurf Polygon for segment p_u1    */
   DBVector u0hp;        /* The hard point for p_u0                 */
   DBVector umhp;        /* The hard point for p_um                 */
   DBVector u1hp;        /* The hard point for p_u1                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  u_case;        /* Parameterisation case:                  */
                         /* Eq. 1: Not yet programmed ...           */
                         /* Eq. 2: Mid point for u= 0.5             */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*!----------------- Theory ----------------------------------------*/
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
"sur260 Enter *** varkon_sur_hardptcre1 p_u0 %d p_um %d p_u1 %d \n",
                (int)p_u0, (int)p_um, (int)p_u1 );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


/*!                                                                 */
/* 1. Consurf polygons for input segments                           */
/* ______________________________________                           */

/* Calculate the Consurf polygons                                   */
/* Call of ctoconp for each segment      Error SU2973 for failure.  */
/* Polygon u01, u02, u03, u04 for p_u0                              */
/* Polygon um1, um2, um3, um4 for p_um                              */
/* Polygon u11, u12, u13, u14 for p_u1                              */
/*                                                                 !*/

   status=ctoconp(p_u0 , &u01, &u02, &u03, &u04 );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(u0)%%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=ctoconp(p_um , &um1, &um2, &um3, &um4 );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(um)%%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2973",errbuf));
   }

   status=ctoconp(p_u1 , &u11, &u12, &u13, &u14 );
   if (status<0) 
   {
   sprintf(errbuf,"ctoconp(u1)%%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2973",errbuf));
   }

/*!                                                                 */
/* 2. Hard points in cartesian coordinates                          */
/* _______________________________________                          */

/* Let u0hp= (u02.X/u02.w , u02.Y/u02.w , u02.Z/u02.w)              */
/*                                                                 !*/


   u0hp.x_gm= u02.X / u02.w;
   u0hp.y_gm= u02.Y / u02.w;
   u0hp.z_gm= u02.Z / u02.w;

   umhp.x_gm= um2.X / um2.w;
   umhp.y_gm= um2.Y / um2.w;
   umhp.z_gm= um2.Z / um2.w;

   u1hp.x_gm= u12.X / u12.w;
   u1hp.y_gm= u12.Y / u12.w;
   u1hp.z_gm= u12.Z / u12.w;




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur260 u0hp   %f %f %f \n",
  u0hp.x_gm , u0hp.y_gm , u0hp.z_gm );
fprintf(dbgfil(SURPAC),
"sur260 umhp   %f %f %f \n",
  umhp.x_gm , umhp.y_gm , umhp.z_gm );
fprintf(dbgfil(SURPAC),
"sur260 u1hp   %f %f %f \n",
  u1hp.x_gm , u1hp.y_gm , u1hp.z_gm );
  fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Create the hard point curve segment                           */
/* ______________________________________                           */

/* Interpolate points u0hp, umhp and u1hp with a parabola.          */
/* Call of varkon_seg_parabola (sur700)                             */
/*                                                                 !*/

   u_case = 1;

status=varkon_seg_parabola(u0hp,umhp,u1hp , u_case, p_hp);
if (status<0) 
    {
    sprintf(errbuf,"sur700%%varkon_sur_hardptcre1 (sur260)");
    return(varkon_erpush("SU2943",errbuf));
    }


    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

/*!********* Internal ** function ***********************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function converts the coefficients of a rational cubic       */
/* segment to the Consur polygon.                                   */

   static short ctoconp (p_seg, p1, p2, p3, p4 )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_seg;        /* Pointer to a rational cubic segment     */

/* Out:                                                             */
                         /* The Consurf Polygon for a curve segment */
   HOMOP *p1;            /* P1 = (X1w1,Y1w1,Z1w1,w1)                */
   HOMOP *p2;            /* P2 = (X1w2,Y1w2,Z1w2,w2)                */
   HOMOP *p3;            /* P3 = (X1w3,Y1w3,Z1w3,w3)                */
   HOMOP *p4;            /* P4 = (X4w4,Y4w4,Z4w4,w4)                */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

#ifdef  DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur260 *** ctoconp: p_seg %d \n", (int)p_seg );
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!                                                                 */
/* 2. Convert from coefficients to polygon                          */
/* _______________________________________                          */
/*                                                                 !*/

/*! Point P1 =        c0                                           !*/
/*  --------                                                        */

    p1->X    = p_seg->c0x;
    p1->Y    = p_seg->c0y;
    p1->Z    = p_seg->c0z;
    p1->w    = p_seg->c0;

/*! Point P2 = (       c1  +        2 * c0 )/ 2                    !*/
/*  --------                                                        */

    p2->X    = (p_seg->c1x + 2.0*p_seg->c0x)/2.0;
    p2->Y    = (p_seg->c1y + 2.0*p_seg->c0y)/2.0;
    p2->Z    = (p_seg->c1z + 2.0*p_seg->c0z)/2.0;
    p2->w    = (p_seg->c1  + 2.0*p_seg->c0 )/2.0;

/*! Point P3 = (       c1  +        2 * c0 -       c3 )/ 2         !*/
/*  --------                                                        */

    p3->X    = (p_seg->c1x + 2.0*p_seg->c0x-p_seg->c3x)/2.0;
    p3->Y    = (p_seg->c1y + 2.0*p_seg->c0y-p_seg->c3y)/2.0;
    p3->Z    = (p_seg->c1z + 2.0*p_seg->c0z-p_seg->c3z)/2.0;
    p3->w    = (p_seg->c1  + 2.0*p_seg->c0 -p_seg->c3 )/2.0;

/*! Point P4 =        c3  +        c2  +        c1  +        c0    !*/
/*  --------                                                        */

    p4->X    = p_seg->c3x + p_seg->c2x + p_seg->c1x + p_seg->c0x;
    p4->Y    = p_seg->c3y + p_seg->c2y + p_seg->c1y + p_seg->c0y;
    p4->Z    = p_seg->c3z + p_seg->c2z + p_seg->c1z + p_seg->c0z;
    p4->w    = p_seg->c3  + p_seg->c2  + p_seg->c1  + p_seg->c0;

/*!                                                                 */
/* 3. Check weights for Debug On                                    */
/* _____________________________                                    */


/* Check 1: Weights must be greater than zero.                      */
/* Check 2: For a conic must weights p1.w and p4.w be equal         */
/* Check 3: For a conic must weights p2.w and p3.w be equal         */
/*                                                                 !*/

#ifdef DEBUG
   if (p1->w < 0.000001 )
   {
   sprintf(errbuf,"p1.w < 0 %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p2->w < 0.000001 )
   {
   sprintf(errbuf,"p2.w < 0 %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p3->w < 0.000001 )
   {
   sprintf(errbuf,"p3.w < 0 %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (p4->w < 0.000001 )
   {
   sprintf(errbuf,"p4.w < 0 %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (fabs(p1->w - p4->w) > 0.000001 )
   {
   sprintf(errbuf,"p1.w ne p4.w %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
   if (fabs(p2->w - p3->w) > 0.000001 )
   {
   sprintf(errbuf,"p2.w ne p3.w %%varkon_sur_hardptcre1 (sur260)");
   return(varkon_erpush("SU2993",errbuf));
   }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur260*ctoconp P1 = %f %f %f %f \n",
  p1->X/p1->w, p1->Y/p1->w, p1->Z/p1->w, p1->w);
fprintf(dbgfil(SURPAC),
"sur260*ctoconp P2 = %f %f %f %f \n",
  p2->X/p2->w, p2->Y/p2->w, p2->Z/p2->w, p2->w);
fprintf(dbgfil(SURPAC),
"sur260*ctoconp P3 = %f %f %f %f \n",
  p3->X/p3->w, p3->Y/p3->w, p3->Z/p3->w, p3->w);
fprintf(dbgfil(SURPAC),
"sur260*ctoconp P4 = %f %f %f %f \n",
  p4->X/p4->w, p4->Y/p4->w, p4->Z/p4->w, p4->w);
  fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

