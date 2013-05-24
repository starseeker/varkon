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
/*  Function: varkon_pat_chebound                  File: sur914.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function determines if there is any intersection            */
/*  (solution) between the bounding box and bounding cone           */
/*  for a geometry object (surface patch) and an intersection       */
/*  plane, etc.                                                     */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-29   Originally written                                 */
/*  1995-06-06   LINTSUR added                                      */
/*  1995-08-26   LINTSUR check added                                */
/*  1995-09-03   LINTSUR check changed and direction added          */
/*  1995-09-16   LINTSUR transform patch. pat_tra added             */
/*  1995-10-23   ctol added for lintsur                             */
/*  1995-11-26   debug added for lintsur                            */
/*  1996-04-29   ctol     for surface/plane                         */
/*  1996-11-15   -99 for bad normals, compilation warnings          */
/*  1998-04-27   Debug and comments added                           */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_chebound   Check intersect. box and plane,. */
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
/*           varkon_pat_trabound    * Transform BBOX and BCONE      */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_pat_chebound (sur914).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_pat_chebound (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_patr,      /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   DBfloat  u_s,         /* U start point          for patch        */
   DBfloat  v_s,         /* V start point          for patch        */
   DBfloat  u_e,         /* U end   point          for patch        */
   DBfloat  v_e,         /* V end   point          for patch        */
   BBOX    *p_pbox,      /* Bounding box  for the patch       (ptr) */
   BCONE   *p_pcone,     /* Bounding cone for the patch       (ptr) */
   DBint   *p_sflag )    /* Flag from checking                (ptr) */
                         /* Eq. -1: No solution Eq. 1: Solution     */
                         /* Eq.-99: Patch is not OK                 */
/* Out:                                                             */
/*         Data to p_sflag                                          */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

   DBint    ctype;       /* Case of computation                     */
   DBfloat  d_crit;      /* A criterion for intersection            */
   DBfloat  d_critstart; /* A criterion for intersection            */
   DBfloat  pi_x;        /* X component for intersect plane         */
   DBfloat  pi_y;        /* Y component for intersect plane         */
   DBfloat  pi_z;        /* Z component for intersect plane         */
   DBfloat  pi_d;        /* D component for intersect plane         */

   DBint    scase;       /* Intersect selection case                */
   DBTmat  pat_tra;      /* Transformation matrix for patches       */
   BBOX    pbox_tra;     /* Bounding box  for the patch, transform. */
   BCONE   pcone_tra;    /* Bounding cone for the patch, transform. */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  ctol;        /* Coordinate tolerance                    */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!------------------ Theory ---------------------------------------*/
/*                                                                  */
/* The bounding box is defined by coordinates:                      */
/* xmin, ymin, zmin, xmax, ymax, zmax                               */
/*                                                                  */
/* Planar intersection case                                         */
/* ------------------------                                         */
/*                                                                  */
/* The intersection plane is defined by:                            */
/* pi_x, pi_y, pi_z, pi_d                                           */
/*                                                                  */
/* A parallell plane at distance d is defined by:                   */
/*                                                                  */
/* pi_x*X + pi_y*Y + pi_z*Z = pi_d + d                              */
/*                                                                  */
/* Positive d: Plane in the positive direction of the plane normal  */
/* Negative d: Plane in the negative direction of the plane normal  */
/*                                                                  */
/* For each corner point is a parallell defined and the             */
/* distance d (called d_crit) is calculated. Example:               */
/*                                                                  */
/* d_crit = pi_x*xmin + pi_y*ymin + pi_z*zmin - pi_d                */
/*                                                                  */
/* The criterion for a possible intersect is that the input         */
/* plane is between at least two of the planes. This is the         */
/* case if the distances d_crit for the box corner points           */
/* have different signs.                                            */
/*                                                                  */
/* The first distance d_crit is assigned to d_critstart and         */
/* the criterion for an intersect is that any of the other          */
/* distances has another sign.                                      */
/*                                                                  */
/*                                                                  */
/* Line/surface intersect case                                      */
/* ---------------------------                                      */
/*                                                                  */
/* The input bounding box is transformed to a local coordinate      */
/* system, where the X axis is equal to the line.                   */
/* Refer to sur916 for a description of the transformation, which   */
/* not only is a transformation. A new BBOX in the local system     */
/* will be created.                                                 */
/*                                                                  */
/* The criterion for a possible hit of the transformed BBOX is      */
/* that a point on the line is inside the YZ view of the BBOX       */
/* in the local system, i.e. a rectangle.                           */
/*                                                                  */
/* The point on the line is the origin (Y=0,Z=0) in the local       */
/* system and within the rectangle is equal to that the             */
/* minimum Y and Z values of the BBOX is less than zero and that    */
/* the maximum values are greater than zero. A tolerance is added   */
/* or subtracted to zero.                                           */
/*                                                                  */
/* For cases that the output points must be in the shooting         */
/* direction are also the maximum and minimum BBOX values used      */
/* If both values are positive in the local system will the         */
/* intersect points be in the shooting direction.                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                 */

/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 2. Determine if there is an intersection                         */
/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
   *p_sflag = -1;        /* Eq. -1: No solution Eq. 1: Solution     */
/*                                                                 !*/

/*!                                                                 */
/* Goto nogood if input p_pbox->flag= -99 (patch not OK)            */
/*                                                                 !*/

   if ( p_pbox->flag == -99 ) 
      {
      *p_sflag = -99;    /* The surface patch is not OK             */
      goto nogood;
      }

   if ( p_pbox->flag <= 0 ) 
      {
      sprintf(errbuf,"pbox_flag%%varkon_pat_chebound (sur914)");
      return(varkon_erpush("SU2993",errbuf));
      }

   if      ( p_pcone->flag == -99 ) 
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur914 p_pcone->flag= %d (Surface normals not OK)\n",
        p_pcone->flag   );
fflush(dbgfil(SURPAC));
}
#endif
      *p_sflag = -99;    /* The surface patch normals are not OK    */
      goto nogood;
      }
/*!                                                                 */
/*  Retrieve intersect plane for computation   type SURPLAN         */
/*  Let p_sflag be one (1) and goto notimp for type F_SILH          */
/*  Let p_sflag be one (1) and goto notimp for type I_SILH          */
/*  Let p_sflag be one (1) and goto notimp for type S_SILH          */
/*  Let p_sflag be one (1) and goto notimp for type F_ISOP          */
/*  Let p_sflag be one (1) and goto notimp for type I_ISOP          */
/*  Let p_sflag be one (1) and goto notimp for type CLOSEPT         */
/*  Let p_sflag be one (1) and goto notimp for type SURAPPR         */
/*  Let p_sflag be one (1) and goto notimp for type BICAPPR         */
/*  Let p_sflag be one (1) and goto notimp for type LINTSUR         */
/*                                                                 !*/
   ctype   = p_comp->ipl_un.ctype;

   if      ( ctype  == SURPLAN )
       {
       pi_x = p_comp->ipl_un.in_x;
       pi_y = p_comp->ipl_un.in_y;
       pi_z = p_comp->ipl_un.in_z;
       pi_d = p_comp->ipl_un.in_d;
       ctol = p_comp->pro_un.ctol;    /* 1996-04-27 */
       }
   else if ( ctype  == F_SILH  )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == I_SILH  )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == S_SILH  )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == F_ISOP  )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == I_ISOP  )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == CLOSEPT )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == SURAPPR )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == BICAPPR )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       goto notimp;             /* Not yet implemented             */
       }
   else if ( ctype  == LINTSUR )
       {
       *p_sflag =  1;           /* Solution "exists"               */
       scase   = p_comp->pro_un.scase;
       pat_tra = p_comp->pro_un.pat_tra; /* 1995-09-16 */
       ctol    = p_comp->pro_un.ctol;    /* 1995-10-23 */
       goto lintsur;            /* Analyse                         */
       }
   else
       {
       sprintf(errbuf,"ctype%%varkon_pat_chebound (sur914)");
       return(varkon_erpush("SU2993",errbuf));
       }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 pi_x %f pi_y %f pi_z %f pi_d %f ctol %f\n",
   pi_x,pi_y,pi_z,pi_d,  ctol );
fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
       p_pbox->xmin,p_pbox->ymin,p_pbox->zmin,
       p_pbox->xmax,p_pbox->ymax,p_pbox->zmax );
fflush(dbgfil(SURPAC));
  }
#endif

/*!                                                                 */
/* 2. Determine if there is an intersection                         */
/* ________________________________________                         */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* d_crit for xmin, ymin, zmin and d_critstart                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmin + 
            pi_y*p_pbox->ymin + 
            pi_z*p_pbox->zmin - 
            pi_d;  

   d_critstart = d_crit;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 1 xmin,ymin,zmin d_crit %10.6f  d_critstart %10.6f\n",
       d_crit , d_critstart);
  }
#endif

   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmax, ymin, zmin                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmax + 
            pi_y*p_pbox->ymin + 
            pi_z*p_pbox->zmin - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 2 xmax,ymin,zmin d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmin, ymax, zmin                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmin + 
            pi_y*p_pbox->ymax + 
            pi_z*p_pbox->zmin - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 3 xmin,ymax,zmin d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmin, ymin, zmax                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmin + 
            pi_y*p_pbox->ymin + 
            pi_z*p_pbox->zmax - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 4 xmin,ymin,zmax d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmax, ymax, zmin                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmax + 
            pi_y*p_pbox->ymax + 
            pi_z*p_pbox->zmin - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 5 xmax,ymax,zmin d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmax, ymax, zmax                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmax + 
            pi_y*p_pbox->ymax + 
            pi_z*p_pbox->zmax - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 6 xmax,ymax,zmax d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmax, ymin, zmax                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmax + 
            pi_y*p_pbox->ymin + 
            pi_z*p_pbox->zmax - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 7 xmax,ymin,zmax d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmin, ymax, zmax                                      */
/*                                                                 !*/

   d_crit = pi_x*p_pbox->xmin + 
            pi_y*p_pbox->ymax + 
            pi_z*p_pbox->zmax - 
            pi_d;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Vertex 8 xmin,ymax,zmax d_crit %10.6f\n",
       d_crit);
fflush(dbgfil(SURPAC));
  }
#endif

   if ( d_critstart < 0  && d_crit >= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( d_critstart > 0  && d_crit <= 0.0 )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }
   if ( fabs(d_crit) <= ctol )
      {
      *p_sflag = 1;      /* Eq. -1: No solution Eq. 1: Solution     */
      goto solut;
      }

/*!                                                                 */
/* d_crit for xmin, ymax, zmax                                      */
/*                                                                 !*/

  goto endche;

lintsur:; /*! Label lintsur: Surface line intersect                !*/

/*!                                                                 */
/*     Transformation of BBOX and BCONE.                            */
/*     Call of varkon_pat_trabound (sur916).                        */
/*                                                                 !*/

status=varkon_pat_trabound
(p_pbox, p_pcone, &pat_tra , &pbox_tra , &pcone_tra );
if (status<0) 
   {
   sprintf(errbuf,"sur916(pbox,pcone)%%varkon_sur_bictra (sur600)");
   return(varkon_erpush("SU2943",errbuf));
   }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Patch %d %d ymin %8.2f ymax %8.2f zmin %8.2f zmax %8.2f\n",
          (int)u_s, (int)v_s, pbox_tra.ymin-ctol,pbox_tra.ymax+ctol,
                                  pbox_tra.zmin-ctol,pbox_tra.zmax+ctol );
fflush(dbgfil(SURPAC));
  }

if ( pbox_tra.ymin-ctol    <= 0.0 && pbox_tra.ymax+ctol    >= 0.0 && 
     pbox_tra.zmin-ctol    <= 0.0 && pbox_tra.zmax+ctol    >= 0.0    )
{
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Possible line/BBOX intersect u_s %5.2f v_s %5.2f u_e %5.2f v_e %5.2f\n",
                              u_s, v_s, u_e, v_e );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Possible line/BBOX intersect in patch IU= %d IV= %d \n",
          (int)u_s, (int)v_s );
  }
}

if ( pbox_tra.ymin-ctol    >  0.0   )
{
if ( dbglev(SURPAC) == 1 && fabs(pbox_tra.ymin) < 1.0 )
{
fprintf(dbgfil(SURPAC),
"sur914 No line/BBOX intersect u_s %5.2f v_s %5.2f u_e %5.2f v_e %5.2f ymin %10.4f > 0\n",
                             u_s, v_s, u_e, v_e , pbox_tra.ymin);
}
}

if ( pbox_tra.ymax+ctol    <  0.0   )
{
if ( dbglev(SURPAC) == 1 && fabs(pbox_tra.ymax) < 1.0 )
{
fprintf(dbgfil(SURPAC),
"sur914 No line/BBOX intersect u_s %5.2f v_s %5.2f u_e %5.2f v_e %5.2f ymax %10.4f < 0\n",
                             u_s, v_s, u_e, v_e , pbox_tra.ymax);
fprintf(dbgfil(SURPAC),
"sur914 No line/BBOX intersect ymax+ctol %25.18f < 0\n", pbox_tra.ymax+ctol);
}
}


if ( pbox_tra.zmin-ctol    >  0.0   )
{
if ( dbglev(SURPAC) == 1 && fabs(pbox_tra.zmin) < 1.0 )
{
fprintf(dbgfil(SURPAC),
"sur914 No line/BBOX intersect u_s %5.2f v_s %5.2f u_e %5.2f v_e %5.2f zmin %10.4f > 0\n",
                             u_s, v_s, u_e, v_e , pbox_tra.zmin);
}
}

if ( pbox_tra.zmax+ctol    <  0.0   )
{
if ( dbglev(SURPAC) == 1 && fabs(pbox_tra.zmax) < 1.0 )
{
fprintf(dbgfil(SURPAC),
"sur914 No line/BBOX intersect u_s %5.2f v_s %5.2f u_e %5.2f v_e %5.2f zmax %10.4f < 0\n",
                             u_s, v_s, u_e, v_e , pbox_tra.zmax);
}
}

#endif

  if ( pbox_tra.ymin-ctol    <= 0.0 && pbox_tra.ymax+ctol    >= 0.0 && 
       pbox_tra.zmin-ctol    <= 0.0 && pbox_tra.zmax+ctol    >= 0.0    )
    {
    /* There might be a solution */
    }
  else
    {  /* No intersect in this box */
    *p_sflag = -1;      /* Eq. -1: No solution Eq. 1: Solution     */
    goto endche;
    }

#ifdef DEBUG
if ( scase == 2 || scase == 3 || scase == 4 || scase == 5 || scase == 6  )
{
if ( dbglev(SURPAC) == 1 )
  {
  if ( pbox_tra.xmin <= 0.0  &&  pbox_tra.xmax <= 0.0 )
     {
     fprintf(dbgfil(SURPAC),
     "sur914 No intersection xmin %10.4f and xmax %10.4f <=0 for scase %d\n",
             pbox_tra.xmin, pbox_tra.xmax , (int)scase );
fflush(dbgfil(SURPAC));
     }
  }
if ( dbglev(SURPAC) == 2 )
  {
  if ( pbox_tra.xmin >  0.0  &&  pbox_tra.xmax >  0.0 )
     {
     fprintf(dbgfil(SURPAC),
     "sur914 Possible intersection xmin %10.4f and/or xmax %10.4f > 0 for scase %d\n",
             pbox_tra.xmin, pbox_tra.xmax , (int)scase );
     }
  }
} /* End scase */
#endif

  if ( scase == 2 || scase == 3 || scase == 4 || scase == 5 || scase == 6  )
     { /* Solution only in the shooting direction (all the above cases) */
     if ( pbox_tra.xmin <= 0.0  &&  pbox_tra.xmax <= 0.0 )
       { /* No solution if X extreme values are less or equal zero */
       *p_sflag = -1;      /* Eq. -1: No solution Eq. 1: Solution     */
       goto endche;
       }
     }

#ifdef NOT_YET_IMPLEMENTED 
/*
For scase 3 and 4 can surface normals be compared with the X axis (the
shooting direction. It is not possble to eliminate solutions for scase 5 
and 6 must since intersect points will be compared with respect to the
surface normals. 
Copy angle calculation parts from sur912! Calculate angle between X axis
and the "average" BCONE vector. If
*/
if ( pcone_tra.flag == 1)
{
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Line/BCONE test xdir %10.4f ydir %10.4f zdir %10.4f ang %10.4f flag %d\n",
   pcone_tra.xdir, pcone_tra.ydir, pcone_tra.zdir, pcone_tra.ang, pcone_tra.flag );
  }
if ( dbglev(SURPAC) == 1 && (scase == 3 || scase == 4) )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Line/BCONE test scase=  %d\n", (int)scase );
  }
}
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur914 Line/BBOX intersect u_s %10.4f v_s %10.4f u_e %10.4f v_e %10.4f\n",
                              u_s, v_s, u_e, v_e );
  }
#endif



  goto endche;

/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Data to output parameters p_pbox and p_pcone                     */
/*                                                                  */
notimp: /* Label: Check is not implemented                          */
solut:  /* Label: Solution may exist                                */
nogood: /* Label: The surface patch is not OK                       */
endche: /* Label: End of check                                      */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur914 Exit p_pbox->flag %d p_pcone->flag %d p_sflag= %d \n",
             p_pbox->flag,   p_pcone->flag,  (int)*p_sflag);
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
