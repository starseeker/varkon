/*!******************************************************************/
/*  igsurf.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGsuro();     Create sur_rot.. statement                        */
/*  IGsuof();     Create sur_offs.. statement                       */
/*  IGsucy();     Create sur_cyl...statement                        */
/*  IGsusw();     Create sur_sweep..statement                       */
/*  IGsuru();     Create sur_ruled...statement                      */
/*  IGsurt();     Create sur_trim...   statement                    */
/*  IGsura();     Create sur_approx... statement                    */
/*  IGsuco();     Create sur_comp...   statement                    */
/*  IGsuex();     Create sur_exdef...  statement                    */
/*  IGsulo();     Create sur_conic...  statement                    */
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

/*!******************************************************/

        short IGsuro()

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
    if ( (status=IGcref(459,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Två positioner.
*/
    if ( (status=IGcpos(460,&exnpt2)) < 0 ) goto end;
    if ( (status=IGcpos(461,&exnpt3)) < 0 ) goto end;
/*
***Start och slutvinkel.
*/
    if ( (status=IGcflt(206,v1_str,istr,&exnpt4)) < 0 ) goto end;
    strcpy(v1_str,istr);

    if ( (status=IGcflt(207,v2_str,istr,&exnpt5)) < 0 ) goto end;
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
    if ( IGcges("SUR_ROT",valparam) < 0 ) goto error;
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023","SUR_ROT");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGsuof()

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
    if ( (status=IGcref(1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Offset.
*/
    if ( (status=IGcflt(278,ofstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ofstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_OFFS",valparam) < 0 ) goto error;

exit:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","SUR_OFFS");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGsucy()

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
    if ( (status=IGcref(1611,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Avstånd.
*/
    if ( (status=IGcflt(1612,dstr,istr,&exnpt2)) < 0 ) goto end;
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
    if ( IGcges("SUR_CYL",valparam) < 0 ) goto error;
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","SUR_CYL");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGsusw()

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
    if ( (status=IGcref(1611,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Referens till styrkurva.
*/
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=IGcref(1613,&typ,&exnpt2,&end,&right)) < 0 ) goto end;
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
    if ( IGcges("SUR_SWEEP",valparam) < 0 ) goto error;
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","SUR_SWEEP");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGsuru()

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
    if ( (status=IGcref(1614,&typ,&exnpt1,&end,&right)) < 0 ) goto end;
/*
***Referens till 2:a linje/cirkel/kurva.
*/
    typ = LINTYP+ARCTYP+CURTYP;
    if ( (status=IGcref(1615,&typ,&exnpt2,&end,&right)) < 0 ) goto end;
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
    if ( IGcges("SUR_RULED",valparam) < 0 ) goto error;
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","SUR_RULED");
    WPerhg();
    errmes();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGsurt()

/*      Huvudrutin för sur_trim...  
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa SUR_TRIM sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,exnpt4,exnpt5,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char ustastr[V3STRLEN+1] ="0.0";
    static char uendstr[V3STRLEN+1] ="1.0";
    static char vstastr[V3STRLEN+1] ="0.0";
    static char vendstr[V3STRLEN+1] ="1.0";
/*
***Skapa referens till yta.
*/
    typ = SURTYP;
    if ( (status=IGcref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa start U parametervärde.
*/
    if ( (status=IGcflt( 539,ustastr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ustastr,istr);
/*
***Skapa slut  U parametervärde.
*/
    if ( (status=IGcflt( 540,uendstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(uendstr,istr);
/*
***Skapa start V parametervärde.
*/
    if ( (status=IGcflt( 541,vstastr,istr,&exnpt4)) < 0 ) goto exit;
    strcpy(vstastr,istr);
/*
***Skapa slut  V parametervärde.
*/
    if ( (status=IGcflt( 542,vendstr,istr,&exnpt5)) < 0 ) goto exit;
    strcpy(vendstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,      &retla,  &dummy);
    pmtcon(exnpt4,  retla,      &retla,  &dummy);
    pmtcon(exnpt5,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_TRIM",valparam) < 0 )
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

       short IGsura()

/*      Huvudrutin för sur_approx...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa SUR_APPROX sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *      1998-04-17 t547->t521, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char typstr[V3STRLEN+1] ="CUB_SUR";
    static char metstr[V3STRLEN+1] ="1";
    static char nosstr[V3STRLEN+1] ="4";
    static char tolstr[V3STRLEN+1] ="0.05";
/*
***Skapa referens till yta.
*/
    typ = SURTYP;
    if ( (status=IGcref(1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa typ av utyta.
*/
    if ( (status=IGcstr(521,typstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Skapa metod för approximation.
*/
    if ( (status=IGcint(546,metstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(metstr,istr);
/*
***Skapa antal ytlappar eller koordinat tolerans.
*/
    if ( strcmp(metstr,"1") == 0 )
      {
      if ( (status=IGcint(538,nosstr,istr,&exnpt4)) < 0 ) goto exit;
      strcpy(nosstr,istr);
      }
    else
      {
      if ( (status=IGcflt(544,tolstr,istr,&exnpt4)) < 0 ) goto exit;
      strcpy(tolstr,istr);
      }
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,      &retla,  &dummy);
    pmtcon(exnpt4,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_APPROX",valparam) < 0 )
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

       short IGsuco()

/*      Huvudrutin för sur_comp(#id,ref1,ref2,,,,)
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_COMP sats
 *
 *      (C)microform ab 10/02/98 G. Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
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
      typ = SURTYP;
      if ( (status=IGcref(1608,&typ,&exnpt,&end,&right)) == REJECT ) break;
      if ( status == GOMAIN ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nref;
      }

    if ( nref == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_COMP",retla) < 0 )
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

       short IGsuex()

/*      Huvudrutin för sur_exdef...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa SUR_EXDEF sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *      1998-04-17 t547->t557, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
    char        istr[V3STRLEN+1];

 static char filstr[V3STRLEN+1] ="";
 static char forstr[V3STRLEN+1] ="VDA-FS";
 static char typstr[V3STRLEN+1] ="POL_SUR";
/*
***Skapa filnamn.
*/
    if ( (status=IGcstr(537,filstr,istr,&exnpt1)) < 0 ) goto exit;
    strcpy(filstr,istr);
/*
***Skapa ytdataformat.
*/
    if ( (status=IGcstr(536,forstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(forstr,istr);
/*
***Skapa typ av utyta.
*/
    if ( (status=IGcstr(557,typstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_EXDEF",valparam) < 0 )
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

       short IGsulo()

/*      Huvudrutin för sur_conic...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa SUR_CONIC sats
 *
 *      (C)microform ab 10/02/98 G. Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt,retla,dummy;
    short   nstrip,status,tnr;
    DBetype   typ;
    bool    end,right;
    PMLITVA litval;
/*
***Skapa referens till spine kurva.
*/
    typ = CURTYP;
    if ( (status=IGcref ( 534,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
    pmtcon(exnpt,(pm_ptr)NULL, &retla,  &dummy);
/*
***Referenser till direktriser och mittkurvor.
*/
    nstrip = 0;
    typ = CURTYP;
    litval.lit_type = C_STR_VA;
    while ( 4*nstrip < V2PARMAX-1 )
      {
      if ( (status=IGcref( 533,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      if ( (status=IGcref( 532,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      if ( nstrip > 0 )
        {
        if ( IGialt( 526,68,67,FALSE) ) goto no_more;
        }
      if ( IGialt( 531, 529, 530,FALSE) )
        {
        strcpy(litval.lit.str_va,"P");
        tnr =  527;
        }
      else
        {
        strcpy(litval.lit.str_va,"M");
        tnr =  528;
        }
      pmclie( &litval, &exnpt);
      pmtcon(exnpt,retla,&retla,&dummy);
      if ( (status=IGcref(tnr,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nstrip;
      }

no_more:

    if ( nstrip == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("SUR_CONIC",retla) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }

exit:
    WPerhg();
    return(status);
  }

/********************************************************/
