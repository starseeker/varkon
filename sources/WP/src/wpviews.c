/**********************************************************************
*
*    wpviews.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPview_dialog();         The view dialog
*    WPinit_views();          Creates default views
*    WPcreate_view();         Adds a view to wpviewtab[]
*    WPdelete_view();         Deletes a view in wpviewtab[]
*    WPcampos_to_matrix();    View matrix from camera position
*    WPreload_view();         Reloads view data into window
*    WPactivate_view();       Activates a view from wpviewtab[]
*    WPgtvi();                GET_VIEW in MBS
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <math.h>
#include <string.h>

extern int actfunc; /* For the help system */

/*
***wpviewtab[] holds named WPVIEW's for graphical windows.
*/
WPVIEW wpviewtab[WPMAXVIEWS];

static bool premiere = TRUE; /* True only for the first invocation */
static int  iwin_x;          /* The current position of the view dialogue */
static int  iwin_y;          /* even after a user move */

/*!******************************************************/

         short WPview_dialog(DBint grw_id)

/*      The view dialogue.
 *
 *      In:  grw_id = ID of graphical window.
 *
 *      (C)2007-02-05 J.Kjellander
 *
 *      2007-08-31 Added WPrepaint at extit, J.Kjellander
 *      2007-09-22 Bug repaint, J.Kjellander
 *
 ******************************************************!*/

  {
   char    *namlst[WPMAXVIEWS],rubrik[81],view_name[WPVNLN+1],
           *str,close[81],help[81],save[81],delete[81],
            csys[81],camera[81],ed_pmt[81],buf[81],csystt[81],
            cameratt[81],savett[81],deletett[81],closett[81],
            helptt[81];
   char    *typ[20];
   int      n_views,actalt,i,rad,radant,wm_x1,wm_y1,wm_x2,wm_y2,
            wintype,actfunc_org;
   short    status,main_dx,main_dy,alt_x,alt_y,uph,lowh,uplen,
            lowlen1,lowlen2,dx_up,dx_low,ly,lm;
   DBint    iwin_id,alt_id[WP_IWSMAX],but_id,help_id,close_id,
            save_id,delete_id,csys_id,camera_id,edit_id;
   bool     dum;
   unsigned int dum1,dum2;
   DBVector campos;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPGWIN  *gwinpt;
   WPRWIN  *rwinpt;
   WPVIEW   new_view;
   DBId     csy_id[MXINIV];
   DBetype  type;
   DBptr    la;
   DBCsys   csy;
   XEvent   event;
   XrmValue value;
   VYWIN    minwin;
   WPBUTT  *butptr;
   WPEDIT  *edtptr;

/*
***Set actfunc, see IG/include/futab.h.
*/
   actfunc_org = actfunc;
   actfunc = 102;
/*
***Is this the first time this function is called ?
***Initial window position.
*/
   if ( premiere )
     {
     iwin_x = iwin_y = 5;

     if ( XrmGetResource(xresDB,"varkon.view.geometry",
                                "Varkon.View.Geometry",
                                 typ,&value) ) 
       XParseGeometry((char *)value.addr,&iwin_x,&iwin_y,&dum1,&dum2);

     premiere = FALSE;
     }
/*
***Is the caller WPGWIN or WPRWIN ?
*/
   winptr = WPwgwp((wpw_id)grw_id);

   if      ( winptr->typ == TYP_GWIN )
     {
     wintype = TYP_GWIN;
     gwinpt = (WPGWIN *)winptr->ptr;
     }
   else if ( winptr->typ == TYP_RWIN )
     {
     wintype = TYP_RWIN;
     rwinpt = (WPRWIN *)winptr->ptr;
     }
   else return(-3);
/*
***During user interaction the dialogue window may get deleted
***and created again with new contents. This is where we start
***recreating a new instance.
*/
start:
/*
***Currently active view name in this window.
*/
   if ( wintype == TYP_GWIN ) strcpy(view_name,gwinpt->vy.name);
   else                       strcpy(view_name,rwinpt->vy.name);
/*
***Get the array of all viewnames from wpviewtab[].
***Set actalt = the view currently active in this
***window and view_name to it's name.
*/
   actalt  = -1;
   n_views =  0;

   for ( i=0; i<WPMAXVIEWS; ++i )
     {
     if ( wpviewtab[i].status > VIEW_NOT_USED )
       {
       if ( strcmp(wpviewtab[i].name,view_name) == 0 ) actalt = n_views;
       namlst[n_views] = wpviewtab[i].name;
       ++n_views;
       }
     }
   if ( actalt < 0 ) view_name[0] = '\0';
/*
***Window title, save, delete, csys, camera, close and help from
***the ini-file.
*/
   if ( !WPgrst("varkon.view.title",rubrik) )            strcpy(rubrik,"Views");
   if ( !WPgrst("varkon.view.csys.text",csys) )          strcpy(csys,"Csys");
   if ( !WPgrst("varkon.view.csys.tooltip",csystt) )     strcpy(csystt,"");
   if ( !WPgrst("varkon.view.campos.text",camera) )      strcpy(camera,"Camera");
   if ( !WPgrst("varkon.view.campos.tooltip",cameratt) ) strcpy(cameratt,"");
   if ( !WPgrst("varkon.view.save.text",save) )          strcpy(save,"Save");
   if ( !WPgrst("varkon.view.save.tooltip",savett) )     strcpy(savett,"");
   if ( !WPgrst("varkon.view.delete.text",delete) )      strcpy(delete,"Delete");
   if ( !WPgrst("varkon.view.delete.tooltip",deletett) ) strcpy(deletett,"");
   if ( !WPgrst("varkon.input.close",close) )            strcpy(close,"Close");
   if ( !WPgrst("varkon.input.close.tooltip",closett) )  strcpy(closett,"");
   if ( !WPgrst("varkon.input.help",help) )              strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )    strcpy(helptt,"");

   strcpy(ed_pmt,IGgtts(1662));
/*
***What is the 1.2*length of the longest view name (uplen) ?
*/
   uplen = 0;

   for ( i=0; i<n_views; ++i )
     if ( WPstrl(namlst[i]) > uplen ) uplen = WPstrl(namlst[i]);
   uplen *= 1.2;
/*
***What is the 1.2*length of the longest text in the lower half
***section of the dialogue (lowlen1) ?
*/
   lowlen1 = 0;
   if ( WPstrl(save)       > lowlen1 ) lowlen1 = WPstrl(save);
   if ( WPstrl(delete)     > lowlen1 ) lowlen1 = WPstrl(delete);
   if ( WPstrl(csys)       > lowlen1 ) lowlen1 = WPstrl(csys);
   if ( WPstrl(camera)     > lowlen1 ) lowlen1 = WPstrl(camera);
   if ( WPstrl(ed_pmt)     > lowlen1 ) lowlen1 = WPstrl(ed_pmt);
   if ( WPVNLN*WPstrl("A") > lowlen1 ) lowlen1 = WPVNLN*WPstrl("A");
   lowlen1 *= 1.2;

   lowlen2 = 0;
   if ( WPstrl(close)      > lowlen2 ) lowlen2 = WPstrl(close);
   if ( WPstrl(help)       > lowlen2 ) lowlen2 = WPstrl(help);
   lowlen2 *= 1.8;
/*
***How many lines of view buttons ?
*/
   radant = (int)(ceil((n_views/4.0)-0.1));
/*
***Calculate outside air (ly), up height (uph), low height (lowh)
***and air between (lm).
*/
   ly   = (short)(0.8*WPstrh());
   uph  = (short)(1.6*WPstrh()); 
   lowh = (short)(2.0*WPstrh());
   lm   = (short)(1.4*WPstrh());
/*
***Upper section is 4 columns of uplen and lower section is 
***2 columns of lowlen1. Which is longest ? Calculate max
***window size in X-direction.
*/
   dx_up  = ly + 4*uplen + 3*lm + ly;
   dx_low = ly + 2*lowlen1 + lm + ly;

   if ( dx_up > dx_low )
      {
      main_dx = dx_up;
      lowlen1  = (main_dx - 2*ly - lm)/2;
      }
   else
      {
      main_dx = dx_low;
      uplen = (main_dx - 2*ly - 3*lm)/4;
      }
/*
***Calculate window size in Y-direction.
*/
   main_dy = ly + radant*uph + (radant-1)*ly +     /* View buttons */
             lowh +                                /* Line */
             lowh +                                /* Csys + camera */
             lowh + ly +                           /* Save + delete */
             lowh + lowh +                         /* Input WPEDIT */
             lowh + lowh +                         /* Line */
             ly + lowh + ly;                       /* Reject + Help */
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
***Create the buttons.
*/
   for ( rad=0; rad<radant; ++rad )
     {
/*
***Button in 1:st column.
*/
     alt_x  = ly;
     alt_y  = ly + rad*(uph + ly);
     str    = namlst[4*rad];

     if ( 4*rad == actalt )
       status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                               str,str,"",WP_BGND3,WP_FGND,&alt_id[4*rad]);
     else
       status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                               str,str,"",WP_BGND2,WP_FGND,&alt_id[4*rad]);
/*
***Button in the 2:nd column.
*/
     if ( n_views > (4*rad)+1 )
       {
       alt_x  = ly + uplen + lm;
       str    = namlst[(4*rad)+1];

       if ( (4*rad)+1 == actalt )
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND3,WP_FGND,&alt_id[(4*rad)+1]);
       else
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND2,WP_FGND,&alt_id[(4*rad)+1]);
       }
/*
***Button in the 3:rd column.
*/
     if ( n_views > (4*rad)+2 )
       {
       alt_x  = ly + uplen + lm + uplen + lm;
       str    = namlst[(4*rad)+2];

       if ( (4*rad)+2 == actalt )
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND3,WP_FGND,&alt_id[(4*rad)+2]);
       else
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND2,WP_FGND,&alt_id[(4*rad)+2]);
       }
/*
***Button in the 4:th column.
*/
     if ( n_views > (4*rad)+3 )
       {
       alt_x  = ly + uplen + lm + uplen + lm + uplen + lm;
       str    = namlst[(4*rad)+3];

       if ( (4*rad)+3 == actalt )
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND3,WP_FGND,&alt_id[(4*rad)+3]);
       else
         status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,uplen,uph,(short)1,
                          str,str,"",WP_BGND2,WP_FGND,&alt_id[(4*rad)+3]);
       }
     }
/*
***A line.
*/
   alt_x  = main_dx/8;;
   alt_y  = radant*(uph + ly) + lowh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,alt_x + 6*main_dx/8,alt_y);
/*
***Csys and Camera.
*/
   alt_x  = ly;
   alt_y  += lowh;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)2,
                           csys,csys,"",WP_BGND2,WP_FGND,&csys_id);
   butptr = (WPBUTT *)iwinpt->wintab[csys_id].ptr;
   strcpy(butptr->tt_str,csystt);

   alt_x  = ly + lowlen1 + lm;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)2,
                           camera,camera,"",WP_BGND2,WP_FGND,&camera_id);
   butptr = (WPBUTT *)iwinpt->wintab[camera_id].ptr;
   strcpy(butptr->tt_str,cameratt);
/*
***Save and delete.
*/
   alt_x  = ly;
   alt_y += ly + lowh;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)2,
                           save,save,"",WP_BGND2,WP_FGND,&save_id);
   butptr = (WPBUTT *)iwinpt->wintab[save_id].ptr;
   strcpy(butptr->tt_str,savett);

   alt_x  = ly + lowlen1 + lm;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)2,
                           delete,delete,"",WP_BGND2,WP_FGND,&delete_id);
   butptr = (WPBUTT *)iwinpt->wintab[delete_id].ptr;
   strcpy(butptr->tt_str,deletett);
/*
***Promt and edit.
*/
   alt_x  = ly;
   alt_y += lowh + lowh;
   status = WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(ed_pmt),lowh,
                           ed_pmt,&edit_id);

   alt_x  = ly + WPstrl(ed_pmt) + lm;
   status = WPmced((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)1,
                   view_name,WPVNLN,&edit_id);
   edtptr = (WPEDIT *)iwinpt->wintab[edit_id].ptr;
   WPfoed(edtptr,TRUE);
/*
***A line.
*/
   alt_x  = main_dx/8;
   alt_y += lowh + lowh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,alt_x + 6*main_dx/8,alt_y);
/*
***Close and help.
*/
   alt_x  = ly;
   alt_y += ly;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen2,lowh,(short)3,
                           close,close,"",WP_BGND2,WP_FGND,&close_id);
   butptr = (WPBUTT *)iwinpt->wintab[close_id].ptr;
   strcpy(butptr->tt_str,closett);

   alt_x  = main_dx - ly - lowlen2;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen2,lowh,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
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
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Csys button. Hide view dialogue during
***selection of coordinate system.
*/
   if ( but_id == csys_id )
     {
     XUnmapWindow(xdisp,iwinpt->id.x_id);
     WPaddmess_mcwin(IGgtts(271),WP_MESSAGE);
     type = CSYTYP;
     status = IGgsid(csy_id,&type,&dum,&dum,0);
     WPclear_mcwin();
     XMapWindow(xdisp,iwinpt->id.x_id);
     if ( status < 0 ) goto loop;

     status = DBget_pointer('I',csy_id,&la,&type);
     if ( status == 0  &&  type == CSYTYP ) DBread_csys(&csy,NULL,la);
     else
       {
       WPbell();
       goto loop;
       }

     if ( wintype == TYP_RWIN )
       {
       rwinpt->vy.matrix.k11 = csy.mat_pl.g11;
       rwinpt->vy.matrix.k12 = csy.mat_pl.g12;
       rwinpt->vy.matrix.k13 = csy.mat_pl.g13;
       rwinpt->vy.matrix.k21 = csy.mat_pl.g21;
       rwinpt->vy.matrix.k22 = csy.mat_pl.g22;
       rwinpt->vy.matrix.k23 = csy.mat_pl.g23;
       rwinpt->vy.matrix.k31 = csy.mat_pl.g31;
       rwinpt->vy.matrix.k32 = csy.mat_pl.g32;
       rwinpt->vy.matrix.k33 = csy.mat_pl.g33;
       WPsodl_all(rwinpt);
       XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                 0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
       }
     else
       {
       gwinpt->vy.matrix.k11 = csy.mat_pl.g11;
       gwinpt->vy.matrix.k12 = csy.mat_pl.g12;
       gwinpt->vy.matrix.k13 = csy.mat_pl.g13;
       gwinpt->vy.matrix.k21 = csy.mat_pl.g21;
       gwinpt->vy.matrix.k22 = csy.mat_pl.g22;
       gwinpt->vy.matrix.k23 = csy.mat_pl.g23;
       gwinpt->vy.matrix.k31 = csy.mat_pl.g31;
       gwinpt->vy.matrix.k32 = csy.mat_pl.g32;
       gwinpt->vy.matrix.k33 = csy.mat_pl.g33;
       WPauto_modwin(gwinpt,&minwin);
       V3MOME(&minwin,&gwinpt->vy.modwin,sizeof(VYWIN));
       WPnrgw(gwinpt);
       WPrepaint_GWIN(gwinpt->id.w_id);
       }
     goto loop;
     }
/*
***Camera button. Hide view dialogue during input
***of position.
*/
   else if ( but_id == camera_id )
     {
     XUnmapWindow(xdisp,iwinpt->id.x_id);
     WPaddmess_mcwin(IGgtts(330),WP_MESSAGE);
     status=IGcpov((DBVector *)&campos);
     WPclear_mcwin();
     XMapWindow(xdisp,iwinpt->id.x_id);
     if ( status < 0 ) WPbell();
     else
       {
       if ( wintype == TYP_RWIN )
         {
         WPcampos_to_matrix(&campos,&rwinpt->vy.matrix);
         WPsodl_all(rwinpt);
         XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                   0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
         }
       else
         {
         WPcampos_to_matrix(&campos,&gwinpt->vy.matrix);
         WPauto_modwin(gwinpt,&minwin);
         V3MOME(&minwin,&gwinpt->vy.modwin,sizeof(VYWIN));
         WPnrgw(gwinpt);
         WPrepaint_GWIN(gwinpt->id.w_id);
         }
       }
     goto loop;
     }
/*
***Save button. Save the current window view in wpviewtab[] and
***make the view active.
*/
   else if ( but_id == save_id )
     {
     WPgted(iwin_id,edit_id,view_name);
     sprintf(buf,"%s: %s",IGgtts(1659),view_name);
     if ( view_name[0] == '\0' )
       {
       WPbell();
       goto loop;
       }
     else if ( WPialt(buf,IGgtts(67),IGgtts(68),FALSE) )
       {
       if ( wintype == TYP_GWIN )
         {
         V3MOME(&gwinpt->vy,&new_view,sizeof(WPVIEW));
         strcpy(new_view.name,view_name);
         strcpy(gwinpt->vy.name,view_name);
         WPcreate_view(&new_view,VIEW_3D_AND_2D);
         WPtitle_GWIN(gwinpt);
         }
       else
         {
         V3MOME(&rwinpt->vy,&new_view,sizeof(WPVIEW));
         strcpy(new_view.name,view_name);
         strcpy(rwinpt->vy.name,view_name);
         WPcreate_view(&new_view,VIEW_3D_ONLY);
         }
       goto update;
       }
     goto loop;
     }
/*
***Delete button.
*/
   else if ( but_id == delete_id )
     {
     WPgted(iwin_id,edit_id,view_name);
     sprintf(buf,"%s: %s",IGgtts(1663),view_name);
     if ( view_name[0] == '\0' )
       {
       WPbell();
       goto loop;
       }
     else if ( WPialt(buf,IGgtts(67),IGgtts(68),FALSE) )
       {
       WPdelete_view(view_name);
       goto update;
       }
     else goto loop;
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
***A view button.
*/
   else
     {
     for ( i=0; i < n_views; ++i )
       {
       if ( but_id == alt_id[i] )
         {
         if ( wintype == TYP_RWIN )
           {
           WPactivate_view(namlst[i],NULL,rwinpt,TYP_RWIN);
           glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
           WPsodl_all(rwinpt);
           XCopyArea(xdisp,rwinpt->id.x_id,rwinpt->savmap,rwinpt->win_gc,
                     0,0,rwinpt->geo.dx,rwinpt->geo.dy,0,0);
           }
         else
           {
           WPactivate_view(namlst[i],gwinpt,NULL,TYP_GWIN);
           WPrepaint_GWIN(gwinpt->id.w_id);
           }
         goto update;
         }
       }
/*
***Unknown event, should not happen.
*/
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
***Time to exit. Reset global actfunc and remeber window position.
*/
exit:
   actfunc = actfunc_org;
   WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
   iwin_x = iwin_x + wm_x2 - wm_x1;
   iwin_y = iwin_y + wm_y2 - wm_y1;
   WPwdel(iwin_id);
/*
***If the view dialog overlaps the WPRWIN 
***(which is rather likley) it seems that the
***area under the view dialog is not restored
***correctly. I can't figure out why. An extra
***WPsodl_all() here does not help but a full
***repaint helps.
*/
  if ( wintype == TYP_RWIN ) WPrepaint_RWIN(rwinpt->id.w_id,FALSE);
/*
***The end.
*/
   return(status);
 }

/********************************************************/
/*!******************************************************/

        short   WPinit_views()

/*      Clears wpviewtab[] and creates some default
 *      views.
 *
 *      (C)2007-02-10 J.Kjellander
 *
 ******************************************************!*/

  {
   int      i;
   DBVector campos;
   WPVIEW   view;

/*
***Clear wpviewtab[].
*/
   for ( i=0; i<WPMAXVIEWS; ++i ) wpviewtab[i].status = VIEW_NOT_USED;
/*
***Create the view "xy".
*/
    strcpy(view.name,"xy");
    campos.x_gm = campos.y_gm = 0.0; campos.z_gm = 1.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "iso".
*/
    strcpy(view.name,"iso");
    campos.x_gm = campos.y_gm = campos.z_gm = 1.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "right".
*/
    strcpy(view.name,"right");
    campos.x_gm = 1.0; campos.y_gm = campos.z_gm = 0.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "left".
*/
    strcpy(view.name,"left");
    campos.x_gm = -1.0; campos.y_gm = campos.z_gm = 0.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "up".
*/
    strcpy(view.name,"up");
    campos.x_gm = 0.0; campos.y_gm = 1.0; campos.z_gm = 0.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "down".
*/
    strcpy(view.name,"down");
    campos.x_gm = 0.0; campos.y_gm = -1.0; campos.z_gm = 0.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "back".
*/
    strcpy(view.name,"back");
    campos.x_gm = campos.y_gm = 0.0; campos.z_gm = -1.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***Create the view "115".
*/
    strcpy(view.name,"115");
    campos.x_gm = 1.0; campos.y_gm = 1.0; campos.z_gm = 5.0;
    WPcampos_to_matrix(&campos,&view.matrix);
    WPcreate_view(&view,VIEW_3D_ONLY);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPcreate_view(
        WPVIEW *view,
        int     status)

/*      Creates/overwrites a view in wpviewtab[].
 *
 *      In: view   => C ptr to WPVIEW
 *          status => 2D/3D
 *
 *      Return:  0 = Ok.
 *              -1 = wpviewtab[] full.
 *
 *      (C)2007-02-10 J.Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***Serach for a view with this name in wpviewtab[]
*/
   for ( i=0; i<WPMAXVIEWS; ++i )
     {
     if ( wpviewtab[i].status > VIEW_NOT_USED  && 
          strcmp(view->name,wpviewtab[i].name) == 0 ) break;
     }
/*
***If the view doesn't exist. Search for an empty pos
***for the new view.
*/
   if ( i == WPMAXVIEWS )
     {
     for ( i=0; i<WPMAXVIEWS; ++i )
       {
       if ( !wpviewtab[i].status > VIEW_NOT_USED ) break;
       }
     }
/*
***If no empty position is found, return -1.
*/
   if ( i == WPMAXVIEWS ) return(-1);
/*
***If the view is 2D and 3D it has a valid modwin.
***This makes it possible to set up a reasonable
***scrwin and calculate temporary 2D transformation
***constants. If everything works as planned these
***will never be used though. When the view is
***activated into a window the scrwin of the window
***will be used instead.
*/
   if ( status == VIEW_3D_AND_2D )
     {
     view->scrwin.xmin = 0;
     view->scrwin.ymin = 0;
     view->scrwin.xmax = 500;
     view->scrwin.ymax = 500;
     view->k2x = (view->scrwin.xmax - view->scrwin.xmin) /
                 (view->modwin.xmax - view->modwin.xmin);
     view->k2y = (view->scrwin.ymax - view->scrwin.ymin) /
                 (view->modwin.ymax - view->modwin.ymin);
     view->k1x = view->scrwin.xmin - view->modwin.xmin * view->k2x;
     view->k1y = view->scrwin.ymin - view->modwin.ymin * view->k2y;
     }
/*
***Store the view in wpviewtab[].
*/
   view->status = status;
   V3MOME(view,&wpviewtab[i],sizeof(WPVIEW));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPdelete_view(
        char   name[])

/*      Deletes a view in wpviewtab[].
 *
 *      In: name   => View name.
 *
 *      Return:  0 = Ok.
 *              -1 = View not found.
 *
 *      (C)2007-02-10 J.Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***Serach for a view with this name in wpviewtab[]
*/
   for ( i=0; i<WPMAXVIEWS; ++i )
     {
     if ( wpviewtab[i].status > VIEW_NOT_USED  && 
          strcmp(name,wpviewtab[i].name) == 0 ) break;
     }
/*
***If the view doesn't exist return -1.
*/
   if ( i == WPMAXVIEWS ) return(-1);
/*
***View found, delete it.
*/
   wpviewtab[i].status = VIEW_NOT_USED;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short     WPcampos_to_matrix(
        DBVector *campos,
        VYTRF    *matrix)

/*      Calculates the 3X3 view transformation defined
 *      by looking at BASIC (0,0,0) from campos with
 *      Y-axis up.
 *
 *      In:   campos => C ptr to 3D position
 *
 *      Out: *matrix => 3X3 view transformation.
 *
 *      Return:  0 = Ok.
 *
 *      (C)2007-02-08 J.Kjellander
 *
 ******************************************************!*/

  {
   DBVector por,pu1,pu2;
   DBTmat   matrix44;

/*
***Calculate the view transformation matrix. If campos
***is on the Y+ use Z down (the view "up"). If
***campos is on Y- use Z up.
*/
   if ( (campos->x_gm*campos->x_gm +
         campos->z_gm*campos->z_gm) < 0.001 )
     {
     if ( campos->y_gm > 0.0 )
       {
       pu1.x_gm = 1.0; pu1.y_gm = 0.0; pu1.z_gm = 0.0;
       pu2.x_gm = 0.0; pu2.y_gm = 0.0; pu2.z_gm = -1.0;
       }
     else
       {
       pu1.x_gm = 1.0; pu1.y_gm = 0.0; pu1.z_gm = 0.0;
       pu2.x_gm = 0.0; pu2.y_gm = 0.0; pu2.z_gm = 1.0;
       }
     }
/*
***Campos is not on the Y-axis. Use Y up.
*/
   else
     {
     pu1.x_gm = campos->z_gm; pu1.y_gm = 0.0; pu1.z_gm = 0.0;

     if ( campos->x_gm != 0.0 ) pu1.z_gm = -campos->x_gm;

     if ( (campos->y_gm*campos->y_gm) < 0.001 )
       {
       pu2.x_gm = 0.0; pu2.y_gm = 1.0; pu2.z_gm = 0.0;
       }
     else
       {
       pu2.x_gm = campos->z_gm*pu1.y_gm +
                  campos->y_gm*pu1.z_gm;
       pu2.y_gm = campos->z_gm*pu1.x_gm -
                  campos->x_gm*pu1.z_gm;
       pu2.z_gm = campos->x_gm*pu1.y_gm -
                  campos->y_gm*pu1.x_gm;
       }
     }

   por.x_gm = por.y_gm = por.z_gm = 0.0;
   GEmktf_3p(&por,&pu1,&pu2,&matrix44);
/*
***Copy elements from 4X4 DBTmat to 3X3 VYTRF.
*/
   matrix->k11 = matrix44.g11;
   matrix->k12 = matrix44.g12;
   matrix->k13 = matrix44.g13;
   matrix->k21 = matrix44.g21;
   matrix->k22 = matrix44.g22;
   matrix->k23 = matrix44.g23;
   matrix->k31 = matrix44.g31;
   matrix->k32 = matrix44.g32;
   matrix->k33 = matrix44.g33;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPreload_view(
        DBint win_id)

/*      Reloads view data från wpviewtab[] into windows.
 *      Used by IG during startup.
 *
 *      In: win_id = ID for WPGWIN or WPRWIN or GWIN_ALL
 *
 *      Felkoder: WP1362 = Fönstret %s finns ej.
 *
 *      (C)2007-02-11 J.Kjellander
 *
 *      2007-08-31 Bugfix WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
   bool    hit;
   int     i;
   char    errbuf[81];
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Initiering.
*/
   hit = FALSE;
/*
***Look upp all WPGWIN's and WPRWIN's and
***activate the view of each window. If the
***view does not exist, activate "xy" instead.
*/
   for ( i=0; i<WTABSIZ; ++i)
     {
     if ( wpwtab[i].ptr != NULL )
       {
       if ( wpwtab[i].typ == TYP_GWIN )
         {
         gwinpt = (WPGWIN *)wpwtab[i].ptr;
         if ( win_id == gwinpt->id.w_id  ||  win_id == GWIN_ALL )
           {
           if ( WPactivate_view(gwinpt->vy.name,gwinpt,NULL,TYP_GWIN) < 0 )
             WPactivate_view("xy",gwinpt,NULL,TYP_GWIN);
           hit = TRUE;
           }
         }
       else if ( wpwtab[i].typ == TYP_RWIN )
         {
         rwinpt = (WPRWIN *)wpwtab[i].ptr;
         if ( win_id == rwinpt->id.w_id  ||  win_id == GWIN_ALL )
           {
           if ( WPactivate_view(rwinpt->vy.name,NULL,rwinpt,TYP_RWIN) < 0 )
             WPactivate_view("xy",NULL,rwinpt,TYP_RWIN);
           hit = TRUE;
           }
         }
       }
     }
/*
***Gick det bra ?
*/
   if ( !hit )
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1362",errbuf));
     }
   else return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPactivate_view(
        char    name[],
        WPGWIN *gwinpt,
        WPRWIN *rwinpt,
        int     wintype)

/*      Activates a view in wpviewtab[].
 *
 *      In: name    = View name.
 *          gwinpt  = C ptr to WPGWIN or NULL.
 *          rwinpt  = C ptr to WPRWIN or NULL.
 *          wintype = TYP_GWIN or TYP_RWIN.
 *
 *      Return:  0 = Ok.
 *              -1 = View not found.
 *
 *      (C)2007-02-07 J.Kjellander
 *
 ******************************************************!*/

  {
   int   i;
   VYWIN minwin;

/*
***Serach for a view with this name in wpviewtab[]
*/
   for ( i=0; i<WPMAXVIEWS; ++i )
     {
     if ( wpviewtab[i].status > VIEW_NOT_USED  && 
          strcmp(name,wpviewtab[i].name) == 0 ) break;
     }
/*
***If no view is found, return -1.
*/
   if ( i == WPMAXVIEWS ) return(-1);
/*
***WPRWIN.
*/
   if ( wintype == TYP_RWIN )
     {
     strcpy(rwinpt->vy.name,name);
     rwinpt->movx = rwinpt->movy = 0.0;
     rwinpt->scale = 1.0;
     V3MOME(&wpviewtab[i].matrix,&rwinpt->vy.matrix,sizeof(VYTRF));
     }
/*
***WPGWIN, if valid, save current view as previous.
***If the view is 2D and 3D copy matrix and modwin
***from wpviewtab[]. If 3D only, create default
***modwin.
*/
   else
     {
     if ( gwinpt->vy.status > VIEW_NOT_USED )
       V3MOME(&gwinpt->vy,&gwinpt->old_vy,sizeof(WPVIEW));
     strcpy(gwinpt->vy.name,name);
     V3MOME(&wpviewtab[i].matrix,&gwinpt->vy.matrix,sizeof(VYTRF));

     if ( wpviewtab[i].status == VIEW_3D_AND_2D )
       {
       V3MOME(&wpviewtab[i].modwin,&gwinpt->vy.modwin,sizeof(VYWIN));
       }
     else
       {
       WPauto_modwin(gwinpt,&minwin);
       V3MOME(&minwin,&gwinpt->vy.modwin,sizeof(VYWIN));
       }
     WPnrgw(gwinpt);
     WPtitle_GWIN(gwinpt);
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short   WPgtvi(
        DBint   win_id,
        char   *vynamn,
        double *skala,
        double *xmin,
        double *ymin,
        double *xmax,
        double *ymax,
        DBTmat *vymat,
        double *persp)

/*      Used by MBS procedure GET_VIEW().
 *
 *      In: win_id = ID of requested window.
 *
 *      Out: *vynamn = View name.
 *           *skala  = Current scale.
 *           *xmin   = Current model window.
 *           *ymin   =         -""-
 *           *xmax   =         -""-
 *           *ymax   =         -""-
 *           *vymat  = Current 3D-transformation.
 *           *persp  = Perspective distance.
 *
 *      Error: WP1402 = Window %s does not exist.
 *             WP1742 = Window %s is of illegal type.
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[V3STRLEN];
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;
   WPVIEW *viewpt;
   WPWGEO *geopt;

/*
***Get C ptrs to the window data.
*/
   if ( (winptr=WPwgwp((wpw_id)win_id)) != NULL )
     {
     if ( winptr->typ == TYP_GWIN )
       {
       gwinpt = (WPGWIN *)winptr->ptr;
       geopt  = &gwinpt->geo;
       viewpt = &gwinpt->vy;
       }
     else if ( winptr->typ == TYP_RWIN )
       {
       rwinpt = (WPRWIN *)winptr->ptr;
       geopt  = &rwinpt->geo;
       viewpt = &rwinpt->vy;
       }
/*
***Illegal window type.
*/
     else
       {
       sprintf(errbuf,"%d",win_id);
       return(erpush("WP1742",errbuf));
       }
     }
/*
***Window does not exist.
*/
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("WP1402",errbuf));
     }
/*
***Return view data.
*/
   strcpy(vynamn,viewpt->name);

  *skala = geopt->psiz_x*
          (viewpt->scrwin.xmax - viewpt->scrwin.xmin) /
          (viewpt->modwin.xmax - viewpt->modwin.xmin);

  *xmin  = viewpt->modwin.xmin;
  *ymin  = viewpt->modwin.ymin;
  *xmax  = viewpt->modwin.xmax;
  *ymax  = viewpt->modwin.ymax;

   vymat->g11 = viewpt->matrix.k11;
   vymat->g12 = viewpt->matrix.k12;
   vymat->g13 = viewpt->matrix.k13;
   vymat->g14 = 0.0;

   vymat->g21 = viewpt->matrix.k21;
   vymat->g22 = viewpt->matrix.k22;
   vymat->g23 = viewpt->matrix.k23;
   vymat->g24 = 0.0;

   vymat->g31 = viewpt->matrix.k31;
   vymat->g32 = viewpt->matrix.k32;
   vymat->g33 = viewpt->matrix.k33;
   vymat->g34 = 0.0;

   vymat->g41 = 0.0;
   vymat->g42 = 0.0;
   vymat->g43 = 0.0;
   vymat->g44 = 1.0;

  *persp = viewpt->pdist;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
