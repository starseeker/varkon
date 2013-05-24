/**********************************************************************
*
*    dbarc.c
*    =======
*
*    This file includes the following public functions:
*
*    DBinsert_arc();   Inserts an arc entity
*    DBread_arc();     Reads an arc entity
*    DBupdate_arc();   Updates an arc entity
*    DBdelete_arc();   Deletes an acr entity
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_arc(
        DBArc   *arcpek,
        DBSeg   *segmnt,
        DBId    *idpek,
        DBptr   *lapek)

/*      Inserts a new arc entity in the DB. A 2D arc
 *      doesn't have any segments. A 3D arc may have
 *      1-4 segments.
 *
 *      In: arcpek  => Pointer to arc data.
 *          segmnt  => Pointer to segments.
 *          idpek   => Pointer to ID.
 *
 *      Out: *lapek => The arc address.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *      25/11/91 Nytt format för segment, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      6/3/93   GMPOSTV2, J. Kjellander
 *      1997-12-27 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint i;

/*
***Store segments, last one first. Pointer in last
***segment = 0.
*/
    *lapek = 0;
    for ( i=arcpek->ns_a-1; i >= 0; --i)
      {
      (segmnt+i)->nxt_seg = *lapek;
      if ( wrdat1((char *)&segmnt[i],lapek,sizeof(DBSeg)) < 0 ) return(-3);
      }
/*
***Type-specific data.
*/
    arcpek->hed_a.type = ARCTYP;   /* Typ = arc */
    arcpek->hed_a.vers = GMPOSTV3; /* Version */
    arcpek->sptr_a = *lapek;       /* Pekare till 1:a segmentet */
/*
***Store the arc record itself.
*/
    return(inpost((DBAny *)arcpek,idpek,lapek,sizeof(DBArc) ));
  }

/********************************************************/
/********************************************************/

        DBstatus DBread_arc(
        DBArc   *arcpek,
        DBSeg   *segmnt,
        DBptr    la)

/*      Reads an arc entity from the DB. If segmnt <> NULL
 *      and segments exist they will be read as well. It
 *      is up to the calling routine to allocate sufficient
 *      memory for up to maximum 4 segments.
 *
 *      In:  la     => Arc address.
 *
 *      Ut: *arcpek => Arc data.
 *          *segmnt => 0 - 4 segments.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 31/12/84 J. Kjellander
 *
 *      17/3/88  segmnt=NULL, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      6/3/93   GMPOSTV2, J. Kjellander
 *      1997-02-12 Bug version, J.Kjellander
 *      1997-12-27 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint   i,version;
    DBptr   la_seg;
    DBHeader *hedpek;

/*
***Get C-ptr to header and check version.
*/
    hedpek = (DBHeader *)gmgadr(la);
    version = GMVERS(hedpek);
/*
***DBArc version 1 and 2 are identical but segments differ.
*/
    switch ( version )
      {
      case GMPOSTV3:
      V3MOME(hedpek,arcpek,sizeof(DBArc));
      break;
/*
***Version 1 and 2 have no WIDTH.
*/
      case GMPOSTV2:
      case GMPOSTV1:
      V3MOME(hedpek,arcpek,sizeof(GMARC1));
      arcpek->wdt_a = 0.0;
      break;
/*
***DBArc version 0 has no csys_ptr.
*/
      default:
      V3MOME(hedpek,arcpek,sizeof(GMARC0));
      arcpek->pcsy_a = DBNULL;
      arcpek->wdt_a = 0.0;
      break;
      }
/*
***Read segments. Note that when 1:st segment is read
***the data that hedpek points to may be overwritten by the
***segment just read because of paging.
***This bug was fixed 1997-02-12 JK.
*/
    if ( segmnt != NULL )
      {
      la_seg = arcpek->sptr_a;
      for ( i=0; i < arcpek->ns_a; ++i)
        {
        switch ( version )
          {
          case GMPOSTV3:
          case GMPOSTV2:
          rddat1( (char *) &segmnt[i], la_seg, sizeof(DBSeg));
          break;

          case GMPOSTV1:
          rddat1( (char *) &segmnt[i], la_seg, sizeof(GMSEG1));
          (segmnt+i)->typ = CUB_SEG;
          break;

          default:
          rddat1( (char *) &segmnt[i], la_seg, sizeof(GMSEG0));
          (segmnt+i)->ofs = 0.0;
          (segmnt+i)->sl  = 0.0;
          (segmnt+i)->typ = CUB_SEG;
          break;
          }
        la_seg = segmnt[i].nxt_seg;
        }
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        DBstatus DBupdate_arc(
        DBArc   *arcpek,
        DBSeg   *segmnt,
        DBptr    la)

/*      Updates an arc entity in the DB. If segments
 *      exist and segmnt <> NULL also the segments
 *      are updated.
 *
 *      In: arcpek => Ptr to DBArc.
 *          segmnt => Array with max 4 segments.
 *          la     => Arc adress i DB.
 *
 *      Return:    0 => OK.
 *                <0 => Error.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av segment, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      6/3/93   GMPOSTV2, J. Kjellander
 *      1997-02-12 Bug version, J.Kjellander
 *      1997-12-27 GMPOSTV3, J.Kjellander
 *      2008-05-13 Segments updated, J.Kjellander
 *      2008-12-14 Bugfix segm.ptrs, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la_seg;
    DBint   i,version,ns_old,ns_new;
    DBArc   oldarc;
    DBSeg   oldseg[4];

/*
**Get the current DBArc.
*/
    DBread_arc(&oldarc,oldseg,la);
/*
***What DB version is it ?
*/
    version = oldarc.hed_a.vers;
/*
***Update segments ?
*/
    if ( segmnt != NULL )
      {
      ns_old = oldarc.ns_a;
      ns_new = arcpek->ns_a;
/*
***Is the number of segments unchanged ?
***A bug was fixed in this code segment 2008-12-14, JK
*/
      if ( ns_new == ns_old )
        {
        la_seg = oldarc.sptr_a;
        for ( i=0; i<arcpek->ns_a; ++i )
          {
          segmnt[i].nxt_seg = oldseg[i].nxt_seg;

          switch ( version )
            {
            case GMPOSTV3:
            case GMPOSTV2:
            updata( (char *) &segmnt[i], la_seg, sizeof(DBSeg));
            break;

            case GMPOSTV1:
            updata( (char *) &segmnt[i], la_seg, sizeof(GMSEG1));
            break;

            default:
            updata( (char *) &segmnt[i], la_seg, sizeof(GMSEG0));
            break;
            }
          la_seg = oldseg[i].nxt_seg;
          }
        }
/*
***The number of segments has changed. Delete old segemnts
***and create new. NOTE that this code segment assumes version
***GMPOSTV3 or later. The only MBS procedure that can change the number
***of segments is TRIM and that means that a module is running so
***the arcs should be GMPOSTV3 or later. There should be no risk
***that we are doing this on old arcs or segments.
*/
      else
        {
        DBread_arc(&oldarc,oldseg,la);
        la_seg = oldarc.sptr_a;

        for ( i=0; i<ns_old; ++i )
          {
          rldat1(la_seg,sizeof(DBSeg));
          la_seg = oldseg[i].nxt_seg;
          }

        la_seg = DBNULL;
        for ( i=arcpek->ns_a-1; i >= 0; --i)
          {
          (segmnt+i)->nxt_seg = la_seg;
          if ( wrdat1((char *)&segmnt[i],&la_seg,sizeof(DBSeg)) < 0 ) return(-3);
          }
        arcpek->sptr_a = la_seg;
        }
      }
/*
***Update the DBArc.
*/
    switch ( version )
      {
      case GMPOSTV3:
      updata( (char *)arcpek, la, sizeof(DBArc));
      break;

      case GMPOSTV2:
      case GMPOSTV1:
      updata( (char *)arcpek, la, sizeof(GMARC1));
      break;

      default:
      updata( (char *)arcpek, la, sizeof(GMARC0));
      break;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        DBstatus DBdelete_arc(DBptr la)

/*      Deletes an arc entity from the DB. The space
 *      allocated by the arc and any existing segments
 *      is freed.
 *
 *      In: la => Arc address.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      6/3/93   GMPOSTV2, J. Kjellander
 *      1997-12-27 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    DBArc  arc;
    DBSeg  segmnt[4];
    DBint  i;
    DBptr  la_seg;

/*
***Read arc from DB.
*/
    DBread_arc(&arc,segmnt,la);
/*
***Release arc from DB.
*/
    switch ( arc.hed_a.vers )
      {
      case GMPOSTV3:
      rldat1(la,sizeof(DBArc));
      break;

      case GMPOSTV2:
      case GMPOSTV1:
      rldat1(la,sizeof(GMARC1));
      break;

      default:
      rldat1(la,sizeof(GMARC0));
      break;
      }
/*
***Release segments from DB.
*/
    la_seg = arc.sptr_a;                /* DBptr to first segm. */

    for ( i=0 ; i < arc.ns_a ; ++i )
      {
      switch ( arc.hed_a.vers )
        {
        case GMPOSTV3:
        case GMPOSTV2:
        rldat1(la_seg,sizeof(DBSeg));
        break;

        case GMPOSTV1:
        rldat1(la_seg,sizeof(GMSEG1));
        break;

        default:
        rldat1(la_seg,sizeof(GMSEG0));
        break;
        }
      la_seg = segmnt[i].nxt_seg;       /* DBptr to next segm. */
      }
/*
***The end.
*/
  return(0);
  }

/********************************************************/
