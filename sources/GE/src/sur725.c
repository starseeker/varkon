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
/*  Function: varkon_cur_approxadd                 File: sur725.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a curve, with given type of segments, which approximates */
/*  another curve. Segments will be added in order to respect       */
/*  given (input) tolerances.                                       */
/*                                                                  */
/*  !!!TODO!!! More should be programmed for curves !!!!!!!!!!!!!   */
/*  !!!TODO!!! on NURBS surfaces. There might be    !!!!!!!!!!!!!   */
/*  !!!TODO!!! kinks in one patch. Use knot vector? !!!!!!!!!!!!!   */
/*  !!!TODO!!! One UV segment on one closed NURBS patch !!!!!!!!!!  */
/*   The best solution is create a new sur150 function which        */
/*   creates an iso-curve with many segments.                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-17   Originally written                                 */
/*  1996-11-03   Bug when called from sur960 ....                   */
/*  1997-01-31   Debug                                              */
/*  1997-12-06   One UV segment on one closed NURBS patch (=surface)*/
/*  1998-01-06   n_add check                                        */
/*  1999-12-05   Free source code modifications                     */
/*  2002-06-28   Bugfix EVC_DR ->EVC_DR + EVC_R in crecub (Sören L) */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_approxadd  Approximate, add segments        */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109                  * Curve evaluation function               */
/* GE133                  * Create rational segment with P value    */
/* GE136                  * Create rational segment with mid point  */
/* GE143                  * Find closest points on curve segment    */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_angd            * Angle between vectors (degr.)           */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Initialization of data             */
static short chepoi ();       /* Check points for current segment   */
static short crecub ();       /* Create output cubic    segment     */
static short crerat ();       /* Create output rational segment     */
static short cheseg ();       /* Check  output curve segment        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  ctol;          /* Coordinate tolerance for approxim.  */
static DBfloat  ttol;          /* Tangent    tolerance for approxim.  */
static DBfloat  idpoint;       /* Identical point criterion           */
static DBfloat  d_che;         /* Delta (R*3) for check points        */
static short  nstart;        /* The number of restarts              */
static DBint  n_che;         /* Number of check points per segment  */
static DBVector p_che[30];     /* Check points   for one segment      */
static DBVector t_che[30];     /* Check tangents for one segment      */
static DBfloat  t_delta;       /* Delta parameter value for n_add     */  
static DBfloat  t_start;       /* Start parameter value for segment   */  
static DBfloat  t_end;         /* End   parameter value for segment   */  
static DBSeg *p_cseg;        /* Current output segment        (ptr) */
static DBfloat  dist_max;      /* Maximum distance to check point     */
static DBfloat  ang_max;       /* Maximum angle    to check tangent   */
static DBfloat  chord_len;     /* Chord length for output segment     */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function ... failed in varkon_cur_approxadd      */
/* SU2973 = Internal function () failed in varkon_sur_aplane        */
/* SU2993 = Severe program error ( )   in varkon_cur_approxadd      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_approxadd (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Input curve                       (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  APPCUR  *p_appdat,     /* Curve approximation data          (ptr) */
  DBCurve *p_ocur,       /* Output curve                      (ptr) */
  DBSeg   *p_oseg )      /* Coefficients for curve segments   (ptr) */

/* Out:                                                             */
/*        Coefficients for the ouput curve, number of segments      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   n_add;         /* Number of added segments                */
  DBint   i_segin;       /* Loop index segment record input curve   */
  DBint   i_extra;       /* Loop index segment extra segments       */
  DBint   n_outseg;      /* Number of output segments               */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint   n_outseg_start;/* Number of output segments for new segm. */

  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 Enter ctype %d d_che %f n_alloc %d\n",
    (short)p_appdat->ctype , p_appdat->d_che,
    (short)p_appdat->n_alloc );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 p_cur->ns_cu %d \n", (short)p_cur->ns_cu );
fflush(dbgfil(SURPAC)); 
}

#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

    status= initial (p_appdat, p_ocur);
    if (status<0) 
      {
      sprintf(errbuf,"initial%%sur725");
      return(varkon_erpush("SU2973",errbuf));
    }


/*!                                                                 */
/*  2. Create approximate segments                                  */
/*  ______________________________                                  */
/*                                                                  */
/*                                                                 !*/


   n_outseg = 0;                /* Initialize no of output segments */

/*!                                                                 */
/*  Start loop all segments of the input curve                      */
/*                                                                 !*/

   for (i_segin=1; i_segin<= p_cur->ns_cu; i_segin= i_segin+1) 
     {                                   /* Start loop i_segin     */
/*!                                                                 */
/*   Calculate check points for the current segment                 */
/*                                                                 !*/
     status= chepoi  ( p_cur, p_seg, p_appdat, i_segin );
     if (status<0) 
       {
       sprintf(errbuf,"chepoi%%sur725");
       return(varkon_erpush("SU2973",errbuf));
       }

/*!                                                                 */
/*   Create curve segments and check that they are within tolerance */
/*                                                                 !*/

/*   Start value for the number of segments                         */
     n_add = 0;
/*   Start value for output number of segments before adding        */
     n_outseg_start = n_outseg;

addseg:; /*! Label: Segments must be added                         !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 addseg: i_segin %d n_add %d n_outseg %d\n",
       (short)i_segin, (short)n_add , (short)n_outseg);
fflush(dbgfil(SURPAC)); 
}
#endif

/*   An additional segment                                          */
     n_add = n_add + 1;

/* Added 1998-01-05 */
       if ( n_outseg+n_add > p_appdat->n_alloc )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 Allocated area will be exceeded n_alloc %d\n", 
          (short)p_appdat->n_alloc );
fflush(dbgfil(SURPAC));
}
#endif
         sprintf(errbuf,"Alloc. iwill be exceeded%%sur725");
         return(varkon_erpush("SU2993",errbuf));
         }






/*   Delta parameter step corresponding to n_add                    */
     t_delta = 1.0/(DBfloat)n_add;
/*   Start number for output segments                               */
     n_outseg = n_outseg_start;
/*   Start loop extra segments                                      */
     for (i_extra=1; i_extra<= n_add; i_extra= i_extra+1) 
       {                                   /* Start loop i_extra     */

       n_outseg = n_outseg + 1;   /* An additional segment          */
       if ( n_outseg > p_appdat->n_alloc )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 Allocated area exceeded n_alloc %d\n", 
          (short)p_appdat->n_alloc );
fflush(dbgfil(SURPAC));
}
#endif
         sprintf(errbuf,"Alloc. exceeded%%sur725");
         return(varkon_erpush("SU2993",errbuf));
         }

       t_start  = (DBfloat)i_segin+((DBfloat)i_extra -1.0)*t_delta;
       t_end    = t_start + t_delta - p_appdat->comptol; 
       t_start  = t_start           + p_appdat->comptol; 

       p_cseg = p_oseg + n_outseg - 1;

       if        ( p_appdat->acase == 2 )
         {
         status= crecub  (p_cur, p_seg );
         if   (status < 0 && n_add  <= 1 ) 
           {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 crecub failed  n_add %d\n", (short)n_add );
fflush(dbgfil(SURPAC));
}
#endif
           goto addseg;
           }
         else if (status < 0  ) 
           {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 crecub failed  n_add %d\n", (short)n_add );
fflush(dbgfil(SURPAC));
}
#endif
           sprintf(errbuf,"crecub%%sur725");
           return(varkon_erpush("SU2973",errbuf));
           }
         }
       else if   ( p_appdat->acase == 5 )
         {
         status= crerat  (p_cur, p_seg );
         if      ( status < 0 && n_add  <= 1 ) 
           {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 crerat failed  n_add %d\n", (short)n_add );
fflush(dbgfil(SURPAC));
}
#endif
           goto addseg;
           }
         else if ( status < 0 ) 
           {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 crerat failed  n_add %d\n", (short)n_add );
fflush(dbgfil(SURPAC));
}
#endif
           sprintf(errbuf,"crerat%%sur725");
           return(varkon_erpush("SU2973",errbuf));
           }
         }
       else 
         {
         sprintf(errbuf,"acase%%sur725");
         return(varkon_erpush("SU2993",errbuf));
         }


       status= cheseg  (p_cur, p_seg, p_ocur );
       if (status<0) 
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 cheseg failed  n_add %d\n", (short)n_add );
fflush(dbgfil(SURPAC));
}
#endif
         sprintf(errbuf,"cheseg%%sur725");
         return(varkon_erpush("SU2973",errbuf));
         }


         if  (  dist_max > ctol || ang_max > ttol )
           {
           goto addseg;
           }

       }      /* End   loop new added segments                      */
     }        /* End   loop all segments of the input curve         */
/*!                                                                 */
/*  End   loop all segments of the input curve                      */
/*                                                                 !*/


/*!                                                                 */
/*  4. Output curve header data ns_cu                               */
/*  ___________________________________                             */
/*                                                                  */
/*                                                                 !*/

   p_ocur->ns_cu = (short)n_outseg;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 Exit p_ocur->ns_cu %d\n", p_ocur->ns_cu );
fflush(dbgfil(SURPAC));
}
#endif


  return(SUCCED);

} /* End of function */


/********************************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Checks input data and initializes data                           */

   static short initial ( p_appdat, p_ocur )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  APPCUR  *p_appdat;     /* Curve approximation data          (ptr) */
/*                                                                  */
/* Out:                                                             */
  DBCurve *p_ocur;       /* Output curve                      (ptr) */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur725*initial* Enter **  \n");
  }
#endif


/*!                                                                 */
/*   Check acase                                                    */
/*                                                                 !*/

  if ( p_appdat->acase == 2 || p_appdat->acase == 5 )
    {
    ;          /* Cubic or rational is OK */
    }
  else
    {
    sprintf(errbuf,"acase not 2 or 5%%sur725");
    return(varkon_erpush("SU2993",errbuf));
    }

   idpoint   = varkon_idpoint();

  if ( p_appdat->d_che < 10.0*idpoint && p_appdat->n_req < 1 )
    {
    sprintf(errbuf,"d_che<10*idpoint%%varkon_cur_approxadd (sur725)");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( p_appdat->n_req > 30 )
    {
    sprintf(errbuf,"n_req>30%%sur725");
    return(varkon_erpush("SU2993",errbuf));
    }

  ctol    = p_appdat->ctol;
  ttol    = p_appdat->ntol;
  d_che   = p_appdat->d_che;

/*!                                                                 */
/*  Output curve header data (DBCurve)                              */
/*                                                                 !*/

   p_ocur->ns_cu = (short)1;  /* Used for the check below */
   p_ocur->al_cu = 0.0;        
   p_ocur->plank_cu  = FALSE;      
   p_ocur->hed_cu.type = CURTYP;


/* Number of restarts for closest point                             */


   nstart = 2;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  fprintf(dbgfil(SURPAC),
  "sur725*initial  Exit \n"); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** chepoi  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculate check points                                           */

   static short chepoi  ( p_cur, p_seg, p_appdat, i_segin )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
  APPCUR  *p_appdat;     /* Curve approximation data          (ptr) */
  DBint   i_segin;       /* Loop index segment record input curve   */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   i_che;         /* Loop index check point                  */
  DBfloat   ud_che;        /* Delta parameter for check points        */  
  DBfloat   t_curr ;       /* Current parameter value                 */  
  EVALC   xyz_c;         /* Current check point                     */
/*----------------------------------------------------------------- */

  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur725*chepoi * Enter **  \n");
  }
#endif

   xyz_c.evltyp   = EVC_DR + EVC_R;   

/*   Calculate n_che for the given distance d_che                   */
     if    ( d_che < 0  &&  p_appdat->n_req >= 1 )
       {
       n_che = p_appdat->n_req;
       ud_che = 1.0/(DBfloat)(n_che+1);
       }
     else
       {
       sprintf(errbuf,"d_che>0 not yet impl%%sur725*chepoi");
       return(varkon_erpush("SU2993",errbuf));
       }


/*   Start loop check points                                        */
     for (i_che =1; i_che <= n_che; i_che = i_che  +1) 
       {   

       t_curr   = (DBfloat)i_segin+(DBfloat)i_che*ud_che;  

       xyz_c.t_global = t_curr ;   


       status=GE109 ((DBAny *)p_cur,p_seg,&xyz_c);
       if (status<0) 
         {
         sprintf(errbuf,"GE109 che%%sur725");
         return(varkon_erpush("SU2943",errbuf));
         }

       p_che[i_che-1] = xyz_c.r;   
       t_che[i_che-1] = xyz_c.drdt;   

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && ( i_che == 1 || i_che == n_che ))
{
fprintf(dbgfil(SURPAC),
"sur725*chepoi xyz_c.t_global %12.8f i_segin %d i_che %d\n",
  xyz_c.t_global , (short)i_segin , (short)i_che );
fprintf(dbgfil(SURPAC),
"sur725*chepoi xyz_c.r %12.2f %12.2f %12.2f\n",
  xyz_c.r.x_gm, xyz_c.r.y_gm,  xyz_c.r.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

       }                 /* End loop i_seg   */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  fprintf(dbgfil(SURPAC),
  "sur725*chepoi   Exit \n"); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/






/*!********* Internal ** function ** crecub  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create current output segment                                    */

   static short crecub  ( p_cur, p_seg )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  EVALC   xyz_s;         /* Start point for output segment          */
  EVALC   xyz_e;         /* End   point for output segment          */
  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBfloat   pvalue;        /* P value for the rational segment        */
/*----------------------------------------------------------------- */

  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur725*crecub * Enter **  \n");
  }
#endif

/*!                                                                 */
/* Curve evaluation ....                                            */
/* Calls of GE109.                                                  */
/*                                                                 !*/

/* Evaluation of coordinates and tangents                           */
   xyz_s.evltyp   = EVC_R + EVC_DR;
   xyz_e.evltyp   = EVC_R + EVC_DR;

/* Parameter value for start and end of segment                     */

   xyz_s.t_global = t_start;   
   xyz_e.t_global = t_end;   

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*crecub xyz_s.t_global %12.8f _e %12.8f p_cur->ns_cu %d\n",
  xyz_s.t_global , xyz_e.t_global, (short)p_cur->ns_cu );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
   if (status<0) 
     {
     sprintf(errbuf,"GE109 _s%%sur725*crecub");
     return(varkon_erpush("SU2943",errbuf));
     }


   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_e);
   if (status<0) 
     {
     sprintf(errbuf,"GE109 _e%%sur725*crecub");
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

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*crecub Start         point %10.2f %10.2f %10.2f\n",
  points[0].x_gm, points[0].y_gm, points[0].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crecub Start tangent point %10.2f %10.2f %10.2f\n",
  points[1].x_gm, points[1].y_gm, points[1].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crecub End   tangent point %10.2f %10.2f %10.2f\n",
  points[2].x_gm, points[2].y_gm, points[2].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crecub Start         point %10.2f %10.2f %10.2f\n",
  points[3].x_gm, points[3].y_gm, points[3].z_gm   );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

   status=GE133 (points,pvalue,p_cseg );
   if (status<0) 
     {
     sprintf(errbuf,"GE133%%sur725*crecub");
     return(varkon_erpush("SU2943",errbuf));
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  fprintf(dbgfil(SURPAC),
  "sur725*crecub   Exit \n"); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/






/*!********* Internal ** function ** cheseg  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Check  current output segment                                    */

   static short cheseg  ( p_cur, p_seg, p_ocur )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
  DBCurve *p_ocur;       /* Output curve                      (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  EVALC   xyz_c;         /* Closest point to the check point        */
  DBint   i_che;         /* Loop index check point                  */

  DBVector extpt;        /* External check point                    */
  short   noinse;        /* Number of closest pts from one segment  */
  DBfloat   useg[INTMAX];  /* The unordered array of u solutions      */
                         /* from one segment                        */
  DBfloat   dist;          /* Distance between extpt and curve        */
  DBfloat   angle;         /* Deviation angle to reference tangent    */
/*----------------------------------------------------------------- */

  DBfloat   s_tan[3];      /* Reference tangent                       */
  DBfloat   e_tan[3];      /* Current   tangent                       */
  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur725*cheseg * Enter **  \n");
  }
#endif

  xyz_c.evltyp   = EVC_DR; 

  dist_max = -50000.0;
  ang_max  = -50000.0;
  dist     = -1.23456789;
  angle    = -1.23456789;

/*   Start loop check points                                        */
     for (i_che =1; i_che <= n_che; i_che = i_che  +1) 
       {                             
       extpt = p_che[i_che-1];   
       s_tan[0] = t_che[i_che-1].x_gm;
       s_tan[1] = t_che[i_che-1].y_gm;
       s_tan[2] = t_che[i_che-1].z_gm;

       status=GE143
       ((DBAny *)p_ocur,p_cseg,&extpt,nstart,&noinse,useg);
       if (status<0) 
         {
         sprintf(errbuf,"GE143%%sur725*cheseg");
         return(varkon_erpush("SU2943",errbuf));
         }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*cheseg i_che %d noinse %d\n",
  (short)i_che, (short)noinse  );
fflush(dbgfil(SURPAC));
}
#endif


/*     Check distance and tangent if there is a closest point       */
       if ( noinse >= 1 )
         {

         xyz_c.t_global = useg[0] + 1.0;   
         status=GE109 ((DBAny *)p_ocur,p_cseg,&xyz_c);
         if (status<0) 
           {
           sprintf(errbuf,"GE109 che 2%%sur725");
           return(varkon_erpush("SU2943",errbuf));
           }

         dist = SQRT(
         (xyz_c.r.x_gm-extpt.x_gm)*(xyz_c.r.x_gm-extpt.x_gm)+
         (xyz_c.r.y_gm-extpt.y_gm)*(xyz_c.r.y_gm-extpt.y_gm)+
         (xyz_c.r.z_gm-extpt.z_gm)*(xyz_c.r.z_gm-extpt.z_gm));   

         if  ( dist > dist_max ) dist_max = dist;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725 i_che %d noinse %d dist %f ctol %8.4f useg[0] %8.4f\n",
  (short)i_che, (short)noinse, dist, ctol,  useg[0]);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725 extpt  %12.4f %12.4f %12.4f\n",
        extpt.x_gm, extpt.y_gm, extpt.z_gm );
fprintf(dbgfil(SURPAC),
"sur725 xyz_c.r  %12.4f %12.4f %12.4f\n",
        xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

         if  (  dist_max > ctol )
           {
           goto outtol;
           }

       e_tan[0] = xyz_c.drdt.x_gm;
       e_tan[1] = xyz_c.drdt.y_gm;
       e_tan[2] = xyz_c.drdt.z_gm;

         status= varkon_angd (e_tan,s_tan,&angle);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur275*cheseg");
           return(varkon_erpush("SU2943",errbuf));
           }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*cheseg i_che %d angle %f ttol %f\n",
  (short)i_che, angle, ttol );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


         if  ( fabs(angle) > ang_max ) ang_max = fabs(angle);
         if  (  ang_max > ttol )
           {
           goto outtol;
           }


         }  /* End noinse >= 1 */

       }      /* End  loop check points for the current new segment */

  if ( dist_max < -49999.9 )
    {
    dist_max = 0.0;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC), "sur725*cheseg  No closest point !!! \n"); 
#endif
    }

outtol:;         /* Label: Out of tolerance */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur725*cheseg Exit dist_max %f ang_max %f\n",dist_max,  ang_max ); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** crerat  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create current output segment                                    */

   static short crerat  ( p_cur, p_seg )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur;        /* Input curve                       (ptr) */
  DBSeg   *p_seg;        /* Coefficients for curve segments   (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  EVALC   xyz_s;         /* Start point for output segment          */
  EVALC   xyz_m;         /* Mid   point for output segment          */
  EVALC   xyz_e;         /* End   point for output segment          */
  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBVector midp;         /* Mid point                               */
/*----------------------------------------------------------------- */

  short   status;        /* Error code from a called fctn           */
  char    errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur725*crerat * Enter **  \n");
  }
#endif

/*!                                                                 */
/* Curve evaluation ....                                            */
/* Calls of GE109.                                                  */
/*                                                                 !*/

/* Evaluation of coordinates and tangents                           */
   xyz_s.evltyp   = EVC_DR + EVC_R;
   xyz_m.evltyp   = EVC_DR + EVC_R;
   xyz_e.evltyp   = EVC_DR + EVC_R;

/* Parameter value for start and end of segment                     */

   xyz_s.t_global = t_start;   
   xyz_m.t_global = t_start + 0.5*(t_end-t_start);   
   xyz_e.t_global = t_end;   

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*crerat xyz_s.t_global %12.8f _m %12.8f _e %12.8f\n",
  xyz_s.t_global , xyz_m.t_global, xyz_e.t_global );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif


   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_s);
   if (status<0) 
     {
     sprintf(errbuf,"GE109 _s%%sur725*crerat");
     return(varkon_erpush("SU2943",errbuf));
     }

   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_m);
   if (status<0) 
     {
     sprintf(errbuf,"GE109 _m%%sur725*crerat");
     return(varkon_erpush("SU2943",errbuf));
     }


   status=GE109 ((DBAny *)p_cur,p_seg,&xyz_e);
   if (status<0) 
     {
     sprintf(errbuf,"GE109 _e%%sur725*crerat");
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
   midp.x_gm = xyz_m.r.x_gm;
   midp.y_gm = xyz_m.r.y_gm;
   midp.z_gm = xyz_m.r.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur725*crerat Start         point %10.2f %10.2f %10.2f\n",
  points[0].x_gm, points[0].y_gm, points[0].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crerat Start tangent point %10.2f %10.2f %10.2f\n",
  points[1].x_gm, points[1].y_gm, points[1].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crerat End   tangent point %10.2f %10.2f %10.2f\n",
  points[2].x_gm, points[2].y_gm, points[2].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crerat End           point %10.2f %10.2f %10.2f\n",
  points[3].x_gm, points[3].y_gm, points[3].z_gm   );
fprintf(dbgfil(SURPAC),
"sur725*crerat Mid           point %10.2f %10.2f %10.2f\n",
  midp.x_gm, midp.y_gm, midp.z_gm   );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

   status=GE136 (points,&midp ,p_cseg );
   if (status<0) 
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725*crerat Creation of rational with mid point failed\n");
}
#endif
      status= crecub  (p_cur, p_seg );
      if (status<0) 
        {
        sprintf(errbuf,"crecub%%sur725*crerat");
        return(varkon_erpush("SU2973",errbuf));
        }
      }

  chord_len = SQRT(   
(points[0].x_gm-points[3].x_gm)*(points[0].x_gm-points[3].x_gm)+
(points[0].y_gm-points[3].y_gm)*(points[0].y_gm-points[3].y_gm)+
(points[0].z_gm-points[3].z_gm)*(points[0].z_gm-points[3].z_gm));

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur725 crerat: chord_len %f\n",
       chord_len );
fflush(dbgfil(SURPAC)); 
}
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  fprintf(dbgfil(SURPAC),
  "sur725*crerat   Exit \n"); 
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


