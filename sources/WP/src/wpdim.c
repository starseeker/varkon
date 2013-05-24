/**********************************************************************
*
*    wpdim.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
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

extern short actpen;

static short drawdm(WPGWIN *gwinpt, DBAny *dimpek, DBptr la, bool draw);

/*!******************************************************/

        short WPdrdm(
        DBAny *dimpek,
        DBptr   la,
        DBint   win_id)

/*      Display linear, diameter, radius or angular dimension.
 *
 *      In: dimpek => Pekare till mått-post.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Return:  0 => Ok.
 *
 *      (C) microform ab 27/1/95 J. Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om den är blankad är det enkelt.
*/
   if ( dimpek->hed_un.blank) return(0);
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
***Ja, ligger det på en nivå som är tänd i detta fönster ?
*/
         if ( WPnivt(gwinpt,dimpek->hed_un.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( dimpek->hed_un.pen != actpen ) WPspen(dimpek->hed_un.pen);
/*
***Sen är det bara att rita.
*/
           drawdm(gwinpt,dimpek,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPdldm(
        DBAny *dimpek,
        DBptr   la,
        DBint   win_id)

/*      Erase dimension.
 *
 *      In: dimpek => Pekare till mått-post.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Return:  0 => Ok.
 *
 *      (C) microform ab 27/1/95 J. Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
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
         if ( WPfobj(gwinpt,la,dimpek->hed_un.type,&typ) )
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
           if ( !WPnivt(gwinpt,dimpek->hed_un.level)  ||
                               dimpek->hed_un.blank) return(0);
           drawdm(gwinpt,dimpek,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawdm(
        WPGWIN *gwinpt,
        DBAny  *dimpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar ett mått i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          dimpek => Pekare till mått-post.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      (C)microform ab 27/1/95 J. Kjellander
 *
 *      2006-12-26 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;
   char   a[PLYMXV];
   int    k;

/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;

   switch ( dimpek->hed_un.type )
     {
     case LDMTYP:
     WPplld(&dimpek->ldm_un,&k,x,y,z,a);
     break;

     case CDMTYP:
     WPplcd(&dimpek->cdm_un,&k,x,y,z,a);
     break;

     case RDMTYP:
     WPplrd(&dimpek->rdm_un,&k,x,y,z,a);
     break;

     case ADMTYP:
     scale = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
              gwinpt->geo.psiz_x /
             (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
     WPplad(&dimpek->adm_un,scale,&k,x,y,z,a);
     break;
    }
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( WPcply(gwinpt,-1,&k,x,y,a) )
     {
     if ( draw  &&  dimpek->hed_un.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,dimpek->hed_un.type) )
         WPdobj(gwinpt,TRUE);
       else
         return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short   WPplld(
        DBLdim *dimpek,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      for a linear dimension.
 *      
 *      In:  dimpek  = C-ptr to DBLdim.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
    double fi,sinfi,cosfi;
    double a,b,d,e,d5,tt;
    double xp2,xp3,xp6,xp7,yp1,yp4;
    double dy,dx,pa,radk;
    short  dimtyp,ndig;
    DBText txtrec;

/*
***Initializations.
*/
    radk = PI/180.0;
    dimtyp = dimpek->dtyp_ld;           /* Type */
    ndig = dimpek->ndig_ld;             /* Decimals */
    d = dimpek->asiz_ld;                /* Arrow size */
    d5 = d/5;

    k = *n;
    x5 = dimpek->p3_ld.x_gm;            /* Text position */
    y5 = dimpek->p3_ld.y_gm; 
    x1 = dimpek->p1_ld.x_gm;            /* Start */
    y1 = dimpek->p1_ld.y_gm;       
    x4 = dimpek->p2_ld.x_gm;            /* End */
    y4 = dimpek->p2_ld.y_gm;
    if ((x1 == x4) && (y1 == y4)) return(0);

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimpek->tsiz_ld;      /* Text size */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;
/*
***Beräkna vinkeln och x2,y2,x3,y3
*/ 
    switch (dimtyp) {

         case LDHORIZON: {                     /* Horisontellt mått */
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

         case LDVERTIC: {                      /* Vertikalt mått */
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

         case LDPARALL: {                      /* Parallelt mått */
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
    if ((e > a) && (b < e)) {               /* Vänster */

         xp2 = b;
         xp3 = e;
         xp7 = e + 2*d;      
         if ((fi > 90.0) && (fi <= 270.0))
              xp6 = -(strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0);
         else
              xp6 = 0.0;  
    } else if ((b > a) && (e < b)) {
                xp6 = -(b + 2*d);            /* Höger */
                xp2 = -b; 
                xp3 = -e;
                if ((fi <= 90.0) || (fi > 270.0))
                     xp7 = strlen(txt)*txtrec.b_tx*txtrec.h_tx/60.0;
                else
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
              txtrec.v_tx = fi + 180.0;
         else
              txtrec.v_tx = fi - 180.0;
    } else
         txtrec.v_tx = fi;

    txtrec.crd_tx.x_gm = x[ k ];
    txtrec.crd_tx.y_gm = y[ k-- ];
    txtrec.pmod_tx     = 0;

    if ( dimpek->auto_ld ) WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
/*
***End.
*/
   *n = k;    

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPplcd(
        DBCdim *dimpek,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      for a circular dimension.
 *      
 *      In:  dimpek  = C-ptr to DBCdim.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
    double fi,sinfi,cosfi;
    double a,b,d,e,d5,tt;
    double xp2,xp3,xp6,xp7,yp1,yp4;
    double dy,dx,pa,radk;
    short  dimtyp,ndig;
    DBText txtrec;

    radk = PI/180.0;

    k = *n;    
    x5 = dimpek->p3_cd.x_gm;            /* Text position */
    y5 = dimpek->p3_cd.y_gm; 

    dimtyp = dimpek->dtyp_cd;                 /* Måtttyp */

    ndig = dimpek->ndig_cd;
    x1 = dimpek->p1_cd.x_gm;                 /* Start */
    y1 = dimpek->p1_cd.y_gm;       
    x4 = dimpek->p2_cd.x_gm;                 /* Slut */
    y4 = dimpek->p2_cd.y_gm;
    if ((x1 == x4) && (y1 == y4))
         return(0);

    d = dimpek->asiz_cd;                      /* Pilstorlek */
    d5 = d/5;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimpek->tsiz_cd;            /* Textstorlek */
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

    txtrec.crd_tx.x_gm = x[ k ];
    txtrec.crd_tx.y_gm = y[ k-- ];
    txtrec.pmod_tx     = 0;

    if (dimpek->auto_cd) WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);
/*
***The End.
*/
    *n = k;    

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPplrd(
        DBRdim *dimpek,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      for a radius dimension.
 *      
 *      In:  dimpek  = C-ptr to DBRdim.
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x1,y1,x2,y2,x3,y3;
    double fi,sinfi,cosfi,tt;
    double x5,y5,dy,dx;
    double d,d5,c,radk;
    short  ndig;
    bool   undef;
    DBText txtrec;

/*
***Initializations.
*/
    radk = PI/180.0;

    k = *n;    

    ndig = dimpek->ndig_rd;
    x1 = dimpek->p1_rd.x_gm;  
    y1 = dimpek->p1_rd.y_gm;       
    x2 = dimpek->p2_rd.x_gm;  
    y2 = dimpek->p2_rd.y_gm;
    x3 = dimpek->p3_rd.x_gm;  
    y3 = dimpek->p3_rd.y_gm;

    d = dimpek->asiz_rd;                      /* Pilstorlek */
    d5 = d/5;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimpek->tsiz_rd;            /* Textstorlek */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;
/*
***Beräkna vinkeln fi för hänvisningspilen
*/ 
    dx = x1 - x2; dy = y1 - y2;
    if ( dx > 0.0 ) 
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
        fi = 0.0;             /* 0/0 undefined */

    undef = ((c = SQRT(dx*dx + dy*dy)) <= 0.0);        /* Pilens längd */
/*
***Bygg pil i lokalt koordinatsystem med x2,y2 i origo
*/
    if ( !undef )
      {
      i = k;
      x[++k] = 0.0;    y[k] = 0.0;    typ[k] = 0;
      x[++k] = c;      y[k] = 0.0;    typ[k] = VISIBLE;
      x[++k] = c - d;  y[k] = d5;     typ[k] = VISIBLE; 
      x[++k] = c - d;  y[k] = -d5;    typ[k] = VISIBLE; 
      x[++k] = c;      y[k] = 0.0;    typ[k] = VISIBLE; 
/*
***Transformera till globala koordinater
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
***Beräkna vinkeln fi för måttlinjen
*/ 
    dx = x3 - x2; dy = y3 - y2;
    if ( dx > 0.0 ) 
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
        fi = 0.0;             /* 0/0 undefined */

    c = SQRT(dx*dx + dy*dy);        /* Måttlinjens längd */
/*
***Bygg måttlinje i lokalt koordinatsystem med x2,y2 i origo
*/
    i = k;
    x[++k] = 0.0;    y[k] = 0.0;    typ[k] = 0;
/*
***Om pilens riktning var odef. bygg på den här i stället
*/
    if ( undef )
      { 
      x[++k] = d;    y[k] = d5;   typ[k] = VISIBLE; 
      x[++k] = d;    y[k] = -d5;  typ[k] = VISIBLE; 
      x[++k] = 0.0;  y[k] = 0.0;  typ[k] = VISIBLE; 
      }

    x[++k] = c;    y[k] = 0.0;   typ[k] = VISIBLE;

    sinfi = SIN(radk*fi);
    cosfi = COS(radk*fi);

    if ( dimpek->auto_rd )             /* ska det vara måttext? */
      {
      sprintf(txtformat,"R%%0.%df",ndig);       /* Textformat */
      sprintf(txt,txtformat,dimpek->r_rd);       /* Måttsträng */

      if ( (fi > 90.0) && (fi < 270.0) ) /* Rita upponer på undersidan */
        {
        txtrec.v_tx = 180.0;
        x5 = c;  
        y5 = -d5;
        }
      else   /* Rita förskjutet åt vänster på ovansidan */
        {
        txtrec.v_tx = 0.0;
        x5 = c - (strlen(txt)*(txtrec.b_tx*txtrec.h_tx/60.0));
        y5 = d5;
        }
      txtrec.crd_tx.x_gm = x5;
      txtrec.crd_tx.y_gm = y5;
      txtrec.pmod_tx     = 0;
      x[ ++k ] = x5;
      y[ k-- ] = y5;
      WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);

      txtrec.crd_tx.x_gm = x2 + x5*cosfi - y5*sinfi; /* textstart */
      txtrec.crd_tx.y_gm = y2 + x5*sinfi + y5*cosfi;
      }
    else
      {
      txtrec.crd_tx.x_gm = x2;                 /* Brytpunkten */
      txtrec.crd_tx.y_gm = y2;
      }
/*
***Transform to global coordinates and set Z = 0.
*/
    while ( ++i <= k )
      {
      tt = x[i];
      x[i] = x2 + tt*cosfi - y[i]*sinfi;
      y[i] = y2 + tt*sinfi + y[i]*cosfi;
      z[i] = 0.0;
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
        DBAdim *dimpek,
        double  scale,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    typ[])

/*      Creates the graphical 3D polyline representation 
 *      for an angular dimension.
 *      
 *      In:  dimpek  = C-ptr to DBAdim.
 *           scale   = For WPplar()
 *           n+1     = Offset to polyline start.
 *
 *      Out: n       = Offset to polyline end.
 *           x,y,z,a = Polyline coordinates and status
 *
 *      Return: 0    = Ok.
 *
 *      (C)2006-12-26 J.Kjellander
 *
 ******************************************************!*/

  {
    int    i,k,ndig;
    char   txt[MAXTXT+1];
    char   txtformat[16];
    double x3,y3,x5,y5,v1,v2,tv,r,r1,r2;
    double sinfi,cosfi,dfi;
    double d,d5,tt;
    double len,radk;
    bool   flag;
    DBArc  arcrec;
    DBSeg  seg;
    DBText txtrec;

/*
***Initializations.
*/
    k = *n;    

    radk = PI/180.0;

    x3 = dimpek->pos_ad.x_gm;                      /* cirkelcentrum */
    y3 = dimpek->pos_ad.y_gm;

    txtrec.fnt_tx = 0;
    txtrec.h_tx = dimpek->tsiz_ad;            /* Textstorlek */
    txtrec.b_tx = 60.0;
    txtrec.l_tx = 15.0;

    ndig = dimpek->ndig_ad;
    v1 = dimpek->v1_ad;                      /* Startvinkel */
    v2 = dimpek->v2_ad;                      /* Slutvinkel */  
    tv = dimpek->tv_ad;                      /* Textvinkel */
    r =  dimpek->r_ad;                       /* Radien */
    r1 = dimpek->r1_ad;                      /* startradien startv. */
    r2 = dimpek->r2_ad;                      /* Startradie slutv */
/*
***Normalisera till 0.0 <= v < 360.0
*/
    while ( tv >= 360.0 ) tv -= 360.0;
    while ( tv <    0.0 ) tv += 360.0;
    flag = (v1 != v2);
    while ( v1 >= 360.0 ) v1 -= 360.0;
    while ( v1 <    0.0 ) v1 += 360.0;
    while ( v2 >= 360.0 ) v2 -= 360.0;
    while ( v2 <    0.0 ) v2 += 360.0;
/*
***Slutvinkeln ska vara större än startvinkeln
***och textvinkelns relation till v1 och v2 ska bevaras.
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
***Förläng cirkelbågen på sidan närmast tv
*/
    if ( tv > ((v1 + v2)/2.0 + 180.0)) 
         tv -= 360.0;
    else if ( tv < ((v1 + v2)/2.0 - 180.0)) 
         tv += 360.0;

    d = dimpek->asiz_ad;                      /* Pilstorlek */
    d5 = d/5;
/*
***Ska det vara måttext?
*/
    if ( dimpek->auto_ad )
      {
      sprintf(txtformat,"%%0.%df\015",ndig);    /* Textformat */
      sprintf(txt,txtformat,(v2-v1));            /* Måttsträng */
      if ( (len=strlen(txt)*(txtrec.b_tx*txtrec.h_tx/60.0) - 1.0) < 0.0 ) len = 0.0;
      }
    else len = 0.0;

    arcrec.fnt_a = SOLIDARC;           /* Heldragen */
    arcrec.x_a = x3;
    arcrec.y_a = y3;
    arcrec.r_a = r;
    arcrec.ns_a = 0;                    /* 2D */
    arcrec.v1_a = v1;
    arcrec.v2_a = v2;

    k = -1;
    WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);    /* bygg cirkelbåge */

    if (k > PLYMXV-15) {                /* Out of space */
         *n = k;    
         return(0);
    }
/*
***Dra ut cirkelbågen
*/
    dfi = 1.0/(r*radk);
    if (tv >= v2) {
         arcrec.v1_a = v1 - (d + d)*dfi;
         arcrec.v2_a = v1;
         WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
         arcrec.v1_a = v2;
         if (tv <= 180.0)
              arcrec.v2_a = tv;
         else
              arcrec.v2_a = tv + len*dfi;
         WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
    } else if (tv <= v1) {
         arcrec.v1_a = v2;
         arcrec.v2_a = v2 + (d + d)*dfi;
         WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
         arcrec.v2_a = v1;
         if (tv <= 180.0)
              arcrec.v1_a = tv - len*dfi;
         else
              arcrec.v1_a = tv;
         WPplar(&arcrec,&seg,scale,&k,x,y,z,typ);
    }


    if (k > PLYMXV-15) {                /* Out of space */
         *n = k;    
         return(0);
    }

    if ((flag = ((tv <= v1) || (tv >= v2))))
         d = -d;
/*
***Bygg begr. linje 1
*/
         i = k;
         x[ ++k ] = r1;     y[ k ] = 0.0;    typ[ k ] = 0;
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;
         x[ ++k ] = r + d5; y[ k ] = d;      typ[ k ] = VISIBLE; 
         x[ ++k ] = r - d5; y[ k ] = d;      typ[ k ] = VISIBLE; 
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
         x[ ++k ] = r + d5; y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
/*
***Transformera till globala koordinater
*/     
         sinfi = SIN(radk*v1);
         cosfi = COS(radk*v1);
         while (++i <= k) {
              tt = x[ i ];
              x[ i ] = x3 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y3 + tt*sinfi + y[ i ]*cosfi;
         }
/*
***Bygg begr. linje 2
*/
         i = k;
         x[ ++k ] = r2;     y[ k ] = 0.0;    typ[ k ] = 0;
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE;
         x[ ++k ] = r + d5; y[ k ] = -d;     typ[ k ] = VISIBLE; 
         x[ ++k ] = r - d5; y[ k ] = -d;     typ[ k ] = VISIBLE; 
         x[ ++k ] = r;      y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
         x[ ++k ] = r + d5; y[ k ] = 0.0;    typ[ k ] = VISIBLE; 
/*
***Transformera till globala koordinater
*/     
         sinfi = SIN(radk*v2);
         cosfi = COS(radk*v2);
         while (++i <= k) {
              tt = x[ i ];
              x[ i ] = x3 + tt*cosfi - y[ i ]*sinfi;
              y[ i ] = y3 + tt*sinfi + y[ i ]*cosfi;
         }

    if (flag)
         d = -d;


    if (dimpek->auto_ad) {             /* ska det vara måttext? */


         if (r != 0.0)
              dfi = ATAN(0.5*len/r)/radk;
         else
              dfi = 0.0;


         if (tv >= 360.0)
            tv -= 360.0;
         if (tv <= 180.0) {
                                     /* Rita texten rakt ned */
              txtrec.v_tx = 270.0;
              x5 = r + d5;
              y5 = 0.5*len;
              dfi = -dfi;
         } else {
                                     /* Rita texten uppåt */
              txtrec.v_tx = 90.0;
              tt = r*r - len*len*0.25;
              if (tt < 0.0)
                   x5 = -SQRT(-tt) - d5;
              else
                   x5 = SQRT(tt) - d5;
              y5 = -0.5*len;

         }
         txtrec.crd_tx.x_gm = x5;
         txtrec.crd_tx.y_gm = y5;
         txtrec.pmod_tx     = 0;
         x[++k] = x5;
         y[k--] = y5;
/*
***Följander rad är en bug-rättning som upptäcktes i samband
***med optimering av text-genereringen. 1:a vektorn i texten
***transformerades inte till globala koordinater. Detta hade
***ingen betydelse tidigare då texten normalt började med två
***släckta vektorer.
*/
         i = k;
/*
***Slut bugrättning 2/11/92 JK.
*/
         WPpltx(&txtrec,(unsigned char *)txt,&k,x,y,z,typ);

/* Transformera till globala koordinater */
  
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

    } else {
         txtrec.crd_tx.x_gm = x3;
         txtrec.crd_tx.y_gm = y3;
    }
/*
***The End.
*/
   *n = k;    

    return(0);
  }

/********************************************************/
