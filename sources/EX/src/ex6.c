/*!*******************************************************
*
*    ex6.c
*    =====
*
*    EXeldm();     Create linear dimension
*    EXldim();     Create LDIM
*    EXecdm();     Create circular dimension
*    EXcdim();     Create CDIM
*    EXerdm();     Create radius dimension
*    EXrdim();     Create RDIM
*    EXeadm();     Create angular dimension
*    EXadim();     Create ADIM
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../GP/include/GP.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern short   gptrty;

/*!******************************************************/

       short EXeldm(
       DBId   *id,
       GMLDM  *ldmpek,
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
    gpdrld(ldmpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrdm((GMUNON *)ldmpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrdm((GMUNON *)ldmpek,la,GWIN_ALL);
#endif

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
    GMLDM   ldim;

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
       GMCDM  *cdmpek,
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
    gpdrcd(cdmpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrdm((GMUNON *)cdmpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrdm((GMUNON *)cdmpek,la,GWIN_ALL);
#endif

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
    GMARC   oldarc;
    GMSEG   seg[4];
    GMCDM   cdim;

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
       GMRDM  *rdmpek,
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
    gpdrrd(rdmpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrdm((GMUNON *)rdmpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrdm((GMUNON *)rdmpek,la,GWIN_ALL);
#endif

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
    GMARC   oldarc;
    GMSEG   seg[4];
    GMRDM   rdim;

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
       GMADM  *admpek,
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
    gpdrad(admpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrdm((GMUNON *)admpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrdm((GMUNON *)admpek,la,GWIN_ALL);
#endif

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
    GMLIN   lin1,lin2;
    GMADM   adim;

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
/*!******************************************************/

       short EXexht(
       DBId   *id,
       GMXHT  *xhtpek,
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
    gpdrxh(xhtpek,crdvek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrxh(xhtpek,crdvek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrxh(xhtpek,crdvek,la,GWIN_ALL);
#endif

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
    gmflt   angt;
    gmflt   crdvek[4*GMXMXL];
    GMLIN  *linvek=NULL,*lpvek[GMMXXH];
    GMARC  *arcvek=NULL,*apvek[GMMXXH];
    GMCUR  *curvek=NULL,*cpvek[GMMXXH];
    GMSEG  *spvek[GMMXXH];
    GMXHT   xht;

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
           if ( (linvek=(GMLIN *)v3mall((unsigned)(GMMXXH*sizeof(GMLIN)),
                           "EXxht")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_line(&linvek[nlin],la);
         lpvek[nlin] = &linvek[nlin];
         ++nlin;
         break;

         case (ARCTYP):
         if ( narc == 0 )
           {
           if ( (arcvek=(GMARC *)v3mall((unsigned)(GMMXXH*sizeof(GMARC)),
                           "EXxht")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_arc(&arcvek[narc],NULL,la);
         apvek[narc] = &arcvek[narc];
         ++narc;
         break;

         case (CURTYP):
         if ( ncur == 0 )
           {
           if ((curvek=(GMCUR *)v3mall((unsigned)(GMMXXH*sizeof(GMCUR)),
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
