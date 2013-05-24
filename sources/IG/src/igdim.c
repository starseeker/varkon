/*!******************************************************************/
/*  igdim.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGld0();    Create horisontal LDIM                              */
/*  IGld1();    Create vertical   LDIM                              */
/*  IGld2();    Create parallell  LDIM                              */
/*                                                                  */
/*  IGcd0();    Create horisontal CDIM                              */
/*  IGcd1();    Create vertical   CDIM                              */
/*  IGcd2();    Create parallell  CDIM                              */
/*                                                                  */
/*  IGrdim();   Create RDIM                                         */
/*                                                                  */
/*  IGadim();   Create ADIM                                         */
/*                                                                  */
/*  IGxht();    Create XHATCH                                       */
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

static short ldimpm(short alt);
static short cdimpm(short alt);

/*!******************************************************/

       short IGld0()

/*      Skapa "ldim(#id,p1,p2,p3,0)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(ldimpm(0));
  }

/********************************************************/
/*!******************************************************/

       short IGld1()

/*      Skapa "ldim(#id,p1,p2,p3,1)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(ldimpm(1));
  }

/********************************************************/
/*!******************************************************/

       short IGld2()

/*      Skapa "ldim(#id,p1,p2,p3,2)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(ldimpm(2));
  }

/********************************************************/
/*!******************************************************/

static short ldimpm(short alt)

/*      Huvudrutin f�r ldim.....satsen. 
 *
 *      In: alt => Typ av m�tt.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa LDIM sats
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      4/9/85  Anrop till skapa satsen. B. Doverud   
 *      20/3/86 Anrop till pmtcon, pmclie B. Doverud
 *      23/3/86 IGcpos(pnr,  B. Doverud
 *      24/3/86 Felutg�ng B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    PMLITVA litstr;

/*
***Skapa 3 positioner.
*/
start:
    if ( (status=IGcpos (258,&exnpt1)) < 0 ) goto end;
    if ( (status=IGcpos (259,&exnpt2)) < 0 ) goto end;
    if ( (status=IGcpos (301,&exnpt3)) < 0 ) goto end;
/*
***Skapa alternativ.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = alt;
    pmclie(&litstr,&exnpt4);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("LDIM",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);
/*
***Felutg�ngar.
*/
error:
    erpush("IG5023","");
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGcd0()

/*      Skapa "cdim(#id,p1,p2,p3,0)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(cdimpm(0));
  }

/********************************************************/
/*!******************************************************/

       short IGcd1()

/*      Skapa "cdim(#id,p1,p2,p3,1)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(cdimpm(1));
  }

/********************************************************/
/*!******************************************************/

       short IGcd2()

/*      Skapa "cdim(#id,p1,p2,p3,2)" sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    return(cdimpm(2));
  }

/********************************************************/
/*!******************************************************/

static short cdimpm(short alt)

/*      Huvudrutin f�r cdim.....satsen. 
 *
 *      In: alt => Typ av m�tt.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CDIM sats
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      31/10/85 �nde och sida, J. Kjellander
 *      20/3/86  Anrop till pmtcon, pmclie B. Doverud
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    DBetype   typ;
    bool    end,right;
    PMLITVA litstr;

/*
***Skapa referens.
*/
start:
    typ = ARCTYP;
    if ( (status=IGcref(277,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa position.
*/
    if ( (status=IGcpos (301,&exnpt2)) < 0  ) goto exit;
/*
***Skapa alternativ.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = alt;
    pmclie(&litstr,&exnpt3);
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("CDIM",valparam) < 0 ) goto error;

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
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGrdim()

/*      Huvudrutin f�r rdim.....satsen. 
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa RDIM sats
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      14/10/85 Bytt t-str�ng, J. Kjellander
 *      31/10/85 �nde och sida, J. Kjellander
 *      20/3/86  Anrop till pmtcon B. Doverud
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,retla;
    DBetype   typ;
    bool    end,right;

/*
***Skapa referens.
*/
start:
    typ = ARCTYP;
    if ( (status=IGcref(277,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa 2 positioner.
*/
    if ( (status=IGcpos (303,&exnpt2)) < 0 ) goto exit;
    if ( (status=IGcpos (259,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("RDIM",valparam) < 0 ) goto error;

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
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGadim()

/*      Huvudrutin f�r adim.....satsen. 
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod:  IG5023 = Kan ej skapa ADIM sats
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      31/10/85 �nde och sida, J. Kjellander
 *      6/3/86   Defaultv�rde, J. Kjellander
 *      20/3/86  Anrop till pmtcon B. Doverud
 *      23/3/86  IGcpos(pnr,  B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      23/10/86 alt, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status,alt;
    pm_ptr  valparam,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,retla;
    DBetype   typ;
    bool    end,right1,right2;
    PMLITVA litval;

/*
***Skapa 2 referenser.
*/
start:
    typ = LINTYP;
    if ( (status=IGcref(304,&typ,&exnpt1,&end,&right1)) < 0 ) goto exit;

    typ = LINTYP;
    if ( (status=IGcref(305,&typ,&exnpt2,&end,&right2)) < 0 ) goto exit;
/*
***Textens l�ge.
*/
    if ( (status=IGcpos(301,&exnpt3)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    alt = 1;
    if ( IGialt(367,67,68,FALSE) ) alt = 2;
    if ( !right1 ) alt = -alt;

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
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("ADIM",valparam) < 0 ) goto error;

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
    errmes();
    WPerhg();
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short IGxht()

/*      Huvudrutin f�r xhatch(#id,dist,ang,ref1,ref2,,,,)
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5162 = Storheten redan utpekad
 *              IG5023 = Kan ej skapa XHATCH sats
 *
 *      (C)microform ab  5/8/85 J. Kjellander
 *
 *      4/9/85   Anrop till IGcges()
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 �nde och sida, J. Kjellander
 *      16/3/86  Defaultstr�ngar B. Doverud
 *      20/3/86  Anrop till pmtcon, pmclie B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      16/3/88  IGgmid(), J. Kjellander
 *      25/7/90  Kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    PMREFVA ridmat[GMMXXH][MXINIV];
    short   status;
    int     i,nref;
    pm_ptr  exnpt1,exnpt2,exnpt,retla,dummy;
    PMLITVA litval;
    char    istr[V3STRLEN+1];
    DBetype typv[GMMXXH];

    static char astr[V3STRLEN+1] ="";
    static char vstr[V3STRLEN+1] ="";

/*
***Ange linjer och cirklar i konturen.
*/
start:
    IGptma(268,IG_MESS);
    nref = GMMXXH;
    typv[0] = LINTYP + ARCTYP + CURTYP;
    status = IGgmid(ridmat,typv,&nref);
    IGrsma();

    if ( status < 0  ||  nref == 0 ) goto exit;
/*
***Avst�nd.
*/
    if ( (status=IGcflt(288,astr,istr,&exnpt1)) < 0 ) goto exit;
    pmtcon( exnpt1, (pm_ptr)NULL, &retla, &dummy);
    strcpy(astr,istr);
/*
***Vinkel.
*/
    if ( (status=IGcflt(274,vstr,istr,&exnpt2)) < 0 ) goto exit;
    pmtcon( exnpt2, retla, &retla, &dummy);
    strcpy(vstr,istr);
/*
***Skapa pm-referenser av ridvek och l�nka ihop med parameterlistan.
*/
    for ( i=0; i<nref; ++i)
      {
      litval.lit_type = C_REF_VA;
      litval.lit.ref_va[0].seq_val = ridmat[i][0].seq_val;
      litval.lit.ref_va[0].ord_val = ridmat[i][0].ord_val;
      litval.lit.ref_va[0].p_nextre = ridmat[i][0].p_nextre;
      pmclie(&litval,&exnpt);
      pmtcon( exnpt, retla, &retla, &dummy);
      }
/*
***Skapa, interpretera och l�nka in satsen i modulen.
*/
    if ( IGcges("XHATCH",retla) < 0 ) goto error;

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
    errmes();
    WPerhg();
    goto exit;
  }

/********************************************************/
