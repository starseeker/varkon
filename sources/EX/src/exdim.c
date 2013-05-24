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

       short   EXeldm(
       DBId   *id,
       DBLdim *ldmptr,
       V2NAPA *pnp)

/*      Create linear dimension, store in DB and display.
 *
 *      In: id     => C ptr to LDIM ID.
 *          ldmptr => C ptr to LDIM data.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1442 = Can't insert LDIM in DB.
 *
 *      (C)microform ab 15/11 B.Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attributes.
*/
    ldmptr->hed_ld.blank = pnp->blank;
    ldmptr->hed_ld.pen   = pnp->pen;
    ldmptr->hed_ld.level = pnp->level;
    ldmptr->asiz_ld      = pnp->dasize;
    ldmptr->tsiz_ld      = pnp->dtsize;
    ldmptr->ndig_ld      = pnp->dndig;
    ldmptr->auto_ld      = pnp->dauto;
    ldmptr->wdt_ld       = pnp->width;
    ldmptr->pcsy_ld      = lsysla;
/*
***Insert in DB.
*/
    if ( pnp->save )
      {
      ldmptr->hed_ld.hit = pnp->hit;
      if ( DBinsert_ldim(ldmptr,id,&la) < 0 )
           return(erpush("EX1442",""));
      }
    else
      {
      ldmptr->hed_ld.hit = 0;
      }
/*
***Display.
*/
    if ( ldmptr->pcsy_ld > 0 ) DBread_csys(&csy,NULL,ldmptr->pcsy_ld);
    WPdrdm((DBAny *)ldmptr,&csy,la,GWIN_ALL);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXldim(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Create linear dimension (LDIM).
 *
 *      In: id     => C ptr to LDIM ID.
 *          p1     => C ptr to start position.
 *          p2     => C ptr to end position.
 *          p3     => C ptr to text position.
 *          alt    => Orientation, 0,1 or 2.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1442 = Cant insert LDIM in DB.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J. Kjellander
 *      20/11/85   Anrop till EXeldm, B. Doverud
 *      27/12/86   hit, J. Kjellander
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBLdim   ldim;

 /*
***Varkon pre SVN#27 saved LDIM DB-geometry in BASIC.
***From SVN#27 LDIM data is saved in LOCAL coordinates.
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
***Save in DB and display.
*/
    return(EXeldm(id,&ldim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXecdm(
       DBId   *id,
       DBCdim *cdmptr,
       V2NAPA *pnp)

/*      Create circular dimension, store in DB and display.
 *
 *      In: id     => C ptr to CDIM ID.
 *          cdmptr => C ptr to CDIM data.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1452 = Can't insert CDIM in DB.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attributes.
*/
    cdmptr->hed_cd.blank = pnp->blank;
    cdmptr->hed_cd.pen   = pnp->pen;
    cdmptr->hed_cd.level = pnp->level;
    cdmptr->asiz_cd      = pnp->dasize;
    cdmptr->tsiz_cd      = pnp->dtsize;
    cdmptr->ndig_cd      = pnp->dndig;
    cdmptr->auto_cd      = pnp->dauto;
    cdmptr->wdt_cd       = pnp->width;
    cdmptr->pcsy_cd      = lsysla;
/*
***Insert into DB.
*/
    if ( pnp->save )
      {
      cdmptr->hed_cd.hit = pnp->hit;
      if ( DBinsert_cdim(cdmptr,id,&la) < 0 )
             return(erpush("EX1452",""));
      }
    else
      {
      cdmptr->hed_cd.hit = 0;
      }
/*
***Display.
*/
    if ( cdmptr->pcsy_cd > 0 ) DBread_csys(&csy,NULL,cdmptr->pcsy_cd);
    WPdrdm((DBAny *)cdmptr,&csy,la,GWIN_ALL);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXcdim(
       DBId     *id,
       DBId     *refid,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Create circular dimension (CDIM).
 *
 *      In: id     => C ptr to CDIM ID.
 *          refid  => C ptr to ARC ID.
 *          pos    => C ptr to text position.
 *          alt    => Orientation, 0,1 or 2.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1402 = Arc does not exist in DB
 *              EX1412 = refid is not an arc
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85    Nya felkoder, R. Svedin
 *      14/10/85   Headerdata, J. Kjellander
 *      14/10/85   Uppdatering av referensräknare, J. Kjellander
 *      20/11/85   Anrop till EXecdm, B. Doverud
 *      22/10/86   Ingen test av 3D/2D typ, R. Svedin
 *      27/12/86   hit, J. Kjellander
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBCdim  cdim;

 /*
***Varkon pre SVN#28 saved CDIM DB-geometry in BASIC.
***From SVN#28 CDIM data is saved in LOCAL coordinates.
*/

/*
***Get the arc DBptr.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***Get arc data from DB.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Calculate CDIM data (3 pos). Note that DBArc
***centre pos x_a and y_a are BASIC coordinates
***and can not be used in 3D since Z is missing.
*/
    GE821(&oldarc,seg,pos,alt,lsyspk,&cdim);
/*
***Store in DB and display.
*/
    return(EXecdm(id,&cdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXerdm(
       DBId   *id,
       DBRdim *rdmptr,
       V2NAPA *pnp)

/*      Create radius dimension, store in DB and display.
 *
 *      In: id     => C ptr to RDIM ID.
 *          rdmptr => C ptr to RDIM data.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1462 = Can't store RDIM in DB.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      2007-09-23 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attributes.
*/
    rdmptr->hed_rd.blank = pnp->blank;
    rdmptr->hed_rd.pen   = pnp->pen;
    rdmptr->hed_rd.level = pnp->level;
    rdmptr->asiz_rd      = pnp->dasize;
    rdmptr->tsiz_rd      = pnp->dtsize;
    rdmptr->ndig_rd      = pnp->dndig;
    rdmptr->auto_rd      = pnp->dauto;
    rdmptr->wdt_rd       = pnp->width;
    rdmptr->pcsy_rd      = lsysla;
/*
***Insert into DB.
*/
    if ( pnp->save )
      {
      rdmptr->hed_rd.hit = pnp->hit;
      if ( DBinsert_rdim(rdmptr,id,&la) < 0 )
           return(erpush("EX1462",""));
      }
    else
      {
      rdmptr->hed_rd.hit = 0;
      }
/*
***Display.
*/
    if ( rdmptr->pcsy_rd > 0 ) DBread_csys(&csy,NULL,rdmptr->pcsy_rd);
    WPdrdm((DBAny *)rdmptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXrdim(
       DBId     *id,
       DBId     *refid,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Create radius dimension (RDIM).
 *
 *      In: id     => C ptr to RDIM ID.
 *          refid  => C ptr to ARC ID.
 *          p1     => C ptr to break position.
 *          p2     => C ptr to end position.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1402 = Arc does not exist in DB
 *              EX1412 = refid is not an arc
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
    DBetype typ;
    DBArc   oldarc;
    DBSeg   seg[4];
    DBRdim  rdim;

 /*
***Varkon pre SVN#30 saved RDIM DB-geometry in BASIC.
***From SVN#30 RDIM data is saved in LOCAL coordinates.
*/
/*
***Transformera till basic.
*
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
*
***Get the arc DBptr.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",""));

    if ( typ != ARCTYP )
         return(erpush("EX1412",""));
/*
***Get arc data from DB.
*/
    DBread_arc(&oldarc,seg,la);
/*
***Calculate RDIM data (3 pos).
*/
    GE822(&oldarc,seg,p1,p2,lsyspk,&rdim);
/*
***Store in DB and display.
*/
    return(EXerdm(id,&rdim,pnp));
  }

/********************************************************/
/*!******************************************************/

       short   EXeadm(
       DBId   *id,
       DBAdim *admptr,
       V2NAPA *pnp)

/*      Create angular dimension, store in DB and display.
 *
 *      In: id     => C ptr to ADIM ID.
 *          admptr => C ptr to ADIM data.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1462 = Can't store ADIM in DB.
 *
 *      (C)microform ab  15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBCsys csy;

/*
***Add attributes.
*/
    admptr->hed_ad.blank = pnp->blank;
    admptr->hed_ad.pen   = pnp->pen;
    admptr->hed_ad.level = pnp->level;
    admptr->asiz_ad      = pnp->dasize;
    admptr->tsiz_ad      = pnp->dtsize;
    admptr->ndig_ad      = pnp->dndig;
    admptr->auto_ad      = pnp->dauto;
    admptr->wdt_ad       = pnp->width;
    admptr->pcsy_ad      = lsysla;
/*
***Insert into DB.
*/
    if ( pnp->save )
      {
      admptr->hed_ad.hit = pnp->hit;
      if ( DBinsert_adim(admptr,id,&la) < 0 )
           return(erpush("EX1472",""));
      }
    else
      {
      admptr->hed_ad.hit = 0;
      }
/*
***Display.
*/
    if ( admptr->pcsy_ad > 0 ) DBread_csys(&csy,NULL,admptr->pcsy_ad);
    WPdrdm((DBAny *)admptr,&csy,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short     EXadim(
       DBId     *id,
       DBId     *refid1,
       DBId     *refid2,
       DBVector *pos,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Create angular dimension (ADIM).
 *
 *      In: id     => C ptr to ADIM ID.
 *          refid  => C ptr to line 1 ID.
 *          refid  => C ptr to line 2 ID.
 *          pos    => C ptr to text position.
 *          alt    => Alternative, + or - 1,2,3 eller 4.
 *          pnp    => C ptr to attributes.
 *
 *      Return:      0 = Ok.
 *              EX1402 = Can't find entity in DB.
 *              EX1412 = Illegal entity type.
 *              EX1532 = Cant calculate ADIM data.
 *
 *      (C)microform ab  4/8/85 J. Kjellander
 *
 *      10/9/85  Nya felkoder, R. Svedin
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Uppdatering av referensräknare, J. Kjellander
 *      20/11/85 Anrop till EXeadm, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *      2007-09-24 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;
    DBLine   lin1,lin2;
    DBAdim   adim;

 /*
***Varkon pre SVN#30 saved ADIM DB-geometry in BASIC.
***From SVN#30 ADIM data is saved in LOCAL coordinates.
*
***Transformera till basic.
*
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
*
***Get DBptr for first line.
*/
    if ( DBget_pointer('I',refid1,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***Get line data.
*/
    DBread_line(&lin1,la);
/*
***Get DBptr for second line.
*/
    if ( DBget_pointer('I',refid2,&la,&typ) < 0 )
         return(erpush("EX1402",""));
    if ( typ != LINTYP )
         return(erpush("EX1412",""));
/*
***Get line data.
*/
    DBread_line(&lin2,la);
/*
***Calculate ADIM data.
*/
    if ( GE823(&lin1,&lin2,pos,alt,lsyspk,&adim) < 0 )
         return(erpush("EX1532",""));
/*
***Store in DB and display.
*/
    return(EXeadm(id,&adim,pnp));
  }

/********************************************************/
