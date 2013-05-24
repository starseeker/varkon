/**********************************************************************
*
*    dbarc.c
*    =======
*
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
        GMARC   *arcpek,
        GMSEG   *segmnt,
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
***Lagra segmenten, det sista först. Länken i sista
***segmentet = 0.
*/
    *lapek = 0;
    for ( i=arcpek->ns_a-1; i >= 0; --i)
      {
      (segmnt+i)->nxt_seg = *lapek;
      if ( wrdat1((char *)&segmnt[i],lapek,sizeof(GMSEG)) < 0 ) return(-3);
      }
/*
***Typ-specifika data.
*/
    arcpek->hed_a.type = ARCTYP;   /* Typ = arc */
    arcpek->hed_a.vers = GMPOSTV3; /* Version */
    arcpek->sptr_a = *lapek;       /* Pekare till 1:a segmentet */
/*
***Lagra själva arc-posten. 
*/
    return(inpost((GMUNON *)arcpek,idpek,lapek,sizeof(GMARC) ));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_arc(
        GMARC   *arcpek,
        GMSEG   *segmnt,
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
    GMRECH *hedpek;

/*
***Fixa pekare till header och ta reda på version.
*/
    hedpek = (GMRECH *)gmgadr(la);
    version = GMVERS(hedpek);
/*
***GMARC version 1 och 2 är samma sak, det är segmenten
***som skiljer.
*/
    switch ( version )
      {
      case GMPOSTV3:
      V3MOME(hedpek,arcpek,sizeof(GMARC));
      break;
/*
***Version 1 och 2 saknar bredd.
*/
      case GMPOSTV2:
      case GMPOSTV1:
      V3MOME(hedpek,arcpek,sizeof(GMARC1));
      arcpek->wdt_a = 0.0;
      break;
/*
***GMARC version 0 saknar pekare till aktivt koordinatsystem.
*/ 
      default:
      V3MOME(hedpek,arcpek,sizeof(GMARC0));
      arcpek->pcsy_a = DBNULL;
      arcpek->wdt_a = 0.0;
      break;
      }
/*
***Läs segmenten. Observera att när 1:a segmentet är läst
***kan de data som hedpek pekar på vara överskrivna av det
***lästa segmentet pga. paging. Denna bug rättades 1997-02-12 JK.
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
          rddat1( (char *) &segmnt[i], la_seg, sizeof(GMSEG));
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

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_arc(
        GMARC   *arcpek,
        GMSEG   *segmnt,
        DBptr    la)

/*      Updates an arc entity in the DB. If segments
 *      exist and segmnt <> NULL also the segments
 *      are updated. In that case the number of segments
 *      (arcpek->ns_a) must be equal to the original
 *      number and the segment adresses (segmnt->nxt_seg)
 *      must be identical to the original segments.
 *
 *      In: arcpek => Pekare till en arc-structure.
 *          segmnt => Array med max 4 segment.
 *          la     => Arc:ens adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV:        0 => OK.
 *                <0 => Fel från underliggande rutiner.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av segment, J. Kjellander
 *      22/3/92  GMPOSTV1, J. Kjellander
 *      6/3/93   GMPOSTV2, J. Kjellander
 *      1997-02-12 Bug version, J.Kjellander
 *      1997-12-27 GMPOSTV3, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la_seg;
    DBint   i,version;
    GMRECH *hedpek;
/*
***Viken post-version är det ?
*/
    hedpek = (GMRECH *)gmgadr(la);
    version = GMVERS(hedpek);
/*
***Ev. uppdatering av segment.
*/
    if ( segmnt != NULL  &&  arcpek->ns_a > 0 )
      {
      la_seg = arcpek->sptr_a;
      for ( i=0; i<arcpek->ns_a; ++i )
        {
        switch ( version )
          {
          case GMPOSTV3:
          case GMPOSTV2:
          updata( (char *) &segmnt[i], la_seg, sizeof(GMSEG));
          break;

          case GMPOSTV1:
          updata( (char *) &segmnt[i], la_seg, sizeof(GMSEG1));
          break;

          default:
          updata( (char *) &segmnt[i], la_seg, sizeof(GMSEG0));
          break;
          }
        la_seg = (segmnt+i)->nxt_seg;
        }
      }
/*
***Uppdatering av arc-posten.
*/
    switch ( version )
      {
      case GMPOSTV3:
      updata( (char *)arcpek, la, sizeof(GMARC));
      break;

      case GMPOSTV2:
      case GMPOSTV1:
      updata( (char *)arcpek, la, sizeof(GMARC1));
      break;

      default:
      updata( (char *)arcpek, la, sizeof(GMARC0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

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
    GMARC  arc;
    GMSEG  segmnt[4];
    DBint  i;
    DBptr  la_seg;

/*
***Läs posten.
*/
    DBread_arc(&arc,segmnt,la);
/*
***Stryk arc-posten.
*/
    switch ( arc.hed_a.vers )
      {
      case GMPOSTV3:
      rldat1(la,sizeof(GMARC));
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
***Stryk segmenten.
*/
    la_seg = arc.sptr_a;                /* Pekare till 1:a segm. */

    for ( i=0 ; i < arc.ns_a ; ++i )
      {
      switch ( arc.hed_a.vers )
        {
        case GMPOSTV3:
        case GMPOSTV2:
        rldat1(la_seg,sizeof(GMSEG));
        break;

        case GMPOSTV1:
        rldat1(la_seg,sizeof(GMSEG1));
        break;

        default:
        rldat1(la_seg,sizeof(GMSEG0));
        break;
        }
      la_seg = segmnt[i].nxt_seg;       /* Pekare till nästa segm. */
      }

  return(0);
  }

/********************************************************/
