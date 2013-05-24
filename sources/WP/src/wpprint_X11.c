/**********************************************************************
*
*    WPprint_X11.c
*    =============
*
*    This file is part of the VARKON WindowPac library.
*    URL: http://www.varkon.com
*
*    WPprint_dialogue();  The print dialogue
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
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"

extern char jobnam[];                        /* Current job name */
extern int  actfunc;                         /* For the help system */

#define PRINTMODE_AUTO   0                   /* Print with autozoom */
#define PRINTMODE_SELECT 1                   /* Print rubberband window */
#define PRINTMODE_MANUAL 2                   /* Print with manual cinfiguration */

static bool   premiere = TRUE;               /* True only for the first invocation */
static int    iwin_x;                        /* The current position of the window */
static int    iwin_y;                        /* even after a user move */

static char   varkon_driver[V3STRLEN];       /* The varkon plotter driver/filter */
static char   unix_command[V3STRLEN];        /* The Unix print command */
static double media_x;                       /* Media size i horisontal direction */
static double media_y;                       /* Media size i vertical direction */
static double xmin;                          /* Plot origin X-coordinate */
static double ymin;                          /* Plot origin Y-coordinate */
static double rotation;                      /* Plot CCW rotation angle */
static double scale;                         /* Plot scale */
static bool   portrait = TRUE;               /* Print orientation */

/*
***Prototypes for internal functions.
*/
static void  start_print(WPGWIN *gwinpt, int mode);
static void  read_config();
static void  write_config();
static void  config_dialogue();
static short manual_config();

/*!******************************************************/

         short WPprint_dialogue(DBint grw_id)

/*      The print dialogue.
 *
 *      In: grw_id = ID of WPGWIN that called us.
 *
 *      (C)2007-04-08 J.Kjellander
 *
 *      2007-09-10 Auto/select/manual, J.Kjellander
 *
 ******************************************************!*/

  {
   char     title[81],autos[81],sel[81],man[81],reject[81],conf[81],
            help[81],autostt[81],seltt[81],mantt[81],rejecttt[81],
            conftt[81],helptt[81];
   char    *typ[20];
   int      l1,l2,ly,lm,bh,main_dx,main_dy,wm_x1,wm_y1,
            wm_x2,wm_y2,alt_x,alt_y,butlen1,butlen2,actfunc_org;
   unsigned int dum1,dum2;
   DBint    iwin_id,auto_id,sel_id,man_id,reject_id,conf_id,help_id,
            but_id;
   WPGWIN  *gwinpt;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;
   XEvent   event;
   XrmValue value;

/*
***Set actfunc during user action, see IG/include/futab.h.
*/
   actfunc_org = actfunc;
   actfunc = 150;
/*
***Is this the first time this function is called ?
***Establish initial window position and load printer
***configuration into static variables.
*/
   if ( premiere )
     {
     iwin_x = iwin_y = 5;

     if ( XrmGetResource(xresDB,"varkon.print.geometry",
                                "Varkon.Print.Geometry",
                                 typ,&value) ) 
       XParseGeometry((char *)value.addr,&iwin_x,&iwin_y,&dum1,&dum2);

     read_config();
     premiere = FALSE;
     }
/*
***Get a C ptr to the graphical window that called us.
*/
   gwinpt = (WPGWIN *)wpwtab[grw_id].ptr;
/*
***Window title, portrait, landscape, configure,
***cancel and help from the ini-file.
*/
   if ( !WPgrst("varkon.print.title",title) )              strcpy(title,"Print");

   if ( !WPgrst("varkon.print.auto",autos) )               strcpy(autos,"Auto");
   if ( !WPgrst("varkon.print.auto.tooltip",autostt) )     strcpy(autostt,"");
   if ( !WPgrst("varkon.print.select",sel) )               strcpy(sel,"Select");
   if ( !WPgrst("varkon.print.select.tooltip",seltt) )     strcpy(seltt,"");
   if ( !WPgrst("varkon.print.manual",man) )               strcpy(man,"Manual");
   if ( !WPgrst("varkon.print.manual.tooltip",mantt) )     strcpy(mantt,"");

   if ( !WPgrst("varkon.print.configure",conf) )           strcpy(conf,"Configure");
   if ( !WPgrst("varkon.print.configure.tooltip",conftt) ) strcpy(conftt,"");

   if ( !WPgrst("varkon.input.reject",reject) )            strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) )  strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )                strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.toltip",helptt) )       strcpy(helptt,"");
/*
***What is the 1.2*length of the longest text ?
***Don't include the title, it will fit anyhow.
*/
   butlen1 = 0;
   if ( WPstrl(autos) > butlen1 ) butlen1 = WPstrl(autos);
   if ( WPstrl(sel)   > butlen1 ) butlen1 = WPstrl(sel);
   if ( WPstrl(man)   > butlen1 ) butlen1 = WPstrl(man);
   butlen1 *= 1.8;

   butlen2 = 0;
   if ( WPstrl(reject) > butlen2 ) butlen2 = WPstrl(reject);
   if ( WPstrl(conf)   > butlen2 ) butlen2 = WPstrl(conf);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.2;
/*
***Calculate outside air (ly), button height (bh) and air between (lm).
*/
   ly = (short)(1.2*WPstrh());
   bh = (short)(1.8*WPstrh());
   lm = (short)(0.8*WPstrh());
/*
***Calculate the window size in X-direction.
*/
   l1 = ly + butlen1 + lm + butlen1 + lm + butlen1 + ly;
   l2 = ly + butlen2 + lm + butlen2 + lm + butlen2 + ly;

   if ( l1 > l2 ) main_dx = l1;
   else           main_dx = l2;
/*
***Calculate the window size in Y-direction.
*/
   main_dy = ly + bh + bh + lm + bh + ly;
/*
***Create the dialogue window as a WPIWIN.
*/
   WPwciw((short)iwin_x,(short)iwin_y,main_dx,main_dy,title,&iwin_id);
/*
***Get a C-ptr to the WPIWIN.
*/
   winptr = WPwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Auto, select and manual.
*/
   alt_x  = ly;
   alt_y  = ly;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)1,
                           autos,autos,"",WP_BGND2,WP_FGND,&auto_id);
   butptr = (WPBUTT *)iwinpt->wintab[auto_id].ptr;
   strcpy(butptr->tt_str,autostt);

   alt_x = ly + butlen1 + (main_dx - 2*ly - 3*butlen1)/2;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)1,
                           sel,sel,"",WP_BGND2,WP_FGND,&sel_id);
   butptr = (WPBUTT *)iwinpt->wintab[sel_id].ptr;
   strcpy(butptr->tt_str,seltt);

   alt_x = main_dx - ly - butlen1;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)1,
                           man,man,"",WP_BGND2,WP_FGND,&man_id);
   butptr = (WPBUTT *)iwinpt->wintab[man_id].ptr;
   strcpy(butptr->tt_str,mantt);
/*
***A horizontal line.
*/
   alt_x  = main_dx/8;
   alt_y  += bh + bh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Reject, configure and help.
*/
   alt_x  = ly;
   alt_y += lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)3,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = (main_dx - butlen2)/2;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           conf,conf,"",WP_BGND2,WP_FGND,&conf_id);
   butptr = (WPBUTT *)iwinpt->wintab[conf_id].ptr;
   strcpy(butptr->tt_str,conftt);

   alt_x  = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Show the dialogue.
*/
   WPwshw(iwin_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Where did it actually get positioned ?
***This code copied from WPmsip().
*/
   XWindowEvent(xdisp,iwinpt->id.x_id,ExposureMask,&event);
   XPutBackEvent(xdisp,&event);
   WPgtwp(iwinpt->id.x_id,&wm_x1,&wm_y1);
/*
***Wait for action.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Auto button.
*/
   if ( but_id == auto_id )
     {
     start_print(gwinpt,PRINTMODE_AUTO);
     goto exit;
     }
/*
***Select button. Delete window here so rubberband
***selection is possible.
*/
   else if ( but_id == sel_id )
     {
     WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
     iwin_x = iwin_x + wm_x2 - wm_x1;
     iwin_y = iwin_y + wm_y2 - wm_y1;
     WPwdel(iwin_id);
     start_print(gwinpt,PRINTMODE_SELECT);
     return(0);
     }
/*
***Man button.
*/
   else if ( but_id == man_id )
     {
     start_print(gwinpt,PRINTMODE_MANUAL);
     goto exit;
     }
/*
***Reject button.
*/
   else if ( but_id == reject_id )
     {
     goto exit;
     }
/*
***Config button.
*/
   else if ( but_id == conf_id )
     {
     config_dialogue();
     goto loop;
     }
/*
***Help button.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Unknown event, should not happen.
*/
   else
     {
     WPbell();
     goto loop;
     }
/*
***Time to exit. Remeber the current position.
*/
exit:
   actfunc = actfunc_org;
   WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
   iwin_x = iwin_x + wm_x2 - wm_x1;
   iwin_y = iwin_y + wm_y2 - wm_y1;
   WPwdel(iwin_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static void    start_print(
        WPGWIN *gwinpt,
        int     print_mode)

/*      Creates a GKS plotfile, reads the printer
 *      configuration file and sends the output to
 *      the printer que.
 *
 *      In: gwinpt     = C ptr to WPGWIN to print.
 *          print_mode = AUTO/SELECT/MANUAL
 *
 *      (C)2007-04-08 J.Kjellander
 *
 *      2007-09-10 print_mode, J.Kjellander
 *
 ******************************************************!*/

  {
   char     pltnam[V3PTHLEN],params[V3STRLEN],
            oscmd[V3PTHLEN+V3STRLEN+V3PTHLEN];
   double   x,y,sx,sy,s;
   wpw_id   grw_id;
   int      ix1,iy1,ix2,iy2,tmp;
   FILE    *fp;
   DBVector origo;
   VYWIN    minwin;
   WPWIN   *winptr;
   WPGWIN  *act_gwinpt;

/*
***Read current plotter configuration file.
*/
   read_config();
/*
***Create a temporary plot file name.
*/
   strcpy(pltnam,IGgenv(VARKON_TMP));
   strcat(pltnam,jobnam);
   strcat(pltnam,PLTEXT);
/*
***Open new GKS_meta_file.
*/
   if ( (fp=fopen(pltnam,"w+")) == NULL )
     {
     erpush("EX1742",pltnam);
     errmes();
     return;
     }

   origo.x_gm = origo.y_gm = 0.0;
/*
***What print configuration ?
*/
   switch ( print_mode )
     {
/*
***Auto.
*/
     case PRINTMODE_AUTO:
     WPauto_modwin(gwinpt,&minwin);
     WPmkpf(gwinpt,fp,&minwin,&origo);
     fclose(fp);
/*
***Auto-Portrait.
*/
     if ( portrait )
       {
       sx = media_x/(minwin.xmax - minwin.xmin);
       sy = media_y/(minwin.ymax - minwin.ymin);

       if ( sx < sy )
         {
         s = sx;
         x = 0.0;
         y = -(media_y - (minwin.ymax - minwin.ymin)*s)/2.0;
         }
       else
         {
         s = sy;
         x = -(media_x - (minwin.xmax - minwin.xmin)*s)/2.0;
         y = 0.0;
         }
       sprintf(params," -x%g -y%g -s%g ",x,y,s);
       }
/*
***Auto-Landscape.
*/
     else
       {
       sx = media_y/(minwin.xmax - minwin.xmin);
       sy = media_x/(minwin.ymax - minwin.ymin);

       if ( sx < sy )
         {
         s = sx;
         x = - media_x + (media_x - (minwin.ymax - minwin.ymin)*s)/2.0;
         y = 0.0;
         }
       else
         {
         s = sy;
         x = - media_x;
         y = - (media_y - (minwin.xmax - minwin.xmin)*s)/2.0;
         }
       sprintf(params," -v90 -x%g -y%g -s%g",x,y,s);
       }

     break;
/*
***Select with rubberband rectangle. Use the window
***of the selction. 2 first lines to fix possible
***timing bug in X server.
*/
     case PRINTMODE_SELECT:

     XFlush(xdisp);
     for (ix1=0; ix1<10000000; ++ix1);

     if ( WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,WP_RUB_RECT,TRUE) < 0 ) return;
     if ( ix2 < ix1 ) { tmp = ix1; ix1 = ix2; ix2 = tmp; }
     if ( iy2 < iy1 ) { tmp = iy1; iy1 = iy2; iy2 = tmp; }

     winptr = WPwgwp((wpw_id)grw_id);
     if ( winptr->typ == TYP_GWIN ) act_gwinpt = (WPGWIN *)winptr->ptr;
     else { WPbell(); return; }

     minwin.xmin = act_gwinpt->vy.modwin.xmin +
                   ((ix1 - act_gwinpt->vy.scrwin.xmin)/
                   (act_gwinpt->vy.scrwin.xmax - act_gwinpt->vy.scrwin.xmin))*
                   (act_gwinpt->vy.modwin.xmax - act_gwinpt->vy.modwin.xmin);
     minwin.xmax = act_gwinpt->vy.modwin.xmin +
                   ((ix2 - act_gwinpt->vy.scrwin.xmin)/
                   (act_gwinpt->vy.scrwin.xmax - act_gwinpt->vy.scrwin.xmin))*
                   (act_gwinpt->vy.modwin.xmax - act_gwinpt->vy.modwin.xmin);
     minwin.ymin = act_gwinpt->vy.modwin.ymin +
                   ((iy1 - act_gwinpt->vy.scrwin.ymin)/
                   (act_gwinpt->vy.scrwin.ymax - act_gwinpt->vy.scrwin.ymin))*
                   (act_gwinpt->vy.modwin.ymax - act_gwinpt->vy.modwin.ymin);
     minwin.ymax = act_gwinpt->vy.modwin.ymin +
                   ((iy2 - act_gwinpt->vy.scrwin.ymin)/
                   (act_gwinpt->vy.scrwin.ymax - act_gwinpt->vy.scrwin.ymin))*
                   (act_gwinpt->vy.modwin.ymax - act_gwinpt->vy.modwin.ymin);

     WPmkpf(act_gwinpt,fp,&minwin,&origo);
     fclose(fp);
/*
***Select-Portrait.
*/
     if ( portrait )
       {
       sx = media_x/(minwin.xmax - minwin.xmin);
       sy = media_y/(minwin.ymax - minwin.ymin);

       if ( sx < sy )
         {
         s = sx;
         x = 0.0;
         y = -(media_y - (minwin.ymax - minwin.ymin)*s)/2.0;
         }
       else
         {
         s = sy;
         x = -(media_x - (minwin.xmax - minwin.xmin)*s)/2.0;
         y = 0.0;
         }
       sprintf(params," -x%g -y%g -s%g ",x,y,s);
       }
/*
***Select-Landscape.
*/
     else
       {
       sx = media_y/(minwin.xmax - minwin.xmin);
       sy = media_x/(minwin.ymax - minwin.ymin);

       if ( sx < sy )
         {
         s = sx;
         x = - media_x + (media_x - (minwin.ymax - minwin.ymin)*s)/2.0;
         y = 0.0;
         }
       else
         {
         s = sy;
         x = - media_x;
         y = - (media_y - (minwin.xmax - minwin.xmin)*s)/2.0;
         }
       sprintf(params," -v90 -x%g -y%g -s%g",x,y,s);
       }
     break;
/*
***Manual configuration. Use all params from config file.
*/
     case PRINTMODE_MANUAL:
     WPmkpf(gwinpt,fp,&gwinpt->vy.modwin,&origo);
     fclose(fp);
     if ( manual_config() == REJECT ) return;
     sprintf(params," -x%g -y%g -v%g -s%g ",xmin,ymin,rotation,scale);
     break;
     }
/*
***Make shell command line.
*/
   strcpy(oscmd,"$VARKON_BIN/");
   strcat(oscmd,varkon_driver);
   strcat(oscmd," < ");
   strcat(oscmd,pltnam);
   strcat(oscmd,params);
   strcat(oscmd,unix_command);
   strcat(oscmd,";rm ");
   strcat(oscmd,pltnam);
/*
***Confirmational message.
*/
   WPaddmess_mcwin(oscmd,WP_MESSAGE);
/*
***Start plot.
*/
   EXos(oscmd,0);
/*
***The end.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

 static void read_config()

/*      Read the configuration parameters from the
 *      config file.
 *
 *      Error: WP1692 = Can't open %s
 *             WP1702 = Error reading %s
 *
 *      (C)2007-04-08 J.Kjellander
 *
 *      2007-09-10 removed autofit, J.Kjellander
 *
 ******************************************************!*/

  {
   char  cnfnam[V3PTHLEN+1],line[V3STRLEN+1];
   int   nl;
   FILE *fp;
/*
***Path and name for plotter configuration file.
*/
   strcpy(cnfnam,IGgenv(VARKON_PLT));
   strcat(cnfnam,"plot.cnf");
/*
***Open the file file.
*/
   if ( (fp=fopen(cnfnam,"r")) == NULL )
     {
     erpush("WP1692",cnfnam);
     errmes();
     return;
     }
/*
***Read Varkon_driver:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Varkon_driver:",14) != 0 ) goto error;
   nl = strlen(line) - 1;
   if ( line[nl] == '\n' ) line[nl] = '\0';
   strcpy(varkon_driver,&line[14]);
/*
***Read Unix_print_command:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Unix_print_command:",19) != 0 ) goto error;
   nl = strlen(line) - 1;
   if ( line[nl] == '\n' ) line[nl] = '\0';
   strcpy(unix_command,&line[19]);
/*
***Read Media_size_x:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Media_size_x:",13) != 0 ) goto error;
   sscanf(&line[13],"%lf",&media_x);
/*
***Read Media_size_y:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Media_size_y:",13) != 0 ) goto error;
   sscanf(&line[13],"%lf",&media_y);
/*
***Read Xmin:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Xmin:",5) != 0 ) goto error;
   sscanf(&line[5],"%lf",&xmin);
/*
***Read Ymin:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Ymin:",5) != 0 ) goto error;
   sscanf(&line[5],"%lf",&ymin);
/*
***Read Rotation:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Rotation:",9) != 0 ) goto error;
   sscanf(&line[9],"%lf",&rotation);
/*
***Read Scale:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Scale:",6) != 0 ) goto error;
   sscanf(&line[6],"%lf",&scale);
/*
***Skip possible Autofit: from 1.19B and get
***Portrait:
*/
   if ( fgets(line,V3STRLEN,fp) == NULL ) goto error;
   if ( strncmp(line,"Autofit:",8) == 0 )
     {
     if ( fgets(line,V3STRLEN,fp) == NULL ) strcpy(line,"Portrait: True");
     }

   if ( strncmp(line,"Portrait:",9) != 0 ) goto error;
   if ( strncmp(&line[10],"True",4) == 0 ) portrait = TRUE;
   else                                    portrait = FALSE;
/*
***Close the plotter configuration file.
*/
   fclose(fp);
/*
***The end.
*/
   return;
/*
***Error exit.
*/
error:
   erpush("WP1702",cnfnam);
   errmes();
   return;
  }

/********************************************************/
/*!******************************************************/

 static void write_config()

/*      Write the configuration parameters to the
 *      config file.
 *
 *      Error: WP1692 = Can't open %s
 *
 *      (C)2007-04-08 J.Kjellander
 *
 *      2007-09-10 removed autofit, J.Kjellander
 *
 ******************************************************!*/

  {
   char  cnfnam[V3PTHLEN+1];
   int   i;
   FILE *fp;

/*
***Path and name for plotter configuration file.
*/
   strcpy(cnfnam,IGgenv(VARKON_PLT));
   strcat(cnfnam,"plot.cnf");
/*
***Open the file file.
*/
   if ( (fp=fopen(cnfnam,"w")) == NULL )
     {
     erpush("WP1692",cnfnam);
     errmes();
     return;
     }
/*
***Write Varkon_driver: Strip leading blanks.
*/
   i = 0; while ( varkon_driver[i] == ' ' ) ++i;
   fprintf(fp,"Varkon_driver:%s\n",&varkon_driver[i]);
/*
***Write Unix_print_command:
*/
   i = 0; while ( unix_command[i] == ' ' ) ++i;
   fprintf(fp,"Unix_print_command: %s\n",&unix_command[i]);
/*
***Write Media_size_x:
*/
   fprintf(fp,"Media_size_x: %g\n",media_x);
/*
***Write Media_size_y:
*/
   fprintf(fp,"Media_size_y: %g\n",media_y);
/*
***Write Xmin:
*/
   fprintf(fp,"Xmin: %g\n",xmin);
/*
***Write Ymin:
*/
   fprintf(fp,"Ymin: %g\n",ymin);
/*
***Write Rotation:
*/
   fprintf(fp,"Rotation: %g\n",rotation);
/*
***Write Scale:
*/
   fprintf(fp,"Scale: %g\n",scale);
/*
***Write Portrait:
*/
   if ( portrait ) fprintf(fp,"Portrait: True\n");
   else            fprintf(fp,"Portrait: False\n");
/*
***Close the plotter configuration file.
*/
   fclose(fp);
/*
***The end.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

 static void config_dialogue()

/*      The dialog for printer configuration.
 *
 *      (C)2007-04-08 J.Kjellander
 *
 *      2007-09-10 Portrait/landscape, J.Kjellander
 *
 ******************************************************!*/

  {
   char     title[81],drv[81],drvtt[81],cmd[81],cmdtt[81],
            media[81],mediaxtt[81],mediaytt[81],mxstr[81],
            mystr[81],orient[81],port[81],porttt[81],land[81],
            landtt[81],okey[81],reject[81],help[81],okeytt[81],
            rejecttt[81],helptt[81];
   int      ly,bh,lm,main_x,main_y,main_dx,main_dy,butlen1,
            butlen2,butlen3,edlen,l1,l2,alt_x,alt_y;
   DBint    iwin_id,drv_id,cmd_id,mx_id,my_id,port_id,land_id,
            okey_id,reject_id,help_id,but_id;
   double   tmpx,tmpy;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;
   WPEDIT  *edtptr;

/*
***Window title,
***cancel and help from the ini-file.
*/
   if ( !WPgrst("varkon.print.configure.title",title) )    strcpy(title,"Configuration");
   if ( !WPgrst("varkon.print.driver",drv) )               strcpy(drv,"Driver");
   if ( !WPgrst("varkon.print.driver.tooltip",drvtt) )     strcpy(drvtt,"");
   if ( !WPgrst("varkon.print.command",cmd) )              strcpy(cmd,"Command");
   if ( !WPgrst("varkon.print.command.tooltip",cmdtt) )    strcpy(cmdtt,"");
   if ( !WPgrst("varkon.print.mediasize",media) )          strcpy(media,"Media size");
   if ( !WPgrst("varkon.print.media_x.tooltip",mediaxtt) ) strcpy(mediaxtt,"");
   if ( !WPgrst("varkon.print.media_y.tooltip",mediaytt) ) strcpy(mediaytt,"");

   if ( !WPgrst("varkon.print.orientation",orient) )       strcpy(orient,"Orientation");
   if ( !WPgrst("varkon.print.portrait",port) )            strcpy(port,"Portrait");
   if ( !WPgrst("varkon.print.portrait.tooltip",porttt) )  strcpy(porttt,"");
   if ( !WPgrst("varkon.print.landscape",land) )           strcpy(land,"Landscape");
   if ( !WPgrst("varkon.print.landscape.tooltip",landtt) ) strcpy(landtt,"");

   if ( !WPgrst("varkon.input.okey",okey) )                strcpy(okey,"Okey");
   if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )      strcpy(okeytt,"");
   if ( !WPgrst("varkon.input.reject",reject) )            strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) )  strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )                strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )      strcpy(helptt,"");
/*
***Text lengths.
*/
   butlen1 = 0;
   if ( WPstrl(drv)   > butlen1 ) butlen1 = WPstrl(drv);
   if ( WPstrl(cmd)   > butlen1 ) butlen1 = WPstrl(cmd);

   edlen = 25*WPstrl("A");

   butlen2 = 0;
   if ( WPstrl(okey)   > butlen2 ) butlen2 = WPstrl(okey);
   if ( WPstrl(reject) > butlen2 ) butlen2 = WPstrl(reject);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.8;

   butlen3 = WPstrl("X:");
/*
***Outside air (ly), button height (bh) and air between (lm).
*/
   ly = (short)(1.4*WPstrh());
   bh = (short)(1.8*WPstrh());
   lm = (short)(0.8*WPstrh());
/*
***Window position.
*/
   main_x = iwin_x + 50;
   main_y = iwin_y + 50;
/*
***Calculate the window size in X-direction.
*/
   l1 = ly + butlen1 + lm + edlen + ly;
   l2 = ly + butlen2 + lm + butlen2 + lm + butlen2 + ly;

   if ( l1 > l2 ) main_dx = l1;
   else           main_dx = l2;
/*
***Calculate the window size in Y-direction.
*/
   main_dy = ly +          /* Driver */
             bh + lm +     /* Command */
             bh + bh +     /* Line */
             lm +          /* Media size */
             bh + lm +     /* X and Y */
             bh + bh +     /* Line */
             lm +          /* Plot scale */
             bh + lm +     /* Portrait and Landscape */
             bh + bh +     /* Line */
             lm +          /* Ok, Cancel and Help */
             bh + ly;      /* The rest */
/*
***Create the dialogue window as a WPIWIN.
*/
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,title,&iwin_id);
/*
***Get a C-ptr to the WPIWIN.
*/
   winptr = WPwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Read the config file now.
*/
   read_config();
/*
***Driver.
*/
   alt_x = ly;
   alt_y = ly;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(drv),bh,drv,&but_id);

   alt_x  += butlen1 + lm;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edlen,bh,(short)1,varkon_driver,80,&drv_id);
   edtptr = (WPEDIT *)iwinpt->wintab[drv_id].ptr;
   strcpy(edtptr->tt_str,drvtt);
/*
***Command.
*/
   alt_x = ly;
   alt_y += bh + lm;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(cmd),bh,cmd,&but_id);

   alt_x  += butlen1 + lm;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edlen,bh,(short)1,unix_command,80,&cmd_id);
   edtptr = (WPEDIT *)iwinpt->wintab[cmd_id].ptr;
   strcpy(edtptr->tt_str,cmdtt);
/*
***A horizontal line.
*/
   alt_x  = main_dx/8;
   alt_y  += bh + bh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Media size.
*/
   alt_x = 0;
   alt_y += lm;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,main_dx,bh,media,&but_id);

   alt_x = 3*ly;
   alt_y += bh + lm;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,butlen3,bh,"X:",&but_id);

   alt_x  += butlen3 + lm;
   sprintf(mxstr,"%g",media_x);
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edlen/2,bh,(short)1,mxstr,80,&mx_id);
   edtptr = (WPEDIT *)iwinpt->wintab[mx_id].ptr;
   strcpy(edtptr->tt_str,mediaxtt);

   alt_x = main_dx - 3*ly - edlen/2 - lm - butlen3;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,butlen3,bh,"Y:",&but_id);

   alt_x  += butlen3 + lm;
   sprintf(mystr,"%g",media_y);
   WPmced((wpw_id)iwin_id,alt_x,alt_y,edlen/2,bh,(short)1,mystr,80,&my_id);
   edtptr = (WPEDIT *)iwinpt->wintab[my_id].ptr;
   strcpy(edtptr->tt_str,mediaytt);
/*
***A horizontal line.
*/
   alt_x  = main_dx/8;
   alt_y  += bh + bh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Portrait and Landscape.
*/
   alt_x = 0;
   alt_y += lm;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,main_dx,bh,orient,&but_id);

   alt_x  = 3*ly;
   alt_y += bh + lm;
   if ( portrait )
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           port,port,"",WP_BGND3,WP_FGND,&port_id);
   else
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           port,port,"",WP_BGND2,WP_FGND,&port_id);

   butptr = (WPBUTT *)iwinpt->wintab[port_id].ptr;
   strcpy(butptr->tt_str,porttt);


   alt_x  = main_dx - 3*ly - butlen2;
   if ( portrait )
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                             land,land,"",WP_BGND2,WP_FGND,&land_id);
   else
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                             land,land,"",WP_BGND3,WP_FGND,&land_id);

   butptr = (WPBUTT *)iwinpt->wintab[land_id].ptr;
   strcpy(butptr->tt_str,landtt);
/*
***A horizontal line.
*/
   alt_x  = main_dx/8;
   alt_y  += bh + bh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Okey, Reject and help.
*/
   alt_x  = ly;
   alt_y += lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)3,
                           okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   alt_x  = (main_dx - butlen2)/2;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)1,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Show the dialogue.
*/
   WPwshw(iwin_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action. SMBPOSM is not an issue in this situation.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Portrait button.
*/
   if ( but_id == port_id )
     {
     portrait = TRUE;
     butptr = (WPBUTT *)iwinpt->wintab[port_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[land_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Landscape button.
*/
   else if ( but_id == land_id )
     {
     portrait = FALSE;
     butptr = (WPBUTT *)iwinpt->wintab[port_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[land_id].ptr;
     WPscbu(butptr,WP_BGND3);
     goto loop;
     }
/*
***Okey button.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,mx_id,mxstr);
     if ( sscanf(mxstr,"%lf",&tmpx) < 1 )
       {
       WPbell();
       goto loop;
       }

     WPgted(iwin_id,my_id,mystr);
     if ( sscanf(mystr,"%lf",&tmpy) < 1 )
       {
       WPbell();
       goto loop;
       }

     WPgted(iwin_id,drv_id,varkon_driver);
     WPgted(iwin_id,cmd_id,unix_command);
     media_x = tmpx;
     media_y = tmpy;

     write_config();

     goto exit;
     }
/*
***Reject button.
*/
   else if ( but_id == reject_id )
     {
     goto exit;
     }
/*
***Help button.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Other events are not important.
*/
   else
     {
     goto loop;
     }
/*
***Time to exit.
*/
exit:
   WPwdel(iwin_id);
   return;
  }

/********************************************************/
/*!******************************************************/

 static short manual_config()

/*      Manually configure scale and orientation.
 *
 *      (C)2007-04-08 J.Kjellander
 *
 ******************************************************!*/

  {
   char   ox_pmt[81],oy_pmt[81],r_pmt[81],s_pmt[81],
          dox_str[81],doy_str[81],dr_str[81],ds_str[81],
          iox_str[81],ioy_str[81],ir_str[81],is_str[81],
          title[81];
   short  status,maxlen[4];
   int    typarr[4];
   double tmpx,tmpy,tmpr,tmps;
   char  *pmtarr[4];
   char  *dstarr[4];
   char  *istarr[4];

/*
***Promts from the ini-file.
*/
   if ( !WPgrst("varkon.print.manual.title",title) )    strcpy(title,"Configuration");
   if ( !WPgrst("varkon.print.manual.rotation",r_pmt) ) strcpy(r_pmt,"Rotation");
   if ( !WPgrst("varkon.print.manual.xmin",ox_pmt) )    strcpy(ox_pmt,"Xmin");
   if ( !WPgrst("varkon.print.manual.ymin",oy_pmt) )    strcpy(oy_pmt,"Ymin");
   if ( !WPgrst("varkon.print.manual.scale",s_pmt) )    strcpy(s_pmt,"Scale");
/*
***Max input lengths. Origin_x, Origin_y, Rotation and Scale.
*/
   maxlen[0] = maxlen[1] = maxlen[2] = maxlen[3] = 10;
/*
***Promts.
*/
   pmtarr[0] = r_pmt;
   pmtarr[1] = ox_pmt;
   pmtarr[2] = oy_pmt;
   pmtarr[3] = s_pmt;
/*
***Default strings.
*/
   dstarr[0] = dr_str;
   dstarr[1] = dox_str;
   dstarr[2] = doy_str;
   dstarr[3] = ds_str;

   sprintf(dr_str,"%g",rotation);
   sprintf(dox_str,"%g",-xmin);
   sprintf(doy_str,"%g",-ymin);
   sprintf(ds_str,"%g",scale);
/*
***Input strings.
*/
   istarr[0] = ir_str;
   istarr[1] = iox_str;
   istarr[2] = ioy_str;
   istarr[3] = is_str;
/*
***Input types.
*/
   typarr[0] = typarr[1] = typarr[2] = typarr[3] = C_STR_VA;
/*
***Go.
*/
loop:
   status = WPmsip(title,pmtarr,dstarr,istarr,maxlen,typarr,4);
/*
***Check for syntax errors.
*/
   if ( status == 0 )
     {
     if ( sscanf(ir_str,"%lf",&tmpr) < 1 )
       {
       WPbell();
       goto loop;
       }

     if ( sscanf(iox_str,"%lf",&tmpx) < 1 )
       {
       WPbell();
       goto loop;
       }

     if ( sscanf(ioy_str,"%lf",&tmpy) < 1 )
       {
       WPbell();
       goto loop;
       }

     if ( sscanf(is_str,"%lf",&tmps) < 1  ||  tmps < 1E-10 )
       {
       WPbell();
       goto loop;
       }
/*
***Everything ok, update current settings.
*/
     rotation = tmpr;
     xmin     = -tmpx;
     ymin     = -tmpy;
     scale    = tmps;
     write_config();
/*
***The end.
*/
     return(0);
     }
/*
***REJECT.
*/
     else return(REJECT);
  }

/********************************************************/
