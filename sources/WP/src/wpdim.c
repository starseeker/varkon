/**********************************************************************
*
*    wpdim.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrdm();    Draw dimension
*    WPdldm();    Delete dimension
*    WPplld();    Create linear dim 3D polyline
*    WPplcd();    Create circular dim 3D polyline
*    WPplrd();    Create radius dim 3D polyline
*    WPplad();    Create angular dim 3D polyline
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
#include <math.h>

/*
***Orientation codes for linear and diameter dimensions.
*/
#define LDHORIZON  0         /* Linear horisontal */
#define LDVERTIC   1         /* Linear vertical */
#define LDPARALL   2         /* Linear parallell */
#define CDHORIZON  0         /* Diameter horisontal */
#define CDVERTIC   1         /* Diameter vertical */
#define CDPARALL   2         /* Diameter parallell */

extern int actpen;

static short drawdm(WPGWIN *gwinpt, DBAny *dimptr, DBCsys *csyptr, DBptr la, bool draw);

/*!******************************************************/

        short   WPdrdm(
        DBAny  *dimptr,
        DBCsys *csyptr,
        DBptr   la,
        DBint   win_id)

/*      Display linear, diameter, radius or angular dimension.
 *
 *      In: dimptr => C ptr to dim record.
 *          csyptr => C ptr to dim csys.
 *          la     => Dim address in DB.
 *          win_id => WPGWIN ID to display in or GWIN_ALL.
 *
 *      Return:  0 => Ok.
 *
 *      (C) microform ab 27/1/95 J. Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Don't display blanked entities.
*/
   if ( dimptr->hed_un.blank) return(0);
/*
***Loop through all WPGWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Is this window involved ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Yes, is the dim level visible in this window ?
*/
         if ( WPnivt(gwinpt->nivtab,dimptr->hed_un.level) )
           {
/*
***Yes,set color.
*/
           if ( dimptr->hed_un.pen != actpen ) WPspen(dimptr->hed_un.pen);
/*
***Display.
*/
           drawdm(gwinpt,dimptr,csyptr,la,TRUE);
           }
         }
       }
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPdldm(
        DBAny  *dimptr,
        DBCsys *csyptr,
        DBptr   la,
        DBint   win_id)

/*      Erase (undisplay) dimension.
 *
 *      In: dimptr => C ptr to dim record.
 *          csyptr => C ptr to dim csys.
 *          la     => Dim address in DB.
 *          win_id => WPGWIN ID to display in or GWIN_ALL.
 *
 *      Return:  0 => Ok.
 *
 *      (C) microform ab 27/1/95 J. Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN-windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Is this window involved ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Remove from DF.
*/
         if ( WPfobj(gwinpt,la,dimptr->hed_un.type,&typ) ) WProbj(gwinpt);
/*
***Remove from display.
*/
         if ( !WPnivt(gwinpt->nivtab,dimptr->hed_un.level)  ||
                               dimptr->hed_un.blank) return(0);
         drawdm(gwinpt,dimptr,csyptr,la,FALSE);
         }
       }
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawdm(
        WPGWIN *gwinpt,
        DBAny  *dimptr,
        DBCsys *csyptr,
        DBptr   la,
        bool    draw)

/*      Draw/erase dim in a WPGWIN and update DF.
 *
 *      In: gwinpt => C ptr to window.
 *          dimptr => C ptr to dim record.
 *          csyptr => C ptr to dim csys.
 *          la     => Dim address in DB.
 *          draw   => TRUE for draw, FALSE for erase.
 *
 *      (C)microform ab 27/1/95 J.Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
 *      2007-09-01 WIDTH, J.Kjellander
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale,w;
   char   a[PLYMXV];
   int    k;

/*
***Create graphical representation (polyline).
*/
   k = -1;

   switch ( dimptr->hed_un.type )
     {
     case LDMTYP:
     WPplld(&dimptr->ldm_un,csyptr,&k,x,y,z,a);
     WPpply(gwinpt,k,x,y,z);
     w = dimptr->ldm_un.wdt_ld;
     break;

     case CDMTYP:
     WPplcd(&dimptr->cdm_un,csyptr,&k,x,y,z,a);
     WPpply(gwinpt,k,x,y,z);
     w = dimptr->cdm_un.wdt_cd;
     break;

     case RDMTYP:
     WPplrd(&dimptr->rdm_un,csyptr,&k,x,y,z,a);
     WPpply(gwinpt,k,x,y,z);
     w = dimptr->rdm_un.wdt_rd;
     break;

     case ADMTYP:
     scale = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
              gwinpt->geo.psiz_x /
             (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
     WPplad(&dimptr->adm_un,csyptr,scale,&k,x,y,z,a);
     WPpply(gwinpt,k,x,y,z);
     w = dimptr->adm_un.wdt_ad;
     break;
    }
/*
***Set linewidth.
*/
   if ( w != 0.0 ) WPswdt(gwinpt->id.w_id,w);
/*
***Clip and display.
*/
   if ( WPcply(&gwinpt->vy.modwin,-1,&k,x,y,a) )
     {
     if ( draw  &&  dimptr->hed_un.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,dimptr->hed_un.type) )
         WPdobj(gwinpt,TRUE);
       else
         return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }
/*
***Reset linewidth.
*/
   if ( w != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
/*
***The end.
*/
   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short   WPplld(
        DBLdim *dimptr,
        DBCsys *csyptr,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      of a linear dimension.
 *
 *      In:  dimptr  = C-ptr to DBLdim.
 *           csyptr  = C-ptr to current csys.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 *      2007-90-17 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
    double fi,sinfi,cosfi;
    double a,b,d,e,d5,tt;
    double xp2,xp3,xp6,xp7,yp1,yp4,xt,yt,zt;
    double dy,dx,pa,radk;
    short  dimtyp,ndig;
    DBText txtrec;
    DBTmat t;

/*
***Initializations.
*/
    radk = PI/180.0;
    dimtyp = dimptr->dtyp_ld;           /* Type */
    ndig = dimptr->ndig_ld;             /* Decimals */
    d = dimptr->asiz_ld;                /* Arrow size */
    d5 = d/5;

    k = *n;
    x5 = dimptr->p3_ld.x_gm;            /* Text position */
    y5 = dimptr->p3_ld.y_gm; 
    x1 = dimptr->p1_ld.x_gm;            /* Start */
    y1 = dimptr->p1_ld.y_gm;
    x4 = dimptr->p2_ld.x_gm;            /* End */
    y4 = dimptr->p2_ld.y_gm;
    if ((x1 == x4) && (y1 == y4)) return(0);

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimptr->tsiz_ld;      /* Text size */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;
/*
***Calculate angle and x2,y2,x3,y3
*/
    switch (dimtyp)
      {
      case LDHORIZON:
              if ((y5 > y1) && (y5 > y4)) {
                   fi = 180.0;
                   if (x4 > x1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 0.0;
                   if (x4 < x1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }
              x2 = x1;
              x3 = x4;
              y2 = y3 = y5;
      break;

      case LDVERTIC:
              if ((x5 < x1) && (x5 < x4)) { 
                   fi = 270.0;
                   if (y4 > y1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 90.0;
                   if (y4 < y1) {  
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }
              x2 = x3 = x5;
              y2 = y1;
              y3 = y4;
      break;

      case LDPARALL:
              dy = x1 - x4; dx = y4 - y1;
              tt = SQRT(dx*dx + dy*dy);
              dx /= tt; dy /= tt;
              pa = dx*(x5 -x1) + dy*(y5 -y1);
              if (pa != 0.0) {
                   if (pa < 0.0) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                        dx = -dx; dy = -dy;
                        pa = -pa;
                   }
                   x2 = x1 + pa*dx; y2 = y1 + pa*dy;
                   x3 = x4 + x2 - x1; y3 = y4 + y2 - y1;
              } else {
                   x2 = x3 = y2 = y3 = 0;
              }

              dx = x3 - x2; dy = y3 - y2;
              if (dx > 0.0) 
                   if (dy > 0.0) 
                        fi = ATAN(dy/dx)/radk;
                   else if (dy < 0.0)
                        fi = 360.0 - ATAN(-(dy/dx))/radk;
                   else
                        fi = 0.0;
              else if (dx < 0.0)
                   if (dy > 0.0)
                        fi = 180.0 - ATAN(-(dy/dx))/radk;
                   else if (dy < 0.0)
                        fi = 180.0 + ATAN(dy/dx)/radk;
                   else
                        fi = 180.0;
              else
                   if (dy > 0.0) 
                        fi = 90.0;
                   else if (dy < 0.0) 
                        fi = 270.0;
                   else
                        fi = 0.0;             /* undefined */

      break;
      }


    a = SQRT((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
    b = SQRT((x5 - x2)*(x5 - x2) + (y5 - y2)*(y5 - y2));
    e = SQRT((x5 - x3)*(x5 - x3) + (y5 - y3)*(y5 - y3));
    yp1 = SQRT((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
    yp4 = SQRT((x4 - x3)*(x4 - x3) + (y4 - y3)*(y4 - y3));

    switch (dimtyp) {

         case LDHORIZON: {                     /* Horisontellt mått */
              if (fi == 0.0) {
                   if (y1 < y5)
                        yp1 = -yp1;
                   if (y4 < y5)
                        yp4 = -yp4;
              }
              break;
         }

         case LDVERTIC: {                      /* Vertikalt mått */
              if (fi == 90.0) {
                   if (x1 > x5)
                        yp1 = -yp1;
                   if (x4 > x5)
                        yp4 = -yp4;
              }
              break;
         }

         case LDPARALL: {                      /* Parallelt mått */
              break;
         }
    }

    sprintf(txtformat,"%%0.%df",ndig);
    sprintf(txt,txtformat,a);                   /* Bygg måtttext */
/*
***Av någon anledning har -strlen(txt)*txtrec.... slutat
***fungera i och med 1.11 på DIAB-maskinerna. Ett par extra
***parenteser löser problemet, fråga inte mig varför.
***DIAB!!!!
*/
    if ( (e > a) && (b < e) ) {               /* Left */

         xp2 = b;
         xp3 = e;
         xp7 = e + 2*d;
         if ((fi > 90.0) && (fi <= 270.0))
              xp6 = -(strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0);
         else
              xp6 = 0.0;
    } else if ((b > a) && (e < b)) {
                xp6 = -(b + 2*d);            /* Right */
                xp2 = -b; 
                xp3 = -e;
                if ((fi <= 90.0) || (fi > 270.0))
                     xp7 = strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0;
                else
                     xp7 = 0.0;
           } else {
                xp3 = xp7 = e;
                xp2 = xp6 = -b;          /* Between arrows */
           }
/*
***Make polyline.
*/
    x[++k] = xp2;      y[k] = yp1;    typ[k] = 0;
    x[++k] = xp2;      y[k] = -d5;    typ[k] = VISIBLE;
    x[++k] = xp3;      y[k] = yp4;    typ[k] = 0;
    x[++k] = xp3;      y[k] = -d5;    typ[k] = VISIBLE;
    x[++k] = xp6;      y[k] = 0.0;    typ[k] = 0;
    x[++k] = xp7;      y[k] = 0.0;    typ[k] = VISIBLE;

    if ( xp2*xp3 > 0.0 ) d = -d;

    x[++k] = xp2;      y[k] = 0.0;    typ[k] = 0;
    x[++k] = xp2 + d;  y[k] = d5;     typ[k] = VISIBLE;
    x[++k] = xp2 + d;  y[k] = -d5;    typ[k] = VISIBLE;
    x[++k] = xp2;      y[k] = 0.0;    typ[k] = VISIBLE;

    x[++k] = xp3;      y[k] = 0.0;    typ[k] = 0;
    x[++k] = xp3 - d;  y[k] = d5;     typ[k] = VISIBLE;
    x[++k] = xp3 - d;  y[k] = -d5;    typ[k] = VISIBLE;
    x[++k] = xp3;      y[k] = 0.0;    typ[k] = VISIBLE;

    x[++k] = 0;                       typ[k] = 0;

    if ((fi > 90.0) && (fi <= 270.0))
         y[ k ] = -2*d5;
    else
         y[ k ] = 2*d5;
/*
***Transform to local 2D csys.
*/
    if (fi == 0.0)                        /* cos=1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] += x5;
              y[ i ] += y5;
         }
    else if (fi == 90.0)                  /* cos=0 sin=1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 - y[ i ];
              y[ i ] = y5 + tt;
         }
    else if (fi == 180.0)                 /* cos=-1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] = x5 - x[ i ];
              y[ i ] = y5 - y[ i ];
         }
    else if (fi == 270.0)                 /* cos=0 sin=-1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + y[ i ];
              y[ i ] = y5 - tt;
         }
    else {
         sinfi = SIN(radk*fi);
         cosfi = COS(radk*fi);
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y5 + tt*sinfi + y[ i ]*cosfi;
         }
    }
/*
***All Z-coordinates so far are zero.
*/
   for ( i = *n+1; i <= k; ++i ) z[i] = 0.0;
/*
***Add the text.
*/
    if ( (fi > 90.0) && (fi <= 270.0) )
      {
      if ( fi <= 180.0 ) txtrec.v_tx = fi + 180.0;
      else               txtrec.v_tx = fi - 180.0;
      }
      else txtrec.v_tx = fi;

    txtrec.crd_tx.x_gm = x[k];
    txtrec.crd_tx.y_gm = y[k];
    txtrec.crd_tx.z_gm = z[k--];
    txtrec.pmod_tx     = 0;

    if ( dimptr->auto_ld ) WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
/*
***If needed, transform 2D polyline to XY-plane of 3D csys.
*/
    if ( dimptr->pcsy_ld > 0 )
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
/*!******************************************************/

        short   WPplcd(
        DBCdim *dimptr,
        DBCsys *csyptr,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      of a circular dimension.
 *
 *      In:  dimptr  = C-ptr to DBCdim.
 *           csyptr  = C-ptr to current csys.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 *      2007-09-17 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
    double fi,sinfi,cosfi;
    double a,b,d,e,d5,tt;
    double xp2,xp3,xp6,xp7,yp1,yp4,xt,yt,zt;
    double dy,dx,pa,radk;
    short  dimtyp,ndig;
    DBText txtrec;
    DBTmat t;

    radk = PI/180.0;

    k = *n;
    x5 = dimptr->p3_cd.x_gm;            /* Text position */
    y5 = dimptr->p3_cd.y_gm; 

    dimtyp = dimptr->dtyp_cd;                 /* Måtttyp */

    ndig = dimptr->ndig_cd;
    x1 = dimptr->p1_cd.x_gm;                 /* Start */
    y1 = dimptr->p1_cd.y_gm;
    x4 = dimptr->p2_cd.x_gm;                 /* Slut */
    y4 = dimptr->p2_cd.y_gm;
    if ( (x1 == x4) && (y1 == y4) ) return(0);

    d = dimptr->asiz_cd;                      /* Pilstorlek */
    d5 = d/5;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimptr->tsiz_cd;            /* Textstorlek */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;
/*
***Beräkna vinkeln och x2,y2,x3,y3
*/
    switch (dimtyp) {
         case CDHORIZON: {                     /* Horisontellt mått */
              if ((y5 > y1) && (y5 > y4)) { 
                   fi = 180.0;
                   if (x4 > x1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 0.0;
                   if (x4 < x1) {  
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }
              x2 = x1;
              x3 = x4;
              y2 = y3 = y5;
              break;
         }

         case CDVERTIC: {                      /* Vertikalt mått */
              if ((x5 < x1) && (x5 < x4)) { 
                   fi = 270.0;
                   if (y4 > y1) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              } else {
                   fi = 90.0;
                   if (y4 < y1) {  
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                   }
              }
              x2 = x3 = x5;
              y2 = y1;
              y3 = y4;
              break;
         }

         case CDPARALL: {                      /* Parallelt mått */
              dy = x1 - x4; dx = y4 - y1;
              tt = sqrt(dx*dx + dy*dy);
              dx /= tt; dy /= tt;
              pa = dx*(x5 -x1) + dy*(y5 -y1);
              if (pa != 0.0) {
                   if (pa < 0.0) {
                        tt = x1; x1 = x4; x4 = tt;
                        tt = y1; y1 = y4; y4 = tt;
                        dx = -dx; dy = -dy;
                        pa = -pa;
                   }
                   x2 = x1 + pa*dx; y2 = y1 + pa*dy;
                   x3 = x4 + x2 - x1; y3 = y4 + y2 - y1;
              } else {
                   x2 = x3 = y2 = y3 = 0;
              }

              dx = x3 - x2; dy = y3 - y2;
              if (dx > 0.0) 
                   if (dy > 0.0) 
                        fi = ATAN(dy/dx)/radk;
                   else if (dy < 0.0)
                        fi = 360.0 - ATAN(-(dy/dx))/radk;
                   else
                        fi = 0.0;
              else if (dx < 0.0)
                   if (dy > 0.0)
                        fi = 180.0 - ATAN(-(dy/dx))/radk;
                   else if (dy < 0.0)
                        fi = 180.0 + ATAN(dy/dx)/radk;
                   else
                        fi = 180.0;
              else
                   if (dy > 0.0) 
                        fi = 90.0;
                   else if (dy < 0.0) 
                        fi = 270.0;
                   else
                        fi = 0.0;             /* undefined */

              break;
         }
    }


    a = SQRT((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
    b = SQRT((x5 - x2)*(x5 - x2) + (y5 - y2)*(y5 - y2));
    e = SQRT((x5 - x3)*(x5 - x3) + (y5 - y3)*(y5 - y3));
    yp1 = SQRT((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
    yp4 = SQRT((x4 - x3)*(x4 - x3) + (y4 - y3)*(y4 - y3));

    switch (dimtyp) {
         case CDHORIZON: {                     /* Horisontellt mått */
              if (fi == 0.0) {
                   if (y1 < y5)
                        yp1 = -yp1;
                   if (y4 < y5)
                        yp4 = -yp4;
              }
              break;
         }
         case CDVERTIC: {                      /* Vertikalt mått */
              if (fi == 90.0) {
                   if (x1 > x5)
                        yp1 = -yp1;
                   if (x4 > x5)
                        yp4 = -yp4;
              }
              break;
         }
         case CDPARALL: {                      /* Parallelt mått */
              break;
         }
    }

    sprintf(txtformat,"\020%%0.%df",ndig);
    sprintf(txt,txtformat,a);                   /* Bygg måttsträng */
/*
***Av någon anledning har -strlen(txt)*txtrec.... slutat
***fungera i och med 1.11 på DIAB-maskinerna. Ett par extra
***parenteser löser problemet, fråga inte mig varför.
***DIAB!!!!
*/
    if ((e > a) && (b < e)) {               /* Vänster */

         xp2 = b;
         xp3 = e;
         xp7 = e + 2*d;
         if ((fi > 90.0) && (fi <= 270.0)) {
              xp6 = -(strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0);
         } else
              xp6 = 0.0;  
    } else if ((b > a) && (e < b)) {
                xp6 = -(b + 2*d);            /* Höger */
                xp2 = -b; 
                xp3 = -e;
                if ((fi <= 90.0) || (fi > 270.0)) {
                     xp7 = strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0;
                } else
                     xp7 = 0.0;  
           } else {
                xp3 = xp7 = e;
                xp2 = xp6 = -b;          /* Mellan */
           }
/*
***Bygg måttet
*/ 
    x[ ++k ] = xp2;      y[ k ] = yp1;    typ[ k ] = 0;
    x[ ++k ] = xp2;      y[ k ] = -d5;    typ[ k ] = VISIBLE;
    x[ ++k ] = xp3;      y[ k ] = yp4;    typ[ k ] = 0;
    x[ ++k ] = xp3;      y[ k ] = -d5;    typ[ k ] = VISIBLE;
    x[ ++k ] = xp6;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp7;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;

    if (xp2*xp3 > 0.0)
         d = -d;

    x[ ++k ] = xp2;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp2 + d;  y[ k ] = d5;     typ[ k ] = VISIBLE;
    x[ ++k ] = xp2 + d;  y[ k ] = -d5;    typ[ k ] = VISIBLE;
    x[ ++k ] = xp2;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;

    x[ ++k ] = xp3;      y[ k ] = 0.0;    typ[ k ] = 0;
    x[ ++k ] = xp3 - d;  y[ k ] = d5;     typ[ k ] = VISIBLE;
    x[ ++k ] = xp3 - d;  y[ k ] = -d5;    typ[ k ] = VISIBLE;
    x[ ++k ] = xp3;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;

    x[ ++k ] = 0;                         typ[ k ] = 0;

    if ((fi > 90.0) && (fi <= 270.0))
         y[ k ] = -2*d5;
    else
         y[ k ] = 2*d5;
/*
***Transformera till lokalt koordinatsystem
*/
    if (fi == 0.0)                        /* cos=1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] += x5;
              y[ i ] += y5;
         }
    else if (fi == 90.0)                  /* cos=0 sin=1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 - y[ i ];
              y[ i ] = y5 + tt;
         }
    else if (fi == 180.0)                 /* cos=-1 sin=0 */
         for (i = 0; i <= k; i++) {
              x[ i ] = x5 - x[ i ];
              y[ i ] = y5 - y[ i ];
         }
    else if (fi == 270.0)                 /* cos=0 sin=-1 */
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + y[ i ];
              y[ i ] = y5 - tt;
         }
    else {
         sinfi = SIN(radk*fi);
         cosfi = COS(radk*fi);
         for (i = 0; i <= k; i++) {
              tt = x[ i ];
              x[ i ] = x5 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y5 + tt*sinfi + y[ i ]*cosfi;
         }
    }
/*
***All Z-coordinates so far are zero.
*/
   for ( i = *n+1; i <= k; ++i ) z[i] = 0.0;
/*
***Add the text.
*/
    if ((fi > 90.0) && (fi <= 270.0)) {
         if (fi <= 180.0)
              txtrec.v_tx = fi + 180;
         else
              txtrec.v_tx = fi - 180;
    } else
         txtrec.v_tx = fi;

    txtrec.crd_tx.x_gm = x[k];
    txtrec.crd_tx.y_gm = y[k];
    txtrec.crd_tx.z_gm = z[k--];
    txtrec.pmod_tx     = 0;

    if (dimptr->auto_cd) WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
/*
***If needed, transform 2D polyline to XY-plane of 3D csys.
*/
    if ( dimptr->pcsy_cd > 0 )
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
***The End.
*/
    *n = k;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPplrd(
        DBRdim *dimptr,
        DBCsys *csyptr,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      of a radius dimension.
 *
 *      In:  dimptr  = C-ptr to DBRdim.
 *           csyptr  = C-ptr to current csys.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 *      2007-09-23 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3;
    double fi,sinfi,cosfi,tt;
    double x5,y5,dy,dx,xt,yt,zt;
    double d,d5,c,radk;
    short  ndig;
    bool   undef;
    DBText txtrec;
    DBTmat t;

/*
***Initializations.
*/
    radk = PI/180.0;

    k = *n;

    ndig = dimptr->ndig_rd;
    x1 = dimptr->p1_rd.x_gm;
    y1 = dimptr->p1_rd.y_gm;
    x2 = dimptr->p2_rd.x_gm;
    y2 = dimptr->p2_rd.y_gm;
    x3 = dimptr->p3_rd.x_gm;
    y3 = dimptr->p3_rd.y_gm;

    d = dimptr->asiz_rd;                      /* Pilstorlek */
    d5 = d/5;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimptr->tsiz_rd;            /* Textstorlek */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;
/*
***Compute arrow angle fi.
*/
    dx = x1 - x2; dy = y1 - y2;

    if ( dx > 0.0 )
      {
      if       ( dy > 0.0 ) fi = ATAN(dy/dx)/radk;
      else if ( dy < 0.0 ) fi = 360.0 - ATAN(-(dy/dx))/radk;
      else                 fi = 0.0;
      }
    else if ( dx < 0.0 )
      {
      if       ( dy > 0.0 ) fi = 180.0 - ATAN(-(dy/dx))/radk;
      else if ( dy < 0.0 ) fi = 180.0 + ATAN(dy/dx)/radk;
      else                  fi = 180.0;
      }
    else
      {
      if       ( dy > 0.0 ) fi = 90.0;
      else if ( dy < 0.0 ) fi = 270.0;
      else                  fi = 0.0;             /* 0/0 undefined */
      }

    undef = ((c = SQRT(dx*dx + dy*dy)) <= 0.0);        /* Pilens längd */
/*
***Make arrow in local coordinatsystem with x2,y2 as
***the origin.
*/
    if ( !undef )
      {
      i = k;
      x[++k] = 0.0;    y[k] = 0.0;    z[k] = 0.0;   typ[k] = 0;
      x[++k] = c;      y[k] = 0.0;    z[k] = 0.0;   typ[k] = VISIBLE;
      x[++k] = c - d;  y[k] = d5;     z[k] = 0.0;   typ[k] = VISIBLE;
      x[++k] = c - d;  y[k] = -d5;    z[k] = 0.0;   typ[k] = VISIBLE;
      x[++k] = c;      y[k] = 0.0;    z[k] = 0.0;   typ[k] = VISIBLE;
/*
***Transform to global 2D coordinates.
*/
      sinfi = SIN(radk*fi);
      cosfi = COS(radk*fi);

      while ( ++i <= k )
        {
        tt = x[ i ];
        x[i] = x2 + tt*cosfi - y[i]*sinfi;
        y[i] = y2 + tt*sinfi + y[i]*cosfi;
        }
      }
/*
***Compute line angle fi.
*/
    dx = x3 - x2; dy = y3 - y2;

    if ( dx > 0.0 )
      {
      if       ( dy > 0.0 ) fi = ATAN(dy/dx)/radk;
      else if ( dy < 0.0 ) fi = 360.0 - ATAN(-(dy/dx))/radk;
      else                  fi = 0.0;
      }
    else if (dx < 0.0)
      {
      if       ( dy > 0.0 ) fi = 180.0 - ATAN(-(dy/dx))/radk;
      else if ( dy < 0.0 ) fi = 180.0 + ATAN(dy/dx)/radk;
      else                  fi = 180.0;
      }
    else
      {
      if       ( dy > 0.0 ) fi = 90.0;
      else if ( dy < 0.0 ) fi = 270.0;
      else                  fi = 0.0;             /* 0/0 undefined */
      }

    c = SQRT(dx*dx + dy*dy);        /* Line length */
/*
***Make line in local coordinatsystem width x2,y2
***as origin.
*/
    i = k;
    x[++k] = 0.0;   y[k] = 0.0;  z[k] = 0.0;  typ[k] = 0;
/*
***If arrow direction is undefined, add it here.
*/
    if ( undef )
      {
      x[++k] = d;    y[k] = d5;    z[k] = 0.0;  typ[k] = VISIBLE; 
      x[++k] = d;    y[k] = -d5;   z[k] = 0.0;  typ[k] = VISIBLE; 
      x[++k] = 0.0;  y[k] = 0.0;   z[k] = 0.0;  typ[k] = VISIBLE; 
      }

    x[++k] = c;   y[k] = 0.0;   z[k] = 0.0;  typ[k] = VISIBLE;

    sinfi = SIN(radk*fi);
    cosfi = COS(radk*fi);
/*
***Add text.
*/
    if ( dimptr->auto_rd )
      {
      sprintf(txtformat,"R%%0.%df",ndig);
      sprintf(txt,txtformat,dimptr->r_rd);

      if ( ( fi > 90.0 ) && ( fi < 270.0 ) ) /* Up side down */
        {
        txtrec.v_tx = 180.0;
        x5 = c;
        y5 = -d5;
        }
      else
        {
        txtrec.v_tx = 0.0;
        x5 = c - (strlen(txt)*(txtrec.b_tx*txtrec.h_tx/60.0));
        y5 = d5;
        }

      txtrec.crd_tx.x_gm = x5;
      txtrec.crd_tx.y_gm = y5;
      txtrec.crd_tx.z_gm = 0.0;
      txtrec.pmod_tx     = 0;
      x[++k] = x5;
      y[k]   = y5;
      z[k--] = 0.0;
      WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
      }
/*
***Transform to global 2D coordinates and set Z = 0.
*/
    while ( ++i <= k )
      {
      tt = x[i];
      x[i] = x2 + tt*cosfi - y[i]*sinfi;
      y[i] = y2 + tt*sinfi + y[i]*cosfi;
      z[i] = 0.0;
      }
/*
***If needed, transform 2D polyline to XY-plane of 3D csys.
*/
    if ( dimptr->pcsy_rd > 0 )
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
/*!******************************************************/

        short   WPplad(
        DBAdim *dimptr,
        DBCsys *csyptr,
        double  scale,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      of an angular dimension.
 *
 *      In:  dimptr  = C-ptr to DBAdim.
 *           csyptr  = C-ptr to current csys.
 *           scale   = For WPplar().
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 *      2007-09-24 3D, J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k,ndig;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x3,y3,x5,y5,v1,v2,tv,r,r1,r2;
    double sinfi,cosfi,dfi;
    double d,d5,tt,xt,yt,zt;
    double txtlen,radk;
    bool   flag;
    DBArc  arcrec;
    DBSeg  seg;
    DBText txtrec;
    DBTmat t;

/*
***Initializations.
*/
    k = *n;

    radk = PI/180.0;

    x3 = dimptr->pos_ad.x_gm;                      /* arc centre */
    y3 = dimptr->pos_ad.y_gm;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimptr->tsiz_ad;
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;

    ndig = dimptr->ndig_ad;
    v1 = dimptr->v1_ad;                      /* Start angle */
    v2 = dimptr->v2_ad;                      /* End angle */
    tv = dimptr->tv_ad;                      /* Text angle */
    r =  dimptr->r_ad;                       /* Radius */
    r1 = dimptr->r1_ad;                      /* Start radius */
    r2 = dimptr->r2_ad;                      /* End radius */
/*
***Normalize angles to 0.0 <= v < 360.0
*/
    while ( tv >= 360.0 ) tv -= 360.0;
    while ( tv <    0.0 ) tv += 360.0;
    flag = (v1 != v2);
    while ( v1 >= 360.0 ) v1 -= 360.0;
    while ( v1 <    0.0 ) v1 += 360.0;
    while ( v2 >= 360.0 ) v2 -= 360.0;
    while ( v2 <    0.0 ) v2 += 360.0;
/*
***End angle should be greater than start angle while
***the text angle should keep it's relation to v1 and v2.
*/
    if ( v2 <= v1 )
      {
      if ( flag )
        {
        v2 += 360.0;
        tv += 360.0;
        }
      }
/*
***Extend arc to reach text.
*/
    if       ( tv > ((v1 + v2)/2.0 + 180.0)) tv -= 360.0;
    else if ( tv < ((v1 + v2)/2.0 - 180.0)) tv += 360.0;
/*
***Arrow size.
*/
    d = dimptr->asiz_ad;
    d5 = d/5;
/*
***Text length.
*/
    if ( dimptr->auto_ad )
      {
      sprintf(txtformat,"%%0.%df\015",ndig);
      sprintf(txt,txtformat,(v2-v1));
      if ( (txtlen=strlen(txt)*(txtrec.b_tx*txtrec.h_tx/60.0) - 1.0) < 0.0 ) txtlen = 0.0;
      }
    else txtlen = 0.0;
/*
***Build arc.
*/
    arcrec.fnt_a = SOLIDARC;
    arcrec.x_a = x3;
    arcrec.y_a = y3;
    arcrec.r_a = r;
    arcrec.ns_a = 0;         /* 2D */
    arcrec.v1_a = v1;
    arcrec.v2_a = v2;

    k = -1;
    WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);

    if ( k > PLYMXV-15 )
      {
     *n = k;
      return(0);
      }
/*
***Extend arc.
*/
    dfi = 1.0/(r*radk);

    if ( tv >= v2 )
      {
      arcrec.v1_a = v1 - (d + d)*dfi;
      arcrec.v2_a = v1;
      WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
      arcrec.v1_a = v2;
      if ( tv <= 180.0 ) arcrec.v2_a = tv;
      else               arcrec.v2_a = tv + txtlen*dfi;
      WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
      }
    else if ( tv <= v1 )
      {
      arcrec.v1_a = v2;
      arcrec.v2_a = v2 + (d + d)*dfi;
      WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
      arcrec.v2_a = v1;
      if ( tv <= 180.0 ) arcrec.v1_a = tv - txtlen*dfi;
      else               arcrec.v1_a = tv;
      WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
      }

    if ( k > PLYMXV-15 )
      {
     *n = k;
      return(0);
      }

    if ( (flag = ((tv <= v1) || (tv >= v2))) ) d = -d;
/*
***Build line and arrow 1.
*/
    i = k;
    x[++k] = r1;     y[k] = 0.0;    z[k] = 0.0; typ[k] = 0;
    x[++k] = r;      y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE;
    x[++k] = r + d5; y[k] = d;      z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r - d5; y[k] = d;      z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r;      y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r + d5; y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE; 
/*
***Transform to 2D system.
*/
    sinfi = SIN(radk*v1);
    cosfi = COS(radk*v1);

    while ( ++i <= k )
      {
      tt = x[i];
      x[i] = x3 + tt*cosfi - y[i]*sinfi;
      y[i] = y3 + tt*sinfi + y[i]*cosfi;
      }
/*
***Build line and arrow 2.
*/
    i = k;
    x[++k] = r2;     y[k] = 0.0;    z[k] = 0.0; typ[k] = 0;
    x[++k] = r;      y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE;
    x[++k] = r + d5; y[k] = -d;     z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r - d5; y[k] = -d;     z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r;      y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE; 
    x[++k] = r + d5; y[k] = 0.0;    z[k] = 0.0; typ[k] = VISIBLE; 
/*
***Transform to 2D system.
*/
    sinfi = SIN(radk*v2);
    cosfi = COS(radk*v2);

    while ( ++i <= k )
      {
      tt = x[i];
      x[i] = x3 + tt*cosfi - y[i]*sinfi;
      y[i] = y3 + tt*sinfi + y[i]*cosfi;
      }
/*
***Build text.
*/
    if ( flag ) d = -d;


    if (dimptr->auto_ad)
      {
      if ( r != 0.0 ) dfi = ATAN(0.5*txtlen/r)/radk;
      else            dfi = 0.0;

      if ( tv >= 360.0 ) tv -= 360.0;

      if ( tv <= 180.0 ) /* Text dir down */
        {
        txtrec.v_tx = 270.0;
        x5 = r + d5;
        y5 = 0.5*txtlen;
        dfi = -dfi;
        }
      else /* Text dir up */
        {
        txtrec.v_tx = 90.0;
        tt = r*r - txtlen*txtlen*0.25;
        if ( tt < 0.0 ) x5 = -SQRT(-tt) - d5;
        else            x5 = SQRT(tt) - d5;
        y5 = -0.5*txtlen;
        }

      txtrec.crd_tx.x_gm = x5;
      txtrec.crd_tx.y_gm = y5;
      txtrec.pmod_tx     = 0;
      x[++k] = x5;
      y[k--] = y5;

      i = k;
      WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
/*
***Transform to 2D system
*/
      sinfi = SIN(radk*(tv + dfi));
      cosfi = COS(radk*(tv + dfi));

      while ( ++i <= k )
        {
        tt = x[i];
        x[i] = x3 + tt*cosfi - y[i]*sinfi;
        y[i] = y3 + tt*sinfi + y[i]*cosfi;
        z[i] = 0.0;
        }

      txtrec.crd_tx.x_gm = x3 + x5*cosfi - y5*sinfi;
      txtrec.crd_tx.y_gm = y3 + x5*sinfi + y5*cosfi;
      }
/*
***No text.
*/
    else
      {
      txtrec.crd_tx.x_gm = x3;
      txtrec.crd_tx.y_gm = y3;
      }
/*
***If needed, transform 2D polyline to XY-plane of 3D csys.
*/
    if ( dimptr->pcsy_ad > 0 )
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
***The End.
*/
   *n = k;

    return(0);
  }

/********************************************************/
