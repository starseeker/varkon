/*******************************************************************
*
*     dbid.c
*     ======
*
*     This file includes the following public routines:
*
*     DBget_pointer();     Return DBpointer and and type of entity
*     DBget_id();          Return ID of entity
*     DBget_highest_id();  Return highest used ID sequencenumber
*     DBget_free_id();     Return free ID sequence number
*
*    This file is part of the VARKON Database Library.
*    URL:  http://www.varkon.com
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

DBetype  lsttyp;    /* Sista storhets typ, används för att 
                       avgöra om föregående storhet var en part */
DBptr    lstprt;    /* LA för sista part */
DBptr    lstla;     /* LA för sista storhet */
DBptr    nxtins;    /* LA för nästa instans */
DBptr    lstidt;    /* LA för sista ID-tab */
DBseqnum lstid;     /* Sista ID */
DBseqnum lstidm;    /* Sista ID-tab:s  storlek */

/*!******************************************************/

        DBstatus DBget_pointer(
        char     fkod,
        DBId    *id,
        DBptr   *la,
        DBetype *ptyp)

/*      Returnerar LA och typ för angiven gmpost. Vilken
 *      post som avses ges av id och fkod. Med fkod = 'I'
 *      används id för att hitta posten. Med fkod = 'F'
 *      returneras la till den aktiva partens part-post.
 *      Om fkod = 'N' returneras nästa post. Med upprepade
 *      anrop av typen DBget_pointer(x,'N',x,x) traverseras hela
 *      gm sekvensiellt. Med fkod = 'G' fås samma effekt
 *      som med 'I' med den skillnaden att identiteten
 *      tolkas som global istället för lokal.
 *
 *      ID räknas alltid relativt den modul som för til-
 *      lfället är aktiv, dvs sökning börjar alltid i den
 *      ID-tabell som utpekas av actidt. Om ID är en
 *      global referens, dvs. om 1:a sekvensnummer är
 *      negativt räknas ID relativt huvudmodulens ID-
 *      tabell.
 *
 *      Kontroll sker att en post med angiven identitet
 *      verkligen finns i gm. Om id är länkad i flera
 *      nivåer måste alla poster på vägen fram till
 *      lägsta nivån existera och vara part-poster. Detta
 *      kontrolleras också     .
 *
 *      In:  fkod -> Funktions-kod, 'I','F' eller 'N'.
 *           id   -> Pekare till identitet-structure.
 *           la   -> Pekare till DBptr-variabel.
 *           ptyp -> Pekare till DBetype-variabel.
 *
 *      Ut: *la   -> Logisk adress till GM-post.
 *          *ptyp -> Typ av post, POITYP, LINTYP etc.
 *
 *      FV:  0  => Ok.
 *          -1  => Ett sekvensnummer pekar utanför
 *                 motsvarande part:s IDTAB.
 *          -2  => Post med angiven id finns ej i GM.
 *                 Alla ev part:s på vägen fram till
 *                 själva posten finns men postens
 *                 IDTAB - pekare är < 0.
 *          -3  => Någon av de parts som ingår i en länkad
 *                 identitet saknas. En IDTAB - pekare är
 *                 < 0 eller pekar på något som inte
 *                 är en part.
 *          -4  => Okänd funktionskod.
 *          -5  => Ett anrop med fkod='N' har gjorts
 *                 men det finns inga fler storheter
 *                 i gm.
 *          -6  => Instansen finns ej.
 *
 *       Felkoder: GM0092 = Storheten %s refereras i aktiv modul
 *                 GM0102 = Storheten %s refereras av %s
 *
 *      (C)microform ab 24/12/84 J. Kjellander
 *
 *      21/10/85 Ordningsnummer, J. Kjellander
 *      7/11/85  NOTUSD, J. Kjellander
 *      9/11/85  Bug instanser, J. Kjellander
 *      4/4/86   'G', J. Kjellander
 *      15/11/86 Instanser av part, J. Kjellander
 *      22/12/86 Globala referenser, J. Kjellander
 *      25/12/86 erpush, J. Kjellander
 *      30/4/87  'P', J. Kjellander
 *      15/11/88 'n', J. Kjellander
 *
 ******************************************************!*/
  {
    char     idstr[80],pidstr[80]; /* Felsträngar för erpush() */
    DBptr    la_tmp,idtadr,adress;
    DBseqnum seknr,idtsiz;
    DBordnum ordnr;
    DBstatus status;
    GMPRT    part;
    GMRECH   header;
    DBId    *link;
    DBId     pid[MXINIV];   /* Temporär för erpush() */

    switch(fkod)
    {
/*
***Angivet globala ID-skall användas för att hitta posten.
*/
    case 'G':
    idtadr = 0;             /* Pekare till huvud-partens IDTAB */
    idtsiz = huvidm+1;      /* Huvudpartens IDTAB:s storlek */
    goto lokal;
/*
***Angivet lokala ID-skall användas för att hitta posten.
*/
    case 'I':
    if ( id->seq_val < 0 )
      {
      idtadr = 0;             /* Pekare till huvud-partens IDTAB */
      idtsiz = huvidm+1;      /* Huvudpartens IDTAB:s storlek */
      }
    else
      {
      idtadr = actidt;         /* Pekare till aktiv IDTAB */
      idtsiz = actidm+1;       /* Aktuell IDTAB:s storlek */
      }

lokal:
    seknr = abs(id->seq_val);
    ordnr = id->ord_val;
    link = id->p_nextre;
/*
***Kolla att sekvensnummer ligger inom tillåtet intervall.
*/
  loop:
    if ( seknr >= idtsiz || seknr < 0 ) goto error1;
/*
***Läs motsvarande LA.
*/
    adress = (sizeof(DBptr)*seknr) + idtadr;
    rddat1((char *)&la_tmp,adress,sizeof(DBptr));
/*
***Om link = NULL avsluta.
*/
    if ( link == NULL )
      {
      if ( la_tmp < 0 ) goto error2;

      if ( (*la=gtlain(la_tmp,ordnr,ptyp)) < 0 ) goto error6;

      return(0);
      }
/*
***Annars fortsätt ner i nästa part.
*/
    else 
      {
      if ( la_tmp < 0 ) goto error3;
      if ( (la_tmp=gtlain(la_tmp,ordnr,ptyp)) < 0 ) goto error6;

      DBread_part(&part,la_tmp);
      if ( part.hed_pt.type != PRTTYP ) goto error3;

      idtadr = part.itp_pt;
      idtsiz = part.its_pt;
      seknr = link->seq_val;
      ordnr = link->ord_val;
      link = link->p_nextre;
      goto loop;
      }
    break;
/*
***Returnera la för huvudmodulens part-post.
*/
    case 'F':

    *ptyp = PRTTYP;
    *la = huvprt;
    lstprt = 0;         /* Uppdatera last-variablerna */
    lstid = 0;
    lstidm = 0;
    lstidt = 0;
    lsttyp = 0;         /* Fix för 'n' */
    nxtins = DBNULL;
    return(0);
    break;
/*
***Returnera nästa entry.
*/
    case 'N':
    case 'n':
    case 'P':

nextid:
    if ( lstid == lstidm && nxtins == DBNULL  &&  lsttyp != PRTTYP )
      {
/*
***Slutet av huvudpartens ID-tabell.
*/
      if ( lstprt == huvprt ) return(-5);
/*
***Början av huvudpartens ID-tabell.
*/
      else if ( lstprt == 0 )
        {
        lstprt = huvprt;
        lsttyp = 0;           /* Fix för 'n' */
        lstid = -1;
        lstidm = huvidm;
        lstidt = 0;
        nxtins = DBNULL;
        goto nextid;
        }
/*
***Slutet av en anropad parts ID-tabell.
*/
      else if ( fkod == 'n' ) return(-5);

      else
        {
        DBread_part( &part, lstprt);      /* Läs nuvarande part-post */
        lstprt = part.hed_pt.p_ptr;  /* Återvänd till föreg. part */
        lstid = part.hed_pt.seknr;   /* Sista id */
        nxtins = part.hed_pt.n_ptr;  /* Nästa instans */
        lsttyp = 0;                  /* Fix för 'n' */

        if ( lstprt == huvprt) 
          {
          lstidm = huvidm;           /* Föreg. part == huvudpart */
          lstidt = 0;
          }
        else 
          {
          DBread_part( &part, lstprt);    /* Föreg. part != huvudpart */
          lstidm = part.its_pt-1;
          lstidt = part.itp_pt;
          }
        goto nextid;
        }
      }
/*
***Nästa instans eller ID i tabellen.
***Om föregående storhet var en part skall vi
***normalt gå ned i den. Dock ej om fkod='n'.
*/
    else
      {
      if ( lsttyp == PRTTYP )
        {
        lsttyp = 0;
        if ( fkod != 'n' )
          {
          lstprt = lstla;
          lstid = -1;
          DBread_part( &part, lstla);
          lstidm = part.its_pt-1;
          lstidt = part.itp_pt;
          nxtins = DBNULL;
          }
        goto nextid;
        }
/*
***Nästa storhet var en instans av föregående.
*/
      if ( nxtins != DBNULL )
        {
        DBread_header(&header,nxtins);
        *la = lstla = nxtins;
        *ptyp = lsttyp = header.type;
        nxtins = header.n_ptr;
        }
/*
***Nästa storhet var en ny storhet.
*/
      else
        {
        ++lstid;
        adress = sizeof(DBptr)*lstid + lstidt;
        rddat1((char *)&la_tmp,adress,sizeof(DBptr));
        if ( (*la=la_tmp) < 0 )
          {
          lsttyp = 0;        /* Fix för 'n' */
          goto nextid;
          }
        DBread_header( &header, *la );
        lstla = *la;
        *ptyp = lsttyp = header.type;
        nxtins = header.n_ptr;
        }
/*
***Om funktionskod 'P' och part, nästa ID annars slut.
*/
      if ( fkod == 'P' && header.type != PRTTYP ) goto nextid;
      else return(0);
      }

    break;
    }
/*
***Okänd funktionskod.
*/
    return(-4);
/*
***Felutgångar.
*/
error1:
    status = -1;
    goto notfnd;

error2:
    status = -2;
    goto notfnd;

error3:
    status = -3;
    goto notfnd;

error6:
    status = -6;
    goto notfnd;
/*
***Storheten finns ej.
*/
notfnd:
    gmidst(id,idstr);     /* Storhetens ID */

    if ( actprt == huvprt )
      {
      erpush("GM0092",idstr);
      }
    else
      {
      DBget_id(actprt,pid);   /* Anropande parts ID */
      gmidst(pid,pidstr);
      strcat(pidstr,"%");
      strcat(pidstr,idstr);
      erpush("GM0102",pidstr);
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBget_id(
        DBptr    la,
        DBId    *idpek)

/*      Returnerar global identitet för storhet
 *      vid LA.
 *
 *      OBS!!! Om det som finns vid la inte är en
 *             GM-post kraschar systemet.
 *
 *      In: la     => Logisk adress.
 *          idpek  => Pekare till en array av
 *                    identitet-structures. Arrayen
 *                    består av minst MXINIV element.
 *
 *      Ut: *idpek => En länkad lista av identitet-
 *                    structures med sista pekaren=0.
 *                    1:a sekvensnummret negativt.
 *
 *      FV:     0  => Ok.
 *
 *      (C)microform ab 5/1-85 J. Kjellander
 *
 *       22/10/85 Ordningsnummer, J, Kjellander
 *       23/12/86 Globala ref, J. Kjellander
 *
 ******************************************************!*/

  {
     DBId    *tidpek;
     GMRECH   header;
     DBptr    la_tmp;
     DBseqnum idvek[MXINIV];
     DBordnum onvek[MXINIV];
     DBint    nivant,i;

/*
***Lite initiering.
*/
     la_tmp = la;
     nivant = 0;
/*
***Länka ihop listan.
*/
     for ( i=0; i<MXINIV-1; ++i ) (idpek+i)->p_nextre = &idpek[i+1];
     (idpek+MXINIV-1)->p_nextre = NULL;
/*
***Läs postens header.
*/
nxtniv:
     DBread_header(&header,la_tmp);
/*
***Ta reda på lokal identitet för denna nivå.
*/
     idvek[nivant] = header.seknr;
     onvek[nivant] = header.ordnr;
/*
***Om header.p_ptr = huvprt har vi kommit till högsta nivån.
***Avsluta genom att kopiera idvek baklänges till idpek.
*/
     if ( header.p_ptr == huvprt )
       {
       tidpek = idpek;
       while ( nivant > 0 )
         {
         tidpek->seq_val = idvek[nivant];
         tidpek->ord_val = onvek[nivant--];
         tidpek = tidpek->p_nextre;
         }
       tidpek->seq_val = idvek[0];
       tidpek->ord_val = onvek[0];
       tidpek->p_nextre = NULL;       
       idpek->seq_val = -(idpek->seq_val); /* Returnera global ref */
       return(0);
       }
/*
***Om inte fortsätt ett varv till.
*/
     else
       {
       ++nivant;
       la_tmp = header.p_ptr;
       goto nxtniv;
       }
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBget_highest_id(DBseqnum *idmax)

/*      Returnerar huvudmodulens största id-nummer.
 *      Om inga storheter finns i GM returneras 
 *      idmax = -1.
 *
 *      In: idmax  -> Pekare till resultat.
 *
 *      Ut: *idmax -> Största identitet.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 21/5/85 J. Kjellander
 *
 ******************************************************!*/

  {
    *idmax = huvidm;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  DBget_free_id(DBseqnum *id)

/*      Returns a free id sequence number that can
 *      be used to insert a new entity.
 *
 *      In: id  -> Pointer to output.
 *
 *      Out: *id -> A free sequencenumber.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( huvidm == -1 ) *id = 1;
    else                *id = huvidm + 1;

    return(0);
  }

/********************************************************/
