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
*    DBfree_getrimcurves();  Free's allocated trimcurve memory
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

#ifdef UNIX
#include <GL/gl.h>
#include <GL/glu.h> /*SLTEST*/
#endif
/*
***Prototypes for internal functions.
*/
static DBptr    write_segments (int nseg, DBSeg *segdat);
static DBSeg   *read_segments(int nseg, DBptr la);
static DBstatus delete_segments(int nseg,DBptr la);
static DBstatus write_NURBS(DBPatchNU *np, DBptr *pla);
static DBstatus read_NURBS(DBPatchNU *np);
static DBstatus delete_NURBS(DBptr la);

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
 *      2007-01-11 Trimcurves, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr    *tabpek,*ptab,tabla,la;
    DBint     nu,nv,i,j,size,tabsiz;
    DBPatch  *patpek,*spek;
    DBSegarr *saptr;

/*
***Allocate C-memory for the DB patch table.
*/
    nu = surpek->nu_su;
    nv = surpek->nv_su;
    tabsiz = nu*nv*sizeof(DBptr);
    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBinsert_surface")) == NULL )
      return(erpush("DB1043",""));
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
          write_NURBS((DBPatchNU *)patpek->spek_c,&la);
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
***If trimcurves exist, store them now.
*/
    if ( surpek->ntrim_su > 0 )
      {
      saptr = ptrim;

      for ( i=0; i<surpek->ntrim_su; ++i )
        {
        saptr->segptr_db = write_segments(saptr->nseg,saptr->segptr_c);
      ++saptr;
        }

      size = surpek->ntrim_su*sizeof(DBSegarr);
      if ( size <= PAGSIZ ) wrdat1((char *)ptrim,&la,size);
      else                  wrdat2((char *)ptrim,&la,size);
      surpek->getrim_su = la;
      }
    else surpek->getrim_su = DBNULL;
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
***GMSUR1 has no trimcurves or linewidth.
*/
      case GMPOSTV1:
      V3MOME(hedpek,surpek,sizeof(GMSUR1));
      surpek->ntrim_su         = 0;
      surpek->getrim_su        = DBNULL;
      surpek->ngrwborder_su    = 0;
      surpek->grwborder_su     = DBNULL;
      surpek->ngrwiso_su       = 0;
      surpek->grwiso_su        = DBNULL;
      surpek->vertextype_su    = GL_MAP2_VERTEX_3;
      surpek->grstrim_su       = DBNULL;
      surpek->nustep_su        = -1.0;
      surpek->nvstep_su        = -1.0;
      surpek->wdt_su           = 0.0;
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
      surpek->wdt_su           = 0.0;
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

/*      Uppdates (owerwrites) a DBSurf.
 *
 *      In: surpek => C-ptr to DBSurf-structure.
 *          la     => Address in DB.
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

/*      Deletes a surface from DB. Patches, trimcurves and
 *      graphical representation is also deleted.
 *
 *      In: la         => Surface DB address
 *
 *      Error:  0      => Ok.
 *              GM1123 => Can't malloc for trimcurves
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
 *      2007-01-12 Trimcurves, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr    *tabpek,*ptab,tabla;
    DBint     nu,nv,i,j,size,tabsiz;
    DBSurf    sur;
    DBPatch   toppat;
    DBSegarr *saptr;

/*
***Read DBSurf.
*/
    DBread_surface(&sur,la);
/*
***Delete graphical wireframe representation from DB.
*/
    DBdelete_sur_grwire(&sur);
/*
***Delete graphical surface representation from DB.
*/
    DBdelete_sur_grsur(&sur);
/*
***If trimcurves exist, delete them from DB.
*/
    if ( sur.ntrim_su > 0 )
      {
      size = sur.ntrim_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBdelete_surface")) == NULL ) return(erpush("DB1123",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,sur.getrim_su,size);
      else                  rddat2((char *)saptr,sur.getrim_su,size);

      for ( i=0; i<sur.ntrim_su; ++i )
        {
        delete_segments((saptr+i)->nseg,(saptr+i)->segptr_db);
        }
      if ( size <= PAGSIZ ) rldat1(sur.getrim_su,size);
      else                  rldat2(sur.getrim_su,size);

      v3free((char *)saptr,"DBdelete_surface");
      }
/*
***Allocate C-memory for patch table.
*/
    nu     = sur.nu_su;
    nv     = sur.nv_su;
    tabsiz = nu*nv*sizeof(DBptr);

    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBdelete_surface")) == NULL )
      return(erpush("DB1043",""));
    ptab = tabpek;
/*
***Read the patch table and then delete it from DB.
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
***Delete the patches from DB.
*/
    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
/*
***First the topological patch. NUL_PAT does not need to be deleted.
*/
        if ( *ptab != DBNULL )
          {
          rddat1((char *)&toppat,*ptab,sizeof(DBPatch));
          rldat1(*ptab,sizeof(DBPatch));
/*
***Then the geometrical patch.
*/
          switch ( toppat.styp_pat )
            {
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
            delete_NURBS(toppat.spek_gm);
            rldat1(toppat.spek_gm,sizeof(DBPatchNU));
            break;
            }
          }
/*
***Next topological patch.
*/
        ++ptab;
        }
      }
/*
***There is a theoretical chance that a new surface could
***end up on the same DBptr as the deleted surface. DB
***always tries to reuse memory if possible. If that
***happens DBread_one_patch() might have an old patch
***in it's cahce. To avoid that we init DBread_one_patch()
***here.
*/
    DBread_one_patch(NULL,NULL,0,0);
/*
***Return C-memory allocated for the patch table.
*/
    v3free(tabpek,"DBdelete_surface");
/*
***Finally, delete the DBSurf itself from DB.
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
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  DBread_patches(
        DBSurf   *surpek,
        DBPatch **pptpat)

/*      Reads all patches of a surface, topological and
 *      geometrical. C-memory for patches is allocated.
 *
 *      In:   surpek = C-ptr to DBSurf.
 *
 *      Out: *pptpat = C-pointer to memory allocated and
 *                    filled with patch data.
 *
 *      Error: GM1043 = Can't malloc for patch table
 *             GM1033 = Can't malloc for topological patches
 *             GM1063 = Can't malloc for geometrical patches
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
***Allocate C memory for the patch table.
*/
    nu = surpek->nu_su;
    nv = surpek->nv_su;
    tabsiz = nu*nv*sizeof(DBptr);
    if ( (tabpek=(DBptr *)v3mall(tabsiz,"DBread_patches")) == NULL )
      return(erpush("DB1043",""));
    ptab = tabpek;
/*
***Read the patch table from DB.
*/
    tabla = surpek->ptab_su;
    if ( tabsiz <= PAGSIZ ) rddat1((char *)tabpek,tabla,tabsiz);
    else                    rddat2((char *)tabpek,tabla,tabsiz);
/*
***Allocate memory for topological patches.
*/
    if ( (*pptpat=(DBPatch *)DBcreate_patches(TOP_PAT,nu*nv)) == NULL )
      return(erpush("DB1033",""));
    patpek = *pptpat;
/*
***Read the patches from DB.
*/
    for ( i=0; i<nu; ++i )
      {
      for ( j=0; j<nv; ++j )
        {
/*
***First the topological patch. If it is a NUL_PAT
***we just create one here without reading anything.
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
***Then the geometrical patch.
*/
        switch ( patpek->styp_pat )
          {
          case CUB_PAT:
          if ( (patpek->spek_c=DBcreate_patches(CUB_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchC));
          break;

          case RAT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(RAT_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchR));
          break;

          case LFT_PAT:
          if ( (patpek->spek_c=DBcreate_patches(LFT_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchL));
          break;

          case FAC_PAT:
          if ( (patpek->spek_c=DBcreate_patches(FAC_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchF));
          break;

          case PRO_PAT:
          if ( (patpek->spek_c=DBcreate_patches(PRO_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP));
          break;

          case CON_PAT:
          if ( (patpek->spek_c=DBcreate_patches(CON_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchN));
          break;

          case P3_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P3_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP3));
          break;

          case P5_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P5_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP5));
          break;

          case P7_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P7_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP7));
          break;

          case P9_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P9_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP9));
          break;

          case P21_PAT:
          if ( (patpek->spek_c=DBcreate_patches(P21_PAT,1)) == NULL )
            return(erpush("DB1063",""));
          rddat2(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchP21));
          break;

          case NURB_PAT:
          if ( (patpek->spek_c=DBcreate_patches(NURB_PAT,1)) == NULL )
            return(erpush("DB1053",""));
          rddat1(patpek->spek_c,patpek->spek_gm,sizeof(DBPatchNU));
          if ( read_NURBS((DBPatchNU *)patpek->spek_c) < 0 )
            return(erpush("DB1063",""));
          break;
/*
***If the geometric patch is topological no memory is allocated.
*/
          case TOP_PAT:
          patpek->spek_c = (char *) (*pptpat +
                           (patpek->su_pat-1)*nv + patpek->sv_pat - 1);
          break;
          }
/*
***Next topological patch.
*/
        ++ptab;
        ++patpek;
        }
      }
/*
***The end.
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

/*      Reads one patch. Includes a cache mechanism that
 *      speeds repeated calls within the same surface.
 *      This function is typically used if you only need
 *      a limited number of patches within the same surface.
 *      Otherwise, use DBread_patches() which will read all
 *      patches.
 *
 *      To empty the cache, call with surpek = NULL.
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
        return(erpush("DB1043",""));
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
        return(erpush("DB1073",""));
        }
      }
    return(0);
  }

/********************************************************/
/*!******************************************************/

        char *DBcreate_patches(
        DBint typ,
        DBint antal)

/*      Allocates C memory for patch-data.
 *
 *      In: typ   = Patch type.
 *          antal = Number of patches, nu*nv.
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

/*      Allocates C memory for NURBS data in a DBPatchNU.
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

/*      Free's C memory allocated for patches.
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
        DBSurf    *surptr,
        DBSegarr **ppgetrimcurves)

/*      Allocates memory for, and reads, the geometric
 *      trimcurves of a surface.
 *
 *      In: surptr = C ptr to surface
 *
 *      Out: *ppgetrimcurves = A pointer to an array of
 *                             pointers to trimcurves.
 *
 *      (C)2007-01-01 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i,size;
    DBSegarr *saptr;

    if ( surptr->ntrim_su > 0 )
      {
      size = surptr->ntrim_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBread_getrimcurves")) == NULL ) return(erpush("DB1123",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,surptr->getrim_su,size);
      else                  rddat2((char *)saptr,surptr->getrim_su,size);

      *ppgetrimcurves = saptr;

      for ( i=0; i<surptr->ntrim_su; ++i )
        {
        saptr->segptr_c = read_segments(saptr->nseg,saptr->segptr_db);
      ++saptr;
        }
      }
    else *ppgetrimcurves = NULL;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  DBfree_getrimcurves(
        DBSurf   *surptr,
        DBSegarr *pgetrimcurves)

/*      Free's all memory allocated for geometrical
 *      trimcurves, both seggarr's and segments.
 *
 *      In: surptr        = C ptr to surface
 *          pgetrimcurves = C ptr to array of trimcurves
 *
 *      (C)2007-01-17 J. Kjellander
 *
 ******************************************************!*/

  {
    int i;

/*
***Free the trimcurve segments.
*/
    if ( surptr->ntrim_su > 0  &&  pgetrimcurves != NULL ) 
      {
      for ( i=0; i<surptr->ntrim_su; ++i )
        {
        v3free((char *)(pgetrimcurves+i)->segptr_c,"DBfree_getrimcurves");
        }
/*
***Free memory for the DBSegarr's.
*/
      v3free((char *)pgetrimcurves,"DBfree_getrimcurves");
      } 
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBSegarr *DBcreate_segarrs(DBint nsegarr)

/*      Allocates C memory for DBSegarr's. Memory
 *      for segments is not allocated.
 *
 *      In: nsegarr = Number of DBsegarr's
 *
 *      Return *saptr = Pointer to allocated memory or NULL.
 *
 *      (C)2007-01-05 J. Kjellander
 *
 ******************************************************!*/

  {
    int       size;
    DBSegarr *saptr;

/*
***Allocate C-memory for nsegarr DBsegarr's.
*/
    if ( nsegarr > 0 )
      {
      size = nsegarr*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBcreate_segarrs")) == NULL ) return(NULL);
      return(saptr);
      }
    else return(NULL);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_sur_grwire(
        DBSurf   *psurf,
        DBSegarr *pborder,
        DBSegarr *piso)

/*      Adds a graphical wireframe representation to
 *      a surface.
 *
 *      In: psurf   = C-pointer to surface
 *          pborder = C-Pointer to array of DBSegarr's
 *                    for outer loop.
 *          piso    = C-Pointer to array of DBSegarr's
 *                    for midcurves.
 *
 *      (C)2007-01-05 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i,size;
    DBptr     la;
    DBSegarr *saptr;

/*
***Loop through the border curves and store associated segments.
***Then store the segarr-array and put its address in the surface.
*/
    saptr = pborder;

    for ( i=0; i<psurf->ngrwborder_su; ++i )
      {
      saptr->segptr_db = write_segments(saptr->nseg,saptr->segptr_c);
    ++saptr;
      }

    if ( psurf->ngrwborder_su > 0 )
      {
      size = psurf->ngrwborder_su*sizeof(DBSegarr);
      if ( size <= PAGSIZ ) wrdat1((char *)pborder,&la,size);
      else                  wrdat2((char *)pborder,&la,size);
      psurf->grwborder_su = la;
      }
    else psurf->grwborder_su = DBNULL;
/*
***Same for mid curves.
*/
    saptr = piso;

    for ( i=0; i<psurf->ngrwiso_su; ++i )
      {
      saptr->segptr_db = write_segments(saptr->nseg,saptr->segptr_c);
    ++saptr;
      }

    if ( psurf->ngrwiso_su > 0 )
      {
      size = psurf->ngrwiso_su*sizeof(DBSegarr);
      if ( size <= PAGSIZ ) wrdat1((char *)piso,&la,size);
      else                  wrdat2((char *)piso,&la,size);
      psurf->grwiso_su = la; 
      }
    else psurf->grwiso_su = DBNULL;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus   DBread_sur_grwire(
        DBSurf    *psurf,
        DBSegarr **pborder,
        DBSegarr **piso)

/*      Reads a surface graphical wireframe representation.
 *      C-memory for segarr's and segments is allocated.
 *
 *      In:  psurf    = C-pointer to surface
 *
 *      Out: *pborder = C-Pointer to array of DBSegarr's
 *                      for outer loop.
 *           *piso    = C-Pointer to array of DBSegarr's
 *                      for midcurves.
 *
 *      Return: GM1113 = Can't malloc
 *
 *      (C)2007-01-05 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i,size;
    DBSegarr *saptr;

/*
***Allocate C-memory for the bordercurve DBsegarr's and
***read them from DB.
*/
    if ( psurf->ngrwborder_su > 0 )
      {
      size = psurf->ngrwborder_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBread_sur_grwire")) == NULL ) return(erpush("DB1113",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,psurf->grwborder_su,size);
      else                  rddat2((char *)saptr,psurf->grwborder_su,size);
/*
***Return pointer to DBsegarr's.
*/
     *pborder = saptr;
/*
***Read the border segments from the DB.
*/
      for ( i=0; i<psurf->ngrwborder_su; ++i )
        {
        saptr->segptr_c = read_segments(saptr->nseg,saptr->segptr_db);
      ++saptr;
        }
      }
    else *pborder = NULL;
/*
***Same for iso curves.
*/
    if ( psurf->ngrwiso_su > 0 )
      {
      size = psurf->ngrwiso_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBread_sur_grwire")) == NULL ) return(erpush("DB1113",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,psurf->grwiso_su,size);
      else                  rddat2((char *)saptr,psurf->grwiso_su,size);

      *piso = saptr;

      for ( i=0; i<psurf->ngrwiso_su; ++i )
        {
        saptr->segptr_c = read_segments(saptr->nseg,saptr->segptr_db);
      ++saptr;
        }
      }
    else *piso = NULL;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus   DBdelete_sur_grwire(
        DBSurf    *psurf)

/*      Deletes a surface graphical wireframe representation
 *      from the DB. C-memory for segarr's and segments is
 *      not free'd.
 *
 *      In:  psurf    = C-pointer to surface
 *
 *      (C)2007-01-05 J. Kjellander
 *
 ******************************************************!*/

  {
    int       i,size;
    DBSegarr *saptr;

/*
***Read the bordercurve DBsegarr's from DB.
*/
    if ( psurf->ngrwborder_su > 0 )
      {
      size = psurf->ngrwborder_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBdelete_sur_grwire")) == NULL ) return(erpush("DB1113",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,psurf->grwborder_su,size);
      else                  rddat2((char *)saptr,psurf->grwborder_su,size);
/*
***Delete the bordercurve segments from the DB.
*/
      for ( i=0; i<psurf->ngrwborder_su; ++i )
        {
        delete_segments((saptr+i)->nseg,(saptr+i)->segptr_db);
        }
/*
***Delete the bordercurve DBsegarr's from the DB.
*/
      if ( size <= PAGSIZ ) rldat1(psurf->grwborder_su,size);
      else                  rldat2(psurf->grwborder_su,size);
/*
***Free C-memory allocated for temporary DBSegarr's.
*/
      v3free((char *)saptr,"DBdelete_sur_grwire");
      }
/*
***Same for iso curves.
*/
    if ( psurf->ngrwiso_su > 0 )
      {
      size = psurf->ngrwiso_su*sizeof(DBSegarr);
      if ( (saptr=v3mall(size,"DBdelete_sur_grwire")) == NULL ) return(erpush("DB1113",""));
      if ( size <= PAGSIZ ) rddat1((char *)saptr,psurf->grwiso_su,size);
      else                  rddat2((char *)saptr,psurf->grwiso_su,size);

      for ( i=0; i<psurf->ngrwiso_su; ++i )
        {
        delete_segments((saptr+i)->nseg,(saptr+i)->segptr_db);
        }

      if ( size <= PAGSIZ ) rldat1(psurf->grwiso_su,size);
      else                  rldat2(psurf->grwiso_su,size);

      v3free((char *)saptr,"DBdelete_sur_grwire");
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus  DBfree_sur_grwire(
        DBSurf   *psurf,
        DBSegarr *pborder,
        DBSegarr *piso)

/*      Free's C memory allocated for surface graphical
 *      wireframe representation.
 *
 *      In: psurf   = C-pointer to surface
 *          pborder = C-Pointer to array of DBSegarr's
 *                    for outer loop.
 *          piso    = C-Pointer to array of DBSegarr's
 *                    for midcurves.
 *
 *      (C)2007-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *sptr;

/*
***Free the bordercurve segments.
*/
    for ( i=0; i<psurf->ngrwborder_su; ++i )
      {
      sptr = (char *)(pborder+i)->segptr_c;
      if ( sptr != NULL  &&  (pborder+i)->nseg > 0 ) v3free(sptr,"DBfree_sur_grwire");
      }
/*
***Free memory for the DBSegarr's.
*/
    if ( pborder != NULL ) v3free((char *)pborder,"DBfree_sur_grwire");
/*
***Same for iso curves.
*/
    for ( i=0; i<psurf->ngrwiso_su; ++i )
      {
      sptr = (char *)(piso+i)->segptr_c;
      if ( sptr != NULL  &&  (piso+i)->nseg > 0 ) v3free(sptr,"DBfree_sur_grwire");
      }

    if ( piso != NULL ) v3free((char *)piso,"DBfree_sur_grwire");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus        DBadd_sur_grsur(
        DBSurf         *surpek,
        GLfloat        *kvu,
        GLfloat        *kvv,
        GLfloat        *cpts,
        DBGrstrimcurve *ptrim)

/*      Stores the graphical surface (NURBS) representation
 *      of a (trimmed) surface in the DB.
 *
 *      In: surpek  => C ptr to surface
 *          kvu,kvv => C ptr to arrays of knot values
 *          cpts    => C ptr to control points
 *          ptrim   => C ptr to trimcurves
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 *      2007-01-18 Trim curves, J.Kjellander
 *
 ******************************************************!*/

  {
   DBint           i,nbytes;
   DBint           gl_cpts_size;
   DBGrstrimcurve *tcptr;
/*
***Store surface knot values in DB.
*/
   nbytes = surpek->nku_su*sizeof(GLfloat);
   if ( nbytes < PAGSIZ ) wrdat1((char *)kvu,&surpek->pkvu_su,nbytes);
   else                   wrdat2((char *)kvu,&surpek->pkvu_su,nbytes);

   nbytes = surpek->nkv_su*sizeof(GLfloat);
   if ( nbytes < PAGSIZ ) wrdat1((char *)kvv,&surpek->pkvv_su,nbytes);
   else                   wrdat2((char *)kvv,&surpek->pkvv_su,nbytes);
/*
***Calculate the size of a control point and store them in DB.
*/
   if ( surpek->vertextype_su == GL_MAP2_VERTEX_4 ) gl_cpts_size=4;
   else                                             gl_cpts_size=3;

   nbytes = (surpek->nku_su - surpek->uorder_su) *
            (surpek->nkv_su - surpek->vorder_su) *
             gl_cpts_size * sizeof(GLfloat);

   if ( nbytes < PAGSIZ ) wrdat1((char *)cpts,&surpek->pcpts_su,nbytes);
   else                   wrdat2((char *)cpts,&surpek->pcpts_su,nbytes);
/*
***If we have trimcurves, store them too.
*/
   if ( surpek->ntrim_su > 0 )
     {
     tcptr = ptrim;

     for ( i=0; i<surpek->ntrim_su; ++i )
       {
/*
***Store trimcurve knot values in DB.
*/
       nbytes = tcptr->nknots * sizeof(GLfloat);
       if ( nbytes <= PAGSIZ ) wrdat1((char *)tcptr->knots_c,&tcptr->knots_db,nbytes);
       else                    wrdat2((char *)tcptr->knots_c,&tcptr->knots_db,nbytes);
/*
***Calculate the size of a trimcurve controlpoint and store cpts in the DB.
*/
       if ( tcptr->vertextype == GLU_MAP1_TRIM_3 ) gl_cpts_size=3;   /* SLTEST GL_MAP1_VERTEX_3 was wrong */
       else                                         gl_cpts_size=2;

       nbytes = (tcptr->nknots - tcptr->order) * gl_cpts_size * sizeof(GLfloat);
       if ( nbytes <= PAGSIZ ) wrdat1((char *)tcptr->cpts_c,&tcptr->cpts_db,nbytes);
       else                    wrdat2((char *)tcptr->cpts_c,&tcptr->cpts_db,nbytes);
/*
***Next trimcurve.
*/
     ++tcptr;
       }
/*
***Store DBGrstrimcurve array in DB
*/
     nbytes = surpek->ntrim_su * sizeof(DBGrstrimcurve);
     if ( nbytes <= PAGSIZ ) wrdat1((char *)ptrim,&surpek->grstrim_su,nbytes);
     else                    wrdat2((char *)ptrim,&surpek->grstrim_su,nbytes);
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus         DBread_sur_grsur(
        DBSurf          *surpek,
        GLfloat        **kvu,
        GLfloat        **kvv,
        GLfloat        **cpts,
        DBGrstrimcurve **trim)

/*      Reads the graphical surface (NURBS) representation
 *      of a surface from the DB. C-memory for NURBS data
 *      with optional trimcurves is allocated.
 *
 *      In:  surpek => C ptr to surface
 *
 *      Out: *kvu   => C ptr to surface u knots
 *           *kvv   => C ptr to surface v knots
 *           *cpts  => C ptr to surface control points
 *           *trim  => C ptr to trim curves
 *
 *      Return:      0 => Ok
 *              GM1083 => Can't malloc for %s cpts
 *              GM1093 => Can't malloc for %s knots
 *              GM1123 => Can't malloc for trimcurves
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 *      2007-01-18 Trimcurves, J.Kjellander
 *
 ******************************************************!*/

  {
   int             i,nbytes,ncpts,gl_cpts_size;
   char            errbuf[20];
   DBGrstrimcurve *tcptr;

/*
***Allocate C memory for knots and read values from DB.
***Return error messages if memory can't be allocated.
*/
   nbytes = surpek->nku_su*sizeof(GLfloat);
   if ( (*kvu=(GLfloat *)v3mall(nbytes,"DBread_sur_grsur")) == NULL )
     {
     sprintf(errbuf,"%d",surpek->nku_su);
     return(erpush("DB1093",errbuf));
     }

   if ( nbytes < PAGSIZ ) rddat1((char *)*kvu,surpek->pkvu_su,nbytes);
   else                   rddat2((char *)*kvu,surpek->pkvu_su,nbytes);
  
   nbytes = surpek->nkv_su*sizeof(GLfloat);
   if ( (*kvv=(GLfloat *)v3mall(nbytes,"DBread_sur_grsur")) == NULL )
     {
     sprintf(errbuf,"%d",surpek->nkv_su);
     return(erpush("DB1093",errbuf));
     }

   if ( nbytes < PAGSIZ ) rddat1((char *)*kvv,surpek->pkvv_su,nbytes);
   else                   rddat2((char *)*kvv,surpek->pkvv_su,nbytes);
/*
***Same for control points.
*/
   if ( surpek->vertextype_su == GL_MAP2_VERTEX_4 ) gl_cpts_size=4;
   else                                             gl_cpts_size=3;

   ncpts = (surpek->nku_su - surpek->uorder_su) *
           (surpek->nkv_su - surpek->vorder_su);

   nbytes  = ncpts * gl_cpts_size * sizeof(GLfloat);

   if ( (*cpts=(GLfloat *)v3mall(nbytes,"DBread_sur_grsur")) == NULL )
     {
     sprintf(errbuf,"%d",ncpts);
     return(erpush("DB1083",errbuf));
     }

   if ( nbytes < PAGSIZ ) rddat1((char *)*cpts,surpek->pcpts_su,nbytes);
   else                   rddat2((char *)*cpts,surpek->pcpts_su,nbytes);
/*
***Optional trimcurves. Read array of DBGrstrimcurve.
*/
    if ( surpek->ntrim_su > 0 )
      {
      nbytes = surpek->ntrim_su*sizeof(DBGrstrimcurve);
      if ( (tcptr=v3mall(nbytes,"DBread_sur_grsur")) == NULL ) return(erpush("DB1123",""));
      if ( nbytes <= PAGSIZ ) rddat1((char *)tcptr,surpek->grstrim_su,nbytes);
      else                    rddat2((char *)tcptr,surpek->grstrim_su,nbytes);

     *trim = tcptr;
/*
***Read the individual trimcurces, knots first.
*/
      for ( i=0; i<surpek->ntrim_su; ++i )
        {
        nbytes = tcptr->nknots * sizeof(GLfloat);
        if ( (tcptr->knots_c=v3mall(nbytes,"DBread_sur_grsur")) == NULL )
          return(erpush("DB1123",""));
        if ( nbytes <= PAGSIZ ) rddat1((char *)tcptr->knots_c,tcptr->knots_db,nbytes);
        else                    rddat2((char *)tcptr->knots_c,tcptr->knots_db,nbytes);
/*
***Then the controlpoints. Size depends on vertextype.
*/
        if ( tcptr->vertextype == GLU_MAP1_TRIM_3 ) gl_cpts_size=3;    /*SLTEST GL_MAP1_VERTEX_3 was wrong */
        else                                         gl_cpts_size=2;

        nbytes = (tcptr->nknots - tcptr->order) * gl_cpts_size * sizeof(GLfloat);
        if ( (tcptr->cpts_c=v3mall(nbytes,"DBread_sur_grsur")) == NULL )
          return(erpush("DB1123",""));
        if ( nbytes <= PAGSIZ ) rddat1((char *)tcptr->cpts_c,tcptr->cpts_db,nbytes);
        else                    rddat2((char *)tcptr->cpts_c,tcptr->cpts_db,nbytes);
/*
***Next curve.
*/
      ++tcptr;
        }
      }
/*
***No trim curves.
*/
    else *trim = NULL;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_sur_grsur(DBSurf *surpek)

/*      Deletes the graphical surface (NURBS) representation
 *      of a surface in the DB.
 *
 *      In:     surpek => C pointer to DBSurf.
 *
 *      Return:      0 => Ok
 *              GM1123 => Can't malloc for trimcurves
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 *      2007-01-18 Trimcurves, J.Kjellander
 *
 ******************************************************!*/

  {
   int             i,nbytes,ncpts,gl_cpts_size;
   DBGrstrimcurve *tcptr,*trim;

/*
***Delete surface nodes from DB.
*/
   nbytes = surpek->nku_su*sizeof(GLfloat);
   if ( nbytes < PAGSIZ ) rldat1(surpek->pkvu_su,nbytes);
   else                   rldat2(surpek->pkvu_su,nbytes);

   nbytes = surpek->nkv_su*sizeof(GLfloat);
   if ( nbytes < PAGSIZ ) rldat1(surpek->pkvv_su,nbytes);
   else                   rldat2(surpek->pkvv_su,nbytes);
/*
***And surface contolpoints.
*/
   if ( surpek->vertextype_su == GL_MAP2_VERTEX_4 ) gl_cpts_size=4;
   else                                             gl_cpts_size=3;

   ncpts = (surpek->nku_su - surpek->uorder_su) *
           (surpek->nkv_su - surpek->vorder_su);

   nbytes  = ncpts * gl_cpts_size * sizeof(GLfloat);

   if ( nbytes < PAGSIZ ) rldat1(surpek->pcpts_su,nbytes);
   else                   rldat2(surpek->pcpts_su,nbytes);
/*
***And optional trimcurves. Read array of DBGrstrimcurve first.
*/
   if ( surpek->ntrim_su > 0 )
     {
     nbytes = surpek->ntrim_su * sizeof(DBGrstrimcurve);
     if ( (tcptr=v3mall(nbytes,"DBdelete_sur_grsur")) == NULL ) return(erpush("DB1123",""));
     if ( nbytes <= PAGSIZ ) rddat1((char *)tcptr,surpek->grstrim_su,nbytes);
     else                    rddat2((char *)tcptr,surpek->grstrim_su,nbytes);

     trim = tcptr;
/*
***Delete the individual curves, knots first.
*/
     for ( i=0; i<surpek->ntrim_su; ++i )
       {
       nbytes = tcptr->nknots * sizeof(GLfloat);
       if ( nbytes <= PAGSIZ ) rldat1(tcptr->knots_db,nbytes);
       else                    rldat2(tcptr->knots_db,nbytes);
/*
***Then the controlpoints. Size depends on vertextype.
*/
       if ( tcptr->vertextype == GLU_MAP1_TRIM_3 ) gl_cpts_size=3;    /* SLTEST GL_MAP1_VERTEX_3 was wrong */
       else                                         gl_cpts_size=2;

       nbytes = (tcptr->nknots - tcptr->order) * gl_cpts_size * sizeof(GLfloat);
       if ( nbytes <= PAGSIZ ) rldat1(tcptr->cpts_db,nbytes);
       else                    rldat2(tcptr->cpts_db,nbytes);

     ++tcptr;
       } 
/*
***Finally, delete the DBGrstrimcurve array.
*/
     nbytes = surpek->ntrim_su*sizeof(DBGrstrimcurve);
     if ( nbytes <= PAGSIZ ) rldat1(surpek->grstrim_su,nbytes);
     else                    rldat2(surpek->grstrim_su,nbytes);
     v3free((char *)trim,"DBdelete_sur_grsur");
     }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus        DBfree_sur_grsur(
        DBSurf         *surpek,
        GLfloat        *kvu,
        GLfloat        *kvv,
        GLfloat        *cpts,
        DBGrstrimcurve *trim)

/*      Free's C memory for the graphical surface (NURBS)
 *      representation of a surface.
 *
 *      In: surpek  = C ptr to surface
 *          kvu,kvv = Ptr to C-memory allocated for knots
 *          cpts    = Ptr C-memory allocated for cpts
 *          trim    = C ptr to optional trimcurves
 *
 *      (C)microform ab 1997-03-05 J. Kjellander
 *
 *      2007-01-18 Trimcurves, J.Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***Free memory for knots and control points.
*/
    if ( kvu  != NULL )  v3free((char *)kvu,"DBfree_sur_grsur");
    if ( kvv  != NULL )  v3free((char *)kvv,"DBfree_sur_grsur");
    if ( cpts != NULL )  v3free((char *)cpts,"DBfree_sur_grsur");
/*
***Free optional trimcurves.
*/
    if ( surpek->ntrim_su > 0  &&  trim != NULL)
      {
      for ( i=0; i<surpek->ntrim_su; ++i )
        {
        v3free((char *)(trim+i)->knots_c,"DBfree_sur_grsur");
        v3free((char *)(trim+i)->cpts_c,"DBfree_sur_grsur");
        }
/*
***Free memory for the DBSegarr's.
*/
      v3free((char *)trim,"DBfree_sur_grsur");
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBptr write_segments(
        int   nseg,
        DBSeg segdat[])

/*      Stores curve segments in DB.
 *
 *      In: segdat => C ptr to list of segments
 *          nseg   => Number of segments
 *
 *      Return:  > 0 => DB pointer to segment list
 *               = 0 => Can't store segments in DB
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
***Store segment list. Last segment first.
*/
    la = DBNULL;

    for ( i=nseg; i > 0; --i)
      {
      (segdat+i-1)->nxt_seg = la;
      if ( wrdat1((char *)&segdat[i-1],&la,sizeof(DBSeg)) < 0 ) return(0);
      }

  return(la);
  }

/********************************************************/
/*!******************************************************/

 static DBSeg *read_segments(
        int    nseg,
        DBptr  la)

/*      Reads curve segment list from DB. C memory for
 *      curve segment list is allocated.
 *
 *      In: nseg   => Number of segments.
 *          la     => Segment list DB address.
 *
 *      Return:  > 0 => C ptr to segment list.
 *               = 0 => Can'tmalloc 
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint  i;
    DBptr  la_seg;
    DBSeg *segpek,*ptr_seg;

/*
***nseg = 0 ?
*/
   if ( nseg == 0 ) return(NULL);
/*
***Allocate C memory.
*/
    if ( (segpek=DBcreate_segments((DBint)nseg)) == NULL ) return(NULL);
/*
***Read segment list from DB.
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

 static DBstatus delete_segments(
        int      nseg,
        DBptr    la)

/*      Deletes segment list from DB.
 *
 *      In: nseg   => Number of segments.
 *          la     => Address of segment list in DB
 *
 *      Return: Always = 0
 *
 *      (C)microform ab 14/3/94 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint i;
    DBptr la_seg;
    DBSeg seg;

/*
***DB address of first segment in the list.
*/
    la_seg = la;
/*
***Read segments and delete from DB.
*/
    for ( i=0; i<nseg; ++i)
      {
      rddat1((char *)&seg,la_seg,sizeof(DBSeg));
      rldat1(la_seg,sizeof(DBSeg));
      la_seg = seg.nxt_seg;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus   write_NURBS(
        DBPatchNU *np,
        DBptr     *pla)

/*      Stores NURBS patch in DB.
 *
 *      In:   np  = C ptr to DBPatchNU.
 *
 *      Out: *pla = DB pointer
 *
 *      Return: Always 0
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint size;

/*
***Store knots.
*/
    size = np->nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) wrdat1((char *)np->kvec_u,&(np->pku),size);
    else                  wrdat2((char *)np->kvec_u,&(np->pku),size);

    size = np->nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) wrdat1((char *)np->kvec_v,&(np->pkv),size);
    else                  wrdat2((char *)np->kvec_v,&(np->pkv),size);
/*
***Store control points.
*/
    size = (np->nk_u - np->order_u) *
           (np->nk_v - np->order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) wrdat1((char *)np->cpts,&(np->pcpts),size);
    else                  wrdat2((char *)np->cpts,&(np->pcpts),size);
/*
***Store patch data.
*/
    wrdat1((char *)np,pla,sizeof(DBPatchNU));
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus read_NURBS(DBPatchNU *np)

/*      Reads nurbspatch from DB. Allocates C memory.
 *
 *      In:   np = C ptr to partly initialized DBPatchNU.
 *
 *      Out: *np = DBPatchNU with C ptrs to NURBS data.
 *
 *      Return:    0 = Ok.
 *               < 0 = Can't malloc.
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBstatus status;
    DBint    size;

/*
***Allocate C memory for NURBS data.
*/
    if ( (status=DBcreate_NURBS(np)) < 0 ) return(status);
/*
***Read knots.
*/
    size = np->nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rddat1((char *)np->kvec_u,np->pku,size);
    else                  rddat2((char *)np->kvec_u,np->pku,size);

    size = np->nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rddat1((char *)np->kvec_v,np->pkv,size);
    else                  rddat2((char *)np->kvec_v,np->pkv,size);
/*
***Read control points.
*/
    size = (np->nk_u - np->order_u) *
           (np->nk_v - np->order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) rddat1((char *)np->cpts,np->pcpts,size);
    else                  rddat2((char *)np->cpts,np->pcpts,size);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static DBstatus delete_NURBS(DBptr la)

/*      Deletes NURBS data from DB. The NURBS
 *      patch itself is not deleted.
 *
 *      In: la => DB ptr to DBPatchNU.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 1997-11-05 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint   size;
    DBPatchNU np;

/*
***Read patch.
*/
    rddat1((char *)&np,la,sizeof(DBPatchNU));
/*
***Delete knots.
*/
    size = np.nk_u*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rldat1(np.pku,size);
    else                  rldat2(np.pku,size);

    size = np.nk_v*sizeof(DBfloat);
    if ( size <= PAGSIZ ) rldat1(np.pkv,size);
    else                  rldat2(np.pkv,size);
/*
***Delete control points.
*/
    size = (np.nk_u - np.order_u) *
           (np.nk_v - np.order_v) * sizeof(DBHvector);
    if ( size <= PAGSIZ ) rldat1(np.pcpts,size);
    else                  rldat2(np.pcpts,size);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
