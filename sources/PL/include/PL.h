/*!******************************************************************/
/*  File: PL.h                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  Main includefile for the Varkon PL library.                     */
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

#include <stdio.h>
#include <math.h>

/*
***Blocksize for bitmaps.
*/
#define BMBSIZ 32000

/*
***Highest pen number for wide lines = 200. Starting
***with zero this makes 201 different widths.
*/
#define WPNMAX 201

/*
***Function prototypes for exported functions.
*/

/*
***All drivers/filters supply the following entrypoints.
*/
short plinpl();
short plexpl();
short plmove(double x, double y);
short pldraw(double x, double y);
short plchpn(short pn);
short plchwd(double width);

/*
***pl1.c
*/
short plppar(int argc, char *argv[]);
short pllpfp(char *fnam);
short pllpfr(char *path);

/*
***pl2.c
*/
short plprpf();
short plpgks(char *inbuf);
short plpdxf(char *inbuf);
char *plgets(char *buf);

/*
***pl3.c
*/
short plrast(short ix1, short iy1, short ix2, short iy2);

/*
***pl4.c
*/
short plbpoi(GMPOI *poipek, short *n, double x[], double y[], char a[]);
short plblin(GMLIN *linpek, short *n, double x[], double y[], char a[]);
short plbarc(GMARC *arcpek, short *n, double x[], double y[], char a[]);
short plbtxt(GMTXT *txtpek, char  *strpek, short *k,
             double x[], double y[], char a[]);

/*
***pl5.c
*/
short pntpoi(GMPOI *poipek);
short pntlin(GMLIN *linpek);
short pntarc(GMARC *arcpek);
short pnttxt(GMTXT *txtpek, char *str);
short pntply(short n, double x[], double y[], char a[]);

/*
***pl6.c
*/
short plpoly(short n, double x[], double y[], char a[]);
short plcwdt(int pen);
short plcnpt(double width);
short plclip(double *v, double *w); 
