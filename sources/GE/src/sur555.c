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
/*  Function: varkon_vda_rp3                         File: sur555.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve VDA-FS coefficients for P3_PAT  patch.                 */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-02   Originally written (by MBS module p_reavdav0)      */
/*  1999-12-04   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rp3        Read P3_PAT  VDA-FS coefficients */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_vda_rsur3    * Get all floats from VDA-FS line            */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_vda_rp3    (sur555) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rp3 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   FILE    *f_vda,       /* Input VDA-FS file                 (ptr) */
   DBint    n_degu,      /* Degree in U direction             (ptr) */
   DBint    n_degv,      /* Degree in V direction             (ptr) */
   DBPatch *p_t )        /* Topological (current) patch       (ptr) */
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
   DBint   i_f;          /* Loop index line float                   */
   DBint   n_flt;        /* Number of floats in VDA-FS line   (ptr) */
   DBfloat all_flt[SMAX];/* All floats from one VDA-FS line   (ptr) */
   DBint   n_coeff;      /* Number of coefficients for the patch    */
   DBint   n_retrieved;  /* Number of retrieved coefficients        */
   DBint   n_maxcoeff;   /* Maximum number of (all) coefficients    */
   DBfloat ret_coeff[48];/* Retrieved coefficients                  */
   DBfloat all_coeff[48];/* All coefficients                        */
   DBint   n_degree;     /* Degree for patch P3_PAT                 */
   DBint   i_c;          /* Loop index X, Y and Z component         */
   DBint   i_u;          /* Loop index horizontal row in C matrix   */
   DBint   i_v;          /* Loop index vertical   row in C matrix   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   GMPATP3  *p_g;        /* Current geometric   patch         (ptr) */
   char     *p_gpat;     /* Allocated area geom.  patch data  (ptr) */
   char      rad[133];   /* One line in the VDA-FS file             */
   DBint     status;     /* Error code from a called function       */
   char      errbuf[80]; /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check of input data and initializations                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur555*Enter*  \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Number of coefficients values:                                   */
   n_coeff = 3 * (n_degu + 1) * (n_degv +1);
/*                                                                 !*/

  n_degree = 3  ;  

  for ( i_f = 0; i_f < 48 ; ++i_f )
    {
    ret_coeff[i_f] = F_UNDEF;
    all_coeff[i_f] = F_UNDEF;
    }

/*!                                                                 */
/* Loop for all lines in the input VDA-FS file. There is a maximum  */
/* of max_l= n_coeff as a program check.                            */
/*                                                                 !*/

max_l   = n_coeff;
i_l     =     0;
n_retrieved = 0;

while ( fgets(rad,132,f_vda)  != NULL )
  {
/*!                                                                 */
/*      i. Check that number of iterations not is exceeded          */
/*                                                                 !*/
  i_l = i_l + 1;
  if ( i_l > max_l )
    {
    sprintf(errbuf,"read not OK%%sur555");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*     ii. Get all floats from the current line                     */
/*                                                                 !*/

   status=varkon_vda_rsur3 ( rad, &n_flt, all_flt );
    if (status<0) 
        {
        sprintf(errbuf,"sur553%%sur555");
        return(varkon_erpush("SU2943",errbuf));
        }
/*!                                                                 */
/*    iii. Loop all floats from the line                            */
/*                                                                 !*/

  if ( n_flt <= 0 )
    {
    sprintf(errbuf,"n_flt=0%%sur555");
    return(varkon_erpush("SU2993",errbuf));
    }

    for ( i_f = 1; i_f <= n_flt; ++i_f )
      {
      ret_coeff[n_retrieved] = all_flt[i_f-1];
      n_retrieved = n_retrieved + 1;
      }  /* End loop i_f     */


/*!                                                                 */
/*     iv. Goto allpar if all coefficient values are retrieved      */
/*                                                                 !*/
    if ( n_retrieved >= n_coeff ) goto allpar;

  }                /* End while */


  sprintf(errbuf,"n_coeff>n_retrieved%%sur555");
  return(varkon_erpush("SU2993",errbuf));


allpar:; /*! Label: All coefficient values retrieved               !*/

/*!                                                                 */
/* Check that there is no extra data is on the last line            */
/*                                                                 !*/

  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved%%sur555");
    return(varkon_erpush("SU2993",errbuf));
    }
    

/*!                                                                 */
/* Dynamic allocation of area for one geometric (P3_PAT ) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(P3_PAT ,1)) == NULL)
 { 
 sprintf(errbuf, "(allocg)%%sur555");
 return(varkon_erpush("SU2993",errbuf));
 }

  p_g    = (GMPATP3  *)p_gpat;           /* Current geometric patch */

  p_t->styp_pat = P3_PAT ;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/

/*!                                                                 */
/* Coefficients ret_coeff to all_coeff.                             */
/*                                                                 !*/

   n_retrieved = 0;
   n_maxcoeff  = 0;

  for ( i_c = 1; i_c <=  3; ++i_c )
    {
    for ( i_v = 0; i_v <=  n_degree; ++i_v )
      {
      for ( i_u = 0; i_u <=  n_degree; ++i_u )
        {
        if    ( i_u <= n_degu && i_v <= n_degv )
          {
          all_coeff[n_maxcoeff] = ret_coeff[n_retrieved];
          n_retrieved = n_retrieved + 1;
          }
        else
          {
          all_coeff[n_maxcoeff] = 0.0;
          }
        n_maxcoeff  = n_maxcoeff + 1;
        }
      }
    }


#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 && n_degu < n_degv ) /* Not a tested case */
{
for ( i_l = 0; i_l <   48 ; ++i_l )
{
fprintf(dbgfil(SURPAC),
"sur555 ret_coeff[%d]= %f all_coeff[%d]= %f \n", 
 (short)i_l, ret_coeff[i_l], (short)i_l, all_coeff[i_l] );
}
fflush(dbgfil(SURPAC));
}
  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved (2)%%sur555");
#ifdef LNAME
    return(varkon_erpush("SU2993",errbuf));
#endif
#ifdef SNAME
    return(erpush("SU2993",errbuf));
#endif
    }
#endif

/*!                                                                 */
/* Coefficients to output geometry patch.                           */
/*                                                                 !*/

    p_g->c0000x = all_coeff[   0];            /*    u** 0 v** 0     */
    p_g->c0100x = all_coeff[   1];            /*    u** 1 v** 0     */
    p_g->c0200x = all_coeff[   2];            /*    u** 2 v** 0     */
    p_g->c0300x = all_coeff[   3];            /*    u** 3 v** 0     */

    p_g->c0001x = all_coeff[   4];            /*    u** 0 v** 1     */
    p_g->c0101x = all_coeff[   5];            /*    u** 1 v** 1     */
    p_g->c0201x = all_coeff[   6];            /*    u** 2 v** 1     */
    p_g->c0301x = all_coeff[   7];            /*    u** 3 v** 1     */

    p_g->c0002x = all_coeff[   8];            /*    u** 0 v** 2     */
    p_g->c0102x = all_coeff[   9];            /*    u** 1 v** 2     */
    p_g->c0202x = all_coeff[  10];            /*    u** 2 v** 2     */
    p_g->c0302x = all_coeff[  11];            /*    u** 3 v** 2     */

    p_g->c0003x = all_coeff[  12];            /*    u** 0 v** 3     */
    p_g->c0103x = all_coeff[  13];            /*    u** 1 v** 3     */
    p_g->c0203x = all_coeff[  14];            /*    u** 2 v** 3     */
    p_g->c0303x = all_coeff[  15];            /*    u** 3 v** 3     */


    p_g->c0000y = all_coeff[  16];            /*    u** 0 v** 0     */
    p_g->c0100y = all_coeff[  17];            /*    u** 1 v** 0     */
    p_g->c0200y = all_coeff[  18];            /*    u** 2 v** 0     */
    p_g->c0300y = all_coeff[  19];            /*    u** 3 v** 0     */

    p_g->c0001y = all_coeff[  20];            /*    u** 0 v** 1     */
    p_g->c0101y = all_coeff[  21];            /*    u** 1 v** 1     */
    p_g->c0201y = all_coeff[  22];            /*    u** 2 v** 1     */
    p_g->c0301y = all_coeff[  23];            /*    u** 3 v** 1     */

    p_g->c0002y = all_coeff[  24];            /*    u** 0 v** 2     */
    p_g->c0102y = all_coeff[  25];            /*    u** 1 v** 2     */
    p_g->c0202y = all_coeff[  26];            /*    u** 2 v** 2     */
    p_g->c0302y = all_coeff[  27];            /*    u** 3 v** 2     */

    p_g->c0003y = all_coeff[  28];            /*    u** 0 v** 3     */
    p_g->c0103y = all_coeff[  29];            /*    u** 1 v** 3     */
    p_g->c0203y = all_coeff[  30];            /*    u** 2 v** 3     */
    p_g->c0303y = all_coeff[  31];            /*    u** 3 v** 3     */


    p_g->c0000z = all_coeff[  32];            /*    u** 0 v** 0     */
    p_g->c0100z = all_coeff[  33];            /*    u** 1 v** 0     */
    p_g->c0200z = all_coeff[  34];            /*    u** 2 v** 0     */
    p_g->c0300z = all_coeff[  35];            /*    u** 3 v** 0     */

    p_g->c0001z = all_coeff[  36];            /*    u** 0 v** 1     */
    p_g->c0101z = all_coeff[  37];            /*    u** 1 v** 1     */
    p_g->c0201z = all_coeff[  38];            /*    u** 2 v** 1     */
    p_g->c0301z = all_coeff[  39];            /*    u** 3 v** 1     */

    p_g->c0002z = all_coeff[  40];            /*    u** 0 v** 2     */
    p_g->c0102z = all_coeff[  41];            /*    u** 1 v** 2     */
    p_g->c0202z = all_coeff[  42];            /*    u** 2 v** 2     */
    p_g->c0302z = all_coeff[  43];            /*    u** 3 v** 2     */

    p_g->c0003z = all_coeff[  44];            /*    u** 0 v** 3     */
    p_g->c0103z = all_coeff[  45];            /*    u** 1 v** 3     */
    p_g->c0203z = all_coeff[  46];            /*    u** 2 v** 3     */
    p_g->c0303z = all_coeff[  47];            /*    u** 3 v** 3     */






    p_g->ofs_pat = 0.0;                  /* Offset for patch        */




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur555 Exit*varkon_vda_rp3   * i_l %d n_coeff %d n_retrieved %d\n", 
                  (short)i_l, (short)n_coeff, (short)n_retrieved );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
