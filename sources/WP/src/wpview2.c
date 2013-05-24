/**********************************************************************
*
*    wpview2.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPlstv();  Interactive Last view
*    WPperp();  Interactive PERP_VIEW
*    WPcent();  Interactive CEN_VIEW
*    WPscle();  Interactive SCL_VIEW
*    WPacvi();  ACT_VIEW in MBS
*    WPupvi();  Uppdate view after SCL_VIEW or CEN_VIEW
*    WPuppr();  Uppdate view after PERP_VIEW
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

extern VY          vytab[];

static int vyindx(char vynamn[]);
static void drwarr(WPGWIN *gwinpt, GC arr_gc, int ix1, int iy1,
                   int ix2, int iy2);

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
   WPVY tmpvy;

/*
***Om det finns en föregående vy låter vi den och nuvarande vy
***byta plats annar gör vi ingenting.
*/
   if ( gwinpt->old_vy.valid )
     {
     V3MOME(&gwinpt->vy,&tmpvy,sizeof(WPVY));
     V3MOME(&gwinpt->old_vy,&gwinpt->vy,sizeof(WPVY));
     V3MOME(&tmpvy,&gwinpt->old_vy,sizeof(WPVY));
/*
***Uppdatera fönsterramen.
*/
     WPupwb(gwinpt);
/*
***Uppdatera fönstret på skärmen.
*/
     WPrepa((wpw_id)gwinpt->id.w_id);
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

   if ( WPstrl(rubrik) + WPstrl(gwinpt->vy.vynamn) > altlen )
     altlen = WPstrl(rubrik) + WPstrl(gwinpt->vy.vynamn);
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
   strcat(rubrik,gwinpt->vy.vynamn);
   WPposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Ramlös button för persp.
*/
   alt_x  = ly;
   alt_y  = ly;
   altlen = WPstrl(persp);
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)0,persp,persp,"",WP_BGND,WP_FGND,&dum_id);
/*
***Toggle-button för ON/OFF.
*/
   alt_x = alt_x + WPstrl(persp) + lm;
   if ( WPstrl(on) > WPstrl(off) ) altlen = (short)(WPstrl(on)  + 15);
   else                            altlen = (short)(WPstrl(off) + 15);

   dsval = gwinpt->vy.vydist;
   if ( dsval == 0.0 ) diston = FALSE;
   else                diston = TRUE;

   if ( diston == FALSE )
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)1,off,on,"",WP_BGND,WP_FGND,&onoff_id);
/*
***Om perspektiv redan är satt skall avståndet visas.
*/
   else
     {
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)1,on,off,"",WP_BGND,WP_FGND,&onoff_id);
     alt_x  = ly;
     alt_y  = ly + alth + ly;
     altlen = WPstrl(dist);
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     (short)0,dist,dist,"",WP_BGND,WP_FGND,&dist_id);

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
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,okey,okey,"",WP_BGND,WP_FGND,&okey_id);

   alt_x  = (short)(alt_x + altlen + lm);
   altlen = (short)(WPstrl(reject) + 15);
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,reject,reject,"",WP_BGND,WP_FGND,&reject_id);

   altlen = (short)(WPstrl(help) + 15);
   alt_x  = (short)(main_dx - altlen - lm);
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)2,help,help,"",WP_BGND,WP_FGND,&help_id);
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

     V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));
     gwinpt->vy.vydist = dsval;
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
     ighelp();
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
       WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                       (short)0,dist,dist,"",WP_BGND,WP_FGND,&dist_id);

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
   WPrepa((wpw_id)gwinpt->id.w_id);

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
   igptma(310,IG_MESS);
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
       igrsma();

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

       igptma(133,IG_MESS);
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
       igrsma();
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
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

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
   WPrepa((wpw_id)gwinpt->id.w_id);
/*
***Utgång. Återställ eventmask och cursor.
*/
exit:
   XSelectInput(xdisp,gwinpt->id.x_id,GWEM_NORM);
   WPscur(gwinpt->id.w_id,FALSE,(Cursor)0);
   XFreeGC(xdisp,Arr_GC);

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short WPscle(
        WPGWIN *gwinpt,
        int     x,
        int     y)

/*      Varkon-funktion för skala med X-Windows.
 *
 *      In: gwinpt = Pekare till fönstret som skall skalas.
 *          x,y    = Aktiverande knapps/ikons rootposition.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      Felkod: IG3042 = Kan ej minska skalan mera.
 *
 *      (C)microform ab 4/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],dubbla[81],half[81],reject[81],help[81],
            scale[81];
   int      main_x,main_y;
   short    main_dx,main_dy,alt_x,alt_y,alth,altlen,ly,lm;
   DBint    iwin_id,scale_id,one_id,but_id,help_id,
            dubbla_id,half_id,reject_id;
   double   skala,xmin,xmax,ymin,ymax,mdx,mdy;

/*
***Texter för rubrik, dubbla, halva, avbryt och hjälp.
*/
   if ( !WPgrst("varkon.scale.title",rubrik) )  strcpy(rubrik,"Skala");
   if ( !WPgrst("varkon.scale.double",dubbla) ) strcpy(dubbla,"Dubbla");
   if ( !WPgrst("varkon.scale.half",half) )     strcpy(half,"Halva");
   if ( !WPgrst("varkon.input.reject",reject) ) strcpy(reject,"Avbryt");
   if ( !WPgrst("varkon.input.help",help) )     strcpy(help,"Hj{lp");
/*
***För att nu kunna bestämma storleken på själva
***skala-fönstret utgår vi från  2 kolumner med
***knappar där knapparnas längd bestäms av den längsta
***alternativtexten.
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
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/
   ly   = (short)(0.8*WPstrh());
   alth = (short)(1.6*WPstrh()); 
   lm   = (short)(1.4*WPstrh());

   main_dx = (short)(ly + altlen + lm + altlen + ly);
   main_dy = (short)(ly + 2*(alth + ly) +  ly + 2*WPstrh() + ly);  
/*
***Skapa själva alternativfönstret som ett WPIWIN.
*/
   WPposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Edit-knapp för godtycklig skala.
***Beräkna nuvarande skala.
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
***Button för skala = 1.0.
*/
   alt_x  = ly + altlen + lm;
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                           "1.0","1.0","",WP_BGND,WP_FGND,&one_id);
/*
***Dubbla och halva.
*/
   alt_x  = ly;
   alt_y  = ly + alth + ly;
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                           dubbla,dubbla,"",WP_BGND,WP_FGND,&dubbla_id);

   alt_x  = ly + altlen + lm;
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)1,
                          half,half,"",WP_BGND,WP_FGND,&half_id);
/*
***Avbryt och hjälp.
*/
   alt_x  = (short)(ly);
   alt_y  = (short)(ly + 2*(alth + ly) + ly);
   alth   = (short)(2*WPstrh());
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)2,
                           reject,reject,"",WP_BGND,WP_FGND,&reject_id);

   alt_x  = ly + altlen + lm;
   WPmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,(short)2,
                           help,help,"",WP_BGND,WP_FGND,&help_id);
/*
***Klart för visning.
*/
   WPwshw(iwin_id);
/*
***Vänta på action. Service-nivå för key-event saknar
***intresse i sammanhanget.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Avbryt.
*/
   if ( but_id == reject_id )
     {
     WPwdel(iwin_id);
     return(REJECT);
     }
/*
***Hjälp.
*/
   else if ( but_id == help_id )
     {
     ighelp();
     goto loop;
     }
/*
***Inmatad skala.
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
***Skala = 1.0
*/
   else if ( but_id == one_id ) skala = 1.0;
/*
***Dubbla.
*/
   else if ( but_id == dubbla_id ) skala *= 2.0;
/*
***Halva.
*/
   else if ( but_id == half_id ) skala /= 2.0;
/*
***Dags att sluta.
*/
   WPwdel(iwin_id);
/*
***Vi har nu en ny skalfaktor och kan beräkna ett nytt modellfönster.
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
***Uppdatera WPGWIN-posten.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.modwin.xmin = xmin;
   gwinpt->vy.modwin.xmax = xmax;
   gwinpt->vy.modwin.ymin = ymin;
   gwinpt->vy.modwin.ymax = ymax;
   WPnrgw(gwinpt);
/*
***Uppdatera skärmen.
*/
   WPrepa((wpw_id)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPacvi(
        char   name[],
        DBint  win_id)

/*      Interface-rutin för ACT_VIEW. Aktiverar en vy ur
 *      vytab[] i ett visst grafiskt fönster.
 *
 *      In: name   => Vyns namn.
 *          win_id => ID för grafiskt fönster.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1332 = Vyn %s finns ej.
 *                WP1342 = Fönstret %s finns ej.
 *                WP1352 = Fönstret %s är ej ett WPGWIN.
 *                WP1412 = Vyn %s är korrupt.
 *
 *      (C)microform ab 21/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i,j;
   char     errbuf[81];
   DBVector por,pu1,pu2;
   DBTmat   vymat;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;

/*
***Var i vytab finns vyn ?
*/
   if ( (i=vyindx(name)) == -1 ) return(erpush("WP1332",name));
/*
***Finns fönstret i wpwtab[] ?
*/
   if ( (winptr=WPwgwp((wpw_id)win_id)) == NULL )
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1342",errbuf));
     }
/*
***Är det ett WPGWIN ? Om så, fixa C-pekare till fönstret.
*/
   if ( winptr->typ != TYP_GWIN )
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1352",errbuf));
     }
   else gwinpt = (WPGWIN *)winptr->ptr;
/*
***Allt är OK.  Om detta är huvudfönstret uppdaterar vi vytab
***med den gamla vyn:s modellfönster och perspektivavstånd
***(om den finns i vytab).
*/
   if ( win_id == GWIN_MAIN  &&  (j=vyindx(gwinpt->vy.vynamn)) >= 0 )
     {
     vytab[j].vywin[0] = gwinpt->vy.modwin.xmin;
     vytab[j].vywin[1] = gwinpt->vy.modwin.ymin;
     vytab[j].vywin[2] = gwinpt->vy.modwin.xmax;
     vytab[j].vywin[3] = gwinpt->vy.modwin.ymax;
     vytab[j].vydist   = gwinpt->vy.vydist;
     }
/*
***Nu uppdaterar vi WPGWIN-posten med data
***från vytab. Om vy:n är av typen betraktelseposition
***måste matris beräknas. Observera att detta gäller
***även i 2D tex. för offsetkurvor.
*/
   strcpy(gwinpt->vy.vynamn,vytab[i].vynamn);
   gwinpt->vy.vy_3D  = vytab[i].vy3d;
   gwinpt->vy.vydist = vytab[i].vydist;
/*
***Lite felkontroller innan vi sätter upp modellfönstret.
*/
   if ( vytab[i].vywin[2] <= vytab[i].vywin[0] )
     return(erpush("WP1412",name));
   if ( vytab[i].vywin[3] <= vytab[i].vywin[1] )
     return(erpush("WP1412",name));

   gwinpt->vy.modwin.xmin = vytab[i].vywin[0];
   gwinpt->vy.modwin.xmax = vytab[i].vywin[2];
   gwinpt->vy.modwin.ymin = vytab[i].vywin[1];
   gwinpt->vy.modwin.ymax = vytab[i].vywin[3];
/*
***Vyn definieras av en betraktelseposition. Vymatris
***beräknas här med samma metod som i grapac/gp1/gpstvi().
***Beräkna med hjälp av vyvektorn två nya vektorer som
***spänner upp X/Y-planet i den önskade vyn.
*/
   if ( vytab[i].vytypp )
     {
     pu1.y_gm = 0.0;
     if ( (vytab[i].vyrikt.x_vy*vytab[i].vyrikt.x_vy +
           vytab[i].vyrikt.z_vy*vytab[i].vyrikt.z_vy) < 0.001 )
       {
       pu1.x_gm = 0.0; pu1.z_gm = 1.0; pu2.x_gm = 1.0;
       pu2.y_gm = 0.0; pu2.z_gm = 0.0;
       }
     else
       {
       pu1.x_gm = vytab[i].vyrikt.z_vy;
       pu1.z_gm = 0.0;
       if ( vytab[i].vyrikt.x_vy != 0.0 ) pu1.z_gm = -vytab[i].vyrikt.x_vy;
       if ( (vytab[i].vyrikt.y_vy*vytab[i].vyrikt.y_vy) < 0.001 )
         {
         pu2.x_gm = 0.0; pu2.y_gm = 1.0; pu2.z_gm = 0.0;
         }
       else
         {
         pu2.x_gm = vytab[i].vyrikt.z_vy*pu1.y_gm +
                    vytab[i].vyrikt.y_vy*pu1.z_gm;
         pu2.y_gm = vytab[i].vyrikt.z_vy*pu1.x_gm -
                    vytab[i].vyrikt.x_vy*pu1.z_gm;
         pu2.z_gm = vytab[i].vyrikt.x_vy*pu1.y_gm -
                    vytab[i].vyrikt.y_vy*pu1.x_gm;
         }
       }
/*
***Bilda transformationsmatris.
*/
     por.x_gm = 0.0; por.y_gm = 0.0; por.z_gm = 0.0;
     GEmktf_3p(&por,&pu1,&pu2,&vymat);

     gwinpt->vy.vymat.k11 = vymat.g11;
     gwinpt->vy.vymat.k12 = vymat.g12;
     gwinpt->vy.vymat.k13 = vymat.g13;
     gwinpt->vy.vymat.k21 = vymat.g21;
     gwinpt->vy.vymat.k22 = vymat.g22;
     gwinpt->vy.vymat.k23 = vymat.g23;
     gwinpt->vy.vymat.k31 = vymat.g31;
     gwinpt->vy.vymat.k32 = vymat.g32;
     gwinpt->vy.vymat.k33 = vymat.g33;
     }
/*
***Det är en matris-vy, då blir det enklare.
*/
   else
     {
     gwinpt->vy.vymat.k11 = vytab[i].vymatr.v11;
     gwinpt->vy.vymat.k12 = vytab[i].vymatr.v12;
     gwinpt->vy.vymat.k13 = vytab[i].vymatr.v13;
     gwinpt->vy.vymat.k21 = vytab[i].vymatr.v21;
     gwinpt->vy.vymat.k22 = vytab[i].vymatr.v22;
     gwinpt->vy.vymat.k23 = vytab[i].vymatr.v23;
     gwinpt->vy.vymat.k31 = vytab[i].vymatr.v31;
     gwinpt->vy.vymat.k32 = vytab[i].vymatr.v32;
     gwinpt->vy.vymat.k33 = vytab[i].vymatr.v33;
     }
/*
***Normalisera.
*/
   WPnrgw(gwinpt);
/*
***Uppdatera fönsterramen.
*/
   WPupwb(gwinpt);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPupvi(
        char *name,
        VY   *newwin)

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
       if ( strcmp(gwinpt->vy.vynamn,name ) == 0 )
         {
         gwinpt->vy.modwin.xmin = newwin->vywin[0];
         gwinpt->vy.modwin.xmax = newwin->vywin[2];
         gwinpt->vy.modwin.ymin = newwin->vywin[1];
         gwinpt->vy.modwin.ymax = newwin->vywin[3];
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
       if ( strcmp(gwinpt->vy.vynamn,name ) == 0 ) gwinpt->vy.vydist = dist;
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int vyindx(
        char vynamn[])

/*
 *      In: Vynamn.
 *
 *      Ut: Inget.
 *
 *      FV:  Noll eller större => Vyns plats i vytab.
 *           -1 => Vyn finns ej.
 *
 *      (C)microform ab 21/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;
/*
***Vyer med namnet "" (tom sträng) får ej finnas i vytab.
*/
   if ( vynamn[0] == '\0' ) return(-1);
/*
***Sök igenom hela vytab.
*/
   for ( i=0; i<GPMAXV; ++i)
     if ( strcmp(vynamn,vytab[i].vynamn) == 0 ) return(i);
  
   return(-1);
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
