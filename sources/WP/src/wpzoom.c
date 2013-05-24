/**********************************************************************
*
*    wpzoom.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPzoom();         Interactive ZOOM
*    WPiaut();         Interactive AutoZOOM
*    WPmaut();         AutoZOOM from MBS
*    WPauto_modwin();  Creates a modwin for AutoZOOM
*    WPlstv();         Interactive Previous view
*    WPperp();         Interactive PERP_VIEW
*    WPcent();         Interactive CEN_VIEW
*    WPscle();         Interactive SCL_VIEW
*    WPupvi();         Uppdate view after SCL_VIEW or CEN_VIEW
*    WPuppr();         Uppdate view after PERP_VIEW
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
#include <string.h>
#include <math.h>

extern int actfunc; /* For the help system */

static void drwarr(WPGWIN *gwinpt, GC arr_gc, int ix1, int iy1,
                   int ix2, int iy2);

/*!******************************************************/

        short WPzoom()

/*      Interactive function for WPGWIN/WPRWIN ZOOM.
 *
 *      Return:  0 = OK.
 *          REJECT = Operation rejected.
 *
 *      Error: IG3042 = Can't decrease scale this much.
 *
 *      (C)microform ab 14/12/94 J. Kjellander
 *
 *      1998-04-03 Ny WPgtsw(), J.Kjellander
 *      2007-08-01 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   int     ix1,iy1,ix2,iy2,tmp,actfunc_org;
   double  x1,y1,x2,y2;
   double  move,dx,dy;
   wpw_id  grw_id;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Set actfunc during user action, see IG/include/futab.h.
*/
   actfunc_org = actfunc;
   actfunc = 193;
/*
***Get a rubberband window, then reset actfunc.
*/
   status = WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,WP_RUB_RECT,TRUE);
   actfunc = actfunc_org;
   if ( status < 0 ) return(status);
/*
***Sort coordinates.
*/
   if ( ix2 < ix1 ) { tmp = ix1; ix1 = ix2; ix2 = tmp; }
   if ( iy2 < iy1 ) { tmp = iy1; iy1 = iy2; iy2 = tmp; }
/*
***Translate window ID to C pointer. Check window type
***and execute zoom.
*/
   if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL  &&  winptr->typ == TYP_GWIN )
     {
/*
***The window is a WPGWIN.
***Transform to model coordinates.
*/
     gwinpt = (WPGWIN *)winptr->ptr;

     x1 = gwinpt->vy.modwin.xmin + (ix1-gwinpt->vy.scrwin.xmin)*
         (gwinpt->vy.modwin.xmax-gwinpt->vy.modwin.xmin)/
         (gwinpt->vy.scrwin.xmax-gwinpt->vy.scrwin.xmin);

     y1 = gwinpt->vy.modwin.ymin + (iy1-gwinpt->vy.scrwin.ymin)*
         (gwinpt->vy.modwin.ymax-gwinpt->vy.modwin.ymin)/
         (gwinpt->vy.scrwin.ymax-gwinpt->vy.scrwin.ymin);

     x2 = gwinpt->vy.modwin.xmin + (ix2-gwinpt->vy.scrwin.xmin)*
         (gwinpt->vy.modwin.xmax-gwinpt->vy.modwin.xmin)/
         (gwinpt->vy.scrwin.xmax-gwinpt->vy.scrwin.xmin);

     y2 = gwinpt->vy.modwin.ymin + (iy2-gwinpt->vy.scrwin.ymin)*
         (gwinpt->vy.modwin.ymax-gwinpt->vy.modwin.ymin)/
         (gwinpt->vy.scrwin.ymax-gwinpt->vy.scrwin.ymin);
/*
***Error check.
*/
     dx = x2 - x1;
     dy = y2 - y1;

     if ( dx < 1e-10 || dy < 1e-10 )
       {
       erpush("IG3042","WPzoom");
       errmes();
       return(0);
       }
/*
***Save new model window and normalize.
*/
     V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

     gwinpt->vy.modwin.xmin = x1;
     gwinpt->vy.modwin.xmax = x2;
     gwinpt->vy.modwin.ymin = y1;
     gwinpt->vy.modwin.ymax = y2;

     WPnrgw(gwinpt);
/*
***Update the display.
*/
     WPrepaint_GWIN((DBint)gwinpt->id.w_id);
     }
/*
***The window is a WPRWIN.
*/
   else
     {
     rwinpt = (WPRWIN *)winptr->ptr;
/*
***Calculate the translation neeed.
*/
     move = rwinpt->geo.dx/2 - (ix1 + ((ix2 - ix1)/2));
     move = (move/rwinpt->geo.dx)*(rwinpt->xmax - rwinpt->xmin);
     rwinpt->movx -= move*rwinpt->scale;

     move = rwinpt->geo.dy/2 - (iy1 + ((iy2 - iy1)/2));
     move = (move/rwinpt->geo.dy)*(rwinpt->ymax - rwinpt->ymin);
     rwinpt->movy -= move*rwinpt->scale;
/*
***Then the scale, use the mean value of X and Y.
*/
     rwinpt->scale *= (((double)(ix2-ix1)/rwinpt->geo.dx) +
                        ((double)(iy2-iy1)/rwinpt->geo.dy))/2.0;
     if ( rwinpt-> scale < 1E-10 ) rwinpt->scale = 1E-10;

     WPsodl_all(rwinpt);
/*
***Copy window contents to the "save-under" bitmap.
*/
     XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                     0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPiaut(
        WPGWIN *gwinpt,
        WPRWIN *rwinpt,
        int     wintype)

/*      Interactive function AutoZOOM for graphical windows.
 *
 *      In: gwinpt  = C ptr to WPGWIN
 *          rwinpt  = C ptr to WPRWIN
 *          wintype = TYP_GWIN or TYPRWIN
 *
 *      (C)microform ab 29/12/94 J. Kjellander
 *
 *      2007-02-18 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
   VYWIN  minwin;

/*
***GWIN or RWIN ?
*/
   switch ( wintype )
     {
/*
***WPGWIN.
*/
     case TYP_GWIN:
     WPergw((DBint)gwinpt->id.w_id);
     WPauto_modwin(gwinpt,&minwin);
     V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));
     V3MOME(&minwin,&gwinpt->vy.modwin,sizeof(VYWIN));
     WPnrgw(gwinpt);
     WPrepaint_GWIN((DBint)gwinpt->id.w_id);
     break;
/*
***WPRWIN.
*/
     case TYP_RWIN:
     rwinpt->movx  = rwinpt->movy = 0.0;
     rwinpt->rotx  = rwinpt->roty = rwinpt->rotz = 0.0;
     rwinpt->scale = 1.0;
     WPsodl_all(rwinpt);
     XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
               0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
     break;
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPmaut(
        DBint win_id)

/*      AutoZOOM med X-Windows från MBS.
 *
 *      In: win_id = ID för fönster som skall zoom:as.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1382 = Fönster %s finns ej
 *
 *      (C)microform ab 18/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[81];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om fönstret finns fixar vi en C-pekare till det
***och anropar den interaktiva versionen av AutoZOOM.
*/
   if ( (winptr=WPwgwp((wpw_id)win_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     return(WPiaut(gwinpt,NULL,TYP_GWIN));
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1382",errbuf));
     }
  }

/********************************************************/
/*!******************************************************/

        short   WPauto_modwin(
        WPGWIN *gwinpt,
        VYWIN  *minwin)

/*      Creates a model window such that the
 *      model fits when projected according to
 *      the view matrix in the supplied WPGWIN.
 *
 *      In: gwinpt = C ptr to WPGWIN.
 *
 *      Out: *minwin = The minimum model window
 *
 *      (C)2007-02-11 J.Kjellander
 *
 ******************************************************!*/

  {
   double dmx,dmy,dx,dy;
   WPVIEW minvy;

/*
***Calculate bounding box.
*/
   WPmsiz(gwinpt,&minvy);
/*
***If the bounding box is infinitely small, ie. the model
***consits of a single point, put the point in the middle
**of the modwin and the scale = 1.
*/
   dx = minvy.modwin.xmax - minvy.modwin.xmin;
   dy = minvy.modwin.ymax - minvy.modwin.ymin;

   if ( dx == 0.0  &&  dy == 0.0 )
     {
     dmx = gwinpt->geo.psiz_x *
          (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     dmy = gwinpt->geo.psiz_y *
          (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     minvy.modwin.xmin -= dmx/2.0;
     minvy.modwin.ymin -= dmy/2.0;
     minvy.modwin.xmax += dmx/2.0;
     minvy.modwin.ymax += dmy/2.0;
     }
/*
***If the model is empty make a default modwin with zero
***in lower left and scale = 1.0
*/
   else if ( dx < 1e-10  &&  dy < 1e-10 )
     {
     minvy.modwin.xmin = 0.0;
     minvy.modwin.ymin = 0.0;
     minvy.modwin.xmax = gwinpt->vy.scrwin.xmax;
     minvy.modwin.ymax = gwinpt->vy.scrwin.ymax;
     }
/*
***If the bounding box is extremely thin, ie. a horisontal or vertical
***line, make it a little thicker.
*/
loop:
   dx = minvy.modwin.xmax - minvy.modwin.xmin;
   dy = minvy.modwin.ymax - minvy.modwin.ymin;

   if ( dx < 1e-10 )
     {
     minvy.modwin.xmin -= (0.05*dy);
     minvy.modwin.xmax += (0.05*dy);
     goto loop;
     }

   if ( dy < 1e-10 )
     {
     minvy.modwin.ymin -= (0.05*dx);
     minvy.modwin.ymax += (0.05*dx);
     goto loop;
     }
/*
***Increase the size by 8%.
*/
   minvy.modwin.xmin -= (0.04*dx);
   minvy.modwin.xmax += (0.04*dx);
   minvy.modwin.ymin -= (0.04*dy);
   minvy.modwin.ymax += (0.04*dy);
/*
***Copy minvy to output.
*/
   V3MOME(&minvy.modwin,minwin,sizeof(VYWIN));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPlstv(
        WPGWIN *gwinpt)

/*      X11-funktion för föregående vy.
 *
 *      In: gwinpt = Pekare till fönstret som innehåller vyn.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *
 *      (C)microform ab 5/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   WPVIEW tmpvy;

/*
***Om det finns en föregående vy låter vi den och nuvarande vy
***byta plats annar gör vi ingenting.
*/
   if ( gwinpt->old_vy.status != VIEW_NOT_USED )
     {
     V3MOME(&gwinpt->vy,&tmpvy,sizeof(WPVIEW));
     V3MOME(&gwinpt->old_vy,&gwinpt->vy,sizeof(WPVIEW));
     V3MOME(&tmpvy,&gwinpt->old_vy,sizeof(WPVIEW));
/*
***Uppdatera fönsterramen.
*/
     WPtitle_GWIN(gwinpt);
/*
***Uppdatera fönstret på skärmen.
*/
     WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPperp(
        WPGWIN *gwinpt,
        int     x,
        int     y)

/*      Varkon-funktion för inställning av perspektiv
 *      i viss vy med X-Windows.
 *
 *      In: gwinpt = Pekare till fönstret som innehåller vyn.
 *          x,y    = Aktiverande knapps/ikons rootposition.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 5/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],persp[81],dist[81],okey[81],reject[81],help[81],
            on[81],off[81],valstr[81];
   int      main_x,main_y;
   short    main_dx,main_dy,alt_x,alt_y,alth,altlen,ly,lm;
   DBint    iwin_id,dum_id,onoff_id,but_id,help_id,okey_id,reject_id,
            dist_id,value_id;
   bool     diston;
   double   dsval;

   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPBUTT  *buttpt;
   WPEDIT  *editpt;

/*
***Texter för rubrik mm.
*/
   if ( !WPgrst("varkon.persp.title",rubrik) )
     strcpy(rubrik,"Perspektiv f|r vy : ");

   if ( !WPgrst("varkon.persp.persp",persp) )
     strcpy(persp,"Sant perspektiv");

   if ( !WPgrst("varkon.persp.dist",dist) )
     strcpy(dist,"Avstånd");

   if ( !WPgrst("varkon.input.on",on) )
     strcpy(on,"På");

   if ( !WPgrst("varkon.input.off",off) )
     strcpy(off,"Av");

   if ( !WPgrst("varkon.input.okey",okey) )
     strcpy(okey,"Okej");

   if ( !WPgrst("varkon.input.reject",reject) )
     strcpy(reject,"Avbryt");

   if ( !WPgrst("varkon.input.help",help) )
     strcpy(help,"Hjälp");
/*
***Längsta texten avgör fönstrets bredd.
*/
   altlen = 0;

   if ( WPstrl(rubrik) + WPstrl(gwinpt->vy.name) > altlen )
     altlen = WPstrl(rubrik) + WPstrl(gwinpt->vy.name);
   if ( WPstrl(persp) + WPstrl(on) > altlen )
     altlen = WPstrl(persp) + WPstrl(on);
   if ( WPstrl(dist) + WPstrl("1234567890123") > altlen )
     altlen = WPstrl(dist) + WPstrl("1234567890123");
   if ( WPstrl(okey) + WPstrl(reject) + WPstrl(help) > altlen )
     altlen = WPstrl(okey) + WPstrl(reject) + WPstrl(help);
/*
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/
   ly   = (short)(0.8*WPstrh());
   alth = (short)(1.6*WPstrh()); 
   lm   = (short)(1.4*WPstrh());

   main_dx = (short)(ly + altlen + 2*lm + ly);
   main_dy = (short)(ly + 2*(alth + ly) +  ly + 2*WPstrh() + ly);  
/*
***Skapa själva perspektivfönstret som ett WPIWIN.
*/
   strcat(rubrik,gwinpt->vy.name);
   WPposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Ramlös button för persp.
*/
   alt_x  = ly;
   alt_y  = ly;
   altlen = WPstrl(persp);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)0,persp,persp,"",WP_BGND2,WP_FGND,&dum_id);
/*
***Toggle-button för ON/OFF.
*/
   alt_x = alt_x + WPstrl(persp) + lm;
   if ( WPstrl(on) > WPstrl(off) ) altlen = (short)(WPstrl(on)  + 15);
   else                            altlen = (short)(WPstrl(off) + 15);

   dsval = gwinpt->vy.pdist;
   if ( dsval == 0.0 ) diston = FALSE;
   else                diston = TRUE;

   if ( diston == FALSE )
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)1,off,on,"",WP_BGND2,WP_FGND,&onoff_id);
/*
***Om perspektiv redan är satt skall avståndet visas.
*/
   else
     {
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)1,on,off,"",WP_BGND2,WP_FGND,&onoff_id);
     alt_x  = ly;
     alt_y  = ly + alth + ly;
     altlen = WPstrl(dist);
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)0,dist,dist,"",WP_BGND2,WP_FGND,&dist_id);

     alt_x  = (short)(ly + altlen + 10);
     altlen = WPstrl("1234567890123");
     sprintf(valstr,"%g",dsval);
     WPmced((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                             valstr,15,&value_id);
     }
/*
***Okey, avbryt och hjälp.
*/
   alt_x  = (short)(ly);
   alt_y  = (short)(ly + 2*(alth + ly) + ly);
   alth   = (short)(2*WPstrh());
   altlen = (short)(WPstrl(okey) + 15);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,okey,okey,"",WP_BGND2,WP_FGND,&okey_id);

   alt_x  = (short)(alt_x + altlen + lm);
   altlen = (short)(WPstrl(reject) + 15);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,reject,reject,"",WP_BGND2,WP_FGND,&reject_id);

   altlen = (short)(WPstrl(help) + 15);
   alt_x  = (short)(main_dx - altlen - lm);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,help,help,"",WP_BGND2,WP_FGND,&help_id);
/*
***Klart för visning.
*/
   WPwshw(iwin_id);
/*
***Vänta på action.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Ok.
*/
   if ( but_id == okey_id )
     {
     if ( diston == TRUE )
       {
       WPgted(iwin_id,value_id,valstr);
       if ( sscanf(valstr,"%lf",&dsval) != 1 )
         {
         XBell(xdisp,100);
         goto loop;
         }
       }
     else dsval = 0.0;

     V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));
     gwinpt->vy.pdist = dsval;
     }
/*
***Avbryt.
*/
   else if ( but_id == reject_id )
     {
     WPwdel(iwin_id);
     return(REJECT);
     }
/*
***Hjälp.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Onoff.
*/
   else if ( but_id == onoff_id )
     {
/*
***Om perspektiv är på stänger vi av det och suddar.
*/
     if ( diston == TRUE )
       {
       WPgted(iwin_id,value_id,valstr);
       if ( sscanf(valstr,"%lf",&dsval) != 1 )
         {
         XBell(xdisp,100);
         goto loop;
         }
       WPwdls(iwin_id,dist_id);
       WPwdls(iwin_id,value_id);
       diston = FALSE;
       }
/*
***Om perspektiv är av sätter vi på det och
***skapar två nya subfönster.
*/
     else
       {
       alt_x  = ly;
       alt_y  = ly + alth + ly;
       altlen = WPstrl(dist);
       WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                       (short)0,dist,dist,"",WP_BGND2,WP_FGND,&dist_id);

       alt_x  = (short)(ly + altlen + 10);
       altlen = WPstrl("1234567890123");
       sprintf(valstr,"%g",dsval);
       WPmced((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                               valstr,15,&value_id);

       winptr = WPwgwp((wpw_id)iwin_id);
       iwinpt = (WPIWIN *)winptr->ptr;
       buttpt = (WPBUTT *)iwinpt->wintab[dist_id].ptr;
       editpt = (WPEDIT *)iwinpt->wintab[value_id].ptr;
       WPfoed(editpt,TRUE);
       WPxpbu(buttpt);
       WPxped(editpt);
       XFlush(xdisp);
       diston = TRUE;
       }
     goto loop;
     }
/*
***Händelse från edit-fönstret bryr vi oss inte om.
*/
   else if ( but_id == value_id ) goto loop;
/*
***Nu är det dags att lägga av.
*/
   WPwdel(iwin_id);
/*
***Uppdatera skärmen.
*/
   WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPcent(
        WPGWIN *gwinpt)

/*      Varkon-funktion för panorering med X-Windows.
 *
 *      In: gwinpt = Pekare till fönstret som skall panoreras.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 4/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
    short     status=0;
    int       x1=0,y1=0,curx2,cury2,lastx2=0,lasty2=0;
    double    mdx,mdy;
    Window    root,child;
    int       root_x,root_y,win_x,win_y;
    unsigned  int mask;
    XEvent    xev;           
    GC        Arr_GC;
    XGCValues values;

/*
***Eventmask och cursor för gummiband.
*/
   WPaddmess_mcwin(IGgtts(310),WP_PROMPT);
   XSelectInput(xdisp,gwinpt->id.x_id,GWEM_RUB);
   WPscur(gwinpt->id.w_id,TRUE,xgcur1);
/*
***GC för pilspetsen.
*/
   Arr_GC = XCreateGC(xdisp,gwinpt->id.x_id,0,&values);
   XSetLineAttributes(xdisp,Arr_GC,2,LineSolid,CapButt,JoinBevel);
   XSetFunction(xdisp,Arr_GC,GXxor);
   XSetBackground(xdisp,Arr_GC,WPgcol(0));
   XSetForeground(xdisp,Arr_GC,WPgcol(3));
/*
***När vi börjar har musknappen ännu inte tryckts ned.
*/
   while ( 1 )
     {
     XMaskEvent(xdisp,ButtonPressMask |
                      ButtonReleaseMask |
                      PointerMotionMask,&xev);

     switch (xev.type)
       {
/*
***Detta är första gången som musknappen trycks ned.
***Kolla att det är i rätt fönster.
*/
       case ButtonPress:
       WPscur(gwinpt->id.w_id,TRUE,xgcur2);

       if ( xev.xany.window != gwinpt->id.x_id )
         {
         status = REJECT;
         goto exit;
         }
/*
***Knapp 2 och 3 betyder avbrott.
*/
       if ( xev.xbutton.button == 2 )
         {
         status = GOMAIN;
         goto exit;
         }
       if ( xev.xbutton.button == 3 )
         {
         status = REJECT;
         goto exit;
         }

       WPaddmess_mcwin(IGgtts(133),WP_PROMPT);
       x1 = curx2 = lastx2 = xev.xkey.x;
       y1 = cury2 = lasty2 = xev.xkey.y;
       XDrawArc(xdisp,gwinpt->id.x_id,Arr_GC,
                                 x1-10,y1-10,20,20,0,360*64);
       drwarr(gwinpt,Arr_GC,x1,y1,x1,y1);
       break;
/*
***Nu flyttar sig musen.
*/
       case MotionNotify:
       curx2 = xev.xmotion.x;
       cury2 = xev.xmotion.y;
       if ( curx2 != lastx2  || cury2 != lasty2 )
         {
         drwarr(gwinpt,Arr_GC,x1,y1,lastx2,lasty2);
         if ( curx2 != x1  || cury2 != y1 )
           drwarr(gwinpt,Arr_GC,x1,y1,curx2,cury2);
         lastx2 = curx2;
         lasty2 = cury2;
         }
/*
***För att få ett nytt Motion-event måste vi anropa XQueryPointer().
*/
       XQueryPointer(xdisp,gwinpt->id.x_id,&root,&child,&root_x,&root_y,
                     &win_x,&win_y,&mask);
       break;
/*
***När knappen släpps igen är det slut.
*/
       case ButtonRelease:
       if ( lastx2 != x1  || lasty2 != y1 )
         drwarr(gwinpt,Arr_GC,x1,y1,lastx2,lasty2);
       curx2 = xev.xkey.x;
       cury2 = xev.xkey.y;
       XDrawArc(xdisp,gwinpt->id.x_id,Arr_GC,
                                 x1-10,y1-10,20,20,0,360*64);
/*
***Varkons y-axel är motsatt X11.
*/
       y1    = gwinpt->geo.dy - y1;
       cury2 = gwinpt->geo.dy - cury2;
       goto update;
       }
     }
/*
***Hur mycket skall vi flytta ?
*/
update:
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

   mdx = ((double)(curx2 - x1)/(double)gwinpt->geo.dx) *
         (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
   mdy = ((double)(cury2 - y1)/(double)gwinpt->geo.dy) *
         (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);

   gwinpt->vy.modwin.xmin -= mdx;
   gwinpt->vy.modwin.xmax -= mdx;
   gwinpt->vy.modwin.ymin -= mdy;
   gwinpt->vy.modwin.ymax -= mdy;

   WPnrgw(gwinpt);
/*
***Uppdatera skärmen.
*/
   WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);
/*
***Utgång. Återställ eventmask och cursor.
*/
exit:
   XSelectInput(xdisp,gwinpt->id.x_id,GWEM_NORM);
   WPscur(gwinpt->id.w_id,FALSE,(Cursor)0);
   XFreeGC(xdisp,Arr_GC);
   WPclear_mcwin();

   return(status);
  }

/********************************************************/
/********************************************************/

        short   WPscle(
        WPGWIN *gwinpt,
        int     x,
        int     y)

/*      The scale dilaog for WPGWIN's.
 *
 *      In: gwinpt = C ptr to WPGWIN.
 *          x,y    = Root position of activiating button.
 *
 *      Return:  0 = OK.
 *          REJECT = Operation cancelled.
 *
 *      Error: IG3042 = Can't decrease scale more.
 *
 *      (C)microform ab 4/1/95 J. Kjellander
 *
 *      2008-01-25 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],dubbla[81],half[81],reject[81],help[81],
            scale[81];
   int      main_x,main_y,actfunc_org;
   short    status,main_dx,main_dy,alt_x,alt_y,alth,altlen,ly,lm;
   DBint    iwin_id,scale_id,one_id,but_id,help_id,
            dubbla_id,half_id,reject_id;
   double   skala,xmin,xmax,ymin,ymax,mdx,mdy;

/*
***Set actfunc, see IG/include/futab.h.
*/
   actfunc_org = actfunc;
   actfunc = 188;
/*
***Button texts from ini-file.
*/
   if ( !WPgrst("varkon.scale.title",rubrik) )  strcpy(rubrik,"Skala");
   if ( !WPgrst("varkon.scale.double",dubbla) ) strcpy(dubbla,"Dubbla");
   if ( !WPgrst("varkon.scale.half",half) )     strcpy(half,"Halva");
   if ( !WPgrst("varkon.input.reject",reject) ) strcpy(reject,"Avbryt");
   if ( !WPgrst("varkon.input.help",help) )     strcpy(help,"Hj{lp");
/*
***Button size.
*/
   altlen = 0;

   if ( WPstrl("1.0")      > altlen ) altlen = (short)WPstrl("1.0");
   if ( WPstrl(dubbla)     > altlen ) altlen = (short)WPstrl(dubbla);
   if ( WPstrl(half)       > altlen ) altlen = (short)WPstrl(half);
   if ( WPstrl(reject)     > altlen ) altlen = (short)WPstrl(reject);
   if ( WPstrl(help)       > altlen ) altlen = (short)WPstrl(help);
   if ( WPstrl("12345678") > altlen ) altlen = (short)WPstrl("12345678");

   altlen *= 1.3;
/*
***Window size.
*/
   ly   = (short)(0.8*WPstrh());
   alth = (short)(1.6*WPstrh()); 
   lm   = (short)(1.4*WPstrh());

   main_dx = (short)(ly + altlen + lm + altlen + ly);
   main_dy = (short)(ly + 2*(alth + ly) +  ly + 2*WPstrh() + ly);  
/*
***Create a WPIWIN.
*/
   WPposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Edit for user scale.
*/
   mdx   =  gwinpt->geo.psiz_x *
           (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   mdy   =  gwinpt->geo.psiz_y *
           (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   skala = mdx/(gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
   sprintf(scale,"%g",skala);

   alt_x  = ly;
   alt_y  = ly;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                           scale,15,&scale_id);
/*
***Button for scale = 1.0.
*/
   alt_x  = ly + altlen + lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                           "1.0","1.0","",WP_BGND2,WP_FGND,&one_id);
/*
***Double and half.
*/
   alt_x  = ly;
   alt_y  = ly + alth + ly;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                           dubbla,dubbla,"",WP_BGND2,WP_FGND,&dubbla_id);

   alt_x  = ly + altlen + lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                          half,half,"",WP_BGND2,WP_FGND,&half_id);
/*
***Reject and help.
*/
   alt_x  = (short)(ly);
   alt_y  = (short)(ly + 2*(alth + ly) + ly);
   alth   = (short)(2*WPstrh());
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);

   alt_x  = ly + altlen + lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
/*
***Show the dialog.
*/
   WPwshw(iwin_id);
/*
***Wait for action.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Reject.
*/
   if ( but_id == reject_id )
     {
     WPwdel(iwin_id);
     status = REJECT;
     goto exit;
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***User scale.
*/
   else if ( but_id == scale_id )
     {
     WPgted(iwin_id,but_id,scale);
     if ( sscanf(scale,"%lf",&skala) != 1 )
       {
       XBell(xdisp,100);
       goto loop;
       }
     }
/*
***Scale = 1.0
*/
   else if ( but_id == one_id ) skala = 1.0;
/*
***Double.
*/
   else if ( but_id == dubbla_id ) skala *= 2.0;
/*
***Half.
*/
   else if ( but_id == half_id ) skala /= 2.0;
/*
***Time to finish.
*/
   WPwdel(iwin_id);
/*
***Calculate new model window.
*/
   xmin =  gwinpt->vy.modwin.xmin + 
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 -
                     mdx/2.0/skala;
   xmax =  gwinpt->vy.modwin.xmin +
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0 + 
                     mdx/2.0/skala;
   ymin =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 -
                     mdy/2.0/skala;
   ymax =  gwinpt->vy.modwin.ymin +
          (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0 +
                     mdy/2.0/skala;
/*
***Uppdate the WPGWIN data.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));

   gwinpt->vy.modwin.xmin = xmin;
   gwinpt->vy.modwin.xmax = xmax;
   gwinpt->vy.modwin.ymin = ymin;
   gwinpt->vy.modwin.ymax = ymax;
   WPnrgw(gwinpt);
/*
***Update the display.
*/
   WPrepaint_GWIN((wpw_id)gwinpt->id.w_id);
   status = 0;
/*
***The end.
*/
exit:
   actfunc = actfunc_org;
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short WPupvi(
        char   *name,
        WPVIEW *newwin)

/*      Anropas av SCL_VIEW() och CEN_VIEW().
 *      Uppdaterar alla fönster som har den aktuella vyn
 *      med ett nytt modellfönster.
 *
 *      In: name   = Vyns namn.
 *          newwin = Nytt modellfönster.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1332 = Vyn %s finns ej.
 *
 *      (C)microform ab 1996-11-03 J. Kjellander
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
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN )
       {
/*
***Ett grafiskt fönster har hittats. Är vyn "name" aktiv
***i detta fönster ?
*/
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Om så är fallet uppdaterar vi dess modellfönster.
*/
       if ( strcmp(gwinpt->vy.name,name ) == 0 )
         {
         gwinpt->vy.modwin.xmin = newwin->modwin.xmin;
         gwinpt->vy.modwin.xmax = newwin->modwin.xmax;
         gwinpt->vy.modwin.ymin = newwin->modwin.ymin;
         gwinpt->vy.modwin.ymax = newwin->modwin.ymax;
         WPnrgw(gwinpt);
         }
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPuppr(
        char  *name,
        double dist)

/*      Anropas av PERP_VIEW().
 *      Uppdaterar alla fönster som har den aktuella vyn
 *      med ett nytt perpektivasvstånd.
 *
 *      In: name   = Vyns namn.
 *          dist   = Nytt perspektivavstånd.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1332 = Vyn %s finns ej.
 *
 *      (C)microform ab 1999-02-02 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster och uppdatera dom med rätt namn.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&  winptr->typ == TYP_GWIN )
       {
       gwinpt = (WPGWIN *)winptr->ptr;
       if ( strcmp(gwinpt->vy.name,name ) == 0 ) gwinpt->vy.pdist = dist;
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

  static void drwarr(
         WPGWIN *gwinpt,
         GC      arr_gc,
         int     ix1,
         int     iy1,
         int     ix2,
         int     iy2)

/*      Ritar/suddar gummibands-pil.
 *
 *      In:  gwinpt = C-pekare till grafiskt fönster.
 *           arr_gc = GC för pilspets.
 *           ix1    = X-koordinat 1.
 *           iy1    = Y-koordinat 1.
 *           ix2    = X-koordinat 2.
 *           iy2    = Y-koordinat 2.
 *
 *      Ut:  Inget.
 *
 *      (C)microform ab 4/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int    dx,dy;
   double alfa,cosalf,sinalf;

/*
***Beräkna vinkeln mellan gummibandslinjen och positiva
***X-axeln.
*/
   dx = ix2 - ix1;
   dy = iy2 - iy1;

   if ( dx == 0  &&  dy > 0 ) alfa = PI05;
   else if ( dx == 0 ) alfa = -PI05;
   else
     {
     if ( dx > 0 )
       {
       if ( dy >= 0 ) alfa =  ATAN((double)dy/(double)dx);
       else           alfa = -ATAN((double)-dy/(double)dx);
       }
     else
       {
       if ( dy >= 0 ) alfa = PI - ATAN((double)dy/(double)-dx);
       else           alfa = ATAN((double)-dy/(double)-dx) - PI;
       }
     }
/*
***Rita själva pilen.
*/
    XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->rub_gc,
                                     ix1,iy1,ix2,iy2);
/*
***Pilspetsen
*/
    cosalf = cos(alfa);
    sinalf = sin(alfa);

    dx = (int)(-25.0*cosalf + 10.0*sinalf);
    dy = (int)(-10.0*cosalf - 25.0*sinalf);

    XDrawLine(xdisp,gwinpt->id.x_id,arr_gc,
                                     ix2,iy2,ix2+dx,iy2+dy);

    dx = (int)(-25.0*cosalf - 10.0*sinalf);
    dy = (int)( 10.0*cosalf - 25.0*sinalf);

    XDrawLine(xdisp,gwinpt->id.x_id,arr_gc,
                                     ix2,iy2,ix2+dx,iy2+dy);

    XFlush(xdisp);
  }

/********************************************************/
