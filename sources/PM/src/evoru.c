/**********************************************************************
*
*    evoru.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evorrstd();      Evaluerar ORU_RSTIMEDIFF
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
*    (C) 2004.... Sören Larsson, Örebro University                             
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern PMLITVA *func_vp;  /* Pekare till resultat. */



/*!******************************************************/

        short evorrstd()

/*      Interface-routine for ORU_RSTIMEDIFF
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-05-12 Sören Larsson, Örebro University
 *
 ******************************************************!*/
 
   {
   short   status;
   PMLITVA litval3;
   PMLITVA litval4;
   
   status = EXorurstimediff(func_pv[1].par_va.lit.int_va,
                            func_pv[2].par_va.lit.int_va,
                           &litval3.lit.float_va,
                           &litval4.lit.float_va,
                           &func_vp->lit.int_va);

   if ( status < 0 ) return(status);
   
   /*
   ***Write to MBS-variable 'diff' and robtime.
   */
   status = inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval3);
   if ( status < 0 ) return(status);
   return(inwvar(func_pv[4].par_ty,func_pv[4].par_va.lit.adr_va,0,NULL,&litval4));
   }



                         
                        
