/*!*******************************************************
*
*    exxhatch.c
*    ==========
*
*    EXexht();     Create xhatch
*    EXxht();      Create XHATCH
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

extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;

/*!******************************************************/

       short    EXexht(
       DBId    *id,
       DBHatch *xhtpek,
       DBfloat  crdvek[],
       V2NAPA  *pnp)

/*      Create a hatch (XHATCH), store in DB and display.
 *
 *      In: id     => C ptr to hatch ID
 *          xhtpek => C ptr to hatch record
 *          crdvek => C ptr to hatch lines
 *          pnp    => C ptr to attributes
 *
 *
 *      Return:  0 = Ok.
 *          EX1542 = Can't store hatch in DB.
 *
 *      (C)microform ab 15/11/85 B. Doverud
 *
 *      15/10/86   SAVE, J.Kjellander
 *      27/12/86   hit, J.Kjellander
 *      20/3/92    lsysla, J.Kjellander
 *      2007-09-01 WIDTH, J.Kjellander
 *      2007-09-30 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attribute data and csys.
*/
    xhtpek->hed_xh.blank = pnp->blank;
    xhtpek->hed_xh.pen   = pnp->pen;
    xhtpek->hed_xh.level = pnp->level;
    xhtpek->fnt_xh       = pnp->xfont;
    xhtpek->lgt_xh       = pnp->xdashl;
    xhtpek->wdt_xh       = pnp->width;
    xhtpek->pcsy_xh      = lsysla;
/*
***Save hatch in DB.
*/
    if ( pnp->save )
      {
      xhtpek->hed_xh.hit = pnp->hit;
      if ( DBinsert_xhatch(xhtpek,crdvek,id,&la) < 0 )
        return(erpush("EX1542",""));
      }
    else
      {
      xhtpek->hed_xh.hit = 0;
      }
/*
***Display hatch.
*/
    if ( xhtpek->pcsy_xh > 0 ) DBread_csys(&csy,NULL,xhtpek->pcsy_xh);
    WPdrxh(xhtpek,crdvek,&csy,la,GWIN_ALL);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short   EXxht(
       DBId   *id,
       DBId   *ridvek,
       DBshort nref,
       DBfloat dist,
       DBfloat ang,
       V2NAPA *pnp)

/*      Create a XHATCH.
 *
 *      In: id     => C ptr to hatch entity ID.
 *          ridvek => C ptr to array of refs.
 *          nref   => Number of refs.
 *          dist   => Hatch line distance.
 *          ang    => Hatch line angle.
 *          pnp    => C ptr to attributes.
 *
 *      Return:  0 = Ok.
 *          EX1402 = Entity does not exist in DB
 *          EX1412 = Illegal entity type
 *          EX1552 = Can't calculate hatch data
 *          EX1962 = malloc() error.
 *
 *      (C)microform ab 19/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensräknare, J. Kjellander
 *      20/11/85 Anrop till EXexht, B. Doverud
 *      29/10/86 Flyttat ut linvek o arcvek, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      24/7/90  Kurvor, J. Kjellander
 *      9/6/93   Dynamiska segment, J. Kjellander
 *      2007-10-06 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la;
    DBetype  typ;
    short    i,nlin,narc,ncur,status;
    DBfloat  angt;
    DBfloat  crdvek[4*GMXMXL];
    DBLine  *lpvek[GMMXXH],lin[GMMXXH];
    DBArc   *apvek[GMMXXH],arc[GMMXXH];
    DBSeg   *aspvek[GMMXXH],arcseg[GMMXXH][4];
    DBCurve *cpvek[GMMXXH],cur[GMMXXH];
    DBSeg   *cspvek[GMMXXH];
    DBHatch  xht;
    DBVector pos;

/*
***Get the geometry of all referenced entities and save
***in mallocated C memory.
*/
    nlin = narc = ncur = 0;

    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&la,&typ) < 0 ) 
        return(erpush("EX1402",""));

      switch ( typ )
         {
         case (LINTYP):
         DBread_line(&lin[nlin],la);
         lpvek[nlin] = &lin[nlin];
         if ( lsyspk != NULL ) GEtfLine_to_local(lpvek[nlin],lsyspk,lpvek[nlin]);
       ++nlin;
         break;

         case (ARCTYP):
         DBread_arc(&arc[narc],&arcseg[narc][0],la);
         if ( arc[narc].ns_a == 0 )
           {
           pos.x_gm = arc[narc].x_a;
           pos.y_gm = arc[narc].y_a;
           pos.z_gm = 0.0;

           if ( GE300(&pos,arc[narc].r_a,arc[narc].v1_a,arc[narc].v2_a,NULL,
                 &arc[narc],&arcseg[narc][0],3) < 0 ) return(erpush("GE7213","GE300"));
           }
         apvek[narc] = &arc[narc];
         aspvek[narc] = &arcseg[narc][0];
         if ( lsyspk != NULL )
           GEtfArc_to_local(apvek[narc],aspvek[narc],lsyspk,apvek[narc],aspvek[narc]);
       ++narc;
         break;

         case (CURTYP):
         DBread_curve(&cur[ncur],NULL,&cspvek[ncur],la);
         cpvek[ncur] = &cur[ncur];
         if ( lsyspk != NULL )
           GEtfCurve_to_local(cpvek[narc],cspvek[narc],NULL,lsyspk,cpvek[narc],cspvek[narc],NULL);
       ++ncur;
         break;

         default:
         status = erpush("EX1412","");
         goto end;
         }
      }
/*
***Angle and distance.
*/
    xht.dist_xh = dist;

    GEtfang_to_basic(ang,lsyspk,&angt);

    xht.ang_xh = angt;
    xht.nlin_xh = 0;
/*
***Calculate hatch line coordinates.
*/
    if ( GEhatch2D(lpvek,nlin,apvek,aspvek,narc,cpvek,cspvek,ncur,lsyspk,&xht,crdvek) < 0 )
      {
      status = erpush("EX1552","");
      goto end;
      }
/*
***Save in DB and display.
*/
    status = EXexht(id,&xht,crdvek,pnp);
/*
***Release C memory allocated for curve segments.
*/
end:
    if ( ncur > 0 )
      {
      for ( i=0; i<ncur; ++i ) DBfree_segments(cspvek[i]);
      }
/*
***The end.
*/
    return(status);
  }

/********************************************************/
