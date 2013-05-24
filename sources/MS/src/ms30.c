/**********************************************************************
*
*    ms30.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msamod();   Edit active module
*     msomod();   Edit other module
*     mscoal();   Compile all
*     msgewp();   Id to C-ptr for WPEWIN
*     msdlew();   Delete WPEWIN
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
#include "../../../sources/PM/include/ppdef.h"
#include "../../../sources/IG/include/isch.h"

static bool mledit();    /* Editerar modul (MBS-fil) */
static bool mlfind();    /* Söker i MBS-text */
static bool fndmes();    /* Processar sök-meddelande */
static bool fndcmp();    /* Jämför två strängar */
static bool comp_1();    /* Kompilerar MBS-fil = Aktiv modul = Visst fönster */
static bool comp_2();    /* Kompilerar MBS-fil och flyttar MBO */
static bool comp_file(); /* Kompilerar MBS-fil */
static bool getprg();    /* Frågar efter modulnamn (MBS-namn) */
static bool saveew();    /* Sparar edit-fönster som MBS-fil */
static bool addcr();     /* Konverterar LF till CR+LF */

LRESULT CALLBACK mscbew(HWND,UINT,WPARAM,LPARAM); /* Callback för editfönster */
/*
*******************************************************!*/

extern HFONT  ms_lfont;
extern char   jobdir[],jobnam[],mbsdir[],mbodir[];

/*!******************************************************/

        short msamod()

/*      Huvudrutin för editering av aktiv modul
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/11/95 J. Kjellander
 *
 *      1996-12-05 Engelska texter, J.Kjellander
 *
 ******************************************************!*/

 {
   char  tmpnam[V3PTHLEN+1],namn[JNLGTH+5];
   char  rubrik[V3PTHLEN+40];
   FILE *fp;

/*
***Öppna temporär fil.
*/
   strcpy(tmpnam,v3genv(VARKON_TMP));
   strcat(tmpnam,jobnam);
   strcat(tmpnam,MBSEXT);

   if ( (fp=fopen(tmpnam,"w")) == NULL )
     {
     MessageBox(ms_main,tmpnam,
               "Systemfel, kan ej öppna fil",MB_OK | MB_ICONSTOP);
     return(0);
     }
/*
***Dekompilera.
*/
   pprmo(PPFILE,fp);
/*
***Stäng filen igen.
*/
   fclose(fp);
/*
***Editera.
*/
   if ( !msgrst("ACTMEDIT.TITLE",rubrik) ) strcpy(rubrik,"Aktiv modul : ");
   strcpy(namn,jobnam);
   strcat(namn,MBSEXT);
   mledit(rubrik,v3genv(VARKON_TMP),namn,TRUE);
/*
***Städa bort den temporära MBS-filen.
*
   v3fdel(tmpnam);
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short msomod()

/*      Huvudrutin för editering av "annan modul" dvs.
 *      godtyckligt MBS-program.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/11/95 J. Kjellander
 *
 *      1996-12-05 Engelska texter, J.Kjellander
 *      1998-04-01 Ny msilse(), J.Kjellander
 *      2004-07-18 Bug, defnam, J.Kjellander, Örebro universitet
 *
 ******************************************************!*/

  {
   short status;
   char  path[V3PTHLEN],mesbuf[V3STRLEN],rubrik[V3STRLEN],
         namn[V3PTHLEN];
   char *pekarr[1000],strarr[20000];
   int   nstr;

static char defnam[V3PTHLEN] = "";

/*
***Skapa filförteckning. mbsdir/*.MBS
***Resultatet är filnamn utan extension.
*/
   igdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Låt användaren välja och lägg till extension.
*/
   status = msilse(50,50,iggtts(464),defnam,pekarr,-1,nstr,namn);
   if ( status < 0 ) return(REJECT);
   strcpy(defnam,namn);
   strcat(namn,MBSEXT);
/*
***Editera.
*/
   strcpy(path,mbsdir);
   strcat(path,namn);

   if ( !msgrst("PARTEDIT.NEWMOD",mesbuf) )
     strcpy(mesbuf,"Modulen finns ej, vill du skapa ?");

   if (  v3ftst(path)  ||  MessageBox(ms_main,
                                      mesbuf,
                                      path,
                                      MB_YESNO | MB_ICONQUESTION) == IDYES )
     {
     if ( !msgrst("PARTEDIT.TITLE",rubrik) ) strcpy(rubrik,"Editera : ");
     mledit(rubrik,mbsdir,namn,FALSE);
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short mscoal()

/*      Varkonfunktion för kompilering av alla.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1996-02-06 J. Kjellander
 *
 ******************************************************!*/

 {
   char  mesbuf[V3STRLEN];
   char *pekarr[1000],strarr[20000];
   int   i,nstr,errant;

/*
***Skapa filförteckning. mbsdir/*.MBS
***igdir() returnerar filnamn utan extension.
*/
   igdir(mbsdir,MBSEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Kompilera.
*/
   errant = 0;

   for ( i=0; i<nstr; ++i )
     {
     if ( comp_2(mbsdir,pekarr[i],mbodir) )
       {
       msexla(FALSE);
       sprintf(mesbuf,"%s%s %s",pekarr[i],MBSEXT,iggtts(466));
       igwlma(mesbuf,IG_MESS);
       }
     else
       {
       msexla(TRUE);
     ++errant;
       }
     }
/*
***Töm PM på anropade moduler.
*/
   clheap();
/*
***Skriv ut hur många och antal fel.
*/
   if ( errant == 0 ) sprintf(mesbuf,"%d %s",nstr,iggtts(467));
   else               sprintf(mesbuf,"%d %s %d",nstr,iggtts(468),errant);
   igwlma(mesbuf,IG_MESS);
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

 static bool  mledit(rubrik,dir,namn,actmod)
        char *rubrik;
        char *dir;
        char *namn;
        bool  actmod;

/*      V3:s multiline modul-editor. 
 *
 *      In: rubrik = Önskad fönsterrubrik.
 *          dir    = Filkatalog där modilfilen finns avslutat med en "\".
 *          namn   = Namn med extension.
 *          actmod = TRUE => Aktiv modul
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int                 filsiz,dx,dy,ntkn,style;
   char               *buf1,*buf2;
   char                path[V3PTHLEN],titel[V3STRLEN];
   char                arkiv[V3STRLEN],spara[V3STRLEN],skriv[V3STRLEN],
                       edit[V3STRLEN],copy[V3STRLEN],paste[V3STRLEN],
                       cut[V3STRLEN],compile[V3STRLEN],find[V3STRLEN],
                       replace[V3STRLEN];
   int                 px,py,edx,edy;
   bool                horscr,verscr;
   wpw_id              id;
   WPEWIN             *ewinpt;
   HWND                dwin_id,edit_id;
   HDC                 dc;
   HFONT               old_font;
   TEXTMETRIC          tm;
   HMENU               menu,popup;
   SECURITY_ATTRIBUTES sa;
   HANDLE              fh;


/*
***Skapa ett ledigt fönster-ID.
*/
   if ( (id=mswffi()) < 0 ) return(FALSE);
/*
***Skapa ett WPEWIN.
*/
   if ( (ewinpt=(WPEWIN *)v3mall(sizeof(WPEWIN),"mledit"))
                                                   == NULL ) return(FALSE);
   ewinpt->id.w_id  = id;
   ewinpt->id.p_id  = NULL;
   ewinpt->id.ms_id = NULL;
   ewinpt->edit_id  = NULL;

   ewinpt->geo.x  =  0;
   ewinpt->geo.y  =  0;
   ewinpt->geo.dx =  0;
   ewinpt->geo.dy =  0;

   strcpy(ewinpt->dir,dir);
   strcpy(ewinpt->namn,namn);
   strcpy(path,dir);
   strcat(path,namn);
   strcpy(ewinpt->path,path);

   ewinpt->actmod = actmod;
/*
***Lagra fönstret i fönstertabellen.
*/
   wpwtab[id].typ = TYP_EWIN;
   wpwtab[id].ptr = (char *)ewinpt;
/*
***Ta reda på aktuell fontstorlek.
*/
   dc = GetDC(ms_main);
   old_font = SelectObject(dc,ms_lfont);
   GetTextMetrics(dc,&tm);
   SelectObject(dc,old_font);
   ReleaseDC(ms_main,dc);
/*
***Editfönstrets storlek i X-led. Editfönster har alltid vertikal 
***scrolllist till skillnad mot listfönster som inte kan växa.
*/
   dx = tm.tmMaxCharWidth*80 + 
        GetSystemMetrics(SM_CXVSCROLL) + 2*GetSystemMetrics(SM_CXFRAME);

   if ( dx > msmwsx() )
     {
     horscr = TRUE;
     dx = msmwsx();
     }
   else horscr = FALSE;
/*
***Och i Y-led.
*/
   verscr = TRUE;

   dy = 40 * (tm.tmHeight + tm.tmExternalLeading) + 
        2*GetSystemMetrics(SM_CYCAPTION) + 2*GetSystemMetrics(SM_CYFRAME);
   if ( horscr ) dy += GetSystemMetrics(SM_CYHSCROLL);

   if ( dy > msmwsy() ) dy = msmwsy();
/*
***EWIN-fönstrets meny.
*/
   if ( !msgrst("MENU.FILE",arkiv) ) strcpy(arkiv,"File");
   if ( !msgrst("MENU.SAVE",spara) ) strcpy(spara,"Save");
   if ( !msgrst("MENU.PRINT",skriv) ) strcpy(skriv,"Print");

   if ( !msgrst("MENU.EDIT",edit) ) strcpy(edit,"Edit");
   if ( !msgrst("MENU.PASTE",paste) ) strcpy(paste,"Paste");
   if ( !msgrst("MENU.CUT",cut) ) strcpy(cut,"Cut");
   if ( !msgrst("MENU.COPY",copy) ) strcpy(copy,"Copy");
   if ( !msgrst("MENU.FIND",find) ) strcpy(find,"Find");
   if ( !msgrst("MENU.REPLACE",replace) ) strcpy(replace,"Replace");

   if ( !msgrst("MENU.COMPILE",compile) ) strcpy(compile,"Compile");

   menu  = CreateMenu();

   popup = CreatePopupMenu();
   AppendMenu(menu,MF_STRING | MF_POPUP,(UINT)popup,arkiv);
   AppendMenu(popup,MF_STRING,V3_SAVE,spara);
   AppendMenu(popup,MF_STRING,V3_PRINT,skriv);

   popup = CreatePopupMenu();
   AppendMenu(menu,MF_STRING | MF_POPUP,(UINT)popup,edit);
   AppendMenu(popup,MF_STRING,V3_PASTE,paste);
   AppendMenu(popup,MF_STRING,V3_CUT,cut);
   AppendMenu(popup,MF_STRING,V3_COPY,copy);
   AppendMenu(popup,MF_SEPARATOR,NULL,NULL);
   AppendMenu(popup,MF_STRING,V3_FIND,find);
   AppendMenu(popup,MF_STRING,V3_REPLACE,replace);

   AppendMenu(menu,MF_STRING,V3_COMPILE,compile);
/*
***Skapa WIN32-fönster för WPEWIN.
*/
   strcpy(titel,rubrik);
   strcat(titel,namn);

   dwin_id = CreateWindow(VARKON_EWIN_CLASS,
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

   ewinpt->id.ms_id = dwin_id;
/*
***Hur stor blev applikationsarean. Så stor kan edit:en bli.
*/
   msggeo(dwin_id,&px,&py,&edx,&edy,NULL,NULL);
/*
***Sen ett WIN32-Editfönster i WPEWIN-fönstret.
*/
   style =  WS_CHILD | WS_VISIBLE | ES_MULTILINE;

   if ( horscr ) style = style | WS_HSCROLL;
   if ( verscr ) style = style | WS_VSCROLL;

   edit_id = CreateWindow("EDIT",
                          "",
                          style,
		      		      0,
		         	      0,
		  	      		  edx,
		  	      		  edy,
		  	      		  dwin_id,
		  	      		  NULL,
			      		  ms_inst,
			      		  NULL);

   ewinpt->edit_id = edit_id;
/*
***Om filen vid path finns läser vi in den till buf
***och skickar den vidare till edit-fönstret.
*/
   if ( v3ftst(path) ) 
     {
     sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
     sa.lpSecurityDescriptor = NULL;
     sa.bInheritHandle       = FALSE;

     fh = CreateFile(path,
                     GENERIC_READ,
                     0,
                    &sa,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);
     filsiz = GetFileSize(fh,NULL);
     if ( (buf1=v3mall(filsiz + 1,"mledit")) == NULL )
       {
       CloseHandle(fh);
       return(FALSE);
       }
     ReadFile(fh,buf1,filsiz,&ntkn,NULL);
     CloseHandle(fh);
    *(buf1+ntkn) = '\0';
     if ( addcr(buf1,&buf2) )
       {
       SendMessage(edit_id,WM_SETTEXT,0,(LPARAM)buf2);
       v3free(buf1,"mledit");
       v3free(buf2,"mledit");
       }
     else
       {
       CloseHandle(fh);
       return(FALSE);
       }
     }
/*
***Font och fokus.
*/
   SendMessage(edit_id,WM_SETFONT,(WPARAM)ms_lfont,MAKELPARAM(FALSE,0));
   SetFocus(edit_id);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!*******************************************************/

LRESULT CALLBACK mscbew(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)
       

/*      Callbackrutin för WPEWIN-fönster.
 *
 *      In: win32_id = Fönster som det hänt något i.
 *          message  = Vad som hänt.
 *          wp,lp    = Ytterligare data.
 *
 *      Ut: Inget.   
 *
 *      (C)microform ab 22/11/95 J. Kjellander
 *
 *      1996-12-24 Find, J.Kjellander
 *
 ******************************************************!*/

  {
   int     status,dx,dy;
   char    olddir[V3PTHLEN+1],mbofil[V3PTHLEN+1],
           namn[V3PTHLEN+1],sndpth[V3PTHLEN+1];
   WPWIN  *winptr;
   WPEWIN *ewinpt;


   switch ( message )
	   {
/*
***Kommer det ett WM_SIZE till WPLWIN-fönstret har det ändrat
***storlek och då skall även dess edit-fönster ändras.
*/
      case WM_SIZE:
      dx = LOWORD(lp);
      dy = HIWORD(lp);
      if ( (ewinpt=msgewp(win32_id)) != NULL  &&  ewinpt->edit_id != NULL )
        MoveWindow(ewinpt->edit_id,0,0,dx,dy,TRUE);
	   break;
/*
***WM_CLOSE innebär clickning på döda-knappen.
***msdlew() stryker inte WPLWIN-fönstret ur wpwtab så
***det skall vi göra här.
*/
      case WM_CLOSE:
      if ( (ewinpt=msgewp(win32_id)) != NULL )
        {
        if ( ewinpt->actmod )
          {
          status = IDYES;
          }
        else
          {
          if ( SendMessage(ewinpt->edit_id,EM_GETMODIFY,0,0) ==  TRUE )
            {
            status = MessageBox(ms_main,
                               "Vill du spara dina ändringar ?",
                                ewinpt->path,
                                MB_YESNOCANCEL | MB_ICONQUESTION);
            }
          else status = IDNO;

          if ( status == IDYES ) saveew(ewinpt);
          }

        if ( status != IDCANCEL )
          {
          if ( (winptr=mswgwp(ewinpt->id.w_id)) != NULL )
            {
            winptr->typ = TYP_UNDEF;
            winptr->ptr = NULL;
            }
          msdlew(ewinpt);
          DestroyWindow(win32_id);
          if ( !IsWindowEnabled(ms_main) ) EnableWindow(ms_main,TRUE);
          }
        }
      break;
/*
***WM_COMMAND innebär menyval.
*/
      case WM_COMMAND:
      if ( HIWORD(wp) == 0 )
        {
        switch ( LOWORD(wp) )
          {
/*
***Skriv ut WPEWIN-fönstret till sin fil.
*/
          case V3_SAVE:
          if ( (ewinpt=msgewp(win32_id)) != NULL )
            {
            if ( v3ftst(ewinpt->path) )
              {
              status = MessageBox(ms_main,
                                 "Modulen finns, vill du skriva över ?",
                                  ewinpt->path,
                                  MB_YESNO | MB_ICONQUESTION);
              if ( status == IDYES ) saveew(ewinpt);
              }
            else
              {
              saveew(ewinpt);
              }
            }
          break;
/*
***Kompilera.
*/
          case V3_COMPILE:
          if ( (ewinpt=msgewp(win32_id)) != NULL  &&  saveew(ewinpt) )
            {
/*
***Aktiv modul.
*/
            if ( ewinpt->actmod )
              {
              if ( comp_1(ewinpt) )
                {
                sprintf(sndpth,"%sComp_ok.wav",v3genv(VARKON_SND));
                sndPlaySound(sndpth,SND_SYNC);
                msexla(FALSE);
                strcpy(olddir,jobdir);
                strcpy(jobdir,ewinpt->dir);
                igldmo();
                strcpy(jobdir,olddir);
                v3fdel(ewinpt->path);
                strcpy(mbofil,v3genv(VARKON_TMP));
                strcat(mbofil,jobnam);
                strcat(mbofil,MODEXT);
                v3fdel(mbofil);
                }
              else
                {
                if ( MessageBox(ms_main,"View errors ?",
                                "Compile error !",
                                MB_YESNO | MB_ICONSTOP) == IDYES )
                  msexla(TRUE);
                else
                  msexla(FALSE);
                }
              }
/*
***Annan modul. Kompilera med comp_2 som vill ha namnet utan extension.
*/
            else
              {
              strcpy(namn,ewinpt->namn);
              if ( strlen(namn) > 4 ) namn[strlen(namn)-4] = '\0';
              if ( comp_2(mbsdir,namn,mbodir) )
                {
                sprintf(sndpth,"%sComp_ok.wav",v3genv(VARKON_SND));
                sndPlaySound(sndpth,SND_SYNC);
                msexla(FALSE);
                clheap();
                }
              else
                {
                if ( MessageBox(ms_main,"View errors ?",
                                "Compile Error !",
                                MB_YESNO | MB_ICONSTOP) == IDYES )
                  msexla(TRUE);
                else
                  msexla(FALSE);
                }
              }
            }
          break;
/*
***Skriv ut på skrivaren.
*/
          case V3_PRINT:
          if ( (ewinpt=msgewp(win32_id)) != NULL )
            msprtf(ewinpt->path,ewinpt->path);
          break;
/*
***Klipp bort selekterad text till clipboard..
*/
          case V3_CUT:
          if ( (ewinpt=msgewp(win32_id)) != NULL )
            SendMessage(ewinpt->edit_id,WM_CUT,0,0);
          break;
/*
***Kopiera selekterad text till clipboard..
*/
          case V3_COPY:
          if ( (ewinpt=msgewp(win32_id)) != NULL )
            SendMessage(ewinpt->edit_id,WM_COPY,0,0);
          break;
/*
***Klistra in clipboard till markör.
*/
          case V3_PASTE:
          if ( (ewinpt=msgewp(win32_id)) != NULL )
            SendMessage(ewinpt->edit_id,WM_PASTE,0,0);
          break;
/*
***Sök, poppa upp sök-dialogen.
*/
          case V3_FIND:
          mlfind(win32_id);
          break;
          }
        }
      break;
/*
***Kan det vara ett meddelande från sök/ersätt-dialogen ?
*/
	   default:
       if ( message == RegisterWindowMessage("commdlg_FindReplace") )
         {
         if ( !fndmes(win32_id,message,wp,lp) )
	       return(DefWindowProc(win32_id,message,wp,lp));
         }
/*
***Meddelanden som vi inte bryr oss om.
*/
	   else return(DefWindowProc(win32_id,message,wp,lp));
	   }

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static bool mlfind(win32_id)
        HWND win32_id;

/*      Skapar och poppar upp sök-dialog.
 *
 *      In: win32_id = Edit-fönstrets id.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Ok.
 *          FALSE = Kan ej skapa sökdialog.
 *
 *      (C)microform ab 1996-12-24 J. Kjellander
 *
 ******************************************************!*/

  {
   HWND    fnd_id;
   WPEWIN *ewinpt;

static FINDREPLACE fr;
static char fndstr[V3STRLEN] = "";

/*
***Ut med dialogboxen.
*/
   fr.lStructSize      = sizeof(FINDREPLACE);
   fr.hwndOwner        = win32_id;
   fr.hInstance        = NULL;
   fr.Flags            = FR_DOWN | FR_HIDEWHOLEWORD;
   fr.lpstrFindWhat    = fndstr;
   fr.lpstrReplaceWith = NULL;
   fr.wFindWhatLen     = V3STRLEN;
   fr.wReplaceWithLen  = 0;
   fr.lCustData        = 0;
   fr.lpfnHook         = NULL;
   fr.lpTemplateName   = NULL;
   fnd_id = FindText(&fr);
/*
***Sök från början av texten.
*/
   if ( (ewinpt=msgewp(win32_id)) != NULL )
     SendMessage(ewinpt->edit_id,EM_SETSEL,(WPARAM)-1,(LPARAM)0);
/*
***Registrera dialogboxen som aktiv.
*/
   if ( fnd_id != NULL )
     {
     mssacd(fnd_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

 static bool fndmes(HWND win32_id,UINT message,WPARAM wp,LPARAM lp)

/*      Processar meddelanden från sökdialogboxen.
 *
 *      In: win32_id = Edit-fönstrets id.
 *          message  = Meddelande.
 *          wp       = ???
 *          lp       = Pekare till FINDREPLACE struct.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Meddelandet processat.
 *          FALSE = Meddelandet ej processat.
 *
 *      (C)microform ab 1996-12-24 J. Kjellander
 *
 ******************************************************!*/

  {
  int          i,ntkn,start,stop,inc;
  char         notfnd[V3STRLEN];
  char        *fndstr,*buf;
  bool         exact;
  WPEWIN      *ewinpt;
  FINDREPLACE *pfr;

/*
***Vilket fönster ?
*/
   if ( (ewinpt=msgewp(win32_id)) == NULL ) return(FALSE);
/*
***Fixa en pekare till FINDREPLACE.
*/
   pfr = (FINDREPLACE *)lp;
/*
***Gäller det sökning tro ? Sök från aktuell cursorposition.
*/
   if ( pfr->Flags & FR_FINDNEXT )
     {
     ntkn = GetWindowTextLength(ewinpt->edit_id);
     SendMessage(ewinpt->edit_id,EM_GETSEL,(WPARAM)&start,(LPARAM)&stop);

     if ( pfr->Flags & FR_DOWN )
       {
       if ( start == 0  &&  stop == 0 ) start = 0;
       else                             start = stop;
       stop = ntkn;
       inc = 1;
       }
     else
       {
       if ( start == 0  &&  stop == 0 ) start = ntkn - 1;
       else if ( start > 0 )            start = start - 1;
       stop = -1;
       inc  = -1;
       }

     fndstr = pfr->lpstrFindWhat;
/*
***Exakt eller normal sökning ?
*/
   if ( pfr->Flags & FR_MATCHCASE ) exact = TRUE;
   else                             exact = FALSE;
/*
***Kopiera edit-fönstrets inehåll till lokal buffert.
*/
     if ( (buf=v3mall(ntkn+1,"fndmes")) == NULL ) return(FALSE);
     GetWindowText(ewinpt->edit_id,buf,ntkn+1);
/*
***Sök.
*/
     for ( i = start; i != stop; i += inc )
       {
       if ( fndcmp(buf+i,fndstr,1,exact)  &&
            strlen(fndstr) <= (unsigned)(ntkn-i+2)  &&
            fndcmp(buf+i,fndstr,strlen(fndstr),exact) ) break;
       }
/*
***Träff.
*/
     if ( i != stop )
       {
       SendMessage(ewinpt->edit_id,EM_SETSEL,(WPARAM)i,(LPARAM)(i+strlen(fndstr)));
       SendMessage(ewinpt->edit_id,EM_SCROLLCARET,(WPARAM)0,(LPARAM)0);
       SetFocus(ewinpt->edit_id);
       }
/*
***Miss.
*/
     else
       {
       if ( !msgrst("ACTMEDIT.NOTFND",notfnd) ) strcpy(notfnd,"Hittar ej den sökta texten !");
       MessageBox(ms_main,notfnd,"VARKON",MB_OK | MB_ICONINFORMATION);
       }
/*
***Avallokera minne.
*/
     v3free(buf,"fndmes");
/*
***Slut.
*/
     return(TRUE);
     }
/*
***Om det gäller avsluta låter vi systemet ta hand om det.
*/
   else if ( pfr->Flags & FR_DIALOGTERM )
     {
     mssacd(NULL);
     SetFocus(ewinpt->edit_id);
     return(FALSE);
     }
/*
***Övriga (borde i och för sig inte finnas några).
*/
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

 static bool  fndcmp(str1,str2,ntkn,exact)
        char *str1;
        char *str2;
        int   ntkn;
        bool  exact;

/*      Jämför ntkn tecken med eller utan hänsyn till
 *      stora och små bokstäver (case).
 *
 *      In: str1  = Pekare till sträng 1
 *          str2  = Pekare till sträng 2
 *          ntkn  = Antal tecken att jämföra
 *          exact = TRUE => Ta hänsysn till case
 *
 *      Ut: TRUE  = Lika.
 *          FALSE = Olika.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1996-12-26 J. Kjellander
 *
 ******************************************************!*/

  {
    int  i;
    char c1,c2;


    if ( exact )
      {
      if ( strncmp(str1,str2,ntkn) == 0 ) return(TRUE);
      else return(FALSE);
      }
    else
      {
      for ( i=0; i<ntkn; ++i )
        {
        c1 = *(str1+i);
        c2 = *(str2+i);
        if ( isilower(c1) ) c1 = toiupper(c1);
        if ( isilower(c2) ) c2 = toiupper(c2);
        if ( c1 != c2 ) return(FALSE);        
        }
      return(TRUE);
      }
  }

/********************************************************/
/*!******************************************************/

 static bool    comp_1(ewinpt)
        WPEWIN *ewinpt;

/*      Kompilerar det MBS-program som svarar mot ett
 *      WPEDIT-fönster.
 *
 *      In: ewinpt = C-Pekare till aktuellt fönster.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 20/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   return(comp_file(ewinpt->dir,ewinpt->namn));
  }

/********************************************************/
/*!******************************************************/

 static bool  comp_2(dir,namn,utdir)
        char *dir;
        char *namn;
        char *utdir;

/*      Kompilerar MBS-program på viss katalog och placerar
 *      resultatet på en annan katalog.
 *
 *      In: dir    = Filkatalog där MBS-filen finns avslutat med en "\".
 *          namn   = Namn utan extension.
 *          utdir  = Filkatalog där man vill ha MBO-filen eller NULL
 *                   om samma som MBS. "\" sist.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1996-02-27 J. Kjellander
 *
 ******************************************************!*/

  {
   char  from[V3PTHLEN+1],to[V3PTHLEN+1];
   bool  compok;

/*
***Kompilera.
*/
   compok = comp_file(dir,namn);
/*
***Ev. flyttning av MBO-fil.
*/
   if ( compok  &&  (utdir != NULL)  &&  (strcmp(dir,utdir) != 0 ) )
     {
     strcpy(from,dir);
     strcat(from,namn);
     strcat(from,MODEXT);
     strcpy(to,utdir);
     strcat(to,namn);
     strcat(to,MODEXT);
     if ( v3fmov(from,to) < 0 )
       {
       msexla(FALSE);
       errmes();
       }
     }
/*
***Slut.
*/
   return(compok);
  }

/********************************************************/
/*!******************************************************/

 static bool  comp_file(dir,fil)
        char *dir,*fil;

/*      Kompilerar ett MBS-program.
 *      Används av comp_1 och comp_2.
 *
 *      In: dir = Path avslutad med "\".
 *          fil = MBS-filens namn med eller utan extension.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Kompileringen gick bra.
 *          FALSE = Fel vid kompilering.
 *
 *      (C)microform ab 1996-02-27 J. Kjellander
 *
 *      1997-01-15 gtenv3(), J.Kjellander
 *      1997-05-21 NT, J.Kjellander
 *      2004-08-04, MBSC.EXE, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    int                 radsiz,errcode;
    bool                compok,readok;
    char                cmd[256],buf[512],tkn;
    DWORD               ntkn;
    HANDLE              read,write;
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

    extern V3MSIZ       sysize;

/*
***Att sätta om stdout för en subprocess och läsa därifrån
***med en pipe är beskrivet i on-line dokumentationen under
***Using Processes and Threads i kap 43.
*/

/*
***Skapa anonym pipe för MBSC:s stdout.
*/
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = TRUE;
   sa.lpSecurityDescriptor = NULL;

   if ( !CreatePipe(&read,&write,&sa,0) ) return(FALSE);
/*
***Initiera startupinfo.
*/
   memset(&si,0,sizeof(STARTUPINFO));
   si.cb          = sizeof(STARTUPINFO);
   si.lpReserved  = NULL;
   si.lpDesktop   = NULL;
   si.lpTitle     = NULL;
   si.dwFlags     = STARTF_USESTDHANDLES;
   si.cbReserved2 = 0;
   si.lpReserved2 = NULL;
   si.hStdInput   = NULL;
   si.hStdOutput  = write;
   si.hStdError   = NULL;
/*
***Starta subprocess. Ge mbsc lika stort PM som vi har nu.
*/
   sprintf(cmd,"%s\\BIN\\MBSC.EXE -pm%d %s",gtenv3("VARKON_ROOT"),sysize.pm,fil);

   if ( !CreateProcess(NULL,
                       cmd,
                       NULL,
                       NULL,
                       TRUE,
                       DETACHED_PROCESS,          /* Inget DOS-fönster */
                       NULL,
                       dir,                       /* Default katalog för mbsc */
                      &si,
                      &pi) )
      {
      CloseHandle(read);
      CloseHandle(write);
      return(FALSE);
      }
/*
***Initiera listfönster.
*/
   strcpy(buf,"Kompilering av ");
   strcat(buf,dir);
   strcat(buf,fil);
   msinla(buf);
/*
***Write-sidan av pipen skall stängas innan man läser står det.
*/
   CloseHandle(write);

   radsiz = 0;
   compok = FALSE;

   readok = ReadFile(read,&tkn,1,&ntkn,NULL);

   if ( !readok  ||  ntkn == 0 )
     {
	 errcode = GetLastError();
	 errcode = ERROR_BROKEN_PIPE;
	 }

   while ( !compok  &&  readok  &&  ntkn > 0 )
     {
     switch ( tkn )
       {
       case '\r':
       break;

       case '\n':
       if ( radsiz > 0 )
         {
         if ( strcmp(buf," No compiler detected errors") == 0 ) compok = TRUE;
         msalla(buf,(short)1);
         }
       radsiz = 0;
       break;

       default:
       if ( radsiz < 80 )
         {
         buf[radsiz] = tkn;
       ++radsiz;
         buf[radsiz] = '\0';
         }
       else
         {
         msalla(buf,(short)1);
         radsiz = 0;
         }
       break;
       }
     readok = ReadFile(read,&tkn,1,&ntkn,NULL);
     }
/*
***Färdigt. Nu skall läs-sidan av pipen stängas samt subprocessen
***och dess primary thread.
*/
   CloseHandle(read);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);
/*
***Slut.
*/
   return(compok);
  }

/********************************************************/
/*!******************************************************/

 static bool  getprg(rubrik,mbsdir,mbsnam)
        char *rubrik;
        char *mbsdir;
        char *mbsnam;

/*      Frågar efter ett MBS-program.
 *
 *      In: rubrik = Dialogboxens rubriktext.
 * 
 *      Ut: mbsnam = Bara filnamnet.
 *          mbspth = Bara filkatalogen med avslutande "\".
 *
 *      FV: 0.
 *
 *      (C)microform ab 20/11/95 J. Kjellander
 *
 ******************************************************!*/

 {
   char path[V3PTHLEN];

   static OPENFILENAME ofn;
   static char         defdir[V3PTHLEN] = "";
   static char         defnam[V3PTHLEN] = "";

/*
***Default namn och filkatalog.
*/
   if ( defnam[0] == '\0' )
     {
     strcpy(defdir,jobdir);
     strcpy(defnam,jobnam);
     strcat(defnam,MBSEXT);
     }

   strcpy(path,defnam);
/*
***Initiera ofn.
*/
   ofn.lStructSize       = sizeof(OPENFILENAME);
   ofn.hwndOwner         = ms_main;
   ofn.lpstrFilter       = "MBS-filer(*.MBS)\0*.mbs\0";
   ofn.nFilterIndex      = 1;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter    = 0;
   ofn.lpstrFile         = path;                      /* Namn med path */
   ofn.nMaxFile          = sizeof(path);
   ofn.lpstrFileTitle    = defnam;                    /* Namn utan path */
   ofn.nMaxFileTitle     = sizeof(defnam);
   ofn.lpstrInitialDir   = defdir;                    /* Default path */
   ofn.lpstrDefExt       = "MBS";
   ofn.lpstrTitle        = rubrik;
   ofn.Flags             = OFN_PATHMUSTEXIST      |
                           OFN_HIDEREADONLY       |
                           OFN_NONETWORKBUTTON    |
                           OFN_NOREADONLYRETURN   |
                           OFN_EXTENSIONDIFFERENT |
                           OFN_ALLOWMULTISELECT;
                           

   if ( GetOpenFileName(&ofn) )
     {
     strcpy(&path[ofn.nFileExtension],"MBS");    /* mbsc vill ha ".MBS" med stora boksäver */
     strcpy(defdir,path);
     defdir[ofn.nFileOffset] = '\0';
     strcpy(mbsdir,defdir);
     strcpy(mbsnam,&path[ofn.nFileOffset]);
     return(TRUE);
     }
   else return(FALSE);
 }

/********************************************************/
/*!******************************************************/

 static bool saveew(ewinpt)
        WPEWIN *ewinpt;

/*      Spara edit-fönster.
 *
 *      In: ewinpt = C-pekare till WPEWIN.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 22/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
   int   rad,radant,ntkn,tknpos;
   char  buf[V3STRLEN + 1];
   WORD *wptbuf = (WORD *)buf;
   FILE *fp;

/*
***Öppna fil.
*/
   if ( (fp=fopen(ewinpt->path,"w")) == NULL )
     {
     MessageBox(ms_main,ewinpt->path,
               "Systemfel, kan ej öppna fil",MB_OK | MB_ICONSTOP);
     return(FALSE);
     }
/*
***Hur många rader är det fråga om ?
*/
   radant = SendMessage(ewinpt->edit_id,EM_GETLINECOUNT,(WPARAM)0,(LPARAM)0);
/*
***Läs raderna en i taget och skriv ut till filen.
***Kolla att ingen rad är för lång.
*/
   for ( rad=0; rad<radant; ++rad )
     {
     tknpos = SendMessage(ewinpt->edit_id,EM_LINEINDEX,(WPARAM)rad,(LPARAM)0);
     ntkn   = SendMessage(ewinpt->edit_id,EM_LINELENGTH,(WPARAM)tknpos,(LPARAM)0);

     if ( ntkn > V3STRLEN )
       {
       msmbox("Filen innehåller en eller flera rader som är > 132 tecken !",
              "Fel vid lagring av MBS-fil !",0);
       goto end;
       }

    *wptbuf = V3STRLEN;
     if ( SendMessage(ewinpt->edit_id,EM_GETLINE,(WPARAM)rad,(LPARAM)buf) > 0 )
       {
       buf[ntkn] = '\0';
       fprintf(fp,"%s\n",buf);
       }
     else fprintf(fp,"\n");
     }
/*
***Stäng filen igen.
*/
end:
   fclose(fp);
/*
***Slut.
*/
   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        int     msdlew(ewinpt)
        WPEWIN *ewinpt;

/*      Dödar ett WPEWIN.
 *
 *      In: ewinptr = C-pekare till WPEWIN.
 *
 *      Ut: Inget.   
 *
 *      Felkod: .
 *
 *      (C)microform ab 23/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    v3free((char *)ewinpt,"msdlew");

    return(0);
  }

/********************************************************/
/*!******************************************************/

        WPEWIN *msgewp(win32_id)
        HWND   win32_id;

/*      Letar upp multiline-editfönstret med angiven
 *      MS-id och returnerar en C-pekare till fönstret.
 *
 *      In: win32_id  = Fönstrets MS-id.
 *
 *      Ut: Inget.   
 *
 *      FV: C-adress eller NULL om fönster saknas.
 *
 *      (C)microform ab 23/11/95 J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    WPEWIN  *ewinpt;

/*
***Leta upp fönstret.
*/
    for ( i=0; i<WTABSIZ; ++i)
      {
      if ( wpwtab[i].ptr != NULL )
        {
        if ( wpwtab[i].typ == TYP_EWIN )
          {
          ewinpt = (WPEWIN *)wpwtab[i].ptr;
          if ( ewinpt->id.ms_id == win32_id ) return(ewinpt);
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

 static bool  addcr(src,dstptr)
        char *src;
        char **dstptr;

/*      Kopierar tecknen i src till *dstptr och konverterar
 *      samtidigt LF till CR+LF.
 *
 *      In: src    = Pekare till ursprunglig text.
 *          dstptr = Pekare till utdata.
 *
 *      Ut: *dstptr = Pekare till ny teckenbuffert.
 *
 *      FV: TRUE  = OK
 *          FALSE = Kan ej allokera minne för ny buffert.
 *
 *      (C)microform ab 1996-12-06 J. Kjellander
 *
 ******************************************************!*/

  {
   int   i,j,ntkn,ncr;
   char *tmpbuf;

/*
***Det måste vara minst ett tecken.
*/
   if ( *src == '\0' ) return(FALSE);
/*
***Räkna hur många tecken det är och hur många CR det saknas.
*/
   ntkn = 1;

   ncr = 0; if ( src[0] == '\n' ) ncr = 1;

   while ( src[ntkn] != '\0' )
     {
     if ( src[ntkn] == '\n'  &&  src[ntkn-1] != '\r' ) ++ncr;
     ++ntkn;
     }
/*
***Allokera ny buffert. ntkn tecken + NULL + ev. nya CR.
*/
   if ( (tmpbuf=v3mall(ntkn + 1 + ncr,"addcr")) == NULL ) return(FALSE);
/*
***Kopiera från src till tmpbuf.
*/
   j = 0;

   if ( src[0] == '\n' ) tmpbuf[j++] = '\r';
   tmpbuf[j++] = src[0];

   for ( i=1; i<ntkn; ++i )
     {
     if ( src[i] == '\n'  &&  src[i-1] != '\r') tmpbuf[j++] = '\r';
     tmpbuf[j++] = src[i];
     }

   tmpbuf[j] = '\0';

  *dstptr = tmpbuf;
/*	     
***Slut.
*/
   return(TRUE);
  }

/********************************************************/
