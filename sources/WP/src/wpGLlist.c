/**********************************************************************
*
*    wpGLlist.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPmmod();    Measures size of model
*    WPmodl();    Creates OpenGL displaylist
*    WPsodl();    Displays OpenGL displaylist
*    WPscog();    Sets active color
*    WPnrrw();    Normalizes view box
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

static short        actpen;    /* Current pen */
static DBfloat      actwdt;    /* Current linewidth */
static GLUnurbsObj *gl_nobj;   /* Nurbs-object */

extern DBptr  lsysla;          /* Currently active csys */

#ifdef WIN32
extern int   msgrgb();
#endif

static void  pr_poi(WPRWIN *rwinpt, DBPoint *poi);
static void  pr_lin(WPRWIN *rwinpt, DBLine *lin);
static void  pr_arc(WPRWIN *rwinpt, DBArc *arc, DBSeg *arcseg);
static void  pr_cur(WPRWIN *rwinpt, DBCurve *cur, DBSeg *graseg);
static void  pr_txt(WPRWIN *rwinpt, DBText *txt, char *str);
static void  pr_xht(WPRWIN *rwinpt, DBHatch *xht, DBfloat crdvek[]);
static void  pr_ldm(WPRWIN *rwinpt, DBLdim *ldm);
static void  pr_cdm(WPRWIN *rwinpt, DBCdim *cdm);
static void  pr_rdm(WPRWIN *rwinpt, DBRdim *rdm);
static void  pr_adm(WPRWIN *rwinpt, DBAdim *adm);
static void  pr_bpl(WPRWIN *rwinpt, DBBplane *bpl);
static void  pr_msh(WPRWIN *rwinpt, DBMesh *mesh);
static void  pr_csy(WPRWIN *rwinpt, DBCsys *csy, DBptr la);
static void  pr_sur1(WPRWIN *rwinpt, DBSurf *sur, GLfloat *p_kvu,
                     GLfloat *p_kvv, GLfloat *p_cpts);
static void  pr_sur2(WPRWIN *rwinpt, DBSurf *sur, DBPatch *patpek);
static void  set_linewidth(WPRWIN *rwinpt, DBfloat width);

/*!******************************************************/

    short   WPmmod(
    WPRWIN *rwinpt)

/*    Calculates the bounding box of the model.
 * 
 *    In: rwinpt = C-ptr to WPRWIN
 *
 *    Return: 0
 *
 *    (C)microform ab 1998-01-09 J. Kjellander
 *
 *    1998-10-27 Bugfix deallokering, J.Kjellander
 *    2004-07-10 Mesh J.Kjellander, Örebro university
 *    2006-12-09 Removed gpxxxx() calls, J.Kjellander
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
   double   x[PLYMXV],y[PLYMXV],z[PLYMXV],crdvek[4*GMXMXL];
   double   xmin,xmax,ymin,ymax,zmin,zmax;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***This might take time.
*/
#ifdef V3_X11
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
     if ( !hdr.blank  &&  WPnivt(gwinpt,hdr.level) )
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
***An arc.
***En Cirkelbåge, scale=1 bör bytas ut mot smartare beräkning som
***oavsett curnog ger rimligt få vektorer i polylinjen.
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
***A curve. Scale must be fixed.
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
         DBread_xhatch(&xht,crdvek,la);
         i = 0;
         while ( i <  4*xht.nlin_xh )
           {
           if ( crdvek[i] < xmin ) xmin = crdvek[i];
           if ( crdvek[i] > xmax ) xmax = crdvek[i];
           ++i;
           if ( crdvek[i] < ymin ) ymin = crdvek[i];
           if ( crdvek[i] > ymax ) ymax = crdvek[i];
           ++i;
           }
         break;
/*
***A Linear dimension.
*/
         case LDMTYP:
         DBread_ldim(&ldm,la);
         if ( ldm.p1_ld.x_gm < xmin ) xmin = ldm.p1_ld.x_gm;
         if ( ldm.p1_ld.x_gm > xmax ) xmax = ldm.p1_ld.x_gm;
         if ( ldm.p1_ld.y_gm < ymin ) ymin = ldm.p1_ld.y_gm;
         if ( ldm.p1_ld.y_gm > ymax ) ymax = ldm.p1_ld.y_gm;

         if ( ldm.p2_ld.x_gm < xmin ) xmin = ldm.p2_ld.x_gm;
         if ( ldm.p2_ld.x_gm > xmax ) xmax = ldm.p2_ld.x_gm;
         if ( ldm.p2_ld.y_gm < ymin ) ymin = ldm.p2_ld.y_gm;
         if ( ldm.p2_ld.y_gm > ymax ) ymax = ldm.p2_ld.y_gm;

         if ( ldm.p3_ld.x_gm < xmin ) xmin = ldm.p3_ld.x_gm;
         if ( ldm.p3_ld.x_gm > xmax ) xmax = ldm.p3_ld.x_gm;
         if ( ldm.p3_ld.y_gm < ymin ) ymin = ldm.p3_ld.y_gm;
         if ( ldm.p3_ld.y_gm > ymax ) ymax = ldm.p3_ld.y_gm;
         break;
/*
***A Circilar dimension.
*/
         case CDMTYP:
         DBread_cdim(&cdm,la);
         if ( cdm.p1_cd.x_gm < xmin ) xmin = cdm.p1_cd.x_gm;
         if ( cdm.p1_cd.x_gm > xmax ) xmax = cdm.p1_cd.x_gm;
         if ( cdm.p1_cd.y_gm < ymin ) ymin = cdm.p1_cd.y_gm;
         if ( cdm.p1_cd.y_gm > ymax ) ymax = cdm.p1_cd.y_gm;

         if ( cdm.p2_cd.x_gm < xmin ) xmin = cdm.p2_cd.x_gm;
         if ( cdm.p2_cd.x_gm > xmax ) xmax = cdm.p2_cd.x_gm;
         if ( cdm.p2_cd.y_gm < ymin ) ymin = cdm.p2_cd.y_gm;
         if ( cdm.p2_cd.y_gm > ymax ) ymax = cdm.p2_cd.y_gm;

         if ( cdm.p3_cd.x_gm < xmin ) xmin = cdm.p3_cd.x_gm;
         if ( cdm.p3_cd.x_gm > xmax ) xmax = cdm.p3_cd.x_gm;
         if ( cdm.p3_cd.y_gm < ymin ) ymin = cdm.p3_cd.y_gm;
         if ( cdm.p3_cd.y_gm > ymax ) ymax = cdm.p3_cd.y_gm;
         break;
/*
***A Radius dimension.
*/
         case RDMTYP:
         DBread_rdim(&rdm,la);
         if ( rdm.p1_rd.x_gm < xmin ) xmin = rdm.p1_rd.x_gm;
         if ( rdm.p1_rd.x_gm > xmax ) xmax = rdm.p1_rd.x_gm;
         if ( rdm.p1_rd.y_gm < ymin ) ymin = rdm.p1_rd.y_gm;
         if ( rdm.p1_rd.y_gm > ymax ) ymax = rdm.p1_rd.y_gm;

         if ( rdm.p2_rd.x_gm < xmin ) xmin = rdm.p2_rd.x_gm;
         if ( rdm.p2_rd.x_gm > xmax ) xmax = rdm.p2_rd.x_gm;
         if ( rdm.p2_rd.y_gm < ymin ) ymin = rdm.p2_rd.y_gm;
         if ( rdm.p2_rd.y_gm > ymax ) ymax = rdm.p2_rd.y_gm;

         if ( rdm.p3_rd.x_gm < xmin ) xmin = rdm.p3_rd.x_gm;
         if ( rdm.p3_rd.x_gm > xmax ) xmax = rdm.p3_rd.x_gm;
         if ( rdm.p3_rd.y_gm < ymin ) ymin = rdm.p3_rd.y_gm;
         if ( rdm.p3_rd.y_gm > ymax ) ymax = rdm.p3_rd.y_gm;
         break;
/*
***An angular dimension.
*/
         case ADMTYP:
         DBread_adim(&adm,la);
         if ( adm.pos_ad.x_gm < xmin ) xmin = adm.pos_ad.x_gm;
         if ( adm.pos_ad.x_gm > xmax ) xmax = adm.pos_ad.x_gm;
         if ( adm.pos_ad.y_gm < ymin ) ymin = adm.pos_ad.y_gm;
         if ( adm.pos_ad.y_gm > ymax ) ymax = adm.pos_ad.y_gm;
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

   if ( xmax - xmin < 1E3 )
     {
     xmax += 0.01;
     xmin -= 0.01;
     }

   if ( ymax - ymin < 1E3 )
     {
     ymax += 0.01;
     ymin -= 0.01;
     }

   if ( zmax - zmin < 1E3 )
     {
     zmax += 0.01;
     zmin -= 0.01;
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
***Normalize viewbox.
*/
   WPnrrw(rwinpt);
/*
***Turn off wait....
*/
#ifdef V3_X11
   WPscur(rwinpt->id.w_id,FALSE,(Cursor)0);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_ARROW));
#endif
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

    short   WPmodl(
    WPRWIN *rwinpt)

/*      Make OpenGL display list. Traverse DB and
 *      process all visible entities.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-09-22 DBCsys, J.Kjellander
 *      1998-10-27 Bugfix deallokering, J.Kjellander
 *      2000-08-25 Linewidth added, J.Kjellander
 *      2004-07-10 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBetype  type;
   DBptr    la;
   char     str[V3STRLEN+1];
   GLfloat *p_kvu,*p_kvv,*p_cpts,tol;
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
   DBPatch *patpek;
   DBMesh   mesh;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;

/*
***Get a ptr to the main graphics window.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Turn on Wait....
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
   actpen = -1;
   actwdt = 0.0;
/*
***Inte säker på att man behöver sätta upp
***modelltransformation innan man skapar listan
***men det kostar så lite.
*/
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
/*
***Skapa ny lista. Vi använder bara list-id 1.
*/
   glNewList(1,GL_COMPILE);
/*
***Diverse andra inställningar.
*/
   glShadeModel(GL_SMOOTH);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);

   gl_nobj = gluNewNurbsRenderer();
   WPget_cacc(&cn);
   if ( (tol=(GLfloat)(100.0/cn)) < 1.0 ) tol = 1.0;
   gluNurbsProperty(gl_nobj,GLU_SAMPLING_METHOD,GLU_PATH_LENGTH);
   gluNurbsProperty(gl_nobj,GLU_SAMPLING_TOLERANCE,tol);
   gluNurbsProperty(gl_nobj,GLU_DISPLAY_MODE,GLU_FILL);
/*
***Linewidth and pointsize.
*/
   glPointSize((GLfloat)2);
   set_linewidth(rwinpt,actwdt);
/*
***Traverse DB.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     DBread_header(&hdr,la);
     if ( !hdr.blank  &&  WPnivt(gwinpt,hdr.level) )
       {
       switch ( type )
         {
/*
***A point.
*/
         case POITYP:
         DBread_point(&poi,la);
         pr_poi(rwinpt,&poi);
         break;
/*
***A line.
*/
         case LINTYP:
         DBread_line(&lin,la);
         pr_lin(rwinpt,&lin);
         break;
/*
***An arc.
*/
         case ARCTYP:
         DBread_arc(&arc,arcseg,la);
         pr_arc(rwinpt,&arc,arcseg);
         break;
/*
***A curve.
*/
         case CURTYP:
         DBread_curve(&cur,&graseg,NULL,la);
         pr_cur(rwinpt,&cur,graseg);
         DBfree_segments(graseg);
         break;
/*
***A text.
*/
         case TXTTYP:
         DBread_text(&txt,str,la);
         pr_txt(rwinpt,&txt,str);
         break;
/*
***A hatch.
*/
         case XHTTYP:
         DBread_xhatch(&xht,crdvek,la);
         pr_xht(rwinpt,&xht,crdvek);
         break;
/*
***A linear dimension.
*/
         case LDMTYP:
         DBread_ldim(&ldm,la);
         pr_ldm(rwinpt,&ldm);
         break;
/*
***A circular dimension.
*/
         case CDMTYP:
         DBread_cdim(&cdm,la);
         pr_cdm(rwinpt,&cdm);
         break;
/*
***A radius dimension.
*/
         case RDMTYP:
         DBread_rdim(&rdm,la);
         pr_rdm(rwinpt,&rdm);
         break;
/*
***An angular dimension.
*/
         case ADMTYP:
         DBread_adim(&adm,la);
         pr_adm(rwinpt,&adm);
         break;
/*
***A B-plane.
*/
         case BPLTYP:
         DBread_bplane(&bpl,la);
         pr_bpl(rwinpt,&bpl);
         break;
/*
***A mesh.
*/
         case MSHTYP:
         DBread_mesh(&mesh,la,MESH_ALL);
         pr_msh(rwinpt,&mesh);
         break;
/*
***A coordinate system.
*/
         case CSYTYP:
         DBread_csys(&csy,NULL,la);
         pr_csy(rwinpt,&csy,la);
         break;
/*
***A surface.
*/
         case SURTYP:
         DBread_surface(&sur,la);
         if ( sur.typ_su == FAC_SUR )
           {
           DBread_patches(&sur,&patpek);
           pr_sur2(rwinpt,&sur,patpek);
           DBfree_patches(&sur,patpek);
           }
         else
           {
           DBread_sur_grsur(&sur,&p_kvu,&p_kvv,&p_cpts);
           pr_sur1(rwinpt,&sur,p_kvu,p_kvv,p_cpts);
           DBfree_sur_grsur(p_kvu,p_kvv,p_cpts);
           }
         break;
         }
       }
     }
/*
***Close list.
*/
   glEndList();
/*
***Turn off Wait....
*/
#ifdef V3_X11
   WPscur(rwinpt->id.w_id,FALSE,(Cursor)0);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_ARROW));
#endif
/*
***End.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

    short   WPsodl(
    WPRWIN *rwinpt,
    GLuint  list)

/*      Visar en OpenGL Display List.
 * 
 *      Felkoder: 
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 *      1998-12-09 Perspektiv mm. J.Kjellander
 *      1999-01-04 Z-klipp, J.Kjellander
 *
 ******************************************************!*/

 {
   double   mdx,mdy,mdz,midx,midy,midz,vxmax,vxmin,vymax,
            vymin,vzmax,vzmin,vdx,vdy,vdz05,fd;
   GLfloat  gl_matrix[16];

static GLdouble plane[4] = {0.0,0.0,-1.0,0.0};

/*
***Det här kan ta tid.
*/
#ifdef V3_X11
   WPscur(rwinpt->id.w_id,TRUE,xwcur);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_WAIT));
#endif
/*
***Sudda.
*/
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
***Hur stor är modellboxen, dvs. den
***låda som WPmodl() skapat och som säkert
***rymmer hela modellen och dessutom tillåter att den
***roteras inuti lådan utan att sticka ut utanför.
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
***Vyboxens storlek i Z-led är konstant och
***lika stor som modellboxen i Z-led.
***vdz05 = halva vyboxens storlek.
*/
   vdz05 = 0.5*mdz;
/*
***Fokalplanet och därmed hela boxens läge i Z-led beror
***av aktuell perspektiv-faktor. Får ej vara mindre
***än halva vyboxens dz och skall variera inom vettiga värden
***så att lagom variation i perspektiv-effekt uppnås
***när rwinpt->pfactor musstyrs från 0 -> 100. Default
***pfaktor = 0%, se wp10.c.
*/
   fd = mdz + 0.2*(100.0-rwinpt->pfactor)*mdz;
/*
***Vyboxens storlek i X- och Y-led beror av modellboxens
***storlek och aktuell skala. Faktorn 1.3 är godtycklig.
***1.3 är "lagom" för att modellen skall bli lite mindre
***än fönstret.
*/
   vdx = 1.3*mdx*rwinpt->scale;
   vdy = 1.3*mdy*rwinpt->scale;
/*
***Vyboxens placering i X- och Y-led är alltid
***symmetriskt runt (0,0). Storleken beror av
***aktuellt prespektiv. Formler från Brian Paul.
*/
   vxmax = 0.5*vdx*(fd-vdz05)/fd;
   vxmin = -vxmax;

   vymax = 0.5*vdy*(fd-vdz05)/fd;
   vymin = -vymax;
/*
***Vyboxens placering i Z-led är alltid symmetriskt
***runt fokalplanet.
*/
   vzmax = fd+vdz05;
   vzmin = fd-vdz05;
/*
***Skapa projektionsmatris. Om pfactor=0 kör vi med Ortho
***vilket bör vara snabbare.
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
***Om Z-klipp är aktivt börjar vi med att lägga
***klipplanet på rätt Z.
*/
   if ( rwinpt->zclip )
     {
     plane[3] = -(vzmin + rwinpt->zfactor*mdz*0.01);
     glClipPlane(GL_CLIP_PLANE0,plane);
     }
/*
***Eftersom transformationerna "utförs" i
***omvänd ordning mot den man multiplicerar
***ihop matriserna med kommer här sista
***transformationen. Den förflyttar den roterade
***modellen från (0,0,0) till den position
***i XY-planet den panorerats till och till
***det ställe i Z-led som perspektivet kräver.
*/
   glTranslated(-rwinpt->movx,-rwinpt->movy,-fd);
/*
***Justera rotationskoefficienterna i den aktuella
***OpenGL-matrisen så att de motsvarar modellens aktuella
***rotationsläge. Gunnars kod.
*/
   glGetFloatv(GL_MODELVIEW_MATRIX,gl_matrix); 
   gl_matrix[ 0] = (GLfloat)rwinpt->vy.vymat.k11;
   gl_matrix[ 1] = (GLfloat)rwinpt->vy.vymat.k12;
   gl_matrix[ 2] = (GLfloat)rwinpt->vy.vymat.k13;
   gl_matrix[ 3] = 0.0;
   gl_matrix[ 4] = (GLfloat)rwinpt->vy.vymat.k21;
   gl_matrix[ 5] = (GLfloat)rwinpt->vy.vymat.k22;
   gl_matrix[ 6] = (GLfloat)rwinpt->vy.vymat.k23;
   gl_matrix[ 7] = 0.0;
   gl_matrix[ 8] = (GLfloat)rwinpt->vy.vymat.k31;
   gl_matrix[ 9] = (GLfloat)rwinpt->vy.vymat.k32;
   gl_matrix[10] = (GLfloat)rwinpt->vy.vymat.k33;
   gl_matrix[11] = 0.0;
   glLoadMatrixf(gl_matrix);
/*
***Rotera modellen runt aktuella axlar. Gunnars kod.
*/
   glRotated(rwinpt->rotx,gl_matrix[ 0],gl_matrix[ 4],gl_matrix[ 8] );
   glRotated(rwinpt->roty,gl_matrix[ 1],gl_matrix[ 5],gl_matrix[ 9] );
/*
***Nollställ rotationsvinklar igen.
*/
   rwinpt->rotx = 0.0;
   rwinpt->roty = 0.0;
/*
***Rotation runt Z, tills vidare ej supportad.
*
   glRotated(rwinpt->rotz,gl_matrix[ 2],gl_matrix[ 6],gl_matrix[10] );
   rwinpt->rotz = 0.0;
*
***Spara nya vymatrisen. Gunnars kod.
*/
   glGetFloatv(GL_MODELVIEW_MATRIX,gl_matrix);
   rwinpt->vy.vymat.k11 = (DBfloat)gl_matrix[ 0];
   rwinpt->vy.vymat.k12 = (DBfloat)gl_matrix[ 1];
   rwinpt->vy.vymat.k13 = (DBfloat)gl_matrix[ 2];
   rwinpt->vy.vymat.k21 = (DBfloat)gl_matrix[ 4];
   rwinpt->vy.vymat.k22 = (DBfloat)gl_matrix[ 5];
   rwinpt->vy.vymat.k23 = (DBfloat)gl_matrix[ 6];
   rwinpt->vy.vymat.k31 = (DBfloat)gl_matrix[ 8];
   rwinpt->vy.vymat.k32 = (DBfloat)gl_matrix[ 9];
   rwinpt->vy.vymat.k33 = (DBfloat)gl_matrix[10];
/*
***Här kommer 1:a transformationen. Modellen
***translateras till origo.
*/
   glTranslated(-midx,-midy,-midz);
/*
***Exekvera displaylistan.
*/
   glCallList(list);
/*
***Visa.
*/
   if ( rwinpt->double_buffer )
#ifdef V3_X11
     glXSwapBuffers(xdisp,rwinpt->id.x_id);
#endif
#ifdef WIN32
     SwapBuffers(rwinpt->dc);
#endif
   else glFlush();
/*
***Slå av vänta.
*/
#ifdef V3_X11
   WPscur(rwinpt->id.w_id,FALSE,(Cursor)0);
#endif
#ifdef WIN32
   SetCursor(LoadCursor(NULL,IDC_ARROW));
#endif
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

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
***Color.
*/
   if ( poi->hed_p.pen != actpen )
     {
     actpen = poi->hed_p.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( poi->wdt_p != actwdt ) set_linewidth(rwinpt, poi->wdt_p);
/*
***Point size = 1.5% of current frustum size.
*/
   size = 0.015*(rwinpt->xmax - rwinpt->xmin);
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
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

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
***Color.
*/
   if ( lin->hed_l.pen != actpen )
     {
     actpen = lin->hed_l.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( lin->wdt_l != actwdt ) set_linewidth(rwinpt, lin->wdt_l);
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
/*
***Slut.
*/
   return;
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
***Färg.
*/
   if ( arc->hed_a.pen != actpen )
     {
     actpen = arc->hed_a.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( arc->wdt_a != actwdt ) set_linewidth(rwinpt, arc->wdt_a);
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
/*
***End.
*/
   return;
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
***Color.
*/
   if ( cur->hed_cu.pen != actpen )
     {
     actpen = cur->hed_cu.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( cur->wdt_cu != actwdt ) set_linewidth(rwinpt, cur->wdt_cu);
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
/*
***Slut.
*/
   return;
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
***Color.
*/
   if ( txt->hed_tx.pen != actpen )
     {
     actpen = txt->hed_tx.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( txt->wdt_tx != actwdt ) set_linewidth(rwinpt, txt->wdt_tx);
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
/*
***End.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void     pr_xht(
       WPRWIN  *rwinpt,
       DBHatch *xht,
       DBfloat  crdvek[])

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
***Color.
*/
   if ( xht->hed_xh.pen != actpen )
     {
     actpen = xht->hed_xh.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0 ) set_linewidth(rwinpt, 0.0);
/*
***Create 3D polyline.
*/
   k = -1;
   WPplxh(xht,crdvek,&k,x,y,z,a);
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
/*
***End.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void    pr_ldm(
       WPRWIN *rwinpt,
       DBLdim *ldmptr)

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
***Color.
*/
   if ( ldmptr->hed_ld.pen != actpen )
     {
     actpen = ldmptr->hed_ld.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0 ) set_linewidth(rwinpt, 0.0);
/*
***Create graphical representation.
*/
   k = -1;
   WPplld(ldmptr,&k,x,y,z,a);
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
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void    pr_cdm(
       WPRWIN *rwinpt,
       DBCdim *cdmptr)

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
***Color.
*/
   if ( cdmptr->hed_cd.pen != actpen )
     {
     actpen = cdmptr->hed_cd.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0 ) set_linewidth(rwinpt, 0.0);
/*
***Create graphical representation.
*/
   k = -1;
   WPplcd(cdmptr,&k,x,y,z,a);
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
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void    pr_rdm(
       WPRWIN *rwinpt,
       DBRdim *rdmptr)

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
***Color.
*/
   if ( rdmptr->hed_rd.pen != actpen )
     {
     actpen = rdmptr->hed_rd.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0 ) set_linewidth(rwinpt, 0.0);
/*
***Create graphical representation.
*/
   k = -1;
   WPplrd(rdmptr,&k,x,y,z,a);
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
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void    pr_adm(
       WPRWIN *rwinpt,
       DBAdim *admptr)

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
***Color.
*/
   if ( admptr->hed_ad.pen != actpen )
     {
     actpen = admptr->hed_ad.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0 ) set_linewidth(rwinpt, 0.0);
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
   WPplad(admptr,scale,&k,x,y,z,a);
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
/*
***Slut.
*/
   return;
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
 *
 ******************************************************!*/

 {
   bool     triangle;
   double   d,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
   GLfloat  nx,ny,nz;
   DBVector p21,p32,p41,prod;

/*
***Linewidth.
*/
   if ( bpl->wdt_bp != actwdt ) set_linewidth(rwinpt, bpl->wdt_bp);
/*
***Initiering.
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
***Beräkna normalvektor vecn(vprod(p2-p1,p4-p1)).
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
***3- eller 4-sidigt plan ?
*/
   p32.x_gm = x3 - x2;
   p32.y_gm = y3 - y2;
   p32.z_gm = z3 - z2;

   d = p32.x_gm*p32.x_gm + p32.y_gm*p32.y_gm + p32.z_gm*p32.z_gm;

   if ( d < DTOL ) triangle = TRUE;
   else            triangle = FALSE;
/*
***Färg.
*/
   if ( bpl->hed_bp.pen != actpen )
     {
     actpen = bpl->hed_bp.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Ge planet till OpenGL.
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
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void    pr_msh(
       WPRWIN *rwinpt,
       DBMesh *mesh)

/*     Process Mesh.
 * 
 *      (C)2004-07-11, J. Kjellander, Örebro university
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
***Color.
*/
   if ( mesh->hed_m.pen != actpen )
     {
     actpen = mesh->hed_m.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0) set_linewidth(rwinpt, 0.0);
/*
***Face representation.
*/
   if ( mesh->nf_m > 0  &&  (mesh->font_m == 0  ||  mesh->font_m == 4) ) 
     {
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
***Edge representation.
*/
   else if ( mesh->nh_m > 0  &&  (mesh->font_m == 0 || mesh->font_m == 3) ) 
     {
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
/*
***End.
*/
   return;
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
   char   a[PLYMXV],buf[80];
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];

/*
***Axis length depend on size of window.
*/
   size = rwinpt->xmax - rwinpt->xmin;
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
***Color.
*/
   if ( mode == V3_CS_ACTIVE )
     {
     if ( WPgrst("varkon.act_csys_pen",buf)  &&
          sscanf(buf,"%d",&i) == 1 ) WPscog(rwinpt,(short)i);
     }
   else if ( csy->hed_pl.pen != actpen )
     {
     actpen = csy->hed_pl.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0) set_linewidth(rwinpt, 0.0);
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
/*
***End.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void     pr_sur1(
       WPRWIN  *rwinpt,
       DBSurf  *sur,
       GLfloat *p_kvu,
       GLfloat *p_kvv,
       GLfloat *p_cpts)

/*     Process Surface with NURBS-representation.
 * 
 *      (C)microform ab 1998-01-08 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *
 ******************************************************!*/

 {
 
/*
***Color.
*/
   if ( sur->hed_su.pen != actpen )
     {
     actpen = sur->hed_su.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0) set_linewidth(rwinpt, 0.0);
/*
***Give NURBS data to OpenGL.
*/ 
   gluBeginSurface(gl_nobj);

   gluNurbsSurface(gl_nobj,
                   sur->nku_su, p_kvu,
                   sur->nkv_su, p_kvv,
                  (sur->nkv_su-sur->vorder_su)*3, 3,
                   p_cpts,
                   sur->uorder_su, sur->vorder_su,                         
                   GL_MAP2_VERTEX_3);

   gluEndSurface(gl_nobj);
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

static void     pr_sur2(
       WPRWIN  *rwinpt,
       DBSurf  *sur,
       DBPatch *patpek)

/*     Process Surface with facet representation (FAC_SUR).
 * 
 *      (C)microform ab 1998-11-23 J. Kjellander
 *
 *      2000-08-25 Linewidth added, J.Kjellander
 *
 ******************************************************!*/

 {
   int      i,j;
   double   d,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
   GLfloat  nx,ny,nz;
   DBVector p21,p41,prod;
   DBPatch *toppat;
   DBPatchF  *facpat;
 
/*
***Color.
*/
   if ( sur->hed_su.pen != actpen )
     {
     actpen = sur->hed_su.pen;
     WPscog(rwinpt,actpen);
     }
/*
***Linewidth.
*/
   if ( actwdt != 0.0) set_linewidth(rwinpt, 0.0);
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
***Or 2 triangles.
*/
         else
           {
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
         }
       ++toppat;
       }
     }
/*
***End.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

        short   WPscog(
        WPRWIN *rwinpt,
        short   pen)

/*      Sätter aktiv färg eller material. Pena 1 tom MTTABSIZ
 *      kan användas för att definiera material.
 *
 *      In: VARKON pennummer.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
   GLfloat mat[4];


#ifdef V3_X11
   GLint   cind[3];
   XColor rgb;
#endif

#ifdef WIN32
   COLORREF col;
#endif


#ifdef V3_X11
   if ( rwinpt->rgb_mode )
     {
     WPgrgb(pen,&rgb);
     mat[0] = rgb.red/65536.0;
     mat[1] = rgb.green/65536.0;
     mat[2] = rgb.blue/65536.0;
     mat[3] = 1.0;

     glMaterialfv(GL_FRONT,GL_AMBIENT,mat);
     glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);
     glMaterialfv(GL_FRONT,GL_SPECULAR,mat);
     mat[0] = mat[1] = mat[2] = 0.0;
     glMaterialfv(GL_FRONT,GL_EMISSION,mat);
     glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)40.0);
     }
   else
     {
     cind[0] = cind[1] = cind[2] = (GLint)pen;
     glMaterialiv(GL_FRONT,GL_COLOR_INDEXES,cind);
     }
#endif
/*
***eller om vi kör WIN32 
*/
#ifdef WIN32
   msgrgb(pen,&col);
   mat[0] = (GLfloat)(GetRValue(col)/255.0);
   mat[1] = (GLfloat)(GetGValue(col)/255.0);
   mat[2] = (GLfloat)(GetBValue(col)/255.0);
   mat[3] = 1.0;

   glMaterialfv(GL_FRONT,GL_AMBIENT,mat);
   glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);
   glMaterialfv(GL_FRONT,GL_SPECULAR,mat);
   mat[0] = mat[1] = mat[2] = 0.0;
   glMaterialfv(GL_FRONT,GL_EMISSION,mat);
   glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)40.0);
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

  static void    set_linewidth(
         WPRWIN *rwinpt,
         DBfloat width)

/*      Sets current linewidth.
 *
 *      In: Linewidth in millimeters.
 *          Ptr to rendering window.
 *
 *      Out: -
 *
 *      Return: 0
 *
 *      (C)Örebro University 2000-08-25 J. Kjellander
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

   glLineWidth((GLfloat)npix);
/*
***Set current linewidth global variable.
*/
   actwdt = width;

   return;
  }

/********************************************************/
/*!******************************************************/

        short   WPnrrw(
        WPRWIN *rwinpt)

/*      Normaliserar proportionerna för RWIN-fönstrets
 *      vy-box.
 *
 *      In: rwinpt => Pekare till fönster.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1998-01-08 J. Kjellander
 *
 ******************************************************!*/

  {
   double mdx,mdy,mdz,gadx,gady,mprop,gprop;

/*
***Hur stor är fönstrets grafiska area.
*/
   gadx = rwinpt->geo.psiz_x *
        (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin);
   gady = rwinpt->geo.psiz_y *
        (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin);
/*
***Hur stort är modellfönstret i millimeter.
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
/*
***Förhållandet mellan grafiska areans höjd och bredd är gady/gadx.
***Se till att modellfönstret får samma förhållande så att cirklar
***blir "runda" tex.
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
***Gör boxen lite större.
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
   mdz = rwinpt->zmax - rwinpt->zmin;

   rwinpt->xmin -= 0.2*mdx;
   rwinpt->xmax += 0.2*mdx;

   rwinpt->ymin -= 0.2*mdy;
   rwinpt->ymax += 0.2*mdy;
/*
***Sätt boxens storlek i Z-led så stor att den klarar
***en full rotation utan att klippas.
*/
   if ( mdx > mdz )
     {
     rwinpt->zmin -= (mdx - mdz)/2.0;
     rwinpt->zmax += (mdx - mdz)/2.0;
     mdz = rwinpt->zmax - rwinpt->zmin;
     }

   if ( mdy > mdz )
     {
     rwinpt->zmin -= (mdy - mdz)/2.0;
     rwinpt->zmax += (mdy - mdz)/2.0;
     mdz = rwinpt->zmax - rwinpt->zmin;
     }

   rwinpt->zmin -= 0.6*mdz;
   rwinpt->zmax += 0.6*mdz;

   return(0);
  }

/********************************************************/
