/**********************************************************************
*
*    wp18.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpnivt();    Check if level is visible
*    wpupgp();    Uppdate GP actvy, actvym etc.
*    wpfixg();    Fix  GP variables for vector generation 
*    wpspen();    Set active pen
*    wpswdt();    Set active linewidth
*    wpmsiz();    Get model size
*    wpclin();    Clip line to window
*    wpcply();    Clip polyline to window
*    wpdply();    Draw/erase polyline
*    wpcpmk();    Create pickmarker
*    wpepmk();    Erase pickmarker
*    wpdpmk();    Draw pickmarker
*    wpscur();    Change grapfical cursor
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GP/include/GP.h"
#include "../include/WP.h"

extern short actpen;

#ifdef WIN32
extern int msgrgb();
#endif

/********************************************************/

        bool wpnivt(
        WPGWIN *gwinpt,
        short   level)

/*      Kollar om viss nivå i visst fönster är tänd.
 *
 *      In: gwinpt => Pekare till fönster.
 *          level  => Nivå.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Nivån är tänd.
 *           FALSE = Nivån är släckt.
 *
 *      (C)microform ab 15/1/95 J. Kjellander
 *
 ********************************************************/

 {
   int  bytofs,bitmsk;

   bytofs = (int)level;
   bytofs = bytofs>>3;
   bitmsk = 1;
   bitmsk = bitmsk<<(level&7);
   if ( gwinpt->nivtab[bytofs] & bitmsk ) return(TRUE);
   else                                   return(FALSE);
 }

/********************************************************/
/********************************************************/

        short wpupgp(
        WPGWIN *gwinpt)

/*      Uppdaterar grapac:s actwin och actvym mm.
 *
 *      In: gwinpt => Pekare till fönster.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/1/95 J. Kjellander
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
   actvy.vy3d     = gwinpt->vy.vy_3D;
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

        short wpfixg(
        WPGWIN *gwinpt)

/*      Sätter om globala variabler i grapac så att
 *      grapac:s vektorgenereringsrutiner kan användas
 *      av winpac för WPGWIN-fönster.
 *
 *      In: gwinpt => Pekare till fönster eller NULL för
 *                    återställning av ursprungliga data.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1/1/95 J. Kjellander
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
     actvy.vy3d     = gwinpt->vy.vy_3D;
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

        short wpspen(
        short pen)

/*      Sätter om aktiv penna, dvs. färg.
 *
 *      In: pen = Pennummer.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 31/12/94 J. Kjellander
 *
 *       1997-12-26 Separata GC, J.Kjellander
 *
 ********************************************************/

{
   int     i;
   WPGWIN *gwinpt;

/*
***Gå igenom alla grafiska fönster och byt förgrundsfärg
***i deras respektive GC/DC.
*/
#ifdef V3_X11
   XFlush(xdisp);
#endif
   if ( pen != actpen )
     {
     for ( i=0; i<WTABSIZ; ++i)
       {
       if ( wpwtab[i].ptr != NULL )
         {
         if ( wpwtab[i].typ == TYP_GWIN )
           {
           gwinpt = (WPGWIN *)wpwtab[i].ptr;
#ifdef WIN32
           SelectObject(gwinpt->dc,msgcol(pen));
           SelectObject(gwinpt->bmdc,msgcol(pen));
#endif
#ifdef V3_X11
           XSetForeground(xdisp,gwinpt->win_gc,wpgcol(pen));
#endif
           }
         }
       }
     actpen = pen;
     }

    return(0);
}

/********************************************************/
/********************************************************/

        short  wpswdt(
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

#ifdef V3_X11
   XFlush(xdisp);
#endif
/*
***Fixa C-pekare.
*/
   if ( (winptr=wpwgwp(win_id)) != NULL  &&  winptr->typ == TYP_GWIN ) 
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
#ifdef V3_X11
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

        short wpmsiz(
        WPGWIN *gwinpt,
        VY     *pminvy)

/*      Beräknar max och min-koordinater för modell
 *      i visst fönster.
 *
 *      In: gwinpt => Pekare till aktuellt fönster.
 *          pminvy => Pekare till utdata.
 *
 *      Ut: *pminvy = Vy som rymmer hela modellen.
 *
 *      FV:  0 = OK.
 *
 *      (C)microform ab 17/1/95 J. Kjellander
 *
 *      2004-07-21 Mesh, J.Kjellander
 *
 ******************************************************!*/

 {
    DBptr   la;
    DBetype typ;
    int     k,i,n,ncrdxy;
    char    str[V3STRLEN+1];
    double  x[PLYMXV],y[PLYMXV],z[PLYMXV];
    char    a[PLYMXV];
    gmflt   crdvek[4*GMXMXL];
    DBId    dummy;
    GMPOI   poi;
    GMLIN   lin;
    GMARC   arc;
    GMCUR   cur;
    GMSUR   sur;
    GMTXT   txt;
    GMXHT   xht;
    GMUNON  dim;
    GMCSY   csy;
    GMBPL   bpl;
    DBMesh  msh;
    DBTmat  pmat;
    GMSEG  *sptarr[6],*segptr,arcseg[4];
    WPGRPT  pt1,pt2;

/*
***Ställ om grapac.
*/
    wpfixg(gwinpt);
/*
***Sätt upp världens minsta fönster.
*/
    pminvy->vywin[0] = pminvy->vywin[1] =  1e20;
    pminvy->vywin[2] = pminvy->vywin[3] = -1e20;
/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av GM.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
/*
***Eftersom det här kan ta tid kollar vi med avbrottssystemet
***om det är läge att fortsätta.
*/
#ifdef V3_X11
      if ( wpintr() ) return(AVBRYT);
#endif
/*
***För varje storhet, generera x,y på snabbaste
***sätt.
*/
      ncrdxy = 0;

      switch(typ)
        {
        case POITYP:
        DBread_point(&poi,la);
        if ( !poi.hed_p.blank  &&  wpnivt(gwinpt,poi.hed_p.level) )
          {
          wptrpo(gwinpt,&poi.crd_p,&pt1);
          x[0] = pt1.x;
          y[0] = pt1.y;
          ncrdxy = 1;
          }
        break;

        case LINTYP:
        DBread_line(&lin,la);
        if ( !lin.hed_l.blank  &&  wpnivt(gwinpt,lin.hed_l.level) )
          {
          wptrpo(gwinpt,&lin.crd1_l,&pt1);
          x[0] = pt1.x; y[0] = pt1.y;
          wptrpo(gwinpt,&lin.crd2_l,&pt2);
          x[1] = pt2.x; y[1] = pt2.y;
          ncrdxy = 2;
          }
        break;

        case ARCTYP:
        DBread_arc(&arc,arcseg,la);
        if ( !arc.hed_a.blank  &&  wpnivt(gwinpt,arc.hed_a.level) )
          {
          arc.fnt_a = 0;
          k = -1;
          gpplar(&arc,arcseg,&k,x,y,a);
          ncrdxy = k + 1;
          }
        break;

        case CURTYP:
        DBread_curve(&cur,&segptr,NULL,la);
        if ( !cur.hed_cu.blank  &&  wpnivt(gwinpt,cur.hed_cu.level) )
          {
          cur.fnt_cu = 0;
          k = -1;
          gpplcu(&cur,segptr,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        DBfree_segments(segptr);
        break;

        case SURTYP:
        DBread_surface(&sur,la);
        DBread_srep_curves(&sur,sptarr);
        if ( !sur.hed_su.blank  &&  wpnivt(gwinpt,sur.hed_su.level) )
          {
          sur.lgt_su = 100000;
          k = -1;
          gpplsu(&sur,sptarr,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        DBfree_srep_curves(sptarr);
        break;

        case TXTTYP:
        DBread_text(&txt,str,la);
        if ( !txt.hed_tx.blank  &&  wpnivt(gwinpt,txt.hed_tx.level) )
          {
          k = -1;
          gppltx(&txt,(unsigned char *)str,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        break;

        case XHTTYP:
        DBread_xhatch(&xht,crdvek,la);
        if ( !xht.hed_xh.blank  &&  wpnivt(gwinpt,xht.hed_xh.level) )
          {
          k = -1;
          i =  0;
          n =  4*xht.nlin_xh;
  
          while ( i < n )
            {
            x[++k] = crdvek[i++];
            y[k]   = crdvek[i++];
            x[++k] = crdvek[i++];
            y[k]   = crdvek[i++];
            }
          ncrdxy = k + 1;
          }
        break;

        case LDMTYP:
        DBread_ldim(&dim.ldm_un,la);
        if ( !dim.hed_un.blank  &&  wpnivt(gwinpt,dim.hed_un.level) )
          {
          dim.ldm_un.auto_ld = FALSE;
          k = -1;
          x[0] = dim.ldm_un.p3_ld.x_gm;
          y[0] = dim.ldm_un.p3_ld.y_gm;
          gpplld(&dim.ldm_un,&k,x,y,a);
          ncrdxy = k + 1;
          }
        break;

        case CDMTYP:
        DBread_cdim(&dim.cdm_un,la);
        if ( !dim.hed_un.blank  &&  wpnivt(gwinpt,dim.hed_un.level) )
          {
          dim.cdm_un.auto_cd = FALSE;
          k = -1;
          x[0] = dim.cdm_un.p3_cd.x_gm;
          y[0] = dim.cdm_un.p3_cd.y_gm;
          gpplcd(&dim.cdm_un,&k,x,y,a);
          ncrdxy = k + 1;
          }
        break;

        case RDMTYP:
        DBread_rdim(&dim.rdm_un,la);
        if ( !dim.hed_un.blank  &&  wpnivt(gwinpt,dim.hed_un.level) )
          {
          dim.rdm_un.auto_rd = FALSE;
          k = -1;
          gpplrd(&dim.rdm_un,&txt,&k,x,y,a);
          ncrdxy = k + 1;
          }
        break;

        case ADMTYP:
        DBread_adim(&dim.adm_un,la);
        if ( !dim.hed_un.blank  &&  wpnivt(gwinpt,dim.hed_un.level) )
          {
          dim.adm_un.auto_ad = FALSE;
          k = -1;
          gpplad(&dim.adm_un,&txt,&k,x,y,a);
          ncrdxy = k + 1;
          }
        break;

        case CSYTYP:
        DBread_csys(&csy,&pmat,la);
        if ( !csy.hed_pl.blank  &&  wpnivt(gwinpt,csy.hed_pl.level) )
          {
          k = -1;
          wpplcs(gwinpt,&csy,&pmat,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        break;

        case BPLTYP:
        DBread_bplane(&bpl,la);
        if ( !bpl.hed_bp.blank  &&  wpnivt(gwinpt,bpl.hed_bp.level) )
          {
          k = -1;
          wpplbp(gwinpt,&bpl,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        break;

        case MSHTYP:
        DBread_mesh(&msh,la,MESH_HEADER);
        msh.font_m = 1;
        if ( !msh.hed_m.blank  &&  wpnivt(gwinpt,msh.hed_m.level) )
          {
          k = -1;
          wpplms(gwinpt,&msh,&k,x,y,z,a);
          ncrdxy = k + 1;
          }
        break;
        }
/*
***Sök igenom x och y efter max och min.
*/
    for ( i=0; i<ncrdxy; ++i )
      {
      if ( x[i] > pminvy->vywin[2] ) pminvy->vywin[2] = x[i];
      if ( x[i] < pminvy->vywin[0] ) pminvy->vywin[0] = x[i];

      if ( y[i] > pminvy->vywin[3] ) pminvy->vywin[3] = y[i];
      if ( y[i] < pminvy->vywin[1] ) pminvy->vywin[1] = y[i];
      }
/*
***Nästa storhet.
*/
    }
/*
***Återställ grapac.
*/
    wpfixg(NULL);

    return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpclin(
        WPGWIN *gwinpt,
        WPGRPT *pt1,
        WPGRPT *pt2)

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
 *      (C) microform ab 27/12/94 J. Kjellander
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

   bool wpcply(
        WPGWIN  *gwinpt,
        int      kmin,
        int     *kmax,
        double   x[],
        double   y[],
        char     a[])

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
*       (C)microform ab 31/12/94 J. Kjellander
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
     } while ( wpclin(gwinpt,&pt1,&pt2) == -1 );
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
     switch ( wpclin(gwinpt,&pt1,&pt2) )
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

  short wpdply(
        WPGWIN *gwinpt,
        int     k,
        double  x[],
        double  y[],
        char    a[],
        bool    s)

/*      Ritar (suddar) en polylinje.
 *
 *      In: gwinpt => Pekare till fönster att rita i.
 *          k      => Antal vektorer
 *          x      => X-koordinater
 *          y      => Y-koordinater
 *          a      => Tänd/Släck.
 *          s      => TRUE = Rita (FALSE = Sudda).
 *
 *      Ut: Inget.
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

#ifdef V3_X11
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
***skärmkoordinat bör göras på samma sätt här som i wpsply().
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
#ifdef V3_X11
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
#ifdef V3_X11
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
     if ( actpen != 0 ) wpspen(0);
     return(wpdply(gwinpt,k,x,y,a,TRUE));
     }
/*
***Töm outputbufferten.
*/
#ifdef V3_X11
   XFlush(xdisp);
#endif

   return(0);
  }

/********************************************************/
/********************************************************/

   short wpfply(
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
#ifdef V3_X11
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
     if ( actpen != 0 ) wpspen(0);
     return(wpfply(gwinpt,k,x,y,TRUE));
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
     if ( actpen != 0 ) wpspen(0);
     wpfply(gwinpt,k,x,y,TRUE);
     }
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpcpmk(
        v2int win_id,
        int   ix,
        int   iy)

/*      Ritar pekmärke och uppdaterar pekmärkes-listan.
 *
 *      In: win_id  => ID för fönster att rita i.
 *          ix,iy   => Pekmärkets position i fönstret.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
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
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         wpdpmk(gwinpt,ix,iy,TRUE);
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

        short wpepmk(
        v2int win_id)

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
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
         for ( j=0; j<gwinpt->pmkant; ++j )
           {
           wpdpmk(gwinpt,(int)gwinpt->pmktab[j].x,
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

        short wpdpmk(
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
   
   if      (  draw  &&  actpen != 1 ) wpspen(1);
   else if ( !draw  &&  actpen != 0 ) wpspen(0);

#ifdef V3_X11
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,ix,iy-1,ix,iy+1);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,ix,iy-1,ix,iy+1);
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,ix-1,iy,ix+1,iy);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,ix-1,iy,ix+1,iy);
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
#ifdef V3_X11
/*!******************************************************/

        short wpscur(
        int     win_id,
        bool    set,
        Cursor  cursor)

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
***Loopa igenom alla WPGWIN/WPRWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL )
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
         if ( win_id == rwinpt->id.w_id )
           {
           if ( set ) XDefineCursor(xdisp,rwinpt->id.x_id,cursor);
           else       XUndefineCursor(xdisp,rwinpt->id.x_id);
           XFlush(xdisp);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
#endif
