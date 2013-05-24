/**********************************************************************
*
*    ge207.c
*    =======
*
*    This file includes:
*
*    GE207() Compute the position of tangency between line and 2D arc
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

/*!******************************************************/

        DBstatus GE207(
        GMARC    *arcp,
        DBVector *pos,
        DBTmat   *pc,
        DBshort   nalter,
        DBfloat  *puout,
        DBVector *ptanpt)

/*      Geopac-rutin för funktionen linje från pos. till
 *      tangent på en 2D-cirkel.
 *
 *      In: *arcp   => Pekare till cirkel_structure.
 *          *pos    => Startposition för linjen.
 *          *pc     => Pekare till lokalt koordinatsystem.
 *           nalter => Anger vilken punkt som skall returneras.
 *           puout  => Pekare till parametervärde.
 *          *ptanpt => Pekare till koordinater.
 *
 *      Ut: *puout  => Parametervärde för tangeringspunkten.
 *          *ptanpt => Den begärda punktens koordinater.
 *
 *      FV:  0      => Ok.
 *           GE2172 => Tangeringspunkten saknas
 *
 *      (C)microform ab 6/8/85 R. Svedin
 *           Reviderad 15/8/85 J. Kjellander
 *           Reviderad 14/9/85 J. Kjellander beräkn. av u
 *
 ******************************************************!*/

  {
     DBfloat q,d,t1,t2,s1,s2,x[2],y[2],u[2],dx,dy;
     short notan;
/*
***Div. initiering.
*/
     d = (arcp->x_a - pos->x_gm) * (arcp->x_a - pos->x_gm) +
         (arcp->y_a - pos->y_gm) * (arcp->y_a - pos->y_gm);

     if ( sqrt(d) < arcp->r_a + 0.001 ) return(-1 );
/*
***Beräkna tangeringspunkter.
*/
     if ( fabs(pos->x_gm - arcp->x_a) > 0.001 )
        {
        q = sqrt(arcp->r_a * arcp->r_a * (arcp->y_a - pos->y_gm) *
            (arcp->y_a - pos->y_gm) + d * (arcp->x_a - pos->x_gm) *
            (arcp->x_a - pos->x_gm) - d * arcp->r_a * arcp->r_a);
      
        t1 = -(arcp->r_a * arcp->r_a) * (arcp->y_a - pos->y_gm) /
             d + arcp->r_a / d * q;

        t2 = -(arcp->r_a * arcp->r_a) * (arcp->y_a - pos->y_gm) /
             d - arcp->r_a / d * q;

        s1 = -(arcp->r_a * arcp->r_a + t1 * (arcp->y_a - pos->y_gm)) /
             (arcp->x_a - pos->x_gm);

        s2 = -(arcp->r_a * arcp->r_a + t2 * (arcp->y_a - pos->y_gm)) /
             (arcp->x_a - pos->x_gm);
        }
/*
***Alternativ metod.
*/
     else
        {
        q = sqrt(arcp->r_a * arcp->r_a * (arcp->x_a - pos->x_gm) *
            (arcp->x_a - pos->x_gm) + d * (arcp->y_a - pos->y_gm) *
            (arcp->y_a - pos->y_gm) - d * arcp->r_a * arcp->r_a);

        s1 = -(arcp->r_a * arcp->r_a) * (arcp->x_a - pos->x_gm) /
             d + arcp->r_a / d * q;

        s2 = -(arcp->r_a * arcp->r_a) * (arcp->x_a - pos->x_gm) /
             d - arcp->r_a / d * q;

        t1 = -(arcp->r_a * arcp->r_a + s1 * (arcp->x_a - pos->x_gm)) /
             (arcp->y_a - pos->y_gm);

        t2 = -(arcp->r_a * arcp->r_a + s2 * (arcp->x_a - pos->x_gm)) /
             (arcp->y_a - pos->y_gm);
        }
/*
***Tangeringspunkternas koordinater.
*/
     x[0] = s1 + arcp->x_a;
     y[0] = t1 + arcp->y_a;

     x[1] = s2 + arcp->x_a;
     y[1] = t2 + arcp->y_a;
/*
***Motsvarande parametervärden.
*/
     dx = x[0] - arcp->x_a;
     dy = y[0] - arcp->y_a;
     GE315(arcp,dx,dy,&u[0]);

     dx = x[1] - arcp->x_a;
     dy = y[1] - arcp->y_a;
     GE315(arcp,dx,dy,&u[1]);
/*
***Sortera bort punkter som inte ligger på bågen.
*/
     notan = 2;

     if ( u[0] < 1.0 || u[0] > 2.0 )
        {
        u[0] = u[1]; 
        x[0] = x[1];
        y[0] = y[1];
        --notan;
        }

     if ( u[1] < 1.0 || u[1] > 2.0 ) --notan;
/*
***Välj punkt.
*/
     if ( nalter > notan ) return(erpush("GE2172",""));

     --nalter;
     ptanpt->x_gm = x[nalter];
     ptanpt->y_gm = y[nalter];
    *puout = u[nalter];

     return(0);
  }

/********************************************************/



