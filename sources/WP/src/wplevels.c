/**********************************************************************
*
*    wplevels.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPlevel_dialog();      The level dialog
*    WPmtsl();              Blank/unblank range of levels
*    WPnivt();              Check if level is visible
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
#include <math.h>
#include <string.h>

extern int    actfunc;           /* For the help system */

#define LISTMAX 100              /* The maximum number of levels to list */

static bool premiere = TRUE;     /* True only for the first invocation */
static int  n_selected;          /* Number of levels in current selectset */
static int  sel_set[WP_NIVANT];  /* The list of currently selected levels */
static int  level_id[LISTMAX];   /* ID's for level number buttons */
static int  name_id[LISTMAX];    /* ID's for level name edits */
static int  blank_id[LISTMAX];   /* ID's for level status buttons */
static int  list_size;           /* The max number of levels to display */
static int  start,stop;          /* The portion of sel_set[] displayed */
static int  plus_id,minus_id;    /* ID of scroll butons */

static WPIWIN *iwinpt;           /* C ptr to the levels dialogue */
static DBint   iwin_id;          /* Levels dialogue ID */
static int     iwin_x;           /* The current position of the window */
static int     iwin_y;           /* even after a user move */
/*
***Prototypes for internal functions.
*/
static void mk_selset_range(int n1, int n2);
static void mk_selset_name(char *name);
static void mk_selset_used();
static void delete_list();
static void update_names();

/*!******************************************************/

        short WPlevel_dialog(
        DBint grw_id)

/*      The levels dialog.
 *
 *      In:  grw_id = ID of WPGWIN/WPRWIN.
 *
 *      (C)2007-04-01 J.Kjellander
 *
 ******************************************************!*/

  {
   short    status;
   int      i,ly,lm,level,numlen,namelen,statlen,uph,lowh,
            scrllen,wm_x1,wm_y1,main_dx,main_dy,nplus,nminus,
            wm_x2,wm_y2,lowlen1,lowlen2,n1,n2,space,alt_x,alt_y,
            actfunc_org;
   unsigned int dum1,dum2; 
   bool     level_status;
   char     numstr[81],namstr[81],level_name[V3STRLEN+1],
            title[81],blank[81],unblank[81],close[81],closett[81],
            help[81],helptt[81],scrlstr[81],name[81],
            namett[81],range[81],rangett[81],rangestr[81],used[81],
            usedtt[81],plustt[81],minustt[81],apply[81];
   char    *type[20];
   DBint    help_id,close_id,nam_id,nb_id,
            range_id,rb_id,but_id,used_id;
   WPWIN   *winptr;
   XEvent   event;
   WPBUTT  *butptr;
   WPEDIT  *edtptr;
   XrmValue value;

/*
***Set actfunc, see IG/include/futab.h.
*/
   actfunc_org = actfunc;
   actfunc = 197;
/*
***Calculate outside air (ly), up height (uph), low height (lowh)
***and air between (lm).
*/
   ly   = 0.8*WPstrh();
   uph  = 1.6*WPstrh(); 
   lowh = 2.0*WPstrh();
   lm   = 3;
/*
***Is this the first time this function is called ?
*/
   if ( premiere )
     {
/*
***Initial window position.
*/
     iwin_x = iwin_y = 5;

     if ( XrmGetResource(xresDB,"varkon.level.geometry",
                                "Varkon.Level.Geometry",
                                 type,&value) ) 
       XParseGeometry((char *)value.addr,&iwin_x,&iwin_y,&dum1,&dum2);
/*
***Make a default select set and list range. Start and stop
***control what part of the select set that is visible in the list.
***How many levels can we have in the list ? Text size versus
***screeen size. Max is LISTMAX.
*/
     space = DisplayHeight(xdisp,xscr) - 
             (uph + uph + lowh + lowh + lowh + ly + lowh +
              lowh + lowh + lowh + ly + lowh + ly) - 100;

     list_size = space/(uph + lm) - 1;
     list_size = 5*(list_size/5);

     mk_selset_range(0,list_size-1);
     start = 0;
     if ( n_selected > list_size ) stop = start + list_size;
     else                          stop = n_selected;
/*
***Next time this function is called is not the premiere.
*/
     premiere = FALSE;
     }
/*
***Texts from the ini-file.
*/
   if ( !WPgrst("varkon.level.title",title) )      strcpy(title,"Levels");
   if ( !WPgrst("varkon.level.blank",blank) )      strcpy(blank,"Blanked");
   if ( !WPgrst("varkon.level.unblank",unblank) )  strcpy(unblank,"Unblanked");
   if ( !WPgrst("varkon.input.apply",apply) )      strcpy(apply,"");
   if ( !WPgrst("varkon.level.name",name) )        strcpy(name,"Name");
   if ( !WPgrst("varkon.level.range",range) )      strcpy(range,"Range");
   if ( !WPgrst("varkon.level.used",used) )        strcpy(used,"Used");
   if ( !WPgrst("varkon.input.close",close) )      strcpy(close,"Close");
   if ( !WPgrst("varkon.input.help",help) )        strcpy(help,"Help");

   if ( !WPgrst("varkon.level.name.tooltip",namett) )   strcpy(namett,"");
   if ( !WPgrst("varkon.level.range.tooltip",rangett) ) strcpy(rangett,"");
   if ( !WPgrst("varkon.level.used.tooltip",usedtt) )   strcpy(usedtt,"");
   if ( !WPgrst("varkon.level.plus.tooltip",plustt) )   strcpy(plustt,"");
   if ( !WPgrst("varkon.level.minus.tooltip",minustt) ) strcpy(minustt,"");

   if ( !WPgrst("varkon.input.close.tooltip",closett) ) strcpy(closett,"");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )   strcpy(helptt,"");
/*
***The width of this dialogue. First 5 pos for level number
***then 20 for the name and then the status button.
*/
   numlen  = WPstrl("12345");
   namelen = 20*WPstrl("A");
   statlen = WPstrl(blank);
   if ( statlen < WPstrl(unblank) ) statlen = WPstrl(unblank);
   if ( statlen < WPstrl("AA") ) statlen = WPstrl("AA");

   numlen  *= 1.2;
   namelen *= 1.2;
   statlen *= 1.2;
/*
***What is the 1.2*length of the longest text in the lower half
***section of the dialogue (lowlen1 and lowlen2) ?
*/
   lowlen1 = 0;
   if ( WPstrl(apply)  > lowlen1 ) lowlen1 = WPstrl(apply);
   lowlen1 *= 1.2;

   lowlen2 = 0;
   if ( WPstrl(close)  > lowlen2 ) lowlen2 = WPstrl(close);
   if ( WPstrl(help)   > lowlen2 ) lowlen2 = WPstrl(help);
   lowlen2 *= 1.8;
/*
***The size of the scroll buttons.
*/
   scrllen = 1.5*WPstrl("+ 20");
/*
***Window size in X-direction.
*/
   main_dx  = numlen + lm + namelen + lm + statlen + ly;
/*
***Calculate window size in Y-direction.
*/
   main_dy = ly + list_size*uph +    /* Levels listing */
                  list_size*lm  +    /* Air between edits */
             uph  + uph +            /* Scroll buttons */
             lowh +                  /* Name label */
             lowh +                  /* Name edit */
             lowh + ly +             /* Range label */
             lowh +                  /* Range edit */
             lowh + lowh +           /* All used button */
             lowh + ly +             /* Line */
             lowh + ly;              /* Close + Help */
/*
***Create the dialogue window as a WPIWIN.
*/
   WPwciw((short)iwin_x,(short)iwin_y,main_dx,main_dy,title,&iwin_id);
/*
***Get a C-ptr to the WPIWIN.
*/
   winptr = WPwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Now, create the static contents of the window.
***First the name edit with label and apply-button.
*/
   alt_x = numlen + lm;
   alt_y = ly + list_size*(uph+lm) + uph + uph;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(name),lowh,name,&but_id);

   alt_y += lowh;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,15*WPstrl("A"),lowh,(short)1,
                   "*",V3STRLEN,&nam_id);
   edtptr = (WPEDIT *)iwinpt->wintab[nam_id].ptr;
   strcpy(edtptr->tt_str,namett);

   alt_x += 15*WPstrl("A") + lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)1,
                               apply,apply,"",WP_BGND2,WP_FGND,&nb_id);
   butptr = (WPBUTT *)iwinpt->wintab[nb_id].ptr;
   strcpy(butptr->tt_str,namett);
/*
***The range edit with label an apply-button.
*/
   alt_x  = numlen + lm;
   alt_y += lowh + ly;
   WPcrlb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(range),lowh,range,&but_id);

   alt_y += lowh;
   WPmced((wpw_id)iwin_id,alt_x,alt_y,15*WPstrl("A"),lowh,(short)1,
                   "",12,&range_id);
   edtptr = (WPEDIT *)iwinpt->wintab[range_id].ptr;
   strcpy(edtptr->tt_str,rangett);

   alt_x += 15*WPstrl("A") + lm;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen1,lowh,(short)1,
                               apply,apply,"",WP_BGND2,WP_FGND,&rb_id);
   butptr = (WPBUTT *)iwinpt->wintab[rb_id].ptr;
   strcpy(butptr->tt_str,rangett);
/*
***All used.
*/
   alt_x  = numlen + lm;
   alt_y += lowh + lowh;
   WPcrpb((wpw_id)iwin_id,alt_x,alt_y,1.2*WPstrl(used),lowh,(short)1,
                           used,used,"",WP_BGND2,WP_FGND,&used_id);
   butptr = (WPBUTT *)iwinpt->wintab[used_id].ptr;
   strcpy(butptr->tt_str,usedtt);
/*
***A line.
*/
   alt_x  = main_dx/8;
   alt_y += lowh + lowh;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,alt_x + 6*main_dx/8,alt_y);
/*
***Close and help.
*/
   alt_x  = main_dx/10;
   alt_y += ly;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,lowlen2,lowh,(short)3,
                           close,close,"",WP_BGND2,WP_FGND,&close_id);
   butptr = (WPBUTT *)iwinpt->wintab[close_id].ptr;
   strcpy(butptr->tt_str,closett);

   alt_x  = 9*main_dx/10 - lowlen2;
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
***Create the list of selected levels.
*/
start:
   for ( i=0; i<stop-start; ++i )
     {
     level = sel_set[start+i];
/*
***Level number.
*/
     alt_x  = 0;
     alt_y  = ly + i*(uph+lm);
     sprintf(numstr,"%d",level);

     WPcrlb((wpw_id)iwin_id,alt_x,alt_y,numlen,uph,numstr,&level_id[i]);
/*
***Name edit.
*/
   alt_x += numlen + lm;
   EXget_level(level,grw_id,&level_status,level_name);

   status = WPmced((wpw_id)iwin_id,alt_x,alt_y,namelen,uph,(short)1,
                   level_name,V3STRLEN,&name_id[i]);
/*
***Status button.
*/
     alt_x  += namelen + lm;
     if ( level_status )
       status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,statlen,uph,(short)1,
                               blank,unblank,"",WP_BGND2,WP_FGND,&blank_id[i]);
     else
       status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,statlen,uph,(short)1,
                               unblank,blank,"",WP_BGND2,WP_FGND,&blank_id[i]);
     }
/*
***Scroll buttons maybe ?
*/
   plus_id = 0;
   minus_id = 0;

   if ( stop < n_selected )
     {
     if ( (nplus=n_selected - stop) > list_size ) nplus = list_size;
     alt_x = main_dx - (scrllen + lm + statlen + ly);
     alt_y  = ly + i*(uph+lm);
     sprintf(scrlstr,"+ %d",nplus);


     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,scrllen,uph,(short)2,
                           scrlstr,scrlstr,"",WP_BGND2,WP_FGND,&plus_id);
     butptr = (WPBUTT *)iwinpt->wintab[plus_id].ptr;
     strcpy(butptr->tt_str,plustt);
     }

   if ( start > 0 )
     {
     if ( (nminus=start) > list_size ) nminus = list_size;
     alt_x  = numlen + lm;
     alt_y  = ly + i*(uph+lm);
     sprintf(scrlstr,"- %d",nminus);
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,scrllen,uph,(short)2,
                           scrlstr,scrlstr,"",WP_BGND2,WP_FGND,&minus_id);
     butptr = (WPBUTT *)iwinpt->wintab[minus_id].ptr;
     strcpy(butptr->tt_str,minustt);
     }
/*
***Make sure that focus is on the range edit.
*/
   edtptr = (WPEDIT *)iwinpt->wintab[nam_id].ptr;
   WPfoed(edtptr,FALSE);
   edtptr = (WPEDIT *)iwinpt->wintab[range_id].ptr;
   WPfoed(edtptr,TRUE);
/*
***Force an expose on the dialogue to make sure that the new windows
***in the list are displayed immediately.
*/
   WPxpiw(iwinpt);
/*
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_NONE,&but_id);
/*
***Blank/Unblank button.
*/
   for ( i=0; i<stop-start; ++i )
     {
     if ( but_id == blank_id[i] )
       {
       level = sel_set[start + i];
       EXget_level(level,grw_id,&level_status,level_name);
       if ( level_status )
         EXunblank_level(level,grw_id);
       else
         EXblank_level(level,grw_id);

       update_names();
       delete_list();

       winptr = WPwgwp((wpw_id)grw_id);

       if ( winptr->typ == TYP_GWIN ) WPrepaint_GWIN(grw_id);
       else                           WPrepaint_RWIN(grw_id,FALSE);

       goto start;
       }
     }
/*
***Name select.
*/
   if ( but_id == nb_id )
     {
     update_names();
     WPgted(iwin_id,nam_id,namstr);
     mk_selset_name(namstr);
     delete_list();

     start = 0;
     if ( n_selected > list_size ) stop = start + list_size;
     else                          stop = n_selected;

     goto start;
     }
/*
***Range select.
*/
   else if ( but_id == rb_id )
     {
     update_names();
     WPgted(iwin_id,range_id,rangestr);
     switch ( sscanf(rangestr,"%d%*[-]%d",&n1,&n2) )
       {
       case 1:
       mk_selset_range(n1,n1);
       break;

       case 2:
       mk_selset_range(n1,n2);
       break;

       default:
       WPbell();
       goto loop;
       }

     delete_list();

     start = 0;
     if ( n_selected > list_size ) stop = start + list_size;
     else                          stop = n_selected;

     goto start;
     }
/*
***All used select.
*/
   else if ( but_id == used_id )
     {
     update_names();
     mk_selset_used();
     delete_list();
     start = 0;
     if ( n_selected > list_size ) stop = start + list_size;
     else                          stop = n_selected;
     goto start;
     }
/*
***The + button.
*/
   else if ( but_id == plus_id )
     {
     if ( stop < n_selected )
       {
       update_names();
       delete_list();
       start += list_size;
       stop  += list_size;
       if ( stop > n_selected ) stop = n_selected;
       goto start;
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***The - button.
*/
   else if ( but_id == minus_id )
     {
     if ( start > 0 )
       {
       update_names();
       delete_list();
       start -= list_size;
       stop   = start + list_size;
       if ( start < 0 )
         {
         start = 0;
         stop = list_size;
         }
       goto start;
       }
     else
       {
       WPbell();
       goto loop;
       }
     }
/*
***Close button.
*/
   else if ( but_id == close_id )
     {
     update_names();
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
***Other events, we do not care about.
*/
   else
     {
     goto loop;
     }
/*
***Time to exit. Reset actfunc and remember the window position.
*/
exit:
   actfunc = actfunc_org;
   WPgtwp(iwinpt->id.x_id,&wm_x2,&wm_y2);
   iwin_x = iwin_x + wm_x2 - wm_x1;
   iwin_y = iwin_y + wm_y2 - wm_y1;
   WPwdel(iwin_id);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short WPmtsl(
        DBint grw_id,
        int   first,
        int   last,
        int   mode)

/*      Tänder/Släcker/Listar nivåer i ett WPGWIN
 *      eller WPRWIN.
 *
 *      In: grw_id = ID of WPGWIN or WPRWIN
 *          first  = First level to process.
 *          last   = Last level to process.
 *          mode   = WP_BLANKL  => Blank levels.
 *                   WP_UBLANKL => Unblank levels.
 *                   WP_LISTL   => List levels.
 *
 *      Ut: Inget.
 *
 *      Felkod: WP1392 = Fönster med id %s finns ej
 *
 *      (C)microform ab 16/1/95 J. Kjellander
 *
 *      1999-03-22 mode ej deklarerad R. Svedin
 *      2007-01-01 Removed nivtb1[], J.Kjellander
 *      2007-01-13 WPRWIN, J.Kjellander
 *
 ******************************************************!*/

  {
   int     i,start,stop,inc,bytofs,bitmsk,tsnum;
   char    buf[81],errbuf[81];
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Get a C ptr to the window.
*/
   if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL )
     {
     if      ( winptr->typ == TYP_GWIN ) gwinpt = (WPGWIN *)winptr->ptr;
     else if ( winptr->typ == TYP_RWIN ) rwinpt = (WPRWIN *)winptr->ptr;
     }
   else
     {
     sprintf(errbuf,"%d",grw_id);
     return(erpush("WP1392",errbuf));
     }
/*
***Forward or reverse ?
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
***What to do ?
*/ 
   switch ( mode )
     {
/*
***Unblank.
*/
     case WP_UBLANKL:

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = bitmsk<<(i&7);
       if ( winptr->typ == TYP_GWIN ) gwinpt->nivtab[bytofs] |= bitmsk;
       else                           rwinpt->nivtab[bytofs] |= bitmsk;
       }

     break;
/*
***Blank.
*/
     case WP_BLANKL:

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = ~(bitmsk<<(i&7));
       if ( winptr->typ == TYP_GWIN ) gwinpt->nivtab[bytofs] &= bitmsk;
       else                           rwinpt->nivtab[bytofs] &= bitmsk;
       }

     break;
/*
***List.
*/
     case WP_LISTL:
     WPinla("");

     for ( i=start; i!=stop; i += inc )
       {
       bytofs = i>>3;
       bitmsk = 1;
       bitmsk = bitmsk<<(i&7);
       tsnum = 226;
       if ( (winptr->typ == TYP_GWIN)  &&
            (gwinpt->nivtab[bytofs] & bitmsk) ) tsnum = 225;
       if ( (winptr->typ == TYP_RWIN)  &&
            (rwinpt->nivtab[bytofs] & bitmsk) ) tsnum = 225;
       sprintf(buf,"%4d = %-15s",i,IGgtts(tsnum));
       WPalla(buf,(short)1);
       }

     WPexla(TRUE);
     break;
     }

   return(0);
  }

/********************************************************/
/********************************************************/

         bool WPnivt(
unsigned char lvltab[],
         int  level)

/*      Is this level unblanked ?
 *
 *      In: lvltab => Level table.
 *          level  => Level.
 *
 *      Return: TRUE  = Unblanked.
 *              FALSE = Blanked.
 *
 *      (C)2007-01-12, J. Kjellander
 *
 ********************************************************/

 {
   int  bytofs,bitmsk;

   bytofs = level;
   bytofs = bytofs>>3;
   bitmsk = 1;
   bitmsk = bitmsk<<(level&7);
   if ( lvltab[bytofs] & bitmsk ) return(TRUE);
   else                           return(FALSE);
 }

/********************************************************/
/********************************************************/

static  void mk_selset_range(
        int  n1,
        int  n2)

/*      Make a select set from a range between n1 and n2.
 *
 *      In: n1 = First level number
 *          n2 = Last level number
 *
 *      (C)2007-04-01, J. Kjellander
 *
 ********************************************************/

 {
   int i;

/*
***No levels selected yet.
*/
   n_selected = 0;
/*
***Error check.
*/
   if ( n1 < 0 ) n1 = 0;
   if ( n2 < 0 ) n2 = 0;
   if ( n1 > WP_NIVANT-1 ) n1 = WP_NIVANT-1;
   if ( n2 > WP_NIVANT-1 ) n2 = WP_NIVANT-1;
/*
***Create the set.
*/
   if ( n1 > n2 )
     {
     for ( i=n2; i<=n1; ++i )
       {
       sel_set[n_selected] = i;
     ++n_selected;
       }
     }
   else if ( n1 < n2 )
     {
     for ( i=n1; i<=n2; ++i )
       {
       sel_set[n_selected] = i;
     ++n_selected;
       }
     }
   else
     {
     sel_set[0] = n1;
     n_selected = 1;
     }
 }

/********************************************************/
/********************************************************/

static  void mk_selset_name(
        char *wc_name)

/*      Make a select set from a name, optionally
 *      with wildcards.
 *
 *      In: name = C ptr to string.
 *
 *      (C)2007-04-01, J. Kjellander
 *
 ********************************************************/

 {
   int  i;
   bool level_status;
   char level_name[V3STRLEN+1];

/*
***No levels selected yet.
*/
   n_selected = 0;
/*
***Loop through all levels.
*/
   for ( i=0; i<WP_NIVANT; ++i )
     {
     EXget_level(i,0,&level_status,level_name);
     if ( IGcmpw(wc_name,level_name) )
       {
       sel_set[n_selected] = i;
     ++n_selected;
       }
     }
 }

/********************************************************/
/********************************************************/

static  void mk_selset_used()

/*      Make a select set from all levels used.
 *
 *      (C)2007-04-01, J. Kjellander
 *
 ********************************************************/

 {
   int      i;
   bool     used[WP_NIVANT];
   DBetype  type;
   DBptr    la;
   DBHeader hdr;

/*
***No levels selected yet.
*/
   for ( i=0; i<WP_NIVANT; ++i ) used[i] = FALSE;
   n_selected = 0;
/*
***Traverse DB.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     DBread_header(&hdr,la);
     used[hdr.level] = TRUE;
     }
/*
***Create the select set.
*/
   for ( i=0; i<WP_NIVANT; ++i )
     {
     if ( used[i] )
       {
       sel_set[n_selected] = i;
     ++n_selected;
       }
     }
 }

/********************************************************/
/********************************************************/

static  void update_names()

/*      Updates the names of all levels in the currently
 *      displayed list.
 *
 *      (C)2007-04-02, J. Kjellander
 *
 ********************************************************/

 {
   int  i;
   char level_name[V3STRLEN+1];

   for ( i=0; i<stop-start; ++i )
     {
     WPgted(iwin_id,name_id[i],level_name);
     EXname_level(sel_set[start+i],level_name);
     }
 }

/********************************************************/
/********************************************************/

static  void delete_list()

/*      Deletes the buttons and edits in the currently
 *      displayed list.
 *
 *      (C)2007-04-01, J. Kjellander
 *
 ********************************************************/

 {
   int     i,but_id,edit_id;
   WPBUTT *butptr;
   WPEDIT *edtptr;

/*
***Delete the level number, name and status buttons.
*/
   for ( i=0; i<stop-start; ++i )
     {
     but_id = level_id[i];
     butptr = (WPBUTT *)iwinpt->wintab[but_id].ptr;
     XDestroyWindow(xdisp,butptr->id.x_id);
     WPdlbu(butptr);
     iwinpt->wintab[but_id].ptr = NULL;
     iwinpt->wintab[but_id].typ = TYP_UNDEF;

     edit_id = name_id[i];
     edtptr = (WPEDIT *)iwinpt->wintab[edit_id].ptr;
     XDestroyWindow(xdisp,edtptr->id.x_id);
     WPdled(edtptr);
     iwinpt->wintab[edit_id].ptr = NULL;
     iwinpt->wintab[edit_id].typ = TYP_UNDEF;

     but_id = blank_id[i];
     butptr = (WPBUTT *)iwinpt->wintab[but_id].ptr;
     XDestroyWindow(xdisp,butptr->id.x_id);
     WPdlbu(butptr);
     iwinpt->wintab[but_id].ptr = NULL;
     iwinpt->wintab[but_id].typ = TYP_UNDEF;
     }
/*
***Delete active scroll buttons.
*/
   if ( plus_id > 0 )
     {
     butptr = (WPBUTT *)iwinpt->wintab[plus_id].ptr;
     XDestroyWindow(xdisp,butptr->id.x_id);
     WPdlbu(butptr);
     iwinpt->wintab[plus_id].ptr = NULL;
     iwinpt->wintab[plus_id].typ = TYP_UNDEF;
     }

   if ( minus_id > 0 )
     {
     butptr = (WPBUTT *)iwinpt->wintab[minus_id].ptr;
     XDestroyWindow(xdisp,butptr->id.x_id);
     WPdlbu(butptr);
     iwinpt->wintab[minus_id].ptr = NULL;
     iwinpt->wintab[minus_id].typ = TYP_UNDEF;
     }
 }

/********************************************************/
