/*!******************************************************************/
/*  File: ig17.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  ighelp();    Interactiv help system                             */
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
#include "../../EX/include/EX.h"
#include <string.h>

extern char  hlpdir[];
extern char  actpnm[];
extern short mstack[];
extern short actfun,mant;

#ifdef WIN32
extern HWND ms_main;
#endif

static short iglhlp(char *hlpnam);

/*!******************************************************/

        short ighelp()

/*      Skriver ut hjälp om aktiv funktion, meny eller modul.
 *      Den globala variabeln actfun = -1 => meny
 *                                     -2 => modul
 *                                     >0 => funktion
 *
 *      FV: 0
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      1998-10-30 HTML, J.Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[JNLGTH+5];
    short mnum;

/*
***Modul.
*/
    if ( actfun == -2 )
      {
      strcpy(filnam,actpnm);
      }
/*
***Meny.
*/
    else if ( actfun == -1 )
      {
      if ( (mnum=mstack[mant-1]) == 0 ) mnum = iggmmu();
      filnam[0] = 'm';
      sprintf(&filnam[1],"%d",mnum);
      }
/*
***Funktion, skapa filnamn av funktionsnummer.
*/
    else
      {
      if      ( actfun < 10  ) sprintf(filnam,"f00%d",actfun);
      else if ( actfun < 100 ) sprintf(filnam,"f0%d",actfun);
      else                     sprintf(filnam,"f%d",actfun);
      }
/*
***Lista hjälp-filen.
*/
    return(iglhlp(filnam));
  }

/*!******************************************************/
/*!******************************************************/

static short iglhlp(char *hlpnam)

/*      Listar hjälpfil.
 *
 *      In: hlpnam = Pekare till filnamn.
 *                   partnamn, m+heltal eller f+heltal
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      Felkoder: IG0202 = Kan ej hitta hjälpfilen %s
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      5/11/86  GOMAIN, J. Kjellander
 *      15/2/95  VARKON_DOC, J. Kjellander
 *      1998-10-30 HTML, J.Kjellander
 *      1999-03-09 WIN32, J.Kjellander
 *
 ******************************************************!*/

  {
    char   filnam[V3PTHLEN+1];
    char   linbuf[V3STRLEN+1];
    char   htmcmd[V3STRLEN+1];
    char   oscmd[512];
    short  status;
    FILE  *hlpfil;

/*
***Var finns filen. Prova först på hlpdir, dvs. det
***aktuella projektets hjälpkatalog.
*/
    if ( iggrst("html_viewer",htmcmd) )
      {
      strcpy(filnam,hlpdir);
      strcat(filnam,hlpnam);
      strcat(filnam,".htm");
      if ( v3facc(filnam,'R') ) goto show;
      erpush("IG0202",filnam);
      }

    strcpy(filnam,hlpdir);
    strcat(filnam,hlpnam);
    strcat(filnam,".txt");
    if ( v3facc(filnam,'R') ) goto show;
    erpush("IG0202",filnam);
/*
***Om det inte gick, prova istället VARKON_DOC-katalogen.
*/
    if ( iggrst("html_viewer",htmcmd) )
      {
#ifdef UNIX
      sprintf(filnam,"%sv_man/%s.htm",v3genv(VARKON_DOC),hlpnam);
#endif
#ifdef WIN32
      sprintf(filnam,"%sv_man\\%s.htm",v3genv(VARKON_DOC),hlpnam);
#endif
      if ( v3facc(filnam,'R') ) goto show;
      erpush("IG0202",filnam);
      }
#ifdef UNIX
    sprintf(filnam,"%sv_man/%s.txt",v3genv(VARKON_DOC),hlpnam);
#endif
#ifdef WIN32
    sprintf(filnam,"%sv_man\\%s.txt",v3genv(VARKON_DOC),hlpnam);
#endif
    if ( v3facc(filnam,'R') ) goto show;
    erpush("IG0202",filnam);
/*
***Om det inte gick, prova default hjälpfil för menyer och parter.
*/
    if ( actfun < 0 )
      {
      if      ( actfun == -1 ) strcpy(hlpnam,"menydoc");
      else if ( actfun == -2 ) strcpy(hlpnam,"partdoc");

      if ( iggrst("html_viewer",htmcmd) )
        {
#ifdef UNIX
        sprintf(filnam,"%sv_man/%s.htm",v3genv(VARKON_DOC),hlpnam);
#endif
#ifdef WIN32
        sprintf(filnam,"%sv_man\\%s.htm",v3genv(VARKON_DOC),hlpnam);
#endif
        if ( v3facc(filnam,'R') ) goto show;
        erpush("IG0202",filnam);
        }
#ifdef UNIX
      sprintf(filnam,"%sv_man/%s.txt",v3genv(VARKON_DOC),hlpnam);
#endif
#ifdef WIN32
      sprintf(filnam,"%sv_man\\%s.txt",v3genv(VARKON_DOC),hlpnam);
#endif
      if ( v3facc(filnam,'R') ) goto show;
      erpush("IG0202",filnam);
      }
/*
***Om det fortfarande inte gick, felmeddelande.
*/
    errmes();
    return(0);
/*
***Fil har hittats. Töm felstacken och visa filen.
***Om HTML-viewer finns, använd den.
*/
show:
     erinit();

     if ( iggrst("html_viewer",htmcmd) )
       {
       sprintf(oscmd,"%s %s",htmcmd,filnam);
       EXos(oscmd,2);
       return(0);
       }
/*
***Om inte använder vi listarean.
*/
    igplma(" ",IG_MESS);
    strcpy(linbuf,iggtts(358));
    strcat(linbuf,filnam);
    iginla(linbuf);
/*
***Lista filen. Newline-tecken strippas bort.
*/
    hlpfil = fopen(filnam,"r"); 
    while(fgets(linbuf,V3STRLEN,hlpfil) != NULL)
      {
      linbuf[strlen(linbuf)-1] = '\0';
      if ( (status=igalla(linbuf,(short)1)) < 0 ) goto end;
      }
/*
***Återställ listarean..
*/
    status = igexla();
/*
***Avslutning.
*/
end:
    fclose(hlpfil);
    igrsma();
    return(status);
  }
/********************************************************/
