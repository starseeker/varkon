/*!******************************************************************/
/*  igpoint.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGpofr();     Generate poi_free.. statement                     */
/*  IGpopr();     Generate poi_proj.. statement                     */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"

static short poipm(char *typ);

/*!******************************************************/

       short IGpofr()

/*      Genererar poi_free... sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Inga.
 *
 *      (C)microform ab  19/11/85 J. Kjellander
 *
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_FREE"));
  }

/********************************************************/
/*!******************************************************/

       short IGpopr()

/*      Genererar poi_proj... sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Inga.
 *
 *      (C)microform ab  19/11/85 J. Kjellander
 *
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_PROJ"));
  }

/********************************************************/
/*!******************************************************/

 static short poipm(char *typ)

/*      Huvudrutin för poi_free.....
 *
 *      In: typ = "POI_FREE" eller "POI_PROJ"
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa POI_FREE sats
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      2/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      19/11/85 Slagit ihop free och proj, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  IGcpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  exnpt,dummy;

/*
***Skapa position.
*/
start:
    if ( (status=IGcpos(264,&exnpt)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges(typ,valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023",typ);
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
