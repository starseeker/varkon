/*!******************************************************************/
/*  igPID.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGdir();   Create directory listing                             */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../EX/include/EX.h"

/*!******************************************************/

        short IGdir(
        char *inpath,
        char *typ,
        int   maxant,
        int   maxsiz,
        char *pekarr[],
        char *strarr,
        int  *nf)

/*      Returnerar filer med visst efternamn. Efternanmnet
 *      returneras inte.
 *
 *      In:
 *          inpath = Sökvägbeskrivning, tex. "/usr/v3/pid".
 *                   Ev. slash på slutet går bra.
 *          typ    = Filtyp inklusive punkt, tex. ".PID".
 *          maxant = Max antal filer (pekarr:s storlek)
 *          maxsiz = Max antal tecken (buf:s storlek).
 *          strarr = Plats att lagra filnamn.
 *
 *      Ut:
 *          pekarr = Array med nf stycken pekare till filnamn.
 *          nf     = Antal filer.
 *
 *      FV:  0 = Ok.
 *
 *      (C)microform ab 1998-04-10 J. Kjellander
 *
 ******************************************************!*/

  {
   char  pattern[V3STRLEN],*s;
   short status;
   int   i,n;
   DBint nfiles;

/*
***Skapa söksträng.
*/
   strcpy(pattern,"*");
   strcat(pattern,typ);
/*
***Hämta filförteckning.
*/
   status = EXdirl(inpath,pattern,maxant,maxsiz,pekarr,strarr,&nfiles);
   if ( status < 0 ) return(status);
  *nf = nfiles;
/*
***Strippa efternamnen.
*/
   for ( i=0; i<*nf; ++i )
     {
     s = pekarr[i];
     n = strlen(s) - strlen(typ);
     if ( n >= 0 ) *(s+n) = '\0';
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
