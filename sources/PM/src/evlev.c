/**********************************************************************
*
*    evlev.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evbllv();     Evaluerar BLANK_LEV
*    evublv();     Evaluerar UNBLANK_LEV
*    evgtlv();     Evaluerar GET_LEV
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
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evbllv()

/*      Evaluerar proceduren BLANK_LEV.
 *
 *      In:  extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      17/1-95  Multifönster, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXbllv(proc_pv[1].par_va.lit.int_va,
                  proc_pv[2].par_va.lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evublv()

/*      Evaluerar proceduren UNBLANK_LEV.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      17/1-95  Multifönster, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXublv(proc_pv[1].par_va.lit.int_va,
                  proc_pv[2].par_va.lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evgtlv()

/*      Evaluerar proceduren GET_LEV.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
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
    bool    blank;
    char    name[NIVNLN+1];
    short   status;
    PMLITVA litval[2];

/*
***Hämta nivå-info.
*/
    if ( (status=EXgtlv((short)proc_pv[1].par_va.lit.int_va,
		                      &blank,name)) < 0 )
          return(status);;
/*
***Kopiera till PMLITVA.
*/
    if ( blank ) litval[0].lit.int_va = 1;
    else         litval[0].lit.int_va = 0;
    strcpy(litval[1].lit.str_va,name);
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
    evwval(litval,2,proc_pv);
/*
***Slut.
*/
    return(0);

  }

/********************************************************/
