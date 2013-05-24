/**********************************************************************
*
*    wp3.c
*    ====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpgrst();  Returns X-resource value
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

/*!******************************************************/

        bool wpgrst(
        char *resnam,
        char *resval)

/*      Om resursen är definierad returneras dess värde
 *      och TRUE annars FALSE.
 *
 *      In: resnam = Resurs.
 *
 *      Ut: resval = Värde.
 *
 *      FV: TRUE = Resursen finns, värde returneras.
 *          FALSE = Resursen finns ej, "" returneras.
 *
 *      (C)microform ab 17/10/95 J. Kjellander
 *
 ******************************************************!*/

 {
   bool     hit;
   int      i,ntkn;
   char     buf[V3STRLEN+1];

#ifdef V3_X11
   char    *type[20];
   XrmValue value;
#endif

/*
***Lite initiering.
*/
   hit    = FALSE;
  *resval = '\0';
/*
***Här kommer X-Windows versionen. För att kunna använda
***XrmGetResource() måste vi skapa två strängar, en med
***små bokstäver och en med stora.
*/
#ifdef V3_X11

   ntkn = strlen(resnam);
   if ( ntkn > 0  &&  ntkn < V3STRLEN )
     {
     strcpy(buf,resnam);

     buf[0] -= 32;
     for ( i=0; i<ntkn; ++i )
       if ( resnam[i] == '.' ) buf[i+1] -= 32;
     if ( XrmGetResource(xresDB,resnam,buf,type,&value) )
       {
       strcpy(resval,value.addr);
       hit = TRUE;
       }
     }
#endif
/*
***Någon WIN32 version finns inte.
*/

    return(hit); 
  }

/********************************************************/
