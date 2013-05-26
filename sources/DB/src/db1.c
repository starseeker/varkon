/*!******************************************************
*
*    db1.c
*    ====
*
*    This file includes the following internal functions:
*
*    DBreset();   Resets DB
*    gmsvpf();    Writes pagefile to disc
*    gmldpf();    Loads pagefile from disc
*    gmclr();     Clears DB
*    gmgrow();    Increases DB virtual size
*    gmcrpf();    Creates new pagefile
*    gmclpf();    Closes pagefile
*    gmidst();    DBId to string conversion
*
*
*    This file is part of the VARKON Database Library.
*    URL:  http://varkon.sourceforge.net
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
*********************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

char *gmbuf = NULL;

/*   gmbuf är en character-array som utgör gm:s fysiska
     minne. Sidor i gm skyfflas mellan gmbuf och page-
     filen vid pagefault. fystal håller reda på antalet
     allokerade sidor. Minne för gmbuf allokeras dynamiskt
     vid uppstart. */


FILE *gmpfpk = NULL;

/*   gmpfpk är filpekaren till page-filen. gmpfpk initieras
     till NULL vilket betyder att filen ej är öppen. Detta
     testas vid skrivning och läsning. gmpfpk sätts också
     till NULL av gmclpf() */

DBint pfsiz;

/*   pfsiz är storleken på pagefilen. När en ny sida
     allokeras placeras den sist i pagefilen. 1:a
     biten av filen reserveras för pfsiz själv var-
     för pfsiz initieras till sizeof(pfsiz) alt. ftell(). */

LTBSTR *logtab = NULL;

/*   logtab är en map-tabell mellan logiskt sidnummer och
     fysiskt. Varje logisk sida motsvaras av ett element i
     logtab. logtab[i].ptr är en pekare till en mot-
     svarande fysisk sida. Om pekaren > 0 finns sidan på
     motsvarande adress i gmbuf. Om pekaren < 0 finns sidan i
     page-filen. Observera att pekarna i logtab inte är
     vanliga C-pekare utan heltal som pekar på enskilda
     element i gmbuf. Om pekaren = NOPGFA är sidan inte
     allokerad, dvs finns ej i gmbuf och ej heller i page-
     filen. Om logtab[i].all är TRUE är sidan upptagen av
     data eller IDTAB annars är den ledig. Fr.o.m. V1.8E
     allokeras minne för logtab dynamiskt. logtab = NULL
     innebär att inget minne ännu har allokerats.  */


FTBSTR  *fystab = NULL;
DBpagnum fystsz,fystal;

/*   fystab är en array av structures. Varje structure
     innehåller information om motsvarande sida i gmbuf,
     dvs. det fysiska minnet. Mot varje sida i det fysiska
     minnet (gmbuf) svarar ett element i FYSTAB. pagnum
     är sidnumret för den sida som ligger på motsvarande
     plats i gmbuf. pfpadr är adressen till den sida i
     page-filen där sidan hör hemma. wrflg är TRUE om
     skrivning skett på sidan, annars FALSE. reflg är
     TRUE om sidan refererats, annars FALSE. fystsz håller
     reda på antal element i fystab och därmed storleken
     på gmbuf. fystal håller reda på hur många som är
     allokerade. */


DBpagnum dpgnum;
DBshort  datofs;

/*   dpgnum är sidnumret för den sida där data sist lagrats.
     datofs är offset inom dpgnum. datofs är initierad till
     PAGSIZ så att första anropet till alldat() säkert skall
     medföra att en ny sida allokeras. dpgnum är initierad
     till slutet av LOGTAB mest för ordnings skull. */


DBpagnum ipgnum;

/*   ipgnum är sidnumret för huvidm, dvs huvudmodulens
     id-tabell:s storlek i sidor. ipgnum initieras till
      -1 för att sidor säkert skall allokeras för 1:a id
     som lagras. */


DBseqnum huvidm;
DBptr    huvprt;

/*   huvidm är huvudmodulens största id. huvidm initieras
     till -1 så att första storhet som lagras säkert upp-
     daterar huvidm,ipgnum och annat på rätt sätt. huvprt
     är en pekare till huvudmodulens part-post. */


DBptr    actidt;
DBseqnum actidm;
DBptr    actprt;

/*    actidt är en gm-pekare till början av den aktiva ID-
      tabellen. actidt initieras till 0 där ID-tabellen
      för huvud-modulen alltid skall finnas. När modul B
      anropas av modul A, via part-instruktionen, skapas
      en ny part-post i GM och minne för modul B:s ID-
      tabell allokeras. Därefter sätts actidt att peka på
      den nya ID-tabellen. När modul B är färdigkörd och
      exekveringen återvänder till modul A stängs parten B
      av interpretatorn genom ett anrop till DBclose_part(). Därvid
      återställs actidt att peka på ID-tabellen för modul A
      igen. actidm är huvidm för den aktiva modulen. 
      actprt är en gm-pekare till den aktiva partens part-
      post. */


DBptr    templa = DBNULL;
DBseqnum tempsn = 0;

/*    templa är la till senaste temporära storhet. templa =
      DBNULL innebär att ingen sådan finns. tempsn är storhetens
      serienummer. Variablerna nollställs av gmclr() och
      utnyttjas av gmmtm(), gmumtm() och gmrltm(). */


DBint    nkeys = 0;
KEYDATA *keytab = NULL;

/*    keytab is a pointer to the current table of data
      saved by key. nkeys is the number of entries in keytab
      including not used entries. */


V3MDAT gmsdat_org;

V3MDAT gmsdat_db = { 0,              /* Not used by DB */
                     DB_LIBVERSION,  /* Version */
                     DB_LIBREVISION, /* Revision */
                     DB_LIBLEVEL,    /* Level */
                     0,0,0,0,0,      /* Creation date */
                     0,0,0,0,0,      /* Last update */
                     "?",            /* OS or Hostname */
                     " ",            /* Not used by DB */
                     0,              /* Not used by DB */
                     0,              /* Not used by DB */
                     "?",            /* OS Release */
                     "?",            /* OS Version */
                     0 };            /* Not used by DB */

/*   gmsdat_db is the system data for the current DB library.
     DBinit() sets Creation date and OS info. DBexit() stets
     the update date and writes gmsdat_db to the DB file.
     DBLoad() checks that the file loaded is compatible with
     the current library version. */

V3MSIZ gmssiz;

/*    gmssiz är en kopia av aktuell systemstorlek.
      gmssiz.gm används för att sätta GM:s fysiska
      storlek i antal sidor. */

GMMDAT metdat;

/*    metdat är en structure med data väsentliga för GM:s
      minneshantering. metdat lagras precis som gmsdat i page-
      filen. */

/*!******************************************************/

        DBstatus DBreset()

/*      Clears the DB with gmclr() and creates the top
 *      level part entity named "Root_part".
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      1998-12-16 Tagit bot jobnam, J.Kjellander
 *      1999-01-24 Init ptr. to keytable.
 *      2004-07-18 Mesh, J.Kjellander, Örebro uniersity
 *
 ******************************************************!*/

  {
    DBstatus status;
    GMPRT    part;

/*
***Init the DB with default size of logtab.
*/
    if ( (status=gmclr(LTSDEF)) < 0 ) return(status);
/*
***Create the root part. From. 1.16E a complete part is created
***with all members set. Earlier versions only had some members
**set.
*/
    part.hed_pt.type     = PRTTYP;
    part.hed_pt.p_ptr    = DBNULL;     /* The root part has no parent */
    part.hed_pt.n_ptr    = DBNULL;     /* From 1.16E */
    part.hed_pt.seknr    = 0;          /* From 1.16E */
    part.hed_pt.ordnr    = 0;          /* From 1.16E */
    part.hed_pt.vers     = GMPOSTV0;   /* From 1.16E */
    part.hed_pt.g_ptr[0] = DBNULL;     /* Used for keytab from 1.16E */
    part.hed_pt.g_ptr[1] = DBNULL;     /* Currently not used */
    part.hed_pt.g_ptr[2] = DBNULL;     /* Currently not used */
    part.hed_pt.blank    = 0;          /* From 1.16E */
    part.hed_pt.hit      = 0;          /* From 1.16E */
    part.hed_pt.pen      = 0;          /* From 1.16E */
    part.hed_pt.level    = 0;          /* From 1.16E */

    strcpy(part.name_pt,"Root_part");  /* Name */
    part.dummy_pt  = 0;                /* From 1.16E */
    part.dts_pt    = 0;                /* From 1.16E */
    part.dtp_pt    = DBNULL;           /* From 1.16E */
    part.itp_pt    = 0;                /* Pointer to root ID table */
    part.its_pt    = 0;                /* Current size of root ID table */

    wrdat1( (char *)&part,&huvprt,sizeof(GMPRT)); /* Store in DB */
    actprt = huvprt;                              /* Update pointer */
/*
***This DB will have the same version as the current system.
*/
    V3MOME(&gmsdat_db,&gmsdat_org,sizeof(V3MDAT));
/*
***Clear the patchdata cache of DBread_one_patch() so that any
***old patchdata is not used again.
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Clear the mesh data cache of DBread_mesh() so that any
***old data is not used again.
*/
    DBread_mesh(NULL,DBNULL,0);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmsvpf()

/*      Skriver ut GM till disc. Först skrivs alla
 *      modifierade sidor i primärminne ut till page-
 *      filen. Därmed är alla GM:s sidor placerade
 *      i pagefilen. För att kunna läsa in den igen
 *      skrivs också LOGTAB mm. ut sist i page-filen.
 *      För att kunna hitta LOGTAB skrivs pfsiz in först
 *      i pagefilen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GM0043 => Fel vid skrivning till disk.
 *          GM0113 => Skrivning till ej öppnad page-fil.
 *
 *      (C)microform ab 28/12/84 J. Kjellander
 *
 *      9/10/86  IBMATX, J. Kjellander
 *      2/11/88  Felhantering, J. Kjellander
 *      17/11/92 VARREC, J. Kjellander
 *      1999-02-09 gmsdat_db, J.Kjellander
 *
 ******************************************************!*/

  {
    char   pagbuf[PAGSIZ];
    char  *bptr;
    DBint  i,size;

/*
***Dont try to save a DB that is not initiated.
*/
    if ( gmpfpk == NULL ) return(erpush("DB0113",""));
/*
***Write the keytable cache to pf.
*/
    gmsvkt();
/*
***If an unlinked entity exits remove it now so that
***it is not written to disc.
*/
    if ( templa != DBNULL ) gmrltm();
/*
***Write all modifed pages in the buffer to the pagefile.
*/
    for ( i=0; i < fystsz; ++i )
      {
      if ( fystab[i].wrflg == TRUE )
/*
***This page is modified. Write it to the pf.
*/
        {
        if ( fystab[i].pfpadr == PGFPNA )
          {
          fystab[i].pfpadr = pfsiz;
          if ( fseek( gmpfpk, (long)fystab[i].pfpadr, 0) != 0 ) goto error;
          if ( fwrite((char *)&gmbuf[PAGSIZ*i],PAGSIZ,1,gmpfpk) == 0 )
            goto error;
          pfsiz += PAGSIZ;
          }
        else
          {
          if ( fseek( gmpfpk, (long)fystab[i].pfpadr, 0) != 0 ) goto error;
          if ( fwrite((char *)&gmbuf[PAGSIZ*i],PAGSIZ,1,gmpfpk) == 0 )
            goto error;
          }
        fystab[i].wrflg = FALSE;
        }
/*
***This page is not modified. Give it back to the pf without
***writing.
*/
      if( fystab[i].pagnum != GMBPNA )
         logtab[ fystab[i].pagnum ].ptr = -fystab[i].pfpadr;
      fystab[i].pagnum = GMBPNA;
      }
/*
***Now write all metadata to the PF as well. Note that gmsdat_org
***is now replaced with gmsdat_db.
*/
    bptr = pagbuf;
    V3MOME(&gmsdat_db,bptr,sizeof(V3MDAT));  bptr += sizeof(V3MDAT);
    V3MOME(&metdat,bptr,sizeof(GMMDAT));     bptr += sizeof(GMMDAT);
    V3MOME(&dpgnum,bptr,sizeof(dpgnum));     bptr += sizeof(dpgnum);
    V3MOME(&datofs,bptr,sizeof(datofs));     bptr += sizeof(datofs);
    V3MOME(&ipgnum,bptr,sizeof(ipgnum));     bptr += sizeof(ipgnum);
    V3MOME(&huvidm,bptr,sizeof(huvidm));     bptr += sizeof(huvidm);
    V3MOME(&huvprt,bptr,sizeof(DBptr));      bptr += sizeof(DBptr);

    size = (DBint)(bptr - pagbuf);           /* Size of metadata */
    if ( fseek( gmpfpk, (long)pfsiz, 0) != 0 ) goto error;
    if ( fwrite(pagbuf,(size_t)size,1,gmpfpk) == 0 ) goto error;
/*
***Last of all, write logtab.
*/
    if ( fwrite((char *)logtab,sizeof(LTBSTR),
                (size_t)metdat.logmax,gmpfpk) == 0 ) goto error;
/*
***First 4 bytes hold pf size.
*/
    if ( fseek( gmpfpk, (long)0, 0) != 0 ) goto error;
    if ( fwrite((char *)&pfsiz,sizeof(pfsiz),1,gmpfpk) == 0 ) goto error;
/*
***End.
*/
    return(0);
/*
***Error exit.
*/
error:
    return(erpush("DB0043",""));
    }
     
/********************************************************/
/*!******************************************************/

        DBstatus gmldpf()

/*      Loads a pagefile from disc. 
 *
 *      FV:      0 => Ok.
 *          GM0033 => Invalid data or disc read error.
 *
 *      (C)microform ab 28/12/84 J. Kjellander
 *
 *      17/2/86    gmsdat, J. Kjellander
 *      5/4/86     Ny felhantering, J. Kjellander
 *      9/10/86    IBMATX, J. Kjellander
 *      17/11/92   VARREC, J. Kjellander
 *      1999-01-24 gmsdat_org, J.Kjellander
 *      1999-02-09 errorchecking, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***First, read pfsiz.
*/
   if ( fseek( gmpfpk,(long)0, 0) != 0 ) goto read_error;
   if ( fread((char *)&pfsiz,sizeof(pfsiz),1,gmpfpk) == 0 ) goto read_error;
   if ( pfsiz < 0 ) goto data_error;
/*
***Now read various metadata.
*/
   if ( fseek(gmpfpk, (long)pfsiz, 0) != 0 ) goto read_error;

   if ( fread((char *)&gmsdat_org,sizeof(V3MDAT),1,gmpfpk) == 0 )
     goto read_error;
   if ( fread((char *)&metdat,sizeof(GMMDAT),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&dpgnum,sizeof(dpgnum),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&datofs,sizeof(datofs), 1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&ipgnum,sizeof(ipgnum),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&huvidm,sizeof(huvidm),1,gmpfpk) == 0 )
      goto read_error;
   if ( fread((char *)&huvprt,sizeof(DBptr), 1,gmpfpk) == 0 )
      goto read_error;
/*
***Check for fatal errors. This is not a waterproof test
***of the DB file validity but will discover obvoius errors.
*/
   if ( dpgnum <   0 ) goto data_error;
   if ( datofs <   0 ) goto data_error;
   if ( ipgnum <  -1 ) goto data_error;
   if ( huvidm <  -1 ) goto data_error;
   if ( huvprt <=  0 ) goto data_error;
/*
***Read logtab.
*/
   if ( fread((char *)logtab,sizeof(LTBSTR),
                           (size_t)metdat.logmax,gmpfpk) == 0 ) goto read_error;
/*
***Init act-variables.
*/
   actidm = huvidm;
   actprt = huvprt;
/*
***Load and init keytab.
*/
  if ( keytab != NULL )
    {
    v3free(keytab,"gmldpf");
    keytab = NULL;
    nkeys  = 0;
    }
   gmldkt();
/*
***End.
*/
   return(0);
/*
***Read error.
*/
read_error:
   gmclpf();
   return(erpush("DB0033",""));
/*
***Corrupt data.
*/
data_error:
   gmclpf();
   return(erpush("DB0223",""));
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmldkt()

/*      Loads a keytable if one exists. Called
 *      by gmldpf() after everything else is
 *      loaded and initialized.
 *
 *      (C)microform ab 1999-01-24 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr keytab_ptr;
   DBint nbytes;
   GMPRT part;

/*
***Keytables were introduced in Varkon 1.16E by using
***the first 2 group pointers of the Root part.
***Group pointers from older files are not valid.
*/
   if ( gmsdat_org.vernr == 1  &&  (gmsdat_org.revnr < 16  ||
        (gmsdat_org.revnr == 16 && gmsdat_org.level < 'E')) ) return(0);
/*
***Files 1.16E or later may have a keytable stored. Read
***the Root part and check the first group pointer.
*/
   rddat1((char *)&part,huvprt,sizeof(GMPRT));
   keytab_ptr = part.hed_pt.g_ptr[0];
/*
***If the file includes a keytable, load it.
*/
   if ( keytab_ptr != DBNULL )
     {
     nkeys = (DBint)part.hed_pt.g_ptr[1];
     nbytes = nkeys*sizeof(KEYDATA);
     keytab = (KEYDATA *)v3mall(nbytes,"gmldkt");
     if ( keytab == NULL ) return(erpush("DB0193",""));
     if ( nbytes <= PAGSIZ ) rddat1((char *)keytab,keytab_ptr,nbytes);
     else                    rddat2((char *)keytab,keytab_ptr,nbytes);
     }
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmsvkt()

/*      Writes the keytable to pf. Called
 *      by gmsvpf().
 *
 *      (C)microform ab 1999-01-24 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr keytab_ptr;
   DBint keytab_size,nbytes;
   GMPRT part;

/*
***Get the Root part.
*/
   rddat1((char *)&part,huvprt,sizeof(GMPRT));
/*
***If an old table exists in pf, delete it. Only files 1.16E
***or later can have keytables.
*/
   if ( gmsdat_org.vernr > 1  ||
        (gmsdat_org.vernr == 1 && (gmsdat_org.revnr > 16 ||
         (gmsdat_org.revnr == 16 && (gmsdat_org.level >= 'E')))) )
     {
     keytab_ptr = part.hed_pt.g_ptr[0];
     if ( keytab_ptr != DBNULL )
       {
       keytab_size = part.hed_pt.g_ptr[1];
       nbytes = keytab_size*sizeof(KEYDATA);
       if ( nbytes <= PAGSIZ ) rldat1(keytab_ptr,nbytes);
       else                    rldat2(keytab_ptr,nbytes);
       }
     }
/*
***Older versions have incomplete Root parts. Since this pf
***will be saved with curent version we fix that here so that
***next time it is loaded it's a valid 1.16E or later.
*/
   else
     {
     part.hed_pt.type     = PRTTYP;
     part.hed_pt.p_ptr    = DBNULL;     /* The root part has no parent */
     part.hed_pt.n_ptr    = DBNULL;     /* From 1.16E */
     part.hed_pt.seknr    = 0;          /* From 1.16E */
     part.hed_pt.ordnr    = 0;          /* From 1.16E */
     part.hed_pt.vers     = GMPOSTV0;   /* From 1.16E */
     part.hed_pt.g_ptr[0] = DBNULL;     /* Used for keytab from 1.16E */
     part.hed_pt.g_ptr[1] = DBNULL;     /* Currently not used */
     part.hed_pt.g_ptr[2] = DBNULL;     /* Currently not used */
     part.hed_pt.blank    = 0;          /* From 1.16E */
     part.hed_pt.hit      = 0;          /* From 1.16E */
     part.hed_pt.pen      = 0;          /* From 1.16E */
     part.hed_pt.level    = 0;          /* From 1.16E */

     strcpy(part.name_pt,"Root_part");  /* Name */
     part.dummy_pt  = 0;                /* From 1.16E */
     part.dts_pt    = 0;                /* From 1.16E */
     part.dtp_pt    = DBNULL;           /* From 1.16E */
     part.itp_pt    = 0;                /* Pointer to root ID table */
     part.its_pt    = 0;                /* Current size of root ID table */
     }
/*
***Does a keytable cache exist ? If so, save it in pf.
*/
   if ( keytab != NULL )
     {
     keytab_size = nkeys;
     nbytes = keytab_size*sizeof(KEYDATA);
     if ( nbytes <= PAGSIZ ) wrdat1((char *)keytab,&keytab_ptr,nbytes);
     else                    wrdat2((char *)keytab,&keytab_ptr,nbytes);
     }
/*
***If not, set the pointer and size = 0.
*/
   else
     {
     keytab_ptr = DBNULL;
     keytab_size = 0;
     }
/*
***Save the Root part with the new keytab pointer and size.
*/
   part.hed_pt.g_ptr[0] = keytab_ptr;
   part.hed_pt.g_ptr[1] = keytab_size;

   updata((char *)&part,huvprt,sizeof(GMPRT));
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmclr(DBint logmax)

/*      Clears (deletes) everyting in the DB. This routine
 *      marks all pages unused, clears keytab, inits act-
 *      variables, removes temporary entites, allocates
 *      not allocated buffers etc. etc.
 *
 *      gmclr() is used by DBreset() to reset the DB to
 *      initial status.
 *      
 *      In: logmax = Önskat antal logiska sidor, dvs. GM:s
 *                   maximala storlek.
 *
 *      Ut: Inget.
 *
 *      Felkoder: GM0123 = Kan ej allokera minne för logtab
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      21/10/85 reflg, J. Kjellander
 *      17/2/86  gmsdat, J. Kjellander
 *      22/5/89  logmax, J. Kjellander
 *      30/3/92  templa, J. Kjellander
 *      18/11/92 fystsz, J. Kjellander
 *      14/3/95  v3mall()/v3free(), J. Kjellander
 *      1999-01-21 keytab, J.Kjellander
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBint  i;
    char   buf[40];

/*
***Allokera minne för gmbuf och fystab. Om sysize.gm
***har ett värde har detta angetts på kommandoraden när
***systemet startades. Om inte tar vi default från
***DBintern.h.
*/
    if ( gmssiz.gm > 0 ) fystsz = (DBpagnum)gmssiz.gm;
    else fystsz = FYSTSZ;

    if ( fystab == NULL )
      {
      if ( (gmbuf=(char *)v3mall(fystsz*PAGSIZ,"gmbuf")) == NULL )
        {
        sprintf(buf,"%d",(int)fystsz);
        return(erpush("DB0133",buf));
        }
      if ( (fystab=(FTBSTR *)v3mall(fystsz*sizeof(FTBSTR),"fystab")) == NULL )
        {
        sprintf(buf,"%d",(int)fystsz);
        return(erpush("DB0143",buf));
        }
      }
/*
***Init FYSTAB.
*/
    for ( i=0; i<fystsz; ++i) 
      {
      fystab[i].pagnum = GMBPNA;     /* Sidan är ej allokerad */
      fystab[i].pfpadr = PGFPNA;     /* PF-sida är ej allokerad */
      fystab[i].wrflg = FALSE;       /* Ej heller modifierad */
      fystab[i].reflg = FALSE;       /* eller ens refererad */
      }
    cirpek = cirdef = fystsz - 10;
    fystal = 0;
/*
***Init LOGTAB.
*/
    if ( logtab != NULL ) v3free((char *)logtab,"gmclr");
    if ( (logtab=(LTBSTR *)v3mall(logmax*sizeof(LTBSTR),"logtab")) == NULL )
      {
      sprintf(buf,"%d",(int)logmax);
      return(erpush("DB0123",buf));
      }

    for ( i=0; i<logmax; ++i)
      {
      logtab[i].ptr = PGFPNA;        /* Pagefil-sida ej allokerad */
      logtab[i].all = FALSE;         /* Sidan är ledig */
      }

    metdat.logmax = logmax;
/*
***Init IDTAB.
*/
    ipgnum = -1;
    huvidm = -1;
/*
***Init data-area.
*/
    dpgnum = logmax - 1;
    datofs = PAGSIZ;
    pfsiz  = sizeof(pfsiz);
/*
***Init act-variables.
*/
    actidm = huvidm;
    actidt = 0;
/*
***Init last-variables.
*/
    lstprt = 0;
    lstid  = 0;
    nxtins = DBNULL;
    lstidm = 0,
    lstidt = 0;
/*
***Init temp-variables.
*/
    templa = DBNULL;
    tempsn = 0;
/*
***Init keytab.
*/
    if ( keytab != NULL ) v3free((char *)keytab,"gmclr");
    keytab = NULL;
    nkeys = 0;
/*
***Clear the patchdata cache of DBread_one_patch() so that any
***old patchdata is not used again.
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Clear the mesh data cache of DBread_mesh() so that any
***old data is not used again.
*/
    DBread_mesh(NULL,DBNULL,0);

/*
***End.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmgrow()

/*      Ökar storleken på logtab med LTSDEF.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: GM0123 = Kan ej allokera minne för logtab
 *
 *      (C)microform ab 22/5/89 J. Kjellander
 *
 *      28/6/89 Bug act-variabler, J. Kjellander
 *      31/10/92 realloc(), J. Kjellander
 *      14/3/95  v3rall(), J. Kjellander
 *
 ******************************************************!*/

  {
   DBint newmax,i;
   char  buf[80];

/*
***Beräkna ny storlek på logtab.
*/
   newmax = metdat.logmax + LTSDEF;
/*
***Utöka logtabs storlek med realloc(). Den gamla algoritmen
***bestod i att göra gmsvpf() + gmclr() + gmldpf(). Detta tog
***för mycket tid, särskilt på VAX. 921031 JK.
*/
   if ( (logtab=(LTBSTR *)v3rall((char *)logtab,
      (unsigned)(newmax*sizeof(LTBSTR)),"gmgrow")) == NULL )
     {
     sprintf(buf,"%d",(int)newmax);
     return(erpush("DB0123",buf));
     }
/*
***Initiera de nya sidorna.
*/
    for ( i=metdat.logmax; i<newmax; ++i )
      {
      logtab[i].ptr = PGFPNA;        /* Pagefil-sida ej allokerad */
      logtab[i].all = FALSE;         /* Sidan är ledig */
      }
/*
***Ställ om GM:s storlek i meta-data strukturen.
*/
   metdat.logmax += LTSDEF;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmcrpf(char *pagfil)

/*      Skapar en ny pagefil.
 *
 *      In: pagfil = Pagefilens namn.
 *
 *      Ut: Inget.
 *
 *      FV:  0       => Ok.
 *           GM0013  => Kan ej skapa page-fil.
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      17/11/92 VARREC, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Öppna en ny fil för läsning och skrivning.
*/
#ifdef WIN32
    if ( (gmpfpk=fopen(pagfil,"w+b")) == NULL ) goto error;
#else
    if ( (gmpfpk=fopen(pagfil,"w+")) == NULL ) goto error;
#endif
/*
***Slut.
*/
    return(0);
/*
***Fel.
*/
error:
       return(erpush("DB0013",pagfil));
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmclpf()

/*      Stänger pagefilen.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0       => Ok.
 *           GM0023  => Kan ej stänga pagefilen.
 *
 *      (C)microform ab 29/7/85 J. Kjellander
 *
 *      23/5/89  free(), J. Kjellander
 *      14/3/95  v3free(), J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Prova att stänga.
*/
    if ( fclose(gmpfpk) == EOF )
       return(erpush("DB0023",""));
/*
***Om det gick bra, sätt gmpfpk = NULL vilket betyder att 
***page-filen är stängd, deallokera minne för logtab och
***returnera 0.
*/
    else
      {
      gmpfpk = NULL;
      if ( logtab != NULL ) v3free(logtab,"gmclpf");
      logtab = NULL;
      return(0);
      }
  }

/********************************************************/
/*!******************************************************/

        DBstatus gmidst(DBId *idvek, char *idstr)

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
 *      (C)microform ab 1998-12-15 J. Kjellander
 *
 ******************************************************!*/

  {
    char  tmp[V3STRLEN];
    DBId *idptr;

    idstr[0] = '\0';
    idptr = idvek;

    if ( idptr->seq_val < 0 ) strcat(idstr,"#");

    do
      {
      sprintf(tmp,"#%d.%d",(int)abs(idptr->seq_val),(int)idptr->ord_val);
      strcat(idstr,tmp);
      }
    while ( (idptr=idptr->p_nextre) != NULL );

    return(0);
  }

/********************************************************/
