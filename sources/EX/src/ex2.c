/*!*******************************************************
*
*    ex2.c
*    =====
*
*    EXecsy();    Create coordinatesystem
*    EXcs3p();    Create CSYS_3P
*    EXcs1p();    Create CSYS_1P
*    EXetxt();    Create text
*    EXtext();    Create TEXT
*    EXgrp();     Create GROUP
*
*    This file is part of the VARKON Execute  Library.
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

extern short   gptrty;
extern DBptr   lsysla;
extern DBTmat  lklsyi;
extern DBTmat *lsyspk;

/*!******************************************************/

       short EXecsy(
       DBId    *id,
       GMCSY   *crdpek,
       DBTmat  *pmat,
       V2NAPA  *pnp)

/*      Lagrar i GM och ritar ett koordinatsystem.
 *
 *      In: id     => Pekare till identitet.
 *          crdpek => Pekare till GM-stuktur.
 *          pmat   => Pekare plan-matris.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1392 => Kan ej lagra koordinatsystem
 *
 *      (C)microform ab 15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      1/2/95   Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparameterdata.
*/
    crdpek->hed_pl.blank = pnp->blank;
    crdpek->hed_pl.pen   = pnp->pen;
    crdpek->hed_pl.level = pnp->level;
    crdpek->pcsy_pl      = lsysla;
/*
***Lagra i GM.
*/
    if ( pnp->save )
      {
      crdpek->hed_pl.hit = pnp->hit;
      if ( DBinsert_csys(crdpek,pmat,id,&la) < 0 ) 
              return(erpush("EX1392",""));
      }
    else
      {
      crdpek->hed_pl.hit = 0;
      }
/*
***Rita.
*/
    gpdrcs(crdpek,pmat,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrcs(crdpek,pmat,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrcs(crdpek,pmat,la,GWIN_ALL);
#endif

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXcs3p(
       DBId     *id,
       char     *str,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       V2NAPA   *pnp)

/*      Skapa koordinatsystem med 2 eller 3 positioner.
 *
 *      In: id     => Pekare till identitet.
 *          str    => Pekare till namnsträng.
 *          p1     => Pekare till 1:a punkten.
 *          p2     => Pekare till 2:a punkten.
 *          p3     => Pekare till 3:e punkten eller NULL.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod: EX1382 = Can't calculate csys data
 *
 *      (C)microform ab 9/2/85 J.Kjellander
 *
 *      14/10/85   Header-data. J.Kjellander
 *      20/11/85   Anrop till EXecsy. B.Doverud
 *      27/12/86   hit, J.Kjellander
 *      1996-07-09 Y-axel optionell, J.Kjellander
 *
 ******************************************************!*/

  {
    GMCSY   csy;
    DBTmat  pmat;

/*
***Transformera positionerna till basic. 
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      if ( p3 != NULL ) GEtfpos_to_local(p3,&lklsyi,p3);
      }
/*
***Skapa matrisen.
*/
    if ( GEmktf_2p(p1,p2,p3,&pmat) < 0 ) return(erpush("EX1382",""));
/*
***Fyll i namnet.
*/
    *(str+JNLGTH) = '\0';
    strcpy(csy.name_pl,str);
/*
***Lagra i GM och rita.
*/
    return ( EXecsy(id,&csy,&pmat,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXcs1p(
       DBId     *id,
       char     *str,
       DBVector *p,
       DBfloat   v1,
       DBfloat   v2,
       DBfloat   v3,
       V2NAPA   *pnp)

/*      Skapa koordinatsystem med 1 position och vinklar.
 *
 *      In: id     => Pekare till identitet.
 *          str    => Pekare till namnsträng.
 *          p      => Pekare till origo.
 *          v1     => Vridning runt X.
 *          v2     => Vridning runt Y.
 *          v3     => Vridning runt Z.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1382 => Kan ej beräkna koordinatsystem-data
 *
 *      (C)microform ab 30/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
    GMCSY   csy;
    DBTmat  pmat;

/*
***Skapa matrisen.
*/
    if ( GEmktf_1p(p,v1,v2,v3,lsyspk,&pmat) < 0 )
              return(erpush("EX1382",""));
/*
***Fyll i namnet.
*/
    *(str+JNLGTH) = '\0';
    strcpy(csy.name_pl,str);
/*
***Lagra i GM och rita.
*/
    return ( EXecsy(id,&csy,&pmat,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXetxt(
       DBId   *id,
       GMTXT  *txtpek,
       char    strpek[],
       V2NAPA *pnp)

/*      Lagrar i GM och ritar text.
 *
 *      In: id     => Pekare till identitet.
 *          txtpek => Pekare till GM-struktur.
 *          strpek => Pekare till text-strängen.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1432 => Kan ej lagra text
 *
 *      (C)microform ab 15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      27/12/92 tfont, J. Kjellander
 *      1998-10-01 TPMODE, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparameterdata.
*/
    txtpek->hed_tx.blank = pnp->blank;
    txtpek->hed_tx.pen   = pnp->pen;
    txtpek->hed_tx.level = pnp->level;
    txtpek->h_tx         = pnp->tsize;
    txtpek->b_tx         = pnp->twidth;
    txtpek->l_tx         = pnp->tslant;
    txtpek->fnt_tx       = pnp->tfont;
    txtpek->wdt_tx       = pnp->width;
    txtpek->pmod_tx      = pnp->tpmode;
    txtpek->pcsy_tx      = lsysla;
/*
***Lagra i GM.
*/
    if ( pnp->save )
      {
      txtpek->hed_tx.hit = pnp->hit;
      if ( DBinsert_text(txtpek,strpek,id,&la) < 0 ) 
                 return(erpush("EX1432",""));
      }
    else
      {
      txtpek->hed_tx.hit = 0;
      }
/*
***Rita.
*/
    gpdrtx(txtpek,strpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrtx(txtpek,strpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrtx(txtpek,strpek,la,GWIN_ALL);
#endif

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXtext(
       DBId     *id,
       DBVector *pos,
       DBfloat   vridn,
       char     *strpek,
       V2NAPA   *pnp)

/*      Skapa text.
 *
 *      In: id     => Pekare till identitet.
 *          pos    => Pekare till position.
 *          vridn  => Vridning.
 *          strpek => Pekare till text-strängen.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1432 => Kan ej lagra text
 *
 *      (C)microform ab 15/4/85 J. Kjellander
 *
 *      14/10/85 Header-data. J. Kjellander
 *      20/11/85 Anrop till exetxt B. Doverud
 *      27/12/86 hit, J. Kjellander
 *      1998-10-01 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    gmflt   vridnt;
    GMTXT   txt;

/*
***Spara lokal pos och vridning i text-posten innan
***dom transformeras till basic.
*/
    txt.lpos_tx.x_gm = pos->x_gm;
    txt.lpos_tx.y_gm = pos->y_gm;
    txt.lpos_tx.z_gm = pos->z_gm;
    txt.lang_tx      = vridn;
/*
***Transformera pos till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Transformera vridn till basic.
*/
    GEtfang_to_basic(vridn,lsyspk,&vridnt);
/*
***Övriga post-data.
*/
    txt.crd_tx.x_gm = pos->x_gm;
    txt.crd_tx.y_gm = pos->y_gm;
    txt.crd_tx.z_gm = pos->z_gm;
    txt.v_tx = vridnt;
/*
***Lagra i GM och rita.
*/
    return ( EXetxt (id,&txt,strpek,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXgrp(
       DBId  *id,
       DBId  *ridvek,
       GMGRP *grppek)

/*      Skapa grupp.
 *
 *      In: id     => Pekare till gruppens identitet.
 *          ridvek => Pekare till vektor med referenser.
 *          grppek => Pekare till group-structure.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1482 => Kan ej lagra grupp i GM.
 *          EX1492 => Storheten redan med i max antal grupper.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      14/10/85 Header-data. J. Kjellander
 *
 ******************************************************!*/

  {
    DBint   i;
    DBptr   lavek[GMMXGP];
    DBptr   la;
    DBetype   typ;

/*
***Hämta la för samtliga refererade storheter.
*/
    for ( i=0; i<grppek->nmbr_gp; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&lavek[i],&typ) < 0 ) 
        return(erpush("EX1402",""));
      }
/*
***Lagra grupp-posten i GM.
*/
    if ( DBinsert_group(grppek,lavek,id,&la) < 0 )
      return(erpush("EX1482",""));
/*
***Lagra grupp-pekare i de refererade storheterna.
*/
    for ( i=0; i<grppek->nmbr_gp; ++i )
      {
      if ( gmagpp(lavek[i],la) < 0 ) 
        erpush("EX1492","");
      }
    
    return(0);
  }
  
/********************************************************/
