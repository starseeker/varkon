/**********************************************************************
*
*    wpgetalt.c
*    ==========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPialt();             Input alternative
*    WPilse();             Input from list with edit
*    WPselect_partname(); Select a module to be used in a PART statement
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

/*********************************************************/

     bool WPialt(
     char *ps,
     char *ts,
     char *fs,
     bool  pip)

/*   Läser in alternativ (1 tecken) typ j/n, +/- etc.
 *
 *   In: ps  = Promptsträng
 *       ts  = TRUE-sträng
 *       fs  = FALSE-sträng
 *       pip = Inledande pip, Ja/Nej
 *
 *   Ut: Inget.
 *
 *   FV: TRUE  om svar = 1:a tecknet i tsnum
 *       FALSE om svar = 1:a tecknet i fsnum
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
    short    status,x,y,dx,dy,lx,ly,px,py,ay,lm;
    char     title[81],ps1[V3STRLEN+1],ps2[V3STRLEN+1];
    DBint    iwin_id,pmt_id,true_id,false_id,but_id,edit_id;
    bool     svar;
    int      ix,iy,tdx,tdy;
    unsigned int dum1,dum2;
    char    *type[20];
    XrmValue value;

/*
***Ev. inledande pip.
*/
    if ( pip ) XBell(xdisp,100);
/*
***För att nu kunna bestämma storleken på själva
***alternativ-fönstret utgår vi från promt-strängen,
***och beräknar promt-fönstrets storlek. En lång promt
***kan behöva delas i 2 rader. Max storlek i X-led är
***80% av skärmen. Blir fönstret större delar vi.
*/
    px = (short)(1.1*WPstrl(ps));
    py = (short)(1.5*WPstrh());
/*
***Vi utgår från att promten är det som tar mest
***plats i X-led och beräknar alternativfönstrets längd
***därefter.
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
    else
      ps1[0] = '\0';
/*
***Beräkna luft yttre, knapparnas höjd, luft mellan och
***huvudfönstrets höjd.
*/  
    ly = WPstrh();
    ay = (short)(3.0*WPstrh()); 
    lm = (short)(0.8*ly);

    dy = ly + py + lm + ay + ly;  
/*
***Alternativfönstrets läge.
*/
    ix  = (DisplayWidth(xdisp,xscr)-dx)/2;
    iy  = (DisplayHeight(xdisp,xscr)-dy)/2;

    if ( XrmGetResource(xresDB,"varkon.alternative.geometry",
                               "Varkon.Alternative.Geometry",type,&value) )
      XParseGeometry((char *)value.addr,&ix,&iy,&dum1,&dum2);

    x = (short)ix;
    y = (short)iy;
/*
***Fönstertitel.
*/
    if ( !WPgrst("varkon.alternative.title",title) )
      strcpy(title,"Alternativ");
/*
***Skapa själva alternativfönstret som ett WPIWIN.
*/
    status = WPwciw(x,y,dx,dy,title,&iwin_id);
/*
***Skapa promt, om ps1 <> "" krävs 2 rader. 
*/
    x  = lx;
    y  = ly;
    dx = px;
    dy = py;

    if ( ps1[0] == '\0' )
      status = WPmcbu((wpw_id)iwin_id,x,y,dx,dy,(short)0,
                              ps,"","",WP_BGND1,WP_FGND,&pmt_id);
    else
      {
      status = WPmcbu((wpw_id)iwin_id,x,y,dx,dy/2,(short)0,
                              ps1,"","",WP_BGND1,WP_FGND,&pmt_id);
      status = WPmcbu((wpw_id)iwin_id,x,y+dy/2,dx,dy/2,(short)0,
                              ps2,"","",WP_BGND1,WP_FGND,&pmt_id);
      }
/*
***Skapa true-knapp.
*/
    x = 0.2*px;
    y  = ly + py + lm;
    if ( WPstrl(ts) > WPstrl(fs) ) dx = 1.5*WPstrl(ts);
    else                           dx = 1.5*WPstrl(fs);
    if ( dx < 6*WPstrl(" ") )      dx = 6*WPstrl(" ");
    dy = ay;

    status = WPmcbu((wpw_id)iwin_id,x,y,dx,dy,(short)3,
                            ts,ts,"",WP_BGND2,WP_FGND,&true_id);
/*
***Skapa FALSE-knapp.
*/
    x  = lx + px  - (short)(0.2*px) - dx;

    status = WPmcbu((wpw_id)iwin_id,x,y,dx,dy,(short)3,
                            fs,fs,"",WP_BGND2,WP_FGND,&false_id);
/*
***En oändligt liten ramlös edit för att kunna läsa tecken.
*/
   status = WPmced((wpw_id)iwin_id,1,1,1,1,(short)0,
                   "",1,&edit_id);
/*
***Klart för visning.
*/
    WPwshw(iwin_id);
/*
***Vänta på action. Service-nivå för key-event = SLEVEL_ALL
***så att vi kan ta hand om varje tecken.
*/
loop:
    status = WPwwtw(iwin_id,SLEVEL_ALL,&but_id);
/*
***Har det hänt nåt i TRUE- eller FALSE-fönstret ?
*/
    if      ( but_id == true_id  ) svar = TRUE;
    else if ( but_id == false_id ) svar = FALSE;
/*
***Eller i det lilla osynliga edit-fönstret ?
*/
    else if ( but_id == edit_id  )
      {
      WPgted(iwin_id,edit_id,&tkn);
      if      ( tkn == *ts ) svar = TRUE;
      else if ( tkn == *fs ) svar = FALSE;
      else
        {
        XBell(xdisp,100);
        goto loop;
        }
      }
    else goto loop;
/*
***Dags att sluta.
*/
    status = WPwdel(iwin_id);

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
 *   with the keyboard. I actalt > 0 this button is
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
           status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
                                   str,str,"",WP_BGND3,WP_FGND,&alt_id[alt]);
         else
           status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
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
   WPmcbu(iwin_id,alt_x,alt_y-1,6*main_dx/8,1,(short)0,"","","",WP_BOTS,WP_BOTS,&but_id);
   WPmcbu(iwin_id,alt_x,alt_y  ,6*main_dx/8,1,(short)0,"","","",WP_TOPS,WP_TOPS,&but_id);
/*
***Ok, reject and help.
*/
   alt_x  = ly;
   alt_y  = ly + radant*(alth + ly) + alth + alth + alth;
   alth   = 2*WPstrh();
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           okey,okey,"",WP_BGND2,WP_FGND,&okey_id);
   butptr = (WPBUTT *)iwinpt->wintab[okey_id].ptr;
   strcpy(butptr->tt_str,okeytt);

   alt_x  = ly + butlen2 + ly;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - ly - butlen2;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
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
 *   In:   source => Initial source, 1=jobdir, 2=mbodir
 *         namlst => Array of C ptrs to part names
 *         nstr   => Number of names
 *
 *   Out: *name = The name of the selected module
 *
 *   Return:  1 = Switch to jobdir
 *            2 = Switch to mbodir
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
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                job,job,"",WP_BGND3,WP_FGND,&job_id);
   else
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                job,job,"",WP_BGND2,WP_FGND,&job_id);

   butptr = (WPBUTT *)iwinpt->wintab[job_id].ptr;
   strcpy(butptr->tt_str,jobtt);

   alt_x += butlen3;

   if ( source == 2 )
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
                                lib,lib,"",WP_BGND3,WP_FGND,&lib_id);
   else
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen3,alth,(short)1,
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
     WPmcbu((wpw_id)iwin_id,alt_x,alt_y,WPstrl(empty),alth,(short)0,
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
         WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen1,alth,(short)1,
                                 str,str,"",WP_BGND2,WP_FGND,&alt_id[alt]);
         }
       }
     }
/*
***A 3D line.
*/
   alt_x  = main_dx/8;
   alt_y += alth + ly + ly;
   WPmcbu(iwin_id,alt_x,alt_y-1,6*main_dx/8,1,(short)0,"","","",WP_BOTS,WP_BOTS,&but_id);
   WPmcbu(iwin_id,alt_x,alt_y  ,6*main_dx/8,1,(short)0,"","","",WP_TOPS,WP_TOPS,&but_id);
/*
***Reject and help.
*/
   alt_x   = ly;
   alt_y  += ly;
   alth   = 2*WPstrh();
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
                           reject,reject,"",WP_BGND2,WP_FGND,&reject_id);
   butptr = (WPBUTT *)iwinpt->wintab[reject_id].ptr;
   strcpy(butptr->tt_str,rejecttt);

   alt_x  = main_dx - ly - butlen2;
   status = WPmcbu((wpw_id)iwin_id,alt_x,alt_y,butlen2,alth,(short)2,
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
