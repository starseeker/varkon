/**********************************************************************
*
*    evvfile.c
*    =========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evclgm();     Evaluerar CLEAR_GM
*    evldgm();     Evaluates LOAD_GM
*    evsvgm();     Evaluates SAVE_GM
*    evldjb();     Evaluates LOAD_JOB
*    evsvjb();     Evaluates SAVE_JOB
*    evlmdf();     Evaluates LOAD_MDF
*    evldpm();     Evaluates LOAD_PM 
*    evgenv();     Evaluates GET_ENVIRONMENT
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#ifdef UNIX
#include "../../WP/include/WP.h"
#endif
#include "../../EX/include/EX.h"
#include <string.h>

extern char     jobnam[];
extern short    v3mode,modtyp;
extern pm_ptr   actmod;
extern struct   ANSYREC sy;
extern V2NAPA   defnap;
extern DBseqnum snrmax;

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pointer to results. */


/*!******************************************************/

        short evclgm()

/*      Evaluerar proceduren CLEAR_GM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return( EXclgm() );
  }

/********************************************************/
/*!******************************************************/

        short evldgm()

/*      Evaluerar proceduren LOAD_GM.
 *
 *      In:   extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXldgm(&proc_pv[1].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evsvgm()

/*      Evaluerar proceduren SAVE_GM.
 *
 *      In:   extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXsvgm(&proc_pv[1].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evldjb()

/*      Evaluerar proceduren LOAD_JOB.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/10/88 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXload_job(&proc_pv[1].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evsvjb()

/*      Evaluerar proceduren SAVE_JOB.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/10/88 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXsave_job(&proc_pv[1].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evlmdf()

/*      Evaluerar proceduren LOAD_MDF.
 *
 *      In:   extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      Felkod: IN5252 = Menyfilen %s finns ej 
 *
 *      (C)microform ab 1996-05-21 J.Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    if ( IGlmdf(proc_pv[1].par_va.lit.str_va) < 0 )
      return(erpush("IN5252",proc_pv[1].par_va.lit.str_va));
    else
      return(0);
  }

/********************************************************/
/*!******************************************************/

        short evldpm()

/*      Evaluerar LOAD_PM.
 *
 *      In: extern proc_pv   = Pekare till parametrar.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1998-04-03 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short    status,oldmty;
    pm_ptr   oldmod;
    char    *oldrtb,*oldrsp;
    char     curmna[V2SYNLEN+1];
    PMMODULE modhed;
    PMMONO  *np;

/*
***I ritmodulen laddar man inga moduler !
*/
    if ( v3mode == RIT_MOD ) return(0);
/*
***Vad heter nuvarande modul. Måste vi veta för att
***kunna anropa pmgeba() på slutet.
*/
    pmgmod((pm_ptr)0,&np);
    strcpy(curmna,pmgadr(np->mona_));
/*
***Spara nuvarande MACRO-moduls PM-baspekare och sätt 
***om den till aktiv modul.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
/*
***Spara nuvarande MACRO-moduls RTS-bas och stack-pekare. 
*/
    oldrtb = ingrtb();
    oldrsp = ingrsp();
/*
***Spara nuvarande MACRO-moduls modultyp och sätt om till
***den som aktiv modul har.
*/
   oldmty = modtyp;
   pmrmod(&modhed);
   modtyp = modhed.mtype;
/*
***Ladda.
*/
   status = pmload(proc_pv[1].par_va.lit.str_va,&actmod);
   if ( status < 0 ) erpush("IG4062","");
/*
***Se till att den får rätt namn.
*/
   pmrmod(&modhed);
   strcpy(modhed.mname,jobnam);
   pmumod(&modhed);
/*
***Sätt snrmax.
*/
   snrmax = modhed.idmax;
/*
***Gör rätt modul aktiv igen.
*/
   modtyp = oldmty;
   insrtb(oldrtb);
   insrsp(oldrsp);
   pmgeba(curmna,&oldmod);
   pmsbla(oldmod);
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgenv()

/*      Evaluerar GET_ENVIRONMENT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   char *strpek;

/*
***Prova att hämta.
*/
   strpek = IGenv3(func_pv[1].par_va.lit.str_va);
/*
***Om den inte finns returnerar vi tom sträng "".
*/ 
   if ( strpek == NULL ) func_vp->lit.str_va[0] = '\0';
/*
***Finns den gör vi säker kopiering. Max. V3STRLEN tecken.
*/
   else
     {
     strncpy(func_vp->lit.str_va,strpek,V3STRLEN);
             func_vp->lit.str_va[V3STRLEN] = '\0';
     }

   return(0);
 }

/********************************************************/
