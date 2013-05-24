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
/*  Function: varkon_vda_rp21                        File: sur559.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve VDA-FS coefficients for P21_PAT patch.                 */
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
/*sdescr varkon_vda_rp21       Read P21_PAT VDA-FS coefficients */
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
/* SU2993 = Severe program error (  ) in varkon_vda_rp21   (sur559) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rp21 (

/*_------------- Argument declarations -----------------------------*/
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
   DBfloat ret_coeff[1452];/* Retrieved coefficients                */
   DBfloat all_coeff[1452];/* All coefficients                      */
   DBint   n_degree;     /* Degree for patch P21_PAT                */
   DBint   i_c;          /* Loop index X, Y and Z component         */
   DBint   i_u;          /* Loop index horizontal row in C matrix   */
   DBint   i_v;          /* Loop index vertical   row in C matrix   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   GMPATP21 *p_g;        /* Current geometric   patch         (ptr) */
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
fprintf(dbgfil(SURPAC),"sur559*Enter*  \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Number of coefficients values:                                   */
   n_coeff = 3 * (n_degu + 1) * (n_degv +1);
/*                                                                 !*/

  n_degree = 21 ;  

  for ( i_f = 0; i_f < 1452; ++i_f )
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
    sprintf(errbuf,"read not OK%%sur559");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*     ii. Get all floats from the current line                     */
/*                                                                 !*/

   status=varkon_vda_rsur3 ( rad, &n_flt, all_flt );
    if (status<0) 
        {
        sprintf(errbuf,"sur553%%sur559");
        return(varkon_erpush("SU2943",errbuf));
        }
/*!                                                                 */
/*    iii. Loop all floats from the line                            */
/*                                                                 !*/

  if ( n_flt <= 0 )
    {
    sprintf(errbuf,"n_flt=0%%sur559");
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


  sprintf(errbuf,"n_coeff>n_retrieved%%sur559");
  return(varkon_erpush("SU2993",errbuf));


allpar:; /*! Label: All coefficient values retrieved               !*/

/*!                                                                 */
/* Check that there is no extra data is on the last line            */
/*                                                                 !*/

  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved%%sur559");
    return(varkon_erpush("SU2993",errbuf));
    }
    

/*!                                                                 */
/* Dynamic allocation of area for one geometric (P21_PAT) patch.    */
/* Call of function DBcreate_patches.                                         */
/*                                                                 !*/

if ((p_gpat=DBcreate_patches(P21_PAT,1)) == NULL)
 { 
 sprintf(errbuf, "(allocg)%%sur559");
 return(varkon_erpush("SU2993",errbuf));
 }

  p_g    = (GMPATP21 *)p_gpat;           /* Current geometric patch */

  p_t->styp_pat = P21_PAT;               /* Type of secondary patch */
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
for ( i_l = 0; i_l <   1452; ++i_l )
{
fprintf(dbgfil(SURPAC),
"sur559 ret_coeff[%d]= %f all_coeff[%d]= %f \n", 
 (short)i_l, ret_coeff[i_l], (short)i_l, all_coeff[i_l] );
}
fflush(dbgfil(SURPAC));
}
  if  ( n_coeff != n_retrieved )
    {
    sprintf(errbuf,"n_coeff!=n_retrieved (2)%%sur559");
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
    p_g->c1000x = all_coeff[  10];            /*    u**10 v** 0     */
    p_g->c1100x = all_coeff[  11];            /*    u**11 v** 0     */
    p_g->c1200x = all_coeff[  12];            /*    u**12 v** 0     */
    p_g->c1300x = all_coeff[  13];            /*    u**13 v** 0     */
    p_g->c1400x = all_coeff[  14];            /*    u**14 v** 0     */
    p_g->c1500x = all_coeff[  15];            /*    u**15 v** 0     */
    p_g->c1600x = all_coeff[  16];            /*    u**16 v** 0     */
    p_g->c1700x = all_coeff[  17];            /*    u**17 v** 0     */
    p_g->c1800x = all_coeff[  18];            /*    u**18 v** 0     */
    p_g->c1900x = all_coeff[  19];            /*    u**19 v** 0     */
    p_g->c2000x = all_coeff[  20];            /*    u**20 v** 0     */
    p_g->c2100x = all_coeff[  21];            /*    u**21 v** 0     */

    p_g->c0001x = all_coeff[  22];            /*    u** 0 v** 1     */
    p_g->c0101x = all_coeff[  23];            /*    u** 1 v** 1     */
    p_g->c0201x = all_coeff[  24];            /*    u** 2 v** 1     */
    p_g->c0301x = all_coeff[  25];            /*    u** 3 v** 1     */
    p_g->c0401x = all_coeff[  26];            /*    u** 4 v** 1     */
    p_g->c0501x = all_coeff[  27];            /*    u** 5 v** 1     */
    p_g->c0601x = all_coeff[  28];            /*    u** 6 v** 1     */
    p_g->c0701x = all_coeff[  29];            /*    u** 7 v** 1     */
    p_g->c0801x = all_coeff[  30];            /*    u** 8 v** 1     */
    p_g->c0901x = all_coeff[  31];            /*    u** 9 v** 1     */
    p_g->c1001x = all_coeff[  32];            /*    u**10 v** 1     */
    p_g->c1101x = all_coeff[  33];            /*    u**11 v** 1     */
    p_g->c1201x = all_coeff[  34];            /*    u**12 v** 1     */
    p_g->c1301x = all_coeff[  35];            /*    u**13 v** 1     */
    p_g->c1401x = all_coeff[  36];            /*    u**14 v** 1     */
    p_g->c1501x = all_coeff[  37];            /*    u**15 v** 1     */
    p_g->c1601x = all_coeff[  38];            /*    u**16 v** 1     */
    p_g->c1701x = all_coeff[  39];            /*    u**17 v** 1     */
    p_g->c1801x = all_coeff[  40];            /*    u**18 v** 1     */
    p_g->c1901x = all_coeff[  41];            /*    u**19 v** 1     */
    p_g->c2001x = all_coeff[  42];            /*    u**20 v** 1     */
    p_g->c2101x = all_coeff[  43];            /*    u**21 v** 1     */

    p_g->c0002x = all_coeff[  44];            /*    u** 0 v** 2     */
    p_g->c0102x = all_coeff[  45];            /*    u** 1 v** 2     */
    p_g->c0202x = all_coeff[  46];            /*    u** 2 v** 2     */
    p_g->c0302x = all_coeff[  47];            /*    u** 3 v** 2     */
    p_g->c0402x = all_coeff[  48];            /*    u** 4 v** 2     */
    p_g->c0502x = all_coeff[  49];            /*    u** 5 v** 2     */
    p_g->c0602x = all_coeff[  50];            /*    u** 6 v** 2     */
    p_g->c0702x = all_coeff[  51];            /*    u** 7 v** 2     */
    p_g->c0802x = all_coeff[  52];            /*    u** 8 v** 2     */
    p_g->c0902x = all_coeff[  53];            /*    u** 9 v** 2     */
    p_g->c1002x = all_coeff[  54];            /*    u**10 v** 2     */
    p_g->c1102x = all_coeff[  55];            /*    u**11 v** 2     */
    p_g->c1202x = all_coeff[  56];            /*    u**12 v** 2     */
    p_g->c1302x = all_coeff[  57];            /*    u**13 v** 2     */
    p_g->c1402x = all_coeff[  58];            /*    u**14 v** 2     */
    p_g->c1502x = all_coeff[  59];            /*    u**15 v** 2     */
    p_g->c1602x = all_coeff[  60];            /*    u**16 v** 2     */
    p_g->c1702x = all_coeff[  61];            /*    u**17 v** 2     */
    p_g->c1802x = all_coeff[  62];            /*    u**18 v** 2     */
    p_g->c1902x = all_coeff[  63];            /*    u**19 v** 2     */
    p_g->c2002x = all_coeff[  64];            /*    u**20 v** 2     */
    p_g->c2102x = all_coeff[  65];            /*    u**21 v** 2     */

    p_g->c0003x = all_coeff[  66];            /*    u** 0 v** 3     */
    p_g->c0103x = all_coeff[  67];            /*    u** 1 v** 3     */
    p_g->c0203x = all_coeff[  68];            /*    u** 2 v** 3     */
    p_g->c0303x = all_coeff[  69];            /*    u** 3 v** 3     */
    p_g->c0403x = all_coeff[  70];            /*    u** 4 v** 3     */
    p_g->c0503x = all_coeff[  71];            /*    u** 5 v** 3     */
    p_g->c0603x = all_coeff[  72];            /*    u** 6 v** 3     */
    p_g->c0703x = all_coeff[  73];            /*    u** 7 v** 3     */
    p_g->c0803x = all_coeff[  74];            /*    u** 8 v** 3     */
    p_g->c0903x = all_coeff[  75];            /*    u** 9 v** 3     */
    p_g->c1003x = all_coeff[  76];            /*    u**10 v** 3     */
    p_g->c1103x = all_coeff[  77];            /*    u**11 v** 3     */
    p_g->c1203x = all_coeff[  78];            /*    u**12 v** 3     */
    p_g->c1303x = all_coeff[  79];            /*    u**13 v** 3     */
    p_g->c1403x = all_coeff[  80];            /*    u**14 v** 3     */
    p_g->c1503x = all_coeff[  81];            /*    u**15 v** 3     */
    p_g->c1603x = all_coeff[  82];            /*    u**16 v** 3     */
    p_g->c1703x = all_coeff[  83];            /*    u**17 v** 3     */
    p_g->c1803x = all_coeff[  84];            /*    u**18 v** 3     */
    p_g->c1903x = all_coeff[  85];            /*    u**19 v** 3     */
    p_g->c2003x = all_coeff[  86];            /*    u**20 v** 3     */
    p_g->c2103x = all_coeff[  87];            /*    u**21 v** 3     */

    p_g->c0004x = all_coeff[  88];            /*    u** 0 v** 4     */
    p_g->c0104x = all_coeff[  89];            /*    u** 1 v** 4     */
    p_g->c0204x = all_coeff[  90];            /*    u** 2 v** 4     */
    p_g->c0304x = all_coeff[  91];            /*    u** 3 v** 4     */
    p_g->c0404x = all_coeff[  92];            /*    u** 4 v** 4     */
    p_g->c0504x = all_coeff[  93];            /*    u** 5 v** 4     */
    p_g->c0604x = all_coeff[  94];            /*    u** 6 v** 4     */
    p_g->c0704x = all_coeff[  95];            /*    u** 7 v** 4     */
    p_g->c0804x = all_coeff[  96];            /*    u** 8 v** 4     */
    p_g->c0904x = all_coeff[  97];            /*    u** 9 v** 4     */
    p_g->c1004x = all_coeff[  98];            /*    u**10 v** 4     */
    p_g->c1104x = all_coeff[  99];            /*    u**11 v** 4     */
    p_g->c1204x = all_coeff[ 100];            /*    u**12 v** 4     */
    p_g->c1304x = all_coeff[ 101];            /*    u**13 v** 4     */
    p_g->c1404x = all_coeff[ 102];            /*    u**14 v** 4     */
    p_g->c1504x = all_coeff[ 103];            /*    u**15 v** 4     */
    p_g->c1604x = all_coeff[ 104];            /*    u**16 v** 4     */
    p_g->c1704x = all_coeff[ 105];            /*    u**17 v** 4     */
    p_g->c1804x = all_coeff[ 106];            /*    u**18 v** 4     */
    p_g->c1904x = all_coeff[ 107];            /*    u**19 v** 4     */
    p_g->c2004x = all_coeff[ 108];            /*    u**20 v** 4     */
    p_g->c2104x = all_coeff[ 109];            /*    u**21 v** 4     */

    p_g->c0005x = all_coeff[ 110];            /*    u** 0 v** 5     */
    p_g->c0105x = all_coeff[ 111];            /*    u** 1 v** 5     */
    p_g->c0205x = all_coeff[ 112];            /*    u** 2 v** 5     */
    p_g->c0305x = all_coeff[ 113];            /*    u** 3 v** 5     */
    p_g->c0405x = all_coeff[ 114];            /*    u** 4 v** 5     */
    p_g->c0505x = all_coeff[ 115];            /*    u** 5 v** 5     */
    p_g->c0605x = all_coeff[ 116];            /*    u** 6 v** 5     */
    p_g->c0705x = all_coeff[ 117];            /*    u** 7 v** 5     */
    p_g->c0805x = all_coeff[ 118];            /*    u** 8 v** 5     */
    p_g->c0905x = all_coeff[ 119];            /*    u** 9 v** 5     */
    p_g->c1005x = all_coeff[ 120];            /*    u**10 v** 5     */
    p_g->c1105x = all_coeff[ 121];            /*    u**11 v** 5     */
    p_g->c1205x = all_coeff[ 122];            /*    u**12 v** 5     */
    p_g->c1305x = all_coeff[ 123];            /*    u**13 v** 5     */
    p_g->c1405x = all_coeff[ 124];            /*    u**14 v** 5     */
    p_g->c1505x = all_coeff[ 125];            /*    u**15 v** 5     */
    p_g->c1605x = all_coeff[ 126];            /*    u**16 v** 5     */
    p_g->c1705x = all_coeff[ 127];            /*    u**17 v** 5     */
    p_g->c1805x = all_coeff[ 128];            /*    u**18 v** 5     */
    p_g->c1905x = all_coeff[ 129];            /*    u**19 v** 5     */
    p_g->c2005x = all_coeff[ 130];            /*    u**20 v** 5     */
    p_g->c2105x = all_coeff[ 131];            /*    u**21 v** 5     */

    p_g->c0006x = all_coeff[ 132];            /*    u** 0 v** 6     */
    p_g->c0106x = all_coeff[ 133];            /*    u** 1 v** 6     */
    p_g->c0206x = all_coeff[ 134];            /*    u** 2 v** 6     */
    p_g->c0306x = all_coeff[ 135];            /*    u** 3 v** 6     */
    p_g->c0406x = all_coeff[ 136];            /*    u** 4 v** 6     */
    p_g->c0506x = all_coeff[ 137];            /*    u** 5 v** 6     */
    p_g->c0606x = all_coeff[ 138];            /*    u** 6 v** 6     */
    p_g->c0706x = all_coeff[ 139];            /*    u** 7 v** 6     */
    p_g->c0806x = all_coeff[ 140];            /*    u** 8 v** 6     */
    p_g->c0906x = all_coeff[ 141];            /*    u** 9 v** 6     */
    p_g->c1006x = all_coeff[ 142];            /*    u**10 v** 6     */
    p_g->c1106x = all_coeff[ 143];            /*    u**11 v** 6     */
    p_g->c1206x = all_coeff[ 144];            /*    u**12 v** 6     */
    p_g->c1306x = all_coeff[ 145];            /*    u**13 v** 6     */
    p_g->c1406x = all_coeff[ 146];            /*    u**14 v** 6     */
    p_g->c1506x = all_coeff[ 147];            /*    u**15 v** 6     */
    p_g->c1606x = all_coeff[ 148];            /*    u**16 v** 6     */
    p_g->c1706x = all_coeff[ 149];            /*    u**17 v** 6     */
    p_g->c1806x = all_coeff[ 150];            /*    u**18 v** 6     */
    p_g->c1906x = all_coeff[ 151];            /*    u**19 v** 6     */
    p_g->c2006x = all_coeff[ 152];            /*    u**20 v** 6     */
    p_g->c2106x = all_coeff[ 153];            /*    u**21 v** 6     */

    p_g->c0007x = all_coeff[ 154];            /*    u** 0 v** 7     */
    p_g->c0107x = all_coeff[ 155];            /*    u** 1 v** 7     */
    p_g->c0207x = all_coeff[ 156];            /*    u** 2 v** 7     */
    p_g->c0307x = all_coeff[ 157];            /*    u** 3 v** 7     */
    p_g->c0407x = all_coeff[ 158];            /*    u** 4 v** 7     */
    p_g->c0507x = all_coeff[ 159];            /*    u** 5 v** 7     */
    p_g->c0607x = all_coeff[ 160];            /*    u** 6 v** 7     */
    p_g->c0707x = all_coeff[ 161];            /*    u** 7 v** 7     */
    p_g->c0807x = all_coeff[ 162];            /*    u** 8 v** 7     */
    p_g->c0907x = all_coeff[ 163];            /*    u** 9 v** 7     */
    p_g->c1007x = all_coeff[ 164];            /*    u**10 v** 7     */
    p_g->c1107x = all_coeff[ 165];            /*    u**11 v** 7     */
    p_g->c1207x = all_coeff[ 166];            /*    u**12 v** 7     */
    p_g->c1307x = all_coeff[ 167];            /*    u**13 v** 7     */
    p_g->c1407x = all_coeff[ 168];            /*    u**14 v** 7     */
    p_g->c1507x = all_coeff[ 169];            /*    u**15 v** 7     */
    p_g->c1607x = all_coeff[ 170];            /*    u**16 v** 7     */
    p_g->c1707x = all_coeff[ 171];            /*    u**17 v** 7     */
    p_g->c1807x = all_coeff[ 172];            /*    u**18 v** 7     */
    p_g->c1907x = all_coeff[ 173];            /*    u**19 v** 7     */
    p_g->c2007x = all_coeff[ 174];            /*    u**20 v** 7     */
    p_g->c2107x = all_coeff[ 175];            /*    u**21 v** 7     */

    p_g->c0008x = all_coeff[ 176];            /*    u** 0 v** 8     */
    p_g->c0108x = all_coeff[ 177];            /*    u** 1 v** 8     */
    p_g->c0208x = all_coeff[ 178];            /*    u** 2 v** 8     */
    p_g->c0308x = all_coeff[ 179];            /*    u** 3 v** 8     */
    p_g->c0408x = all_coeff[ 180];            /*    u** 4 v** 8     */
    p_g->c0508x = all_coeff[ 181];            /*    u** 5 v** 8     */
    p_g->c0608x = all_coeff[ 182];            /*    u** 6 v** 8     */
    p_g->c0708x = all_coeff[ 183];            /*    u** 7 v** 8     */
    p_g->c0808x = all_coeff[ 184];            /*    u** 8 v** 8     */
    p_g->c0908x = all_coeff[ 185];            /*    u** 9 v** 8     */
    p_g->c1008x = all_coeff[ 186];            /*    u**10 v** 8     */
    p_g->c1108x = all_coeff[ 187];            /*    u**11 v** 8     */
    p_g->c1208x = all_coeff[ 188];            /*    u**12 v** 8     */
    p_g->c1308x = all_coeff[ 189];            /*    u**13 v** 8     */
    p_g->c1408x = all_coeff[ 190];            /*    u**14 v** 8     */
    p_g->c1508x = all_coeff[ 191];            /*    u**15 v** 8     */
    p_g->c1608x = all_coeff[ 192];            /*    u**16 v** 8     */
    p_g->c1708x = all_coeff[ 193];            /*    u**17 v** 8     */
    p_g->c1808x = all_coeff[ 194];            /*    u**18 v** 8     */
    p_g->c1908x = all_coeff[ 195];            /*    u**19 v** 8     */
    p_g->c2008x = all_coeff[ 196];            /*    u**20 v** 8     */
    p_g->c2108x = all_coeff[ 197];            /*    u**21 v** 8     */

    p_g->c0009x = all_coeff[ 198];            /*    u** 0 v** 9     */
    p_g->c0109x = all_coeff[ 199];            /*    u** 1 v** 9     */
    p_g->c0209x = all_coeff[ 200];            /*    u** 2 v** 9     */
    p_g->c0309x = all_coeff[ 201];            /*    u** 3 v** 9     */
    p_g->c0409x = all_coeff[ 202];            /*    u** 4 v** 9     */
    p_g->c0509x = all_coeff[ 203];            /*    u** 5 v** 9     */
    p_g->c0609x = all_coeff[ 204];            /*    u** 6 v** 9     */
    p_g->c0709x = all_coeff[ 205];            /*    u** 7 v** 9     */
    p_g->c0809x = all_coeff[ 206];            /*    u** 8 v** 9     */
    p_g->c0909x = all_coeff[ 207];            /*    u** 9 v** 9     */
    p_g->c1009x = all_coeff[ 208];            /*    u**10 v** 9     */
    p_g->c1109x = all_coeff[ 209];            /*    u**11 v** 9     */
    p_g->c1209x = all_coeff[ 210];            /*    u**12 v** 9     */
    p_g->c1309x = all_coeff[ 211];            /*    u**13 v** 9     */
    p_g->c1409x = all_coeff[ 212];            /*    u**14 v** 9     */
    p_g->c1509x = all_coeff[ 213];            /*    u**15 v** 9     */
    p_g->c1609x = all_coeff[ 214];            /*    u**16 v** 9     */
    p_g->c1709x = all_coeff[ 215];            /*    u**17 v** 9     */
    p_g->c1809x = all_coeff[ 216];            /*    u**18 v** 9     */
    p_g->c1909x = all_coeff[ 217];            /*    u**19 v** 9     */
    p_g->c2009x = all_coeff[ 218];            /*    u**20 v** 9     */
    p_g->c2109x = all_coeff[ 219];            /*    u**21 v** 9     */

    p_g->c0010x = all_coeff[ 220];            /*    u** 0 v**10     */
    p_g->c0110x = all_coeff[ 221];            /*    u** 1 v**10     */
    p_g->c0210x = all_coeff[ 222];            /*    u** 2 v**10     */
    p_g->c0310x = all_coeff[ 223];            /*    u** 3 v**10     */
    p_g->c0410x = all_coeff[ 224];            /*    u** 4 v**10     */
    p_g->c0510x = all_coeff[ 225];            /*    u** 5 v**10     */
    p_g->c0610x = all_coeff[ 226];            /*    u** 6 v**10     */
    p_g->c0710x = all_coeff[ 227];            /*    u** 7 v**10     */
    p_g->c0810x = all_coeff[ 228];            /*    u** 8 v**10     */
    p_g->c0910x = all_coeff[ 229];            /*    u** 9 v**10     */
    p_g->c1010x = all_coeff[ 230];            /*    u**10 v**10     */
    p_g->c1110x = all_coeff[ 231];            /*    u**11 v**10     */
    p_g->c1210x = all_coeff[ 232];            /*    u**12 v**10     */
    p_g->c1310x = all_coeff[ 233];            /*    u**13 v**10     */
    p_g->c1410x = all_coeff[ 234];            /*    u**14 v**10     */
    p_g->c1510x = all_coeff[ 235];            /*    u**15 v**10     */
    p_g->c1610x = all_coeff[ 236];            /*    u**16 v**10     */
    p_g->c1710x = all_coeff[ 237];            /*    u**17 v**10     */
    p_g->c1810x = all_coeff[ 238];            /*    u**18 v**10     */
    p_g->c1910x = all_coeff[ 239];            /*    u**19 v**10     */
    p_g->c2010x = all_coeff[ 240];            /*    u**20 v**10     */
    p_g->c2110x = all_coeff[ 241];            /*    u**21 v**10     */

    p_g->c0011x = all_coeff[ 242];            /*    u** 0 v**11     */
    p_g->c0111x = all_coeff[ 243];            /*    u** 1 v**11     */
    p_g->c0211x = all_coeff[ 244];            /*    u** 2 v**11     */
    p_g->c0311x = all_coeff[ 245];            /*    u** 3 v**11     */
    p_g->c0411x = all_coeff[ 246];            /*    u** 4 v**11     */
    p_g->c0511x = all_coeff[ 247];            /*    u** 5 v**11     */
    p_g->c0611x = all_coeff[ 248];            /*    u** 6 v**11     */
    p_g->c0711x = all_coeff[ 249];            /*    u** 7 v**11     */
    p_g->c0811x = all_coeff[ 250];            /*    u** 8 v**11     */
    p_g->c0911x = all_coeff[ 251];            /*    u** 9 v**11     */
    p_g->c1011x = all_coeff[ 252];            /*    u**10 v**11     */
    p_g->c1111x = all_coeff[ 253];            /*    u**11 v**11     */
    p_g->c1211x = all_coeff[ 254];            /*    u**12 v**11     */
    p_g->c1311x = all_coeff[ 255];            /*    u**13 v**11     */
    p_g->c1411x = all_coeff[ 256];            /*    u**14 v**11     */
    p_g->c1511x = all_coeff[ 257];            /*    u**15 v**11     */
    p_g->c1611x = all_coeff[ 258];            /*    u**16 v**11     */
    p_g->c1711x = all_coeff[ 259];            /*    u**17 v**11     */
    p_g->c1811x = all_coeff[ 260];            /*    u**18 v**11     */
    p_g->c1911x = all_coeff[ 261];            /*    u**19 v**11     */
    p_g->c2011x = all_coeff[ 262];            /*    u**20 v**11     */
    p_g->c2111x = all_coeff[ 263];            /*    u**21 v**11     */

    p_g->c0012x = all_coeff[ 264];            /*    u** 0 v**12     */
    p_g->c0112x = all_coeff[ 265];            /*    u** 1 v**12     */
    p_g->c0212x = all_coeff[ 266];            /*    u** 2 v**12     */
    p_g->c0312x = all_coeff[ 267];            /*    u** 3 v**12     */
    p_g->c0412x = all_coeff[ 268];            /*    u** 4 v**12     */
    p_g->c0512x = all_coeff[ 269];            /*    u** 5 v**12     */
    p_g->c0612x = all_coeff[ 270];            /*    u** 6 v**12     */
    p_g->c0712x = all_coeff[ 271];            /*    u** 7 v**12     */
    p_g->c0812x = all_coeff[ 272];            /*    u** 8 v**12     */
    p_g->c0912x = all_coeff[ 273];            /*    u** 9 v**12     */
    p_g->c1012x = all_coeff[ 274];            /*    u**10 v**12     */
    p_g->c1112x = all_coeff[ 275];            /*    u**11 v**12     */
    p_g->c1212x = all_coeff[ 276];            /*    u**12 v**12     */
    p_g->c1312x = all_coeff[ 277];            /*    u**13 v**12     */
    p_g->c1412x = all_coeff[ 278];            /*    u**14 v**12     */
    p_g->c1512x = all_coeff[ 279];            /*    u**15 v**12     */
    p_g->c1612x = all_coeff[ 280];            /*    u**16 v**12     */
    p_g->c1712x = all_coeff[ 281];            /*    u**17 v**12     */
    p_g->c1812x = all_coeff[ 282];            /*    u**18 v**12     */
    p_g->c1912x = all_coeff[ 283];            /*    u**19 v**12     */
    p_g->c2012x = all_coeff[ 284];            /*    u**20 v**12     */
    p_g->c2112x = all_coeff[ 285];            /*    u**21 v**12     */

    p_g->c0013x = all_coeff[ 286];            /*    u** 0 v**13     */
    p_g->c0113x = all_coeff[ 287];            /*    u** 1 v**13     */
    p_g->c0213x = all_coeff[ 288];            /*    u** 2 v**13     */
    p_g->c0313x = all_coeff[ 289];            /*    u** 3 v**13     */
    p_g->c0413x = all_coeff[ 290];            /*    u** 4 v**13     */
    p_g->c0513x = all_coeff[ 291];            /*    u** 5 v**13     */
    p_g->c0613x = all_coeff[ 292];            /*    u** 6 v**13     */
    p_g->c0713x = all_coeff[ 293];            /*    u** 7 v**13     */
    p_g->c0813x = all_coeff[ 294];            /*    u** 8 v**13     */
    p_g->c0913x = all_coeff[ 295];            /*    u** 9 v**13     */
    p_g->c1013x = all_coeff[ 296];            /*    u**10 v**13     */
    p_g->c1113x = all_coeff[ 297];            /*    u**11 v**13     */
    p_g->c1213x = all_coeff[ 298];            /*    u**12 v**13     */
    p_g->c1313x = all_coeff[ 299];            /*    u**13 v**13     */
    p_g->c1413x = all_coeff[ 300];            /*    u**14 v**13     */
    p_g->c1513x = all_coeff[ 301];            /*    u**15 v**13     */
    p_g->c1613x = all_coeff[ 302];            /*    u**16 v**13     */
    p_g->c1713x = all_coeff[ 303];            /*    u**17 v**13     */
    p_g->c1813x = all_coeff[ 304];            /*    u**18 v**13     */
    p_g->c1913x = all_coeff[ 305];            /*    u**19 v**13     */
    p_g->c2013x = all_coeff[ 306];            /*    u**20 v**13     */
    p_g->c2113x = all_coeff[ 307];            /*    u**21 v**13     */

    p_g->c0014x = all_coeff[ 308];            /*    u** 0 v**14     */
    p_g->c0114x = all_coeff[ 309];            /*    u** 1 v**14     */
    p_g->c0214x = all_coeff[ 310];            /*    u** 2 v**14     */
    p_g->c0314x = all_coeff[ 311];            /*    u** 3 v**14     */
    p_g->c0414x = all_coeff[ 312];            /*    u** 4 v**14     */
    p_g->c0514x = all_coeff[ 313];            /*    u** 5 v**14     */
    p_g->c0614x = all_coeff[ 314];            /*    u** 6 v**14     */
    p_g->c0714x = all_coeff[ 315];            /*    u** 7 v**14     */
    p_g->c0814x = all_coeff[ 316];            /*    u** 8 v**14     */
    p_g->c0914x = all_coeff[ 317];            /*    u** 9 v**14     */
    p_g->c1014x = all_coeff[ 318];            /*    u**10 v**14     */
    p_g->c1114x = all_coeff[ 319];            /*    u**11 v**14     */
    p_g->c1214x = all_coeff[ 320];            /*    u**12 v**14     */
    p_g->c1314x = all_coeff[ 321];            /*    u**13 v**14     */
    p_g->c1414x = all_coeff[ 322];            /*    u**14 v**14     */
    p_g->c1514x = all_coeff[ 323];            /*    u**15 v**14     */
    p_g->c1614x = all_coeff[ 324];            /*    u**16 v**14     */
    p_g->c1714x = all_coeff[ 325];            /*    u**17 v**14     */
    p_g->c1814x = all_coeff[ 326];            /*    u**18 v**14     */
    p_g->c1914x = all_coeff[ 327];            /*    u**19 v**14     */
    p_g->c2014x = all_coeff[ 328];            /*    u**20 v**14     */
    p_g->c2114x = all_coeff[ 329];            /*    u**21 v**14     */

    p_g->c0015x = all_coeff[ 330];            /*    u** 0 v**15     */
    p_g->c0115x = all_coeff[ 331];            /*    u** 1 v**15     */
    p_g->c0215x = all_coeff[ 332];            /*    u** 2 v**15     */
    p_g->c0315x = all_coeff[ 333];            /*    u** 3 v**15     */
    p_g->c0415x = all_coeff[ 334];            /*    u** 4 v**15     */
    p_g->c0515x = all_coeff[ 335];            /*    u** 5 v**15     */
    p_g->c0615x = all_coeff[ 336];            /*    u** 6 v**15     */
    p_g->c0715x = all_coeff[ 337];            /*    u** 7 v**15     */
    p_g->c0815x = all_coeff[ 338];            /*    u** 8 v**15     */
    p_g->c0915x = all_coeff[ 339];            /*    u** 9 v**15     */
    p_g->c1015x = all_coeff[ 340];            /*    u**10 v**15     */
    p_g->c1115x = all_coeff[ 341];            /*    u**11 v**15     */
    p_g->c1215x = all_coeff[ 342];            /*    u**12 v**15     */
    p_g->c1315x = all_coeff[ 343];            /*    u**13 v**15     */
    p_g->c1415x = all_coeff[ 344];            /*    u**14 v**15     */
    p_g->c1515x = all_coeff[ 345];            /*    u**15 v**15     */
    p_g->c1615x = all_coeff[ 346];            /*    u**16 v**15     */
    p_g->c1715x = all_coeff[ 347];            /*    u**17 v**15     */
    p_g->c1815x = all_coeff[ 348];            /*    u**18 v**15     */
    p_g->c1915x = all_coeff[ 349];            /*    u**19 v**15     */
    p_g->c2015x = all_coeff[ 350];            /*    u**20 v**15     */
    p_g->c2115x = all_coeff[ 351];            /*    u**21 v**15     */

    p_g->c0016x = all_coeff[ 352];            /*    u** 0 v**16     */
    p_g->c0116x = all_coeff[ 353];            /*    u** 1 v**16     */
    p_g->c0216x = all_coeff[ 354];            /*    u** 2 v**16     */
    p_g->c0316x = all_coeff[ 355];            /*    u** 3 v**16     */
    p_g->c0416x = all_coeff[ 356];            /*    u** 4 v**16     */
    p_g->c0516x = all_coeff[ 357];            /*    u** 5 v**16     */
    p_g->c0616x = all_coeff[ 358];            /*    u** 6 v**16     */
    p_g->c0716x = all_coeff[ 359];            /*    u** 7 v**16     */
    p_g->c0816x = all_coeff[ 360];            /*    u** 8 v**16     */
    p_g->c0916x = all_coeff[ 361];            /*    u** 9 v**16     */
    p_g->c1016x = all_coeff[ 362];            /*    u**10 v**16     */
    p_g->c1116x = all_coeff[ 363];            /*    u**11 v**16     */
    p_g->c1216x = all_coeff[ 364];            /*    u**12 v**16     */
    p_g->c1316x = all_coeff[ 365];            /*    u**13 v**16     */
    p_g->c1416x = all_coeff[ 366];            /*    u**14 v**16     */
    p_g->c1516x = all_coeff[ 367];            /*    u**15 v**16     */
    p_g->c1616x = all_coeff[ 368];            /*    u**16 v**16     */
    p_g->c1716x = all_coeff[ 369];            /*    u**17 v**16     */
    p_g->c1816x = all_coeff[ 370];            /*    u**18 v**16     */
    p_g->c1916x = all_coeff[ 371];            /*    u**19 v**16     */
    p_g->c2016x = all_coeff[ 372];            /*    u**20 v**16     */
    p_g->c2116x = all_coeff[ 373];            /*    u**21 v**16     */

    p_g->c0017x = all_coeff[ 374];            /*    u** 0 v**17     */
    p_g->c0117x = all_coeff[ 375];            /*    u** 1 v**17     */
    p_g->c0217x = all_coeff[ 376];            /*    u** 2 v**17     */
    p_g->c0317x = all_coeff[ 377];            /*    u** 3 v**17     */
    p_g->c0417x = all_coeff[ 378];            /*    u** 4 v**17     */
    p_g->c0517x = all_coeff[ 379];            /*    u** 5 v**17     */
    p_g->c0617x = all_coeff[ 380];            /*    u** 6 v**17     */
    p_g->c0717x = all_coeff[ 381];            /*    u** 7 v**17     */
    p_g->c0817x = all_coeff[ 382];            /*    u** 8 v**17     */
    p_g->c0917x = all_coeff[ 383];            /*    u** 9 v**17     */
    p_g->c1017x = all_coeff[ 384];            /*    u**10 v**17     */
    p_g->c1117x = all_coeff[ 385];            /*    u**11 v**17     */
    p_g->c1217x = all_coeff[ 386];            /*    u**12 v**17     */
    p_g->c1317x = all_coeff[ 387];            /*    u**13 v**17     */
    p_g->c1417x = all_coeff[ 388];            /*    u**14 v**17     */
    p_g->c1517x = all_coeff[ 389];            /*    u**15 v**17     */
    p_g->c1617x = all_coeff[ 390];            /*    u**16 v**17     */
    p_g->c1717x = all_coeff[ 391];            /*    u**17 v**17     */
    p_g->c1817x = all_coeff[ 392];            /*    u**18 v**17     */
    p_g->c1917x = all_coeff[ 393];            /*    u**19 v**17     */
    p_g->c2017x = all_coeff[ 394];            /*    u**20 v**17     */
    p_g->c2117x = all_coeff[ 395];            /*    u**21 v**17     */

    p_g->c0018x = all_coeff[ 396];            /*    u** 0 v**18     */
    p_g->c0118x = all_coeff[ 397];            /*    u** 1 v**18     */
    p_g->c0218x = all_coeff[ 398];            /*    u** 2 v**18     */
    p_g->c0318x = all_coeff[ 399];            /*    u** 3 v**18     */
    p_g->c0418x = all_coeff[ 400];            /*    u** 4 v**18     */
    p_g->c0518x = all_coeff[ 401];            /*    u** 5 v**18     */
    p_g->c0618x = all_coeff[ 402];            /*    u** 6 v**18     */
    p_g->c0718x = all_coeff[ 403];            /*    u** 7 v**18     */
    p_g->c0818x = all_coeff[ 404];            /*    u** 8 v**18     */
    p_g->c0918x = all_coeff[ 405];            /*    u** 9 v**18     */
    p_g->c1018x = all_coeff[ 406];            /*    u**10 v**18     */
    p_g->c1118x = all_coeff[ 407];            /*    u**11 v**18     */
    p_g->c1218x = all_coeff[ 408];            /*    u**12 v**18     */
    p_g->c1318x = all_coeff[ 409];            /*    u**13 v**18     */
    p_g->c1418x = all_coeff[ 410];            /*    u**14 v**18     */
    p_g->c1518x = all_coeff[ 411];            /*    u**15 v**18     */
    p_g->c1618x = all_coeff[ 412];            /*    u**16 v**18     */
    p_g->c1718x = all_coeff[ 413];            /*    u**17 v**18     */
    p_g->c1818x = all_coeff[ 414];            /*    u**18 v**18     */
    p_g->c1918x = all_coeff[ 415];            /*    u**19 v**18     */
    p_g->c2018x = all_coeff[ 416];            /*    u**20 v**18     */
    p_g->c2118x = all_coeff[ 417];            /*    u**21 v**18     */

    p_g->c0019x = all_coeff[ 418];            /*    u** 0 v**19     */
    p_g->c0119x = all_coeff[ 419];            /*    u** 1 v**19     */
    p_g->c0219x = all_coeff[ 420];            /*    u** 2 v**19     */
    p_g->c0319x = all_coeff[ 421];            /*    u** 3 v**19     */
    p_g->c0419x = all_coeff[ 422];            /*    u** 4 v**19     */
    p_g->c0519x = all_coeff[ 423];            /*    u** 5 v**19     */
    p_g->c0619x = all_coeff[ 424];            /*    u** 6 v**19     */
    p_g->c0719x = all_coeff[ 425];            /*    u** 7 v**19     */
    p_g->c0819x = all_coeff[ 426];            /*    u** 8 v**19     */
    p_g->c0919x = all_coeff[ 427];            /*    u** 9 v**19     */
    p_g->c1019x = all_coeff[ 428];            /*    u**10 v**19     */
    p_g->c1119x = all_coeff[ 429];            /*    u**11 v**19     */
    p_g->c1219x = all_coeff[ 430];            /*    u**12 v**19     */
    p_g->c1319x = all_coeff[ 431];            /*    u**13 v**19     */
    p_g->c1419x = all_coeff[ 432];            /*    u**14 v**19     */
    p_g->c1519x = all_coeff[ 433];            /*    u**15 v**19     */
    p_g->c1619x = all_coeff[ 434];            /*    u**16 v**19     */
    p_g->c1719x = all_coeff[ 435];            /*    u**17 v**19     */
    p_g->c1819x = all_coeff[ 436];            /*    u**18 v**19     */
    p_g->c1919x = all_coeff[ 437];            /*    u**19 v**19     */
    p_g->c2019x = all_coeff[ 438];            /*    u**20 v**19     */
    p_g->c2119x = all_coeff[ 439];            /*    u**21 v**19     */

    p_g->c0020x = all_coeff[ 440];            /*    u** 0 v**20     */
    p_g->c0120x = all_coeff[ 441];            /*    u** 1 v**20     */
    p_g->c0220x = all_coeff[ 442];            /*    u** 2 v**20     */
    p_g->c0320x = all_coeff[ 443];            /*    u** 3 v**20     */
    p_g->c0420x = all_coeff[ 444];            /*    u** 4 v**20     */
    p_g->c0520x = all_coeff[ 445];            /*    u** 5 v**20     */
    p_g->c0620x = all_coeff[ 446];            /*    u** 6 v**20     */
    p_g->c0720x = all_coeff[ 447];            /*    u** 7 v**20     */
    p_g->c0820x = all_coeff[ 448];            /*    u** 8 v**20     */
    p_g->c0920x = all_coeff[ 449];            /*    u** 9 v**20     */
    p_g->c1020x = all_coeff[ 450];            /*    u**10 v**20     */
    p_g->c1120x = all_coeff[ 451];            /*    u**11 v**20     */
    p_g->c1220x = all_coeff[ 452];            /*    u**12 v**20     */
    p_g->c1320x = all_coeff[ 453];            /*    u**13 v**20     */
    p_g->c1420x = all_coeff[ 454];            /*    u**14 v**20     */
    p_g->c1520x = all_coeff[ 455];            /*    u**15 v**20     */
    p_g->c1620x = all_coeff[ 456];            /*    u**16 v**20     */
    p_g->c1720x = all_coeff[ 457];            /*    u**17 v**20     */
    p_g->c1820x = all_coeff[ 458];            /*    u**18 v**20     */
    p_g->c1920x = all_coeff[ 459];            /*    u**19 v**20     */
    p_g->c2020x = all_coeff[ 460];            /*    u**20 v**20     */
    p_g->c2120x = all_coeff[ 461];            /*    u**21 v**20     */

    p_g->c0021x = all_coeff[ 462];            /*    u** 0 v**21     */
    p_g->c0121x = all_coeff[ 463];            /*    u** 1 v**21     */
    p_g->c0221x = all_coeff[ 464];            /*    u** 2 v**21     */
    p_g->c0321x = all_coeff[ 465];            /*    u** 3 v**21     */
    p_g->c0421x = all_coeff[ 466];            /*    u** 4 v**21     */
    p_g->c0521x = all_coeff[ 467];            /*    u** 5 v**21     */
    p_g->c0621x = all_coeff[ 468];            /*    u** 6 v**21     */
    p_g->c0721x = all_coeff[ 469];            /*    u** 7 v**21     */
    p_g->c0821x = all_coeff[ 470];            /*    u** 8 v**21     */
    p_g->c0921x = all_coeff[ 471];            /*    u** 9 v**21     */
    p_g->c1021x = all_coeff[ 472];            /*    u**10 v**21     */
    p_g->c1121x = all_coeff[ 473];            /*    u**11 v**21     */
    p_g->c1221x = all_coeff[ 474];            /*    u**12 v**21     */
    p_g->c1321x = all_coeff[ 475];            /*    u**13 v**21     */
    p_g->c1421x = all_coeff[ 476];            /*    u**14 v**21     */
    p_g->c1521x = all_coeff[ 477];            /*    u**15 v**21     */
    p_g->c1621x = all_coeff[ 478];            /*    u**16 v**21     */
    p_g->c1721x = all_coeff[ 479];            /*    u**17 v**21     */
    p_g->c1821x = all_coeff[ 480];            /*    u**18 v**21     */
    p_g->c1921x = all_coeff[ 481];            /*    u**19 v**21     */
    p_g->c2021x = all_coeff[ 482];            /*    u**20 v**21     */
    p_g->c2121x = all_coeff[ 483];            /*    u**21 v**21     */


    p_g->c0000y = all_coeff[ 484];            /*    u** 0 v** 0     */
    p_g->c0100y = all_coeff[ 485];            /*    u** 1 v** 0     */
    p_g->c0200y = all_coeff[ 486];            /*    u** 2 v** 0     */
    p_g->c0300y = all_coeff[ 487];            /*    u** 3 v** 0     */
    p_g->c0400y = all_coeff[ 488];            /*    u** 4 v** 0     */
    p_g->c0500y = all_coeff[ 489];            /*    u** 5 v** 0     */
    p_g->c0600y = all_coeff[ 490];            /*    u** 6 v** 0     */
    p_g->c0700y = all_coeff[ 491];            /*    u** 7 v** 0     */
    p_g->c0800y = all_coeff[ 492];            /*    u** 8 v** 0     */
    p_g->c0900y = all_coeff[ 493];            /*    u** 9 v** 0     */
    p_g->c1000y = all_coeff[ 494];            /*    u**10 v** 0     */
    p_g->c1100y = all_coeff[ 495];            /*    u**11 v** 0     */
    p_g->c1200y = all_coeff[ 496];            /*    u**12 v** 0     */
    p_g->c1300y = all_coeff[ 497];            /*    u**13 v** 0     */
    p_g->c1400y = all_coeff[ 498];            /*    u**14 v** 0     */
    p_g->c1500y = all_coeff[ 499];            /*    u**15 v** 0     */
    p_g->c1600y = all_coeff[ 500];            /*    u**16 v** 0     */
    p_g->c1700y = all_coeff[ 501];            /*    u**17 v** 0     */
    p_g->c1800y = all_coeff[ 502];            /*    u**18 v** 0     */
    p_g->c1900y = all_coeff[ 503];            /*    u**19 v** 0     */
    p_g->c2000y = all_coeff[ 504];            /*    u**20 v** 0     */
    p_g->c2100y = all_coeff[ 505];            /*    u**21 v** 0     */

    p_g->c0001y = all_coeff[ 506];            /*    u** 0 v** 1     */
    p_g->c0101y = all_coeff[ 507];            /*    u** 1 v** 1     */
    p_g->c0201y = all_coeff[ 508];            /*    u** 2 v** 1     */
    p_g->c0301y = all_coeff[ 509];            /*    u** 3 v** 1     */
    p_g->c0401y = all_coeff[ 510];            /*    u** 4 v** 1     */
    p_g->c0501y = all_coeff[ 511];            /*    u** 5 v** 1     */
    p_g->c0601y = all_coeff[ 512];            /*    u** 6 v** 1     */
    p_g->c0701y = all_coeff[ 513];            /*    u** 7 v** 1     */
    p_g->c0801y = all_coeff[ 514];            /*    u** 8 v** 1     */
    p_g->c0901y = all_coeff[ 515];            /*    u** 9 v** 1     */
    p_g->c1001y = all_coeff[ 516];            /*    u**10 v** 1     */
    p_g->c1101y = all_coeff[ 517];            /*    u**11 v** 1     */
    p_g->c1201y = all_coeff[ 518];            /*    u**12 v** 1     */
    p_g->c1301y = all_coeff[ 519];            /*    u**13 v** 1     */
    p_g->c1401y = all_coeff[ 520];            /*    u**14 v** 1     */
    p_g->c1501y = all_coeff[ 521];            /*    u**15 v** 1     */
    p_g->c1601y = all_coeff[ 522];            /*    u**16 v** 1     */
    p_g->c1701y = all_coeff[ 523];            /*    u**17 v** 1     */
    p_g->c1801y = all_coeff[ 524];            /*    u**18 v** 1     */
    p_g->c1901y = all_coeff[ 525];            /*    u**19 v** 1     */
    p_g->c2001y = all_coeff[ 526];            /*    u**20 v** 1     */
    p_g->c2101y = all_coeff[ 527];            /*    u**21 v** 1     */

    p_g->c0002y = all_coeff[ 528];            /*    u** 0 v** 2     */
    p_g->c0102y = all_coeff[ 529];            /*    u** 1 v** 2     */
    p_g->c0202y = all_coeff[ 530];            /*    u** 2 v** 2     */
    p_g->c0302y = all_coeff[ 531];            /*    u** 3 v** 2     */
    p_g->c0402y = all_coeff[ 532];            /*    u** 4 v** 2     */
    p_g->c0502y = all_coeff[ 533];            /*    u** 5 v** 2     */
    p_g->c0602y = all_coeff[ 534];            /*    u** 6 v** 2     */
    p_g->c0702y = all_coeff[ 535];            /*    u** 7 v** 2     */
    p_g->c0802y = all_coeff[ 536];            /*    u** 8 v** 2     */
    p_g->c0902y = all_coeff[ 537];            /*    u** 9 v** 2     */
    p_g->c1002y = all_coeff[ 538];            /*    u**10 v** 2     */
    p_g->c1102y = all_coeff[ 539];            /*    u**11 v** 2     */
    p_g->c1202y = all_coeff[ 540];            /*    u**12 v** 2     */
    p_g->c1302y = all_coeff[ 541];            /*    u**13 v** 2     */
    p_g->c1402y = all_coeff[ 542];            /*    u**14 v** 2     */
    p_g->c1502y = all_coeff[ 543];            /*    u**15 v** 2     */
    p_g->c1602y = all_coeff[ 544];            /*    u**16 v** 2     */
    p_g->c1702y = all_coeff[ 545];            /*    u**17 v** 2     */
    p_g->c1802y = all_coeff[ 546];            /*    u**18 v** 2     */
    p_g->c1902y = all_coeff[ 547];            /*    u**19 v** 2     */
    p_g->c2002y = all_coeff[ 548];            /*    u**20 v** 2     */
    p_g->c2102y = all_coeff[ 549];            /*    u**21 v** 2     */

    p_g->c0003y = all_coeff[ 550];            /*    u** 0 v** 3     */
    p_g->c0103y = all_coeff[ 551];            /*    u** 1 v** 3     */
    p_g->c0203y = all_coeff[ 552];            /*    u** 2 v** 3     */
    p_g->c0303y = all_coeff[ 553];            /*    u** 3 v** 3     */
    p_g->c0403y = all_coeff[ 554];            /*    u** 4 v** 3     */
    p_g->c0503y = all_coeff[ 555];            /*    u** 5 v** 3     */
    p_g->c0603y = all_coeff[ 556];            /*    u** 6 v** 3     */
    p_g->c0703y = all_coeff[ 557];            /*    u** 7 v** 3     */
    p_g->c0803y = all_coeff[ 558];            /*    u** 8 v** 3     */
    p_g->c0903y = all_coeff[ 559];            /*    u** 9 v** 3     */
    p_g->c1003y = all_coeff[ 560];            /*    u**10 v** 3     */
    p_g->c1103y = all_coeff[ 561];            /*    u**11 v** 3     */
    p_g->c1203y = all_coeff[ 562];            /*    u**12 v** 3     */
    p_g->c1303y = all_coeff[ 563];            /*    u**13 v** 3     */
    p_g->c1403y = all_coeff[ 564];            /*    u**14 v** 3     */
    p_g->c1503y = all_coeff[ 565];            /*    u**15 v** 3     */
    p_g->c1603y = all_coeff[ 566];            /*    u**16 v** 3     */
    p_g->c1703y = all_coeff[ 567];            /*    u**17 v** 3     */
    p_g->c1803y = all_coeff[ 568];            /*    u**18 v** 3     */
    p_g->c1903y = all_coeff[ 569];            /*    u**19 v** 3     */
    p_g->c2003y = all_coeff[ 570];            /*    u**20 v** 3     */
    p_g->c2103y = all_coeff[ 571];            /*    u**21 v** 3     */

    p_g->c0004y = all_coeff[ 572];            /*    u** 0 v** 4     */
    p_g->c0104y = all_coeff[ 573];            /*    u** 1 v** 4     */
    p_g->c0204y = all_coeff[ 574];            /*    u** 2 v** 4     */
    p_g->c0304y = all_coeff[ 575];            /*    u** 3 v** 4     */
    p_g->c0404y = all_coeff[ 576];            /*    u** 4 v** 4     */
    p_g->c0504y = all_coeff[ 577];            /*    u** 5 v** 4     */
    p_g->c0604y = all_coeff[ 578];            /*    u** 6 v** 4     */
    p_g->c0704y = all_coeff[ 579];            /*    u** 7 v** 4     */
    p_g->c0804y = all_coeff[ 580];            /*    u** 8 v** 4     */
    p_g->c0904y = all_coeff[ 581];            /*    u** 9 v** 4     */
    p_g->c1004y = all_coeff[ 582];            /*    u**10 v** 4     */
    p_g->c1104y = all_coeff[ 583];            /*    u**11 v** 4     */
    p_g->c1204y = all_coeff[ 584];            /*    u**12 v** 4     */
    p_g->c1304y = all_coeff[ 585];            /*    u**13 v** 4     */
    p_g->c1404y = all_coeff[ 586];            /*    u**14 v** 4     */
    p_g->c1504y = all_coeff[ 587];            /*    u**15 v** 4     */
    p_g->c1604y = all_coeff[ 588];            /*    u**16 v** 4     */
    p_g->c1704y = all_coeff[ 589];            /*    u**17 v** 4     */
    p_g->c1804y = all_coeff[ 590];            /*    u**18 v** 4     */
    p_g->c1904y = all_coeff[ 591];            /*    u**19 v** 4     */
    p_g->c2004y = all_coeff[ 592];            /*    u**20 v** 4     */
    p_g->c2104y = all_coeff[ 593];            /*    u**21 v** 4     */

    p_g->c0005y = all_coeff[ 594];            /*    u** 0 v** 5     */
    p_g->c0105y = all_coeff[ 595];            /*    u** 1 v** 5     */
    p_g->c0205y = all_coeff[ 596];            /*    u** 2 v** 5     */
    p_g->c0305y = all_coeff[ 597];            /*    u** 3 v** 5     */
    p_g->c0405y = all_coeff[ 598];            /*    u** 4 v** 5     */
    p_g->c0505y = all_coeff[ 599];            /*    u** 5 v** 5     */
    p_g->c0605y = all_coeff[ 600];            /*    u** 6 v** 5     */
    p_g->c0705y = all_coeff[ 601];            /*    u** 7 v** 5     */
    p_g->c0805y = all_coeff[ 602];            /*    u** 8 v** 5     */
    p_g->c0905y = all_coeff[ 603];            /*    u** 9 v** 5     */
    p_g->c1005y = all_coeff[ 604];            /*    u**10 v** 5     */
    p_g->c1105y = all_coeff[ 605];            /*    u**11 v** 5     */
    p_g->c1205y = all_coeff[ 606];            /*    u**12 v** 5     */
    p_g->c1305y = all_coeff[ 607];            /*    u**13 v** 5     */
    p_g->c1405y = all_coeff[ 608];            /*    u**14 v** 5     */
    p_g->c1505y = all_coeff[ 609];            /*    u**15 v** 5     */
    p_g->c1605y = all_coeff[ 610];            /*    u**16 v** 5     */
    p_g->c1705y = all_coeff[ 611];            /*    u**17 v** 5     */
    p_g->c1805y = all_coeff[ 612];            /*    u**18 v** 5     */
    p_g->c1905y = all_coeff[ 613];            /*    u**19 v** 5     */
    p_g->c2005y = all_coeff[ 614];            /*    u**20 v** 5     */
    p_g->c2105y = all_coeff[ 615];            /*    u**21 v** 5     */

    p_g->c0006y = all_coeff[ 616];            /*    u** 0 v** 6     */
    p_g->c0106y = all_coeff[ 617];            /*    u** 1 v** 6     */
    p_g->c0206y = all_coeff[ 618];            /*    u** 2 v** 6     */
    p_g->c0306y = all_coeff[ 619];            /*    u** 3 v** 6     */
    p_g->c0406y = all_coeff[ 620];            /*    u** 4 v** 6     */
    p_g->c0506y = all_coeff[ 621];            /*    u** 5 v** 6     */
    p_g->c0606y = all_coeff[ 622];            /*    u** 6 v** 6     */
    p_g->c0706y = all_coeff[ 623];            /*    u** 7 v** 6     */
    p_g->c0806y = all_coeff[ 624];            /*    u** 8 v** 6     */
    p_g->c0906y = all_coeff[ 625];            /*    u** 9 v** 6     */
    p_g->c1006y = all_coeff[ 626];            /*    u**10 v** 6     */
    p_g->c1106y = all_coeff[ 627];            /*    u**11 v** 6     */
    p_g->c1206y = all_coeff[ 628];            /*    u**12 v** 6     */
    p_g->c1306y = all_coeff[ 629];            /*    u**13 v** 6     */
    p_g->c1406y = all_coeff[ 630];            /*    u**14 v** 6     */
    p_g->c1506y = all_coeff[ 631];            /*    u**15 v** 6     */
    p_g->c1606y = all_coeff[ 632];            /*    u**16 v** 6     */
    p_g->c1706y = all_coeff[ 633];            /*    u**17 v** 6     */
    p_g->c1806y = all_coeff[ 634];            /*    u**18 v** 6     */
    p_g->c1906y = all_coeff[ 635];            /*    u**19 v** 6     */
    p_g->c2006y = all_coeff[ 636];            /*    u**20 v** 6     */
    p_g->c2106y = all_coeff[ 637];            /*    u**21 v** 6     */

    p_g->c0007y = all_coeff[ 638];            /*    u** 0 v** 7     */
    p_g->c0107y = all_coeff[ 639];            /*    u** 1 v** 7     */
    p_g->c0207y = all_coeff[ 640];            /*    u** 2 v** 7     */
    p_g->c0307y = all_coeff[ 641];            /*    u** 3 v** 7     */
    p_g->c0407y = all_coeff[ 642];            /*    u** 4 v** 7     */
    p_g->c0507y = all_coeff[ 643];            /*    u** 5 v** 7     */
    p_g->c0607y = all_coeff[ 644];            /*    u** 6 v** 7     */
    p_g->c0707y = all_coeff[ 645];            /*    u** 7 v** 7     */
    p_g->c0807y = all_coeff[ 646];            /*    u** 8 v** 7     */
    p_g->c0907y = all_coeff[ 647];            /*    u** 9 v** 7     */
    p_g->c1007y = all_coeff[ 648];            /*    u**10 v** 7     */
    p_g->c1107y = all_coeff[ 649];            /*    u**11 v** 7     */
    p_g->c1207y = all_coeff[ 650];            /*    u**12 v** 7     */
    p_g->c1307y = all_coeff[ 651];            /*    u**13 v** 7     */
    p_g->c1407y = all_coeff[ 652];            /*    u**14 v** 7     */
    p_g->c1507y = all_coeff[ 653];            /*    u**15 v** 7     */
    p_g->c1607y = all_coeff[ 654];            /*    u**16 v** 7     */
    p_g->c1707y = all_coeff[ 655];            /*    u**17 v** 7     */
    p_g->c1807y = all_coeff[ 656];            /*    u**18 v** 7     */
    p_g->c1907y = all_coeff[ 657];            /*    u**19 v** 7     */
    p_g->c2007y = all_coeff[ 658];            /*    u**20 v** 7     */
    p_g->c2107y = all_coeff[ 659];            /*    u**21 v** 7     */

    p_g->c0008y = all_coeff[ 660];            /*    u** 0 v** 8     */
    p_g->c0108y = all_coeff[ 661];            /*    u** 1 v** 8     */
    p_g->c0208y = all_coeff[ 662];            /*    u** 2 v** 8     */
    p_g->c0308y = all_coeff[ 663];            /*    u** 3 v** 8     */
    p_g->c0408y = all_coeff[ 664];            /*    u** 4 v** 8     */
    p_g->c0508y = all_coeff[ 665];            /*    u** 5 v** 8     */
    p_g->c0608y = all_coeff[ 666];            /*    u** 6 v** 8     */
    p_g->c0708y = all_coeff[ 667];            /*    u** 7 v** 8     */
    p_g->c0808y = all_coeff[ 668];            /*    u** 8 v** 8     */
    p_g->c0908y = all_coeff[ 669];            /*    u** 9 v** 8     */
    p_g->c1008y = all_coeff[ 670];            /*    u**10 v** 8     */
    p_g->c1108y = all_coeff[ 671];            /*    u**11 v** 8     */
    p_g->c1208y = all_coeff[ 672];            /*    u**12 v** 8     */
    p_g->c1308y = all_coeff[ 673];            /*    u**13 v** 8     */
    p_g->c1408y = all_coeff[ 674];            /*    u**14 v** 8     */
    p_g->c1508y = all_coeff[ 675];            /*    u**15 v** 8     */
    p_g->c1608y = all_coeff[ 676];            /*    u**16 v** 8     */
    p_g->c1708y = all_coeff[ 677];            /*    u**17 v** 8     */
    p_g->c1808y = all_coeff[ 678];            /*    u**18 v** 8     */
    p_g->c1908y = all_coeff[ 679];            /*    u**19 v** 8     */
    p_g->c2008y = all_coeff[ 680];            /*    u**20 v** 8     */
    p_g->c2108y = all_coeff[ 681];            /*    u**21 v** 8     */

    p_g->c0009y = all_coeff[ 682];            /*    u** 0 v** 9     */
    p_g->c0109y = all_coeff[ 683];            /*    u** 1 v** 9     */
    p_g->c0209y = all_coeff[ 684];            /*    u** 2 v** 9     */
    p_g->c0309y = all_coeff[ 685];            /*    u** 3 v** 9     */
    p_g->c0409y = all_coeff[ 686];            /*    u** 4 v** 9     */
    p_g->c0509y = all_coeff[ 687];            /*    u** 5 v** 9     */
    p_g->c0609y = all_coeff[ 688];            /*    u** 6 v** 9     */
    p_g->c0709y = all_coeff[ 689];            /*    u** 7 v** 9     */
    p_g->c0809y = all_coeff[ 690];            /*    u** 8 v** 9     */
    p_g->c0909y = all_coeff[ 691];            /*    u** 9 v** 9     */
    p_g->c1009y = all_coeff[ 692];            /*    u**10 v** 9     */
    p_g->c1109y = all_coeff[ 693];            /*    u**11 v** 9     */
    p_g->c1209y = all_coeff[ 694];            /*    u**12 v** 9     */
    p_g->c1309y = all_coeff[ 695];            /*    u**13 v** 9     */
    p_g->c1409y = all_coeff[ 696];            /*    u**14 v** 9     */
    p_g->c1509y = all_coeff[ 697];            /*    u**15 v** 9     */
    p_g->c1609y = all_coeff[ 698];            /*    u**16 v** 9     */
    p_g->c1709y = all_coeff[ 699];            /*    u**17 v** 9     */
    p_g->c1809y = all_coeff[ 700];            /*    u**18 v** 9     */
    p_g->c1909y = all_coeff[ 701];            /*    u**19 v** 9     */
    p_g->c2009y = all_coeff[ 702];            /*    u**20 v** 9     */
    p_g->c2109y = all_coeff[ 703];            /*    u**21 v** 9     */

    p_g->c0010y = all_coeff[ 704];            /*    u** 0 v**10     */
    p_g->c0110y = all_coeff[ 705];            /*    u** 1 v**10     */
    p_g->c0210y = all_coeff[ 706];            /*    u** 2 v**10     */
    p_g->c0310y = all_coeff[ 707];            /*    u** 3 v**10     */
    p_g->c0410y = all_coeff[ 708];            /*    u** 4 v**10     */
    p_g->c0510y = all_coeff[ 709];            /*    u** 5 v**10     */
    p_g->c0610y = all_coeff[ 710];            /*    u** 6 v**10     */
    p_g->c0710y = all_coeff[ 711];            /*    u** 7 v**10     */
    p_g->c0810y = all_coeff[ 712];            /*    u** 8 v**10     */
    p_g->c0910y = all_coeff[ 713];            /*    u** 9 v**10     */
    p_g->c1010y = all_coeff[ 714];            /*    u**10 v**10     */
    p_g->c1110y = all_coeff[ 715];            /*    u**11 v**10     */
    p_g->c1210y = all_coeff[ 716];            /*    u**12 v**10     */
    p_g->c1310y = all_coeff[ 717];            /*    u**13 v**10     */
    p_g->c1410y = all_coeff[ 718];            /*    u**14 v**10     */
    p_g->c1510y = all_coeff[ 719];            /*    u**15 v**10     */
    p_g->c1610y = all_coeff[ 720];            /*    u**16 v**10     */
    p_g->c1710y = all_coeff[ 721];            /*    u**17 v**10     */
    p_g->c1810y = all_coeff[ 722];            /*    u**18 v**10     */
    p_g->c1910y = all_coeff[ 723];            /*    u**19 v**10     */
    p_g->c2010y = all_coeff[ 724];            /*    u**20 v**10     */
    p_g->c2110y = all_coeff[ 725];            /*    u**21 v**10     */

    p_g->c0011y = all_coeff[ 726];            /*    u** 0 v**11     */
    p_g->c0111y = all_coeff[ 727];            /*    u** 1 v**11     */
    p_g->c0211y = all_coeff[ 728];            /*    u** 2 v**11     */
    p_g->c0311y = all_coeff[ 729];            /*    u** 3 v**11     */
    p_g->c0411y = all_coeff[ 730];            /*    u** 4 v**11     */
    p_g->c0511y = all_coeff[ 731];            /*    u** 5 v**11     */
    p_g->c0611y = all_coeff[ 732];            /*    u** 6 v**11     */
    p_g->c0711y = all_coeff[ 733];            /*    u** 7 v**11     */
    p_g->c0811y = all_coeff[ 734];            /*    u** 8 v**11     */
    p_g->c0911y = all_coeff[ 735];            /*    u** 9 v**11     */
    p_g->c1011y = all_coeff[ 736];            /*    u**10 v**11     */
    p_g->c1111y = all_coeff[ 737];            /*    u**11 v**11     */
    p_g->c1211y = all_coeff[ 738];            /*    u**12 v**11     */
    p_g->c1311y = all_coeff[ 739];            /*    u**13 v**11     */
    p_g->c1411y = all_coeff[ 740];            /*    u**14 v**11     */
    p_g->c1511y = all_coeff[ 741];            /*    u**15 v**11     */
    p_g->c1611y = all_coeff[ 742];            /*    u**16 v**11     */
    p_g->c1711y = all_coeff[ 743];            /*    u**17 v**11     */
    p_g->c1811y = all_coeff[ 744];            /*    u**18 v**11     */
    p_g->c1911y = all_coeff[ 745];            /*    u**19 v**11     */
    p_g->c2011y = all_coeff[ 746];            /*    u**20 v**11     */
    p_g->c2111y = all_coeff[ 747];            /*    u**21 v**11     */

    p_g->c0012y = all_coeff[ 748];            /*    u** 0 v**12     */
    p_g->c0112y = all_coeff[ 749];            /*    u** 1 v**12     */
    p_g->c0212y = all_coeff[ 750];            /*    u** 2 v**12     */
    p_g->c0312y = all_coeff[ 751];            /*    u** 3 v**12     */
    p_g->c0412y = all_coeff[ 752];            /*    u** 4 v**12     */
    p_g->c0512y = all_coeff[ 753];            /*    u** 5 v**12     */
    p_g->c0612y = all_coeff[ 754];            /*    u** 6 v**12     */
    p_g->c0712y = all_coeff[ 755];            /*    u** 7 v**12     */
    p_g->c0812y = all_coeff[ 756];            /*    u** 8 v**12     */
    p_g->c0912y = all_coeff[ 757];            /*    u** 9 v**12     */
    p_g->c1012y = all_coeff[ 758];            /*    u**10 v**12     */
    p_g->c1112y = all_coeff[ 759];            /*    u**11 v**12     */
    p_g->c1212y = all_coeff[ 760];            /*    u**12 v**12     */
    p_g->c1312y = all_coeff[ 761];            /*    u**13 v**12     */
    p_g->c1412y = all_coeff[ 762];            /*    u**14 v**12     */
    p_g->c1512y = all_coeff[ 763];            /*    u**15 v**12     */
    p_g->c1612y = all_coeff[ 764];            /*    u**16 v**12     */
    p_g->c1712y = all_coeff[ 765];            /*    u**17 v**12     */
    p_g->c1812y = all_coeff[ 766];            /*    u**18 v**12     */
    p_g->c1912y = all_coeff[ 767];            /*    u**19 v**12     */
    p_g->c2012y = all_coeff[ 768];            /*    u**20 v**12     */
    p_g->c2112y = all_coeff[ 769];            /*    u**21 v**12     */

    p_g->c0013y = all_coeff[ 770];            /*    u** 0 v**13     */
    p_g->c0113y = all_coeff[ 771];            /*    u** 1 v**13     */
    p_g->c0213y = all_coeff[ 772];            /*    u** 2 v**13     */
    p_g->c0313y = all_coeff[ 773];            /*    u** 3 v**13     */
    p_g->c0413y = all_coeff[ 774];            /*    u** 4 v**13     */
    p_g->c0513y = all_coeff[ 775];            /*    u** 5 v**13     */
    p_g->c0613y = all_coeff[ 776];            /*    u** 6 v**13     */
    p_g->c0713y = all_coeff[ 777];            /*    u** 7 v**13     */
    p_g->c0813y = all_coeff[ 778];            /*    u** 8 v**13     */
    p_g->c0913y = all_coeff[ 779];            /*    u** 9 v**13     */
    p_g->c1013y = all_coeff[ 780];            /*    u**10 v**13     */
    p_g->c1113y = all_coeff[ 781];            /*    u**11 v**13     */
    p_g->c1213y = all_coeff[ 782];            /*    u**12 v**13     */
    p_g->c1313y = all_coeff[ 783];            /*    u**13 v**13     */
    p_g->c1413y = all_coeff[ 784];            /*    u**14 v**13     */
    p_g->c1513y = all_coeff[ 785];            /*    u**15 v**13     */
    p_g->c1613y = all_coeff[ 786];            /*    u**16 v**13     */
    p_g->c1713y = all_coeff[ 787];            /*    u**17 v**13     */
    p_g->c1813y = all_coeff[ 788];            /*    u**18 v**13     */
    p_g->c1913y = all_coeff[ 789];            /*    u**19 v**13     */
    p_g->c2013y = all_coeff[ 790];            /*    u**20 v**13     */
    p_g->c2113y = all_coeff[ 791];            /*    u**21 v**13     */

    p_g->c0014y = all_coeff[ 792];            /*    u** 0 v**14     */
    p_g->c0114y = all_coeff[ 793];            /*    u** 1 v**14     */
    p_g->c0214y = all_coeff[ 794];            /*    u** 2 v**14     */
    p_g->c0314y = all_coeff[ 795];            /*    u** 3 v**14     */
    p_g->c0414y = all_coeff[ 796];            /*    u** 4 v**14     */
    p_g->c0514y = all_coeff[ 797];            /*    u** 5 v**14     */
    p_g->c0614y = all_coeff[ 798];            /*    u** 6 v**14     */
    p_g->c0714y = all_coeff[ 799];            /*    u** 7 v**14     */
    p_g->c0814y = all_coeff[ 800];            /*    u** 8 v**14     */
    p_g->c0914y = all_coeff[ 801];            /*    u** 9 v**14     */
    p_g->c1014y = all_coeff[ 802];            /*    u**10 v**14     */
    p_g->c1114y = all_coeff[ 803];            /*    u**11 v**14     */
    p_g->c1214y = all_coeff[ 804];            /*    u**12 v**14     */
    p_g->c1314y = all_coeff[ 805];            /*    u**13 v**14     */
    p_g->c1414y = all_coeff[ 806];            /*    u**14 v**14     */
    p_g->c1514y = all_coeff[ 807];            /*    u**15 v**14     */
    p_g->c1614y = all_coeff[ 808];            /*    u**16 v**14     */
    p_g->c1714y = all_coeff[ 809];            /*    u**17 v**14     */
    p_g->c1814y = all_coeff[ 810];            /*    u**18 v**14     */
    p_g->c1914y = all_coeff[ 811];            /*    u**19 v**14     */
    p_g->c2014y = all_coeff[ 812];            /*    u**20 v**14     */
    p_g->c2114y = all_coeff[ 813];            /*    u**21 v**14     */

    p_g->c0015y = all_coeff[ 814];            /*    u** 0 v**15     */
    p_g->c0115y = all_coeff[ 815];            /*    u** 1 v**15     */
    p_g->c0215y = all_coeff[ 816];            /*    u** 2 v**15     */
    p_g->c0315y = all_coeff[ 817];            /*    u** 3 v**15     */
    p_g->c0415y = all_coeff[ 818];            /*    u** 4 v**15     */
    p_g->c0515y = all_coeff[ 819];            /*    u** 5 v**15     */
    p_g->c0615y = all_coeff[ 820];            /*    u** 6 v**15     */
    p_g->c0715y = all_coeff[ 821];            /*    u** 7 v**15     */
    p_g->c0815y = all_coeff[ 822];            /*    u** 8 v**15     */
    p_g->c0915y = all_coeff[ 823];            /*    u** 9 v**15     */
    p_g->c1015y = all_coeff[ 824];            /*    u**10 v**15     */
    p_g->c1115y = all_coeff[ 825];            /*    u**11 v**15     */
    p_g->c1215y = all_coeff[ 826];            /*    u**12 v**15     */
    p_g->c1315y = all_coeff[ 827];            /*    u**13 v**15     */
    p_g->c1415y = all_coeff[ 828];            /*    u**14 v**15     */
    p_g->c1515y = all_coeff[ 829];            /*    u**15 v**15     */
    p_g->c1615y = all_coeff[ 830];            /*    u**16 v**15     */
    p_g->c1715y = all_coeff[ 831];            /*    u**17 v**15     */
    p_g->c1815y = all_coeff[ 832];            /*    u**18 v**15     */
    p_g->c1915y = all_coeff[ 833];            /*    u**19 v**15     */
    p_g->c2015y = all_coeff[ 834];            /*    u**20 v**15     */
    p_g->c2115y = all_coeff[ 835];            /*    u**21 v**15     */

    p_g->c0016y = all_coeff[ 836];            /*    u** 0 v**16     */
    p_g->c0116y = all_coeff[ 837];            /*    u** 1 v**16     */
    p_g->c0216y = all_coeff[ 838];            /*    u** 2 v**16     */
    p_g->c0316y = all_coeff[ 839];            /*    u** 3 v**16     */
    p_g->c0416y = all_coeff[ 840];            /*    u** 4 v**16     */
    p_g->c0516y = all_coeff[ 841];            /*    u** 5 v**16     */
    p_g->c0616y = all_coeff[ 842];            /*    u** 6 v**16     */
    p_g->c0716y = all_coeff[ 843];            /*    u** 7 v**16     */
    p_g->c0816y = all_coeff[ 844];            /*    u** 8 v**16     */
    p_g->c0916y = all_coeff[ 845];            /*    u** 9 v**16     */
    p_g->c1016y = all_coeff[ 846];            /*    u**10 v**16     */
    p_g->c1116y = all_coeff[ 847];            /*    u**11 v**16     */
    p_g->c1216y = all_coeff[ 848];            /*    u**12 v**16     */
    p_g->c1316y = all_coeff[ 849];            /*    u**13 v**16     */
    p_g->c1416y = all_coeff[ 850];            /*    u**14 v**16     */
    p_g->c1516y = all_coeff[ 851];            /*    u**15 v**16     */
    p_g->c1616y = all_coeff[ 852];            /*    u**16 v**16     */
    p_g->c1716y = all_coeff[ 853];            /*    u**17 v**16     */
    p_g->c1816y = all_coeff[ 854];            /*    u**18 v**16     */
    p_g->c1916y = all_coeff[ 855];            /*    u**19 v**16     */
    p_g->c2016y = all_coeff[ 856];            /*    u**20 v**16     */
    p_g->c2116y = all_coeff[ 857];            /*    u**21 v**16     */

    p_g->c0017y = all_coeff[ 858];            /*    u** 0 v**17     */
    p_g->c0117y = all_coeff[ 859];            /*    u** 1 v**17     */
    p_g->c0217y = all_coeff[ 860];            /*    u** 2 v**17     */
    p_g->c0317y = all_coeff[ 861];            /*    u** 3 v**17     */
    p_g->c0417y = all_coeff[ 862];            /*    u** 4 v**17     */
    p_g->c0517y = all_coeff[ 863];            /*    u** 5 v**17     */
    p_g->c0617y = all_coeff[ 864];            /*    u** 6 v**17     */
    p_g->c0717y = all_coeff[ 865];            /*    u** 7 v**17     */
    p_g->c0817y = all_coeff[ 866];            /*    u** 8 v**17     */
    p_g->c0917y = all_coeff[ 867];            /*    u** 9 v**17     */
    p_g->c1017y = all_coeff[ 868];            /*    u**10 v**17     */
    p_g->c1117y = all_coeff[ 869];            /*    u**11 v**17     */
    p_g->c1217y = all_coeff[ 870];            /*    u**12 v**17     */
    p_g->c1317y = all_coeff[ 871];            /*    u**13 v**17     */
    p_g->c1417y = all_coeff[ 872];            /*    u**14 v**17     */
    p_g->c1517y = all_coeff[ 873];            /*    u**15 v**17     */
    p_g->c1617y = all_coeff[ 874];            /*    u**16 v**17     */
    p_g->c1717y = all_coeff[ 875];            /*    u**17 v**17     */
    p_g->c1817y = all_coeff[ 876];            /*    u**18 v**17     */
    p_g->c1917y = all_coeff[ 877];            /*    u**19 v**17     */
    p_g->c2017y = all_coeff[ 878];            /*    u**20 v**17     */
    p_g->c2117y = all_coeff[ 879];            /*    u**21 v**17     */

    p_g->c0018y = all_coeff[ 880];            /*    u** 0 v**18     */
    p_g->c0118y = all_coeff[ 881];            /*    u** 1 v**18     */
    p_g->c0218y = all_coeff[ 882];            /*    u** 2 v**18     */
    p_g->c0318y = all_coeff[ 883];            /*    u** 3 v**18     */
    p_g->c0418y = all_coeff[ 884];            /*    u** 4 v**18     */
    p_g->c0518y = all_coeff[ 885];            /*    u** 5 v**18     */
    p_g->c0618y = all_coeff[ 886];            /*    u** 6 v**18     */
    p_g->c0718y = all_coeff[ 887];            /*    u** 7 v**18     */
    p_g->c0818y = all_coeff[ 888];            /*    u** 8 v**18     */
    p_g->c0918y = all_coeff[ 889];            /*    u** 9 v**18     */
    p_g->c1018y = all_coeff[ 890];            /*    u**10 v**18     */
    p_g->c1118y = all_coeff[ 891];            /*    u**11 v**18     */
    p_g->c1218y = all_coeff[ 892];            /*    u**12 v**18     */
    p_g->c1318y = all_coeff[ 893];            /*    u**13 v**18     */
    p_g->c1418y = all_coeff[ 894];            /*    u**14 v**18     */
    p_g->c1518y = all_coeff[ 895];            /*    u**15 v**18     */
    p_g->c1618y = all_coeff[ 896];            /*    u**16 v**18     */
    p_g->c1718y = all_coeff[ 897];            /*    u**17 v**18     */
    p_g->c1818y = all_coeff[ 898];            /*    u**18 v**18     */
    p_g->c1918y = all_coeff[ 899];            /*    u**19 v**18     */
    p_g->c2018y = all_coeff[ 900];            /*    u**20 v**18     */
    p_g->c2118y = all_coeff[ 901];            /*    u**21 v**18     */

    p_g->c0019y = all_coeff[ 902];            /*    u** 0 v**19     */
    p_g->c0119y = all_coeff[ 903];            /*    u** 1 v**19     */
    p_g->c0219y = all_coeff[ 904];            /*    u** 2 v**19     */
    p_g->c0319y = all_coeff[ 905];            /*    u** 3 v**19     */
    p_g->c0419y = all_coeff[ 906];            /*    u** 4 v**19     */
    p_g->c0519y = all_coeff[ 907];            /*    u** 5 v**19     */
    p_g->c0619y = all_coeff[ 908];            /*    u** 6 v**19     */
    p_g->c0719y = all_coeff[ 909];            /*    u** 7 v**19     */
    p_g->c0819y = all_coeff[ 910];            /*    u** 8 v**19     */
    p_g->c0919y = all_coeff[ 911];            /*    u** 9 v**19     */
    p_g->c1019y = all_coeff[ 912];            /*    u**10 v**19     */
    p_g->c1119y = all_coeff[ 913];            /*    u**11 v**19     */
    p_g->c1219y = all_coeff[ 914];            /*    u**12 v**19     */
    p_g->c1319y = all_coeff[ 915];            /*    u**13 v**19     */
    p_g->c1419y = all_coeff[ 916];            /*    u**14 v**19     */
    p_g->c1519y = all_coeff[ 917];            /*    u**15 v**19     */
    p_g->c1619y = all_coeff[ 918];            /*    u**16 v**19     */
    p_g->c1719y = all_coeff[ 919];            /*    u**17 v**19     */
    p_g->c1819y = all_coeff[ 920];            /*    u**18 v**19     */
    p_g->c1919y = all_coeff[ 921];            /*    u**19 v**19     */
    p_g->c2019y = all_coeff[ 922];            /*    u**20 v**19     */
    p_g->c2119y = all_coeff[ 923];            /*    u**21 v**19     */

    p_g->c0020y = all_coeff[ 924];            /*    u** 0 v**20     */
    p_g->c0120y = all_coeff[ 925];            /*    u** 1 v**20     */
    p_g->c0220y = all_coeff[ 926];            /*    u** 2 v**20     */
    p_g->c0320y = all_coeff[ 927];            /*    u** 3 v**20     */
    p_g->c0420y = all_coeff[ 928];            /*    u** 4 v**20     */
    p_g->c0520y = all_coeff[ 929];            /*    u** 5 v**20     */
    p_g->c0620y = all_coeff[ 930];            /*    u** 6 v**20     */
    p_g->c0720y = all_coeff[ 931];            /*    u** 7 v**20     */
    p_g->c0820y = all_coeff[ 932];            /*    u** 8 v**20     */
    p_g->c0920y = all_coeff[ 933];            /*    u** 9 v**20     */
    p_g->c1020y = all_coeff[ 934];            /*    u**10 v**20     */
    p_g->c1120y = all_coeff[ 935];            /*    u**11 v**20     */
    p_g->c1220y = all_coeff[ 936];            /*    u**12 v**20     */
    p_g->c1320y = all_coeff[ 937];            /*    u**13 v**20     */
    p_g->c1420y = all_coeff[ 938];            /*    u**14 v**20     */
    p_g->c1520y = all_coeff[ 939];            /*    u**15 v**20     */
    p_g->c1620y = all_coeff[ 940];            /*    u**16 v**20     */
    p_g->c1720y = all_coeff[ 941];            /*    u**17 v**20     */
    p_g->c1820y = all_coeff[ 942];            /*    u**18 v**20     */
    p_g->c1920y = all_coeff[ 943];            /*    u**19 v**20     */
    p_g->c2020y = all_coeff[ 944];            /*    u**20 v**20     */
    p_g->c2120y = all_coeff[ 945];            /*    u**21 v**20     */

    p_g->c0021y = all_coeff[ 946];            /*    u** 0 v**21     */
    p_g->c0121y = all_coeff[ 947];            /*    u** 1 v**21     */
    p_g->c0221y = all_coeff[ 948];            /*    u** 2 v**21     */
    p_g->c0321y = all_coeff[ 949];            /*    u** 3 v**21     */
    p_g->c0421y = all_coeff[ 950];            /*    u** 4 v**21     */
    p_g->c0521y = all_coeff[ 951];            /*    u** 5 v**21     */
    p_g->c0621y = all_coeff[ 952];            /*    u** 6 v**21     */
    p_g->c0721y = all_coeff[ 953];            /*    u** 7 v**21     */
    p_g->c0821y = all_coeff[ 954];            /*    u** 8 v**21     */
    p_g->c0921y = all_coeff[ 955];            /*    u** 9 v**21     */
    p_g->c1021y = all_coeff[ 956];            /*    u**10 v**21     */
    p_g->c1121y = all_coeff[ 957];            /*    u**11 v**21     */
    p_g->c1221y = all_coeff[ 958];            /*    u**12 v**21     */
    p_g->c1321y = all_coeff[ 959];            /*    u**13 v**21     */
    p_g->c1421y = all_coeff[ 960];            /*    u**14 v**21     */
    p_g->c1521y = all_coeff[ 961];            /*    u**15 v**21     */
    p_g->c1621y = all_coeff[ 962];            /*    u**16 v**21     */
    p_g->c1721y = all_coeff[ 963];            /*    u**17 v**21     */
    p_g->c1821y = all_coeff[ 964];            /*    u**18 v**21     */
    p_g->c1921y = all_coeff[ 965];            /*    u**19 v**21     */
    p_g->c2021y = all_coeff[ 966];            /*    u**20 v**21     */
    p_g->c2121y = all_coeff[ 967];            /*    u**21 v**21     */


    p_g->c0000z = all_coeff[ 968];            /*    u** 0 v** 0     */
    p_g->c0100z = all_coeff[ 969];            /*    u** 1 v** 0     */
    p_g->c0200z = all_coeff[ 970];            /*    u** 2 v** 0     */
    p_g->c0300z = all_coeff[ 971];            /*    u** 3 v** 0     */
    p_g->c0400z = all_coeff[ 972];            /*    u** 4 v** 0     */
    p_g->c0500z = all_coeff[ 973];            /*    u** 5 v** 0     */
    p_g->c0600z = all_coeff[ 974];            /*    u** 6 v** 0     */
    p_g->c0700z = all_coeff[ 975];            /*    u** 7 v** 0     */
    p_g->c0800z = all_coeff[ 976];            /*    u** 8 v** 0     */
    p_g->c0900z = all_coeff[ 977];            /*    u** 9 v** 0     */
    p_g->c1000z = all_coeff[ 978];            /*    u**10 v** 0     */
    p_g->c1100z = all_coeff[ 979];            /*    u**11 v** 0     */
    p_g->c1200z = all_coeff[ 980];            /*    u**12 v** 0     */
    p_g->c1300z = all_coeff[ 981];            /*    u**13 v** 0     */
    p_g->c1400z = all_coeff[ 982];            /*    u**14 v** 0     */
    p_g->c1500z = all_coeff[ 983];            /*    u**15 v** 0     */
    p_g->c1600z = all_coeff[ 984];            /*    u**16 v** 0     */
    p_g->c1700z = all_coeff[ 985];            /*    u**17 v** 0     */
    p_g->c1800z = all_coeff[ 986];            /*    u**18 v** 0     */
    p_g->c1900z = all_coeff[ 987];            /*    u**19 v** 0     */
    p_g->c2000z = all_coeff[ 988];            /*    u**20 v** 0     */
    p_g->c2100z = all_coeff[ 989];            /*    u**21 v** 0     */

    p_g->c0001z = all_coeff[ 990];            /*    u** 0 v** 1     */
    p_g->c0101z = all_coeff[ 991];            /*    u** 1 v** 1     */
    p_g->c0201z = all_coeff[ 992];            /*    u** 2 v** 1     */
    p_g->c0301z = all_coeff[ 993];            /*    u** 3 v** 1     */
    p_g->c0401z = all_coeff[ 994];            /*    u** 4 v** 1     */
    p_g->c0501z = all_coeff[ 995];            /*    u** 5 v** 1     */
    p_g->c0601z = all_coeff[ 996];            /*    u** 6 v** 1     */
    p_g->c0701z = all_coeff[ 997];            /*    u** 7 v** 1     */
    p_g->c0801z = all_coeff[ 998];            /*    u** 8 v** 1     */
    p_g->c0901z = all_coeff[ 999];            /*    u** 9 v** 1     */
    p_g->c1001z = all_coeff[1000];            /*    u**10 v** 1     */
    p_g->c1101z = all_coeff[1001];            /*    u**11 v** 1     */
    p_g->c1201z = all_coeff[1002];            /*    u**12 v** 1     */
    p_g->c1301z = all_coeff[1003];            /*    u**13 v** 1     */
    p_g->c1401z = all_coeff[1004];            /*    u**14 v** 1     */
    p_g->c1501z = all_coeff[1005];            /*    u**15 v** 1     */
    p_g->c1601z = all_coeff[1006];            /*    u**16 v** 1     */
    p_g->c1701z = all_coeff[1007];            /*    u**17 v** 1     */
    p_g->c1801z = all_coeff[1008];            /*    u**18 v** 1     */
    p_g->c1901z = all_coeff[1009];            /*    u**19 v** 1     */
    p_g->c2001z = all_coeff[1010];            /*    u**20 v** 1     */
    p_g->c2101z = all_coeff[1011];            /*    u**21 v** 1     */

    p_g->c0002z = all_coeff[1012];            /*    u** 0 v** 2     */
    p_g->c0102z = all_coeff[1013];            /*    u** 1 v** 2     */
    p_g->c0202z = all_coeff[1014];            /*    u** 2 v** 2     */
    p_g->c0302z = all_coeff[1015];            /*    u** 3 v** 2     */
    p_g->c0402z = all_coeff[1016];            /*    u** 4 v** 2     */
    p_g->c0502z = all_coeff[1017];            /*    u** 5 v** 2     */
    p_g->c0602z = all_coeff[1018];            /*    u** 6 v** 2     */
    p_g->c0702z = all_coeff[1019];            /*    u** 7 v** 2     */
    p_g->c0802z = all_coeff[1020];            /*    u** 8 v** 2     */
    p_g->c0902z = all_coeff[1021];            /*    u** 9 v** 2     */
    p_g->c1002z = all_coeff[1022];            /*    u**10 v** 2     */
    p_g->c1102z = all_coeff[1023];            /*    u**11 v** 2     */
    p_g->c1202z = all_coeff[1024];            /*    u**12 v** 2     */
    p_g->c1302z = all_coeff[1025];            /*    u**13 v** 2     */
    p_g->c1402z = all_coeff[1026];            /*    u**14 v** 2     */
    p_g->c1502z = all_coeff[1027];            /*    u**15 v** 2     */
    p_g->c1602z = all_coeff[1028];            /*    u**16 v** 2     */
    p_g->c1702z = all_coeff[1029];            /*    u**17 v** 2     */
    p_g->c1802z = all_coeff[1030];            /*    u**18 v** 2     */
    p_g->c1902z = all_coeff[1031];            /*    u**19 v** 2     */
    p_g->c2002z = all_coeff[1032];            /*    u**20 v** 2     */
    p_g->c2102z = all_coeff[1033];            /*    u**21 v** 2     */

    p_g->c0003z = all_coeff[1034];            /*    u** 0 v** 3     */
    p_g->c0103z = all_coeff[1035];            /*    u** 1 v** 3     */
    p_g->c0203z = all_coeff[1036];            /*    u** 2 v** 3     */
    p_g->c0303z = all_coeff[1037];            /*    u** 3 v** 3     */
    p_g->c0403z = all_coeff[1038];            /*    u** 4 v** 3     */
    p_g->c0503z = all_coeff[1039];            /*    u** 5 v** 3     */
    p_g->c0603z = all_coeff[1040];            /*    u** 6 v** 3     */
    p_g->c0703z = all_coeff[1041];            /*    u** 7 v** 3     */
    p_g->c0803z = all_coeff[1042];            /*    u** 8 v** 3     */
    p_g->c0903z = all_coeff[1043];            /*    u** 9 v** 3     */
    p_g->c1003z = all_coeff[1044];            /*    u**10 v** 3     */
    p_g->c1103z = all_coeff[1045];            /*    u**11 v** 3     */
    p_g->c1203z = all_coeff[1046];            /*    u**12 v** 3     */
    p_g->c1303z = all_coeff[1047];            /*    u**13 v** 3     */
    p_g->c1403z = all_coeff[1048];            /*    u**14 v** 3     */
    p_g->c1503z = all_coeff[1049];            /*    u**15 v** 3     */
    p_g->c1603z = all_coeff[1050];            /*    u**16 v** 3     */
    p_g->c1703z = all_coeff[1051];            /*    u**17 v** 3     */
    p_g->c1803z = all_coeff[1052];            /*    u**18 v** 3     */
    p_g->c1903z = all_coeff[1053];            /*    u**19 v** 3     */
    p_g->c2003z = all_coeff[1054];            /*    u**20 v** 3     */
    p_g->c2103z = all_coeff[1055];            /*    u**21 v** 3     */

    p_g->c0004z = all_coeff[1056];            /*    u** 0 v** 4     */
    p_g->c0104z = all_coeff[1057];            /*    u** 1 v** 4     */
    p_g->c0204z = all_coeff[1058];            /*    u** 2 v** 4     */
    p_g->c0304z = all_coeff[1059];            /*    u** 3 v** 4     */
    p_g->c0404z = all_coeff[1060];            /*    u** 4 v** 4     */
    p_g->c0504z = all_coeff[1061];            /*    u** 5 v** 4     */
    p_g->c0604z = all_coeff[1062];            /*    u** 6 v** 4     */
    p_g->c0704z = all_coeff[1063];            /*    u** 7 v** 4     */
    p_g->c0804z = all_coeff[1064];            /*    u** 8 v** 4     */
    p_g->c0904z = all_coeff[1065];            /*    u** 9 v** 4     */
    p_g->c1004z = all_coeff[1066];            /*    u**10 v** 4     */
    p_g->c1104z = all_coeff[1067];            /*    u**11 v** 4     */
    p_g->c1204z = all_coeff[1068];            /*    u**12 v** 4     */
    p_g->c1304z = all_coeff[1069];            /*    u**13 v** 4     */
    p_g->c1404z = all_coeff[1070];            /*    u**14 v** 4     */
    p_g->c1504z = all_coeff[1071];            /*    u**15 v** 4     */
    p_g->c1604z = all_coeff[1072];            /*    u**16 v** 4     */
    p_g->c1704z = all_coeff[1073];            /*    u**17 v** 4     */
    p_g->c1804z = all_coeff[1074];            /*    u**18 v** 4     */
    p_g->c1904z = all_coeff[1075];            /*    u**19 v** 4     */
    p_g->c2004z = all_coeff[1076];            /*    u**20 v** 4     */
    p_g->c2104z = all_coeff[1077];            /*    u**21 v** 4     */

    p_g->c0005z = all_coeff[1078];            /*    u** 0 v** 5     */
    p_g->c0105z = all_coeff[1079];            /*    u** 1 v** 5     */
    p_g->c0205z = all_coeff[1080];            /*    u** 2 v** 5     */
    p_g->c0305z = all_coeff[1081];            /*    u** 3 v** 5     */
    p_g->c0405z = all_coeff[1082];            /*    u** 4 v** 5     */
    p_g->c0505z = all_coeff[1083];            /*    u** 5 v** 5     */
    p_g->c0605z = all_coeff[1084];            /*    u** 6 v** 5     */
    p_g->c0705z = all_coeff[1085];            /*    u** 7 v** 5     */
    p_g->c0805z = all_coeff[1086];            /*    u** 8 v** 5     */
    p_g->c0905z = all_coeff[1087];            /*    u** 9 v** 5     */
    p_g->c1005z = all_coeff[1088];            /*    u**10 v** 5     */
    p_g->c1105z = all_coeff[1089];            /*    u**11 v** 5     */
    p_g->c1205z = all_coeff[1090];            /*    u**12 v** 5     */
    p_g->c1305z = all_coeff[1091];            /*    u**13 v** 5     */
    p_g->c1405z = all_coeff[1092];            /*    u**14 v** 5     */
    p_g->c1505z = all_coeff[1093];            /*    u**15 v** 5     */
    p_g->c1605z = all_coeff[1094];            /*    u**16 v** 5     */
    p_g->c1705z = all_coeff[1095];            /*    u**17 v** 5     */
    p_g->c1805z = all_coeff[1096];            /*    u**18 v** 5     */
    p_g->c1905z = all_coeff[1097];            /*    u**19 v** 5     */
    p_g->c2005z = all_coeff[1098];            /*    u**20 v** 5     */
    p_g->c2105z = all_coeff[1099];            /*    u**21 v** 5     */

    p_g->c0006z = all_coeff[1100];            /*    u** 0 v** 6     */
    p_g->c0106z = all_coeff[1101];            /*    u** 1 v** 6     */
    p_g->c0206z = all_coeff[1102];            /*    u** 2 v** 6     */
    p_g->c0306z = all_coeff[1103];            /*    u** 3 v** 6     */
    p_g->c0406z = all_coeff[1104];            /*    u** 4 v** 6     */
    p_g->c0506z = all_coeff[1105];            /*    u** 5 v** 6     */
    p_g->c0606z = all_coeff[1106];            /*    u** 6 v** 6     */
    p_g->c0706z = all_coeff[1107];            /*    u** 7 v** 6     */
    p_g->c0806z = all_coeff[1108];            /*    u** 8 v** 6     */
    p_g->c0906z = all_coeff[1109];            /*    u** 9 v** 6     */
    p_g->c1006z = all_coeff[1110];            /*    u**10 v** 6     */
    p_g->c1106z = all_coeff[1111];            /*    u**11 v** 6     */
    p_g->c1206z = all_coeff[1112];            /*    u**12 v** 6     */
    p_g->c1306z = all_coeff[1113];            /*    u**13 v** 6     */
    p_g->c1406z = all_coeff[1114];            /*    u**14 v** 6     */
    p_g->c1506z = all_coeff[1115];            /*    u**15 v** 6     */
    p_g->c1606z = all_coeff[1116];            /*    u**16 v** 6     */
    p_g->c1706z = all_coeff[1117];            /*    u**17 v** 6     */
    p_g->c1806z = all_coeff[1118];            /*    u**18 v** 6     */
    p_g->c1906z = all_coeff[1119];            /*    u**19 v** 6     */
    p_g->c2006z = all_coeff[1120];            /*    u**20 v** 6     */
    p_g->c2106z = all_coeff[1121];            /*    u**21 v** 6     */

    p_g->c0007z = all_coeff[1122];            /*    u** 0 v** 7     */
    p_g->c0107z = all_coeff[1123];            /*    u** 1 v** 7     */
    p_g->c0207z = all_coeff[1124];            /*    u** 2 v** 7     */
    p_g->c0307z = all_coeff[1125];            /*    u** 3 v** 7     */
    p_g->c0407z = all_coeff[1126];            /*    u** 4 v** 7     */
    p_g->c0507z = all_coeff[1127];            /*    u** 5 v** 7     */
    p_g->c0607z = all_coeff[1128];            /*    u** 6 v** 7     */
    p_g->c0707z = all_coeff[1129];            /*    u** 7 v** 7     */
    p_g->c0807z = all_coeff[1130];            /*    u** 8 v** 7     */
    p_g->c0907z = all_coeff[1131];            /*    u** 9 v** 7     */
    p_g->c1007z = all_coeff[1132];            /*    u**10 v** 7     */
    p_g->c1107z = all_coeff[1133];            /*    u**11 v** 7     */
    p_g->c1207z = all_coeff[1134];            /*    u**12 v** 7     */
    p_g->c1307z = all_coeff[1135];            /*    u**13 v** 7     */
    p_g->c1407z = all_coeff[1136];            /*    u**14 v** 7     */
    p_g->c1507z = all_coeff[1137];            /*    u**15 v** 7     */
    p_g->c1607z = all_coeff[1138];            /*    u**16 v** 7     */
    p_g->c1707z = all_coeff[1139];            /*    u**17 v** 7     */
    p_g->c1807z = all_coeff[1140];            /*    u**18 v** 7     */
    p_g->c1907z = all_coeff[1141];            /*    u**19 v** 7     */
    p_g->c2007z = all_coeff[1142];            /*    u**20 v** 7     */
    p_g->c2107z = all_coeff[1143];            /*    u**21 v** 7     */

    p_g->c0008z = all_coeff[1144];            /*    u** 0 v** 8     */
    p_g->c0108z = all_coeff[1145];            /*    u** 1 v** 8     */
    p_g->c0208z = all_coeff[1146];            /*    u** 2 v** 8     */
    p_g->c0308z = all_coeff[1147];            /*    u** 3 v** 8     */
    p_g->c0408z = all_coeff[1148];            /*    u** 4 v** 8     */
    p_g->c0508z = all_coeff[1149];            /*    u** 5 v** 8     */
    p_g->c0608z = all_coeff[1150];            /*    u** 6 v** 8     */
    p_g->c0708z = all_coeff[1151];            /*    u** 7 v** 8     */
    p_g->c0808z = all_coeff[1152];            /*    u** 8 v** 8     */
    p_g->c0908z = all_coeff[1153];            /*    u** 9 v** 8     */
    p_g->c1008z = all_coeff[1154];            /*    u**10 v** 8     */
    p_g->c1108z = all_coeff[1155];            /*    u**11 v** 8     */
    p_g->c1208z = all_coeff[1156];            /*    u**12 v** 8     */
    p_g->c1308z = all_coeff[1157];            /*    u**13 v** 8     */
    p_g->c1408z = all_coeff[1158];            /*    u**14 v** 8     */
    p_g->c1508z = all_coeff[1159];            /*    u**15 v** 8     */
    p_g->c1608z = all_coeff[1160];            /*    u**16 v** 8     */
    p_g->c1708z = all_coeff[1161];            /*    u**17 v** 8     */
    p_g->c1808z = all_coeff[1162];            /*    u**18 v** 8     */
    p_g->c1908z = all_coeff[1163];            /*    u**19 v** 8     */
    p_g->c2008z = all_coeff[1164];            /*    u**20 v** 8     */
    p_g->c2108z = all_coeff[1165];            /*    u**21 v** 8     */

    p_g->c0009z = all_coeff[1166];            /*    u** 0 v** 9     */
    p_g->c0109z = all_coeff[1167];            /*    u** 1 v** 9     */
    p_g->c0209z = all_coeff[1168];            /*    u** 2 v** 9     */
    p_g->c0309z = all_coeff[1169];            /*    u** 3 v** 9     */
    p_g->c0409z = all_coeff[1170];            /*    u** 4 v** 9     */
    p_g->c0509z = all_coeff[1171];            /*    u** 5 v** 9     */
    p_g->c0609z = all_coeff[1172];            /*    u** 6 v** 9     */
    p_g->c0709z = all_coeff[1173];            /*    u** 7 v** 9     */
    p_g->c0809z = all_coeff[1174];            /*    u** 8 v** 9     */
    p_g->c0909z = all_coeff[1175];            /*    u** 9 v** 9     */
    p_g->c1009z = all_coeff[1176];            /*    u**10 v** 9     */
    p_g->c1109z = all_coeff[1177];            /*    u**11 v** 9     */
    p_g->c1209z = all_coeff[1178];            /*    u**12 v** 9     */
    p_g->c1309z = all_coeff[1179];            /*    u**13 v** 9     */
    p_g->c1409z = all_coeff[1180];            /*    u**14 v** 9     */
    p_g->c1509z = all_coeff[1181];            /*    u**15 v** 9     */
    p_g->c1609z = all_coeff[1182];            /*    u**16 v** 9     */
    p_g->c1709z = all_coeff[1183];            /*    u**17 v** 9     */
    p_g->c1809z = all_coeff[1184];            /*    u**18 v** 9     */
    p_g->c1909z = all_coeff[1185];            /*    u**19 v** 9     */
    p_g->c2009z = all_coeff[1186];            /*    u**20 v** 9     */
    p_g->c2109z = all_coeff[1187];            /*    u**21 v** 9     */

    p_g->c0010z = all_coeff[1188];            /*    u** 0 v**10     */
    p_g->c0110z = all_coeff[1189];            /*    u** 1 v**10     */
    p_g->c0210z = all_coeff[1190];            /*    u** 2 v**10     */
    p_g->c0310z = all_coeff[1191];            /*    u** 3 v**10     */
    p_g->c0410z = all_coeff[1192];            /*    u** 4 v**10     */
    p_g->c0510z = all_coeff[1193];            /*    u** 5 v**10     */
    p_g->c0610z = all_coeff[1194];            /*    u** 6 v**10     */
    p_g->c0710z = all_coeff[1195];            /*    u** 7 v**10     */
    p_g->c0810z = all_coeff[1196];            /*    u** 8 v**10     */
    p_g->c0910z = all_coeff[1197];            /*    u** 9 v**10     */
    p_g->c1010z = all_coeff[1198];            /*    u**10 v**10     */
    p_g->c1110z = all_coeff[1199];            /*    u**11 v**10     */
    p_g->c1210z = all_coeff[1200];            /*    u**12 v**10     */
    p_g->c1310z = all_coeff[1201];            /*    u**13 v**10     */
    p_g->c1410z = all_coeff[1202];            /*    u**14 v**10     */
    p_g->c1510z = all_coeff[1203];            /*    u**15 v**10     */
    p_g->c1610z = all_coeff[1204];            /*    u**16 v**10     */
    p_g->c1710z = all_coeff[1205];            /*    u**17 v**10     */
    p_g->c1810z = all_coeff[1206];            /*    u**18 v**10     */
    p_g->c1910z = all_coeff[1207];            /*    u**19 v**10     */
    p_g->c2010z = all_coeff[1208];            /*    u**20 v**10     */
    p_g->c2110z = all_coeff[1209];            /*    u**21 v**10     */

    p_g->c0011z = all_coeff[1210];            /*    u** 0 v**11     */
    p_g->c0111z = all_coeff[1211];            /*    u** 1 v**11     */
    p_g->c0211z = all_coeff[1212];            /*    u** 2 v**11     */
    p_g->c0311z = all_coeff[1213];            /*    u** 3 v**11     */
    p_g->c0411z = all_coeff[1214];            /*    u** 4 v**11     */
    p_g->c0511z = all_coeff[1215];            /*    u** 5 v**11     */
    p_g->c0611z = all_coeff[1216];            /*    u** 6 v**11     */
    p_g->c0711z = all_coeff[1217];            /*    u** 7 v**11     */
    p_g->c0811z = all_coeff[1218];            /*    u** 8 v**11     */
    p_g->c0911z = all_coeff[1219];            /*    u** 9 v**11     */
    p_g->c1011z = all_coeff[1220];            /*    u**10 v**11     */
    p_g->c1111z = all_coeff[1221];            /*    u**11 v**11     */
    p_g->c1211z = all_coeff[1222];            /*    u**12 v**11     */
    p_g->c1311z = all_coeff[1223];            /*    u**13 v**11     */
    p_g->c1411z = all_coeff[1224];            /*    u**14 v**11     */
    p_g->c1511z = all_coeff[1225];            /*    u**15 v**11     */
    p_g->c1611z = all_coeff[1226];            /*    u**16 v**11     */
    p_g->c1711z = all_coeff[1227];            /*    u**17 v**11     */
    p_g->c1811z = all_coeff[1228];            /*    u**18 v**11     */
    p_g->c1911z = all_coeff[1229];            /*    u**19 v**11     */
    p_g->c2011z = all_coeff[1230];            /*    u**20 v**11     */
    p_g->c2111z = all_coeff[1231];            /*    u**21 v**11     */

    p_g->c0012z = all_coeff[1232];            /*    u** 0 v**12     */
    p_g->c0112z = all_coeff[1233];            /*    u** 1 v**12     */
    p_g->c0212z = all_coeff[1234];            /*    u** 2 v**12     */
    p_g->c0312z = all_coeff[1235];            /*    u** 3 v**12     */
    p_g->c0412z = all_coeff[1236];            /*    u** 4 v**12     */
    p_g->c0512z = all_coeff[1237];            /*    u** 5 v**12     */
    p_g->c0612z = all_coeff[1238];            /*    u** 6 v**12     */
    p_g->c0712z = all_coeff[1239];            /*    u** 7 v**12     */
    p_g->c0812z = all_coeff[1240];            /*    u** 8 v**12     */
    p_g->c0912z = all_coeff[1241];            /*    u** 9 v**12     */
    p_g->c1012z = all_coeff[1242];            /*    u**10 v**12     */
    p_g->c1112z = all_coeff[1243];            /*    u**11 v**12     */
    p_g->c1212z = all_coeff[1244];            /*    u**12 v**12     */
    p_g->c1312z = all_coeff[1245];            /*    u**13 v**12     */
    p_g->c1412z = all_coeff[1246];            /*    u**14 v**12     */
    p_g->c1512z = all_coeff[1247];            /*    u**15 v**12     */
    p_g->c1612z = all_coeff[1248];            /*    u**16 v**12     */
    p_g->c1712z = all_coeff[1249];            /*    u**17 v**12     */
    p_g->c1812z = all_coeff[1250];            /*    u**18 v**12     */
    p_g->c1912z = all_coeff[1251];            /*    u**19 v**12     */
    p_g->c2012z = all_coeff[1252];            /*    u**20 v**12     */
    p_g->c2112z = all_coeff[1253];            /*    u**21 v**12     */

    p_g->c0013z = all_coeff[1254];            /*    u** 0 v**13     */
    p_g->c0113z = all_coeff[1255];            /*    u** 1 v**13     */
    p_g->c0213z = all_coeff[1256];            /*    u** 2 v**13     */
    p_g->c0313z = all_coeff[1257];            /*    u** 3 v**13     */
    p_g->c0413z = all_coeff[1258];            /*    u** 4 v**13     */
    p_g->c0513z = all_coeff[1259];            /*    u** 5 v**13     */
    p_g->c0613z = all_coeff[1260];            /*    u** 6 v**13     */
    p_g->c0713z = all_coeff[1261];            /*    u** 7 v**13     */
    p_g->c0813z = all_coeff[1262];            /*    u** 8 v**13     */
    p_g->c0913z = all_coeff[1263];            /*    u** 9 v**13     */
    p_g->c1013z = all_coeff[1264];            /*    u**10 v**13     */
    p_g->c1113z = all_coeff[1265];            /*    u**11 v**13     */
    p_g->c1213z = all_coeff[1266];            /*    u**12 v**13     */
    p_g->c1313z = all_coeff[1267];            /*    u**13 v**13     */
    p_g->c1413z = all_coeff[1268];            /*    u**14 v**13     */
    p_g->c1513z = all_coeff[1269];            /*    u**15 v**13     */
    p_g->c1613z = all_coeff[1270];            /*    u**16 v**13     */
    p_g->c1713z = all_coeff[1271];            /*    u**17 v**13     */
    p_g->c1813z = all_coeff[1272];            /*    u**18 v**13     */
    p_g->c1913z = all_coeff[1273];            /*    u**19 v**13     */
    p_g->c2013z = all_coeff[1274];            /*    u**20 v**13     */
    p_g->c2113z = all_coeff[1275];            /*    u**21 v**13     */

    p_g->c0014z = all_coeff[1276];            /*    u** 0 v**14     */
    p_g->c0114z = all_coeff[1277];            /*    u** 1 v**14     */
    p_g->c0214z = all_coeff[1278];            /*    u** 2 v**14     */
    p_g->c0314z = all_coeff[1279];            /*    u** 3 v**14     */
    p_g->c0414z = all_coeff[1280];            /*    u** 4 v**14     */
    p_g->c0514z = all_coeff[1281];            /*    u** 5 v**14     */
    p_g->c0614z = all_coeff[1282];            /*    u** 6 v**14     */
    p_g->c0714z = all_coeff[1283];            /*    u** 7 v**14     */
    p_g->c0814z = all_coeff[1284];            /*    u** 8 v**14     */
    p_g->c0914z = all_coeff[1285];            /*    u** 9 v**14     */
    p_g->c1014z = all_coeff[1286];            /*    u**10 v**14     */
    p_g->c1114z = all_coeff[1287];            /*    u**11 v**14     */
    p_g->c1214z = all_coeff[1288];            /*    u**12 v**14     */
    p_g->c1314z = all_coeff[1289];            /*    u**13 v**14     */
    p_g->c1414z = all_coeff[1290];            /*    u**14 v**14     */
    p_g->c1514z = all_coeff[1291];            /*    u**15 v**14     */
    p_g->c1614z = all_coeff[1292];            /*    u**16 v**14     */
    p_g->c1714z = all_coeff[1293];            /*    u**17 v**14     */
    p_g->c1814z = all_coeff[1294];            /*    u**18 v**14     */
    p_g->c1914z = all_coeff[1295];            /*    u**19 v**14     */
    p_g->c2014z = all_coeff[1296];            /*    u**20 v**14     */
    p_g->c2114z = all_coeff[1297];            /*    u**21 v**14     */

    p_g->c0015z = all_coeff[1298];            /*    u** 0 v**15     */
    p_g->c0115z = all_coeff[1299];            /*    u** 1 v**15     */
    p_g->c0215z = all_coeff[1300];            /*    u** 2 v**15     */
    p_g->c0315z = all_coeff[1301];            /*    u** 3 v**15     */
    p_g->c0415z = all_coeff[1302];            /*    u** 4 v**15     */
    p_g->c0515z = all_coeff[1303];            /*    u** 5 v**15     */
    p_g->c0615z = all_coeff[1304];            /*    u** 6 v**15     */
    p_g->c0715z = all_coeff[1305];            /*    u** 7 v**15     */
    p_g->c0815z = all_coeff[1306];            /*    u** 8 v**15     */
    p_g->c0915z = all_coeff[1307];            /*    u** 9 v**15     */
    p_g->c1015z = all_coeff[1308];            /*    u**10 v**15     */
    p_g->c1115z = all_coeff[1309];            /*    u**11 v**15     */
    p_g->c1215z = all_coeff[1310];            /*    u**12 v**15     */
    p_g->c1315z = all_coeff[1311];            /*    u**13 v**15     */
    p_g->c1415z = all_coeff[1312];            /*    u**14 v**15     */
    p_g->c1515z = all_coeff[1313];            /*    u**15 v**15     */
    p_g->c1615z = all_coeff[1314];            /*    u**16 v**15     */
    p_g->c1715z = all_coeff[1315];            /*    u**17 v**15     */
    p_g->c1815z = all_coeff[1316];            /*    u**18 v**15     */
    p_g->c1915z = all_coeff[1317];            /*    u**19 v**15     */
    p_g->c2015z = all_coeff[1318];            /*    u**20 v**15     */
    p_g->c2115z = all_coeff[1319];            /*    u**21 v**15     */

    p_g->c0016z = all_coeff[1320];            /*    u** 0 v**16     */
    p_g->c0116z = all_coeff[1321];            /*    u** 1 v**16     */
    p_g->c0216z = all_coeff[1322];            /*    u** 2 v**16     */
    p_g->c0316z = all_coeff[1323];            /*    u** 3 v**16     */
    p_g->c0416z = all_coeff[1324];            /*    u** 4 v**16     */
    p_g->c0516z = all_coeff[1325];            /*    u** 5 v**16     */
    p_g->c0616z = all_coeff[1326];            /*    u** 6 v**16     */
    p_g->c0716z = all_coeff[1327];            /*    u** 7 v**16     */
    p_g->c0816z = all_coeff[1328];            /*    u** 8 v**16     */
    p_g->c0916z = all_coeff[1329];            /*    u** 9 v**16     */
    p_g->c1016z = all_coeff[1330];            /*    u**10 v**16     */
    p_g->c1116z = all_coeff[1331];            /*    u**11 v**16     */
    p_g->c1216z = all_coeff[1332];            /*    u**12 v**16     */
    p_g->c1316z = all_coeff[1333];            /*    u**13 v**16     */
    p_g->c1416z = all_coeff[1334];            /*    u**14 v**16     */
    p_g->c1516z = all_coeff[1335];            /*    u**15 v**16     */
    p_g->c1616z = all_coeff[1336];            /*    u**16 v**16     */
    p_g->c1716z = all_coeff[1337];            /*    u**17 v**16     */
    p_g->c1816z = all_coeff[1338];            /*    u**18 v**16     */
    p_g->c1916z = all_coeff[1339];            /*    u**19 v**16     */
    p_g->c2016z = all_coeff[1340];            /*    u**20 v**16     */
    p_g->c2116z = all_coeff[1341];            /*    u**21 v**16     */

    p_g->c0017z = all_coeff[1342];            /*    u** 0 v**17     */
    p_g->c0117z = all_coeff[1343];            /*    u** 1 v**17     */
    p_g->c0217z = all_coeff[1344];            /*    u** 2 v**17     */
    p_g->c0317z = all_coeff[1345];            /*    u** 3 v**17     */
    p_g->c0417z = all_coeff[1346];            /*    u** 4 v**17     */
    p_g->c0517z = all_coeff[1347];            /*    u** 5 v**17     */
    p_g->c0617z = all_coeff[1348];            /*    u** 6 v**17     */
    p_g->c0717z = all_coeff[1349];            /*    u** 7 v**17     */
    p_g->c0817z = all_coeff[1350];            /*    u** 8 v**17     */
    p_g->c0917z = all_coeff[1351];            /*    u** 9 v**17     */
    p_g->c1017z = all_coeff[1352];            /*    u**10 v**17     */
    p_g->c1117z = all_coeff[1353];            /*    u**11 v**17     */
    p_g->c1217z = all_coeff[1354];            /*    u**12 v**17     */
    p_g->c1317z = all_coeff[1355];            /*    u**13 v**17     */
    p_g->c1417z = all_coeff[1356];            /*    u**14 v**17     */
    p_g->c1517z = all_coeff[1357];            /*    u**15 v**17     */
    p_g->c1617z = all_coeff[1358];            /*    u**16 v**17     */
    p_g->c1717z = all_coeff[1359];            /*    u**17 v**17     */
    p_g->c1817z = all_coeff[1360];            /*    u**18 v**17     */
    p_g->c1917z = all_coeff[1361];            /*    u**19 v**17     */
    p_g->c2017z = all_coeff[1362];            /*    u**20 v**17     */
    p_g->c2117z = all_coeff[1363];            /*    u**21 v**17     */

    p_g->c0018z = all_coeff[1364];            /*    u** 0 v**18     */
    p_g->c0118z = all_coeff[1365];            /*    u** 1 v**18     */
    p_g->c0218z = all_coeff[1366];            /*    u** 2 v**18     */
    p_g->c0318z = all_coeff[1367];            /*    u** 3 v**18     */
    p_g->c0418z = all_coeff[1368];            /*    u** 4 v**18     */
    p_g->c0518z = all_coeff[1369];            /*    u** 5 v**18     */
    p_g->c0618z = all_coeff[1370];            /*    u** 6 v**18     */
    p_g->c0718z = all_coeff[1371];            /*    u** 7 v**18     */
    p_g->c0818z = all_coeff[1372];            /*    u** 8 v**18     */
    p_g->c0918z = all_coeff[1373];            /*    u** 9 v**18     */
    p_g->c1018z = all_coeff[1374];            /*    u**10 v**18     */
    p_g->c1118z = all_coeff[1375];            /*    u**11 v**18     */
    p_g->c1218z = all_coeff[1376];            /*    u**12 v**18     */
    p_g->c1318z = all_coeff[1377];            /*    u**13 v**18     */
    p_g->c1418z = all_coeff[1378];            /*    u**14 v**18     */
    p_g->c1518z = all_coeff[1379];            /*    u**15 v**18     */
    p_g->c1618z = all_coeff[1380];            /*    u**16 v**18     */
    p_g->c1718z = all_coeff[1381];            /*    u**17 v**18     */
    p_g->c1818z = all_coeff[1382];            /*    u**18 v**18     */
    p_g->c1918z = all_coeff[1383];            /*    u**19 v**18     */
    p_g->c2018z = all_coeff[1384];            /*    u**20 v**18     */
    p_g->c2118z = all_coeff[1385];            /*    u**21 v**18     */

    p_g->c0019z = all_coeff[1386];            /*    u** 0 v**19     */
    p_g->c0119z = all_coeff[1387];            /*    u** 1 v**19     */
    p_g->c0219z = all_coeff[1388];            /*    u** 2 v**19     */
    p_g->c0319z = all_coeff[1389];            /*    u** 3 v**19     */
    p_g->c0419z = all_coeff[1390];            /*    u** 4 v**19     */
    p_g->c0519z = all_coeff[1391];            /*    u** 5 v**19     */
    p_g->c0619z = all_coeff[1392];            /*    u** 6 v**19     */
    p_g->c0719z = all_coeff[1393];            /*    u** 7 v**19     */
    p_g->c0819z = all_coeff[1394];            /*    u** 8 v**19     */
    p_g->c0919z = all_coeff[1395];            /*    u** 9 v**19     */
    p_g->c1019z = all_coeff[1396];            /*    u**10 v**19     */
    p_g->c1119z = all_coeff[1397];            /*    u**11 v**19     */
    p_g->c1219z = all_coeff[1398];            /*    u**12 v**19     */
    p_g->c1319z = all_coeff[1399];            /*    u**13 v**19     */
    p_g->c1419z = all_coeff[1400];            /*    u**14 v**19     */
    p_g->c1519z = all_coeff[1401];            /*    u**15 v**19     */
    p_g->c1619z = all_coeff[1402];            /*    u**16 v**19     */
    p_g->c1719z = all_coeff[1403];            /*    u**17 v**19     */
    p_g->c1819z = all_coeff[1404];            /*    u**18 v**19     */
    p_g->c1919z = all_coeff[1405];            /*    u**19 v**19     */
    p_g->c2019z = all_coeff[1406];            /*    u**20 v**19     */
    p_g->c2119z = all_coeff[1407];            /*    u**21 v**19     */

    p_g->c0020z = all_coeff[1408];            /*    u** 0 v**20     */
    p_g->c0120z = all_coeff[1409];            /*    u** 1 v**20     */
    p_g->c0220z = all_coeff[1410];            /*    u** 2 v**20     */
    p_g->c0320z = all_coeff[1411];            /*    u** 3 v**20     */
    p_g->c0420z = all_coeff[1412];            /*    u** 4 v**20     */
    p_g->c0520z = all_coeff[1413];            /*    u** 5 v**20     */
    p_g->c0620z = all_coeff[1414];            /*    u** 6 v**20     */
    p_g->c0720z = all_coeff[1415];            /*    u** 7 v**20     */
    p_g->c0820z = all_coeff[1416];            /*    u** 8 v**20     */
    p_g->c0920z = all_coeff[1417];            /*    u** 9 v**20     */
    p_g->c1020z = all_coeff[1418];            /*    u**10 v**20     */
    p_g->c1120z = all_coeff[1419];            /*    u**11 v**20     */
    p_g->c1220z = all_coeff[1420];            /*    u**12 v**20     */
    p_g->c1320z = all_coeff[1421];            /*    u**13 v**20     */
    p_g->c1420z = all_coeff[1422];            /*    u**14 v**20     */
    p_g->c1520z = all_coeff[1423];            /*    u**15 v**20     */
    p_g->c1620z = all_coeff[1424];            /*    u**16 v**20     */
    p_g->c1720z = all_coeff[1425];            /*    u**17 v**20     */
    p_g->c1820z = all_coeff[1426];            /*    u**18 v**20     */
    p_g->c1920z = all_coeff[1427];            /*    u**19 v**20     */
    p_g->c2020z = all_coeff[1428];            /*    u**20 v**20     */
    p_g->c2120z = all_coeff[1429];            /*    u**21 v**20     */

    p_g->c0021z = all_coeff[1430];            /*    u** 0 v**21     */
    p_g->c0121z = all_coeff[1431];            /*    u** 1 v**21     */
    p_g->c0221z = all_coeff[1432];            /*    u** 2 v**21     */
    p_g->c0321z = all_coeff[1433];            /*    u** 3 v**21     */
    p_g->c0421z = all_coeff[1434];            /*    u** 4 v**21     */
    p_g->c0521z = all_coeff[1435];            /*    u** 5 v**21     */
    p_g->c0621z = all_coeff[1436];            /*    u** 6 v**21     */
    p_g->c0721z = all_coeff[1437];            /*    u** 7 v**21     */
    p_g->c0821z = all_coeff[1438];            /*    u** 8 v**21     */
    p_g->c0921z = all_coeff[1439];            /*    u** 9 v**21     */
    p_g->c1021z = all_coeff[1440];            /*    u**10 v**21     */
    p_g->c1121z = all_coeff[1441];            /*    u**11 v**21     */
    p_g->c1221z = all_coeff[1442];            /*    u**12 v**21     */
    p_g->c1321z = all_coeff[1443];            /*    u**13 v**21     */
    p_g->c1421z = all_coeff[1444];            /*    u**14 v**21     */
    p_g->c1521z = all_coeff[1445];            /*    u**15 v**21     */
    p_g->c1621z = all_coeff[1446];            /*    u**16 v**21     */
    p_g->c1721z = all_coeff[1447];            /*    u**17 v**21     */
    p_g->c1821z = all_coeff[1448];            /*    u**18 v**21     */
    p_g->c1921z = all_coeff[1449];            /*    u**19 v**21     */
    p_g->c2021z = all_coeff[1450];            /*    u**20 v**21     */
    p_g->c2121z = all_coeff[1451];            /*    u**21 v**21     */






    p_g->ofs_pat = 0.0;                  /* Offset for patch        */




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur559 Exit*varkon_vda_rp21  * i_l %d n_coeff %d n_retrieved %d\n", 
                  (short)i_l, (short)n_coeff, (short)n_retrieved );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
