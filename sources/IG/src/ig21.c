/*!******************************************************************/
/*  File: ig21.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igssip();   Single string input                                 */
/*  igsfip();   Single float value input                            */
/*  igsiip();   Single int value input                              */
/*  igmsip();   Multiple string input                               */
/*  igialt();   Input alternative with t-strings                    */
/*  igials();   Input alternative with C-strings                    */
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
#include "../../WP/include/WP.h"

extern short igtrty;

/*!*******************************************************/

     short igssip(
     char *ps,                     /* promptsträng */
     char *is,                     /* inputsträng */
     char *ds,                     /* defaultsträng */
     short ml)                     /* maximal stränglängd */

/*   Läs in en sträng med redigering och snabbval.
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = Återvänd till huvudmenyn.
 *
 *   REVIDERAD:
 *
 *   16/9-85 Snabbval, Ulf Johansson
 *   6/10/86 GOMAIN, J. Kjellander
 *
 *******************************************************!*/

{
    return(igmsip(&ps,&is,&ds,&ml,1));
}

/*********************************************************/
/*!*******************************************************/

     short igsfip(
     char    *ps,                   /* promptsträng */
     DBfloat *fval)                 /* flyttalet */

/*   Läs in ett flyttal med redigering och snabbval.
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = Återvänd till huvudmenyn.
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
      if ( (retsmb = igssip(ps,is,"",MAXDTXT)) == REJECT ||
              retsmb == GOMAIN || retsmb == POSMEN ) break;

      is[strlen(is)] = '\b';
      if ( sscanf(is,"%lf%1s",fval,ch) == 2 )
        {
        if (*ch == '\b') break;
        }

      else igbell();
    } while (TRUE);

    return(retsmb);
}

/*********************************************************/
/*!*******************************************************/

    short igsiip(
    char  *ps,                              /* promptsträng */
    int *ival)                              /* heltalet */

/*   Läs in ett heltal med redigering och snabbval.
 *
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = Återvänd till huvudmenyn.
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
      if ( (retsmb=igssip(ps,is,"",MAXLITXT)) == REJECT ||
              retsmb == GOMAIN || retsmb == POSMEN ) break;

      is[strlen(is)] = '\b';
      if ( sscanf(is,"%d%1s",ival,ch) == 2 )
        {
        if ( *ch == '\b' ) break;
        }

      else igbell();
      } while (TRUE);

    return(retsmb);
}

/*********************************************************/
/*!*******************************************************/

     short igmsip(
     char *ps[],                   /* promptsträngar */
     char *is[],                   /* inputsträngar */
     char *ds[],                   /* defaultsträngar */
     short ml[],                   /* maximala stränglängder */
     short as)                     /* antal strängar att läsa in */

/*   Läs in flera strängar med redigering och snabbval.
 *
 *   FV:       0 = OK
 *        REJECT = Operationen avbruten.
 *        GOMAIN = Återvänd till huvudmenyn.
 *
 *   16/9-85  Snabbval, Ulf Johansson
 *   6/10/86  GOMAIN, J. Kjellander
 *   13/10/86 SMBMAIN, J. Kjellander
 *   8/11/88  snabb till getsmb(), J. Kjellander
 *   1996-12-12 typarr, J.Kjellander
 *
 *******************************************************!*/
{
    int   typarr[V2MPMX];
    short i;

/*
***X11.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      for ( i=0; i<as; ++i ) typarr[i] = C_STR_VA;
      return(WPmsip(igqema(),ps,ds,is,ml,typarr,as));
      }
#endif
/*
***WIN32
*/
#ifdef WIN32
    for ( i=0; i<as; ++i ) typarr[i] = C_STR_VA;
    if ( as < 4 )return((short)msdl03(igqema(),ps,ds,is,ml,(int)as));
    else         return((short)msmsip(igqema(),ps,ds,is,ml,typarr,as));
#endif

   return(0);
}

/**************************************************************/
/*!*******************************************************/

     bool igialt(
     short psnum,
     short tsnum,
     short fsnum,
     bool  pip)

/*   Läser in alternativ (1 tecken) typ j/n, +/- etc.
 *
 *   In: psnum = Promptsträngnummer
 *       tsnum = TRUE-strängnummer
 *       fsnum = FALSE-strängnummer
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
 *   1998-09-17 igials(), J.Kjellander
 *
 *******************************************************!*/

 {
    if ( psnum > 0 )
      return(igials(iggtts(psnum),iggtts(tsnum),iggtts(fsnum),pip));
    else
      return(igials("",iggtts(tsnum),iggtts(fsnum),pip));
 }

/*********************************************************/
/*!*******************************************************/

     bool igials(
     char *ps,
     char *ts,
     char *fs,
     bool  pip)

/*   Samma som igialt men med C-stränga som indata.
 *
 *   In: ps  = Prompt
 *       ts  = TRUE-sträng
 *       fs  = FALSE-sträng
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
#ifdef V3_X11
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
