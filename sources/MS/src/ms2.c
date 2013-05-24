/**********************************************************************
*
*    ms2.c
*    =====
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msinla();   Init list window
*     msalla();   Add line to list window
*     msexla();   Close and display list window
*     mssvlw();   Save list window to file
*     msdllw();   Delete list window
*     msglwp();   Get list window pointer
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
#include <io.h>
#include <fcntl.h>


LRESULT CALLBACK mscblw(HWND,UINT,WPARAM,LPARAM);


static WPLWIN  *actlwin = NULL;

/* actlwin är en C-pekare till ett WPLWIN som skapats (wpinla) men
   ännu ej blivit färdig (msexla). Om actlwin = NULL finns inget
   sådant fönster på gång. */

HFONT ms_lfont; /* Font för listfönster. */


extern char   jobnam[],jobdir[];


/*!******************************************************/

        int msinla(hs)
        char *hs;

/*      Skapar nytt list-fönster, wp:s iginla(). MBS-
 *      LST_INI(rubrik);
 *
 *      In: hs = Rubriktext.  
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 28/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
    char     templ[V3PTHLEN+1],tmpfil[V3PTHLEN+1];
    wpw_id   id;
    WPLWIN  *lwinptr;
    FILE    *fp;

/*
***Kolla att inte ett listfönster redan är på gång.
***Isåfall, stäng det. Därmed
***borde det vara OK för errmes() att skapa ny lista
***med felmeddelanden.
*/
    if ( actlwin != NULL ) msexla(TRUE);
/*
***Allra först ser vi till att öppna en ny temporär-fil.
***Skulle det misslyckas är ju ingen skada skedd och vi 
***slipper jobbig felhantering med malloc() och sånt.
*/  
    strcpy(templ,v3genv(VARKON_TMP));
    strcat(templ,jobnam);
    strcat(templ,".XXXXXX");
    mktemp(templ);
    strcpy(tmpfil,templ);

    if ( (fp=fopen(tmpfil,"w+")) == NULL ) return(-2);
/*
***Skapa ett ledigt fönster-ID.
*/
    if ( (id=mswffi()) < 0 ) return(-2);
/*
***Skapa ett interimistiskt WPLWIN.
***Alla data finns ännu inte. WIN32-fönstret är tex. inte skapat.
***Detta görs först i msexla() som då får fylla i resten.
*/
    if ( (lwinptr=(WPLWIN *)v3mall(sizeof(WPLWIN),"msinla"))
                                                   == NULL ) return(-2);

    lwinptr->id.w_id  = id;
    lwinptr->id.p_id  = NULL;
    lwinptr->id.ms_id = NULL;
    lwinptr->edit_id  = NULL;

    lwinptr->geo.x  =  0;
    lwinptr->geo.y  =  0;
    lwinptr->geo.dx =  0;
    lwinptr->geo.dy =  0;

    lwinptr->filpek = fp;
    strcpy(lwinptr->filnam,tmpfil);
/*
***Skriv ut rubriken till listfilen.
*/
    fprintf(lwinptr->filpek,"%s\n\n",hs);
    lwinptr->filsiz = strlen(hs) + 4;
    lwinptr->maxlen = strlen(hs);
    lwinptr->radant = 2;
/*
***Lagra fönstret i fönstertabellen.
***Fönstret är ännu ej komplett med alla data tex. WIN32-id eller
***geometri men detta skall inte vara något problem.
*/
    wpwtab[id].typ = TYP_LWIN;
    wpwtab[id].ptr = (char *)lwinptr;
/*
***Sätt pekaren till aktivt listfönster.
*/
    actlwin = lwinptr;
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int  msalla(s,rs)
        char *s; 
        short rs;

/*      Skriver in en rad text i listfilen.
 *
 *      In: s  = textsträng. 
 *          rs = radsprång.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 28/10/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int i,ntkn;

/*
***Kolla att aktivt listfönster verkligen finns.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Kolla att raden inte är längre än 80 tecken.
*/
    if ( (ntkn=strlen(s)) > V3STRLEN ) s[V3STRLEN] = '\0';
/*
***Är den längre än någon annan rad ?
*/
    if ( ntkn > actlwin->maxlen ) actlwin->maxlen = ntkn;
/*
***Skriv ut raden till temporärfilen.
*/
    fprintf(actlwin->filpek,"%s\n",s);

    actlwin->filsiz += ntkn + 2;
  ++actlwin->radant;
/*
***Skriv ut ev. radsprång.
*/
    for ( i=0; i<rs - 1; i++ )
      {
      fprintf(actlwin->filpek,"\n");
      actlwin->filsiz += 2;
    ++actlwin->radant;
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        int  msexla(show)
        bool show;

/*      Avslutar en listning till ett listfönster.
 *      Stänger temporär-filen, skapar WIN32-fönstret.
 *
 *      In: show = Visa ja/nej.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 14/11/95 J. Kjellander
 *
 *      1997-03-07 Bring-to-top, J.Kjellander
 *
 ******************************************************!*/

  {
   int        dx,dy,fd,ntkn,style;
   char      *buf;
   char       titel[V3STRLEN],arkiv[V3STRLEN],spara[V3STRLEN],
              skriv[V3STRLEN],edit[V3STRLEN],copy[V3STRLEN];
   int        px,py,edx,edy;
   bool       horscr,verscr;
   WPWIN     *winptr;
   HWND       lwin_id,edit_id;
   HDC        dc;
   HFONT      old_font;
   TEXTMETRIC tm;
   HMENU      menu,popup;

/*
***Kolla att ett listfönster verkligen är på gång.
*/
   if ( actlwin == NULL ) return(-2);
/*
***Stäng temporärfilen.
*/
   fclose(actlwin->filpek);
/*
***Om vi skall avsluta listningen utan att visa listan är det enkelt.
*/
   if ( !show )
     {
     v3fdel(actlwin->filnam);
     if ( (winptr=mswgwp(actlwin->id.w_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       v3free((char *)actlwin,"msexla");
       }
     actlwin = NULL;
     return(0);
     }
/*
***Ta reda på aktuell fontstorlek.
*/
  dc = GetDC(ms_main);
  old_font = SelectObject(dc,ms_lfont);
  GetTextMetrics(dc,&tm);
  SelectObject(dc,old_font);
  ReleaseDC(ms_main,dc);
/*
***Listfönstrets storlek i X-led = Max radlängd + 2 marginaler + scroll-
***list + 2 ramar.
*/
   dx = tm.tmMaxCharWidth*actlwin->maxlen + 2*5 +
        GetSystemMetrics(SM_CXVSCROLL) + 2*GetSystemMetrics(SM_CXFRAME);

   if ( dx > msmwsx() )
     {
     horscr = TRUE;
     dx = msmwsx();
     }
   else horscr = FALSE;
/*
***Och i Y-led = antal rader*texthöjd + titelbalk + menybalk + 2 ramar.
***Det har visat sig att det behövs lite extra plats annars kan sista
***raden försvinna. Därav actlwin->radant + 1.
*/
   dy = (actlwin->radant+1) * (tm.tmHeight + tm.tmExternalLeading) + 
        2*GetSystemMetrics(SM_CYCAPTION) + 2*GetSystemMetrics(SM_CYFRAME);

   if ( horscr ) dy += GetSystemMetrics(SM_CYHSCROLL);

   if ( dy > msmwsy() )
     {
     verscr = TRUE;
     dy = msmwsy();
     }
   else
     {
     dx -= GetSystemMetrics(SM_CXVSCROLL);
     verscr = FALSE;
     }
/*
***Fönstertitel.
*/
   strcpy(titel,"l-");
   strcat(titel,jobnam);
/*
***Listfönstrets meny.
*/
   if ( !msgrst("MENU.FILE",arkiv) ) strcpy(arkiv,"Arkiv");
   if ( !msgrst("MENU.SAVE_AS",spara) ) strcpy(spara,"Spara som..");
   if ( !msgrst("MENU.PRINT",skriv) ) strcpy(skriv,"Skriv ut");
   if ( !msgrst("MENU.EDIT",edit) ) strcpy(edit,"Redigera");
   if ( !msgrst("MENU.COPY",copy) ) strcpy(copy,"Kopiera");


   menu  = CreateMenu();

   popup = CreatePopupMenu();
   AppendMenu(menu,MF_STRING | MF_POPUP,(UINT)popup,arkiv);
   AppendMenu(popup,MF_STRING,V3_SAVE_AS,spara);
   AppendMenu(popup,MF_STRING,V3_PRINT,skriv);

   popup = CreatePopupMenu();
   AppendMenu(menu,MF_STRING | MF_POPUP,(UINT)popup,edit);
   AppendMenu(popup,MF_STRING,V3_COPY,copy);
/*
***Skapa WIN32-fönster.
*/
   lwin_id = CreateWindow(VARKON_LWIN_CLASS,
                          titel,
   		  		   		  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		      			  CW_USEDEFAULT,
		      			  CW_USEDEFAULT,
		  	      		  dx,
		  	      		  dy,
		  	      		  ms_main,
		  	      		  menu,
			      		  ms_inst,
			      		  NULL); 

   actlwin->id.ms_id = lwin_id;
/*
***Hur stor blev applikationsarean. Så stor kan edit:en bli.
*/
   msggeo(lwin_id,&px,&py,&edx,&edy,NULL,NULL);
/*
***Sen ett WIN32-Editfönster i WPLWIN-fönstret.
*/
   style =  WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY;

   if ( horscr ) style = style | WS_HSCROLL;
   if ( verscr ) style = style | WS_VSCROLL;

   edit_id = CreateWindow("EDIT",
                          "",
                          style,
		      		      5,
		         	      5,
		  	      		  edx-10,
		  	      		  edy-10,
		  	      		  lwin_id,
		  	      		  NULL,
			      		  ms_inst,
			      		  NULL);

   actlwin->edit_id = edit_id;
/*
***Allokera buffert för listtext.
*/
   if ( (buf=v3mall(actlwin->filsiz + 1,"msexla")) == NULL ) return(-2);
/*
***Öppna filen igen och läs till den allokerade bufferten.
*/
   if ( (fd=open(actlwin->filnam,_O_RDONLY | _O_BINARY)) == -1 ) return(-2);

   ntkn = read(fd,buf,actlwin->filsiz);

   close(fd);
/*
***Lägg ett NULL sist.
*/
  *(buf+ntkn) = '\0';
/*
***Fyll på med texten.
*/
   SendMessage(edit_id,WM_SETFONT,(WPARAM)ms_lfont,MAKELPARAM(FALSE,0));
   SendMessage(edit_id,WM_SETTEXT,0,(LPARAM)buf);
   if ( horscr  ||  verscr ) SetFocus(edit_id);
/*
***Deallokera bufferten igen.
*/
   v3free(buf,"msexla");
/*
***Aktivt listfönster finns ej nu längre.
*/
   actlwin = NULL;
/*
***Se till att listfönstret läggs på topp
***och blir aktivt.
*/
   BringWindowToTop(lwin_id);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mscblw(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för WPLWIN-fönster.
 *
 *      In: win32_id = Fönster som det hänt något i.
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 15/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int     dx,dy;
   WPWIN  *winptr;
   WPLWIN *lwinpt;

   switch ( message )
	   {
/*
***Kommer det ett WM_SIZE till WPLWIN-fönstret har det ändrat
***storlek och då skall även dess edit-fönster ändras.
*/
      case WM_SIZE:
      dx = LOWORD(lp);
      dy = HIWORD(lp);
      if ( (lwinpt=msglwp(win32_id)) != NULL  &&  lwinpt->edit_id != NULL )
        MoveWindow(lwinpt->edit_id,5,5,dx-10,dy-10,TRUE);
	   break;
/*
***WM_CLOSE innebär clickning på döda-knappen.
***msdllw() stryker inte WPLWIN-fönstret ur wpwtab så
***det får vi göra här.
*/
      case WM_CLOSE:
      if ( (lwinpt=msglwp(win32_id)) != NULL )
        {
        if ( (winptr=mswgwp(lwinpt->id.w_id)) != NULL )
          {
          winptr->typ = TYP_UNDEF;
          winptr->ptr = NULL;
          }
        msdllw(lwinpt);
        DestroyWindow(win32_id);
        if ( !IsWindowEnabled(ms_main) ) EnableWindow(ms_main,TRUE);
        }
      break;
/*
***WM_COMMAND innebär menyval.
*/
      case WM_COMMAND:
      if ( HIWORD(wp) == 0  &&  (lwinpt=msglwp(win32_id)) != NULL )
        {
        switch ( LOWORD(wp) )
          {
/*
***Spara listfönstret i fil.
*/
          case V3_SAVE_AS:
          mssvlw(lwinpt);
          break;
/*
***Skriv ut på skrivaren.
*/
          case V3_PRINT:
          msprtf("VARKON-listfönster",lwinpt->filnam);
          break;
/*
***Kopiera selekterad text till clipboard..
*/
          case V3_COPY:
          SendMessage(lwinpt->edit_id,WM_COPY,0,0);
          break;
          }
        }
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

        int mssvlw(lwinpt)
        WPLWIN *lwinpt;

/*      Sparar ett listfönster i en fil.
 *
 *      In: lwinpt = C-pekare till listfönster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 15/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int  i;
   char dirnam[256],filnam[256],filter[256],ftitel[256];

   static OPENFILENAME ofn;

/*
***Filkatalog.
*/
   strcpy(dirnam,jobdir);
/*
***Filnamn.
*/
   strcpy(filnam,jobnam);
   strcat(filnam,".TXT");
/*
***Filter. Två strängar efter varandra + ett extra null sist.
***Textfiler(*.TXT)\0*.txt\0\0
*/
   strcpy(filter,"Textfiler(*.TXT)|*.txt|Datafiler(*.DAT)|*.dat|Alla filer(*.*)|*|");

   for ( i=0; filter[i] != '\0'; ++i )
     {
     if ( filter[i] == '|' ) filter[i] = '\0';
     }
/*
***Initiera ofn.
*/
   ofn.lStructSize       = sizeof(OPENFILENAME);
   ofn.hwndOwner         = lwinpt->id.ms_id;
   ofn.lpstrFilter       = filter;
   ofn.nFilterIndex      = 1;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter    = 0;
   ofn.lpstrFile         = filnam;
   ofn.nMaxFile          = sizeof(filnam);
   ofn.lpstrFileTitle    = ftitel;
   ofn.nMaxFileTitle     = sizeof(ftitel);
   ofn.lpstrInitialDir   = dirnam;
   ofn.Flags             = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY |
                           OFN_PATHMUSTEXIST;
/*
***Fråga efter nytt filnamn och kopiera om OK.
*/
   if ( GetSaveFileName(&ofn) )
     {
     v3fcpy(lwinpt->filnam,filnam);
     }

    return(0);
 }

/********************************************************/
/*!******************************************************/

        int     msdllw(lwinpt)
        WPLWIN *lwinpt;

/*      Dödar ett WPLWIN-fönster.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1/11/95
 *
 ******************************************************!*/

  {
/*
***Ta bort rätt temporärfil.
*/
   v3fdel(lwinpt->filnam);
/*
***Lämna tillbaks allokerat minne för själva listfönstret.
*/
   v3free((char *)lwinpt,"msdllw");
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        WPLWIN *msglwp(win32_id)
        HWND win32_id;

/*      Översätter MS-id till WPLWIN-C-Pekare.
 *
 *      In: ms_id = WIN32 fönster ID.
 *
 *      Ut: Inget.
 * 
 *      FV: C-pekare till WPLWIN-post eller NULL.
 *
 *      (C)microform ab 15/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i;
   WPLWIN *lwinpt;

/*
***Sök igenom hela wpwtab efter WPLWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL  &&  wpwtab[i].typ == TYP_LWIN )
       {
       lwinpt = (WPLWIN *)wpwtab[i].ptr;
       if ( lwinpt->id.ms_id == win32_id ) return(lwinpt);
       }
     }
/*
***Ingen träff.
*/
   return(NULL);
  }

/********************************************************/
