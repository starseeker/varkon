/*!******************************************************************/
/*  igmodule19.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*    crepar();   Create parameter                                  */
/*    chapar();   Edit parameter value                              */
/*    lstpar();   List parameter values                             */
/*    igevpl();   Evalueate parameters of active module             */
/*    igramo();   Run active module interactively                   */
/*    igream();   Run active module from macro                      */
/*    igrnmo();   Run named module                                  */
/*    igmfun();   Run macro                                         */
/*    prtmod();   Decompile to file                                 */
/*    lstmod();   Decompile to window                               */
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
#include "../include/isch.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include <string.h>

#define TS_INT   4        /* datatyp - lstpar */
#define TS_FLOAT 5        /* datatyp - lstpar */
#define TS_STR   6        /* datatyp - lstpar */
#define TS_VEC   7        /* datatyp - lstpar */
#define TS_PLHEAD 8       /* headline parameter list */
#define TS_VLHEAD 9       /* headline view list */
#define TS_ISTR  10       /* prompt - inläsning av text-sträng */

extern pm_ptr actmod;
extern DBptr  lsysla;
extern bool   rstron;
extern DBfloat  rstrox,rstroy,rstrdx,rstrdy;
extern short  modtyp,igtrty;
extern char   jobdir[],jobnam[];
extern V3MDAT sydata;

/*!******************************************************/

        short crepar()

/*      Varkonfunktion för att skapa parameter
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG2082 = Kan ej skapa parameter
 *              IG2252 = Parametern finns redan
 *
 *      (C)microform ab 27/1/85 J. Kjellander
 *
 *      22/9/85  Ledtexter, B. Doverud
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      31/10/85 Sträng, J. Kjellander
 *      5/11/85  igexfu(), J. Kjellander
 *      19/11/85 Stora bokstäver, J. Kjellander
 *      2/12/85  isialpha(), J. Kjellander
 *      20/3/86  Anrop till pmclie, B Doverud
 *      6/10/86  GOMAIN, B. Doverud
 *      7/10/86  GOMAIN, J. Kjellander
 *      29/9/87  genxxv(), J. Kjellander
 *      1/3/94   Snabbval, J. Kjellander
 *      1998-01-15 uchar/isialpha, J.Kjellander
 *      1998-03-18 (char *) för AIX, J.Kjellander
 *
 ******************************************************!*/

  {
     unsigned char namn[V2SYNLEN+1];   /* VC++ 5.0 1998-01-15 */
     char    promt[V3STRLEN+1];
     short   alt=0,alttyp,status;
     double  defflt;
     DBint   defint;
     char    defstr[V3STRLEN+1];
     pm_ptr  ref,retla,exnpt;
     short   i;
     stidcl  kind;
     char    istr[V3STRLEN+1];
     PMLITVA litval;
     DBVector   defpos;
     MNUALT *pmualt;

    static char defnam[V2SYNLEN+1] ="";   /* Default parameternamn */
    static char dstr[V3STRLEN+1] ="";     /* Default värde-sträng */

/*
***Läs in namn.
*/
loop1:
     igptma(228,IG_INP);
     status=igssip(iggtts(267),(char *)namn,defnam,V2SYNLEN);
     igrsma();
     if ( status < 0 ) return(status);
     strcpy(defnam,(char *)namn);
/*
***Kolla att namnet börjar på en bokstav och sedan består av
***bokstäver och/eller siffror.
*/
     if ( !isialpha(namn[0]) ) goto error;

     for ( i=0; i<strlen((char *)namn); ++i)
        if ( !isialpha(namn[i]) && !isdigit(namn[i]) ) goto error;
/*
***Kolla att en variabel, konstant eller label
***med detta namn inte finns sen tidigare.
*/
     stlook((char *)namn,&kind,&ref);
     if ( ref != (pm_ptr)NULL && 
        ( kind == ST_VAR || kind == ST_CONST || kind == ST_LABEL ) )
        {
        erpush("IG2252",(char *)namn);
        errmes();
        goto loop1;
        }
/*
***Läs in promt-sträng.
*/
     igssip(iggtts(229),promt,"",V3STRLEN);
/*
***Skriv ut meny och läs in typ. Snabbval ej tillåtet.
*/
#ifdef WIN32
     msshmu(143);
#else
     igaamu(143);
#endif

loop2: 
     iggalt(&pmualt,&alttyp);

#ifdef WIN32
     mshdmu();
#endif

     if ( pmualt == NULL )
       {
       igsamu();
       switch ( alttyp )
         {
         case SMBRETURN:
         status = REJECT;
         goto end;

         case SMBMAIN:
         status = GOMAIN;
         goto end;
         }
       }
     else alt = pmualt->actnum;

     switch (alt)
       {
/*
***Parameter av typen heltal.
*/
       case 1:
       if ( (status=geninv(230,istr,dstr,&defint)) < 0 ) goto end;
       strcpy(dstr,istr);
       litval.lit_type = C_INT_VA;
       litval.lit.int_va = defint;

       pmclie( &litval, &exnpt);
       if ( stdtyp((sttycl)ST_INT,(short)0,&ref) < 0) goto error;
       break;
/*
***Parameter av typen flyttal.
*/
       case 2:
       if ( (status=genflv(230,istr,dstr,&defflt)) < 0 ) goto end;
       strcpy(dstr,istr);
       litval.lit_type = C_FLO_VA;
       litval.lit.float_va = defflt;

       pmclie( &litval, &exnpt);
       if ( stdtyp((sttycl)ST_FLOAT,(short)0,&ref) < 0) goto error;
       break;
/*
***Parameter av typen sträng.
*/
       case 3:
       if ( (status=genstv(230,istr,dstr,defstr)) < 0 ) goto end;
       strcpy(dstr,istr);
       litval.lit_type = C_STR_VA;
       strcpy(litval.lit.str_va,defstr);

       pmclie( &litval, &exnpt);
       if ( stdtyp((sttycl)ST_STR,(short)V3STRLEN,&ref) < 0) goto error;
       break;
/*
***Parameter av typen vektor.
*/
       case 4:
       igptma(230,IG_MESS);
       status = genpov(&defpos);
       igrsma();
       if ( status < 0 ) goto end;
       litval.lit_type = C_VEC_VA;
       litval.lit.vec_va.x_val = defpos.x_gm;
       litval.lit.vec_va.y_val = defpos.y_gm;
       litval.lit.vec_va.z_val = defpos.z_gm;

       pmclie( &litval, &exnpt);
       if ( stdtyp((sttycl)ST_VEC,(short)0,&ref) < 0) goto error;
       break;
/*
***Okänt svar, fel i menyn.
*/
       default:
       erpush("IG0103","");
       errmes();
       goto loop2;
       }
/*
***Skapa parametern och länka in i parameterlistan.
*/
       if ( stcvar((char *)namn,ref,ST_VPAVA,0,exnpt,&ref) < 0) goto error;
       if ( pmcpar(ref,promt,&retla) < 0) goto error;
       if ( pmlmpa( actmod,retla) < 0) goto error;
/*
***Interpretera parametern.
*/
    if ( (status=inevpa(retla,NULL,NULL,0)) < 0 ) goto error;

end:
    igsamu();
    WPerhg();
    return(status);
/*
***Felutgång.
*/
  error: erpush("IG2082","");
         errmes();
         goto end;
  }

/********************************************************/
/*!******************************************************/

        short chapar()

/*      Varkonfunktion för att ändra parametervärde.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod:
 *
 *      (C)microform ab 27/1/85 J. Kjellander
 *
 *      22/9/85  Ledtexter, B. Doverud
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      19/11/85 Stora bokstäver, J. Kjellander
 *      26/5/86  Nytt anrop till pmrpap(), J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *      29/9/87  genxxv, J. Kjellander
 *      26/9/93  Formatkod flyttal, J. Kjellander
 *
 ******************************************************!*/

  {
    char name[V2SYNLEN+1];   /* parameter name buffer */
    char nname[V2SYNLEN+1];  /*     -"-     */
    char prompt[V3STRLEN+1]; /* parameter prompt string buffer */
    PMLITVA litval;          /* literal value structure */
    pm_ptr panola;           /* PM-pointer to param. node */
    double defflt;           /* Float-parameters nya defaultvärde */
    DBint defint;
    char  defstr[V3STRLEN+1];
    DBVector defpos;
    short status;
    char  istr[V3STRLEN+1];
    char  pstr[V3STRLEN+1];

    static char defnam[V2SYNLEN+1] ="";   /* Default parameternamn */
    static char dstr[V3STRLEN+1] ="";     /* Default värde-sträng */

/*
***Läs in parameternamn.
*/
    igptma(228,IG_INP);
    if ( (status=igssip(iggtts(267),nname,defnam,V2SYNLEN)) < 0 ) goto end;
    strcpy(defnam,nname);
    igrsma();
/*
***Nolltäll parameterpekaren.
*/
    pmrpap((pm_ptr)0);
/*
***Leta upp parametern.
*/
    for (;;)
        {
        pmgpad(&panola);
        if (panola == (pm_ptr)NULL)
           {
           erpush("IG2192",nname);
           errmes();
           goto end;
           }
        pmrpar(panola,name,prompt,&litval);
        if(sticmp(nname,name) == 0) break;
        }
/*
***Läs in nytt defaultvärde.
*/
    switch (litval.lit_type)
       {
       case C_INT_VA:
       sprintf(pstr,"%s%d %s",iggtts(43),litval.lit.int_va,iggtts(328));
       igplma(pstr,IG_INP);
       if ( (status=geninv(0,istr,dstr,&defint)) < 0 ) goto end;
       strcpy(dstr,istr);
       litval.lit.int_va = defint;
       break;

       case C_FLO_VA:
       sprintf(pstr,"%s%.15g %s",iggtts(43),litval.lit.float_va,iggtts(328));
       igplma(pstr,IG_INP);
       if ( (status=genflv(0,istr,dstr,&defflt)) < 0 ) goto end;
       strcpy(dstr,istr);
       litval.lit.float_va = defflt;
       break;

       case C_STR_VA:
       sprintf(pstr,"%s%s %s",iggtts(43),litval.lit.str_va,iggtts(328));
       igplma(pstr,IG_INP);
       if ( (status=genstv(0,istr,dstr,defstr)) < 0 ) goto end;
       strcpy(dstr,istr);
       strcpy(litval.lit.str_va,defstr);
       break;

       case C_VEC_VA:
       igptma(329,IG_MESS);
       if ( (status=genpov(&defpos)) < 0 ) goto end;
       litval.lit.vec_va.x_val = defpos.x_gm;
       litval.lit.vec_va.y_val = defpos.y_gm;
       litval.lit.vec_va.z_val = defpos.z_gm;
       break;
       }
/*
***Uppdatera parameterns defaultvärde.
*/
    pmupar(panola,&litval);
/*
***Interpretera parametern.
*/
    inevpa(panola,NULL,NULL,0);

end:
    igrsma();
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short lstpar()

/*      Varkonfunktion för att lista parametrar
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod:
 *
 *      (C)microform ab 27/1/85 J. Kjellander
 *
 *      28/10/85 Sträng, J. Kjellander
 *      26/5/86  Nytt anrop till pmrpap(), J. Kjellander
 *      1/6/86   Referens, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
  char name[80];         /* parameter name buffer */
  char prompt[V3STRLEN+1]; /* parameter prompt string buffer */
  PMLITVA litval;        /* literal value structure */
  pm_ptr panola;         /* PM-pointer to param. node */
  char linbuf[80];       /* line output buffer */
  char typbuf[15];       /* sprintf() type buffer */
  char valbuf[V3STRLEN]; /* sprintf() value buffer */
  char pmtbuf[V3STRLEN]; /* sprintf() prompt buffer */
  char scrbuf1[V3STRLEN];/* sprintf() scratch buffer */
  char scrbuf2[80];      /* sprintf() scratch buffer */
  short radsp=0;         /* radsprång */
  short status;          /* status från WPalla() */

  if(pmrpap((pm_ptr)0) < 0) /* reset parameter pointer */
      return(0);
  WPinla(iggtts(TS_PLHEAD));     /* init list area */

  for(;;)
      {
      if(pmgpad(&panola) != 0)   /* get pointer to next parameter */
          return(0);
      if(panola == (pm_ptr)NULL) /* no more params. ? */
          {
          WPexla(TRUE);              /* exit from list area */
          return(0);
          }
      if(pmrpar(panola,name,prompt,&litval) != 0)
          return(0);

      sprintf(linbuf,"%-15s",name); /* print name to linbufÄÅ */

      switch(litval.lit_type)       /* print type to typbufÄÅ*/
        {                           /* and print value to valbufÄÅ*/
        case C_INT_VA:
        sprintf(typbuf,"%-9s",iggtts(TS_INT));
        sprintf(scrbuf1,"%d",litval.lit.int_va);
        sprintf(valbuf,"%-25s",scrbuf1);
        break;

        case C_FLO_VA:
        sprintf(typbuf,"%-9s",iggtts(TS_FLOAT));
        sprintf(scrbuf1,"%g",litval.lit.float_va);
        sprintf(valbuf,"%-25s",scrbuf1);
        break;

        case C_STR_VA:
        sprintf(typbuf,"%-9s",iggtts(TS_STR));
        sprintf(scrbuf1,"%s",litval.lit.str_va);
        sprintf(valbuf,"%-25s",scrbuf1);
        break;

        case C_VEC_VA:
        sprintf(typbuf,"%-9s",iggtts(TS_VEC));
        sprintf(scrbuf1,"(%g",litval.lit.vec_va.x_val);
        sprintf(scrbuf2,",%g",litval.lit.vec_va.y_val);
        strcat(scrbuf1,scrbuf2);
        if ( modtyp == 3 )
          {
          sprintf(scrbuf2,",%g",litval.lit.vec_va.z_val);
          strcat(scrbuf1,scrbuf2);
          }
        sprintf(scrbuf2,")");
        strcat(scrbuf1,scrbuf2);
        sprintf(valbuf,"%-25s",scrbuf1);
        break;

        case C_REF_VA:
        sprintf(typbuf,"%-9s",iggtts(48));
        igidst(litval.lit.ref_va,scrbuf1);
        sprintf(valbuf,"%-25s",scrbuf1);
        break;
        }

      sprintf(pmtbuf,"%-30s",prompt);    /* print prompt to linbuf */
      strcat(linbuf,typbuf);
      strcat(linbuf,valbuf);
      strcat(linbuf,pmtbuf);
      if ((status=WPalla(linbuf,radsp)) < 0 ) return(status);
      radsp=2;
      }
  }

/********************************************************/
/*!******************************************************/

        short igevpl()

/*      Går igenom modulens parameterlista och anropar
 *      för varje parameter rutinen inevpa().
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:    IG2263 => Kan ej evaluera parameterlistan
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      26/5/86  Nytt anrop till pmrpap(), J. Kjellander
 *
 ******************************************************!*/

  {
  pm_ptr parptr;

/*
***Initiera parameterpekare.
*/
  if ( pmrpap((pm_ptr)0) < 0 ) goto errend;
/*
***Gå igenom parameterlistan.
*/
  for (;;)
      {
      if ( pmgpad(&parptr) != 0 ) goto errend;
      if ( parptr == (pm_ptr)NULL) return(0);    /* Slut på parametrar ? */
      if ( inevpa(parptr,NULL,NULL,0) < 0 ) goto errend;
      }
/*
***Felutgång.
*/
errend:
  return(erpush("IG2263",""));
  }

/********************************************************/
/*!******************************************************/

        short igramo()

/*      Interaktiv varkonfunktion för att köra om aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 10/6/92 J. Kjellander
 *
 *      8/10/92  inclpp(), J. Kjellander
 *      1996-12-05 Demo, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Initiera parameter-poolen.
***Gjordes ursprungligen i inacc.c.
*/
    inclpp();
/*
***Nollställ RTS.
***Gjordes ursprungligen i inacc.c.
*/
    incrts();
/*
***Kör om modulen.
*/
    status = igream();

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igream()

/*      Kör om aktiv modul. Nollst'ller inte RTS eller
 *      parameterpool.
 *      Används även av MACRO. Gamla igramo().
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG2182 = Fel vid exekvering av modul
 *
 *      (C)microform ab 22/2/85 J. Kjellander
 *
 *      22/11/85 Uppdatera menyfältet, J. Kjellander
 *      14/3/86  Bytt namn, J. Kjellander
 *      7/11/86  Tagit bort ini/exi-tty, J. Kjellander
 *      31/10/88 CGI, J. Kjellander
 *      2/11/88  N220G, R. Svedin
 *      25/1/95  Multifönster, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Sudda skärm och displayfil.
*/
#ifdef UNIX
    WPergw(GWIN_ALL);
    WPwait(GWIN_MAIN,TRUE);
#endif
#ifdef WIN32
    msergw(GWIN_ALL);
#endif
/*
***Töm gm.
*/
    DBreset();
/*
***Försätt systemet i initial-mode.
*/
    EXmoba();
    inrdnp();
/*
***Kör om modulen.
*/
    status=inmod(actmod);
/*
***Visa ev. aktivt koordinatsystem.
*/
    igupcs(lsysla,V3_CS_ACTIVE);
/*
***Tänd ev. raster igen.
*/
    if ( rstron ) WPdrrs(rstrox,rstroy,rstrdx,rstrdy);
/*
***Slut på väntan.
*/
#ifdef UNIX
    WPwait(GWIN_MAIN,FALSE);
#endif
/*
***Ev. felmeddelanden efter körning.
*/
    if ( status < 0 ) errmes();

    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igrnmo()

/*      Varkonfunktion för kör namngiven modul. Modulen
 *      körs utan att anropet lagras i PM. Resultatet
 *      lagras dock i GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      6/3/91   Ny t-sträng 166, J. Kjellander
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;
    static  char dstr[JNLGTH+1] = "";

/*
***Läs in part-namn.
*/
    igptma(166,IG_INP);
    if ( (status=igssip(iggtts(267),filnam,dstr,JNLGTH)) < 0 ) goto end;
    strcpy(dstr,filnam);
/*
***Generera och kör part-satsen. RUN => Ingen länkning.
*/
    if ((status=igcpts(filnam,RUN)) == REJECT || status == GOMAIN) goto end;
    else if ( status < 0 ) errmes();
/*
***Slut.
*/
end:
    igrsma();
    WPerhg();    
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igmfun()

/*      Varkonfunktion för macro-funktion, MFUNC.
 *      Modulen körs utan att anropet lagras i PM eller
 *      att resultatet sparas i GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 15/3/88 J. Kjellander
 *
 *      6/3/91   Ny t-sträng 167, J. Kjellander
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;
    static  char dstr[JNLGTH+1] = "";

/*
***Läs in part-namn.
*/
    igptma(167,IG_INP);
    if ( (status=igssip(iggtts(267),filnam,dstr,JNLGTH)) < 0 ) goto end;
    strcpy(dstr,filnam);
/*
***Generera och kör part-satsen. MFUNC => Ingen länkning, eller lagring.
*/
    if ((status=igcpts(filnam,MFUNC)) == REJECT || status == GOMAIN) goto end;
    else if ( status < 0 ) errmes();
/*
***Slut.
*/
end:
    igrsma();
    WPerhg();    
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short prtmod()

/*      Varkonfunktion för att skriva ut modul till fil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:  IG0212 = Otillåten skyddskod
 *               IG0222 = Kan ej skapa fil
 *
 *      (C)microform ab 18/4/87 J. Kjellander
 *
 *      15/9/87    Ej dekomp. om mpcode = 0, J. Kjellander
 *      23/9/87    Fixat bug. fclose isf. close, R. Svedin
 *      11/11/88   Ny felhant, J. Kjellander
 *      14/2/92    Filen finns redan, J. Kjellander
 *      1996-01-26 ser_crypt, J. Kjellander
 *
 ******************************************************!*/

  {
    char      buf[80];
    short     status;
    PMMODULE  modhed;
    FILE     *mbsfp;

    static char fnam[V3PTHLEN+1]="";

/*
***Kolla att dekompilering är tillåten. Om mpcode >= 0 får bara
***den som skapat modulen dekompilera.
*/
    pmrmod(&modhed);
    if ( modhed.system.sernr != sydata.sernr && modhed.system.mpcode >= 0 )
      {
      sprintf(buf,"%d%%%d",modhed.system.sernr,modhed.system.mpcode);
      erpush("IG0212",buf);
      errmes();
      if ( sydata.sernr != 1000 ) return(0);
      }
/*
***Kolla att inte någon har trixat med modulens serienummer.
***Gäller moduler skapade med 1.13g och senare.
*/
    else if ( modhed.system.revnr > 13  ||
            ( modhed.system.revnr == 13 && modhed.system.level > 'f') )
      {
      if ( modhed.system.sernr != v3dksn(modhed.system.ser_crypt) )
        {
        erpush("IG0502","");
        errmes();
        if ( sydata.sernr != 1000 ) return(0);
        }
      }
/*
***Fråga efter filnamn.
*/
    igptma(376,IG_INP);
    strcpy(fnam,jobdir);
    strcat(fnam,jobnam);
    strcat(fnam,MBSEXT);
    status = igssip(iggtts(267),fnam,fnam,V3PTHLEN);
    igrsma();
    if ( status < 0 ) return(status);
/*
***Finns redan en fil med samma namn ?
*/
    if ( (mbsfp=fopen(fnam,"r")) != NULL )
      {
      fclose(mbsfp);
      if ( !igialt(440,67,68,TRUE) ) return(0);
      }
/*
***Gammal fil finns inte eller skall skrivas över.
*/
    if ( (mbsfp=fopen(fnam,"w")) == NULL )
      {
      erpush("IG0222",fnam);
      errmes();
      return(0);
      }
/*
***Dekompilera.
*/
    pprmo(PPFILE,mbsfp);
    fclose(mbsfp);
/*
***Avslutande meddelande.
*/
    strcpy(buf,fnam);
    strcat(buf,iggtts(374));
    igwlma(buf,IG_MESS);
/*
***Slut.
*/
    return(0);

  }

/********************************************************/
/*!******************************************************/

        short lstmod()

/*      Varkonfunktion för att skriva ut aktiv modul
 *      i listarean.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG0212 = Otillåten skyddskod.
 *
 *      (C)microform ab 18/4/87 J. Kjellander
 *
 *      15/9/87  Ej dekomp. om mpcode = 0, J. Kjellander
 *      11/11/88 Ny felhant, J. Kjellander
 *      1996-01-26 ser_crypt, J. Kjellander
 *
 ******************************************************!*/

  {
    char      buf[80];
    PMMODULE  modhed;

/*
***Kolla att dekompilering är tillåten.
*/
    pmrmod(&modhed);

    if ( modhed.system.sernr != sydata.sernr && modhed.system.mpcode >= 0 )
      {
      sprintf(buf,"%d%%%d",modhed.system.sernr,modhed.system.mpcode);
      erpush("IG0212",buf);
      errmes();
      if ( sydata.sernr != 1000 ) return(0);
      }
    else if ( modhed.system.revnr > 13  ||
            ( modhed.system.revnr == 13 && modhed.system.level > 'f') )
      {
      if ( modhed.system.sernr != v3dksn(modhed.system.ser_crypt) )
        {
        erpush("IG0502","");
        errmes();
        if ( sydata.sernr != 1000 ) return(0);
        }
      }
/*
***Lista modulen.
*/
    strcpy(buf,iggtts(375));
    strcat(buf,jobnam);
    strcat(buf,MODEXT);
    WPinla(buf);
    pprmo(PPIGLIST,NULL);
    WPexla(TRUE);

    return(0);

  }

/********************************************************/
