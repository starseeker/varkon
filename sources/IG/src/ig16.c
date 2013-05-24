/*!******************************************************************/
/*  File: ig16.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  grppm();     Genererate group.. statement                       */
/*  comppm();    Genererate cur_comp.. statement                    */
/*  curopm();    Genererate cur_offs.. statement                    */
/*  curipm();    Genererate cur_int...statement                     */
/*  symbpm();    Genererate symb.. statement                        */
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

/*!******************************************************/

       short grppm()

/*      Huvudrutin för group(#id,name,ref1,ref2,,,,)
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5022 = Kan ej skapa GROUP sats
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      6/9/85   Anrop till igcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      16/11/85 t-sträng, J. Kjellander
 *      16/11/85 Sträng sist, J. Kjellander
 *      4/12/85  Tagit bort loop, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      24/3/86  Felutgångar B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   i,status,nref;
    pm_ptr  exnpt[GMMXGP];
    pm_ptr  retla,dummy,exnpt1;
    DBetype   typ;
    bool    end,right;
    char    istr[V3STRLEN+1];

/*
***Gruppens medlemmar.
*/
    nref = 0;

    while ( nref < GMMXGP )
      {
      typ = ALLTYP;
      if ( (status=genref(268,&typ,&exnpt[nref],&end,&right)) == REJECT) break;
      if (status == GOMAIN ) goto exit;
      ++nref;
      }
/*
***Gruppens namn.
*/
    if ( (status=genstr(318,"",istr,&exnpt1)) < 0 ) goto exit;
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
/*
***Länka ihop parameterlistan.
*/
    for ( i=0; i<nref; ++i )
      pmtcon(exnpt[i],retla,&retla,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("GROUP",retla) < 0 ) goto error;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","");
    errmes();
    goto exit;
  }

/********************************************************/
/*!******************************************************/

       short comppm()

/*      Huvudrutin för cur_comp(#id,ref1,ref2,,,,)
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CUR_COMP sats
 *
 *      (C)microform ab 20/8/85 J. Kjellander
 *
 *      6/9/85   Anrop till igcges(), R. Sviden
 *      31/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      24/3/86  Felutgångar B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      15/11/91 CUR_COM, J.Kjellander
 *      15/6/93  GMMXSG->V2PARMAX, J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt,retla,dummy;
    short   nref,status;
    DBetype   typ;
    bool    end,right;

/*
***Referenser.
*/
    retla = (pm_ptr)NULL;
    nref = 0;

    while ( nref < V2PARMAX )
      {
      typ = LINTYP+ARCTYP+CURTYP;
      if ( (status=genref(268,&typ,&exnpt,&end,&right)) == REJECT ) break;
      if ( status == GOMAIN ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nref;
      }

    if ( nref == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CUR_COMP",retla) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }

exit:
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short curopm()

/*      Huvudrutin för cur_offs.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_OFFS sats
 *
 *      (C)microform ab 15/11/91 J. Kjellander
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
***Skapa referens till annan kurva.
*/
    typ = CURTYP;
    if ( (status=genref (430,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa offset.
*/
    if ( (status=genflt(278,ofstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ofstr,istr);
/*
***Gör offset negativt om pekningen skedde på vänster sida.
*/
    if ( !right ) pmcune(PM_MINUS,exnpt2,&exnpt2);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CUR_OFFS",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }

exit:
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short curipm()

/*      Huvudrutin för cur_int.....
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_INT sats
 *
 *      (C)microform ab 1997-04-21 J. Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
/*
***Skapa referens till yta.
*/
    typ = SURTYP;
    if ( (status=genref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa referens till plan.
*/
    typ = CSYTYP+BPLTYP;
    if ( (status=genref (1609,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Kurvgren.
*/
    if ( (status=genint(1610,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CUR_INT",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }

exit:
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short symbpm()

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
 *      23/3/86  genpos(pnr,  B. Doverud
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
    if ( (status=genstr(138,nstr,istr,&exnpt1)) < 0 ) goto end;
    strcpy(nstr,istr);
/*
***Skapa position.
*/
    if ( (status=genpos(264,&exnpt2)) < 0 ) goto end;
/*
***Skapa storlek.
*/
    if ( (status=genflt(136,sstr,istr,&exnpt3)) < 0 ) goto end;
    strcpy(sstr,istr);
/*
***Skapa vridning.
*/
    if ( (status=genflt(17,vstr,istr,&exnpt4)) < 0 ) goto end;
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
    if ( igcges("SYMB",valparam) < 0 ) goto error;

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
    errmes();
    gphgal(0);
    goto start;
  }

/********************************************************/
