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

extern char jobdir[],jobnam[],mbsdir[],mbodir[];

/*
***For some reason popen and pclose dont seem to be defined
***in stdio.h as they should according to the C documentation.
***To avoid compilation warnings we define them here.
*/
extern FILE *popen(const char *command, const char *type);
extern int   pclose(FILE *stream);

/*
***Prototypes for internal functions.
*/
static bool  edit(char *dir, char *namn);
static bool  comp(char *dir, char *namn, char *utdir);

/********************************************************/

        short WPamod()

/*      Edit the currently active module.
 *
 *      Error: WP1452 Can't create file: %s
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 *      2007-05-27 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   char  tmpnam[V3PTHLEN+1],olddir[V3PTHLEN+1];
   char  buf[256];
   FILE *fp;

/*
***Open temporary file.
*/
   strcpy(tmpnam,IGgenv(VARKON_TMP));
   strcat(tmpnam,jobnam);
   strcat(tmpnam,MBSEXT);

   if ( (fp=fopen(tmpnam,"w")) == NULL )
     {
     erpush("WP1452",tmpnam);
     errmes();
     return(0);
     }
/*
***Decompile actuve module.
*/
   pprmo(PPFILE,fp);
/*
***Close the file.
*/
   fclose(fp);
/*
***Edit.
*/
loop:
   edit(IGgenv(VARKON_TMP),jobnam);
/*
***Compile and optionally execute.
*/
   if ( ( WPgrst("varkon.mbsedit.autocompile",buf)  &&
                                          strcmp(buf,"True") == 0 )  ||
          IGialt(106,67,68,FALSE) )
     {
     if ( comp(IGgenv(VARKON_TMP),jobnam,NULL) )
       {
       WPexla(FALSE);
       strcpy(olddir,jobdir);
       strcpy(jobdir,IGgenv(VARKON_TMP));
       IGldmo();
       WPaddmess_mcwin(IGgtts(465),WP_MESSAGE);
       strcpy(jobdir,olddir);
       sprintf(tmpnam,"%s%s%s",IGgenv(VARKON_TMP),jobnam,MODEXT);
       IGfdel(tmpnam);
       if ( WPgrst("varkon.mbsedit.autoexec",buf)  &&
            strcmp(buf,"True") == 0  &&
            IGramo() < 0 ) errmes();
       }
/*
***Compile errors.
*/
     else
       {
       WPexla(TRUE);
       if ( IGialt(1600,67,68,TRUE) ) goto loop;
       }
     }
/*
***Delete the temporary file.
*/
   sprintf(tmpnam,"%s%s%s",IGgenv(VARKON_TMP),jobnam,MBSEXT);
   IGfdel(tmpnam);
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short WPomod()

/*      Edit a MBS part program.
 *
 *      (C)microform ab 1996-02-06 J. Kjellander
 *
 *      1998-04-01 Ny WPilse(), J.Kjellander
 *      2007-05-27 1.19 J.Kjellander
 *
 ******************************************************!*/

 {
   short status;
   char  mesbuf[V3STRLEN];
   char *pekarr[1000],strarr[20000];
   int   nstr;

   static char namn[JNLGTH+5] = "";
/*
***Create file list. mbsdir/XXX.MBS
*/
   IGdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Let user select.
*/
   status = WPilse(IGgtts(464),namn,pekarr,-1,nstr,namn);
   if ( status < 0 ) return(status);
/*
***Edit.
*/
   edit(mbsdir,namn);
/*
***Compile.
*/
   sprintf(mesbuf,"%s %s",IGgtts(462),namn);

   if ( WPialt(mesbuf,IGgtts(67),IGgtts(68),FALSE) )
     {
     if ( comp(mbsdir,namn,mbodir) )
       {
       WPexla(FALSE);
       clheap();
       sprintf(mesbuf,"%s%s %s",namn,MBSEXT,IGgtts(466));
       WPaddmess_mcwin(mesbuf,WP_MESSAGE);
       }
     else
       {
       XBell(xdisp,100);
       WPexla(TRUE);
       }
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short WPcoal()

/*      Compile all MBS programs in the current MBS directory.
 *
 *      (C)microform ab 1996-02-06 J. Kjellander
 *
 *      2007-05-27 1.19 J.Kjellander
 *
 ******************************************************!*/

 {
   char  mesbuf[V3STRLEN];
   char *pekarr[1000],strarr[20000];
   int   i,nstr,errant;

/*
***Create file list. mbsdir/XXX.MBS
*/
   IGdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Compile all.
*/
   errant = 0;

   for ( i=0; i<nstr; ++i )
     {
     if ( comp(mbsdir,pekarr[i],mbodir) )
       {
       WPexla(FALSE);
       sprintf(mesbuf,"%s%s %s",pekarr[i],MBSEXT,IGgtts(466));
       WPaddmess_mcwin(mesbuf,WP_MESSAGE);
       }
     else
       {
       XBell(xdisp,100);
       WPexla(TRUE);
     ++errant;
       }
     }
/*
***Clear PM.
*/
   clheap();
/*
***Report success or error.
*/
   if ( errant == 0 )
     {
     sprintf(mesbuf,"%d %s",nstr,IGgtts(467));
     WPaddmess_mcwin(mesbuf,WP_MESSAGE);
     }
   else
     {
     sprintf(mesbuf,"%d %s %d",nstr,IGgtts(468),errant);
     WPaddmess_mcwin(mesbuf,WP_ERROR);
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

 static bool  edit(
        char *dir,
        char *namn)

/*      Edit MBS program.
 *
 *      In: dir  = File directory with trailing "/".
 *          namn = File name without extension.
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 ******************************************************!*/

  {
   char cmd[256],buf[256];

/*
***Make sh command. Use xterm.
*/
   if ( !WPgrst("varkon.mbsedit.emulator",cmd) ) strcpy(cmd,"xterm -e");
/*
***The actual compile command.
*/
   strcat(cmd," ");
   if ( !WPgrst("varkon.mbsedit.editor",buf) ) strcpy(buf,"vi");
   strcat(cmd,buf);
   strcat(cmd," ");
   strcat(cmd,dir);
   strcat(cmd,namn);
   strcat(cmd,MBSEXT);
/*
***Execute.
*/
   system(cmd);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

 static bool  comp(
        char *dir,
        char *namn,
        char *utdir)

/*      Compile MBS program.
 *
 *      In: dir    = File directory with trailing "/".
 *          namn   = Name with extension. No extension if file
 *                   is moved.
 *          utdir  = File directory for output MBO file or NULL
 *                   if same as MBS. Trailing "/".
 *
 *      Felkoder: WP1462 = Can't create pipe, cmd = %s
 *
 *      (C)microform ab 1996-02-05 J. Kjellander
 *
 *      2007-05-27 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   int   i;
   char  cmd[256],buf[256],from[V3PTHLEN+1],to[V3PTHLEN+1];
   bool  compok;
   FILE *fp;

/*
***Make shell command.
*/
   strcpy(cmd,"cd ");
   strcat(cmd,dir);
   cmd[strlen(cmd)-1] = ';';

   strcat(cmd,"$VARKON_BIN/");

   if ( !WPgrst("varkon.mbsedit.compiler",buf) ) strcpy(buf,"mbsc");
   strcat(cmd,buf);
   strcat(cmd," ");
   strcat(cmd,namn);
/*
***Message to window.
*/
   WPaddmess_mcwin(cmd,WP_MESSAGE);
/*
***Execute pipe.
*/
   if ( (fp=popen(cmd,"r")) == NULL )
     {
     erpush("WP1462",cmd);
     errmes();
     return(0);
     }
/*
***Init listing output.
*/
   strcpy(buf,IGgtts(469));
   strcat(buf,namn);
   strcat(buf,MBSEXT);
   WPinla(buf);
/*
***Read pipe and check for errors.
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
***Close pipen.
*/
   pclose(fp);
/*
***Optional MBO file move.
*/
   if ( compok  &&  (utdir != NULL)  &&  (strcmp(dir,utdir) != 0 ) )
     {
     strcpy(from,dir);
     strcat(from,namn);
     strcat(from,MODEXT);
     strcpy(to,utdir);
     strcat(to,namn);
     strcat(to,MODEXT);
     if ( IGfmov(from,to) < 0 )
       {
       WPexla(FALSE);
       errmes();
       }
     }
/*
***The end.
*/
   return(compok);
  }

/********************************************************/
