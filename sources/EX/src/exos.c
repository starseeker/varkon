/*!*****************************************************
*
*    exos.c
*    ======
*
*    EXos();      Interface routine for OS
*
*    This file is part of the VARKON Execute Library.
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include <string.h>

#ifdef UNIX

#include "../../WP/include/WP.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

#include "../include/EX.h"

#ifdef UNIX
/*!******************************************************/

       int     EXos(
       char    oscmd[],
       DBshort mode)

/*      Execute a OS command.
 *
 *      In: oscmd => Command.
 *          mode  => 0 = Asynchronous with wait
 *                   1 = Interactive
 *                   2 = Asynchronous without wait
 *
 *      Return: Status from system()
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      21/10/86   wait(), J. Kjellander
 *      16/11/88   CGI, J. Kjellander
 *      3/10/91    mode, J. Kjellander
 *      15/4/92    Flyttat v3cmos() hit, J. Kjellander
 *      1996-02-21 WPwton(), J. Kjellander
 *
 ******************************************************!*/

  {
    int system_stat;

/*
***Batch-mode with or without wait..
*/
    if ( mode != 1 )
      {
/*
***Turn of interrupts during system call and
***return status.
*/
      if ( WPwton() ) WPlset(FALSE);

      system_stat = system(oscmd);

      if ( WPwton()  ) WPlset(TRUE);

      return(system_stat);
      }
/*
***Interactive mode.
*/
    else
      {
      IGcmos(oscmd);
      return(0);
      }
  }

/********************************************************/
#endif

#ifdef WIN32
/*!******************************************************/

       short EXos(
       char    oscmd[],
       DBshort mode)

/*      Proceduren OS i WIN32-version.
 *
 *      In: oscmd => Kommandosträng.
 *          mode  => 0 = Asynkront (Batch) med wait
 *                   1 = Interaktivt
 *                   2 = Asynkront utan wait
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX2092 = Fel från os, cmd=%s
 *
 *      (C)microform ab 1996-02-21 J. Kjellander
 *
 *      1997-02-03 _flushall(), J.Kjellander
 *      1997-05-20 COMMAND.COM bort,mode 1, J.Kjellander
 *      1998-03-27 WaitForSingleObject(), J.Kjellander
 *
 ******************************************************!*/

  {
   DWORD               create,errnum;
   char                errbuf[80];
   STARTUPINFO         si;
   PROCESS_INFORMATION pi;

/*
DWORD excode används ej fn.;
*/
/*
***Initiera startupinfo och processinfo.
*/
   memset(&si,0,sizeof(si));
   si.cb = sizeof(si);
/*
***Töm alla filbuffrar.
*/
   _flushall();
/*
***Om mode = 1 skall DOS-fönster skapas.
*/
   if ( mode == 1 ) create = CREATE_NEW_CONSOLE;
   else             create = DETACHED_PROCESS;
/*
***Starta processen.
*/
   if ( !CreateProcess(NULL,
                       oscmd,
                       NULL,
                       NULL,
                       FALSE,
                       create,
                       NULL,
                       NULL,
                      &si,
                      &pi) )
      {
      errnum = GetLastError();
      sprintf(errbuf,"%d%%%s",errnum,oscmd);
      return(erpush("EX2092",errbuf));
      }
/*
***Enligt dok. bör man stänga handtagen primära
***tråden så fort som möjligt om det inte behövs.
*/
    CloseHandle(pi.hThread);
/*
***Om mode = 0 eller 1 skall vi vänta tills subprocessen är
***klar. Detta verkar funka olika i NT och 95. C:s wait funkar
***inte ! Ej heller cwait(). Bytt till WaitForSingleObject()
***1998-03-27, JK.
*/
    if ( mode != 2 ) WaitForSingleObject(pi.hProcess,INFINITE);
/*
***Nu kan vi stänga även handtaget till själva processen.
*/
    CloseHandle(pi.hProcess);
/*
***Det ska gå att få reda på Exit-status men det test
***jag gjorde mot TakCAD funkade inte. VisualBasic inblandat !
*
loop:
    if ( GetExitCodeProcess(pi.hProcess,&excode) )
      {
      if ( excode == STILL_ACTIVE )
        {
        goto loop;
        }
      }
    else
      {
      errnum = GetLastError();
      sprintf(errbuf,"%d%%%s",errnum,oscmd);
      return(erpush("EX2092",errbuf));
      }
*/

 
    return(0);
  }
  
/********************************************************/
#endif
