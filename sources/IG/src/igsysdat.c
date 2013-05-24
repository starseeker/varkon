/*!******************************************************************/
/*  igsysdat.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igangm();    List GM-metadata                                   */
/*  iganpm();    List PM-metadata                                   */
/*  lstitb();    List IDTAB                                         */
/*  rdgmpk();    Reads DB-pointer                                   */
/*  lstsyd();    List systemdata                                    */
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
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"

extern V3MDAT   sydata;
extern V3MSIZ   sysize;
extern DBseqnum huvidm;

/*!******************************************************/

       short igangm()

/*      Listar GM-metadata.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 25/8/85 J. Kjellander
 *
 ******************************************************!*/

  {
    GMMDAT gmd;
    char   buf[80];

/*
***Läs aktuella GM metadata.
*/
    gmgtmd(&gmd);
/*
***Skriv ut.
*/
    WPinla(iggtts(270));

    sprintf(buf,"%s%d",iggtts(27),(int)gmd.pagsiz);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(28),(int)gmd.logmax);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(29),(int)gmd.logusd);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(30),(int)gmd.fysmax);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(31),(int)gmd.fysusd);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(32),(int)gmd.pfsiz);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(33),(int)gmd.pfcnt);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(34),(int)gmd.wrcnt);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(35),(int)gmd.rdcnt);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(36),(int)gmd.ipgnum);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(37),(int)gmd.dpgnum);
    WPalla(buf,1);

    sprintf(buf,"%s%d",iggtts(38),(int)gmd.datofs);
    WPalla(buf,1);

    WPexla(TRUE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short iganpm()

/*      Listar PM-metadata.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 22/4/87 J. Kjellander
 *
 *      21/1/88  NUPART=40, J. Kjellander
 *
 ******************************************************!*/

  {
    char   buf[80];
    short  pant,i;
    char   namvek[100][JNLGTH+1];
    pm_ptr basvek[100];
    pm_ptr sizvek[100];
    pm_ptr stackp;
    pm_ptr heapp;

/*
***Läs aktuella PM metadata.
*/
    pmstat(&pant,namvek,basvek,sizvek,&stackp,&heapp);
/*
***Skriv ut.
*/
    WPinla(iggtts(270));

    sprintf(buf,"PMSIZE = %d",sysize.pm);
    WPalla(buf,1);

    sprintf(buf,"PM-stack = %d",stackp);
    WPalla(buf,1);

    sprintf(buf,"PM-heap = %d",heapp);
    WPalla(buf,1);
/*
***Läs och skriv ut aktuella RTS-data.
*/
    sprintf(buf,"RTSSIZE = %d",sysize.rts);
    WPalla(buf,1);

    sprintf(buf,"RTS-globala = 0");
    WPalla(buf,1);

    sprintf(buf,"RTS-lokala = %d",(int)(inglrb()-inggrb()));
    WPalla(buf,1);

    sprintf(buf,"RTS-bas = %d",(int)(ingrtb()-inggrb()));
    WPalla(buf,1);

    sprintf(buf,"RTS-stack = %d",(int)(ingrsp()-inggrb()));
    WPalla(buf,1);
/*
***Moduler i PM.
*/
    WPalla("Modules in cache:",2);

    for ( i=0; i<pant; ++i )
      {
      sprintf(buf,"Part = %-10s  Adress = %6d  Size = %6d",
        namvek[i],basvek[i],sizvek[i]);
      WPalla(buf,1);
      }

    WPexla(TRUE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short lstitb()

/*      Listar IDTAB
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      7/11/85  NOTUSD, J. Kjellander
 *      1/2/86   anrop till gmrdid() ändrat, J. Kjellander
 *
 ******************************************************!*/

  {
    DBseqnum snr;
    DBptr    la;
    char     strbuf[80];
    DBHeader   hdr;

    WPinla(iggtts(270));
    for (snr=0; snr<=huvidm; ++snr)
      {
      if ( (la=gmrdid(DBNULL,snr)) >= 0 )
         {
         DBread_header(&hdr,la);
         sprintf(strbuf,"ID=%d   LA=%d   Typ=%d",(int)snr,la,hdr.type);
         }
      else if ( la == NOTUSD )
         sprintf(strbuf,"ID=%d   Not used",(int)snr);
      else if ( la == ERASED )
         sprintf(strbuf,"ID=%d   Deleted",(int)snr);
      else
         sprintf(strbuf,"ID=%d   Not valid",(int)snr);
      WPalla(strbuf,1);
      }
    WPexla(TRUE);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short rdgmpk()

/*      Läser värde ur GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 8/8/85 J. Kjellander
 *
 ******************************************************!*/
  {
    int     adr;
    DBchar  val1;
    DBshort val2;
    DBint   val4;
    DBfloat val8;
    char    buf[V3STRLEN];

    if ( igsiip("Adress : ",&adr) == REJECT ) return(0);
    rddat1((char *) &val1, (DBptr)adr, sizeof(val1));
    rddat1((char *) &val2, (DBptr)adr, sizeof(val2));
    rddat1((char *) &val4, (DBptr)adr, sizeof(val4));
    rddat1((char *) &val8, (DBptr)adr, sizeof(val8));
    sprintf(buf,"val1=%d  val2=%d  val4=%d  val8=%g",
                 val1,val2,val4,val8);
    igplma(buf,IG_MESS);

    return(0);
  }
/********************************************************/
/*!******************************************************/

       short lstsyd()

/*      Listar sydat.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 30/1/85 J. Kjellander
 *
 *      11/9/85  Textsträngar, R. Svedin
 *      13/11/85 CPU-nummer, J. Kjellander
 *      4/5/86   Modul-data, J. Kjellander
 *      2/3/88   Ritpaket, J. Kjellander
 *      1996-12-05 Demo, J.Kjellander
 *
 ******************************************************!*/

  {
    char     buf[80];
    short    status;
    PMMODULE mhed;

/*
***Listrubrik.
*/
    WPinla(iggtts(270));
/*
***Demo-version.
*/
#ifdef V3_DEMO
    if ( (status=WPalla("Limited demo version of",(short)1)) < 0 ) return(status);
#endif
/*
***Serienummer.
*/
    if ( sydata.opmode == BAS_MOD )
      {
      sprintf(buf,"VARKON-3D/B %d.%d%c Serienummer #%d",sydata.vernr,
            sydata.revnr,sydata.level,sydata.sernr);
      }
    else if ( sydata.opmode == RIT_MOD )
      {
      sprintf(buf,"VARKON-3D/R %d.%d%c Serienummer #%d",sydata.vernr,
            sydata.revnr,sydata.level,sydata.sernr);
      }
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
/*
***Operativsystem.
*/
    sprintf(buf,"Operativsystem : %s %s.%s",sydata.sysname,sydata.release,
            sydata.version);
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
/*
***Jobb-data.
*/
    sprintf(buf,"Jobb skapat : %d/%d/%d  %d:%d",sydata.day_c,sydata.mon_c,
            sydata.year_c,sydata.hour_c,sydata.min_c);
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);

    sprintf(buf,"Jobb uppdaterat : %d/%d/%d  %d:%d",sydata.day_u,sydata.mon_u,
            sydata.year_u,sydata.hour_u,sydata.min_u);
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
/*
***Modul-data.
*/
    if ( sydata.opmode == BAS_MOD )
      {
      pmrmod(&mhed);
      sprintf(buf,"Modulens serienummer : %d",mhed.system.sernr);
      if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
/*
      sprintf(buf,"Modulens ser_crypt : %d",mhed.system.ser_crypt);
      if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
*/

      sprintf(buf,"Modulens skyddskod : %d",mhed.system.mpcode);
      if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
      }
/*
***Grafik-data.
*/
#ifdef V3_X11
    sprintf(buf,"Grafik = X11");
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
#endif
#ifdef WIN32
    sprintf(buf,"Grafik = Microsoft WIN32");
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);
#endif
    sprintf(buf,"Displayfilens storlek : %d poster",sysize.df);
    if ( (status=WPalla(buf,(short)1)) < 0 ) return(status);

    WPexla(TRUE);

    return(0);
  }

/********************************************************/
