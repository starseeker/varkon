/**********************************************************************
*
*    wpmesh.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPdrms();    Draw Mesh
*    WPdlms();    Erase Mesh
*    WPplms();    Create Mesh polyline
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

extern short    actpen;

static short drawms(WPGWIN *gwinpt, DBMesh *mshptr, DBptr la, bool draw);

/*!******************************************************/

        short   WPdrms(
        DBMesh *mshptr,
        DBptr   la,
        DBint   win_id)

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
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Is the mesh level on in this window`?
*/
         if ( WPnivt(gwinpt,mshptr->hed_m.level) )
           {
/*
***Yes, activate the right color.
*/
           if ( mshptr->hed_m.pen != actpen ) WPspen(mshptr->hed_m.pen);
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

        short   WPdlms(
        DBMesh *mshptr,
        DBptr   la,
        DBint   win_id)

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
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***If the mesh is in the window displayfile, use quick erase.
*/
         if ( WPfobj(gwinpt,la,MSHTYP,&typ) )
           {
           WPdobj(gwinpt,FALSE);
           WProbj(gwinpt);
           }
/*
***If not, generate polyline and draw with background color.
*/
         else
           {
           if ( !WPnivt(gwinpt,mshptr->hed_m.level)  ||
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
   double x[300000],y[300000],z[300000],size;
   char   a[300000];
   int    k;

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
***Point size = 0.75/scale.
*/
   size = 0.75/((gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
                gwinpt->geo.psiz_x /
                (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin));
/*
***Create polyline.
*/
   k = -1;
   WPplms(mshptr,size,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Clip and draw if visible.
*/
   if ( WPcply(gwinpt,-1,&k,x,y,a) )
     {
     if ( draw  &&  mshptr->hed_m.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,MSHTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short   WPplms(
        DBMesh *mshptr,
        double  size,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Creates a polyline representation of a mesh.
 *
 *      In: mshptr => Ptr to mesh.
 *          size   => Vertex size
 *          n+1    => Offset tto polyline start position
 *
 *      Out:*n     => Offset to polyline end position
 *           x,y,a => Polyline data
 *
 *      FV: 0
 *
 *      (C)2004-07-21 Johan Kjellander, Örebro university
 *
 *      2006-12-28, Removed gwinpt, J.Kjellander
 *
 ******************************************************!*/

  {
    double   d;
    int      i,k,oh_index,sv_index,ev_index;

/*
***Init.
*/
   k = *n + 1;
/*
***With font = 1, display the bounding box.
*/
   if ( mshptr->font_m == 1 )
     {
     x[k] = mshptr->bbox_m.xmin;      /* 1 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] =0;
 
     x[k] = mshptr->bbox_m.xmax;      /* 2 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmax;      /* 3 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 4 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 1 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 5 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmax;      /* 6 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmax;      /* 7 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 8 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 5 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmin;      /* 8 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = 0;

     x[k] = mshptr->bbox_m.xmin;      /* 4 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmax;      /* 3 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmin;
     a[k++] = 0;

     x[k] = mshptr->bbox_m.xmax;      /* 7 */
     y[k] = mshptr->bbox_m.ymax;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = VISIBLE;

     x[k] = mshptr->bbox_m.xmax;      /* 6 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmax;
     a[k++] = 0;

     x[k] = mshptr->bbox_m.xmax;      /* 2 */
     y[k] = mshptr->bbox_m.ymin;
     z[k] = mshptr->bbox_m.zmin;
     a[k] = VISIBLE;
     }
/*
***With font = 2 or 0 and the mesh has no edges,
***display as pointcloud.
*/
    else if ( (mshptr->font_m == 0  &&  mshptr->nh_m == 0)  ||
               mshptr->font_m == 2 )
      {
      d =  size/2.0;

      for ( i=0; i<mshptr->nv_m; ++i)
        {
        x[k] = mshptr->pv_m[i].p.x_gm - d;
        y[k] = mshptr->pv_m[i].p.y_gm - d;
        z[k] = mshptr->pv_m[i].p.z_gm;
        a[k++] = 0;

        x[k] = mshptr->pv_m[i].p.x_gm + d;
        y[k] = mshptr->pv_m[i].p.y_gm + d;
        z[k] = mshptr->pv_m[i].p.z_gm;
        a[k] = VISIBLE;

        x[k] = mshptr->pv_m[i].p.x_gm + d;
        y[k] = mshptr->pv_m[i].p.y_gm - d;
        z[k] = mshptr->pv_m[i].p.z_gm;
        a[k] = 0;

        x[k] = mshptr->pv_m[i].p.x_gm - d;
        y[k] = mshptr->pv_m[i].p.y_gm + d;
        z[k] = mshptr->pv_m[i].p.z_gm;
        a[k] = VISIBLE;

        x[k] = mshptr->pv_m[i].p.x_gm;
        y[k] = mshptr->pv_m[i].p.y_gm;
        z[k] = mshptr->pv_m[i].p.z_gm + d;
        a[k] = 0;

        x[k] = mshptr->pv_m[i].p.x_gm;
        y[k] = mshptr->pv_m[i].p.y_gm;
        z[k] = mshptr->pv_m[i].p.z_gm - d;
        a[k] = VISIBLE;
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
          x[k] = mshptr->pv_m[sv_index].p.x_gm;
          y[k] = mshptr->pv_m[sv_index].p.y_gm;
          z[k] = mshptr->pv_m[sv_index].p.z_gm;
          a[k++] = 0;
          x[k] = mshptr->pv_m[ev_index].p.x_gm;
          y[k] = mshptr->pv_m[ev_index].p.y_gm;
          z[k] = mshptr->pv_m[ev_index].p.z_gm;
          a[k++] = VISIBLE;
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
