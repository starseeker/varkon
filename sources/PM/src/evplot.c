/**********************************************************************
*
*    evplot.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    evplwi();     Evaluates MBS procedure PLOT_WIN()
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;  /* Access structure for MBS routines */
extern short    proc_pc;  /* Number of actual parameters */

/*!******************************************************/

        short evplwi()

/*      Evaluates MBS procedure 
 *      PLOT_WIN(win_id,minpos,maxpos,filename,[base_pos]);
 *
 *      (C)2007-04-08 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***If number of parameters = 5, a plot origin is supplied.
*/
   if ( proc_pc == 5 )
     {
     return(EXplwi((DBint)      proc_pv[1].par_va.lit.int_va,
                   (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                   (DBVector *)&proc_pv[3].par_va.lit.vec_va,
                                proc_pv[4].par_va.lit.str_va,
                   (DBVector *)&proc_pv[5].par_va.lit.vec_va));
     }
/*
***If not, the lower left corner should be used.
*/
   else
     {
     return(EXplwi((DBint)      proc_pv[1].par_va.lit.int_va,
                   (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                   (DBVector *)&proc_pv[3].par_va.lit.vec_va,
                                proc_pv[4].par_va.lit.str_va,
                   (DBVector *)&proc_pv[2].par_va.lit.vec_va));
     }
  }

/********************************************************/
