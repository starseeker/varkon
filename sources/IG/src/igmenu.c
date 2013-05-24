/*!******************************************************************/
/*  igmenu.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*     IGaamu();         Add menu (to stack) and make active        */
/*     IGsamu();         Sub menu and make previous active          */
/*     IGgalt();         Read choice from active menu               */
/*     IGsmmu();         Sets main menu number                      */
/*     IGgmmu();         Returns number of main menu                */
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
#include "../../WP/include/WP.h"
#include <string.h>

short mstack[MENLEV+1];         /* The menu stack */
short mant = 0;                 /* Number of menus on the stack */

static short mmain;             /* Current number of the main menu */

extern MNUDAT  mnutab[];        /* All menus */
extern short   v3mode;          /* Current system mode */

/*!****************************************************************/

         bool IGaamu(int mnum)

/*       Add a menu to the stack and make it active.
 *
 *       In: mnum = Menu number.
 *
 *       (C)2007-02-14, J. Kjellander
 *
 ****************************************************************!*/

  {
   int m;

/*
***If there is still space for more menus....
*/
    if ( mant < MENLEV )
      {
/*
***add this one....
*/
      mstack[mant++] = mnum;
/*
***and display it.
*/
    if ( mant > 0 )
      {
      if ( (m=mstack[mant-1]) == 0 ) m = IGgmmu();
      WPactivate_menu(&mnutab[m]);
      }

    return(TRUE);
    }
    else return(FALSE);
  }

/******************************************************************/
/*!****************************************************************/

        bool IGsamu()

/*      Subtract a menu from the stack and make
 *      previous menu active.
 *
 *      (C)2007-02-14, J. Kjellander
 *
 ****************************************************************!*/

  {
    int m;

/*
***If there are any menus on the stack....
*/
    if ( mant > 0 )
      {
/*
***push the active one.
*/
      mant--;
/*
***If any left, activate the new one.
*/
    if ( mant > 0 )
      {
      if ( (m=mstack[mant-1]) == 0 ) m = IGgmmu();
      WPactivate_menu(&mnutab[m]);
      }

      return(TRUE);
      }
      else return(FALSE);
  }

/******************************************************************/
/*!****************************************************************/

        short IGgalt(
        MNUALT **paltp,
        short   *ptyp)

/*      Handles menu selections. Uses WPgtch() to get user
 *      input. Some types of input are served locally.
 *
 *      Out: *paltp = Ptr to alternative or NULL
 *           *ptyp  = Type of alternative if paltp=NULL
 *
 *      22/10-85 Symboler, Ulf Johansson
 *      6/11/85  Meny noll, J. Kjellander
 *      24/2/86  Input läge i X-led R. Svedin
 *      2/10/86  Direkt till huvudmeny, J. Kjellander
 *      12/10/86 Historik, J. Kjellander
 *      28/10/86 Flyttat menyarean, R. Svedin
 *      8/11/88  Snabbval, J. Kjellander
 *      7/8/91   Pil uppåt, J. Kjellander
 *      23/8/92  X11, J. Kjellander
 *
 ****************************************************************!*/

  {
    MNUDAT *menu;
    MNUALT *alt;
    short   i,nalt,cnum,nhits,mnum,altnum,symbol;
    char    c,cbuf[81];
    bool    exit;

/*
***What menu is currently active ?.
*/
    if ( (mnum=mstack[mant-1]) == 0 ) mnum = IGgmmu();
    menu = &mnutab[mnum];
    nalt = menu->nalt;
/*
***No characters read yet.
*/
    cnum    = 0;
    cbuf[0] = '\0';
/*
***Get user input, ie. a symbol.
*/
    exit = FALSE;

    while ( !exit )
      {
      c = WPgtch(&alt,&symbol,TRUE);
      switch ( symbol )
        {
/*
***Check cbuf for hit against all menu alternative texts.
*/
        case SMBCHAR:
        cbuf[cnum++] = c;
        cbuf[cnum] = '\0';

        i = nhits = altnum = 0;
        alt = menu->alt;

        while ( i++ < nalt )
          {
          if (strncmp(alt->str,cbuf,cnum) == 0)
            {
            nhits++;
           *paltp = alt;
            altnum = i;
            }
          alt++;
          }
/*
***No menu alternatie. Maybe RJECT or GOMAIN ?
*/
        if ( nhits <= 0 )
          {
          WPbell();
          cbuf[ --cnum ] = '\0';
          }
        else if ( nhits == 1 ) exit = TRUE;
        break;
/*
***Return or arrow up.
*/
        case SMBRETURN:
        case SMBUP:
        *paltp = NULL;
        *ptyp = SMBRETURN;
        exit = TRUE;
        break;
/*
***Back to main menu.
*/
        case SMBMAIN:
        goto gomain;
        break;
/*
***Menu alternative.
*/
        case SMBALT:
       *paltp = alt;
        WPunfocus_mcwin();
        return(0);
        break;
/*
***Help.
*/
        case SMBHELP:
        IGhelp();
        break;
/*
***Error.
*/
        default:
        WPbell();
        break;
        }
      }
/*
***The end.
*/
    return(0);
/*
***GOMAIN.
*/
gomain:
   *paltp = NULL;
   *ptyp = SMBMAIN;
    WPunfocus_mcwin();
    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        short IGsmmu(int m)

/*      Sets the menu to be used as main menu.
 *
 *      In: m = Menu number.
 *
 *      (C)microform 1996-02-26  J.Kjellander
 *
 ****************************************************************!*/

  {
    mmain = m;

    return(0);
  }

/******************************************************************/
/*!****************************************************************/

        int IGgmmu()

/*      Returns the number of the main menu.
 *
 *      In: Inget.
 *
 *      FV: menynummer.
 *
 *      (C)microform 1996-02-26  J.Kjellander
 *
 ****************************************************************!*/

  {

/*
***If mmain = 0 use the system main menu, ie. 2, 3 or 4
***depending on v3mode. Otherwise the user has specified
***a custom menu in his MDF-file and then we use that.
*/
   if ( mmain == 0 )
     {
     switch ( v3mode )
       {
       case BAS2_MOD: return(2);
       case BAS3_MOD: return(3);
       case RIT_MOD:  return(4);
       default:       return(-1);
       }
     }
/*
***Om mmain <> 0 har denna huvudmeny angetts som huvudmeny
***i den aktuella menyfilen.  main_menu = nnn.
*/
   else return(mmain);
  }

/******************************************************************/
