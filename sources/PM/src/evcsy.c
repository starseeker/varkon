/**********************************************************************
*
*    evcsy.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evcs3p();     Evaluate CSYS_3P
*    evcs1p();     Evaluate CSYS_1P
*    evcsud();     Evaluate Csys_usrdef
*    evpcatm();    Evalute PCA() function.
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

extern short  modtyp;

extern V2REFVA *geop_id; /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv; /* ingeop.c *pv Access structure for MBS routines */
extern short    geop_pc; /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np; /* ingeop.c *npblock Pekare till namnparameterblock.*/

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pointer to result value. */

/*!******************************************************/

        short    evcs3p()

/*      Evaluerar geometri-proceduren CSYS_3P.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      1996-05-28 Optionell Y-axel, J.Kjellander
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBVector *pyaxel;

/*
***Har vi fått en Y-axel eller har den utelämnats.
*/
   if ( geop_pc ==  4 ) pyaxel = (DBVector *)&geop_pv[4].par_va.lit.vec_va;
   else            pyaxel =  NULL;
/*
***Exekvera CSYS_3P.
*/
   return(EXcs3p(  geop_id,   geop_pv[1].par_va.lit.str_va,
                 (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                 (DBVector *)&geop_pv[3].par_va.lit.vec_va,
                              pyaxel,
                              geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcs1p()

/*      Evaluerar geometri-proceduren CSYS_1P.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 30/9/87 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBfloat v1,v2,v3;

/*
***Exekvera CSYS_1P.
*/
   if ( modtyp == 3 )
     {
     if ( geop_pc == 2 )
       {
       v1 = v2 = v3 = 0.0;
       }
     else if ( geop_pc == 3 )
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = v3 = 0.0;
       }
     else if ( geop_pc == 4 )
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = geop_pv[4].par_va.lit.float_va;
       v3 = 0.0;
       }
     else
       {
       v1 = geop_pv[3].par_va.lit.float_va;
       v2 = geop_pv[4].par_va.lit.float_va;
       v3 = geop_pv[5].par_va.lit.float_va;
       }
     }
/*
***2D-fallet.
*/
   else
     {
     if ( geop_pc < 5 )
       {
       v1 = v2 = v3 = 0.0;
       }
     else
       {
       v3 = geop_pv[5].par_va.lit.float_va;
       v1 = v2 = 0.0;
       }
     }
/*
***Exekvera.
*/
   return(EXcs1p(   geop_id, geop_pv[1].par_va.lit.str_va,
                (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                             v1,v2,v3, geop_np));
}

/********************************************************/
/*!******************************************************/

        short    evcsud()

/*      Evaluates CSYS_USRDEF().
 *
 *      In: extern *geop_id   => Entity ID.
 *          extern *geop_pv   => Parameter values.
 *          extern  geop_pc   => Number of actual parameters.
 *          extern *geop_np   => Ptr to named parameter block.
 *
 *      Return: Returns status of called routines.    
 *
 *      (C)Örebro university 26/5/2008  M. Rahayem
 *
 *******************************************************!*/

{
   short   status;
   DBint   valadr;
   int     radsiz,fltsiz;
   PMLITVA fval;
   STTYTBL typtbl;
   STARRTY arrtyp;
   DBTmat  tr;
   
/*
***Calculate RTS-offsets.
***radsiz = size of a FLOAT(4)       Normally 32 bytes.
***fltsiz = size of a FLOAT          Normally  8 bytes.
*/
   strtyp(geop_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Copy 4x4-matrix from MBS variable to C struct DBTmat.
*/
   valadr = geop_pv[2].par_va.lit.adr_va;

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
***Execute.
*/
   status = EXcsud(geop_id,
                   geop_pv[1].par_va.lit.str_va,
                   &tr,
                   geop_np);     
/*
***The end.
*/
   return(status);
}

/********************************************************/
/*!******************************************************/

        short    evpcatm()

/*      Evalute PCA() function.
 *
 *      In:  Global param. func_pv[] => Parameter value array
 *      Out: Global param. *func_vp  => C-pointer to result value
 * 
 *      Return:  Return the status of called routines
 *
 *
 *     (C) Örebro university 26/5/2008  M. Rahayem
 *
 ******************************************************!*/

{
   char      errbuf[V3STRLEN];  /* Error message buffer */
   DBint     npos;              /* Number of points */ 
   DBint     posadr;            /* Index to array */ 
   DBint     dekl_dim;          /* Declared dimension */ 
   DBVector *ppek;              /* Allocated area for the points */
   DBint     vecsiz;            /* Size of VECTOR */ 
   DBTmat    tr;                /* The created 4x4 transformation matrix */
   DBfloat  *fltptr;            /* Ptr to matrix element */
   int       i,j;               /* Loop counters */
   int       index[2];          /* Matrix element index */
   short     status;            /* Status from execute function */
   STTYTBL   typtbl;            /* Parameter value type info. */ 
   PMLITVA   val;               /* Value */
   STARRTY   arrtyp;            /* Array type */ 

/*
***Number of points.
*/
   npos = func_pv[1].par_va.lit.int_va;
/*
***Check the number of points.
*/
   if ( npos < 4 )
     {
      func_vp->lit.int_va = -1;
      return(0);
     }
/*
***Check that declared dimension of the MBS-array is big enough.
*/
   posadr = func_pv[2].par_va.lit.adr_va;
   strtyp(func_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < npos ) return(erpush("IN5892",""));   
/*
***Allocate memory for the points.
*/
   if ( (ppek=(DBVector *)v3mall(npos*sizeof(DBVector),"evpcatm")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5902",errbuf));
     }   
/*
***vecsiz = size of a VECTOR.
*/
   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Copy points from RTS to allocated area.
*/
   for ( i=0; i<npos; ++i )
     {
     ingval(posadr+i*vecsiz,arrtyp.base_arr,FALSE,&val);
     (ppek+i)->x_gm = val.lit.vec_va.x_val;
     (ppek+i)->y_gm = val.lit.vec_va.y_val;
     (ppek+i)->z_gm = val.lit.vec_va.z_val;
     }
/*
***Execute PCA function.
*/
   status=EXpcatm(ppek,npos,&tr);
/*
***Deallocate memory
*/
   v3free(ppek,"evpcatm");
/*
***Return value equal to status.
*/
   func_vp->lit.int_va = status;   
/*
***Write result (tr) to MBS variable.
*/
   fltptr = (DBfloat *)&tr; 

   for ( i=0; i<4; ++i )
     {
     index[0] = i+1;
     for ( j=0; j<4; ++j )
       {
       index[1] = j+1;
       val.lit.float_va = *fltptr;
       status = inwvar(func_pv[3].par_ty, 
                       func_pv[3].par_va.lit.adr_va,
                       2,index,&val);
     ++fltptr;
       }
     }
/*
***Clear the error message stack from any errors created
***by GEmktf_pca() by Gunnar Liden.
*/
   erinit();
/*
***The end.
*/
   return(0);
}

/********************************************************/
