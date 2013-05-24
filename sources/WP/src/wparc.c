/**********************************************************************
*
*    wparc.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrar();     Draw arc
*    WPdlar();     Delete arc
*    WPplar();     Create 3D polyline
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

extern int actpen;  /* Currently active pen number */

static short drawar(WPGWIN *gwinpt, DBArc *arcpek, DBSeg *segpek,
                    DBptr la, bool draw);

/*!******************************************************/

        short   WPdrar(
        DBArc  *arcpek,
        DBSeg  *segpek,
        DBptr   la,
        DBint   win_id)

/*      Display an arc.
 *
 *      In: arcpek => C-ptr to DBArc.
 *          segpek => Pekare till DBSeg.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-27 Breda linjer,J.Kjellander
 *      2006-12-09 Removed gpdrar(), J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om cirkeln är blankad gör vi inget alls !
*/
   if ( arcpek->hed_a.blank) return(0);
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
***Ja, ligger cirkel på en nivå som är tänd i detta fönster ?
*/
         if ( WPnivt(gwinpt->nivtab,arcpek->hed_a.level) )
           {
/*
***Ja. Kolla att rätt färg och bredd är inställd.
*/
           if ( arcpek->hed_a.pen != actpen ) WPspen(arcpek->hed_a.pen);
           if ( arcpek->wdt_a != 0.0 ) WPswdt(gwinpt->id.w_id,arcpek->wdt_a);
/*
***Sen är det bara att rita.
*/
           drawar(gwinpt,arcpek,segpek,la,TRUE);
           if ( arcpek->wdt_a != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPdlar(
        DBArc  *arcpek,
        DBSeg  *segpek,
        DBptr   la,
        DBint   win_id)

/*      Suddar en arc.
 *
 *      In: arcpek => Pekare till arc-post.
 *          segpek => Pekare till DBSeg.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-27 Breda linjer,J.Kjellander
 *      2006-12-09 Removed gpdlar(), J.Kjellander
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
***Ja. Stryk ur DF.
*/
         if ( WPfobj(gwinpt,la,ARCTYP,&typ) ) WProbj(gwinpt);
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda från skärm.
*/
         if ( !WPnivt(gwinpt->nivtab,arcpek->hed_a.level)  ||
                             arcpek->hed_a.blank) return(0);
           
         if ( arcpek->wdt_a != 0.0 ) WPswdt(gwinpt->id.w_id,arcpek->wdt_a);
         drawar(gwinpt,arcpek,segpek,la,FALSE);
         if ( arcpek->wdt_a != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawar(
        WPGWIN *gwinpt,
        DBArc  *arcpek,
        DBSeg  *segpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar en grafisk cirkel i ett visst fönster.
 *      Vid ritning lagras samtidigt objektet i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          arcpek => Pekare till arc-post.
 *          segpek => Pekare till DBSeg.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 19/2/94 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   double scale;
   int    k;

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
            gwinpt->geo.psiz_x /
           (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
/*
***Create graphical polyline representation.
*/
   k = -1;
   WPplar(arcpek,segpek,scale,&k,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,k,x,y,z);
/*
***Clip the polyline to the window borders.
***Display or erase visible parts.
*/
   if ( WPcply(&gwinpt->vy.modwin,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  arcpek->hed_a.hit )
       {
       if ( WPsply(gwinpt,k,x,y,a,la,ARCTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,k,x,y,a,draw);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short  WPplar(
        DBArc *arcpek,
        DBSeg *segmnt,
        double scale,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Creates the graphical 3D polyline representation 
 *      for an arc.
 *
 *      In:  arcpek  =  C-ptr to DBArc.
 *           segmnt  =  Ptr to array of max 4 segments or NULL
 *                      if arc is 2D.
 *           scale   =  Arc scale factor (ZOOM-status). Used to
 *                      determine step length.
 *           n+1     =  Offset to polyline start.
 *
 *      Out: n       =  Offset to polyline end.
 *           x,y,z,a =  Polyline coordinates and status
 *
 *      Return:    0 = Ok.
 *
 *      (C)2006-12-09 J. Kjellander
 *
 ******************************************************!*/

  {
    double  cx,cy,cr,v1,v2,fi,fi1,fi2,fi3,dfi,
            sindfi,cosdfi,lgt,fis,fim,cn;
    int     i,k,state;
    DBCurve cur;

/*
***2D-arc.
*/
    if ( arcpek->ns_a == 0 )
      {
/*
***Initializations.
*/
      cx = arcpek->x_a;
      cy = arcpek->y_a;
      cr = arcpek->r_a;
      lgt = arcpek->lgt_a;
      v1 = arcpek->v1_a*DGTORD;
      v2 = arcpek->v2_a*DGTORD;
      WPget_cacc(&cn);
/*
***Calculate angle increment.
*/
      dfi = 1.0/SQRT(scale*cr)/cn;
/*
***What font ?
*/
      switch ( arcpek->fnt_a )
        {
/*
***Solid.
*/
        case SOLIDARC:
        state = 0;
        do
          {
          switch ( state )
            {
/*
***Start.
*/
            case 0:
            k = *n+1;
            fi1 = v1;
            fi2 = v2;
            sindfi = SIN(dfi);
            cosdfi = COS(dfi);
            x[k] = cr*COS(fi1);
            y[k] = cr*SIN(fi1);
            z[k] = 0.0;
            a[k] = 0;
            state = 1;
            break;
/*
***Lines.
*/
            case 1:
            state = -1;
            i = k++;
            fi = fi1 + dfi;
            while ( fi < fi2 )
              {
              if ( k >= (PLYMXV-2) )
                {
                dfi *= 4.0;
                state = 0;
                break;
                }
              x[k] = x[i]*cosdfi - y[i]*sindfi;
              y[k] = y[i]*cosdfi + x[i]*sindfi;
              z[k] = 0.0;
              a[k] = VISIBLE;
              i = k++;
              fi += dfi;
              }
            x[k] = cr*COS(fi2);  /* slutpunkt */
            y[k] = cr*SIN(fi2);
            z[k] = 0.0;
            a[k] = VISIBLE;
            break;
          }
        } while ( state >= 0 );
        break;
/*
***Phantom.
*/
        case DADOARC:
        state = 0;
        do
          {
          switch ( state )
            {
/*
***A long dash.
*/
            case 1:
            fi2 = fi1 + fis; 
            if ( fi2 >= fi3 )
              {
              fi2 = fi3;
              state = -1;
              }
            else state = 2;
            i = k++;
            fi = fi1 + dfi;
            while ( fi < fi2 )
              {
              if ( k >= (PLYMXV-2) )
	        {
                state = 6;
                break;
                }
              x[k] = x[i]*cosdfi - y[i]*sindfi;
              y[k] = y[i]*cosdfi + x[i]*sindfi;
              z[k] = 0.0;
              a[k] = VISIBLE;
              i = k++;
              fi += dfi;
              }
            x[k] = cr*COS(fi2);
            y[k] = cr*SIN(fi2);
            z[k] = 0.0;
            a[k] = VISIBLE;
            fi1 = fi2;
            break;
/*
***A space.
*/
            case 2:
            state = 3;
            fi2 = fi1 + fim; 
            if ( fi2 < fi3 )
              {
              if ( k >= (PLYMXV-2) )
                {
                state = 6;
                break;
                }
              x[++k] = cr*COS(fi2); 
              y[k] = cr*SIN(fi2);
              z[k] = 0.0;
              a[k] = 0;
              fi1 = fi2;
              }
            break;
/*
***A short dash.
*/
            case 3:
            fi2 = fi1 + fim; 
            if ( fi2 >= fi3 )
              {
              fi2 = fi3;
              state = -1;
              }
            else state = 4;
            i = k++;
            fi = fi1 + dfi;
            while ( fi < fi2 )
              {
              if ( k >= (PLYMXV-2) )
                {
                state = 6;
                break;
                }
              x[k] = x[i]*cosdfi - y[i]*sindfi;
              y[k] = y[i]*cosdfi + x[i]*sindfi;
              z[k] = 0.0;
              a[k] = VISIBLE;
              i = k++;
              fi += dfi;
              }
            x[k] = cr*COS(fi2);
            y[k] = cr*SIN(fi2);
            z[k] = 0.0;
            a[k] = VISIBLE;
            fi1 = fi2;
            break;
/*
***A space.
*/
            case 4:
            state = 1;
            fi2 = fi1 + fim; 
            if ( fi2 < fi3 )
              {
              if ( k >= (PLYMXV-2) )
                {
                state = 6;
                break;
                }
              x[++k] = cr*COS(fi2); 
              y[k] = cr*SIN(fi2);
              z[k] = 0.0;
              a[k] = 0;
              fi1 = fi2;
              }
            break;
/*
***Start.
*/
            case 0:
            k = *n+1;
            fi1 = v1;
            fi2 = v2;
            sindfi = SIN(dfi);
            cosdfi = COS(dfi);
            fi3 = fi2;
            if ( lgt <= STOL ) fis = fi2;
            else               fis = lgt/cr;
            fim = fis*DASHRATIO;
            x[k] = cr*COS(fi1);
            y[k] = cr*SIN(fi1);
            z[k] = 0.0;
            a[k] = 0;
            state = 1;
            break;
/*
***Overflow.
*/
            case 6:
            if ( dfi < 0.5 )
              {
              dfi *= 4.0;
              state = 0;
              }
            else state = -1;
            break;

            }
          } while (state >= 0);

        break;
/*
***Dashed.
*/
        case DASHARC:
        state = 0;
        do
          {
          switch ( state )
            {
/*
***A dash.
*/
            case 1:
            fi2 = fi1 + fis; 
            if ( fi2 >= fi3 )
              {
              fi2 = fi3;
              state = -1;
              }
            else state = 2;
            i = k++;
            fi = fi1 + dfi;
            while ( fi < fi2 )
              {
              if ( k >= (PLYMXV-2) )
                {
                state = 6;
                break;
                }
              x[k] = x[i]*cosdfi - y[i]*sindfi;
              y[k] = y[i]*cosdfi + x[i]*sindfi;
              z[k] = 0.0;
              a[k] = VISIBLE;
              i = k++;
              fi += dfi;
              }
            x[k] = cr*COS(fi2);    /* slutpunkt */
            y[k] = cr*SIN(fi2);
            z[k] = 0.0;
            a[k] = VISIBLE;
            fi1 = fi2;
            break;
/*
***A space.
*/
            case 2:
            state = 1;
            fi2 = fi1 + fim; 
            if ( fi2 < fi3 )
              {
              if ( k >= (PLYMXV-2) )
                {
                state = 6;
                break;
                }
              x[++k] = cr*COS(fi2); 
              y[k] = cr*SIN(fi2);
              z[k] = 0.0;
              a[k] = 0;
              fi1 = fi2;
              }
            break;
/*
***Start.
*/
            case 0:
            k = *n+1;
            fi1 = v1;
            fi2 = v2;
            sindfi = SIN(dfi);
            cosdfi = COS(dfi);
            fi3 = fi2;
            if ( lgt <= STOL ) fis = fi2;
            else               fis = lgt/cr;
            fim = fis*DASHRATIO;
            x[k] = cr*COS(fi1);
            y[k] = cr*SIN(fi1);
            z[k] = 0.0;
            a[k] = 0;
            state = 1;
            break;
/*
***Overflow.
*/
            case 6:
            if ( dfi < 0.5 )
              {
              dfi *= 4.0;
              state = 0;
              }
            else state = -1;
            break;
            }
          } while ( state >= 0 );

        break;
      }    
/*
***Endside-information.
*/
      for ( i = *n+1; i <= (*n + k)/2; i++ )
        {
        x[i] += cx;
        y[i] += cy;
        }
      for ( i = (*n + k)/2 + 1; i <= k; i++ )
        {
        x[i] += cx;
        y[i] += cy;
        a[i] |= ENDSIDE;
        }
      }
/*
***The polyline for a 3D arc is created by WPplcu(), same as curves.
***To use WPplcu(), create temporary curve structure.
*/
   else
     {
     k = *n;
     cur.hed_cu.type = CURTYP;
     cur.ns_cu = cur.nsgr_cu = arcpek->ns_a;
     cur.fnt_cu = arcpek->fnt_a;
     cur.lgt_cu = arcpek->lgt_a;
     cur.al_cu = arcpek->al_a;
     cur.plank_cu = FALSE;
     WPplcu(&cur,segmnt,scale,&k,x,y,z,a);
     }
/*
***Offset to last position in polyline.
*/
   *n = k;

   return(0);
}

/********************************************************/
