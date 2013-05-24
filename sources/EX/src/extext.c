/*!*******************************************************
*
*    extext.c
*    ========
*
*    EXetxt();    Create text
*    EXtext();    Create TEXT
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern DBptr   lsysla;
extern DBTmat  lklsyi;
extern DBTmat *lsyspk;

/*!******************************************************/

       short EXetxt(
       DBId   *id,
       DBText  *txtpek,
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
    WPdrtx(txtpek,strpek,la,GWIN_ALL);

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
    DBfloat   vridnt;
    DBText   txt;

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
