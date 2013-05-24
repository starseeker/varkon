/*!******************************************************************/
/*  igcsy.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  cs3ppm();    Generate csys_3p... statement                      */
/*  cs1ppm();    Generate csys_1p... statement                      */
/*  modbpm();    Genererate mode_basic.... statement                */
/*  modgpm();    Genererate mode_global.... statement               */
/*  modlpm();    Genererate mode_local.... statement                */
/*  igupcs();    Redisplay currently active coordinate system       */
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

extern short   modtyp;
extern DBptr   lsysla;
extern DBTmat *lsyspk;

/*!******************************************************/

       short cs3ppm()

/*      Genererar csys_3p.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CSYS_3P sats
 *
 *      (C)microform ab 17/2/85 J. Kjellander
 *
 *      3/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      16/11/85 t-sträng, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      1997-03-12 2D, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    char    istr[V3STRLEN+1];

/*
***Skapa 3 positioner.
*/
start:
    if ( (status=genpos(262,&exnpt2)) < 0 ) goto end;
    if ( (status=genpos(272,&exnpt3)) < 0 ) goto end;

    if ( modtyp == 3 )
      {
      if ( (status=genpos(273,&exnpt4)) < 0 ) goto end;
      }
/*
***Skapa sträng.
*/
    if ( (status=genstr(306,"",istr,&exnpt1)) < 0 ) goto end;
/*
***Länka ihop parameterlistan.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);

    if ( modtyp == 3 )
      {
      pmtcon(exnpt3,retla,&retla,&dummy);
      pmtcon(exnpt4,retla,&valparam,&dummy);
      }
    else
      {
      pmtcon(exnpt3,retla,&valparam,&dummy);
      }
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CSYS_3P",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_3P");
    errmes();
    WPerhg();
    goto start;
  }
  
/********************************************************/
/*!******************************************************/

       short cs1ppm()

/*      Genererar csys_1p.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CSYS_1P sats
 *
 *      (C)microform ab 1/10/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,exnpt5;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    char    istr[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "0.0";
/*
***Skapa position.
*/
start:
    if ( (status=genpos(262,&exnpt2)) < 0 ) goto end;
/*
***Skapa vinklar.
*/
    if ( modtyp == 3 )
      {
      if ( (status=genflt(390,dstr,istr,&exnpt3)) < 0 ) goto end;
      strcpy(dstr,istr);
      if ( (status=genflt(391,dstr,istr,&exnpt4)) < 0 ) goto end;
      strcpy(dstr,istr);
      if ( (status=genflt(392,dstr,istr,&exnpt5)) < 0 ) goto end;
      strcpy(dstr,istr);
      }
    else
      {
      if ( (status=genflt(392,dstr,istr,&exnpt5)) < 0 ) goto end;
      strcpy(dstr,istr);
      val.lit_type = C_FLO_VA;
      val.lit.float_va = 0.0;
      pmclie(&val,&exnpt3);
      pmclie(&val,&exnpt4);
      }
/*
***Skapa sträng.
*/
    if ( (status=genstr(306,"",istr,&exnpt1)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&retla,&dummy);
    pmtcon(exnpt5,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CSYS_1P",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_1P");
    errmes();
    WPerhg();
    goto start;
  }
  
/********************************************************/
/*!******************************************************/

       short modbpm()

/*      Genererar mode_basic sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5023 = Kan ej skapa mode_global sats
 *
 *      (C)microform ab 1997-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr old_la;

/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_BASIC",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_BASIC");
      goto errend;
      }
/*
***Uppdatera skärmen.
*/
    igupcs(old_la,V3_CS_NORMAL);

    return(0);
/*
***Felutgångar.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short modgpm()

/*      Genererar mode_global sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5023 = Kan ej skapa mode_global sats
 *              IG2262 = Globala systemet är redan aktivt
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      12/11/85 Koll av lsyspk, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr old_la;

/*
***Om lsyspk = NULL är det globala redan aktivt.
*/
    if ( lsyspk == NULL )
      {
      erpush("IG2262","");
      goto errend;
      }
/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_GLOBAL",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_GLOBAL");
      goto errend;
      }
/*
***Uppdatera skärmen.
*/
    igupcs(old_la,V3_CS_NORMAL);

    return(0);
/*
***Felutgångar.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short modlpm()

/*      Genererar mode_local sats.

 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: IG5023 = Kan ej skapa MODE_LOCAL sats
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      4/7/84   Felhantering, B. Doverud
 *      9/9/85   Higlight, R. Svedin
 *      30/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ;
    DBptr  old_la;
    bool   right,end;
    pm_ptr valparam,exnpt,dummy;
    short  status;

/*
***Vilken LA har nuvarande koordinatsystem ?
*/
    old_la = lsysla;
/*
***Skapa referens till planet.
*/
    typ = CSYTYP;
    if ( (status=genref(271,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcprs("MODE_LOCAL",valparam) < 0 ) goto error;
/*
***Uppdatera skärmen.
*/
    igupcs(old_la,V3_CS_NORMAL);
    igupcs(lsysla,V3_CS_ACTIVE);

exit:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","MODE_LOCAL");
    errmes();
    WPerhg();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

        short igupcs(
        DBptr la,
        int   mode)

/*      Uppdaterar grafiska fönster m.a.p.  ev.
 *      aktivt koordinatsystem.
 *
 *      In: la   = GM-pekare till koordinatsystem eller DBNULL.
 *                 Vanligen detsamma som lsysla.
 *          mode = V3_CS_NORMAL eller V3_CS_ACTIVE
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-03-11 J. Kjellander
 *
 ******************************************************!*/

  {
   DBCsys  csy;

   if ( la != DBNULL )
     {
     DBread_csys(&csy,NULL,la);
     WPupcs(&csy,la,mode,GWIN_ALL);
     }

   return(0);
   }

/********************************************************/
