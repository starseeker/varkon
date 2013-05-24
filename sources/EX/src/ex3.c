/*!*******************************************************
*
*     ex3.c
*     =====
*
*     EXepoi();     Create point
*     EXpofr();     Create POI_FREE
*     EXpopr();     Create POI_PROJ
*
*     EXecur();     Create curve
*     EXcufr();     Create CUR_FREE
*     EXcupr();     Create CUR_PROJ
*     EXcurs();     Create CUR_SPLINE
*     EXcurc();     Create CUR_CONIC
*     EXcuro();     Create CUR_OFFS
*     EXcomp();     Create CUR_COMP
*     EXcurt();     Create CUR_TRIM
*     EXcuru();     Create CUR_USRDEF
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
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif
#include "../include/EX.h"

extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern DBptr   lsysla;
extern short   gptrty,modtyp;

#ifdef WIN32
extern short wpdrpo(),wpdrcu();
#endif

typedef struct enttab
{
DBptr  la;
DBetype  typ;
bool   reversed;
int    ngeoseg;
DBSeg *pgeoseg;
}ENTTAB;

/*  
***Används av EXcomp()
*/
static short  check(DBint ncur, DBCurve *comp_cur, ENTTAB et[]);
static short  reverse(DBCurve *cur,DBSeg *seg);

/*!******************************************************/

       short EXepoi(
       DBId    *id,
       DBPoint   *poipek,
       V2NAPA  *pnp)

/*     Skapar punkt, lagrar i DB och ritar.
 *
 *     In: id     => Pekare till identitet.
 *         poipek => Pekare till DB-struktur.
 *         pnp    => Pekare till namnparameterblock.
 *
 *     Ut: Inget.
 *
 *     Felkod:      0 = Ok.
 *             EX1312 = Kan ej lagra punkt i DB.
 *
 *     (C)microform ab 14/11/85 B.Doverud
 *
 *     15/10/86 SAVE, J. Kjellander
 *     27/12/86 hit och save, J. Kjellander
 *     20/3/92  lsysla, J. Kjellander
 *     1998-04-03 WIDTH, J.Kjellander
 *     1999-12-18 Empty error message stack G Liden
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Namnparametrar.
*/
    poipek->hed_p.blank = (char)pnp->blank;
    poipek->hed_p.pen   = (short)pnp->pen;
    poipek->hed_p.level = (short)pnp->level;
    poipek->wdt_p       = pnp->width;
    poipek->pcsy_p      = lsysla;
/*
***Lagra i DB.
*/
    if ( pnp->save )
      {
      poipek->hed_p.hit = (char)pnp->hit;
      if ( DBinsert_point(poipek,id,&la) < 0 ) return(erpush("EX1312",""));
      }
    else
      {
      poipek->hed_p.hit = 0;
      }
/*
***Rita.
*/
    gpdrpo(poipek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrpo(poipek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrpo(poipek,la,GWIN_ALL);
#endif

/*
***Succesful creation of point. Empty error message stack
*/

    varkon_erinit();

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXpofr(
       DBId     *id,
       DBVector *pv,
       V2NAPA   *pnp)

/*      Skapar POI_FREE, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Pekare till koordinater.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1302 = Kan ej beräkna punkt-data..
 *              EX1312 = Kan ej lagra punkt i DB.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXepoi, B. Doverud
 *      27/12/86 hit och save, J. Kjellander
 *
 ******************************************************!*/

  {
    DBPoint   poi;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pv,&lklsyi,pv);
/*
***Beräkna koordinater.
*/
    if ( GE400(pv,&poi) < 0 )
         return(erpush("EX1302",""));
/*
***Lagra i gm och rita.
*/
    return ( EXepoi(id,&poi,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXpopr(
       DBId     *id,
       DBVector *pv,
       V2NAPA   *pnp)

/*      Skapar POI_PROJ, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Pekare till koordinater.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1302 = Kan ej beräkna punkt-data.
 *              EX1312 = Kan ej lagra punkt i DB.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXepoi, B. Doverud
 *      27/12/86 hit och save, J. Kjellander
 *
 ******************************************************!*/

  {
    DBPoint   poi;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pv,&lklsyi,pv);
/*
***Beräkna koordinater.
*/
    if ( GE401(pv,lsyspk,&poi) < 0 )
         return(erpush("EX1302",""));
/*
***Lagra i gm och rita.
*/
    return ( EXepoi(id,&poi,pnp));
  }

/********************************************************/
/*!******************************************************/

       short EXecur(
       DBId    *id,
       DBCurve   *curpek,
       DBSeg   *grsegp,
       DBSeg   *gesegp,
       V2NAPA  *pnp)

/*     Lagrar kurva i DB och ritar. Denna rutin anropas
 *     av alla andra kurvrutiner.
 *
 *      In: id     => Pekare till identitet.
 *          curpek => Pekare till DB-struktur.
 *          gesegp => Pekare till geometriska segment.
 *          grsegp => Pekare till grafiska segment.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1372 = Kan ej lagra kurva i DB.
 *
 *      (C)microform ab 14/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit och save, J. Kjellander
 *      22/11/91 Font mm. J. Kjellander
 *      5/1/91   Kurvlängd, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      13/4/93  gesegp, J. Kjellander
 *      18/12/94 GEarclength() på grseg, J. Kjellander
 *      1999-12-18 Empty error message stack G Liden
 *
 ******************************************************!*/

  {
    short  status,nsgeo;
    DBptr  la;
    DBfloat  dummy;

/*
***Fyll i attribut.
*/
    curpek->hed_cu.blank = (char)pnp->blank;
    curpek->hed_cu.pen   = (short)pnp->pen;
    curpek->hed_cu.level = (short)pnp->level;
    curpek->fnt_cu       = (short)pnp->cfont;
    curpek->lgt_cu       = pnp->cdashl;
    curpek->wdt_cu       = pnp->width;
    curpek->pcsy_cu      = lsysla;
/*
***Typ av storhet fylls visserligen i av DBinsert_curve() men behövs
***för att GEarclength() skall göra rätt. Typ av kurva fylls i av
***den rutin som skapat kurvan (geoxxx/EXexxx).
*/
    curpek->hed_cu.type = CURTYP;
/*
***Beräkna segmentlängder och total kurvlängd i R3.
***Dessa lagras av GEarclength() i resp. poster så resultatet 
***bryr vi oss inte om för tillfället, dummy.
***Om separat grafisk representation ingår måste även dessa
***segments längder beräknas så att tex. plycud()+geo717() kan göra
***streckade kurvor. GEarclength är egentligen avsedd för geometriska
***segment men detta kan vi fixa med lite fusk just här.
*/
   if ( grsegp != gesegp )
     {
     nsgeo = curpek->ns_cu;
     curpek->ns_cu = curpek->nsgr_cu;
     if ( (status=GEarclength((DBAny *)curpek,grsegp,&dummy)) < 0 )
       return(status);
     curpek->ns_cu = nsgeo;
     }
/*
***Under alla omständigheter skall geo-segmentens längder beräknas.
***Detta måste göras efter att grafiska segments längder har beräknats
***annars kommer curpek->al_cu att hålla fel värde.
*/
   if ( (status=GEarclength((DBAny *)curpek,gesegp,&dummy)) < 0 )
     return(status);
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      curpek->hed_cu.hit = (char)pnp->hit;
      if ( DBinsert_curve(curpek,grsegp,gesegp,id,&la) < 0 )
           return(erpush("EX1372",""));
      }
    else
      {
      curpek->hed_cu.hit = 0;
      }
/*
***Rita.
*/
    gpdrcu(curpek,grsegp,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrcu(curpek,grsegp,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrcu(curpek,grsegp,la,GWIN_ALL);
#endif

/*
***Succesful creation of curve. Empty error message stack
*/

    varkon_erinit();

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXcufr(
       DBId     *id,
       DBVector *pv,
       DBVector *tv,
       DBshort   npoi,
       V2NAPA   *pnp)

/*      Skapar CUR_FREE, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Vektor med punkter.
 *          tv     => Vektor med tangenter.
 *          npoi   => Antal punkter.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1362 = Kan ej beräkna kurv-data.
 *
 *      (C)microform ab 20/1/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXecur, B. Doverud
 *      20/11/85 Tangenter, J. Kjellander
 *      10/1/86  Bug tangenter, J. Kjellander
 *      30/10/86 v3dbuf, J. Kjellander
 *      27/12/86 hit och save, J. Kjellander
 *      22/11/91 plank, J. Kjellander
 *      7/6/93   Dynamiska segment, J. Kjellander
 *      1996-11-06 nsgr_cu, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    DBCurve   cur;
    DBSeg  *segpek;
    
/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      for ( i=0; i<npoi; ++i)
         {
         GEtfpos_to_local(&pv[i],&lklsyi,&pv[i]);
         GEtfvec_to_local(&tv[i],&lklsyi,&tv[i]);
         }
      }
/*
***Allokera minne för segment.
*/
    segpek = DBcreate_segments((int)npoi-1);
/*
***Beräkna segmentkoefficienter.
*/
    if ( GE810(pv,tv,npoi,&cur,segpek) < 0 )
      {
      status = erpush("EX1362","");
      goto end;
      }
/*
***Antal grafiska segment.
*/
    cur.nsgr_cu = cur.ns_cu;
/*
***Lagra i gm och rita.
*/
    status = EXecur(id,&cur,segpek,segpek,pnp);
/*
***Slut.
*/
end:
    DBfree_segments(segpek);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXcupr(
       DBId     *id,
       DBVector *pv,
       DBVector *tv,
       DBshort   npoi,
       V2NAPA   *pnp)

/*      Skapar CUR_PROJ, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          pv     => Vektor med punkter.
 *          tv     => Vektor med tangenter.
 *          npoi   => Antal punkter.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1362 = Kan ej beräkna kurv-data.
 *
 *      (C)microform ab 11/7/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      20/11/85 Anrop till EXecur, B. Doverud
 *      20/11/85 Tangenter, J. Kjellander
 *      10/1/86  Bug tangenter, J. Kjellander
 *      30/10/86 v3dbuf, J. Kjellander
 *      27/12/86 hit och save, J. Kjellander
 *      7/6/93   Dynamiska segment, J. Kjellander
 *      1996-11-06 nsgr_cu, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    DBCurve   cur;
    DBSeg  *segpek;

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      for ( i=0; i<npoi; ++i)
         {
         GEtfpos_to_local(&pv[i],&lklsyi,&pv[i]);
         GEtfvec_to_local(&tv[i],&lklsyi,&tv[i]);
         }
      }
/*
***Allokera minne för segment.
*/
    segpek = DBcreate_segments((int)npoi-1);
/*
***Beräkna segmentkoefficienter.
*/
    if ( GE811(pv,tv,npoi,lsyspk,&cur,segpek) < 0 )
      {
      status = erpush("EX1362","");
      goto end;
      }
/*
***Antal grafiska segment.
*/
    cur.nsgr_cu = cur.ns_cu;
/*
***Lagra i gm och rita.
*/
    status = EXecur(id,&cur,segpek,segpek,pnp);
/*
***Slut.
*/
end:
    DBfree_segments(segpek);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       DBstatus  EXcurs(
       DBId     *id,
       DBint     type,
       DBVector *pv,
       DBVector *tv,
       DBint     npoi,
       V2NAPA   *pnp)

/*      Creates a spline. type 1,2 and 3 were added 1999-10-26.
 *      type 4 and 5 are not documented and only left for back-
 *      ward compatibility. They correspond to the old stiffness-
 *      spline without the "free end".
 *
 *      In: id     => Curve ID.
 *          type   => 1 = Ferguson
 *                    2 = Chord length
 *                    3 = Stiffness
 *                    4 = Free (obsolete)
 *                    5 = Proj (obsolete)
 *          pv     => Positions to interpolate
 *          tv     => Optional tangent positions
 *          npoi   => Number of positions
 *          pnp    => Pointer to attributes
 *
 *      Errcodes:      0 = Ok.
 *                EX1362 = Can't calculate curve data.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      7/6/93     Dynamiska segment, J. Kjellander
 *      1996-11-06 nsgr_cu, J.Kjellander
 *      1999-10-26 type, J.Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    DBstatus status;
    DBCurve    cur;
    DBSeg   *segpek;

/*
***Tangent positions to true tangents.
*/
    for ( i=0; i<npoi; ++i )
      {
      if ( tv[i].x_gm != 0.0  ||  tv[i].y_gm != 0.0  ||  tv[i].z_gm != 0.0 )
        {
        tv[i].x_gm -= pv[i].x_gm;
        tv[i].y_gm -= pv[i].y_gm;
        tv[i].z_gm -= pv[i].z_gm;
        }
      }
/*
***Transform positions and tangents to basic.
*/
    if ( lsyspk != NULL )
      {
      for ( i=0; i<npoi; ++i )
        {
        GEtfpos_to_local(&pv[i],&lklsyi,&pv[i]);
        GEtfvec_to_local(&tv[i],&lklsyi,&tv[i]);
        }
      }
/*
***Allocate memory for segments.
*/
    segpek = DBcreate_segments((int)npoi-1);
/*
***Calculate segment coefficients.
*/
    switch ( type )
      {
      case 1: status = GE807(pv,tv,npoi,&cur,segpek); break;
      case 2: status = GE808(pv,tv,npoi,&cur,segpek); break;
      case 3: status = GE809(pv,tv,npoi,&cur,segpek); break;
      case 4: status = GE810(pv,tv,(short)npoi,&cur,segpek); break;
      case 5: status = GE811(pv,tv,(short)npoi,lsyspk,&cur,segpek); break;
      default: status = -3; break;
      }

    if ( status < 0 )
      {
      status = erpush("EX1362","");
      goto end;
      }
/*
***Number of graphical segments.
*/
    cur.nsgr_cu = cur.ns_cu;
/*
***Save in DB and display.
*/
    status = EXecur(id,&cur,segpek,segpek,pnp);
/*
***Clean up.
*/
end:
    DBfree_segments(segpek);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXcurc(
       DBId     *id,
       bool      proj,
       DBVector *pv,
       DBVector *tv,
       DBVector *iv,
       DBfloat   fv[],
       DBshort   npoi,
       V2NAPA   *pnp)

/*      Skapar CUR_CONIC, lagrar i DB och ritar.
 *
 *      In: id     => Pekare till identitet.
 *          proj   => TRUE=PROJ, FALSE=FREE.
 *          pv     => Vektor med punkter.
 *          tv     => Vektor med tangent-definitionspunkter.
 *          iv     => Vektor med mellanliggande punkter.
 *          fv     => Vektor med P-värden (flyttal).
 *                    P < 0 : Mellanliggande punkt
 *                    P = 0 : F2-segment
 *                    P > 0 : P-värde
 *          npoi   => Antal punkter.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX1362 = Kan ej beräkna kurv-data.
 *              EX2012 = För stort P-värde.
 *              EX2052 = Kan ej mallokera
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      15/4/92    Bug, npoi-1, J. Kjellander
 *      7/6/93     Dynamiska segment, J. Kjellander
 *      1996-11-06 nsgr_cu, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i,status;
    DBCurve   cur;
    DBSeg  *segpek;

/*
***Kolla att P-värden ligger inom tillåtet intervall.
*/
   for ( i=0; i<npoi-1; ++i )
     if ( fv[i] > 0.85 ) return(erpush("EX2012","")); 
/*
***Transformera npoi punkter och tangent-definitionspunkter till basic.
***Antalet mellanliggande punkter är npoi-1 och transformeras bara om
***segmentets P-värde < 0.
*/
    if ( lsyspk != NULL )
      {
      for ( i=0; i<npoi; ++i)
         {
         GEtfpos_to_local(&pv[i],&lklsyi,&pv[i]);
         GEtfpos_to_local(&tv[i],&lklsyi,&tv[i]);
         }
      for ( i=0; i<npoi-1; ++i)
         if ( fv[i] < 0.0 ) GEtfpos_to_local(&iv[i],&lklsyi,&iv[i]);
      }
/*
***Allokera minne för segment.
*/
    if ( (segpek=DBcreate_segments((int)npoi-1)) == NULL )
      return(erpush("EX2052","CUR_CONIC"));
/*
***Beräkna segmentkoefficienter.
*/
    if ( proj )
      {
      if ( GE816(pv,tv,iv,fv,npoi,lsyspk,&cur,segpek) < 0 )
        {
        status = erpush("EX1362","");
        goto end;
        }
      cur.plank_cu = TRUE;
      GE814(&cur,lsyspk);
      }
    else
      {
      if ( GE815(pv,tv,iv,fv,npoi,&cur,segpek) < 0 )
        {
        status = erpush("EX1362","");
        goto end;
        }
      cur.plank_cu = FALSE;
      }
/*
***Antal grafiska segment.
*/
    cur.nsgr_cu = cur.ns_cu;
/*
***Lagra i gm och rita.
*/
    status = EXecur(id,&cur,segpek,segpek,pnp);
/*
***Slut.
*/
end:
    DBfree_segments(segpek);
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXcuro(
       DBId   *id,
       DBId   *rid,
       DBfloat offs,
       V2NAPA *pnp)

/*      Skapa CUR_OFFS.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad kurvas id.
 *          offs   => Offset.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      7/6/93   Dynamiska segment, J. Kjellander
 *      20/4/94  UV-segment, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype   typ;
    DBCurve   ocur,ncur;
    DBSeg  *oldgra,*oldgeo,*newgra,*newgeo;

/*
***Hämta geometri-data för refererad storhet.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&ocur,&oldgra,&oldgeo,la);
      break;

      default:
      return(erpush("EX1412","CUR_OFFS"));
      }
/*
***Allokera minne för den nya kurvans segment.
*/
    newgeo = DBcreate_segments((int)ocur.ns_cu);

    if ( oldgra != oldgeo )
      newgra = DBcreate_segments((int)ocur.nsgr_cu);
    else
      newgra = newgeo;
/*
***Bilda kurva.
*/
    status = GE813(&ocur,oldgra,oldgeo,lsyspk,offs,&ncur,newgra,newgeo);
    if ( status < 0 ) goto end;
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&ncur,newgra,newgeo,pnp);
/*
***Lämna tillbaks allokerat minne.
*/
end:
    if ( oldgra != oldgeo )
      {
      DBfree_segments(oldgra);
      DBfree_segments(newgra);
      }
    
    DBfree_segments(oldgeo);
    DBfree_segments(newgeo);

    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcomp(
       DBId    *id,
       DBId    *ridvek,
       DBshort  nref,
       V2NAPA  *pnp)

/*      Skapa CUR_COMP.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          ridvek => Pekare till vektor med referenser.
 *          nref   => Antal referenser.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *          EX1502 => Kan ej konvertera linje till kurva
 *
 *      (C)microform ab 23/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J. Kjellander
 *      20/11/85   Anrop till EXecom, B. Doverud
 *      30/10/86   v3dbuf, J. Kjellander
 *      27/12/86   hit och save, J. Kjellander
 *      11/5/87    linje, J. Kjellander
 *      5/11/91    com->cur, J. Kjellander
 *      5/3/93     Typ av segment, J. Kjellander
 *      7/6/93     Dynamiska segment, J. Kjellander
 *      20/4/94    UV-segment, J. Kjellander
 *      1996-06-10 Bug långa kurvor, J.Kjellander
 *      1997-04-14 Pass 0. J.Kjellander
 *      1997-05-25 Bug långa kurvor, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    int     i,allant,nseg;
    DBptr   ent_la;
    DBetype   ent_typ;
    bool    pass2;
    DBLine  lin;
    DBArc   arc;
    DBCurve   oldcur,newcur;
    DBSeg  *oldgra,*oldgeo,*newgra,*newgeo;
    DBSeg   arcseg[4];
    DBVector   pos;
    ENTTAB  et[GMMXCO];

/*
***Pass 0. Gå igenom storheterna och skapa entity_table et.
*/
    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&ent_la,&ent_typ) < 0 ) 
        return(erpush("EX1402",""));

      et[i].la       = ent_la;
      et[i].typ      = ent_typ;
      et[i].reversed = FALSE;
      et[i].ngeoseg  = 0;
      et[i].pgeoseg  = NULL;
      }
/*
***Till att börja utgår vi ifrån att resultatet inte är en
***plan kurva.
*/
    newcur.plank_cu = FALSE;
/*
***Sen blir det antingen ett eller två pass.
***Det första för geometriska segment. Om det därvid
***visar sig att någon av kurvorna har har separat grafisk
***representation sätts flaggan pass2 och ett andra pass
***bygger upp den grafiska representationen.
*/
    pass2 = FALSE;
/*
***Här börjar pass 1.
***Allokera minne för ett gäng nya geometriska segment.
*/
    newgeo = DBcreate_segments(DSGANT);
    allant = DSGANT;
    nseg   = 0;
/*
***Hämta geometri-data för refererade storheter och sätt
***ihop geo-segment till en ny kurva.
*/
    for ( i=0; i<nref ; ++i )
      {
      switch ( et[i].typ )
         {
/*
***Linje.
*/
         case (LINTYP):
         if ( allant < nseg+1 )
           {
           newgeo = DBadd_segments(newgeo,allant+DSGANT);
           allant += DSGANT;
           }
         DBread_line(&lin,et[i].la);
         (newgeo+nseg)->c0x = lin.crd1_l.x_gm;
         (newgeo+nseg)->c0y = lin.crd1_l.y_gm;
         (newgeo+nseg)->c0z = lin.crd1_l.z_gm;
         (newgeo+nseg)->c0  = 1.0;
         (newgeo+nseg)->c1x = lin.crd2_l.x_gm - lin.crd1_l.x_gm;
         (newgeo+nseg)->c1y = lin.crd2_l.y_gm - lin.crd1_l.y_gm;
         (newgeo+nseg)->c1z = lin.crd2_l.z_gm - lin.crd1_l.z_gm;
         (newgeo+nseg)->c1  = (newgeo+nseg)->c2x =
         (newgeo+nseg)->c2y = (newgeo+nseg)->c2z =
         (newgeo+nseg)->c2  = (newgeo+nseg)->c3x =
         (newgeo+nseg)->c3y = (newgeo+nseg)->c3z =
         (newgeo+nseg)->c3  = (newgeo+nseg)->ofs = 0.0;
         (newgeo+nseg)->typ = CUB_SEG;
         et[i].ngeoseg = 1;
         et[i].pgeoseg = newgeo + nseg;
         break;
/*
***Arc.
*/
         case (ARCTYP):
         DBread_arc(&arc,arcseg,et[i].la);
         if ( arc.ns_a == 0 )
           {
           pos.x_gm = arc.x_a;
           pos.y_gm = arc.y_a;
           pos.z_gm = 0.0;
           GE300(&pos,arc.r_a,arc.v1_a,arc.v2_a,NULL,&arc,arcseg,3);
           }
         if ( allant < nseg+arc.ns_a )
           {
           newgeo = DBadd_segments(newgeo,allant+DSGANT);
           allant += DSGANT;
           }
         V3MOME(arcseg,newgeo+nseg,arc.ns_a*sizeof(DBSeg));
         et[i].ngeoseg = arc.ns_a;
         et[i].pgeoseg = newgeo + nseg;
         break;
/*
***Curve.
*/
         case (CURTYP):
         DBread_curve(&oldcur,&oldgra,&oldgeo,et[i].la);
         if ( oldgra != oldgeo )
           {
           DBfree_segments(oldgra);
           pass2 = TRUE;
           }

         if ( allant < nseg+oldcur.ns_cu )
           {
           newgeo  = DBadd_segments(newgeo,nseg+oldcur.ns_cu+DSGANT);
           allant  = nseg+oldcur.ns_cu+DSGANT;
           }

         V3MOME(oldgeo,newgeo+nseg,oldcur.ns_cu*sizeof(DBSeg));
         DBfree_segments(oldgeo);
         et[i].ngeoseg = oldcur.ns_cu;

         if ( oldcur.plank_cu )
           {
           newcur.plank_cu = TRUE;
           V3MOME(&oldcur.csy_cu,&newcur.csy_cu,sizeof(DBTmat));
           }
         break;
/*
***Icke tillåten typ av storhet.
*/
         default:
         return(erpush("EX1412","CUR_COMP"));
         }
/*
***Hur många geometriska segment är vi uppe i nu ?
*/
      nseg += et[i].ngeoseg;
      }
/*
***Typ av storhet och totalt antal geometriska segment.
*/
    newcur.hed_cu.type = CURTYP;
    newcur.ns_cu       = nseg;
/*
***Beräkna C-adresser till 1:a segmentet i varje storhet.
***OBS ! Detta kan göras först nu eftersom det är först
***nu som vi vet vilken värde newgeo slutligen fick.
***Reallkoering (DBadd_segments()) ändrar värdet på newgeo ! 1997-05-25
*/
    for ( nseg=i=0; i<nref; ++i )
      {
      et[i].pgeoseg = newgeo + nseg;
      nseg += et[i].ngeoseg;
      }
/*
***Kolla att de nya kurvorna sitter ihop och byt riktning på dom om
***det behövs.
*/
    status = check((DBint)nref,&newcur,et);
    if ( status < 0 ) goto end;
/*
***Här börjar pass 2.
*/
    if ( pass2 )
      {
      newgra = DBcreate_segments(DSGANT);
      allant = DSGANT;
      nseg   = 0;
/*
***Gå igenom storheterna igen och bilda grafisk reresentation.
*/
      for ( i=0; i<nref ; ++i )
        {
        switch ( et[i].typ )
          {
/*
***För linje och cirkel använder vi de geometriska segmenten
***även som grafisk representation.
*/
          case (LINTYP):
          case (ARCTYP):
          if ( allant < nseg+et[i].ngeoseg )
            {
            newgra = DBadd_segments(newgra,allant+DSGANT);
            allant += DSGANT;
            }
          V3MOME(et[i].pgeoseg,newgra+nseg,et[i].ngeoseg*sizeof(DBSeg));
          nseg += et[i].ngeoseg;
          break;
/*
***För kurva använder vi den ursprungliga grafiska representationen.
***Om kurvar är reverserad måste även de grafiska segmenten reverseras.
*/
          case (CURTYP):
          DBread_curve(&oldcur,&oldgra,NULL,et[i].la);

          if ( et[i].reversed )
            {
            oldcur.ns_cu = oldcur.nsgr_cu;
            status = reverse(&oldcur,oldgra);
            if ( status < 0 ) goto end;
            }

          if ( allant < nseg+oldcur.nsgr_cu )
            {
            newgra  = DBadd_segments(newgra,nseg+oldcur.nsgr_cu+DSGANT);
            allant  = nseg+oldcur.nsgr_cu+DSGANT;
            }

          V3MOME(oldgra,newgra+nseg,oldcur.nsgr_cu*sizeof(DBSeg));
          DBfree_segments(oldgra);
          nseg += oldcur.nsgr_cu;
          break;
          }
       }
    }
/*
***Om inget pass 2 behövs är grafisk och geometrisk
***representation lika.
*/
    else newgra = newgeo;
/*
***Slut pass 2. Hur många grafiska segment blev det ?
*/
    newcur.nsgr_cu = nseg;
/*
***Lagra comp-posten i DB och rita.
*/
    status = EXecur(id,&newcur,newgra,newgeo,pnp);
/*
***Deallokera minne.
*/
    if ( newgra != newgeo ) DBfree_segments(newgra);
end:
    DBfree_segments(newgeo);

    return(status);
  }
  
/********************************************************/
/*!******************************************************/

 static short   check(
        DBint   ncur,
        DBCurve  *comp_cur,
        ENTTAB  et[])

/*      Testar att kurvorna som skapats i pass 1
 *      sitter ihop och om inte provar att vända
 *      på dom.
 *
 *      In: ncur     => Antal kurvor
 *          comp_cur => Den nya sammansatta kurvans kurvpost
 *          et       => Kurvtabell
 *
 *      Ut: Inget.
 *
 *      FV:       0 => Ok.
 *               <0 => Fel.
 *
 *      Felkoder: GE8272 = Två segment sitter inte ihop.
 *
 *      (C)microform ab 1997-04-14 J. Kjellander
 *
 *      1998-02-03 Bug, J.Kjellander
 *      1999-12-18 sur741->varkon_idpoint  G Liden
 *
 ******************************************************!*/

  {
   char   errbuf[V3STRLEN+1];
   short  i,status;
   DBfloat  dist;
   DBfloat  idpoint;
   DBVector  start[GMMXCO],slut[GMMXCO],diff;
   EVALC  seg;
   DBCurve  tmpcur;

/*
***Är det bara en enda kurva kan det inte gärna vara fel.
*/
   if ( ncur == 1 ) return(0);
/*
***Tolerens för 2 positioner lika.
*/
   idpoint = varkon_idpoint();
/*
***Gå igenom kurvorna och beräkna alla start och slutpositioner.
***Vi använder en kopia av kurvposten för den sammanslagna kurvan
***där vi justerar antal segment inför varje anrop.
*/
   V3MOME(comp_cur,&tmpcur,sizeof(DBCurve));

   for ( i=0; i<ncur; ++i )
     {
     tmpcur.ns_cu = et[i].ngeoseg;

     seg.evltyp = EVC_R;
     seg.t_local = 0.0;
     if ( (status=GE110((DBAny *)&tmpcur,et[i].pgeoseg,&seg)) < 0 )
       return(status);
     start[i].x_gm = seg.r.x_gm;
     start[i].y_gm = seg.r.y_gm;
     start[i].z_gm = seg.r.z_gm;

     seg.evltyp = EVC_R;
     seg.t_local = 1.0;
     if ( (status=GE110((DBAny *)&tmpcur,et[i].pgeoseg+
                                 et[i].ngeoseg-1,&seg)) < 0 ) return(status);
     slut[i].x_gm = seg.r.x_gm;
     slut[i].y_gm = seg.r.y_gm;
     slut[i].z_gm = seg.r.z_gm;
     }
/*
***Vi börjar med att kolla slut på 1:a mot start på 2:a.
*/
     diff.x_gm = slut[0].x_gm - start[1].x_gm;
     diff.y_gm = slut[0].y_gm - start[1].y_gm;
     diff.z_gm = slut[0].z_gm - start[1].z_gm;
     if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
     else               dist = GEvector_length3D(&diff);
     
/*
***Om dom inte sitter ihop kollar vi start mot start.
*/
     if ( ABS(dist) > idpoint )
       {
       diff.x_gm = start[0].x_gm - start[1].x_gm;
       diff.y_gm = start[0].y_gm - start[1].y_gm;
       diff.z_gm = start[0].z_gm - start[1].z_gm;
       if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
       else               dist = GEvector_length3D(&diff);
/*
***Om dom inte sitter ihop kollar vi slut mot slut.
*/
       if ( ABS(dist) > idpoint )
         {
         diff.x_gm = slut[0].x_gm - slut[1].x_gm;
         diff.y_gm = slut[0].y_gm - slut[1].y_gm;
         diff.z_gm = slut[0].z_gm - slut[1].z_gm;
         if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
         else               dist = GEvector_length3D(&diff);
/*
***Om dom inte sitter ihop kollar vi start mot slut.
*/
         if ( ABS(dist) > idpoint )
           {
           diff.x_gm = start[0].x_gm - slut[1].x_gm;
           diff.y_gm = start[0].y_gm - slut[1].y_gm;
           diff.z_gm = start[0].z_gm - slut[1].z_gm;
           if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
           else               dist = GEvector_length3D(&diff);
/*
***Om dom fortfarande inte sitter ihop är det glapp.
*/
           if ( ABS(dist) > idpoint )
             {
             sprintf(errbuf,"%d%%%d",1,2);
             return(erpush("GE8272",errbuf));
             }
/*
***Båda behöver vändas.
*/
           else
             {
             tmpcur.ns_cu = et[0].ngeoseg;
             status = reverse(&tmpcur,et[0].pgeoseg);
             if ( status < 0 ) return(status);
             et[0].reversed = TRUE;
             tmpcur.ns_cu = et[1].ngeoseg;
             status = reverse(&tmpcur,et[1].pgeoseg);
             if ( status < 0 ) return(status);
             et[1].reversed = TRUE;
             }
           }
/*
***Den 2:a behöver vändas.
*/
         else
           {
           tmpcur.ns_cu = et[1].ngeoseg;
           status = reverse(&tmpcur,et[1].pgeoseg);
           if ( status < 0 ) return(status);
           et[1].reversed = TRUE;
           }
         }
/*
***Den 1:a behöver vändas.
*/
       else
         {
         tmpcur.ns_cu = et[0].ngeoseg;
         status = reverse(&tmpcur,et[0].pgeoseg);
         if ( status < 0 ) return(status);
         et[0].reversed = TRUE;
         }
       }
/*
***Kolla resten.
*/
   for ( i=2; i<ncur; ++i )
     {
     if ( !et[i-1].reversed )
       {
       diff.x_gm = slut[i-1].x_gm - start[i].x_gm;
       diff.y_gm = slut[i-1].y_gm - start[i].y_gm;
       diff.z_gm = slut[i-1].z_gm - start[i].z_gm;
       }
     else
       {
       diff.x_gm = start[i-1].x_gm - start[i].x_gm;
       diff.y_gm = start[i-1].y_gm - start[i].y_gm;
       diff.z_gm = start[i-1].z_gm - start[i].z_gm;
       }
     if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
     else               dist = GEvector_length3D(&diff);

     if ( ABS(dist) > idpoint )
       {
       if ( !et[i-1].reversed )
	     {
         diff.x_gm = slut[i-1].x_gm - slut[i].x_gm;
         diff.y_gm = slut[i-1].y_gm - slut[i].y_gm;
         diff.z_gm = slut[i-1].z_gm - slut[i].z_gm;
         }
       else
         {
         diff.x_gm = start[i-1].x_gm - slut[i].x_gm;
         diff.y_gm = start[i-1].y_gm - slut[i].y_gm;
         diff.z_gm = start[i-1].z_gm - slut[i].z_gm;
         }
       if ( modtyp == 2 ) dist = GEvector_length2D(&diff);
       else               dist = GEvector_length3D(&diff);

       if ( ABS(dist) > idpoint )
         {
         sprintf(errbuf,"%d%%%d",i,i+1);
         return(erpush("GE8272",errbuf));
         }
       else
         {
         tmpcur.ns_cu = et[i].ngeoseg;
         status = reverse(&tmpcur,et[i].pgeoseg);
         if ( status < 0 ) return(status);
         et[i].reversed = TRUE;
         }
       }
     }

   return(0);
} 

/********************************************************/
/*!******************************************************/

 static short  reverse(
        DBCurve *cur,
        DBSeg *seg)

/*      Reverserar segmenten i en kurva.
 *
 *      In: cur => Kurvpost
 *          seg => Segment
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-04-14 J. Kjellander
 *
 ******************************************************!*/

  {
   short  status;
   DBfloat  t1,t2;
   DBCurve  newcur;
   DBSeg *newseg;

/*
***Allokera minne för nya segment.
*/
   newseg = DBcreate_segments((int)cur->ns_cu);
/*
***Beräkna nya segment.
*/
   t1 = (DBfloat)cur->ns_cu + 1.0;
   t2 = 1.0;
   status = GE817((DBAny *)cur,seg,&newcur,newseg,t1,t2);
   if ( status < 0 ) return(status);
/*
***Ersätt dom gamla.
*/
   V3MOME(newseg,seg,(int)cur->ns_cu*sizeof(DBSeg));
/*
***Lämna tillbaks minne.
*/
   DBfree_segments(newseg);
/*
***Slut.
*/
   return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcurt(
       DBId    *id,
       DBId    *rid,
       DBfloat  t1,
       DBfloat  t2,
       V2NAPA  *pnp)

/*      Skapa CUR_TRIM.
 *
 *      In: id   => Pekare till kurvans identitet.
 *          rid  => Pekare till refererad id.
 *          t1   => t-värde för position 1.
 *          t2   => t-värde för position 2.
 *          pnp  => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1402 => Refererad storhet finns ej.
 *                EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 16/11/91 J. Kjellander
 *
 *       23/11/94 UV_SEG, G. Liden
 *      1999-12-18 sur960->varkon_sur_scur_gra  G. Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype   typ;
    DBCurve   oldcur,newcur;
    DBSeg  *osegpk,*nsegpk,*graseg;

/*
***Hämta geometri-data för refererad storhet.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case (CURTYP):
      DBread_curve(&oldcur,NULL,&osegpk,la);
      break;

      default:
      return(erpush("EX1412","CUR_TRIM"));
      }
/*
***Allokera minne för ny kurva.
*/
    nsegpk = DBcreate_segments((int)oldcur.ns_cu);
/*
***Beräkna ny kurva.
*/
    t1 += 1.0; t2 += 1.0;
    status = GE817((DBAny *)&oldcur,osegpk,&newcur,nsegpk,t1,t2);
    if ( status < 0 ) goto end;
/*
***Bilda grafisk representation. Observera att varkon_sur_scur_gra()
***anropar rutiner som tittar på al_cu. Normalt är den
***inte satt i detta läge och systemet dyker om den är
***odefinierad. 1996-12-20 JK
*/
    newcur.al_cu = 0.0;        /* JK */

    status = varkon_sur_scur_gra(&newcur,nsegpk,&graseg);
    if ( status < 0 ) goto end; 
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&newcur,graseg,nsegpk,pnp);
/*
***Lämna tillbaks allokerat minne.
*/
    DBfree_segments(graseg);
end: 
    DBfree_segments(osegpk);
    DBfree_segments(nsegpk);

    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcuru(
       DBId   *id,
       DBshort nseg,
       DBSeg   geoseg[],
       DBId   *surid,
       V2NAPA *pnp)

/*      Skapa CUR_USRDEF.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          nseg   => Antal segment.
 *          geoseg => Koefficient-matriser.
 *          surid  => Pekare till yta eller NULL.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 16/11/91 J. Kjellander
 *
 *      4/10/92 Slutputs, J. Kjellander
 *      1996-11-06 nsgr_cu, J.Kjellander
 *      1997-12-18 UV-kurvor, J.Kjellander
 *      1998-04-18 Check UV med sur367, sur778  G. Liden
 *      1999-12-18 sur960->varkon_sur_scur_gra
 *                 sur367->varkon_cur_usrche
 *                 sur778->varkon_ini_gmcur     G. Liden
 *
 ******************************************************!*/

  {
    short  status;
    DBint  i;
    DBptr  la;
    DBetype  typ;
    DBCurve  cur;
    DBSeg *graseg;
    DBSurf sur;
    DBPatch *p_pat=NULL;

/*
***Lokalt koordinatsystem aktivt ?
***Transformera segmenten till basic.
*/
    if ( surid != NULL  &&  lsyspk != NULL )
      {
      for ( i=0; i<nseg; ++i ) GEtfseg_to_local(&geoseg[i],&lklsyi,&geoseg[i]);
      }
/*
***Antal segment mm.
*/
    varkon_ini_gmcur(&cur);
    cur.hed_cu.type = CURTYP;
    cur.ns_cu       = cur.nsgr_cu = nseg;
    cur.plank_cu    = FALSE;
    cur.al_cu       = 0.0;
/*
***Är det en UV-kurva ?
*/
    if ( surid != NULL )
      {
      if ( DBget_pointer('I',surid,&la,&typ) < 0 ) return(erpush("EX1402",""));

      switch ( typ )
        {
        case SURTYP:
        if ( (status=DBread_surface(&sur,la))     < 0 ) goto error2;
        if ( (status=DBread_patches(&sur,&p_pat)) < 0 ) goto error2;
        for ( i=0; i<nseg; ++i )
          {
          geoseg[i].typ      = UV_SEG;
          geoseg[i].subtyp   = 1;
          geoseg[i].spek_gm  = la;
          geoseg[i].spek2_gm = DBNULL;
          }
        status = varkon_cur_usrche(&sur,p_pat,&cur, geoseg);
        DBfree_patches(&sur, p_pat);
        if ( status < 0 ) goto error2;
        status = varkon_sur_scur_gra(&cur,geoseg,&graseg);
        if      ( status < 0  &&  graseg != NULL ) goto error1;
        else if ( status < 0 )                     goto error2;
        break;

        default:
        return(erpush("EX1412","CUR_USRDEF"));
        }
      }
    else graseg = geoseg;
/*
***Lagra posten i DB och rita. Anropande rutin deallokerar
***geometriska segment. Eventuella grafiska deallokeras här.
*/
    status = EXecur(id,&cur,graseg,geoseg,pnp);
error1:
    if ( graseg != geoseg ) DBfree_segments(graseg);
error2:
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
