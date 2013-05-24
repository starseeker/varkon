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
/*  Function: varkon_vda_rp9                         File: sur558.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve VDA-FS coefficients for P9_PAT  patch.                 */
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
/*sdescr varkon_vda_rp9        Read P9_PAT  VDA-FS coefficients */
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
/* SU2993 = Severe program error (  ) in varkon_vda_rp9    (sur558) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rp9 (

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
   DBfloat ret_coeff[300];/* Retrieved coefficients                 */
   DBfloat all_coeff[300];/* All coefficients                       */
   DBint   n_degree;     /* Degree for patch P9_PAT                 */
   DBint   i_c;          /* Loop index X, Y and Z component         */
   DBint   i_u;          /* Loop index horizontal row in C matrix   */
   DBint   i_v;          /* Loop index vertical   row in C matrix   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   GMPATP9  *p_g;        /* Current geometric   patch         (ptr) */
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
fprintf(dbgfil(SURPAC),"sur558*Enter*  \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Number of coefficients values:                                   */
   n_coeff = 3 * (n_degu + 1) * (n_degv +1);
/*                                                                 !*/

  n_degree = 9  ;  

  for ( i_f = 0; i_f < 300; ++i_f )
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
    sprintf(errbuf,"read not OK%%sur558");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*     ii. Get all floats from the current line                     */
/*                                                                 !*/

   status=varkon_vda_rsur3 ( rad, &n_flt, all_flt );
    if (status<0) 
        {
        sprintf(errbuf,"sur553%%sur558");
        return(varkon_erpush("SU2943",errbuf));
        }
/*!                                                                 */
/*    iii. Loop all floats from the line                            */
/*                                                                 !*/

  if ( n_flt <= 0 )
    {
    sprintf(errbuf,"n_flt=0%%sur558");
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


  sprintf(errbuf,"n_coeff>n_retrieved%%sur558");
  return(varkon_erpush("SU2993",errbuf));


allpar:; /*! Label: All coefficient values retrieved               !*/

/*!                                                                 */
/* Check that there is no extra data is on the last line            */
/*                                                                 !*/

  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved%%sur558");
    return(varkon_erpush("SU2993",errbuf));
    }
    

/*!                                                                 */
/* Dynamic allocation of area for one geometric (P9_PAT ) patch.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(P9_PAT ,1)) == NULL)
 { 
 sprintf(errbuf, "(allocg)%%sur558");
 return(varkon_erpush("SU2993",errbuf));
 }

  p_g    = (GMPATP9  *)p_gpat;           /* Current geometric patch */

  p_t->styp_pat = P9_PAT ;               /* Type of secondary patch */
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
for ( i_l = 0; i_l <   300; ++i_l )
{
fprintf(dbgfil(SURPAC),
"sur558 ret_coeff[%d]= %f all_coeff[%d]= %f \n", 
 (short)i_l, ret_coeff[i_l], (short)i_l, all_coeff[i_l] );
}
fflush(dbgfil(SURPAC));
}
  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved (2)%%sur558");
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

/*!                                                                 */
/* Coefficients to output geometry patch.                           */
/*                                                                 !*/

    p_g->c0000x = all_coeff[   0];            /*    u** 0 v** 0     */
    p_g->c0100x = all_coeff[   1];            /*    u** 1 v** 0     */
    p_g->c0200x = all_coeff[   2];            /*    u** 2 v** 0     */
    p_g->c0300x = all_coeff[   3];            /*    u** 3 v** 0     */
    p_g->c0400x = all_coeff[   4];            /*    u** 4 v** 0     */
    p_g->c0500x = all_coeff[   5];            /*    u** 5 v** 0     */
    p_g->c0600x = all_coeff[   6];            /*    u** 6 v** 0     */
    p_g->c0700x = all_coeff[   7];            /*    u** 7 v** 0     */
    p_g->c0800x = all_coeff[   8];            /*    u** 8 v** 0     */
    p_g->c0900x = all_coeff[   9];            /*    u** 9 v** 0     */

    p_g->c0001x = all_coeff[  10];            /*    u** 0 v** 1     */
    p_g->c0101x = all_coeff[  11];            /*    u** 1 v** 1     */
    p_g->c0201x = all_coeff[  12];            /*    u** 2 v** 1     */
    p_g->c0301x = all_coeff[  13];            /*    u** 3 v** 1     */
    p_g->c0401x = all_coeff[  14];            /*    u** 4 v** 1     */
    p_g->c0501x = all_coeff[  15];            /*    u** 5 v** 1     */
    p_g->c0601x = all_coeff[  16];            /*    u** 6 v** 1     */
    p_g->c0701x = all_coeff[  17];            /*    u** 7 v** 1     */
    p_g->c0801x = all_coeff[  18];            /*    u** 8 v** 1     */
    p_g->c0901x = all_coeff[  19];            /*    u** 9 v** 1     */

    p_g->c0002x = all_coeff[  20];            /*    u** 0 v** 2     */
    p_g->c0102x = all_coeff[  21];            /*    u** 1 v** 2     */
    p_g->c0202x = all_coeff[  22];            /*    u** 2 v** 2     */
    p_g->c0302x = all_coeff[  23];            /*    u** 3 v** 2     */
    p_g->c0402x = all_coeff[  24];            /*    u** 4 v** 2     */
    p_g->c0502x = all_coeff[  25];            /*    u** 5 v** 2     */
    p_g->c0602x = all_coeff[  26];            /*    u** 6 v** 2     */
    p_g->c0702x = all_coeff[  27];            /*    u** 7 v** 2     */
    p_g->c0802x = all_coeff[  28];            /*    u** 8 v** 2     */
    p_g->c0902x = all_coeff[  29];            /*    u** 9 v** 2     */

    p_g->c0003x = all_coeff[  30];            /*    u** 0 v** 3     */
    p_g->c0103x = all_coeff[  31];            /*    u** 1 v** 3     */
    p_g->c0203x = all_coeff[  32];            /*    u** 2 v** 3     */
    p_g->c0303x = all_coeff[  33];            /*    u** 3 v** 3     */
    p_g->c0403x = all_coeff[  34];            /*    u** 4 v** 3     */
    p_g->c0503x = all_coeff[  35];            /*    u** 5 v** 3     */
    p_g->c0603x = all_coeff[  36];            /*    u** 6 v** 3     */
    p_g->c0703x = all_coeff[  37];            /*    u** 7 v** 3     */
    p_g->c0803x = all_coeff[  38];            /*    u** 8 v** 3     */
    p_g->c0903x = all_coeff[  39];            /*    u** 9 v** 3     */

    p_g->c0004x = all_coeff[  40];            /*    u** 0 v** 4     */
    p_g->c0104x = all_coeff[  41];            /*    u** 1 v** 4     */
    p_g->c0204x = all_coeff[  42];            /*    u** 2 v** 4     */
    p_g->c0304x = all_coeff[  43];            /*    u** 3 v** 4     */
    p_g->c0404x = all_coeff[  44];            /*    u** 4 v** 4     */
    p_g->c0504x = all_coeff[  45];            /*    u** 5 v** 4     */
    p_g->c0604x = all_coeff[  46];            /*    u** 6 v** 4     */
    p_g->c0704x = all_coeff[  47];            /*    u** 7 v** 4     */
    p_g->c0804x = all_coeff[  48];            /*    u** 8 v** 4     */
    p_g->c0904x = all_coeff[  49];            /*    u** 9 v** 4     */

    p_g->c0005x = all_coeff[  50];            /*    u** 0 v** 5     */
    p_g->c0105x = all_coeff[  51];            /*    u** 1 v** 5     */
    p_g->c0205x = all_coeff[  52];            /*    u** 2 v** 5     */
    p_g->c0305x = all_coeff[  53];            /*    u** 3 v** 5     */
    p_g->c0405x = all_coeff[  54];            /*    u** 4 v** 5     */
    p_g->c0505x = all_coeff[  55];            /*    u** 5 v** 5     */
    p_g->c0605x = all_coeff[  56];            /*    u** 6 v** 5     */
    p_g->c0705x = all_coeff[  57];            /*    u** 7 v** 5     */
    p_g->c0805x = all_coeff[  58];            /*    u** 8 v** 5     */
    p_g->c0905x = all_coeff[  59];            /*    u** 9 v** 5     */

    p_g->c0006x = all_coeff[  60];            /*    u** 0 v** 6     */
    p_g->c0106x = all_coeff[  61];            /*    u** 1 v** 6     */
    p_g->c0206x = all_coeff[  62];            /*    u** 2 v** 6     */
    p_g->c0306x = all_coeff[  63];            /*    u** 3 v** 6     */
    p_g->c0406x = all_coeff[  64];            /*    u** 4 v** 6     */
    p_g->c0506x = all_coeff[  65];            /*    u** 5 v** 6     */
    p_g->c0606x = all_coeff[  66];            /*    u** 6 v** 6     */
    p_g->c0706x = all_coeff[  67];            /*    u** 7 v** 6     */
    p_g->c0806x = all_coeff[  68];            /*    u** 8 v** 6     */
    p_g->c0906x = all_coeff[  69];            /*    u** 9 v** 6     */

    p_g->c0007x = all_coeff[  70];            /*    u** 0 v** 7     */
    p_g->c0107x = all_coeff[  71];            /*    u** 1 v** 7     */
    p_g->c0207x = all_coeff[  72];            /*    u** 2 v** 7     */
    p_g->c0307x = all_coeff[  73];            /*    u** 3 v** 7     */
    p_g->c0407x = all_coeff[  74];            /*    u** 4 v** 7     */
    p_g->c0507x = all_coeff[  75];            /*    u** 5 v** 7     */
    p_g->c0607x = all_coeff[  76];            /*    u** 6 v** 7     */
    p_g->c0707x = all_coeff[  77];            /*    u** 7 v** 7     */
    p_g->c0807x = all_coeff[  78];            /*    u** 8 v** 7     */
    p_g->c0907x = all_coeff[  79];            /*    u** 9 v** 7     */

    p_g->c0008x = all_coeff[  80];            /*    u** 0 v** 8     */
    p_g->c0108x = all_coeff[  81];            /*    u** 1 v** 8     */
    p_g->c0208x = all_coeff[  82];            /*    u** 2 v** 8     */
    p_g->c0308x = all_coeff[  83];            /*    u** 3 v** 8     */
    p_g->c0408x = all_coeff[  84];            /*    u** 4 v** 8     */
    p_g->c0508x = all_coeff[  85];            /*    u** 5 v** 8     */
    p_g->c0608x = all_coeff[  86];            /*    u** 6 v** 8     */
    p_g->c0708x = all_coeff[  87];            /*    u** 7 v** 8     */
    p_g->c0808x = all_coeff[  88];            /*    u** 8 v** 8     */
    p_g->c0908x = all_coeff[  89];            /*    u** 9 v** 8     */

    p_g->c0009x = all_coeff[  90];            /*    u** 0 v** 9     */
    p_g->c0109x = all_coeff[  91];            /*    u** 1 v** 9     */
    p_g->c0209x = all_coeff[  92];            /*    u** 2 v** 9     */
    p_g->c0309x = all_coeff[  93];            /*    u** 3 v** 9     */
    p_g->c0409x = all_coeff[  94];            /*    u** 4 v** 9     */
    p_g->c0509x = all_coeff[  95];            /*    u** 5 v** 9     */
    p_g->c0609x = all_coeff[  96];            /*    u** 6 v** 9     */
    p_g->c0709x = all_coeff[  97];            /*    u** 7 v** 9     */
    p_g->c0809x = all_coeff[  98];            /*    u** 8 v** 9     */
    p_g->c0909x = all_coeff[  99];            /*    u** 9 v** 9     */


    p_g->c0000y = all_coeff[ 100];            /*    u** 0 v** 0     */
    p_g->c0100y = all_coeff[ 101];            /*    u** 1 v** 0     */
    p_g->c0200y = all_coeff[ 102];            /*    u** 2 v** 0     */
    p_g->c0300y = all_coeff[ 103];            /*    u** 3 v** 0     */
    p_g->c0400y = all_coeff[ 104];            /*    u** 4 v** 0     */
    p_g->c0500y = all_coeff[ 105];            /*    u** 5 v** 0     */
    p_g->c0600y = all_coeff[ 106];            /*    u** 6 v** 0     */
    p_g->c0700y = all_coeff[ 107];            /*    u** 7 v** 0     */
    p_g->c0800y = all_coeff[ 108];            /*    u** 8 v** 0     */
    p_g->c0900y = all_coeff[ 109];            /*    u** 9 v** 0     */

    p_g->c0001y = all_coeff[ 110];            /*    u** 0 v** 1     */
    p_g->c0101y = all_coeff[ 111];            /*    u** 1 v** 1     */
    p_g->c0201y = all_coeff[ 112];            /*    u** 2 v** 1     */
    p_g->c0301y = all_coeff[ 113];            /*    u** 3 v** 1     */
    p_g->c0401y = all_coeff[ 114];            /*    u** 4 v** 1     */
    p_g->c0501y = all_coeff[ 115];            /*    u** 5 v** 1     */
    p_g->c0601y = all_coeff[ 116];            /*    u** 6 v** 1     */
    p_g->c0701y = all_coeff[ 117];            /*    u** 7 v** 1     */
    p_g->c0801y = all_coeff[ 118];            /*    u** 8 v** 1     */
    p_g->c0901y = all_coeff[ 119];            /*    u** 9 v** 1     */

    p_g->c0002y = all_coeff[ 120];            /*    u** 0 v** 2     */
    p_g->c0102y = all_coeff[ 121];            /*    u** 1 v** 2     */
    p_g->c0202y = all_coeff[ 122];            /*    u** 2 v** 2     */
    p_g->c0302y = all_coeff[ 123];            /*    u** 3 v** 2     */
    p_g->c0402y = all_coeff[ 124];            /*    u** 4 v** 2     */
    p_g->c0502y = all_coeff[ 125];            /*    u** 5 v** 2     */
    p_g->c0602y = all_coeff[ 126];            /*    u** 6 v** 2     */
    p_g->c0702y = all_coeff[ 127];            /*    u** 7 v** 2     */
    p_g->c0802y = all_coeff[ 128];            /*    u** 8 v** 2     */
    p_g->c0902y = all_coeff[ 129];            /*    u** 9 v** 2     */

    p_g->c0003y = all_coeff[ 130];            /*    u** 0 v** 3     */
    p_g->c0103y = all_coeff[ 131];            /*    u** 1 v** 3     */
    p_g->c0203y = all_coeff[ 132];            /*    u** 2 v** 3     */
    p_g->c0303y = all_coeff[ 133];            /*    u** 3 v** 3     */
    p_g->c0403y = all_coeff[ 134];            /*    u** 4 v** 3     */
    p_g->c0503y = all_coeff[ 135];            /*    u** 5 v** 3     */
    p_g->c0603y = all_coeff[ 136];            /*    u** 6 v** 3     */
    p_g->c0703y = all_coeff[ 137];            /*    u** 7 v** 3     */
    p_g->c0803y = all_coeff[ 138];            /*    u** 8 v** 3     */
    p_g->c0903y = all_coeff[ 139];            /*    u** 9 v** 3     */

    p_g->c0004y = all_coeff[ 140];            /*    u** 0 v** 4     */
    p_g->c0104y = all_coeff[ 141];            /*    u** 1 v** 4     */
    p_g->c0204y = all_coeff[ 142];            /*    u** 2 v** 4     */
    p_g->c0304y = all_coeff[ 143];            /*    u** 3 v** 4     */
    p_g->c0404y = all_coeff[ 144];            /*    u** 4 v** 4     */
    p_g->c0504y = all_coeff[ 145];            /*    u** 5 v** 4     */
    p_g->c0604y = all_coeff[ 146];            /*    u** 6 v** 4     */
    p_g->c0704y = all_coeff[ 147];            /*    u** 7 v** 4     */
    p_g->c0804y = all_coeff[ 148];            /*    u** 8 v** 4     */
    p_g->c0904y = all_coeff[ 149];            /*    u** 9 v** 4     */

    p_g->c0005y = all_coeff[ 150];            /*    u** 0 v** 5     */
    p_g->c0105y = all_coeff[ 151];            /*    u** 1 v** 5     */
    p_g->c0205y = all_coeff[ 152];            /*    u** 2 v** 5     */
    p_g->c0305y = all_coeff[ 153];            /*    u** 3 v** 5     */
    p_g->c0405y = all_coeff[ 154];            /*    u** 4 v** 5     */
    p_g->c0505y = all_coeff[ 155];            /*    u** 5 v** 5     */
    p_g->c0605y = all_coeff[ 156];            /*    u** 6 v** 5     */
    p_g->c0705y = all_coeff[ 157];            /*    u** 7 v** 5     */
    p_g->c0805y = all_coeff[ 158];            /*    u** 8 v** 5     */
    p_g->c0905y = all_coeff[ 159];            /*    u** 9 v** 5     */

    p_g->c0006y = all_coeff[ 160];            /*    u** 0 v** 6     */
    p_g->c0106y = all_coeff[ 161];            /*    u** 1 v** 6     */
    p_g->c0206y = all_coeff[ 162];            /*    u** 2 v** 6     */
    p_g->c0306y = all_coeff[ 163];            /*    u** 3 v** 6     */
    p_g->c0406y = all_coeff[ 164];            /*    u** 4 v** 6     */
    p_g->c0506y = all_coeff[ 165];            /*    u** 5 v** 6     */
    p_g->c0606y = all_coeff[ 166];            /*    u** 6 v** 6     */
    p_g->c0706y = all_coeff[ 167];            /*    u** 7 v** 6     */
    p_g->c0806y = all_coeff[ 168];            /*    u** 8 v** 6     */
    p_g->c0906y = all_coeff[ 169];            /*    u** 9 v** 6     */

    p_g->c0007y = all_coeff[ 170];            /*    u** 0 v** 7     */
    p_g->c0107y = all_coeff[ 171];            /*    u** 1 v** 7     */
    p_g->c0207y = all_coeff[ 172];            /*    u** 2 v** 7     */
    p_g->c0307y = all_coeff[ 173];            /*    u** 3 v** 7     */
    p_g->c0407y = all_coeff[ 174];            /*    u** 4 v** 7     */
    p_g->c0507y = all_coeff[ 175];            /*    u** 5 v** 7     */
    p_g->c0607y = all_coeff[ 176];            /*    u** 6 v** 7     */
    p_g->c0707y = all_coeff[ 177];            /*    u** 7 v** 7     */
    p_g->c0807y = all_coeff[ 178];            /*    u** 8 v** 7     */
    p_g->c0907y = all_coeff[ 179];            /*    u** 9 v** 7     */

    p_g->c0008y = all_coeff[ 180];            /*    u** 0 v** 8     */
    p_g->c0108y = all_coeff[ 181];            /*    u** 1 v** 8     */
    p_g->c0208y = all_coeff[ 182];            /*    u** 2 v** 8     */
    p_g->c0308y = all_coeff[ 183];            /*    u** 3 v** 8     */
    p_g->c0408y = all_coeff[ 184];            /*    u** 4 v** 8     */
    p_g->c0508y = all_coeff[ 185];            /*    u** 5 v** 8     */
    p_g->c0608y = all_coeff[ 186];            /*    u** 6 v** 8     */
    p_g->c0708y = all_coeff[ 187];            /*    u** 7 v** 8     */
    p_g->c0808y = all_coeff[ 188];            /*    u** 8 v** 8     */
    p_g->c0908y = all_coeff[ 189];            /*    u** 9 v** 8     */

    p_g->c0009y = all_coeff[ 190];            /*    u** 0 v** 9     */
    p_g->c0109y = all_coeff[ 191];            /*    u** 1 v** 9     */
    p_g->c0209y = all_coeff[ 192];            /*    u** 2 v** 9     */
    p_g->c0309y = all_coeff[ 193];            /*    u** 3 v** 9     */
    p_g->c0409y = all_coeff[ 194];            /*    u** 4 v** 9     */
    p_g->c0509y = all_coeff[ 195];            /*    u** 5 v** 9     */
    p_g->c0609y = all_coeff[ 196];            /*    u** 6 v** 9     */
    p_g->c0709y = all_coeff[ 197];            /*    u** 7 v** 9     */
    p_g->c0809y = all_coeff[ 198];            /*    u** 8 v** 9     */
    p_g->c0909y = all_coeff[ 199];            /*    u** 9 v** 9     */


    p_g->c0000z = all_coeff[ 200];            /*    u** 0 v** 0     */
    p_g->c0100z = all_coeff[ 201];            /*    u** 1 v** 0     */
    p_g->c0200z = all_coeff[ 202];            /*    u** 2 v** 0     */
    p_g->c0300z = all_coeff[ 203];            /*    u** 3 v** 0     */
    p_g->c0400z = all_coeff[ 204];            /*    u** 4 v** 0     */
    p_g->c0500z = all_coeff[ 205];            /*    u** 5 v** 0     */
    p_g->c0600z = all_coeff[ 206];            /*    u** 6 v** 0     */
    p_g->c0700z = all_coeff[ 207];            /*    u** 7 v** 0     */
    p_g->c0800z = all_coeff[ 208];            /*    u** 8 v** 0     */
    p_g->c0900z = all_coeff[ 209];            /*    u** 9 v** 0     */

    p_g->c0001z = all_coeff[ 210];            /*    u** 0 v** 1     */
    p_g->c0101z = all_coeff[ 211];            /*    u** 1 v** 1     */
    p_g->c0201z = all_coeff[ 212];            /*    u** 2 v** 1     */
    p_g->c0301z = all_coeff[ 213];            /*    u** 3 v** 1     */
    p_g->c0401z = all_coeff[ 214];            /*    u** 4 v** 1     */
    p_g->c0501z = all_coeff[ 215];            /*    u** 5 v** 1     */
    p_g->c0601z = all_coeff[ 216];            /*    u** 6 v** 1     */
    p_g->c0701z = all_coeff[ 217];            /*    u** 7 v** 1     */
    p_g->c0801z = all_coeff[ 218];            /*    u** 8 v** 1     */
    p_g->c0901z = all_coeff[ 219];            /*    u** 9 v** 1     */

    p_g->c0002z = all_coeff[ 220];            /*    u** 0 v** 2     */
    p_g->c0102z = all_coeff[ 221];            /*    u** 1 v** 2     */
    p_g->c0202z = all_coeff[ 222];            /*    u** 2 v** 2     */
    p_g->c0302z = all_coeff[ 223];            /*    u** 3 v** 2     */
    p_g->c0402z = all_coeff[ 224];            /*    u** 4 v** 2     */
    p_g->c0502z = all_coeff[ 225];            /*    u** 5 v** 2     */
    p_g->c0602z = all_coeff[ 226];            /*    u** 6 v** 2     */
    p_g->c0702z = all_coeff[ 227];            /*    u** 7 v** 2     */
    p_g->c0802z = all_coeff[ 228];            /*    u** 8 v** 2     */
    p_g->c0902z = all_coeff[ 229];            /*    u** 9 v** 2     */

    p_g->c0003z = all_coeff[ 230];            /*    u** 0 v** 3     */
    p_g->c0103z = all_coeff[ 231];            /*    u** 1 v** 3     */
    p_g->c0203z = all_coeff[ 232];            /*    u** 2 v** 3     */
    p_g->c0303z = all_coeff[ 233];            /*    u** 3 v** 3     */
    p_g->c0403z = all_coeff[ 234];            /*    u** 4 v** 3     */
    p_g->c0503z = all_coeff[ 235];            /*    u** 5 v** 3     */
    p_g->c0603z = all_coeff[ 236];            /*    u** 6 v** 3     */
    p_g->c0703z = all_coeff[ 237];            /*    u** 7 v** 3     */
    p_g->c0803z = all_coeff[ 238];            /*    u** 8 v** 3     */
    p_g->c0903z = all_coeff[ 239];            /*    u** 9 v** 3     */

    p_g->c0004z = all_coeff[ 240];            /*    u** 0 v** 4     */
    p_g->c0104z = all_coeff[ 241];            /*    u** 1 v** 4     */
    p_g->c0204z = all_coeff[ 242];            /*    u** 2 v** 4     */
    p_g->c0304z = all_coeff[ 243];            /*    u** 3 v** 4     */
    p_g->c0404z = all_coeff[ 244];            /*    u** 4 v** 4     */
    p_g->c0504z = all_coeff[ 245];            /*    u** 5 v** 4     */
    p_g->c0604z = all_coeff[ 246];            /*    u** 6 v** 4     */
    p_g->c0704z = all_coeff[ 247];            /*    u** 7 v** 4     */
    p_g->c0804z = all_coeff[ 248];            /*    u** 8 v** 4     */
    p_g->c0904z = all_coeff[ 249];            /*    u** 9 v** 4     */

    p_g->c0005z = all_coeff[ 250];            /*    u** 0 v** 5     */
    p_g->c0105z = all_coeff[ 251];            /*    u** 1 v** 5     */
    p_g->c0205z = all_coeff[ 252];            /*    u** 2 v** 5     */
    p_g->c0305z = all_coeff[ 253];            /*    u** 3 v** 5     */
    p_g->c0405z = all_coeff[ 254];            /*    u** 4 v** 5     */
    p_g->c0505z = all_coeff[ 255];            /*    u** 5 v** 5     */
    p_g->c0605z = all_coeff[ 256];            /*    u** 6 v** 5     */
    p_g->c0705z = all_coeff[ 257];            /*    u** 7 v** 5     */
    p_g->c0805z = all_coeff[ 258];            /*    u** 8 v** 5     */
    p_g->c0905z = all_coeff[ 259];            /*    u** 9 v** 5     */

    p_g->c0006z = all_coeff[ 260];            /*    u** 0 v** 6     */
    p_g->c0106z = all_coeff[ 261];            /*    u** 1 v** 6     */
    p_g->c0206z = all_coeff[ 262];            /*    u** 2 v** 6     */
    p_g->c0306z = all_coeff[ 263];            /*    u** 3 v** 6     */
    p_g->c0406z = all_coeff[ 264];            /*    u** 4 v** 6     */
    p_g->c0506z = all_coeff[ 265];            /*    u** 5 v** 6     */
    p_g->c0606z = all_coeff[ 266];            /*    u** 6 v** 6     */
    p_g->c0706z = all_coeff[ 267];            /*    u** 7 v** 6     */
    p_g->c0806z = all_coeff[ 268];            /*    u** 8 v** 6     */
    p_g->c0906z = all_coeff[ 269];            /*    u** 9 v** 6     */

    p_g->c0007z = all_coeff[ 270];            /*    u** 0 v** 7     */
    p_g->c0107z = all_coeff[ 271];            /*    u** 1 v** 7     */
    p_g->c0207z = all_coeff[ 272];            /*    u** 2 v** 7     */
    p_g->c0307z = all_coeff[ 273];            /*    u** 3 v** 7     */
    p_g->c0407z = all_coeff[ 274];            /*    u** 4 v** 7     */
    p_g->c0507z = all_coeff[ 275];            /*    u** 5 v** 7     */
    p_g->c0607z = all_coeff[ 276];            /*    u** 6 v** 7     */
    p_g->c0707z = all_coeff[ 277];            /*    u** 7 v** 7     */
    p_g->c0807z = all_coeff[ 278];            /*    u** 8 v** 7     */
    p_g->c0907z = all_coeff[ 279];            /*    u** 9 v** 7     */

    p_g->c0008z = all_coeff[ 280];            /*    u** 0 v** 8     */
    p_g->c0108z = all_coeff[ 281];            /*    u** 1 v** 8     */
    p_g->c0208z = all_coeff[ 282];            /*    u** 2 v** 8     */
    p_g->c0308z = all_coeff[ 283];            /*    u** 3 v** 8     */
    p_g->c0408z = all_coeff[ 284];            /*    u** 4 v** 8     */
    p_g->c0508z = all_coeff[ 285];            /*    u** 5 v** 8     */
    p_g->c0608z = all_coeff[ 286];            /*    u** 6 v** 8     */
    p_g->c0708z = all_coeff[ 287];            /*    u** 7 v** 8     */
    p_g->c0808z = all_coeff[ 288];            /*    u** 8 v** 8     */
    p_g->c0908z = all_coeff[ 289];            /*    u** 9 v** 8     */

    p_g->c0009z = all_coeff[ 290];            /*    u** 0 v** 9     */
    p_g->c0109z = all_coeff[ 291];            /*    u** 1 v** 9     */
    p_g->c0209z = all_coeff[ 292];            /*    u** 2 v** 9     */
    p_g->c0309z = all_coeff[ 293];            /*    u** 3 v** 9     */
    p_g->c0409z = all_coeff[ 294];            /*    u** 4 v** 9     */
    p_g->c0509z = all_coeff[ 295];            /*    u** 5 v** 9     */
    p_g->c0609z = all_coeff[ 296];            /*    u** 6 v** 9     */
    p_g->c0709z = all_coeff[ 297];            /*    u** 7 v** 9     */
    p_g->c0809z = all_coeff[ 298];            /*    u** 8 v** 9     */
    p_g->c0909z = all_coeff[ 299];            /*    u** 9 v** 9     */






    p_g->ofs_pat = 0.0;                  /* Offset for patch        */




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur558 Exit*varkon_vda_rp9   * i_l %d n_coeff %d n_retrieved %d\n", 
                  (short)i_l, (short)n_coeff, (short)n_retrieved );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
