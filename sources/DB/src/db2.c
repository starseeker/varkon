/**********************************************************************
*
*    db2.c
*    =====
*
*    This file includes the following internal functions:
*
*    pfault();    Serves pagefault
*    insid();     Creates a new ID
*    gmrdid();    Reads la from active ID table
*    wrid();      Writes la into active ID table
*    inpost();    Inserts a new entity
*    gmgtmd();    Returns metadata
*    gmvers();    Returns version number.
*
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

#include "../include/DB.h"
#include "../include/DBintern.h"

DBshort cirpek;    /* Cirkulär pekare, initieras till cirdef i gmclr() */
DBshort cirdef;    /* cirpek:s startvärde */
DBint   pfcnt=0;   /* Page-fault statistik */
DBint   wrcnt=0;
DBint   rdcnt=0;

/*!*****************************************************/

        DBptr pfault(DBpagnum pagnum)

/*      Servar pagefault. Om logtab[i].ptr för sida nummer
 *      i är < 0 finns inte sidan i primärminne, dvs
 *      i gmbuf. Om logtab[i].ptr = PGFPNA har sidan aldrig
 *      refererats. pafult allokerar då sidan genom att
 *      ge den en sid-adress i gmbuf. Om sidan
 *      redan har allokerats är logtab[i].ptr = sidans
 *      adress i pagefilen och det är bara att läsa
 *      in den.
 *
 *      In: pagnum => Sidnummer för begärd sida.
 *
 *      Ut: Inget.
 *
 *      FV: pagadr, dvs adressen till den sida i gmbuf
 *                  som sidan pagnum mappats till.
 *
 *      (C)microform ab 8/12/84 J. Kjellander
 *
 *      22/10/85 Second chance, J. Kjellander
 *      18/11/92 Snabbare algoritm, J. Kjellander
 *
 ******************************************************!*/

  {
    DBpagnum i;
    DBptr    pagadr;

/*
***Lite statistik.
*/
    ++pfcnt;
/*
***Börja med att fixa en ledig sida i gmbuf. Förhop-
***pningsvis finns det någon som ännu ej allokerats.
***Sökningen börjar på sidan fystal. Lediga sidor med
***lägre sidnummer än fystal finns ej. gmclr() nollställer
***fystal och rldat1() sätter fystal till den släppta sidan
***om den är lägre än fystal. Vitsen är att vid kör aktiv modul
***skall sidor snabbt allokeras genom att fystal räknas upp
***ett snäpp i taget och sökning i fystab undviks. Samtidigt
***skall släppta sidor (rldat1) återanvändas på ett effektivt
***sätt vid tex. ändra part.
*/
    while ( fystal < fystsz  &&  fystab[fystal].pagnum != GMBPNA ) ++ fystal;

    if ( fystal < fystsz ) i = fystal;
/*
***Om alla sidor i gmbuf är allokerade måste vi välja ut en
***av dom att släppa. Snabbaste metoden är att låta merparten
***av gmbuf ligga orörd och att använda en liten del för paging.
***Används hela för paging kommer varje ZOOM etc. att kräva att
***hela PF läses. Om tex. bara 10 sidor används för paging kommer
***i vart fall resten att alltid finnas i gmbuf. På så vis utnyttjar
***man gmbuf bättre. cirdef avgör hur stor del som skall paga:s.
*/
    else
      {
      do
        {
        fystab[cirpek].reflg = FALSE;
        if ( ++cirpek == fystsz ) cirpek = cirdef;
        }
      while ( fystab[cirpek].reflg == TRUE );

      i = cirpek;
/*
***Sidan i skall släppas. Om den är modifierad måste den skrivas
***ut till PF.
*/
      if ( fystab[i].wrflg == TRUE )
        {
/*
***Om den inte har någon PF-sida allokerad, dvs. om detta är
***första gången den skall skrivas ut måste vi allokera plats
***i PF innan vi skriver.
*/
        if ( fystab[i].pfpadr == PGFPNA )
          {
          fystab[i].pfpadr = pfsiz;
          fseek( gmpfpk,(long)fystab[i].pfpadr, 0);
          fwrite( &gmbuf[PAGSIZ*i], PAGSIZ, 1, gmpfpk);
          pfsiz += PAGSIZ;
          }
/*
***gmbuf-sidan hade redan en giltig PF-adress. Då kan vi skriva ut
***den direkt utan att allokera mer plats i PF.
*/
        else
          {
          fseek( gmpfpk,(long)fystab[i].pfpadr, 0);
          fwrite( &gmbuf[PAGSIZ*i], PAGSIZ, 1, gmpfpk);
          }
        fystab[i].wrflg = FALSE;
        ++wrcnt;
        }
/*
***Sidan i var aldrig modifierad eller har nu skrivits ut. Då måste
***den ha en giltig PF-adress och kan snabbt och enkelt släppas.
***Den kan också vara suddad !  Då är logtab.all = FALSE.
*/
        logtab[fystab[i].pagnum].ptr = -fystab[i].pfpadr;
      }
/*
***En ledig sida med sidnummer=i finns nu i gmbuf. Då finns det
***två möjligheter. Antingen är den begärda sidan pagnum en helt
***ny-allokerad sida eller också är det en sida som skall läsas
***från page-filen.
*/
    if ( logtab[pagnum].ptr == PGFPNA )
      {
/*
***Sidan pagnum är ny-allokerad.
*/
      pagadr = PAGSIZ*i;              /* Beräkna sidadress */
      logtab[pagnum].ptr = pagadr;    /* Skriv gmbuf-C-offset i logtab */
      fystab[i].pagnum = pagnum;      /* Skriv sidnummer i fystab */
      fystab[i].pfpadr = PGFPNA;      /* Ingen PF-sida ännu */
      }
/*
***Sidan som begärts är allokerad, dvs finns i pagefilen.
***Kopiera in den från pagefilen till gmbuf.
*/
    else
      {
      pagadr = PAGSIZ*i;                       /* Beräkna sidadress */
      fystab[i].pfpadr = -logtab[pagnum].ptr;  /* Adress i pagefilen */
      fseek(gmpfpk,(long)fystab[i].pfpadr,0);  /* Positionera filpekaren */
      fread(&gmbuf[pagadr],PAGSIZ,1,gmpfpk);   /* Läs */
      logtab[pagnum].ptr = pagadr;             /* Sidadress i logtab */
      fystab[i].pagnum = pagnum;               /* Sidnummer i fystab */
      ++rdcnt;
      }
    
    return(pagadr);       /* Returnera sidadress */
  }

/********************************************************/
/*!******************************************************/

        DBstatus insid( DBId *id, DBptr la)

/*      Skapar ett nytt ID. Kontrollerar att plats finns
 *      för det angivna ID:t i IDTAB och lagrar LA på
 *      rätt plats. Om ID är större än huvidm+1 så att
 *      ett hål uppstår i IDTAB fylls mellanliggande
 *      ID:n med värdet NOTUSD. Alla ID i IDTAB har
 *      alltså antingen ett vettigt värde = LA eller
 *      också ett ovettigt värde = NOTUSD (eller ERASED).
 *
 *      Om actprt != huvprt, dvs om en anropad modul är
 *      under exekvering skall inte ID lagras i
 *      huvudmodulens IDTAB utan i den som utpekas
 *      av actidt.
 *
 *      In: *id  => Pekare till Identitet-structure.
 *           la  => Logisk adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok,
 *          -2  => Platsbrist
 *
 *      (C)microform ab 6/11/84 J. Kjellander
 *
 *      7/11/85  ERASED, J. Kjellander
 *      11/10/85 IBMATX, J. Kjellander
 *
 ******************************************************!*/

    {
    DBpagnum pagnum;
    DBseqnum i,j;

/*
***Kolla vilken IDTAB som ID:t skall in i.
*/
    i = id->seq_val;
    if ( actprt == huvprt )
      {
/*
***Huvudmodulen är aktiv, allokera plats i huvudmodulens
***IDTAB. Beräkna ID:ts sidnummer och kolla om IDTAB behöver
***expanderas. Isåfall får de sidor som tillförs IDATB
***inte vara allokerade sedan tidigare. Däremot skall
***dom allokeras nu.
*/
      pagnum = sizeof(DBptr)*(DBint)i/PAGSIZ;
      while ( pagnum > ipgnum )
        {
        if ( logtab[++ipgnum].all ) return(-2);
        logtab[ipgnum].all = TRUE;
        }
/*
***Är ID > huvidm+1 ? Isåfall sätt mellanliggande ID=NOTUSD
***och lagra.
*/
      while ( i > huvidm+1 )
        {
        j = ++huvidm;
        wrid(j,(DBptr)NOTUSD);
        }
      wrid(i,la);
      if ( i > huvidm )
        {
        huvidm = i;           /* Max id i huvudmodulen */
        actidm = huvidm;      /* Huvudmodulen är aktiv */
        }
      }
/*
***Huvudmodulen är inte aktiv, ingen allokering behövs.
***Skriv in direkt i den aktiva modulens IDTAB.
*/
    else
      wrid(i,la);

    return(0);
    }

/********************************************************/
/*!******************************************************/

        DBptr gmrdid(DBptr idtab, DBseqnum id)

/*      Läser la ur ID-tabell som utpekas av idtab.
 *
 *      In:  idtab => LA för ID-tabell
 *           id    => ID, dvs. storhetens sekvensnummer.
 *                    Lokal eller Global.
 *
 *      Ut: Inget.
 *
 *      FV: Den lästa gm-pekaren.
 *
 *      (C)microform ab 14/10/85 J. Kjellander
 *
 *      21/10/85 reflg, J. Kjellander
 *      1/2/86   idtab, J. Kjellander
 *      11/10/86 IBMATX, J. Kjellander
 *      23/12/86 Globala ref, J. Kjellander
 *      23/9/87  Tagit bort PEKARE, J. Kjellander
 *
 ******************************************************!*/

    {
    DBptr    adress,pagadr,gmbadr,val,offset;
    DBpagnum pagnum;
    char    *valpek;
/*
***Beräkna ID:ts adress, sidnummer och offset.
*/
    adress = idtab + sizeof(DBptr)*abs(id);
    pagnum = adress/PAGSIZ;
    offset = adress - PAGSIZ*pagnum;
/*
***Läs värdet.
*/
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    fystab[pagadr/PAGSIZ].reflg = TRUE;  /* Sätt referens-flaggan */

    gmbadr = pagadr+offset;              /* Adress i gmbuf */
    valpek = (char *)&val;

    *(valpek++) = gmbuf[gmbadr++];
    *(valpek++) = gmbuf[gmbadr++];
    *(valpek++) = gmbuf[gmbadr++];
    *valpek     = gmbuf[gmbadr];

    return(val);
    }

/********************************************************/
/*!******************************************************/

        DBstatus wrid(DBseqnum id, DBptr val)

/*      Skriver gm-pekaren val på platsen id i aktuell
 *      ID-tabell. Inga felkontroller görs.
 *
 *      In:  id   => ID.
 *           val  => Värde.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 16/12/84 J. Kjellander
 *
 *      21/10/85 reflg, J. Kjellander
 *      23/9/87  Tagit bort PEKARE, J. Kjellander
 *
 ******************************************************!*/

    {
    DBptr    adress,pagadr,gmbadr,offset;
    DBpagnum pagnum;
    char    *valpek;
/*
***Beräkna ID:ts adress, sidnummer och offset.
*/
    adress=actidt + sizeof(DBptr)*id;
    pagnum=adress/PAGSIZ;
    offset=adress - PAGSIZ*pagnum;
/*
***Skriv in värdet.
*/
    if ( (pagadr=logtab[pagnum].ptr) < 0 ) pagadr=pfault(pagnum);

    fystab[pagadr/PAGSIZ].wrflg = TRUE;  /* Sätt skriv-flaggan */
    fystab[pagadr/PAGSIZ].reflg = TRUE;  /* Sätt referens-flaggan */

    gmbadr = pagadr + offset;            /* Adress i gmbuf */
    valpek = (char *)&val;

    gmbuf[gmbadr++] = *(valpek++);
    gmbuf[gmbadr++] = *(valpek++);
    gmbuf[gmbadr++] = *(valpek++);
    gmbuf[gmbadr]   = *valpek;

    return(0);
    }

/********************************************************/
/*!******************************************************/

        DBstatus inpost(GMUNON *gmpost, DBId *idpek,
                        DBptr *lapek, DBint size)

/*      Allokerar plats för ID och data i den aktiva
 *      parten. Uppdaterar partens ID-tabell och lagrar
 *      data på allokerad plats.
 *
 *      In: gmpost => Pekare till en GM-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *          size   => sizeof(GM-structure).
 *
 *      Ut: *la     => Logisk adress till post i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 22/12/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/10/85 Ordningsnummer, J. Kjellander
 *      7/11/85  ERASED, J. Kjellander
 *      16/3/92  refcnt bort, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la_next;
    GMRECH  oldhed;

/*
***Initiera headerdata.
*/
    gmpost->hed_un.p_ptr = actprt;
    gmpost->hed_un.seknr = idpek->seq_val;
    gmpost->hed_un.g_ptr[0] = DBNULL;
    gmpost->hed_un.g_ptr[1] = DBNULL;
    gmpost->hed_un.g_ptr[2] = DBNULL;
/*
***Kolla att ID har tillåtet värde, dvs är
***större än eller lika med noll. Om en anropad modul är
***under exekvering får ID inte vara större än actidm.
*/
      if ( idpek->seq_val < 0 ) return(-2);
      if ( actprt != huvprt && idpek->seq_val > actidm ) return(-2);
/*
***Kolla om detta är 1:a eller n:te instansen.
*/
    if ( idpek->seq_val > actidm || 
                    (la_next=gmrdid(actidt,idpek->seq_val)) < 0 )
      {
/*
***1:a instansen. Lagra posten och skriv in la i IDTAB.
*/
      gmpost->hed_un.ordnr = 1;
      gmpost->hed_un.n_ptr = DBNULL;
      if ( wrdat1((char *)gmpost,lapek,size) < 0 ) return(-3);
      return(insid(idpek, *lapek));
      }
/*
***n:te instansen.
*/
    else
      {
      DBread_header(&oldhed,la_next);
      gmpost->hed_un.ordnr = oldhed.ordnr + 1;
      gmpost->hed_un.n_ptr = la_next;
      if ( wrdat1((char *)gmpost,lapek,size) < 0 ) return(-3);
      wrid(idpek->seq_val,*lapek);
      return(0);
      }
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmgtmd(GMMDAT *pmd)

/*      Returnerar GM-metadata.
 *
 *      In:  pmd  -> Pekare till metadata-struktur.
 *
 *      Ut: *pmd  -> Struktur med aktuella metadata
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 26/8/85 J. Kjellander
 *
 *      921116 Debug, J. Kjellander
 *
 ******************************************************!*/

  {
    DBint i,logusd,logdel,fysusd;

/*
***Beräkna antal använda och suddade sidor.
*/
    logusd = logdel = 0;
    for ( i=0; i<metdat.logmax; ++i )
      {
      if ( logtab[i].all ) ++logusd;
      else if ( logtab[i].ptr != PGFPNA ) ++logdel;
      }

    fysusd = 0;
    for ( i=0; i<fystsz; ++i )
      if ( fystab[i].pagnum != GMBPNA ) ++fysusd;
/*
***Fyll i strukturen.
*/
    pmd->pagsiz = PAGSIZ;
    pmd->logmax = metdat.logmax;
    pmd->logusd = logusd;
    pmd->fysmax = fystsz;
    pmd->fysusd = fysusd;
    pmd->pfsiz = pfsiz;
    pmd->pfcnt = pfcnt;
    pmd->wrcnt = wrcnt;
    pmd->rdcnt = rdcnt;
    pmd->ipgnum = ipgnum;
    pmd->dpgnum = dpgnum;
    pmd->datofs = datofs;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBint gmvers(GMRECH *ptr)

/*      Returns the versionnumber of a DB entity. Used
 *      by the GMVERS() macro to read direct from DB
 *      memory. 
 *
 *      In:  ptr  -> Pekare till GM-post.
 *
 *      Ut:  Inget.
 *
 *      FV:  Headerns versionsnummer.
 *
 *      (C)microform ab 7/1/93 J. Kjellander
 *
 *      1999-04-08 CRAY, J.Kjellander
 *
 ******************************************************!*/

  {
    DBshort vers;
    char   *pvers = (char *)&vers;
    char   *phed;
    DBint   offs;
    GMRECH  hed;

    offs = (char *)&hed.vers - (char *)&hed;
    phed = (char *)ptr + offs;

    *pvers = *phed;
   ++pvers;
   ++phed;
    *pvers = *phed;
#ifdef _CRAYT3E
   ++pvers;
   ++phed;
    *pvers = *phed;
   ++pvers;
   ++phed;
    *pvers = *phed;
#endif

    return((DBint)vers);
  }

/********************************************************/
