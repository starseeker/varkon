/*!******************************************************************/
/*  File: varkon.c                                                  */
/*  ==============                                                  */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  main()              Main entrypoint for interactive Varkon      */
/*  IGcheck_jobname();  Check jobname for consitency                */
/*  igcenv();           Check/register WIN32 environment            */
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
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../include/debug.h"
#include "../include/svnversion.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include "../../GE/include/GE.h"
#include <fcntl.h>
#include <string.h>

#ifdef UNIX
#include "/usr/include/sys/utsname.h"
#endif


/*
***Default system meta data.
*/
V3MDAT  sydata =

           { 1000,       /* Serial number, default = 1000 */
             1,          /* Version number*/
             19,         /* Revision */
            'C',         /* 2008-02-28 */
             0,0,0,0,0,  /* Dummy times */
             0,0,0,0,0,
             " ",        /* OS name */
             " ",        /* Not used */
             0,          /* Not used */
             0,          /* Not used */
             " ",        /* OS Release */
             " ",        /* OS Version */
             0      } ;  /* Encrypted serial number */

/*
***Current SVN version number. Initialized
***from the contents of IG/include/svnversion.h
*/
char svnversion[V3STRLEN] = SVNVERSION;

/*
***Current system sizes is initialized during startup.
*/
V3MSIZ  sysize;

/*
***Some global data.
*/
char    jobnam[JNLGTH+1] = "";         /* Current job */
char    jobdir[V3PTHLEN+1] = "";       /* Current user job directory */
char    libdir[10*V3PTHLEN+10] = "";   /* Module library path(s) ($VARKON_LIB) */
char    mdffil[V3PTHLEN+1] = "";       /* Current menufile */
char    inifil_1[V3PTHLEN+1] = "";     /* 1:st inifile */
char    inifil_2[V3PTHLEN+1] = "";     /* 2:nd inifile */

/*
***Current module type. From 1.19B modtyp is
***always 3D. modtyp 2D is obsolete.
*/
short modtyp = _3D;

/*
***Current module attribute.
***modatt = LOCAL, GLOBAL, BASIC or MACRO
*/
short modatt;

/*
***Largest ID sequence number in current module.
*/
DBseqnum snrmax;

/*
***Current pointer to start of active module and top
***of PM stack.
*/
pm_ptr actmod,pmstkp;

/*
***Flag for temporary references.
*/
bool tmpref;

/*
***0 <= posmode <= 8 controls the current
***method to use for positional input.
*** 0 = Absolute, 1 = Relative,  2 = Cursor
*** 3 = MBS,      4 = End,       5 = On
*** 6 = Center,   7 = Intersect, 8 = Grid
***posmode defaults to 2 = Cursor and is managed
***through the buttons in the menu window. The
***rel button (button 1) alters the meaning of
***the 8 other buttons by adding an offset.
*/
int  posmode = 2;
bool relpos  = FALSE;

/*
***Name of active coordinate system.
*/
char actcnm[JNLGTH+1];

/*
***Number of active function, name of active part
***and flag for <CTRL>c interrupt from keyboard.
***actfunc >  0 => Number of active function
***        = -1 => Menu active
***        = -2 => Part active
***        =  1 => During startup
*/
int  actfunc = 1;
char actpnm[JNLGTH+1];
bool intrup = FALSE;

/*
***C:s iobuffer for stdout.
*/
char sobuf[BUFSIZ];

/*
***Current system mode.
***NOT_DEFINED  = Not yet defined
***EXPLICIT     = Explicit mode
***GENERIC      = Generic mode
*/
int sysmode;

/*
***A special error tracing technique is used
***during startup to make it easier to trace
***various configuration errors etc. A flag
***is used to determine if the system is in
***startup mode or not and a logfile is opened
***in $VARKON_ROOT during startup for output of
***trace messages.
***startup_complete = TRUE  => System started
***startup_complete = FALSE => System starting
***and messages are added to the logfile.
*/
int   startup_complete;
FILE *startup_logfile;

/*
***Some flags that can be set by command line options.
***igxflg = -x parameter. TRUE => Execute if RES-file is missing.
***iggflg = -g parameter. TRUE => Generate global refs.
***igbflg = -b parameter. TRUE => Batch mode, implies -x.
***igmatt = -LOCAL/-GLOBAL/-BASIC. Default attribute for new modules.
*/
bool  igxflg,iggflg,igbflg;
short igmatt;

/*
***Prototypes for internal functions.
*/
static short process_cmdline(int argc, char *argv[]);

/*
***Some defs for WIN32 version of Varkon.
*/
#ifdef WIN32
HWND          ms_main;      /* Application main window. */
int           ms_wmod;      /* Initial mode */
HINSTANCE     ms_inst;      /* Process instancenumber */

extern int   msinit(),msmbox(),msargv();
extern short v3free(),IGmkdr();
extern void *v3mall();
extern bool  IGfacc();
#endif

/********************************************************/

#ifdef UNIX
        int main(ac,av)
        int   ac;     /* arg. count */
        char *av[];   /* argument vector */
#endif

#ifdef WIN32
        int WINAPI WinMain(
        HINSTANCE inst,
        HINSTANCE prev_instr,
        LPSTR     args,
        int       wmode)
#endif

/*      Main program for xvarkon/VARKON.EXE. Inits the
 *      graphical environment, processes command line
 *      parameters and inits various sub packages like
 *      memory allocation, geometry, debug etc. then
 *      calls the main loop.
 *
 *      (C)microform ab 31/10/95 J. Kjellander
 *
 *      2007-01-05 Major rewrite, J.Kjellander
 *      2007-11-18 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;
    int   i;
    char  lfname[V3PTHLEN+V3STRLEN],line[V3STRLEN];

#ifdef WIN32
    int    ercode;
    char   errbuf[V3PTHLEN+80];
    int    i;
    int    argc=25;
    char  *argv[25];
    char  *argbuf;
#endif


/*
***Check that VARKON_ROOT is defined.
*/
   if ( getenv("VARKON_ROOT") == NULL )
     {
     printf("VARKON_ROOT is not defined !\n");
     exit(-1);
     }
/*
***Open startup_logfile.
*/
   strcpy(lfname,getenv("VARKON_ROOT"));
   strcat(lfname,"/varkon.LOG");
   if ( (startup_logfile=fopen(lfname,"w")) == NULL )
     {
     printf("Can't open startup_logfile %s\n",lfname);
     exit(-1);
     }
   fprintf(startup_logfile,"VARKON_ROOT is %s\n",getenv("VARKON_ROOT"));
   fflush(startup_logfile);
/*
***System is now starting up.
*/
   startup_complete = FALSE;
/*
***On the WIN32 platform, WinMain() supplies two parameters
***that must be saved.
*/
#ifdef WIN32
   ms_inst = inst;
   ms_wmod = wmode;
#endif
/*
***First thing to do now is to init the C memory allocation
***system so that called functions can allocate memory.
*/
   v3mini();
   fprintf(startup_logfile,"Varkon memory allocation system initialized\n");
   fflush(startup_logfile);
/*
***Set up the size of the outputbuffer to stdout.
*/
   setbuf(stdout,sobuf);
/*
***Init system data. This function has a misleading name.
*/
   IGckhw();
   fprintf(startup_logfile,"Varkon system data initialized\n");
   fflush(startup_logfile);
/*
***Init debug before processing command line parameters.
*/
   dbgini();
   fprintf(startup_logfile,"Varkon debug system initialized\n");
   fflush(startup_logfile);
/*
***Process command line parameters.
*/
   fprintf(startup_logfile,"Processing command line\n");
   for ( i=0; i<ac; ++i )
     {
     fprintf(startup_logfile,"%d %s\n",i,av[i]);
     fflush(startup_logfile);
     }

   process_cmdline(ac,av);

   fprintf(startup_logfile,"Command line sucessfully processed\n");
/*
***Init signals and error handling.
*/
   IGsini();
   erinit();

   fprintf(startup_logfile,"Varkon error system initialized\n");
   fflush(startup_logfile);
/*
***Init level names.
*/
   EXinit_levels();

   fprintf(startup_logfile,"Varkon levels initialized\n");
   fflush(startup_logfile);
/*
***Init WP.
*/
   fprintf(startup_logfile,"inifil_1 = %s\n",inifil_1);
   fflush(startup_logfile);
   fprintf(startup_logfile,"inifil_2 = %s\n",inifil_2);
   fflush(startup_logfile);

   if ( WPinit(inifil_1,inifil_2) < 0 )
     {
     fprintf(startup_logfile,"Can't init WinPac (WP)\n");
     fflush(startup_logfile);
     status = EREXIT;
     goto end;
     }

   fprintf(startup_logfile,"WinPac (WP) initialized\n");
   fflush(startup_logfile);
/*
***If sysmode is not supplied on the command line,
***ask the user what to do.
*/
   if ( sysmode == TOP_MOD )
     {
     if ( (status=WPselect_sysmode(&sysmode)) < 0 )
       {
       if ( status == REJECT )
         {
         fprintf(startup_logfile,"WPselect_sysmode rejected\n");
         status = V3EXOK;
         }
       else
         {
         fprintf(startup_logfile,"WPselect_sysmode help error\n");
         status = EREXIT;
         }
       fflush(startup_logfile);
       goto end;
       }
     }
   fprintf(startup_logfile,"sysmode set to: %d\n",sysmode);
   fflush(startup_logfile);
/*
***Load menufile.
*/
   strcpy(mdffil,getenv("VARKON_MDF"));

   fprintf(startup_logfile,"mdffile set to: %s\n",mdffil);
   fflush(startup_logfile);

   if ( IGinit(mdffil) < 0 )
     {
     fprintf(startup_logfile,"Error loading menus\n");
     fflush(startup_logfile);
     status = EREXIT;
     goto end;
     }

   fprintf(startup_logfile,"MDF file loaded\n");
   fflush(startup_logfile);
/*
***job directory. If a job directory was not specified
***on the command line use VARKON_ROOT.
*/
   if ( jobdir[0] == '\0' )
     {
     strcpy(jobdir,getenv("VARKON_ROOT"));
     }

   if ( jobdir[strlen(jobdir)-1] != '/' ) strcat(jobdir,"/");

   fprintf(startup_logfile,"jobdir set to: %s\n",jobdir);
   fflush(startup_logfile);
/*
***User library path.
*/
   strcpy(libdir,getenv("VARKON_LIB"));
   strcat(libdir,"/");
   fprintf(startup_logfile,"libdir set to: %s\n",libdir);
   fflush(startup_logfile);
/*
***VARKON_ROOT, VARKON_LIB and VARKON_MDF have now been processed.
***Print remaining environment variable values to the logfile.
*/
   fprintf(startup_logfile,"$VARKON_DOC=%s/\n",getenv("VARKON_DOC"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_ERM=%s/\n",getenv("VARKON_ERM"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_TMP=%s/\n",getenv("VARKON_TMP"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_FNT=%s/\n",getenv("VARKON_FNT"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_PLT=%s/\n",getenv("VARKON_PLT"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_ICO=%s/\n",getenv("VARKON_ICO"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_TOL=%s/\n",getenv("VARKON_TOL"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_INI=%s/\n",getenv("VARKON_INI"));
   fflush(startup_logfile);
   fprintf(startup_logfile,"$VARKON_BIN=%s/\n",getenv("VARKON_BIN"));
   fflush(startup_logfile);
/*
***Init surpac.
*/
   if ( suinit() < 0 )
     {
     status = EREXIT;
     goto end;
     }

   fprintf(startup_logfile,"Surpac initialized\n");
   fflush(startup_logfile);
/*
***If no jobname is supplied on the command line, ask for one.
*/
   if ( jobnam[0] == '\0' )
     {
     status = IGselect_job(jobnam);

     if ( status == 0 )
       {
       fprintf(startup_logfile,"jobnam set to: %s\n",jobnam);
       fflush(startup_logfile);
       }
     else if ( status == REJECT )
       {
       status = V3EXOK;
       goto end;
       }
     else
       {
       status = EREXIT;
       goto end;
       }
     }
/*
***Init WP's menuhandler.
*/
    if ( !igbflg  &&  (status=WPinit_menu()) < 0 )
      {
      status = EREXIT;
      goto end;
      }
   fprintf(startup_logfile,"Menu handler initialized\n");
   fflush(startup_logfile);
/*
***Start Varkon in right mode.
*/
   fprintf(startup_logfile,"Calling main loop\n");
   fflush(startup_logfile);

   if ( sysmode == GENERIC ) status = IGgeneric();
   else                      status = IGexplicit();
/*
***Varkon is now running. When we reach this point
***we are about to make a normal system close down
***or a close down because of some problem (EREXIT)
***that we must report to the user before ending.
*/
end:

   if ( status == EREXIT )
     {
     WPinla("Startup log !");
     fclose(startup_logfile);
     startup_logfile=fopen(lfname,"r");
     while ( fgets(line,V3STRLEN,startup_logfile) != NULL )
       {
       for ( i=0; i<V3STRLEN; ++i ) if ( line[i] == '\n' ) line[i] = '\0';
       line[i] = '\0';
       WPalla(line,1);
       }
     WPalla("",2);
     WPalla("Something is wrong with your Varkon installation.",1);
     WPalla("This startup log is stored as varkon.LOG on your Varkon",1);
     WPalla("installation root directory and can be a good help to",1);
     WPalla("understand what the problem is.",2);
     WPalla("If you need help, you can contact the Varkon developers",1);
     WPalla("through the development site:",2);
     WPalla("http://sourceforge.net/projects/varkon",1);
     WPexla(TRUE);
     IGials("Varkon will now terminate !","Ok","Ok",TRUE);
     IGexit();
     }
/*
***Normal exit, remove logfile.
*/
   else
     {
     fclose(startup_logfile);
     IGfdel(lfname);
     IGexit();
     }
/*
***The following line is there to avoid compiler warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short process_cmdline(
        int   argc,
        char *argv[])

/*      Process command line arguments (parameters)
 *      and save result in global variables. In the
 *      case of an error a message is written to the
 *      startup_logfile and the system is closed down.
 *
 *      In: argc => Argument count
 *          argv => Argument string ptrs
 *
 *      Return: 0 = OK
 *
 *     (C)microform ab Johan Kjellander  8/5/89
 *
 *     17/6/92  Debug, J. Kjellander
 *     14/11/94 Parametern -global_ref, J. Kjellander
 *     1998-03-12 2 inifiler, J.Kjellander
 *     2007-11-07 2.0, J.Kjellander
 *
 ******************************************************!*/

{
   int i,ival;

/*
***Default values.
*/
   jobnam[0]   = '\0';
   sysize.pm   = PMSIZE;
   sysize.gm   = 0;
   sysize.rts  = RTSSIZE;
   igbflg      = FALSE;
   igxflg      = FALSE;
   iggflg      = FALSE;
   igmatt      = GLOBAL;
   sysmode     = TOP_MOD;
   inifil_1[0] = '\0';
   inifil_2[0] = '\0';
/*
***Parse the argument list. With UNIX we first call
***WPmrdb() to remove any arguments related to X.
*/
#ifdef UNIX
   WPmrdb(&argc,argv);
#endif
/*
***Loop through all arguments.
*/
   for ( i=1; i<argc; ++i)
     {
/*
***If leading minus sign (-) is missing this must be
***the job name. Check that the name is valid.
*/
     if ( argv[i][0] != '-')
       {
       if ( jobnam[0] == '\0' )
         {
         if ( IGcheck_jobname(argv[i]) < 0 )
           {
           fprintf(startup_logfile,"Invalid jobname: %s\n",argv[i]);
           fclose(startup_logfile);
           exit(V3EXOK);
           }
         else strcpy(jobnam,argv[i]);
         }
       else goto usage;
       }
/*
***Arguments with leading minus sign (-).
*/
     else
       {
/*
***Module attribute.
*/
       if      ( sticmp(&argv[i][1],"LOCAL")     == 0 ) igmatt = LOCAL;
       else if ( sticmp(&argv[i][1],"GLOBAL")    == 0 ) igmatt = GLOBAL;
       else if ( sticmp(&argv[i][1],"BASIC")     == 0 ) igmatt = BASIC;
/*
***System mode.
*/
       else if ( sticmp(&argv[i][1],"EXPLICIT")  == 0 ) sysmode = EXPLICIT;
       else if ( sticmp(&argv[i][1],"GENERIC")   == 0 ) sysmode = GENERIC;
       else
         {
         switch (argv[i][1])
           {
/*
***Job directory.
*/
           case 'j':
           if ( argv[i][2] == 'd'  &&
                sscanf(&argv[i][3],"%s",jobdir) == 1 )
             {
             break;
             }
           else goto usage;
/*
***PM RAM size.
*/
           case 'p':
           if ( argv[i][2] == 'm'  &&
                sscanf(&argv[i][3],"%d",&ival) == 1  && ival > 0 )
             {
             sysize.pm = ival*1024;
             break;
             }
           else goto usage;
/*
***RTS size.
*/
           case 'r':
           if ( argv[i][2] == 't'  &&  argv[i][3] == 's'  &&
                sscanf(&argv[i][4],"%d",&ival) == 1  && ival > 0 )
             {
             sysize.rts = ival*1024;
             break;
             }
           else goto usage;
/*
***DB RAM buffer size. Round to closest number of pages.
*/
           case 'd':
           if ( argv[i][2] == 'b'  &&
                sscanf(&argv[i][3],"%d",&ival) == 1  && ival > 0 )
             {
             sysize.gm = ival*1024;
             sysize.gm = sysize.gm/PAGSIZ;
             break;
             }
/*
***Use global_ref in part calls.
*/
           else if ( sticmp(&argv[i][1],"GLOBAL_REF") == 0 )
             {
             iggflg = TRUE;
             break;
             }
           else goto usage;
/*
***Print version.
*/
           case 'v':
           case 'V':
           if ( argv[i][2] == '\0' )
             {
             printf("VARKON %d.%d%c svn#%s\n",sydata.vernr,sydata.revnr,
                                              sydata.level,svnversion);
             exit(V3EXOK);
             }
           else goto usage;
/*
***Automatically run active module if RES-file is missing.
*/
           case 'x':
           igxflg = TRUE;
           break;
/*
***Run in debug mode.
*/
           case 'D':
           if ( dbgon(&argv[i][2]) < 0 ) goto usage;
           break;
/*
***Max 2 init-files.
*/
           case 'I':
           case 'i':
           if      ( inifil_1[0] == '\0' ) strcpy(inifil_1,&argv[i][2]);
           else if ( inifil_2[0] == '\0' ) strcpy(inifil_2,&argv[i][2]);
           else goto usage;
           break;
/*
***Batch mode.
*/
           case 'b':
           igxflg = igbflg = TRUE;
           sysmode = GENERIC;
           break;
/*
***Illegal option.
*/
           default:
           goto usage;
           }
         }
       }
     }
/*
***The end.
*/
     return(0);
/*
***Error exit.
*/
usage:
     fprintf(startup_logfile,"Illegal option: %s\n",argv[i]);
     fclose(startup_logfile);

     printf("usage: %s (job) (-jd) (-pm) (-gm) (-rts) (-I) (-x) (-b) (-v) (-D)\n",
            argv[0]);
     printf("          (-EXPLICIT) (-GENERIC) (-LOCAL) (-GLOBAL) (-BASIC)\n");
     exit(V3EXOK);
}

/********************************************************/
/*!******************************************************/

        short IGcheck_jobname(char jobnam[])

/*      Check a job name for syntax errors.
 *
 *      In:  jobnam = Name to check.
 *
 *      Return:  0 = OK
 *              -1 = Syntax error
 *
 *     (C)2007-11-25 J.Kjellander
 *
 ******************************************************!*/

{
   int j,jnl;

/*
***Check name length.
*/
    if ( (jnl=strlen(jobnam)) > JNLGTH ) return(-1);
/*
***Check for illegal characters.
*/
    else
      {
      for ( j=0; j<jnl; ++j )
        {
        if ( jobnam[j]  <  33  ||  jobnam[j] == '!'  ||
             jobnam[j] == '&'  ||  jobnam[j] == '}'  ||
             jobnam[j] == ']'  ||  jobnam[j] == '{'  ||
             jobnam[j] == '['  ||  jobnam[j] == '|'  ||
             jobnam[j] == '\\' ||  jobnam[j] == '/'  ||
             jobnam[j] == '*'  ||  jobnam[j] == '?'  ||
             jobnam[j] == '.'  ||  jobnam[j] == ';'  ||
             jobnam[j] == '<'  ||  jobnam[j] == '>' ) return(-1);
        }
      }
/*
***The end.
*/
     return(0);
}

/********************************************************/

#ifdef WIN32
/*!******************************************************/

        int igcenv()

/*      Kollar att nödvändiga ENV-parametrar registrerats
 *      och om så inte är fallet föreslås auto-registrering
 *      med VARKON_ROOT = katalogen ovanför den där varkon
 *      startats ifrån, dvs. den där VARKON95.EXE ligger.
 *
 *      FV:  0.
 *
 *     (C)microform ab 1998-01-03 Johan Kjellander
 *
 *      1998-01-16 C:\TMP, J.Kjellander
 *      2004-10-13 Check environment of this process
 *      2004-10-13 Sets variables also in current process
 *      2004-10-13 Added SendMessageTimeout()
 *      
 ******************************************************!*/

{
   char  path[V3PTHLEN+1],buf[2*V3PTHLEN];
   int   n;
   long  status;
   HKEY  envkey;
   DWORD disposition,dwReturnValue;
   /*
   HKEY  key;
   DWORD size;
   */
   static char  envbuf[V3PTHLEN+1],buf2[2*V3PTHLEN];
 
   
   char *tmp;




/*
***Kolla om VARKON_ROOT finns i registret.
*/

/* Removed by SL, as this is done inderectly by getenv 
   status = RegOpenKeyEx(HKEY_CURRENT_USER,"Environment",
                    (DWORD)0,KEY_QUERY_VALUE,&key);
   if ( status == ERROR_SUCCESS )
     {
     size= V3PTHLEN;
	 status = RegQueryValueEx(key,"VARKON_ROOT",NULL,NULL,envbuf,&size);
     RegCloseKey(key);
     if ( status == ERROR_SUCCESS ) return(0);
     }
*/

/*
***Check for VARKON_ROOT in environment.
*/
tmp = getenv( "VARKON_ROOT" );




/*
***VARKON_ROOT är ej registrerat.
   if ( status != ERROR_SUCCESS )
*/
   if ( tmp == NULL )


     {
     if ( MessageBox(NULL,
                     "No registry settings found, register default ?",
                     "VARKON Configuration error",
                      MB_ICONQUESTION | MB_YESNO) == IDYES )
       {
/*
***Hämta path till den exe-fil som nu körs.
***Tex. C:\VARKON\BIN\VARKON95.EXE
*/
       GetModuleFileName(NULL,path,V3PTHLEN);
/*
***Ta bort 2 sista nivåerna i vägbeskrivningen,
***dvs. troligen "\BIN\VARKON95.EXE".
*/
       n = strlen(path);
       while ( --n > 0  &&  path[n] != '\\')
         path[n] = '\0';

       if ( path[n] == '\\' )
         while ( --n > 0  &&  path[n] != '\\')
           path[n] = '\0';

       if ( path[n] == '\\' ) path[n] = '\0';

       strcpy(buf,"Do you want to register ");
       strcat(buf,path);
       strcat(buf," as VARKON:s root directory ?");
       if ( MessageBox(NULL,
                       buf,
                       "VARKON Configuration",
                        MB_ICONQUESTION | MB_YESNO) == IDYES )
         {
/*
***Registrera.
*/
         status = RegCreateKeyEx(HKEY_CURRENT_USER,
                                 "Environment",
                                 0,
                                 NULL,
                                 REG_OPTION_NON_VOLATILE,
                                 KEY_ALL_ACCESS,
                                 NULL,
                                 &envkey,
                                 &disposition);

         strcpy(buf,path);
         RegSetValueEx(envkey,"VARKON_ROOT",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_ROOT=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\PID");
         RegSetValueEx(envkey,"VARKON_PID",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_PID=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\MAN");
         RegSetValueEx(envkey,"VARKON_DOC",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_DOC=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\ERM");
         RegSetValueEx(envkey,"VARKON_ERM",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_ERM=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\CNF\\FNT");
         RegSetValueEx(envkey,"VARKON_FNT",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_FNT=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\CNF\\INI\\ENGLISH");
         RegSetValueEx(envkey,"VARKON_INI",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_INI=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\LIB");
         RegSetValueEx(envkey,"VARKON_LIB",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_LIB=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\MDF\\ENGLISH");
         RegSetValueEx(envkey,"VARKON_MDF",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_MDF=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\CNF\\PLT");
         RegSetValueEx(envkey,"VARKON_PLT",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_PLT=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\APP");
         RegSetValueEx(envkey,"VARKON_PRD",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_PRD=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\CNF\\SND");
         RegSetValueEx(envkey,"VARKON_SND",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_SND=");
         strcat(buf2,buf);
         _putenv(buf2);

         strcpy(buf,path);
         strcat(buf,"\\CNF\\TOL");
         RegSetValueEx(envkey,"VARKON_TOL",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_TOL=");
         strcat(buf2,buf);
         _putenv(buf2);
/*
***TMP lägger vi på den lokala hårddisken C:\TEMP
***Om inte filkatalogen finns skapar vi den dessutom.
*/
         strcpy(buf,"C:\\TEMP");
         RegSetValueEx(envkey,"VARKON_TMP",0,REG_SZ,buf,strlen(buf)+1);
         strcpy(buf2,"VARKON_TMP=");
         strcat(buf2,buf);
         _putenv(buf2);

         if ( !IGfacc(buf,'X') ) IGmkdr(buf);
/*
***Stäng registret.
*/
         RegCloseKey(envkey);



/*
***Notify system that the environment variables has been updated in registry.
***
*/         
         SendMessageTimeout(HWND_BROADCAST,WM_SETTINGCHANGE,0,
              (LPARAM)"Environment",SMTO_ABORTIFHUNG,5000,&dwReturnValue);

         return(0);
         }
       }
	 }

/*
***Slut.
*/
     return(0);
}

/********************************************************/
#endif
