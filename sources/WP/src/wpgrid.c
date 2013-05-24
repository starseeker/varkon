/**********************************************************************
*
*    WPgrid.c
*    ========
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    WPgrid_dialogue();  The grid dialogue
*    WPdraw_grid();      Display grid
*    WPdelete_grid();    Undisplay grid
*    WPget_grid();       Get grid data
*    WPset_grid();       Set grid data
*    WPgrid_on();        Set grid on
*    WPgrid_off();       Set grid off
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
#include "../../GE/include/GE.h"
#include "../../IG/include/IG.h"
#include "../../AN/include/AN.h"
#include "../../WP/include/WP.h"

#include <math.h>

extern DBTmat   lklsyi,*lsyspk;

#ifdef UNIX
extern Display *xdisp;
#endif

static bool   premiere = TRUE;               /* True only for the first invocation */
static int    iwin_x;                        /* The current position of the window */
static int    iwin_y;                        /* even after a user move */

/*
***Prototypes for internal functions.
*/
static void  draw_grid(WPGWIN *gwinpt);
static short get_resolution(WPGWIN *gwinpt, double *pdx, double *pdy);

/*!******************************************************/

         short WPgrid_dialogue(DBint grw_id)
        
/*      The grid dialogue.
 *
 *      In: grw_id = ID of WPGWIN that called us.
 *
 *      (C)2007-03-12 J.Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],close[81],help[81],edit[81],pos[81],
            res[81],show[81],hide[81],move[81],movett[81],
            edittt[81],showtt[81],hidett[81],bs1[81],bs2[81];
   char    *typ[20];
   int      bh,wm_x1,wm_y1,wm_x2,wm_y2,butlen,bl1,bl2;
   short    status,main_dx,main_dy,alt_x,alt_y,butlen1,butlen2,ly,lm;
   DBint    iwin_id,move_id,edit_id,show_id,hide_id,close_id,
            help_id,but_id;
   unsigned int dum1,dum2;
   DBVector newpos;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;
   WPGWIN  *gwinpt;
   XEvent   event;
   XrmValue value;

/*
***Is this the first time this function is called ?
***Initial window position.
*/
   if ( premiere )
     {
     iwin_x = iwin_y = 5;

     if ( XrmGetResource(xresDB,"varkon.grid.geometry",
                                "Varkon.Grid.Geometry",
                                 typ,&value) ) 
       XParseGeometry((char *)value.addr,&iwin_x,&iwin_y,&dum1,&dum2);

     premiere = FALSE;
     }
/*
***Get a C ptr to the graphical window that called us.
*/
   gwinpt = (WPGWIN *)wpwtab[grw_id].ptr;
/*
***During user interaction the dialogue window may get deleted
***and created again with new contents. This is where we start
***recreating a new instance.
*/
start:

/*
***Window title, position, resolution, move, edit, show, hide,
***close and help from the ini-file.
*/
   if ( !WPgrst("varkon.grid.title",rubrik) )        strcpy(rubrik,"Grid");
   if ( !WPgrst("varkon.grid.position.text",pos) )   strcpy(pos,"Position");
   if ( !WPgrst("varkon.grid.resolution.text",res) ) strcpy(res,"Resolution");
   if ( !WPgrst("varkon.grid.show.text",show) )      strcpy(show,"Show");
   if ( !WPgrst("varkon.grid.show.tooltip",showtt) ) strcpy(showtt,"");
   if ( !WPgrst("varkon.grid.hide.text",hide) )      strcpy(hide,"Hide");
   if ( !WPgrst("varkon.grid.hide.tooltip",hidett) ) strcpy(hidett,"");
   if ( !WPgrst("varkon.grid.move.text",move) )      strcpy(move,"Move");
   if ( !WPgrst("varkon.grid.move.tooltip",movett) ) strcpy(movett,"");
   if ( !WPgrst("varkon.grid.edit.text",edit) )      strcpy(edit,"Edit");
   if ( !WPgrst("varkon.grid.edit.tooltip",edittt) ) strcpy(edittt,"");
   if ( !WPgrst("varkon.input.close",close) )        strcpy(close,"Close");
   if ( !WPgrst("varkon.input.help",help) )          strcpy(help,"Help");
/*
***What is the 1.2*length of the longest text ?
***Don't include the title, it will fit anyhow.
*/
   butlen1 = 0;
   if ( WPstrl(pos)    > butlen1 ) butlen1 = WPstrl(pos);
   if ( WPstrl(res)    > butlen1 ) butlen1 = WPstrl(res);
   if ( WPstrl(move)   > butlen1 ) butlen1 = WPstrl(move);
   if ( WPstrl(edit)   > butlen1 ) butlen1 = WPstrl(edit);
   if ( WPstrl(show)   > butlen1 ) butlen1 = WPstrl(show);
   if ( WPstrl(hide)   > butlen1 ) butlen1 = WPstrl(hide);
   butlen1 *= 1.2;

   butlen2 = 0;
   if ( WPstrl(close)  > butlen2 ) butlen2 = WPstrl(close);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.8;
/*
***Calculate outside air (ly), button height (bh) and air between (lm).
*/
   ly = (short)(1.2*WPstrh());
   bh = (short)(1.8*WPstrh());
   lm = (short)(0.8*WPstrh());
/*
***Calculate the window size in X-direction.
*/
   main_dx = ly + butlen1 + ly + ly + butlen1 + ly;
/*
***Calculate the window size in Y-direction.
*/
   main_dy = ly + bh + lm + bh + 0 + bh + lm + bh + bh + bh + bh + bh + bh + ly;
/*
***Create the dialogue window as a WPIWIN.
*/
   WPwciw((short)iwin_x,(short)iwin_y,main_dx,main_dy,rubrik,&iwin_id);
/*
***Get a C-ptr to the WPIWIN.
*/
   winptr = WPwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***A vertical line.
*/
   alt_x  = ly + butlen1 + ly;
   alt_y  = ly + lm;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,1,4*bh + 2*ly - 2*lm,(short)0,
                           "","","",WP_TOPS,WP_TOPS,&but_id);
   status = WPmcbu((wpw_id)iwin_id,alt_x-1,alt_y,1,4*bh + 2*ly - 2*lm,(short)0,
                           "","","",WP_BOTS,WP_BOTS,&but_id);
/*
***Position and resolution texts.
*/
   alt_x  = ly;
   alt_y  = ly;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)0,
                           pos,pos,"",WP_BGND1,WP_FGND,&but_id);

   alt_x  = ly + butlen1 + ly + ly;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)0,
                           res,res,"",WP_BGND1,WP_FGND,&but_id);
/*
***X, Y, DX and DY texts.
*/
   sprintf(bs1,"X = %g",gwinpt->grid_x);
   bl1 = WPstrl(bs1);
   sprintf(bs2,"Y = %g",gwinpt->grid_y);
   bl2 = WPstrl(bs2);
   if ( bl1 > bl2 ) butlen = bl1;
   else             butlen = bl2;

   alt_x  = (ly + butlen1 + ly - butlen)/2;
   alt_y  = ly + bh + lm;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,bl1,bh,(short)0,
                           bs1,bs1,"",WP_BGND1,WP_FGND,&but_id);

   alt_y  = ly + bh + lm + bh;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,bl2,bh,(short)0,
                           bs2,bs2,"",WP_BGND1,WP_FGND,&but_id);

   sprintf(bs1,"DX = %g",gwinpt->grid_dx);
   bl1 = WPstrl(bs1);
   sprintf(bs2,"DY = %g",gwinpt->grid_dy);
   bl2 = WPstrl(bs2);
   if ( bl1 > bl2 ) butlen = bl1;
   else             butlen = bl2;

   alt_x  = ly + ly + butlen1 + (ly + butlen1 + ly - butlen)/2;
   alt_y  = ly + bh + lm;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,bl1,bh,(short)0,
                           bs1,bs1,"",WP_BGND1,WP_FGND,&but_id);

   alt_y  = ly + bh + lm + bh;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,bl2,bh,(short)0,
                           bs2,bs2,"",WP_BGND1,WP_FGND,&but_id);
/*
***Move and edit.
*/
   alt_x  = ly;
   alt_y += bh + lm;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)2,
                           move,move,"",WP_BGND2,WP_FGND,&move_id);
   butptr = (WPBUTT *)iwinpt->wintab[move_id].ptr;
   strcpy(butptr->tt_str,movett);

   alt_x  = ly + butlen1 + ly + ly;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)2,
                           edit,edit,"",WP_BGND2,WP_FGND,&edit_id);
   butptr = (WPBUTT *)iwinpt->wintab[edit_id].ptr;
   strcpy(butptr->tt_str,edittt);
/*
***Show and hide.
*/
   alt_x  = ly;
   alt_y  += bh + bh;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)2,
                           show,show,"",WP_BGND2,WP_FGND,&show_id);
   butptr = (WPBUTT *)iwinpt->wintab[show_id].ptr;
   strcpy(butptr->tt_str,showtt);

   alt_x  = ly + butlen1 + ly + ly;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,bh,(short)2,
                           hide,hide,"",WP_BGND2,WP_FGND,&hide_id);
   butptr = (WPBUTT *)iwinpt->wintab[hide_id].ptr;
   strcpy(butptr->tt_str,hidett);
/*
***A horizontal line.
*/
   alt_x  = main_dx/8;
   alt_y  += bh + bh;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,6*main_dx/8,1,(short)0,
                           "","","",WP_TOPS,WP_TOPS,&but_id);
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y-1,6*main_dx/8,1,(short)0,
                           "","","",WP_BOTS,WP_BOTS,&but_id);
/*
***Close and help.
*/
   alt_x  = ly;
   alt_y += bh;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)3,
                           close,close,"",WP_BGND2,WP_FGND,&close_id);

   alt_x  = main_dx - ly - butlen2;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,bh,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
/*
***Show the dialogue.
*/
   WPwshw(iwin_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Where did it actually get positioned ?
***This code copied from WPmsip().
*/
   XWindowEvent(xdisp,iwinpt->id.x_id,ExposureMask,&event);
   XPutBackEvent(xdisp,&event);
   WPgtwp(iwinpt->id.x_id,&wm_x1,&wm_y1);
/*
***Wait for action. SMBPOSM is not an issue in this situation.
*/
   status = 0;
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Move button.
*/
   if ( but_id == move_id )
     {
     WPaddmess_mcwin(IGgtts(44),WP_PROMPT);
     status = IGcpov(&newpos);
     WPerhg();    
     IGrsma();
     if ( status == 0 )
       {
       WPdelete_grid(grw_id);
       gwinpt->grid_x = newpos.x_gm;
       gwinpt->grid_y = newpos.y_gm;
       WPdraw_grid(grw_id);
       goto update;
       }
     else goto loop;
     }
/*
***Edit button.
*/
   else if ( but_id == edit_id )
     {
     status = get_resolution(gwinpt,&newpos.x_gm,&newpos.y_gm);
     if ( status == 0 )
       {
       WPdelete_grid(grw_id);
       gwinpt->grid_dx = newpos.x_gm;
       gwinpt->grid_dy = newpos.y_gm;
       WPdraw_grid(grw_id);
       goto update;
       }
     else goto loop;
     }
/*
***Show button.
*/
   else if ( but_id == show_id )
     {
     WPgrid_on(grw_id);
     WPdraw_grid(grw_id);
     goto loop;
     }
/*
**Hide button.
*/
   else if ( but_id == hide_id )
     {
     WPdelete_grid(grw_id);
     WPgrid_off(grw_id);
     goto loop;
     }
/*
***Close button.
*/
   else if ( but_id == close_id )
     {
     status = 0;
     goto exit;
     }
/*
***Help button.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Unknown event, should not happen.
*/
   else
     {
     WPbell();
     goto loop;
     }
/*
***A view was added or removed so we need to update the window.
***Before deleting it, update the variables for it's current position.
*/
update:
   WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
   iwin_x = iwin_x + wm_x2 - wm_x1;
   iwin_y = iwin_y + wm_y2 - wm_y1;
   WPwdel(iwin_id);
   goto start;
/*
***Time to exit. Remeber the current position.
*/
exit:
   WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
   iwin_x = iwin_x + wm_x2 - wm_x1;
   iwin_y = iwin_y + wm_y2 - wm_y1;
   WPwdel(iwin_id);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short WPdraw_grid(wpw_id grw_id)

/*      Display the grid in the specified window(s).
 *
 *      In: grw_id = ID of WPGWIN or GWIN_ALL.
 *
 *      (C)2007-04-03 J.Kjellander
 *
 ******************************************************!*/

{
   int     i;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Found a WPGWIN, does it match the input ?
*/
         if ( grw_id == gwinpt->id.w_id  ||  grw_id == GWIN_ALL )
           {
/*
***Yes. To draw the grid it must be on !
*/
           if ( !gwinpt->grid_on ) return(0);
/*
***If it's on it may already be visible and the
***reason why we are here is that the grid geometry,
***current view or active coordinate system has changed.
***Whoever changed it is responsible of erasing the old
***grid before making the changes.
*/
           WPspen(WP_BGND3);
           draw_grid(gwinpt);
           }
         }
       }
     }
/*
***The end.
*/
    return(0);
}

/********************************************************/
/*!******************************************************/

        short WPdelete_grid(wpw_id grw_id)

/*      Erase the grid in the specified window(s).
 *
 *      In: grw_id = ID of WPGWIN or GWIN_ALL.
 *
 *      (C)2007-04-03 J.Kjellander
 *  
 ******************************************************!*/

{
   int     i;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Found a WPGWIN, does it match the input ?
*/
         if ( grw_id == gwinpt->id.w_id  ||  grw_id == GWIN_ALL )
           {
/*
***Yes. To erase the grid it must be on !
*/
           if ( !gwinpt->grid_on ) return(0);
/*
***Erase with pen 0.
*/
           WPspen(0);
           draw_grid(gwinpt);
           }
         }
       }
     }
/*
***The end.
*/
    return(0);
}

/********************************************************/
/*!******************************************************/

        short   WPget_grid(
        double *ox,
        double *oy,
        double *dx,
        double *dy,
        bool   *on)

/*      Return grid geometry and status of GWIN_MAIN.
 *
 *      Out: *ox,*oy => Origin in model coordinates
 *           *dx,*dy => Spacing
 *           *on     => True/False
 *
 *      (C)2007-04-03 J.Kjellander
 *
 ******************************************************!*/

{
   WPGWIN *gwinpt;

   gwinpt = (WPGWIN *)wpwtab[GWIN_MAIN].ptr;

  *ox = gwinpt->grid_x;
  *oy = gwinpt->grid_y;
  *dx = gwinpt->grid_dx;
  *dy = gwinpt->grid_dy;
  *on = gwinpt->grid_on;

   return(0);
}

/********************************************************/
/*!******************************************************/

        short  WPset_grid(
        double ox,
        double oy,
        double dx,
        double dy)

/*      Set grid geometry of GWIN_MAIN.
 *
 *      In: ox,oy => Origin in model coordinates
 *          dx,dy => Spacing
 *
 *      (C)2007-04-03 J.Kjellander
 *
 ******************************************************!*/

{
   WPGWIN *gwinpt;

   gwinpt = (WPGWIN *)wpwtab[GWIN_MAIN].ptr;

   gwinpt->grid_x  = ox;
   gwinpt->grid_y  = oy;
   gwinpt->grid_dx = dx;
   gwinpt->grid_dy = dy;

   return(0);
}

/********************************************************/
/*!******************************************************/

        void WPgrid_on(wpw_id grw_id)

/*      Turn the grid on in the specified window(s).
 *
 *      In: grw_id = ID of WPGWIN or GWIN_ALL.
 *
 *      (C)2007-04-03 J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Found a WPGWIN, does it match the input ?
*/
         if ( grw_id == gwinpt->id.w_id  ||  grw_id == GWIN_ALL )
           {
/*
***Yes, turn the grid on in this window.
*/
           gwinpt->grid_on = TRUE;
           }
         }
       }
     }
 }

/********************************************************/
/*!******************************************************/

        void WPgrid_off(wpw_id grw_id)

/*      Turn the grid off in the specified window(s).
 *
 *      In: grw_id = ID of WPGWIN or GWIN_ALL.
 *
 *      (C)2007-04-03 J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPGWIN *gwinpt;

/*
***Loop through all WPGWIN's.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
/*
***Found a WPGWIN, does it match the input ?
*/
         if ( grw_id == gwinpt->id.w_id  ||  grw_id == GWIN_ALL )
           {
/*
***Yes, turn the grid off in this window.
*/
           gwinpt->grid_on = FALSE;
           }
         }
       }
     }
 }

/********************************************************/
/*!******************************************************/

 static void draw_grid(WPGWIN *gwinpt)

/*      Draw/erase a 3D grid on the currently active
 *      XY-plane of a window.
 *
 *      In: gwinpt => Ptr to window
 *
 *      (C)2007-03-21 J.Kjellander
 *  
 ******************************************************!*/

{
   int      ix,iy,ix1,ix2,iy1,iy2,size_x,size_y,l,
            idx,idy,pix_dx,ngx,i;
   bool     vertical,one_more,pass_2;
   double   xs,ys,xg,yg,tt,ng,dx_x,dy_x,dx_y,dy_y,
            curx,cury,actx,acty,k,m,y1,y2,pix_prop;
   DBVector p,t;
   DBTmat   gridmat,tmpmat;

/*
***Check that dx and dy are valid numbers.
*/
   if ( gwinpt->grid_dx <= 0.0 )
     {
     WPwlma("Illegal grid dx !");
     return;
     }

   if ( gwinpt->grid_dy <= 0.0 )
     {
     WPwlma("Illegal grid dy !");
     return;
     }
/*
***Make a 4x4 grid matrix of the 3x3 view matrix.
*/
  gridmat.g11 = gwinpt->vy.matrix.k11;
  gridmat.g12 = gwinpt->vy.matrix.k12;
  gridmat.g13 = gwinpt->vy.matrix.k13;
  gridmat.g14 = 0.0;

  gridmat.g21 = gwinpt->vy.matrix.k21;
  gridmat.g22 = gwinpt->vy.matrix.k22;
  gridmat.g23 = gwinpt->vy.matrix.k23;
  gridmat.g24 = 0.0;

  gridmat.g31 = gwinpt->vy.matrix.k31;
  gridmat.g32 = gwinpt->vy.matrix.k32;
  gridmat.g33 = gwinpt->vy.matrix.k33;
  gridmat.g34 = 0.0;

  gridmat.g41 = gwinpt->vy.matrix.k11;
  gridmat.g42 = gwinpt->vy.matrix.k12;
  gridmat.g43 = gwinpt->vy.matrix.k13;
  gridmat.g44 = 1.0;
/*
***If a local coordinate system is active, transform 
***gridmat to the XY-plane of the local system.
*/
  if ( lsyspk != NULL )
     {
     GEtform_mult(&gridmat,&lklsyi,&tmpmat);
     V3MOME(&tmpmat,&gridmat,sizeof(DBTmat));
     }
/*
***Grid matrix g33 must be <> 0 or the XY-plane
***is parallell to the view direction.
*/
   if ( ABS(gridmat.g33) < 1E-10 ) return;
/*
***We need a gridpoint that is surley inside the window to
***use as a start position. The grid point closest to the
***centre of the window should be a good candidate.
***Calculate centre of model window.
*/
   xs = gwinpt->vy.modwin.xmin + (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/2.0;
   ys = gwinpt->vy.modwin.ymin + (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/2.0;
/*
***Transform to 3D model coordinate p in BASIC.
*/
   p.x_gm = gwinpt->vy.matrix.k11*xs + gwinpt->vy.matrix.k21*ys;
   p.y_gm = gwinpt->vy.matrix.k12*xs + gwinpt->vy.matrix.k22*ys;
   p.z_gm = gwinpt->vy.matrix.k13*xs + gwinpt->vy.matrix.k23*ys;
/*
***Shooting direction t = view Z-axis in BASIC.
*/
   t.x_gm = gwinpt->vy.matrix.k31;
   t.y_gm = gwinpt->vy.matrix.k32;
   t.z_gm = gwinpt->vy.matrix.k33;
/*
***Transform p and t to LOCAL.
*/
   if ( lsyspk != NULL )
     {
     GEtfpos_to_local(&p,lsyspk,&p);
     GEtfvec_to_local(&t,lsyspk,&t);
     }
/*
***Z-component of t must be <> 0 or the XY-plane
***is parallell to the view direction.
*/
   if ( ABS(t.z_gm) < 1E-10 ) return;
/*
***Calculate the intersect between the shooting line and the XY-plane.
*/
   tt = -p.z_gm/t.z_gm;
   xg = p.x_gm + tt*t.x_gm;
   yg = p.y_gm + tt*t.y_gm;
/*
***Find closest point in the grid (same as hit calculation).
*/
   ng = ABS((xg - gwinpt->grid_x)/gwinpt->grid_dx);
   if ( DEC(ng) > 0.5 ) ++ng;
   if ( xg > gwinpt->grid_x ) xg = gwinpt->grid_x + HEL(ng)*gwinpt->grid_dx;
   else xg = gwinpt->grid_x - HEL(ng)*gwinpt->grid_dx;

   ng = ABS((yg - gwinpt->grid_y)/gwinpt->grid_dy);
   if ( DEC(ng) > 0.5 ) ++ng;
   if ( yg > gwinpt->grid_y ) yg = gwinpt->grid_y + HEL(ng)*gwinpt->grid_dy;
   else yg = gwinpt->grid_y - HEL(ng)*gwinpt->grid_dy;
/*
***Transform back to window coordinates.
*/
   xs = gridmat.g11*xg + gridmat.g12*yg + gridmat.g14;
   ys = gridmat.g21*xg + gridmat.g22*yg + gridmat.g24;
/*
***(xs,ys) is now the point in the grid which is closest
***to the middle of the window. If it is outside the window
***the distance between individual gridpoints must be very
***large and there is no meaning to draw anything.
*/
   if ( xs < gwinpt->vy.modwin.xmin  ||
        xs > gwinpt->vy.modwin.xmax ) return;

   if ( ys < gwinpt->vy.modwin.ymin  ||
        ys > gwinpt->vy.modwin.ymax ) return;
/*
***The gridpoint is inside the window. Calculate
***the offsets (dx_x,dy_x and dx_y,dy_y) to the next
***point in the X- and Y-direction.
*/
   dx_x = gridmat.g11*gwinpt->grid_dx;
   dy_x = gridmat.g21*gwinpt->grid_dx;

   dx_y = gridmat.g12*gwinpt->grid_dy;
   dy_y = gridmat.g22*gwinpt->grid_dy;
/*
***If the distance between gridpoints as seen on the screen is
***less than a few pixels (8), there is no meaning to display the grid.
*/
   ix1 = gwinpt->vy.k1x + gwinpt->vy.k2x*xs;
   iy1 = gwinpt->vy.k1y + gwinpt->vy.k2y*ys;
   ix2 = gwinpt->vy.k1x + gwinpt->vy.k2x*(xs + dx_x);
   iy2 = gwinpt->vy.k1y + gwinpt->vy.k2y*(ys + dy_x);
   idx = ix2 - ix1;
   idy = iy2 - iy1;

   if ( (pix_dx=SQRT(idx*idx + idy*idy)) < 8.0 )
     {
     WPwlma("Grid dx too small !");
     return;
     }

   ix2 = gwinpt->vy.k1x + gwinpt->vy.k2x*(xs + dx_y);
   iy2 = gwinpt->vy.k1y + gwinpt->vy.k2y*(ys + dy_y);
   idx = ix2 - ix1;
   idy = iy2 - iy1;

   if ( SQRT(idx*idx + idy*idy) < 8.0 )
     {
     WPwlma("Grid dy too small !");
     return;
     }
/*
***If dx_x is very small the direction of increasing X
***is vertical.
*/
    if ( ABS(dx_x) > 1E-10 ) vertical = FALSE;
    else                     vertical = TRUE;
/*
***A cross in the middle of the screen to show the X- and
***Y-directions.
*/
   size_x = gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin;
   size_y = gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin;

   if (size_x < size_y ) l = size_x/2.5;
   else                  l = size_y/2.5;

   ix1 = gwinpt->vy.k1x + gwinpt->vy.k2x*xs;
   iy1 = gwinpt->vy.k1y + gwinpt->vy.k2y*ys;

   pix_prop = gwinpt->geo.psiz_y/gwinpt->geo.psiz_x;

   ix2 = ix1 + l*gridmat.g11;
   iy2 = iy1 + l*gridmat.g21/pix_prop;
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);

   ix2 = ix1 - l*gridmat.g11;
   iy2 = iy1 - l*gridmat.g21/pix_prop;
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);

   ix2 = ix1 + l*gridmat.g12;
   iy2 = iy1 + l*gridmat.g22/pix_prop;
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);

   ix2 = ix1 - l*gridmat.g12;
   iy2 = iy1 - l*gridmat.g22/pix_prop;
   XDrawLine(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);
   XDrawLine(xdisp,gwinpt->savmap,gwinpt->win_gc,
             ix1,gwinpt->geo.dy-iy1,ix2,gwinpt->geo.dy-iy2);
/*
***How big is the screen window ? Use the biggest of geo_dx
***and sgeo_dy to calculate the minimum number of gridpoints needed.
*/
   if ( gwinpt->geo.dx < gwinpt->geo.dy ) ngx = gwinpt->geo.dy/pix_dx + 1;
   else                                   ngx = gwinpt->geo.dx/pix_dx + 1;
/*
***Start with (actx,acty) = (xs,ys) and generate ngx points in
***increasing/decreasing X-direction. Then increment acty until
***no more points fall into the window. Then set pass_2 = TRUE
***and start with acty = ys - dy_y and finish the rest.
*/
   pass_2 = FALSE;
   actx   = xs;
   acty   = ys;

restart:

   do
     {
/*
***Display ngx gridpoints in increasing X direction.
*/
     curx = actx;
     cury = acty;

     for ( i=0; i<ngx; ++i )
       {
       ix = gwinpt->vy.k1x + gwinpt->vy.k2x*curx;
       iy = gwinpt->vy.k1y + gwinpt->vy.k2y*cury;

       if ( ix>gwinpt->vy.scrwin.xmin && ix<gwinpt->vy.scrwin.xmax  &&
            iy>gwinpt->vy.scrwin.ymin && iy<gwinpt->vy.scrwin.ymax)
         {
         XDrawRectangle(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
                        ix,gwinpt->geo.dy - iy,1,1);
         XDrawRectangle(xdisp,gwinpt->savmap,gwinpt->win_gc,
                        ix,gwinpt->geo.dy - iy,1,1);
         }

       curx += dx_x;
       cury += dy_x;
       }
/*
***Display ngx gridpoints in decreasing X direction.
*/
     curx = actx - dx_x;
     cury = acty - dy_x;

     for ( i=0; i<ngx; ++i )
       {
       ix = gwinpt->vy.k1x + gwinpt->vy.k2x*curx;
       iy = gwinpt->vy.k1y + gwinpt->vy.k2y*cury;

       if ( iy > gwinpt->vy.scrwin.ymin  &&  iy < gwinpt->vy.scrwin.ymax )
         {
         XDrawRectangle(xdisp,gwinpt->id.x_id,gwinpt->win_gc,
                        ix,gwinpt->geo.dy - iy,1,1);
         XDrawRectangle(xdisp,gwinpt->savmap,gwinpt->win_gc,
                        ix,gwinpt->geo.dy - iy,1,1);
         }

       curx -= dx_x;
       cury -= dy_x;
       }
/*
***Next Y.
*/
     if ( pass_2 )
       {
       actx -= dx_y;
       acty -= dy_y;
       }
     else
       {
       actx += dx_y;
       acty += dy_y;
       }
/*
***Calculate the coefficients of the equation of a straight
***line in the direction of increasing X.
***If the line is not vertical it will intersect the model window xmin and xmax ?
**If so, calculate the coordinates, modwin.xmin=>y1 and modwin.xmax=>y2. Use
***y1 and y2 to establish the stop criterion.
*/
     if ( !vertical )
       {
       k = dy_x/dx_x;
       m = acty - k*actx;
       y1 = k*gwinpt->vy.modwin.xmin + m;
       y2 = k*gwinpt->vy.modwin.xmax + m;

       if ( (acty > gwinpt->vy.modwin.ymin  &&  acty < gwinpt->vy.modwin.ymax) ||
            (y1   > gwinpt->vy.modwin.ymin  &&  y1   < gwinpt->vy.modwin.ymax) ||
            (y2   > gwinpt->vy.modwin.ymin  &&  y2   < gwinpt->vy.modwin.ymax) )
         one_more = TRUE;
       else
         one_more = FALSE;
       }
/*
***If the line is vertical it will not intersect the model window xmin and xmax
***and another stop criterion is needed.
*/
     else
       {
       if ( actx > gwinpt->vy.modwin.xmin  &&  actx < gwinpt->vy.modwin.xmax )
         one_more = TRUE;
       else
         one_more = FALSE;
       }
/*
***Next step in Y-direction.
*/
     } while ( one_more );
/*
***Are we finished or do we need one more turn ?
*/
   if ( pass_2 )
     {
     XFlush(xdisp);
     return;
     }
   else
     {
     pass_2 = TRUE;
     actx   = xs - dx_y;
     acty   = ys - dy_y;
     goto restart;
     }
}

/********************************************************/
/*!******************************************************/

 static short   get_resolution(
        WPGWIN *gwinpt,
        double *pdx,
        double *pdy)

/*      Input dialogue for grid resolution DX, DY.
 *
 *      In:  gwinpt = C ptr to WPGWIN
 *
 *      Out: *pdx = New distance in X-direction
 *           *pdy = New distance in Y-direction
 *
 *      Return:    0 => Ok.
 *            REJECT => Operation aborted.
 *
 *      (C)2007-03-14 J.Kjellander
 *
 ******************************************************!*/

  {
    char    expr[132],ps[2][40],is[2][80],ds[2][80],
           *psarr[2],*isarr[2],*dsarr[2];
    short   ml[2],status;
    pm_ptr  exnpt,valtyp;
    sttycl  type;
    PMLITVA val;

/*
***Init.
*/
    strcpy(ps[0],"DX");
    strcpy(ps[1],"DY");

    ml[0] = ml[1] = 80;

    psarr[0] = ps[0];
    psarr[1] = ps[1];

    isarr[0] = is[0];
    isarr[1] = is[1];
 
    sprintf(ds[0],"%g",gwinpt->grid_dx);
    sprintf(ds[1],"%g",gwinpt->grid_dy);

    dsarr[0] = ds[0];
    dsarr[1] = ds[1];
/*
***Get DX and DY values.
*/
loop:
    IGptma(45,IG_INP);
    status = IGmsip(psarr,isarr,dsarr,ml,(short)2);
    IGrsma();
    if ( status < 0 ) return(status);
/*
***Remember the answer.
*/
    strcpy(ds[0],is[0]); 
    strcpy(ds[1],is[1]);
/*
***Create MBS vector expression.
*/
    sprintf(expr,"vec(%s,%s)",is[0],is[1]);
    pmmark();
    status = anaexp(expr,FALSE,&exnpt,&type);
    if ( status != 0 )
      {
      pmrele();
      errmes();
      goto loop;
      }
/*
***Evaluate expression.
*/
    else
      {
      inevev(exnpt,&val,&valtyp);
     *pdx = val.lit.vec_va.x_val;
     *pdy = val.lit.vec_va.y_val;
      pmrele();
      return(0);
      }
    }

/********************************************************/
