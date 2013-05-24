/*!******************************************************************/
/*  File: ge120.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE120() Computes the arc length of a segment                    */
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

#define DIVIS 3      /* The necessary number of divisions of */
                     /* the interval to get the numeric      */
                     /* accuracy                             */

/********************************************************************/

      DBstatus GE120(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBfloat  interv[],
      DBfloat *pdelta)

/*    Computes the arc length of a segment.
 *
 *      In: pstr   = Pointer to arc/curve
 *          pseg   = Pointer to the segment
 *          interv = Start and end parametric values
 *
 *      Out: *pdelta => The length
 *
 *      (C)microform ab 1985-05-26 G.Liden
 *
 *      1999-04-20 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

 {
   DBfloat u;              /* Parameter value u                   */
   DBfloat dxdu,dydu,dzdu; /* Derivatives for u                   */
   DBfloat deltau;         /* The delta parameter step for the    */
                           /* numerical integration               */
   DBfloat f[5];           /* Function values for the numeric     */
                           /* integration (Simpson)               */
   DBfloat s1,s2;          /* Dummy variables for Simpson         */
   DBfloat sum;            /* The output from Simpson             */
   DBfloat delarc;         /* The output arclength                */
   DBint   i;              /* Loop index : divisions              */
   DBint   j;              /* Loop index : five function values   */
   DBfloat duabs;          /* Absolutbeloppet av deltau           */
   DBfloat du4;            /* 4*deltau                            */
   DBfloat max_leng;       /* Maximum tangent length              */
   DBfloat min_leng;       /* Minimum tangent length              */
   DBint   c_divis;        /* Current DIVIS                       */
   EVALC   evldat;         /* For GE110()                         */
   DBfloat k;              /* Temporary                           */

/*
***Divisions.
*/
   c_divis = DIVIS;

rstart: /* Restart if difference in tangent lengths too big */

/*
***The delta parameter value.
*/
   deltau = (interv[1] - interv[0])/4.0/(DBfloat)c_divis;
   du4    = deltau*4.0;
   duabs  = ABS(deltau);
/*
***Start arclength = 0.
*/
   delarc = 0.0;
/*
***Loop each division of the segment.
*/
   evldat.evltyp = EVC_DR;

   for ( i=1; i <= c_divis; i++ )
     {
     u = interv[0] + (i-1)*du4;
     max_leng= 0.00001;   /* Maximum tangent length */
     min_leng= 50000.0;   /* Minimum tangent length */
/*
***Loop 5 function values in the segment.
*/
     for ( j=0; j<5 ;  j++ )
       {
       evldat.t_local = u;
       if ( GE110(pstr,pseg,&evldat) != 0 ) return(erpush("GE1213","GE120"));

       dxdu = evldat.drdt.x_gm;
       dydu = evldat.drdt.y_gm;
       dzdu = evldat.drdt.z_gm;

       f[j] = SQRT(dxdu*dxdu + dydu*dydu + dzdu*dzdu);
       if ( f[j] > max_leng ) max_leng = f[j];
       if ( f[j] < min_leng ) min_leng = f[j];

       u += deltau;
       }
/*
***End inner loop.
*/
     if ( min_leng/max_leng < 0.1  &&  c_divis == DIVIS )
       {
       c_divis = 5*DIVIS;
       goto rstart;
       }

     k  = 0.666666666666667*duabs;
     s1 = k*(f[0] + 4.0*f[2] + f[4]);
     s2 = 0.5*k*(f[0] + 4.0*f[1] + 2.0*f[2] + 4.0*f[3] + f[4]);
     sum = (s2+(s2-s1)/15.0);

     delarc += sum;
     }
/*
***End outer loop.
*/
  *pdelta = delarc;

   return(0);
 }

/********************************************************************/
