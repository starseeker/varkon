/*!******************************************************************/
/*  File: igID.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   igstid();   String to ID                                       */
/*   igidst();   ID to string                                       */
/*   igcsid();   Compare 2 ID's                                     */
/*   igcmid();   Compare one ID with many                           */
/*   igcpre();   Copy ID                                            */
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
#include <string.h>
#include <ctype.h>

/*!******************************************************/

        short igstid(
        char *idstr,
        DBId *idvek)

/*      Konvertera en sträng av ASCII-tecken till en ID.
 *
 *      In: idstr => Pekare till char-array.
 *          idvek => Pekare till lista av id-structures.
 *
 *      Ut: *idvek => Länkad lista av DBId   .
 *
 *      FV:        0 => Ok.
 *                -1 => Syntaxfel.
 *
 *      (C)microform ab 6/8/85 J. Kjellander
 *
 *      9/11/85  Ordningsnummer, J. Kjellander
 *      25/11/85 Bytt till punkt, J. Kjellander
 *      22/12/86 Globala ref, J. Kjellander
 *      23/10/89 386-Xenix, EOF i sscanf(), J. Kjellander
 *
 ******************************************************!*/

  {
    short  i,niv,n;
    short  snr,onr;
    char   c;
    bool   global;

/*
***Lite initiering.
*/
    i = 0;
    niv = 0;
    global = FALSE;
/*
***Lokal eller global?
*/
    while ( isspace(idstr[i]) != 0 ) ++i;  /* Skippa blanka */

    if ( strncmp(&idstr[i],"##",2) == 0 )
      {
      global = TRUE;
      ++i;
      }
/*
***Beta av resten.
*/
loop:
    while ( isspace(idstr[i]) != 0 ) ++i;  /* Skippa blanka */
    n = sscanf( &idstr[i], "%c", &c);      /* Läs "#" */
    if ( n == EOF && niv > 0 ) goto end;
/*
***I samband med övergång till 386-Xenix fick denna rad läggas till
*/
    if ( n == 0   && niv > 0 ) goto end;
/*
***för att slut på strängen skulle upptäckas. sscanf()
***returnerar inte EOF förrän vid nästa läsning. Kan detta
***vara standard???
*/
    if ( n == EOF && niv == 0 ) return(-1);
    if ( c != '#' ) return(-1);

    ++i;
    n = sscanf( &idstr[i], "%hd", &snr);   /* Läs sekv.nr. */
    if ( n == 0 || n == EOF ) return(-1);
    if ( snr < 0 ) return(-1);
    while ( isdigit(idstr[i]) != 0 ) ++i;  /* Skippa siffror */

    n = sscanf( &idstr[i], "%c", &c);      /* Läs ev. "." */
    if ( n != EOF && c == '.' )
      {
      ++i;
      n = sscanf( &idstr[i], "%hd", &onr);  /* Läs ordn.nr. */
      if ( n == 0 || n == EOF ) return(-1);
      if ( onr < 1 ) return(-1);
      while ( isdigit(idstr[i]) != 0 ) ++i;  /* Skippa siffror */
      }
    else
      {
      onr = 1;
      }

    idvek[niv].seq_val = snr;
    idvek[niv].ord_val = onr;
    idvek[niv].p_nextre = &idvek[niv+1];
    ++niv;

    goto loop;

end:
    idvek[niv-1].p_nextre = NULL;
    if ( global ) idvek[0].seq_val = -(idvek[0].seq_val);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igidst(
        DBId *idvek,
        char *idstr)

/*      Konverterar en identitet till sträng av ASCII-
 *      tecken.
 *
 *      In: idvek => Pekare till lista av id-structures.
 *          idstr => Pekare till char-array.
 *
 *      Ut: *idstr => Sträng, null-terminated.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 28/1/85 J. Kjellander
 *
 *      16/10/85 Ordningsnummer, J. Kjellandser
 *      25/11/85 Bytt till punkt, J. Kjellander
 *      22/12/86 Globala ref, J. Kjellander
 *
 ******************************************************!*/

  {
    char     tmp[80];
    DBId    *idptr;

    idstr[0] = '\0';
    idptr = idvek;

    if ( idptr->seq_val < 0 ) strcat(idstr,"#");

    do
      {
      sprintf(tmp,"#%d.%d",abs(idptr->seq_val),idptr->ord_val);
      strcat(idstr,tmp);
      }
    while ( (idptr=idptr->p_nextre) != NULL );

    return(0);
  }

/********************************************************/
/*******************************************************!*/

        bool igcsid(
        DBId *pid1,
        DBId *pid2)

/*      Jämför två enstaka identiteter.
 *
 *      In: pid1  => Pekare till storhet1:s identitet.
 *          pid2  => Pekare till storhet2:s identitet.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  => pid1 =  pid2.
 *          FALSE => pid1 != pid2.
 *
 *      (C)microform ab 22/8/85 R. Svedin.
 *
 ******************************************************!*/

  {
     DBId     *id1pek;
     DBId     *id2pek;

/*
***Jämför sekvensnummer och ordningsnummer.
*/
     if ( pid1->seq_val  != pid2->seq_val )  return(FALSE);
     else if ( pid1->ord_val  != pid2->ord_val )  return(FALSE);
/*
***Sekvensnr. och ordningsnr. är lika. Kolla om båda har vidarepekare.
*/
     if ( pid1->p_nextre != NULL && pid2->p_nextre != NULL )
       {
       id1pek = pid1->p_nextre;
       id2pek = pid2->p_nextre;
       return( igcsid(id1pek, id2pek));
       }
/*
***En eller båda har pekare = NULL. Kolla om båda har det.
*/
     else if ( pid1->p_nextre == NULL && 
               pid2->p_nextre == NULL ) return(TRUE);
/*
***Bara den ena hade pekare = NULL.
*/
     else return(FALSE);

  }

/********************************************************/
/******************************************************!*/

        bool igcmid(
        DBId *idpek,
        DBId  idvek[][MXINIV],
        short vn)

/*      Jämför en identitet med ett antal andra identiteter.
 *
 *      In: idpek => Pekare till storhet1:s identitet.
 *          idvek => Pekare till en vektor av identiteter.
 *          vn    => Antal element i vektorn.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  => idpek finns redan med i idvek.
 *          FALSE => idpek finns ej med i idvek.
 *
 *      (C)microform ab 23/8/85 R. Svedin.
 * 
 *      16/3/88  Omgjord för getmid(), J. Kjellander
 *
 ******************************************************!*/

  {
     short i;

/*
***Jämför
*/
     for ( i = 0; i < vn; ++i )
       {
       if ( igcsid(idpek,&idvek[i][0]))
         {
         return(TRUE);       /* Storheten finns i vektorn */
         }
       }
      return(FALSE);         /* Storheten finns ej med i vektorn */
  }

/*********************************************************/
/*!******************************************************/

        short igcpre(
        DBId *frompk,
        DBId *topk)

/*      Kopierar en referens.
 *
 *      In: frompk = Pekare till gammal referens.
 *          topk   = Pekare till ny referens (array).
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 2/6/86 J. Kjellander
 *
 ******************************************************!*/

  {
    short i;

    for ( i=0;; ++i )
      {
      (topk+i)->seq_val = (frompk+i)->seq_val;
      (topk+i)->ord_val = (frompk+i)->ord_val;
      if ( (frompk+i)->p_nextre == NULL )
        {
        (topk+i)->p_nextre = NULL;
        break;
        }
      else
        {
        (topk+i)->p_nextre = &topk[i+1];
        }
      }

    return(0);
  }
  
/********************************************************/
