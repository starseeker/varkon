/*!******************************************************************/
/*  igexpr.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGcint();     Genererate integer expression                     */
/*  IGcinv();     Genererate integer literal value                  */
/*  IGcflt();     Genererate float expression                       */
/*  IGcflv();     Genererate float literal value                    */
/*  IGcstr();     Genererate string expression                      */
/*  IGcstv();     Genererate string literal value                   */
/*  IGcstm();     Genererate string litval by menu selection        */
/*  IGcref();     Genererate reference expression                   */
/*  IGcpos();     Genererate vector expression                      */
/*  IGcpov();     Genererate vector literal value                   */
/*  IGgsid();     Returns  ID + type by crosshairs & mask           */
/*  IGgmid();     Returns many ID                                   */
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
#include "../../GE/include/GE.h"
#include "../include/IG.h"
#include "../../AN/include/AN.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <math.h>

extern bool    tmpref;
extern short   v3mode,modtyp,posmode;
extern MNUALT  smbind[];
extern DBTmat *lsyspk;

static short genuv(short pnr, pm_ptr *pexnpt);
static short genrfs(short pnr, DBetype *ptyp, pm_ptr *pexnpt, bool *pend,
                    bool *pright, short utstat);
static short gnpmen(pm_ptr *pexnpt);
static short gnpabs(pm_ptr *pexnpt);
static short gnprel(pm_ptr *pexnpt);
static short gnpcrh(pm_ptr *pexnpt);
static short gnpexp(pm_ptr *pexnpt);
static short gnpend(pm_ptr *pexnpt);
static short gnpon(pm_ptr *pexnpt);
static short gnpint(pm_ptr *pexnpt);
static short gnpcen(pm_ptr *pexnpt);
static short getwid(DBId idmat1[][MXINIV], DBId idmat2[][MXINIV],
                    int *idant1, int idant2, DBetype typvek[]);

static short igpcrh(DBVector *vecptr);
static short igpcrh_3D(DBVector *vecptr);
static short igpcrh_2D(DBVector *vecptr);
static short igpend(DBVector *vecptr);
static short igpon(DBVector *vecptr);
static short igpint(DBVector *vecptr);
static short igpcen(DBVector *vecptr);

/*!******************************************************/

        short   IGcint(
        short   pnr,
        char   *dstr,
        char   *istr,
        pm_ptr *pexnpt)

/*      Huvudrutin f�r att skapa en uttrycksnod f�r
 *      ett heltals-v�rde.
 *
 *      In: pnr    => Promptstr�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          pexnpt => Pekare till pm_ptr.
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *           IG2232 => Fel datatyp
 *
 *      (C)microform ab 17/1/85 M Nelson
 *
 *      30/10/85 Defaultstr�ng, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    sttycl type;

/*
***Skriv ut ev. promtstr�ng.
*/
    if (pnr > 0) IGptma(pnr,IG_INP);
/*
***L�s in v�rde.
*/
loop:
    if ( (status=IGssip(IGgtts(46),istr,dstr,V3STRLEN)) < 0) goto exit;

    if (anaexp(istr,FALSE,pexnpt,&type) != 0)
      {
      errmes();
      goto loop;
      }

    if (type != ST_INT)
        {
        erpush("IG2232",IGgtts(4));
        errmes();
        goto loop;
        }
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short   IGcinv(
        short   pnr,
        char   *istr,
        char   *dstr,
        DBint  *ival)

/*      L�ser in ett heltalsuttryck och ber�knar v�rdet.
 *
 *      In: pnr    => Promptstr�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          ival   => Pekare till resultat.
 *
 *      Ut: *ival  => Heltal.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 29/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt,valtyp;
    PMLITVA val;

/*
***Spara aktuell pm-stack pekare.
*/
    pmmark();
/*
***Generera uttryck.
*/
    if ( (status=IGcint(pnr,dstr,istr,&exnpt)) < 0 ) goto end;
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);

    *ival = val.lit.int_va;
/*
***Slut.
*/
end:
    pmrele();
    return(status);

  }
/********************************************************/
/*!******************************************************/

        short   IGcflt(
        short   pnr,
        char   *dstr,
        char   *istr,
        pm_ptr *pexnpt)

/*      Huvudrutin f�r att skapa en uttrycksnod f�r
 *      ett double-v�rde.
 *
 *      In: pnr    => Promptstr�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          pexnpt => Pekare till pm_ptr.
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *           IG2232 => Fel datatyp
 *
 *      (C)microform ab 17/1/85 J. Kjellander
 *
 *      28/10/85 Defaultstr�ng, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    sttycl type;

/*
***Skriv ut ev. promtstr�ng.
*/
    if (pnr > 0) IGptma(pnr,IG_INP);
/*
***L�s in v�rde.
*/
loop:
    if ( (status=IGssip(IGgtts(204),istr,dstr,V3STRLEN)) < 0 ) goto exit;

    if (anaexp(istr,FALSE,pexnpt,&type) != 0)
       {
       errmes();
       goto loop;
       }

    if ((type != ST_FLOAT) && (type != ST_INT))
        {
        erpush("IG2232",IGgtts(5));
        errmes();
        goto loop;
        }

exit:
    return(status);

  }
/********************************************************/
/*!******************************************************/

        short   IGcflv(
        short   pnr,
        char   *istr,
        char   *dstr,
        double *fval)

/*      L�ser in ett flyttalsuttryck och ber�knar v�rdet.
 *
 *      In: pnr    => Promptstr�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          fval   => Pekare till resultat.
 *
 *      Ut: *fval  => Flyttal.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 29/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt,valtyp;
    PMLITVA val;

/*
***Spara aktuell pm-stack pekare.
*/
    pmmark();
/*
***Generera uttryck.
*/
    if ( (status=IGcflt(pnr,dstr,istr,&exnpt)) < 0 ) goto end;
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) *fval = val.lit.int_va;
    else *fval = val.lit.float_va;
/*
***Slut.
*/
end:
    pmrele();
    return(status);

  }
/********************************************************/
/*!******************************************************/

 static short   genuv(
        short   pnr,
        pm_ptr *pexnpt)

/*      Ett UV-v�rde. Skapar en uttrycksnod
 *      av typen vector.
 *
 *      In:
 *           pnr    => Promtnummer. 
 *           pexnpt => Pekare till utdata.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *             < 0 => Fel fr�n PM.
 *
 *      (C)microform ab 6/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
    char   ps[2][40];
    char   is[2][80];
    char  *psarr[2];
    char  *isarr[2];
    char  *dsarr[2];
    short  ml[2];
    sttycl type;
    char   expr[132];
    short  status;

    static char ds[2][80] = {"0","0"};

/*
***Initiering.
*/
    strcpy(ps[0],IGgtts(182));
    strcpy(ps[1],IGgtts(183));

    ml[0]=80;
    ml[1]=80;

    psarr[0] = ps[0];
    psarr[1] = ps[1];

    isarr[0] = is[0];
    isarr[1] = is[1];

    dsarr[0] = ds[0];
    dsarr[1] = ds[1];
/*
***Skriv ut ev. promtstr�ng.
*/
    if ( pnr > 0 ) IGptma(pnr,IG_INP);
/*
***L�s in v�rden.
*/    
loop:
    if ( (status=IGmsip(psarr,isarr,dsarr,ml,(short)2)) < 0 ) goto exit;
/*
*** Kopiera svaret till defaultstr�ngarna 
*/
    strcpy(ds[0],is[0]); 
    strcpy(ds[1],is[1]);
/*
***Skapa funktionsstr�ngen
*/
    sprintf(expr,"vec(%s,%s)",is[0],is[1]);

    if ( anaexp(expr,FALSE,pexnpt,&type) != 0 )
      {
      errmes();
      goto loop;
      }

    else status = 0;
/*
***Slut.
*/
exit:
    return(status);
    }

/********************************************************/
/*!******************************************************/

        short   IGcstr(
        short   pnr,
        char   *dstr,
        char   *istr,
        pm_ptr *pexnpt)

/*      Huvudrutin f�r att skapa en uttrycksnod f�r
 *      ett str�ng-v�rde. Provar f�rst att tolka
 *      den inl�sta str�ngen som en str�ngvariabel. Om
 *      ingen variabel av klassen parameter och typen str�ng
 *      finns antas str�ngen representera ett explicit
 *      v�rde.
 *
 *      In: pnr    => Promt-str�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          pexnpt => Pekare till pm_ptr.
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 25/2/85  Mats Nelson
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      15/11/85 anaexp, J. Kjellander
 *      3/12/85  erinit, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      23/3/86  Prompt-str�ng B. Doverud
 *      14/4/86  Bug, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    sttycl  type;
    PMLITVA litval;

/*
***Skriv ut ev. promtstr�ng.
*/
    if (pnr > 0) IGptma(pnr,IG_INP);
/*
***L�s in v�rde.
*/
    if ( (status=IGssip(IGgtts(266),istr,dstr,V3STRLEN)) < 0 )
       goto exit;
/*
***Analysera. Om det var ett MBS-str�nguttryck �r allt ok.
*/
    if (anaexp(istr,FALSE,pexnpt,&type) == 0 && type == ST_STR)
      {
      status = 0;
      }
/*
***Inget MBS-str�nguttryck, t�m felstacken och skapa en literal.
*/
    else
      {
      erinit();
      litval.lit_type = C_STR_VA;
      strcpy(litval.lit.str_va,istr);
      pmclie( &litval, pexnpt);
      }

exit:
    return(status);

  }

/********************************************************/
/*!******************************************************/

        short   IGcstv(
        short   pnr,
        char   *istr,
        char   *dstr,
        char   *strval)

/*      L�ser in ett str�nguttryck och ber�knar v�rdet.
 *
 *      In: pnr    => Promptstr�ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstr�ng.
 *          istr   => Pekare till inputstr�ng.
 *          strval => Pekare till resultat.
 *
 *      Ut: *strval => Str�ng.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 29/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    pm_ptr exnpt,valtyp;
    PMLITVA val;

/*
***Spara aktuell pm-stack pekare.
*/
    pmmark();
/*
***Generera uttryck.
*/
    if ( (status=IGcstr(pnr,dstr,istr,&exnpt)) < 0 ) goto end;
/*
***Interpretera uttrycket.
*/
    inevev(exnpt,&val,&valtyp);

    strcpy(strval,val.lit.str_va);
/*
***Slut.
*/
end:
    pmrele();
    return(status);

  }
/********************************************************/
/*!******************************************************/

        short   IGcstm(
        short   mnum,
        pm_ptr *pexnpt)

/*      Genererar ett uttryck best�ende av aktionskod
 *      f�r valt alternativ i en meny.
 *
 *      In: mnum   = Menynummer.
 *          pexnpt = Pekare till resultat.
 *
 *      Ut: *pexnpt = Str�nguttryck.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 11/11/88 J. Kjellander
 *
 *       1/3/94  Snabbval, J. Kjellander
 *
 ******************************************************!*/

  {
    short   alttyp;
    MNUALT *pmualt;
    PMLITVA litval;

/*
***Generera uttryck.
*/
#ifdef WIN32
   msshmu(mnum);
#else
   IGaamu(mnum);
#endif

   IGgalt(&pmualt,&alttyp);

#ifdef WIN32
   mshdmu();
#endif

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        IGsamu();
        return(REJECT);

        case SMBMAIN:
        return(GOMAIN);
        }
      }
    else
      {
      litval.lit_type = C_STR_VA;
      litval.lit.str_va[0] = pmualt->acttyp;
      sprintf(&litval.lit.str_va[1],"%d",pmualt->actnum);
      pmclie( &litval, pexnpt);
      IGsamu();
      return(0);
      }

   return(0);
  }
/********************************************************/
/*!******************************************************/

        short    IGcref(
        short    pnr,
        DBetype *ptyp,
        pm_ptr  *pexnpt,
        bool    *pend,
        bool    *pright)

/*      Huvudrutin f�r att skapa en uttrycksnod f�r
 *      en referens.
 *
 *      In: pnr    => Promt-str�ng nr, om 0 ingen utskrift.
 *          ptyp   => Pekare till DBetype, typmask.
 *          pexnpt => Pekare till pm_ptr.
 *          pend   => Pekare till bool.
 *          pright => Pekare till bool.
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *          *pend   => TRUE om pekning i �nden.
 *          *pright => TRUE om pekning till h�ger.
 *          *typ    => Typ av storhet.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 17/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      31/10/85 �nde och sida, J. Kjellander
 *      23/3/86  Prompt-str�ng B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    PMREFVA idvek[MXINIV];
    PMLITVA litval;

/*
***Skriv ut ev. promtstr�ng.
*/
    if (pnr > 0) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***L�s in ID f�r refererad storhet.
*/
    if ((status=IGgsid(idvek,ptyp,pend,pright,(short)0)) != 0 ) goto exit;
/*
***Skapa en literal value node av typen referens.
*/
    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val = idvek[0].seq_val;
    litval.lit.ref_va[0].ord_val = idvek[0].ord_val;
    litval.lit.ref_va[0].p_nextre = idvek[0].p_nextre;
    pmclie( &litval, pexnpt);

exit:
    WPclear_mcwin();
    return(status);
   }

/********************************************************/
/*!******************************************************/

static  short    genrfs(
        short    pnr,
        DBetype *ptyp,
        pm_ptr  *pexnpt,
        bool    *pend,
        bool    *pright,
        short    utstat)

/*      Huvudrutin f�r att skapa en uttrycksnod f�r
 *      en referens med m�jlighet att till�ta extra
 *      utstatus.
 *
 *      In: pnr    => Promt-str�ng nr, om 0 ingen utskrift.
 *          ptyp   => Pekare till DBetype, typmask.
 *          pexnpt => Pekare till pm_ptr.
 *          pend   => Pekare till bool.
 *          pright => Pekare till bool.
 *          utstat => Utstatus till IGgsid().
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *          *pend   => TRUE om pekning i �nden.
 *          *pright => TRUE om pekning till h�ger.
 *          *typ    => Typ av storhet.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 17/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    PMREFVA idvek[MXINIV];
    PMLITVA litval;

/*
***Skriv ut ev. promtstr�ng.
*/
    if ( pnr > 0 ) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***L�s in ID f�r refererad storhet.
*/
    if ( (status=IGgsid(idvek,ptyp,pend,pright,utstat)) != 0 ) goto exit;
/*
***Skapa en literal value node av typen referens.
*/
    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val = idvek[0].seq_val;
    litval.lit.ref_va[0].ord_val = idvek[0].ord_val;
    litval.lit.ref_va[0].p_nextre = idvek[0].p_nextre;
    pmclie( &litval, pexnpt);

exit:
    if ( pnr > 0 ) WPclear_mcwin();
    return(status);
   }

/********************************************************/
/*!******************************************************/

       short   IGcpos(
       short   pnr,
       pm_ptr *pexnpt)

/*      Huvudrutin f�r att skapa en uttrycks-nod f�r
 *      en position.
 *
 *      In:   pnr    => Promt-str�ng nr, om 0 ingen utskrift.
 *            pexnpt => Pekare till pm_ptr variabel.
 *
 *      Out: *pexnpt => PM-pekare till expression-node.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Tillbaks till huvudmenyn
 *             < 0 => Fel fr�n PM.
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 *      1998-03-31 default, J.Kjellander
 *      2007-07-28 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Output prompt.
*/
start:
    if ( pnr > 0 ) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***Select method.
*/
    switch ( posmode )
      {
      case 0:  status = gnpabs(pexnpt); break;
      case 1:  status = gnprel(pexnpt); break;
      case 2:  status = gnpcrh(pexnpt); break;
      case 3:  status = gnpexp(pexnpt); break;
      case 4:  status = gnpend(pexnpt); break;
      case 5:  status = gnpon(pexnpt);  break;
      case 6:  status = gnpcen(pexnpt); break;
      case 7:  status = gnpint(pexnpt); break;
      case 8:  status = gnpcrh(pexnpt); break;
      default: status = REJECT; break;
      }
/*
***Clear the promt.
*/
    WPclear_mcwin();
/*
***Change pos method maybe ?
*/
    if ( status == SMBPOSM ) goto start;
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short IGcpov(DBVector *pos)

/*      Skapar ett positions-v�rde. Ers�tter getpos().
 *
 *      In: pos    => Pekare till position.
 *
 *      Ut: *pos   => Position.
 *
 *      FV: Returnerar status fr�n IGcpos().
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   pm_ptr  exnpt,valtyp;
   PMLITVA val;

/*
***Spara pm-stack pekaren.
*/
   pmmark();
/*
***Skapa positions-uttryck.
*/
   if ( (status=IGcpos((short)0,&exnpt)) < 0 ) goto end;
/*
***Interpretera.
*/
   inevev(exnpt,&val,&valtyp);

   pos->x_gm = val.lit.vec_va.x_val;
   pos->y_gm = val.lit.vec_va.y_val;
   pos->z_gm = val.lit.vec_va.z_val;
/*
***Slut.
*/
end:
   pmrele();
   return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpmen(pm_ptr *pexnpt)

/*      Skapar en uttrycks-nod f�r en position via val
 *      i positionsmenyn.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression-node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Tillbaks till huvudmenyn
 *             < 0 => Fel fr�n PM.
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 *       1/3/94  Snabbval, J. Kjellander
 *       1998-03-31 H�rkors i 3D, J.Kjellander
 *       1998-10-20 �ven f�r WIN32, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,alttyp,alt=-1;
    MNUALT *pmualt;

/*
***Skriv ut meny och l�s in svar, snabbval ej till�tet.
*/
#ifdef WIN32
l1:
    msshmu(141);
#else
    IGaamu(141);
l1:
#endif

    IGgalt(&pmualt,&alttyp);

#ifdef WIN32
    mshdmu();
#endif

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        IGsamu();
        return(REJECT);

        case SMBMAIN:
        return(GOMAIN);

        case SMBPOSM:
        WPbell();
        goto l1;
        }
      }
    else alt = pmualt->actnum;

    switch(alt)
      {
      case 1:                            /* Absolut */
      IGptma(326,IG_INP);
      status=gnpabs(pexnpt);
      IGrsma();
      break;

      case 2:                            /* Relativt */
      status=gnprel(pexnpt);
      break;

      case 3:                            /* H�rkors */
      status=gnpcrh(pexnpt);
      break;

      case 4:                            /* Uttryck */
      status=gnpexp(pexnpt);
      break;

      case 5:                            /* �nde av storhet */
      status=gnpend(pexnpt);
      break;

      case 6:                            /* P� en storhet */
      status=gnpon(pexnpt);
      break;

      case 7:                            /* Centrum av storhet */
      status=gnpcen(pexnpt);
      break;

      case 8:                            /* Sk�rning */
      status=gnpint(pexnpt);
      break;

      case 10:                            /* Byt referens */
      if ( tmpref ) tmpref = FALSE;
      else tmpref = TRUE;
      goto l1;

      default:                           /* Ok�nt alt. */
      erpush("IG0103","");
      errmes();
      goto l1;
      }

    if ( status == REJECT )
      {
      goto l1;
      }
/*
***Sudda positionsmenyn och �terv�nd.
*/
    if ( status != GOMAIN ) IGsamu();

    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpabs(pm_ptr *pexnpt)

/*      Position med X,Y,Z format. Skapar en uttrycksnod
 *      av typen vector.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *             < 0 => Fel fr�n PM.
 *
 *      (C)microform ab 16/1/85 J. Kjellander
 *
 *      22/10/85 L�ngre inputstr�ngar, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      8/10/86  Promt, J. Kjellander
 *
 ******************************************************!*/

  {
    char   ps[3][40];     /* promptstr�ng */
    char   is[3][80];     /* inputstr�ng */
    char  *psarr[3];
    char  *isarr[3];
    char  *dsarr[3];
    short  ml[3];         /* maxl�ngder */
    sttycl type;
    char   expr[132];
    short  status;

    static char ds[3][80] = {"0","0","0"};

/*
***Initiering.
*/
    strcpy(ps[0],IGgtts(201));       /* kopiera promptar */
    strcpy(ps[1],IGgtts(202));
    strcpy(ps[2],IGgtts(203));

    ml[0]=80;
    ml[1]=80;
    ml[2]=80;

    psarr[0] = ps[0];                  /* initiera pekarna */
    psarr[1] = ps[1];
    psarr[2] = ps[2];

    isarr[0] = is[0];
    isarr[1] = is[1];
    isarr[2] = is[2];

    dsarr[0] = ds[0];
    dsarr[1] = ds[1];
    dsarr[2] = ds[2];
/*
***L�s in v�rden.
*/    
loop:
    if ( (status=IGmsip(psarr,isarr,dsarr,ml,modtyp)) != 0 ) return(status);
    if ( modtyp == 2 ) strcpy(is[2],"0");
/*
*** Kopiera svaret till defaultstr�ngarna 
*/
    strcpy(ds[0],is[0]); 
    strcpy(ds[1],is[1]);
    strcpy(ds[2],is[2]);
/*
***Skapa funktionsstr�ngen
*/
    if ( modtyp == 2 )
      {
      sprintf(expr,"vec(%s,%s)",is[0],is[1]);
      }
    else
      {
      sprintf(expr,"vec(%s,%s,%s)",is[0],is[1],is[2]);
      }

    if (anaexp(expr,FALSE,pexnpt,&type) != 0)
      {
      errmes();
      goto loop;
      }

    return(0);
    }

/********************************************************/
/*!******************************************************/

static short gnprel(pm_ptr *pexnpt)

/*      Position relativt position. Skapar en uttrycksnod
 *      av typen vector.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => �ter till huvudmenyn
 *
 *      (C)microform ab 9/8/85 J. Kjellander
 *
 *      23/3/86  Anrop IGcpos(pnr,... B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt1,exnpt2;
    short   status;

/*
***Skapa basposition.
*/
     WPaddmess_mcwin(IGgtts(51),WP_PROMPT);
     if ( (status=gnpmen(&exnpt1)) < 0 ) goto end;
     WPclear_mcwin();
/*
***Skapa "vec(dx,dy,dz)".
*/
     IGptma(287,IG_INP);
     if ( (status=gnpabs(&exnpt2)) < 0 ) goto end;
/*
***L�nka ihop till "pos + vec(dx,dy,dz)".
*/
    pmcbie(PM_PLUS,exnpt1,exnpt2,pexnpt);

end:
    return(status);
    }

/********************************************************/
/*!******************************************************/

static short gnpcrh(pm_ptr *pexnpt)

/*      Position med h�rkors. Skapar en literal
 *      av typen vector.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 3/7/85 J. Kjellander
 *
 *      3/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    DBVector   pos;
    PMLITVA litstr;
    short   status;

/*
***L�s in h�rkorsposition.
*/
    if ( (status=igpcrh(&pos)) != 0 ) return(status);
/*
***Skapa literal.
*/
    litstr.lit_type = C_VEC_VA;
    litstr.lit.vec_va.x_val = pos.x_gm;
    litstr.lit.vec_va.y_val = pos.y_gm;
    litstr.lit.vec_va.z_val = pos.z_gm;
    pmclie(&litstr,pexnpt);

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short gnpexp(pm_ptr *pexnpt)

/*      Position med uttryck. Skapar en identifierar-
 *      uttrycksnod.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn
 *
 *      Felkoder: IG2063 = Variabeln har fel datatyp
 *
 *      (C)microform ab 16/1/85 J. Kjellander
 *
 *      6/10/86 GOMAIN, J. Kjellander
 *      7/10/86 errmes(), J. Kjellander
 *
 ******************************************************!*/

    {
    char    istr[V3STRLEN+1];
    sttycl  type;
    short   status;

/*
***L�s in uttryck.
*/
    IGptma(209,IG_INP);
loop:
    if ( (status=IGssip(IGgtts(267),istr,"",V3STRLEN)) != 0 ) goto end;
/*
***Prova att analysera.
*/
    if ( anaexp(istr,FALSE,pexnpt,&type ) != 0 )
      {
      errmes();
      goto loop;
      }
/*
***Kolla att det var r�tt typ.
*/
    if (type != ST_VEC)
      {
      erpush("IG2232",IGgtts(7));
      errmes();
      goto loop;
      }
/*
***Slut.
*/
end:
    return(status);
    }

/********************************************************/
/*!******************************************************/

static short gnpend(pm_ptr *pexnpt)

/*      Huvudrutin f�r position i �nden av storhet med fast
 *      eller tempor�r referens.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 1/11/85 J. Kjellander
 *
 *      20/3/86    Anrop pmtcon, pmclie B. Doverud
 *      6/10/86    GOMAIN, J. Kjellander
 *      1997-04-21 STARTP/ENDP, J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ;
    pm_ptr   arglst,ref,dummy;
    pm_ptr   exnpt;
    stidcl   kind;
    bool     end,right;
    short    status;
    PMLITVA  litstr;
    DBVector posvec;

/*
***Tempor�r referens.
*/
    if ( tmpref)
      {
      if ( (status=igpend(&posvec)) != 0 ) goto exit;

      litstr.lit_type = C_VEC_VA;
      litstr.lit.vec_va.x_val = posvec.x_gm;
      litstr.lit.vec_va.y_val = posvec.y_gm;
      litstr.lit.vec_va.z_val = posvec.z_gm;
      pmclie( &litstr, pexnpt);
      }
/*
***Fast referens.
*/
    else
      {
/*
***Create REF expression.
*/
      typ = LINTYP+ARCTYP+CURTYP;
      if ( (status=genrfs(331,&typ,&exnpt,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Skapa argumentlistan.
*/
      pmtcon(exnpt,(pm_ptr)NULL,&arglst,&dummy);
/*
***Skapa funktionsuttrycksnoden.
*/
      if ( end ) stlook("ENDP",&kind,&ref);
      else       stlook("STARTP",&kind,&ref);

      pmcfue(ref,arglst,pexnpt);
      }
/*
***Avslutning.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpon(pm_ptr *pexnpt)

/*      Huvudrutin f�r position p� en storhet med fast
 *      eller tempor�r referens.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 16/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 �nde och sida, J. Kjellander
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      1998-09-24 b_plan, J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ;
    pm_ptr   retla,arglst,ref,dummy;
    pm_ptr   exnpt1,exnpt2;
    stidcl   kind;
    bool     end,right;
    char     istr[V3STRLEN+1];
    short    status;
    PMLITVA  litstr;
    DBVector posvec;

    static char dstr[V3STRLEN+1] = "0.5";

/*
***Tempor�r referens.
*/
    if ( tmpref)
      {
      if ( (status=igpon(&posvec)) != 0 ) goto exit;

      litstr.lit_type = C_VEC_VA;
      litstr.lit.vec_va.x_val = posvec.x_gm;
      litstr.lit.vec_va.y_val = posvec.y_gm;
      litstr.lit.vec_va.z_val = posvec.z_gm;
      pmclie( &litstr, pexnpt);
      }
/*
***Fast referens.
*/
    else
      {
/*
***Skapa referens.
*/
      typ = POITYP+LINTYP+ARCTYP+CURTYP+CSYTYP+BPLTYP+
            TXTTYP+LDMTYP+CDMTYP+RDMTYP+ADMTYP+SURTYP;
      if ( (status=genrfs(52,&typ,&exnpt1,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Alla typer av storheter kr�ver en referens som argument.
*/
      pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
/*
***Tr�dstorheter har en FLOAT som ytterligare parameter.
*/
      if ( typ == LINTYP  ||  typ == ARCTYP  ||  typ == CURTYP )
        {
        if ( (status=IGcflt((short)208,dstr,istr,&exnpt2)) < 0 ) goto exit;
        pmtcon(exnpt2,retla,&arglst,&dummy);
        strcpy(dstr,istr);
        }
/*
***Ytor har en VECTOR som ytterligare argument.
*/
      else if ( typ == SURTYP  ||  typ == BPLTYP )
        {
        if ( (status=genuv((short)208,&exnpt2)) < 0 ) goto exit;
        pmtcon(exnpt2,retla,&arglst,&dummy);
        }
/*
***�vriga storheter har ingen ytterligare parameter.
*/
      else arglst = retla;
/*
***Skapa funktionsuttrycksnoden.
*/
      stlook("ON",&kind,&ref);
      pmcfue(ref,arglst,pexnpt);
      }
/*
***Avslutning.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpint(pm_ptr *pexnpt)

/*      Huvudrutin f�r position sk�rning mellan storheter
 *      med fast eller tempor�r referens.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression-node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 �nde och sida, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutg�ng B. Doverud
 *      7/10/86  GOMAIN, J. Kjellander
 *      20/11/89 Neg. intnr, J. Kjellander
 *      23/12/91 Bplan och koord.sys, J. Kjellander
 *      7/9/95   Ytor, J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype  typ1,typ2;
    pm_ptr   retla,arglst,ref,dummy;
    stidcl   kind;
    pm_ptr   exnpt1,exnpt2,exnpt3;
    short    status;
    bool     end,right;
    char     istr[V3STRLEN+1];
    DBVector posvec;
    PMLITVA  litstr;

/*
***Tempor�r referens.
*/
    if ( tmpref || (v3mode == RIT_MOD) )
      {
      if ( (status=igpint(&posvec)) != 0 ) goto exit;

      litstr.lit_type = C_VEC_VA;
      litstr.lit.vec_va.x_val = posvec.x_gm;
      litstr.lit.vec_va.y_val = posvec.y_gm;
      litstr.lit.vec_va.z_val = posvec.z_gm;
      pmclie( &litstr, pexnpt);
      }
/*
***Fast referens.
*/
    else
      {
/*
***L�s in 2 referenser.
*/
      typ1 = LINTYP+ARCTYP+CURTYP;
      if ( modtyp == 3 ) typ1 += BPLTYP+CSYTYP+SURTYP;
      if ( (status=genrfs(324,&typ1,&exnpt1,&end,
                          &right,(short)0)) != 0 ) goto exit;

      typ2 = LINTYP+ARCTYP+CURTYP;
      if ( modtyp == 3  &&  typ1 != BPLTYP  &&
                            typ1 != CSYTYP  &&
                            typ1 != SURTYP )
        typ2 += BPLTYP+CSYTYP+SURTYP;
      if ( (status=genrfs(325,&typ2,&exnpt2,&end,
                          &right,(short)0)) != 0 ) goto exit;
/*
***Om sk�rning linje/linje, alt = -1.
*/
      if ( typ1 == LINTYP && typ2 == LINTYP )
         {
         litstr.lit_type = C_INT_VA;
         litstr.lit.int_va = -1;
         pmclie( &litstr, &exnpt3);
         }
/*
***Annars l�s in alternativ.
*/
      else
         {
         if ( (status=IGcint(327,"1",istr,&exnpt3)) < 0 ) goto exit;
         }
/*
***Skapa argumentlistan.
*/
      pmtcon( exnpt1, (pm_ptr)NULL, &retla, &dummy);
      pmtcon( exnpt2, retla, &retla, &dummy);
      pmtcon( exnpt3, retla, &arglst, &dummy);
/*
***Skapa funktionsuttrycksnoden.
*/
      stlook("INTERSECT",&kind,&ref);
      pmcfue( ref, arglst, pexnpt);
      }
/*
***Avslutning.
*/
exit:
    return(status);

  }

/********************************************************/
/*!******************************************************/

static short gnpcen(pm_ptr *pexnpt)

/*      Huvudrutin f�r position i kr�kningscentrum med fast
 *      eller tempor�r referens.
 *
 *      In: pexnpt => Pekare till pm_ptr variabel.
 *
 *      Ut: *pexnpt => PM-pekare till expression node.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn
 *
 *      (C)microform ab 26/4/87 J. Kjellander
 *
 ******************************************************!*/

  {
    DBetype   typ;
    pm_ptr  retla,arglst,ref,dummy;
    pm_ptr  exnpt1,exnpt2;
    stidcl  kind;
    bool    end,right;
    char    istr[V3STRLEN+1];
    short   status;
    PMLITVA litstr;
    DBVector   posvec;

    static char dstr[V3STRLEN+1] = "0.0";

/*
***Tempor�r referens.
*/
    if ( tmpref)
      {
      if ( (status=igpcen(&posvec)) != 0 ) goto exit;

      litstr.lit_type = C_VEC_VA;
      litstr.lit.vec_va.x_val = posvec.x_gm;
      litstr.lit.vec_va.y_val = posvec.y_gm;
      litstr.lit.vec_va.z_val = posvec.z_gm;
      pmclie( &litstr, pexnpt);
      }
/*
***Fast referens.
*/
    else
      {
/*
***Skapa referens.
*/
      typ = ARCTYP+CURTYP;
      if ( (status=genrfs(53,&typ,&exnpt1,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Om arc, s�tt parameterv�rdet till 0.
*/
      if ( typ == ARCTYP )
        {
        litstr.lit_type = C_FLO_VA;
        litstr.lit.float_va = 0.0;
        pmclie( &litstr, &exnpt2);
        }
/*
***Om kurva l�s i parameterv�rde.
*/
      else
        {
        if ( (status=IGcflt(208,dstr,istr,&exnpt2)) < 0 ) goto exit;
        strcpy(dstr,istr);
        }
/*
***Skapa argumentlistan.
*/
      pmtcon( exnpt1, (pm_ptr)NULL, &retla, &dummy);
      pmtcon( exnpt2, retla, &arglst, &dummy);
/*
***Skapa funktionsuttrycksnoden.
*/
      stlook("CENTRE",&kind,&ref);
      pmcfue( ref, arglst, pexnpt);
      }
/*
***Avslutning.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short    IGgsid(
        DBId    *idvek,
        DBetype *typ,
        bool    *end,
        bool    *right,
        short    utstat)

/*      T�nder h�rkors. L�ser in koordinat + pektecken.
 *      G�r s�kning i displayfil via gpgtla() med mask
 *      som beror av pek-tecknet. Med utstat <> 0 till�ts
 *      rutinen avsluta med status = utstat f�rutom de
 *      vanliga REJECT och GOMAIN.
 *
 *      In: idvek  => Pekare till array av DBId   .
 *                    Arrayen beh�ver ej vara l�nkad.
 *                    M�ste finnas minst MXINIV element.
 *          typ    => �nskad typ.
 *          end    => Slut�nde, TRUE/FALSE.
 *          right  => Sida, TRUE/FALSE.
 *          utstat => 0 or WINDOW
 *
 *      Ut: *idvek => L�nkad lista med identitet.
 *          *typ   => GM-typ.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Huvudmenyn
 *           IG3532 => Storheten ing�r ej i en part
 *           IG2242 => Syntaxfel i id-str�ng = %s
 *           IG2272 => Storheten %s finns ej
 *
 *      (C)microform ab 4/2/85 J. Kjellander
 *
 *      28/10/85 �nde och sida, J. Kjellander
 *      30/12/85 Pekm�rke, J. Kjellander
 *      30/12/85 Symbol, J. Kjellander
 *      10/3/86  Part, J. Kjellander
 *      13/3/86  Pektecken "i", J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      20/10/86 HELP, J. Kjellander
 *      23/12/86 Global ref, J. Kjellander
 *      27/8/87  B_plan, J. Kjellander
 *      17/11/88 utstat, J. Kjellander
 *      10/1-95  Multif�nster, J. Kjellander
 *      1996-04-30 Pekning p� part, J. Kjellander
 *      1997-04-10 WIN32, J.Kjellander
 *      2006-12-08 Tagit bort gpgtla(), J.Kjellander
 *
 ******************************************************!*/

  {
    char    pektkn;
    short   ix,iy,status;
    DBetype typmsk,pektyp;
    DBptr   la;
    char    idstr[V3STRLEN+1];
    DBint   win_id;

/*
***Get a cursor position.
*/
loop:
    if ( WPgtsc(FALSE,&pektkn,&ix,&iy,&win_id) == SMBPOSM ) return(SMBPOSM);
    if ( pektkn == *smbind[1].str ) return(REJECT);
    if ( pektkn == *smbind[7].str ) return(GOMAIN);
    if ( pektkn == *smbind[8].str )
      {
      if ( IGhelp() == GOMAIN ) return(GOMAIN);
      else goto loop;
      }
/*
***Window mode ?
*/
    if ( utstat == WINDOW  &&  pektkn == *IGgtts(98) ) return(WINDOW);
/*
***If the requested typemask is PART all entities are allowed.
*/
    if ( *typ == PRTTYP ) pektyp = ALLTYP;
    else                  pektyp = *typ;
/*
***Check pointer character and make typemask.
*/
    if ( pektkn == *IGgtts(80) )
      {
      if ( (pektyp & POITYP) == 0 ) goto typerr;
      typmsk = POITYP;
      }

    else if ( pektkn == *IGgtts(81) )
      {
      if ( (pektyp & LINTYP) == 0 ) goto typerr;
      typmsk = LINTYP;
      }

    else if ( pektkn == *IGgtts(82) )
      {
      if ( (pektyp & ARCTYP) == 0 ) goto typerr;
      typmsk = ARCTYP;
      }

    else if ( pektkn == *IGgtts(83) )
      {
      if ( (pektyp & CURTYP) == 0 ) goto typerr;
      typmsk = CURTYP;
      }

    else if ( pektkn == *IGgtts(84) )
      {
      if ( (pektyp & SURTYP) == 0 ) goto typerr;
      typmsk = SURTYP;
      }

    else if ( pektkn == *IGgtts(85) )
      {
      if ( (pektyp & CSYTYP) == 0 ) goto typerr;
      typmsk = CSYTYP;
      }

    else if ( pektkn == *IGgtts(86) )
      {
      if ( (pektyp & TXTTYP) == 0 ) goto typerr;
      typmsk = TXTTYP;
      }

    else if ( pektkn == *IGgtts(87) )
      {
      if ( (pektyp & LDMTYP) == 0 ) goto typerr;
      typmsk = LDMTYP;
      }

    else if ( pektkn == *IGgtts(88) )
      {
      if ( (pektyp & CDMTYP) == 0 ) goto typerr;
      typmsk = CDMTYP;
      }

    else if ( pektkn == *IGgtts(89) )
      {
      if ( (pektyp & RDMTYP) == 0 ) goto typerr;
      typmsk = RDMTYP;
      }

    else if ( pektkn == *IGgtts(90) )
      {
      if ( (pektyp & ADMTYP) == 0 ) goto typerr;
      typmsk = ADMTYP;
      }

    else if ( pektkn == *IGgtts(91) )
      {
      if ( (pektyp & XHTTYP) == 0 ) goto typerr;
      typmsk = XHTTYP;
      }

    else if ( pektkn == *IGgtts(95) )
      {
      if ( (pektyp & PRTTYP) == 0 ) goto typerr;
      typmsk = ALLTYP;
      }

    else if ( pektkn == *IGgtts(96) )
      {
      if ( (pektyp & BPLTYP) == 0 ) goto typerr;
      typmsk = BPLTYP;
      }

    else if ( pektkn == *IGgtts(79) )
      {
      if ( (pektyp & MSHTYP) == 0 ) goto typerr;
      typmsk = MSHTYP;
      }

    else if ( pektkn == ' ' ) typmsk = pektyp;
/*
***Inmatning av ID fr�n tangentbordet. Om global identitet
***matas in (##id) returnerar vi �nd� lokal eftersom m�nga
***rutiner tex. pmlges() kr�ver detta.
*/
    else if ( pektkn == *IGgtts(97) )
      {
inid:
      status = IGssip(IGgtts(283),idstr,"",V3STRLEN);
      if ( status == REJECT ) return(REJECT);
      if ( status == GOMAIN ) return(GOMAIN);
      if ( IGstid(idstr,idvek) < 0 )
        {
        erpush("IG2242",idstr);
        errmes();
        goto inid;
        }
      if ( DBget_pointer('I',idvek,&la,&pektyp) < 0 )
        {
        erpush("IG2272",idstr);
        errmes();
        goto loop;
        }
      if ( idvek->seq_val < 0 ) idvek->seq_val = -idvek->seq_val;
      goto finish;
      }
/*
***Otill�tet pektecken.
*/
    else goto typerr;
/*
***G�r s�kning i df och skapa lokal ID.
*/
    if ( (la=WPgtla(win_id,typmsk,ix,iy,&pektyp,end,right)) < 0 ) goto typerr;
    DBget_id(la,idvek);
    idvek->seq_val = abs(idvek->seq_val);
/*
***Storheten hittad, highlighting.
*/
finish:
    WPhgen(GWIN_ALL,la,TRUE);
/*
***Om best�lld typ = part eller pektkn f�r part
***anv�nts ser vi till att returnera identiteten f�r en
***part och inte storheten som ing�r i parten.
*/
    if ( *typ == PRTTYP || pektkn == *IGgtts(95) )
      {
/*
***Om id bara har en niv� kan det vara en enkel storhet i
***aktiv modul. Detta �r inte till�tet. Vi kollar det med
***DBget_pointer().
*/
      if ( idvek->p_nextre == NULL )
        {
        if ( DBget_pointer('I',idvek,&la,&pektyp) < 0 ) goto loop;
    
        if ( pektyp != PRTTYP )
          {
          erpush("IG3532","");
          errmes();
          WPepmk(GWIN_ALL);
          goto loop;
          }
        }
      idvek->p_nextre = NULL;
      *typ = PRTTYP;
      }
/*
***Returnera r�tt typ.
*/
    else *typ = pektyp;

    return(0);
/*
***Fel pektecken.
*/
typerr:
    WPbell();
    goto loop;
  }

/********************************************************/
/*!******************************************************/

        short    IGgmid(
        DBId     idmat[][MXINIV],
        DBetype  typv[],
        int     *idant)

/*      Interactive selection of one or more entities.
 *      Returns the Varkon ID for idant entities in
 *      idmat declared as: DBId idmat[idant][MXINIV]
 *
 *      In:  idant[0] = Max number of ID's to return.
 *
 *      Out: *idmat = Id's.
 *           *typv  = Types.
 *           *idant = Number of ID's returned.
 *
 *      Return:  0 => Ok.
 *          GOMAIN => Main menu.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,pekmod;
    int     nref,nmax,nleft;
    DBetype orgtyp;
    bool    end,right;

/*
***Init.
*/
   nref   = 0;
   nmax   = *idant;
   orgtyp = typv[0];

   if ( nmax > 1 ) pekmod = WINDOW;
   else            pekmod = 0;
/*
***Loop.
*/
   while ( nref <  nmax )
     {
     typv[nref] = orgtyp;
/*
***Try first to get a single entity.
*/
     status = IGgsid(&idmat[nref][0],&typv[nref],&end,&right,pekmod);
     if       ( status == REJECT ) break;
     else if ( status == GOMAIN ) return(GOMAIN);
/*
***Status = WINDOW means window mode.
*/
     else if ( status == WINDOW )
       {
       nleft = nmax - nref;
       status = getwid(&idmat[nref],idmat,&nleft,nref,&typv[nref]);
       if ( status == GOMAIN) return(GOMAIN);
       nref += nleft;
       }
/*
***A single entity is selected. Check if it is alredy
***selected.
*/
     else
       {
       if ( IGcmid(&idmat[nref][0],idmat,nref) )
         {
         erpush("IG5162","");
         errmes();
         }
/*
***Increase nref and try agaian.
*/
       else ++nref;
       }
     }
/*
***The end.
*/
   *idant = nref;

   if ( nref == 0 ) return(REJECT);
   else             return(0);

  }

/********************************************************/
/*!******************************************************/

 static short   getwid(
        DBId    idmat1[][MXINIV],
        DBId    idmat2[][MXINIV],
        int    *idant1,
        int     idant2,
        DBetype typvek[])

/*      Returns idant ID's using window mode and WPgmla().
 *      Returns the Varkon ID for idant entities in
 *      idmat declared as: DBId idmat[idant][MXINIV].
 *
 *      In: idmat2    = ID's to test against
 *         *idant1    = Max number of ID's in idmat1
 *          idant2    = Current number of ID's in idmat2
 *          typvek[0] = Typemask
 *
 *      Out: *idmat1 = New ID's
 *           *typvek = Their types
 *           *idant1 = Number of new ID's
 *
 *      Felkoder: IG3082 = Punkt 2 = Punkt 1
 *
 *      Return:  0 => Ok.
 *          GOMAIN => Main menu.
 *
 *      (C)microform ab 17/11/88 J. Kjellander
 *
 *      1997-04-10 WIN32, J.Kjellander
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status,nref;
    int     ix1,ix2,iy1,iy2,i,tmp,mode,nok;
    DBetype orgtyp;
    DBptr   lavek[IGMAXID];
    wpw_id  grw_id;

/*
***Init.
*/
start:
   nref   = *idant1;
   nok    = 0;
   orgtyp = typvek[0];
   mode   = 1;  /* All totally or partially inside */
/*
***Get two pos with rubberband rectangle. Possible
***status from WPgtsw() is 0, -1 (p1=p2) REJECT and GOMAIN.
*/
   status = WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,WP_RUB_RECT,TRUE);
/*
***p1 = p2.
*/
   if ( status == -1 )
     {
     erpush("IG3082","");
     errmes();
     goto start;
     }

   else if ( status < 0 ) return(status);
/*
***Sort.
*/
   if ( ix1 > ix2 ) { tmp=ix1; ix1=ix2; ix2=tmp; }
   if ( iy1 > iy2 ) { tmp=iy1; iy1=iy2; iy2=tmp; }
/*
***Get the DBptr of all entities inside the rectangle.
***Status < 0 => Illegal window.
*/
    status = WPgmla(grw_id,(short)ix1,(short)iy1,(short)ix2,(short)iy2,mode,TRUE,&nref,typvek,lavek);
    if ( status < 0 )
      {
      WPbell();
      goto start;
      }
/*
***Translate DBptr to local ID and remove all entities
***already selected.
*/
   for ( i=0; i<nref; ++i)
     {
     DBget_id(lavek[i],idmat1[nok]);
     idmat1[nok][0].seq_val = abs(idmat1[nok][0].seq_val);
     if ( !IGcmid(idmat1[nok],idmat2,idant2) ) ++nok;
     }
/*
***The end.
*/
   *idant1 = nok;

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short igpcrh(DBVector *pos)

/*      L�s in position med h�rkors.
 *
 *      In: pos = Pekare till utdata.
 *
 *      Ut: *pos = Koordinat.
 *
 *      FV:      0  = Ok.
 *          REJECT  = Operationen avbruten.
 *          GOMAIN  = Huvudmenyn.
 *          SMBPOSM = Pos-button selected.
 *
 *      (C)microform ab 1998-03-31 J.Kjellander
 *
 ******************************************************!*/

  {
   short status;

   switch ( modtyp )
     {
     case 2: status = igpcrh_2D(pos); break;
     case 3: status = igpcrh_3D(pos); break;
     default: status = REJECT; break;
     }

   return(status);
  }

/********************************************************/
/*!******************************************************/

static short igpcrh_3D(DBVector *vecptr)

/*      L�s in 3D position med h�rkors.
 *
 *      In: vecptr => Pekare till utdata.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 1998-03-31, J.Kjellander
 *
 *      1998-10-20 WIN32, J.Kjellander
 *      2007-04-03 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    char    pektkn;
    double  x,y,xg,yg,ng;
    wpw_id  grw_id;
    WPGWIN *gwinpt;

/*
***Get 3D model coordinates by mouse position. WPgmc3()
***returns local coordinates. This is different from WPgmc2() !
*/
loop:
#ifdef UNIX
    if ( (status=WPgmc3(TRUE,&pektkn,vecptr,&grw_id)) == SMBPOSM ) return(SMBPOSM);
#endif
#ifdef WIN32
    status = msgmc3(&pektkn,vecptr,TRUE);
#endif

    if ( pektkn == *smbind[1].str) return(REJECT);
    if ( pektkn == *smbind[7].str) return(GOMAIN);
    if ( pektkn == *smbind[8].str)
      {
      if ( IGhelp() == GOMAIN ) return(GOMAIN);
      goto loop;
      }
/*
***Negative status means that the active XY-plane
***is perpendicular to the screen.
*/
    if ( status < 0 )
      {
      WPbell();
      WPepmk(GWIN_ALL);
      goto loop;
      }
/*
***Grid processing.
*/
    if ( posmode == 8  ||  pektkn == *IGgtts(93) )     
      {
/*
***Get a C ptr to the WPGWIN. WPRWIN's don't support grid yet.
*/
      if ( wpwtab[grw_id].typ != TYP_GWIN ) return(0);

      gwinpt = (WPGWIN *)wpwtab[grw_id].ptr;

      x = vecptr->x_gm;
      y = vecptr->y_gm;

      ng = ABS((x - gwinpt->grid_x)/gwinpt->grid_dx);
      if ( DEC(ng) > 0.5 ) ++ng;

      if ( x > gwinpt->grid_x ) xg = gwinpt->grid_x + HEL(ng) * gwinpt->grid_dx;
      else                      xg = gwinpt->grid_x - HEL(ng) * gwinpt->grid_dx;

      ng = ABS((y - gwinpt->grid_y)/gwinpt->grid_dy);
      if ( DEC(ng) > 0.5 ) ++ng;

      if ( y > gwinpt->grid_y ) yg = gwinpt->grid_y + HEL(ng) * gwinpt->grid_dy;
      else                      yg = gwinpt->grid_y - HEL(ng) * gwinpt->grid_dy;

      vecptr->x_gm = xg;
      vecptr->y_gm = yg;
      vecptr->z_gm = 0.0;
      }
/*
***The end.
*/ 
    return(0);
  }

/********************************************************/
/*!******************************************************/

static short igpcrh_2D(DBVector *vecptr)

/*      L�s in 2D position med h�rkors eller
 *      position p� en rasterpunkt. Positionen 
 *      transformeras till aktivt koordinatsystem.
 *      <SP> och h ger h�rkorsposition.
 *      r ger rasterposition.
 *
 *      In: vecptr => Pekare till DBVector.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0  = Ok.
 *          REJECT  = Operationen avbruten.
 *          GOMAIN  = Huvudmenyn.
 *          SMBPOSM = Pos-button selected
 *
 *      (C)microform ab
 *
 *      9/9/85   H�rkors/raster sammanslaget, R. Svedin
 *      30/12/85 Pekm�rke, J. Kjellander
 *      3/10/86  GOMAIN, J. Kjellander
 *      20/10/86 HELP, J. Kjellander
 *      2007-03-12 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    char    pektkn;
    double  x,y,xg,yg,ng;
    wpw_id  grw_id;
    WPGWIN *gwinpt;

/*
***Get 2D model coordinates by mouse position.
*/
loop:
    if ( WPgmc2(TRUE,&pektkn,&x,&y,&grw_id) == SMBPOSM ) return(SMBPOSM);
    if ( pektkn == *smbind[1].str) return(REJECT);
    if ( pektkn == *smbind[7].str) return(GOMAIN);
    if ( pektkn == *smbind[8].str)
      {
      if ( IGhelp() == GOMAIN ) return(GOMAIN);
      goto loop;
      }
/*
***Transform to local coordinates.
*/
    vecptr->x_gm = x;
    vecptr->y_gm = y;
    vecptr->z_gm = 0.0;
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);
/*
***Grid.
*/
    if ( posmode == 8  ||  pektkn == *IGgtts(93) )
      {
/*
***Get a C ptr to the WPGWIN. WPRWIN's don't support grid yet.
*/
      if ( wpwtab[grw_id].typ != TYP_GWIN ) return(0);

      gwinpt = (WPGWIN *)wpwtab[grw_id].ptr;

      x = vecptr->x_gm;
      y = vecptr->y_gm;

      ng = ABS((x - gwinpt->grid_x)/gwinpt->grid_dx);
      if ( DEC(ng) > 0.5 ) ++ng;

      if ( x > gwinpt->grid_x ) xg = gwinpt->grid_x + HEL(ng) * gwinpt->grid_dx;
      else                      xg = gwinpt->grid_x - HEL(ng) * gwinpt->grid_dx;

      ng = ABS((y - gwinpt->grid_y)/gwinpt->grid_dy);
      if ( DEC(ng) > 0.5 ) ++ng;

      if ( y > gwinpt->grid_y ) yg = gwinpt->grid_y + HEL(ng) * gwinpt->grid_dy;
      else                      yg = gwinpt->grid_y - HEL(ng) * gwinpt->grid_dy;

      vecptr->x_gm = xg;
      vecptr->y_gm = yg;
      vecptr->z_gm = 0.0;

      return(0);
      }
/*
***Cursor.
*/
    else if ( pektkn == *IGgtts(92) || pektkn == 32 )
      {
      return(0);
      }
/*
***Unknown pointer character.
*/
    else 
      {
      WPbell();
      goto loop;
      }
  }

/********************************************************/
/*!******************************************************/

 static short igpend(DBVector *vecptr)

/*      Position i ena �nden av en storhet.
 *
 *      In: vecptr => Pekare till DBVector.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 28/10/85 J. Kjellander
 *
 *      26/3/86  Felutskrift fr�n EXon, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    DBfloat   t;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];
    DBAny  gmpost;

/*
***H�mta id f�r den refererade storheten.
*/
loop:
    typ = LINTYP+ARCTYP+CURTYP;
    WPaddmess_mcwin(IGgtts(331),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***Kolla typ och �nde och v�lj r�tt parameterv�rde.
*/
    if ( end )
      {
      if ( typ == CURTYP )
        {
        DBget_pointer('I',idvek,&la,&typ);
        DBread_curve(&gmpost.cur_un,NULL,NULL,la);
        t = gmpost.cur_un.ns_cu;
        }
      else t = 1.0;
      }
    else t = 0.0;
/*
***Ber�kna positionen.
*/
    if ( EXon(idvek,t,(DBfloat)0.0,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***Avslutning.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

 static short igpon(DBVector *vecptr)

/*      Position p� en storhet.
 *
 *      In: vecptr => Pekare till DBVector.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      28/10/85 �nde och sida, J. Kjellander
 *      29/12/85 Symbol, J. Kjellander
 *      26/3/86  Felutskrift fr�n EXon, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      22/2/93  Nytt anrop till EXon(), J. Kjellander
 *
 ******************************************************!*/

  {
    double  t;
    DBetype typ;
    bool     end,right;
    short    status;
    DBId     idvek[MXINIV];

/*
***H�mta id f�r den refererade storheten.
*/
loop:
    typ = POITYP+LINTYP+ARCTYP+CURTYP+CSYTYP+
          TXTTYP+LDMTYP+CDMTYP+RDMTYP+ADMTYP;
    WPaddmess_mcwin(IGgtts(52),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***Kolla typ. Om punkt etc. s�tt parameterv�rdet = 0.
*/
    if ( typ == POITYP || typ == CSYTYP || typ == TXTTYP ||
         typ == LDMTYP || typ == CDMTYP || typ == RDMTYP ||
         typ == ADMTYP ) t = 0.0;
    else
       {
       IGptma(208,IG_INP);
       status=IGsfip(IGgtts(320),&t);
       if ( status < 0 ) goto exit;
       }
/*
***Ber�kna positionen.
*/
    if ( EXon (idvek,t,(DBfloat)0.0,vecptr) < 0 )
      {
      WPclear_mcwin();
      errmes();
      goto loop;
      }
/*
***Avslutning.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short igpint(DBVector *vecptr)

/*      Position i sk�rningspunkt.
 *
 *      In: vecptr => Pekare till DBVector.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 4/2/85 J. Kjellander 
 *
 *      28/10/85 �nde och sida, J. Kjellander
 *      26/3/86  Felutskrift fr�n EXsect, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      26/11/89 Neg intnr, J. Kjellander
 *
 ******************************************************!*/

  {
    short   alt,status;
    int     ival;
    DBetype typ1,typ2;
    DBptr   la;
    bool    end,right,enkel=FALSE;
    DBId    idv1[MXINIV],idv2[MXINIV];
    DBAny  gmpost;
    DBSeg   arcseg[4];

/*
***1:a storheten.
*/
loop:
    WPaddmess_mcwin(IGgtts(324),WP_PROMPT);
    typ1 = LINTYP+ARCTYP+CURTYP;
    if ( modtyp == 3 ) typ1 += BPLTYP+CSYTYP;
    if ( (status=IGgsid(idv1,&typ1,&end,&right,(short)0)) < 0 ) goto exit;
    WPclear_mcwin();
/*
***2:a storheten.
*/
    typ2 = LINTYP+ARCTYP+CURTYP;
    if ( modtyp == 3  &&  typ1 != BPLTYP  &&  typ1 != CSYTYP )
      typ2 += BPLTYP+CSYTYP;
    WPaddmess_mcwin(IGgtts(325),WP_PROMPT);
    if ( (status=IGgsid(idv2,&typ2,&end,&right,(short)0)) < 0 ) goto exit;
    WPclear_mcwin();
/*
***Om sk�rning linje/linje, alt = -1.
*/
    if ( typ1 == LINTYP  &&  typ2 == LINTYP ) alt = -1;
/*
***�r det en enkel 2D-sk�rning ?
*/
    else
      {
      if ( typ1 == LINTYP  &&  typ2 == ARCTYP )
        {
        DBget_pointer('I',idv2,&la,&typ2);
        DBread_arc(&gmpost.arc_un,arcseg,la);
        if ( gmpost.arc_un.ns_a == 0 ) enkel = TRUE;
        }
      else if ( typ1 == ARCTYP  &&  typ2 == LINTYP )
        {
        DBget_pointer('I',idv1,&la,&typ1);
        DBread_arc(&gmpost.arc_un,arcseg,la);
        if ( gmpost.arc_un.ns_a == 0 ) enkel = TRUE;
        }
      else if ( typ1 == ARCTYP  &&  typ2 == ARCTYP )
        {
        DBget_pointer('I',idv1,&la,&typ1);
        DBread_arc(&gmpost.arc_un,arcseg,la);
        if ( gmpost.arc_un.ns_a == 0 )
          {
          DBget_pointer('I',idv2,&la,&typ2);
          DBread_arc(&gmpost.arc_un,arcseg,la);
          if ( gmpost.arc_un.ns_a == 0 ) enkel = TRUE;
          } 
        }
      else enkel = FALSE;
/*
***Om det �r en enkel 2D-sk�rning, fr�ga efter 1:a eller
***2:a sk�rningen.
*/
      if ( enkel )
        {
        if ( IGialt(160,161,162,FALSE) ) alt = -1;
        else alt = -2;
        }
/*
***Nej det �r inte en enkel 2D-sk�rning, l�s in alternativ.
*/
      else
        {
        IGptma(327,IG_INP);
        if ( (status=IGsiip(IGgtts(46), &ival)) < 0 ) goto exit;
        alt = (short)ival;
        }
      }
/*
***Ber�kna sk�rningen.
*/
    if ( EXsect (idv1,idv2,alt,0,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***Utg�ng f�r avbruten operation.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

 static short igpcen(DBVector *vecptr)

/*      Position i kr�kningscentrum.
 *
 *      In: vecptr => Pekare till DBVector.
 *
 *      Ut: *vecptr => Vektor.
 *
 *      FV:      0 => Ok.
 *          REJECT => Operationen avbruten.
 *          GOMAIN => Huvudmenyn.
 *
 *      (C)microform ab 26/4/87 J. Kjellander
 *
 ******************************************************!*/

  {
    double  t;
    DBetype   typ;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];

/*
***H�mta id f�r den refererade storheten.
*/
loop:
    typ = ARCTYP+CURTYP;
    WPaddmess_mcwin(IGgtts(53),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***Kolla typ. Om arc s�tt parameterv�rdet = 0.
*/
    if ( typ == ARCTYP ) t = 0.0;
    else
       {
       IGptma(208,IG_INP);
       status=IGsfip(IGgtts(320),&t);
       if ( status < 0 ) goto exit;
       }
/*
***Ber�kna positionen.
*/
    if ( EXcen(idvek,t,lsyspk,vecptr) < 0 )
      {
      WPclear_mcwin();
      errmes();
      goto loop;
      }
/*
***Avslutning.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
