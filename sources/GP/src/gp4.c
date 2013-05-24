/**********************************************************************
*
*    gp4.c
*    =====
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gppltr();    Project point to active view
*    gppstr();    Perspective transformation
*    gptrpv();    Transform position to active view
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
#include "../../IG/include/IG.h"
#include "../include/GP.h"

extern bool   gpgenz;
extern int    ncrdxy;
extern double x[],y[],z[];
extern DBTmat actvym;
extern VY     actvy;

/*!******************************************************/

        short gppltr(
        DBVector *p,
        double   *x,
        double   *y,
        double   *z)

/*      Parallell projection of 3D point to active viewplane.
 *
 *      In:  p => 3D point.
 *
 *      Out: Projected coordinates in x and y.
 *           If gpgenz == TRUE also in z.
 *
 *      Return: 0
 *
 *      (C)J, Kjellander 2004, Örebro university
 *
 ******************************************************!*/

  {

/*
***Om det är en 3D-vy måste åtminstone X och Y-koordinaten
***transformeras.
*/
    if ( actvy.vy3d )
      {
      *x = actvym.g11 * p->x_gm +
           actvym.g12 * p->y_gm +
           actvym.g13 * p->z_gm;

      *y = actvym.g21 * p->x_gm +
           actvym.g22 * p->y_gm +
           actvym.g23 * p->z_gm;

      if ( gpgenz )
        {
        *z = actvym.g31 * p->x_gm +
             actvym.g32 * p->y_gm +
             actvym.g33 * p->z_gm;
        }
      }
/*
***Om det inte är en 3D-vy behövs ingen transformation.
*/
    else
      {
      *x = p->x_gm; *y = p->y_gm;
      if ( gpgenz ) *z = p->z_gm;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gppstr(
        double x[],
        double y[],
        double z[])

/*      Perspektivtransformerar ncrdxy lång polylinje.
 *
 *      In: x,y,z => Koordinater för polylinje.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 3/2/89 J. Kjellander
 *
 *      3/2/95  Ändrat anrop, J. Kjellander
 *
 ******************************************************!*/

  {
    int    i;
    double d,dz;

    d = actvy.vydist;

    for ( i=0; i<ncrdxy; ++i )
      {
      dz = d - z[i];
      if ( dz > 0.0 )
        {
        x[i] = x[i]/dz; x[i] = x[i]*d;
        y[i] = y[i]/dz; y[i] = y[i]*d;
        z[i] = z[i]/dz; z[i] = z[i]*d;
        }
      }

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short gptrpv(
        DBVector *p)

/*      Transformerar en 3D-position till aktiv vy. Utför
 *      både projicering och perspektiv-transformation.
 *      Används tex. av plot_win och hide_view för att
 *      beräkna plot-origo.
 *
 *      In: p => 3D position.
 *
 *      Ut: Nya X- och Y-koordinater. Z = 0.
 *
 *      FV: 0
 *
 *      (C)microform ab 5/6/91 J. Kjellander
 *
 ******************************************************!*/

  {
    double x,y,z,d,dz;

/*
***Transformation till aktivt vy-plan.
*/
    if ( actvy.vy3d )
      {
      x = actvym.g11 * p->x_gm +
          actvym.g12 * p->y_gm +
          actvym.g13 * p->z_gm;

      y = actvym.g21 * p->x_gm +
          actvym.g22 * p->y_gm +
          actvym.g23 * p->z_gm;

      z = actvym.g31 * p->x_gm +
          actvym.g32 * p->y_gm +
          actvym.g33 * p->z_gm;
      }
    else
      { x = p->x_gm; y = p->y_gm; z = p->z_gm; }
/*
***Perspektiv-transformering.
*/
    if ( actvy.vydist != 0.0 )
      {
      d = actvy.vydist;
      dz = d - z;
      if ( dz > 0.0 )
        {
        x = x/dz; x = x*d;
        y = y/dz; y = y*d;
        z = z/dz; z = z*d;
        }
      }
/*
***Klart.
*/
    p->x_gm = x; p->y_gm = y; p->z_gm = 0.0;

    return(0);
  }

/********************************************************/
