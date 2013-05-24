/**********************************************************************
*
*    dbtform.c
*    =========
*
*
*    This file includes the following public functions:
*
*    DBinsert_tform();   Inserts a transformation entity
*    DBread_tform();     Reads a transformation entity
*    DBupdate_tform();   Updates a transformation entity
*    DBdelete_tform();   Deletes a transformation entity
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_tform(
        GMTRF   *trfpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av transformation.
 *
 *      In: trfpek => Pekare till en trf-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Out: *la    => Logisk adress till post i GM.
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    trfpek->hed_tf.type = TRFTYP;   /* Typ = transformation */
    trfpek->hed_tf.vers = GMPOSTV0; /* Version */
/*
***Lagra posten.
*/
    return(inpost((GMUNON *)trfpek,idpek,lapek,sizeof(GMTRF)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_tform(
        GMTRF *trfpek,
        DBptr  la)

/*      Huvudrutin för läsning av transformation.
 *
 *      In: trfpek => Pekare till en trf-structure.
 *          la     => Postens adress i GM.
 *
 *      Out: *trfpek => Transformations-post.
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    return(rddat1((char *)trfpek,la,sizeof(GMTRF)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmuptr(
        GMTRF *trfpek,
        DBptr  la)

/*      Skriver över en existerande transformations-post.
 *
 *      In: trfpek => Pekare till en trf-structure.
 *          la     => Postens adress i GM.
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    return(updata((char *)trfpek,la,sizeof(GMTRF)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_tform(DBptr la)

/*      Stryker en trf-post och deallokerar allokerat minne. 
 *
 *      In: la => Postens GM-adress.
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    return(rldat1(la,sizeof(GMTRF)));
  }

/********************************************************/
