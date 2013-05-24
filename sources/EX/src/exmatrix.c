/*!*******************************************************
*
*    exmatrix.c
*    ==========
*
*    EXeigasm();   Create a tf matrix by array of points
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

/*
* SUBROUTINE DGEEV( JOBVL, JOBVR, N, A, LDA, WR, WI, VL, LDVL, VR,
* $ LDVR, WORK, LWORK, INFO )
*/

void dgeev_(char *jobvl, char *jobvr, int *n, double *a, int *lda,
double *wr, double *wi,
double *vl, int *ldvl, double *vr, int *ldvr,
double *work, int *lwork, int *info);


/*!******************************************************/

       short EXeigasym(
       DBint       n,
       DBfloat     m[],
       DBfloat    eval[],
       DBfloat    evec[])

/*      Create a transformation matrix by array of points.
 *
 *      In: n      => Order of given matrix.
 *          m      => nxn real non-symmetric matrix.
 *
 *      Out: eval  => Real eigen values array.
 *           evec  => Real eigen vectors array, one after one.
 *
 *
 *     (C) Örebo university 15/04/2009 Mohamed Rahayem
 *
 ******************************************************!*/

  {
    short    status;
    DBint    i;
    char jobvl = 'N';
    char jobvr = 'V';
    DBfloat wr[n];
    DBfloat wi[n];
    DBfloat vl[n*3];
    DBint   ldvl = n;
    DBfloat vr[n*3];
    DBint   ldvr = n;
    DBint   lwork = (2+n)*n;
    DBfloat work[lwork];
    DBint   info;
/*
*** Call Lapack function.
*/
    dgeev_(&jobvl, &jobvr, &n, m, &n, wr, wi, vl, &ldvl, vr, &ldvr, work, &lwork, &info);
/*
*** Check if one or more of the eigen values is an complex.
*/
   for(i = 0; i < n; i++)
     {
      if (wi[i] != 0.0)
       {
       status = -2;
       return(status);
       }
     }
/*
*** Store eigen values and eigen vectors in the returned arraya.
*/
   if (info == 0)
     {
     status = info;
     for (i = 0; i < n; i++)
       {
        eval[i] = wr[i];
       }
     for (i = 0; i < 3*n; i++)
       {
        evec[i] = vr[i];
       }
     }
   else
     {
      status = info;
     }
/*
*** Return status.
*/
    return(status);
  }

/********************************************************/