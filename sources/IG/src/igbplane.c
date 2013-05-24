/*!******************************************************************/
/*  igbplane.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGbpln();     Generate b_plane... statement                     */
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

/*!******************************************************/

       short IGbpln()

/*      Genererar b_plan.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa B_PLANE sats
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    pm_ptr exnpt1,exnpt2,exnpt3,exnpt4;
    pm_ptr valparam;
    pm_ptr retla,dummy;

/*
***Skapa 4 positioner.
*/
start:
    if ( (status=IGcpos(384,&exnpt1)) < 0 ) goto end;
    if ( (status=IGcpos(385,&exnpt2)) < 0 ) goto end;
    if ( (status=IGcpos(386,&exnpt3)) < 0 ) goto end;
    if ( (status=IGcpos(387,&exnpt4)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("B_PLANE",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","B_PLANE");
    errmes();
    WPerhg();
    goto start;
  }
  
/********************************************************/
