/*!*******************************************************
*
*    ex2.c            !!!!!!!!!!Dummy for Leica !
*    =====
*
*    EXecsy();    Create coordinatesystem
*    EXcs3p();    Create CSYS_3P
*    EXcs1p();    Create CSYS_1P
*    EXoppt();    Create/open part
*    EXclpt();    Close part
*    EXetxt();    Create text
*    EXtext();    Create TEXT
*    EXgrp();     Create GROUP
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../GP/include/GP.h"

#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif

/* #include "../include/EX.h" */

/*!******************************************************/

       short EXoppt(
       DBId    *id,
       DBId    *refid,
       GMPRT   *prtpek,
       GMPDAT  *datpek,
       V2PATLOG typarr[],
       char     valarr[])

/*      Skapa part.
 *
 *      In: id     => Pekare till partens lokala identitet.
 *          refid  => Pekare till id för koordinatsystem.
 *          prtpek => Pekare till part-structure.
 *          datpek => Pekare till part-data structure.
 *          typarr => Pekare till array med typ-info strukturer.
 *          valarr => Pekare till array med värden för
 *                    ev. parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: 0 => Ok.
 *           EX1762 => Refererat koordinatsystem finns ej i GM.
 *           EX1422 => Kan ej lagra part i GM.
 *           EX1562 => Koordinatsystem saknas
 *           EX1572 => Geometrimodul anropar ritningsmodul
 *           EX1752 => Refererad storhet ej koordinatsystem
 *
 *      (C)microform ab 26/4/85 J. Kjellander
 *
 *      14/10/85 Header-data. J. Kjellander
 *      7/11/85  BASIC, J. Kjellander
 *      21/11/85 GLOBAL-bug, J. Kjellander
 *      1/2/86   Koll av refid, J. Kjellander
 *      23/2/86  Nytt anrop, J. Kjellander
 *      4/3/86   EX1572, J. Kjellander
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      25/11/86 Koll av typ, J. Kjellander
 *      25/11/86 tmspek, J. Kjellander
 *      7/9/87   Flyttat hant. av modtyp t. PM, J. Kjellander
 *      20/3/92  Attribut, J. Kjellander
 *      6/2/95   msysla, J. Kjellander
 *
 ******************************************************!*/

  {

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXclpt()

/*      Stänger part.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 26/4/85 J. Kjellander
 *
 *      7/8/87  modtyp, J. Kjellander
 *      6/2/95  msysla, J. Kjellander
 *
 ******************************************************!*/

  {


    return(0);

  }
  
/********************************************************/
