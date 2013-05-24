/**********************************************************************
*
*    env.c
*    ======
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
***********************************************************************/

/*
***This file includes a default version of a routine
***similar to C's getenv(). It is used by the GE module.
***You can use this code or write your own if you want to
***(see below).
***This file is provided as a default replacement of the
***original VARKON routine which is more complex
***and relies on a lot of VARKON specific code which
***has nothing with DB or GE to do.
*/

/*
***Include DB.h.
*/

#include "../../DB/include/DB.h"

/*
***This routine translates an environment parameter to
***it's corresponding value. If no value is found it
***returns NULL. The GE module will use hardcoded
***default handling if no value is found so for the
***time being we just need to return NULL.
*/

char *v3genv(char *name) /* Environment variable name */
{
   return(NULL);
}
