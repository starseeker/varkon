/**********************************************************************
*
*    evcsys.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short evmolo()    evaluate procedure mode_local
*    short evmogl()    evaluate procedure mode_global
*    short evmoba()    evaluate procedure mode_basic 
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

extern V2NAPA  defnap;      /* Gloabal area for named parameter values */

extern PMPARVA *proc_pv;    /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;    /* inproc.c parcount Number of actual parameters */
extern pm_ptr   proc_argl;  /* inproc.c arglist  Pekare till aktuell parameterlista */


/********************************************************/
/*!******************************************************/

        short evmolo()

/*      Evaluate procedure MODE_LOCAL.
 *
 *      In:   extern proc_pv => Pekare till array med parametervärden
 *
 *      Out: 
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
   short status;
/*
***execute MODE_LOCAL 
*/
   if ( ( status = EXmolo(  &proc_pv[1].par_va.lit.ref_va[0] ) ) < -1 )
      return( status );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evmogl()

/*      Evaluate procedure MODE_GLOBAL.
 *
 *      In:   
 *
 *      Out: 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(EXmogl());
  }

/********************************************************/
/*!******************************************************/

        short evmoba()

/*      Evaluate procedure MODE_BASIC.
 *
 *      In: 
 *
 *      Out: 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
    return(EXmoba());
  } 

/********************************************************/
