/**********************************************************************
*
*    wpgetalt.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPialt();            Input True/False alternative
*    WPilse();            Input from list with edit
*    WPselect_partname(); Select a module to be used in a PART statement
*    WPselect_sysmode();  Select system mode during startup
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

extern V3MDAT  sydata;
extern char    svnversion[];

/*********************************************************/

     bool  WPialt(
     char *ps,
     char *ts,
     char *fs,
     bool  pip)

/*   Input alternative (1 char) type y/n, +/- etc.
 *
 *   In: ps  = Prompt string.
 *       ts  = TRUE-string.
 *       fs  = FALSE-string.
 *       pip = Optional beep, Yes/No.
 *
 *   Return: TRUE  if answer = First char in ts
 *           FALSE if answer = First char in fs
 *
 *   (C)microform ab 24/6/92 J. Kjellander
 *
 *   12/12/93 Omarbetad, J. Kjellander
 *   7/11/95  Resurs för titel, J. Kjellander
 *   1996-02-09 Edit och geometry-resurs, J. Kjellander
 *
 *******************************************************!*/

 {
    char     tkn;
    short    x,y,dx,dy,lx,ly,ay,lm;
    char     title[81],ps1[V3STRLEN+1],ps2[V3STRLEN+1];
    DBint    iwin_id,pmt_id,true_id,false_id,but_id,edit_id;
    bool     svar;
    int      px,py,ix,iy,tdx,tdy;
    unsigned int dum1,dum2;
    char    *type[20];
    XrmValue value;

/*
***Optional beep.
*/
    if ( pip ) WPbell();
/*
***Prompt size.
*/
    px = 1.1*WPstrl(ps);
    py = 1.5*WPstrh();
/*
***Calculate dialog size in X-direction.
***A long prompt (more than 80% of display) needs
***splitting.
*/
    lx = WPstrl(" ");
    dx = lx + px + lx;

    if ( dx > 0.8*DisplayWidth(xdisp,xscr) )
      {
      WPdivs(ps,(int)(0.8*DisplayWidth(xdisp,xscr)),&tdx,&tdy,ps1,ps2);
      px = tdx;
      py = tdy;
      dx = lx + px + lx;
      }
    else ps1[0] = '\0';
/*
***Calculate dialog size in Y-direction.
*/
    ly = WPstrh();
    ay = 3.0*WPstrh();
    lm = 0.8*ly;

    dy = ly + py + lm + ay + ly;
/*
***Dialog position.
*/
    ix  = (DisplayWidth(xdisp,xscr)-dx)/2;
    iy  = (DisplayHeight(xdisp,xscr)-dy)/2;

    if ( XrmGetResource(xresDB,"varkon.alternative.geometry",
                               "Varkon.Alternative.Geometry",type,&value) )
      XParseGeometry((char *)value.addr,&ix,&iy,&dum1,&dum2);

    x = (short)ix;
    y = (short)iy;
/*
***Dialog title.
*/
    if ( !WPgrst("varkon.alternative.title",title) )
      strcpy(title,"Alternativ");
/*
***Create WPIWIN.
*/
    WPwciw(x,y,dx,dy,title,&iwin_id);
/*
***Create promt, if ps1 <> "" use two lines.
*/
    x  = lx;
    y  = ly;
    dx = px;
    dy = py;

    if ( ps1[0] == '\0' )
      WPcrpb((wpw_id)iwin_id,x,y,dx,dy,(short)0,
                     ps,"","",WP_BGND1,WP_FGND,&pmt_id);
    else
      {
      WPcrpb((wpw_id)iwin_id,x,y,dx,dy/2,(short)0,
                              ps1,"","",WP_BGND1,WP_FGND,&pmt_id);
      WPcrpb((wpw_id)iwin_id,x,y+dy/2,dx,dy/2,(short)0,
                              ps2,"","",WP_BGND1,WP_FGND,&pmt_id);
      }
/*
***TRUE button.
*/
    x = 0.2*px;
    y  = ly + py + lm;
    if ( WPstrl(ts) > WPstrl(fs) ) dx = 1.5*WPstrl(ts);
    else                           dx = 1.5*WPstrl(fs);
    if ( dx < 6*WPstrl(" ") )      dx = 6*WPstrl(" ");
    dy = ay;

    WPcrpb((wpw_id)iwin_id,x,y,dx,dy,(short)3,
                            ts,ts,"",WP_BGND2,WP_FGND,&true_id);
/*
***FALSE button.
*/
    x  = lx + px  - (short)(0.2*px) - dx;

    WPcrpb((wpw_id)iwin_id,x,y,dx,dy,(short)3,
                            fs,fs,"",WP_BGND2,WP_FGND,&false_id);
/*
***An invisible edit for keyboard input.
*/
    WPmced((wpw_id)iwin_id,1,1,1,1,(short)0,
                   "",1,&edit_id);
/*
***Display.
*/
    WPwshw(iwin_id);
/*
***Wait for action. Service-level for key-event = SLEVEL_ALL
***so that each char may be processed.
*/
loop:
    WPwwtw(iwin_id,SLEVEL_ALL,&but_id);
/*
***Action in TRUE- or FALSE-button ?
*/
    if      ( but_id == true_id  ) svar = TRUE;
    else if ( but_id == false_id ) svar = FALSE;
/*
***Keypress ?
*/
    else if ( but_id == edit_id  )
      {
      WPgted(iwin_id,edit_id,&tkn);
      if       ( tkn == *ts ) svar = TRUE;
      else if ( tkn == *fs ) svar = FALSE;
      else
        {
        WPbell();
        goto loop;
        }
      }
    else goto loop;
/*
***Time to exit.
*/
    WPwdel(iwin_id);
/*
***The end.
*/
    return(svar);
  }

/********************************************************/
/********************************************************/

     short WPilse(
     char *rubrik,
     char *defstr,
     char *strlst[],
     int   actalt,
     int   nstr,
     char *altstr)

/*   Select from a list of strings or input a string
 *   with the keyboard. If actalt > 0 this button is
 *   shown in active state.
 *
 *   In:   rubrik => Window title
 *         defstr => Default text in the  edit
 *         strlst => Array of C ptres to alternative texts
 *         actalt => Active alternative or -1 for none
 *         nstr   => Number of alternatives
 *
 *   Out: *altstr = The alternative text chosen (or entered).
 *
 *   Return:  0 = Ok.
 *       REJECT = Cancel.
 *
 *   (C)microform ab 25/9/95 J. Kjellander
 *
 *   1988-04-01 defstr mm., J.Kjellander
 *   2004-09-03 JNLGTH->31, J.Kjellander, Örebro university
 *   2007-04-21 1.19, J.Kjellander
 *
 *******************************************************!*/

  {
   char    *str,okey[81],okeytt[81],reject[81],rejecttt[81],
            help[81],helptt[81];
   int      i,rad,radant,main_x,main_y,width,height,kol,kolant,
            alt,butlen1,butlen2,ix,iy;
   short    status,main_dx,main_dy,alt_x,alt_y,alth,
            edtlen,ly,lm;
   DBint    iwin_id,alt_id[WP_IWSMAX],but_id,okey_id,help_id,
            reject_id,edit_id;
   unsigned int dum1,dum2;
   char    *type[20];
   XrmValue value;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;

/*
***Window position.
*/
    ix = iy = 20;

    if ( XrmGetResource(xresDB,"varkon.alternative.geometry",
                                 "Varkon.Alternative.Geometry",type,&value) )
      XParseGeometry((char *)value.addr,&ix,&iy,&dum1,&dum2);
/*
***Texts from the ini-file.
*/
   if ( !WPgrst("varkon.input.okey",okey) )               strcpy(okey,"Okej");
   if ( !WPgrst("varkon.input.okey.tooltip",okeytt) )     strcpy(okeytt,"");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Avbryt");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Hjälp");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");
/*
***How long is the longest alternative text (butlen1) ?
*/
   butlen1 = 1;
   for ( i=0; i<nstr; ++i )
     {
     if ( WPstrl(strlst[i]) > butlen1 ) butlen1 = WPstrl(strlst[i]);
     }
   butlen1 *= 1.3;
/*
***How long is the longest of Ok, Reject and Help (butlen2) ?
*/
   butlen2 = 0;
   if ( WPstrl(okey)   > butlen2 ) butlen2 = WPstrl(okey);
   if ( WPstrl(reject) > butlen2 ) butlen2 = WPstrl(reject);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.8;
/*
***Make the edit 15 chars long ?
*/
   edtlen = (short)(15*WPstrl("A")*1.3);
/*
***More geometry.
*/
   ly   = (short)(0.6*WPstrh());
   alth = (short)(1.6*WPstrh()); 
   lm   = (short)(1.0*WPstrh());
/*
***The size of the screen.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);
/*
***Lets plan for at least 3 columns and increase if more
***are needed.
*/
   kolant = 3;

kloop:
   radant = (int)(ceil((nstr/(double)kolant)-0.1));
/*
***Calculate the size of the window.
*/
   main_dx = ly + (kolant-1)*(butlen1 + lm) + butlen1 + ly;
   if ( main_dx < (3*butlen2 + 4*ly) ) main_dx = 3*butlen2 + 4*ly;
   main_dy = ly + radant*(alth + ly) + ly + alth + alth + alth + alth + ly;
/*
***Do we fit in the X-direction ? If not try with fewer
***columns.
*/
   if ( main_dx > width )
     {
   --kolant;
     nstr = radant*kolant + 1;
     strcpy(strlst[nstr-1],"?????");
     goto kloop;
     }
/*
***Try one more column.
*/
   else if ( main_dy > height-30 )
     {
   ++kolant;
     goto kloop;
     }
/*
***Does the title fit ?
*/
   if ( WPstrl(rubrik) >  main_dx-30 ) main_dx = WPstrl(rubrik)+30;
   if ( main_dx > width-30 ) main_dx = width - 30;
/*
***Create the main window, a WPIWIN.
*/
   WPposw(ix,iy,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,rubrik,&iwin_id);
   iwinpt = (WPIWIN *)wpwtab[(wpw_id)iwin_id].ptr;
/*
***Create the alternatives, WPBUTTON.
*/
   for ( rad=0; rad<radant; ++rad )
     {
     alt_y  = ly + rad*(alth+ly);

     for ( kol=0; kol<kolant; ++kol )
       {
       alt_x  = ly + kol*(butlen1+lm);
       alt    = rad*kolant + kol;
       str    = strlst[alt];

       if ( alt < nstr )
         {
         if ( alt == actalt )
           status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
                                   str,str,"",WP_BGND3,WP_FGND,&alt_id[alt]);
         else
           status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
                                   str,str,"",WP_BGND2,WP_FGND,&alt_id[alt]);
         }
       }
     }
/*
***The input field, WPEDIT.
*/
   if ( edtlen > main_dx - 2*ly) edtlen = main_dx - 2* ly;

   alt_x  = (main_dx-edtlen)/2;
   alt_y  = ly + radant*(alth + ly) + ly;
   strcpy(altstr,defstr);
   status = WPmced((wpw_id)iwin_id,alt_x,alt_y,edtlen,alth,(short)1,
                   altstr,JNLGTH,&edit_id);
/*
***A 3D line.
*/
   alt_x = main_dx/8;
   alt_y  = ly + radant*(alth + ly) + ly + alth + alth;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Ok, reject and help.
*/
   alt_x  = ly;
   alt_y  = ly + radant*(alth + ly) + alth + alth + alth;
   alth   = 2*WPstrh();
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   alt_x  = ly + butlen2 + ly;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - ly - butlen2;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Display.
*/
   WPwshw(iwin_id);
/*
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Okey.
*/
   if ( but_id == okey_id )
     {
     WPgted(iwin_id,edit_id,altstr);
     if ( altstr[0] == '\0' )
       {
       XBell(xdisp,100);
       goto loop;
       }
     else goto exit;
     }
/*
***Reject.
*/
   if ( but_id == reject_id ) status = REJECT;
/*
***Help.
*/
   else if ( but_id == help_id )
     {
     IGhelp();
     goto loop;
     }
/*
***Action in the edit. We don't like an empty edit.
*/
   else if ( but_id == edit_id )
     {
     WPgted(iwin_id,edit_id,altstr);
     if ( altstr[0] == '\0' )
       {
       XBell(xdisp,100);
       goto loop;
       }
     else goto exit;
     }
/*
***An alternative selected.
*/
   else
     {
     for ( i=0; i<nstr; ++i )
       {
       if ( but_id == alt_id[i] )
         {
         strcpy(altstr,strlst[i]);
         goto exit;
         }
       }
     goto loop;
     }
/*
***The end.
*/
exit:
   WPwdel(iwin_id);
   XFlush(xdisp);

   return(status);
 }

/********************************************************/
/********************************************************/

     short WPselect_partname(
     int   source,
     char *namlst[],
     int   nstr,
     char *name)

/*    This dialogue is used by IG to let the user select
 *    a module to use in creating a PART statement.
 *
 *   In:   source => Initial source, 1=jobdir, 2=jobdir/lib
 *         namlst => Array of C ptrs to part names
 *         nstr   => Number of names
 *
 *   Out: *name = The name of the selected module
 *
 *   Return:  1 = Switch to jobdir
 *            2 = Switch to jobdir/lib
 *            3 = Too many modules
 *            0 = Ok.
 *       REJECT = Cancel.
 *
 *   (C)2007-04-21 J. Kjellander
 *
 *******************************************************!*/

 {
   char    *str,reject[81],rejecttt[81],help[81],helptt[81],title[81],
            job[81],jobtt[81],lib[81],libtt[81],empty[81];
   int      i,rad,radant,main_x,main_y,width,height,kol,kolant,
            alt,butlen1,butlen2,butlen3,ix,iy,alth,ly,lm,maxcols,maxlines;
   short    status,main_dx,main_dy,alt_x,alt_y;
   DBint    iwin_id,alt_id[WP_IWSMAX],job_id,lib_id,but_id,help_id,reject_id;
   unsigned int dum1,dum2;
   char    *type[20];
   XrmValue value;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;

/*
***Window position.
*/
   ix = iy = 20;

   if ( XrmGetResource(xresDB,"varkon.partname.geometry",
                               "Varkon.Partname.Geometry",type,&value) )
     XParseGeometry((char *)value.addr,&ix,&iy,&dum1,&dum2);
/*
***Texts from the ini-file.
*/
   if ( !WPgrst("varkon.partname.title",title) )          strcpy(title,"Part");
   if ( !WPgrst("varkon.partname.jobdir",job) )           strcpy(job,"Jobdir");
   if ( !WPgrst("varkon.partname.jobdir.tooltip",jobtt) ) strcpy(jobtt,"");
   if ( !WPgrst("varkon.partname.libdir",lib) )           strcpy(lib,"Library");
   if ( !WPgrst("varkon.partname.libdir.tooltip",libtt) ) strcpy(libtt,"");
   if ( !WPgrst("varkon.partname.empty",empty) )          strcpy(empty,"Empty");
   if ( !WPgrst("varkon.input.reject",reject) )           strcpy(reject,"Reject");
   if ( !WPgrst("varkon.input.reject.tooltip",rejecttt) ) strcpy(rejecttt,"");
   if ( !WPgrst("varkon.input.help",help) )               strcpy(help,"Help");
   if ( !WPgrst("varkon.input.help.tooltip",helptt) )     strcpy(helptt,"");
/*
***How long is the longest alternative text (butlen1) ?
*/
   butlen1 = 1;
   for ( i=0; i<nstr; ++i )
     {
     if ( WPstrl(namlst[i]) > butlen1 ) butlen1 = WPstrl(namlst[i]);
     }
   butlen1 *= 1.3;
/*
***How long is the longest of Reject and Help (butlen2) ?
*/
   butlen2 = 0;
   if ( WPstrl(reject) > butlen2 ) butlen2 = WPstrl(reject);
   if ( WPstrl(help)   > butlen2 ) butlen2 = WPstrl(help);
   butlen2 *= 1.8;
/*
***How long is the longest of job and lib (butlen3) ?
*/
   butlen3 = 0;
   if ( WPstrl(job) > butlen3 ) butlen3 = WPstrl(job);
   if ( WPstrl(lib) > butlen3 ) butlen3 = WPstrl(lib);
   butlen3 *= 1.3;
/*
***More geometry.
*/
   ly   = 0.5*WPstrh();
   alth = 1.6*WPstrh(); 
   lm   = 1.0*WPstrh();
/*
***The size of the screen.
*/
   width  = DisplayWidth(xdisp,xscr);
   height = DisplayHeight(xdisp,xscr);
/*
***What is the maximum number of columns and lines possible ?
*/
   maxcols = (width - 2*ly - 20)/(butlen1 + lm);
   maxlines = (height - alth - 2*ly - alth - 2*ly - 60)/(alth + ly);
/*
***Calculate the actual number of columns and lines
***needed.
*/
   kolant = nstr;
   if ( nstr > 3 ) kolant = 3;
   if ( kolant < 1 ) kolant = 1;

kloop:
   radant = (int)(ceil((nstr/(double)kolant)-0.1));

   if ( radant > maxlines )
     {
     if ( kolant < maxcols )
       {
     ++kolant;
       goto kloop;
       }
     else
       {
       return(3);
       }
     }
/*
***Calculate the size of the window.
*/
   main_dx = ly + (kolant-1)*(butlen1 + lm) + butlen1 + ly;
   if ( main_dx < (2*butlen2 + 3*ly) ) main_dx = 2*butlen2 + 3*ly;
   if ( main_dx < (2*butlen3) ) main_dx = 2*butlen3;
   butlen3 = main_dx/2;

   main_dy = alth + ly + ly +                   /* Source buttons */
             radant*(alth + ly) + ly +          /* Name buttons */
             ly +                               /* Line */
             alth + ly + ly;                    /* Cancel and Help */

   if ( radant == 0 ) main_dy += alth+ly;      /* The "empty"-notice */
/*
***Does the title fit ?
*/
   if ( WPstrl(title) >  main_dx-30 ) main_dx = WPstrl(title)+30;
   if ( main_dx > width-30 ) main_dx = width - 30;
/*
***Create the main window, a WPIWIN.
*/
   WPposw(ix,iy,main_dx+10,main_dy+25,&main_x,&main_y);
   WPwciw((short)main_x,(short)main_y,main_dx,main_dy,title,&iwin_id);
   iwinpt = (WPIWIN *)wpwtab[(wpw_id)iwin_id].ptr;
/*
***Create the source buttons.
*/
   alt_x = 0;
   alt_y = 0;

   if ( source == 1 )
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                job,job,"",WP_BGND3,WP_FGND,&job_id);
   else
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                job,job,"",WP_BGND2,WP_FGND,&job_id);

   butptr = (WPBUTT *)iwinpt->wintab[job_id].ptr;
   strcpy(butptr->tt_str,jobtt);

   alt_x += butlen3;

   if ( source == 2 )
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                lib,lib,"",WP_BGND3,WP_FGND,&lib_id);
   else
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                lib,lib,"",WP_BGND2,WP_FGND,&lib_id);

   butptr = (WPBUTT *)iwinpt->wintab[lib_id].ptr;
   strcpy(butptr->tt_str,libtt);
/*
***Create the alternatives, WPBUTTON.
*/
   alt_y += ly;

   if ( nstr == 0 )
     {
     alt_x = (main_dx - WPstrl(empty))/2;
     alt_y += alth + ly;
     WPcrpb((wpw_id)iwin_id,alt_x,alt_y,WPstrl(empty),alth,(short)0,
                                 empty,empty,"",WP_BGND1,WP_FGND,&but_id);
     }

   else for ( rad=0; rad<radant; ++rad )
     {
     alt_y += alth + ly;

     for ( kol=0; kol<kolant; ++kol )
       {
       alt_x  = ly + kol*(butlen1+lm);
       alt    = rad*kolant + kol;
       str    = namlst[alt];

       if ( alt < nstr )
         {
         WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
                                 str,str,"",WP_BGND2,WP_FGND,&alt_id[alt]);
         }
       }
     }
/*
***A 3D line.
*/
   alt_x  = main_dx/8;
   alt_y += alth + ly + ly;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,7*main_dx/8,alt_y);
/*
***Reject and help.
*/
   alt_x   = ly;
   alt_y  += ly;
   alth   = 2*WPstrh();
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - ly - butlen2;
   status = WPcrpb((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           help,help,"",WP_BGND2,WP_FGND,&help_id);
   butptr = (WPBUTT *)iwinpt->wintab[help_id].ptr;
   strcpy(butptr->tt_str,helptt);
/*
***Display.
*/
   WPwshw(iwin_id);
/*
***Wait for action.
*/
   status = 0;
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Jobdir.
*/
   if ( but_id == job_id )
     {
     status = 1;
     goto exit;
     }
/*
***Jobdir.
*/
   else if ( but_id == lib_id )
     {
     status = 2;
     goto exit;
     }
/*
***Reject.
*/
   else if ( but_id == reject_id )
     {
     status = REJECT;
     goto exit;
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
***A partname selected.
*/
   else
     {
     for ( i=0; i<nstr; ++i )
       {
       if ( but_id == alt_id[i] )
         {
         strcpy(name,namlst[i]);
         goto exit;
         }
       }
     goto loop;
     }
/*
***The end.
*/
exit:
   WPwdel(iwin_id);
   XFlush(xdisp);

   return(status);
 }

/********************************************************/
/*********************************************************/

     short WPselect_sysmode(int *mode)

/*   Select system mode during startup.
 *
 *   Out: *mode = GENERIC or EXPLICIT
 *
 *   Return: 0      = Ok.
 *           REJECT = Exit.
 *          -2      = Error from the help system
 *
 *   (C)2007-10-14 J. Kjellander
 *
 *******************************************************!*/

 {
   char     expl[81],gen[81],exit[81],learn[81],line1[81],
            line2[81],line3[81],line4[81];
   short    status,tlen,air1,air2,bh1,bh2,iwin_x,iwin_y,main_dx,
            main_dy,alt_x,alt_y;
   DBint    iwin_id,expl_id,gen_id,exit_id,learn_id,but_id,dum_id;
   WPWIN   *winptr;
   WPIWIN  *iwinpt;
   WPBUTT  *butptr;

/*
***Line 1 = Welcome to VARKON
***Line 2 = version X.X.X svn#X
***Line 3 = an open source CAD-tool
*/
   strcpy(line1,"Welcome to VARKON");
   sprintf(line2,"version %d.%d%c svn#%s",(int)sydata.vernr,
                 (int)sydata.revnr,sydata.level,svnversion);
   strcpy(line3,"an open source CAD-tool");
/*
***Line 4 = What would you like to do next ?
***Explicit-, generic-, exit- and learn-texts.
*/
   if ( !WPgrst("varkon.mode.what",line4) )      strcpy(line4,"What next ?");
   if ( !WPgrst("varkon.mode.explicit",expl) )   strcpy(expl,"Explicit");
   if ( !WPgrst("varkon.mode.generic",gen) )     strcpy(gen,"Generic");
   if ( !WPgrst("varkon.mode.learnmore",learn) ) strcpy(learn,"Help");
   if ( !WPgrst("varkon.mode.exit",exit) )       strcpy(exit,"Exit");
/*
***Length and heights of texts.
*/
   WPsfnt(WP_FNTBIG);
   bh1  = WPstrh();
   tlen = WPstrl(line1);
   if ( WPstrl(line4) > tlen ) tlen = WPstrl(line4);

   WPsfnt(WP_FNTNORMAL);
   bh2 =  WPstrh();
   if ( WPstrl(line2) > tlen ) tlen = WPstrl(line2);
   if ( WPstrl(line3) > tlen ) tlen = WPstrl(line3);
   if ( WPstrl(expl)  > tlen ) tlen = WPstrl(expl);
   if ( WPstrl(gen)   > tlen ) tlen = WPstrl(gen);
   if ( WPstrl(exit)  > tlen ) tlen = WPstrl(exit);
   if ( WPstrl(learn) > tlen ) tlen = WPstrl(learn);
/*
***Calculate outside air (air1) and air between (air2).
*/
   air1 = 1.3*bh1;
   air2 = 0.5*bh2;
/*
***Window position.
*/
   iwin_x = 200;
   iwin_y = 200;
/*
***Calculate the window size in X-direction.
*/
   main_dx = air1 + air1 + tlen + air1;
/*
***Calculate the window size in Y-direction.
*/
   main_dy = air1 + bh1 + air2 + bh2 + air2 + bh2 + air1 +
             + air1 + bh1 + air1 + bh2 + air2 + bh2 + air2 + bh2 +
             air2 + bh2 + air1;
/*
***Create the dialogue window as a WPIWIN.
*/
   WPwciw(iwin_x,iwin_y,main_dx,main_dy,"Select mode",&iwin_id);
/*
***Get a C-ptr to the WPIWIN.
*/
   winptr = WPwgwp((wpw_id)iwin_id);
   iwinpt = (WPIWIN *)winptr->ptr;
/*
***Line 1.
*/
   alt_x = air1;
   alt_y = air1;
   WPcrlb(iwin_id,alt_x,alt_y,tlen,bh1,line1,&dum_id);
   butptr = (WPBUTT *)iwinpt->wintab[dum_id].ptr;
   butptr->font = WP_FNTBIG;
/*
***Line 2.
*/
   alt_x  = air1;
   alt_y += bh1 + air2;
   WPcrlb(iwin_id,alt_x,alt_y,tlen,bh1,line2,&dum_id);
/*
***Line 3.
*/
   alt_x  = air1;
   alt_y += bh2 + air2;
   WPcrlb(iwin_id,alt_x,alt_y,tlen,bh1,line3,&dum_id);
/*
***A 3D line.
*/
   alt_x = (main_dx - tlen)/2;
   alt_y += bh2 + air1;
   WPcreate_3Dline(iwin_id,alt_x,alt_y,alt_x+tlen,alt_y);
/*
***Line 4.
*/
   alt_x  = air1;
   alt_y += bh2 + air1;
   WPcrlb(iwin_id,alt_x,alt_y,tlen,bh1,line4,&dum_id);
   butptr = (WPBUTT *)iwinpt->wintab[dum_id].ptr;
   butptr->font = WP_FNTBIG;
/*
***Explicit.
*/
   alt_x += air1;
   alt_y += bh2 + air1;
   WPcrtb((wpw_id)iwin_id,alt_x,alt_y,expl,&expl_id);
/*
***Generic.
*/
   alt_y += bh2 + air2;
   WPcrtb((wpw_id)iwin_id,alt_x,alt_y,gen,&gen_id);
/*
***Learn more.
*/
   alt_y += bh2 + air2;
   WPcrtb((wpw_id)iwin_id,alt_x,alt_y,learn,&learn_id);
/*
***Exit.
*/
   alt_y += bh2 + air2;
   WPcrtb((wpw_id)iwin_id,alt_x,alt_y,exit,&exit_id);
/*
***Show the dialogue and wait for action.
*/
   WPwshw(iwin_id);
/*
***Wait for action.
*/
loop:
   WPwwtw(iwin_id,SLEVEL_V3_INP,&but_id);
/*
***Explicit button.
*/
   if ( but_id == expl_id )
     {
    *mode = EXPLICIT;
     status = 0;
     goto exit;
     }
/*
***Generic button.
*/
   else if ( but_id == gen_id )
     {
    *mode = GENERIC;
     status = 0;
     goto exit;
     }
/*
***Exit button.
*/
   else if ( but_id == exit_id )
     {
     status = REJECT;
     goto exit;
     }
/*
***Learn more button (help). Negative status from the help
***system is a serious indication that something is mis-
***configured.
*/
   else if ( but_id == learn_id )
     {
     if ( (status=IGhelp()) < 0 ) goto exit;
     else                         goto loop;
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
***Time to exit.
*/
exit:
   WPwdel(iwin_id);
/*
***The end.
*/
   return(status);
  }

/********************************************************/
