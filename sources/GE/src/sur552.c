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
/*  Function: varkon_vda_rsur2                       File: sur552.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve parameter values  for the VDA-FS surface.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-01   Originally written                                 */
/*  1998-03-31   Debug                                              */
/*  1999-12-04   Free source code modifications                     */
/*  2001-07-12   rest line with possible parameter values added     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rsur2      Parameter values VDA-FS surface  */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol      * Retrieve computer tolerance                */
/* varkon_vda_rsur3    * Get all floats from VDA-FS line            */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_vda_rsur2  (sur552) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short addparam();      /* Add parameter values               */
/*                                                                  */
/*-----------------------------------------------------------------!*/
/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   n_param;      /* Number of parameters for the surface*/
static DBint   n_retrieved;  /* Number of retrieved parameters      */
static DBint   n_upar;       /* Number of parameter values in all_u */
static DBint   n_vpar;       /* Number of parameter values in all_v */
static DBfloat all_u[NUMAX]; /* All U parameter values              */
static DBfloat all_v[NVMAX]; /* All V parameter values              */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rsur2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   FILE    *f_vda,       /* Input VDA-FS file                 (ptr) */
   char    rest[],       /* Substring with possible parameter values*/
   DBint    nu,          /* Number of patches in U direction  (ptr) */
   DBint    nv,          /* Number of patches in V direction  (ptr) */
   DBPatch *p_pat )      /* Topological patch data            (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/* Data to                                                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   max_l;        /* Maximum number of lines in input file   */
   DBint   i_l;          /* Loop index line in input file           */
   DBint   iu;           /* Loop index U patch                      */
   DBint   iv;           /* Loop index V patch                      */
   DBPatch *p_t;         /* Current patch                     (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   i_p;          /* Loop index parameter value              */
   DBfloat comptol;      /* Computer tolerance (accuracy)           */
   char    rad[133];     /* One line in the VDA-FS file             */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check of input data and initializations                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur552*Enter*  \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Number parameter values:                                         */
   n_param = nu + 1 + nv +1;
/*                                                                 !*/

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

   n_retrieved = 0;
   n_upar      = 0;
   n_vpar      = 0;


/*!                                                                 */
/* Loop for all lines in the input VDA-FS file. There is a maximum  */
/* of max_l= nu*nv+2 as a program check.                            */
/* First get parameters from the input (part of the) previous line  */
/*                                                                 !*/

  status= addparam( rest, nu, nv );
  if (status<0) 
    return(status);

max_l   = nu*nv+2;
i_l     =     0;

while ( fgets(rad,132,f_vda)  != NULL )
  {
/*!                                                                 */
/*      i. Check that number of interations not is exceeded         */
/*                                                                 !*/
  i_l = i_l + 1;
  if ( i_l > max_l )
    {
    sprintf(errbuf,"read not OK%%sur552");
    return(varkon_erpush("SU2993",errbuf));
    }

    status= addparam( rad, nu, nv );
    if (status<0) 
      return(status);

    if ( n_retrieved >= n_param ) goto allpar;

  }                /* End while */


  sprintf(errbuf,"n_param>n_retrieved%%sur552");
  return(varkon_erpush("SU2993",errbuf));


allpar:; /*! Label: All parameter values retrieved                 !*/

/*!                                                                 */
/* Check that there is no extra data is on the last line            */
/*                                                                 !*/

  if  ( n_param != n_retrieved )
    {
    sprintf(errbuf,"n_param!=n_retrieved%%sur552");
    return(varkon_erpush("SU2993",errbuf));
    }
    
/* Temporarely for Michael No check !!!!!  */
/*!                                                                 */
/* Only programmed for 0-1 parameter intervals. Check !             */
/*                                                                 !*/
/*
  for ( i_p = 1; i_p < n_upar; ++i_p )
    {
    if  (  fabs(all_u[i_p]-all_u[i_p-1]-1.0) > 10.0*comptol )
      {
      sprintf(errbuf,"not 0-1 U value%%sur552");
      return(varkon_erpush("SU2993",errbuf));
      }
    }

  for ( i_p = 1; i_p < n_vpar; ++i_p )
    {
    if  (  fabs(all_v[i_p]-all_v[i_p-1]-1.0) > 10.0*comptol )
      {
      sprintf(errbuf,"not 0-1 V value%%sur552");
      return(varkon_erpush("SU2993",errbuf));
      }
    }
*/
/*!                                                                 */
/* Parameter values to the topological patches.                     */
/*                                                                 !*/


for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

    p_t           = p_pat  + iu*nv + iv; 

    p_t->iu_pat   = (short)(iu + 1);
    p_t->iv_pat   = (short)(iv + 1);
    p_t->us_pat   = (DBfloat)iu+1.0;
    p_t->ue_pat   = (DBfloat)iu+2.0-comptol;
    p_t->vs_pat   = (DBfloat)iv+1.0;
    p_t->ve_pat   = (DBfloat)iv+2.0-comptol;

     }
   }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur552 n_upar %d n_vpar %d\n",(short)n_upar,(short)n_vpar);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur552 Exit*varkon_vda_rsur2 * i_l %d n_param %d n_retrieved %d\n", 
                  (short)i_l, (short)n_param, (short)n_retrieved );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/


/*!********* Internal ** function ** addparam ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function adds parameter values to array                      */

   static short addparam( rad, nu, nv )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   char    rad[];        /* One line in the VDA-FS file             */
   DBint    nu;          /* Number of patches in U direction  (ptr) */
   DBint    nv;          /* Number of patches in V direction  (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   n_flt;        /* Number of floats in VDA-FS line   (ptr) */
   DBfloat all_flt[SMAX];/* All floats from one VDA-FS line   (ptr) */
/*----------------------------------------------------------------- */
   DBint   i_f;          /* Loop index line float                   */
   DBint   status;       /* Error code from a called function       */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/
  for ( i_f = 0; i_f < NUMAX; ++i_f )
    {
    all_u[i_f] = F_UNDEF;
    }

/*!                                                                 */
/*     ii. Get all floats from the current line                     */
/*                                                                 !*/

   status=varkon_vda_rsur3 ( rad, &n_flt, all_flt );
    if (status<0) 
        {
        sprintf(errbuf,"sur553%%sur552");
        return(varkon_erpush("SU2943",errbuf));
        }
/*!                                                                 */
/*    iii. Loop all floats from the line                            */
/*                                                                 !*/

  if ( n_flt <= 0 )
    {
    sprintf(errbuf,"n_flt=0%%sur552");
    return(varkon_erpush("SU2993",errbuf));
    }

  for ( i_f = 1; i_f <= n_flt; ++i_f )
    {
    if        ( n_upar <= nu  )
      {
      all_u[n_upar] = all_flt[i_f-1];
      n_upar = n_upar + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur552 nu %d n_upar %d all_u[] %f\n",
       (int)nu,  (int)n_upar, all_u[n_upar-1]);
fflush(dbgfil(SURPAC));
}
#endif
      }
    else if   ( n_vpar <= nv  )
      {
      all_v[n_vpar] = all_flt[i_f-1];
      n_vpar = n_vpar + 1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur552 nv %d n_vpar %d all_v[] %f\n",
       (int)nv,  (int)n_vpar, all_v[n_vpar-1]);
fflush(dbgfil(SURPAC));
}
#endif
      }
    else
      {
      sprintf(errbuf,"all_u/all_v%%sur552");
      return(varkon_erpush("SU2993",errbuf));
      }

    }  /* End loop i_f */


/*!                                                                 */
/*     iv. Goto allpar if all parameter values are retrieved        */
/*                                                                 !*/
    n_retrieved = n_retrieved + n_flt;


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


