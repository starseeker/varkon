/*!*******************************************************
*
*     expoint.c
*     =========
*
*     EXepoi();     Create point
*     EXpofr();     Create POI_FREE
*     EXpopr();     Create POI_PROJ
*
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://www.varkon.com
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../GP/include/GP.h"
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif
#include "../include/EX.h"

extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern DBptr   lsysla;
extern short   gptrty,modtyp;

#ifdef WIN32
extern short wpdrpo();
#endif


/*!******************************************************/

       short EXepoi(
       DBId    *id,
       DBPoint   *poipek,
       V2NAPA  *pnp)

/*     Skapar punkt, lagrar i DB och ritar.
 *
 *     In: id     => Pekare till identitet.
 *         poipek => Pekare till DB-struktur.
 *         pnp    => Pekare till namnparameterblock.
 *
 *     Ut: Inget.
 *
 *     Felkod:      0 = Ok.
 *             EX1312 = Kan ej lagra punkt i DB.
 *
 *     (C)microform ab 14/11/85 B.Doverud
 *
 *     15/10/86 SAVE, J. Kjellander
 *     27/12/86 hit och save, J. Kjellander
 *     20/3/92  lsysla, J. Kjellander
 *     1998-04-03 WIDTH, J.Kjellander
 *     1999-12-18 Empty error message stack G Liden
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Namnparametrar.
*/
    poipek->hed_p.blank = (char)pnp->blank;
    poipek->hed_p.pen   = (short)pnp->pen;
    poipek->hed_p.level = (short)pnp->level;
    poipek->wdt_p       = pnp->width;
    poipek->pcsy_p      = lsysla;
/*
***Lagra i DB.
*/
    if ( pnp->save )
      {
      poipek->hed_p.hit = (char)pnp->hit;
      if ( DBinsert_point(poipek,id,&la) < 0 ) return(erpush("EX1312",""));
      }
    else
      {
      poipek->hed_p.hit = 0;
      }
/*
***Rita.
*/
    gpdrpo(poipek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrpo(poipek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrpo(poipek,la,GWIN_ALL);
#endif

/*
***Succesful creation of point. Empty error message stack
*/

    varkon_erinit();

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXpofr(
       DBId     *id,
       DBVector *pv,
       V2NAPA   *pnp)

/*      Skapar POI_FREE, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Pekare till koordinater.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1302 = Kan ej ber�kna punkt-data..
 *              EX1312 = Kan ej lagra punkt i DB.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXepoi, B. Doverud
 *      27/12/86 hit och save, J. Kjellander
 *
 ******************************************************!*/

  {
    DBPoint   poi;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pv,&lklsyi,pv);
/*
***Ber�kna koordinater.
*/
    if ( GE400(pv,&poi) < 0 )
         return(erpush("EX1302",""));
/*
***Lagra i gm och rita.
*/
    return ( EXepoi(id,&poi,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXpopr(
       DBId     *id,
       DBVector *pv,
       V2NAPA   *pnp)

/*      Skapar POI_PROJ, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Pekare till koordinater.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1302 = Kan ej ber�kna punkt-data.
 *              EX1312 = Kan ej lagra punkt i DB.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXepoi, B. Doverud
 *      27/12/86 hit och save, J. Kjellander
 *
 ******************************************************!*/

  {
    DBPoint   poi;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pv,&lklsyi,pv);
/*
***Ber�kna koordinater.
*/
    if ( GE401(pv,lsyspk,&poi) < 0 )
         return(erpush("EX1302",""));
/*
***Lagra i gm och rita.
*/
    return ( EXepoi(id,&poi,pnp));
  }

/********************************************************/