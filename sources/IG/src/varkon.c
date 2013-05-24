/*!******************************************************************/
/*  File: varkon.c                                                  */
/*  ==============                                                  */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  main()     Main entrypoint for the interactive Varkon system    */  
/*  igckjn();  Check jobname for consitency                         */
/*  igcenv();  Check/register WIN32 environment                     */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
#include "../include/debug.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include "../../GE/include/GE.h"
#include <string.h>

#ifdef UNIX
#include "/usr/include/sys/utsname.h"
#endif

#ifdef WIN32
#include <io.h>
static int igcenv();
#endif

/*
***Default system meta data.
*/
V3MDAT  sydata =

           { 1000,       /* Serial number, microform = 1000 */
             1,          /* Version number*/
             19,         /* Revision */
            'B',         /* 2007-02-11 */
             0,0,0,0,0,  /* Dummy times */
             0,0,0,0,0,
             " ",        /* Sysname */
             " ",        /* Dummy */
             BAS_MOD,    /* opmode */
             0,          /* Module protection code */
             " ",        /* Release */
             " ",        /* Version */
             0      } ;  /* Encrypted serial number */

/*
***Current system sizes is initialized during startup.
*/
V3MSIZ  sysize;

/*
***Some global data.
*/
char    pidnam[JNLGTH+1] = "";         /* Current project, "" = No project */
char    jobnam[JNLGTH+1] = "";         /* Current job */
bool    jnflag = FALSE;                /* Jobname defined by user */
char    jobdir[V3PTHLEN+1] = "";       /* Current user job directory */
char    amodir[10*V3PTHLEN+10] = "";   /* Alternate user module libs */
char    asydir[V3PTHLEN+1] = "";       /* Current user symbol directory */
char    hlpdir[V3PTHLEN+1] = "";       /* Current user doc-directory */
char    mdffil[V3PTHLEN+1] = "";       /* Current menufile */
char    mbsdir[V3PTHLEN+1] = "";       /* Current user MBS-directory */
char    mbodir[V3PTHLEN+1] = "";       /* Current user module library */
char    inifil_1[V3PTHLEN+1] = "";     /* 1:st inifile */
char    inifil_2[V3PTHLEN+1] = "";     /* 2:nd inifile */

/*
***Current module type and attribute.
***modtyp = 3 => GEOMETRY
***modtyp = 2 => DRAWING
***modatt = LOCAL, GLOBAL or BASIC
*/
short   modtyp,modatt;

/*
***Largest ID sequence number in current module.
*/
DBseqnum snrmax;

/*
***Current pointer to start of active module and top
***of PM stack.
*/
pm_ptr  actmod,pmstkp;

/*
***Flag for temporary references.
*/
bool    tmpref;

/*
***0 <= posmode <= 8 controls the current
***method to use for positional input.
*** 0 = Absolute, 1 = Relative,  2 = Cursor
*** 3 = MBS,      4 = End,       5 = On
*** 6 = Center,   7 = Intersect, 8 = Grid
***posmode defaults to 2 = Cursor and is managed
***through the buttons in the menu window.
*/
short posmode = 2;

/*
***Name of active coordinate system.
*/
char    actcnm[JNLGTH+1];

/*
***Number of active function, name of active part
***and flag for <CTRL>c interrupt from keyboard.
***actfun >  0 => Number of active function
***       = -1 => Menu active
***       = -2 => Part active
*/
short   actfun;
char    actpnm[JNLGTH+1];
bool    intrup = FALSE;

/*
***C:s iobuffer for stdout.
*/
char    sobuf[BUFSIZ];

/*
***Current system mode.
***TOP_MOD  = During startup
***RIT_MOD  = Non generic drafting mode
***BAS2_MOD = Generic DRAWING (2D) mode
***BAS3_MOD = Generic GEOMETR (3D) mode
*/
short   v3mode;

/*
***Some flags that can be set by command line options.
***igxflg  = -x parameter. TRUE => Execute if RES-file is missing.
***iggflg  = -g parameter. TRUE => Generate global refs.
***igbflg  = -b parameter. TRUE => Batch mode, implies -x.
***igmtyp, igmatt = -GEOMETRY/-DRAWING and -LOCAL/-GLOBAL/-BASIC.
***igmtyp = igmatt = 0 => not defined => menu select in iginmo()
*/
bool    igxflg,iggflg,igbflg;
short   igmtyp,igmatt;

/*
***Prototypes for internal routines.
*/
static short igppar(int argc, char *argv[]);

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

/*!******************************************************/

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
 *      calls IGgene().
 *
 *      (C)microform ab 31/10/95 J. Kjellander
 *
 *      2007-01-05 Major rewrite, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;

#ifdef WIN32
    int    ercode;
    char   errbuf[V3PTHLEN+80];
    int    i;
    int    argc=25;
    char  *argv[25];
    char  *argbuf;
#endif

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
/*
***Set up the size of the outputbuffer to stdout.
*/
#ifdef UNIX
    setbuf(stdout,sobuf);
#endif

#ifdef WIN32
    setvbuf(stdout,sobuf,_IOFBF,BUFSIZ);
#endif
/*
***Init system data.
*/
    IGckhw();
/*
***Init debug before processing command line parameters.
*/
    dbgini();
/*
***Process command line parameters.
*/
#ifdef UNIX
    igppar(ac,av);
#endif

#ifdef WIN32
    argbuf = v3mall(argc*V3STRLEN,"WinMain");
    for ( i=0; i<argc; ++i ) argv[i] = argbuf + i*V3STRLEN;
    msargv(args,&argc,argv);
    igppar(argc,argv);
    v3free(argbuf,"WinMain");
#endif
/*
***Init signals and error handling.
*/
    IGsini();
    erinit();
/*
***Init level names.
*/
    EXinit_levels();
/*
***Init WP.
*/
#ifdef UNIX
   if ( WPinit(inifil_1,inifil_2) < 0 )
     {
     status = EREXIT;
     goto end;
     }
#endif
/*
***On the WIN32 platform init ms and check that environment
***variables are registred.
*/
#ifdef WIN32
   if ( (ercode=msinit(inifil_1)) < 0 )
     {
     sprintf(errbuf,"Can't init WINDOWS, error code = %d",ercode);
     msmbox("VARKON Startup error",errbuf,0);
     exit(V3EXOK);
     }
   igcenv();
#endif
/*
***Load default menufile.
*/
   strcpy(mdffil,IGgenv(VARKON_MDF));

#ifdef UNIX
   strcat(mdffil,"x11.MDF");
#endif

#ifdef WIN32
   strcat(mdffil,"mswin.MDF");
#endif

   if ( IGinit(mdffil) < 0 )
     {
#ifdef UNIX
     status = EREXIT;
     goto end;
#endif

#ifdef WIN32
     sprintf(errbuf,"Can't load menufile : %s",mdffil);
     msmbox("Varkon Startup error",errbuf,0);
     IGexit();
#endif
     }
/*
***Init surpac.
*/
   if ( suinit() < 0 )
     {
     status = EREXIT;
     goto end;
     }
/*
***If no project name is supplied on the command line
***ask for a project.
*/
pid:
   if ( pidnam[0] == '\0' )
     {
     if ( IGselp(pidnam) < 0 )
       {
       status = 0;
       goto end;
       }
     else goto pid;
     }
/*
***If '.' is given as project name, create a temporary project.
*/
    else if ( strcmp(pidnam,".") == 0 )
      {
#ifdef UNIX
      strcpy(pidnam,".");
      strcpy(jobdir,"./");
      strcpy(amodir,".");
      strcpy(asydir,"./");
      strcpy(hlpdir,"./");
#endif

#ifdef WIN32
      strcpy(pidnam,".");
      strcpy(jobdir,".\\");
      strcpy(amodir,".");
      strcpy(asydir,".\\");
      strcpy(hlpdir,".\\");
#endif
      }
/*
***If a project name is supplied, load the PID-file. If this
***doesn't work, we assume that the PID-file does not exist
***so we create one and then load it.
*/
    else
      {
      if ( IGldpf(pidnam) < 0 )
        {
        if ( IGialt(195,67,68,TRUE) )
          {
          status = IGcnpr(pidnam);
          if ( status == REJECT )
            {
            pidnam[0] = '\0';
            goto pid;
            }
          else if ( status < 0 )
            {
            status = EREXIT;
            goto end;
            }
          }
        else
          {
          pidnam[0] = '\0';
          goto pid;
          }
        }
/*
***PID file is loaded. Check that it seems OK and load
***the MDF-file it points to.
*/
      else if ( IGckpr() < 0  ||  IGinit(mdffil) < 0 )
        {
        status = EREXIT;
        goto end;
        }
      }
/*
***If no jobname is supplied on the command line, ask for one.
*/
   if ( jobnam[0] == '\0' )
     {
     status = IGselj(jobnam);

     if ( status < 0 )
       {
       pidnam[0] = '\0';
       goto pid;
       }
     else jnflag = TRUE;
     }
   else
     {
     jnflag = TRUE;
     }
/*
***Start Varkon in right mode.
*/
   if ( sydata.opmode == BAS_MOD ) status = IGgene();
   else                            status = IGexpl();
/*
***Varkon is now running. When we reach this point
***we are about to make a normal system close down
***or a close down because of some problem (EREXIT)
***that we must report to the user before ending.
*/
end:

   if ( status == EREXIT )
     {
     errmes();
     IGials("Varkon will now terminate !","Ok","Ok",TRUE);
     }
/*
***Now we can exit.
*/
   IGexit();
/*
***The following line is there to avoid compiler warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short igppar(
        int   argc,
        char *argv[])

/*      Processar kommandorads-parametrar.
 *
 *      In: argc => Antal parametrar
 *          argv => Array med pekare till parametrar
 *
 *      Ut: *pidnam
 *          *jobnam
 *
 *      FV: 0 om OK, annars görs exit med felmeddelande.
 *
 *     (C)microform ab Johan Kjellander  8/5/89
 *
 *     17/6/92  Debug, J. Kjellander
 *     14/11/94 Parametern -global_ref, J. Kjellander
 *     1998-03-12 2 inifiler, J.Kjellander
 *      
 ******************************************************!*/

{
   int i,ival;

/*
***Förställda värden.
*/
   pidnam[0]      = jobnam[0] = '\0';
   sysize.pm      = PMSIZE;
   sysize.gm      = 0;
   sysize.rts     = RTSSIZE;
   igxflg         = FALSE;
   iggflg         = FALSE;
   igmtyp         = IGUNDEF;
   igmatt         = IGUNDEF;
   v3mode         = sydata.opmode;
   inifil_1[0]    = '\0';
   inifil_2[0]    = '\0';
/*
***Parsa parameterlistan. X-versionen av V3 använder då
***först en särskild rutin för att plocka bort X-relaterade
***parametrar och bygga sin resursdatabas. Övriga parametrar
***tas om hand här på vanligt sätt.
*/
#ifdef UNIX
   WPmrdb(&argc,argv);
#endif

/*
***Loopa igenom alla parametrarna.
*/
   for ( i=1; i<argc; ++i)
     {
/*
***Om parametern inte föregås av ett minustecken är
***det fråga om pid- eller jobnamn.
*/
     if ( argv[i][0] != '-')
       {
       if ( pidnam[0] == '\0' )
         {
         strncpy(pidnam,argv[i],JNLGTH);
         pidnam[JNLGTH] = '\0';
         }
/*
***Jobnamn kollas särskilt noga med avseende på antal tecken,
***å,ä,ö mm. mm.
*/
       else if ( jobnam[0] == '\0' )
         {
         if ( igckjn(argv[i]) < 0 )
           {
           printf("%s: Invalid jobname\n",argv[0]);
           exit(V3EXOK);
           }
         else strcpy(jobnam,argv[i]);
         }
       else goto usage;
       }
/*
***Parametrar som föregås av ett minustecken.
*/
     else
       {
/*
***Modultyp och attribut.
*/
       if      ( sticmp(&argv[i][1],"DRAWING")  == 0 ) igmtyp = _2D;
       else if ( sticmp(&argv[i][1],"GEOMETRY") == 0 ) igmtyp = _3D;
       else if ( sticmp(&argv[i][1],"LOCAL")    == 0 ) igmatt = LOCAL;
       else if ( sticmp(&argv[i][1],"GLOBAL")   == 0 ) igmatt = GLOBAL;
       else if ( sticmp(&argv[i][1],"BASIC")    == 0 ) igmatt = BASIC;
       else
         {
         switch (argv[i][1])
           {
/*
***PM:s storlek.
*/
           case 'p':
#ifdef V3_DEMO
           goto usage;
#else
           if ( argv[i][2] == 'm'  &&
                sscanf(&argv[i][3],"%d",&ival) == 1  && ival > 0 )
             {
             sysize.pm = ival;
             break;
             }
           else goto usage;
#endif
/*
***Ritmodulen eller RTS:s storlek.
*/
           case 'r':
           if ( argv[i][2] == '\0' )
             {
             v3mode = sydata.opmode = RIT_MOD;
             break;
             }
           else if ( argv[i][2] == 't'  &&  argv[i][3] == 's'  &&
                sscanf(&argv[i][4],"%d",&ival) == 1  && ival > 0 )
             {
#ifdef V3_DEMO
             goto usage;
#else
             sysize.rts = ival;
#endif
             break;
             }
           else goto usage;
/*
***GM:s storlek.
*/
           case 'g':
           if ( argv[i][2] == 'm'  &&
                sscanf(&argv[i][3],"%d",&ival) == 1  && ival > 0 )
             {
             sysize.gm = ival;
             break;
             }
/*
***Parametern för global_ref vid interaktivt part-anrop.
*/
           else if ( sticmp(&argv[i][1],"GLOBAL_REF") == 0 )
             {
             iggflg = TRUE;
             break;
             }
           else goto usage;
/*
***Skriv ut version och serienummer.
*/
           case 'v':
           case 'V':
           if ( argv[i][2] == '\0' )
             {
             if ( sydata.opmode == RIT_MOD ) printf("VARKON-3D/R");
             else                            printf("VARKON-3D/B");
             printf(" %d.%d%c Serienummer : %d\n",sydata.vernr,sydata.revnr,
                                                  sydata.level,sydata.sernr);
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
           case 'd':
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
***Slut.
*/
     return(0);

usage:
     printf("usage: %s (pid) (job) (-pm) (-gm) (-r) (-i) (-x) (-v) (-D)\n",
            argv[0]);
     printf("          (-DRAWING) (-GEOMETRY) (-LOCAL) (-GLOBAL) (-BASIC)\n");
     exit(V3EXOK);
}

/********************************************************/
/*!******************************************************/

        short igckjn(char jobnam[])

/*      Kollar att ett jobnamn inte är för långt eller
 *      innehåller otillåtna tecken.
 *
 *      In:  jobnam = Pekare till NULL-terminerad sträng.
 *
 *      FV:  0 om OK. Annars -1.
 *
 *     (C)microform ab Johan Kjellander  8/5/89
 *      
 ******************************************************!*/

{
   int j,jnl;

/*
***Kolla antal tecken.
*/
    if ( (jnl=strlen(jobnam)) > JNLGTH ) return(-1);
/*
***Kolla otillåtna tecken.
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
***Slut.
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
