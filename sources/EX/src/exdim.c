/*!*******************************************************
*
*    exdim.c
*    =======
*
*    EXeldm();     Create linear dimension
*    EXldim();     Create LDIM
*    EXecdm();     Create circular dimension
*    EXcdim();     Create CDIM
*    EXerdm();     Create radius dimension
*    EXrdim();     Create RDIM
*    EXeadm();     Create angular dimension
*    EXadim();     Create ADIM
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

       short EXeldm(
       DBId   *id,
       DBLdim  *ldmpek,
       V2NAPA *pnp)

/*      Skapar längdmått, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          ldmpek => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1442 = Kan ej lagra längdmått i GM.
 *
 *      (C)microform ab 15/11 B.Doverud
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
    ldmpek->hed_ld.blank = pnp->blank;
    ldmpek->hed_ld.pen   = pnp->pen;
    ldmpek->hed_ld.level = pnp->level;
    ldmpek->asiz_ld      = pnp->dasize;
    ldmpek->tsiz_ld      = pnp->dtsize;
    ldmpek->ndig_ld      = pnp->dndig;
    ldmpek->auto_ld      = pnp->dauto;
    ldmpek->pcsy_ld      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      ldmpek->hed_ld.hit = pnp->hit;
      if ( DBinsert_ldim(ldmpek,id,&la) < 0 )
           return(erpush("EX1442",""));
      }
    else
      {
      ldmpek->hed_ld.hit = 0;
      }
/*
***Rita.
*/
    WPdrdm((DBAny *)ldmpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXldim(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar LDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Pekare till måttets startpunkt.
 *          p2     => Pekare till måttets slutpunkt.
 *          p3     => Pekare till textens läge.
 *          alt    => Alternativ, 0,1 eller 2.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1442 = Kan ej lagra längdmått i GM.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXeldm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBLdim   ldim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      }
/*
***Skapa måttet.
*/
    ldim.p1_ld.x_gm = p1->x_gm;
    ldim.p1_ld.y_gm = p1->y_gm;
    ldim.p1_ld.z_gm = 0.0;

    ldim.p2_ld.x_gm = p2->x_gm;
    ldim.p2_ld.y_gm = p2->y_gm;
    ldim.p2_ld.z_gm = 0.0;

    ldim.p3_ld.x_gm = p3->x_gm;
    ldim.p3_ld.y_gm = p3->y_gm;
    ldim.p3_ld.z_gm = 0.0;

    ldim.dtyp_ld = alt;
/*
***Lagra i gm och rita.
*/
    return ( EXeldm(id,&ldim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXecdm(
       DBId   *id,
       DBCdim  *cdmpek,
       V2NAPA *pnp)

/*      Skapar diametermått, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          cdmpek => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1452 = Kan ej lagra diametermått i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
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
    cdmpek->hed_cd.blank = pnp->blank;
    cdmpek->hed_cd.pen   = pnp->pen;
    cdmpek->hed_cd.level = pnp->level;
    cdmpek->asiz_cd      = pnp->dasize;
    cdmpek->tsiz_cd      = pnp->dtsize;
    cdmpek->ndig_cd      = pnp->dndig;
    cdmpek->auto_cd      = pnp->dauto;
    cdmpek->pcsy_cd      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      cdmpek->hed_cd.hit = pnp->hit;
      if ( DBinsert_cdim(cdmpek,id,&la) < 0 )
             return(erpush("EX1452",""));
      }
    else
      {
      cdmpek->hed_cd.hit = 0;
      }
/*
***Rita.
*/
    WPdrdm((DBAny *)cdmpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXcdim(
       DBId     *id,
       DBId     *refid,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar CDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id för refererad cirkel.
 *          pos    => Pekare till textens läge.
 *          alt    => Alternativ, 0,1 eller 2.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX 1412 = Otillåten geometri-typ för denna operation
 *              EX1452 = Kan ej lagra diametermått i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensräknare, J. Kjellander
 *      20/11/85 Anrop till EXecdm, B. Doverud
 *      22/10/86 Ingen test av 3D/2D typ, R. Svedin
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBCdim   cdim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Hämta la för den refererade cirkeln.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***Läs cirkeldata.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Beräkna mått-data.
*/
    GE821(&oldarc,pos,alt,&cdim);
/*
***Lagra i gm och rita.
*/
    return(EXecdm(id,&cdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXerdm(
       DBId   *id,
       DBRdim  *rdmpek,
       V2NAPA *pnp)

/*      Skapar radiemått, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          rdmpek => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1462 = Kan ej lagra radiemått i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
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
    rdmpek->hed_rd.blank = pnp->blank;
    rdmpek->hed_rd.pen   = pnp->pen;
    rdmpek->hed_rd.level = pnp->level;
    rdmpek->asiz_rd      = pnp->dasize;
    rdmpek->tsiz_rd      = pnp->dtsize;
    rdmpek->ndig_rd      = pnp->dndig;
    rdmpek->auto_rd      = pnp->dauto;
    rdmpek->pcsy_rd      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      rdmpek->hed_rd.hit = pnp->hit;
      if ( DBinsert_rdim(rdmpek,id,&la) < 0 )
           return(erpush("EX1462",""));
      }
    else
      {
      rdmpek->hed_rd.hit = 0;
      }
/*
***Rita.
*/
    WPdrdm((DBAny *)rdmpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXrdim(
       DBId     *id,
       DBId     *refid,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Skapar RDIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id för refererad cirkel.
 *          p1     => Pekare till måttets brytpunkt.
 *          p2     => Pekare till måttets slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX1462 = Kan ej lagra radiemått i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensräknare, J. Kjellander
 *      20/11/85 Anrop til EXerdm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *      29/4/87  3D-cirkel, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBRdim   rdim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Hämta la för den refererade cirkeln.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***Läs cirkeldata.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Beräkna mått-data.
*/
    GE822(&oldarc,p1,p2,&rdim);
/*
***Lagra i gm och rita.
*/
    return(EXerdm(id,&rdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXeadm(
       DBId   *id,
       DBAdim  *admpek,
       V2NAPA *pnp)

/*      Skapar vinkelmått, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          admpek => Pekare till GM-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1472 = Kan ej lagra vinkelmått i GM.
 *
 *      (C)microform ab  15/11/85 B. Doverud
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
    admpek->hed_ad.blank = pnp->blank;
    admpek->hed_ad.pen   = pnp->pen;
    admpek->hed_ad.level = pnp->level;
    admpek->asiz_ad      = pnp->dasize;
    admpek->tsiz_ad      = pnp->dtsize;
    admpek->ndig_ad      = pnp->dndig;
    admpek->auto_ad      = pnp->dauto;
    admpek->pcsy_ad      = lsysla;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      admpek->hed_ad.hit = pnp->hit;
      if ( DBinsert_adim(admpek,id,&la) < 0 )
           return(erpush("EX1472",""));
      }
    else
      {
      admpek->hed_ad.hit = 0;
      }
/*
***Rita.
*/
    WPdrdm((DBAny *)admpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXadim(
       DBId     *id,
       DBId     *refid1,
       DBId     *refid2,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar ADIM, lagrar i GM och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till id för refererad linje-1.
 *          refid  => Pekare till id för refererad linje-2.
 *          pos    => Pekare till textens läge.
 *          alt    => Alternativ, + eller - 1,2,3 eller 4.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Den refererade storhten finns ej i GM
 *              EX1412 = Otillåten geometri-typ för denna operation
 *              EX1532 = Kan ej beräkna mått-data
 *              EX1472 = Kan ej lagra vinkelmått i GM.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensräknare, J. Kjellander
 *      20/11/85 Anrop till EXeadm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBLine   lin1,lin2;
    DBAdim   adim;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Hämta la för den 1:a refererade linjen.
*/
    if ( DBget_pointer('I',refid1,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***Läs linjedata.
*/
    DBread_line(&lin1,la);
/*
***Hämta la för den 2:a refererade linjen.
*/
    if ( DBget_pointer('I',refid2,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***Läs linjedata.
*/
    DBread_line(&lin2,la);
/*
***Beräkna mått-data.
*/
    if ( GE823(&lin1,&lin2,pos,alt,&adim) < 0 )
         return(erpush("EX1532",""));
/*
***Lagra i gm och rita.
*/
    return(EXeadm(id,&adim,pnp));
  }
/********************************************************/
