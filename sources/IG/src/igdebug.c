/*!******************************************************************/
/*  File: igdebug.c                                                 */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  dbgini();    Init debug                                         */
/*  dbgexi();    Exit debug                                         */
/*  dbgon();     Turn debug on                                      */
/*  dbglev();    Returns debug level                                */
/* *dbgfil();    Returns ptr to debugfile                           */
/*  v3time();    Handles time measuring                             */
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
#include "../include/debug.h"
#include <stdio.h>
#include <string.h>


#ifdef UNIX
#include <sys/times.h>
#include <unistd.h>
#endif

extern char jobdir[];

/* dbgtab is the debug module- and level table */

static struct
  {
  int level;
  FILE *fptr;
  } dbgtab[11];

/* timtab is the timer table */

#ifdef UNIX
static struct
  {
  int     count;
  clock_t ticks;
  clock_t total;
  } timtab[10];
#endif

/*!******************************************************/

        void dbgini()

/*      Initializes debug system and timers.
 *
 *      (C)microform ab 17/6/92 J. Kjellander
 *
 *      1998-04-27 Timers, J.Kjellander
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *
 ******************************************************!*/

 {
   int i;

/*
***Init debug.
*/
   dbgtab[IGEPAC].level = 0;
   dbgtab[GRAPAC].level = 0;
   dbgtab[GEOPAC].level = 0;
   dbgtab[GMPAC].level  = 0;
   dbgtab[EXEPAC].level = 0;
   dbgtab[ANAPAC].level = 0;
   dbgtab[PMPAC].level  = 0;
   dbgtab[SURPAC].level = 0;
   dbgtab[WINPAC].level = 0;
   dbgtab[MSPAC].level  = 0;
/*
***Init timers.
*/
   for ( i=0; i<10; ++i ) v3time(V3_TIMER_RESET,i,"");
 }

/*!******************************************************/
/*!******************************************************/

        void dbgexi()

/*      Closes all currently open debug files.
 *
 *      (C)microform ab 17/6/92 J. Kjellander
 *
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *
 ******************************************************!*/

 {
    if ( dbgtab[IGEPAC].level > 0 ) fclose(dbgtab[IGEPAC].fptr);
    if ( dbgtab[GRAPAC].level > 0 ) fclose(dbgtab[GRAPAC].fptr);
    if ( dbgtab[GEOPAC].level > 0 ) fclose(dbgtab[GEOPAC].fptr);
    if ( dbgtab[GMPAC].level  > 0 ) fclose(dbgtab[GMPAC].fptr);
    if ( dbgtab[EXEPAC].level > 0 ) fclose(dbgtab[EXEPAC].fptr);
    if ( dbgtab[ANAPAC].level > 0 ) fclose(dbgtab[ANAPAC].fptr);
    if ( dbgtab[PMPAC].level  > 0 ) fclose(dbgtab[PMPAC].fptr);
    if ( dbgtab[SURPAC].level > 0 ) fclose(dbgtab[SURPAC].fptr);
    if ( dbgtab[WINPAC].level > 0 ) fclose(dbgtab[WINPAC].fptr);
    if ( dbgtab[MSPAC].level  > 0 ) fclose(dbgtab[MSPAC].fptr);
 }

/*!******************************************************/
/*!******************************************************/

        int dbgon(char *str)

/*      Turns on debug for a source code module. Called
 *      by process_cmdline() at startup.
 *
 *      In: str = Command line option -DMn where
 *                M=Module and n=level. -D is stripped
 *                by iprocess_cmdline(). See debug.h
 *
 *      Return:  0 = Ok.
 *              <0 = Syntax error.
 *
 *      (C)microform ab 17/6/92 J. Kjellander
 *
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *
 ******************************************************!*/

 {
   int   pac,lev,len;
   char  fnam[80];

/*
***Which source code module ?
*/
   if ( strncmp(str,"IGEPAC",6) == 0  ||
        strncmp(str,"igepac",6) == 0 )
     { pac = IGEPAC; len = 6; }
   else if ( strncmp(str,"GRAPAC",6) == 0  ||
             strncmp(str,"grapac",6) == 0 )
     { pac = GRAPAC; len = 6; }
   else if ( strncmp(str,"GEOPAC",6) == 0  ||
             strncmp(str,"geopac",6) == 0 )
     { pac = GEOPAC; len = 6; }
   else if ( strncmp(str,"GMPAC",5) == 0  ||
             strncmp(str,"gmpac",5) == 0 )
     { pac = GMPAC; len = 5; }
   else if ( strncmp(str,"EXEPAC",6) == 0  ||
             strncmp(str,"exepac",6) == 0 )
     { pac = EXEPAC; len = 6; }
   else if ( strncmp(str,"ANAPAC",6) == 0  ||
             strncmp(str,"anapac",6) == 0 )
     { pac = ANAPAC; len = 6; }
   else if ( strncmp(str,"PMPAC",5) == 0  ||
             strncmp(str,"pmpac",5) == 0 )
     { pac = PMPAC; len = 5; }
   else if ( strncmp(str,"SURPAC",6) == 0  ||
             strncmp(str,"surpac",6) == 0 )
     { pac = SURPAC; len = 6; }
   else if ( strncmp(str,"WINPAC",6) == 0  ||
             strncmp(str,"winpac",6) == 0 )
     { pac = WINPAC; len = 6; }
   else if ( strncmp(str,"MSPAC",5) == 0  ||
             strncmp(str,"mspac",5) == 0 )
     { pac = MSPAC; len = 5; }
   else return(-1);
/*
***What debug level?
*/
   if ( sscanf(str+len,"%d",&lev) < 1 ) return(-1);
   dbgtab[pac].level = lev;
/*
***Open debug file.
*/
   strncpy(fnam,str,len); fnam[len] = '\0';
   strcat(fnam,".DBG");
   if ( (dbgtab[pac].fptr=fopen(fnam,"w+")) == NULL ) return(-1);
/*
***Write a header
*/
   fprintf(dbgtab[pac].fptr,"Debug-fil för VARKON-3D, nivå = %s\n",str);

   return(0);
 }

/*!******************************************************/
/*!******************************************************/

        int dbglev(int srcpac)

/*      Returns the current debug level for a specific
 *      soyrce code module.
 *
 *      In: srcpac = Source code module, ie. GEOPAC.
 *
 *      Return: Debug level.
 *
 *      (C)microform ab 17/6/92 J. Kjellander
 *
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *
 ******************************************************!*/

 {
    if ( srcpac < IGEPAC  ||  srcpac > MSPAC ) return(0);
    else return(dbgtab[srcpac].level);
 }

/*!******************************************************/
/*!******************************************************/

        FILE *dbgfil(int srcpac)

/*      Returns pointer (FILE *) to debug file open
 *      for write.
 *
 *      In: srcpac = Source code module, ie. GEOPAC
 *
 *      FV: File pointer.
 *
 *      (C)microform ab 17/6/92 J. Kjellander
 *
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *
 ******************************************************!*/

 {
    if ( srcpac < IGEPAC  ||  srcpac > MSPAC ) return(0);
    else return(dbgtab[srcpac].fptr);
 }

/*!******************************************************/
/*!******************************************************/

        void v3time(
        int   op,
        int   num,
        char *s)

/*      Handles time measurements.
 *
 *      In: op  = See debug.h
 *          num = Timer number, 0->9
 *          s   = Text description
 *
 *      (C)microform ab 1998-04-28 J. Kjellander
 *
 *      1998-05-25 #ifdef UNIX, J.Kjellander
 *      2006-12-05 Moved to file igdebug.c, J.Kjellander
 *      2007-01-15 Text, J.Kjellander
 *
 ******************************************************!*/

 {
#ifdef UNIX
   FILE   *f;
   struct  tms timbuf;
   clock_t ticks;
   double  secs;
   char    path[V3PTHLEN];

   if ( num < 0  ||  num > 9 ) return;

   switch (op)
     {
     case V3_TIMER_WRITE:
     strcpy(path,jobdir);
     strcat(path,"timelog.txt");
     f = fopen(path,"a");
     secs = (double)timtab[num].total/sysconf(_SC_CLK_TCK);
     fprintf(f,"Timer %d: %s = %g seconds. Active %d times.\n",
                                              num,s,secs,timtab[num].count);
     fclose(f);
     break;

     case V3_TIMER_ON:
   ++timtab[num].count;
     timtab[num].ticks = times(&timbuf);
     break;

     case V3_TIMER_OFF:
     ticks = times(&timbuf);
     timtab[num].total += ticks - timtab[num].ticks;
     break;

     case V3_TIMER_RESET:
     timtab[num].count = 0;
     timtab[num].ticks = 0;
     timtab[num].total = 0;
     break;
     }
#endif
   return;
 }

/*!******************************************************/
