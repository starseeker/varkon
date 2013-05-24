/**********************************************************************
*
*    ge402.c
*    =======
*
*    This file includes:
*
*    GE402() Position of tangency on arc/curve
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
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
/*  1985-08-15 Support for 2D-circle J. Kjellander                  */
/*  1986-05-11 extern short, B. Doverud                             */
/*  1991-11-20 Nytt format för kurvor, J. Kjellander                */
/*  1995-03-08 Bug GEsort_1(), J. Kjellander                          */
/*  1999-04-02 Rewritten. J.Kjellander                              */
/*                                                                  */
/********************************************************************/



      DBstatus GE402(
      DBAny    *pstr1,   /* Pointer to a arc/curve entity           */
      DBSeg    *pseg1,   /* Pointer to arc/curve segments           */
      DBVector *pexpt,   /* The external position                   */
      DBTmat   *pc,      /* Pointer to current coordinatesystem     */
      short     nalter,  /* Requested solution number               */
      DBfloat  *puout,   /* Pointer to the output u value           */
      DBVector *ptanpt)  /* Pointer to the output position          */

 {
   short     status;      /* Function value from gei402             */
   DBArc    *parc1;       /* If the curve is an arc                 */
   DBCurve  *pcur1;       /* If the curve ia a  curve               */
   short     noseg=0;     /* Number of segments in the curve        */
   DBSeg    *pcseg;       /* Pointer to the current rational segment*/
   short     nstart;      /* The number of restarts (=RESTRT except */
                          /* for arcs where nstart=RESTRT+1 )       */
   short     noinse;      /* Number of intersects from one segment  */
   short     noint;       /* Number of solutions on the curve       */
   DBfloat   useg[INTMAX];/* The unordered array of u solutions from*/
                          /* one segment                            */
   short     k;           /* Loop index                             */
   short     noall;       /* The total number of intersects incl.   */
                          /* possible double points ( and before    */
                          /* ordering of the u values )             */
   DBfloat  uglob[INTMAX];/* The global u before ordering           */
   DBfloat  uout[INTMAX]; /* The global u after  ordering           */
   short    i;            /* Loop index                             */
   DBVector ploc;         /* For the projecion of the output point  */
   EVALC    evldat;       /* For GE109()                            */


/*
***Retrieve the number of segments from the curve
*/
   switch ( pstr1->hed_un.type )
     {
     case ARCTYP:      
     parc1 = (DBArc *)pstr1;
     noseg = parc1->ns_a;
/*
***2D Arc is done in GE207)=.
*/
     if ( noseg == 0 )
       {
       if ( GE207(parc1,pexpt,pc,nalter,puout,ptanpt) < 0 )
         return(erpush("GE4053","GE402"));
       else return(0);
       }
     break;

     case CURTYP:
     pcur1 = (DBCurve *)pstr1;
     noseg = pcur1->ns_cu;
     break;

     default:
     return(erpush("GE4063","GE402"));
     }
/*
***The total number of intersects before ordering and deletion
***of equal points . Start value = 0
*/
   noall = 0;
/*
***The number of restarts
*/
   nstart = RESTRT;

   if ( pstr1->hed_un.type == ARCTYP ) nstart = nstart + 1;
/*
***Loop for all segments in the curve
*/
   pcseg = pseg1;

   for ( k=1; k <= noseg; k++ )
     {
     status = GE403(pstr1,pcseg,pexpt,pc,nstart,&noinse,useg);
     if ( status < 0 ) return(erpush("GE4053","GE402(GE402)")); 
/*
***Add u values to the global u vector
*/
     for ( i=noall; i <= noall+noinse-1; i++ )
           uglob[i] = useg[i-noall] + (DBfloat)k;
/*
***Increase the number of intersects
*/
     noall = noall + noinse;
/*
***Next segment.
*/
     ++pcseg;
     }
/*
***noint = noall if noall <= 1
*/
   noint = noall;

   if ( noall == 0 ) return(erpush("GE4072","GE402"));
/*
***Här har en bug rättats 8/3-95. Tidigare anropades inte
***GEsort_1() om det bara blev en lösning. Likadant är det
***i resten av geopac. Istället skulle man här ha satt
***noint = 1 och uoutÄ0Å = uglobÄ0Å men detsaknades.
***Bästa lösningen är dock att anropa GEsort_1 även om det
***bara är en (eller ingen) lösning. Så får det bli här.
***Det är osäkert om den GEsort_1() som GL skrev ursprung-
***ligen klarar det och dessutom var den lite omodern så
***för säkerhets skull har också en ny GEsort_1() skrivits
***som säkert klarar 0, 1 eller oändligt många lösningar
***och som säkert är mycket snabbare än orginalet.
*/
   if ( GEsort_1(uglob,noall,&noint,uout ) < 0 ) 
     return(erpush("GE4053","GE402(GEsort_1)"));
/*
***Existence of the requested solution
*/
   if ( nalter > noint || nalter < 1 )return(erpush("GE4072","GE402"));
/*
***Compute the corresponding R3 coordinates.
*/
   evldat.evltyp = EVC_R;
   evldat.t_global = uout[nalter-1];

   if ( GE109(pstr1,pseg1,&evldat) < 0 )
       return(erpush("GE4034","GE402(GE109)"));

   ptanpt->x_gm = evldat.r.x_gm;
   ptanpt->y_gm = evldat.r.y_gm;
   ptanpt->z_gm = evldat.r.z_gm;
/*
***Project onto the xy - plane
*/
   if ( pc == NULL )
     {
     ptanpt->z_gm = 0.0;
     return(0);
     }

   if ( GEtfpos_to_local(ptanpt,pc,&ploc) < 0 )
     return(erpush("GE4053","GE402(GEtfpos_to_local)"));
   ploc.z_gm = 0.0;
   if ( GEtfpos_to_basic(&ploc,pc,ptanpt) < 0 )
     return(erpush("GE4053","GE402(GEtfpos_to_basic)"));

   return(0);
 } 

/********************************************************************/
