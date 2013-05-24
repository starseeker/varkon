/*!******************************************************************/
/*  igline.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGlifr();     Genererate lin_free.. statement                   */
/*  IGlipr();     Genererate lin_proj.. statement                   */
/*  IGliof();     Genererate lin_offs.. statement                   */
/*  IGlipv();     Genererate lin_ang... statement                   */
/*  IGlipt();     Genererate lin_tan1... statement                  */
/*  IGli2t();     Genererate lin_tan2... statement                  */
/*  IGlipe();     Genererate lin_perp...statement                   */
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

static short linpm(char *typ);

/*!******************************************************/

       short IGlifr()

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

       short IGlipr()

/*      Huvudrutin f�r lin_proj...
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

/*      Anv�nds av IGlifr och IGlipr f�r att skapa
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
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      25/3/86  Felutg�ng, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,retla;
/*
***Skapa tv� positioner.
*/
start:
    if ( (status=IGcpos(258,&exnpt1)) < 0 ) goto end;
    if ( (status=IGcpos(259,&exnpt2)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges(typ,valparam) < 0 ) goto error;

    WPerhg();
    goto start;
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023",typ);
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGliof()

/*      Huvudrutin f�r lin_offs....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5063 = Kan ej skapa offset, fel i IGliof
 *              IG5023 = Kan ej skapa LIN_OFFS sats
 *
 *      (C)microform ab 14/1/85 J. Kjellander
 *
 *      19/6/85  Felhantering, B. Doverud
 *      4/9/85   Anrop till IGcges(), R. Svedin
 *      1/11/85  �nde och sida, J. Kjellander
 *      14/3/86  Defaultstr�ngar B. Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      24/3/86  Felutg�ng, B. Doverud
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
    if ( (status=IGcref(260,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Offset.
*/
    if ( (status=IGcflt(261,ofstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ofstr,istr);
/*
***G�r offset negativt om pekningen skedde p� v�nster sida.
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
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LIN_OFFS",valparam) < 0 ) goto error2;

    WPerhg();
    goto start;

exit:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/ 
error1:
    erpush("IG5063","");
    goto errend;

error2:
    erpush("IG5023","");

errend:
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGlipv()

/*      Huvudrutin f�r lin_ang...
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
 *      16/3/86  Defaultstr�ngar B. Doverud
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      25/3/86  Felutg�ng, B. Doverud
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
    if ( (status=IGcpos(258,&exnpt1)) < 0 ) goto exit;
/*
***Vinkel.
*/
    if ( (status=IGcflt(274,vstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(vstr,istr);
/*
***L�ngd.
*/
    if ( (status=IGcflt(275,lstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(lstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LIN_ANG",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

exit:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023","");
    WPerhg(); 
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGlipt()

/*      Huvudrutin f�r lin_tan1...
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
 *      4/9/85   Anrop till IGcges(), R. Svedin
 *      31/10/85 �nde och sida, J. Kjellander
 *      6/3/86   Defaultstr�ng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      25/3/86  Felutg�ng, B. Doverud
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
    if ( (status=IGcpos(258,&exnpt1)) < 0 ) goto exit;
/*
***Skapa referens till arc eller curve eller composite.
*/
    typ = ARCTYP+CURTYP;
    if ( (status=IGcref(268,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    if ( (status=IGcint(276,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LIN_TAN1",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

exit:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023","");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGli2t()

/*      Huvudrutin f�r lin_tan2...
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
 *      4/9/85   Anrop till IGcges(), R. Svedin
 *      31/10/85 �nde och sida, J. Kjellander
 *      6/3/86   Defaultstr�ng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      25/3/86  Felutg�ng, B. Doverud
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
    if ( (status=IGcref(268,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa 2:a referens till arc.
*/
    typ = ARCTYP;
    if ( (status=IGcref(268,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    if ( (status=IGcint(276,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LIN_TAN2",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

exit:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023","");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGlipe()

/*      Huvudrutin f�r lin_perp....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5143 = Kan ej skapa l�ngd
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
    if ( (status=IGcpos(258,&exnpt0)) < 0 ) goto exit;
/*
***Referens till en annan linje.
*/
    typ = LINTYP;
    if ( (status=IGcref(377,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***L�ngd.
*/
    if ( (status=IGcflt(275,lstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(lstr,istr);
/*
***G�r l�ngd negativ om pekningen skedde p� v�nster sida.
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
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LIN_PERP",valparam) < 0 ) goto error2;

    WPerhg();
    goto start;

exit:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/ 
error1:
    erpush("IG5143","LIN_PERP");
    goto errend;

error2:
    erpush("IG5023","LIN_PERP");

errend:
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
