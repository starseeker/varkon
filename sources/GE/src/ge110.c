/*!******************************************************************/
/*  File: ge110.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE110() Evaluator for single arc/curve segment                  */
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

/*!******************************************************/

        DBstatus GE110(
        DBAny   *gmpost,
        DBSeg   *segpek,
        EVALC   *evlpek)

/*      Common evaluation routine for 3D-DBarc and DBCurve.
 *      Input is t_local, ie. t for the segment.
 *
 *      In:  gmpost   =  Pointer to curve/arc-structure.
 *           segpek   =  Pointer to a segment.
 *           evlpek   =  Pointer to an EVALC.
 *
 *      Ut: *evlpek = EVALC with requested data
 *
 *      (C)microform ab 1/5/94 J. Kjellander
 *
 *      1994-05-27 sur214 replaces geo108 Gunnar Liden
 *      1994-12-01 evltyp, J. Kjellander
 *      1999-12-18 sur214->varkon_sur_uvsegeval G Liden
 *      2002-05-23 NURBS, Johan Kjellander,Örebro University
 *      2006-11-03 Added UV_NURB_SEG S.Larsson, Örebro university
 *
 ******************************************************!*/

  {
    short   status,rcode=0;
    int     evltyp;
    char    errbuf[80];
    DBfloat out[17];

/*
***Curves.
*/
    switch ( gmpost->hed_un.type )
      {
/*
***What kind of segment ?
*/
      case CURTYP:
      switch ( segpek->typ )
        {
/*
***Cubic segment.
*/
        case CUB_SEG:
        evltyp = evlpek->evltyp;
        rcode  = -1;
        if (  evltyp & EVC_R   ) rcode = 0;
        if (  evltyp & EVC_DR  ) rcode = 1;
        if (  evltyp & EVC_D2R ) rcode = 2;
        if ( (evltyp & EVC_PN ) ||
             (evltyp & EVC_BN ) ||
             (evltyp & EVC_KAP) ) rcode = 3;
        if ( rcode < 0 )
          {
          sprintf(errbuf,"%d",evltyp);
          return(erpush("GE1303",errbuf));
          }

        status = GE107(gmpost,segpek,evlpek->t_local,rcode,out);
        if ( status < 0 ) return(status);

        evlpek->r.x_gm = out[0];
        evlpek->r.y_gm = out[1];
        evlpek->r.z_gm = out[2];

        if ( rcode > 0 )
          {
          evlpek->drdt.x_gm = out[3];
          evlpek->drdt.y_gm = out[4];
          evlpek->drdt.z_gm = out[5];
          }
        if ( rcode > 1 )
          {
          evlpek->d2rdt2.x_gm = out[6];
          evlpek->d2rdt2.y_gm = out[7];
          evlpek->d2rdt2.z_gm = out[8];
          }
        if ( rcode > 2 )
          {
          evlpek->p_norm.x_gm = out[9];
          evlpek->p_norm.y_gm = out[10];
          evlpek->p_norm.z_gm = out[11];
          evlpek->b_norm.x_gm = out[12];
          evlpek->b_norm.y_gm = out[13];
          evlpek->b_norm.z_gm = out[14];
          evlpek->kappa       = out[15];
          }
        break;
/*
***UV-segment, cubic or NURBS.
*/
        case UV_CUB_SEG:
        case UV_NURB_SEG:
        return(varkon_sur_uvsegeval((DBCurve *)gmpost,segpek,evlpek));        
/*
***NURBS-span.
*/
        case NURB_SEG:
        return(GEevalnc((DBCurve *)gmpost,segpek,evlpek));
/*
***Error, unknown type of segment.
*/
        default:
        sprintf(errbuf,"%d",segpek->typ);
        return(erpush("GE1293",errbuf));
        }
      break;
/*
***3D-Circle.
*/
      case ARCTYP:
      evltyp = evlpek->evltyp;
      if (  evltyp & EVC_R   ) rcode = 0;
      if (  evltyp & EVC_DR  ) rcode = 1;
      if (  evltyp & EVC_D2R ) rcode = 2;
      if ( (evltyp & EVC_PN ) ||
           (evltyp & EVC_BN ) ||
           (evltyp & EVC_KAP) ) rcode = 3;

      status = GE107(gmpost,segpek,evlpek->t_local,rcode,out);
      if ( status < 0 ) return(status);

      evlpek->r.x_gm = out[0];
      evlpek->r.y_gm = out[1];
      evlpek->r.z_gm = out[2];

      if ( rcode > 0 )
        {
        evlpek->drdt.x_gm = out[3];
        evlpek->drdt.y_gm = out[4];
        evlpek->drdt.z_gm = out[5];
        }
      if ( rcode > 1 )
        {
        evlpek->d2rdt2.x_gm = out[6];
        evlpek->d2rdt2.y_gm = out[7];
        evlpek->d2rdt2.z_gm = out[8];
        }
      if ( rcode > 2 )
        {
        evlpek->p_norm.x_gm = out[9];
        evlpek->p_norm.y_gm = out[10];
        evlpek->p_norm.z_gm = out[11];
        evlpek->b_norm.x_gm = out[12];
        evlpek->b_norm.y_gm = out[13];
        evlpek->b_norm.z_gm = out[14];
        evlpek->kappa       = out[15];
        }
      break;
/*
***Error, unknown type of entity.
*/
      default:
      sprintf(errbuf,"%d",gmpost->hed_un.type);
      return(erpush("GE1283",errbuf));
      }

   return(0);
  }

/********************************************************/
