/**********************************************************************
*
*    ms6.c
*    =====
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msgtch();   Input one event
*     mssacd();   Set active dialogue box
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

#define INBSIZ  256        /* Storlek på teckenbuffert */

static HWND actdlg = NULL;

/* actdlg är win32_id för en aktiv dialogbox, tex. sök/ersätt */

extern MNUALT   smbind[];
extern MNUALT  *ms_cmdtab[];

/*!*******************************************************/

     char msgtch(altptr,alttyp,flag)
     MNUALT **altptr;
     short   *alttyp;
     bool     flag;

/*   Läser ett tecken från tangentbordet och pollar
 *   samtidigt event-kön. Om event finns servas de.
 *   Denna event-rutin används bara av V3 själv och
 *   då bara när en meny är aktiv och V3 väntar på
 *   svar. 
 *
 *   In: altptr = Pekare till menyalternativ-pekare.
 *       alttyp = Pekare till typ av alternativ/symbol.
 *       flag   = TRUE  => Alla typer av symboler tillåtna.
 *                FALSE => Bara tecken tillåtna.
 *
 *   Ut: **altptr = Pekare till alternativ.
 *        *alttyp = Typ av alternativ.
 *
 *   FV: Det lästa tecknet och/eller symbol.
 *
 *   (C)microform ab 1/11/95 J. Kjellander
 *
 *   1996-12-09 BN_CLICKED, J.Kjellander
 *   1996-12-26 mssacd(), J.Kjellander
 *   2000-03-05 LEFT_BUTTON, J.Kjellander
 *
 *******************************************************!*/

 {
    int           i,cmd_id,actnum;
    char          c,tknbuf[INBSIZ],butstr[V3STRLEN];
    wpw_id        dum_id;
    static int    ntkn = 0;
    MSG           message;

/*
***Om tecken redan finns i tknbuf, returnera direkt utan att
***vänta på nytt event.
*/
    if ( ntkn > 0 )
      {
      c = tknbuf[0];
      if ( c == 13 ) c = '\n';
      --ntkn;
      for ( i=0; i<ntkn; ++i ) tknbuf[i] = tknbuf[i+1];
     *alttyp = SMBCHAR;
      return(c);
      }
/*
***Om meddelanden finns, serva dom. Om inga medelanden finns och ej
***heller några tecken buffrade i tknbuf lägger vi oss och väntar.
*/
    c = '\0';
   *alttyp = SMBNONE;

loop:
   if ( GetMessage(&message,NULL,0,0) == TRUE )
     {
/*
***Om en dialogbox är aktiv kan meddelandet vara avsett
***för denna.
*/
     if ( actdlg != NULL  &&  IsDialogMessage(actdlg,&message) ) goto loop;
/*
***Om ett meddelande fanns att hämta skickar vi det vidare
***till fönstrets callback-rutin. Vissa meddelanden servas där
***men några lämnas kvar till längre ner.
*/
     TranslateMessage(&message);    /* Translate virtual keycodes */
                                    /* to ASCII */
     DispatchMessage(&message);	    /* Tillbaks till windows igen */
	                                /* och sen till callback-rutinen */

     switch ( ms_lstmes.msg )
	   {
/*
***Vänster musknapp.
*/
       case WM_LBUTTONDOWN:
       if ( msifae(ms_lstmes.wh,altptr) )
         {
         *alttyp = SMBALT;
         return(0);
         }
/*
       else if ( mswbut(&ms_lstmes,&dum_id) == FALSE )
         {
         *alttyp = SMBRETURN;
         return(0);
         }
*/
       else if ( mswbut(&ms_lstmes,&dum_id) == TRUE )
         {
         break;
         }
       else if ( msggwp(ms_lstmes.wh) != NULL  &&  
                 msgrst("MOUSE.LEFT_BUTTON",butstr)  &&
                 sscanf(&butstr[1],"%d",&actnum) == 1 )
         {
         switch ( *butstr )
           {
           case 'f':
           igdofu((short)FUNC,(short)actnum);
           break;

           case 'M':
           igdofu((short)MFUNC,(short)actnum);
           break;
           }
         }
       break;
/*
***Höger musknapp.
*/
       case WM_RBUTTONDOWN:
       if ( msgrst("MOUSE.RIGHT_BUTTON",butstr)  &&
            sscanf(&butstr[1],"%d",&actnum) == 1 )
         {
         switch ( *butstr )
           {
           case 'f':
           igdofu((short)FUNC,(short)actnum);
           break;

           case 'M':
           igdofu((short)MFUNC,(short)actnum);
           break;
           }
         }
      *alttyp = SMBRETURN;
       return(0);
/*
***Ett WM_COMMAND från huvudfönstret med HIWORD(wp) == 0 är ett menyval.
*/
       case WM_COMMAND:
       if ( ms_lstmes.wh == ms_main  &&  HIWORD(ms_lstmes.wp) == 0 )
         {
         cmd_id = LOWORD(ms_lstmes.wp);
         if ( cmd_id > 999  &&  cmd_id < MALMAX + 1000 )
           {
          *altptr = ms_cmdtab[cmd_id - 1000];
	       *alttyp = SMBALT;
           return(0);
           }
         }
/*
***Om HIWORD(wp) == BN_CLICKED är det en pekning i en knapp med
***MS-Klass "BUTTON".
*/
       else if ( HIWORD(ms_lstmes.wp) == BN_CLICKED )
         {
         if ( msifae((HWND)ms_lstmes.lp,altptr) )
           {
           *alttyp = SMBALT;
           return(0);
           }
         }
       break;
/*
***Okänt meddelande.
*/
       default:
       break;
       }
     goto loop;
	  }
   else
     {
     ExitProcess(message.wParam);
     }

   return(c);
 }

/*********************************************************/
/*!*******************************************************/

     int mssacd(win32_id)
     HWND win32_id;

/*   Registrerar en viss dialogbox som aktiv. Meddelande-
 *   loopen i msgtch() kollar om dialogbox är aktiv och
 *   anropar isåfall IsDialogMessage() för att TAB, CR etc.
 *   skall funka.
 *
 *   In: win32_id = Fönster ID eller NULL.
 *
 *   Ut: Inget.
 *
 *   (C)microform ab 1996-12-26 J. Kjellander
 *
 *******************************************************!*/

 {
  actdlg = win32_id;

  return(0);
 }

/*********************************************************/
