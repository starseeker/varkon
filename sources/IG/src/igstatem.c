/*!******************************************************************/
/*  igstatem.c                                                      */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igcges();    Generic create geometrical statement               */
/*  igcprs();    Generic create procedure call statement            */
/*  iggnid();    Generate new ID sequencenumber                     */
/*  igedst();    Edit MBS statement                                 */
/*  iganrf();    Analyzes reference dependencies                    */
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
#include "../../AN/include/AN.h"
#include "../../WP/include/WP.h"

extern short    v3mode,modtyp;
extern pm_ptr   actmod;
extern DBseqnum snrmax;
extern DBptr    lsysla;
extern char     actcnm[];
extern struct   ANSYREC sy;

/*!******************************************************/

       short igcges(
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
 *      Felkod: IG5213 = Fel från pmcges i igcges
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5233 = Fel från pmlmst i igcges
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *
 ******************************************************!*/

  {
    pmseqn geid;
    pm_ptr retla,ref;
    stidcl kind;
/*
***Create a new unused ID.
*/
    geid = iggnid();
/*
***Mark current PM-stack pointer.
*/
    pmmark();
/*
***Create the statement.
*/
    stlook( typ, &kind, &ref);
    if ( pmcges( ref, geid, pplist, (pm_ptr)NULL, &retla) < 0)
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

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short igcprs(
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
 *      Felkod: IG5253 = Fel från pmcges i igcprs
 *              IG5222 = Fel vid interpretering av %s-sats
 *              IG5263 = Fel från pmlmst i igcprs
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      15/3/88    Ritpaketet, J. Kjellander
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *
 ******************************************************!*/

  {
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

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       DBseqnum iggnid()

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
 *      1/2/86  anrop till gmrdid() ändrat, J. Kjellander
 *      13/3/86 Slopat ERASED, J. Kjellander
 *      15/3/88 Ritpaketet, J. Kjellander
 *      1998-04-02 Kolla även GM, J.Kjellander
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
***Om detta id redan är använt (eller suddat) i GM bör vi
***inte använda det. Så kan det tex. bli om man laddar ett
***GM från fil och sen börjar jobba vidare.
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

        short igedst()

/*      Varkonfunktion för att editera en geometri-
 *      eller part-sats på MBS-format.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 ******************************************************!*/

  {
    char     oldstr[V3STRLEN+1],newstr[V3STRLEN+1];
    char     oldpar[V3STRLEN+1],newpar[V3STRLEN+1];
    char     promt[V3STRLEN+1];

    short    status,nid,i,ntkn;
    DBetype  typv[1];
    DBptr    csyla;
    pm_ptr   slstla,lstla,nextla,statla,retla;
    PMMONO  *np;
    DBId     idmat[1][MXINIV];
    ANFSET   set;
    DBCsys    csy;
    V2NAPA   oldnap;

/*
***Hämta storhetens id.
*/
    nid = 1; igptma(268,IG_MESS); typv[0] = ALLTYP;
    status = getmid(idmat,typv,&nid);
    igrsma();
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
***Ta reda på det koordinatsystem
***som var aktivt när den skapades.
*/
    status = EXgatt((DBId *)idmat,&oldnap,&csyla);
    if ( status < 0 ) goto exit;
/*
***Om csyla =  DBNULL var inget lokalt system aktivt.
***Om lsysla = DBNULL är inget lokalt system aktivt.
***Jämför dom med varandra och meddela vilket som gäller
***under ändringen.
*/
    strcpy(promt,iggtts(116));

    if ( csyla != lsysla )
      {
      if ( csyla != DBNULL )
        {
        DBread_csys(&csy,NULL,csyla);
        strcat(promt,csy.name_pl);
        }
      else strcat(promt,iggtts(223));
      }
    else strcat(promt,actcnm);

    igplma(promt,IG_INP);
/*
***Var i PM ligger satsen ? Först en C-pekare till aktiv modul.
***Sen en PM-pekare till list-noden för utpekad sats.
***Sen en PM-pekare till själva satsen.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = np->pstl_;
    if ( (status=pmlges((DBId *)idmat,&slstla,&lstla)) < 0 ) goto exit;
    if ( lstla == (pm_ptr)NULL ) goto exit;
    if ( (status=pmglin(lstla,&nextla,&statla)) < 0 ) goto exit;
/*
***De-kompilera dito till en sträng.
*/
    if ( (status=pprsts(statla,modtyp,oldstr,V3STRLEN)) < 0 ) goto exit;
/*
***Klipp ut allt fram till 1:a parametern och gör det
***till promt, resten blir default-värde.
*/
edit:
    for ( i=0; i<(int)strlen(oldstr); ++i ) if ( oldstr[i] == ',' ) break;
    i++;

    strcpy(promt,oldstr); promt[i] = '\0';
    strcpy(oldpar,oldstr+i);
/*
***Låt användaren editera.
*/
    ntkn = V3STRLEN;
    status = igglin(promt,oldpar,&ntkn,newpar);
    if ( status < 0 )
      {
      igrsma();
      WPerhg();
      return(status);
      }
/*
***Kopiera ihop promt och newpar till newstr.
*/
   strcpy(newstr,promt);
   strcat(newstr,newpar);
/*
***Notera aktuellt läge i PM och initiera scannern.
***Skapa tomt set.
***Hämta första token. anascan() returnerar ingen status.
***Analysera. anunst() är en void.
***Stäng scannern.
*/
    pmmark(); anlogi();
    if ( (status=asinit(newstr,ANRDSTR)) < 0 ) goto exit;
    ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
    anascan(&sy);
    anunst(&retla,&set);
    if ( (status=asexit()) < 0 ) goto exit;
/*
***Blev det några fel ?
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
***Finns det några framåt-referenser i den nya satsen.
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
***kommer från anunst().
*/
    status = pmrgps(lstla,retla);
    if ( status < 0 ) goto exit;
/*
***Oavsett om storheten är refererad eller ej provar
***vi att reinterpretera för att se om det går bra.
*/
    status = EXrist((DBId *)idmat);
/*
***Om det inte gick bra att reinterpretera måste vi
***länka in den gamla satsen i PM igen så att allt blir
***som det var från början.
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
***Om storheten är refererad kanske hela modulen skall
***köras om.
*/
    if ( pmamir((DBId *)idmat)  &&  igialt(175,67,68,FALSE) )
      {
      status = igramo();
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
    igrsma();
    WPerhg();
    if ( status < 0 ) errmes();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short iganrf()

/*      Varkonfunktion för att analysera vilka storheter
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
    int      refant;
    char     buf[80],typstr[20];
    short    status,nid,i;
    DBetype  typv[IGMAXID];
    PMREFVA  idmat[IGMAXID][MXINIV],id;
    PMREFL  *reftab;
    DBHeader   hdr;
    DBPart    part;
/*
***Hämta id för någon av storhetens instanser.
*/
    nid = 1;
    igptma(268,IG_MESS);
    typv[0] = ALLTYP;
    status = getmid(idmat,typv,&nid);
    igrsma();
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
***Kolla i GM vilken typ av storhet det är.
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
