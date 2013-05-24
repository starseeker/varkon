/*!******************************************************************/
/*  File: gesort.c                                                  */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GEsort_1()    Sort array and remove dupletts                    */
/*  GEsort_2()    Sort array-1 with respect to array-2              */
/*  GEsort_mesh() Sort faces with respect to max X, Y or Z          */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*!******************************************************************/

        DBstatus GEsort_1(
        DBfloat *uin,
        short    nin,
        short   *nut,
        DBfloat *uut)

/*      Sort array of float numbers and remove values
 *      equal within +/- TOL4.
 *
 *      In:   uin  = Unsorted values
 *            nin  = Number of unsorted values
 *
 *      Out: *nut  = Number of sorted values
 *           *uut  = Sorted values
 *
 *      (C)microform ab 8/3/95 J. Kjellander
 *
 *      1996-06-03 Bug vid > 2 lika lösn, J.Kjellander
 *      1999-05-18 Rewritten, J.Kjellander
 *
 ******************************************************************!*/

 {
   short   i,start,end,imin,j;
   bool    shift;
   DBfloat min,tmp,diff;

/*
***If uin <> uut copy uin to uut.
*/
   if ( uut != uin ) for ( i=0; i<nin; ++i ) *(uut+i) = *(uin+i);

  *nut = nin;
/*
***Sen sorterar vi. Bubble-sort width decreasing depth.
*/
   start = 0;
   end   = nin - 1;

loop:
   shift =  FALSE;
   min   = *(uut+start);
   imin  =  start;

   for ( i=start; i<end; ++i )
     {
     if ( *(uut+i+1) < *(uut+i) )
       {
       shift = TRUE;
       tmp = *(uut+i); *(uut+i) = *(uut+i+1); *(uut+i+1) = tmp;
       if ( *(uut+i) < min )
         {
         min = *(uut+i);
         imin = i;
         }
       }
     }

   if ( shift )
     {
     tmp = *(uut+imin); *(uut+imin) = *(uut+start); *(uut+start) = tmp;
     start++;
     end--;
     goto loop;
     }
/*
***Remove all dupletts.
*/
   end = nin;

   for ( i=0; i<end-1; ++i )
     {
     diff = *(uut+i) - *(uut+i+1);
     if ( ABS(diff) < TOL4 )
       {
       for ( j=i; j<end-1; ++j ) *(uut+j) = *(uut+j+1);
       end--;
       i--;   /* Bugfix, 1996-06-03 JK */
       }
     }

   *nut = (short)end;

   return(0);
 }

/********************************************************************/
/********************************************************************/

      DBstatus GEsort_2(
      DBfloat uin1[],
      DBfloat uin2[],
      short   noin,
      short  *pnout,
      DBfloat uout1[],
      DBfloat uout2[])

/*    Orders array-2 with respect to array-1 and removes 
 *    values equal within +/- TOL4.
 *
 *    In: uin1  = The first input array
 *        uin2  = The second input array
 *        noin  = The number of elements in uin1 (and uin2)
 *
 *    Out: *pnout = The number of output elements
 *         *uout1 = Ordered array 1
 *         *uout2 = Ordered array 2
 *
 *     (C)Microform AB,  Gunnar Liden 1985-02-03
 *
 *     1985-07-10 Error system  B. Doverud
 *     1999-05-18 Rewritten, J.Kjellander
 *
 ********************************************************************/

 {
   DBfloat cuin1[GMMXSG*INTMAX]; /* A copy of uin1 in order not to destroy */
                                 /* the input vector                       */
   DBfloat cuin2[GMMXSG*INTMAX]; /* A copy of uin2 in order not to destroy */
                                 /* the input vector                       */
   short    k,i ;                /* Loop index                             */
   short   nout;                 /* The number of output points (=*pnout)  */
   short   imin=0;               /* For the ordering of the elements       */

/*
***Copy uin1 to cuin1 and uin2 to cuin2 in order not to
***destroy the input data
*/
   for ( k=0; k<noin; k++ )
     {
     cuin1[k] = uin1[k];
     cuin2[k] = uin2[k];
     }
/*
***Start value for nout is noin
*/
   nout = noin;
/*
***Sort the elements in cuin1 to uout1 and let uin2 be sorted
***the same way into uout2
*/
   for ( k=1; k <= noin; k++ )
     { 
     uout1[k-1] = TOL6;
     if ( k > 1 ) cuin1[imin-1] = 1.1*TOL6;

     for ( i=1; i <= noin; i++ )
      {
/*
***Check if elements are equal
*/
      if ( ABS(cuin1[i-1] - uout1[k-1]) < TOL4 )
        {
      --nout;
        cuin1[i-1] = 1.1*TOL6;
        }
      if ( cuin1[i-1] <= uout1[k-1] )
        {
        uout1[k-1] = cuin1[i-1];
        uout2[k-1] = cuin2[i-1];
        imin = i;
        }
      }
    }
/*
***The end.
*/
  *pnout = nout;

   return(0);
 }

/********************************************************************/
/*!******************************************************************/

        DBstatus GEsort_mesh(
        DBMesh  *mshptr,
        char     mode,
        DBint   *facelist,
        DBfloat *min,
        DBfloat *max)

/*      Sort mesh faces with respect to max coordinate in X,Y or Z.
 *      Returns sorted array and unsorted min- and max-coordinates.
 *
 *      In:   mshptr = C-ptr to mesh
 *            mode   = X, Y or Z
 *
 *      Out: *facelist = List of sorted face indexes
 *           *min      = Min values
 *           *max      = Max values
 *
 *      (C)2004-07-31 J. Kjellander, Örebro university
 *
 ******************************************************************!*/

 {
   char      errbuf[V3STRLEN];
   bool      shift;
   DBint     i,start,end,imin,tmpi;
   DBHedge  *hedge_1,*hedge_2,*hedge_3;
   DBVector *p1,*p2,*p3;
   DBfloat  *v,v1,v2,v3,vmin,tmpf;

/*
***Init facelist.
*/
   for ( i=0; i<mshptr->nf_m; ++i ) *(facelist+i) = i + 1;
/*
***Allocate memory for nf coordinates.
*/
   if ( (v=(DBfloat *)v3mall(mshptr->nf_m*sizeof(DBfloat),"GEsort_mesh")) == NULL )
     {
     sprintf(errbuf,"%d",mshptr->nf_m);
     return(erpush("GE6303",errbuf));
     }
/*
***Build the array v of coordinates to sort. Find maximum coordinates
***in the right direction and save in v for sorting and max for unsorted
***output. Store min vaues in min for unsorted output.
*/
   for ( i=0; i<mshptr->nf_m; ++i )
     {
     hedge_1 = &(mshptr->ph_m[mshptr->pf_m[i].i_hedge]);
     hedge_2 = &(mshptr->ph_m[hedge_1->i_nhedge]);
     hedge_3 = &(mshptr->ph_m[hedge_2->i_nhedge]);

     p1      = &(mshptr->pv_m[hedge_1->i_evertex].p);
     p2      = &(mshptr->pv_m[hedge_2->i_evertex].p);
     p3      = &(mshptr->pv_m[hedge_3->i_evertex].p);

     switch (mode)
       {
       case 'X': v1 = p1->x_gm; v2 = p2->x_gm; v3 = p3->x_gm; break;
       case 'Y': v1 = p1->y_gm; v2 = p2->y_gm; v3 = p3->y_gm; break;
       case 'Z': v1 = p1->z_gm; v2 = p2->z_gm; v3 = p3->z_gm; break;
       }

     if ( v1 > v2 )
       {
       if ( v1 > v3 )
         {
        *(v+i) = *(max+i) = v1;
         if ( v2 > v3 ) *(min+i) = v3;
         else           *(min+i) = v2;
         }
       else
         {
        *(v+i) = *(max+i) = v3;
        *(min+i) = v2;
         }
       }
     else
       {
       if ( v2 > v3 )
         {
        *(v+i) = *(max+i) = v2;
         if ( v1 > v3 ) *(min+i) = v3;
         else           *(min+i) = v1;
         }
       else
         {
        *(v+i) = *(max+i) = v3;
        *(min+i) = v1;
         }
       }
     }
/*
***Bubble sort v with decreasing depth. When shifting, shift facelist too.
*/
   start = 0;
   end   = mshptr->nf_m - 1;

loop:
   shift =  FALSE;
   vmin   = *(v+start);
   imin  =  start;

   for ( i=start; i<end; ++i )
     {
     if ( *(v+i+1) < *(v+i) )
       {
       shift = TRUE;
       tmpf = *(v+i); *(v+i) = *(v+i+1); *(v+i+1) = tmpf;
       tmpi = *(facelist+i); *(facelist+i) = *(facelist+i+1); *(facelist+i+1) = tmpi;
       if ( *(v+i) < vmin )
         {
         vmin = *(v+i);
         imin = i;
         }
       }
     }

   if ( shift )
     {
     tmpf = *(v+imin); *(v+imin) = *(v+start); *(v+start) = tmpf;
     tmpi = *(facelist+imin); *(facelist+imin) = *(facelist+start); *(facelist+start) = tmpi;
     start++;
     end--;
     goto loop;
     }
/*
***Free memory.
*/
   v3free(v,"GEsort_mesh");

   return(0);
 }

/********************************************************************/
