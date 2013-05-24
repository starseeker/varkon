/*!*****************************************************
*
*    exgatt.c
*    ========
*
*    EXgatt();     Get the attributes of an entity
*    EXgala();     Same by DBptr
*    EXrist();     Reinterpret statement
*
*    This file is part of the VARKON Execute  Library.
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

extern DBptr  lsysla;
extern V2NAPA defnap;

/*!******************************************************/

        short EXgatt(
        DBId    *id,
        V2NAPA  *pnp,
        DBptr   *pcsypk)

/*      Läser en storhets attribut och koordinatsystem.
 *
 *      In: id     => Storhetens identitet.
 *          pnp    => Pekare till namnparameterblock.
 *          pcsypk => Pekare till koordinatsystem-pekare.
 *
 *      Ut: *pnp    => Berörda attribut modifierade.
 *          *pcsypk => Storhetens koordinatsystems LA.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 25/2/93 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;

/*
***Hämta storhetens la och typ.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
      return(erpush("EX1402",""));
/*
***Hämta attribut för storhet vid LA.
*/
    return(EXgala(la,typ,pnp,pcsypk));
  }

/********************************************************/
/*!******************************************************/

        short EXgala(
        DBptr    la,
        DBetype  typ,
        V2NAPA  *pnp,
        DBptr   *pcsypk)

/*      Läser en storhets attribut och koordinatsystem.
 *
 *      In: la     => Storhetens adress i GM.
 *          typ    => Typ av storhet.
 *          pnp    => Pekare till namnparameterblock.
 *          pcsypk => Pekare till koordinatsystem-pekare.
 *
 *      Ut: *pnp    => Berörda attribut modifierade.
 *          *pcsypk => Storhetens koordinatsystems LA.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 15/3/92 J. Kjellander
 *
 *      27/11/92 Ytor, J. Kjellander
 *      25/2/93  EXgatt()/EXgala(), J. Kjellander
 *      4/11/94  Lagt till CSYTYP, J. Kjellander
 *      1998-01-08 WIDTH, J.Kjellander
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBAny  gmpost;

/*
***Först typ-specifika attribut.
*/
    switch(typ)
      {
      case POITYP:
      DBread_point(&gmpost.poi_un,la);
      pnp->width  = gmpost.poi_un.wdt_p;
      pnp->pfont  = gmpost.poi_un.fnt_p;
      pnp->psize  = gmpost.poi_un.size_p;
      *pcsypk     = gmpost.poi_un.pcsy_p;
      break;

      case LINTYP:
      DBread_line(&gmpost.lin_un,la);
      pnp->lfont  = gmpost.lin_un.fnt_l;
      pnp->ldashl = gmpost.lin_un.lgt_l;
      pnp->width  = gmpost.lin_un.wdt_l;
      *pcsypk     = gmpost.lin_un.pcsy_l;
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,NULL,la);
      pnp->afont  = gmpost.arc_un.fnt_a;
      pnp->adashl = gmpost.arc_un.lgt_a;
      pnp->width  = gmpost.arc_un.wdt_a;
      *pcsypk     = gmpost.arc_un.pcsy_a;
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,NULL,la);
      pnp->cfont  = gmpost.cur_un.fnt_cu;
      pnp->cdashl = gmpost.cur_un.lgt_cu;
      pnp->width  = gmpost.cur_un.wdt_cu;
      *pcsypk     = gmpost.cur_un.pcsy_cu;
      break;

      case SURTYP:
      DBread_surface(&gmpost.sur_un,la);
      pnp->sfont   = gmpost.sur_un.fnt_su;
      pnp->sdashl  = gmpost.sur_un.lgt_su;
      pnp->nulines = gmpost.sur_un.ngu_su;
      pnp->nvlines = gmpost.sur_un.ngv_su;
      pnp->width   = gmpost.sur_un.wdt_su;
      *pcsypk      = gmpost.sur_un.pcsy_su;
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un,NULL,la);
      *pcsypk     = gmpost.csy_un.pcsy_pl;
      break;

      case BPLTYP:
      DBread_bplane(&gmpost.bpl_un,la);
      pnp->width  = gmpost.bpl_un.wdt_bp;
      *pcsypk     = gmpost.bpl_un.pcsy_bp;
      break;

      case MSHTYP:
      DBread_mesh(&gmpost.msh_un,la,MESH_HEADER);
      pnp->width  = gmpost.msh_un.wdt_m;
      pnp->mfont  = gmpost.msh_un.font_m;
      *pcsypk     = gmpost.msh_un.pcsy_m;
      break;

      case TXTTYP:
      DBread_text(&gmpost.txt_un,NULL,la);
      pnp->tsize  = gmpost.txt_un.h_tx;
      pnp->twidth = gmpost.txt_un.b_tx;
      pnp->tslant = gmpost.txt_un.l_tx;
      pnp->tfont  = gmpost.txt_un.fnt_tx;
      pnp->width  = gmpost.txt_un.wdt_tx;
      *pcsypk     = gmpost.txt_un.pcsy_tx;
      break;

      case XHTTYP:
      DBread_xhatch(&gmpost.xht_un,NULL,NULL,la);
      pnp->xfont  = gmpost.xht_un.fnt_xh;
      pnp->xdashl = gmpost.xht_un.lgt_xh;
      *pcsypk     = gmpost.xht_un.pcsy_xh;
      break;

      case LDMTYP:
      DBread_ldim(&gmpost.ldm_un,NULL,la);
      pnp->dasize = gmpost.ldm_un.asiz_ld;
      pnp->dtsize = gmpost.ldm_un.tsiz_ld;
      pnp->dndig  = gmpost.ldm_un.ndig_ld;
      pnp->dauto  = gmpost.ldm_un.auto_ld;
      *pcsypk     = gmpost.ldm_un.pcsy_ld;
      break;

      case CDMTYP:
      DBread_cdim(&gmpost.cdm_un,NULL,la);
      pnp->dasize = gmpost.cdm_un.asiz_cd;
      pnp->dtsize = gmpost.cdm_un.tsiz_cd;
      pnp->dndig  = gmpost.cdm_un.ndig_cd;
      pnp->dauto  = gmpost.cdm_un.auto_cd;
      *pcsypk     = gmpost.cdm_un.pcsy_cd;
      break;

      case RDMTYP:
      DBread_rdim(&gmpost.rdm_un,NULL,la);
      pnp->dasize = gmpost.rdm_un.asiz_rd;
      pnp->dtsize = gmpost.rdm_un.tsiz_rd;
      pnp->dndig  = gmpost.rdm_un.ndig_rd;
      pnp->dauto  = gmpost.rdm_un.auto_rd;
      *pcsypk     = gmpost.rdm_un.pcsy_rd;
      break;

      case ADMTYP:
      DBread_adim(&gmpost.adm_un,NULL,la);
      pnp->dasize = gmpost.adm_un.asiz_ad;
      pnp->dtsize = gmpost.adm_un.tsiz_ad;
      pnp->dndig  = gmpost.adm_un.ndig_ad;
      pnp->dauto  = gmpost.adm_un.auto_ad;
      *pcsypk     = gmpost.adm_un.pcsy_ad;
      break;
/*
***Övriga storheter, tex. en part eller grupp har
***bara en header.
*/
      default:
      DBread_header(&gmpost.hed_un,la);
      *pcsypk = DBNULL;
      break;
      }
/*
***Alla storheter har dessutom attributen level,pen,hit och blank.
*/
    pnp->level = gmpost.hed_un.level;
    pnp->blank = gmpost.hed_un.blank;
    pnp->hit   = gmpost.hed_un.hit;
    pnp->pen   = gmpost.hed_un.pen;
/*
***Eftersom storheten finns i GM måste alltså även SAVE
***sättas på. Detta kan ha betydelse tex. vid reinterpretering
***med MACRO.
*/
    pnp->save = 1;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXrist(DBId *id)

/*      Reinterpreterar en Part/Geometri-sats.
 *
 *      In: id = Storhetens identitet.
 *
 *      Ut: Inget.
 *
 *      FV:   0 = OK
 *          < 0 = Fel vid interpretering.
 *
 *      (C)microform ab 10/5/92 J. Kjellander
 *
 *      7/12/94  Ändra aktivt koordinatsystem, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBetype    typ;
    DBptr    csyla,tmpams,old_la,new_la;
    pm_ptr   slstla,lstla,nextla,statla;
    PMMONO  *np;
    V2NAPA   oldnap;

/*
***Hämta storhetens la före ändringen. Detta behövs
***senare för att kolla om det är aktivt koordinatsystem
***som har ändrats.
*/
    if ( DBget_pointer('I',id,&old_la,&typ) < 0 )
      return(erpush("EX1402",""));
/*
***Ta reda på de attribut och det koordinatsystem
***som var aktivt när storheten skapades.
*/
    V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
    status = EXgatt(id,&defnap,&csyla);
    if ( status < 0 ) goto exit;
/*
***Om csyla =  NULL var inget lokalt system aktivt.
***Om lsysla = NULL är inget lokalt system aktivt nu.
***Jämför dom med varandra och aktivera rätt koordinatsystem.
*/
    tmpams = lsysla;
    if ( csyla != lsysla )
      {
      if ( csyla != DBNULL ) EXmlla(csyla);
      else EXmogl();
      }
/*
***Var i PM ligger satsen ? Först en C-pekare till aktiv modul.
***Sen en PM-pekare till list-noden för utpekad sats.
***Sen en PM-pekare till själva satsen.
***lstla = NULL => Satsen finns ej.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0);
    slstla = np->pstl_;
    if ( (status=pmlges(id,&slstla,&lstla)) < 0 ) goto exit;
    if ( lstla == (pm_ptr)NULL ) goto exit;
    if ( (status=pmglin(lstla,&nextla,&statla)) < 0 ) goto exit;
/*
***Oavsett om storheten är refererad eller ej provar
***vi att reinterpretera för att se om det går bra.
***Sudda och länka bort ur GM.
*/
    EXdraw(id,FALSE);
    gmmtm(id[0].seq_val);
/*
***Sen provar vi att re-interpretera.
*/
    status = inssta(statla); 
/*
***Slutligen återställer vi attribut och koordinatsystem
***till ursprungligt skick. Om den ändrade storheten är
***ett koordinatsystem och just aktivt koordinatsystem 
***ligger det inte längre kvar på samma plats i GM. Det
***som ursprungligen var lsysla, dvs. tmpams pekar då
***på det gamla suddade systemet !!! Detta fixar vi här
***genom att jämföra lsysla med storhetens gamla GM-adress
***och om de är lika ändra lsysla till den nya adressen.
*/
    V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

    lsysla = tmpams;
    if ( lsysla == old_la )
      {
      DBget_pointer('I',id,&new_la,&typ);
      lsysla = new_la;
      }
    
    if ( lsysla != csyla )
      {
      if ( lsysla == DBNULL ) EXmogl();
      else EXmlla(lsysla);
      }
/*
***Om det inte gick bra att reinterpretera, länka in den gamla
***storheten i GM igen och rita.
*/
    if ( status < 0 ) 
      {
      gmumtm();
      EXdraw(id,TRUE);
      }
/*
***Slut.
*/
exit:
    return(status);
  }

/********************************************************/
