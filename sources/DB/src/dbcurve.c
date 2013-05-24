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
*      *DBcreate_segments(); Allocates memory for curve segments
*      *DBadd_segments();    Increases allocated memory for curve segments
*       DBfree_segments();   Free's memory allocated for curve segments
*       DBwrite_nurbs();     Stores NURBS-curve data in DB
*       DBread_nurbs();      Reads NURBS curve data from DB
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
*    2004-09-02 DBread_nurbs static->public, Sören Larsson, Örebro University
*
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

static DBstatus gmrdoc(GMCUR *, GMSEG **, DBptr, DBint);
static DBstatus gmrdos(DBptr, GMSEG *, DBint);
static DBstatus gmupoc(GMCUR *, GMSEG *, DBptr, DBint);
static DBstatus gmdloc(DBptr, DBint);
/*static DBstatus DBread_nurbs(DBHvector *cpts, DBint ncpts, DBfloat *knots,
                             DBint nknots, DBptr cpts_la, DBptr knots_la);*/

/*
***I samband med V1.11 infördes nya kurvor.
***Den nya kurvan lagras inte i GM genom att bara skriva ut
***GMCUR-posten som den är. För att slippa lagra koordinat-
***system för kurvor som inte är plana, används här en egen
***interface-struktur. Detta innebär visserligen kopiering
***vid varje accsess men sparar mycket utrymme. Den gamla
***kurv-posten GMCUR0 finns i olddefs.h.
*/

/*
***I samband med V1.12 infördes kurvor på ytor, GMPOSTV2.
***GMCUR1 flyttades då till olddefs.h och ersattes med 
***GMCUR2. En ny GMSEG infördes och den gamla blev GMSEG1.
*/

/***1997-12-27, linjebredd. GMCUR2 flyttad till olddefs.h */

typedef struct gmcur3       /* GMCUR version 3 */
{
GMRECH  hed_nc;             /* Header */
DBshort fnt_nc;             /* Font */
DBfloat lgt_nc;             /* Streck-längd */
DBfloat al_nc;              /* geo-segmentens sammanlagda båglängd */
DBshort nges_nc;            /* Antal geo-segment */
DBptr   pges_nc;            /* Pekare till 1:a geo-segmentet */
DBptr   cptr_nc;            /* Pekare till kurvplan */
DBptr   pcsy_nc;            /* Pekare till aktivt koordinatsystem */
DBshort ngrs_nc;            /* Antal grafiska segment */
DBptr   pgrs_nc;            /* Pekare till 1:a grafiska segmentet */
DBfloat wdt_nc;             /* Bredd */
} GMCUR3;

/*!******************************************************/

        DBstatus DBinsert_curve(
        GMCUR   *curpek,
        GMSEG   *grsegp,
        GMSEG   *gesegp,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av curve. Först lagras
 *      själva curve-posten och därefter segmenten i
 *      2 länkade listor där första pekaren finns i 
 *      curve-posten och sista pekaren == DBNULL. 
 *      Om grsegp == gesegp lagras bara en uppsättning
 *      segment. Under alla omständigheter får båda pekarna
 *      i kurv-posten värden, ev. samma värde.
 *
 *      Observera att både grsegp och gesegp alltid måste
 *      ha vettiga värden vid anropet även om kurvan har
 *      samma grafiska som geometriska representation.
 *
 *      In: curpek => Pekare till en curve-structure.
 *          grsegp => Pekare till array med grafiska segment.
 *          gesegp => Pekare till array med geometriska segment.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till curve-data i GM.
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
 *      18/3/92  GMCUR1, J. Kjellander
 *      5/3/93   GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint  i;
    GMCUR3 cur3;

/*
***Lagra geo-segmenten, det sista först. Länken i sista
***segmentet = 0.
*/
    *lapek = 0;
    for ( i=curpek->ns_cu-1; i >= 0; --i)
      {
      (gesegp+i)->nxt_seg = *lapek;
      if ( wrdat1((char *)&gesegp[i],lapek,sizeof(GMSEG)) < 0 ) return(-3);
      }
    cur3.nges_nc =  curpek->ns_cu;
    cur3.pges_nc = *lapek;
/*
***Lagra eventuella gr-segment.
*/
    if ( grsegp == gesegp )
      {
      cur3.ngrs_nc = cur3.nges_nc;
      cur3.pgrs_nc = cur3.pges_nc;
      }
    else
      {
      *lapek = 0;
      for ( i=curpek->nsgr_cu-1; i >= 0; --i)
        {
        (grsegp+i)->nxt_seg = *lapek;
        if ( wrdat1((char *)&grsegp[i],lapek,sizeof(GMSEG)) < 0 ) return(-3);
        }
      cur3.ngrs_nc =  curpek->nsgr_cu;
      cur3.pgrs_nc = *lapek;
      }
/*
***Typ-specifika data.
*/
    curpek->hed_cu.type = CURTYP;   /* Typ = curve */
    curpek->hed_cu.vers = GMPOSTV3; /* Version */
/*
***Skapa en GMCUR3.
*/
    V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(GMRECH));
    cur3.fnt_nc  = curpek->fnt_cu;
    cur3.lgt_nc  = curpek->lgt_cu;
    cur3.al_nc   = curpek->al_cu;
    cur3.pcsy_nc = curpek->pcsy_cu;
    cur3.wdt_nc  = curpek->wdt_cu;
/*
***Om det är en plan kurva skall även koordinatsystemet lagras.
*/
    if ( curpek->plank_cu )
      {
      if ( wrdat1( (char *) &curpek->csy_cu, lapek,
             sizeof(DBTmat)) < 0 ) return(-3);
      cur3.cptr_nc = *lapek;
      }
    else cur3.cptr_nc = DBNULL;
/*
***Lagra själva curve-posten. 
*/
    return(inpost((GMUNON *)&cur3,idpek,lapek,sizeof(GMCUR3)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_curve(
        GMCUR   *curpek,
        GMSEG  **graseg,
        GMSEG  **geoseg,
        DBptr    la)

/*      Huvudrutin för läsning av en kurva. Returnerar
 *      kurvpost + segment om gra/geo-seg <> NULL.
 *
 *      In: curpek => Pekare till en curve-structure.
 *          graseg => Pekare till GMSEG-pekare.
 *          geoseg => Pekare till GMSEG-pekare.
 *          la     => Kurvans adress i GM.
 *
 *      Ut: *curpek => Curve-post.
 *          *graseg => Pekare till array med segment.
 *          *geoseg => Pekare till array med segment.
 *
 *      FV: Inget.
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
***C-pekare till kurv-posten och version.
*/
    hedpek = (GMRECH *)gmgadr(la);
    vers   =  GMVERS(hedpek);
/*
***Börja med att läsa kurvposten.
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
***Om läsning av både grafiska och geometriska segment
***begärts kan de vara samma och då skall vi bara allokera
***en area. Dessutom räcker det att läsa segmenten en gång !
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
***Allokera minne och läs ev. gra-segment.
***graseg = NULL => inga segment skall läsas.
*/
      if ( graflg )
        {
        segptr = *graseg;
        la_seg = cur3.pgrs_nc;
        for ( i=0; i < cur3.ngrs_nc; ++i)
          {
          rddat1((char *)segptr,la_seg,sizeof(GMSEG));
          la_seg = segptr->nxt_seg;
          ++segptr;
          }
        }
/*
***Läs ev. geo-segment. geoseg = NULL => inga segment skall läsas.
*/
      if ( geoflg )
        {
        segptr = *geoseg;
        la_seg = cur3.pges_nc;
        for ( i=0; i < cur3.nges_nc; ++i)
          {
          rddat1((char *)segptr,la_seg,sizeof(GMSEG));
          la_seg = segptr->nxt_seg;
/*
***If the segment is a NURBS span, read the array of controlpoints
***and the corresponding knot vector. If these already exist, just
***update their C-pointers.
*/
          if ( segptr->typ == NURB_SEG )
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
***Läs ev. koordinatsystem.
*/
      if ( cur3.cptr_nc != DBNULL )
        {
        rddat1( (char *) &curpek->csy_cu, cur3.cptr_nc, sizeof(DBTmat));
        curpek->plank_cu = TRUE;
        }
      else curpek->plank_cu = FALSE;
/*
***Övriga data.
*/
      V3MOME(&cur3.hed_nc,&curpek->hed_cu,sizeof(GMRECH));
      curpek->fnt_cu  = cur3.fnt_nc;
      curpek->lgt_cu  = cur3.lgt_nc;
      curpek->al_cu   = cur3.al_nc;
      curpek->ns_cu   = cur3.nges_nc;
      curpek->nsgr_cu = cur3.ngrs_nc;
      curpek->pcsy_cu = cur3.pcsy_nc;
      curpek->wdt_cu  = cur3.wdt_nc;
      break;
/*
***Äldre versioner, dvs. GMCUR0/GMSEG0 och GMCUR1/GMSEG1.
***Dessa kurvor har alltid grafisk = geometrisk representation.
***Observera att denna rutin kan anropas med geoseg OCH graseg
***lika med NULL.
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
        GMCUR   *curpek,
        GMSEG   *segmnt,
        DBptr    la)

/*      Uppdaterar en curve-post. Uppdatering av kurv-plan
 *      är inte implementerat i denna version.
 *
 *      In: curpek => Pekare till en curve-structure.
 *          segmnt => Array med segment.
 *          la     => Kurvans adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 27/4/85 J. Kjellander
 *
 *      1/1/86   Uppdatering av segment, J. Kjellander
 *      18/3/92  GMCUR1, J. Kjellander
 *      18/2/93  GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *
 ******************************************************!*/

  {
     DBptr   la_seg;
     DBint   i,vers;
     GMCUR3  cur3;
     GMSEG   seg;
     GMRECH *hedpek;

/*
***Vilken version av kurva är det ?
*/
    hedpek = (GMRECH *)gmgadr(la);
    vers   =  GMVERS(hedpek);
/*
***Läs kurvposten utan segment.
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
***Uppdatera.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      case GMPOSTV2:
/*
***Ev. uppdatering av geo-segment.
*/
      if ( segmnt != NULL && cur3.nges_nc > 0 )
        {
        la_seg = cur3.pges_nc;
        for ( i=0; i<cur3.nges_nc; ++i )
          {
          rddat1((char *)&seg,la_seg,sizeof(GMSEG));
          updata((char *)&segmnt[i],la_seg,sizeof(GMSEG));
          la_seg = seg.nxt_seg;
          }
        }
      }
/*
***Uppdatering av cur-posten. Vad som kan uppdateras (i denna version)
***är header, bredd, font och strecklängd.
***Detta utnyttjas av några ige-rutiner.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(GMRECH));
      cur3.fnt_nc  = curpek->fnt_cu;
      cur3.lgt_nc  = curpek->lgt_cu;
      cur3.wdt_nc  = curpek->wdt_cu;
      updata((char *)&cur3, la, sizeof(GMCUR3));
      break;

      case GMPOSTV2:
      V3MOME(&curpek->hed_cu,&cur3.hed_nc,sizeof(GMRECH));
      cur3.fnt_nc  = curpek->fnt_cu;
      cur3.lgt_nc  = curpek->lgt_cu;
      updata((char *)&cur3, la, sizeof(GMCUR2));
      break;
/*
***Äldre kurvor.
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

/*      Stryker en curve-post och deallokerar allokerat
 *      minne.
 *
 *      In: la => Kurvans GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 29/12/84 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare, J. Kjellander
 *      18/3/92  GMCUR1, J. Kjellander
 *      18/2/93  GMCUR2, J. Kjellander
 *      1997-12-27 GMCUR3, J.Kjellander
 *
 ******************************************************!*/

  {
    GMCUR3  cur3;
    GMRECH *hedpek;
    GMSEG   seg;
    DBint   i,vers;
    DBptr   la_seg;

/*
***Vilken version av kurva är det ?
*/
    hedpek = (GMRECH *)gmgadr(la);
    vers   = GMVERS(hedpek);
/*
***Läs kurvposten utan segment och stryk den.
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
***Stryk segmenten.
*/
    switch ( vers )
      {
      case GMPOSTV3:
      case GMPOSTV2:
      la_seg = cur3.pges_nc;                /* Pekare till 1:a segm. */
      for ( i=0; i<cur3.nges_nc; ++i )
        {
        rddat1((char *)&seg,la_seg,sizeof(GMSEG));
        rldat1(la_seg,sizeof(GMSEG));
        la_seg = seg.nxt_seg;               /* Pekare till nästa segm. */
        }
/*
***Stryk ev. grafiska segment.
*/
      if ( cur3.pgrs_nc != cur3.pges_nc )
        {
        la_seg = cur3.pgrs_nc;
        for ( i=0; i<cur3.ngrs_nc; ++i )
          {
          rddat1((char *)&seg,la_seg,sizeof(GMSEG));
          rldat1(la_seg,sizeof(GMSEG));
          la_seg = seg.nxt_seg;
          }
        }
/*
***Stryk ev. plan.
*/
      if ( cur3.cptr_nc != DBNULL ) rldat1(cur3.cptr_nc,sizeof(DBTmat));
      break;
/*
***Äldre kurvor.
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
/*!*****************************************************

        GMSEG *DBcreate_cpts(DBint antal)

/*      Allokerar plats i primärminne för NURBS control points.
 *
 *      In: antal = Antal control points.
 *
 *      Ut: Inget.
 *
 *      FV: C-adress till ledig minnesarea.
 *
 *      Felkoder: GM1053 = Kan ej allokera minne
 *
 *      (C)Sören Larsson, örebro University
 *
 ******************************************************!

  {
    DBint nbytes;
    DBstatus status;
    char *ptr,errbuf[80];

/*
***Allokera minne.

    if ( (ptr=v3mall(antal*sizeof(GMSEG),"DBcreate_segments")) == NULL )
      {
      sprintf(errbuf,"%d",(int)antal);
      erpush("GM1053",errbuf);
      }
/*
***Slut.

    return((GMSEG *)ptr);
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
