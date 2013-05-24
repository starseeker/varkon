/**********************************************************************
*
*    wpplylin.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPswdt();     Set active linewidth
*    WPmsiz();     Get model size
*    WPclin();     Clip line to window
*    WPcply();     Clip polyline to window
*    WPdply();     Draw/erase polyline
*    WPpply();     Project polyline to view
*    WPcpmk();     Create pickmarker
*    WPepmk();     Erase pickmarker
*    WPdpmk();     Draw pickmarker
*    WPscur();     Change graphical cursor
*    WPppos();     Projects a pos on the viewplane of a WPGWIN
*    WPcpos();     Clip a pos to the borders of a WPGWIN
*    WPset_cacc(); Set curve accuracy
*    WPget_cacc(); Get current curve accuracy
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

#ifdef WIN32
extern int msgrgb();
#endif

/*
***The current global value for curve accuracy is kept in curnog.
***Functions using this value access it through WPget_cacc() and
***WPset_cacc(). The value of curnog is used in polyline generation
***for curved entities to establish the number of polylines to use.
***Each such routine first establishes a suitable density of lines
***and then multiplies this number with curnog. User can set
***0.01<=curnog<=100.0 through IG. curnog can also be set
***by MBS routine. 
*/
static double curnog = 1.0;


/********************************************************/

        short  WPswdt(
        wpw_id win_id,
        double width)

/*      Sätter om aktiv linjebredd.
 *
 *      In: win_id = Fönster att sätta om bredd i
 *          width =  Önskad bredd eller 0.0 för
 *                   minsta möjliga.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1997-12-27 J. Kjellander
 *
 ********************************************************/

{
   int     npix;
   double  skala;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

#ifdef WIN32
   static bool     delflg = FALSE;
   static HPEN     oldpen,newpen;
   static COLORREF color;
#endif

#ifdef UNIX
   XFlush(xdisp);
#endif
/*
***Fixa C-pekare.
*/
   if ( (winptr=WPwgwp(win_id)) != NULL  &&  winptr->typ == TYP_GWIN ) 
     {
     gwinpt = (WPGWIN *)winptr->ptr;
/*
***Behöver bredden ändras ?
*/
     if ( width != gwinpt->linwdt )
       {
/*
***Ja, beräkna linjebredd i pixels.
*/
       if ( width == 0.0 ) npix = 0;
       else
         {
         skala = (gwinpt->geo.psiz_x *
                 (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin))/
                 (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
         npix  = (int)((skala*width)/((gwinpt->geo.psiz_x+gwinpt->geo.psiz_y)/2.0) + 0.5);
         }

#ifdef WIN32
       msgrgb(actpen,&color);
       newpen = CreatePen(PS_SOLID,npix,color);
       SelectObject(gwinpt->dc,newpen);
       SelectObject(gwinpt->bmdc,newpen);

       if ( delflg ) DeleteObject(oldpen);
       else          delflg = TRUE;

       oldpen = newpen;
#endif
#ifdef UNIX
       XSetLineAttributes(xdisp,gwinpt->win_gc,npix,
                                     LineSolid,CapRound,JoinRound);
#endif
       gwinpt->linwdt = width;
       }
     }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short   WPmsiz(
        WPGWIN *gwinpt,
        WPVIEW *pminvy)

/*      Calculate projected model 2D bounding rectangle.
 *
 *      In:   gwinpt => Ptr to window for projection.
 *
 *      Out: *pminvy => 2D bounding rectangle.
 *
 *      Return:  0 = OK.
 *
 *      (C)microform ab 17/1/95 J. Kjellander
 *
 *      2004-07-21 Mesh, J.Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *      2007-01-08 piso, pborder,  Sören L
 *      2007-04-23 Bug, text projection, J.Kjellander
 *      2007-08-28 Dim text, J.Kjellander
 *
 ******************************************************!*/

 {
    DBptr     la;
    DBetype   typ;
    int       k,i;
    char      str[V3STRLEN+1];
    double    x[PLYMXV],y[PLYMXV],z[PLYMXV];
    char      a[PLYMXV];
    double    size,crdvek[4*GMXMXL];
    DBId      dummy;
    DBPoint   poi;
    DBLine    lin;
    DBArc     arc;
    DBCurve   cur;
    DBSurf    sur;
    DBText    txt;
    DBHatch   xht;
    DBAny     dim;
    DBCsys    csy;
    DBBplane  bpl;
    DBMesh    msh;
    DBSeg    *segptr,arcseg[4];
    DBSegarr *pborder,*piso;
    WPGRPT    pt1,pt2;

/*
***The worlds smallest rectangle !
*/
    pminvy->modwin.xmin = pminvy->modwin.ymin =  1e20;
    pminvy->modwin.xmax = pminvy->modwin.ymax = -1e20;
/*
***Traverse DB.
*/
    DBget_pointer('F',&dummy,&la,&typ);

    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
/*
***For each entity, establish the bounding box of
***it's polyline projected to the view of the window.
*/
      k = -1;

      switch(typ)
        {
        case POITYP:
        DBread_point(&poi,la);
        if ( !poi.hed_p.blank  &&  WPnivt(gwinpt->nivtab,poi.hed_p.level) )
          {
          WPppos(gwinpt,&poi.crd_p,&pt1);
          x[0] = pt1.x;
          y[0] = pt1.y;
          k++;
          }
        break;

        case LINTYP:
        DBread_line(&lin,la);
        if ( !lin.hed_l.blank  &&  WPnivt(gwinpt->nivtab,lin.hed_l.level) )
          {
          WPppos(gwinpt,&lin.crd1_l,&pt1);
          x[0] = pt1.x; y[0] = pt1.y;
          WPppos(gwinpt,&lin.crd2_l,&pt2);
          x[1] = pt2.x; y[1] = pt2.y;
          k += 2;
          }
        break;

        case ARCTYP:
        DBread_arc(&arc,arcseg,la);
        if ( !arc.hed_a.blank  &&  WPnivt(gwinpt->nivtab,arc.hed_a.level) )
          {
          arc.fnt_a = 0;
          WPplar(&arc,arcseg,1.0,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case CURTYP:
        DBread_curve(&cur,&segptr,NULL,la);
        if ( !cur.hed_cu.blank  &&  WPnivt(gwinpt->nivtab,cur.hed_cu.level) )
          {
          cur.fnt_cu = 0;
          WPplcu(&cur,segptr,1.0,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        DBfree_segments(segptr);
        break;

        case SURTYP:
        DBread_surface(&sur,la);
        if ( !sur.hed_su.blank  &&  WPnivt(gwinpt->nivtab,sur.hed_su.level) )
          {
          if ( sur.typ_su == FAC_SUR )
            {
            k = -1;
            WPplsu(&sur,NULL,NULL,1.0,&k,x,y,z,a);
            }
          else
            {
            DBread_sur_grwire(&sur,&pborder,&piso);
            sur.lgt_su = 100000;
            k = -1;
            WPplsu(&sur,pborder,piso,1.0,&k,x,y,z,a);
            DBfree_sur_grwire(&sur,pborder,piso);
            }
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case TXTTYP:
        DBread_text(&txt,str,la);
        if ( !txt.hed_tx.blank  &&  WPnivt(gwinpt->nivtab,txt.hed_tx.level) )
          {
          txt.fnt_tx = 0;
          WPpltx(&txt,(unsigned char *)str,&k,x,y,z,a);
          WPprtx(gwinpt,&txt,k+1,x,y,z);
          }
        break;

        case XHTTYP:
        DBread_xhatch(&xht,crdvek,&csy,la);
        if ( !xht.hed_xh.blank  &&  WPnivt(gwinpt->nivtab,xht.hed_xh.level) )
          {
          WPplxh(&xht,crdvek,&csy,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case LDMTYP:
        DBread_ldim(&dim.ldm_un,&csy,la);
        if ( !dim.hed_un.blank  &&  WPnivt(gwinpt->nivtab,dim.hed_un.level) )
          {
          WPplld(&dim.ldm_un,&csy,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case CDMTYP:
        DBread_cdim(&dim.cdm_un,&csy,la);
        if ( !dim.hed_un.blank  &&  WPnivt(gwinpt->nivtab,dim.hed_un.level) )
          {
          WPplcd(&dim.cdm_un,&csy,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case RDMTYP:
        DBread_rdim(&dim.rdm_un,&csy,la);
        if ( !dim.hed_un.blank  &&  WPnivt(gwinpt->nivtab,dim.hed_un.level) )
          {
          WPplrd(&dim.rdm_un,&csy,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case ADMTYP:
        DBread_adim(&dim.adm_un,&csy,la);
        if ( !dim.hed_un.blank  &&  WPnivt(gwinpt->nivtab,dim.hed_un.level) )
          {
          WPplad(&dim.adm_un,&csy,1.0,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case CSYTYP:
        DBread_csys(&csy,NULL,la);
        if ( !csy.hed_pl.blank  &&  WPnivt(gwinpt->nivtab,csy.hed_pl.level) )
          {
          size = (gwinpt->vy.modwin.xmax-gwinpt->vy.modwin.xmin)/10.0;
          WPplcs(&csy,size,V3_CS_NORMAL,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case BPLTYP:
        DBread_bplane(&bpl,la);
        if ( !bpl.hed_bp.blank  &&  WPnivt(gwinpt->nivtab,bpl.hed_bp.level) )
          {
          WPplbp(&bpl,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;

        case MSHTYP:
        DBread_mesh(&msh,la,MESH_HEADER);
        msh.font_m = 1;
        if ( !msh.hed_m.blank  &&  WPnivt(gwinpt->nivtab,msh.hed_m.level) )
          {
          WPplms(&msh,1.0,&k,x,y,z,a);
          WPpply(gwinpt,k,x,y,z);
          }
        break;
        }
/*
***Adjust the size of the rectangle.
*/
    for ( i=0; i<=k; ++i )
      {
      if ( x[i] > pminvy->modwin.xmax ) pminvy->modwin.xmax = x[i];
      if ( x[i] < pminvy->modwin.xmin ) pminvy->modwin.xmin = x[i];

      if ( y[i] > pminvy->modwin.ymax ) pminvy->modwin.ymax = y[i];
      if ( y[i] < pminvy->modwin.ymin ) pminvy->modwin.ymin = y[i];
      }
/*
***Next entity.
*/
    }
/*
***The end.
*/
    return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPclin(
        VYWIN  *clipwin,
        WPGRPT *pt1,
        WPGRPT *pt2)

/*      Klipper en grafisk linje mot ett fönster.
 *
 *      In: clipwin  => Pekare till klippfönster.
 *          pt1,pt2  => Pekare till indata/utdata.
 *
 *      Ut: *pt1,*pt2 = Ändpunkter, ev. klippta.
 *
 *      FV: -1 => Linjen är oklippt och ligger utanför fönstret
 *           0 => Linjen är oklippt och ligger innanför fönstret.
 *           1 => Linjen är klippt i pt1 (start).
 *           2 => Linjen är klippt i pt2 (slut).
 *           3 => Linjen är klippt i både pt1 och pt2.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 ******************************************************!*/

 {
   short  sts1,sts2;
   double p1x,p1y,p2x,p2y,xmin,xmax,ymin,ymax;

   xmin = clipwin->xmin;
   ymin = clipwin->ymin;
   xmax = clipwin->xmax;
   ymax = clipwin->ymax;

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

   bool         WPcply(
        VYWIN  *clipwin,
        int     kmin,
        int    *kmax,
        double  x[],
        double  y[],
        char    a[])

/*      Klipper en polylinje mot ett visst fönster.
*
*       In: clipwin => Pekare till klippfönster.
*           kmin+1  => Offset till polylinjestart.
*           kmax    => Offset till polylinjeslut.
*           x       => X-koordinater.
*           y       => Y-koordinater.
*           a       => Tänd/Släck.
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
*       (C)microform ab 31/12/94 J. Kjellander
*
*       2007-04-08 1.19, J.Kjellander
*
********************************************************!*/

{
   int    i,j,l;
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
     } while ( WPclin(clipwin,&pt1,&pt2) == -1 );
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
     switch ( WPclin(clipwin,&pt1,&pt2) )
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
/********************************************************/

        short   WPdply(
        WPGWIN *gwinpt,
        int     k,
        double  x[],
        double  y[],
        char    a[],
        bool    s)

/*      Draw or erase a polyline.
 *
 *      In: gwinpt => Pekare till fönster att rita i.
 *          k      => Antal vektorer
 *          x      => X-koordinater
 *          y      => Y-koordinater
 *          a      => Pen up/down.
 *          s      => TRUE = Draw, FALSE = Erase.
 *
 *      Felkod: WP1373 = Otillåtet antal vektorer.
 *
 *      (C) microform ab 30/12/94 J. Kjellander
 *
 *      1996-12-17 WIN32, J.Kjellander
 *
 *********************************************************/

  {
   int    i,np;

#ifdef UNIX
   XPoint ip[PLYMXV];
#endif
#ifdef WIN32
   POINT  ip[PLYMXV];
#endif

/*
***Initiering.
*/
   np = 0;
/*
***Felkontroll.
*/
   if ( k > PLYMXV-2  ||  k < 1 ) return(erpush("WP1373",""));
/*
***Rita. Observera att transformationen modellkoordinat till
***skärmkoordinat bör göras på samma sätt här som i WPsply().
***Olika metoder double/int/short kan avrunda till olika pixels.
*/
   if ( s )
     {
     for ( i=0; i<=k; i++ )
       {
       if ( (a[i] & VISIBLE) == VISIBLE ) 
         {
         ip[np].x = (short)(gwinpt->vy.k1x + gwinpt->vy.k2x*x[i]);
         ip[np].y = gwinpt->geo.dy -
                    (short)(gwinpt->vy.k1y + gwinpt->vy.k2y*y[i]);
         np++;
         }
        else
         {
         if ( np > 0 )
           {
#ifdef UNIX
           XDrawLines(xdisp,gwinpt->id.x_id,
                            gwinpt->win_gc,ip,np,CoordModeOrigin);
           XDrawLines(xdisp,gwinpt->savmap,
                            gwinpt->win_gc,ip,np,CoordModeOrigin);
#endif
#ifdef WIN32
           Polyline(gwinpt->dc,ip,np);
           Polyline(gwinpt->bmdc,ip,np);
#endif
           np = 0;
           }
         ip[np].x = (short)(gwinpt->vy.k1x + gwinpt->vy.k2x*x[i]);
         ip[np].y = gwinpt->geo.dy -
                    (short)(gwinpt->vy.k1y + gwinpt->vy.k2y*y[i]);
         np++;
         }
       }
     if ( np > 0 )
       {
#ifdef UNIX
       XDrawLines(xdisp,gwinpt->id.x_id,
                        gwinpt->win_gc,ip,np,CoordModeOrigin);
       XDrawLines(xdisp,gwinpt->savmap,
                        gwinpt->win_gc,ip,np,CoordModeOrigin);
#endif
#ifdef WIN32
       Polyline(gwinpt->dc,ip,np);
       Polyline(gwinpt->bmdc,ip,np);
#endif
       np = 0;
       }
     }
/*
***Sudda. Här gör vi return direkt utan att tömma buffrar
***eftersom annars buffrar kommer att tömmas två gånger varav
***ena gången tom.
*/
   else
     {
     if ( actpen != 0 ) WPspen(0);
     return(WPdply(gwinpt,k,x,y,a,TRUE));
     }
/*
***Töm outputbufferten.
*/
#ifdef UNIX
   XFlush(xdisp);
#endif

   return(0);
  }

/********************************************************/
/********************************************************/

        short   WPpply(
        WPGWIN *gwinpt,
        int     k,
        double  x[],
        double  y[],
        double  z[])

/*      Project 3D polyline to 2D view of WPGWIN.
 *
 *      In: gwinpt = C-ptr to WPGWIN
 *          k      = Offset to last point in polyline
 *          x      = X-coordinates
 *          y      = Y-coordinates
 *          z      = Z-coordinates
 *
 *      Return: X- and Y-coordinates projected.
 *
 *      (C)2006-12-10 J. Kjellander
 *
 *********************************************************/

  {
   int    i;
   double xt,yt,zt,d,dz;

/*
***Transform all X- and Y in the polyline.
*/
     for ( i=0; i<=k; i++ )
       {
       xt = gwinpt->vy.matrix.k11 * x[i] +
            gwinpt->vy.matrix.k12 * y[i] +
            gwinpt->vy.matrix.k13 * z[i];

       yt = gwinpt->vy.matrix.k21 * x[i] +
            gwinpt->vy.matrix.k22 * y[i] +
            gwinpt->vy.matrix.k23 * z[i];
/*
***If the view is a perspective view we must also transform Z.
*/
       if ( gwinpt->vy.pdist > 0 )
         {
         zt = gwinpt->vy.matrix.k31 * x[i] +
              gwinpt->vy.matrix.k32 * y[i] +
              gwinpt->vy.matrix.k33 * z[i];

         d  = gwinpt->vy.pdist;
         dz = d - zt;

         if ( dz > 0.0 )
           {
           xt /= dz; xt *= d;
           yt /= dz; yt *= d;
           }
         else return(FALSE);
         }
       x[i] = xt;
       y[i] = yt;
       }

   return(0);
  }

/********************************************************/
/********************************************************/

   short WPfply(
        WPGWIN *gwinpt,
        int     k,
        double  x[],
        double  y[],
        bool    draw)

/*
 *      Fill/erase area enclosed by polygon.
 *
 *      In:
 *         k:   Number of lines (vertices - 1)
 *         x:   X-coordinates
 *         y:   Y-coordinates
 *         pen: Color number
 *
 *      (C)microform ab 1999-12-13 J.Kjellander
 *
 *********************************************************/

  {
/*
***X version.
*/
#ifdef UNIX
   int    i;
   XPoint p[PLYMXV+1];

/*
***Draw ?
*/
   if ( draw )
     {
/*
***Make XPoint array.
*/
     for ( i=0; i<=k; ++i )
       {
       p[i].x = (short)(gwinpt->vy.k1x + gwinpt->vy.k2x*x[i]);
       p[i].y = (short)(gwinpt->geo.dy -
                       (gwinpt->vy.k1y + gwinpt->vy.k2y*y[i]));
       }

     XFillPolygon(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
                  p,(int)(k+1),Nonconvex,CoordModeOrigin);
     XFillPolygon(xdisp,gwinpt->savmap,gwinpt->win_gc,
                  p,(int)(k+1),Nonconvex,CoordModeOrigin);
     }
/*
***Erase. Return from recursion here to eliminate
***double flushing.
*/
   else
     {
     if ( actpen != 0 ) WPspen(0);
     return(WPfply(gwinpt,k,x,y,TRUE));
     }

   XFlush(xdisp);
#endif

/*
***WIN32 version.
*/
#ifdef WIN32
   int   i;
   POINT p[PLYMXV+1];

/*
***Draw ?
*/
   if ( draw )
     {
/*
***What color ?
*/
   SelectObject(gwinpt->dc,msgbrush((int)actpen));
   SelectObject(gwinpt->bmdc,msgbrush((int)actpen));
/*
***Make POINT array.
*/
     for ( i=0; i<=k; ++i )
       {
       p[i].x = (long)(gwinpt->vy.k1x + gwinpt->vy.k2x*x[i]);
       p[i].y = (long)(gwinpt->geo.dy -
                      (gwinpt->vy.k1y + gwinpt->vy.k2y*y[i]));
       }
/*
***Fill polygon.
*/
     Polygon(gwinpt->dc,p,(int)(k+1));
     Polygon(gwinpt->bmdc,p,(int)(k+1));
     }
/*
***Erase.
*/
   else
     {
     if ( actpen != 0 ) WPspen(0);
     WPfply(gwinpt,k,x,y,TRUE);
     }
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPcpmk(
        DBint win_id,
        int   ix,
        int   iy)

/*      Ritar pekmärke och uppdaterar pekmärkes-listan.
 *
 *      In: win_id  => ID för fönster att rita i.
 *          ix,iy   => Pekmärkets position i fönstret.
 *
 *      (C) microform ab 19/1/95 J. Kjellander
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
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         WPdpmk(gwinpt,ix,iy,TRUE);
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

        short WPepmk(
        DBint win_id)

/*      Suddar alla pekmärken och nollställer pekmärkes-
 *      listan i det/de begärda fönstret/fönstren.
 *
 *      In: win_id => Fönster-ID eller GWIN_ALL.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C) microform ab 19/1/95 J. Kjellander
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
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         for ( j=0; j<gwinpt->pmkant; ++j )
           {
           WPdpmk(gwinpt,(int)gwinpt->pmktab[j].x,
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

        short WPdpmk(
        WPGWIN *gwinpt,
        int     ix,
        int     iy,
        bool    draw)

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
 *      (C) microform ab 19/1/95 J. Kjellander
 *
 *      1996-12-17 WIN32, J.Kjellander
 *
 ******************************************************!*/

 {
   
   if      (  draw  &&  actpen != 1 ) WPspen(1);
   else if ( !draw  &&  actpen != 0 ) WPspen(0);

#ifdef UNIX
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,ix,iy-1,ix,iy+1);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,ix,iy-1,ix,iy+1);
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,ix-1,iy,ix+1,iy);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,ix-1,iy,ix+1,iy);
   XFlush(xdisp);
#endif
#ifdef WIN32
   MoveToEx(gwinpt->dc,ix,iy-1,NULL);
   LineTo(gwinpt->dc,ix,iy+1);
   MoveToEx(gwinpt->dc,ix-1,iy,NULL);
   LineTo(gwinpt->dc,ix+1,iy);
#endif

   return(0);
 }

/********************************************************/
#ifdef UNIX
/*!******************************************************/

        short WPscur(
        int     win_id,
        bool    set,
        Cursor  cursor)

/*      Sets/unsets active mouse cursor.
 *
 *      In: win_id => Window ID or GWIN_ALL
 *          set    => TRUE or FALSE
 *          cursor => X-cursor
 *
 *      (C) microform ab 23/1/95 J. Kjellander
 *
 *      1998-10-29 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Loop through alla WPGWIN/WPRWIN windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL )
       {
/*
***WPGWIN.
*/
       if (  winptr->typ == TYP_GWIN ) 
         {
         gwinpt = (WPGWIN *)winptr->ptr;
         if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
           {
           if ( set ) XDefineCursor(xdisp,gwinpt->id.x_id,cursor);
           else       XUndefineCursor(xdisp,gwinpt->id.x_id);
           }
         }
/*
***WPRWIN.
*/
       else if (  winptr->typ == TYP_RWIN ) 
         {
         rwinpt = (WPRWIN *)winptr->ptr;
         if ( win_id == GWIN_ALL  ||  rwinpt->id.w_id )
           {
           if ( set ) XDefineCursor(xdisp,rwinpt->id.x_id,cursor);
           else       XUndefineCursor(xdisp,rwinpt->id.x_id);
           }
         }
       }
     }
/*
***Flush X.
*/
   XFlush(xdisp);
/*
***The end.
*/
   return(0);
 }

/********************************************************/
#endif
/*!******************************************************/

        bool       WPppos(
        WPGWIN    *gwinpt,
        DBVector  *po,
        WPGRPT    *pt)

/*      Projects a position on the viewplane of a WPGWIN.
 *
 *      In:   gwinpt => C-ptr to WPGWIN.
 *            po     => C-ptr to 3D position.
 *
 *      Out: *pt     => Position transformed.
 *
 *      Return: TRUE  => OK.
 *              FALSE => Z-clip during perspective transformation.
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
   double d,dz;

   pt->x = gwinpt->vy.matrix.k11 * po->x_gm +
           gwinpt->vy.matrix.k12 * po->y_gm +
           gwinpt->vy.matrix.k13 * po->z_gm;

   pt->y = gwinpt->vy.matrix.k21 * po->x_gm +
           gwinpt->vy.matrix.k22 * po->y_gm +
           gwinpt->vy.matrix.k23 * po->z_gm;
/*
***If the view is a perspective view we must also transform Z.
*/
   if ( gwinpt->vy.pdist > 0 )
     {
     pt->z = gwinpt->vy.matrix.k31 * po->x_gm +
             gwinpt->vy.matrix.k32 * po->y_gm +
             gwinpt->vy.matrix.k33 * po->z_gm;

     d  = gwinpt->vy.pdist;
     dz = d - pt->z;

     if ( dz > 0.0 )
       {
       pt->x /= dz; pt->x *= d;
       pt->y /= dz; pt->y *= d;
       }
     else return(FALSE);
     }
/*
***The end.
*/
   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

        bool    WPcpos(
        WPGWIN *gwinpt,
        WPGRPT *pt)

/*      Clip a graphical point to the borders of a 
 *      WPGWIN.
 *
 *      In:  gwinpt => C-ptr to WPGWIN.
 *           pt     => C-ptr to graphical point.
 *
 *      FV: TRUE  => Point visible.
 *          FALSE => Point outside.
 *
 *      (C)2006-12-30 J. Kjellander
 *
 ******************************************************!*/

 {
     return ((pt->x > gwinpt->vy.modwin.xmin ) &&
             (pt->x < gwinpt->vy.modwin.xmax ) &&
             (pt->y > gwinpt->vy.modwin.ymin ) &&
             (pt->y < gwinpt->vy.modwin.ymax ));
 }

/********************************************************/
/*!******************************************************/

        void   WPset_cacc(
        double newacc)

/*      Sets curve accuracy for polyline generation.
 *
 *      In: newacc = 0.01<= The new curve accuracy <=100.0
 *
 *      (C)2006-12-31 J. Kjellander
 *
 ******************************************************!*/

  {
    double c;

    if      ( newacc < 0.01 )  c = 0.01;
    else if ( newacc > 100.0 ) c = 100.0;
    else                       c = newacc;

    curnog = c;
  }

/********************************************************/
/*!******************************************************/

        void    WPget_cacc(
        double *caccpt)

/*      Returns current curve accuracy.
 *
 *      Out: *caccpt => Curve accuracy
 *
 *      (C)2006-12-31 J. Kjellander
 *
 ******************************************************!*/

  {
   *caccpt = curnog;
  }

/********************************************************/
