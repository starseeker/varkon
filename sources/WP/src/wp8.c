/**********************************************************************
*
*    wp8.c
*    ====
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpmsip();   Input multiple strings
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"

typedef struct
{
short   scr_dx;            /* Skärmens storlek i X-led */
short   scr_dy;            /* Skärmens storlek i Y-led */
v2int   iwin_id;           /* Huvudfönstrets ID */
WPIWIN *iwinpt;            /* C-pekare till huvudfönster */
short   iwin_dx,iwin_dy;   /* Huvudfönstrets storlek */
short   tdx,tdy;           /* Huvudpromtens storlek */
short   lx,ly,lm;          /* Luft i olika riktningar */
short   ih;                /* Inmatningsfältens höjd */
short   ah;                /* Alternativrutors höjd */
v2int   ok_id;             /* Okey-knappens ID */
short   ok_x,ok_y;         /* Okey-knappens placering */
short   ok_dx,ok_dy;       /* Okey-knappens storlek */
v2int   av_id;             /* Avbryt-knappens ID */
short   av_x,av_y;         /* Avbryt-knappens placering */
short   av_dx,av_dy;       /* Avbryt-knappens storlek */
v2int   hlp_id;            /* Hjälp-knappens ID */
short   hlp_x,hlp_y;       /* Hjälp-knappens placering */
short   hlp_dx,hlp_dy;     /* Hjälp-knappens storlek */
v2int   nxt_id;            /* Bläddra-knappens ID */
v2int   ed_id[V2MPMX];     /* Inmatningsfältens ID */
v2int   pmt_id[V2MPMX];    /* Promtars ID */
v2int   but_id[V2MPMX];    /* Pos-knappars ID */
short   max_ed;            /* Max antal inmatningsfält som får plats */
short   start_ed;          /* 1:a inmatnigsfält på aktiv sida */
short   ant_ed;            /* Aktivt antal inmatningsfält */
short   actpag,pagant;     /* Aktiv sida och antal sidor */
} MSIDAT;

/*
***MSIDAT är en struct som håller alla nödvändiga data om
***en viss inmatning. wpmsip() har en sådan som lokal variabel
***och kommunicerar med sina underordnade rutiner via en pekare
***till denna. Därmed är wpmsip() reentrant.
*/

static short iwin_x = 30;
static short iwin_y = 160;
 
/*
***iwin_x och iwin_y är huvudfönstrets läge på skärmen.
***Om användaren under en inmatning flyttar fönstret uppdateras
***dessa så att fönstret nästa gång kommer på det nya stället.
*/

static char title [81];        /* varkon.input.title */
static char page  [81];        /* varkon.input.page  */
static char okey  [81];        /* varkon.input.okey  */
static char reject[81];        /* varkon.input.reject*/
static char next  [81];        /* varkon.input.next  */
static char help  [81];        /* varkon.input.help  */

/*
***Ovanstående variabler håller texter från resursfilen.
*/
extern short    modtyp;

static short crewin(MSIDAT *md, char *dps, char *ips[], short nstr);
static short crepag(MSIDAT *md, char *ips[], char *is[], short maxtkn[],
                    int typarr[], short nstr);

/*!*******************************************************/

     short wpmsip(
     char *dps,
     char *ips[],
     char *ds[],
     char *is[],
     short maxtkn[],
     int   typarr[],
     short nstr)

/*   wp:s igmsip(). Multiple string input. Formulär-
 *   orienterad inläsning av en eller flera strängar.
 *   Om antalet stängar är större än vad som får plats
 *   på skärmen skapas flera sidor.
 *
 *   In: dps    = Dialogboxens Promptsträng
 *       ips    = Inmatningsfältens Promtsträngar
 *       ds     = Default-strängar
 *       is     = Pekare till resultat.
 *       maxtkn = Max längder på inmatat resultat.
 *       typarr = Inmatningsfältens typer.
 *       nstr   = Antal strängar.
 *
 *   Ut: *is = nstr stycken textsträngar.
 *
 *   FV:      0 = Ok.
 *       REJECT = Inmatningen avbruten.
 *
 *   (C)microform ab 17/8/92 J. Kjellander
 *
 *   25/1/94  Omarbetad, J. Kjellander
 *   4/11/94  Resurser för texter, J. Kjellander
 *   1996-12-12 typarr, J.Kjellander
 *   1997-01-31 Bug avslut med ett fält, J.Kjellander
 *   1998-10-04 Nollställning av nxt_id, J.Kjellander
 *   1999-03-01 Bug promt+typ vid bläddra, J.Kjellander
 *
 *******************************************************!*/

 {
    MSIDAT  md;    /* Lokala data som delas med andra rutiner */

    char     tmpbuf[V3STRLEN+1];
    short    status=0,i,j;
    v2int    wait_id;
    int      wm_x1,wm_x2,wm_y1,wm_y2;
    pm_ptr   expr;
    DBetype  typ;
    bool     right,end;
    WPEDIT  *edtptr;
    XEvent   event;


/*
***Först en liten koll så att det inte är för många texter.
***Max antal = Max antal parametrar i ett part-anrop eftersom
***det är det som denna rutin oftast används till.
*/
   if ( nstr > V2MPMX ) nstr = V2MPMX;
/*
***Vilka texter skall vi ha i fönsterram och på knappar ?
*/
    if ( !wpgrst("varkon.input.title",title) )   strcpy(title,"Inmatning");
    if ( !wpgrst("varkon.input.page",page) )     strcpy(page,"Sida");
    if ( !wpgrst("varkon.input.okey",okey) )     strcpy(okey,"Okej");
    if ( !wpgrst("varkon.input.reject",reject) ) strcpy(reject,"Avbryt");
    if ( !wpgrst("varkon.input.next",next) )     strcpy(next,"Bläddra");
    if ( !wpgrst("varkon.input.help",help) )     strcpy(help,"Hjälp");
/*
***Nu skapar vi själva huvudfönstret.
*/
    crewin(&md,dps,ips,nstr);
/*
***Vart hamnade fönstret ? Window Managern har antagligen
***flyttat lite på det för att ge plats åt sin ram.  För
***att vara säkra på att WM har tagit hand om fönstret
***och allt är stabilt väntar vi med att kolla läget tills
***vi fått vårt 1:a expose-event i fönstret. Detta är antagligen
***detsamma som att vänta på MapNotify.
*/
    XWindowEvent(xdisp,md.iwinpt->id.x_id,ExposureMask,&event);
    XPutBackEvent(xdisp,&event);
    wpgtwp(md.iwinpt->id.x_id,&wm_x1,&wm_y1);
/*
***För att alla ut-strängar säkert skall få värden
***kopierar vi till att börja med default-strängarna
***till dem.
*/
    for ( i=0; i<nstr; ++i ) strcpy(is[i],ds[i]);
/*
***Ut med inmatningsfälten. Vi börjar med sida 1.
*/
    md.actpag = 1;

loop1:
     crepag(&md,ips,is,maxtkn,typarr,nstr);
/*
***Vänta på klickning i knapp eller händelse i edit-fönster.
*/
loop2:
    wpwwtw(md.iwin_id,SLEVEL_V3_INP,&wait_id);
/*
***Klickning i OK-knapp. Med bara ett inmatningsfält och
***inget svar (tomt) betyder detta REJECT. Annars OK.
*/
    if ( wait_id == md.ok_id )
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
***Klickning i Hjälp-knapp.
*/
    else if ( wait_id == md.hlp_id )
      {
      ighelp();
      goto loop2;
      }
/*
***Klickning i Bläddra-knapp.
*/
    else if ( md.pagant > 1  &&  wait_id == md.nxt_id )
      {
      ++md.actpag;
      if ( md.actpag > md.pagant ) md.actpag = 1;
      for ( i=md.start_ed,j=0; i<md.start_ed+md.ant_ed; ++i,++j )
        wpgted(md.iwin_id,md.ed_id[j],is[i]);
      goto loop1;
      }
/*
***Ingen av ovanstående !
***Då måste det vara en edit (med pos- eller ref-knapp).
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
***CR eller pil nedåt. Om vi bara har ett inmatningsfält är 
***detta lika med OK såvida inte fältet är tomt. Annars betyder
***det "nästa fält" precis som vanligt.
*/
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
***Pil uppåt = Avbryt om det bara är ett fält annars "föregående"
***precis som vanligt.
*/
            case SMBUP:
            if ( nstr == 1 ) status = REJECT;
            else             goto loop2;
            break;
/*
***TAB = Huvudmenyn.
*/
            case SMBMAIN:
            status = GOMAIN;
            break;
/*
***<CTRL>b = Byt pos-metod.
*/
            case SMBPOSM:
            status = POSMEN;
            break;
/*
***<CTRL>a = Hjälp.
*/
            case SMBHELP:
            ighelp();
            goto loop2;
            break;

            default:
            goto loop2;
            }
          }
/*
***Inte edit, då måste det vara en pos/ref-knapp.
*/
        else if ( wait_id == md.but_id[i] )
          {
          XUnmapWindow(xdisp,md.iwinpt->id.x_id);
          igplma(ips[md.start_ed+i],IG_MESS);      /* Bugfix 1999-03-01 */
          pmmark();
          if ( typarr[md.start_ed+i] == C_VEC_VA ) /* Bugfix 1999-03-01 */
            status = genpos(0,&expr);
          else
            {
            typ = ALLTYP;
            status = genref((short)0,&typ,&expr,&right,&end);
            }
          pmrele();
          igrsma();
          XMapWindow(xdisp,md.iwinpt->id.x_id);
          if ( status < 0 ) goto loop2;
          pprexs(expr,modtyp,tmpbuf,V3STRLEN);
          edtptr = (WPEDIT *)md.iwinpt->wintab[(wpw_id)(wait_id-1)].ptr;
          wpuped(edtptr,tmpbuf);
          goto loop2;
          }
        }
/*
***Kommer vi hit har vi servat händelsen och skall sluta.
*/
      }
/*
***Om status = 0 läser vi av textfönstren.
*/
    if ( status == 0 )
      {
      for ( i=md.start_ed,j=0; i<md.start_ed+md.ant_ed; ++i,++j )
        wpgted(md.iwin_id,md.ed_id[j],is[i]);
      }
/*
***Innan vi slutar kollar vi igen var fönstret befinner sig.
***Skiljer sig positionen sen sist (wm_x1,wm_y1) har användaren
***flyttat fönstret. För att denna förflyttning skall bestå
***uppdaterar vi iwin_x och iwin_y i motsvarande grad.
*/
    wpgtwp(md.iwinpt->id.x_id,&wm_x2,&wm_y2);
    iwin_x = iwin_x + wm_x2 - wm_x1;
    iwin_y = iwin_y + wm_y2 - wm_y1;
/*
***Dags att döda hela skiten.
*/
    wpwdel(md.iwin_id);
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

/*   Skapar huvudfönster med dom knappar som alltid finns
 *   i ett huvudfönster.
 *
 *   In: md  = Pekare till data.
 *
 *   Ut: *md = Lagrar beräknade data.
 *
 *   FV:      0 = Ok.
 *
 *   (C)microform ab 26/1/94 J. Kjellander
 *
 *******************************************************!*/

 {
    char   str1[V3STRLEN+1];  /* Huvudpromt rad 1 */
    char   str2[V3STRLEN+1];  /* Huvudpromt rad 2 */
    short  max,i,x,y,dx,dy;
    v2int  pmt_id;
    int    tdx,tdy;
    WPWIN *winptr;

/*
***Hur stor är skärmen ?
*/
    md->scr_dx = (short)DisplayWidth(xdisp,xscr);
    md->scr_dy = (short)DisplayHeight(xdisp,xscr);
/*
***Utgångsläget för dialogboxens bredd är halva skärmen.
*/
    md->iwin_dx =  0.5*md->scr_dx;
/*
***Luft i X-led, Y-led, mellan och alternativrutors höjd.
*/
    md->lx = wpstrl("w");
    md->ly = 0.6*wpstrh();
    md->lm = 0.5*wpstrh();
    md->ih = 1.8*wpstrh();
    md->ah = 2.2*wpstrh(); 
/*
***1:a steget är att avgöra hur bred dialogboxen behöver vara.
***Kolla om promttexten skall delas upp i 2 rader.
***Längsta raden = md.tdx. Oavsett antal rader är plats-
***behovet i Y-led = md.tdy.
*/
    wpdivs(dps,md->iwin_dx,&tdx,&tdy,str1,str2);
    md->tdx = tdx;
    md->tdy = tdy;
    max = md->tdx;
/*
***Beräkna största verkliga platsbehovet för inmatnings-promt och
***max antal tecken som får matas in.
*/
    for ( i=0; i<nstr; ++i )
      if ( wpstrl(ips[i]) > max ) max = wpstrl(ips[i]);
      
    if ( 50*wpstrl("w") > max ) max = 50*wpstrl("w");
/*
***Beräkna huvudfönstrets verkliga bredd = dx. Om platsbehovet för
***texterna och inmatningsfönstret är mindre än halva skärmen gör
***vi dialogboxen mindre men minst 40 tecken.
*/  
    max   = md->lx + max + md->lx;
    md->iwin_dx = md->lx + md->iwin_dx + md->lx;
    if ( md->iwin_dx > max ) md->iwin_dx = max;
/*
***Beräkna huvudfönstrets höjd.
*/  
    md->max_ed = nstr;
loop:
    md->iwin_dy = md->ly + md->tdy + md->lm + 
        md->max_ed*(wpstrh() + md->lm + md->ih + md->lm) +
          md->ly + md->ah + md->ly;  
/*
***Får det plats på skärmen ? Om inte, minska antal inmatnings-
***fält och prova igen.
*/
    if ( md->iwin_dy > md->scr_dy - 30 )
      {
      md->max_ed -= 1;
      goto loop;
      }
/*
***Beräkna antal sidor som behövs.
*/
    if ( nstr > md->max_ed )
      md->pagant = 1 + (short)((double)nstr/(double)md->max_ed);
    else
      md->pagant = 1;
/*
***Inga inmatningsfält på skärmen ännu.
*/
    md->ant_ed = 0;
/*
***Placering tar vi samma som sist. Om fönstret är större den här
***gången så att det sticker ut utanför skärmen backar vi lite.
*/
    if ( iwin_x + md->iwin_dx + 30 > md->scr_dx )
      iwin_x = md->scr_dx - md->iwin_dx - 30;
    if ( iwin_y + md->iwin_dy + 30 > md->scr_dy )
      iwin_y = md->scr_dy - md->iwin_dy - 30;
/*
***Skapa huvudfönster i form av ett WPIWIN. 
*/
    wpwciw(iwin_x,iwin_y,md->iwin_dx,md->iwin_dy,
                                              "",&md->iwin_id);
    winptr = wpwgwp(md->iwin_id);
    md->iwinpt = (WPIWIN *)winptr->ptr;
/*
***Huvudfönstrets rubrik, noll en eller två rader.
***Om dps = "" skall vi inte skapa någon button.
*/
    x = md->lx;
    y = md->ly;

    if ( *str1 != '\0' )
      {
      if ( *str2 == '\0' )
        wpmcbu((wpw_id)md->iwin_id,x,y,md->tdx,md->tdy,(short)0,
                                str1,"","",WP_BGND,WP_FGND,&pmt_id);
      else
        {
        wpmcbu((wpw_id)md->iwin_id,x,y,md->tdx,md->tdy/2,(short)0,
                                str1,"","",WP_BGND,WP_FGND,&pmt_id);
        wpmcbu((wpw_id)md->iwin_id,x,y+md->tdy/2,md->tdx,md->tdy/2,(short)0,
                                str2,"","",WP_BGND,WP_FGND,&pmt_id);
        }
      }
/*
***Beräkna Ok-fönstrets storlek och placering och skapa.
*/
    if ( wpstrl(okey) > wpstrl(reject) )
      md->ok_dx = md->lx + wpstrl(okey) + md->lx;
    else
      md->ok_dx = md->lx + wpstrl(reject) + md->lx;
    md->ok_dy = md->ah;
    md->ok_x  = md->lx;
    md->ok_y  = md->ly + md->tdy + md->lm +
                md->max_ed*(wpstrh() + md->lm + md->ih + md->lm) + md->ly;

    wpmcbu((wpw_id)md->iwin_id,md->ok_x,md->ok_y,md->ok_dx,md->ok_dy,
                        (short)2,okey,okey,"",WP_BGND,WP_FGND,&md->ok_id);
/*
***Beräkna Reject-fönstrets storlek och placering och skapa.
*/
    md->av_dx = md->lx + wpstrl(reject) + md->lx;
    md->av_dy = md->ah;
    md->av_x  = md->lx + md->ok_dx + md->lx;
    md->av_y  = md->ok_y;

    wpmcbu((wpw_id)md->iwin_id,md->av_x,md->av_y,md->av_dx,md->av_dy,
                  (short)2,reject,reject,"",WP_BGND,WP_FGND,&md->av_id);
/*
***Ev. bläddra-knapp.
*/
    if ( md->pagant > 1 )
      {
      dx = md->lx + wpstrl(next) + md->lx;
      dy = md->ah;
      x  = md->lx + md->ok_dx + md->lx + md->av_dx + md->lx;
      y  = md->ok_y;

      wpmcbu((wpw_id)md->iwin_id,x,y,dx,dy,
               (short)2,next,next,"",WP_BGND,WP_FGND,&md->nxt_id);
      }
/*
***Beräkna Hjälp-fönstrets storlek och placering och skapa.
*/
    md->hlp_dx = md->lx + wpstrl(help) + md->lx;
    md->hlp_dy = md->ah;
    md->hlp_x  = md->iwin_dx - md->lx - md->hlp_dx;
    md->hlp_y  = md->ok_y;

    wpmcbu((wpw_id)md->iwin_id,md->hlp_x,md->hlp_y,md->hlp_dx,md->hlp_dy,
                (short)2,help,help,"",WP_BGND,WP_FGND,&md->hlp_id);
/*
***Mappa manuellt.  wpwshw() sätter iwinpt->mapped = TRUE,
***det vill vi inte eftersom vi vill mappa alla edit-fönster
***samtidigt med ett anrop och inte automatiskt när de skapas.
*/
    XMapSubwindows(xdisp,md->iwinpt->id.x_id);
    XMapWindow(xdisp,md->iwinpt->id.x_id);
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

/*   Läser in en sida texter med redigering i dialogbox.
 *
 *   In: md     = Pekare till gemensamma data
 *       ips    = Inmatningsfältens Promtsträngar
 *       is     = Pekare till resultat.
 *       maxtkn = Max längder på inmatat resultat.
 *       typarr = Fälttyper.
 *       nstr   = Antal strängar.
 *
 *   Ut: *is = nstr stycken textsträngar.
 *
 *   FV:      0 = Ok.
 *       REJECT = Inmatningen avbruten.
 *
 *   (C)microform ab 17/8/92 J. Kjellander
 *
 *   1997-01-14 Bug bläddring, J.Kjellander
 *
 *******************************************************!*/

 {
    char    sidbuf[80];
    short   i,j,x,y,dx,dy,pdx;
    Window  xwin_id;
    WPEDIT *edtptr;
    WPBUTT *butptr;

/*
***För att slippa en massa onödiga Expose-events när
***vi nu dödar och sedan skapar nya fönster stänger
***vi av detta på huvudfönstret.
*/
    XSelectInput(xdisp,md->iwinpt->id.x_id,0);
/*
***Döda nuvarande edit-fönster, promtar samt ev. knappar
***för VECTOR/REF-parametrar.
*/
    for ( i=0; i<md->ant_ed; ++i )
      {
      edtptr = (WPEDIT *)md->iwinpt->wintab[md->ed_id[i]].ptr;
      xwin_id = edtptr->id.x_id;
      wpdled(edtptr);
      md->iwinpt->wintab[md->ed_id[i]].ptr = NULL;
      md->iwinpt->wintab[md->ed_id[i]].typ = TYP_UNDEF;
      XDestroyWindow(xdisp,xwin_id);

      butptr = (WPBUTT *)md->iwinpt->wintab[md->pmt_id[i]].ptr;
      xwin_id = butptr->id.x_id;
      wpdlbu(butptr);
      md->iwinpt->wintab[md->pmt_id[i]].ptr = NULL;
      md->iwinpt->wintab[md->pmt_id[i]].typ = TYP_UNDEF;
      XDestroyWindow(xdisp,xwin_id);

      if ( md->but_id[i] > 0 )
        {
        butptr = (WPBUTT *)md->iwinpt->wintab[md->but_id[i]].ptr;
        xwin_id = butptr->id.x_id;
        wpdlbu(butptr);
        md->iwinpt->wintab[md->but_id[i]].ptr = NULL;
        md->iwinpt->wintab[md->but_id[i]].typ = TYP_UNDEF;
        XDestroyWindow(xdisp,xwin_id);
        }
      }
/*
***Vilka fält skall visas på denna sida ?
*/
    md->start_ed = (md->actpag - 1)*md->max_ed;
    md->ant_ed   = nstr - md->start_ed;

    if ( md->ant_ed > md->max_ed ) md->ant_ed = md->max_ed;
/*
***Beräkna inmatnings-fönstrens placering och
***skapa dem. Om promttexter finns, skapa en label med promt,
***annars skapa ett litet tomt fönster, pdx=1.
***Se till att inmatningsfönstrets längd säkert blir > 0.
*/
    x = md->lx;
    y = md->ly + md->tdy +md->lm + wpstrh() + md->lm;
    dx = md->iwin_dx - md->lx - md->lx - md->ih - 3;
    dy = md->ih;

    for ( i=md->start_ed,j=0; i<md->start_ed+md->ant_ed; ++i,++j )
      {
/*
***Promt.
*/
      if ( *ips[i] != '\0' ) pdx = wpstrl(ips[i]);
      else                   pdx = 1;
      wpmcbu(md->iwin_id,x,y-wpstrh()-md->lm,pdx,wpstrh(),
                 (short)0,ips[i],"","",WP_BGND,WP_FGND,&md->pmt_id[j]);
/*
***Själva edit-fältet.
*/     
      if ( maxtkn[i] < 1 ) maxtkn[i] = 1;
      wpmced(md->iwin_id,x,y,dx,dy,(short)2,is[i],maxtkn[i],&md->ed_id[j]);
/*
***Ev. pos/ref-knapp.
*/
      if ( typarr[i] == C_VEC_VA  ||  typarr[i] == C_REF_VA )
        wpmcbu(md->iwin_id,x+dx+2,y,md->ih+1,md->ih+1,
                 (short)1,"V","V","",WP_BGND,WP_FGND,&md->but_id[j]);
      else md->but_id[j] = -1;
/*
***Nästa inmatningsfält.
*/
      y += wpstrh() + md->lm + dy + md->lm;
      }
/*
***Mappa de nya fönstren och sätt input focus på det första.
*/
    edtptr = wpffoc(md->iwinpt,FIRST_EDIT);
    if ( edtptr != NULL ) wpfoed(edtptr,TRUE);

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
***Sist av allt slår vi på expose igen och gör expose
***så allt syns på skärmen.
*/
    XSelectInput(xdisp,md->iwinpt->id.x_id,ExposureMask | KeyPressMask);
    wpxpiw(md->iwinpt);
    XFlush(xdisp);
/*
***Slut.
*/
    return(0);
 }

/*********************************************************/
