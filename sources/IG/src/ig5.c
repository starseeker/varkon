/*!******************************************************************/
/*  File: ig5.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  pofrpm();     Generate poi_free.. statement                     */
/*  poprpm();     Generate poi_proj.. statement                     */
/*  cuftpm();     Generate cur_spline("FERGUSON"... with tangents   */
/*  cuctpm();     Generate cur_spline("CHORD... with tangents       */
/*  cuvtpm();     Generate cur_spline("STIFFNESS... with tangents   */
/*  cufnpm();     Generate cur_spline("FERGUSON"...  no tangents    */
/*  cucnpm();     Generate cur_spline("CHORD...  no tangents        */
/*  cuvnpm();     Generate cur_spline("STIFFNESS... no tangents     */
/*  cucfpm();     Generate cur_conic("FREE... statement             */
/*  cucppm();     Generate cur_conic("PROJ... statement             */
/*  partpm();     Generate part... statement                        */
/*  textpm();     Generate text... statement                        */
/*  cs3ppm();     Generate csys_3p... statement                     */
/*  cs1ppm();     Generate csys_1p... statement                     */
/*  bplnpm();     Generate b_plane... statement                     */
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

extern short modtyp;

static short poipm(char *typ);
static short curpm(char *typ, bool tang);
static short cucpm(char *typ);

/*!******************************************************/

       short pofrpm()

/*      Genererar poi_free... sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Inga.
 *
 *      (C)microform ab  19/11/85 J. Kjellander
 *
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_FREE"));
  }

/********************************************************/
/*!******************************************************/

       short poprpm()

/*      Genererar poi_proj... sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: Inga.
 *
 *      (C)microform ab  19/11/85 J. Kjellander
 *
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
     return(poipm("POI_PROJ"));
  }

/********************************************************/
/*!******************************************************/

 static short poipm(char *typ)

/*      Huvudrutin för poi_free.....
 *
 *      In: typ = "POI_FREE" eller "POI_PROJ"
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa POI_FREE sats
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      2/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      19/11/85 Slagit ihop free och proj, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  exnpt,dummy;

/*
***Skapa position.
*/
start:
    if ( (status=genpos(264,&exnpt)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges(typ,valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

end:
    gphgal(0);
    return(status);
/*
***Felutgångar.
*/
error:
    erpush("IG5023",typ);
    errmes();
    gphgal(0);
    goto start;
  }

/********************************************************/
/*!******************************************************/

       DBstatus cuftpm()

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

       DBstatus cuctpm()

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

       DBstatus cuvtpm()

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

       DBstatus cufnpm()

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

       DBstatus cucnpm()

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

       DBstatus cuvnpm()

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
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      19/11/85 Slagit ihop free och proj, J. Kjellander
 *      20/11/85 Tangenter, J. Kjellander
 *      20/3/86  Anrop till pmtcon pmclie, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
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
      if ( (status=genpos(264,&exnpt)) == REJECT ) break;
      if ( status == GOMAIN) goto end;

      pmtcon(exnpt,retla,&retla,&dummy);
/*
***Eventuella tangenter.
*/
      if ( !tang  ||  ((status=genpos(345,&exnpt)) == REJECT) )
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
    if ( igcges("CUR_SPLINE",retla) < 0 ) goto error1;

end:
    gphgal(0);
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
    gphgal(0);
    goto end;
  }

/********************************************************/
/*!******************************************************/

       short cucfpm()

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

       short cucppm()

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
    sprintf(pbuf,"%s %s %d",iggtts(258),iggtts(429),nseg);
    igplma(pbuf,IG_MESS);
    status = genpos(0,&exnpt[pn]);
    igrsma();
    if ( status == REJECT ) goto end;
    if ( status == GOMAIN) goto end;
/*
***1:a tangent-definitionspunkten.
*/
startt:
    pn = 1;
    nseg = 1;
    sprintf(pbuf,"%s %s %d",iggtts(421),iggtts(429),nseg);
    igplma(pbuf,IG_MESS);
    status = genpos(0,&exnpt[pn]);
    igrsma();
    if ( status == REJECT ) goto startp;
    if ( status == GOMAIN) goto end;
/*
***Segmentets slutposition.
*/
ploop:
    ++pn;
    sprintf(pbuf,"%s %s %d",iggtts(259),iggtts(429),nseg);
    igplma(pbuf,IG_MESS);
    status=genpos(0,&exnpt[pn]);
    igrsma();
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
    sprintf(pbuf,"%s %s %d",iggtts(422),iggtts(429),nseg);
    igplma(pbuf,IG_MESS);
    status = genpos(0,&exnpt[pn]);
    igrsma();
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
    if ( igialt(423,424,425,FALSE) )
      {
      sprintf(pbuf,"%s %s %d",iggtts(426),iggtts(429),nseg);
      igplma(pbuf,IG_INP);
      status = genflt(0,dstr,istr,&exnpt[pn]);
      strcpy(dstr,istr);
      igrsma();
      if ( status == REJECT )
        {
        pn -= 2;
        goto tloop;
        }
      else if ( status == GOMAIN ) goto end;
      }
    else
      {
      sprintf(pbuf,"%s %s %d",iggtts(427),iggtts(429),nseg);
      igplma(pbuf,IG_MESS);
      status = genpos(0,&exnpt[pn]);
      igrsma();
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
      if ( igialt(428,67,68,FALSE) )
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
    if ( igcges("CUR_CONIC",retla) < 0 )
      {
      erpush("IG5023","CUR_CONIC");
      errmes();
      }
/*
***Slut.
*/
end:
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short partpm()

/*      Huvudrutin för part...
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 27/2/85 Mats Neslon
 *
 *      3/7/85   Släck message area, B. Doverud
 *      4/9/85   Släck highlight märke, B. Doverud
 *      23/2/86  Nytt anrop till igcpts(), J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;
    static  char dstr[JNLGTH+1] = "";

/*
***Läs in part-namn.
*/
    igptma(244,IG_INP);
    if ( (status=igssip(iggtts(267),filnam,dstr,JNLGTH)) < 0 )
      {
      igrsma();
      goto end;
      }
    strcpy(dstr,filnam);
    igrsma();
/*
***Generera part-satsen.
*/
    if ((status=igcpts(filnam,PART)) == REJECT || status == GOMAIN ) goto end;
    else if ( status < 0 ) errmes();
/*
***Slut.
*/
end:
    gphgal(0);    
    return(status);

  }

/********************************************************/
/*!******************************************************/

       short textpm()

/*      Genererar text...sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa TEXT sats
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 *      3/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      16/11/85 t-sträng, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  genpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3;
    char    istr[V3STRLEN+1];

    static char sdstr[V3STRLEN+1] = "";
    static char vstr[V3STRLEN+1] = "0.0";

/*
***Skapa position.
*/
start:
    if ( (status=genpos(264,&exnpt1)) < 0 ) goto end;
/*
***Skapa vridning.
*/
    if ( (status=genflt(17,vstr,istr,&exnpt2)) < 0 ) goto end;
    strcpy(vstr,istr);
/*
***Skapa sträng.
*/
    if ( (status=genstr(10,sdstr,istr,&exnpt3)) < 0 ) goto end;
    strcpy(sdstr,istr);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("TEXT",valparam) < 0 ) goto error;

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

    gphgal(0);
    goto start;

end:
    gphgal(0);
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_3P");
    errmes();
    gphgal(0);
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

    gphgal(0);
    goto start;

end:
    gphgal(0);
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_1P");
    errmes();
    gphgal(0);
    goto start;
  }
  
/********************************************************/
/*!******************************************************/

       short bplnpm()

/*      Genererar b_plan.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa B_PLANE sats
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    pm_ptr exnpt1,exnpt2,exnpt3,exnpt4;
    pm_ptr valparam;
    pm_ptr retla,dummy;

/*
***Skapa 4 positioner.
*/
start:
    if ( (status=genpos(384,&exnpt1)) < 0 ) goto end;
    if ( (status=genpos(385,&exnpt2)) < 0 ) goto end;
    if ( (status=genpos(386,&exnpt3)) < 0 ) goto end;
    if ( (status=genpos(387,&exnpt4)) < 0 ) goto end;
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
    if ( igcges("B_PLANE",valparam) < 0 ) goto error;

    gphgal(0);
    goto start;

end:
    gphgal(0);
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","B_PLANE");
    errmes();
    gphgal(0);
    goto start;
  }
  
/********************************************************/
