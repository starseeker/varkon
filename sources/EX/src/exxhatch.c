/*!*******************************************************
*
*    exxhatch.c
*    ==========
*
*    EXexht();     Create xhatch
*    EXxht();      Create XHATCH
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
*
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

       short EXexht(
       DBId   *id,
       DBHatch  *xhtpek,
       DBfloat crdvek[],
       V2NAPA *pnp)

/*      Skapa snitt.
 *
 *      In: id     => Pekare till snittets identitet.
 *          xhtpek => Pekare till snitt-structure.
 *          crdvek => Pekare till snittlinje-vektor
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = Ok.
 *          EX1542 = Kan ej lagra snitt i GM.
 *
 *      (C)microform ab 15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparameterdata.
*/
    xhtpek->hed_xh.blank = pnp->blank;
    xhtpek->hed_xh.pen   = pnp->pen;
    xhtpek->hed_xh.level = pnp->level;
    xhtpek->fnt_xh       = pnp->xfont;
    xhtpek->lgt_xh       = pnp->xdashl;
    xhtpek->pcsy_xh      = lsysla;
/*
***Lagra posten i GM.
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
***Rita snittet.
*/
    WPdrxh(xhtpek,crdvek,la,GWIN_ALL);

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short EXxht(
       DBId   *id,
       DBId   *ridvek,
       DBshort nref,
       DBfloat dist,
       DBfloat ang,
       V2NAPA *pnp)

/*      Skapa snitt.
 *
 *      In: id     => Pekare till snittets identitet.
 *          ridvek => Pekare till vektor med referenser.
 *          nref   => Antal referenser.
 *          dist   => Avstånd mellan snittlinjer.
 *          ang    => Lutning på snittlinjerna.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = Ok.
 *          EX1402 = Den refererade storhten finns ej i GM
 *          EX1412 = Otillåten geometri-typ för denna operation
 *          EX1552 = Kan ej beräkna snitt-data
 *          EX1542 = Kan ej lagra snitt i GM.
 *          EX1962 = Fel från malloc().
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
 *
 ******************************************************!*/

  {
    register short   i;            /* Loopräknare */

    DBptr   la;
    DBetype   typ;
    short   nlin,narc,ncur,status;
    DBfloat   angt;
    DBfloat   crdvek[4*GMXMXL];
    DBLine  *linvek=NULL,*lpvek[GMMXXH];
    DBArc  *arcvek=NULL,*apvek[GMMXXH];
    DBCurve  *curvek=NULL,*cpvek[GMMXXH];
    DBSeg  *spvek[GMMXXH];
    DBHatch   xht;

/*
***Hämta geometri-data för samtliga refererade storheter och
***lagra i mallokerade minnesareor.
*/
    nlin = narc = ncur = 0;

    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&la,&typ) < 0 ) 
        return(erpush("EX1402",""));

      switch ( typ )
         {
         case (LINTYP):
         if ( nlin == 0 )
           {
           if ( (linvek=(DBLine *)v3mall((unsigned)(GMMXXH*sizeof(GMLIN)),
                           "EXxht")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_line(&linvek[nlin],la);
         lpvek[nlin] = &linvek[nlin];
         ++nlin;
         break;

         case (ARCTYP):
         if ( narc == 0 )
           {
           if ( (arcvek=(DBArc *)v3mall((unsigned)(GMMXXH*sizeof(GMARC)),
                           "EXxht")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_arc(&arcvek[narc],NULL,la);
         apvek[narc] = &arcvek[narc];
         ++narc;
         break;

         case (CURTYP):
         if ( ncur == 0 )
           {
           if ((curvek=(DBCurve *)v3mall((unsigned)(GMMXXH*sizeof(GMCUR)),
                          "EXxht")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_curve(&curvek[ncur],NULL,&spvek[ncur],la);
         cpvek[ncur] = &curvek[ncur];
         ++ncur;
         break;

         default:
         status = erpush("EX1412","");
         goto end;
         }
      }
/*
***Vinkel och avstånd.
*/
    xht.dist_xh = dist;

    GEtfang_to_basic(ang,lsyspk,&angt);

    xht.ang_xh = angt;
    xht.nlin_xh = 0;
/*
***Generera snittlinjer.
*/
    if ( GEhatch2D(lpvek,nlin,apvek,narc,cpvek,spvek,ncur,&xht,crdvek) < 0 )
      {
      status = erpush("EX1552","");
      goto end;
      }
/*
***Lagra i gm och rita.
*/
    status = EXexht(id,&xht,crdvek,pnp);
/*
***Deallokera minne.
*/
end:
    if ( nlin > 0 ) v3free(linvek,"EXxht");
    if ( narc > 0 ) v3free(arcvek,"EXxht");
    if ( ncur > 0 )
      {
      v3free(curvek,"EXxht");
      for ( i=0; i<ncur; ++i ) DBfree_segments(spvek[i]);
      }
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
