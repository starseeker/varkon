/**********************************************************************
*
*    evexit.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    short evexit()    evaluate procedure EXIT
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
extern short    proc_pc;    /* Number of actual parameters */

/*!******************************************************/

        short evexit()

/*      Evaluate procedure EXIT.
 *
 *      In:   extern proc_pv => Pekare till array med parametervärden
 *            extern proc_pc => 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***Returvärde = 3 => Slut med felmeddelande. Partanrop lagras ej.
***             4 => Slut utan felmeddelande. Partanrop lagras.
*** 1 används för GOTO och 2 för IF.
*/
   if ( proc_pc == 1 )
     {
     if ( strlen(proc_pv[1].par_va.lit.str_va) > 0 )
       {
       erpush("IN2292",proc_pv[1].par_va.lit.str_va);
       errmes();
       }
     return(3);
     }
   else return(4);
  }

/********************************************************/
