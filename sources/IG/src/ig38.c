/*!******************************************************************/
/*  File: ig38.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  cuispm();    Genererate cur_iso...    statement                 */
/*  curapm();    Genererate cur_approx... statement                 */
/*  curtpm();    Genererate cur_trim...   statement                 */
/*  cusipm();    Genererate cur_sil...    statement                 */
/*  surtpm();    Genererate sur_trim...   statement                 */
/*  surapm();    Genererate sur_approx... statement                 */
/*  sucopm();    Genererate sur_comp...   statement                 */
/*  suexpm();    Genererate sur_exdef...  statement                 */
/*  sulopm();    Genererate sur_conic...  statement                 */
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
#include "../../GP/include/GP.h"

/*!******************************************************/

       short cuispm()

/*      Huvudrutin för cur_iso......
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_ISO  sats
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
    static char isostr[V3STRLEN+1] ="0.0";
    static char stastr[V3STRLEN+1] ="0.0";
    static char endstr[V3STRLEN+1] ="1.0";
    PMLITVA litval;
/*
***Skapa referens till yta.
*/
    typ = SURTYP;
    if ( (status=genref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***U eller V parameter.
*/
    litval.lit_type = C_STR_VA;
    if ( igialt( 550, 548, 549,FALSE) )
      strcpy(litval.lit.str_va,"U");
    else
      strcpy(litval.lit.str_va,"V");
    pmclie( &litval, &exnpt2);
/*
***Skapa isoparametervärde.
*/
    if ( (status=genflt( 551,isostr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(isostr,istr);
/*
***Skapa start parametervärde.
*/
    if ( (status=genflt( 552,stastr,istr,&exnpt4)) < 0 ) goto exit;
    strcpy(stastr,istr);
/*
***Skapa slut  parametervärde.
*/
    if ( (status=genflt( 553,endstr,istr,&exnpt5)) < 0 ) goto exit;
    strcpy(endstr,istr);
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
    if ( igcges("CUR_ISO",valparam) < 0 )
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

       short curapm()

/*      Huvudrutin för cur_approx...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_APPROX sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *      1998-04-16 t400->t1539, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char typstr[V3STRLEN+1] ="CUB_SEG";
    static char metstr[V3STRLEN+1] ="1";
    static char nosstr[V3STRLEN+1] ="4";
    static char tolstr[V3STRLEN+1] ="0.05";
/*
***Skapa referens till kurva.
*/
    typ = CURTYP;
    if ( (status=genref(1539,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa typ av utkurva.
*/
    if ( (status=genstr(547,typstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Skapa metod för approximation.
*/
    if ( (status=genint(546,metstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(metstr,istr);
/*
***Skapa antal segment eller koordinat tolerans.
*/
    if ( strcmp(metstr,"1") == 0 )
      {
      if ( (status=genint(545,nosstr,istr,&exnpt4)) < 0 ) goto exit;
      strcpy(nosstr,istr);
      }
    else
      {
      if ( (status=genflt(544,tolstr,istr,&exnpt4)) < 0 ) goto exit;
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
    if ( igcges("CUR_APPROX",valparam) < 0 )
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

       short curtpm()

/*      Huvudrutin för cur_trim...  
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_TRIM sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *      1998-04-16 t400->t1539, J.Kjellander
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char stastr[V3STRLEN+1] ="0.0";
    static char endstr[V3STRLEN+1] ="1.0";
/*
***Skapa referens till kurva.
*/
    typ = CURTYP;
    if ( (status=genref(1539,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa start parametervärde.
*/
    if ( (status=genflt(552,stastr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(stastr,istr);
/*
***Skapa slut  parametervärde.
*/
    if ( (status=genflt(553,endstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(endstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL, &retla,  &dummy);
    pmtcon(exnpt2,  retla,      &retla,  &dummy);
    pmtcon(exnpt3,  retla,     &valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CUR_TRIM",valparam) < 0 )
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

       short cusipm()

/*      Huvudrutin för cur_sil......
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa CUR_SIL  sats
 *
 *      (C)microform ab 10/02/98 G.Liden
 *
 *      1998-04-13 Nya text sträng nummer G. Liden
 *
 ******************************************************!*/

  {
    short       status;
    pm_ptr      valparam,dummy;
    pm_ptr      exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype       typ;
    bool        end,right;
    char        istr[V3STRLEN+1];
    static char typstr[V3STRLEN+1] ="2";
/*
***Skapa referens till yta.
*/
    typ = SURTYP;
    if ( (status=genref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa betraktelseriktning.
*/
    if ( (status=genpos(49,&exnpt2)) < 0 ) goto exit;
/*
***Skapa typ av silhuette.
*/
    if ( (status=genint( 543,typstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Kurvgren.
*/
    if ( (status=genint(1610,"1",istr,&exnpt4)) < 0 ) goto exit;
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
    if ( igcges("CUR_SIL",valparam) < 0 )
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

       short surtpm()

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
    if ( (status=genref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa start U parametervärde.
*/
    if ( (status=genflt( 539,ustastr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(ustastr,istr);
/*
***Skapa slut  U parametervärde.
*/
    if ( (status=genflt( 540,uendstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(uendstr,istr);
/*
***Skapa start V parametervärde.
*/
    if ( (status=genflt( 541,vstastr,istr,&exnpt4)) < 0 ) goto exit;
    strcpy(vstastr,istr);
/*
***Skapa slut  V parametervärde.
*/
    if ( (status=genflt( 542,vendstr,istr,&exnpt5)) < 0 ) goto exit;
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
    if ( igcges("SUR_TRIM",valparam) < 0 )
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

       short surapm()

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
    if ( (status=genref(1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa typ av utyta.
*/
    if ( (status=genstr(521,typstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Skapa metod för approximation.
*/
    if ( (status=genint(546,metstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(metstr,istr);
/*
***Skapa antal ytlappar eller koordinat tolerans.
*/
    if ( strcmp(metstr,"1") == 0 )
      {
      if ( (status=genint(538,nosstr,istr,&exnpt4)) < 0 ) goto exit;
      strcpy(nosstr,istr);
      }
    else
      {
      if ( (status=genflt(544,tolstr,istr,&exnpt4)) < 0 ) goto exit;
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
    if ( igcges("SUR_APPROX",valparam) < 0 )
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

       short sucopm()

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
      if ( (status=genref(1608,&typ,&exnpt,&end,&right)) == REJECT ) break;
      if ( status == GOMAIN ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nref;
      }

    if ( nref == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("SUR_COMP",retla) < 0 )
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

       short suexpm()

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
    if ( (status=genstr(537,filstr,istr,&exnpt1)) < 0 ) goto exit;
    strcpy(filstr,istr);
/*
***Skapa ytdataformat.
*/
    if ( (status=genstr(536,forstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(forstr,istr);
/*
***Skapa typ av utyta.
*/
    if ( (status=genstr(557,typstr,istr,&exnpt3)) < 0 ) goto exit;
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
    if ( igcges("SUR_EXDEF",valparam) < 0 )
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

       short sulopm()

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
    if ( (status=genref ( 534,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
    pmtcon(exnpt,(pm_ptr)NULL, &retla,  &dummy);
/*
***Referenser till direktriser och mittkurvor.
*/
    nstrip = 0;
    typ = CURTYP;
    litval.lit_type = C_STR_VA;
    while ( 4*nstrip < V2PARMAX-1 )
      {
      if ( (status=genref( 533,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      if ( (status=genref( 532,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      if ( nstrip > 0 )
        {
        if ( igialt( 526,68,67,FALSE) ) goto no_more;
        }
      if ( igialt( 531, 529, 530,FALSE) )
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
      if ( (status=genref(tnr,&typ,&exnpt,&end,&right)) < 0 ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nstrip;
      }

no_more:

    if ( nstrip == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("SUR_CONIC",retla) < 0 )
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
    gphgal(0);
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
    gphgal(0);
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
    gphgal(0);
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
    gphgal(0);
    return(status);
  }

/********************************************************/
