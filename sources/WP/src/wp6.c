/**********************************************************************
*
*    wp6.c
*    ====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpgtch();   Get single input
*    wplups();   WP:s version of LookupString
*    wpkepf();   Serves PF key accelerators
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"
#include <string.h>

#define XK_MISCELLANY
#define XK_LATIN1

#ifdef V3_X11
#include <X11/keysymdef.h>
#endif

#define INBSIZ  256        /* Storlek på teckenbuffert */

extern MNUALT   smbind[];

/*!*******************************************************/

     char wpgtch(
     MNUALT **altptr,
     short   *alttyp,
     bool     flag)

/*   Läser ett tecken från tangentbordet och pollar
 *   samtidigt event-kön. Om event finns servas de.
 *   Denna event-rutin används bara av V3 själv och
 *   då bara när en meny är aktiv och V3 väntar på
 *   svar. 
 *
 *   In: altptr = Pekare till menyalternativ-pekare.
 *       alttyp = Pekare till typ av alternativ/symbol.
 *       flag   = TRUE  => Alla typer av symboler tillåtna.
 *                FALSE => Bara tecken tillåtna.
 *
 *   Ut: **altptr = Pekare till alternativ.
 *        *alttyp = Typ av alternativ.
 *
 *   FV: Det lästa tecknet och/eller symbol.
 *
 *   (C)microform ab 11/7/92 J. Kjellander
 *
 *    23/9/93    ClientMessageEvent, J. Kjellander
 *    25/1/94    Omarbetad, J. Kjellander
 *    31/1/95    Multifönster, J. Kjellander
 *    20/1/96    tknbuf->static, J. Kjellander
 *    1996-02-12 Focus, J. Kjellander
 *
 *******************************************************!*/

 {
    int                  i,n;
    XEvent               event;
    XKeyEvent           *keyev = (XKeyEvent    *)  &event;
    XButtonEvent        *butev = (XButtonEvent *)  &event;
    XFocusInEvent       *focev = (XFocusInEvent *) &event;
    char                 c = '\0';
    static char          tknbuf[INBSIZ];
    static int           ntkn = 0;
    wpw_id               serv_id;

/*
***Om tecken redan finns i tknbuf, returnera direkt utan att
***vänta på nytt event.
*/
    if ( ntkn > 0 )
      {
      c = tknbuf[0];
      if ( c == 13 ) c = '\n';
      --ntkn;
      for ( i=0; i<ntkn; ++i ) tknbuf[i] = tknbuf[i+1];
     *alttyp = SMBCHAR;
      return(c);
      }
/*
***Om events finns, serva dom. Om inga events finns och ej
***heller några tecken buffrade i tknbuf lägger vi oss och väntar.
*/
   *alttyp = SMBNONE;

    while ( XEventsQueued(xdisp,QueuedAfterReading) > 0  ||
                                                  *alttyp == SMBNONE )
      {
      XNextEvent(xdisp,&event);
      switch ( event.type )
        {
/*
***Expose-event.
*/
        case Expose:
        wpwexp((XExposeEvent *)&event);
        break;
/*
***ConfigureNotify-event.
*/
        case ConfigureNotify:
        wpwcon((XConfigureEvent *)&event);
        break;
/*
***ReparentNotify-event.
*/
        case ReparentNotify:
        wpwrep((XReparentEvent *)&event);
        break;
/*
***Leave/Enter-notify.
*/
        case EnterNotify:
        case LeaveNotify:
        wpwcro((XCrossingEvent *)&event);
        break;
/*
***Musknapp. Vänster knapp kan vara menyval. Höger knapp
***medför REJECT. Ev. mittknapp = GOMAIN.
*/
        case ButtonPress:
        switch ( butev->button )
          {
          case 1:
          if ( wpifae(&event,altptr) == TRUE ) *alttyp = SMBALT;
          else if ( wpwbut(butev,&serv_id) == FALSE ) *alttyp = SMBRETURN;
          break;

          case 2:
          if ( wpwbut(butev,&serv_id) == FALSE ) *alttyp = SMBMAIN;
          break;

          case 3:
          if ( wpwbut(butev,&serv_id) == FALSE ) *alttyp = SMBUP;
          break;
          }
        break;
/*
***En tangenttryckning kan generera noll, ett eller flera tecken.
***Oavsett antal lagras de först i en intern fifo-stack och
***returneras till iggtch() ett i taget.
*/
        case KeyPress:
/*
***Mappa till motsvarande textsträng.
*/
        n = wplups(keyev,&tknbuf[ntkn],INBSIZ-ntkn);
/*
***Blev det något resultat ?
*/
        if ( n > 0 )
          {
          ntkn += n;
         *alttyp = SMBCHAR;
          }
/*
***Nej, då kan det ha varit en funktionstangent.
*/
        else if ( wpkepf(keyev) == SMBMAIN )
          {
         *alttyp = SMBCHAR;
          tknbuf[ntkn] = *smbind[7].str;
          ntkn += 1;
          }
        break;
/*
***Focus.
*/
        case FocusIn:
        wpwfoc(focev);
        break;
/*
***Client message.
*/
        case ClientMessage:
        wpwclm((XClientMessageEvent *)&event);
        break;
/*
***Okänt event.
*/
        default:
        break;
        }
      }
/*
***Nu är alla events servade och vi har antingen ett tecken
***att returnera eller en mustryckning i meny-fönstret.
*/
    if ( *alttyp == SMBCHAR )
      {
      c = tknbuf[0];
      if ( c == 13 ) c = '\n';
      --ntkn;
      for ( i=0; i<ntkn; ++i ) tknbuf[i] = tknbuf[i+1];
      }
/*
***Slut.
*/
   return(c);
 }

/*********************************************************/
/*!******************************************************/

        short wplups(
        XKeyEvent *keyev, 
        char      *s,
        int        tknmax)

/*      Översätter keycode och state i ett key-event
 *      till en sträng.
 *
 *      In: keyev  = Pekare till key-event.
 *          s      = Pekare till utdata.
 *          tknmax = Max antal önskde tecken.
 *
 *      Ut: *s = Motsvarande sträng.
 * 
 *      Fv:  Antal tecken.
 *
 *      (C)microform ab 12/8/92 J. Kjellander
 *
 *      931203 Shiftade Funktionstang. J. Kjellander
 *      1997-10-16 Bug, 8-bits ASCII, J.Kjellander
 *
 ******************************************************!*/

  {
    bool            numlock = FALSE;
    bool            shift   = FALSE;
    KeySym          keysym;
    XComposeStatus  costat;
    int             ntkn = 0;

/*
***Numlock = Mod2Mask har det visat
***sig under ODT, detta är inte nödvändigtvis standard.
*/
    if ( ((keyev->state & Mod2Mask)  > 0) ) numlock = TRUE;
/*
***Var shift-tangenten nere ?
*/
    if ( ((keyev->state & ShiftMask)  > 0) ) shift = TRUE;
/*
***Vi börjar med att använda LookupString för att ta reda på
***vilken keysym det var. Vi kunde använda LookupKeysym() men
***LookupString() tar hänsyn till shift, numlock etc. åt oss
***på ett bättre sätt (hårdvaruoberoende).
*/
    ntkn = XLookupString(keyev,s,tknmax,&keysym,&costat);
/*
***Vissa symboler skall mappas på ett för V3 speciellt sätt.
*/
    switch ( keysym )
      {
/*
***å, ä och ö.
*/
      case XK_aring:      *s = 'å'; ntkn = 1; break;
      case XK_Aring:      *s = 'Å'; ntkn = 1; break;
      case XK_adiaeresis: *s = 'ä'; ntkn = 1; break;
      case XK_Adiaeresis: *s = 'Ä'; ntkn = 1; break;
      case XK_odiaeresis: *s = 'ö'; ntkn = 1; break;
      case XK_Odiaeresis: *s = 'Ö';ntkn = 1; break;
/*
***Funktionstangenter.
*/
      case XK_F1:  case XK_F2:  case XK_F3:  case XK_F4:
      case XK_F5:  case XK_F6:  case XK_F7:  case XK_F8:
      case XK_F9:  case XK_F10: case XK_F11: case XK_F12:
      case XK_F13: case XK_F14: case XK_F15: case XK_F16:
      case XK_F17: case XK_F18: case XK_F19: case XK_F20:
      case XK_F21: case XK_F22: case XK_F23: case XK_F24:
      case XK_F25: case XK_F26: ntkn = 0; break;
/*
***Delete-tangenten skall i shiftad version mappas
***till '.' Med normal shift mappas den till KP_Separator
***och med numlock inte alls.
*/
      case XK_Delete: if ( numlock == TRUE ) *s = '.'; ntkn = 1; break;
/*
***Keypad-separator skall mappas till '.'
*/
      case XK_KP_Separator: *s = '.'; ntkn = 1; break;
/*
***Piltangenter.
*/
      case XK_Up:    strcpy(s,"\033[A"); ntkn = 3; break;
      case XK_Down:  strcpy(s,"\033[B"); ntkn = 3; break;
      case XK_Left:  strcpy(s,"\033[D"); ntkn = 3; break;
      case XK_Right: strcpy(s,"\033[C"); ntkn = 3; break;
/*
***Diverse kombinations-tangenter typ Shift, CapsLock, Ctrl och Alt.
***Dessa åstadkommer inga egna tecken.
*/
      case XK_Shift_L: 
      case XK_Shift_R: 
      case XK_Control_L: 
      case XK_Control_R: 
      case XK_Alt_L: 
      case XK_Alt_R: 
      case XK_Caps_Lock: 
      case XK_Mode_switch: 
      case XK_Num_Lock:
      ntkn = 0;
      break;
/*
***Övriga tangenter använder vi LookupString():s mappning.
*/
      }
/*
***Sist ett NULL.
*/
   *(s+ntkn) = '\0';

    return((short)ntkn);
  }

/********************************************************/
/*!******************************************************/

        short wpkepf(
        XKeyEvent *keyev)

/*      Servar keyevent som kommer från funktionstangent
 *      dvs. snabbval.
 *
 *      In: keyev   => Pekare till Key Event.
 *
 *      Ut: Inget.
 *
 *      FV: SMBESCAPE => Det var ett snabbval och det har servats.
 *          SMBMAIN   => Det var ett snabbval som avbröts med TAB.
 *          SMBNONE   => Det var inget snabbval.
 *
 *      (C)microform ab 13/12/94 J. Kjellander
 *
 *      1996-01-31 Shift, J. Kjellander
 *
 ******************************************************!*/

 {
    short           status;
    int             i,ntkn;
    char            tknbuf[MAXTXT+1];
    bool            shift;
    KeySym          keysym;

/*
***Till att börja med översätter vi Key Eventet till motsvarande
***keysym med hjälp av XLookupKeysym. Tidigare användes XLookupString
***men denna gav olika resultat i SCO/UNIX och tex. AIX beträffande
***tolkningen av shift så för att vara på säkra sidan kollar vi detta
***själva genom att först be om den oshiftade betydelsen och sedan
***kolla om shift var nedtryckt. Detta bör ge samma resultat i alla
***implementationer av X11.
*/
    keysym = XLookupKeysym(keyev,0);
/*
***Var shifttangenten nedtryckt ?
*/
    if ( keyev->state & ShiftMask ) shift = TRUE;
    else                            shift = FALSE;
/*
***Sen kollar vi om det var en funktionstangent. Om så,
***mappar vi till motsvarande VARKON-sträng.
*/
    switch ( keysym )
      {
      case XK_F1:  strcpy(tknbuf,"\033A"); ntkn = 2; break;
      case XK_F2:  strcpy(tknbuf,"\033B"); ntkn = 2; break;
      case XK_F3:  strcpy(tknbuf,"\033C"); ntkn = 2; break;
      case XK_F4:  strcpy(tknbuf,"\033D"); ntkn = 2; break;
      case XK_F5:  strcpy(tknbuf,"\033E"); ntkn = 2; break;
      case XK_F6:  strcpy(tknbuf,"\033F"); ntkn = 2; break;
      case XK_F7:  strcpy(tknbuf,"\033G"); ntkn = 2; break;
      case XK_F8:  strcpy(tknbuf,"\033H"); ntkn = 2; break;
      case XK_F9:  strcpy(tknbuf,"\033I"); ntkn = 2; break;
      case XK_F10: strcpy(tknbuf,"\033J"); ntkn = 2; break;
      case XK_F11: strcpy(tknbuf,"\033K"); ntkn = 2; break;
      case XK_F12: strcpy(tknbuf,"\033L"); ntkn = 2; break;
      case XK_F13: strcpy(tknbuf,"\033M"); ntkn = 2; break;
      case XK_F14: strcpy(tknbuf,"\033N"); ntkn = 2; break;
      case XK_F15: strcpy(tknbuf,"\033O"); ntkn = 2; break;
      case XK_F16: strcpy(tknbuf,"\033P"); ntkn = 2; break;
      case XK_F17: strcpy(tknbuf,"\033Q"); ntkn = 2; break;
      case XK_F18: strcpy(tknbuf,"\033R"); ntkn = 2; break;
      case XK_F19: strcpy(tknbuf,"\033S"); ntkn = 2; break;
      case XK_F20: strcpy(tknbuf,"\033T"); ntkn = 2; break;
      case XK_F21: strcpy(tknbuf,"\033U"); ntkn = 2; break;
      case XK_F22: strcpy(tknbuf,"\033V"); ntkn = 2; break;
      case XK_F23: strcpy(tknbuf,"\033W"); ntkn = 2; break;
      case XK_F24: strcpy(tknbuf,"\033X"); ntkn = 2; break;
      case XK_F25: strcpy(tknbuf,"\033Y"); ntkn = 2; break;
      case XK_F26: strcpy(tknbuf,"\033Z"); ntkn = 2; break;
/*
***Om det inte var en funktionstangent returnerar vi FALSE.
*/
      default: return(SMBNONE);
      }
/*
***En av de 26 funktionstangenterna var det. Om shift var
***nedtryckt samtidigt byter vi den stora bokstaven mot en
***liten.
*/
   if ( shift ) tknbuf[1] += 32;
/*
***Funktionstangent var det, vilket snabbval blir det ?
*/
   for ( i=0; i<SMBMAX; ++i )
     {
     if ( smbind[i].str != NULL )
       {
       if ( strncmp(smbind[i].str,tknbuf,ntkn) == 0 )
         {
         strcpy(tknbuf,igqema());
         if ( *tknbuf != '\0' ) igrsma();
         status = igdofu(smbind[i].acttyp,smbind[i].actnum);
         if ( *tknbuf != '\0' ) igplma(tknbuf,IG_MESS);
         if ( status == GOMAIN ) return(SMBMAIN);
         else                    return(SMBESCAPE);
         break;
         }
       }
     }
/*
***Om funktionstangenten inte var programmerad returnerar
***vi SMBNONE.
*/
   return(SMBNONE);
 }

/********************************************************/
