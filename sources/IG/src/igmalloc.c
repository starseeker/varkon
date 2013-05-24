/*!******************************************************************/
/*  igmalloc.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  v3mini();     Inits memory allocation system                    */
/* *v3mall();     Allocates C memory                                */
/* *v3rall();     Reallocates C memory                              */
/*  v3free();     Deallocates C memory                              */
/*  v3msta();     Writes allocation table to disc                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
#include "../include/IG.h"

#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include "../include/debug.h"
#endif

typedef struct alarea
{
void *adress;
char  name[31];
int   size;
} ALAREA;

static ALAREA *alltab;
static int     altsiz;

#ifdef DEBUG
#define ALTSIZ 2
#else
#define ALTSIZ 200
#endif

/* alltab är en tabell med info om allokerade areor. */
/* altsiz är alltabs aktuella storlek */
/* ALTSIZ är default storlek samt inkrement */

/*!******************************************************/

        void v3mini()

/*      Initierar minnesallokeringssystemet.
 *
 *      (C)microform ab 14/3/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    ALAREA *altptr;

/*
***Allokera minne för alltab.
*/
    altsiz = ALTSIZ;
    alltab = (ALAREA *)malloc(altsiz*sizeof(ALAREA));
/*
***Nollställ alltab.
*/
    altptr = alltab;

    for ( i=0; i<altsiz; i++ )
      {
      altptr->adress  = NULL;
      altptr->name[0] = '\0';
      altptr->name[6] = '\0';
      altptr->size    = 0;
      ++altptr;
      }
  }

/********************************************************/
/*!******************************************************/

        void *v3mall(
        unsigned size,
        char    *name)

/*      Allokerar C-minne med hjälp av malloc(). Anrop med
 *      namn noteras i alltab.
 *
 *      In: size = Antal bytes att allokera.
 *          name = Anropande rutins namn, max 6 tkn.
 *
 *      Ut: Inget.
 *
 *      FV: Adress eller NULL om fel.
 *
 *      Felkoder: IG0363 = Kan ej mallokera %s bytes.
 *
 *      (C)microform ab 14/3/95 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef DEBUG
   char    errbuf[81];
   int     i;
   ALAREA *altptr;

   if ( dbglev(IGEPAC) == 361  &&  *name != '\0' )
     {
     fprintf(dbgfil(IGEPAC),"***Start-v3mall***\n");
     fflush(dbgfil(IGEPAC));
     }
/*
***Om ett namn har skickats med skall allokeringen noteras i alltab.
*/
   if ( *name != '\0' )
     {
/*
***Leta upp ett ledigt entry i alltab.
*/
     i = 0;
     altptr = alltab;

     while ( i < altsiz  &&  altptr->adress != NULL )
       {
       i++;
       altptr++;
       }
/*
***Finns det inget ledigt ökar vi storleken på alltab och
***anropar oss själva.
*/
    if ( i == altsiz )
      {
      if ( (alltab=(ALAREA *)realloc((void *)alltab,
                             (altsiz+ALTSIZ)*sizeof(ALAREA))) == NULL )
        {
        sprintf(errbuf,"%d",size);
        erpush("IG0363",errbuf);
        v3msta();
        return(NULL);
        }

      altptr = alltab + altsiz;
      altsiz += ALTSIZ;
 
      while ( i<altsiz ) 
        {
        altptr->adress  = NULL;
        altptr->name[0] = '\0';
        altptr->name[6] = '\0';
        altptr->size    = 0;
        i++;
        altptr++;
        }

      return(v3mall(size,name));
      }
/*
***Ledig plats finns i alltab. Prova att allokera.
*/
     altptr->adress = malloc(size);
/*
***Gick det bra ?
*/
     if ( altptr->adress == NULL )
       {
       sprintf(errbuf,"%d",size);
       erpush("IG0363",errbuf);
       v3msta();
       return(NULL);
       }
/*
***Det gjorde det, notera även namn och storlek i alltab.
*/
     strncpy(altptr->name,name,6);
     altptr->size = size;

   if ( dbglev(IGEPAC) == 361 )
     {
     v3msta();
     fprintf(dbgfil(IGEPAC),"***Slut-v3mall***\n\n");
     fflush(dbgfil(IGEPAC));
     }

     return(altptr->adress);
     }
/*
***Om inget namn skickats med allokerar vi direkt utan
***notering i alltab.
*/
   else return(malloc(size));
/*
***Kompilerar vi utan DEBUG skall det gå så fort som möjligt.
*/
#else
   return(malloc(size));
#endif
  }

/********************************************************/
/*!******************************************************/

        void *v3rall(
        void    *ptr,
        unsigned size,
        char    *name)

/*      Rellokerar C-minne med hjälp av realloc().
 *
 *      In: ptr  = Dataareans nuvarande adress.
 *          size = Önskad ny storlek.
 *          name = Anropande rutins namn, max 6 tkn.
 *
 *      Ut: Inget.
 *
 *      FV: Adress eller NULL om fel.
 *
 *      Felkoder: IG0383 = Kan ej reallokera.
 *
 *      (C)microform ab 14/3/95 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef DEBUG
   int     i;
   ALAREA *altptr;

/*
   if ( dbglev(IGEPAC) == 361 )
     ä
     fprintf(dbgfil(IGEPAC),"***Start-v3rall***Ön");
     fprintf(dbgfil(IGEPAC),"ptr = %d, size = %d, name = %sÖn",
                                                      ptr,size,name);
     fflush(dbgfil(IGEPAC));
     å
*/
/*
***Leta upp adressen i alltab.
*/
   altptr = alltab;

   for ( i=0; i<altsiz; ++i )
     {
     if ( altptr->adress == ptr ) break;
     altptr++;
     }
/*
***Finns den inte ?
*/
   if ( i == altsiz )
     {
     erpush("IG0383",name);
     errmes();
     return(NULL);
     }
/*
**Jodå, reallokera.
*/
    altptr->adress = realloc(ptr,size);
/*
***Gick det bra ?
*/
   if ( altptr->adress == NULL )
     {
     erpush("IG0383",name);
     return(NULL);
     }
/*
***Notera ny storlek i alltab.
*/
    altptr->size = size;

    return(altptr->adress);
/*
***Kompilerar vi utan DEBUG skall det gå så fort som möjligt.
*/
#else
   return(realloc(ptr,size));
#endif
  }

/********************************************************/
/*!******************************************************/

        short v3free(
        void *ptr,
        char *name)

/*      Deallokerar C-minne med hjälp av free().
 *
 *      In: ptr  = Allokerade areans adress.
 *          name = Anropande rutins namn, max 6 tkn.
 *
 *      Ut: Inget.
 *
 *      FV:   0 = Ok.
 *          < 0 = Felaktig adress som indata.
 *
 *      Felkoder: IG0373 = Kan ej deallokera.
 *
 *      (C)microform ab 14/3/95 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef DEBUG
   int     i;
   ALAREA *altptr;

   if ( dbglev(IGEPAC) == 361  &&  *name != '\0' )
     {
     fprintf(dbgfil(IGEPAC),"***Start-v3free***\n");
     fflush(dbgfil(IGEPAC));
     }
/*
***Skall den finnas i alltab ?
*/
   if ( *name != '\0' )
     {
/*
***ja, leta upp adressen i alltab.
*/
     altptr = alltab;

     for ( i=0; i<altsiz; ++i )
       {
       if ( altptr->adress == ptr ) break;
       altptr++;
       }
/*
***Finns den inte ?
*/
     if ( i == altsiz )
       {
       erpush("IG0373",name);
       errmes();
       return(-3);
       }
/*
***Jodå, då deallokerar vi.
*/
     altptr->adress  = NULL;
     altptr->name[0] = '\0';
     altptr->size    = 0;
     }
/*
***Oavsett om den tidigare lagrats i alltab eller inte är
***det nu dags att deallokera.
*/
   free(ptr);

   if ( dbglev(IGEPAC) == 361  &&  *name != '\0' )
     {
     v3msta();
     fprintf(dbgfil(IGEPAC),"***Slut-v3free***\n\n");
     fflush(dbgfil(IGEPAC));
     }
/*
***Kompilerar vi utan DEBUG skall det gå så fort som möjligt.
*/
#else
    free(ptr);
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        void v3msta()

/*      Skriver ut alltab.
 *
 *      (C)microform ab 14/3/95 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Skriv ut alltab.
*/
#ifdef DEBUG
    int     i,totsiz;
    ALAREA *altptr;

    altptr = alltab;
    totsiz = 0;

  if ( dbglev(IGEPAC) == 36  ||
       dbglev(IGEPAC) == 361 )
    {
    for ( i=0; i<altsiz; i++ )
      {
      if ( altptr->size > 0 )
        {
        fprintf(dbgfil(IGEPAC),"alltab(%d) = %d, %s, %d\n",i,(int)altptr->adress,
                                                   altptr->name,altptr->size);

        totsiz += altptr->size;
        }
      ++altptr;
      }
    fprintf(dbgfil(IGEPAC),"Totalt = %d bytes\n",totsiz);
    fflush(dbgfil(IGEPAC));
    }
#endif

  }

/********************************************************/
