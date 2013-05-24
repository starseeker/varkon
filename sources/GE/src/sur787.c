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

/* Compile optimization problem in WIN32 Code OK*/
#ifdef  WIN32
#pragma optimize("g",off)
#endif

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_cur_extidpoint                File: sur787.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the parameter values for an extension   */
/*  of the curve the distance idpoint (identical point criterion).  */
/*                                                                  */
/*  The function is assumed to be used for curve interrogation      */
/*  functions.                                                      */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-16   Originally written                                 */
/*  1996-01-27   ctol->idpoint, initialize, comptol, UV_CUB_SEG         */
/*  1997-02-15   No compile optimization in WIN32                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_extidpoint    Extend idpoint param. values  */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/* Reference: Computational Geometry for Design and Manufacture     */
/*            Faux & Pratt,  page 95-96                             */
/*                                                                  */
/* Taylors theorem:                                                 */
/* _        _           _                                           */
/* r(u+h) = r(u) + h * dr/du + ......                               */
/*                                                                  */
/* Assume that the curve can be approximated by the first derivative*/
/*                                                                  */
/* _        _           _                                           */
/* r(u+h) = r(u) + h * dr/du                                        */
/*                                                                  */
/*        <===>                                                     */
/* _        _           _                                           */
/* r(u+h) - r(u) = h * dr/du   Length of vectors ==>                */
/*                                                                  */
/*                                                                  */
/*       _           _      _                                       */
/* h * !dr/du! = ! r(u+h) - r(u) !                                  */
/*                                                                  */
/*       _        _          _                                      */
/* h = ! r(u+h) - r(u) ! / !dr/du!                                  */
/*                                                                  */
/* Distance to the extended point shall be idpoint ==>              */
/*                _                                                 */
/* h = idpoint / !dr/du!                                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_idpoint         * Identical points criterion              */
/* GE109                  * Curve   evaluation routine              */
/* varkon_erpush          * Error message to error stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_extidpoint       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_extidpoint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBint    c_case,       /* Computation case (only 1 implemented)   */
  DBfloat *p_tstart,     /* Global parameter for extended start pt  */
  DBfloat *p_tend )      /* Global parameter for extended end   pt  */

/* Out:                                                             */
/*        Parameter values to p_start and p_tend                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  EVALC   xyz;           /* Coordinates and derivatives for crv pt  */
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBfloat idpoint;       /* Identical point criterion               */
  DBfloat t_start;       /* Start parameter for intersect calcul.   */
  DBfloat t_end;         /* End   parameter for intersect calcul.   */
  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

#ifdef DEBUG
  DBVector s_pt;         /* Start point curve                       */
  DBVector e_pt;         /* End   point curve                       */
  DBfloat  dist;         /* Check distance                          */
#endif

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur787 Enter varkon_cur_extidpoint: idpoint extrapolaton U values\n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/* Retrieve identical point criterion.                              */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

   comptol   = varkon_comptol();
   idpoint   = varkon_idpoint();

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 idpoint %f ns_cu %d\n", idpoint, (int)p_cur->ns_cu );
fflush(dbgfil(SURPAC)); 
}

if ( dbglev(SURPAC) == 1 && p_seg->typ ==  UV_CUB_SEG ) 
{
fprintf(dbgfil(SURPAC),
"sur787 idpoint Start segment is UV_CUB_SEG (=%d)\n", (int)p_seg->typ );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && p_seg->typ == CUB_SEG ) 
{
fprintf(dbgfil(SURPAC),
"sur787 idpoint Start segment is CUB_SEG (=%d)\n", (int)p_seg->typ );
fflush(dbgfil(SURPAC)); 
}


if ( dbglev(SURPAC) == 1 && (p_seg+p_cur->ns_cu-1)->typ ==  UV_CUB_SEG ) 
{
fprintf(dbgfil(SURPAC),
"sur787 idpoint Last  segment is UV_CUB_SEG \n" );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && (p_seg+p_cur->ns_cu-1)->typ == CUB_SEG ) 
{
fprintf(dbgfil(SURPAC),
"sur787 idpoint Last  segment is CUB_SEG \n" );
fflush(dbgfil(SURPAC)); 
}

#endif

/*!                                                                 */
/* Initialize to p_start = 1 and p_tend = ns_cu + 1                 */
/*                                                                 !*/

  *p_tstart = 1.0 + comptol;                 /* 1996-01-27 */
  *p_tend   = (DBfloat)p_cur->ns_cu + 1.0 - comptol;

/*!                                                                 */
/* Check of c_case for Debug On.                                    */
/*                                                                 !*/

#ifdef DEBUG
if ( c_case == 1 )
   {
   /*  OK  */
   }
else
   {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 c_case= %d is not OK \n", (int)c_case  );
fflush(dbgfil(SURPAC)); 
}
#endif
      sprintf(errbuf,"(c_case= %d)%%varkon_cur_extidpoint (sur787)",
                     (int)c_case);
      return(varkon_erpush("SU2993",errbuf));
   }
#endif


/*!                                                                 */
/* 2. Parameter values for idpoint extended curve                   */
/* ______________________________________________                   */
/*                                                                  */
/* Calculate point and tangent at ends of curve.                    */
/* Calls of varkon_GE109 (GE109).                                 */
/*                                                                 !*/

/*!                                                                 */
/* Start of curve (no extraoplation for UV_CUB_SEG)                     */
/*                                                                 !*/

   if  ( p_seg->typ ==  UV_CUB_SEG ) 
     {
     t_start = 1.0 + comptol;                 /* 1996-01-27 */
     goto endseg;
     }

   xyz.evltyp   = EVC_DR;
   xyz.t_global = 1.0 + comptol;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 GE109 failed for xyz.t_global %f\n", xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif
        sprintf(errbuf,"GE109%%varkon_sur_intplan");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
  s_pt = xyz.r;
#endif

  t_start = SQRT( xyz.drdt.x_gm*xyz.drdt.x_gm +
                  xyz.drdt.y_gm*xyz.drdt.y_gm +
                  xyz.drdt.z_gm*xyz.drdt.z_gm );
  if ( t_start > 0.000001 )
     {
     if ( idpoint/t_start  < 0.999999 )   /* 1996-01-27 */
       {
       t_start = 1.0 - idpoint/t_start;
       }
     else
       {
       t_start = 1.0 - 0.999999 ;
       }
     }

/* End   of curve                                                   */

endseg:;  /* Label: First segment is a UV_CUB_SEGment                   */

   if  ( (p_seg+p_cur->ns_cu-1)->typ ==  UV_CUB_SEG ) 
     {
     t_end   = (DBfloat)p_cur->ns_cu + 1.0 - comptol;
     goto endcur;
     }


   xyz.t_global = (DBfloat)p_cur->ns_cu + 1.0 - comptol;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 GE109 failed for xyz.t_global %f\n", xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif
        sprintf(errbuf,"GE109%%varkon_sur_intplan");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
  e_pt = xyz.r;
#endif

  t_end   = SQRT( xyz.drdt.x_gm*xyz.drdt.x_gm +
                  xyz.drdt.y_gm*xyz.drdt.y_gm +
                  xyz.drdt.z_gm*xyz.drdt.z_gm );
  if ( t_end   > 0.000001 )
     {
     if ( idpoint/t_end  < 0.999999 )   /* 1996-01-27 */
       {
       t_end   = (DBfloat)p_cur->ns_cu + 1.0 + idpoint/t_end;
       }
     else
       {
       t_end   = (DBfloat)p_cur->ns_cu + 1.0 + 0.999999;       
       }
     }

endcur:;  /* Label: Last  segment is a UV_CUB_SEGment                   */

  *p_tstart = t_start;
  *p_tend   = t_end;

/*!                                                                 */
/* 3. Check of distance for Debug On                                */
/* __________________________________                               */
/*                                                                  */
/* Criterion for distance check is 0.9*dist <= dist <= 1.2*idpoint  */
/* Calls of varkon_GE109 (GE109).                                 */
/*                                                                 !*/

#ifdef DEBUG
/* Start DEBUG:    Check of distance                                */

   if  ( p_seg->typ ==  UV_CUB_SEG ) goto endsegc;

   xyz.evltyp   = EVC_DR;
   xyz.t_global = t_start;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_intplan");
        return(varkon_erpush("SU2943",errbuf));
        }

  dist    = SQRT( (s_pt.x_gm-xyz.r.x_gm)*(s_pt.x_gm-xyz.r.x_gm) +
                  (s_pt.y_gm-xyz.r.y_gm)*(s_pt.y_gm-xyz.r.y_gm) +
                  (s_pt.z_gm-xyz.r.z_gm)*(s_pt.z_gm-xyz.r.z_gm) );

   if ( dist >= 0.9*idpoint && dist <= 1.2*idpoint )
      {
      /*  OK  */
      }
   else
      {
      sprintf(errbuf,"(start extension)%% ._cur_extidpoint (sur787)");
      return(varkon_erpush("SU2993",errbuf));
      }

/*   Check of distance                                */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 dist %15.10f idpoint %f for t_start %f\n", dist , idpoint , t_start);
}

endsegc:;

   if  ( (p_seg+p_cur->ns_cu-1)->typ ==  UV_CUB_SEG ) goto  endcurc;

   xyz.evltyp   = EVC_DR;
   xyz.t_global = t_end;

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_intplan");
        return(varkon_erpush("SU2943",errbuf));
        }

  dist    = SQRT( (e_pt.x_gm-xyz.r.x_gm)*(e_pt.x_gm-xyz.r.x_gm) +
                  (e_pt.y_gm-xyz.r.y_gm)*(e_pt.y_gm-xyz.r.y_gm) +
                  (e_pt.z_gm-xyz.r.z_gm)*(e_pt.z_gm-xyz.r.z_gm) );

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 dist %15.10f idpoint %f for t_end   %f\n", 
             dist , idpoint , t_end  );
}

   if ( dist >= 0.9*idpoint && dist <= 1.2*idpoint )
      {
      /*  OK  */
      }
   else
      {
      sprintf(errbuf,"(end extension)%%varkon_cur_extidpoint (sur787)");
      return(varkon_erpush("SU2993",errbuf));
      }

endcurc:;

/* End   DEBUG:    Check of distance                                */
#endif

/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur787 t_start %f t_end %f\n", t_start ,t_end  );
fflush(dbgfil(SURPAC)); 
}
#endif


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur787 Exit _cur_extidpoint t start %15.10f t end %15.10f\n", 
             *p_tstart , *p_tend );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
