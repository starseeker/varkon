/**********************************************************************
*
*    evplot.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evplvi();     Evaluerar PLOT_VIEW
*    evplwi();     Evaluerar PLOT_WIN
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

        short evplvi()

/*      Evaluerar proceduren PLOT_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 3/11/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXplvi(&proc_pv[1].par_va.lit.str_va[0],
                  &proc_pv[2].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evplwi()

/*      Evaluerar proceduren PLOT_WIN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 16/2/88 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

/*
***Om antalet parametrar är 4 har plot-origo utryckligen
***getts som indata. ( 4:e parametern )
*/
    if ( proc_pc == 4 )
      {
      return(EXplwi((DBVector *)&proc_pv[1].par_va.lit.vec_va,
                    (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                                 proc_pv[3].par_va.lit.str_va,
                    (DBVector *)&proc_pv[4].par_va.lit.vec_va));
      }
/*
***Om inte skall nedre vänstra hörnet på plotområdet
***användas. ( 1:a parametern )
*/
    else
      {
      return(EXplwi((DBVector *)&proc_pv[1].par_va.lit.vec_va,
                    (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                                 proc_pv[3].par_va.lit.str_va,
                    (DBVector *)&proc_pv[1].par_va.lit.vec_va));
      }

  }

/********************************************************/
