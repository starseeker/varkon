/*!******************************************************************/
/*  File: screen.h                                                  */
/*  ==============                                                  */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

/* symbols for t-string referenses */

/* vt100 text fonts */
#define NORMAL 0       /* normal */
#define UNDLIN 4       /* understruket */
#define REVERS 7       /* omvänd video */

/* igfstr - justification Codes */
#define JULEFT 1       /* vänsterjustering */
#define JUMID  2       /* centerjustering */
#define JURIGHT 3      /* högerjustering */

/* ------------ menu area symbols --------------*/
/* t-strings */
#define MSELPR 0          /* menyarea - prompt vid val av alternativ */

/* ------------ input area symbols --------------*/
#define MSMAX  4          /* inputarea - max antal strängar på en gång */

/* ------------ list area symbols --------------*/
/* t-strings */
#define LANXT  2          /* listarea - list continue promt */
#define LARDY  3          /* listarea - list continue promt */

/* ------------ message area symbols --------------*/

/* ------------ general t-strings --------------*/
#define TS_INT   4        /* datatyp - lstpar */
#define TS_FLOAT 5        /* datatyp - lstpar */
#define TS_STR   6        /* datatyp - lstpar */
#define TS_VEC   7        /* datatyp - lstpar */
#define TS_PLHEAD 8       /* headline parameter list */
#define TS_VLHEAD 9       /* headline view list */
#define TS_ISTR  10       /* prompt - inläsning av text-sträng */

/********************************************************************/
