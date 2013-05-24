/*!******************************************************************/
/*  File: ig22.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igload();     Loads new job                                     */
/*  iginjb();     Creates new jobdata                               */
/*  igldjb();     Loads jobdata                                     */
/*  igsvjb();     Saves jobdata                                     */
/*  iginmo();     Creates new module                                */
/*  igldmo();     Loads module                                      */
/*  igsvmo();     Saves module                                      */
/*  igingm();     Creates new result                                */
/*  igldgm();     Loads result                                      */
/*  igsvgm();     Saves result                                      */
/*  igsjpg();     Saves all                                         */
/*  igsaln();     Saves all with new name                           */
/*  igspmn();     Saves module with new name                        */
/*  igsgmn();     Saves result with new name                        */
/*  igsjbn();     Saves jobdata with new name                       */
/*  igcatt();     Change module attribute                           */
/*  igcmpc();     Change module protection code                     */
/*  v3exit();     Exits                                             */
/*  igexsn();     Exit without saving                               */
/*  igexsa();     Exit with saving                                  */
/*  igexsd();     Exit with saving and decompiling                  */
/*  ignjsd();     Save, decompile and new job                       */
/*  ignjsa();     Save and new job                                  */
/*  igsjsa();     Save and continue                                 */
/*  ignjsn();     New job without saving                            */
/*  igselj();     Select job from list                              */
/*  igchjn();     Change name of current job                        */
/*  iggrst();     Returns resource value                            */
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
#include "../../GE/include/GE.h"
#include "../include/IG.h"
#include "../include/debug.h"
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include "../../EX/include/EX.h"
#include <string.h>

char   tmprit[V3PTHLEN+1];

/* Namn på temporär .RIT-fil under körning. */

extern pm_ptr   actmod,pmstkp;
extern bool     tmpref,rstron,igxflg,igbflg,jnflag;
extern char     pidnam[],jobnam[],jobdir[],actcnm[];
extern short    modtyp,modatt,v3mode,menlev,stalev,actfun,
                rmarg,bmarg,igtrty,gptrty,posmod,igmatt,igmtyp;
extern gmflt    rstrox,rstroy,rstrdx,rstrdy;
extern DBptr    msysla,lsysla;
extern DBseqnum snrmax;
extern V3MDAT   sydata;
extern V3MSIZ   sysize;
extern VY       vytab[],actvy;
extern tbool    nivtb1[];
extern NIVNAM   nivtb2[];
extern DBTmat  *lsyspk,*msyspk;
extern DBTmat   lklsys,lklsyi,modsys;
extern V2NAPA   defnap;


extern char *mktemp();

/*
***Internal routines.
*/
static short iginjb();
static short igldjb();
static short igsvjb();
static short iginmo();
static short getmta(short *typ, short *att);
static short igsvmo();
static short igingm();
static short igldgm();
static short igsvgm();
static short init_macro();
static short newjob_macro();
static short exit_macro();

/*!******************************************************/

        short igload()

/*      Laddar ett jobb.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/11/85 J. Kjellander
 *
 *      12/10/88 iginmo först om ftabnr=4, J. Kjellander
 *      21/10/88 CGI, J. Kjellander
 *      28/2/92  omflag, J. Kjellander
 *      28/1/95  Multifönster, J. Kjellander
 *      1997-03-11 igupcs(), J.Kjellander
 *      1998-03-12 init/newjob_macro, J.Kjellander
 *      1999-04-23 Cray, J.Kjellander
 *      1999-06-05 igbflg, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    bool   newjob;

/*
***När denna rutin anropas är menyerna laddade men
***ingen output har ännu gjorts till skärmen. Initiera GP
***nu så att ev. felmeddelanden etc. från laddning av job,
***modul och resultat kan skrivas ut även på system där
***alfa-output går genom grafiken. GP initierat behövs
***också innan jobfiler laddas så att skärm-proportioner
***kan sättas upp riktigt i aktiv vy.
*/
    if ( gpinit(iggtts(350)) < 0 ) goto errend;
/*
***Om ritmodulen aktiv, initiera PM först och ladda sedan
***jobbfilen. PM-initiering medför ju återställning av
***alla attribut. Om detta görs efter laddning av job-filen
***skrivs attribut lagrade i jobbfilen över av iginmo().
*/
    if ( v3mode == RIT_MOD )
      {
      if ( iginmo() < 0 ) goto errend;
      pmgstp(&pmstkp);
/*
***Ladda jobbfil.
*/
      if ( (status=igldjb()) == -1 ) iginjb();
      else if ( status < 0 ) goto errend;
/*
***Ladda ritfil.
*/
      if ( (status=igldgm()) == -1 )
        {
        if ( igingm() < 0 ) goto errend;
        newjob = TRUE;
        }
      else if ( status < 0 ) goto errend;
      else newjob = FALSE;
      }
/*
***Om basmodulen aktiv, gör tvärtom.
*/
    else
      {
      if ( (status=igldjb()) == -1 ) iginjb();
      else if ( status < 0 ) goto errend;

      if ( (status=igldmo()) == -1 )
        {
        status = iginmo();
        if      ( status == REJECT ) return(REJECT);
        else if ( status == GOMAIN ) return(GOMAIN);
        else if ( status < 0 ) goto errend;
        newjob = TRUE;
        }
      else
        {
        if (status < 0 ) goto errend;
        newjob = FALSE;
        }

      if ( v3mode & BAS_MOD )
        {
        if ( modtyp == 2 ) v3mode = BAS2_MOD;
        else v3mode = BAS3_MOD;
        }
/*
***Ladda ev. resultatfil. Om resultatfil saknas men modulfil
***fanns kanske vi ska börja med att köra modulen.
*/
      if ( (status=igldgm()) == -1 )
        {
        if ( igingm() < 0 ) goto errend;
        if ( !newjob )
          {
          if ( igxflg || igialt(118,67,68,TRUE) )
            {
            igramo();
            if ( igbflg ) return(igexsn());
            }
          }
        }
      else if ( status == 0  &&  igbflg )
        {
        igramo();
        return(igexsn());
        }
      else if ( status < 0 ) goto errend;
      }
/*
***Nu är det dags att köra ev. init_macro.
*/
   if ( init_macro() < 0 )
     {
     errmes();
     goto errend;
     }
/*
***Om det är ett helt nytt jobb ska vi kanske köra ett
***newjob_macro.
*/
   if ( newjob )
     {
     if ( iggrst("newjob_macro",NULL) )
       {
       if ( newjob_macro() < 0 )
         {
         errmes();
         goto errend;
         }
       goto end;
       }
     }
/*
***Rita om skärmen.
*/
#ifdef V3_X11
    wpwait(GWIN_ALL,TRUE);
    EXdral(GWIN_ALL);
    if ( rstron ) gpdrrs(rstrox,rstroy,rstrdx,rstrdy);
    igupcs(lsysla,V3_CS_ACTIVE);
    wpwait(GWIN_ALL,FALSE);
    igrsma();
#endif

#ifdef WIN32
    msrepa(GWIN_ALL);
    if ( rstron ) gpdrrs(rstrox,rstroy,rstrdx,rstrdy);
    igupcs(lsysla,V3_CS_ACTIVE);
    igrsma();
#endif
/*
***Kör vi WIN32 måste vi ansluta rätt huvudmeny innan
***vi slutar.
*/
end:

#ifdef WIN32
    mssmmu((int)iggmmu());
#endif
/*
***Ev. demo-utskrift.
*/
#ifdef V3_DEMO
    igdemo();
#endif

    return(0);
/*
***Felutgång.
*/
errend:
    gpexit();
    return(EREXIT);
  }

/********************************************************/
/*!******************************************************/

static short iginjb()

/*      Initierar (skapar) ett nytt jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Alltid = 0.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      22/10/85 Släck ej def. nivåer, J. Kjellander
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      5/3/88   stalev, J. Kjellander
 *      15/11/88 posmod, J. Kjellander
 *      31/1/95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    short status,i,y,m,d,h,min,s;
    VYVEC bpos;

/*
***Initiera "jobb skapat datum" i sydata.
*/
    EXtime(&y,&m,&d,&h,&min,&s);
    sydata.year_c = y;
    sydata.mon_c  = m;
    sydata.day_c  = d;
    sydata.hour_c = h;
    sydata.min_c  = min;
/*
***Initiera vytab genom att sätta 1:a och sista tecknet i
***vynamnet till 'Ö0'. Sista tecknet finns alltid för 
***säkerhets skull.
*/
    for (i=0; i<GPMAXV; ++i) 
      {
      vytab[i].vynamn[0]= '\0';
      vytab[i].vynamn[GPVNLN]= '\0';
      }
/*
***Kör vi X11 är det nu dags att skapa grafiska fönster
***samt default vy och aktivera denna i samtliga fönster.
*/
#ifdef V3_X11
     if ( gptrty == X11 )
       if ( (status=wpcgws(TRUE)) < 0 ) return(status);
#endif
#ifdef WIN32
     if ( (status=(short)mscdgw(TRUE)) < 0 ) return(status);
#endif
/*
***Utan X11 är det bara att skapa och aktivera default vy.
*/
    if ( gptrty != X11  &&  gptrty != MSWIN )
      {
      bpos.x_vy = 0.0;
      bpos.y_vy = 0.0;
      bpos.z_vy = 1.0;
      EXcrvp("xy",&bpos);
      EXacvi("xy",0);
      }
/*
***Initiera diverse flaggor.
*/
    tmpref = FALSE;
    posmod = 0;
/*
***Initiera nivåer.
*/
    for ( i=0; i<NT1SIZ; ++i) nivtb1[i] = FALSE;
    for ( i=0; i<NT2SIZ; ++i) nivtb2[i].nam[0] = '\0';
/*
***Initiera koordinatsystem. Modulens system = BASIC och
***inget lokalt system aktivt.
*/
    msyspk = NULL;
    msysla = DBNULL;

    lsyspk = NULL;
    lsysla = DBNULL;

    EXmsini();                     /* Stackpekaren */
    strcpy(actcnm,iggtts(223));    /* Globala */
/*
***Initiera raster.
*/
    rstrox = 0.0;
    rstroy = 0.0;
    rstrdx = 10.0;
    rstrdy = 10.0;
    rstron = FALSE;
/*
***Statusarea på.
*/
    stalev = 1;
/*
***Nytt jobb påbörjat.
*/
    igwtma(211);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igldjb()

/*      Laddar jobb från jobbfil om den finns.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:   0 = Ok.
 *           -1 = Filen finns ej.
 *          < -1 = Status från EXldjb()
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/11/86  V1.3, J. Kjellander
 *      26/12/86 hit och save, J. Kjellander
 *      15/11/88 EXldjb() och posmod, J. Kjellander
 *      30/1-95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
   short  status;
   char   filnam[V3PTHLEN+1];

/*
***Bilda filnamn och prova att ladda.
*/
   strcpy(filnam,jobdir);
   strcat(filnam,jobnam);
   strcat(filnam,JOBEXT);
   status = EXldjb(filnam,(short)0);
/*
***Om status < 0 och felkod = EX1862 finns filen inte.
*/
   if ( status < 0  &&  erlerr() == 186 )
     {
     erinit();
     return(-1);
     }
/*
***Annars om status < 0, felmeddelande.
*/
   else if ( status < 0 ) return(status);
/*
***Kör vi X11 och inga fönster har skapats fanns det inga
***fönster i jobfilen. Då skapar vi default fönster enl.
***resursfil nu.
*/
#ifdef V3_X11
     if ( gptrty == X11  &&  wpngws() == 0 )
       if ( (status=wpcgws(FALSE)) < 0 ) return(status);
#endif
#ifdef WIN32
     if ( msngws() == 0 )
       {
       if ( (status=(short)mscdgw(FALSE)) < 0 ) return(status);
       }
#endif
/*
***Initiera koordinatsystem.
*/
   if ( msyspk != NULL ) msyspk = &modsys;

   if ( lsyspk != NULL )
     {
     lsyspk = &lklsys;
     GEtform_inv(&lklsys,&lklsyi);
     }

   EXmsini();
/*
***Aktivera aktiv vy.
*/
   if ( gptrty != X11  &&  gptrty != MSWIN ) EXacvi(actvy.vynamn,0);
/*
***Jobbfil inläst.
*/
   igwtma(212);

   return(0);
  }

/********************************************************/
/*!******************************************************/

static short igsvjb()

/*      Lagrar ett jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0133 => Kan ej lagra jobbfil.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      30/9/86  Ny nivåhantering, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      16/11/86 V1.3, J. Kjellander
 *      17/19/88 Anrop av gemansam exerutin R. Svedin
 *
 ******************************************************!*/

  {
    char   filnam[V3PTHLEN+1];

/*
***Skapa filnamn och öppna filen.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,jobnam);
    strcat(filnam,JOBEXT);
/*
***Skriv ut aktiva jobb-data till fil.
*/
    if ( EXsvjb(filnam) < 0 ) return(erpush("IG0133",filnam));
/*
***Jobbfil lagrad.
*/
    igwtma(218);
    return(0);
  }

/********************************************************/
/*!******************************************************/

static short iginmo()

/*      Initierar (skapar) ny modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0143 => Kan ej skapa modul
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      29/10/85 Ny pmcmod, J. Kjellander
 *      7/10/86  Time, J. Kjellander
 *      2/3/92   igmtyp/igmatt, J. Kjellander
 *      1996-02-26 Krypterat serienummer, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,y,m,d,h,min,s;
    PMMODULE modhed;
    VYVEC    bpos;

/*
***Ta reda på den blivande modulens typ och attribut.
*/
    if ( (status=getmta(&modtyp,&modatt)) < 0 ) return(status);
/*
***Skapa default vy.
*/
    if ( modtyp == _3D  &&  gptrty != X11 )
      {
      bpos.x_vy = -1.0; bpos.y_vy =  0.0; bpos.z_vy =  0.0;
      EXcrvp("zy",&bpos);
      bpos.x_vy =  0.0; bpos.y_vy =  1.0; bpos.z_vy =  0.0;
      EXcrvp("zx",&bpos);
      bpos.x_vy =  1.0; bpos.y_vy =  1.0; bpos.z_vy =  1.0;
      EXcrvp("iso",&bpos);
      }
/*
***Nollställ och initiera PM.
*/
    if ( pmclea() < 0 ) goto error;
    if ( incrts() < 0 ) goto error;
    inrdnp();
/*
***Initiera ett modulhuvud.
*/
    modhed.parlist = (pm_ptr)NULL;
    modhed.stlist = (pm_ptr)NULL;
    modhed.idmax = 0;
    modhed.ldsize = 0;
    modhed.system.sernr = sydata.sernr;
    modhed.system.vernr = sydata.vernr;
    modhed.system.revnr = sydata.revnr;
    modhed.system.level = sydata.level;

    EXtime(&y,&m,&d,&h,&min,&s);

    modhed.system.year_c = y;
    modhed.system.mon_c  = m;
    modhed.system.day_c  = d;
    modhed.system.hour_c = h;
    modhed.system.min_c  = min;

    modhed.system.year_u = 0;
    modhed.system.mon_u  = 0;
    modhed.system.day_u  = 0;
    modhed.system.hour_u = 0;
    modhed.system.min_u  = 0;

    strcpy(modhed.system.sysname,sydata.sysname);
    strcpy(modhed.system.release,sydata.release);
    strcpy(modhed.system.version,sydata.version);

    modhed.system.mpcode = 0;
    modhed.system.ser_crypt = sydata.ser_crypt;
/*
***Skapa modulen.
*/
    modhed.mtype = (char)modtyp;
    modhed.mattri = (char)modatt;
    if (pmcmod(jobnam,&modhed,&actmod) < 0 ) goto error;
/*
***Initiera största sekvensnummer.
*/
    snrmax = 0;
/*
***Initiera koordinatsystem. Modulens system = BASIC och inget
***loaklt aktivt.
*/
  if ( v3mode & BAS_MOD )
    {
    lsyspk = NULL;
    lsysla = DBNULL;

    msyspk = NULL;
    msysla = DBNULL;

    EXmsini();
    strcpy(actcnm,iggtts(223));
/*
***Modul skapad.
*/
    igwtma(308);
    }

    return(0);
/*
***Felutgång.
*/
error:
    return(erpush("IG0143",""));
  }

/********************************************************/
/*!******************************************************/

 static short getmta(
        short *typ,
        short *att)

/*      Tar reda på vilken typ och vilket attribut
 *      en ny modul skall ha.
 *
 *      In: typ = Pekare till utdata.
 *          att = Pekare till utdata.
 *
 *      Ut: *typ = _2D eller _3D.
 *          *att = LOCAL, GLOBAL eller BASIC
 *
 *      FV: 0, REJECT eller GOMAIN.
 *
 *      (C)microform ab 8/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    short alt;

/*
***I ritmodulen är alla "moduler" DRAWING och BASIC.
*/
  if ( v3mode == RIT_MOD )
    {
   *typ = _2D;
   *att = BASIC;
    }
/*
***I basmodulen kan en modul ha typen DRAWING (_2D) eller
***GEOMETRY (_3D).
*/
  else
    {
#ifdef WIN32
    if ( igmtyp == IGUNDEF  ||  igmatt == IGUNDEF )
      return(msdl01(typ,att));
    else
      {
     *att = igmatt;
     *typ = igmtyp;
      return(0);
      }
#endif
    if ( igmtyp == IGUNDEF )
      {
      igexfu( 144, &alt);
      switch ( alt )
        {
        case 1:
       *typ = _3D;
        break;

        case 2:
       *typ = _2D;
        break;

        case REJECT:
        return(REJECT);

        case GOMAIN:
        return(GOMAIN);

        default:
        return(-1);
        }
      }
    else *typ = igmtyp;
/*
***Attribut kan vara LOCAL, GLOBAL eller BASIC..
*/
    if ( igmatt == IGUNDEF )
      {
      igexfu( 145, &alt);
      switch ( alt )
        {
        case 1:
       *att = LOCAL;
        break;
 
        case 2:
       *att = GLOBAL;
        break;

        case 3:
       *att = BASIC;
        break;

        case REJECT:
        return(REJECT);

        case GOMAIN:
        return(GOMAIN);

        default:
        return(-1);
        }
      }
    else *att = igmatt;
    }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igldmo()

/*      Laddar modul från fil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *              -1 => Filen finns ej.
 *          IG0153 => Kan ej ladda modulen
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 ******************************************************!*/

  {
    PMMODULE modhed;

/*
***Laddning av modul.
*/
    if ( pmclea() < 0 ) goto error;
    if ( incrts() < 0 ) goto error;

    if ( pmload(jobnam, &actmod) < 0 )
      {
      if ( erlerr() == 201 )
        {
        erinit();
        return(-1);
        }
      else goto error;
      }
/*
***Läs modulhuvud och sätt modtyp och modattr därefter.
*/
    pmrmod(&modhed);
    modtyp = modhed.mtype;
    modatt = modhed.mattri;
/*
***Initiera största sekvensnummer.
*/
    snrmax = modhed.idmax;
/*
***Interpretera modulens parameterlista.
*/
    igevpl();
/*
***Modul inläst.
*/
    igwtma(213);

    return(0);
/*
***Felutgång.
*/
error:
    return(erpush("IG0153",jobnam));
  }

/********************************************************/
/*!******************************************************/

static short igsvmo()

/*      Lagra PM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG0173 = Systemfel vid lagring.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      7/10/86  Time, J. Kjellander
 *
 ******************************************************!*/

  {
    short    y,m,d,h,min,s;
    PMMODULE modhed;

/*
***Uppdatera modulhuvudet.
*/
    pmrmod(&modhed);

    EXtime(&y,&m,&d,&h,&min,&s);
    modhed.system.year_u = y;
    modhed.system.mon_u  = m;
    modhed.system.day_u  = d;
    modhed.system.hour_u = h;
    modhed.system.min_u  = min;

    pmumod(&modhed);
/*
***Lagra modulfil.
*/
    if ( pmsave(actmod) < 0 ) return(erpush("IG0173",jobnam));
/*
***Modulfil lagrad.
*/
    igwtma(216);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igingm()

/*      Skapar resultafil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0183 => Kan ej skapa resultatfil
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      29/11/88   Temporärfil, J. Kjellander
 *      1999-02-09 Ny gminit(), J.Kjellander
 *
 ******************************************************!*/

  {
    char filnam[V3PTHLEN+1],templ[JNLGTH+10];

    strcpy(filnam,jobdir);

    if ( v3mode == RIT_MOD )
      {
      strcpy(templ,jobnam);
      strcat(templ,".XXXXXX");
      strcat(filnam,mktemp(templ));
      strcpy(tmprit,filnam);
      }
    else
      {
      strcat(filnam,jobnam);
      strcat(filnam,RESEXT);
      }

    if ( DBinit(filnam,sysize.gm,
                DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 )
                                      return(erpush("IG0183",filnam));
/*
***Resultatfil skapad.
*/
    igwtma(309);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igldgm()

/*      Laddar resultafil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  0 => Ok.
 *                -1 => Kan ej ladda resultat.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 *      14/11/85   Bug, J. Kjellander
 *      5/4/86     Ny felhantering, J. Kjellander
 *      29/11/88   Temporärfil, J. Kjellander
 *      1999-02-09 Ny felhantering, J.Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1],templ[JNLGTH+10];
    short status;

/*
***Bilda filnamn utan extension.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,jobnam);
/*
***Ritpaketet, kolla om filen finns. Finns den,
***kopieras den till en temporärfil och temporärfilen
***laddas. Finns den inte returneras -1.
*/
    if ( v3mode == RIT_MOD )
      {
      strcat(filnam,RITEXT);
      if ( v3ftst(filnam) )
        {
        strcpy(tmprit,jobdir);
        strcpy(templ,jobnam);
        strcat(templ,".XXXXXX");
        strcat(tmprit,mktemp(templ));
        v3fcpy(filnam,tmprit);
        status = DBload(tmprit,sysize.gm,
                    DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
        if ( status < 0 )
          {
          errmes();       /* Allvarligare fel */
          return(status);
          }
        else
          {
          igwtma(140);    /* Laddning gick bra */
          return(0);
          }
        }
      else return(-1);    /* Filen finns ej */
      }
/*
***Basmodulen ! Prova att ladda gammal resultatfil.
***Status =  0 => Filen har laddats.
***Status = -1 => Filen finns ej.
***Status < -1 => Allvarligare fel, tex. filen tom.
*/
    else
      {
      strcat(filnam,RESEXT);
      status = DBload(filnam,sysize.gm,
                      DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);

      if ( status == 0 )
        {
        igwtma(214);                        /* Filen laddad */
        return(0);
        }
      else if ( status == -1 ) return(-1);  /* Filen finns ej */
      else
        {
        errmes();                           /* Allvarligare fel */
        return(-1);
        }
      }
  }

/********************************************************/
/*!******************************************************/

static short igsvgm()

/*      Lagra resultatfil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0193 => Kan ej lagra resultatfilen
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 ******************************************************!*/

  {
    char  filnam[V3PTHLEN+1];

/*
***Lagra resultatfil.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));

/*
***Resultatfil lagrad.
*/
    if ( v3mode & BAS_MOD ) igwtma(217);
/*
***Om ritpaketet, kopiera den temporära arbetsfilen
***till en .RIT-fil.
*/
    else
     {
     strcpy(filnam,jobdir);
     strcat(filnam,jobnam);
     strcat(filnam,RITEXT);
     v3fcpy(tmprit,filnam);
     v3fdel(tmprit);
     igwtma(141);
     }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igsjpg()

/*      Lagra jobb, PM och GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/10/85 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lagra jobbet.
*/
    if ( igsvjb() < 0 ) errmes();
/*
***Lagra modul.
*/
    if ( v3mode & BAS_MOD  &&  igsvmo() < 0 ) errmes();
/*
***Lagra resultat.
*/
    if ( igsvgm() < 0 ) errmes();

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short igsaln()

/*      Lagrar allt med nytt namn.
 *
 *      FV: 0      = OK
 *          REJECT = Avsluta
 *          GOMAIN = Huvudmenyn
 *
 *      Felkod: IG0342 = Otillåtet jobnamn
 *              IG0422 = Nytt namn = aktuellt namn
 *
 *      (C)microform ab 1998-09-16 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    char     resfil[V3PTHLEN+1];
    char     newfil[V3PTHLEN+1];
    char     tmpnam[JNLGTH+1];
    char     path[V3PTHLEN+1];
    bool     flag;
    PMMODULE modhed;

    static char newnam[JNLGTH+1] = "";

/*
***Läs in nytt jobbnamn.
*/
    igptma(210,IG_INP);     
    status = igssip(iggtts(267),newnam,newnam,JNLGTH);
    igrsma();
    if ( status < 0 ) return(status);
/*
***Kolla att namnet är ok.
*/
   if ( igckjn(newnam) < 0 )
     {
     erpush("IG0342",newnam);
     errmes();
     goto exit;
     }
/*
***Kolla att det nya namnet inte är lika med aktuellt jobbnamn.
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0422",newnam);
      errmes();
      goto exit;
      }
/*
***Kolla om det redan finns ett jobb med det angivna namnet.
*/
    flag = FALSE;

    strcpy(path,jobdir);
    strcat(path,newnam);
    strcat(path,JOBEXT);
    if ( v3ftst(path) ) flag = TRUE;

    if ( v3mode == !RIT_MOD )
      {
      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,MODEXT);
      if ( v3ftst(path) ) flag = TRUE;

      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,RESEXT);
      if ( v3ftst(path) ) flag = TRUE;
      }
   else
     {
      strcpy(path,jobdir);
      strcat(path,newnam);
      strcat(path,RITEXT);
      if ( v3ftst(path) ) flag = TRUE;
      }

    if ( flag  &&  !igialt(1626,67,68,TRUE) ) goto exit;
/*
***Lagra jobfil.
*/
    strcpy(tmpnam,jobnam);
    strcpy(jobnam,newnam);
    if ( igsvjb() < 0 ) errmes();
    strcpy(jobnam,tmpnam);
/*
***Lagra ev. modul. Ändra namnet i modulhuvudet tillfälligt
***och skriv ut.
*/
    if ( v3mode != RIT_MOD )
      {
      pmrmod(&modhed);
      strcpy(modhed.mname,newnam);
      pmumod(&modhed);

      if ( igsvmo() < 0 ) errmes();

      pmrmod(&modhed);
      strcpy(modhed.mname,jobnam);
      pmumod(&modhed);
      }
/*
***Lagra GM.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Kopiera den lagrade pagefilen till en fil med det nya namnet.
*/
    if ( v3mode == RIT_MOD ) strcpy(resfil,tmprit);
    else
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);
      } 
 
    strcpy(newfil,jobdir);
    strcat(newfil,newnam);
    if ( v3mode == RIT_MOD ) strcat(newfil,RITEXT);
    else strcat(newfil,RESEXT);
/*
***Kopiera filen.
*/
    if ( (status=v3fcpy(resfil,newfil)) < 0 )
      return(status);
    else
      {
      if ( v3mode == RIT_MOD ) igwtma(141);
      else igwtma(217);
      }
/*
***Öppna GM igen.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
    igrsma();
    igptma(196,IG_MESS);     
/*
***Slut.
*/
exit:
    return(0);
  }

/********************************************************/
/********************************************************/

        short igspmn()

/*      Lagra modul med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 24/11/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      10/10/86 default, J. Kjellander
 *
 ******************************************************!*/

  {
    short      status;
    char       newnam[JNLGTH+1];
    PMMODULE   modhed;

/*
***Läs in nytt filnamn.
*/
    igptma(349,IG_INP);
    if ( (status=igssip(iggtts(267),newnam,jobnam,JNLGTH)) < 0 )
        goto exit;
/*
***Ändra namnet i modulhuvudet.
*/
    pmrmod(&modhed);
    strcpy(modhed.mname,newnam);
    pmumod(&modhed);
/*
***Lagra modul.
*/
    if ( igsvmo() < 0 ) errmes();
/*
***Ändra tillbaks namnet i modulhuvudet.
*/
    pmrmod(&modhed);
    strcpy(modhed.mname,jobnam);
    pmumod(&modhed);

exit:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igsgmn()

/*      Lagra GM med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG0082 = Resultatfilens namn = jobbnamn
 *
 *      (C)microform ab 30/7/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      2/2/93   copy på VAX, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    resfil[V3PTHLEN+1];
    char    newfil[V3PTHLEN+1];
    char    newnam[JNLGTH+1];
#ifdef VMS
    char    oscmd[2*(V3PTHLEN+1)+20];
#endif

/*
***Läs in nytt filnamn.
*/
loop:
    igptma(279,IG_INP);     
    status = igssip(iggtts(267),newnam,"",JNLGTH);
    igrsma();
    if ( status < 0 ) return(status);
/*
***Kolla att det nya namnet inte är lika med aktuellt jobbnamn.
*/
    if ( strcmp(newnam,jobnam) == 0 )
      {
      erpush("IG0082","");
      errmes();
      goto loop;
      }
/*
***Lagra GM.
*/
    if ( DBexit() < 0 ) return(erpush("IG0193",jobnam));
/*
***Kopiera den lagrade pagefilen till en fil med det nya namnet.
*/
    else
      {
      if ( v3mode == RIT_MOD ) strcpy(resfil,tmprit);
      else
        {
        strcpy(resfil,jobdir);
        strcat(resfil,jobnam);
        strcat(resfil,RESEXT);
        } 
 
      strcpy(newfil,jobdir);
      strcat(newfil,newnam);
      if ( v3mode == RIT_MOD ) strcat(newfil,RITEXT);
      else strcat(newfil,RESEXT);
/*
***Kopiera filen, på VAXEN kan inte v3fcpy() användas.
*/
#ifdef UNIX
      if ( (status=v3fcpy(resfil,newfil)) < 0 )
        return(status);
      else
        {
        if ( v3mode == RIT_MOD ) igwtma(141);
        else igwtma(217);
        }
      }
#endif

#ifdef WIN32
      if ( (status=v3fcpy(resfil,newfil)) < 0 )
        return(status);
      else
        {
        if ( v3mode == RIT_MOD ) igwtma(141);
        else igwtma(217);
        }
      }
#endif

#ifdef VMS
      strcpy(oscmd,"copy ");
      strcat(oscmd,resfil);
      strcat(oscmd," ");
      strcat(oscmd,newfil);
      EXos(oscmd,(short)0);
      if ( v3mode == RIT_MOD ) igwtma(141);
      else igwtma(217);
      }
#endif
/*
***Öppna GM igen.
*/
    DBload(resfil,sysize.gm,
           DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
    igrsma();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igsjbn()

/*      Lagra jobbfil med nytt namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 11/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
    short      status;
    char       newnam[JNLGTH+1];
    char       tmpnam[JNLGTH+1];

/*
***Läs in nytt filnamn.
*/
    igptma(357,IG_INP);
    if ( (status=igssip(iggtts(267),newnam,jobnam,JNLGTH)) < 0 )
        goto exit;
/*
***Lagra jobb.
*/
    strcpy(tmpnam,jobnam);
    strcpy(jobnam,newnam);
    if ( igsvjb() < 0 ) errmes();
    strcpy(jobnam,tmpnam);

exit:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcatt()

/*      Ändra modulens attribut.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 24/11/85 J. Kjellander
 *
 *      9/10/86  GOMAIN, J. Kjellander
 *      1/3/94   Snabbval, J. Kjellander
 *
 ******************************************************!*/

  {
    short     alt=-1,alttyp;
    MNUALT   *pmualt;
    PMMODULE  modhed;

/*
***Ta reda på nytt attribut.
*/
#ifdef WIN32
    msshmu(145);
#else
    igaamu(145);
#endif

l1:
    iggalt(&pmualt,&alttyp);

#ifdef WIN32
    mshdmu();
#endif

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        igsamu();
        return(REJECT);

        case SMBMAIN:
        return(GOMAIN);
        }
      }
    else alt = pmualt->actnum;

    switch ( alt )
      {
      case 1:
      modatt = LOCAL;
      break;

      case 2:
      modatt = GLOBAL;
      break;

      case 3:
      modatt = BASIC;
      break;

      default:                           /* Okänt alt. */
      erpush("IG0103","");
      errmes();
      goto l1;
      }
/*
***Uppdatera modulhuvudet.
*/
    pmrmod(&modhed);
    modhed.mattri = (char)modatt;
    pmumod(&modhed);
/*
***Uppdatera statusarean.
*/
    igupsa();
    igsamu();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igcmpc()

/*      Ändra modulens skyddskod.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG0092 = Rättighet saknas
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short    status=0;
    int      value;
    PMMODULE modhed;

/*
***Kolla om det är tillåtet att ändra.
*/
    pmrmod(&modhed);
    if ( modhed.system.sernr != sydata.sernr ) 
      {
      erpush("IG0092","");
      errmes();
      goto exit;
      }
/*
***Ta reda på ny skyddskod.
*/
    igptma(355,IG_INP);
    if ( (status=igsiip(iggtts(4),&value)) < 0 ) goto exit;
/*
***Uppdatera modulhuvudet.
*/
    modhed.system.mpcode = (short)value;
    pmumod(&modhed);
/*
***Slut.
*/
exit:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short v3exit()

/*      Avslutningsrutin. Används av toppnivån (futab4)
 *      och trap-rutiner för att återvända till OS.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 13/4/86 J. Kjellander
 *
 *      25/9/95  X11, J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Stäng surpac.
*/
   suexit();
/*
***Här stänger vi fönsterhanteringssystemet.
*/
#ifdef V3_X11
   if ( igtrty == X11 ) wpexit();
#endif

#ifdef WIN32
   msexit();
#endif
/*
***Stäng IGE.
*/
   igwtma(219);
   igexit();
/*
***Återställ terminalporten.
*/
   igextt();
/*
***Stäng ev. debug-filer.
*/
   dbgexi();
/*
***Återvänd till OS.
*/
#ifdef WIN32
   ExitProcess(0);
#else
   exit(V3EXOK);
#endif
/*
***För att slippa kompileringsvarning.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igexsn()

/*      Sluta utan att lagra.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  v3exit, J. Kjellander
 *      29/11/88 Temporär .RIT-fil, J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
    char resfil[V3PTHLEN+1];

/*
***Först av allt, ett ev. exit_macro.
*/
    exit_macro();
/*
***Stäng GM och ta bort resultatfilen.
*/
    gmclpf();

    if ( v3mode == RIT_MOD ) v3fdel(tmprit);
    else
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);
      v3fdel(resfil);
      }
/*
***Avsluta.
*/
    gpexit();
    return(EXIT);
  }

/********************************************************/
/*!******************************************************/

        short igexsa()

/*      Sluta och lagra allt. f122.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      10/2/86  gpexit, J. Kjellander
 *      13/4/86  v3exit, J. Kjellander
 *      26/9/95  jnflag, J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char  newnam[JNLGTH+1];

/*
***Om inget riktigt jobnamn ännu har definierats
***frågar vi om detta nu och byter namn på jobbet.
*/
   if ( !jnflag )
     {
     igptma(193,IG_INP);
     if ( (status=igssip(iggtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);

     if ( igchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***Kör ev. exit_macro.
*/
   exit_macro();
/*
***Lagra allt.
*/
   igsjpg();
/*
***Avsluta.
*/
   gpexit();
   return(EXIT);
  }

/********************************************************/
/*!******************************************************/

        short igexsd()

/*      Sluta och lagra allt, även dekompilerad
 *      version av aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 19/4/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Dekompilera.
*/
    if ( prtmod() < 0 ) return(0);
/*
***Sluta och lagra.
*/
    else return(igexsa());
  }

/********************************************************/
/*!******************************************************/

        short ignjsd()

/*      Nytt jobb och lagra allt, även dekompilerad
 *      version av aktiv modul.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 9/8/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Dekompilera.
*/
    if ( prtmod() < 0 ) return(0);
/*
***Sluta och lagra.
*/
    else return(ignjsa());
  }

/********************************************************/
/*!******************************************************/

        short ignjsa()

/*      Lagra allt och nytt jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 16/6/85 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      26/9/95  igselj(), J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   oldnam[JNLGTH+1];

/*
***Om inget riktigt jobnamn ännu har definierats
***frågar vi om detta nu och byter namn på jobbet.
*/
   if ( !jnflag )
     {
     igptma(193,IG_INP);
     if ( (status=igssip(iggtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);

     if ( igchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***Läs in nytt jobnamn.
*/
   status = igselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Kör ev. exit_macro.
*/
    exit_macro();
/*
***Lagra allt.
*/
    igsjpg();
/*
***Lagra nya namnet men spara först det gamla så att
***vi kan byta tillbaks om det inte går att ladda det
***nya jobbet.
*/
    strcpy(oldnam,jobnam);
    strcpy(jobnam,newnam);
/*
***Ladda/skapa nytt jobb, ny modul och nytt resultat.
***Om det inte går eller avbryts av användaren laddar
***vi tillbaks det gamla jobbet igen.
*/
    gpexit();

    status = igload();

    if ( status < 0 )
      {
      if ( status != REJECT  &&  status != GOMAIN ) errmes();
      strcpy(jobnam,oldnam);
      gpexit();
      if ( igload() < 0 ) return(EREXIT);
      else return(status);
      }
    else return(GOMAIN);
  }

/********************************************************/
/*!******************************************************/

        short igsjsa()

/*      Lagra allt och fortsätt.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 28/9/95 J. Kjellander
 *
 *      1998-02-06 WIN32, J.Kjellander
 *
 ******************************************************!*/

  {
   short  status;
   char   newnam[JNLGTH+1],ritfil[V3PTHLEN+1];

/*
***Om inget riktigt jobnamn ännu har definierats
***frågar vi om detta nu och byter namn på jobbet.
*/
   if ( !jnflag )
     {
     igptma(193,IG_INP);
     if ( (status=igssip(iggtts(400),newnam,"",JNLGTH)) < 0 )
        return(status);
     igrsma();

     if ( igchjn(newnam) < 0 )
       {
       errmes();
       return(0);
       }
     }
/*
***Lagra JOB-fil.
*/
   if ( igsvjb() < 0 ) errmes();
   igrsma();
/*
***Kanske även MBO-fil.
*/
   if ( v3mode & BAS_MOD )
     {
     if ( igsvmo() < 0 ) errmes();
     igrsma();
     }
/*
***Och kanske även RIT-fil.
*/
   if ( v3mode == RIT_MOD )
     {
     if ( DBexit() < 0 )
       {
       erpush("IG0193",jobnam);
       errmes();
       return(0);
       }

     strcpy(ritfil,jobdir);
     strcat(ritfil,jobnam);
     strcat(ritfil,RITEXT);

#ifdef UNIX
     if ( v3fcpy(tmprit,ritfil) < 0 )
       {
       errmes();
       return(0);
       }
#endif

#ifdef WIN32
     if ( v3fcpy(tmprit,ritfil) < 0 )
       {
       errmes();
       return(0);
       }
#endif

#ifdef VMS
     strcpy(oscmd,"copy ");
     strcat(oscmd,tmprit);
     strcat(oscmd," ");
     strcat(oscmd,ritfil);
     EXos(oscmd,(short)0);
#endif
/*
***Öppna GM igen.
*/
     if ( DBload(tmprit,sysize.gm,
          DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 ) errmes();
     }
/*
***Å så ett litet meddelande.
*/
   igwtma(196);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short ignjsn()

/*      Lagra inget och nytt jobb.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      (C)microform ab 6/10/86 J. Kjellander
 *
 *      8/5/87   Defaultsträng, J. Kjellander
 *      26/9/95  tmprit, J. Kjellander
 *      26/9/95  igselj(), J. Kjellander
 *      1998-03-12 exit_macro, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    char   newnam[JNLGTH+1];
    char   resfil[V3PTHLEN+1];

/*
***Läs in nytt jobnamn.
*/
   status = igselj(newnam);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Kör ev. exit_macro.
*/
    exit_macro();
/*
***Lagra inget.
*/
   gmclpf();

   if ( v3mode & BAS_MOD )
     {
     strcpy(resfil,jobdir);
     strcat(resfil,jobnam);
     strcat(resfil,RESEXT);
     v3fdel(resfil);
     }
   else v3fdel(tmprit);
/*
***Lagra nya namnet. Eftersom detta namn är givet av användaren
***sätter vi nu jnflaggan till true.
*/
    strcpy(jobnam,newnam);
    jnflag = TRUE;
/*
***Ladda/skapa nytt jobb, ny modul och nytt resultat.
*/
    gpexit();

    status = igload();

    if (      status == REJECT ) return(EXIT);
    else if ( status == GOMAIN ) return(EXIT);
    else if ( status < 0 ) return(EREXIT);
    else return(GOMAIN);
  }

/********************************************************/
/*!******************************************************/

        short igselj(char *newjob)

/*      Interaktiv funktion för att välja jobb.
 *
 *      In: newjob = Pekare till utdata.
 *
 *      Ut: *newjob = Jobbnamn eller odefinierat.
 *
 *      FV:  0      = Ok.
 *           REJECT = Avbryt.
 *          -1      = Kan ej skapa pipe till "ls".
 *
 *      Felkoder: IG0342 = %s är ett otillåtet jobbnamn.
 *                IG0442 = Kan ej öppna pipe %s
 *
 *      (C)microform ab 25/9/95 J. Kjellander
 *
 *      1998-11-03 actfun, J.Kjellander
 *
 ******************************************************!*/

  {
   short status,oldafu;
   char *pekarr[1000],strarr[20000];
   char  typ[5],mesbuf[V3STRLEN+1];
   int   i,nstr,actalt;

/*
***Skapa filförteckning.
*/
   if ( v3mode & BAS_MOD ) strcpy(typ,MODEXT);
   else                    strcpy(typ,RITEXT);

   igdir(jobdir,typ,1000,20000,pekarr,strarr,&nstr);
/*
***Vilket av dem är aktivt ?
*/
   for ( i=0; i<nstr; ++i ) if ( strcmp(pekarr[i],jobnam) == 0 ) break;

   if ( i < nstr ) actalt =  i;
   else            actalt = -1;
/*
***Aktiv funktion, specialare för hjälp-systemet.
*/
   oldafu = actfun;
   actfun = 1001;
/*
***Låt användaren välja.
*/
   sprintf(mesbuf,"%s - %s ",pidnam,iggtts(210));

   if ( nstr > 0 )
     {
#ifdef V3_X11
     if ( igtrty == X11 )
       status = wpilse(20,20,mesbuf,"",pekarr,actalt,nstr,newjob);
     else
       {
       igplma(mesbuf,IG_INP);
       status=igssip(iggtts(210),newjob,"",JNLGTH);
       igrsma();
       }
#else
#ifdef WIN32
     status = msilse(20,20,mesbuf,"",pekarr,actalt,nstr,newjob);
#else
     igplma(mesbuf,IG_INP);
     status=igssip(iggtts(210),newjob,"",JNLGTH);
     igrsma();
#endif
#endif
     }
/*
***Om det inte finns några jobb att välja mellan räcker det
***med en enkel fråga.
*/
   else
     {
     igplma(mesbuf,IG_INP);
     status=igssip(iggtts(210),newjob,"",JNLGTH);
     igrsma();
     }

   actfun = oldafu;

   if ( status <  0 ) return(status);
/*
***Han kan ha matat in ett JOB-namn från tangentbordet så det är
***bäst att kolla att det följer reglerna.
*/
   if ( igckjn(newjob) < 0 ) return(erpush("IG0342",newjob));
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igchjn(char *newnam)

/*      Ändrar namn på aktivt jobb.
 *
 *      In: newnam = Nytt jobbnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0342 = Jobnamnet %s är ej tillåtet
 *                IG0422 = Jobbet finns redan
 *
 *      (C)microform ab 26/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
    char       oldres[V3PTHLEN+1],newres[V3PTHLEN+1],
               filnam[V3PTHLEN+1],templ[JNLGTH+10];
    PMMODULE   modhed;

/*
***Kolla att det är ett tillåtet namn.
*/
    if ( igckjn(newnam) < 0 ) return(erpush("IG0342",newnam));
/*
***Kolla at inte ett jobb med detta namn finns redan.
*/
    strcpy(filnam,jobdir);
    strcat(filnam,newnam);
    strcat(filnam,JOBEXT);
    if ( v3ftst(filnam) ) return(erpush("IG0422",newnam));

    if ( v3mode & BAS_MOD )
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,MODEXT);
      if ( v3ftst(filnam) ) return(erpush("IG0422",newnam));

      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,RESEXT);
      if ( v3ftst(filnam) ) return(erpush("IG0422",newnam));
      }
    else
      {
      strcpy(filnam,jobdir);
      strcat(filnam,newnam);
      strcat(filnam,RITEXT);
      if ( v3ftst(filnam) ) return(erpush("IG0422",newnam));
      }
/*
***Kör vi basmodulen skall vi ändra namnet i modulhuvudet....
*/
    if ( v3mode & BAS_MOD )
      {
      pmrmod(&modhed);
      strcpy(modhed.mname,newnam);
      pmumod(&modhed);
/*
***samt byta namn på resultatfilen.
*/
      strcpy(oldres,jobdir);
      strcat(oldres,jobnam);
      strcat(oldres,RESEXT);
      strcpy(newres,jobdir);
      strcat(newres,newnam);
      strcat(newres,RESEXT);
      v3fmov(oldres,newres);
      }
/*
***Kör vi ritmodulen räcker det att byta namn på temporärfilen.
*/
    else
      {
      strcpy(templ,newnam);
      strcat(templ,".XXXXXX");
      mktemp(templ);

      strcpy(newres,jobdir);
      strcat(newres,templ);
      v3fmov(tmprit,newres);
      strcpy(tmprit,newres);
      }
/*
***Sist av allt byter vi aktivt jobnamn.
*/
   strcpy(jobnam,newnam);
   jnflag = TRUE;
/*
***Och uppdaterar fönsterramen.
*/
#ifdef V3_X11
   if ( igtrty == X11 ) wpupwb(NULL);
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool iggrst(
        char *resurs,
        char *pvalue)

/*      OS-oberoende version av wpgrst/msgrst. Returnerar
 *      värdet på en resurs om den är definierad. 
 *
 *      In: resurs = Resursnamn utan "varkon."
 *          pvalue = Pekare till utdata eller NULL.
 *
 *      Ut: *pvalue = Resursvärde om tillgängligt.
 *
 *      FV: TRUE  = Resursen finns.
 *          FALSE = Resursen finns ej.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char value[1000];
   bool status;

/*
***X-resurser heter samma sak som WIN32-resurser
***men med ordet varkon framför.
*/
#ifdef V3_X11
   char xrmnam[V3STRLEN];

   strcpy(xrmnam,"varkon.");
   strcat(xrmnam,resurs);
   status = wpgrst(xrmnam,value);
#endif

/*
***WIN32-resurser heter samma sak som i igepac.
*/
#ifdef WIN32
   status = msgrst(resurs,value);
#endif
/*
***Skall vi returnera resursvärde ?
*/
   if ( status )
     {
     if ( pvalue != NULL ) strcpy(pvalue,value);
     }
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

 static short init_macro()

/*      Kör macro när jobb laddas oavsett om det är
 *      ett nytt jobb som skapas eller ett gammalt
 *      som laddas.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char name[V3PTHLEN];

   if ( iggrst("init_macro",name) )
     {
     name[JNLGTH] = '\0';
     return(igcpts(name,MFUNC));
     }
   else return(0);
   }

/********************************************************/
/*!******************************************************/

 static short newjob_macro()

/*      Kör macro bara när nytt jobb skapas.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   char name[V3PTHLEN];

   if ( iggrst("newjob_macro",name) )
     {
     name[JNLGTH] = '\0';
     return(igcpts(name,MFUNC));
     }
   else return(0);
   }

/********************************************************/
/*!******************************************************/

 static short exit_macro()

/*      Kör macro vid avslut.
 *
 *      FV:  0 = OK.
 *          <0 = Fel.
 *
 *      (C)microform ab 1998-03-12 J. Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char  name[V3PTHLEN];

   if ( iggrst("exit_macro",name) )
     {
     name[JNLGTH] = '\0';
     status = igcpts(name,MFUNC);
     if ( status < 0 )
       {
       errmes();
       igialt(457,458,458,TRUE);
       }
     return(status);
     }
   else return(0);
   }

/********************************************************/
