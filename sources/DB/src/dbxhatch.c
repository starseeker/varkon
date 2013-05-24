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

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_xhatch(
        DBHatch *xhtpek,
        DBfloat *crdpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Insert a new hatch into the DB.
 *
 *      In: xhtpek => C ptr to hatch record.
 *          crdpek => C ptr to hatch line coordinates.
 *          idpek  => C ptr to hatch ID.
 *
 *      Out: *la   => Hatch address in DB.
 *
 *      Return:  0 => Ok.
 *              -1 => ID outside virtual area.
 *              -2 => IDTAB full.
 *              -3 => No space.
 *              -4 => Entity with this ID already exists.
 *
 *      (C)microform ab 10/7/84 J.Kjellander
 *
 *      14/10/85    Header data, J.Kjellander
 *      22/3/92     GMPOSTV1, J.Kjellander
 *      2007-09-01  GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la_crd;
    DBstatus status;

/*
***Save hatch lines.
*/
    status=wrdat2((char *)crdpek, &la_crd, 4*xhtpek->nlin_xh*sizeof(DBfloat));
    if ( status < 0 ) return(status);
/*
***Add meta data.
*/
    xhtpek->hed_xh.type = XHTTYP;
    xhtpek->hed_xh.vers = GMPOSTV2;
    xhtpek->lptr_xh = la_crd;
/*
***Save hatch record.
*/
    return(inpost((GMUNON *)xhtpek,idpek,lapek,sizeof(DBHatch)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_xhatch(
        DBHatch *xhtpek,
        DBfloat *crdpek,
        DBCsys  *csyptr,
        DBptr    la)

/*      Read a hatch entity from DB.
 *
 *      In: xhtpek => C ptr to recieve hatch data.
 *          crdpek => C ptr to recieve hatch line
 *                    coordinates or NULL.
 *          csyptr => C ptr to recieve coordinate
 *                    system data or NULL.
 *          la     => Hatch address in DB.
 *
 *      Out: *xhtpek => Hatch data.
 *           *crdpek => Hatch line coordinates (2D).
 *           *csyptr => Coordinate system data.
 *
 *      Return: 0
 *
 *      (C)microform ab 10/7/84 J.Kjellander
 *
 *      17/3/88     crdpek=NULL, J.Kjellander
 *      22/3/92     GMPOSTV1, J.Kjellander
 *      2007-09-01  GMPOSTV2, J.Kjellander
 *      2007-09-30  3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedpek;

/*
***Read the hatch record.
*/
    hedpek = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      V3MOME(hedpek,xhtpek,sizeof(DBHatch));
      if ( csyptr != NULL  &&  xhtpek->pcsy_xh > 0 ) DBread_csys(csyptr,NULL,xhtpek->pcsy_xh);
      break;

      case GMPOSTV1:
      V3MOME(hedpek,xhtpek,sizeof(GMXHT1));
      xhtpek->wdt_xh = 0.0;
      break;

      default:
      V3MOME(hedpek,xhtpek,sizeof(GMXHT0));
      xhtpek->pcsy_xh = DBNULL;
      xhtpek->wdt_xh = 0.0;
      break;
      }
/*
***Read hatchline coordinates.
*/
  if ( crdpek != NULL )
   rddat2((char *)crdpek,xhtpek->lptr_xh,4*xhtpek->nlin_xh*sizeof(DBfloat));
/*
***The end.
*/
  return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_xhatch(
        DBHatch *xhtpek,
        DBfloat  crdpek[],
        DBptr    la)

/*      Update existing hatch entity.
 *
 *      In: xhtpek => C ptr to hatch record.
 *          crdpek => C ptr to hatch line coordinates.
 *          la     => Hatch address in DB.
 *
 *      Ut: Inget.
 *
 *      Return:  0 => Ok.
 *              <0 => Error from wrdat2().
 *
 *      (C)microform ab 13/12/84 J.Kjellander
 *
 *      2/1/86      Uppdatering av snittlinjer, J.Kjellander
 *      22/3/92     GMPOSTV1, J.Kjellander
 *      2007-09-01  GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBstatus  status;
    DBptr     la_crd;
    DBHeader *hedpek;

/*
***Optionally update the hatch lines.
*/
    if ( crdpek != NULL )
      {
      rldat2(xhtpek->lptr_xh,4*xhtpek->nlin_xh*sizeof(DBfloat));
      status=wrdat2((char *)crdpek, &la_crd, 4*xhtpek->nlin_xh*sizeof(DBfloat));
      if ( status < 0 ) return(status);
      xhtpek->lptr_xh = la_crd;
      }
/*
***Updtate hatch record.
*/
    hedpek = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      updata( (char *)xhtpek, la, sizeof(DBHatch));
      break;

      case GMPOSTV1:
      updata( (char *)xhtpek, la, sizeof(GMXHT1));
      break;

      default:
      updata( (char *)xhtpek, la, sizeof(GMXHT0));
      break;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_xhatch(DBptr la)

/*      Delete a hatch from DB.
 *
 *      In: la => Hatch address in DB.
 *
 *      Return:  0  => Ok.
 *
 *      (C)microform ab 10/7/85 J.Kjellander
 *
 *      16/10/85    Koll av referensräknare, J.Kjellander
 *      22/3/92     GMPOSTV1, J.Kjellander
 *      2007-09-01  GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHatch xhatch;

/*
***Read hatch record.
*/
    DBread_xhatch(&xhatch,NULL,NULL,la);
/*
***Delete hatch lines.
*/
    rldat2(xhatch.lptr_xh,4*xhatch.nlin_xh*sizeof(DBfloat));
/*
***Delete the hatch record.
*/
    switch ( xhatch.hed_xh.vers )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBHatch));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMXHT1));
      break;

      default:
      rldat1(la,sizeof(GMXHT0));
      break;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
