/*!******************************************************************/
/*  File: ig3.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  suropm();     Create sur_rot.. statement                        */
/*  suofpm();     Create sur_offs.. statement                       */
/*  sucypm();     Create sur_cyl...statement                        */
/*  suswpm();     Create sur_sweep..statement                       */
/*  surupm();     Create sur_ruled...statement                      */
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

        short suropm()

/*      Skapar sur_rot-sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_ROT sats
 *
 *      (C)microform ab 1997-03-24 J. Kjellander
 *
 *      1997-04-21 Riktning, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    bool    end,right;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,exnpt5,
            exnpt6,exnpt7,retla;
    char    istr[V3STRLEN+1];
    PMLITVA litval;

static char v1_str[V3STRLEN+1] ="0.0";
static char v2_str[V3STRLEN+1] ="360.0";

/*
***Referens till linje/cirkel/kurva.
*/
start:
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(459,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Två positioner.
*/
    if ( (status=genpos(460,&exnpt2)) < 0 ) goto end;
    if ( (status=genpos(461,&exnpt3)) < 0 ) goto end;
/*
***Start och slutvinkel.
*/
    if ( (status=genflt(206,v1_str,istr,&exnpt4)) < 0 ) goto end;
    strcpy(v1_str,istr);

    if ( (status=genflt(207,v2_str,istr,&exnpt5)) < 0 ) goto end;
    strcpy(v2_str,istr);
/*
***Rotation och reverse. Optionella parametrar men
***alltid med här så att man kan ändra interaktivt.
*/
    litval.lit_type = C_INT_VA;
    litval.lit.int_va = 1;
    pmclie(&litval,&exnpt6);
    pmclie(&litval,&exnpt7);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&retla,&dummy);
    pmtcon(exnpt5,retla,&retla,&dummy);
    pmtcon(exnpt6,retla,&retla,&dummy);
    pmtcon(exnpt7,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("SUR_ROT",valparam) < 0 ) goto error;
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
    erpush("IG5023","SUR_ROT");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short suofpm()

/*      Skapar sur_offs-sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_OFFS sats
 *
 *      (C)microform ab 1997-03-24 J. Kjellander
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
***Referens till en annan yta.
*/
start:
    typ = SURTYP;
    if ( (status=genref(1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Offset.
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
    if ( igcges("SUR_OFFS",valparam) < 0 ) goto error;

exit:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","SUR_OFFS");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short sucypm()

/*      Skapar sur_cyl-sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_CYL sats
 *
 *      (C)microform ab 1997-04-21 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    bool    end,right;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    char    istr[V3STRLEN+1];
    PMLITVA litval;

static char dstr[V3STRLEN+1] ="1.0";

/*
***Referens till profilkurva.
*/
start:
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(1611,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Avstånd.
*/
    if ( (status=genflt(1612,dstr,istr,&exnpt2)) < 0 ) goto end;
    strcpy(dstr,istr);
/*
***Reversering. Optionell parameter men
***alltid med här så att man kan reversera interaktivt.
*/
    litval.lit_type = C_INT_VA;
    litval.lit.int_va = 1;
    pmclie(&litval,&exnpt3);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("SUR_CYL",valparam) < 0 ) goto error;
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
    erpush("IG5023","SUR_CYL");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short suswpm()

/*      Skapar sur_sweep-sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_SWEEP sats
 *
 *      (C)microform ab 1997-04-21 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    bool    end,right;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    PMLITVA litval;

/*
***Referens till profilkurva.
*/
start:
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(1611,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Referens till styrkurva.
*/
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(1613,&typ,&exnpt2,&end,&right)) < 0 ) goto end;
/*
***Reversering. Optionell parameter men
***alltid med här så att man kan ändra interaktivt.
*/
    litval.lit_type = C_INT_VA;
    litval.lit.int_va = 1;
    pmclie(&litval,&exnpt3);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("SUR_SWEEP",valparam) < 0 ) goto error;
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
    erpush("IG5023","SUR_SWEEP");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short surupm()

/*      Skapar sur_ruled-sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_RULED sats
 *
 *      (C)microform ab 1997-04-21 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    bool    end,right;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    PMLITVA litval;

/*
***Referens till 1:a linje/cirkel/kurva.
*/
start:
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(1614,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Referens till 2:a linje/cirkel/kurva.
*/
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=genref(1615,&typ,&exnpt2,&end,&right)) < 0 ) goto end;
/*
***Reversering och parametrisering. Optionella parametrar men
***alltid med här så att man kan ändra interaktivt.
*/
    litval.lit_type = C_INT_VA;
    litval.lit.int_va = 1;
    pmclie(&litval,&exnpt3);
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
    if ( igcges("SUR_RULED",valparam) < 0 ) goto error;
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
    erpush("IG5023","SUR_RULED");
    gphgal(0);
    errmes();
    goto start;
  }

/********************************************************/
