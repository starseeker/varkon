/*!*******************************************************
*
*    exarc.c
*    =======
*
*    EXearc();    Create arc
*    EXarfr();    Create ARC_1POS
*    EXar2p();    Create ARC_2POS
*    EXar3p();    Create ARC_3POS
*    EXarof();    Create ARC_OFFS
*    EXarfl();    Create ARC_FIL
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

extern short   modtyp;
extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;


/*!******************************************************/

       short   EXearc(
       DBId   *id,
       DBArc  *arcpek,
       DBSeg   segmnt[],
       V2NAPA *pnp)

/*      Create arc. Generic for all arc methods.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          arcpek => Pekare till GM-struktur.
 *          segmnt => Pekare till segment-array.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:  EX1352 = Kan ej lagra cirkelbåge i GM.
 *
 *      (C)microform ab  14/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      26/3/92  båglängd, J. Kjellander
 *      2006-12-09 Removed gpdrar(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBfloat dummy;
    short   status;

/*
***Current arc attributes.
*/
    arcpek->hed_a.blank = pnp->blank;
    arcpek->hed_a.pen   = pnp->pen;
    arcpek->hed_a.level = pnp->level;
    arcpek->fnt_a       = pnp->afont;
    arcpek->lgt_a       = pnp->adashl;
    arcpek->wdt_a       = pnp->width;
    arcpek->pcsy_a      = lsysla;
/*
***Calculate segment lengths and total arc length.
***Result is stored by GEarclength() in DBArc and DBSeg.
***Setting the entity type to ARCTYP is needed by GEarclength()
*/
   arcpek->hed_a.type = ARCTYP;
   if ( (status=GEarclength((DBAny *)arcpek,segmnt,&dummy)) < 0 ) return(status);
/*
***Save arc in DB.
*/
    if ( pnp->save )
      {
      arcpek->hed_a.hit = pnp->hit;
      if ( DBinsert_arc(arcpek,segmnt,id,&la) < 0 ) return(erpush("EX1352",""));
      }
    else
      {
      arcpek->hed_a.hit = 0;
      }
/*
***Display in all graphical windows.
*/
    WPdrar(arcpek,segmnt,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXarfr(
       DBId     *id,
       DBVector *pos,
       DBfloat   r,
       DBfloat   v1,
       DBfloat   v2,
       V2NAPA   *pnp)

/*      Skapa cirkelbåge med position som centrum. Lagra
 *      i GM och rita på skärm.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          pos    => Pekare till centrumpunkt.
 *          r      => Radie.
 *          v1     => Startvinkel.
 *          v2     => Slutvinkel.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1342 = Kan ej beräkna cirkel-data
 *                EX1352 = Kan ej lagra cirkelbåge i GM.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *	20/11/85 Anrop till EXearc, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBArc   arc;
    DBSeg   segmnt[4];

/*
***Transformera origo till basic.
*/
    if ( lsyspk != 0 ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Beräkna gm-data.
*/
      if ( GE300(pos,r,v1,v2,lsyspk,&arc,segmnt,modtyp) < 0 ) 
                  return(erpush("EX1342",""));
/*
***Lagra i gm och rita.
*/
    return ( EXearc(id,&arc,segmnt,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXar2p(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBfloat   r,
       V2NAPA   *pnp)

/*      Skapa cirkelbåge genom 2 punkter och lagra i GM.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          p1     => Pekare till startpunkt.
 *          p2     => Pekare till slutpunkt.
 *          r      => Radie.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1342 = Kan ej beräkna cirkel-data
 *                EX1352 = Kan ej lagra cirkelbåge i GM.
 *
 *      (C)microform ab 27/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *	20/11/85 Anrop till EXearc, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBArc   arc;
    DBSeg   segmnt[4];

/*
***Transformera till basic.
*/
    if ( lsyspk != 0 )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Skapa gm-data.
*/
    if ( GE320(p1,p2,r,lsyspk,&arc,segmnt,modtyp) < 0 )
                return(erpush("EX1342",""));
/*
***Lagra i gm och rita.
*/
    return ( EXearc(id,&arc,segmnt,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXar3p(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       V2NAPA   *pnp)

/*      Skapa cirkelbåge genom 3 punkter. Lagra i GM
 *      och rita.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          p1     => Pekare till startpunkt.
 *          p2     => Pekare till mittpunkt.
 *          p3     => Pekare till slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1342 = Kan ej beräkna cirkel-data
 *                EX1352 = Kan ej lagra cirkelbåge i GM.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *	20/11/85 Anrop till EXearc, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBArc   arc;
    DBSeg   segmnt[4];

/*
***Transformera till basic.
*/
    if ( lsyspk != 0 )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      }
/*
***Skapa gm-data.
*/
    if ( GE310(p1,p2,p3,lsyspk,&arc,segmnt,modtyp) < 0 )
                return(erpush("EX1342",""));
/*
***Lagra i gm och rita.
*/
    return ( EXearc(id,&arc,segmnt,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXarof(
       DBId    *id,
       DBId    *refid,
       DBfloat  dist,
       V2NAPA  *pnp)

/*      Skapar ARC_OFFS.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          refid  => Pekare till id för refererad cirkel.
 *          dist   => Avstånd.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1342 = Kan ej beräkna cirkel-data
 *                EX1352 = Kan ej lagra cirkelbåge i GM.
 *                EX1402 = Den refererade storheten finns ej
 *                EX1412 = Den refererade storheten har fel typ
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *	20/11/85 Anrop till EXearc, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBArc   arc,oldarc;
    DBSeg   segmnt[4],oldseg[4];

/*
***Hämta la för den refererade cirkeln.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***Läs cirkeln.
*/
    DBread_arc(&oldarc,oldseg,la);
/*
***Skapa segment.
*/
    if ( GE306(&oldarc,oldseg,dist,lsyspk,&arc,segmnt) < 0 ) 
                return(erpush("EX1342",""));
/*
***Lagra i gm och rita.
*/
    return ( EXearc(id,&arc,segmnt,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXarfl(
       DBId    *id,
       DBId    *refid1,
       DBId    *refid2,
       DBfloat  r,
       DBshort  alt,
       V2NAPA  *pnp)

/*      Skapar ARC_FIL.
 *
 *      In: id     => Pekare till cirkelns identitet.
 *          refid1 => Pekare till id för 1:a refererade storhet
 *          refid2 => Pekare till id för 2:a refererade storhet
 *          r      => Radie
 *          alt    => Alternativ
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1342 = Kan ej beräkna cirkel-data
 *                EX1352 = Kan ej lagra cirkelbåge i GM.
 *                EX1402 = Den refererade storheten finns ej
 *                EX1412 = Den refererade storheten har fel typ
 *                EX1722 = Radie <= 0.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *	20/11/85 Anrop till EXearc, B. Doverud
 *      30/12/85 Trim, J. Kjellander
 *      22/10/86 EX1722, J. Kjellander
 *      30/10/86 v3dbuf, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      7/6/93   Dynamiska segment, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la1,la2;
    DBetype   typ1,typ2;
    DBAny  ref1,ref2;
    DBArc   arc;
    DBSeg   arcseg[4];
    DBSeg  *spek1,*spek2;
    DBSeg   aseg1[4],aseg2[4];

/*
***Kolla att radien > 0.
*/
    if ( r <= 0.0 ) return(erpush("EX1722",""));
/*
***Hämta la för den 1:a refererade storheten.
*/
    if ( DBget_pointer('I',refid1,&la1,&typ1) < 0 )
         return(erpush("EX1402",""));
/*
***Testa typ och hämta data för första storheten ur GM.
*/
    switch (typ1)
      {
      case LINTYP:
      spek1 = NULL;
      DBread_line(&ref1.lin_un,la1);
      break;

      case ARCTYP:
      spek1 = aseg1;
      DBread_arc(&ref1.arc_un,spek1,la1);
      break;

      case CURTYP:
      DBread_curve(&ref1.cur_un,NULL,&spek1,la1);
      break;

      default:
      return(erpush("EX1412",""));
      }
/*
***Hämta la för den 2:a refererade storheten.
*/
    if ( DBget_pointer('I',refid2,&la2,&typ2) < 0 )
         return(erpush("EX1402",""));
/*
***Testa typ och hämta data för andra storheten ur GM.
*/
    switch (typ2)
      {
      case LINTYP:
      spek2 = NULL;
      DBread_line(&ref2.lin_un,la2);
      break;

      case ARCTYP:
      spek2 = aseg2;
      DBread_arc(&ref2.arc_un,spek2,la2);
      break;

      case CURTYP:
      DBread_curve(&ref2.cur_un,NULL,&spek2,la2);
      break;

      default:
      if ( typ1 == CURTYP ) DBfree_segments(spek1);
      return(erpush("EX1412",""));
      }
/*
***Skapa segment.
*/
    status = GE307(&ref1,spek1,&ref2,spek2,r,alt,lsyspk,&arc,arcseg,modtyp);
/*
***Oavsett hur det gick kan vi nu släppa ev. seg-minne.
*/
    if ( typ1 == CURTYP ) DBfree_segments(spek1);
    if ( typ2 == CURTYP ) DBfree_segments(spek2);
/*
***Gick det bra att skapa segment ?
*/
    if ( status < 0 ) return(erpush("EX1342",""));
/*
***Lagra i gm och rita.
*/
    return(EXearc(id,&arc,arcseg,pnp));
  }

/********************************************************/
