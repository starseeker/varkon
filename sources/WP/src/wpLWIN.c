/**********************************************************************
*
*    wpLWIN.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPinla();     Init list window (WPLWIN)
*    WPalla();     Add lines to WPLWIN
*    WPexla();     Exit list window
*
*    WPxplw();     Expose routine for WPLWIN
*    WPbtlw();     Button routine for WPLWIN
*    WPcrlw();     Crossing routine for WPLWIN
*    WPcmlw();     ClientMessage routine for WPLWIN
*    WPcolw();     Configure routine for WPLWIN
*    WPdllw();     Delete WPLWIN
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
#include "../../EX/include/EX.h"
#include "../include/WP.h"

/*
***Since Varkon is compiled with -ansi and -Wall we will
***get compiler warnings about the two following functions
***if we don't declare them here. They are not ansi but POSIX
***and seem to exist in all Unix implementations. The reason
***for using them is that they are reccommended by the compiler
***to be the most secure way to create a temporary file.
*/
extern int   mkstemp();
extern FILE *fdopen();

/*
***External Varkon global variables.
*/
extern char  jobdir[],jobnam[];

/*
***Static variables.
*/
static WPLWIN  *actlwin = NULL;

/*
***actlwin is a C ptr to a WPLWIN which has been created (WPinla)
***but not yet closed (WPexla). If actlwin = NULL no WPLWIN is
***open but there may exist many closed WPLWIN.
*/

static char title [V3STRLEN];   /* Current window border title  */

/*
***Internal functions.
*/
static short crlwin(int,int,int,int,char *wtitle);
static short savelw(WPLWIN *lwinpt);

/********************************************************/

        short WPinla(char *hs)

/*      Create (open) a WPLWIN. LST_INI(title) in MBS.
 *
 *      In: hs = Header string (title).
 *
 *      (C)microform ab 22/7/92 U. Andersson
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      8/11/94  Resurser för texterna, J. Kjellander
 *      1996-04-25 6 st. "X", J. Kjellander
 *      1997-01-15 IGgenv(), J.Kjellander
 *      2007-10-30 Slidebars, J.Kjellander
 *      2007-12-14 mkstemp(), J.Kjellander
 *
 ******************************************************!*/

  {
    char     tmpnam[V3PTHLEN+JNLGTH+1],tmpbuf[V3STRLEN+1];
    wpw_id   id;
    int      tmp_fd,dx;
    WPLWIN  *lwinptr;
    FILE    *fp;

/*
***Check that another WPLWIN is not already opened.
*/
    if ( actlwin != NULL )
      {
      WPexla(TRUE);
      return(-2);
      }
/*
***Create a temporary file for list contents. For security reasons,
***use mkstemp() and fdopen() which are not ansi but POSIX and seem
***to be available everywhere.
*/
    strncpy(tmpnam,IGgenv(VARKON_TMP),V3PTHLEN+1);
    strncat(tmpnam,jobnam,JNLGTH+1);
    strncat(tmpnam,".XXXXXX",8);
    tmp_fd = mkstemp(tmpnam);

    if ( (fp=fdopen(tmp_fd,"w+")) == NULL ) return(-2);
/*
***Get text resources from the ini-file.
*/
    if ( !WPgrst("varkon.list.title",title) ) strcpy(title," ");
    if (  WPgrst("varkon.list.title.jobnam",tmpbuf)  &&
          strcmp(tmpbuf,"True") == 0 ) strcat(title,jobnam);
/*
***Allocate a window-ID.
*/
    if ( (id=WPwffi()) < 0 ) return(-2);
/*
***Create an opened WPLWIN. Note that there is still
***no X window created. This is done when the WPLWIN
***is closed.
*/
    if ( (lwinptr=(WPLWIN *)v3mall(sizeof(WPLWIN),"WPinla"))
                                                   == NULL ) return(-2);
    lwinptr->id.w_id = id;
    lwinptr->id.p_id = 0;
    lwinptr->id.x_id = 0;

    lwinptr->geo.x   = 0;
    lwinptr->geo.y   = 0;
    lwinptr->geo.dx  = 0;
    lwinptr->geo.dy  = 0;

    lwinptr->wintab[0].typ = TYP_UNDEF;
    lwinptr->wintab[0].ptr = NULL;
    lwinptr->wintab[1].typ = TYP_UNDEF;
    lwinptr->wintab[1].ptr = NULL;

    strcpy(lwinptr->filnam,tmpnam);
    lwinptr->filpek  = fp;
    strcpy(lwinptr->rubrik,hs);
/*
***Calculate minimum window width.
*/
    dx = WPstrl(title);

    if ( WPstrl(hs) > dx ) dx = WPstrl(hs);

    lwinptr->maxchars = 0;
    lwinptr->maxlen   = dx;

    lwinptr->nl_first = 0;    /* First visible line */
    lwinptr->nl_tot   = 0;    /* Total number of lines */
    lwinptr->nl_vis   = 0;    /* Number of lines in window */
/*
***Enter opened WPLWIN in global window table.
*/
    wpwtab[id].typ = TYP_LWIN;
    wpwtab[id].ptr = (char *)lwinptr;
/*
***This WPLWIN is now opened but not closed.
*/
    actlwin = lwinptr;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        short WPalla(
        char *s,
        short rs)

/*      Add a line to the currently open WPLWIN.
 *
 *      In: s  = Text string.
 *          rs = Line space.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *      931207 Omarbetad, J. Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    int lt,i,rest,nstkn;
    char mell[V3STRLEN+1],rad[V3STRLEN+1];

/*
***An open WPLWIN must exist.
*/
    if ( actlwin == NULL ) return(-2);
/*
***Check line length. Truncate at V3STRLEN.
*/
    nstkn = strlen(s);
    if ( nstkn > V3STRLEN )
      {
      s[V3STRLEN] = '\0';
      nstkn = V3STRLEN;
      }

    if ( nstkn > actlwin->maxchars ) actlwin->maxchars = nstkn;
/*
***Line length in pixels.
*/
    lt = WPstrl(s);
    if ( lt > actlwin->maxlen ) actlwin->maxlen = lt;
/*
***If line is shorter than V3STRLEN, pad with space.
*/
    rest  = V3STRLEN - nstkn; 
    for ( i=0; i<rest; i++ ) mell[i] = ' ';
    mell[rest] = '\0';
    strcpy(rad,s);
    strcat(rad,mell);
    fprintf(actlwin->filpek,"%s\n",rad);
/*
***Update line counter.
*/
  ++actlwin->nl_tot;
/*
***Optional line spacing.
*/
    if ( rs > 1 )
      {
      for (i=0;i<V3STRLEN;i++) mell[i] = ' ';
      mell[V3STRLEN] = '\0';

      for ( i=0; i<rs - 1; i++ )
        {
      ++actlwin->nl_tot;
        fprintf(actlwin->filpek,"%s\n",mell);
        }
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

        short WPexla(
        bool  show)

/*      Close an open WPLWIN.
 *
 *      In: show = TRUE  => Display.
 *                 FALSE => Don't display.
 *
 *      (C)microform ab 22/7/92 U. Andersson 
 *
 *       8/12/93    Omarbetad, J. Kjellander
 *       7/11/94    Resurser för placering, J. Kjellander
 *       1996-02-05 show, J. Kjellander
 *       2007-20-28 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    int      x,y,maxnl,nl,a,b,dx,dy,sx,sy,sdx,sdy,end;
    unsigned int dum1,dum2;
    double   c,d;
    char    *type[20];
    XrmValue value;
    WPWIN   *winptr;
    WPSBAR  *sbptr;

/*
***A WPLWIN must be open.
*/
   if ( actlwin == NULL ) return(-2);
/*
***Close temporary file.
*/
   fclose(actlwin->filpek);
/*
***If no show it's simple. Remove temp. file, and
***kill open WPLWIN created by WPinla().
*/
   if ( !show)
     {
     IGfdel(actlwin->filnam);
     if ( (winptr=WPwgwp(actlwin->id.w_id)) != NULL )
       {
       winptr->typ = TYP_UNDEF;
       winptr->ptr = NULL;
       v3free((char *)actlwin,"WPexla");
       }
     actlwin = NULL;
     return(0);
     }
/*
***WPLWIN height.
*/
    maxnl = (int)(0.6*DisplayHeight(xdisp,xscr)/WPstrh()); /* Max number of lines */
    nl = actlwin->nl_tot;

    if ( nl > maxnl ) nl = maxnl;

    dy = WPstrh()*(nl + 5);
/*
***WPLWIN width.
*/
    dx = 2*WPstrh() + actlwin->maxlen + WPstrh();
    if ( dx > 0.8*DisplayWidth(xdisp,xscr) ) dx = 0.8*DisplayWidth(xdisp,xscr);
/*
***WPLWIN position.
*/
    x  = 90;
    y  = DisplayHeight(xdisp,xscr) - dy - 50; 

    if ( XrmGetResource(xresDB,"varkon.list.geometry","Varkon.List.Geometry",
         type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Create the window in X.
*/
    crlwin(x,y,dx,dy,title);
/*
***What size did it get (after possible fight with a WM)?
*/
    WPgwsz(actlwin->id.x_id,&a,&b,&dx,&dy,&c,&d);

    actlwin->geo.dx = dx;
    actlwin->geo.dy = dy;
/*
***How many lines visible.
*/
    actlwin->nl_vis = dy/WPstrh() - 4;
/*
***Add optional vertical slidebar.
*/
    if ( actlwin->nl_vis < actlwin->nl_tot )
      {
      sx  = actlwin->geo.dx-WPstrh();
      sy  = 0;
      sdx = WPstrh();
      sdy = actlwin->geo.dy-WPstrh();
      end = (double)((double)actlwin->nl_vis/(double)actlwin->nl_tot)*sdy;
      if ( end < 15 ) end = 15;

      WPcreate_slidebar(actlwin->id.w_id,sx,sy,sdx,sdy,0,end,WP_SBARV,&sbptr);

      sbptr->id.p_id         = actlwin->id.w_id;
      sbptr->cback           = WPxplw;
      actlwin->wintab[0].typ = TYP_SBAR;
      actlwin->wintab[0].ptr = (char *)sbptr;
      }
/*
***Add optional horizontal slidebar.
*/
    if ( actlwin->maxlen > actlwin->geo.dx - 2*WPstrh() )
      {
      sx  = 0;
      sy  = actlwin->geo.dy-WPstrh();
      sdx = actlwin->geo.dx-WPstrh();
      sdy = WPstrh();
      end = ((double)(sdx - 2*WPstrh())/(double)actlwin->maxlen)*(sdx - 2*WPstrh());
      if ( end < 15 ) end = 15;

      WPcreate_slidebar(actlwin->id.w_id,sx,sy,sdx,sdy,0,end,WP_SBARH,&sbptr);

      sbptr->id.p_id         = actlwin->id.w_id;
      sbptr->cback           = WPxplw;
      actlwin->wintab[1].typ = TYP_SBAR;
      actlwin->wintab[1].ptr = (char *)sbptr;
      }
/*
***Display.
*/
    WPwshw(actlwin->id.w_id);
/*
***The open WPLWIN is now closed.
*/
    actlwin = NULL;
/*
***The end.
*/
    return(0);
  }
/********************************************************/
/********************************************************/

        bool    WPxplw(
        WPLWIN *lwinpt)

/*      Expose handler for WPLWIN.
 *
 *      In: lwinpt = C-pekare till WPLWIN.
 *
 *      (C)microform ab 11/7/92 U. Andersson.
 *
 *      7/12/93  Omarbetad, J. Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *      2007-10-30 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
   char    rad[V3STRLEN+3];
   int     j,tx,ty,a,b,px,py,butsiz,dx,nskip;
   double  c,d;
   WPSBAR *sbptr;

/*
***Colors.
*/
   XSetBackground(xdisp,xgc,WPgcol(0)); /*WP_BGND1));*/
   XSetForeground(xdisp,xgc,WPgcol(1));
/*
***Current WPLWIN size.
*/
   WPgwsz(lwinpt->id.x_id,&a,&b,&px,&py,&c,&d);
/*
***How many lines are possible to display ?
*/
   lwinpt->nl_vis = py/WPstrh() - 4;
/*
***Optional vertical slidebar status.
*/
   sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;

   if ( (lwinpt->nl_tot > lwinpt->nl_vis) && (sbptr != NULL) )
     {
     butsiz = sbptr->butend - sbptr->butstart;
     if ( sbptr->butstart == 0 )
       {
       lwinpt->nl_first = 0;
       }
     else
       {
       lwinpt->nl_first = ((double)sbptr->butstart/
                          ((double)(sbptr->geo.dy - butsiz))*
                           (lwinpt->nl_tot - lwinpt->nl_vis));
       }

     if ( lwinpt->nl_first > (lwinpt->nl_tot - lwinpt->nl_vis) )
       lwinpt->nl_first = lwinpt->nl_tot - lwinpt->nl_vis - 1;
     }
/*
***Optional horizontal slidebar status.
*/
   sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;
   dx = lwinpt->geo.dx - 2*WPstrh();

   if ( sbptr != NULL ) dx -= WPstrh();

   if ( ( lwinpt->maxlen > dx ) && (sbptr != NULL) )
     {
     butsiz = sbptr->butend - sbptr->butstart;
     nskip = ((double)sbptr->butstart/((double)(dx - butsiz)))*
             ((double)(lwinpt->maxlen - dx)/(double)lwinpt->maxlen)*
             lwinpt->maxchars;
     }
   else nskip = 0;
/*
***Current Y position in pixels.
*/
   tx = ty = 2*WPstrh();
/*
***Display header string.
*/
   WPwstr(lwinpt->id.x_id,tx,ty,lwinpt->rubrik);
/*
***Empty line.
*/
   ty = ty + WPstrh();
/*
***Open list file.
*/
   lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Position to first visible line.
*/
   fseek(lwinpt->filpek,lwinpt->nl_first*(V3STRLEN+1),SEEK_SET);
/*
***Read lines and display.
*/
   for ( j=0; j<lwinpt->nl_vis; ++j )
     {
     if ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL )
       {
       ty +=  WPstrh();
       rad[V3STRLEN] = '\0';
       if ( strlen(rad) > nskip ) WPwstr(lwinpt->id.x_id,tx,ty,&rad[nskip]);
       }
     else break;
     }
/*
***Close the list file.
*/
   fclose(lwinpt->filpek); 
/*
***Expose optional slidebars.
*/
   sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;
   if ( sbptr != NULL ) WPexpose_slidebar(sbptr);

   sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;
   if ( sbptr != NULL ) WPexpose_slidebar(sbptr);
/*
***The end.
*/
   return(0);
 }

/*********************************************************/
/********************************************************/

        bool          WPbtlw(
        WPLWIN       *lwinpt,
        XButtonEvent *butev,
        wpw_id       *serv_id)

/*      Button handler for WPLWIN.
 *
 *      In: iwinpt    = C ptr to WPLWIN.
 *          butev     = C ptr to X-event.
 *
 *      Out: *serv_id = ID of window that served the event.
 *
 *      Return: TRUE =  Event served.
 *              FALSE = This window (with subwindows) not involved.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 *      1997-01-16 Bug, butptr=NULL, J.Kjellander
 *      2007-10-30 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
   bool    hit;
   WPSBAR *sbptr;

/*
***Mouse event in the actual WPLWIN ?
*/
    if ( butev->window == lwinpt->id.x_id  &&
         butev->type   == ButtonPress )
      {
      switch ( butev->button )
        {
/*
****Rightclick = Save/Print.
*/
        case 3:
        savelw(lwinpt);
       *serv_id = lwinpt->id.w_id;
        return(TRUE);
/*
***Something else should not be possible.
*/
        default:
        WPbell();
        return(FALSE);
        }
      return(TRUE);
      }
/*
***Optional vertical slidebar.
*/
    hit = FALSE;
    sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;

    if ( sbptr != NULL  &&  butev->window == sbptr->id.x_id )
      {
      WPbutton_slidebar(sbptr,butev);
     *serv_id = lwinpt->id.w_id;
      hit = TRUE;
      }
/*
***Optional horizontal slidebar.
*/
    sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;

    if ( sbptr != NULL  &&  butev->window == sbptr->id.x_id )
      {
      WPbutton_slidebar(sbptr,butev);
     *serv_id = lwinpt->id.w_id;
      hit = TRUE;
      }
/*
***The end.
*/
    return(hit);
  }

/********************************************************/
/*!******************************************************/

        bool            WPcrlw(
        WPLWIN         *lwinpt,
        XCrossingEvent *croev)

/*      Crossing handler for WPLWIN with sub windows.
 *      (Not currently needed, maybe later... )
 *
 *      In: lwinpt = C ptr to WPLWIN.
 *
 *      Return: TRUE  => Event served.
 *              FALSE => Not this window.
 *
 *      (C)2007-10-18 J. Kjellander
 *
 ******************************************************!*/

  {
    return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool                  WPcmlw(
        WPLWIN               *lwinpt,
        XClientMessageEvent  *clmev)

/*      ClientMessage handler for WPLWIN.
 *
 *      In: iwinpt  = C ptr to WPLWIN.
 *          clmev   = C ptr to event.
 *
 *      FV: TRUE  = Eventet served.
 *          FALSE = Not this window.
 *
 *      (C)microform ab 6/12/93 J. Kjellander
 *
 ******************************************************!*/

  {
/*
*** If it's a WM_DELETE_WINDOW, delete the window.
*/
   if ( clmev->message_type ==
        XInternAtom(xdisp,"WM_PROTOCOLS",False) &&
        clmev->data.l[0]    ==
        XInternAtom(xdisp,"WM_DELETE_WINDOW",False) )
     {
     WPwdel((DBint)lwinpt->id.w_id);
     return(TRUE);
     }
   else return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool             WPcolw(
        WPLWIN          *lwinpt,
        XConfigureEvent *conev)

/*      Configure handler for WPLWIN. Handles user
 *      resize.
 *
 *      In: lwinpt = C-ptr to WPLWIN.
 *          conev  = C-ptr to ConfigureEvent.
 *
 *      Return: TRUE.
 *
 *      (C)2007-10-21 J.Kjellander
 *
 ******************************************************!*/

  {
   int     olddx,olddy,newdx,newdy,ddx,ddy,end;
   XEvent  event;
   WPSBAR *sbptr;

/*
***To prevent multiple updates of the window during resize
***by the user, remove pending StructureNotify events.
*/
  while ( XCheckMaskEvent(xdisp, StructureNotifyMask, &event)  &&
          event.type == ConfigureNotify)
    V3MOME(&event.xconfigure,conev,sizeof(XConfigureEvent));
/*
***Old size.
*/
   olddx = lwinpt->geo.dx; olddy = lwinpt->geo.dy;
/*
***What is the new size ?
*/
   newdx = conev->width;
   newdy = conev->height;
/*
***How big is the change ?
*/
   ddx = newdx - olddx; ddy = newdy - olddy;
/*
***The window changed size i X direction.  Update
***window geometry and optional slidebars.
*/
   if ( ddx != 0.0 )
     {
     lwinpt->geo.dx = newdx;
/*
***A vertical slidebar needs to be moved in the X direction.
*/
     sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;

     if ( sbptr != NULL )
       {
       sbptr->geo.x = newdx - WPstrh();
       XMoveWindow(xdisp,sbptr->id.x_id,sbptr->geo.x,0);
       }
/*
***A horizontal slidebar may have to be resized or deleted.
*/
     sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;

     if ( sbptr != NULL )
       {
       if ( lwinpt->geo.dx < lwinpt->maxlen + 3*WPstrh() )
         {
         sbptr->geo.dx = newdx - WPstrh();
         sbptr->butstart = 0;
         sbptr->butend = ((double)(lwinpt->geo.dx - 3*WPstrh())/
                          (double)lwinpt->maxlen)*
                          (lwinpt->geo.dx - 3*WPstrh());
         if ( sbptr->butend < 15 ) sbptr->butend = 15;
         XResizeWindow(xdisp,sbptr->id.x_id,sbptr->geo.dx,WPstrh());
         }
       else
         {
         XDestroyWindow(xdisp,sbptr->id.x_id);
         v3free((char *)sbptr,"WPcolw");
         lwinpt->wintab[1].ptr = NULL;
         }
       }
/*
***If there is no horizontal slidebar, one may have to be added.
*/
     else if ( lwinpt->geo.dx < lwinpt->maxlen + 3*WPstrh() )
       {
       end = ((double)(lwinpt->geo.dx - 3*WPstrh())/(double)lwinpt->maxlen)*
              (lwinpt->geo.dx - 3*WPstrh());
       if ( end < 15 ) end = 15;
       WPcreate_slidebar(lwinpt->id.w_id,
                         0,lwinpt->geo.dy-WPstrh(),
                         lwinpt->geo.dx-WPstrh(),WPstrh(),
                         0,end,WP_SBARH,&sbptr);
       lwinpt->wintab[1].ptr = (char *)sbptr;
       sbptr->id.p_id = lwinpt->id.w_id;
       sbptr->cback   = WPxplw;
       XMapWindow(xdisp,sbptr->id.x_id);
       }
     }
/*
***The window changed size i Y direction.  Update
***window geometry and optional slidebars.
*/
   if ( ddy != 0.0 )
     {
     lwinpt->geo.dy = newdy;
     sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;
/*
***A horizontal slidebar needs to be moved in the Y direction.
*/
     if ( sbptr != NULL )
       {
       sbptr->geo.y = newdy - WPstrh();
       XMoveWindow(xdisp,sbptr->id.x_id,0,sbptr->geo.y);
       }
/*
***A vertical slidebar may have to be resized or deleted.
*/
     sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;

     if ( sbptr != NULL )
       {
       if ( lwinpt->nl_vis < lwinpt->nl_tot )
         {
         sbptr->geo.dy = newdy - WPstrh();
         sbptr->butstart = 0;
         sbptr->butend = (double)((double)lwinpt->nl_vis/
                                   (double)lwinpt->nl_tot)*sbptr->geo.dy;
         if ( sbptr->butend < 15 ) sbptr->butend = 15;
         XResizeWindow(xdisp,sbptr->id.x_id,WPstrh(),sbptr->geo.dy);
         }
       else
         {
         XDestroyWindow(xdisp,sbptr->id.x_id);
         v3free((char *)sbptr,"WPcolw");
         lwinpt->wintab[0].ptr = NULL;
         }
       }
/*
***If there is no vertical slidebar, one may have to be added.
*/
     else if ( lwinpt->nl_vis < lwinpt->nl_tot )
       {
       end = (double)((double)lwinpt->nl_vis/
             (double)lwinpt->nl_tot)*lwinpt->geo.dy-WPstrh();
       if ( end < 15 ) end = 15;
       WPcreate_slidebar(lwinpt->id.w_id,
                         lwinpt->geo.dx-WPstrh(),0,
                         WPstrh(),lwinpt->geo.dy-WPstrh(),
                         0,end,WP_SBARV,&sbptr);
       lwinpt->wintab[0].ptr = (char *)sbptr;
       sbptr->id.p_id = lwinpt->id.w_id;
       sbptr->cback   = WPxplw;
       XMapWindow(xdisp,sbptr->id.x_id);
       }
     }
/*
***Expose the WPLWIN.
*/
   WPxplw(lwinpt);
/*
***The end.
*/
   return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short   WPdllw(
        WPLWIN *lwinpt)

/*      Kill a WPLWIN window.
 *
 *      In: lwinpt = C-ptr to WPLWIN.
 *
 *      (C)microform ab 24/7/92 U. Andersson 
 *
 *      7/12/93 Omarbetad, J. Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

   {
   WPSBAR *sbptr;

/*
***Remove temporary file.
*/
   IGfdel(lwinpt->filnam);
/*
***Deallocate memory for subwindows.
*/
   sbptr = (WPSBAR *)lwinpt->wintab[0].ptr;
   if ( sbptr != NULL ) WPdelete_slidebar(sbptr);

   sbptr = (WPSBAR *)lwinpt->wintab[1].ptr;
   if ( sbptr != NULL ) WPdelete_slidebar(sbptr);
/*
***Free memory for the WPLWIN itself.
*/
   v3free((char *)lwinpt,"WPdllw");
/*
***Slut.
*/
   return(0);
   }
/********************************************************/
/*!******************************************************/

 static short crlwin(
        int   x,
        int   y,
        int   dx,
        int   dy,
        char *wtitle)

/*      Creates a WPLWIN in X.
 *
 *      In: x,y    = Window position.
 *          dx,dy  = Window size.
 *          wtitel = Window title.
 *
 *      (C)microform ab 19/7/92 U. Andersson
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1998-03-11 Lï¿½ngre rader, J.Kjellander
 *      2006-12-19 ButtonReleaseMask, J.Kjellander
 *      2007-10-18 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
    XSetWindowAttributes xwina;
    unsigned long        xwinm;
    XSizeHints           xhint;
    char                 titel[V3STRLEN];

/*
***Title.
*/
    strcpy(titel,"l-");
    strcat(titel,jobnam);
/*
***Attributes.
*/
    xwina.background_pixel = WPgcol(0); /*WP_BGND1);*/
    xwina.border_pixel = BlackPixel( xdisp, xscr );
    xwina.override_redirect = False;
    xwina.save_under = False;

    xwinm = ( CWBackPixel        | CWBorderPixel |
              CWOverrideRedirect | CWSaveUnder );
/*
***Create window.
*/
    actlwin->id.x_id = XCreateWindow(xdisp,DefaultRootWindow(xdisp),
                                     x,y,dx,dy,3,
                                     DefaultDepth(xdisp,xscr),
                                     InputOutput,
                                     CopyFromParent,xwinm,&xwina);

    xhint.flags  = USPosition | USSize | PMinSize | PMaxSize;
    xhint.x = x;
    xhint.y = y;
    xhint.width  = dx;
    xhint.height = dy;
    xhint.min_height = 4*WPstrh();
    xhint.min_width  = WPstrl(wtitle);
    xhint.max_width  = V3STRLEN*WPstrl("w")+2*WPstrh();
    xhint.max_height = (int)(0.8*DisplayHeight(xdisp,xscr));

    XSetNormalHints(xdisp,actlwin->id.x_id,&xhint);

    XStoreName(xdisp,actlwin->id.x_id,wtitle);
    XSetIconName(xdisp,actlwin->id.x_id,titel);
/*
***Set the WM delete protocol.
*/
    WPsdpr(actlwin->id.x_id);
/*
***Event mask.
*/
    XSelectInput(xdisp,actlwin->id.x_id,ExposureMask    |
                                        ButtonPressMask |
                                        ButtonReleaseMask |
                                        StructureNotifyMask);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short savelw(
        WPLWIN *lwinpt)

/*      Save to file or print WPLWIN contents.
 *
 *      In: lwinpt = C ptr to WPLWIN.
 *
 *      (C)microform ab 7/8/92 U. Andersson 
 *
 *      8/12/93 Omarbetad, J. Kjellander
 *      1997-01-25 printer, J.Kjellander
 *      1998-03-11 Längre rader, J.Kjellander
 *      2004-09-03 English texts, Johan Kjellander, Örebro university
 *      2007-10-20 Slidebars, J.Kjellander
 *
 ******************************************************!*/

  {
   int   first,count;
   char *slt = "Entire list or window";
   char *est = "e";
   char *wst = "w";  
   char *spt = "  File or printer  ";  
   char *fst = "f";
   char *pst = "p";  
   char  rad[V3STRLEN+2];
   bool  hela,fil;
   short status;
   char  oscmd[V3PTHLEN+25],fnam[V3PTHLEN],printer[V3STRLEN];
   FILE *tempfil;

   static char dstr[V3PTHLEN] = "";

/*
***Entire list or only current window ?
*/
    hela = WPialt(slt,est,wst,FALSE);
/*
***First line.
*/
    if ( hela )  first = 0;
    else         first = lwinpt->nl_first;
/*
***Save to file or print ?
*/
    fil = WPialt(spt,fst,pst,FALSE);
/*
***File name ?
*/
    if ( dstr[0] == '\0' )
      {
      strcpy(dstr,jobdir);
      strcat(dstr,jobnam);
      strcat(dstr,LSTEXT);
      }

    if ( fil )
      {
      status = IGssip("","Enter filename :",fnam,dstr,V3PTHLEN);
      if ( status < 0 ) goto end;
      strcpy(dstr,fnam);
      }
    else
      {
      strcpy(fnam,dstr);
      }
/*
***Open list file.
*/
    lwinpt->filpek = fopen(lwinpt->filnam,"r");
/*
***Create and open temporary file.
*/
    tempfil = fopen(fnam,"w+");
/*
***Write header
*/
    fprintf(tempfil,"%s\n\n",lwinpt->rubrik);
/*
***Write listfile contents.
*/
     fseek(lwinpt->filpek,first*(V3STRLEN+1),SEEK_SET);

     if ( hela )
       {
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL) 
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
         }
       }
     else
       {
       count = 0;
       while ( fgets(rad,V3STRLEN+2,lwinpt->filpek) != NULL &&
               count < lwinpt->nl_vis )
         {
         rad[V3STRLEN] = '\0';
         fprintf(tempfil,"%s\n",rad);
       ++count;
         }
       }
/*
***Close files.
*/
    fclose(lwinpt->filpek);
    fclose(tempfil);
/*
***Possible printout.
*/
    if ( !WPgrst("varkon.list.printer",printer) ) strcpy(printer,"lp");

    if ( !fil )
      {
      strcpy(oscmd,"cat ");
      strcat(oscmd,fnam);
      strcat(oscmd," | ");
      strcat(oscmd,printer);
      EXos(oscmd,2);
      }
/*
***The end.
*/
end:
    return(0);
  }
/********************************************************/
