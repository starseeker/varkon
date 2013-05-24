/**********************************************************************
*
*    infunc.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*      inevfu();     Interpreterar funktion
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
#include "../include/evfuncs.h"
#include <math.h>

PMPARVA *func_pv;   /* Access structure for MBS routines */
short    func_pc;   /* Number of actual parameters */
PMLITVA *func_vp;   /* Ptr to result. */
pm_ptr  *func_ty;   /* Ptr to result type. */
pm_ptr   func_al;   /* Ptr to actual parameter list. */

/*!******************************************************/

        short inevfu(
        pm_ptr routla,
        pm_ptr arglist,
        PMLITVA *valp,
        pm_ptr *tylap)

/*      Central rutin för interpretering av funktioner.
 *
 *      In: routla  => Pekare till rutin.
 *          arglist => Pekare till aktuell parameterlista.
 *          func_vp => Pekare till resultat.
 *          tylap   => Pekare till resultattyp.
 *
 *      Ut: *valp   => Resultat.
 *          *tylap  => Typ av resultat.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 23/3/86 J. Kjellander 
 *
 ******************************************************!*/

 {
   PMPARVA *cur_pv;
   short    cur_pc;
   PMLITVA *cur_vp;
   pm_ptr  *cur_ty;
   pm_ptr   cur_al;
   STPROC   rout;        /* Access structure for MBS routines */
   short    status;

/*
***Läs rutin-info från symboltabellen.
*/
   if ( ( status = strrou( routla, &rout ) ) < -1 )
      return( status );
/*
***Save global variables that might change during recursive calls.
*/
   func_vp = valp;
   func_ty = tylap;
   func_al = arglist;

   cur_pv = func_pv;
   cur_pc = func_pc;
   cur_vp = func_vp;
   cur_ty = func_ty;
   cur_al = func_al;
/*
***Evaluera funktionens parameterlista.
*/
   if ( ( status = inapar( arglist, routla, &rout, &cur_pv, &cur_pc ) ) != 0 )
      {
      inrpar();
      return(status);
      }
/*
***Reset global variables.
*/
   func_pv = cur_pv;
   func_pc = cur_pc;
   func_vp = cur_vp;
   func_ty = cur_ty;
   func_al = cur_al;
/*
***Resultat-typ.
*/
   func_vp->lit_type = func_pv[0].par_va.lit_type;
   *tylap = func_pv[0].par_ty;
/*
***Exekvera funktionen.
*/
   status = (*functab[rout.kind_pr])();
/*
***Frigör parameter-pool utrymme.
*/
   inrpar();                     /* release parameter pool space */
/*
***Felkontroll.
*/
   if ( status < -1 ) return(status);
   else return(0);
 }

/************************************************************************/
