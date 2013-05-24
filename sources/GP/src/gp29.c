/**********************************************************************
*
*    gp29.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*
*    gpdrms();    Draw Mesh
*    gpdlms();    Erase Mesh
*    gpplms();    Make polyline
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/GP.h"

extern tbool   nivtb1[];
extern double  k2x,k2y;   
extern double  x[],y[],z[];
extern char    a[];
extern short   actpen;
extern int     ncrdxy;
extern VY      actvy;

/*!******************************************************/

        short   gpdrms(
        DBMesh *mshptr,
        DBptr   la,
        short   drmod)

/*      Display a mesh.
 *
 *      In: mshptr => Ptr to mesh.
 *          la     => Mesh DB adress.
 *          drmod  => Display mode.
 *
 *      Return:  0 => Ok.
 *          GP0012 => Display file full.
 *
 *      (C)2004, J. Kjellander, Örebro university
 *
 ******************************************************!*/

{
    int     k;

/*
***Maybe the mesh is on a blanked level ? 
*/
    if ( nivtb1[mshptr->hed_m.level] ||
          mshptr->hed_m.blank) return(0);
/*
***Create polyline.
*/
    k = -1;
    gpplms(mshptr, x, y, z, a, &k);
    ncrdxy = k+1;
/*
***Clip to view limits.
*/
    if ( drmod > GEN )
      {
      ncrdxy = 0;
      if (klpply(-1,&k,x,y,a))
        {
        ncrdxy = k+1;
/*
***Display and store in display file.
*/
        if ( drmod == DRAW )
          {
          if ( mshptr->hed_m.pen != actpen ) gpspen(mshptr->hed_m.pen);
          if ( mshptr->hed_m.hit )
            {
            if ( stoply(k,x,y,a,la,MSHTYP) ) drwobj(TRUE);
            else return(erpush("GP0012",""));
            }
          else
            {
            drwply(k,x,y,a,TRUE);
            }
          }
        }
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short   gpdlms(
        DBMesh *mshptr,
        DBptr   la)

/*      Deletes a mesh from display file and display.
 *
 *      In: mshptr => Ptr to mesh.
 *          la     => Mesh DB adress.
 *
 *      (C)2004, J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
     DBetype typ;
     int     k;

/*
***If the mesh is in the display file it must
***also be on the didplay. Erase from both.
*/
     if (fndobj(la,MSHTYP,&typ))
       {
       gphgen(la,HILIINVISIBLE);
       drwobj(FALSE);
       remobj();
       }
/*
***If not, it may be on the display anyway.
*/
     else
       {
       if ( nivtb1[mshptr->hed_m.level] || mshptr->hed_m.blank) 
         {
         return(0);
         }
       else
         {
         k = -1;
         gpplms(mshptr, x, y, z, a, &k);
         if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);
         }
       }

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short   gpplms(
        DBMesh *mshptr,
        double *x,
        double *y,
        double *z,
        char   *a,
        int    *n)
      
/*      Creates a mesh polyline.
 *
 *      In:  mshptr =>  Ptr to mesh.
 *           n+1    =>  Offset to polyline start position
 *
 *      Out: n      =>  Offset to polyline  end position
 *           x,y,z,a=>  Polyline coordinates and visibility
 *
 *      (C)2004, J. Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    double   dpx,dpy;
    int      i,k,oh_index,sv_index,ev_index;
    DBVector p1,p2,p3,p4,p5,p6,p7,p8;

/*
***Initiering.
*/
    k = *n+1;
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

     gppltr(&p1,x+k, y+k, z+k); a[k++] = 0;
     gppltr(&p2,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p3,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p4,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p1,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p5,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p6,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p7,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p8,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p5,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p8,x+k, y+k, z+k); a[k++] = 0;
     gppltr(&p4,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p7,x+k, y+k, z+k); a[k++] = 0;
     gppltr(&p3,x+k, y+k, z+k); a[k++] = VISIBLE;
     gppltr(&p6,x+k, y+k, z+k); a[k++] = 0;
     gppltr(&p2,x+k, y+k, z+k); a[k]   = VISIBLE;
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
      dpx = DOTSIZ/k2x;
      dpy = DOTSIZ/k2y;

      for ( i=0; i<mshptr->nv_m; ++i)
        {
        gppltr(&(mshptr->pv_m[i].p),x+k, y+k, z+k);
        a[k++] = 0;
        x[k] = x[k-1] + dpx;
        y[k] = y[k-1] + dpy;
        z[k] = z[k-1];
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
          gppltr(&(mshptr->pv_m[sv_index].p),&x[k], &y[k], &z[k]);
          a[k++] = 0;
          gppltr(&(mshptr->pv_m[ev_index].p),&x[k], &y[k], &z[k]);
          a[k++] = VISIBLE;
          }
        }
      k--;
      }

    *n = k;
/*
***Perspective transformation.
*/
   if ( actvy.vydist != 0.0 )
     {
     ncrdxy = k+1;
     gppstr(x,y,z);
     }
/*
***Slut.
*/
   return(0);

  }

/********************************************************/
