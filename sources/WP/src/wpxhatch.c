/**********************************************************************
*
*    wpxhatch.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrxh();    Draw xhatch
*    WPdlxh();    Delete xhatch
*    WPplxh();    Create 3D polyline
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
#include "../../GE/include/GE.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

extern int actpen;

static short drawxh(WPGWIN *gwinpt, DBHatch *xhtpek, DBfloat crdvek[],
                    DBCsys *csyptr, DBptr la, bool draw);

/*!******************************************************/

        short    WPdrxh(
        DBHatch *xhtpek,
        DBfloat  crdvek[],
        DBCsys  *csyptr,
        DBptr    la,
        DBint    win_id)

/*      Display a hatch.
 *
 *      In: xhtpek => C ptr to hatch data.
 *          crdvek => C ptr to coordinates.
 *          la     => Hatch DB address.
 *          win_id => WPGWIN id or GWIN_ALL.
 *
 *      Return: 0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
 *
 *      2007-09-01 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Don't display if the hatch is blanked.
*/
   if ( xhtpek->hed_xh.blank) return(0);
/*
***Loop through all WPGWIN windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Hit ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Yes, is this level visible ?
*/
         if ( WPnivt(gwinpt->nivtab,xhtpek->hed_xh.level) )
           {
/*
***Yes, set color and width.
*/
           if ( xhtpek->hed_xh.pen != actpen ) WPspen(xhtpek->hed_xh.pen);
           if ( xhtpek->wdt_xh != 0.0 ) WPswdt(gwinpt->id.w_id,xhtpek->wdt_xh);
/*
***Display.
*/
           drawxh(gwinpt,xhtpek,crdvek,csyptr,la,TRUE);
           if ( xhtpek->wdt_xh != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPdlxh(
        DBHatch *xhtpek,
        DBfloat  crdvek[],
        DBCsys  *csyptr,
        DBptr    la,
        DBint    win_id)

/*      Erase (undisplay) a hatch.
 *
 *      In: xhtpek => C ptr do hatch data.
 *          crdvek => C ptr to coordinates.
 *          la     => Hatch DB address.
 *          win_id => WPGWIN id or GWIN_ALL.
 *
 *      Return: 0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
 *
 *      2007-09-01 WIDTH, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Hit ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Remove object from DF.
*/
         if ( WPfobj(gwinpt,la,XHTTYP,&typ) ) WProbj(gwinpt);
/*
***If blanked we are now finished.
*/
         if ( !WPnivt(gwinpt->nivtab,xhtpek->hed_xh.level)  ||
                               xhtpek->hed_xh.blank) return(0);
/*
***Remove from display.
*/
         if ( xhtpek->wdt_xh != 0.0 ) WPswdt(gwinpt->id.w_id,xhtpek->wdt_xh);
         drawxh(gwinpt,xhtpek,crdvek,csyptr,la,FALSE);
         if ( xhtpek->wdt_xh != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short    drawxh(
        WPGWIN  *gwinpt,
        DBHatch *xhtpek,
        DBfloat  crdvek[],
        DBCsys  *csyptr,
        DBptr    la,
        bool     draw)

/*      Display/erase hatch in a WPGWIN and insert
 *      object into DF.
 *
 *      In: gwinpt => C ptr to WPGWIN.
 *          xhtpek => C ptr to hatch data.
 *          crdvek => C ptr to coordinates.
 *          la     => Hatch DB address.
 *          draw   => TRUE = Display, FALSE = Erase
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 26/1/95 J. Kjellander
 *
 *      2006-12-17 WPplli(), J.Kjellander
 *      2007-10-06 3D, J.Kjellander
 *
 ******************************************************!*/

 {
   int    n;
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];

/*
***Create polyline.
*/
   n = -1;
   WPplxh(xhtpek,crdvek,csyptr,&n,x,y,z,a);
   WPpply(gwinpt,n,x,y,z);
/*
***Clip.
*/
   if ( WPcply(&gwinpt->vy.modwin,-1,&n,x,y,a) )
     {
     if ( draw  &&  xhtpek->hed_xh.hit )
       {
       if ( WPsply(gwinpt,n,x,y,a,la,XHTTYP) ) WPdobj(gwinpt,TRUE);
       }
     else WPdply(gwinpt,n,x,y,a,draw);
     }
/*
***The end.
*/
   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short    WPplxh(
        DBHatch *xhtptr,
        DBfloat  crdvek[],
        DBCsys  *csyptr,
        int     *n,
        double   x[],
        double   y[],
        double   z[],
        char     a[])

/*      Creates the graphical 3D polyline representation 
 *      for a xhatch.
 *
 *      In:  xhtptr  = C-ptr to DBHatch.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-08 J.Kjellander
 *
 *      2007-09-30 3D,J.Kjellander
 *
 ******************************************************!*/

 {
   int     i,k,nl;
   DBfloat xt,yt,zt;
   DBLine  lin;
   DBTmat  t;

/*
***Initializations.
*/
   k  = *n;
   i  =  0;
   nl =  4*xhtptr->nlin_xh;

   lin.fnt_l = xhtptr->fnt_xh;
   lin.lgt_l = xhtptr->lgt_xh;
   lin.crd1_l.z_gm = 0.0;
   lin.crd2_l.z_gm = 0.0;
/*
***Use WPplli() to create the polyline.
*/
   while ( i<nl )
     {
     lin.crd1_l.x_gm = crdvek[i++];
     lin.crd1_l.y_gm = crdvek[i++];
     lin.crd1_l.z_gm = 0.0;
     lin.crd2_l.x_gm = crdvek[i++];
     lin.crd2_l.y_gm = crdvek[i++];
     lin.crd2_l.z_gm = 0.0;
     WPplli(&lin,&k,x,y,z,a); 
     }
/*
***If needed, transform 2D polyline to XY-plane of 3D csys.
*/
    if ( xhtptr->pcsy_xh > 0 )
      {
      GEtform_inv(&csyptr->mat_pl,&t);

      for ( i=0; i<=k; i++ )
        {
        xt = t.g11 * x[i] +
             t.g12 * y[i] +
             t.g13 * z[i] +
             t.g14;
        yt = t.g21 * x[i] +
             t.g22 * y[i] +
             t.g23 * z[i] +
             t.g24;
        zt = t.g31 * x[i] +
             t.g32 * y[i] +
             t.g33 * z[i] +
             t.g34;

        x[i] = xt;
        y[i] = yt;
        z[i] = zt;
        }
      }
/*
***The end.
*/
   *n = k;

    return(0);
 }

/********************************************************/
