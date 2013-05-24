/**********************************************************************
*
*    ms17.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     mslstv()    Previous view
*     msperp();   Perspective
*     mscent();   Pan view
*     msscle();   Scale view
*     msnivs();   Set levels
*     msacvi();   Activate view
*     msupvi();   SCL_VIEW and CEN_VIEW in MBS
*     msuppr();   PERP_VIEW in MBS
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
#include "../../../sources/GE/include/GE.h"
#include "../../../sources/GP/include/GP.h"
#include "../../../sources/WP/include/WP.h"

static int    msmtsl();  /* Tänder/Släcker/Listar nivåer */
static int    vyindy();  /* Översätter vynamn -> index i vytab */
static void   drwarr();  /* Ritar panoreringspil */

extern VY      vytab[];
extern tbool   nivtb1[];
extern NIVNAM  nivtb2[];
extern HCURSOR ms_grcu;

/*!******************************************************/

        int     mslstv(gwinpt)
        WPGWIN *gwinpt;

/*      Varkon-funktion för föregående vy.
 *
 *      In: gwinpt = Pekare till fönstret som innehåller vyn.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *
 *      (C)microform ab 9/11/95 J. Kjellander
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
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
     if ( gwinpt->id.w_id == GWIN_MAIN )
       wpupgp(gwinpt);
/*
***Uppdatera fönsterramen.
*/
     msupwb(gwinpt);
/*
***Uppdatera fönstret på skärmen.
*/
     msrepa((wpw_id)gwinpt->id.w_id);
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int     msperp(gwinpt,x,y)
        WPGWIN *gwinpt;
        int     x,y;

/*      Varkon-funktion för inställning av perspektiv
 *      i viss vy.
 *
 *      In: gwinpt = Pekare till fönstret som innehåller vyn.
 *          x,y    = Aktiverande knapps/ikons position.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 9/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],persp[81],dist[81],okey[81],reject[81],help[81],
            on[81],off[81],valstr[81];
   int      main_dx,main_dy,alt_x,alt_y,alth,altlen,ly,lm;
   v2int    iwin_id,dum_id,onoff_id,but_id,help_id,okey_id,reject_id,
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
   if ( !msgrst("PERSP.TITLE",rubrik) ) strcpy(rubrik,"Perspektiv för vy : ");
   if ( !msgrst("PERSP.PERSP",persp) ) strcpy(persp,"Sant perspektiv");
   if ( !msgrst("PERSP.DIST",dist) ) strcpy(dist,"Avstånd");
   if ( !msgrst("INPUT.ON",on) ) strcpy(on,"På");
   if ( !msgrst("INPUT.OFF",off) ) strcpy(off,"Av");
   if ( !msgrst("INPUT.OKEY",okey) ) strcpy(okey,"Okej");
   if ( !msgrst("INPUT.REJECT",reject) ) strcpy(reject,"Avbryt");
   if ( !msgrst("INPUT.HELP",help) ) strcpy(help,"Hjälp");
/*
***Längsta texten avgör fönstrets bredd.
*/
   altlen = 0;

   if ( msstrl(rubrik) + msstrl(gwinpt->vy.vynamn) > altlen )
     altlen = msstrl(rubrik) + msstrl(gwinpt->vy.vynamn);
   if ( msstrl(persp) + msstrl(on) > altlen )
     altlen = msstrl(persp) + msstrl(on);
   if ( msstrl(dist) + msstrl("1234567890123") > altlen )
     altlen = msstrl(dist) + msstrl("1234567890123");
   if ( msstrl(okey) + msstrl(reject) + msstrl(help) > altlen )
     altlen = msstrl(okey) + msstrl(reject) + msstrl(help);
/*
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/
   ly   = (int)(0.8*msstrh());
   alth = (int)(1.6*msstrh()); 
   lm   = (int)(1.4*msstrh());

   main_dx = (int)(ly + altlen + 2*lm + ly);
   main_dy = (int)(ly + 2*(alth + ly) +  ly + 2*msstrh() + ly);  
/*
***Skapa själva perspektivfönstret som ett WPIWIN.
*/
   strcat(rubrik,gwinpt->vy.vynamn);
   mswciw(x,y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Ramlös button för persp.
*/
   alt_x  = ly;
   alt_y  = ly;
   altlen = msstrl(persp)+5;
   msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   0,persp,persp,"",WP_BGND,WP_FGND,&dum_id);
/*
***Toggle-button för ON/OFF.
*/
   alt_x = alt_x + msstrl(persp) + lm;
   if ( msstrl(on) > msstrl(off) ) altlen = msstrl(on)  + 15;
   else                            altlen = msstrl(off) + 15;

   dsval = gwinpt->vy.vydist;
   if ( dsval == 0.0 ) diston = FALSE;
   else                diston = TRUE;

   if ( diston == FALSE )
     msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     1,off,on,"",WP_BGND,WP_FGND,&onoff_id);
/*
***Om perspektiv redan är satt skall avståndet visas.
*/
   else
     {
     msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     1,on,off,"",WP_BGND,WP_FGND,&onoff_id);
     alt_x  = ly;
     alt_y  = ly + alth + ly;
     altlen = msstrl(dist)+5;
     msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                     0,dist,dist,"",WP_BGND,WP_FGND,&dist_id);

     alt_x  = ly + altlen + 10;
     altlen = msstrl("1234567890123");
     sprintf(valstr,"%g",dsval);
     msmced((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                             valstr,15,&value_id);
     }
/*
***Okey, avbryt och hjälp.
*/
   alt_x  = ly;
   alt_y  = ly + 2*(alth + ly) + ly;
   alth   = 2*msstrh();
   altlen = msstrl(okey) + 15;
   mscrdb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   2,okey,"",WP_BGND,WP_FGND,&okey_id);

   alt_x  = alt_x + altlen + lm;
   altlen = msstrl(reject) + 15;
   msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   2,reject,reject,"",WP_BGND,WP_FGND,&reject_id);

   altlen = msstrl(help) + 15;
   alt_x  = main_dx - altlen - lm;
   msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   2,help,help,"",WP_BGND,WP_FGND,&help_id);
/*
***Klart för visning.
*/
   mswshw(iwin_id);
/*
***Vänta på action.
*/
loop:
   mswwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Ok.
*/
   if ( but_id == okey_id )
     {
     if ( diston == TRUE )
       {
       msgted(iwin_id,value_id,valstr);
       if ( sscanf(valstr,"%lf",&dsval) != 1 )
         {
         Beep(1000,1000);
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
     mswdel(iwin_id);
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
       msgted(iwin_id,value_id,valstr);
       if ( sscanf(valstr,"%lf",&dsval) != 1 )
         {
         Beep(1000,1000);
         goto loop;
         }
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
       altlen = msstrl(dist)+5;
       msmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                       0,dist,dist,"",WP_BGND,WP_FGND,&dist_id);

       alt_x  = ly + altlen + 10;
       altlen = msstrl("1234567890123");
       sprintf(valstr,"%g",dsval);
       msmced((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                               valstr,15,&value_id);

       winptr = mswgwp((wpw_id)iwin_id);
       iwinpt = (WPIWIN *)winptr->ptr;
       buttpt = (WPBUTT *)iwinpt->wintab[dist_id].ptr;
       editpt = (WPEDIT *)iwinpt->wintab[value_id].ptr;
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
   mswdel(iwin_id);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
   if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
/*
***Uppdatera skärmen.
*/
   msrepa((wpw_id)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int mscent(gwinpt)
        WPGWIN *gwinpt;

/*      Varkon-funktion för panorering.
 *
 *      In: gwinpt = Pekare till fönstret som skall panoreras.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 9/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    bool    down,update;
    int     org_mode,status,x1,y1,curx2,cury2,lastx2,lasty2;
    double  mdx,mdy;
    MSG     message;
    HPEN    dash_pen,old_pen;

/*
***Ett meddelande.
*/
   igptma(310,IG_MESS);
/*
***Lite initiering.
*/
   down = update = FALSE;

   ms_grcu = LoadCursor(NULL,IDC_CROSS);
   SetCapture(gwinpt->id.ms_id);
   org_mode = GetROP2(gwinpt->dc);
   SetROP2(gwinpt->dc,R2_XORPEN);
   dash_pen = CreatePen(PS_DOT,1,RGB(0,0,0));
   old_pen = SelectObject(gwinpt->dc,dash_pen);
/*
***Meddelandeloop.
*/
loop:
   if ( GetMessage(&message,NULL,0,0) == TRUE )
     {
/*
***Om ett meddelande fanns att hämta skickar vi det vidare
***till fönstrets callback-rutin. Vissa meddelanden servas där
***men några lämnas kvar till längre ner.
*/
     TranslateMessage(&message);
     DispatchMessage(&message);

     switch ( ms_lstmes.msg )
	    {
/*
***Vänster musknapp trycks ned.
*/
       case WM_LBUTTONDOWN:
       x1 = curx2 = lastx2 = LOWORD(ms_lstmes.lp);
       y1 = cury2 = lasty2 = HIWORD(ms_lstmes.lp);
       igrsma();
       igptma(133,IG_MESS);
       down = TRUE;

       Arc(gwinpt->dc,x1-7,y1-7,x1+7,y1+7,x1+7,y1+7,x1+7,y1+7);
       drwarr(gwinpt,x1,y1,x1,y1);
       goto loop;
/*
***Höger musknapp trycks ned = REJECT.
*/
       case WM_RBUTTONDOWN:
       igrsma();
       status = REJECT;
       goto exit;

/*
***Nu flyttar sig musen.
*/
       case WM_MOUSEMOVE:
       if ( down )
         {
         curx2 = LOWORD(ms_lstmes.lp);
         cury2 = HIWORD(ms_lstmes.lp);
/*
***Det bör inte vara tillåtet att gå ut utanför fönstret.
*/
         if ( curx2 < (int)gwinpt->vy.scrwin.xmin )
              curx2 = (int)gwinpt->vy.scrwin.xmin;

         if ( curx2 > (int)gwinpt->vy.scrwin.xmax )
              curx2 = (int)gwinpt->vy.scrwin.xmax;

         if ( cury2 < (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax) )
              cury2 = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax);

         if ( cury2 > (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin) )
              cury2 = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin);
/*
***Vi är fortfarande kvar inuti fönstret. Rita om pilen.
*/
         if ( curx2 != lastx2  || cury2 != lasty2 )
           {
           drwarr(gwinpt,x1,y1,lastx2,lasty2);
           if ( curx2 != x1  || cury2 != y1 )
             drwarr(gwinpt,x1,y1,curx2,cury2);
           lastx2 = curx2;
           lasty2 = cury2;
           }
         }
       goto loop;
/*
***När knappen släpps igen är det slut.
*/
       case WM_LBUTTONUP:
       if ( down )
         {
         curx2 = LOWORD(ms_lstmes.lp);
         cury2 = HIWORD(ms_lstmes.lp);

         if ( curx2 < (int)gwinpt->vy.scrwin.xmin )
              curx2 = (int)gwinpt->vy.scrwin.xmin;

         if ( curx2 > (int)gwinpt->vy.scrwin.xmax )
              curx2 = (int)gwinpt->vy.scrwin.xmax;

         if ( cury2 < (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax) )
              cury2 = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax);

         if ( cury2 > (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin) )
              cury2 = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin);

         igrsma();

         if ( lastx2 != x1  || lasty2 != y1 )
           drwarr(gwinpt,x1,y1,lastx2,lasty2);
         Arc(gwinpt->dc,x1-7,y1-7,x1+7,y1+7,x1+7,y1+7,x1+7,y1+7);
/*
***Varkons y-axel är motsatt X11.
*/
         y1    = gwinpt->geo.dy - y1;
         cury2 = gwinpt->geo.dy - cury2;
         update = TRUE;
         goto exit;
         }
       else goto loop;
       }
     goto loop;
     }
/*
***Slut, återställ GC:et.
*/
exit:
   ReleaseCapture();
   SetROP2(gwinpt->dc,org_mode);
   SelectObject(gwinpt->dc,old_pen);
   DeleteObject(dash_pen);
   ms_grcu = LoadCursor(NULL,IDC_ARROW);
/*
***Ska vi flytta bilden och i så fall hur mycket ?
*/
   if ( update )
     {
     V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

     mdx = ((double)(curx2 - x1)/(double)gwinpt->geo.dx) *
           (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
     mdy = ((double)(cury2 - y1)/(double)gwinpt->geo.dy) *
           (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);

     gwinpt->vy.modwin.xmin -= mdx;
     gwinpt->vy.modwin.xmax -= mdx;
     gwinpt->vy.modwin.ymin -= mdy;
     gwinpt->vy.modwin.ymax -= mdy;

     msnrgw(gwinpt);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
     if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
/*
***Uppdatera skärmen.
*/
     msrepa((wpw_id)gwinpt->id.w_id);
     }

   return(status);
  }

/********************************************************/
/*!******************************************************/

        int     msscle(gwinpt,x,y)
        WPGWIN *gwinpt;
        int     x,y;

/*      Varkon-funktion för skala.
 *
 *      In: gwinpt = Pekare till fönstret som skall skalas.
 *          x,y    = Aktiverande knapps/ikons position.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      Felkod: IG3042 = Kan ej minska skalan mera.
 *
 *      (C)microform ab 10/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   double   skala,xmin,xmax,ymin,ymax,mdx,mdy;

/*
***Beräkna nuvarande skala.
*/
   mdx   =  gwinpt->geo.psiz_x *
           (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   mdy   =  gwinpt->geo.psiz_y *
           (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   skala = mdx/(gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
/*
***Läs in ny skala.
*/
   if ( msdl02(&skala,gwinpt->vy.vynamn) < 0 ) return(REJECT);
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
   msnrgw(gwinpt);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
   if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
/*
***Uppdatera skärmen.
*/
   msrepa((wpw_id)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int     msnivs(gwinpt,x,y)
        WPGWIN *gwinpt;
        int     x,y;

/*      Varkon-funktion för nivåer.
 *
 *      In: gwinpt = Pekare till grafiskt fönster.
 *          x,y    = Aktiverande knapps/ikons position.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      Felkod:
 *
 *      (C)microform ab 1996-12-12 J. Kjellander
 *
 ******************************************************!*/

  {
  int level,count,mode;

/*
***Poppa upp dialogbox 7.
*/
   if ( msdl07(&level,&count,&mode) < 0 ) return(REJECT);
/*
***Fixa nivåerna.
*/
   msmtsl(gwinpt->id.w_id,level,count,mode);
/*
***Uppdatera skärmen, såvida det inte är fråga om en listning förstås.
*/
   if ( mode != WP_LISTL) msrepa((wpw_id)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int msmtsl(win_id,first,count,mode)
        v2int win_id;
        int   first,count,mode;

/*      Tänder/Släcker/Listar nivåer.
 *
 *      In: win_id = ID för aktuellt fönster.
 *          first  = Första nivå
 *          count  = Antal
 *          mode   = WP_BLANKL  => Släck nivåer.
 *                   WP_UBLANKL => Tänd nivåer.
 *                   WP_LISTL   => Lista nivåer.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1392 = Fönster med id %s finns ej
 *
 *      (C)microform ab 1996-12-12 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i,j,start,stop,inc,bytofs,bitmsk;
   char    buf[81],errbuf[81];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Fixa C-pekare till WPGWIN-fönstret.
*/
   if ( (winptr=mswgwp((wpw_id)win_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1392",errbuf));
     }
/*
***Framåt eller bakåt ?
*/
   if ( count > 0 )
     {
     start = first;
     stop  = first + count;
     inc   = 1;
     }
   else
     {
     start =  first;
     stop  =  first + count;
     inc   = -1;
     } 
/*
***Vad skall göras ?
*/ 
   switch ( mode )
     {
/*
***Tänd.
*/
     case WP_UBLANKL:

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = bitmsk<<(i&7);
       gwinpt->nivtab[bytofs] |= bitmsk;
       if ( win_id == GWIN_MAIN ) nivtb1[i] = FALSE;
       }

     break;
/*
***Släck.
*/
     case WP_BLANKL:

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = ~(bitmsk<<(i&7));
       gwinpt->nivtab[bytofs] &= bitmsk;
       if ( win_id == GWIN_MAIN ) nivtb1[i] = TRUE;
       }

     break;
/*
***Lista.
*/
     case WP_LISTL:
     msinla("                    ");

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = bitmsk<<(i&7);
       if ( gwinpt->nivtab[bytofs] & bitmsk )
         sprintf(buf,"%4d = %-15s",i,iggtts(225));
       else
         sprintf(buf,"%4d = %-15s",i,iggtts(226));

       for ( j=0; j<NT2SIZ; ++j )
         { 
         if ( nivtb2[j].nam[0] != '\0'  &&  nivtb2[j].num == i )
           strcat(buf,nivtb2[j].nam);
         }
       msalla(buf,(short)1);
       }

     msexla(TRUE);
     break;
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int    msacvi(name,win_id)
        char   name[];
        v2int  win_id;

/*      Interface-rutin för ACT_VIEW. Aktiverar en vy ur
 *      vytabÄÅ i ett visst grafiskt fönster.
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
 *      (C)microform ab 3/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i,j;
   char    errbuf[81];
   VY      newvy;
   DBVector   por,pu1,pu2;
   DBTmat  vymat;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Var i vytab finns vyn?
*/
   if ( (i=vyindy(name)) == -1 ) return(erpush("WP1332",name));
/*
***Finns fönstret i wpwtabÄÅ?
*/
   if ( (winptr=mswgwp((wpw_id)win_id)) == NULL )
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
   if ( win_id == GWIN_MAIN  &&  (j=vyindy(gwinpt->vy.vynamn)) >= 0 )
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
   msnrgw(gwinpt);
/*
***Om det är huvudfönstret som skall få den aktuella vyn
***måste även grapac få veta om det. Då sätter vi inte upp
***det modellfönster som finns i vytab utan det som fönstret
***själv fått efter normalisering så att actvy å sånt får
***rätt värden.
*/
   if ( win_id == GWIN_MAIN )
     {
     V3MOME(&vytab[i],&newvy,sizeof(VY));

     newvy.vywin[0] = gwinpt->vy.modwin.xmin;
     newvy.vywin[1] = gwinpt->vy.modwin.ymin;
     newvy.vywin[2] = gwinpt->vy.modwin.xmax;
     newvy.vywin[3] = gwinpt->vy.modwin.ymax;

     gpswin(&newvy);
     gpstvi(&newvy);
     }
/*
***Uppdatera fönsterramen.
*/
   msupwb(gwinpt);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int   msupvi(name,newwin)
        char *name;
        VY   *newwin;

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
     if ( (winptr=mswgwp((wpw_id)i)) != NULL  &&
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
         msnrgw(gwinpt);
         }
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int   msuppr(name,dist)
        char  *name;
        double dist;

/*      Anropas av PERP_VIEW.
 *      Uppdaterar alla fönster som har den aktuella vyn
 *      med ett nytt perspektivavstånd.
 *
 *      In: name = Vyns namn.
 *          dist = Nytt avstånd..
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
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=mswgwp((wpw_id)i)) != NULL  &&  winptr->typ == TYP_GWIN )
       {
       gwinpt = (WPGWIN *)winptr->ptr;
       if ( strcmp(gwinpt->vy.vynamn,name ) == 0 ) gwinpt->vy.vydist = dist;
       }
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int vyindy(vynamn)
        char vynamn[];

/*
 *      In: Vynamn.
 *
 *      Ut: Inget.
 *
 *      FV:  Noll eller större => Vyns plats i vytab.
 *           -1 => Vyn finns ej.
 *
 *      (C)microform ab 3/11/95 J. Kjellander
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

  static void drwarr(gwinpt,ix1,iy1,ix2,iy2)
         WPGWIN *gwinpt;
         int     ix1,iy1,ix2,iy2;

/*      Ritar/suddar gummibands-pil.
 *
 *      In:  gwinpt = C-pekare till grafiskt fönster.
 *           ix1    = X-koordinat 1.
 *           iy1    = Y-koordinat 1.
 *           ix2    = X-koordinat 2.
 *           iy2    = Y-koordinat 2.
 *
 *      Ut:  Inget.
 *
 *      (C)microform ab 9/11/95 J. Kjellander
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
    MoveToEx(gwinpt->dc,ix1,iy1,NULL);
    LineTo(gwinpt->dc,ix2,iy2);
/*
***Pilspetsen
*/
    cosalf = cos(alfa);
    sinalf = sin(alfa);

    dx = (int)(-25.0*cosalf + 10.0*sinalf);
    dy = (int)(-10.0*cosalf - 25.0*sinalf);

    LineTo(gwinpt->dc,ix2+dx,iy2+dy);

    dx = (int)(-25.0*cosalf - 10.0*sinalf);
    dy = (int)( 10.0*cosalf - 25.0*sinalf);

    MoveToEx(gwinpt->dc,ix2,iy2,NULL);
    LineTo(gwinpt->dc,ix2+dx,iy2+dy);

    return;
  }

/********************************************************/
