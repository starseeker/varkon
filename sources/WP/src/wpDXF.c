/**********************************************************************
*
*    wpDXF.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    WPdxf_out();    Create plotfile on DXF format
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
#include "../../WP/include/WP.h"
#include "../../GE/include/geotol.h"
#include <math.h>

extern V3MDAT  sydata;

static double  x[PLYMXV],y[PLYMXV],z[PLYMXV];  /* Polyline coordinates */
static char    a[PLYMXV];                      /* and status */
static WPGWIN  actwin;                         /* Current window */

/*
***Internal function prototypes.
*/
static short dxfopo(FILE *filpek, DBPoint *poipek, DBVector *origo);
static short dxfoli(FILE *filpek, DBLine *linpek, DBVector *origo);
static short dxfoar(FILE *filpek, DBArc *arcpek, DBSeg *seg, DBVector *origo);
static short dxfocu(FILE *filpek, DBCurve *curpek, DBSeg *seg, DBVector *origo);
static short dxfocs(FILE *filpek, DBCsys *csypek, DBTmat *pmat, DBVector *origo);
static short dxfobp(FILE *filpek, DBBplane *bplpek, DBVector *origo);
static short dxfoms(FILE *filpek, DBMesh *mshpek, DBVector *origo);
static short dxfotx(FILE *filpek, DBText *txtpek, char str[], DBVector *origo);
static short dxfold(FILE *filpek, DBLdim *ldmptr, DBCsys *csyptr, DBVector *origo);
static short dxfocd(FILE *filpek, DBCdim *cdmpek, DBCsys *csyptr, DBVector *origo);
static short dxford(FILE *filpek, DBRdim *rdmpek, DBCsys *csyptr, DBVector *origo);
static short dxfoad(FILE *filpek, DBAdim *admpek, DBCsys *csyptr, DBVector *origo);
static short dxfoxh(FILE *filpek, DBHatch *xhtpek, DBfloat crd[], DBVector *origo);
static short dxfopl(FILE *filpek, DBHeader *hedpek, int n,
                    double x[], double y[], char a[], DBVector *origo);
static short dxfniv(FILE *filpek, short niv);
static short dxfpen(FILE *filpek, short pen);
static short dxfwdt(FILE *filpek, double wdt);
static short dxffnt(FILE *filpek, short typ, DBfloat lgt);

/*!******************************************************/

        short     WPdxf_out(
        WPGWIN   *gwinpt,
        FILE     *filpek,
        WPVIEW   *plotvy,
        DBVector *origo)

/*      Creates "plotfile" on DXF-format.
 *
 *      In:  filpek = Ptr to open DXF-fil.
 *           plotvy = Plot area.
 *           origo  = Plot origin.
 *
 *      Return :  0 = Ok
 *               -1 = Avbrott från tangentbordet.
 *           GP0043 = Systemfel.
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1999-06-14 Comment+HEADER, J.Kjellander
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *      2006-12-29 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   DBptr   la;
   DBetype type;
   char    str[V3STRLEN+1];
   DBfloat xhtcrd[4*GMXMXL];
   DBId    dummy;
   DBAny   gmpost;
   DBTmat  pmat;
   DBSeg  *segptr,arcseg[4];
   DBCsys  csy;

/*
***Initializations.
*/
   fseek(filpek,(long)0,0);
/*
***Projection and Clipping is performed to all
***entitys so we need a temp WPGWIN with the
***right clip boundaries. Make a copy of the
***current WPGWIN and adjust its model window
***to fit the requested plotvy.
*/
   V3MOME(gwinpt,&actwin,sizeof(WPGWIN));
   actwin.vy.modwin.xmin = plotvy->modwin.xmin;
   actwin.vy.modwin.xmax = plotvy->modwin.xmax;
   actwin.vy.modwin.ymin = plotvy->modwin.ymin;
   actwin.vy.modwin.ymax = plotvy->modwin.ymax;
/*
***Lets start with a comment.
*/
   fprintf(filpek,"999\nDXF-file created by Varkon %d.%d%c\n",
                  (int)sydata.vernr,(int)sydata.revnr,sydata.level);
/*
***Header section. AC1009 = R11.
*/
   fprintf(filpek,"  0\nSECTION\n  2\nHEADER\n");
   fprintf(filpek,"  9\n$ACADVER\n  1\nAC1009\n");
   fprintf(filpek,"  0\nENDSEC\n");
/*
***Entities section.
*/
   fprintf(filpek,"  0\nSECTION\n  2\nENTITIES\n");
/*
***Traversera GM.
*/
   DBget_pointer('F',&dummy,&la,&type);

   while ( DBget_pointer('N',&dummy,&la,&type) == 0 )
     {
/*
***Processa.
*/
     switch(type)
       {
       case POITYP: DBread_point(&gmpost.poi_un,la);
                    dxfopo(filpek,&gmpost.poi_un,origo); break;

       case LINTYP: DBread_line(&gmpost.lin_un,la);
                    dxfoli(filpek,&gmpost.lin_un,origo); break;
 
       case ARCTYP: DBread_arc(&gmpost.arc_un,arcseg,la);
                    dxfoar(filpek,&gmpost.arc_un,arcseg,origo); break;

       case CURTYP: DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
                    dxfocu(filpek,&gmpost.cur_un,segptr,origo);
                    DBfree_segments(segptr); break;

       case CSYTYP: DBread_csys(&gmpost.csy_un,&pmat,la);
                    dxfocs(filpek,&gmpost.csy_un,&pmat,origo); break;

       case BPLTYP: DBread_bplane(&gmpost.bpl_un,la);
                    dxfobp(filpek,&gmpost.bpl_un,origo); break;

       case MSHTYP: DBread_mesh(&gmpost.msh_un,la,MESH_HEADER+MESH_VERTEX+MESH_HEDGE);
                    dxfoms(filpek,&gmpost.msh_un,origo); break;

       case TXTTYP: DBread_text(&gmpost.txt_un,str,la);
                    dxfotx(filpek,&gmpost.txt_un,str,origo); break;

       case LDMTYP: DBread_ldim(&gmpost.ldm_un,&csy,la);
                    dxfold(filpek,&gmpost.ldm_un,&csy,origo); break;

       case CDMTYP: DBread_cdim(&gmpost.cdm_un,&csy,la);
                    dxfocd(filpek,&gmpost.cdm_un,&csy,origo); break;

       case RDMTYP: DBread_rdim(&gmpost.rdm_un,&csy,la);
                    dxford(filpek,&gmpost.rdm_un,&csy,origo); break;

       case ADMTYP: DBread_adim(&gmpost.adm_un,&csy,la);
                    dxfoad(filpek,&gmpost.adm_un,&csy,origo); break;

       case XHTTYP: DBread_xhatch(&gmpost.xht_un,xhtcrd,&csy,la);
                    dxfoxh(filpek,&gmpost.xht_un,xhtcrd,origo); break;
       }
     }
/*
***End section.
*/
   fprintf(filpek,"  0\nENDSEC\n  0\nEOF\n");
/*
***The end.
*/
   return(0);
}

/********************************************************/
/*!******************************************************/

 static short     dxfopo(
        FILE     *filpek,
        DBPoint  *poipek,
        DBVector *origo)

/*      Writes point on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           poipek = Pekare till punktpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   WPGRPT p;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,poipek->hed_p.level) ||
        poipek->hed_p.blank ) return(0);
/*
***Project the point on the viewplane of the window.
*/
   WPppos(&actwin,&poipek->crd_p,&p);
/*
***Clip to the borders of the "plot area".
*/
   if ( WPcpos(&actwin,&p) )
     {
/*
***Write to file.
*/
     fprintf(filpek,"  0\nPOINT\n");
     dxfniv(filpek,poipek->hed_p.level);
     dxfpen(filpek,poipek->hed_p.pen);
     fprintf(filpek," 10\n%g\n",p.x - origo->x_gm);
     fprintf(filpek," 20\n%g\n",p.y - origo->y_gm);
     }

   return(0);
 }

/*******************************************************/
/*!******************************************************/

 static short     dxfoli(
        FILE     *filpek,
        DBLine   *linpek,
        DBVector *origo)

/*      Writes a line on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           linpek = Pekare till linjepost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k,orgfont;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,linpek->hed_l.level) ||
        linpek->hed_l.blank ) return(0);
/*
***Create the polyline of a solid line.
*/
   orgfont = linpek->fnt_l;
   linpek->fnt_l = 0;
   k = -1;
   WPplli(linpek,&k,x,y,z,a);
   linpek->fnt_l = orgfont;
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
/*
***Write visible part to file.
*/
     fprintf(filpek,"  0\nLINE\n");
     dxffnt(filpek,linpek->fnt_l,linpek->lgt_l);
     dxfniv(filpek,linpek->hed_l.level);
     dxfpen(filpek,linpek->hed_l.pen);
     dxfwdt(filpek,linpek->wdt_l);
     fprintf(filpek," 10\n%g\n",x[0]-origo->x_gm);
     fprintf(filpek," 20\n%g\n",y[0]-origo->y_gm);
     fprintf(filpek," 11\n%g\n",x[1]-origo->x_gm);
     fprintf(filpek," 21\n%g\n",y[1]-origo->y_gm);
     }

   return(0);
 }

/*******************************************************/
/*!******************************************************/

 static short     dxfoar(
        FILE     *filpek,
        DBArc    *arcpek,
        DBSeg    *seg,
        DBVector *origo)

/*      Writes an arc on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           arcpek = Pekare till arcpost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int    k,korg;
   double scale;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,arcpek->hed_a.level) ||
        arcpek->hed_a.blank ) return(0);

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (actwin.vy.scrwin.xmax - actwin.vy.scrwin.xmin) *
            actwin.geo.psiz_x /
           (actwin.vy.modwin.xmax - actwin.vy.modwin.xmin);
/*
***Make polyline.
*/
   k = -1;
   WPplar(arcpek,seg,scale,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   korg = k;
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
/*
***Some part is visible. k = korg means that probably all was visible.
***Same number of vectors in polyline before/after clip.
***2D arc first.
*/
     if ( arcpek->ns_a == 0  &&  k == korg )
       {
       if ( ABS(arcpek->v2_a - arcpek->v1_a - 360.0) > TOL7 )
         fprintf(filpek,"  0\nARC\n");
       else
         fprintf(filpek,"  0\nCIRCLE\n");

       dxffnt(filpek,arcpek->fnt_a,arcpek->lgt_a);
       dxfniv(filpek,arcpek->hed_a.level);
       dxfpen(filpek,arcpek->hed_a.pen);
       dxfwdt(filpek,arcpek->wdt_a);

       fprintf(filpek," 10\n%g\n",arcpek->x_a - origo->x_gm);
       fprintf(filpek," 20\n%g\n",arcpek->y_a - origo->y_gm);
       fprintf(filpek," 40\n%g\n",arcpek->r_a);

       if ( ABS(arcpek->v2_a - arcpek->v1_a-360.0) > TOL7 )
         {
         fprintf(filpek," 50\n%g\n",arcpek->v1_a);
         fprintf(filpek," 51\n%g\n",arcpek->v2_a);
         }
       }
/*
***3D arc or partly visible.
*/
     else
       {
       dxfopl(filpek,&(arcpek->hed_a),k,x,y,a,origo);
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfocu(
        FILE     *filpek,
        DBCurve  *curpek,
        DBSeg    *seg,
        DBVector *origo)

/*      Writes a curve on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           curpek = Pekare till curpost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int    k;
   double scale;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,curpek->hed_cu.level) ||
        curpek->hed_cu.blank ) return(0);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (actwin.vy.scrwin.xmax - actwin.vy.scrwin.xmin) *
            actwin.geo.psiz_x /
           (actwin.vy.modwin.xmax - actwin.vy.modwin.xmin);
/*
***Make polyline.
*/
   k = -1;
   WPplcu(curpek,seg,scale,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(curpek->hed_cu),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfocs(
        FILE     *filpek,
        DBCsys   *csypek,
        DBTmat   *pmat,
        DBVector *origo)

/*      Writes coordinate system on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           csypek = Pekare till csypost.
 *           seg    = Pekare till segment.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;
   double lgdx,lgdy,lgd;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,csypek->hed_pl.level) ||
        csypek->hed_pl.blank ) return(0);
/*
***Axis length 20% of "plot window" size.
*/
   lgdx = actwin.vy.modwin.xmax - actwin.vy.modwin.xmin;
   lgdy = actwin.vy.modwin.ymax - actwin.vy.modwin.ymin; 

   if ( lgdx > lgdy ) lgd = lgdx;
   else               lgd = lgdy;
/*
***Make polyline.
*/
   k = -1;
   WPplcs(csypek,lgd,V3_CS_NORMAL,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(csypek->hed_pl),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfobp(
        FILE     *filpek,
        DBBplane *bplpek,
        DBVector *origo)

/*      Writes a B-plane on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           bplpek = Pekare till bplpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,bplpek->hed_bp.level) ||
        bplpek->hed_bp.blank ) return(0);
/*
***Make polyline.
*/
   k = -1;
   WPplbp(bplpek,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(bplpek->hed_bp),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfoms(
        FILE     *filpek,
        DBMesh   *mshpek,
        DBVector *origo)

/*      Writes a Mesh on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           mshpek = Pekare till mshpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)18/7/2004 J. Kjellander, Örebro university
 *
 *      2006-12-30 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   int    k;
   double size;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,mshpek->hed_m.level) ||
        mshpek->hed_m.blank ) return(0);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
***Point size = 0.75/scale.
*/
   size = 0.75/((actwin.vy.scrwin.xmax - actwin.vy.scrwin.xmin) *
                 actwin.geo.psiz_x /
                (actwin.vy.modwin.xmax - actwin.vy.modwin.xmin));
/*
***Make polyline.
*/
   k = -1;
   WPplms(mshpek,size,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(mshpek->hed_m),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfotx(
        FILE     *filpek,
        DBText   *txtpek,
        char      str[],
        DBVector *origo)

/*      Writes a text on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           txtpek = Pekare till txtpost.
 *           str    = Pekare till sträng.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1996-06-27 7-bitars till PC, J.Kjellander
 *      1998-10-01 3D-text, J.Kjellander
 *      2006-12-30 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   int   k,korg;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,txtpek->hed_tx.level) ||
        txtpek->hed_tx.blank ) return(0);
/*
***Make polyline.
*/
   k = -1;
   WPpltx(txtpek,(unsigned char *)str,&k,x,y,z,a);
/*
***Project to the view of the window. WPpply() can
***not be used since text has two projection modes (TPMODE).
*/
   WPprtx(&actwin,txtpek,k+1,x,y,z);
/*
***Clip.
*/
   korg = k;
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
/*
***Some part is visible. k = korg means that probably all was visible.
***Same number of vectors in polyline before/after clip.
*/
     if ( k == korg )
       {
/*
***All visible.
*/
       fprintf(filpek,"  0\nTEXT\n");

       dxfniv(filpek,txtpek->hed_tx.level);
       dxfpen(filpek,txtpek->hed_tx.pen);

       fprintf(filpek," 10\n%g\n",txtpek->crd_tx.x_gm - origo->x_gm);
       fprintf(filpek," 20\n%g\n",txtpek->crd_tx.y_gm - origo->y_gm);
       fprintf(filpek," 40\n%g\n",txtpek->h_tx);
       fprintf(filpek," 41\n%g\n",txtpek->b_tx/57.0);
       fprintf(filpek,"  1\n%s\n",str);
/*
***Angle and slant.
*/
       fprintf(filpek," 50\n%g\n",txtpek->v_tx);
       fprintf(filpek," 51\n%g\n",txtpek->l_tx*0.45);
       }
/*
***Only partly visible.
*/
     else
       {
       dxfopl(filpek,&(txtpek->hed_tx),k,x,y,a,origo);
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfold(
        FILE     *filpek,
        DBLdim   *ldmptr,
        DBCsys   *csyptr,
        DBVector *origo)

/*      Writes a linear dimension on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           ldmptr = Pekare till ldmpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,ldmptr->hed_ld.level) ||
        ldmptr->hed_ld.blank ) return(0);
/*
***Make polyline.
*/
   k = -1;
   WPplld(ldmptr,csyptr,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(ldmptr->hed_ld),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfocd(
        FILE     *filpek,
        DBCdim   *cdmpek,
        DBCsys   *csyptr,
        DBVector *origo)

/*      Writes diameter dimension on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           cdmpek = Pekare till cdmpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,cdmpek->hed_cd.level) ||
        cdmpek->hed_cd.blank ) return(0);
/*
***Make polyline.
*/
   k = -1;
   WPplcd(cdmpek,csyptr,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(cdmpek->hed_cd),k,x,y,a,origo);
     }
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxford(
        FILE     *filpek,
        DBRdim   *rdmpek,
        DBCsys   *csyptr,
        DBVector *origo)

/*      Writes radius dimension on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           rdmpek = Pekare till rdmpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int k;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,rdmpek->hed_rd.level) ||
        rdmpek->hed_rd.blank ) return(0);
/*
***Make polyline.
*/
   k = -1;
   WPplrd(rdmpek,csyptr,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(rdmpek->hed_rd),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfoad(
        FILE     *filpek,
        DBAdim   *admpek,
        DBCsys   *csyptr,
        DBVector *origo)

/*      Writes angular dimension on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           admpek = Pekare till admpost.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int    k;
   double scale;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,admpek->hed_ad.level) ||
        admpek->hed_ad.blank ) return(0);
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (actwin.vy.scrwin.xmax - actwin.vy.scrwin.xmin) *
            actwin.geo.psiz_x /
           (actwin.vy.modwin.xmax - actwin.vy.modwin.xmin);
/*
***Make polyline.
*/
   k = -1;
   WPplad(admpek,csyptr,scale,&k,x,y,z,a);
/*
***Project.
*/
   WPpply(&actwin,k,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&actwin.vy.modwin,(short)-1,&k,x,y,a) )
     {
     dxfopl(filpek,&(admpek->hed_ad),k,x,y,a,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfoxh(
        FILE     *filpek,
        DBHatch  *xhtpek,
        DBfloat   crd[],
        DBVector *origo)

/*      Writes a hatch on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           xhtpek = Pekare till xhtpost.
 *           crd    = Pekare till koordinater.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int    i,n;
   DBLine lin;

/*
***Visible ?
*/
   if ( !WPnivt(actwin.nivtab,xhtpek->hed_xh.level) ||
        xhtpek->hed_xh.blank ) return(0);
/*
***Write each hatch line as a DXF LINE entity.
*/
   i = 0;
   n = 4*xhtpek->nlin_xh;

   lin.hed_l.blank = xhtpek->hed_xh.blank;
   lin.hed_l.level = xhtpek->hed_xh.level;
   lin.hed_l.pen   = xhtpek->hed_xh.pen;
   lin.fnt_l       = xhtpek->fnt_xh;
   lin.lgt_l       = xhtpek->lgt_xh;
   lin.crd1_l.z_gm = 0.0;
   lin.crd2_l.z_gm = 0.0;

   while (i < n)
     {
     lin.crd1_l.x_gm = crd[i++];
     lin.crd1_l.y_gm = crd[i++];

     lin.crd2_l.x_gm = crd[i++];
     lin.crd2_l.y_gm = crd[i++];

     dxfoli(filpek,&lin,origo);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     dxfopl(
        FILE     *filpek,
        DBHeader *hedpek,
        int       n,
        double    x[],
        double    y[],
        char      a[],
        DBVector *origo)

/*      Writes a polyline on DXF-format.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           hedpek = Pekare till headerpost.
 *           n      = Antal vektorer.
 *           x,y    = Koordinater.
 *           a      = Vektorstatus.
 *           origo  = Nollpunkt.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 *      1996-06-25 Bug 66-grupp mm, J.Kjellander
 *      2006-12-30 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   int i=0;

/*
***POLYLINE.
*/
start:
   fprintf(filpek,"  0\nPOLYLINE\n");
   dxfniv(filpek,hedpek->level);
   dxfpen(filpek,hedpek->pen);
   fprintf(filpek," 66\n1\n");
/*
***VERTEX. Även varje VERTEX skall ha en layer-post.
*/
   do
     {
     fprintf(filpek,"  0\nVERTEX\n");
     dxfniv(filpek,hedpek->level);
     fprintf(filpek," 10\n%g\n",x[i] - origo->x_gm);
     fprintf(filpek," 20\n%g\n",y[i] - origo->y_gm);
     ++i;
     if (  i < n  &&  (a[i] & VISIBLE) != VISIBLE )
       {
       fprintf(filpek,"  0\nSEQEND\n");
       goto start;
       }
     } while ( i <= n );

   fprintf(filpek,"  0\nSEQEND\n");

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short dxfniv(
        FILE *filpek,
        short niv)

/*      Writes level (LAYER) record.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           niv    = Nivå.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   fprintf(filpek,"  8\nLAY%d\n",niv);
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short dxfpen(
        FILE *filpek,
        short pen)

/*      Writes pen record.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           pen    = penna.
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 {
   fprintf(filpek," 62\n%d\n",pen);
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short  dxfwdt(
        FILE  *filpek,
        double wdt)

/*      Writes line width record.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           wdt    = Bredd 
 *
 *      FV:  0 => Ok
 *
 *      (c) microform ab 1999-06-14 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( wdt > 0.0 ) fprintf(filpek," 39\n%f\n",wdt);
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   dxffnt(
        FILE   *filpek,
        short   typ,
        DBfloat lgt)

/*      Writes line type record.
 *
 *      In:  filpek = Pekar på öppen DXF-fil.
 *           typ    = 0, 1 eller 2.
 *           lgt    = Ev. strecklängd.
 *
 *      FV:  0 => Ok
 *
 *      (C)microform ab 1996-07-04 J. Kjellander
 *
 ******************************************************!*/

 {
#define SNR_ROBUST 4141         /*  Robusts serienummer */

/*
***Vilken linjetyp är det ?
*/
   switch ( typ )
     {
/*
***Streckad. (Specialare för Robust i Nykroppa)
*/
     case 1:
     if ( sydata.sernr == SNR_ROBUST )
       {
       if      ( lgt < 5.0 )  fprintf(filpek,"  6\nBORDER\n");
       else if ( lgt <= 7.5 ) fprintf(filpek,"  6\nBORDER2\n");
       else                   fprintf(filpek,"  6\nBORDERX2\n");
       }
     else fprintf(filpek,"  6\nDSH%g\n",lgt);
     break;
/*
***Streck-prickad. (Specialare för Robust i Nykroppa)
*/
     case 2:
     if ( sydata.sernr == SNR_ROBUST )
       {
       if      ( lgt < 5.0 )  fprintf(filpek,"  6\nPHANTOM\n");
       else if ( lgt <= 7.5 ) fprintf(filpek,"  6\nPHANTOM2\n");
       else                   fprintf(filpek,"  6\nPHANTOMX2\n");
       }
     else fprintf(filpek,"  6\nCEN%g\n",lgt);
     break;

     default:
     break;
     }

   return(0);
 }

/********************************************************/
