/*!******************************************************************/
/*  File: ppdef.h                                                   */
/*  =============                                                   */
/*                                                                  */
/* Definition of MBS's internal form.                               */
/*                                                                  */
/*  This file includes the definition of the last part of the       */
/*  system global symbol table.                                     */
/*                                                                  */
/*  This file is part of the VARKON Pmpac Library.                  */
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
/*  (C)Microform AB 1984-1999, J. Kjellander johan@microform.se     */
/*                                                                  */
/********************************************************************/
/*         Definitioner för dekompilering.
*********************************************************************/

/*      Type and constants for operator priority            */

typedef short ppopri;

#define PP_NOPRI  0      /*  no priority */

#define PP_NOT   10
#define PP_OR    10

#define PP_AND   15

#define PP_RELOP 20

#define PP_ADDOP 25

#define PP_MULOP 30

#define PP_EXP   35

/*------------------------------------------------------------------------*/

#define MAXIND  50    /* maximum indentation */
#define PPLINLEN 200  /* length of Pretty's line buffer */
#define PPVALSLEN 60  /* value string length */

/*
***CGI:s list-area klarar max 77 tkn. Övriga, vet ej.
*/
#define PPMXLINL 77   /* max line length for output to MBS-file */
/*
***Define:s för PP:s output-moder.
*/
#define PPSTRING 0      /* Skriv till sträng */
#define PPIGLIST 1      /* Skicka till list-arean */
#define PPFILE   2      /* Skicka till fil */

#define DEFINDL  2    /* Default indentation length */

