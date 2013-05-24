/*!******************************************************************/
/*  File: ig23.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   iggset();   Creates set(param=expr) statement                  */
/*   igslvl();   Creates set(level=n) statement                     */
/*   igspen();   Creates set(pen=n) statement                       */
/*   igslfs();   Creates set(lfont=0) statement                     */
/*   igslfd();   Creates set(lfont=1) statement                     */
/*   igslfc();   Creates set(lfont=2) statement                     */
/*   igsafs();   Creates set(afont=0) statement                     */
/*   igsafd();   Creates set(afont=1) statement                     */
/*   igsafc();   Creates set(afont=2) statement                     */
/*   igscfs();   Creates set(cfont=0) statement                     */
/*   igscfd();   Creates set(cfont=1) statement                     */
/*   igscfc();   Creates set(cfont=2) statement                     */
/*   igsldl();   Creates set(ldashl=a) statement                    */
/*   igsadl();   Creates set(adashl=a) statement                    */
/*   igscdl();   Creates set(cdashl=a) statement                    */
/*   igstsz();   Creates set(tsize=a) statement                     */
/*   igstwd();   Creates set(twidth=a) statement                    */
/*   igstsl();   Creates set(tslant=a) statement                    */
/*   igstfn();   Creates set(tfont=a) statement                     */
/*   igstpm();   Creates set(tpmode=a) statement                    */
/*   igsdts();   Creates set(dtsiz=a) statement                     */
/*   igsdas();   Creates set(dasiz=a) statement                     */
/*   igsdnd();   Creates set(dndig=a) statement                     */
/*   igsda0();   Creates set(dauto=0) statement                     */
/*   igsda1();   Creates set(dauto=1) statement                     */
/*   igsxfs();   Creates set(xfont=0) statement                     */
/*   igsxfd();   Creates set(xfont=1) statement                     */
/*   igsxfc();   Creates set(xfont=2) statement                     */
/*   igsxdl();   Creates set(xdashl=a) statement                    */
/*   igswdt();   Creates set(width=a) statement                     */
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

extern V2NAPA defnap;

/*!******************************************************/

static short iggset(
       short  param,
       pm_ptr exnpt)

/*      Genererar set(param=expression)...sats. Används
 *      av alla de andra set-rutinerna.
 *
 *      In: param  => Namngiven parameter tex. level
 *          exnpt  => Pekare till uttryck
 *
 *      Ut: Inget.
 *
 *      Felkoder:  IG5012 = Kan ej skapa namnparameterlista
 *                 IG5022 = Kan ej skapa set-sats
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86 Anrop pmtcon B. Doverud
 *       
 ******************************************************!*/

  {
    pm_ptr  namparam,arglst,dummy;

/*
***Skapa namnparameternod och gör en lista av den.
*/
    if ( pmcnpa(param,exnpt,&namparam) < 0 ) goto error1;

    pmtcon(namparam,(pm_ptr)NULL,&arglst,&dummy);
/*
***Skapa satsen, interpretera och länka in i modulen.
*/
    if ( igcprs("SET",arglst) < 0) goto error2;

    return(0);

/*
***Felutgångar.
*/
error1:
    erpush("IG5012","iggset");
    goto errend;

error2:
    erpush("IG5022","SET");

errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igslvl()

/*      Genererar set(level=literal)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod : Nivån "%s" finns ej
 *
 *      (C)microform ab 19/6/85 J. Kjellander
 *
 *      13/9/85  Felnummer/utskrifter R. Svedin   
 *      18/3/86  Anrop pmclie B. Doverud
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Generera nivånummer-uttryck.
*/
    if ( (status=genint(227,"",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-satsen.
*/
    iggset(PMLEVEL,exnpt);
/*
***Uppdatera statusarean.
*/
    igupsa();

end:
    return(status);

  }
  
/********************************************************/
/*!******************************************************/

       short igspen()

/*      Genererar set(pen=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 19/6/85 J. Kjellander
 *
 *      13/3/86 igrsma(), J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Generera pennummer-uttryck.
*/
    if ( (status=genint(16,"1",istr,&exnpt)) < 0 ) goto exit;
/*
***Generera set-sats.
*/
    iggset(PMPEN,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();
/*
***Sudda meddelande-raden.
*/
exit:
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igslfs()

/*      Genererar set(lfont=0)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 0;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMLFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igslfd()

/*      Genererar set(lfont=1)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 1;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMLFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igslfc()

/*      Genererar set(lfont=2)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 2;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMLFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsafs()

/*      Genererar set(afont=0)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 0;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMAFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsafd()

/*      Genererar set(afont=1)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 1;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMAFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsafc()

/*      Genererar set(afont=2)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 2;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMAFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igscfs()

/*      Genererar set(cfont=0)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 12/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 0;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMCFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igscfd()

/*      Genererar set(cfont=1)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 12/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 1;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMCFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igscfc()

/*      Genererar set(cfont=2)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 12/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 2;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMCFONT,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsldl()

/*      Genererar set(ldashl=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  1/7/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(12),defnap.ldashl,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera strecklängd-uttryck.
*/
    if ( (status=genflt(0,"3.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMLDASHL,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igsadl()

/*      Genererar set(adashl=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  1/7/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(12),defnap.adashl,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera strecklängd-uttryck.
*/
    if ( (status=genflt(0,"3.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMADASHL,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igscdl()

/*      Genererar set(cdashl=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  12/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(12),defnap.cdashl,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera strecklängd-uttryck.
*/
    if ( (status=genflt(0,"3.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMCDASHL,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igstsz()

/*      Genererar set(tsize=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  1/7/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(11),defnap.tsize,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera textstorlek-uttryck.
*/
    if ( (status=genflt(0,"5.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMTSIZE,exnpt);
/*
***Uppdatera statusfältet.
*/
    igupsa();

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igstwd()

/*      Genererar set(twidth=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  1/7/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(13),defnap.twidth,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera textbredd-uttryck.
*/
    if ( (status=genflt(0,"60.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMTWIDTH,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igstsl()

/*      Genererar set(tslant=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab  1/7/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(15),defnap.tslant,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera textlutning-uttryck.
*/
    if ( (status=genflt(0,"15.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMTSLANT,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igstfn()

/*      Genererar set(TFONT=literal)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 7/1/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(108),(int)defnap.tfont,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera fontnummer-uttryck.
*/
    if ( (status=genint(0,"",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-satsen.
*/
    iggset(PMTFONT,exnpt);
/*
***Uppdatera statusarean.
*/
    igupsa();

end:
    igrsma();
    return(status);

  }
  
/********************************************************/
/*!******************************************************/

       short igstpm()

/*      Genererar set(TPMODE=literal)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 1988-10-02 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(43),(int)defnap.tpmode,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera tpmode-uttryck.
*/
    if ( (status=genint(0,"",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-satsen.
*/
    iggset(PMTPMODE,exnpt);
/*
***Uppdatera statusarean.
*/
    igupsa();

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igsdts()

/*      Genererar set(dtsize=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(11),defnap.dtsize,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera sifferstorlek-uttryck.
*/
    if ( (status=genflt(0,"5.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMDTSIZE,exnpt);

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igsdas()

/*      Genererar set(dasize=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(11),defnap.dasize,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera pilstorlek-uttryck.
*/
    if ( (status=genflt(0,"5.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMDASIZE,exnpt);

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igsdnd()

/*      Genererar set(dndig=utryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad, B. Doverud
 *      6/3/86   Defaultsträng, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%d  %s",iggtts(24),defnap.dndig,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera decimalantal-uttryck.
*/
    if ( (status=genint(0,"2",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMDNDIG,exnpt);

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igsda0()

/*      Genererar set(dauto=0)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med heltal=0.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 0;
    pmclie(&litstr,&exnpt);
/*
***Generera set-sats.
*/
    iggset(PMDAUTO,exnpt);
/*
***Skriv ut nytt värde.
*/
    igwtma(26);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short igsda1()

/*      Genererar set(dauto=1)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med heltal=0.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 1;
    pmclie(&litstr,&exnpt);
/*
***Generera set-sats.
*/
    iggset(PMDAUTO,exnpt);
/*
***Skriv ut nytt värde.
*/
    igwtma(25);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short igsxfs()

/*      Genererar set(xfont=0)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 0;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMXFONT,exnpt);

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsxfd()

/*      Genererar set(xfont=1)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 1;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMXFONT,exnpt);

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsxfc()

/*      Genererar set(xfont=2)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      18/3/86  Anrop pmclie B. Doverud
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt;
    PMLITVA litstr;

/*
***Skapa uttrycksnod med fontnummer.
*/
    litstr.lit_type = C_INT_VA;
    litstr.lit.int_va = 2;
    pmclie(&litstr,&exnpt);
/*
***Generera set-satsen.
*/
    iggset(PMXFONT,exnpt);

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short igsxdl()

/*      Genererar set(xdashl=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      10/9/85  Släck meddelande rad B. Doverud   
 *      18/3/86  Defaultsträng B. Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(12),defnap.xdashl,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera snittstrecklängd-uttryck.
*/
    if ( (status=genflt(0,"3.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMXDASHL,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short igswdt()

/*      Genererar set(width=uttryck)....sats
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 1997-12-29 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt;

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(13),defnap.width,iggtts(248));
    igplma(strbuf,IG_INP);
/*
***Generera bredd-uttryck.
*/
    if ( (status=genflt(0,"0.0",istr,&exnpt)) < 0 ) goto end;
/*
***Generera set-sats.
*/
    iggset(PMWIDTH,exnpt);

end:
    igrsma();
    return(status);
  }
  
/********************************************************/
