/*!******************************************************************/
/*  igsymb.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGsymb();    Genererate symb.. statement                        */
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

        short IGsymb()

/*      Genererar symb...sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SYMBOL sats
 *
 *      (C)microform ab 21/3/86 R. Svedin
 *
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      24/3/86  Felutgångar R. Svedin
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4;
    char    istr[V3STRLEN+1];

    static char nstr[V3STRLEN+1] = "";
    static char sstr[V3STRLEN+1] = "1.0";
    static char vstr[V3STRLEN+1] = "0.0";

/*
***Skapa namn.
*/
start:
    if ( (status=IGcstr(138,nstr,istr,&exnpt1)) < 0 ) goto end;
    strcpy(nstr,istr);
/*
***Skapa position.
*/
    if ( (status=IGcpos(264,&exnpt2)) < 0 ) goto end;
/*
***Skapa storlek.
*/
    if ( (status=IGcflt(136,sstr,istr,&exnpt3)) < 0 ) goto end;
    strcpy(sstr,istr);
/*
***Skapa vridning.
*/
    if ( (status=IGcflt(17,vstr,istr,&exnpt4)) < 0 ) goto end;
    strcpy(vstr,istr);
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
    if ( IGcges("SYMB",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","");
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
