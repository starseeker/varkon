/*!******************************************************************/
/*  File: ig27.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igintt();     Inits serial port                                 */
/*  igextt();     Resets serial port                                */
/*  igsini();     Inits signals                                     */
/*  sigtrp();     Trap-routine for signals                          */
/*  igcmos();     Interactive OS-command                            */
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
#include "../include/screen.h"
#include "../../EX/include/EX.h"
#include "../../GP/include/GP.h"

#include <signal.h>

#ifdef UNIX
#include <sys/ioctl.h>
/*#include <sys/termios.h>*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

#ifdef UNIX
/* static struct termios trmpar; */
extern MNUALT smbind[];

/*     trmpar innehåller terminalport-parametrar.
       smbind innehåller bla. snabbvalen för Quit och
       interrupt.
*/
#endif

extern short  igtrty,v3mode,ialy,rmarg,bmarg;
extern bool   intrup;

static void sigtrp(int sigval);

/*!******************************************************/

        short igintt()

/*      Sätter upp terminalporten.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/2/85 J. Kjellander
 *
 *      12/10/88 Tagit bort setbuf(), J. Kjellander
 *      1999-04-07 termios, J.Kjellander
 *
 ******************************************************!*/

  {
/*
#ifdef UNIX
    struct termios tmp;
#endif
*/

/*
***Det här skippar vi på en del datorer.
*/
#ifdef WIN32
    return(0);
#endif
/*
***Läs nuvarande terminalparametrar och spara i trmpar.
***Kopiera till tmp och sätt !ECHO samt !ICANON.
*
#ifdef UNIX
    if ( igtrty == VT100 )
      {
      igflsh();
      tcgetattr(0,&trmpar);
      V3MOME(&trmpar,&tmp,sizeof(struct termios));
      tmp.c_lflag &= ~ICANON;
      tmp.c_lflag &= ~ECHO;
      tcsetattr(0,TCSANOW,&tmp);
      }
#endif
*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igextt()

/*      Återställer terminalporten i ursprungligt skick.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      1999-04-07 termios, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Det här skippar vi på en del datorer.
*/
#ifdef WIN32
    return(0);
#endif
/*
***Skriv tillbaks trmpar.
*
#ifdef UNIX
    if ( igtrty == VT100 )
      {
      igflsh();
      tcsetattr(0,TCSANOW,&trmpar);
      }
#endif
*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igsini()

/*      Sätter upp vilka signaler som skall fångas.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/11/85 J. Kjellander
 *
 *      23/10/86 Ny hantering av SIGFPE, J. Kjellander
 *      4/3/92   Hangup, J. Kjellander
 *      3/4/92   SIGTERM, J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef WIN32
   return(0);
#else
/*
***Fånga signalerna hangup, interrupt och quit.
*/
    signal(SIGHUP,sigtrp);
    signal(SIGINT,sigtrp);
    signal(SIGQUIT,sigtrp);
    signal(SIGTERM,sigtrp);
/*
***Fånga inte flyttalsfel men låt core-fil skapas.
*/
    signal(SIGFPE,SIG_DFL);
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

static void sigtrp(int sigval)

/*      Trap-rutin för signaler.
 *
 *      In: sigval => Nummer på signal som orsakat trap.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/2/85 J. Kjellander
 *
 *      9/5/86   SIGFPE, J. Kjellander
 *      23/10/86 Ny hantering av SIGFPE, J. Kjellander
 *      16/3/88  v3exit(), J. Kjellander
 *      4/3/92   Hangup, J. Kjellander
 *      3/4/92   SIGTERM, J. Kjellander
 *      1998-02-22 void, J.Kjellander
 *
 ******************************************************!*/

  {

#ifdef WIN32
   return;
#else

   switch (sigval)
     {
/*
***Hangup, tex. om ett modem kopplar ner.
*/
     case SIGHUP:
     case SIGTERM:
     signal(SIGHUP,SIG_IGN);
     signal(SIGTERM,SIG_IGN);
     if ( v3mode & (BAS_MOD+RIT_MOD) ) igexsa();
     v3exit();
     break;
/*
***Interrupt, normalt <CTL>c.
*/
     case SIGINT:
     igsini();
     intrup = TRUE;
     break;
/*
***Quit, normalt <DEL>.
*/
     case SIGQUIT:
     if ( v3mode & (BAS_MOD+RIT_MOD) ) igexsa();
     v3exit();
     break;
     }
#endif

   return;
  }

/********************************************************/
/*!******************************************************/

       short igcmos(char oscmd[])

/*      Interaktivt kommando till OS.
 *
 *      In: oscmd -> Kommandosträng
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/3/88 J. Kjellander
 *
 *      4/11/88 CGI, N220G, J. Kjellander
 *
 ******************************************************!*/

 {
   char   s[2];
   short  status,oltrty;
   double tmpcng,tmprsk;
   VY     tmpavy;

/*
***Spara aktuell terminaltyp i oltrty. Under OS-kommandot
***sätts terminalen tillfälligt om till VT100 bla. för att
***kunna läsa in svaret på frågan "Fortsätt = <CR>".
*/
   oltrty = igtrty;

   switch ( igtrty )
     {
/*
***CGI, lämna CGI och gör skärmen till en VT100.
***ialy sätts här till 25 så att utskrifter från UNIX inte
***skrivs över (24 rader).
*/
     case CGI:
     gpgwin(&tmpavy);
     gpgrsk(&tmprsk);
     gpgtcn(&tmpcng);
     gpexit();
     igtrty = VT100;
     ialy = 25;
     break;
/*
***Tek4010-terminaler utan VT100-bildminne. Sudda skärmen
***och ställ den i VT100-mode.
*/
     case N220G:
     case MSCOLOUR:
     case MSMONO:
     igpstr("\033[?38l",NORMAL);
     igtrty = VT100;
     break;
     }
/*
***Övriga terminaler, rensa bara VT100-bildminnet och återställ
***porten i det läge den var då V3 startades.
*/
   igersc();
   igmvac(1,1);
   igextt();
/*
***Starta upp ny process och vänta tills den är klar.
***Oavsett terminaltyp har igintt() gjorts i v3.c, alltså
***måste igextt göras nu.
*/
   EXos(oscmd,(short)0);
/*
***Vänta på användarens <CR>.
*/
   igintt();
   status = igssip(iggtts(3),s,"",1);
/*
***Återställ skärm (och port).
*/
   igtrty = oltrty;

   switch ( igtrty )
     {
/*
***CGI, lämna VT-100 mode, initiera CGI och återställ
***skärmen utan att rita om grafiken.
*/
     case CGI:
     gpinit("CGI");
     gpswin(&tmpavy);
     gpstvi(&tmpavy);
     gpstcn(tmpcng);
     gpsrsk(tmprsk);
     gpdram();
     igupmu();
     iggnsa();
     break;
/*
***Tek-4010 utan VT100-bildminne. Återgå till Tek-mode,
***återställ skärmen utan att rita om grafiken.
*/
     case N220G:
     case MSCOLOUR:
     case MSMONO:
     igpstr("\033[?38h",NORMAL);
     gpdram();
     igupmu();
     iggnsa();
     break;
/*
***Skärmar med VT100-bildminne. Återställ VT100 skärmen.
***Grafiken ligger kvar i det grafiska bildminnet.
*/
     default:
     igmvac(1,1);
     igerar(rmarg,bmarg);
     igupmu();
     iggnsa();
     break;
     }
/*
***Slut.
*/
   return(status);
 }

/*********************************************************************/
