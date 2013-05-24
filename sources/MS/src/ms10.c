/**********************************************************************
*
*    ms10.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msrenw();   Dynamic rendering window
*     mswcrw();   Create WPRWIN
*     msrprw();   Repaint WPRWIN
*     msdlrw();   Delete WPRWIN
*     msbtrw();   Button handler
*     msrsrw();   Resize handler
*     msgrwp();   ID to C-ptr
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
#include "../../../sources/WP/include/WP.h"
#include <commdlg.h>
#include <commctrl.h>

static short create_rc(); /* Skapar OpenGL Rendering context */
static bool  select_pf(); /* Sätter upp pixelformat */
static short create_tb(); /* Skapar toolbar */

LRESULT CALLBACK mscbrw(HWND,UINT,WPARAM,LPARAM);

#define RWIN_MARGIN 20

/*!******************************************************/

        short msrenw()

/*      Huvudfunktions för VARKON:s interaktiva
 *      renderings-fönster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 1997-12-21 J. Kjellander
 *
 ******************************************************!*/

 {
   short status;
   v2int id;

   status = (short)mswcrw(10,10,500,500,"OpenGL",&id);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        int    mswcrw(x,y,dx,dy,label,id)
        int    x,y,dx,dy;
        char  *label;
        v2int *id;

/*      Skapar WPRWIN-fönster.
 *
 *      In: x     = Läge i X-led.
 *          y     = Läge i Y-led.   
 *          dx    = Storlek i X-led.
 *          dy    = Storlek i Y-led.
 *          label = Fönstertitel.
 *          id    = Pekare till utdata.
 *
 *      Ut: *id = Giltigt entry i wpwtab.
 *
 *      Felkod: WP1052 = wpwtab full.
 *              WP1062 = Fel från malloc().
 *              WP1612 = Kan ej initiera OpenGL.
 *
 *      (C)microform ab 1998-01-13 J. Kjellander
 *
 ******************************************************!*/

  {
   int        i,px,py,width,height;
   double     width_mm,height_mm;
   double     mdx,mdy,mdz;
   HWND       win32_id;
   RECT       rect;
   WPRWIN    *rwinpt;

/*
***Skapa ett ledigt fönster-ID.
*/
   if ( (*id=mswffi()) < 0 ) return(erpush("WP1052",label));
/*
***Skapa ett WIN32-fönster.
*/
   win32_id = CreateWindow(VARKON_RWIN_CLASS,
                           "",
                           WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS |
                           WS_VISIBLE | WS_CLIPCHILDREN,
	      	  			   x,
		  	      		   y,
		  			       dx,
		  				   dy,
      		  			   ms_main,
		      			   NULL,
			      		   ms_inst,
					       NULL);
/*
***Skapa ett WPRWIN.
*/
    if ( (rwinpt=(WPRWIN *)v3mall(sizeof(WPRWIN),"wpwcrw")) == NULL )
      return(erpush("WP1062",label));

    rwinpt->id.w_id  = *id;
    rwinpt->id.p_id  =  0;
    rwinpt->id.ms_id =  win32_id;
/*
***Storlek.
*/
    msggeo(win32_id,&px,&py,&width,&height,
                    &width_mm,&height_mm);

    rwinpt->geo.x  =  x;
    rwinpt->geo.y  =  y;
    rwinpt->geo.dx =  width;
    rwinpt->geo.dy =  height;

    rwinpt->geo.psiz_x = (double)width_mm  / (double)width;
    rwinpt->geo.psiz_y = (double)height_mm / (double)height;
/*
***Ett privat DC för detta fönster. Detta behöver inte deallokeras.
*/
    rwinpt->dc = GetDC(win32_id);
/*
***Ett DC och en bitmap för "SaveUnder". Enklast är att göra bitmappen
***lika stor som skärmen. Därmed kan fönstret maximeras utan problem.
*/
    rwinpt->bmdc = CreateCompatibleDC(rwinpt->dc);
    rwinpt->bmh  = CreateCompatibleBitmap(rwinpt->dc,
                                          GetSystemMetrics(SM_CXFULLSCREEN),
                                          GetSystemMetrics(SM_CYFULLSCREEN));
    SelectObject(rwinpt->bmdc,rwinpt->bmh);
/*
***Default aktiv vy. Motsvarar vy:n "xy" med skala = 1.0.
***och origo i nedre vänstra hörnet.
*/
    rwinpt->vy.vynamn[0] = '\0';
/*
***Skärmfönstrets storlek och marginaler.
*/
    rwinpt->vy.scrwin.xmin = 0;
    rwinpt->vy.scrwin.ymin = 0;
    rwinpt->vy.scrwin.xmax = dx;
    rwinpt->vy.scrwin.ymax = dy - RWIN_MARGIN;
/*
***Modellfönstret gör vi lika med det grafiska fönstret i skala = 1.0.
*/
    rwinpt->vy.modwin.xmin = 0;
    rwinpt->vy.modwin.ymin = 0;
    rwinpt->vy.modwin.xmax =
              (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin) *
               rwinpt->geo.psiz_x;
    rwinpt->vy.modwin.ymax =
              (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin) *
               rwinpt->geo.psiz_y;

    rwinpt->vy.vy_3D = TRUE;
    rwinpt->vy.vydist = 0.0;

    rwinpt->vy.vymat.k11 = 1.0;
    rwinpt->vy.vymat.k12 = 0.0;
    rwinpt->vy.vymat.k13 = 0.0;
    rwinpt->vy.vymat.k21 = 0.0;
    rwinpt->vy.vymat.k22 = 1.0;
    rwinpt->vy.vymat.k23 = 0.0;
    rwinpt->vy.vymat.k31 = 0.0;
    rwinpt->vy.vymat.k32 = 0.0;
    rwinpt->vy.vymat.k33 = 1.0;
/*
***Denna vy är fullständig och kan användas, valid = TRUE men
***den har inget namn.
*/
    rwinpt->vy.valid = TRUE;
/*
***Default translation och rotation.
*/
    rwinpt->movx = rwinpt->movy = 0.0;
    rwinpt->rotx = rwinpt->roty = rwinpt->rotz = 0.0;
    rwinpt->scale = 1.0;
    rwinpt->light = 50.0;
    rwinpt->pfactor = 0.0;
    rwinpt->leftdown = FALSE;
    rwinpt->zclip    = FALSE;
/*
***Fönstret har ännu inga subfönster.
*/
    for ( i=0; i<WP_RWSMAX; ++i) rwinpt->wintab[i].ptr = NULL;
/*
***Lagra fönstret i fönstertabellen.
*/
    wpwtab[*id].typ = TYP_RWIN;
    wpwtab[*id].ptr = (char *)rwinpt;
/*
***Skapa toolbar.
*/
   create_tb(rwinpt);
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
    FillRect(rwinpt->dc,&rect,GetStockObject(LTGRAY_BRUSH));

    rect.right =  GetSystemMetrics(SM_CXFULLSCREEN);
    rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
    FillRect(rwinpt->bmdc,&rect,GetStockObject(LTGRAY_BRUSH));
/*
***Skapa rendering context.
***OBSOBSOBS Här fattas städning vid fel !!!!!
*/
   if ( create_rc(rwinpt) < 0 )
     {
     return(erpush("WP1612",""));
     }
/*
***Hur stor är modellen ?
*/
    wpmmod(rwinpt);
    mdx = rwinpt->xmax - rwinpt->xmin;
    mdy = rwinpt->ymax - rwinpt->ymin;
    mdz = rwinpt->zmax - rwinpt->zmin;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-mdx/2.0,+mdx/2.0,-mdy/2.0,+mdy/2.0,mdz,mdz+2*mdz);
/*
***Skapa OpenGL displaylist 1.
*/
    wpmodl(rwinpt);
/*
***Visa bilden.
*/
    wpsodl(rwinpt,(GLuint)1);
/*
***Kopiera resultatet (hela fönstret) till "save_under"-bitmappen.
*/
   BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mscbrw(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för WPRWIN-fönster.
 *
 *      In: win32_id = Fönster som det hänt något i.
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1998-01-13 J. Kjellander
 *
 *      1998-10-27, WM_SIZE, J.Kjellander
 *
 ******************************************************!*/

  {
   int        dx,dy;
   wpw_id     win_id;
   WPWIN     *winptr;
   WPRWIN    *rwinpt;

   MSG mess;
   static int nMin,nSek; //tidräknare
   static POINTS ptkoordner,ptkoordupp;
   static bool sizflg = FALSE;   /* Satt under interaktiv resize */

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
   if ( (rwinpt=msgrwp(win32_id)) == NULL )
	  return DefWindowProc(win32_id,message,wp,lp);
/*
***Vissa meddelanden servas dock här.
*/
   switch ( message )
     {

/*
***PAINT sköts av en egen rutin.
*/
     case WM_PAINT:
     msrprw(rwinpt);
     break;
/*
***Resize. Vanliga WM_SIZE kommer det många om man
***ändrar storlek med musen. Isåfall visar vi inte den
***nya bilden förrän vi får WM_EXITSIZEMOVE. Klickar man
***på maximize-knappen får man bara ett WM_SIZE och inget
***WM_ENTERSIZEMOVE/WM_EXITSIZEMOVE.
*/
     case WM_ENTERSIZEMOVE:
     sizflg = TRUE;
     break;

     case WM_SIZE:
     dx = LOWORD(lp);
     dy = HIWORD(lp);
     if ( wp != SIZE_MINIMIZED  &&
           dx > 0  &&  dy > 0 ) 
       {
       msrsrw(rwinpt,dx,dy);  
       if ( sizflg == FALSE )
         { 
         wpsodl(rwinpt,(GLuint)1);
         BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                            rwinpt->dc,0,0,SRCCOPY);
         }
       }
     break;

     case WM_EXITSIZEMOVE:
     wpsodl(rwinpt,(GLuint)1);
     BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
     sizflg = FALSE;
     break;
/*
***Tag hand om vänster musknapp ner.
*/
     case WM_LBUTTONDOWN:
     {
         rwinpt->leftdown = TRUE;
         ptkoordner = MAKEPOINTS(lp);
     }
     break;
/*
***Tag hand om vänster musknapp upp.
*/
     case WM_LBUTTONUP:
     {
        if(rwinpt->leftdown)
           BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY); 
        rwinpt->leftdown = FALSE;
     }
     break;
/*
***Tag hand om mousemove.
*/
     case WM_MOUSEMOVE:
     {
        ptkoordupp = MAKEPOINTS(lp);
        while(PeekMessage(&mess, win32_id, WM_MOUSEMOVE,
            WM_MOUSEMOVE, PM_REMOVE) != 0)
        {
           ptkoordupp = MAKEPOINTS(mess.lParam);
        }
        if(rwinpt->leftdown)
        {
           mscbmm(rwinpt,&ptkoordner, &ptkoordupp);
        }
     }
     break;

/*
***Tag hand om toolbarens knappar.
*/
     case WM_COMMAND:
     {
        msprWC(rwinpt,wp);
     }
     break;
/*
***WM_CLOSE innebär clickning på döda-knappen.
*/
     case WM_CLOSE:
     win_id = rwinpt->id.w_id;
     msdlrw(rwinpt);
     if ( (winptr=mswgwp(win_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       }
     DestroyWindow(win32_id);
     if ( !IsWindowEnabled(ms_main) ) EnableWindow(ms_main,TRUE);
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

        bool    msrprw(rwinpt)
        WPRWIN *rwinpt;

/*      Repaint-rutin för WPRWIN.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *
 *      (C)microform ab 1998-01-13, J. Kjellander
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
   dc = BeginPaint(rwinpt->id.ms_id,&ps); 
/*
***Kopiera från 'save under'- bitmappen.
*/
   x  = ps.rcPaint.left;
   y  = ps.rcPaint.top;
   dx = ps.rcPaint.right - x;
   dy = ps.rcPaint.bottom - y;
   BitBlt(rwinpt->dc,x,y,dx,dy,rwinpt->bmdc,x,y,SRCCOPY);
/*
***Nu gör vi inget mer med det grafiska fönstret.
*/
   EndPaint(rwinpt->id.ms_id,&ps);
/*
***Slut.
*/
   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        WPRWIN *msgrwp(win32_id)
        HWND   win32_id;

/*      Letar upp WPRWIN-fönstret med angiven
 *      MS-id och returnerar en C-pekare till fönstret.
 *
 *      In: win32_id  = Fönstrets MS-id.
 *
 *      Ut: Inget.   
 *
 *      FV: C-adress eller NULL om fönster saknas.
 *
 *      (C)microform ab 1998-01-13, J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPRWIN  *rwinpt;

/*
***Leta upp fönstret.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
        if ( wpwtab[i].typ == TYP_RWIN )
          {
          rwinpt = (WPRWIN *)wpwtab[i].ptr;
          if ( rwinpt->id.ms_id == win32_id ) return(rwinpt);
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

        bool    msbtrw(rwinpt,butmes,serv_id)
        WPRWIN  *rwinpt;
        WPMES32 *butmes;
        wpw_id  *serv_id;

/*      Button-rutin för WPRWIN med vidhängande sub-fönster.
 *      Kollar om muspekning skett i något av WPRWIN-fönstrets
 *      subfönster och servar isåfall eventet.
 *
 *      In: rwinptr = C-pekare till WPRWIN.
 *          butmes  = Button mesage.
 *          serv_id = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat eventet.
 *
 *      Fv: TRUE  = Eventet servat.
 *          FALSE = Detta fönster ej inblandat.
 *
 *      (C)microform ab 1998-01-14 J. Kjellander
 *
 ******************************************************!*/

  {
    short   i,acttyp,actnum;
    bool    hit;
    char   *subptr;
    WPBUTT *butptr;
    WPICON *icoptr;

/*
***Gå igenom snabbvalsknapparna och serva om träff.
*/
    hit = FALSE;

    for ( i=0; i<WP_RWSMAX; ++i )
      {
      subptr = rwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( rwinpt->wintab[i].typ )
          {
          case TYP_BUTTON:
          butptr = (WPBUTT *)subptr;
          if ( butmes->wh == butptr->id.ms_id )
            {
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
          switch ( actnum )
            {
            case 0:
            rwinpt->rotx += 5;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 1:
            rwinpt->rotx -= 5;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 2:
            rwinpt->roty += 5;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 3:
            rwinpt->roty -= 5;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 4:
            rwinpt->movx += 0.05*(rwinpt->xmax - rwinpt->xmin);
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 5:
            rwinpt->movx -= 0.05*(rwinpt->xmax - rwinpt->xmin);
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 6:
            rwinpt->movy += 0.05*(rwinpt->ymax - rwinpt->ymin);
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 7:
            rwinpt->movy -= 0.05*(rwinpt->ymax - rwinpt->ymin);
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 8:
            rwinpt->scale *= 1.1;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;

            case 9:
            rwinpt->scale *= 0.9;
            wpsodl(rwinpt,(GLuint)1);
            BitBlt(rwinpt->bmdc,0,0,rwinpt->geo.dx,rwinpt->geo.dy,
                                          rwinpt->dc,0,0,SRCCOPY);
            break;
/*
***Stäng fönstret.
*/
            case 10:
            mswdel((v2int)rwinpt->id.w_id);
            break;
            }
          return(TRUE);
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool    msrsrw(rwinpt,newdx,newdy)
        WPRWIN *rwinpt;
        int newdx,newdy;

/*      Resize-rutin för WPRWIN.
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *          newdx  = Ny storlek i X-led.
 *          newdy  = Ny storlek i Y-led.
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE.
 *
 *      (C)microform ab 1998-10-27 J. Kjellander
 *
 ******************************************************!*/

  {
    double mdx,mdy,gadx,gady,mprop,gprop;
/*
***Ny fönsterstorlek.
*/
    rwinpt->geo.dx = newdx;
    rwinpt->geo.dy = newdy;
/*
***Ny vy.
*/
    rwinpt->vy.scrwin.xmin = 0;
    rwinpt->vy.scrwin.ymin = 0;
    rwinpt->vy.scrwin.xmax = newdx;
    rwinpt->vy.scrwin.ymax = newdy - RWIN_MARGIN;
/*
***Normalisera. Se wpnrrw(). Här lägger vi inte
***på nåt extra dock.
*/
    gadx = rwinpt->geo.psiz_x *
         (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin);
    gady = rwinpt->geo.psiz_y *
         (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin);

   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;

   gprop = gady/gadx;
   mprop = mdy/mdx;

   if ( mprop > gprop )
     {
     rwinpt->xmin -= (mdy/gprop - mdx)/2.0;
     rwinpt->xmax += (mdy/gprop - mdx)/2.0;
     }
   else if ( mprop < gprop )
     {
     rwinpt->ymin -= (gprop*mdx - mdy)/2.0;
     rwinpt->ymax += (gprop*mdx - mdy)/2.0;
     }
/*
***Viewport.
*/
    glScissor((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
    glEnable(GL_SCISSOR_TEST);

    glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
               (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
               (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
/*
***Ny projektion.
*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(rwinpt->xmin,rwinpt->xmax,
            rwinpt->ymin,rwinpt->ymax,
           -rwinpt->zmax,-rwinpt->zmin);
/*
***Sudda hela fönstret så att det inte blir skräp till höger om knapparna.
*/
    glDisable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
/*
***Slut.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        int   msdlrw(rwinpt)
        WPRWIN *rwinpt;

/*      Dödar ett WPRWIN.
 *
 *      In: rwinptr = C-pekare till WPRWIN.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1998-01-13 J. Kjellander
 *
 ******************************************************!*/

  {
    int   i;
    char *subptr;

/*
***Stäng OpenGL.
*/  
   wglMakeCurrent(rwinpt->dc,NULL); 
   wglDeleteContext(rwinpt->rc);
   ReleaseDC(rwinpt->id.ms_id,rwinpt->dc);
/*
***Döda eventuella subfönster.
*/
    for ( i=0; i<WP_RWSMAX; ++i )
      {
      subptr = rwinpt->wintab[i].ptr;
      if ( subptr != NULL )
        {
        switch ( rwinpt->wintab[i].typ )
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
    DeleteObject(rwinpt->bmh);
    DeleteDC(rwinpt->bmdc);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
    v3free((char *)rwinpt,"msdlrw");

    return(0);
  }

/********************************************************/
/*!******************************************************/

static short create_rc(rwinpt)
       WPRWIN *rwinpt;

/*      Skapa OpenGL Rendering Context.
 *
 *      In: rwinpt = Pekare till renderingsfönster.
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1633 = Ingen "visual"
 *                WP1643 = Ingen context

 *
 *      (C)microform ab 1998-01-04 J. Kjellander
 *
 ******************************************************!*/

  {
   int   ival;
   char  buf[V3STRLEN];
   GLfloat ambient[4], diffuse[4], specular[4];
   HGLRC rc;

/*
***Initiera OpenGL. Skapa rendering context.
*/
   if ( !select_pf(rwinpt) ) return(erpush("WP1633",""));

   if ( (rc=wglCreateContext(rwinpt->dc)) == NULL )
     return(erpush("WP1643",""));
/*
***Aktivera Rendering Contextet.
*/
   wglMakeCurrent(rwinpt->dc,rc);
   rwinpt->rc = rc;
/*
***Aktivera djuptest.
*/
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glClearDepth((GLclampd)1.0);
/*
***Viewport. Den del av det grafiska fönstret som får användas.
***Eventuella marginaler för knappar skall ej skrivas över.
*/
   glScissor((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
             (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
             (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
   glEnable(GL_SCISSOR_TEST);

   glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
/*
***Låt positiv normal definieras av moturs riktning precis
***som i Varkon.
*/
   glFrontFace(GL_CCW);
/*
***Slå på ljus.
*/
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

/*
***Default ljusstyrka.
***Först Ambient.
*/
   if ( msgrst("shade.ambient.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) ambient[0] = ival/100.0;
   else ambient[0] = 0.4;

   if ( msgrst("shade.ambient.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) ambient[1] = ival/100.0;
   else ambient[1] = 0.4;

   if ( msgrst("shade.ambient.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) ambient[2] = ival/100.0;
   else ambient[2] = 0.4;

   ambient[3] = 1.0;

   rwinpt->ambient[0] = ambient[0];
   rwinpt->ambient[1] = ambient[1];
   rwinpt->ambient[2] = ambient[2];

   ambient[0] *= rwinpt->light/100.0;
   ambient[1] *= rwinpt->light/100.0;
   ambient[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);

/* Diffuse*/

   if ( msgrst("shade.diffuse.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) diffuse[0] = ival/100.0;
   else diffuse[0] = 1.0;

   if ( msgrst("shade.diffuse.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) diffuse[1] = ival/100.0;
   else diffuse[1] = 1.0;

   if ( msgrst("shade.diffuse.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) diffuse[2] = ival/100.0;
   else diffuse[2] = 1.0;

   diffuse[3] = 1.0;

   rwinpt->diffuse[0] = diffuse[0];
   rwinpt->diffuse[1] = diffuse[1];
   rwinpt->diffuse[2] = diffuse[2];

   diffuse[0] *= rwinpt->light/100.0;
   diffuse[1] *= rwinpt->light/100.0;
   diffuse[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);

/* Specular */

   if ( msgrst("shade.specular.red",buf)  &&
        sscanf(buf,"%d",&ival) == 1             &&
        ival >=0  &&  ival <= 100 ) specular[0] = ival/100.0;
   else specular[0] = 0.8;

   if ( msgrst("shade.specular.green",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) specular[1] = ival/100.0;
   else specular[1] = 0.8;

   if ( msgrst("shade.specular.blue",buf)  &&
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) specular[2] = ival/100.0;
   else specular[2] = 0.8;

   specular[3] = 1.0;

   rwinpt->specular[0] = specular[0];
   rwinpt->specular[1] = specular[1];
   rwinpt->specular[2] = specular[2];

   specular[0] *= rwinpt->light/100.0;
   specular[1] *= rwinpt->light/100.0;
   specular[2] *= rwinpt->light/100.0;
   glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
/*
***Sudda färgbuffert och djupbuffert.
*/ 
   glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short create_tb(rwinpt)
        WPRWIN *rwinpt;

/*      Skapar toolbar
 *
 *      In: rwinpt = C-pekare till WPRWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 1998-12-02 M.Lindberg,R.Bäckgren
 *
 ******************************************************!*/

  {
     static TBBUTTON tbb[8];
     InitCommonControls();
     //tbab.hInst = ms_inst;
     //tbab.nID = IDR_TOOLBAR1; 
        
     tbb[0].idCommand = ID_EXIT;
     tbb[0].fsState = TBSTATE_ENABLED;    
     tbb[0].fsStyle = TBSTYLE_BUTTON;     
     tbb[0].iBitmap = 0;
    
     tbb[1].idCommand = ID_VIEW;
     tbb[1].fsState = TBSTATE_ENABLED;
     tbb[1].fsStyle = TBSTYLE_BUTTON;
     tbb[1].iBitmap = 1;  
    
     tbb[2].idCommand = ID_LIGHT;
     tbb[2].fsState = TBSTATE_ENABLED;
     tbb[2].fsStyle = TBSTYLE_BUTTON;
     tbb[2].iBitmap = 2;
    
     tbb[3].idCommand = ID_CLIP;
     tbb[3].fsState = TBSTATE_ENABLED;
     tbb[3].fsStyle = TBSTYLE_BUTTON;
     tbb[3].iBitmap = 3; 
    
     tbb[4].idCommand = ID_ZOOM;
     tbb[4].fsState = TBSTATE_ENABLED;
     tbb[4].fsStyle = TBSTYLE_CHECKGROUP;
     tbb[4].iBitmap = 4; 
    
     tbb[5].idCommand = ID_PAN;
     tbb[5].fsState = TBSTATE_ENABLED;
     tbb[5].fsStyle = TBSTYLE_CHECKGROUP;
     tbb[5].iBitmap = 5;
    
     tbb[6].idCommand = ID_ROT;
     tbb[6].fsState = TBSTATE_ENABLED|TBSTATE_CHECKED;
     tbb[6].fsStyle = TBSTYLE_CHECKGROUP;
     tbb[6].iBitmap = 6;  
    
     tbb[7].idCommand = ID_HELPP;
     tbb[7].fsState = TBSTATE_ENABLED;
     tbb[7].fsStyle = TBSTYLE_BUTTON;
     tbb[7].iBitmap = 7;
        
     rwinpt->tb_id = CreateToolbarEx(rwinpt->id.ms_id,WS_CHILD|WS_BORDER,
                              IDR_TOOLBAR1,8,ms_inst,
                              IDR_TOOLBAR1,tbb,
                              8,43,15,43,15,
                              sizeof(TBBUTTON));
   
    ShowWindow(rwinpt->tb_id,SW_SHOW);     
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static bool select_pf(rwinpt)
        WPRWIN *rwinpt;

/*      Sätter upp pixelformat.
 *
 *      In: rwinpt = C-pekare till WPRWIN
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1998-01-13 J. Kjellander
 *
 ******************************************************!*/

  {
    int pixelformat;

/*
***Önskade egenskaper.
*/
    static PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),		// size of this pfd
	1,									// version number
	PFD_DRAW_TO_WINDOW |				// support window
	PFD_SUPPORT_OPENGL | 				// support OpenGL
    PFD_DOUBLEBUFFER,                   // Dubbelbuffrat
	PFD_TYPE_RGBA,                      // Färgsystem
	24,									// 24-bit color depth
	0, 0, 0, 0, 0, 0,					// color bits ignored
	0,									// no alpha buffer
	0,									// shift bit ignored
	0,									// no accumulation buffer
	0, 0, 0, 0, 						// accum bits ignored
    32,									// 32-bit z-buffer	
	0,									// no stencil buffer
	0,									// no auxiliary buffer
	PFD_MAIN_PLANE,						// main layer
	0,									// reserved
	0, 0, 0								// layer masks ignored
    };
/*
***Vad är närmast möjliga?
*/
    if ( (pixelformat=ChoosePixelFormat(rwinpt->dc,&pfd)) == 0 )
      {
      MessageBox(NULL,"ChoosePixelFormat failed","Error",MB_OK);
      return(FALSE);
      }
/*
***Aktivera det.
*/
    if (SetPixelFormat(rwinpt->dc,pixelformat,&pfd) == FALSE)
      {
      MessageBox(NULL,"SetPixelFormat failed","Error",MB_OK);
      return(FALSE);
      }
/*
***Antar att dubbelbuffring gick bra.
*/
    rwinpt->double_buffer = TRUE;
/*
***Hur gick det.
*/
    pixelformat = GetPixelFormat(rwinpt->dc);
    DescribePixelFormat(rwinpt->dc,pixelformat,sizeof(PIXELFORMATDESCRIPTOR),&pfd);

    if ( pfd.dwFlags & PFD_NEED_PALETTE )
      {
      return(TRUE);
      }
 


    return(TRUE);
  }

/********************************************************/
