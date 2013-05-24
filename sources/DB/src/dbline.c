/**********************************************************************
*
*    dbline.c
*    ========
*
*
*    This file includes the following public functions:
*
*    DBinsert_line();   Inserts a line entity
*    DBread_line();     Reads a line entity
*    DBupdate_line();   Uppdates a line entity
*    DBdelete_line();   Deletes a line entity
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

        DBstatus DBinsert_line(
        GMLIN   *linpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Inserts a line entity in the DB.
 *
 *      In: linpek => Pointer to line data.
 *          idpek  => Pointer to line ID.
 *
 *      Out: *lapek => The line address.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 27/12/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1997-12-24 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Typ-specifika data.
*/
    linpek->hed_l.type = LINTYP;
    linpek->hed_l.vers = GMPOSTV2;
/*
***Lagra.
*/
    return(inpost((GMUNON *)linpek,idpek,lapek,sizeof(GMLIN)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_line(
        GMLIN   *linpek,
        DBptr    la)

/*      Reads a line entity from the DB.
 *
 *      In:  la      => Line address.
 *
 *      Out: *linpek => Line data.
 *
 *      (C)microform ab 27/12/84 J. Kjellander
 *
 *      22/3/92    GMPOSTV1, J. Kjellander
 *      1997-12-24 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      V3MOME(hedpek,linpek,sizeof(GMLIN));
      break;
 
      case GMPOSTV1:
      V3MOME(hedpek,linpek,sizeof(GMLIN1));
      linpek->wdt_l = 0.0;
      break;
 
      default:
      V3MOME(hedpek,linpek,sizeof(GMLIN0));
      linpek->pcsy_l = DBNULL;
      linpek->wdt_l = 0.0;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_line(
        GMLIN   *linpek,
        DBptr    la)

/*      Updates a line entity in the DB. The line
 *      address is not changed.
 *
 *      In: linpek => Pointer to line data.
 *          la     => Line address..
 *
 *      (C)microform ab 27/12/84 J. Kjellander
 *
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1997-12-24 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      updata( (char *)linpek, la, sizeof(GMLIN));
      break;
 
      case GMPOSTV1:
      updata( (char *)linpek, la, sizeof(GMLIN1));
      break;
 
      default:
      updata( (char *)linpek, la, sizeof(GMLIN0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_line(DBptr la)

/*      Deletes a line entity from the DB. The space
 *      allocated by the line data is freed.
 *
 *      In: la => Line address.
 *
 *      (C)microform ab 28/12/84 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      1997-12-24 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(GMLIN));
      break;
 
      case GMPOSTV1:
      rldat1(la,sizeof(GMLIN1));
      break;
 
      default:
      rldat1(la,sizeof(GMLIN0));
      break;
      }

    return(0);
  }

/********************************************************/
