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
/*  Function: varkon_pat_trap7                     File: sur512.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a GMPATP7   patch with the input transformation      */
/*  matrix (the input coordinate system).                           */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-29   Originally written                                 */
/*  1996-12-05   ifdef UNIX and WIN32, temporary fix for Windows 95 */
/*  1997-02-07   ifdef  SAAB                                        */
/*  1997-04-19   Bug transformation                                 */
/*  1998-02-07   pragma Switch off optimizer in compiler            */
/*  1998-03-23   pragma line and SAAB erased                        */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_trap7      Transformation of GMPATP7        */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_trap7  (sur512)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_trap7 (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATP7   *p_i,        /* Pointer to the input  patch             */
  DBTmat    *p_c,        /* Local coordinate system                 */
  GMPATP7   *p_o )       /* Pointer to the output patch             */

/* Out:                                                             */
/*        Coefficients of the transformed patch                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATP7    p_t;       /* Transformed (temporary used) patch      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/* 1998-03-22 #ifdef  SAAB  */

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_pat_trap7  (sur512) ********\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )                       /* Check that the system   */
 {                                       /* p_c is defined          */
 sprintf(errbuf,                         /* Error SU2993 for p_c    */
 "(p_c)%%varkon_pat_trap7  (sur512");    /* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }

/*!                                                                 */
/* 2. Transform the coefficients for the patch                      */
/* ___________________________________________                      */
/*                                                                 !*/


    p_t.ofs_pat = p_i->ofs_pat;          /* Offset for patch        */

    p_t.c0000x =                                   /*    c0000x     */
    p_i->c0000x*p_c->g11 +p_i->c0000y*p_c->g12 +
    p_i->c0000z*p_c->g13 +  1.0      *p_c->g14;
    p_t.c0001x =                                   /*    c0001x     */
    p_i->c0001x*p_c->g11 +p_i->c0001y*p_c->g12 +
    p_i->c0001z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0002x =                                   /*    c0002x     */
    p_i->c0002x*p_c->g11 +p_i->c0002y*p_c->g12 +
    p_i->c0002z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0003x =                                   /*    c0003x     */
    p_i->c0003x*p_c->g11 +p_i->c0003y*p_c->g12 +
    p_i->c0003z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0004x =                                   /*    c0004x     */
    p_i->c0004x*p_c->g11 +p_i->c0004y*p_c->g12 +
    p_i->c0004z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0005x =                                   /*    c0005x     */
    p_i->c0005x*p_c->g11 +p_i->c0005y*p_c->g12 +
    p_i->c0005z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0006x =                                   /*    c0006x     */
    p_i->c0006x*p_c->g11 +p_i->c0006y*p_c->g12 +
    p_i->c0006z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0007x =                                   /*    c0007x     */
    p_i->c0007x*p_c->g11 +p_i->c0007y*p_c->g12 +
    p_i->c0007z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0100x =                                   /*    c0100x     */
    p_i->c0100x*p_c->g11 +p_i->c0100y*p_c->g12 +
    p_i->c0100z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0101x =                                   /*    c0101x     */
    p_i->c0101x*p_c->g11 +p_i->c0101y*p_c->g12 +
    p_i->c0101z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0102x =                                   /*    c0102x     */
    p_i->c0102x*p_c->g11 +p_i->c0102y*p_c->g12 +
    p_i->c0102z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0103x =                                   /*    c0103x     */
    p_i->c0103x*p_c->g11 +p_i->c0103y*p_c->g12 +
    p_i->c0103z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0104x =                                   /*    c0104x     */
    p_i->c0104x*p_c->g11 +p_i->c0104y*p_c->g12 +
    p_i->c0104z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0105x =                                   /*    c0105x     */
    p_i->c0105x*p_c->g11 +p_i->c0105y*p_c->g12 +
    p_i->c0105z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0106x =                                   /*    c0106x     */
    p_i->c0106x*p_c->g11 +p_i->c0106y*p_c->g12 +
    p_i->c0106z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0107x =                                   /*    c0107x     */
    p_i->c0107x*p_c->g11 +p_i->c0107y*p_c->g12 +
    p_i->c0107z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0200x =                                   /*    c0200x     */
    p_i->c0200x*p_c->g11 +p_i->c0200y*p_c->g12 +
    p_i->c0200z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0201x =                                   /*    c0201x     */
    p_i->c0201x*p_c->g11 +p_i->c0201y*p_c->g12 +
    p_i->c0201z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0202x =                                   /*    c0202x     */
    p_i->c0202x*p_c->g11 +p_i->c0202y*p_c->g12 +
    p_i->c0202z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0203x =                                   /*    c0203x     */
    p_i->c0203x*p_c->g11 +p_i->c0203y*p_c->g12 +
    p_i->c0203z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0204x =                                   /*    c0204x     */
    p_i->c0204x*p_c->g11 +p_i->c0204y*p_c->g12 +
    p_i->c0204z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0205x =                                   /*    c0205x     */
    p_i->c0205x*p_c->g11 +p_i->c0205y*p_c->g12 +
    p_i->c0205z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0206x =                                   /*    c0206x     */
    p_i->c0206x*p_c->g11 +p_i->c0206y*p_c->g12 +
    p_i->c0206z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0207x =                                   /*    c0207x     */
    p_i->c0207x*p_c->g11 +p_i->c0207y*p_c->g12 +
    p_i->c0207z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0300x =                                   /*    c0300x     */
    p_i->c0300x*p_c->g11 +p_i->c0300y*p_c->g12 +
    p_i->c0300z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0301x =                                   /*    c0301x     */
    p_i->c0301x*p_c->g11 +p_i->c0301y*p_c->g12 +
    p_i->c0301z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0302x =                                   /*    c0302x     */
    p_i->c0302x*p_c->g11 +p_i->c0302y*p_c->g12 +
    p_i->c0302z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0303x =                                   /*    c0303x     */
    p_i->c0303x*p_c->g11 +p_i->c0303y*p_c->g12 +
    p_i->c0303z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0304x =                                   /*    c0304x     */
    p_i->c0304x*p_c->g11 +p_i->c0304y*p_c->g12 +
    p_i->c0304z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0305x =                                   /*    c0305x     */
    p_i->c0305x*p_c->g11 +p_i->c0305y*p_c->g12 +
    p_i->c0305z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0306x =                                   /*    c0306x     */
    p_i->c0306x*p_c->g11 +p_i->c0306y*p_c->g12 +
    p_i->c0306z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0307x =                                   /*    c0307x     */
    p_i->c0307x*p_c->g11 +p_i->c0307y*p_c->g12 +
    p_i->c0307z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0400x =                                   /*    c0400x     */
    p_i->c0400x*p_c->g11 +p_i->c0400y*p_c->g12 +
    p_i->c0400z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0401x =                                   /*    c0401x     */
    p_i->c0401x*p_c->g11 +p_i->c0401y*p_c->g12 +
    p_i->c0401z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0402x =                                   /*    c0402x     */
    p_i->c0402x*p_c->g11 +p_i->c0402y*p_c->g12 +
    p_i->c0402z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0403x =                                   /*    c0403x     */
    p_i->c0403x*p_c->g11 +p_i->c0403y*p_c->g12 +
    p_i->c0403z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0404x =                                   /*    c0404x     */
    p_i->c0404x*p_c->g11 +p_i->c0404y*p_c->g12 +
    p_i->c0404z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0405x =                                   /*    c0405x     */
    p_i->c0405x*p_c->g11 +p_i->c0405y*p_c->g12 +
    p_i->c0405z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0406x =                                   /*    c0406x     */
    p_i->c0406x*p_c->g11 +p_i->c0406y*p_c->g12 +
    p_i->c0406z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0407x =                                   /*    c0407x     */
    p_i->c0407x*p_c->g11 +p_i->c0407y*p_c->g12 +
    p_i->c0407z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0500x =                                   /*    c0500x     */
    p_i->c0500x*p_c->g11 +p_i->c0500y*p_c->g12 +
    p_i->c0500z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0501x =                                   /*    c0501x     */
    p_i->c0501x*p_c->g11 +p_i->c0501y*p_c->g12 +
    p_i->c0501z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0502x =                                   /*    c0502x     */
    p_i->c0502x*p_c->g11 +p_i->c0502y*p_c->g12 +
    p_i->c0502z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0503x =                                   /*    c0503x     */
    p_i->c0503x*p_c->g11 +p_i->c0503y*p_c->g12 +
    p_i->c0503z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0504x =                                   /*    c0504x     */
    p_i->c0504x*p_c->g11 +p_i->c0504y*p_c->g12 +
    p_i->c0504z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0505x =                                   /*    c0505x     */
    p_i->c0505x*p_c->g11 +p_i->c0505y*p_c->g12 +
    p_i->c0505z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0506x =                                   /*    c0506x     */
    p_i->c0506x*p_c->g11 +p_i->c0506y*p_c->g12 +
    p_i->c0506z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0507x =                                   /*    c0507x     */
    p_i->c0507x*p_c->g11 +p_i->c0507y*p_c->g12 +
    p_i->c0507z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0600x =                                   /*    c0600x     */
    p_i->c0600x*p_c->g11 +p_i->c0600y*p_c->g12 +
    p_i->c0600z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0601x =                                   /*    c0601x     */
    p_i->c0601x*p_c->g11 +p_i->c0601y*p_c->g12 +
    p_i->c0601z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0602x =                                   /*    c0602x     */
    p_i->c0602x*p_c->g11 +p_i->c0602y*p_c->g12 +
    p_i->c0602z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0603x =                                   /*    c0603x     */
    p_i->c0603x*p_c->g11 +p_i->c0603y*p_c->g12 +
    p_i->c0603z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0604x =                                   /*    c0604x     */
    p_i->c0604x*p_c->g11 +p_i->c0604y*p_c->g12 +
    p_i->c0604z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0605x =                                   /*    c0605x     */
    p_i->c0605x*p_c->g11 +p_i->c0605y*p_c->g12 +
    p_i->c0605z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0606x =                                   /*    c0606x     */
    p_i->c0606x*p_c->g11 +p_i->c0606y*p_c->g12 +
    p_i->c0606z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0607x =                                   /*    c0607x     */
    p_i->c0607x*p_c->g11 +p_i->c0607y*p_c->g12 +
    p_i->c0607z*p_c->g13 +  0.0      *p_c->g14;

    p_t.c0700x =                                   /*    c0700x     */
    p_i->c0700x*p_c->g11 +p_i->c0700y*p_c->g12 +
    p_i->c0700z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0701x =                                   /*    c0701x     */
    p_i->c0701x*p_c->g11 +p_i->c0701y*p_c->g12 +
    p_i->c0701z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0702x =                                   /*    c0702x     */
    p_i->c0702x*p_c->g11 +p_i->c0702y*p_c->g12 +
    p_i->c0702z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0703x =                                   /*    c0703x     */
    p_i->c0703x*p_c->g11 +p_i->c0703y*p_c->g12 +
    p_i->c0703z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0704x =                                   /*    c0704x     */
    p_i->c0704x*p_c->g11 +p_i->c0704y*p_c->g12 +
    p_i->c0704z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0705x =                                   /*    c0705x     */
    p_i->c0705x*p_c->g11 +p_i->c0705y*p_c->g12 +
    p_i->c0705z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0706x =                                   /*    c0706x     */
    p_i->c0706x*p_c->g11 +p_i->c0706y*p_c->g12 +
    p_i->c0706z*p_c->g13 +  0.0      *p_c->g14;
    p_t.c0707x =                                   /*    c0707x     */
    p_i->c0707x*p_c->g11 +p_i->c0707y*p_c->g12 +
    p_i->c0707z*p_c->g13 +  0.0      *p_c->g14;


    p_t.c0000y =                                   /*    c0000y     */
    p_i->c0000x*p_c->g21 +p_i->c0000y*p_c->g22 +
    p_i->c0000z*p_c->g23 +  1.0      *p_c->g24;
    p_t.c0001y =                                   /*    c0001y     */
    p_i->c0001x*p_c->g21 +p_i->c0001y*p_c->g22 +
    p_i->c0001z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0002y =                                   /*    c0002y     */
    p_i->c0002x*p_c->g21 +p_i->c0002y*p_c->g22 +
    p_i->c0002z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0003y =                                   /*    c0003y     */
    p_i->c0003x*p_c->g21 +p_i->c0003y*p_c->g22 +
    p_i->c0003z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0004y =                                   /*    c0004y     */
    p_i->c0004x*p_c->g21 +p_i->c0004y*p_c->g22 +
    p_i->c0004z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0005y =                                   /*    c0005y     */
    p_i->c0005x*p_c->g21 +p_i->c0005y*p_c->g22 +
    p_i->c0005z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0006y =                                   /*    c0006y     */
    p_i->c0006x*p_c->g21 +p_i->c0006y*p_c->g22 +
    p_i->c0006z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0007y =                                   /*    c0007y     */
    p_i->c0007x*p_c->g21 +p_i->c0007y*p_c->g22 +
    p_i->c0007z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0100y =                                   /*    c0100y     */
    p_i->c0100x*p_c->g21 +p_i->c0100y*p_c->g22 +
    p_i->c0100z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0101y =                                   /*    c0101y     */
    p_i->c0101x*p_c->g21 +p_i->c0101y*p_c->g22 +
    p_i->c0101z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0102y =                                   /*    c0102y     */
    p_i->c0102x*p_c->g21 +p_i->c0102y*p_c->g22 +
    p_i->c0102z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0103y =                                   /*    c0103y     */
    p_i->c0103x*p_c->g21 +p_i->c0103y*p_c->g22 +
    p_i->c0103z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0104y =                                   /*    c0104y     */
    p_i->c0104x*p_c->g21 +p_i->c0104y*p_c->g22 +
    p_i->c0104z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0105y =                                   /*    c0105y     */
    p_i->c0105x*p_c->g21 +p_i->c0105y*p_c->g22 +
    p_i->c0105z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0106y =                                   /*    c0106y     */
    p_i->c0106x*p_c->g21 +p_i->c0106y*p_c->g22 +
    p_i->c0106z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0107y =                                   /*    c0107y     */
    p_i->c0107x*p_c->g21 +p_i->c0107y*p_c->g22 +
    p_i->c0107z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0200y =                                   /*    c0200y     */
    p_i->c0200x*p_c->g21 +p_i->c0200y*p_c->g22 +
    p_i->c0200z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0201y =                                   /*    c0201y     */
    p_i->c0201x*p_c->g21 +p_i->c0201y*p_c->g22 +
    p_i->c0201z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0202y =                                   /*    c0202y     */
    p_i->c0202x*p_c->g21 +p_i->c0202y*p_c->g22 +
    p_i->c0202z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0203y =                                   /*    c0203y     */
    p_i->c0203x*p_c->g21 +p_i->c0203y*p_c->g22 +
    p_i->c0203z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0204y =                                   /*    c0204y     */
    p_i->c0204x*p_c->g21 +p_i->c0204y*p_c->g22 +
    p_i->c0204z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0205y =                                   /*    c0205y     */
    p_i->c0205x*p_c->g21 +p_i->c0205y*p_c->g22 +
    p_i->c0205z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0206y =                                   /*    c0206y     */
    p_i->c0206x*p_c->g21 +p_i->c0206y*p_c->g22 +
    p_i->c0206z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0207y =                                   /*    c0207y     */
    p_i->c0207x*p_c->g21 +p_i->c0207y*p_c->g22 +
    p_i->c0207z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0300y =                                   /*    c0300y     */
    p_i->c0300x*p_c->g21 +p_i->c0300y*p_c->g22 +
    p_i->c0300z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0301y =                                   /*    c0301y     */
    p_i->c0301x*p_c->g21 +p_i->c0301y*p_c->g22 +
    p_i->c0301z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0302y =                                   /*    c0302y     */
    p_i->c0302x*p_c->g21 +p_i->c0302y*p_c->g22 +
    p_i->c0302z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0303y =                                   /*    c0303y     */
    p_i->c0303x*p_c->g21 +p_i->c0303y*p_c->g22 +
    p_i->c0303z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0304y =                                   /*    c0304y     */
    p_i->c0304x*p_c->g21 +p_i->c0304y*p_c->g22 +
    p_i->c0304z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0305y =                                   /*    c0305y     */
    p_i->c0305x*p_c->g21 +p_i->c0305y*p_c->g22 +
    p_i->c0305z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0306y =                                   /*    c0306y     */
    p_i->c0306x*p_c->g21 +p_i->c0306y*p_c->g22 +
    p_i->c0306z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0307y =                                   /*    c0307y     */
    p_i->c0307x*p_c->g21 +p_i->c0307y*p_c->g22 +
    p_i->c0307z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0400y =                                   /*    c0400y     */
    p_i->c0400x*p_c->g21 +p_i->c0400y*p_c->g22 +
    p_i->c0400z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0401y =                                   /*    c0401y     */
    p_i->c0401x*p_c->g21 +p_i->c0401y*p_c->g22 +
    p_i->c0401z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0402y =                                   /*    c0402y     */
    p_i->c0402x*p_c->g21 +p_i->c0402y*p_c->g22 +
    p_i->c0402z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0403y =                                   /*    c0403y     */
    p_i->c0403x*p_c->g21 +p_i->c0403y*p_c->g22 +
    p_i->c0403z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0404y =                                   /*    c0404y     */
    p_i->c0404x*p_c->g21 +p_i->c0404y*p_c->g22 +
    p_i->c0404z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0405y =                                   /*    c0405y     */
    p_i->c0405x*p_c->g21 +p_i->c0405y*p_c->g22 +
    p_i->c0405z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0406y =                                   /*    c0406y     */
    p_i->c0406x*p_c->g21 +p_i->c0406y*p_c->g22 +
    p_i->c0406z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0407y =                                   /*    c0407y     */
    p_i->c0407x*p_c->g21 +p_i->c0407y*p_c->g22 +
    p_i->c0407z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0500y =                                   /*    c0500y     */
    p_i->c0500x*p_c->g21 +p_i->c0500y*p_c->g22 +
    p_i->c0500z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0501y =                                   /*    c0501y     */
    p_i->c0501x*p_c->g21 +p_i->c0501y*p_c->g22 +
    p_i->c0501z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0502y =                                   /*    c0502y     */
    p_i->c0502x*p_c->g21 +p_i->c0502y*p_c->g22 +
    p_i->c0502z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0503y =                                   /*    c0503y     */
    p_i->c0503x*p_c->g21 +p_i->c0503y*p_c->g22 +
    p_i->c0503z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0504y =                                   /*    c0504y     */
    p_i->c0504x*p_c->g21 +p_i->c0504y*p_c->g22 +
    p_i->c0504z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0505y =                                   /*    c0505y     */
    p_i->c0505x*p_c->g21 +p_i->c0505y*p_c->g22 +
    p_i->c0505z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0506y =                                   /*    c0506y     */
    p_i->c0506x*p_c->g21 +p_i->c0506y*p_c->g22 +
    p_i->c0506z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0507y =                                   /*    c0507y     */
    p_i->c0507x*p_c->g21 +p_i->c0507y*p_c->g22 +
    p_i->c0507z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0600y =                                   /*    c0600y     */
    p_i->c0600x*p_c->g21 +p_i->c0600y*p_c->g22 +
    p_i->c0600z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0601y =                                   /*    c0601y     */
    p_i->c0601x*p_c->g21 +p_i->c0601y*p_c->g22 +
    p_i->c0601z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0602y =                                   /*    c0602y     */
    p_i->c0602x*p_c->g21 +p_i->c0602y*p_c->g22 +
    p_i->c0602z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0603y =                                   /*    c0603y     */
    p_i->c0603x*p_c->g21 +p_i->c0603y*p_c->g22 +
    p_i->c0603z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0604y =                                   /*    c0604y     */
    p_i->c0604x*p_c->g21 +p_i->c0604y*p_c->g22 +
    p_i->c0604z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0605y =                                   /*    c0605y     */
    p_i->c0605x*p_c->g21 +p_i->c0605y*p_c->g22 +
    p_i->c0605z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0606y =                                   /*    c0606y     */
    p_i->c0606x*p_c->g21 +p_i->c0606y*p_c->g22 +
    p_i->c0606z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0607y =                                   /*    c0607y     */
    p_i->c0607x*p_c->g21 +p_i->c0607y*p_c->g22 +
    p_i->c0607z*p_c->g23 +  0.0      *p_c->g24;

    p_t.c0700y =                                   /*    c0700y     */
    p_i->c0700x*p_c->g21 +p_i->c0700y*p_c->g22 +
    p_i->c0700z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0701y =                                   /*    c0701y     */
    p_i->c0701x*p_c->g21 +p_i->c0701y*p_c->g22 +
    p_i->c0701z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0702y =                                   /*    c0702y     */
    p_i->c0702x*p_c->g21 +p_i->c0702y*p_c->g22 +
    p_i->c0702z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0703y =                                   /*    c0703y     */
    p_i->c0703x*p_c->g21 +p_i->c0703y*p_c->g22 +
    p_i->c0703z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0704y =                                   /*    c0704y     */
    p_i->c0704x*p_c->g21 +p_i->c0704y*p_c->g22 +
    p_i->c0704z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0705y =                                   /*    c0705y     */
    p_i->c0705x*p_c->g21 +p_i->c0705y*p_c->g22 +
    p_i->c0705z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0706y =                                   /*    c0706y     */
    p_i->c0706x*p_c->g21 +p_i->c0706y*p_c->g22 +
    p_i->c0706z*p_c->g23 +  0.0      *p_c->g24;
    p_t.c0707y =                                   /*    c0707y     */
    p_i->c0707x*p_c->g21 +p_i->c0707y*p_c->g22 +
    p_i->c0707z*p_c->g23 +  0.0      *p_c->g24;


    p_t.c0000z =                                   /*    c0000z     */
    p_i->c0000x*p_c->g31 +p_i->c0000y*p_c->g32 +
    p_i->c0000z*p_c->g33 +  1.0      *p_c->g34;
    p_t.c0001z =                                   /*    c0001z     */
    p_i->c0001x*p_c->g31 +p_i->c0001y*p_c->g32 +
    p_i->c0001z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0002z =                                   /*    c0002z     */
    p_i->c0002x*p_c->g31 +p_i->c0002y*p_c->g32 +
    p_i->c0002z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0003z =                                   /*    c0003z     */
    p_i->c0003x*p_c->g31 +p_i->c0003y*p_c->g32 +
    p_i->c0003z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0004z =                                   /*    c0004z     */
    p_i->c0004x*p_c->g31 +p_i->c0004y*p_c->g32 +
    p_i->c0004z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0005z =                                   /*    c0005z     */
    p_i->c0005x*p_c->g31 +p_i->c0005y*p_c->g32 +
    p_i->c0005z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0006z =                                   /*    c0006z     */
    p_i->c0006x*p_c->g31 +p_i->c0006y*p_c->g32 +
    p_i->c0006z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0007z =                                   /*    c0007z     */
    p_i->c0007x*p_c->g31 +p_i->c0007y*p_c->g32 +
    p_i->c0007z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0100z =                                   /*    c0100z     */
    p_i->c0100x*p_c->g31 +p_i->c0100y*p_c->g32 +
    p_i->c0100z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0101z =                                   /*    c0101z     */
    p_i->c0101x*p_c->g31 +p_i->c0101y*p_c->g32 +
    p_i->c0101z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0102z =                                   /*    c0102z     */
    p_i->c0102x*p_c->g31 +p_i->c0102y*p_c->g32 +
    p_i->c0102z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0103z =                                   /*    c0103z     */
    p_i->c0103x*p_c->g31 +p_i->c0103y*p_c->g32 +
    p_i->c0103z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0104z =                                   /*    c0104z     */
    p_i->c0104x*p_c->g31 +p_i->c0104y*p_c->g32 +
    p_i->c0104z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0105z =                                   /*    c0105z     */
    p_i->c0105x*p_c->g31 +p_i->c0105y*p_c->g32 +
    p_i->c0105z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0106z =                                   /*    c0106z     */
    p_i->c0106x*p_c->g31 +p_i->c0106y*p_c->g32 +
    p_i->c0106z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0107z =                                   /*    c0107z     */
    p_i->c0107x*p_c->g31 +p_i->c0107y*p_c->g32 +
    p_i->c0107z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0200z =                                   /*    c0200z     */
    p_i->c0200x*p_c->g31 +p_i->c0200y*p_c->g32 +
    p_i->c0200z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0201z =                                   /*    c0201z     */
    p_i->c0201x*p_c->g31 +p_i->c0201y*p_c->g32 +
    p_i->c0201z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0202z =                                   /*    c0202z     */
    p_i->c0202x*p_c->g31 +p_i->c0202y*p_c->g32 +
    p_i->c0202z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0203z =                                   /*    c0203z     */
    p_i->c0203x*p_c->g31 +p_i->c0203y*p_c->g32 +
    p_i->c0203z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0204z =                                   /*    c0204z     */
    p_i->c0204x*p_c->g31 +p_i->c0204y*p_c->g32 +
    p_i->c0204z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0205z =                                   /*    c0205z     */
    p_i->c0205x*p_c->g31 +p_i->c0205y*p_c->g32 +
    p_i->c0205z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0206z =                                   /*    c0206z     */
    p_i->c0206x*p_c->g31 +p_i->c0206y*p_c->g32 +
    p_i->c0206z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0207z =                                   /*    c0207z     */
    p_i->c0207x*p_c->g31 +p_i->c0207y*p_c->g32 +
    p_i->c0207z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0300z =                                   /*    c0300z     */
    p_i->c0300x*p_c->g31 +p_i->c0300y*p_c->g32 +
    p_i->c0300z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0301z =                                   /*    c0301z     */
    p_i->c0301x*p_c->g31 +p_i->c0301y*p_c->g32 +
    p_i->c0301z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0302z =                                   /*    c0302z     */
    p_i->c0302x*p_c->g31 +p_i->c0302y*p_c->g32 +
    p_i->c0302z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0303z =                                   /*    c0303z     */
    p_i->c0303x*p_c->g31 +p_i->c0303y*p_c->g32 +
    p_i->c0303z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0304z =                                   /*    c0304z     */
    p_i->c0304x*p_c->g31 +p_i->c0304y*p_c->g32 +
    p_i->c0304z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0305z =                                   /*    c0305z     */
    p_i->c0305x*p_c->g31 +p_i->c0305y*p_c->g32 +
    p_i->c0305z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0306z =                                   /*    c0306z     */
    p_i->c0306x*p_c->g31 +p_i->c0306y*p_c->g32 +
    p_i->c0306z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0307z =                                   /*    c0307z     */
    p_i->c0307x*p_c->g31 +p_i->c0307y*p_c->g32 +
    p_i->c0307z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0400z =                                   /*    c0400z     */
    p_i->c0400x*p_c->g31 +p_i->c0400y*p_c->g32 +
    p_i->c0400z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0401z =                                   /*    c0401z     */
    p_i->c0401x*p_c->g31 +p_i->c0401y*p_c->g32 +
    p_i->c0401z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0402z =                                   /*    c0402z     */
    p_i->c0402x*p_c->g31 +p_i->c0402y*p_c->g32 +
    p_i->c0402z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0403z =                                   /*    c0403z     */
    p_i->c0403x*p_c->g31 +p_i->c0403y*p_c->g32 +
    p_i->c0403z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0404z =                                   /*    c0404z     */
    p_i->c0404x*p_c->g31 +p_i->c0404y*p_c->g32 +
    p_i->c0404z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0405z =                                   /*    c0405z     */
    p_i->c0405x*p_c->g31 +p_i->c0405y*p_c->g32 +
    p_i->c0405z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0406z =                                   /*    c0406z     */
    p_i->c0406x*p_c->g31 +p_i->c0406y*p_c->g32 +
    p_i->c0406z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0407z =                                   /*    c0407z     */
    p_i->c0407x*p_c->g31 +p_i->c0407y*p_c->g32 +
    p_i->c0407z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0500z =                                   /*    c0500z     */
    p_i->c0500x*p_c->g31 +p_i->c0500y*p_c->g32 +
    p_i->c0500z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0501z =                                   /*    c0501z     */
    p_i->c0501x*p_c->g31 +p_i->c0501y*p_c->g32 +
    p_i->c0501z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0502z =                                   /*    c0502z     */
    p_i->c0502x*p_c->g31 +p_i->c0502y*p_c->g32 +
    p_i->c0502z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0503z =                                   /*    c0503z     */
    p_i->c0503x*p_c->g31 +p_i->c0503y*p_c->g32 +
    p_i->c0503z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0504z =                                   /*    c0504z     */
    p_i->c0504x*p_c->g31 +p_i->c0504y*p_c->g32 +
    p_i->c0504z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0505z =                                   /*    c0505z     */
    p_i->c0505x*p_c->g31 +p_i->c0505y*p_c->g32 +
    p_i->c0505z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0506z =                                   /*    c0506z     */
    p_i->c0506x*p_c->g31 +p_i->c0506y*p_c->g32 +
    p_i->c0506z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0507z =                                   /*    c0507z     */
    p_i->c0507x*p_c->g31 +p_i->c0507y*p_c->g32 +
    p_i->c0507z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0600z =                                   /*    c0600z     */
    p_i->c0600x*p_c->g31 +p_i->c0600y*p_c->g32 +
    p_i->c0600z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0601z =                                   /*    c0601z     */
    p_i->c0601x*p_c->g31 +p_i->c0601y*p_c->g32 +
    p_i->c0601z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0602z =                                   /*    c0602z     */
    p_i->c0602x*p_c->g31 +p_i->c0602y*p_c->g32 +
    p_i->c0602z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0603z =                                   /*    c0603z     */
    p_i->c0603x*p_c->g31 +p_i->c0603y*p_c->g32 +
    p_i->c0603z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0604z =                                   /*    c0604z     */
    p_i->c0604x*p_c->g31 +p_i->c0604y*p_c->g32 +
    p_i->c0604z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0605z =                                   /*    c0605z     */
    p_i->c0605x*p_c->g31 +p_i->c0605y*p_c->g32 +
    p_i->c0605z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0606z =                                   /*    c0606z     */
    p_i->c0606x*p_c->g31 +p_i->c0606y*p_c->g32 +
    p_i->c0606z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0607z =                                   /*    c0607z     */
    p_i->c0607x*p_c->g31 +p_i->c0607y*p_c->g32 +
    p_i->c0607z*p_c->g33 +  0.0      *p_c->g34;

    p_t.c0700z =                                   /*    c0700z     */
    p_i->c0700x*p_c->g31 +p_i->c0700y*p_c->g32 +
    p_i->c0700z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0701z =                                   /*    c0701z     */
    p_i->c0701x*p_c->g31 +p_i->c0701y*p_c->g32 +
    p_i->c0701z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0702z =                                   /*    c0702z     */
    p_i->c0702x*p_c->g31 +p_i->c0702y*p_c->g32 +
    p_i->c0702z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0703z =                                   /*    c0703z     */
    p_i->c0703x*p_c->g31 +p_i->c0703y*p_c->g32 +
    p_i->c0703z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0704z =                                   /*    c0704z     */
    p_i->c0704x*p_c->g31 +p_i->c0704y*p_c->g32 +
    p_i->c0704z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0705z =                                   /*    c0705z     */
    p_i->c0705x*p_c->g31 +p_i->c0705y*p_c->g32 +
    p_i->c0705z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0706z =                                   /*    c0706z     */
    p_i->c0706x*p_c->g31 +p_i->c0706y*p_c->g32 +
    p_i->c0706z*p_c->g33 +  0.0      *p_c->g34;
    p_t.c0707z =                                   /*    c0707z     */
    p_i->c0707x*p_c->g31 +p_i->c0707y*p_c->g32 +
    p_i->c0707z*p_c->g33 +  0.0      *p_c->g34;


    p_o->c0000x = p_t.c0000x;
    p_o->c0001x = p_t.c0001x;
    p_o->c0002x = p_t.c0002x;
    p_o->c0003x = p_t.c0003x;
    p_o->c0004x = p_t.c0004x;
    p_o->c0005x = p_t.c0005x;
    p_o->c0006x = p_t.c0006x;
    p_o->c0007x = p_t.c0007x;

    p_o->c0100x = p_t.c0100x;
    p_o->c0101x = p_t.c0101x;
    p_o->c0102x = p_t.c0102x;
    p_o->c0103x = p_t.c0103x;
    p_o->c0104x = p_t.c0104x;
    p_o->c0105x = p_t.c0105x;
    p_o->c0106x = p_t.c0106x;
    p_o->c0107x = p_t.c0107x;

    p_o->c0200x = p_t.c0200x;
    p_o->c0201x = p_t.c0201x;
    p_o->c0202x = p_t.c0202x;
    p_o->c0203x = p_t.c0203x;
    p_o->c0204x = p_t.c0204x;
    p_o->c0205x = p_t.c0205x;
    p_o->c0206x = p_t.c0206x;
    p_o->c0207x = p_t.c0207x;

    p_o->c0300x = p_t.c0300x;
    p_o->c0301x = p_t.c0301x;
    p_o->c0302x = p_t.c0302x;
    p_o->c0303x = p_t.c0303x;
    p_o->c0304x = p_t.c0304x;
    p_o->c0305x = p_t.c0305x;
    p_o->c0306x = p_t.c0306x;
    p_o->c0307x = p_t.c0307x;

    p_o->c0400x = p_t.c0400x;
    p_o->c0401x = p_t.c0401x;
    p_o->c0402x = p_t.c0402x;
    p_o->c0403x = p_t.c0403x;
    p_o->c0404x = p_t.c0404x;
    p_o->c0405x = p_t.c0405x;
    p_o->c0406x = p_t.c0406x;
    p_o->c0407x = p_t.c0407x;

    p_o->c0500x = p_t.c0500x;
    p_o->c0501x = p_t.c0501x;
    p_o->c0502x = p_t.c0502x;
    p_o->c0503x = p_t.c0503x;
    p_o->c0504x = p_t.c0504x;
    p_o->c0505x = p_t.c0505x;
    p_o->c0506x = p_t.c0506x;
    p_o->c0507x = p_t.c0507x;

    p_o->c0600x = p_t.c0600x;
    p_o->c0601x = p_t.c0601x;
    p_o->c0602x = p_t.c0602x;
    p_o->c0603x = p_t.c0603x;
    p_o->c0604x = p_t.c0604x;
    p_o->c0605x = p_t.c0605x;
    p_o->c0606x = p_t.c0606x;
    p_o->c0607x = p_t.c0607x;

    p_o->c0700x = p_t.c0700x;
    p_o->c0701x = p_t.c0701x;
    p_o->c0702x = p_t.c0702x;
    p_o->c0703x = p_t.c0703x;
    p_o->c0704x = p_t.c0704x;
    p_o->c0705x = p_t.c0705x;
    p_o->c0706x = p_t.c0706x;
    p_o->c0707x = p_t.c0707x;


    p_o->c0000y = p_t.c0000y;
    p_o->c0001y = p_t.c0001y;
    p_o->c0002y = p_t.c0002y;
    p_o->c0003y = p_t.c0003y;
    p_o->c0004y = p_t.c0004y;
    p_o->c0005y = p_t.c0005y;
    p_o->c0006y = p_t.c0006y;
    p_o->c0007y = p_t.c0007y;

    p_o->c0100y = p_t.c0100y;
    p_o->c0101y = p_t.c0101y;
    p_o->c0102y = p_t.c0102y;
    p_o->c0103y = p_t.c0103y;
    p_o->c0104y = p_t.c0104y;
    p_o->c0105y = p_t.c0105y;
    p_o->c0106y = p_t.c0106y;
    p_o->c0107y = p_t.c0107y;

    p_o->c0200y = p_t.c0200y;
    p_o->c0201y = p_t.c0201y;
    p_o->c0202y = p_t.c0202y;
    p_o->c0203y = p_t.c0203y;
    p_o->c0204y = p_t.c0204y;
    p_o->c0205y = p_t.c0205y;
    p_o->c0206y = p_t.c0206y;
    p_o->c0207y = p_t.c0207y;

    p_o->c0300y = p_t.c0300y;
    p_o->c0301y = p_t.c0301y;
    p_o->c0302y = p_t.c0302y;
    p_o->c0303y = p_t.c0303y;
    p_o->c0304y = p_t.c0304y;
    p_o->c0305y = p_t.c0305y;
    p_o->c0306y = p_t.c0306y;
    p_o->c0307y = p_t.c0307y;

    p_o->c0400y = p_t.c0400y;
    p_o->c0401y = p_t.c0401y;
    p_o->c0402y = p_t.c0402y;
    p_o->c0403y = p_t.c0403y;
    p_o->c0404y = p_t.c0404y;
    p_o->c0405y = p_t.c0405y;
    p_o->c0406y = p_t.c0406y;
    p_o->c0407y = p_t.c0407y;

    p_o->c0500y = p_t.c0500y;
    p_o->c0501y = p_t.c0501y;
    p_o->c0502y = p_t.c0502y;
    p_o->c0503y = p_t.c0503y;
    p_o->c0504y = p_t.c0504y;
    p_o->c0505y = p_t.c0505y;
    p_o->c0506y = p_t.c0506y;
    p_o->c0507y = p_t.c0507y;

    p_o->c0600y = p_t.c0600y;
    p_o->c0601y = p_t.c0601y;
    p_o->c0602y = p_t.c0602y;
    p_o->c0603y = p_t.c0603y;
    p_o->c0604y = p_t.c0604y;
    p_o->c0605y = p_t.c0605y;
    p_o->c0606y = p_t.c0606y;
    p_o->c0607y = p_t.c0607y;

    p_o->c0700y = p_t.c0700y;
    p_o->c0701y = p_t.c0701y;
    p_o->c0702y = p_t.c0702y;
    p_o->c0703y = p_t.c0703y;
    p_o->c0704y = p_t.c0704y;
    p_o->c0705y = p_t.c0705y;
    p_o->c0706y = p_t.c0706y;
    p_o->c0707y = p_t.c0707y;


    p_o->c0000z = p_t.c0000z;
    p_o->c0001z = p_t.c0001z;
    p_o->c0002z = p_t.c0002z;
    p_o->c0003z = p_t.c0003z;
    p_o->c0004z = p_t.c0004z;
    p_o->c0005z = p_t.c0005z;
    p_o->c0006z = p_t.c0006z;
    p_o->c0007z = p_t.c0007z;

    p_o->c0100z = p_t.c0100z;
    p_o->c0101z = p_t.c0101z;
    p_o->c0102z = p_t.c0102z;
    p_o->c0103z = p_t.c0103z;
    p_o->c0104z = p_t.c0104z;
    p_o->c0105z = p_t.c0105z;
    p_o->c0106z = p_t.c0106z;
    p_o->c0107z = p_t.c0107z;

    p_o->c0200z = p_t.c0200z;
    p_o->c0201z = p_t.c0201z;
    p_o->c0202z = p_t.c0202z;
    p_o->c0203z = p_t.c0203z;
    p_o->c0204z = p_t.c0204z;
    p_o->c0205z = p_t.c0205z;
    p_o->c0206z = p_t.c0206z;
    p_o->c0207z = p_t.c0207z;

    p_o->c0300z = p_t.c0300z;
    p_o->c0301z = p_t.c0301z;
    p_o->c0302z = p_t.c0302z;
    p_o->c0303z = p_t.c0303z;
    p_o->c0304z = p_t.c0304z;
    p_o->c0305z = p_t.c0305z;
    p_o->c0306z = p_t.c0306z;
    p_o->c0307z = p_t.c0307z;

    p_o->c0400z = p_t.c0400z;
    p_o->c0401z = p_t.c0401z;
    p_o->c0402z = p_t.c0402z;
    p_o->c0403z = p_t.c0403z;
    p_o->c0404z = p_t.c0404z;
    p_o->c0405z = p_t.c0405z;
    p_o->c0406z = p_t.c0406z;
    p_o->c0407z = p_t.c0407z;

    p_o->c0500z = p_t.c0500z;
    p_o->c0501z = p_t.c0501z;
    p_o->c0502z = p_t.c0502z;
    p_o->c0503z = p_t.c0503z;
    p_o->c0504z = p_t.c0504z;
    p_o->c0505z = p_t.c0505z;
    p_o->c0506z = p_t.c0506z;
    p_o->c0507z = p_t.c0507z;

    p_o->c0600z = p_t.c0600z;
    p_o->c0601z = p_t.c0601z;
    p_o->c0602z = p_t.c0602z;
    p_o->c0603z = p_t.c0603z;
    p_o->c0604z = p_t.c0604z;
    p_o->c0605z = p_t.c0605z;
    p_o->c0606z = p_t.c0606z;
    p_o->c0607z = p_t.c0607z;

    p_o->c0700z = p_t.c0700z;
    p_o->c0701z = p_t.c0701z;
    p_o->c0702z = p_t.c0702z;
    p_o->c0703z = p_t.c0703z;
    p_o->c0704z = p_t.c0704z;
    p_o->c0705z = p_t.c0705z;
    p_o->c0706z = p_t.c0706z;
    p_o->c0707z = p_t.c0707z;





    p_o->ofs_pat = p_t.ofs_pat;          /* Offset for patch        */

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur512 Exit ****** varkon_pat_trap7  ****** \n");
}
#endif


/* 1998-03-22 #endif */ /*  SAAB  */

    return(SUCCED);

  }

/*********************************************************/
