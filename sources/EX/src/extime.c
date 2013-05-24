/*!*****************************************************
*
*    extime.c
*    ========
*
*    EXtime();      Interface routine for TIME
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"
#include <time.h>

/*!******************************************************/

        short EXtime(
        int  *y,
        int  *mo,
        int  *d,
        int  *h,
        int  *mi,
        int  *s)

/*      Interface-rutine for TIME. Returns current date and time.
 *
 *      Out: *y  => Year.
 *           *mo => Month.
 *           *d  => Day.
 *           *h  => Hour.
 *           *mi => Minute.
 *           *s  => Second.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      011010 time_t, J.Kjellander
 *      2007-03-18 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    time_t     reltim;
    struct tm *timpek;
/*
***Get current time.
*/
    reltim = time ((time_t *)0);
    timpek = localtime(&reltim);
/*
***Return data.
*/
   *y  = timpek->tm_year + 1900;
   *mo = timpek->tm_mon + 1;
   *d  = timpek->tm_mday;
   *h  = timpek->tm_hour;
   *mi = timpek->tm_min;
   *s  = timpek->tm_sec;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
