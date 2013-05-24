/**********************************************************************
*
*    evarc.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evarfr();     Evaluerar ARC_1POS
*    evar2p();     Evaluerar ARC_2POS
*    evar3p();     Evaluerar ARC_3POS
*    evarof();     Evaluerar ARC_OFFS
*    evarfl();     Evaluerar ARC_FIL
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

extern short  modtyp;

extern V2REFVA *geop_id;    /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv;    /* ingeop.c *pv      Access structure for MBS routines */
extern short    geop_pc;    /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np;    /* ingeop.c *npblock Pekare till namnparameterblock.*/

/*!******************************************************/

       short evarfr()

/*      Evaluerar geometri-proceduren ARC_1POS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera ARC_1POS.
*/
   return(EXarfr( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                                        geop_pv[2].par_va.lit.float_va,
                                        geop_pv[3].par_va.lit.float_va,
                                        geop_pv[4].par_va.lit.float_va,
                                        geop_np));
}
/********************************************************/
/*!******************************************************/

       short evar2p()

/*      Evaluerar geometri-proceduren ARC_2POS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera ARC_2POS.
*/
   return(EXar2p( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                                        geop_pv[3].par_va.lit.float_va,
                                        geop_np));
}

/********************************************************/
/*!******************************************************/

       short evar3p()

/*      Evaluerar geometri-proceduren ARC_3POS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera ARC_3POS.
*/
   return(EXar3p( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[3].par_va.lit.vec_va,
                                        geop_np));
	   } 

/********************************************************/
/*!******************************************************/

        short evarof()

/*      Evaluerar geometri-proceduren ARC_OFFS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera ARC_OFS.
*/
   return(EXarof( geop_id, geop_pv[1].par_va.lit.ref_va,
                           geop_pv[2].par_va.lit.float_va,
                           geop_np));
}

/********************************************************/
/*!******************************************************/

        short evarfl()

/*      Evaluerar geometri-proceduren ARC_FIL.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera ARC_FIL.
*/
   return(EXarfl( geop_id, geop_pv[1].par_va.lit.ref_va,
                           geop_pv[2].par_va.lit.ref_va,
                           geop_pv[3].par_va.lit.float_va,
                    (short)geop_pv[4].par_va.lit.int_va,
                           geop_np));
}

/********************************************************/
/********************************************************/
