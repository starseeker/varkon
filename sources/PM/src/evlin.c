/**********************************************************************
*
*    evlin.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evlifr();     Evaluerar LIN_FREE
*    evlipr();     Evaluerar LIN_PROJ
*    evliof();     Evaluerar LIN_OFFS
*    evlian();     Evaluerar LIN_ANG
*    evlit1();     Evaluerar LIN_TAN1
*    evlit2();     Evaluerar LIN_TAN2
*    evlipe();     Evaluerar LIN_PERP
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

extern V2REFVA *geop_id; /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv; /* ingeop.c *pv Access structure for MBS routines */
extern short    geop_pc; /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np; /* ingeop.c *npblock Pekare till namnparameterblock.*/

/*!******************************************************/

        short evlifr()

/*      Evaluerar geometri-proceduren LIN_FREE.
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
***Exekvera LIN_FREE.
*/
   return(EXlifr( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va, geop_np));

}

/********************************************************/
/*!******************************************************/

        short evlipr()

/*      Evaluerar geometri-proceduren LIN_PROJ.
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
***Exekvera LIN_PROJ.
*/
   return(EXlipr( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va, geop_np));

}

/********************************************************/
/*!******************************************************/

        short evliof()

/*      Evaluerar geometri-proceduren LIN_OFFS.
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
***Exekvera LIN_OFFS.
*/
   return(EXliof( geop_id, geop_pv[1].par_va.lit.ref_va,
                           geop_pv[2].par_va.lit.float_va, geop_np));

}

/********************************************************/
/*!******************************************************/

        short evlian()

/*      Evaluerar geometri-proceduren LIN_ANG.
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
***Exekvera LIN_ANG.
*/
   return(EXlipv( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                                        geop_pv[2].par_va.lit.float_va,
                                        geop_pv[3].par_va.lit.float_va, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evlit1()

/*      Evaluerar geometri-proceduren LIN_TAN1.
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
***Exekvera LIN_TAN1.
*/
   return(EXlipt( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                                        geop_pv[2].par_va.lit.ref_va,
                                 (short)geop_pv[3].par_va.lit.int_va,
								        geop_np));
}

/********************************************************/
/*!******************************************************/

       short evlit2()

/*      Evaluerar geometri-proceduren LIN_TAN2.
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
***Exekvera LIN_TAN2.
*/
   return(EXli2t( geop_id, geop_pv[1].par_va.lit.ref_va,
                           geop_pv[2].par_va.lit.ref_va,
                    (short)geop_pv[3].par_va.lit.int_va, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evlipe()

/*      Evaluerar geometri-proceduren LIN_PERP.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 24/4/87  R. Svedin
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera LIN_PERP.
*/
   return(EXlipe( geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                                        geop_pv[2].par_va.lit.ref_va,
                                        geop_pv[3].par_va.lit.float_va,
										geop_np));
}

/********************************************************/
/********************************************************/
