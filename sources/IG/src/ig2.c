/*!******************************************************************/
/*  File: ig2.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  iggtts();     Get C-ptr to t-string                             */
/*  igbell();     Ring bell                                         */
/*  iggtsm();     Read symbol from stdin                            */
/*  igglin();     MBS-read                                          */
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
#include <string.h>

#ifdef UNIX
#undef VSTART
#include "termio.h"
#endif

extern MNUALT smbind[];      /* Symboler */
extern char   txtmem[];      /* Alla t-strängar i en "ragged" array */
extern char  *txtind[];      /* Pekare till alla textsträngar */
extern short  igtrty;        /* Terminaltyp */
extern short  mant;          /* Antal aktiva menyer */
extern char   hstack[][40];
extern char   jobdir[],jobnam[];

/*!******************************************************/

       char *iggtts(short tnr)

/*      Returnerar en c-pekare till den med tnr angivna
 *      t-strängen.
 *
 *      In: tnr = T-strängpekarens index i txtind.
 *
 *      FV: C-pekare till sträng.
 *
 *      (C)microform ab 1996-05-30 J.Kjellander
 *
 ******************************************************!*/


  {
    char  *tp; 
    static char notdef[80];

/*
***Har tnr ett rimligt värde ?
*/
    if ( tnr < 0  ||  tnr >= TXTMAX )
      {
      sprintf(notdef,"<invalid t%d>",tnr);
      return(notdef);
      }
/*
***Fixa fram motsvarande pekare.
*/
    tp = txtind[tnr];
/*
***Är den definierad ?
*/
    if ( tp == NULL )
      {
      sprintf(notdef,"<no t%d>",tnr);
      return(notdef);
      }

    return(tp);
  }

/*!******************************************************/
/*!******************************************************/

       void igbell()

/*     Säger pip!
 *
 *      (C)microform ab J. Kjellander
 *
 *      1998-03-13 Bug x11->X11, J.Kjellander
 *
 ******************************************************!*/

  {

#ifdef V3_X11
    XBell(xdisp,100);
#endif
  }

/*!******************************************************/
/*!******************************************************/

       short iggtsm(
       char   *cp,
       MNUALT **altptr)

/*     Läser symbol från standard input.
 *
 *      In: cp     = Pekare till utdata.
 *          altptr = Pekare till utdata.
 *
 *      Ut: *altptr = Ev. menyalternativ-pekare.
 *          *cp     = Sist inmatade tecken
 *
 *      FV:
 *          SMBNONE    - okänd symbol
 *          SMBCHAR    - ett tecken
 *          SMBRETURN  - return
 *          SMBBACKSP  - backspace
 *          SMBLEFT    - åt vänster
 *          SMBRIGHT   - åt höger
 *          SMBUP      - uppåt
 *          SMBDOWN    - nedåt
 *          SMBESCAPE  - escape
 *          SMBMAIN    - huvudmenyn
 *          SMBHELP    - hjälp
 *          SMBPOSM    - positionsmenyn
 *          SMBALT     - menyalternativ
 *
 *
 *      (C)microform ab 16/10-85 Ulf Johansson
 *
 *      29/10-85 Bug, Ulf johansson
 *      8/10/86  SMBMAIN, J, Kjellander
 *      13/10/86 SMBHELP, J, Kjellander
 *      8/11/88  snabb, J. Kjellander
 *      15/11/88 SMBPOSM J. Kjellander
 *      15/7/92  X11, J. Kjellander
 *      1/11/95  WIN32, J. Kjellander
 *
 ******************************************************!*/

{
   short   i,j;
   char    tbuf[MAXTXT+1],cbuf[MAXTXT+1];
   short   symbol;
   char    c;
   bool    hit,exit;

/*
***Här börjar det hela.
*/     
   symbol = SMBNONE;
   i = 0;
   exit = FALSE;

   do
     {
/*
***Med X11/WIN32 använder vi XXgtch() som returnerar ett tecken eller
***en symbol/menyalternativ. Om tecken returneras gör vi som vanligt.
***Om menyval returneras avslutar vi och returnerar menyvalet.
***Om REJECT returneras avslutar vi och returnerar dito.
*/
#ifdef V3_X11
     if ( igtrty == X11 )
       {
       c = WPgtch(altptr,&symbol,TRUE);
       if      ( symbol == SMBCHAR )   goto tecken;
       else if ( symbol == SMBALT )    return(SMBALT);
       else if ( symbol == SMBRETURN ) return(SMBRETURN);
       else if ( symbol == SMBUP )     return(SMBUP);
       else if ( symbol == SMBMAIN )   return(SMBMAIN);
       }
#endif
#ifdef WIN32
     c = msgtch(altptr,&symbol,TRUE);
     if      ( symbol == SMBCHAR )   goto tecken;
     else if ( symbol == SMBALT )    return(SMBALT);
     else if ( symbol == SMBRETURN ) return(SMBRETURN);
     else if ( symbol == SMBUP )     return(SMBUP);
     else if ( symbol == SMBMAIN )   return(SMBMAIN);
#endif
   
tecken:
     cbuf[i++] = c;                            /* Buffra tecknet */
     if ( i >= MAXTXT ) break;
     cbuf[i] = '\0';                           /* null terminate */
/*
***Är det frågan om ett snabbval ?
*/
     hit = FALSE;
     for ( j=0; j < SMBMAX; j++ )
       {
       if ( smbind[j].str != NULL )
         {
         if ( strncmp(smbind[j].str,cbuf,i) == 0 )
           {
           hit = TRUE;
           break;
           }
         }
       }
/*
***Ja det är ett snabbval.
*/
     if ( hit )
       {
       if ( strlen(smbind[j].str) == i )
         {
/*
***Är det snabbval av typen ALT, dvs. CR, BS etc.
*/
         if ( smbind[j].acttyp == ALT )
           {
           symbol = smbind[j].actnum;

           switch (symbol)
             {
             case SMBRETURN:       /* return */
             case SMBBACKSP:       /* backspace */
             case SMBLEFT:         /* åt vänster */
             case SMBRIGHT:        /* åt höger */
             case SMBUP:           /* uppåt */
             case SMBDOWN:         /* nedåt */
             case SMBHELP:         /* hjälp */
             case SMBMAIN:         /* huvudmenyn */
             case SMBPOSM:         /* pos-menyn */
             break;

             default:
             symbol = SMBNONE;     /* okänd symbol */
             break;
             }
           }
/*
***Snabbval av typen funktionstangent, dvs. ESC-sekvens.
*/
         else
           {
           symbol = SMBESCAPE;
           strcpy(tbuf,igqema());
           if ( *tbuf != '\0' ) igrsma();
           strcpy(hstack[mant],iggtts(50));
           if ( igdofu(smbind[j].acttyp,smbind[j].actnum) == GOMAIN )
              symbol = SMBMAIN;
           if ( *tbuf != '\0' ) igplma(tbuf,IG_MESS);
           }
         exit = TRUE;
         }
       }
/*
***Nej ingen träff.
*/
     else
       {
       if ( i <= 1 )
         if ( (c >= ' ') && (c <= '~') ) symbol = SMBCHAR;
           exit = TRUE;
       }

     } while (!exit);

  *cp = c;
   return(symbol);
}

/*!******************************************************/
/*!******************************************************/

       short igglin(
       char  *pmt,
       char  *dstr,
       short *ntkn,
       char  *istr)

/*     Läser sträng från stdin med promt, defaultvärde, hjälp
 *     och editering men utan snabbval.
 *
 *     In: pmt   = Pekare till promttext.
 *         dstr  = Pekare till defaultsträng.
 *         ntkn  = Pekare till max antal tecken.
 *         istr  = Pekare till resultat.
 *
 *     Ut: *istr = Nollterminerad sträng.
 *         *ntkn = Antal tecken lästa.
 *
 *     FV: 0
 *
 *     (C)microform ab 7/11/86 J. Kjellander
 *
 *     28/4/87  Bug, J. Kjellander
 *     8/11/88  Anropar nu iglned(), J. Kjellander
 *     7/8/91   SMBMAIN, J. Kjellander
 *     27/1/92  status, J. Kjellander
 *     1996-03-08 WIN32, J.Kjelander
 *
 ******************************************************!*/

{
   int   typarr[1];
   char  is[MAXTXT+1],*isadr=is;
   char  ds[MAXTXT+1],*dsadr=ds;
   short symbol,status,cursor,scroll,pl;
   bool  dfuse;
   
/*
***Initiering.
*/
   typarr[0] = C_STR_VA;
   is[0] = '\0';
   strcpy(ds,dstr);
   strcpy(is,dstr);
   cursor = scroll = 0;
   dfuse = TRUE;
   if ( (pl=strlen(pmt)) > 0 ) ++pl;
/*
***Om det är X11 använder vi tills vidare XXmsip().
***Denna returnerar status typ REJECT och GOMAIN,
***ej symboler.
*/
#ifdef V3_X11
    if ( igtrty == X11 )
      {
      symbol = WPmsip(igqema(),&pmt,&dsadr,&isadr,ntkn,typarr,(short)1);
      if ( symbol == REJECT ) symbol = SMBUP;
      if ( symbol == GOMAIN ) symbol = SMBMAIN;
      goto slut;
      }
#endif
/*
***Med WIN32 använder vi igssip() som i sin tur använder
***msmsip() som använder den ordinarie WIN32-dialogboxen.
*/
#ifdef WIN32
    symbol = igssip(pmt,isadr,dsadr,*ntkn);
    if ( symbol == REJECT ) symbol = SMBUP;
    if ( symbol == GOMAIN ) symbol = SMBMAIN;
    goto slut;
#endif 

start:
slut:
   switch ( symbol )
     {
     case SMBHELP:
     ighelp();
     strcpy(ds,is);
     goto start;

     case SMBUP:
     *ntkn = 0;
     is[0] = '\0';
     status = REJECT;
     break;

     case SMBMAIN:
     *ntkn = 0;
     is[0] = '\0';
     status = GOMAIN;
     break;

     default:
     status = 0;
     break;
     }

   *ntkn = strlen(is);
   strcpy(istr,is);

   return(status);
}

/*!******************************************************/

