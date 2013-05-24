/**********************************************************************
*
*    dbpoint.c
*    =========
*
*    This file includes the following public functions:
*
*    DBinsert_point();   Inserts a point entity
*    DBread_point();     Reads a point entity
*    DBupdate_point();   Updates a point entity
*    DBdelete_point();   Deletes a point entity
*
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_point(
        DBPoint *poipek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Inserts a new point entity in the DB.
 *
 *      In: poipek => Pointer to point data.
 *          idpek  => Pointer point ID.
 *
 *      Out: *lapek => The point address.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 13/12/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1998-04-03 GMPOSTV2, J.Kjellander
 *      2007-03-22 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Type-specific data.
*/
    poipek->hed_p.type = POITYP;     /* Type = point */
    poipek->hed_p.vers = GMPOSTV3;   /* Version */
/*
***Save.
*/
    return(inpost((GMUNON *)poipek,idpek,lapek,sizeof(DBPoint)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_point(
        DBPoint *poipek,
        DBptr    la)

/*      Reads a point entity from the DB.
 *
 *      In: la       => Point address.
 *
 *      Out: *poipek => Point data.
 *
 *      (C)microform ab 13/12/84 J. Kjellander
 *
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1998-04-03 GMPOSTV2, J.Kjellander
 *      2007-03-22 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV3:
      V3MOME(hedpek,poipek,sizeof(DBPoint));
      break;

      case GMPOSTV2:
      V3MOME(hedpek,poipek,sizeof(GMPOI2));
      poipek->fnt_p = 0;
      poipek->size_p = 2;
      break;

      case GMPOSTV1:
      V3MOME(hedpek,poipek,sizeof(GMPOI1));
      poipek->wdt_p = 0.0;
      poipek->fnt_p = 0;
      poipek->size_p = 2;
      break;
 
      default:
      V3MOME(hedpek,poipek,sizeof(GMPOI0));
      poipek->pcsy_p = DBNULL;
      poipek->wdt_p = 0.0;
      poipek->fnt_p = 0;
      poipek->size_p = 2;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_point(
        DBPoint *poipek,
        DBptr    la)

/*      Updates a point entity in the DB. The point
 *      address is not changed.
 *
 *      In: poipek => Pointer to point data.
 *          la     => Point address.
 *
 *      (C)microform ab 13/12/84 J. Kjellander
 *
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1998-04-03 GMPOSTV2, J.Kjellander
 *      2007-03-22 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV3:
      updata( (char *)poipek, la, sizeof(DBPoint));
      break;

      case GMPOSTV2:
      updata( (char *)poipek, la, sizeof(GMPOI2));
      break;
 
      case GMPOSTV1:
      updata( (char *)poipek, la, sizeof(GMPOI1));
      break;
 
      default:
      updata( (char *)poipek, la, sizeof(GMPOI0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_point(DBptr la)

/*      Deletes a point entity from the DB. The space
 *      allocated by the point data is freed.
 *
 *      In: la => Point address.
 *
 *      (C)microform ab 17/12/84 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      21/10/58 Läs endast headern, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1998-04-03 GMPOSTV2, J.Kjellander
 *      2007-03-22 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV3:
      rldat1(la,sizeof(DBPoint));
      break;

      case GMPOSTV2:
      rldat1(la,sizeof(GMPOI2));
      break;
 
      case GMPOSTV1:
      rldat1(la,sizeof(GMPOI1));
      break;
 
      default:
      rldat1(la,sizeof(GMPOI0));
      break;
      }

    return(0);
  }

/********************************************************/
