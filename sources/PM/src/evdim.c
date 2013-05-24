/**********************************************************************
*
*    evdim.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evldim();      Evaluerar LDIM
*    evcdim();      Evaluerar CDIM
*    evrdim();      Evaluerar RDIM
*    evadim();      Evaluerar ADIM

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

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */


/*!******************************************************/

        short evldim()

/*      Evaluerar geometri-proceduren LDIM.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Execute LDIM.
*/
   return(EXldim( geop_id,(DBVector *)&geop_pv[1].par_va.lit.vec_va, /* pos1 */
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va, /* pos2 */
                           (DBVector *)&geop_pv[3].par_va.lit.vec_va, /* pos3 */
                                (short)geop_pv[4].par_va.lit.int_va, /* alt */
                                       geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evcdim()

/*      Evaluerar geometri-proceduren CDIM.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Execute CDIM.
*/
   return(EXcdim( geop_id, &geop_pv[1].par_va.lit.ref_va[0], /* ref */
               (DBVector *)&geop_pv[2].par_va.lit.vec_va,    /* pos */
                     (short)geop_pv[3].par_va.lit.int_va,    /* alt */
                            geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evrdim()

/*      Evaluerar geometri-proceduren RDIM.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Execute RDIM.
*/
   return(EXrdim( geop_id, &geop_pv[1].par_va.lit.ref_va[0], /* ref */
               (DBVector *)&geop_pv[2].par_va.lit.vec_va,    /* pos1 */
               (DBVector *)&geop_pv[3].par_va.lit.vec_va,    /* pos2 */
                            geop_np));
 }

/********************************************************/
/*!******************************************************/

       short evadim()

/*      Evaluerar geometri-proceduren ADIM.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Execute ADIM.
*/
   return(EXadim( geop_id, &geop_pv[1].par_va.lit.ref_va[0], /* ref1 */
                           &geop_pv[2].par_va.lit.ref_va[0], /* ref2 */
               (DBVector *)&geop_pv[3].par_va.lit.vec_va,    /* pos */
                     (short)geop_pv[4].par_va.lit.int_va,    /* alt */
                            geop_np));
 }

/********************************************************/
/********************************************************/
