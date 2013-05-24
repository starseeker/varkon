/**********************************************************************
*
*    dbsurf.c
*    ========
*
*
*    This file includes the following public functions:
*
*    DBinsert_surface();     Inserts a surface entity (no patch data)
*    DBread_surface();       Reads a surface entity (no patch data)
*    DBupdate_surface();     Updates a surface entity (no patch data)
*    DBdelete_surface();     Deletes a surface entity (no patch data)
*
*    DBread_patches();       Reads all patches
*    DBread_one_patch();     Reads one patch
*   *DBcreate_patches();     Allocates memory for patch data
*    DBcreate_NURBS();       Allocates memory for NURBS data
*    DBfree_patches();       Free's allocated patch memory
*
*    DBadd_srep_curves();    Writes wireframe representation
*    DBread_srep_curves();   Reads wireframe representation
*    DBdelete_srep_curves(); Deletes wireframe representation
*    DBfree_srep_curves();   Free's mem allocated for wireframe representation
*
*    DBadd_srep_NURBS();     Writes NURBS representation
*    DBread_srep_NURBS();    Reads NURBS representation
*    DBdelete_srep_NURBS();  Deletes NURBS representation
*    DBfree_srep_NURBS();    Free's memory allocated for NURBS representation
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
***********************************************************************/

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

static DBptr    wrgseg (short nseg, GMSEG *segdat);
static GMSEG   *rdgseg(short nseg, DBptr la);
static DBstatus dlgseg(short nseg,DBptr la);
static DBstatus gmwrnp(GMPATNU *np, DBptr *pla);
static DBstatus gmrdnp(GMPATNU *np);
static DBstatus gmrlnp(DBptr la);

/*!******************************************************/

        DBstatus DBinsert_surface(
        GMSUR   *surpek,
        GMPAT   *ptpat,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av yta.
 *
 *      In: surpek => Pekare till en surf-structure.
 *          ptpat  => Pekare till topologipatchnätet.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till surf-post.
 *
 *      FV:  Returnerar status från inpost().
 *
 *      Felkoder: GM1043 = Kan ej allokera minne för patchtabell
 *
 *      (C)microform ab 3/11/92 J. Kjellander
 *
 *      22/2/93    RATSUR och LFTSUR, J.Kjellander
 *      31/1/94    Grafiska segment, J.Kjellander
 *      14/3/94    Nya GMPAT, J.Kjellander
 *      15/3/95    v3mall(), J.Kjellander
 *      1996-01-29 CON_PAT, J.Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      1997-11-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   *tabpek,*ptab,tabla,la;
    DBint    nu,nv,i,j,tabsiz;
    GMPAT   *patpek,*spek;

/*
***Allokera minne för patch-tabellen.
*/
    nu = surpek->nu_su;
    nv = surpek->nv_su;
    tabsiz = nu*nv*sizeof(DBptr);
    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBinsert_surface")) == NULL )
      return(erpush("GM1043",""));
    ptab = tabpek;
/*
***Gå igenom de primära patcharna. Lagra eventuella geometriska
***patchar och därefter den primära patchen själv.
*/
    patpek = ptpat;

    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
        switch ( patpek->styp_pat )
          {
/*
***Den primära patchen pekar direkt på en geometrisk patch.
***Lagra den geometriska patchen och notera dess GM-adress i den
***primära patchen.
*/
          case CUB_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATC));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case RAT_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATR));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case LFT_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(GMPATL));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case FAC_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATF));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case PRO_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(GMPATP));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case CON_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATN));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case P3_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATP3));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case P5_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(GMPATP5));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case P7_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(GMPATP7));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case P9_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(GMPATP9));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case P21_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(GMPATP21));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;

          case NURB_PAT:
          gmwrnp((GMPATNU *)patpek->spek_c,&la);
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;
/*
***Den primära patchen pekar på en NULL-patch. Ingenting
***lagras.
*/
          case NUL_PAT:
         *ptab = DBNULL;
          break;
/*
***Den primära patchen pekar på en annan primär (GMPAT) patch.
***Lagra inte denna men notera dess patchadress.
*/
          case TOP_PAT:
          spek = (GMPAT *) patpek->spek_c;
          patpek->su_pat = spek->iu_pat;
          patpek->sv_pat = spek->iv_pat;
          wrdat1((char *)patpek,ptab,sizeof(GMPAT));
          break;
          }
/*
***Nästa patch.
*/
        ++ptab;
        ++patpek;
        }
      }
/*
***Lagra patch-tabellen.
*/
    if ( tabsiz <= PAGSIZ ) wrdat1((char *)tabpek,&tabla,tabsiz);
    else                    wrdat2((char *)tabpek,&tabla,tabsiz);
    surpek->ptab_su = tabla;
    v3free(tabpek,"DBinsert_surface");
/*
***Ännu finns det ingen grafisk representation. Varken grafiska
***segment eller B-Splineyta.
*/
    for ( i=0; i< 6; ++i )
      {
      surpek->ngseg_su[i] = 0;
      surpek->pgseg_su[i] = DBNULL;
      }

    surpek->uorder_su = surpek->vorder_su = 0;
    surpek->nku_su = surpek->nkv_su = 0;
    surpek->pkvu_su = surpek->pkvv_su = surpek->pcpts_su = DBNULL;
/*
***Typ-specifika data.
*/
    surpek->hed_su.type = SURTYP;
    surpek->hed_su.vers = GMPOSTV1;
/*
***Lagra själva surf-posten. 
*/
    return(inpost((GMUNON *)surpek,idpek,lapek,sizeof(GMSUR)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_surface(
        GMSUR  *surpek,
        DBptr   la)

/*      Huvudrutin för läsning av en yta.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *          la     => Ytans adress i GM.
 *
 *      Ut: *surpek => Surf-post.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      V3MOME(hedpek,surpek,sizeof(GMSUR));
      break;
/*
***GMSUR0 har ingen B-spline approximation.
*/ 
      default:
      V3MOME(hedpek,surpek,sizeof(GMSUR0));
      surpek->uorder_su = surpek->vorder_su = 0;
      surpek->nku_su = surpek->nkv_su = 0;
      surpek->pkvu_su = surpek->pkvv_su = surpek->pcpts_su = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_surface(
        GMSUR  *surpek,
        DBptr   la)

/*      Uppdaterar (skriver över) en yt-post.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *          la     => Ytans adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV1:
      updata( (char *)surpek, la, sizeof(GMSUR));
      break;
 
      default:
      updata( (char *)surpek, la, sizeof(GMSUR0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_surface(DBptr la)

/*      Stryker en yt-post med patch-data och deallokerar
 *      allokerat minne.
 *
 *      In: la => Ytan:s GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 26/11/92 J. Kjellander
 *
 *      22/2/93    RATSUR och LFTSUR, J. Kjellander
 *      21/3/94    Nya GMPAT, J. Kjellander
 *      1996-01-29 CON_PAT, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      1997-05-28 DBread_one_patch(NULL), J.Kjellander
 *      1997-05-28 DBdelete_srep_NURBS(), J.Kjellander
 *      1997-11-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  *tabpek,*ptab,tabla;
    DBint   nu,nv,i,j,tabsiz;
    GMSUR   sur;
    GMPAT   toppat;

/*
***Läs själva yt-posten.
*/
    DBread_surface(&sur,la);
/*
***Stryk eventuella grafiska segment.
*/
    DBdelete_srep_curves(&sur);
/*
***Stryk ev. nurbs-representation.
*/
    DBdelete_srep_NURBS(&sur);
/*
***Stryk själva yt-posten.
*/
    switch ( sur.hed_su.vers )
      {
      case GMPOSTV1:
      rldat1(la,sizeof(GMSUR));
      break;
 
      default:
      rldat1(la,sizeof(GMSUR0));
      break;
      }
/*
***Allokera minne för patch-tabellen.
*/
    nu     = sur.nu_su;
    nv     = sur.nv_su;
    tabsiz = nu*nv*sizeof(DBptr);

    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBdelete_surface")) == NULL )
      return(erpush("GM1043",""));
    ptab = tabpek;
/*
***Läs tabellen och stryk den ur GM.
*/
    tabla = sur.ptab_su;

    if ( tabsiz <= PAGSIZ )
      {
      rddat1((char *)tabpek,tabla,tabsiz);
      rldat1(tabla,tabsiz);
      }
    else
      {
      rddat2((char *)tabpek,tabla,tabsiz);
      rldat2(tabla,tabsiz);
      }
/*
***Stryk patcharna ?
*/
    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
/*
***Först den primära patchen. NUL_PAT behöver inte strykas.
*/
        if ( *ptab != DBNULL )
          {
          rddat1((char *)&toppat,*ptab,sizeof(GMPAT));
          rldat1(*ptab,sizeof(GMPAT));
/*
***Sen den sekundära patchen.
*/
          switch ( toppat.styp_pat )
            {
/*
***Om den sekundära patchen är geometrisk stryker vi den.
*/
            case CUB_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATC));
            break;

            case RAT_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATR));
            break;

            case LFT_PAT:
            rldat2(toppat.spek_gm,sizeof(GMPATL));
            break;

            case FAC_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATF));
            break;

            case PRO_PAT:
            rldat2(toppat.spek_gm,sizeof(GMPATP));
            break;

            case CON_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATN));
            break;

            case P3_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATP3));
            break;

            case P5_PAT:
            rldat1(toppat.spek_gm,sizeof(GMPATP5));
            break;

            case P7_PAT:
            rldat2(toppat.spek_gm,sizeof(GMPATP7));
            break;

            case P9_PAT:
            rldat2(toppat.spek_gm,sizeof(GMPATP9));
            break;

            case P21_PAT:
            rldat2(toppat.spek_gm,sizeof(GMPATP21));
            break;

            case NURB_PAT:
            gmrlnp(toppat.spek_gm);
            rldat1(toppat.spek_gm,sizeof(GMPATNU));
            break;
            }
          }
/*
***Nästa (primära) patch.
*/
        ++ptab;
        }
      }
/*
***För säkerhets skull nollställer vi också DBread_one_patch():s
***cache så att en ny yta vars patchtabell råkar hamna
***på samma plats som den nu borttagna ytans säkert läses
***från disk vid nästa anrop till DBread_one_patch().
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Lämna tillbaks C-minne för patchtabellen.
*/
    v3free(tabpek,"DBdelete_surface");
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus gmwrnp(
        GMPATNU *np,
        DBptr   *pla)

/*      Lagrar nurbspatch.
 *
 *      In: np  => Pekare till en GMPATNU-structure.
 *          pla => Pekare till utdata.
 *
 *      Ut: *pla = GM-pekare.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint size;

/*
***Lagra först knutvektorerna.
*/
    size = np->nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) wrdat1((char *)np->kvec_u,&(np->pku),size);
    else                  wrdat2((char *)np->kvec_u,&(np->pku),size);

    size = np->nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) wrdat1((char *)np->kvec_v,&(np->pkv),size);
    else                  wrdat2((char *)np->kvec_v,&(np->pkv),size);
/*
***Och sen styrpolygonen.
*/
    size = (np->nk_u - np->order_u) *
           (np->nk_v - np->order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) wrdat1((char *)np->cpts,&(np->pcpts),size);
    else                  wrdat2((char *)np->cpts,&(np->pcpts),size);
/*
***Och slutligen själva patch-posten.
*/
    wrdat1((char *)np,pla,sizeof(GMPATNU));

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus gmrdnp(GMPATNU *np)

/*      Läser nurbspatch.
 *
 *      In: np  => Pekare till en delvis ifylld 
 *                 GMPATNU-structure.
 *
 *      Ut: *np = GMPATNU med C-pekare till NURBS-data.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBstatus status;
    DBint    size;

/*
***Allokera minne för NURBS-data.
*/
    if ( (status=DBcreate_NURBS(np)) < 0 ) return(status);
/*
***Läs knutvektorerna.
*/
    size = np->nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rddat1((char *)np->kvec_u,np->pku,size);
    else                  rddat2((char *)np->kvec_u,np->pku,size);

    size = np->nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rddat1((char *)np->kvec_v,np->pkv,size);
    else                  rddat2((char *)np->kvec_v,np->pkv,size);
/*
***Och sen styrpolygonen.
*/
    size = (np->nk_u - np->order_u) *
           (np->nk_v - np->order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) rddat1((char *)np->cpts,np->pcpts,size);
    else                  rddat2((char *)np->cpts,np->pcpts,size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus gmrlnp(DBptr la)

/*      Stryker nurbsdata.
 *
 *      In: la => Pekare till GMPATNU-post.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint   size;
    GMPATNU np;

/*
***Läs posten.
*/
    rddat1((char *)&np,la,sizeof(GMPATNU));
/*
***Stryk knutvektorerna.
*/
    size = np.nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rldat1(np.pku,size);
    else                  rldat2(np.pku,size);

    size = np.nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rldat1(np.pkv,size);
    else                  rldat2(np.pkv,size);
/*
***Och sen styrpolygonen.
*/
    size = (np.nk_u - np.order_u) *
           (np.nk_v - np.order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) rldat1(np.pcpts,size);
    else                  rldat2(np.pcpts,size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_patches(
        GMSUR  *surpek,
        GMPAT **pptpat)

/*      Läser patch-data för alla patchar i ytan.
 *      Allokerar minne för patcharna.
 *
 *      In: surpek => Pekare till yt-post.
 *          pptpat => Pekare till en patch-pekare.
 *
 *      Ut: *pptpat = Pekare till patchdata.
 *
 *      FV: Inget.
 *
 *      Felkoder: GM1043 = malloc-fel patchtabell
 *                GM1033 = malloc-fel primära patchar
 *                GM1063 = malloc-fel sekundär patch
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      22/2/93    RATSUR och LFTSUR, J. Kjellander
 *      14/3/94    Nya GMPAT, J. Kjellander
 *      1996-01-29 CON_PAT, J. Kjellander
 *      1996-07-04 Bug, PRO_PAT, J.Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-11-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr *tabpek,*ptab,tabla;
    DBint  nu,nv,i,j,tabsiz;
    GMPAT *patpek;

/*
***Allokera minne för patch-tabellen.
*/
    nu = surpek->nu_su;
    nv = surpek->nv_su;
    tabsiz = nu*nv*sizeof(DBptr);
    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBread_patches")) == NULL )
      return(erpush("GM1043",""));
    ptab = tabpek;
/*
***Läs patch-tabellen.
*/
    tabla = surpek->ptab_su;
    if ( tabsiz <= PAGSIZ ) rddat1((char *)tabpek,tabla,tabsiz);
    else                    rddat2((char *)tabpek,tabla,tabsiz);
/*
***Allokera minne för primära patchar.
*/
    if ( (*pptpat=(GMPAT *)DBcreate_patches(TOP_PAT,nu*nv)) == NULL )
      return(erpush("GM1033",""));
    patpek = *pptpat;
/*
***Läs patcharna.
*/
    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
/*
***Först den primära patchen. NUL_PAT behöver inte ens läsas
***utan skapas här.
*/
        if ( *ptab == DBNULL )
          {
          patpek->styp_pat = NUL_PAT;
          patpek->spek_c   = NULL;
          patpek->iu_pat   = (short) (i + 1);
          patpek->iv_pat   = (short) (j + 1);
          }
        else rddat1((char *)patpek,*ptab,sizeof(GMPAT));
/*
***Sen den sekundära patchen.
*/
        switch ( patpek->styp_pat )
          {
/*
***Om den sekundära patchen är geometrisk allokerar vi minne
***för den nu och läser in den.
*/
          case CUB_PAT:
          if ( (patpek->spek_c=DBcreate_patches(CUB_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATC));
          break;

          case RAT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(RAT_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATR));
          break;

          case LFT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(LFT_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATL));
          break;

          case FAC_PAT:
          if ( (patpek->spek_c=DBcreate_patches(FAC_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATF));
          break;

          case PRO_PAT:
          if ( (patpek->spek_c=DBcreate_patches(PRO_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP));
          break;

          case CON_PAT:
          if ( (patpek->spek_c=DBcreate_patches(CON_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATN));
          break;

          case P3_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P3_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP3));
          break;

          case P5_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P5_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP5));
          break;

          case P7_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P7_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP7));
          break;

          case P9_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P9_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP9));
          break;

          case P21_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P21_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP21));
          break;

          case NURB_PAT:
          if ( (patpek->spek_c=DBcreate_patches(NURB_PAT,1)) == NULL )
            return(erpush("GM1053",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATNU));
          if ( gmrdnp((GMPATNU *)patpek->spek_c) < 0 )
            return(erpush("GM1063",""));
          break;
/*
***Om den sekundära patchen är en topologisk patch beräknar
***vi dess C-adress utan att allokera eller läsa något.
*/
          case TOP_PAT:
          patpek->spek_c = (char *) (*pptpat +
                           (patpek->su_pat-1)*nv + patpek->sv_pat - 1);
          break;
          }
/*
***Nästa (primära) patch.
*/
        ++ptab;
        ++patpek;
        }
      }
/*
***Slut.
*/
    v3free(tabpek,"DBread_patches");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_one_patch(
        GMSUR  *surpek,
        GMPAT  *patpek,
        short   iu,
        short   iv)

/*      Läser patch-data för en enstaka patch. Lagrar
 *      sist lästa patchtabell i cache för snabb åtkomst
 *      vid upprepade acesser.
 *
 *      Vill man tömma cachen anropar man med surpek = NULL.
 *
 *      In: surpek => Pekare till yt-post eller NULL.
 *          patpek => Pekare till en patch-pekare.
 *          iu     => Patchnummer i U-riktn. 1 = Första
 *          iv     => Patchnummer i V-riktn. 1 = Första
 *
 *      Ut: *patpek = Pekare till patchdata.
 *
 *      Felkoder: GM1073 = Felaktig typ av patch
 *
 *      (C)microform ab 7/9/93 J. Kjellander
 *
 *      1/3/95     surpek = NULL, J. Kjellander
 *      17/3/95    Reentrans, J. Kjellander
 *      1996-01-29 CON_PAT, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-11-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
#ifdef V3_X11
extern bool wpintr();
#endif

    DBptr  *tabpek,*ptab,tabla,patla;
    DBint   nu,nv,tabsiz;
    char   *geopat;

static DBptr  otabla = DBNULL;
static DBptr  tab[500];

/*
***Om surpek == NULL handlar det bara om att tömma cachen.
*/
   if ( surpek == NULL )
     {
     otabla = DBNULL;
     return(0);
     }
/*
***På prov lägger vi in ett anrop till vänta-systemet här.
*/
#ifdef V3_X11
    wpintr();
#endif
/*
***Hur många patchar har ytan och var finns patchtabellen ?
*/
    nu     = surpek->nu_su;
    nv     = surpek->nv_su;
    tabla  = surpek->ptab_su;
/*
***Om patchtabellen finns i cachen är allt ok. Om inte
***läs in den. Om större än 500 patchar, allokera dynamisk
***area istället.
*/
    tabsiz = nu*nv*sizeof(DBptr);

    if      ( tabla == otabla )
      {
      ptab  = tab + (iu-1)*nv + iv-1;
      patla = *ptab;
      }
    else if ( nu*nv < 500 )
      { 
      if ( tabsiz <= PAGSIZ ) rddat1((char *)tab,tabla,tabsiz);
      else                    rddat2((char *)tab,tabla,tabsiz);
      ptab   = tab + (iu-1)*nv + iv-1;
      patla  = *ptab;
      otabla = tabla;
      }
    else
      {
      if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBread_one_patch")) == NULL )
        return(erpush("GM1043",""));
      if ( tabsiz <= PAGSIZ ) rddat1((char *)tabpek,tabla,tabsiz);
      else                    rddat2((char *)tabpek,tabla,tabsiz);
      ptab  = tabpek + (iu-1)*nv + iv-1;
      patla = *ptab;
      v3free((char *)tabpek,"DBread_one_patch");
      }
/*
***Innan topologiska patchen läses från GM noterar vi dess
***C-pekare till geometrisk patch eftersom denna kommer att
***skrivas över. Sen läser vi och sen återställer vi rätt adress.
*/
    geopat = patpek->spek_c;
    rddat1((char *)patpek,patla,sizeof(GMPAT));
    patpek->spek_c = geopat;
/*
***Nu kan vi läsa även den geometriska patchen.
*/
    switch ( patpek->styp_pat )
      {
      case CUB_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATC));
      break;

      case RAT_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATR));
      break;

      case LFT_PAT:
      rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATL));
      break;

      case FAC_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATF));
      break;

      case PRO_PAT:
      rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP));
      break;

      case CON_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATN));
      break;

      case P3_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP3));
      break;

      case P5_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP5));
      break;

      case P7_PAT:
      rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP7));
      break;

      case P9_PAT:
      rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP9));
      break;

      case P21_PAT:
      rddat2(patpek->spek_c,patpek->spek_gm,sizeof(GMPATP21));
      break;

      case TOP_PAT:
      rddat1(patpek->spek_c,patpek->spek_gm,sizeof(GMPAT));
      break;

      default:
      return(erpush("GM1073",""));
      }
    return(0);
  }

/********************************************************/
/*!******************************************************/

        char *DBcreate_patches(
        DBint typ,
        DBint antal)

/*      Allokerar plats i primärminne för patch-data.
 *
 *      In: typ   = Typ av patch.
 *          antal = Antal patchar totalt nu*nv.
 *
 *      Ut: Inget.
 *
 *      FV:   NULL = Kan ej allokera minne.
 *          > NULL = C-adress till ledig minnesarea.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      17/3/94    GMPAT, J. Kjellander
 *      1996-01-29 GMPATN, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-05-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
    char *ptr;
    DBint nbytes;

/*
***Vilken typ av patchar är det ?
*/
    switch ( typ )
      {
      case CUB_PAT:  nbytes = antal*sizeof(GMPATC);   break;
      case RAT_PAT:  nbytes = antal*sizeof(GMPATR);   break;
      case LFT_PAT:  nbytes = antal*sizeof(GMPATL);   break;
      case FAC_PAT:  nbytes = antal*sizeof(GMPATF);   break;
      case PRO_PAT:  nbytes = antal*sizeof(GMPATP);   break;
      case CON_PAT:  nbytes = antal*sizeof(GMPATN);   break;
      case P3_PAT:   nbytes = antal*sizeof(GMPATP3);  break;
      case P5_PAT:   nbytes = antal*sizeof(GMPATP5);  break;
      case P7_PAT:   nbytes = antal*sizeof(GMPATP7);  break;
      case P9_PAT:   nbytes = antal*sizeof(GMPATP9);  break;
      case P21_PAT:  nbytes = antal*sizeof(GMPATP21); break;
      case NURB_PAT: nbytes = antal*sizeof(GMPATNU);  break;
      case TOP_PAT:  nbytes = antal*sizeof(GMPAT);    break;
      default: return(NULL);
      }
/*
***Allokera minne.
*/
    if ( (ptr=v3mall(nbytes,"DBcreate_patches")) == NULL ) return(NULL);
/*
***Slut.
*/
    return(ptr);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBcreate_NURBS(GMPATNU *patpek)

/*      Allokerar minne för NURBS-data i en GMPATNU.
 *
 *      In: patpek => Pekare till delvis ifylld GMPATNU.
 *
 *      Ut: Inget.
 *
 *      FV:  0 = Ok.
 *          -2 = Kan ej allokera.
 *
 *      (C)microform ab 1997-05-05 J. Kjellander
 *
 ******************************************************!*/

  {
   unsigned size;

/*
***Allokera minne för knutpunkter.
*/
   size = patpek->nk_u*sizeof(DBfloat);
   if ( (patpek->kvec_u=(DBfloat *)v3mall(size,"DBcreate_NURBS")) == NULL )
     return(-2);
  
   size = patpek->nk_v*sizeof(DBfloat);
   if ( (patpek->kvec_v=(DBfloat *)v3mall(size,"DBcreate_NURBS")) == NULL )
     return(-2);
/*
***Samma för kontrollpunkter.
*/
   size = (patpek->nk_u - patpek->order_u) *
          (patpek->nk_v - patpek->order_v) *  sizeof(DBHvector);

   if ( (patpek->cpts=(DBHvector *)v3mall(size,"DBcreate_NURBS")) == NULL )
     return(-2);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBfree_patches(
        GMSUR *surpek,
        GMPAT *patpek)

/*      Återlämnar C-minne allokerat för patchar.
 *
 *      In: surpek => Pekare till yta.
 *          patpek => Pekare till patchar.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      21/3/94    Nya GMPAT, J. Kjellandert
 *      1996-01-29 GMPATN, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-05-05 GMPATNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint    i,j;
    GMPAT   *ppat;
    GMPATNU *nurbpat;

/*
***Initiering.
*/
    ppat = patpek;
/*
***Deallokera minne för alla geometriska patchar..
***C-rutinen free() är en void och man kan inte få
***reda på om det gick bra eller inte.
*/
    for ( i=0; i<surpek->nu_su; ++i )
      {
      for ( j=0; j<surpek->nv_su; ++j )
        {
        switch ( ppat->styp_pat )
          {
          case CUB_PAT:
          case RAT_PAT:
          case LFT_PAT:
          case FAC_PAT:
          case PRO_PAT:
          case CON_PAT:
          case P3_PAT:
          case P5_PAT:
          case P7_PAT:
          case P9_PAT:
          case P21_PAT:
          v3free(ppat->spek_c,"DBfree_patches");
          break;

          case NURB_PAT:
          nurbpat = (GMPATNU *)ppat->spek_c;
          v3free((char *)nurbpat->kvec_u,"DBfree_patches");
          v3free((char *)nurbpat->kvec_v,"DBfree_patches");
          v3free((char *)nurbpat->cpts,"DBfree_patches");
          v3free((char *)nurbpat,"DBfree_patches");
          break;
          }
        ++ppat;
        }
      }
/*
***Deallokera minne för dom topologiska patcharna.
*/
    v3free((char *)patpek,"DBfree_patches");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_srep_curves(
        GMSUR  *surpek,
        GMSEG  *sptarr[])

/*      Lagrar en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *          sptarr => Pekare till segment.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 ******************************************************!*/

  {

    if ( surpek->ngseg_su[0] > 0 )
      surpek->pgseg_su[0] = wrgseg(surpek->ngseg_su[0],sptarr[0]);
    else surpek->pgseg_su[0] = DBNULL;

    if ( surpek->ngseg_su[1] > 0 )
      surpek->pgseg_su[1] = wrgseg(surpek->ngseg_su[1],sptarr[1]);
    else surpek->pgseg_su[1] = DBNULL;

    if ( surpek->ngseg_su[2] > 0 )
      surpek->pgseg_su[2] = wrgseg(surpek->ngseg_su[2],sptarr[2]);
    else surpek->pgseg_su[2] = DBNULL;

    if ( surpek->ngseg_su[3] > 0 )
      surpek->pgseg_su[3] = wrgseg(surpek->ngseg_su[3],sptarr[3]);
    else surpek->pgseg_su[3] = DBNULL;

    if ( surpek->ngseg_su[4] > 0 )
      surpek->pgseg_su[4] = wrgseg(surpek->ngseg_su[4],sptarr[4]);
    else surpek->pgseg_su[4] = DBNULL;

    if ( surpek->ngseg_su[5] > 0 )
      surpek->pgseg_su[5] = wrgseg(surpek->ngseg_su[5],sptarr[5]);
    else surpek->pgseg_su[5] = DBNULL;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_srep_curves(
        GMSUR  *surpek,
        GMSEG  *sptarr[])

/*      Läser en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *          sptarr => Pekare till utdata.
 *
 *      Ut: *sptarrÄÅ => Pekare till segment.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Läs segmentdata för de 6 "kurvorna" och returnera C-pekare.
*/
    if ( surpek->pgseg_su[0] > 0 )
      sptarr[0] = rdgseg(surpek->ngseg_su[0],surpek->pgseg_su[0]);
    else
      sptarr[0] = NULL;

    if ( surpek->pgseg_su[1] > 0 )
      sptarr[1] = rdgseg(surpek->ngseg_su[1],surpek->pgseg_su[1]);
    else
      sptarr[1] = NULL;

    if ( surpek->pgseg_su[2] > 0 )
      sptarr[2] = rdgseg(surpek->ngseg_su[2],surpek->pgseg_su[2]);
    else
      sptarr[2] = NULL;

    if ( surpek->pgseg_su[3] > 0 )
      sptarr[3] = rdgseg(surpek->ngseg_su[3],surpek->pgseg_su[3]);
    else
      sptarr[3] = NULL;

    if ( surpek->pgseg_su[4] > 0 )
      sptarr[4] = rdgseg(surpek->ngseg_su[4],surpek->pgseg_su[4]);
    else
      sptarr[4] = NULL;

    if ( surpek->pgseg_su[5] > 0 )
      sptarr[5] = rdgseg(surpek->ngseg_su[5],surpek->pgseg_su[5]);
    else
      sptarr[5] = NULL;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_srep_curves(GMSUR  *surpek)

/*      Suddar en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 14/3/94 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Läs segmentdata för de 6 "kurvorna" och returnera C-pekare.
*/
    if ( surpek->pgseg_su[0] > 0 )
      dlgseg(surpek->ngseg_su[0],surpek->pgseg_su[0]);

    if ( surpek->pgseg_su[1] > 0 )
      dlgseg(surpek->ngseg_su[1],surpek->pgseg_su[1]);

    if ( surpek->pgseg_su[2] > 0 )
      dlgseg(surpek->ngseg_su[2],surpek->pgseg_su[2]);

    if ( surpek->pgseg_su[3] > 0 )
      dlgseg(surpek->ngseg_su[3],surpek->pgseg_su[3]);

    if ( surpek->pgseg_su[4] > 0 )
      dlgseg(surpek->ngseg_su[4],surpek->pgseg_su[4]);

    if ( surpek->pgseg_su[5] > 0 )
      dlgseg(surpek->ngseg_su[5],surpek->pgseg_su[5]);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBfree_srep_curves(GMSEG *sptarr[])

/*      Återlämnar minne för grafiska segment.
 *
 *      In: sptarr = 6 GMSEG-pekare.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( sptarr[0] != NULL ) v3free((char *)sptarr[0],"DBfree_srep_curves");
    if ( sptarr[1] != NULL ) v3free((char *)sptarr[1],"DBfree_srep_curves");
    if ( sptarr[2] != NULL ) v3free((char *)sptarr[2],"DBfree_srep_curves");
    if ( sptarr[3] != NULL ) v3free((char *)sptarr[3],"DBfree_srep_curves");
    if ( sptarr[4] != NULL ) v3free((char *)sptarr[4],"DBfree_srep_curves");
    if ( sptarr[5] != NULL ) v3free((char *)sptarr[5],"DBfree_srep_curves");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_srep_NURBS(
        GMSUR   *surpek,
        GLfloat *kvu,
        GLfloat *kvv,
        GLfloat *cpts)

/*      Lagrar en ytas B-splineapproximation.
 *
 *      In: kvu,kvv => Nodvektorerna
 *          cpts    => Kontrollpunkterna.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint size;

/*
***Kolla att data finns att lagra.
*/
   if ( surpek->nku_su     == 0  ||
        surpek->nkv_su     == 0  ||
        surpek->uorder_su  == 0  ||
        surpek->vorder_su  == 0 )
     {
     surpek->pkvu_su  = DBNULL;
     surpek->pkvv_su  = DBNULL;
     surpek->pcpts_su = DBNULL;
     return(-2);
     }
/*
***Lagra nodvektorerna.
*/
   size = surpek->nku_su*sizeof(GLfloat);
   if ( size < PAGSIZ ) wrdat1((char *)kvu,&surpek->pkvu_su,size);
   else                 wrdat2((char *)kvu,&surpek->pkvu_su,size);

   size = surpek->nkv_su*sizeof(GLfloat);
   if ( size < PAGSIZ ) wrdat1((char *)kvv,&surpek->pkvv_su,size);
   else                 wrdat2((char *)kvv,&surpek->pkvv_su,size);
/*
***Lagra kontrollpunkterna.
*/
   size = (surpek->nku_su - surpek->uorder_su) *
          (surpek->nkv_su - surpek->vorder_su) * 3 * sizeof(GLfloat);

   if ( size < PAGSIZ ) wrdat1((char *)cpts,&surpek->pcpts_su,size);
   else                 wrdat2((char *)cpts,&surpek->pcpts_su,size);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  DBread_srep_NURBS(
        GMSUR    *surpek,
        GLfloat **kvu,
        GLfloat **kvv,
        GLfloat **cpts)

/*      Läser en ytas B-splineapproximation. C-minne
 *      allokeras och pekare returneras.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *          kvu    => Nodvektor U
 *          kvv    => Nodvektor V
 *          cpts   => Kontrollpunkter
 *
 *      Ut: *kvu  => Pekare till U-noder
 *          *kvv  => Pekare till V-noder
 *          *cpts => Pekare till Kontrollpunkter
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 ******************************************************!*/

  {
   unsigned size;

/*
***Kolla att data finns att läsa.
*/
   if ( surpek->nku_su     == 0  ||
        surpek->nkv_su     == 0  ||
        surpek->uorder_su  == 0  ||
        surpek->vorder_su  == 0  ||
        surpek->pkvu_su    == DBNULL  ||
        surpek->pkvv_su    == DBNULL  ||
        surpek->pcpts_su   == DBNULL ) return(-2);
/*
***Allokera minne för noder och läs.
*/
   size = surpek->nku_su*sizeof(GLfloat);
   if ( (*kvu=(GLfloat *)v3mall(size,"DBread_srep_NURBS")) == NULL ) return(-2);
   if ( size < PAGSIZ ) rddat1((char *)*kvu,surpek->pkvu_su,size);
   else                 rddat2((char *)*kvu,surpek->pkvu_su,size);
  
   size = surpek->nkv_su*sizeof(GLfloat);
   if ( (*kvv=(GLfloat *)v3mall(size,"DBread_srep_NURBS")) == NULL ) return(-2);
   if ( size < PAGSIZ ) rddat1((char *)*kvv,surpek->pkvv_su,size);
   else                 rddat2((char *)*kvv,surpek->pkvv_su,size);
/*
***Samma för kontrollpunkter.
*/
   size = (surpek->nku_su - surpek->uorder_su) *
          (surpek->nkv_su - surpek->vorder_su) * 3 * sizeof(GLfloat);

   if ( (*cpts=(GLfloat *)v3mall(size,"DBread_srep_NURBS")) == NULL ) return(-2);

   if ( size < PAGSIZ ) rddat1((char *)*cpts,surpek->pcpts_su,size);
   else                 rddat2((char *)*cpts,surpek->pcpts_su,size);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_srep_NURBS(GMSUR *surpek)

/*      Suddar en ytas B-splineapproximation.
 *
 *      In: surpek => Pekare till en GMSUR-structure.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint size;

/*
***Kolla att data finns att sudda.
*/
   if ( surpek->nku_su     == 0  ||
        surpek->nkv_su     == 0  ||
        surpek->uorder_su  == 0  ||
        surpek->vorder_su  == 0  ||
        surpek->pkvu_su    == DBNULL  ||
        surpek->pkvv_su    == DBNULL  ||
        surpek->pcpts_su   == DBNULL ) return(-2);
/*
***Sudda nodvektorerna.
*/
   size = surpek->nku_su*sizeof(GLfloat);
   if ( size < PAGSIZ ) rldat1(surpek->pkvu_su,size);
   else                 rldat2(surpek->pkvu_su,size);

   size = surpek->nkv_su*sizeof(GLfloat);
   if ( size < PAGSIZ ) rldat1(surpek->pkvv_su,size);
   else                 rldat2(surpek->pkvv_su,size);
/*
***Och kontrollpunkterna.
*/
   size = (surpek->nku_su - surpek->uorder_su) *
          (surpek->nkv_su - surpek->vorder_su) * 3 * sizeof(GLfloat);

   if ( size < PAGSIZ ) rldat1(surpek->pcpts_su,size);
   else                 rldat2(surpek->pcpts_su,size);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBfree_srep_NURBS(
        GLfloat *kvu,
        GLfloat *kvv,
        GLfloat *cpts)

/*      Återlämnar minne för B-splineapproximation.
 *
 *      In: kvu,kvv = Minne för nodvektorer.
 *          cpts    = Minne för kontrollpunkter.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( kvu != NULL ) v3free((char *)kvu,"DBfree_srep_NURBS");
    if ( kvv != NULL ) v3free((char *)kvv,"DBfree_srep_NURBS");
    if ( cpts != NULL ) v3free((char *)cpts,"DBfree_srep_NURBS");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        static DBptr wrgseg(
        short  nseg,
        GMSEG  segdat[])

/*      Lagrar nseg stycken GMSEG och returnerar GM-adress.
 *
 *      In: segdat => Pekare till segment.
 *          nseg   => Antal segment.
 *
 *      Ut: Inget.
 *
 *      FV:  > 0 => GM-adress.
 *           = 0 => Kan ej lagra.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 *      15/3/95 Bugfix, J. Kjellander
 *
 ******************************************************!*/

  {
    DBint  i;
    DBptr  la;

/*
***Lagra segmenten och länka ihop dom.
*/
    la = 0;

    for ( i=nseg; i > 0; --i)
      {
      (segdat+i-1)->nxt_seg = la;
      if ( wrdat1((char *)&segdat[i-1],&la,sizeof(GMSEG)) < 0 ) return(0);
      }

  return(la);
  }

/********************************************************/
/*!******************************************************/

        static GMSEG *rdgseg(
        short  nseg,
        DBptr  la)

/*      Läser nseg stycken GMSEG och returnerar C-adress.
 *
 *      In: nseg   => Antal segment.
 *          la     => Segmentens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  > 0 => Giltig C-adress.
 *           = 0 => Fel från DBcreate_segments(), dvs. malloc().
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint  i;
    DBptr  la_seg;
    GMSEG *segpek,*ptr_seg;

/*
***Allokera minne.
*/
    if ( (segpek=DBcreate_segments((DBint)nseg)) == NULL ) return(NULL);
/*
***Läs segment.
*/
    la_seg  = la;
    ptr_seg = segpek;

    for ( i=0; i < nseg; ++i)
      {
      rddat1((char *)ptr_seg,la_seg,sizeof(GMSEG));
      la_seg = ptr_seg->nxt_seg;
      ++ptr_seg;
      }

    return(segpek);
  }

/********************************************************/
/*!******************************************************/

        static DBstatus dlgseg(
        short  nseg,
        DBptr  la)

/*      Dödar nseg stycken GMSEG vid la.
 *
 *      In: nseg   => Antal segment.
 *          la     => Segmentens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0
 *
 *      (C)microform ab 14/3/94 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint i;
    DBptr la_seg;
    GMSEG seg;

    la_seg = la;

    for ( i=0; i<nseg; ++i)
      {
      rddat1((char *)&seg,la_seg,sizeof(GMSEG));
      rldat1(la_seg,sizeof(GMSEG));
      la_seg = seg.nxt_seg;
      }

    return(0);
  }

/********************************************************/
