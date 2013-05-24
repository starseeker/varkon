/**********************************************************************
*
*    wpxhatch.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
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
#include "../../IG/include/IG.h"
#include "../include/WP.h"

extern int actpen;

static short drawxh(WPGWIN *gwinpt, DBHatch *xhtpek, DBfloat crdvek[],
                    DBptr la, bool draw);

/*!******************************************************/

        short    WPdrxh(
        DBHatch *xhtpek,
        DBfloat  crdvek[],
        DBptr    la,
        DBint    win_id)

/*      Display a hatch.
 *
 *      In: xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
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
***Ja, ligger snittet på en nivå som är tänd i detta fönster ?
*/
         if ( WPnivt(gwinpt->nivtab,xhtpek->hed_xh.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( xhtpek->hed_xh.pen != actpen ) WPspen(xhtpek->hed_xh.pen);
/*
***Sen är det bara att rita.
*/
           drawxh(gwinpt,xhtpek,crdvek,la,TRUE);
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
        DBptr    la,
        DBint    win_id)

/*      Suddar ett snitt.
 *
 *      In: xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
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
***Ja. Om den finns i DF och det är heldraget kan vi sudda snabbt.
*/
         if ( WPfobj(gwinpt,la,XHTTYP,&typ) )
           {
           if ( xhtpek->fnt_xh == 0 ) WPdobj(gwinpt,FALSE);
           else
             {
             WProbj(gwinpt);
             if ( !WPnivt(gwinpt->nivtab,xhtpek->hed_xh.level)  ||
                                 xhtpek->hed_xh.blank) return(0);
             drawxh(gwinpt,xhtpek,crdvek,la,FALSE);
             }
           }
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !WPnivt(gwinpt->nivtab,xhtpek->hed_xh.level)  ||
                               xhtpek->hed_xh.blank) return(0);
           drawxh(gwinpt,xhtpek,crdvek,la,FALSE);
           }
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
        DBptr    la,
        bool     draw)

/*      Ritar/suddar ett snitt ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 26/1/95 J. Kjellander
 *
 *      2006-12-17 WPplli(), J.Kjellander
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
   WPplxh(xhtpek,crdvek,&n,x,y,z,a);
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
***End.
*/
   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short    WPplxh(
        DBHatch *xhtptr,
        DBfloat  crdvek[],
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
 ******************************************************!*/

 {
   int    i,k,nl;
   DBLine lin;
 
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
     lin.crd2_l.x_gm = crdvek[i++];
     lin.crd2_l.y_gm = crdvek[i++];
     WPplli(&lin,&k,x,y,z,a); 
     }

   *n = k;
/*
***End.
*/
    return(0);
 }

/********************************************************/
