/*!*******************************************************
*
*     excur1.c
*     ========
*
*     EXecur();     Create curve
*     EXcurs();     Create CUR_SPLINE
*     EXcunu();     Create CUR_NURBS

*     EXcurc();     Create CUR_CONIC
*     EXcuro();     Create CUR_OFFS
*     EXcomp();     Create CUR_COMP
*     EXcurt();     Create CUR_TRIM
*     EXcuru();     Create CUR_USRDEF
*
*    This file is part of the VARKON Execute Library.
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
/*#include "../../GP/include/GP.h"*/
#ifdef UNIX
#include "../../WP/include/WP.h"
#endif
#include "../include/EX.h"
#include <memory.h> /* for memset */

extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern DBptr   lsysla;
extern short   modtyp;

#ifdef WIN32
extern short WPdrcu();
#endif

typedef struct enttab
{
DBptr    la;         /* logical adress to this entity                     */
DBetype  typ;        /* type of entity                                    */
DBshort  segtyp;     /* type of segments if typ=CURTYP, can be SEG_MIXED  */
DBptr    surla;      /* pointer to surface if UV_CUB_SEG or UV_NURB_SEG   */
bool     mixedofs;   /* true if mixed segment offsets                     */
DBfloat  ofs;        /* segment ofset if same for all segments            */
bool     reversed;
bool     newuv;
int      ngeoseg;    /* no of new segments for this entity                */
DBSeg   *pgeoseg;    /* pointer to first segment for this entity          */
}ENTTAB;

/*
***Internal functions used by EXcomp()
*/
static short check(DBint ncur, DBCurve *comp_cur, ENTTAB et[]);
static short makeuv(DBint ncur, DBCurve *comp_cur, ENTTAB et[], BBOX uvminmax, short typ_su);
static short reverse(DBCurve *cur,DBSeg *seg);
static short reverseNURB(DBCurve *cur,DBSeg *seg);
static short getseginfo(DBptr curla, DBshort *segtype, DBptr *surla,
                         DBfloat *ofs, bool *mixedofs);
static short mergeNURB(DBint ncur,DBCurve *comp_cur,ENTTAB et[]); 

#define MIXED_SEG  -99


/*!******************************************************/

       short EXecur(
       DBId    *id,
       DBCurve *curpek,
       DBSeg   *grsegp,
       DBSeg   *gesegp,
       V2NAPA  *pnp)

/*     Create curve. Generic for all curve methods.
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
 *      2006-12-09 Removed gpdrcu() J.Kjellander
 *
 ******************************************************!*/

  {
    short    status,nsgeo;
    DBptr    la;
    DBfloat  dummy;

/*
***Current curve attributes.
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
***Save in DB.
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
***Display curve in all graphical windows.
*/
    WPdrcu(curpek,grsegp,la,GWIN_ALL);

    return(0);
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

       DBstatus  EXcunu(
       DBId      *id,
       DBVector  *cpts,
       DBfloat   *w,
       DBint      ncpts,
       DBfloat   *knots,
       DBint      nknots,
       DBint      degree,
       V2NAPA    *pnp)

/*      Creates a NURBS curve.
 *
 *      In: id     => Curve ID
 *          cpts   => Pointer to array of R3 control points
 *          w      => Pointer to array of weight values
 *          ncpts  => Number of control points
 *          knots  => Pointer to array of knot values
 *          degree => Order - 1
 *          pnp    => Pointer to attributes
 *
 *      Errcodes:      0 = Ok.
 *                EX4212 = Fel från DB.
 *
 *      (C)2002-06-13 J. Kjellander, Örebro University
 *         2003-03-03 No gra-segs if blank=1, Sören Larsson
 *         2006-11-10 Use GEcur_cre_gra , Sören Larsson
 *
 ******************************************************!*/

  {
   int        i,nspan,segnr;
   DBHvector *phcpts;
   DBfloat    difftol;
   DBstatus   status = 0;
   DBptr      cpts_la,knots_la;

   DBCurve    cur;
   DBSeg     *p_seg,*p_appseg;
   
/*
***Transform control positions to basic.
*/
   if ( lsyspk != NULL )
     {
     for ( i=0; i<ncpts; ++i )
       {
       GEtfpos_to_local(&cpts[i],&lklsyi,&cpts[i]);
       }
     }
/*
***Allocate memory for and copy cpts and w to homogenous vector.
*/
   phcpts = v3mall(ncpts*sizeof(DBHvector),"EXcunu");
   
   for ( i=0; i<ncpts; ++i )
     {
     (phcpts + i)->x_gm = cpts[i].x_gm * w[i];
     (phcpts + i)->y_gm = cpts[i].y_gm * w[i];
     (phcpts + i)->z_gm = cpts[i].z_gm * w[i];
     (phcpts + i)->w_gm = w[i];
     }
/*
***How many DBSeg's (spans) will be needed ?
*/
   nspan = 0;
   difftol =COMPTOL*10;

   for ( i=0+degree; i<nknots-1-degree; i++ )
     {
     if ( knots[i+1] - knots[i] > difftol ) ++nspan;
     }
/*
***Allocate memory for segments.
*/
    p_seg = DBcreate_segments(nspan);
/*
***Store controlpoints and knotvector in DB.
*/
   status = DBwrite_nurbs(phcpts, ncpts, knots, nknots,
                         &cpts_la, &knots_la);

   if ( status < 0 ) return(erpush("EX4212",""));
/*
***Update DBSeg's. Each DBSeg has a C-pointer to the
***start of the cpts-array and knots-array and also
***a corresponding DB-pointer. Each DBseg also includes
***an individual offset for cpts and knots.
*/
   for ( i=0; i<nspan; ++i )
     {
     (p_seg+i)->ncpts        = ncpts;
     (p_seg+i)->cpts_db      = cpts_la;
     (p_seg+i)->cpts_c       = phcpts; 
     (p_seg+i)->nknots       = nknots;
     (p_seg+i)->knots_db     = knots_la;
     (p_seg+i)->knots_c      = knots;
     (p_seg+i)->typ          = NURB_SEG;
     (p_seg+i)->subtyp       = 0;
     (p_seg+i)->ofs          = 0.0;
     (p_seg+i)->nurbs_degree = degree;
     }
/*
***For each span, calculate the corresponding offset
***in the hcpts- and knots-array.
*/
   segnr = 0;

   for ( i=0+degree; i<nknots-1-degree; ++i )
     {
     if ( knots[i+1] - knots[i] > difftol )
       {
       (p_seg+segnr)->offset_cpts = i - degree;
       (p_seg+segnr)->offset_knots = i;
        segnr++;
       }
     }
/*
***Number of spans/segments.
*/
    cur.ns_cu   = nspan;
/*
***For the time being, let's assume this is a nonplane curve.
*/
    cur.hed_cu.type = CURTYP;
    cur.plank_cu = FALSE;
/*
***Create graphical representation. 
*/
/*    
       Skapa tillsvidare alltid grafisk rep (annars kan grafisk rep 
       av offset & tcopy missas) SL
       
       if ( pnp->blank==0 )
       {
       */
       
       p_appseg=NULL;
       GEcur_cre_gra(&cur,p_seg,&p_appseg);
       
/*       
       }
    else
       {
       p_appseg=NULL;
       cur.nsgr_cu = 0;
       }
*/
       
/* 
***Save in DB and display. 
*/
    status = EXecur(id,&cur,p_appseg,p_seg,pnp);
/*
***Clean up.
*/
    v3free(phcpts,"EXcunu");
    DBfree_segments(p_seg);
    DBfree_segments(p_appseg);

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
 *      7/6/93     Dynamiska segment, J. Kjellander
 *      20/4/94    UV-segment, J. Kjellander
 *      2004-09-06 NURBS curves, Sören Larsson, Örebro University
 *
 ******************************************************!*/

  {
    short     status;
    DBptr     la;
    DBetype   typ;
    DBCurve   ocur,ncur;
    DBSeg    *oldgra,*oldgeo,*newgra,*newgeo;

    DBHvector *phcpts;
    DBfloat   *pknots;
    bool       nurbsflag;
    DBint      nknots,ncpts;
    int i;
    DBptr      cpts_db,knots_db;
       
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
***Check for the existence of NURBS-segments.
*/
    nurbsflag = FALSE;
    for ( i=0; i < ocur.ns_cu; ++i )
      {
      if ( (oldgeo+i)->typ == NURB_SEG )
        {
        nurbsflag  = TRUE;
        ncpts      = (oldgeo+i)->ncpts;
        nknots     = (oldgeo+i)->nknots;
        break;
        }
      }      
/*
***If NURBS curve, allocate memory for control points and knots
*/
    if (nurbsflag)     
      {
      if ( (phcpts=v3mall(ncpts*sizeof(DBHvector),"EXcuro")) == NULL )
         {
         erpush("EX4222","");
         }      
      if ( (pknots=v3mall(nknots*sizeof(DBfloat),"EXcuro")) == NULL )
         {
         erpush("EX4222","");
         }       
/*
***Write pointer to control points and knots to new segments
*/
      for ( i=0; i<ocur.ns_cu; ++i )
         {
         (newgeo+i)->cpts_c  = phcpts; 
         (newgeo+i)->knots_c = pknots;
         }
      }
/*
***Create curve.
*/
    status = GE813(&ocur,oldgra,oldgeo,lsyspk,offs,&ncur,newgra,newgeo);
    if ( status < 0 ) goto end;
/*
***Store the curve data in DB and draw.
*/
    if (nurbsflag)     
      {
      status = DBwrite_nurbs(phcpts, ncpts, pknots,nknots,&cpts_db, &knots_db);
      if ( status < 0 ) return(erpush("EX4212",""));
      for ( i=0; i<ocur.ns_cu; ++i )
         {
         (newgeo+i)->cpts_db      = cpts_db;
         (newgeo+i)->knots_db     = knots_db;
         }
      }
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

    if ( nurbsflag )
      {
      v3free(phcpts,"EXcuro");
      v3free(pknots,"EXcuro");
      }    
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcomp(
       DBId    *id,
       DBId    *ridvek,
       DBshort  nref,
       DBId    *sur_id,
       V2NAPA  *pnp)

/*      Create CUR_COMP.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          ridvek => Pekare till vektor med referenser.
 *          nref   => Antal referenser.
 *          sur_id => Pointer to optional surface used by CUR_COMPARR
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
 *      2006-11-03 Added NURBS S. Larsson
 *      2006-11-03 Added possibility to create uv curves, Sören L.
 *      2007-01-22 Bugfix graph rep, comp of nurbs and lines, Sören L.
 *
 ******************************************************!*/

  {
    short     status;
    int       i,j,allant,nseg;
    DBptr     ent_la;
    DBetype   ent_typ;
    bool      pass2;
    DBLine    lin;
    DBArc     arc;
    DBCurve   oldcur,newcur,tmpcur;
    DBSeg    *oldgra,*oldgeo,*newgra,*newgeo,*tmpgra;
    DBSeg     arcseg[4];
    DBVector  pos;
    ENTTAB    et[GMMXCO];
    bool      contain_UV_NURB_SEG;
    bool      contain_UV_CUB_SEG;
    bool      contain_NURB_SEG;
    bool      contain_CUB_SEG;
    bool      contain_MIXED_SEG;
    bool      contain_UV;
    bool      contain_R3;
    char      errbuf[V3STRLEN+1];
    DBptr     cpts_la,knots_la,sur_la;
    DBfloat   comptol,idknot;
    BBOX      p_cbox;
    DBSurf    sur; 
    BBOX      uvminmax;
    DBPatch   toppat_s,toppat_e;
    idknot    = varkon_idknot();
    comptol   = varkon_comptol();

/*
***Pass 0. Traverse entities and create entity_table et.
*/
    contain_UV_NURB_SEG  = FALSE;
    contain_UV_CUB_SEG   = FALSE;
    contain_NURB_SEG     = FALSE;
    contain_CUB_SEG      = FALSE;
    contain_MIXED_SEG    = FALSE;
    contain_UV           = FALSE;
    contain_R3           = FALSE;

    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&ent_la,&ent_typ) < 0 ) 
        return(erpush("EX1402",""));  
      et[i].la       = ent_la;
      et[i].typ      = ent_typ;
      et[i].surla    = DBNULL;
      et[i].newuv    = FALSE;
      if (ent_typ == LINTYP || ent_typ == ARCTYP)
        {
        contain_R3 = TRUE;
        if (sur_id == NULL)  et[i].segtyp = CUB_SEG;
        else                 et[i].segtyp = UV_CUB_SEG;
        et[i].ofs      = 0.0;
        et[i].mixedofs = FALSE;
        }
      else if (ent_typ == CURTYP)
        {
        getseginfo(ent_la,&et[i].segtyp,&et[i].surla,&et[i].ofs,&et[i].mixedofs);
        if      (et[i].segtyp == CUB_SEG)    contain_CUB_SEG     = TRUE;
        else if (et[i].segtyp == UV_CUB_SEG) contain_UV_CUB_SEG  = TRUE;
        else if (et[i].segtyp == NURB_SEG)   contain_NURB_SEG    = TRUE;
        else if (et[i].segtyp == UV_NURB_SEG)contain_UV_NURB_SEG = TRUE;
        else if (et[i].segtyp == MIXED_SEG)  contain_MIXED_SEG   = TRUE;
        }
      et[i].reversed = FALSE;
      et[i].ngeoseg  = 0;
      et[i].pgeoseg  = NULL;
      }

    if (sur_id != NULL) 
      {
      contain_UV=TRUE;
      contain_R3=FALSE;
      if ( DBget_pointer('I',sur_id,&sur_la,&ent_typ) < 0 ) 
        {
        return(erpush("EX1402",""));
        }
      if(DBread_surface(&sur, sur_la) < 0 )
        {
        return(erpush("EX4292","")); /* cant read surface from  DB*/
        }

      if (  sur.typ_su == NURB_SUR)
        {
/*
*** For a nurbs surface the surface uv-span (defined by the knot-
*** vectors) can be retrieved from the start and end topological 
*** patches.
*/
        toppat_s.spek_c = NULL; /* do not read geom patches */
        toppat_e.spek_c = NULL;
        DBread_one_patch(&sur,&toppat_s,1,1);
        DBread_one_patch(&sur,&toppat_e,sur.nu_su,sur.nv_su);
        uvminmax.xmin= toppat_s.us_pat - idknot;
        uvminmax.ymin= toppat_s.vs_pat - idknot;
        uvminmax.xmax= toppat_e.ue_pat + idknot;
        uvminmax.ymax= toppat_e.ve_pat + idknot;
        }
      else
        {
        uvminmax.xmin= -idknot;
        uvminmax.ymin= -idknot;
        uvminmax.xmax= sur.nu_su +  idknot;
        uvminmax.ymax= sur.nv_su +  idknot;
        }
      }  
    else
      {
      if (contain_UV_CUB_SEG||contain_UV_NURB_SEG||contain_MIXED_SEG) 
        {
        contain_UV=TRUE;
        }
      else contain_UV=FALSE;
      if (contain_CUB_SEG || contain_NURB_SEG ||  contain_MIXED_SEG) 
        {
        contain_R3=TRUE;
        }
      else contain_R3= FALSE;
      }
/*
***Input entites with internally mixed seg types (CUB_SEG / UV_CUB_SEG) are
***not allowed when conversion to uv will occur (sur_id != NULL). This 
***would destroy continuity whitin curve).
***If (sur_id != NULL) also set surface pointer in entity table (et) for all
***entities that not already have surface pointer.
*/
if (sur_id != NULL)
  {
  if (contain_MIXED_SEG==TRUE)
    {
    return(erpush("EX4252",""));
    }
  if ( DBget_pointer('I',sur_id,&ent_la,&ent_typ) < 0 ) 
    {
    return(erpush("EX1402","")); /* cant find entity in DB */
    }
  if (ent_typ != SURTYP)
    {
    IGidst(sur_id,errbuf);
    return(erpush("EX1412",errbuf));
    }
  for ( i=0; i<nref ; ++i )
    {
    if (et[i].surla == DBNULL) 
      {
      et[i].surla=ent_la;
      et[i].newuv=TRUE;
      if (fabs(et[i].ofs) > comptol) return(erpush("EX4262","")); 
      }
    }
  }

/*
***Additional checks for NURBS:
***We do not allow mixed offsets
***We do not allow mixed UV / R3 entites.
***All UV entities must point to same surface.
*/
if (contain_NURB_SEG||contain_UV_NURB_SEG) 
  {
  if (et[0].mixedofs==TRUE) return(erpush("EX4262","")); /*mixed offsets*/
  for ( i=1; i<nref ; ++i )
    {
    if (et[i].mixedofs==TRUE) return(erpush("EX4262","")); 
    if (fabs(et[i].ofs-et[i].ofs) > comptol) return(erpush("EX4262","")); 
    }
  if (contain_UV)
    {
    if (contain_R3 )   
      return(erpush("EX4232","")); /*mixed UV/R3*/
    for ( i=1; i<nref ; ++i )
      {
      if (et[i].surla != et[0].surla)
      return(erpush("EX4242","")); /*mixed surface pointers*/
      }
    }
  }
/*
***Asume this is not a planar curve
*/
    newcur.plank_cu = FALSE;
/*
***It will be one or two passes. The first pass create the
***geometrical segments.
***If the flag 'pass2' is set, the second pass will build
***up the graphical representation.
*/
    pass2 = FALSE;
/*
***Start pass 1.
***Allocates memory for a nummber of new geaometric segments.
*/
    newgeo = DBcreate_segments(DSGANT);
    allant = DSGANT;
    nseg   = 0;
/*
***Get geometry-data for entities and merge them
***to a new curve.
*/
    for ( i=0; i<nref ; ++i )
      {
      switch ( et[i].typ )
         {
/*
***Line.
*/
         case (LINTYP):
         if ( allant < nseg+1 )
           {
           newgeo = DBadd_segments(newgeo,allant+DSGANT);
           allant += DSGANT;
           }
         DBread_line(&lin,et[i].la);
         if (et[i].newuv==TRUE)
           {
           if (lin.crd1_l.x_gm < uvminmax.xmin || lin.crd1_l.x_gm > uvminmax.xmax ||
               lin.crd1_l.y_gm < uvminmax.ymin || lin.crd1_l.y_gm > uvminmax.ymax ||
               lin.crd2_l.x_gm < uvminmax.xmin || lin.crd2_l.x_gm > uvminmax.xmax ||
               lin.crd2_l.y_gm < uvminmax.ymin || lin.crd2_l.y_gm > uvminmax.ymax)
               {
               IGidst(&ridvek[i],errbuf);
               return(erpush("EX4302",errbuf));
               }
           if (lin.crd1_l.z_gm + comptol < 0 || lin.crd1_l.z_gm - comptol > 0 ||
               lin.crd2_l.z_gm + comptol < 0 || lin.crd2_l.z_gm - comptol > 0)
               {
               IGidst(&ridvek[i],errbuf);
               return(erpush("EX4312",errbuf));
               } 
           }
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
         if (et[i].newuv==TRUE)
           { 
           tmpcur.ns_cu=arc.ns_a;
           tmpcur.hed_cu.type = CURTYP;
           tmpcur.plank_cu = FALSE;
           tmpcur.al_cu = 0.0;
           varkon_cur_bound(&tmpcur,arcseg,&p_cbox);
           if (p_cbox.xmin < uvminmax.xmin || p_cbox.xmax > uvminmax.xmax ||
               p_cbox.ymin < uvminmax.ymin || p_cbox.ymax > uvminmax.ymax)
             {
             IGidst(&ridvek[i],errbuf);
             return(erpush("EX4302",errbuf));
             }
           if (p_cbox.zmin + comptol < 0 || p_cbox.zmax - comptol > 0)
             {
             IGidst(&ridvek[i],errbuf);
             return(erpush("EX4312",errbuf));
             }
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
         if (et[i].newuv==TRUE)
           {
           varkon_cur_bound(&oldcur,oldgeo,&p_cbox);
           if (p_cbox.xmin < uvminmax.xmin || p_cbox.xmax > uvminmax.xmax ||
               p_cbox.ymin < uvminmax.ymin || p_cbox.ymax > uvminmax.ymax)
             {
             IGidst(&ridvek[i],errbuf);
             return(erpush("EX4302",errbuf));
             }
           if (p_cbox.zmin + comptol < 0 || p_cbox.zmax - comptol > 0)
             {
             IGidst(&ridvek[i],errbuf);
             return(erpush("EX4312",errbuf));
             }
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
***Not allowed type of entity
*/
         default:
         return(erpush("EX1412","CUR_COMP"));
         }
/*
***How many geometric segments so far?
*/
      nseg += et[i].ngeoseg;
      }
/*
***Typ of entity and number of geometric segments.
*/
    newcur.hed_cu.type = CURTYP;
    newcur.ns_cu       = nseg;
/*
***Calculate C-addresses to the first segement of each entity.
***NB! This can be done now, since we know the final value of newgeo.
***Reallocation (DBadd_segments()) changes the value of newgeo ! 1997-05-25
*/
    for ( nseg=i=0; i<nref; ++i )
      {
      et[i].pgeoseg = newgeo + nseg;
      nseg += et[i].ngeoseg;
      }
/*
****if sur_id != NULL, call makeuv() to convert all R3 segments to UV.
*/ 
    if (sur_id != NULL)
      {
      pass2=TRUE; /* we will need to create new graphical rep. */
      status = makeuv((DBint)nref,&newcur,et,uvminmax,sur.typ_su);
      if ( status < 0 ) goto end; 
      }
/*
***Check that the curves are connected and reverse them if needed.
*/
    status = check((DBint)nref,&newcur,et);
    if ( status < 0 ) goto end;
/*
***If contain_NURB_SEG || contain_UV_NURB_SEG
***call mergeNURBS()
*/
    if (contain_NURB_SEG || contain_UV_NURB_SEG)
      {
      status=mergeNURB((DBint)nref,&newcur,et);
      if ( status < 0 ) goto end;  
      }
/*
    if ( pnp->blank==0 )   !!!TODO tepmorary always create graph rep
      {                    !!!will else be missed by offset and tcopy     
*/

/*
***Start pass 2, creation of graphical representation.
*/
      if (pass2 )
        {
        newgra = DBcreate_segments(DSGANT);
        allant = DSGANT;
        nseg   = 0;
/*
***Loop through etities and create graphical representation.
*/
        for ( i=0; i<nref ; ++i )
          {
/*
***If this is an entity converted to uv (cubic or nurb),
***call varkon_sur_scur_gra() to create graphical representation.
*/
          if (et[i].newuv)
            {
            tmpcur.ns_cu=et[i].ngeoseg;
            tmpcur.hed_cu.type = CURTYP;
            tmpcur.plank_cu = FALSE;
            tmpcur.al_cu = 0.0;
/*
*** memory for graphical segments is allocated by GEcur_cre_gra()
*** and need to be returned at the end of this operation
*/
            tmpgra=NULL;
            status = GEcur_cre_gra(&tmpcur,et[i].pgeoseg,&tmpgra);

            if (status==0 && tmpcur.nsgr_cu>0)
              {
              if ( allant < tmpcur.nsgr_cu + nseg)
                {
                newgra = DBadd_segments(newgra,tmpcur.nsgr_cu + nseg +DSGANT);
                allant = tmpcur.nsgr_cu + nseg + DSGANT;
                }
              V3MOME( tmpgra, newgra+nseg , tmpcur.nsgr_cu * sizeof(DBSeg) ); 
              nseg += tmpcur.nsgr_cu;         
              DBfree_segments(tmpgra);
              }

            }
/*
***Else treat the entities dependent on type
*/
          else
            {  
            switch ( et[i].typ )
              {
/*
***For lines and arcs, we use the geometrical segments also as 
***graphical representation.
***But ... they may have been converted to NURBS_SEG.
***In the case of UV segs we are creating new garph rep
***and will not reach here
*/
              case (LINTYP):
              case (ARCTYP):

              if(contain_NURB_SEG)
                {
                for(j=0; j< et[i].ngeoseg; j++)
                  {
                  et[i].pgeoseg[j].typ=CUB_SEG;
                  }
                }

              if ( allant < nseg+et[i].ngeoseg )
                {
                newgra = DBadd_segments(newgra,allant+DSGANT);
                allant += DSGANT;
                }
              V3MOME(et[i].pgeoseg,newgra+nseg,et[i].ngeoseg*sizeof(DBSeg));
              if(contain_NURB_SEG)
                {
                for(j=0; j< et[i].ngeoseg; j++)
                  {
                  et[i].pgeoseg[j].typ=NURB_SEG;
                  }
                }
              nseg += et[i].ngeoseg;
              break;
/*
***For curves we use the original graphical representation.
***If the curve is reversed, also the graphical segments must be reversed.
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
        }
/*
***If no pass 2 is needed graphical and geometrical representation 
***are the same.
*/
      else newgra = newgeo;
/*
***End pass 2. How many graphical segments ?
*/
      newcur.nsgr_cu = nseg;
/*      }   */
/*
***Else this was a blanked curve
*/
/*  else 
      {
      newgra = NULL;
      newcur.nsgr_cu =0;
      }*/

/*
***If we now have a NURBS curve, store cpts and knots in DB 
***and update the segments DB pointers to them.
***The other data are filled in before.
*/
    if (contain_NURB_SEG || contain_UV_NURB_SEG) 
      {
      status = DBwrite_nurbs(newgeo->cpts_c, newgeo->ncpts, newgeo->knots_c, 
                          newgeo->nknots, &cpts_la, &knots_la);
      if ( status < 0 ) return(erpush("EX4212",""));
      for ( i=0; i<nseg; ++i )
        {
        (newgeo+i)->cpts_db      = cpts_la;
        (newgeo+i)->knots_db     = knots_la;
        }
      }
/*
***Store in DB and draw.
*/
    status = EXecur(id,&newcur,newgra,newgeo,pnp);
/*
***Dealocate momory.
*/
    if ( newgra != NULL && newgra !=newgeo) DBfree_segments(newgra);
end:
    if (contain_NURB_SEG || contain_UV_NURB_SEG) 
      {
      DBfree_nurbs(newgeo[0].knots_c, newgeo[0].cpts_c);
      }
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
***If this is NURB segment, use reverseNURB() instead
*/
   if (seg[0].typ==NURB_SEG || seg[0].typ==UV_NURB_SEG)
     return(reverseNURB(cur,seg));
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
 *       23/11/94 UV_CUB_SEG, G. Liden
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
    short     status;
    DBint     i;
    DBptr     la;
    DBetype   typ;
    DBCurve   cur;
    DBSeg    *graseg;
    DBSurf    sur;
    DBPatch  *p_pat=NULL;

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
          geoseg[i].typ      = UV_CUB_SEG;
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





/*!******************************************************/

 static short    getseginfo(
        DBptr    curla,
        DBshort *segtype,
        DBptr   *surla,
        DBfloat *ofs,
        bool    *mixedofs)

/*      Get segment type for first segment in a curve
 *      If it is a uv-seg, also get logical adress to surface.
 *
 *      In:  curla   => logical adress to the curve 
 *
 *      Out: segtype   => returned segment type for first segment
 *           surla     => la to surface if first seg is a uv seg
 *           ofs       => offset for first segment (0 if not ofset seg)
 *           mixedofs  => true if the curve has mixed offsets
 *
 *      (c)2006-10-06, S. Larsson, Örebro university
 *
 ******************************************************!*/

  {
    DBCurve  cur;
    DBSeg    *segpk,*grasegpk;
    int      segindex;
    DBfloat   comptol;

    comptol   = varkon_comptol();
    
    DBread_curve(&cur,&grasegpk,&segpk,curla);
    if ( grasegpk != segpk ) DBfree_segments(grasegpk);

    *segtype = segpk[0].typ; 
    *ofs     = segpk[0].ofs;
    *mixedofs = FALSE;
            
    if (*segtype==CUB_SEG || *segtype==UV_CUB_SEG) /* these can be mixed */
      { 
      for ( segindex=1; segindex < cur.ns_cu; segindex++)  
        {
        if(*segtype != segpk[segindex].typ)
          {
          *segtype = MIXED_SEG;
          }
        if(fabs(*ofs - segpk[segindex].ofs) > comptol)
          {
          *mixedofs = TRUE;
          }          
        }   
      }
   
    if (*segtype==UV_CUB_SEG || *segtype==UV_NURB_SEG)
      {
      *surla   = segpk->spek_gm;
      }
    else
      {
      *surla   = DBNULL;
      }
    DBfree_segments(segpk);
    return(0);
  }


  
/*!******************************************************/

 static short     makeuv(
        DBint     ncur,
        DBCurve  *comp_cur,
        ENTTAB    et[],
        BBOX      uvminmax,
        short     typ_su)

/*      Convert segments to UV_CUB_SEG or UV_NURB_SEG
 *
 *      In: ncur     => Number of curves
 *          comp_cur => The new composite curve
 *          et       => entity table
 *
 *      Out: -
 *
 *      FV:       0 => Ok.
 *               <0 => Fel.
 *
 *      (c)2006-10-05, S. Larsson, Örebro university
 *
 ******************************************************!*/

  {
   DBSeg  *segpek;
   DBfloat g11,g12,g13,g14,g21,g22,g23,g24,g41,g42,g43,g44;
   
   int     i,segindex,c;

   for ( i=0; i<ncur; ++i )
     {
     for ( segindex=0; segindex<et[i].ngeoseg; segindex++)
       {
       segpek=et[i].pgeoseg + segindex;
       
       
       if (segpek->typ==CUB_SEG)
         {
         if (typ_su != NURB_SUR)
           {
/*  
*** transformation of segment to varkon internal surface parameterisation
*** transform (1,1,0)
*/     
           g11 = segpek->c0x;
           g12 = segpek->c1x;
           g13 = segpek->c2x;
           g14 = segpek->c3x;

           g21 = segpek->c0y;
           g22 = segpek->c1y;
           g23 = segpek->c2y;
           g24 = segpek->c3y;

           g41 = segpek->c0;
           g42 = segpek->c1;
           g43 = segpek->c2;
           g44 = segpek->c3;

           segpek->c0x  = g11 + g41;
           segpek->c1x =  g12 + g42;
           segpek->c2x =  g13 + g43;
           segpek->c3x =  g14 + g44;

           segpek->c0y =  g21 + g41;
           segpek->c1y =  g22 + g42;
           segpek->c2y =  g23 + g43;
           segpek->c3y =  g24 + g44;
           }       
         segpek->typ      = UV_CUB_SEG;
         segpek->subtyp   = 1; /* one surface */
         segpek->spek_gm  = et[i].surla;
         segpek->spek2_gm = DBNULL;
         segpek->sl =0.0;

         }
       else if (segpek->typ==NURB_SEG)
         {
         if (typ_su != NURB_SUR)
           {
/*
*** transformation of cpts to varkon internal surface parameterisation
*** transform (1,1,0)
*/
           if (segindex==0)
             {
             for (c=0; c < segpek->ncpts; c++)
               {
               segpek->cpts_c[c].x_gm = segpek->cpts_c[c].x_gm + 1.0;
               segpek->cpts_c[c].y_gm = segpek->cpts_c[c].y_gm + 1.0;
               segpek->cpts_c[c].z_gm = 0.0;
               }
             }
           }
         segpek->typ      = UV_NURB_SEG;
         segpek->subtyp   = 1; /* one surface */
         segpek->spek_gm  = et[i].surla;
         segpek->spek2_gm = DBNULL;
         segpek->sl =0.0;
         }
       }
     }
  return(0);
  }   

  
/*!******************************************************/

 static short  reverseNURB(
        DBCurve *cur,
        DBSeg *seg)

/*      Reverses a NURBS curve
 *
 *      In:  cur => The curve
 *           seg => The segments
 *
 *      Out: -
 *
 *      (c)2006-10-05, S. Larsson, Örebro university
 *
 ******************************************************!*/

  {
  DBHvector  tmpcpt;
  DBfloat   *pknots;
  int        i;
  int        ncpts,nknots;
  DBint      *offset_cpts;
  DBshort    nseg,mseg;

/*
*** Reverse control points
*/   
  ncpts=seg->ncpts;
  for ( i=0; i < floor(ncpts*0.5+0.001); i++ )
    {
    tmpcpt.x_gm = seg->cpts_c[i].x_gm;
    tmpcpt.y_gm = seg->cpts_c[i].y_gm;      
    tmpcpt.z_gm = seg->cpts_c[i].z_gm;      
    tmpcpt.w_gm = seg->cpts_c[i].w_gm;      
      
    seg->cpts_c[i].x_gm = seg->cpts_c[ncpts-i-1].x_gm;
    seg->cpts_c[i].y_gm = seg->cpts_c[ncpts-i-1].y_gm;      
    seg->cpts_c[i].z_gm = seg->cpts_c[ncpts-i-1].z_gm;      
    seg->cpts_c[i].w_gm = seg->cpts_c[ncpts-i-1].w_gm;       
      
    seg->cpts_c[ncpts-i-1].x_gm = tmpcpt.x_gm;
    seg->cpts_c[ncpts-i-1].y_gm = tmpcpt.y_gm;    
    seg->cpts_c[ncpts-i-1].z_gm = tmpcpt.z_gm;  
    seg->cpts_c[ncpts-i-1].w_gm = tmpcpt.w_gm;
    }
/*
*** Reverse knot vector
*/
  nknots=seg->nknots;
  if ( (pknots=v3mall(nknots*sizeof(DBfloat),"reverseNURB")) == NULL )
      erpush("EX4222","");
  V3MOME( seg[0].knots_c, pknots , nknots*sizeof(DBfloat) );  
  for ( i=0; i<nknots; ++i )
    {
    seg->knots_c[i] = pknots[nknots-1] - pknots[nknots-1-i];
    }
  v3free(pknots,"reverseNURB");    
/*
***recalculate offset pointers
*/
  nseg= cur->ns_cu;
  mseg=nseg-1;
  if ( (offset_cpts=v3mall(nseg*sizeof(DBint),"reverseNURB")) == NULL )
      erpush("EX4222","");
  for ( i=0; i<nseg; i++ )  offset_cpts[i]=seg[i].offset_cpts; 
  for ( i=0; i<nseg; i++ ) 
    {
    seg[i].offset_cpts = offset_cpts[mseg]-offset_cpts[mseg-i];
    seg[i].offset_knots = seg[i].offset_cpts + seg[i].nurbs_degree;
    }    
  v3free(offset_cpts,"reverseNURB"); 
  return(0);
  }
  
  
  /*!******************************************************/

 static short     mergeNURB(
        DBint     ncur,
        DBCurve  *comp_cur,
        ENTTAB    et[])

/*      Merge curves to one NURBS curve
 *
 *      In: ncur     => Number of input curves
 *          comp_cur => The new composite curve
 *          et       => Entity table
 *
 *      Ut: -
 *
 *      (c)2006-10-05, S. Larsson, Örebro university
 *
 ******************************************************!*/

  {
   int        i,j,s,cpts_offs,ncpts,nknots,nnew_cpts,nnew_knots,maxNURBdeg,global_offs;
   DBshort    segtype;
   DBHvector *cpts_c,*new_cpts_c;            /* C-pointer to 4D controlpoints */
   DBfloat   *knots_c,*new_knots_c;  
   DBfloat    startknot,endknot;
   DBint      maxknotind;
   DBfloat   comptol;

   comptol   = varkon_comptol();

/*
***convert all curves to nurbs.
***input entities are nurbs or cubic uv or R3 curves
*/
maxNURBdeg=0;
nnew_cpts=1;
for ( i=0; i<ncur; i++ )
  {

  if (et[i].pgeoseg->typ==CUB_SEG || et[i].pgeoseg->typ==UV_CUB_SEG)
    {
    if (et[i].pgeoseg->typ==CUB_SEG)    segtype=NURB_SEG;
    if (et[i].pgeoseg->typ==UV_CUB_SEG) segtype=UV_NURB_SEG;
/*
***Allocate memory for cpts and knots for this input curve
*/
    ncpts=et[i].ngeoseg*3+1;
    nknots=ncpts+4;
    nnew_cpts = nnew_cpts + ncpts - 1;

    if ( (et[i].pgeoseg[0].cpts_c = v3mall(ncpts*sizeof(DBHvector),"mergeNURB"))==NULL)
      {
      erpush("EX4222","");
      }
    if ( (et[i].pgeoseg[0].knots_c = v3mall(nknots*sizeof(DBfloat),"mergeNURB")) == NULL )
      {
      erpush("EX4222","");
      }
    cpts_c=et[i].pgeoseg[0].cpts_c;
    knots_c=et[i].pgeoseg[0].knots_c;
/*
***build knot vector
*/
    j=0;
    knots_c[0]=0;
    for ( s=0; s <= et[i].ngeoseg; s++ )
      {
        knots_c[j+1] = (DBfloat)s;
        knots_c[j+2] = (DBfloat)s;
        knots_c[j+3] = (DBfloat)s;
        j=j+3;
      }
    knots_c[nknots-1] = knots_c[nknots-2];
    if (3>maxNURBdeg) maxNURBdeg=3;

/*
***loop through segments to build cpts array and asign segment data
*/
    for ( s=0; s < et[i].ngeoseg; s++ )
      {
/*
*** Calculate cpts for this segment.
*** Segments must be connected
*/
      cpts_offs=s*3;
      GEsegcpts(FALSE,&et[i].pgeoseg[s],cpts_c+cpts_offs);
      et[i].pgeoseg[s].cpts_c        = cpts_c;
      et[i].pgeoseg[s].knots_c       = knots_c;
      et[i].pgeoseg[s].ncpts         = ncpts;
      et[i].pgeoseg[s].nknots        = nknots; 
      et[i].pgeoseg[s].offset_cpts   = cpts_offs; 
      et[i].pgeoseg[s].offset_knots  = cpts_offs + 3; 
      et[i].pgeoseg[s].nurbs_degree  = 3;
      et[i].pgeoseg[s].typ           = segtype;
      }
    }
/*
***else this entiity was already a NURBS curve
*/
  else 
    {
    nnew_cpts = nnew_cpts + et[i].pgeoseg[0].ncpts - 1;
    if (et[i].pgeoseg[0].nurbs_degree > maxNURBdeg) maxNURBdeg=et[i].pgeoseg[0].nurbs_degree; 

    maxknotind = et[i].pgeoseg[0].nknots-1;
    startknot  = et[i].pgeoseg[0].knots_c[0];
    endknot    = et[i].pgeoseg[0].knots_c[maxknotind];

    for ( j=1; j <= et[i].pgeoseg[0].nurbs_degree; j++ )
      {
      if(fabs( et[i].pgeoseg[0].knots_c[j] -  startknot) > comptol ||
         fabs( et[i].pgeoseg[0].knots_c[maxknotind-j] -  endknot) > comptol)
        {
        return(erpush("EX4282","")); /*unclamped*/
        }
      }
    }
  }

/*
***Now all input entities are NURBS curves and we can
***proceed by checking that input curves have same degree/order
***We already now the highest order: maxNURBdeg
*/

/*   TODO  Check / convert to same degree   */
/*   If conversion, also update nnew_cpts  */

for ( i=0; i<ncur; i++ )
  {
  if (et[i].pgeoseg[0].nurbs_degree != maxNURBdeg)
  return(erpush("EX4322","")); /*Degree elevation not impl*/
  }


/*
***Allocate memory for new cpts and knots
*/
  nnew_knots=nnew_cpts+maxNURBdeg+1;
  if ( (new_cpts_c = v3mall(nnew_cpts*sizeof(DBHvector),"mergeNURB"))==NULL)
    {
    erpush("EX4222","");
    } 
  if ( (new_knots_c=v3mall(nnew_knots*sizeof(DBfloat),"mergeNURB")) == NULL )
    {
    erpush("EX4222","");
    }
/*
***first elements of knot vector 
*/
  new_knots_c[0]=et[0].pgeoseg[0].knots_c[0];
  for ( i=1; i<maxNURBdeg+1; i++ )
    {
    new_knots_c[i]=new_knots_c[i-1];   
    }
/*
***Iterate through the input curves to
***build new cpts and knots, free the old ones
***and update c-pointers etc in the segments.
*/
  global_offs=0;
  for ( i=0; i<ncur; i++ )
    {
    /* copy cpts */
    V3MOME( et[i].pgeoseg[0].cpts_c , new_cpts_c+global_offs, 
            et[i].pgeoseg[0].ncpts * sizeof(DBHvector));
    /* calculate knots */
    for ( j = maxNURBdeg + 1; 
          j < maxNURBdeg + 1 + et[i].pgeoseg[0].ncpts; j++ )
      {
      new_knots_c[global_offs+j] = new_knots_c[global_offs+j-1] + 
         et[i].pgeoseg[0].knots_c[j] - et[i].pgeoseg[0].knots_c[j-1];
      }
/*
***free old cpts and knots (once per input curve)
*/
    DBfree_nurbs(et[i].pgeoseg[0].knots_c, et[i].pgeoseg[0].cpts_c);
/*
***update segments
*/
    ncpts = et[i].pgeoseg[0].ncpts;
    for ( s=0; s<et[i].ngeoseg; s++ )
      {
      et[i].pgeoseg[s].cpts_c        = new_cpts_c;
      et[i].pgeoseg[s].knots_c       = new_knots_c; 
      et[i].pgeoseg[s].ncpts         = nnew_cpts;
      et[i].pgeoseg[s].nknots        = nnew_knots;  
      et[i].pgeoseg[s].offset_cpts   = et[i].pgeoseg[s].offset_cpts + global_offs;
      et[i].pgeoseg[s].offset_knots  = et[i].pgeoseg[s].offset_knots + global_offs;
      }
      global_offs = global_offs + ncpts -1;
    }
  return(0);
  }


