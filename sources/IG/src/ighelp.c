/*!******************************************************************/
/*  ighelp.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGhelp();       Interactive context sensitive help system       */
/*  IGabout_help(); List help about help                            */
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
#include "../../EX/include/EX.h"

extern char  jobdir[],actpnm[];
extern short mstack[],mant;
extern int   actfunc;

/*
***Internal function prototypes.
*/
static short display_file(char *file_name);
static int   exec_html_viewer(char *file_path);

/********************************************************/

        short IGhelp()

/*      Displays help on currently active function,
 *      menu or module. A help file name is formed
 *      from the menu number, module name or function
 *      number.
 *
 *      Global variable actfun = -1 => menu
 *                               -2 => module
 *                               >0 => function
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      1998-10-30 HTML, J.Kjellander
 *      2007-12-01 2.0, J.Kjellander
 *
 ********************************************************/

  {
    char  file_name[JNLGTH+5];
    short mnum;

/*
***A module is executing or being called, use module name.
*/
    if ( actfunc == -2 )
      {
      strncpy(file_name,actpnm,JNLGTH+5);
      }
/*
***A menu is active, make name from menu number.
*/
    else if ( actfunc == -1 )
      {
      if ( (mnum=mstack[mant-1]) == 0 ) mnum = IGgmmu();
      file_name[0] = 'm';
      sprintf(&file_name[1],"%d",mnum);
      }
/*
***Function, make name from function number.
*/
    else
      {
      if      ( actfunc < 10  ) sprintf(file_name,"f00%d",actfunc);
      else if ( actfunc < 100 ) sprintf(file_name,"f0%d",actfunc);
      else                      sprintf(file_name,"f%d",actfunc);
      }
/*
***Display.
*/
    return(display_file(file_name));
  }

/********************************************************/
/********************************************************/

        short IGabout_help()

/*      Displays help about help.
 *
 *      (C)2007-12-11 J.Kjellander
 *
 ********************************************************/

  {
   int oldafu;

/*
***Save actfunc and temporarily set it to 153 so
***that f153.htm is displayed.
*/
   oldafu = actfunc;
   actfunc = 153;
/*
***Display.
*/
   IGhelp();
/*
***Reset actfunc.
*/
   actfunc = oldafu;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

static short display_file(char *file_name)

/*      Display's a html help file.
 *
 *      In: file_name = File name.
 *                      partname, m+number or f+number
 *
 *      Return: IG0202 = Can't find help file %s
 *
 *      (C)microform ab 13/10/86 J. Kjellander
 *
 *      5/11/86  GOMAIN, J. Kjellander
 *      15/2/95  VARKON_DOC, J. Kjellander
 *      1998-10-30 HTML, J.Kjellander
 *      1999-03-09 WIN32, J.Kjellander
 *      2007-12-01 2.0, J.Kjellander
 *
 ********************************************************/

  {
   char   file_path[V3PTHLEN+1];
   char   linbuf[V3STRLEN+1];
   short  status;
   int    last,exec_status;
   FILE  *hlpfil;

/*
***Global variable actfunc is used to determine what
***state the system is in. actfunc = 1 means that the
***system is starting and has not yet entered the main
***loop. During this time the WPselect_sysmode() dialog
***may be displayed and help requested from the user.
***This case is specially treated by displaying the top
***help file $VARKON_DOC/index.htm.
***During startup jobdir is not yet defined.
*/
   if ( actfunc == 1 )
     {
     sprintf(file_path,"%sindex.htm",IGgenv(VARKON_DOC));
     if ( IGfacc(file_path,'R') ) goto show;
     else return(erpush("IG0202",file_path));
     }
/*
***Where is the html file ? First try on jobdir.
*/
   strcpy(file_path,jobdir);
   strcat(file_path,file_name);
   strcat(file_path,".htm");
   if ( IGfacc(file_path,'R') ) goto show;
   else                         erpush("IG0202",file_path);
/*
***The file was not found in jobdir.
***Try $VARKON_DOC/GUI.
*/
   sprintf(file_path,"%sGUI/%s.htm",IGgenv(VARKON_DOC),file_name);
   if ( IGfacc(file_path,'R') ) goto show;
   else                         erpush("IG0202",file_path);
/*
***Still no file found. Try with default filenames
***menudoc.htm/partdoc.htm/funcdoc.htm on $VARKON_DOC/GUI
*/
   if      ( actfunc == -1 ) strcpy(file_name,"menudoc");
   else if ( actfunc == -2 ) strcpy(file_name,"partdoc");
   else                      strcpy(file_name,"funcdoc");

   sprintf(file_path,"%sGUI/%s.htm",IGgenv(VARKON_DOC),file_name);
   if ( IGfacc(file_path,'R') ) goto show;
   else                         erpush("IG0202",file_path);
/*
***Nothing left to do but error message.
*/
   errmes();
   return(0);
/*
***A file is found. Display using html viewer if possible.
*/
show:
   erinit();
   exec_status = exec_html_viewer(file_path);

   if ( exec_status == 0 ) return(0);
   else
     {
     sprintf(file_path,"%sno_html_viewer.txt",IGgenv(VARKON_DOC));
     if ( !IGfacc(file_path,'R') ) return(erpush("IG0202",file_path));
     }
/*
***If html viewer is not avalable, use a list window.
*/
   strcpy(linbuf,"Helpfile: ");
   strcat(linbuf,file_path);
   if ( (status=WPinla(linbuf)) < 0 ) goto end;
/*
***List the file. Strip newlines and returns.
*/
   hlpfil = fopen(file_path,"r"); 
   while(fgets(linbuf,V3STRLEN,hlpfil) != NULL)
     {
     last = strlen(linbuf) - 1;
     if ( linbuf[last] == '\n' )
       {
       linbuf[last] = '\0';
     --last;
       }
     if ( linbuf[last] == '\r' ) linbuf[last] = '\0';
     WPalla(linbuf,(short)1);
     }

   WPexla(TRUE);
/*
***The end.
*/
end:
   fclose(hlpfil);
   return(status);
  }

/********************************************************/
/********************************************************/

static int exec_html_viewer(char *file_path)

/*      Executes the html_viewer.
 *
 *      In: file = html file (path+name)
 *
 *      Return: 0 = Ok.
 *             -1 = No viewer available
 *
 *      (C)2007-12-11 J.Kjellander
 *
 ********************************************************/

 {
   char  viewer[V3STRLEN],cmd[V3STRLEN+V3PTHLEN];

/*
***Check that the "html_viewer" resource is defined.
***TODO: Check if the viewer specified exists.
*/
   if ( !IGgrst("html_viewer",viewer) ) return(-1);

   sprintf(cmd,"(%s %s)&",viewer,file_path);
   EXos(cmd,2);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
