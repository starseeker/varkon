/**********************************************************************
*
*    wp29.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*
*    This file includes:
*
*    wpdrms();    Draw Mesh
*    wpdlms();    Erase Mesh
*    wpplms();    Create Mesh polyline
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
*    (C)Johan Kjellander 2004, Örebro university
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

extern short    actpen;

static short drawms(WPGWIN *gwinpt, DBMesh *mshptr, DBptr la, bool draw);

/*!******************************************************/

        short   wpdrms(
        DBMesh *mshptr,
        DBptr   la,
        v2int   win_id)

/*      Display a Mesh.
 *
 *      In: mshptr => Ptr to mesh.
 *          la     => GM DB-adress.
 *          win_id => Window(s) to display in
 *
 *      Return:  0 => Ok.
 *
 *      (C)2004-07-21 Johan Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***We don't display blanked meshes..
*/
   if ( mshptr->hed_m.blank) return(0);
/*
***Find graphical window(s) in the windowtable.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Is the mesh level on in this window`?
*/
         if ( wpnivt(gwinpt,mshptr->hed_m.level) )
           {
/*
***Yes, activate the right color.
*/
           if ( mshptr->hed_m.pen != actpen ) wpspen(mshptr->hed_m.pen);
/*
***Display.
*/
           drawms(gwinpt,mshptr,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   wpdlms(
        DBMesh *mshptr,
        DBptr   la,
        v2int   win_id)

/*      Undisplays a mesh.
 *
 *      In: mshptr => Ptr to mesh.
 *          la     => GM DB-adress.
 *          win_id => Window(s) to undisplay in
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)2004-07-21 Johan Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Find graphical window(s) in the windowtable.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***If the mesh is in the window displayfile, use quick erase.
*/
         if ( wpfobj(gwinpt,la,MSHTYP,&typ) )
           {
           wpdobj(gwinpt,FALSE);
           wprobj(gwinpt);
           }
/*
***If not, generate polyline and draw with background color.
*/
         else
           {
           if ( !wpnivt(gwinpt,mshptr->hed_m.level)  ||
                               mshptr->hed_m.blank) return(0);
           drawms(gwinpt,mshptr,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawms(
        WPGWIN *gwinpt,
        DBMesh *mshptr,
        DBptr   la,
        bool    draw)

/*      Draw/erase a mesh in graphical window..
 *      During draw the polyline is also inserted in the
 *      window display file.
 *
 *      In: gwinpt => Ptr to window.
 *          mshptr => Ptr to mesh.
 *          la     => GM DB-adress.
 *          draw   => TRUE = Draw, FALSE = Erase
 *
 *      Return:     0 = OK.
 *
 *      (C)2004-07-21 Johan Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   double x[300000],y[300000],z[300000];
   char   a[300000];
   int    k;

/*
***Create polyline.
*/
   k = -1;
   wpplms(gwinpt,mshptr,&k,x,y,z,a);
/*
***Clip and draw if visible.
*/
   if ( wpcply(gwinpt,-1,&k,x,y,a) )
     {
     if ( draw  &&  mshptr->hed_m.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,MSHTYP) ) wpdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else wpdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short   wpplms(
        WPGWIN *gwinpt,
        DBMesh *mshptr,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Creates a polyline representation of a mesh.
 *
 *      In: gwinpt => Ptr to graphical window.
 *          mshptr => Ptr to mesh.
 *          n+1    => Offset tto polyline start position
 *
 *      Out:*n     => Offset to polyline end position
 *           x,y,a => Polyline data
 *
 *      FV: 0
 *
 *      (C)2004-07-21 Johan Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    WPGRPT   pt;
    double   dpx,dpy;
    int      i,k,oh_index,sv_index,ev_index;
    DBVector p1,p2,p3,p4,p5,p6,p7,p8;
/*
***Init.
*/
   k = *n + 1;
/*
***With font = 1, display the bounding box.
*/
   if ( mshptr->font_m == 1 )
     {
     p1.x_gm = mshptr->bbox_m.xmin;
     p1.y_gm = mshptr->bbox_m.ymin;
     p1.z_gm = mshptr->bbox_m.zmin;
 
     p2.x_gm = mshptr->bbox_m.xmax;
     p2.y_gm = mshptr->bbox_m.ymin;
     p2.z_gm = mshptr->bbox_m.zmin;

     p3.x_gm = mshptr->bbox_m.xmax;
     p3.y_gm = mshptr->bbox_m.ymax;
     p3.z_gm = mshptr->bbox_m.zmin;

     p4.x_gm = mshptr->bbox_m.xmin;
     p4.y_gm = mshptr->bbox_m.ymax;
     p4.z_gm = mshptr->bbox_m.zmin;

     p5.x_gm = mshptr->bbox_m.xmin;
     p5.y_gm = mshptr->bbox_m.ymin;
     p5.z_gm = mshptr->bbox_m.zmax;

     p6.x_gm = mshptr->bbox_m.xmax;
     p6.y_gm = mshptr->bbox_m.ymin;
     p6.z_gm = mshptr->bbox_m.zmax;

     p7.x_gm = mshptr->bbox_m.xmax;
     p7.y_gm = mshptr->bbox_m.ymax;
     p7.z_gm = mshptr->bbox_m.zmax;

     p8.x_gm = mshptr->bbox_m.xmin;
     p8.y_gm = mshptr->bbox_m.ymax;
     p8.z_gm = mshptr->bbox_m.zmax;

     wptrpo(gwinpt,&p1,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
     wptrpo(gwinpt,&p2,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p3,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p4,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p1,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p5,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p6,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p7,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p8,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p5,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p8,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
     wptrpo(gwinpt,&p4,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p7,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
     wptrpo(gwinpt,&p3,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
     wptrpo(gwinpt,&p6,&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
     wptrpo(gwinpt,&p2,&pt); x[k] = pt.x; y[k] = pt.y; a[k]   = VISIBLE;
     }
/*
***With font = 0 or 2 or if the mesh has no edges,
***display as pointcloud.
*/
    else if ( (mshptr->font_m == 0  &&  mshptr->nh_m == 0)  ||
               mshptr->font_m == 2 )
      {
/*
***Transform all vertex positions to active view
***and save in x,y,z.
*/
      dpx = 1.0/gwinpt->vy.k2x;
      dpy = 1.0/gwinpt->vy.k2y;

      for ( i=0; i<mshptr->nv_m; ++i)
        {
        wptrpo(gwinpt,&(mshptr->pv_m[i].p),&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
        x[k]   = x[k-1] + dpx;
        y[k]   = y[k-1];
        z[k]   = z[k-1];
        a[k++] = VISIBLE;
        x[k]   = x[k-1];
        y[k]   = y[k-1] + dpy;
        z[k]   = z[k-1];
        a[k++] = VISIBLE;
        x[k]   = x[k-1] - dpx;
        y[k]   = y[k-1];
        z[k]   = z[k-1];
        a[k++] = VISIBLE;
        }
      k--;
      }
/*
***If edges exist, display mesh edges instead of pointcloud.
*/
    else if ( (mshptr->font_m == 0  ||  mshptr->font_m == 3)  &&
               mshptr->nh_m > 0 )
      {
/*
***Loop through the array of halfedges. We only need to display
***one of the two halfedges in each pair. One way to do that is
***to compare the array index of each halfedge with the index of
***it's opposite halfedge. If the index of the opposite halfedge
***is higher than the index of the current halfedge we display
***the halfedge, otherwise not.
*/
      for ( i=0; i<mshptr->nh_m; ++i)
        {
        oh_index = mshptr->ph_m[i].i_ohedge;
        if ( oh_index > i )
          {
          sv_index = mshptr->ph_m[oh_index].i_evertex;
          ev_index = mshptr->ph_m[i].i_evertex;
          wptrpo(gwinpt,&(mshptr->pv_m[sv_index].p),&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = 0;
          wptrpo(gwinpt,&(mshptr->pv_m[ev_index].p),&pt); x[k] = pt.x; y[k] = pt.y; a[k++] = VISIBLE;
          }
        }
      k--;
      }
/*
***End.
*/
  *n = k;

   return(0);
  }

/********************************************************/
