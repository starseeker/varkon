/**********************************************************************
*
*    ms31.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     mshlp1();   Help about help
*     mshlp2();   Contents
*     mshlp3();   Index
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

        short mshlp1()

/*      Hjälp om hjälp.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 29/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
    WinHelp(ms_main,"",HELP_HELPONHELP,0);

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short mshlp2()

/*      Hjälp innehåll.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 29/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
    char hlpfil[V3PTHLEN];

    sprintf(hlpfil,"%sVARKON.HLP",v3genv(VARKON_DOC));

    WinHelp(ms_main,hlpfil,HELP_CONTENTS,0);

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short mshlp3()

/*      Hjälp index.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 29/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
    char hlpfil[V3PTHLEN];

    sprintf(hlpfil,"%sVARKON.HLP",v3genv(VARKON_DOC));
    WinHelp(ms_main,hlpfil,HELP_INDEX,0);

    return(0);
 }

/********************************************************/
