/*!*****************************************************
*
*    extime.c
*    ========
*
*    EXtime();      Interface routine for TIME
*
*    This file is part of the VARKON Execute  Library.
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"
#include <time.h>

/*!******************************************************/


        short EXtime(
        DBshort *y,
        DBshort *mo,
        DBshort *d,
        DBshort *h,
        DBshort *mi,
        DBshort *s)

/*      Interface-rutin för TIME. Hämtar datum
 *      och tid.
 *
 *      In: Inget.
 *
 *      Ut: *y  => År.
 *          *mo => Månad.
 *          *d  => Dag i månaden.
 *          *h  => Timme.
 *          *mi => Minut.
 *          *s  => Sekund.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      011010 time_t, J.Kjellander
 *
 ******************************************************!*/

  {
    time_t     reltim;
    struct tm *timpek;
/*
***Läs av klockan.
*/
    reltim = time ((time_t *)0);
    timpek = localtime(&reltim);

    *y  = (short)timpek->tm_year;
    *mo = (short)(timpek->tm_mon+1);
    *d  = (short)timpek->tm_mday;
    *h  = (short)timpek->tm_hour;
    *mi = (short)timpek->tm_min;
    *s  = (short)timpek->tm_sec;

    return(0);
  }

/********************************************************/
