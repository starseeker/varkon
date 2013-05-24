/**********************************************************************
*
*    dbmesh.c
*    ========
*
*    This file includes the following public functions:
*
*    DBinsert_mesh();   Inserts a mesh entity
*    DBread_mesh();     Reads a mesh entity
*    DBdelete_mesh();   Deletes a mesh entity
*    DBalloc_mesh();    Allocates C memory for mesh data
*    DBfree_mesh();     Deallocates mesh data memory
*
*
*    This file is part of the VARKON Database Library.
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

static DBVertex *vrtcache = NULL;
static DBptr     vrt_la   = DBNULL;
static DBHedge  *hdgcache = NULL;
static DBptr     hdg_la   = DBNULL;
static DBFace   *faccache = NULL;
static DBptr     fac_la   = DBNULL;
static DBfloat  *xfcache  = NULL;
static DBptr     xf_la    = DBNULL;

/*!******************************************************/

        DBstatus DBinsert_mesh(
        DBMesh  *mshptr,
        DBId    *idptr,
        DBptr   *laptr)

/*      Inserts a new mesh entity in the DB.
 *
 *      In: mshptr => Pointer to mesh data.
 *          idptr  => Pointer mesh ID.
 *
 *      Out: *laptr => The mesh DB-address.
 *
 *      Return: 0  => Ok.
 *             -1  => Illegal ID.
 *             -2  => ID-table full.
 *             -3  => No room for data.
 *             -4  => Entity already exists.
 *
 *      (C)2004 J. Kjellander, Örebro university
 *         2004-10-07 Extra floats, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr la;

/*
***Store the array of vertices.
*/
    if ( mshptr->nv_m > 0 )
      {
      if ( wrdat2((char *)mshptr->pv_m,&la,mshptr->nv_m*sizeof(DBVertex)) < 0 ) return(-3);
      else mshptr->pvarr_m = la;
      }
    else mshptr->pvarr_m = DBNULL;
/*
***Store the array of halfedges.
*/
    if ( mshptr->nh_m > 0 )
      {
      if ( wrdat2((char *)mshptr->ph_m,&la,mshptr->nh_m*sizeof(DBHedge)) < 0 ) return(-3);
      else mshptr->pharr_m = la;
      }
    else mshptr->pharr_m = DBNULL;
/*
***Store the array of faces.
*/
    if ( mshptr->nf_m > 0 )
      {
      if ( wrdat2((char *)mshptr->pf_m,&la,mshptr->nf_m*sizeof(DBFace)) < 0 ) return(-3);
      else mshptr->pfarr_m = la;
      }
    else mshptr->pfarr_m = DBNULL;
/*
***Store the array of extra floats.
*/
    if ( mshptr->nx_m > 0 )
      {
      if ( wrdat2((char *)mshptr->px_m,&la,mshptr->nx_m*sizeof(DBfloat)) < 0 ) return(-3);
      else mshptr->pxarr_m = la;
      }
    else mshptr->pxarr_m = DBNULL;
/*
***Type-specific data.
*/
    mshptr->hed_m.type = MSHTYP;     /* Typ = punkt */
    mshptr->hed_m.vers = GMPOSTV0;   /* Version */
/*
***Store the actual DBMesh object.
*/
    return(inpost((GMUNON *)mshptr,idptr,laptr,sizeof(DBMesh)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_mesh(
        DBMesh  *mshptr,
        DBptr    la,
        DBint    mode)

/*      Reads a mesh entity from the DB.
 *
 *      In: la       => Mesh DB-address.
 *          mode     => What to read. MESH_HEADER
 *                                    MESH_VERTEX
 *                                    MESH_HEDGE
 *                                    MESH_FACE
 *                                    MESH_XFLOAT
 *      Out: *mshptr => Mesh data.
 *
 *      Return: 0 = Ok.
 *
 *      (C)2004 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;
    char      errbuf[80];


/*
***Maybe we should just clear the read-cache.
*/
    if ( mshptr == NULL )
      {
      if ( vrtcache != NULL ) v3free((char *)vrtcache,"DBread_mesh");
      vrtcache = NULL;
      vrt_la   = DBNULL;
      if ( hdgcache != NULL ) v3free((char *)hdgcache,"DBread_mesh");
      hdgcache = NULL;
      hdg_la   = DBNULL;
      if ( faccache != NULL ) v3free((char *)faccache,"DBread_mesh");
      faccache = NULL;
      fac_la   = DBNULL;
      if ( xfcache != NULL ) v3free((char *)xfcache,"DBread_mesh");
      xfcache = NULL;
      xf_la   = DBNULL;
      return(0);
      }
/*
***Read the DBMesh entity header from the DB.
*/ 
   hedptr = (DBHeader *)gmgadr(la);
   V3MOME(hedptr,(char *)mshptr,sizeof(DBMesh));
/*
***What more to read ?
*/
   if ( mode == MESH_HEADER ) return(0);

   if ( !(mode & MESH_VERTEX) ) mshptr->nv_m = 0;
   if ( !(mode & MESH_HEDGE)  ) mshptr->nh_m = 0;
   if ( !(mode & MESH_FACE)   ) mshptr->nf_m = 0;
   if ( !(mode & MESH_XFLOAT) ) mshptr->nx_m = 0;
/*
***Read the array of vertices.
*/
      if ( mshptr->nv_m > 0 )
        {
        if ( mshptr->pvarr_m == vrt_la ) mshptr->pv_m = vrtcache;
        else
          {
          if ( vrtcache != NULL ) v3free((char *)vrtcache,"DBread_mesh");
          if ( (mshptr->pv_m=(DBVertex *)v3mall(mshptr->nv_m*sizeof(DBVertex),"DBread_mesh")) == NULL )
            {
            sprintf(errbuf,"%d",mshptr->nv_m);
            return(erpush("DB1103",errbuf));
            }
          rddat2((char *)mshptr->pv_m,mshptr->pvarr_m,mshptr->nv_m*sizeof(DBVertex));
          vrt_la = mshptr->pvarr_m;
          vrtcache = mshptr->pv_m;
          }
        }
/*
***Read the array of halfedges.
*/
    if ( mshptr->nh_m > 0 )
        {
        if ( mshptr->pharr_m == hdg_la ) mshptr->ph_m = hdgcache;
        else
          {
          if ( hdgcache != NULL ) v3free((char *)hdgcache,"DBread_mesh");
          if ( (mshptr->ph_m=(DBHedge *)v3mall(mshptr->nh_m*sizeof(DBHedge),"DBread_mesh")) == NULL )
            {
            sprintf(errbuf,"%d",mshptr->nh_m);
            return(erpush("DB1103",errbuf));
            }
          rddat2((char *)mshptr->ph_m,mshptr->pharr_m,mshptr->nh_m*sizeof(DBHedge));
          hdg_la = mshptr->pharr_m;
          hdgcache = mshptr->ph_m;
          }
        }
/*
***Read the array of faces.
*/
    if ( mshptr->nf_m > 0 )
        {
        if ( mshptr->pfarr_m == fac_la ) mshptr->pf_m = faccache;
        else
          {
          if ( faccache != NULL ) v3free((char *)faccache,"DBread_mesh");
          if ( (mshptr->pf_m=(DBFace *)v3mall(mshptr->nf_m*sizeof(DBFace),"DBread_mesh")) == NULL )
            {
            sprintf(errbuf,"%d",mshptr->nf_m);
            return(erpush("DB1103",errbuf));
            }
          rddat2((char *)mshptr->pf_m,mshptr->pfarr_m,mshptr->nf_m*sizeof(DBFace));
          fac_la = mshptr->pfarr_m;
          faccache = mshptr->pf_m;
          }
        }
/*
***Read the array of extra floats.
*/
    if ( mshptr->nx_m > 0 )
        {
        if ( mshptr->pxarr_m == xf_la ) mshptr->px_m = xfcache;
        else
          {
          if ( xfcache != NULL ) v3free((char *)xfcache,"DBread_mesh");
          if ( (mshptr->px_m=(DBfloat *)v3mall(mshptr->nx_m*sizeof(DBfloat),"DBread_mesh")) == NULL )
            {
            sprintf(errbuf,"%d",mshptr->nx_m);
            return(erpush("DB1103",errbuf));
            }
          rddat2((char *)mshptr->px_m,mshptr->pxarr_m,mshptr->nx_m*sizeof(DBfloat));
          xf_la = mshptr->pxarr_m;
          xfcache = mshptr->px_m;
          }
        }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_mesh(DBptr la)

/*      Deletes a mesh entity from the DB. The space
 *      allocated by the mesh data is freed.
 *
 *      In: la => Mesh DB-address.
 *
 *      Return: 0 = Ok.
 *
 *      (C)2004 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;
    DBMesh    mesh;

/*
***Read and delete the DBMesh entity from the DB.
*/ 
   hedptr = (DBHeader *)gmgadr(la);
   V3MOME(hedptr,(char *)&mesh,sizeof(DBMesh));
   rldat1(la,sizeof(DBMesh));
/*
***Delete the arrays.
*/
    if ( mesh.nv_m > 0 ) rldat2(mesh.pvarr_m,mesh.nv_m*sizeof(DBVertex));
    if ( mesh.nh_m > 0 ) rldat2(mesh.pharr_m,mesh.nh_m*sizeof(DBHedge));
    if ( mesh.nf_m > 0 ) rldat2(mesh.pfarr_m,mesh.nf_m*sizeof(DBFace));
    if ( mesh.nx_m > 0 ) rldat2(mesh.pxarr_m,mesh.nx_m*sizeof(DBfloat));


    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBalloc_mesh(DBMesh *mshptr)

/*      Allocates C-memory for mesh.
 *
 *      In: mshptr => C-pointer to mesh entity
 *
 *      Return:  0 = Ok.
 *              -3 = Out of memory.
 *
 *      (C)2004 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char errbuf[80];

/*
***Allocate memory for vertices.
*/
    if ( mshptr->nv_m > 0 )
      {
      if ( (mshptr->pv_m=(DBVertex *)v3mall(mshptr->nv_m*sizeof(DBVertex),"DBalloc_mesh")) == NULL )
        {
        sprintf(errbuf,"%d",mshptr->nv_m);
        return(erpush("DB1103",errbuf));
        }
      }
    else
      {
      mshptr->pv_m = NULL;
      }
/*
***Allocate memory for halfedges.
*/
    if ( mshptr->nh_m > 0 )
      {
      if ( (mshptr->ph_m=(DBHedge *)v3mall(mshptr->nh_m*sizeof(DBHedge),"DBalloc_mesh")) == NULL )
        {
        sprintf(errbuf,"%d",mshptr->nh_m);
        return(erpush("DB1103",errbuf));
        }
      }
    else
      {
      mshptr->ph_m = NULL;
      }
/*
***Allocate memory for faces.
*/
    if ( mshptr->nf_m > 0 )
      {
      if ( (mshptr->pf_m=(DBFace *)v3mall(mshptr->nf_m*sizeof(DBFace),"DBalloc_mesh")) == NULL )
        {
        sprintf(errbuf,"%d",mshptr->nf_m);
        return(erpush("DB1103",errbuf));
        }
      }
    else
      {
      mshptr->pf_m = NULL;
      }
/*
***Allocate memory for extra floats.
*/
    if ( mshptr->nx_m > 0 )
      {
      if ( (mshptr->px_m=(DBfloat *)v3mall(mshptr->nx_m*sizeof(DBfloat),"DBalloc_mesh")) == NULL )
        {
        sprintf(errbuf,"%d",mshptr->nx_m);
        return(erpush("DB1103",errbuf));
        }
      }
    else
      {
      mshptr->px_m = NULL;
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBfree_mesh(DBMesh *mshptr)

/*      Deallocates C memory allocated for mesh.
 *
 *      In: mshptr => C-pointer to mesh entity
 *
 *      Return: 0 = Ok.
 *
 *      (C)2004 J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
/*
***Free memory for vertices.
*/
    if ( mshptr->nv_m > 0 )
      {
      if ( vrtcache == mshptr->pv_m )
        {
        vrtcache = NULL;
        vrt_la   = DBNULL;
        }
      v3free((char *)mshptr->pv_m,"DBfree_mesh");
      }
/*
***Free memory for half edges.
*/
    if ( mshptr->nh_m > 0 )
      {
      if ( hdgcache == mshptr->ph_m )
        {
        hdgcache = NULL;
        hdg_la   = DBNULL;
        }
      v3free((char *)mshptr->ph_m,"DBfree_mesh");
      }
/*
***Free memory for faces.
*/
    if ( mshptr->nf_m > 0 )
      {
      if ( faccache == mshptr->pf_m )
        {
        faccache = NULL;
        fac_la   = DBNULL;
        }
      v3free((char *)mshptr->pf_m,"DBfree_mesh");
      }
/*
***Free memory for extra floats.
*/
    if ( mshptr->nx_m > 0 )
      {
      if ( xfcache == mshptr->px_m )
        {
        xfcache = NULL;
        xf_la   = DBNULL;
        }
      v3free((char *)mshptr->px_m,"DBfree_mesh");
      }


    return(0);
  }

/********************************************************/
