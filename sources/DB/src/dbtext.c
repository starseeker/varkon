/**********************************************************************
*
*    dbtext.c
*    ========
*
*
*    This file includes the following public functions:
*
*    DBinsert_text();   Inserts a text entity
*    DBread_text();     Reads a text entity
*    DBupdate_text();   Updates a text entity
*    DBdelete_text();   Deletes a text entity
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

        DBstatus DBinsert_text(
        GMTXT   *txtpek,
        char    *strpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av text. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      Denna version av DBinsert_text hanterar inte text
 *      med högre ordningsnummer än 1.
 *
 *      In: txtpek => Pekare till en text-structure.
 *          strpek => Pekare till "null-terminated" sträng.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till text-post i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      26/12/92 GMPOSTV2, J. Kjellander
 *      1997-12-31 GMPOSTV3, J. Kjellander
 *      1998-09-30 GMPOSTV4, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr la_str;

/*
***Lagra strängen.
*/
    txtpek->nt_tx = strlen(strpek)+1;   /* Antal tecken inkl. NULL */
    if ( wrdat1(strpek,&la_str,(DBint)txtpek->nt_tx) < 0 ) return(-3);
/*
***Typ-specifika data.
*/
    txtpek->hed_tx.type = TXTTYP;       /* Typ = text */
    txtpek->hed_tx.vers = GMPOSTV4;     /* Version */
    txtpek->tptr_tx = la_str;
/*
***Lagra själva posten.
*/
    return(inpost((GMUNON *)txtpek,idpek,lapek,sizeof(GMTXT)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_text(
        GMTXT   *txtpek,
        char    *strpek,
        DBptr    la)

/*      Huvudrutin för läsning av text.
 *
 *      In: txtpek => Pekare till en text-structure.
 *          strpek => Pekare till sträng.
 *          la     => Textens GM-adress.
 *
 *      Ut: *txtpek => Text-post.
 *          *strpek => Sträng.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 *      15/3/92 strpek = NULL, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      26/12/92 font, J. Kjellander
 *      1997-12-31 GMPOSTV3, J. Kjellander
 *      1998-09-30 GMPOSTV4, J. Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV4:
      V3MOME(hedpek,txtpek,sizeof(GMTXT));
      break;
 
      case GMPOSTV3:
      V3MOME(hedpek,txtpek,sizeof(GMTXT2));
      txtpek->pmod_tx = 0;
      break;
 
      case GMPOSTV2:
      V3MOME(hedpek,txtpek,sizeof(GMTXT1));
      txtpek->wdt_tx = 0.0;
      txtpek->pmod_tx = 0;
      break;
 
      case GMPOSTV1:
      V3MOME(hedpek,txtpek,sizeof(GMTXT1));
      txtpek->fnt_tx = 0;
      txtpek->wdt_tx = 0.0;
      txtpek->pmod_tx = 0;
      break;
 
      default:
      V3MOME(hedpek,txtpek,sizeof(GMTXT0));
      txtpek->fnt_tx  = 0;
      txtpek->wdt_tx  = 0.0;
      txtpek->pcsy_tx = DBNULL;
      txtpek->pmod_tx = 0;
      break;
      }
/*
***Läs strängen.
*/
    if ( strpek != NULL )
      rddat1(strpek,txtpek->tptr_tx,(DBint)txtpek->nt_tx);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_text(
        GMTXT   *txtpek,
        char    *strpek,
        DBptr    la)

/*      Skriver över en existerande text-post. Om 
 *      strpek == NULL uppdateras ej strängen.
 *
 *      In: txtpek => Pekare till en text-structure.
 *          strpek => Pekare till sträng.
 *          la     => Textens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      27/12/85 Uppdatering av sträng, J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      1997-12-05 GMPOSTV2, J.Kjellander
 *      1997-12-31 GMPOSTV3, J. Kjellander
 *      1998-09-30 GMPOSTV4, J. Kjellander
 *
 ******************************************************!*/

  {
    char    oldstr[V3STRLEN+1];
    DBptr   la_str;
    GMTXT   oldtxt;
    GMRECH *hedpek;

/*
***Ev. uppdatering av strängen.
*/
    if ( strpek != NULL )
      {
      DBread_text(&oldtxt,oldstr,la);
      rldat1(oldtxt.tptr_tx,(DBint)oldtxt.nt_tx);
      txtpek->nt_tx = strlen(strpek)+1;
      if ( wrdat1(strpek,&la_str,(DBint)txtpek->nt_tx) < 0 ) return(-3);
      txtpek->tptr_tx = la_str;
      }
/*
***Uppdatering av själva text-posten.
*/
    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV4:
      updata((char *)txtpek,la,sizeof(GMTXT));
      break;
 
      case GMPOSTV3:
      updata((char *)txtpek,la,sizeof(GMTXT2));
      break;
 
      case GMPOSTV2:
      case GMPOSTV1:
      updata((char *)txtpek,la,sizeof(GMTXT1));
      break;
 
      default:
      updata((char *)txtpek,la,sizeof(GMTXT0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_text(DBptr la)

/*      Stryker en text-post och deallokerar allokerat minne.
 *
 *      In: la => Text-postens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 5/5/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      1997-12-05 GMPOSTV2, J.Kjellander
 *      1997-12-31 GMPOSTV3, J. Kjellander
 *      1998-09-30 GMPOSTV4, J. Kjellander
 *
 ******************************************************!*/

  {
    GMTXT  text;
    char   str[V3STRLEN+1];

/*
***Läs text-posten.
*/
    DBread_text(&text,str,la);
/*
***Stryk själva text-posten.
*/
    switch ( text.hed_tx.vers )
      {
      case GMPOSTV4:
      rldat1(la,sizeof(GMTXT));
      break;
 
      case GMPOSTV3:
      rldat1(la,sizeof(GMTXT2));
      break;
 
      case GMPOSTV2:
      case GMPOSTV1:
      rldat1(la,sizeof(GMTXT1));
      break;
 
      default:
      rldat1(la,sizeof(GMTXT0));
      break;
      }
/*
***Stryk textsträngen.
*/
    rldat1(text.tptr_tx,(DBint)text.nt_tx);

    return(0);
  }

/********************************************************/
