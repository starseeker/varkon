/**********************************************************************
*
*    dbxhatch.c
*    ==========
*
*
*    This file includes the following public functions:
*
*    DBinsert_xhatch();   Inserts a xhatch entity
*    DBread_xhatch();     Reads a xhatch entity
*    DBupdate_xhatch();   Updates a xhatch entity
*    DBdelete_xhatch();   Deletes a xhatch entity
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
***********************************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_xhatch(
        GMXHT   *xhtpek,
        DBfloat *crdpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av snitt.
 *
 *      In: xhtpek => Pekare till en snitt-structure.
 *          crdpek => Pekare till array med koordinater.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till snitt-post i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 10/7/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la_crd;
    DBstatus status;

/*
***Lagra snittlinjerna.
*/
    status=wrdat2((char *)crdpek, &la_crd, 4*xhtpek->nlin_xh*sizeof(DBfloat));
    if ( status < 0 ) return(status);
/*
***Typ-specifika data.
*/
    xhtpek->hed_xh.type = XHTTYP;
    xhtpek->hed_xh.vers = GMPOSTV1;
    xhtpek->lptr_xh = la_crd;
/*
***Lagra själva posten.
*/
    return(inpost((GMUNON *)xhtpek,idpek,lapek,sizeof(GMXHT)));

  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_xhatch(
        GMXHT   *xhtpek,
        DBfloat *crdpek,
        DBptr    la)

/*      Läsning av snitt-post.
 *
 *      In: xhtpek => Pekare till en snitt-structure.
 *          crdpek => Pekare till array för linjer
 *          la     => Snittets adress i GM.
 *
 *      Ut: *xhtpek => Snitt-post.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 10/7/84 J. Kjellander
 *
 *      17/3/88  crdpek=NULL, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

/*
***Läs själva posten.
*/
    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,xhtpek,sizeof(GMXHT));
      break;
 
      default:
      V3MOME(hedpek,xhtpek,sizeof(GMXHT0));
      xhtpek->pcsy_xh = DBNULL;
      break;
      }
/*
***Läs linje-koordinater.
*/
  if ( crdpek != NULL )
   rddat2((char *)crdpek,xhtpek->lptr_xh,4*xhtpek->nlin_xh*sizeof(DBfloat));

  return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_xhatch(
        GMXHT   *xhtpek,
        DBfloat  crdpek[],
        DBptr    la)

/*      Skriver över en existerande snitt-post.
 *
 *      In: xhtpek => Pekare till en snitt-structure.
 *          crdpek => Pekare till snittlinjer.
 *          la     => Punktens adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *              <0 => Fel från wrdat2().
 *
 *      (C)microform ab 13/12/84 J. Kjellander
 *
 *      2/1/86   Uppdatering av snittlinjer, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBstatus status;
    DBptr    la_crd;
    GMRECH  *hedpek;

/*
***Skall snittlinjerna uppdateras?
*/
    if ( crdpek != NULL )
      {
      rldat2(xhtpek->lptr_xh,4*xhtpek->nlin_xh*sizeof(DBfloat));
      status=wrdat2((char *)crdpek, &la_crd, 4*xhtpek->nlin_xh*sizeof(DBfloat));
      if ( status < 0 ) return(status);
      xhtpek->lptr_xh = la_crd;
      }
/*
***Uppdatera själva posten.
*/
    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)xhtpek, la, sizeof(GMXHT));
      break;
 
      default:
      updata( (char *)xhtpek, la, sizeof(GMXHT0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_xhatch(DBptr la)

/*      Stryker ett snitt och deallokerar allokerat minne.
 *
 *      In: la => Snittets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 10/7/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMXHT xhatch;

/*
***Läs posten.
*/
    DBread_xhatch(&xhatch,NULL,la);
/*
***Stryk snittlinjerna.
*/
    rldat2(xhatch.lptr_xh,4*xhatch.nlin_xh*sizeof(DBfloat));
/*
***Stryk själva snitt-posten.
*/
    switch ( xhatch.hed_xh.vers )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMXHT));
      break;
 
      default:
      rldat1(la,sizeof(GMXHT0));
      break;
      }

    return(0);
  }

/********************************************************/
