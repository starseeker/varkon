/**********************************************************************
*
*    memory.c
*    ========
*
*    This file is part of the VARKON Utility Library.
*    URL: http://www.tech.oru.se/cad/varkon
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

/*
***This file includes default versions of memory
***allocation routines used by the Varkon system.
***You can use these or write your own if you want to
***(see below).
***This file is provided as a default replacement of the
***original VARKON routines which are more complex
***and rely on a lot of VARKON specific code.
*/

/*
***First of all include the standard C header file.
*/

#include <stdlib.h>

/*
***All memory allocation is done by Varkon routines 
***v3mall(),v3rall() and v3free(). In the original
***VARKON version these routines use the standard
***C-library malloc(), realloc() and free() but also
***provide a mechanism for logging memory usage and
***thereby fixing memory leaks and trapping illegal
***adresses etc.
***The logging code in the VARKON versions includes a
***a lot of calls to interactive functions that
***display log statistics using dialogs. To include
***that here you would need to include a lot of
***VARKON specific code of no interest for other
***applications. If you need any of the above
***functionality we assume you write your own.
*/

void *v3mall(                /* Allocate new memory */
      unsigned  size,        /* Number of bytes */
      char     *name)        /* Log message */
{
  return(malloc(size));      /* Return C-adress */
}

void *v3rall(                /* Reallocate memory */
      void     *ptr,         /* Pointer to allocated area */
      unsigned  size,        /* New sizes */
      char     *name)        /* Log message */
{
  return(realloc(ptr,size)); /* Return new C-adress */
}

short v3free(                /* Deallocate memory */
      void     *ptr,         /* Pointer to allocated area */
      char     *name)        /* Log message */
{
  free(ptr);
  return(0);                 /* Return status, 0=OK, < 0=Error */
}
