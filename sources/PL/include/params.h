/*!******************************************************************/
/*  File: params.h                                                  */
/*  ==============                                                  */
/*                                                                  */
/*  Global variables for command line options                       */
/*                                                                  */
/*  This file is part of the VARKON Plotter Library.                */
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

/*
***All platforms use these.
*/
double xmin;             /* Parameter -x   */
double ymin;             /* Parameter -y   */
double skala;            /* Parameter -s   */
double wskala;           /* Parameter -ws  */
double vinkel;           /* Parameter -v   */
double curnog;           /* Parameter -k   */
double hast;             /* Parameter -h   */
double formw;            /* Parameter -w   */
short  npins;            /* Parameter -p   */
bool   debug;            /* Parameter -d   */
bool   formf;            /* Parameter -FF  */
bool   bw;               /* Parameter -bw  */
short  nrows;            /* Parameter -l   */
char   pfrnam[V3PTHLEN]; /* Parameter -pf  */

/*
***These are only used for WIN32.
*/
bool   raw;              /* Parameter -raw Raw mode */
bool   del;              /* Parameter -del */
char   plnamn[V3PTHLEN]; /* Parameter -P[printername] */
char   pltfil[V3PTHLEN]; /* File to plot */

/*
***This is no parameter but almost.
*/
char prognam[256];


/********************************************************/
