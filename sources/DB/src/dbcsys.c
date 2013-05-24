/**********************************************************************
*
*    dbcsys.c
*    ========
*
*
*    This file includes the following public functions:
*
*    DBinsert_csys();   Inserts a coordinate system entity
*    DBread_csys();     Reads a coordinate system entity
*    DBupdate_csys();   Updates a coordinate system entity
*    DBdelete_csys();   Deletes a coordinate system entity
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

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_csys(
        GMCSY   *planpk,
        DBTmat  *pmat,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av plan. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      Denna version av DBinsert_csys hanterar inte plan
 *      med högre ordningsnummer än 1.
 *
 *      In: planpk => Pekare till en plan-structure.
 *          pmat   => Pekare till 4X4 matris.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till plan-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 10/2/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    planpk->hed_pl.type = CSYTYP;   /* Typ = koordinatsystem */
    planpk->hed_pl.vers = GMPOSTV1; /* Version */
/*
***Kopiera in matrisen i posten.
*/
    V3MOME((char *)pmat,(char *)&planpk->mat_pl,sizeof(DBTmat));
/*
***Lagra planet.
*/
    return(inpost((GMUNON *)planpk,idpek,lapek,sizeof(GMCSY)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_csys(
        GMCSY   *planpk,
        DBTmat  *pmat,
        DBptr    la)

/*      Huvudrutin för läsning av plan. Om pmat = NULL
 *      lagras matrisen i GMCSY-posten, annars lagras
 *      den i pmat. Med pmat = NULL blir det en pekare
 *      mindre att hålla reda på, men en extra kopiering
 *      av matrisen. Snabbast är alltså att använda pmat.
 *
 *      In: planpk => Pekare till en plan-structure.
 *          pmat   => Pekare till matris
 *          la     => Planets adress i GM.
 *
 *      Ut: *planpk => Plan-post.
 *          *pmat   => Plan-matris
 *
 *      FV: Inget.
 *
 *      (C)microform ab 10/2/85 J. Kjellander
 *
 *      28/11/91 pmat=NULL, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/
  {

    DBTmat  tmp;
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME((char *)hedpek,(char *)planpk,sizeof(GMCSY));
      if ( pmat != NULL ) V3MOME((char *)&planpk->mat_pl,
                                 (char *)pmat,sizeof(DBTmat));
      break;
/*
***Koordinatsystem av typen GMPOSTV0.
***Pga. gamla synder lagras 4X4-matrisen i GMCSY-posten på
***fel adress. "planpk+sizeof(GMRECH)+GMCNLN+1" är normalt inte
***detsamma som "&planpk->mat_pl" eftersom datorer inte gillar
***udda adresser osv. Detta är dock inte något problem om man vet
***om det och tar hänsyn till det. Vill man använda sig av 4X4-
***matrisen som finns i GMCSY-posten måste man dock först flytta
***på den till rätt adress, dvs. 1 byte framåt på en 386:a tex.
***Kopiera matrisen till planpk eller pmat.
*/
      default:
      V3MOME(hedpek,planpk,sizeof(GMCSY0));
      planpk->pcsy_pl = DBNULL;

      if ( pmat == NULL )
        {
        V3MOME((char *)planpk+sizeof(GMRECH)+JNLGTH+1,(char *)&tmp,
                 sizeof(DBTmat));
        V3MOME((char *)&tmp,(char *)&planpk->mat_pl,sizeof(DBTmat));
        }
      else
        V3MOME((char *)planpk+sizeof(GMRECH)+JNLGTH+1,(char *)pmat,
                 sizeof(DBTmat));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_csys(
        GMCSY   *planpk,
        DBTmat  *pmat,
        DBptr    la)

/*      Skriver över en existerande plan-post.
 *
 *      In: planpk => Pekare till en plan-structure.
 *          pmat   => Pekare till matris
 *          la     => Planets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av matrisen, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME((char *)pmat,(char *)&planpk->mat_pl,sizeof(DBTmat));
      updata( (char *)planpk, la, sizeof(GMCSY));
      break;
 
      default:
      V3MOME((char *)pmat,(char *)planpk+sizeof(GMRECH)+JNLGTH+1,
              sizeof(DBTmat));
      updata((char *)planpk,la,sizeof(GMCSY0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_csys(DBptr la)

/*      Stryker en plan-post och deallokerar allokerat minne. 
 *
 *      In: la => Planets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 5/5/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMCSY));
      break;
 
      default:
      rldat1(la,sizeof(GMCSY0));
      break;
      }
   return(0);
  }

/********************************************************/
