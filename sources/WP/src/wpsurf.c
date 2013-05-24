/**********************************************************************
*
*    wpsurf.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrsu();     Draw surface
*    WPdlsu();     Delete surface
*    WPplsu();     Create 3D polyline
*    WPuvstepsu(); Calculate u/v tesselation steps
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
#include <math.h>

extern int actpen;

static short drawsu(WPGWIN *gwinpt, DBSurf *surpek, DBSegarr *pborder,
                    DBSegarr *piso, DBptr la, bool draw);
static short plyfac(GMSUR *surpek, int *n, double x[], double y[],
             double z[], char a[]);

/*!******************************************************/

        short     WPdrsu(
        DBSurf   *surpek,
        DBSegarr *pborder,
        DBSegarr *piso,
        DBptr     la,
        DBint     win_id)

/*      Display a surface.
 *
 *      In: surpek => Pekare till sur-post.
 *          sptarr => Pekare till grafisk representation.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 22/1/95 J. Kjellander
 *
 *       2007-01-08 pborder, piso,   Sören L.
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om ytan är blankad är det enkelt.
*/
   if ( surpek->hed_su.blank) return(0);
/*
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi rita i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, ligger ytan på en nivå som är tänd i detta fönster ?
*/
         if ( WPnivt(gwinpt->nivtab,surpek->hed_su.level) )
           {
/*
***Ja. Kolla att rätt färg och linjebredd är inställd.
*/
           if ( surpek->hed_su.pen != actpen ) WPspen(surpek->hed_su.pen);
           if ( surpek->wdt_su != 0.0 ) WPswdt(gwinpt->id.w_id,surpek->wdt_su);
/*
***Sen är det bara att rita.
*/
           drawsu(gwinpt,surpek,pborder,piso,la,TRUE);
           if ( surpek->wdt_su != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short     WPdlsu(
        DBSurf   *surpek,
        DBSegarr *pborder,
        DBSegarr *piso,
        DBptr     la,
        DBint     win_id)

/*      Erase a surface.
 *
 *      In: surpek => Pekare till sur-post.
 *          sptarr => Pekare till grafisk representation.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 21/1/95 J. Kjellander
 *
 *      2007-01-08 pborder, piso,   Sören L.
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi sudda i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja. Om den finns i DF kan vi sudda snabbt.
*/
         if ( WPfobj(gwinpt,la,SURTYP,&typ) )
           {
           WPdobj(gwinpt,FALSE);
           WProbj(gwinpt);
           }
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !WPnivt(gwinpt->nivtab,surpek->hed_su.level)  ||
                               surpek->hed_su.blank) return(0);
           if ( surpek->wdt_su != 0.0 ) WPswdt(gwinpt->id.w_id,surpek->wdt_su);
           drawsu(gwinpt,surpek,pborder,piso,la,FALSE);
           if ( surpek->wdt_su != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short     drawsu(
        WPGWIN   *gwinpt,
        DBSurf   *surpek,
        DBSegarr *pborder,
        DBSegarr *piso,
        DBptr     la,
        bool      draw)

/*      Ritar/suddar en yta i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          surpek => Pekare till sur-post.
 *          sptarr => Pekare till DBSeg-arrayer.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 21/1/95 J. Kjellander
 *
 *      2007-01-08 pborder, piso,   Sören L.
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;
   char   a[PLYMXV];
   int    k;

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
            gwinpt->geo.psiz_x /
           (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
/*
***Create polyline.
*/
   k = -1;
   WPplsu(surpek,pborder,piso,scale,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Clip and draw.
*/
   if ( WPcply(&gwinpt->vy.modwin,-1,&k,x,y,a) )
     {
     if ( draw  &&  surpek->hed_su.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,SURTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short     WPplsu(
        DBSurf   *surpek,
        DBSegarr *pborder,
        DBSegarr *piso,
        double    scale,
        int      *n,
        double    x[],
        double    y[],
        double    z[],
        char      a[])

/*      Makes the polyline of a surface graphical
 *      wireframe representation.
 *
 *      In: surpek   = C ptr to DBSurf.
 *          pborder  = C ptr to outer loop (border)
 *          piso     = C ptr to mid curves
 *
 *      Out: n       = Offset to last vector in polyline.
 *           x,y,z,a = Polyline.
 *
 *      Return: 0 = Ok.
 *            < 0 = Error fron WPplcu().
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 *      5/12/94  kurvlängd=0, J. Kjellander
 *      9/1/96   FAC_SUR, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *      2007-01-08 pborder, piso, Sören L.
 *
 ******************************************************!*/

  {
    short   status;
    DBCurve cur;
    double  cn;
    DBint   bcount,icount;

/*
***Get current value of curve accuracy.
*/
   WPget_cacc(&cn);
/*
***Tills vidare har vi bara en font.
*/
   if ( surpek->fnt_su != 0 ) return(0);
/*
***Facettytor har annan grafisk representation.
*/
   if ( surpek->typ_su == FAC_SUR )
     return( plyfac(surpek,n,x,y,z,a));
/*
***Initiera div data i en kurv-post så att vi kan 
***använda den som indata till WPplcu().
*/
   cur.hed_cu.type = CURTYP;
   cur.plank_cu    = FALSE;
/*
***Vi börjar med den kurvnoggrannhet som användaren begärt, men
***om det blir för många vektorer minskar vi succesivt.
*/
start:
   *n = -1;
/*
***Border curves.
*/
   cur.fnt_cu = 0;
   cur.lgt_cu = 0;

   for ( bcount=0; bcount<surpek->ngrwborder_su; bcount++ ) 
     {
     cur.al_cu  = 0.0;
     cur.ns_cu = cur.nsgr_cu = pborder[bcount].nseg;
     if ( cur.ns_cu > 0 )
       {
       status = WPplcu(&cur, pborder[bcount].segptr_c,scale,n,x,y,z,a);
       if ( status == -1 )
         {
         cn /= 2.0;
         if ( cn < 0.1 ) goto end;
         else                goto start;
         }
       }
     }
/*
***iso curves.
*/
   cur.fnt_cu = 1;
   cur.lgt_cu = surpek->lgt_su;

   for ( icount=0; icount< surpek->ngrwiso_su; icount++ ) 
     {
     cur.al_cu  = 0.0;
     cur.ns_cu = cur.nsgr_cu = piso[icount].nseg;
     if ( cur.ns_cu > 0 )
       {
       status = WPplcu(&cur, piso[icount].segptr_c,scale,n,x,y,z,a);
       if ( status == -1 )
         {
         cn /= 2.0;
         if ( cn < 0.1 ) goto end;
         else                goto start;
         }
       }
     }
/*
***The end.
*/
end:

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short plyfac(
        DBSurf *surpek,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Create 3D polyline for facet surface.
 *
 *      In: surpek =  Pekare till yt-structure.
 *          sptarr =  Array med pekare till grafiska segment.
 *
 *      Ut: n      =  Offset till sista vektorn i polylinjen
 *
 *      FV: 0 => Ok.
 *
 *      (C)microform ab 1996-01-28 J. Kjellander
 *
 *      2006-12-29 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   int       k,i,j,nvec,ustop,vstop;
   DBPatch  *patpek,*toppat;
   DBPatchF *facpat;

/*
***Hämta ytans patchar från GM.
*/
   DBread_patches(surpek,&patpek);
   toppat = patpek;
/*
***Hur många positioner kan det bli som mest ? (6/patch)
*/
   nvec = surpek->nu_su*surpek->nv_su*6;
/*
***För att detta skall få plats med tanke på PLYMXV krävs det lite
***kontroller. Om det inte får plats ritar vi inte hela ytan.
*/
   if ( nvec < PLYMXV )
     {
     ustop = surpek->nu_su;
     vstop = surpek->nv_su;
     }
   else
     {
     if ( surpek->nu_su == 1 )
       {
       ustop = 1;
       vstop = (int)HEL((double)PLYMXV/(double)nvec*(double)surpek->nv_su);
       }
     else if ( surpek->nv_su == 1 )
       {
       ustop = (int)HEL((double)PLYMXV/(double)nvec*(double)surpek->nu_su);
       vstop = 1;
       }
     else
       {
       ustop = 2;
       vstop = 2;
       }
     }
/*
***Loopa igenom alla patchar och lagra ränder i x,y,z.
*/
   k = 0;

   for ( i=0; i<ustop; ++i )
     {
     for ( j=0; j<vstop; ++j )
       {
       if ( toppat->styp_pat == FAC_PAT )
         {
         facpat = (GMPATF *)toppat->spek_c;
/*
***Show all 4 outer boundaries.
*/
         x[k] = facpat->p1.x_gm;
         y[k] = facpat->p1.y_gm;
         z[k] = facpat->p1.z_gm;
         a[k++] = 0;

         x[k] = facpat->p2.x_gm;
         y[k] = facpat->p2.y_gm;
         z[k] = facpat->p2.z_gm;
         a[k++] = VISIBLE;

         x[k] = facpat->p3.x_gm;
         y[k] = facpat->p3.y_gm;
         z[k] = facpat->p3.z_gm;
         a[k++] = VISIBLE;

         x[k] = facpat->p4.x_gm;
         y[k] = facpat->p4.y_gm;
         z[k] = facpat->p4.z_gm;
         a[k++] = VISIBLE;

         x[k] = facpat->p1.x_gm;
         y[k] = facpat->p1.y_gm;
         z[k] = facpat->p1.z_gm;
         a[k++] = VISIBLE;
/*
***If the patch is 2 triangles show diagonal.
*/
         if ( facpat->triangles )
           {
           x[k] = facpat->p3.x_gm;
           y[k] = facpat->p3.y_gm;
           z[k] = facpat->p3.z_gm;
           a[k++] = VISIBLE;
           }
         }
       ++toppat;
       }
     }
   DBfree_patches(surpek,patpek);
/*
***Hur många vektorer blev det ?
*/
  *n = k-1;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short     WPuvstepsu(
        DBSurf   *surpek,
        DBPatch  *patpek,
        DBfloat   uscale,
        DBfloat   vscale)

/*      Calculate u/v tesselation steps.
 *
 *      It is possible to develop this function to evaluate u and v
 *      values separately by samples on the surface instead of using 
 *      the cone angles. Doing so can reduce the number of u/v steps
 *      needed in the opengl rendering.
 *
 *      In:  surpek => pointer to surface structure.
 *           patpek => pointer to patches
 *           uscale => 1 Geo patch / graph. u-unit (normally = 1)
 *           vscale => 1 Geo patch / graph. v-unit (normally = 1)
 *
 *      Out: Sets nustep_su and nvstep_su i the DBSurf.
 *
 *      (C) 2007-01-04 Sören Larsson, Örebro university
 *
 *      2007-02-12 Flat/trimmed surf. J.Kjellander
 *
 ******************************************************!*/

{
   DBfloat   nustep_su,nvstep_su;
   DBfloat   this_nustep_su,this_nvstep_su;
   DBint     i,j;
   DBPatch  *pthispat;
   BBOX      box_pat;
   DBfloat   this_pat_size;
   DBfloat   cone_angle;

/*
***Initializations.
*/
   nustep_su = 0.5;
   nvstep_su = 0.5;
/*
*** Loop through patches to get the 'worst case'
*/
   for ( i=0; i<surpek->nu_su; ++i )
     {
     for ( j=0; j<surpek->nv_su; ++j )
       {
       pthispat = patpek + i*surpek->nv_su + j;

       box_pat = pthispat->box_pat;

       this_pat_size = sqrt((box_pat.xmax-box_pat.xmin)*(box_pat.xmax-box_pat.xmin)+
                       (box_pat.ymax-box_pat.ymin)*(box_pat.ymax-box_pat.ymin)+
                       (box_pat.zmax-box_pat.zmin)*(box_pat.zmax-box_pat.zmin));
/*
***If the cone angle of this patch is defined, use it.
***If not set at all, it to 45 degrees,
*/
       if ( pthispat->cone_pat.flag == 1 ) cone_angle = pthispat->cone_pat.ang;
       else                                cone_angle = 45;
/*
***If the surface is trimmed, increase the cone angle. A big surface
***needs more steps because of the trimcurves even if the cone angle
***is small so for trimmed surfaces, always use a cone_angle of 5*20
***or bigger.
*/
       if ( surpek->ntrim_su > 0 )
         {
         if ( cone_angle < 20.0 ) cone_angle = 20.0;
         cone_angle *= 5;
         }
/*
***Calculate nustep/nvstep for this patch.
*/
       this_nustep_su = this_pat_size * cone_angle /
                        (pthispat->ue_pat - pthispat->us_pat);
       this_nvstep_su = this_pat_size * cone_angle /
                        (pthispat->ve_pat - pthispat->vs_pat);
/*
***Is it the biggest so far ?
*/
       if ( this_nustep_su > nustep_su ) nustep_su = this_nustep_su;
       if ( this_nvstep_su > nvstep_su ) nvstep_su = this_nvstep_su;
       }
     }
/*
***Save result.
*/
   surpek->nustep_su = nustep_su * uscale;
   surpek->nvstep_su = nvstep_su * vscale;
/*
***The end.
*/
   return(0);
}

/********************************************************/
