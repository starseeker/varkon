/**********************************************************************
*
*    evcray.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evcrnp();     Evaluates CRAY_NUM_PES
*    evcrmp();     Evaluates CRAY_MY_PE
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
#include <sys/utsname.h>
#endif

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evcrnp()

/*      Evaluates CRAY_NUM_PES() on the T3E.
 *      On all other platforms it returns 1.
 *
 *      Out: *valp  = Number of PE:s currently executing.
 *
 *      (C)microform ab 1999-04-20 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Alla UNIX-system bör ha uname().
*/
#ifdef _CRAYT3E
    func_vp->lit.int_va = _num_pes();
#else
    func_vp->lit.int_va = 1;
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evcrmp()

/*      Evaluates CRAY_MY_PE() on the T3E.
 *      On all other platforms it returns 0.
 *
 *      Out: *valp  = Number of this PE.
 *
 *      (C)microform ab 1999-04-20 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***Alla UNIX-system bör ha uname().
*/
#ifdef _CRAYT3E
    func_vp->lit.int_va = _my_pe();
#else
    func_vp->lit.int_va = 0;
#endif

    return(0);
  }

/********************************************************/
