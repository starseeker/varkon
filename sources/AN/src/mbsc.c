/**********************************************************************
*
*    mbsc.c
*    ======
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    main function for the MBS analyser.
*
*    This file includes the following routines:
*
*    main()                Analyser main function
*    short erpush()        Diag - push error
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

#ifdef ANALYZER

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/AN.h"
#include <string.h>

#ifdef UNIX
#include "/usr/include/sys/utsname.h"
#endif


#define DEFLIN 70                  /* default number of lines/page */
#define DEFCOL 80                  /* default number of columns/line */
#define MAXSRC 800                 /* max number of source files */
#define MAXCHR 80                  /* max number of characters/file name */

/*
***External definitions
*/
struct ANSYREC sy;                 /* scanner access structure */
V3MSIZ sysize;                     /* Div. storlekar. */
V3MDAT sydata = {                  /* System constants */
                1000,              /* Serienummer      */
                1,                 /* Version          */
                19,                /* Revision         */
                'C',               /* "upplaga"        */
                0, 0, 0, 0, 0,     /* tider            */
                0, 0, 0, 0, 0,
                "",                /* Sysname          */
                "",                /* Not used         */
                GENERIC,           /* opmode           */
                0,                 /* Skyddskod MN860610 */
                "",                /* Release          */
                "",                /* Version          */
                0                  /* CPU-nummer       */
                };

char jobdir[] = {"./"};            /* output file directory */
char libdir[] = {"./"};            /* dummy, 4f */
static bool dbgflg = FALSE;        /* debug flag */
short modtyp;                      /* module type 2 = _2D, 3 = _3D */
short modatt;                      /* module attribute LOCAL, GLOBAL etc. */
bool  andbg();

static void init_sydata();

/*!******************************************************/

        int   main(
        int   argc,
        char *argv[])

/*      MBS compiler main entry point.
 *      analyses the MBS source file specified by 1:st argument.
 *
 *      *.MBS --> ANALYZER -- > *.MBO, object file (if no severe errors)
 *                         -- > errors/warnings on stdout (if any errors)
 *                         -- > *.LST, list file
 *
 *      In:   
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *      2007-12-23 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
   pm_ptr modptr;                  /* PM-pointer to PM-module */
   short i,j,namcnt;
   int   lcount = DEFLIN;          /* for arguments */
   int   ccount = DEFCOL;
   int   snrval;
   char  cntstr[10];
   char  source[MAXSRC][MAXCHR+1];
   int   ival;
   bool  colflg,linflg,lstflg,snrflg,pmsflg;
/*
***Börja det hela med en hårdvarucheck.
***Därmed fylls sydata i.
*/
   init_sydata();
/*
***Default storlek för PM.
*/
   sysize.pm = PMSIZE;
/*
***Argument från kommandoraden.
*/
   colflg = linflg = lstflg = snrflg = pmsflg = FALSE;
   i = namcnt = 0;

   while( ++i < (short)argc )
     {
     if ( argv[i][0] == '-' )
       {
       switch ( argv[i][1] )
         {
/*
***Debug.
*/
         case 'D':
         case 'd':
         if ( argv[i][2] != '\0' )
           {
           printf(
           "%s : unknown modifier : %s, use -l, -h<nr>, -w<nr>, -s<nr>\n",
           argv[0], argv[i]);
           exit(-1);
           }
         else if ( dbgflg == TRUE )
           {
           printf("%s : multiple use of modifier : -d\n",argv[0]);
            exit(-1);
           }
         else dbgflg = TRUE;
         break;
/*
***Rader per sida.
*/
         case 'H':
         case 'h':
         if ( argv[i][2] == '\0' )
           {
           ++i;
           strcpy(cntstr,argv[i]);
           }
         else strcpy(cntstr,&argv[i][2]);

         if ( linflg == TRUE )
           {
           printf("%s : multiple use of modifier : -h\n",argv[0]);
           exit(-1);
           }
         else
           {
           linflg = TRUE;
           if ( sscanf(cntstr,"%d",&lcount) != 1)
             {
             printf("%s : illegal -h value\n",argv[0]);
             exit(-1);
             }
           }
           break;
/*
***Kolumner per rad.
*/
         case 'W':
         case 'w':
         if ( argv[i][2] == '\0' )
           {
           ++i;
           strcpy(cntstr,argv[i]);
           }
         else strcpy(cntstr,&argv[i][2]);

         if ( colflg == TRUE )
           {
           printf("%s : multiple use of modifier : -w\n",argv[0]);
           exit(-1);
           }
         else
           {
           colflg = TRUE;
           if ( sscanf(cntstr,"%d",&ccount) != 1 )
             {
             printf("%s : illegal -w value\n",argv[0]);
             exit(-1);
             }
           }
           break;
/*
***Lista.
*/
         case 'L':
         case 'l':
         if ( argv[i][2] != '\0' )
           {
           printf(
           "%s : unknown modifier : %s, use -l, -h<nr>, -w<nr>, -s<nr>\n",
           argv[0], argv[i]);
           exit(-1);
           }
         else if ( lstflg == TRUE )
           {
           printf("%s : multiple use of modifier : -l\n",argv[0]);
           exit(-1);
           }
         else lstflg = TRUE;
         break;
/*
***Serienummer.
*/
         case 'S':
         case 's':
         if ( argv[i][2] == '\0' )
           {
           ++i;
           strcpy(cntstr,argv[i]);
           }
         else strcpy(cntstr,&argv[i][2]);

         if ( snrflg == TRUE )
           {
           printf("%s : multiple use of modifier : -s\n",argv[0]);
           exit(-1);
           }
         else
           {
           snrflg = TRUE;
           if ( sscanf(cntstr,"%d",&snrval) != 1 )
             {
             printf("%s : illegal -s value\n",argv[0]);
             exit(-1);
             }
           else sydata.mpcode = snrval;
           }
           break;
/*
***PM:s storlek.
*/
         case 'P':
         case 'p':
         if ( argv[i][2] != 'm'  &&  argv[i][2] != 'M' )
           {
           printf("%s : unknown modifier : %s\n",argv[0],argv[i]);
           exit(-1);
           }
         else if ( argv[i][3] =='\0' )
           {
           ++i;
           strcpy(cntstr,argv[i]);
           }
         else strcpy(cntstr,&argv[i][3]);

         if ( pmsflg == TRUE )
           {
           printf("%s : multiple use of modifier : -pm\n",argv[0]);
           exit(-1);
           }
         else
           {
           pmsflg = TRUE;
           if ( sscanf(cntstr,"%d",&ival) != 1  ||  ival < 0 )
             {
             printf("%s : illegal -pm value\n",argv[0]);
             exit(-1);
             }
           else sysize.pm = ival*1024;
           }
           break;
/*
***Versionsnummer.
*/
           case 'V':
           case 'v':
           printf("VARKON %d.%d%c\n",
             sydata.vernr,sydata.revnr,sydata.level);
           exit(V3EXOK);
           break;
/*
***Otillåten option.
*/
           default:
           printf("%s : unknown modifier : %s\n",argv[0],argv[i]);
           exit(-1);
           break;
           }
       }
     else
       {
       if ( namcnt >= MAXSRC )
         {
         printf("%s : too many source files : max %d allowed\n",
                  argv[0], MAXSRC);
         exit(-1);
         }
       else
         {
         if ( strlen(argv[i]) > MAXCHR )
           {
           printf("%s : too many characters in path : max %d allowed\n",
                  argv[0], MAXCHR);
           exit(-1);
           }
         strcpy(source[namcnt],argv[i]);
         namcnt++;
         }
       }
    }
/*
***Alla ev. optioner samt MBS-filnamn klara.
***Nu kan vi starta själva analysen.
*/
   printf("\n");
/*
***Initiera PM.
*/
   pminit();
/*
***Analysera filerna en i taget.
*/
   for ( j=0; j<namcnt; j++ )
     {
/*
***Lägg till .MBS om det saknas.
*/
     i = strlen(source[j]);
     if ( i < 5 ) strcat(source[j],".MBS");
     else if ( strcmp(source[j]+i-4,".MBS") != 0 ) strcat(source[j],".MBS");
/*
***Skriv ut filnamnet om det är fler än en fil.
*/
     if ( namcnt > 1 ) printf("%s :\n",source[j]);
/*
***Initiera scanner och felloggning.
*/
     if ( asinit(source[j],ANRDFIL) != 0 )
       {
       printf("%s : can't open\n",source[j]);
       continue;
       }
     anascan(&sy);

     anlogi(source[j], lcount, ccount, lstflg);
/*
***Pass 1, analysera modulen.
*/
     anpmod(&modptr);
/*
***Pass 2, kontroll av framåt-referenser.
*/
     ancseq();
/*
***Pass 3, kontroll av lablar.
*/
     anclab();
/*
***Avsluta scannern.
*/
     asexit();
/*
***Ev. debug-lista.
*/
     if ( andbg( ) ) pmprmo(modptr);
/*
***Skapa MBO-fil.
*/
    if ( !anyerr() )
      if (pmsave(modptr) != 0 )
        printf("%s : can't create .MBO file\n", argv[0]);
/*
****Stäng av felloggningen.
*/        
    anlogx();
/*
***Listfil och felmeddelanden.
*/
    printf("\n");
    anlist(source[j]);
    printf("\n");
/*
***Reset på PM och RTS och sedan nästa fil.
*/
    pmclea();
    }
/*
***Helt slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool andbg( )

/*      DEBUG MODE.
 *
 *      In:   
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(dbgflg);
  }

/********************************************************/
/*!******************************************************/

        short erpush(errstr,instr)
        char errstr[],instr[];

/*      ERPUSH.
 *
 *      In:   
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-17 Mats Nelson
 *
 *      1999-04-27 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( andbg() ) printf("erpush : %s %s\n", errstr, instr );
   return(-(errstr[5] - '0'));
  }

#endif

/*********************************************************************/
/*!******************************************************/

 static void init_sydata()

/*      Initierar sydata och kollar att rätt hårdvara
 *      används. Krypterar serienumret.
 *
 *      (C)microform ab 3/3/88 J. Kjellander
 *
 *      1996-01-25 Tagit bort sysserial, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Unix.
*/

#ifdef UNIX
    struct utsname name;

    uname(&name);
    strncpy(sydata.sysname,name.sysname,8);
    sydata.sysname[8] = '\0';
    strncpy(sydata.release,name.release,8);
    sydata.release[8] = '\0';
    strncpy(sydata.version,name.version,8);
    sydata.version[8] = '\0';
#endif

/*
***Microsoft Windows.
*/

#ifdef WIN32
    char  buf[9];
    int   major,minor;
    DWORD version;

    strcpy(sydata.sysname,"Win32");
    version = GetVersion();
    major = (int)(version&0x00FF);
    minor = (int)((version&0xFF00)>>8);
    sprintf(buf,"%d",major);
    buf[8] = '\0';
    strcpy(sydata.release,buf);
    sprintf(buf,"%d",minor);
    buf[8] = '\0';
    strcpy(sydata.version,buf);
#endif

/*
***Encrypted serial number not used.
*/
   sydata.ser_crypt = 0;

   return;
  }

/********************************************************/
