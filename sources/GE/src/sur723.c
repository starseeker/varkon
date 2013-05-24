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
/*  Function: varkon_cur_approxseg                 File: sur723.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Find a curve, with given type of segments, which approximates   */
/*  another curve. The number of output segments is input to        */
/*  the function. There will be no checks that the curve is         */
/*  within any tolerances.                                          */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-29   Originally written                                 */
/*  1997-03-12   Debug comments                                     */
/*  1999-12-05   Free source code modifications                     */
/*  2002-06-28   Bug fix   EVC_DR -> EVC_DR + EVC_R  (Sören L       */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_approxseg  Approximate, given no of segments*/
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109();               * Curve   evaluation routine              */
/* GE133();               * Rational cubic with P-value             */
/* varkon_erpush();       * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ... failed in varkon_cur_approxseg      */
/* SU2993 = Severe program error ( )   in varkon_cur_approxseg      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_approxseg (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Input curve                       (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  APPCUR  *p_appdat,     /* Curve approximation data          (ptr) */
  DBCurve *p_ocur,       /* Output curve                      (ptr) */
  DBSeg   *p_oseg )      /* Coefficients for curve segments   (ptr) */

/* Out:                                                             */
/*        Number of intersects and ordered U values                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    i_segin;      /* Loop index segment record input curve   */
  DBint    i_extra;      /* Loop index segment extra segments       */
  DBfloat  t_delta;      /* Delta parameter value for n_req         */
  DBfloat  t_start;      /* Start parameter value for segment       */
  DBfloat  t_end;        /* End   parameter value for segment       */
  DBint    n_outseg;     /* Number of output segments               */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  EVALC    xyz_s;        /* Start point for output segment          */
  EVALC    xyz_e;        /* End   point for output segment          */
  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBfloat  pvalue;       /* P value for the rational segment        */
  DBSeg   *pdummy;       /* Pointer to output segments              */

  DBint    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur723 Enter varkon_cur_approxseg Approximate ctype %d n_req %d\n",
             p_appdat->ctype , p_appdat->n_req );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

  if ( p_appdat->acase != 2 )
    {
    sprintf(errbuf,"acase != 2%%varkon_cur_approxseg (sur723)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( p_appdat->n_req < 1 )
    {
    sprintf(errbuf,"n_req<1%%varkon_cur_approxseg (sur723)");
    return(varkon_erpush("SU2993",errbuf));
    }

  t_delta = 1.0/(DBfloat)p_appdat->n_req;


/* Evaluation of coordinates and tangents                           */
   xyz_s.evltyp   = EVC_DR + EVC_R;  /* (a) bugfix 2002-06-28  */
   xyz_e.evltyp   = EVC_DR + EVC_R;

/*!                                                                 */
/*  2. Create approximative segments                                */
/*  _________________________________                               */
/*                                                                  */
/*                                                                 !*/


   n_outseg = 0;                /* Initialize no of output segments */

   for (i_segin=1; i_segin<= p_cur->ns_cu; i_segin= i_segin+1) 
     {                                   /* Start loop i_segin     */
     for (i_extra=1; i_extra<= p_appdat->n_req; i_extra= i_extra+1) 
       {                                   /* Start loop i_extra     */

       n_outseg = n_outseg + 1;   /* An additional segment          */
       if ( n_outseg > p_appdat->n_alloc )
         {
         sprintf(errbuf,"n_alloc%%varkon_cur_approxseg (sur723)");
         return(varkon_erpush("SU2943",errbuf));
         }
       t_start  = ((DBfloat)n_outseg-1.0)*t_delta + 1.0;
       t_end    = t_start + t_delta - p_appdat->comptol; 
       t_start  = t_start           + p_appdat->comptol; 

/*!                                                                 */
/*     Curve evaluation ....                                        */
/*     Calls of varkon_GE109 (GE109).                             */
/*                                                                 !*/

/*     Parameter value for start and end of segment                 */

       xyz_s.t_global = t_start;   
       xyz_e.t_global = t_end;   

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur724 %d xyz_s.t_global %12.8f _e %12.8f start pt i_segin %d i_extra %d\n",
  n_outseg , xyz_s.t_global , xyz_e.t_global, i_segin , i_extra );
fflush(dbgfil(SURPAC)); 
}
#endif


       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 _s%%varkon_cur_approxseg (sur723)");
         return(varkon_erpush("SU2943",errbuf));
         }


       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_e);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 _e%%varkon_cur_approxseg (sur723)");
         return(varkon_erpush("SU2943",errbuf));
         }

       points[0].x_gm = xyz_s.r.x_gm;
       points[0].y_gm = xyz_s.r.y_gm;
       points[0].z_gm = xyz_s.r.z_gm;
       points[1].x_gm = xyz_s.r.x_gm+10.0*xyz_s.drdt.x_gm;
       points[1].y_gm = xyz_s.r.y_gm+10.0*xyz_s.drdt.y_gm;
       points[1].z_gm = xyz_s.r.z_gm+10.0*xyz_s.drdt.z_gm;
       points[2].x_gm = xyz_e.r.x_gm+10.0*xyz_e.drdt.x_gm;
       points[2].y_gm = xyz_e.r.y_gm+10.0*xyz_e.drdt.y_gm;
       points[2].z_gm = xyz_e.r.z_gm+10.0*xyz_e.drdt.z_gm;
       points[3].x_gm = xyz_e.r.x_gm;
       points[3].y_gm = xyz_e.r.y_gm;
       points[3].z_gm = xyz_e.r.z_gm;
       pvalue         = 0.5;

       pdummy = p_oseg + n_outseg - 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur724 Start         point %10.2f %10.2f %10.2f\n",
  points[0].x_gm, points[0].y_gm, points[0].z_gm   );
fprintf(dbgfil(SURPAC),
"sur724 Start tangent point %10.2f %10.2f %10.2f\n",
  points[1].x_gm, points[1].y_gm, points[1].z_gm   );
fprintf(dbgfil(SURPAC),
"sur724 End   tangent point %10.2f %10.2f %10.2f\n",
  points[2].x_gm, points[2].y_gm, points[2].z_gm   );
fprintf(dbgfil(SURPAC),
"sur724 Start         point %10.2f %10.2f %10.2f\n",
  points[3].x_gm, points[3].y_gm, points[3].z_gm   );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

       status=GE133 (points,pvalue,pdummy );
       if (status<0) 
         {
         sprintf(errbuf,"GE133%%varkon_cur_approxseg (sur723)");
         return(varkon_erpush("SU2943",errbuf));
          }


       }                                          /* End   loop     */
     }                                            /* End   loop     */


/*!                                                                 */
/*  4. Output curve header data (DBCurve)                           */
/*  ___________________________________                             */
/*                                                                  */
/*                                                                 !*/

   p_ocur->ns_cu = (short)n_outseg;
   p_ocur->al_cu = 0.0;        
   p_ocur->plank_cu  = FALSE;      
   p_ocur->hed_cu.type = CURTYP;


  return(SUCCED);

} /* End of function */


/********************************************************************/



