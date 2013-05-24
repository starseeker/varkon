/*!******************************************************************/
/*  igtform.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  tfmopm();    Genererate tform_move... statement                 */
/*  tfropm();    Genererate tform_rotl... statement                 */
/*  tfmipm();    Genererate tform_mirr... statement                 */
/*  tcpypm();    Genererate tcopy...      statement                 */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"

/*!******************************************************/

       short tfmopm()

/*      Huvudrutin för tform_move...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_MOVE  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,retla;
/*
***Skapa start punkt.
*/
    if ( (status=genpos( 525,&exnpt1)) < 0 ) goto exit;
/*
***Skapa slut punkt.
*/
    if ( (status=genpos( 524,&exnpt2)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("TFORM_MOVE",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short tfropm()

/*      Huvudrutin för tform_rotl...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_ROTL  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
    char        istr[V3STRLEN+1];
    static char rotstr[V3STRLEN+1] ="0.0";
/*
***Skapa start punkt rotationslinje.
*/
    if ( (status=genpos( 523,&exnpt1)) < 0 ) goto exit;
/*
***Skapa slut punkt rotationslinje.
*/
    if ( (status=genpos( 522,&exnpt2)) < 0 ) goto exit;
/*
***Skapa rotationsvinkel.
*/
    if ( (status=genflt(17,rotstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(rotstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("TFORM_ROTL",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short tfmipm()

/*      Huvudrutin för tform_mirr...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TFORM_MIRR  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *      1998-04-16 t554-556, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
/*
***Skapa punkt 1 i speglingsplan.
*/
    if ( (status=genpos( 554,&exnpt1)) < 0 ) goto exit;
/*
***Skapa punkt 2 i speglingsplan.
*/
    if ( (status=genpos( 555,&exnpt2)) < 0 ) goto exit;
/*
***Skapa punkt 3 i speglingsplan.
*/
    if ( (status=genpos( 556,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("TFORM_MIRR",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }
/********************************************************/
/*!******************************************************/

       short tcpypm()

/*      Huvudrutin för tcopy...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TCOPY  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,retla;
    DBetype       typ;
    bool        end,right;
/*
***Skapa referens till storhet.
*/
    typ = ALLTYP;
    if ( (status=genref (268,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa referens till transformationsmatris.
*/
    typ = TRFTYP;
    if ( (status=genref ( 520,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("TCOPY",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
