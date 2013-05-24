/*!******************************************************************/
/*  File: ig29.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  v2mome();     Move bytes in memory                              */
/*  v3fopr();     Open file with path =  path;path...               */
/*  v3trfp();     Process $environment in path                      */
/*  igckhw();     Check HW                                          */
/*  igckdl();     Check date                                        */
/*  v3dksn();     Dekrypt serial number                             */
/* *v3genv();     VARKON specific env-paths                         */
/* *gtenv3();     C's getenv() for Varkon                           */
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
#include <time.h>
#include <string.h>
#include <fcntl.h>

#ifdef UNIX
#include "/usr/include/sys/utsname.h"
#endif

#ifdef WIN32
#include <io.h>
#endif

/*
***envtabÄÅ är en tabell med VARKON:s standard-
***environment parametrar. Antalet parametrar i
***envtab = #define-konstanten VARKON_SND+1.
*/
static char *envtab[] = { "VARKON_ERM",
                          "VARKON_DOC",
                          "VARKON_PID",
                          "VARKON_MDF",
                          "VARKON_LIB",
                          "VARKON_TMP",
                          "VARKON_FNT",
                          "VARKON_ICO",
                          "VARKON_PLT",
                          "VARKON_PRD",
                          "VARKON_TOL",
                          "VARKON_INI",
                          "VARKON_SND"};

extern V3MDAT sydata;

/*!******************************************************/

        short v2mome(
        char *frompk,
        char *topk,
        int   size)

/*      Flyttar data i primärminne.
 *
 *      In: frompk => Pekare till ställe där data hämtas.
 *          topk   => Pekare till ställe där data lagras.
 *          size   => Antal char som flyttas.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/5/85 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef UNIX
     memcpy(topk,frompk,size);
#endif

#ifdef WIN32
     memcpy(topk,frompk,size);
#endif

#ifdef VMS
     register int i;
     register char *pf,*pt;

     pf = frompk;
     pt = topk;

     for ( i=0; i<size; ++i,++pt,++pf ) *pt = *pf;
#endif

     return(0);
  }

/********************************************************/
/*!******************************************************/

        int v3fopr(
        char *path,
        char *fil,
        char *ext)

/*      Öppnar en fil för läsning. Path kan ges på formen
 *      path1;path2 osv. i max 10 nivåer om max V3PTHLEN
 *      tecken.
 *
 *      In: path => Pekare till vägbeskrivning.
 *          fil  => Pekare till filnamn.
 *          ext  => Pekare till extension.
 *
 *      Ut: Inget.
 *
 *      FV: Filedescriptor eller < 0 om öppning misslyckats.
 *
 *      (C)microform ab 28/3/89 J. Kjellander
 *
 *      5/5/92   Plustecken på VAX/VMS, J. Kjellander
 *      6/11/95  WIN32, J. Kjellander
 *      19/1/96  v3trfp(), J. Kjellander
 *      1997-01-08 :->;, J.Kjellander
 *
 ******************************************************!*/

  {
    char  *p1,*p2;
    char   fnam[V3PTHLEN+1];
    char   buf[10*V3PTHLEN+10];
    int    fd;
 
/*
***Lite initiering.
*/ 
    strcpy(buf,path);
    p1 = p2 = buf;
/*
***Sök upp 'Ö0' eller semikolon i vägbeskrivningen.
***Gamla PID-filer kan innehålla : i UNIX.
*/
loop:
#ifdef UNIX
    if ( *p2 == ';'  ||  *p2 == ':' )
#endif
#ifdef WIN32
    if ( *p2 == ';' )
#endif
#ifdef VMS
    if ( *p2 == '+' )
#endif
      {
      *p2 = '\0';
      if ( *p1 != '\0' )
        {
        v3trfp(p1,fnam);
#ifdef UNIX
        strcat(fnam,"/"); 
#endif
#ifdef WIN32
        strcat(fnam,"\\"); 
#endif
        strcat(fnam,fil); strcat(fnam,ext);
        }
      else
        {
        strcpy(fnam,fil);
        strcat(fnam,ext);
        }
#ifdef WIN32
      if ( (fd=open(fnam,O_BINARY | O_RDONLY)) < 0 ) 
#else
      if ( (fd=open(fnam,0)) < 0 ) 
#endif
        {
        ++p2;
        p1 = p2;
        goto loop;
        }
      else return(fd);
      }
/*
***Nu har vi kommit till sista alternativet.
*/
    else if ( *p2 == '\0' )
      {
      if ( *p1 != '\0' )
        {
        v3trfp(p1,fnam);
#ifdef UNIX
        strcat(fnam,"/"); 
#endif
#ifdef WIN32
        strcat(fnam,"\\"); 
#endif
        strcat(fnam,fil);
        strcat(fnam,ext);
        }
      else
        {
        strcpy(fnam,fil);
        strcat(fnam,ext);
        }
#ifdef WIN32
      return(open(fnam,O_BINARY | O_RDONLY));
#else
      return(open(fnam,0));
#endif
      }
/*
***Nästa tecken.
*/
    else
      {
      ++p2;
      goto loop;
      }
  }

/********************************************************/
/*!******************************************************/

        void v3trfp(
        char *path1,
        char *path2)

/*      Translate file path. Översätter ev. $env i början
 *      på en path till dess värde.
 *
 *      In:  path1  => Pekare till ej översatt path.
 *
 *      Ut: *path2  => Översatt path.
 *
 *      (C)microform ab 23/3/95 J. Kjellander
 *
 *       2004-07-30 Slash/Backslash conversion, J.Kjellander
 *
 ******************************************************!*/

  {
   int   i,ntkn;
   char  envpar[V3STRLEN+1];


/*
***Orimliga indata tex. strängar med mindre än 2 tecken
***bryr vi oss inte om.
*/
   ntkn = strlen(path1);

   if ( ntkn < 2 )
     {
     strcpy(path2,path1);
     return;
     }
/*
***Turn slashes/backslashes.
*/
#ifdef UNIX
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '\\' ) *(path1+i) = '/';
#endif
#ifdef WIN32
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '/' ) *(path1+i) = '\\';
#endif
/*
***Om pathen börjar med dollar plockar vi ut environment-
***parametern.
*/
   if ( *path1 == '$' )
     {
#ifdef UNIX
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '/' ) break;
#endif
#ifdef WIN32
     for ( i=0; i<ntkn; ++i ) if ( *(path1+i) == '\\' ) break;
#endif
     strncpy(envpar,path1+1,i-1);
     envpar[i-1] = '\0';
/*
***Sen provar vi att översätta. Om parametern inte finns
***returnerar vi path1 som utdata. Annars den översatta pathen.
*/
     if ( gtenv3(envpar) == NULL )
       {
       strcpy(path2,path1);
       }
     else
       {
       strcpy(path2,gtenv3(envpar));
       strcat(path2,(path1+i));
       }
     }
   else strcpy(path2,path1);
/*
***Slut.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        short igckhw()

/*      Initierar sydata och kollar att rätt hårdvara
 *      används. Krypterar serienumret.
 *
 *      (C)microform ab 3/3/88 J. Kjellander
 *
 *      1996-01-25 Tagit bort sysserial, J. Kjellander
 *
 ******************************************************!*/

  {
   unsigned long sernum;

/*
***Initiera sydata.
*/

#ifdef UNIX
/*
***SCO/UNIX har plats för max 9 tkn (8+Ö0) i följande
***tre medlemmar av strukturen utsname. Hur det är med
***DNIX, IRIX, AIX osv. är inte känt men Solaris har plats för
***257 !!! V3:s sydata har i vilket fall bara plats för 9.
***För att vara på säkra sidan måste vi klippa efter 8 tecken.
*/
    struct utsname name;      /* UNIX-Struktur för system-data */

    uname(&name);
    strncpy(sydata.sysname,name.sysname,8);
    sydata.sysname[8] = '\0';
    strncpy(sydata.release,name.release,8);
    sydata.release[8] = '\0';
    strncpy(sydata.version,name.version,8);
    sydata.version[8] = '\0';
#endif
/*
***Microsoft Windows 32.
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
***VAX/VMS.
*/
#ifdef VMS
    strcpy(sydata.sysname,"VAX/VMS");
    strcpy(sydata.release,"-");
    strcpy(sydata.version,"-");
#endif

/*
***Kryptera serienummer och lagra.
*/
   sernum = (unsigned long)sydata.sernr;

   sernum = sernum - 3;
   sernum = sernum<<4;
   sernum = sernum + 52101207;
   sydata.ser_crypt = sernum;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igckdl(
        int yl,
        int ml,
        int dl)

/*      Kolla att datum inte gått ut.
 *
 *      (C)microform ab 1996-04-16 J. Kjellander
 *
 *      1998-03-12 1998, J.Kjellander
 *      1998-10-20 Omgjord, J.Kjellander
 *
 ******************************************************!*/

  {
   long       reltim;
   struct tm *timpek;

   if ( yl > 0 )
     {
     reltim = time((long *)0);
     timpek = localtime(&reltim);
     if ( timpek->tm_year  > yl ) return(-1);
     if ( timpek->tm_year == yl )
       {
       if ( timpek->tm_mon >  ml-1 ) return(-1);
       if ( timpek->tm_mon == ml-1 )
         {
         if ( timpek->tm_mday > dl ) return(-1);
         }
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short v3dksn(unsigned long crypt)

/*      Dekrypterar serienummer.
 *
 *      In: crypt = 32-bitars krypterat tal.
 *
 *      FV: 16-bitars signed tal.
 *
 *      (C)microform ab 1996-01-25 J. Kjellander
 *
 ******************************************************!*/

  {
   unsigned long sernum;
/*
***Dekryptera.
*/
   sernum = crypt - 52101207;
   sernum = sernum>>4;
   sernum = sernum +3;

   return((short)sernum);
  }

/********************************************************/
/*!******************************************************/

        char *v3genv(int envkod)

/*      Mappar environmentparameter till klartext.
 *
 *      In:
 *          envkod = Kod för VARKON_DOC,V3$DOC etc.
 *                   Se "env.h" på include.
 *
 *      Ut: Inget.
 *
 *      FV: NULL = Hittar ingen översättning.
 *          ptr  = Pekare till översättning eller
 *                 hårdkodat default.
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      8/12/95    VARKON_TOL, J. Kjellander
 *      20/1/96    VARKON_SND, J. Kjellander
 *      1997-09-30 Ny defaulthantering, J.Kjellander
 *
 ******************************************************!*/

  {
          char *envptr;
          int   ntkn;
   static char  envbuf[V3PTHLEN+1];

/*
***deftabÄÅ är en tabell med defaultvärden som används
***om vi kör VAX/VMS.
*/

#ifdef VMS
    static char *deftab[] = { "V3$ERM:",
                              "V3$DOC:",
                              "V3$PID:",
                              "V3$MDF:",
                              "V3$LIB:",
                              "V3$TMP:",
                              "V3$FNT:",
                              "V3$ICO:",
                              "V3$PLT:",
                              "V3$PRD:",
                              "V3$TOL:",
                              "V3$INI:",
                              "V3$SND:" };
#endif

/*
***För säkerhets skull kollar vi att envkod har ett
***rimligt värde.
*/
   if ( envkod < 0  || envkod > VARKON_SND ) return(NULL);
/*
***Om vi kör VMS är det bara att returnera defaultvärdet
***dvs. det mot envkod svarande logiska namnet.
*/
#ifdef VMS
    return(deftab[envkod]);
#else
/*
***Kör vi UNIX eller WIN32 provar vi först med gtenv3() och
***om vi då inte får träff returnerar vi NULL.
*/
   if ( (envptr=gtenv3(envtab[envkod])) == NULL ) return(NULL);
/*
***Kommer vi hit har gtenv3() fått träff.
***Om en environmentparameter inte
***har med den avslutande slashen lägger vi till en.
*/
   else
     {
     strcpy(envbuf,envptr);
     ntkn = strlen(envbuf);
#ifdef UNIX
     if ( envbuf[ntkn-1] != '/' ) strcat(envbuf,"/");
#endif
#ifdef WIN32
     if ( envbuf[ntkn-1] != '\\' ) strcat(envbuf,"\\");
#endif
     return(envbuf);
     }
#endif
  }

/********************************************************/
/*!******************************************************/

        char *gtenv3(char *envstr)

/*      Ersätter C's getenv(). 
 *
 *      In:
 *          Environmentparameter
 *
 *      Ut: Inget.
 *
 *      FV: NULL = Hittar ingen översättning.
 *          ptr  = Pekare till översättning.
 *
 *      (C)microform ab 1997-01-15 J. Kjellander
 *
 *      1997-09-30 Defaultparametrar, J.Kjellander
 *      2004-10-13 WIN32: Use getenv() instead of registry
 *                 Sören Larsson, Örebro University
 *
 ******************************************************!*/

  {
   int i;

   extern char *getenv();

/*
***I WIN32 kollar vi först registret.
*/
#ifdef WIN32
   /*
   long  status;
   HKEY  key;
   DWORD size;
   */
   static char *deftab[] = { "C:\\varkon\\erm\\",
                             "C:\\varkon\\man\\",
                             "C:\\varkon\\pid\\",
                             "C:\\varkon\\mdf\\english\\",
                             "C:\\varkon\\lib\\",
                             "C:\\varkon\\tmp\\",
                             "C:\\varkon\\cnf\\fnt\\",
                             "C:\\varkon\\cnf\\ico\\",
                             "C:\\varkon\\cnf\\plt\\",
                             "C:\\varkon\\app\\",
                             "C:\\varkon\\cnf\\tol\\",
                             "C:\\varkon\\cnf\\ini\\english\\",
                             "C:\\varkon\\cnf\\snd\\" };

   static char  envbuf[V3PTHLEN+1];


/*
***Öppna rätt avdelning i registret.
*/

/* removed by SL
   status = RegOpenKeyEx(HKEY_CURRENT_USER,"Environment",
                    (DWORD)0,KEY_QUERY_VALUE,&key);
   if ( status == ERROR_SUCCESS )
     {
     size= V3PTHLEN;
	 status = RegQueryValueEx(key,envstr,NULL,NULL,envbuf,&size);
     RegCloseKey(key);
     if ( status == ERROR_SUCCESS ) return(envbuf);
     }
*/
/*
***Om namnet inte finns i registret provar vi med C-
***bibliotekets getenv().
*/
/*   if ( status != ERROR_SUCCESS )
     {  */
     if ( getenv(envstr) != NULL ) return(getenv(envstr));
/*
***Om getenv() returnerar NULL kollar vi slutligen
***om det är någon av VARKON:s standard-parametrar.
*/
     else
       {
       for ( i=0; i<=VARKON_SND; ++i )
         {
         if ( strcmp(envstr,envtab[i]) == 0 ) return(deftab[i]);
         }
       return(NULL);
       }
	/* } */
#endif

/*
***I UNIX slipper vi strula med registry't.
*/
#ifdef UNIX
    static char *deftab[] = { "/usr/v3/erm/",
                              "/usr/v3/man/",
                              "/usr/v3/pid/",
                              "/usr/v3/mdf/",
                              "/usr/v3/lib/",
                              "/usr/v3/tmp/",
                              "/usr/v3/cnf/fnt/",
                              "/usr/v3/cnf/ico/",
                              "/usr/v3/cnf/plt/",
                              "/usr/v3/prd/",
                              "/usr/v3/cnf/tol/",
                              "/usr/v3/cnf/ini/",
                              "/usr/v3/cnf/snd/" };

   if ( getenv(envstr) != NULL ) return(getenv(envstr));
   else
     {
     for ( i=0; i<=VARKON_SND; ++i )
       {
       if ( strcmp(envstr,envtab[i]) == 0 ) return(deftab[i]);
       }
     return(NULL);
     }
#endif

   return(NULL);
  }

/********************************************************/
