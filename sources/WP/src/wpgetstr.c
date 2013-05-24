/**********************************************************************
*
*    wpgetstr.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPmsip();   Input multiple strings
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

typedef struct
{
short   scr_dx;            /* Sk�rmens storlek i X-led */
short   scr_dy;            /* Sk�rmens storlek i Y-led */
DBint   iwin_id;           /* Huvudf�nstrets ID */
WPIWIN *iwinpt;            /* C-pekare till huvudf�nster */
short   iwin_dx,iwin_dy;   /* Huvudf�nstrets storlek */
short   tdx,tdy;           /* Huvudpromtens storlek */
short   lx,ly,lm;          /* Luft i olika riktningar */
short   ih;                /* Inmatningsf�ltens h�jd */
short   ah;                /* Alternativrutors h�jd */
DBint   ok_id;             /* Okey-knappens ID */
short   ok_x,ok_y;         /* Okey-knappens placering */
short   ok_dx,ok_dy;       /* Okey-knappens storlek */
DBint   av_id;             /* Avbryt-knappens ID */
short   av_x,av_y;         /* Avbryt-knappens placering */
short   av_dx,av_dy;       /* Avbryt-knappens storlek */
DBint   hlp_id;            /* Hj�lp-knappens ID */
short   hlp_x,hlp_y;       /* Hj�lp-knappens placering */
short   hlp_dx,hlp_dy;     /* Hj�lp-knappens storlek */
DBint   nxt_id;            /* Bl�ddra-knappens ID */
DBint   ed_id[V2MPMX];     /* Inmatningsf�ltens ID */
DBint   pmt_id[V2MPMX];    /* Promtars ID */
DBint   but_id[V2MPMX];    /* Pos-knappars ID */
short   max_ed;            /* Max antal inmatningsf�lt som f�r plats */
short   start_ed;          /* 1:a inmatnigsf�lt p� aktiv sida */
short   ant_ed;            /* Aktivt antal inmatningsf�lt */
short   actpag,pagant;     /* Aktiv sida och antal sidor */
} MSIDAT;

/*
***MSIDAT �r en struct som h�ller alla n�dv�ndiga data om
***en viss inmatning. WPmsip() har en s�dan som lokal variabel
***och kommunicerar med sina underordnade rutiner via en pekare
***till denna. D�rmed �r WPmsip() reentrant.
*/

static short iwin_x = 300;
static short iwin_y = 250;
 
/*
***iwin_x och iwin_y �r huvudf�nstrets l�ge p� sk�rmen.
***Om anv�ndaren under en inmatning flyttar f�nstret uppdateras
***dessa s� att f�nstret n�sta g�ng kommer p� det nya st�llet.
*/

static char title   [81];        /* varkon.input.title */
static char page    [81];        /* varkon.input.page */
static char okey    [81];        /* varkon.input.okey */
static char okeytt  [81];        /* varkon.input.okey.tooltip */
static char reject  [81];        /* varkon.input.reject */
static char rejecttt[81];        /* varkon.input.reject.tooltip */
static char next    [81];        /* varkon.input.next */
static char help    [81];        /* varkon.input.help */
static char helptt  [81];        /* varkon.input.help.tooltip */

extern short    modtyp;

static short crewin(MSIDAT *md, char *dps, char *ips[], short nstr);
static short crepag(MSIDAT *md, char *ips[], char *is[], short maxtkn[],
                    int typarr[], short nstr);

/*!*******************************************************/

     short WPmsip(
     char *dps,
     char *ips[],
     char *ds[],
     char *is[],
     short maxtkn[],
     int   typarr[],
     short nstr)

/*   Multiple string input. Formul�r-
 *   orienterad inl�sning av en eller flera str�ngar.
 *   Om antalet st�ngar �r st�rre �n vad som f�r plats
 *   p� sk�rmen skapas flera sidor.
 *
 *   In: dps    = Dialogboxens Promptstr�ng
 *       ips    = Inmatningsf�ltens Promtstr�ngar
 *       ds     = Default-str�ngar
 *       is     = Pekare till resultat.
 *       maxtkn = Max l�ngder p� inmatat resultat.
 *       typarr = Inmatningsf�ltens typer.
 *       nstr   = Antal str�ngar.
 *
 *   Ut: *is = nstr stycken textstr�ngar.
 *
 *   FV:      0  = Ok.
 *       REJECT  = Inmatningen avbruten.
 *       SMBPOSM = Pos-button in menu window pressed.
 *
 *   (C)microform ab 17/8/92 J. Kjellander
 *
 *   25/1/94  Omarbetad, J. Kjellander
 *   4/11/94  Resurser f�r texter, J. Kjellander
 *   1996-12-12 typarr, J.Kjellander
 *   1997-01-31 Bug avslut med ett f�lt, J.Kjellander
 *   1998-10-04 Nollst�llning av nxt_id, J.Kjellander
 *   1999-03-01 Bug promt+typ vid bl�ddra, J.Kjellander
 *   2007-03-25 1.19 J.Kjellander
 *
 *******************************************************!*/

 {
    MSIDAT  md;    /* Lokala data som delas med andra rutiner */

    char     tmpbuf[V3STRLEN+1];
    short    status=0,i,j;
    DBint    wait_id;
    int      wm_x1,wm_x2,wm_y1,wm_y2;
    pm_ptr   expr;
    DBetype  typ;
    bool     right,end;
    WPEDIT  *edtptr;
    XEvent   event;

/*
***F�rst en liten koll s� att det inte �r f�r m�nga texter.
***Max antal = Max antal parametrar i ett part-anrop eftersom
***det �r det som denna rutin oftast anv�nds till.
*/
   if ( nstr > V2MPMX ) nstr = V2MPMX;
/*
***Vilka texter skall vi ha i f�nsterram och p� knappar ?
*/
    if ( !WPgrst("varkon.input.title",title) )             strcpy(title,"Input");
    if ( !WPgrst("varkon.input.page",page) )               strcpy(page,"Page");
    if ( !WPgrst("varkon.input.okey",okey) )               strcpy(okey,"Okey");
    if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )     strcpy(okeytt,"");
    if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Reject");
    if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
    if ( !WPgrst("varkon.input.next",next) )               strcpy(next,"Next");
    if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Help");
    if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");
/*
***Nu skapar vi sj�lva huvudf�nstret.
*/
    crewin(&md,dps,ips,nstr);
/*
***Vart hamnade f�nstret ? Window Managern har antagligen
***flyttat lite p� det f�r att ge plats �t sin ram.  F�r
***att vara s�kra p� att WM har tagit hand om f�nstret
***och allt �r stabilt v�ntar vi med att kolla l�get tills
***vi f�tt v�rt 1:a expose-event i f�nstret. Detta �r antagligen
***detsamma som att v�nta p� MapNotify.
*/
    XWindowEvent(xdisp,md.iwinpt->id.x_id,ExposureMask,&event);
    XPutBackEvent(xdisp,&event);
    WPgtwp(md.iwinpt->id.x_id,&wm_x1,&wm_y1);
/*
***F�r att alla ut-str�ngar s�kert skall f� v�rden
***kopierar vi till att b�rja med default-str�ngarna
***till dem.
*/
    for ( i=0; i<nstr; ++i ) strcpy(is[i],ds[i]);
/*
***Ut med inmatningsf�lten. Vi b�rjar med sida 1.
*/
    md.actpag = 1;

loop1:
     crepag(&md,ips,is,maxtkn,typarr,nstr);
/*
***V�nta p� klickning i knapp eller h�ndelse i edit-f�nster...
*/
loop2:
    status = WPwwtw(md.iwin_id,SLEVEL_V3_INP,&wait_id);
/*
***...eller klickning i pos-knapp i menyf�nstret.
*/
    if ( status == SMBPOSM ) goto loop2;
/*
***Klickning i OK-knapp. Med bara ett inmatningsf�lt och
***inget svar (tomt) betyder detta REJECT. Annars OK.
*/
    else if ( wait_id == md.ok_id )
      {
      if ( nstr == 1 )
        {
        edtptr = (WPEDIT *)md.iwinpt->wintab[(wpw_id)md.ed_id[0]].ptr;
        if ( edtptr->str[0] == '\0' ) status = REJECT;
        else                          status = 0;
        }
      else status = 0;
      }
/*
***Klickning i Avbryt-knapp.
*/
    else if ( wait_id == md.av_id ) status = REJECT;
/*
***Klickning i Hj�lp-knapp.
*/
    else if ( wait_id == md.hlp_id )
      {
      IGhelp();
      goto loop2;
      }
/*
***Klickning i Bl�ddra-knapp.
*/
    else if ( md.pagant > 1  &&  wait_id == md.nxt_id )
      {
      ++md.actpag;
      if ( md.actpag > md.pagant ) md.actpag = 1;
      for ( i=md.start_ed,j=0; i<md.start_ed+md.ant_ed; ++i,++j )
        WPgted(md.iwin_id,md.ed_id[j],is[i]);
      goto loop1;
      }
/*
***Ingen av ovanst�ende !
***D� m�ste det vara en edit (med pos- eller ref-knapp).
*/
    else
      {
      for ( i=0; i<nstr; ++i )
        {
        if ( wait_id == md.ed_id[i] )
          {
          edtptr = (WPEDIT *)md.iwinpt->wintab[(wpw_id)wait_id].ptr;
          switch ( edtptr->symbol )
            {
/*
***TAB, CR eller pil ned�t. Om vi bara har ett inmatningsf�lt �r 
***detta lika med OK s�vida inte f�ltet �r tomt. Annars betyder
***det "n�sta f�lt" precis som vanligt.
*/
            case SMBMAIN:
            case SMBRETURN:
            case SMBDOWN:
            if ( nstr == 1 )
              {
              if ( edtptr->str[0] == '\0' ) status = REJECT;
              else                          status = 0;
              }
            else
              goto loop2;
            break;
/*
***Pil upp�t = Avbryt om det bara �r ett f�lt annars "f�reg�ende"
***precis som vanligt.
*/
            case SMBUP:
            if ( nstr == 1 ) status = REJECT;
            else             goto loop2;
            break;
/*
***<CTRL>a = Hj�lp.
*/
            case SMBHELP:
            IGhelp();
            goto loop2;
            break;

            default:
            goto loop2;
            }
          }
/*
***Inte edit, d� m�ste det vara en pos/ref-knapp.
*/
        else if ( wait_id == md.but_id[i] )
          {
          XUnmapWindow(xdisp,md.iwinpt->id.x_id);
          WPaddmess_mcwin(ips[md.start_ed+i],WP_PROMPT);
          pmmark();
          if ( typarr[md.start_ed+i] == C_VEC_VA ) /* Bugfix 1999-03-01 */
            status = IGcpos(0,&expr);
          else
            {
            typ = ALLTYP;
            status = IGcref((short)0,&typ,&expr,&right,&end);
            }
          pmrele();
          WPclear_mcwin();
          XMapWindow(xdisp,md.iwinpt->id.x_id);
          if ( status < 0 ) goto loop2;
          pprexs(expr,modtyp,tmpbuf,V3STRLEN);
          WPuped(md.iwin_id,(wpw_id)(wait_id-1),tmpbuf);
          goto loop2;
          }
        }
/*
***Kommer vi hit har vi servat h�ndelsen och skall sluta.
*/
      }
/*
***Om status = 0 l�ser vi av textf�nstren.
*/
    if ( status == 0 )
      {
      for ( i=md.start_ed,j=0; i<md.start_ed+md.ant_ed; ++i,++j )
        WPgted(md.iwin_id,md.ed_id[j],is[i]);
      }
/*
***Innan vi slutar kollar vi igen var f�nstret befinner sig.
***Skiljer sig positionen sen sist (wm_x1,wm_y1) har anv�ndaren
***flyttat f�nstret. F�r att denna f�rflyttning skall best�
***uppdaterar vi iwin_x och iwin_y i motsvarande grad.
*/
    WPgtwp(md.iwinpt->id.x_id,&wm_x2,&wm_y2);
    iwin_x = iwin_x + wm_x2 - wm_x1;
    iwin_y = iwin_y + wm_y2 - wm_y1;
/*
***Dags att d�da hela skiten.
*/
    WPwdel(md.iwin_id);
/*
***Slut.
*/
    return(status);
 }

/*********************************************************/
/*!*******************************************************/

     static short crewin(
     MSIDAT *md,
     char *dps,
     char *ips[],
     short nstr)

/*   Skapar huvudf�nster med dom knappar som alltid finns
 *   i ett huvudf�nster.
 *
 *   In: md  = Pekare till data.
 *
 *   Ut: *md = Lagrar ber�knade data.
 *
 *   FV:      0 = Ok.
 *
 *   (C)microform ab 26/1/94 J. Kjellander
 *
 *******************************************************!*/

 {
    char    str1[V3STRLEN+1];  /* Huvudpromt rad 1 */
    char    str2[V3STRLEN+1];  /* Huvudpromt rad 2 */
    short   max,i,x,y,dx,dy;
    DBint   pmt_id;
    int     tdx,tdy;
    WPWIN  *winptr;
    WPBUTT *butptr;

/*
***Hur stor �r sk�rmen ?
*/
    md->scr_dx = (short)DisplayWidth(xdisp,xscr);
    md->scr_dy = (short)DisplayHeight(xdisp,xscr);
/*
***Utg�ngsl�get f�r dialogboxens bredd �r halva sk�rmen.
*/
    md->iwin_dx =  0.5*md->scr_dx;
/*
***Luft i X-led, Y-led, mellan och alternativrutors h�jd.
*/
    md->lx = WPstrl("w");
    md->ly = 0.6*WPstrh();
    md->lm = 0.8*WPstrh();
    md->ih = 1.8*WPstrh();
    md->ah = 2.2*WPstrh(); 
/*
***1:a steget �r att avg�ra hur bred dialogboxen beh�ver vara.
***Kolla om promttexten skall delas upp i 2 rader.
***L�ngsta raden = md.tdx. Oavsett antal rader �r plats-
***behovet i Y-led = md.tdy.
*/
    WPdivs(dps,md->iwin_dx,&tdx,&tdy,str1,str2);
    md->tdx = tdx;
    md->tdy = tdy;
    max = md->tdx;
/*
***Ber�kna st�rsta verkliga platsbehovet f�r inmatnings-promt och
***max antal tecken som f�r matas in.
*/
    for ( i=0; i<nstr; ++i )
      if ( WPstrl(ips[i]) > max ) max = WPstrl(ips[i]);
      
    if ( 50*WPstrl("w") > max ) max = 50*WPstrl("w");
/*
***Ber�kna huvudf�nstrets verkliga bredd = dx. Om platsbehovet f�r
***texterna och inmatningsf�nstret �r mindre �n halva sk�rmen g�r
***vi dialogboxen mindre men minst 40 tecken.
*/  
    max   = md->lx + max + md->lx;
    md->iwin_dx = md->lx + md->iwin_dx + md->lx;
    if ( md->iwin_dx > max ) md->iwin_dx = max;
/*
***Main window size in Y-direction.
*/  
    md->max_ed = nstr;
loop:
    md->iwin_dy = md->ly + md->tdy + md->lm + 
        md->max_ed*(WPstrh() + md->lm + md->ih + md->lm) +
          md->ly + md->ly + md->ah + md->ly;  
/*
***If md->iwin_dy is too large to fit the screen we use
***multi page mode. How many input fields can we have
***one each page....
*/
    if ( md->iwin_dy > md->scr_dy - 30 )
      {
      md->max_ed -= 1;
      goto loop;
      }
/*
***...and how many pages do we need ?
*/
    if ( nstr > md->max_ed )
      md->pagant = 1 + (short)((double)nstr/(double)md->max_ed);
    else
      md->pagant = 1;
/*
***Inga inmatningsf�lt p� sk�rmen �nnu.
*/
    md->ant_ed = 0;
/*
***Placering tar vi samma som sist. Om f�nstret �r st�rre den h�r
***g�ngen s� att det sticker ut utanf�r sk�rmen backar vi lite.
*/
    if ( iwin_x + md->iwin_dx + 30 > md->scr_dx )
      iwin_x = md->scr_dx - md->iwin_dx - 30;
    if ( iwin_y + md->iwin_dy + 30 > md->scr_dy )
      iwin_y = md->scr_dy - md->iwin_dy - 30;
/*
***Skapa huvudf�nster i form av ett WPIWIN. 
*/
    WPwciw(iwin_x,iwin_y,md->iwin_dx,md->iwin_dy,
                                              "",&md->iwin_id);
    winptr = WPwgwp(md->iwin_id);
    md->iwinpt = (WPIWIN *)winptr->ptr;
/*
***Main promt, two, one or zero lines.
*/
    x = md->lx;
    y = md->ly;

    if ( *str1 != '\0' )
      {
      if ( *str2 == '\0' )
        WPmcbu((wpw_id)md->iwin_id,x,y,md->tdx,md->tdy,(short)0,
                                str1,"","",WP_BGND1,WP_FGND,&pmt_id);
      else
        {
        WPmcbu((wpw_id)md->iwin_id,x,y,md->tdx,md->tdy/2,(short)0,
                                str1,"","",WP_BGND1,WP_FGND,&pmt_id);
        WPmcbu((wpw_id)md->iwin_id,x,y+md->tdy/2,md->tdx,md->tdy/2,(short)0,
                                str2,"","",WP_BGND1,WP_FGND,&pmt_id);
        }
      }
/*
***A 3D line.
*/
   x = md->iwin_dx/8;
   y = md->ly + md->tdy + md->lm +
                md->max_ed*(WPstrh() + md->lm + md->ih + md->lm) + md->ly;
   WPmcbu(md->iwin_id,x,y-1,6*md->iwin_dx/8,1,(short)0,"","","",WP_BOTS,WP_BOTS,&pmt_id);
   WPmcbu(md->iwin_id,x,y  ,6*md->iwin_dx/8,1,(short)0,"","","",WP_TOPS,WP_TOPS,&pmt_id);
/*
***Ber�kna Ok-f�nstrets storlek och placering och skapa.
*/
    if ( WPstrl(okey) > WPstrl(reject) )
      md->ok_dx = md->lx + WPstrl(okey) + md->lx;
    else
      md->ok_dx = md->lx + WPstrl(reject) + md->lx;

    md->ok_dy = md->ah;
    md->ok_x  = md->lx;
    md->ok_y  = md->ly + md->tdy + md->lm +
                md->max_ed*(WPstrh() + md->lm + md->ih + md->lm) + md->ly + md->ly;

    WPmcbu((wpw_id)md->iwin_id,md->ok_x,md->ok_y,md->ok_dx,md->ok_dy,
                        (short)2,okey,okey,"",WP_BGND2,WP_FGND,&md->ok_id);
   butptr = (WPBUTT *)(md->iwinpt->wintab[md->ok_id].ptr);
   strcpy(butptr->tt_str,okeytt);
/*
***Ber�kna Reject-f�nstrets storlek och placering och skapa.
*/
    md->av_dx = md->lx + WPstrl(reject) + md->lx;
    md->av_dy = md->ah;
    md->av_x  = md->lx + md->ok_dx + md->lx;
    md->av_y  = md->ok_y;

    WPmcbu((wpw_id)md->iwin_id,md->av_x,md->av_y,md->av_dx,md->av_dy,
                  (short)2,reject,reject,"",WP_BGND2,WP_FGND,&md->av_id);
   butptr = (WPBUTT *)(md->iwinpt->wintab[md->av_id].ptr);
   strcpy(butptr->tt_str,rejecttt);
/*
***Ev. bl�ddra-knapp.
*/
    if ( md->pagant > 1 )
      {
      dx = md->lx + WPstrl(next) + md->lx;
      dy = md->ah;
      x  = md->lx + md->ok_dx + md->lx + md->av_dx + md->lx;
      y  = md->ok_y;

      WPmcbu((wpw_id)md->iwin_id,x,y,dx,dy,
               (short)2,next,next,"",WP_BGND2,WP_FGND,&md->nxt_id);
      }
/*
***Ber�kna Hj�lp-f�nstrets storlek och placering och skapa.
*/
    md->hlp_dx = md->lx + WPstrl(help) + md->lx;
    md->hlp_dy = md->ah;
    md->hlp_x  = md->iwin_dx - md->lx - md->hlp_dx;
    md->hlp_y  = md->ok_y;

    WPmcbu((wpw_id)md->iwin_id,md->hlp_x,md->hlp_y,md->hlp_dx,md->hlp_dy,
                (short)2,help,help,"",WP_BGND2,WP_FGND,&md->hlp_id);
   butptr = (WPBUTT *)(md->iwinpt->wintab[md->hlp_id].ptr);
   strcpy(butptr->tt_str,helptt);
/*
***Mappa manuellt.  WPwshw() s�tter iwinpt->mapped = TRUE,
***det vill vi inte eftersom vi vill mappa alla edit-f�nster
***samtidigt med ett anrop och inte automatiskt n�r de skapas.
*/
    XMapSubwindows(xdisp,md->iwinpt->id.x_id);
    XMapRaised(xdisp,md->iwinpt->id.x_id);
/*
***Slut.
*/
    return(0);
 }

/*********************************************************/
/*!*******************************************************/

     static short crepag(
     MSIDAT *md,
     char *ips[],
     char *is[],
     short maxtkn[],
     int   typarr[],
     short nstr)

/*   L�ser in en sida texter med redigering i dialogbox.
 *
 *   In: md     = Pekare till gemensamma data
 *       ips    = Inmatningsf�ltens Promtstr�ngar
 *       is     = Pekare till resultat.
 *       maxtkn = Max l�ngder p� inmatat resultat.
 *       typarr = F�lttyper.
 *       nstr   = Antal str�ngar.
 *
 *   Ut: *is = nstr stycken textstr�ngar.
 *
 *   FV:      0 = Ok.
 *       REJECT = Inmatningen avbruten.
 *
 *   (C)microform ab 17/8/92 J. Kjellander
 *
 *   1997-01-14 Bug bl�ddring, J.Kjellander
 *
 *******************************************************!*/

 {
    char    sidbuf[80];
    short   i,j,x,y,dx,dy,pdx;
    Window  xwin_id;
    WPEDIT *edtptr;
    WPBUTT *butptr;

/*
***F�r att slippa en massa on�diga Expose-events n�r
***vi nu d�dar och sedan skapar nya f�nster st�nger
***vi av detta p� huvudf�nstret.
*/
    XSelectInput(xdisp,md->iwinpt->id.x_id,0);
/*
***D�da nuvarande edit-f�nster, promtar samt ev. knappar
***f�r VECTOR/REF-parametrar.
*/
    for ( i=0; i<md->ant_ed; ++i )
      {
      edtptr = (WPEDIT *)md->iwinpt->wintab[md->ed_id[i]].ptr;
      xwin_id = edtptr->id.x_id;
      WPdled(edtptr);
      md->iwinpt->wintab[md->ed_id[i]].ptr = NULL;
      md->iwinpt->wintab[md->ed_id[i]].typ = TYP_UNDEF;
      XDestroyWindow(xdisp,xwin_id);

      butptr = (WPBUTT *)md->iwinpt->wintab[md->pmt_id[i]].ptr;
      xwin_id = butptr->id.x_id;
      WPdlbu(butptr);
      md->iwinpt->wintab[md->pmt_id[i]].ptr = NULL;
      md->iwinpt->wintab[md->pmt_id[i]].typ = TYP_UNDEF;
      XDestroyWindow(xdisp,xwin_id);

      if ( md->but_id[i] > 0 )
        {
        butptr = (WPBUTT *)md->iwinpt->wintab[md->but_id[i]].ptr;
        xwin_id = butptr->id.x_id;
        WPdlbu(butptr);
        md->iwinpt->wintab[md->but_id[i]].ptr = NULL;
        md->iwinpt->wintab[md->but_id[i]].typ = TYP_UNDEF;
        XDestroyWindow(xdisp,xwin_id);
        }
      }
/*
***Vilka f�lt skall visas p� denna sida ?
*/
    md->start_ed = (md->actpag - 1)*md->max_ed;
    md->ant_ed   = nstr - md->start_ed;

    if ( md->ant_ed > md->max_ed ) md->ant_ed = md->max_ed;
/*
***Ber�kna inmatnings-f�nstrens placering och
***skapa dem. Om promttexter finns, skapa en label med promt,
***annars skapa ett litet tomt f�nster, pdx=1.
***Se till att inmatningsf�nstrets l�ngd s�kert blir > 0.
*/
    x = md->lx;
    y = md->ly + md->tdy +md->lm + WPstrh() + md->lm;
    dx = md->iwin_dx - md->lx - md->lx - md->ih - 3;
    dy = md->ih;

    for ( i=md->start_ed,j=0; i<md->start_ed+md->ant_ed; ++i,++j )
      {
/*
***Promt.
*/
      if ( *ips[i] != '\0' ) pdx = WPstrl(ips[i]);
      else                   pdx = 1;
      WPmcbu(md->iwin_id,x,y-WPstrh()-md->ly,pdx,WPstrh(),
                 (short)0,ips[i],"","",WP_BGND1,WP_FGND,&md->pmt_id[j]);
/*
***Sj�lva edit-f�ltet.
*/     
      if ( maxtkn[i] < 1 ) maxtkn[i] = 1;
      WPmced(md->iwin_id,x,y,dx,dy,(short)2,is[i],maxtkn[i],&md->ed_id[j]);
/*
***Ev. pos/ref-knapp.
*/
      if ( typarr[i] == C_VEC_VA  ||  typarr[i] == C_REF_VA )
        WPmcbu(md->iwin_id,x+dx+2,y,md->ih+1,md->ih+1,
                 (short)1,"V","V","",WP_BGND2,WP_FGND,&md->but_id[j]);
      else md->but_id[j] = -1;
/*
***N�sta inmatningsf�lt.
*/
      y += WPstrh() + md->lm + dy + md->lm;
      }
/*
***Mappa de nya f�nstren och s�tt input focus p� det f�rsta.
*/
    edtptr = WPffoc(md->iwinpt,FIRST_EDIT);
    if ( edtptr != NULL ) WPfoed(edtptr,TRUE);

    XMapSubwindows(xdisp,md->iwinpt->id.x_id);
/*
***Utskrift av aktiv sida i WM-ramen.
*/
    if ( md->pagant > 1 )
      {
      sprintf(sidbuf,"%s   %s %d(%d)",title,page,md->actpag,md->pagant);
      XStoreName(xdisp,md->iwinpt->id.x_id,sidbuf);
      }
    else
      XStoreName(xdisp,md->iwinpt->id.x_id,title);
/*
***Sist av allt sl�r vi p� expose igen och g�r expose
***s� allt syns p� sk�rmen.
*/
    XSelectInput(xdisp,md->iwinpt->id.x_id,ExposureMask | KeyPressMask);
    WPxpiw(md->iwinpt);
    XFlush(xdisp);
/*
***Slut.
*/
    return(0);
 }

/*********************************************************/
