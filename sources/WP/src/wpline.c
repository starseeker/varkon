/**********************************************************************
*
*    wpline.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPdrli();    Draw line
*    WPdlli();    Delete line
*    WPplli();    Create 3D polyline
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

extern int actpen;


static short drawli(WPGWIN *gwinpt, DBLine *linpek, DBptr la, bool draw);

/*!******************************************************/

        short   WPdrli(
        DBLine *linpek,
        DBptr   la,
        DBint   win_id)

/*      Display a line.
 *
 *      In: linpek => Pekare till linje-post.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *      2006-12-08 Removed gpdrli(), J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om linjen är blankad behöver vi inte göra nånting.
*/
   if ( linpek->hed_l.blank) return(0);
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
***Ja, är den nivå som punkten ligger på tänd i detta fönster.
*/
         if ( WPnivt(gwinpt->nivtab,linpek->hed_l.level) )
           {
/*
***Set color and width.
*/
           if ( linpek->hed_l.pen != actpen ) WPspen(linpek->hed_l.pen);
           if ( linpek->wdt_l != 0.0 ) WPswdt(gwinpt->id.w_id,linpek->wdt_l);
/*
***Display.
*/
           drawli(gwinpt,linpek,la,TRUE);
           if ( linpek->wdt_l != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short   WPdlli(
        DBLine *linpek,
        DBptr   la,
        DBint   win_id)

/*      Delete a line.
 *
 *      In: linpek => Pekare till linje-post.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 27/12/94 J. Kjellander
 *
 *      1997-12-25 Breda linjer, J.Kjellander
 *      2006-12-08 Removed gpdlli(), J.Kjellander
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
***Ja. Stryk den ur DF om den finns där.
*/
         if ( WPfobj(gwinpt,la,LINTYP,&typ) ) WProbj(gwinpt);
/*
***Om den ligger på en släckt nivå eller är blankad gör vi inget mer.
*/
         if ( !WPnivt(gwinpt->nivtab,linpek->hed_l.level) ||
                             linpek->hed_l.blank) return(0);
/*
***Annars suddar vi från skärmen också.
*/
          if ( linpek->wdt_l != 0.0 ) WPswdt(gwinpt->id.w_id,linpek->wdt_l);
          drawli(gwinpt,linpek,la,FALSE);
          if ( linpek->wdt_l != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static short   drawli(
        WPGWIN *gwinpt,
        DBLine *linpek,
        DBptr   la,
        bool    draw)

/*      Display or erase a line in a window.
 *
 *      In:  gwinpt = C-ptr to window.
 *           lptr   = C-ptr to line.
 *           la     = DB-ptr to line.
 *           draw   = TRUE = Draw, FALSE = Erase
 *
 *      Return = 0.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    n;

/*
***Create 3D polyline.
*/
   n = -1;
   WPplli(linpek,&n,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,n,x,y,z);
/*
***Clip the polyline to the window borders.
***Display or erase visible parts.
*/
   if ( WPcply(&gwinpt->vy.modwin,(short)-1,&n,x,y,a) )
     {
     if ( draw  &&  linpek->hed_l.hit )
       {
       if ( WPsply(gwinpt,n,x,y,a,la,LINTYP) ) WPdobj(gwinpt,TRUE);
       else return(erpush("GP0012",""));
       }
     else WPdply(gwinpt,n,x,y,a,draw);
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short   WPplli(
        DBLine *linept,
        int    *n,
        double  x[],
        double  y[],
        double  z[],
        char    a[])

/*      Creates the graphical 3D polyline representation 
 *      for a line.
 *
 *      In:  linept  = C-ptr to DBLine.
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
    int    i,k=0;
    char   aa=0;
    double len,dlen,x1,y1,z1,x2,y2,z2,dx,dy,dz,c1,c2,dxc1,dyc1,dzc1,
           dxc2,dyc2,dzc2;
/*
***Initializations.
*/
    k = *n;

    x[k+1] = linept->crd1_l.x_gm;
    y[k+1] = linept->crd1_l.y_gm;
    z[k+1] = linept->crd1_l.z_gm;
    a[k+1] = 0;

    x[k+2] = linept->crd2_l.x_gm;
    y[k+2] = linept->crd2_l.y_gm;
    z[k+2] = linept->crd2_l.z_gm;
    a[k+2] = 1;
/*
***What font ?
*/
    switch ( linept->fnt_l )
      {
/*
***Solid is easy.
*/
       case SOLIDLN:
       k += 2;
       break;
/*
***Dashed needs more attention.
*/
       case DASHLN:
       i = 0;
       do
         {
         switch ( i )
           {
/*
***Start.
*/
           case 0:
           k = *n;
           x1 = x[k+1];    /* Startpunkt */
           y1 = y[k+1];
           z1 = z[k+1];

           x2 = x[k+2];    /* Slutpunkt */
           y2 = y[k+2];
           z2 = z[k+2];

           dx = x2 - x1;
           dy = y2 - y1;
           dz = z2 - z1;

           len = SQRT(dx*dx + dy*dy + dz*dz);
           c1 = linept->lgt_l;
           c2 = c1*DASHRATIO;
/*
***If the line is too short for two dashes make it solid.
*/
           if ( len < 1.5*c1 + c2 ) c1 = len + 1.0;
           else
             {
             dxc1 = c1*dx/len;
             dyc1 = c1*dy/len;
             dxc2 = c2*dx/len;
             dyc2 = c2*dy/len;
             dzc1 = c1*dz/len;
             dzc2 = c2*dz/len;
             }

           dlen = 0.0;
           k++;
           aa = 0;
           i = 1;
           break;
/*
***A dash.
*/
           case 1:
           dlen += c1;
           if ( dlen < len )
             {
             x1 += dxc1;
             y1 += dyc1;
             z1 += dzc1;
             i = 2;
             }
           else i = 3;

           aa = VISIBLE;
           k++;
           break;
/*
***A space.
*/
           case 2:
           dlen += c2;
           if ( dlen < len )
             {
             x1 += dxc2;
             y1 += dyc2;
             z1 += dzc2;
             k++;
             i = 1;
             }
           else i = 3;

           aa = 0;
           break;
/*
***Stop.
*/
           case 3:
           x1 = x2;
           y1 = y2;
           z1 = z2;
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Save in x, y, z and a.
*/
         if ( k < PLYMXV )
           {
           x[k] = x1;
           y[k] = y1;
           z[k] = z1;
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while ( i >= 0 );
       break;
/*
***Phantom.
*/
       case DADOLN:
       i = 0;
       do
         {
         switch (i)
           {
/*
***Start.
*/
           case 0:
           k = *n;
           x1 = x[k+1];    /* Startpunkt */
           y1 = y[k+1];
           z1 = z[k+1];

           x2 = x[k+2];    /* Slutpunkt */
           y2 = y[k+2];
           z2 = z[k+2];

           dx = x2 - x1;
           dy = y2 - y1;
           dz = z2 - z1;
           len = SQRT(dx*dx + dy*dy + dz*dz);

           c1 = linept->lgt_l;
           if ( c1 <= STOL ) c1 = len;
           c2 = c1*DASHRATIO;
           dxc1 = c1*dx/len;
           dyc1 = c1*dy/len;
           dzc1 = c1*dz/len;
           dxc2 = c2*dx/len;
           dyc2 = c2*dy/len;
           dzc2 = c2*dz/len;
           dlen = 0.0;
           k++;
           aa = 0;
           i = 1;
           break;
/*
***A long dash.
*/
           case 1:
           dlen += c1;
           if (dlen < len)
             {
             x1 += dxc1;
             y1 += dyc1;
             z1 += dzc1;
             i = 2;
             }
           else i = 5;

           aa = VISIBLE;
           k++;
           break;
/*
***A space.
*/
           case 2:
           dlen += c2;
           if (dlen < len)
             {
             x1 += dxc2;
             y1 += dyc2;
             z1 += dzc2;
             k++;
             i = 3;
             }
           else i = 5;

           aa = 0;
           break;
/*
***A short dash.
*/
           case 3:
           dlen += c2;
           if (dlen < len)
             {
             x1 += dxc2;
             y1 += dyc2;
             z1 += dzc2;
             i = 4;
             }
           else i = 5;

           aa = VISIBLE;
           k++;
           break;
/*
***Another space.
*/
           case 4:
           dlen += c2;
           if (dlen < len)
             {
             x1 += dxc2;
             y1 += dyc2;
             z1 += dzc2;
             k++;
             i = 1;
             }
           else i = 5;

           aa = 0;
           break;
/*
***Stop.
*/
           case 5:
           x1 = x2;
           y1 = y2;
           z1 = z2;
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Save in x, y, z and a.
*/
         if (k < PLYMXV)
           {
           x[k] = x1;
           y[k] = y1;
           z[k] = z1;
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while (i >= 0);
       break;
/*
***Dotted.
*/
       case DOTLN:
       i = 0;
       do
         {
         switch (i)
           {
/*
***Start.
*/
           case 0:
           k = *n;
           x1 = x[k+1];    /* Startpunkt */
           y1 = y[k+1];
           z1 = z[k+1];

           x2 = x[k+2];    /* Slutpunkt */
           y2 = y[k+2];
           z2 = z[k+2];

           dx = x2 - x1;
           dy = y2 - y1;
           dz = z2 - z1;

           len = SQRT(dx*dx + dy*dy + dz*dz);
           c1 = linept->lgt_l;
           c2 = c1*DOTRATIO;
/*
***If the line is too short make it solid.
*/
           if ( len < 1.5*c1 + c2 ) c1 = len + 1.0;
           else
             {
             dxc1 = c1*dx/len;
             dyc1 = c1*dy/len;
             dzc1 = c1*dz/len;
             dxc2 = c2*dx/len;
             dyc2 = c2*dy/len;
             dzc2 = c2*dz/len;
             }

           dlen = 0.0;
           k++;
           aa = 0;
           i = 1;
           break;
/*
***A dot.
*/
           case 1:
           dlen += c1;
           if ( dlen < len )
             {
             x1 += dxc1;
             y1 += dyc1;
             z1 += dzc1;
             i = 2;
             }
           else i = 3;

           aa = VISIBLE;
           k++;
           break;
/*
***A space.
*/
           case 2:
           dlen += c2;
           if ( dlen < len )
             {
             x1 += dxc2;
             y1 += dyc2;
             z1 += dzc2;
             k++;
             i = 1;
             }
           else i = 3;

           aa = 0;
           break;
/*
***Stop.
*/ 
           case 3:
           x1 = x2;
           y1 = y2;
           z1 = z2;
           aa = VISIBLE;
           i = -1;
           break;
           }
/*
***Save in x, y, z and a.
*/
         if ( k < PLYMXV )
           {
           x[k] = x1;
           y[k] = y1;
           z[k] = z1;
           a[k] = aa;
           }
         else
           {
           k = PLYMXV - 1;
           i = -1;
           }
         } while ( i >= 0 );
       break;
       } 
/*
***Endside-information.
*/
    for ( i=k/2+1; i<=k; ++i ) a[i] = a[i] | ENDSIDE;

   *n = k;
/*
***End.
*/
    return(0);
 }

/********************************************************/
