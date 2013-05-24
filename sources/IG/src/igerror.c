/*!******************************************************************/
/*  igerror.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  erinit();    Init error stack                                   */
/*  erpush();    Push error message                                 */
/*  erlerr();    Return last errcode                                */
/*  errmes();    Display error stack                                */
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
#include "../include/debug.h"
#include "../../WP/include/WP.h"
#include <string.h>

#define ESTKSZ 25          /* Felstackens storlek */
#define INSLEN 256         /* Max längd på insert-strängen */

typedef struct errrpt
  {
  char   module[3];         /* Modul, 2 tecken + Ön */
  short  errnum;            /* Felnummer */
  short  sev;               /* Severity-code */
  char   insert[INSLEN+1];  /* Insert-sträng, tecken + \n */
  } ERRRPT;

ERRRPT  errstk[ESTKSZ];

/* errstk är felmeddelandestacken */

extern bool  igbflg;
extern char  jobdir[],jobnam[];

/*!******************************************************/

        short erinit()

/*      Tömmer fel-stacken.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 6/9/85 J. Kjellander
 *
 ******************************************************!*/

  {
    short i;

    for ( i=0; i<ESTKSZ; ++i )
        {
        errstk[i].module[0] = '\0';
        errstk[i].insert[0] = '\0';
        }
    return(0);
  }

/*!******************************************************/
/*!******************************************************/

        short erpush(
        char  *code,
        char  *str)

/*      Felhanteringsrutin.
 *
 *      In: code => Pekare till sträng innehållande
 *                    felkod. 6 tecken avslutat med Ön.
 *          str  => Pekare till "insert"-sträng.
 *
 *      Ut: Inget.
 *
 *      FV: Severity-kod med minustecken.
 *
 *      (C)microform ab 7/1/85 J. Kjellander
 *
 *      21/10/85 Rapport om sev=4, J. Kjellander
 *      18/2/86  igexsa() om sev=4, J. Kjellander
 *      7/3/95   max 80 tecken i str, J. Kjellander
 *      1998-03-11 INSLEN, J.Kjellander
 *      2004-02-21 igialt(), J.Kjellander
 *
 ******************************************************!*/

    {
    int   errval,sevval;
    short i;

/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"***Start-erpush***\n");
      fprintf(dbgfil(IGEPAC),"Felkod=%s\n",code);
      fprintf(dbgfil(IGEPAC),"Insert=%s\n",str);
      }
#endif
/*
***Insertsträngar får inte vara längre än INSLEN tecken.
*/
    if ( strlen(str) > INSLEN ) str[INSLEN] = '\0';
/*
***Skifta felstacken ett snäpp.
*/
    for ( i=ESTKSZ-1; i>0; --i )
        {
        strcpy(errstk[i].module,errstk[i-1].module);
        errstk[i].errnum = errstk[i-1].errnum;
        errstk[i].sev = errstk[i-1].sev;
        strcpy(errstk[i].insert,errstk[i-1].insert);
        }
/*
***Packa upp felkoden, skapa felrapport och lagra sist i stacken.
*/
    errstk[0].module[0] = code[0];
    errstk[0].module[1] = code[1];
    errstk[0].module[2] = '\0';

    sscanf(code+2,"%d",&errval);
    errval = errval/10;
    errstk[0].errnum = errval;

    sscanf(code+5,"%d",&sevval);
    errstk[0].sev = sevval;

    strcpy(errstk[0].insert,str);
/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"Severity=%d\n",sevval);
      fprintf(dbgfil(IGEPAC),"***Slut-erpush***\n");
      }
#endif
/*
***Om severity 4 rapportera felet och bryt ner systemet.
*/
    if ( sevval == 4 )
      {
      errmes();
      igialt(457,458,458,TRUE);
      igexsa();
      }
/*
***Returnera severity.
*/
    return(-(errstk[0].sev));

    }

/*!******************************************************/
/*!******************************************************/

        short errmes()

/*      Rapporterar fel.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 17/4/85 Mats Nelson
 *
 *      5/9/85     J. Kjellander, felstack.
 *      3/2/86     Bytt getchar mot iggtch R. Svedin
 *      4/3/86     VMS, J. Kjellander
 *      8/10/86    Meddelanderaden, J. Kjellander
 *      13/10/86   ersatt scanf, J. Kjellander
 *      25/8/92    X11, J. Kjellander
 *      26/4/93    Bug långa texter, J. Kjellander
 *      15/2/95    VARKON_ERM, J. Kjellander
 *      7/3/95     Ännu längre texter, J. Kjellander
 *      15/1/96    X-resurs för rubrik, J. Kjellander
 *      1996-04-25 Bug filnamÄ20Å, J.Kjellander
 *      1998-03-11 INSLEN, J.Kjellander
 *      1998-10-22 Kan ej öppna ermfil, J.Kjellander
 *
 ******************************************************!*/

  {
    char filnam[V3PTHLEN+1],codbuf[20],linbuf[512],ferrst[512];
    char ins1[512],ins2[512],ins3[512];
    short ferrnm,fsevnm,estkpt,i1,i2,slen;
    FILE *errfil;

#ifdef X11
    char title[V3STRLEN];
#endif

/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"***Start-errmes***\n");
      fflush(dbgfil(IGEPAC));
      }
#endif
/*
***Initiering.
*/
    estkpt = 0;
/*
***Med X11/WIN32 använder vi list-arean för felrapportering.
***Om listarean redan är öppen kommer den nu att öppnas en
***gång till och man förlorar den 1:a listan. Ett sätt är
***att här alltid stänga först även om det inte behövs.
***WPexla() tål detta.
*/
    WPexla(TRUE);

#ifdef V3_X11
    if ( !WPgrst("varkon.error.title",title) )
      strcpy(title,"      FELRAPPORT-FELRAPPORT-FELRAPPORT");
    WPinla(title);
#endif

#ifdef WIN32
    if ( !msgrst("ERROR.TITLE",title) )
      strcpy(title,"      FELRAPPORT-FELRAPPORT-FELRAPPORT");
    msinla(title);
#endif
/*
***Skriv ut rapporter ur felstacken.
*/
loop:
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"\nestkpt=%hd\n",estkpt);
      fflush(dbgfil(IGEPAC));
      }
#endif
    if ( estkpt == ESTKSZ ) goto end;
    if ( errstk[estkpt].module[0] == '\0' ) goto end;
/*
***Skapa fel-filnamn och prova att öppna filen.
*/
    strcpy(filnam,v3genv(VARKON_ERM));
    strcat(filnam,errstk[estkpt].module);
    strcat(filnam,ERMEXT);

    errfil = fopen(filnam,"r");
/*
***Filen går ej att öppna.
*/
    if ( errfil == NULL )
        {
#ifdef V3_X11
        WPalla("Can't open error message file !",(short)1);
        WPalla(filnam,(short)1);
#endif
#ifdef WIN32
        msalla("Can't open error message file !",(short)1);
        msalla(filnam,(short)1);
#endif
        goto end;
        }
/*
***Läs en rad från filen.
*/
    while(fgets(linbuf,512,errfil) != NULL)
      {
/*
***Kolla om raden börjar med ett heltal, dvs ett felnummer.
*/
      if ( sscanf(linbuf,"%hd",&ferrnm) == 1 )
        {
/*
***Det gjorde den, kolla om det är rätt nummer.
*/
        if ( ferrnm == errstk[estkpt].errnum )
          {
/*
***Rätt meddelande, läs felnumret, sevcode och feltext.
*/
          if ( sscanf(linbuf,"%hd%hd%[\136\n]",
                    &ferrnm,&fsevnm,ferrst) == 3 )
            {
/*
***Packa upp den insertsträng som finns på felstacken i
***sina upp till max tre olika delar.
*/
           ins1[0] = '\0';
           ins2[0] = '\0';
           ins3[0] = '\0';

            slen = strlen(errstk[estkpt].insert);
            for ( i1=0; i1<slen; ++i1 )
               {
               if ( errstk[estkpt].insert[i1] == '%' ) 
                 {
                 for ( i1++,i2=0 ; i1<slen; ++i1,++i2 )
                    {
                    if ( errstk[estkpt].insert[i1] == '%' ) 
                      {
                      strcpy(ins3,&errstk[estkpt].insert[i1+1]);
                      goto cont;
                      }
                    ins2[i2] = errstk[estkpt].insert[i1];
                    ins2[i2+1] = '\0';
                    }
                 }
               ins1[i1] = errstk[estkpt].insert[i1];
               ins1[i1+1] = '\0';
               }
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"i1=%s,i2=%s,i3=%s\n",ins1,ins2,ins3);
      fprintf(dbgfil(IGEPAC),ferrst,ins1,ins2,ins3);
      fflush(dbgfil(IGEPAC));
      }
#endif
/*
***Skriv felmeddelandet till linbuf. Klipp vid 115 tecken så att
***själva felkoden (13 tecken) säkert kommer med.
*/
cont:
            sprintf(linbuf,ferrst,ins1,ins2,ins3);

            if ( strlen(linbuf) > 115 ) linbuf[115] = '\0';
/*
***Lägg till felkoden inom paranteser.
*/
            sprintf(codbuf," - (%s %d %d)",
                   errstk[estkpt].module,
                   errstk[estkpt].errnum,
                   errstk[estkpt].sev);
            strcat(linbuf,codbuf);
/*
***Pip och skriv ut.
*/
            fclose(errfil);
            igbell();
#ifdef V3_X11
            WPalla(linbuf,(short)1);
            ++estkpt;
            goto loop;
#endif
#ifdef WIN32
            msalla(linbuf,(short)1);
            ++estkpt;
            goto loop;
#endif
            }
         }
      }
   }
/*
***Inget meddelande med rätt nummer har hittats.
*/
    fclose(errfil);
    sprintf(linbuf,"<EOF> and no hit, errcode=%s%d%d, %s",
            errstk[estkpt].module,
            errstk[estkpt].errnum,
            errstk[estkpt].sev,
            errstk[estkpt].insert);
    igbell();

#ifdef V3_X11
    WPalla(linbuf,(short)1);
#endif
#ifdef WIN32
    msalla(linbuf,(short)1);
#endif
/*
***Här slutar vi.
*/
end:

#ifdef V3_X11
    WPexla(TRUE);
#endif
#ifdef WIN32
    msexla(TRUE);
#endif
/*
***Alla meddelanden är nu utskrivna, reinitiera felsystemet igen.
*/
    erinit();
/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(IGEPAC) == 33 )
      {
      fprintf(dbgfil(IGEPAC),"***Slut-errmes***\n\n");
      fflush(dbgfil(IGEPAC));
      }
#endif

    return(0);
  }
/********************************************************/
/*!******************************************************/

        short erlerr()

/*      Returnerar senaste felnumer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Felnummer.
 *
 *      (C)microform ab 13/1/85 J. Kjellander
 *
 ******************************************************!*/

    {
    return(errstk[0].errnum);
    }

/*!******************************************************/
