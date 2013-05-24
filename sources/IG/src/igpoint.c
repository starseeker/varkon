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
/*  URL:  http://varkon.sourceforge.net                             */
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

/********************************************************/

       short IGpofr()

/*      Create poi_free... statement
 *
 *      Return: 0      = OK
 *              REJECT = Operation cancelled
 *              GOMAIN = Main menu
 *
 *      (C)2007-11-11 J.Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_FREE"));
  }

/********************************************************/
/********************************************************/

       short IGpopr()

/*      Create poi_proj... statement
 *
 *      Return: 0      = OK
 *              REJECT = Operation cancelled
 *              GOMAIN = Main menu
 *
 *      (C)2007-11-11 J.Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_PROJ"));
  }

/********************************************************/
/********************************************************/

 static short poipm(char *typ)

/*      Create point.
 *
 *      In: typ = "POI_FREE" or "POI_PROJ"
 *
 *      Return: 0      = OK
 *              REJECT = Operation cancelled
 *              GOMAIN = Main menu
 *
 *      Error: IG5023 = Can't create point statement
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
 *      2007-11-11 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  exnpt,dummy;

/*
***Create position.
*/
start:
    if ( (status=IGcpos(71,&exnpt)) < 0 ) goto end;
/*
***Create parameter list.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Create statement.
*/
    if ( IGcges(typ,valparam) < 0 ) goto error;

    WPerhg();
    goto start;
/*
***The end.
*/
end:
    WPerhg();
    return(status);
/*
***Error exit.
*/
error:
    erpush("IG5023",typ);
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
