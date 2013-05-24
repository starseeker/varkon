/**********************************************************************
*
*    wpmenu.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPinit_menu();      Init menu window handler
*    WPactivate_menu();  Make menu active and display
*    WPupdate_menu();    Update contents of menu window
*    WPmenu_button();    Button handler for menu window
*    WPfocus_menu();     Focus In for menu
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
#include <math.h>

extern char    jobnam[];     /* Current jobname for menu window border */
extern int     posmode;      /* Currently active positions method in IG */
extern bool    relpos;       /* Relative positions flag On/Off */
extern short   sysmode;       /* EXPLICIT/GENERIC */
extern char    actcnm[];     /* Name of currently active coordinate system */
extern V2NAPA  defnap;       /* Currently active attributes */

static MNUDAT *actmeny;       /* Currently active menu */
static wpw_id  men_id;        /* WP-id of the menu window */
static Window  altid[25];     /* X-id of the alternative buttons */
static Window  posid[9];      /* X-id of the pos buttons */
static Window  levelid;       /* X-id of the level button */
static Window  penid;         /* X-id of the pen button */
static Window  wdtid;         /* X-id of the width button */
static Window  csyid;         /* X-id of the csy button */
static Window  mbsid;         /* X-id of the MBS button */
static Window  runid;         /* X-id of the Run button */
static Window  edpid;         /* X-id of the EditP button */
static Window  delid;         /* X-id of the Delete button */

static char    pbtext_1[81];  /* Text for pos button 1 absolute */
static char    pbtext_2[81];  /* Text for pos button 2 relative */
static char    pbtext_3[81];  /* Text for pos button 3 mouse */
static char    pbtext_4[81];  /* Text for pos button 4 mbs */
static char    pbtext_5[81];  /* Text for pos button 5 end */
static char    pbtext_6[81];  /* Text for pos button 6 on */
static char    pbtext_7[81];  /* Text for pos button 7 centre */
static char    pbtext_8[81];  /* Text for pos button 8 intersect */
static char    pbtext_9[81];  /* Text for pos button 9 grid */

static char    pbttip_1[81];  /* Tooltip for pos button 1 */
static char    pbttip_2[81];  /* Tooltip for pos button 2 */
static char    pbttip_3[81];  /* Tooltip for pos button 3 */
static char    pbttip_4[81];  /* Tooltip for pos button 4 */
static char    pbttip_5[81];  /* Tooltip for pos button 5 */
static char    pbttip_6[81];  /* Tooltip for pos button 6 */
static char    pbttip_7[81];  /* Tooltip for pos button 7 */
static char    pbttip_8[81];  /* Tooltip for pos button 8 */
static char    pbttip_9[81];  /* Tooltip for pos button 9 */

static char    lvltip[81];     /* Tooltip for LVL button */
static char    pentip[81];     /* Tooltip for PEN button */
static char    csytip[81];     /* Tooltip for CSY button */
static char    wdttip[81];     /* Tooltip for WIDTH button */
static char    mbstip[81];     /* Tooltip for MBS button */
static char    runtext[81];    /* Text for Run button */
static char    runtip[81];     /* Tooltip for Run button */
static char    edptext[81];    /* Text for EditP button */
static char    edptip[81];     /* Tooltip for EditP button */
static char    deltext[81];    /* Text for Delete button */
static char    deltip[81];     /* Tooltip for Delete button */
/*
***Prototypes for internal functions.
*/
static short create_menuwindow(int x,int y,int dx,int dy,char *label,DBint *id);
static short csys_dialogue(int x,int y);

/*!******************************************************/

        short WPinit_menu()

/*      Init for menus. Creates the menu window.
 * 
 *      Error: WP1252 = Can't create menu on display %s
 *
 *      (C)microform ab 30/1/95 J. Kjellander
 *
 *      2007-03-08 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   int       x,y,dx,dy;
   unsigned int dum1,dum2; 
   short     status;
   DBint     w_id;
   char     *type[20];
   XrmValue  value;

/*
***Geometry.
*/
   x  = y  = 5;
   dx = dy = 30;

   if ( XrmGetResource(xresDB,"varkon.menu.geometry","Varkon.Menu.Geometry",
        type,&value) ) XParseGeometry((char *)value.addr,&x,&y,&dum1,&dum2);
/*
***Text and tooltip for button 1.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_1.text",
                              "Varkon.Menu.Button_1.Text",type,&value) )
     {
     strncpy(pbtext_1,value.addr,80);
     pbtext_1[80] = '\0';
     }
   else strcpy(pbtext_1,"abs");

   if ( XrmGetResource(xresDB,"varkon.menu.button_1.tooltip",
                              "Varkon.Menu.Button_1.Tooltip",type,&value) )
     {
     strncpy(pbttip_1,value.addr,80);
     pbttip_1[80] = '\0';
     }
   else strcpy(pbttip_1,"Tooltip missing !");
/*
***Text and tooltip for button 2.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_2.text",
                              "Varkon.Menu.Button_2.Text",type,&value) )
     {
     strncpy(pbtext_2,value.addr,80);
     pbtext_2[80] = '\0';
     }
   else strcpy(pbtext_2,"rel");

   if ( XrmGetResource(xresDB,"varkon.menu.button_2.tooltip",
                              "Varkon.Menu.Button_2.Tooltip",type,&value) )
     {
     strncpy(pbttip_2,value.addr,80);
     pbttip_2[80] = '\0';
     }
   else strcpy(pbttip_2,"Tooltip missing !");
/*
***Text and tooltip for button 3.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_3.text",
                              "Varkon.Menu.Button_3.Text",type,&value) )
     {
     strncpy(pbtext_3,value.addr,80);
     pbtext_3[80] = '\0';
     }
   else strcpy(pbtext_3,"curs");

   if ( XrmGetResource(xresDB,"varkon.menu.button_3.tooltip",
                              "Varkon.Menu.Button_3.Tooltip",type,&value) )
     {
     strncpy(pbttip_3,value.addr,80);
     pbttip_3[80] = '\0';
     }
   else strcpy(pbttip_3,"Tooltip missing !");
/*
***Text and tooltip for button 4.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_4.text",
                              "Varkon.Menu.Button_4.Text",type,&value) )
     {
     strncpy(pbtext_4,value.addr,80);
     pbtext_4[80] = '\0';
     }
   else strcpy(pbtext_4,"mbs");

   if ( XrmGetResource(xresDB,"varkon.menu.button_4.tooltip",
                              "Varkon.Menu.Button_4.Tooltip",type,&value) )
     {
     strncpy(pbttip_4,value.addr,80);
     pbttip_4[80] = '\0';
     }
   else strcpy(pbttip_4,"Tooltip missing !");
/*
***Text and tooltip for button 5.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_5.text",
                              "Varkon.Menu.Button_5.Text",type,&value) )
     {
     strncpy(pbtext_5,value.addr,80);
     pbtext_5[80] = '\0';
     }
   else strcpy(pbtext_5,"end");

   if ( XrmGetResource(xresDB,"varkon.menu.button_5.tooltip",
                              "Varkon.Menu.Button_5.Tooltip",type,&value) )
     {
     strncpy(pbttip_5,value.addr,80);
     pbttip_5[80] = '\0';
     }
   else strcpy(pbttip_5,"Tooltip missing !");
/*
***Text and tooltip for button 6.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_6.text",
                              "Varkon.Menu.Button_6.Text",type,&value) )
     {
     strncpy(pbtext_6,value.addr,80);
     pbtext_6[80] = '\0';
     }
   else strcpy(pbtext_6,"on");

   if ( XrmGetResource(xresDB,"varkon.menu.button_6.tooltip",
                              "Varkon.Menu.Button_6.Tooltip",type,&value) )
     {
     strncpy(pbttip_6,value.addr,80);
     pbttip_6[80] = '\0';
     }
   else strcpy(pbttip_6,"Tooltip missing !");
/*
***Text and tooltip for button 7.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_7.text",
                              "Varkon.Menu.Button_7.Text",type,&value) )
     {
     strncpy(pbtext_7,value.addr,80);
     pbtext_7[80] = '\0';
     }
   else strcpy(pbtext_7,"cent");

   if ( XrmGetResource(xresDB,"varkon.menu.button_7.tooltip",
                              "Varkon.Menu.Button_7.Tooltip",type,&value) )
     {
     strncpy(pbttip_7,value.addr,80);
     pbttip_7[80] = '\0';
     }
   else strcpy(pbttip_7,"Tooltip missing !");
/*
***Text and tooltip for button 8.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_8.text",
                              "Varkon.Menu.Button_8.Text",type,&value) )
     {
     strncpy(pbtext_8,value.addr,80);
     pbtext_8[80] = '\0';
     }
   else strcpy(pbtext_8,"int");

   if ( XrmGetResource(xresDB,"varkon.menu.button_8.tooltip",
                              "Varkon.Menu.Button_8.Tooltip",type,&value) )
     {
     strncpy(pbttip_8,value.addr,80);
     pbttip_8[80] = '\0';
     }
   else strcpy(pbttip_8,"Tooltip missing !");
/*
***Text and tooltip for button 9.
*/
   if ( XrmGetResource(xresDB,"varkon.menu.button_9.text",
                              "Varkon.Menu.Button_9.Text",type,&value) )
     {
     strncpy(pbtext_9,value.addr,80);
     pbtext_9[80] = '\0';
     }
   else strcpy(pbtext_9,"grid");

   if ( XrmGetResource(xresDB,"varkon.menu.button_9.tooltip",
                              "Varkon.Menu.Button_9.Tooltip",type,&value) )
     {
     strncpy(pbttip_9,value.addr,80);
     pbttip_9[80] = '\0';
     }
   else strcpy(pbttip_9,"Tooltip missing !");
/*
***Tooltips for Level, Pen, Width, Csy and Mbs buttons.
*/
   if ( !WPgrst("varkon.menu.level.tooltip",lvltip) )  strcpy(lvltip,"");
   if ( !WPgrst("varkon.menu.pen.tooltip",pentip) )    strcpy(pentip,"");
   if ( !WPgrst("varkon.menu.csy.tooltip",csytip) )    strcpy(csytip,"");
   if ( !WPgrst("varkon.menu.width.tooltip",wdttip) )  strcpy(wdttip,"");
   if ( !WPgrst("varkon.menu.mbs.tooltip",mbstip) )    strcpy(mbstip,"");
/*
***Text and tooltips for Run, EditP and Delete buttons.
*/
   if ( !WPgrst("varkon.menu.run.text",runtext) )    strcpy(runtext,"?");
   if ( !WPgrst("varkon.menu.run.tooltip",runtip) )    strcpy(runtip,"");
   if ( !WPgrst("varkon.menu.editp.text",edptext) )  strcpy(edptext,"?");
   if ( !WPgrst("varkon.menu.editp.tooltip",edptip) )  strcpy(edptip,"");
   if ( !WPgrst("varkon.menu.delete.text",deltext) ) strcpy(deltext,"?");
   if ( !WPgrst("varkon.menu.delete.tooltip",deltip) ) strcpy(deltip,"");
/*
***Create the menu window. Window border title is added later each
***time the window is updated so we just supply "" here.
*/
   status = create_menuwindow(x,y,dx,dy,"",&w_id);
   if ( status < 0 ) return(erpush("WP1252",XDisplayName(NULL)));
/*
***No meny active at this state.
*/
   actmeny = NULL;
/*
***Init (turn off) the relative positions button.
*/
   relpos = FALSE;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!*******************************************************/

     short   WPactivate_menu(
     MNUDAT *meny)

/*   Make a menu active and display it.
 *
 *   In: *meny = C ptr to menu
 *
 *   (C)microform ab 24/1/94 J. Kjellander
 *
 *******************************************************!*/

 {

/*
***Update actmeny.
*/
    actmeny = meny;
/*
***Update contents of menu window.
*/
    WPupdate_menu();
/*
***The end.
*/
    return(0);
 }

/*********************************************************/
/*!*******************************************************/

     short WPupdate_menu()

/*   Updates the contents of the menu window with
 *   new alternatives and buttons.
 *
 *   (C)microform ab 8/7/92 U. Andersson.
 *
 *    24/1/94    Omarbetad, J. Kjellander
 *    2007-03-17 More buttons, J.Kjellander
 *    2007-09-09 relpos, J.Kjellander
 *
 *******************************************************!*/

 {
   int         ly,mendx,mendy,butx,butdx,buty,butdy;
   char        title[V3STRLEN],butstr[V3STRLEN];
   int         bw,lm,tl,mrl,i,tlmax,minalt,text_color; 
   DBint       rubr_id,alt_id,pos_id,lvl_id,pen_id,
               wdt_id,csy_id,mbs_id,run_id,edp_id,del_id;
   MNUALT     *altptr;
   WPIWIN     *mwinpt;
   WPBUTT     *butptr;
   XSizeHints *size_hints;

/*
***We can't update if the menu window is not yet created.
*/
   if ( actmeny == NULL ) return(0);
/*
***Window title.
*/
   if ( !WPgrst("varkon.menu.title",title) ) strcpy(title,jobnam);
/*
***Some initializations.
*/
   altptr = actmeny->alt;
   mrl    = 1.2*WPstrl(actmeny->rubr);
   bw     = 1;
   ly     = 0.6*WPstrh();
   lm     = 0.4*WPstrh();
   butdy  = 1.5*WPstrh();
/*
***Menu height. Minimum height = 7 menu alternatives
***plus the stationary buttons.
*/
   if ( (minalt=actmeny->nalt) < 8 ) minalt = 7;

   mendy = ly + butdy + minalt*(lm + butdy) + 2*lm + 2*lm +
           butdy + lm + butdy + lm + butdy + lm + butdy + 
           lm + butdy + 2*lm + 2*lm + 3*butdy + 2*bw + ly;
/*
***What is the minimum width of the menu window ?
***Three columns of 4-character pos-buttons must fit
***as well as all the alternative texts and the title.
*/
   tlmax = 3*1.5*WPstrl("ABCD") + 4*bw;

   for (i=0; i<actmeny->nalt; i++)
     {
     tl = 1.2*WPstrl(altptr->str);
   ++altptr;
     if ( tl > tlmax) tlmax = tl;
     }

   if ( mrl > tlmax) tlmax = mrl;
/*
***Window width, mendx.
*/
   butdx = tlmax;
   mendx = ly + butdx + ly;
/*
***Kill current subwindows (menu title + alternatives + pos buttons).
*/
   mwinpt = (WPIWIN *)wpwtab[men_id].ptr;

   for ( i=0; i<WP_IWSMAX; ++i )
     {
     butptr = (WPBUTT *)mwinpt->wintab[i].ptr;
     if ( butptr != NULL )
       {
       WPdlbu(butptr);
       mwinpt->wintab[i].ptr = NULL;
       mwinpt->wintab[i].typ = TYP_UNDEF;
       }
     else break;
     }

   XDestroySubwindows(xdisp,mwinpt->id.x_id);
/*
***Resize menu window.
*/
   mwinpt->geo.dx = (short)mendx;
   mwinpt->geo.dy = (short)mendy;
   XResizeWindow(xdisp,mwinpt->id.x_id,mendx,mendy);

   size_hints = XAllocSizeHints();
   size_hints->flags      = USSize | PMinSize | PMaxSize; 
   size_hints->min_width  = size_hints->max_width  = size_hints->width  = mendx;
   size_hints->min_height = size_hints->max_height = size_hints->height = mendy; 
   XSetWMNormalHints(xdisp,mwinpt->id.x_id,size_hints);
   XFree(size_hints);
/*
***Create new menu title button.
*/
   butx = ly;
   buty = ly;
   WPcrlb(men_id,butx,buty,butdx,butdy,actmeny->rubr,&rubr_id);
/*
***Create new alternative buttons.
*/
   altptr = actmeny->alt;
   butx   = ly - bw;

   for ( i=0; i<actmeny->nalt; i++ )
     {
     buty += lm + butdy;
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,
                   altptr->str,altptr->str,"",WP_BGND2,WP_FGND,&alt_id);
     butptr = (WPBUTT *)mwinpt->wintab[alt_id].ptr;
     altid[i] = butptr->id.x_id;
     strcpy(butptr->tt_str,"Click to activate this menu alternative");
   ++altptr;
     }
/*
***A line.
*/
   buty = ly + minalt*(lm + butdy) + butdy + 2*lm;
   WPcreate_3Dline(men_id,butx+butdx/4,buty,butx+3*butdx/4,buty);
/*
***Pen and Level.
*/
   buty += 2*lm;
   sprintf(butstr,"LVL %d",defnap.level);
   WPcrpb(men_id,butx,buty,butdx/2 - bw,butdy,bw,
                           butstr,butstr,"",WP_BGND2,WP_FGND,&lvl_id);
   butptr = (WPBUTT *)mwinpt->wintab[lvl_id].ptr;
   strcpy(butptr->tt_str,lvltip);
   levelid = butptr->id.x_id;

   sprintf(butstr,"PEN %d",defnap.pen);
   WPcrpb(men_id,butx + butdx/2 + bw,buty,butdx/2 - bw,butdy,bw,
                           butstr,butstr,"",WP_BGND2,WP_FGND,&pen_id);
   butptr = (WPBUTT *)mwinpt->wintab[pen_id].ptr;
   strcpy(butptr->tt_str,pentip);
   penid = butptr->id.x_id;
/*
***The width-button.
*/
   buty += (butdy + lm);
   sprintf(butstr,"WIDTH %g",defnap.width);
   if ( DEC(defnap.width) == 0.0 ) strcat(butstr,".0");
   WPcrpb(men_id,butx,buty,butdx,butdy,bw,
                           butstr,butstr,"",WP_BGND2,WP_FGND,&wdt_id);
   butptr = (WPBUTT *)mwinpt->wintab[wdt_id].ptr;
   strcpy(butptr->tt_str,wdttip);
   wdtid = butptr->id.x_id;
/*
***The csys-button.
*/
   buty += (butdy + lm);
   sprintf(butstr,"CSY %s",actcnm);
   WPcrpb(men_id,butx,buty,butdx,butdy,bw,
                           butstr,butstr,"",WP_BGND2,WP_FGND,&csy_id);
   butptr = (WPBUTT *)mwinpt->wintab[csy_id].ptr;
   strcpy(butptr->tt_str,csytip);
   csyid = butptr->id.x_id;
/*
***MBS and Run. Gray texts in explicit mode.
*/
   if ( sysmode == EXPLICIT ) text_color = WP_BGND3;
   else                     text_color = WP_FGND;

   buty += (butdy + lm);
   WPcrpb(men_id,butx,buty,butdx/2 - bw,butdy,bw,
                           "MBS","MBS","",WP_BGND2,text_color,&mbs_id);
   butptr = (WPBUTT *)mwinpt->wintab[mbs_id].ptr;
   strcpy(butptr->tt_str,mbstip);
   mbsid = butptr->id.x_id;

   WPcrpb(men_id,butx + butdx/2 + bw,buty,butdx/2 - bw,butdy,bw,
                           runtext,runtext,"",WP_BGND2,text_color,&run_id);
   butptr = (WPBUTT *)mwinpt->wintab[run_id].ptr;
   strcpy(butptr->tt_str,runtip);
   runid = butptr->id.x_id;
/*
***EditP and Delete.
*/
   buty += (butdy + lm);
   WPcrpb(men_id,butx,buty,butdx/2 - bw,butdy,bw,
                           edptext,edptext,"",WP_BGND2,WP_FGND,&edp_id);
   butptr = (WPBUTT *)mwinpt->wintab[edp_id].ptr;
   strcpy(butptr->tt_str,edptip);
   edpid = butptr->id.x_id;

   WPcrpb(men_id,butx + butdx/2 + bw,buty,butdx/2 - bw,butdy,bw,
                           deltext,deltext,"",WP_BGND2,WP_FGND,&del_id);
   butptr = (WPBUTT *)mwinpt->wintab[del_id].ptr;
   strcpy(butptr->tt_str,deltip);
   delid = butptr->id.x_id;
/*
***A second line.
*/
   buty += butdy + 2*lm;
   WPcreate_3Dline(men_id,butx+butdx/4,buty,butx+3*butdx/4,buty);
/*
***Position alternatives, line 1.
*/
   buty += 2*lm;
   butdx = 1.5*WPstrl("ABCD");
   if ( posmode == 0 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_1,pbtext_1,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_1,pbtext_1,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_1);
   posid[0] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( relpos )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_2,pbtext_2,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_2,pbtext_2,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_2);
   posid[1] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( posmode == 2 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_3,pbtext_3,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_3,pbtext_3,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_3);
   posid[2] = butptr->id.x_id;
/*
***Position alternatives, line 2.
*/
   butx  = ly - bw;
   buty += butdy + 2*bw;
   if ( posmode == 3 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_4,pbtext_4,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_4,pbtext_4,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_4);
   posid[3] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( posmode == 4 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_5,pbtext_5,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_5,pbtext_5,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_5);
   posid[4] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( posmode == 5 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_6,pbtext_6,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_6,pbtext_6,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_6);
   posid[5] = butptr->id.x_id;
/*
***Position alternatives, line 3.
*/
   butx  = ly - bw;
   buty += butdy + 2*bw;
   if ( posmode == 6 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_7,pbtext_7,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_7,pbtext_7,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_7);
   posid[6] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( posmode == 7 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_8,pbtext_8,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_8,pbtext_8,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_8);
   posid[7] = butptr->id.x_id;

   butx += butdx + 2*bw;
   if ( posmode == 8 )
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_9,pbtext_9,"",WP_BGND3,WP_FGND,&pos_id);
   else
     WPcrpb(men_id,butx,buty,butdx,butdy,bw,pbtext_9,pbtext_9,"",WP_BGND2,WP_FGND,&pos_id);
   butptr = (WPBUTT *)mwinpt->wintab[pos_id].ptr;
   strcpy(butptr->tt_str,pbttip_9);
   posid[8] = butptr->id.x_id;
/*
***Update window border title.
*/
   XStoreName(xdisp,mwinpt->id.x_id,title);
/*
***Map all sub windows now. This is not done earlier.
*/
   XMapSubwindows(xdisp,mwinpt->id.x_id);
/*
***The end.
*/
   return(0);
 }

/*********************************************************/
/*!******************************************************/

        bool      WPmenu_button(
        XEvent   *event,
        MNUALT  **altptr)

/*      Button handler for menu window.
 *
 *      In:   event  = Ptr to event.
 *
 *      Out: *altptr = Ptr to menu alternative or NULL
 *
 *      Return: TRUE  = Event handled
 *              FALSE = Event not handled
 *
 *      (C)microform ab 17/7/92 J. Kjellander
 *
 *      2007-03-25 1.19, J.Kjellander
 *      2007-09-09 relpos, J.Kjellander
 *
 ******************************************************!*/

  {
   int  i   = 0;
   bool hit = FALSE;

/*
***Is this event a button event ?
*/
   if ( event->type != ButtonRelease ) return(FALSE);
/*
***If no menu exists, return FALSE.
*/
   if ( actmeny == NULL ) return(FALSE);
/*
***altptr == NULL means no alternatives can be selected.
*/
   else if ( altptr == NULL ) return(FALSE);
/*
***Search through the menu alternative buttons for a hit.
*/
   else
     {
     *altptr = actmeny->alt;

      while ( hit == FALSE  &&  i < actmeny->nalt )
        {
        if ( event->xany.window == altid[i++] ) hit = TRUE;
        else *altptr = *altptr + 1;
        }
      }
/*
***If no hit, maybe one of the other buttons ?
*/
   if ( !hit )
     {
     if ( event->xany.window == levelid )
       {
       IGslvl();
       WPupdate_menu();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == penid )
       {
       IGspen();
       WPupdate_menu();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == wdtid )
       {
       IGswdt();
       WPupdate_menu();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == mbsid )
       {
       if ( sysmode == EXPLICIT ) WPbell();
       else                     WPamod();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == runid )
       {
       if ( sysmode == EXPLICIT ) WPbell();
       else                     IGrun_active();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == edpid )
       {
       IGcptw();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == delid )
       {
       IGdelete_entity();
      *altptr = NULL;
       hit = TRUE;
       }
     else if ( event->xany.window == csyid )
       {
       csys_dialogue(event->xbutton.x_root,event->xbutton.y_root);
       WPupdate_menu();
      *altptr = NULL;
       hit = TRUE;
       }
     }
/*
***If still no hit, search through the pos windows. A hit here
***changes the value of posmode but returns altptr = NULL
***so that the caller will be noticed that the event was
***serviced but no alternative selected.
*/
   if ( !hit )
     {
     for ( i=0; i<9; ++i )
       {
       if ( event->xany.window == posid[i] )
         {
         if ( i == 1 )
           {
           if ( relpos ) relpos = FALSE;
           else          relpos = TRUE;
           }
         else posmode = i;

         WPupdate_menu();
        *altptr = NULL;
         hit = TRUE;
         break;
         }
       }
     }
/*
***The end. If an alternative was selected, return true,
***otherwise return false.
*/
    return(hit);
  }

/********************************************************/
/*!******************************************************/

        void WPfocus_menu()

/*      Raises the menu window.
 *
 *      (C)microform ab 1996-02-12 J. Kjellander
 *
 ******************************************************!*/

  {
   WPIWIN *mwinpt;

/*
***Get a C ptr to the menu window.
*/
   mwinpt = (WPIWIN *)wpwtab[men_id].ptr;
/*
***Raise.
*/
   XRaiseWindow(xdisp,mwinpt->id.x_id);
  }

/********************************************************/
/*!******************************************************/

 static short  create_menuwindow(
        int    x,
        int    y,
        int    dx,
        int    dy,
        char  *label,
        DBint *id)

/*      Creates the WPIWIN-window for the menu.
 *
 *      In:  x,y,dx,dy = Geometry
 *           label     = Window border title
 *
 *      Out: id        = WP id
 *
 *      (C)microform ab 24/1/94 J. Kjellander
 *
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    WPIWIN *iwinpt;

/*
***Create a WPIWIN-window. Save ID in static-variable 
***men_id.
*/
    status = WPwciw(x,y,dx,dy,label,id);
    if ( status < 0 ) return(status);
    men_id = *id;
/*
***A C-ptr to the WPIWIN.
*/
    iwinpt = (WPIWIN *)wpwtab[(wpw_id)*id].ptr;
/*
***Modify the SelectInput mask so that also button
***release is allowed in the menu window.
*/
    XSelectInput(xdisp,iwinpt->id.x_id,ExposureMask     |
                                       KeyPressMask     |
                                       ButtonPressMask  |
                                       ButtonReleaseMask);
/*
***Map.
*/
    WPwshw(*id);
/*
***To prevent each sub window to be mapped individually each
***time a new menu is displayed we set the map falg to FALSE.
***Mapping is then done by WPupdate_menu();
*/
    iwinpt->mapped = FALSE;

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short csys_dialogue(
        int main_x,
        int main_y)

/*      Handles the CSY-button in the menu window.
 *
 *      In: main_x, main_y = Requested window position.
 *
 *      Return:  0 = OK.
 *          REJECT = Operation canceled.
 *
 *      (C)2007-03-18 J. Kjellander
 *
 ******************************************************!*/

  {
   char     rubrik[81],reject[81],rejecttt[81],help[81],helptt[81],
            local[81],global[81],basic[81],localtt[81],globaltt[81],
            basictt[81];
   short    status;
   int      altlen,alth,main_dx,main_dy,ly,lm,alt_x,alt_y,csy_mode;
   DBint    iwin_id,local_id,global_id,basic_id,reject_id,help_id,but_id;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;

/*
***Local, Global and Basic + Cancel and Help.
*/
   strcpy(local,"LOCAL");
   strcpy(global,"GLOBAL");
   strcpy(basic,"BASIC");

   if ( !WPgrst("varkon.menu.csy.title",rubrik) )         strcpy(rubrik,"Csys");
   if ( !WPgrst("varkon.menu.local.tooltip",localtt) )    strcpy(localtt,"");
   if ( !WPgrst("varkon.menu.global.tooltip",globaltt) )  strcpy(globaltt,"");
   if ( !WPgrst("varkon.menu.basic.tooltip",basictt) )    strcpy(basictt,"");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");
/*
***Which text is longest ?
*/
   altlen = 0;

   if ( WPstrl(local)  > altlen ) altlen = WPstrl(local);
   if ( WPstrl(global) > altlen ) altlen = WPstrl(global);
   if ( WPstrl(basic)  > altlen ) altlen = WPstrl(basic);
   if ( WPstrl(reject) > altlen ) altlen = WPstrl(reject);
   if ( WPstrl(help)   > altlen ) altlen = WPstrl(help);

   altlen *= 1.4;
/*
***Window geometry.
*/
   ly   = 0.8*WPstrh();
   lm   = 1.4*WPstrh();
   alth = 1.7*WPstrh();

   main_dx = ly + altlen + lm + altlen + lm + altlen + ly;
   main_dy = ly + alth + ly + ly + alth + ly;  
/*
***Create the window as a WPIWIN.
*/
   WPwciw(main_x,main_y,main_dx,main_dy,rubrik,&iwin_id);
   iwinpt = (WPIWIN *)wpwtab[iwin_id].ptr;
/*
***Which csy mode is active now ?
*/
   if       ( strcmp(actcnm,"BASIC") == 0 ) csy_mode = 1;
   else if ( strcmp(actcnm,"GLOBAL") == 0 ) csy_mode = 2;
   else                                     csy_mode = 3;
/*
***LOCAL button.
*/
   alt_x  = ly;
   alt_y  = ly;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                          local,local,"",WP_BGND2,WP_FGND,&local_id);
   butptr = (WPBUTT *)iwinpt->wintab[local_id].ptr;
   strcpy(butptr->tt_str,localtt);
/*
***GLOBAL button.
*/
   alt_x  = ly + altlen + lm;
   if ( csy_mode == 2 )
     {
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                            global,global,"",WP_BGND3,WP_FGND,&global_id);
     }
   else
     {
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                            global,global,"",WP_BGND2,WP_FGND,&global_id);
     }
   butptr = (WPBUTT *)iwinpt->wintab[global_id].ptr;
   strcpy(butptr->tt_str,globaltt);
/*
***BASIC button.
*/
   alt_x  = ly + altlen + lm + altlen + lm;
   if ( csy_mode == 1 )
     {
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                            basic,basic,"",WP_BGND3,WP_FGND,&basic_id);
     }
   else
     {
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                            basic,basic,"",WP_BGND2,WP_FGND,&basic_id);
     }
   butptr = (WPBUTT *)iwinpt->wintab[basic_id].ptr;
   strcpy(butptr->tt_str,basictt);
/*
***Reject and help.
*/
   alt_x  = ly;
   alt_y += (alth + ly + ly);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                          reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - (altlen + ly);
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,altlen,alth,1,
                          help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Show the dialogue.
*/
   WPwshw(iwin_id);
   XRaiseWindow(xdisp,iwinpt->id.x_id);
/*
***Wait for action.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Local.
*/
   if ( but_id == local_id )
     {
     WPwdel(iwin_id);
     status = IGmodl();
     return(status);
     }
/*
***Global. If already active, beep.
*/
   else if ( but_id == global_id )
     {
     if ( csy_mode != 2 )
       {
       IGmodg();
       WPwdel(iwin_id);
       WPupdate_menu();
       return(0);
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***Basic. If already active, beep.
*/
   else if ( but_id == basic_id )
     {
     if ( csy_mode != 1 )
       {
       IGmodb();
       WPwdel(iwin_id);
       WPupdate_menu();
       return(0);
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     WPwdel(iwin_id);
     return(REJECT);
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
***Unknown event.
*/
    else goto loop;
  }

/********************************************************/
