/**********************************************************************
*
*    evtform.c
*    =========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*
*    evtrmv();      Evaluerar TFORM_MOVE
*    evtrrp();      Evaluerar TFORM_ROTP
*    evtrrl();      Evaluerar TFORM_ROTL
*    evtrmi();      Evaluerar TFORM_MIRR
*    evtrco();      Evaluerar TFORM_COMP
*    evtrus();      Evaluerar TFORM_USDEF
*    evtcop();      Evaluerar TCOPY
*    evtmult();     Evaluerar TFORM_MULT
*    evtinv();      Evaluerar TFORM_INV
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
*    2006-07-12 added evtmult() and evtinv(), Sören Larsson
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern V2REFVA *geop_id;  /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv;  /* ingeop.c *pv      Access structure for MBS routines */
extern short    geop_pc;  /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np;  /* ingeop.c *npblock Pekare till namnparameterblock.*/

extern PMPARVA *proc_pv;  /*          Access structure for MBS routines */


/*!******************************************************/

        short evtrmv()

/*      Evaluerar geometri-proceduren TFORM_MOVE.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern  geop_pc  => Antal parametrar.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    DBfloat sx,sy,sz;

/*
***Har skalning begärts ?
*/
    if      ( geop_pc == 2 ) sx = sy = sz = 1.0;
    else if ( geop_pc == 3 ) sx = sy = sz = geop_pv[3].par_va.lit.float_va;
    else if ( geop_pc == 4 )
      {
      sx = geop_pv[3].par_va.lit.float_va;
      sy = sz = geop_pv[4].par_va.lit.float_va;
      }
    else if ( geop_pc == 5 )
      {
      sx = geop_pv[3].par_va.lit.float_va;
      sy = geop_pv[4].par_va.lit.float_va;
      sz = geop_pv[5].par_va.lit.float_va;
      }
/*
***Exekvera.
*/
    return(EXtrmv(geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                                        sx,sy,sz,geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evtrrp()

/*      Evaluerar geometri-proceduren TFORM_ROTP.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern  geop_pc  => Antal parametrar.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    DBfloat sx,sy,sz;
    DBVector p2;

/*
***Positionen P2 på rotationsaxeln = P1 + (0,0,1).
*/
    p2.x_gm = geop_pv[1].par_va.lit.vec_va.x_val;
    p2.y_gm = geop_pv[1].par_va.lit.vec_va.y_val;
    p2.z_gm = geop_pv[1].par_va.lit.vec_va.z_val + 1.0;
/*
***Har skalning begärts ?
*/
    if      ( geop_pc == 2 ) sx = sy = sz = 1.0;
    else if ( geop_pc == 3 ) sx = sy = sz = geop_pv[3].par_va.lit.float_va;
    else if ( geop_pc == 4 )
      {
      sx = geop_pv[3].par_va.lit.float_va;
      sy = sz = geop_pv[4].par_va.lit.float_va;
      }
    else if ( geop_pc == 5 )
      {
      sx = geop_pv[3].par_va.lit.float_va;
      sy = geop_pv[4].par_va.lit.float_va;
      sz = geop_pv[5].par_va.lit.float_va;
      }
/*
***Exekvera.
*/
    return(EXtrro(geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                                       &p2,
                                        geop_pv[2].par_va.lit.float_va,
                                        sx,sy,sz,
                                        geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evtrrl()

/*      Evaluerar geometri-proceduren TFORM_ROTL.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern  geop_pc  => Antal parametrar.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    DBfloat sx,sy,sz;

/*
***Har skalning begärts ?
*/
    if      ( geop_pc == 3 ) sx = sy = sz = 1.0;
    else if ( geop_pc == 4 ) sx = sy = sz = geop_pv[4].par_va.lit.float_va;
    else if ( geop_pc == 5 )
      {
      sx = geop_pv[4].par_va.lit.float_va;
      sy = sz = geop_pv[5].par_va.lit.float_va;
      }
    else if ( geop_pc == 6 )
      {
      sx = geop_pv[4].par_va.lit.float_va;
      sy = geop_pv[5].par_va.lit.float_va;
      sz = geop_pv[6].par_va.lit.float_va;
      }
/*
***Exekvera.
*/
    return(EXtrro(geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                                        geop_pv[3].par_va.lit.float_va,
                                        sx,sy,sz,
                                        geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evtrmi()

/*      Evaluerar geometri-proceduren TFORM_MIRR.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern  geop_pc  => Antal parametrar.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    DBVector p3;

/*
***Om bara 2 punkter getts skapar vi en tredje i Z-
***axelns riktning.
*/
    if ( geop_pc == 2 )
      {
      p3.x_gm = geop_pv[1].par_va.lit.vec_va.x_val;
      p3.y_gm = geop_pv[1].par_va.lit.vec_va.y_val;
      p3.z_gm = geop_pv[1].par_va.lit.vec_va.z_val + 1.0;
      }
    else
      {
      p3.x_gm = geop_pv[3].par_va.lit.vec_va.x_val;
      p3.y_gm = geop_pv[3].par_va.lit.vec_va.y_val;
      p3.z_gm = geop_pv[3].par_va.lit.vec_va.z_val;
      }
/*
***Exekvera.
*/
    return(EXtrmi(geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                                       &p3,
                                        geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evtrco()

/*      Evaluerar geometri-proceduren TFORM_COMP.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 24/2/93 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    return(EXtrco(geop_id, (DBVector *)&geop_pv[1].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[3].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[4].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[5].par_va.lit.vec_va,
                           (DBVector *)&geop_pv[6].par_va.lit.vec_va,
                                        geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evtrus()

/*      Evaluerar geometri-proceduren TFORM_USDEF.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
   DBint   valadr;
   int     radsiz,fltsiz;
   PMLITVA fval;
   STTYTBL typtbl;
   STARRTY arrtyp;
   DBTmat  tr;

/*
***Beräkna div. RTS-offset.
***radsiz = storleken på en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken på en FLOAT           Normalt 8   bytes.
*/
   strtyp(geop_pv[1].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matrisen till DBTmat.
*/
   valadr = geop_pv[1].par_va.lit.adr_va;

   ingval(valadr,arrtyp.base_arr,FALSE,&fval);
   tr.g11 = fval.lit.float_va;
   ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g12 = fval.lit.float_va;
   ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g13 = fval.lit.float_va;
   ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g14 = fval.lit.float_va;

   ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g21 = fval.lit.float_va;
   ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g22 = fval.lit.float_va;
   ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g23 = fval.lit.float_va;
   ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g24 = fval.lit.float_va;

   ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g31 = fval.lit.float_va;
   ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g32 = fval.lit.float_va;
   ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g33 = fval.lit.float_va;
   ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g34 = fval.lit.float_va;

   ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g41 = fval.lit.float_va;
   ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g42 = fval.lit.float_va;
   ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g43 = fval.lit.float_va;
   ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr.g44 = fval.lit.float_va;
/*
***Exekvera TFORM_USDEF.
*/
   return(EXtrus(geop_id,&tr,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evtcop()

/*      Evaluerar geometri-proceduren TCOPY.
 *
 *      In: extern *geop_id  => Storhetens ID.
 *          extern *geop_pv  => Pekare till array med parametervärden.
 *          extern *geop_np  => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
    return(EXtcop(geop_id, &geop_pv[1].par_va.lit.ref_va[0],
                           &geop_pv[2].par_va.lit.ref_va[0],
                     (short)geop_pv[3].par_va.lit.int_va,
                            geop_np));
 }

/********************************************************/




/*!******************************************************/

        short evtmult()

/*      Evaluates the procedure TFORM_MULT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C) 2006-07-12 Sören Larsson, Örebro University 
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,j,index[2];
     PMLITVA litval;
     DBfloat  *matpek;

     DBTmat  tr1;
     DBTmat  tr2;
     DBTmat  tr3;
     
     int     radsiz,fltsiz;
     PMLITVA fval;
     STTYTBL typtbl;
     STARRTY arrtyp;
     DBint   valadr;
     
/*
***1:a parametern
***Beräkna div. RTS-offset.
***radsiz = storleken på en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken på en FLOAT           Normalt 8   bytes.
*/
   strtyp(proc_pv[1].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matrisen till DBTmat.
*/
   valadr = proc_pv[1].par_va.lit.adr_va;

   ingval(valadr,arrtyp.base_arr,FALSE,&fval);
   tr1.g11 = fval.lit.float_va;
   ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g12 = fval.lit.float_va;
   ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g13 = fval.lit.float_va;
   ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g14 = fval.lit.float_va;

   ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g21 = fval.lit.float_va;
   ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g22 = fval.lit.float_va;
   ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g23 = fval.lit.float_va;
   ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g24 = fval.lit.float_va;

   ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g31 = fval.lit.float_va;
   ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g32 = fval.lit.float_va;
   ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g33 = fval.lit.float_va;
   ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g34 = fval.lit.float_va;

   ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g41 = fval.lit.float_va;
   ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g42 = fval.lit.float_va;
   ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g43 = fval.lit.float_va;
   ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g44 = fval.lit.float_va;     
     
/*     
*** 2:a parametern
***Beräkna div. RTS-offset.
***radsiz = storleken på en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken på en FLOAT           Normalt 8   bytes.
*/
   strtyp(proc_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matrisen till DBTmat.
*/
   valadr = proc_pv[2].par_va.lit.adr_va;

   ingval(valadr,arrtyp.base_arr,FALSE,&fval);
   tr2.g11 = fval.lit.float_va;
   ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g12 = fval.lit.float_va;
   ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g13 = fval.lit.float_va;
   ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g14 = fval.lit.float_va;

   ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g21 = fval.lit.float_va;
   ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g22 = fval.lit.float_va;
   ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g23 = fval.lit.float_va;
   ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g24 = fval.lit.float_va;

   ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g31 = fval.lit.float_va;
   ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g32 = fval.lit.float_va;
   ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g33 = fval.lit.float_va;
   ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g34 = fval.lit.float_va;

   ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g41 = fval.lit.float_va;
   ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g42 = fval.lit.float_va;
   ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g43 = fval.lit.float_va;
   ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr2.g44 = fval.lit.float_va; 
  
   
   status = GEtform_mult(&tr1,&tr2,&tr3);

/*
***Returnera .
*/
     matpek = &tr3; 

     for ( i=0; i<4; ++i )
        {
        index[0] = i+1;
        for ( j=0; j<4; ++j )
           {
           index[1] = j+1;
           litval.lit.float_va = *matpek;
           status = inwvar(proc_pv[3].par_ty, 
                     proc_pv[3].par_va.lit.adr_va,
                     2, index, &litval);
           ++matpek;
           }
        }
/*
***Slut.
*/
     return(status);
  }
/********************************************************/


/*!******************************************************/

        short evtinv()

/*      Evaluates the procedure TFORM_INV.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C) 2006-07-12 Sören Larsson, Örebro University 
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,j,index[2];
     PMLITVA litval;
     DBfloat  *matpek;

     DBTmat  tr1;
     DBTmat  tr2;
     
     int     radsiz,fltsiz;
     PMLITVA fval;
     STTYTBL typtbl;
     STARRTY arrtyp;
     DBint   valadr;
/*
***1:a parametern
***Beräkna div. RTS-offset.
***radsiz = storleken på en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken på en FLOAT           Normalt 8   bytes.
*/
   strtyp(proc_pv[1].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matrisen till DBTmat.
*/
   valadr = proc_pv[1].par_va.lit.adr_va;

   ingval(valadr,arrtyp.base_arr,FALSE,&fval);
   tr1.g11 = fval.lit.float_va;
   ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g12 = fval.lit.float_va;
   ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g13 = fval.lit.float_va;
   ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g14 = fval.lit.float_va;

   ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g21 = fval.lit.float_va;
   ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g22 = fval.lit.float_va;
   ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g23 = fval.lit.float_va;
   ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g24 = fval.lit.float_va;

   ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g31 = fval.lit.float_va;
   ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g32 = fval.lit.float_va;
   ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g33 = fval.lit.float_va;
   ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g34 = fval.lit.float_va;

   ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g41 = fval.lit.float_va;
   ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g42 = fval.lit.float_va;
   ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g43 = fval.lit.float_va;
   ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
   tr1.g44 = fval.lit.float_va;     
   
   status = GEtform_inv(&tr1,&tr2);

/*
***Returnera .
*/
     matpek = &tr2; 

     for ( i=0; i<4; ++i )
        {
        index[0] = i+1;
        for ( j=0; j<4; ++j )
           {
           index[1] = j+1;
           litval.lit.float_va = *matpek;
           status = inwvar(proc_pv[2].par_ty, 
                     proc_pv[2].par_va.lit.adr_va,
                     2, index, &litval);
           ++matpek;
           }
        }
/*
***Slut.
*/
     return(status);
  }

