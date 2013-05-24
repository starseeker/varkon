/**********************************************************************
*
*    evtime.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evtime();         evaluate TIME
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
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;    /* Access structure for MBS routines */

/*!******************************************************/

        short evtime()

/*      Evaluerar proceduren TIME.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short   status,i;
    int     y,mo,d,h,mi,s;
    PMLITVA litval[6];

/*
***Hämta datum och tid.
*/
    if ( (status=EXtime(&y,&mo,&d,&h,&mi,&s)) < 0 )
      return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
    litval[0].lit.int_va = y;
    litval[1].lit.int_va = mo;
    litval[2].lit.int_va = d;
    litval[3].lit.int_va = h;
    litval[4].lit.int_va = mi;
    litval[5].lit.int_va = s;
/*
***Skriv parametervärden till motsvarande MBS variabler.
*/
    for ( i=0; i<6; ++i )
      {
      inwvar(proc_pv[i+1].par_ty, proc_pv[i+1].par_va.lit.adr_va,
             0, NULL, &litval[i]);
      }

    return(0);
  }

/*!******************************************************/
