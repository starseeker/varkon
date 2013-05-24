/**********************************************************************
*
*    wpGLlist.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPmmod();           Measures size of model
*    WPmodl_all();       Make OpenGL displaylist for all entities
*    WPmodl_highlight(); Make OpenGL displaylist for highlighted entities
*    WPdodl_highlight(); Delete all highlight lists.
*    WPsodl_all();       Show (execute) all displaylists in a window
*    WPeodls();          Executes OpenGL DisplayList 1 for Selection
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <GL/glu.h>
#include <math.h>

extern DBptr  lsysla;                     /* Currently active csys */
extern int    actpen_gl;                  /* Active pen for OpenGL */

static DBfloat      actwdt;               /* Current linewidth */
static GLUnurbsObj *gl_nobj;              /* Nurbs-object */
static DBfloat      nurbs_display_factor; /* For NURBS tesselation */

/*
***Open GL list ID 1 is always the list with all the visible entities.
***Additional lists with ID 2,3,4,5... are used for highlighted entities.
***n_hllits is used to keep track of how many highlight lists currently
***exist.
*/
static int          n_hllists = 0;

/*
***Wireframes and surfaces use different light models.
*/
#define WIREFRAME_MODEL 1
#define SURFACE_MODEL   2

/*
***Prototypes for internal functions.
*/
static void  pr_poi(WPRWIN *rwinpt, DBPoint *poi);
static void  pr_lin(WPRWIN *rwinpt, DBLine *lin);
static void  pr_arc(WPRWIN *rwinpt, DBArc *arc, DBSeg *arcseg);
static void  pr_cur(WPRWIN *rwinpt, DBCurve *cur, DBSeg *graseg);
static void  pr_txt(WPRWIN *rwinpt, DBText *txt, char *str);
static void  pr_xht(WPRWIN *rwinpt, DBHatch *xht, DBfloat crdvek[], DBCsys *csy);
static void  pr_ldm(WPRWIN *rwinpt, DBLdim *ldm, DBCsys *csyptr);
static void  pr_cdm(WPRWIN *rwinpt, DBCdim *cdm, DBCsys *csyptr);
static void  pr_rdm(WPRWIN *rwinpt, DBRdim *rdm, DBCsys *csyptr);
static void  pr_adm(WPRWIN *rwinpt, DBAdim *adm, DBCsys *csyptr);
static void  pr_bpl(WPRWIN *rwinpt, DBBplane *bpl);
static void  pr_msh(WPRWIN *rwinpt, DBMesh *mesh);
static void  pr_csy(WPRWIN *rwinpt, DBCsys *csy, DBptr la);
static void  pr_sur1(WPRWIN *rwinpt, DBSurf *sur);
static void  pr_sur2(WPRWIN *rwinpt, DBSurf *sur);
static void  set_linewidth(WPRWIN *rwinpt, DBfloat width);
static int   width_to_pixels(WPRWIN *rwinpt, DBfloat width);
static void  set_lightmodel(WPRWIN *rwinpt, int model);

/*!******************************************************/

    short   WPmmod(
    WPRWIN *rwinpt)

/*    Calculates the bounding box of the model and
 *    creates a viewbox with same proportions in X and
 *    Y as the window and big enough in Z to permit rotation
 *    without clipping.
 *
 *    In: rwinpt = C-ptr to WPRWIN
 *
 *    (C)microform ab 1998-01-09 J. Kjellander
 *
 *    1998-10-27 Bugfix deallokering, J.Kjellander
 *    2004-07-10 Mesh J.Kjellander, Örebro university
 *    2007-06-16 1.19, J.Kjellander
 *    2007-09-24 3D dims, J.Kjellander
 *
 ******************************************************!*/

 {
   int      i,k;
   DBetype  type;
   DBptr    la;
   DBHeader hdr;
   DBPoint  poi;
   DBLine   lin;
   DBArc    arc;
   DBText   txt;
   DBHatch  xht;
   DBLdim   ldm;
   DBCdim   cdm;
   DBRdim   rdm;
   DBAdim   adm;
   DBCurve  cur;
   DBBplane bpl;
   DBCsys   csy;
   DBSurf   sur;
   DBTmat   invcrd;
   DBSeg   *graseg,seg[4];
   DBMesh   mesh;
   char     a[PLYMXV];
   double   x[PLYMXV],y[PLYMXV],z[PLYMXV],crdvek[4*GMXMXL],xmin,xmax,
            ymin,ymax,zmin,zmax,mdx,mdy,mdz,centre_x,centre_y,centre_z,
            biggest,gadx,gady,mprop,gprop;

/*
***This might take time.
*/
#ifdef UNIX
   WPscur(rwinpt->id.w_id,TRUE,xwcur);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_WAIT));
#endif
/*
***Initialization.
*/
   xmin = ymin = zmin =  1E20;
   xmax = ymax = zmax = -1E20;
/*
***Traverse DB.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     DBread_header(&hdr,la);
     if ( !hdr.blank  &&  WPnivt(rwinpt->nivtab,hdr.level) )
       {
       switch ( type )
         {
/*
***A point.
*/
         case POITYP:
         DBread_point(&poi,la);
         if ( poi.crd_p.x_gm < xmin ) xmin = poi.crd_p.x_gm;
         if ( poi.crd_p.x_gm > xmax ) xmax = poi.crd_p.x_gm;
         if ( poi.crd_p.y_gm < ymin ) ymin = poi.crd_p.y_gm;
         if ( poi.crd_p.y_gm > ymax ) ymax = poi.crd_p.y_gm;
         if ( poi.crd_p.z_gm < zmin ) zmin = poi.crd_p.z_gm;
         if ( poi.crd_p.z_gm > zmax ) zmax = poi.crd_p.z_gm;
         break;
/*
***A line.
*/
         case LINTYP:
         DBread_line(&lin,la);
         if ( lin.crd1_l.x_gm < xmin ) xmin = lin.crd1_l.x_gm;
         if ( lin.crd1_l.x_gm > xmax ) xmax = lin.crd1_l.x_gm;
         if ( lin.crd1_l.y_gm < ymin ) ymin = lin.crd1_l.y_gm;
         if ( lin.crd1_l.y_gm > ymax ) ymax = lin.crd1_l.y_gm;
         if ( lin.crd1_l.z_gm < zmin ) zmin = lin.crd1_l.z_gm;
         if ( lin.crd1_l.z_gm > zmax ) zmax = lin.crd1_l.z_gm;

         if ( lin.crd2_l.x_gm < xmin ) xmin = lin.crd2_l.x_gm;
         if ( lin.crd2_l.x_gm > xmax ) xmax = lin.crd2_l.x_gm;
         if ( lin.crd2_l.y_gm < ymin ) ymin = lin.crd2_l.y_gm;
         if ( lin.crd2_l.y_gm > ymax ) ymax = lin.crd2_l.y_gm;
         if ( lin.crd2_l.z_gm < zmin ) zmin = lin.crd2_l.z_gm;
         if ( lin.crd2_l.z_gm > zmax ) zmax = lin.crd2_l.z_gm;
         break;
/*
***An arc. TODO, fix scale.
*/
         case ARCTYP:
         DBread_arc(&arc,seg,la);
         arc.fnt_a = 0;

         k = -1;
         WPplar(&arc,seg,(double)1.0,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A curve. TODO fix scale.
*/
         case CURTYP:
         DBread_curve(&cur,&graseg,NULL,la);
         cur.fnt_cu = 0;

         k = -1;
         WPplcu(&cur,graseg,1.0,&k,x,y,z,a);
         DBfree_segments(graseg);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A Text.
*/
         case TXTTYP:
         DBread_text(&txt,NULL,la);
         if ( txt.crd_tx.x_gm < xmin ) xmin = txt.crd_tx.x_gm;
         if ( txt.crd_tx.x_gm > xmax ) xmax = txt.crd_tx.x_gm + txt.b_tx;
         if ( txt.crd_tx.y_gm < ymin ) ymin = txt.crd_tx.y_gm;
         if ( txt.crd_tx.y_gm > ymax ) ymax = txt.crd_tx.y_gm + txt.h_tx;
         if ( txt.crd_tx.z_gm < zmin ) zmin = txt.crd_tx.z_gm;
         if ( txt.crd_tx.z_gm > zmax ) zmax = txt.crd_tx.z_gm;
         break;
/*
***A Hatch.
*/
         case XHTTYP:
         DBread_xhatch(&xht,crdvek,&csy,la);
         k = -1;
         WPplxh(&xht,crdvek,&csy,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A Linear dimension.
*/
         case LDMTYP:
         DBread_ldim(&ldm,&csy,la);
         k = -1;
         WPplld(&ldm,&csy,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A Circular dimension.
*/
         case CDMTYP:
         DBread_cdim(&cdm,&csy,la);
         k = -1;
         WPplcd(&cdm,&csy,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A Radius dimension.
*/
         case RDMTYP:
         DBread_rdim(&rdm,&csy,la);
         k = -1;
         WPplrd(&rdm,&csy,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***An angular dimension.
*/
         case ADMTYP:
         DBread_adim(&adm,&csy,la);
         k = -1;
         WPplad(&adm,&csy,1.0,&k,x,y,z,a);

         for ( i=0; i<=k; ++i )
           {
           if ( x[i] < xmin ) xmin = x[i];
           if ( x[i] > xmax ) xmax = x[i];
           if ( y[i] < ymin ) ymin = y[i];
           if ( y[i] > ymax ) ymax = y[i];
           if ( z[i] < zmin ) zmin = z[i];
           if ( z[i] > zmax ) zmax = z[i];
           }
         break;
/*
***A B-plane.
*/
         case BPLTYP:
         DBread_bplane(&bpl,la);
         if ( bpl.crd1_bp.x_gm < xmin ) xmin = bpl.crd1_bp.x_gm;
         if ( bpl.crd1_bp.x_gm > xmax ) xmax = bpl.crd1_bp.x_gm;
         if ( bpl.crd1_bp.y_gm < ymin ) ymin = bpl.crd1_bp.y_gm;
         if ( bpl.crd1_bp.y_gm > ymax ) ymax = bpl.crd1_bp.y_gm;
         if ( bpl.crd1_bp.z_gm < zmin ) zmin = bpl.crd1_bp.z_gm;
         if ( bpl.crd1_bp.z_gm > zmax ) zmax = bpl.crd1_bp.z_gm;

         if ( bpl.crd2_bp.x_gm < xmin ) xmin = bpl.crd2_bp.x_gm;
         if ( bpl.crd2_bp.x_gm > xmax ) xmax = bpl.crd2_bp.x_gm;
         if ( bpl.crd2_bp.y_gm < ymin ) ymin = bpl.crd2_bp.y_gm;
         if ( bpl.crd2_bp.y_gm > ymax ) ymax = bpl.crd2_bp.y_gm;
         if ( bpl.crd2_bp.z_gm < zmin ) zmin = bpl.crd2_bp.z_gm;
         if ( bpl.crd2_bp.z_gm > zmax ) zmax = bpl.crd2_bp.z_gm;

         if ( bpl.crd3_bp.x_gm < xmin ) xmin = bpl.crd3_bp.x_gm;
         if ( bpl.crd3_bp.x_gm > xmax ) xmax = bpl.crd3_bp.x_gm;
         if ( bpl.crd3_bp.y_gm < ymin ) ymin = bpl.crd3_bp.y_gm;
         if ( bpl.crd3_bp.y_gm > ymax ) ymax = bpl.crd3_bp.y_gm;
         if ( bpl.crd3_bp.z_gm < zmin ) zmin = bpl.crd3_bp.z_gm;
         if ( bpl.crd3_bp.z_gm > zmax ) zmax = bpl.crd3_bp.z_gm;

         if ( bpl.crd4_bp.x_gm < xmin ) xmin = bpl.crd4_bp.x_gm;
         if ( bpl.crd4_bp.x_gm > xmax ) xmax = bpl.crd4_bp.x_gm;
         if ( bpl.crd4_bp.y_gm < ymin ) ymin = bpl.crd4_bp.y_gm;
         if ( bpl.crd4_bp.y_gm > ymax ) ymax = bpl.crd4_bp.y_gm;
         if ( bpl.crd4_bp.z_gm < zmin ) zmin = bpl.crd4_bp.z_gm;
         if ( bpl.crd4_bp.z_gm > zmax ) zmax = bpl.crd4_bp.z_gm;
         break;
/*
***A coordinate system.
*/
         case CSYTYP:
         DBread_csys(&csy,NULL,la);
         GEtform_inv(&csy.mat_pl,&invcrd);
         if ( invcrd.g14 < xmin ) xmin = invcrd.g14;
         if ( invcrd.g14 > xmax ) xmax = invcrd.g14;
         if ( invcrd.g24 < ymin ) ymin = invcrd.g24;
         if ( invcrd.g24 > ymax ) ymax = invcrd.g24;
         if ( invcrd.g34 < zmin ) zmin = invcrd.g34;
         if ( invcrd.g34 > zmax ) zmax = invcrd.g34;
         break;
/*
***A surface.
*/
         case SURTYP:
         DBread_surface(&sur,la);
         if ( sur.box_su.xmin < xmin ) xmin = sur.box_su.xmin;
         if ( sur.box_su.xmax > xmax ) xmax = sur.box_su.xmax;
         if ( sur.box_su.ymin < ymin ) ymin = sur.box_su.ymin;
         if ( sur.box_su.ymax > ymax ) ymax = sur.box_su.ymax;
         if ( sur.box_su.zmin < zmin ) zmin = sur.box_su.zmin;
         if ( sur.box_su.zmax > zmax ) zmax = sur.box_su.zmax;
         break;
/*
***A Mesh.
*/
         case MSHTYP:
         DBread_mesh(&mesh,la,MESH_HEADER);
         if ( mesh.bbox_m.xmin < xmin ) xmin = mesh.bbox_m.xmin;
         if ( mesh.bbox_m.xmax > xmax ) xmax = mesh.bbox_m.xmax;
         if ( mesh.bbox_m.ymin < ymin ) ymin = mesh.bbox_m.ymin;
         if ( mesh.bbox_m.ymax > ymax ) ymax = mesh.bbox_m.ymax;
         if ( mesh.bbox_m.zmin < zmin ) zmin = mesh.bbox_m.zmin;
         if ( mesh.bbox_m.zmax > zmax ) zmax = mesh.bbox_m.zmax;
         break;
         }
       }
     }
/*
***Assure that the viewbox has positive volume.
*/
   if ( xmin > xmax ) xmin = xmax = 0.0;
   if ( ymin > ymax ) ymin = ymax = 0.0;
   if ( zmin > zmax ) zmin = zmax = 0.0;

   if ( xmax - xmin < 1E-3 )
     {
     xmax += 0.01;
     xmin -= 0.01;
     }

   if ( ymax - ymin < 1E-3 )
     {
     ymax += 0.01;
     ymin -= 0.01;
     }

   if ( zmax - zmin < 1E-3 )
     {
     zmax += 0.01;
     zmin -= 0.01;
     }
/*
***Where is the centre and which direction is biggest ?
*/
   centre_x = (xmin + xmax)/2.0;
   centre_y = (ymin + ymax)/2.0;
   centre_z = (zmin + zmax)/2.0;

   mdx = xmax - xmin;
   mdy = ymax - ymin;
   mdz = zmax - zmin;

   biggest = mdx;
   if ( mdy > biggest ) biggest = mdy;
   if ( mdz > biggest ) biggest = mdz;
/*
***Set the size of the viewbox in X and Y equal to 1.2*(the biggest)
***of X, Y and Z. This is a compromise between the possibility
***to rotate the object without getting it clipped to the window
***limits and still getting a reasonable scale.
*/
   xmax = centre_x + 1.2*biggest/2.0;
   xmin = centre_x - 1.2*biggest/2.0;
   ymax = centre_y + 1.2*biggest/2.0;
   ymin = centre_y - 1.2*biggest/2.0;
/*
***For Z we calculate the true space diagonal and make the viewbox
***big enough to permit rotation without Z-clipping.
*/
   biggest = SQRT(3*biggest*biggest)/2.0;
   zmax = centre_z + biggest;
   zmin = centre_z - biggest;
/*
***Window size in millimeters.
*/
   gadx = rwinpt->geo.psiz_x *
        (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin);
   gady = rwinpt->geo.psiz_y *
        (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin);
/*
***Viewbox size in millimeters.
*/
   mdx = xmax - xmin;
   mdy = ymax - ymin;
/*
***Window proportions are gady/gadx. Adjust viewbox X and Y
***to the same proportions.
*/
   gprop = gady/gadx;
   mprop = mdy/mdx;

   if ( mprop > gprop )
     {
     rwinpt->xmin -= (mdy/gprop - mdx)/2.0;
     rwinpt->xmax += (mdy/gprop - mdx)/2.0;
     }
   else if ( mprop < gprop )
     {
     rwinpt->ymin -= (gprop*mdx - mdy)/2.0;
     rwinpt->ymax += (gprop*mdx - mdy)/2.0;
     }
/*
***Save viewbox in WPRWIN.
*/
   rwinpt->xmin = xmin;
   rwinpt->xmax = xmax;
   rwinpt->ymin = ymin;
   rwinpt->ymax = ymax;
   rwinpt->zmin = zmin;
   rwinpt->zmax = zmax;
/*
***Turn off wait....
*/
#ifdef UNIX
   WPscur(rwinpt->id.w_id,FALSE,(Cursor)0);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_ARROW));
#endif
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

    short WPmodl_all(WPRWIN *rwinpt)

/*      Make OpenGL displaylist for all entities (List 1).
 *      Traverse DB and process all visible entities
 *      into a single OpenGL display list.
 *
 *      NOTE: The RC for this window must be activated
 *             before calling.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-09-22 DBCsys, J.Kjellander
 *      1998-10-27 Bugfix deallokering, J.Kjellander
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2004-07-10 Mesh, J.Kjellander, ï¿½rebro university
 *      2007-06-17 1.19 J.Kjellander
 *
 ******************************************************!*/

 {
   int       i;
   DBetype  type;
   DBptr    la;
   char     str[V3STRLEN+1];
   DBfloat  crdvek[4*GMXMXL],cn;
   DBHeader hdr;
   DBPoint  poi;
   DBLine   lin;
   DBArc    arc;
   DBCurve  cur;
   DBText   txt;
   DBHatch  xht;
   DBLdim   ldm;
   DBCdim   cdm;
   DBRdim   rdm;
   DBAdim   adm;
   DBBplane bpl;
   DBCsys   csy;
   DBSurf   sur;
   DBSeg    arcseg[4],*graseg;
   DBMesh   mesh;
   DBfloat  model_size;

/*
***Initialization. Make pen and width invalid so
***that the first entity in the list forces them
***to be set.
*/
   actpen_gl = -1;
   actwdt    = -1.0;
/*
***Empty the namestack and push a default name.
***glLoadName() will replace the default name
***when entities from DB are processed.
*/
   glInitNames();
   glPushName(0);
/*
***Delete the current main list (if it exists).
*/
   if ( glIsList((GLuint)1) ) glDeleteLists((GLuint)1,(GLsizei)1);
/*
***Create a new main list (list 1).
*/
   glNewList(1,GL_COMPILE);
/*
***Render in Smooth mode.
*/
   glShadeModel(GL_SMOOTH);
/*
***Calculate normal's automatically.
*/
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);
/*
***Display both sides of polygons filled (shaded).
***This applies to B_planes, Mesh and FAC_SUR.
*/
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
/*
***Display NURBS surfaces filled (shaded) and
***use Varkon's displayfactor to control tesselation.
*/
   gl_nobj = gluNewNurbsRenderer();

   WPget_cacc(&cn);
   model_size = sqrt((rwinpt->xmax - rwinpt->xmin) * (rwinpt->xmax - rwinpt->xmin)+
                     (rwinpt->ymax - rwinpt->ymin) * (rwinpt->ymax - rwinpt->ymin)+
                     (rwinpt->zmax - rwinpt->zmin) * (rwinpt->zmax - rwinpt->zmin));
   nurbs_display_factor = cn / model_size;

   gluNurbsProperty(gl_nobj,GLU_DISPLAY_MODE,GLU_FILL);
   gluNurbsProperty(gl_nobj,GLU_SAMPLING_METHOD,GLU_DOMAIN_DISTANCE);
/*
***Traverse DB.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     DBread_header(&hdr,la);
     if ( !hdr.blank  &&  WPnivt(rwinpt->nivtab,hdr.level) )
       {
/*
***Color/material.
*/
       if ( hdr.pen != actpen_gl )
         {
         WPspenGL(rwinpt,hdr.pen);
         }
/*
***Load the DB pointer to the name stack.
*/
       glLoadName(la);
/*
***Process geometry.
*/
       switch ( type )
         {
/*
***A point.
*/
         case POITYP:
         DBread_point(&poi,la);
         if ( poi.wdt_p != actwdt ) set_linewidth(rwinpt,poi.wdt_p);
         pr_poi(rwinpt,&poi);
         break;
/*
***A line.
*/
         case LINTYP:
         DBread_line(&lin,la);
         if ( lin.wdt_l != actwdt ) set_linewidth(rwinpt,lin.wdt_l);
         pr_lin(rwinpt,&lin);
         break;
/*
***An arc.
*/
         case ARCTYP:
         DBread_arc(&arc,arcseg,la);
         if ( arc.wdt_a != actwdt ) set_linewidth(rwinpt,arc.wdt_a);
         pr_arc(rwinpt,&arc,arcseg);
         break;
/*
***A curve.
*/
         case CURTYP:
         DBread_curve(&cur,&graseg,NULL,la);
         if ( cur.wdt_cu != actwdt ) set_linewidth(rwinpt,cur.wdt_cu);
         pr_cur(rwinpt,&cur,graseg);
         DBfree_segments(graseg);
         break;
/*
***A text.
*/
         case TXTTYP:
         DBread_text(&txt,str,la);
         if ( txt.wdt_tx != actwdt ) set_linewidth(rwinpt,txt.wdt_tx);
         pr_txt(rwinpt,&txt,str);
         break;
/*
***A hatch.
*/
         case XHTTYP:
         DBread_xhatch(&xht,crdvek,&csy,la);
         if ( xht.wdt_xh != actwdt ) set_linewidth(rwinpt,xht.wdt_xh);
         pr_xht(rwinpt,&xht,crdvek,&csy);
         break;
/*
***A linear dimension.
*/
         case LDMTYP:
         DBread_ldim(&ldm,&csy,la);
         if ( ldm.wdt_ld != actwdt ) set_linewidth(rwinpt,ldm.wdt_ld);
         pr_ldm(rwinpt,&ldm,&csy);
         break;
/*
***A circular dimension.
*/
         case CDMTYP:
         DBread_cdim(&cdm,&csy,la);
         if ( cdm.wdt_cd != actwdt ) set_linewidth(rwinpt,cdm.wdt_cd);
         pr_cdm(rwinpt,&cdm,&csy);
         break;
/*
***A radius dimension.
*/
         case RDMTYP:
         DBread_rdim(&rdm,&csy,la);
         if ( rdm.wdt_rd != actwdt ) set_linewidth(rwinpt,rdm.wdt_rd);
         pr_rdm(rwinpt,&rdm,&csy);
         break;
/*
***An angular dimension.
*/
         case ADMTYP:
         DBread_adim(&adm,&csy,la);
         if ( adm.wdt_ad != actwdt ) set_linewidth(rwinpt,adm.wdt_ad);
         pr_adm(rwinpt,&adm,&csy);
         break;
/*
***A B-plane.
*/
         case BPLTYP:
         DBread_bplane(&bpl,la);
         if ( bpl.wdt_bp != actwdt ) set_linewidth(rwinpt,bpl.wdt_bp);
         pr_bpl(rwinpt,&bpl);
         break;
/*
***A mesh.
*/
         case MSHTYP:
         DBread_mesh(&mesh,la,MESH_ALL);
         if ( mesh.wdt_m != actwdt ) set_linewidth(rwinpt,mesh.wdt_m);
         pr_msh(rwinpt,&mesh);
         break;
/*
***A coordinate system.
*/
         case CSYTYP:
         DBread_csys(&csy,NULL,la);
         if ( actwdt != 0.0 ) set_linewidth(rwinpt,0.0);

         if ( la == lsysla )
           {
           if ( WPgrst("varkon.act_csys_pen",str)  &&  sscanf(str,"%d",&i) == 1 )
             {
             WPspenGL(rwinpt,(short)i);
             }
           }
          else if ( csy.hed_pl.pen != actpen_gl )
           {
           WPspenGL(rwinpt,csy.hed_pl.pen);
           }

         pr_csy(rwinpt,&csy,la);
         break;
/*
***A surface.
*/
         case SURTYP:
         DBread_surface(&sur,la);
         if ( sur.wdt_su != actwdt ) set_linewidth(rwinpt,sur.wdt_su);
         if ( sur.typ_su == FAC_SUR ) pr_sur2(rwinpt,&sur);
         else                         pr_sur1(rwinpt,&sur);
         break;
         }
       }
     }
/*
***Close list.
*/
   glEndList();
/*
***End.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

    short   WPmodl_highlight(
    WPRWIN *rwinpt,
    int     n_ents,
    DBptr   la_arr[])

/*      Make OpenGL display list for one or more
 *      highlighted entities.
 *
 *      In: rwinpt = C ptr to the WPRWIN
 *          n_ents = Number of entities
 *          la_arr = Array of entity DBptr's
 *
 *      (C)2007-06-16 J. Kjellander
 *
 ******************************************************!*/

 {
   int      i;
   bool     org_fill;
   DBptr    la;
   char     str[V3STRLEN+1];
   DBfloat  crdvek[4*GMXMXL];
   DBHeader hdr;
   DBPoint  poi;
   DBLine   lin;
   DBArc    arc;
   DBCurve  cur;
   DBText   txt;
   DBHatch  xht;
   DBLdim   ldm;
   DBCdim   cdm;
   DBRdim   rdm;
   DBAdim   adm;
   DBBplane bpl;
   DBCsys   csy;
   DBSurf   sur;
   DBSeg    arcseg[4],*graseg;
   DBMesh   mesh;

/*
***Activate the RC of this window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Create new highlight list.
*/
 ++n_hllists;
   glNewList(n_hllists + 1,GL_COMPILE);
/*
***Highlight color.
*/
   WPspenGL(rwinpt,WP_ENTHG);
/*
***No depth test.
*/
   glDepthFunc(GL_ALWAYS);
/*
***Loop through all entities in la_arr.
*/
   for ( i=0; i<n_ents; ++i )
     {
     la = la_arr[i];
     DBread_header(&hdr,la);
/*
***Process geometry.
*/
     switch ( hdr.type )
       {
/*
***A point.
*/
       case POITYP:
       DBread_point(&poi,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,poi.wdt_p) + 1);
       pr_poi(rwinpt,&poi);
       break;
/*
***A line.
*/
       case LINTYP:
       DBread_line(&lin,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,lin.wdt_l) + 1);
       pr_lin(rwinpt,&lin);
       break;
/*
***An arc.
*/
       case ARCTYP:
       DBread_arc(&arc,arcseg,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,arc.wdt_a) + 1);
       pr_arc(rwinpt,&arc,arcseg);
       break;
/*
***A curve.
*/
       case CURTYP:
       DBread_curve(&cur,&graseg,NULL,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,cur.wdt_cu) + 1);
       pr_cur(rwinpt,&cur,graseg);
       DBfree_segments(graseg);
       break;
/*
***A text.
*/
       case TXTTYP:
       DBread_text(&txt,str,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,txt.wdt_tx) + 1);
       pr_txt(rwinpt,&txt,str);
       break;
/*
***A hatch.
*/
       case XHTTYP:
       DBread_xhatch(&xht,crdvek,&csy,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,xht.wdt_xh) + 1);
       pr_xht(rwinpt,&xht,crdvek,&csy);
       break;
/*
***A linear dimension.
*/
       case LDMTYP:
       DBread_ldim(&ldm,&csy,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,ldm.wdt_ld) + 1);
       pr_ldm(rwinpt,&ldm,&csy);
       break;
/*
***A circular dimension.
*/
       case CDMTYP:
       DBread_cdim(&cdm,&csy,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,cdm.wdt_cd) + 1);
       pr_cdm(rwinpt,&cdm,&csy);
       break;
/*
***A radius dimension.
*/
       case RDMTYP:
       DBread_rdim(&rdm,&csy,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,rdm.wdt_rd) + 1);
       pr_rdm(rwinpt,&rdm,&csy);
       break;
/*
***An angular dimension.
*/
       case ADMTYP:
       DBread_adim(&adm,&csy,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,adm.wdt_ad) + 1);
       pr_adm(rwinpt,&adm,&csy);
       break;
/*
***A B-plane.
*/
       case BPLTYP:
       org_fill = rwinpt->fill;
       rwinpt->fill = FALSE;
       DBread_bplane(&bpl,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,bpl.wdt_bp) + 1);
       pr_bpl(rwinpt,&bpl);
       rwinpt->fill = org_fill;
       break;
/*
***A mesh.
*/
       case MSHTYP:
       DBread_mesh(&mesh,la,MESH_ALL);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,mesh.wdt_m) + 1);
       pr_msh(rwinpt,&mesh);
       break;
/*
***A coordinate system.
*/
       case CSYTYP:
       DBread_csys(&csy,NULL,la);
       glLineWidth((GLfloat)2);
       pr_csy(rwinpt,&csy,la);
       break;
/*
***A surface.
*/
       case SURTYP:
       org_fill = rwinpt->fill;
       rwinpt->fill = FALSE;
       DBread_surface(&sur,la);
       glLineWidth((GLfloat)width_to_pixels(rwinpt,sur.wdt_su) + 1);
       if ( sur.typ_su == FAC_SUR ) pr_sur2(rwinpt,&sur);
       else                         pr_sur1(rwinpt,&sur);
       rwinpt->fill = org_fill;
       break;
       }
     }
/*
***Close list.
*/
   actwdt = -1;
   glDepthFunc(GL_LESS);
   glEndList();
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

    void WPdodl_highlight(WPRWIN *rwinpt)

/*      Delete all highlight lists in this window.
 *
 *      (C)2007-06-6 J. Kjellander
 *
 ******************************************************!*/

 {

/*
***Delete all highligt lists (if there are any ).
*/
   if ( n_hllists > 0 )
     {
     glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
     glDeleteLists((GLuint)(2),(GLsizei)n_hllists);
     n_hllists = 0;
     }
/*
***The end.
*/
   return;
 }

/********************************************************/
/********************************************************/

    short WPsodl_all(WPRWIN *rwinpt)

/*      Show (execute) all OpenGL Display Lists in
 *      this window.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-12-09 Perspektiv mm. J.Kjellander
 *      1999-01-04 Z-klipp, J.Kjellander
 *      2007-06-14 1.19, J.Kjellander
 *      2008-01-23 Lights, J.Kjellander
 *
 ******************************************************!*/

 {
   int      i;
   double   mdx,mdy,mdz,midx,midy,midz,vxmax,vxmin,vymax,
            vymin,vzmax,vzmin,vdx,vdy,vdz05,fd;
   bool     defined,on,follow_model;
   DBfloat  intensity;
   GLfloat  gl_matrix[16];

static GLdouble plane[4] = {0.0,0.0,-1.0,0.0};

/*
***Activate the OpenGL RC of this window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***Each new frame must start with clearing the previous.
***This is done here but may be moved to the calling functions
***in the future if we need to be able to display a list without
***clearing.
*/
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
***The size of the view box.
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
   mdz = rwinpt->zmax - rwinpt->zmin;
/*
***Projection transformation.
*/
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
/*
***vdz05 = half size in Z.
*/
   vdz05 = 0.5*mdz;
/*
***The position of the focal plane (and position of box in Z)
***depends on the current perspective. Not to be smaller than
***half box dz and vary within reasonable values so that
***a good variation in perspective effect is achieved when
***rwinpt->pfactor varies between 0 -> 100. Default
***pfactor = 0%.
*/
   fd = mdz + 0.2*(100.0-rwinpt->pfactor)*mdz;
/*
***Scale the viewbox according to current WPRWIN.
*/
   vdx = mdx*rwinpt->scale;
   vdy = mdy*rwinpt->scale;
/*
***Vyboxens placering i X- och Y-led ï¿½r alltid
***symmetriskt runt (0,0). Storleken beror av
***aktuellt prespektiv. Formler frï¿½n Brian Paul.
*/
   vxmax = 0.5*vdx*(fd-vdz05)/fd;
   vxmin = -vxmax;

   vymax = 0.5*vdy*(fd-vdz05)/fd;
   vymin = -vymax;
/*
***Vyboxens placering i Z-led ï¿½r alltid symmetriskt
***runt fokalplanet.
*/
   vzmax = fd+vdz05;
   vzmin = fd-vdz05;
/*
***Skapa projektionsmatris. Om pfactor=0 kï¿½r vi med Ortho
***vilket bï¿½r vara snabbare.
*/
   if ( rwinpt->pfactor > 0.0 ) glFrustum(vxmin,vxmax,vymin,vymax,vzmin,vzmax);
   else                         glOrtho(vxmin,vxmax,vymin,vymax,vzmin,vzmax);
/*
***Modellboxens mittpunkt.
*/
   midx = rwinpt->xmin + 0.5*mdx;
   midy = rwinpt->ymin + 0.5*mdy;
   midz = rwinpt->zmin + 0.5*mdz;
/*
***Model transformation.
*/
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
/*
***Optional Z-clipping.
*/
   if ( rwinpt->zclip )
     {
     plane[3] = -(vzmin + rwinpt->zfactor*mdz*0.01);
     glClipPlane(GL_CLIP_PLANE0,plane);
     }
/*
***Eftersom transformationerna "utfï¿½rs" i
***omvï¿½nd ordning mot den man multiplicerar
***ihop matriserna med kommer hï¿½r sista
***transformationen. Den fï¿½rflyttar den roterade
***modellen frï¿½n (0,0,0) till den position
***i XY-planet den panorerats till och till
***det stï¿½lle i Z-led som perspektivet krï¿½ver.
*/
   glTranslated(-rwinpt->movx,-rwinpt->movy,-fd);
/*
***Adjust the current rotation coefficients with values
***from the window. The mouse may have been moved.
*/
   glGetFloatv(GL_MODELVIEW_MATRIX,gl_matrix);
   gl_matrix[ 0] = (GLfloat)rwinpt->vy.matrix.k11;
   gl_matrix[ 1] = (GLfloat)rwinpt->vy.matrix.k21;
   gl_matrix[ 2] = (GLfloat)rwinpt->vy.matrix.k31;
   gl_matrix[ 3] = 0.0;
   gl_matrix[ 4] = (GLfloat)rwinpt->vy.matrix.k12;
   gl_matrix[ 5] = (GLfloat)rwinpt->vy.matrix.k22;
   gl_matrix[ 6] = (GLfloat)rwinpt->vy.matrix.k32;
   gl_matrix[ 7] = 0.0;
   gl_matrix[ 8] = (GLfloat)rwinpt->vy.matrix.k13;
   gl_matrix[ 9] = (GLfloat)rwinpt->vy.matrix.k23;
   gl_matrix[10] = (GLfloat)rwinpt->vy.matrix.k33;
   gl_matrix[11] = 0.0;
   glLoadMatrixf(gl_matrix);
/*
***Rotate the model.
*/
   glRotated(rwinpt->rotx,gl_matrix[0],gl_matrix[4],gl_matrix[8] );
   glRotated(rwinpt->roty,gl_matrix[1],gl_matrix[5],gl_matrix[9] );
/*
***Clear rotation angles in window.
*/
   rwinpt->rotx = 0.0;
   rwinpt->roty = 0.0;
/*
***Rotation around Z currently not used.
*
   glRotated(rwinpt->rotz,gl_matrix[ 2],gl_matrix[ 6],gl_matrix[10] );
   rwinpt->rotz = 0.0;
*
***Remember the new model rotation.
*/
   glGetFloatv(GL_MODELVIEW_MATRIX,gl_matrix);
   rwinpt->vy.matrix.k11 = (DBfloat)gl_matrix[ 0];
   rwinpt->vy.matrix.k21 = (DBfloat)gl_matrix[ 1];
   rwinpt->vy.matrix.k31 = (DBfloat)gl_matrix[ 2];
   rwinpt->vy.matrix.k12 = (DBfloat)gl_matrix[ 4];
   rwinpt->vy.matrix.k22 = (DBfloat)gl_matrix[ 5];
   rwinpt->vy.matrix.k32 = (DBfloat)gl_matrix[ 6];
   rwinpt->vy.matrix.k13 = (DBfloat)gl_matrix[ 8];
   rwinpt->vy.matrix.k23 = (DBfloat)gl_matrix[ 9];
   rwinpt->vy.matrix.k33 = (DBfloat)gl_matrix[10];
/*
***Here is the 1:st transformation. Model
***translated back to the origin.
*/
   glTranslated(-midx,-midy,-midz);
/*
***Define light sources that should be transformed
***with the MODELVIEW matrix here.
*/
   for ( i=0; i<7; ++i )
     {
     WPget_light(i,&defined,&on,&intensity,&follow_model);
     if ( defined && on && follow_model ) WPactivate_light(i,intensity,1,1);
     }
/*
***Execute the main list.
*/
   glCallList((GLuint)1);
/*
***Execute optional highligt lists.
*/
   for ( i=0; i<n_hllists; ++i ) glCallList((GLuint)(2 + i));
/*
***Display.
*/
   if ( rwinpt->double_buffer )
#ifdef UNIX
     glXSwapBuffers(xdisp,rwinpt->id.x_id);
#endif
#ifdef WIN32
     SwapBuffers(rwinpt->dc);
#endif
   else glFlush();
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

    short   WPeodls(
    WPRWIN *rwinpt,
    int     ix,
    int     iy,
    int     dix,
    int     diy)

/*   Sets up a GL projection with a pick matrix and executes
 *   the GL list for selection.
 *
 *      (C)2007-02-14 J.Kjellander
 *
 ******************************************************!*/

 {
   double  mdx,mdy,mdz,vxmax,vxmin,vymax,
           vymin,vzmax,vzmin,vdx,vdy,vdz05,fd;
   GLint   viewport[4];

/*
***Get the current viewport and set up the pick matrix. The
***difference between normal rendering and rendering for 
***selection is that the PickMatrix is part of the projection.
*/
   glGetIntegerv(GL_VIEWPORT, viewport);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPickMatrix((GLdouble)ix,(GLdouble)iy,(GLdouble)dix,(GLdouble)diy,viewport);
/*
***Continue with the same projection setup as for normal
***display, see WPsodl().
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
   mdz = rwinpt->zmax - rwinpt->zmin;

   vdz05 = 0.5*mdz;

   fd = mdz + 0.2*(100.0-rwinpt->pfactor)*mdz;

   vdx = 1.2*mdx*rwinpt->scale;
   vdy = 1.2*mdy*rwinpt->scale;

   vxmax = 0.5*vdx*(fd-vdz05)/fd;
   vxmin = -vxmax;

   vymax = 0.5*vdy*(fd-vdz05)/fd;
   vymin = -vymax;

   vzmax = fd+vdz05;
   vzmin = fd-vdz05;
/*
***Here comes the final transformation.
*/
   if ( rwinpt->pfactor > 0.0 ) glFrustum(vxmin,vxmax,vymin,vymax,vzmin,vzmax);
   else                         glOrtho(vxmin,vxmax,vymin,vymax,vzmin,vzmax);
/*
***Call the main list.
*/
   glCallList((GLuint)1);
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

static void     pr_poi(
       WPRWIN  *rwinpt,
       DBPoint *poi)

/*     Process Point.
 * 
 *      (C)microform ab 1998-10-05 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2004-07-25 Bug linewidth, J.Kjellander
 *      2006-12-28 Polyline rep, J.Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],size;
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Point size = 1.5% of current frustum size.
*/
   size = 1.5/((rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
                rwinpt->geo.psiz_x /
               (rwinpt->xmax - rwinpt->xmin));
/*
***Create graphical representation.
*/
   k = -1;
   WPplpt(poi,size,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   if ( k > 0  &&  k < PLYMXV )
     {
     i = 0;
     while ( i <= k )
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
         {
         glBegin(GL_LINES);
         glVertex3d(x[i-1], y[i-1], z[i-1]);
         glVertex3d(x[i], y[i], z[i]);                  
         glEnd();
         }
       i++;
       }
     }
 }

/********************************************************/
/********************************************************/

static void    pr_lin(
       WPRWIN *rwinpt,
       DBLine *lin)

/*     Process Line.
 * 
 *      (C)microform ab 1998-01-09 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2006-12-08 WPplli(), J.Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create graphical representation.
*/
   k = -1;
   WPplli(lin,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   if ( k > 0  &&  k < PLYMXV )
     {
     i = 0;
     while ( i <= k )
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
         {
         glBegin(GL_LINES);
         glVertex3d(x[i-1], y[i-1], z[i-1]);
         glVertex3d(x[i], y[i], z[i]);                  
         glEnd();
         }
       i++;
       }
     }
 }

/********************************************************/
/*!******************************************************/

static void    pr_arc(
       WPRWIN *rwinpt,
       DBArc  *arc,
       DBSeg  *seg)

/*     Process Arc.
 * 
 *      (C)microform ab 1998-10-05 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2004-07-25 pr_cur(), J.Kjellander
 *      2006-12-09 WPplar(), J.Kjellander
 *
 ******************************************************!*/

 {
   int    i,k;
   char   a[PLYMXV];
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / frustum_size
*/
   scale = (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
            rwinpt->geo.psiz_x / (rwinpt->xmax - rwinpt->xmin);
/*
***Create polyline.
*/
   k = -1;
   WPplar(arc,seg,scale,&k,x,y,z,a);
/*
***Give polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3d(x[i],y[i],z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3d(x[i],y[i],z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void     pr_cur(
       WPRWIN  *rwinpt,
       DBCurve *cur,
       DBSeg   *graseg)

/*     Process Curve.
 * 
 *      (C)microform ab 1998-10-05 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2006-12-10 Removed gpxxxx(), J.Kjellander
 *
 ******************************************************!*/

 {
   int     i,k;
   char    a[PLYMXV];
   DBfloat x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / frustum_size
*/
   scale = (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
            rwinpt->geo.psiz_x / (rwinpt->xmax - rwinpt->xmin);
/*
***Create polyline.
*/
   k = -1;
   WPplcu(cur,graseg,scale,&k,x,y,z,a);
/*
***Ge polylinjen till OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void    pr_txt(
       WPRWIN *rwinpt,
       DBText *txt,
       char   *str)

/*     Process Text.
 * 
 *      (C)microform ab 1998-10-01 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2006-12-16 GP->WP, J.Kjellander
 *
 ******************************************************!*/

 {
   int   i,k;
   char  a[PLYMXV];
   DBfloat x[PLYMXV],y[PLYMXV],z[PLYMXV];

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create 3D polyline.
*/
   k = -1;
   WPpltx(txt,(unsigned char *)str,&k,x,y,z,a);
/*
***Give it to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void     pr_xht(
       WPRWIN  *rwinpt,
       DBHatch *xht,
       DBfloat  crdvek[],
       DBCsys  *csyptr)

/*     Process Hatch.
 * 
 *      (C)2006-12-17 J. Kjellander
 *
 ******************************************************!*/

 {
   int   i,k;
   char  a[PLYMXV];
   DBfloat x[PLYMXV],y[PLYMXV],z[PLYMXV];

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create 3D polyline.
*/
   k = -1;
   WPplxh(xht,crdvek,csyptr,&k,x,y,z,a);
/*
***Give it to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void    pr_ldm(
       WPRWIN *rwinpt,
       DBLdim *ldmptr,
       DBCsys *csyptr)

/*     Process Linear dimension.
 * 
 *      (C)2006-12-26 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create graphical representation.
*/
   k = -1;
   WPplld(ldmptr,csyptr,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }

   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void    pr_cdm(
       WPRWIN *rwinpt,
       DBCdim *cdmptr,
       DBCsys *csyptr)

/*     Process Circular dimension.
 * 
 *      (C)2006-12-26 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create graphical representation.
*/
   k = -1;
   WPplcd(cdmptr,csyptr,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void    pr_rdm(
       WPRWIN *rwinpt,
       DBRdim *rdmptr,
       DBCsys *csyptr)

/*     Process Radius dimension.
 * 
 *      (C)2006-12-26 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Create graphical representation.
*/
   k = -1;
   WPplrd(rdmptr,csyptr,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void    pr_adm(
       WPRWIN *rwinpt,
       DBAdim *admptr,
       DBCsys *csyptr)

/*     Process Angular dimension.
 * 
 *      (C)2006-12-26 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;
   char   a[PLYMXV];
   int    i,k;

/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / frustum_size
*/
   scale = (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
            rwinpt->geo.psiz_x / (rwinpt->xmax - rwinpt->xmin);
/*
***Create graphical representation.
*/
   k = -1;
   WPplad(admptr,csyptr,scale,&k,x,y,z,a);
/*
***Give the polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( (a[i] & VISIBLE) == VISIBLE ) 
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   if ( i > 0 ) glEnd();
 }

/********************************************************/
/*!******************************************************/

static void      pr_bpl(
       WPRWIN   *rwinpt,
       DBBplane *bpl)

/*     Process B_plane.
 * 
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2007-01-14 Wire/surf, J.Kjellander
 *
 ******************************************************!*/

 {
   bool     triangle;
   double   d,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
   GLfloat  nx,ny,nz;
   DBVector p21,p32,p41,prod;
   double   x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char     a[PLYMXV];
   int      i,k;

/*
***If we are in fill mode, calculate normal = vecn(vprod(p2-p1,p4-p1))
***and display as GL_TRIANGLES or GL_QUADS.
*/
   if ( rwinpt->fill )
     {
/*
***Light model.
*/
     set_lightmodel(rwinpt,SURFACE_MODEL);
/*
***Vertex coordinates.
*/
     x1 = bpl->crd1_bp.x_gm;
     y1 = bpl->crd1_bp.y_gm;
     z1 = bpl->crd1_bp.z_gm;

     x2 = bpl->crd2_bp.x_gm;
     y2 = bpl->crd2_bp.y_gm;
     z2 = bpl->crd2_bp.z_gm;

     x3 = bpl->crd3_bp.x_gm;
     y3 = bpl->crd3_bp.y_gm;
     z3 = bpl->crd3_bp.z_gm;

     x4 = bpl->crd4_bp.x_gm;
     y4 = bpl->crd4_bp.y_gm;
     z4 = bpl->crd4_bp.z_gm;
/*
***3 or 4 sided ?
*/
     p32.x_gm = x3 - x2;
     p32.y_gm = y3 - y2;
     p32.z_gm = z3 - z2;

     d = p32.x_gm*p32.x_gm + p32.y_gm*p32.y_gm + p32.z_gm*p32.z_gm;

     if ( d < DTOL ) triangle = TRUE;
     else            triangle = FALSE;
/*
***Calculate normal.
*/
     p21.x_gm = x2 - x1;
     p21.y_gm = y2 - y1;
     p21.z_gm = z2 - z1;

     p41.x_gm = x4 - x1;
     p41.y_gm = y4 - y1;
     p41.z_gm = z4 - z1;

     prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
     prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
     prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;

     d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

     if ( d > DTOL )
       {
       d = 1.0/SQRT(d);
       nx = (GLfloat)(prod.x_gm * d);
       ny = (GLfloat)(prod.y_gm * d);
       nz = (GLfloat)(prod.z_gm * d);
       }
     else return;
/*
***Give to GL.
*/
     if ( triangle )
       {
       glBegin(GL_TRIANGLES);
       glNormal3f(nx,ny,nz);
       glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
       glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
       glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
       glEnd();
       }
     else
       {
       glBegin(GL_QUADS);
       glNormal3f(nx,ny,nz);
       glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
       glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
       glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
       glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
       glEnd();
       }
     }
/*
***In wireframe mode, cretae polyline and display as
***GL_LINE_STRIP.
*/
   else
     {
     set_lightmodel(rwinpt,WIREFRAME_MODEL);
     k = -1;
     WPplbp(bpl,&k,x,y,z,a);

     for ( i=0; i<=k; ++i )
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
         {
         glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
         }
       else
         {
         if ( i > 0 ) glEnd();
         glBegin(GL_LINE_STRIP);
         glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
         }
       }
     if ( i > 0 ) glEnd();
     }
 }

/********************************************************/
/*!******************************************************/

static void    pr_msh(
       WPRWIN *rwinpt,
       DBMesh *mesh)

/*     Process Mesh.
 * 
 *      (C)2004-07-11, J. Kjellander, ï¿½rebro university
 *
 *      2007-01-14 Wire/surf, J.Kjellander
 *
 ******************************************************!*/

 {
   int      i,edge_1,edge_2,edge_3,
            vertex_1,vertex_2,vertex_3,
            sv_index,ev_index,oh_index;
   double   d,x1,x2,x3,x4,x5,x6,x7,x8,
            y1,y2,y3,y4,y5,y6,y7,y8,
            z1,z2,z3,z4,z5,z6,z7,z8;
   GLfloat  nx,ny,nz;
   DBVector p21,p31,prod;

/*
***If (faces exist) and (the font is 0 or 4) and
***(the display mode is fill), use face representation.
*/
   if ( mesh->nf_m > 0  &&
       (mesh->font_m == 0  ||  mesh->font_m == 4) &&
        rwinpt->fill ) 
     {
     set_lightmodel(rwinpt,SURFACE_MODEL);
     glBegin(GL_TRIANGLES);
     for ( i=0; i<mesh->nf_m; ++i )
       {
       edge_1 = mesh->pf_m[i].i_hedge;
       edge_2 = mesh->ph_m[edge_1].i_nhedge;
       edge_3 = mesh->ph_m[edge_2].i_nhedge;

       vertex_1 = mesh->ph_m[edge_1].i_evertex;
       vertex_2 = mesh->ph_m[edge_2].i_evertex;
       vertex_3 = mesh->ph_m[edge_3].i_evertex;
/*
***Vertex coordinates.
*/
       x1 = mesh->pv_m[vertex_1].p.x_gm;
       y1 = mesh->pv_m[vertex_1].p.y_gm;
       z1 = mesh->pv_m[vertex_1].p.z_gm;

       x2 = mesh->pv_m[vertex_2].p.x_gm;
       y2 = mesh->pv_m[vertex_2].p.y_gm;
       z2 = mesh->pv_m[vertex_2].p.z_gm;

       x3 = mesh->pv_m[vertex_3].p.x_gm;
       y3 = mesh->pv_m[vertex_3].p.y_gm;
       z3 = mesh->pv_m[vertex_3].p.z_gm;
/*
***Face normal.
*/
       p21.x_gm = x2 - x1;
       p21.y_gm = y2 - y1;
       p21.z_gm = z2 - z1;

       p31.x_gm = x3 - x1;
       p31.y_gm = y3 - y1;
       p31.z_gm = z3 - z1;

       prod.x_gm = p21.y_gm*p31.z_gm - p21.z_gm*p31.y_gm;
       prod.y_gm = p21.z_gm*p31.x_gm - p21.x_gm*p31.z_gm;
       prod.z_gm = p21.x_gm*p31.y_gm - p21.y_gm*p31.x_gm;

       d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;
  
       if ( d > DTOL )
         {
         d = 1.0/SQRT(d);
         nx = (GLfloat)(prod.x_gm * d);
         ny = (GLfloat)(prod.y_gm * d);
         nz = (GLfloat)(prod.z_gm * d);
         }
       else return;
/*
***Feed the face data to OpenGL.
*/
       glNormal3f(nx,ny,nz);
       glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
       glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
       glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
       }
     glEnd();
     }
/*
***If (edges exist) and (the font is 0 or 3 or displaymode = wireframe)
***use edge representation.
*/
   else if ( mesh->nh_m > 0  &&
            (mesh->font_m == 0 || mesh->font_m == 3  || !rwinpt->fill) ) 
     {
     set_lightmodel(rwinpt,WIREFRAME_MODEL);
     glBegin(GL_LINES);
     for ( i=0; i<mesh->nh_m; ++i )
       {
       oh_index = mesh->ph_m[i].i_ohedge;
       if ( oh_index > i )
         {
         sv_index = mesh->ph_m[oh_index].i_evertex;
         ev_index = mesh->ph_m[i].i_evertex;
         x1 = mesh->pv_m[sv_index].p.x_gm;
         y1 = mesh->pv_m[sv_index].p.y_gm;
         z1 = mesh->pv_m[sv_index].p.z_gm;
         x2 = mesh->pv_m[ev_index].p.x_gm;
         y2 = mesh->pv_m[ev_index].p.y_gm;
         z2 = mesh->pv_m[ev_index].p.z_gm;
         glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
         glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
         }
       }
     glEnd();
     }
/*
***Bounding box representation.
*/
   else if ( mesh->font_m == 1 )
     {
     set_lightmodel(rwinpt,WIREFRAME_MODEL);
     x1 = mesh->bbox_m.xmin;
     y1 = mesh->bbox_m.ymin;
     z1 = mesh->bbox_m.zmin;

     x2 = mesh->bbox_m.xmax;
     y2 = mesh->bbox_m.ymin;
     z2 = mesh->bbox_m.zmin;

     x3 = mesh->bbox_m.xmax;
     y3 = mesh->bbox_m.ymax;
     z3 = mesh->bbox_m.zmin;

     x4 = mesh->bbox_m.xmin;
     y4 = mesh->bbox_m.ymax;
     z4 = mesh->bbox_m.zmin;

     x5 = mesh->bbox_m.xmin;
     y5 = mesh->bbox_m.ymin;
     z5 = mesh->bbox_m.zmax;

     x6 = mesh->bbox_m.xmax;
     y6 = mesh->bbox_m.ymin;
     z6 = mesh->bbox_m.zmax;

     x7 = mesh->bbox_m.xmax;
     y7 = mesh->bbox_m.ymax;
     z7 = mesh->bbox_m.zmax;

     x8 = mesh->bbox_m.xmin;
     y8 = mesh->bbox_m.ymax;
     z8 = mesh->bbox_m.zmax;

     glBegin(GL_LINES);

     glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
     glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);

     glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
     glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);

     glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
     glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);

     glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
     glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);

     glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
     glVertex3f((GLfloat)x5,(GLfloat)y5,(GLfloat)z5);

     glVertex3f((GLfloat)x5,(GLfloat)y5,(GLfloat)z5);
     glVertex3f((GLfloat)x6,(GLfloat)y6,(GLfloat)z6);

     glVertex3f((GLfloat)x6,(GLfloat)y6,(GLfloat)z6);
     glVertex3f((GLfloat)x7,(GLfloat)y7,(GLfloat)z7);

     glVertex3f((GLfloat)x7,(GLfloat)y7,(GLfloat)z7);
     glVertex3f((GLfloat)x8,(GLfloat)y8,(GLfloat)z8);

     glVertex3f((GLfloat)x8,(GLfloat)y8,(GLfloat)z8);
     glVertex3f((GLfloat)x5,(GLfloat)y5,(GLfloat)z5);

     glVertex3f((GLfloat)x8,(GLfloat)y8,(GLfloat)z8);
     glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);

     glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
     glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);

     glVertex3f((GLfloat)x7,(GLfloat)y7,(GLfloat)z7);
     glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);

     glVertex3f((GLfloat)x6,(GLfloat)y6,(GLfloat)z6);
     glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);

     glEnd();
     }
/*
***Point representation.
*/
   else
     {
     set_lightmodel(rwinpt,WIREFRAME_MODEL);
     glBegin(GL_POINTS);
     for ( i=0; i<mesh->nv_m; ++i )
       {
       x1 = mesh->pv_m[i].p.x_gm;
       y1 = mesh->pv_m[i].p.y_gm;
       z1 = mesh->pv_m[i].p.z_gm;
       glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
       }
     glEnd();
     }
 }

/********************************************************/
/*!******************************************************/

static void    pr_csy(
       WPRWIN *rwinpt,
       DBCsys *csy,
       DBptr   la)

/*    Process coordinate system.
 * 
 *    (C)microform ab 1998-09-22 J. Kjellander
 *
 *    2000-08-25 Linewidth added, J.Kjellander
 *    2006-12-16 Active csys, J.Kjellander
 *
 ******************************************************!*/

 {
   int    i,k,mode;
   double size;
   char   a[PLYMXV];
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];

/*
***Axis length depend on size of window.
*/
   size = rwinpt->xmax - rwinpt->xmin;
/*
***Light model.
*/
   set_lightmodel(rwinpt,WIREFRAME_MODEL);
/*
***Currently acvtive coordinate system looks different.
*/
   if ( la == lsysla ) mode = V3_CS_ACTIVE;
   else                mode = V3_CS_NORMAL;
/*
***Create 3D-polyline.
*/
   k = -1;
   WPplcs(csy,size,mode,&k,x,y,z,a);
/*
***Give polyline to OpenGL.
*/
   for ( i=0; i<=k; ++i )
     {
     if ( a[i] == VISIBLE )
       {
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     else
       {
       if ( i > 0 ) glEnd();
       glBegin(GL_LINE_STRIP);
       glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
       }
     }
   glEnd();
 }

/********************************************************/
/*!******************************************************/

static void     pr_sur1(
       WPRWIN  *rwinpt,
       DBSurf  *sur)

/*     Process Surface (not FAC_SUR).
 * 
 *      (C)microform ab 1998-01-08 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2007-01-08 Vertextype, Sï¿½ren L
 *      2007-01-08 usteps, vsteps, Sï¿½ren L
 *      2007-01-14 Wire/surf, J.Kjellander
 *      2007-01-19 Trimmed surfaces, Sï¿½ren L
 *
 ******************************************************!*/

 {
   DBPatch        *patpek;
   DBfloat         uscale,vscale;
   GLfloat        *p_kvu,*p_kvv,*p_cpts;
   GLfloat         ustep,vstep;
   int             i,k;
   char            a[PLYMXV];
   double          x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;
   DBSegarr       *pborder;
   DBSegarr       *piso;
   DBGrstrimcurve *pgrstrimcvs;

/*
***If we are in fill mode, read NURBS data from DB and
***display as a NURBS object.
*/
   if ( rwinpt->fill )
     {
     DBread_sur_grsur(sur,&p_kvu,&p_kvv,&p_cpts,&pgrstrimcvs);
/*
***Call WPuvstepsu if needed (this should normally be precalculated and stored in DB)
*/
     if ( sur->nustep_su == -1 || sur->nvstep_su == -1)
       {
       uscale = ((DBfloat)sur->nu_su) / (p_kvu[sur->nku_su-1] - p_kvu[0]);
       vscale = ((DBfloat)sur->nv_su) / (p_kvv[sur->nkv_su-1] - p_kvv[0]);
       DBread_patches(sur,&patpek);
       WPuvstepsu(sur,patpek,uscale,vscale);
       DBfree_patches(sur,patpek);
       }
/*
***Calculate OpenGL usteps and vsteps for this rendering.
*/
     ustep = 1 + floor(sur->nustep_su*nurbs_display_factor);
     vstep = 1 + floor(sur->nvstep_su*nurbs_display_factor);

     gluNurbsProperty(gl_nobj,GLU_U_STEP,ustep);
     gluNurbsProperty(gl_nobj,GLU_V_STEP,vstep);
/*
***Give NURBS data to OpenGL.
*/
     set_lightmodel(rwinpt,SURFACE_MODEL);
     gluBeginSurface(gl_nobj);

     if ( sur->vertextype_su == GL_MAP2_VERTEX_3 )
       {
       gluNurbsSurface(gl_nobj,
                       sur->nku_su, p_kvu,
                       sur->nkv_su, p_kvv,
                      (sur->nkv_su-sur->vorder_su)*3, 3,
                       p_cpts,
                       sur->uorder_su, sur->vorder_su,
                       GL_MAP2_VERTEX_3);
       }
     else if ( sur->vertextype_su == GL_MAP2_VERTEX_4 )
       {
       gluNurbsSurface(gl_nobj,
                       sur->nku_su, p_kvu,
                       sur->nkv_su, p_kvv,
                      (sur->nkv_su-sur->vorder_su)*4, 4,
                       p_cpts,
                       sur->uorder_su, sur->vorder_su,
                       GL_MAP2_VERTEX_4);
       }
     else
       {
       erpush("WP1673",""); /* undefined vertex type */
       errmes();
       }
/*
***Trim curves if any
*/
     if ( sur->ntrim_su > 0  )
       {
       for ( i=0; i<sur->ntrim_su; i++ )
         {
         gluBeginTrim(gl_nobj);
         if ( pgrstrimcvs[i].vertextype == GLU_MAP1_TRIM_2 )
           {
           gluNurbsCurve(gl_nobj,(GLint) pgrstrimcvs[i].nknots, pgrstrimcvs[i].knots_c,
                   2, pgrstrimcvs[i].cpts_c,(GLint) pgrstrimcvs[i].order,GLU_MAP1_TRIM_2);
           }
         else if ( pgrstrimcvs[i].vertextype == GLU_MAP1_TRIM_3 )
           {
           gluNurbsCurve(gl_nobj,(GLint) pgrstrimcvs[i].nknots, pgrstrimcvs[i].knots_c,
                   3, pgrstrimcvs[i].cpts_c,(GLint) pgrstrimcvs[i].order,GLU_MAP1_TRIM_3);
           }
         else
           {
           erpush("WP1673",""); /* undefined vertex type */
           errmes();
           }
         gluEndTrim(gl_nobj);
         }
       }
     DBfree_sur_grsur(sur,p_kvu,p_kvv,p_cpts,pgrstrimcvs);
     gluEndSurface(gl_nobj);
     }
/*
***In wireframe mode, read curves from DB, create polyline
***and display as GL_LINE_STRIP.
*/
   else
     {
     DBread_sur_grwire(sur,&pborder,&piso);

     scale = (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
              rwinpt->geo.psiz_x / (rwinpt->xmax - rwinpt->xmin);
     k = -1;
     WPplsu(sur,pborder,piso,scale,&k,x,y,z,a);
     set_lightmodel(rwinpt,WIREFRAME_MODEL);

     for ( i=0; i<=k; ++i )
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
         {
         glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
         }
       else
         {
         if ( i > 0 ) glEnd();
         glBegin(GL_LINE_STRIP);
         glVertex3f((GLfloat)x[i],(GLfloat)y[i],(GLfloat)z[i]);
         }
       }
     if ( i > 0 ) glEnd();

     DBfree_sur_grwire(sur,pborder,piso);
     }
 }

/********************************************************/
/*!******************************************************/

static void     pr_sur2(
       WPRWIN  *rwinpt,
       DBSurf  *sur)

/*     Process Surface with facet representation (FAC_SUR).
 * 
 *      (C)microform ab 1998-11-23 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2007-01-14 Wire/surf, J.Kjellander
 *
 ******************************************************!*/

 {
   int       i,j;
   double    d,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
   GLfloat   nx,ny,nz;
   DBVector  p21,p41,prod;
   DBPatch  *toppat,*patpek;
   DBPatchF *facpat;
 
/*
***Read patches from DB.
*/
   DBread_patches(sur,&patpek);
/*
***Give facetts to OpenGL.
*/ 
   toppat = patpek;
   for ( i=0; i<sur->nu_su; ++i )
     {
     for ( j=0; j<sur->nv_su; ++j )
       {
       if ( toppat->styp_pat == FAC_PAT )
         {
         facpat = (DBPatchF *)toppat->spek_c;
         x1 = facpat->p1.x_gm; y1 = facpat->p1.y_gm; z1 = facpat->p1.z_gm;
         x2 = facpat->p2.x_gm; y2 = facpat->p2.y_gm; z2 = facpat->p2.z_gm;
         x3 = facpat->p3.x_gm; y3 = facpat->p3.y_gm; z3 = facpat->p3.z_gm;
         x4 = facpat->p4.x_gm; y4 = facpat->p4.y_gm; z4 = facpat->p4.z_gm;
/*
***The facet is either one quad.......
*/
         if ( !facpat->triangles )
           {
/*
***A filled quad.
*/
           if ( rwinpt->fill )
             {
             set_lightmodel(rwinpt,SURFACE_MODEL);
             glBegin(GL_QUADS);
             p21.x_gm = x2 - x1;
             p21.y_gm = y2 - y1;
             p21.z_gm = z2 - z1;
             p41.x_gm = x4 - x1;
             p41.y_gm = y4 - y1;
             p41.z_gm = z4 - z1;
             prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
             prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
             prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;
             d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

             if ( d > DTOL )
               {
               d = 1.0/SQRT(d);
               nx = (GLfloat)(prod.x_gm * d);
               ny = (GLfloat)(prod.y_gm * d);
               nz = (GLfloat)(prod.z_gm * d);
               }

             glNormal3f(nx,ny,nz);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
             glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
             glEnd();
             }
/*
***A wireframe quad.
*/
           else
             {
             set_lightmodel(rwinpt,WIREFRAME_MODEL);
             glBegin(GL_LINE_STRIP);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
             glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glEnd();
             }
           }
/*
***...or 2 triangles.
*/
         else
           {
/*
***2 filled triangles.
*/
           if ( rwinpt->fill )
             {
             set_lightmodel(rwinpt,SURFACE_MODEL);
             glBegin(GL_TRIANGLES);
             p21.x_gm = x2 - x1;
             p21.y_gm = y2 - y1;
             p21.z_gm = z2 - z1;
             p41.x_gm = x3 - x1;
             p41.y_gm = y3 - y1;
             p41.z_gm = z3 - z1;
             prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
             prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
             prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;
             d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

             if ( d > DTOL )
               {
               d = 1.0/SQRT(d);
               nx = (GLfloat)(prod.x_gm * d);
               ny = (GLfloat)(prod.y_gm * d);
               nz = (GLfloat)(prod.z_gm * d);
               }

             glNormal3f(nx,ny,nz);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);

             p21.x_gm = x3 - x1;
             p21.y_gm = y3 - y1;
             p21.z_gm = z3 - z1;
             p41.x_gm = x4 - x1;
             p41.y_gm = y4 - y1;
             p41.z_gm = z4 - z1;
             prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
             prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
             prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;
             d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

             if ( d > DTOL )
               {
               d = 1.0/SQRT(d);
               nx = (GLfloat)(prod.x_gm * d);
               ny = (GLfloat)(prod.y_gm * d);
               nz = (GLfloat)(prod.z_gm * d);
               }

             glNormal3f(nx,ny,nz);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
             glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
             glEnd();
             }
/*
***2 wireframe triangles.
*/
           else
             {
             set_lightmodel(rwinpt,WIREFRAME_MODEL);
             glBegin(GL_LINE_STRIP);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x2,(GLfloat)y2,(GLfloat)z2);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
             glVertex3f((GLfloat)x4,(GLfloat)y4,(GLfloat)z4);
             glVertex3f((GLfloat)x1,(GLfloat)y1,(GLfloat)z1);
             glVertex3f((GLfloat)x3,(GLfloat)y3,(GLfloat)z3);
             glEnd();
             }
           }
         }
       ++toppat;
       }
     }
   DBfree_patches(sur,patpek);
 }

/********************************************************/
/********************************************************/

  static void    set_linewidth(
         WPRWIN *rwinpt,
         DBfloat width)

/*      Sets current linewidth.
 *
 *      In: rwinpt = C-ptr to rendering window.
 *          width  = Linewidth in millimeters.
 *
 *      (C)ï¿½rebro University 2000-08-25 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Calculate linewidth in pixels and notify OpenGL.
*/
   glLineWidth((GLfloat)width_to_pixels(rwinpt,width));
/*
***Set current linewidth global variable.
*/
   actwdt = width;

   return;
  }

/********************************************************/
/********************************************************/

  static int     width_to_pixels(
         WPRWIN *rwinpt,
         DBfloat width)

/*      Calculates linewidth in pixels.
 *
 *      In: rwinpt = C-ptr to rendering window.
 *          width  = Linewidth in millimeters.
 *
 *      (C)2007-06-17 J. Kjellander
 *
 ******************************************************!*/

  {
   int    npix;
   double scale;

/*
***Calculate linewidth in pixels and notify OpenGL.
*/
   if ( width == 0.0 ) npix = 1;
   else
     {
     scale = (rwinpt->geo.psiz_x *
             (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin))/
             (rwinpt->xmax - rwinpt->xmin);
     npix  = (int)((scale*width)/((rwinpt->geo.psiz_x+rwinpt->geo.psiz_y)/2.0) + 0.5);
     }
/*
***The end.
*/
   return(npix);
  }

/********************************************************/
/********************************************************/

  static void    set_lightmodel(
         WPRWIN *rwinpt,
         int     model)

/*      Sets the current lightmodel. Wireframes are
 *      rendered in GL_LIGHT_MODEL_AMBIENT mode. Surfaces
 *      are rendered using the currently active light
 *      sources.
 *
 *      In: rwinpt = C ptr to OpenGL window.
 *          model  = WIREFRAME_model or SURFACE_MODEL
 *
 *      (C)2007-01-29 J. Kjellander
 *
 ******************************************************!*/

  {
   static GLfloat ambient_su[4] = {0.4, 0.4, 0.4, 1.0 };
   static GLfloat ambient_wf[4] = {1.0, 1.0, 1.0, 1.0 };
   static int     current_model = -1;

   if ( model != current_model )
     {
     switch ( model )
       {
       case WIREFRAME_MODEL:
       WPenable_lights(FALSE);
       glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient_wf);
       current_model = WIREFRAME_MODEL;
       break;

       case SURFACE_MODEL:
       glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient_su);
       WPenable_lights(TRUE);
       current_model = SURFACE_MODEL;
       break;
       }
     }
  }

/********************************************************/
