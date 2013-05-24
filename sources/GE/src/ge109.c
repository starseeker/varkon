/*!******************************************************************/
/*  File: ge109.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE109() Evaluation of multi segment arcs and curves             */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://www.varkon.com                                     */
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

/********************************************************************/

      DBstatus GE109(
      DBAny   *gmpost,
      DBSeg   *segpek,
      EVALC   *evlpek)

/*    Evaluation of arcs and curves with global parameter
 *    as input. GE109() supports all curve representations.
 *
 *      In: gmpost  => Pointer to arc/curve
 *          segpek  => Pointer to segments
 *          evlpek  => Requested evaluation and global
 *                     parametric value
 *
 *      Out: *evlpek => Requested evaluated data
 *
 *      (C)microform ab 1999-04-14 J. Kjellander
 *      1999-12-18 sur785->varkon_cur_segadr Liden
 *      2006-12-12 Optimizations for speed, J.Kjellander
 *
 *****************************************************************!*/

 {
   int     it;
   short   status;
   char    errbuf[80];
   DBfloat t;

/*
***Arc or curve ?
*/
   switch ( gmpost->hed_un.type )
     {
/*
***Curve. Compute what segment is involved and it's local
***parametric value.
*/
     case CURTYP:
     if ( (status=varkon_cur_segadr(evlpek->t_global,gmpost->cur_un.ns_cu,
                                   &evlpek->iseg,&evlpek->t_local)) < 0 ) return(status);
     else return(GE110(gmpost,segpek+evlpek->iseg-1,evlpek));
/*
***Same for 3D-Arc.
*/
     case ARCTYP:
     t = evlpek->t_global - 1.0;
     if ( t > gmpost->arc_un.ns_a ) t = gmpost->arc_un.ns_a;
     else if ( t < 0.0 ) t = 0.0;
     if ( (it=HEL(t)) == gmpost->arc_un.ns_a ) it -= 1;
     evlpek->t_local = t-it;
     evlpek->iseg    = it;
     return(GE110(gmpost,segpek+it,evlpek));
/*
***Unknown entity type.
*/
     default:
     sprintf(errbuf,"%d",gmpost->hed_un.type);
     return(erpush("GE1283",errbuf));
     }

   return(0);
 }

/********************************************************************/
