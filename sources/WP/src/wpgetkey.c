/**********************************************************************
*
*    wpgetkey.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPgtch();   Get single input
*    WPlups();   WP:s version of LookupString
*    WPkepf();   Serves PF key accelerators
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
#include "../include/WP.h"
#include <string.h>

#define XK_MISCELLANY
#define XK_LATIN1

#ifdef UNIX
#include <X11/keysymdef.h>
#endif

#define INBSIZ  256        /* Size of character buffer */

extern MNUALT   smbind[];

/*!*******************************************************/

     char     WPgtch(
     MNUALT **altptr,
     short   *alttyp,
     bool     flag)

/*   L�ser ett tecken fr�n tangentbordet och pollar
 *   samtidigt event-k�n. Om event finns servas de.
 *   Denna event-rutin anv�nds bara av V3 sj�lv och
 *   d� bara n�r en meny �r aktiv och V3 v�ntar p�
 *   svar. 
 *
 *   In: altptr = Pekare till menyalternativ-pekare.
 *       alttyp = Pekare till typ av alternativ/symbol.
 *       flag   = TRUE  => Alla typer av symboler till�tna.
 *                FALSE => Bara tecken till�tna.
 *
 *   Ut: **altptr = Pekare till alternativ.
 *        *alttyp = Typ av alternativ.
 *
 *   FV: Det l�sta tecknet och/eller symbol.
 *
 *   (C)microform ab 11/7/92 J. Kjellander
 *
 *    23/9/93    ClientMessageEvent, J. Kjellander
 *    25/1/94    Omarbetad, J. Kjellander
 *    31/1/95    Multif�nster, J. Kjellander
 *    20/1/96    tknbuf->static, J. Kjellander
 *    1996-02-12 Focus, J. Kjellander
 *    2006-12-11 ButtonPress->Release, J.Kjellander
 *    2007-07-25 1.19 J.Kjellander
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
***v�nta p� nytt event.
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
***heller n�gra tecken buffrade i tknbuf l�gger vi oss och v�ntar.
*/
start:
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
        WPwexp((XExposeEvent *)&event);
        break;
/*
***ConfigureNotify-event.
*/
        case ConfigureNotify:
        WPwcon((XConfigureEvent *)&event);
        break;
/*
***ReparentNotify-event.
*/
        case ReparentNotify:
        WPwrep((XReparentEvent *)&event);
        break;
/*
***Leave/Enter-notify.
*/
        case EnterNotify:
        case LeaveNotify:
        WPwcro((XCrossingEvent *)&event);
        break;
/*
***Left button could be a menu selection, a pos-button,
***A scroll_down in a WPLWIN or a button in a WPGWIN/WPRWIN.
***Optional mid button is always = GOMAIN.
***Right button is normally = REJECT but could be scroll_up in WPLWIN.
***Normally a ButtonRelease is the event that triggers
***an action but in  WPGWIN/WPRWIN we also need to handle ButtonPress.
***A button press is usually preceeded by a crosing event in that window.
***A tooltip may thus have been ordered but not yet displayed. To avoid
***that a ButtonPress always turns off any pending displays of tooltips.
*/
        case ButtonPress:
        WPclear_tooltip();
        if ( (serv_id=WPwfpx(butev->window)) >= 0 )
          {
          if ( wpwtab[serv_id].typ == TYP_RWIN ||
               wpwtab[serv_id].typ == TYP_GWIN ) WPwbut(butev,&serv_id);
          }
        break;

        case ButtonRelease:
        switch ( butev->button )
          {
          case 1:
          if ( WPmenu_button(&event,altptr) == TRUE )
            {
            if ( altptr == NULL ) goto start;                /* Pos-button */
            else                 *alttyp = SMBALT;           /* Alternative */
            }
          else if ( WPwbut(butev,&serv_id) == FALSE ) *alttyp = SMBRETURN;
          break;

          case 2:
         *alttyp = SMBMAIN;
          break;

          case 3:
          if ( WPwbut(butev,&serv_id) == FALSE )  *alttyp = SMBUP;
          break;
          }
        break;

/*
***A key press can either be a menu selection (if no command window has focus)
***or a command (if a command window has focus).
*/
        case KeyPress:
        if ( WPkey_mcwin(keyev) ) break;
/*
***En tangenttryckning kan generera noll, ett eller flera tecken.
***Oavsett antal lagras de f�rst i en intern fifo-stack och
***returneras till anroparen ett i taget.
***Mappa till motsvarande textstr�ng.
*/
        n = WPlups(keyev,&tknbuf[ntkn],INBSIZ-ntkn);
/*
***Did the key map to the string defined for Return, Main or Help ?
***In that case set n=0 (no characters) and return the symbol instead.
*/
        if ( n == 1 )
          {
          if ( tknbuf[ntkn] == 13 ) tknbuf[ntkn] = '\n';
          if      ( tknbuf[ntkn] ==  *smbind[1].str ) { *alttyp = SMBRETURN; n = 0; }
          else if ( tknbuf[ntkn] ==  *smbind[7].str ) { *alttyp = SMBMAIN;   n = 0; }
          else if ( tknbuf[ntkn] ==  *smbind[8].str ) { *alttyp = SMBHELP;   n = 0; }
          }
/*
***If not, it may have mapped to a string of one or more chars.
*/
        if ( n > 0 )
          {
         *alttyp = SMBCHAR;
          ntkn += n;
          }
/*
***If not mapped at all it may have been one of the PF-keys. If it
***is mapped to a function this function may return SMBMAIN.
*/
        else if ( WPkepf(keyev) == SMBMAIN )
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
        WPwfoc(focev);
        break;
/*
***Client message.
*/
        case ClientMessage:
        WPwclm((XClientMessageEvent *)&event);
        break;
/*
***Unknown event.
*/
        default:
        break;
        }
      }
/*
***Now all events are served.
***If the result maps to character(s), return
***the last character from tknbuf[] and return it.
*/
    if ( *alttyp == SMBCHAR )
      {
      c = tknbuf[0];
      if ( c == 13 ) c = '\n';
      --ntkn;
      for ( i=0; i<ntkn; ++i ) tknbuf[i] = tknbuf[i+1];
      }
/*
***The end.
*/
   return(c);
 }

/*********************************************************/
/*!******************************************************/

        short WPlups(
        XKeyEvent *keyev, 
        char      *s,
        int        tknmax)

/*      �vers�tter keycode och state i ett key-event
 *      till en str�ng.
 *
 *      In: keyev  = Pekare till key-event.
 *          s      = Pekare till utdata.
 *          tknmax = Max antal �nskde tecken.
 *
 *      Ut: *s = Motsvarande str�ng.
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
***sig under ODT, detta �r inte n�dv�ndigtvis standard.
*/
    if ( ((keyev->state & Mod2Mask)  > 0) ) numlock = TRUE;
/*
***Var shift-tangenten nere ?
*/
    if ( ((keyev->state & ShiftMask)  > 0) ) shift = TRUE;
/*
***Vi b�rjar med att anv�nda LookupString f�r att ta reda p�
***vilken keysym det var. Vi kunde anv�nda LookupKeysym() men
***LookupString() tar h�nsyn till shift, numlock etc. �t oss
***p� ett b�ttre s�tt (h�rdvaruoberoende).
*/
    ntkn = XLookupString(keyev,s,tknmax,&keysym,&costat);
/*
***Vissa symboler skall mappas p� ett f�r V3 speciellt s�tt.
*/
    switch ( keysym )
      {
/*
***Å, Ä och�Ö.
*/
      case XK_aring:      *s = '�'; ntkn = 1; break;
      case XK_Aring:      *s = '�'; ntkn = 1; break;
      case XK_adiaeresis: *s = '�'; ntkn = 1; break;
      case XK_Adiaeresis: *s = '�'; ntkn = 1; break;
      case XK_odiaeresis: *s = '�'; ntkn = 1; break;
      case XK_Odiaeresis: *s = '�';ntkn = 1; break;
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
      case XK_KP_Delete: if ( numlock == TRUE ) *s = '.'; ntkn = 1; break;
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
***Dessa �stadkommer inga egna tecken.
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
***�vriga tangenter anv�nder vi LookupString():s mappning.
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

        short WPkepf(
        XKeyEvent *keyev)

/*      Servar keyevent som kommer fr�n funktionstangent
 *      dvs. snabbval.
 *
 *      In: keyev   => Pekare till Key Event.
 *
 *      Ut: Inget.
 *
 *      FV: SMBESCAPE => Det var ett snabbval och det har servats.
 *          SMBMAIN   => Det var ett snabbval som avbr�ts med TAB.
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
***Till att b�rja med �vers�tter vi Key Eventet till motsvarande
***keysym med hj�lp av XLookupKeysym. Tidigare anv�ndes XLookupString
***men denna gav olika resultat i SCO/UNIX och tex. AIX betr�ffande
***tolkningen av shift s� f�r att vara p� s�kra sidan kollar vi detta
***sj�lva genom att f�rst be om den oshiftade betydelsen och sedan
***kolla om shift var nedtryckt. Detta b�r ge samma resultat i alla
***implementationer av X11.
*/
    keysym = XLookupKeysym(keyev,0);
/*
***Var shifttangenten nedtryckt ?
*/
    if ( keyev->state & ShiftMask ) shift = TRUE;
    else                            shift = FALSE;
/*
***Sen kollar vi om det var en funktionstangent. Om s�,
***mappar vi till motsvarande VARKON-str�ng.
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
         strcpy(tknbuf,IGqema());
         status = IGdofu(smbind[i].acttyp,smbind[i].actnum);
         if ( *tknbuf != '\0' ) WPaddmess_mcwin(tknbuf,WP_MESSAGE);
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
