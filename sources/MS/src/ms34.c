/**********************************************************************
*
*    ms34.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msargv();  Translates args into argc and argv
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"

/*!******************************************************/

        int   msargv(args,pargc,argv)
        char  *args;
        int   *pargc;
        char  *argv[];

/*      Skapar argc/argv från WinMains args-parameter.
 *      Vid anropet är pargc satt dimensionen hos argv,
 *      dvs. hur många parametrar som får plats i argv.
 *      Pekarna i argv är initierade till att peka på
 *      pargc st. ej initierade strängar V3STRLEN stora.
 *
 *      In: args  = Kommandoraden som en enda sträng.
 *          pargc = Max antal parametrar.
 *
 *      Ut: *pargs = Antal parametrar.
 *          *argv  = Pekare till parametrarna.
 *
 *      FV: 0.
 *
 *      (C)microform ab 24/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int  start,stop,maxpar,npar,args_len;
   char parbuf[V3STRLEN];

/*
***Initiering.
*/
   args_len = strlen(args);
   maxpar = *pargc;
/*
***1:a parametern skall vara kommandot som startade v3.
*/
   strcpy(argv[0],"v3");
   npar = 1;
/*
***Scanna args.
*/
   start = stop = 0;

   for ( ;; )
     {
     if ( *(args+stop) == ' '  ||  *(args+stop) == '\0' )
       {
       if ( stop > start )
         {
         if ( stop-start < V3STRLEN )
           {
           strncpy(parbuf,(args+start),stop-start);
           parbuf[stop-start] = '\0';
           strcpy(argv[npar],parbuf);
         ++npar;
           if ( npar == maxpar ) goto end;
           while ( *(args+stop) == ' ' ) ++stop;
           start = stop;
           }
         else ++stop;
         }
       else ++stop;
       }
     else ++stop;
     if ( stop > args_len ) goto end;
     }
/*
***Slut.
*/
end:

  *pargc  = npar;

   return(0);
  }

/********************************************************/
