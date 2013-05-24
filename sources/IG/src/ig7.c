/*!******************************************************************/
/*  File: ig7.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  ar1ppm();    Genererate arc_1pos.. statement                    */
/*  ar2ppm();    Genererate arc_2pos.. statement                    */
/*  ar3ppm();    Genererate arc_3pos.. statement                    */
/*  arofpm();    Genererate arc_offs.. statement                    */
/*  arflpm();    Genererate arc_fil... statement                    */
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

static char   rdstr[V3STRLEN+1] ="";

/*!******************************************************/

       short ar1ppm()

/*      Huvudrutin för arc_1pos...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ARC_1POS sats
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      20/6/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      3/12/85  Bytt namn till arc_1pos, J. Kjellander
 *      6/3/86   Defaultsträngar, B.Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    char    istr[V3STRLEN+1];

/*
***Skapa position.
*/
start:
    if ( (status=genpos (262,&exnpt1)) < 0 ) goto end;
/*
***Radie, start och slutvinkel.
*/
    if ( (status=genflt(205,rdstr,istr,&exnpt2)) < 0 ) goto end;
    strcpy(rdstr,istr);

    if ( (status=genflt(206,"0.0",istr,&exnpt3)) < 0 ) goto end;

    if ( (status=genflt(207,"360.0",istr,&exnpt4)) < 0 ) goto end;
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
    if ( igcges("ARC_1POS",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

end:
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

       short ar2ppm()

/*      Huvudrutin för arc_2pos.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ARC_2POS sats
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *      20/6/85  Felrutiner, B. Doverud 
 *      4/9/85   Anrop till skapa sats, R. Svedin      
 *      6/3/86   Defaultsträngar, B.Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      25/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    char    istr[V3STRLEN+1];

/*
***Skapa 2 positioner.
*/
start:
    if ( (status=genpos (258,&exnpt1)) < 0 ) goto end;
    if ( (status=genpos (259,&exnpt2)) < 0 ) goto end;
/*
***Skapa radie.
*/
    if ( (status=genflt(205,rdstr,istr,&exnpt3)) < 0 ) goto end;
    strcpy(rdstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("ARC_2POS",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

end:
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

       short ar3ppm()

/*      Huvudrutin för arc_3pos.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ARC_3POS sats
 *
 *      (C)microform ab 17/1/85 J. Kjellander
 *
 *      20/6/85  Felrutiner, B. Doverud 
 *      4/9/85   Anrop till skapa sats, R. Svedin  
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;

/*
***Skapa 3 positioner.
*/
start:
    if ( (status=genpos (258,&exnpt1)) < 0 ) goto end;
    if ( (status=genpos (263,&exnpt2)) < 0 ) goto end;
    if ( (status=genpos (259,&exnpt3)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("ARC_3POS",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

end:
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

       short arofpm()

/*      Huvudrutin för arc_offs.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ARC_OFFS sats
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      15/7/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      6/3/86   Defaultsträngar, B.Doverud
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
***Skapa referens till annan arc.
*/
start:
    typ = ARCTYP;
    if ( (status=genref (277,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa offset.
*/
    if ( (status=genflt(278,ofstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ofstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("ARC_OFFS",valparam) < 0 ) goto error;

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

       short arflpm()

/*      Huvudrutin för arc_fil.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ARC_FIL sats
 *
 *      (C)microform ab 12/7/85 J. Kjellander
 *
 *      15/7/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      16/11/85 Alt via pekning, J. Kjellander
 *      6/3/86   Defaultsträngar, B.Doverud
 *      20/3/86  Anrop till pmtcon pmclie, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype   typ;
    bool    end,right1,right2;
    short   alt=0,status;
    PMLITVA litval;
    char    istr[V3STRLEN+1];

/*
***Skapa referens till första storhet.
*/
start:
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref (332,&typ,&exnpt1,&end,&right1)) < 0 ) goto exit;
/*
***Skapa referens till andra storhet.
*/
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref (333,&typ,&exnpt2,&end,&right2)) < 0 ) goto exit;
/*
***Skapa radie.
*/
    if ( (status=genflt(205,rdstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(rdstr,istr);
/*
***Avgör med hjälp av pekningarna vilket alternativ som önskas.
*/
    if (  right1 &&  right2 ) alt = 1;
    if ( !right1 &&  right2 ) alt = 2;
    if (  right1 && !right2 ) alt = 3;
    if ( !right1 && !right2 ) alt = 4;

    litval.lit_type = C_INT_VA;
    litval.lit.int_va = alt;
    pmclie(&litval,&exnpt4);
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
    if ( igcges("ARC_FIL",valparam) < 0 ) goto error;

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
