/*!******************************************************
*
*    dbheader.c
*    ==========
*
*    This file includes the following public routines:
*
*    DBread_header();    Reads an entity header
*    DBupdate_header();  Rewrites/Updates an entity header
*
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
*********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBread_header(
        GMRECH *hedptr,
        DBptr   la)

/*      Reads an entity header.
 *
 *      In:  hedptr => Pointer to GMRECH
 *           la     => Entity DB pointer
 *
 *      Out: *hedptr = Copies header to hedptr
 *
 *      FV:  0 => Ok
 *
 *      (C)microform ab 6/8/85 J. Kjellander
 *
 ******************************************************!*/

  {
    rddat1( (char *)hedptr, la, sizeof(GMRECH));
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_header(
        GMRECH *hedptr,
        DBptr   la)

/*      Updates/rewrites an entity header.
 *
 *      In:  hedptr => Pointer to GMRECH
 *           la     => Entity DB pointer
 *
 *      FV:  0 => Ok.
 *
 *      (C)microform ab 6/8/85 J. Kjellander
 *
 ******************************************************!*/

  {
    updata( (char *)hedptr, la, sizeof(GMRECH));
    return(0);
  }

/********************************************************/
