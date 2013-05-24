/**********************************************************************
*
*    wpDECRN.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_3Dline();      Create a 3D line decoration
*    WPcreate_fillrect();    Create filled rectangle
*    WPexpose_decoartion();  Expose decoration
*    WPdelete_decoration();  Delete a decoration
*    WP3db();                Add 3D border to window.
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

/*!******************************************************/

        short  WPcreate_3Dline(
        int    pid,
        short  x1,
        short  y1,
        short  x2,
        short  y2)

/*      Create a 3D line decoration in a WPIWIN window.
 *
 *      In: pid   = Parent window ID.
 *          x1,y1 = Start position.
 *          x2,y2 = End position.
 *
 *      (C)2007-10-23 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   wpw_id   did;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPDECRN *dcrptr;

/*
***Get a C ptr to the WPIWIN.
*/
   winptr = WPwgwp(pid);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Allocate an ID for the new decoration.
*/
   i = 0;
   while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;

   if ( i == WP_IWSMAX ) return(-3);
   else did = i;
/*
***Create a WPDECRN.
*/
   if ( (dcrptr=(WPDECRN *)v3mall(sizeof(WPDECRN),"WPcreate_3Dline")) == NULL )
      return(-3);

   dcrptr->id.w_id = did;
   dcrptr->id.p_id = pid;
   dcrptr->id.x_id = iwinpt->id.x_id;

   dcrptr->type = LINE3DDECRN;

   dcrptr->x1 = x1;
   dcrptr->y1 = y1;
   dcrptr->x2 = x2;
   dcrptr->y2 = y2;

   dcrptr-> color = 0;
/*
***Add the decoration to the WPIWIN.
*/
   iwinpt->wintab[did].typ = TYP_DECRN;
   iwinpt->wintab[did].ptr = (char *)dcrptr;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPcreate_fillrect(
        int    pid,
        short  x,
        short  y,
        short  dx,
        short  dy,
        short  color)

/*      Create a filled rectangle decoration in a WPIWIN window.
 *
 *      In: pid   = Parent window ID.
 *          x,y   = Start position.
 *          dx,dy = Size.
 *          color = Fill color
 *
 *      (C)2007-11-06 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   wpw_id   did;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPDECRN *dcrptr;

/*
***Get a C ptr to the WPIWIN.
*/
   winptr = WPwgwp(pid);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Allocate an ID for the new decoration.
*/
   i = 0;
   while ( i < WP_IWSMAX  &&  iwinpt->wintab[i].ptr != NULL ) ++i;

   if ( i == WP_IWSMAX ) return(-3);
   else did = i;
/*
***Create a WPDECRN.
*/
   if ( (dcrptr=(WPDECRN *)v3mall(sizeof(WPDECRN),"WPcreate_fillrect")) == NULL )
      return(-3);

   dcrptr->id.w_id = did;
   dcrptr->id.p_id = pid;
   dcrptr->id.x_id = iwinpt->id.x_id;

   dcrptr->type = FILLRECTDECRN;

   dcrptr->x1 = x;
   dcrptr->y1 = y;
   dcrptr->x2 = dx;
   dcrptr->y2 = dy;

   dcrptr-> color = color;
/*
***Add the decoration to the WPIWIN.
*/
   iwinpt->wintab[did].typ = TYP_DECRN;
   iwinpt->wintab[did].ptr = (char *)dcrptr;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool     WPexpose_decoration(
        WPDECRN *dcrptr)

/*      Expose handler for WPDECRN.
 *
 *      In: buttptr = C ptr to WPDECRN.
 *
 *      (C)2007-10-23 J.Kjellander
 *
 ******************************************************!*/

  {
   short dx,dy;

/*
***What type of decoration ?
*/
   switch ( dcrptr->type )
     {
/*
***A 3D line.
*/
     case LINE3DDECRN:
     XSetForeground(xdisp,xgc,WPgcol(WP_BOTS));
     XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1,dcrptr->y1,
                                         dcrptr->x2,dcrptr->y2);
     XSetForeground(xdisp,xgc,WPgcol(WP_TOPS));

     dx = dcrptr->x2 - dcrptr->x1;
     dy = dcrptr->y2 - dcrptr->y1;

     if ( dy == 0 )
       XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1,dcrptr->y1-1,
                                           dcrptr->x2,dcrptr->y2-1);
     else if ( dx == 0 )
       XDrawLine(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1-1,dcrptr->y1,
                                           dcrptr->x2-1,dcrptr->y2);
     break;
/*
***A filled rectangle.
*/
     case FILLRECTDECRN:
     XSetForeground(xdisp,xgc,dcrptr->color);
     XFillRectangle(xdisp,dcrptr->id.x_id,xgc,dcrptr->x1,
                                              dcrptr->y1,
                                              dcrptr->x2,
                                              dcrptr->y2);
     break;
     }
/*
***The end.
*/
    return(TRUE);
  }

/********************************************************/
/*!******************************************************/

        short    WPdelete_decoration(
        WPDECRN *dcrptr)

/*      Delete a WPDECRN.
 *
 *      In: buttptr = C ptr to WPDECRN.
 *
 *      (C)2007-10-23 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Release allocated C memory.
*/
   v3free((char *)dcrptr,"WPdelete_decoration");
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPd3db(
        char  *winptr,
        int    wintyp)

/*      L�gniv�-rutin f�r att f�rse ett f�nster med 
 *      en 3D-ram.
 *
 *      In: winptr = Pekare till WPBUTT, WPEDIT eller WPICON.
 *          wintyp = Typ av f�nster, TYP_EDIT, TYP_BUTTON
 *                                   TYP_ICON.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 19/1/94 J. Kjellander
 *
 *      1998-03-29 OpenGL f�r AIX, J.Kjellander
 *
 ******************************************************!*/

  {
    int     dx,dy,bw,i;
    short   bc,upleft,dnrgth;
    Window  xid;
    GC      act_gc;
    WPBUTT *butptr;
    WPEDIT *edtptr;
    WPICON *icoptr;
    WPRWIN *rwinpt;
    unsigned long bgpix,fgpix,ulpix,drpix;

/*
***Vilken sorts f�nster �r det ? Ta reda p� storlek etc.
*/
    switch ( wintyp )
      {
      case TYP_BUTTON:
      butptr = (WPBUTT *)winptr;
      if ( wpwtab[butptr->id.p_id].typ  ==  TYP_RWIN )
        {
        rwinpt = (WPRWIN *)wpwtab[butptr->id.p_id].ptr;
        act_gc = rwinpt->win_gc;
        }
      else act_gc = xgc;

      xid    = butptr->id.x_id;
      dx     = butptr->geo.dx;
      dy     = butptr->geo.dy;
      bw     = butptr->geo.bw;
      bc     = butptr->color.bckgnd;
      upleft = WP_TOPS;
      dnrgth = WP_BOTS;
      ulpix  = WPgcbu(butptr->id.p_id,WP_TOPS);
      drpix  = WPgcbu(butptr->id.p_id,WP_BOTS);
      bgpix  = WPgcbu(butptr->id.p_id,WP_BGND1);
      fgpix  = WPgcbu(butptr->id.p_id,WP_FGND);
      break;

      case TYP_EDIT:
      edtptr = (WPEDIT *)winptr;
      act_gc = xgc;
      xid    = edtptr->id.x_id;
      dx     = edtptr->geo.dx;
      dy     = edtptr->geo.dy;
      bw     = edtptr->geo.bw;
      bc     = WP_BGND2;
      upleft = WP_BOTS;
      dnrgth = WP_TOPS;
      ulpix  = WPgcol(WP_BOTS);
      drpix  = WPgcol(WP_TOPS);
      bgpix  = WPgcol(WP_BGND2);
      fgpix  = WPgcol(WP_FGND);
      break;

      case TYP_ICON:
      icoptr = (WPICON *)winptr;
      act_gc = xgc;
      xid    = icoptr->id.x_id;
      dx     = icoptr->geo.dx;
      dy     = icoptr->geo.dy;
      bw     = icoptr->geo.bw;
      bc     = WP_BGND2;
      upleft = WP_TOPS;
      dnrgth = WP_BOTS;
      ulpix  = WPgcol(WP_TOPS);
      drpix  = WPgcol(WP_BOTS);
      bgpix  = WPgcol(WP_BGND2);
      fgpix  = WPgcol(WP_FGND);
      break;

      default:
      return(0);
      }
/*
***Rita. Antal pixels = dx,dy men adresserna = 0 till dx-1.
*/
    dx -= 1;
    dy -= 1;
/*
***Skugga �ver och till v�nster.
*/
    if ( upleft != WP_FGND )
      XSetForeground(xdisp,act_gc,ulpix);

    for ( i=1; i<bw+1; ++i )
      {
      XDrawLine(xdisp,xid,act_gc,i,i,dx-i,i);
      XDrawLine(xdisp,xid,act_gc,i,i,i,dy-i);
      }
/*
***Om knappen har annorlunda bakgrundsf�rg �n huvud-
***f�nstret m�ste en ram p� 1 pixel med bakgrundsf�rg
***skapas f�rst. ( Luften mellan 3D-ramen och Notify-ramen ).
***Detta g�rs h�r (mellan upleft och dnrgth) f�r att minimera
***antalet anrop till XSetForeground().
*/
    if ( bc != WP_BGND1 )
      {
      XSetForeground(xdisp,act_gc,bgpix);
      XDrawLine(xdisp,xid,act_gc,0,0,dx,0);
      XDrawLine(xdisp,xid,act_gc,dx,0,dx,dy);
      XDrawLine(xdisp,xid,act_gc,dx,dy,0,dy);
      XDrawLine(xdisp,xid,act_gc,0,dy,0,0);
      }
/*
***Skugga under och till h�ger.
*/
    XSetForeground(xdisp,act_gc,drpix);

    for ( i=1; i<bw+1; ++i )
      {
      XDrawLine(xdisp,xid,act_gc,dx-i,i,dx-i,dy-i);
      XDrawLine(xdisp,xid,act_gc,i,dy-i,dx-i,dy-i);
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
