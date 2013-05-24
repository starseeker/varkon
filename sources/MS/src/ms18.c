/**********************************************************************
*
*    ms18.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msupgp();   Update grapac
*     msfixg();   Sets grapac for drawing
*     msspen();   Sets active pen
*     msclin();   Clip line
*     mscply();   Clip polyline
*
*     mscpmk();   Create hit marker
*     msepmk();   Delete hit marker
*     msdpmk();   Draw/erase hit marker
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
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/GP/include/GP.h"
#include "../../../sources/WP/include/WP.h"

#ifdef senare
int     msscur();    /* Sätter grafisk cursor */
#endif

extern short   actpen;

/********************************************************/

        int     msupgp(gwinpt)
        WPGWIN *gwinpt;

/*      Uppdaterar grapac:s actwin och actvym mm.
 *
 *      In: gwinpt => Pekare till fönster.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ********************************************************/

 {
   double viewpt[4];

extern bool   gpgenz;
extern short  gpgorx,gpgory,gpsnpx,gpsnpy;
extern double k1x,k2x,k1y,k2y;
extern VY     actvy;
extern DBTmat actvym;

/*
***Vi börjar med viewport. Denna kan ändras tex. av wpcogw().
***ConfigureNotify-handlern.
*/
   viewpt[0] = gwinpt->vy.scrwin.xmin;
   viewpt[1] = gwinpt->vy.scrwin.ymin;
   viewpt[2] = gwinpt->vy.scrwin.xmax;
   viewpt[3] = gwinpt->vy.scrwin.ymax;
   gpsvpt(viewpt);
/*
***grapac-variablerna gpgnpx och gpgnpy sätts av gpsvpt()
***men inte gpgorx och gpgory.
*/
   gpgorx = (short)gwinpt->vy.scrwin.xmin;
   gpgory = (short)gwinpt->vy.scrwin.ymin;
/*
***Av samma anledning måste globala variablerna gpsnpx och
***gpsnpy uppdateras.
*/
   gpsnpx = gwinpt->geo.dx;
   gpsnpy = gwinpt->geo.dy;
/*
***Sen tar vi aktiv vy.
*/
   strcpy(actvy.vynamn,gwinpt->vy.vynamn);
   actvy.vywin[0] = gwinpt->vy.modwin.xmin;
   actvy.vywin[2] = gwinpt->vy.modwin.xmax;
   actvy.vywin[1] = gwinpt->vy.modwin.ymin;
   actvy.vywin[3] = gwinpt->vy.modwin.ymax;
   actvy.vydist   = gwinpt->vy.vydist;
   actvy.vy3d     = (tbool)gwinpt->vy.vy_3D;
/*
***Om perspektiv är på skall Z-flaggan sättas.
*/
   if ( gwinpt->vy.vydist == 0.0 ) gpgenz = FALSE;
   else                            gpgenz = TRUE;
/*
***2D Transformationskonstanter.
*/
   k1x = gwinpt->vy.k1x;
   k1y = gwinpt->vy.k1y;
   k2x = gwinpt->vy.k2x;
   k2y = gwinpt->vy.k2y;
/*
***Aktiv 3D transformationsmatris.
*/
   actvym.g11 = gwinpt->vy.vymat.k11;
   actvym.g12 = gwinpt->vy.vymat.k12;
   actvym.g13 = gwinpt->vy.vymat.k13;
   actvym.g14 = 0.0;
  
   actvym.g21 = gwinpt->vy.vymat.k21;
   actvym.g22 = gwinpt->vy.vymat.k22;
   actvym.g23 = gwinpt->vy.vymat.k23;
   actvym.g24 = 0.0;
  
   actvym.g31 = gwinpt->vy.vymat.k31;
   actvym.g32 = gwinpt->vy.vymat.k32;
   actvym.g33 = gwinpt->vy.vymat.k33;
   actvym.g34 = 0.0;
  
   actvym.g41 = 0.0;
   actvym.g42 = 0.0;
   actvym.g43 = 0.0;
   actvym.g44 = 1.0;

   return(0);
 }

/********************************************************/
/********************************************************/

        int     msfixg(gwinpt)
        WPGWIN *gwinpt;

/*      Sätter om globala variabler i grapac så att
 *      grapac:s vektorgenereringsrutiner kan användas
 *      av MSPAC för WPGWIN-fönster.
 *
 *      In: gwinpt => Pekare till fönster eller NULL för
 *                    återställning av ursprungliga data.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ********************************************************/

 {
extern double k2x,k2y,gpgszx;
extern VY     actvy;
extern DBTmat actvym;

static   double orgszx,orgk2x,orgk2y;
static   VY     orgvy;
static   DBTmat orgvym;

/*
***Gäller det "tillställning" eller "återställning" ?
*/
   if ( gwinpt != NULL )
     {
     orgszx = gpgszx;
     orgk2x = k2x;
     orgk2y = k2y;
     V3MOME(&actvy,&orgvy,sizeof(VY));
     V3MOME(&actvym,&orgvym,sizeof(DBTmat));

     actvy.vywin[0] = gwinpt->vy.modwin.xmin;
     actvy.vywin[1] = gwinpt->vy.modwin.ymin;
     actvy.vywin[2] = gwinpt->vy.modwin.xmax;
     actvy.vywin[3] = gwinpt->vy.modwin.ymax;
     actvy.vydist   = gwinpt->vy.vydist;
     actvy.vy3d     = (tbool)gwinpt->vy.vy_3D;
     gpgszx         = gwinpt->geo.dx * gwinpt->geo.psiz_x;
     k2x            = gwinpt->vy.k2x;
     k2y            = gwinpt->vy.k2y;

     actvym.g11 = gwinpt->vy.vymat.k11;
     actvym.g12 = gwinpt->vy.vymat.k12;
     actvym.g13 = gwinpt->vy.vymat.k13;
     actvym.g14 = 0.0;
  
     actvym.g21 = gwinpt->vy.vymat.k21;
     actvym.g22 = gwinpt->vy.vymat.k22;
     actvym.g23 = gwinpt->vy.vymat.k23;
     actvym.g24 = 0.0;
  
     actvym.g31 = gwinpt->vy.vymat.k31;
     actvym.g32 = gwinpt->vy.vymat.k32;
     actvym.g33 = gwinpt->vy.vymat.k33;
     actvym.g34 = 0.0;
  
     actvym.g41 = 0.0;
     actvym.g42 = 0.0;
     actvym.g43 = 0.0;
     actvym.g44 = 1.0;
     }
/*
***Vid återställning kopierar vi tillbaks orginalen.
*/
   else
     {
     gpgszx         = orgszx;
     k2x            = orgk2x;
     k2y            = orgk2y;
     V3MOME(&orgvy,&actvy,sizeof(VY));
     V3MOME(&orgvym,&actvym,sizeof(DBTmat));
     }

   return(0);
 }

/********************************************************/
/********************************************************/

        int   msspen(pen)
        short pen;

/*      Sätter om aktiv penna, dvs. färg.
 *
 *      In: pen = Pennummer.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ********************************************************/

{
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Ändra aktiv penna (förgrundsfärg) i WPGWIN-fönstrets GC.
*/
   if ( pen != actpen  &&
       (winptr=mswgwp((wpw_id)GWIN_MAIN)) != NULL  &&
        winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     SelectObject(gwinpt->dc,msgcol(pen));
     SelectObject(gwinpt->bmdc,msgcol(pen));
     actpen = pen;
     }

    return(0);
}

/********************************************************/
/*!******************************************************/

        int     msclin(gwinpt,pt1,pt2)
        WPGWIN *gwinpt;
        WPGRPT *pt1,*pt2;

/*      Klipper en grafisk linje mot ett fönster.
 *
 *      In: gwinpt  => Pekare till fönster.
 *          pt1,pt2 => Pekare till indata/utdata.
 *
 *      Ut: *pt1,*pt2 = Ändpunkter, ev. klippta.
 *
 *      FV: -1 => Linjen är oklippt och ligger utanför fönstret
 *           0 => Linjen är oklippt och ligger innanför fönstret.
 *           1 => Linjen är klippt i pt1 (start).
 *           2 => Linjen är klippt i pt2 (slut).
 *           3 => Linjen är klippt i både pt1 och pt2.
 *
 *      (C) microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   short  sts1,sts2;
   double p1x,p1y,p2x,p2y,xmin,xmax,ymin,ymax;

   xmin = gwinpt->vy.modwin.xmin;
   ymin = gwinpt->vy.modwin.ymin;
   xmax = gwinpt->vy.modwin.xmax;
   ymax = gwinpt->vy.modwin.ymax;

   p1x = pt1->x;
   p1y = pt1->y;
   p2x = pt2->x;
   p2y = pt2->y;

/*
***Till att börja med antar vi att hela vektorn ligger
***innanför fönstret.
*/
   sts1 = sts2 = 0;
/*
***Om punkt 1 ligger till vänster om fönstret kanske
***punkt 2 gör det också.
*/
   if ( p1x < xmin )
     {
     if ( p2x < xmin ) return(-1);
/*
***Om inte klipper vi.
*/
     p1y += (p2y - p1y)*(xmin - p1x)/(p2x - p1x);
     p1x  = xmin;
     sts1 = 1;
     }
/*
***Punkt 1 ligger till höger om fönstret. Kanske punkt 2 också.
*/
   else if ( p1x > xmax )
     {
     if ( p2x > xmax ) return(-1);
/*
***Ack nej, då klipper vi mot högra kanten istället.
*/
     p1y += (p2y - p1y)*(xmax - p1x)/(p2x - p1x);
     p1x  = xmax;
     sts1 = 1; 
     }
/*
***Om punkt 1 ligger nedanför kanske punkt 2 gör det också.
*/
   if ( p1y < ymin )
     {
     if ( p2y < ymin ) return(-1);
/*
***Nej då klipper vi mot underkanten.
*/
     p1x += (p2x - p1x)*(ymin - p1y)/(p2y - p1y);
     p1y  = ymin;
     sts1 = 1;
     }
/*
***Om punkt 1 ligger ovanför kanske punkt 2 gör det också.
*/
   else if ( p1y > ymax )
     {
     if ( p2y > ymax ) return(-1);
/*
***Nej, då klipper vi mot överkanten.
*/
     p1x += (p2x - p1x)*(ymax - p1y)/(p2y - p1y);
     p1y  = ymax;
     sts1 = 1;
     }
/*
***Om punkt 1 är klippt men fortfarande utanför måste ändå
***hela vektorn ligga utanför.
*/
   if ( sts1 == 1 )
     if ( (p1x < xmin)  || (p1x > xmax)  ||
          (p1y < ymin)  || (p1y > ymax)) return(-1);
/*
***Punkt 1 ligger innanför fönstret, klipp punkt 2 om utanför.
*/ 
   if ( p2x < xmin )
     {
     p2y += (p2y -  p1y)*(xmin - p2x)/(p2x - p1x);
     p2x  = xmin;
     sts2 = 2;
     }
   else if ( p2x > xmax )
     {
     p2y += (p2y -  p1y)*(xmax - p2x)/(p2x - p1x);
     p2x  = xmax;
     sts2 = 2;
     }

   if ( p2y < ymin )
     {
     p2x += (p2x - p1x)*(ymin - p2y)/(p2y - p1y);
     p2y  = ymin;
     sts2 = 2;
     }
   else if ( p2y > ymax )
     {
     p2x += (p2x - p1x)*(ymax - p2y)/(p2y - p1y);
     p2y  = ymax;
     sts2 = 2;
     }
/*
***Kopiera tillbaks klippta koordinater till pt1 och pt2.
*/
   pt1->x = p1x;
   pt1->y = p1y;
   pt2->x = p2x;
   pt2->y = p2y;
/*
***Slut.
*/
   return(sts1 + sts2);
 }

/********************************************************/
/*!******************************************************/

        bool     mscply(gwinpt,kmin,kmax,x,y,a)
        WPGWIN  *gwinpt;
        short    kmin;
        short   *kmax;
        double   x[];
        double   y[];
        char     a[];

/*      Klipper en polylinje mot ett visst fönster.
*
*       In: gwinpt => Pekare till grafiskt fönster.
*           kmin+1 => Offset till polylinjestart.
*           kmax   => Offset till polylinjeslut.
*           x      => X-koordinater.
*           y      => Y-koordinater.
*           a      => Tänd/Släck.
*
*           xÄkmin+1Å,yÄ min+1Å,aÄkmin+1Å = Startpunkt med status
*            !
*           xÄkmaxÅ,yÄkmaxÅ,aÄkmaxÅ = Slutpunkt med status
* 
*       Ut: *x,*y  => Klippta vektorer.
*           *kmax  => Offset till polylinjeslut.
*           *a     => Status (Bit(ENDSIDE) modifierad) om mer än 50%
*                    av polylinjen klippts bort.
* 
*       FV: TRUE = Någon del av polylinjen är synlig.
*
*       (C)microform ab 1/11/95 J. Kjellander
*
********************************************************!*/

{
   short  i,j,l;
   double sav0,sav1;
   WPGRPT pt1,pt2;

/*
***Initiering.
*/
   i = j = kmin + 1;
/*
***Leta upp 1:a synliga vektor. Om ingen synlig, returnera FALSE.
*/
   do
     { 
     if ( i >= *kmax )
       {
       *kmax = kmin;
       return(FALSE);
       } 
     pt1.x = x[i];
     pt1.y = y[i];
     pt2.x = sav0 = x[++i];
     pt2.y = sav1 = y[i];
     } while ( msclin(gwinpt,&pt1,&pt2) == -1 );
/*
***I vec ligger nu en vektor som efter klippning syns, helt
***eller delvis! Börja med en släckt förflyttning till
***startpunkten och en normal förflyttning till nästa punkt.
*/
   x[j]  = pt1.x;
   y[j]  = pt1.y;
   a[j] &= a[i-1] & ~VISIBLE; /* Varför & framför = ???? */

   x[++j] = pt2.x;
   y[j]   = pt2.y;
   a[j]   = a[i];
/*
***Loopa igenom resten av polylinjen.
*/
   while ( ++i <= *kmax )
     {
     pt1.x = sav0;
     pt1.y = sav1;
     pt2.x = sav0 = x[i];
     pt2.y = sav1 = y[i];
/*
***Klipp aktuell vektor. 0 => Vektorn helt innanför.
***                      2 => Ände 2 utanför.
*/
     switch ( msclin(gwinpt,&pt1,&pt2) )
       {
       case 0:
       case 2:
       x[++j] = pt2.x;
       y[j]   = pt2.y;
       a[j]   = a[i];            /* Detta är fel !!! */
       break;
/*
*** 1 => Ände 1 utanför.
*** 3 => Båda ändarna utanför. Vektorn klippt på två ställen.
*/
       case 1: 
       case 3:
       if ( ++j == i )
         { /* En koordinat blir två */    
         if ( ++i >= PLYMXV )
           { /* Om det finns plats, */
           *kmax = j;
           return(TRUE);
           }
         for ( l = ++(*kmax); l >= j; l-- )
           { /* fixa utrymme */
           x[l] = x[l-1];
           y[l] = y[l-1];
           a[l] = a[l-1];
           }
         }
       x[j]   = pt1.x;
       y[j]   = pt1.y;
       a[j]   = a[i-1] & ~VISIBLE;    /* Osynlig */
       x[++j] = pt2.x;
       y[j]   = pt2.y;
       a[j]   = a[i];
       break;
       }
     }
/*
***Uppdatera polylinjeslut.
*/
     *kmax = j;

     return(TRUE);
}

/********************************************************/
/*!******************************************************/

        int   mscpmk(win_id,ix,iy)
        v2int win_id;
        int   ix,iy;

/*      Ritar pekmärke och uppdaterar pekmärkes-listan.
 *
 *      In: win_id  => ID för fönster att rita i.
 *          ix,iy   => Pekmärkets position i fönstret.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C) microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster och rita där så
***begärts.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=mswgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         msdpmk(gwinpt,ix,iy,TRUE);
         if ( gwinpt->pmkant < WP_PMKMAX-1 )
           {
           gwinpt->pmktab[gwinpt->pmkant].x = ix;
           gwinpt->pmktab[gwinpt->pmkant].y = iy;
         ++gwinpt->pmkant;
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        int   msepmk(win_id)
        v2int win_id;

/*      Suddar alla pekmärken och nollställer pekmärkes-
 *      listan i det/de begärda fönstret/fönstren.
 *
 *      In: win_id => Fönster-ID eller GWIN_ALL.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C) microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i,j;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster och sudda där så
***begärts.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=mswgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         for ( j=0; j<gwinpt->pmkant; ++j )
           {
           msdpmk(gwinpt,(int)gwinpt->pmktab[j].x,
                         (int)gwinpt->pmktab[j].y,FALSE);
           }
         gwinpt->pmkant = 0;
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        int     msdpmk(gwinpt,ix,iy,draw)
        WPGWIN *gwinpt;
        int     ix,iy;
        bool    draw;

/*      Ritar pekmärke och uppdaterar pekmärkes-listan.
 *
 *      In: gwinpt  => Pekare till fönster att rita i.
 *          ix,iy   => Pekmärkets position i fönstret.
 *          draw    => TRUE = Rita, FALSE = Sudda.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C) microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   
   if      (  draw  &&  actpen != 1 ) msspen(1);
   else if ( !draw  &&  actpen != 0 ) msspen(0);

   MoveToEx(gwinpt->dc,ix,iy-1,NULL);
   LineTo(gwinpt->dc,ix,iy+1);

   MoveToEx(gwinpt->dc,ix-1,iy,NULL);
   LineTo(gwinpt->dc,ix+1,iy);

   return(0);
 }

/********************************************************/
#ifdef senare
/*!******************************************************/

        int     msscur(win_id,set,cursor)
        int     win_id;
        bool    set;
        Cursor  cursor;

/*      Ställer om aktiv cursor.
 *
 *      In: win_id => Fönster att byta cursor i.
 *          set    => TRUE = sätt, FALSE = återställ
 *          cursor => Om set, X-cursor.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C) microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=mswgwp((wpw_id)i)) != NULL  &&  winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Kanske skall vi byta i detta fönster ?
*
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         if ( set ) XDefineCursor(xdisp,gwinpt->id.x_id,cursor);
         else       XUndefineCursor(xdisp,gwinpt->id.x_id);
         }
         */
       }
     }

   return(0);
 }

/********************************************************/
#endif
