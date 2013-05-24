/**********************************************************************
*
*    wpsurf.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPdrsu();    Draw surface
*    WPdlsu();    Delete surface
*    WPplsu();    Create 3D polyline
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

extern short actpen;

static short drawsu(WPGWIN *gwinpt, DBSurf *surpek, DBSeg *sptarr[],
                    DBptr la, bool draw);
static short plyfac(GMSUR *surpek, int *n, double x[], double y[],
             double z[], char a[]);

/*!******************************************************/

        short   WPdrsu(
        DBSurf *surpek,
        DBSeg  *sptarr[],
        DBptr   la,
        DBint   win_id)

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
         if ( WPnivt(gwinpt,surpek->hed_su.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( surpek->hed_su.pen != actpen ) WPspen(surpek->hed_su.pen);
/*
***Sen är det bara att rita.
*/
           drawsu(gwinpt,surpek,sptarr,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPdlsu(
        DBSurf *surpek,
        DBSeg  *sptarr[],
        DBptr   la,
        DBint   win_id)

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
           if ( !WPnivt(gwinpt,surpek->hed_su.level)  ||
                               surpek->hed_su.blank) return(0);
           drawsu(gwinpt,surpek,sptarr,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawsu(
        WPGWIN *gwinpt,
        DBSurf *surpek,
        DBSeg  *sptarr[],
        DBptr   la,
        bool    draw)

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
   WPplsu(surpek,sptarr,scale,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Clip and draw.
*/
   if ( WPcply(gwinpt,-1,&k,x,y,a) )
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

        short WPplsu(
        DBSurf *surpek,
        DBSeg  *sptarr[],
        double  scale,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Bygger en GMSUR i form av en polyline.
 *      Resultatet lagras i globala arrayer x,y,z och a.
 *
 *      In: surpek =  Pekare till yt-structure.
 *          sptarr =  Array med pekare till grafiska segment.
 *
 *      Ut: n      =  Offset till sista vektorn i polylinjen
 *
 *      FV: 0 => Ok.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 *      5/12/94  kurvlängd=0, J. Kjellander
 *      9/1/96   FAC_SUR, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBCurve cur;
    double  cn;

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
***använda den som indata till gpplcu().
*/
    cur.hed_cu.type = CURTYP;
    cur.plank_cu    = FALSE;
/*
***Vi börjar med den kurvnoggrannhet som användaren begärt, men
***om det blir för många vektorer minskar vi succesivt.
***För att kunna återställa curnog sparar vi det ursprungliga 
***värdet här. För att inte gpplcu() skall perspektivtransformera
***hela polylinjen vi varje anrop stänger vi tillfälligt av och
***slår på först på slutet igen.
*/
start:
    *n = -1;
/*
***Border 1. V=0.
*/
    cur.fnt_cu = 0;
    cur.lgt_cu = 0;
    cur.al_cu  = 0.0;

    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[0];
    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[0],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Border 2. U=1.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[1];

    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[1],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Border 3. V=1.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[2];

    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[2],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Border 4. U=0.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[3];

    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[3],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Mid curve 1. U=constant.
*/
    cur.al_cu  = 0.0;
    cur.fnt_cu = 1;
    cur.lgt_cu = surpek->lgt_su;
    cur.ns_cu  = cur.nsgr_cu = surpek->ngseg_su[4];

    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[4],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Mid curve 2. V=constant.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[5];

    if ( cur.ns_cu > 0 )
      {
      status = WPplcu(&cur,sptarr[5],scale,n,x,y,z,a);
      if ( status == -1 )
        {
        cn /= 2.0;
        if ( cn < 0.1 ) goto end;
        else                goto start;
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
