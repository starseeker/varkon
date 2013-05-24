/**********************************************************************
*
*    file.c
*    ======
*
*    This file is part of the VARKON Utility Library.
*    URL:  http://www.varkon.com
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
***This file includes default versions of some file
***manipulation routines used by the DB and GE modules.
***You can use these or write your own if you want to
***(see below).
***This file is provided as a default replacement of the
***original VARKON routines which are more complex
***and rely on a lot of VARKON specific code which
***has nothing with DB or GE to do.
*/

/*
***First of all include the standard C header file.
*/

#include <stdio.h>

/*
***Also include the definition of bool and TRUE and FALSE.
*/

#include "../../DB/include/DB.h"

/*
***This routine checks if a file is read accessible.
***access() could be used instead of fopen() but
***fopen() has shown to detect problems that acess()
***does not find on all platforms.
*/

bool  v3ftst(                /* Check if file can be opened read_only */
      char *path)            /* File name and path */
{
   FILE  *f;

  if ( (f=fopen(path,"r")) == 0 ) return(FALSE);
  else
    {
    fclose(f);
    return(TRUE);
    }
}
