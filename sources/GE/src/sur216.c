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
/*  Function: varkon_cur_mbseval                   File: sur216.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter value t on a curve.                           */
/*                                                                  */
/*  Output is one (1) vector with the requested derivative.         */
/*  The function corresponds to MBS statement: EVAL (curve point)   */
/*                                                                  */
/*  Not yet implemented:                                            */
/*  - Handling of curvature cases planar, developable,              */
/*    spherical, i.e. the logical variables in structure            */
/*    variable EVALC (function value of varkon_sur_uvsegeval).      */
/*                                                                  */ 
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1994-11-12   evltyp for different cases                         */
/*  1996-11-04   evltyp for UV, SEGTYPE and SEGADR                  */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_mbseval    Curve   MBS evaluation routine   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE109                   * Curve evaluation function              */
/* varkon_ini_evalc        * Initialize EVALC                       */
/* varkon_erinit           * Initiate error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2733 = Curve EVAL case is not valid        varkon_cur_mbseval  */
/* SU2723 = Curve evaluation failed for t= ...  varkon_cur_mbseval  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
/* SU2993 = Severe program error in varkon_cur_mbseval (sur216)     */


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_cur_mbseval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Alloc. area for segment coeff.'s  (ptr) */
   char    *ecase,       /* Evaluation case                         */
   DBfloat  tglob,       /* Global u value                          */
   DBVector *p_outvec )  /* Output derivative vector          (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!----- Evaluation cases described with examples ------------------*/
/*                                                                  */
/* Free curve (a segment of type CUB_SEG):                          */
/*                                                                  */
/* tlocal     0.30000000                                            */
/* segadr     2.00000000                                            */
/*                                                                  */
/* xyz      349.99993730     156.21372573     233.20676524          */
/* drdt       0.00003999      95.24445612      13.60192388          */
/* d2rdt2     0.00038189      -0.28038114       2.10419911          */
/*                                                                  */
/* segtype    1.00000000                                            */
/*                                                                  */
/* prinormal  0.00017990      -0.14137626       0.98995592          */
/* binormal   0.99999998       0.00002502      -0.00017815          */
/*                                                                  */
/* kappa      0.00022932                                            */
/*                                                                  */
/* Surface curve (a segment of type UV_CUB_SEG):                        */
/*                                                                  */
/* tlocal     0.20000000                                            */
/* segadr     1.00000000                                            */
/*                                                                  */
/* xyz      299.99998851      55.24711822     185.89848359          */
/* drdt       0.00001075       3.29409137       0.14923647          */
/* d2rdt2     0.00004468       0.00580593       0.00922601          */
/*                                                                  */
/* segtype    2.00000000                                            */
/*                                                                  */
/* prinormal  0.00498801      -0.04525732       0.99896291          */
/* binormal   0.99998756       0.00022249      -0.00498305          */
/*                                                                  */
/* kappa      0.00082348                                            */
/* normkappa  0.00066032                                            */
/* geodesic  -0.00049203                                            */
/*                                                                  */
/* uv        11.00531678       1.95521782                           */
/* duvdt      0.02674440       0.05593214                           */
/* d2uvdt2    0.00160491       0.00011398                           */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALC   xyz;          /* Coordinates and derivatives from curve  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   segtype;      /* Segment type                            */

   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */

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
  "sur216 Enter *** varkon_cur_mbseval: ecase %s tglob= %f\n",
                      ecase, tglob);
  }
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

/* Initiate variabel EVALS                                          */
/* Call of varkon_ini_evals (sur170).                               */
/*                                                                 !*/

   varkon_ini_evalc (&xyz);

   xyz.t_global = tglob;   /* Global parameter value                */
   if      ( strcmp(ecase,"xyz")  == 0  || 
             strcmp(ecase,"XYZ")  == 0    )
   xyz.evltyp   = EVC_R; 
   else if ( strcmp(ecase,"drdt") == 0  || 
             strcmp(ecase,"DRDT") == 0    )
   xyz.evltyp   = EVC_DR; 
   else if ( strcmp(ecase,"d2rdt2") == 0  || 
             strcmp(ecase,"D2RDT2") == 0    )
   xyz.evltyp   = EVC_D2R; 
   else if ( strcmp(ecase,"uv") == 0  || 
             strcmp(ecase,"UV") == 0    )
   xyz.evltyp   = EVC_R; 
   else if ( strcmp(ecase,"segtype")   == 0  || 
             strcmp(ecase,"SEGTYPE")   == 0    )
   xyz.evltyp   = EVC_R; 
   else if ( strcmp(ecase,"segadr")   == 0  || 
             strcmp(ecase,"SEGADR")   == 0    )
   xyz.evltyp   = EVC_R; 
   else
   xyz.evltyp   = EVC_KAP; /* All derivatives                       */

/*!                                                                 */
/* 2. Curve evaluation                                              */
/* ___________________                                              */
/*                                                                  */
/* Calculate coordinates and derivatives.                           */
/* Call of varkon_GE109 (GE109).                                  */
/*                                                                 !*/


status=GE109 ((DBAny *)p_cur ,p_seg ,&xyz  );
if (status<0) 
   {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur216 GE109 failed for tglob %f\n",
   tglob);
  }
 fflush(dbgfil(SURPAC)); 
#endif
   sprintf(errbuf,"%f %%  (varkon_cur_mbseval)",tglob-1.0);
   return(varkon_erpush("SU2723",errbuf));
   }


/*!                                                                 */
/* 3. Output vector                                                 */
/* ________________                                                 */
/*                                                                  */
/* Requested derivative vector to output variable p_outvec.         */
/*                                                                 !*/


   if ( xyz.surpat == TRUE )
      segtype =  UV_CUB_SEG;
   else
      segtype = CUB_SEG;



   if      ( strcmp(ecase,"xyz")  == 0  || 
             strcmp(ecase,"XYZ")  == 0    )
      {
      *p_outvec = xyz.r;
      }
   else if ( strcmp(ecase,"drdt") == 0  || 
             strcmp(ecase,"DRDT") == 0    )
      {
      *p_outvec = xyz.drdt;
      }
   else if ( strcmp(ecase,"d2rdt2") == 0  || 
             strcmp(ecase,"D2RDT2") == 0    )
      {
      *p_outvec = xyz.d2rdt2;
      }
   else if ( strcmp(ecase,"uv") == 0  || 
             strcmp(ecase,"UV") == 0    )
      {
      if ( segtype == CUB_SEG )
      {    
      sprintf(errbuf," %s for surface %% (varkon_cur_mbseval)",ecase);
      varkon_erinit();
      return(varkon_erpush("SU2733",errbuf));
      }                            
      p_outvec->x_gm = xyz.u;   
      p_outvec->y_gm = xyz.v;   
      p_outvec->z_gm = 0.0;     
      }

   else if ( strcmp(ecase,"duvdt") == 0  || 
             strcmp(ecase,"DUVDT") == 0    )
      {
      if ( segtype == CUB_SEG )
      {    
      sprintf(errbuf," %s for surface %% (varkon_cur_mbseval)",ecase);
      varkon_erinit();
      return(varkon_erpush("SU2733",errbuf));
      }                            
      p_outvec->x_gm = xyz.u_t; 
      p_outvec->y_gm = xyz.v_t; 
      p_outvec->z_gm = 0.0;     
      }
   else if ( strcmp(ecase,"d2uvdt2") == 0  || 
             strcmp(ecase,"D2UVDT2") == 0    )
      {
      if ( segtype == CUB_SEG )
      {    
      sprintf(errbuf," %s for surface %% (varkon_cur_mbseval)",ecase);
      varkon_erinit();
      return(varkon_erpush("SU2733",errbuf));
      }                            
      p_outvec->x_gm = xyz.u_t2;
      p_outvec->y_gm = xyz.v_t2;
      p_outvec->z_gm = 0.0;     
      }




   else if ( strcmp(ecase,"binormal") == 0  || 
             strcmp(ecase,"BINORMAL") == 0    )
      {
      *p_outvec = xyz.b_norm;
      }
   else if ( strcmp(ecase,"prinormal") == 0  || 
             strcmp(ecase,"PRINORMAL") == 0    )
      {
      *p_outvec = xyz.p_norm;
      }
   else if ( strcmp(ecase,"tlocal")   == 0  || 
             strcmp(ecase,"TLOCAL")   == 0    )
      {
      p_outvec->x_gm = xyz.t_local; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"kappa")   == 0  || 
             strcmp(ecase,"KAPPA")   == 0    )
      {
      p_outvec->x_gm = xyz.kappa; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"normkappa")   == 0  || 
             strcmp(ecase,"NORMKAPPA")   == 0    )
      {
      p_outvec->x_gm = xyz.nkappa; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"geodesic")   == 0  || 
             strcmp(ecase,"GEODESIC")   == 0    )
      {
      p_outvec->x_gm = xyz.geodesic; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"segtype")   == 0  || 
             strcmp(ecase,"SEGTYPE")   == 0    )
      {
      p_outvec->x_gm = (DBfloat)segtype + 0.000000000001; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }
   else if ( strcmp(ecase,"segadr")   == 0  || 
             strcmp(ecase,"SEGADR")   == 0    )
      {
      p_outvec->x_gm = (DBfloat)xyz.iseg  +0.000000000001; 
      p_outvec->y_gm = 0.0;     
      p_outvec->z_gm = 0.0;        
      }



    else
      {
      sprintf(errbuf," %s %% (varkon_cur_mbseval)",ecase);
      varkon_erinit();
      return(varkon_erpush("SU2733",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur216 Exit*varkon_cur_mbseval ecase %s outvec %f %f %f\n",
    ecase, p_outvec->x_gm,p_outvec->y_gm,p_outvec->z_gm );
  }
 fflush(dbgfil(SURPAC)); 
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

