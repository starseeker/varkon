/**********************************************************************
*
*    ge403.c
*    =======
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    GE403() Compute point of tangency on curve segment.
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
#include "../include/GE.h"

/*!******************************************************************/
/*                                                                  */
/*  Gunnar Liden 1985-07-23                                         */
/*                                                                  */
/*  1986-05-11 extern short, B. Doverud                             */
/*  1999-04-02 Rewritten, J.Kjellander                              */
/*                                                                  */
/********************************************************************/

      DBstatus GE403(
      GMUNON   *pstr,    /* Pointer to the structure (arc, curve comp)*/
      GMSEG    *pseg,    /* Pointer to the rational cubic segment     */
      DBVector *pextpt,  /* Pointer to the external point             */
      DBTmat   *pc,      /* Pointer to the coordinate system          */
      short    nstart,   /* The number of restarts                    */
      short   *pnoint,   /* Pointer to the number of intersects. Note */
                         /* that double points normally exist from    */
                         /* the restarts.                             */
      DBfloat uout[])    /* An array of u solutions (parametric       */
                         /* values) Note that the u values are not    */
                         /* sorted                                    */
{
#define JMAX  20         /* The maximum number of iterations    */

   DBVector pexloc;         /* The transformed external point      */
   DBSeg    ratloc;         /* The transformed rational segment    */
   DBfloat  u;              /* Parameter value u                   */
   DBfloat  f;              /* Function value                      */
   DBfloat  dfdu;           /* Derivative of f with respect to u   */
   DBfloat  deltau;         /* The u step for the restarts         */
   DBfloat  h;              /* The Newton Rhapson step             */
   short    i,j;            /* Loop index i=restarts j=Newton R    */
   short    numint;         /* The number of intersects (=*pnoint) */
   EVALC    evldat;         /* For GE110()                         */

/*
***Transform the point to the local system
*/
   if ( GEtfpos_to_local(pextpt,pc,&pexloc) < 0 )
     return(erpush("GE4053","GE403(GEtfpos_to_local)"));
/*
***Transform the rational segment to the local system
*/
   if ( GEtfseg_to_local(pseg,pc,&ratloc) < 0 )
     return(erpush("GE4053","GE403(GEtfseg_to_local)"));
/*
***The step for the restarts
***Min should be 2 and note that last u will be < 1.0 )
*/
   deltau = (1.0 - TOL4)/((DBfloat)nstart - 1.0);
/*
***Start numint = 0
*/
   numint = 0;

   for ( i=1; i <= nstart; ++i )
     {
     u = deltau*(i-1);
     for( j=1; j <= JMAX; ++j )
       {
/*
***Compute function f and dfdu
*/
       evldat.evltyp = EVC_R + EVC_DR + EVC_D2R;
       evldat.t_local = u;

       if ( GE110(pstr,&ratloc,&evldat) != 0 )
         return(erpush("GE1003","GE403"));

       f = (evldat.r.x_gm - pexloc.x_gm)*evldat.drdt.y_gm -
           (evldat.r.y_gm - pexloc.y_gm)*evldat.drdt.x_gm;

       dfdu = evldat.drdt.x_gm*evldat.drdt.y_gm +
             (evldat.r.x_gm - pexloc.x_gm)*evldat.d2rdt2.y_gm -
              evldat.drdt.x_gm*evldat.drdt.y_gm -
             (evldat.r.y_gm - pexloc.y_gm)*evldat.d2rdt2.x_gm;
/*
***Intersect point found ?
*/
       if ( ABS(f) <= TOL2 ) 
         {
/*
***Intersect point is found
***Increase the number of intersects, also if it is
***the same point, and add u point to uout vector
***Solution only if the u value is between 0 and 1/
***Note the problem to have the solution as a u val
***Accepting +/- TOL4 will give the wrong segment
***adress
*/
         if ( u >= 0.0  )
           {
           if ( u < 1.0  )
             {
              numint = numint + 1;
              uout[numint-1] = u;
              }
            }
/*
***Note that u = 0 and 1-TOL4 will be tested
***The minimum number of restarts is 2
*/
         j = JMAX;
         }
/*
***Derivative dfdu to small (only nearest point)  ?
*/
         if ( ABS(dfdu) <= TOL3 ) 
           {
/*
***The step h to big
*/
            j = JMAX;
            h = TOL6;
            }
          else h = -f/dfdu;
/*
***Solution outside 0 < u < 1 ?
*/
       if ( ABS(u) < TOL4  &&  h < 0 ) j = JMAX;

       if ( ABS(u-1.0) < TOL4  &&  h > 0 ) j = JMAX;

       if ( ABS(h) > 2.0 ) j = JMAX;
/*
***Compute the next parameter value
*/
       u = u + h;
       if ( u < -TOL4 ) u = 0.1*TOL4;
       if ( u > 1.0 + TOL4 ) u = 1.0 - 0.1*TOL4;
       } /* end loop j : Newton Rhapson */
     } /* end loop i : Restarts with deltau */

   *pnoint = numint;

   return(0);
 }

/*******************************************************************/
