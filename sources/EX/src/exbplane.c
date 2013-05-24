/*!*******************************************************
*
*    exbplane.c
*    ==========
*
*    EXebpl();     Create B-plane
*    EXbpln();     Create B_PLANE
*
*
*    This file is part of the VARKON Execute Library.
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
#include <string.h>

extern char    jobdir[];
extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;

/*!******************************************************/

       short     EXebpl(
       DBId     *id,
       DBBplane *bplpek,
       V2NAPA   *pnp)

/*      Skapar B-plan.
 *
 *      In: id     => Pekare till identitet.
 *          bplpek => Pekare till DB-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1842 = Kan ej lagra B-plan i DB.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      20/3/92  lsysla, J. Kjellander
 *      24/7/04  width, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparametrar.
*/
    bplpek->hed_bp.blank = pnp->blank;
    bplpek->hed_bp.pen   = pnp->pen;
    bplpek->hed_bp.level = pnp->level;
    bplpek->pcsy_bp      = lsysla;
    bplpek->wdt_bp       = pnp->width;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      bplpek->hed_bp.hit = pnp->hit;
      if ( DBinsert_bplane(bplpek,id,&la) < 0 ) return(erpush("EX1842",""));
      }
    else
      {
      bplpek->hed_bp.hit = 0;
      }
/*
***Rita.
*/
    WPdrbp(bplpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXbpln(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBVector *p4,
       V2NAPA   *pnp)

/*      Skapar B_PLANE.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Startpunkt.
 *          p2     => Punkt 2.
 *          p2     => Punkt 3.
 *          p4     => Slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *      1999-12-18 sur718->varkon_bpl_analyse G Liden
 *
 ******************************************************!*/

  {
    DBBplane bpl;

/* #define BPLAN_TEST */
#ifdef BPLAN_TEST

    DBint   btype;       /* Type of B-plane polygon                 */
                         /* Type of polygon                         */
                         /* Eq.  1: Triangle                        */
                         /* Eq.  2: Convex 4-point polygon          */
                         /* Eq.  3: Non-convex polygon              */
                         /* Eq.  4: p4, p1, p2 are colinear         */
                         /* Eq.  5: p1, p2, p3 are colinear         */
                         /* Eq.  6: p2, p3, p4 are colinear         */
                         /* Eq.  7: p3, p4, p1 are colinear         */
                         /* Eq. -1: p1 = p2                         */
                         /* Eq. -2: p3 = p4                         */
                         /* Eq. -3: p1 = p4                         */
                         /* Eq. -4: p1, p2, p3, p4 not coplanar     */
                         /* Eq. -5: Self-intersecting polygon       */
  DBint    pnr;          /* Non-convex point number                 */
  DBint    status;       /* Error code from called function         */
#endif

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      GEtfpos_to_local(p4,&lklsyi,p4);
      }
/*
***Ev. kontroll av planets hörn.
*/
#ifdef BPLAN_TEST
    status = varkon_bpl_analyse (p1, p2, p3, p4, &btype, &pnr );
    if ( btype == -1 )  return(erpush("SU1203",""));
    if ( btype == -2 )  return(erpush("SU1213",""));
    if ( btype == -3 )  return(erpush("SU1223",""));
    if ( btype == -4 )  return(erpush("SU1233",""));
    if ( btype == -5 )  return(erpush("SU1243",""));
#endif
/*
***Lagra positioner i B-plan posten.
*/
    V3MOME((char *)p1,(char *)&bpl.crd1_bp,sizeof(DBVector));
    V3MOME((char *)p2,(char *)&bpl.crd2_bp,sizeof(DBVector));
    V3MOME((char *)p3,(char *)&bpl.crd3_bp,sizeof(DBVector));
    V3MOME((char *)p4,(char *)&bpl.crd4_bp,sizeof(DBVector));
/*
***Lagra i gm och rita.
*/
    return ( EXebpl(id,&bpl,pnp)); 
  }

/********************************************************/
