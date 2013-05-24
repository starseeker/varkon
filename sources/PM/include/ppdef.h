/*!******************************************************************/
/*  File: ppdef.h                                                   */
/*  =============                                                   */
/*                                                                  */
/* Definitions for MBS pretty printing (decompiler)                 */
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
/********************************************************************/

/*
***Type and constants for operator priority.
*/
typedef short ppopri;

#define PP_NOPRI    0  /*  No priority */
#define PP_NOT     10
#define PP_OR      10
#define PP_AND     15
#define PP_RELOP   20
#define PP_ADDOP   25
#define PP_MULOP   30
#define PP_EXP     35

/*
***Indentation.
*/
#define DEFINDL     2  /* Default indentation length */
#define MAXIND     50  /* Maximum indentation */

/*
***Max lengths.
*/
#define PPLINLEN 1000  /* Length of Pretty's line buffer */
#define PPVALSLEN  60  /* Value string length */
#define PPMXLINL   77  /* Max line length for output to MBS-file */

/*
***Define:s for PP:s output modes.
*/
#define PPSTRING    0  /* Write to string */
#define PPIGLIST    1  /* Write to WPLWIN */
#define PPFILE      2  /* Write to file */
