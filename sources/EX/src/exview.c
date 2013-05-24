/*!*****************************************************
*
*    exview.c
*    ========
*
*    EXclgm();     Interface-routine for CLEAR_GM
*    EXldgm();     Interface-routine for LOAD_GM
*    EXsvgm();     Interface-routine for SAVE_GM
*
*    EXldjb();     Interface-routine for LOAD_JOB
*    EXsvjb();     Interface-routine for SAVE_JOB
*
*    EXbllv();     Interface-routine for BLANK_LEV
*    EXublv();     Interface-routine for UNBLANK_LEV
*    EXgtlv();     Interface-routine for GET_LEV
*
*    EXcrvp();     Interface-routine for CRE_VIEW by position
*    EXcrvc();     Interface-routine for CRE_VIEW by coord.sys.
*    EXacvi();     Interface-routine for ACT_VIEW
*    EXscvi();     Interface-routine for SCL_VIEW
*    EXcevi();     Interface-routine for CEN_VIEW
*    EXhdvi();     Interface-routine for HIDE_VIEW
*    EXprvi();     Interface-routine for PERP_VIEW
*    EXervi();     Interface-routine for ERASE_VIEW
*    EXrpvi();     Interface-routine for REP_VIEW
*    EXgvnl();     Returns viewname list
*    EXcavi();     Interface-routine for CACC_VIEW
*    EXplvi();     Interface-routine for PLOT_VIEW
*    EXplwi();     Interface-routine for PLOT_WIN
*
*    vyindx();     Translate viewname -> address in vytab
*
*    This file is part of the VARKON Execute Library.
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"
#include "../include/oldatts.h"
#include <string.h>


/*
***Old level tables. To be removed.
*/
tbool   nivtb1[NT1SIZ];   /* TRUE = Not visible */
NIVNAM  nivtb2[NT2SIZ];   /* Level name "" = Not defined */

extern DBptr  lsysla;
extern char   jobdir[],jobnam[],tmprit[];
extern V2NAPA defnap;
extern VY     vytab[],actvy;
extern double gpgszx,gpgszy;
extern bool   rstron,tmpref;
extern DBfloat  rstrox,rstroy,rstrdx,rstrdy;
extern V3MDAT sydata;
extern V3MSIZ sysize;
extern short  modtyp,modatt,menlev,stalev,posmod;
extern char   actcnm[];
extern DBTmat *lsyspk;
extern DBTmat lklsys,lklsyi;

#ifdef WIN32
extern int    msacvi(),msupvi(),msmaut(),msrepa();
extern short  mssvjb(),msldjb(),msuppr();
#endif

/*!******************************************************/

        short EXclgm()

/*      Interface-rutin för CLEAR_GM(). Nollställer och
 *      initierar GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 18/2/86 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sudda skärm och displayfil.
*/
    WPergw(GWIN_ALL);
/*
***Töm och initiera GM.
*/
    DBreset();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXldgm(char *filnam)

/*      Interface-rutin för LOAD_GM(fil). Laddar resultatfil
 *      till GM.
 *
 *      In: filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1642 = Kan ej ladda filen till GM.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      22/4/88  gmexit(), J. Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Sudda skärm och displayfil.
*/
    WPergw(GWIN_ALL);
/*
***Lagra GM.
*/
    if ( (status=DBexit()) < 0 ) return(status);
/*
***Ladda den nya resultatfilen.
*/
    if ( DBload(filnam,sysize.gm,
                DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 )
                              return(erpush("EX1642",filnam));
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXsvgm(char *filnam)

/*      Interface-rutin för SAVE_GM(fil). Lagrar GM i
 *      resultatfilen filnam.
 *
 *      In: filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1652 = Kan ej lagra resultatfil
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      19/9/95  Ritmodulen, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    char  resfil[V3PTHLEN+1];

/*
***Lagra GM.
*/
    if ( (status=DBexit()) < 0 ) return(status);
/*
***Om vi kör bas-moulen heter pagefilen någonting.RES.
*/
    if ( sydata.opmode == BAS_MOD )
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);
      }
/*
***Om vi kör ritmodulen heter den något temporärt.
*/
    else strcpy(resfil,tmprit);
/*
***Kopiera den lagrade resultatfilen till en fil med det nya namnet.
*/ 
    if ( v3fcpy(resfil,filnam) < 0 ) return(erpush("EX1652",filnam));
/*
***Ladda den gamla resultatfilen igen.
*/
    return(DBload(resfil,sysize.gm,
                  DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL));
  }

/********************************************************/
/*!******************************************************/

        short EXsvjb(char *filnam)

/*      Rutin för att lagra aktiva jobbdata i filen 
 *      filnam.JOB.
 *
 *      In: filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      FELKODER: EX1863 => Kan ej öppna jobbfil "filnamn".
 *                EX1873 => Kan ej skriva till jobbfil "filnamn".
 *                EX1883 => Kan ej stänga jobbfil "filnamn".
 *
 *      (C)microform ab 17/10/88 R. Svedin
 *
 *      15/11/88 posmod, J. Kjellander
 *      28/1-95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    FILE  *jobfpk;
    short  y,m,d,h,min,s;
    int    winant;
    double cnog,dscl;

/*
***Öppna filen.
*/
#ifdef WIN32
    if ( (jobfpk=fopen(filnam, "w+b")) == 0 ) return(erpush("EX1863",filnam));
#else
    if ( (jobfpk=fopen(filnam, "w+")) == 0 ) return(erpush("EX1863",filnam));
#endif
/*
***Uppdatera senaste tid för uppdatering.
*/
    EXtime(&y,&m,&d,&h,&min,&s);
    sydata.year_c = y;
    sydata.mon_c  = m;
    sydata.day_c  = d;
    sydata.hour_c = h;
    sydata.min_c  = min;
/*
***Ta reda på aktuell kurvnoggrannhet och ritningsskala.
*/
    WPget_cacc(&cnog);
    dscl = 1.0;
/*
***Skriv ut jobb-data.
*/
    if ( fwrite((char *)&sydata, sizeof(V3MDAT),     1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&modtyp, sizeof(short),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&modatt, sizeof(short),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&tmpref, sizeof(bool),       1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&lsyspk, sizeof(DBTmat *),   1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&lklsys, sizeof(DBTmat),     1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&defnap, sizeof(V2NAPA),     1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)nivtb1,  sizeof(tbool), NT1SIZ, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)nivtb2,  sizeof(NIVNAM),NT2SIZ, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&actvy,  sizeof(VY),         1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)vytab,   sizeof(VY),    GPMAXV, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)actcnm,  sizeof(char),JNLGTH+1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&rstrox, sizeof(DBfloat),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&rstroy, sizeof(DBfloat),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&rstrdx, sizeof(DBfloat),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&rstrdy, sizeof(DBfloat),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&rstron, sizeof(bool),       1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&cnog,   sizeof(double),     1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&dscl,   sizeof(double),     1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&menlev, sizeof(short),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&stalev, sizeof(short),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&posmod, sizeof(short),      1, jobfpk) == 0 ) 
      goto error;
    if ( fwrite((char *)&lsysla, sizeof(DBptr),      1, jobfpk) == 0 ) 
      goto error;
/*
***Multifönster.
*/
    winant = 0;

#ifdef UNIX
    if ( WPsvjb(jobfpk) < 0 ) goto error;
#else
#ifdef WIN32
    if ( mssvjb(jobfpk) < 0 ) goto error;
#else
    if ( fwrite((char *)&winant,sizeof(int),1, jobfpk) == 0 ) goto error;
#endif
#endif
/*
***Stäng jobfilen.
*/
    if ( fclose(jobfpk) == EOF) return(erpush("EX1883",filnam));

    return(0);
/*
***Felutgång.
*/
error:
    return(erpush("EX1873",filnam));

  }

/********************************************************/
/*!******************************************************/

        short EXldjb(
        char *filnam,
        short  kod)

/*      Rutin för läsa in jobdata ur filen filnam.JOB 
 *      enligt angiven kod.
 *      kod = 0 Alla jobbdata.     kod = 1 Endast nivåer. 
 *      kod = 2 Endast aktiv nivå. kod = 3 Endast attribut.
 *
 *      In: filnam = Pekare till filnamn.
 *          kod    = Typ av jobbdata att läsa in
 *
 *      Ut: Inget.
 *
 *      Felkod: EX1862 => Kan ej öppna jobbfil "filnamn".
 *              EX1893 => Fel vid läsning av jobbfil "filnamn".
 *              EX1902 => Jobbfilen ej kompatibel med
 *                        aktuell V3-version.
 *              EX1912 => Felaktig kod för LOAD_JOB()
 *
 *      (C)microform ab 21/10/88 R. Svedin
 *
 *      15/11/88 posmod, J. Kjellander
 *      1/2/89   Ny VY, J. Kjellander
 *      12/1/92  Kurvattribut, J. Kjellander
 *      25/3/92  lsysla, J. Kjellander
 *      25/9/92  Ny VY, J. Kjellander
 *      27/12/92 Nya attribut, J. Kjellander
 *      28/1-95  Multifönster, J. Kjellander
 *      1997-12-25 WIDTH, J.Kjellander
 *      1998-10-01 TPMODE, J.Kjellander
 *      2004-07-11 MFONT, J.Kjellander
 *
 ******************************************************!*/

  {
    short  i;
    double cnog,dscl;
    char   dumbuf[4*GMXMXL];         /* Skall klara största */
    V3MDAT tmpsyd;
    V2NAPA tmpdfn;
    NIV    nivvek[NIVANT];
    FILE  *jobfpk;
    OVY17  avy17,vyt17[GPMAXV];
    OVY111 avy111,vyt111[GPMAXV];

    if ( kod < 0 || kod > 3 ) return(erpush("EX1912",""));

/*
***Prova att öppna filen.
*/
#ifdef WIN32
    if ( (jobfpk=fopen(filnam, "rb")) == 0 ) return(erpush("EX1862",filnam));
#else
    if ( (jobfpk=fopen(filnam, "r")) == 0 ) return(erpush("EX1862",filnam));
#endif
/*
***Läs sydata från job-filen och kolla att 
***kompatibilitetsproblem ej föreligger.
*/
    if ( fread( &tmpsyd,sizeof(V3MDAT),1,jobfpk) == 0 ) goto error;

    if ( tmpsyd.revnr < 1 ) return(erpush("EX1902",filnam));
/*
***Om hela jobbfilen skall läsas in kopiera datum.
*/
    if ( kod == 0 )
      {
      sydata.year_c = tmpsyd.year_c;
      sydata.mon_c = tmpsyd.mon_c;
      sydata.day_c = tmpsyd.day_c;
      sydata.hour_c = tmpsyd.hour_c;
      sydata.min_c = tmpsyd.min_c;

      sydata.year_u = tmpsyd.year_u;
      sydata.mon_u = tmpsyd.mon_u;
      sydata.day_u = tmpsyd.day_u;
      sydata.hour_u = tmpsyd.hour_u;
      sydata.min_u = tmpsyd.min_u;
      }
/*
***Läs från jobfilen  till global eller dummy variabel.
*/
    if ( kod == 0 ) 
      {
      if ( fread( &modtyp, sizeof(short),    1, jobfpk) == 0 ) goto error;
      if ( fread( &modatt, sizeof(short),    1, jobfpk) == 0 ) goto error;
      if ( fread( &tmpref, sizeof(bool),     1, jobfpk) == 0 ) goto error;
      if ( fread( &lsyspk, sizeof(DBTmat *), 1, jobfpk) == 0 ) goto error;
      if ( fread( &lklsys, sizeof(DBTmat),   1, jobfpk) == 0 ) goto error;
      }
    else 
      {
      if ( fread( dumbuf, sizeof(short),    1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(short),    1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(bool),     1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBTmat *), 1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBTmat),   1, jobfpk) == 0 ) goto error;
      }
/*
***Defnap läses olika beroende på jobfilens ålder.
*/
    if ( tmpsyd.revnr > 17 || (tmpsyd.revnr == 17 && tmpsyd.level > 'D') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(V2NAPA), 1, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf,sizeof(V2NAPA), 1, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 16 || (tmpsyd.revnr == 16 && tmpsyd.level > 'B') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP117D), 1, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP117D), 1, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 15 || (tmpsyd.revnr == 15 && tmpsyd.level > 'B') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP116B), 1, jobfpk) == 0 ) goto error;
        defnap.tpmode = 0;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP116B), 1, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 11 || (tmpsyd.revnr == 11 && tmpsyd.level > 'F') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP115), 1, jobfpk) == 0 ) goto error;
        defnap.tpmode = 0;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP115), 1, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 10 || (tmpsyd.revnr == 10 && tmpsyd.level > 'L') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP111), 1, jobfpk) == 0 ) goto error;
        defnap.tfont = 0;
        defnap.tpmode = 0;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP111), 1, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 3 || (tmpsyd.revnr == 2 && tmpsyd.level > 'B') )
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP110), 1, jobfpk) == 0 ) goto error;
        defnap.cfont = 0;
        defnap.cdashl = 3.0;
        defnap.tfont = 0;
        defnap.tpmode = 0;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP110), 1, jobfpk) == 0 ) goto error;
        }
      }
    else
      {
      if ( kod == 0 || (kod == 2) || (kod == 3 ))
        {
        if ( fread( &tmpdfn,sizeof(ONAP13),1,jobfpk) == 0 ) goto error;
        defnap.hit = TRUE;
        defnap.save = TRUE;
        defnap.cfont = 0;
        defnap.cdashl = 3.0;
        defnap.tfont = 0;
        defnap.tpmode = 0;
        }
      else
        {
        if ( fread( dumbuf,sizeof(ONAP13),1,jobfpk) == 0 ) goto error;
        }
      }
/*
***Nivåer läses olika from. V1.2I.
*/
    if ( tmpsyd.revnr > 2 || (tmpsyd.revnr == 2 && tmpsyd.level > 'H') )
      {
      if ( kod == 0 || (kod == 1 ))
        {
        if ( fread( nivtb1, sizeof(tbool),  NT1SIZ, jobfpk) == 0 ) goto error;
        if ( fread( nivtb2, sizeof(NIVNAM), NT2SIZ, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf, sizeof(tbool),  NT1SIZ, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(NIVNAM), NT2SIZ, jobfpk) == 0 ) goto error;
        }
      }
    else
      {
      if ( kod == 0 || (kod == 1 ))
        {
        if (fread(  nivvek, sizeof(NIV),    NIVANT, jobfpk) == 0 ) goto error;
        for ( i=0; i<NIVANT; ++i )
           {
           if ( !nivvek[i].vis ) nivtb1[i] = TRUE;
           if (  nivvek[i].def )
             {
             nivtb2[i].num = i;
             strcpy(nivtb2[i].nam,nivvek[i].nivnam);
             }
           }
        }
      else
        {
        if ( fread( dumbuf, sizeof(NIV),    NIVANT, jobfpk) == 0 ) goto error;
        }
      }
/*
***Vyer läses olika fr.o.m. V1.7E och V1.11C.
*/  
    if ( tmpsyd.revnr > 11 || (tmpsyd.revnr == 11 && tmpsyd.level > 'B') )
      {
      if ( kod == 0 )
        {
        if ( fread( &actvy,  sizeof(VY),     1, jobfpk) == 0 ) goto error;
        if ( fread(  vytab,  sizeof(VY),GPMAXV, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf, sizeof(VY),     1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(VY),GPMAXV, jobfpk) == 0 ) goto error;
        }
      }
    else if ( tmpsyd.revnr > 7 || (tmpsyd.revnr == 7 && tmpsyd.level > 'D') )
      {
      if ( kod == 0 )
        {
        if ( fread( &avy111,  sizeof(OVY111),     1, jobfpk) == 0 ) goto error;
        strcpy(actvy.vynamn,avy111.vynamn);
        V3MOME(&avy111.vyrikt,&actvy.vyrikt,sizeof(VYVEC));
        V3MOME(avy111.vywin,actvy.vywin,4*sizeof(double));
        actvy.vydist = avy111.vydist;
        actvy.vy3d = avy111.vy3d;
        actvy.vytypp = TRUE;
        if ( fread(  vyt111,  sizeof(OVY111),GPMAXV, jobfpk) == 0 ) goto error;
        for ( i=0; i<GPMAXV; ++i )
          {
          if ( vyt111[i].vynamn[0] != '\0' )
            {
            strcpy(vytab[i].vynamn,vyt111[i].vynamn);
            V3MOME(&vyt111[i].vyrikt,&vytab[i].vyrikt,sizeof(VYVEC));
            V3MOME(vyt111[i].vywin,vytab[i].vywin,4*sizeof(double));
            vytab[i].vydist = vyt111[i].vydist;
            vytab[i].vy3d = vyt111[i].vy3d;
            vytab[i].vytypp = TRUE;
            }
          }
        }
      else
        {
        if ( fread( dumbuf, sizeof(OVY111),     1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(OVY111),GPMAXV, jobfpk) == 0 ) goto error;
        }
      }
    else
      {
      if ( kod == 0 )
        {
        if ( fread( &avy17,  sizeof(OVY17),     1, jobfpk) == 0 ) goto error;
        strcpy(actvy.vynamn,avy17.vynamn);
        V3MOME(&avy17.vyrikt,&actvy.vyrikt,sizeof(VYVEC));
        V3MOME(avy17.vywin,actvy.vywin,4*sizeof(double));
        if ( actvy.vyrikt.x_vy != 0.0  ||  actvy.vyrikt.y_vy != 0.0 )
          actvy.vy3d = TRUE; else actvy.vy3d = FALSE;
        actvy.vydist = 0.0;
        actvy.vytypp = TRUE;
        if ( fread(  vyt17,  sizeof(OVY17),GPMAXV, jobfpk) == 0 ) goto error;
        for ( i=0; i<GPMAXV; ++i )
           {
           if ( vyt17[i].vynamn[0] != '\0' )
             {
             strcpy(vytab[i].vynamn,vyt17[i].vynamn);
             V3MOME(&vyt17[i].vyrikt,&vytab[i].vyrikt,sizeof(VYVEC));
             V3MOME(vyt17[i].vywin,vytab[i].vywin,4*sizeof(double));
             if ( vyt17[i].vyrikt.x_vy != 0.0  || 
               vyt17[i].vyrikt.y_vy != 0.0 ) vytab[i].vy3d = TRUE;
             else vytab[i].vy3d = FALSE;
             vytab[i].vydist = 0.0;
             actvy.vytypp = TRUE;
             }
           }
        }
      else
        {
        if ( fread( dumbuf, sizeof(OVY17),     1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(OVY17),GPMAXV, jobfpk) == 0 ) goto error;
        }
      }
/*
***Om hela jobbfilen skall läsas in 
***Läs resten av jobfilen direkt till aktuella globala varabler.
*/  
    if ( kod == 0 )
      {
      if ( fread(  actcnm, sizeof(char),JNLGTH+1, jobfpk) == 0 ) goto error;
      if ( fread( &rstrox, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( &rstroy, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( &rstrdx, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( &rstrdy, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( &rstron, sizeof(bool),       1, jobfpk) == 0 ) goto error;
      }
    else
      {
      if ( fread( dumbuf, sizeof(char),JNLGTH+1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(DBfloat),      1, jobfpk) == 0 ) goto error;
      if ( fread( dumbuf, sizeof(bool),       1, jobfpk) == 0 ) goto error;
      }
/*
***From. V1.3 även följande.
*/
    if ( tmpsyd.revnr > 2 )
      {
      if ( kod == 0 )
        {
        if ( fread( &cnog, sizeof(double),  1, jobfpk) == 0 ) goto error;
        WPset_cacc(cnog);
        if ( fread( &dscl, sizeof(double),  1, jobfpk) == 0 ) goto error;
        if ( fread( &menlev, sizeof(short), 1, jobfpk) == 0 ) goto error;
        if ( fread( &stalev, sizeof(short), 1, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf, sizeof(double), 1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(double), 1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(short),  1, jobfpk) == 0 ) goto error;
        if ( fread( dumbuf, sizeof(short),  1, jobfpk) == 0 ) goto error;
        }
      }
/*
***From. V1.7 även följande.
*/
    if ( tmpsyd.revnr > 6 )
      {
      if ( kod == 0 )
        {
        if ( fread( &posmod, sizeof(short),  1, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf, sizeof(short),  1, jobfpk) == 0 ) goto error;
        }
      }
    else posmod = 0;
/*
***From. V1.10O även följande.
*/
    if ( tmpsyd.revnr > 10 || (tmpsyd.revnr == 10 && tmpsyd.level > 'N') )
      {
      if ( kod == 0 )
        {
        if ( fread( &lsysla, sizeof(DBptr),  1, jobfpk) == 0 ) goto error;
        }
      else
        {
        if ( fread( dumbuf, sizeof(DBptr),  1, jobfpk) == 0 ) goto error;
        }
      }
    else lsysla = DBNULL;
/*
***From. V1.12G även multifönster.
*/
#ifdef UNIX
    if ( tmpsyd.revnr > 12 || (tmpsyd.revnr == 12 && tmpsyd.level > 'F') )
      {
      if ( kod == 0 )
        {
        if ( WPldjb(jobfpk) < 0 ) goto error;
        }
      }
#endif
/*
***WIN32 innebär alltid multifönster.
*/
#ifdef WIN32
    if ( msldjb(jobfpk,&tmpsyd) < 0 ) goto error;
#endif
/*
***Stäng jobbfilen.
*/
    if ( fclose(jobfpk) == EOF ) goto error;
/*
***Jobbfil inläst.
***Om hela jobbfilen eller något attribut är inläst uppdatera defnap.
*/
    if ( kod == 0 || (kod == 2) || (kod == 3) ) incdnp(&tmpdfn,kod);

    return(0);
/*
***Felutgång.
*/
error:
    return(erpush("EX1893",filnam));
  }

/********************************************************/
/*!******************************************************/

        short EXbllv(
        DBint levnum,
        DBint win_id)

/*      Interface-rutin för BLANK_LEV(n). Släcker
 *      nivå.
 *
 *      In: levnum = Nivånummer.
 *          win_id = Fönster-ID.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1632 = Otillåtet nivånummer.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      1/9/86   R. Svedin. Ändrad feltest.
 *      29/9/86  R. Svedin. Ny nivåhant.
 *      17/1-95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
/*
***Initiering.
*/
    status = 0;
/*
***Kolla att levnum har rimligt värde.
*/
    if ( levnum < 0 || levnum > NT1SIZ-1) return(erpush("EX1632",""));
/*
***Släck nivån.
*/
#ifdef UNIX
    WPmtsl(win_id,levnum,levnum,WP_BLANKL);
#else
    nivtb1[levnum] = TRUE;
#endif

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short EXublv(
        DBint levnum,
        DBint win_id)

/*      Interface-rutin för UNBLANK_LEV(n). Tänder
 *      nivå.
 *
 *      In: levnum = Nivånummer.
 *          win_id = Fönster-ID.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1632 = Otillåtet nivånummer.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      1/8/86   Ändrad feltest, R. Svedin.
 *      29/9/86  Ny nivåhantering, R. Svedin.
 *      17/1-95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
/*
***Initiering.
*/
    status = 0;
/*
***Kolla att levnum har rimligt värde.
*/
    if ( levnum < 0 || levnum > NT1SIZ-1 ) return(erpush("EX1632",""));
/*
***Tänd nivån.
*/
#ifdef V3_X11
    status = WPmtsl(win_id,levnum,levnum,WP_UBLANKL);
#else
    nivtb1[levnum] = FALSE;
#endif

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short EXgtlv(
        DBshort levnum,
        bool   *blank,
        char   *name)

/* !!!!!!!!! OBS namn bör ersättas med fönster-ID. !!!!!!!
/*      Interface-rutin för GET_LEV(n,blank,name).
 *      Returnerar information om nivå.
 *
 *      In: levnum = Nivånummer.
 *
 *      Ut: blank = 0=>Tänd, 1=>Släckt
 *          name  = Pekare till namn.
 *
 *      Felkoder: EX1632 = Otillåtet nivånummer.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      29/9/86 Ny nivåhantering, R. Svedin.
 *
 ******************************************************!*/

  {
/*
    short i;

*
***Kolla att levnum har rimligt värde.
*
    if ( levnum < 0 || levnum > NT1SIZ-1 ) return(erpush("EX1632",""));
*
***Returnera värden.
*
    name[0] = '\0';
    *blank = nivtb1[levnum];

    for ( i = 0; i < NT2SIZ; ++i )
       {
       if ( nivtb2[i].nam[0] != '\0'  &&  nivtb2[i].num == levnum )
          strcpy(name,nivtb2[i].nam);
       }

    return(0);
*/
  }

/********************************************************/
/*!******************************************************/

        short EXcrvp(
        char   name[],
        VYVEC *bpos)

/*      Interface-rutin för CRE_VIEW. Skapar en ny vy
 *      med hjälp av en betraktelseposition.
 *
 *      In: name  => Vyns namn.
 *          bpos  => Betraktelseposition.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1672 = Max antal vyer redan skapade, vy'n ej skapad.
 *
 *      (C)microform ab 22/7/86 R. Svedin
 *
 *      10/10/86 Tagit bort param. för vyfönster, R. Svedin
 *      18/10/86 gpgwin(), J. Kjellander
 *      24/10/91 Bytt namn till EXcrvp(), J. Kjellander
 *      29/9/92  vytypp, J. Kjellander
 *      23/1-95  Multifönster, J. Kjellander
 *      2007-01-01 removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   short   i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Om vyn inte finns redan, leta upp en ledig plats i vytab.
*/
   if ( (i=vyindx(name)) == -1 )
     {
     for ( i=0; i<GPMAXV; ++i ) if ( vytab[i].vynamn[0] == '\0' ) break;
     if ( i == GPMAXV ) return(erpush("EX1672",""));
     }
/*
***Lagra/uppdatera vy:n på plats "i".
*/
   strncpy(vytab[i].vynamn,name,GPVNLN);
/*
***Kopiera betraktelsepositionen.
*/
   vytab[i].vyrikt.x_vy = bpos->x_vy;
   vytab[i].vyrikt.y_vy = bpos->y_vy;
   vytab[i].vyrikt.z_vy = bpos->z_vy;
/*
***Copy model window from GWIN_MAIN.
*/
   vytab[i].vywin[0] = gwinpt->vy.modwin.xmin;
   vytab[i].vywin[1] = gwinpt->vy.modwin.ymin;
   vytab[i].vywin[2] = gwinpt->vy.modwin.xmax;
   vytab[i].vywin[3] = gwinpt->vy.modwin.ymax;
/*
***Betraktelseavstånd och 3D-flagga.
*/
   vytab[i].vydist = 0.0;

   if ( bpos->x_vy == 0.0  &&  bpos->y_vy == 0.0  &&  bpos->z_vy > 0 )
     vytab[i].vy3d = FALSE;
   else
     vytab[i].vy3d = TRUE;
/*
***Vy är skapad med hjälp av betraktelseposition.
*/
   vytab[i].vytypp = TRUE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcrvc(
        char  name[],
        DBId *idvek)

/*      Skapar en ny vy med hjälp av ett koordinatsystem.
 *
 *      In: name  => Vyns namn.
 *          idvek => Pekare till identitet.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1672 = Max antal vyer redan skapade, vy'n ej skapad.
 *                EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 29/9/92 J. Kjellander
 *
 *      23/1-95  Multifönster, J. Kjellander
 *      2006-12-31 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    DBptr   la;
    DBetype typ;
    DBCsys  csy;
    DBTmat  mat;
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Get csys from DB.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0  ||  typ != CSYTYP )
        return(erpush("EX1402","EXcrvc"));
    DBread_csys(&csy,&mat,la);
/*
***Om vyn inte finns redan, leta upp en ledig plats i vytab.
*/
   if ( (i=vyindx(name)) == -1 )
     {
     for ( i=0; i<GPMAXV; ++i ) if ( vytab[i].vynamn[0] == '\0' ) break;
     if ( i == GPMAXV ) return(erpush("EX1672",""));
     }
/*
*** Kopiera namnet till vytab.
*/
   strncpy(vytab[i].vynamn,name,GPVNLN);
/*
***Copy model window from GWIN_MAIN.
*/
   vytab[i].vywin[0] = gwinpt->vy.modwin.xmin;
   vytab[i].vywin[1] = gwinpt->vy.modwin.ymin;
   vytab[i].vywin[2] = gwinpt->vy.modwin.xmax;
   vytab[i].vywin[3] = gwinpt->vy.modwin.ymax;
/*
***Betraktelseavstånd och 3D-flagga.
*/
   vytab[i].vydist = 0.0;
   vytab[i].vy3d = TRUE;
/*
***Själva matrisen.
*/
   vytab[i].vymatr.v11 = mat.g11;
   vytab[i].vymatr.v12 = mat.g12;
   vytab[i].vymatr.v13 = mat.g13;

   vytab[i].vymatr.v21 = mat.g21;
   vytab[i].vymatr.v22 = mat.g22;
   vytab[i].vymatr.v23 = mat.g23;

   vytab[i].vymatr.v31 = mat.g31;
   vytab[i].vymatr.v32 = mat.g32;
   vytab[i].vymatr.v33 = mat.g33;
/*
***Vyn är icke skapad med betraktelseposition.
*/
   vytab[i].vytypp = FALSE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXacvi(
        char  name[],
        DBint win_id)

/*      Interface-rutin för ACT_VIEW. Aktiverar en vy.
 *
 *      In: name   => Vyns namn.
 *          win_id => ID för grafiskt fönster.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *
 *      (C)microform ab 1/8/86 R. Svedin
 *
 *      7/10/86  Tagit bort repagm, R. Svedin.
 *      15/10/86 gpswin(), J. Kjellander
 *      18/10/86 gpgwin(), J. Kjellander
 *      29/10/86 uppdatering av vytab, J. Kjellander
 *      20/12/94 Multipla fönster, J. Kjellander
 *      2006-12-31 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {

#ifdef UNIX
   return(WPacvi(name,win_id));
#endif

#ifdef WIN32
   return(msacvi(name,win_id));
#endif
  }

/********************************************************/
/*!******************************************************/

        short   EXscvi(
        char    name[],
        DBfloat scl)

/*      Interface-rutin för SCL_VIEW().
 *      Måste göras om !!!!!!! Man kan inte skala en namngiven
 *      vy i vytab eftersom vytab inte har några fönsterstorlekar.
 *      Man kan skala den vy som är aktiv i ett visst fönster.
 *      MBS SCL_VIEW(vynamn,skala) borde alltså ersättas med
 *      SCL_VIEW(fönster_id,skala);
 *
 *      OBS-OBS-OBS: Tills vidare ändrar vi skalan i GWIN_MAIN
 *      utan att uppdatera vytab.
 *
 *      In: name => Vyns namn.
 *          scl  => Ny skala.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1692 = Kan ej minska skalan till begärt värde
 *                EX1702 = Kan ej skapa fönster.
 *
 *      (C)2006-12-31, J.Kjellander
 *
 ******************************************************!*/

  {
   double  mdx,mdy,xmin,xmax,ymin,ymax;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Check for invalid scale factor.
*/
   if ( scl <= 1e-10 ) return(erpush("EX1702",""));
/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Size of current window.
*/
   mdx   =  gwinpt->geo.psiz_x *
           (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   mdy   =  gwinpt->geo.psiz_y *
           (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Vi har nu en ny skalfaktor och kan beräkna ett nytt modellfönster.
*/
   xmin =  gwinpt->vy.modwin.xmin + 
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 -
                     mdx/2.0/scl;
   xmax =  gwinpt->vy.modwin.xmin +
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 + 
                     mdx/2.0/scl;
   ymin =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 -
                     mdy/2.0/scl;
   ymax =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 +
                     mdy/2.0/scl;
/*
***Save previous and activate new scale.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.modwin.xmin = xmin;
   gwinpt->vy.modwin.xmax = xmax;
   gwinpt->vy.modwin.ymin = ymin;
   gwinpt->vy.modwin.ymax = ymax;
   WPnrgw(gwinpt);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcevi(
        char    name[],
        DBfloat x,
        DBfloat y)

/*      Interface-rutin för CEN_VIEW. Sätter ett nytt centrum.
 *      OBS !!!!!!! Samma som SCL_VIEW(), se ovan.
 *
 *      In: name  => Vyns namn.
 *          x     => X-koord för nytt centrum.
 *          y     => Y-koord för nytt centrum.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1692 = Kan ej minska skalan till begärt värde
 *                EX1702 = Kan ej skapa fönster.
 *
 *      (C)2006-12-31 J.Kjellander
 *
 ******************************************************!*/

  {
    double  xmin,ymin,xmax,ymax,
            new_xmin,new_ymin,new_xmax,new_ymax;
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***The current window limits.
*/
   xmin = gwinpt->vy.modwin.xmin;
   xmax = gwinpt->vy.modwin.xmax;
   ymin = gwinpt->vy.modwin.ymin;
   ymax = gwinpt->vy.modwin.ymax;
/*
***Calculate new limits.
*/
   new_xmin = x - (xmax - xmin)/2.0; 
   new_xmax = x + (xmax - xmin)/2.0; 
   new_ymin = y - (ymax - ymin)/2.0; 
   new_ymax = y + (ymax - ymin)/2.0; 
/*
***Save previous and activate new limits.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.modwin.xmin = new_xmin;
   gwinpt->vy.modwin.xmax = new_xmax;
   gwinpt->vy.modwin.ymin = new_ymin;
   gwinpt->vy.modwin.ymax = new_ymax;
   WPnrgw(gwinpt);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXhdvi(
        char      name[],
        bool      flag1,
        bool      flag2,
        FILE     *pfil,
        DBVector *origo)

/*      Interface routine for HIDE_VIEW(). 
 *      To be changed to support any window.
 *
 *      In: name  => Vyns namn
 *          flag1 => TRUE = Rita på skärm
 *          flag2 => TRUE = Rita till fil
 *          pfil  => Filpekare
 *          origo => Pekare till nollpunkt eller NULL
 *
 *      Ut: Inget.
 *
 *      FV:      0 = Ok.
 *          AVBRYT = <CTRL>c i WPhide().
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *
 *      (C)microform ab 21/3/89 J. Kjellander
 *
 *      29/5/91  Origo, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBVector nollp;
    WPWIN   *winptr;
    WPGWIN  *gwinpt;
    WPGRPT   projpos;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Transformera origo till basic och sedan till aktiv vy och
***beräkna avstånd till vy:ns nedre vänstra hörn. Om origo = NULL
***ta nedre vänstra hörnet på aktiv vy.
*/
    if ( origo != NULL )
      {
      if ( lsyspk != NULL ) GEtfpos_to_local(origo,&lklsyi,origo);

      WPppos((WPGWIN *)winptr->ptr,origo,&projpos);
      origo->x_gm = projpos.x;
      origo->y_gm = projpos.y;
      origo->z_gm = 0.0;

      nollp.x_gm = origo->x_gm - gwinpt->vy.modwin.xmin;
      nollp.y_gm = origo->y_gm - gwinpt->vy.modwin.ymin;
      nollp.z_gm = 0.0;
      }
    else nollp.x_gm = nollp.y_gm = nollp.z_gm = 0.0;
/*
***Make hide in the main window.
*/
    status = WPhide((WPGWIN *)winptr->ptr,flag1,flag2,pfil,&nollp);
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short EXprvi(
        char    name[],
        DBfloat dist)

/*      Interface-rutin för PERP_VIEW. Ändrar perspektiv-
 *      avstånd.
 *
 *      In: name  => Vyns namn.
 *          dist  => Nytt avstånd. 0 = Parallellprojektion.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1682 = Vy med detta namn finns ej.
 *                EX1922 = Avstånd < 0.
 *
 *      (C)2006-12-31 J.Kjellander
 *
 ******************************************************!*/

 {
    WPWIN  *winptr;
    WPGWIN *gwinpt;

/*
***Check distance for validity.
*/
    if ( dist < 0.0 ) return(erpush("EX1922",""));
/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Save previous and activate new perspective distance.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.vydist = dist;
/*
***The end.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short EXervi(DBint win_id)

/*      Interface-rutin för ERASE_VIEW(). Suddar fönster.
 *
 *      In: win_id => Fönsterid.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      20/1-95  Multifönster, J. Kjellander
 *      2006-12-29 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Erase requested graphic window.
*/
    WPergw(win_id);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrpvi(
        DBint autoz,
        DBint win_id)

/*      Interface-rutin för REP_VIEW(). Genererar en ny bild.
 *
 *      In: autoz  => Autozoom = 1 annars 0.
 *          win_id => Fönster att rita om.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      7/10/86  Bytt till repagm, R. Svedin
 *      1/1/95   Multifönster, J. Kjellander
 *      1996-12-19 WIN32, J.Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***X-Windows.
*/
#ifdef UNIX
    if ( autoz == 1 )
      {
      status = WPmaut(win_id);
      return(status);
      }
    else
      {
      WPrepa(win_id);
      return(0);
      }
#endif

/*
***Microsoft Windows.
*/
#ifdef WIN32
    if ( autoz == 1 )
      {
      status = (short)msmaut(win_id);
      return(status);
      }
    else
      {
      msrepa(win_id);
      return(0);
      }
#endif
  }

/********************************************************/
/*!******************************************************/

        short EXgvnl(
        char  *namptr[],
        DBint *numptr)

/*      Returnerar en lista med alla definierade vyers namn.
 *
 *      In: namptr => Pekare till utdata.
 *          numptr => Pekare till utdata.
 *
 *      Ut: *namptr = Pekare till vynamn.
 *          *numptr = Antal definierade vyer.
 *
 *      (C)microform ab 1998-11-25 J.Kjellander
 *
 ******************************************************!*/

  {
   int  i,n;

   for ( i=n=0; i<GPMAXV; ++i )
     {
     if ( vytab[i].vynamn[0] != '\0' )
       {
       namptr[n++] = vytab[i].vynamn;
       }
     }

   *numptr = n;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcavi(DBfloat newcn)


/*      Interface-rutin för CACC_VIEW(). Ändra kurvnogranhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  EX1712 = Felaktig kurvnogranhet.
 *
 *      (C)microform ab 4/8/86 R. Svedin
 *
 ******************************************************!*/

  {
/*
*** Ändra kurvnogranheten.
*/
    if (newcn < 0.01 || newcn > 100.0 ) return(erpush("EX1712",""));

    WPset_cacc(newcn);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXplvi(
        char *vynam,
        char *filnam)

/*      Interface routine for PLOT_VIEW().
 *      Skapar plottfil av en namngiven vy.
 *
 *      In: vynam  = Pekare till vynamn.
 *          filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  EX1682 = Vyn %s finns ej.
 *                 EX1742 = Kan ej skapa plotfil, fel från OS.
 *
 *      (C)microform ab 4/11/86 R. Svedin
 *
 *      5/11/86 gpstvi(), R. Svedin
 *      7/5/87  plotorigo, J. Kjellander
 *      17/2/88 plotvy, J. Kjellander
 *      2006-12-27 WPmkpf(), J.Kjellander
 *
 ******************************************************!*/

  {
    short    vynum;
    DBVector origo;
    FILE    *filpek;
    WPWIN   *winptr;
    WPGWIN  *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Vilken vy ?
*/
    if ( (vynum=vyindx(vynam)) == -1 )
      {
      erpush("EX1682",vynam);
      goto error;
      }
/*
***Skapa meta_fil.
*/
    if ( (filpek=fopen(filnam,"w+")) == NULL )
      {
      erpush("EX1742","");
      goto error;
      }

    origo.x_gm = origo.y_gm = 0.0;

    if ( strcmp(filnam+strlen(filnam)-4,DXFEXT) == 0 )
      WPdxf_out(gwinpt,filpek,&vytab[vynum],&origo);
    else
      WPmkpf(gwinpt,filpek,&vytab[vynum],&origo);

    fclose(filpek);
    goto exit;
/*
***Fel.
*/
error:
    errmes();
/*
***The end.
*/
exit:
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short     EXplwi(
        DBVector *p1,
        DBVector *p2,
        char     *filnam,
        DBVector *p0)

/*      Interface routine for PLOT_WIN().
 *      Creates A plotfile of restricted area.
 *
 *      In: p1,p2  = Bounding rectangle.
 *          filnam = Ptr to output filename.
 *          p0     = Plot origin.
 *
 *      Felkoder:  EX1852 = Felaktigt plotfönster.
 *                 EX1742 = Kan ej skapa plotfil, fel från OS.
 *
 *      (C)microform ab 17/2/88 J. Kjellander
 *
 *      11/10/88 Bug, plotorigo. J. Kjellander
 *      2/10/91  Bug, DXF-origo. J. Kjellander
 *      2006-12-27 WPmkpf(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBVector origo;
    VY       plotvy;
    FILE    *filpek;
    WPWIN   *winptr;
    WPGWIN  *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Set up the plot area.
*/
    plotvy.vywin[0] = p1->x_gm;
    plotvy.vywin[1] = p1->y_gm;
    plotvy.vywin[2] = p2->x_gm;
    plotvy.vywin[3] = p2->y_gm;
/*
***Cretate GKS_meta_file or DXF_file.
*/
    if ( (filpek=fopen(filnam,"w+")) == NULL )
      {
      erpush("EX1742","");
      goto error;
      }

    if ( strcmp(filnam+strlen(filnam)-4,DXFEXT) == 0 )
      {
      origo.x_gm = p0->x_gm;
      origo.y_gm = p0->y_gm;
      WPdxf_out(gwinpt,filpek,&plotvy,&origo);
      }
    else
      {
      origo.x_gm = p0->x_gm - p1->x_gm;
      origo.y_gm = p0->y_gm - p1->y_gm;
      WPmkpf(gwinpt,filpek,&plotvy,&origo);
      }

    fclose(filpek);
    goto exit;
/*
***Fel.
*/
error:
    errmes();
/*
***The end.
*/
exit:
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short vyindx(char vynamn[])

/*
 *      In: Vynamn.
 *
 *      Ut: Inget.
 *
 *      FV:  Noll eller större => Vyns plats i vytab.
 *           -1 => Vyn finns ej.
 *
 *      (C)microform ab 22/7/86 R. Svedin
 *
 ******************************************************!*/

  {
    short i;

    for ( i = 0; i<GPMAXV; ++i)
      if ( strcmp(vynamn,vytab[i].vynamn) == 0 ) return(i);
  
    return(-1);

  }

/********************************************************/
