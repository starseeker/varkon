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
/*  Function: varkon_cur_approxrdr                 File: sur724.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Find a curve, with rational cubic segments, which approximates  */
/*  another curve. Try to reduce the number of segments.            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-29   Originally written                                 */
/*  1996-05-28   c_seg internal variable deleted                    */
/*  1997-03-12   Comments                                           */
/*  1999-12-05   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_approxrdr  Approxim., reduce with rationals */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109                  * Curve   evaluation routine              */
/* GE133                  * Rational cubic with P-value             */
/* GE136                  * Rational cubic with mid pt              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ... failed in varkon_cur_approxrdr      */
/* SU2993 = Severe program error ( )   in varkon_cur_approxrdr      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_approxrdr (

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
  DBint    n_req;        /* Number of segments correspond. to d_che */
  DBint    n_che;        /* Number of check points                  */
  DBint    n_outseg;     /* Number of output segments               */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  EVALC    xyz[1000];    /* All check points                        */
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
"sur724 Enter varkon_cur_approxrdr Approximate curve ctype %d \n",
             p_appdat->ctype );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

  if ( p_appdat->acase != 5 )
    {
    sprintf(errbuf,"acase != 5%%varkon_cur_approxrdr (sur724)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( p_appdat->ctype !=  3  )
    {
    sprintf(errbuf,"ctype != 3%%varkon_cur_approxrdr (sur724)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( p_appdat->d_che < 0.1  )
    {
    sprintf(errbuf,"d_che< 0.1%%varkon_cur_approxrdr (sur724)");
    return(varkon_erpush("SU2993",errbuf));
    }



/* Evaluation of coordinates and tangents                           */
   xyz_s.evltyp   = EVC_DR;
   xyz_e.evltyp   = EVC_DR;

/*!                                                                 */
/*  2. Calculate all check points                                   */
/*  _____________________________                                   */
/*                                                                  */
/*                                                                 !*/


   n_outseg = 0;                /* Initialize no of output segments */
   n_che    = 0;                /* Initialize no of check points    */

   for (i_segin=1; i_segin<= p_cur->ns_cu; i_segin= i_segin+1) 
     {                                   /* Start loop i_segin     */
     n_req = 1;
     if ( (p_seg+i_segin-1)->sl > p_appdat->d_che )
       {
       n_req = (DBint)((p_seg+i_segin-1)->sl/p_appdat->d_che) + 1;
       }
     t_delta = 1.0/(DBfloat)n_req;
     for (i_extra=1; i_extra<= n_req; i_extra= i_extra+1) 
       {                                   /* Start loop i_extra     */

       n_outseg = n_outseg + 1;   /* An additional segment          */
       if ( n_outseg > p_appdat->n_alloc )
         {
         sprintf(errbuf,"n_alloc%%varkon_cur_approxrdr (sur724)");
         return(varkon_erpush("SU2943",errbuf));
         }
       t_start  = ((DBfloat)n_outseg-1.0)*t_delta + 1.0;
       t_start  = (DBfloat)i_segin + ((DBfloat)i_extra-1.0)*t_delta;
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
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 _s%%varkon_cur_approxrdr (sur724)");
         return(varkon_erpush("SU2943",errbuf));
         }

       n_che    = n_che    + 1;   /* An additional check point      */
       if ( n_che    > 1000 - 1 )
         {
         sprintf(errbuf,"n_che>1000%%varkon_cur_approxrdr (sur724)");
         return(varkon_erpush("SU2943",errbuf));
         }

       xyz[n_che-1] = xyz_s;

       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_e);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 _e%%varkon_cur_approxrdr (sur724)");
         return(varkon_erpush("SU2943",errbuf));
         }

       if ( i_extra == n_req ) 
         {
         n_che    = n_che  + 1;   /* An additional check point      */
         xyz[n_che-1] = xyz_s;
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
         sprintf(errbuf,"GE133%%varkon_cur_approxrdr (sur724)");
         return(varkon_erpush("SU2943",errbuf));
         }


       }                                          /* End   loop     */
     }                                            /* End   loop     */

/*!                                                                 */
/*  3. Create approximative segments                                */
/*  _________________________________                               */
/*                                                                  */
/*                                                                 !*/

#ifdef  CONTINUE_HERE
   n_outseg = 0;                /* Initialize no of output segments */
   i_start  = 1;
   i_end    = 3;

   for (i_poi=i_end;   i_poi <= n_che ; i_poi= i_poi+1) 
     {                                          /* Start loop point */


     }                                          /* End   loop point */
#endif

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
