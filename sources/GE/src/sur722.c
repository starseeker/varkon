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
/*  Function: varkon_cur_approx                    File: sur722.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Find a curve, with given type of segments, which approximates   */
/*  another curve.                                                  */
/*                                                                  */
/*  Memory area is allocated for the curve segment coefficients.    */
/*  This area must be deallocated by the calling function, i.e      */
/*  deallocatiom must be made when output pointer NOT is NULL.      */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-29   Originally written                                 */
/*  1996-10-17   sur725 added                                       */
/*  1996-11-03   n_alloc change                                     */
/*  1998-01-06   Debug                                              */
/*  1998-02-28   sur726 added                                       */
/*  1999-12-01   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_approx     Approximate to a given curve type*/
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_linecrit         * Criterion straight line                */
/* varkon_ini_appcur       * Initialization of APPCUR               */
/* varkon_ini_gmcur        * Initiate DBCurve                       */
/* varkon_ini_gmseg        * Initiate DBSeg                         */
/* varkon_cur_approxseg    * Approximate, given segments            */
/* varkon_cur_approxrdr    * Approximate, reduce rational           */
/* varkon_cur_approxrci    * Approximate to circles/lines           */
/* varkon_cur_approxadd    * Approximate, add segments              */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_cur_approx          */
/* SU2993 = Severe program error ( ) in varkon_cur_approx           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus        varkon_cur_approx (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBint    c_type,       /* Type of calculation                     */
                         /* Eq. 1: No of output segments is input   */
                         /* Eq. 2: Only add segments                */
                         /* Eq. 3: Try to reduce no of segments     */
  DBint    n_req,        /* Requested number of segments c_type= 1  */
                         /* or number of check points for c_type=2  */
  DBint    t_incur,      /* Type of input curve (what to use)       */
                         /* Eq. 1: Points (segment end points)      */
                         /* Eq. 2: Lines                            */
                         /* Eq. 3: Points and tangents              */
                         /* Eq. 4: Points, tangents and curvature   */

  DBint    a_case,       /* Approximation case                      */
                         /* Eq. 1: Approximate to LINE   segments   */
                         /* Eq. 2: Approximate to CUBIC  segments   */
                         /* Eq. 3: Approximate to CIRCLE segments   */
                         /* Eq. 4: Approximate to CONIC  segments   */
                         /* Eq. 5: Approximate to RATCUB segments   */

                         /* Input tolerances ( < 0 ==> default )    */
  DBfloat  in_ctol,      /* Coordinate       tolerance              */
  DBfloat  in_ntol,      /* Tangent          tolerance (degrees)    */
  DBfloat  in_rtol,      /* Curvature radius tolerance              */
                         /*              or                         */
                         /* Criterion straight line   for a_case= 3 */
  DBfloat  d_che,        /* Arclength check distance                */
  DBCurve *p_ocur,       /* Output curve                      (ptr) */
  DBSeg  **pp_oseg )     /* Alloc. area for curve segments    (ptr) */

/* Out:                                                             */
/*        Number of intersects and ordered U values                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  APPCUR  appdat;        /* Curve approximation data                */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint   n_alloc;       /* Number of allocated segments            */
  DBint   i_s;           /* Loop index segment record               */
  DBSeg  *p_outseg;      /* Curve segments                    (ptr) */
  DBfloat d_ctol;        /* Coordinate       tolerance (default)    */
  DBfloat d_ntol;        /* Tangent angle    tolerance (default)    */
  DBfloat d_linecrit;    /* Line criterion   tolerance (default)    */
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBfloat ctol;          /* Coordinate tolerance                    */
  DBfloat idangle;       /* Identical angles criterion              */
  DBfloat lcrit;         /* Criterion straight line (radius)        */
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
"sur722 Enter varkon_cur_approx : Approximate curve a_case %d\n",
             (short)a_case );
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur722 c_type %d n_req %d t_incur %d \n",
   (short)c_type , (short)n_req , (short)t_incur  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur722 in_ctol %f  in_ntol %f  in_rtol %f \n",
        in_ctol ,   in_ntol ,   in_rtol     );
fflush(dbgfil(SURPAC)); 
}

#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/* Initialization of APPCUR. Call of varkon_ini_appcur (sur762).    */
/*                                                                 !*/

   varkon_ini_appcur(&appdat);

/*!                                                                 */
/* Let pp_oseg= NULL                                                */
/*                                                                 !*/
   *pp_oseg = NULL;

/*!                                                                 */
/* Initialize local variables                                       */
/*                                                                 !*/
   n_alloc = 0;

   lcrit    = F_UNDEF;

/*!                                                                 */
/* Surface computer accuracy and coordinate tolerance.              */
/* Call of varkon_comptol (sur753) and varkon_ctol (sur751).        */
/*                                                                 !*/

   comptol     = varkon_comptol();
   d_ctol      = varkon_idpoint();
   d_ntol      = varkon_idangle();
   d_linecrit  = varkon_linecrit();

  if ( in_ctol < 0 ) ctol = d_ctol;
  else               ctol = in_ctol;
  if ( in_ntol < 0 ) idangle = d_ntol;
  else               idangle = in_ntol;
  if  ( 3 == a_case )
    {
    if ( in_rtol < 0 ) lcrit   = d_linecrit;
    else               lcrit   = in_rtol;
    }

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur722 ctol %10.6f  idangle %10.6f  lcrit %15.8f\n",
        ctol ,   idangle,    lcrit    );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  2. Allocate memory for the output curve                         */
/*  ________________________________________                        */
/*                                                                  */
/* Size of area is defined by c_type and n_req.                     */
/* Allocation with function DBcreate_segments.                      */
/*                                                                 !*/

  if       ( c_type ==  1  ) n_alloc = (n_req+1)*p_cur->ns_cu;
  else if  ( c_type ==  3  ) 
    {
    if ( d_che < 0.1  )
      {
      sprintf(errbuf,"d_che< 0.1%%sur722");
      return(varkon_erpush("SU2993",errbuf));
      }

    if ( p_cur->al_cu  >  d_che )
      {
      n_alloc = (DBint)(p_cur->al_cu/d_che)+p_cur->ns_cu;
      }
    else  
      {
      n_alloc = 4*p_cur->ns_cu;
      }
    }  /*  End c_type == 3  */

  else if  ( c_type == 2 ) 
    {
    if ( d_che < 0.1  && n_req < 1 )
      {
      sprintf(errbuf,"d_che< 0.1,n_req<1%%sur722");
      return(varkon_erpush("SU2993",errbuf));
      }

    if ( p_cur->al_cu  >  d_che && d_che > 0.0 )
      {
      n_alloc = (DBint)(p_cur->al_cu/d_che)+p_cur->ns_cu;
      }
    else  
      {
      n_alloc = 20*p_cur->ns_cu;    /* 1996-11-03 4->20 */
      }

    }  /*  End c_type == 2 */



  else
    {                 
    sprintf(errbuf,  "(c_type allocation)%%sur722");
     return(varkon_erpush("SU2993",errbuf));
    }

  if((*pp_oseg = DBcreate_segments(n_alloc))==NULL)
    {                 
    sprintf(errbuf,  "(alloc)%%sur722");
    return(varkon_erpush("SU2993",errbuf));
    }                                     

/*!                                                                 */
/* Initialize segment data in p_ocur and pp_oseg.                   */
/* Calls of varkon_ini_gmcur (sur778) and varkon_ini_gmseg (sur779) */
/*                                                                 !*/

    varkon_ini_gmcur (p_ocur);

   for (i_s=1; i_s<= n_alloc; i_s= i_s+1)
     {
     varkon_ini_gmseg ( *pp_oseg+i_s-1);
     }

   p_outseg = *pp_oseg;                   /* Avoid problems ....    */


/*!                                                                 */
/* 3. Approximation computation data to structure variable appdat.  */
/* _______________________________________________________________  */
/*                                                                 !*/


appdat.ctol    = ctol;
appdat.ntol    = idangle;
appdat.acase   = a_case;
appdat.ctype   = c_type;
if ( 3 == a_case ) appdat.t_type = 1;
appdat.n_req   = n_req;  
appdat.n_alloc = n_alloc;
appdat.comptol = comptol;
appdat.d_che   = d_che;
appdat.lcrit   = lcrit;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur722 appdat.ctol %f appdat.d_che %f appdat.n_alloc %d \n", 
        appdat.ctol,   appdat.d_che, (short)appdat.n_alloc );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 4. Create the approximate curve for the different cases          */
/* _______________________________________________________          */
/*                                                                 !*/

/*!                                                                 */
/* c_type= 1:               Call varkon_cur_approxseg (sur723)      */
/* c_type= 2:               Call varkon_cur_approxadd (sur725)      */
/* c_type= 3:               Call varkon_cur_approxrdr (sur724)      */
/* c_type= 3 and a_case= 3  Call varkon_cur_approxrci (sur726)      */
/*                                                                 !*/

   if     ( c_type == 1 && a_case == 2 )
     {
    status = varkon_cur_approxseg
    ( p_cur,p_seg, &appdat, p_ocur, p_outseg );
    if(status<0)
    {
    sprintf(errbuf,"sur725%%sur722");
    return(varkon_erpush("SU2943",errbuf));
    }
     }      /* End c_type == 1 && a_case == 2 */


   else if     ( c_type == 2 && (a_case == 2 || a_case == 5) )
     {
    status = varkon_cur_approxadd
    ( p_cur,p_seg, &appdat, p_ocur, p_outseg );
    if(status<0)
    {
    sprintf(errbuf,"sur725%%sur722");
    return(varkon_erpush("SU2943",errbuf));
    }
     }      /* End c_type == 2 && (a_case == 2 || a_case == 5) */



   else if ( c_type == 3 && a_case == 5 )
     {
    status = varkon_cur_approxrdr
    ( p_cur,p_seg, &appdat, p_ocur, p_outseg );
    if(status<0)
    {
    sprintf(errbuf,"sur724%%sur722");
    return(varkon_erpush("SU2943",errbuf));
    }
     }      /* End c_type == 3 && a_case == 5 */

   else if ( c_type == 2 && a_case == 3 )
     {
    status = varkon_cur_approxrci
    ( p_cur,p_seg, &appdat, p_ocur, p_outseg );
    if(status<0)
    {
    sprintf(errbuf,"sur726%%sur722");
    return(varkon_erpush("SU2943",errbuf));
    }
     }      /* End c_type == 2 && a_case == 3 */

  else
    {                 
    sprintf(errbuf,  "(c_type & a_case)%%sur722");
    return(varkon_erpush("SU2993",errbuf));
    }




#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur722 Exit p_ocur->ns_cu %d \n", p_ocur->ns_cu  );
fflush(dbgfil(SURPAC));
}
#endif

  return(SUCCED);

} /* End of function */


/********************************************************************/
