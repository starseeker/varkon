/*!******************************************************************/
/*  iginput.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGssip();   Single string input                                 */
/*  IGsfip();   Single float value input                            */
/*  IGsiip();   Single int value input                              */
/*  IGmsip();   Multiple string input                               */
/*  IGialt();   Input alternative with t-strings                    */
/*  IGials();   Input alternative with C-strings                    */
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
#include "../../WP/include/WP.h"

extern MNUALT smbind[];      /* Symboler */
extern short  mant;          /* Antal aktiva menyer */

/*!*******************************************************/

     short IGssip(
     char *prompt,                 /* Main prompt */
     char *ps,                     /* promptsträng */
     char *is,                     /* inputsträng */
     char *ds,                     /* defaultsträng */
     short ml)                     /* maximal stränglängd */

/*   Single string input.
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = ï¿½tervï¿½nd till huvudmenyn.
 *
 *   REVIDERAD:
 *
 *   16/9-85 Snabbval, Ulf Johansson
 *   6/10/86 GOMAIN, J. Kjellander
 *
 *******************************************************!*/

{
    return(IGmsip(prompt,&ps,&is,&ds,&ml,1));
}

/*********************************************************/
/*!*******************************************************/

     short IGsfip(
     char    *ps,                   /* promptstrï¿½ng */
     DBfloat *fval)                 /* flyttalet */

/*   Lï¿½s in ett flyttal med redigering och snabbval.
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = ï¿½tervï¿½nd till huvudmenyn.
 *
 *   REVIDERAD:
 *
 *   16/9-85 Snabbval, Ulf Johansson
 *   6/10/86 GOMAIN, J. Kjellander
 *
 *******************************************************!*/

{
    char ch[ 1 + 1 ];
    char is[ MAXDTXT + 1 ];
    short retsmb;

    do
      {
      if ( (retsmb = IGssip("",ps,is,"",MAXDTXT)) == REJECT ||
              retsmb == GOMAIN ) break;

      is[strlen(is)] = '\b';
      if ( sscanf(is,"%lf%1s",fval,ch) == 2 )
        {
        if (*ch == '\b') break;
        }

      else WPbell();
    } while (TRUE);

    return(retsmb);
}

/*********************************************************/
/*!*******************************************************/

    short IGsiip(
    char  *ps,                              /* promptstrï¿½ng */
    int *ival)                              /* heltalet */

/*   Lï¿½s in ett heltal med redigering och snabbval.
 *
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = ï¿½tervï¿½nd till huvudmenyn.
 *
 *   REVIDERAD:
 *
 *   16/9-85 Snabbval, Ulf Johansson
 *   6/10/86 GOMAIN, J. Kjellander
 *
 *******************************************************!*/

{
    char ch[ 1 + 1 ];
    char is[ MAXLITXT + 1 ];
    short retsmb;

    do
      {
      if ( (retsmb=IGssip("",ps,is,"",MAXLITXT)) == REJECT ||
              retsmb == GOMAIN ) break;

      is[strlen(is)] = '\b';
      if ( sscanf(is,"%d%1s",ival,ch) == 2 )
        {
        if ( *ch == '\b' ) break;
        }

      else WPbell();
      } while (TRUE);

    return(retsmb);
}

/*********************************************************/
/*!*******************************************************/

     short IGmsip(
     char *prompt,                 /* Main prompt */
     char *ps[],                   /* input prompts */
     char *is[],                   /* input strings */
     char *ds[],                   /* default strings */
     short ml[],                   /* max string lengths */
     short as)                     /* number of strings to get */

/*   Get one or more strings.
 *
 *   Return:   0 = OK
 *        REJECT = Operationen cancelled.
 *        GOMAIN = Back to main menu.
 *
 *   16/9-85  Snabbval, Ulf Johansson
 *   6/10/86  GOMAIN, J. Kjellander
 *   13/10/86 SMBMAIN, J. Kjellander
 *   8/11/88  snabb till getsmb(), J. Kjellander
 *   1996-12-12 typarr, J.Kjellander
 *   2007-09-09 prompt, J.Kjellander
 *
 *******************************************************!*/
{
    int   i,typarr[V2MPMX];
    short status;

/*
***Use WPmsip(). All inputs are strings.
*/
    for ( i=0; i<as; ++i ) typarr[i] = C_STR_VA;

    status = WPmsip(prompt,ps,ds,is,ml,typarr,as);
/*
***The end.
*/
   return(status);
}

/**************************************************************/
/*!*******************************************************/

     bool IGialt(
     short psnum,
     short tsnum,
     short fsnum,
     bool  pip)

/*   Lï¿½ser in alternativ (1 tecken) typ j/n, +/- etc.
 *
 *   In: psnum = Promptstrï¿½ngnummer
 *       tsnum = TRUE-strï¿½ngnummer
 *       fsnum = FALSE-strï¿½ngnummer
 *       pip   = Inledande pip, Ja/Nej
 *
 *   Ut: Inget.
 *
 *   FV: TRUE  om svar = 1:a tecknet i tsnum
 *       FALSE om svar = 1:a tecknet i fsnum
 *
 *   (C)microform ab 15/3/86 J. Kjellander
 *
 *   23/10/86   pip, J. Kjellander
 *   1998-09-17 IGials(), J.Kjellander
 *
 *******************************************************!*/

 {
    if ( psnum > 0 )
      return(IGials(IGgtts(psnum),IGgtts(tsnum),IGgtts(fsnum),pip));
    else
      return(IGials("",IGgtts(tsnum),IGgtts(fsnum),pip));
 }

/*********************************************************/
/*!*******************************************************/

     bool IGials(
     char *ps,
     char *ts,
     char *fs,
     bool  pip)

/*   Samma som IGialt men med C-strï¿½nga som indata.
 *
 *   In: ps  = Prompt
 *       ts  = TRUE-strï¿½ng
 *       fs  = FALSE-strï¿½ng
 *       pip = Inledande pip, Ja/Nej
 *
 *   Ut: Inget.
 *
 *   FV: TRUE  om svar = 1:a tecknet i tsnum
 *       FALSE om svar = 1:a tecknet i fsnum
 *
 *   (C)microform ab 1998-09-17 J. Kjellander
 *
 *******************************************************!*/

 {

/*
***X11.
*/
#ifdef UNIX
      return(WPialt(ps,ts,fs,pip));
#endif
/*
***WIN32.
*/
#ifdef WIN32
      return(msialt(ps,ts,fs,pip));
#endif

 }

/*********************************************************/
