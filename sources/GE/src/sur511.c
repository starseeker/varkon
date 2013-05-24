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
/*  Function: varkon_pat_trap5                     File: sur511.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a GMPATP5   patch with the input transformation      */
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
/*  1997-02-07   ifdef  SAAB                                        */
/*  1997-04-19   Bug transformation                                 */
/*  1998-02-07   pragma Switch off optimizer in compiler            */
/*  1998-03-23   pragma line and SAAB erased                        */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_trap5      Transformation of GMPATP5        */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_trap5  (sur511)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_pat_trap5 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATP5   *p_i,        /* Pointer to the input  patch             */
  DBTmat    *p_c,        /* Local coordinate system                 */
  GMPATP5   *p_o )       /* Pointer to the output patch             */

/* Out:                                                             */
/*        Coefficients of the transformed patch                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATP5    p_t;       /* Transformed (temporary used) patch      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

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
  "Enter ***** varkon_pat_trap5  (sur511) ********\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )                       /* Check that the system   */
 {                                       /* p_c is defined          */
 sprintf(errbuf,                         /* Error SU2993 for p_c    */
 "(p_c)%%varkon_pat_trap5  (sur511");    /* failure                 */
 return(varkon_erpush("SU2993",errbuf)); /*                         */
 }

/*!                                                                 */
/* 2. Transform the coefficients for the patch                      */
/* ___________________________________________                      */
/*                                                                 !*/


/* 1998-03-22 #ifdef  SAAB  */

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


    p_o->c0000x = p_t.c0000x;
    p_o->c0001x = p_t.c0001x;
    p_o->c0002x = p_t.c0002x;
    p_o->c0003x = p_t.c0003x;
    p_o->c0004x = p_t.c0004x;
    p_o->c0005x = p_t.c0005x;

    p_o->c0100x = p_t.c0100x;
    p_o->c0101x = p_t.c0101x;
    p_o->c0102x = p_t.c0102x;
    p_o->c0103x = p_t.c0103x;
    p_o->c0104x = p_t.c0104x;
    p_o->c0105x = p_t.c0105x;

    p_o->c0200x = p_t.c0200x;
    p_o->c0201x = p_t.c0201x;
    p_o->c0202x = p_t.c0202x;
    p_o->c0203x = p_t.c0203x;
    p_o->c0204x = p_t.c0204x;
    p_o->c0205x = p_t.c0205x;

    p_o->c0300x = p_t.c0300x;
    p_o->c0301x = p_t.c0301x;
    p_o->c0302x = p_t.c0302x;
    p_o->c0303x = p_t.c0303x;
    p_o->c0304x = p_t.c0304x;
    p_o->c0305x = p_t.c0305x;

    p_o->c0400x = p_t.c0400x;
    p_o->c0401x = p_t.c0401x;
    p_o->c0402x = p_t.c0402x;
    p_o->c0403x = p_t.c0403x;
    p_o->c0404x = p_t.c0404x;
    p_o->c0405x = p_t.c0405x;

    p_o->c0500x = p_t.c0500x;
    p_o->c0501x = p_t.c0501x;
    p_o->c0502x = p_t.c0502x;
    p_o->c0503x = p_t.c0503x;
    p_o->c0504x = p_t.c0504x;
    p_o->c0505x = p_t.c0505x;


    p_o->c0000y = p_t.c0000y;
    p_o->c0001y = p_t.c0001y;
    p_o->c0002y = p_t.c0002y;
    p_o->c0003y = p_t.c0003y;
    p_o->c0004y = p_t.c0004y;
    p_o->c0005y = p_t.c0005y;

    p_o->c0100y = p_t.c0100y;
    p_o->c0101y = p_t.c0101y;
    p_o->c0102y = p_t.c0102y;
    p_o->c0103y = p_t.c0103y;
    p_o->c0104y = p_t.c0104y;
    p_o->c0105y = p_t.c0105y;

    p_o->c0200y = p_t.c0200y;
    p_o->c0201y = p_t.c0201y;
    p_o->c0202y = p_t.c0202y;
    p_o->c0203y = p_t.c0203y;
    p_o->c0204y = p_t.c0204y;
    p_o->c0205y = p_t.c0205y;

    p_o->c0300y = p_t.c0300y;
    p_o->c0301y = p_t.c0301y;
    p_o->c0302y = p_t.c0302y;
    p_o->c0303y = p_t.c0303y;
    p_o->c0304y = p_t.c0304y;
    p_o->c0305y = p_t.c0305y;

    p_o->c0400y = p_t.c0400y;
    p_o->c0401y = p_t.c0401y;
    p_o->c0402y = p_t.c0402y;
    p_o->c0403y = p_t.c0403y;
    p_o->c0404y = p_t.c0404y;
    p_o->c0405y = p_t.c0405y;

    p_o->c0500y = p_t.c0500y;
    p_o->c0501y = p_t.c0501y;
    p_o->c0502y = p_t.c0502y;
    p_o->c0503y = p_t.c0503y;
    p_o->c0504y = p_t.c0504y;
    p_o->c0505y = p_t.c0505y;


    p_o->c0000z = p_t.c0000z;
    p_o->c0001z = p_t.c0001z;
    p_o->c0002z = p_t.c0002z;
    p_o->c0003z = p_t.c0003z;
    p_o->c0004z = p_t.c0004z;
    p_o->c0005z = p_t.c0005z;

    p_o->c0100z = p_t.c0100z;
    p_o->c0101z = p_t.c0101z;
    p_o->c0102z = p_t.c0102z;
    p_o->c0103z = p_t.c0103z;
    p_o->c0104z = p_t.c0104z;
    p_o->c0105z = p_t.c0105z;

    p_o->c0200z = p_t.c0200z;
    p_o->c0201z = p_t.c0201z;
    p_o->c0202z = p_t.c0202z;
    p_o->c0203z = p_t.c0203z;
    p_o->c0204z = p_t.c0204z;
    p_o->c0205z = p_t.c0205z;

    p_o->c0300z = p_t.c0300z;
    p_o->c0301z = p_t.c0301z;
    p_o->c0302z = p_t.c0302z;
    p_o->c0303z = p_t.c0303z;
    p_o->c0304z = p_t.c0304z;
    p_o->c0305z = p_t.c0305z;

    p_o->c0400z = p_t.c0400z;
    p_o->c0401z = p_t.c0401z;
    p_o->c0402z = p_t.c0402z;
    p_o->c0403z = p_t.c0403z;
    p_o->c0404z = p_t.c0404z;
    p_o->c0405z = p_t.c0405z;

    p_o->c0500z = p_t.c0500z;
    p_o->c0501z = p_t.c0501z;
    p_o->c0502z = p_t.c0502z;
    p_o->c0503z = p_t.c0503z;
    p_o->c0504z = p_t.c0504z;
    p_o->c0505z = p_t.c0505z;





    p_o->ofs_pat = p_t.ofs_pat;          /* Offset for patch        */

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur511 Exit ****** varkon_pat_trap5  ****** \n");
}
#endif


/* 1998-03-22 #endif */ /*  SAAB  */


    return(SUCCED);

  }

/*********************************************************/
