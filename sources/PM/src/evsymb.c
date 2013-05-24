/**********************************************************************
*
*    evsymb.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evsymb();      Evaluerar SYMB
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

extern V2REFVA *geop_id;  /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv;  /* ingeop.c *pv      Access structure for MBS routines */
extern short    geop_pc;  /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np;  /* ingeop.c *npblock Pekare till namnparameterblock.*/

/*!******************************************************/

        short evsymb()

/*      Evaluerar geometri-proceduren SYMB.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

  {
/*
***Exekvera.
*/
     return(EXsymb(  geop_id, geop_pv[1].par_va.lit.str_va,
                 (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                              geop_pv[3].par_va.lit.float_va,
                              geop_pv[4].par_va.lit.float_va,
                              geop_np));
  }

/********************************************************/
