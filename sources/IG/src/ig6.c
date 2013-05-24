/*!******************************************************************/
/*  File: ig6.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  lifrpm();     Genererate lin_free.. statement                   */
/*  liprpm();     Genererate lin_proj.. statement                   */
/*  liofpm();     Genererate lin_offs.. statement                   */
/*  lipvpm();     Genererate lin_ang... statement                   */
/*  liptpm();     Genererate lin_tan1... statement                  */
/*  li2tpm();     Genererate lin_tan2... statement                  */
/*  lipepm();     Genererate lin_perp...statement                   */
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
#include "../../GP/include/GP.h"

static short linpm(char *typ);

/*!******************************************************/

       short lifrpm()

/*      Huvudrutin för lin_free...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Se linpm().
 *
 *      (C)microform ab 19/3/86 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
     return(linpm("LIN_FREE"));
  }

/********************************************************/
/*!******************************************************/

       short liprpm()

/*      Huvudrutin för lin_proj...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Se linpm().
 *
 *      (C)microform ab 19/3/86 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
     return(linpm("LIN_PROJ"));
  }

/********************************************************/
/*!******************************************************/

 static short linpm(char *typ)

/*      Används av lifrpm och liprpm för att skapa
 *      linje-sats.
 *
 *      In: typ = Typ av linje.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa lin_free sats
 *
 *      (C)microform ab 19/3/86 J. Kjellander
 *
 *      23/3/86  genpos(pnr,  B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,retla;
/*
***Skapa två positioner.
*/
start:
    if ( (status=genpos(258,&exnpt1)) < 0 ) goto end;
    if ( (status=genpos(259,&exnpt2)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges(typ,valparam) < 0 ) goto error;

    gphgal(0);
    goto start;
/*
***Slut.
*/
end:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023",typ);
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short liofpm()

/*      Huvudrutin för lin_offs....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5063 = Kan ej skapa offset, fel i liofpm
 *              IG5023 = Kan ej skapa LIN_OFFS sats
 *
 *      (C)microform ab 14/1/85 J. Kjellander
 *
 *      19/6/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      1/11/85  Ände och sida, J. Kjellander
 *      14/3/86  Defaultsträngar B. Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char ofstr[V3STRLEN+1] ="";

/*
***Referens till en annan linje.
*/
start:
    typ = LINTYP;
    if ( (status=genref(260,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Offset.
*/
    if ( (status=genflt(261,ofstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ofstr,istr);
/*
***Gör offset negativt om pekningen skedde på vänster sida.
*/
    if ( !right )
      {
      if ( (status=pmcune(PM_MINUS,exnpt2,&exnpt2)) < 0 ) goto error1;
      }
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LIN_OFFS",valparam) < 0 ) goto error2;

    gphgal(0);
    goto start;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/ 
error1:
    erpush("IG5063","");
    goto errend;

error2:
    erpush("IG5023","");

errend:
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short lipvpm()

/*      Huvudrutin för lin_ang...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa LIN_ANG sats
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      15/7/85  Felrutiner B. Doverud    
 *      4/9/85   Anrop till skapa sats R. Svedin     
 *      16/3/86  Defaultsträngar B. Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  genpos(pnr,  B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short  status;
    pm_ptr valparam,dummy;
    pm_ptr exnpt1,exnpt2,exnpt3,retla;
    char   istr[V3STRLEN+1];

    static char vstr[V3STRLEN+1] ="0.0";
    static char lstr[V3STRLEN+1] ="";

/*
***Skapa position.
*/
start:
    if ( (status=genpos(258,&exnpt1)) < 0 ) goto exit;
/*
***Vinkel.
*/
    if ( (status=genflt(274,vstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(vstr,istr);
/*
***Längd.
*/
    if ( (status=genflt(275,lstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(lstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LIN_ANG",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","");
    gphgal(0); 
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short liptpm()

/*      Huvudrutin för lin_tan1...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa LIN_TAN1 sats
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      15/7/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  genpos(pnr,  B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    DBetype   typ;
    bool    end,right;
    char    istr[V3STRLEN+1];

/*
***Skapa position.
*/
start:
    if ( (status=genpos(258,&exnpt1)) < 0 ) goto exit;
/*
***Skapa referens till arc eller curve eller composite.
*/
    typ = ARCTYP+CURTYP;
    if ( (status=genref(268,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    if ( (status=genint(276,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LIN_TAN1",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short li2tpm()

/*      Huvudrutin för lin_tan2...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa LIN_TAN2 sats
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      15/7/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    DBetype   typ;
    bool    end,right;
    char    istr[V3STRLEN+1];

/*
***Skapa 1:a referens till arc.
*/
start:
    typ = ARCTYP;
    if ( (status=genref(268,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa 2:a referens till arc.
*/
    typ = ARCTYP;
    if ( (status=genref(268,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    if ( (status=genint(276,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LIN_TAN2",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short lipepm()

/*      Huvudrutin för lin_perp....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5143 = Kan ej skapa längd
 *              IG5023 = Kan ej skapa LIN_PERP sats
 *
 *      (C)microform ab 25/4/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt0,exnpt1,exnpt2,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];

    static char lstr[V3STRLEN+1] ="";

/*
***Startposition.
*/
start:
    if ( (status=genpos(258,&exnpt0)) < 0 ) goto exit;
/*
***Referens till en annan linje.
*/
    typ = LINTYP;
    if ( (status=genref(377,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Längd.
*/
    if ( (status=genflt(275,lstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(lstr,istr);
/*
***Gör längd negativ om pekningen skedde på vänster sida.
*/
    if ( !right )
      {
      if ( (status=pmcune(PM_MINUS,exnpt2,&exnpt2)) < 0 ) goto error1;
      }
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt0,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt1,retla,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LIN_PERP",valparam) < 0 ) goto error2;

    gphgal(0);
    goto start;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/ 
error1:
    erpush("IG5143","LIN_PERP");
    goto errend;

error2:
    erpush("IG5023","LIN_PERP");

errend:
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
