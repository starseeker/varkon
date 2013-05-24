/**********************************************************************
*
*    evmatrix.c
*    ==========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*
*    eveigasym();     Evaluate EIGASYM()
*
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

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pointer to result value. */

/********************************************************/

        short    eveigasym()

/*      Evalute MBS function status:=EIGASYM(n,m,eigval,eigvec).
 *
 *      INT    status;     Function output status
 *      INT    n;          In: Matrix order (n rows, n columns)
 *      FLOAT  m(n*n);     In: Matrix coefficients 1-dim array
 *      FLOAT  eigval(n);  Out: Eigenvalues 1-dim array
 *      VECTOR eigvec(n);  Out: Eigenvectors 1-dim array
 *
 *      In:  Global param. func_pv[] => Parameter value array
 *      Out: Global param. *func_vp  => C-pointer to result value
 *
 *      Return:    0 => Ok
 *               < 0 => Error message pushed on error stack
 *
 *     (C)Örebro university 2009-04-16 J.Kjellander
 *
 ******************************************************!*/

{
   char      errbuf[V3STRLEN];  /* Error message buffer */
   short     status;            /* Return status from EX-function */
   int       i;                 /* Loop variable */
   int       index[1];          /* Index to MBS array */
   DBint     n;                 /* Order of matrix (n*n)*/
   DBint     madr;              /* RTS-index to MBS variable m */
   DBint     dekl_dim;          /* Declared dimension */
   DBfloat  *mc;                /* Allocated area for matrix coefficients */
   STTYTBL   typtbl;            /* Parameter value type info. */
   STARRTY   arrtyp;            /* Array type */
   DBint     fltsiz;            /* Size of a MBS FLOAT */
   DBint     vecsiz;            /* Size of a MBS VECTOR */
   PMLITVA   val;               /* MBS value */
   DBfloat  *eval;              /* Ptr to output eigenvalues */
   DBfloat  *evec;              /* Ptr to output eigenvectors */


/*
***Order of matrix.
*/
   n = func_pv[1].par_va.lit.int_va;
/*
***Check that the order of the given matrix is not too small.
*/
   if ( n < 2 ) return(erpush("IN5912",""));
/*
***Check that declared dimension of the output MBS vector eigval(n) is big enough.
*/
   strtyp(func_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < n ) return(erpush("IN5922",""));
/*
***Check that declared dimension of the output MBS vector eigvec(n) is big enough.
*/
   strtyp(func_pv[4].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < n ) return(erpush("IN5932",""));

   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Check that declared dimension of the input MBS array m(n*n) is at least n*n.
*/
   strtyp(func_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < n*n ) return(erpush("IN5942",""));

   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Index to MBS variable m(n*n) on RTS.
*/
   madr = func_pv[2].par_va.lit.adr_va;
/*
***Allocate memory for the matrix coefficients.
*/
   if ( (mc=(DBfloat *)v3mall(n*n*sizeof(DBfloat),"eveigasym")) == NULL )
     {
     sprintf(errbuf,"%d",n);
     return(erpush("IN5952",errbuf));
     }
/*
***Copy matrix coefficients from RTS to allocated area.
*/
   for ( i=0; i<n*n; ++i )
     {
     ingval(madr+i*fltsiz,arrtyp.base_arr,FALSE,&val);
    *(mc+i) = val.lit.float_va;
     }
/*
***Allocate memory for output eigenvalues.
*/
   if ( (eval=(DBfloat *)v3mall(n*sizeof(DBfloat),"eveigasym")) == NULL )
     {
     v3free(mc,"eveigasym");
     sprintf(errbuf,"%d",n);
     return(erpush("IN5952",errbuf));
     }
/*
***Allocate memory for output eigenvectors.
*/
   if ( (evec=(DBfloat *)v3mall(3*n*sizeof(DBfloat),"eveigasym")) == NULL )
     {
     v3free(mc,"eveigasym");
     v3free(eval,"eveigasym");
     sprintf(errbuf,"%d",n);
     return(erpush("IN5952",errbuf));
     }
/*
***Execute.
*/
#ifdef DV3ATLAS
   status = EXeigasym(n,mc,eval,evec);
#endif
/*
***Write eval and evec to MBS variables eigval and eigvec.
*/
   if ( status == 0 )
     {
     for ( i=0; i<n; ++i )
       {
       index[0] = i+1;
       val.lit.float_va = *(eval+i);
       inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,1,index,&val);
       }

     for ( i=0; i<n; ++i )
       {
       index[0] = i+1;
       V3MOME(evec+i*3,&val.lit.vec_va,vecsiz);
       inwvar(func_pv[4].par_ty,func_pv[4].par_va.lit.adr_va,1,index,&val);
       }
     }
/*
***Deallocate memory
*/
   v3free(mc,"eveigasym");
   v3free(eval,"eveigasym");
   v3free(evec,"eveigasym");
/*
***MBS function return value.
*/
   func_vp->lit.int_va = status;
/*
***The end.
*/
   return(0);
}

/********************************************************/
