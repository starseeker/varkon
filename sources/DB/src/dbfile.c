/**********************************************************************
*
*    dbfile.c
*    ========
*
*    This file includes the following public functions:
*
*    DBinit();   Init DB and create new file
*    DBload();   Init DB and load existing file
*    DBexit();   Exit DB and save file
*
*    This file is part of the VARKON Database Library.
*    URL:  http://varkon.sourceforge.net
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

#include "../include/DB.h"
#include "../include/DBintern.h"
#include <time.h>
#include <string.h>

#ifdef UNIX
#include <sys/utsname.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

static int init_gmsdat(),update_gmsdat();

/*!******************************************************/

        DBstatus DBinit(
        char   *dbfile,
        DBint   cache_size,
        DBint   version,
        DBint   revision,
        char    level)

/*      Inits the DB and creates a new DB file. If a file
 *      with the same name already exists it is overwritten.
 *
 *      In: dbfile     = Path/name of new file
 *          cache_size = Size of DB RAM cache in pages
 *          version    = Callers DB_LIBVERSION
 *          revision   = Callers DB_LIBREVISION
 *          level      = Callers DB_LIBLEVEL
 *
 *
 *      FV:      0 => Ok.
 *             < 0 => Error from called routines.
 *
 *      (C)microform ab 30/10/85 J. Kjellander
 *
 *      1999-02-09 libversion, J.Kjellander
 *
 ******************************************************!*/

  {
   DBstatus status;

/*
***First of all check that the libversion supplied is
***compatible with the current DB library.
*/
   if ( version  != DB_LIBVERSION  ||
        revision != DB_LIBREVISION ||
        level    != DB_LIBLEVEL ) return(erpush("DB0213",""));
/*
***Set creation date etc. in gmsdat_db.
*/
   init_gmsdat();
/*
***Init gmssiz. DB only cares about gmssiz.gm
*/
   if ( cache_size > 0 ) gmssiz.gm = cache_size;
   else                  gmssiz.gm = 0;
   gmssiz.pm  = 0;
   gmssiz.df  = 0;
   gmssiz.rts = 0;
/*
***Create new file.
*/
   if ( (status=gmcrpf(dbfile)) < 0 ) return(status);
/*
***Reset DB to empty status.
*/
   if ( (status=DBreset()) < 0 ) return(status);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBload(
        char    *dbfile,
        DBint    cache_size,
        DBint    version,
        DBint    revision,
        char     level)

/*      Inits DB and loads existing pagefile from disc.
 *
 *      In:
 *          dbfile     = Path and name of DB file
 *          cache_size = Size of DB RAM cache in pages
 *          version    = Callers DB_LIBVERSION
 *          revision   = Callers DB_LIBREVISION
 *          level      = Callers DB_LIBLEVEL
 *
 *      Return:
 *               0 => Ok.
 *              -1 => File does not exist
 *          GM0052 => DB file not compatible with DBlib
 *          GM0033 => System read error
 *              -3 => Corrupt data in DB file
 *
 *      (C)microform ab 8/12/84 J. Kjellander
 *
 *      17/10/85 Sydata, J. Kjellander
 *      21/10/85 Errorcodes, J. Kjellander
 *      17/2/86  gmsdat, J. Kjellander
 *      5/4/86   New error handling, J. Kjellander
 *      9/10/86  IBMATX, J. Kjellander
 *      1999-02-09 libversion, J.Kjellander
 *
 ******************************************************!*/

 {
   DBstatus status;
   char  errbuf[V3STRLEN];

/*
***First of all check that the libversion supplied is
***compatible with the current DB library.
*/
   if ( version  != DB_LIBVERSION  ||
        revision != DB_LIBREVISION ||
        level    != DB_LIBLEVEL ) return(erpush("DB0213",""));
/*
***Init gmssiz. DB only cares about gmssiz.gm
*/
   if ( cache_size > 0 ) gmssiz.gm = cache_size;
   else                  gmssiz.gm = 0;
   gmssiz.pm  = 0;
   gmssiz.df  = 0;
   gmssiz.rts = 0;
/*
**Check that the file may be opened.
*/
#ifdef WIN32
   if ( (gmpfpk=fopen(dbfile,"r+b") ) == NULL ) return(-1);
#else
   if ( (gmpfpk=fopen(dbfile,"r+") ) == NULL ) return(-1);
#endif
/*
***Read original gmsdat and check for compatibility.
*/
   if ( fseek( gmpfpk, (long)0, 0) != 0 ) goto readerr;
   if ( fread((char *)&pfsiz,sizeof(pfsiz),1,gmpfpk) == 0 ) goto readerr;
   if ( fseek( gmpfpk,(long)pfsiz,0) != 0 ) goto readerr;
   if ( fread((char *)&gmsdat_org,sizeof(V3MDAT),1,gmpfpk) == 0 ) goto readerr;
/*
***Varkon DB files earlier than 1.0A (1985) are not supported.
*/
   if ( gmsdat_org.revnr < 1 )
     {
     gmclpf();
     return(erpush("DB0052",dbfile));
     }
/*
***All other versions are backward compatible but forward compatibility
***is not supported.
*/
   if ( gmsdat_org.vernr > gmsdat_db.vernr  ||
        (gmsdat_org.vernr == gmsdat_db.vernr  &&
         (gmsdat_org.revnr > gmsdat_db.revnr ||
           (gmsdat_org.revnr == gmsdat_db.revnr &&
            gmsdat_org.level > gmsdat_db.level))) )
     {
     sprintf(errbuf,"%d%d.%c",(int)gmsdat_db.vernr,
                              (int)gmsdat_db.revnr,
                                   gmsdat_db.level);
     erpush("DB0171",errbuf);
     sprintf(errbuf,"%s%%%d%d.%c",dbfile,
                              (int)gmsdat_org.vernr,
                              (int)gmsdat_org.revnr,
                                   gmsdat_org.level);
     erpush("DB0181",errbuf);
     gmclpf();
     return(erpush("DB0052",dbfile));
     }
/*
***Read metdat and do gmclear with right logmax.
*/
   if ( fread((char *)&metdat,sizeof(GMMDAT),1,gmpfpk) == 0 ) goto readerr;

   if ( (status=gmclr((DBint)metdat.logmax)) < 0 ) 
     {
     gmclpf();
     return(status);
     }
/*
***Load the file.
*/
   if ( gmldpf() < 0 ) return(-3);
/*
***End.
*/
   return(0);
/*
***Error exit.
*/
readerr:
   gmclpf();
   return(erpush("DB0033",""));
 }

/********************************************************/
/*!******************************************************/

        DBstatus DBexit()

/*      Saves DB on file.
 *
 *      FV:   0  => Ok.
 *          < 0  => Error from "gmclpf".
 *
 *      (C)microform ab 5/12/84 J. Kjellander
 *
 ******************************************************!*/

  {
   DBstatus status;

/*
***Update gmsdat_db with curren time.
*/
   update_gmsdat();
/*
***Write everything to pagefile.
*/
   if ( (status=gmsvpf()) < 0 )
     {
     gmclpf();
     return(status);
     }
/*
***Close pagefile.
*/
   return(gmclpf());
  }

/********************************************************/
/*!******************************************************/

        int init_gmsdat()

/*      Updates gmsdat_db with current date and OS.
 *
 *      (C)microform ab 1999-02-09 J. Kjellander
 *
 ******************************************************!*/

  {
   long       reltim;
   struct tm *timpek;

/*
***Current time.
*/
   reltim = time((long *)0);
   timpek = localtime(&reltim);

   gmsdat_db.year_c = (short)timpek->tm_year;
   gmsdat_db.mon_c  = (short)(timpek->tm_mon+1);
   gmsdat_db.day_c  = (short)timpek->tm_mday;
   gmsdat_db.hour_c = (short)timpek->tm_hour;
   gmsdat_db.min_c  = (short)timpek->tm_min;

   gmsdat_db.year_u = gmsdat_db.year_c;
   gmsdat_db.mon_u  = gmsdat_db.mon_c;
   gmsdat_db.day_u  = gmsdat_db.day_c;
   gmsdat_db.hour_u = gmsdat_db.hour_c;
   gmsdat_db.min_u  = gmsdat_db.min_c;
/*
***OS data. gmsdat_db can only hold 8+1 chars.
*/
#ifdef UNIX
    {
    struct utsname name;

    uname(&name);
    strncpy(gmsdat_db.sysname,name.sysname,8);
    gmsdat_db.sysname[8] = '\0';
    strncpy(gmsdat_db.release,name.release,8);
    gmsdat_db.release[8] = '\0';
    strncpy(gmsdat_db.version,name.version,8);
    gmsdat_db.version[8] = '\0';
    }
#endif

#ifdef WIN32
    {
    char  buf[9];
    int   major,minor;
    DWORD version;

    strcpy(gmsdat_db.sysname,"Win32");
    version = GetVersion();
    major = (int)(version&0x00FF);
    minor = (int)((version&0xFF00)>>8);
    sprintf(buf,"%d",major);
    buf[8] = '\0';
    strcpy(gmsdat_db.release,buf);
    sprintf(buf,"%d",minor);
    buf[8] = '\0';
    strcpy(gmsdat_db.version,buf);
    }
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int update_gmsdat()

/*      Updates gmsdat_db with current date and time.
 *
 *      (C)microform ab 1999-02-09 J. Kjellander
 *
 ******************************************************!*/

  {
   long       reltim;
   struct tm *timpek;

/*
***Current time.
*/
   reltim = time((long *)0);
   timpek = localtime(&reltim);

   gmsdat_db.year_u = (short)timpek->tm_year;
   gmsdat_db.mon_u  = (short)(timpek->tm_mon+1);
   gmsdat_db.day_u  = (short)timpek->tm_mday;
   gmsdat_db.hour_u = (short)timpek->tm_hour;
   gmsdat_db.min_u  = (short)timpek->tm_min;

   return(0);
  }

/********************************************************/
