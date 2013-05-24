/*!******************************************************************/
/*  File: pl1.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plppar();  Parses command line options                         */
/*   pllpfp();  Loads plotter specific penfile                      */
/*   pllpfr();  Loadsr drawing specific penfile                     */
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include <string.h>

extern double xmin;        /* Parameter -x */
extern double ymin;        /* Parameter -y */
extern double skala;       /* Parameter -s */
extern double wskala;      /* Parameter -ws */
extern double vinkel;      /* Parameter -v */
extern double hast;        /* Parameter -h */
extern double formw;       /* Parameter -w */
extern short  npins;       /* Parameter -p */
extern bool   debug;       /* Debug -d */
extern bool   formf;       /* Formfeed -FF */
extern short  nrows;       /* Parameter -l */
extern double curnog;      /* Curve accuracy */
extern char   pfrnam[];    /* Penfile */
extern bool   raw;         /* Parameter -raw */
extern bool   del;         /* Parameter -del */
extern bool   bw;          /* Parameter -bw */
extern char   prognam[];   /* Current device */

double ptabp[WPNMAX];      /* Plotterns pentable */
double ptabr[WPNMAX];      /* Drawing pentable */

V3MDAT sydata;             /* Dummy for ig29.c (v3genv()) */

/*!******************************************************/

        short plppar(
        int   argc,
        char *argv[])

/*      Processar parametrar.
 *
 *      In: argc => Antal parametrar
 *          argv => Array med pekare till parametrar
 *
 *     (C)microform ab Johan Kjellander  23/2/85
 *      
 *      14/4/86 Ny syntax på kommandoraden. R. Svedin
 *      9/9/86  Rättat fel i usage-sträng. R. Svedin
 *      3/1/89  Debug, J. Kjellander
 *      25/1/89 npins, J. Kjellander
 *      8/1/90  -FF, J. Kjellander
 *      4/2/91  -K -PF, J. Kjellander
 *      15/2/95 VARKON_PLT, J. Kjellander
 *      1997-01-22 -raw, J.Kjellander
 *      1997-03-24 -del, J.Kjellander
 *      1998-03-24 -bw, J.Kjellander
 *      1998-11-12 -ws, J.Kjellander
 *
 ******************************************************!*/

 {
    short i;
    bool  wsflag;

/*
***Förställda värden.
*/
    skala  = 1.0;
    wskala = 1.0;
    wsflag = FALSE;
    xmin   = 0.0;
    ymin   = 0.0;
    vinkel = 0.0;
    hast   = 0.6;
    debug  = FALSE;
    formf  = FALSE;
    curnog = 1.0;
    pfrnam[0] = '\0';
    raw    = FALSE;
    del    = FALSE;
    bw     = FALSE;
/*
***Parsa parameterlistan.
*/
    for ( i=1; i<argc; ++i)
      {
      if ( argv[i][0] != '-') goto error;

      switch (argv[i][1])
        {
        case 'S':
        case 's':
        sscanf(&argv[i][2],"%lf",&skala);
        if ( skala <= 0.0 ) goto error;
        break;

        case 'X':
        case 'x':
        sscanf(&argv[i][2],"%lf",&xmin);
        break;

        case 'Y':
        case 'y':
        sscanf(&argv[i][2],"%lf",&ymin);
        break;

        case 'V':
        case 'v':
        sscanf(&argv[i][2],"%lf",&vinkel);
        break;

        case 'K':
        case 'k':
        sscanf(&argv[i][2],"%lf",&curnog);
        if ( curnog < 0.001  ||  curnog > 100.0 ) goto error;
        break;

        case 'H':
        case 'h':
        sscanf(&argv[i][2],"%lf",&hast);
        break;

        case 'P':
        case 'p':
        if ( argv[i][2] == 'F'  ||  argv[i][2] == 'f' ) 
          sscanf(&argv[i][3],"%s",pfrnam);
        else
          sscanf(&argv[i][2],"%hd",&npins);
        break;

        case 'B':
        case 'b':
        if ( argv[i][2] == 'W'  ||  argv[i][2] == 'w' ) bw = TRUE;
        else goto error;
        break;

        case 'R':
        case 'r':
        if ( argv[i][2] == 'A'  ||  argv[i][2] == 'a' ) 
          if ( argv[i][3] == 'W'  ||  argv[i][3] == 'w' ) raw = TRUE;
        break;

        case 'F':
        if ( argv[i][2] == 'F') formf = TRUE;
        else goto error;
        break;

        case 'W':
        case 'w':
        if ( argv[i][2] == 's'  ||  argv[i][2] == 'S' ) 
          {
          sscanf(&argv[i][3],"%lf",&wskala);
          if ( wskala <= 0.0 ) goto error;
          wsflag = TRUE;
          }
        else
          {
          sscanf(&argv[i][2],"%lf",&formw);
          if ( formw <= 0.0 ) goto error;
          }
        break;

        case 'L':
        case 'l':
        sscanf(&argv[i][2],"%hd",&nrows);
        if ( nrows < 1  ||  nrows > 1000 ) goto error;
        break;

        case 'D':
        case 'd':
        if ( (argv[i][2] == 'E'  ||  argv[i][2] == 'e')  &&
             (argv[i][3] == 'L'  ||  argv[i][3] == 'l') ) del = TRUE;
        else debug = TRUE;
        break;

        default:
        goto error;
        break;
        }
     }
/*
***Om wskala inte getts, sätt den = skala.
*/
   if ( !wsflag ) wskala = skala;

   return(0);
/*
***Felutgång.
*/
error:
   printf("usage: %s (option)\n",argv[0]);
   printf("\nOptions that apply to all plotters/printers:\n");
   printf("-xx  or -Xx  Moves image -x mm. along X-axis of device\n");
   printf("-yx  or -Yx  Moves image -x mm. along Y-axis of device\n");
   printf("-sx  or -Sx  Scales image with factor x\n");
   printf("-wsx or -WSx Scales linewidth with factor x\n");
   printf("-vx  or -Vx  Rotates image x degrees anticlockwise\n");
   printf("-kx  or -Kx  Curve accuracy for DXF-files 100=Max, 0.001=Min\n");

   printf("\nOptions that apply only to matrix printers:\n");
   printf("-px  or -Px  Number of pins to be used, 8 or 24\n");
   printf("-wx  or -Wx  Sets width of image window to x mm.\n");
   printf("-lx  or -Lx  Number of lines to be used for image window\n");
   printf("-d   or -D   Shows outline of image window and status\n");
   printf("-FF          Adds formfeed to end of plot\n");

   printf("\nOptions that apply only to matrix or pen printers:\n");
   printf("-hx  or -Hx  Pen or printerhead speed 1=Max, 0=Min\n");
   printf("-pfx or -PFx Use PEN-file x in this plot\n");

   printf("\nOptions that apply only to color printers:\n");
   printf("-bw  or -BW  Don't use color\n");

   exit(0);
 }

/********************************************************/
/*!******************************************************/

        short pllpfp(char *fnam)

/*      Laddar plotterns penfil.
 *
 *      In: fnam = Fil som skall laddas.
 *
 *      Ut: ptabp ifylld.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab Johan Kjellander  4/2/91
 *
 *      1996-02-22 WIN32, J. Kjellander
 *      
 ******************************************************!*/

 {
   char   buf[132];
   int    i;
   double w;
   FILE  *pf;

/*
***Initiera plotterns pentabell.
*/
   for ( i=0; i<WPNMAX; ++i ) ptabp[i] = 0.0;
/*
***Öppna pen-filen.
*/
    strcpy(buf,v3genv(VARKON_PLT));
    strcat(buf,fnam);

    if ( (pf=fopen(buf,"r")) == NULL )
      {
      printf("%s: Can't find plotter PEN-file %s\n",prognam,buf);
      exit(V3EXOK);
      }
/*
***Läs rad för rad.
*/
   while ( fscanf(pf,"%d%*[ =]%lf%*[ \n]",&i,&w) != EOF )
     if ( i < WPNMAX ) ptabp[i] = w;
/*
***Slut.
*/
   fclose(pf);

   return(0);
}

/********************************************************/
/*!******************************************************/

        short pllpfr(char *path)

/*      Laddar ritnings pennbreddsfil.
 *
 *      In: path = Vägbeskrivning till fil som skall laddas.
 *
 *      Ut: ptabr ifylld.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab Johan Kjellander  4/2/91
 *      
 ******************************************************!*/

 {
   int    i;
   double w;
   FILE  *pf;

/*
***Initiera pentabell.
*/
   for ( i=0; i<WPNMAX; ++i ) ptabr[i] = 0.0;
/*
***Öppna pennbredds-filen.
*/
    if ( (pf=fopen(path,"r")) == NULL )
      {
      printf("%s: Can't find drawing PEN-file %s\n",prognam,path);
      exit(V3EXOK);
      }
/*
***Läs rad för rad.
*/
   while ( fscanf(pf,"%d%*[ =]%lf%*[ \n]",&i,&w) != EOF )
     if ( i < WPNMAX ) ptabr[i] = w;
/*
***Slut.
*/
   fclose(pf);

   return(0);
}

/********************************************************/
