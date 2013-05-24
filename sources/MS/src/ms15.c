/**********************************************************************
*
*    ms15.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     mswcgw();   Create WPGWIN
*     msnrgw();   Normalise WPGWIN
*     msrpgw();   Repaint WPGWIN
*     msrepa();   Repaint WPGWIN from DB
*     msbtgw();   Button handler
*     msrsgw();   Resize handler
*     msergw();   Erase one or more WPGWIN
*     msdlgw();   Delete WPGWIN
*     msggwp();   Id to C-ptr
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
#include "../../../sources/EX/include/EX.h"
#include "../../../sources/WP/include/WP.h"

static int creqbt(); /* Skapar snabbvalsknappar */

LRESULT CALLBACK mscbgw(HWND,UINT,WPARAM,LPARAM);

extern short     actfun,v3mode;
extern DBptr     lsysla;
extern double    rstrox,rstroy,rstrdx,rstrdy;
extern bool      rstron;
extern HCURSOR   ms_grcu;

/*!******************************************************/

        int  mswcgw(x,y,dx,dy,label,main,id)
        int     x,y,dx,dy;
        char   *label;
        bool    main;
        v2int  *id;

/*      Skapar WPGWIN-fönster. V3:s grafiska huvudfönster
 *      får ock skall ovillkorligen ha ID = 0 = GWIN_MAIN.
 *
 *      In: x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = Fönstertitel.
 *          main  = TRUE  = Huvudfönstret
 *                  FALSE = Ytterligare fönster
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel från malloc().
 *
 *      (C)microform ab 25/10/95 J. Kjellander
 *
 *      1997-04-10 Saveunder, J.Kjellander
 *      1999-12-15 Clip, J.Kjellander
 *      2000-03-17 Clip region, J.Kjellander
 *
 ******************************************************!*/

  {
   char    buf[20],*wclass;
   int     i,status,px,py,width,height,margin;
   double  width_mm,height_mm;
   WPGWIN *gwinpt;
   HWND    win32_id;
   RECT    rect;


/*
***Skapa ett ledigt fönster-ID. Om det är huvudfönstret
***som skall skapas tvingar vi ID till GWIN_MAIN = 0.
*/
   if ( main ) *id = GWIN_MAIN;
   else if ( (*id=mswffi()) < 0 ) return(erpush("WP1052",label));
/*
***Skapa ett WIN32-fönster. WS_CLIPCHILDREN behövs inte eftersom vi
***aldrig rör ev. children, dvs. tryckknappar.
*/
   if ( main ) wclass = VARKON_MGWIN_CLASS;
   else        wclass = VARKON_GWIN_CLASS;

   win32_id = CreateWindow(
                           wclass,
                           "",
   		     	   		   WS_CHILD | WS_THICKFRAME | WS_CAPTION |
         	   		       WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
   		     	   		   WS_CLIPSIBLINGS | WS_SYSMENU,
	      	  			   x,
		  	      		   y,
		  			       dx,
		  				   dy,
      		  			   ms_main,
		      			   NULL,
			      		   ms_inst,
					       NULL);
/*
***Skapa ett WPGWIN -fönster.
*/
    if ( (gwinpt=(WPGWIN *)v3mall(sizeof(WPGWIN),"mswcgw")) == NULL )
      return(erpush("WP1062",label));

    gwinpt->id.w_id  = *id;
    gwinpt->id.p_id  =  NULL;
    gwinpt->id.ms_id =  win32_id;
/*
***Hela fönstret är dx gånger dy men grafiska arean (applikationsarean)
***är mindre pga. titelbalk och fönsterramar.
*/
    msggeo(win32_id,&px,&py,&width,&height,&width_mm,&height_mm);

    gwinpt->geo.x  =  x;
    gwinpt->geo.y  =  y;
    gwinpt->geo.dx =  width;
    gwinpt->geo.dy =  height;

    gwinpt->geo.psiz_x = (double)width_mm  / (double)width;
    gwinpt->geo.psiz_y = (double)height_mm / (double)height;
/*
***Ett privat DC för detta fönster. Detta behöver inte deallokeras.
*/
    gwinpt->dc = GetDC(win32_id);
/*
***Ett DC och en bitmap för "SaveUnder". Enklast är att göra bitmappen
***lika stor som skärmen. Därmed kan fönstret maximeras utan problem.
*/
    gwinpt->bmdc = CreateCompatibleDC(gwinpt->dc);
    gwinpt->bmh  = CreateCompatibleBitmap(gwinpt->dc,
                                          GetSystemMetrics(SM_CXFULLSCREEN),
                                          GetSystemMetrics(SM_CYFULLSCREEN));
    SelectObject(gwinpt->bmdc,gwinpt->bmh);
/*
***Aktuell linjebredd.
*/
    gwinpt->linwdt = 0.0;
/*
***Default aktiv vy. Motsvarar vy:n "xy" med skala = 1.0.
***och origo i nedre vänstra hörnet.
*/
    gwinpt->vy.vynamn[0] = '\0';
/*
***Hur stort skall skärmfönstret vara ? Skall det ha marginaler
***för tryckknappar ?
*/
    gwinpt->vy.scrwin.xmin = 0;
    gwinpt->vy.scrwin.ymin = 0;
    gwinpt->vy.scrwin.xmax = width;
    gwinpt->vy.scrwin.ymax = height;
    gwinpt->margin_up      = 0;
    gwinpt->margin_down    = 0;
    gwinpt->margin_left    = 0;
    gwinpt->margin_right   = 0;
    gwinpt->clprgn         = NULL;

    if ( msgrst("MARGIN_UP",buf)  &&  sscanf(buf,"%d",&margin) == 1 )
      {
      gwinpt->margin_up = margin;
      gwinpt->vy.scrwin.ymax -= margin;
      }
    if ( msgrst("MARGIN_DOWN",buf)  &&  sscanf(buf,"%d",&margin) == 1 )
      {
      gwinpt->margin_down = margin;
      gwinpt->vy.scrwin.ymin += margin;
      }
    if ( msgrst("MARGIN_LEFT",buf)  &&  sscanf(buf,"%d",&margin) == 1 )
      {
      gwinpt->margin_left = margin;
      gwinpt->vy.scrwin.xmin += margin;
      }
    if ( msgrst("MARGIN_RIGHT",buf)  &&  sscanf(buf,"%d",&margin) == 1 )
      {
      gwinpt->margin_right = margin;
      gwinpt->vy.scrwin.xmax -= margin;
      }

    gwinpt->clprgn = CreateRectRgn(gwinpt->margin_left,
                                   gwinpt->margin_up,
                                   width - gwinpt->margin_right,
                                   height - gwinpt->margin_down);
/*
***Modellfönstret gör vi lika med det grafiska fönstret i skala = 1.0.
*/
    gwinpt->vy.modwin.xmin = 0;
    gwinpt->vy.modwin.ymin = 0;
    gwinpt->vy.modwin.xmax =
              (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
               gwinpt->geo.psiz_x;
    gwinpt->vy.modwin.ymax =
              (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) *
               gwinpt->geo.psiz_y;

    gwinpt->vy.vy_3D = FALSE;
    gwinpt->vy.vydist = 0.0;

    gwinpt->vy.vymat.k11 = 1.0;
    gwinpt->vy.vymat.k12 = 0.0;
    gwinpt->vy.vymat.k13 = 0.0;
    gwinpt->vy.vymat.k21 = 0.0;
    gwinpt->vy.vymat.k22 = 1.0;
    gwinpt->vy.vymat.k23 = 0.0;
    gwinpt->vy.vymat.k31 = 0.0;
    gwinpt->vy.vymat.k32 = 0.0;
    gwinpt->vy.vymat.k33 = 1.0;
/*
***Denna vy är fullständig och kan användas, valid = TRUE men
***den har inget namn. Föregående vy finns ännu ej.
*/
    gwinpt->vy.valid     = TRUE;
    gwinpt->old_vy.valid = FALSE;
/*
***Displayfil.
*/
    gwinpt->df_adr =  NULL;
    gwinpt->df_all =  0;
    gwinpt->df_ptr = -1;
    gwinpt->df_cur = -1;
/*
***Tänd alla nivåer.
*/
    for ( i=0; i<WP_NTSIZ; ++i ) gwinpt->nivtab[i] = 255;
/*
***Fönstret har ännu inga subfönster.
*/
    for ( i=0; i<WP_GWSMAX; ++i) gwinpt->wintab[i].ptr = NULL;
/*
***Eller pekmärken.
*/
    gwinpt->pmkant = 0;
/*
***Lagra fönstret i fönstertabellen.
*/
    wpwtab[*id].typ = TYP_GWIN;
    wpwtab[*id].ptr = (char *)gwinpt;
/*
***Visa resultatet.
*/
    mswshw(*id);
/*
***Gråa hela fönstret inkl. ev. marginaler en gång så att det inte blir skräp där.
***På så vis blir marginalyta som inte utnyttjas för knappar grå istället för obestämd.
***Detta måste göras först här efter show annars funkar det inte.
*/
    rect.top    = 0;
    rect.left   = 0;
    rect.right  = width;
    rect.bottom = height;
    FillRect(gwinpt->dc,&rect,GetStockObject(LTGRAY_BRUSH));

    rect.right =  GetSystemMetrics(SM_CXFULLSCREEN);
    rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
    FillRect(gwinpt->bmdc,&rect,GetStockObject(LTGRAY_BRUSH));

    if ( gwinpt->clprgn != NULL )
      {
      SelectClipRgn(gwinpt->dc,gwinpt->clprgn);
      SelectClipRgn(gwinpt->bmdc,gwinpt->clprgn); 
      }
/*
***Skapa fönstrets tryckknappar.
*/
    if ( (status=creqbt(gwinpt)) < 0 ) return(status);    
/*
***Normalisera, dvs. fixa till modellfönstrets proportioner
***och beräkna 2D transformationskonstanter.
*/
    msnrgw(gwinpt);
/*
***Om det är huvudfönstret det gäller meddelar vi även
***grapac.
*/
    if ( main ) wpupgp(gwinpt);


    return(0);
  }

/********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mscbgw(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för WPGWIN-fönster.
 *
 *      In: win32_id = Fönster som det hänt något i.
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 19/11/95 J. Kjellander
 *
 *      1996-12-15 Fixat WM_WINDOWPOSCHANGING, J.Kjellander
 *      1997-12-28 VC++ 5.0, J.Kjellander
 *      1997-12-28 Bug, WM_CLOSE, J.Kjellander
 *
 ******************************************************!*/

  {
   int        x,y,dx,dy,ymax,underk;
   bool       illegal;
   wpw_id     win_id;
   WPWIN     *winptr;
   WPGWIN    *gwinpt;
   WINDOWPOS *wpos;

/*
***Hårkors-rutinerna använder ms_lstmes.
*/
   ms_lstmes.wh  = win32_id;
   ms_lstmes.msg = message;
   ms_lstmes.wp  = wp;
   ms_lstmes.lp  = lp;
/*
***Fixa fram en C-pekare till fönstret i fråga.
***Om inte det går är det nåt allvarligt fel så då processar
***vi med default-rutinen.
*/
   if ( (gwinpt=msggwp(win32_id)) == NULL )
	  return DefWindowProc(win32_id,message,wp,lp);
/*
***Vissa meddelanden servas dock här.
*/
   switch ( message )
     {
/*
***Om någon vill ändra placering eller storlek får vi först
***ett meddelande om detta så att vi kan kontrollera om ändringen
***är tillåten.
*/
     case WM_WINDOWPOSCHANGING:
     wpos = (WINDOWPOS *)lp;
     illegal = FALSE;
/*
***Det bör inte vara tillåtet att flytta fönstret uppåt så att det hamnar
***delvis under huvudfönstrets menybalk eller täcker skuggan på insidan (2).
*/
     if ( wpos->y < 2 )
       {
       wpos->y = 2;
       illegal = TRUE;
       }
/*
***Det är heller inte tillåtet att flytta fönstret nedåt så
***att det överlappar huvudfönstrets promtrad. Flyttning nedåt
***kan bero på att hela fönstret har flyttats eller bara underkanten
***eller kanske båda samtidigt som vid maximering. Vid bara flyttning
***är SWP_NOSIZE satt av systemet så det måste vi ändra på för att
***kunna påverka fönstrets storlek.
*/
     msggeo(ms_main,&x,&y,&dx,&dy,NULL,NULL);
     ymax = dy - (1 + 15 + 1 + 2 + 2);

     underk = wpos->y + wpos->cy;

     if ( underk > ymax )
       {
       wpos->cy = ymax - wpos->y;
       if ( wpos->cy <= (GetSystemMetrics(SM_CYCAPTION) + 2*GetSystemMetrics(SM_CYBORDER)) )
         {
         wpos->cy = GetSystemMetrics(SM_CYCAPTION);
         wpos->y = ymax - wpos->cy - 2*GetSystemMetrics(SM_CYBORDER);
         }
       wpos->flags &= !SWP_NOSIZE;
       illegal = TRUE;
       }
     if ( illegal ) return(0);
     else return(DefWindowProc(win32_id,message,wp,lp));
/*
***Ett WM_MOVE processas här. De koordinater som returneras är
***av någon anledning inte fönstrets nya position utan clientareans
***nya läge. Därför kompenserar vi med en ram och en titelbalk.
*/
     case WM_MOVE:
     gwinpt->geo.x = LOWORD(lp) - GetSystemMetrics(SM_CXBORDER);
     gwinpt->geo.y = HIWORD(lp) - GetSystemMetrics(SM_CYBORDER) -
                                  GetSystemMetrics(SM_CYCAPTION);
	 break;
/*
***Kommer det ett WM_SIZE till WPGWIN-fönstret har det ändrat
***storlek. VC++ 5.0 krävde att testet om SIZE_MINIMIZED och
***storlek lades till 1997-12-28 JK.
*/
     case WM_SIZE:
     dx = LOWORD(lp);
     dy = HIWORD(lp);
     if ( wp != SIZE_MINIMIZED  &&
           dx > 0  &&  dy > 0 ) msrsgw(gwinpt,dx,dy);
	 break;
/*
***Om musen rör sig i detta fönster är den i detta fönster !
***Då ser vi till att rätt cursor visas.
*/
     case WM_MOUSEMOVE:
     SetCursor(ms_grcu);
     break;
/*
***WM_CLOSE innebär clickning på döda-knappen.
***msdlgw() stryker inte WPLWIN-fönstret ur wpwtab så
***det får vi göra här. Däremot stryks WPGWIN-posten
***så vi får inte använda gwinpt->id.w_id som test.
***Därav variabeln win_id, 1997-12-28 JK.
*/
     case WM_CLOSE:
     win_id = gwinpt->id.w_id;
     msdlgw(gwinpt);
     if ( (winptr=mswgwp(win_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       }
     DestroyWindow(win32_id);
     if ( !IsWindowEnabled(ms_main) ) EnableWindow(ms_main,TRUE);
     break;
/*
***PAINT sköts av en egen rutin.
*/
     case WM_PAINT:
     msrpgw(gwinpt);
     break;
/*
***Meddelanden som vi inte bryr oss om.
*/
	 default:
	 return DefWindowProc(win32_id,message,wp,lp);
	 }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int    msnrgw(gwinpt)
        WPGWIN *gwinpt;

/*      Normaliserar proportionerna för GWIN-fönstrets
 *      modellfönster och beräknar nya 2D-transformations-
 *      konstanter.
 *
 *      In: gwinpt => Pekare till fönster.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   double mdx,mdy,gadx,gady,mprop,gprop;

#ifdef DEBUG
    if ( dbglev(MSPAC) == 4 )
      {
      fprintf(dbgfil(MSPAC),"***Start-msnrgw***\n");
      fprintf(dbgfil(MSPAC),"ID=%d\n",gwinpt->id.w_id);
      fflush(dbgfil(MSPAC));
      }
#endif

/*
***Hur stor är fönstrets grafiska area.
*/
   gadx = gwinpt->geo.psiz_x *
        (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   gady = gwinpt->geo.psiz_y *
        (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Hur stort är modellfönstret i millimeter.
*/
   mdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
   mdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin;
/*
***Förhållandet mellan grafiska areans höjd och bredd är gady/gadx.
***Se till att modellfönstret får samma förhållande så att cirklar
***blir "runda" tex.
*/
   gprop = gady/gadx;
   mprop = mdy/mdx;

   if ( mprop > gprop )
     {
     gwinpt->vy.modwin.xmin -= (mdy/gprop - mdx)/2.0;
     gwinpt->vy.modwin.xmax += (mdy/gprop - mdx)/2.0;
     }
   else if ( mprop < gprop )
     {
     gwinpt->vy.modwin.ymin -= (gprop*mdx - mdy)/2.0;
     gwinpt->vy.modwin.ymax += (gprop*mdx - mdy)/2.0;
     }
/*
***Beräkna nya transformationskonstanter.
*/
   gwinpt->vy.k2x = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) /
                    (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
   gwinpt->vy.k2y = (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) /
                    (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);

   gwinpt->vy.k1x = gwinpt->vy.scrwin.xmin -
                    gwinpt->vy.modwin.xmin * gwinpt->vy.k2x;
   gwinpt->vy.k1y = gwinpt->vy.scrwin.ymin -
                    gwinpt->vy.modwin.ymin * gwinpt->vy.k2y;


#ifdef DEBUG
    if ( dbglev(MSPAC) == 4 )
      {
      fprintf(dbgfil(MSPAC),"***Slut-msnrgw***\n");
      fflush(dbgfil(MSPAC));
      }
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    msrpgw(gwinpt)
        WPGWIN *gwinpt;

/*      Repaint-rutin för WPGWIN.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 3/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int         x,y,dx,dy;
   HDC         dc;
   PAINTSTRUCT ps;

/*
***Eftersom grafiska fönster har privata DC:n måste
***WM_PAINT meddelanden processas med BeginPaint() och
***EndPaint() för att inte en oändlig loop skall uppstå.
*/
   dc = BeginPaint(gwinpt->id.ms_id,&ps); 
/*
***Kopiera från 'save under'- bitmappen. Tillåt
***update även av marginaler.
*/
   x  = ps.rcPaint.left;
   y  = ps.rcPaint.top;
   dx = ps.rcPaint.right - x;
   dy = ps.rcPaint.bottom - y;
/*
   BitBlt(gwinpt->dc,x,y,dx,dy,gwinpt->bmdc,x,y,SRCCOPY);
*/
   BitBlt(dc,x,y,dx,dy,gwinpt->bmdc,x,y,SRCCOPY);
/*
***Nu gör vi inget mer med det grafiska fönstret.
*/
   EndPaint(gwinpt->id.ms_id,&ps);

/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        int   msrepa(win_id)
        v2int win_id;

/*      Ritar om ett eller alla grafiska fönster med tillhörande
 *      pixmap för save_under.
 *
 *      In: win_id = ID för ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = Fönstret %s finns ej.
 *
 *      (C)microform ab 1996-12-03 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sudda.
*/
    msergw(win_id);
/*
***Rita om GM.
*/
    EXdral(win_id);
/*
***Och om det är huvudfönstret även eventuellt raster.
*/
    if ( ( win_id == GWIN_ALL  ||  win_id == GWIN_MAIN )  &&  rstron )
      gpdrrs(rstrox,rstroy,rstrdx,rstrdy);
/*
***Aktivt koordinatsystem.
*/
    igupcs(lsysla,V3_CS_ACTIVE);

/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool    msbtgw(gwinpt,butmes,serv_id)
        WPGWIN  *gwinpt;
        WPMES32 *butmes;
        wpw_id  *serv_id;

/*      Button-rutin för WPGWIN med vidhängande sub-fönster.
 *      Kollar om muspekning skett i något av WPGWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: iwinptr = C-pekare till WPGWIN.
 *          butmes  = Button mesage.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 7/11/95 J. Kjellander
 *
 *       1998-03-19 Ny mszoom(), J.Kjellander
 *
 ******************************************************!*/

  {
    int     x,y;
    short   i,acttyp,actnum,oldfun;
    bool    hit;
    char   *subptr;
    WPBUTT *butptr;
    WPICON *icoptr;

/*
***Gå igenom snabbvalsknapparna och serva om träff.
*/
    hit = FALSE;

    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          butptr = (WPBUTT *)subptr;
          if ( butmes->wh == butptr->id.ms_id )
            {
            x = gwinpt->geo.x + butptr->geo.x;
            y = gwinpt->geo.y + butptr->geo.y;
            hit     = TRUE;
            acttyp  = butptr->acttyp;
            actnum  = butptr->actnum;
           *serv_id = butptr->id.w_id;
            }
          break;

          case TYP_ICON:
          icoptr = (WPICON *)subptr;
          if ( butmes->wh == icoptr->id.ms_id )
            {
            x = gwinpt->geo.x + icoptr->geo.x;
            y = gwinpt->geo.y + icoptr->geo.y;
            hit     = TRUE;
            acttyp  = icoptr->acttyp;
            actnum  = icoptr->actnum;
           *serv_id = icoptr->id.w_id;
            }
          break;
          }
/*
***Träff kanske ?
*/
        if ( hit )
          {
/*
***Om så tar vi särskilt hand om vissa snabbval här.
*/
          if ( acttyp == FUNC )
            {
            oldfun = actfun;
            actfun = actnum;

            switch ( actnum )
              {
              case 188:
              msscle(gwinpt,x,y);
              break;

              case 189:
              mscent(gwinpt);
              break;

              case 190:
              if ( v3mode == BAS3_MOD ) msperp(gwinpt,x,y);
              else Beep(1000,1000);
              break;

              case 191:
              mslstv(gwinpt);
              break;

              case 193:
              mszoom();
              break;

              case 194:
              msiaut(gwinpt);
              break;

              case 195:
              mschvi(gwinpt,x,y);
              break;

              case 197:
              msnivs(gwinpt,x,y);
              break;

              default:
              actfun = oldfun;
              igdofu(acttyp,actnum); 
              break;
              }
            actfun = oldfun;
            return(TRUE);
            }
          else
            {
            igdofu(acttyp,actnum); 
            return(TRUE);
            }
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool    msrsgw(gwinpt,newdx,newdy)
        WPGWIN *gwinpt;
        int newdx,newdy;

/*      Resize-rutin för WPGWIN.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *          newdx  = Ny storlek i X-led.
 *          newdy  = Ny storlek i Y-led.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE.
 *
 *      (C)microform ab 19/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int    oldx,oldy,olddx,olddy,newx,newy,
           dx,dy,ddx,ddy;
    double oldmdx,oldmdy,sizx,sizy;
    bool   right,left,up,down;


/*
***Fönstrets gamla placering och storlek.
*/
    oldx  = gwinpt->geo.x;  oldy  = gwinpt->geo.y;
    olddx = gwinpt->geo.dx; olddy = gwinpt->geo.dy;
/*
***Var befinner sig fönstret nu ?
*/ 
    newx = oldx;
    newy = oldy;
/*
***Hur mycket har det ändrats ?
*/
    dx  = newx -  oldx;  dy  = newy -  oldy;
    ddx = newdx - olddx; ddy = newdy - olddy;
/*
***Fönstret kan: 1 - Ha flyttats.
***              2 - Ha ändrat storlek utan att flyttas
***              3 - Båda om man tar övre och/eller vänstra kanten.
***Vilket är det som gäller ?
*/
    right = left = up = down = FALSE;

    if ( ddx != 0 )
      {
      if ( dx == 0 ) right = TRUE;
      else           left  = TRUE;
      }

    if ( ddy != 0 )
      {
      if ( dy == 0 ) down = TRUE;
      else           up   = TRUE;
      }
/*
***Lagra den nya fönstergeometrin i WPGWIN-posten.
*/
    gwinpt->geo.x  = newx;
    gwinpt->geo.y  = newy;
    gwinpt->geo.dx = newdx;
    gwinpt->geo.dy = newdy;
/*
***Ny clipregion.
*/
    gwinpt->clprgn = CreateRectRgn(gwinpt->margin_left,
                                   gwinpt->margin_up,
                                   newdx - gwinpt->margin_right,
                                   newdy - gwinpt->margin_down);
    SelectClipRgn(gwinpt->dc,gwinpt->clprgn);
    SelectClipRgn(gwinpt->bmdc,gwinpt->clprgn);
/*
***Vad är det som har hänt ?
***Om fönstret har ändrat storlek beräknar vi nytt modell-
***fönster, viewport mm. så att bilden efter automatisk repaint
***ligger kvar på samma ställe som förut även om fönstrets origo
***pga. ändringen har flyttats.
*/
    if ( ( ddx != 0.0 ) || ( ddy != 0.0 ) )
      {
/*
***Under all omständigheter skall grafiska arean ändra storlek.
***Detta gör vi genom att flytta origo till det nya nedre vänstra
***hörnet och justera xmax och ymax därefter oavsett om det är
***högra eller vänstra alt. övre eller nedre kanten som har ändrats.
*/
      gwinpt->vy.scrwin.xmax += ddx;
      gwinpt->vy.scrwin.ymax += ddy;
/*
***Hur blir det med modellfönstret ? Här justerar vi den kant som
***verkligen har ändrats så att bilden ligger still på skärmen.
*/
      oldmdx = gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin;
      oldmdy = gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin;

      if ( right )
        gwinpt->vy.modwin.xmax += (double)ddx/(double)olddx*oldmdx;

      if ( left )
        gwinpt->vy.modwin.xmin -= (double)ddx/(double)olddx*oldmdx;

      if ( up )
        gwinpt->vy.modwin.ymax += (double)ddy/(double)olddy*oldmdy;

      if ( down )
        gwinpt->vy.modwin.ymin -= (double)ddy/(double)olddy*oldmdy;
/*
***Nya 2D transformationskonstanter.
*/
      gwinpt->vy.k2x =
        (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) /
        (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
      gwinpt->vy.k2y =
        (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) /
        (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin);
      gwinpt->vy.k1x =
        gwinpt->vy.scrwin.xmin - gwinpt->vy.modwin.xmin * gwinpt->vy.k2x;
      gwinpt->vy.k1y =
        gwinpt->vy.scrwin.ymin - gwinpt->vy.modwin.ymin * gwinpt->vy.k2y;
/*
***Om det är huvudfönstret det gäller meddelar vi även grapac.
*/
      if ( gwinpt->id.w_id == GWIN_MAIN )
        {
        wpupgp(gwinpt);
        sizx = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
                gwinpt->geo.psiz_x;
        sizy = (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin) *
                gwinpt->geo.psiz_y;
        gpsgsz(sizx,sizy);
        }
/*
***Rita om fönstret.
*/
      msrepa(gwinpt->id.w_id);
/*
***Föregående vy skall nu kunna visas i det nya fönstret, alltså
***måste även dennas modellfönster uppdateras.
*/
      if ( gwinpt->old_vy.valid )
        {
        gwinpt->old_vy.scrwin.xmax += ddx;
        gwinpt->old_vy.scrwin.ymax += ddy;
        oldmdx = gwinpt->old_vy.modwin.xmax - gwinpt->old_vy.modwin.xmin;
        oldmdy = gwinpt->old_vy.modwin.ymax - gwinpt->old_vy.modwin.ymin;

        if ( right )
          gwinpt->old_vy.modwin.xmax += (double)ddx/(double)olddx*oldmdx;

        if ( left )
          gwinpt->old_vy.modwin.xmin -= (double)ddx/(double)olddx*oldmdx;

        if ( up )
          gwinpt->old_vy.modwin.ymax += (double)ddy/(double)olddy*oldmdy;

        if ( down )
          gwinpt->old_vy.modwin.ymin -= (double)ddy/(double)olddy*oldmdy;

        gwinpt->old_vy.k2x =
          (gwinpt->old_vy.scrwin.xmax - gwinpt->old_vy.scrwin.xmin) /
          (gwinpt->old_vy.modwin.xmax - gwinpt->old_vy.modwin.xmin);
        gwinpt->old_vy.k2y =
          (gwinpt->old_vy.scrwin.ymax - gwinpt->old_vy.scrwin.ymin) /
          (gwinpt->old_vy.modwin.ymax - gwinpt->old_vy.modwin.ymin);
        gwinpt->old_vy.k1x = gwinpt->old_vy.scrwin.xmin -
                        gwinpt->old_vy.modwin.xmin * gwinpt->old_vy.k2x;
        gwinpt->old_vy.k1y = gwinpt->old_vy.scrwin.ymin -
                        gwinpt->old_vy.modwin.ymin * gwinpt->old_vy.k2y;
        }
      }
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        int   msergw(win_id)
        v2int win_id;

/*      Suddar ett grafiskt fönster.
 *
 *      In: win_id = ID för ett WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkoder: WP1362 = Fönstret %s finns ej.
 *
 *      (C)microform ab 6/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   bool    hit;
   int     i;
   char    errbuf[81];
   WPGWIN *gwinpt;
   RECT    rect;

/*
***Initiering.
*/
   hit = FALSE;
/*
***Loopa igenom alla WPGWIN och sudda det eller de som
***önskats.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Ett WPGWIN har hittats. Skall det suddas ?
*/
         if ( win_id == gwinpt->id.w_id  ||  win_id == GWIN_ALL )
           {
/*
***Ja, sudda själva fönstret. Här suddar vi bara grafiska arean ej
***eventuella marginaler eftersom tryckkknappar då skulle skrivas över.
***Genom att bara sudda grafiska arean spar vi tid och slipper dessutom
***ha WS_CLIPCHILDREN påslaget på det grafiska fönstret vilket också
***spar tid.
*/
           hit = TRUE;
           rect.left   = 0;
           rect.right  = gwinpt->geo.dx;
           rect.top = 0;
           rect.bottom = gwinpt->geo.dy;


           FillRect(gwinpt->dc,&rect,GetStockObject(WHITE_BRUSH));
           FillRect(gwinpt->bmdc,&rect,GetStockObject(WHITE_BRUSH));
/*
***Om det är huvudfönstret skall grapac:s DF suddas.
*/
           if ( gwinpt->id.w_id == GWIN_MAIN ) gperdf();
/*
***Övriga fönster suddar vi DF:en på här.
*/
           else gwinpt->df_ptr = gwinpt->df_cur = -1;
           }
         }
       }
     }
/*
***Gick det bra ?
*/
   if ( !hit )
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1362",errbuf));
     }
   else return(0);
  }

/********************************************************/
/*!******************************************************/

        int   msdlgw(gwinpt)
        WPGWIN *gwinpt;

/*      Dödar ett WPGWIN.
 *
 *      In: gwinptr = C-pekare till WPGWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Döda eventuella subfönster.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      subptr = gwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( gwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          msdlbu((WPBUTT *)subptr);
          break;

          case TYP_ICON:
          msdlic((WPICON *)subptr);
          break;
          }
        }
      }
/*
***Lämna tillbaks grafiska fönstrets "Save under" - bitmapp.
*/
    DeleteObject(gwinpt->bmh);
    DeleteDC(gwinpt->bmdc);
/*
***Displayfil.
*/
    if ( gwinpt->df_adr != NULL) v3free((char *)gwinpt->df_adr,"msdlgw");
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)gwinpt,"msdlgw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPGWIN *msggwp(win32_id)
        HWND   win32_id;

/*      Letar upp det grafiska fönstret med angiven
 *      MS-id och returnerar en C-pekare till fönstret.
 *
 *      In: win32_id  = Fönstrets MS-id.
 *
 *      Ut: Inget.   
 *
 *      FV: C-adress eller NULL om fönster saknas.
 *
 *      (C)microform ab 1/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPGWIN  *gwinpt;

/*
***Leta upp fönstret.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
        if ( wpwtab[i].typ == TYP_GWIN )
          {
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
          if ( gwinpt->id.ms_id == win32_id ) return(gwinpt);
          }
        }
      }
/*
***Ingen träff.
*/
    return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static int creqbt(gwinpt)
        WPGWIN *gwinpt;

/*      Skapar grafiska fönstrets snabbvalsknappar.
 *
 *      In: gwinpt = C-pekare till WPGWIN.
 *
 *      (C)microform ab 20/12/95 J. Kjellander
 *
 *      1996-05-20 Bug numrering, J. Kjellander
 *
 ******************************************************!*/

  {
    char      butstr[V3STRLEN],actstr[V3STRLEN],resnam[V3STRLEN],
              buf[V3STRLEN],iconam[V3PTHLEN+1];
    int       i,x,y,dx,dy,status,nsub,actnum,dummy;

/*
***Antal subfönster är till att börja med = 0.
*/
    nsub = 0;
/*
***Loopa igenom alla möjliga resursnamn.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
/*
***Prova att hämta de olika resurserna för en FBUTTON.
*/
      sprintf(resnam,"BUTTON_%d.X",i);
      if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&x) == 1 )
        {
        sprintf(resnam,"BUTTON_%d.Y",i);
        if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&y) == 1 )
          {
          sprintf(resnam,"BUTTON_%d.DX",i);
          if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&dx) == 1 )
            {
            sprintf(resnam,"BUTTON_%d.DY",i);
            if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&dy) == 1 )
              {
              sprintf(resnam,"BUTTON_%d.TEXT",i);
              if ( !msgrst(resnam,butstr) ) sprintf(butstr,"%d",i);
              sprintf(resnam,"BUTTON_%d.ACTION",i);
              if ( !msgrst(resnam,actstr) ) strcpy(actstr,"f0");
              if ( sscanf(&actstr[1],"%d",&actnum) != 1 ) actnum = 0;
              actstr[1] = '\0';

              status = mscrfb(gwinpt->id.w_id,x,y,dx,dy,butstr,
                                                    actstr,actnum,&dummy);
              if ( status < 0 ) return(status);
/*
***Räkna upp nsub och kolla om det finns plats för fler.
*/
              if ( ++nsub == WP_GWSMAX ) return(0);
              }
            }
          }
        }
      }
/*
***Samma sak med ikoner.
*/
    for ( i=0; i<WP_GWSMAX; ++i )
      {
      sprintf(resnam,"ICON_%d.X",i);
      if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&x) == 1 )
        {
        sprintf(resnam,"ICON_%d.Y",i);
        if ( msgrst(resnam,buf)  &&  sscanf(buf,"%d",&y) == 1 )
          {
          sprintf(resnam,"ICON_%d.NAME",i);
          if ( msgrst(resnam,iconam) )
            {
            sprintf(resnam,"ICON_%d.ACTION",i);
            if ( !msgrst(resnam,actstr) ) strcpy(actstr,"f0");
            if ( sscanf(&actstr[1],"%d",&actnum) != 1 )actnum = 0;
            actstr[1] = '\0';

            status = mscrfi(gwinpt->id.w_id,x,y,iconam,actstr,actnum,&dummy);
            if ( status < 0 ) return(status);
  
            if ( ++nsub == WP_GWSMAX ) return(0);
            }
          }
        }
      }

   return(0);
  }

/********************************************************/
