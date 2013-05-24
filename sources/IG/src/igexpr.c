/********************************************************************/
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
/*  URL:  http://varkon.sourceforge.net                             */
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
extern short   sysmode,modtyp;
extern int     posmode;
extern bool    relpos;
extern MNUALT  smbind[];
extern DBTmat *lsyspk;

static short genuv(short pnr, pm_ptr *pexnpt);
static short genrfs(short pnr, DBetype *ptyp, pm_ptr *pexnpt, bool *pend,
                    bool *pright, short utstat);
static short gnpany(pm_ptr *pexnpt);
static short gnprel(pm_ptr *pexnpt);
static short gnpabs(pm_ptr *pexnpt);
static short gnpcrh(pm_ptr *pexnpt);
static short gnpmbs(pm_ptr *pexnpt);
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

/*      Huvudrutin fï¿½r att skapa en uttrycksnod fï¿½r
 *      ett heltals-vï¿½rde.
 *
 *      In: pnr    => Promptstrï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
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
 *      30/10/85 Defaultstrï¿½ng, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    sttycl type;

/*
***Get string.
*/
loop:
    if ( (status=IGssip(IGgtts(pnr),IGgtts(46),istr,dstr,V3STRLEN)) < 0) goto exit;
/*
***Analyze.
*/
    if ( anaexp(istr,FALSE,pexnpt,&type ) != 0)
      {
      errmes();
      goto loop;
      }

    if ( type != ST_INT )
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

/*      Lï¿½ser in ett heltalsuttryck och berï¿½knar vï¿½rdet.
 *
 *      In: pnr    => Promptstrï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
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

/*      Huvudrutin fï¿½r att skapa en uttrycksnod fï¿½r
 *      ett double-vï¿½rde.
 *
 *      In: pnr    => Promptstrï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
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
 *      28/10/85 Defaultstrï¿½ng, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    sttycl type;

/*
***Lï¿½s in vï¿½rde.
*/
loop:
    if ( (status=IGssip(IGgtts(pnr),IGgtts(204),istr,dstr,V3STRLEN)) < 0 ) goto exit;

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

/*      Lï¿½ser in ett flyttalsuttryck och berï¿½knar vï¿½rdet.
 *
 *      In: pnr    => Promptstrï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
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

/*      Ett UV-vï¿½rde. Skapar en uttrycksnod
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
 *             < 0 => Fel frï¿½n PM.
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
***Get strings.
*/
loop:
    if ( (status=IGmsip(IGgtts(pnr),psarr,isarr,dsarr,ml,(short)2)) < 0 ) goto exit;
/*
*** Kopiera svaret till defaultstrï¿½ngarna 
*/
    strcpy(ds[0],is[0]); 
    strcpy(ds[1],is[1]);
/*
***Skapa funktionsstrï¿½ngen
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

/*      Huvudrutin fï¿½r att skapa en uttrycksnod fï¿½r
 *      ett strï¿½ng-vï¿½rde. Provar fï¿½rst att tolka
 *      den inlï¿½sta strï¿½ngen som en strï¿½ngvariabel. Om
 *      ingen variabel av klassen parameter och typen strï¿½ng
 *      finns antas strï¿½ngen representera ett explicit
 *      vï¿½rde.
 *
 *      In: pnr    => Promt-strï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
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
 *      23/3/86  Prompt-strï¿½ng B. Doverud
 *      14/4/86  Bug, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    sttycl  type;
    PMLITVA litval;

/*
***Lï¿½s in vï¿½rde.
*/
    if ( (status=IGssip(IGgtts(pnr),IGgtts(266),istr,dstr,V3STRLEN)) < 0 )
       goto exit;
/*
***Analysera. Om det var ett MBS-strï¿½nguttryck ï¿½r allt ok.
*/
    if (anaexp(istr,FALSE,pexnpt,&type) == 0 && type == ST_STR)
      {
      status = 0;
      }
/*
***Inget MBS-strï¿½nguttryck, tï¿½m felstacken och skapa en literal.
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

/*      Lï¿½ser in ett strï¿½nguttryck och berï¿½knar vï¿½rdet.
 *
 *      In: pnr    => Promptstrï¿½ng nr, om 0 ingen utskrift.
 *          dstr   => Pekare till defaultstrï¿½ng.
 *          istr   => Pekare till inputstrï¿½ng.
 *          strval => Pekare till resultat.
 *
 *      Ut: *strval => Strï¿½ng.
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

/*      Genererar ett uttryck bestï¿½ende av aktionskod
 *      fï¿½r valt alternativ i en meny.
 *
 *      In: mnum   = Menynummer.
 *          pexnpt = Pekare till resultat.
 *
 *      Ut: *pexnpt = Strï¿½nguttryck.
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

/*      Huvudrutin fï¿½r att skapa en uttrycksnod fï¿½r
 *      en referens.
 *
 *      In: pnr    => Promt-strï¿½ng nr, om 0 ingen utskrift.
 *          ptyp   => Pekare till DBetype, typmask.
 *          pexnpt => Pekare till pm_ptr.
 *          pend   => Pekare till bool.
 *          pright => Pekare till bool.
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *          *pend   => TRUE om pekning i ï¿½nden.
 *          *pright => TRUE om pekning till hï¿½ger.
 *          *typ    => Typ av storhet.
 *
 *      FV:       0 => Ok.
 *           REJECT => Operationen avbruten.
 *           GOMAIN => Tillbaks till huvudmenyn
 *
 *      (C)microform ab 17/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      31/10/85 ï¿½nde och sida, J. Kjellander
 *      23/3/86  Prompt-strï¿½ng B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    PMREFVA idvek[MXINIV];
    PMLITVA litval;

/*
***Skriv ut ev. promtstrï¿½ng.
*/
    if (pnr > 0) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***Lï¿½s in ID fï¿½r refererad storhet.
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

/*      Huvudrutin fï¿½r att skapa en uttrycksnod fï¿½r
 *      en referens med mï¿½jlighet att tillï¿½ta extra
 *      utstatus.
 *
 *      In: pnr    => Promt-strï¿½ng nr, om 0 ingen utskrift.
 *          ptyp   => Pekare till DBetype, typmask.
 *          pexnpt => Pekare till pm_ptr.
 *          pend   => Pekare till bool.
 *          pright => Pekare till bool.
 *          utstat => Utstatus till IGgsid().
 *
 *      Ut: *pexnpt => PM-pekare till expression-nod.
 *          *pend   => TRUE om pekning i ï¿½nden.
 *          *pright => TRUE om pekning till hï¿½ger.
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
***Skriv ut ev. promtstrï¿½ng.
*/
    if ( pnr > 0 ) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***Lï¿½s in ID fï¿½r refererad storhet.
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

/*      Create a position expression (VECTOR).
 *
 *      In:   pnr    => Prompt number or 0 for no prompt.
 *
 *      Out: *pexnpt => PM ptr to VECTOR expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *             < 0 => Internal error.
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 *      1998-03-31 default, J.Kjellander
 *      2007-07-28 1.19, J.Kjellander
 *      2007-09-09 relpos, J.Kjelander
 *
 ******************************************************!*/

  {
    short status;

/*
***Remember current PM stack pointer for
***garbage collection if needed.
*/
    pmmark();
/*
***Output prompt.
*/
start:
    if ( pnr > 0 ) WPaddmess_mcwin(IGgtts(pnr),WP_PROMPT);
/*
***Relative or ordinary position ?
*/
    if ( relpos ) status = gnprel(pexnpt);
    else          status = gnpany(pexnpt);
/*
***Clear the promt.
*/
    WPclear_mcwin();
/*
***Change pos method maybe ? Collect garbage
***and try again.
*/
    if ( status == SMBPOSM )
      {
      pmrele();
      goto start;
      }
/*
***If status still is negative, collect garbage in PM.
*/
    if ( status < 0 ) pmrele();
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short IGcpov(DBVector *pos)

/*      Create a position literal value (VECTOR).
 *
 *      Out: *pos   => C ptr to VECTOR value.
 *
 *      Return: Returns the status from IGcpos().
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   pm_ptr  exnpt,valtyp;
   PMLITVA val;

/*
***Save PM stack pointer.
*/
   pmmark();
/*
***Create position expression.
*/
   if ( (status=IGcpos((short)0,&exnpt)) < 0 ) goto end;
/*
***Evaluate.
*/
   inevev(exnpt,&val,&valtyp);

   pos->x_gm = val.lit.vec_va.x_val;
   pos->y_gm = val.lit.vec_va.y_val;
   pos->z_gm = val.lit.vec_va.z_val;
/*
***Collect garbage in PM and exit.
*/
end:
   pmrele();
   return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpany(pm_ptr *pexnpt)

/*      Create a position expression (VECTOR).
 *
 *      Out: *pexnpt => PM ptr to VECTOR expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *             < 0 => Internal error.
 *
 *      (C)2007-09-09 J. Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Select method. Relative is not allowed here.
*/
    switch ( posmode )
      {
      case 0:  status = gnpabs(pexnpt); break;
      case 2:  status = gnpcrh(pexnpt); break;
      case 3:  status = gnpmbs(pexnpt); break;
      case 4:  status = gnpend(pexnpt); break;
      case 5:  status = gnpon(pexnpt);  break;
      case 6:  status = gnpcen(pexnpt); break;
      case 7:  status = gnpint(pexnpt); break;
      case 8:  status = gnpcrh(pexnpt); break;
      default: status = REJECT; break;
      }
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnprel(pm_ptr *pexnpt)

/*      Create a relative position expression.
 *
 *      Out: *pexnpt => PM ptr to expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu
 *
 *      (C)microform ab 9/8/85 J. Kjellander
 *
 *      23/3/86    Anrop IGcpos(pnr,... B. Doverud
 *      6/10/86    GOMAIN, J. Kjellander
 *      2007-09-09 relpos, J.Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr  exnpt1,exnpt2;
    short   status;

/*
***Base position. Turn off relpos during
***base position. Update the menu afterwards
***to ensure that the rel button is still highligted.
*/
     WPaddmess_mcwin(IGgtts(51),WP_PROMPT);
     relpos = FALSE;
     status = gnpany(&exnpt1);
     relpos = TRUE;
     WPupdate_menu();
     WPclear_mcwin();

     if ( status == SMBPOSM ) return(status);
     else if ( status < 0 ) goto end;
/*
***Offset "vec(dx,dy,dz)".
*/
     WPaddmess_mcwin(IGgtts(287),WP_PROMPT);
     status = gnpabs(&exnpt2);
     WPclear_mcwin();

     if ( status == SMBPOSM ) return(status);
     else if ( status < 0 ) goto end;
/*
***Make an expressions list "pos + vec(dx,dy,dz)".
*/
    pmcbie(PM_PLUS,exnpt1,exnpt2,pexnpt);
/*
***The end.
*/
end:
    return(status);
    }

/********************************************************/
/*!******************************************************/

static short gnpabs(pm_ptr *pexnpt)

/*      Position using absolute coordinates.
 *
 *      Out: *pexnpt => PM ptr to VEC(x,y,z)-expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *             < 0 => Internal error.
 *
 *      (C)microform ab 16/1/85 J. Kjellander
 *
 *      22/10/85   Längre inputsträngar, J. Kjellander
 *      6/10/86    GOMAIN, J. Kjellander
 *      8/10/86    Promt, J. Kjellander
 *      2007-09-09 relpos, J.Kjellander
 *
 ******************************************************!*/

  {
    char   ps[3][40];     /* prompt string */
    char   is[3][80];     /* input string */
    char  *psarr[3];
    char  *isarr[3];
    char  *dsarr[3];
    short  ml[3];         /* max lengths */
    sttycl type;
    char   expr[132];
    short  status;
    int    pnr;

    static char ds[3][80] = {"0","0","0"};

/*
***Init.
*/
    strcpy(ps[0],IGgtts(201));       /* copy prompts */
    strcpy(ps[1],IGgtts(202));
    strcpy(ps[2],IGgtts(203));

    ml[0]=80;
    ml[1]=80;
    ml[2]=80;

    psarr[0] = ps[0];                  /* init ptrs */
    psarr[1] = ps[1];
    psarr[2] = ps[2];

    isarr[0] = is[0];
    isarr[1] = is[1];
    isarr[2] = is[2];

    dsarr[0] = ds[0];
    dsarr[1] = ds[1];
    dsarr[2] = ds[2];
/*
***Main prompt.
*/
    if ( relpos ) pnr = 66;
    else          pnr = 326;
/*
***Get string values.
*/
loop:
    if ( (status=IGmsip(IGgtts(pnr),psarr,isarr,dsarr,ml,modtyp)) != 0 ) return(status);
    if ( modtyp == 2 ) strcpy(is[2],"0");
/*
***Save as default.
*/
    strcpy(ds[0],is[0]); 
    strcpy(ds[1],is[1]);
    strcpy(ds[2],is[2]);
/*
***Create VECTOR expression.
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
/*
***The end.
*/
    return(0);
    }

/********************************************************/
/*!******************************************************/

static short gnpcrh(pm_ptr *pexnpt)

/*      Position using mouse pointer or grid.
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
***Lï¿½s in hï¿½rkorsposition.
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

static short gnpmbs(pm_ptr *pexnpt)

/*      Position using MBS.
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
***Get string.
*/
loop:
    if ( (status=IGssip(IGgtts(209),IGgtts(267),istr,"",V3STRLEN)) != 0 ) goto end;
/*
***Analyze.
*/
    if ( anaexp(istr,FALSE,pexnpt,&type ) != 0 )
      {
      errmes();
      goto loop;
      }
/*
***Typecheck.
*/
    if (type != ST_VEC)
      {
      erpush("IG2232",IGgtts(7));
      errmes();
      goto loop;
      }
/*
***The end.
*/
end:
    return(status);
    }

/********************************************************/
/*!******************************************************/

static short gnpend(pm_ptr *pexnpt)

/*      Create ENDP(ref)/STARTP(ref) position expression.
 *
 *      Out: *pexnpt => PM ptr to expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *
 *      (C)microform ab 1/11/85 J. Kjellander
 *
 *      20/3/86    Anrop pmtcon, pmclie B. Doverud
 *      6/10/86    GOMAIN, J. Kjellander
 *      1997-04-21 STARTP/ENDP, J.Kjellander
 *      2007-09-29 Removed prompt, J.Kjellander
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
***Temporary reference (MACRO call for example).
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
***True reference.
*/
    else
      {
/*
***Create STARTP/ENDP function expression.
*/
      typ = LINTYP+ARCTYP+CURTYP;
      if ( (status=genrfs(0,&typ,&exnpt,&end,&right,(short)0)) != 0 ) goto exit;

      pmtcon(exnpt,(pm_ptr)NULL,&arglst,&dummy);

      if ( end ) stlook("ENDP",&kind,&ref);
      else       stlook("STARTP",&kind,&ref);

      pmcfue(ref,arglst,pexnpt);
      }
/*
***The end.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpon(pm_ptr *pexnpt)

/*      Create ON(ref), ON(ref,t) or ON(ref,uv) position
 *      expression.
 *
 *      Out: *pexnpt => PM ptr to expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *
 *      (C)microform ab 16/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      1998-09-24 b_plan, J.Kjellander
 *      2007-09-29 Removed prompt, J.Kjellander
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
***Temp ref.
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
***True ref.
*/
    else
      {
/*
***Create ref.
*/
      typ = POITYP+LINTYP+ARCTYP+CURTYP+CSYTYP+BPLTYP+
            TXTTYP+LDMTYP+CDMTYP+RDMTYP+ADMTYP+SURTYP;
      if ( (status=genrfs(0,&typ,&exnpt1,&end,&right,(short)0)) != 0 ) goto exit;
      pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
/*
***Add parameter value for wireframe geometry.
*/
      if ( typ == LINTYP  ||  typ == ARCTYP  ||  typ == CURTYP )
        {
        if ( (status=IGcflt((short)208,dstr,istr,&exnpt2)) < 0 ) goto exit;
        pmtcon(exnpt2,retla,&arglst,&dummy);
        strcpy(dstr,istr);
        }
/*
***Add U,V values for surfaces.
*/
      else if ( typ == SURTYP  ||  typ == BPLTYP )
        {
        if ( (status=genuv((short)208,&exnpt2)) < 0 ) goto exit;
        pmtcon(exnpt2,retla,&arglst,&dummy);
        }
/*
***Nothing to ad for other entities.
*/
      else arglst = retla;
/*
***Create function expression.
*/
      stlook("ON",&kind,&ref);
      pmcfue(ref,arglst,pexnpt);
      }
/*
***The end.
*/
exit:
    return(status);
  }

/********************************************************/
/*!******************************************************/

static short gnpint(pm_ptr *pexnpt)

/*      Create INTERSECT(ref1,ref2,alt) position expression.
 *
 *      Out: *pexnpt => PM ptr to expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *
 *      (C)microform ab 10/1/85 J. Kjellander
 *
 *      28/10/85 Ny def. av PMLITVA, J. Kjellander
 *      28/10/85 Ände och sida, J. Kjellander
 *      6/3/86   Ny defaulthantering, B. Doverud
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      7/10/86  GOMAIN, J. Kjellander
 *      20/11/89 Neg. intnr, J. Kjellander
 *      23/12/91 Bplan och koord.sys, J. Kjellander
 *      7/9/95   Ytor, J. Kjellander
 *      2007-09-29 Removed prompt, J.Kjellander
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
***Temp ref.
*/
    if ( tmpref || (sysmode == EXPLICIT) )
      {
      if ( (status=igpint(&posvec)) != 0 ) goto exit;

      litstr.lit_type = C_VEC_VA;
      litstr.lit.vec_va.x_val = posvec.x_gm;
      litstr.lit.vec_va.y_val = posvec.y_gm;
      litstr.lit.vec_va.z_val = posvec.z_gm;
      pmclie( &litstr, pexnpt);
      }
/*
***True ref.
*/
    else
      {
/*
***Create the two refs.
*/
      typ1 = LINTYP+ARCTYP+CURTYP;
      if ( modtyp == 3 ) typ1 += BPLTYP+CSYTYP+SURTYP;
      if ( (status=genrfs(0,&typ1,&exnpt1,&end,
                          &right,(short)0)) != 0 ) goto exit;

      typ2 = LINTYP+ARCTYP+CURTYP;
      if ( modtyp == 3  &&  typ1 != BPLTYP  &&
                            typ1 != CSYTYP  &&
                            typ1 != SURTYP )
        typ2 += BPLTYP+CSYTYP+SURTYP;
      if ( (status=genrfs(0,&typ2,&exnpt2,&end,
                          &right,(short)0)) != 0 ) goto exit;
/*
***If line/line, alt = -1.
*/
      if ( typ1 == LINTYP && typ2 == LINTYP )
         {
         litstr.lit_type = C_INT_VA;
         litstr.lit.int_va = -1;
         pmclie( &litstr, &exnpt3);
         }
/*
***If not, ask user for alt value.
*/
      else
         {
         if ( (status=IGcint(327,"1",istr,&exnpt3)) < 0 ) goto exit;
         }
/*
***Create expression.
*/
      pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
      pmtcon(exnpt2,retla,&retla,&dummy);
      pmtcon(exnpt3,retla,&arglst,&dummy);

      stlook("INTERSECT",&kind,&ref);
      pmcfue( ref, arglst, pexnpt);
      }
/*
***The end.
*/
exit:
    return(status);

  }

/********************************************************/
/*!******************************************************/

static short gnpcen(pm_ptr *pexnpt)

/*      Create CENTRE(ref,t) position expression.
 *
 *      Out: *pexnpt => PM ptr to expression.
 *
 *      Return:  0 => Ok.
 *          REJECT => Operation cancelled.
 *          GOMAIN => Back to main menu.
 *
 *      (C)microform ab 26/4/87 J. Kjellander
 *
 *      2007-09-29 Removed prompt, J.Kjellander
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

    static char dstr[V3STRLEN+1] = "0.0";

/*
***Temp ref.
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
***True ref.
*/
    else
      {
/*
***Create reference.
*/
      typ = ARCTYP+CURTYP;
      if ( (status=genrfs(0,&typ,&exnpt1,&end,&right,(short)0)) != 0 ) goto exit;
/*
***An arc can use t=0.
*/
      if ( typ == ARCTYP )
        {
        litstr.lit_type = C_FLO_VA;
        litstr.lit.float_va = 0.0;
        pmclie( &litstr, &exnpt2);
        }
/*
***If it's a curve, ask user for t value.
*/
      else
        {
        if ( (status=IGcflt(208,dstr,istr,&exnpt2)) < 0 ) goto exit;
        strcpy(dstr,istr);
        }
/*
***Create expression.
*/
      pmtcon( exnpt1, (pm_ptr)NULL, &retla, &dummy);
      pmtcon( exnpt2, retla, &arglst, &dummy);

      stlook("CENTRE",&kind,&ref);
      pmcfue( ref, arglst, pexnpt);
      }
/*
***The end.
*/
exit:
    return(status);
  }

/********************************************************/
/********************************************************/

        short    IGgsid(
        DBId    *idvek,
        DBetype *typ,
        bool    *end,
        bool    *right,
        short    utstat)

/*      Select entity (pick with mouse).
 *
 *      Med utstat <> 0 tillåts
 *      rutinen avsluta med status = utstat förutom de
 *      vanliga REJECT och GOMAIN.
 *
 *      In: idvek  => Ptr to array of MXINIV DBId
 *          typ    => Requested type (mask)
 *          end    => End, TRUE/FALSE
 *          right  => Side, TRUE/FALSE
 *          utstat => 0 or WINDOW
 *
 *      Out: *idvek => ID (linked list).
 *           *typ   => Typecode.
 *
 *      Return:   0  => Ok, ID returned
 *           REJECT  => Operation cancelled
 *           GOMAIN  => Main menu
 *           SMBPOSM => Pos button
 *           IG3532  => Entity does not belong to a part
 *           IG2242  => Syntax error in ID string
 *           IG2272  => Entity %s does not exist
 *
 *      (C)microform ab 4/2/85 J. Kjellander
 *
 *      28/10/85 Ände och sida, J. Kjellander
 *      30/12/85 Pekmärke, J. Kjellander
 *      30/12/85 Symbol, J. Kjellander
 *      10/3/86  Part, J. Kjellander
 *      13/3/86  Pektecken "i", J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      20/10/86 HELP, J. Kjellander
 *      23/12/86 Global ref, J. Kjellander
 *      27/8/87  B_plan, J. Kjellander
 *      17/11/88 utstat, J. Kjellander
 *      10/1-95  Multifönster, J. Kjellander
 *      1996-04-30 Pekning på part, J. Kjellander
 *      1997-04-10 WIN32, J.Kjellander
 *      2006-12-08 Tagit bort gpgtla(), J.Kjellander
 *      2008-07-08 Comments, J.Kjellander
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
***Inmatning av ID frï¿½n tangentbordet. Om global identitet
***matas in (##id) returnerar vi ï¿½ndï¿½ lokal eftersom mï¿½nga
***rutiner tex. pmlges() krï¿½ver detta.
*/
    else if ( pektkn == *IGgtts(97) )
      {
inid:
      status = IGssip("",IGgtts(283),idstr,"",V3STRLEN);
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
***Otillï¿½tet pektecken.
*/
    else goto typerr;
/*
***Gï¿½r sï¿½kning i df och skapa lokal ID.
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
***Om bestï¿½lld typ = part eller pektkn fï¿½r part
***anvï¿½nts ser vi till att returnera identiteten fï¿½r en
***part och inte storheten som ingï¿½r i parten.
*/
    if ( *typ == PRTTYP || pektkn == *IGgtts(95) )
      {
/*
***Om id bara har en nivï¿½ kan det vara en enkel storhet i
***aktiv modul. Detta ï¿½r inte tillï¿½tet. Vi kollar det med
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
***Returnera rï¿½tt typ.
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

/*      Lï¿½s in position med hï¿½rkors.
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

/*      Lï¿½s in 3D position med hï¿½rkors.
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

/*      Läs in 2D position med hårkors eller
 *      position på en rasterpunkt. Positionen 
 *      transformeras till aktivt koordinatsystem.
 *      <SP> och h ger hårkorsposition.
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
 *      9/9/85   Hårkors/raster sammanslaget, R. Svedin
 *      30/12/85 Pekmärke, J. Kjellander
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
/********************************************************/

 static short igpend(DBVector *vecptr)

/*      Explicit position at the end of a line/arc/curve.
 *
 *      In: vecptr => Ptr to output coordinates.
 *
 *      Out: *vecptr => The position (coordinates).
 *
 *      Return:   0 => Ok, coordinates returned
 *          SMBPOSM => Position method
 *          REJECT  => Operation rejected
 *          GOMAIN  => Main menu
 *
 *      (C)microform ab 28/10/85 J. Kjellander
 *
 *      26/3/86    Felutskrift från EXon, B. Doverud
 *      6/10/86    GOMAIN, J. Kjellander
 *      2008-07-08 SMBPOSM, J.Kjellander
 *
 ********************************************************/

  {
    DBfloat t;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];
    DBAny   gmpost;

/*
***Get the ID of the entity.
*/
loop:
    typ = LINTYP+ARCTYP+CURTYP;
    WPaddmess_mcwin(IGgtts(331),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Select parameter value.
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
***Calculate explicit position.
*/
    if ( EXon(idvek,t,(DBfloat)0.0,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***The end.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/********************************************************/

 static short igpon(DBVector *vecptr)

/*      Explicit position on entity.
 *
 *      In: vecptr => Ptr to output coordinates.
 *
 *      Out: *vecptr => The position (coordinates).
 *
 *      Return:   0 => Ok, coordinates returned
 *          SMBPOSM => Position method
 *          REJECT  => Operation rejected
 *          GOMAIN  => Main menu
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      28/10/85 Ände och sida, J. Kjellander
 *      29/12/85 Symbol, J. Kjellander
 *      26/3/86  Felutskrift från EXon, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      22/2/93  Nytt anrop till EXon(), J. Kjellander
 *      2008-07-08 SMBPOSM, J.Kjellander
 *
 ********************************************************/

  {
    double  t;
    DBetype typ;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];

/*
***Get the ID of the entity.
*/
loop:
    typ = POITYP+LINTYP+ARCTYP+CURTYP+CSYTYP+
          TXTTYP+LDMTYP+CDMTYP+RDMTYP+ADMTYP;
    WPaddmess_mcwin(IGgtts(52),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Select parameter value.
*/
    if ( typ == POITYP || typ == CSYTYP || typ == TXTTYP ||
         typ == LDMTYP || typ == CDMTYP || typ == RDMTYP ||
         typ == ADMTYP ) t = 0.0;
    else
       {
       WPaddmess_mcwin(IGgtts(208),WP_PROMPT);
       status=IGsfip(IGgtts(320),&t);
       if ( status < 0 ) goto exit;
       }
/*
***Calculate explicit position.
*/
    if ( EXon (idvek,t,(DBfloat)0.0,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***The end.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/********************************************************/

static short igpint(DBVector *vecptr)

/*      Explicit intersect position.
 *
 *      In: vecptr => Ptr to output coordinates.
 *
 *      Out: *vecptr => The position (coordinates).
 *
 *      Return:   0 => Ok, coordinates returned
 *          SMBPOSM => Position method
 *          REJECT  => Operation rejected
 *          GOMAIN  => Main menu
 *
 *      (C)microform ab 4/2/85 J. Kjellander 
 *
 *      28/10/85 Ände och sida, J. Kjellander
 *      26/3/86  Felutskrift från EXsect, B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      26/11/89 Neg intnr, J. Kjellander
 *      2008-07-08 SMBPOSM, J.Kjellander
 *
 ********************************************************/

  {
    short   alt,status;
    int     ival;
    DBetype typ1,typ2;
    bool    end,right,simple;
    DBId    idv1[MXINIV],idv2[MXINIV];

/*
***Get the ID of the first entity.
*/
loop:
    WPaddmess_mcwin(IGgtts(324),WP_PROMPT);
    typ1 = LINTYP+ARCTYP+CURTYP+BPLTYP+CSYTYP;
    if ( (status=IGgsid(idv1,&typ1,&end,&right,(short)0)) != 0 ) goto exit;
    WPclear_mcwin();
/*
***Get the ID of the second entity.
*/
    typ2 = LINTYP+ARCTYP+CURTYP;
    if ( typ1 != BPLTYP  &&  typ1 != CSYTYP ) typ2 += BPLTYP+CSYTYP;
    WPaddmess_mcwin(IGgtts(325),WP_PROMPT);
    if ( (status=IGgsid(idv2,&typ2,&end,&right,(short)0)) != 0 ) goto exit;
    WPclear_mcwin();
/*
***Intersect line/line means that only one intersect
***position is possible and that we can use alt = -1.
*/
    if ( typ1 == LINTYP  &&  typ2 == LINTYP ) alt = -1;
/*
***How many possible intersects ? simple = TRUE means 2,
***simple = FALSE means more.
*/
    else
      {
      if ( typ1 == LINTYP  &&  typ2 == ARCTYP )
        {
        simple = TRUE;
        }
      else if ( typ1 == ARCTYP  &&  typ2 == LINTYP )
        {
        simple = TRUE;
        }
      else if ( typ1 == ARCTYP  &&  typ2 == ARCTYP )
        {
        simple = TRUE;
        }
      else simple = FALSE;
/*
***With only 2 possible intersects ask for first or second.
*/
      if ( simple )
        {
        if ( IGialt(160,161,162,FALSE) ) alt = 1;
        else alt = 2;
        }
/*
***If not, ask for alternative.
*/
      else
        {
        WPaddmess_mcwin(IGgtts(327),WP_PROMPT);
        if ( (status=IGsiip(IGgtts(46), &ival)) < 0 ) goto exit;
        alt = (short)ival;
        }
      }
/*
***Calculate explicit position.
*/
    if ( EXsect (idv1,idv2,alt,0,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***The end.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/********************************************************/

 static short igpcen(DBVector *vecptr)

/*      Explicit center position.
 *
 *      In: vecptr => Ptr to output coordinates.
 *
 *      Out: *vecptr => The position (coordinates).
 *
 *      Return:   0 => Ok, coordinates returned
 *          SMBPOSM => Position method
 *          REJECT  => Operation rejected
 *          GOMAIN  => Main menu
 *
 *      (C)microform ab 26/4/87 J. Kjellander
 *
 *      2008-07-08 SMBPOSM, J.Kjellander
 *
 ********************************************************/

  {
    double  t;
    DBetype typ;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];

/*
***Get the ID of the entity.
*/
loop:
    typ = ARCTYP+CURTYP;
    WPaddmess_mcwin(IGgtts(53),WP_PROMPT);
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) != 0 ) goto exit;
/*
***Select parameter value.
*/
    if ( typ == ARCTYP ) t = 0.0;
    else
       {
       WPaddmess_mcwin(IGgtts(208),WP_PROMPT);
       status=IGsfip(IGgtts(320),&t);
       if ( status < 0 ) goto exit;
       }
/*
***Calculate explicit position.
*/
    if ( EXcen(idvek,t,lsyspk,vecptr) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***The end.
*/
exit:
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
