/**********************************************************************
*
*    wpattributes.c
*    ==============
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPatdi();   The attributes dialogue
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

extern V2NAPA defnap;           /* Attributes fir active module */

static char    title[81];       /* Window title */
static char    poitxt[81];      /* Text for Point button */
static char    poitip[81];      /* Tooltip for Point button */
static char    lintxt[81];      /* Text for Line button */
static char    lintip[81];      /* Tooltip for Line button */
static char    arctxt[81];      /* Text for Arc button */
static char    arctip[81];      /* Tooltip for Arc button */
static char    curtxt[81];      /* Text for Curve button */
static char    curtip[81];      /* Tooltip for Curve button */
static char    surtxt[81];      /* Text for Surf button */
static char    surtip[81];      /* Tooltip for Surf button */
static char    txttxt[81];      /* Text for Text button */
static char    txttip[81];      /* Tooltip for Text button */
static char    xhttxt[81];      /* Text for Hatch button */
static char    xhttip[81];      /* Tooltip for Hatch button */
static char    dimtxt[81];      /* Text for Dim button */
static char    dimtip[81];      /* Tooltip for Dim button */
static char    okey[81];        /* Okey */
static char    okeytt[81];      /* Okey tooltip */
static char    reject[81];      /* Reject */
static char    rejecttt[81];    /* Reject tooltip */
static char    help[81];        /* Help */
static char    helptt[81];      /* Help tooltip */

static int     actbut = 1;      /* Active button, 1=Point, 2=Line etc. */
static int     butlen1;         /* Length of etype buttons */
static int     butlen2;         /* Length of Close- and Help button */
static int     ly,lm,bh;        /* Layout geometry */
static int     iwin_id;         /* ID of WPIWIN */
static WPIWIN *iwinpt;          /* C ptr to WPIWIN */
static int     ebut_id[8];      /* ID of the 8 etype buttons. */

static bool    premiere = TRUE; /* True only for the first invocation */
static int     main_x;          /* The current position of the window */
static int     main_y;          /* even after a user move */
static int     main_dx;         /* Current window size in X-direction */
static int     main_dy;         /* Current window size in Y-direction */

/*
***Prototypes for internal functions.
*/
static void  create_etype_buttons();
static int   poi_atts();
static int   lin_atts();
static int   arc_atts();
static int   cur_atts();
static int   sur_atts();
static int   txt_atts();
static int   xht_atts();
static int   dim_atts();

/*!******************************************************/

        short WPatdi()

/*      Main entry for the attributes dialogue.
 * 
 *
 *      (C)2007-03-22 J.Kjellander
 *
 ******************************************************!*/

  {
   unsigned int dum1,dum2;
   int          i,status,wm_x1,wm_y1,wm_x2,wm_y2;
   char        *type[20];
   XrmValue     value;
   XSizeHints  *size_hints;
   XEvent       event;

/*
***Is this the first time this function is called ?
***Initial window position.
*/
   if ( premiere )
     {
     main_x = main_y = 5;

     if ( XrmGetResource(xresDB,"varkon.attribute.geometry",
                                "Varkon.Attribute.Geometry",
                                 type,&value) ) 
       XParseGeometry((char *)value.addr,&main_x,&main_y,&dum1,&dum2);

     premiere = FALSE;
     }
/*
***Attributes window title.
*/
   if ( !WPgrst("varkon.attribute.title",title) ) strcpy(title,"Attributes");
/*
***Text and tooltip for button 1, Points.
*/
   if ( !WPgrst("varkon.attribute.point.text",poitxt) ) strcpy(poitxt,"Point");
   if ( !WPgrst("varkon.attribute.point.tooltip",poitip) ) strcpy(poitip,"");
/*
***Text and tooltip for button 2, Lines.
*/
   if ( !WPgrst("varkon.attribute.line.text",lintxt) ) strcpy(lintxt,"Line");
   if ( !WPgrst("varkon.attribute.line.tooltip",lintip) ) strcpy(lintip,"");
/*
***Text and tooltip for button 3, Arcs.
*/
   if ( !WPgrst("varkon.attribute.arc.text",arctxt) ) strcpy(arctxt,"Arc");
   if ( !WPgrst("varkon.attribute.arc.tooltip",arctip) ) strcpy(arctip,"");
/*
***Text and tooltip for button 4, Curves.
*/
   if ( !WPgrst("varkon.attribute.curve.text",curtxt) ) strcpy(curtxt,"Curve");
   if ( !WPgrst("varkon.attribute.curve.tooltip",curtip) ) strcpy(curtip,"");
/*
***Text and tooltip for button 5, Surfaces.
*/
   if ( !WPgrst("varkon.attribute.surf.text",surtxt) ) strcpy(surtxt,"Surf");
   if ( !WPgrst("varkon.attribute.surf.tooltip",surtip) ) strcpy(surtip,"");
/*
***Text and tooltip for button 6, Text.
*/
   if ( !WPgrst("varkon.attribute.text.text",txttxt) ) strcpy(txttxt,"Text");
   if ( !WPgrst("varkon.attribute.text.tooltip",txttip) ) strcpy(txttip,"");
/*
***Text and tooltip for button 7, Hatch.
*/
   if ( !WPgrst("varkon.attribute.hatch.text",xhttxt) ) strcpy(xhttxt,"Hatch");
   if ( !WPgrst("varkon.attribute.hatch.tooltip",xhttip) ) strcpy(xhttip,"");
/*
***Text and tooltip for button 8, Dim (and Xht).
*/
   if ( !WPgrst("varkon.attribute.dim.text",dimtxt) ) strcpy(dimtxt,"Dim");
   if ( !WPgrst("varkon.attribute.dim.tooltip",dimtip) ) strcpy(dimtip,"");
/*
***What is the 1.2*length of the longest text ?
***Don't include the title, it will fit anyhow.
*/
   butlen1 = 0;
   if ( WPstrl(poitxt)  > butlen1 ) butlen1 = WPstrl(poitxt);
   if ( WPstrl(lintxt)  > butlen1 ) butlen1 = WPstrl(lintxt);
   if ( WPstrl(arctxt)  > butlen1 ) butlen1 = WPstrl(arctxt);
   if ( WPstrl(curtxt)  > butlen1 ) butlen1 = WPstrl(curtxt);
   if ( WPstrl(surtxt)  > butlen1 ) butlen1 = WPstrl(surtxt);
   if ( WPstrl(txttxt)  > butlen1 ) butlen1 = WPstrl(txttxt);
   if ( WPstrl(xhttxt)  > butlen1 ) butlen1 = WPstrl(xhttxt);
   if ( WPstrl(dimtxt)  > butlen1 ) butlen1 = WPstrl(dimtxt);
   butlen1 *= 1.2;
/*
***Okey, reject and help.
*/
   if ( !WPgrst("varkon.input.okey",okey) )               strcpy(okey,"Okey");
   if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )     strcpy(okeytt,"");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");

   butlen2 = 0;
   if ( WPstrl(okey)   > butlen2 ) butlen2 = WPstrl(okey);
   if ( WPstrl(reject) > butlen2 ) butlen2 = WPstrl(reject);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.8;
/*
***Calculate outside air (ly), button height (bh) and air between (lm).
*/
   ly = 1.2*WPstrh();
   bh = 1.8*WPstrh();
   lm = 0.8*WPstrh();
   main_dx = 8*butlen1;
/*
***Create a default main window and map without subwindows.
*/
   main_dy = 100;
   status = WPwciw(main_x,main_y,main_dx,main_dy,title,&iwin_id);
   if ( status < 0 ) return(status);
   iwinpt = (WPIWIN *)wpwtab[(wpw_id)iwin_id].ptr;

   WPwshw(iwin_id);
   iwinpt->mapped = FALSE;
/*
***Where did it actually get positioned ?
***This code copied from WPmsip().
*/
   XWindowEvent(xdisp,iwinpt->id.x_id,ExposureMask,&event);
   XPutBackEvent(xdisp,&event);
   WPgtwp(iwinpt->id.x_id,&wm_x1,&wm_y1);
/*
***Which entity type is active ? Calculate the required
***window size in Y-direction.
*/
loop:
   switch ( actbut )
     {
/*
***Point.
*/
     case 1:
     main_dy = bh + ly + ly + bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Line.
*/
     case 2:
     main_dy = bh + ly + ly + bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Arc.
*/
     case 3:
     main_dy = bh + ly + ly + bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Curve.
*/
     case 4:
     main_dy = bh + ly + ly + bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Surface.
*/
     case 5:
     main_dy = bh + ly + ly + bh + ly + bh + ly + bh +
               bh + ly + bh + ly;
     break;
/*
***Text.
*/
     case 6:
     main_dy = bh + ly + ly + bh + ly + bh + ly + bh + ly +
               bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Hatch.
*/
     case 7:
     main_dy = bh + ly + ly + bh + ly + bh + bh + ly + bh + ly;
     break;
/*
***Dimension.
*/
     case 8:
     main_dy = bh + ly + ly + bh + ly + bh + ly + bh + ly +
               bh + ly + bh + bh + ly;
     break;
/*
***Unknown ?
*/
     default:
     actbut = 1;
     goto loop;
     }
/*
***Resize the window and create the top 8 buttons.
*/
   size_hints = XAllocSizeHints();
   size_hints->flags       = PSize; 
   size_hints->base_width  = main_dx;
   size_hints->base_height = main_dy; 
   XSetWMNormalHints(xdisp,iwinpt->id.x_id,size_hints);
   XFree(size_hints);

   iwinpt->geo.dx = main_dx;
   iwinpt->geo.dy = main_dy;
   XResizeWindow(xdisp,iwinpt->id.x_id,main_dx,main_dy);

   create_etype_buttons();
/*
***Call the handler of this entity type.
*/
   switch ( actbut )
     {
     case 1: status = poi_atts(); break;
     case 2: status = lin_atts(); break;
     case 3: status = arc_atts(); break;
     case 4: status = cur_atts(); break;
     case 5: status = sur_atts(); break;
     case 6: status = txt_atts(); break;
     case 7: status = xht_atts(); break;
     case 8: status = dim_atts(); break;
     }
/*
***Negative status means CLOSE. Zero means continue, 
***probably with a new value for actbut. Delete all subwindows
***and start over again.
*/
     if ( status == 0 )
       {
       for ( i=0; i<WP_IWSMAX; ++i )
         {
         if ( iwinpt->wintab[i].ptr != NULL )
           {
           if ( iwinpt->wintab[i].typ == TYP_BUTTON )
             {
             WPdlbu((WPBUTT *)iwinpt->wintab[i].ptr);
             }
           else if ( iwinpt->wintab[i].typ == TYP_EDIT )
             {
             WPdled((WPEDIT *)iwinpt->wintab[i].ptr);
             }
           iwinpt->wintab[i].ptr = NULL;
           iwinpt->wintab[i].typ = TYP_UNDEF;
           }
         }

       XDestroySubwindows(xdisp,iwinpt->id.x_id);
       goto loop;
       }
/*
***The end.
*/
     else
       {
/*
***Remember current window position (user may have moved it).
*/
       WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
       main_x = main_x + wm_x2 - wm_x1;
       main_y = main_y + wm_y2 - wm_y1;
       WPwdel(iwin_id);
       return(0);
       }
  }

/********************************************************/
/*!******************************************************/

 static int poi_atts()

/*      Creates the buttons and edits for point
 *      attributes and handles their event loop.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],fnt2tip[81],siztip[81],
           defstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut;
   DBint   but_id,okey_id,reject_id,help_id,size_id,fnt0_id,
           fnt1_id,fnt2_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Point tooltips.
*/
   if ( !WPgrst("varkon.attribute.point.font0",fnt0tip) ) strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.point.font1",fnt1tip) ) strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.point.font2",fnt2tip) ) strcpy(fnt2tip,"");
   if ( !WPgrst("varkon.attribute.point.size",siztip) )   strcpy(siztip,"");
/*
***Point window geometry.
*/
   butlen  = 1.2*WPstrl("PFONT :");
   edlen   = 1.2*WPstrl("123.456_");

   if      ( defnap.pfont == 0 ) actfbut = 0;
   else if ( defnap.pfont == 1 ) actfbut = 1;
   else if ( defnap.pfont == 2 ) actfbut = 2;
   else                          actfbut = 100;
/*
***PFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"PFONT :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);

   butx  += fbutlen + ly;
   if ( actfbut == 2 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND3,WP_FGND,&fnt2_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND2,WP_FGND,&fnt2_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
   strcpy(butptr->tt_str,fnt2tip);
/*
***PSIZE label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"PSIZE :",&but_id);

   butx  += butlen + ly;
   sprintf(defstr,"%g",defnap.psize);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,defstr,10,&size_id);
   edtptr = (WPEDIT *)iwinpt->wintab[size_id].ptr;
   strcpy(edtptr->tt_str,siztip);
   WPfoed(edtptr,TRUE);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 2.
*/
   else if ( but_id == fnt2_id )
     {
     actfbut = 2;
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,size_id,attstr);
     if ( strcmp(attstr,defstr) != 0 )
       {
       if ( IGset_active_attribute(PMPSIZE,attstr) < 0 ) goto loop;
       }
     if ( defnap.pfont != actfbut  &&  actfbut != 100 )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMPFONT,attstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int lin_atts()

/*      Creates the buttons and edits for line
 *      attributes and handles their event loop.
 *
 *      (C)2007-03-25 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],fnt2tip[81],dshtip[81],
           defstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut;
   DBint   but_id,okey_id,reject_id,help_id,dash_id,fnt0_id,
           fnt1_id,fnt2_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Line tooltips.
*/
   if ( !WPgrst("varkon.attribute.line.font0",fnt0tip) ) strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.line.font1",fnt1tip) ) strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.line.font2",fnt2tip) ) strcpy(fnt2tip,"");
   if ( !WPgrst("varkon.attribute.line.dashl",dshtip) )  strcpy(dshtip,"");
/*
***Line window geometry.
*/
   butlen  = 1.2*WPstrl("LDASHL :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***Font toggle buttons default state. If the active line font
***is not 0, 1 or 2 set actfbut to a crazy value (100) so that
***none of the buttons will be active.
*/
   if      ( defnap.lfont == 0 ) actfbut = 0;
   else if ( defnap.lfont == 1 ) actfbut = 1;
   else if ( defnap.lfont == 2 ) actfbut = 2;
   else                          actfbut = 100;
/*
***LFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"LFONT  :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);

   butx  += fbutlen + ly;
   if ( actfbut == 2 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND3,WP_FGND,&fnt2_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND2,WP_FGND,&fnt2_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
   strcpy(butptr->tt_str,fnt2tip);
/*
***LDASHL label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"LDASHL :",&but_id);

   butx  += butlen + ly;
   sprintf(defstr,"%g",defnap.ldashl);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,defstr,10,&dash_id);
   edtptr = (WPEDIT *)iwinpt->wintab[dash_id].ptr;
   strcpy(edtptr->tt_str,dshtip);
   WPfoed(edtptr,TRUE);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 2.
*/
   else if ( but_id == fnt2_id )
     {
     actfbut = 2;
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,dash_id,attstr);
     if ( strcmp(attstr,defstr) != 0 )
       {
       if ( IGset_active_attribute(PMLDASHL,attstr) < 0 ) goto loop;
       }
     if ( defnap.lfont != actfbut  &&  actfbut != 100 )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMLFONT,attstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int arc_atts()

/*      Creates the buttons and edits for arc
 *      attributes and handles their event loop.
 *
 *      (C)2007-03-25 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],fnt2tip[81],dshtip[81],
           defstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut;
   DBint   but_id,okey_id,reject_id,help_id,dash_id,fnt0_id,
           fnt1_id,fnt2_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Arc tooltips.
*/
   if ( !WPgrst("varkon.attribute.arc.font0",fnt0tip) ) strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.arc.font1",fnt1tip) ) strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.arc.font2",fnt2tip) ) strcpy(fnt2tip,"");
   if ( !WPgrst("varkon.attribute.arc.dashl",dshtip) )  strcpy(dshtip,"");
/*
***Arc window geometry.
*/
   butlen  = 1.2*WPstrl("ADASHL :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***Font toggle buttons default state. If the active arc font
***is not 0, 1 or 2 set actfbut to a crazy value (100) so that
***none of the buttons will be active.
*/
   if      ( defnap.afont == 0 ) actfbut = 0;
   else if ( defnap.afont == 1 ) actfbut = 1;
   else if ( defnap.afont == 2 ) actfbut = 2;
   else                          actfbut = 100;
/*
***AFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"AFONT  :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);

   butx  += fbutlen + ly;
   if ( actfbut == 2 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND3,WP_FGND,&fnt2_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND2,WP_FGND,&fnt2_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
   strcpy(butptr->tt_str,fnt2tip);
/*
***ADASHL label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"ADASHL :",&but_id);

   butx  += butlen + ly;
   sprintf(defstr,"%g",defnap.adashl);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,defstr,10,&dash_id);
   edtptr = (WPEDIT *)iwinpt->wintab[dash_id].ptr;
   strcpy(edtptr->tt_str,dshtip);
   WPfoed(edtptr,TRUE);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 2.
*/
   else if ( but_id == fnt2_id )
     {
     actfbut = 2;
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,dash_id,attstr);
     if ( strcmp(attstr,defstr) != 0 )
       {
       if ( IGset_active_attribute(PMADASHL,attstr) < 0 ) goto loop;
       }
     if ( defnap.afont != actfbut  &&  actfbut != 100 )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMAFONT,attstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int cur_atts()

/*      Creates the buttons and edits for curve
 *      attributes and handles their event loop.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],fnt2tip[81],dshtip[81],
           defstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut;
   DBint   but_id,okey_id,reject_id,help_id,dash_id,fnt0_id,
           fnt1_id,fnt2_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Curve tooltips.
*/
   if ( !WPgrst("varkon.attribute.curve.font0",fnt0tip) ) strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.curve.font1",fnt1tip) ) strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.curve.font2",fnt2tip) ) strcpy(fnt2tip,"");
   if ( !WPgrst("varkon.attribute.curve.dashl",dshtip) )  strcpy(dshtip,"");
/*
***Curve window geometry.
*/
   butlen  = 1.2*WPstrl("CDASHL :");
   edlen   = 1.2*WPstrl("123.456_");

   if      ( defnap.cfont == 0 ) actfbut = 0;
   else if ( defnap.cfont == 1 ) actfbut = 1;
   else if ( defnap.cfont == 2 ) actfbut = 2;
   else                          actfbut = 100;
/*
***CFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"CFONT  :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);

   butx  += fbutlen + ly;
   if ( actfbut == 2 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND3,WP_FGND,&fnt2_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND2,WP_FGND,&fnt2_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
   strcpy(butptr->tt_str,fnt2tip);
/*
***CDASHL label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"CDASHL :",&but_id);

   butx  += butlen + ly;
   sprintf(defstr,"%g",defnap.cdashl);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,defstr,10,&dash_id);
   edtptr = (WPEDIT *)iwinpt->wintab[dash_id].ptr;
   strcpy(edtptr->tt_str,dshtip);
   WPfoed(edtptr,TRUE);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 2.
*/
   else if ( but_id == fnt2_id )
     {
     actfbut = 2;
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,dash_id,attstr);
     if ( strcmp(attstr,defstr) != 0 )
       {
       if ( IGset_active_attribute(PMCDASHL,attstr) < 0 ) goto loop;
       }
     if ( defnap.cfont != actfbut  &&  actfbut != 100 )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMCFONT,attstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int sur_atts()

/*      Creates the buttons and edits for surface
 *      attributes and handles their event loop.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   char    dshtip[81],nultip[81],nvltip[81],dshstr[81],
           nulstr[81],nvlstr[81],ddshstr[81],dnulstr[81],
           dnvlstr[81];
   int     i,butx,buty,butlen,edlen;
   DBint   but_id,okey_id,reject_id,help_id,dash_id,nul_id,
           nvl_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Surface tooltips.
*/
   if ( !WPgrst("varkon.attribute.surf.dashl",dshtip) )   strcpy(dshtip,"");
   if ( !WPgrst("varkon.attribute.surf.nulines",nultip) ) strcpy(nultip,"");
   if ( !WPgrst("varkon.attribute.surf.nvlines",nvltip) ) strcpy(nvltip,"");
/*
***Surface window geometry.
*/
   butlen  = 1.2*WPstrl("SDASHL :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***SDASHL label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"SDASHL  :",&but_id);

   butx  += butlen + ly;
   sprintf(ddshstr,"%g",defnap.sdashl);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,ddshstr,10,&dash_id);
   edtptr = (WPEDIT *)iwinpt->wintab[dash_id].ptr;
   strcpy(edtptr->tt_str,dshtip);
   WPfoed(edtptr,TRUE);
/*
***NULINES label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"NULINES :",&but_id);

   butx  += butlen + ly;
   sprintf(dnulstr,"%d",defnap.nulines);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dnulstr,10,&nul_id);
   edtptr = (WPEDIT *)iwinpt->wintab[nul_id].ptr;
   strcpy(edtptr->tt_str,nultip);
/*
***NVLINES label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"NVLINES :",&but_id);

   butx  += butlen + ly;
   sprintf(dnvlstr,"%d",defnap.nvlines);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dnvlstr,10,&nvl_id);
   edtptr = (WPEDIT *)iwinpt->wintab[nvl_id].ptr;
   strcpy(edtptr->tt_str,nvltip);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Okey.
*/
   if ( but_id == okey_id )
     {
     WPgted(iwin_id,dash_id,dshstr);
     if ( strcmp(dshstr,ddshstr) != 0 )
       {
       if ( IGset_active_attribute(PMSDASHL,dshstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,nul_id,nulstr);
     if ( strcmp(nulstr,dnulstr) != 0 )
       {
       if ( IGset_active_attribute(PMNULIN,nulstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,nvl_id,nvlstr);
     if ( strcmp(nvlstr,dnvlstr) != 0 )
       {
       if ( IGset_active_attribute(PMNVLIN,nvlstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int txt_atts()

/*      Creates the buttons and edits for text
 *      attributes and handles their event loop.
 *
 *      (C)2007-03-25 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],siztip[81],wdttip[81],
           snttip[81],tpm0tip[81],tpm1tip[81],sizstr[81],
           wdtstr[81],sntstr[81],dsizstr[81],dwdtstr[81],
           dsntstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut,acttbut;
   DBint   but_id,okey_id,reject_id,help_id,size_id,width_id,
           slant_id,fnt0_id,fnt1_id,tpm0_id,tpm1_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Text tooltips.
*/
   if ( !WPgrst("varkon.attribute.text.size",siztip) )     strcpy(siztip,"");
   if ( !WPgrst("varkon.attribute.text.width",wdttip) )    strcpy(wdttip,"");
   if ( !WPgrst("varkon.attribute.text.slant",snttip) )    strcpy(snttip,"");
   if ( !WPgrst("varkon.attribute.text.font0",fnt0tip) )   strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.text.font1",fnt1tip) )   strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.text.tpmode0",tpm0tip) ) strcpy(tpm0tip,"");
   if ( !WPgrst("varkon.attribute.text.tpmode1",tpm1tip) ) strcpy(tpm1tip,"");
/*
***Text window geometry.
*/
   butlen  = 1.2*WPstrl("TWIDTH :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***Font and Tpmode toggle buttons default state.
*/
   if ( defnap.tfont == 0 ) actfbut = 0;
   else                     actfbut = 1;

   if ( defnap.tpmode == 0 ) acttbut = 0;
   else                      acttbut = 1;
/*
***TSIZE label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"TSIZE  :",&but_id);

   butx  += butlen + ly;
   sprintf(dsizstr,"%g",defnap.tsize);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dsizstr,10,&size_id);
   edtptr = (WPEDIT *)iwinpt->wintab[size_id].ptr;
   strcpy(edtptr->tt_str,siztip);
   WPfoed(edtptr,TRUE);
/*
***TWIDTH label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"TWIDTH :",&but_id);

   butx  += butlen + ly;
   sprintf(dwdtstr,"%g",defnap.twidth);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dwdtstr,10,&width_id);
   edtptr = (WPEDIT *)iwinpt->wintab[width_id].ptr;
   strcpy(edtptr->tt_str,wdttip);
/*
***TSLANT label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"TSLANT :",&but_id);

   butx  += butlen + ly;
   sprintf(dsntstr,"%g",defnap.tslant);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dsntstr,10,&slant_id);
   edtptr = (WPEDIT *)iwinpt->wintab[slant_id].ptr;
   strcpy(edtptr->tt_str,snttip);
/*
***TFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"TFONT  :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);
/*
***TPMODE label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"TPMODE :",&but_id);

   butx  += butlen + ly;
   if ( acttbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&tpm0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&tpm0_id);
   butptr = (WPBUTT *)iwinpt->wintab[tpm0_id].ptr;
   strcpy(butptr->tt_str,tpm0tip);

   butx  += fbutlen + ly;
   if ( acttbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&tpm1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&tpm1_id);
   butptr = (WPBUTT *)iwinpt->wintab[tpm1_id].ptr;
   strcpy(butptr->tt_str,tpm1tip);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Tpmode 0.
*/
   else if ( but_id == tpm0_id )
     {
     acttbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[tpm0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[tpm1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Tpmode 1.
*/
   else if ( but_id == tpm1_id )
     {
     acttbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[tpm1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[tpm0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,size_id,sizstr);
     if ( strcmp(sizstr,dsizstr) != 0 )
       {
       if ( IGset_active_attribute(PMTSIZE,sizstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,width_id,wdtstr);
     if ( strcmp(wdtstr,dwdtstr) != 0 )
       {
       if ( IGset_active_attribute(PMTWIDTH,wdtstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,slant_id,sntstr);
     if ( strcmp(sntstr,dsntstr) != 0 )
       {
       if ( IGset_active_attribute(PMTSLANT,sntstr) < 0 ) goto loop;
       }

     if ( defnap.tfont != actfbut )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMTFONT,attstr) < 0 ) goto loop;
       }

     if ( defnap.tpmode != acttbut )
       {
       sprintf(attstr,"%d",acttbut);
       if ( IGset_active_attribute(PMTPMODE,attstr) < 0 ) goto loop;
       }

     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int xht_atts()

/*      Creates the buttons and edits for hatch
 *      attributes and handles their event loop.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   char    fnt0tip[81],fnt1tip[81],fnt2tip[81],dshtip[81],
           defstr[81],attstr[81];
   int     i,butx,buty,butlen,edlen,fbutlen,actfbut;
   DBint   but_id,okey_id,reject_id,help_id,dash_id,fnt0_id,
           fnt1_id,fnt2_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Hatch tooltips.
*/
   if ( !WPgrst("varkon.attribute.hatch.font0",fnt0tip) ) strcpy(fnt0tip,"");
   if ( !WPgrst("varkon.attribute.hatch.font1",fnt1tip) ) strcpy(fnt1tip,"");
   if ( !WPgrst("varkon.attribute.hatch.font2",fnt2tip) ) strcpy(fnt2tip,"");
   if ( !WPgrst("varkon.attribute.hatch.dashl",dshtip) )  strcpy(dshtip,"");
/*
***Hatch window geometry.
*/
   butlen  = 1.2*WPstrl("XDASHL :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***Font toggle buttons default state. If the active hatch font
***is not 0, 1 or 2 set actfbut to a crazy value (100) so that
***none of the buttons will be active.
*/
   if      ( defnap.xfont == 0 ) actfbut = 0;
   else if ( defnap.xfont == 1 ) actfbut = 1;
   else if ( defnap.xfont == 2 ) actfbut = 2;
   else                          actfbut = 100;
/*
***XFONT label and buttons.
*/
   fbutlen = 1.2*WPstrl("AAA");
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"XFONT  :",&but_id);

   butx  += butlen + ly;
   if ( actfbut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&fnt0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&fnt0_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
   strcpy(butptr->tt_str,fnt0tip);

   butx  += fbutlen + ly;
   if ( actfbut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&fnt1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&fnt1_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
   strcpy(butptr->tt_str,fnt1tip);

   butx  += fbutlen + ly;
   if ( actfbut == 2 )
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND3,WP_FGND,&fnt2_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,fbutlen,bh,(short)1,
                    "2","2","",WP_BGND2,WP_FGND,&fnt2_id);
   butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
   strcpy(butptr->tt_str,fnt2tip);
/*
***XDASHL label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"XDASHL :",&but_id);

   butx  += butlen + ly;
   sprintf(defstr,"%g",defnap.xdashl);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,defstr,10,&dash_id);
   edtptr = (WPEDIT *)iwinpt->wintab[dash_id].ptr;
   strcpy(edtptr->tt_str,dshtip);
   WPfoed(edtptr,TRUE);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Font 0.
*/
   if ( but_id == fnt0_id )
     {
     actfbut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 1.
*/
   else if ( but_id == fnt1_id )
     {
     actfbut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Font 2.
*/
   else if ( but_id == fnt2_id )
     {
     actfbut = 2;
     butptr = (WPBUTT *)iwinpt->wintab[fnt2_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[fnt0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     butptr = (WPBUTT *)iwinpt->wintab[fnt1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,dash_id,attstr);
     if ( strcmp(attstr,defstr) != 0 )
       {
       if ( IGset_active_attribute(PMXDASHL,attstr) < 0 ) goto loop;
       }
     if ( defnap.xfont != actfbut  &&  actfbut != 100 )
       {
       sprintf(attstr,"%d",actfbut);
       if ( IGset_active_attribute(PMXFONT,attstr) < 0 ) goto loop;
       }
     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int dim_atts()

/*      Creates the buttons and edits for dimension
 *      attributes and handles their event loop.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   char    tsiztip[81],asiztip[81],ndigtip[81],autotip[81],
           tsizstr[81],asizstr[81],ndigstr[81],attstr[81],
           dtsizstr[81],dasizstr[81],dndigstr[81];
   int     i,butx,buty,butlen,edlen,abutlen,actabut;
   DBint   but_id,okey_id,reject_id,help_id,tsiz_id,asiz_id,
           ndig_id,auto0_id,auto1_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Dimension tooltips.
*/
   if ( !WPgrst("varkon.attribute.dim.tsize",tsiztip) )  strcpy(tsiztip,"");
   if ( !WPgrst("varkon.attribute.dim.asize",asiztip) )  strcpy(asiztip,"");
   if ( !WPgrst("varkon.attribute.dim.ndig",ndigtip) )   strcpy(ndigtip,"");
   if ( !WPgrst("varkon.attribute.dim.auto",autotip) )   strcpy(autotip,"");
/*
***Dimension window geometry.
*/
   butlen  = 1.2*WPstrl("DTSIZ :");
   edlen   = 1.2*WPstrl("123.456_");
/*
***DAUTO toggle buttons default state.
*/
   if ( defnap.dauto == 0 ) actabut = 0;
   else                     actabut = 1;
/*
***DTSIZ label and edit.
*/
   butx = (main_dx - (butlen + ly + edlen))/3;
   buty = bh + ly + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"DTSIZ :",&but_id);

   butx  += butlen + ly;
   sprintf(dtsizstr,"%g",defnap.dtsize);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dtsizstr,10,&tsiz_id);
   edtptr = (WPEDIT *)iwinpt->wintab[tsiz_id].ptr;
   strcpy(edtptr->tt_str,tsiztip);
   WPfoed(edtptr,TRUE);
/*
***DASIZ label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"DASIZ :",&but_id);

   butx  += butlen + ly;
   sprintf(dasizstr,"%g",defnap.dasize);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dasizstr,10,&asiz_id);
   edtptr = (WPEDIT *)iwinpt->wintab[asiz_id].ptr;
   strcpy(edtptr->tt_str,asiztip);
/*
***DNDIG label and edit.
*/
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"DNDIG :",&but_id);

   butx  += butlen + ly;
   sprintf(dndigstr,"%d",defnap.dndig);
   WPmced((wpw_id)iwin_id,butx,buty,edlen,bh,(short)1,dndigstr,10,&ndig_id);
   edtptr = (WPEDIT *)iwinpt->wintab[ndig_id].ptr;
   strcpy(edtptr->tt_str,ndigtip);
/*
***DAUTO label and buttons.
*/
   abutlen = 1.2*WPstrl("AAA");
   butx  = (main_dx - (butlen + ly + edlen))/3;
   buty += bh + ly;
   WPcrlb((wpw_id)iwin_id,butx,buty,butlen,bh,"DAUTO :",&but_id);

   butx  += butlen + ly;
   if ( actabut == 0 )
     WPcrpb((wpw_id)iwin_id,butx,buty,abutlen,bh,(short)1,
                    "0","0","",WP_BGND3,WP_FGND,&auto0_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,abutlen,bh,(short)1,
                    "0","0","",WP_BGND2,WP_FGND,&auto0_id);
   butptr = (WPBUTT *)iwinpt->wintab[auto0_id].ptr;
   strcpy(butptr->tt_str,autotip);

   butx  += abutlen + ly;
   if ( actabut == 1 )
     WPcrpb((wpw_id)iwin_id,butx,buty,abutlen,bh,(short)1,
                    "1","1","",WP_BGND3,WP_FGND,&auto1_id);
   else
     WPcrpb((wpw_id)iwin_id,butx,buty,abutlen,bh,(short)1,
                    "1","1","",WP_BGND2,WP_FGND,&auto1_id);
   butptr = (WPBUTT *)iwinpt->wintab[auto1_id].ptr;
   strcpy(butptr->tt_str,autotip);
/*
***A 3D line.
*/
   buty += bh + bh;
   WPcreate_3Dline(iwin_id,main_dx/8,buty,7*main_dx/8,buty);
/*
***Okey, reject and help buttons.
*/
   butx = ly;
   buty += ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)3,
                  okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   butx += butlen2 + ly;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   butx = main_dx - ly - butlen2;
   WPcrpb((wpw_id)iwin_id,butx,buty,butlen2,bh,(short)2,
                  help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Map all subwindows now.
*/
   XMapSubwindows(xdisp,iwinpt->id.x_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   if ( WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id) == SMBPOSM ) goto loop;
/*
***Any of the 8 type buttons ?
*/
   for ( i=0; i<8; ++i )
     {
     if ( but_id == ebut_id[i] )
       {
       actbut = i + 1;
       return(0);
       }
     }
/*
***Auto 0.
*/
   if ( but_id == auto0_id )
     {
     actabut = 0;
     butptr = (WPBUTT *)iwinpt->wintab[auto0_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[auto1_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Auto 1.
*/
   else if ( but_id == auto1_id )
     {
     actabut = 1;
     butptr = (WPBUTT *)iwinpt->wintab[auto1_id].ptr;
     WPscbu(butptr,WP_BGND3);
     butptr = (WPBUTT *)iwinpt->wintab[auto0_id].ptr;
     WPscbu(butptr,WP_BGND2);
     goto loop;
     }
/*
***Okey.
*/
   else if ( but_id == okey_id )
     {
     WPgted(iwin_id,tsiz_id,tsizstr);
     if ( strcmp(tsizstr,dtsizstr) != 0 )
       {
       if ( IGset_active_attribute(PMDTSIZE,tsizstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,asiz_id,asizstr);
     if ( strcmp(asizstr,dasizstr) != 0 )
       {
       if ( IGset_active_attribute(PMDASIZE,asizstr) < 0 ) goto loop;
       }

     WPgted(iwin_id,ndig_id,ndigstr);
     if ( strcmp(ndigstr,dndigstr) != 0 )
       {
       if ( IGset_active_attribute(PMDNDIG,ndigstr) < 0 ) goto loop;
       }

     if ( defnap.dauto != actabut )
       {
       sprintf(attstr,"%d",actabut);
       if ( IGset_active_attribute(PMDAUTO,attstr) < 0 ) goto loop;
       }

     return(-1);
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     return(-1);
     }
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Events that we don't care about, i.e. focus change
***between WPEDIT's.
*/
   else goto loop;
/*
***Aviod compilation warnings.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static void create_etype_buttons()

/*      Creates the top 8 buttons in the Attributes
 *      dialogue.
 *
 *      (C)2007-04-07 J. Kjellander
 *
 ******************************************************!*/

  {
   short   butx;
   WPBUTT *butptr;

/*
***Create the 8 buttons and save their ID in ebut_id[].
*
***Point.
*/
   butx = 0;
   if ( actbut == 1 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            poitxt,poitxt,"",WP_BGND3,WP_FGND,&ebut_id[0]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            poitxt,poitxt,"",WP_BGND2,WP_FGND,&ebut_id[0]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[0]].ptr;
   strcpy(butptr->tt_str,poitip);
/*
***Line.
*/
   butx += butlen1;
   if ( actbut == 2 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            lintxt,lintxt,"",WP_BGND3,WP_FGND,&ebut_id[1]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            lintxt,lintxt,"",WP_BGND2,WP_FGND,&ebut_id[1]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[1]].ptr;
   strcpy(butptr->tt_str,lintip);
/*
***Arc.
*/
   butx += butlen1;
   if ( actbut == 3 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            arctxt,arctxt,"",WP_BGND3,WP_FGND,&ebut_id[2]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            arctxt,arctxt,"",WP_BGND2,WP_FGND,&ebut_id[2]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[2]].ptr;
   strcpy(butptr->tt_str,arctip);
/*
***Curve.
*/
   butx += butlen1;
   if ( actbut == 4 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            curtxt,curtxt,"",WP_BGND3,WP_FGND,&ebut_id[3]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            curtxt,curtxt,"",WP_BGND2,WP_FGND,&ebut_id[3]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[3]].ptr;
   strcpy(butptr->tt_str,curtip);
/*
***Surface.
*/
   butx += butlen1;
   if ( actbut == 5 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            surtxt,surtxt,"",WP_BGND3,WP_FGND,&ebut_id[4]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            surtxt,surtxt,"",WP_BGND2,WP_FGND,&ebut_id[4]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[4]].ptr;
   strcpy(butptr->tt_str,surtip);
/*
***Text.
*/
   butx += butlen1;
   if ( actbut == 6 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            txttxt,txttxt,"",WP_BGND3,WP_FGND,&ebut_id[5]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            txttxt,txttxt,"",WP_BGND2,WP_FGND,&ebut_id[5]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[5]].ptr;
   strcpy(butptr->tt_str,txttip);
/*
***Hatching.
*/
   butx += butlen1;
   if ( actbut == 7 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            xhttxt,xhttxt,"",WP_BGND3,WP_FGND,&ebut_id[6]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            xhttxt,xhttxt,"",WP_BGND2,WP_FGND,&ebut_id[6]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[6]].ptr;
   strcpy(butptr->tt_str,xhttip);
/*
***Dimensions.
*/
   butx += butlen1;
   if ( actbut == 8 )
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            dimtxt,dimtxt,"",WP_BGND3,WP_FGND,&ebut_id[7]);
   else
     WPcrpb(iwin_id,butx,0,butlen1,bh,1,
            dimtxt,dimtxt,"",WP_BGND2,WP_FGND,&ebut_id[7]);
   butptr = (WPBUTT *)iwinpt->wintab[ebut_id[7]].ptr;
   strcpy(butptr->tt_str,dimtip);
  }

/********************************************************/
