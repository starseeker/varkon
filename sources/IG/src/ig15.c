/*!******************************************************************/
/*  File: ig15.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  ld0pm();    Create horisontal LDIM                              */
/*  ld1pm();    Create vertical   LDIM                              */
/*  ld2pm();    Create parallell  LDIM                              */
/*                                                                  */
/*  cd0pm();    Create horisontal CDIM                              */
/*  cd1pm();    Create vertical   CDIM                              */
/*  cd2pm();    Create parallell  CDIM                              */
/*                                                                  */
/*  rdimpm();   Create RDIM                                         */
/*  adimpm();   Create ADIM                                         */
/*  xhtpm();    Create XHATCH                                       */
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

static short ldimpm(short alt);
static short cdimpm(short alt);

/*!******************************************************/

       short ld0pm()

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

       short ld1pm()

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

       short ld2pm()

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

/*      Huvudrutin för ldim.....satsen. 
 *
 *      In: alt => Typ av mått.
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
 *      23/3/86 genpos(pnr,  B. Doverud
 *      24/3/86 Felutgång B. Doverud
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
    if ( (status=genpos (258,&exnpt1)) < 0 ) goto end;
    if ( (status=genpos (259,&exnpt2)) < 0 ) goto end;
    if ( (status=genpos (301,&exnpt3)) < 0 ) goto end;
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
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("LDIM",valparam) < 0 ) goto error;

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

       short cd0pm()

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

       short cd1pm()

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

       short cd2pm()

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

/*      Huvudrutin för cdim.....satsen. 
 *
 *      In: alt => Typ av mått.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CDIM sats
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      31/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop till pmtcon, pmclie B. Doverud
 *      23/3/86  genpos(pnr,  B. Doverud
 *      24/3/86  Felutgång B. Doverud
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
    if ( (status=genref(277,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa position.
*/
    if ( (status=genpos (301,&exnpt2)) < 0  ) goto exit;
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
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("CDIM",valparam) < 0 ) goto error;

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
    errmes();
    gphgal(0);
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short rdimpm()

/*      Huvudrutin för rdim.....satsen. 
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
 *      14/10/85 Bytt t-sträng, J. Kjellander
 *      31/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop till pmtcon B. Doverud
 *      23/3/86  genpos(pnr,  B. Doverud
 *      24/3/86  Felutgång B. Doverud
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
    if ( (status=genref(277,&typ,&exnpt1,&end,&right)) < 0 ) goto exit;
/*
***Skapa 2 positioner.
*/
    if ( (status=genpos (303,&exnpt2)) < 0 ) goto exit;
    if ( (status=genpos (259,&exnpt3)) < 0 ) goto exit;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("RDIM",valparam) < 0 ) goto error;

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
    errmes();
    gphgal(0);
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short adimpm()

/*      Huvudrutin för adim.....satsen. 
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
 *      4/9/85   Anrop till igcges(), B. Doverud
 *      31/10/85 Ände och sida, J. Kjellander
 *      6/3/86   Defaultvärde, J. Kjellander
 *      20/3/86  Anrop till pmtcon B. Doverud
 *      23/3/86  genpos(pnr,  B. Doverud
 *      24/3/86  Felutgång B. Doverud
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
    if ( (status=genref(304,&typ,&exnpt1,&end,&right1)) < 0 ) goto exit;

    typ = LINTYP;
    if ( (status=genref(305,&typ,&exnpt2,&end,&right2)) < 0 ) goto exit;
/*
***Textens läge.
*/
    if ( (status=genpos(301,&exnpt3)) < 0 ) goto exit;
/*
***Skapa alternativ.
*/
    alt = 1;
    if ( igialt(367,67,68,FALSE) ) alt = 2;
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
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("ADIM",valparam) < 0 ) goto error;

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
    errmes();
    gphgal(0);
    goto start;
  }

/********************************************************/
/*!******************************************************/

       short xhtpm()

/*      Huvudrutin för xhatch(#id,dist,ang,ref1,ref2,,,,)
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
 *      4/9/85   Anrop till igcges()
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 Ände och sida, J. Kjellander
 *      16/3/86  Defaultsträngar B. Doverud
 *      20/3/86  Anrop till pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      16/3/88  getmid(), J. Kjellander
 *      25/7/90  Kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    PMREFVA ridmat[GMMXXH][MXINIV];
    short   i,nref,status;
    pm_ptr  exnpt1,exnpt2,exnpt,retla,dummy;
    PMLITVA litval;
    char    istr[V3STRLEN+1];
    DBetype   typv[GMMXXH];

    static char astr[V3STRLEN+1] ="";
    static char vstr[V3STRLEN+1] ="";

/*
***Ange linjer och cirklar i konturen.
*/
start:
    igptma(268,IG_MESS);
    nref = GMMXXH;
    typv[0] = LINTYP + ARCTYP + CURTYP;
    status = getmid(ridmat,typv,&nref);
    igrsma();

    if ( status < 0  ||  nref == 0 ) goto exit;
/*
***Avstånd.
*/
    if ( (status=genflt(288,astr,istr,&exnpt1)) < 0 ) goto exit;
    pmtcon( exnpt1, (pm_ptr)NULL, &retla, &dummy);
    strcpy(astr,istr);
/*
***Vinkel.
*/
    if ( (status=genflt(274,vstr,istr,&exnpt2)) < 0 ) goto exit;
    pmtcon( exnpt2, retla, &retla, &dummy);
    strcpy(vstr,istr);
/*
***Skapa pm-referenser av ridvek och länka ihop med parameterlistan.
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
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( igcges("XHATCH",retla) < 0 ) goto error;

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
    errmes();
    gphgal(0);
    goto exit;
  }

/********************************************************/
