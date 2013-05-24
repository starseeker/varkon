/**********************************************************************
*
*    dbsurf.c
*    ========
*    This file is part of the VARKON Database Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes the following public functions:
*
*    DBinsert_surface();     Inserts a surface entity
*    DBread_surface();       Reads a surface entity (no patch data)
*    DBupdate_surface();     Updates a surface entity (no patch data)
*    DBdelete_surface();     Deletes a surface entity (no patch data)
*
*    DBread_patches();       Reads all patches
*    DBread_one_patch();     Reads one patch
*    DBcreate_patches();     Allocates memory for patch data
*    DBcreate_NURBS();       Allocates memory for NURBS data
*    DBfree_patches();       Free's allocated patch memory
*
*    DBread_getrimcurves();  Read geometrical trimcurves
*    DBread_getrimcurves();  Free's allocated trimcurve memory
*    DBcreate_segarrs();     Allocates memory for trimcurves
*
*    DBadd_sur_grwire();     Writes wireframe representation
*    DBread_sur_grwire();    Reads wireframe representation
*    DBdelete_sur_grwire();  Deletes wireframe representation
*    DBfree_sur_grwire();    Free's mem allocated for wireframe representation
*
*    DBadd_sur_grsur();      Writes NURBS representation
*    DBread_sur_grsur();     Reads NURBS representation
*    DBdelete_sur_grsur();   Deletes NURBS representation
*    DBfree_sur_grsur();     Free's memory allocated for NURBS representation
*
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

#include <string.h>
#include "../include/DB.h"
#include "../include/DBintern.h"

/*
***Prototypes for internal functions.
*/
static DBptr    wrgseg (short nseg, DBSeg *segdat);
static DBSeg   *rdgseg(short nseg, DBptr la);
static DBstatus dlgseg(short nseg,DBptr la);
static DBstatus gmwrnp(DBPatchNU *np, DBptr *pla);
static DBstatus gmrdnp(DBPatchNU *np);
static DBstatus gmrlnp(DBptr la);

/*!******************************************************/

        DBstatus  DBinsert_surface(
        DBSurf   *surpek,
        DBPatch  *ptpat,
        DBSegarr *ptrim,
        DBId     *idpek,
        DBptr    *lapek)

/*      Insert surface in DB.
 *
 *      In: surpek  => C-ptr to DBSurf.
 *          ptpat   => C-ptr to array of topological patches.
 *          ptrim   => C-ptr to array of trimcurves or NULL.
 *          idpek   => C-ptr to surface ID.
 *
 *      Out: *lapek => Surface DB address.
 *
 *      Return: The status of inpost().
 *
 *      Error codes: GM1043 = Can't allocate memory for patch table
 *
 *      (C)microform ab 3/11/92 J. Kjellander
 *
 *      22/2/93    RATSUR och LFTSUR, J.Kjellander
 *      31/1/94    Grafiska segment, J.Kjellander
 *      14/3/94    Nya DBPatch, J.Kjellander
 *      15/3/95    v3mall(), J.Kjellander
 *      1996-01-29 CON_PAT, J.Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      1997-11-05 DBPatchNU, J.Kjellander
 *      2007-01-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   *tabpek,*ptab,tabla,la;
    DBint    nu,nv,i,j,tabsiz;
    DBPatch *patpek,*spek;

/*
***Allocate C-memory for the DB patch table.
*/
    nu = surpek->nu_su;
    nv = surpek->nv_su;
    tabsiz = nu*nv*sizeof(DBptr);
    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBinsert_surface")) == NULL )
      return(erpush("GM1043",""));
    ptab = tabpek;
/*
***For each topological patch, store the geometric patch first
***(if therre is one) and then store the topological patch itself.
*/
    patpek = ptpat;

    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
        switch ( patpek->styp_pat )
          {
/*
***Store the geometric patch. Save it's DB address in
***the topological patch and then store the topological patch.
*/
          case CUB_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchC));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case RAT_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchR));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case LFT_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(DBPatchL));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case FAC_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchF));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case PRO_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(DBPatchP));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case CON_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchN));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case P3_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchP3));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case P5_PAT:
          wrdat1(patpek->spek_c,&la,sizeof(DBPatchP5));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case P7_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(DBPatchP7));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case P9_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(DBPatchP9));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case P21_PAT:
          wrdat2(patpek->spek_c,&la,sizeof(DBPatchP21));
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;

          case NURB_PAT:
          gmwrnp((DBPatchNU *)patpek->spek_c,&la);
          patpek->spek_gm = la;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;
/*
***A NULL patch is not stored.
*/
          case NUL_PAT:
         *ptab = DBNULL;
          break;
/*
***The primary topological patch points to another topological
****patch. Don't save it but remember it's address.
*/
          case TOP_PAT:
          spek = (DBPatch *) patpek->spek_c;
          patpek->su_pat = spek->iu_pat;
          patpek->sv_pat = spek->iv_pat;
          wrdat1((char *)patpek,ptab,sizeof(DBPatch));
          break;
          }
/*
***Next patch.
*/
        ++ptab;
        ++patpek;
        }
      }
/*
***Store the patch table.
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
***Type and DBSurf version.
*/
    surpek->hed_su.type = SURTYP;
    surpek->hed_su.vers = GMPOSTV2;
/*
***Store the DBSurf itself. 
*/
    return(inpost((GMUNON *)surpek,idpek,lapek,sizeof(DBSurf)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_surface(
        DBSurf  *surpek,
        DBptr    la)

/*      Read surface main data. No geometrical
 *      or graphical data is read.
 *
 *      In: surpek   => C-ptr to surface.
 *          la       => Surface address in DB
 *
 *      Out: *surpek => Surface data.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      2007-01-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedpek;

/*
***Get a C-ptr to the surface header.
*/
    hedpek = (DBHeader *)gmgadr(la);
/*
***What DB version of surface is it ?
*/
    switch ( GMVERS(hedpek) )
      {
/*
***GMPOSTV2 is the current representation
***implemented 2007-01-01 JK.
*/
      case GMPOSTV2:
      V3MOME(hedpek,surpek,sizeof(DBSurf));
      break;
/*
***GMSUR1 has no trimcurves.
*/
      case GMPOSTV1:
      V3MOME(hedpek,surpek,sizeof(GMSUR1));
      surpek->ntrim_su         = 0;
      surpek->getrim_su        = DBNULL;
      surpek->ngrwborder_su    = 0;
      surpek->grwborder_su     = DBNULL;
      surpek->ngrwiso_su       = 0;
      surpek->grwiso_su        = DBNULL;
      surpek->vertextype_su    = -1;
      surpek->grstrim_su       = DBNULL;
      surpek->nustep_su        = -1.0;
      surpek->nvstep_su        = -1.0;
      break;
/*
***GMSUR0 has no NURBS approximation.
***It is not likely to find any GMSUR0
***around anymore but the cost of this
***extra code is nothing and it helps
***to understand the history.
*/ 
      default:
      V3MOME(hedpek,surpek,sizeof(GMSUR0));
      surpek->uorder_su = surpek->vorder_su = 0;
      surpek->nku_su = surpek->nkv_su = 0;
      surpek->pkvu_su = surpek->pkvv_su = surpek->pcpts_su = DBNULL;
      surpek->ntrim_su         = 0;
      surpek->getrim_su        = DBNULL;
      surpek->ngrwborder_su    = 0;
      surpek->grwborder_su     = DBNULL;
      surpek->ngrwiso_su       = 0;
      surpek->grwiso_su        = DBNULL;
      surpek->vertextype_su    = -1;
      surpek->grstrim_su       = DBNULL;
      surpek->nustep_su        = -1.0;
      surpek->nvstep_su        = -1.0;
      break;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_surface(
        DBSurf  *surpek,
        DBptr    la)

/*      Uppdaterar (skriver över) en yt-post.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
 *          la     => Ytans adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/11/92 J. Kjellander
 *
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      2007-01-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    GMRECH *hedpek;

    hedpek = (GMRECH *)gmgadr(la);

    switch ( GMVERS(hedpek) )
      {
      case GMPOSTV2:
      updata( (char *)surpek, la, sizeof(DBSurf));
      break;

      case GMPOSTV1:
      updata( (char *)surpek, la, sizeof(GMSUR1));
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
 *      21/3/94    Nya DBPatch, J. Kjellander
 *      1996-01-29 CON_PAT, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-03-05 GMPOSTV1, J.Kjellander
 *      1997-05-28 DBread_one_patch(NULL), J.Kjellander
 *      1997-05-28 DBdelete_srep_NURBS(), J.Kjellander
 *      1997-11-05 DBPatchNU, J.Kjellander
 *      2007-01-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  *tabpek,*ptab,tabla;
    DBint   nu,nv,i,j,tabsiz;
    DBSurf   sur;
    DBPatch   toppat;

/*
***Läs själva yt-posten.
*/
    DBread_surface(&sur,la);
/*
***Stryk eventuella grafiska segment.
*/
    DBdelete_sur_grwire(&sur);
/*
***Stryk ev. nurbs-representation.
*/
    DBdelete_sur_grsur(&sur);
/*
***Stryk själva yt-posten.
*/
    switch ( sur.hed_su.vers )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBSurf));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMSUR1));
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
          rddat1((char *)&toppat,*ptab,sizeof(DBPatch));
          rldat1(*ptab,sizeof(DBPatch));
/*
***Sen den sekundära patchen.
*/
          switch ( toppat.styp_pat )
            {
/*
***Om den sekundära patchen är geometrisk stryker vi den.
*/
            case CUB_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchC));
            break;

            case RAT_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchR));
            break;

            case LFT_PAT:
            rldat2(toppat.spek_gm,sizeof(DBPatchL));
            break;

            case FAC_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchF));
            break;

            case PRO_PAT:
            rldat2(toppat.spek_gm,sizeof(DBPatchP));
            break;

            case CON_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchN));
            break;

            case P3_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchP3));
            break;

            case P5_PAT:
            rldat1(toppat.spek_gm,sizeof(DBPatchP5));
            break;

            case P7_PAT:
            rldat2(toppat.spek_gm,sizeof(DBPatchP7));
            break;

            case P9_PAT:
            rldat2(toppat.spek_gm,sizeof(DBPatchP9));
            break;

            case P21_PAT:
            rldat2(toppat.spek_gm,sizeof(DBPatchP21));
            break;

            case NURB_PAT:
            gmrlnp(toppat.spek_gm);
            rldat1(toppat.spek_gm,sizeof(DBPatchNU));
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

 static DBstatus   gmwrnp(
        DBPatchNU *np,
        DBptr     *pla)

/*      Lagrar nurbspatch.
 *
 *      In: np  => Pekare till en DBPatchNU-structure.
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
    wrdat1((char *)np,pla,sizeof(DBPatchNU));

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus gmrdnp(DBPatchNU *np)

/*      Läser nurbspatch.
 *
 *      In: np  => Pekare till en delvis ifylld 
 *                 DBPatchNU-structure.
 *
 *      Ut: *np = DBPatchNU med C-pekare till NURBS-data.
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
 *      In: la => Pekare till DBPatchNU-post.
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
    DBPatchNU np;

/*
***Läs posten.
*/
    rddat1((char *)&np,la,sizeof(DBPatchNU));
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

        DBstatus  DBread_patches(
        DBSurf   *surpek,
        DBPatch **pptpat)

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
 *      14/3/94    Nya DBPatch, J. Kjellander
 *      1996-01-29 CON_PAT, J. Kjellander
 *      1996-07-04 Bug, PRO_PAT, J.Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-11-05 DBPatchNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   *tabpek,*ptab,tabla;
    DBint    nu,nv,i,j,tabsiz;
    DBPatch *patpek;

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
    if ( (*pptpat=(DBPatch *)DBcreate_patches(TOP_PAT,nu*nv)) == NULL )
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
        else rddat1((char *)patpek,*ptab,sizeof(DBPatch));
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
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchC));
          break;

          case RAT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(RAT_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchR));
          break;

          case LFT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(LFT_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchL));
          break;

          case FAC_PAT:
          if ( (patpek->spek_c=DBcreate_patches(FAC_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchF));
          break;

          case PRO_PAT:
          if ( (patpek->spek_c=DBcreate_patches(PRO_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP));
          break;

          case CON_PAT:
          if ( (patpek->spek_c=DBcreate_patches(CON_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchN));
          break;

          case P3_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P3_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP3));
          break;

          case P5_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P5_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP5));
          break;

          case P7_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P7_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP7));
          break;

          case P9_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P9_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP9));
          break;

          case P21_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P21_PAT,1)) == NULL )
            return(erpush("GM1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP21));
          break;

          case NURB_PAT:
          if ( (patpek->spek_c=DBcreate_patches(NURB_PAT,1)) == NULL )
            return(erpush("GM1053",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchNU));
          if ( gmrdnp((DBPatchNU *)patpek->spek_c) < 0 )
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
        DBSurf  *surpek,
        DBPatch *patpek,
        short    iu,
        short    iv)

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
 *      1997-11-05 DBPatchNU, J.Kjellander
 *      2006-11-20 if patpek->spek_c != NULL S. Larsson
 *
 ******************************************************!*/

  {
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
    rddat1((char *)patpek,patla,sizeof(DBPatch));
    patpek->spek_c = geopat;
/*
***Nu kan vi läsa även den geometriska patchen.
*/

    if (patpek->spek_c != NULL)
      {
      switch ( patpek->styp_pat )
        {
        case CUB_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchC));
        break;

        case RAT_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchR));
        break;

        case LFT_PAT:
        rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchL));
        break;

        case FAC_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchF));
        break;

        case PRO_PAT:
        rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP));
        break;

        case CON_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchN));
        break;

        case P3_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP3));
        break;

        case P5_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP5));
        break;

        case P7_PAT:
        rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP7));
        break;

        case P9_PAT:
        rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP9));
        break;

        case P21_PAT:
        rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP21));
        break;

        case TOP_PAT:
        rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatch));
        break;
      
        default:
        return(erpush("GM1073",""));
        }
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
 *      17/3/94    DBPatch, J. Kjellander
 *      1996-01-29 DBPatchN, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-05-05 DBPatchNU, J.Kjellander
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
      case CUB_PAT:  nbytes = antal*sizeof(DBPatchC);   break;
      case RAT_PAT:  nbytes = antal*sizeof(DBPatchR);   break;
      case LFT_PAT:  nbytes = antal*sizeof(DBPatchL);   break;
      case FAC_PAT:  nbytes = antal*sizeof(DBPatchF);   break;
      case PRO_PAT:  nbytes = antal*sizeof(DBPatchP);   break;
      case CON_PAT:  nbytes = antal*sizeof(DBPatchN);   break;
      case P3_PAT:   nbytes = antal*sizeof(DBPatchP3);  break;
      case P5_PAT:   nbytes = antal*sizeof(DBPatchP5);  break;
      case P7_PAT:   nbytes = antal*sizeof(DBPatchP7);  break;
      case P9_PAT:   nbytes = antal*sizeof(DBPatchP9);  break;
      case P21_PAT:  nbytes = antal*sizeof(DBPatchP21); break;
      case NURB_PAT: nbytes = antal*sizeof(DBPatchNU);  break;
      case TOP_PAT:  nbytes = antal*sizeof(DBPatch);    break;
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

        DBstatus DBcreate_NURBS(DBPatchNU *patpek)

/*      Allokerar minne för NURBS-data i en DBPatchNU.
 *
 *      In: patpek => Pekare till delvis ifylld DBPatchNU.
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
        DBSurf  *surpek,
        DBPatch *patpek)

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
 *      21/3/94    Nya DBPatch, J. Kjellandert
 *      1996-01-29 DBPatchN, J. Kjellander
 *      1996-07-04 Polynompatchar, J.Kjellander
 *      1997-05-05 DBPatchNU, J.Kjellander
 *
 ******************************************************!*/

  {
    DBint      i,j;
    DBPatch   *ppat;
    DBPatchNU *nurbpat;

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
          nurbpat = (DBPatchNU *)ppat->spek_c;
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

        DBstatus   DBread_getrimcurves(
        DBSurf    *surpek,
        DBSegarr **ppgetrimcurves)

/*      Allocates memory for, and reads, the geometric
 *      trimcurves of a surface.
 *
 *      In: surpek = Ptr to surface
 *
 *      Out: *ppgetrimcurves = A pointer to an array of
 *                             pointers to trimcurves.
 *
 *      (C)2007-01-01 J. Kjellander
 *
 ******************************************************!*/

  {
  }

/********************************************************/
/*!******************************************************/
/* ?????? Skall inte denna heta DBfree_segarrs ???????? */

        DBstatus  DBfree_getrimcurves(
        DBSurf   *surpek,
        DBSegarr *pgetrimcurves)

/*      Free's all memory allocated for geometrical
 *      trimcurves.
 *
 *      In: surpek        = Ptr to surface
 *          pgetrimcurves = Ptr to array of trimcurves
 *
 *      (C)2007-01-01 J. Kjellander
 *
 ******************************************************!*/

  {
  }

/********************************************************/
/*!******************************************************/

        DBSegarr *DBcreate_segarrs(DBint nsegarr)

/*      Allocates C memory for DBSegarrs.
 *
 *      In: nsegarr = Number of segarrs
 *
 *      (C)2007-01-01 J. Kjellander
 *
 ******************************************************!*/

  {
   return(NULL);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_sur_grwire(
        DBSurf  *surpek,
        DBSeg   *sptarr[])

/*      Lagrar en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
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

        DBstatus DBread_sur_grwire(
        DBSurf  *surpek,
        DBSeg   *sptarr[])

/*      Läser en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
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

        DBstatus DBdelete_sur_grwire(DBSurf *surpek)

/*      Suddar en ytas grafiska segment.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
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

        DBstatus DBfree_sur_grwire(DBSeg *sptarr[])

/*      Återlämnar minne för grafiska segment.
 *
 *      In: sptarr = 6 DBSeg-pekare.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( sptarr[0] != NULL ) v3free((char *)sptarr[0],"DBfree_sur_grwire");
    if ( sptarr[1] != NULL ) v3free((char *)sptarr[1],"DBfree_sur_grwire");
    if ( sptarr[2] != NULL ) v3free((char *)sptarr[2],"DBfree_sur_grwire");
    if ( sptarr[3] != NULL ) v3free((char *)sptarr[3],"DBfree_sur_grwire");
    if ( sptarr[4] != NULL ) v3free((char *)sptarr[4],"DBfree_sur_grwire");
    if ( sptarr[5] != NULL ) v3free((char *)sptarr[5],"DBfree_sur_grwire");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_sur_grsur(
        DBSurf   *surpek,
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

        DBstatus  DBread_sur_grsur(
        DBSurf   *surpek,
        GLfloat **kvu,
        GLfloat **kvv,
        GLfloat **cpts)

/*      Läser en ytas B-splineapproximation. C-minne
 *      allokeras och pekare returneras.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
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
   if ( (*kvu=(GLfloat *)v3mall(size,"DBread_sur_grsur")) == NULL ) return(-2);
   if ( size < PAGSIZ ) rddat1((char *)*kvu,surpek->pkvu_su,size);
   else                 rddat2((char *)*kvu,surpek->pkvu_su,size);
  
   size = surpek->nkv_su*sizeof(GLfloat);
   if ( (*kvv=(GLfloat *)v3mall(size,"DBread_sur_grsur")) == NULL ) return(-2);
   if ( size < PAGSIZ ) rddat1((char *)*kvv,surpek->pkvv_su,size);
   else                 rddat2((char *)*kvv,surpek->pkvv_su,size);
/*
***Samma för kontrollpunkter.
*/
   size = (surpek->nku_su - surpek->uorder_su) *
          (surpek->nkv_su - surpek->vorder_su) * 3 * sizeof(GLfloat);

   if ( (*cpts=(GLfloat *)v3mall(size,"DBread_sur_grsur")) == NULL ) return(-2);

   if ( size < PAGSIZ ) rddat1((char *)*cpts,surpek->pcpts_su,size);
   else                 rddat2((char *)*cpts,surpek->pcpts_su,size);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_sur_grsur(DBSurf *surpek)

/*      Suddar en ytas B-splineapproximation.
 *
 *      In: surpek => Pekare till en DBSurf-structure.
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

        DBstatus DBfree_sur_grsur(
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
    if ( kvu != NULL ) v3free((char *)kvu,"DBfree_sur_grsur");
    if ( kvv != NULL ) v3free((char *)kvv,"DBfree_sur_grsur");
    if ( cpts != NULL ) v3free((char *)cpts,"DBfree_sur_grsur");

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBptr wrgseg(
        short nseg,
        DBSeg segdat[])

/*      Lagrar nseg stycken DBSeg och returnerar GM-adress.
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
      if ( wrdat1((char *)&segdat[i-1],&la,sizeof(DBSeg)) < 0 ) return(0);
      }

  return(la);
  }

/********************************************************/
/*!******************************************************/

 static DBSeg *rdgseg(
        short  nseg,
        DBptr  la)

/*      Läser nseg stycken DBSeg och returnerar C-adress.
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
    DBSeg *segpek,*ptr_seg;

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
      rddat1((char *)ptr_seg,la_seg,sizeof(DBSeg));
      la_seg = ptr_seg->nxt_seg;
      ++ptr_seg;
      }

    return(segpek);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus dlgseg(
        short    nseg,
        DBptr    la)

/*      Dödar nseg stycken DBSeg vid la.
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
    DBSeg seg;

    la_seg = la;

    for ( i=0; i<nseg; ++i)
      {
      rddat1((char *)&seg,la_seg,sizeof(DBSeg));
      rldat1(la_seg,sizeof(DBSeg));
      la_seg = seg.nxt_seg;
      }

    return(0);
  }

/********************************************************/
