/*!******************************************************************/
/*  igstatem.c                                                      */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGcges();    Generic create geometrical statement               */
/*  IGcprs();    Generic create procedure call statement            */
/*  IGgnid();    Generate new ID sequencenumber                     */
/*  IGedst();    Edit MBS statement                                 */
/*  IGanrf();    Analyzes reference dependencies                    */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../EX/include/EX.h"
#include "../../AN/include/AN.h"
#include "../../WP/include/WP.h"

extern short    v3mode,modtyp;
extern pm_ptr   actmod;
extern DBseqnum snrmax;
extern DBptr    lsysla;
extern char     actcnm[];
extern struct   ANSYREC sy;

/*!******************************************************/

       short IGcges(
       char  *typ,
       pm_ptr pplist)

/*      Creates, executes and adds a geometric statement
 *      to end of active module.
 *
 *      In: typ    => Typ av geometrisats, tex. "POI-FREE"
 *          pplist => Pekare till parameterlista.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5213 = Fel fr�n pmcges i IGcges
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5233 = Fel fr�n pmlmst i IGcges
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *      2007-07-28 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    char   mesbuf[2*V3STRLEN],mbsbuf[V3STRLEN];
    pmseqn geid;
    pm_ptr retla,ref;
    stidcl kind;
/*
***Create a new unused ID.
*/
    geid = IGgnid();
/*
***Mark current PM-stack pointer.
*/
    pmmark();
/*
***Create the statement.
*/
    stlook(typ,&kind,&ref);
    if ( pmcges(ref,geid,pplist,(pm_ptr)NULL,&retla) < 0)
         return(erpush("IG5213",typ));
/*
***Try to execute.
*/
    if ( inssta(retla) < 0) 
      {
      pmrele();
      return(erpush("IG5222",typ));
      }
/*
***Everything ok, add statement to end of module. In RIT-mode,
***reset PM stack pointer.
*/
    if ( v3mode & BAS_MOD )
      {
      if ( pmlmst(actmod, retla) < 0 ) return(erpush("IG5233",typ));
      }
    else
      {
      pmrele();
      }
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    strcpy(mesbuf,IGgtts(58));
    pprsts(retla,modtyp,mbsbuf,V3STRLEN);
    strcat(mesbuf,mbsbuf);
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short IGcprs(
       char  *typ,
       pm_ptr pplist)

/*      Creates, executes and adds a procedure statement
 *      to end of active module.
 *
 *      In: typ    => Typ av procedursats, tex. "MODE_LOCAL"
 *          pplist => Pekare till parameterlista.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG5253 = Fel fr�n pmcges i IGcprs
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5263 = Fel fr�n pmlmst i IGcprs
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      15/3/88    Ritpaketet, J. Kjellander
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *      2007-07-28 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    char   mesbuf[2*V3STRLEN],mbsbuf[V3STRLEN];
    pm_ptr retla,ref;
    stidcl kind;

/*
***Mark current PM-stack pointer.
*/
    pmmark();
/*
***Create the statement.
*/
    stlook( typ, &kind, &ref);
    if ( pmcprs(ref,pplist,&retla) < 0 ) return(erpush("IG5253",typ));
/*
***Try to execute.
*/
    if ( inssta(retla) < 0)
       {
       pmrele();
       return(erpush("IG5222",typ));
       }
/*
***Everything ok, add procedure call to end of module. In RIT-mode,
***reset PM stack pointer.
*/
    if ( v3mode & BAS_MOD )
      {
      if ( pmlmst(actmod, retla) < 0 ) return(erpush("IG5263",typ));
      }
    else
      {
      pmrele();
      }
/*
***Confirmational message.
*/
    strcpy(mesbuf,IGgtts(58));
    pprsts(retla,modtyp,mbsbuf,V3STRLEN);
    strcat(mesbuf,mbsbuf);
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       DBseqnum IGgnid()

/*      Genererar nytt sekvensnummer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Nytt sekvensnummer.
 *
 *      (C)microform ab 7/11/85 J. Kjellander
 *
 *      1/2/86  anrop till gmrdid() �ndrat, J. Kjellander
 *      13/3/86 Slopat ERASED, J. Kjellander
 *      15/3/88 Ritpaketet, J. Kjellander
 *      1998-04-02 Kolla �ven GM, J.Kjellander
 *
 ******************************************************!*/

  {
    DBseqnum idmax;

/*
***Om ritmodulen aktiv, returnera GM:s idmax + 1.
*/
    if ( v3mode == RIT_MOD )
      {
      DBget_highest_id(&idmax);
      if ( idmax == -1 ) idmax = 0;
      return(idmax+1);
      }
/*
***Om basmodulen aktiv, returnera snrmax + 1.
***Om detta id redan �r anv�nt (eller suddat) i GM b�r vi
***inte anv�nda det. S� kan det tex. bli om man laddar ett
***GM fr�n fil och sen b�rjar jobba vidare.
*/
    else
      {
    ++snrmax;
      DBget_highest_id(&idmax);
      while ( snrmax <= idmax  && 
              gmrdid(DBNULL,(DBseqnum)snrmax) != (DBptr)NOTUSD ) ++snrmax;
      return(snrmax);
      }
  }
  
/********************************************************/
/*!******************************************************/

        short IGedst()

/*      f168 = funktion f�r att editera en geometri-
 *      eller part-sats p� MBS-format.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 *      2007-01-05 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    char     oldstr[V3STRLEN+1],newstr[V3STRLEN+1];
    char     oldpar[V3STRLEN+1],newpar[V3STRLEN+1];
    char     promt[V3STRLEN+1];

    short    status;
    int      nid,i;
    DBetype  typv[1];
    DBptr    csyla;
    pm_ptr   slstla,lstla,nextla,statla,retla;
    PMMONO  *np;
    DBId     idmat[1][MXINIV];
    ANFSET   set;
    DBCsys   csy;
    V2NAPA   oldnap;

/*
***H�mta storhetens id.
*/
    nid = 1; IGptma(268,IG_MESS); typv[0] = ALLTYP;
    status = IGgmid(idmat,typv,&nid);
    IGrsma();
    if ( status < 0 )
      {
      WPerhg();
      return(status);
      }
    else
      {
      idmat[0][0].ord_val = 1;
      idmat[0][0].p_nextre = NULL;
      }
/*
***Ta reda p� det koordinatsystem
***som var aktivt n�r den skapades.
*/
    status = EXgatt((DBId *)idmat,&oldnap,&csyla);
    if ( status < 0 ) goto exit;
/*
***Om csyla =  DBNULL var inget lokalt system aktivt.
***Om lsysla = DBNULL �r inget lokalt system aktivt.
***J�mf�r dom med varandra och meddela vilket som g�ller
***under �ndringen.
*/
    strcpy(promt,IGgtts(116));

    if ( csyla != lsysla )
      {
      if ( csyla != DBNULL )
        {
        DBread_csys(&csy,NULL,csyla);
        strcat(promt,csy.name_pl);
        }
      else strcat(promt,IGgtts(223));
      }
    else strcat(promt,actcnm);

    IGplma(promt,IG_INP);
/*
***Var i PM ligger satsen ? F�rst en C-pekare till aktiv modul.
***Sen en PM-pekare till list-noden f�r utpekad sats.
***Sen en PM-pekare till sj�lva satsen.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = np->pstl_;
    if ( (status=pmlges((DBId *)idmat,&slstla,&lstla)) < 0 ) goto exit;
    if ( lstla == (pm_ptr)NULL ) goto exit;
    if ( (status=pmglin(lstla,&nextla,&statla)) < 0 ) goto exit;
/*
***De-kompilera dito till en str�ng.
*/
    if ( (status=pprsts(statla,modtyp,oldstr,V3STRLEN)) < 0 ) goto exit;
/*
***Klipp ut allt fram till 1:a parametern och g�r det
***till promt, resten blir default-v�rde.
*/
edit:
    for ( i=0; i<(int)strlen(oldstr); ++i ) if ( oldstr[i] == ',' ) break;
    i++;

    strcpy(promt,oldstr); promt[i] = '\0';
    strcpy(oldpar,oldstr+i);
/*
***L�t anv�ndaren editera.
*/
    status = IGssip(promt,newpar,oldpar,V3STRLEN);

    if ( status < 0 )
      {
      IGrsma();
      WPerhg();
      return(status);
      }
/*
***Kopiera ihop promt och newpar till newstr.
*/
   strcpy(newstr,promt);
   strcat(newstr,newpar);
/*
***Notera aktuellt l�ge i PM och initiera scannern.
***Skapa tomt set.
***H�mta f�rsta token. anascan() returnerar ingen status.
***Analysera. anunst() �r en void.
***St�ng scannern.
*/
    pmmark(); anlogi();
    if ( (status=asinit(newstr,ANRDSTR)) < 0 ) goto exit;
    ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
    anascan(&sy);
    anunst(&retla,&set);
    if ( (status=asexit()) < 0 ) goto exit;
/*
***Blev det n�gra fel ?
*/
    if ( anyerr() )
      {
      pmrele();
      erpush("IG3892","");
      errmes();
      strcpy(oldstr,newstr);
      goto edit;
      }
/*
***Finns det n�gra fram�t-referenser i den nya satsen.
*/
    if ( pmargs(retla) == TRUE )
      {
      pmrele();
      erpush("IG3882","");
      errmes();
      strcpy(oldstr,newstr);
      goto edit;
      }
/*
***Byt ut satsen i modulen. lstla = Den gamla satsens listnod
***som returnerats av pmlges(). retla = Den nya satsen som den
***kommer fr�n anunst().
*/
    status = pmrgps(lstla,retla);
    if ( status < 0 ) goto exit;
/*
***Oavsett om storheten �r refererad eller ej provar
***vi att reinterpretera f�r att se om det g�r bra.
*/
    status = EXrist((DBId *)idmat);
/*
***Om det inte gick bra att reinterpretera m�ste vi
***l�nka in den gamla satsen i PM igen s� att allt blir
***som det var fr�n b�rjan.
*/
    if ( status < 0 ) 
      {
      erpush("IG5222","MBS");
      errmes();
      status = pmrgps(lstla,statla);
      if ( status < 0 ) goto exit;
      goto edit;
      }
/*
***Om storheten �r refererad kanske hela modulen skall
***k�ras om.
*/
    if ( pmamir((DBId *)idmat)  &&  IGialt(175,67,68,FALSE) )
      {
      status = IGramo();
      if ( status < 0 )
        { 
        pmrgps(lstla,statla);
        goto edit;
        }
      }
/*
***Slut.
*/
exit:
    IGrsma();
    WPerhg();
    if ( status < 0 ) errmes();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGanrf()

/*      Varkonfunktion f�r att analysera vilka storheter
 *      som refererar till en utpekad storhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i,nid,refant;
    char     buf[80],typstr[20];
    short    status;
    DBetype  typv[IGMAXID];
    PMREFVA  idmat[IGMAXID][MXINIV],id;
    PMREFL  *reftab;
    DBHeader   hdr;
    DBPart    part;
/*
***H�mta id f�r n�gon av storhetens instanser.
*/
    nid = 1;
    IGptma(268,IG_MESS);
    typv[0] = ALLTYP;
    status = IGgmid(idmat,typv,&nid);
    IGrsma();
    if ( status < 0 ) goto exit;
/*
***Test av pmwrme().
*/
    status = pmwrme((DBId *)idmat,&reftab,&refant);
    if ( status < 0 )
      {
      errmes();
      goto exit;
      }
    else
      {
      if ( refant > 1 )
        {
        sprintf(buf," Entity #%d refereced by the following %d entities !",
                      idmat[0][0].seq_val,refant-1);
        WPinla(buf);
  
        for ( i=1; i<refant; ++i )
          {
/*
***Kolla i GM vilken typ av storhet det �r.
*/
          id.seq_val  = reftab[i].snr;
          id.ord_val  = 1;
          id.p_nextre = NULL;
          EXgthd(&id,&hdr);

          switch ( hdr.type )
            {
            case POITYP: strcpy(typstr,"POINT"); break;
            case LINTYP: strcpy(typstr,"LINE"); break;
            case ARCTYP: strcpy(typstr,"ARC"); break;
            case CURTYP: strcpy(typstr,"CURVE"); break;
            case TXTTYP: strcpy(typstr,"TEXT"); break;
            case XHTTYP: strcpy(typstr,"HATCH"); break;
            case LDMTYP: strcpy(typstr,"LINEAR DIMENSION"); break;
            case CDMTYP: strcpy(typstr,"DIAMETER DIMENSION"); break;
            case RDMTYP: strcpy(typstr,"RADIUS DIMENSION"); break;
            case ADMTYP: strcpy(typstr,"ANGULAR DIMENSION"); break;
            case GRPTYP: strcpy(typstr,"GROUP"); break;
            case CSYTYP: strcpy(typstr,"COORDINATE SYSTEM"); break;
            case PRTTYP: strcpy(typstr,"PART"); break;
            case BPLTYP: strcpy(typstr,"B-PLANE"); break;
            case MSHTYP: strcpy(typstr,"MESH"); break;
            default:     strcpy(typstr,"Unknown !"); break;
            }
/*
***Skriv ut.
*/
          sprintf(buf,"%-16s #%-5d",
                       typstr,reftab[i].snr);
          if ( hdr.type == PRTTYP )
            {
            EXgtpt(&id,&part);
            strcat(buf,"Name = ");
            strcat(buf,part.name_pt);
            }
          WPalla(buf,(short)1);
          }
        WPexla(TRUE);
        }
      else
        {
        sprintf(buf," Entity #%d referenced by the following %d entities !",
                      idmat[0][0].seq_val,refant-1);
        WPinla(buf);
        WPalla("Entity not referenced !",(short)1);
        WPexla(TRUE);
        }
      }
/*
***Slut.
*/
exit:
    WPerhg();

    return(status);
  }

/********************************************************/
