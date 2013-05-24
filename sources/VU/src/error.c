/**********************************************************************
*
*    error.c
*    =======
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
***This file includes default versions of error
***handling routines used by the DB and GE modules.
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
***The error handling system used by Varkon is
***based on 4 different routines. erpush() is used
***to push an errror message on the error message stack.
***erlerr() returns the last error code.
***erinit() is used to clear the stack and errmes() is
***used to display the stack to the user.
***
***Error messages are 6-character strings composed
***of 2 initial letters and 4 trailing digits.
***Here is an example: "GE2642"
***The two characters "GE" indicate what module
***the error comes from, The Varkon GE-module in
***this case. The last digit is treated as a severity
***code as follows:
***Severity = 0 means OK, ie. no error.
***Severity = 1 is a warning only.
***Severity = 2 is a non recoverable user error.
***Severity = 3 is a non recoverable system error.
***Severity = 4 is a panic error.
***The remaining 3 digits is just a number, unique
***within the module, that relate to the specific
***error. In Varkon these error numbers are used
***to locate a textual description of the error
***from an external file. Different files for
***different languages.
***
***We decided to supply the three following simplified
***replacements for the original Varkon versions.
***You may use them if you wish or replace them
***with something better of your own choice.
*/

short erinit()                               /* Clear error stack */
{
   return(0);
}

short erpush(
      char *error_code,                      /* Error code */
      char *extra_string)                    /* Extra text */
{
   int severity;

   printf("erpush called, errcode=%s,extra_string=%s\n",
           error_code,extra_string);         /* Use printf to display */

   sscanf(error_code+5,"%d",&severity);      /* Extract severity code */

   return((short)(-severity));               /* Return negative severity */
}

short erlerr()                               /* Return last errcode */
{
   return(0);
}

short errmes()                               /* Display error stack */
{
   return(0);
}
