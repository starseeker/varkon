/**********************************************************************
*
*    dbbplane.c
*    ==========
*
*
*    This file includes the following public functions:
*
*    DBinsert_bplane();   Inserts a bplane entity
*    DBread_bplane();     Reads a bplane entity
*    DBupdate_bplane();   Updates a bplane entity
*    DBdelete_bplane();   Deletes a bplane entity
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

        DBstatus DBinsert_bplane(
        GMBPL   *bplpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av B-plan. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: bplpek => Pekare till en bpl-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till bpl-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *       23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    bplpek->hed_bp.type = BPLTYP;   /* Typ = B-plan */
    bplpek->hed_bp.vers = GMPOSTV1; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)bplpek,idpek,lapek,sizeof(GMBPL)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_bplane(GMBPL *bplpek, DBptr la)

/*      Läsning av Bplan-post.
 *
 *      In: bplpek => Pekare till en bpl-structure.
 *          la     => Planets adress i GM.
 *
 *      Ut: *bplpek => Bplan-post.
 *
 *      FV:   0 => Ok.
 *          < 0 => Fel från rddat1().
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *       23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,bplpek,sizeof(GMBPL));
      break;
 
      default:
      V3MOME(hedpek,bplpek,sizeof(GMBPL0));
      bplpek->pcsy_bp = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_bplane(
        GMBPL   *bplpek,
        DBptr    la)

/*      Skriver över en existerande Bplan-post.
 *
 *      In: bplpek => Pekare till en Bplan-structure.
 *          la     => Planets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *       23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)bplpek, la, sizeof(GMBPL));
      break;
 
      default:
      updata( (char *)bplpek, la, sizeof(GMBPL0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_bplane(DBptr la)

/*      Stryker ett Bplan och deallokerar allokerat minne.
 *
 *      In: la => Planets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *       23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMBPL));
      break;
 
      default:
      rldat1(la,sizeof(GMBPL0));
      break;
      }

    return(0);
  }

/********************************************************/
