/**********************************************************************
*
*    wp16.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpgtmp();    Returns current mouse position
*    wpgtmc();    Returns 2D-model coordinate
*    wpgmc3();    Returns 3D-model coordinate
*    wpgtsc();    Returns screen coordinate in pixels
*    wpgtsw();    Returns window size
*    wpneww();    Interactive creation of new window
*    wpzoom();    Interactive ZOOM
*    wpiaut();    Interactive AutoZOOM
*    wpmaut();    AutoZOOM from MBS
*    wpchvi();    Interactive ACT_VIEW
*    wpnivs();    Interactive blank/unblank/list levels
*    wpitsl();    Blank/unblank/list levels
*    wpmtsl();    Blank/unblank level from MBS
*    wpgtvi();    GET_VIEW in MBS
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <math.h>
#include <string.h>

extern MNUALT   smbind[];
extern VY       vytab[];
extern tbool    nivtb1[];
extern NIVNAM   nivtb2[];
extern char     jobnam[];
extern DBTmat  *lsyspk;

static void setgwm(long eventmask);
static void drwbox(Drawable win, GC gc, int ix1, int iy1,
                   int ix2, int iy2, int mode);

/*!******************************************************/

        short wpgtmp(
        int *px,
        int *py)

/*      Returnerar (pollar) aktuell musposition relativt skärmens
 *      övre vänstra hörn.
 *
 *      In: px     = Pekare till utdata.
 *          py     = Pekare till utdata.
 *
 *      Ut: *px    = X-koordinat i 2D modellkoordinater.
 *          *py    = Y-koordinat i 2D modellkoordinater.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 ******************************************************!*/

 {
   Window          root,parent,child;
   int             xrk,yrk,xck,yck;
   unsigned int    xbuts;

/*
***Var är musen.
*/
   parent = DefaultRootWindow(xdisp);

   XQueryPointer(xdisp,parent,&root,&child,&xrk,&yrk,&xck,&yck,&xbuts);

  *px = xrk;
  *py = yrk;
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpgtmc(
        char   *pektkn,
        double *px,
        double *py,
        bool    mark)

/*      Returnerar pek-tecken samt 2D-koordinat i modell-
 *      fönster koordinatsystemet.
 *
 *      In: pektkn => Pekare till pek-tecknet.
 *          px     => Pekare till X-koordinaten.
 *          py     => Pekare till Y-koordinaten.
 *          mark   => Pek-märke TRUE/FALSE
 *
 *      Ut:*pektkn => Pek-tecken.
 *         *px     => X-koordinat i 2D modellkoordinater.
 *         *py     => Y-koordinat i 2D modellkoordinater.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 8/1-95 J. Kjellander
 *
 ******************************************************!*/

 {
   short   ix,iy;
   v2int   win_id;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

   wpgtsc(mark,pektkn,&ix,&iy,&win_id);

   winptr = wpwgwp((wpw_id)win_id);
   gwinpt = (WPGWIN *)winptr->ptr;

  *px = gwinpt->vy.modwin.xmin + (ix - gwinpt->vy.scrwin.xmin)*
                   (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                   (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
  *py = gwinpt->vy.modwin.ymin + (iy - gwinpt->vy.scrwin.ymin)*
                   (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                   (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short     wpgmc3(
        char     *pektkn,
        DBVector *pout,
        bool      mark)

/*      Returnerar pek-tecken samt 3D modellkoordinat
 *      i aktivt korrdinatsystems XY-plan (lokala
 *      koordinater, Z=0).
 *
 *      In: pektkn => Pekare till utdata.
 *          pout   => Pekare till utdata.
 *          mark   => Pek-märke TRUE/FALSE
 *
 *      Ut:*pektkn => Pek-tecken.
 *         *pout   => 3D position.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 1998-03-31 J. Kjellander
 *
 ******************************************************!*/

 {
   short   ix,iy;
   v2int   win_id;
   gmflt   tt;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   DBVector   p,t;
   DBTmat  vm;

/*
***Hämta skärmkoordinat. wpgtsc() returnerar även ID
***för det fönster som det pekats i.
*/
   wpgtsc(mark,pektkn,&ix,&iy,&win_id);
/*
***Fixa pekare till fönstret.
*/
   winptr = wpwgwp((wpw_id)win_id);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Transformera till 2D modellkoordinat.
*/
   p.x_gm = gwinpt->vy.modwin.xmin + (ix - gwinpt->vy.scrwin.xmin)*
                   (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                   (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   p.y_gm = gwinpt->vy.modwin.ymin + (iy - gwinpt->vy.scrwin.ymin)*
                   (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                   (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
   p.z_gm = 0.0;
/*
***Bilda 4X4 transformationsmatris. Fönstrets vymatris är
***bara 3X3.
*/
   vm.g11 = gwinpt->vy.vymat.k11; vm.g12 = gwinpt->vy.vymat.k12;
   vm.g13 = gwinpt->vy.vymat.k13; vm.g14 = 0.0;
   vm.g21 = gwinpt->vy.vymat.k21; vm.g22 = gwinpt->vy.vymat.k22;
   vm.g23 = gwinpt->vy.vymat.k23; vm.g24 = 0.0;
   vm.g31 = gwinpt->vy.vymat.k31; vm.g32 = gwinpt->vy.vymat.k32;
   vm.g33 = gwinpt->vy.vymat.k33; vm.g34 = 0.0;
   vm.g41 = vm.g42 = vm.g43 = 0.0; vm.g44 = 1.0;
/*
***Transformera p till 3D modellkoordinat i BASIC.
*/
   GEtfpos_to_basic(&p,&vm,&p);
/*
***Skjutriktning, dvs. vyn:s Z-axel uttryckt i BASIC.
*/
   t.x_gm = gwinpt->vy.vymat.k31;
   t.y_gm = gwinpt->vy.vymat.k32;
   t.z_gm = gwinpt->vy.vymat.k33;
/*
***Transformera p och t till LOCAL.
*/
   if ( lsyspk != NULL )
     {
     GEtfpos_to_local(&p,lsyspk,&p);
     GEtfvec_to_local(&t,lsyspk,&t);
     }
/*
***Lokala Z-komponenten måste vara <> 0 annars är XY-planet
***parallellt med skjutriktningen.
*/
   if ( ABS(t.z_gm) < 1E-10 ) return(-1);
/*
***Beräkna skärning linje/XY-plan.
*/
   tt = -p.z_gm/t.z_gm;

   pout->x_gm = p.x_gm + tt*t.x_gm;
   pout->y_gm = p.y_gm + tt*t.y_gm;
   pout->z_gm = 0.0;
/*
***Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpgtsc(
        bool     mark,
        char    *pektkn,
        short   *pix,
        short   *piy,
        wpw_id  *win_id)

/*      Returnerar fönster-ID, pek-tecken samt 2D-koordinat i
 *      ett koordinatsystem med origo i grafiska areans nedre
 *      vänstra hörn.
 *
 *      In: mark    => Pekmärke TRUE/FALSE.
 *          pektkn  => Pekare till utdata.
 *          pix     => Pekare till utdata.
 *          piy     => Pekare till utdata.
 *          win_id  => Pekare till utdata.
 *
 *      Ut: *pektkn = Pek-tecken.
 *          *pix    = X-koordinat.
 *          *piy    = Y-koordinat.
 *          *win_id = ID för fönster i vilket pekningen skett.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 13/12/94 J. Kjellander
 *
 *       8/1-95   Multifönster, J. Kjellander
 *
 ******************************************************!*/

 {
    Window          root,parent,child;
    XEvent          xev;           
    int             xmax = 64;
    char            xkeybuf[65];
    int             xrk,yrk,xck,yck;
    unsigned int    xbuts; 
    short           n,status;
    WPGWIN         *gwinpt;
 
/*
***X11, byt utseende på cursorn och låt användaren
***peka med mus eller tangent. En egen event-loop här
***förhindrar tex. notify på icke aktiva pek-rutor.
*/
   wpscur(GWIN_ALL,TRUE,xgcur1);

   while ( 1 )
     {
     XMaskEvent(xdisp,ButtonPressMask | KeyPressMask | ExposureMask,&xev);
/*
***Vi har nu fått ett event.
*/
     switch (xev.type)
       {
/*
***Musknapptryckning.
*/
       case ButtonPress:
       if ( (gwinpt=wpggwp(xev.xbutton.window)) != NULL )
         {
         switch(xev.xbutton.button)
           {
           case 1:          /* Vänster */
          *pektkn = ' ';
           break;
           case 2:
          *pektkn = '\t';   /* Mitten */
           break;
           case 3:
          *pektkn = '\n';   /* Höger */
           break;
           }
        xrk     = xev.xkey.x;
        yrk     = xev.xkey.y;
        *pix    = (short)(xev.xkey.x);
        *piy    = (short)(gwinpt->geo.dy - xev.xkey.y);
        *win_id = gwinpt->id.w_id;
         goto exit;
         }
       break;
/*
***Tangent på tangentbordet. För att XQyeryPointer skall kunna
***anropas med rätt fönster som indata måste vi ta reda på det
***först. Eftersom vi inte vet vilket av flera möjliga grafiska
***fönster det rör sig om använder vi först XQueryPointer() för
***att ta reda på Root-fönstrets child. Om detta inte är ett
***WPGWIN provar vi om detta har nån child som är det osv. tills 
***vi hittar vårt fönster.
*/
       case KeyPress:
       parent = DefaultRootWindow(xdisp);

       do
         {
         XQueryPointer(xdisp,parent,&root,&child,&xrk,&yrk,&xck,&yck,&xbuts);
         if ( (gwinpt=wpggwp(child)) != NULL ) break;
         parent = child;
         } while ( child != 0 );

       if ( child == 0 ) break;
       else              XQueryPointer(xdisp,child,&root,&child,
                                       &xrk,&yrk,&xck,&yck,&xbuts);

       n = wplups((XKeyEvent *)&xev,xkeybuf,xmax);
       xrk    = xck;
       yrk    = yck;
/*
***Ett tecken blir det vid normal hårkorspekning från tangentbordet.
*/
       if ( n == 1 )
         {
        *pektkn = xkeybuf[0]; 		
         if ( *pektkn == '\015' ) *pektkn = '\n';
        *pix    = (short)(xck);
        *piy    = (short)(gwinpt->geo.dy - yck);
        *win_id = gwinpt->id.w_id;
         goto exit;
         }
/*
***Noll tecken blir det vid funktionstangent eller tex. SHIFT.
***Två tecken elle fler kan vara en piltangent etc.
***Om snabbval avbryts med TAB dvs. igdofu() returnerar SMBMAIN
***ser vi till att detta propageras bakåt genom att avsluta
***även här med TAB dvs. *smbindÄ7Å.str.
*/
       else
         {
         status = wpkepf((XKeyEvent *)&xev);
         if ( status != SMBNONE )
           {
           wpscur(GWIN_ALL,TRUE,xgcur1);
           if ( status == SMBMAIN )
             {
            *pektkn = *smbind[7].str; 		
            *pix    = (short)(xck);
            *piy    = (short)(gwinpt->geo.dy - yck);
            *win_id = gwinpt->id.w_id;
             goto exit;
             }
           }
         }
       break;
/*
***Expose-event skickar vi bara vidare till wpwexp().
*/
       case Expose:
       wpwexp((XExposeEvent *)&xev);
       break;
       } 
     }
/*
***Slut.
*/
exit:
   wpscur(GWIN_ALL,FALSE,(Cursor)0);
/*
***Eventuellt pekmärke.
*/
   if ( mark ) wpcpmk((v2int)gwinpt->id.w_id,xrk,yrk);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpgtsw(
        WPGWIN **gwinptp,
        int    *pix1,
        int    *piy1,
        int    *pix2,
        int    *piy2,
        int     mode,
        bool    prompt)

/*      Interaktiv rutin för skärmfönster med gummibands-
 *      rektangel.
 *
 *      In:  gwinptp = Pekare till utdata.
 *           pix1    = Pekare till utdata.
 *           piy1    = Pekare till utdata.
 *           pix2    = Pekare till utdata.
 *           piy2    = Pekare till utdata.
 *           mode    = Typ av gummibandsfigur
 *           prompt  = Autoprompting
 *
 *      Ut: *gwinptp = Pekare till fönster där pekning skett.
 *          *pix1    = Övre vänstra hörnets X-kordinat.
 *          *piy1    = Övre vänstra hörnets Y-koordinat.
 *          *pix2    = Nedre högra hörnets X-koordinat.
 *          *piy2    = Nedre högra hörnets Y-koordinat.
 *
 *      FV:      0 = OK.
 *              -1 = pos1=pos2
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 12/12/94 J. Kjellander
 *
 *      23/2/95 PointerMotionHintMask, J. Kjellander
 *      1/3/95  drwbox(), J. Kjellander
 *      1998-03-19 Returnera fönster, J.Kjellander
 *      1998-03-24 mode, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      x1=0,y1=0,lastx2=0,lasty2=0;
    int      curx2,cury2,minh,minw;
    Window   root,child;
    int      root_x,root_y,win_x,win_y;
    unsigned int mask;
    XEvent   xev;
    WPGWIN  *gwinpt=NULL;

/*
***Här behövs det en särskild eventmask och cursor.
***Sätt både mask och cursor på alla grafiska fönster.
*/
   if ( prompt ) igptma(322,IG_MESS);
   setgwm(GWEM_RUB);
   wpscur(GWIN_ALL,TRUE,xgcur1);
/*
***Eventloop.
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
***Kolla vilket fönster det är frågan om och returnera
***till anropande rutin.
*/
       case ButtonPress:
       wpscur(GWIN_ALL,TRUE,xgcur2);
       if ( prompt ) igrsma();

       gwinpt = wpggwp(xev.xany.window);
       if ( gwinpt == NULL )
         {
         status = REJECT;
         goto exit;
         }
       else *gwinptp = gwinpt;
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

       x1 = curx2 = lastx2 = xev.xkey.x;
       y1 = cury2 = lasty2 = xev.xkey.y;
       if ( prompt) igptma(323,IG_MESS);
/*
***Pil kräver särskild initiering.
*/
       if ( mode == WP_RUB_ARROW )
         {
         XDrawArc(xdisp,gwinpt->id.x_id,gwinpt->rub_gc,
                                 x1-10,y1-10,20,20,0,360*64);
         drwbox(gwinpt->id.x_id,gwinpt->rub_gc,x1,y1,x1,y1,mode);
         }
       break;
/*
***Nu flyttar sig musen.
*/
       case MotionNotify:
/*
***Det bör inte vara tillåtet att gå ut utanför fönstret.
*/
       if ( xev.xmotion.x < (int)gwinpt->vy.scrwin.xmin )
            xev.xmotion.x = (int)gwinpt->vy.scrwin.xmin;

       if ( xev.xmotion.x > (int)gwinpt->vy.scrwin.xmax )
            xev.xmotion.x = (int)gwinpt->vy.scrwin.xmax;

       if ( xev.xmotion.y < (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax) )
            xev.xmotion.y = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax);

       if ( xev.xmotion.y > (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin) )
            xev.xmotion.y = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin);
/*
***Vi är fortfarande kvar inuti fönstret. Rita om boxen.
*/
       curx2 = xev.xmotion.x;
       cury2 = xev.xmotion.y;

       if ( curx2 != lastx2  || cury2 != lasty2 )
         {
         drwbox(gwinpt->id.x_id,gwinpt->rub_gc,x1,y1,lastx2,lasty2,mode);
         if ( curx2 != x1  || cury2 != y1 )
           drwbox(gwinpt->id.x_id,gwinpt->rub_gc,x1,y1,curx2,cury2,mode);
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
       if ( xev.xkey.x < (int)gwinpt->vy.scrwin.xmin )
            xev.xkey.x = (int)gwinpt->vy.scrwin.xmin;

       if ( xev.xkey.x > (int)gwinpt->vy.scrwin.xmax )
            xev.xkey.x = (int)gwinpt->vy.scrwin.xmax;

       if ( xev.xkey.y < (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax) )
            xev.xkey.y = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymax);

       if ( xev.xkey.y > (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin) )
            xev.xkey.y = (int)(gwinpt->geo.dy-gwinpt->vy.scrwin.ymin);

       if ( prompt ) igrsma();

       if ( lastx2 != x1  || lasty2 != y1 )
         drwbox(gwinpt->id.x_id,gwinpt->rub_gc,x1,y1,lastx2,lasty2,mode);

       curx2 = xev.xkey.x;
       cury2 = xev.xkey.y;
/*
***Pil kräver särskil avslutning.
*/
       if ( mode == WP_RUB_ARROW )
         XDrawArc(xdisp,gwinpt->id.x_id,gwinpt->rub_gc,
                                 x1-10,y1-10,20,20,0,360*64);
/*
***Varkons y-axel är motsatt X11.
*/
       y1    = gwinpt->geo.dy - y1;
       cury2 = gwinpt->geo.dy - cury2;
/*
***Sortera koordinaterna.
*
       if ( x1 < curx2 ) { *pix1 = x1;    *pix2 = curx2; }
       else              { *pix1 = curx2; *pix2 = x1; }
       if ( y1 < cury2 ) { *piy1 = y1;    *piy2 = cury2; }
       else              { *piy1 = cury2; *piy2 = y1; }
*/
        *pix1 = x1;  *pix2 = curx2;
        *piy1 = y1;  *piy2 = cury2;
/*
***Minsta fönster för att betraktas som ok = 1% av skärmen.
***Är det mindre betraktar vi det som en enstaka klickning
***och returnerar -1.
*/
       minh = (int)(0.01*(double)DisplayHeight(xdisp,xscr));
       minw = (int)(0.01*(double)DisplayWidth(xdisp,xscr));

       if ( abs(*pix2 - *pix1) < minw  ||
            abs(*piy2 - *piy1) < minh ) status = -1;
       else                             status =  0;
       goto exit;
       }
     }
/*
***Utgång. Återställ eventmask och cursor.
*/
exit:
   setgwm(GWEM_NORM);
   wpscur(GWIN_ALL,FALSE,(Cursor)0);

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short wpneww()

/*      Interaktiv funktion för att skapa nytt WPGWIN.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 1/1/95 J. Kjellander
 *
 *      23/2/95 PointerMotionHintMask, J. Kjellander
 *      1/3/95  drwbox(), J. Kjellander
 *
 ******************************************************!*/

  {
   int                  x1=0,y1=0,curx2,cury2,lastx2=0,lasty2=0,
                        minh,minw,ix1=0,iy1=0,ix2=0,iy2=0,
                        root_x,root_y,win_x,win_y;
   unsigned int         mask;
   XEvent               xev;           
   XSetWindowAttributes xwina;
   unsigned long        xwinm;
   Window               xwin_id,root,child;
   short                status,ix,iy,dx,dy;
   v2int                id;
   WPWIN               *winptr;
   WPGWIN              *mainpt,*gwinpt;
   GC                   rub_gc;

/*
***Skapa ett (osynligt) InputOnly-fönster lika stort som hela skärmen.
*/
    xwina.override_redirect = True;
    xwinm                   = CWOverrideRedirect;  

    xwin_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),1,1,
                            DisplayWidth(xdisp,xscr),
                            DisplayHeight(xdisp,xscr),0,0,
                            InputOnly,CopyFromParent,xwinm,&xwina);

    XMapRaised(xdisp,xwin_id);
/*
***Modifiera gummibands-GC:et i V3:s huvudfönster lite.
*/
   winptr = wpwgwp((wpw_id)GWIN_MAIN);
   mainpt = (WPGWIN *)winptr->ptr;
   rub_gc = mainpt->rub_gc;
   XSetSubwindowMode(xdisp,rub_gc,IncludeInferiors);
/*
***En variant av wpgtsw() för rubberband-rektangel.
*/
   igptma(322,IG_MESS);
   XSelectInput(xdisp,xwin_id,GWEM_RUB);
   XDefineCursor(xdisp,xwin_id,xgcur1);
/*
***När vi börjar har musknappen ännu inte tryckts ned.
*/
   status = 0;

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
       XDefineCursor(xdisp,xwin_id,xgcur2);
       igrsma();

       if ( xev.xany.window != xwin_id )
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

       x1 = curx2 = lastx2 = xev.xkey.x;
       y1 = cury2 = lasty2 = xev.xkey.y;
       igptma(323,IG_MESS);
       break;
/*
***Nu flyttar sig musen.
*/
       case MotionNotify:
       curx2 = xev.xmotion.x;
       cury2 = xev.xmotion.y;
       if ( curx2 != lastx2  || cury2 != lasty2 )
         {
         drwbox(DefaultRootWindow(xdisp),rub_gc,
                             x1,y1,lastx2,lasty2,WP_RUB_RECT);

         if ( curx2 != x1  || cury2 != y1 )
           drwbox(DefaultRootWindow(xdisp),rub_gc,
                             x1,y1,curx2,cury2,WP_RUB_RECT);
         lastx2 = curx2;
         lasty2 = cury2;
         }
/*
***För att få ett nytt Motion-event måste vi anropa XQueryPointer().
*/
       XQueryPointer(xdisp,DefaultRootWindow(xdisp),&root,&child,
                                    &root_x,&root_y,&win_x,&win_y,&mask);
       break;
/*
***När knappen släpps igen är det slut.
*/
       case ButtonRelease:
       igrsma();
       if ( lastx2 != x1  || lasty2 != y1 )
         drwbox(DefaultRootWindow(xdisp),rub_gc,
                         x1,y1,lastx2,lasty2,WP_RUB_RECT);
       curx2 = xev.xkey.x;
       cury2 = xev.xkey.y;
/*
***Sortera koordinaterna.
*/
       if ( x1 < curx2 ) { ix1 = x1;    ix2 = curx2; }
       else              { ix1 = curx2; ix2 = x1; }
       if ( y1 < cury2 ) { iy1 = y1;    iy2 = cury2; }
       else              { iy1 = cury2; iy2 = y1; }
/*
***Minsta fönster för att betraktas som ok = 1% av skärmen.
*/
       minh = (int)(0.01*(double)DisplayHeight(xdisp,xscr));
       minw = (int)(0.01*(double)DisplayWidth(xdisp,xscr));

       if ( abs(ix2 - ix1) < minw  ||
            abs(iy2 - iy1) < minh ) status = REJECT;
       else                             status = 0;
       goto exit;
       }
     }
/*
***Utgång. Återställ GC och döda spök-fönstret.
*/
exit:
   XSetSubwindowMode(xdisp,rub_gc,ClipByChildren);
   XDestroyWindow(xdisp,xwin_id);
/*
***Om allt har gått bra fortsätter vi annars är det slut.
*/
   if ( status < 0 ) return(status);
/*
***Skapa nytt WPGWIN. Kompensera lite för Motif-ram.
*/
   ix = (short)(ix1 - 6);
   iy = (short)(iy1 - 23);
   dx = (short)(ix2 - ix1);
   dy = (short)(iy2 - iy1);

   if ( wpwcgw(ix,iy,dx,dy,"",FALSE,&id) < 0 )
     {
     errmes();
     return(0);
     }
/*
***Aktivera samma vy som i huvudfönstret.
*/
   winptr = wpwgwp(GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
   wpacvi(gwinpt->vy.vynamn,id);
/*
***Rita.
*/
   wprepa(id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpzoom(
        WPGWIN *gwinpt1)

/*      Varkon-funktion för ZOOM med X-Windows.
 *
 *      In: gwinpt = Pekare till fönstret som skall zoom:as.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      Felkod: IG3042 = Kan ej minska skalan mera.
 *
 *      (C)microform ab 14/12/94 J. Kjellander
 *
 *      1998-04-03 Ny wpgtsw(), J.Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   int     ix1,iy1,ix2,iy2,tmp;
   double  x1,y1,x2,y2;
   double  dx,dy;
   WPGWIN *gwinpt;

/*
***Vi börjar med att hämta ett skärmfönster. wpgtsw()
***sorterar inte koordinaterna (numera).
*/
   if ( (status=wpgtsw(&gwinpt,&ix1,&iy1,&ix2,&iy2,WP_RUB_RECT,TRUE)) < 0 )
     return(status);
/*
***Sortera koordinaterna.
*/
       if ( ix2 < ix1 ) { tmp = ix1; ix1 = ix2; ix2 = tmp; }
       if ( iy2 < iy1 ) { tmp = iy1; iy1 = iy2; iy2 = tmp; }
/*
***Transformera till modellkoordinater.
*/
   x1 = gwinpt->vy.modwin.xmin + (ix1-gwinpt->vy.scrwin.xmin)*
       (gwinpt->vy.modwin.xmax-gwinpt->vy.modwin.xmin)/
       (gwinpt->vy.scrwin.xmax-gwinpt->vy.scrwin.xmin);

   y1 = gwinpt->vy.modwin.ymin + (iy1-gwinpt->vy.scrwin.ymin)*
       (gwinpt->vy.modwin.ymax-gwinpt->vy.modwin.ymin)/
       (gwinpt->vy.scrwin.ymax-gwinpt->vy.scrwin.ymin);

   x2 = gwinpt->vy.modwin.xmin + (ix2-gwinpt->vy.scrwin.xmin)*
       (gwinpt->vy.modwin.xmax-gwinpt->vy.modwin.xmin)/
       (gwinpt->vy.scrwin.xmax-gwinpt->vy.scrwin.xmin);

   y2 = gwinpt->vy.modwin.ymin + (iy2-gwinpt->vy.scrwin.ymin)*
       (gwinpt->vy.modwin.ymax-gwinpt->vy.modwin.ymin)/
       (gwinpt->vy.scrwin.ymax-gwinpt->vy.scrwin.ymin);
/*
***Felkontroll.
*/
   dx = x2 - x1;
   dy = y2 - y1;

   if ( dx < 1e-10 || dy < 1e-10 )
     {
     erpush("IG3042","wpzoom");
     errmes();
     return(0);
     }
/*
***Lagra det nya modellfönstret i WPGWIN-posten och normalisera.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.modwin.xmin = x1;
   gwinpt->vy.modwin.xmax = x2;
   gwinpt->vy.modwin.ymin = y1;
   gwinpt->vy.modwin.ymax = y2;

   wpnrgw(gwinpt);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
   if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
/*
***Rita om fönstret.
*/
   wprepa((v2int)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpiaut(
        WPGWIN *gwinpt)

/*      Varkon-funktion för AutoZOOM med X-Windows.
 *
 *      In: gwinpt = Pekare till fönstret som skall zoom:as.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 29/12/94 J. Kjellander
 *
 ******************************************************!*/

  {
   double dmx,dmy,dx,dy;
   VY     minvy;

/*
***Sudda fönstret.
*/
   wpergw((v2int)gwinpt->id.w_id);
/*
***Vänta...
*/
   wpwait(gwinpt->id.w_id,TRUE);
/*
***Beräkna nytt modellfönster. För att detta skall ske
***med rätt actvy, actvym osv. fixar vi grapac först
***och återställer efteråt.
*/
   if ( gwinpt->id.w_id == GWIN_MAIN )
     {
     wpfixg(gwinpt);
     igmsiz(&minvy);
     wpfixg(NULL);
     }
   else
     {
     if ( wpmsiz(gwinpt,&minvy) == AVBRYT )
       {
       wpwait(gwinpt->id.w_id,FALSE);
       return(igwtma(168));
       }
     }
/*
***Kolla att fönstret inte blev oändligt litet. Kan tex.
***inträffa om modellen består av en enda punkt. Om så är
***fallet placerar vi punkten i mitten av fönstret och sätter
***skala = 1.
*/
   dx = minvy.vywin[2] - minvy.vywin[0];
   dy = minvy.vywin[3] - minvy.vywin[1];

   if ( dx == 0.0  &&  dy == 0.0 )
     {
     dmx = gwinpt->geo.psiz_x *
          (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     dmy = gwinpt->geo.psiz_y *
          (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     minvy.vywin[0] -= dmx/2.0;
     minvy.vywin[1] -= dmy/2.0;
     minvy.vywin[2] += dmx/2.0;
     minvy.vywin[3] += dmy/2.0;
     }
/*
***Om fönstret inte blev oändligt litet men mindre än 1e-10
***har vi inget fönster alls. Inga storheter har gett upphov
***till någon grafik. I såfall kan skärmen förbli suddad och 
***vi avslutar direkt.
*/
   else if ( dx < 1e-10  &&  dy < 1e-10 )
     {
     wpwait(gwinpt->id.w_id,FALSE);
     return(0);
     }
/*
***Om fönstret blev orimligt smalt som tex fallet med en
***horisontell eller vertikal linje gör vi det lite bredare eller högre.
*/
loop:
   dx = minvy.vywin[2] - minvy.vywin[0];
   dy = minvy.vywin[3] - minvy.vywin[1];

   if ( dx < 1e-10 )
     {
     minvy.vywin[0] -= (0.05*dy);
     minvy.vywin[2] += (0.05*dy);
     goto loop;
     }

   if ( dy < 1e-10 )
     {
     minvy.vywin[1] -= (0.05*dx);
     minvy.vywin[3] += (0.05*dx);
     goto loop;
     }
/*
***Gör fönstret 8% större så att allt syns ordentligt.
*/
   minvy.vywin[0] -= (0.04*dx);
   minvy.vywin[2] += (0.04*dx);
   minvy.vywin[1] -= (0.04*dy);
   minvy.vywin[3] += (0.04*dy);
/*
***Uppdatera modellfönstret i WPGWIN-posten och normalisera.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   gwinpt->vy.modwin.xmin = minvy.vywin[0];
   gwinpt->vy.modwin.xmax = minvy.vywin[2];
   gwinpt->vy.modwin.ymin = minvy.vywin[1];
   gwinpt->vy.modwin.ymax = minvy.vywin[3];

   wpnrgw(gwinpt);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*/
   if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
/*
***Slut på väntan.
*/
   wpwait(gwinpt->id.w_id,FALSE);
/*
***Rita om fönstret.
*/
   wprepa((v2int)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpmaut(
        v2int win_id)

/*      AutoZOOM med X-Windows från MBS.
 *
 *      In: win_id = ID för fönster som skall zoom:as.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1382 = Fönster %s finns ej
 *
 *      (C)microform ab 18/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[81];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om fönstret finns fixar vi en C-pekare till det
***och anropar den interaktiva versionen av AutoZOOM.
*/
   if ( (winptr=wpwgwp((wpw_id)win_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     return(wpiaut(gwinpt));
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1382",errbuf));
     }
  }

/********************************************************/
/*!******************************************************/

         short wpchvi(
         WPGWIN *gwinpt,
         int     x,
         int     y)

/*      Byter vy i ett visst fönster.
 *
 *      In:  gwinpt = C-pekare till grafiskt fönster.
 *           x,y    = Aktiverande kanpps/ikons rootläge.
 *
 *      Ut:  Inget.
 *
 *      (C)microform ab 3/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char    *namlst[GPMAXV],rubrik[81];
   int      i,nvy,alt,actalt;

/*
***Fixa en array av pekare till vynamnen.
***Till att börja med är inget alternativ aktivt
***men om någon av vyerna är aktiv i detta fönster
***noterar vi det som actalt.
*/
   actalt = -1;
   nvy    =  0;

   for ( i=0; i<GPMAXV; ++i )
     {
     if ( vytab[i].vynamn[0] != '\0' )
       {
       if ( strcmp(vytab[i].vynamn,gwinpt->vy.vynamn) == 0 ) actalt = nvy;
       namlst[nvy] = vytab[i].vynamn;
       ++nvy;
       }
     }
/*
***Vy-fönstrets rubrik.
*/
   if ( !wpgrst("varkon.view.title",rubrik) ) strcpy(rubrik,"Vyer");
/*
***Anropa wpilst().
*/
   if ( wpilst(x,y,rubrik,namlst,actalt,nvy,&alt) == REJECT ) return(0);
/*
***Träff i alternativ nummer "alt". Aktivera den.
*/
   V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVY));

   wpacvi(namlst[alt],(v2int)gwinpt->id.w_id);
/*
***Rita om fönstret.
*/
   wprepa((v2int)gwinpt->id.w_id);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpnivs(
        WPGWIN *gwinpt,
        int     x,
        int     y)

/*      Varkon-funktion för nivå(er) med X-Windows.
 *
 *      In: gwinpt = Pekare till aktuellt fönster.
 *          x,y    = Aktiverande knapps/ikons läge.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 15/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],blank[81],ublank[81],list[81];
   int      mode=0,main_x,main_y;
   short    status,main_dx,main_dy,alt_x,alt_y,alth,altlen,ly,lm;
   v2int    iwin_id,bl_id,ub_id,list_id,but_id;

/*
***Texter för rubrik, tänd, släck och lista.
*/
   if ( !wpgrst("varkon.level.blank",blank) )    strcpy(blank,"Sl{ck");
   if ( !wpgrst("varkon.level.unblank",ublank) ) strcpy(ublank,"T{nd");
   if ( !wpgrst("varkon.level.list",list) )      strcpy(list,"Lista");
   if ( !wpgrst("varkon.level.title",rubrik) )   strcpy(rubrik,"Niv}er");
/*
***Längsta texten avgör fönstrets bredd.
*/
   altlen = 0;

   if ( wpstrl(blank)  > altlen ) altlen = (short)(wpstrl(blank));
   if ( wpstrl(ublank) > altlen ) altlen = (short)(wpstrl(ublank));
   if ( wpstrl(list)   > altlen ) altlen = (short)(wpstrl(list));
   if ( wpstrl(rubrik) > altlen ) altlen = (short)(wpstrl(rubrik));
 
   altlen += 15;
/*
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/
   ly   = (short)(0.8*wpstrh());
   alth = (short)(1.6*wpstrh()); 
   lm   = (short)(1.4*wpstrh());

   main_dx = (short)(ly + altlen + ly);
   main_dy = (short)(ly + 3*(alth + ly));  
/*
***Skapa själva inmatningsfönstret som ett WPIWIN.
*/
   wpposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   wpwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Tänd.
*/
   alt_x  = ly;
   alt_y  = ly;
   wpmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)1,ublank,ublank,"",WP_BGND,WP_FGND,&ub_id);
/*
***Släck.
*/
   alt_y = ly + alth + ly;
   wpmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)1,blank,blank,"",WP_BGND,WP_FGND,&bl_id);
/*
***Lista.
*/
   alt_y = (short)(ly + 2*(alth + ly));
   wpmcbu((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,
                   (short)1,list,list,"",WP_BGND,WP_FGND,&list_id);
/*
***Klart för visning.
*/
   wpwshw(iwin_id);
/*
***Vänta på action.
*/
   wpwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Så fort det händer nåt avslutar vi.
*/
   wpwdel(iwin_id);
/*
***Tänd.
*/
   if ( but_id == ub_id ) mode = WP_UBLANKL;
/*
***Släck.
*/
   else if ( but_id == bl_id ) mode = WP_BLANKL;
/*
***Lista.
*/
   else if ( but_id == list_id ) mode = WP_LISTL;

   status = wpitsl(gwinpt,x,y,mode);
   if ( status < 0 ) return(status);
/*
***Rita om fönstret.
*/
   if ( but_id != list_id ) wprepa((v2int)gwinpt->id.w_id);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpitsl(
        WPGWIN *gwinpt,
        int     x,
        int     y,
        int     mode)

/*      Varkon-funktion för "tänd/släck/lista nivå(er)"
 *      med X-Windows.
 *
 *      In: gwinpt = Pekare till aktuellt fönster.
 *          x,y    = Aktiverande knapps/ikons läge.
 *          mode   = WP_BLANKL  => Släck nivåer.
 *                   WP_UBLANKL => Tänd nivåer.
 *                   WP_LISTL   => Lista nivåer.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *
 *      (C)microform ab 15/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],first[81],add[81],okey[81],reject[81],help[81],
            forw[81],back[81],last[81],valstr[81],tmp[81];
   int      main_x,main_y,altlen,alt_x,alt_y,alth;
   short    main_dx,main_dy,ly,lm;
   DBint    iwin_id,dum_id,fb_id,but_id,help_id,okey_id,reject_id,
            first_id,add_id,last_id,act_first,act_add,act_last;
   bool     act_forw;

   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPEDIT  *frstpt;
   WPEDIT  *addpt;
   WPEDIT  *lastpt;
   WPBUTT  *forwpt;

static DBint org_first = 0;
static DBint org_add   = 0;
static DBint org_last  = 0;
static bool org_forw   = TRUE;

/*
***Texter för rubrik, första, ytterligare, framåt, bakåt,
***sista, avbryt och hjälp.
*/
   if ( mode == WP_BLANKL )
     {
     if ( !wpgrst("varkon.level.blank",rubrik) ) strcpy(rubrik,"Sl{ck");
     }
   else if ( mode == WP_UBLANKL )
     {
     if ( !wpgrst("varkon.level.unblank",rubrik) ) strcpy(rubrik,"T{nd");
     }
   else
     {
     if ( !wpgrst("varkon.level.list",rubrik) ) strcpy(rubrik,"Lista");
     }

   if ( !wpgrst("varkon.level.title",tmp) ) strcpy(tmp," Niv}er");
   strcat(rubrik," ");
   strcat(rubrik,tmp);

   if ( !wpgrst("varkon.level.first",first) )   strcpy(first,"F|rsta niv}");
   if ( !wpgrst("varkon.level.add",add) )       strcpy(add,"Antal ytterligare");
   if ( !wpgrst("varkon.level.forward",forw) )  strcpy(forw,"Fram}t");
   if ( !wpgrst("varkon.level.backward",back) ) strcpy(back,"Bak}t");
   if ( !wpgrst("varkon.level.last",last) )     strcpy(last,"Sista niv}");
   if ( !wpgrst("varkon.input.okey",okey) )     strcpy(okey,"Okej");
   if ( !wpgrst("varkon.input.reject",reject) ) strcpy(reject,"Avbryt");
   if ( !wpgrst("varkon.input.help",help) )     strcpy(help,"Hj{lp");
/*
***Längsta texten avgör fönstrets bredd.
*/
   altlen = 0;

   if ( wpstrl(first) > altlen ) altlen = wpstrl(first);
   if ( wpstrl(add)   > altlen ) altlen = wpstrl(add);
   if ( wpstrl(last)  > altlen ) altlen = wpstrl(last);
   altlen += wpstrl("1999");

   if ( wpstrl(rubrik) > altlen ) altlen = wpstrl(rubrik);
   if ( wpstrl(forw)   > altlen ) altlen = wpstrl(forw);
   if ( wpstrl(back)   > altlen ) altlen = wpstrl(back);

   if ( wpstrl(okey) + wpstrl(reject) + wpstrl(help) + 45 > altlen )
     altlen = wpstrl(okey) + wpstrl(reject) + wpstrl(help) + 45;
/*
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/
   ly   = (short)(0.8*wpstrh());
   alth = (short)(1.6*wpstrh()); 
   lm   = (short)(1.4*wpstrh());

   main_dx = (short)(ly + altlen + 2*lm + ly);
   main_dy = (short)(ly + 4*(alth + ly) +  ly + 2*wpstrh() + ly);  
/*
***Skapa själva inmatningsfönstret som ett WPIWIN.
*/
   wpposw(x,y,main_dx+10,main_dy+25,&main_x,&main_y);
   wpwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Ramlös button för första nivå.
*/
   alt_x  = ly;
   alt_y  = ly;
   altlen = wpstrl(first);
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
             (short)alth,(short)0,first,first,"",WP_BGND,WP_FGND,&dum_id);
/*
***Edit för första nivå.
*/
   altlen = wpstrl("1999") + 15;
   alt_x  = main_dy - altlen -ly;
   sprintf(valstr,"%d",org_first);
   wpmced((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                                  (short)alth,(short)1,valstr,4,&first_id);
   act_first = org_first;
/*
***Ramlös button för antal ytterligare nivåer.
*/
   alt_x  = ly;
   alt_y  = ly + alth + ly;
   altlen = wpstrl(add);
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                   (short)alth,(short)0,add,add,"",WP_BGND,WP_FGND,&dum_id);
/*
***Edit för antal ytterligare nivåer.
*/
   altlen = wpstrl("1999") + 15;
   alt_x  = main_dy - altlen -ly;
   sprintf(valstr,"%d",org_add);
   wpmced((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                                    (short)alth,(short)1,valstr,4,&add_id);
   act_add = org_add;
/*
***Toggle-button för framåt/bakåt.
*/
   alt_y = ly + 2*(alth+ly);
   if ( wpstrl(forw) > wpstrl(back) ) altlen = wpstrl(forw) + 15;
   else                               altlen = wpstrl(back) + 15;
   alt_x  = main_dy - altlen -ly;

   if ( org_forw )
     wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                  (short)alth,(short)1,forw,back,"",WP_BGND,WP_FGND,&fb_id);
   else
     wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                  (short)alth,(short)1,back,forw,"",WP_BGND,WP_FGND,&fb_id);
   act_forw = org_forw;
/*
***Ramlös button för sista nivå.
*/
   alt_x  = ly;
   alt_y  = ly + 3*(alth + ly);
   altlen = wpstrl(last);
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                  (short)alth,(short)0,last,last,"",WP_BGND,WP_FGND,&dum_id);
/*
***Edit för sista nivå.
*/
   altlen = wpstrl("1999") + 15;
   alt_x  = main_dy - altlen -ly;
   sprintf(valstr,"%d",org_last);
   wpmced((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,
                                    (short)alth,(short)1,valstr,4,&last_id);
   act_last = org_last;
/*
***Okey, avbryt och hjälp.
*/
   alt_x  = ly;
   alt_y  = ly + 4*(alth + ly) + ly;
   alth   = 2*wpstrh();
   altlen = wpstrl(okey) + 15;
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,(short)alth,
                   (short)2,okey,okey,"",WP_BGND,WP_FGND,&okey_id);

   alt_x  = alt_x + altlen + lm;
   altlen = wpstrl(reject) + 15;
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,(short)alth,
                   (short)2,reject,reject,"",WP_BGND,WP_FGND,&reject_id);

   altlen = wpstrl(help) + 15;
   alt_x  = main_dx - altlen - lm;
   wpmcbu((wpw_id)iwin_id,(short)alt_x,(short)alt_y,(short)altlen,(short)alth,
                   (short)2,help,help,"",WP_BGND,WP_FGND,&help_id);
/*
***Klart för visning.
*/
   wpwshw(iwin_id);
/*
***Fixa C-pekare till add_id och last_id.
*/
   winptr = wpwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
   frstpt = (WPEDIT *)iwinpt->wintab[first_id].ptr;
   addpt  = (WPEDIT *)iwinpt->wintab[add_id].ptr;
   forwpt = (WPBUTT *)iwinpt->wintab[fb_id].ptr;
   lastpt = (WPEDIT *)iwinpt->wintab[last_id].ptr;
/*
***Vänta på action.
*/
loop:
   wpwwtw(iwin_id,SLEVEL_ALL,&but_id);
/*
***Ok.
*/
   if ( but_id == okey_id )
     {
     org_first = act_first;
     org_add   = act_add;
     org_forw  = act_forw;
     org_last  = act_last;

     wpmtsl(gwinpt->id.w_id,act_first,act_last,mode);
     }
/*
***Avbryt.
*/
   else if ( but_id == reject_id )
     {
     wpwdel(iwin_id);
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
***Första.
*/
   else if ( but_id == first_id )
     {
     wpgted(iwin_id,first_id,valstr);
     if ( sscanf(valstr,"%d",&act_first) != 1  ||
          act_first < 0                        ||
          act_first > 1999 )
       {
       wpfoed(addpt,FALSE);
       wpxped(addpt);
       wpfoed(frstpt,TRUE);
       wpxped(frstpt);
       XBell(xdisp,100);
       goto loop;
       }

     if ( act_forw ) act_last = act_first + act_add;
     else            act_last = act_first - act_add;

     if ( act_last < 0 )    act_last = 0;
     if ( act_last > 1999 ) act_last = 1999;

     sprintf(valstr,"%d",act_last);
     strcpy(lastpt->str,valstr);
     lastpt->scroll = lastpt->curpos = 0;
     lastpt->fuse = TRUE;
     XClearWindow(xdisp,lastpt->id.x_id);
     wpxped(lastpt);

     goto loop;
     }
/*
***Antal ytterligare.
*/
   else if ( but_id == add_id )
     {
     wpgted(iwin_id,add_id,valstr);
     if ( sscanf(valstr,"%d",&act_add) != 1  ||
          act_add < 0                        ||
          act_add > 1998 )
       {
       wpfoed(lastpt,FALSE);
       wpfoed(addpt,TRUE);
       XBell(xdisp,100);
       goto loop;
       }

     if ( act_forw ) act_last = act_first + act_add;
     else            act_last = act_first - act_add;

     if ( act_last < 0 )    act_last = 0;
     if ( act_last > 1999 ) act_last = 1999;

     sprintf(valstr,"%d",act_last);
     strcpy(lastpt->str,valstr);
     lastpt->scroll = lastpt->curpos = 0;
     lastpt->fuse = TRUE;
     XClearWindow(xdisp,lastpt->id.x_id);
     wpxped(lastpt);
     goto loop;
     }
/*
***Framåt/bakåt.
*/
   else if ( but_id == fb_id )
     {
     if ( act_forw ) act_forw = FALSE;
     else            act_forw = TRUE;

     if ( act_forw ) act_last = act_first + act_add;
     else            act_last = act_first - act_add;

     if ( act_last < 0 )    act_last = 0;
     if ( act_last > 1999 ) act_last = 1999;

     sprintf(valstr,"%d",act_last);
     strcpy(lastpt->str,valstr);
     lastpt->scroll = lastpt->curpos = 0;
     lastpt->fuse = TRUE;
     XClearWindow(xdisp,lastpt->id.x_id);
     wpxped(lastpt);
     goto loop;
     }
/*
***Sista.
*/
   else if ( but_id == last_id )
     {
     wpgted(iwin_id,last_id,valstr);
     if ( sscanf(valstr,"%d",&act_last) != 1  ||
          act_last < 0                        ||
          act_last > 1999 )
       {
       wpfoed(frstpt,FALSE);
       wpfoed(lastpt,TRUE);
       XBell(xdisp,100);
       goto loop;
       }

     if ( act_forw ) act_add = act_last - act_first;
     else            act_add = act_first - act_last;

     if ( act_add < 0 )
       {
       if ( act_forw )
         {
         act_forw = FALSE;
         if ( forwpt->status ) forwpt->status = FALSE;
         else                  forwpt->status = TRUE;
         }
       else           
         {
         act_forw = TRUE;
         if ( forwpt->status ) forwpt->status = FALSE;
         else                  forwpt->status = TRUE;
         }
      
       XClearWindow(xdisp,forwpt->id.x_id);
       wpxpbu(forwpt); 
       act_add = -act_add;
       }

     sprintf(valstr,"%d",act_add);
     strcpy(addpt->str,valstr);
     addpt->scroll = addpt->curpos = 0;
     addpt->fuse = TRUE;
     XClearWindow(xdisp,addpt->id.x_id);
     wpxped(addpt);
     goto loop;
     }
/*
***Nu är det dags att lägga av.
*/
   wpwdel(iwin_id);
/*
***Om det är V3:s huvudfönster skall även grapac underrättas.
*
   if ( gwinpt->id.w_id == GWIN_MAIN ) wpupgp(gwinpt);
*/

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpmtsl(
        v2int win_id,
        int   first,
        int   last,
        int   mode)

/*      Tänder/Släcker/Listar nivåer.
 *
 *      In: win_id = ID för aktuellt fönster.
 *          first  = Första nivå att lista.
 *          last   = Sista nivå att lista.
 *          mode   = WP_BLANKL  => Släck nivåer.
 *                   WP_UBLANKL => Tänd nivåer.
 *                   WP_LISTL   => Lista nivåer.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1392 = Fönster med id %s finns ej
 *
 *      (C)microform ab 16/1/95 J. Kjellander
 *
 *      1999-03-22 mode ej deklarerad R. Svedin
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
   if ( (winptr=wpwgwp((wpw_id)win_id)) != NULL  &&
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
   if ( first <= last )
     {
     start = first;
     stop  = last + 1;
     inc   = 1;
     }
   else
     {
     start =  first;
     stop  =  last - 1;
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
     wpinla("");

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


       wpalla(buf,(short)1);
       }

     wpexla(TRUE);
     break;
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpgtvi(
        v2int   win_id,
        char   *vynamn,
        double *skala,
        double *xmin,
        double *ymin,
        double *xmax,
        double *ymax,
        DBTmat *vymat,
        double *persp)

/*      X-versionen av MBS-GET_VIEW.
 *
 *      In: win_id = ID för aktuellt fönster.
 *
 *      Ut: *vynamn = Vy:ns namn.
 *          *skala  = Aktuell skala.
 *          *xmin   = Aktuellt modellfönster.
 *          *ymin   =         -""-
 *          *xmax   =         -""-
 *          *ymax   =         -""-
 *          *vymat  = Aktuell 3D-transformation.
 *          *persp  = Perspektivavstånd.
 *
 *      Felkod: WP1402 = Fönster med id %s finns ej
 *
 *      (C)microform ab 16/1/95 J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[81];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Fixa C-pekare till WPGWIN-fönstret.
*/
   if ( (winptr=wpwgwp((wpw_id)win_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1402",errbuf));
     }
/*
***Returnera vydata.
*/
   strcpy(vynamn,gwinpt->vy.vynamn);

  *skala = gwinpt->geo.psiz_x*
          (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) /
          (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);

  *xmin  = gwinpt->vy.modwin.xmin;
  *ymin  = gwinpt->vy.modwin.ymin;
  *xmax  = gwinpt->vy.modwin.xmax;
  *ymax  = gwinpt->vy.modwin.ymax;

   vymat->g11 = gwinpt->vy.vymat.k11;
   vymat->g12 = gwinpt->vy.vymat.k12;
   vymat->g13 = gwinpt->vy.vymat.k13;
   vymat->g14 = 0.0;

   vymat->g21 = gwinpt->vy.vymat.k21;
   vymat->g22 = gwinpt->vy.vymat.k22;
   vymat->g23 = gwinpt->vy.vymat.k23;
   vymat->g24 = 0.0;

   vymat->g31 = gwinpt->vy.vymat.k31;
   vymat->g32 = gwinpt->vy.vymat.k32;
   vymat->g33 = gwinpt->vy.vymat.k33;
   vymat->g34 = 0.0;

   vymat->g41 = 0.0;
   vymat->g42 = 0.0;
   vymat->g43 = 0.0;
   vymat->g44 = 1.0;
  
  *persp = gwinpt->vy.vydist;

   return(0);
  }

/********************************************************/
/*!******************************************************/

  static void drwbox(
         Drawable win,
         GC       gc,
         int      ix1,
         int      iy1,
         int      ix2,
         int      iy2,
         int      mode)

/*      Ritar/suddar gummibands-rektangel etc.
 *
 *      In:  win    = X-id för fönster att rita i.
 *           gc     = GC att använda för ritning.
 *           ix1    = Hörn 1 X-koordinat.
 *           iy1    = Hörn 1 Y-koordinat.
 *           ix2    = Hörn 2 X-koordinat.
 *           iy2    = Hörn 2 Y-koordinat.
 *           mode   = 1 => Rektangel
 *                    2 => Linje
 *
 *      Ut:  Inget.
 *
 *      (C)microform ab 12/12/94 J. Kjellander
 *
 *      1/3-95  Bug, dx/dy < 0, J. Kjellander
 *      1998-03-23 mode, J.Kjellander
 *
 ******************************************************!*/

  {
   int          x1,y1,x2,y2,dx,dy;
   unsigned int abs_dx,abs_dy;
   double       alfa,cosalf,sinalf;

/*
***Vilken sorts figur ?
*/
   switch ( mode )
     {
     case WP_RUB_NONE:
     return;
/*
***Rektangel, hur stor ?
*/
     case WP_RUB_RECT:
     dx = ix2 - ix1;
     dy = iy2 - iy1;
/*
***Om dx eller dy är negativ måste vi fixa lite eftersom
***alla X-servrar inte klarar detta.
*/
     if ( dx < 0 ) x1 = ix2, x2 = ix1;
     else          x1 = ix1, x2 = ix2;

     if ( dy < 0 ) y1 = iy2, y2 = iy1;
     else          y1 = iy1, y2 = iy2;

     abs_dx = abs(dx);
     abs_dy = abs(dy);
     XDrawRectangle(xdisp,win,gc,x1,y1,abs_dx,abs_dy);
     break;
/*
***Linje är enkelt.
*/
     case WP_RUB_LINE:
     XDrawLine(xdisp,win,gc,ix1,iy1,ix2,iy2);
     break;
/*
***Pil, beräkna vinkeln mellan gummibandslinjen och positiva
***X-axeln.
*/
     case WP_RUB_ARROW:
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
     XDrawLine(xdisp,win,gc,ix1,iy1,ix2,iy2);
/*
***Pilspetsen
*/
     cosalf = cos(alfa);
     sinalf = sin(alfa);

     dx = (int)(-25.0*cosalf + 10.0*sinalf);
     dy = (int)(-10.0*cosalf - 25.0*sinalf);
     XDrawLine(xdisp,win,gc,ix2,iy2,ix2+dx,iy2+dy);

     dx = (int)(-25.0*cosalf - 10.0*sinalf);
     dy = (int)( 10.0*cosalf - 25.0*sinalf);
     XDrawLine(xdisp,win,gc,ix2,iy2,ix2+dx,iy2+dy);
     break;
     }

   XFlush(xdisp);
  }

/********************************************************/
/*!******************************************************/

  static void setgwm(
         long eventmask)

/*      Sätter en viss eventmask på ALLA grafiska
 *      fönster.
 *
 *      In:  eventmask = Önskad mask, tex. GWEM_NORM
 *                                         GWEM_RUB
 *
 *      (C)microform ab 1998-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;
   WPGWIN *gwinpt;

/*
***Leta upp alla WPGWIN.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
         XSelectInput(xdisp,gwinpt->id.x_id,eventmask);
         }
       }
     }
  }

/********************************************************/
