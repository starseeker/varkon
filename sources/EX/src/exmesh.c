/*!*******************************************************
*
*     exmesh.c
*     ========
*
*     EXemsh();      Create mesh
*     EXmsar();      Create MESH_ARR
*     EXgmsh();      DB-get for mesh header
*     EXgvrt();      DB-get for vertex
*     EXghdg();      DB-get for halfedge
*     EXgface();     DB-get for face
*     EXgxflt();     DB-get for extra float
*     EXsort_mesh(); SORT_MESH() in MBS
*
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
*    (C)2004, Johan Kjellander, Örebro university
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"

extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern DBptr   lsysla;
extern short   modtyp;

#ifdef WIN32
extern short WPdrpo();
#endif


/*!******************************************************/

       DBstatus EXemsh(
       DBId    *id,
       DBMesh  *mshptr,
       V2NAPA  *pnp)

/*     Creates mesh, save in DB and display.
 *
 *     In: id     => Ptr to ID.
 *         mshptr => Ptr to mesh.
 *         pnp    => Ptr to attributes.
 *
 *     Return:      0 = Ok.
 *             EX2112 = Can't save mesh in DB
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    int     i;
    DBfloat xmin,ymin,zmin,xmax,ymax,zmax;
    DBptr   la;

/*
***Attributes.
*/
    mshptr->hed_m.blank = (char)pnp->blank;
    mshptr->hed_m.pen   = (short)pnp->pen;
    mshptr->hed_m.level = (short)pnp->level;
    mshptr->font_m      = pnp->mfont;
    mshptr->wdt_m       = pnp->width;
    mshptr->pcsy_m      = lsysla;
/*
***Calculate bounding box.
*/
    xmin = ymin = zmin =  1E20;
    xmax = ymax = zmax = -1E20;

    for ( i=0; i<mshptr->nv_m; ++i )
      {
      if ( mshptr->pv_m[i].p.x_gm < xmin ) xmin = mshptr->pv_m[i].p.x_gm;
      if ( mshptr->pv_m[i].p.x_gm > xmax ) xmax = mshptr->pv_m[i].p.x_gm;
      if ( mshptr->pv_m[i].p.y_gm < ymin ) ymin = mshptr->pv_m[i].p.y_gm;
      if ( mshptr->pv_m[i].p.y_gm > ymax ) ymax = mshptr->pv_m[i].p.y_gm;
      if ( mshptr->pv_m[i].p.z_gm < zmin ) zmin = mshptr->pv_m[i].p.z_gm;
      if ( mshptr->pv_m[i].p.z_gm > zmax ) zmax = mshptr->pv_m[i].p.z_gm;       
      }

    mshptr->bbox_m.xmin = xmin;
    mshptr->bbox_m.xmax = xmax;
    mshptr->bbox_m.ymin = ymin;
    mshptr->bbox_m.ymax = ymax;
    mshptr->bbox_m.zmin = zmin;
    mshptr->bbox_m.zmax = zmax;
/*
***Save in DB.
*/
    if ( pnp->save )
      {
      mshptr->hed_m.hit = (char)pnp->hit;
      if ( DBinsert_mesh(mshptr,id,&la) < 0 ) return(erpush("EX2112",""));
      }
    else
      {
      mshptr->hed_m.hit = 0;
      }
/*
***Display.
*/
    WPdrms(mshptr,la,GWIN_ALL);
/*
***Succesful creation of mesh. Empty error message stack
*/
    varkon_erinit();

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus EXmsar(
       DBId    *id,
       DBMesh  *mshptr,
       V2NAPA  *pnp)

/*      Executes MESH_ARR.
 *
 *     In: id     => Ptr to ID.
 *         mshptr => Ptr to mesh.
 *         pnp    => Ptr to attributes.
 *
 *     Return:      0 = Ok.
 *             EX2112 = Can't save mesh in DB
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   int i;

/*
***Transform vertices to basic coordinates.
*/
    if ( lsyspk != NULL )
      {
      for ( i=0; i<mshptr->nv_m; ++i )
        {
        GEtfpos_to_local(&(mshptr->pv_m[i].p),&lklsyi,&(mshptr->pv_m[i].p));
        }
      }
/*
***Store in DB and display..
*/
    return(EXemsh(id,mshptr,pnp));
  }

/********************************************************/
/*!******************************************************/

       DBstatus  EXgmsh(
       DBId     *id,
       DBint    *nv,
       DBint    *nh,
       DBint    *nf,
       DBVector *pmin,
       DBVector *pmax,
       DBint    *font)

/*      Executes GETMESHH.
 *
 *     In: id     => Ptr to ID.
 *
 *     Return:      0 = Ok.
 *             EX1402 = Entity does not exist
 *             EX1412 = Entity is not a mesh
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    DBMesh  mesh;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      DBread_mesh(&mesh, la, MESH_HEADER);
      }
    else return(erpush("EX1412","GETMESHH()"));
/*
***Return n_vertices,n_hedges,n_faces,bounding box and font.
*/
    *nv         = mesh.nv_m;
    *nh         = mesh.nh_m;
    *nf         = mesh.nf_m;
     pmin->x_gm = mesh.bbox_m.xmin;
     pmin->y_gm = mesh.bbox_m.ymin;
     pmin->z_gm = mesh.bbox_m.zmin;
     pmax->x_gm = mesh.bbox_m.xmax;
     pmax->y_gm = mesh.bbox_m.ymax;
     pmax->z_gm = mesh.bbox_m.zmax;
    *font       = mesh.font_m;

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus  EXgvrt(
       DBId     *id,
       DBint     vrtnum,
       DBVector *p,
       DBint    *eh)

/*      Executes GETVERTEX.
 *
 *     In: id     => Ptr to ID.
 *         vrtnum => Vertex number
 *         p      => Ptr to output coordinates
 *         eh     => Ptr to index of emanating halfedge
 *
 *     Return:       0 = Ok.
 *              EX1402 = Entity does not exist
 *              EX1412 = Entity is not a mesh
 *              EX2122 = Invalid vertexnumber
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char      errbuf[V3STRLEN];
    DBstatus  status;
    DBptr     la;
    DBetype   typ;
    DBMesh    mesh;
    DBVertex *vp;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      if ( (status=DBread_mesh(&mesh, la, MESH_HEADER+MESH_VERTEX)) < 0 ) return(status);
      if ( vrtnum > 0 && vrtnum <= mesh.nv_m )
        {
        vp      = &(mesh.pv_m[vrtnum-1]);
        p->x_gm = vp->p.x_gm;
        p->y_gm = vp->p.y_gm;
        p->z_gm = vp->p.z_gm;
       *eh      = vp->i_ehedge;
        }
      else
        {
        sprintf(errbuf,"%d",vrtnum);
        return(erpush("EX2122",errbuf));
        }
      }
    else return(erpush("EX1412","GETVERTEX()"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus EXghdg(
       DBId    *id,
       DBint    hdgnum,
       DBint   *ev,
       DBint   *oh,
       DBint   *nh,
       DBint   *pf)

/*      Executes GETHEDGE.
 *
 *     In: id     => Ptr to ID.
 *         hdgnum => Halfedge number
 *         ev     => Ptr to index of end vertex
 *         oh     => Ptr to index of opposite hedge
 *         nh     => Ptr to index of next hedge
 *         pf     => Ptr to index of parent face
 *
 *     Return:      0 = Ok.
 *             EX1402 = Entity does not exist
 *             EX1412 = Entity is not a mesh
 *             EX2132 = Invalid hedgenumber
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char     errbuf[V3STRLEN];
    DBstatus status;
    DBptr    la;
    DBetype  typ;
    DBMesh   mesh;
    DBHedge *hp;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      if ( (status=DBread_mesh(&mesh, la, MESH_HEADER+MESH_HEDGE)) < 0 ) return(status);
      if ( hdgnum > 0 && hdgnum <= mesh.nh_m )
        {
        hp = &(mesh.ph_m[hdgnum-1]);
       *ev = hp->i_evertex;
       *oh = hp->i_ohedge;
       *nh = hp->i_nhedge;
       *pf = hp->i_pface;
        }
      else
        {
        sprintf(errbuf,"%d",hdgnum);
        return(erpush("EX2123",errbuf));
        }
      }
    else return(erpush("EX1412","GETHEDGE()"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus EXgface(
       DBId    *id,
       DBint    facnum,
       DBint   *h)

/*      Executes GETFACE.
 *
 *     In: id     => Ptr to ID.
 *         facnum => Face number
 *         h      => Ptr to index of hedge
 *
 *     Return:      0 = Ok.
 *             EX1402 = Entity does not exist
 *             EX1412 = Entity is not a mesh
 *             EX2142 = Invalid facenumber
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char     errbuf[V3STRLEN];
    DBstatus status;
    DBptr    la;
    DBetype  typ;
    DBMesh   mesh;
    DBFace  *fp;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      if ( (status=DBread_mesh(&mesh, la, MESH_HEADER+MESH_FACE)) < 0 ) return(status);
      if ( facnum > 0 && facnum <= mesh.nf_m )
        {
        fp = &(mesh.pf_m[facnum-1]);
       *h  = fp->i_hedge;
        }
      else
        {
        sprintf(errbuf,"%d",facnum);
        return(erpush("EX2142",errbuf));
        }
      }
    else return(erpush("EX1412","GETFACE()"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus EXgxflt(
       DBId    *id,
       DBint    xfnum,
       DBfloat *f)

/*      Executes GETXFLOAT.
 *
 *     In: id     => Ptr to ID.
 *         facnum => Float index number
 *         h      => Ptr to float value
 *
 *     Return:      0 = Ok.
 *             EX1402 = Entity does not exist
 *             EX1412 = Entity is not a mesh
 *             EX2152 = Invalid float index number
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char     errbuf[V3STRLEN];
    DBstatus status;
    DBptr    la;
    DBetype  typ;
    DBMesh   mesh;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      if ( (status=DBread_mesh(&mesh, la, MESH_HEADER+MESH_XFLOAT)) < 0 ) return(status);
      if ( xfnum > 0 && xfnum <= mesh.nx_m )
        {
       *f = (mesh.px_m[xfnum-1]);
        }
      else
        {
        sprintf(errbuf,"%d",xfnum);
        return(erpush("EX2152",errbuf));
        }
      }
    else return(erpush("EX1412","GETXFLOAT()"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

       DBstatus EXsort_mesh(
       DBId    *id,
       char     mode,
       DBint   *facelist,
       DBfloat *min,
       DBfloat *max)

/*      Executes SORT_MESH.
 *
 *     In: id       => Ptr to ID.
 *         mode     => 'X', 'Y' or 'Z'
 *
 *     Out:
 *         facelist => Ptr to sorted list of face indexes
 *         min      => List of face min X-, Y- or Z-values
 *         max      => List of face max X-, Y- or Z-values
 *
 *     Return:      0 = Ok.
 *             EX1402 = Entity does not exist
 *             EX1412 = Entity is not a mesh
 *
 *     (C)2004, Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBstatus status;
    DBptr    la;
    DBetype  typ;
    DBMesh   mesh;

/*
***GetDB addres and type of entity.
*/
    if ( DBget_pointer( 'I', id, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Check entity type and if it is a mesh, read mesh data.
*/
    if ( typ == MSHTYP )
      {
      if ( (status=DBread_mesh(&mesh, la, MESH_ALL)) < 0 ) return(status);
      }
    else return(erpush("EX1412","SORT_MESH()"));
/*
***Sort.
*/
    if ( (status=GEsort_mesh(&mesh,mode,facelist,min,max)) < 0 ) return(status);

    return(0);
  }

/********************************************************/
