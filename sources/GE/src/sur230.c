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
/*  Function: varkon_sur_nopatch                   File: sur230.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieves the number of patches in the U and V direction        */
/*  and the surface type.                                           */
/*                                                                  */
/*  The surface type and no patches are checked for Debug On.       */
/*                                                                  */
/*  Implemented surface types are CUB_SUR, RAT_SUR, LFT_SUR,        */
/*  FAC_SUR, CON_SUR which are surfaces with only one type of       */
/*  patches (CUB_PAT, RAT_PAT, LFT_PAT, FAC_PAT respectively        */
/*  CON_PAT).                                                       */
/*                                                                  */
/*  New surface (patch) types will be PRO_SUR (PRO_PAT) and MIX_SUR,*/
/*  which is a surface with mixed patch types. A mix of all or      */
/*  some of the patch types above.                                  */
/*                                                                  */
/*  There will also be a zero type patch (NUL_PAT), which can       */
/*  be part of all the surface types above (CUB_SUR, RAT_SUR,       */
/*  LFT_SUR, PRO_SUR and MIX_SUR).                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-01   Originally written                                 */
/*  1996-01-06   FAC_SUR added                                      */
/*  1996-02-02   CON_SUR added                                      */
/*  1996-05-11   Debug   added                                      */
/*  1996-05-15   BOX_SUR added                                      */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1997-11-06   NURB_SUR added                                     */
/*  1999-11-24   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nopatch    Retrieve number of patches       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush         Error message to terminal        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_sur_nopatch (sur230) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_nopatch (
/******************************************************************!*/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf *p_sur,        /* Surface                           (ptr) */
   DBint  *p_nu,         /* Number of patches in U direction  (ptr) */
   DBint  *p_nv,         /* Number of patches in V direction  (ptr) */
   DBint  *p_surtype)    /* Type of surface                   (ptr) */

/* Out:                                                             */
/*        Data to p_nu, p_nv and p_surtype                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

#ifdef DEBUG
   char   errbuf[80];    /* String for error message fctn erpush    */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Retrieve surface information                                  */
/* _______________________________                                  */
/*                                                                  */
/* Number of patches from gmsur record (p_sur ->nu_su ->nv_su)      */
/*                                                                  */
/*                                                                 !*/

   *p_nu      = p_sur->nu_su;
   *p_nv      = p_sur->nv_su;

   *p_surtype = -1;    /* Type of surface = not defined           */

/*!                                                                 */
/* Surface type from gmsur record (p_sur->typ_su)                   */
/*                                                                 !*/

   switch ( p_sur->typ_su )
      {
      case CUB_SUR:
      *p_surtype = CUB_SUR;
      break;

      case RAT_SUR:
      *p_surtype = RAT_SUR;
      break;

      case LFT_SUR:
      *p_surtype = LFT_SUR;
      break;

      case FAC_SUR:
      *p_surtype = FAC_SUR;
      break;

      case CON_SUR:
      *p_surtype = CON_SUR;
      break;

      case POL_SUR:
      *p_surtype = POL_SUR;
      break;

      case  P3_SUR:
      *p_surtype =  P3_SUR;
      break;

      case  P5_SUR:
      *p_surtype =  P5_SUR;
      break;

      case  P7_SUR:
      *p_surtype =  P7_SUR;
      break;

      case  P9_SUR:
      *p_surtype =  P9_SUR;
      break;

      case P21_SUR:
      *p_surtype = P21_SUR;
      break;

      case NURB_SUR:
      *p_surtype = NURB_SUR;
      break;

      case BOX_SUR:
      *p_surtype = BOX_SUR;
      break;
      }

/*!                                                                 */
/* Check that surface type exists                   for Debug On    */
/*                                                                 !*/


#ifdef DEBUG
if (  *p_surtype == -1 )
 {
 if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur230 Error*varkon_sur_nopatch ** Failure p_sur->typ_su %d\n",
            p_sur->typ_su);
  fflush(dbgfil(SURPAC)); 
  }
 sprintf(errbuf, "(type)%%varkon_sur_nopatch (sur230");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

/*!                                                                 */
/* Check that no. of patches is greater than zero   for Debug On    */
/*                                                                 !*/

#ifdef DEBUG
if (  *p_nu < 1 || *p_nv < 1 )
 {
if ( dbglev(SURPAC) == 1 )
 {
 fprintf(dbgfil(SURPAC),
  "sur230 Error in varkon_sur_nopatch  nu= %d nv= %d\n",
            (short)*p_nu, (short)*p_nv);
 }
 sprintf(errbuf, "(nu,nv)%%varkon_sur_nopatch (sur230)");
 return(varkon_erpush("SU2993",errbuf)); 
 }
#endif



/* 2. Exit                                                          */

    return(SUCCED);

  }

/*********************************************************/
