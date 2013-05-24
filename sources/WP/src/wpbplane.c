/**********************************************************************
*
*    wpbplane.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrbp();    Draw B_plane
*    WPdlbp();    Erase B_plane
*    WPplbp();    Create B_plane polyline
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

extern int actpen;

static short drawbp(WPGWIN *gwinpt, DBBplane *bplpek, DBptr la, bool draw);

/*!******************************************************/

        short WPdrbp(
        DBBplane  *bplpek,
        DBptr   la,
        DBint   win_id)

/*      Ritar ett B-plan.
 *
 *      In: bplpek => Pekare till B-plan-post.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/2/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om det är blankat är det enkelt.
*/
   if ( bplpek->hed_bp.blank) return(0);
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
***Ja, ligger B-planet på en nivå som är tänd i detta fönster ?
*/
         if ( WPnivt(gwinpt->nivtab,bplpek->hed_bp.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( bplpek->hed_bp.pen != actpen ) WPspen(bplpek->hed_bp.pen);
/*
***Sen är det bara att rita.
*/
           if ( bplpek->wdt_bp != 0.0 ) WPswdt(gwinpt->id.w_id,bplpek->wdt_bp);
           drawbp(gwinpt,bplpek,la,TRUE);
           if ( bplpek->wdt_bp != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPdlbp(
        DBBplane  *bplpek,
        DBptr   la,
        DBint   win_id)

/*      Suddar ett B-plan.
 *
 *      In: bplpek => Pekare till B-plan-post.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/2/95 J. Kjellander
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
         if ( WPfobj(gwinpt,la,BPLTYP,&typ) )
           {
           WPdobj(gwinpt,FALSE);
           WProbj(gwinpt);
           }
/*
***Om det nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !WPnivt(gwinpt->nivtab,bplpek->hed_bp.level)  ||
                               bplpek->hed_bp.blank) return(0);
           if ( bplpek->wdt_bp != 0.0 ) WPswdt(gwinpt->id.w_id,bplpek->wdt_bp);
           drawbp(gwinpt,bplpek,la,FALSE);
           if ( bplpek->wdt_bp != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawbp(
        WPGWIN *gwinpt,
        DBBplane  *bplpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar ett B-plan i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          bplpek => Pekare till B-plan-post.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/2/95 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    k;

/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   WPplbp(bplpek,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( WPcply(&gwinpt->vy.modwin,-1,&k,x,y,a) )
     {
     if ( draw  &&  bplpek->hed_bp.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,BPLTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short     WPplbp(
        DBBplane *bplpek,
        int      *n,
        double    x[],
        double    y[],
        double    z[],
        char      a[])

/*      Bygger ett B-plan i form av en polylinje.
 *
 *      In: gwinpt => Pekare till fönster.
 *          bplpek => Pekare till B-plan-structure
 *          n+1    => Offset till polylinjens startposition
 *
 *      Ut: *n     => Offset till polylinjens slutposition
 *           x,y,a => Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)2006-12-16 J. Kjellander
 *
 ******************************************************!*/

  {
    int    k;

/*
***Initiering.
*/
   k = *n + 1;
/*
***The B-plane wireframe is just 3 or 4 lines.
*/
   x[k]   = bplpek->crd1_bp.x_gm;
   y[k]   = bplpek->crd1_bp.y_gm;
   z[k]   = bplpek->crd1_bp.z_gm;
   a[k++] = 0;

   x[k]   = bplpek->crd2_bp.x_gm;
   y[k]   = bplpek->crd2_bp.y_gm;
   z[k]   = bplpek->crd2_bp.z_gm;
   a[k++] = VISIBLE;

   if ( bplpek->crd3_bp.x_gm != bplpek->crd2_bp.x_gm ||
        bplpek->crd3_bp.y_gm != bplpek->crd2_bp.y_gm ||
        bplpek->crd3_bp.z_gm != bplpek->crd2_bp.z_gm )
     {
     x[k]   = bplpek->crd3_bp.x_gm;
     y[k]   = bplpek->crd3_bp.y_gm;
     z[k]   = bplpek->crd3_bp.z_gm;
     a[k++] = VISIBLE;
     }

   x[k]   = bplpek->crd4_bp.x_gm;
   y[k]   = bplpek->crd4_bp.y_gm;
   z[k]   = bplpek->crd4_bp.z_gm;
   a[k++] = VISIBLE;

   x[k]   = bplpek->crd1_bp.x_gm;
   y[k]   = bplpek->crd1_bp.y_gm;
   z[k]   = bplpek->crd1_bp.z_gm;
   a[k] = VISIBLE;
/*
***End.
*/
  *n = k;

   return(0);
  }

/********************************************************/
