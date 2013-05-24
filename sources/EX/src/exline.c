/*!*******************************************************
*
*    exline.c
*    ========
*
*    EXelin();     Create line
*    EXlifr();     Create LIN_FREE
*    EXlipr();     Create LIN_PROJ
*    EXliof();     Create LIN_OFFS
*    EXlipv();     Create LIN_ANG
*    EXlipt();     Create LIN_TAN1
*    EXli2t();     Create LIN_TAN2
*    EXlipe();     Create LIN_PERP
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

extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern DBptr   lsysla;

/*!******************************************************/

       short EXelin(
       DBId   *id,
       DBLine *linpek,
       V2NAPA *pnp)

/*      Create line. Generic for all line methods.
 *
 *      In: id     => C-ptr to ID.
 *          linpek => C-ptr to DBLine.
 *          pnp    => C-ptr to attributes.
 *
 *      Return:  0 = Ok.
 *          EX1332 = Can't store line in DB.
 *
 *      (C)microform ab 14/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      2006-12-09 Removed gpxxx(), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Current line attributes.
*/
    linpek->hed_l.blank = pnp->blank;
    linpek->hed_l.pen   = pnp->pen;
    linpek->hed_l.level = pnp->level;
    linpek->fnt_l       = pnp->lfont;
    linpek->lgt_l       = pnp->ldashl;
    linpek->wdt_l       = pnp->width;
    linpek->pcsy_l      = lsysla;
/*
***Save in DB.
*/
    if ( pnp->save )
      {
      linpek->hed_l.hit = pnp->hit;
      if ( DBinsert_line(linpek,id,&la) < 0 ) return(erpush("EX1332",""));
      }
    else
      {
      linpek->hed_l.hit = 0;
      }
/*
***Display line in all graphical windows.
*/
    WPdrli(linpek,la,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXlifr(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Skapar LIN_FREE.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Startpunkt.
 *          p2     => Slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine   lin;

    static char lifr[] = "LIN_FREE";

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Beräkna linjens koordinater.
*/
    if ( GE200( p1, p2, &lin) < 0 )
         return(erpush("EX1322",lifr));
/*
***Lagra i gm och rita.
*/
    return ( EXelin(id,&lin,pnp)); 
  }

/********************************************************/
/*!******************************************************/

       short EXlipr(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Skapar LIN_PROJ.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Startpunkt.
 *          p2     => Slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine   lin;

    static char lipr[] = "LIN_PROJ";

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Beräkna linjens koordinater.
*/
    if ( GE201(p1,p2,lsyspk,&lin) < 0 )
         return(erpush("EX1322",lipr));
/*
***Lagra i gm och rita.
*/
    return ( EXelin(id,&lin,pnp)); 
  }

/********************************************************/
/*!******************************************************/

       short EXliof(
       DBId   *id,
       DBId   *refid,
       DBfloat offset,
       V2NAPA *pnp)

/*      Skapar LIN_OFFS.
 *
 *      In: id     => Pekare till identitet.
 *          refid  => Pekare till refererad identitet.
 *          offset => Offset-värde.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *                EX1402 = Linjen finns ej
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBLine   lin;
    DBLine   oldlin;

    static char liof[] = "LIN_OFFS";

/*
***Hämta la för den refererade linjen.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",liof));
    if ( typ != LINTYP )
         return(erpush("EX1412",liof));
/*
***Läs gamla linjen.
*/
    DBread_line(&oldlin,la);
/*
***Beräkna nya linjens koordinater.
*/
    if ( GE202(&oldlin, (DBfloat)offset, lsyspk, &lin) < 0 )
         return(erpush("EX1322",liof));
/*
***Lagra i gm och rita.
*/
    return ( EXelin(id,&lin,pnp)); 
  }

/********************************************************/
/*!******************************************************/

       short EXlipv(
       DBId     *id,
       DBVector *pos,
       DBfloat   ang,
       DBfloat   length,
       V2NAPA   *pnp)

/*      Skapar LIN_ANG.
 *
 *      In: id     => Pekare till identitet.
 *          pos    => Linjens startposition
 *          ang    => Vinkel mot x-axeln
 *          length => Linjens längd
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      27/12/86 hit, J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine   lin;

    static char lipv[] = "LIN_ANG";

/*
***Transformera positionen till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Beräkna linjens koordinater.
*/
    if ( GE204( pos, lsyspk , ang, length, &lin) < 0 )
         return(erpush("EX1322",lipv));
/*
***Lagra i gm och rita.
*/
    return ( EXelin(id,&lin,pnp)); 
  }

/********************************************************/
/*!******************************************************/

       short EXlipt(
       DBId     *id,
       DBVector *pos,
       DBId     *refid,
       DBshort   alt,
       V2NAPA   *pnp)

/*      Skapar LIN_TAN1.
 *
 *      In: id     => Pekare till identitet.
 *          pos    => Linjens startpunkt.
 *          refid  => Pekare till refererad identitet.
 *          alt    => Vilken tangeringspunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *                EX1402 = Storheten finns ej.
 *                EX1412 = Otillåten typ av storhet.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      14/10/85 Composite, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      30/10/86 v3dbuf, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      7/6/93   Dynamiska segment, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    DBLine   lin;
    DBAny  gmpost;
    DBSeg  *segpek,arcseg[4];

    static char lipt[] = "LIN_TAN1";

/*
***Transformera positionen till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Hämta la för den refererade cirkeln eller kurvan.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",lipt));
/*
***Testa typ och hämta data ur GM.
*/
    switch (typ)
      {
      case ARCTYP:
      segpek = arcseg;
      DBread_arc(&gmpost.arc_un,segpek,la);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      break;

      default:
      return(erpush("EX1412",lipt));
      }
/*
***Beräkna linjens koordinater.
*/
    if ( GE205( &gmpost,segpek,pos,lsyspk,alt,&lin) < 0 )
      {
      status = erpush("EX1322",lipt);
      goto end;
      }
/*
***Lagra i gm och rita.
*/
    status = EXelin(id,&lin,pnp); 
/*
***Slut.
*/
end:
    if ( typ == CURTYP ) DBfree_segments(segpek);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXli2t(
       DBId   *id,
       DBId   *refid1,
       DBId   *refid2,
       DBshort alt,
       V2NAPA *pnp)

/*      Skapar LIN_TAN2.
 *
 *      In: id     => Pekare till identitet.
 *          refid1 => Pekare till refererad identitet 1.
 *          refid2 => Pekare till refererad identitet 2.
 *          alt    => Vilken tangeringspunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *                EX1402 = Storheten finns ej.
 *                EX1412 = Otillåten typ av storhet.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXelin, B. Doverud
 *      30/10/86 v3dbuf, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      15/11/98 Coordinate systems to geo206 G. Liden
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBLine   lin;
    DBSeg   aseg1[4],aseg2[4];
    DBArc   arc1,arc2;
    DBArc   *p_arc_1;  /* Circle 1                  (ptr) */
    DBArc   *p_arc_2;  /* Circle 2                  (ptr) */
    DBptr   la_csys_1; /* Coord. system circle 1 (GM ptr) */
    DBptr   la_csys_2; /* Coord. system circle 2 (GM ptr) */
    DBCsys   gm_csys;   /* Not used data                   */
    DBTmat  csys_1;    /* Coord. system circle 1          */
    DBTmat  csys_2;    /* Coord. system circle 2          */
    DBTmat *p_csys_1;  /* Coord. system circle 1    (ptr) */
    DBTmat *p_csys_2;  /* Coord. system circle 2    (ptr) */

    static char li2t[] = "LIN_TAN2";

/*
***Initializations.
 */
    p_csys_1 = NULL;
    p_csys_2 = NULL;
/*
***Hämta la för den 1:a refererade cirkeln.
*/
    if ( DBget_pointer('I',refid1,&la,&typ) < 0 )
         return(erpush("EX1402",li2t));
/*
***Testa typ och hämta data för första cirkeln ur GM.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&arc1,aseg1,la);
      p_arc_1   = &arc1;
      la_csys_1 = p_arc_1->pcsy_a;
      if ( DBNULL == la_csys_1 ) p_csys_1 = NULL;
      else
        {
        DBread_csys(&gm_csys, &csys_1, la_csys_1);
        p_csys_1 = &csys_1;
        }
      break;

      default:
      return(erpush("EX1412",li2t));
      }
/*
***Hämta la för den 2:a refererade cirkeln.
*/
    if ( DBget_pointer('I',refid2,&la,&typ) < 0 )
         return(erpush("EX1402",li2t));
/*
***Testa typ och hämta data för andra cirkeln ur GM.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&arc2,aseg2,la);
      p_arc_2   = &arc2;
      la_csys_2 = p_arc_2->pcsy_a;
      if ( DBNULL == la_csys_2 ) p_csys_2 = NULL;
      else
        {
        DBread_csys(&gm_csys, &csys_2, la_csys_2);
        p_csys_2 = &csys_2;
        }
      break;

      default:
      return(erpush("EX1412",li2t));
      }
/*
***Beräkna linjens koordinater.
*/
 

   if ( GE206(&arc1,aseg1,p_csys_1,&arc2,aseg2,p_csys_2,alt,
                lsyspk,&lin) < 0 ) return(erpush("EX1322",li2t));
/*
***Lagra i gm och rita.
*/
    return(EXelin(id,&lin,pnp)); 
  }

/********************************************************/
/*!******************************************************/

       short EXlipe(
       DBId     *id,
       DBVector *pos,
       DBId     *refid,
       DBfloat   length,
       V2NAPA   *pnp)

/*      Skapar LIN_PERP.
 *
 *      In: id     => Pekare till identitet.
 *          pos    => Linjens startposition
 *          refid  => ID för gammal linje.
 *          length => Linjens längd
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1322 = Kan ej beräkna linje-data.
 *                EX1402 = Linjen finns ej
 *                EX1412 = Refererad storhet ej linje.
 *
 *
 *      (C)microform ab 24/4/87 R. Svedin 
 *
 *      24/4/87  Kodning, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBLine   lin;
    DBLine   oldlin;

    static char lipe[] = "LIN_PERP";

/*
***Hämta la för den refererade linjen.
*/
    if ( DBget_pointer('I',refid,&la,&typ) < 0 )
         return(erpush("EX1402",lipe));
    if ( typ != LINTYP )
         return(erpush("EX1412",lipe));
/*
***Läs gamla linjen.
*/
    DBread_line(&oldlin,la);
/*
***Transformera startpos till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Beräkna nya linjens koordinater.
*/
    if ( GE203(pos,&oldlin,(DBfloat)length,lsyspk,&lin) < 0 )
         return(erpush("EX1322",lipe));
/*
***Lagra i gm och rita.
*/
    return ( EXelin(id,&lin,pnp)); 
  }

/********************************************************/
