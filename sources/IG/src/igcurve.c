/*!******************************************************************/
/*  igcurve.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGcuft();     Generate cur_spline("FERGUSON"... with tangents   */
/*  IGcuct();     Generate cur_spline("CHORD... with tangents       */
/*  IGcuvt();     Generate cur_spline("STIFFNESS... with tangents   */
/*  IGcufn();     Generate cur_spline("FERGUSON"...  no tangents    */
/*  IGcucn();     Generate cur_spline("CHORD...  no tangents        */
/*  IGcuvn();     Generate cur_spline("STIFFNESS... no tangents     */
/*  IGcucf();     Generate cur_conic("FREE... statement             */
/*  IGcucp();     Generate cur_conic("PROJ... statement             */
/*  IGcomp();     Generate cur_comp.. statement                     */
/*  IGcuro();     Generate cur_offs.. statement                     */
/*  IGcuri();     Generate cur_int...statement                      */
/*  IGcuis();     Generate cur_iso...    statement                  */
/*  IGcura();     Generate cur_approx... statement                  */
/*  IGcurt();     Generate cur_trim...   statement                  */
/*  IGcusi();     Generate cur_sil...    statement                  */
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

static short curpm(char *typ, bool tang);
static short cucpm(char *typ);

/*!******************************************************/

        short IGcuft()

/*      Generate cur_spline("FERGUSON.. with tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("FERGUSON",TRUE));
  }

/********************************************************/
/*!******************************************************/

        short IGcuct()

/*      Generate cur_spline("CHORD.. with tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("CHORD",TRUE));
  }

/********************************************************/
/*!******************************************************/

        short IGcuvt()

/*      Generate cur_spline("STIFFNESS.. with tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("STIFFNESS",TRUE));
  }

/********************************************************/
/*!******************************************************/

        short IGcufn()

/*      Generate cur_spline("FERGUSON.. without tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("FERGUSON",FALSE));
  }

/********************************************************/
/*!******************************************************/

        short IGcucn()

/*      Generate cur_spline("CHORD.. without tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("CHORD",FALSE));
  }

/********************************************************/
/*!******************************************************/

        short IGcuvn()

/*      Generate cur_spline("STIFFNESS.. without tangents.
 *
 *      FV: 0 = OK, REJECT = exit, GOMAIN = main menu.
 *
 *      (C)microform ab  1999-10-29 J. Kjellander
 *
 ******************************************************!*/

  {
     return(curpm("STIFFNESS",FALSE));
  }

/********************************************************/
/*!******************************************************/

static short curpm(
       char *typ,
       bool  tang)

/*      Genererar cur_free... sats
 *
 *      In: typ = FREE eller PROJ
 *          tang = TRUE => Tangenter
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa sats
 *              IG5103 = En kurva måste bestå av minst 2 positioner
 *
 *      (C)microform ab  3/2/85 J. Kjellander
 *
 *      3/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      19/11/85 Slagit ihop free och proj, J. Kjellander
 *      20/11/85 Tangenter, J. Kjellander
 *      20/3/86  Anrop till pmtcon pmclie, B. Doverud
 *      23/3/86  IGcpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      8/12/89  tang, J. Kjellander
 *      15/11/91 CUR_SPLINE, J. Kjellander
 *      15/6/93  GMMXSG->V2PARMAX, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    pm_ptr  retla;
    pm_ptr  exnpt,dummy;
    short   npoi;
    PMLITVA litstr;

/*
***1:a parametern är FREE/PROJ.
*/
    npoi = 0;
    litstr.lit_type = C_STR_VA;
    strcpy(litstr.lit.str_va,typ);
    pmclie( &litstr,&exnpt);
    retla = (pm_ptr)NULL;
    pmtcon(exnpt,retla,&retla,&dummy);
/*
***Skapa positioner och tangenter och länka ihop dom
***till en parameterlista. Max antal positioner = hälften
***av (max antal MBS-parametrar minus 1 för FREE/PROJ).
*/
    while ( npoi < (V2PARMAX-1)/2 )
      {
      if ( (status=IGcpos(264,&exnpt)) == REJECT ) break;
      if ( status == GOMAIN) goto end;

      pmtcon(exnpt,retla,&retla,&dummy);
/*
***Eventuella tangenter.
*/
      if ( !tang  ||  ((status=IGcpos(345,&exnpt)) == REJECT) )
        {
        litstr.lit_type = C_VEC_VA;
        litstr.lit.vec_va.x_val = 0.0;
        litstr.lit.vec_va.y_val = 0.0;
        litstr.lit.vec_va.z_val = 0.0;
        pmclie( &litstr, &exnpt);
        }

      if ( status == GOMAIN) goto end;

      pmtcon(exnpt,retla,&retla,&dummy);
      ++npoi;
      }
    if ( npoi == 0 ) goto end;
    if ( npoi == 1 ) goto error2;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CUR_SPLINE",retla) < 0 ) goto error1;

end:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error1:
    erpush("IG5023","CUR_SPLINE");
    goto errend;

error2:
    erpush("IG5103","");

errend:
    errmes();
    WPerhg();
    goto end;
  }

/********************************************************/
/*!******************************************************/

       short IGcucf()

/*      Genererar cur_conic(id,"free"... sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab  15/11/91 J. Kjellander
 *
 ******************************************************!*/

  {
     return(cucpm("FREE"));
  }

/********************************************************/
/*!******************************************************/

       short IGcucp()

/*      Genererar cur_conic(id,"proj"... sats.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab  15/11/91 J. Kjellander
 *
 ******************************************************!*/

  {
     return(cucpm("PROJ"));
  }

/********************************************************/
/*!******************************************************/

static short cucpm(char *typ)

/*      Genererar cur_conic... sats
 *
 *      In: typ = FREE eller PROJ
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa sats
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      15/6/93  GMMXSG->V2PARMAX, J. Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    int     pn,nseg;
    pm_ptr  retla;
    pm_ptr  exnpt[V2PARMAX],dummy;
    char    pbuf[V3STRLEN+1];
    char    istr[V3STRLEN+1];
    PMLITVA litstr;

    static char dstr[V3STRLEN+1] = "0.5";

/*
***Kurvans startposition.
*/
startp:
    pn = 0;
    nseg = 1;
    sprintf(pbuf,"%s %s %d",IGgtts(258),IGgtts(429),nseg);
    IGplma(pbuf,IG_MESS);
    status = IGcpos(0,&exnpt[pn]);
    IGrsma();
    if ( status == REJECT ) goto end;
    if ( status == GOMAIN) goto end;
/*
***1:a tangent-definitionspunkten.
*/
startt:
    pn = 1;
    nseg = 1;
    sprintf(pbuf,"%s %s %d",IGgtts(421),IGgtts(429),nseg);
    IGplma(pbuf,IG_MESS);
    status = IGcpos(0,&exnpt[pn]);
    IGrsma();
    if ( status == REJECT ) goto startp;
    if ( status == GOMAIN) goto end;
/*
***Segmentets slutposition.
*/
ploop:
    ++pn;
    sprintf(pbuf,"%s %s %d",IGgtts(259),IGgtts(429),nseg);
    IGplma(pbuf,IG_MESS);
    status=IGcpos(0,&exnpt[pn]);
    IGrsma();
    if ( status == REJECT )
      {
      --nseg;
      pn -= 2;
      if ( pn < 2 ) goto startt;
      else          goto mloop;
      }
    else if ( status == GOMAIN ) goto end;
/*
***Segmentets slut-tangent.
*/
tloop:
    ++pn;
    sprintf(pbuf,"%s %s %d",IGgtts(422),IGgtts(429),nseg);
    IGplma(pbuf,IG_MESS);
    status = IGcpos(0,&exnpt[pn]);
    IGrsma();
    if ( status == REJECT )
      {
      pn -= 2;
      goto ploop;
      }
    else if ( status == GOMAIN ) goto end;
/*
***Segmentets P-värde eller mellanliggande punkt.
*/
mloop:
    ++pn;
    if ( IGialt(423,424,425,FALSE) )
      {
      sprintf(pbuf,"%s %s %d",IGgtts(426),IGgtts(429),nseg);
      IGplma(pbuf,IG_INP);
      status = IGcflt(0,dstr,istr,&exnpt[pn]);
      strcpy(dstr,istr);
      IGrsma();
      if ( status == REJECT )
        {
        pn -= 2;
        goto tloop;
        }
      else if ( status == GOMAIN ) goto end;
      }
    else
      {
      sprintf(pbuf,"%s %s %d",IGgtts(427),IGgtts(429),nseg);
      IGplma(pbuf,IG_MESS);
      status = IGcpos(0,&exnpt[pn]);
      IGrsma();
      if ( status == REJECT )
        {
        pn -= 2;
        goto tloop;
        }
      else if ( status == GOMAIN ) goto end;
      }
/*
***Ett segment till ?
*/
    if ( nseg < (V2PARMAX-3)/3 )
      {
      if ( IGialt(428,67,68,FALSE) )
        {
        ++nseg;
        goto ploop;
        }
      }
/*
***1:a parametern är typ, dvs. free/proj.
*/
    ++pn;
    litstr.lit_type = C_STR_VA;
    strcpy(litstr.lit.str_va,typ);
    pmclie( &litstr,&exnpt[pn]);
    retla = (pm_ptr)NULL;
    pmtcon(exnpt[pn],retla,&retla,&dummy);
/*
***Sen kommer startpunkten och dess tangent.
*/
    pmtcon(exnpt[0],retla,&retla,&dummy);
    pmtcon(exnpt[1],retla,&retla,&dummy);
/*
***Länka ihop resten av parameterlistan.
*/
   for ( i=0; i<nseg; ++i )
     {
     pmtcon(exnpt[3*i+4],retla,&retla,&dummy);
     pmtcon(exnpt[3*i+2],retla,&retla,&dummy);
     pmtcon(exnpt[3*i+3],retla,&retla,&dummy);
     }
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CUR_CONIC",retla) < 0 )
      {
      erpush("IG5023","CUR_CONIC");
      errmes();
      }
/*
***Slut.
*/
end:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short IGcomp()

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
 *      6/9/85   Anrop till IGcges(), R. Sviden
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
      if ( (status=IGcref(268,&typ,&exnpt,&end,&right)) == REJECT ) break;
      if ( status == GOMAIN ) goto exit;
      pmtcon(exnpt,retla,&retla,&dummy);
      ++nref;
      }

    if ( nref == 0 ) goto exit;
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CUR_COMP",retla) < 0 )
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

       short IGcuro()

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
    if ( (status=IGcref (430,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa offset.
*/
    if ( (status=IGcflt(278,ofstr,istr,&exnpt2)) < 0 ) goto exit;
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
    if ( IGcges("CUR_OFFS",valparam) < 0 )
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

       short IGcuri()

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
    if ( (status=IGcref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa referens till plan.
*/
    typ = CSYTYP+BPLTYP;
    if ( (status=IGcref (1609,&typ,&exnpt2,&end,&right)) < 0 ) goto exit;
/*
***Kurvgren.
*/
    if ( (status=IGcint(1610,"1",istr,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CUR_INT",valparam) < 0 )
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

       short IGcuis()

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
    if ( (status=IGcref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***U eller V parameter.
*/
    litval.lit_type = C_STR_VA;
    if ( IGialt( 550, 548, 549,FALSE) )
      strcpy(litval.lit.str_va,"U");
    else
      strcpy(litval.lit.str_va,"V");
    pmclie( &litval, &exnpt2);
/*
***Skapa isoparametervärde.
*/
    if ( (status=IGcflt( 551,isostr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(isostr,istr);
/*
***Skapa start parametervärde.
*/
    if ( (status=IGcflt( 552,stastr,istr,&exnpt4)) < 0 ) goto exit;
    strcpy(stastr,istr);
/*
***Skapa slut  parametervärde.
*/
    if ( (status=IGcflt( 553,endstr,istr,&exnpt5)) < 0 ) goto exit;
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
    if ( IGcges("CUR_ISO",valparam) < 0 )
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

       short IGcura()

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
    if ( (status=IGcref(1539,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa typ av utkurva.
*/
    if ( (status=IGcstr(547,typstr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Skapa metod för approximation.
*/
    if ( (status=IGcint(546,metstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(metstr,istr);
/*
***Skapa antal segment eller koordinat tolerans.
*/
    if ( strcmp(metstr,"1") == 0 )
      {
      if ( (status=IGcint(545,nosstr,istr,&exnpt4)) < 0 ) goto exit;
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
    if ( IGcges("CUR_APPROX",valparam) < 0 )
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

       short IGcurt()

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
    if ( (status=IGcref(1539,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa start parametervärde.
*/
    if ( (status=IGcflt(552,stastr,istr,&exnpt2)) < 0 ) goto exit;
    strcpy(stastr,istr);
/*
***Skapa slut  parametervärde.
*/
    if ( (status=IGcflt(553,endstr,istr,&exnpt3)) < 0 ) goto exit;
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
    if ( IGcges("CUR_TRIM",valparam) < 0 )
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

       short IGcusi()

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
    if ( (status=IGcref (1608,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa betraktelseriktning.
*/
    if ( (status=IGcpos(49,&exnpt2)) < 0 ) goto exit;
/*
***Skapa typ av silhuette.
*/
    if ( (status=IGcint( 543,typstr,istr,&exnpt3)) < 0 ) goto exit;
    strcpy(typstr,istr);
/*
***Kurvgren.
*/
    if ( (status=IGcint(1610,"1",istr,&exnpt4)) < 0 ) goto exit;
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
    if ( IGcges("CUR_SIL",valparam) < 0 )
      {
      erpush("IG5023","");
      errmes();
      }
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
