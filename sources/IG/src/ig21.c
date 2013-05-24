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
#include "../include/screen.h"

extern short igtrty,rmarg,ialy,ialx;

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
    short prx[ MSMAX ];                     /* promptlägen */
    short isx[ MSMAX ];                     /* inputfältlägen */
    short fwdt[ MSMAX ];                    /* fältlängder */
    short ilen,plen;
    short tab,air,retsmb=0;
    short cursor[ MSMAX ];                  /* cursor pos */
    short scroll[ MSMAX ];                  /* fönster pos */
    bool dfuse[ MSMAX ];                    /* defaultsäkringar */
    short i,j;

/*
***X11.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      for ( i=0; i<as; ++i ) typarr[i] = C_STR_VA;
      return(wpmsip(igqema(),ps,ds,is,ml,typarr,as));
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
/*
***Gör inmatning.
*/
    if ( as < 1 ) as = 1;
    else if ( as > MSMAX ) as = MSMAX;

    air = 1;                                   /* luft mellan fälten */
    tab = ((rmarg-ialx)/as) - (air*(as - 1));  /* max utrymme per fält */

    j = ialx;                                /* aktuellt x-läge */

    for ( i=0; i < as; i++ )
      {
      prx[i] = j;                            /* promtens läge */
      plen = strlen(ps[i]) + 1;              /* promtlängd */
      isx[i] = j + plen;                     /* infältets läge */
      ilen = (tab - 1) - plen;               /* infältets max längd */
      if (ilen > ml[i])                      /* behövs inte hela fältet ? */
           ilen = ml[i];
      fwdt[i] = ilen;                        /* infältets längd */
      j = isx[i] + fwdt[i] + 1 + air;        /* nästa promt */ 
      }
/*
***Promt och defaultvärde.
*/
    for (i = 0; i < as; i++)
      {
      igmvac(prx[ i ],ialy);     
      igpstr(ps[ i ],REVERS);           /* ut med prompten */

      igdfld(isx[ i ],fwdt[ i ],ialy);
      strcpy(is[ i ],ds[ i ]);            /* default -> infältet */  
      dfuse[ i ] = TRUE;
      cursor[ i ] = 0;
      scroll[ i ] = 0;
      igfstr(is[ i ],JULEFT,NORMAL);      /* infältet */ 
      }

    j = 0;
    while ((j >= 0) && (j < as)) {

        retsmb = 0;
        igdfld(isx[ j ],fwdt[ j ],ialy);
/*
***Läs in sträng med redigering och snabbval.
*/
        switch (iglned(is[ j ],&cursor[ j ],&scroll[ j ],
                        &dfuse[ j ],ds[ j ],ml[ j ],NORMAL,TRUE)) {

             case SMBRETURN:
             case SMBDOWN:
                  if (strlen(is[ j ]) == 0) {
                       j--;
                       retsmb = REJECT;
                  } else
                       j++;
                  break;

             case SMBMAIN:
                  retsmb = GOMAIN;
                  goto end;

             case SMBPOSM:
                  retsmb = POSMEN;
                  goto end;

             case SMBUP:
                  j--;
                  retsmb = REJECT;
                  break;
/*
***Hjälp.
*/
             case SMBHELP:
                  if ( ighelp() == GOMAIN )
                    {
                    retsmb = GOMAIN;
                    goto end;
                    }
/*
***Snabbval och hjälp, återställ inmatningsraden.
*/
             case SMBESCAPE:
                  igmvac(ialx,ialy);
                  igerar(rmarg,1);
                  for (i = 0; i < as; i++)
                     {
                     igmvac(prx[ i ],ialy);     
                     igpstr(ps[ i ],REVERS);
                     igdfld(isx[ i ],fwdt[ i ],ialy);
                     igfstr((is[ i ] + scroll[ i ]),JULEFT,NORMAL);
                     }
                  break;
        }
    }

/*
***Avsluta.
*/
end:
    igmvac(ialx,ialy);
    igerar(rmarg,1);

    return(retsmb);
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
    char c;
/*
***X11.
*/
#ifdef V3_X11
      if ( igtrty == X11 )
        return(wpialt(ps,ts,fs,pip));
#endif
/*
***WIN32.
*/
#ifdef WIN32
      return(msialt(ps,ts,fs,pip));
#endif
/*
***Ev. ett litet pip.
*/
    if ( pip ) igbell();
/*
***Ev. prompt.
*/
loop:
    if ( strlen(ps) > 0 ) igplma(ps,IG_INP);
/*
***Läs in svar, max ett tecken och jämför med ts och fs.
*/
    c = iggtch();

    if ( strlen(ps) > 0 ) igrsma();

    if ( c == *ts ) return(TRUE);
    else if ( c == *fs ) return(FALSE);
    else
     {
     igbell();
     goto loop;
     }
 }

/*********************************************************/
