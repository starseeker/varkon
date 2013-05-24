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
/*  Function: varkon_pat_trap3                     File: sur510.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a GMPATP3   patch with the input transformation      */
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
/*  1997-04-19   Debug                                              */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_trap3      Transformation of GMPATP3        */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_trap3  (sur510)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_pat_trap3 (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATP3   *p_i,        /* Pointer to the input  patch             */
  DBTmat    *p_c,        /* Local coordinate system                 */
  GMPATP3   *p_o )       /* Pointer to the output patch             */

/* Out:                                                             */
/*        Coefficients of the transformed patch                     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATP3    p_t;       /* Transformed (temporary used) patch      */
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
  "Enter ***** varkon_pat_trap3  (sur510) ********\n");
  }
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 In:  c0000 %f %f %f\n",p_i->c0000x, p_i->c0000y, p_i->c0000z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0001 %f %f %f\n",p_i->c0001x, p_i->c0001y, p_i->c0001z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0002 %f %f %f\n",p_i->c0002x, p_i->c0002y, p_i->c0002z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0003 %f %f %f\n",p_i->c0003x, p_i->c0003y, p_i->c0003z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 In:  c0100 %f %f %f\n",p_i->c0100x, p_i->c0100y, p_i->c0100z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0101 %f %f %f\n",p_i->c0101x, p_i->c0101y, p_i->c0101z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0102 %f %f %f\n",p_i->c0102x, p_i->c0102y, p_i->c0102z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0103 %f %f %f\n",p_i->c0103x, p_i->c0103y, p_i->c0103z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 In:  c0200 %f %f %f\n",p_i->c0200x, p_i->c0200y, p_i->c0200z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0201 %f %f %f\n",p_i->c0201x, p_i->c0201y, p_i->c0201z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0202 %f %f %f\n",p_i->c0202x, p_i->c0202y, p_i->c0202z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0203 %f %f %f\n",p_i->c0203x, p_i->c0203y, p_i->c0203z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 In:  c0300 %f %f %f\n",p_i->c0300x, p_i->c0300y, p_i->c0300z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0301 %f %f %f\n",p_i->c0301x, p_i->c0301y, p_i->c0301z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0302 %f %f %f\n",p_i->c0302x, p_i->c0302y, p_i->c0302z );
fprintf(dbgfil(SURPAC),
"sur510 In:  c0303 %f %f %f\n",p_i->c0303x, p_i->c0303y, p_i->c0103z );
}
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )                       /* Check that the system   */
 {                                       /* p_c is defined          */
 sprintf(errbuf,                         /* Error SU2993 for p_c    */
 "(p_c)%%varkon_pat_trap3  (sur510");    /* failure                 */
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


    p_o->c0000x = p_t.c0000x;
    p_o->c0001x = p_t.c0001x;
    p_o->c0002x = p_t.c0002x;
    p_o->c0003x = p_t.c0003x;

    p_o->c0100x = p_t.c0100x;
    p_o->c0101x = p_t.c0101x;
    p_o->c0102x = p_t.c0102x;
    p_o->c0103x = p_t.c0103x;

    p_o->c0200x = p_t.c0200x;
    p_o->c0201x = p_t.c0201x;
    p_o->c0202x = p_t.c0202x;
    p_o->c0203x = p_t.c0203x;

    p_o->c0300x = p_t.c0300x;
    p_o->c0301x = p_t.c0301x;
    p_o->c0302x = p_t.c0302x;
    p_o->c0303x = p_t.c0303x;


    p_o->c0000y = p_t.c0000y;
    p_o->c0001y = p_t.c0001y;
    p_o->c0002y = p_t.c0002y;
    p_o->c0003y = p_t.c0003y;

    p_o->c0100y = p_t.c0100y;
    p_o->c0101y = p_t.c0101y;
    p_o->c0102y = p_t.c0102y;
    p_o->c0103y = p_t.c0103y;

    p_o->c0200y = p_t.c0200y;
    p_o->c0201y = p_t.c0201y;
    p_o->c0202y = p_t.c0202y;
    p_o->c0203y = p_t.c0203y;

    p_o->c0300y = p_t.c0300y;
    p_o->c0301y = p_t.c0301y;
    p_o->c0302y = p_t.c0302y;
    p_o->c0303y = p_t.c0303y;


    p_o->c0000z = p_t.c0000z;
    p_o->c0001z = p_t.c0001z;
    p_o->c0002z = p_t.c0002z;
    p_o->c0003z = p_t.c0003z;

    p_o->c0100z = p_t.c0100z;
    p_o->c0101z = p_t.c0101z;
    p_o->c0102z = p_t.c0102z;
    p_o->c0103z = p_t.c0103z;

    p_o->c0200z = p_t.c0200z;
    p_o->c0201z = p_t.c0201z;
    p_o->c0202z = p_t.c0202z;
    p_o->c0203z = p_t.c0203z;

    p_o->c0300z = p_t.c0300z;
    p_o->c0301z = p_t.c0301z;
    p_o->c0302z = p_t.c0302z;
    p_o->c0303z = p_t.c0303z;





    p_o->ofs_pat = p_t.ofs_pat;          /* Offset for patch        */

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"sur510 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur510 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur510 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur510 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur510 g41 %f g42 %f g43 %f g44 %f \n",
                  p_c->g41,p_c->g42,p_c->g43,p_c->g44);
 fflush(dbgfil(SURPAC));
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 Out: c0000 %f %f %f\n",p_o->c0000x, p_o->c0000y, p_o->c0000z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0001 %f %f %f\n",p_o->c0001x, p_o->c0001y, p_o->c0001z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0002 %f %f %f\n",p_o->c0002x, p_o->c0002y, p_o->c0002z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0003 %f %f %f\n",p_o->c0003x, p_o->c0003y, p_o->c0003z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 Out: c0100 %f %f %f\n",p_o->c0100x, p_o->c0100y, p_o->c0100z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0101 %f %f %f\n",p_o->c0101x, p_o->c0101y, p_o->c0101z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0102 %f %f %f\n",p_o->c0102x, p_o->c0102y, p_o->c0102z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0103 %f %f %f\n",p_o->c0103x, p_o->c0103y, p_o->c0103z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 Out: c0200 %f %f %f\n",p_o->c0200x, p_o->c0200y, p_o->c0200z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0201 %f %f %f\n",p_o->c0201x, p_o->c0201y, p_o->c0201z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0202 %f %f %f\n",p_o->c0202x, p_o->c0202y, p_o->c0202z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0203 %f %f %f\n",p_o->c0203x, p_o->c0203y, p_o->c0203z );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur510 Out: c0300 %f %f %f\n",p_o->c0300x, p_o->c0300y, p_o->c0300z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0301 %f %f %f\n",p_o->c0301x, p_o->c0301y, p_o->c0301z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0302 %f %f %f\n",p_o->c0302x, p_o->c0302y, p_o->c0302z );
fprintf(dbgfil(SURPAC),
"sur510 Out: c0303 %f %f %f\n",p_o->c0303x, p_o->c0303y, p_o->c0103z );
}




if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur510 Exit ****** varkon_pat_trap3  ****** \n");
}
#endif



    return(SUCCED);

  }

/*********************************************************/
