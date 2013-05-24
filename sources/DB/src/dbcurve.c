/**********************************************************************
*
*    dbcurve.c
*    =========
*
*
*    This file includes the following public functions:
*
*       DBinsert_curve();    Inserts a curve entity
*       DBread_curve();      Reads a curve entity
*       DBupdate_curve();    Updates a curve entity
*       DBdelete_curve();    Deletes a curve entity
*
*      *DBcreate_segments(); Allocates C-memory for curve segments
*      *DBadd_segments();    Increases C-allocated memory for curve segments
*       DBfree_segments();   Free's C-memory allocated for curve segments
*
*       DBwrite_nurbs();     Stores NURBS-curve data in DB
*       DBread_nurbs();      Reads NURBS-curve data from DB
*       DBfree_nurbs();      Free's C-memory allocated for NURBS-data
*       DBdelete_nurbs();    Deletes NURBS-data from DB
*
*    This file is part of the VARKON Database Library.
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
*    2004-09-02 DBread_nurbs static->public, S. Larsson, Örebro university
*    2006-10-20 Added free/delete NURBS, J. Kjellander, Örebro university
*
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"


/*
***The DB internal representation of a curve is currently
***GMCUR3 introduced 1997-12-27. Older versions are defined
***in olddefs.h and are kept only for backward compatibility.
*/

typedef struct gmcur3       /* GMCUR version 3 */
{
GMRECH  hed_nc;             /* Header */
DBshort fnt_nc;             /* Font */
DBfloat lgt_nc;             /* Dash length */
DBfloat al_nc;              /* Curve arc length */
DBshort nges_nc;            /* Number of geo segments */
DBptr   pges_nc;            /* Ptr to 1:st geo segment */
DBptr   cptr_nc;            /* Ptr to curve plane */
DBptr   pcsy_nc;            /* Ptr to coordinate system */
DBshort ngrs_nc;            /* Number of graphical segments */
DBptr   pgrs_nc;            /* Ptr to 1:st graph segment */
DBfloat wdt_nc;             /* Width */
} GMCUR3;

static DBstatus gmrdoc(GMCUR *, GMSEG **, DBptr, DBint);
static DBstatus gmrdos(DBptr, GMSEG *, DBint);
static DBstatus gmupoc(GMCUR *, GMSEG *, DBptr, DBint);
static DBstatus gmdloc(DBptr, DBint);

/*!******************************************************/

        DBstatus DBinsert_curve(
        DBCurve *curpek,
        DBSeg   *grsegp,
        DBSeg   *gesegp,
        DBId    *idpek,
        DBptr   *lapek)

/*      Main entry for curve insert. Stores curve data and 
 *      associated segments as 2 linked lists. If grsegp ==
 *      gesegp only one list of segments is stored.
 *
 *      In:  curpek => Ptr to curve.
 *           grsegp => Ptr to array of graphical segments.
 *           gesegp => Ptr to array of geometric segments.
 *           idpek  => Ptr to curve ID.
 *           lapek  => Ptr to output.
 *
 *      Out: *lapek => Curve DB adress.
 *
 *      Return:  0  => Ok.
 *              -1  => Invalid ID
 *              -2  => IDTAB full.
 *              -3  => No space in DB.
 *              -4  => Entity already exists.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      18/3/92  GMCUR1, J. Kjellander
 *      5/3/93   GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *      2006-10-20 English, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBint  i;
    GMCUR3 cur3;

/*
***Save geometrical segments, the last segment first. Last segment
***link = DBNULL.
*/
    *lapek = DBNULL;
    for ( i=curpek->ns_cu-1; i >= 0; --i)
      {
      (gesegp+i)->nxt_seg = *lapek;
      if ( wrdat1((char *)&gesegp[i],lapek,sizeof(DBSeg)) < 0 ) return(-3);
      }
    cur3.nges_nc =  curpek->ns_cu;
    cur3.pges_nc = *lapek;
/*
***Save graphical segments, if they exist.
*/
    if ( grsegp == gesegp )
      {
      cur3.ngrs_nc = cur3.nges_nc;
      cur3.pgrs_nc = cur3.pges_nc;
      }
    else
      {
      *lapek = DBNULL;
      for ( i=curpek->nsgr_cu-1; i >= 0; --i)
        {
        (grsegp+i)->nxt_seg = *lapek;
        if ( wrdat1((char *)&grsegp[i],lapek,sizeof(DBSeg)) < 0 ) return(-3);
        }
      cur3.ngrs_nc =  curpek->nsgr_cu;
      cur3.pgrs_nc = *lapek;
      }
/*
***Type specific data.
*/
    curpek->hed_cu.type = CURTYP;   /* Type = curve */
    curpek->hed_cu.vers = GMPOSTV3; /* Version */
/*
***Copy curve data to internal GMCUR3.
*/
    V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(DBHeader));
    cur3.fnt_nc  = curpek->fnt_cu;
    cur3.lgt_nc  = curpek->lgt_cu;
    cur3.al_nc   = curpek->al_cu;
    cur3.pcsy_nc = curpek->pcsy_cu;
    cur3.wdt_nc  = curpek->wdt_cu;
/*
***Save curve plane data if supplied.
*/
    if ( curpek->plank_cu )
      {
      if ( wrdat1( (char *) &curpek->csy_cu, lapek,
             sizeof(DBTmat)) < 0 ) return(-3);
      cur3.cptr_nc = *lapek;
      }
    else cur3.cptr_nc = DBNULL;
/*
***Save the Curve data.
*/
    return(inpost((DBAny *)&cur3,idpek,lapek,sizeof(GMCUR3)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_curve(
        DBCurve *curpek,
        DBSeg  **graseg,
        DBSeg  **geoseg,
        DBptr    la)

/*      Main entry for curve read. Returns curve data 
 *      and segments if gra/geo-seg <> NULL.
 *
 *      In: curpek => Ptr to curve.
 *          graseg => Ptr to ptr for graphical segments.
 *          geoseg => Ptr to ptr for geometrical segments.
 *          la     => Curve address in DB.
 *
 *      Out: *curpek => Curve data.
 *           *graseg => Ptr to graphical segments.
 *           *geoseg => Ptr to geometrical segments.
 *
 *      Return:  0 = Ok.
 *              <0 = Can't allocate C memory.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      18/3/92  GMCUR1, J. Kjellander
 *      13/5/92  segmnt = NULL, J. Kjellander
 *      18/2/93  GMCUR2, J. Kjellander
 *      7/6/93   Dynamisk mallokering, J. Kjellander
 *      10/1/94  Bug antal grafiska segment, J. Kjellander
 *      18/4/94  Läsning av både gr och ge, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *      2002-05-23 NURBS, Johan Kjellander, Örebro University
 *      2006-10-20 English, J.Kjellander, Örebro university
 *      2007-01-22 Bug reading cpts_c, Sören L
 *
 ******************************************************!*/

  {
    char       errbuf[20];
    DBptr      la_seg;
    DBint      i,vers;
    bool       graflg,geoflg;
    DBHeader  *hedpek;
    GMCUR3     cur3;
    DBSeg     *segptr;
    DBptr      last_cpts_db = DBNULL;
    DBHvector *last_cpts_c;
    DBfloat   *last_knots_c;

/*
***C-ptr to curve and version number.
*/
    hedpek = (DBHeader *)gmgadr(la);
    vers   =  GMVERS(hedpek);
/*
***Read main curve data.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      V3MOME(hedpek,(char *)&cur3,sizeof(GMCUR3));
      break;

      case GMPOSTV2:
      V3MOME(hedpek,(char *)&cur3,sizeof(GMCUR2));
      cur3.wdt_nc = 0.0;
      break;
      }
/*
***Allocate C-memory for zero, one or two lists.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      case GMPOSTV2:

      if ( graseg != NULL  &&  geoseg != NULL  &&
           cur3.pgrs_nc == cur3.pges_nc )
        {
       *graseg = *geoseg = DBcreate_segments(cur3.ngrs_nc);
        graflg = TRUE;
        geoflg = FALSE;
        }
      else
        {
        if ( graseg != NULL )
          {
         *graseg = DBcreate_segments(cur3.ngrs_nc);
          graflg = TRUE;
          }
        else graflg = FALSE;

        if ( geoseg != NULL )
          {
         *geoseg = DBcreate_segments(cur3.nges_nc);
          geoflg = TRUE;
          }
        else geoflg = FALSE;
        }
/*
***Read graphical segments.
***graseg = NULL => no segments read.
*/
      if ( graflg )
        {
        segptr = *graseg;
        la_seg = cur3.pgrs_nc;
        for ( i=0; i < cur3.ngrs_nc; ++i)
          {
          rddat1((char *)segptr,la_seg,sizeof(DBSeg));
          la_seg = segptr->nxt_seg;
          ++segptr;
          }
        }
/*
***Read geometrical segments.
***geoseg = NULL => no segments read.
*/
      if ( geoflg )
        {
        segptr = *geoseg;
        la_seg = cur3.pges_nc;
        for ( i=0; i < cur3.nges_nc; ++i)
          {
          rddat1((char *)segptr,la_seg,sizeof(DBSeg));
          la_seg = segptr->nxt_seg;
/*
***If the segment is a NURBS span, read the array of controlpoints
***and the corresponding knot vector. If these already exist, just
***update their C-pointers.
*/
          if ( segptr->typ == NURB_SEG || segptr->typ == UV_NURB_SEG )
            {
             if ( segptr->cpts_db == last_cpts_db )
               {
               segptr->cpts_c = last_cpts_c;
               segptr->knots_c = last_knots_c;
               }
             else
               {
               if ( (segptr->cpts_c=v3mall(segptr->ncpts*sizeof(DBHvector),"DBread_segments")) == NULL )
                 {
                 sprintf(errbuf,"%d",(int)segptr->ncpts);
                 erpush("GM1083",errbuf);
                 }
               last_cpts_c = segptr->cpts_c;
               last_cpts_db = segptr->cpts_db;  /* line added 2007-01-22 */
               if ( (segptr->knots_c=v3mall(segptr->nknots*sizeof(DBfloat),"DBread_segments")) == NULL )
                 {
                 sprintf(errbuf,"%d",(int)segptr->nknots);
                 erpush("GM1093",errbuf);
                 }
               last_knots_c = segptr->knots_c;

               DBread_nurbs(segptr->cpts_c, segptr->ncpts, segptr->knots_c,
                            segptr->nknots, segptr->cpts_db, segptr->knots_db);
               }
            }
         ++segptr;
          }
        }

/*
***Read curve plane data if existent.
*/
      if ( cur3.cptr_nc != DBNULL )
        {
        rddat1( (char *) &curpek->csy_cu, cur3.cptr_nc, sizeof(DBTmat));
        curpek->plank_cu = TRUE;
        }
      else curpek->plank_cu = FALSE;
/*
***Main curve data.
*/
      V3MOME(&cur3.hed_nc,&curpek->hed_cu,sizeof(DBHeader));
      curpek->fnt_cu  = cur3.fnt_nc;
      curpek->lgt_cu  = cur3.lgt_nc;
      curpek->al_cu   = cur3.al_nc;
      curpek->ns_cu   = cur3.nges_nc;
      curpek->nsgr_cu = cur3.ngrs_nc;
      curpek->pcsy_cu = cur3.pcsy_nc;
      curpek->wdt_cu  = cur3.wdt_nc;
      break;
/*
***Older versions for compatibility.
*/
      default:
      if ( geoseg != NULL ) gmrdoc(curpek,geoseg,la,vers);
      else                  gmrdoc(curpek,graseg,la,vers);
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_curve(
        DBCurve *curpek,
        DBSeg   *segmnt,
        DBptr    la)

/*      Main entry for curve update. Updates main
 *      curve and geometrical segment data but does
 *      NOT update graphical segments, curve plane or
 *      NURBS data. Note also that this routine can
 *      only update a curve whith the same number of
 *      segments as the original curve.
 *      
 *      Curve update is  used by Varkon in explicit
 *      mode interactive 2D editing (move and rotate)
 *      and in all modes during editing of curve
 *      attributes CFONT, CDASHL and WIDTH. In both
 *      cases the number of segments does not change.
 *
 *      Support for graphical segments or NURBS data is
 *      not implemented. This is a bug if NURBS or UV-curves
 *      are moved or rotated in 2D explicit mode (not likely
 *      to happen though).
 *      
 *      In: curpek => Ptr to curve.
 *          segmnt => Ptr to array of geometrical segments.
 *          la     => Curve address in DB.
 *
 *      Out: Nothing.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av segment, J. Kjellander
 *      18/3/92  GMCUR1, J. Kjellander
 *      18/2/93  GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *      2006-10-20 English, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
     DBptr     la_seg;
     DBint     i,vers;
     GMCUR3    cur3;
     DBSeg     seg;
     DBHeader *hedpek;


/*
***C-ptr to curve and version number.
*/
    hedpek = (DBHeader *)gmgadr(la);
    vers   =  GMVERS(hedpek);
/*
***Read main curve data for old curve.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      rddat1((char *)&cur3,la,sizeof(GMCUR3));
      break;

      case GMPOSTV2:
      rddat1((char *)&cur3,la,sizeof(GMCUR2));
      cur3.wdt_nc = 0.0;
      break;
      }
/*
***Update.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      case GMPOSTV2:
/*
***Update geometrical segments.
*/
      if ( segmnt != NULL && cur3.nges_nc > 0 )
        {
        la_seg = cur3.pges_nc;
        for ( i=0; i<cur3.nges_nc; ++i )
          {
          rddat1((char *)&seg,la_seg,sizeof(DBSeg));
          updata((char *)&segmnt[i],la_seg,sizeof(DBSeg));
          la_seg = seg.nxt_seg;
          }
        }
      }
/*
***Update main curve data CFONT, CDASHL and WIDTH. The
***rest is unchanged.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(DBHeader));
      cur3.fnt_nc  = curpek->fnt_cu;
      cur3.lgt_nc  = curpek->lgt_cu;
      cur3.wdt_nc  = curpek->wdt_cu;
      updata((char *)&cur3, la, sizeof(GMCUR3));
      break;

      case GMPOSTV2:
      V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(DBHeader));
      cur3.fnt_nc  = curpek->fnt_cu;
      cur3.lgt_nc  = curpek->lgt_cu;
      updata((char *)&cur3, la, sizeof(GMCUR2));
      break;
/*
***Older curves.
*/
      default:
      gmupoc(curpek,segmnt,la,vers);
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_curve(DBptr la)

/*      Main entry for curve delete.
 *
 *      In:  la => Curve address in DB.
 *
 *      Out: Nothing.
 *
 *      FV:  Always 0.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      18/3/92  GMCUR1, J. Kjellander
 *      18/2/93  GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *      2006-10-20 NURBS, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    GMCUR3    cur3;
    DBHeader *hedpek;
    DBSeg     seg;
    DBint     i,vers;
    DBptr     la_seg;

/*
***C-ptr to curve and version number.
*/
    hedpek = (DBHeader *)gmgadr(la);
    vers   = GMVERS(hedpek);
/*
***Read main curve data and release it's space in DB.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      rddat1((char *)&cur3,la,sizeof(GMCUR3));
      rldat1(la,sizeof(GMCUR3));
      break;

      case GMPOSTV2:
      rddat1((char *)&cur3,la,sizeof(GMCUR2));
      rldat1(la,sizeof(GMCUR2));
      break;
      }
/*
***Release space for NURBS data. A Varkon curve can only have
***segments of the same type so if the first segment is NURBS
***the curve is NURBS.
*/
    if ( cur3.nges_nc > 0 )
      {
      rddat1((char *)&seg,cur3.pges_nc,sizeof(DBSeg));

      if ( seg.typ == NURB_SEG  ||  seg.typ == UV_NURB_SEG )
        {
        DBdelete_nurbs(&seg);
        }
      }
/*
***Release space for geometrical segments.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      case GMPOSTV2:
      la_seg = cur3.pges_nc;
      for ( i=0; i<cur3.nges_nc; ++i )
        {
        rddat1((char *)&seg,la_seg,sizeof(DBSeg));
        rldat1(la_seg,sizeof(DBSeg));
        la_seg = seg.nxt_seg;
        }
/*
***Release space for graphical segments. Graphical segments can only be CUB_SEG.
*/
      if ( cur3.pgrs_nc != cur3.pges_nc )
        {
        la_seg = cur3.pgrs_nc;
        for ( i=0; i<cur3.ngrs_nc; ++i )
          {
          rddat1((char *)&seg,la_seg,sizeof(DBSeg));
          rldat1(la_seg,sizeof(DBSeg));
          la_seg = seg.nxt_seg;
          }
        }
/*
***Release space for curve plane.
*/
      if ( cur3.cptr_nc != DBNULL ) rldat1(cur3.cptr_nc,sizeof(DBTmat));
      break;
/*
***Older curves.
*/
      default:
      gmdloc(la,vers);
      break;
      }

  return(0);
  }

/********************************************************/
/*!******************************************************/

        GMSEG *DBcreate_segments(DBint antal)

/*      Allokerar plats i primärminne för segment-data.
 *
 *      In: antal = Antal segment.
 *
 *      Ut: Inget.
 *
 *      FV: C-adress till ledig minnesarea.
 *
 *      Felkoder: GM1053 = Kan ej allokera minne
 *
 *      (C)microform ab 20/2/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char *ptr,errbuf[80];

/*
***Allokera minne.
*/
    if ( (ptr=v3mall(antal*sizeof(GMSEG),"DBcreate_segments")) == NULL )
      {
      sprintf(errbuf,"%d",(int)antal);
      erpush("GM1053",errbuf);
      }
/*
***Slut.
*/
    return((GMSEG *)ptr);
  }

/********************************************************/
/*!******************************************************/

        GMSEG *DBadd_segments(GMSEG *segpek, DBint antal)

/*      Reallokerar plats i primärminne för segment-data.
 *
 *      In: segpek = Pekare till redan allokerad area.
 *          antal  = Antal segment (inkl. de ursprungliga).
 *
 *      Ut: Inget.
 *
 *      FV: C-adress till utökad minnesarea.
 *
 *      Felkoder: GM1053 = Kan ej allokera minne
 *
 *      (C)microform ab 20/2/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char *ptr,errbuf[80];

/*
***Allokera minne.
*/
    if ( (ptr=v3rall((char *)segpek,antal*sizeof(GMSEG),"DBadd_segments")) == NULL )
      {
      sprintf(errbuf,"(reallokera) %d",(int)antal);
      erpush("GM1053",errbuf);
      }
/*
***Slut.
*/
    return((GMSEG *)ptr);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBfree_segments(GMSEG *ptr)

/*      Återlämnar tidigare allokerat minne.
 *
 *      In: ptr => C-pekare till minnesarea.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 20/2/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***C-rutinen free() är en void och man kan inte få
***reda på om det gick bra eller inte.
*/
    v3free((char *)ptr,"DBfree_segments");
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBwrite_nurbs(
        DBHvector *cpts,
        DBint      ncpts,
        DBfloat   *knots,
        DBint      nknots,
        DBptr     *cpts_la,
        DBptr     *knots_la)

/*      Stores NURBS curve data in DB.
 *
 *      In: .
 *
 *      Out: cpts_la = DB pointer to cpts data
 *           knots_la = DB pointer to knots data
 *
 *      FV:  0  => Ok.
 *
 *      (C)2002-05-16 J. Kjellander, Örebro University
 *
 ******************************************************!*/

  {
   DBint nbytes;
   DBstatus status;

/*
***Store cpts.
*/
   nbytes = ncpts * sizeof(DBHvector);

   if ( nbytes <= PAGSIZ ) status = wrdat1((char *)cpts, cpts_la, nbytes);
   else                    status = wrdat2((char *)cpts, cpts_la, nbytes);

   if (status < 0 ) return(status);
/*
***Store knots.
*/
   nbytes = nknots * sizeof(DBfloat);

   if ( nbytes <= PAGSIZ ) status = wrdat1((char *)knots, knots_la, nbytes);
   else                    status = wrdat2((char *)knots, knots_la, nbytes);

   if (status < 0 ) return(status);

   else return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_nurbs(
        DBHvector *cpts,
        DBint      ncpts,
        DBfloat   *knots,
        DBint      nknots,
        DBptr      cpts_la,
        DBptr      knots_la)

/*      Reads NURBS curve data from DB.
 *
 *      In:  cpts_la = DB pointer to cpts data
 *           knots_la = DB pointer to knots data
 *
 *      FV:  0  => Ok.
 *
 *      (C)2002-06-13 J. Kjellander, Örebro University
 *
 ******************************************************!*/

  {
   DBint nbytes;
   DBstatus status;

/*
***Read cpts.
*/
   nbytes = ncpts * sizeof(DBHvector);

   if ( nbytes <= PAGSIZ ) status = rddat1((char *)cpts, cpts_la, nbytes);
   else                    status = rddat2((char *)cpts, cpts_la, nbytes);

   if (status < 0 ) return(status);
/*
***Read knots.
*/
   nbytes = nknots * sizeof(DBfloat);

   if ( nbytes <= PAGSIZ ) status = rddat1((char *)knots, knots_la, nbytes);
   else                    status = rddat2((char *)knots, knots_la, nbytes);

   if (status < 0 ) return(status);

   else return(0);
  }


/********************************************************/
/*!******************************************************/

        DBstatus DBfree_nurbs(
    DBfloat   *knots,
    DBHvector *cpts)


/*      Deallocates C-memory for NURBS data.
 *
 *      In:  ptr => C-pekare till minnesarea.
 *
 *      Out: None.
 *
 *      Return: Always 0.
 *
 *      (C)2006-10-20 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
/*
***Free memory for knot vector and controlpoints.
*/
    if ( knots != NULL ) v3free((char *)knots,"DBfree_nurbs");
  if ( cpts != NULL )  v3free((char *)cpts,"DBfree_nurbs");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_nurbs(DBSeg *seg)

/*      Deallocates DB memory for NURBS data.
 *
 *      In:  sptr = Ptr to first segment of a NURBS curve.
 *
 *      Out: None.
 *
 *      Return: Always 0.
 *
 *      (C)2006-10-20 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   DBint nbytes;

/*
***Free DB space for knot vector.
*/
   nbytes = seg->nknots * sizeof(DBfloat);

   if ( nbytes <= PAGSIZ ) rldat1(seg->knots_db, nbytes);
   else                    rldat2(seg->knots_db, nbytes);
/*
***Free DB space for control points.
*/
   nbytes = seg->ncpts * sizeof(DBHvector);

   if ( nbytes <= PAGSIZ ) rldat1(seg->cpts_db, nbytes);
   else                    rldat2(seg->cpts_db, nbytes);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        static DBstatus gmrdoc(
        GMCUR   *curpek,
        GMSEG  **segmnt,
        DBptr    la,
        DBint    vers)

/*      Huvudrutin för läsning av gamla kurvor.
 *
 *      In: curpek => Pekare till en curve-structure.
 *          segmnt => Pekare till GMSEG-pekare.
 *          la     => Kurvans adress i GM.
 *          vers   => Version av GMPOST.
 *
 *      Ut: *curpek => Curve-post.
 *          *segmnt => Pekare till segment.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/8/85 J. Kjellander
 *
 *      20/11/91 Ny kurva, J. Kjellander
 *      6/3/93   vers, J. Kjellander
 *      7/6/93   Dynamisk mallokering, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la_seg=DBNULL;
    DBint  i;
    GMCUR0 ocur0;
    GMCUR1 ocur1;
    GMSEG *segptr;

/*
***Vilken typ av kurva är det ?
*/
    switch ( vers )
      {
      case GMPOSTV0:
      rddat1( (char *)&ocur0, la, sizeof(GMCUR0));
      V3MOME(&ocur0.hed_oc,&curpek->hed_cu,sizeof(GMRECH));
      curpek->hed_cu.type = CURTYP;
      curpek->fnt_cu   = 0;
      curpek->lgt_cu   = 0.0;
      curpek->al_cu    = 0.0;
      curpek->ns_cu    = ocur0.ns_oc;
      curpek->nsgr_cu  = ocur0.ns_oc;
      curpek->plank_cu = FALSE;
      curpek->pcsy_cu  = DBNULL;
      la_seg           = ocur0.sptr_oc;
      break;

      case GMPOSTV1:
      rddat1( (char *)&ocur1, la, sizeof(GMCUR1));
      V3MOME(&ocur1.hed_oc,&curpek->hed_cu,sizeof(GMRECH));
      curpek->fnt_cu   = ocur1.fnt_oc;
      curpek->lgt_cu   = ocur1.lgt_oc;
      curpek->al_cu    = ocur1.al_oc;
      curpek->ns_cu    = ocur1.ns_oc;
      curpek->nsgr_cu  = ocur1.ns_oc;
      la_seg           = ocur1.sptr_oc;
      if ( ocur1.cptr_oc != DBNULL )
        {
        rddat1( (char *) &curpek->csy_cu, ocur1.cptr_oc, sizeof(DBTmat));
        curpek->plank_cu = TRUE;
        }
      else curpek->plank_cu = FALSE;
      break;
      }
/*
***Läs segmenten.
*/
    if ( segmnt != NULL )
      {
     *segmnt = segptr = DBcreate_segments(curpek->ns_cu);
      for ( i=0; i < curpek->ns_cu; ++i)
        {
        gmrdos(la_seg,segptr,vers);
        la_seg = segptr->nxt_seg;
        ++segptr;
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        static DBstatus gmrdos(
        DBptr    la,
        GMSEG   *segmnt,
        DBint    vers)

/*      Huvudrutin för läsning av gammmalt segment.
 *
 *      In: la     => Segmentets adress i GM.
 *          segmnt => Pekare till en utdata.
 *          vers   => Version av GMPOST.
 *
 *      Ut: *segmnt => Ett segment.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 6/3/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Hur gammalt är det ?
*/
    switch ( vers )
      {
      case GMPOSTV0:
      rddat1( (char *) segmnt, la, sizeof(GMSEG0));
      segmnt->ofs      = 0.0;
      segmnt->sl       = 0.0;
      segmnt->typ      = CUB_SEG;
      segmnt->subtyp   = 0;
      segmnt->spek_gm  = DBNULL;
      segmnt->spek2_gm = DBNULL;
      break;
  
      case GMPOSTV1:
      rddat1( (char *) segmnt, la, sizeof(GMSEG1));
      segmnt->typ      = CUB_SEG;
      segmnt->subtyp   = 0;
      segmnt->spek_gm  = DBNULL;
      segmnt->spek2_gm = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        static DBstatus gmupoc(
        GMCUR   *curpek,
        GMSEG   *segmnt,
        DBptr    la,
        DBint    vers)

/*      Uppdaterar en gammal kurv-post.
 *
 *      In: curpek => Pekare till en curve-structure.
 *          segmnt => Array med segment.
 *          la     => Kurvans adress i GM.
 *          vers   => Version av GMPOST.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/8/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av segment, J. Kjellander
 *      20/11/91 Ny kurva, J. Kjellander
 *      6/3/92   vers, J. Kjellander
 *
 ******************************************************!*/

  {
     DBptr la_seg=DBNULL;
     DBint i;
     GMCUR0 cur0;
     GMCUR1 cur1;
     GMSEG  seg;

/*
***Vilken version av kurva är det ?
*/
    switch ( vers )
      {
/*
***Version 1.
*/
      case GMPOSTV1:
      rddat1((char *)&cur1,la,sizeof(GMCUR1));
      V3MOME(&curpek->hed_cu,&cur1.hed_oc,sizeof(GMRECH));
      cur1.fnt_oc  = curpek->fnt_cu;
      cur1.lgt_oc  = curpek->lgt_cu;
      updata((char *)&cur1,la,sizeof(GMCUR1));
      la_seg = cur1.sptr_oc;
      break;
/*
***Version 0.
*/
      case GMPOSTV0:
      rddat1((char *)&cur0,la,sizeof(GMCUR0));
      V3MOME(&curpek->hed_cu,&cur0.hed_oc,sizeof(GMRECH));
      updata((char *)&cur0,la,sizeof(GMCUR0));
      la_seg = cur0.sptr_oc;
      break;
      }
/*
***Ev. uppdatering av segment.
*/
    if ( segmnt != NULL && curpek->ns_cu > 0 )
      {
      for ( i=0; i<curpek->ns_cu; ++i )
        {
        gmrdos(la_seg,&seg,vers);
        switch ( vers )
          {
          case GMPOSTV1:
          updata((char *)&segmnt[i],la_seg,sizeof(GMSEG1));
          break;

          case GMPOSTV0:
          updata((char *)&segmnt[i],la_seg,sizeof(GMSEG0));
          break;
          }
        la_seg = seg.nxt_seg;
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        static DBstatus gmdloc(DBptr la, DBint vers)

/*      Stryker en gammal kurva och deallokerar allokerat
 *      minne.
 *
 *      In: la    => Kurvans GM-adress.
 *          vers  => Version av GMPOST.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 21/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      20/11/91 Ny kurva, J. Kjellander
 *      13/4/93  vers, J. Kjellander
 *
 ******************************************************!*/

  {
    GMCUR0 cur0;
    GMCUR1 cur1;
    GMSEG  seg;
    DBint  i,nseg=0;
    DBptr  la_seg=DBNULL;

/*
***Vilken typ av kurva ?
*/
    switch ( vers )
      {
/*
***GMCUR1, stryk kurvpost + ev. plan.
*/
      case GMPOSTV1:
      rddat1((char *)&cur1,la,sizeof(GMCUR1));
      rldat1(la,sizeof(GMCUR1));
      if ( cur1.cptr_oc != DBNULL ) rldat1(cur1.cptr_oc,sizeof(DBTmat));
      la_seg = cur1.sptr_oc;
      nseg   = cur1.ns_oc;
      break;
/*
***GMCUR0.
*/
      case GMPOSTV0:
      rddat1((char *)&cur0,la,sizeof(GMCUR0));
      rldat1(la,sizeof(GMCUR0));
      la_seg = cur0.sptr_oc;
      nseg   = cur0.ns_oc;
      break;
      }
/*
***Stryk segmenten.
*/
    for ( i=0; i < nseg; ++i )
      {
      gmrdos(la_seg,&seg,vers);
      switch (vers )
        {
        case GMPOSTV1:
        rldat1(la_seg,sizeof(GMSEG1));
        break;

        case GMPOSTV0:
        rldat1(la_seg,sizeof(GMSEG0));
        break;
        }
      la_seg = seg.nxt_seg;
      }

  return(0);
  }

/********************************************************/
