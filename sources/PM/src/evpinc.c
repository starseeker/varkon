/**********************************************************************
*
*    evpinc.c
*    ========
*
*    This file is part of the VARKON PM Library.
*
*    This file includes the following routines:
*
*    evmsar();     Evaluates function POS_IN_CONE()
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
*    (C) Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

extern pm_ptr   stvecp;

/*!******************************************************/

        short evpinc()

/*      Evaluates geometric function POS_IN_CONE().
 *
 *      In: extern *geop_id   => Mesh ID.
 *          extern *geop_pv   => Ptr to parameter list.
 *          extern *geop_np   => Ptr to attributes.
 *
 *      Returncodes: IN5662 = Illegal parameter value
 *                   IN5672 = Error in parameter declaration
 *                   IN5682 = Can't malloc
 *
 *      (C)2005-08-04 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   short     status;
   int       npos,posadr,dekl_dim,vecsiz,pstadr,intsiz,i,index[1];
   char      errbuf[V3STRLEN];
   bool      any;
   STTYTBL   typtbl;
   STARRTY   arrtyp;
   PMLITVA   val;
   DBint    *pst;
   DBVector *pos;

/*
***Initialization.
*/
   status = 0;
   pst = NULL;
/*
***How many positions ?
*/
   npos = func_pv[5].par_va.lit.int_va;
/*
***Check the value.
*/
   if ( npos < 1 ) return(erpush("IN5712",""));
/*
***Check declared size of "pos" parameter
*/
   posadr = func_pv[6].par_va.lit.adr_va;
   strtyp(func_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < npos ) return(erpush("IN5722","positions"));

   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Allocate memory for npos vector values.
*/
   if ( (pos=(DBVector *)v3mall(npos*sizeof(DBVector),"evpinc")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5732",errbuf));
     }
/*
***Copy pos array from RTS to allocated area.
*/
   for ( i=0; i<npos; ++i )
     {
     ingval(posadr+i*vecsiz,stvecp,FALSE,&val);
     V3MOME(&val.lit.vec_va,pos+i,sizeof(DBVector));
     }
/*
***If status of individual positions is requested, check/allocate pstat as well.
*/
   if ( func_pc == 7 )
     {
     pstadr = func_pv[7].par_va.lit.adr_va;
     strtyp(func_pv[7].par_ty,&typtbl);
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim < npos ) return(erpush("IN5722","positions"));

     strtyp(arrtyp.base_arr,&typtbl);
     intsiz = typtbl.size_ty;

     if ( (pst=(DBint *)v3mall(npos*sizeof(DBint),"evpinc")) == NULL )
       {
       sprintf(errbuf,"%d",npos);
       return(erpush("IN5732",errbuf));
       }
     }
/*
***Execute. pst = NULL => only output is "any".
*/
   status = EXpinc((DBVector *)&func_pv[1].par_va.lit.vec_va,
                   (DBVector *)&func_pv[2].par_va.lit.vec_va,
                                func_pv[3].par_va.lit.float_va,
                                func_pv[4].par_va.lit.float_va,
                                func_pv[5].par_va.lit.int_va,
                                pos,pst,&any);

   if   ( any ) func_vp->lit.int_va = 1;
   else         func_vp->lit.int_va = 0;
/*
***Deallocate memory for pos data.
*/
   v3free(pos,"evpinc");
/*
***If status of individual positions is requested, copy result to MBS array.
*/
   if ( func_pc == 7 )
     {
     for ( i=0; i<npos; ++i )
       {
       index[0] = i+1;
       val.lit.int_va = *(pst+i);
       inwvar(func_pv[7].par_ty,func_pv[7].par_va.lit.adr_va,1,index,&val);
       }
     v3free(pst,"evpinc");
     }
/*
***Exit.
*/

   return(status);
 }

/********************************************************/
