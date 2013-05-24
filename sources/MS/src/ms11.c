/**********************************************************************
*
*    ms11.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     mswini();   Inits the windowhandler
*     mswexi();   Exits the windowhandler
*     msregc();   Registers window classes
*     mswrep();   Repaint window
*     mswres();   Resize window
*     mswbut();   Button handler
*     msrpmw();   Repaint main window
*
*     mswshw();   SHOW_WIN in MBS
*     mswwtw();   WAIT_WIN in MBS
*     mswdel();   Delete main window and DEL_WIN in MBS
*     mswdls();   Delete sub window and DEL_WIN in MBS
*
*     mswffi();   Find lowest free window index
*     mswfpw();   Id of parent window
*     mswgwp();   Id to C-ptr
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
*    (C) 2000-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"

LRESULT CALLBACK mscbmw(HWND,UINT,WPARAM,LPARAM);  /* Callback för VARKON_MAIN_CLASS */
LRESULT CALLBACK mswcbk(HWND,UINT,WPARAM,LPARAM);  /* Callback för VARKON_IWIN_CLASS */

extern bool igbflg;

/*
******************************************************!*/

WPWIN wpwtab[WTABSIZ];

/* wpwtab är en tabell med typ och pekare till fönster.
   Typ är en kod som anger vilken typ av fönster det rör
   sig om tex. TYP_IWIN för ett input-fönster från MBS.
   Pekaren är en C-pekare som pekar på en structure av
   den aktuella typen tex. WPIWIN för ett input-fönster.

   Alla element i wpwtab initieras av mswini() till NULL.
   När ett nytt fönster skapas får det som ID lägsta lediga
   plats i wpwtab och när det deletas nollställs platsen
   igen.
*/

WPMES32 ms_lstmes;

/* Sist inkomna meddelande.
*/


extern char    jobnam[];
extern HPEN    ms_black,ms_darkg,ms_normg,ms_liteg,ms_white;
extern HCURSOR ms_grcu;

extern LRESULT CALLBACK mscblw(HWND,UINT,WPARAM,LPARAM);
extern LRESULT CALLBACK mscbgw(HWND,UINT,WPARAM,LPARAM);
extern LRESULT CALLBACK mscbrw(HWND,UINT,WPARAM,LPARAM);
extern LRESULT CALLBACK mscbew(HWND,UINT,WPARAM,LPARAM);



/*!******************************************************/

        int mswini()

/*      Initierar wpw-paketet. Skapar applikationens
 *      huvudfönster.
 *
 *      In: Inget.   
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *      2004-10-14 don't show window in batch-mode
 *
 ******************************************************!*/

  {
    int   i,x,y,dx,dy;
    char  buf[20],sndpth[V3PTHLEN+1];

/*
***Nollställ fönster-tabellen.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     wpwtab[i].typ = TYP_UNDEF;
     wpwtab[i].ptr = NULL;
     }
/*
***Registrera fönsterklasser.
*/
   if ( !msregc() ) return((int)GetLastError());
/*
***Skapa huvudfönstret.
*/
   x  = CW_USEDEFAULT;
   y  = CW_USEDEFAULT;
   dx = CW_USEDEFAULT;
   dy = CW_USEDEFAULT;

   if ( msgrst("APPWIN.X",buf) )  sscanf(buf,"%d",&x);
   if ( msgrst("APPWIN.Y",buf) )  sscanf(buf,"%d",&y);
   if ( msgrst("APPWIN.DX",buf) ) sscanf(buf,"%d",&dx);
   if ( msgrst("APPWIN.DY",buf) ) sscanf(buf,"%d",&dy);

   ms_main = CreateWindowEx(0,
                            VARKON_MAIN_CLASS,
                            "VARKON",
   		       	   		    WS_OVERLAPPEDWINDOW,
		  		   		    x,y,dx,dy,
		  			        NULL,
		  	   	            NULL,
						    ms_inst,
						    NULL);

   if ( ms_main == NULL ) return((int)GetLastError());

   if (!igbflg) {
      ShowWindow(ms_main,SW_SHOWNORMAL);
      }
/*
***Default cursor i grafiska fönster = pil.
*/
   ms_grcu = LoadCursor(NULL,IDC_ARROW);
/*
***Ett välkomnande ljud.
*/
   sprintf(sndpth,"%sV3_start.wav",v3genv(VARKON_SND));
   sndPlaySound(sndpth,SND_SYNC);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int mswexi()

/*      Avslutar wpw-paketet.
 *
 *      In: Inget.   
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    char  sndpth[V3PTHLEN+1];
    short i;

/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(MSPAC) == 1 )
      {
      fprintf(dbgfil(MSPAC),"***Start-mswexi***\n");
      fflush(dbgfil(MSPAC));
      }
#endif
/*
***Döda alla fönster i fönster-tabellen.
*/
   for ( i=0; i<WTABSIZ; ++i)
     if ( wpwtab[i].ptr != NULL ) mswdel((v2int)i);
/*
***Ett avslutande ljud.
*/
   sprintf(sndpth,"%sV3_end.wav",v3genv(VARKON_SND));
   sndPlaySound(sndpth,SND_SYNC);


#ifdef DEBUG
    if ( dbglev(MSPAC) == 1 )
      {
      fprintf(dbgfil(MSPAC),"***Slut-mswexi***\n");
      fflush(dbgfil(MSPAC));
      }
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        bool msregc()

/*      Registrerar dom fönsterklasser som V3
*       använder sig av.
 *
 *      In: Inget.   
 *
 *      Ut: Inget.   
 *
 *      FV: TRUE = Ok, FALSE = Fel från RegisterClass().
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      1998-01-13 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    WNDCLASS  class;

/*
***Börja med att registrera huvudfönstrets (applikationens) klass.
***Det är bara huvudfönstret (det enda) som har denna klass med
***egen vidhängande callback.
*/
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_MAIN_CLASS;
   class.lpfnWndProc   = mscbmw;
   class.style         = 0;
   class.hIcon         = LoadIcon(ms_inst,(LPCTSTR)IDI_ICON1);
   class.hCursor       = LoadCursor(NULL,IDC_ARROW);
   class.lpszMenuName  = 0;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(LTGRAY_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
/*
***Sen en klass för WPIWIN, WPBUTT och WPEDIT.
*/
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_IWIN_CLASS;
   class.lpfnWndProc   = mswcbk;
   class.style         = 0;
   class.hIcon         = LoadIcon(NULL,(LPCTSTR)IDI_APPLICATION);
   class.hCursor       = LoadCursor(NULL,IDC_ARROW);
   class.lpszMenuName  = 0;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(LTGRAY_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
/*
***Registrera också klasser för grafiska fönster. Grafiska
***fönster har annan bakgrundsfärg och egen callback. Ingen cursor
***eftersom detta skall skötas av v3. Huvudfönstret kan inte dödas.
*/
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_MGWIN_CLASS;
   class.lpfnWndProc   = mscbgw;
   class.style         = CS_OWNDC | CS_NOCLOSE;
   class.hIcon         = LoadIcon(NULL,(LPCTSTR)IDI_APPLICATION);
   class.hCursor       = NULL;
   class.lpszMenuName  = NULL;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(WHITE_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);

   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_GWIN_CLASS;
   class.lpfnWndProc   = mscbgw;
   class.style         = CS_OWNDC;
   class.hIcon         = LoadIcon(NULL,(LPCTSTR)IDI_APPLICATION);
   class.hCursor       = NULL;
   class.lpszMenuName  = NULL;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(WHITE_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_RWIN_CLASS;
   class.lpfnWndProc   = mscbrw;
   class.style         = CS_OWNDC;
   class.hIcon         = LoadIcon(NULL,(LPCTSTR)IDI_APPLICATION);
   class.hCursor       = NULL;
   class.lpszMenuName  = NULL;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(WHITE_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
/*
***Samt en klass för listfönster som också har en egen callback.
*/
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_LWIN_CLASS;
   class.lpfnWndProc   = mscblw;
   class.style         = 0;
   class.hIcon         = LoadIcon(ms_inst,(LPCTSTR)IDI_ICON1);
   class.hCursor       = LoadCursor(NULL,IDC_ARROW);
   class.lpszMenuName  = 0;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(WHITE_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
/*
***Samt en klass för editfönster med egen callback.
*/
   class.hInstance     = ms_inst;
   class.lpszClassName = VARKON_EWIN_CLASS;
   class.lpfnWndProc   = mscbew;
   class.style         = 0;
   class.hIcon         = LoadIcon(ms_inst,(LPCTSTR)IDI_ICON1);
   class.hCursor       = LoadCursor(NULL,IDC_ARROW);
   class.lpszMenuName  = 0;
   class.cbClsExtra    = 0;
   class.cbWndExtra    = 0;
   class.hbrBackground = GetStockObject(LTGRAY_BRUSH);

   if ( RegisterClass(&class) == 0 ) return(FALSE);
/*
***Slut.
*/
   return(TRUE);
  }

/********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mscbmw(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för VARKON:s huvudfönster.
 *
 *      In: win32_id = Fönster som det hänt något i (ms_main).
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 30/11/95 J. Kjellander
 *
 *      1997-05-16 Döda under uppstart, J.Kjellander
 *
 ******************************************************!*/

  {
   char title[V3STRLEN],lagra[V3STRLEN];

/*
***Huvudfönstrets menyer genererar WM_COMMAND som servas
***annorstädes. Därför servar vi WM_COMMAND bara genom att
***spara dom som alla andra meddelanden i ms_lstmes.
***Övriga meddelanden som hänför sig
***till huvudfönstret servar vi här.
*/
   ms_lstmes.wh  = win32_id;
   ms_lstmes.msg = message;
   ms_lstmes.wp  = wp;
   ms_lstmes.lp  = lp;

   switch ( ms_lstmes.msg )
	   {
      case WM_PAINT:
	  msrpmw();
	  break;

      case WM_SIZE:
      RedrawWindow(win32_id,NULL,NULL,RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT);
	  break;
/*
***Döda-knappen. Om jobnam = "" befinner vi oss i uppstart-läge
***och avslutar utan att lagra osv. 
*/
      case WM_CLOSE:
      if ( *jobnam == '\0' )
        {
        ExitProcess(0);
        }
      else
        {
        if ( !msgrst("EXIT.TITLE",title) ) strcpy(title,"Sluta VARKON");
        if ( !msgrst("EXIT.TEXT",lagra) ) strcpy(lagra,"Vill du lagra först ?");

        switch ( MessageBox(ms_main,lagra,
                            title,MB_YESNOCANCEL | MB_ICONQUESTION) )
          {
          case IDYES:
          igexsa();
          v3exit();
          break;

          case IDNO:
          igexsn();
          v3exit();
          break;

          default:
          return(0);
          break;
          }
        break;
        }

	   default:
	   return DefWindowProc(win32_id,message,wp,lp);
	   }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool mswrep(win32_id)
        HWND win32_id;

/*      Repaint-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess repaint-rutin.
 *
 *      In: ms_id = WIN32 fönster ID.
 *
 *      Ut: TRUE  = Eventet servat.   
 *          FALSE = Eventet ej i något wpw-fönster.
 *
 *      (C)microform ab 23/10/95 J. Kjellander
 *
 *      1998-01-13 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i,j;
    WPIWIN *iwinpt;
    WPGWIN *gwinpt;
    WPBUTT *buttpt;
    WPICON *icoptr;
    WPRWIN *rwinpt;

#ifdef DEBUG
    if ( dbglev(MSPAC) == 11 )
      {
      fprintf(dbgfil(MSPAC),"***Start-mswrep***\n");
	  fprintf(dbgfil(MSPAC),"win32_id=%d\n",win32_id);
      fflush(dbgfil(MSPAC));
      }
#endif

/*
***Sök igenom hela wpwtab efter fönster.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
/*
***Vilken typ av fönster är det ?
*/
        switch ( wpwtab[i].typ )
          {
/*
***WPIWIN-fönster. Kolla fönstret självt och 
***sök igenom alla sub-fönster.
*/
          case TYP_IWIN:
          iwinpt = (WPIWIN *)wpwtab[i].ptr;
          if ( iwinpt->id.ms_id == win32_id ) return(msrpiw(iwinpt));

          for ( j=0; j<WP_IWSMAX; ++j )
            {
            if ( iwinpt->wintab[j].ptr != NULL )
              {
              switch ( iwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)iwinpt->wintab[j].ptr;
                if ( buttpt->id.ms_id == win32_id )
                  {
                  msrpbu(buttpt);
                  return(TRUE);
                  }
                break;

                case TYP_ICON:
                icoptr = (WPICON *)iwinpt->wintab[j].ptr;
                if ( icoptr->id.ms_id == win32_id )
                  {
                  msrpic(icoptr);
                  return(TRUE);
                  }
                break;
                }
              }
            }
          break;
/*
***Grafiskt fönster.
*/
          case TYP_GWIN:
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
          if ( gwinpt->id.ms_id == win32_id )return(msrpgw(gwinpt));

          for ( j=0; j<WP_GWSMAX; ++j )
            {
            if ( gwinpt->wintab[j].ptr != NULL )
              {
              switch ( gwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)gwinpt->wintab[j].ptr;
                if ( buttpt->id.ms_id == win32_id )
                  {
                  msrpbu(buttpt);
                  return(TRUE);
                  }
                break;

                case TYP_ICON:
                icoptr = (WPICON *)gwinpt->wintab[j].ptr;
                if ( icoptr->id.ms_id == win32_id )
                  {
                  msrpic(icoptr);
                  return(TRUE);
                  }
                break;
                }
              }
            }
          break;
/*
***Renderingsfönster.
*/
          case TYP_RWIN:
          rwinpt = (WPRWIN *)wpwtab[i].ptr;
          if ( rwinpt->id.ms_id == win32_id )return(msrprw(rwinpt));

          for ( j=0; j<WP_RWSMAX; ++j )
            {
            if ( rwinpt->wintab[j].ptr != NULL )
              {
              switch ( rwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)rwinpt->wintab[j].ptr;
                if ( buttpt->id.ms_id == win32_id )
                  {
                  msrpbu(buttpt);
                  return(TRUE);
                  }
                break;

                case TYP_ICON:
                icoptr = (WPICON *)rwinpt->wintab[j].ptr;
                if ( icoptr->id.ms_id == win32_id )
                  {
                  msrpic(icoptr);
                  return(TRUE);
                  }
                break;
                }
              }
            }
          break;
          }
        }
     }
/*
***Det kan vara medelanderaden det gäller.
*/
   if ( msrpma(win32_id) ) return(TRUE);
/*
***Ingen träff.
*/
#ifdef DEBUG
    if ( dbglev(MSPAC) == 11 )
      {  
      fprintf(dbgfil(MSPAC),"status = FALSE\n");
      fprintf(dbgfil(MSPAC),"***Slut-mswrep***\n");
      fflush(dbgfil(MSPAC));
      }
#endif

   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool mswres(win32_id)
        HWND win32_id;

/*      Resize-rutinen för wpw-fönstren. Letar upp 
 *      rätt fönster och anropar dess resize-rutin.
 *
 *      In: ms_id = WIN32 fönster ID.
 *
 *      Ut: TRUE  = Resize utförd.
 *          FALSE = Inget av wp:s fönster.
 *
 *      Felkod: .
 *
 *      (C)microform ab 15/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i;
    WPLWIN *lwinpt;
    WPGWIN *gwinpt;

/*
***Sök igenom hela wpwtab efter fönster.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
/*
***Vilken typ av fönster är det ?
*/
        switch ( wpwtab[i].typ )
          {
/*
***WPLWIN-fönster.
*/
          case TYP_LWIN:
          lwinpt = (WPLWIN *)wpwtab[i].ptr;
/*          if ( lwinpt->id.ms_id == win32_id ) return(msrslw(lwinpt)); */
          break;
/*
***Grafiskt fönster.
*/
          case TYP_GWIN:
          gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*          if ( gwinpt->id.ms_id == win32_id )return(msrpgw(gwinpt)); */
          break;
          }
        }
     }
/*
***Ingen träff.
*/
   return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool mswbut(butmes,serv_id)
        WPMES32 *butmes;
        wpw_id  *serv_id;

/*      Button-rutinen för wpw-fönstren. Kollar
 *      vilken typ av fönster det är och anropar
 *      rätt rutin för jobbet.
 *
 *      In: butmes   = Message.
 *          serv_id  = Pekare till utdata.
 *
 *      Ut: *serv_id = ID för subfönster som servat meddelandet.
 *
 *      FV. TRUE  = Meddelandet har servats.
 *          FALSE = Meddelandetgällde inga av dessa fönster.
 *
 *      (C)microform ab 24/10/95 J. Kjellander
 *
 *       1998-01-14 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    short   i;
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    WPGWIN *gwinpt;
    WPRWIN *rwinpt;

/*
***Sök igenom wpwtab och anropa alla fönstrens
***respektive butt-hanterare. Den som vill kännas vid
***eventet tar hand om det.
*/
    for ( i=0; i<WTABSIZ; ++i )
      {
      if ( (winptr=mswgwp((wpw_id)i)) != NULL )
        {
        switch ( winptr->typ )
          {
          case TYP_IWIN:
          iwinpt = (WPIWIN *)winptr->ptr;
          if ( msbtiw(iwinpt,butmes,serv_id) ) return(TRUE);
          break;

          case TYP_GWIN:
          gwinpt = (WPGWIN *)winptr->ptr;
          if ( msbtgw(gwinpt,butmes,serv_id) ) return(TRUE);
          break;

          case TYP_RWIN:
          rwinpt = (WPRWIN *)winptr->ptr;
          if ( msbtrw(rwinpt,butmes,serv_id) ) return(TRUE);
          break;
          }
        }
      }

    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        void msrpmw()

/*      Repaint-rutin för V3:s huvudfönster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 1996-03-13 J. Kjellander
 *
 ******************************************************!*/

  {
   int         pdy;
   POINT       p[4];
   RECT        rect;
   HDC         dc;
   PAINTSTRUCT ps;

/*
***Fixa ett DC.
*/
   dc = BeginPaint(ms_main,&ps); 
/*
***Hur stort är fönstret ?
*/
   GetClientRect(ms_main,&rect);
/*
***Hur hög promtrad nertill ?
*/
   pdy = 15;
/*
***Skugga runt insidan av arbetsytan.
*/
   SelectObject(dc,ms_darkg);

   p[0].x = 0;
   p[0].y = rect.bottom - pdy - 6;
   p[1].x = 0;
   p[1].y = 0;
   p[2].x = rect.right - 1;
   p[2].y = 0;
   Polyline(dc,p,3);

   SelectObject(dc,ms_black);

   p[0].x = 1;
   p[0].y = rect.bottom - pdy - 7;
   p[1].x = 1;
   p[1].y = 1;
   p[2].x = rect.right - 2;
   p[2].y = 1;
   Polyline(dc,p,3);


   SelectObject(dc,ms_white);

   p[0].x = rect.right - 1;
   p[0].y = 0;
   p[1].x = rect.right - 1;
   p[1].y = rect.bottom - pdy - 5;
   p[2].x = -1;
   p[2].y = rect.bottom - pdy - 5;
   Polyline(dc,p,3);

   SelectObject(dc,ms_liteg);

   p[0].x = rect.right - 2;
   p[0].y = 1;
   p[1].x = rect.right - 2;
   p[1].y = rect.bottom - pdy - 6;
   p[2].x = 0;
   p[2].y = rect.bottom - pdy - 6;
   Polyline(dc,p,3);
/*
***Skugga runt insidan av promtraden.
*/
   SelectObject(dc,ms_darkg);

   p[0].x = 0;
   p[0].y = rect.bottom - 1;
   p[1].x = 0;
   p[1].y = rect.bottom - pdy - 2;
   p[2].x = rect.right - 1;
   p[2].y = rect.bottom - pdy - 2;
   Polyline(dc,p,3);

   SelectObject(dc,ms_white);

   p[0].x = rect.right - 1;
   p[0].y = rect.bottom - pdy - 2;
   p[1].x = rect.right - 1;
   p[1].y = rect.bottom - pdy + 1;
   p[2].x = rect.right - 14;
   p[2].y = rect.bottom - 1;
   p[3].x = 0;
   p[3].y = rect.bottom - 1;
   Polyline(dc,p,4);

   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 6;
   p[1].x = rect.right - 10;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);

   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 10;
   p[1].x = rect.right - 6;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);

   SelectObject(dc,ms_darkg);

   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 3;
   p[1].x = rect.right - 13;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);
   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 4;
   p[1].x = rect.right - 12;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);

   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 7;
   p[1].x = rect.right - 9;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);
   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 8;
   p[1].x = rect.right - 8;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);

   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 11;
   p[1].x = rect.right - 5;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);
   p[0].x = rect.right - 2;
   p[0].y = rect.bottom - pdy + 12;
   p[1].x = rect.right - 4;
   p[1].y = rect.bottom - 1;
   Polyline(dc,p,2);
/*
***Lämna tillbaks DC:t.
*/
	EndPaint(ms_main,&ps);
/*
***Slut.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        int mswshw(w_id)
        v2int w_id;

/*      Visar ett fönster.
 *
 *      In: w_id  = Entry i wpwtab.
 *
 *      Ut: Inget.   
 *
 *
 *      (C)microform ab 23/10/95 J. Kjellander
 *
 *      1996-12-09 Fokus på knappar, J.Kjellander
 *      1997-03-07 SetActive.., J.Kjellander
 *      1998-01-13 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
   wpw_id   editid;
   HWND     win32_id;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPLWIN  *lwinpt;
   WPGWIN  *gwinpt;
   WPBUTT  *buttpt;
   WPRWIN  *rwinpt;

/*
***Fixa en C-pekare till fönstrets entry i wpwtab.
*/
   if ( (winptr=mswgwp(w_id)) == NULL ) return(-2);
/*
***Vilken typ av fönster är det ?
*/
   switch ( winptr->typ )
     {
/*
***WPIWIN-fönster. Om det inehåller nån WPEDIT sätt focus på den första.
***Om inga editar finns men en defaultknapp sätter vi fokus på den.
*/
     case TYP_IWIN:
     iwinpt = (WPIWIN *)winptr->ptr;
     win32_id = iwinpt->id.ms_id;
     editid = msffoc(iwinpt,FIRST_EDIT);
     if ( editid > -1 ) msfoeb(iwinpt,editid,TRUE);
     else
       {
       buttpt = msdefb(w_id);
       if ( buttpt != NULL ) msfoeb(iwinpt,buttpt->id.w_id,TRUE);
       }
     break;

     case TYP_GWIN:
     gwinpt = (WPGWIN *)winptr->ptr;
     win32_id = gwinpt->id.ms_id;
     break;

     case TYP_RWIN:
     rwinpt = (WPRWIN *)winptr->ptr;
     win32_id = rwinpt->id.ms_id;
     break;

     case TYP_LWIN:
     lwinpt = (WPLWIN *)winptr->ptr;
     win32_id = lwinpt->id.ms_id;
     break;

     default:
     return(-2);
     }
/*
***Visa resultatet och aktivera fönstret.
*/  
   ShowWindow(win32_id,SW_SHOWNORMAL);
   UpdateWindow(win32_id);
//JKJKJK   SetActiveWindow(iwinpt->id.ms_id);

   return(0);
  }

/********************************************************/
/*!*******************************************************/

     int mswwtw(iwin_id,slevel,subw_id)
     v2int  iwin_id;
     v2int  slevel;
     v2int *subw_id;

/*   Message-loop för MBS-rutinen WAIT_WIN. Lägger sig
 *   och väntar på meddelanden i det WPIWIN-fönster som angetts.
 *
 *   In: iwin_id = ID för huvudfönstret.
 *       slevel  = Service-nivå för key-event.
 *       subw_id = Pekare till utdata.
 *
 *   Ut: *subw_id = ID för det subfönster där ett event inträffat.
 *
 *   Felkoder : WP1202 = iwin_id %s är ej ett fönster
 *
 *   (C)microform ab 23/10/95 J. Kjellander
 *
 *******************************************************!*/

 {
    wpw_id  par_id,serv_id;
    char    errbuf[80];
    WPWIN  *winptr;
    WPIWIN *iwinpt;
    MSG     message;

/*
***Kolla att fönstret finns.
*/
   if ( (winptr=mswgwp((wpw_id)iwin_id)) == NULL )
     {
     sprintf(errbuf,"%d",(int)iwin_id);
     return(erpush("WP1202",errbuf));
     }
/*
***Om det är ett WPIWIN, aktivera det så det ser lite
***trevligare ut. Highligtad titelbalk etc. OBS. Aktivering
***flyttad till mswshw() 970307 JK. Anledningen är att göra
***det möjligt för ett WPLWIN all lägga sig ovanpå ett WPIWIN
***i en wait_win()-loop.
*/
   switch ( winptr->typ )
     {
     case TYP_IWIN:
     iwinpt = (WPIWIN *)winptr->ptr;
/*     SetActiveWindow(iwinpt->id.ms_id); */

     if ( iwinpt->focus_id > -1 ) msfoeb(iwinpt,iwinpt->focus_id,TRUE);

     break;
     }
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
     TranslateMessage(&message);    /* Translate virtual keycodes */
                                    /* to ASCII */
     DispatchMessage(&message);	    /* Tillbaks till windows igen */
    	                            /* och sen till callback-rutinen */
/*
***Om det var ett meddelande om en tangent eller mustryckning
***tar vi hand om det här.
*/
     switch ( ms_lstmes.msg )
       {
/*
***Musknapp. Tryckning i annat fönster än det vi väntar på bryr
***vi oss inte om tillsvidare.
*/
       case WM_LBUTTONDOWN:
       par_id = mswfpw(ms_lstmes.wh);
       if ( par_id != iwin_id ) goto loop;

       if ( mswbut(&ms_lstmes,&serv_id) )
         {
         *subw_id = (v2int)serv_id;
          return(TRUE);
         }
       else goto loop;
/*
***Musclickning i fönster skapat med fönsterklass = BUTTON.
*/
       case WM_COMMAND:
       if ( HIWORD(ms_lstmes.wp) == BN_CLICKED )
         {
         par_id = mswfpw((HWND)ms_lstmes.lp);
         if ( par_id != iwin_id ) goto loop;

         ms_lstmes.wh = (HWND)ms_lstmes.lp;       
         if ( mswbut(&ms_lstmes,&serv_id) )
           {
           *subw_id = (v2int)serv_id;
            return(TRUE);
           }
         else goto loop;
         }
       else goto loop;

       default:
       goto loop;
       }
     }
/*
***Slut.
*/
#ifdef DEBUG
    if ( dbglev(MSPAC) == 1 )
      {
      fprintf(dbgfil(MSPAC),"***Slut-mswwtw***\n\n");
      fflush(dbgfil(MSPAC));
      }
#endif

   return(0);
 }

/*********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mswcbk(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för WPIWIN-fönster.
 *
 *      In: win32_id = Fönster som det hänt något i.
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 23/10/95 J. Kjellander
 *
 ******************************************************!*/

  {	
#ifdef DEBUG
    if ( dbglev(MSPAC) == 11 )
      {
      fprintf(dbgfil(MSPAC),"***Start-mswcbk\n***");
      fprintf(dbgfil(MSPAC),"win32_id = %d, message = %s\n",
                             win32_id,msmest(message));
      fprintf(dbgfil(MSPAC),"***Slut-mswcbk***\n");
      fflush(dbgfil(MSPAC));
      }
#endif

   ms_lstmes.wh  = win32_id;
   ms_lstmes.msg = message;
   ms_lstmes.wp  = wp;
   ms_lstmes.lp  = lp;

   switch ( ms_lstmes.msg )
	   {
	   case WM_PAINT:
	   if ( !mswrep(win32_id) )
	    return DefWindowProc(win32_id,message,wp,lp);
	   break;

	   default:
	   return DefWindowProc(win32_id,message,wp,lp);
	   }


   return(0);
  }

/********************************************************/
/*!*******************************************************/

        int mswdel(w_id)
        v2int w_id;

/*      Dödar ett huvudfönster med alla subfönster.
 *
 *      In: w_id   = Huvudfönstrets entry i wpwtab.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudfönstret finns ej.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1998-01-13 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
    char     errbuf[80];
 
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPLWIN  *lwinpt;
    WPGWIN  *gwinpt;
    WPEWIN  *ewinpt;
    WPRWIN  *rwinpt;

/*
***Fixa en C-pekare till huvud-fönstrets entry i wpwtab.
*/
    if ( (winptr=mswgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av fönster är det ?
*/
    switch ( winptr->typ )
      {
      case TYP_IWIN:
      iwinpt = (WPIWIN *)winptr->ptr;
      DestroyWindow(iwinpt->id.ms_id);
      msdliw(iwinpt);
      break;

      case TYP_LWIN:
      lwinpt = (WPLWIN *)winptr->ptr;
      DestroyWindow(lwinpt->id.ms_id);
      msdllw(lwinpt);
      break;

      case TYP_GWIN:
      gwinpt = (WPGWIN *)winptr->ptr;
      DestroyWindow(gwinpt->id.ms_id);
      msdlgw(gwinpt);
      break;

      case TYP_RWIN:
      rwinpt = (WPRWIN *)winptr->ptr;
      DestroyWindow(rwinpt->id.ms_id);
      msdlrw(rwinpt);
      break;

      case TYP_EWIN:
      ewinpt = (WPEWIN *)winptr->ptr;
      DestroyWindow(ewinpt->id.ms_id);
      msdlew(ewinpt);
      break;
      }
/*
***Stryk fönstret ur fönstertabellen.
*/
    winptr->typ = TYP_UNDEF;
    winptr->ptr = NULL;
   
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int   mswdls(w_id,sub_id)
        v2int w_id;
        v2int sub_id;

/*      Dödar ett subfönster.
 *
 *      In: w_id   = Huvudfönstrets entry i wpwtab.
 *          sub_id = Subfönstrets id.
 *
 *      Ut: Inget.   
 *
 *      Felkod: WP1222 = Huvudfönstret finns ej.
 *              WP1232 = Subfönstret finns ej.
 *
 *      (C)microform ab 1996-03-06 J. Kjellander
 *
 *      1996-05-20 WPGWIN, J.Kjellander
 *      2000-03-14 Clipregion, J.Kjellander
 *
 ******************************************************!*/

  {
    char     errbuf[80];
    char    *subptr;
    WPWIN   *winptr;
    WPIWIN  *iwinpt;
    WPGWIN  *gwinpt;
    WPBUTT  *butptr;
    WPEDIT  *edtptr;
    WPICON  *icoptr;

/*
***Fixa en C-pekare till huvud-fönstrets entry i wpwtab.
*/
    if ( (winptr=mswgwp(w_id)) == NULL )
      {
      sprintf(errbuf,"%d",(int)w_id);
      return(erpush("WP1222",errbuf));
      }
/*
***Vilken typ av fönster är det ?
*/
    switch ( winptr->typ )
      {
/*
***WPIWIN, kolla att subfönstret finns.
*/
      case TYP_IWIN:
      if ( sub_id < 0  ||  sub_id > WP_IWSMAX-1 )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
      iwinpt = (WPIWIN *)winptr->ptr;
      subptr = iwinpt->wintab[(wpw_id)sub_id].ptr;
      if ( subptr == NULL )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
/*
***Döda fönstret ur wpw och WIN32.
*/
      switch ( iwinpt->wintab[(wpw_id)sub_id].typ )
        {
        case TYP_BUTTON:
        butptr = (WPBUTT *)subptr;
        DestroyWindow(butptr->id.ms_id);
        msdlbu(butptr);
        break;

        case TYP_EDIT:
        edtptr = (WPEDIT *)subptr;
        DestroyWindow(edtptr->id.ms_id);
        msdled(edtptr);
        break;

        case TYP_ICON:
        icoptr = (WPICON *)subptr;
        DestroyWindow(icoptr->id.ms_id);
        msdlic(icoptr);
        break;
        }
/*
***Länka bort subfönstret från WPIWIN-fönstret.
*/
      iwinpt->wintab[(wpw_id)sub_id].ptr = NULL;
      iwinpt->wintab[(wpw_id)sub_id].typ = TYP_UNDEF;
      break;
/*
***WPGWIN.
*/
      case TYP_GWIN:
      if ( sub_id < 0  ||  sub_id > WP_GWSMAX-1 )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
      gwinpt = (WPGWIN *)winptr->ptr;
      subptr = gwinpt->wintab[(wpw_id)sub_id].ptr;
      if ( subptr == NULL )
        {
        sprintf(errbuf,"%d%%%d",(int)w_id,(int)sub_id);
        return(erpush("WP1232",errbuf));
        }
/*
***Döda fönstret ur wpw och WIN32.
*/
      SelectClipRgn(gwinpt->dc,NULL);

      switch ( gwinpt->wintab[(wpw_id)sub_id].typ )
        {
        case TYP_BUTTON:
        butptr = (WPBUTT *)subptr;
        DestroyWindow(butptr->id.ms_id);
        msdlbu(butptr);
        break;

        case TYP_EDIT:
        edtptr = (WPEDIT *)subptr;
        DestroyWindow(edtptr->id.ms_id);
        msdled(edtptr);
        break;

        case TYP_ICON:
        icoptr = (WPICON *)subptr;
        DestroyWindow(icoptr->id.ms_id);
        msdlic(icoptr);
        break;
        }

      gwinpt->wintab[(wpw_id)sub_id].ptr = NULL;
      gwinpt->wintab[(wpw_id)sub_id].typ = TYP_UNDEF;

      UpdateWindow(gwinpt->id.ms_id);
      SelectClipRgn(gwinpt->dc,gwinpt->clprgn);
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        wpw_id mswffi()

/*      Letar upp lägsta lediga entry i wpwtab.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Giltigt ID eller erpush().
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
    int i;
/*
***Leta upp ledig plats i fönstertabellen. Lämna ID = 0
***ledigt eftersom detta ID är reserverat för V3:s grafiska
***huvudfönster.
*/
    i = 1;

    while ( i < WTABSIZ  &&  wpwtab[i].ptr != NULL ) ++i;
/*
***Finns det någon ?
*/
    if ( i == WTABSIZ ) return(-2);
/*
***Ja, returnera ID.
*/
    else return((wpw_id)i);
  }

/********************************************************/
/*!******************************************************/

        wpw_id mswfpw(win32_id)
        HWND  win32_id;

/*      Letar upp id för föräldern till ett sub-
 *      fönster med visst ms-id. Om fönstret med
 *      det angivna ms-id:t är en förälder returneras
 *      -1.
 *
 *      Denna rutin används av mswwtw() för att avgöra
 *      om ett meddelande kommer från ett subfönster till det
 *      huvudfönster som vi väntar på.
 *
 *      In: win32_id  = Subfönstrets ms-id.
 *
 *      Ut: Inget.   
 *
 *      FV: id för fönster eller -1.
 *
 *      (C)microform ab 28/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
    short    i,j;
    WPIWIN  *iwinpt;
    WPBUTT  *buttpt;
    WPICON  *iconpt;

/*
***Sök igenom hela wpwtab efter fönster.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
/*
***Vilken typ av fönster är det ?
*/
        switch ( wpwtab[i].typ )
          {
/*
***WPIWIN-fönster. Kolla fönstret självt och 
***sök igenom alla sub-fönster. Tillsvidare är
***det bara aktuellt med knappar.
*/
          case TYP_IWIN:
          iwinpt = (WPIWIN *)wpwtab[i].ptr;
          

          for ( j=0; j<WP_IWSMAX; ++j )
            {
            if ( iwinpt->wintab[j].ptr != NULL )
              {
              switch ( iwinpt->wintab[j].typ ) 
                {
                case TYP_BUTTON:
                buttpt = (WPBUTT *)iwinpt->wintab[j].ptr;
                if ( buttpt->id.ms_id == win32_id ) return(iwinpt->id.w_id);
                break;

                case TYP_ICON:
                iconpt = (WPICON *)iwinpt->wintab[j].ptr;
                if ( iconpt->id.ms_id == win32_id ) return(iwinpt->id.w_id);
                break;
                }
              }
            }
          break;
          }
        }
     }
/*
***Ingen träff.
*/
    return((wpw_id)-1);
  }

/********************************************************/
/*!******************************************************/

        WPWIN *mswgwp(id)
        wpw_id id;

/*      Översätter id till C-pekare för motsvarande entry
 *      i wpwtab.
 *
 *      In: id = Fönstrets entry i wpwtab.
 *
 *      Ut: Inget.
 *
 *      FV: Giltig C-pekare eller NULL.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Är det ett giltigt ID ?
*/
    if ( id < 0  ||  id >= WTABSIZ ) return(NULL);
/*
***Ja, returnera pekare om det finns någon.
*/
    else
      {
      if ( wpwtab[id].ptr != NULL ) return(&wpwtab[id]);
      else return(NULL);
      }
  }

/********************************************************/
