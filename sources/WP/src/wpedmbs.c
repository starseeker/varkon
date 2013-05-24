/**********************************************************************
*
*    wpedmbs.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPamod();   MBS-Edit active module
*    WPomod();   MBS-Edit called module
*    WPcoal();   Compile all
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern char   jobdir[],jobnam[],mbsdir[],mbodir[];

static bool  edit(char *dir, char *namn);
static bool  comp(char *dir, char *namn, char *utdir);

/*!******************************************************/

        short WPamod()

/*      Varkonfunktion för editering av aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1452 Kan ej skapa filen: %s
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 ******************************************************!*/

 {
   char  tmpnam[V3PTHLEN+1],olddir[V3PTHLEN+1];
   char  buf[256];
   FILE *fp;

/*
***Öppna temporär fil.
*/
   strcpy(tmpnam,v3genv(VARKON_TMP));
   strcat(tmpnam,jobnam);
   strcat(tmpnam,MBSEXT);

   if ( (fp=fopen(tmpnam,"w")) == NULL )
     {
     erpush("WP1452",tmpnam);
     errmes();
     return(0);
     }
/*
***Dekompilera.
*/
   pprmo(PPFILE,fp);
/*
***Stäng filen igen.
*/
   fclose(fp);
/*
***Editera.
*/
loop:
   edit(v3genv(VARKON_TMP),jobnam);
/*
***Kompilera.
*/
   if ( ( WPgrst("varkon.mbsedit.autocompile",buf)  &&
                                          strcmp(buf,"True") == 0 )  ||
          igialt(106,67,68,FALSE) )
     {
     if ( comp(v3genv(VARKON_TMP),jobnam,NULL) )
       {
       WPexla(FALSE);
       strcpy(olddir,jobdir);
       strcpy(jobdir,v3genv(VARKON_TMP));
       igldmo();
       igwtma(465);
       strcpy(jobdir,olddir);
       sprintf(tmpnam,"%s%s%s",v3genv(VARKON_TMP),jobnam,MODEXT);
       v3fdel(tmpnam);
       if ( WPgrst("varkon.mbsedit.autoexec",buf)  &&
            strcmp(buf,"True") == 0  &&
            igramo() < 0 ) errmes();
       }
/*
***Fel vid kompilering.
*/
     else
       {
       /*XBell(xdisp,100); */
       WPexla(TRUE);
       if ( igialt(1600,67,68,TRUE) ) goto loop;
       }
     }
/*
***Städa bort MBS-filen.
*/
   sprintf(tmpnam,"%s%s%s",v3genv(VARKON_TMP),jobnam,MBSEXT);
   v3fdel(tmpnam);
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPomod()

/*      Varkonfunktion för editering av partanropad modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1996-02-06 J. Kjellander
 *
 *      1998-04-01 Ny WPilse(), J.Kjellander
 *
 ******************************************************!*/

 {
   short status;
   char  mesbuf[V3STRLEN];
   char *pekarr[1000],strarr[20000];
   int   nstr;

   static char namn[JNLGTH+5] = "";
/*
***Skapa filförteckning. mbsdir/XXX.MBS
*/
   igdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Låt användaren välja.
*
   sprintf(mesbuf,"%s %s %s",iggtts(463),mbsdir,iggtts(464));
*/
   status = WPilse(50,50,iggtts(464),namn,pekarr,-1,nstr,namn);
   if ( status < 0 ) return(status);
/*
***Editera.
*/
   edit(mbsdir,namn);
/*
***Kompilera.
*/
   sprintf(mesbuf,"%s %s",iggtts(462),namn);

   if ( WPialt(mesbuf,iggtts(67),iggtts(68),FALSE) )
     {
     if ( comp(mbsdir,namn,mbodir) )
       {
       WPexla(FALSE);
       clheap();
       sprintf(mesbuf,"%s%s %s",namn,MBSEXT,iggtts(466));
       igwlma(mesbuf,IG_MESS);
       }
     else
       {
       XBell(xdisp,100);
       WPexla(TRUE);
       }
     }
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPcoal()

/*      Varkonfunktion för kompilering av alla.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1996-02-06 J. Kjellander
 *
 ******************************************************!*/

 {
   char  mesbuf[V3STRLEN];
   char *pekarr[1000],strarr[20000];
   int   i,nstr,errant;

/*
***Skapa filförteckning. mbsdir/XXX.MBS Resultatet blir
***filer utan extension !!
*/
   igdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Kompilera.
*/
   errant = 0;

   for ( i=0; i<nstr; ++i )
     {
     if ( comp(mbsdir,pekarr[i],mbodir) )
       {
       WPexla(FALSE);
       sprintf(mesbuf,"%s%s %s",pekarr[i],MBSEXT,iggtts(466));
       igwlma(mesbuf,IG_MESS);
       }
     else
       {
       XBell(xdisp,100);
       WPexla(TRUE);
     ++errant;
       }
     }
/*
***Töm PM på anropade moduler.
*/
   clheap();
/*
***Skriv ut hur många och antal fel.
*/
   if ( errant == 0 ) sprintf(mesbuf,"%d %s",nstr,iggtts(467));
   else               sprintf(mesbuf,"%d %s %d",nstr,iggtts(468),errant);
   igwlma(mesbuf,IG_MESS);
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static bool  edit(
        char *dir,
        char *namn)

/*      Editerar MBS-program.
 *
 *      In: dir    = Filkatalog där mbsfilen finns avslutat med en "/".
 *          namn   = Namn utan extension.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 ******************************************************!*/

  {
   char cmd[256],buf[256];

/*
***Bygg ihop sh-kommando. Först ett xterm-fönster eller liknande.
*/
   if ( !WPgrst("varkon.mbsedit.emulator",cmd) ) strcpy(cmd,"xterm -e");
/*
***Sen en editor och en vägbeskrivning till filen som skall editeras.
*/
   strcat(cmd," ");
   if ( !WPgrst("varkon.mbsedit.editor",buf) ) strcpy(buf,"vi");
   strcat(cmd,buf);
   strcat(cmd," ");
   strcat(cmd,dir);
   strcat(cmd,namn);
   strcat(cmd,MBSEXT);
/*
***Utför.
*/
   system(cmd);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static bool  comp(
        char *dir,
        char *namn,
        char *utdir)

/*      Kompilerar MBS-program.
 *
 *      In: dir    = Filkatalog där MBS-filen finns avslutat med en "/".
 *          namn   = Namn med extension. Om filen skall flyttas skall
 *                   namn ges utan extension !!!!!!!!
 *          utdir  = Filkatalog där man vill ha MBO-filen eller NULL
 *                   om samma som MBS. "/" sist.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1462 = Kan ej skapa pipe, cmd = %s
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 ******************************************************!*/

  {
   int   i;
   char  cmd[256],buf[256],from[V3PTHLEN+1],to[V3PTHLEN+1];
   bool  compok;
   FILE *fp;

/*
***Bygg ihop sh-kommando.
*/
   strcpy(cmd,"cd ");
   strcat(cmd,dir);
   cmd[strlen(cmd)-1] = ';';

   if ( !WPgrst("varkon.mbsedit.compiler",buf) ) strcpy(buf,"mbsc");
   strcat(cmd,buf);
   strcat(cmd," ");
   strcat(cmd,namn);
/*
***Utför som pipe.
*/
   if ( (fp=popen(cmd,"r")) == NULL )
     {
     erpush("WP1462",cmd);
     errmes();
     return(0);
     }
/*
***Förbered för listning.
*/
   strcpy(buf,iggtts(469));
   strcat(buf,namn);
   strcat(buf,MBSEXT);
   WPinla(buf);
/*
***Läs från pipen och kolla om det gick bra.
*/
   compok = FALSE;

   while ( fgets(buf,255,fp) != NULL )
     {
     i = strlen(buf)-1;
     if ( buf[i] == '\n' ) buf[i] = '\0';
     if ( strcmp(buf,"       No compiler detected errors") == 0 ) compok = TRUE;
     WPalla(buf,(short)1);
     }
/*
***Stäng pipen.
*/
   pclose(fp);
/*
***Ev. flyttning av MBO-fil.
*/
   if ( compok  &&  (utdir != NULL)  &&  (strcmp(dir,utdir) != 0 ) )
     {
     strcpy(from,dir);
     strcat(from,namn);
     strcat(from,MODEXT);
     strcpy(to,utdir);
     strcat(to,namn);
     strcat(to,MODEXT);
     if ( v3fmov(from,to) < 0 )
       {
       WPexla(FALSE);
       errmes();
       }
     }
/*
***Slut.
*/
   return(compok);
  }

/********************************************************/
